#include "chams.h"
#include "../hooks/vmt/vhooks.h"
#include <fstream>
#include "../../gui/config/config.h"
#include "animations.h"

using on_render_exec = void( __thiscall* ) ( IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );

chams::chams( ) {

	static bool once = false;

	if ( !once )
	{
		once = true;

		std::ofstream( "csgo/materials/glowOverlay.vmt" ) << R"#("VertexLitGeneric" {
		"$additive" "1"
		"$envmap" "models/effects/cube_white"
		"$envmaptint" "[1 1 1]"
		"$envmapfresnel" "1"
		"$envmapfresnelminmaxexp" "[0 1 2]"
		"$alpha" "0.8"
		})#";

		this->material_default = g_csgo.materialsystem( )->FindMaterial( "debug/debugambientcube", TEXTURE_GROUP_MODEL );
		this->material_flat = g_csgo.materialsystem( )->FindMaterial( "debug/debugdrawflat", TEXTURE_GROUP_MODEL );
		this->material_glow = g_csgo.materialsystem( )->FindMaterial( "glowOverlay", TEXTURE_GROUP_MODEL );
	}
}

chams::~chams( ) { }

void chams::override_material( bool ignoreZ, bool use_env, const Color& rgba, IMaterial* material, float alpha ) {

	material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, ignoreZ );
	material->IncrementReferenceCount( );

	if ( alpha == 255 )
		material->AlphaModulate( 255.0f / alpha );
	else
		material->AlphaModulate( alpha * 0.01f );

	if ( !use_env )
		material->ColorModulate( rgba.red / 255.0f, rgba.green / 255.0f, rgba.blue / 255.0f );
	else
	{
		bool bFound = false;
		IMaterialVar* pMatVar = material->FindVar( "$envmaptint", &bFound );
		if ( bFound )
			( *( void( __thiscall** )( int, float, float, float ) )( *( DWORD* )pMatVar + 0x2c ) )( ( uintptr_t )pMatVar, rgba.red / 255.0f, rgba.green / 255.0f, rgba.blue / 255.0f );
	}

	g_csgo.modelrender( )->ForcedMaterialOverride( material );
}

void chams::on_render( IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pinfo, matrix3x4_t* matrix )
{
	static auto ofunc = hooks::modelrender.get_original< on_render_exec >( hooks::indexes::m_draw_model );

	auto entity = reinterpret_cast< m_entity* > ( g_csgo.entitylist( )->GetClientEntity( pinfo.entity_index ) );
	const char* mdl_name = g_csgo.modelinfo( )->GetModelName( ( model_t* )pinfo.pModel );
	bool is_player = strstr( mdl_name, "models/player" ) != nullptr;

	if ( !entity || !entity->is_alive( ) || entity->IsDormant( ) || !matrix || !csgo::m_local )
		return ofunc( g_csgo.modelrender( ), ctx, state, pinfo, matrix );

	Color m_entity_vis = Color( cfg.chams_enemy.chams_r, cfg.chams_enemy.chams_g, cfg.chams_enemy.chams_b );
	Color m_entity_xqz = Color( cfg.chams_enemy.chams_xqz_r, cfg.chams_enemy.chams_xqz_g, cfg.chams_enemy.chams_xqz_b );
	Color m_entity_ill = Color( cfg.chams_enemy.illuminate_r, cfg.chams_enemy.illuminate_g, cfg.chams_enemy.illuminate_b );

	Color m_local = Color( cfg.chams_local.chams_r, cfg.chams_local.chams_g, cfg.chams_local.chams_b );
	Color m_local_ill = Color( cfg.chams_local.illuminate_r, cfg.chams_local.illuminate_g, cfg.chams_local.illuminate_b );

	Color m_fake = Color( cfg.chams_desync.chams_r, cfg.chams_desync.chams_g, cfg.chams_desync.chams_b );
	Color m_fake_ill = Color( cfg.chams_desync.illuminate_r, cfg.chams_desync.illuminate_g, cfg.chams_desync.illuminate_b );

	if ( is_player && entity->is_player( ) && entity == csgo::m_local )
	{
		if ( cfg.chams_desync.chams )
		{
			for ( auto& i : animations::get( ).fake_matrix )
			{
				i[ 0 ][ 3 ] += pinfo.origin.x;
				i[ 1 ][ 3 ] += pinfo.origin.y;
				i[ 2 ][ 3 ] += pinfo.origin.z;
			}

			override_material( false, false, m_fake, cfg.chams_desync.flat ? this->material_flat : this->material_default, cfg.chams_desync.chams_a );
			ofunc( g_csgo.modelrender( ), ctx, state, pinfo, animations::get( ).fake_matrix );

			if ( cfg.chams_desync.illuminate )
			{
				override_material( false, true, m_fake_ill, this->material_glow, cfg.chams_desync.illuminate_a );
				ofunc( g_csgo.modelrender( ), ctx, state, pinfo, animations::get( ).fake_matrix );
			}

			g_csgo.modelrender( )->ForcedMaterialOverride( nullptr );

			for ( auto& i : animations::get( ).fake_matrix )
			{
				i[ 0 ][ 3 ] -= pinfo.origin.x;
				i[ 1 ][ 3 ] -= pinfo.origin.y;
				i[ 2 ][ 3 ] -= pinfo.origin.z;
			}
		}
	}

	if ( is_player && entity->is_player( ) && entity->m_iTeamNum( ) != csgo::m_local->m_iTeamNum( ) && entity != csgo::m_local )
	{
		if ( cfg.chams_enemy.chams )
		{
			if ( cfg.chams_enemy.chams_xqz )
			{
				override_material( true, false, m_entity_xqz, cfg.chams_enemy.flat ? this->material_flat : this->material_default, cfg.chams_enemy.chams_xqz_a );
				ofunc( g_csgo.modelrender( ), ctx, state, pinfo, matrix );
			}

			override_material( false, false, m_entity_vis, cfg.chams_enemy.flat ? this->material_flat : this->material_default, cfg.chams_enemy.chams_a );
			ofunc( g_csgo.modelrender( ), ctx, state, pinfo, matrix );

			if ( cfg.chams_enemy.illuminate )
			{
				override_material( cfg.chams_enemy.chams_xqz, true, m_entity_ill, this->material_glow, cfg.chams_enemy.illuminate_a );
				ofunc( g_csgo.modelrender( ), ctx, state, pinfo, matrix );
			}
		}
	}

	if ( is_player && entity->is_player( ) && entity == csgo::m_local )
	{
		if ( cfg.chams_local.chams )
		{
			override_material( false, false, m_local, cfg.chams_local.flat ? this->material_flat : this->material_default, cfg.chams_local.chams_a );
			ofunc( g_csgo.modelrender( ), ctx, state, pinfo, matrix );

			if ( cfg.chams_local.illuminate )
			{
				override_material( false, true, m_local_ill, this->material_glow, cfg.chams_local.illuminate_a );
				ofunc( g_csgo.modelrender( ), ctx, state, pinfo, matrix );
			}
		}
	}
}
#include "visuals.h"
#include "../../utils/utl.h"

#include "../sdk/entity/entity.h"
#include "../../gui/config/config.h"

#include "../../utils/math.h"
#include "../globals.h"

#include "autowall.h"
#include "../sdk/other/e_nums.h"

float dormant_alpha[ 1024 ];
float stored_curtime[ 1024 ];

void visuals::init( )
{
	if ( !cfg.esp_enabled )
		return;

	for ( int i = 1; i < g_csgo.global_vars( )->m_maxclients; ++i )
	{
		m_entity* e = reinterpret_cast< m_entity* > ( g_csgo.entitylist()->GetClientEntity( i ) );

		if ( !e->is_valid_player( !cfg.esp_dormant ) )
			continue;

		if ( !e->IsDormant( ) )
			stored_curtime[ e->EntIndex( ) ] = g_csgo.global_vars()->m_curtime;

		if ( e->IsDormant( ) && dormant_alpha[ e->EntIndex( ) ] > 0 && g_csgo.global_vars( )->m_curtime - stored_curtime[ e->EntIndex( ) ] > 2 )
			dormant_alpha[ e->EntIndex( ) ] -= 5;

		else if ( dormant_alpha[ e->EntIndex( ) ] < 255 && !( e->IsDormant( ) ) )
			dormant_alpha[ e->EntIndex( ) ] += 5;

		if ( dormant_alpha[ e->EntIndex( ) ] <= 10 )
			continue;

		this->bound_box( e );

		if ( bbox.bottom == 0 )
			continue;

		if ( cfg.esp_box )
			this->box( e );

		if ( cfg.esp_hp )
			this->hp( e );

		if ( cfg.esp_ammo )
			this->ammo( e );

		if ( cfg.esp_name )
			this->name( e );

		if ( cfg.esp_skeleton )
			this->skeleton( e );

		if ( cfg.esp_weapon )
			this->weapon( e );

		if ( cfg.esp_weapon_icon )
			this->weapon_icon( e );

		
	}
}

void visuals::bound_box( m_entity* e )
{
	bbox.bottom = -1;
	bbox.top = -1;
	bbox.left = -1;
	bbox.right = -1;

	vec2_t w2sBottom, w2sTop;

	utils::world_to_screen( e->m_vecOrigin( ) - Vector( 0, 0, 8 ), w2sBottom );
	utils::world_to_screen( e->hitbox_position( 0 ) + Vector( 0, 0, 10 ), w2sTop );

	int Middle = w2sBottom.y - w2sTop.y;
	int Width = Middle / 3.15f;

	bbox.bottom = Middle;
	bbox.top = w2sTop.y;
	bbox.left = w2sBottom.x - Width;
	bbox.right = Width * 2;
}

void visuals::box( m_entity* e )
{
	Color esp = e->IsDormant( ) ? Color( 255, 255, 255, dormant_alpha[ e->EntIndex( ) ] ) : Color( cfg.colors.box_r, cfg.colors.box_g, cfg.colors.box_b, dormant_alpha[ e->EntIndex( ) ] );
	Color esp2 = e->IsDormant( ) ? Color( 0, 0, 0, dormant_alpha[ e->EntIndex( ) ] * 0.85 ) : Color( 0, 0, 0, dormant_alpha[ e->EntIndex( ) ] * 0.85 );

	surface::get( ).outlined_rect( bbox.left, bbox.top, bbox.right, bbox.bottom, esp );
	surface::get( ).outlined_rect( bbox.left + 1, bbox.top + 1, bbox.right - 2, bbox.bottom - 2, esp2 );
	surface::get( ).outlined_rect( bbox.left - 1, bbox.top - 1, bbox.right + 2, bbox.bottom + 2, esp2 );
}

void visuals::hp( m_entity* e )
{
	Color esp = e->IsDormant( ) ? Color( 255, 255, 255, dormant_alpha[ e->EntIndex( ) ] ) : Color( cfg.colors.hp_r, cfg.colors.hp_g, cfg.colors.hp_b, dormant_alpha[ e->EntIndex( ) ] );
	Color esp2 = e->IsDormant( ) ? Color( 0, 0, 0, dormant_alpha[ e->EntIndex( ) ] * 0.85 ) : Color( 0, 0, 0, dormant_alpha[ e->EntIndex( ) ] * 0.85 );

	surface::get( ).filled_rect( bbox.left - 7, bbox.top - 1, 4, bbox.bottom + 2, esp2 );

	int pixelValue = e->m_iHealth( ) * bbox.bottom / 100;
	surface::get( ).filled_rect( bbox.left - 6, bbox.top + bbox.bottom - pixelValue, 2, pixelValue, esp );

	std::string Health = std::to_string( e->m_iHealth( ) );

	if ( e->m_iHealth( ) < 100 )
		surface::get( ).draw_text( bbox.left - 9, bbox.top + bbox.bottom - pixelValue - 5, Color( 255, 255, 255, dormant_alpha[ e->EntIndex( ) ] * 0.85 ), surface::get( ).fonts.esp_other, false, Health.c_str( ) );
}

void visuals::ammo( m_entity* e )
{
	Color esp = e->IsDormant( ) ? Color( 255, 255, 255, dormant_alpha[ e->EntIndex( ) ] ) : Color( cfg.colors.ammo_r, cfg.colors.ammo_g, cfg.colors.ammo_b, dormant_alpha[ e->EntIndex( ) ] );
	Color esp2 = e->IsDormant( ) ? Color( 0, 0, 0, dormant_alpha[ e->EntIndex( ) ] * 0.85 ) : Color( 0, 0, 0, dormant_alpha[ e->EntIndex( ) ] * 0.85 );

	auto weapon = e->m_hActiveWeapon( );

	if ( !weapon )
		return;

	int ammo = weapon->m_iClip1( );

	float
		bar_width = ammo * bbox.right / weapon->m_data2( )->iMaxClip1;

	AnimationLayer animlayer = e->get_animlayers( )[ 1 ];

	if ( animlayer.m_pOwner ) {

		auto activity = e->sequence_activity( animlayer.m_nSequence );

		bool reloading = ( activity == 967 && animlayer.m_flWeight != 0.0f );

		if ( reloading && animlayer.m_flCycle < 0.99 )
			bar_width = ( animlayer.m_flCycle * bbox.right ) / 1.f;
	}

	surface::get( ).filled_rect( bbox.left - 1, bbox.top + bbox.bottom + 3, bbox.right + 2, 4, esp2 );
	surface::get( ).filled_rect( bbox.left, bbox.top + bbox.bottom + 4, bar_width, 2, esp );
}

void visuals::skeleton( m_entity* e )
{
	Color esp = e->IsDormant( ) ? Color( 255, 255, 255, dormant_alpha[ e->EntIndex( ) ] ) : Color( cfg.colors.skeleton_r, cfg.colors.skeleton_g, cfg.colors.skeleton_b, dormant_alpha[ e->EntIndex( ) ] );

	auto model = e->GetModel( );

	if ( e && !model )
		return;

	studiohdr_t* pStudioModel = g_csgo.modelinfo( )->GetStudiomodel( model );

	if ( pStudioModel ) {
		static matrix3x4_t pBoneToWorldOut[ 128 ];

		if ( e->SetupBones( pBoneToWorldOut, 128, 256, g_csgo.global_vars( )->m_curtime ) ) {
			for ( int i = 0; i < pStudioModel->numbones; i++ ) {
				mstudiobone_t* pBone = pStudioModel->GetBone( i );
				if ( !pBone || !( pBone->flags & 256 ) || pBone->parent == -1 )
					continue;

				Vector2D vBonePos1;
				if ( !utils::world_to_screen( Vector( pBoneToWorldOut[ i ][ 0 ][ 3 ], pBoneToWorldOut[ i ][ 1 ][ 3 ], pBoneToWorldOut[ i ][ 2 ][ 3 ] ), vBonePos1 ) )
					continue;

				Vector2D vBonePos2;
				if ( !utils::world_to_screen( Vector( pBoneToWorldOut[ pBone->parent ][ 0 ][ 3 ], pBoneToWorldOut[ pBone->parent ][ 1 ][ 3 ], pBoneToWorldOut[ pBone->parent ][ 2 ][ 3 ] ), vBonePos2 ) )
					continue;

				surface::get( ).line( ( int )vBonePos1.x, ( int )vBonePos1.y, ( int )vBonePos2.x, ( int )vBonePos2.y, esp );
			}
		}
	}
}

void visuals::name( m_entity* e )
{
	player_info_t pInfo;
	g_csgo.engine( )->GetPlayerInfo( e->EntIndex( ), &pInfo );

	if ( !pInfo.szName )
		return;

	Color esp = e->IsDormant( ) ? Color( 255, 255, 255, dormant_alpha[ e->EntIndex( ) ] ) : Color( cfg.colors.name_r, cfg.colors.name_g, cfg.colors.name_b, dormant_alpha[ e->EntIndex( ) ] );

	surface::get( ).draw_text( bbox.left + ( bbox.right / 2 ), bbox.top - 13, esp, surface::get( ).fonts.esp_font, true, pInfo.szName );
}

void visuals::weapon_icon( m_entity* e )
{
	Color esp = e->IsDormant( ) ? Color( 255, 255, 255, dormant_alpha[ e->EntIndex( ) ] ) : Color( cfg.colors.icon_r, cfg.colors.icon_g, cfg.colors.icon_b, dormant_alpha[ e->EntIndex( ) ] );

	auto weapon = e->m_hActiveWeapon( );

	if ( !weapon )
		return;

	if ( !weapon->m_data( ) )
		return;

	int correct = 0;

	if ( cfg.esp_ammo && !cfg.esp_weapon )
		correct = 10;
	else if ( cfg.esp_ammo && cfg.esp_weapon )
		correct = 23;
	else if ( !cfg.esp_ammo && cfg.esp_weapon )
		correct = 16;
	else
		correct = 6;

	surface::get( ).draw_text( bbox.left + ( bbox.right / 2 ), bbox.top + bbox.bottom + correct, esp, surface::get( ).fonts.weapon_font, true, weapon->weapon_ic( ) );
}

#include <locale>
#include <algorithm>
std::string str_toupper( std::string s ) {
	std::transform( s.begin( ), s.end( ), s.begin( ),
		[ ]( unsigned char c ) { return toupper( c ); }
	);
	return s;
}

void visuals::weapon( m_entity* e )
{
	Color esp = e->IsDormant( ) ? Color( 255, 255, 255, dormant_alpha[ e->EntIndex( ) ] * 0.85 ) : Color( cfg.colors.weapon_r, cfg.colors.weapon_g, cfg.colors.weapon_b, dormant_alpha[ e->EntIndex( ) ] * 0.85 );

	auto weapon = e->m_hActiveWeapon( );

	if ( !weapon )
		return;

	if ( !weapon->m_data( ) )
		return;

	char wpn_name[ 100 ] = "";
	sprintf_s( wpn_name, "%s", str_toupper( weapon->m_data( )->m_WeaponName ).c_str( ) + 7 );

	if ( weapon->m_iItemDefinitionIndex( ) == 64 )
		strcpy_s( wpn_name, "REVOLVER" );

	int correct = 0;

	if ( cfg.esp_ammo )
		correct = 9;
	else
		correct = 3;

	surface::get( ).draw_text( bbox.left + bbox.right / 2, bbox.top + bbox.bottom + correct, esp, surface::get( ).fonts.esp_other, true, wpn_name );
}

void visuals::util_trace_hull( const Vector& vec_start, const Vector& vec_end, const unsigned int n_mask, const Vector& extents, trace_t* p_trace )
{
	CTraceFilterWorldAndPropsOnly Filter;

	Ray_t ray;
	ray.Init( vec_start, vec_end );
	ray.m_Extents = extents;
	ray.m_IsRay = false;

	g_csgo.trace( )->TraceRay( ray, n_mask, &Filter, p_trace );
}

void visuals::third_person( ) {

	if ( !csgo::m_local )
		return;

	if ( !csgo::m_local->is_alive( ) && cfg.misc.tp_bool )
	{
		g_csgo.input( )->m_fCameraInThirdPerson = false;
		csgo::m_local->m_iObserverMode( ) = 5;
		return;
	}

	if ( !csgo::m_local->m_hActiveWeapon( ) )
		return;

	if ( !csgo::m_local->m_hActiveWeapon( )->m_data( ) )
		return;

	if ( cfg.misc.tp_on_grenade && cfg.misc.tp_bool )
	{
		if ( csgo::m_local->m_hActiveWeapon( )->is_grenade( ) )
		{
			g_csgo.input( )->m_fCameraInThirdPerson = false;
			return;
		}
	}

	if ( cfg.misc.tp_bool && utils::get_key( cfg.misc.tp_key, cfg.misc.tp_style ) && csgo::m_local->is_alive( ) )
	{
		Vector view_angles;
		g_csgo.engine( )->GetViewAngles( view_angles );

		g_csgo.input( )->m_fCameraInThirdPerson = true;
		g_csgo.input( )->m_vecCameraOffset = Vector( view_angles.x, view_angles.y, cfg.misc.tp_dist );

		const auto cam_hull_offset = 12.f + ( csgo::m_init_fov / 4.8f - 18.f );
		const Vector cam_hull_extents( cam_hull_offset, cam_hull_offset, cam_hull_offset );
		Vector cam_forward;
		const auto origin = csgo::m_local->get_eye_pos( );

		math::angle_vectors( Vector( g_csgo.input( )->m_vecCameraOffset.x, g_csgo.input( )->m_vecCameraOffset.y, 0.0f ), cam_forward );

		trace_t trace;
		util_trace_hull( origin, origin - ( cam_forward * g_csgo.input( )->m_vecCameraOffset.z ), MASK_SOLID & ~CONTENTS_MONSTER, cam_hull_extents, &trace );

		g_csgo.input( )->m_vecCameraOffset.z *= trace.fraction;
	}
	else
		g_csgo.input( )->m_fCameraInThirdPerson = false;
}

void visuals::post_render( )
{
	g_csgo.cvar( )->FindVar( "r_shadows" )->SetValue( !cfg.removals.remove_shadow );
	g_csgo.cvar( )->FindVar( "cl_csm_shadows" )->SetValue( !cfg.removals.remove_shadow );
	g_csgo.cvar( )->FindVar( "cl_foot_contact_shadows" )->SetValue( !cfg.removals.remove_shadow );
	g_csgo.cvar( )->FindVar( "cl_csm_viewmodel_shadows" )->SetValue( !cfg.removals.remove_shadow );
	g_csgo.cvar( )->FindVar( "cl_csm_rope_shadows" )->SetValue( !cfg.removals.remove_shadow );

	if ( cfg.removals.remove_scope )
	{
		auto m_weapon = csgo::m_local->m_hActiveWeapon( );

		if ( !m_weapon )
			return;

		auto id = m_weapon->m_iItemDefinitionIndex( );

		if ( id == WEAPON_G3SG1 || id == WEAPON_SCAR20 || id == WEAPON_SSG08 || id == WEAPON_AWP  )
		{
			int w, h = 0;
			g_csgo.engine( )->GetScreenSize( w, h );

			if ( csgo::m_local->m_bIsScoped( ) )
			{
				surface::get( ).line( w / 2, 0, w / 2, h, Color( 0, 0, 0, 255 ) );
				surface::get( ).line( 0, h / 2, w, h / 2, Color( 0, 0, 0, 255 ) );
			}
		}
	}
}

void visuals::remove_smoke( )
{
	static auto smoke_count = *( int* )( utils::pattern_scan( "client.dll", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0" ) + 8 );

	static std::vector<std::string> smoke_materials =
	{
		( "particle/beam_smoke_01" ),
		( "particle/particle_smokegrenade" ),
		( "particle/particle_smokegrenade1" ),
		( "particle/particle_smokegrenade2" ),
		( "particle/particle_smokegrenade3" ),
		( "particle/particle_smokegrenade_sc" ),
		( "particle/smoke1/smoke1" ),
		( "particle/smoke1/smoke1_ash" ),
		( "particle/smoke1/smoke1_nearcull" ),
		( "particle/smoke1/smoke1_nearcull2" ),
		( "particle/smoke1/smoke1_snow" ),
		( "particle/smokesprites_0001" ),
		( "particle/smokestack" ),
		( "particle/vistasmokev1/vistasmokev1" ),
		( "particle/vistasmokev1/vistasmokev1_emods" ),
		( "particle/vistasmokev1/vistasmokev1_emods_impactdust" ),
		( "particle/vistasmokev1/vistasmokev1_fire" ),
		( "particle/vistasmokev1/vistasmokev1_nearcull" ),
		( "particle/vistasmokev1/vistasmokev1_nearcull_fog" ),
		( "particle/vistasmokev1/vistasmokev1_nearcull_nodepth" ),
		( "particle/vistasmokev1/vistasmokev1_smokegrenade" ),
		( "particle/vistasmokev1/vistasmokev4_emods_nocull" ),
		( "particle/vistasmokev1/vistasmokev4_nearcull" ),
		( "particle/vistasmokev1/vistasmokev4_nocull" )
	};

	static auto o_smoke = cfg.removals.remove_smoke;

	if ( o_smoke != cfg.removals.remove_smoke ) {
		for ( auto mat : smoke_materials ) {
			auto m = g_csgo.materialsystem( )->FindMaterial( mat.c_str( ), ( "Other textures" ) );
			m->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, cfg.removals.remove_smoke );
		}

		o_smoke = cfg.removals.remove_smoke;
	}

	if ( cfg.removals.remove_smoke )
		*( int* )smoke_count = 0;
}


void visuals::sky_box( )
{
	static auto skybox = g_csgo.cvar()->FindVar( "sv_skyname" );
	*( float* )( ( DWORD )&skybox->fnChangeCallback + 0xC ) = NULL;

	static auto backup_skybox = skybox->GetString( );
	static auto in_game = false;

	if ( !in_game && g_csgo.engine()->IsInGame( ) )
	{
		in_game = true;
		backup_skybox = skybox->GetString( );
	}
	else if ( !g_csgo.engine()->IsInGame( ) )
	{
		in_game = false;
		skybox->SetValueChar( backup_skybox );
	}

	auto skybox_name = backup_skybox;

	switch ( cfg.world.skybox )
	{
	case 0:
		skybox_name = backup_skybox;
		break;
	case 1:
		skybox_name = "cs_tibet";
		break;
	case 2:
		skybox_name = "cs_baggage_skybox_";
		break;
	case 3:
		skybox_name = "italy";
		break;
	case 4:
		skybox_name = "jungle";
		break;
	case 5:
		skybox_name = "office";
		break;
	case 6:
		skybox_name = "sky_cs15_daylight01_hdr";
		break;
	case 7:
		skybox_name = "sky_cs15_daylight02_hdr";
		break;
	case 8:
		skybox_name = "vertigoblue_hdr";
		break;
	case 9:
		skybox_name = "vertigo";
		break;
	case 10:
		skybox_name = "sky_day02_05_hdr";
		break;
	case 11:
		skybox_name = "nukeblank";
		break;
	case 12:
		skybox_name = "sky_venice";
		break;
	case 13:
		skybox_name = "sky_cs15_daylight03_hdr";
		break;
	case 14:
		skybox_name = "sky_cs15_daylight04_hdr";
		break;
	case 15:
		skybox_name = "sky_csgo_cloudy01";
		break;
	case 16:
		skybox_name = "sky_csgo_night02";
		break;
	case 17:
		skybox_name = "sky_csgo_night02b";
		break;
	case 18:
		skybox_name = "sky_csgo_night_flat";
		break;
	case 19:
		skybox_name = "sky_dust";
		break;
	case 20:
		skybox_name = "vietnam";
		break;
	}

	if ( skybox_name != skybox->GetString( ) )
		skybox->SetValueChar( skybox_name );
}

void visuals::modulate_world( )
{
	const auto reset = [ & ]( )
	{
		for ( auto i = g_csgo.materialsystem()->FirstMaterial( ); i != g_csgo.materialsystem()->InvalidMaterial( ); i = g_csgo.materialsystem()->NextMaterial( i ) )
		{
			auto mat = g_csgo.materialsystem()->GetMaterial( i );

			if ( !mat )
				continue;

			if ( mat->IsErrorMaterial( ) )
				continue;

			std::string name = mat->GetName( );
			auto tex_name = mat->GetTextureGroupName( );

			if ( strstr( tex_name, "World" ) || strstr( tex_name, "StaticProp" ) )
			{
				mat->ColorModulate( 1.f, 1.f, 1.f );
				mat->AlphaModulate( 1.f );
			}
		}
	};

	static bool Reset = false;
	static bool Reset2 = false;

	if ( cfg.world.modulate )
	{
		Reset = false;

		static float OldClr[ 3 ];
		static float OldClr1[ 3 ];
		static float OldClr2[ 3 ];

		if ( cfg.world.modulate_world )
		{
			if ( !Reset2 || OldClr[ 0 ] != cfg.world.world_r || OldClr[ 1 ] != cfg.world.world_g || OldClr[ 2 ] != cfg.world.world_b )
			{
				for ( auto i = g_csgo.materialsystem()->FirstMaterial( ); i != g_csgo.materialsystem()->InvalidMaterial( ); i = g_csgo.materialsystem()->NextMaterial( i ) )
				{
					auto mat = g_csgo.materialsystem()->GetMaterial( i );
					if ( !mat )
						continue;

					auto tex_name = mat->GetTextureGroupName( );

					if ( strstr( tex_name, "World" ) )
					{
						mat->ColorModulate( cfg.world.world_r / 255.f,
							cfg.world.world_g / 255.f, cfg.world.world_b / 255.f );
					}
				}

				OldClr[ 0 ] = cfg.world.world_r;
				OldClr[ 1 ] = cfg.world.world_g;
				OldClr[ 2 ] = cfg.world.world_b;

				Reset2 = true;
			}
		}

		if ( cfg.world.prop )
		{
			if ( !Reset2 || OldClr1[ 0 ] != cfg.world.prop_r || OldClr1[ 1 ] != cfg.world.prop_g || OldClr1[ 2 ] != cfg.world.prop_b )
			{
				for ( auto i = g_csgo.materialsystem()->FirstMaterial( ); i != g_csgo.materialsystem()->InvalidMaterial( ); i = g_csgo.materialsystem()->NextMaterial( i ) )
				{
					auto mat = g_csgo.materialsystem()->GetMaterial( i );

					if ( !mat )
						continue;

					auto tex_name = mat->GetTextureGroupName( );

					if ( strstr( tex_name, "StaticProp" ) )
					{
						mat->ColorModulate( cfg.world.prop_r / 255.f,
							cfg.world.prop_g / 255.f, cfg.world.prop_b / 255.f );
					}
				}

				OldClr1[ 0 ] = cfg.world.prop_r;
				OldClr1[ 1 ] = cfg.world.prop_g;
				OldClr1[ 2 ] = cfg.world.prop_b;

				Reset2 = true;
			}
		}
	}
	else
	{
		Reset2 = false;

		if ( !Reset )
		{
			reset( );
			Reset = true;
		}
	}
}

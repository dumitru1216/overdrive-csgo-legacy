#include "vhooks.h"
#include "../../features/chams.h"

using draw_model_execute_t = void( __thiscall* ) ( IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );

void __stdcall hooks::draw_model_execute( IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pinfo, matrix3x4_t* custom_bone_to_world )
{
	static auto ofunc = hooks::modelrender.get_original< draw_model_execute_t >( indexes::m_draw_model );

	if ( m_inUnhook )
		return ofunc( g_csgo.modelrender( ), ctx, state, pinfo, custom_bone_to_world );

	if ( !custom_bone_to_world || !g_csgo.renderview( ) || g_csgo.modelrender( )->IsForcedMaterialOverride( ) && !strstr( pinfo.pModel->szName, "arms" ) && !strstr( pinfo.pModel->szName, "weapons/v_" ) )
		return ofunc( g_csgo.modelrender( ), ctx, state, pinfo, custom_bone_to_world );

	chams::get( ).on_render( ctx, state, pinfo, custom_bone_to_world );

	ofunc( g_csgo.modelrender( ), ctx, state, pinfo, custom_bone_to_world );

	g_csgo.modelrender( )->ForcedMaterialOverride( nullptr );
	g_csgo.renderview( )->SetBlend( 1.f );
}
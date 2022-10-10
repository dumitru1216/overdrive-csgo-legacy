#include "vhooks.h"
#include "../../globals.h"

#include "../../features/animations.h"
#include "../../features/prediction.h"
#include "../../features/visuals.h"
#include "../../features/resolver.hpp"
#include "../../sdk/other/e_nums.h"
#include "../../../gui/config/config.h"

using frame_stage_notify_t = void( __stdcall* )( client_frame_stage_t );

void __stdcall hooks::frame_stage_notify( client_frame_stage_t stage ) {

	static auto original_fn = hooks::client.get_original<frame_stage_notify_t>( indexes::m_stage );

	if ( m_inUnhook )
		return original_fn( stage );

	if ( g_csgo.engine( )->IsInGame( ) && g_csgo.engine( )->IsConnected( ) && csgo::m_local )
	{
		if ( csgo::m_local->is_alive( ) )
		{
			visuals::get( ).modulate_world( );
			visuals::get( ).sky_box( );
			visuals::get( ).remove_smoke( );

			int framstage_minus2 = stage - 2;

			if ( framstage_minus2 ) {
				// wtf
			}
			else {
				prediction::get( ).update_vel( );
			}

			if ( stage == frame_net_update_postdataupdate_start ) {
				for ( int i = 1; i <= g_csgo.global_vars( )->m_maxclients; i++ ) {
					m_entity* e = static_cast< m_entity* >( g_csgo.entitylist( )->GetClientEntity( i ) );

					if ( !e || !e->is_alive( ) || e->IsDormant( ) || e == csgo::m_local )
						continue;

					c_resolver::get().run(e);
					
				}
			}

			if ( stage == frame_net_update_end )
			{
				prediction::get( ).setup_data( );

				if ( cfg.rage.active )
					animations::get( ).UpdatePlayers( );
				else
				{
					for ( int i = 1; i <= g_csgo.global_vars( )->m_maxclients; i++ ) {

						m_entity* e = static_cast< m_entity* >( g_csgo.entitylist( )->GetClientEntity( i ) );

						if ( !e || !e->is_alive( ) || e->IsDormant( ) || e == csgo::m_local )
							continue;

						csgo::m_update[ e->EntIndex( ) ] = true;
						e->update_client_side_animation( );
						csgo::m_update[ e->EntIndex( ) ] = false;

						auto VarMap = reinterpret_cast< uintptr_t >( e ) + 36;
						auto VarMapSize = *reinterpret_cast< int* >( VarMap + 20 );

						for ( auto index = 0; index < VarMapSize; index++ )
							*reinterpret_cast< uintptr_t* >( *reinterpret_cast< uintptr_t* >( VarMap ) + index * 12 ) = 0; // bameware
					}
				}



				//animations::get( ).fake_animations( );
				//animations::get( ).local_animations( );
			}

			if ( stage == frame_render_start )
			{
				*( bool* )g_csgo.post_process( ) = cfg.removals.remove_post_process ? true : false;

				if ( cfg.removals.remove_flash )
					csgo::m_local->m_flFlashDuration( ) = 0;

				if ( cfg.misc.tp_bool && utils::get_key( cfg.misc.tp_key, cfg.misc.tp_style ) && csgo::m_local->is_alive( ) )
				{
					*( Vector* )( ( DWORD )csgo::m_local + 0x31C8 ) = csgo::m_real;
				}

			//	if ( g_csgo.input( )->m_fCameraInThirdPerson )
				//	g_csgo.prediction( )->SetLocalAngles( csgo::m_real );

				if ( csgo::m_local->m_fFlags( ) & FL_ONGROUND ) {
					csgo::m_local->get_animation_state( )->m_bOnGround = true;
					csgo::m_local->get_animation_state( )->m_bInHitGroundAnimation = false;
				}

				csgo::m_local->set_abs_angles( Vector( 0, csgo::m_local->get_animation_state( )->m_flGoalFeetYaw, 0 ) );
			}
		}
	}

	original_fn( stage );
}

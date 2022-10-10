#include "vhooks.h"
#include "../../globals.h"
#include "../../../gui/config/config.h"

#include "../../features/ragebot.h"
#include "../../features/prediction.h"
#include "../../features/movement.h"
#include "../../features/antihit.h"

using create_move_t = bool( __thiscall* )( IClientMode*, float, CUserCmd* );

bool __stdcall hooks::create_move( float smt, CUserCmd* m_pcmd )
{
	static auto original = hooks::clientmode.get_original<create_move_t>( indexes::m_create );
	rage::get( ).drop_target( );

	if ( m_inUnhook )
		return original( g_csgo.clientmode( ), smt, m_pcmd );

	original( g_csgo.clientmode( ), smt, m_pcmd );

	csgo::m_send_packet = true;

	csgo::m_local = reinterpret_cast< m_entity* >( g_csgo.entitylist( )->GetClientEntity( g_csgo.engine( )->GetLocalPlayer( ) ) );
	csgo::m_cmd = m_pcmd;
	
	if ( !csgo::m_cmd || !csgo::m_cmd->command_number || !csgo::m_local )
		return original( g_csgo.clientmode( ), smt, m_pcmd );

	csgo::m_original = m_pcmd->viewangles;
	 
	uintptr_t* frame_ptr;
	__asm mov frame_ptr, ebp;

	csgo::m_send_packet = g_csgo.clientstate( )->m_NetChannel->m_nChokedPackets >= cfg.antiaim.fakelag_type;
	csgo::override_velocity = true;

	if ( csgo::m_local->is_alive( ) )
	{
		rage::get( ).drop_target( );

		prediction::get( ).init( );
		prediction::get( ).predict( m_pcmd );
		{
			antiaim::get( ).run( m_pcmd );

			if ( cfg.rage.active )
				rage::get( ).on_create_move( m_pcmd );

			movement::get( ).fix_movement( m_pcmd );
		}
		prediction::get( ).finish( );
	}

	csgo::override_velocity = false;
	
	if ( csgo::m_send_packet )
		csgo::m_real = m_pcmd->viewangles;
	else 
		csgo::m_fake = m_pcmd->viewangles;

	if ( !cfg.misc.anti_untrusted )
	{
		math::normalize_angles( m_pcmd->viewangles );
		math::clamp_angles( m_pcmd->viewangles );
	}
	
	if ( g_csgo.engine( )->GetNetChannel( )->m_nChokedPackets > cfg.antiaim.fakelag_type )
		csgo::m_send_packet = true;

	*( bool* )( *frame_ptr - 0x1C ) = csgo::m_send_packet;
	
	if ( cfg.rage.active )
		animations::get( ).on_create( );

	return false;
}
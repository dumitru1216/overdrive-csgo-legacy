#include "vhooks.h"

#include "../../globals.h"
#include "../../../gui/config/config.h"

#include "../../../utils/utl.h"
#include "../../features/prediction.h"

using fn = void( __thiscall* )( void*, m_entity*, CUserCmd*, IMoveHelper* );

void __fastcall hooks::run_cmd( void* ecx, void*, m_entity* player, CUserCmd* ucmd, IMoveHelper* helper )
{
	static auto original = hooks::prediction.get_original< fn >( indexes::m_run_cmd );

	if ( !player || player->EntIndex( ) != g_csgo.engine( )->GetLocalPlayer( ) || m_inUnhook )
		return original( ecx, player, ucmd, helper );

	// if ( ucmd->tick_count >= ( int( 1 / interfaces::global_vars->m_interval_per_tick ) + 8 ) ) {
		// ucmd->hasbeenpredicted = true;
		// return;
	// }

	int backup_tickbase = player->m_nTickBase( );
	float backup_curtime = g_csgo.global_vars( )->m_curtime;

	/*
	if ( ucmd->command_number == g_tickbase.m_prediction.m_shifted_command ) {
		player->m_nTickBase( ) = ( g_tickbase.m_prediction.m_original_tickbase - g_tickbase.m_prediction.m_shifted_ticks + 1 );
		++player->m_nTickBase( );

		interfaces::global_vars->m_curtime = ticks_to_time( player->m_nTickBase( ) );
	}
	*/

	float m_flVelModBackup = player->m_flVelocityModifier( );

	prediction::get( ).patch_attack_packet( ucmd, true );

	if ( csgo::override_velocity && ucmd->command_number == g_csgo.clientstate( )->last_command_ack + 1 )
		player->m_flVelocityModifier( ) = csgo::backup.m_velocity_modifier;

	original( ecx, player, ucmd, helper );

	if ( !csgo::override_velocity )
		player->m_flVelocityModifier( ) = m_flVelModBackup;

	/*
	if ( ucmd->command_number == g_tickbase.m_prediction.m_shifted_command ) {
		player->m_nTickBase( ) = backup_tickbase;

		interfaces::global_vars->m_curtime = backup_curtime;
	}
	*/

	prediction::get( ).patch_attack_packet( ucmd, false );
	prediction::get( ).store_data( );
}
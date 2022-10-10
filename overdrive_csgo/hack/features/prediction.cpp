#include "prediction.h"

#include "../../utils/utl.h"
#include "../globals.h"

void prediction::init( )
{
	if ( prediction_data.prediction_stage != INITIALIZE )
		return;

	if ( csgo::backup.m_velocity_modifier < 1.0 ) {
		*reinterpret_cast< int* >( reinterpret_cast< uintptr_t >( g_csgo.prediction( ) + 0x24 ) ) = 1;
	}

	if ( g_csgo.clientstate( )->m_nDeltaTick > 0 )
		g_csgo.prediction( )->Update( g_csgo.clientstate( )->m_nDeltaTick, true, g_csgo.clientstate( )->last_command_ack, g_csgo.clientstate( )->lastoutgoingcommand + g_csgo.clientstate( )->chokedcommands );

	static auto random_seed = *reinterpret_cast< uint32_t** >( utils::pattern_scan( "client.dll", "8B 0D ?? ?? ?? ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4" ) + 0x2 );
	auto globals = g_csgo.global_vars( );

	prediction_data.curtime = globals->m_curtime;
	prediction_data.frametime = globals->m_frametime;

	if ( random_seed )
	{
		prediction_data.prediction_random_seed = random_seed;
		prediction_data.random_seed = *random_seed;
	}

	globals->m_curtime = csgo::m_local->m_nTickBase( ) * globals->m_interval_per_tick;
	globals->m_frametime = globals->m_interval_per_tick;

	backup_data.flags = csgo::m_local->m_fFlags( );
	backup_data.velocity = csgo::m_local->m_vecVelocity( );

	prediction_data.prediction_stage = PREDICT;
}

void prediction::predict( CUserCmd* m_pCmd )
{
	if ( prediction_data.prediction_stage != PREDICT )
		return;

	int m_nServerCommandsAcknowledged = *( int* )( uintptr_t( g_csgo.prediction( ) ) + 0x20 );
	bool m_bInPrediction = *( bool* )( uintptr_t( g_csgo.prediction( ) ) + 8 );

	prediction_data.tickbase = csgo::m_local->m_nTickBase( );
	prediction_data.command_ack = m_nServerCommandsAcknowledged;
	prediction_data.in_prediction = m_bInPrediction;

	if ( prediction_data.prediction_random_seed )
		*prediction_data.prediction_random_seed = MD5_PseudoRandom( m_pCmd->command_number ) & INT_MAX;

	m_entity* m_pPlayer = csgo::m_local;

	static auto m_nImpulse = utils::find_in_datamap( m_pPlayer->GetPredDescMap( ), "m_nImpulse" );
	static auto m_nButtons = utils::find_in_datamap( m_pPlayer->GetPredDescMap( ), "m_nButtons" );
	static auto m_afButtonLast = utils::find_in_datamap( m_pPlayer->GetPredDescMap( ), "m_afButtonLast" );
	static auto m_afButtonPressed = utils::find_in_datamap( m_pPlayer->GetPredDescMap( ), "m_afButtonPressed" );
	static auto m_afButtonReleased = utils::find_in_datamap( m_pPlayer->GetPredDescMap( ), "m_afButtonReleased" );

	if ( m_pCmd->impulse )
		*reinterpret_cast< uint32_t* >( uint32_t( m_pPlayer ) + m_nImpulse ) = m_pCmd->impulse;

	int* buttons = reinterpret_cast< int* >( uint32_t( m_pPlayer ) + m_nButtons );
	const int buttonsChanged = m_pCmd->buttons ^ *buttons;

	*reinterpret_cast< int* >( uint32_t( m_pPlayer ) + m_afButtonLast ) = *buttons;
	*reinterpret_cast< int* >( uint32_t( m_pPlayer ) + m_nButtons ) = m_pCmd->buttons;
	*reinterpret_cast< int* >( uint32_t( m_pPlayer ) + m_afButtonPressed ) = buttonsChanged & m_pCmd->buttons;
	*reinterpret_cast< int* >( uint32_t( m_pPlayer ) + m_afButtonReleased ) = buttonsChanged & ~m_pCmd->buttons;

	CMoveData move_data;
	memset( &move_data, NULL, sizeof( move_data ) );

	*( int* )( uintptr_t( g_csgo.prediction( ) ) + 0x20 ) = 0;
	*( bool* )( uintptr_t( g_csgo.prediction( ) ) + 8 ) = 1;

	csgo::m_local->current_command( ) = m_pCmd;
	g_csgo.movehelper( )->SetHost( csgo::m_local );

	g_csgo.gamemovement( )->StartTrackPredictionErrors( m_pPlayer );
	g_csgo.prediction( )->SetupMove( csgo::m_local, m_pCmd, g_csgo.movehelper( ), &move_data );
	g_csgo.gamemovement( )->ProcessMovement( csgo::m_local, &move_data );

	csgo::m_local->m_nTickBase( ) = prediction_data.tickbase;
	g_csgo.prediction( )->FinishMove( csgo::m_local, m_pCmd, &move_data );

	g_csgo.gamemovement( )->FinishTrackPredictionErrors( m_pPlayer );

	*( int* )( uintptr_t( g_csgo.prediction( ) ) + 0x20 ) = prediction_data.command_ack;
	*( bool* )( uintptr_t( g_csgo.prediction( ) ) + 8 ) = prediction_data.in_prediction;

	prediction_data.prediction_stage = FINISH;
}

void prediction::patch_attack_packet( CUserCmd* m_nCmd, bool m_bPredicted )
{
	static bool m_bLastAttack = false;
	static bool m_bInvalidCycle = false;
	static float m_flLastCycle = 0.f;

	if ( m_bPredicted )
	{
		m_bLastAttack = m_nCmd->weaponselect || ( m_nCmd->buttons & IN_ATTACK );
		m_flLastCycle = csgo::m_local->m_flCycle( );
	}
	else if ( m_bLastAttack && !m_bInvalidCycle )
		m_bInvalidCycle = csgo::m_local->m_flCycle( ) == 0.f && m_flLastCycle > 0.f;

	if ( m_bInvalidCycle )
		csgo::m_local->m_flCycle( ) = m_flLastCycle;
}

void prediction::finish( )
{
	if ( prediction_data.prediction_stage != FINISH )
		return;

	g_csgo.movehelper( )->SetHost( nullptr );
	csgo::m_local->current_command( ) = nullptr;

	if ( prediction_data.prediction_random_seed )
		*prediction_data.prediction_random_seed = -1;

	g_csgo.global_vars( )->m_curtime = prediction_data.curtime;
	g_csgo.global_vars( )->m_frametime = prediction_data.frametime;

	csgo::m_local->m_fFlags( ) = backup_data.flags;
	csgo::m_local->m_vecVelocity( ) = backup_data.velocity;
}

void prediction::store_data( )
{
	int          tickbase;
	StoredData_t* data;

	if ( !csgo::m_local && !csgo::m_local->is_alive( ) ) {
		reset_data( );
		return;
	}

	tickbase = csgo::m_local->m_nTickBase( );

	data = &m_data[ tickbase % MULTIPLAYER_BACKUP ];

	data->m_tickbase = tickbase;
	data->m_punch = csgo::m_local->m_aimPunchAngle( );
	data->m_punch_vel = csgo::m_local->m_aimPunchAngleVel( );
	data->m_view_offset = csgo::m_local->m_vecViewOffset( );
	data->m_velocity_modifier = csgo::m_local->m_flVelocityModifier( );
}

void prediction::setup_data( )
{
	int          tickbase;
	StoredData_t* data;
	Vector        punch_delta, punch_vel_delta;
	Vector       view_delta;
	float        modifier_delta;

	if ( !csgo::m_local && !csgo::m_local->is_alive( ) ) {
		reset_data( );
		return;
	}

	tickbase = csgo::m_local->m_nTickBase( );

	data = &m_data[ tickbase % MULTIPLAYER_BACKUP ];

	if ( csgo::m_local->m_nTickBase( ) != data->m_tickbase )
		return;

	punch_delta = csgo::m_local->m_aimPunchAngle( ) - data->m_punch;
	punch_vel_delta = csgo::m_local->m_aimPunchAngleVel( ) - data->m_punch_vel;
	view_delta = csgo::m_local->m_vecViewOffset( ) - data->m_view_offset;
	modifier_delta = csgo::m_local->m_flVelocityModifier( ) - data->m_velocity_modifier;

	if ( std::abs( punch_delta.x ) < 0.03125f &&
		std::abs( punch_delta.y ) < 0.03125f &&
		std::abs( punch_delta.z ) < 0.03125f )
		csgo::m_local->m_aimPunchAngle( ) = data->m_punch;

	if ( std::abs( punch_vel_delta.x ) < 0.03125f &&
		std::abs( punch_vel_delta.y ) < 0.03125f &&
		std::abs( punch_vel_delta.z ) < 0.03125f )
		csgo::m_local->m_aimPunchAngleVel( ) = data->m_punch_vel;

	if ( std::abs( view_delta.x ) < 0.03125f &&
		std::abs( view_delta.y ) < 0.03125f &&
		std::abs( view_delta.z ) < 0.03125f )
		csgo::m_local->m_vecViewOffset( ) = data->m_view_offset;

	if ( std::abs( modifier_delta ) < 0.03125f )
		csgo::m_local->m_flVelocityModifier( ) = data->m_velocity_modifier;
}

void prediction::reset_data( )
{
	m_data.fill( StoredData_t( ) );
}

void prediction::update_vel( )
{
	static int m_iLastCmdAck = 0;
	static float m_flNextCmdTime = 0.f;

	if ( g_csgo.clientstate( ) && ( m_iLastCmdAck != g_csgo.clientstate( )->last_command_ack || m_flNextCmdTime != g_csgo.clientstate( )->m_flNextCmdTime ) )
	{
		if ( csgo::backup.m_velocity_modifier != csgo::m_local->m_flVelocityModifier( ) )
		{
			*reinterpret_cast< int* >( reinterpret_cast< uintptr_t >( g_csgo.prediction( ) + 0x24 ) ) = 1;
			csgo::backup.m_velocity_modifier = csgo::m_local->m_flVelocityModifier( );
		}

		m_iLastCmdAck = g_csgo.clientstate( )->last_command_ack;
		m_flNextCmdTime = g_csgo.clientstate( )->m_flNextCmdTime;
	}
}
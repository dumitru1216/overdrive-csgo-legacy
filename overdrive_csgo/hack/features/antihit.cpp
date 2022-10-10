#include "antihit.h"
#include "autowall.h"

#include "../../gui/config/config.h"

bool need_to_flip = false;


void antiaim::run( CUserCmd* cmd )
{
	if ( !csgo::m_local->is_alive( ) )
		return;

	auto weapon = csgo::m_local->m_hActiveWeapon( );
	if ( !weapon )
		return;

	if ( weapon->is_grenade( ) )
		if ( weapon->m_fThrowTime( ) > 0 )
			return;

	if ( csgo::m_local->m_nMoveType( ) == 8 /* noclip */ || csgo::m_local->m_nMoveType( ) == 9 /* ladder */ )
		return;

	if ( cmd->buttons & IN_ATTACK )
		return;

	if ( cmd->buttons & IN_USE )
		return;

	if ( csgo::m_send_packet )
		need_to_flip = !need_to_flip;


	switch ( cfg.antiaim.pitch )
	{
	case 1:
		cmd->viewangles.x = -89.9f;
		break;
	case 2:
		cmd->viewangles.x = 89.9f;
		break;
	case 3:
		cmd->viewangles.x = 0.f;
		break;
	}

	if ( cfg.antiaim.base_yaw == 1 )
	{
		vec3_t angles;
		g_csgo.engine()->GetViewAngles( angles );
		cmd->viewangles.y = angles.y;
	}
	else if ( cfg.antiaim.base_yaw == 2 )
		cmd->viewangles.y = at_target( );

	switch ( cfg.antiaim.yaw )
	{
	case 1:
		cmd->viewangles.y += 119.f;
		break;
	}

	need_to_flip ? cmd->viewangles.y -= cfg.antiaim.jitter : cmd->viewangles.y += cfg.antiaim.jitter;

	static float mm = 4.0f;

	if ( fabs( cmd->sidemove ) < 3 && fabs( cmd->forwardmove ) < 3 )
		cmd->sidemove = mm;

	mm = -mm;

	if ( !csgo::m_send_packet )
		cmd->viewangles.y += 58.f;

	cmd->buttons &= ~( IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT );
}

float antiaim::at_target( ) {
	int cur_tar = 0;
	float last_dist = 999999999999.0f;

	for ( int i = 0; i < g_csgo.global_vars()->m_maxclients; i++ ) {
		auto entity = static_cast< m_entity* >( g_csgo.entitylist()->GetClientEntity( i ) );

		if ( !entity || entity == csgo::m_local )
			continue;

		if ( !entity->is_player( ) )
			continue;

		auto m_player = ( m_entity* )entity;
		if ( !m_player->IsDormant( ) && m_player->is_alive( ) && m_player->m_iTeamNum( ) != csgo::m_local->m_iTeamNum( ) ) {
			float cur_dist = ( entity->m_vecOrigin( ) - csgo::m_local->m_vecOrigin( ) ).Length( );

			if ( !cur_tar || cur_dist < last_dist ) {
				cur_tar = i;
				last_dist = cur_dist;
			}
		}
	}

	if ( cur_tar ) {
		auto entity = static_cast< m_entity* >( g_csgo.entitylist()->GetClientEntity( cur_tar ) );
		if ( !entity ) {
			return csgo::m_cmd->viewangles.y;
		}

		Vector target_angle = math::calculate_angle( csgo::m_local->m_vecOrigin( ), entity->m_vecOrigin( ) );
		return target_angle.y;
	}

	return csgo::m_cmd->viewangles.y;
}

float antiaim::get_curtime( CUserCmd* ucmd )
{
	auto local_player = csgo::m_local;

	if ( !local_player )
		return 0;

	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if ( !g_pLastCmd || g_pLastCmd->hasbeenpredicted ) {
		g_tick = ( float )local_player->m_nTickBase( );
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * g_csgo.global_vars( )->m_interval_per_tick;
	return curtime;
}

bool antiaim::next_lby_update( const float yaw_to_break, CUserCmd* cmd )
{
	const auto state = csgo::m_local->get_animation_state( );

	if ( !state )
		return false;

	static auto next_lby_update = 0.f;

	const auto time = ticks_to_time( csgo::m_local->m_nTickBase( ) );

	if ( state->speed_2d > 0.1f ) {
		next_lby_update = time + 0.22f;
	}

	if ( time <= next_lby_update )
		return false;

	next_lby_update = time + 1.1f;

	return true;
}

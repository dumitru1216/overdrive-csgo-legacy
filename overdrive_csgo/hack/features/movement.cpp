#include "movement.h"
#include "../sdk/other/e_nums.h"

void movement::run( Vector org )
{
	static bool bLastJumped = false;
	static bool bShouldFake = false;

	if ( !bLastJumped && bShouldFake )
	{
		bShouldFake = false;
		csgo::m_cmd->buttons |= IN_JUMP;
	}
	else if ( csgo::m_cmd->buttons & IN_JUMP )
	{
		if ( csgo::m_local->m_fFlags( ) & FL_ONGROUND )
		{
			bShouldFake = bLastJumped = true;
		}
		else
		{
			csgo::m_cmd->buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bShouldFake = bLastJumped = false;
	}
	if ( csgo::m_local->m_nMoveType( ) == MoveType_t::MOVETYPE_NOCLIP || csgo::m_local->m_nMoveType( ) == MoveType_t::MOVETYPE_LADDER )
		return;

	if ( csgo::m_cmd->buttons & IN_SPEED && csgo::m_local->m_vecVelocity( ).Length2D( ) < 1.f )
		return;

	static float yaw_add = 0.f;
	static const auto cl_sidespeed = g_csgo.cvar( )->FindVar( "cl_sidespeed" );

	if ( !( csgo::m_local->m_fFlags( ) & FL_ONGROUND ) )
	{
		bool back = csgo::m_cmd->buttons & IN_BACK;
		bool forward = csgo::m_cmd->buttons & IN_FORWARD;
		bool right = csgo::m_cmd->buttons & IN_MOVELEFT;
		bool left = csgo::m_cmd->buttons & IN_MOVERIGHT;

		if ( back ) {
			yaw_add = -180.f;
			if ( right )
				yaw_add -= 45.f;
			else if ( left )
				yaw_add += 45.f;
		}
		else if ( right ) {
			yaw_add = 90.f;
			if ( back )
				yaw_add += 45.f;
			else if ( forward )
				yaw_add -= 45.f;
		}
		else if ( left ) {
			yaw_add = -90.f;
			if ( back )
				yaw_add -= 45.f;
			else if ( forward )
				yaw_add += 45.f;
		}
		else {
			yaw_add = 0.f;
		}

		org.y += yaw_add;
		csgo::m_cmd->forwardmove = 0.f;
		csgo::m_cmd->sidemove = 0.f;

		const auto delta = math::normalize_yaw( org.y - RAD2DEG( atan2( csgo::m_local->m_vecVelocity( ).y, csgo::m_local->m_vecVelocity( ).x ) ) );
		csgo::m_cmd->sidemove = delta > 0.f ? -cl_sidespeed->GetFloat( ) : cl_sidespeed->GetFloat( );
		org.y = math::normalize_yaw( org.y - delta );
	}
}

void movement::fix_movement( CUserCmd* usercmd )
{
	Vector wish_angle;
	g_csgo.engine( )->GetViewAngles( wish_angle );

	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto viewangles = usercmd->viewangles;
	viewangles.Clamp( );

	math::angle_vectors( wish_angle, &view_fwd, &view_right, &view_up );
	math::angle_vectors( viewangles, &cmd_fwd, &cmd_right, &cmd_up );

	const float v8 = sqrtf( ( view_fwd.x * view_fwd.x ) + ( view_fwd.y * view_fwd.y ) );
	const float v10 = sqrtf( ( view_right.x * view_right.x ) + ( view_right.y * view_right.y ) );
	const float v12 = sqrtf( view_up.z * view_up.z );

	const Vector norm_view_fwd( ( 1.f / v8 ) * view_fwd.x, ( 1.f / v8 ) * view_fwd.y, 0.f );
	const Vector norm_view_right( ( 1.f / v10 ) * view_right.x, ( 1.f / v10 ) * view_right.y, 0.f );
	const Vector norm_view_up( 0.f, 0.f, ( 1.f / v12 ) * view_up.z );

	const float v14 = sqrtf( ( cmd_fwd.x * cmd_fwd.x ) + ( cmd_fwd.y * cmd_fwd.y ) );
	const float v16 = sqrtf( ( cmd_right.x * cmd_right.x ) + ( cmd_right.y * cmd_right.y ) );
	const float v18 = sqrtf( cmd_up.z * cmd_up.z );

	const Vector norm_cmd_fwd( ( 1.f / v14 ) * cmd_fwd.x, ( 1.f / v14 ) * cmd_fwd.y, 0.f );
	const Vector norm_cmd_right( ( 1.f / v16 ) * cmd_right.x, ( 1.f / v16 ) * cmd_right.y, 0.f );
	const Vector norm_cmd_up( 0.f, 0.f, ( 1.f / v18 ) * cmd_up.z );

	const float v22 = norm_view_fwd.x * usercmd->forwardmove;
	const float v26 = norm_view_fwd.y * usercmd->forwardmove;
	const float v28 = norm_view_fwd.z * usercmd->forwardmove;
	const float v24 = norm_view_right.x * usercmd->sidemove;
	const float v23 = norm_view_right.y * usercmd->sidemove;
	const float v25 = norm_view_right.z * usercmd->sidemove;
	const float v30 = norm_view_up.x * usercmd->upmove;
	const float v27 = norm_view_up.z * usercmd->upmove;
	const float v29 = norm_view_up.y * usercmd->upmove;

	usercmd->forwardmove = ( ( ( ( norm_cmd_fwd.x * v24 ) + ( norm_cmd_fwd.y * v23 ) ) + ( norm_cmd_fwd.z * v25 ) )
		+ ( ( ( norm_cmd_fwd.x * v22 ) + ( norm_cmd_fwd.y * v26 ) ) + ( norm_cmd_fwd.z * v28 ) ) )
		+ ( ( ( norm_cmd_fwd.y * v30 ) + ( norm_cmd_fwd.x * v29 ) ) + ( norm_cmd_fwd.z * v27 ) );
	usercmd->sidemove = ( ( ( ( norm_cmd_right.x * v24 ) + ( norm_cmd_right.y * v23 ) ) + ( norm_cmd_right.z * v25 ) )
		+ ( ( ( norm_cmd_right.x * v22 ) + ( norm_cmd_right.y * v26 ) ) + ( norm_cmd_right.z * v28 ) ) )
		+ ( ( ( norm_cmd_right.x * v29 ) + ( norm_cmd_right.y * v30 ) ) + ( norm_cmd_right.z * v27 ) );
	usercmd->upmove = ( ( ( ( norm_cmd_up.x * v23 ) + ( norm_cmd_up.y * v24 ) ) + ( norm_cmd_up.z * v25 ) )
		+ ( ( ( norm_cmd_up.x * v26 ) + ( norm_cmd_up.y * v22 ) ) + ( norm_cmd_up.z * v28 ) ) )
		+ ( ( ( norm_cmd_up.x * v30 ) + ( norm_cmd_up.y * v29 ) ) + ( norm_cmd_up.z * v27 ) );

	usercmd->forwardmove = std::clamp( usercmd->forwardmove, -450.f, 450.f );
	usercmd->sidemove = std::clamp( usercmd->sidemove, -450.f, 450.f );
	usercmd->upmove = std::clamp( usercmd->upmove, -320.f, 320.f );
}
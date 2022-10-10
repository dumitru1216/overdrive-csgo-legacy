#include "ragebot.h"
#include "autowall.h"
#include "../../gui/config/config.h"

bool should_slide;

void rage::on_create_move( CUserCmd* m_pcmd )
{
	if ( !m_pcmd || !csgo::m_local->is_alive( ) )
		return;

	if ( !csgo::m_local->m_hActiveWeapon( ) )
		return;

	if ( csgo::m_local->m_hActiveWeapon( )->is_non_aim( ) )
		return;

	this->scan( );
}

void rage::drop_target( )
{
	target_index = -1;
	best_distance = INT_MAX;
	fired_in_that_tick = false;
	current_aim_position = Vector( );
	shot = false;
	should_slide = false;
	autowall::get( ).reset( );
}

float lerp( )
{
	static auto cl_interp = g_csgo.cvar( )->FindVar( "cl_interp" );
	static auto cl_updaterate = g_csgo.cvar( )->FindVar( "cl_updaterate" );
	const auto update_rate = cl_updaterate->GetInt( );
	const auto interp_ratio = cl_interp->GetFloat( );

	auto lerp = interp_ratio / update_rate;

	if ( lerp <= interp_ratio )
		lerp = interp_ratio;

	return lerp;
}

static std::vector<std::tuple<float, float, float>> precomputed_seeds = {};

typedef void( *RandomSeed_t )( UINT );
RandomSeed_t m_RandomSeed = 0;
void random_seed( uint32_t seed ) {
	if ( m_RandomSeed == NULL )
		m_RandomSeed = ( RandomSeed_t )GetProcAddress( GetModuleHandle( "vstdlib.dll" ), "RandomSeed" );
	m_RandomSeed( seed );
}

typedef float( *RandomFloat_t )( float, float );
RandomFloat_t m_RandomFloat;
float random_float( float flLow, float flHigh )
{
	if ( m_RandomFloat == NULL )
		m_RandomFloat = ( RandomFloat_t )GetProcAddress( GetModuleHandle( "vstdlib.dll" ), "RandomFloat" );

	return m_RandomFloat( flLow, flHigh );
}

static const int total_seeds = 255;

void build_seed_table( )
{
	if ( !precomputed_seeds.empty( ) )
		return;

	for ( auto i = 0; i < total_seeds; i++ ) {
		random_seed( i + 1 );

		const auto pi_seed = random_float( 0.f, M_PI * 2 );

		precomputed_seeds.emplace_back( random_float( 0.f, 1.f ),
			sin( pi_seed ), cos( pi_seed ) );
	}
}

void AngleVectors( const Vector& angles, Vector& forward )
{
	Assert( s_bMathlibInitialized );
	Assert( forward );

	float sp, sy, cp, cy;

	sy = sin( DEG2RAD( angles[ 1 ] ) );
	cy = cos( DEG2RAD( angles[ 1 ] ) );

	sp = sin( DEG2RAD( angles[ 0 ] ) );
	cp = cos( DEG2RAD( angles[ 0 ] ) );

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

void VectorAngles( const Vector& vecForward, Vector& vecAngles )
{
	Vector vecView;
	if ( vecForward[ 1 ] == 0.f && vecForward[ 0 ] == 0.f )
	{
		vecView[ 0 ] = 0.f;
		vecView[ 1 ] = 0.f;
	}
	else
	{
		vecView[ 1 ] = atan2( vecForward[ 1 ], vecForward[ 0 ] ) * 180.f / 3.14159265358979323846f;

		if ( vecView[ 1 ] < 0.f )
			vecView[ 1 ] += 360.f;

		vecView[ 2 ] = sqrt( vecForward[ 0 ] * vecForward[ 0 ] + vecForward[ 1 ] * vecForward[ 1 ] );

		vecView[ 0 ] = atan2( vecForward[ 2 ], vecView[ 2 ] ) * 180.f / 3.14159265358979323846f;
	}

	vecAngles[ 0 ] = -vecView[ 0 ];
	vecAngles[ 1 ] = vecView[ 1 ];
	vecAngles[ 2 ] = 0.f;
}

bool HitTraces( animation* _animation, const Vector position, const float chance, int box )
{
	build_seed_table( );

	const auto weapon = csgo::m_local->m_hActiveWeapon( );

	if ( !weapon )
		return false;

	const auto info = weapon->m_data( );

	if ( !info )
		return false;

	if ( cfg.rage_accuracy.penalty_update )
		weapon->penalty( );

	const auto studio_model = g_csgo.modelinfo( )->GetStudiomodel( _animation->player->GetModel( ) );

	if ( !studio_model )
		return false;

	// performance optimization.
	if ( ( csgo::m_local->get_eye_pos( ) - position ).Length2D( ) > info->m_flRange )
		return false;

	// setup calculation parameters.
	const auto id = weapon->m_iItemDefinitionIndex( );
	const auto round_acc = [ ]( const float accuracy ) { return roundf( accuracy * 1000.f ) / 1000.f; };
	const auto crouched = csgo::m_local->m_fFlags( ) & FL_DUCKING;

	// calculate inaccuracy.
	const auto weapon_inaccuracy = weapon->inaccuracy( );

	if ( id == WEAPON_REVOLVER )
		return weapon_inaccuracy < ( crouched ? .0020f : .0055f );

	// calculate start and angle.
	auto start = csgo::m_local->get_eye_pos( );
	const auto aim_angle = math::calculate_angle( start, position );
	Vector forward, right, up;
	math::angle_vectors( aim_angle, forward, right, up );

	// keep track of all traces that hit the enemy.
	auto current = 0;
	int awalls_hit = 0;

	// setup calculation parameters.
	Vector total_spread, spread_angle, end;
	float inaccuracy, spread_x, spread_y;
	std::tuple<float, float, float>* seed;

	// use look-up-table to find average hit probability.
	for ( auto i = 0u; i < total_seeds; i++ )  // NOLINT(modernize-loop-convert)
	{
		// get seed.
		seed = &precomputed_seeds[ i ];

		// calculate spread.
		inaccuracy = std::get<0>( *seed ) * weapon_inaccuracy;
		spread_x = std::get<2>( *seed ) * inaccuracy;
		spread_y = std::get<1>( *seed ) * inaccuracy;
		total_spread = ( forward + right * spread_x + up * spread_y ).Normalized( );

		// calculate angle with spread applied.
		VectorAngles( total_spread, spread_angle );

		// calculate end point of trace.
		AngleVectors( spread_angle, end );
		end = start + end.Normalized( ) * info->m_flRange;

		trace_t tr;
		Ray_t ray;

		ray.Init( start, end );
		g_csgo.trace( )->ClipRayToEntity( ray, MASK_SHOT | CONTENTS_GRATE, _animation->player, &tr );

		if ( auto ent = tr.hit_entity; ent )
		{
			if ( ent == _animation->player )
			{
				current++;

				if ( cfg.rage_accuracy.accuracy_boost > 1.f && autowall::get( ).Think( position, _animation->player, box ).m_damage > 1.f )
					awalls_hit++;
			}
		}
		
		// abort if hitchance is already sufficent.
		if ( ( ( static_cast< float >( current ) / static_cast< float >( total_seeds ) ) >= ( chance / 100.f ) ) )
		{
			if ( ( ( static_cast< float >( awalls_hit ) / static_cast< float >( total_seeds ) ) >= ( cfg.rage_accuracy.accuracy_boost / 100.f ) ) || cfg.rage_accuracy.accuracy_boost <= 1.f )
				return true;
		}

		// abort if we can no longer reach hitchance.
		if ( static_cast< float >( current + total_seeds - i ) / static_cast< float >( total_seeds ) < chance )
			return false;
	}

	return static_cast< float >( current ) / static_cast< float >( total_seeds ) >= chance;
}

bool Hitchance( Vector Aimpoint, bool backtrack, animation* best, int& hitbox )
{
	return HitTraces( best, Aimpoint, cfg.rage_accuracy.hitchance / 100.f, hitbox );
}

void fast_stop( ) 
{
	auto wpn_info = csgo::m_local->m_hActiveWeapon( )->m_data2( );

	if ( !wpn_info )
		return;

	auto get_standing_accuracy = [ & ]( ) -> const float
	{
		const auto max_speed = csgo::m_local->m_hActiveWeapon( )->m_zoomLevel( ) > 0 ? wpn_info->flSpreadAlt : wpn_info->flSpread;
		return max_speed / 3.f; 
	};

	auto velocity = csgo::m_local->m_vecVelocity( );
	float speed = velocity.Length2D( );
	float max_speed = ( csgo::m_local->m_hActiveWeapon( )->m_zoomLevel( ) == 0 ? wpn_info->flSpread : wpn_info->flSpreadAlt ) * 0.1f;

	if ( speed > max_speed ) {
		should_slide = false;
	}
	else {
		should_slide = true;
		return;
	}

	if ( speed <= get_standing_accuracy( ) )
		return;

	Vector Direction;
	Vector RealView;
	VectorAngles( velocity, Direction );
	g_csgo.engine( )->GetViewAngles( RealView );
	Direction.y = RealView.y - Direction.y;

	Vector Forward;
	AngleVectors( Direction, Forward );
	Vector NegativeDirection = Forward * -speed;

	csgo::m_cmd->forwardmove = NegativeDirection.x;
	csgo::m_cmd->sidemove = NegativeDirection.y;
}

bool can_shot( )
{
	auto time = ticks_to_time( csgo::m_local->m_nTickBase( ) );
	auto wpn = csgo::m_local->m_hActiveWeapon( );

	if ( !wpn )
		return false;

	auto m_dat = wpn->m_data2( );

	if ( !m_dat )
		return false;

	if ( csgo::m_cmd->weaponselect )
		return false;
	
	if ( wpn->m_iClip1( ) < 1 )
		return false;
	
	if ( ( csgo::m_local->m_flNextAttack( ) > time ) || wpn->m_flNextPrimaryAttack( ) > time || wpn->m_flNextSecondaryAttack( ) > time )
	{
		if ( wpn->m_iItemDefinitionIndex( ) != WEAPON_REVOLVER && m_dat->WeaponType == 1 )
			csgo::m_cmd->buttons &= ~IN_ATTACK;

		return false;
	}

	return true;
}

int ticks_to_shot( ) 
{
	if ( can_shot( ) )
		return -1;

	auto flServerTime = ( float )csgo::m_local->m_nTickBase( ) * g_csgo.global_vars( )->m_interval_per_tick;
	auto flNextPrimaryAttack = csgo::m_local->m_hActiveWeapon( )->m_flNextPrimaryAttack( );

	return time_to_ticks( fabsf( flNextPrimaryAttack - flServerTime ) );
}

int ticks_to_stop( ) 
{
	static auto predict_velocity = [ ]( Vector* velocity )
	{
		float speed = velocity->Length2D( );
		static auto sv_friction = g_csgo.cvar( )->FindVar( "sv_friction" );
		static auto sv_stopspeed = g_csgo.cvar( )->FindVar( "sv_stopspeed" );

		if ( speed >= 1.f )
		{
			float friction = sv_friction->GetFloat( );
			float stop_speed = std::max< float >( speed, sv_stopspeed->GetFloat( ) );
			float time = std::max< float >( g_csgo.global_vars( )->m_interval_per_tick, g_csgo.global_vars( )->m_frametime );
			*velocity *= std::max< float >( 0.f, speed - friction * stop_speed * time / speed );
		}
	};

	Vector vel = csgo::m_local->m_vecVelocity( );
	int ticks_to_stop = 0;
	while ( true )
	{
		if ( vel.Length2D( ) < 1.f )
			break;
		predict_velocity( &vel );
		ticks_to_stop++;
	}
	return ticks_to_stop;
}

void rage::scan( )
{
	auto weapon = csgo::m_local->m_hActiveWeapon( );

	if ( !weapon )
		return;

	should_slide = false;
	int curhitbox;
	animation* best_anims = nullptr;
	int hitbox = -1;

	float simtime = 0;
	Vector minus_origin = Vector( 0, 0, 0 );
	animation* anims = nullptr;
	int box;

	bool is_able_to_shoot = can_shot( );

	for ( auto i = 1; i <= g_csgo.global_vars( )->m_maxclients; i++ )
	{
		m_entity* e = static_cast< m_entity* >( g_csgo.entitylist( )->GetClientEntity( i ) );

		if ( !e || !e->is_alive( ) || e->IsDormant( ) || e == csgo::m_local || e->m_iTeamNum( ) == csgo::m_local->m_iTeamNum( ) )
			continue;

		if ( e->m_bGunGameImmunity( ) || e->m_fFlags( ) & FL_FROZEN )
			continue;
		
		auto intervals = g_csgo.global_vars( )->m_interval_per_tick * 2.0f;
		auto unlag = fabs( e->m_flSimulationTime( ) - e->m_flOldSimulationTime( ) ) < intervals;

		if ( !unlag && cfg.rage_target.unlag )
			continue;

		if ( cfg.rage_target.delay_shot ) 
			if ( e->m_flSimulationTime( ) == e->m_flOldSimulationTime( ) )
				continue;
		
		Vector aim_position = get_aim_vector( e, simtime, minus_origin, anims, box );

		if ( !anims )
			continue;

		int health = e->m_iHealth( );
		if ( best_distance > health && anims->player == e && aim_position != Vector( 0, 0, 0 ) )
		{
			best_distance = health;
			target_index = i;
			current_aim_position = aim_position;
			current_aim_simulationtime = simtime;
			current_aim_player_origin = minus_origin;
			best_anims = anims;
			hitbox = box;
			target_anims = best_anims;
		}
	}

	if ( hitbox != -1 && target_index != -1 && best_anims && current_aim_position != Vector( 0, 0, 0 ) )
	{
		if ( cfg.rage_target.auto_scope && weapon->is_sniper( ) && weapon->m_zoomLevel( ) == 0 )
		{
			csgo::m_cmd->buttons |= IN_ATTACK2;
			return;
		}

		float flServerTime = csgo::m_local->m_nTickBase( ) * g_csgo.global_vars( )->m_interval_per_tick;
		bool canShoot = ( csgo::m_local->m_hActiveWeapon( )->m_flNextPrimaryAttack( ) <= flServerTime ) && csgo::m_local->m_hActiveWeapon( )->m_iClip1( ) > 0;

		static int MinimumVelocity = csgo::m_local->m_hActiveWeapon( )->m_data2( )->flSpreadAlt * .34f;
		bool htchance = Hitchance( current_aim_position, false, best_anims, hitbox );
		
		should_slide = false;

		if ( csgo::m_local->m_fFlags( ) & FL_ONGROUND  ) {

			bool should_stop = ticks_to_shot( ) <= ticks_to_stop( ) || is_able_to_shoot;

			if ( should_stop )
			{
				if ( !should_slide )
					fast_stop( );

				should_slide = true;
			}
			else
				should_slide = false;
		}

		bool need_to_delay;

		if ( cfg.rage_target.spread_limit )
			need_to_delay = spread_limit( );
		else
			need_to_delay = true;

		if ( htchance && need_to_delay && is_able_to_shoot ) {

			if ( cfg.rage_target.auto_fire )
			{
				csgo::m_send_packet = true;
				csgo::m_cmd->buttons |= IN_ATTACK;
			}
			
			if ( csgo::m_cmd->buttons & IN_ATTACK ) {
				csgo::m_cmd->viewangles = math::calculate_angle( csgo::m_local->get_eye_pos( ), current_aim_position ) - csgo::m_local->m_aimPunchAngle( ) * 2.f;;
				csgo::m_cmd->tick_count = time_to_ticks( anims->sim_time + lerp( ) );
			}
		}
	}
}

bool rage::head_valid( m_entity* e )
{
	return true;
}

std::vector<int> rage::hitboxes_to_scan( m_entity* pEntity )
{
	std::vector< int > hitboxes;

	if ( priority_hitbox( pEntity ) == ( int )HITBOX_PELVIS )
	{
		hitboxes.push_back( ( int )HITBOX_CHEST );
		hitboxes.push_back( ( int )HITBOX_STOMACH );
		hitboxes.push_back( ( int )HITBOX_PELVIS );

		return hitboxes;
	}

	if ( cfg.rage_points.head )
		hitboxes.push_back( ( int )HITBOX_HEAD );

	if ( cfg.rage_points.neck )
		hitboxes.push_back( ( int )HITBOX_NECK );

	if ( cfg.rage_points.upper_chest )
		hitboxes.push_back( ( int )HITBOX_UPPER_CHEST );
	
	if ( cfg.rage_points.chest )
		hitboxes.push_back( ( int )HITBOX_CHEST );

	if ( cfg.rage_points.stomach )
		hitboxes.push_back( ( int )HITBOX_STOMACH );

	if ( cfg.rage_points.pelvis )
		hitboxes.push_back( ( int )HITBOX_PELVIS );

	if ( cfg.rage_points.arms )
	{
		hitboxes.push_back( ( int )HITBOX_LEFT_HAND );
		hitboxes.push_back( ( int )HITBOX_RIGHT_HAND );
		hitboxes.push_back( ( int )HITBOX_LEFT_FOREARM );
		hitboxes.push_back( ( int )HITBOX_RIGHT_FOREARM );
		hitboxes.push_back( ( int )HITBOX_LEFT_UPPER_ARM );
		hitboxes.push_back( ( int )HITBOX_RIGHT_UPPER_ARM );
	}

	if ( cfg.rage_points.legs )
	{
		hitboxes.push_back( ( int )HITBOX_LEFT_CALF );
		hitboxes.push_back( ( int )HITBOX_RIGHT_CALF );
	}

	if ( cfg.rage_points.feet ) {
		hitboxes.push_back( ( int )HITBOX_LEFT_FOOT );
		hitboxes.push_back( ( int )HITBOX_RIGHT_FOOT );
	}
	
	return hitboxes;
}

std::vector<Vector> rage::generate_points( m_entity* entity, int iHitbox, matrix3x4_t BoneMatrix[ 128 ] )
{
	std::vector<Vector> vPoints;

	if ( !entity )
		return vPoints;

	studiohdr_t* pStudioModel = g_csgo.modelinfo( )->GetStudiomodel( entity->GetModel( ) );
	mstudiohitboxset_t* set = pStudioModel->GetHitboxSet( 0 );

	if ( !set )
		return vPoints;

	mstudiobbox_t* untransformedBox = set->GetHitbox( iHitbox );
	if ( !untransformedBox )
		return vPoints;

	Vector vecMin = { 0, 0, 0 };
	math::vec_transform( untransformedBox->bbmin, BoneMatrix[ untransformedBox->bone ], vecMin );

	Vector vecMax = { 0, 0, 0 };
	math::vec_transform( untransformedBox->bbmax, BoneMatrix[ untransformedBox->bone ], vecMax );

	float mod = untransformedBox->m_flRadius != -1.f ? untransformedBox->m_flRadius : 0.f;
	Vector max;
	Vector min;

	float ps = 0.75f;

	if ( entity->m_vecVelocity( ).Length( ) > 300.f && iHitbox > 0 )
		ps = 0.f;
	else {
		if ( iHitbox <= ( int )HITBOX_NECK )
			ps = cfg.rage_points.head_scale / 100.f;
		else if ( iHitbox <= ( int )HITBOX_RIGHT_THIGH )
			ps = cfg.rage_points.body_scale / 100.f;
	}

	math::vector_transform( vec3_t( untransformedBox->bbmax.x + mod, untransformedBox->bbmax.y + mod, untransformedBox->bbmax.z + mod ), BoneMatrix[ untransformedBox->bone ], max );
	math::vector_transform( vec3_t( untransformedBox->bbmin.x - mod, untransformedBox->bbmin.y - mod, untransformedBox->bbmin.z - mod ), BoneMatrix[ untransformedBox->bone ], min );

	auto center = ( min + max ) * 0.5f;
	if ( ps <= 0.05f ) {
		vPoints.push_back( center );
		return vPoints;
	}

	auto clamp_shit = [ ]( float val, float min, float max ) {
		if ( val < min )
			return min;
		if ( val > max )
			return max;
		return val;
	};

	Vector curAngles = math::calculate_angle( center, csgo::m_local->get_eye_pos( ) );
	Vector forward;

	AngleVectors( curAngles, forward );

	Vector right = forward.CrossProduct( Vector( 0, 0, 1 ) );
	Vector left = Vector( -right.x, -right.y, right.z );

	if ( iHitbox == 0 ) 
	{
		for ( auto i = 0; i < 4; ++i )
			vPoints.push_back( center );

		vPoints[ 1 ].x += untransformedBox->m_flRadius * clamp_shit( 0.f, ps - 0.2f, 0.87f );
		vPoints[ 2 ].x -= untransformedBox->m_flRadius * clamp_shit( 0.f, ps - 0.2f, 0.87f );
		vPoints[ 3 ].z += untransformedBox->m_flRadius * ps - 0.05f;
	}
	else if ( iHitbox == ( int )HITBOX_NECK )
		vPoints.push_back( center );
	else if ( iHitbox == ( int )HITBOX_RIGHT_THIGH || iHitbox == ( int )HITBOX_LEFT_THIGH || iHitbox == ( int )HITBOX_RIGHT_CALF || iHitbox == ( int )HITBOX_LEFT_CALF || iHitbox == ( int )HITBOX_LEFT_FOOT ||iHitbox == ( int )HITBOX_RIGHT_FOOT ) 
	{
		if ( iHitbox == ( int )HITBOX_RIGHT_THIGH || iHitbox == ( int )HITBOX_LEFT_THIGH ) 
		{
			vPoints.push_back( center );
		}
		else if ( iHitbox == ( int )HITBOX_RIGHT_CALF || iHitbox == ( int )HITBOX_LEFT_CALF ) 
		{
			vPoints.push_back( center );
		}
		else if ( iHitbox == ( int )HITBOX_RIGHT_FOOT || iHitbox == ( int )HITBOX_LEFT_FOOT ) 
		{
			vPoints.push_back( center );
			vPoints[ 0 ].z += 5.f;
		}
	}
	else if ( iHitbox == ( int )HITBOX_RIGHT_HAND || iHitbox == ( int )HITBOX_LEFT_HAND || iHitbox == ( int )HITBOX_LEFT_UPPER_ARM || iHitbox == ( int )HITBOX_RIGHT_UPPER_ARM || iHitbox == ( int )HITBOX_LEFT_FOREARM ||iHitbox == ( int )HITBOX_RIGHT_FOREARM ) {
		vPoints.push_back( center );
	}
	else 
	{
		for ( auto i = 0; i < 3; ++i )
			vPoints.push_back( center );

		vPoints[ 1 ] += right * ( untransformedBox->m_flRadius * ps );
		vPoints[ 2 ] += left * ( untransformedBox->m_flRadius * ps );
	}

	return vPoints;
}

bool rage::spread_limit( )
{
	auto inaccuracy = csgo::m_local->m_hActiveWeapon( )->inaccuracy( );

	if ( !inaccuracy )
		inaccuracy = 0.0000001f;

	inaccuracy = 1.0f / inaccuracy;

	auto chance = cfg.rage_accuracy.hitchance * 1.5f;

	if ( !chance )
		chance = 150.0f;

	return inaccuracy > chance;
}

bool rage::can_hit( vec3_t start, vec3_t end, animation* record, int box, bool in_shot, matrix3x4_t* bones )
{
	if ( !record || !record->player )
		return false;

	const auto backup_origin = record->player->m_vecOrigin( );
	const auto backup_abs_origin = record->player->get_abs_origin( );
	const auto backup_abs_angles = record->player->get_abs_angles( );
	const auto backup_obb_mins = record->player->get_mins( );
	const auto backup_obb_maxs = record->player->get_maxs( );
	const auto backup_cache = record->player->m_CachedBoneData( ).Base( );

	auto matrix = bones;

	if ( in_shot )
		matrix = bones;

	if ( !matrix )
		return false;

	const model_t* model = record->player->GetModel( );
	if ( !model )
		return false;

	studiohdr_t* hdr = g_csgo.modelinfo( )->GetStudiomodel( model );
	if ( !hdr )
		return false;

	mstudiohitboxset_t* set = hdr->GetHitboxSet( record->player->m_nHitboxSet( ) );
	if ( !set )
		return false;

	mstudiobbox_t* bbox = set->GetHitbox( box );
	if ( !bbox )
		return false;

	vec3_t min, max;
	const auto IsCapsule = bbox->m_flRadius != -1.f;

	if ( IsCapsule ) {
		math::vec_transform( bbox->bbmin, matrix[ bbox->bone ], min );
		math::vec_transform( bbox->bbmax, matrix[ bbox->bone ], max );
		const auto dist = math::segment_to_segment( start, end, min, max );

		if ( dist < bbox->m_flRadius ) {
			return true;
		}
	}
	else {
		CGameTrace tr;

		record->player->m_vecOrigin( ) = record->origin;
		record->player->set_abs_origin( record->abs_origin );
		record->player->set_abs_angles( record->abs_ang );
		record->player->get_mins( ) = record->obb_mins;
		record->player->get_maxs( ) = record->obb_maxs;
		record->player->set_bone_cache( matrix );

		g_csgo.trace( )->ClipRayToEntity( Ray_t( start, end ), MASK_SHOT, record->player, &tr );

		record->player->m_vecOrigin( ) = backup_origin;
		record->player->set_abs_origin( backup_abs_origin );
		record->player->set_abs_angles( backup_abs_angles );
		record->player->get_mins( ) = backup_obb_mins;
		record->player->get_maxs( ) = backup_obb_maxs;
		record->player->set_bone_cache( backup_cache );

		if ( tr.hit_entity == record->player && valid_hitgroup( tr.hitgroup ) )
			return true;
	}

	return false;
}

bool rage::valid_hitgroup( int index ) 
{
	if ( ( index >= HITGROUP_HEAD && index <= HITGROUP_RIGHTLEG ) || index == HITGROUP_GEAR )
		return true;

	return false;
}

Vector rage::get_point( m_entity* pBaseEntity, int iHitbox, matrix3x4_t BoneMatrix[ 128 ] )
{
	std::vector<Vector> vPoints;

	if ( !pBaseEntity )
		return Vector( 0, 0, 0 );

	studiohdr_t* pStudioModel = g_csgo.modelinfo( )->GetStudiomodel( pBaseEntity->GetModel( ) );
	mstudiohitboxset_t* set = pStudioModel->GetHitboxSet( 0 );

	if ( !set )
		return Vector( 0, 0, 0 );

	mstudiobbox_t* untransformedBox = set->GetHitbox( iHitbox );
	if ( !untransformedBox )
		return Vector( 0, 0, 0 );

	Vector vecMin = { 0, 0, 0 };
	math::vector_transform( untransformedBox->bbmin, BoneMatrix[ untransformedBox->bone ], vecMin );

	Vector vecMax = { 0, 0, 0 };
	math::vector_transform( untransformedBox->bbmax, BoneMatrix[ untransformedBox->bone ], vecMax );

	float mod = untransformedBox->m_flRadius != -1.f ? untransformedBox->m_flRadius : 0.f;
	Vector max;
	Vector min;

	math::vector_transform( vec3_t( untransformedBox->bbmax.x + mod, untransformedBox->bbmax.y + mod, untransformedBox->bbmax.z + mod ), BoneMatrix[ untransformedBox->bone ], max );
	math::vector_transform( vec3_t( untransformedBox->bbmin.x - mod, untransformedBox->bbmin.y - mod, untransformedBox->bbmin.z - mod ), BoneMatrix[ untransformedBox->bone ], min );

	return ( min + max ) * 0.5f;
}

Vector rage::primary_scan( animation* anims, int& hitbox, float& simtime, float& best_damage, float min_dmg )
{
	memcpy( old_matrix, anims->bones, sizeof( matrix3x4_t[ 128 ] ) );
	simtime = anims->sim_time;
	set_anims( anims );

	best_damage = -1;
	const auto damage = min_dmg;
	auto best_point = Vector( 0, 0, 0 );
	auto health = anims->player->m_iHealth( );

	if ( min_dmg > health )
		min_dmg = health + 1;

	static const std::vector<int> hitboxes = {
		( int )HITBOX_HEAD,
		( int )HITBOX_CHEST,
		( int )HITBOX_STOMACH,
		( int )HITBOX_PELVIS,
		( int )HITBOX_RIGHT_CALF,
		( int )HITBOX_LEFT_CALF,
	};

	for ( auto HitboxID : hitboxes )
	{
		auto point = get_point( anims->player, HitboxID, old_matrix );
		bool is_safepoint = can_hit( csgo::m_local->get_eye_pos( ), point, anims, HitboxID, true, old_matrix );

		if ( cfg.rage.force_safepoint && !is_safepoint )
			continue;

		auto info = autowall::get( ).Think( point, anims->player );
		if ( ( info.m_damage > min_dmg && info.m_damage > best_damage ) || info.m_damage > health )
		{
			hitbox = HitboxID;
			best_point = point;
			best_damage = info.m_damage;
		}
	}

	restore_anims( anims );
	return best_point;
}

Vector rage::head_scan( animation* anims, int& hitbox, float& best_damage, float min_dmg ) 
{
	Vector best_point = Vector( 0, 0, 0 );
	memcpy( old_matrix, anims->bones, sizeof( matrix3x4_t[ 128 ] ) );
	set_anims( anims );

	int health = anims->player->m_iHealth( );
	if ( min_dmg > health )
		min_dmg = health + 1;

	std::vector<Vector> Points;
	generate_points( anims->player, 0, old_matrix );

	for ( auto HitBox : Points ) {

		bool is_safepoint = can_hit( csgo::m_local->get_eye_pos( ), HitBox, anims, 0, true, old_matrix );

		if ( cfg.rage.force_safepoint && !is_safepoint )
			continue;

		auto info = autowall::get( ).Think( HitBox, anims->player );
		if ( info.m_damage > min_dmg && info.m_damage > best_damage )
		{
			hitbox = 0;
			best_point = HitBox;
			best_damage = info.m_damage;
		}
	}

	restore_anims( anims );
	return best_point;
}

void rage::backup_anim( animation* anims )
{
	auto i = anims->player->EntIndex( );
	backup_anims[ i ].origin = anims->player->m_vecOrigin( );
	backup_anims[ i ].abs_origin = anims->player->get_abs_origin( );
	backup_anims[ i ].obb_mins = anims->player->get_mins( );
	backup_anims[ i ].obb_maxs = anims->player->get_maxs( );
	backup_anims[ i ].bone_cache = anims->player->m_CachedBoneData( ).Base( );
}

void rage::restore_anims( animation* anims )
{
	auto i = anims->player->EntIndex( );
	anims->player->m_vecOrigin( ) = backup_anims[ i ].origin;
	anims->player->set_abs_origin( backup_anims[ i ].abs_origin );
	anims->player->get_mins( ) = backup_anims[ i ].obb_mins;
	anims->player->get_maxs( ) = backup_anims[ i ].obb_maxs;
	anims->player->set_bone_cache( backup_anims[ i ].bone_cache );
}

void rage::set_anims( animation* anims )
{
	anims->player->m_vecOrigin( ) = anims->origin;
	anims->player->set_abs_origin( anims->abs_origin );
	anims->player->get_mins( ) = anims->obb_mins;
	anims->player->get_maxs( ) = anims->obb_maxs;
	anims->player->set_bone_cache( anims->bones );
}

Vector rage::get_aim_vector( m_entity* pTarget, float& simtime, Vector& origin, animation*& best_anims, int& hitbox )
{
	if ( hitboxes_to_scan( pTarget ).size( ) == 0 )
		return Vector( 0, 0, 0 );

	float m_damage = cfg.rage_accuracy.min_damage;
	
	auto latest_animation = animations::get( ).get_latest_animation( pTarget );
	auto record = latest_animation;

	if ( !record.has_value( ) || !record.value( )->player )
		return Vector( 0, 0, 0 );

	backup_anim( record.value( ) );

	if ( !cfg.rage_target.position_ad ) {
		if ( record.has_value( ) )
		{
			float damage = -1.f;
			best_anims = record.value( );

			return full_scan( record.value( ), hitbox, simtime, damage, m_damage );
		}
	}

	if ( cfg.rage_target.on_shot ) 
	{
		record = animations::get( ).get_latest_firing_animation( pTarget );
		if ( record.has_value( ) && record.value( )->player ) {
			float damage = -1.f;
			best_anims = record.value( );
			simtime = record.value( )->sim_time;
			Vector backshoot = head_scan( record.value( ), hitbox, damage, m_damage );
			if ( backshoot != Vector( 0, 0, 0 ) )
				return backshoot;
		}

	}
	
	auto oldest_animation = animations::get( ).get_oldest_animation( pTarget );
	Vector latest_origin = Vector( 0, 0, 0 );
	float best_damage_0 = -1.f, best_damage_1 = -1.f;

	record = latest_animation;
	if ( record.has_value( ) )
	{
		latest_origin = record.value( )->origin;
		float damage = -1.f;
		Vector full = primary_scan( record.value( ), hitbox, simtime, damage, m_damage );
		if ( full != Vector( 0, 0, 0 ) )
			best_damage_0 = damage;
	}

	record = oldest_animation;

	if ( record.has_value( ) && record.value( )->resolver == latest_animation.value( )->resolver )
	{
		float damage = -1.f;
		Vector full = primary_scan( record.value( ), hitbox, simtime, damage, m_damage );
		if ( full != Vector( 0, 0, 0 ) )
			best_damage_1 = damage;
	}

	if ( best_damage_0 >= best_damage_1 )
		record = latest_animation;
	else
		record = oldest_animation;

	if ( record.has_value( ) )
	{
		float damage = -1.f;
		best_anims = record.value( );
		return full_scan( record.value( ), hitbox, simtime, damage, m_damage );
	}

	return Vector( 0, 0, 0 );
}

Vector rage::full_scan( animation* anims, int& hitbox, float& simtime, float& best_damage, float min_dmg ) 
{
	memcpy( old_matrix, anims->bones, sizeof( matrix3x4_t[ 128 ] ) );
	simtime = anims->sim_time;
	best_damage = -1;
	Vector best_point = Vector( 0, 0, 0 );
	set_anims( anims );
	
	int health = anims->player->m_iHealth( );

	if ( min_dmg > health )
		min_dmg = health + 1;

	auto hitboxes = hitboxes_to_scan( anims->player );

	static const std::vector<int> upper_hitboxes = {
		( int )HITBOX_HEAD,
		( int )HITBOX_NECK,
		( int )HITBOX_UPPER_CHEST,
		( int )HITBOX_CHEST,
	};
	static const std::vector<int> baim_hitboxes = {
		( int )HITBOX_CHEST,
		( int )HITBOX_STOMACH,
		( int )HITBOX_PELVIS,
	};

	bool baim_if_lethal = cfg.rage.baim_lefthal;

	if ( baim_if_lethal || cfg.rage.adaptive_baim ) {
		for ( auto HitboxID : baim_hitboxes ) {
			std::vector<Vector> Points = generate_points( anims->player, HitboxID, old_matrix );
			for ( int k = 0; k < Points.size( ); k++ )
			{
				bool is_safepoint = can_hit( csgo::m_local->get_eye_pos( ), Points[ k ], anims, HitboxID, true, old_matrix );

				if ( cfg.rage.force_safepoint && !is_safepoint )
					continue;

				auto info = autowall::get( ).Think( Points[ k ], anims->player );
				if ( ( info.m_damage > min_dmg && info.m_damage > best_damage ) )
				{
					hitbox = HitboxID;
					best_point = Points[ k ];
					best_damage = info.m_damage;
				}
			}
		}

		if ( baim_if_lethal && best_damage > health + 2 ) {
			restore_anims( anims );
			return best_point;
		}

		if ( best_damage > 0 && cfg.rage.adaptive_baim ) {
			
			restore_anims( anims );
			return best_point;
		}
	}

	for ( auto HitboxID : hitboxes )
	{
		std::vector<Vector> Points = generate_points( anims->player, HitboxID, old_matrix );
		for ( int k = 0; k < Points.size( ); k++ )
		{
			bool is_safepoint = can_hit( csgo::m_local->get_eye_pos( ), Points[ k ], anims, HitboxID, true, old_matrix );

			if ( cfg.rage.force_safepoint && !is_safepoint )
				continue;

			auto info = autowall::get( ).Think( Points[ k ], anims->player );
			if ( ( info.m_damage > min_dmg && info.m_damage > best_damage ) )
			{
				hitbox = HitboxID;
				best_point = Points[ k ];
				best_damage = info.m_damage;
			}
		}
	}

	restore_anims( anims );
	return best_point;
}

int rage::priority_hitbox( m_entity* pEntity )
{
	auto weapon = csgo::m_local->m_hActiveWeapon( );

	if ( !weapon )
		return -1;

	if ( !csgo::m_local->is_alive( ) )
		return -1;

	if ( !pEntity->is_alive( ) )
		return -1;

	if ( cfg.rage.baim_air & 1 && !( pEntity->m_fFlags( ) & FL_ONGROUND ) )
		return ( int )HITBOX_PELVIS;

	if ( cfg.rage.baim_hp && pEntity->m_iHealth( ) <= cfg.rage.hp )
		return ( int )HITBOX_PELVIS;

	return 0;
}
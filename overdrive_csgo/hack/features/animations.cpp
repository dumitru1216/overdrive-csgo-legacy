#include "animations.h"
#include "antihit.h"

float calculate_lerp( )
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

void Extrapolate( m_entity* player, Vector& origin, Vector& velocity, int& flags, bool on_ground )
{
	static const auto sv_gravity = g_csgo.cvar( )->FindVar( "sv_gravity" );
	static const auto sv_jump_impulse = g_csgo.cvar( )->FindVar( "sv_jump_impulse" );

	if ( !( flags & FL_ONGROUND ) )
		velocity.z -= ticks_to_time( sv_gravity->GetFloat( ) );
	else if ( player->m_fFlags( ) & FL_ONGROUND && !on_ground )
		velocity.z = sv_jump_impulse->GetFloat( );

	const auto src = origin;
	auto end = src + velocity * g_csgo.global_vars( )->m_interval_per_tick;

	Ray_t r;
	r.Init( src, end, player->get_mins( ), player->get_maxs( ) );

	trace_t t;
	CTraceFilter filter;
	filter.pSkip = player;

	g_csgo.trace( )->TraceRay( r, MASK_PLAYERSOLID, &filter, &t );

	if ( t.fraction != 1.f )
	{
		for ( auto i = 0; i < 2; i++ )
		{
			velocity -= t.plane.normal * velocity.Dot( t.plane.normal );

			const auto dot = velocity.Dot( t.plane.normal );
			if ( dot < 0.f )
				velocity -= Vector( dot * t.plane.normal.x,
				dot * t.plane.normal.y, dot * t.plane.normal.z );

			end = t.endpos + velocity * ticks_to_time( 1.f - t.fraction );

			r.Init( t.endpos, end, player->get_mins( ), player->get_maxs( ) );
			g_csgo.trace( )->TraceRay( r, MASK_PLAYERSOLID, &filter, &t );

			if ( t.fraction == 1.f )
				break;
		}
	}

	origin = end = t.endpos;
	end.z -= 2.f;

	r.Init( origin, end, player->get_mins( ), player->get_maxs( ) );
	g_csgo.trace( )->TraceRay( r, MASK_PLAYERSOLID, &filter, &t );

	flags &= ~FL_ONGROUND;

	if ( t.DidHit( ) && t.plane.normal.z > .7f )
		flags |= FL_ONGROUND;
}

bool animation::is_valid( float range = .2f, float max_unlag = .2f )
{
	if ( !g_csgo.engine( )->GetNetChannelInfo( ) || !valid )
		return false;

	const auto correct = std::clamp( g_csgo.engine( )->GetNetChannelInfo( )->GetLatency( FLOW_INCOMING )
		+ g_csgo.engine( )->GetNetChannelInfo( )->GetLatency( FLOW_OUTGOING )
		+ calculate_lerp( ), 0.f, max_unlag );

	return fabsf( correct - ( g_csgo.global_vars( )->m_curtime - sim_time ) ) < range && correct < 1.f;
}

animation::animation( m_entity* player )
{
	const auto weapon = player->m_hActiveWeapon( );

	this->player = player;
	index = player->EntIndex( );
	dormant = player->IsDormant( );
	velocity = player->m_vecVelocity( );
	origin = player->m_vecOrigin( );
	abs_origin = player->get_abs_origin( );
	obb_mins = player->get_mins( );
	obb_maxs = player->get_maxs( );
	memcpy( layers, player->get_animlayers( ), sizeof( AnimationLayer ) * 13 );
	poses = player->m_flPoseParameter( );
	anim_state = player->get_animation_state( );
	sim_time = player->m_flSimulationTime( );
	interp_time = 0.f;
	last_shot_time = weapon ? weapon->last_shot_time( ) : 0.f;
	duck = player->m_flDuckAmount( );
	lby = player->m_lby( );
	eye_angles = player->m_angEyeAngles( );
	abs_ang = player->get_abs_angles( );
	flags = player->m_fFlags( );
	eflags = player->m_iEFlags( );
	effects = player->m_fEffects( );
	lag = time_to_ticks( player->m_flSimulationTime( ) - player->m_flOldSimulationTime( ) );
	valid = lag >= 0 && lag <= 17;
	lag = std::clamp( lag, 0, 17 );
}

animation::animation( m_entity* player, Vector last_reliable_angle ) : animation( player )
{
	this->last_reliable_angle = last_reliable_angle;
}

void animation::restore( m_entity* player ) const
{
	player->m_fFlags( ) = flags;
	player->m_flDuckAmount( ) = duck;
	memcpy( player->get_animlayers( ), layers, sizeof( AnimationLayer ) * 13 );
	player->m_vecOrigin( ) = origin;
	player->set_abs_origin( abs_origin );
}

void animation::apply( m_entity* player ) const
{
	player->m_angEyeAngles( ) = eye_angles;
	player->m_flDuckAmount( ) = duck;
	player->m_fFlags( ) = flags;
	player->m_vecOrigin( ) = origin;
	player->set_abs_origin( abs_origin );
}

void animation::build_server_bones( m_entity* player )
{
	const auto backup_occlusion_flags = player->occlusion_flags( );
	const auto backup_occlusion_framecount = player->occlusion_framecount( );

	player->occlusion_flags( ) = 0;
	player->occlusion_framecount( ) = 0;

	player->m_nReadebleBones( ) = player->m_nWritableBones( ) = 0;
	player->invalidate_bone_cache( );

	player->m_fEffects( ) |= 8;

	const auto backup_bone = player->m_nBoneArrayForWrite( );
	player->m_nBoneArrayForWrite( ) = bones;

	csgo::m_allow_call = true;
	player->SetupBones( nullptr, -1, 0x7FF00, sim_time );
	csgo::m_allow_call = false;

	player->m_nBoneArrayForWrite( ) = backup_bone;
	
	player->occlusion_flags( ) = backup_occlusion_flags;
	player->occlusion_framecount( ) = backup_occlusion_framecount;

	player->m_fEffects( ) &= ~8;
}

void animations::animation_info::update_animations( animation* record, animation* from )
{
	
	if ( !from )
	{
		// set velocity and layers.
		record->velocity = player->m_vecVelocity( );

		// fix feet spin.
		record->anim_state->m_flFeetYawRate = 0.f;

		// apply record.
		record->apply( player );

		// run update.
		return animations::get( ).update_player( player );
	}

	const auto new_velocity = player->m_vecVelocity( );

	// restore old record.
	memcpy( player->get_animlayers( ), from->layers, sizeof( AnimationLayer ) * 13 );
	player->set_abs_origin( record->origin );
	player->set_abs_angles( from->abs_ang );
	player->m_vecVelocity( ) = from->velocity;

	// setup velocity.
	record->velocity = new_velocity;

	// setup extrapolation parameters.
	auto old_origin = from->origin;
	auto old_flags = from->flags;

	for ( auto i = 0; i < record->lag; i++ )
	{
		// move time forward.
		const auto time = from->sim_time + ticks_to_time( i + 1 );
		const auto lerp = 1.f - ( record->sim_time - time ) / ( record->sim_time - from->sim_time );

		/*player->GetDuckAmount() = Interpolate2(from->duck, record->duck, lerp);*/

		// resolve player.
		if ( record->lag - 1 == i )
		{
			player->m_vecVelocity( ) = new_velocity;
			player->m_fFlags( ) = record->flags;
		}
		else // compute velocity and flags.
		{
			Extrapolate( player, old_origin, player->m_vecVelocity( ), player->m_fFlags( ), old_flags & FL_ONGROUND );
			old_flags = player->m_fFlags( );
		}

		// backup simtime.
		const auto backup_simtime = player->m_flSimulationTime( );

		// set new simtime.
		player->m_flSimulationTime( ) = time;

		// run update.
		animations::get( ).update_player( player );

		// restore old simtime.
		player->m_flSimulationTime( ) = backup_simtime;
	}

	if ( !record->dormant && !from->dormant )
		record->didshot = record->last_shot_time > from->sim_time && record->last_shot_time <= record->sim_time;
}

void animations::UpdatePlayers( )
{
	if ( !g_csgo.engine( )->IsInGame( ) )
		return;

	const auto local = csgo::m_local;

	for ( auto it = animation_infos.begin( ); it != animation_infos.end( );) {
		auto player = reinterpret_cast< m_entity* >( g_csgo.entitylist( )->GetClientEntityFromHandle( it->first ) );

		if ( !player || player != it->second.player || !player->is_alive( )
			|| !local )
		{
			if ( player )
				player->m_bClientSideAnimation( true );

			it = animation_infos.erase( it );
		}
		else
			it = next( it );
	}

	if ( !local )
	{
		for ( auto i = 1; i <= g_csgo.engine( )->GetMaxClients( ); ++i ) {
			const auto entity = reinterpret_cast< m_entity* >( g_csgo.entitylist( )->GetClientEntity( i ) );
			if ( entity && entity->is_player( ) )
				entity->m_bClientSideAnimation( true );
		}
	}

	for ( auto i = 1; i <= g_csgo.engine( )->GetMaxClients( ); ++i ) {
		const auto entity = reinterpret_cast< m_entity* >( g_csgo.entitylist( )->GetClientEntity( i ) );
		if ( !entity || !entity->is_player( ) )
			continue;
		if ( !entity->is_alive( ) || entity->IsDormant( ) )
			continue;

		if ( entity == local )
			continue;

		if ( entity != local && entity->m_iTeamNum( ) == local->m_iTeamNum( ) ) {
			entity->m_bClientSideAnimation( true );
			continue;
		}

		if ( animation_infos.find( entity->GetRefEHandle( ) ) == animation_infos.end( ) )
			animation_infos.insert_or_assign( entity->GetRefEHandle( ), animation_info( entity, {} ) );
	}

	// run post update
	for ( auto& info : animation_infos )
	{
		auto& _animation = info.second;
		const auto player = _animation.player;

		// erase frames out-of-range
		for ( int i = 0; i < _animation.frames.size( ); i++ )
			if ( !_animation.frames[ i ].is_valid( 0.45f, 0.2f ) )
				_animation.frames.erase( _animation.frames.begin( ) + i );

		// resolver->Do( _animation.player );

		// have we already seen this update?
		if ( player->m_flSimulationTime( ) == player->m_flOldSimulationTime( ) )
			continue;

		// reset animstate
		if ( _animation.last_spawn_time != player->m_flSpawnTime( ) )
		{
			const auto state = player->get_animation_state( );
			if ( state )
				player->reset_animation_state( state );

			_animation.last_spawn_time = player->m_flSpawnTime( );
		}

		// grab weapon
		const auto weapon = player->m_hActiveWeapon( );

		// make a full backup of the player
		auto backup = animation ( player );
		backup.apply( player );

		// grab previous
		animation* previous = nullptr;

		if ( !_animation.frames.empty( ) && !_animation.frames.front( ).dormant )
			previous = &_animation.frames.front( );

		const auto shot = weapon && previous && weapon->last_shot_time( ) > previous->sim_time
			&& weapon->m_fLastShotTime( ) <= player->m_flSimulationTime( );

		if ( !shot )
			info.second.last_reliable_angle = player->m_angEyeAngles( );

		// store server record
		auto& record = _animation.frames.emplace_front( player, info.second.last_reliable_angle );

		// run full update
		_animation.update_animations( &record, previous );

		// restore correctly synced values
		backup.restore( player );

		// use uninterpolated data to generate our bone matrix
		record.build_server_bones( player );
	}
}

void animations::update_player( m_entity* pEnt )
{
	float curtime = g_csgo.global_vars( )->m_curtime;
	float frametime = g_csgo.global_vars( )->m_frametime;
	auto IEFlags = pEnt->m_iEFlags( );

	float FeetCycle = pEnt->get_animation_state( )->m_flFeetCycle;
	float FeetRate = pEnt->get_animation_state( )->m_flFeetYawRate;

	g_csgo.global_vars( )->m_curtime = pEnt->m_flSimulationTime( );
	g_csgo.global_vars( )->m_frametime = g_csgo.global_vars( )->m_interval_per_tick;
	pEnt->m_iEFlags( ) &= ~0x1000;
	pEnt->m_vecAbsVelocity( ) = pEnt->m_vecVelocity( );
	pEnt->m_vecVelocity( ).z = 0.0f;

	if ( pEnt->get_animation_state( )->m_iLastClientSideAnimationUpdateFramecount == g_csgo.global_vars( )->m_framecount )
		pEnt->get_animation_state( )->m_iLastClientSideAnimationUpdateFramecount = g_csgo.global_vars( )->m_framecount - 1;

	pEnt->get_animation_state( )->m_flFeetCycle = pEnt->get_animation_state( )->m_flFeetYawRate;
	pEnt->get_animation_state( )->m_flFeetYawRate = 0.0f;

	csgo::m_update[ pEnt->EntIndex( ) ] = true;
	pEnt->m_bClientSideAnimation( true );

	pEnt->update_client_side_animation( );
	pEnt->set_abs_angles( Vector( 0, pEnt->get_animation_state( )->m_flGoalFeetYaw, 0 ) );
	save_data( pEnt, &m_Data[ pEnt->EntIndex( ) ] );

	pEnt->m_bClientSideAnimation( false );
	csgo::m_update[ pEnt->EntIndex( ) ] = false;

	g_csgo.global_vars( )->m_curtime = curtime;
	g_csgo.global_vars( )->m_frametime = frametime;
	pEnt->m_iEFlags( ) = IEFlags;
}

animations::animation_info* animations::get_animation_info( m_entity* player )
{
	auto info = animation_infos.find( player->GetRefEHandle( ) );

	if ( info == animation_infos.end( ) )
		return nullptr;

	return &info->second;
}

bool animation::is_valid_extended( )
{
	if ( !g_csgo.engine( )->GetNetChannelInfo( ) || !valid )
		return false;

	const auto correct = std::clamp( g_csgo.engine( )->GetNetChannelInfo( )->GetLatency( FLOW_INCOMING )
		+ g_csgo.engine( )->GetNetChannelInfo( )->GetLatency( FLOW_OUTGOING )
		+ calculate_lerp( ), 0.f, 0.2f );

	float deltaTime = fabsf( correct - ( g_csgo.global_vars( )->m_curtime - sim_time ) );
	float ping = 0.2f;
	
	return deltaTime < ping&& deltaTime >= ping - .2f;
}

std::optional<animation*> animations::get_latest_animation( m_entity* player )
{
	const auto info = animation_infos.find( player->GetRefEHandle( ) );

	if ( info == animation_infos.end( ) || info->second.frames.empty( ) )
		return std::nullopt;

	for ( auto it = info->second.frames.begin( ); it != info->second.frames.end( ); it = next( it ) ) {
		if ( ( *it ).is_valid_extended( ) ) {
			if ( time_to_ticks( fabsf( ( *it ).sim_time - player->m_flSimulationTime( ) ) ) < 25 )
				return &*it;
		}
	}

	return std::nullopt;
}

std::optional<animation*> animations::get_oldest_animation( m_entity* player )
{
	const auto info = animation_infos.find( player->GetRefEHandle( ) );

	if ( info == animation_infos.end( ) || info->second.frames.empty( ) )
		return std::nullopt;

	for ( auto it = info->second.frames.rbegin( ); it != info->second.frames.rend( ); it = next( it ) ) {
		if ( ( *it ).is_valid_extended( ) ) {
			return &*it;
		}
	}

	return std::nullopt;
}

std::vector<animation*> animations::get_valid_animations( m_entity* player, const float range )
{
	std::vector<animation*> result;

	const auto info = animation_infos.find( player->GetRefEHandle( ) );

	if ( info == animation_infos.end( ) || info->second.frames.empty( ) )
		return result;

	result.reserve( static_cast< int >( std::ceil( range * .2f / g_csgo.global_vars( )->m_interval_per_tick ) ) );

	for ( auto it = info->second.frames.begin( ); it != info->second.frames.end( ); it = next( it ) )
		if ( ( *it ).is_valid( range * .2f ) )
			result.push_back( &*it );

	return result;
}

std::optional<animation*> animations::get_latest_firing_animation( m_entity* player )
{
	const auto info = animation_infos.find( player->GetRefEHandle( ) );

	if ( info == animation_infos.end( ) || info->second.frames.empty( ) )
		return std::nullopt;

	for ( auto it = info->second.frames.begin( ); it != info->second.frames.end( ); it = next( it ) )
		if ( ( *it ).is_valid_extended( ) && ( *it ).didshot )
			return &*it;

	return std::nullopt;
}

void animations::local_animations( )
{
	if ( !csgo::m_local->is_alive( ) )
		return;

	if ( !csgo::m_cmd )
		return;

	const auto backup_frametime = g_csgo.global_vars( )->m_frametime;
	const auto backup_curtime = g_csgo.global_vars( )->m_curtime;

	g_csgo.global_vars( )->m_frametime = g_csgo.global_vars( )->m_interval_per_tick;
	g_csgo.global_vars( )->m_curtime = csgo::m_local->m_flSimulationTime( );

	csgo::m_local->m_iEFlags( ) &= ~0x1000;
	csgo::m_local->m_vecAbsVelocity( ) = csgo::m_local->m_vecVelocity( );

	/* fix onground animation */
	if ( csgo::m_local->m_fFlags( ) & FL_ONGROUND ) {
		csgo::m_local->get_animation_state( )->m_bInHitGroundAnimation = false;
		csgo::m_local->get_animation_state( )->m_bOnGround = true;
		csgo::m_local->get_animation_state( )->m_flFeetYawRate = 0.0f;
	}

	csgo::m_local->invalidate_bone_cache( );
	csgo::m_update[ g_csgo.engine( )->GetLocalPlayer( ) ] = true;

	csgo::m_local->m_bClientSideAnimation( true );
	csgo::m_local->update_animation_state( csgo::m_local->get_animation_state( ), csgo::m_fake );
	csgo::m_local->update_client_side_animation( );

	csgo::m_update[ g_csgo.engine( )->GetLocalPlayer( ) ] = false;

	/* fix unlag */
	if ( !g_csgo.clientstate( )->chokedcommands ) {
		csgo::m_allow_setup_bones = true;
		get_local_matrix = csgo::m_local->SetupBones( real_matrix, 128, BONE_USED_BY_ANYTHING, csgo::m_local->m_flSimulationTime( ) );
		csgo::m_allow_setup_bones = false;

		if ( get_local_matrix ) {
			for ( int i = 0; i < 128; i++ )
				origin_local[ i ] = csgo::m_local->get_abs_origin( ) - real_matrix[ i ].GetOrigin( );
		}
	}

	g_csgo.global_vars( )->m_curtime = backup_curtime;
	g_csgo.global_vars( )->m_frametime = backup_frametime;
}

void animations::fake_animations( )
{
	static float m_flSpawnTime = csgo::m_local->m_flSpawnTime( );
	static bool inited = false;

	if ( m_flSpawnTime != csgo::m_local->m_flSpawnTime( ) )
	{
		m_flSpawnTime = csgo::m_local->m_flSpawnTime( );
		inited = false;
	}

	if ( !inited )
	{
		m_nState = static_cast< CCSGOPlayerAnimState* >( g_csgo.mem_alloc( )->alloc( sizeof( CCSGOPlayerAnimState ) ) );

		if ( m_nState != nullptr )
			csgo::m_local->create_animation_state( m_nState );

		inited = true;
	}

	if ( !m_nState )
		return;

	csgo::m_local->m_fEffects( ) |= 8;

	csgo::m_local->invalidate_bone_cache( );

	AnimationLayer backup_layers[ 13 ];
	if ( csgo::m_local->m_flOldSimulationTime( ) != csgo::m_local->m_flSimulationTime( ) )
	{
		std::memcpy( backup_layers, csgo::m_local->get_animlayers( ),
			( sizeof( AnimationLayer ) * csgo::m_local->num_overlays( ) ) );

		csgo::m_local->update_animation_state( m_nState, csgo::m_real );
		csgo::m_local->set_abs_angles( Vector( 0, m_nState->m_flGoalFeetYaw, 0 ) );

		csgo::m_allow_setup_bones = true;
		csgo::m_local->SetupBones( fake_matrix, 128, BONE_USED_BY_ANYTHING, g_csgo.global_vars( )->m_curtime );
		csgo::m_allow_setup_bones = false;

		for ( auto i = 0; i < 128; i++ )
		{
			fake_matrix[ i ][ 0 ][ 3 ] -= csgo::m_local->GetRenderOrigin( ).x;
			fake_matrix[ i ][ 1 ][ 3 ] -= csgo::m_local->GetRenderOrigin( ).y;
			fake_matrix[ i ][ 2 ][ 3 ] -= csgo::m_local->GetRenderOrigin( ).z;
		}

		std::memcpy( csgo::m_local->get_animlayers( ), backup_layers,
			( sizeof( AnimationLayer ) * csgo::m_local->num_overlays( ) ) );
	}

	csgo::m_local->m_fEffects( ) &= ~8;
}

void animations::save_data( m_entity* pEnt, entity_data* data )
{
	data->m_flFeetYawCycle = pEnt->get_animation_state( )->m_flFeetCycle;
	data->m_flFeetYawRate = pEnt->get_animation_state( )->m_flFeetYawRate;
	data->m_flLowerBodyYaw = pEnt->m_lby( );
	data->m_iEFlags = pEnt->m_iEFlags( );
	data->m_iFlags = pEnt->m_fFlags( );
	memcpy( data->Layers, pEnt->get_animlayers( ), sizeof( AnimationLayer ) * pEnt->num_overlays( ) );
	data->m_bValidData = true;
}

void animations::restore_ent_data( m_entity* pEnt, entity_data* data )
{
	pEnt->get_animation_state( )->m_flFeetCycle = data->m_flFeetYawCycle;
	pEnt->get_animation_state( )->m_flFeetYawRate = data->m_flFeetYawRate;
	pEnt->m_lby( ) = data->m_flLowerBodyYaw;
	pEnt->m_iEFlags( ) = data->m_iEFlags;
	pEnt->m_fFlags( ) = data->m_iFlags;
	memcpy( pEnt->get_animlayers( ), data->Layers, sizeof( AnimationLayer ) * pEnt->num_overlays( ) );
	data->m_bValidData = false;
}

void animations::save_globals( globals_data* data )
{
	data->flCurtime = g_csgo.global_vars( )->m_curtime;
	data->flFrametime = g_csgo.global_vars( )->m_frametime;
	data->flAbsFrametime = g_csgo.global_vars( )->m_absoluteframetime;
	data->iFramecount = g_csgo.global_vars( )->m_framecount;
	data->iTickscount = g_csgo.global_vars( )->m_tickcount;
	data->flInterpAmount = g_csgo.global_vars( )->m_interpolation_amount;
}

void animations::restore_globals( globals_data* data )
{
	g_csgo.global_vars( )->m_curtime = data->flCurtime;
	g_csgo.global_vars( )->m_frametime = data->flFrametime;
	g_csgo.global_vars( )->m_absoluteframetime = data->flAbsFrametime;
	g_csgo.global_vars( )->m_framecount = data->iFramecount;
	g_csgo.global_vars( )->m_tickcount = data->iTickscount;
	g_csgo.global_vars( )->m_interpolation_amount = data->flInterpAmount;
}

void animations::update_globals( m_entity* pEnt )
{
	float m_flSimulationTime = pEnt->m_flSimulationTime( );
	int m_iNextSimulationTick = m_flSimulationTime / g_csgo.global_vars( )->m_interval_per_tick + 1;

	g_csgo.global_vars( )->m_curtime = m_flSimulationTime;
	g_csgo.global_vars( )->m_realtime = m_flSimulationTime;
	g_csgo.global_vars( )->m_frametime = g_csgo.global_vars( )->m_interval_per_tick;
	g_csgo.global_vars( )->m_absoluteframetime = g_csgo.global_vars( )->m_interval_per_tick;
	g_csgo.global_vars( )->m_framecount = m_iNextSimulationTick;
	g_csgo.global_vars( )->m_tickcount = m_iNextSimulationTick;
	g_csgo.global_vars( )->m_interpolation_amount = 0;
}

void animations::main( m_entity* pEnt )
{
	auto m_nData = m_Data[ pEnt->EntIndex( ) ];

	globals_data m_nGlobalsData = *reinterpret_cast< globals_data* >( g_csgo.mem_alloc( )->alloc( sizeof( globals_data ) ) );
	entity_data m_nEntityData = *reinterpret_cast< entity_data* >( g_csgo.mem_alloc( )->alloc( sizeof( entity_data ) ) );

	if ( !m_nData.m_bValidData )
		return;

	save_globals( &m_nGlobalsData );
	save_data( pEnt, &m_nEntityData );

	update_globals( pEnt );
	restore_ent_data( pEnt, &m_nData );

	if ( pEnt->m_fFlags( ) & FL_ONGROUND )
	{
		pEnt->get_animation_state( )->m_bOnGround = true;
		pEnt->get_animation_state( )->m_bInHitGroundAnimation = false;
	}

	*( float* )( uintptr_t( pEnt->get_animation_state( ) ) + 0x110 ) = 0.0f;
	pEnt->get_animation_state( )->m_flFeetYawRate = 0.0f;

	if ( pEnt->get_animation_state( )->m_iLastClientSideAnimationUpdateFramecount == g_csgo.global_vars( )->m_framecount )
		pEnt->get_animation_state( )->m_iLastClientSideAnimationUpdateFramecount = g_csgo.global_vars( )->m_framecount - 1;

	update( pEnt, pEnt->m_flSimulationTime( ) );

	restore_ent_data( pEnt, &m_nEntityData );
	restore_globals( &m_nGlobalsData );
}

void animations::update( m_entity* pEnt, float m_flSimulationTime )
{
	float curtime = g_csgo.global_vars( )->m_curtime;
	float frametime = g_csgo.global_vars( )->m_frametime;
	float interp = g_csgo.global_vars( )->m_interpolation_amount;

	auto IEFlags = pEnt->m_iEFlags( );

	float FeetCycle = pEnt->get_animation_state( )->m_flFeetCycle;
	float FeetRate = pEnt->get_animation_state( )->m_flFeetYawRate;

	g_csgo.global_vars( )->m_curtime = m_flSimulationTime;
	g_csgo.global_vars( )->m_frametime = g_csgo.global_vars( )->m_interval_per_tick;
	g_csgo.global_vars( )->m_interpolation_amount = 0.0f;

	pEnt->m_iEFlags( ) &= ~0x1000;
	pEnt->m_vecAbsVelocity( ) = pEnt->m_vecVelocity( );
	pEnt->m_vecVelocity( ).z = 0.0f;

	if ( pEnt->get_animation_state( )->m_iLastClientSideAnimationUpdateFramecount == g_csgo.global_vars( )->m_framecount )
		pEnt->get_animation_state( )->m_iLastClientSideAnimationUpdateFramecount = g_csgo.global_vars( )->m_framecount - 1;

	pEnt->get_animation_state( )->m_flFeetCycle = pEnt->get_animation_state( )->m_flFeetYawRate;
	pEnt->get_animation_state( )->m_flFeetYawRate = 0.0f;

	csgo::m_update[ pEnt->EntIndex( ) ] = true;
	pEnt->m_bClientSideAnimation( true );

	pEnt->update_animation_state( pEnt->get_animation_state( ), pEnt->m_angEyeAngles( ) );
	pEnt->set_abs_angles( Vector( 0, pEnt->get_animation_state( )->m_flGoalFeetYaw, 0 ) );

	pEnt->m_bClientSideAnimation( false );
	csgo::m_update[ pEnt->EntIndex( ) ] = false;

	pEnt->InvalidatePhysicsRecursive( 8 );
	
	g_csgo.global_vars( )->m_curtime = curtime;
	g_csgo.global_vars( )->m_frametime = frametime;
	g_csgo.global_vars( )->m_interpolation_amount = interp;

	pEnt->m_iEFlags( ) = IEFlags;
	pEnt->get_animation_state( )->m_flFeetCycle = FeetCycle;
	pEnt->get_animation_state( )->m_flFeetYawRate = FeetRate;
}

void animations::on_create( )
{
	for ( int i = 1; i <= g_csgo.global_vars( )->m_maxclients; i++ )
	{
		m_entity* pEnt = ( m_entity* )g_csgo.entitylist( )->GetClientEntity( i );

		if ( !pEnt || !pEnt->is_alive( ) || !pEnt->is_player( ) || pEnt == csgo::m_local || pEnt->m_iTeamNum( )== csgo::m_local->m_iTeamNum( ) )
			continue;

		main( pEnt );
	}
}
#include "resolver.hpp"
c_resolver::c_resolver( ) { } // constructor. -> its initialized

// there is the resolver.
void c_resolver::run( m_entity* player ) {
	int i = player->EntIndex( ); // player index.
	static vec3_t angle; // enemy resolvable angle.
	angle = player->m_angEyeAngles( ); // get enemy angles.

		// entity speed.
	auto speed = player->m_vecVelocity( ).Length2D( );
	auto flags = player->m_fFlags( );
	auto delta = player->m_flSimulationTime( ) - moving_sim[ i ];

	// setup stored lby.
	if ( stored_lby[ i ] != player->m_lby( ) ) {
		old_lby[ i ] = stored_lby[ i ];
		lby_changes[ i ] = true;
		stored_lby[ i ] = player->m_lby( );
	}

	// lby delta setup.
	lby_delta[ i ] = math::normalize_yaw( stored_lby[ i ] - old_lby[ i ] );

	// setup moving.
	if ( speed > 0.1f && flags & FL_ONGROUND ) {
		moving_sim[ i ] = player->m_flSimulationTime( ); // setup moving simulation
	}

	// setup if lby changed.
	if ( lby_changes[ i ] ) {
		if ( delta < .22f ) { // it may be > .22f
			angle.y = lby_delta[ i ]; // angle.y to lby_delta
		}

		lby_changes[ i ] = false;
	}

	// lby update
	if ( stored_lby[ i ] != player->m_lby( ) ) {
		old_lby[ i ] = stored_lby[ i ];
		angle.y = player->m_lby( );
		lby_changes[ i ] = true; // player updates
		stored_lby[ i ] = player->m_lby( );

		// resolve stage.
		resolve_stage[ i ] = "LBU";
	}
	else if ( speed > 0.1f && flags & FL_ONGROUND ) { // moving resolver
		angle.y = player->m_lby( );
		moving_lby[ i ] = player->m_lby( );
		moving_sim[ i ] = player->m_flOldSimulationTime( );
		lby_changes[ i ] = false; // player dont update it

		// resolve stage.
		resolve_stage[ i ] = "MOVING";
	}
	else if ( flags & FL_ONGROUND ) {
		auto lby_delta = player->m_lby( );
		float angle_lby = 0.0f;

		if ( lby_delta < 85.f && lby_delta > -85 ) {
			angle_lby = lby_delta;
		}
		else if ( lby_delta > 85.f ) {
			if ( lby_delta < 180.f )
				angle_lby = 110.f;
		}
		else if ( lby_delta < 85.f ) {
			if ( lby_delta > -180.f ) {
				if ( lby_delta < -85.f )
					angle_lby = -110.f;
				else
					angle_lby = lby_delta;
			}
		}

		// pandora resolving.
		angle.y = angle_lby;

		// resolve stage.
		resolve_stage[ i ] = "STANDING"; // IM NOT SURE WHY THIS GET INVALID DATA
	}
	else if ( !( flags & FL_ONGROUND ) ) {
		angle.y = player->m_lby( ); // thats best pp100

		// resolve stage.
		resolve_stage[ i ] = "AIR";
	}

}
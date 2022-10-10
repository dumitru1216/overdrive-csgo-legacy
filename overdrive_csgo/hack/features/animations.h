#pragma once

#include "../../utils/singleton.h"
#include "../sdk/entity/entity.h"
#include "../globals.h"
#include "../../utils/math.h"
#include "../../utils/utl.h"

#include <unordered_map>
#include <optional>
#include <deque>
#include <algorithm>
#include <array>

struct animation
{
	animation( ) = default;
	explicit animation( m_entity* player );
	explicit animation( m_entity* player, Vector last_reliable_angle );
	void restore( m_entity* player ) const;
	void apply( m_entity* player ) const;
	void build_server_bones( m_entity* player );
	bool is_valid( float range, float max_unlag );
	bool is_valid_extended( );

	m_entity* player{};
	int32_t index{};

	bool valid{}, has_anim_state{};
	alignas( 16 ) matrix3x4_t bones[ 128 ];

	bool dormant{};
	Vector velocity;
	Vector origin;
	matrix3x4_t* bone_cache;
	Vector abs_origin;
	Vector obb_mins;
	Vector obb_maxs;
	AnimationLayer layers[ 13 ];
	std::array<float, 24> poses;
	CCSGOPlayerAnimState* anim_state;
	float anim_time{};
	float sim_time{};
	float interp_time{};
	float duck{};
	float lby{};
	float last_shot_time{};
	Vector last_reliable_angle{};
	Vector eye_angles;
	Vector abs_ang;
	int flags{};
	int eflags{};
	int effects{};
	float m_flFeetCycle{};
	float m_flFeetYawRate{};
	int lag{};
	bool didshot;
	std::string resolver;
};

struct player_animation_t
{
	float m_last_simtime = 0.f;
	float m_spawntime = 0.f;
	bool m_init = false;
	std::array< AnimationLayer, 13 > m_layers{ };
	std::array< float, 20 > m_poses{ };
	CCSGOPlayerAnimState* m_state = nullptr;
	bool m_got_matrix = false;
	matrix3x4_t m_matrix[ 128 ]{};
	bool m_should_recalculate_matrix = false;
};

struct setup_bones_data_t
{
	matrix3x4_t matrix[ 128 ];
	bool got = false;
	float simtime = 0.f;
	float yaw = 0.f;
	Vector pos = Vector( 0, 0, 0 );
	int last_bonemask = 0;
	float spawntime = 0.f;
	CCSGOPlayerAnimState* new_state = nullptr;
	bool init_animstate = false;
	int last_tickcount = -1;
	float simtime_state = 0.f;
	AnimationLayer m_real_layers[ 13 ];
	float m_real_poses[ 20 ];
	float goal_feet_yaw = 0.f;
};

struct globals_data
{
	float flCurtime;
	float flFrametime;
	float flAbsFrametime;
	int iFramecount;
	int iTickscount;
	float flInterpAmount;
};

struct entity_data
{
	float m_flFeetYawRate;
	float m_flFeetYawCycle;
	float m_flLowerBodyYaw;
	float m_flEyeYaw;
	float m_flDuckamount;
	int m_iFlags;
	int m_iEFlags;
	AnimationLayer Layers[ 15 ];
	bool m_bValidData = false;
};


class animations : public singleton <animations> {

public:

	void local_animations( );
	void fake_animations( );
	void update_bones( );
	bool get_local_matrix;
	CCSGOPlayerAnimState* m_nState;
	matrix3x4_t real_matrix[ 128 ];
	matrix3x4_t fake_matrix[ 128 ];
	Vector origin_local[ 128 ];

private:

	struct animation_info {
		animation_info( m_entity* player, std::deque<animation> animations )
			: player( player ), frames( std::move( animations ) ), last_spawn_time( 0 ) { }

		void update_animations( animation* to, animation* from );

		m_entity* player{};
		std::deque<animation> frames;

		// last time this player spawned
		float last_spawn_time;
		float goal_feet_yaw;
		Vector last_reliable_angle;
	};

	std::unordered_map<BaseHandle, animation_info> animation_infos;

public:
	
	void UpdatePlayers( );
	void update_player( m_entity* player );

	animation_info* get_animation_info( m_entity* player );
	std::optional<animation*> get_latest_animation( m_entity* player );
	std::optional<animation*> get_oldest_animation( m_entity* player );
	std::vector<animation*> get_valid_animations( m_entity* player, float range = 1.f );
	std::optional<animation*> get_latest_firing_animation( m_entity* player );

public:

	std::array< player_animation_t, 64 > m_player_states;
public:

	entity_data m_Data[ 65 ];
	void save_data( m_entity* pEnt, entity_data* data );
	void restore_ent_data( m_entity* pEnt, entity_data* data );
	void save_globals( globals_data* data );
	void restore_globals( globals_data* data );
	void update_globals( m_entity* pEnt );
	void main( m_entity* pEnt );
	void update( m_entity* pEnt, float m_flSimulationTime );
	void on_create( );
};
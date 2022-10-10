#include "animations.h"
#include "../sdk/other/e_nums.h"

class rage : public singleton <rage>
{
public:

	void on_create_move( CUserCmd* m_pcmd );
	void scan( );
	Vector get_point( m_entity* pBaseEntity, int iHitbox, matrix3x4_t BoneMatrix[ 128 ] );
	bool spread_limit( );
	bool valid_hitgroup( int index );
	std::vector<Vector> generate_points( m_entity* entity, int hitbox_id, matrix3x4_t BoneMatrix[ 128 ] );
	bool head_valid( m_entity* e );
	Vector head_scan( animation* anims, int& hitbox, float& best_damage, float min_dmg );
	bool can_hit( Vector start, Vector end, animation* record, int box, bool in_shot, matrix3x4_t* bones );
	Vector primary_scan( animation* anims, int& hitbox, float& simtime, float& best_damage, float min_dmg );
	int priority_hitbox( m_entity* pEntity );

	void restore_anims( animation* anims );
	void set_anims( animation* anims );
	void backup_anim( animation* anims );
	Vector get_aim_vector( m_entity* pTarget, float& simtime, Vector& origin, animation*& best_anims, int& hitbox );
	Vector full_scan( animation* anims, int& hitbox, float& simtime, float& best_damage, float min_dmg );
	std::vector<int> hitboxes_to_scan( m_entity* pEntity );
	void drop_target( );

private:

	matrix3x4_t old_matrix[ 128 ];
	animation backup_anims[ 65 ];

private:

	int target_index = -1;
	float best_distance;
	bool did_dt;
	bool aimbotted_in_current_tick;
	bool fired_in_that_tick;
	float current_aim_simulationtime;
	int current_minusticks;
	Vector current_aim_position;
	bool lby_backtrack;
	Vector current_aim_player_origin;
	bool shot;
	bool Shooting;
	Vector Target;
	bool hitchanced;
	bool fired;
	Vector Angles;
	bool last_tick_shooted;
	bool target_lethal;
	bool Shooted[ 65 ];
	bool Hitted[ 65 ];
	bool HeadAiming;
	animation* target_anims;
	Vector last_shot_angle;

};
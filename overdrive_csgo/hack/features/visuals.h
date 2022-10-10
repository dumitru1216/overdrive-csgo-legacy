#pragma once
#include "../../render/render.h"
class visuals : public singleton <visuals>
{

	struct
	{
		int left, top, right, bottom;
	}bbox;

public:

	void init( );
	void bound_box( m_entity* e );

	void box( m_entity* e );
	void hp( m_entity* e );
	void ammo( m_entity* e );
	void name( m_entity* e );
	void skeleton( m_entity* e );
	void oof_arrows( m_entity* player );
	void rotate( std::array< Vector2D, 3 >& points, float rotation );
	void weapon( m_entity* e );
	void weapon_icon( m_entity* e );
	void sky_box( );
	void modulate_world( );
public:

	void autowall_crosshair( m_entity* e );
	void third_person( );
	void util_trace_hull( const Vector& vec_start, const Vector& vec_end, const unsigned int n_mask, const Vector& extents, trace_t* p_trace );
	void post_render( );
	void remove_smoke( );

public:

	struct entity_box_info_t
	{
		int x, y, w, h, alpha, alpha_anim;
		m_entity* ent;
	};

	entity_box_info_t infos[ 4096 ];
	bool bound_box_weapon( m_entity* m_entity, entity_box_info_t& box, bool dynamic );
};
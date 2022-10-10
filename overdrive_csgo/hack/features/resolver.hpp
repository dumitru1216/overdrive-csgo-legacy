#pragma once
#include "../../utils/singleton.h"
#include "../sdk/entity/entity.h"
#include "../globals.h"
#include "../../utils/math.h"
#include "../../utils/utl.h"
#include "../sdk/other/e_nums.h"

#include <unordered_map>
#include <optional>
#include <deque>
#include <algorithm>
#include <array>

class c_resolver {
public:
	c_resolver( );
	static c_resolver& get( ) {
		c_resolver inst;
		return inst;
	}
public:
	void run( m_entity* player );
	void create_move( );
public:
	// resolver vars.
	float stored_lby[ 65 ];
	float old_lby[ 65 ];
	bool lby_changes[ 65 ];
	float lby_delta[ 65 ];
	float moving_sim[ 65 ];
	float moving_lby[ 65 ];

	// resolver string.
	std::string resolve_stage[ 64 ];
};
#pragma once

#include "../../utils/singleton.h"
#include "../sdk/entity/entity.h"
#include "../globals.h"
#include "../../utils/math.h"
#include "../../utils/utl.h"
#include "../../hack/sdk/other/e_nums.h"


class antiaim : public singleton < antiaim >
{
public:

	void run( CUserCmd* cmd );
	float at_target( );
	float get_curtime( CUserCmd* ucmd );
	bool next_lby_update( const float yaw_to_break, CUserCmd* cmd );

};
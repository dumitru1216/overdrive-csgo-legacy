#pragma once

#include "../../utils/singleton.h"
#include "../sdk/entity/entity.h"
#include "../globals.h"
#include "../../utils/math.h"
#include "../../utils/utl.h"
#include <algorithm>

class movement : public singleton<movement>
{
public:

	void fix_movement( CUserCmd* cmd );
	void run( Vector org );

};
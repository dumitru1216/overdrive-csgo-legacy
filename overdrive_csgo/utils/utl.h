#pragma once
#include "../hack/sdk/src_kit.h"

namespace utils
{
	unsigned long pattern_scan( const char* module_name, const char* signature );
	unsigned int find_in_datamap( datamap_t *pMap, const char *name );
	bool world_to_screen( const Vector& origin, Vector2D& screen );
	bool get_key( int key, int style );
};
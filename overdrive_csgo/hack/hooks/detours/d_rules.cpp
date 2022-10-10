#include "dhooks.h"

#include "../../sdk/entity/entity.h"
#include "../../globals.h"

dhooks::standard_blending_rules_t dhooks::origin_standard_blending_rules;

void _fastcall dhooks::standard_blending_rules( m_entity* player, uint32_t, studiohdr_t* hdr, Vector* pos, Quaternion* q, const float time, int mask )
{
	if ( !player || m_inUnhook )
		return origin_standard_blending_rules( player, hdr, pos, q, time, mask );

	*( int* )( ( DWORD )player + 0x2698 ) = 0;
	mask |= 0x200;

	player->m_fEffects( ) |= 8;
	origin_standard_blending_rules( player, hdr, pos, q, time, mask );
	player->m_fEffects( ) &= ~8;
}
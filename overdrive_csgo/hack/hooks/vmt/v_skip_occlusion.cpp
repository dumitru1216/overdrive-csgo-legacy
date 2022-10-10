#include "vhooks.h"

#include "../../sdk/entity/entity.h"
#include "../../../utils/utl.h"
#include "../../globals.h"

#include "../../../gui/config/config.h"
#include <intrin.h>

struct renderable_info_t {
	iclient_renderable* renderable;
	void* alpha_property;
	int enum_count;
	int render_frame;
	unsigned short first_shadow;
	unsigned short leaf_list;
	short area;
	std::uint16_t flags;
	std::uint16_t flags2;
	Vector bloated_abs_mins;
	Vector bloated_abs_maxs;
	Vector abs_mins;
	Vector abs_maxs;
	int pad;
};

int __fastcall hooks::list_leaves_in_box( std::uintptr_t ecx, std::uintptr_t edx, Vector& mins, Vector& maxs, unsigned short* list, int list_max ) {
	static auto original_fn = hooks::bsp.get_original< decltype( &list_leaves_in_box ) >( indexes::m_list );

	static auto insert_into_tree_call_list_leaves_in_box = utils::pattern_scan( "client.dll", "89 44 24 14 EB 08 C7 44 24 ? ? ? ? ? 8B 45" );

	if ( reinterpret_cast< std::uintptr_t >( _ReturnAddress( ) ) != insert_into_tree_call_list_leaves_in_box || !cfg.chams_enemy.full_chams || m_inUnhook )
		return original_fn( ecx, edx, mins, maxs, list, list_max );

	auto info = *reinterpret_cast< renderable_info_t** >( reinterpret_cast< std::uintptr_t >( _AddressOfReturnAddress( ) ) + 0x14 );

	if ( !info || !info->renderable )
		return original_fn( ecx, edx, mins, maxs, list, list_max );

	auto base_entity = info->renderable->Geticlient_unknown( )->GetBaseEntity( );

	if ( !base_entity || !base_entity->is_player( ) )
		return original_fn( ecx, edx, mins, maxs, list, list_max );

	info->flags &= ~0x100;
	info->flags2 |= 0xC0;

	constexpr float MAX_COORD_FLOAT = 16384.f;
	constexpr float MIN_COORD_FLOAT = -MAX_COORD_FLOAT;

	static Vector map_min = Vector( MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT );
	static  Vector map_max = Vector( MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT );

	return original_fn( ecx, edx, map_min, map_max, list, list_max );
}
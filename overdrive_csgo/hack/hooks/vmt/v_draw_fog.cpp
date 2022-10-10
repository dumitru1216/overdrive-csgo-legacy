#include "vhooks.h"
#include "../../../gui/config/config.h"

bool __fastcall hooks::draw_fog( void* ecx, void* edx )
{
	return !cfg.removals.remove_fog;
}
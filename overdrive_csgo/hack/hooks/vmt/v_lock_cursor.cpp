#include "vhooks.h"
#include "../../../gui/ui.h"
#include "../../globals.h"

typedef void( __fastcall* lock_cursor_t ) ( ISurface*, void* );

void __fastcall hooks::lock_cursor( ISurface* thisptr, void* edx )
{
	static auto oLockCursor = hooks::surface.get_original<lock_cursor_t>( indexes::m_lock );

	if ( !control.menu_opened || m_inUnhook )
		return oLockCursor( thisptr, edx );

	g_csgo.surface( )->unlock_cursor( );
}
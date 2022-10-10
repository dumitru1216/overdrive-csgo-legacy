#include "vhooks.h"
#include "../../../gui/ui.h"

LRESULT __stdcall hooks::wndproc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	if ( msg == WM_MOUSEMOVE ) {
		control.mouse_pos.x = GET_X_LPARAM( lparam );
		control.mouse_pos.y = GET_Y_LPARAM( lparam );
	}

	if ( ui::ui_helpers::controller::get( ).menu_opened )
	{
		if ( msg == WM_MOUSEWHEEL )
			ui::ui_helpers::controller::get( ).mouse_wheel_activity = GET_WHEEL_DELTA_WPARAM( wparam ) / WHEEL_DELTA;
	}

	return CallWindowProcA( hooks::m_wndproc, hwnd, msg, wparam, lparam );
}
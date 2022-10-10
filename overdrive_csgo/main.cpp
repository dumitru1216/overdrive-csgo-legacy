#include <Windows.h>
#include <iostream>
#include <thread>
#include "gui/config/tgf_cfg.h"
#include "hack/sdk/interfaces/create_if.h"
#include "hack/sdk/entity/netvar.h"
#include "hack/hooks/vmt/vhooks.h"
#include "hack/globals.h"
#include "hack/hooks/detours/dhooks.h"

unsigned long __stdcall start_hack( _In_ LPVOID reserved )
{
	m_inUnhook = false;

	netvar_sys::get( ).init( );
	hooks::hook( );
	dhooks::hook( );

	while ( !m_inUnhook )
		std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

	hooks::unhook( );
	dhooks::unhook( );

	FreeLibraryAndExitThread( reinterpret_cast< HMODULE >( reserved ), 0 );
}

BOOL WINAPI DllMain( _In_ HINSTANCE instance, _In_ DWORD reason, _In_ LPVOID reserved )
{
	switch ( reason )
	{
	case DLL_PROCESS_ATTACH:
	{
		if ( auto handle = CreateThread( 0, 0, start_hack, instance, 0, 0 ) )
			CloseHandle( handle );
	} break;
	}
	return TRUE;
}
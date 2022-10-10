#include "vhooks.h"
#include <intrin.h>

#include "../../../utils/utl.h"
#include "../../globals.h"

bool __fastcall hooks::sv_cheats_get_bool( PVOID pConVar, void* edx )
{
	static auto dwCAM_Think = utils::pattern_scan( "client.dll", "85 C0 75 30 38 86" );
	static auto ofunc = hooks::sv_cheats.get_original<bool( __thiscall* )( PVOID )>( indexes::m_sv_cheats );

	if ( m_inUnhook )
		return ofunc( pConVar );

	if ( !ofunc )
		return false;

	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == static_cast< DWORD >( dwCAM_Think ) )
		return true;

	return ofunc( pConVar );
}
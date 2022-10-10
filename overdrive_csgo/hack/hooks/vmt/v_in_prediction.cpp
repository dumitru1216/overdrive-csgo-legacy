#include "vhooks.h"
#include <intrin.h>

#include "../../../utils/utl.h"
#include "../../globals.h"

using in_prediction_t = bool( __thiscall* ) ( void* );

const auto ptr_setupbones = reinterpret_cast< void* > ( utils::pattern_scan( ( "client.dll" ), ( "8B 40 ? FF D0 84 C0 74 ? F3 0F 10 05 ? ? ? ? EB ?" ) ) );

bool __fastcall hooks::in_prediction( void* p )
{
	const auto ofunc = hooks::prediction.get_original< in_prediction_t >( indexes::m_prediction );

	if ( m_inUnhook )
		return ofunc( p );

	if ( reinterpret_cast< uintptr_t > ( _ReturnAddress( ) ) == reinterpret_cast< uintptr_t > ( ptr_setupbones ) + 5 )
		return false;

	return ofunc( p );
}
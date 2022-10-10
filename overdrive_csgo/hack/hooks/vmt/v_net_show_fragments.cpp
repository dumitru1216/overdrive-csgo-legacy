#include "vhooks.h"
#include "../../globals.h"
#include <intrin.h>

bool __fastcall hooks::net_show_fragment( PVOID pConVar, void* edx )
{
	static auto _GetBool = hooks::net_fragment.get_original<bool( __thiscall* )( PVOID )>( indexes::m_net_fragment );

	if ( !g_csgo.clientstate( )->m_NetChannel || m_inUnhook )
		return _GetBool( pConVar );

	static uint32_t last_fragment = 0;

	if ( _ReturnAddress( ) == 0 && last_fragment > 0 )
	{
		const auto data = &reinterpret_cast< uint32_t* >( g_csgo.clientstate( )->m_NetChannel )[ 0x54 ];
		const auto bytes_fragments = reinterpret_cast< uint32_t* >( data )[ 0x43 ];

		if ( bytes_fragments == last_fragment )
		{
			auto& buffer = reinterpret_cast< uint32_t* >( data )[ 0x42 ];
			buffer = 0;
		}
	}

	if ( _ReturnAddress( ) == 0 )
	{
		const auto data = &reinterpret_cast< uint32_t* >( g_csgo.clientstate( )->m_NetChannel )[ 0x54 ];
		const auto bytes_fragments = reinterpret_cast< uint32_t* >( data )[ 0x43 ];

		last_fragment = bytes_fragments;
	}

	return _GetBool( pConVar );
}
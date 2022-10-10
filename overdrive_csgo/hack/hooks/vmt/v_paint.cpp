#include "vhooks.h"
#include "../../../utils/utl.h"
#include "../../../render/render.h"
#include "../../../gui/ui.h"
#include "../../features/visuals.h"
#include "../../globals.h"
#include "../../../gui/config/config.h"

typedef void( __thiscall* paint_t )( IEngineVGui*, int );

#define ExtractRed( x ) ((x>>16)&255)
#define ExtractGreen( x ) ((x>>12)&255)
#define ExtractBlue( x ) (x&255)

void __stdcall hooks::paint( int mode )
{
	static auto orig = hooks::engine_vgui.get_original<paint_t>( indexes::m_paint );

	if ( m_inUnhook )
		return orig( g_csgo.engine_gui( ), mode );

	orig( g_csgo.engine_gui( ), mode );

	typedef void( __thiscall* start_drawing )( void* );
	typedef void( __thiscall* finish_drawing )( void* );

	static start_drawing start_draw = ( start_drawing )utils::pattern_scan( "vguimatsurface.dll", "55 8B EC 83 E4 C0 83 EC 38" );
	static finish_drawing end_draw = ( finish_drawing )utils::pattern_scan( "vguimatsurface.dll", "8B 0D ? ? ? ? 56 C6 05" );

	if ( mode & 1 )
	{
		start_draw( g_csgo.surface( ) );

		if ( g_csgo.engine( )->IsConnected( ) && g_csgo.engine( )->IsInGame( ) && csgo::m_local )
		{
			visuals::get( ).init( );
			visuals::get( ).post_render( );
		}

		end_draw( g_csgo.surface( ) );
	}
}
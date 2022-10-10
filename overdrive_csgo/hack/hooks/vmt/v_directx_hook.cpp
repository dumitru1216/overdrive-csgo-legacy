#include "vhooks.h"

#include "../../../gui/ui.h"
#include "../../../gui/controls/init.h"
#include "../../globals.h"

static bool m_init = true;

using p_reset = long( __stdcall* ) ( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
long __stdcall hooks::reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp )
{
	auto original = hooks::direct3d.get_original< p_reset >( indexes::m_reset );
	if ( m_inUnhook )
		return original( device, pp );

	const auto hr = original( device, pp );
	return hr;
}
using p_present = long( __stdcall* ) ( IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA* );

long __stdcall hooks::present( IDirect3DDevice9* device, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion )
{
	static auto original = hooks::direct3d.get_original< p_present >( indexes::m_present );

	if ( m_inUnhook )
		return original( device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );

	IDirect3DStateBlock9* state = NULL;
	IDirect3DVertexDeclaration9* vertDec;
	IDirect3DVertexShader9* vertShader;

	device->CreateStateBlock( D3DSBT_PIXELSTATE, &state );
	device->GetVertexDeclaration( &vertDec );
	device->GetVertexShader( &vertShader );

	if ( draw_list == NULL )
	{
		draw_list = new render( device );

		D3DXCreateFont( device, 13, 0, 100, 4, FALSE, DEFAULT_CHARSET, 1, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma", &draw_list->fonts.main_font );
		D3DXCreateFont( device, 12, 0, 900, 4, FALSE, DEFAULT_CHARSET, 1, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma", &draw_list->fonts.second_font );

		return original( device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );
	}

	draw_list->init_device( );
	ui::c_gui::get( ).init( );

	state->Apply( );
	state->Release( );

	device->SetVertexDeclaration( vertDec );
	device->SetVertexShader( vertShader );

	return original( device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion );
}

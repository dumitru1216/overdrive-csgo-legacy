#include "vhooks.h"
#include "../../globals.h"
#include "../../../gui/config/config.h"

using paint_traverse_t = void( __thiscall* )( void*, vgui::VPANEL, bool, bool );

void __fastcall hooks::paint_traverse( void* ecx, void* edx, vgui::VPANEL panel, bool force_repaint, bool allow_force )
{
	static auto o_paint_traverse = hooks::panel.get_original<paint_traverse_t>( indexes::m_paint_traverse );
	static unsigned int panel_hud_id, panel_id;

	if ( m_inUnhook )
		return o_paint_traverse( ecx, panel, force_repaint, allow_force );

	if ( !panel_hud_id )
		if ( !strcmp( "HudZoom", g_csgo.panel( )->GetName( panel ) ) )
			panel_hud_id = panel;

	if ( cfg.removals.remove_scope && panel_hud_id == panel && csgo::m_local )
		if ( csgo::m_local->m_bIsScoped( ) )
			return;

	o_paint_traverse( ecx, panel, force_repaint, allow_force );
}
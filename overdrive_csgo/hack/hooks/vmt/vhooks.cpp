#include "vhooks.h"
#include "../../../render/render.h"

namespace hooks
{
	void hook( )
	{
		direct3d.setup( g_csgo.d3d_device( ) );
		direct3d.hook_index( indexes::m_present, hooks::present );
		direct3d.hook_index( indexes::m_reset, hooks::reset );

		ConVar* sv_cheats_con = g_csgo.cvar( )->FindVar( "sv_cheats" );
		sv_cheats.setup( sv_cheats_con );
		sv_cheats.hook_index( indexes::m_sv_cheats, hooks::sv_cheats_get_bool );

		ConVar* net_fragment_show = g_csgo.cvar( )->FindVar( "net_showfragments" );
		net_fragment.setup( net_fragment_show );
		net_fragment.hook_index( indexes::m_net_fragment, hooks::net_show_fragment );

		surface.setup( g_csgo.surface( ) );
		surface.hook_index( indexes::m_lock, hooks::lock_cursor );

		clientmode.setup( g_csgo.clientmode( ) );
		clientmode.hook_index( indexes::m_create, hooks::create_move );
		clientmode.hook_index( indexes::m_override_view, hooks::override_view );
		clientmode.hook_index( indexes::m_draw_fog, hooks::draw_fog );

		engine_vgui.setup( g_csgo.engine_gui( ) );
		engine_vgui.hook_index( indexes::m_paint, hooks::paint );

		modelrender.setup( g_csgo.modelrender( ) );
		modelrender.hook_index( indexes::m_draw_model, hooks::draw_model_execute );

		client.setup( g_csgo.client( ) );
		client.hook_index( indexes::m_stage, hooks::frame_stage_notify );

		engine.setup( g_csgo.engine( ) );
		engine.hook_index( indexes::m_is_hltv, hooks::is_hltv );

		prediction.setup( g_csgo.prediction( ) );
		//prediction.hook_index( indexes::m_prediction, hooks::in_prediction );
		prediction.hook_index( indexes::m_run_cmd, hooks::run_cmd );

		panel.setup( g_csgo.panel( ) );
		panel.hook_index( indexes::m_paint_traverse, hooks::paint_traverse );

		bsp.setup( g_csgo.engine( )->GetBSPTreeQuery( ) );
		bsp.hook_index( indexes::m_list, hooks::list_leaves_in_box );

		m_wndproc = reinterpret_cast< WNDPROC >( SetWindowLongPtr( FindWindowA( nullptr, "Counter-Strike: Global Offensive" ), GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( wndproc ) ) );

		static bool m_init = false;

		if ( !m_init )
		{
			m_init = true;

			surface::get( ).fonts.esp_other = g_csgo.surface( )->CreateFont_( );
			g_csgo.surface( )->SetFontGlyphSet( surface::get( ).fonts.esp_other, "Small Fonts", 8, 500, 0, 0, FONTFLAG_OUTLINE );

			surface::get( ).fonts.esp_font = g_csgo.surface( )->CreateFont_( );
			g_csgo.surface( )->SetFontGlyphSet( surface::get( ).fonts.esp_font, "Tahoma", 12, 900, 0, 0, FONTFLAG_DROPSHADOW );

			surface::get( ).fonts.watermark = g_csgo.surface( )->CreateFont_( );
			g_csgo.surface( )->SetFontGlyphSet( surface::get( ).fonts.watermark, "Verdana", 12, 100, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );

			surface::get( ).fonts.indicator = g_csgo.surface( )->CreateFont_( );
			g_csgo.surface( )->SetFontGlyphSet( surface::get( ).fonts.indicator, "Verdana", 25, 700, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );

			surface::get( ).fonts.weapon_font = g_csgo.surface( )->CreateFont_( );
			g_csgo.surface( )->SetFontGlyphSet( surface::get( ).fonts.weapon_font, "undefeated", 14, 300, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );
		}
	}

	void unhook( )
	{
		g_csgo.inputsys( )->enable_input( true );

		direct3d.unhook_all( );
		sv_cheats.unhook_all( );
		surface.unhook_all( );
		clientmode.unhook_all( );
		engine_vgui.unhook_all( );
		modelrender.unhook_all( );
		net_fragment.unhook_all( );
		client.unhook_all( );
		engine.unhook_all( );
		prediction.unhook_all( );
		panel.unhook_all( );
		bsp.unhook_all( );

		SetWindowLongPtr( FindWindowA( nullptr, "Counter-Strike: Global Offensive" ), GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( m_wndproc ) );
	}

	WNDPROC m_wndproc;

	vfunc_hook direct3d;
	vfunc_hook sv_cheats;
	vfunc_hook surface;
	vfunc_hook clientmode;
	vfunc_hook engine_vgui;
	vfunc_hook modelrender;
	vfunc_hook net_fragment;
	vfunc_hook client;
	vfunc_hook engine;
	vfunc_hook prediction;
	vfunc_hook panel;
	vfunc_hook bsp;
};
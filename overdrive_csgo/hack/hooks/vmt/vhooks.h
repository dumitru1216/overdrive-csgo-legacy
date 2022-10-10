#pragma once

#include "../vmt.h"
#include "../../sdk/interfaces/create_if.h"

namespace hooks
{
	extern WNDPROC m_wndproc;
	LRESULT _stdcall wndproc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

	extern vfunc_hook direct3d;
	extern vfunc_hook sv_cheats;
	extern vfunc_hook surface;
	extern vfunc_hook clientmode;
	extern vfunc_hook client;
	extern vfunc_hook engine_vgui;
	extern vfunc_hook modelrender;
	extern vfunc_hook net_fragment;
	extern vfunc_hook engine;
	extern vfunc_hook prediction;
	extern vfunc_hook panel;
	extern vfunc_hook bsp;

	void hook( );
	void unhook( );

	long __stdcall reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp );
	long __stdcall present( IDirect3DDevice9* device, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion );
	bool __fastcall sv_cheats_get_bool( PVOID pConVar, void* edx );
	void __fastcall lock_cursor( ISurface*, void* );
	bool __stdcall create_move( float, CUserCmd* );
	void __stdcall paint( int mode );
	void __stdcall draw_model_execute( IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pinfo, matrix3x4_t* custom_bone_to_world );
	bool __fastcall net_show_fragment( PVOID pConVar, void* edx );
	void __stdcall frame_stage_notify( client_frame_stage_t stage );
	bool __fastcall is_hltv( );
	bool __fastcall in_prediction( void* p );
	void __fastcall override_view( void* ecx, void* edx, CViewSetup* pSetup );
	void __fastcall paint_traverse( void* ecx, void* edx, vgui::VPANEL panel, bool force_repaint, bool allow_force );
	int __fastcall list_leaves_in_box( std::uintptr_t ecx, std::uintptr_t edx, Vector& mins, Vector& maxs, unsigned short* list, int list_max );
	void __fastcall run_cmd( void* ecx, void*, m_entity* player, CUserCmd* ucmd, IMoveHelper* helper );
	bool __fastcall draw_fog( void* ecx, void* edx );

	enum indexes {
		m_reset = 16,
		m_present = 17,
		m_sv_cheats = 13,
		m_lock = 67,
		m_create = 24,
		m_paint = 14,
		m_draw_model = 21,
		m_net_fragment = 13,
		m_stage = 36,
		m_is_hltv = 93,
		m_prediction = 14,
		m_override_view = 18,
		m_paint_traverse = 41,
		m_list = 6,
		m_run_cmd = 19,
		m_draw_fog = 17
	};
};
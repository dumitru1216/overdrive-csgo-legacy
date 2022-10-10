#pragma once

#include "../src_kit.h"

class InterfaceReg {
private:
	using InstantiateInterfaceFn = void* ( *)( );
public:
	InstantiateInterfaceFn m_CreateFn;
	const char* m_pName;
	InterfaceReg* m_pNext;
};

class context
{
public:

	IDirect3DDevice9* d3d_device( );
	IVEngineClient* engine( );
	IInputSystem* inputsys( );
	i_base_client* client( );
	iclient_entityList* entitylist( );
	ICvar* cvar( );
	IEngineTrace* trace( );
	IVRenderView* renderview( );
	IMDLCache* modelcache( );
	IVModelRender* modelrender( );
	IMaterialSystem* materialsystem( );
	IPanel* panel( );
	IVModelInfoClient* modelinfo( );
	IPrediction* prediction( );
	IGameMovement* gamemovement( );
	ISurface* surface( );
	IVDebugOverlay* debugoverlay( );
	IPhysicsSurfaceProps* physsurface( );
	IGameEventManager2* eventmanager( );
	IClientMode* clientmode( );
	CGlobalVarsBase* global_vars( );
	CGlowObjectManager* glow( );
	CClientState* clientstate( );
	IMoveHelper* movehelper( );
	CInput* input( );
	i_mem_alloc* mem_alloc( );
	IEngineVGui* engine_gui( );
	DWORD post_process( );

private:

	template<typename T>
	T* get( const char* mod_name, const char* interface_name, bool exact = false ) {
		T* iface = nullptr;
		InterfaceReg* register_list;
		int part_match_len = strlen( interface_name );

		DWORD interface_fn = reinterpret_cast< DWORD >( GetProcAddress( GetModuleHandleA( mod_name ), "CreateInterface" ) );

		if ( !interface_fn ) {
			return nullptr;
		}

		unsigned int jump_start = ( unsigned int )( interface_fn )+4;
		unsigned int jump_target = jump_start + *( unsigned int* )( jump_start + 1 ) + 5;

		register_list = **reinterpret_cast< InterfaceReg*** >( jump_target + 6 );

		for ( InterfaceReg* cur = register_list; cur; cur = cur->m_pNext ) {
			if ( exact == true ) {
				if ( strcmp( cur->m_pName, interface_name ) == 0 )
					iface = reinterpret_cast< T* >( cur->m_CreateFn( ) );
			}
			else {
				if ( !strncmp( cur->m_pName, interface_name, part_match_len ) && std::atoi( cur->m_pName + part_match_len ) > 0 )
					iface = reinterpret_cast< T* >( cur->m_CreateFn( ) );
			}
		}
		return iface;
	}

};

extern context g_csgo;
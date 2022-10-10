#include "create_if.h"
#include "../../../utils/utl.h"

context g_csgo;

IDirect3DDevice9* context::d3d_device( )
{
	static IDirect3DDevice9* dev = nullptr;

	if ( !dev ) // �������������� 1 ���
		dev = **reinterpret_cast< IDirect3DDevice9*** > ( utils::pattern_scan( "shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C" ) + 1 );

	return dev;
}

IVEngineClient* context::engine( )
{
	static IVEngineClient* eng = nullptr;

	if ( !eng ) // �������������� 1 ���
		eng = this->get<IVEngineClient>( "engine.dll", "VEngineClient" );

	return eng;
}

IInputSystem* context::inputsys( ) 
{
	static IInputSystem* input = nullptr;

	if ( !input ) // �������������� 1 ���
		input = this->get<IInputSystem>( "inputsystem.dll", "InputSystemVersion" );

	return input;
}

i_base_client* context::client( )
{
	static i_base_client* clt = nullptr;

	if ( !clt ) // �������������� 1 ���
		clt = this->get<i_base_client>( "client.dll", "VClient0" );

	return clt;
}

iclient_entityList* context::entitylist( )
{
	static iclient_entityList* list = nullptr;

	if ( !list ) // �������������� 1 ���
		list = this->get<iclient_entityList>( "client.dll", "VClientEntityList" );

	return list;
}

ICvar* context::cvar( )
{
	static ICvar* cv = nullptr;
	
	if ( !cv ) // �������������� 1 ���
		cv = this->get<ICvar>( "vstdlib.dll", "VEngineCvar" );

	return cv;
}

IEngineTrace* context::trace( )
{
	static IEngineTrace* trc = nullptr;

	if ( !trc ) // �������������� 1 ���
		trc = this->get<IEngineTrace>( "engine.dll", "EngineTraceClient" );

	return trc;
}

IVRenderView* context::renderview( )
{
	static IVRenderView* view = nullptr;

	if ( !view ) // �������������� 1 ���
		view = this->get<IVRenderView>( "engine.dll", "VEngineRenderView" );

	return view;
}

IMDLCache* context::modelcache( )
{
	static IMDLCache* mdl = nullptr;

	if ( !mdl ) // �������������� 1 ���
		mdl = this->get<IMDLCache>( "datacache.dll", "MDLCache" );

	return mdl;
}

IVModelRender* context::modelrender( )
{
	static IVModelRender* mdl = nullptr;

	if ( !mdl ) // �������������� 1 ���
		mdl = this->get<IVModelRender>( "engine.dll", "VEngineModel" );

	return mdl;
}

IMaterialSystem* context::materialsystem( )
{
	static IMaterialSystem* mat = nullptr;
	
	if ( !mat ) // �������������� 1 ���
		mat = this->get<IMaterialSystem>( "materialsystem.dll", "VMaterialSystem" );

	return mat;
}

IPanel* context::panel( )
{
	static IPanel* pnl = nullptr;

	if ( !pnl ) // �������������� 1 ���
		pnl = this->get<IPanel>( "vgui2.dll", "VGUI_Panel" );

	return pnl;
}

IVModelInfoClient* context::modelinfo( )
{
	static IVModelInfoClient* mdl_info = nullptr;

	if ( !mdl_info ) // �������������� 1 ���
		mdl_info = this->get<IVModelInfoClient>( "engine.dll", "VModelInfoClient" );

	return mdl_info;
}

IPrediction* context::prediction( )
{
	static IPrediction* pred = nullptr;

	if ( !pred ) // �������������� 1 ���
		pred = this->get<IPrediction>( "client.dll", "VClientPrediction" );

	return pred;
}

IGameMovement* context::gamemovement( )
{
	static IGameMovement* move = nullptr;

	if ( !move ) // �������������� 1 ���
		move = this->get<IGameMovement>( "client.dll", "GameMovement" );

	return move;
}

ISurface* context::surface( )
{
	static ISurface* surf = nullptr;

	if ( !surf ) // �������������� 1 ���
		surf = this->get<ISurface>( "vguimatsurface.dll", "VGUI_Surface" );

	return surf;
}

IVDebugOverlay* context::debugoverlay( )
{
	static IVDebugOverlay* dbg = nullptr;

	if ( !dbg ) // �������������� 1 ���
		dbg = this->get<IVDebugOverlay>( "engine.dll", "VDebugOverlay" );

	return dbg;
}

IPhysicsSurfaceProps* context::physsurface( )
{
	static IPhysicsSurfaceProps* phys = nullptr;

	if ( !phys ) // �������������� 1 ���
		phys = this->get<IPhysicsSurfaceProps>( "vphysics.dll", "VPhysicsSurfaceProps" );

	return phys;
}

IGameEventManager2* context::eventmanager( )
{
	static IGameEventManager2* evt = nullptr;

	if ( !evt ) // �������������� 1 ���
		evt = this->get<IGameEventManager2>( "engine.dll", "GAMEEVENTSMANAGER002", true );

	return evt;
}

IClientMode* context::clientmode( )
{
	static IClientMode* mode = nullptr;

	if ( !mode ) // �������������� 1 ���
		mode = **reinterpret_cast< IClientMode*** >( ( *( DWORD** )( client( ) ) )[ 10 ] + 0x5 );

	return mode;
}

CGlobalVarsBase* context::global_vars( )
{
	static CGlobalVarsBase* globals = nullptr;

	if ( !globals ) // �������������� 1 ���
		globals = **reinterpret_cast< CGlobalVarsBase*** >( ( *( DWORD** )( client( ) ) )[ 0 ] + 0x1B );

	return globals;
}

CGlowObjectManager* context::glow( )
{
	static CGlowObjectManager* glw = nullptr;

	if ( !glw ) // �������������� 1 ���
		glw = *reinterpret_cast< CGlowObjectManager** >( utils::pattern_scan( "client.dll", "0F 11 05 ? ? ? ? 83 C8 01" ) + 3 );

	return glw;
}

CClientState* context::clientstate( )
{
	static CClientState* state = nullptr;

	if ( !state ) // �������������� 1 ���
		state = **reinterpret_cast< CClientState*** > ( ( *reinterpret_cast< uintptr_t** > ( engine( ) ) )[ 12 ] + 0x10 );

	return state;
}

IMoveHelper* context::movehelper( )
{
	static IMoveHelper* move = nullptr;

	if ( !move ) // �������������� 1 ���
		move = **reinterpret_cast< IMoveHelper*** >( utils::pattern_scan( "client.dll", "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01" ) + 2 );

	return move;
}

CInput* context::input( )
{
	static CInput* inp = nullptr;

	if ( !inp ) // �������������� 1 ���
		inp = *reinterpret_cast< CInput** >( utils::pattern_scan( "client.dll", "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10 " ) + 0x1 );

	return inp;
}

i_mem_alloc* context::mem_alloc( )
{
	static i_mem_alloc* alloc = nullptr;

	if ( !alloc ) // �������������� 1 ���
		alloc = *reinterpret_cast< i_mem_alloc** > ( GetProcAddress( GetModuleHandleA( "tier0.dll" ), "g_pMemAlloc" ) );

	return alloc;
}

IEngineVGui* context::engine_gui( )
{
	static IEngineVGui* eng = nullptr;

	if ( !eng ) // �������������� 1 ���
		eng = this->get<IEngineVGui>( "engine.dll", "VEngineVGui001", true );

	return eng;
}

DWORD context::post_process( )
{
	static DWORD post;

	if ( !post )
		post = *( DWORD* )( utils::pattern_scan( "client.dll", "80 3D ? ? ? ? ? 53 56 57 0F 85" ) + 2 );

	return post;
}

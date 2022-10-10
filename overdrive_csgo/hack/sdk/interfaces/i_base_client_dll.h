#include "../other/global_vars.h"
#include "i_client_class.h"
#include "i_app_system.h"
#include "../other/virtual.h"

enum client_frame_stage_t
{
	frame_undefined = -1,
	frame_start,
	frame_net_update_start,
	frame_net_update_postdataupdate_start,
	frame_net_update_postdataupdate_end,
	frame_net_update_end,
	frame_render_start,
	frame_render_end
};

enum render_view_info_t
{
	render_view_unspecified = 0,
	render_view_drawviewmodel = ( 1 << 0 ),
	render_view_drawhud = ( 1 << 1 ),
	render_view_suppressmonitor_rendering = ( 1 << 2 ),
};

class i_base_client
{
public:
	virtual int              connect( create_interface_fn appsystemfactory, CGlobalVarsBase* pglobals ) = 0;
	virtual int              disconnect( void ) = 0;
	virtual int              init( create_interface_fn appsystemfactory, CGlobalVarsBase* pglobals ) = 0;
	virtual void             post_init( ) = 0;
	virtual void             shutdown( void ) = 0;
	virtual void             level_initpre_entity( char const* pmapname ) = 0;
	virtual void             level_initpost_entity( ) = 0;
	virtual void             level_shutdown( void ) = 0;
	virtual ClientClass*    get_all_classes( void ) = 0;

	bool dispatch_user_msg( int messageType, int arg, int arg1, void* data )
	{
		using DispatchUserMessage_t = bool* ( __thiscall* )( void*, int, int, int, void* );
		return call_virtual<DispatchUserMessage_t>( this, 38 )( this, messageType, arg, arg1, data );
	}
};
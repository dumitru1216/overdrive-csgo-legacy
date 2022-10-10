#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <chrono>
#include <thread>
#include <Psapi.h>
#include <map>
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>

#define INRANGE( x,a,b )  ( x >= a && x <= b ) 
#define GETBITS( x )    ( INRANGE( ( x& ( ~0x20 ) ),'A','F' ) ? ( ( x&( ~0x20 ) ) - 'A' + 0xa ) : ( INRANGE( x,'0','9' ) ? x - '0' : 0) )
#define GETBYTE( x )    ( GETBITS( x[ 0 ] ) << 4 | GETBITS( x[ 1 ] ) )

#include "interfaces/i_alloc.h"
#include "interfaces/i_app_system.h"
#include "interfaces/i_base_client_dll.h"
#include "interfaces/i_client_class.h"
#include "interfaces/i_client_entity.h"
#include "interfaces/i_client_entity_list.h"
#include "interfaces/i_client_mode.h"
#include "interfaces/i_client_networkable.h"
#include "interfaces/i_client_renderable.h"
#include "interfaces/i_client_state.h"
#include "interfaces/i_client_thinkable.h"
#include "interfaces/i_client_unknown.h"
#include "interfaces/i_collideable.h"
#include "interfaces/i_con_var.h"
#include "interfaces/i_cvar.h"
#include "interfaces/i_engine_trace.h"
#include "interfaces/i_game_event_manager.h"
#include "interfaces/i_glow_manager.h"
#include "interfaces/i_handle_entity.h"
#include "interfaces/i_input.h"
#include "interfaces/i_input_system.h"
#include "interfaces/i_material_system.h"
#include "interfaces/i_mdl_cache.h"
#include "interfaces/i_move_helper.h"
#include "interfaces/i_panel.h"
#include "interfaces/i_physics.h"
#include "interfaces/i_prediction.h"
#include "interfaces/i_render_view.h"
#include "interfaces/i_surface.h"
#include "interfaces/i_v_debug_overlay.h"
#include "interfaces/i_v_engine_client.h"
#include "interfaces/i_v_model_info_client.h"
#include "interfaces/i_v_model_render.h"
#include "interfaces/i_view_render.h"
#include "interfaces/i_bone_acessor.h"

#include "other/c_user_cmd.h"
#include "other/checksum_md5.h"
#include "other/studio.h"
#include "other/key_values.h"
#include "other/data_map.h"
#include "other/recv.h"
#include "other/utl_vector.h"
#include "other/convar.h"
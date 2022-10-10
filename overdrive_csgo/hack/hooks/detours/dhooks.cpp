#include "dhooks.h"
#include "../../../utils/utl.h"

namespace dhooks
{
	void hook( )
	{
		static const auto skip_frame = utils::pattern_scan( "client.dll", "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02" );
		skip_original = ( skip_frame_t )DetourFunction( ( PBYTE )skip_frame, ( PBYTE )should_skip_animation_frame );

/*
		static const auto setup_bones_ad = utils::pattern_scan( "client.dll", ( "55 8B EC 83 E4 F0 B8 D8" ) ); // im not sure if that works
		DWORD* pointer = ( DWORD* )*( DWORD* )( setup_bones_ad );
		original_setup_bones = ( setupbones_t )DetourFunction( ( PBYTE )pointer[ 13 ], ( PBYTE )setup_bones );

*/
	//	static const auto c_cs_player_table = utils::pattern_scan( "client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C" ) + 0x47;
	//	DWORD* ex_pointer = ( DWORD* )*( DWORD* )( c_cs_player_table );

		//origin_update_client_side_animation = ( update_client_side_animation_t )DetourFunction( ( PBYTE )ex_pointer[ 218 ], ( PBYTE )update_client_side_animation );
		//orig_do_extra_bone_processing = ( do_extra_bone_processing_t )DetourFunction( ( PBYTE )ex_pointer[ 194 ], ( PBYTE )do_extra_bone_processing ); // it hink
		//origin_standard_blending_rules = ( standard_blending_rules_t )DetourFunction( ( PBYTE )ex_pointer[ 200 ], ( PBYTE )standard_blending_rules );
	}

	void unhook( )
	{
		DetourRemove( ( PBYTE )skip_original, ( PBYTE )should_skip_animation_frame );
	//	DetourRemove( ( PBYTE )original_setup_bones, ( PBYTE )setup_bones );
		//DetourRemove( ( PBYTE )origin_update_client_side_animation, ( PBYTE )update_client_side_animation );
	//	DetourRemove( ( PBYTE )orig_do_extra_bone_processing, ( PBYTE )do_extra_bone_processing );
		//DetourRemove( ( PBYTE )origin_standard_blending_rules, ( PBYTE )standard_blending_rules );
	}

};
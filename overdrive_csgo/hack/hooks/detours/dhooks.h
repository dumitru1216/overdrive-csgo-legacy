#pragma once

#include "../detours.h"
#include "../../sdk/interfaces/create_if.h"

namespace dhooks
{
	void hook( );
	void unhook( );

	using skip_frame_t = bool( __thiscall* )( );
	extern skip_frame_t skip_original;
	bool __fastcall should_skip_animation_frame( );

	using send_net_msg_t = bool( __thiscall* )( INetChannel*, INetMessage&, bool, bool );
	extern send_net_msg_t send_msg_original;
	bool __fastcall send_net_msg( INetChannel* pNetChan, void* edx, INetMessage& msg, bool bForceReliable, bool bVoice );

	typedef bool( __thiscall* setupbones_t )( iclient_renderable*, matrix3x4_t*, int, int, float );
	extern setupbones_t original_setup_bones;
	bool __fastcall setup_bones( iclient_renderable* ecx, void* edx, matrix3x4_t* bone_to_world_out, int max_bones, int bone_mask, float curtime );

	typedef void( __thiscall* update_client_side_animation_t )( m_entity* );
	extern update_client_side_animation_t origin_update_client_side_animation;
	void __fastcall update_client_side_animation( m_entity* player, uint32_t );

	typedef void( __thiscall* do_extra_bone_processing_t )( void*, studiohdr_t*, Vector*, Quaternion*, const matrix3x4_t&, uint8_t*, void* );
	extern do_extra_bone_processing_t orig_do_extra_bone_processing;
	void _fastcall  do_extra_bone_processing( void* ecx, uint32_t, studiohdr_t* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_computed, void* context );

	typedef void( __thiscall* standard_blending_rules_t )( m_entity*, studiohdr_t*, Vector*, Quaternion*, float, int );
	extern standard_blending_rules_t origin_standard_blending_rules;
	void _fastcall standard_blending_rules( m_entity* player, uint32_t, studiohdr_t* hdr, Vector* pos, Quaternion* q, const float time, int mask );
};
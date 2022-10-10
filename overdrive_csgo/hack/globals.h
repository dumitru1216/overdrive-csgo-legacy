#pragma once
#include "sdk/entity/entity.h"

namespace csgo
{
	extern m_entity* m_local;
	extern CUserCmd* m_cmd;
	extern bool m_send_packet;
	extern bool m_lag;

	extern bool m_update[ 65 ];

	extern bool m_allow_setup_bones;
	extern bool m_allow_call;

	extern Vector m_real;
	extern Vector m_fake;
	extern Vector m_original;

	extern float m_init_fov;

	struct in_use
	{
		float m_velocity_modifier;
	};

	extern bool override_velocity;
	extern in_use backup;
}

inline bool m_inUnhook;
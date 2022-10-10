#include "globals.h"

namespace csgo
{
	m_entity* m_local;
	CUserCmd* m_cmd;
	bool m_send_packet;
	bool m_lag;

	bool m_update[ 65 ];
	bool m_allow_setup_bones;

	float m_init_fov;
	bool m_allow_call;

	Vector m_real;
	Vector m_fake;
	Vector m_original;

	bool override_velocity;
	in_use backup = in_use{ };
}
#pragma once

#include "../../utils/singleton.h"
#include "../sdk/entity/entity.h"
#include <array>

enum Prediction_stage
{
	INITIALIZE,
	PREDICT,
	FINISH
};

class prediction : public singleton <prediction>
{
	struct Backup_data
	{
		int flags = 0;
		Vector velocity;
	};

	struct Prediction_data
	{
		void reset( )
		{
			prediction_stage = INITIALIZE;
			tickbase = 0;
			random_seed = 0;
			in_prediction = false;
			curtime = 0.0f;
			frametime = 0.0f;
			prediction_random_seed = nullptr;
			command_ack = 0;
		}

		int command_ack = 0;
		Prediction_stage prediction_stage = INITIALIZE;
		int tickbase = 0;
		uint32_t random_seed = 0;
		bool in_prediction = false;
		float curtime = 0.0f;
		float frametime = 0.0f;
		uint32_t* prediction_random_seed = nullptr;
	};

	class StoredData_t {
	public:
		int    m_tickbase;
		Vector  m_punch;
		Vector  m_punch_vel;
		Vector m_view_offset;
		float  m_velocity_modifier;

	public:
		__forceinline StoredData_t( ) : m_tickbase{ }, m_punch{ }, m_punch_vel{ }, m_view_offset{ }, m_velocity_modifier{ } {};
	};

public:

	std::array< StoredData_t, MULTIPLAYER_BACKUP > m_data;

	Backup_data backup_data;
	Prediction_data prediction_data;

	void patch_attack_packet( CUserCmd* m_nCmd, bool m_bPredicted );
	void init( );
	void predict( CUserCmd* m_pcmd );
	void finish( );

	void store_data( );
	void setup_data( );
	void reset_data( );
	void update_vel( );
};
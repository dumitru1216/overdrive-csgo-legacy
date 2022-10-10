#pragma once

#include <cstdint>
#include "../math/vector.h"

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define PAD(size) BYTE MACRO_CONCAT(_pad, __COUNTER__)[size];

class bf_read;
class bf_write;

class INetMessage
{
public:
	virtual					~INetMessage( ) { }
	virtual void			SetNetChannel( void* pNetChannel ) = 0;
	virtual void			SetReliable( bool bState ) = 0;
	virtual bool			Process( ) = 0;
	virtual	bool			ReadFromBuffer( bf_read& buffer ) = 0;
	virtual	bool			WriteToBuffer( bf_write& buffer ) = 0;
	virtual bool			IsReliable( ) const = 0;
	virtual int				GetType( ) const = 0;
	virtual int				GetGroup( ) const = 0;
	virtual const char* GetName( ) const = 0;
	virtual void* GetNetChannel( void ) const = 0;
	virtual const char* ToString( ) const = 0;
};

class INetChannel
{
public:
	char pad_0000[ 20 ];           //0x0000
	bool m_bProcessingMessages;  //0x0014
	bool m_bShouldDelete;        //0x0015
	char pad_0016[ 2 ];            //0x0016
	int32_t m_nOutSequenceNr;    //0x0018 last send outgoing sequence number
	int32_t m_nInSequenceNr;     //0x001C last received incoming sequnec number
	int32_t m_nOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
	int32_t m_nOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int32_t m_nInReliableState;  //0x0028 state of incoming reliable data
	int32_t m_nChokedPackets;    //0x002C number of choked packets
	char pad_0030[ 1044 ];         //0x0030
};

class CClockDriftMgr
{
public:
	float m_ClockOffsets[ 16 ];   //0x0000
	uint32_t m_iCurClockOffset; //0x0044
	uint32_t m_nServerTick;     //0x0048
	uint32_t m_nClientTick;     //0x004C
}; //Size: 0x0050

class CEventInfo {
public:
	enum {
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	// 0 implies not in use
	short                    m_class_id;
	float                    m_fire_delay;
	const void* m_send_table;
	const ClientClass* m_client_class;//void*
	int                        m_bits;
	uint8_t* m_data;
	int                        m_flags;
	PAD( 0x1C );
	CEventInfo* m_next;
};

class CClientState
{
public:
	void force_full_update( ) {
		*reinterpret_cast< int* >( std::uintptr_t( this ) + 0x174 ) = -1;
	}

	char pad_0000[ 156 ];             //0x0000
	INetChannel* m_NetChannel;       //0x009C
	uint32_t m_nChallengeNr;        //0x00A0
	char pad_00A4[ 100 ];             //0x00A4
	uint32_t m_nSignonState;        //0x0108
	char pad_010C[ 8 ];               //0x010C
	float m_flNextCmdTime;          //0x0114
	uint32_t m_nServerCount;        //0x0118
	uint32_t m_nCurrentSequence;    //0x011C
	char pad_0120[ 8 ];               //0x0120
	CClockDriftMgr m_ClockDriftMgr; //0x0128
	uint32_t m_nDeltaTick;          //0x0174
	bool m_bPaused;                 //0x0178 pauses the game xD
	char pad_017D[ 3 ];               //0x017D
	uint32_t m_nViewEntity;         //0x0180
	uint32_t m_nPlayerSlot;         //0x0184
	char m_szLevelName[ 260 ];        //0x0188
	char m_szLevelNameShort[ 80 ];    //0x028C
	char m_szGroupName[ 80 ];         //0x02DC
	char pad_032Ñ[ 92 ];              //0x032Ñ
	uint32_t m_nMaxClients;         //0x0388
	char pad_0314[ 18824 ];           //0x0314
	float m_flLastServerTickTime;   //0x4C98
	bool insimulation;              //0x4C9C
	char pad_4C9D[ 3 ];               //0x4C9D
	uint32_t oldtickcount;          //0x4CA0
	float m_tickRemainder;          //0x4CA4
	float m_frameTime;              //0x4CA8
	uint32_t lastoutgoingcommand;        //0x4CAC
	uint32_t chokedcommands;             //0x4CB0
	uint32_t last_command_ack;           //0x4CB4
	uint32_t command_ack;                //0x4CB8 does nothing
	uint32_t m_nSoundSequence;           //0x4CBC
	std::byte		pad8[ 0x50 ];				//0x4D38
	Vector			angViewPoint;			//0x4D88
	std::byte		pad9[ 0xD0 ];				//0x4D9A
	CEventInfo*		pEvents;
};

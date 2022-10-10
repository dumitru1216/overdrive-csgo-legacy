#pragma once

#include "../other/recv.h"
enum ClassId;

class ClientClass;
class iclient_networkable;

typedef iclient_networkable* ( *CreateClientClassFn )( int entnum, int serialNum );
typedef iclient_networkable* ( *CreateEventFn )( );

class ClientClass
{
public:
	CreateClientClassFn      m_pCreateFn;
	CreateEventFn            m_pCreateEventFn;
	char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	ClassId                  m_ClassID;
};
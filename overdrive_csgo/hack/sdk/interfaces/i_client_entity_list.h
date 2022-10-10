#pragma once

#include "i_handle_entity.h"

class iclient_networkable;
class iclient_entity;

class iclient_entityList
{
public:
	virtual iclient_networkable* GetClientNetworkable( int entnum ) = 0;
	virtual void* vtablepad0x1( void ) = 0;
	virtual void* vtablepad0x2( void ) = 0;
	virtual iclient_entity* GetClientEntity( int entNum ) = 0;
	virtual iclient_entity* GetClientEntityFromHandle( BaseHandle hEnt ) = 0;
	virtual int                   NumberOfEntities( bool bIncludeNonNetworkable ) = 0;
	virtual int                   GetHighestEntityIndex( void ) = 0;
	virtual void                  SetMaxEntities( int maxEnts ) = 0;
	virtual int                   GetMaxEntities( ) = 0;
};
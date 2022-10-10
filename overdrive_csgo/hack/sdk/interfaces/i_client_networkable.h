#pragma once

class iclient_unknown;
class ClientClass;
class bf_read;

class iclient_networkable
{
public:
	virtual iclient_unknown* Geticlient_unknown( ) = 0;
	virtual void             Release( ) = 0;
	virtual ClientClass* GetClientClass( ) = 0;
	virtual void             NotifyShouldTransmit( int state ) = 0;
	virtual void             OnPreDataChanged( int updateType ) = 0;
	virtual void             OnDataChanged( int updateType ) = 0;
	virtual void             PreDataUpdate( int updateType ) = 0;
	virtual void             PostDataUpdate( int updateType ) = 0;
	virtual void             __unkn( void ) = 0;
	virtual bool             IsDormant( void ) = 0;
	virtual int              EntIndex( void ) const = 0;
	virtual void             ReceiveMessage( int classID, bf_read& msg ) = 0;
	virtual void* GetDataTableBasePtr( ) = 0;
	virtual void             SetDestroyedOnRecreateEntities( void ) = 0;
};
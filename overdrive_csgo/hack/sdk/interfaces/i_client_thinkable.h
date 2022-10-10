#pragma once

class iclient_unknown;
class CClientThinkHandlePtr;
typedef CClientThinkHandlePtr* ClientThinkHandle_t;

class iclient_thinkable
{
public:
	virtual iclient_unknown* Geticlient_unknown( ) = 0;
	virtual void                ClientThink( ) = 0;
	virtual ClientThinkHandle_t GetThinkHandle( ) = 0;
	virtual void                SetThinkHandle( ClientThinkHandle_t hThink ) = 0;
	virtual void                Release( ) = 0;
};
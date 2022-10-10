#pragma once

#include "i_handle_entity.h"
#include "i_collideable.h"

class iclient_networkable;
class iclient_renderable;
class iclient_entity;
class m_entity;
class iclient_thinkable;
class IClientAlphaProperty;

class iclient_unknown : public IHandleEntity
{
public:
	virtual ICollideable* GetCollideable( ) = 0;
	virtual iclient_networkable* GetClientNetworkable( ) = 0;
	virtual iclient_renderable* GetClientRenderable( ) = 0;
	virtual iclient_entity* Geticlient_entity( ) = 0;
	virtual m_entity* GetBaseEntity( ) = 0;
	virtual iclient_thinkable* GetClientThinkable( ) = 0;
	//virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
	virtual IClientAlphaProperty* GetClientAlphaProperty( ) = 0;
};
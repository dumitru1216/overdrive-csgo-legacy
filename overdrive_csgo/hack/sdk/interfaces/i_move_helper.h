#pragma once

class iclient_entity;

class IMoveHelper
{
public:
	virtual	void _vpad( ) = 0;
	virtual void SetHost( iclient_entity* host ) = 0;
};

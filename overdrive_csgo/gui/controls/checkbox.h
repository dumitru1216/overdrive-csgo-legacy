#pragma once
#include "group.h"

class checkbox : public groupbox
{
public:

	checkbox( );
	checkbox( std::string name, bool* value );

	void draw( );
	void update( );

	vec2_t position;

private:

	std::string name;
	bool* value;
};
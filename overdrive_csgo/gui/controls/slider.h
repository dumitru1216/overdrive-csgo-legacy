#pragma once

#include "group.h"

class slider : public groupbox
{
public:

	slider( );
	slider( std::string name, float* value, int minimum_value = 0, int maximum_value = 100 );

	void draw( );
	void update( );

	vec2_t position;

private:

	std::string name;
	float* value;
	int minimum_value;
	int maximum_value;

};
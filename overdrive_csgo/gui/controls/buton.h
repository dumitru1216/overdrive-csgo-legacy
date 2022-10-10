#pragma once

#include "group.h"

class button : public groupbox
{
public:

	button( );
	button( std::string name, std::function< void( ) > n_func );

	void draw( );
	void update( );

	vec2_t position;

private:

	std::string name;
	std::function< void( ) > n_func = { };

	bool focused;
};
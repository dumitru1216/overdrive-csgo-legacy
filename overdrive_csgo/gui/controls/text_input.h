#pragma once

#include "group.h"

class text_input : public groupbox
{
public:

	text_input( );
	text_input( std::string label, std::string* change );

	void update( );
	void draw( );

	vec2_t position;

private:

	std::string label;
	std::string* change;

	bool focused;
	int current_element;

};
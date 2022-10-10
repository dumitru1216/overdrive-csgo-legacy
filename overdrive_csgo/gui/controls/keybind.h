#pragma once
#include "group.h"

class keybind : public groupbox
{
public:

	keybind( );
	keybind( int* value, int* key_style, int current_element );

	void update( );
	void draw( );
	void list( );

	vec2_t position;
	int current_element;

private:

	int* value;
	int* key_style;

	bool focused;
	bool focused_on_list;

};
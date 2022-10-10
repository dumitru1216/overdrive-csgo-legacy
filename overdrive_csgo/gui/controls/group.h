#pragma once

#include "window.h"
#include <functional>

class checkbox;
class slider;
class combobox;
class keybind;
class multi;
class color_picker;
class text_input;
class button;

class groupbox : public window
{
public:

	std::string label;
	std::string var_name;

	groupbox( );
	groupbox( std::string name, vec2_t position, vec2_t size, int current_element, int content_height, int tab = 0 );

	void run( );
	void draw( );

	void add_control( checkbox* check );
	void add_control( slider* slider );
	void add_control( combobox* combobox );
	void add_control( multi* multicombobox );
	void add_control( keybind* keybind );
	void add_control( color_picker* color );
	void add_control( text_input* text );
	void add_control( button* text );

	void init_last_element( color_picker* color );

	std::string name;
	vec2_t position;
	vec2_t size;
	vec2_t cursor;

	int tab;

	int current_element;
	int content_height;

	int control_offset_x;
	int control_offset_y[ 512 ];

	int control_element[ 512 ];
	void scroll_process( );

	bool visible;

};
#pragma once

#include "group.h"

struct multicombobox_t
{
	multicombobox_t( std::string name, bool* value )
	{
		this->name = name;
		this->value = value;
	}

	std::string name;
	bool* value;
};

class multi : public groupbox
{
public:

	multi( );
	multi( std::string name, int current_element );

	void update( );
	void draw( );
	void add_item( std::string name, bool* value );

	vec2_t position;
	std::vector<multicombobox_t> items;

	bool focused;
	int current_element;

private:

	std::string name;

};
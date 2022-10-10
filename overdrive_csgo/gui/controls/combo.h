#pragma once
#include "group.h"

class combobox : public groupbox
{
public:

	combobox( );
	combobox( std::string name, int* value, std::vector<std::string> items, int current_element );

	void update( );
	void draw( );

	vec2_t position;
	std::vector<std::string> items;
	int current_element;

private:

	std::string name;
	int* value;
	bool focused;

};
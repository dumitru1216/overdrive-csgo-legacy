#pragma once
#include "group.h"

class color_picker : public groupbox
{
public:

	color_picker( );
	color_picker( std::string name, float* red, float* green, float* blue, float* alpha, bool alpha_bar = false );

	void update( );
	void draw( );
	void init( );

	vec2_t position;
	bool focused;

private:

	std::string name;

	float* red;
	float* green;
	float* blue;
	float* alpha;
	bool alpha_bar;

};
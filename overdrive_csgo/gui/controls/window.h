#pragma once
#include <vector>
#include <string>
#include "../../hack/sdk/math/vector2d.h"

/* predefine */
class groupbox;

/* window class */
class window {
public:
	/* setup window */
	window( );
	window( std::string name, vec2_t* position, vec2_t size, std::vector<std::string> window_tabs = { }, int* selected_tab = 0 );

	/* add groupbox */
	void add_groupbox( groupbox* groupbox );

	/* other settings */
	vec2_t* window_position;
	std::vector<std::string> window_tabs;
	int* selected_tab;

private:
	/* main */
	void draw( );
	void run( );

	/* handle */
	void handle_tabs( );
	void handle_position( );

	/* other settings */
	std::string name;
	vec2_t size;
};
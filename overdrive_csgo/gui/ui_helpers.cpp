#include "ui.h"

namespace ui
{
	static bool key_state[ 256 ];
	static bool previous_key_state[ 256 ];

	bool ui_helpers::mouse_pointer( vec2_t position, vec2_t size )
	{
		return ui_helpers::controller::get( ).mouse_pos.x > position.x && ui_helpers::controller::get( ).mouse_pos.y > position.y && ui_helpers::controller::get( ).mouse_pos.x < position.x + size.x && ui_helpers::controller::get( ).mouse_pos.y < position.y + size.y;
	}

	void ui_helpers::init_input( )
	{
		for ( int i = 0; i < 256; i++ ) {
			previous_key_state[ i ] = key_state[ i ];
			key_state[ i ] = GetAsyncKeyState( i );
		}
	}

	bool ui_helpers::key_was_pressed( const int i )
	{
		return key_state[ i ] && !previous_key_state[ i ];
	}
};

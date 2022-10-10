#include "keybind.h"
#include "../ui.h"

#include <algorithm>

static short int stored_index = -1;
static short int stored_index_list = -1;

static bool open = false;
static bool open_list = false;

static int animation[ max_elements ];
static int key = -1;

std::string key_name[ max_elements ];
static bool good_key = false;

char name_buf[ 128 ];
const char* key_strings[ 254 ] = {
	"INVLD", "M1", "M2", "BRK", "M3", "M4", "M5",
	"INVLD", "BSPC", "TAB", "INVLD", "INVLD", "INVLD", "ENTER", "INVLD", "INVLD", "SHI",
	"CTRL", "ALT", "PAU", "CAPS", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"ESC", "INVLD", "INVLD", "INVLD", "INVLD", "SPACE", "PGUP", "PGDOWN", "END", "HOME", "LEFT",
	"UP", "RIGHT", "DOWN", "INVLD", "PRNT", "INVLD", "PRTSCR", "INS", "DEL", "INVLD", "0", "1",
	"2", "3", "4", "5", "6", "7", "8", "9", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U",
	"V", "W", "X", "Y", "Z", "LFTWIN", "RGHTWIN", "INVLD", "INVLD", "INVLD", "NUM0", "NUM1",
	"NUM2", "NUM3", "NUM4", "NUM5", "NUM6", "NUM7", "NUM8", "NUM9", "*", "+", "_", "-", ".", "/", "F1", "F2", "F3",
	"F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20",
	"F21",
	"F22", "F23", "F24", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"NUM LOCK", "SCROLL LOCK", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "LSHFT", "RSHFT", "LCTRL",
	"RCTRL", "LMENU", "RMENU", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "NTRK", "PTRK", "STOP", "PLAY", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", ";", "+", ",", "-", ".", "/?", "~", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "{", "\\|", "}", "'\"", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD", "INVLD",
	"INVLD", "INVLD"
};

std::vector<std::string> need_to_draw = { "always", "toggle", "hold on", "hold off" };

keybind::keybind( ) { }

keybind::keybind( int* value, int* key_style, int current_element )
{
	this->value = value;
	this->key_style = key_style;
	this->current_element = current_element;
}

void keybind::update( )
{
	bool can = ui::ui_helpers::controller::get( ).group.can_scroll && !ui::ui_helpers::controller::get( ).elements.is_any_multi_combo_active && !ui::ui_helpers::controller::get( ).elements.is_any_combo_active && !ui::ui_helpers::controller::get( ).elements.is_color_active;

	if ( open_list )
	{
		ui::ui_helpers::controller::get( ).elements.can_open_color = false;
		ui::ui_helpers::controller::get( ).elements.can_open_any_combo = false;
		ui::ui_helpers::controller::get( ).group.can_scroll = false;
	}
	else
	{
		ui::ui_helpers::controller::get( ).elements.can_open_color = true;
		ui::ui_helpers::controller::get( ).elements.can_open_any_combo = true;
		ui::ui_helpers::controller::get( ).group.can_scroll = true;
	}

	if ( open_list && stored_index_list == ui::ui_helpers::controller::get( ).index && ui::ui_helpers::key_was_pressed( VK_LBUTTON ) )
	{
		open_list = !open_list;
		stored_index_list = -1;

		ui::ui_helpers::controller::get( ).elements.is_list_active[ this->current_element ] = open_list;
		ui::ui_helpers::controller::get( ).elements.is_any_list_active = open_list;
	}

	if ( ui::ui_helpers::key_was_pressed( VK_RBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 190, this->position.y + 2 ), vec2_t( 38, 10 ) ) && ui::ui_helpers::controller::get( ).elements.can_open_list && can )
	{
		open_list = !open_list;
		stored_index_list = ui::ui_helpers::controller::get( ).index;

		ui::ui_helpers::controller::get( ).elements.is_list_active[ this->current_element ] = open_list;
		ui::ui_helpers::controller::get( ).elements.is_any_list_active = open_list;
	}

	if ( stored_index_list == ui::ui_helpers::controller::get( ).index )
		this->focused_on_list = open_list;

	if ( GetAsyncKeyState( VK_LBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 190, this->position.y + 2 ), vec2_t( 38, 10 ) ) && !this->focused_on_list && can )
	{
		if ( !open )
			open = true;

		stored_index = ui::ui_helpers::controller::get( ).index;
	}

	if ( stored_index == ui::ui_helpers::controller::get( ).index )
		this->focused = open;

	if ( this->focused )
	{
		for ( int i = 0; i < 255; i++ )
		{
			if ( ui::ui_helpers::key_was_pressed( i ) )
			{
				if ( i == VK_ESCAPE )
				{
					*this->value = -1;
					stored_index = -1;
					return;
				}

				*this->value = i;
				stored_index = -1;
				return;
			}
		}
	}

	std::string empty = "[ ";
	std::string empty_2 = " ]";

	if ( this->focused )
	{
		key_name[ this->current_element ] = "[ none ]";
	}
	else
	{
		if ( *this->value >= 0 )
		{
			key_name[ this->current_element ] = empty + key_strings[ *this->value ] + empty_2;
			if ( key_name[ this->current_element ].c_str( ) )
			{
				good_key = true;
			}
			else
			{
				if ( GetKeyNameText( *this->value << 16, name_buf, 127 ) )
				{
					key_name[ this->current_element ] = empty + name_buf;
					good_key = true;
				}
			}
		}

		if ( !good_key )
		{
			key_name[ this->current_element ] = "[ none ]";
		}
	}
}

void keybind::draw( )
{
	if ( key_name[ this->current_element ] == "" )
		key_name[ this->current_element ] = "[ none ]";

	auto i = std::strlen( key_name[ this->current_element ].c_str( ) );

	std::transform( key_name[ this->current_element ].begin( ), key_name[ this->current_element ].end( ), key_name[ this->current_element ].begin( ), ::tolower );
	auto text_size = draw_list->get_text_width( key_name[ this->current_element ].c_str( ), draw_list->fonts.main_font );

	draw_list->text( this->position.x + 230 - text_size - 3, this->position.y, draw_list->fonts.main_font, key_name[ this->current_element ].c_str( ), this->focused ? Color( 152, 150, 159, 255 ) : Color( 65, 66, 74, 255 ) );
}

void keybind::list( )
{
	if ( this->focused_on_list )
		animation[ this->current_element ]++;
	else
		animation[ this->current_element ]--;

	if ( animation[ this->current_element ] > 19 )
		animation[ this->current_element ] = 19;

	if ( animation[ this->current_element ] < 0 )
		animation[ this->current_element ] = 0;

	std::clamp<int>( animation[ this->current_element ], 0, 19 );

	if ( animation[ this->current_element ] != 0 )
	{
		for ( int i = 0; i < need_to_draw.size( ); i++ )
		{
			ui::external_rendering::gradient_to_draw.insert( ui::external_rendering::gradient_to_draw.begin( ), ui::external_rendering::gradient_draw_t( this->position.x + 235, this->position.y + ( animation[ this->current_element ] * i ) + 1, 70, 18, Color( 20, 20, 20 ), Color( 25, 25, 25 ) ) );

			if ( i != 0 )
				ui::external_rendering::line_to_draw.insert( ui::external_rendering::line_to_draw.begin( ), ui::external_rendering::line_draw_t( this->position.x + 235, this->position.y + ( animation[ this->current_element ] * i ), this->position.x + 305, this->position.y + ( animation[ this->current_element ] * i ), Color( 180, 0, 0, 255 ) ) );

			ui::external_rendering::outlines_to_draw.insert( ui::external_rendering::outlines_to_draw.begin( ), ui::external_rendering::outline_draw_t( this->position.x + 235, this->position.y, 70, 4 * animation[ this->current_element ], Color( 180, 0, 0 ) ) );

			if ( ui::ui_helpers::key_was_pressed( VK_LBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 235, this->position.y + ( i * 19 ) ), vec2_t( 70, 18 ) ) )
				*this->key_style = i;

			ui::external_rendering::text_to_draw.insert( ui::external_rendering::text_to_draw.begin( ), ui::external_rendering::text_draw_t( this->position.x + 235 + 9, this->position.y + ( i * animation[ this->current_element ] ) + 3, draw_list->fonts.main_font, need_to_draw[ i ], i == *this->key_style ? Color( 255, 0, 0, 255 ) : Color( 152, 150, 159, 255 ) ) );
		}
	}
}
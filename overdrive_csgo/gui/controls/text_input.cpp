#include "text_input.h"
#include "../ui.h"

extern const char* lowercase[ 254 ];
extern const char* uppercase[ 254 ];

static short int stored_index = -1;
static bool open = false;

float next_blink;
constexpr short int m_size = 220;

text_input::text_input( ) { }

text_input::text_input( std::string name, std::string* change )
{
	this->label = name;
	this->change = change;
}

void text_input::update( )
{
	if ( open && stored_index == ui::ui_helpers::controller::get( ).index && ui::ui_helpers::key_was_pressed( VK_LBUTTON ) )
	{
		open = !open;
		stored_index = -1;
	}

	if ( ui::ui_helpers::key_was_pressed( VK_LBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 5, this->position.y + 5 ), vec2_t( m_size, 18 ) ) )
	{
		open = !open;
		stored_index = ui::ui_helpers::controller::get( ).index;
	}

	if ( stored_index == ui::ui_helpers::controller::get( ).index )
		this->focused = open;

	if ( this->focused )
	{
		std::string str = *this->change;

		for ( int i = 0; i < 255; i++ ) {
			if ( ui::ui_helpers::key_was_pressed( i ) ) {
				if ( i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT ) {
					stored_index = -1;
					return;
				}

				if ( strlen( str.c_str( ) ) != 0 ) {
					if ( GetAsyncKeyState( VK_BACK ) ) {
						*this->change = str.substr( 0, strlen( str.c_str( ) ) - 1 );
					}
				}

				if ( strlen( str.c_str( ) ) < 42 && i != NULL && uppercase[ i ] != nullptr ) {
					if ( GetAsyncKeyState( VK_SHIFT ) ) {
						*this->change = str + uppercase[ i ];
					}
					else {
						*this->change = str + lowercase[ i ];
					}

					return;
				}

				if ( strlen( str.c_str( ) ) < 42 && i == 32 ) {
					*this->change = str + " ";
					return;
				}
			}
		}
	}

}

void text_input::draw( )
{
	std::string to_render = *this->change;

	if ( GetTickCount64( ) >= next_blink )
		next_blink = GetTickCount64( ) + 800;

	if ( this->focused && GetTickCount64( ) > ( next_blink - 400 ) )
		to_render += "_";

	draw_list->gradient_vertical_init( this->position.x + 5, this->position.y + 5, m_size, 18, Color( 20, 20, 20 ), Color( 25, 25, 25 ) );
	draw_list->rect_init( this->position.x + 5, this->position.y + 5, m_size, 18, this->focused ? Color( 155, 153, 158, 255 ) : Color( 65, 66, 74, 255 ) );
	draw_list->text( this->position.x + 4 + 5, this->position.y + 7, draw_list->fonts.main_font, to_render.c_str( ), this->focused ? Color( 180, 0, 0, 255 ) : Color( 65, 66, 74, 255 ) );
	draw_list->text( this->position.x + 5, this->position.y - 10, draw_list->fonts.main_font, this->label, this->focused ? Color( 152, 150, 159, 255 ) : Color( 65, 66, 74, 255 ) );
}

const char* lowercase[ 254 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
"y", "z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

const char* uppercase[ 254 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "_", ".", "?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "{", "|", "}", "\"", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
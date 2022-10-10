#include "buton.h"
#include "../ui.h"

static short int stored_index = -1;
static bool open = false;

constexpr short int m_size = 220;

button::button( ) { }

button::button( std::string name, std::function< void( ) > n_func )
{
	this->name = name;
	this->n_func = n_func;
}

void button::update( ) {
	if ( !ui::ui_helpers::key_was_pressed( VK_LBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 5, this->position.y + 5 ), vec2_t( m_size, 26 ) ) )
	{
		if ( open )
			this->n_func( );

		open = false;
	}

	if ( ui::ui_helpers::controller::get( ).last.open[ 0 ] == -1 && !ui::ui_helpers::controller::get( ).elements.is_any_combo_active && ui::ui_helpers::key_was_pressed( VK_LBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 5, this->position.y + 5 ), vec2_t( m_size, 26 ) ) )
	{
		open = true;
		stored_index = ui::ui_helpers::controller::get( ).index;
	}

	if ( stored_index == ui::ui_helpers::controller::get( ).index )
		this->focused = open;
}

void button::draw( ) {
	auto size = draw_list->get_text_width( this->name.c_str( ), draw_list->fonts.main_font );

	draw_list->gradient_vertical_init( this->position.x + 5, this->position.y + 5, m_size, 26, Color( 20, 20, 20 ), Color( 25, 25, 25 ) );
	draw_list->rect_init( this->position.x + 5, this->position.y + 5, m_size, 26, this->focused ? Color( 180, 0, 0, 255 ) : Color( 65, 66, 74, 255 ) );
	draw_list->text( this->position.x + 5 + ( m_size / 2 ) - ( size / 2 ), this->position.y + 11, draw_list->fonts.main_font, this->name, this->focused ? Color( 180, 0, 0, 255 ) : Color( 65, 66, 74, 255 ) );
}
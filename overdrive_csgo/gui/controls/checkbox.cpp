#include "checkbox.h"
#include "../ui.h"

checkbox::checkbox( ) { }

checkbox::checkbox( std::string name, bool* value ) {
	this->name = name;
	this->value = value;
}

void checkbox::update( ) {
	bool can = ui::ui_helpers::controller::get( ).group.can_scroll && !ui::ui_helpers::controller::get( ).elements.is_any_multi_combo_active && !ui::ui_helpers::controller::get( ).elements.is_any_combo_active && !ui::ui_helpers::controller::get( ).elements.is_color_active;

	if ( ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 5, this->position.y + 3 ), vec2_t( 10, 10 ) ) && ui::ui_helpers::key_was_pressed( VK_LBUTTON ) && can )
		*this->value = !*this->value;
}

void checkbox::draw( ) {
	draw_list->rect_init( this->position.x + 5, this->position.y + 3, 10, 10, Color( 67, 67, 75, 255 ) );
	draw_list->text( this->position.x + 25, this->position.y + 1, draw_list->fonts.main_font, this->name, *this->value ? Color( 152, 150, 159, 255 ) : Color( 65, 66, 74, 255 ) );

	if ( *this->value )
	{
		draw_list->gradient_vertical_init( this->position.x + 5, this->position.y + 3, 10, 10, Color( 255, 0, 0 ), Color( 180, 0, 0 ) );
		draw_list->rect_init( this->position.x + 5, this->position.y + 3, 10, 10, Color( 120, 0, 0, 255 ) );
	}
}

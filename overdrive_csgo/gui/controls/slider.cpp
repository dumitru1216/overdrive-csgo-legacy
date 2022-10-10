#include "slider.h"
#include "../ui.h"

constexpr short int slider_size = 220;

slider::slider( ) { }

slider::slider( std::string name, float* value, int minimum_value, int maximum_value )
{
	this->name = name;
	this->value = value;
	this->minimum_value = minimum_value;
	this->maximum_value = maximum_value;
}

void slider::update( )
{
	bool can = ui::ui_helpers::controller::get( ).group.can_scroll && !ui::ui_helpers::controller::get( ).elements.is_any_multi_combo_active && !ui::ui_helpers::controller::get( ).elements.is_any_combo_active && !ui::ui_helpers::controller::get( ).elements.is_color_active;

	static auto stored_index = -1;
	auto max_min_delta = this->maximum_value - this->minimum_value;

	if ( ui::ui_helpers::key_was_pressed( VK_LBUTTON ) & 1 && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x, this->position.y + 2 ), vec2_t( slider_size, 8 ) ) && can )
		stored_index = ui::ui_helpers::controller::get( ).index;

	if ( GetAsyncKeyState( VK_LBUTTON ) && stored_index == ui::ui_helpers::controller::get( ).index )
	{
		*this->value = this->minimum_value + max_min_delta * ( ui::ui_helpers::controller::get( ).mouse_pos.x - this->position.x - 2 ) / slider_size;

		if ( *this->value < this->minimum_value )
			*this->value = this->minimum_value;

		if ( *this->value > this->maximum_value )
			*this->value = this->maximum_value;
	}

	if ( !GetAsyncKeyState( VK_LBUTTON ) )
		stored_index = -1;
}

/* posible position */
struct pos {
	int x, y;
};

void mouse_pos( pos& position ) {
	RECT rect;
	GetWindowRect( GetActiveWindow( ), &rect );

	POINT Pos;
	GetCursorPos( &Pos );

	position.x = Pos.x - rect.left;
	position.y = Pos.y - rect.top;
}

void slider::draw( )
{
	auto value_min_delta = *this->value - this->minimum_value;
	auto max_min_delta = this->maximum_value - this->minimum_value;

	auto value_modifier = ( value_min_delta / max_min_delta ) * slider_size;
	bool is_override = ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 5, this->position.y + 2 ), vec2_t( slider_size, 8 ) );

	bool value = *this->value >= 0.1f;

	/* posible position */
	pos slider_position;
	mouse_pos( slider_position );
	int posible_position = ( slider_position.x - this->position.x );

	if ( ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 5, this->position.y + 2 ), vec2_t( slider_size, 8 ) ) ) {
		draw_list->gradient_vertical_init( this->position.x + 5, this->position.y + 2, posible_position, 8, Color( 255, 0, 0, 120 ), Color( 180, 0, 0, 120 ) );
	}

	draw_list->gradient_vertical_init( this->position.x + 5, this->position.y + 2, value_modifier, 8, Color( 255, 0, 0 ), Color( 180, 0, 0 ) );
	draw_list->rect_init( this->position.x + 5, this->position.y + 2, slider_size, 8, value ? Color( 120, 0, 0 ) : Color( 67, 67, 75, 255 ) );

	std::string add = "";

	if ( is_override )
	{
		add = ": " + std::to_string( ( int )*this->value );
		add = add + "%";
	}

	draw_list->text( this->position.x + 5, this->position.y - 13, draw_list->fonts.main_font, this->name + add, value ? Color( 152, 150, 159, 255 ) : Color( 65, 66, 74, 255 ) );
}
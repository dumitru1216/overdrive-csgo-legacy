#include "combo.h"
#include "../ui.h"

#include <algorithm>

static short int stored_index = -1;
static bool open = false;

constexpr short int combobox_size_ = 220;
static int animation[ max_elements ];

static int val[ 1 ];

combobox::combobox( ) { }

combobox::combobox( std::string name, int* value, std::vector<std::string> items, int current_element )
{
	this->name = name;
	this->value = value;
	this->items = items;
	this->current_element = current_element;
}

void combobox::update( )
{
	if ( !ui::ui_helpers::controller::get( ).elements.can_open_any_combo || ui::ui_helpers::controller::get( ).elements.is_any_multi_combo_active )
	{
		animation[ this->current_element ] = 0;
		open = false;
		this->focused = false;
	}

	if ( open && stored_index == ui::ui_helpers::controller::get( ).index && ui::ui_helpers::key_was_pressed( VK_LBUTTON ) /*&& !ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 5, this->position.y + 5 ), vec2_t( combobox_size_, 18 ) )*/ && !ui::ui_helpers::controller::get( ).elements.is_any_multi_combo_active )
	{
		open = !open;
		stored_index = -1;

		ui::ui_helpers::controller::get( ).elements.is_combo_active[ this->current_element ] = open;
		ui::ui_helpers::controller::get( ).elements.is_any_combo_active = open;
	}

	if ( val[ 0 ] == -1 && ui::ui_helpers::key_was_pressed( VK_LBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 5, this->position.y + 5 ), vec2_t( combobox_size_, 18 ) ) && !ui::ui_helpers::controller::get( ).elements.is_any_multi_combo_active )
	{
		open = !open;
		stored_index = ui::ui_helpers::controller::get( ).index;
	}

	if ( val[ 0 ] >= 35 )
		val[ 0 ] = 35;
	else if ( val[ 0 ] <= 0 )
		val[ 0 ] = 0;

	if ( open )
		val[ 0 ]++;
	else
		val[ 0 ]--;

	std::clamp( val[ 0 ], -1, 36 );

	ui::ui_helpers::controller::get( ).last.open[ 0 ] = val[ 0 ];

	if ( stored_index == ui::ui_helpers::controller::get( ).index )
	{
		this->focused = open;

		ui::ui_helpers::controller::get( ).elements.is_combo_active[ this->current_element ] = open;
		ui::ui_helpers::controller::get( ).elements.is_any_combo_active = open;
	}

	if ( animation[ this->current_element ] != 0 && ui::ui_helpers::controller::get( ).elements.can_open_any_combo )
	{
		for ( int i = 0; i < this->items.size( ); i++ )
		{
			if ( ui::ui_helpers::key_was_pressed( VK_LBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x, this->position.y + 23 + ( i * 19 ) ), vec2_t( combobox_size_, 18 ) ) )
			{
				*this->value = i;
			}
		}
	}
}

void combobox::draw( )
{
	auto arrow = [ ]( int x, int y, bool turn ) {
		if ( turn )
		{
			for ( auto i = 10; i >= 4; --i ) {
				auto offset = 10 - i;
				draw_list->line_init( x + offset, y + offset, x + offset + std::clamp( i - offset, 0, 10 ), y + offset, Color( 72, 72, 72, 255 ) );
			}
		}
		else
		{
			draw_list->line_init( x + 4, y, x + 6, y, Color( 72, 72, 72, 255 ) );
			draw_list->line_init( x + 3, y + 1, x + 7, y + 1, Color( 72, 72, 72, 255 ) );
			draw_list->line_init( x + 2, y + 2, x + 8, y + 2, Color( 72, 72, 72, 255 ) );
			draw_list->line_init( x + 1, y + 3, x + 9, y + 3, Color( 72, 72, 72, 255 ) );
			draw_list->line_init( x, y + 4, x + 10, y + 4, Color( 72, 72, 72, 255 ) );
		}
	};

	draw_list->gradient_vertical_init( this->position.x + 5, this->position.y + 5, combobox_size_, 18, Color( 20, 20, 20 ), Color( 25, 25, 25 ) );
	draw_list->rect_init( this->position.x + 5, this->position.y + 5, combobox_size_, 18, Color( 180, 0, 0, 255 ) );
	draw_list->text( this->position.x + 4 + 5, this->position.y + 7, draw_list->fonts.main_font, this->items[ *this->value ], Color( 152, 150, 159, 255 ) );
	draw_list->text( this->position.x + 5, this->position.y - 10, draw_list->fonts.main_font, this->name, Color( 152, 150, 159, 255 ) );

	if ( ui::ui_helpers::controller::get( ).elements.can_open_any_combo )
		arrow( this->position.x + 207, this->position.y + 12, this->focused ? false : true );
	else
		arrow( this->position.x + 207, this->position.y + 12, true );

	if ( this->focused )
		animation[ this->current_element ]++;
	else
		animation[ this->current_element ]--;

	if ( animation[ this->current_element ] > 19 )
		animation[ this->current_element ] = 19;

	if ( animation[ this->current_element ] < 0 )
		animation[ this->current_element ] = 0;

	std::clamp<int>( animation[ this->current_element ], 0, 19 );

	if ( animation[ this->current_element ] != 0 && ui::ui_helpers::controller::get( ).elements.can_open_any_combo )
	{
		for ( int i = 0; i < this->items.size( ); i++ )
		{
			auto desing = 23 + ( animation[ this->current_element ] * i );

			if ( i != 0 )
				ui::external_rendering::line_to_draw.insert( ui::external_rendering::line_to_draw.begin( ), ui::external_rendering::line_draw_t( this->position.x + 5, this->position.y + desing, this->position.x + 225, this->position.y + desing, Color( 180, 0, 0, 255 ) ) );

			ui::external_rendering::gradient_to_draw.insert( ui::external_rendering::gradient_to_draw.begin( ), ui::external_rendering::gradient_draw_t( this->position.x + 5, this->position.y + 23 + ( animation[ this->current_element ] * i ) + 1, combobox_size_, 18, Color( 24, 23, 25 ), Color( 30, 29, 31 ) ) );
			ui::external_rendering::outlines_to_draw.insert( ui::external_rendering::outlines_to_draw.begin( ), ui::external_rendering::outline_draw_t( this->position.x + 5, this->position.y + 23, combobox_size_, this->items.size( ) * animation[ this->current_element ], Color( 180, 0, 0, 255 ) ) );

			ui::external_rendering::text_to_draw.insert( ui::external_rendering::text_to_draw.begin( ), ui::external_rendering::text_draw_t( this->position.x + 9, this->position.y + 24 + ( i * animation[ this->current_element ] ) + 1, draw_list->fonts.main_font, this->items[ i ], i == *this->value ? Color( 255, 0, 0, 255 ) : Color( 152, 150, 159, 255 ) ) );
		}
	}
}
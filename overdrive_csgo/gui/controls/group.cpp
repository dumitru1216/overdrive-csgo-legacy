#include "group.h"
#include "../ui.h"

/* control includes */
#include "checkbox.h"
#include "color_selector.h"
#include "combo.h"
#include "keybind.h"
#include "multi.h"
#include "slider.h"
#include "buton.h"
#include "text_input.h"

static short int outer_position[ max_elements ];

groupbox::groupbox( ) { }

groupbox::groupbox( std::string name, vec2_t position, vec2_t size, int current_element, int content_height, int tab )
{
	this->name = name;
	this->position = position;
	this->size = size;
	this->tab = tab;
	this->content_height = content_height;
	this->current_element = current_element;
}

void groupbox::run( )
{
	this->control_offset_x = this->position.x + 7;
	this->control_offset_y[ this->current_element ] = this->position.y + 8 + outer_position[ this->current_element ];

	this->draw( );
}

void groupbox::draw( )
{
	auto text_size = draw_list->get_text_width( this->name.c_str( ), draw_list->fonts.second_font );

	/* render */
	draw_list->rect_fill_init( this->position.x, this->position.y, this->size.x, this->size.y, Color( 22, 22, 22 ) );

	draw_list->line_init( this->position.x, this->position.y, this->position.x + this->size.x - 15 - text_size, this->position.y, Color( 70, 70, 70, 255 ) );
	draw_list->line_init( this->position.x + this->size.x, this->position.y, this->position.x + this->size.x - 28 + 21, this->position.y, Color( 70, 70, 70, 255 ) );
	draw_list->line_init( this->position.x, this->position.y, this->position.x, this->position.y + this->size.y, Color( 70, 70, 70, 255 ) );
	draw_list->line_init( this->position.x + this->size.x, this->position.y, this->position.x + this->size.x, this->position.y + this->size.y, Color( 70, 70, 70, 255 ) );
	draw_list->line_init( this->position.x, this->position.y + this->size.y, this->position.x + this->size.x, this->position.y + this->size.y, Color( 70, 70, 70, 255 ) );

	/* outline */
	draw_list->line_init( this->position.x - 1, this->position.y - 1, this->position.x + this->size.x - 15 - text_size - 1, this->position.y - 1, Color( 0, 0, 0, 255 ) );
	draw_list->line_init( this->position.x + this->size.x + 1, this->position.y - 1, this->position.x + this->size.x - 28 + 21, this->position.y - 1, Color( 0, 0, 0, 255 ) );
	draw_list->line_init( this->position.x - 1, this->position.y - 1, this->position.x - 1, this->position.y + this->size.y - 1, Color( 0, 0, 0, 255 ) );
	draw_list->line_init( this->position.x + this->size.x + 1, this->position.y + 1, this->position.x + this->size.x + 1, this->position.y + this->size.y + 1, Color( 0, 0, 0, 255 ) );
	draw_list->line_init( this->position.x + 1, this->position.y + this->size.y + 1, this->position.x + this->size.x + 1, this->position.y + this->size.y + 1, Color( 0, 0, 0, 255 ) );


	draw_list->text( this->position.x + this->size.x - 10 - text_size, this->position.y - 8, draw_list->fonts.second_font, this->name.c_str( ), Color( 162, 160, 163, 255 ) );
}

void groupbox::add_control( checkbox* check )
{
	if ( this->visible )
	{
		D3DVIEWPORT9 old_viewport = draw_list->get_viewport( );
		draw_list->set_viewport( { ( DWORD )position.x, ( DWORD )position.y + 5, ( DWORD )size.x, ( DWORD )size.y - 10, 0.f, 1.f } );

		check->position = vec2_t( this->control_offset_x, this->control_offset_y[ this->current_element ] );

		if ( ui::ui_helpers::mouse_pointer( vec2_t( position.x, position.y + 5 ), vec2_t( size.x, size.y - 10 ) ) )
			check->update( );

		check->draw( );

		this->control_offset_y[ this->current_element ] += 25;
		this->control_element[ this->current_element ] += 25;

		ui::ui_helpers::controller::get( ).index++;

		draw_list->set_viewport( old_viewport );
	}

	delete check;
}

void groupbox::add_control( slider* slider )
{
	if ( this->visible )
	{
		D3DVIEWPORT9 old_viewport = draw_list->get_viewport( );
		draw_list->set_viewport( { ( DWORD )position.x, ( DWORD )position.y + 5, ( DWORD )size.x, ( DWORD )size.y - 10, 0.f, 1.f } );

		slider->position = vec2_t( this->control_offset_x, this->control_offset_y[ this->current_element ] + 10 );

		if ( ui::ui_helpers::mouse_pointer( vec2_t( position.x, position.y + 5 ), vec2_t( size.x, size.y - 10 ) ) )
			slider->update( );

		slider->draw( );

		this->control_offset_y[ this->current_element ] += 32;
		this->control_element[ this->current_element ] += 32;

		ui::ui_helpers::controller::get( ).index++;

		draw_list->set_viewport( old_viewport );
	}

	delete slider;
}

void groupbox::add_control( combobox* combobox )
{
	if ( this->visible )
	{
		D3DVIEWPORT9 old_viewport = draw_list->get_viewport( );
		combobox->position = vec2_t( this->control_offset_x, this->control_offset_y[ this->current_element ] + 7 );

		if ( !ui::ui_helpers::controller::get( ).elements.is_combo_active[ combobox->current_element ] )
			draw_list->set_viewport( { ( DWORD )position.x, ( DWORD )position.y + 5, ( DWORD )size.x, ( DWORD )size.y - 10, 0.f, 1.f } );

		if ( !ui::ui_helpers::controller::get( ).elements.is_combo_active[ combobox->current_element ] && !ui::ui_helpers::controller::get( ).elements.is_any_combo_active && ui::ui_helpers::controller::get( ).elements.can_open_any_combo )
		{
			ui::ui_helpers::controller::get( ).group.can_scroll = true;

			if ( ui::ui_helpers::mouse_pointer( vec2_t( position.x, position.y + 5 ), vec2_t( size.x, size.y - 10 ) ) )
				combobox->update( );
		}
		else if ( ui::ui_helpers::controller::get( ).elements.is_combo_active[ combobox->current_element ] )
		{
			ui::ui_helpers::controller::get( ).group.can_scroll = false;
			combobox->update( );
		}

		combobox->draw( );

		this->control_offset_y[ this->current_element ] += 42;
		this->control_element[ this->current_element ] += 42;

		ui::ui_helpers::controller::get( ).index++;

		draw_list->set_viewport( old_viewport );
	}

	delete combobox;
}

void groupbox::add_control( multi* multicombobox )
{
	if ( this->visible )
	{
		D3DVIEWPORT9 old_viewport = draw_list->get_viewport( );
		multicombobox->position = vec2_t( this->control_offset_x, this->control_offset_y[ this->current_element ] + 7 );

		if ( !ui::ui_helpers::controller::get( ).elements.is_multi_combo_active[ ui::ui_helpers::controller::get( ).index ] && !ui::ui_helpers::controller::get( ).elements.is_any_multi_combo_active && ui::ui_helpers::controller::get( ).elements.can_open_any_combo )
		{
			ui::ui_helpers::controller::get( ).group.can_scroll = true;

			if ( ui::ui_helpers::mouse_pointer( vec2_t( position.x, position.y + 5 ), vec2_t( size.x, size.y - 10 ) ) )
				multicombobox->update( );
		}
		else if ( ui::ui_helpers::controller::get( ).elements.is_multi_combo_active[ ui::ui_helpers::controller::get( ).index ] )
		{
			ui::ui_helpers::controller::get( ).group.can_scroll = false;
			multicombobox->update( );
		}

		if ( !ui::ui_helpers::controller::get( ).elements.is_multi_combo_active[ ui::ui_helpers::controller::get( ).index ] )
			draw_list->set_viewport( { ( DWORD )position.x, ( DWORD )position.y + 5, ( DWORD )size.x, ( DWORD )size.y - 10, 0.f, 1.f } );

		multicombobox->draw( );

		this->control_offset_y[ this->current_element ] += 42;
		this->control_element[ this->current_element ] += 42;

		ui::ui_helpers::controller::get( ).index++;

		draw_list->set_viewport( old_viewport );
	}

	delete multicombobox;
}

void groupbox::add_control( color_picker* color )
{
	if ( this->visible )
	{
		D3DVIEWPORT9 old_viewport = draw_list->get_viewport( );
		draw_list->set_viewport( { ( DWORD )position.x, ( DWORD )position.y + 5, ( DWORD )size.x, ( DWORD )size.y - 10, 0.f, 1.f } );

		color->position = vec2_t( this->control_offset_x, this->control_offset_y[ this->current_element ] - 23 );

		if ( !ui::ui_helpers::controller::get( ).elements.is_color_active && ui::ui_helpers::mouse_pointer( vec2_t( position.x, position.y + 5 ), vec2_t( size.x, size.y - 10 ) ) )
			color->update( );
		else if ( ui::ui_helpers::controller::get( ).elements.is_color_active )
			color->update( );

		color->draw( );

		ui::ui_helpers::controller::get( ).index++;

		draw_list->set_viewport( old_viewport );
	}
}

void groupbox::add_control( keybind* keybind )
{
	if ( this->visible )
	{
		D3DVIEWPORT9 old_viewport = draw_list->get_viewport( );
		draw_list->set_viewport( { ( DWORD )position.x, ( DWORD )position.y + 5, ( DWORD )size.x, ( DWORD )size.y - 10, 0.f, 1.f } );

		keybind->position = vec2_t( this->control_offset_x, this->control_offset_y[ this->current_element ] - 24 );

		if ( !ui::ui_helpers::controller::get( ).elements.is_list_active[ keybind->current_element ] && ui::ui_helpers::mouse_pointer( vec2_t( position.x, position.y + 5 ), vec2_t( size.x, size.y - 10 ) ) )
		{
			ui::ui_helpers::controller::get( ).group.can_scroll = true;
			keybind->update( );
		}
		else if ( ui::ui_helpers::controller::get( ).elements.is_list_active[ keybind->current_element ] )
		{
			ui::ui_helpers::controller::get( ).group.can_scroll = false;
			keybind->update( );
		}

		keybind->draw( );
		ui::ui_helpers::controller::get( ).index++;

		draw_list->set_viewport( old_viewport );

		keybind->list( );
	}

	delete keybind;
}

void groupbox::add_control( text_input* text )
{
	if ( this->visible )
	{
		D3DVIEWPORT9 old_viewport = draw_list->get_viewport( );
		draw_list->set_viewport( { ( DWORD )position.x, ( DWORD )position.y + 5, ( DWORD )size.x, ( DWORD )size.y - 10, 0.f, 1.f } );

		text->position = vec2_t( this->control_offset_x, this->control_offset_y[ this->current_element ] + 7 );

		if ( ui::ui_helpers::mouse_pointer( vec2_t( position.x, position.y + 5 ), vec2_t( size.x, size.y - 10 ) ) )
			text->update( );

		text->draw( );

		this->control_offset_y[ this->current_element ] += 42;
		this->control_element[ this->current_element ] += 42;

		ui::ui_helpers::controller::get( ).index++;

		draw_list->set_viewport( old_viewport );
	}

	delete text;
}

void groupbox::add_control( button* button )
{
	if ( this->visible )
	{
		D3DVIEWPORT9 old_viewport = draw_list->get_viewport( );
		draw_list->set_viewport( { ( DWORD )position.x, ( DWORD )position.y + 5, ( DWORD )size.x, ( DWORD )size.y - 10, 0.f, 1.f } );

		button->position = vec2_t( this->control_offset_x, this->control_offset_y[ this->current_element ] - 7 );

		if ( ui::ui_helpers::mouse_pointer( vec2_t( position.x, position.y + 5 ), vec2_t( size.x, size.y - 10 ) ) )
			button->update( );

		button->draw( );

		this->control_offset_y[ this->current_element ] += 38;
		this->control_element[ this->current_element ] += 38;

		ui::ui_helpers::controller::get( ).index++;

		draw_list->set_viewport( old_viewport );
	}

	delete button;
}

void groupbox::init_last_element( color_picker* color )
{
	if ( this->visible )
		color->init( );

	delete color;
}

/* scroll */
void groupbox::scroll_process( )
{
	if ( this->visible && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x, this->position.y ), vec2_t( this->size.x, this->size.y ) ) && ui::ui_helpers::controller::get( ).group.can_scroll && !ui::ui_helpers::controller::get( ).elements.is_any_multi_combo_active && !ui::ui_helpers::controller::get( ).elements.is_any_combo_active && !ui::ui_helpers::controller::get( ).elements.is_color_active && !ui::ui_helpers::controller::get( ).elements.is_any_list_active )
	{
		if ( ui::ui_helpers::controller::get( ).mouse_wheel_activity != 0 ) {
			if ( this->control_element[ this->current_element ] > this->size.y - 8 ) {

				if ( this->content_height > 0 )
					outer_position[ this->current_element ] += ui::ui_helpers::controller::get( ).mouse_wheel_activity * 10;

				ui::ui_helpers::controller::get( ).mouse_wheel_activity = 0;

				if ( outer_position[ this->current_element ] > 0 )
					outer_position[ this->current_element ] = 0;

				if ( outer_position[ this->current_element ] < ( size.y - 8 ) - this->control_element[ this->current_element ] )
					outer_position[ this->current_element ] = ( size.y - 8 ) - this->control_element[ this->current_element ];
			}
		}
	}

	if ( this->visible )
	{
		int max_scroll_value = this->control_element[ this->current_element ] - size.y + 10.f;
		float scroll_pos = ( ( float )outer_position[ this->current_element ] / ( float )this->control_element[ this->current_element ] ) * ( size.y - 20.f ) * ( -1 );
		auto scroll_pos_max = max_scroll_value / ( float )this->control_element[ this->current_element ] * ( size.y - 20.f );

		static int old; // ghetto fix

		if ( old != max_scroll_value )
		{
			old = max_scroll_value;

			if ( outer_position[ this->current_element ] > 0 )
				outer_position[ this->current_element ] = 0;

			if ( outer_position[ this->current_element ] < ( size.y - 8 ) - this->control_element[ this->current_element ] )
				outer_position[ this->current_element ] = ( size.y - 8 ) - this->control_element[ this->current_element ];
		}

		if ( this->content_height > 0 /*  */ )
			draw_list->rect_fill_init( this->position.x + size.x - 4, this->position.y + scroll_pos + 3, 2, this->size.y - scroll_pos_max - 4, Color( 70, 70, 70, 255 ) );
	}
}
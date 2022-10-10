/* includes */
#include "window.h"
#include "group.h"
#include "../ui.h"

/* predefine */
window::window( ) { }

/* setup window -> full */
window::window( std::string name, vec2_t* position, vec2_t size, std::vector<std::string> window_tabs, int* selected_tab )
{
	this->name = name;
	this->window_position = position;
	this->size = size;
	this->window_tabs = window_tabs;
	this->selected_tab = selected_tab;

	this->run( );
}

void window::run( )
{
	this->handle_position( );
	this->draw( );
	this->handle_tabs( );
}

void window::draw( )
{
	/* animation part */
	static int window_animation = 125;
	if ( ui::ui_helpers::mouse_pointer( vec2_t( this->window_position->x, this->window_position->y ), vec2_t( this->size.x, this->size.y ) ) ) {
		if ( window_animation < 255 ) window_animation++;
	}
	else {
		if ( window_animation > 2 ) window_animation--;
	}

	draw_list->rect_fill_init( this->window_position->x, this->window_position->y, this->size.x, this->size.y, Color( 15, 15, 15, 255 ) );
	draw_list->rect_fill_init( this->window_position->x, this->window_position->y, this->size.x, 25, Color( 20, 20, 20, 255 ) );
	draw_list->rect_fill_init( this->window_position->x + 1, this->window_position->y + 25, this->size.x - 1, 1, Color( 255, 0, 0, 255 ) );
	draw_list->rect_init( this->window_position->x, this->window_position->y, this->size.x, this->size.y, Color( 255, 0, 0, window_animation ) );
	draw_list->text( this->window_position->x + 12, this->window_position->y + 6, draw_list->fonts.main_font, this->name, Color( 147, 147, 155, 255 ) );
}

void window::handle_tabs( )
{
	size_t add = { 285 };

	if ( this->window_tabs.size( ) > 0 )
	{
		auto tab_size = ( this->size.x / 2 ) / this->window_tabs.size( );

		for ( int i = 0; i < this->window_tabs.size( ); i++ )
		{
			RECT tab_position = { this->window_position->x + add + ( i * tab_size ), this->window_position->y - 1, tab_size, 26 };

			if ( ui::ui_helpers::mouse_pointer( vec2_t( tab_position.left, tab_position.top ), vec2_t( tab_position.right, tab_position.bottom ) ) && ui::ui_helpers::key_was_pressed( VK_LBUTTON ) )
				*this->selected_tab = i;

			Color color = i == *selected_tab ? Color( 147, 147, 155, 255 ) : Color( 68, 68, 76, 255 );

			auto size_in_int = std::strlen( this->window_tabs[ i ].c_str( ) );
			auto text_size = draw_list->get_text_width( this->window_tabs[ i ].c_str( ), draw_list->fonts.main_font );

			short int re_sizer = 0;

			if ( i == 3 )
				re_sizer = 8;
			else if ( i == 4 )
				re_sizer = 18;

			if ( i != 4 )
				draw_list->text( tab_position.left - re_sizer + ( text_size / 2 ) + 10 + ( tab_position.right / 2 ), tab_position.top + 7, draw_list->fonts.main_font, "/", Color( 68, 68, 76, 255 ), true );

			draw_list->text( tab_position.left - re_sizer + ( tab_position.right / 2 ), tab_position.top + 7, draw_list->fonts.main_font, this->window_tabs[ i ], color, true );
		}
	}
}

void window::handle_position( )
{
	static bool drag = false;
	static vec2_t drag_offset;

	auto delta = ui::ui_helpers::controller::get( ).mouse_pos - drag_offset;

	if ( drag && !GetAsyncKeyState( VK_LBUTTON ) )
		drag = false;

	if ( drag && GetAsyncKeyState( VK_LBUTTON ) )
		*this->window_position = delta;

	if ( ui::ui_helpers::mouse_pointer( vec2_t( this->window_position->x, this->window_position->y ), vec2_t( this->size.x, 20 ) ) )
	{
		drag = true;
		drag_offset = ui::ui_helpers::controller::get( ).mouse_pos - *this->window_position;
	}

	if ( this->window_position->x < 0 )
		this->window_position->x = 0;

	if ( this->window_position->y < 0 )
		this->window_position->y = 0;

	static int size_w, size_h;
	g_csgo.engine()->GetScreenSize( size_w, size_h );

	if ( ( this->window_position->x + this->size.x ) > size_w )
		this->window_position->x = size_w - this->size.x;

	if ( ( this->window_position->y + this->size.y ) > size_h )
		this->window_position->y = size_h - this->size.y;
}

void window::add_groupbox( groupbox* groupbox )
{
	groupbox->visible = groupbox->tab == *this->selected_tab;

	if ( groupbox->visible )
	{
		groupbox->position += *this->window_position;
		groupbox->run( );
	}
}
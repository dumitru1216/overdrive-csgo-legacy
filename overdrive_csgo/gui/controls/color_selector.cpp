#include "color_selector.h"
#include "init.h"
#include "../ui.h"

constexpr int add = 215;

constexpr float m_pi = 3.141592653589793;
static float brightness = 255;

static bool open = false;
static short int stored_index = -1;

namespace r_math
{
	float Rad_2_Deg( float rad )
	{
		return ( rad + m_pi ) / ( 2 * m_pi ) * 360;
	}

	float XY2_Polar_1( int x, int y )
	{
		float r = sqrt( x * x + y * y );
		return r;
	}

	float XY2_Polar_2( int x, int y )
	{
		float phi = atan2( y, x );
		return phi;
	}

	Color HSV_TO_RGB( float hue, float saturation, int value, int calc )
	{
		float chroma = value * saturation;
		float hue1 = hue / 60;
		float x = chroma * ( 1 - abs( fmod( hue1, 2.0 ) - 1 ) );
		float r1, g1, b1;
		r1 = 0; g1 = 0; b1 = 0;
		if ( hue1 >= 0 && hue1 <= 1 ) {
			r1 = chroma;
			g1 = x;
			b1 = 0;
		}
		else if ( hue1 >= 1 && hue1 <= 2 ) {
			r1 = x;
			g1 = chroma;
			b1 = 0;
		}
		else if ( hue1 >= 2 && hue1 <= 3 ) {
			r1 = 0;
			g1 = chroma;
			b1 = x;
		}
		else if ( hue1 >= 3 && hue1 <= 4 ) {
			r1 = 0;
			g1 = x;
			b1 = chroma;
		}
		else if ( hue1 >= 4 && hue1 <= 5 ) {
			r1 = x;
			g1 = 0;
			b1 = chroma;
		}
		else if ( hue1 >= 5 && hue1 <= 6 ) {
			r1 = chroma;
			g1 = 0;
			b1 = x;
		}

		float m = value - chroma;
		float r, g, b;
		r = 0.1; g = 0.1; b = 0.1;
		r = r1 + m;
		g = g1 + m;
		b = b1 + m;

		return Color( r * brightness, g * brightness, b * brightness );
	}

}

namespace in_render
{
	void r_circle( int x1, int y1, int radius )
	{
		int rad = radius;
		int x, y;

		for ( x = -rad; x < rad; x++ ) {
			for ( y = -rad; y < rad; y++ ) {
				float r = r_math::XY2_Polar_1( x, y );
				float phi = r_math::XY2_Polar_2( x, y );

				if ( r > rad )
					continue;

				float deg = r_math::Rad_2_Deg( phi );

				float adjustedX = x + rad + x1;
				float adjustedY = y + rad + y1;

				float hue = deg;
				float saturation = r / rad;

				draw_list->rect_init( adjustedX, adjustedY, 1, 1, r_math::HSV_TO_RGB( hue, saturation, 1.0, 0 ) );
			}
		}
	}
}

color_picker::color_picker( ) { }

color_picker::color_picker( std::string name, float* red, float* green, float* blue, float* alpha, bool alpha_bar )
{
	this->name = name;
	this->red = red;
	this->green = green;
	this->blue = blue;
	this->alpha = alpha;
	this->alpha_bar = alpha_bar;
}

void color_picker::update( )
{
	if ( open )
	{
		ui::ui_helpers::controller::get( ).elements.can_open_list = false;
		ui::ui_helpers::controller::get( ).elements.is_color_active = true;
		ui::ui_helpers::controller::get( ).group.can_scroll = false;
		ui::ui_helpers::controller::get( ).elements.can_open_any_combo = false;
	}
	else
	{
		ui::ui_helpers::controller::get( ).elements.can_open_list = true;
		ui::ui_helpers::controller::get( ).elements.is_color_active = false;
		ui::ui_helpers::controller::get( ).group.can_scroll = true;
		ui::ui_helpers::controller::get( ).elements.can_open_any_combo = true;
	}

	if ( open && stored_index == ui::ui_helpers::controller::get( ).index && !ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 240 - 30, this->position.y ), vec2_t( 15, 10 ) ) && !ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 16 + add, this->position.y ), vec2_t( 188, 207 ) ) && ui::ui_helpers::key_was_pressed( VK_LBUTTON ) )
	{
		open = !open;
		stored_index = -1;
	}

	if ( ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 240 - 30, this->position.y ), vec2_t( 15, 10 ) ) && ui::ui_helpers::key_was_pressed( VK_LBUTTON ) && ui::ui_helpers::controller::get( ).elements.can_open_color )
	{
		open = !open;
		stored_index = ui::ui_helpers::controller::get( ).index;
	}

	if ( stored_index == ui::ui_helpers::controller::get( ).index )
		this->focused = open;
}

void color_picker::draw( )
{
	draw_list->rect_fill_init( this->position.x + 240 - 30, this->position.y, 15, 10, Color( *red, *green, *blue ) );
	draw_list->rect_init( this->position.x + 240 - 30, this->position.y, 15, 10, Color( 67, 67, 75 ) );
}

void color_picker::init( )
{
	auto draw_slider = [ ]( int x, int y, int m_max, const char* text, float* item ) -> void
	{
		constexpr float size = 170;
		float in_pixel = m_max / size;

		if ( GetAsyncKeyState( VK_LBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( x + 20 - 3, y + 14 ), vec2_t( size + ( 3 * 2 ), 8 ) ) )
			*item = abs( ui::ui_helpers::controller::get( ).mouse_pos.x - ( x + 20 ) ) * in_pixel;

		if ( *item > m_max )
			*item = m_max;

		if ( *item < 0 )
			*item = 0;

		if ( *item )
		{
			if ( *item == m_max )
				draw_list->gradient_vertical_init( x + 22, y + 16, size - 3, 5, Color( 255, 0, 0 ), Color( 180, 0, 0 ) );
			else
				draw_list->gradient_vertical_init( x + 22, y + 16, ( *item / in_pixel ) - 3, 5, Color( 255, 0, 0 ), Color( 180, 0, 0 ) );
		}

		draw_list->rect_init( x + 20, y + 14, size, 8, Color( 180, 0, 0 ) );
		draw_list->text( x + 19, y, draw_list->fonts.main_font, text, Color( 152, 150, 159, 255 ) );
	};

	if ( this->focused )
	{
		draw_list->rect_fill_init( this->position.x + 16 + add, this->position.y - 1, 188, 207, Color( 25, 25, 25 ) );
		draw_list->rect_init( this->position.x + 17 + add, this->position.y, 186, 205, Color( 103, 102, 106 ) );
		draw_list->rect_init( this->position.x + 16 + add, this->position.y - 1, 188, 207, Color( 0, 0, 0 ) );

		static int value = 0;

		if ( this->alpha_bar )
			value = 0;
		else
			value = 15;

		draw_slider( this->position.x + 5 + add, this->position.y + 150 + value, 255, "Brightness", &brightness );

		if ( this->alpha_bar )
			draw_slider( this->position.x + 5 + add, this->position.y + 175, 255, "Alpha", this->alpha );

		in_render::r_circle( this->position.x + 47 + add, this->position.y + 10, 65 );

		if ( GetAsyncKeyState( VK_LBUTTON ) && ui::ui_helpers::mouse_pointer( vec2_t( this->position.x + 47 + add, this->position.y + 10 ), vec2_t( 130, 130 ) ) ) {

			HDC hdc = GetDC( nullptr );

			COLORREF selected_colour;
			selected_colour = GetPixel( hdc, ui::ui_helpers::controller::get( ).mouse_pos.x, ui::ui_helpers::controller::get( ).mouse_pos.y );

			if ( GetRValue( selected_colour ) == 26 && GetGValue( selected_colour ) == 21 && GetBValue( selected_colour ) == 57 )
				return;

			if ( GetAsyncKeyState( VK_LBUTTON ) ) {
				*red = GetRValue( selected_colour );
				*green = GetGValue( selected_colour );
				*blue = GetBValue( selected_colour );
			}
		}
	}
}
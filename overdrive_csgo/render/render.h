#pragma once
#include "../hack/sdk/src_kit.h"
#include "../hack/sdk/other/color.h"
#include "../hack/sdk/math/vector2d.h"
#include "../utils/singleton.h"

/* source sdk includes */
#include <array>
#include <mutex>
#include <shared_mutex>

/* struct helpers */
struct vertice_t
{
	float x, y, z, rhw;
	int c;
};

/* class: render */
class render {
public:
	render( IDirect3DDevice9* device )
	{
		this->device = device;
		this->vertex_shader = vertex_shader;

		device->GetViewport( &screen_size );
	}

	IDirect3DVertexShader9* vertex_shader;
	IDirect3DDevice9* device;
	D3DVIEWPORT9 screen_size;

	void init_device( );

	int get_text_width( const char* text, LPD3DXFONT font );
	void text( float x, float y, LPD3DXFONT font, std::string text, Color color, bool centered = false, int drop_shadow = 0, Color shadow_color = Color( ) );

	void line( vec2_t a, vec2_t b, Color c );
	void line_init( float x, float y, float x1, float x2, Color c );

	void rect_fill( vec2_t a, vec2_t b, Color c );
	void rect_fill_init( float x, float y, float w, float h, Color c );

	void rect( vec2_t a, vec2_t b, Color c );
	void rect_init( float x, float y, float w, float h, Color c );

	void gradient_vertical( vec2_t a, vec2_t b, Color c_a, Color c_b );
	void gradient_vertical_init( float x, float y, float w, float h, Color c_a, Color c_b );

	void set_viewport( D3DVIEWPORT9 vp )
	{
		device->SetViewport( &vp );
	}

	D3DVIEWPORT9 get_viewport( )
	{
		D3DVIEWPORT9 vp;
		device->GetViewport( &vp );
		return vp;
	}

	struct
	{
		LPD3DXFONT main_font;
		LPD3DXFONT second_font;
		LPD3DXFONT esp_name_font;
	} fonts;
};

/* class: surface */
class surface : public singleton <surface> {
public:
	void draw_text( int X, int Y, Color Color, int Font, bool Center, const char* _Input, ... );
	void outlined_rect( int x, int y, int w, int h, Color color );
	void filled_rect( int x, int y, int w, int h, Color color );
	void line( int x, int y, int x2, int y2, Color color );

	void add_textured_polygon( int n, Vertex_t* vertice, int r, int g, int b, int a );
	void draw_filled_triangle( std::array< Vector2D, 3 >points, Color colour );

	RECT get_text_size_rect( DWORD font, const char* text );

	struct
	{

		DWORD esp_font;
		DWORD esp_other;
		DWORD weapon_font;
		DWORD watermark;
		DWORD indicator;

	} fonts;
};

/* class: draw_calls */
enum draw_call_type {
	none,
	filled_box,
	bordered_box,
	text_m,
	line_m,
	gradient_box
};
class draw_call_filled_box {
public:

	vec2_t pos;
	vec2_t size;
	Color clr;
};
class draw_call_borded_box {
public:

	vec2_t pos;
	vec2_t size;
	Color clr;
};
class draw_call_line {
public:

	vec2_t pos;
	vec2_t size;
	Color clr;
};
class draw_call_gradient {
public:

	vec2_t pos;
	vec2_t size;
	Color clr;
	Color clr2;
};
class draw_call_text {
public:

	vec2_t pos;
	LPD3DXFONT font;
	std::string text;
	bool centered;
	Color clr;
};
class draw_call {
public:

	draw_call( ) { };
	draw_call_type type = none;

	draw_call_filled_box draw_call_filled_box_init;
	draw_call_borded_box draw_call_borded_box_init;
	draw_call_text draw_call_text_init;
	draw_call_line draw_call_line_init;
	draw_call_gradient draw_call_gradient_init;
};

/* class: d3d9_helper */
class d3d9_render : public singleton <d3d9_render> {
public:
	void add_filled_box( vec2_t pos, vec2_t size, Color c );
	void add_bordered_box( vec2_t pos, vec2_t size, Color c );
	void add_gradient( vec2_t pos, vec2_t size, Color c, Color c2 );
	void add_line( vec2_t pos, vec2_t size, Color c );
	void add_text( vec2_t pos, LPD3DXFONT font, std::string text, Color color, bool centered );

	void begin( );
	void end( );

	void render( IDirect3DDevice9* device );

	std::vector<draw_call> draw_calls;
	std::vector<draw_call> safe_draw_calls;

	mutable std::shared_mutex mutex;
};
extern render* draw_list;
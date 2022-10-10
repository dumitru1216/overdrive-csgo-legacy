/* defines */
#define _CRT_SECURE_NO_WARNINGS

/* includes */
#include "render.h"
#include "../hack/sdk/interfaces/create_if.h"

/* predefine */
render* draw_list;

/* init device */
void render::init_device( ) {
	/* setup shaders */
	device->SetVertexShader( nullptr );
	device->SetPixelShader( nullptr );
	device->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	device->SetRenderState( D3DRS_LIGHTING, false );
	device->SetRenderState( D3DRS_FOGENABLE, false );
	device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	/* setup states */
	device->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	device->SetRenderState( D3DRS_SCISSORTESTENABLE, true );
	device->SetRenderState( D3DRS_ZWRITEENABLE, false );
	device->SetRenderState( D3DRS_STENCILENABLE, false );

	/* setup states for fonts */
	device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, true );
	device->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, true );

	/* setup states for render system */
	device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	device->SetRenderState( D3DRS_ALPHATESTENABLE, false );
	device->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, true );
	device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	device->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA );
	device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	device->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );

	/* setup states for color */
	device->SetRenderState( D3DRS_SRGBWRITEENABLE, false );
	device->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );

	/* setup states for textures */
	device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	/* init samples */
	device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
}

/* render->init_text setup */
int render::get_text_width( const char* text, LPD3DXFONT font ) {
	RECT rcRect = { 0, 0, 0, 0 };
	if ( font )
	{
		font->DrawText( NULL, text, strlen( text ), &rcRect, DT_CALCRECT,
			D3DCOLOR_XRGB( 0, 0, 0 ) );
	}

	return rcRect.right - rcRect.left;
}

void render::text( float x, float y, LPD3DXFONT font, std::string text, Color color, bool centered, int drop_shadow, Color shadow_color ) {
	if ( centered )
		x -= ( get_text_width( text.c_str( ), font ) / 2 );

	RECT DrawArea = { x, y, 1920, DrawArea.top + 200 };

	if ( drop_shadow == 1 )
	{
		RECT DrawArea2 = { x + 1, y, 1920, DrawArea.top + 200 };
		RECT DrawArea4 = { x, y + 1, 1920, DrawArea.top + 200 };
		font->DrawTextA( NULL, text.c_str( ), -1, &DrawArea2, 0, D3DCOLOR_RGBA( shadow_color.red, shadow_color.green, shadow_color.blue, shadow_color.alpha ) );
		font->DrawTextA( NULL, text.c_str( ), -1, &DrawArea4, 0, D3DCOLOR_RGBA( shadow_color.red, shadow_color.green, shadow_color.blue, shadow_color.alpha ) );
	}
	else if ( drop_shadow == 2 )
	{
		RECT DrawArea2 = { x + 1.f, y, 1920, DrawArea.top + 200 };
		RECT DrawArea3 = { x - 1.f, y, 1920, DrawArea.top + 200 };
		RECT DrawArea4 = { x, y + 1.f, 1920, DrawArea.top + 200 };
		RECT DrawArea5 = { x, y - 1.f, 1920, DrawArea.top + 200 };
		font->DrawTextA( NULL, text.c_str( ), -1, &DrawArea2, 0, D3DCOLOR_RGBA( shadow_color.red, shadow_color.green, shadow_color.blue, shadow_color.alpha ) );
		font->DrawTextA( NULL, text.c_str( ), -1, &DrawArea3, 0, D3DCOLOR_RGBA( shadow_color.red, shadow_color.green, shadow_color.blue, shadow_color.alpha ) );
		font->DrawTextA( NULL, text.c_str( ), -1, &DrawArea4, 0, D3DCOLOR_RGBA( shadow_color.red, shadow_color.green, shadow_color.blue, shadow_color.alpha ) );
		font->DrawTextA( NULL, text.c_str( ), -1, &DrawArea5, 0, D3DCOLOR_RGBA( shadow_color.red, shadow_color.green, shadow_color.blue, shadow_color.alpha ) );
	}

	font->DrawTextA( NULL, text.c_str( ), -1, &DrawArea, 0, D3DCOLOR_RGBA( color.red, color.green, color.blue, color.alpha ) );
}

/* render->gradient setup */
void render::gradient_vertical( vec2_t a, vec2_t b, Color c_a, Color c_b ) {
	b += a;

	vertice_t verts[ 4 ] = {
		{ a.x, a.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c_a.red, c_a.green, c_a.blue, c_a.alpha ) },
	{ b.x, a.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c_a.red, c_a.green, c_a.blue, c_a.alpha ) },
	{ a.x, b.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c_b.red, c_b.green, c_b.blue, c_b.alpha ) },
	{ b.x, b.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c_b.red, c_b.green, c_b.blue, c_b.alpha ) }
	};

	device->SetTexture( 0, nullptr );
	device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &verts, 20 );
}

void render::gradient_vertical_init( float x, float y, float w, float h, Color c_a, Color c_b ) {
	gradient_vertical( { x, y }, { w, h }, c_a, c_b );
}

/* render->line setup */
void render::line( vec2_t a, vec2_t b, Color c ) {
	vertice_t verts[ 2 ] = {
		{ a.x, a.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) },
	{ b.x, b.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) }
	};

	device->SetTexture( 0, nullptr );
	device->DrawPrimitiveUP( D3DPT_LINELIST, 1, &verts, 20 );
}

void render::line_init( float x, float y, float x1, float x2, Color c ) {
	line( { x, y }, { x1, x2 }, c );
}

/* render->rect setup */
void render::rect( vec2_t a, vec2_t b, Color c ) {
	b += a;

	b.x -= 1;
	b.y -= 1;

	vertice_t verts[ 5 ] = {
		{ float( a.x ), float( a.y ), 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) },
		{ float( b.x ), float( a.y ), 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) },
		{ float( b.x ), float( b.y ), 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) },
		{ float( a.x ), float( b.y ), 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) },
		{ float( a.x ), float( a.y ), 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) }
	};

	device->SetTexture( 0, nullptr );
	device->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, &verts, 20 );
}

void render::rect_init( float x, float y, float w, float h, Color c ) {
	rect( { x, y }, { w + 1, h + 1 }, c );
}

/* render->filled_rect setup */
void render::rect_fill( vec2_t a, vec2_t b, Color c ) {
	b += a;

	vertice_t verts[ 4 ] = {
		{ a.x, a.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) },
	{ b.x, a.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) },
	{ a.x, b.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) },
	{ b.x, b.y, 0.01f, 0.01f, D3DCOLOR_RGBA( c.red, c.green, c.blue, c.alpha ) }
	};

	device->SetTexture( 0, nullptr );
	device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &verts, 20 );
}

void render::rect_fill_init( float x, float y, float w, float h, Color c ) {
	rect_fill( { x, y }, { w, h }, c );
}

/* start surface */
void surface::draw_text( int X, int Y, Color Color, int Font, bool Center, const char* _Input, ... )
{
	va_list va_alist;
	char buf[ 1024 ];
	va_start( va_alist, _Input );
	_vsnprintf( buf, sizeof( buf ), _Input, va_alist );
	va_end( va_alist );
	wchar_t wbuf[ 1024 ];
	MultiByteToWideChar( CP_UTF8, 0, buf, 256, wbuf, 256 );

	int Width = 0, Height = 0;

	if ( Center )
		g_csgo.surface()->GetTextSize( Font, wbuf, Width, Height );

	g_csgo.surface()->DrawSetTextColor( Color.red, Color.green, Color.blue, Color.alpha );
	g_csgo.surface()->DrawSetTextFont( Font );
	g_csgo.surface()->DrawSetTextPos( X - ( Width / 2 ), Y );
	g_csgo.surface()->DrawPrintText( wbuf, wcslen( wbuf ) );
}

void surface::outlined_rect( int x, int y, int w, int h, Color color )
{
	g_csgo.surface()->DrawSetColor( color.red, color.green, color.blue, color.alpha );
	g_csgo.surface()->DrawOutlinedRect( x, y, x + w, y + h );
}

void surface::filled_rect( int x, int y, int w, int h, Color color )
{
	g_csgo.surface()->DrawSetColor( color.red, color.green, color.blue, color.alpha );
	g_csgo.surface()->DrawFilledRect( x, y, x + w, y + h );
}

RECT surface::get_text_size_rect( DWORD font, const char* text )
{
	size_t origsize = strlen( text ) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[ newsize ];
	mbstowcs_s( &convertedChars, wcstring, origsize, text, _TRUNCATE );

	RECT rect; int x, y;
	g_csgo.surface()->GetTextSize( font, wcstring, x, y );
	rect.left = x; rect.bottom = y;
	rect.right = x;
	return rect;
}

void surface::line( int x, int y, int x2, int y2, Color color ) {
	g_csgo.surface()->DrawSetColor( color.red, color.green, color.blue, color.alpha );
	g_csgo.surface()->DrawLine( x, y, x2, y2 );
}

void surface::add_textured_polygon( int n, Vertex_t* vertice, int r, int g, int b, int a ) {
	static int texture_id = g_csgo.surface()->CreateNewTextureID( true );
	static unsigned char buf[ 4 ] = { 255, 255, 255, 255 };
	g_csgo.surface()->DrawSetColor( r, g, b, a );
	g_csgo.surface()->DrawSetTexture( texture_id );
	g_csgo.surface()->DrawTexturedPolygon( n, vertice );
}

void surface::draw_filled_triangle( std::array< Vector2D, 3 >points, Color colour ) {
	std::array< Vertex_t, 3 >vertices{ Vertex_t( points.at( 0 ) ), Vertex_t( points.at( 1 ) ), Vertex_t( points.at( 2 ) ) };
	add_textured_polygon( 3, vertices.data( ), colour.red, colour.green, colour.blue, colour.alpha );
}

void d3d9_render::add_filled_box( vec2_t pos, vec2_t size, Color c )
{
	draw_call call;

	call.type = filled_box;
	call.draw_call_filled_box_init.pos = pos;
	call.draw_call_filled_box_init.size = size;
	call.draw_call_filled_box_init.clr = c;

	draw_calls.push_back( call );
}

void d3d9_render::add_bordered_box( vec2_t pos, vec2_t size, Color c )
{
	draw_call call;

	call.type = bordered_box;
	call.draw_call_borded_box_init.pos = pos;
	call.draw_call_borded_box_init.size = size;
	call.draw_call_borded_box_init.clr = c;

	draw_calls.push_back( call );
}

void d3d9_render::add_line( vec2_t pos, vec2_t size, Color c )
{
	draw_call call;

	call.type = line_m;
	call.draw_call_line_init.pos = pos;
	call.draw_call_line_init.size = size;
	call.draw_call_line_init.clr = c;

	draw_calls.push_back( call );
}

void d3d9_render::add_text( vec2_t pos, LPD3DXFONT font, std::string text, Color color, bool centered )
{
	draw_call call;

	call.type = text_m;
	call.draw_call_text_init.pos = pos;
	call.draw_call_text_init.font = font;
	call.draw_call_text_init.text = text;
	call.draw_call_text_init.clr = color;
	call.draw_call_text_init.centered = centered;

	if ( call.draw_call_text_init.centered )
		call.draw_call_text_init.pos.x -= ( draw_list->get_text_width( text.c_str( ), font ) / 2 );

	draw_calls.push_back( call );
}

void d3d9_render::add_gradient( vec2_t pos, vec2_t size, Color c, Color c2 )
{
	draw_call call;

	call.type = gradient_box;
	call.draw_call_gradient_init.pos = pos;
	call.draw_call_gradient_init.size = size;
	call.draw_call_gradient_init.clr = c;
	call.draw_call_gradient_init.clr2 = c2;

	draw_calls.push_back( call );
}

void d3d9_render::begin( )
{
	draw_calls.clear( );
}

void d3d9_render::end( )
{
	std::unique_lock<std::shared_mutex> lock( mutex );
	draw_calls.swap( safe_draw_calls );
}

void d3d9_render::render( IDirect3DDevice9* device )
{
	std::unique_lock<std::shared_mutex> lock( mutex );

	for ( draw_call& call : safe_draw_calls )
	{
		switch ( call.type )
		{
		case none:
			break;
		case filled_box:
			draw_list->rect_fill_init( call.draw_call_filled_box_init.pos.x, call.draw_call_filled_box_init.pos.y, call.draw_call_filled_box_init.size.x, call.draw_call_filled_box_init.size.y, call.draw_call_filled_box_init.clr );
			break;
		case bordered_box:
			draw_list->rect_init( call.draw_call_borded_box_init.pos.x, call.draw_call_borded_box_init.pos.y, call.draw_call_borded_box_init.size.x, call.draw_call_borded_box_init.size.y, call.draw_call_borded_box_init.clr );
			break;
		case text_m:
			draw_list->text( call.draw_call_text_init.pos.x, call.draw_call_text_init.pos.y, call.draw_call_text_init.font, call.draw_call_text_init.text, call.draw_call_text_init.clr );
			break;
		case line_m:
			draw_list->line_init( call.draw_call_line_init.pos.x, call.draw_call_line_init.pos.y, call.draw_call_line_init.size.x, call.draw_call_line_init.size.y, call.draw_call_line_init.clr );
			break;
		case gradient_box:
			draw_list->gradient_vertical_init( call.draw_call_gradient_init.pos.x, call.draw_call_gradient_init.pos.y, call.draw_call_gradient_init.size.x, call.draw_call_gradient_init.size.y, call.draw_call_gradient_init.clr, call.draw_call_gradient_init.clr2 );
			break;
		default:
			break;
		}
	}
}
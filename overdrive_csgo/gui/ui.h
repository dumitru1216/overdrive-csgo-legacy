#pragma once

#include "../hack/sdk/src_kit.h"
#include "../utils/singleton.h"
#include "../render/render.h"
#include "../hack/sdk/interfaces/create_if.h"

#define max_elements 256

namespace ui
{
	class c_gui : public singleton <c_gui>
	{
	public:

		void init( );

		void save_config( );
		void load_config( );

	private:
		void handle_graphics( );
	};

	namespace ui_helpers
	{

		class controller : public singleton <controller>
		{
		public:

			bool menu_opened = true;
			int index;

			vec2_t mouse_pos;
			float mouse_wheel_activity;

			struct
			{
				bool can_open_any_combo = true;

				bool is_any_combo_active = false;
				bool is_any_multi_combo_active = false;
				bool is_any_list_active = false;

				bool is_combo_active[ max_elements ];
				bool is_multi_combo_active[ max_elements ];
				bool is_list_active[ max_elements ];

				bool is_color_active = false;

				bool can_open_color = true;
				bool can_open_list = true;

			} elements;

			struct
			{
				std::vector<std::string> config_slots = { "slot 1", "slot 2", "slot 3", "slot 4" };

				bool m_nCallSave = false;
				bool m_nCallLoad = false;

			} config_manager;

			struct
			{
				bool can_scroll = true;
				int total;
			} group;

			struct
			{
				short int open[ 1 ];
			} last;

		};

		void init_input( );
		bool key_was_pressed( const int key );
		bool mouse_pointer( vec2_t position, vec2_t size );

	};

	namespace external_rendering
	{

		struct rectangle_draw_t
		{
			rectangle_draw_t( float x, float y, float w, float h, Color color )
			{
				this->x = x;
				this->y = y;
				this->w = w;
				this->h = h;
				this->color = color;
			}

			float x;
			float y;
			float w;
			float h;
			Color color;
		};

		struct text_draw_t
		{
			text_draw_t( float x, float y, LPD3DXFONT font, std::string text, Color color )
			{
				this->x = x;
				this->y = y;
				this->font = font;
				this->text = text;
				this->color = color;
			}

			float x;
			float y;
			LPD3DXFONT font;
			std::string text;
			Color color;
		};

		struct outline_draw_t
		{
			outline_draw_t( float x, float y, float w, float h, Color color )
			{
				this->x = x;
				this->y = y;
				this->w = w;
				this->h = h;
				this->color = color;
			}

			float x;
			float y;
			float w;
			float h;
			Color color;
		};

		struct line_draw_t
		{
			line_draw_t( float x, float y, float w, float h, Color color )
			{
				this->x = x;
				this->y = y;
				this->w = w;
				this->h = h;
				this->color = color;
			}

			float x;
			float y;
			float w;
			float h;
			Color color;
		};

		struct gradient_draw_t
		{
			gradient_draw_t( float x, float y, float w, float h, Color color, Color color2 )
			{
				this->x = x;
				this->y = y;
				this->w = w;
				this->h = h;
				this->color = color;
				this->color2 = color2;
			}

			float x;
			float y;
			float w;
			float h;
			Color color;
			Color color2;
		};

		extern std::vector<line_draw_t> line_to_draw;
		extern std::vector<gradient_draw_t> gradient_to_draw;
		extern std::vector<rectangle_draw_t> rectangles_to_draw;
		extern std::vector<text_draw_t> text_to_draw;
		extern std::vector<outline_draw_t> outlines_to_draw;

	};
};

#define helpers ui::ui_helpers 
#define control ui::ui_helpers::controller::get( ) 
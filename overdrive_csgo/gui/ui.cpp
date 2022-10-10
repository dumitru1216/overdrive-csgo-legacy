#include "ui.h"
#include "config/config.h"
#include "config/tgf_cfg.h"
#include "../hack/globals.h"

#include "controls/window.h"
#include "controls/group.h"
#include "controls/checkbox.h"
#include "controls/slider.h"
#include "controls/combo.h"
#include "controls/multi.h"
#include "controls/color_selector.h"
#include "controls/keybind.h"

vec2_t window_position = vec2_t( 800, 200 );
vec2_t window_size = vec2_t( 500, 350 );

std::vector<std::string> tabs = { "aimbot", "anti-aim", "visuals", "misc", "config" };
std::vector<std::string> element = { "enemy", "local", "fake" };
std::vector<std::string> material = { "default", "flat" };
std::vector<std::string> yaw = { "off", "backward", "static", "random" };
std::vector<std::string> pitch = { "none", "up", "down", "zero" };
std::vector<std::string> base_yaw = { "default", "view", "at targets" };
std::vector<std::string> real_yaw = { "none", "backwards" };
std::vector<std::string> fake = { "off", "on" };
std::vector<std::string> skybox = { "none", "tibet", "baggage", "italy", "aztec", "vertigo", "daylight", "daylight 2", "clouds", "clouds 2", "gray", "clear", "canals", "cobblestone", "assault", "clouds dark", "night", "night 2", "night flat", "dusty", "rainy" };
std::vector<std::string> cfg_item = { "slot 1", "slot 2", "slot 3", "slot 4" };
int main_window_selected_tab;
int select_element;

enum list { aim, antiaim, esp, misc, config };
enum content { non, massive };

//4

namespace ui
{
	static int m_slot = 0;

	void c_gui::load_config( )
	{
		m_setup->load( cfg_item.at( m_slot ) );
		control.config_manager.m_nCallLoad = false;
	}

	void c_gui::save_config( )
	{
		m_setup->save( cfg_item.at( m_slot ) );
		control.config_manager.m_nCallSave = false;
	}

	void c_gui::init( )
	{
		ui_helpers::init_input( );
		
		if ( ui_helpers::key_was_pressed( VK_INSERT ) ) {
			control.menu_opened = !control.menu_opened;
			g_csgo.inputsys( )->enable_input( !ui_helpers::controller::get( ).menu_opened );
		}
			
		if ( !control.menu_opened )
			return;

		if ( control.config_manager.m_nCallLoad )
			load_config( );

		if ( control.config_manager.m_nCallSave )
			save_config( );

		ui_helpers::controller::get( ).index = 0;
		ui_helpers::controller::get( ).last.open[ 0 ] = -1;

		auto main = new window( "deathrow_csgo [ beta ]", &window_position, vec2_t( 540, 410 ), tabs, &main_window_selected_tab ); {
			if ( main_window_selected_tab == 0 ) {
				auto group = new groupbox( "ragebot", vec2_t( 20, 40 ), vec2_t( 245, 170 ), 0, content::massive, list::aim ); {
					main->add_groupbox( group );
					group->add_control( new checkbox( "enable aimbot", &cfg.rage.active ) );
					group->add_control( new checkbox( "automatic fire", &cfg.rage_target.auto_fire ) );
					group->add_control( new checkbox( "automatic scope", &cfg.rage_target.auto_scope ) );
					group->add_control( new checkbox( "adjust backtrack position", &cfg.rage_target.position_ad ) );
					group->add_control( new checkbox( "on-shot priority", &cfg.rage_target.on_shot ) );
					group->add_control( new checkbox( "delay shot", &cfg.rage_target.delay_shot ) );
					if ( cfg.rage_target.delay_shot == true ) {
						group->add_control( new checkbox( "unlag unrestricted delay", &cfg.rage_target.unlag ) );
					}
					group->add_control( new checkbox( "unrestrict spread limit", &cfg.rage_target.spread_limit ) );
					group->scroll_process( );
				} 
				auto group_accuracy = new groupbox( "hitscan", vec2_t( 20, 225 ), vec2_t( 245, 170 ), 1, content::massive, list::aim ); {
					main->add_groupbox( group_accuracy );
					group_accuracy->add_control( new checkbox( "multipoint [ head ]", &cfg.rage_points.head ) );
					group_accuracy->add_control( new checkbox( "multipoint [ stomach ]", &cfg.rage_points.stomach ) );
					group_accuracy->add_control( new checkbox( "multipoint [ chest ]", &cfg.rage_points.chest ) );
					group_accuracy->add_control( new checkbox( "multipoint [ upper chest ]", &cfg.rage_points.upper_chest ) );
					group_accuracy->add_control( new checkbox( "multipoint [ neck ]", &cfg.rage_points.neck ) );
					group_accuracy->add_control( new checkbox( "multipoint [ pelvis ]", &cfg.rage_points.pelvis ) );
					group_accuracy->add_control( new checkbox( "multipoint [ arms ]", &cfg.rage_points.arms ) );
					group_accuracy->add_control( new checkbox( "multipoint [ feet ]", &cfg.rage_points.feet ) );
					group_accuracy->add_control( new checkbox( "multipoint [ legs ]", &cfg.rage_points.legs ) );

					if ( cfg.rage_points.head == true ) {
						group_accuracy->add_control( new slider( "multipoint [ head scale ]", &cfg.rage_points.head_scale, 0, 100 ) );
						if ( cfg.rage_points.stomach == true ) {
							group_accuracy->add_control( new slider( "multipoint [ body scale ]", &cfg.rage_points.body_scale, 0, 100 ) );
						}
					}
					group_accuracy->scroll_process( );
				} 
				auto group_accuracy2 = new groupbox( "accuracy", vec2_t( 275, 40 ), vec2_t( 245, 170 ), 2, content::massive, list::aim ); {
					main->add_groupbox( group_accuracy2 );
					group_accuracy2->add_control( new checkbox( "penalty update", &cfg.rage_accuracy.penalty_update ) );
					group_accuracy2->add_control( new slider( "hitchance", &cfg.rage_accuracy.hitchance, 0, 100 ) );
					group_accuracy2->add_control( new slider( "accuracy boost", &cfg.rage_accuracy.accuracy_boost, 0, 100 ) );
					group_accuracy2->add_control( new slider( "minimum damage", &cfg.rage_accuracy.min_damage, 0, 120 ) );
				} 
				auto group_other = new groupbox( "baim", vec2_t( 275, 225 ), vec2_t( 245, 170 ), 3, content::massive, list::aim ); {
					main->add_groupbox( group_other );
					group_other->add_control( new checkbox( "safe multipoint", &cfg.rage.force_safepoint ) );
					group_other->add_control( new checkbox( "prefer baim when lethal", &cfg.rage.baim_lefthal ) );
					group_other->add_control( new checkbox( "prefer baim when air", &cfg.rage.baim_air ) );
					group_other->add_control( new checkbox( "prefer adaptive baim", &cfg.rage.adaptive_baim ) );
					group_other->add_control( new checkbox( "prefer baim if hp under x", &cfg.rage.baim_hp ) );
					if ( cfg.rage.baim_hp == true ) {
						group_other->add_control( new slider( "hp value", &cfg.rage.hp, 0, 100 ) );
					}
				} 
				
				delete group; delete group_accuracy; delete group_accuracy2; delete group_other;
			}
			else if ( main_window_selected_tab == 1 ) {
				auto group = new groupbox( "main", vec2_t( 20, 40 ), vec2_t( 245, 350 ), 51, content::non, list::antiaim );
				{
					main->add_groupbox( group );

					group->add_control( new checkbox( "enable anti-aim", &cfg.antiaim.active ) );
					group->add_control( new combobox( "pitch", &cfg.antiaim.pitch, pitch, 56 ) );
				}

				auto group1 = new groupbox( "detail", vec2_t( 275, 40 ), vec2_t( 245, 350 ), 52, content::non, list::antiaim );
				{
					main->add_groupbox( group1 );

					group1->add_control( new combobox( "base yaw", &cfg.antiaim.base_yaw, base_yaw, 57 ) );
					group1->add_control( new combobox( "real yaw", &cfg.antiaim.yaw, real_yaw, 58 ) );
					group1->add_control( new slider( "jitter", &cfg.antiaim.jitter, 0, 90 ) );
					group1->add_control( new slider( "fakelag", &cfg.antiaim.fakelag_type, 0, 14 ) );
				}

				delete group; delete group1;
			}
			else if ( main_window_selected_tab == 2 ) {

				auto clr = new color_picker( "#", &cfg.colors.box_r, &cfg.colors.box_g, &cfg.colors.box_b, &cfg.colors.box_a );
				auto clr1 = new color_picker( "#1", &cfg.colors.name_r, &cfg.colors.name_g, &cfg.colors.name_b, &cfg.colors.name_a );
				auto clr2 = new color_picker( "#2", &cfg.colors.hp_r, &cfg.colors.hp_g, &cfg.colors.hp_b, &cfg.colors.hp_a );
				auto clr3 = new color_picker( "#3", &cfg.colors.ammo_r, &cfg.colors.ammo_g, &cfg.colors.ammo_b, &cfg.colors.ammo_a );
				auto clr4 = new color_picker( "#4", &cfg.colors.weapon_r, &cfg.colors.weapon_g, &cfg.colors.weapon_b, &cfg.colors.weapon_a );
				auto clr5 = new color_picker( "#5", &cfg.colors.icon_r, &cfg.colors.icon_g, &cfg.colors.icon_b, &cfg.colors.icon_a );
				auto clr6 = new color_picker( "#6", &cfg.colors.oof_r, &cfg.colors.oof_g, &cfg.colors.oof_b, &cfg.colors.oof_a, true );
				auto clr7 = new color_picker( "#7", &cfg.colors.skeleton_r, &cfg.colors.skeleton_g, &cfg.colors.skeleton_b, &cfg.colors.skeleton_a );

				auto clr8 = new color_picker( "#8", &cfg.chams_enemy.chams_r, &cfg.chams_enemy.chams_g, &cfg.chams_enemy.chams_b, &cfg.chams_enemy.chams_a, true );
				auto clr9 = new color_picker( "#9", &cfg.chams_enemy.glow_r, &cfg.chams_enemy.glow_g, &cfg.chams_enemy.glow_b, &cfg.chams_enemy.glow_a, true );
				auto clr10 = new color_picker( "#10", &cfg.chams_enemy.chams_xqz_r, &cfg.chams_enemy.chams_xqz_g, &cfg.chams_enemy.chams_xqz_b, &cfg.chams_enemy.chams_xqz_a, true );
				auto clr11 = new color_picker( "#11", &cfg.chams_enemy.illuminate_r, &cfg.chams_enemy.illuminate_g, &cfg.chams_enemy.illuminate_b, &cfg.chams_enemy.illuminate_a, true );
				auto clr12 = new color_picker( "#12", &cfg.chams_local.chams_r, &cfg.chams_local.chams_g, &cfg.chams_local.chams_b, &cfg.chams_local.chams_a, true );
				auto clr13 = new color_picker( "#13", &cfg.chams_local.glow_r, &cfg.chams_local.glow_g, &cfg.chams_local.glow_b, &cfg.chams_local.glow_a, true );
				auto clr14 = new color_picker( "#14", &cfg.chams_local.illuminate_r, &cfg.chams_local.illuminate_g, &cfg.chams_local.illuminate_b, &cfg.chams_local.illuminate_a, true );
				auto clr15 = new color_picker( "#15", &cfg.chams_desync.chams_r, &cfg.chams_desync.chams_g, &cfg.chams_desync.chams_b, &cfg.chams_desync.chams_a, true );
				auto clr16 = new color_picker( "#16", &cfg.chams_desync.illuminate_r, &cfg.chams_desync.illuminate_g, &cfg.chams_desync.illuminate_b, &cfg.chams_desync.illuminate_a, true );

				auto clr20 = new color_picker( "#20", &cfg.world.world_r, &cfg.world.world_g, &cfg.world.world_b, &cfg.world.world_a );
				auto clr21 = new color_picker( "#21", &cfg.world.prop_r, &cfg.world.prop_g, &cfg.world.prop_b, &cfg.world.prop_a, true );

				auto group_esp = new groupbox( "player esp", vec2_t( 20, 40 ), vec2_t( 245, 170 ), 4, content::massive, list::esp ); {
					main->add_groupbox( group_esp );
					group_esp->add_control( new checkbox( "enabled", &cfg.esp_enabled ) );
					group_esp->add_control( new checkbox( "dormant", &cfg.esp_dormant ) );
					group_esp->add_control( new checkbox( "box", &cfg.esp_box ) );
					group_esp->add_control( clr );
					group_esp->add_control( new checkbox( "name", &cfg.esp_name ) );
					group_esp->add_control( clr1 );
					group_esp->add_control( new checkbox( "health", &cfg.esp_hp ) );
					group_esp->add_control( clr2 );
					group_esp->add_control( new checkbox( "ammo", &cfg.esp_ammo ) );
					group_esp->add_control( clr3 );
					group_esp->add_control( new checkbox( "weapon", &cfg.esp_weapon ) );
					group_esp->add_control( clr4 );
					group_esp->add_control( new checkbox( "weapon icon", &cfg.esp_weapon_icon ) );
					group_esp->add_control( clr5 );
					group_esp->add_control( new checkbox( "skeleton", &cfg.esp_skeleton ) );
					group_esp->add_control( clr7 );
					group_esp->scroll_process( );
				}
				auto group_otheresp = new groupbox( "other esp", vec2_t( 20, 225 ), vec2_t( 245, 170 ), 5, content::massive, list::esp ); {
					main->add_groupbox( group_otheresp );
					auto mult1 = new multi( "removals", 5 );
					mult1->add_item( "scope", &cfg.removals.remove_scope);
					mult1->add_item( "flash", &cfg.removals.remove_flash );
					mult1->add_item( "smoke", &cfg.removals.remove_smoke );
					mult1->add_item( "visual recoil", &cfg.removals.remove_recoil );
					mult1->add_item( "post processing", &cfg.removals.remove_post_process );
					mult1->add_item( "fog", &cfg.removals.remove_fog );
					mult1->add_item( "shadows", &cfg.removals.remove_shadow );
					mult1->add_item( "first zoom", &cfg.removals.remove_zoom );
					group_otheresp->add_control( mult1 );
					group_otheresp->add_control( new checkbox( "thirdperson ", &cfg.misc.tp_bool ) );
					group_otheresp->add_control( new keybind( &cfg.misc.tp_key, &cfg.misc.tp_style, 1 ) );
					if ( cfg.misc.tp_bool == true ) {
						group_otheresp->add_control( new checkbox( "disable on grenade ", &cfg.misc.tp_on_grenade ) );
						group_otheresp->add_control( new slider( "distance", &cfg.misc.tp_dist, 0.0, 150.0 ) );
					}
				} 
				auto group_chams = new groupbox( "chams", vec2_t( 275, 40 ), vec2_t( 245, 170 ), 6, select_element == 0 ? content::massive : content::non, list::esp ); {
					main->add_groupbox( group_chams );
					if ( select_element == 2 )
						group_chams->control_element[ group_chams->current_element ] += 42;					
					group_chams->add_control( new combobox( "entity", &select_element, element, 0 ) );
					if ( select_element == 0 )
					{
						group_chams->add_control( new checkbox( "enable glow", &cfg.chams_enemy.glow ) );
						group_chams->add_control( clr9 );
						group_chams->add_control( new checkbox( "enable chams", &cfg.chams_enemy.chams ) );
						group_chams->add_control( clr8 );
						group_chams->add_control( new checkbox( "enable chams (hidden)", &cfg.chams_enemy.chams_xqz ) );
						group_chams->add_control( clr10 );
						group_chams->add_control( new checkbox( "flat", &cfg.chams_enemy.flat ) );
						group_chams->add_control( new checkbox( "illuminate", &cfg.chams_enemy.illuminate ) );
						group_chams->add_control( clr11 );
						group_chams->add_control( new checkbox( "fix occlusion", &cfg.chams_enemy.full_chams ) );
						group_chams->add_control( new checkbox( "skip occlusion", &cfg.chams_enemy.skip_occlusion ) );
					}
					else if ( select_element == 1 )
					{
						group_chams->add_control( new checkbox( "enable glow", &cfg.chams_local.glow ) );
						group_chams->add_control( clr13 );
						group_chams->add_control( new checkbox( "enable chams", &cfg.chams_local.chams ) );
						group_chams->add_control( clr12 );
						group_chams->add_control( new checkbox( "flat", &cfg.chams_local.flat ) );
						group_chams->add_control( new checkbox( "illuminate", &cfg.chams_local.illuminate ) );
						group_chams->add_control( clr14 );
					}
					else if ( select_element == 2 )
					{
						group_chams->add_control( new checkbox( "enable chams", &cfg.chams_desync.chams ) );
						group_chams->add_control( clr15 );
						group_chams->add_control( new checkbox( "flat", &cfg.chams_desync.flat ) );
						group_chams->add_control( new checkbox( "illuminate", &cfg.chams_desync.illuminate ) );
						group_chams->add_control( clr16 );
					}
					group_chams->scroll_process( );		
				}
				auto group_world = new groupbox( "world", vec2_t( 275, 225 ), vec2_t( 245, 170 ), 7, content::massive, list::esp ); {
					main->add_groupbox( group_world );
					group_world->add_control( new checkbox( "enabled", &cfg.world.enabled ) );
					group_world->add_control( new checkbox( "modulate walls", &cfg.world.modulate_world ) );
					group_world->add_control( clr20 );
					group_world->add_control( new checkbox( "modulate props", &cfg.world.prop ) );
					group_world->add_control( clr21 );
					group_world->add_control( new combobox( "skybox", &cfg.world.skybox, skybox, 3 ) );
				} 

				group_esp->init_last_element( clr );
				group_esp->init_last_element( clr1 );
				group_esp->init_last_element( clr2 );
				group_esp->init_last_element( clr3 );
				group_esp->init_last_element( clr4 );
				group_esp->init_last_element( clr5 );
				group_esp->init_last_element( clr6 );
				group_esp->init_last_element( clr7 );
				group_esp->init_last_element( clr8 );
				group_esp->init_last_element( clr9 );
				group_esp->init_last_element( clr10 );
				group_esp->init_last_element( clr11 );
				group_esp->init_last_element( clr12 );
				group_esp->init_last_element( clr13 );
				group_esp->init_last_element( clr14 );
				group_esp->init_last_element( clr15 );
				group_esp->init_last_element( clr16 );
				group_esp->init_last_element( clr20 );
				group_esp->init_last_element( clr21 );

				delete group_esp; delete group_otheresp; delete group_chams; delete group_world;
			}
			else if ( main_window_selected_tab == 3 ) {

			}
			else if ( main_window_selected_tab == 4 ) {
				auto group_config = new groupbox( "configs", vec2_t( 20, 40 ), vec2_t( 245, 350 ), 9, content::massive, list::config ); {
					main->add_groupbox( group_config );
					static int config_slot = 0;
					group_config->add_control( new combobox( "config slot", &config_slot, cfg_item, 0 ) );
					group_config->add_control( new checkbox( "save", &control.config_manager.m_nCallSave ) );
					group_config->add_control( new checkbox( "load", &control.config_manager.m_nCallLoad ) );
				} 
				
				delete group_config;
			}
		} delete main;

		handle_graphics( );
	}

	void c_gui::handle_graphics( )
	{
		for ( int i = 0; i < external_rendering::rectangles_to_draw.size( ); i++ )
			draw_list->rect_fill_init( external_rendering::rectangles_to_draw[ i ].x, external_rendering::rectangles_to_draw[ i ].y, external_rendering::rectangles_to_draw[ i ].w, external_rendering::rectangles_to_draw[ i ].h, external_rendering::rectangles_to_draw[ i ].color );

		for ( int i = 0; i < external_rendering::gradient_to_draw.size( ); i++ )
			draw_list->gradient_vertical_init( external_rendering::gradient_to_draw[ i ].x, external_rendering::gradient_to_draw[ i ].y, external_rendering::gradient_to_draw[ i ].w, external_rendering::gradient_to_draw[ i ].h, external_rendering::gradient_to_draw[ i ].color, external_rendering::gradient_to_draw[ i ].color2 );

		for ( int i = 0; i < external_rendering::outlines_to_draw.size( ); i++ )
			draw_list->rect_init( external_rendering::outlines_to_draw[ i ].x, external_rendering::outlines_to_draw[ i ].y, external_rendering::outlines_to_draw[ i ].w, external_rendering::outlines_to_draw[ i ].h, external_rendering::outlines_to_draw[ i ].color );

		for ( int i = 0; i < external_rendering::line_to_draw.size( ); i++ )
			draw_list->line_init( external_rendering::line_to_draw[ i ].x, external_rendering::line_to_draw[ i ].y, external_rendering::line_to_draw[ i ].w, external_rendering::line_to_draw[ i ].h, external_rendering::line_to_draw[ i ].color );

		for ( int i = 0; i < external_rendering::text_to_draw.size( ); i++ )
			draw_list->text( external_rendering::text_to_draw[ i ].x, external_rendering::text_to_draw[ i ].y, external_rendering::text_to_draw[ i ].font, external_rendering::text_to_draw[ i ].text, external_rendering::text_to_draw[ i ].color );

		external_rendering::rectangles_to_draw.clear( );
		external_rendering::text_to_draw.clear( );
		external_rendering::outlines_to_draw.clear( );
		external_rendering::gradient_to_draw.clear( );
		external_rendering::line_to_draw.clear( );
	}

	namespace external_rendering
	{
		std::vector<rectangle_draw_t> rectangles_to_draw;
		std::vector<text_draw_t> text_to_draw;
		std::vector<outline_draw_t> outlines_to_draw;
		std::vector<gradient_draw_t> gradient_to_draw;
		std::vector<line_draw_t> line_to_draw;
	};
};
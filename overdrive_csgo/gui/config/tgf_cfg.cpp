#include "tgf_cfg.h"
#include "config.h"

#include <Windows.h>
#include <Psapi.h>
#include <lmcons.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <shlobj.h>
#include <time.h>
#include <random>
#include <sstream>
#include <fstream>
#include <shlwapi.h>
#include <iomanip>
#include <ctime>
#include <d3d9.h>
#include <d3dx9.h>

CConfig* m_setup = new CConfig( );

CConfig::CConfig( )
{
	setup( );
}

CConfig::~CConfig( )
{
	delete m_setup;
}

void CConfig::setup( )
{
	setup_value( cfg.rage.active, false, "aimbot", "a" );
	setup_value( cfg.rage.adaptive_baim, false, "aimbot", "b" );
	setup_value( cfg.rage.baim_air, false, "aimbot", "ddwaa" );
	setup_value( cfg.rage.baim_hp, false, "aimbot", "sad" );
	setup_value( cfg.rage.baim_lefthal, false, "aimbot", "sadwa" );
	setup_value( cfg.rage.force_safepoint, false, "aimbot", "wa123" );
	setup_value( cfg.rage_accuracy.penalty_update, false, "aimbot", "wqe123" );
	setup_value( cfg.rage_points.arms, false, "aimbot", "wqdq123" );
	setup_value( cfg.rage_points.head, false, "aimbot", "sqwfqw132" );
	setup_value( cfg.rage_points.legs, false, "aimbot", "asgqw13" );
	setup_value( cfg.rage_points.stomach, false, "aimbot", "gfqw13325" );
	setup_value( cfg.rage_points.pelvis, false, "aimbot", "dhgtr123213" );
	setup_value( cfg.rage_points.neck, false, "aimbot", "dhgt2r123213" );
	setup_value( cfg.rage_points.upper_chest, false, "aimbot", "nsawe123" );
	setup_value( cfg.rage_points.chest, false, "aimbot", "nte4232" );
	setup_value( cfg.rage_points.feet, false, "aimbot", "nb11223" );
	setup_value( cfg.rage_target.auto_fire, false, "aimbot", "bq12443" );
	setup_value( cfg.rage_target.auto_scope, false, "aimbot", "312343awq" );
	setup_value( cfg.rage_target.delay_shot, false, "aimbot", "1231wawrqw" );
	setup_value( cfg.rage_target.on_shot, false, "aimbot", "123325213" );
	setup_value( cfg.rage_target.position_ad, false, "aimbot", "123edwgw" );
	setup_value( cfg.misc.tp_bool, false, "misc", "w21342w6q" );
	setup_value( cfg.rage_target.spread_limit, false, "aimbot", "w2342wq" );
	setup_value( cfg.rage_target.unlag, false, "aimbot", "65daww1" );
	setup_value( cfg.antiaim.active, false, "aa", "65d3a232ww1" );
	setup_value( cfg.misc.tp_key, 0, "misc", "j123it" );
	setup_value( cfg.misc.tp_dist, 0, "misc", "j2213i2t" );
	setup_value( cfg.misc.tp_style, 0, "misc", "ji3252t" );
	setup_value( cfg.antiaim.jitter, 0, "aa", "jit" );
	setup_value( cfg.antiaim.base_yaw, 0, "aa", "ji2t" );
	setup_value( cfg.antiaim.pitch, 0, "aa", "pitch" );
	setup_value( cfg.antiaim.yaw, 0, "aa", "ji3t" );
	setup_value( cfg.rage.hp, 0, "aimbot", "qwq21453223" );
	setup_value( cfg.esp_enabled, false, "esp", "enabled" );
	setup_value( cfg.esp_box, false, "esp", "box" );
	setup_value( cfg.esp_dormant, false, "esp", "dormant" );
	setup_value( cfg.esp_name, false, "esp", "name" );
	setup_value( cfg.esp_hp, false, "esp", "hp" );
	setup_value( cfg.esp_ammo, false, "esp", "ammo" );
	setup_value( cfg.esp_skeleton, false, "esp", "skeleton" );
	setup_value( cfg.esp_weapon, false, "esp", "weapon" );
	setup_value( cfg.esp_weapon_icon, false, "esp", "weapon_icon" );
	setup_value(cfg.colors.box_r, 255, "esp", "box_r" );
	setup_value(cfg.colors.box_g, 255, "esp", "box_g" );
	setup_value(cfg.colors.box_b, 255, "esp", "box_b" );
	setup_value(cfg.colors.box_a, 255, "esp", "box_a" );
	setup_value(cfg.colors.name_r, 255, "esp", "name_r" );
	setup_value(cfg.colors.name_g, 255, "esp", "name_g" );
	setup_value(cfg.colors.name_b, 255, "esp", "name_b" );
	setup_value(cfg.colors.name_a, 255, "esp", "name_a" );
	setup_value(cfg.colors.hp_r, 255, "esp", "hp_r" );
	setup_value(cfg.colors.hp_g, 255, "esp", "hp_g" );
	setup_value(cfg.colors.hp_b, 255, "esp", "hp_b" );
	setup_value(cfg.colors.hp_a, 255, "esp", "hp_a" );
	setup_value(cfg.colors.ammo_r, 255, "esp", "ammo_r" );
	setup_value(cfg.colors.ammo_g, 255, "esp", "ammo_g" );
	setup_value(cfg.colors.ammo_b, 255, "esp", "ammo_b" );
	setup_value(cfg.colors.ammo_a, 255, "esp", "ammo_a" );
	setup_value(cfg.colors.weapon_r, 255, "esp", "weapon_r" );
	setup_value(cfg.colors.weapon_g, 255, "esp", "weapon_g" );
	setup_value(cfg.colors.weapon_b, 255, "esp", "weapon_b" );
	setup_value(cfg.colors.weapon_a, 255, "esp", "weapon_a" );
	setup_value(cfg.colors.icon_r, 255, "esp", "icon_r" );
	setup_value(cfg.colors.icon_g, 255, "esp", "icon_g" );
	setup_value(cfg.colors.icon_b, 255, "esp", "icon_b" );
	setup_value(cfg.colors.icon_a, 255, "esp", "icon_a" );
	setup_value(cfg.colors.skeleton_r, 255, "esp", "skeleton_r" );
	setup_value(cfg.colors.skeleton_g, 255, "esp", "skeleton_g" );
	setup_value(cfg.colors.skeleton_b, 255, "esp", "skeleton_b" );
	setup_value(cfg.colors.skeleton_a, 255, "esp", "skeleton_a" );
	setup_value( cfg.world.skybox, 0, "esp", "sky_box" );
	setup_value( cfg.world.prop, false, "esp", "prop" );
	setup_value( cfg.world.modulate_world, false, "esp", "mdl_world" );
	setup_value( cfg.world.modulate, false, "esp", "mdl" );
	setup_value( cfg.world.prop_r, 255, "esp", "prop_r" );
	setup_value( cfg.world.prop_g, 255, "esp", "prop_g" );
	setup_value( cfg.world.prop_b, 255, "esp", "prop_b" );
	setup_value( cfg.world.prop_a, 255, "esp", "prop_a" );
	setup_value( cfg.world.world_r, 255, "esp", "world_r" );
	setup_value( cfg.world.world_g, 255, "esp", "world_g" );
	setup_value( cfg.world.world_b, 255, "esp", "world_b" );
	setup_value( cfg.world.world_a, 255, "esp", "world_a" );
	setup_value( cfg.rage_accuracy.accuracy_boost, 0, "aimbot", "wsa434waaw6123" );
	setup_value( cfg.rage_accuracy.hitchance, 0, "aimbot", "21321ewqq56" );
	setup_value( cfg.rage_accuracy.min_damage, 0, "aimbot", "sf12w1" );
	setup_value( cfg.rage_points.head_scale, 0, "aimbot", "qg44122s" );
	setup_value( cfg.rage_points.body_scale, 0, "aimbot", "rhyyy44" );
	setup_value( cfg.chams_enemy.chams, false, "esp", "chams" );
	setup_value( cfg.chams_enemy.chams_xqz, false, "esp", "chams_xqz" );
	setup_value( cfg.chams_enemy.flat, false, "esp", "flat" );
	setup_value( cfg.chams_enemy.illuminate, false, "esp", "illuminate" );
	setup_value( cfg.chams_enemy.skip_occlusion, false, "esp", "skip_occlusion" );
	setup_value( cfg.chams_local.glow, false, "esp", "glow_local" );
	setup_value( cfg.chams_local.chams, false, "esp", "chams_local" );
	setup_value( cfg.chams_local.flat, false, "esp", "flat_local" );
	setup_value( cfg.chams_local.illuminate, false, "esp", "illuminate_local" );
	setup_value( cfg.chams_desync.chams, false, "esp", "chams_desync" );
	setup_value( cfg.chams_desync.flat, false, "esp", "flat_desync" );
	setup_value( cfg.chams_desync.illuminate, false, "esp", "illuminate_desync" );
	setup_value( cfg.chams_enemy.chams_r, 255, "esp", "chams_r" );
	setup_value( cfg.chams_enemy.chams_g, 255, "esp", "chams_g" );
	setup_value( cfg.chams_enemy.chams_b, 255, "esp", "chams_b" );
	setup_value( cfg.chams_enemy.chams_a, 255, "esp", "chams_a" );
	setup_value( cfg.chams_enemy.illuminate_r, 255, "esp", "illuminate_r" );
	setup_value( cfg.chams_enemy.illuminate_g, 255, "esp", "illuminate_g" );
	setup_value( cfg.chams_enemy.illuminate_b, 255, "esp", "illuminate_b" );
	setup_value( cfg.chams_enemy.illuminate_a, 255, "esp", "illuminate_a" );
	setup_value( cfg.chams_enemy.glow_r, 255, "esp", "glow_r" );
	setup_value( cfg.chams_enemy.glow_g, 255, "esp", "glow_g" );
	setup_value( cfg.chams_enemy.glow_b, 255, "esp", "glow_b" );
	setup_value( cfg.chams_enemy.glow_a, 255, "esp", "glow_a" );
	setup_value( cfg.chams_enemy.chams_xqz_r, 255, "esp", "chams_xqz_r" );
	setup_value( cfg.chams_enemy.chams_xqz_g, 255, "esp", "chams_xqz_g" );
	setup_value( cfg.chams_enemy.chams_xqz_b, 255, "esp", "chams_xqz_b" );
	setup_value( cfg.chams_enemy.chams_xqz_a, 255, "esp", "chams_xqz_a" );
	setup_value( cfg.chams_local.chams_r, 255, "esp", "chams_r_local" );
	setup_value( cfg.chams_local.chams_g, 255, "esp", "chams_g_local" );
	setup_value( cfg.chams_local.chams_b, 255, "esp", "chams_b_local" );
	setup_value( cfg.chams_local.chams_a, 255, "esp", "chams_a_local" );
	setup_value( cfg.chams_local.illuminate_r, 255, "esp", "illuminate_r_local" );
	setup_value( cfg.chams_local.illuminate_g, 255, "esp", "illuminate_g_local" );
	setup_value( cfg.chams_local.illuminate_b, 255, "esp", "illuminate_b_local" );
	setup_value( cfg.chams_local.illuminate_a, 255, "esp", "illuminate_a_local" );
	setup_value( cfg.chams_local.glow_r, 255, "esp", "glow_r_local" );
	setup_value( cfg.chams_local.glow_g, 255, "esp", "glow_g_local" );
	setup_value( cfg.chams_local.glow_b, 255, "esp", "glow_b_local" );
	setup_value( cfg.chams_local.glow_a, 255, "esp", "glow_a_local" );
	setup_value( cfg.chams_desync.chams_r, 255, "esp", "chams_r_desync" );
	setup_value( cfg.chams_desync.chams_g, 255, "esp", "chams_g_desync" );
	setup_value( cfg.chams_desync.chams_b, 255, "esp", "chams_b_desync" );
	setup_value( cfg.chams_desync.chams_a, 255, "esp", "chams_a_desync" );
	setup_value( cfg.chams_desync.illuminate_r, 255, "esp", "illuminate_r_desync" );
	setup_value( cfg.chams_desync.illuminate_g, 255, "esp", "illuminate_g_desync" );
	setup_value( cfg.chams_desync.illuminate_b, 255, "esp", "illuminate_b_desync" );
	setup_value( cfg.chams_desync.illuminate_a, 255, "esp", "illuminate_a_desync" );
	setup_value( cfg.removals.remove_smoke, false, "rem", "11" );
	setup_value( cfg.removals.remove_flash, false, "rem", "212" );
	setup_value( cfg.removals.remove_scope, false, "rem", "2345" );
	setup_value( cfg.removals.remove_zoom, false, "rem", "21754" );
	setup_value( cfg.removals.remove_post_process, false, "rem", "1685" );
	setup_value( cfg.removals.remove_shadow, false, "rem", "87122" );
	setup_value( cfg.removals.remove_fog, false, "rem", "4376" );
	setup_value( cfg.removals.remove_recoil, false, "rem", "4376231" );
}

void CConfig::setup_value( int& value, int def, std::string category, std::string name )
{
	value = def;
	ints.push_back( new ConfigValue< int >( category, name, &value ) );
}

void CConfig::setup_value( unsigned long& value, unsigned long def, std::string category, std::string name )
{
	value = def;
	longs.push_back( new ConfigValue< unsigned long >( category, name, &value ) );
}

void CConfig::setup_value( float& value, float def, std::string category, std::string name )
{
	value = def;
	floats.push_back( new ConfigValue< float >( category, name, &value ) );
}

void CConfig::setup_value( bool& value, bool def, std::string category, std::string name )
{
	value = def;
	bools.push_back( new ConfigValue< bool >( category, name, &value ) );
}

void CConfig::save( std::string ConfigName )
{
	static TCHAR path[ MAX_PATH ];
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		folder = std::string( path ) + "\\styles\\";
		file = std::string( path ) + "\\styles\\" + ConfigName + ".ini";
	}

	CreateDirectory( folder.c_str( ), NULL );

	for ( auto value : ints )
		WritePrivateProfileString( value->category.c_str( ), value->name.c_str( ), std::to_string( *value->value ).c_str( ), file.c_str( ) );

	for ( auto value : floats )
		WritePrivateProfileString( value->category.c_str( ), value->name.c_str( ), std::to_string( *value->value ).c_str( ), file.c_str( ) );

	for ( auto value : longs )
		WritePrivateProfileString( value->category.c_str( ), value->name.c_str( ), std::to_string( *value->value ).c_str( ), file.c_str( ) );

	for ( auto value : bools )
		WritePrivateProfileString( value->category.c_str( ), value->name.c_str( ), *value->value ? "true" : "false", file.c_str( ) );
}

void CConfig::load( std::string ConfigName )
{
	static TCHAR path[ MAX_PATH ];
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		folder = std::string( path ) + "\\styles\\";
		file = std::string( path ) + "\\styles\\" + ConfigName + ".ini";
	}

	CreateDirectory( folder.c_str( ), NULL );

	char value_l[ 32 ] = { '\0' };

	for ( auto value : ints )
	{
		GetPrivateProfileString( value->category.c_str( ), value->name.c_str( ), "", value_l, 32, file.c_str( ) );
		*value->value = atoi( value_l );
	}

	for ( auto value : floats )
	{
		GetPrivateProfileString( value->category.c_str( ), value->name.c_str( ), "", value_l, 32, file.c_str( ) );
		*value->value = atof( value_l );
	}

	for ( auto value : longs )
	{
		GetPrivateProfileString( value->category.c_str( ), value->name.c_str( ), "", value_l, 32, file.c_str( ) );
		*value->value = atof( value_l );
	}

	for ( auto value : bools )
	{
		GetPrivateProfileString( value->category.c_str( ), value->name.c_str( ), "", value_l, 32, file.c_str( ) );
		*value->value = !strcmp( value_l, "true" );
	}
}

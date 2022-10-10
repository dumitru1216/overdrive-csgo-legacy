#pragma once

class config
{
public:

	/* random structs */
	bool esp_enabled;
	bool esp_box;
	bool esp_name;
	bool esp_hp;
	bool esp_ammo;
	bool esp_weapon;
	bool esp_weapon_icon;
	bool esp_skeleton;
	bool esp_oof;
	bool esp_dormant;
	bool grenade_preview;
	bool bullet_impacts;

	float oof_size;
	float oof_dist;
	float dist;

	struct
	{
		bool active = false;
		bool force_safepoint = false;

		bool adaptive_baim = false;
		bool baim_lefthal = false;
		bool baim_air = false;
		bool baim_hp = false;
		float hp = 0;
	} rage;

	struct
	{
		bool head = false;
		bool neck = false;
		bool upper_chest = false;
		bool chest = false;
		bool stomach = false;
		bool pelvis = false;
		bool arms = false;
		bool legs = false;
		bool feet = false;

		float head_scale = 0.f;
		float body_scale = 0.f;
	} rage_points;

	struct
	{
		bool penalty_update = false;
		float hitchance = 0;
		float accuracy_boost = 0;
		float min_damage = 0;

	} rage_accuracy;

	struct
	{
		bool auto_fire = false;
		bool auto_scope = false;
		bool position_ad = false;
		bool on_shot = false;
		bool delay_shot = false;
		bool unlag = false;
		bool spread_limit = false;

	} rage_target;

	struct
	{
		bool active = false;
		bool name = false;
		bool health = false;
		bool weapon = false;
		bool ammo = false;
		bool box = false;
		bool dormant = false;
		bool hp_number = false;
		bool team_check = false;
	} esp;

	struct
	{
		float box_r, box_g, box_b, box_a;
		float name_r, name_g, name_b, name_a;
		float hp_r, hp_g, hp_b, hp_a;
		float ammo_r, ammo_g, ammo_b, ammo_a;
		float weapon_r, weapon_g, weapon_b, weapon_a;
		float icon_r, icon_g, icon_b, icon_a;
		float skeleton_r, skeleton_g, skeleton_b, skeleton_a;
		float oof_r, oof_g, oof_b, oof_a;

	} colors;

	struct
	{
		bool enabled;
		bool modulate;

		bool prop;
		bool modulate_world;

		int skybox;

		float drop_box_r, drop_box_g, drop_box_b, drop_box_a;
		float drop_name_r, drop_name_g, drop_name_b, drop_name_a;
		float drop_ammo_r, drop_ammo_g, drop_ammo_b, drop_ammo_a;

		float world_r, world_g, world_b, world_a;
		float prop_r, prop_g, prop_b, prop_a;

	} world;

	struct
	{
		bool full_chams;
		bool glow;
		bool chams;
		bool chams_xqz;
		bool flat;
		bool illuminate;
		bool skip_occlusion;

		float glow_r, glow_g, glow_b, glow_a;
		float chams_r, chams_g, chams_b, chams_a;
		float chams_xqz_r, chams_xqz_g, chams_xqz_b, chams_xqz_a;
		float illuminate_r, illuminate_g, illuminate_b, illuminate_a;

	} chams_enemy;

	struct
	{
		bool glow = false;
		bool chams = false;
		bool flat = false;
		bool illuminate = false;

		float glow_r, glow_g, glow_b, glow_a;
		float chams_r, chams_g, chams_b, chams_a;
		float illuminate_r, illuminate_g, illuminate_b, illuminate_a;

	} chams_local;

	struct
	{
		bool chams = false;
		bool flat = false;
		bool illuminate = false;

		float chams_r, chams_g, chams_b, chams_a;
		float illuminate_r, illuminate_g, illuminate_b, illuminate_a;

	} chams_desync;

	struct
	{
		bool anti_untrusted = false;
		bool tp_bool = false;
		bool tp_on_grenade = false;
		int tp_style;
		int tp_key;
		bool disable_grenade = false;
		float tp_dist = 0;

	} misc;

	struct
	{
		bool active = false;

		int yaw = 0;
		int pitch = 0;
		int desync_type = 0;
		int bind = -1;
		float fakelag_type;
		float max_delta = 0;
		float jitter = 0;

		int base_yaw;

	} antiaim;

	struct
	{
		bool remove_shadow = false;
		bool remove_flash = false;
		bool remove_smoke = false;
		bool remove_post_process = false;
		bool remove_fog = false;
		bool remove_zoom = false;
		bool remove_scope = false;
		bool remove_recoil = false;

	} removals;

};

inline config cfg;


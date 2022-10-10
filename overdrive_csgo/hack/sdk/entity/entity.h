#pragma once

#include "../interfaces/create_if.h"
#include "netvar.h"

#define Assert( _exp ) ((void)0)

#define OFFSET(funcname, type, offset) type& funcname() \
{ \
	static uint16_t _offset = offset; \
	Assert(_offset); \
	return *reinterpret_cast<type*>(uintptr_t(this) + _offset); \
}

#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = netvar_sys::get().GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
    }

#define tick_interval			(g_csgo.global_vars()->m_interval_per_tick)
#define ticks_to_time(t) (g_csgo.global_vars()->m_interval_per_tick * (t) )
#define time_to_ticks( dt )		( (int)( 0.5 + (float)(dt) / g_csgo.global_vars()->m_interval_per_tick ) )

struct datamap_t;
class AnimationLayer;

class CBasePlayerAnimState;
class CCSGOPlayerAnimState;

class m_entity;
class m_weapon;

class m_entity : public iclient_entity
{
public:

	template<class T>
	T GetValue( const int offset )
	{
		return *reinterpret_cast< T* >( reinterpret_cast< std::uintptr_t >( this ) + offset );
	}

	datamap_t * GetDataDescMap( );
	datamap_t * GetPredDescMap( );
	
	NETVAR( bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity" );
	NETVAR( Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin" );
	NETVAR( int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth" );
	NETVAR( int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState" );
	NETVAR( bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet" );
	NETVAR( bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor" );
	NETVAR( int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue" );
	NETVAR( int, m_MoveType, "DT_CSPlayer", "m_MoveType" );
	NETVAR( Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]" );
	NETVAR( bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped" );;
	OFFSET( m_nTakeDamage, int, 0x27C );
	NETVAR( int32_t, m_bDidSmokeEffect, "DT_BaseCSGrenadeProjectile", "m_bDidSmokeEffect" );
	NETVAR( int32_t, m_nSmokeEffectTickBegin, "DT_BaseCSGrenadeProjectile", "m_nSmokeEffectTickBegin" );
	NETVAR( Vector, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle" );
	NETVAR( Vector, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle" );
	NETVAR( bool, m_bUseCustomAutoExposureMin, "DT_EnvTonemapController", "m_bUseCustomAutoExposureMin" );
	NETVAR( bool, m_bUseCustomAutoExposureMax, "DT_EnvTonemapController", "m_bUseCustomAutoExposureMax" );
	NETVAR( float, m_flCustomAutoExposureMin, "DT_EnvTonemapController", "m_flCustomAutoExposureMin" );
	NETVAR( float, m_flCustomAutoExposureMax, "DT_EnvTonemapController", "m_flCustomAutoExposureMax" );
	NETVAR( float, m_flCustomBloomScale, "DT_EnvTonemapController", "m_flCustomBloomScale" );
	NETVAR( float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration" );
	NETVAR( float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha" );
	NETVAR( float, m_flDuckSpeed, "DT_BaseEntity", "m_flDuckSpeed" );
	NETVAR( float, m_flDuckAmount, "DT_BaseEntity", "m_flDuckAmount" );
	NETVAR( int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags" );
	NETVAR( Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]" );
	NETVAR( int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase" );
	NETVAR( float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime" );
	NETVAR( Vector, m_angAbsAngles, "DT_BaseEntity", "m_angAbsAngles" );
	NETVAR( float, m_lby, "DT_CSPlayer", "m_flLowerBodyYawTarget" );
	NETVAR( Vector, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]" );
	NETVAR( int, m_fEffects, "DT_BaseEntity", "m_fEffects" );
	NETVAR( float, m_flNextAttack, "CBaseCombatCharacter", "m_flNextAttack" );
	NETVAR( int32_t, m_nExplodeEffectTickBegin, "DT_BaseCSGrenadeProjectile", "m_nExplodeEffectTickBegin" );
	NETVAR( int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired" );
	NETVAR( int32_t, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet" );
	NETVAR( int, m_iObserverMode, "DT_BasePlayer", "m_iObserverMode" );
	NETVAR( float, m_flCycle, "DT_BaseAnimating", "m_flCycle" );
	NETVAR( Vector, m_aimPunchAngleVel, "DT_BasePlayer", "m_aimPunchAngleVel" );

	CCSGOPlayerAnimState* get_animation_state( );
	float m_flOldSimulationTime( );
	matrix3x4_t get_bone_matrix( int BoneID );
	void m_bClientSideAnimation( bool value );
	
	static void update_animation_state( CCSGOPlayerAnimState* state, Vector angle );
	static void reset_animation_state( CCSGOPlayerAnimState* state );
	void create_animation_state( CCSGOPlayerAnimState* state );

	void update_client_side_animation( );
	
	bool is_valid_player( bool d );
	void invalidate_bone_cache( void );
	int& m_bone_count( );
	int num_overlays( );

	void set_abs_origin( Vector origin );
	CUserCmd*& current_command( );
	m_weapon* m_hActiveWeapon( );
	void shoot_pos( Vector* vec );

	Vector get_eye_pos( );
	float_t m_flSpawnTime( );
	bool is_visible_vector( Vector bone );

	int m_iTeamNum( );
	Vector get_bone_pos( int bone );
	int m_nMoveType( );

	AnimationLayer* get_animlayers( );
	int sequence_activity( int sequence );
	void set_abs_angles( const Vector& angles );

	bool is_alive( );
	bool is_player( );
	player_info_t GetPlayerInfo( );
	void* get_renderable( void );
	
	Vector& get_abs_origin( );
	Vector get_abs_angles( );
	VMatrix& GetCollisionBoundTrans( );
	bool is_weapon( );
	
	const Vector world_space_center( );
	inline const char* get_classname( void );
	Vector hitbox_position( int hitbox_id );

	float_t &m_surfaceFriction( );
	Vector &m_vecBaseVelocity( );
	float_t &m_flMaxspeed( );
	Vector& m_vecAbsVelocity( );
	int& m_fEffects( );
	int& m_iEFlags( );
	float* m_flEncodedController( );
	void attachment_helper( );

	matrix3x4_t*& m_nBoneArrayForWrite( );
	uint32_t& m_nReadebleBones( );
	uint32_t& m_nWritableBones( );
	int32_t& m_nThinkTick( );
	uint32_t& m_nMostRecentBoneCounter( );

	void InvalidatePhysicsRecursive( int32_t flags );
	CUtlVector< matrix3x4_t >& m_CachedBoneData( );
	matrix3x4_t**& m_nBoneCache( );
	CStudioHdr* m_nModelHdr( );
	CStudioHdr* GetModelPtr( );
	float& m_flVelocityModifier( );
	float& m_flLastBoneSetupTime( );
	CBoneAccessor* GetBoneAccessor( );
	uint32_t& m_iOcclusionMask( );
	void set_bone_cache( matrix3x4_t* m );

	Vector& get_mins( );
	Vector& get_maxs( );
	uint32_t& occlusion_flags( );
	uint32_t& occlusion_framecount( );

	std::array<float, 24>& m_flPoseParameter( ) const {
		static int _m_flPoseParameter = netvar_sys::get( ).GetOffset( "DT_BaseAnimating", "m_flPoseParameter" );
		return *( std::array<float, 24>* )( ( uintptr_t )this + _m_flPoseParameter );
	}
};

class m_weapon : public m_entity
{
public:

	NETVAR( float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack" );
	NETVAR( float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack" );
	NETVAR( int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1" );
	NETVAR( int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2" );
	NETVAR( float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex" );
	NETVAR( int32_t, m_iViewModelIndex, "DT_BaseCombatWeapon", "m_iViewModelIndex" );
	NETVAR( int32_t, m_iWorldModelIndex, "DT_BaseCombatWeapon", "m_iWorldModelIndex" );
	NETVAR( int32_t, m_iWorldDroppedModelIndex, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex" );
	NETVAR( bool, m_bPinPulled, "DT_BaseCSGrenade", "m_bPinPulled" );
	NETVAR( float_t, m_fThrowTime, "DT_BaseCSGrenade", "m_fThrowTime" );
	NETVAR( float_t, m_flPostponeFireReadyTime, "DT_BaseCombatWeapon", "m_flPostponeFireReadyTime" );
	NETVAR( float_t, m_fLastShotTime, "DT_WeaponCSBase", "m_fLastShotTime" );
	NETVAR( int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized" );
	NETVAR( int32_t, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex" );
	NETVAR( int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel" );
	NETVAR( int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID" );
	NETVAR( int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow" );
	NETVAR( int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh" );
	NETVAR( int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality" );
	NETVAR( int32_t, m_zoomLevel, "DT_WeaponCSBaseGun", "m_zoomLevel" );
	NETVAR( float_t, m_flThrowStrength, "DT_BaseCSGrenade", "m_flThrowStrength" );

	bool is_empty( );
	bool can_fire( );
	bool is_non_aim( );
	bool is_pistol( );
	bool can_fire_post_pone( );
	bool is_grenade( );
	bool is_reloading( );

	const char* weapon_ic( );
	weapon_info_t* m_data( );
	CSWeaponInfo_t* m_data2( );
	float inaccuracy( );
	float spread( );
	void penalty( );
	bool is_sniper( );
	float last_shot_time( );
};

class AnimationLayer
{
public:
	char  pad_0000[ 20 ];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void* m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[ 4 ]; //0x0034
}; //Size: 0x0038

class CCSGOPlayerAnimState
{
public:
	void* pThis;
	char pad2[ 91 ];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[ 4 ]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[ 4 ];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[ 8 ];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[ 10 ];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[ 4 ]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[ 4 ]; //NaN
	float m_flUnknown3;
	char pad10[ 528 ];
};
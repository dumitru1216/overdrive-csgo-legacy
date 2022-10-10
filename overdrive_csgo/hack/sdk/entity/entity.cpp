#include "entity.h"

#include "../other/e_nums.h"
#include "../../../utils/utl.h"
#include "../../../utils/math.h"
#include "../../globals.h"

float m_entity::m_flOldSimulationTime( )
{
	static int m_flOldSimulationTime = netvar_sys::get( ).GetOffset( "DT_BaseEntity", "m_flSimulationTime" ) + 4;
	return GetValue<float>( m_flOldSimulationTime );
}

CCSGOPlayerAnimState* m_entity::get_animation_state( )
{
	return *( CCSGOPlayerAnimState** )( ( DWORD )this + 0x3874 );
}

uint32_t& m_entity::occlusion_flags( )
{
	return *( uint32_t* )( ( DWORD )this + 0xA28 );
}

uint32_t& m_entity::occlusion_framecount( )
{
	return *( uint32_t* )( ( DWORD )this + 0xA30 );
}

bool m_entity::is_valid_player( bool d ) {

	if ( !this )
		return false;

	if ( !this->is_alive( ) )
		return false;

	if ( csgo::m_local->m_iTeamNum( ) == this->m_iTeamNum( ) )
		return false;

	if ( this->GetClientClass( )->m_ClassID != 35 )
		return false;

	if ( this->EntIndex( ) == g_csgo.engine( )->GetLocalPlayer( ) )
		return false;

	if ( this->m_bGunGameImmunity( ) )
		return false;

	if ( this->IsDormant( ) && d )
		return false;

	return true;
}
matrix3x4_t m_entity::get_bone_matrix( int BoneID )
{
	matrix3x4_t matrix;

	auto offset = *reinterpret_cast< uintptr_t* >( uintptr_t( this ) + 0x2698 );
	if ( offset )
		matrix = *reinterpret_cast< matrix3x4_t* >( offset + 0x30 * BoneID ); // im not sure abt that

	return matrix;
}

void m_entity::m_bClientSideAnimation( bool value )
{
	static int m_bClientSideAnimation = netvar_sys::get( ).GetOffset( "DT_BaseAnimating", "m_bClientSideAnimation" );
	*reinterpret_cast< bool* >( uintptr_t( this ) + m_bClientSideAnimation ) = value;
}

void m_entity::update_animation_state( CCSGOPlayerAnimState* state, Vector angle )
{
	static auto UpdateAnimState = utils::pattern_scan(
		"client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24" );

	if ( !UpdateAnimState )
		return;

	__asm {
		push 0
	}

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[ angle + 4 ]
		movss xmm2, dword ptr[ angle ]

		call UpdateAnimState
	}
}

void m_entity::reset_animation_state( CCSGOPlayerAnimState* state )
{
	using ResetAnimState_t = void( __thiscall* )( CCSGOPlayerAnimState* );
	static auto ResetAnimState = ( ResetAnimState_t )utils::pattern_scan( "client.dll", "56 6A 01 68 ? ? ? ? 8B F1" );
	if ( !ResetAnimState )
		return;

	ResetAnimState( state );
}

void m_entity::create_animation_state( CCSGOPlayerAnimState* state )
{
	using CreateAnimState_t = void( __thiscall* )( CCSGOPlayerAnimState*, m_entity* );
	static auto CreateAnimState = ( CreateAnimState_t )utils::pattern_scan( "client.dll", "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46" );
	if ( !CreateAnimState )
		return;

	CreateAnimState( state, this );
}

bool m_entity::is_alive( )
{
	return m_lifeState( ) == 0;
}

bool m_entity::is_weapon( )
{
	if ( !this )
		return false;

	return call_virtual<bool( __thiscall* )( m_entity* )>( this, 160 )( this );
}

Vector m_entity::get_abs_angles( )
{
	typedef Vector& ( __thiscall* OriginalFn )( void* );
	return ( ( OriginalFn )call_virtual<OriginalFn>( this, 11 ) )( this );
}

Vector& m_entity::get_abs_origin( )
{
	typedef Vector& ( __thiscall* OriginalFn )( void* );
	return ( ( OriginalFn )call_virtual<OriginalFn>( this, 10 ) )( this );
}

int& m_entity::m_bone_count( ) // check for that
{
	return *( int* )( ( uintptr_t )this + 0x291C );
}

int m_entity::num_overlays( )
{
	return *( int* )( ( DWORD )this + 0x297C );
}

void* m_entity::get_renderable( void )
{
	return ( void* )( ( uintptr_t )this + 0x4 );
}

void m_entity::shoot_pos( Vector* vec ) // do that using vectors
{
	static auto Pattern = utils::pattern_scan( "client.dll",
		"55 8B EC 56 8B 75 08 57 8B F9 56 8B 07 FF 90 ? ? ? ?" );

	typedef float* ( __thiscall* ShootPosFn )( void*, Vector* );

	static auto Fn = ( ShootPosFn )Pattern;

	if ( !Fn )
		return;

	Fn( this, vec );
}

Vector m_entity::get_eye_pos( ) // do that using vectors
{
	Vector mod( 0, 0, 0 );
	shoot_pos( &mod );
	return mod;
}

VMatrix& m_entity::GetCollisionBoundTrans( )
{
	return *reinterpret_cast< VMatrix* >( ( DWORD )this + 0x0444 );
}

player_info_t m_entity::GetPlayerInfo( )
{
	player_info_t info;
	g_csgo.engine( )->GetPlayerInfo( EntIndex( ), &info );
	return info;
}

int m_entity::sequence_activity( int sequence )
{
	auto hdr = g_csgo.modelinfo( )->GetStudiomodel( this->GetModel( ) );

	if ( !hdr )
		return -1;

	static auto get_sequence_activity = reinterpret_cast< int( __fastcall* )( void*, studiohdr_t*, int ) >( utils::pattern_scan( "client.dll", "55 8B EC 83 7D 08 FF 56 8B F1 74 3D" ) );
	return get_sequence_activity( this, hdr, sequence );
}

void m_entity::update_client_side_animation( )
{
	return call_virtual< void( __thiscall* ) ( void* ) >( this, 218 ) ( this );
}

void m_entity::set_bone_cache( matrix3x4_t* m ) 
{
	auto cache = this->m_CachedBoneData( );
	memcpy( cache.Base( ), m, sizeof( matrix3x4_t ) * cache.Count( ) );
}

inline const char* m_entity::get_classname( void )
{
	return ( ( const char* ( __thiscall* )( m_entity* ) ) * ( uintptr_t* )( *( uintptr_t* )this + 556 ) )( this );
}

int m_entity::m_iTeamNum( )
{
	static int m_iTeamNum = netvar_sys::get( ).GetOffset( "DT_BaseEntity", "m_iTeamNum" );
	return GetValue<int>( m_iTeamNum );
}

Vector& m_entity::get_mins( ) {
	static int offset = netvar_sys::get( ).GetOffset( "DT_BaseEntity", "m_vecMins" );
	return *( Vector* )( ( DWORD )this + offset );
}

Vector& m_entity::get_maxs( ) {
	static int offset = netvar_sys::get( ).GetOffset( "DT_BaseEntity", "m_vecMaxs" );
	return *( Vector* )( ( DWORD )this + offset );
}

Vector m_entity::hitbox_position( int hitbox_id ) {
	matrix3x4_t matrix[ 128 ];

	if ( this->SetupBones( matrix, 128, 0x100, 0.f ) ) {
		studiohdr_t* hdr = g_csgo.modelinfo( )->GetStudiomodel( this->GetModel( ) ); // remake studio models

		if ( hdr ) {
			mstudiobbox_t* hitbox = hdr->GetHitboxSet( 0 )->GetHitbox( hitbox_id );

			if ( hitbox ) {
				Vector
					min = Vector{ },
					max = Vector{ };

				math::vector_transform( hitbox->bbmin, matrix[ hitbox->bone ], min );
				math::vector_transform( hitbox->bbmax, matrix[ hitbox->bone ], max );

				return ( min + max ) / 2.0f;
			}
		}
	}

	return Vector{ };
}

float_t m_entity::m_flSpawnTime( )
{
	return *( float_t* )( ( uintptr_t )this + 0xA370 );
}

AnimationLayer* m_entity::get_animlayers( )
{
	return *( AnimationLayer** )( ( DWORD )this + 0x2970 );
}

void m_entity::set_abs_angles( const Vector& angles ) {
	if ( !this )
		return;

	static auto _set_abs_angles = ( void( __thiscall* )( void*, const Vector& ) ) utils::pattern_scan( "client.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" );

	_set_abs_angles( this, angles );
}

void m_entity::invalidate_bone_cache( void ) {
	static auto invalidate_bone_cache_fn = utils::pattern_scan( "client.dll",
		"80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" );
	reinterpret_cast< void( __fastcall* ) ( void* ) > ( invalidate_bone_cache_fn ) ( this );
}

CUserCmd*& m_entity::current_command( ) // oh fuck
{
	static auto current_command = *reinterpret_cast< uint32_t* > ( utils::pattern_scan( "client.dll",
		"89 BE ? ? ? ? E8 ? ? ? ? 85 FF" ) + 2 );
	return *reinterpret_cast< CUserCmd** > ( reinterpret_cast< uintptr_t > ( this ) + current_command );
}

void m_entity::set_abs_origin( Vector origin )
{
	using Fn = void( __thiscall* )( void*, const Vector & origin );
	static Fn AbsOrigin = ( Fn )utils::pattern_scan( "client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8" );

	AbsOrigin( this, origin );
}

datamap_t *m_entity::GetPredDescMap( )
{
	typedef datamap_t*( __thiscall *o_GetPredDescMap )( void* );
	return call_virtual<o_GetPredDescMap>( this, 17 )( this );
}

datamap_t * m_entity::GetDataDescMap( )
{
	typedef datamap_t*( __thiscall *o_GetPredDescMap )( void* );
	return call_virtual<o_GetPredDescMap>( this, 15 )( this );
}

bool m_entity::is_player( )
{
	return this->GetClientClass( )->m_ClassID == 35;
	//return call_virtual< bool( __thiscall* ) ( m_entity* ) >( this, 157 ) ( this );
}

const Vector m_entity::world_space_center( )
{
	Vector vec_origin = m_vecOrigin( );

	Vector min = this->GetCollideable( )->OBBMins( ) + vec_origin;
	Vector max = this->GetCollideable( )->OBBMaxs( ) + vec_origin;

	Vector size = max - min;
	size /= 2.f;
	size += min;

	return size;
}

CBoneAccessor* m_entity::GetBoneAccessor( ) // see that
{
	return ( CBoneAccessor* )( ( uintptr_t )this + 0x2698 - 0x4 );
}

uint32_t& m_entity::m_iOcclusionMask( )
{
	return *reinterpret_cast< uint32_t* >( reinterpret_cast< uintptr_t >( this ) + 0xA20 );
}

float& m_entity::m_flLastBoneSetupTime( )// see that
{
	return *reinterpret_cast< float* >( reinterpret_cast< uintptr_t >( this ) + 0x2924 );
}

float& m_entity::m_flVelocityModifier( )// see that
{
	return *reinterpret_cast< float* >( reinterpret_cast< uintptr_t >( this ) + 0xA38C );
}

CStudioHdr* m_entity::GetModelPtr( )
{
	return *( CStudioHdr** )( ( uintptr_t )this + 0x294C );
}

CStudioHdr* m_entity::m_nModelHdr( )
{
	return *reinterpret_cast< CStudioHdr** >( reinterpret_cast< uintptr_t >( this ) + 0x294C );
}

matrix3x4_t**& m_entity::m_nBoneCache( )// see that
{
	return *reinterpret_cast< matrix3x4_t*** >( reinterpret_cast< uintptr_t >( this ) + 0x2910 );
}

CUtlVector< matrix3x4_t >& m_entity::m_CachedBoneData( ) {
	return *( CUtlVector< matrix3x4_t >* )( ( uintptr_t )this + 0x2910 );
}

void m_entity::InvalidatePhysicsRecursive( int32_t flags ) {
	static const auto invalidate_physics_recursive = reinterpret_cast< void( __thiscall* )( m_entity*, int32_t ) >(
		utils::pattern_scan( "client.dll" , "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56" ) );
	invalidate_physics_recursive( this, flags );
}

float_t &m_entity::m_surfaceFriction( )
{
	static unsigned int _m_surfaceFriction = utils::find_in_datamap( GetPredDescMap( ), "m_surfaceFriction" );
	return *( float_t* )( ( uintptr_t )this + _m_surfaceFriction );
}

Vector &m_entity::m_vecBaseVelocity( )
{
	static unsigned int _m_vecBaseVelocity = utils::find_in_datamap( GetPredDescMap( ), "m_vecBaseVelocity" );
	return *( Vector* )( ( uintptr_t )this + _m_vecBaseVelocity );
}

float_t &m_entity::m_flMaxspeed( )
{
	static unsigned int _m_flMaxspeed = utils::find_in_datamap( GetPredDescMap( ), "m_flMaxspeed" );
	return *( float_t* )( ( uintptr_t )this + _m_flMaxspeed );
}

Vector& m_entity::m_vecAbsVelocity( )
{
	static unsigned int _m_flMaxspeed = utils::find_in_datamap( GetPredDescMap( ), "m_vecAbsVelocity" );
	return *( Vector* )( ( uintptr_t )this + _m_flMaxspeed );
}

int m_entity::m_nMoveType( )
{
	return *( int* )( ( uintptr_t )this + 0x258 );
}

int& m_entity::m_fEffects( )
{
	static unsigned int _m_flMaxspeed = utils::find_in_datamap( GetPredDescMap( ), "m_fEffects" );
	return *( int* )( ( uintptr_t )this + _m_flMaxspeed );
}

int& m_entity::m_iEFlags( )
{
	static unsigned int _m_flMaxspeed = utils::find_in_datamap( GetPredDescMap( ), "m_iEFlags" );
	return *( int* )( ( uintptr_t )this + _m_flMaxspeed );
}

Vector m_entity::get_bone_pos( int bone )
{
	matrix3x4_t bone_matrix[ 128 ];

	if ( this->SetupBones( bone_matrix, 128, BONE_USED_BY_ANYTHING, 0.0f ) )
		return bone_matrix[ bone ].at( 3 );

	return Vector{ };
}

m_weapon* m_entity::m_hActiveWeapon( )
{
	static int offset = netvar_sys::get( ).GetOffset( "DT_BaseCombatCharacter", "m_hActiveWeapon" );
	DWORD weaponData = *( DWORD* )( ( DWORD )this + offset );
	return ( m_weapon* )g_csgo.entitylist( )->GetClientEntityFromHandle( weaponData );
}

bool m_entity::is_visible_vector( Vector bone )
{
	if ( !this || !csgo::m_local )
		return false;

	Ray_t ray;
	trace_t tr;
	ray.Init( csgo::m_local->get_eye_pos( ), bone );
	CTraceFilter traceFilter;
	traceFilter.pSkip = csgo::m_local;
	g_csgo.trace( )->TraceRay( ray, MASK_SHOT, &traceFilter, &tr );

	return ( tr.hit_entity == this || tr.IsVisible( ) );
}

float* m_entity::m_flEncodedController( )
{
	return ( float* )( ( uintptr_t )this + 0x0A54 );
}

void m_entity::attachment_helper( )
{
	using AttachmentHelperFn = void( __thiscall* )( m_entity*, CStudioHdr* );
	static AttachmentHelperFn DoAttachments = ( AttachmentHelperFn )( utils::pattern_scan( "client.dll" , "55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4" ) );
	DoAttachments( this, this->GetModelPtr( ) );
};

matrix3x4_t*& m_entity::m_nBoneArrayForWrite( ) {
	return *reinterpret_cast< matrix3x4_t** >( reinterpret_cast< uintptr_t >( this ) + 0x2698 );
}

uint32_t& m_entity::m_nReadebleBones( )
{
	return *reinterpret_cast< uint32_t* >( reinterpret_cast< uintptr_t >( this ) + 0x26AC );
}
uint32_t& m_entity::m_nWritableBones( ) {
	return *reinterpret_cast< uint32_t* >( reinterpret_cast< uintptr_t >( this ) + 0x26B0 );
}

int32_t& m_entity::m_nThinkTick( ) {
	return *reinterpret_cast< int32_t* >( reinterpret_cast< uintptr_t >( this ) + 0x40 );
}

uint32_t& m_entity::m_nMostRecentBoneCounter( ) {
	return *reinterpret_cast< uint32_t* >( reinterpret_cast< uintptr_t >( this ) + 0x2690 );
}

bool m_weapon::is_empty( ) {
	return m_iClip1( ) <= 0;
}

bool m_weapon::can_fire( ) {
	return true;
}

bool m_weapon::can_fire_post_pone( )
{
	float rdyTime = m_flPostponeFireReadyTime( );

	if ( rdyTime > 0 && rdyTime < g_csgo.global_vars( )->m_curtime )
		return true;

	return false;
}

bool m_weapon::is_non_aim( )
{
	int idx = m_iItemDefinitionIndex( );

	return ( idx == WEAPON_C4 || idx == WEAPON_KNIFE || idx == WEAPON_BAYONET || idx == WEAPON_KNIFE_BUTTERFLY || idx == WEAPON_KNIFE_FALCHION
		|| idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_GUT || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_M9_BAYONET || idx == WEAPON_KNIFE_PUSH
		|| idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_TACTICAL || idx == WEAPON_FLASHBANG || idx == WEAPON_HEGRENADE
		|| idx == WEAPON_SMOKEGRENADE || idx == WEAPON_MOLOTOV || idx == WEAPON_DECOY || idx == WEAPON_INCGRENADE );
}

bool m_weapon::is_grenade( )
{
	int idx = m_iItemDefinitionIndex( );

	return ( idx == WEAPON_FLASHBANG || idx == WEAPON_HEGRENADE
		|| idx == WEAPON_SMOKEGRENADE || idx == WEAPON_MOLOTOV || idx == WEAPON_DECOY || idx == WEAPON_INCGRENADE );
}

bool m_weapon::is_pistol( )
{
	int WeapID = m_iItemDefinitionIndex( );

	return WeapID == ItemDefinitionIndex::WEAPON_DEAGLE || WeapID == ItemDefinitionIndex::WEAPON_ELITE || WeapID == ItemDefinitionIndex::WEAPON_FIVESEVEN || WeapID == ItemDefinitionIndex::WEAPON_P250 ||
		WeapID == ItemDefinitionIndex::WEAPON_GLOCK || WeapID == ItemDefinitionIndex::WEAPON_USP_SILENCER || WeapID == ItemDefinitionIndex::WEAPON_CZ75A || WeapID == ItemDefinitionIndex::WEAPON_HKP2000 || WeapID == ItemDefinitionIndex::WEAPON_TEC9 || WeapID == ItemDefinitionIndex::WEAPON_REVOLVER;
}

void m_weapon::penalty( )
{
	typedef void( __thiscall* OrigFn )( void* );
	return call_virtual <OrigFn>( this, 470 )( this );
}

float m_weapon::spread( )
{
	typedef float( __thiscall* oWeaponSpread )( PVOID );
	return call_virtual< oWeaponSpread >( this, 439 )( this );
}

float m_weapon::inaccuracy( )
{
	typedef float( __thiscall* oInaccuracy )( PVOID );
	return call_virtual< oInaccuracy >( this, 469 )( this );
}

bool m_weapon::is_reloading( ) // see that
{
	static auto in_reload = *reinterpret_cast< uint32_t* > ( utils::pattern_scan( ( "client.dll" ), ( "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90" ) ) + 2 );
	return *reinterpret_cast< bool* > ( reinterpret_cast< uintptr_t > ( this ) + in_reload );
}

const char* m_weapon::weapon_ic( )
{
	int WeapID = m_iItemDefinitionIndex( );

	switch ( WeapID )
	{
	case WEAPON_BAYONET:
		return "1";
	case WEAPON_KNIFE:
		return "[";
	case WEAPON_KNIFE_T:
		return "[";
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return "7";
	case WEAPON_KNIFE_BUTTERFLY:
		return "8";
	case WEAPON_KNIFE_FALCHION:
		return "0";
	case WEAPON_KNIFE_FLIP:
		return "2";
	case WEAPON_KNIFE_GUT:
		return "3";
	case WEAPON_KNIFE_KARAMBIT:
		return "4";
	case WEAPON_KNIFE_M9_BAYONET:
		return "5";
	case WEAPON_KNIFE_TACTICAL:
		return "6";
	case WEAPON_KNIFE_PUSH:
		return "]";
	case WEAPON_DEAGLE:
		return "A";
	case WEAPON_ELITE:
		return "B";
	case WEAPON_FIVESEVEN:
		return "C";
	case WEAPON_GLOCK:
		return "D";
	case WEAPON_HKP2000:
		return "E";
	case WEAPON_P250:
		return "F";
	case WEAPON_USP_SILENCER:
		return "G";
	case WEAPON_TEC9:
		return "H";
	case WEAPON_REVOLVER:
		return "A";
	case WEAPON_MAC10:
		return "K";
	case WEAPON_UMP45:
		return "L";
	case WEAPON_BIZON:
		return "M";
	case WEAPON_MP7:
		return "N";
	case WEAPON_MP9:
		return "O";
	case WEAPON_P90:
		return "P";
	case WEAPON_GALILAR:
		return "Q";
	case WEAPON_FAMAS:
		return "R";
	case WEAPON_M4A1_SILENCER:
		return "S";
	case WEAPON_M4A1:
		return "T";
	case WEAPON_AUG:
		return "U";
	case WEAPON_SG556:
		return "V";
	case WEAPON_AK47:
		return "W";
	case WEAPON_G3SG1:
		return "X";
	case WEAPON_SCAR20:
		return "Y";
	case WEAPON_AWP:
		return "Z";
	case WEAPON_SSG08:
		return "a";
	case WEAPON_XM1014:
		return "b";
	case WEAPON_SAWEDOFF:
		return "c";
	case WEAPON_MAG7:
		return "d";
	case WEAPON_NOVA:
		return "e";
	case WEAPON_NEGEV:
		return "f";
	case WEAPON_M249:
		return "g";
	case WEAPON_TASER:
		return "h";
	case WEAPON_FLASHBANG:
		return "i";
	case WEAPON_HEGRENADE:
		return "j";
	case WEAPON_SMOKEGRENADE:
		return "k";
	case WEAPON_MOLOTOV:
		return "l";
	case WEAPON_DECOY:
		return "m";
	case WEAPON_INCGRENADE:
		return "n";
	case WEAPON_C4:
		return "o";
	case WEAPON_CZ75A:
		return "I";
	default:
		return " ";
	}
}

weapon_info_t* m_weapon::m_data( )
{
	return call_virtual< weapon_info_t * ( __thiscall* )( void* ) >( this, 446 )( this );
}
CSWeaponInfo_t* m_weapon::m_data2( )
{
	return call_virtual< CSWeaponInfo_t* ( __thiscall* )( void* ) >( this, 446 )( this );
}

bool m_weapon::is_sniper( )
{
	int WeaponId = this->m_iItemDefinitionIndex( );
	return WeaponId == WEAPON_AWP || WeaponId == WEAPON_SCAR20 || WeaponId == WEAPON_G3SG1 || WeaponId == WEAPON_SSG08;
}

float m_weapon::last_shot_time( )
{
	static int offset = netvar_sys::get( ).GetOffset( "DT_WeaponCSBase", "m_fLastShotTime" );
	return *( float* )( ( DWORD )this + offset );
}

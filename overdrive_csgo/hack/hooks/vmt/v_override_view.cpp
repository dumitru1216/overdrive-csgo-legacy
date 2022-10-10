#include "vhooks.h"

#include "../../globals.h"
#include "../../features/visuals.h"

#include "../../sdk/other/e_nums.h"
#include "../../../gui/config/config.h"

typedef void( __fastcall* over_ride_view_t ) ( void*, void*, CViewSetup* );

void __fastcall hooks::override_view( void* ecx, void* edx, CViewSetup* pSetup )
{
	static auto original = hooks::clientmode.get_original<over_ride_view_t>( indexes::m_override_view );

	if ( m_inUnhook )
		return original( ecx, edx, pSetup );

	if ( g_csgo.engine( )->IsInGame( ) && g_csgo.engine( )->IsConnected( ) && csgo::m_local )
	{
		static bool once = false;

		if ( !once )
		{
			once = true;
			csgo::m_init_fov = pSetup->fov;
		}

		visuals::get( ).third_person( );

		if ( cfg.removals.remove_recoil )
		{
			Vector viewPunch = csgo::m_local->m_viewPunchAngle( );
			Vector aimPunch = csgo::m_local->m_aimPunchAngle( );

			pSetup->angles[ 0 ] -= ( viewPunch[ 0 ] + ( aimPunch[ 0 ] * 2 * 0.4499999f ) );
			pSetup->angles[ 1 ] -= ( viewPunch[ 1 ] + ( aimPunch[ 1 ] * 2 * 0.4499999f ) );
			pSetup->angles[ 2 ] -= ( viewPunch[ 2 ] + ( aimPunch[ 2 ] * 2 * 0.4499999f ) );
		}

		if ( cfg.removals.remove_zoom && csgo::m_local->m_bIsScoped( ) )
		{
			auto m_weapon = csgo::m_local->m_hActiveWeapon( );

			if ( !m_weapon )
				return;

			auto id = m_weapon->m_iItemDefinitionIndex( );

			if ( id == WEAPON_G3SG1 || id == WEAPON_SCAR20 || id == WEAPON_AWP || id == WEAPON_SSG08 )
			{
				if ( m_weapon->m_zoomLevel( ) == 1 )
					pSetup->fov = csgo::m_init_fov;
				else if ( m_weapon->m_zoomLevel( ) == 2 )
					pSetup->fov = csgo::m_init_fov / 2;
			}
		}
	}

	original( ecx, edx, pSetup );
}
#include "dhooks.h"

#include "../../sdk/entity/entity.h"
#include "../../globals.h"

#include "../../features/animations.h"
#include "../../../gui/config/config.h"

dhooks::setupbones_t dhooks::original_setup_bones;

bool __fastcall dhooks::setup_bones( iclient_renderable* ecx, void* edx, matrix3x4_t* bone_to_world_out, int max_bones, int bone_mask, float curtime )
{
	auto pEnt = reinterpret_cast< m_entity* >( uintptr_t( ecx ) - 0x4 );

	if ( !pEnt || !pEnt->is_player( ) )
		return original_setup_bones( ecx, bone_to_world_out, max_bones, bone_mask, curtime );

	if ( pEnt->EntIndex( ) == g_csgo.engine( )->GetLocalPlayer( ) )
	{
		*( int* )( ( DWORD )pEnt + 0x2698 ) = 0;
		bone_mask |= 0x200;

		if ( pEnt->m_vecVelocity( ).Length( ) < 10 )
			pEnt->get_animlayers( )[ 6 ].m_flWeight = 0.0f;

		if ( pEnt->get_animation_state( )->m_bInHitGroundAnimation )
		{
			pEnt->get_animlayers( )[ 5 ].m_flWeight = 1.0f;
			pEnt->get_animlayers( )[ 5 ].m_flCycle = 1.0f;
		}

		pEnt->get_animlayers( )[ 3 ].m_flWeight = 0.0f;
		pEnt->get_animlayers( )[ 3 ].m_flCycle = 0.0f;
		pEnt->get_animlayers( )[ 12 ].m_flWeight = std::numeric_limits<float>::epsilon( );

		if ( !csgo::m_allow_setup_bones )
		{
			if ( bone_to_world_out )
			{
				if ( animations::get( ).get_local_matrix )
					memcpy( bone_to_world_out, animations::get( ).real_matrix, sizeof( matrix3x4_t ) * max_bones );
			}

			return true;
		}
		else
		{
			if ( ecx )
				*reinterpret_cast< int* > ( reinterpret_cast< uintptr_t > ( ecx ) + 0x269C ) = 0;

			*reinterpret_cast< float* > ( reinterpret_cast< uintptr_t > ( pEnt ) + 2664 ) = 0;

			static auto offs_bone_mask = netvar_sys::get( ).GetOffset( "DT_BaseAnimating", "m_nForceBone" ) + 0x20;
			*reinterpret_cast< int* > ( reinterpret_cast< uintptr_t > ( pEnt ) + offs_bone_mask ) = 0;
		}

		*( uint32_t* )( uintptr_t( pEnt ) + 0xA68 ) = 0;
	}

	pEnt->m_fEffects( ) |= 8;
	bool m_bSetuped = original_setup_bones( ecx, bone_to_world_out, max_bones, bone_mask, curtime );
	pEnt->m_fEffects( ) &= ~8;

	return m_bSetuped;
}
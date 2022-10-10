#include "dhooks.h"

#include "../../sdk/entity/entity.h"
#include "../../globals.h"

#include "../../features/animations.h"
#include "../../../gui/config/config.h"

dhooks::update_client_side_animation_t dhooks::origin_update_client_side_animation;

void __fastcall dhooks::update_client_side_animation( m_entity* player, uint32_t )
{
	if ( !player || !player->is_alive( ) || m_inUnhook )
		return origin_update_client_side_animation( player );

	if ( player == csgo::m_local )
	{
		if ( animations::get( ).get_local_matrix )
		{
			for ( int i = 0; i < 128; i++ )
				animations::get( ).real_matrix[ i ].SetOrigin( player->get_abs_origin( ) - animations::get( ).origin_local[ i ] );

			memcpy( player->m_CachedBoneData( ).Base( ), animations::get( ).real_matrix, sizeof( matrix3x4_t ) * player->m_bone_count( ) );

			const auto oldBones = player->GetBoneAccessor( )->m_pBones;
			player->GetBoneAccessor( )->m_pBones = animations::get( ).real_matrix;
			player->attachment_helper( );
			player->GetBoneAccessor( )->m_pBones = oldBones;
		}

		if ( csgo::m_update[ player->EntIndex( ) ] )
			origin_update_client_side_animation( player );
	}
	else
		origin_update_client_side_animation( player );
}
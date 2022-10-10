#include "dhooks.h"
#include "../../globals.h"

dhooks::skip_frame_t dhooks::skip_original;

bool __fastcall dhooks::should_skip_animation_frame( )
{
	if ( m_inUnhook )
		return skip_original( );

	return false;
}
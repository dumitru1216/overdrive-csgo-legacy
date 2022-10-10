#include "dhooks.h"

#include "../../sdk/entity/entity.h"
#include "../../globals.h"

dhooks::do_extra_bone_processing_t dhooks::orig_do_extra_bone_processing;

void _fastcall dhooks::do_extra_bone_processing( void* ecx, uint32_t, studiohdr_t* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_computed, void* context )
{
	return;
}
#pragma once

#include "../../utils/singleton.h"
#include "../sdk/entity/entity.h"
#include "../globals.h"

class chams : public singleton < chams >
{
	friend class singleton< chams >;

	chams( );
	~chams( );

public:

	IMaterial* material_default = nullptr;
	IMaterial* material_flat = nullptr;
	IMaterial* material_glow = nullptr;

	void on_render( IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pinfo, matrix3x4_t* matrix );
	void override_material( bool ignoreZ, bool use_env, const Color& rgba, IMaterial* material, float alpha );

};
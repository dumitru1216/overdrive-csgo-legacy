#include "../other/virtual.h"

class i_mem_alloc {
public:
	auto alloc( int nSize ) -> void* {
		using Fn = void* ( __thiscall* )( void*, int );
		return call_virtual< Fn >( this, 1 )( this, nSize );
	}

	auto realloc( void* pMem, int nSize ) -> void* {
		using Fn = void* ( __thiscall* )( void*, void*, int );
		return call_virtual< Fn >( this, 3 )( this, pMem, nSize );
	}

	auto free( void* pMem ) -> void {
		using Fn = void( __thiscall* )( void*, void* );
		return call_virtual< Fn >( this, 5 )( this, pMem );
	}
};
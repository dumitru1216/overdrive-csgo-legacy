#include "utl.h"
#include "../hack/sdk/interfaces/create_if.h"

namespace utils
{
	unsigned long pattern_scan( const char* module_name, const char* signature )
	{
		MODULEINFO module_info;

		GetModuleInformation( GetCurrentProcess( ), GetModuleHandleA( module_name ), &module_info, sizeof( MODULEINFO ) );

		DWORD start_addr = ( DWORD )module_info.lpBaseOfDll;
		DWORD end_addr = start_addr + module_info.SizeOfImage;
		const char* pat = signature;

		DWORD first_match = 0;

		for ( DWORD cur = start_addr; cur < end_addr; cur++ ) {
			if ( !*pat ) return first_match;
			if ( *( PBYTE )pat == '\?' || *( BYTE* )cur == GETBYTE( pat ) ) {
				if ( !first_match ) first_match = cur;
				if ( !pat[ 2 ] ) return first_match;
				if ( *( PWORD )pat == '\?\?' || *( PBYTE )pat != '\?' ) pat += 3;
				else pat += 2;
			}
			else {
				pat = signature;
				first_match = 0;
			}
		}

		return NULL;
	}
	// ---------------------------------------------------------------------
	unsigned int find_in_datamap( datamap_t *pMap, const char *name ) {
		while ( pMap ) {
			for ( int i = 0; i < pMap->dataNumFields; i++ ) {
				if ( pMap->dataDesc[ i ].fieldName == NULL )
					continue;

				if ( strcmp( name, pMap->dataDesc[ i ].fieldName ) == 0 )
					return pMap->dataDesc[ i ].fieldOffset[ TD_OFFSET_NORMAL ];

				if ( pMap->dataDesc[ i ].fieldType == FIELD_EMBEDDED ) {
					if ( pMap->dataDesc[ i ].td ) {
						unsigned int offset;

						if ( ( offset = find_in_datamap( pMap->dataDesc[ i ].td, name ) ) != 0 )
							return offset;
					}
				}
			}
			pMap = pMap->baseMap;
		}

		return 0;
	}
	// ---------------------------------------------------------------------
	bool world_to_screen( const Vector& origin, Vector2D& screen )
	{
		const auto screenTransform = [ &origin, &screen ]( ) -> bool
		{
			static std::uintptr_t pViewMatrix;
			if ( !pViewMatrix )
			{
				pViewMatrix = static_cast< std::uintptr_t >( pattern_scan( "client.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9" ) );
				pViewMatrix += 3;
				pViewMatrix = *reinterpret_cast< std::uintptr_t* >( pViewMatrix );
				pViewMatrix += 176;
			}

			const VMatrix& w2sMatrix = *reinterpret_cast< VMatrix* >( pViewMatrix );
			screen.x = w2sMatrix.m[ 0 ][ 0 ] * origin.x + w2sMatrix.m[ 0 ][ 1 ] * origin.y + w2sMatrix.m[ 0 ][ 2 ] * origin.z + w2sMatrix.m[ 0 ][ 3 ];
			screen.y = w2sMatrix.m[ 1 ][ 0 ] * origin.x + w2sMatrix.m[ 1 ][ 1 ] * origin.y + w2sMatrix.m[ 1 ][ 2 ] * origin.z + w2sMatrix.m[ 1 ][ 3 ];

			float w = w2sMatrix.m[ 3 ][ 0 ] * origin.x + w2sMatrix.m[ 3 ][ 1 ] * origin.y + w2sMatrix.m[ 3 ][ 2 ] * origin.z + w2sMatrix.m[ 3 ][ 3 ];

			if ( w < 0.001f )
			{
				screen.x *= 100000;
				screen.y *= 100000;
				return true;
			}

			float invw = 1.f / w;
			screen.x *= invw;
			screen.y *= invw;

			return false;
		};

		if ( !screenTransform( ) )
		{
			int iScreenWidth, iScreenHeight;
			g_csgo.engine( )->GetScreenSize( iScreenWidth, iScreenHeight );

			screen.x = ( iScreenWidth * 0.5f ) + ( screen.x * iScreenWidth ) * 0.5f;
			screen.y = ( iScreenHeight * 0.5f ) - ( screen.y * iScreenHeight ) * 0.5f;

			return true;
		}
		return false;
	}	bool get_key( int key, int style )
	{
		if ( style == 0 )
			return true;
		else if ( style == 1 )
			return GetKeyState( key );
		else if ( style == 2 )
			return GetAsyncKeyState( key );
		else if ( style == 3 )
			return !GetAsyncKeyState( key );
	}
};
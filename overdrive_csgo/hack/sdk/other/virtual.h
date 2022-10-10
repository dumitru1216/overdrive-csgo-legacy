#pragma once

template < typename T >
T call_virtual( void* address, int index )
{
	return ( *reinterpret_cast< T * * >( address ) )[ index ];
}
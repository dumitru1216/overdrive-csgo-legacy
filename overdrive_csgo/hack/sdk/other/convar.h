#pragma once
#include "virtual.h"

class ConVar
{
public:

	void SetValue( const char* str )
	{
		typedef void( __thiscall* Fn )( void*, const char* );
		return call_virtual<Fn>( this, 13 )( this, str );
	}
	void SetValue( float value )
	{
		typedef void( __thiscall* Fn )( void*, float );
		return call_virtual<Fn>( this, 15 )( this, value );
	}
	void SetValue( int value )
	{
		typedef void( __thiscall* Fn )( void*, int );
		return call_virtual<Fn>( this, 16 )( this, value );
	}
	void SetValueChar( const char* name )
	{
		typedef void( __thiscall* Fn )( void*, const char* );
		return call_virtual<Fn>( this, 14 )( this, name );
	}
	void InternalSetString( const char* str )
	{
		typedef void( __thiscall* Fn )( void*, const char* );
		return call_virtual<Fn>( this, 17 )( this, str );
	}
	char* GetBaseName( )
	{
		typedef char* ( __thiscall* Fn )( void* );
		return call_virtual<Fn>( this, 6 )( this );
	}
	float GetFloat( )
	{
		typedef float( __thiscall* Fn )( void* );
		return call_virtual< Fn >( this, 12 )( this );
	}
	bool GetBool( void ) {
		return !!GetInt( );
	}
	int GetInt( void ) const {
		return pParent->nValue;
	}
	const char* GetString( void ) const {
		return pParent->pszDefaultValue;
	}

	char pad_0x0000[ 0x4 ]; //0x0000
	ConVar* pNext; //0x0004 
	__int32 bRegistered; //0x0008 
	char* pszName; //0x000C 
	char* pszHelpString; //0x0010 
	__int32 nFlags; //0x0014 
	char pad_0x0018[ 0x4 ]; //0x0018
	ConVar* pParent; //0x001C 
	char* pszDefaultValue; //0x0020 
	char* strString; //0x0024 
	__int32 StringLength; //0x0028 
	float fValue; //0x002C 
	__int32 nValue; //0x0030 
	__int32 bHasMin; //0x0034 
	float fMinVal; //0x0038 
	__int32 bHasMax; //0x003C 
	float fMaxVal; //0x0040 
	void* fnChangeCallback; //0x0044 
};
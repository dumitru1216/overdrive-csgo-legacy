#pragma once

typedef void* ( *create_interface_fn )( const char* pName, int* pReturnCode );
typedef void* ( *instantiate_interface_fn )( );

class i_app_system
{
public:
	virtual bool                            connect( create_interface_fn factory ) = 0;                                     // 0
	virtual void                            disconnect( ) = 0;                                                           // 1
	virtual void*							queryinterface( const char* pinterfacename ) = 0;                             // 2
	virtual int /*initreturnval_t*/         init( ) = 0;                                                                 // 3
	virtual void                            shutdown( ) = 0;                                                             // 4
	virtual const void* /*appsysteminfo_t*/ getdependencies( ) = 0;                                                      // 5
	virtual int /*appsystemtier_t*/         gettier( ) = 0;                                                              // 6
	virtual void                            reconnect( create_interface_fn factory, const char* pinterfacename ) = 0;       // 7
	virtual void                            unkfunc( ) = 0;                                                   // 8
};

class IEngineVGui
{
public:
	virtual					~IEngineVGui( void ) { }
	virtual int	GetPanel( int type ) = 0;
	virtual bool			IsGameUIVisible( ) = 0;
};
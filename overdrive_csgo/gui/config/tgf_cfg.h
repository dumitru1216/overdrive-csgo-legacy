#pragma once
#include <iostream>
#include <vector>

template< typename T >
class ConfigValue
{
public:
	ConfigValue( std::string category_, std::string name_, T* value_ )
	{
		category = category_;
		name = name_;
		value = value_;
	}

	std::string category, name;
	T* value;
};

class CConfig
{
protected:

	std::vector< ConfigValue< int >* > ints;
	std::vector< ConfigValue< bool >* > bools;
	std::vector< ConfigValue< float >* > floats;
	std::vector< ConfigValue< unsigned long >* > longs;

private:

	void setup_value( int&, int, std::string, std::string );
	void setup_value( unsigned long&, unsigned long, std::string, std::string );
	void setup_value( bool&, bool, std::string, std::string );
	void setup_value( float&, float, std::string, std::string );

public:

	CConfig( );
	~CConfig( );
	
	void setup( );

	void save( std::string ConfigName );
	void load( std::string ConfigName );

};

extern CConfig* m_setup;
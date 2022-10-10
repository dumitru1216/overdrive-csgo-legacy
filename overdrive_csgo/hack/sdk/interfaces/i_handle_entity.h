#pragma once

using BaseHandle = uint32_t;
class IHandleEntity
{
public:
	virtual ~IHandleEntity( ) = default;
	virtual void SetRefEHandle( const BaseHandle& handle ) = 0;
	virtual const BaseHandle& GetRefEHandle( ) const = 0;
};
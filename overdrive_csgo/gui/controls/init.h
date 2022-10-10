#pragma once

#include "../../hack/sdk/src_kit.h"
#include "../../utils/singleton.h"
#include "../../render/render.h"
#include "../../hack/sdk/interfaces/create_if.h"

class m_draw : public singleton<m_draw>
{
public:

    int m_pos_x;
    int m_pos_y;

public:

    LPDIRECT3DTEXTURE9 ColorPickerTexture;
    LPD3DXSPRITE ColorPickerSprite;
    int ColorPickerWidth;
    int ColorPickerHeight;

    D3DCOLOR HSLToRGB( float H, float S, float L )
    {
        float Q;

        if ( L < 0.5f )
            Q = L * ( S + 1.0f );
        else
            Q = L + S - ( L * S );

        float P = 2 * L - Q;

        float RGBs[ 3 ];

        RGBs[ 0 ] = H + ( 1.0f / 3.0f );
        RGBs[ 1 ] = H;
        RGBs[ 2 ] = H - ( 1.0f / 3.0f );

        for ( int i = 0; i < 3; ++i )
        {
            if ( RGBs[ i ] < 0 )
                RGBs[ i ] += 1.0f;

            if ( RGBs[ i ] > 1 )
                RGBs[ i ] -= 1.0f;

            if ( RGBs[ i ] < ( 1.0f / 6.0f ) )
                RGBs[ i ] = P + ( ( Q - P ) * 6 * RGBs[ i ] );
            else if ( RGBs[ i ] < 0.5f )
                RGBs[ i ] = Q;
            else if ( RGBs[ i ] < ( 2.0f / 3.0f ) )
                RGBs[ i ] = P + ( ( Q - P ) * 6 * ( ( 2.0f / 3.0f ) - RGBs[ i ] ) );
            else
                RGBs[ i ] = P;
        }

        return D3DCOLOR_XRGB( int( RGBs[ 0 ] * 255.0f ), int( RGBs[ 1 ] * 255.0f ), int( RGBs[ 2 ] * 255.0f ) );
    }

    void ColorPicker_Destroy( )
    {
        if ( ColorPickerSprite != NULL )
        {
            ColorPickerSprite->Release( );
            ColorPickerSprite = NULL;
        }

        if ( ColorPickerTexture != NULL )
        {
            ColorPickerTexture->Release( );
            ColorPickerTexture = NULL;
        }
    }

    HRESULT ColorPicker_Init( LPDIRECT3DDEVICE9 Device, int Width, int Height )
    {
        ColorPickerWidth = Width;
        ColorPickerHeight = Height;

        bool Bits32 = true;
        ColorPicker_Destroy( );

        HRESULT CreateTextureReturn;

        CreateTextureReturn = Device->CreateTexture( Width, Height, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &ColorPickerTexture, 0 );

        if ( FAILED( CreateTextureReturn ) )
        {
            Bits32 = false;

            CreateTextureReturn = Device->CreateTexture( Width, Height, 1, 0, D3DFMT_X4R4G4B4, D3DPOOL_MANAGED, &ColorPickerTexture, 0 );

            if ( FAILED( CreateTextureReturn ) )
                return CreateTextureReturn;
        }

        D3DLOCKED_RECT Palette;

        CreateTextureReturn = ColorPickerTexture->LockRect( 0, &Palette, 0, 0 );

        if ( FAILED( CreateTextureReturn ) )
        {
            ColorPicker_Destroy( );
            return CreateTextureReturn;
        }

        float H = 0;
        float S = 0.99f;
        float L = 1.0f;

        D3DCOLOR Color;

        BYTE R;
        BYTE G;
        BYTE B;

        DWORD* Colors32 = ( ( DWORD* )Palette.pBits ) - 1;
        WORD* Colors = ( ( WORD* )Palette.pBits ) - 1;

        for ( int i = 0; i < Width; ++i )
        {
            for ( int j = 0; j < Height; ++j )
            {
                Color = HSLToRGB( H, S, L );

                if ( Bits32 )
                {
                    Colors32++;
                    *Colors32 = Color;
                }
                else
                {
                    R = ( ( Color >> 16 ) & 0xFF ) / 0x10;
                    G = ( ( Color >> 8 ) & 0xFF ) / 0x10;
                    B = ( ( Color >> 0 ) & 0xFF ) / 0x10;

                    Colors++;

                    *Colors = ( 0xFF << 12 ) | ( R << 8 ) | ( G << 4 ) | ( B << 0 );
                }

                H += ( 1.0f / Width );
            }

            L -= ( 1.0f / Height );
            H = 0.0f;
        }

        ColorPickerTexture->UnlockRect( 0 );

        if ( ColorPickerSprite == NULL )
            D3DXCreateSprite( Device, &ColorPickerSprite );

        return S_OK;
    }

    LPDIRECT3DTEXTURE9 ColorPicker_GetTexture( )
    {
        return ColorPickerTexture;
    }

    void DrawColorPicker( float X, float Y, LPDIRECT3DDEVICE9 Device )
    {
        if ( ColorPickerSprite != NULL )
        {
            ColorPickerSprite->Begin( D3DXSPRITE_ALPHABLEND );
            ColorPickerSprite->Draw( ColorPicker_GetTexture( ), NULL, NULL, &D3DXVECTOR3( X, Y, 0.0f ), 0xFFFFFFFF );
            ColorPickerSprite->End( );
        }
    }

    D3DCOLOR GetPickedColor( int X, int Y )
    {
        float H = X * ( 1.0f / ColorPickerWidth );
        float S = 0.99f;
        float L = 1.0f - Y * ( 1.0f / ColorPickerHeight );

        return HSLToRGB( H, S, L );
    }

};

#define sprite m_draw::get( ) 
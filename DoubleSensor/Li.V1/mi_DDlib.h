//**************************************************************************************
// Copyright 2009 Aptina Imaging Corporation. All rights reserved.
//
//
// No permission to use, copy, modify, or distribute this software and/or
// its documentation for any purpose has been granted by Aptina Imaging Corporation.
// If any such permission has been granted ( by separate agreement ), it
// is required that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear in
// supporting documentation, and that the name of Aptina Imaging Corporation or any
// of its trademarks may not be used in advertising or publicity pertaining
// to distribution of the software without specific, written prior permission.
//
//
//      This software and any associated documentation are provided "AS IS" and
//      without warranty of any kind. APTINA IMAGING CORPORATION EXPRESSLY DISCLAIMS
//      ALL WARRANTIES EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO, NONINFRINGEMENT
//      OF THIRD PARTY RIGHTS, AND ANY IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS
//      FOR A PARTICULAR PURPOSE. APTINA DOES NOT WARRANT THAT THE FUNCTIONS CONTAINED
//      IN THIS SOFTWARE WILL MEET YOUR REQUIREMENTS, OR THAT THE OPERATION OF THIS SOFTWARE
//      WILL BE UNINTERRUPTED OR ERROR-FREE. FURTHERMORE, APTINA DOES NOT WARRANT OR
//      MAKE ANY REPRESENTATIONS REGARDING THE USE OR THE RESULTS OF THE USE OF ANY
//      ACCOMPANYING DOCUMENTATION IN TERMS OF ITS CORRECTNESS, ACCURACY, RELIABILITY,
//      OR OTHERWISE.
//*************************************************************************************/

#pragma once

///////////////////////////////////////////////////
//  Pick one

//#define USE_DIRECTDRAW7
#define USE_DIRECTX9

///////////////////////////////////////////////////


#ifdef USE_DIRECTDRAW7
#include <ddraw.h>
#endif
#ifdef USE_DIRECTX9
#include <d3d9.h>
#endif


class CDirectDrawSurfaceBase
{
public:
    CDirectDrawSurfaceBase();
    ~CDirectDrawSurfaceBase();

    enum {
        USEDD_STANDARD,
        USEDD_ACCELERATED,
        USEDD_NONE,
    };

//Implementation
public:
    virtual HRESULT Create(HWND hWnd, int width, int height,
                           bool bFullscreen = false);
    virtual HRESULT CopyText(const char *pStr, bool bOutline = true,
                             UINT uFormat = DT_NOCLIP | DT_TOP | DT_CENTER,
                             COLORREF cRGBoutline = RGB(0,0,0),
                             COLORREF cRGB = RGB(255,255,255),
                             int x = 0, int y = 2);
    virtual HRESULT CreateSourceSurface(unsigned char *pBuffer, int width, int height);
    virtual HRESULT DestroySourceSurface(unsigned char *pBuffer);
    virtual HRESULT CopyBitmap(unsigned char *pBuffer, int dx, int dy, int sx, int sy);
    virtual HRESULT CopyBitmap(HBITMAP hbm, int dx, int dy, int sx = 0, int sy = 0);
    virtual HRESULT Flip();
    virtual bool    StereoScreen() { return false; }

    virtual void    SetDirectDrawAccel(int bUseDirectDrawAccel)    { m_bUseDirectDrawAccel = bUseDirectDrawAccel; }
    virtual int     GetDirectDrawAccel()                           { return m_bUseDirectDrawAccel; }
    virtual int     ImageBpp()                                     { return m_nImageBpp; }

protected:
    HWND            m_hWnd;
    int             m_bUseDirectDrawAccel;
    bool            m_bFullscreen;
    int             m_nImageBpp;
};



#ifdef USE_DIRECTDRAW7

#include <ddraw.h>

class CDirectDrawSurface : public CDirectDrawSurfaceBase
{
public:
    CDirectDrawSurface();
    ~CDirectDrawSurface();

//Attributes
protected:
    int                     m_bRemoteDesktop;
    LPDIRECTDRAW7           m_lpDD;         // DirectDraw object defined in DDRAW.H
    LPDIRECTDRAWSURFACE7    m_lpDDSPrimary; // DirectDraw primary surface
    LPDIRECTDRAWSURFACE7    m_lpDDSBack;    // DirectDraw back surface
    LPDIRECTDRAWCLIPPER     m_lpClipper;    // DirectDraw clipper
    int                     m_width;
    int                     m_height;
    int                     m_nFullscreenW;
    int                     m_nFullscreenH;
    LPDIRECTDRAWSURFACE7    m_lpDDSSource[4]; // DirectDraw source surfaces (around our system memory buffers)
    unsigned char *         m_lpSource[4];

    enum {
        GDI_TO_WINDOW_DC,
        GDI_TO_PRIMARY,
        GDI_TO_BACK__DD_TO_PRIMARY,
        GDI_TO_BACK__FLIP,
        DD_TO_BACK__FLIP,
        DD_TO_PRIMARY,
    } m_method;

//Implementation
public:
    HRESULT Create(HWND hWnd, int width, int height,
                   bool bFullscreen = false);
    HRESULT CopyText(const char *pStr, bool bOutline = true,
                     UINT uFormat = DT_NOCLIP | DT_TOP | DT_CENTER,
                     COLORREF cRGBoutline = RGB(0,0,0), COLORREF cRGB = RGB(255,255,255),
                     int x = 0, int y = 2);
    HRESULT CreateSourceSurface(unsigned char *pBuffer, int width, int height);
    HRESULT DestroySourceSurface(unsigned char *pBuffer);
    HRESULT CopyBitmap(unsigned char *pBuffer, int dx, int dy, int sx, int sy);
    HRESULT CopyBitmap(HBITMAP hbm, int dx, int dy, int sx = 0, int sy = 0);
    HRESULT Flip();

protected:
    void    UpdateBackBuffer();
    HRESULT restoreAll();
    HRESULT CopyBitmap(unsigned char *pBuffer,
                       HBITMAP hbm,
                       int dx, int dy, int sx, int sy);
};

#endif /// USE_DIRECTDRAW7




#ifdef USE_DIRECTX9

#ifndef DD_OK
#define DD_OK D3D_OK
#endif

#define NVSTEREO_IMAGE_SIGNATURE 0x4433564e
typedef struct _Nv_Stereo_Image_Header
{
    unsigned int dwSignature;
    unsigned int dwWidth;
    unsigned int dwHeight;
    unsigned int dwBPP;
    unsigned int dwFlags;
} NVSTEREOIMAGEHEADER, *LPNVSTEREOIMAGEHEADER;
#define SIH_SWAP_EYES 0x00000001
#define SIH_SCALE_TO_FIT 0x00000002


class CDirectDrawSurface : public CDirectDrawSurfaceBase
{
public:
    CDirectDrawSurface();
    ~CDirectDrawSurface();

//Attributes
protected:
    int                     m_bRemoteDesktop;
    LPDIRECT3D9             m_pD3D;
    LPDIRECT3DDEVICE9       m_pd3dDevice;
    IDirect3DSurface9 *     m_pDDSBack;    // DirectDraw back surface
    int                     m_width;
    int                     m_height;
    int                     m_nFullscreenW;
    int                     m_nFullscreenH;
    int                     m_nBackbufferW;
    int                     m_nBackbufferH;
    IDirect3DSurface9 *     m_pDDSMono;
    IDirect3DSurface9 *     m_pDDSStereo;
    IDirect3DSurface9 *     m_pDDSSource[4]; // texture surfaces (around our system memory buffers)
    unsigned char *         m_pSource[4];

    enum {
        GDI_TO_WINDOW_DC,
        GDI_TO_BACK__FLIP,
        CPY_TO_STEREO_BACK__FLIP,
        D3D_TO_BACK__FLIP,
    } m_method;

//Implementation
public:
    HRESULT Create(HWND hWnd, int width, int height,
                   bool bFullscreen = false, bool bStereo = false);
    void    Destroy();
    HRESULT CopyText(const char *pStr, bool bOutline = true,
                     UINT uFormat = DT_NOCLIP | DT_TOP | DT_CENTER,
                     COLORREF cRGBoutline = RGB(0,0,0), COLORREF cRGB = RGB(255,255,255),
                     int x = 0, int y = 2);
    HRESULT CreateSourceSurface(unsigned char *pBuffer, int width, int height);
    HRESULT DestroySourceSurface(unsigned char *pBuffer);
    HRESULT CopyBitmap(unsigned char *pBuffer, int dx, int dy, int sx, int sy);
    HRESULT CopyBitmap(HBITMAP hbm, int dx, int dy, int sx = 0, int sy = 0);
    HRESULT Flip();
    bool    StereoScreen();

protected:
    void    UpdateBackBuffer();
    HRESULT restoreAll();
    HRESULT CopyBitmap(unsigned char *pBuffer,
                       HBITMAP hbm,
                       int dx, int dy, int sx, int sy);
};

#endif /// USE_DIRECTX9

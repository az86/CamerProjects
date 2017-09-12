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
// mi_DDlib.cpp : implementation file
//
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "mi_DDlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int g_FullscreenResolution[10][2] =
{
    640, 480,
    800, 600,
    1024, 768,
    1280, 1024,
    1600, 900,
    1600, 1200,
    1680, 1050,
    1920, 1080,
    1920, 1200,
    2048, 1536,
};
#define NUM_RESNS (sizeof(g_FullscreenResolution)/sizeof(int)/2)




CDirectDrawSurfaceBase::CDirectDrawSurfaceBase()
{
    m_hWnd                  = NULL;
    m_bUseDirectDrawAccel   = USEDD_STANDARD;
    m_bFullscreen           = false;
    m_nImageBpp             = 0;
}

CDirectDrawSurfaceBase::~CDirectDrawSurfaceBase()
{
}

HRESULT CDirectDrawSurfaceBase::Create(HWND hWnd, int width, int height, bool bFullscreen)
{
    return DD_OK;
}

HRESULT CDirectDrawSurfaceBase::CreateSourceSurface(unsigned char *pBuffer,
                                                    int width, int height)
{
    return DD_OK;
}

HRESULT CDirectDrawSurfaceBase::DestroySourceSurface(unsigned char *pBuffer)
{
    return DD_OK;
}

/*
 *  This function copies a bitmap into a DirectDraw surface.
 *  If the buffer has previously been wrapped in its own DirectDraw
 *  surface with the CreateSourceSurface() method, the fast
 *  DirectDraw blit is used. If windowed mode, it goes straight
 *  to the screen, otherwise to the backbuffer.
 *  Otherwise it wraps the buffer in a GDI bitmap and uses GDI
 *  BitBlt() call to put to the backbuffer.
 */
HRESULT CDirectDrawSurfaceBase::CopyBitmap(unsigned char *pBuffer,
                                           int dx, int dy, int sx, int sy)
{
    return DD_OK;
}

/*
 * This function copies a previously loaded bitmap into a DirectDraw surface.
 * Notice that we can obtain a GDI style device context for a
 * DirectDraw surface, with which to BitBlt the bitmap into the
 * surface.
 */
HRESULT CDirectDrawSurfaceBase::CopyBitmap(HBITMAP hbm, int dx, int dy,
                                           int sx/*=0*/, int sy/*=0*/)
{
    return DD_OK;
}

HRESULT CDirectDrawSurfaceBase::CopyText(const char *pStr, 
                                     bool bOutline/*=true*/, 
                                     UINT uFormat/*=DT_NOCLIP | DT_TOP | DT_CENTER*/,
                                     COLORREF cRGBoutline/*=RGB(0,0,0)*/,
                                     COLORREF cRGB/*=RGB(255,255,255)*/,
                                     int x/*=0*/, int y/*=2*/)
{
    return DD_OK;
}

HRESULT CDirectDrawSurfaceBase::Flip()
{
    return DD_OK;
}

///////////////////////////////////////////////////////////////////////
////////////////////           DirectDraw7        /////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef USE_DIRECTDRAW7

CDirectDrawSurface::CDirectDrawSurface()
{
    m_lpDD                  = NULL;
    m_lpDDSPrimary          = NULL;
    m_lpDDSBack             = NULL;
    m_lpClipper             = NULL;
    memset(m_lpDDSSource, 0, sizeof(m_lpDDSSource));
    memset(m_lpSource,    0, sizeof(m_lpSource));

    DISPLAY_DEVICE ddd;
    memset(&ddd, 0, sizeof(ddd));
    ddd.cb = sizeof(ddd);
    EnumDisplayDevices(NULL, 0, &ddd, 0);
    m_bRemoteDesktop = (strstr(ddd.DeviceKey, "RDPDD") != NULL);
}

/*
 * Call Release on all objects we created to clean up
 */
CDirectDrawSurface::~CDirectDrawSurface()
{
    if (m_lpClipper != NULL)
    {
        m_lpClipper->Release();
        m_lpClipper = NULL;
    }
    if( m_lpDDSPrimary != NULL )
    {
        m_lpDDSPrimary->Release();
        m_lpDDSPrimary = NULL;
        if (m_bFullscreen)
            m_lpDDSBack = NULL;
    }
    for (int i = 0; i < 4; ++i)
        if( m_lpDDSSource[i] != NULL )
        {
            m_lpDDSSource[i]->PageUnlock(0);
            m_lpDDSSource[i]->Release();
            m_lpDDSSource[i] = NULL;
        }
    if (m_bFullscreen)
    {
        m_lpDD->RestoreDisplayMode();
    }
    else
    {
        if( m_lpDDSBack != NULL )
        {
            m_lpDDSBack->Release();
            m_lpDDSBack = NULL;
        }
    }
    if( m_lpDD != NULL )
    {
        m_lpDD->Release();
        m_lpDD = NULL;
    }
}

// function to initialize DirectDraw in windowed mode
HRESULT CDirectDrawSurface::Create(HWND hWnd, int width, int height,
                                   bool bFullscreen /*=false*/)
{
    HRESULT hr;
    DDSURFACEDESC2 ddsd;

    //store width, height and fullscreen mode
    m_width       = width;
    m_height      = height;
    m_nImageBpp   = GetDeviceCaps(::GetDC(NULL), BITSPIXEL);
    m_bFullscreen = bFullscreen;

    /*
     * create the main DirectDraw object
     */
    //  Specifically create a DirectDraw7 object
    hr = CoCreateInstance(CLSID_DirectDraw,
        NULL, CLSCTX_ALL, IID_IDirectDraw7, (LPVOID *)&m_lpDD);
    if(!FAILED(hr))
        hr = IDirectDraw7_Initialize(m_lpDD, NULL);
    if( hr != DD_OK )
    {
        if (m_bFullscreen)
            return hr;//  Need DD for fullscreen
        m_hWnd = hWnd;
        return DD_OK;//  fall back to GDI
    }

    if (m_bFullscreen)
        hr = m_lpDD->SetCooperativeLevel( hWnd, DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
    else
        hr = m_lpDD->SetCooperativeLevel( hWnd, DDSCL_NORMAL ); // using DDSCL_NORMAL means we will coexist with GDI
    if( hr != DD_OK )
    {
        if (m_bFullscreen)
            return hr;//  Need DD for fullscreen
        m_hWnd = hWnd;
        return DD_OK;//  fall back to GDI
    }

    if (m_bFullscreen)
    {
        //determine the fullscreen resolution
        int i;
        //  Find fullscreen resolution >= capture size
        for (i = 0; i < NUM_RESNS; ++i)
            if (width  <= g_FullscreenResolution[i][0] &&
                height <= g_FullscreenResolution[i][1])
                break;
        if (i >= NUM_RESNS)
            i = NUM_RESNS - 1;
        m_nFullscreenW = g_FullscreenResolution[i][0];
        m_nFullscreenH = g_FullscreenResolution[i][1];
        //  If can't open at full size, try smaller sizes until works
        do
        {
            hr = m_lpDD->SetDisplayMode(m_nFullscreenW, m_nFullscreenH,
                                        GetDeviceCaps(::GetDC(NULL), BITSPIXEL),
                                        0 /* refresh rate */, 0 /* flags */);
            if (hr != DD_OK && i > 0)
            {
                --i;
                m_nFullscreenW = g_FullscreenResolution[i][0];
                m_nFullscreenH = g_FullscreenResolution[i][1];
            }
        } while (hr != DD_OK && i > 0);
        if (hr != DD_OK)
            return hr;
    }

    memset( &ddsd, 0, sizeof(DDSURFACEDESC2) );
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    if (m_bFullscreen)
    {
        ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
        ddsd.dwBackBufferCount = 1;
    }
    else
    {
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    }

    // The primary surface is not a page flipping surface this time
    hr = m_lpDD->CreateSurface( &ddsd, &m_lpDDSPrimary, NULL );
    if( hr != DD_OK )
    {
        if (m_bFullscreen)
            return hr;//  Need DD for fullscreen
        m_lpDDSPrimary = NULL;
        m_hWnd = hWnd;
        return DD_OK;//  fall back to GDI
    }

    if (m_bFullscreen)
    {
        DDSCAPS2 ddsCaps;
        memset(&ddsCaps, 0, sizeof(ddsCaps));
        ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

        hr = m_lpDDSPrimary->GetAttachedSurface(&ddsCaps, &m_lpDDSBack );
        if( hr != DD_OK )
        {
            return hr;
        }
    }
    else
    {
        // Create a clipper to ensure that our drawing stays inside our window
        // setting it to our hwnd gives the clipper the coordinates from our window
        // attach the clipper to the primary surface
        if ((hr = m_lpDD->CreateClipper(0, &m_lpClipper, NULL)) != DD_OK ||
            (hr = m_lpClipper->SetHWnd(0, hWnd)               ) != DD_OK ||
            (hr = m_lpDDSPrimary->SetClipper(m_lpClipper)     ) != DD_OK)
        {
            //  Don't know why it should ever get this far then fail, but anyway
            //  fall back to GDI
            if (m_lpDDSPrimary != NULL)
            {
                m_lpDDSPrimary->Release();
                m_lpDDSPrimary = NULL;
            }
            m_hWnd = hWnd;
            return DD_OK;
        }
        UpdateBackBuffer();
    }

    //all went well, store the window handle
    m_hWnd = hWnd;

    return DD_OK;
}

void CDirectDrawSurface::UpdateBackBuffer()
{
    HRESULT hr;
    DDSURFACEDESC2 ddsd;

    if (m_bFullscreen)
        return;
    if (!m_lpDDSPrimary)
        return;

    //  Create back buffer (but no bigger than primary)
    memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(DDSURFACEDESC2);
    m_lpDDSPrimary->GetSurfaceDesc(&ddsd);
    if (m_width <= (int)ddsd.dwWidth && m_height <= (int)ddsd.dwHeight)
    {// Create a back buffer
        if (m_lpDDSBack)
        {
            //  Is it big enough?
            m_lpDDSBack->GetSurfaceDesc(&ddsd);
            if ((int)ddsd.dwWidth >= m_width && (int)ddsd.dwHeight >= m_height)
                return;//  Use what we got

            //  Delete this one and create a new one
            m_lpDDSBack->Release();
            m_lpDDSBack = NULL;
        }
        memset( &ddsd, 0, sizeof(DDSURFACEDESC2) );
        ddsd.dwSize = sizeof(DDSURFACEDESC2);
        ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        ddsd.dwWidth = m_width;
        ddsd.dwHeight = m_height;

        hr = m_lpDD->CreateSurface( &ddsd, &m_lpDDSBack, NULL );
        if (hr != DD_OK)
        {
            m_lpDDSBack = NULL;
        }
    }
    else
    {//  Free back buffer
        if (m_lpDDSBack)
        {
            m_lpDDSBack->Release();
            m_lpDDSBack = NULL;
        }
    }
}

HRESULT CDirectDrawSurface::restoreAll()
{
    HRESULT hr;

    if (!m_lpDDSPrimary)
        return DD_OK;

    // for each surface your app has created you should do the following:
    hr = m_lpDDSPrimary->Restore(); // this reattaches the video memory to the surface
    if (hr != DD_OK)
        return hr;

    if (m_lpDDSBack != NULL)
        hr = m_lpDDSBack->Restore(); // (dam) not sure if this is needed or even works

    return hr;
}

//
//  Create a DirectDraw surface around a system memory bitmap.
//  Will be used as the source surface for blit to screen.
//
HRESULT CDirectDrawSurface::CreateSourceSurface(unsigned char *pBuffer,
                                                int width, int height)
{
    HRESULT hr;
    DDSURFACEDESC2 ddsd;

    if (!m_lpDDSPrimary)
        return DD_OK;

    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LPSURFACE | DDSD_PITCH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd.lpSurface = pBuffer;
    ddsd.dwWidth = width;
    ddsd.dwHeight = height;
    //  pixelformat defaults to same as display
    switch (m_nImageBpp)
    {
    case 32:
        ddsd.lPitch = width * 4;
        break;
    case 24:
        ddsd.lPitch = (width * 3 + 3) & ~3;
        break;
    case 16:
        ddsd.lPitch = (width * 2 + 3) & ~3;
        break;
    default:
        return DDERR_INVALIDPIXELFORMAT;
    }
    int i;
    for (i = 0; i < 3; ++i)
        if (m_lpDDSSource[i] == NULL)
            break;

    hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSSource[i], NULL);
    if( hr != DD_OK )
    {
        return hr;
    }
    m_lpDDSSource[i]->PageLock(0); //  Make memory unswappable so DMA can be used
    m_lpSource[i] = pBuffer; //  To keep track of which surface goes with which buffer
    return DD_OK;
}

HRESULT CDirectDrawSurface::DestroySourceSurface(unsigned char *pBuffer)
{
    int i;
    for (i = 0; i < 4; ++i)
    {
        if (m_lpDDSSource[i] == NULL)
            continue;
        if (m_lpSource[i] == pBuffer)
            break;
    }
    if (i > 3)
        return DD_OK;
    m_lpDDSSource[i]->PageUnlock(0);
    m_lpDDSSource[i]->Release();
    m_lpDDSSource[i] = NULL;
    return DD_OK;
}

HRESULT CDirectDrawSurface::CopyBitmap(unsigned char *pBuffer,
                                       int dx, int dy, int sx, int sy)
{
    return CopyBitmap(pBuffer, NULL, dx, dy, sx, sy);
}
HRESULT CDirectDrawSurface::CopyBitmap(HBITMAP hbm, int dx, int dy,
                                       int sx/*=0*/, int sy/*=0*/)
{
    return CopyBitmap(NULL, hbm, dx, dy, sx, sy);
}
HRESULT CDirectDrawSurface::CopyBitmap(unsigned char *pBuffer,
                                       HBITMAP hbm,
                                       int dx, int dy, int sx, int sy)
{
    LPDIRECTDRAWSURFACE7    pDDSSource  = NULL;
    LPDIRECTDRAWSURFACE7    pDDSDest    = NULL;
    HRESULT                 hr          = DD_OK;
    POINT                   p           = {0, 0};
    HDC                     hdc         = NULL;
    HDC                     hdcImage    = NULL;
    HGDIOBJ                 old         = NULL;
    DDBLTFX                 ddbltfx;
    RECT                    rcSrc;
    RECT                    rcDest;

    if (m_hWnd == NULL)
        return (!DD_OK); //not initialized

    m_width = dx;
    m_height = dy;

    //  Source DD surface (if any)
    for (int i = 0; pBuffer && i < 4; ++i)
    {
        if (m_lpDDSSource[i] == NULL)
            continue;
        if (m_lpSource[i] == pBuffer)
        {
            pDDSSource = m_lpDDSSource[i];
            break;
        }
    }

    //  Figure out what method to use.
    //  Depends on Fullscreen mode, user-requested DD Use mode,
    //  existence of Back surface, and existence of DD source surface
    //  wrapped around system memory buffers
    //GDI_TO_WINDOW_DC              GDI BitBlt to window DC (nop in Flip())
    //GDI_TO_PRIMARY                GDI BitBlt to DD Primary (nop in Flip())
    //GDI_TO_BACK__DD_TO_PRIMARY    GDI BitBlt to DD Back (DD Blt to Primary in Flip())
    //GDI_TO_BACK__FLIP             GDI BitBlt to DD Back (DD Flip in Flip())
    //DD_TO_BACK__FLIP              DD Blt to DD Back (DD Flip in Flip())
    //DD_TO_PRIMARY                 DD Blt to DD Primary (nop in Flip())
    if (m_bFullscreen)
    {
        switch (m_bUseDirectDrawAccel)
        {
        case USEDD_NONE:
            //  Can't do it, fall to Standard
        case USEDD_STANDARD:
            m_method = m_lpDDSBack ? GDI_TO_BACK__FLIP : GDI_TO_PRIMARY;
            break;
        case USEDD_ACCELERATED:
            if (pDDSSource && m_lpDDSBack)
                m_method = DD_TO_BACK__FLIP;
            else if (pDDSSource)
                m_method = DD_TO_PRIMARY;
            else
                m_method = GDI_TO_PRIMARY;
            break;
        }
    }
    else
    {
        switch (m_bUseDirectDrawAccel)
        {
        case USEDD_NONE:
            m_method = GDI_TO_WINDOW_DC;
            break;
        case USEDD_STANDARD:
            UpdateBackBuffer();
            m_method = m_lpDDSBack ? GDI_TO_BACK__DD_TO_PRIMARY : GDI_TO_WINDOW_DC;
            break;
        case USEDD_ACCELERATED:
            UpdateBackBuffer();
            if (pDDSSource)
                m_method = DD_TO_PRIMARY;
            else if (m_lpDDSBack)
                m_method = GDI_TO_BACK__DD_TO_PRIMARY;
            else
                m_method = GDI_TO_WINDOW_DC;
            break;
        }
        if (m_bRemoteDesktop)
            m_method = GDI_TO_WINDOW_DC; //  only mode with no problems under Remote Desktop
    }

    //
    //  Set Up
    //
    if (sx == 0 && sy == 0)
    {
        sx = dx;
        sy = dy;
    }
    rcSrc.left = 0;
    rcSrc.top = 0;
    rcSrc.right = sx;
    rcSrc.bottom = sy;
    rcDest.left = 0;
    rcDest.top = 0;
    rcDest.right = dx;
    rcDest.bottom = dy;
    int bitmap_width = sx;

    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
        if (pBuffer)
            hbm = CreateBitmap(bitmap_width, sy, 1, m_nImageBpp, pBuffer);
        hdc = GetDC(m_hWnd);
        hdcImage = CreateCompatibleDC(hdc);
        old = SelectObject(hdcImage, hbm);
        break;

    case GDI_TO_PRIMARY:               // GDI BitBlt to DD Primary (nop in Flip())
    case GDI_TO_BACK__DD_TO_PRIMARY:   // GDI BitBlt to DD Back (DD Blt to Primary in Flip())
    case GDI_TO_BACK__FLIP:            // GDI BitBlt to DD Back (DD Flip in Flip())
        if (pBuffer)
            hbm = CreateBitmap(bitmap_width, sy, 1, m_nImageBpp, pBuffer);
        pDDSDest = (m_method == GDI_TO_PRIMARY) ? m_lpDDSPrimary : m_lpDDSBack;
        hr = pDDSDest->GetDC(&hdc);
        if (hr == DD_OK)
        {
            hdcImage = CreateCompatibleDC(hdc);
            old = SelectObject(hdcImage, hbm);
        }
        break;

    case DD_TO_PRIMARY:                // DD Blt to DD Primary (nop in Flip())
        pDDSDest = m_lpDDSPrimary;
        break;

    case DD_TO_BACK__FLIP:             // DD Blt to DD Back (DD Flip in Flip())
        pDDSDest = m_lpDDSBack;
        break;
    }


    //
    //  Blt
    //
    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
    case GDI_TO_PRIMARY:               // GDI BitBlt to DD Primary (nop in Flip())
    case GDI_TO_BACK__DD_TO_PRIMARY:   // GDI BitBlt to DD Back (DD Blt to Primary in Flip())
    case GDI_TO_BACK__FLIP:            // GDI BitBlt to DD Back (DD Flip in Flip())
        if (m_bFullscreen && (dx != m_nFullscreenW || dy != m_nFullscreenH))
        {//  Black border, offset image in destination
            if (dx > m_nFullscreenW || dy > m_nFullscreenH)
            {//  Shrink image to screen size
                double shrink = min((double)m_nFullscreenW / dx,
                                    (double)m_nFullscreenH / dy);
                dx = min((int)floor(dx * shrink + .5), m_nFullscreenW);
                dy = min((int)floor(dy * shrink + .5), m_nFullscreenH);
                rcDest.right = dx;
                rcDest.bottom = dy;
            }
            if (dx < m_nFullscreenW || dy < m_nFullscreenH)
                BitBlt(hdc, 0, 0, m_nFullscreenW, m_nFullscreenH, hdcImage, 0, 0, BLACKNESS); //???
            OffsetRect(&rcDest, (m_nFullscreenW - dx) / 2, (m_nFullscreenH - dy) / 2);
        }
        if (sx == dx && sy == dy)
            BitBlt(hdc, rcDest.left, rcDest.top, dx, dy, hdcImage, 0, 0, SRCCOPY);
        else
        {
            if (dx < sx || dy < sy)
                SetStretchBltMode(hdc, HALFTONE);
            StretchBlt(hdc, rcDest.left, rcDest.top, dx, dy, hdcImage, 0, 0, sx, sy, SRCCOPY);
        }
        break;

    case DD_TO_PRIMARY:                // DD Blt to DD Primary (nop in Flip())
    case DD_TO_BACK__FLIP:             // DD Blt to DD Back (DD Flip in Flip())
        if (hr != DD_OK)
            break;
        memset(&ddbltfx, 0, sizeof(ddbltfx));
        ddbltfx.dwSize = sizeof(ddbltfx);
        if (m_method == DD_TO_PRIMARY)
            ddbltfx.dwDDFX = DDBLTFX_NOTEARING;
        if (m_bFullscreen && (dx != m_nFullscreenW || dy != m_nFullscreenH))
        {
            if (dx > m_nFullscreenW || dy > m_nFullscreenH)
            {
#if 1
                //  Shrink image to screen size
                double shrink = min((double)m_nFullscreenW / dx,
                                    (double)m_nFullscreenH / dy);
                dx = min((int)floor(dx * shrink + .5), m_nFullscreenW);
                dy = min((int)floor(dy * shrink + .5), m_nFullscreenH);
#else
                //  Crop source image to screen size
                if (dx > m_nFullscreenW)
                {
                    sx = m_nFullscreenW;
                    dx = m_nFullscreenW;
                }
                if (dy > m_nFullscreenH)
                {
                    sy = m_nFullscreenH;
                    dy = m_nFullscreenH;
                }
                OffsetRect(&rcSrc, (m_nFullscreenW - sx) / 2, (m_nFullscreenH - sy) / 2);
#endif
                rcDest.right = dx;
                rcDest.bottom = dy;
            }
            if (dx < m_nFullscreenW || dy < m_nFullscreenH)
            {
                ddbltfx.dwFillColor = 0;
                pDDSDest->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
            }
            OffsetRect(&rcDest, (m_nFullscreenW - dx) / 2, (m_nFullscreenH - dy) / 2);
        }
        if (!m_bFullscreen)
        {//  only DD_TO_PRIMARY/windowed case gets here
            GetClientRect(m_hWnd, &rcDest);
            ClientToScreen(m_hWnd, &p);
            OffsetRect(&rcDest, p.x, p.y);
        }
        hr = pDDSDest->Blt(&rcDest, pDDSSource, &rcSrc, /*DDBLT_WAIT |*/ DDBLT_DDFX, &ddbltfx);
        break;
    }


    //
    //  Clean Up
    //
    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
        SelectObject(hdcImage, old);
        DeleteDC(hdcImage);
        ReleaseDC(m_hWnd, hdc);
        if (pBuffer && hbm)
            DeleteObject(hbm);
        break;

    case GDI_TO_PRIMARY:               // GDI BitBlt to DD Primary (nop in Flip())
    case GDI_TO_BACK__DD_TO_PRIMARY:   // GDI BitBlt to DD Back (DD Blt to Primary in Flip())
    case GDI_TO_BACK__FLIP:            // GDI BitBlt to DD Back (DD Flip in Flip())
        if (old)
            SelectObject(hdcImage, old);
        if (hdcImage)
            DeleteDC(hdcImage);
        if (hdc)
            pDDSDest->ReleaseDC(hdc);
        if (pBuffer && hbm)
            DeleteObject(hbm);
        break;

    case DD_TO_PRIMARY:                // DD Blt to DD Primary (nop in Flip())
        break;

    case DD_TO_BACK__FLIP:             // DD Blt to DD Back (DD Flip in Flip())
        break;
    }

    return hr;
}

HRESULT CDirectDrawSurface::CopyText(const char *pStr, 
                                     bool bOutline/*=true*/, 
                                     UINT uFormat/*=DT_NOCLIP | DT_TOP | DT_CENTER*/,
                                     COLORREF cRGBoutline/*=RGB(0,0,0)*/,
                                     COLORREF cRGB/*=RGB(255,255,255)*/,
                                     int x/*=0*/, int y/*=2*/)
{
    LPDIRECTDRAWSURFACE7    pDDSDest= NULL;
    HDC                     hdc     = NULL;
    HRESULT                 hr      = DD_OK;
    RECT                    rect    = { x, y, m_width /* - x*/, 0};

    if (m_hWnd == NULL) 
        return (!DD_OK); //not initialized

    //  Set Up
    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
    case GDI_TO_PRIMARY:               // GDI BitBlt to DD Primary (nop in Flip())
    case DD_TO_PRIMARY:                // DD Blt to DD Primary (nop in Flip())
        hdc = GetDC(m_hWnd);
        break;

    case GDI_TO_BACK__DD_TO_PRIMARY:   // GDI BitBlt to DD Back (DD Blt to Primary in Flip())
    case GDI_TO_BACK__FLIP:            // GDI BitBlt to DD Back (DD Flip in Flip())
    case DD_TO_BACK__FLIP:             // DD Blt to DD Back (DD Flip in Flip())
        pDDSDest = m_lpDDSBack;
        hr = pDDSDest->GetDC(&hdc);
        break;
    }
    if (hr != DD_OK)
        return hr;

    if (m_bFullscreen)
        rect.right = m_nFullscreenW - x;

    //
    //  Draw Text
    //
    SetBkMode(hdc, TRANSPARENT);
    if (bOutline)
    {
        SetTextColor(hdc, cRGBoutline);
        DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);
        OffsetRect(&rect, 2, 0);
        DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);
        OffsetRect(&rect, 0, 2);
        DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);
        OffsetRect(&rect, -2, 0);
        DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);
        OffsetRect(&rect, 1, -1);
    }
    SetTextColor(hdc, cRGB); //RGB( 255, 255, 255))
    DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);

    //  Clean Up
    if (pDDSDest)
        pDDSDest->ReleaseDC(hdc);
    else
        ReleaseDC(m_hWnd, hdc);

    return hr;
}

/*
*   Puts the image to the screen (if it's not already)
*/
HRESULT CDirectDrawSurface::Flip()
{
    HRESULT hr          = DD_OK;
    RECT    rcRectSrc   = {0, 0, m_width, m_height};
    RECT    rcRectDest;
    POINT   p           = {0, 0};

    if (m_hWnd == NULL) 
        return (!DD_OK); //not initialized

    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
    case GDI_TO_PRIMARY:               // GDI BitBlt to DD Primary (nop in Flip())
    case DD_TO_PRIMARY:                // DD Blt to DD Primary (nop in Flip())
        // do nothing here in these cases
        return DD_OK;

    case GDI_TO_BACK__FLIP:            // GDI BitBlt to DD Back (DD Flip in Flip())
    case DD_TO_BACK__FLIP:             // DD Blt to DD Back (DD Flip in Flip())
        hr = m_lpDDSPrimary->Flip(NULL, DDFLIP_WAIT);
        break;

    case GDI_TO_BACK__DD_TO_PRIMARY:   // GDI BitBlt to DD Back (DD Blt to Primary in Flip())
        GetClientRect(m_hWnd, &rcRectDest);
        ClientToScreen(m_hWnd, &p);
        OffsetRect(&rcRectDest, p.x, p.y);
        //hr = m_lpDDSPrimary->Blt(&rcRectDest, m_lpDDSBack, &rcRectSrc, DDBLT_ASYNC, NULL);
        hr = m_lpDDSPrimary->Blt(&rcRectDest, m_lpDDSBack, &rcRectSrc, DDBLT_WAIT, NULL);
        break;
    }

    if (hr == DDERR_SURFACELOST)
    {
        hr = restoreAll();
    }

    return hr;
}

#endif /// USE_DIRECTDRAW7



///////////////////////////////////////////////////////////////////////
////////////////////            DirectX9          /////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef USE_DIRECTX9

#pragma comment(lib, "d3d9.lib")

CDirectDrawSurface::CDirectDrawSurface()
{
    m_pD3D          = NULL;
    m_pd3dDevice    = NULL;
    m_pDDSBack      = NULL;
    m_width         = 0;
    m_height        = 0;
    m_nBackbufferW  = 0;
    m_nBackbufferH  = 0;
    m_pDDSMono      = NULL;
    m_pDDSStereo    = NULL;
    memset(m_pDDSSource, 0, sizeof(m_pDDSSource));
    memset(m_pSource,    0, sizeof(m_pSource));

    DISPLAY_DEVICE ddd;
    memset(&ddd, 0, sizeof(ddd));
    ddd.cb = sizeof(ddd);
    EnumDisplayDevices(NULL, 0, &ddd, 0);
    m_bRemoteDesktop = (strstr(ddd.DeviceKey, "RDPDD") != NULL);
}

/*
 * Call Release on all objects we created to clean up
 */
CDirectDrawSurface::~CDirectDrawSurface()
{
    Destroy();
}

void CDirectDrawSurface::Destroy()
{
    if (m_bFullscreen)
    {
    }
    if (m_pDDSMono != NULL)
    {
        m_pDDSMono->Release();
        m_pDDSMono = NULL;
    }
    if (m_pDDSStereo != NULL)
    {
        m_pDDSStereo->Release();
        m_pDDSStereo = NULL;
    }
    if (m_pDDSBack != NULL)
    {
        m_pDDSBack->Release();
        m_pDDSBack = NULL;
    }
    if (m_pd3dDevice != NULL)
    {
        m_pd3dDevice->Release();
        m_pd3dDevice = NULL;
    }
    if (m_pD3D != NULL)
    {
        m_pD3D->Release();
        m_pD3D = NULL;
    }
    m_hWnd = NULL;
    m_width = 0;
    m_height = 0;
    m_nBackbufferW = 0;
    m_nBackbufferH = 0;
}

HRESULT CDirectDrawSurface::Create(HWND hWnd, int width, int height,
                                   bool bFullscreen /*=false*/,
                                   bool bStereo /*=false*/)
{
    if (bStereo)
        width = width / 2;

    if (hWnd        == m_hWnd   &&
        width       == m_width  &&
        height      == m_height &&
        bFullscreen == m_bFullscreen)
        return D3D_OK; // already created

    Destroy();

    //  Store width, height and fullscreen mode
    m_width       = width;
    m_height      = height;
    m_bFullscreen = bFullscreen;

    /*
     * create the main DirectDraw object
     */
    if (m_pD3D == NULL)
        m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (m_pD3D == NULL)
    {
        if (m_bFullscreen)
            return E_FAIL;//  Need DD for fullscreen
        m_hWnd = hWnd;
        return D3D_OK;//  fall back to GDI
    }

    //  Determine the fullscreen resolution
    HDC hdc = GetDC(hWnd);
    m_nFullscreenW = GetDeviceCaps(hdc, HORZRES);
    m_nFullscreenH = GetDeviceCaps(hdc, VERTRES);
    m_nImageBpp    = GetDeviceCaps(hdc, BITSPIXEL);
    ReleaseDC(hWnd, hdc);

    D3DPRESENT_PARAMETERS d3dpp;
    memset(&d3dpp, 0, sizeof(d3dpp));
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //throw old frames away
    d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    d3dpp.hDeviceWindow = hWnd;

    if (m_bFullscreen)
    {
        m_nImageBpp = 32;
        d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
        m_nBackbufferW = m_nFullscreenW;
        m_nBackbufferH = m_nFullscreenH;
        d3dpp.Windowed = FALSE;
    }
    else
    {
        d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
        m_nBackbufferW = min(width, m_nFullscreenW);
        m_nBackbufferH = min(height, m_nFullscreenH);
        d3dpp.Windowed = TRUE;
    }
    d3dpp.BackBufferWidth =  m_nBackbufferW;
    d3dpp.BackBufferHeight = m_nBackbufferH;

    int ret = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
                                   D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pd3dDevice);
    if (ret != D3D_OK)
    {
        if (m_bFullscreen)
            return E_FAIL;//  Need DD for fullscreen
        m_hWnd = hWnd;
        return D3D_OK;//  fall back to GDI
    }

    //  All went well, store the window handle
    m_hWnd = hWnd;

    return D3D_OK;
}

bool CDirectDrawSurface::StereoScreen()
{
    return (m_pDDSStereo != NULL);
}

void CDirectDrawSurface::UpdateBackBuffer()
{
    HRESULT hr = D3D_OK;

    if (m_pd3dDevice == NULL)
        return;

    if (m_bFullscreen ||
        (m_width <= m_nBackbufferW && m_height <= m_nBackbufferH))
    {//  Get back buffer
        if (m_pDDSBack)
            return;
        hr = m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pDDSBack);
        if (hr != D3D_OK)
            m_pDDSBack = NULL;
    }
    else
    {//  Free back buffer
        if (m_pDDSBack)
        {
            m_pDDSBack->Release();
            m_pDDSBack = NULL;
        }
    }
}

HRESULT CDirectDrawSurface::restoreAll()
{
    HRESULT hr = D3D_OK;

    return hr;
}

//
//  Create a DirectDraw surface around a system memory bitmap.
//  Will be used as the source surface for blit to screen.
//
HRESULT CDirectDrawSurface::CreateSourceSurface(unsigned char *pBuffer,
                                                int width, int height)
{
    switch (m_bUseDirectDrawAccel)
    {
    case USEDD_STANDARD:
    case USEDD_NONE:
        return D3D_OK;
    case USEDD_ACCELERATED:
        break;
    }

    return DD_OK;
}

HRESULT CDirectDrawSurface::DestroySourceSurface(unsigned char *pBuffer)
{
    return D3D_OK;
}


HRESULT CDirectDrawSurface::CopyBitmap(unsigned char *pBuffer,
                                       int dx, int dy, int sx, int sy)
{
    return CopyBitmap(pBuffer, NULL, dx, dy, sx, sy);
}
HRESULT CDirectDrawSurface::CopyBitmap(HBITMAP hbm, int dx, int dy,
                                       int sx/*=0*/, int sy/*=0*/)
{
    return CopyBitmap(NULL, hbm, dx, dy, sx, sy);
}
HRESULT CDirectDrawSurface::CopyBitmap(unsigned char *pBuffer,
                                       HBITMAP hbm,
                                       int dx, int dy, int sx, int sy)
{
    HRESULT                 hr          = D3D_OK;
    POINT                   p           = {0, 0};
    HDC                     hdc         = NULL;
    HDC                     hdcImage    = NULL;
    HGDIOBJ                 old         = NULL;
    IDirect3DSurface9 *     pDDSSource  = NULL;
    RECT                    rcSrc;
    RECT                    rcDest;

    //  re-create if needed
    Create(m_hWnd, dx, dy, m_bFullscreen);

    if (m_hWnd == NULL)
        return E_FAIL; //not initialized

    m_width = dx;
    m_height = dy;

    //  Source D3D surface (if any)
    for (int i = 0; pBuffer && i < 4; ++i)
    {
        if (m_pDDSSource[i] == NULL)
            continue;
        if (m_pSource[i] == pBuffer)
        {
            pDDSSource = m_pDDSSource[i];
            break;
        }
    }
    //  If there are no source surfaces, create one and copy the data to it
    if (m_pd3dDevice &&
        pBuffer &&
        m_pDDSStereo == NULL &&
        m_bUseDirectDrawAccel != USEDD_NONE &&
        pDDSSource == NULL &&
        m_pDDSMono == NULL)
    {
        //  1:1 temp buffer in GPU memory
        HRESULT ret = m_pd3dDevice->CreateOffscreenPlainSurface(sx, sy,
                            D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pDDSMono, NULL);
        if (ret != D3D_OK)
        {
            if (m_pDDSMono != NULL)
            {
                m_pDDSMono->Release();
                m_pDDSMono = NULL;
            }
        }
    }
    if (pDDSSource == NULL)
        pDDSSource = m_pDDSMono;

    //  Figure out what method to use.
    //  Depends on Fullscreen mode, user-requested DD Use mode,
    //  existence of Back surface, and existence of DD source surface
    //  wrapped around system memory buffers
    //GDI_TO_WINDOW_DC              GDI BitBlt to window DC (nop in Flip())
    //GDI_TO_BACK__FLIP             GDI BitBlt to D3D Back (D3D Present in Flip())
    //D3D_TO_BACK__FLIP             D3D render to D3D Back (D3D Present in Flip())
    //CPY_TO_STEREO_BACK__FLIP      Memcpy to D3D Stereo buffer, D3D StretchRect to D3D Back (D3D Present in Flip())
    if (m_bFullscreen)
    {
        switch (m_bUseDirectDrawAccel)
        {
        case USEDD_NONE:
            //  Can't do it, fall to Standard
        case USEDD_STANDARD:
            UpdateBackBuffer();
            if (m_pDDSStereo && m_pDDSBack)
                m_method = CPY_TO_STEREO_BACK__FLIP;
            else if (pDDSSource && m_pDDSBack)
                m_method = D3D_TO_BACK__FLIP;
            else if (m_pDDSBack)
                m_method = GDI_TO_BACK__FLIP;
            else
                m_method = GDI_TO_WINDOW_DC;
            break;
        case USEDD_ACCELERATED:
            UpdateBackBuffer();
            if (m_pDDSStereo && m_pDDSBack)
                m_method = CPY_TO_STEREO_BACK__FLIP;
            else if (pDDSSource && m_pDDSBack)
                m_method = D3D_TO_BACK__FLIP;
            else if (m_pDDSBack)
                m_method = GDI_TO_BACK__FLIP;
            else
                m_method = GDI_TO_WINDOW_DC;
            break;
        }
    }
    else
    {
        switch (m_bUseDirectDrawAccel)
        {
        case USEDD_NONE:
            m_method = GDI_TO_WINDOW_DC;
            break;
        case USEDD_STANDARD:
            UpdateBackBuffer();
            if (pDDSSource && m_pDDSBack)
                m_method = D3D_TO_BACK__FLIP;
            else if (m_pDDSBack)
                m_method = GDI_TO_BACK__FLIP;
            else
                m_method = GDI_TO_WINDOW_DC;
            break;
        case USEDD_ACCELERATED:
            UpdateBackBuffer();
            if (pDDSSource && m_pDDSBack)
                m_method = D3D_TO_BACK__FLIP;
            else if (m_pDDSBack)
                m_method = GDI_TO_BACK__FLIP;
            else
                m_method = GDI_TO_WINDOW_DC;
            break;
        }
        if (m_bRemoteDesktop)
            m_method = GDI_TO_WINDOW_DC; //  only mode with no problems under Remote Desktop
    }


    //
    //  Set Up
    //
    if (sx == 0 && sy == 0)
    {
        sx = dx;
        sy = dy;
    }
    rcSrc.left = 0;
    rcSrc.top = 0;
    rcSrc.right = sx;
    rcSrc.bottom = sy;
    rcDest.left = 0;
    rcDest.top = 0;
    rcDest.right = dx;
    rcDest.bottom = dy;
    int bitmap_width = sx;
    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
    case GDI_TO_BACK__FLIP:            // GDI BitBlt to D3D Back (D3D Present in Flip())
    case D3D_TO_BACK__FLIP:            // D3D StretchRect to D3D Back (D3D Present in Flip())
    case CPY_TO_STEREO_BACK__FLIP:     // Memcpy to D3D Stereo buffer, D3D StretchRect to D3D Back (D3D Present in Flip())
        if (m_bFullscreen && (dx != m_nFullscreenW || dy != m_nFullscreenH))
        {//  Black border, offset image in destination
            //  Resize image to screen size, keep aspect ratio
            double shrink = min((double)m_nFullscreenW / dx,
                                (double)m_nFullscreenH / dy);

            dx = min((int)floor(dx * shrink + .5), m_nFullscreenW);
            dy = min((int)floor(dy * shrink + .5), m_nFullscreenH);
            rcDest.right = dx;
            rcDest.bottom = dy;
            OffsetRect(&rcDest, (m_nFullscreenW - dx) / 2, (m_nFullscreenH - dy) / 2);
        }
        if (m_bFullscreen && (dx < m_nFullscreenW || dy < m_nFullscreenH))
        {
            if (m_pDDSBack)
                m_pd3dDevice->ColorFill(m_pDDSBack, NULL, 0);
        }
        break;
    }

    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
        if (pBuffer)
            hbm = CreateBitmap(bitmap_width, sy, 1, m_nImageBpp, pBuffer);
        hdc = GetDC(m_hWnd);
        hdcImage = CreateCompatibleDC(hdc);
        old = SelectObject(hdcImage, hbm);
        break;

    case GDI_TO_BACK__FLIP:            // GDI BitBlt to D3D Back (D3D Flip in Flip())
        if (pBuffer)
            hbm = CreateBitmap(bitmap_width, sy, 1, m_nImageBpp, pBuffer);
        hr = m_pDDSBack->GetDC(&hdc);
        hdcImage = CreateCompatibleDC(hdc);
        old = SelectObject(hdcImage, hbm);
        break;

    case D3D_TO_BACK__FLIP:            // D3D StretchRect to D3D Back (D3D Present in Flip())
        //  If pDDSSource is not a wrapper for pBuffer then need to copy
        //  the data from pBuffer to pDDSSource
        if (pBuffer && pDDSSource == m_pDDSMono)
        {
            D3DLOCKED_RECT rect;
            hr = m_pDDSMono->LockRect(&rect, NULL, 0);
            if (hr == D3D_OK)
            {
                int nStride = (bitmap_width << 2);
                size_t cpysize = min(abs(rect.Pitch), (sx << 4));
                for (int y = 0; y < sy; ++y)
                {
                    memcpy((unsigned char *)rect.pBits + y * rect.Pitch,
                           pBuffer + y * nStride, cpysize);
                }
                m_pDDSMono->UnlockRect();
            }
        }
        break;

    case CPY_TO_STEREO_BACK__FLIP:     // Memcpy to D3D Stereo buffer, D3D StretchRect to D3D Back (D3D Present in Flip())
        if (pBuffer)
        {
            D3DLOCKED_RECT rect;
            LPNVSTEREOIMAGEHEADER psih;
            hr = m_pDDSStereo->LockRect(&rect, NULL, 0);
            if (hr == D3D_OK)
            {
                int nStride = (bitmap_width << 2);
                for (int y = 0; y < m_height; ++y)
                {
                    memcpy((unsigned char *)rect.pBits + y * rect.Pitch,
                           pBuffer + y * nStride, nStride);
                }
                psih = (LPNVSTEREOIMAGEHEADER)(((unsigned char *)rect.pBits 
                     +  (rect.Pitch * m_height)));
                psih->dwSignature = NVSTEREO_IMAGE_SIGNATURE;
                psih->dwBPP = 32;
                psih->dwFlags = 0; // other option: SIH_SWAP_EYES
                psih->dwWidth = m_width * 2;
                psih->dwHeight = m_height;
                m_pDDSStereo->UnlockRect();
            }
        }
        else
        {
        }
        break;
    }


    //
    //  Render (to screen or into back buffer)
    //
    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
    case GDI_TO_BACK__FLIP:            // GDI BitBlt to D3D Back (D3D Flip in Flip())
        if (sx == dx && sy == dy)
        {
            BitBlt(hdc, rcDest.left, rcDest.top, dx, dy, hdcImage, 0, 0, SRCCOPY);
        }
        else
        {
            if (m_bFullscreen || (dx < sx || dy < sy))
                SetStretchBltMode(hdc, HALFTONE);
            StretchBlt(hdc, rcDest.left, rcDest.top, dx, dy, hdcImage, 0, 0, sx, sy, SRCCOPY);
        }
        break;

    case D3D_TO_BACK__FLIP:            // D3D StretchRect to D3D Back (D3D Present in Flip())
        m_pd3dDevice->StretchRect(pDDSSource, &rcSrc, m_pDDSBack, &rcDest, D3DTEXF_LINEAR);
        break;

    case CPY_TO_STEREO_BACK__FLIP:     // Memcpy to D3D Stereo buffer, D3D StretchRect to D3D Back (D3D Present in Flip())
        rcSrc.right = sx * 2;
        m_pd3dDevice->StretchRect(m_pDDSStereo, &rcSrc, m_pDDSBack, &rcDest, D3DTEXF_LINEAR);
        break;
    }


    //
    //  Clean Up
    //
    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
        SelectObject(hdcImage, old);
        DeleteDC(hdcImage);
        ReleaseDC(m_hWnd, hdc);
        if (pBuffer && hbm)
            DeleteObject(hbm);
        break;

    case GDI_TO_BACK__FLIP:            // GDI BitBlt to D3D Back (D3D Flip in Flip())
        if (old)
            SelectObject(hdcImage, old);
        if (hdcImage)
            DeleteDC(hdcImage);
        if (hdc)
            m_pDDSBack->ReleaseDC(hdc);
        if (pBuffer && hbm)
            DeleteObject(hbm);
        break;

    case D3D_TO_BACK__FLIP:            // D3D render to D3D Back (D3D Flip in Flip())
    case CPY_TO_STEREO_BACK__FLIP:     // Memcpy to D3D Stereo buffer, D3D StretchRect to D3D Back (D3D Present in Flip())
        break;
    }

    return hr;
}

HRESULT CDirectDrawSurface::CopyText(const char *pStr, 
                                     bool bOutline/*=true*/, 
                                     UINT uFormat/*=DT_NOCLIP | DT_TOP | DT_CENTER*/,
                                     COLORREF cRGBoutline/*=RGB(0,0,0)*/,
                                     COLORREF cRGB/*=RGB(255,255,255)*/,
                                     int x/*=0*/, int y/*=2*/)
{
    HDC                     hdc     = NULL;
    HRESULT                 hr      = D3D_OK;
    RECT                    rect    = { x, y, m_width /* - x*/, 0};

    if (m_hWnd == NULL) 
        return (!D3D_OK); //not initialized

    //  Set Up
    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
        hdc = GetDC(m_hWnd);
        break;

    case GDI_TO_BACK__FLIP:            // GDI BitBlt to D3D Back (D3D Present in Flip())
    case D3D_TO_BACK__FLIP:            // D3D StretchRect to D3D Back (D3D Present in Flip())
    case CPY_TO_STEREO_BACK__FLIP:     // Memcpy to D3D Stereo buffer, D3D StretchRect to D3D Back (D3D Present in Flip())
        if (m_pDDSBack == NULL)
            return E_FAIL;
        hr = m_pDDSBack->GetDC(&hdc);
        break;
    }
    if (hr != D3D_OK)
        return hr;

    if (m_bFullscreen)
        rect.right = m_nFullscreenW - x;

    //
    //  Draw Text
    //
    SetBkMode(hdc, TRANSPARENT);
    if (bOutline)
    {
        SetTextColor(hdc, cRGBoutline);
        DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);
        OffsetRect(&rect, 2, 0);
        DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);
        OffsetRect(&rect, 0, 2);
        DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);
        OffsetRect(&rect, -2, 0);
        DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);
        OffsetRect(&rect, 1, -1);
    }
    SetTextColor(hdc, cRGB); //RGB( 255, 255, 255))
    DrawText(hdc, pStr, (int)strlen(pStr), &rect, uFormat);

    //  Clean Up
    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
        ReleaseDC(m_hWnd, hdc);
        break;

    case GDI_TO_BACK__FLIP:            // GDI BitBlt to D3D Back (D3D Present in Flip())
    case D3D_TO_BACK__FLIP:            // D3D StretchRect to D3D Back (D3D Present in Flip())
    case CPY_TO_STEREO_BACK__FLIP:     // Memcpy to D3D Stereo buffer, D3D StretchRect to D3D Back (D3D Present in Flip())
        if (m_pDDSBack)
            m_pDDSBack->ReleaseDC(hdc);
        break;
    }

    return hr;
}

/*
*   Puts the image to the screen (if it's not already)
*/
HRESULT CDirectDrawSurface::Flip()
{
    HRESULT hr          = DD_OK;
    RECT    rcRectSrc   = {0, 0, m_width, m_height};
    POINT   p           = {0, 0};

    if (m_hWnd == NULL) 
        return (!DD_OK); //not initialized

    switch (m_method)
    {
    case GDI_TO_WINDOW_DC:             // GDI BitBlt to window DC (nop in Flip())
        // do nothing here
        return DD_OK;

    case GDI_TO_BACK__FLIP:            // GDI BitBlt to D3D Back (D3D Present in Flip())
    case D3D_TO_BACK__FLIP:            // D3D StretchRect to D3D Back (D3D Present in Flip())
    case CPY_TO_STEREO_BACK__FLIP:     // Memcpy to D3D Stereo buffer, D3D StretchRect to D3D Back (D3D Present in Flip())
        if (m_pDDSBack)
            m_pDDSBack->Release();
        m_pDDSBack = NULL;
        m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        break;
    }

    return hr;
}

#endif

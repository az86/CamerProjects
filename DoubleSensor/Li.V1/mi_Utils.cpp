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
//      without warranty of any kind.   APTINA IMAGING CORPORATION EXPRESSLY DISCLAIMS         
//      ALL WARRANTIES EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO, NONINFRINGEMENT       
//      OF THIRD PARTY RIGHTS, AND ANY IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS        
//      FOR A PARTICULAR PURPOSE.  APTINA DOES NOT WARRANT THAT THE FUNCTIONS CONTAINED        
//      IN THIS SOFTWARE WILL MEET YOUR REQUIREMENTS, OR THAT THE OPERATION OF THIS SOFTWARE   
//      WILL BE UNINTERRUPTED OR ERROR-FREE.  FURTHERMORE, APTINA DOES NOT WARRANT OR          
//      MAKE ANY REPRESENTATIONS REGARDING THE USE OR THE RESULTS OF THE USE OF ANY            
//      ACCOMPANYING DOCUMENTATION IN TERMS OF ITS CORRECTNESS, ACCURACY, RELIABILITY,         
//      OR OTHERWISE.                                                                          
//*************************************************************************************/       
// mi_Utils.cpp : implementation file
//

#include <assert.h>
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif
#include <afxwin.h>     //for use of AfxMessageBox

#include "stdafx.h"
#include "mi_Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// 
// some usefull function

CString Int2Str(int value)
{
    CString str;
    str.Format("%d", value);
    return str;
}

CString Flt2Str(float value, int width/*=4*/, int precision/*=1*/)
{
    CString fstr;
    fstr.Format("%%%d.%df", width, precision);
    CString str;
    str.Format(fstr, value);
    return str;
}

CString FormatStr(LPCTSTR lpszFormat, ...)
{
    va_list arg;
    va_start( arg, lpszFormat); /* Initialize variable arguments. */
    
    char str[1024];
    vsprintf_s(str, lpszFormat, arg);
    
    va_end( arg );              /* Reset variable arguments.      */

    return str;
}

///////////////////////////////////////////////////////////////////////////////
// Conversion functions between Str, Int & Hex 

int IntStr2Int(CString str)
{
    int value = 0;
    int sign = 1;
    for (int i=0;i<str.GetLength();i++)
    {
        if (str[i]=='-' && i==0)
            sign = -1;
        else if (str[i]>='0' && str[i]<='9')
            value = value*10 + str[i]-'0';
        else
            ASSERT(0);

    }
    return sign*value;
}

int HexStr2Int(CString str)
{
    int nRet   = 0;
    int nStart = 0;

    //check if string is preceded by "0x"
    if (str.Left(2).CompareNoCase("0x")==0)
        nStart=2;

    for (int i=nStart;i<str.GetLength();i++)
    {
        if (str[i]>='0' && str[i]<='9')
            nRet = nRet*16 + str[i]-'0';
        else if (str[i]>='A' && str[i]<='F')
            nRet = nRet*16 + str[i]-'A'+10;
        else if (str[i]>='a' && str[i]<='f')
            nRet = nRet*16 + str[i]-'a'+10;
        else
            ASSERT(0);
    }
    return nRet;
}

int Str2Int(CString str)
{
    if (str.IsEmpty())
        return 0;

    if (str.Left(2).CompareNoCase("0x")==0)
        return HexStr2Int(str);
    else
        return IntStr2Int(str);
}

CString Int2IntStr(int i)
{
    CString str;

    while (i>0)
    {
        char b = '0' + i%10;
        i = i/10;
        str = b + str;
    }

    if (str.IsEmpty())
        return "0";

    return str;
}

CString Int2HexStr(int i)
{
    CString str;

    while (i>0)
    {
        int h = i%16;
        char b = (h<10?'0':'A'-10) + h;
        i = i/16;
        str = b + str;
    }

    if (str.IsEmpty())
        return "0x0000";
    
    CString prefix = "0x0000";

    return prefix.Left(2+(3-(str.GetLength()-1)%4)) + str;
}


void BitMaskAdjust(unsigned short nBitmask, unsigned short *pValue, bool bSet)
{
    ASSERT(nBitmask != 0);

    /*
     * find start = index of first bit in bitmask. end = last
     */
    unsigned short start = 0;
    while (((1<<start) & nBitmask) == 0) start++;
    unsigned short end = 16;
    while (((1<<end) & nBitmask) == 0) end--;

    if (bSet)
    {
        *pValue = *pValue << start;
        *pValue = *pValue & nBitmask;
    }
    else
    { //Get
        *pValue = *pValue & nBitmask;
        *pValue = *pValue >> start;
    }
}

unsigned short BitMaskAdjust(unsigned short nBitmask, unsigned short nValue, bool bSet)
{
    BitMaskAdjust(nBitmask, &nValue, bSet);
    return nValue;
}

/////////////////////////////////////////////////////////////////////////////
// CFrameCounter
// 
// use the systems high frequency clock to calculate the FPS for a scene

CFrameCounter::CFrameCounter()
{
    if (QueryPerformanceFrequency(&m_nFrequency) == 0)
    {
        //assert(0); //dunno what to do if there is no system clock
    }
}

CFrameCounter::~CFrameCounter()
{
}

void CFrameCounter::Reset()
{
    m_nFrame     = 0;
    for (int i=0;i<MAX_FRAMECALC;i++)
        m_nTimeFrame[i].QuadPart = 0;
}

void CFrameCounter::operator++(int n)
{
    QueryPerformanceCounter(&m_nTimeFrame[m_nFrame%MAX_FRAMECALC]);
    m_nFrame++;
}

int CFrameCounter::Frames()
{
    return m_nFrame;
}

float CFrameCounter::FramesPerSecond()
{
    LARGE_INTEGER   nTimeNow;
    QueryPerformanceCounter(&nTimeNow);
    float fps;
    _int64 tmp = 0;

    if (m_nFrame<MAX_FRAMECALC)
    {
        for (int i=0;i<m_nFrame-1;i++)
            tmp += m_nTimeFrame[i+1].QuadPart - 
                   m_nTimeFrame[i  ].QuadPart;

        fps = (float)tmp / (float)m_nFrequency.QuadPart;
        if (fps!=0)
            fps = (float)(m_nFrame-1) / fps;
    }
    else
    {
        for (int i=0;i<MAX_FRAMECALC-1;i++)
            tmp += m_nTimeFrame[(m_nFrame+i+1)%MAX_FRAMECALC].QuadPart - 
                   m_nTimeFrame[(m_nFrame+i  )%MAX_FRAMECALC].QuadPart;

        fps = (float)tmp / (float)m_nFrequency.QuadPart;
        if (fps!=0)
            fps = (float)(MAX_FRAMECALC-1) / fps;
    }

    return fps;
}

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
// Load and Save settings to and from a Ini File

CSettings::CSettings()
{
    char str[MAX_PATH] = "";
    GetCurrentDirectory(MAX_PATH, str);

    m_szIniFileName = str; 
    m_szIniFileName += "default.ini"; //should be overwritten by a call to SetIniFilname (below)
}

CSettings::~CSettings()
{
}

void CSettings::SetIniFilename(CString szFileName)
{
    m_szIniFileName = szFileName;
}

void CSettings::Clear(const char *pszSection)
{
    WritePrivateProfileSection(pszSection, "", m_szIniFileName);
}

void CSettings::Set(const char *pszSection, const char *pszKey, const char *pszValue)
{
    WritePrivateProfileString(pszSection, pszKey, pszValue, m_szIniFileName);
}

void CSettings::Set(const char *pszSection, const char *pszKey, int dwValue)
{
    WritePrivateProfileString(pszSection, pszKey, Int2Str(dwValue), m_szIniFileName);
}

int CSettings::Get(const char *pszSection, const char *pszKey, int nDefault)
{
    return GetPrivateProfileInt(pszSection, pszKey, nDefault, m_szIniFileName);
}

void CSettings::Get(const char *pszSection, const char *pszKey, char* pszValue, int nSize, const char* pszDefault)
{
    GetPrivateProfileString(pszSection, pszKey, pszDefault, pszValue, nSize, m_szIniFileName);
}

/////////////////////////////////////////////////////////////////////////////
// GetCompileDate
//
// return a string containing the date this file was compiled

void GetCompileDate(char* pszVersion, int nLength)
{
    char Mmm[4] = {0};
    int mmm, ddd, yyyy;
    sscanf_s(__DATE__, "%3s %d %d", Mmm, &ddd, &yyyy);
    if      (strcmp(Mmm,"Jan")==0) mmm = 1;
    else if (strcmp(Mmm,"Feb")==0) mmm = 2;
    else if (strcmp(Mmm,"Mar")==0) mmm = 3;
    else if (strcmp(Mmm,"Apr")==0) mmm = 4;
    else if (strcmp(Mmm,"May")==0) mmm = 5;
    else if (strcmp(Mmm,"Jun")==0) mmm = 6;
    else if (strcmp(Mmm,"Jul")==0) mmm = 7;
    else if (strcmp(Mmm,"Aug")==0) mmm = 8;
    else if (strcmp(Mmm,"Sep")==0) mmm = 9;
    else if (strcmp(Mmm,"Oct")==0) mmm =10;
    else if (strcmp(Mmm,"Nov")==0) mmm =11;
    else if (strcmp(Mmm,"Dec")==0) mmm =12;
    else 
    {
        mmm=0;
        ASSERT(0);
    }
    sprintf_s(pszVersion, nLength, "%02d%02d%02d", mmm, ddd, yyyy%100);
}


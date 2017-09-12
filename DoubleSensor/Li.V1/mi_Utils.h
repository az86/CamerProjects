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
// Utils.h : main header file for the Utils
//

#if !defined(_MI_UTILS_H_)
#define _MI_UTILS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// usefull macros

#define CLAMP(c,min,max) (c<(min))?(c=(min)):((c>(max))?(c=(max)):c)

/////////////////////////////////////////////////////////////////////////////
// 
// some usefull function

CString     Int2Str(int value);
CString     Flt2Str(float value, int width=4, int precision=1);
CString     FormatStr(LPCTSTR lpszFormat, ...);
int         IntStr2Int(CString str);
int         HexStr2Int(CString str);
int         Str2Int(CString str);
CString     Int2IntStr(int i);
CString     Int2HexStr(int i);


/////////////////////////////////////////////////////////////////////////////
// CFrameCounter
// 
// use the systems high frequency clock to calculate the FPS for a scene

#define MAX_FRAMECALC   10  //over how many frames should the calculations take place

class CFrameCounter
{
    //creating
public:
    CFrameCounter();
    ~CFrameCounter();

//Attributes
protected:
    LARGE_INTEGER   m_nFrequency;
    LARGE_INTEGER   m_nTimeFrame[MAX_FRAMECALC];
    int             m_nFrame;

//Implementation
public:
    void    Reset();
    void    operator++(int);
    int     Frames();
    float   FramesPerSecond();
};

/////////////////////////////////////////////////////////////////////////////
// CSettings
//
// Load and Save settings to and from a Ini File

class CSettings
{
// Construction
public:
    CSettings();
    ~CSettings();

// Attributes
protected:
    CString m_szIniFileName;

// Implementation
public:
    void    SetIniFilename(CString szFileName);
    void    Clear(const char *pszSection);
    int     Get(const char* pszSection, const char* pszKey, int nDefault);
    void    Get(const char* pszSection, const char* pszKey, char* pszValue, int nSize, const char* pszDefault);
    void    Set(const char* pszSection, const char* pszKey, const char* pszValue);
    void    Set(const char* pszSection, const char* pszKey, int dwValue);
};

/////////////////////////////////////////////////////////////////////////////

void GetCompileDate(char* pszVersion);


#endif //_MI_UTILS_H_


// Li.MainFrame.h : Li.MainFrame Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "MainFrm.h"

// CLiMainFrameApp:
// �йش����ʵ�֣������ Li.MainFrame.cpp
//

class CLiMainFrameApp : public CWinAppEx
{
public:
	CLiMainFrameApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	CMainFrame* pFrame;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLiMainFrameApp theApp;

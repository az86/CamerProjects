
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "Li.MainFrame.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);

	m_Li_V1_MainView = new MainView;
}

CMainFrame::~CMainFrame()
{
	if(m_Li_V1_MainView != NULL)  delete m_Li_V1_MainView;	//AZ
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	
 	 m_Li_V1_MainView->Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, 1234,NULL);
 	 m_Li_V1_MainView->ShowWindow(SW_SHOWMAXIMIZED);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	//cs.style = WS_OVERLAPPED | WS_SYSMENU | WS_BORDER;
	return TRUE;
}
// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
 	CRect rectMenuBar,rectToolBar,rectStatusBar,rect1;
 
    	if (m_Li_V1_MainView != NULL && m_Li_V1_MainView->GetSafeHwnd())
    	{
    		m_wndStatusBar.GetClientRect(&rectStatusBar);
   		m_Li_V1_MainView->MoveWindow(0, 0, cx, cy - rectStatusBar.Height());
    	}
	
}
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
 	if (m_Li_V1_MainView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
 		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnKillFocus(CWnd* pNewWnd)
{
	CFrameWndEx::OnKillFocus(pNewWnd);

	if (m_Li_V1_MainView->GetSafeHwnd())
	{
		m_Li_V1_MainView->ClearHotKeys();
	}
}


void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	CFrameWndEx::OnSetFocus(pOldWnd);

 	if (m_Li_V1_MainView->GetSafeHwnd())
 	{
 		m_Li_V1_MainView->InsertHotKeys();
 	}
}

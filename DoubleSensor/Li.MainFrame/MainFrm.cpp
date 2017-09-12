
// MainFrm.cpp : CMainFrame 类的实现
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
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
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
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
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
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	//cs.style = WS_OVERLAPPED | WS_SYSMENU | WS_BORDER;
	return TRUE;
}
// CMainFrame 诊断

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


// CMainFrame 消息处理程序
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

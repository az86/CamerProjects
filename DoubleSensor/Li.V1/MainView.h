#pragma once
#include "Resource.h"
#include "DebugView.h"
#include "DebugViewToolBar.h"
#include "USB3Camera.h"
#include "CameraDataProcess.h"
#include <vector>

using namespace std;
#define _VECTORLOCK()  \
		WaitForSingleObject( m_hAccessSem, INFINITE)
#define _UNVECTORLOCK()    \
		ReleaseSemaphore( m_hAccessSem, 1, NULL )

class AFX_EXT_CLASS MainView : public CWnd
{
public:
	DebugView          DebugView;
	DebugViewToolBar   DebugViewToolBar;

	USB3Camera         *m_USB3Camera;
	CameraDataProcess *m_CameraDataProcess;

	bool               m_IsPlaying;
	bool               m_IsDisplayAll;
	int                m_PictureFormat;

	bool               m_IsSetting;
	void ClearHotKeys();
	void InsertHotKeys();
	void StartDisplay();
public:
	MainView(void);
	virtual ~MainView(void);

	// Generated message map functions
private:
	bool bPnP_DevNodeChange;
    bool bPnP_Removal;
	bool bPnP_Arrival;

	static void Display_Thread(void *param);
	
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(MainView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LONG OnDeviceState(UINT, LONG);
	afx_msg LONG OnDisplay(UINT, LONG);
	afx_msg LONG OnDuanCount(UINT, LONG);
	afx_msg LONG OnFailureCount(UINT, LONG);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	virtual BOOL DestroyWindow();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


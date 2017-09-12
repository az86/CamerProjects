#include "StdAfx.h"
#include "MainView.h"
#include "Resource.h"
#include <dbt.h>
#include "LogTool.h"
#include "SetInfoHelper.h"

MainView::MainView(void)
{
	//::CoInitialize(NULL);
	CoInitializeEx(0, COINIT_MULTITHREADED);

	this->m_USB3Camera = NULL;
	if(this->m_USB3Camera==NULL)
	{
		this->m_USB3Camera = new USB3Camera(this);
	}

	this->m_CameraDataProcess = NULL;
	if(this->m_CameraDataProcess==NULL)
	{
		this->m_CameraDataProcess = new CameraDataProcess();
	}

	m_IsPlaying = false;
	m_IsSetting = false;
	m_IsDisplayAll = false;
}


MainView::~MainView(void)
{
	::CoUninitialize();

	this->m_USB3Camera->StopPlay();
	HANDLE HANDLE_DISPLAY = CreateEvent(NULL, FALSE, FALSE, _T("HANDLE_DISPLAY"));
	ResetEvent(HANDLE_DISPLAY);
	CloseHandle(HANDLE_DISPLAY);
	this->m_IsPlaying = false;
	
	Sleep(4000);
	if(this->m_USB3Camera!=NULL)
	{
		this->m_USB3Camera->CloseDevice();

		delete this->m_USB3Camera;
		this->m_USB3Camera = NULL;
	}

	//Sleep(2000);
}

BEGIN_MESSAGE_MAP(MainView, CWnd)
	//{{AFX_MSG_MAP(MainView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_USB_STATE, OnDeviceState)
	ON_MESSAGE(WM_USB_DISPLAY, OnDisplay)
	ON_MESSAGE(WM_USB_COUNT, OnDuanCount)
	ON_MESSAGE(WM_USB_FAILURECOUNT, OnFailureCount)
	//}}AFX_MSG_MAP
	ON_WM_HOTKEY()
	ON_WM_CHAR()
	ON_WM_ACTIVATE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

int MainView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL b = DebugViewToolBar.Create(IDD_DIG_TOOL,this);//IDD_DIG_TOOL
	b = DebugViewToolBar.ShowWindow(SW_SHOW);

	DebugView.Create(IDD_DLG_V1,this);//IDD_DLG_V1
	DebugView.ShowWindow(SW_SHOW);

	if(this->m_USB3Camera!=NULL)
	{
		this->m_USB3Camera->GetDeviceNames();
	}

	SetInfoHelper helper;
	this->m_PictureFormat = helper.GetPictureFormat();
	this->m_CameraDataProcess->m_PictureFormat = this->m_PictureFormat;

	m_CameraDataProcess->Open(DebugView.GetDisplayDC()->m_hDC);

	//AfxBeginThread((AFX_THREADPROC)Display_Thread,(void*)this);

	//RegisterHotKey(m_hWnd,1001,0,'0'); 
	RegisterHotKey(this->m_hWnd,1002,0,VK_NUMPAD0); 

	return 0;
}
void MainView::StartDisplay()
{
	this->m_IsPlaying = true;
	AfxBeginThread((AFX_THREADPROC)Display_Thread,(void*)this);
}
void MainView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if (DebugViewToolBar.GetSafeHwnd())
	{
		DebugViewToolBar.MoveWindow(0,0,cx,50);
		DebugView.MoveWindow(0,50,cx,cy-50);
	}
}

LONG MainView::OnDuanCount(WPARAM buff, LPARAM length)
{
	BSTR b = (BSTR)buff;
	CString str(b);
	SysFreeString(b);

	DebugViewToolBar.DisplayDuanCount(str);

    return 0;
}
LONG MainView::OnFailureCount(WPARAM buff, LPARAM length)
{
	BSTR b = (BSTR)buff;
	CString str(b);
	SysFreeString(b);

	DebugViewToolBar.DisplayFailureCount(str);

    return 0;
}

LONG MainView::OnDeviceState(WPARAM buff, LPARAM length)
{
	BSTR b = (BSTR)buff;
	CString str(b);
	SysFreeString(b);

	DebugViewToolBar.DisplayDeviceState(str);

	int i = (int)length;
	switch(i)
	{
		case 1:
			break;
		case 2:
			((CButton*)(DebugViewToolBar.GetDlgItem(IDC_BUTTON_PLAY)))->EnableWindow(true);
			((CButton*)(DebugViewToolBar.GetDlgItem(IDC_BUTTON_PLAY)))->SetWindowText(_T("播放"));
		   this->m_IsPlaying = false;
			break;
		case 3:
			break;
	}
	
	return -1;
}
LONG MainView::OnDisplay(WPARAM buff, LPARAM length)
{
	//显示信标
	HANDLE HANDLE_DISPLAY = CreateEvent(NULL, FALSE, FALSE, _T("HANDLE_DISPLAY"));

	BufData buf;
	buf.pBuf = (unsigned char *)buff;
	buf.size = (long)length;

	long rowindex = 0;
	LogTool::Writelog(LogTool::FormatLog(buf.pBuf,buf.size,&rowindex),"mdata");
	LogTool::Writelog("\t\n","mdata");

	//SetEvent(HANDLE_DISPLAY);

	CloseHandle(HANDLE_DISPLAY);

	return -1;
}

LRESULT MainView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DEVICECHANGE) 
	{
		if (wParam == DBT_DEVNODES_CHANGED) 
		{
			bPnP_DevNodeChange = true;
			bPnP_Removal = false;
		}

		if (wParam == DBT_DEVICEARRIVAL) 
		{
			bPnP_Arrival = true;
			bPnP_DevNodeChange = false;
		}

		if (wParam == DBT_DEVICEREMOVECOMPLETE) 
			bPnP_Removal = true;

		if (bPnP_Removal==true) 
		{
			bPnP_Removal = false;

			this->m_USB3Camera->CloseDevice();
			this->m_USB3Camera->GetDeviceNames();
		}

		if (bPnP_DevNodeChange== false && bPnP_Arrival == true) 
		{
			bPnP_Arrival = false;
			bPnP_DevNodeChange = false;
			
			this->m_USB3Camera->CloseDevice();
			this->m_USB3Camera->GetDeviceNames();
		}

	}

	return CWnd::WindowProc(message, wParam, lParam);
}
void MainView::Display_Thread(void *param)
{
	int imagecount = 0;
	CTime t1=CTime::GetCurrentTime();

	MainView *pMainView = (MainView *)param;
	//信标
	HANDLE hHandle = CreateEvent(NULL, TRUE, FALSE, _T("HANDLE_DISPLAY"));
	while ( pMainView->m_IsPlaying )
	{
		WaitForSingleObject( hHandle, INFINITE );
		if(!pMainView->m_USB3Camera->m_pImageBuffers.empty())
		{
			BufData buf = *(pMainView->m_USB3Camera->m_pImageBuffers.begin());

			WaitForSingleObject( pMainView->m_USB3Camera->m_hImageAccessSem, INFINITE);
			pMainView->m_USB3Camera->m_pImageBuffers.erase(pMainView->m_USB3Camera->m_pImageBuffers.begin());
			ReleaseSemaphore( pMainView->m_USB3Camera->m_hImageAccessSem, 1, NULL );

			switch(pMainView->m_USB3Camera->m_CameraConfigureInfo.OutFormat)
			{
			case 0x02://YUV
				//pMainView->m_CameraDataProcess->ProcessData4(buf.pBuf,buf.size);
				//pMainView->m_CameraDataProcess->ProcessYUVData(buf.pBuf,buf.size);
				pMainView->m_CameraDataProcess->ProcessYUVData(buf.pBuf,buf.size,pMainView->m_IsDisplayAll);
				//LogTool::CreateYuvFile(buf.pBuf,buf.size);
				break;
			case 0x03://RAW RGB
				//pMainView->m_CameraDataProcess->ProcessData2(buf.pBuf,buf.size);
				//pMainView->m_CameraDataProcess->ProcessRAWData(buf.pBuf,buf.size);
				CRect rect = pMainView->DebugView.GetDisplayRect();
				if(rect.Width()<1280)
				{
					pMainView->m_CameraDataProcess->SetDisplayWidthHeight(rect.Width(), rect.Height());
				}
				else
				{
					pMainView->m_CameraDataProcess->SetDisplayWidthHeight(1280,1024);
				}
				pMainView->m_CameraDataProcess->ProcessRAWData(buf.pBuf, buf.size, pMainView->m_IsDisplayAll);
				//pMainView->m_CameraDataProcess->ProcessRAWData(buf.pBuf + buf.size, buf.size, pMainView->m_IsDisplayAll);
				//LogTool::CreateRAWFile(buf.pBuf,buf.size);
				break;
			}

			pMainView->DebugView.UpdateDisplayer();

			delete [] buf.pBuf;

			CString logstr;
			logstr.Format("pUSB3Camera->m_pImageBuffers:%d",pMainView->m_USB3Camera->m_pImageBuffers.size());
			LogTool::WriteTimelog(logstr,"DisplayImageBuffersCount");

			imagecount++;
			CTime t = CTime::GetCurrentTime();
			CTimeSpan span = t-t1;
			int totMS = span.GetTotalSeconds();
			if (totMS >0)
			{
				int rate = imagecount / totMS;

				CString logstr;
				logstr.Format("%d",rate);
					
				((CStatic*)(pMainView->DebugViewToolBar.GetDlgItem(IDC_STA_FPSVALUE)))->SetWindowText(logstr);

				t1=CTime::GetCurrentTime();
				imagecount = 0;
			}
		}
		else
		{
			Sleep(100);
			//ResetEvent(hHandle);
		}
	}
}

BOOL MainView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	/*
	if(pMsg->message==WM_KEYDOWN || pMsg->message==WM_RBUTTONDOWN)	//jiehuo鼠标消息
	{
		if(pMsg -> wParam == VK_NUMPAD0  || pMsg -> wParam == 0x30 )
		{
			AfxMessageBox("0");
			return TRUE;
		}
	}
	*/
	return CWnd::PreTranslateMessage(pMsg);
}

void MainView::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	CWnd::OnHotKey(nHotKeyId, nKey1, nKey2);

	if(nHotKeyId==1002||nHotKeyId==1001)
	{
		CWnd::SetForegroundWindow();//使得被激活窗口出现在前景   

		if(m_IsSetting==false)
		{
			DebugViewToolBar.OnBnClickedButtonPlay();
		}
	}
	
}


BOOL MainView::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	UnregisterHotKey(m_hWnd,1001);    
	UnregisterHotKey(m_hWnd,1002); 

	return CWnd::DestroyWindow();
}

void MainView::ClearHotKeys()
{
	UnregisterHotKey(m_hWnd,1002);
	UnregisterHotKey(m_hWnd,1001);
}
void MainView::InsertHotKeys()
{
	RegisterHotKey(m_hWnd,1002,0,VK_NUMPAD0); 
	RegisterHotKey(m_hWnd,1001,0,'0');  
}


void MainView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnChar(nChar, nRepCnt, nFlags);
}


void MainView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	// TODO: Add your message handler code here
}


void MainView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
}

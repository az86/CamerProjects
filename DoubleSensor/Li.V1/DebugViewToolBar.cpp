// DebugViewToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "DebugViewToolBar.h"
#include "afxdialogex.h"
#include "DebugDlg.h"
#include "OptionDlg.h"
#include "MainView.h"
#include "DebugSetDlg.h"
#include "CameraConfigureInfoHelper.h"
#include "SetInfo.h"
#include "SetInfoHelper.h"
#include "SystemHelper.h"
#include "LogTool.h"



// DebugViewToolBar dialog

IMPLEMENT_DYNAMIC(DebugViewToolBar, CDialog)

DebugViewToolBar::DebugViewToolBar(CWnd* pParent /*=NULL*/)
	: CDialog(DebugViewToolBar::IDD, pParent)
{
	this->m_WDSFileName.ReleaseBuffer();
}

DebugViewToolBar::~DebugViewToolBar()
{
}

void DebugViewToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DebugViewToolBar, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &DebugViewToolBar::OnBnClickedButtonPlay)
	//ON_BN_CLICKED(IDC_BUTTON_STOP, &DebugViewToolBar::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &DebugViewToolBar::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_DEBUG1, &DebugViewToolBar::OnBnClickedButtonDebug1)
	ON_BN_CLICKED(IDC_BT_LOADYUVFILE, &DebugViewToolBar::OnBnClickedBtLoadyuvfile)
	ON_BN_CLICKED(IDC_BT_LOADIMGFILE, &DebugViewToolBar::OnBnClickedBtLoadimgfile)
	ON_STN_CLICKED(IDC_STA_FPSVALUE, &DebugViewToolBar::OnStnClickedStaFpsvalue)
	ON_BN_CLICKED(IDC_BT_LOADRAWFILE, &DebugViewToolBar::OnBnClickedBtLoadrawfile)
	ON_BN_CLICKED(IDC_BTN_GETDATA, &DebugViewToolBar::OnBnClickedBtnGetdata)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE, &DebugViewToolBar::OnBnClickedButtonCapture)
	ON_BN_CLICKED(IDC_BTN_DALL, &DebugViewToolBar::OnBnClickedBtnDall)
	ON_BN_CLICKED(IDC_BUTTON_MIRROR, &DebugViewToolBar::OnBnClickedButtonMirror)
	ON_BN_CLICKED(IDC_BUTTON_ROTATE, &DebugViewToolBar::OnBnClickedButtonRotate)
	ON_BN_CLICKED(IDC_BUTTON_WROTE_ROM, &DebugViewToolBar::OnBnClickedButtonWriteRom)
	ON_BN_CLICKED(IDC_BUTTON_READ_ROM, &DebugViewToolBar::OnBnClickedButtonReadRom)
	ON_BN_CLICKED(IDC_BUTTON_SET_RESOLUTION, &DebugViewToolBar::OnBnClickedButtonSetResolution)
END_MESSAGE_MAP()


// DebugViewToolBar message handlers


void DebugViewToolBar::OnBnClickedButtonDebug()
{
	DebugDlg dlg;
	dlg.DoModal();
}


void DebugViewToolBar::OnBnClickedButtonOption()
{
	OptionDlg dlg;
	dlg.DoModal();
}
void DebugViewToolBar::DisplayDeviceState(CString sate)
{
	GetDlgItem(IDC_STATIC_CAMERAINFO)->SetWindowText(sate);
}
void DebugViewToolBar::DisplayDuanCount(CString count)
{
	GetDlgItem(IDC_STATIC_DUANCOUNT)->SetWindowText(count);
}
void DebugViewToolBar::DisplayFailureCount(CString count)
{
	GetDlgItem(IDC_STATIC_FAILURECOUNT)->SetWindowText(count);
}
BOOL DebugViewToolBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_hAccel   =   ::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR1)); 

	return TRUE;  
	
}

void DebugViewToolBar::OnBnClickedButtonPlay()
{
	if(((MainView*)GetParent())->m_IsPlaying==false)
	{
		//打开电压
		//((MainView*)GetParent())->m_USB3Camera->TurnonPower();
		if(this->m_WDSFileName.IsEmpty())
		{
			if(!OnLoadWDSFile())
			{
				return;
			}
		}
		else
		{
			 LoadWDSFile(this->m_WDSFileName);
		}
		
		((MainView*)GetParent())->DebugView.IsDisplay(true);

		((MainView*)GetParent())->m_USB3Camera->ClearInBuffers();

		((MainView*)GetParent())->StartDisplay();
		((MainView*)GetParent())->m_USB3Camera->StartPlay();
		HANDLE HANDLE_DISPLAY = CreateEvent(NULL, FALSE, FALSE, _T("HANDLE_DISPLAY"));
		SetEvent(HANDLE_DISPLAY);
		CloseHandle(HANDLE_DISPLAY);

		((CButton*)(this->GetDlgItem(IDC_BUTTON_PLAY)))->SetWindowText(_T("停止"));
		((MainView*)GetParent())->m_IsPlaying = true;
	}
	else
	{
		((MainView*)GetParent())->m_USB3Camera->StopPlay();
		HANDLE HANDLE_DISPLAY = CreateEvent(NULL, FALSE, FALSE, _T("HANDLE_DISPLAY"));
		ResetEvent(HANDLE_DISPLAY);
		CloseHandle(HANDLE_DISPLAY);

		((CButton*)(this->GetDlgItem(IDC_BUTTON_PLAY)))->SetWindowText(_T("播放"));
		((CButton*)(this->GetDlgItem(IDC_BUTTON_PLAY)))->EnableWindow(false);
		((MainView*)GetParent())->m_IsPlaying = false;

		((MainView*)GetParent())->DebugView.IsDisplay(false);

		((MainView*)GetParent())->m_USB3Camera->ClearInBuffers();

	}
	
}

/*
void DebugViewToolBar::OnBnClickedButtonStop()
{
	((MainView*)GetParent())->m_USB3Camera->StopPlay();
	HANDLE HANDLE_DISPLAY = CreateEvent(NULL, FALSE, FALSE, _T("HANDLE_DISPLAY"));
	ResetEvent(HANDLE_DISPLAY);
	CloseHandle(HANDLE_DISPLAY);
}
*/
void DebugViewToolBar::OnBnClickedButtonLoad()
{
	/*
	static char szWDSFilter[]="WDS文件(*.WDS)|*.WDS||";

	CString fileName;
	CFileDialog *fileDlg = NULL;
	fileDlg = new CFileDialog(TRUE,NULL,"*.WDS",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szWDSFilter);

	//fileDlg.m_ofn.lpstrInitialDir="D:\\";
	if(fileDlg->DoModal()==IDOK)
	{
		fileName = fileDlg->GetPathName();

	   ((MainView*)GetParent())->m_USB3Camera->LoadWDSFile(fileName);

		//AfxMessageBox("WDS文件加载完成！");
		
		int w = ((MainView*)GetParent())->m_USB3Camera->m_CameraConfigureInfo.PreviewWidth;
		int h = ((MainView*)GetParent())->m_USB3Camera->m_CameraConfigureInfo.PreviewHeight;

		int rate = 2;
		((MainView*)GetParent())->m_CameraDataProcess->SetWidthHeight(w/rate,h/rate);
		((MainView*)GetParent())->DebugView.SetCameraSize(w/rate,h/rate);

		fileName.ReleaseBuffer();
	}

	delete [] fileDlg;
	fileDlg = NULL;
	*/
	
}

bool DebugViewToolBar::OnLoadWDSFile()
{
	OleInitialize(NULL);

	char szFileName[MAX_PATH] = "";
    OPENFILENAME stOF;               //必须的
    RtlZeroMemory(&stOF,sizeof(stOF));//必须的
    stOF.lStructSize = sizeof(stOF); //必须的
    stOF.hwndOwner = NULL;
    stOF.lpstrFilter = "WDS Files(*.WDS)\0 *.WDS\0 All Files";
    stOF.lpstrFile = szFileName;
    stOF.nMaxFile = MAX_PATH;
    stOF.Flags = OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
    if (GetOpenFileName(&stOF) != NULL)
    {

        if(((MainView*)GetParent())->m_USB3Camera->LoadWDSFile(szFileName)==false)
		{
			return false;
		}

		int w = ((MainView*)GetParent())->m_USB3Camera->m_CameraConfigureInfo.PreviewWidth;
		int h = ((MainView*)GetParent())->m_USB3Camera->m_CameraConfigureInfo.PreviewHeight;

		CString info;
		info.Format("分辨率：%d X %d",w,h);
		GetDlgItem(IDC_STATIC_FB)->SetWindowText(info);


		((MainView*)GetParent())->m_CameraDataProcess->SetWidthHeight(w,h);
		CRect rect = ((MainView*)GetParent())->DebugView.GetDisplayRect();
		//((MainView*)GetParent())->m_CameraDataProcess->SetDisplayWidthHeight(rect.Width(),rect.Height());
		((MainView*)GetParent())->m_CameraDataProcess->SetDisplayWidthHeight(1280,1024);
		
		this->m_WDSFileName.Format("%s",szFileName);

		return true;
    }

	OleUninitialize();

	return false;
}
void DebugViewToolBar::LoadWDSFile(CString file)
{
	if(((MainView*)GetParent())->m_USB3Camera->LoadWDSFile(file)==false)
	{
		return;
	}

	//AfxMessageBox("WDS文件加载完成！");
		
	int w = ((MainView*)GetParent())->m_USB3Camera->m_CameraConfigureInfo.PreviewWidth;
	int h = ((MainView*)GetParent())->m_USB3Camera->m_CameraConfigureInfo.PreviewHeight;

	CString info;
	info.Format("分辨率：%d X %d",w,h);
	GetDlgItem(IDC_STATIC_FB)->SetWindowText(info);

	((MainView*)GetParent())->m_CameraDataProcess->SetWidthHeight(w,h);
	//((MainView*)GetParent())->DebugView.SetCameraSize(w,h);
	CRect rect = ((MainView*)GetParent())->DebugView.GetDisplayRect();
	//((MainView*)GetParent())->m_CameraDataProcess->SetDisplayWidthHeight(rect.Width(),rect.Height());
	((MainView*)GetParent())->m_CameraDataProcess->SetDisplayWidthHeight(1280,1024);
}



void DebugViewToolBar::OnBnClickedButtonDebug1()
{
	((MainView*)GetParent())->m_IsSetting = true;
	((MainView*)GetParent())->ClearHotKeys();

	DebugSetDlg dlg;
	dlg.SetCamera( ((MainView*)GetParent())->m_USB3Camera );
	dlg.SetPictureFormat(((MainView*)GetParent())->m_PictureFormat);
	dlg.DoModal();

	((MainView*)GetParent())->m_PictureFormat = dlg.m_PictureFormat;
	((MainView*)GetParent())->m_CameraDataProcess->m_PictureFormat = dlg.m_PictureFormat;
	((MainView*)GetParent())->InsertHotKeys();
	((MainView*)GetParent())->m_IsSetting = false;
}


void DebugViewToolBar::OnBnClickedBtLoadyuvfile()
{
	CString fileName = "";

	CFileDialog fileDlg(TRUE,NULL,"*.YUV",1,"Access file(*.YUV)|*.YUV||");
	if(fileDlg.DoModal()==IDOK)
	{
	   fileName = fileDlg.GetPathName();
	   BufData data = ((MainView*)GetParent())->m_CameraDataProcess->ReadFile(fileName);

	   //((MainView*)GetParent())->m_CameraDataProcess->ProcessData4(data.pBuf,data.size);
	   ((MainView*)GetParent())->m_CameraDataProcess->ProcessData5(data.pBuf,data.size);

	   delete[] data.pBuf;
	}
}


void DebugViewToolBar::OnBnClickedBtLoadimgfile()
{
	/*
	static char szFilter[]="img文件(*.img)|*.img||";

	CString fileName;

	CFileDialog *fileDlg = NULL;
	fileDlg = new CFileDialog(TRUE,NULL,"*.img",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if(fileDlg->DoModal()==IDOK)
	{
		fileName = fileDlg->GetPathName();

		((MainView*)GetParent())->m_USB3Camera->LoadImgFile(fileName);

		//AfxMessageBox("img文件加载完成！");
	}
	delete [] fileDlg;
	fileDlg = NULL;
	*/

	char szFileName[MAX_PATH] = "img File\0";
    OPENFILENAME stOF;               //必须的
    RtlZeroMemory(&stOF,sizeof(stOF));//必须的
    stOF.lStructSize = sizeof(stOF); //必须的
    stOF.hwndOwner = NULL;
    stOF.lpstrFilter = "img Files(*.img)\0 *.img\0 All Files";
    stOF.lpstrFile = szFileName;
    stOF.nMaxFile = MAX_PATH;
    stOF.Flags = OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
    if (GetOpenFileName(&stOF) != NULL)
    {

        //MessageBox(szFileName, "执行结果", MB_OK);
		((MainView*)GetParent())->m_USB3Camera->LoadImgFile(szFileName);

    }
	

	/*
	char szFileName[MAX_PATH];
	CComPtr<IFileOpenDialog> dialog;
	HRESULT hr = dialog.CoCreateInstance(__uuidof(FileOpenDialog));
	if(FAILED(hr) || dialog == NULL)
		return;


	hr = dialog->Show(this->m_hWnd);
	if(S_OK == hr)
	{
		CComPtr<IShellItem> itemResult;
		hr = dialog->GetResult(&itemResult);

		WCHAR* buffer = NULL;
		hr = itemResult->GetDisplayName(SIGDN_FILESYSPATH, &buffer);
		if(SUCCEEDED(hr) && buffer != NULL)
		{
			CString path;
			WideCharToMultiByte(CP_ACP,0,buffer,256,path.GetBuffer(0),256,NULL,NULL);

			((MainView*)GetParent())->m_USB3Camera->LoadImgFile(path);


			CoTaskMemFree(buffer);
		}
	}
	*/
}


void DebugViewToolBar::OnStnClickedStaFpsvalue()
{
	// TODO: Add your control notification handler code here
}


void DebugViewToolBar::OnBnClickedBtLoadrawfile()
{
	CString fileName = "";

	CFileDialog fileDlg(TRUE,NULL,"*.RAW",1,"Access file(*.RAW)|*.RAW||");
	if(fileDlg.DoModal()==IDOK)
	{
	   fileName = fileDlg.GetPathName();
	   BufData data = ((MainView*)GetParent())->m_CameraDataProcess->ReadFile(fileName);
	   ((MainView*)GetParent())->m_CameraDataProcess->ProcessData11(data.pBuf,data.size);

	   //((MainView*)GetParent())->m_CameraDataProcess->ProcessData13(data.pBuf,data.size);

	   delete[] data.pBuf;
	}
}

void DebugViewToolBar::OnBnClickedBtnGetdata()
{
	if(((MainView*)GetParent())->m_IsPlaying==false)
	{
		//打开电压
		//((MainView*)GetParent())->m_USB3Camera->TurnonPower();

		if(!OnLoadWDSFile())
		{
			return;
		}

		((MainView*)GetParent())->m_USB3Camera->StartGetData();
		((CButton*)(this->GetDlgItem(IDC_BTN_GETDATA)))->SetWindowText(_T("停止采集"));
		((MainView*)GetParent())->m_IsPlaying = true;
	}
	else
	{
		((MainView*)GetParent())->m_USB3Camera->StopGetData();

		((CButton*)(this->GetDlgItem(IDC_BTN_GETDATA)))->SetWindowText(_T("开始采集"));
		((MainView*)GetParent())->m_IsPlaying = false;

	}
}


void DebugViewToolBar::OnBnClickedButtonCapture()
{
	if(((MainView*)GetParent())->m_CameraDataProcess->m_IsCapturing == false)
	{
		((MainView*)GetParent())->m_CameraDataProcess->m_IsCapturing = true;
	}
}


BOOL DebugViewToolBar::PreTranslateMessage(MSG* pMsg)
{
	//OnBnClickedButtonPlay();
	if(pMsg->message==WM_KEYDOWN || pMsg->message==WM_RBUTTONDOWN)	//jiehuo鼠标消息
	{
		if(pMsg -> wParam == VK_ESCAPE || pMsg -> wParam == VK_RETURN )
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void DebugViewToolBar::OnBnClickedBtnDall()
{
	if(((MainView*)GetParent())->m_IsDisplayAll==false)
	{
		((CButton*)(this->GetDlgItem(IDC_BTN_DALL)))->SetWindowText(_T("自适应显示"));
		((MainView*)GetParent())->m_IsDisplayAll = true;
		((MainView*)GetParent())->DebugView.m_NeedUpdateDisplayer = true;
	}
	else
	{
		((CButton*)(this->GetDlgItem(IDC_BTN_DALL)))->SetWindowText(_T("全分辨显示"));
		((MainView*)GetParent())->m_IsDisplayAll = false;
		((MainView*)GetParent())->DebugView.m_NeedUpdateDisplayer = true;
	}
}


void DebugViewToolBar::OnBnClickedButtonMirror()
{
	// TODO:  在此添加控件通知处理程序代码
	switch (((MainView*)GetParent())->m_CameraDataProcess->m_MirrorStatus)
	{
	case MirrorNon:
		((MainView*)GetParent())->m_CameraDataProcess->m_MirrorStatus = MirrorX;
		break;
	case MirrorX:
		((MainView*)GetParent())->m_CameraDataProcess->m_MirrorStatus = MirrorY;
		break;
	case MirrorY:
		((MainView*)GetParent())->m_CameraDataProcess->m_MirrorStatus = MirrorXY;
		break;
	case MirrorXY:
		((MainView*)GetParent())->m_CameraDataProcess->m_MirrorStatus = MirrorNon;
		break;
	default:
		break;
	}
}


void DebugViewToolBar::OnBnClickedButtonRotate()
{
	// TODO:  在此添加控件通知处理程序代码
}


void DebugViewToolBar::OnBnClickedButtonWriteRom()
{
	// TODO:  在此添加控件通知处理程序代码
}


void DebugViewToolBar::OnBnClickedButtonReadRom()
{
	// TODO:  在此添加控件通知处理程序代码
}


void DebugViewToolBar::OnBnClickedButtonSetResolution()
{
	// TODO:  在此添加控件通知处理程序代码
}

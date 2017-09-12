#pragma once
#include "Resource.h"
#include "CameraConfigureInfo.h"

// DebugViewToolBar dialog

class DebugViewToolBar : public CDialog
{
	DECLARE_DYNAMIC(DebugViewToolBar)

public:
	DebugViewToolBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~DebugViewToolBar();
	
	void GetCameraConfigureInfo(CString configureFileName);


// Dialog Data
	enum { IDD = IDD_DIG_TOOL };

	void DisplayDeviceState(CString sate);
	void DisplayDuanCount(CString count);
	void DisplayFailureCount(CString count);

	bool OnLoadWDSFile();

	CString m_WDSFileName;
	void LoadWDSFile(CString file);

	//HACCEL   m_hAccel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDebug();
	afx_msg void OnBnClickedButtonOption();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPlay();
	//afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonDebug1();
	afx_msg void OnBnClickedBtLoadyuvfile();
	afx_msg void OnBnClickedBtLoadimgfile();
	afx_msg void OnStnClickedStaFpsvalue();
	afx_msg void OnBnClickedBtLoadrawfile();
	afx_msg void OnBnClickedBtnGetdata();
	afx_msg void OnBnClickedButtonCapture();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnDall();
	afx_msg void OnBnClickedButtonMirror();
	afx_msg void OnBnClickedButtonRotate();
	afx_msg void OnBnClickedButtonWriteRom();
	afx_msg void OnBnClickedButtonReadRom();
	afx_msg void OnBnClickedButtonSetResolution();
};

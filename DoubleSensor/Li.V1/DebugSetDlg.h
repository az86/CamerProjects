#pragma once
#include "Resource.h"
#include "USB3Camera.h"
#include "afxwin.h"

// DebugSetDlg dialog

class DebugSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DebugSetDlg)

private:
	USB3Camera         *m_USB3Camera;
public:
	DebugSetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~DebugSetDlg();

	void SetCamera(USB3Camera *camera);
	void SetPictureFormat(int f);
	void InitClockHzValue();
	

// Dialog Data
	enum { IDD = IDD_DIA_DEBUGSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool m_IsInitClockHzValue;

	void SetAVDD(int i);
	void SetDOVDD(int i);
	void SetDVDD(int i);
	void SetClock(int i);
	void SetDiaoDian(CString valuestr);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtDiaodianctrl();
	afx_msg void OnBnClickedBtReset();
	afx_msg void OnBnClickedBtClockctrl();
	virtual BOOL OnInitDialog();
	CComboBox m_DiaodianControlComb;

	afx_msg void OnClickedRadioAVDD();
	afx_msg void OnClickedRadioDOVDD();
	afx_msg void OnClickedRadioDVDD();
	int m_iAVDDVol;
	int m_iDOVDDVol;
	int m_iDVDDVol;
	afx_msg void OnClickedRadio10();
	int m_ClockValue;
	int m_ClockHzValue;
	afx_msg void OnBnClickedOk();
	afx_msg void OnChangeEditHzvaule();
	int m_PictureFormat;
	afx_msg void OnBnClickedBtnI2cr();
	afx_msg void OnBnClickedBtnI2cw();
	CString m_ADDRCString;
	CString m_REGCString;
	CString m_VALUECString;
	afx_msg void OnClickedRadio15();
	afx_msg void OnBnClickedBtPhpath();
	CString m_PhotoesPath;
	int m_Gpio1;
	int m_Gpio2;
	afx_msg void OnClickedRadio17();
	afx_msg void OnClickedRadio19();
};

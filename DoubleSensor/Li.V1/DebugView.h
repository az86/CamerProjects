#pragma once
#include "Resource.h"
#include "CameraConfigureInfo.h"
// DebugView dialog
typedef struct tagTABLE_UYVY2RGB 
{ 
unsigned short YtoR[256]; 
unsigned short YtoG[256]; 
unsigned short YtoB[256]; 
unsigned short UtoG[256]; 
unsigned short UtoB[256]; 
unsigned short VtoR[256]; 
unsigned short VtoG[256]; 
}TABLE_UYVY2RGB; 
 

class DebugView : public CDialog
{
	DECLARE_DYNAMIC(DebugView)

public:
	BOOL AllocateBuffers(CameraConfigureInfo info);

	DebugView(CWnd* pParent = NULL);   // standard constructor
	virtual ~DebugView();

	enum { IDD = IDD_DLG_V1 };

	void Display(unsigned char *pBuff,long length);
	void SetCameraConfigureInfo(CameraConfigureInfo cameraConfigureInfo);
	void SetCameraSize(int w ,int h);
	CDC* GetDisplayDC();

	int ProcessData(byte *pIn,int size);
	CRect GetDisplayRect();

	void IsDisplay(bool f);

	bool m_NeedUpdateDisplayer;
	void UpdateDisplayer();
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CameraConfigureInfo m_CameraConfigureInfo;
	BOOL UYVY2BMP(BYTE* pDst, BYTE *pSrc) ;
	void MakeConversionTable() ;
	CDC memDC;
	CDC * pDC;
	TABLE_UYVY2RGB table_UYVY2rgb; 
public:

private:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();

	CStatic m_Video;
	
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
};

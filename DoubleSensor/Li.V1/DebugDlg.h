#pragma once
#include "Resource.h"

// DebugDlg dialog

class DebugDlg : public CDialog
{
	DECLARE_DYNAMIC(DebugDlg)

public:
	DebugDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~DebugDlg();

// Dialog Data
	enum { IDD = IDD_DLG_DEBUG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

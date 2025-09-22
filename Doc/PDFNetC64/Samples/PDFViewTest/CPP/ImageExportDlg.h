#pragma once

#include "resource.h"

// CImageExportDlg dialog

class CImageExportDlg : public CDialog
{
	DECLARE_DYNAMIC(CImageExportDlg)

public:
	CImageExportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageExportDlg();

// Dialog Data
	enum { IDD = IDD_IMAGE_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	UINT m_dpi;
	int m_format;
	afx_msg void OnBnClickedOk();
};

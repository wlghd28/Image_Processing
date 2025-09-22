#if !defined(AFX_AUTHORIZEDLG_H__50D5040C_CDF0_43F7_8FE1_22815AD30CD2__INCLUDED_)
#define AFX_AUTHORIZEDLG_H__50D5040C_CDF0_43F7_8FE1_22815AD30CD2__INCLUDED_

#pragma once
// AuthorizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAuthorize dialog

class CAuthorize : public CDialog
{
// Construction
public:
	CAuthorize(CString dlg_title, CString static_title, CWnd* pParent = NULL);   

// Dialog Data
	//{{AFX_DATA(CAuthorize)
	enum { IDD = IDD_AUTHORIZE };
	CString	m_password;
	//}}AFX_DATA

	CString	m_static_title;
	CString	m_dlg_title;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuthorize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAuthorize)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTHORIZEDLG_H__50D5040C_CDF0_43F7_8FE1_22815AD30CD2__INCLUDED_)

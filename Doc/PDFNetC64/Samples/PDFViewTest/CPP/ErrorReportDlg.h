#if !defined(AFX_ERRORREPORTDLG_H__35066DE9_C5B3_437D_A503_F3DC52EE447A__INCLUDED_)
#define AFX_ERRORREPORTDLG_H__35066DE9_C5B3_437D_A503_F3DC52EE447A__INCLUDED_

#pragma once
// ErrorReportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CErrorReportDlg dialog

class CErrorReportDlg : public CDialog
{
// Construction
public:
	CErrorReportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CErrorReportDlg)
	enum { IDD = IDD_ERROR };
	CString	m_error_msg;
	CString	m_detail_msg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrorReportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CErrorReportDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERRORREPORTDLG_H__35066DE9_C5B3_437D_A503_F3DC52EE447A__INCLUDED_)

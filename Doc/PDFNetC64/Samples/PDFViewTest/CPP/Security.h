#if !defined(AFX_DOCSECURITY_H__6C46CE3C_CF8B_4FBB_8D65_1C31D333D60B__INCLUDED_)
#define AFX_DOCSECURITY_H__6C46CE3C_CF8B_4FBB_8D65_1C31D333D60B__INCLUDED_

#pragma once
// DocSecurity.h : header file
//

#include <PDF/PDFDoc.h>
#include "resource.h"

using namespace pdftron;

/////////////////////////////////////////////////////////////////////////////
// CDocSecurity dialog

class CDocSecurity : public CDialog
{
// Construction
public:
	CDocSecurity(PDF::PDFDoc& doc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDocSecurity)
	enum { IDD = IDD_DOC_SECURITY };
	CComboBox	m_sec_handler;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocSecurity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int m_selected;			// the index of currently selected SecurityHandler
	int m_initial_selection;
	PDF::PDFDoc& mr_doc;
	SDF::SecurityHandler m_new_sec_hdlr; // current SecurityHandler 

	// Generated message map functions
	//{{AFX_MSG(CDocSecurity)
	virtual BOOL OnInitDialog();
	afx_msg void OnSecHandlerChange();
	afx_msg void OnChangeSecurSettings();
	virtual void OnOK();
	afx_msg void OnViewSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCSECURITY_H__6C46CE3C_CF8B_4FBB_8D65_1C31D333D60B__INCLUDED_)

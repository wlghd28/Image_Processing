#if !defined(AFX_EditSecurityHandler_H__D8C00411_AE6D_4DE9_A34B_B20B2B47FDFF__INCLUDED_)
#define AFX_EditSecurityHandler_H__D8C00411_AE6D_4DE9_A34B_B20B2B47FDFF__INCLUDED_

#pragma once
// EditSecurityHandler.h : header file
//
#include <string>
#include <vector>
#include <Common/BasicTypes.h>

using namespace std;
using namespace pdftron;

/////////////////////////////////////////////////////////////////////////////
// CEditSecurityHandler dialog

class CEditSecurityHandler : public CDialog
{
// Construction
public:
	CEditSecurityHandler(string& old_user_pass, string& old_owner_pass, 
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditSecurityHandler)
	enum { IDD = IDD_STD_SECUR_EDIT };
	CComboBox	m_key_length;
	BOOL	m_user_pass_req;
	BOOL	m_owner_pass_req;
	CString	m_owner_pass;
	CString	m_user_pass;
	BOOL	m_access_support;
	BOOL	m_assemble_doc;
	BOOL	m_doc_modify;
	BOOL	m_extract_content;
	BOOL	m_fill_forms;
	BOOL	m_mod_annot;
	BOOL	m_print;
	BOOL	m_high_rez_print;
	//}}AFX_DATA


	int m_rev_num; 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditSecurityHandler)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	string& m_old_user_pass;
	string& m_old_owner_pass;

	void EnablePermissions();

	// Generated message map functions
	//{{AFX_MSG(CEditSecurityHandler)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeKeyStrength();
	afx_msg void OnUserPswdChk();
	afx_msg void OnOwnerPswdChk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EditSecurityHandler_H__D8C00411_AE6D_4DE9_A34B_B20B2B47FDFF__INCLUDED_)

// EditSecurityHandler.cpp : implementation file
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "EditSecurityHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditSecurityHandler dialog


CEditSecurityHandler::CEditSecurityHandler(std::string& old_user_pass, std::string& old_owner_pass, CWnd* pParent /*=NULL*/)
	: CDialog(CEditSecurityHandler::IDD, pParent), 
	  m_old_user_pass(old_user_pass),
	  m_old_owner_pass(old_owner_pass)
{
	//{{AFX_DATA_INIT(CEditSecurityHandler)
	m_user_pass_req = FALSE;
	m_owner_pass_req = FALSE;
	m_owner_pass = _T("");
	m_user_pass =  _T("");
	m_access_support = FALSE;
	m_assemble_doc = FALSE;
	m_doc_modify = FALSE;
	m_extract_content = FALSE;
	m_fill_forms = FALSE;
	m_mod_annot = FALSE;
	m_print = FALSE;
	m_high_rez_print = FALSE;
	//}}AFX_DATA_INIT
}

void CEditSecurityHandler::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditSecurityHandler)
	DDX_Control(pDX, IDC_KEY_STRENGTH, m_key_length);
	DDX_Check(pDX, IDC_USER_PSWD_CHK, m_user_pass_req);
	DDX_Check(pDX, IDC_OWNER_PSWD_CHK, m_owner_pass_req);
	DDX_Text(pDX, IDC_OWNER_PSWD_EDIT, m_owner_pass);
	DDV_MaxChars(pDX, m_owner_pass, 255);
	DDX_Text(pDX, IDC_USER_PSWD_EDIT, m_user_pass);
	DDV_MaxChars(pDX, m_user_pass, 256);
	DDX_Check(pDX, IDC_ACCESS_SUPPORT, m_access_support);
	DDX_Check(pDX, IDC_ASSEMBLE_DOC, m_assemble_doc);
	DDX_Check(pDX, IDC_DOC_MODIFY, m_doc_modify);
	DDX_Check(pDX, IDC_EXTRACT_CONTENTS, m_extract_content);
	DDX_Check(pDX, IDC_FILL_FORMS, m_fill_forms);
	DDX_Check(pDX, IDC_MOD_ANNOT, m_mod_annot);
	DDX_Check(pDX, IDC_PRINT, m_print);
	DDX_Check(pDX, IDC_HIGH_REZ_PRINT, m_high_rez_print);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditSecurityHandler, CDialog)
	//{{AFX_MSG_MAP(CEditSecurityHandler)
	ON_CBN_SELCHANGE(IDC_KEY_STRENGTH, OnChangeKeyStrength)
	ON_BN_CLICKED(IDC_USER_PSWD_CHK, OnUserPswdChk)
	ON_BN_CLICKED(IDC_OWNER_PSWD_CHK, OnOwnerPswdChk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditSecurityHandler message handlers

BOOL CEditSecurityHandler::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_key_length.AddString(_T("40-bit RC4 (PDF 1.3, Acrobat 3 or later.)"));
	m_key_length.AddString(_T("128-bit RC4 (PDF 1.4, Acrobat 5 or later.)"));
	m_key_length.AddString(_T("128-bit AES (PDF 1.6, Acrobat 7 or later.)"));

	m_key_length.SetCurSel(m_rev_num-2);

	if (!m_user_pass_req) GetDlgItem(IDC_USER_PSWD_EDIT)->EnableWindow(FALSE);
	if (!m_owner_pass_req) GetDlgItem(IDC_OWNER_PSWD_EDIT)->EnableWindow(FALSE);

	EnablePermissions();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#include "AuthorizeDlg.h"

void CEditSecurityHandler::OnOK() 
{
	UpdateData();

	CAuthorize dlg(_T("Confirm Password"), _T("Confirm password to open the document")); 

	// Make sure that password are unique
	if (m_user_pass != "") 
	{ 
		CString user_pass = m_user_pass;
		if (m_user_pass == _T("üüüüüüüüüü"))  
		{
			user_pass = m_old_user_pass.c_str();
		}

		CString owner_pass = m_owner_pass;
		if (m_owner_pass == _T("üüüüüüüüüü"))  
		{
			owner_pass = m_old_owner_pass.c_str();
		}

		if (user_pass == owner_pass) 
		{
			MessageBox(_T("The owner and user passwords must be unique. Please update entered passwords."), _T("Security Handler"), MB_ICONINFORMATION);
			return; 
		}
	}

	if (m_user_pass_req && m_user_pass != _T("") && m_user_pass != _T("üüüüüüüüüü")) 
	{
		if (dlg.DoModal() == IDOK) 
		{
			if (dlg.m_password != m_user_pass) 
			{
				MessageBox(_T("The password was not confirmed. Please reenter the password."), _T("Security Handler"), MB_ICONINFORMATION);
				CEditSecurityHandler::OnOK();
				return; 
			}
		}
		else return; 
	}

	dlg.m_static_title = _T("Confirm password to change security options");

	if (m_owner_pass_req && m_owner_pass != _T("") && m_owner_pass != _T("üüüüüüüüüü")) 
	{
		if (dlg.DoModal() == IDOK) 
		{
			if (dlg.m_password != m_owner_pass) 
			{
				MessageBox(_T("The password was not confirmed. Please reenter the password."), _T("Security Handler"), MB_ICONINFORMATION);
				CEditSecurityHandler::OnOK();
				return; 
			}
		}
		else return; 
	}

	
	CDialog::OnOK();
}

void CEditSecurityHandler::OnChangeKeyStrength()
{
	m_rev_num = m_key_length.GetCurSel() + 2;
	ASSERT (m_rev_num==2 || m_rev_num==3 || m_rev_num==4);
	EnablePermissions();
}


void CEditSecurityHandler::EnablePermissions()
{
	// Enable all permissions
	GetDlgItem(IDC_PRINT)->EnableWindow(TRUE);
	GetDlgItem(IDC_DOC_MODIFY)->EnableWindow(TRUE);
	GetDlgItem(IDC_EXTRACT_CONTENTS)->EnableWindow(TRUE);
	GetDlgItem(IDC_MOD_ANNOT)->EnableWindow(TRUE);
	GetDlgItem(IDC_FILL_FORMS)->EnableWindow(TRUE);
	GetDlgItem(IDC_ACCESS_SUPPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ASSEMBLE_DOC)->EnableWindow(TRUE);
	GetDlgItem(IDC_HIGH_REZ_PRINT)->EnableWindow(TRUE);

	// Selectively disable some permissions that are not present in older revisions
	switch(m_rev_num) 
	{
	case 2:
		GetDlgItem(IDC_FILL_FORMS)->EnableWindow(FALSE);
		GetDlgItem(IDC_ACCESS_SUPPORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_ASSEMBLE_DOC)->EnableWindow(FALSE);
		GetDlgItem(IDC_HIGH_REZ_PRINT)->EnableWindow(FALSE);
	}
}

void CEditSecurityHandler::OnUserPswdChk() 
{
	if (m_user_pass_req) 
	{
		GetDlgItem(IDC_USER_PSWD_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER_PSWD_EDIT)->SetWindowText(_T(""));
		m_user_pass_req = false;
	}
	else 
	{
		GetDlgItem(IDC_USER_PSWD_EDIT)->EnableWindow(TRUE);
		m_user_pass_req = true;
	}
}

void CEditSecurityHandler::OnOwnerPswdChk() 
{
	if (m_owner_pass_req) 
	{
		GetDlgItem(IDC_OWNER_PSWD_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_OWNER_PSWD_EDIT)->SetWindowText(_T(""));
		m_owner_pass_req = false;
	}
	else 
	{
		GetDlgItem(IDC_OWNER_PSWD_EDIT)->EnableWindow(TRUE);
		m_owner_pass_req = true;
	}
}

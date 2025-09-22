// DocPropSecurity.cpp : implementation file
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "DocPropSecurity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocPropSecurity property page

IMPLEMENT_DYNCREATE(CDocPropSecurity, CPropertyPage)

CDocPropSecurity::CDocPropSecurity() : CPropertyPage(CDocPropSecurity::IDD)
{
	//{{AFX_DATA_INIT(CDocPropSecurity)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_is_secured = false;
}

CDocPropSecurity::~CDocPropSecurity()
{
}

void CDocPropSecurity::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocPropSecurity)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDocPropSecurity, CPropertyPage)
	//{{AFX_MSG_MAP(CDocPropSecurity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocPropSecurity message handlers

BOOL CDocPropSecurity::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if (m_is_secured && m_hdlr) 
	{
		GetDlgItem(IDC_SECURITY_METHOD)->SetWindowText(_T("Standard Security Handler."));

		{	// Set Encryption level
			CString enc_level;
			int rev_num = m_hdlr.GetRevisionNumber();
			if (rev_num < 4) {
				char tmp_buf[32];
				sprintf(tmp_buf, "%d-bit RC4.", m_hdlr.GetKeyLength() * 8);
				enc_level += tmp_buf;
			}
			else {
				enc_level += "Crypt Filters using 128 bit ";
				if (m_hdlr.IsAES()) enc_level += "AES";
				if (m_hdlr.IsAES() && m_hdlr.IsRC4()) enc_level += " and ";
				if (m_hdlr.IsRC4()) enc_level += "RC4";
				enc_level += ".";
			}
#ifdef _UNICODE
			GetDlgItem(IDC_ENCRYPTION_LEVEL)->SetWindowText(enc_level);
#else
			GetDlgItem(IDC_ENCRYPTION_LEVEL)->SetWindowText((LPCSTR) enc_level);
#endif
		}

		if (m_hdlr.IsUserPasswordRequired()) 
			GetDlgItem(IDC_USER_PWD)->SetWindowText(_T("Yes"));
		else
			GetDlgItem(IDC_USER_PWD)->SetWindowText(_T("No"));

		if (m_hdlr.IsMasterPasswordRequired())
			GetDlgItem(IDC_OWNER_PWD)->SetWindowText(_T("Yes"));
		else
			GetDlgItem(IDC_OWNER_PWD)->SetWindowText(_T("No"));

		if (m_hdlr.Authorize(SDF::SecurityHandler::e_print)) 
			GetDlgItem(IDC_PRINT)->SetWindowText(_T("Allowed"));
		else 
			GetDlgItem(IDC_PRINT)->SetWindowText(_T("Not Allowed"));

		if (m_hdlr.Authorize(SDF::SecurityHandler::e_doc_modify)) 
			GetDlgItem(IDC_DOC_MODIFY)->SetWindowText(_T("Allowed"));
		else 
			GetDlgItem(IDC_DOC_MODIFY)->SetWindowText(_T("Not Allowed"));

		if (m_hdlr.Authorize(SDF::SecurityHandler::e_mod_annot)) 
			GetDlgItem(IDC_MOD_ANNOT)->SetWindowText(_T("Allowed"));
		else 
			GetDlgItem(IDC_MOD_ANNOT)->SetWindowText(_T("Not Allowed"));

		if (m_hdlr.Authorize(SDF::SecurityHandler::e_extract_content)) 
			GetDlgItem(IDC_EXTRACT_CONTENTS)->SetWindowText(_T("Allowed"));
		else 
			GetDlgItem(IDC_EXTRACT_CONTENTS)->SetWindowText(_T("Not Allowed"));

		if (m_hdlr.GetRevisionNumber() == 2)
		{
			GetDlgItem(IDC_HIGH_REZ_PRINT)->SetWindowText(_T("Not Applicable"));
			GetDlgItem(IDC_FILL_FORMS)->SetWindowText(_T("Not Applicable"));
			GetDlgItem(IDC_ACCESS_SUPPORT)->SetWindowText(_T("Not Applicable"));
			GetDlgItem(IDC_ASSEMBLE_DOC2)->SetWindowText(_T("Not Applicable"));
			
		}
		else 
		{
			if (m_hdlr.Authorize(SDF::SecurityHandler::e_print_high)) 
				GetDlgItem(IDC_HIGH_REZ_PRINT)->SetWindowText(_T("Allowed"));
			else 
				GetDlgItem(IDC_HIGH_REZ_PRINT)->SetWindowText(_T("Not Allowed"));

			if (m_hdlr.Authorize(SDF::SecurityHandler::e_fill_forms)) 
				GetDlgItem(IDC_FILL_FORMS)->SetWindowText(_T("Allowed"));
			else 
				GetDlgItem(IDC_FILL_FORMS)->SetWindowText(_T("Not Allowed"));

			if (m_hdlr.Authorize(SDF::SecurityHandler::e_access_support)) 
				GetDlgItem(IDC_ACCESS_SUPPORT)->SetWindowText(_T("Allowed"));
			else 
				GetDlgItem(IDC_ACCESS_SUPPORT)->SetWindowText(_T("Not Allowed"));
		
			if (m_hdlr.Authorize(SDF::SecurityHandler::e_assemble_doc)) 
				GetDlgItem(IDC_ASSEMBLE_DOC2)->SetWindowText(_T("Allowed"));
			else 
				GetDlgItem(IDC_ASSEMBLE_DOC2)->SetWindowText(_T("Not Allowed"));
		}
	}
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

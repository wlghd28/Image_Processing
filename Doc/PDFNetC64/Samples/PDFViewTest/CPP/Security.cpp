// DocSecurity.cpp : implementation file
//

#include "stdafx.h"
#include "Security.h"
#include <PDF/PDFNet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocSecurity dialog


CDocSecurity::CDocSecurity(PDF::PDFDoc& doc, CWnd* pParent /*=NULL*/)
	: CDialog(CDocSecurity::IDD, pParent), 
	mr_doc(doc)
{
	//{{AFX_DATA_INIT(CDocSecurity)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDocSecurity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocSecurity)
	DDX_Control(pDX, IDC_SEC_HANDLER_CHOICE, m_sec_handler);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDocSecurity, CDialog)
	//{{AFX_MSG_MAP(CDocSecurity)
	ON_CBN_SELCHANGE(IDC_SEC_HANDLER_CHOICE, OnSecHandlerChange)
	ON_BN_CLICKED(IDC_CHANGE_SECUR_SETTINGS, OnChangeSecurSettings)
	ON_BN_CLICKED(IDC_VIEW_SETTINGS, OnViewSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocSecurity message handlers

BOOL CDocSecurity::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// By default 'No security' is selected
	m_initial_selection = m_selected = 0;	
	m_sec_handler.AddString(_T("No security"));

	Common::Iterator<PDFNet::SecurityDescriptor> itr = PDFNet::GetSecHdlrInfoIterator();
	SDF::SecurityHandler hdlr = mr_doc.GetSecurityHandler();
	for (int i=1; itr.HasNext(); itr.Next(), ++i)
	{
		m_sec_handler.AddString(CString(itr.Current().GetGuiName()));

		if (hdlr != 0)
		{
			if (!strcmp(hdlr.GetHandlerDocName(), itr.Current().GetName()))	
			{
				m_initial_selection = m_selected = i;	// Set the initial selection
			}
		}
	}

	if (m_selected == 0) 
	{
		GetDlgItem(IDC_CHANGE_SECUR_SETTINGS)->EnableWindow(FALSE);
	}


	m_sec_handler.SetCurSel(m_selected);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDocSecurity::OnSecHandlerChange() 
{
	int idx = m_sec_handler.GetCurSel();

    if (idx == CB_ERR) return;
	if (m_selected == idx) return;
	
	if (idx == 0)
	{
		if (MessageBox(_T("Are you sure you want to remove security from this document?"), _T("Document security"), MB_ICONEXCLAMATION | MB_OKCANCEL) 
			== IDOK) 
		{
			GetDlgItem(IDC_CHANGE_SECUR_SETTINGS)->EnableWindow(FALSE);
			m_selected = 0;
			m_new_sec_hdlr = SDF::SecurityHandler();
		}
	}
	else  // Find the new handler
	{
		Common::Iterator<PDFNet::SecurityDescriptor> itr = PDFNet::GetSecHdlrInfoIterator();
		for (int i=1; itr.HasNext(); itr.Next(), ++i)
		{
			if (idx == i)
			{
				SDF::SecurityHandler hdlr = mr_doc.GetSecurityHandler();
				if (hdlr!=0 && !strcmp(hdlr.GetHandlerDocName(), itr.Current().GetName())) {	
					m_new_sec_hdlr = hdlr;
				}
				else {
					TRN_SecurityHandler sec_hdlr = itr.Current().m_factory_method(itr.Current().GetName(), 128, 2, 0);

					void* derived = 0;
					TRN_SecurityHandlerDerivedDestroyProc d = 0;
					TRN_SecurityHandlerDerivedCloneProc c = 0;
					TRN_SecurityHandlerAuthProc a = 0;
					TRN_SecurityHandlerAuthFailedProc af = 0;
					TRN_SecurityHandlerGetAuthDataProc ga = 0;
					TRN_SecurityHandlerEditSecurDataProc es = 0;
					TRN_SecurityHandlerFillEncryptDictProc fd = 0;

					TRN_SecurityHandlerGetDerived(sec_hdlr, &derived, &d, &c, &a, &af, &ga, &es, &fd);

					UInt32 deriv_procs = 0;
					if (derived) {
						assert(d && c); // Every derived class must implement the Clone() method.
						if (c) deriv_procs |= SDF::SecurityHandler::has_CloneProc;
						if (a) deriv_procs |= SDF::SecurityHandler::has_AuthProc;
						if (af) deriv_procs |= SDF::SecurityHandler::has_AuthFailedProc;
						if (ga) deriv_procs |= SDF::SecurityHandler::has_GetAuthDataProc;
						if (es) deriv_procs |= SDF::SecurityHandler::has_EditSecurDataProc;
						if (fd) deriv_procs |= SDF::SecurityHandler::has_FillEncDictProc;
					}

					m_new_sec_hdlr = SDF::SecurityHandler(sec_hdlr, true, deriv_procs);
				}

				// edit the security handler.
				if (m_new_sec_hdlr.EditSecurityData(mr_doc)) {
					GetDlgItem(IDC_CHANGE_SECUR_SETTINGS)->EnableWindow(TRUE);
					m_selected = i;
				}
			}
		}
	}

	// No change in selection
	m_sec_handler.SetCurSel(m_selected);

}

void CDocSecurity::OnChangeSecurSettings() 
{
	assert (m_selected > 0);

	// Get permission data and possibly authorize user to edit security settings.
	// if (m_sec_hdlr->GetPermission(mr_doc, SDF::SecurityHandler::e_owner))
	{
		if (m_initial_selection != m_selected) 
		{		// edit new security handler
			m_new_sec_hdlr.EditSecurityData(mr_doc);
		}
		else	// edit current security handler
		{
			SDF::SecurityHandler hdlr = mr_doc.GetSecurityHandler();
			hdlr.EditSecurityData(mr_doc);
		}
	}
}

void CDocSecurity::OnOK() 
{
	if (m_initial_selection != m_selected) {
		if (m_selected == 0) {
			mr_doc.RemoveSecurity();
		}
		else {
			mr_doc.SetSecurityHandler(m_new_sec_hdlr);
		}
	}

	CDialog::OnOK();
}

#include "DocPropSecurity.h"

void CDocSecurity::OnViewSettings() 
{
	CPropertySheet doc_prop(_T("Document Properties"));
	CDocPropSecurity security;

	if (mr_doc.GetSecurityHandler() !=0) { 
		security.m_is_secured = true;
		security.m_hdlr = mr_doc.GetSecurityHandler();
	}

	doc_prop.AddPage(&security);
	if (doc_prop.DoModal() == IDOK)
	{
	}
}


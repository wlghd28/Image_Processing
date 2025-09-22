// AuthorizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "AuthorizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAuthorize dialog


CAuthorize::CAuthorize(CString dlg_title,
					   CString static_title, 
					   CWnd* pParent /*=NULL*/)
	: CDialog(CAuthorize::IDD, pParent)
{
	m_static_title = static_title;
	m_dlg_title = dlg_title;

	//{{AFX_DATA_INIT(CAuthorize)
	m_password = _T("");
	//}}AFX_DATA_INIT
}


void CAuthorize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAuthorize)
	DDX_Text(pDX, ID_PASSWORD, m_password);
	DDV_MaxChars(pDX, m_password, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAuthorize, CDialog)
	//{{AFX_MSG_MAP(CAuthorize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAuthorize message handlers

BOOL CAuthorize::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_dlg_title);
	GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(m_static_title);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ErrorReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "ErrorReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CErrorReportDlg dialog


CErrorReportDlg::CErrorReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CErrorReportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CErrorReportDlg)
	m_error_msg = _T("");
	m_detail_msg = _T("");
	//}}AFX_DATA_INIT
}


void CErrorReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrorReportDlg)
	DDX_Text(pDX, IDC_ERROR_MSG, m_error_msg);
	DDX_Text(pDX, IDC_DETAIL_MSG, m_detail_msg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CErrorReportDlg, CDialog)
	//{{AFX_MSG_MAP(CErrorReportDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CErrorReportDlg message handlers

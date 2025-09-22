// DocPropCreation.cpp : implementation file
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "DocPropCreation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocPropCreation property page

IMPLEMENT_DYNCREATE(CDocPropCreation, CPropertyPage)

CDocPropCreation::CDocPropCreation() : CPropertyPage(CDocPropCreation::IDD)
{
	//{{AFX_DATA_INIT(CDocPropCreation)
	m_autor = _T("");
	m_created_date = _T("");
	m_creator = _T("");
	m_file = _T("");
	m_keywords = _T("");
	m_mod_date = _T("");
	m_producer = _T("");
	m_security = _T("");
	m_subject = _T("");
	m_title = _T("");
	//}}AFX_DATA_INIT

	m_is_data_modified = false; 
}

CDocPropCreation::~CDocPropCreation()
{
}

void CDocPropCreation::DoDataExchange(CDataExchange* pDX)
{
	CString	m_tmp_title = m_title;
	CString	m_tmp_subject = m_subject;
	CString	m_tmp_autor = m_autor;
	CString	m_tmp_keywords = m_keywords;
	CString	m_tmp_creator = m_creator;
	CString	m_tmp_producer = m_producer;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocPropCreation)
	DDX_Text(pDX, IDC_AUTHOR, m_autor);
	DDV_MaxChars(pDX, m_autor, 256);
	DDX_Text(pDX, IDC_CREATED_DATE, m_created_date);
	DDV_MaxChars(pDX, m_created_date, 256);
	DDX_Text(pDX, IDC_CREATOR, m_creator);
	DDV_MaxChars(pDX, m_creator, 256);
	DDX_Text(pDX, IDC_FILE, m_file);
	DDV_MaxChars(pDX, m_file, 1024);
	DDX_Text(pDX, IDC_KEYWORDS, m_keywords);
	DDV_MaxChars(pDX, m_keywords, 1024);
	DDX_Text(pDX, IDC_MOD_DATE, m_mod_date);
	DDV_MaxChars(pDX, m_mod_date, 256);
	DDX_Text(pDX, IDC_PRODUCER, m_producer);
	DDV_MaxChars(pDX, m_producer, 256);
	DDX_Text(pDX, IDC_SECURITY, m_security);
	DDV_MaxChars(pDX, m_security, 256);
	DDX_Text(pDX, IDC_SUBJECT, m_subject);
	DDV_MaxChars(pDX, m_subject, 1024);
	DDX_Text(pDX, IDC_TITLE, m_title);
	DDV_MaxChars(pDX, m_title, 1024);
	//}}AFX_DATA_MAP

	if (m_tmp_title != m_title ||
		m_tmp_subject != m_subject ||
		m_tmp_autor != m_autor ||
		m_tmp_keywords != m_keywords ||
		m_tmp_creator != m_creator ||
		m_tmp_producer != m_producer)
	{
		m_is_data_modified = true;
	}
}


BEGIN_MESSAGE_MAP(CDocPropCreation, CPropertyPage)
	//{{AFX_MSG_MAP(CDocPropCreation)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocPropCreation message handlers

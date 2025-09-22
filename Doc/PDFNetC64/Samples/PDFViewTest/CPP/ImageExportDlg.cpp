// ImageExportDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "PDFView.h"
#include "ImageExportDlg.h"
#include ".\imageexportdlg.h"


// CImageExportDlg dialog

IMPLEMENT_DYNAMIC(CImageExportDlg, CDialog)
CImageExportDlg::CImageExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageExportDlg::IDD, pParent)
	, m_dpi(0)
	, m_format(0)
{
}

CImageExportDlg::~CImageExportDlg()
{
}

void CImageExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DPI, m_dpi);
	DDV_MinMaxUInt(pDX, m_dpi, 1, 600);
}


BEGIN_MESSAGE_MAP(CImageExportDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CImageExportDlg message handlers

void CImageExportDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

// Options.cpp : implementation file
//

#include "stdafx.h"
#include "Options.h"


// COptions dialog

IMPLEMENT_DYNAMIC(COptions, CDialog)
COptions::COptions(CWnd* pParent /*=NULL*/)
	: CDialog(COptions::IDD, pParent)
	, m_gamma(0)
{
}

COptions::~COptions()
{
}

void COptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GAMMA, m_gamma);
	DDV_MinMaxDouble(pDX, m_gamma, 0.1, 3.0);
}


BEGIN_MESSAGE_MAP(COptions, CDialog)
END_MESSAGE_MAP()


// COptions message handlers

#pragma once

#include ".\resource.h"

// COptions dialog

class COptions : public CDialog
{
	DECLARE_DYNAMIC(COptions)

public:
	COptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptions();

// Dialog Data
	enum { IDD = IDD_OPTIONS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_gamma;
};

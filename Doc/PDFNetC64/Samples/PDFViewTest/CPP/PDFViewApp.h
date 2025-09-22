// PDFView.h : main header file for the PDFView application
//
#pragma once

//#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
//#endif

#include "resource.h"       // main symbols
#include <afxwinappex.h>

// CPDFViewApp:
// See PDFView.cpp for the implementation of this class
//

class CPDFViewApp : public CWinAppEx
{
public:
	CPDFViewApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
//	afx_msg void OnUpdateSmallthumbnails(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateLargethumbnails(CCmdUI *pCmdUI);
	afx_msg void OnFileCloseall();
	afx_msg void OnFileSaveAs();
};

extern CPDFViewApp theApp;

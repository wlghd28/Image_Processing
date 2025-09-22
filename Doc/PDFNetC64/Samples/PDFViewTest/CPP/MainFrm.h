#pragma once

#include "TrueColorToolBar.h"
#include <afxmdiframewndex.h>

class CPageNumEditCtrl : public CEdit {
public:
	DECLARE_DYNAMIC(CPageNumEditCtrl)
	CPageNumEditCtrl() {}
	virtual ~CPageNumEditCtrl() {}
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	int _page_num;
#ifdef _UNICODE
    wchar_t _buf[256];
#else
	char _buf[256];
#endif

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;

	CTrueColorToolBar    m_basic_bar;
	CTrueColorToolBar    m_forms_bar;
	CTrueColorToolBar    m_misc_bar;
	CTrueColorToolBar    m_fopen_bar;
	CTrueColorToolBar    m_pagemode_bar;
	CTrueColorToolBar    m_nav_bar;
public:

	CPageNumEditCtrl  m_page_num;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	bool SetPageNum(int page_num, bool reset = false);

	afx_msg void OnSelectall();
	afx_msg void OnUpdateBasicToolSelection(CCmdUI* pCmdUI);
	afx_msg void OnChangePageNumber();
	afx_msg void OnCancelPageNumber();

protected:
};

CMainFrame* GetMainFrame();

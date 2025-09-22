// LeftView.h : interface of the CLeftView class
//

#pragma once

#pragma warning(disable:4786)
class CPDFViewDoc;

#include <PDF/PDFView.h>
#include <afxcview.h>
#include ".\TabView.h"


// This view will be used for bookmark outline and other page navigation tools.

class CLeftView : public CTabView
{
protected: // create from serialization only
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
	CPDFViewDoc* GetDocument();

// Operations
public:
	void SetTargetPDFView(pdftron::PDF::PDFView& view, CSplitterWnd* s);

// Overrides
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CLeftView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	pdftron::PDF::PDFView* mp_view;
	CSplitterWnd* mp_wndSplitter;

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CPDFViewDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CPDFViewDoc*>(m_pDocument); }
#endif


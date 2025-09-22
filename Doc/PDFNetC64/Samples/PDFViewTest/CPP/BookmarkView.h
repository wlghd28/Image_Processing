// LeftView.h : interface of the CBookmarkView class
//

#pragma once

#pragma warning(disable:4786)
class CPDFViewDoc;

#include <PDF/PDFView.h>
#include <afxcview.h>

// This view will be used for bookmark outline and other page navigation tools.

class CBookmarkView : public CTreeView
{
protected: // create from serialization only
	CBookmarkView();
	DECLARE_DYNCREATE(CBookmarkView)

// Attributes
public:
	CPDFViewDoc* GetDocument();

// Operations
public:
	void SetTargetPDFView(pdftron::PDF::PDFView& view);

// Overrides
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CBookmarkView();

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
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CPDFViewDoc* CBookmarkView::GetDocument()
   { return reinterpret_cast<CPDFViewDoc*>(m_pDocument); }
#endif


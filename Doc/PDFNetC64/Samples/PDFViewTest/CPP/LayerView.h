// BookmarkView.h : interface of the CLayerView class
//

#pragma once

#pragma warning(disable:4786)
class CPDFViewDoc;

#include <PDF/PDFView.h>
#include <afxcview.h>

// This view will be used for bookmark outline and other page navigation tools.

class CLayerView : public CTreeView
{
protected: // create from serialization only
	CLayerView();
	DECLARE_DYNCREATE(CLayerView)

// Attributes
public:
	CPDFViewDoc* GetDocument();

// Operations
public:
	void SetTargetPDFView(pdftron::PDF::PDFView& view);

// Overrides
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:

// Implementation
public:
	virtual ~CLayerView();

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
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
protected:
};

#ifndef _DEBUG  // debug version
inline CPDFViewDoc* CLayerView::GetDocument()
   { return reinterpret_cast<CPDFViewDoc*>(m_pDocument); }
#endif


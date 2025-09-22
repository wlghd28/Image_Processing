// PDFViewView.h : interface of the CPDFViewView class
//
#pragma once

#include <PDF/PDFView.h>
#include <PDF/PDFDraw.h>
#include <PDF/PDFRasterizer.h>

using namespace pdftron;

class DrawTimer {
public:
	DrawTimer() : hwnd(0), timer(0) {}
	HWND hwnd;
	UINT_PTR timer;
	DWORD dwTime;
};

class CPDFViewView : public CView
{
protected: // create from serialization only
	CPDFViewView();
	DECLARE_DYNCREATE(CPDFViewView)

public:
	// enum{ IDD = IDD_PDFView_FORM };

// Attributes
public:
	CPDFViewDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

	void InitBitmapInfo(int width, int height, int stride, int bits_per_pixel);
	void SetScrolls();

// Implementation
public:
	virtual ~CPDFViewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	int GetCurrentPageNum() { return m_view.GetCurrentPage(); }

	PDF::PDFView m_view;

private:

	BITMAPINFO m_bmp;
	int m_scroll_pix_inc;

	bool m_panning;
	CPoint m_cur_point;

	DrawTimer m_draw_timer;

	HCURSOR m_hand_cur;
	HCURSOR m_drag_cur;

	bool m_anti_alias, m_smooth_images, m_builtin_rasterizer;
	bool m_pixel_grid_fit, m_stroke_adjust;
	bool m_show_trans_grid;
	int m_overprint;

	// Printing.
	PDF::Rect GetPageDimensions(CDC* pDC, bool use_physical_page);
	PDF::PDFDraw m_pdfdraw;   
	
// Generated message map functions
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnDraw(CDC* /*pDC*/);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	// Print support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

public:
	void OnUpdatePageViewPresentation(CCmdUI* pCmdUI);
	void OnPageViewPresentation(UINT nID);
	void OnUpdateViewNextPage(CCmdUI* pCmdUI);
	void OnViewNextPage();
	void OnUpdateViewPrevPage(CCmdUI* pCmdUI);
	void OnViewPrevPage();
	void OnUpdateViewFirstPage(CCmdUI* pCmdUI);
	void OnViewFirstPage();
	void OnUpdateViewLastPage(CCmdUI* pCmdUI);
	void OnViewLastPage();
	bool SetCurrentPage(int page_num);
	void OnAntiAlias();
	void OnUpdateAntiAlias(CCmdUI* pCmdUI);
	void OnThinLinePixelGridFit();
	void OnUpdateThinLinePixelGridFit(CCmdUI* pCmdUI);
	void OnOverprint(UINT nID);
	void OnUpdateOverprint(CCmdUI* pCmdUI);
	void OnShowTransGrid();
	void OnUpdateShowTransGrid(CCmdUI* pCmdUI);
	void OnThinLineStrokeAdjust();
	void OnUpdateThinLineStrokeAdjust(CCmdUI* pCmdUI);
	void OnSmoothImages();
	void OnUpdateSmoothImages(CCmdUI* pCmdUI);
	void OnToggleRasterizer();
	void OnUpdateRasterizerOption(CCmdUI* pCmdUI);

	void OnViewZoomIn();
	void OnViewZoomOut();
	void OnViewFitPage();
	void OnViewFitPageWidth();
	void OnViewRotatePagesClockwise();
	void OnViewRotatePagesCounterClockwise();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnOptionsGamma();

	afx_msg LRESULT OnProgressiveRender(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // debug version in PDFViewView.cpp
inline CPDFViewDoc* CPDFViewView::GetDocument() const
   { return reinterpret_cast<CPDFViewDoc*>(m_pDocument); }
#endif


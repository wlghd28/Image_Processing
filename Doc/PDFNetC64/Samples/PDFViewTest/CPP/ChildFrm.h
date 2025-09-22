// ChildFrm.h : interface of the CChildFrame class
//


#pragma once

class CPDFViewView;

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
public:
	CPDFViewView* GetPDFView();

// Operations
public:

// Overrides
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CPDFViewView* GetRightPane();
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:

	bool SetCurrentPage(int page_num);

	afx_msg void OnPageViewPresentation(UINT nID);
	afx_msg void OnViewNextPage();
	afx_msg void OnUpdatePageViewPresentation(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewNextPage(CCmdUI* pCmdUI);

	afx_msg void OnUpdateViewPrevPage(CCmdUI* pCmdUI);
	afx_msg void OnViewPrevPage();
	afx_msg void OnUpdateViewFirstPage(CCmdUI* pCmdUI);
	afx_msg void OnViewFirstPage();
	afx_msg void OnUpdateViewLastPage(CCmdUI* pCmdUI);
	afx_msg void OnViewLastPage();

	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewFitPage();
	afx_msg void OnViewFitPageWidth();

	afx_msg void OnViewRotatePagesClockwise();
	afx_msg void OnViewRotatePagesCounterClockwise();

	afx_msg void OnAntiAlias();
	afx_msg void OnUpdateAntiAlias(CCmdUI* pCmdUI);
	afx_msg void OnThinLinePixelGridFit();
	afx_msg void OnUpdateThinLinePixelGridFit(CCmdUI* pCmdUI);
	afx_msg void OnThinLineStrokeAdjust();
	afx_msg void OnUpdateThinLineStrokeAdjust(CCmdUI* pCmdUI);
	afx_msg void OnSmoothImages();
	afx_msg void OnUpdateSmoothImages(CCmdUI* pCmdUI);
	afx_msg void OnShowTransGrid();
	afx_msg void OnUpdateShowTransGrid(CCmdUI* pCmdUI);
	afx_msg void OnOverprint(UINT nID);
	afx_msg void OnUpdateOverprint(CCmdUI* pCmdUI);
	afx_msg void OnToggleRasterizer();
	afx_msg void OnUpdateRasterizerOption(CCmdUI* pCmdUI);

	afx_msg void OnFileExport();

	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
};

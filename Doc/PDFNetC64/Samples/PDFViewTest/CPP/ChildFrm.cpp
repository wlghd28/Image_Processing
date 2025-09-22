// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "PDFViewApp.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "LeftView.h"
#include "PDFViewView.h"
#include "ImageExportDlg.h"
#include "PDFViewDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)

ON_COMMAND_RANGE(ID_VIEW_SINGLE_PAGE, ID_VIEW_FACING, OnPageViewPresentation)
ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_SINGLE_PAGE, ID_VIEW_FACING, OnUpdatePageViewPresentation)

ON_COMMAND(ID_VIEW_NEXT_PAGE, OnViewNextPage)
ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT_PAGE, OnUpdateViewNextPage)

ON_COMMAND(ID_VIEW_PREVIOUS_PAGE, OnViewPrevPage)
ON_UPDATE_COMMAND_UI(ID_VIEW_PREVIOUS_PAGE, OnUpdateViewPrevPage)

ON_COMMAND(ID_VIEW_FIRST_PAGE, OnViewFirstPage)
ON_UPDATE_COMMAND_UI(ID_VIEW_FIRST_PAGE, OnUpdateViewFirstPage)

ON_COMMAND(ID_VIEW_LAST_PAGE, OnViewLastPage)
ON_UPDATE_COMMAND_UI(ID_VIEW_LAST_PAGE, OnUpdateViewLastPage)

ON_COMMAND(ID_OPTIONS_SMOOTHIMAGES, OnSmoothImages)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SMOOTHIMAGES, OnUpdateSmoothImages)

ON_COMMAND(ID_OPTIONS_ANTI_ALIAS, OnAntiAlias)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_ANTI_ALIAS, OnUpdateAntiAlias)

ON_COMMAND(ID_OPTIONS_GRID_FIT, OnThinLinePixelGridFit)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GRID_FIT, OnUpdateThinLinePixelGridFit)

ON_COMMAND(ID_OPTIONS_SHOW_TRANS_GRID, OnShowTransGrid)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOW_TRANS_GRID, OnUpdateShowTransGrid)

ON_COMMAND_RANGE(ID_OVERPRINT_OFF, ID_OVERPRINT_PDFX, OnOverprint)
ON_UPDATE_COMMAND_UI_RANGE(ID_OVERPRINT_OFF, ID_OVERPRINT_PDFX, OnUpdateOverprint)

ON_COMMAND(ID_OPTIONS_STROKE_ADJUST, OnThinLineStrokeAdjust)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_STROKE_ADJUST, OnUpdateThinLineStrokeAdjust)

ON_COMMAND(ID_OPTIONS_RASTERIZER, OnToggleRasterizer)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_RASTERIZER, OnUpdateRasterizerOption)

ON_COMMAND(ID_VIEW_ZOOM_ZOOMIN, OnViewZoomIn)
ON_COMMAND(ID_VIEW_ZOOM_ZOOMOUT, OnViewZoomOut)
ON_COMMAND(ID_VIEW_ZOOM_FITWHOLEPAGE, OnViewFitPage)
ON_COMMAND(ID_VIEW_ZOOM_FITWIDTH, OnViewFitPageWidth)

ON_COMMAND(ID_VIEW_ROTATE_CLOCKWISE, OnViewRotatePagesClockwise)
ON_COMMAND(ID_VIEW_ROTATE_COUNTERCLOCKWISE, OnViewRotatePagesCounterClockwise)

ON_COMMAND(ID_FILE_EXPORT, OnFileExport)

ON_WM_MDIACTIVATE()
ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	m_wndSplitter.SetFocus();
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(0, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CPDFViewView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	m_wndSplitter.SetActivePane(0, 1);

	CLeftView* bookmark_view = static_cast<CLeftView*>(m_wndSplitter.GetPane(0, 0));
	CPDFViewView* pdf_view = GetPDFView();

	bookmark_view->SetTargetPDFView(pdf_view->m_view, &m_wndSplitter);

	return TRUE;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style |= WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_MAXIMIZE; 
	return CMDIChildWnd::PreCreateWindow(cs);
}


// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


CPDFViewView* CChildFrame::GetPDFView()
{
	return static_cast<CPDFViewView*>(m_wndSplitter.GetPane(0, 1));
}

void CChildFrame::OnPageViewPresentation(UINT nID)
{
	CPDFViewView* view = GetPDFView();
	if (view) view->OnPageViewPresentation(nID);
}

void CChildFrame::OnUpdatePageViewPresentation(CCmdUI* pCmdUI)
{
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdatePageViewPresentation(pCmdUI);
}

void CChildFrame::OnViewNextPage() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewNextPage();
}

void CChildFrame::OnUpdateViewNextPage(CCmdUI* pCmdUI)
{
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateViewNextPage(pCmdUI);
}

void CChildFrame::OnUpdateViewPrevPage(CCmdUI* pCmdUI) {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateViewPrevPage(pCmdUI);
}

void CChildFrame::OnViewPrevPage(){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewPrevPage();
}

void CChildFrame::OnUpdateViewFirstPage(CCmdUI* pCmdUI){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateViewFirstPage(pCmdUI);
}
void CChildFrame::OnViewFirstPage(){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewFirstPage();
}

void CChildFrame::OnUpdateViewLastPage(CCmdUI* pCmdUI){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateViewLastPage(pCmdUI);
}
void CChildFrame::OnViewLastPage(){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewLastPage();
}

void CChildFrame::OnViewZoomIn() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewZoomIn();
}

void CChildFrame::OnViewZoomOut() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewZoomOut();
}

void CChildFrame::OnViewFitPage() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewFitPage();
}

void CChildFrame::OnViewFitPageWidth() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewFitPageWidth();
}

void CChildFrame::OnViewRotatePagesClockwise() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewRotatePagesClockwise();
}

void CChildFrame::OnViewRotatePagesCounterClockwise() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnViewRotatePagesCounterClockwise();
}

bool CChildFrame::SetCurrentPage(int page_num){
	CPDFViewView* view = GetPDFView();
	if (view) return view->SetCurrentPage(page_num);
	return false;
}

void CChildFrame::OnAntiAlias() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnAntiAlias();
}

void CChildFrame::OnUpdateAntiAlias(CCmdUI* pCmdUI){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateAntiAlias(pCmdUI);
}

void CChildFrame::OnThinLinePixelGridFit() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnThinLinePixelGridFit();
}

void CChildFrame::OnUpdateThinLinePixelGridFit(CCmdUI* pCmdUI){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateThinLinePixelGridFit(pCmdUI);
}

void CChildFrame::OnShowTransGrid()
{
	CPDFViewView* view = GetPDFView();
	if (view) view->OnShowTransGrid();
}

void CChildFrame::OnUpdateShowTransGrid(CCmdUI* pCmdUI)
{
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateShowTransGrid(pCmdUI);
}

void CChildFrame::OnOverprint(UINT nID)
{
	CPDFViewView* view = GetPDFView();
	if (view) view->OnOverprint(nID);
}

void CChildFrame::OnUpdateOverprint(CCmdUI* pCmdUI)
{
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateOverprint(pCmdUI);
}


void CChildFrame::OnThinLineStrokeAdjust() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnThinLineStrokeAdjust();
}

void CChildFrame::OnUpdateThinLineStrokeAdjust(CCmdUI* pCmdUI){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateThinLineStrokeAdjust(pCmdUI);
}

void CChildFrame::OnSmoothImages() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnSmoothImages();
}

void CChildFrame::OnUpdateSmoothImages(CCmdUI* pCmdUI){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateSmoothImages(pCmdUI);
}

void CChildFrame::OnToggleRasterizer() {
	CPDFViewView* view = GetPDFView();
	if (view) view->OnToggleRasterizer();
}

void CChildFrame::OnUpdateRasterizerOption(CCmdUI* pCmdUI){
	CPDFViewView* view = GetPDFView();
	if (view) view->OnUpdateRasterizerOption(pCmdUI);
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	CEdit& page_num_box = GetMainFrame()->m_page_num;
	if (bActivate)
	{
		CPDFViewView* view = GetPDFView();
		GetMainFrame()->SetPageNum(view->GetCurrentPageNum(), true);
		page_num_box.EnableWindow(TRUE);
	}
	else if (pActivateWnd == NULL)
	{
		page_num_box.SetWindowText(_T(""));
		page_num_box.EnableWindow(FALSE);
	}
}

void CChildFrame::OnFileExport() 
{
	CImageExportDlg export_dlg;
	if (export_dlg.DoModal() == IDOK) {
	}
}

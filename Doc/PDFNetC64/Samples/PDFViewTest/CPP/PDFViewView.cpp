// PDFViewView.cpp : implementation of the CPDFViewView class
//

#include "stdafx.h"
#include <cassert>

// Undefine this macro since it collides with Common::Exception::GetMessage()
#undef GetMessage	

#include "PDFViewApp.h"

#include "PDFViewDoc.h"
#include "PDFViewView.h"

#include ".\MainFrm.h"
#include ".\pdfviewview.h"
#include ".\Options.h"

#ifdef _DEBUG
  #define new DEBUG_NEW
#endif



// CPDFViewView

IMPLEMENT_DYNCREATE(CPDFViewView, CView)

BEGIN_MESSAGE_MAP(CPDFViewView, CView)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()

	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_OPTIONS_GAMMA, OnOptionsGamma)
	ON_MESSAGE(ID_PROGRESSIVE_RENDER, OnProgressiveRender)

END_MESSAGE_MAP()

// CPDFViewView construction/destruction

CPDFViewView::CPDFViewView()
	: CView(), 
	m_hand_cur(0), 
	m_drag_cur(0), 
	m_panning(false), 
	m_anti_alias(true),
	m_pixel_grid_fit(false),
	m_stroke_adjust(true),
	m_smooth_images(true),
	m_show_trans_grid(false),
	m_builtin_rasterizer(true),
	m_overprint(2)
{
	m_drag_cur = AfxGetApp()->LoadCursor(IDC_DRAG_CUR);
	m_hand_cur = AfxGetApp()->LoadCursor(IDC_HAND_CUR);

	// Uncomment to use GDI+ based rasterizer as a default.
	// m_view.SetRasterizerType(PDF::PDFRasterizer::e_GDIPlus);
}

CPDFViewView::~CPDFViewView()
{
}

void CPDFViewView::DoDataExchange(CDataExchange* pDX)
{
	CView::DoDataExchange(pDX);
}

BOOL CPDFViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CView::PreCreateWindow(cs);
}

#define DRAW_TIMER_ID 1

void CALLBACK EXPORT DrawTimerProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
	if (nIDEvent == DRAW_TIMER_ID) {
		::InvalidateRect(hWnd, 0, FALSE);
		::PostMessage(hWnd, ID_PROGRESSIVE_RENDER, 0, 0);
	}
}

LRESULT CPDFViewView::OnProgressiveRender(WPARAM wParam, LPARAM lParam)
{
	m_view.UpdateBuffer();
	return 0;
}

void BeginRendering(void* data) 
{
	DrawTimer* t = (DrawTimer*) data;
	t->dwTime = GetTickCount();
	t->timer = ::SetTimer(t->hwnd, DRAW_TIMER_ID, 200, DrawTimerProc);
}

void FinishedRendering(void* data, bool canceled) 
{	
	DrawTimer* t = (DrawTimer*) data;
	::KillTimer(t->hwnd, t->timer); 

	if (canceled) {
		if(GetTickCount() - t->dwTime < 10) {
			return;
		}
	}

	::InvalidateRect(t->hwnd, 0, FALSE);
	::PostMessage(t->hwnd, WM_PAINT, 0, 0);	// Last refresh.
}

void ErrorCallback(const char* message, void* custom_data) 
{
	static bool reported_once = false;
	if (!reported_once) {
		reported_once = true;
		::AfxMessageBox(CString(message), MB_OK);
	}
}

void CPDFViewView::OnInitialUpdate()
{
	try
	{
		ShowScrollBar(SB_BOTH);

		// Set the increment size for scrolling (70 increments per screen is the default).
		m_scroll_pix_inc = ::GetSystemMetrics(SM_CYFULLSCREEN) / 70;
		if (m_scroll_pix_inc < 5) m_scroll_pix_inc = 5; // Minimum scroll increment.

		PDF::PDFDoc* doc = GetDocument()->mp_doc.get();
		assert(doc); 

		m_view.SetDoc(*GetDocument()->mp_doc.get());
		// m_view.SetCaching(false);
		m_draw_timer.hwnd = GetSafeHwnd();
		m_draw_timer.timer = 0;
		m_view.SetRenderBeginProc(BeginRendering, &m_draw_timer);
		m_view.SetRenderFinishProc(FinishedRendering, &m_draw_timer);
		m_view.SetErrorReportProc(ErrorCallback, 0);			
		m_view.Update();

		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());

		CView::OnInitialUpdate();
	}
	catch (Common::Exception& ex) {		
		AfxMessageBox(CString(ex.GetMessage()), MB_ICONEXCLAMATION, 0);
	}
}


// CPDFViewView diagnostics

#ifdef _DEBUG
void CPDFViewView::AssertValid() const
{
	CView::AssertValid();
}

void CPDFViewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPDFViewDoc* CPDFViewView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPDFViewDoc)));
	return (CPDFViewDoc*)m_pDocument;
}
#endif //_DEBUG

void CPDFViewView::InitBitmapInfo(int width, int height, int stride, int bits_per_pixel)
{
    m_bmp.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
    m_bmp.bmiHeader.biWidth  = width;
    m_bmp.bmiHeader.biHeight = -height;
    m_bmp.bmiHeader.biPlanes = 1;
    m_bmp.bmiHeader.biBitCount = bits_per_pixel;
    m_bmp.bmiHeader.biCompression = 0;
    m_bmp.bmiHeader.biSizeImage = height * stride;
    m_bmp.bmiHeader.biXPelsPerMeter = 0;
    m_bmp.bmiHeader.biYPelsPerMeter = 0;
    m_bmp.bmiHeader.biClrUsed = 0;
    m_bmp.bmiHeader.biClrImportant = 0;
}

void CPDFViewView::SetScrolls()
{
	RECT rect;
	GetClientRect(&rect);
	SCROLLINFO scrollinfo;
    scrollinfo.cbSize = sizeof(SCROLLINFO);

    scrollinfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
	scrollinfo.nMin = 0;
    scrollinfo.nMax = int(m_view.GetCanvasWidth());
	scrollinfo.nPos = int(m_view.GetHScrollPos());
	scrollinfo.nPage = rect.right + 1;
	SetScrollInfo(SB_HORZ, &scrollinfo, TRUE);   

	scrollinfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
	scrollinfo.nMin = 0;
    scrollinfo.nMax = int(m_view.GetCanvasHeight());
	scrollinfo.nPos = int(m_view.GetVScrollPos());
	scrollinfo.nPage = rect.bottom + 1;
	SetScrollInfo(SB_VERT, &scrollinfo, TRUE);  
}

void CPDFViewView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	int comps = 4;
	int stride = ((cx * comps + 3) / 4) * 4;   
	InitBitmapInfo(cx, cy, stride, comps * 8);
	m_view.OnSize(cx, cy);
	SetScrolls();
	GetMainFrame()->SetPageNum(m_view.GetCurrentPage());
}

void CPDFViewView::OnDraw(CDC* pDC)
{
	if (GetDocument()->mp_doc.get()) // if the document is attached to this view.
	{
		const char* buf = m_view.GetBuffer();
		if (buf) 
		{
			::SetDIBitsToDevice(pDC->m_hDC, 
				0, 0,
				m_bmp.bmiHeader.biWidth, abs(m_bmp.bmiHeader.biHeight),
				0, 0, 0, 
				abs(m_bmp.bmiHeader.biHeight), 
				buf, 
				&m_bmp, 
				DIB_RGB_COLORS);
		}
	}
}

void CPDFViewView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{	
}

void CPDFViewView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    SCROLLINFO scrollinfo;
	scrollinfo.cbSize = sizeof(SCROLLINFO);
	scrollinfo.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &scrollinfo);

    switch (nSBCode) 
    {
    case SB_LINELEFT: 
        m_view.OnScroll(-m_scroll_pix_inc, 0);
        break;
    case SB_LINERIGHT: 
        m_view.OnScroll(m_scroll_pix_inc, 0);
        break;
    case SB_PAGELEFT: 
        m_view.OnScroll(-int(scrollinfo.nPage), 0);
        break;
    case SB_PAGERIGHT: 
        m_view.OnScroll(scrollinfo.nPage, 0);
        break;
    case SB_THUMBPOSITION: 
    case SB_THUMBTRACK:
        m_view.SetHScrollPos(scrollinfo.nTrackPos);
        break;
    }

	scrollinfo.nPos = int(m_view.GetHScrollPos());
	scrollinfo.cbSize = sizeof(SCROLLINFO);
	scrollinfo.fMask = SIF_POS;
	SetScrollInfo(SB_HORZ, &scrollinfo, TRUE);

	GetMainFrame()->SetPageNum(m_view.GetCurrentPage());
}

void CPDFViewView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO scrollinfo;
	scrollinfo.cbSize = sizeof(SCROLLINFO);
	scrollinfo.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &scrollinfo);

    switch (nSBCode) 
    {
	case SB_LINEUP:
		m_view.OnScroll(0, -m_scroll_pix_inc);
        break;
	case SB_LINEDOWN: 		
		m_view.OnScroll(0, m_scroll_pix_inc);
        break;
    case SB_PAGEUP: 		
		m_view.OnScroll(0, -int(scrollinfo.nPage));
        break;
	case SB_PAGEDOWN:
		m_view.OnScroll(0, scrollinfo.nPage);
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        m_view.SetVScrollPos(scrollinfo.nTrackPos);
        break;
    }

	scrollinfo.nPos = int(m_view.GetVScrollPos());
	scrollinfo.cbSize = sizeof(SCROLLINFO);
	scrollinfo.fMask = SIF_POS;
	SetScrollInfo(SB_VERT, &scrollinfo, TRUE);

	GetMainFrame()->SetPageNum(m_view.GetCurrentPage());
}

BOOL CPDFViewView::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	if ((fFlags & MK_CONTROL) == MK_CONTROL)
	{
		if (zDelta > 0)
		{
			OnViewZoomIn();
		}
		else if (zDelta < 0)
		{
			OnViewZoomOut();
		}
		return 1; // "Nonzero if mouse wheel scrolling is enabled, otherwise 0." It's always enabled, so return 1.
	}
	
	SCROLLINFO scrollinfo;
	scrollinfo.cbSize = sizeof(SCROLLINFO);
	scrollinfo.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &scrollinfo);

	m_view.SetVScrollPos(m_view.GetVScrollPos()-zDelta);

	scrollinfo.nPos = int(m_view.GetVScrollPos());
	scrollinfo.cbSize = sizeof(SCROLLINFO);
	scrollinfo.fMask = SIF_POS;
	SetScrollInfo(SB_VERT, &scrollinfo, TRUE);

	GetMainFrame()->SetPageNum(m_view.GetCurrentPage());

	return 1; // "Nonzero if mouse wheel scrolling is enabled, otherwise 0." It's always enabled, so return 1.
}

void CPDFViewView::OnPageViewPresentation(UINT nID)
{
	PDF::PDFView::PagePresentationMode prev_mode = m_view.GetPagePresentationMode();
	PDF::PDFView::PagePresentationMode new_mode = PDF::PDFView::e_single_continuous;
	switch (nID)
	{
	case ID_VIEW_SINGLE_PAGE:
		new_mode = PDF::PDFView::e_single_page;
		break;
	case ID_VIEW_SINGLE_CONT:
		new_mode = PDF::PDFView::e_single_continuous;
		break;
	case ID_VIEW_FACING_CONT:
		new_mode = PDF::PDFView::e_facing_continuous;
		break;
	case ID_VIEW_FACING:
		new_mode = PDF::PDFView::e_facing;
		break;
	}

	if (new_mode != prev_mode) 
	{
		m_view.SetPagePresentationMode(new_mode);
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());
	}
}

void CPDFViewView::OnUpdatePageViewPresentation(CCmdUI* pCmdUI)
{
	PDF::PDFView::PagePresentationMode p = m_view.GetPagePresentationMode();
	switch (pCmdUI->m_nID)
	{
	case ID_VIEW_SINGLE_PAGE: 
		{
			bool s = p == PDF::PDFView::e_single_page;
			pCmdUI->SetCheck(s);
		}
		break;
	case ID_VIEW_SINGLE_CONT:
		{
			bool s = p == PDF::PDFView::e_single_continuous;
			pCmdUI->SetCheck(s);
		}
		break;
	case ID_VIEW_FACING_CONT:
		{
			bool s = p == PDF::PDFView::e_facing_continuous;
			pCmdUI->SetCheck(s);
		}
		break;
	case ID_VIEW_FACING:
		{
			bool s = p == PDF::PDFView::e_facing;
			pCmdUI->SetCheck(s);
		}
		break;
	}
}

void CPDFViewView::OnUpdateViewNextPage(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(FALSE);
	pCmdUI->Enable(m_view.GetCurrentPage() < m_view.GetPageCount());
}

void CPDFViewView::OnViewNextPage() 
{
	if (m_view.GotoNextPage()) 
	{
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
	}
}

void CPDFViewView::OnUpdateViewPrevPage(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(FALSE);
	pCmdUI->Enable(m_view.GetCurrentPage() > 1);
}

void CPDFViewView::OnViewPrevPage() 
{
	if (m_view.GotoPreviousPage()) 
	{
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
	}
}

void CPDFViewView::OnUpdateViewFirstPage(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(FALSE);
	pCmdUI->Enable(m_view.GetCurrentPage() > 1 && m_view.GetPageCount() > 1);
}

void CPDFViewView::OnViewFirstPage() 
{
	if (m_view.GotoFirstPage()) 
	{
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
	}
}

void CPDFViewView::OnUpdateViewLastPage(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(FALSE);
	pCmdUI->Enable(m_view.GetCurrentPage() < m_view.GetPageCount());
}

void CPDFViewView::OnViewLastPage() 
{
	if (m_view.GotoLastPage()) 
	{
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
	}
}

bool CPDFViewView::SetCurrentPage(int page_num)
{
	if (m_view.SetCurrentPage(page_num)) 
	{
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
		return true;
	}

	return false;
}

void CPDFViewView::OnLButtonDown(UINT nFlags, CPoint point)
{
	::GetCursorPos(&m_cur_point);
	SetCapture();
	m_panning = true;

	assert(m_drag_cur);
	SetCursor(m_drag_cur);

	CView::OnLButtonDown(nFlags, point);
}

void CPDFViewView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_panning)
	{
		::GetCursorPos(&point);
		CPoint pt = m_cur_point - point;
		
		if (pt != CPoint(0, 0))
		{
			m_view.OnScroll(pt.x, pt.y);
			SetScrolls();
			GetMainFrame()->SetPageNum(m_view.GetCurrentPage());
			m_cur_point = point;
		}
	}

	CView::OnMouseMove(nFlags, point);
}

void CPDFViewView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_panning) {
		ReleaseCapture();
		m_panning = false;
	}

	CView::OnLButtonUp(nFlags, point);
}

BOOL CPDFViewView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		SetCursor(m_panning ? m_drag_cur : m_hand_cur);
		return TRUE;
	}

	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CPDFViewView::OnViewZoomIn() 
{
	if (m_view.SetZoom(m_view.GetZoom()*2)) 
	{
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
	}
}

void CPDFViewView::OnViewZoomOut() 
{
	if (m_view.SetZoom(m_view.GetZoom()/2)) 
	{
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
	}
}

void CPDFViewView::OnViewFitPage() 
{
	if (m_view.GetPageViewMode() != PDF::PDFView::e_fit_page) 
	{
		m_view.SetPageViewMode(PDF::PDFView::e_fit_page);
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
	}
}

void CPDFViewView::OnViewFitPageWidth() 
{
	if (m_view.GetPageViewMode() != PDF::PDFView::e_fit_width) 
	{
		m_view.SetPageViewMode(PDF::PDFView::e_fit_width);
		SetScrolls();
		GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
	}
}

void CPDFViewView::OnViewRotatePagesClockwise() {
	m_view.RotateClockwise();
	SetScrolls();
	GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
}

void CPDFViewView::OnViewRotatePagesCounterClockwise() {
	m_view.RotateCounterClockwise();
	SetScrolls();
	GetMainFrame()->SetPageNum(m_view.GetCurrentPage());	
}


void CPDFViewView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	// Update page number, zoom, and other view specifics
	GetMainFrame()->SetPageNum(m_view.GetCurrentPage(), true);	
}

BOOL CPDFViewView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE; // return CView::OnEraseBkgnd(pDC);
}

void CPDFViewView::OnAntiAlias() 
{
	m_anti_alias = !m_anti_alias;
	m_view.SetAntiAliasing(m_anti_alias);
	m_view.Update();
}

void CPDFViewView::OnUpdateAntiAlias(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_anti_alias);
}

void CPDFViewView::OnSmoothImages() 
{
	m_smooth_images = !m_smooth_images;
	m_view.SetImageSmoothing(m_smooth_images);
	m_view.Update();
}

void CPDFViewView::OnUpdateSmoothImages(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_smooth_images);
}

void CPDFViewView::OnToggleRasterizer() 
{
	m_builtin_rasterizer = !m_builtin_rasterizer;
	m_view.SetRasterizerType(m_builtin_rasterizer ? PDF::PDFRasterizer::e_BuiltIn : PDF::PDFRasterizer::e_GDIPlus);
	m_view.Update();
}

void CPDFViewView::OnUpdateRasterizerOption(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_builtin_rasterizer);
}

void CPDFViewView::OnThinLinePixelGridFit() 
{
	m_pixel_grid_fit = !m_pixel_grid_fit;
	m_view.SetThinLineAdjustment(m_pixel_grid_fit, m_stroke_adjust);
	m_view.Update();
}

void CPDFViewView::OnUpdateThinLinePixelGridFit(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_pixel_grid_fit);
}

void CPDFViewView::OnShowTransGrid()
{
	m_show_trans_grid = !m_show_trans_grid;
	m_view.SetPageTransparencyGrid(m_show_trans_grid);
	m_view.Update();
}

void CPDFViewView::OnUpdateShowTransGrid(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_show_trans_grid);
}

void CPDFViewView::OnOverprint(UINT nID)
{
	int old_op = m_overprint; 
	switch (nID)
	{
	case ID_OVERPRINT_OFF:
		m_overprint = 0;
		break;
	case ID_OVERPRINT_ON:
		m_overprint = 1;
		break;
	case ID_OVERPRINT_PDFX:
		m_overprint = 2;
		break;
	}
	if ( old_op != m_overprint )
	{
		m_view.SetOverprint(pdftron::PDF::PDFRasterizer::OverprintPreviewMode(m_overprint));
		m_view.Update();
	}
}

void CPDFViewView::OnUpdateOverprint(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
	case ID_OVERPRINT_OFF: 
		{
			pCmdUI->SetCheck(m_overprint==0);
		}
		break;
	case ID_OVERPRINT_ON:
		{
			pCmdUI->SetCheck(m_overprint==1);
		}
		break;
	case ID_OVERPRINT_PDFX:
		{
			pCmdUI->SetCheck(m_overprint==2);
		}
		break;
	}
}

void CPDFViewView::OnThinLineStrokeAdjust() 
{
	m_stroke_adjust = !m_stroke_adjust;
	m_view.SetThinLineAdjustment(m_pixel_grid_fit, m_stroke_adjust);
	m_view.Update();
}

void CPDFViewView::OnUpdateThinLineStrokeAdjust(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_stroke_adjust);
}
	
void CPDFViewView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	WORD null_val = 0xFFFF;
	WORD nScrollCode = null_val;
	switch (nChar) { 
		case VK_UP: 
			nScrollCode = SB_LINEUP; 
			break; 
		case VK_PRIOR: 
			nScrollCode = SB_PAGEUP; 
			break; 
		case VK_NEXT: 
			nScrollCode = SB_PAGEDOWN; 
			break; 
		case VK_DOWN: 
			nScrollCode = SB_LINEDOWN; 
			break; 
		case VK_HOME: 
			nScrollCode = SB_TOP; 
			break; 
		case VK_END: 
			nScrollCode = SB_BOTTOM; 
			break; 
	}

	if (nScrollCode != null_val) {
		OnVScroll(nScrollCode, 0, 0);
		return;
	}

	switch (nChar) { 
		case VK_LEFT: 
			nScrollCode = SB_LINELEFT; 
			break; 
		case VK_RIGHT: 
			nScrollCode = SB_LINERIGHT; 
			break; 
	}

	if (nScrollCode != null_val) {
		OnHScroll(nScrollCode, 0, 0);
	}
}

void CPDFViewView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	AfxMessageBox(_T("Coordinate conversion test. Ok."), MB_OK);

	double cx = point.x, cy = point.y;
	m_view.ConvScreenPtToPagePt(cx, cy);

	double px = 0, py = 0;
	m_view.ConvPagePtToScreenPt(px, py);

	int page_num = m_view.GetPageNumberFromScreenPt(point.x, point.y);

	CView::OnMButtonDblClk(nFlags, point);
}


BOOL CPDFViewView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// TODO:  call DoPreparePrinting to invoke the Print dialog box
	CPDFViewDoc* pDoc = GetDocument(); 	
	ASSERT_VALID(pDoc);
	if (pDoc && pDoc->mp_doc.get())
		pInfo->SetMaxPage(pDoc->mp_doc->GetPageCount());

	return DoPreparePrinting(pInfo);
}

void CPDFViewView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// PDFNet includes two different rasterizer implementations. 
	//
	// The two implementations offer a trade-off between print
	// speed and accuracy/quality, as well as a trade-off between 
	// vector and raster output.
	//
	// e_GDIPlus rasterizer can be used to render the page 
	// using Windows GDI+, whereas e_BuiltIn rasterizer can
	// be used to render bitmaps using platform-independent 
	// graphics engine (in this case images are always converted 
	// to bitmap prior to printing).
	m_pdfdraw.SetRasterizerType(PDF::PDFRasterizer::e_GDIPlus);
	m_pdfdraw.SetPrintMode(true);

	// You can uncomment the following lines in order to use 
	// built-in, platform-independent rasterizer instead of GDI+.
	// m_pdfdraw.SetRasterizerType(PDF::PDFRasterizer::e_BuiltIn);

	m_pdfdraw.SetDPI(200);
}

void CPDFViewView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
}

PDF::Rect CPDFViewView::GetPageDimensions(CDC* pDC, bool use_physical_page)
{
	PDF::Rect r;
	if (use_physical_page) {
		// The physical page is almost always greater than the 
		// printable area of the page, and never smaller. 
		r.x1 = -pDC->GetDeviceCaps(PHYSICALOFFSETX);
		r.y1 = -pDC->GetDeviceCaps(PHYSICALOFFSETY);
		r.x2 = pDC->GetDeviceCaps(PHYSICALWIDTH) - pDC->GetDeviceCaps(PHYSICALOFFSETX);
		r.y2 = pDC->GetDeviceCaps(PHYSICALHEIGHT) - pDC->GetDeviceCaps(PHYSICALOFFSETY);
	}
	else {
		// use the printable area of the page for printing.
		r.x1 = r.y1 = 0;
		r.x2 = pDC->GetDeviceCaps(PHYSICALWIDTH) - pDC->GetDeviceCaps(PHYSICALOFFSETX) * 2;
		r.y2 = pDC->GetDeviceCaps(PHYSICALHEIGHT) - pDC->GetDeviceCaps(PHYSICALOFFSETY) * 2;
	}
	return r;
}

void CPDFViewView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	PDF::PDFDoc* doc = GetDocument()->mp_doc.get();
	doc->Lock();
	try 
	{
		PDF::Page p = doc->GetPage(pInfo->m_nCurPage);
		if (p) //  Page found
		{
			PDF::Rect rect(GetPageDimensions(pDC, false));

			// The page dimensions are in in device units. We convert 
			// the dimensions to PDF units (or points). One point is 
			// 1/72 of an inch.
			double xdpi = pDC->GetDeviceCaps(LOGPIXELSX);
			double ydpi = pDC->GetDeviceCaps(LOGPIXELSY);
			double scale_x = 72.0 / xdpi;
			double scale_y = 72.0 / ydpi;
			rect.x1 *= scale_x; rect.y1 *= scale_y;
			rect.x2 *= scale_x; rect.y2 *= scale_y;
			m_pdfdraw.DrawInRect(p, pDC->m_hDC, rect);
		}
	} 
	catch (Common::Exception& ex) {
		std::string msg("Printing Error: ");
		msg += ex.GetMessage();
		AfxMessageBox(CString(msg.c_str()), MB_ICONEXCLAMATION, 0);
	}
	catch (...) {
		::AfxMessageBox(_T("An error encountered during printing"), MB_ICONEXCLAMATION, 0);
	}

	doc->Unlock(); 
}

void CPDFViewView::OnOptionsGamma()
{
	COptions dlg;
	dlg.m_gamma = 0.1;
	if (dlg.DoModal() == IDOK) {
		m_view.SetGamma(dlg.m_gamma);
		m_view.Update();
	}
}

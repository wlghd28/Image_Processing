// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "PDFViewDoc.h"

#include <PDF/OCG/Group.h>
#include ".\BookmarkView.h"
#include ".\LayerView.h"
#include ".\LeftView.h"

using namespace pdftron;
using namespace Common;
using namespace SDF;
using namespace PDF;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTabView)

BEGIN_MESSAGE_MAP(CLeftView, CTabView)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CLeftView construction/destruction

CLeftView::CLeftView() 
{
	mp_view = 0;
	mp_wndSplitter = 0;
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style |= TCS_VERTICAL | TCS_MULTILINE;
	return CTabView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
	ASSERT(GetView(0)->IsKindOf(RUNTIME_CLASS(CBookmarkView)));
	CBookmarkView* bookmark_view = dynamic_cast<CBookmarkView*>(GetView(0));
	bookmark_view->SetTargetPDFView(*mp_view);

	ASSERT(GetView(2)->IsKindOf(RUNTIME_CLASS(CLayerView)));
	CLayerView* layer_view = dynamic_cast<CLayerView*>(GetView(2));
	layer_view->SetTargetPDFView(*mp_view);
    
	// Set the width of the bookmark view.
	if (mp_wndSplitter) // && doc->GetFirstBookmark().IsValid()) 
	{
		mp_wndSplitter->SetColumnInfo(0, 150, 0);
		mp_wndSplitter->SetColumnInfo(1, 400, 0);
		mp_wndSplitter->RecalcLayout();
	}

	CTabView::OnInitialUpdate();
}

// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTabView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}

CPDFViewDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPDFViewDoc)));
	return (CPDFViewDoc*)m_pDocument;
}
#endif //_DEBUG

void CLeftView::SetTargetPDFView(pdftron::PDF::PDFView& view, CSplitterWnd* s) {
	mp_view = &view;
	mp_wndSplitter = s;	
}

int CLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetFontHeight(14);
	Insert(_T("Bookmarks"), RUNTIME_CLASS(CBookmarkView));
	Insert(_T("Pages"), RUNTIME_CLASS(CLayerView));
	Insert(_T("Layers"), RUNTIME_CLASS(CLayerView));

	return 0;
}


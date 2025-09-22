// LeftView.cpp : implementation of the CBookmarkView class
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "BookmarkView.h"
#include "PDFViewDoc.h"

#include <PDF/Bookmark.h>

using namespace pdftron;
using namespace Common;
using namespace PDF;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBookmarkView

IMPLEMENT_DYNCREATE(CBookmarkView, CTreeView)

BEGIN_MESSAGE_MAP(CBookmarkView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
END_MESSAGE_MAP()


// CBookmarkView construction/destruction

CBookmarkView::CBookmarkView() 
{
	mp_view = 0;
	mp_wndSplitter = 0;
}

CBookmarkView::~CBookmarkView()
{
}

BOOL CBookmarkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style |= TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;

	return CTreeView::PreCreateWindow(cs);
}

// Populate the tree control with bookmark items.
void FillBookmarkTree(Bookmark& item, CTreeCtrl& tree, HTREEITEM hParent)
{
	for (int i=0; item.IsValid(); item=item.GetNext(), ++i)
	{
		UString title(item.GetTitle());
#ifdef _UNICODE
		HTREEITEM node = tree.InsertItem(title.ConvertToNativeWString().c_str(), hParent);
#else
		HTREEITEM node = tree.InsertItem(title.ConvertToAscii().c_str(), hParent);
#endif
		
		tree.SetItemData(node, (DWORD_PTR) item.mp_obj);

		if (item.IsOpen()) {
			tree.SetItemState(node, TVIS_EXPANDED, TVIS_EXPANDED);
		}

		if ((item.GetFlags() & 0x02) != 0) {
			tree.SetItemState(node, TVIS_BOLD, TVIS_BOLD);
		}

		if (item.HasChildren())	 // Recursively add children sub-trees
		{
			FillBookmarkTree(item.GetFirstChild(), tree, node);
		}
	}
}

void CBookmarkView::SetTargetPDFView(pdftron::PDF::PDFView& view) 
{
	mp_view = &view;

	CTreeCtrl& tree = GetTreeCtrl();
	PDF::PDFDoc* doc = GetDocument()->mp_doc.get();
	ASSERT(doc);

	doc->Lock();

	// Populate a bookmark tree control with bookmark nodes (if any). 
	Bookmark root = doc->GetFirstBookmark();
	if (root.IsValid()) {
		FillBookmarkTree(root, tree, TVI_ROOT);
	}
	doc->Unlock();
}

// CBookmarkView diagnostics

#ifdef _DEBUG
void CBookmarkView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CBookmarkView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CPDFViewDoc* CBookmarkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPDFViewDoc)));
	return (CPDFViewDoc*)m_pDocument;
}
#endif //_DEBUG


void CBookmarkView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TRN_Bookmark b = (TRN_Bookmark) pNMTreeView->itemNew.lParam;
	if (!b) return;

	PDF::PDFDoc* doc = GetDocument()->mp_doc.get();
	doc->Lock();

	try {
		Bookmark item(b);
		Action action = item.GetAction();
		if (action.IsValid())  // Handle goto actions. 
		{	// Other types of actions can be handled in similar way.
			if (action.GetType() == Action::e_GoTo)
			{
				Destination dest = action.GetDest();
				if (dest.IsValid()) 
				{
					Page page = dest.GetPage();
					ASSERT(mp_view);
					mp_view->SetCurrentPage(page.GetIndex());
				}
			}
		}
	}
	catch(...) {
		MessageBox(_T("Error reading a bookmark"), _T("Error"), MB_ICONINFORMATION);
	}


	doc->Unlock();

	*pResult = 0;
}

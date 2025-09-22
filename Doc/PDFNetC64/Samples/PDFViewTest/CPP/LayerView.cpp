// LayerView.cpp : implementation of the CLayerView class
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "PDFViewDoc.h"

#include <PDF/OCG/Group.h>
#include ".\LayerView.h"

using namespace pdftron;
using namespace Common;
using namespace SDF;
using namespace PDF;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLayerView

IMPLEMENT_DYNCREATE(CLayerView, CTreeView)

BEGIN_MESSAGE_MAP(CLayerView, CTreeView)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CLayerView construction/destruction

CLayerView::CLayerView() 
{
	mp_view = 0;
	mp_wndSplitter = 0;
}

CLayerView::~CLayerView()
{
}

BOOL CLayerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style |= TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;
	return CTreeView::PreCreateWindow(cs);
}

// Populate the tree control with OCG (Optional Content Group) layers.
void BuildLayerTree(Obj layer_arr, const OCG::Config& init_cfg, CTreeCtrl& tree, HTREEITEM hParent)
{
	if (!layer_arr.IsArray()) { // must be an array.
		ASSERT(false);
		return;
	}

	HTREEITEM node;
	SDF::Obj lobj;
	int i, sz = int(layer_arr.Size());
	for (i=0; i<sz; ++i)
	{
		lobj = layer_arr.GetAt(i);
		if (lobj.IsArray() && lobj.Size()>0) 
		{
			node = tree.InsertItem(_T(""), hParent);
			tree.SetCheck(node, TRUE);
			tree.SetItemState(node, TVIS_EXPANDED, TVIS_EXPANDED);
			tree.SetItemData(node, (DWORD_PTR) lobj.mp_obj);
			// Recursively add children sub-trees
			BuildLayerTree(lobj, init_cfg, tree, node);
		}
		else if (i==0 && lobj.IsString()) 
		{
			UString label;
			lobj.GetAsPDFText(label);
#ifdef _UNICODE
			tree.SetItemText(hParent, label.ConvertToNativeWString().c_str());
#else
			tree.SetItemText(hParent, label.ConvertToAscii().c_str());
#endif
		}
		else {
			OCG::Group grp(lobj);
			if (grp.IsValid()) 
			{
				UString layer_name = grp.GetName();
#ifdef _UNICODE
				node = tree.InsertItem(layer_name.ConvertToNativeWString().c_str(), hParent);
#else
				node = tree.InsertItem(layer_name.ConvertToAscii().c_str(), hParent);
#endif
				tree.SetItemData(node, (DWORD_PTR) lobj.mp_obj);
				tree.SetCheck(node, grp.GetInitialState(init_cfg));
			}
		}
	}
}

// CLayerView diagnostics

#ifdef _DEBUG
void CLayerView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLayerView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CPDFViewDoc* CLayerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPDFViewDoc)));
	return (CPDFViewDoc*)m_pDocument;
}
#endif //_DEBUG


void CLayerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags=0; 
	CTreeCtrl& tree = GetTreeCtrl();
	HTREEITEM hnode = tree.HitTest(point, &uFlags); 

	if( uFlags & TVHT_ONITEMSTATEICON ) { 
		ASSERT(mp_view);
		OCG::Context ctx = mp_view->GetOCGContext();
		if (ctx.IsValid()) {
			if (TRN_OCG ocg_obj = (TRN_OCG) tree.GetItemData(hnode)) 
			{				
				BOOL checked = tree.GetCheck(hnode);
				ctx.SetState(OCG::Group(ocg_obj), !checked); // Toggle the state.
				mp_view->Update();
			}
		}
	} 

	CTreeView::OnLButtonDown(nFlags, point);
}

void CLayerView::SetTargetPDFView(pdftron::PDF::PDFView& view) 
{
	mp_view = &view;

	CTreeCtrl& tree = GetTreeCtrl();
	PDF::PDFDoc* doc = GetDocument()->mp_doc.get();
	ASSERT(doc);

	doc->Lock();
	if (doc->HasOC()) {
		OCG::Config cfg = doc->GetOCGConfig();
		if (Obj order = cfg.GetOrder()) {
			tree.ModifyStyle(0, TVS_CHECKBOXES);
			BuildLayerTree(order, cfg, tree, TVI_ROOT);
		}
	}
	doc->Unlock();
}
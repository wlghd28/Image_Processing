// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMainFrame* GetMainFrame()
{
	return static_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
}

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_SELECTALL, OnSelectall)

	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_PAN, ID_VIEW_PAN, OnUpdateBasicToolSelection)

	ON_CONTROL(IDC_PAGENUM_FINISH_EDIT, IDC_PAGENUM_NFO, OnChangePageNumber)
	ON_CONTROL(IDC_PAGENUM_CANCEL_EDIT, IDC_PAGENUM_NFO, OnCancelPageNumber)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CPageNumEditCtrl, CEdit)

BEGIN_MESSAGE_MAP(CPageNumEditCtrl, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

BOOL CPageNumEditCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			GetMainFrame()->SendMessage(WM_COMMAND, MAKEWPARAM(static_cast<WPARAM>(GetDlgCtrlID()), IDC_PAGENUM_FINISH_EDIT), (LPARAM) m_hWnd);
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			GetMainFrame()->SendMessage(WM_COMMAND, MAKEWPARAM(static_cast<WPARAM>(GetDlgCtrlID()), IDC_PAGENUM_CANCEL_EDIT), (LPARAM) m_hWnd);
			return TRUE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() 
 : _page_num(-1)
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD toolbar_style = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;

	//---------------------------------------------------------------------------------------------
	if (!m_fopen_bar.CreateEx(this, TBSTYLE_FLAT, toolbar_style, CRect(1, 1, 1, 1), 4001) || !m_fopen_bar.LoadToolBar(IDR_FOPEN_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	m_fopen_bar.LoadTrueColorToolBar(25, IDR_FOPEN_TOOLBAR, IDR_FOPEN_TOOLBAR_HOT, IDR_FOPEN_TOOLBAR_DISABLED);
	m_fopen_bar.AdjustLayout();
	m_fopen_bar.AdjustSize();
	//---------------------------------------------------------------------------------------------
	if (!m_basic_bar.CreateEx(this, TBSTYLE_FLAT, toolbar_style, CRect(1, 1, 1, 1), 4002) || !m_basic_bar.LoadToolBar(IDR_BASIC_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	m_basic_bar.LoadTrueColorToolBar(25, IDR_BASIC_TOOLBAR, IDR_BASIC_TOOLBAR_HOT, IDR_BASIC_TOOLBAR_DISABLED);
	m_basic_bar.AdjustLayout();
	m_basic_bar.AdjustSize();
	//---------------------------------------------------------------------------------------------

	if (!m_pagemode_bar.CreateEx(this, TBSTYLE_FLAT, toolbar_style, CRect(1, 1, 1, 1), 4003) || !m_pagemode_bar.LoadToolBar(IDR_PAGEVIEW_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	m_pagemode_bar.LoadTrueColorToolBar(25, IDR_PAGEVIEW_TOOLBAR, IDR_PAGEVIEW_TOOLBAR_HOT, IDR_PAGEVIEW_TOOLBAR_DISABLED);
	m_pagemode_bar.AdjustLayout();
	m_pagemode_bar.AdjustSize();

	//---------------------------------------------------------------------------------------------
	if (!m_nav_bar.CreateEx(this, TBSTYLE_FLAT, toolbar_style, CRect(1, 1, 1, 1), 4004) || !m_nav_bar.LoadToolBar(IDR_NAV_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	m_nav_bar.LoadTrueColorToolBar(25, IDR_NAV_TOOLBAR, IDR_NAV_TOOLBAR_HOT, IDR_NAV_TOOLBAR_DISABLED);
	m_nav_bar.SetButtonInfo(2, IDC_PAGENUM_NFO, TBBS_SEPARATOR, 65);

	CRect rect;
	m_nav_bar.GetItemRect(2, rect);
	rect.DeflateRect(2, 0);
	m_nav_bar.AdjustLayout();
	m_nav_bar.AdjustSize();

	m_page_num.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED | ES_CENTER | ES_NUMBER, rect, &m_nav_bar, IDC_PAGENUM_NFO);

	//---------------------------------------------------------------------------------------------
	if (!m_misc_bar.CreateEx(this, TBSTYLE_FLAT, toolbar_style, CRect(1, 1, 1, 1), 4005) || !m_misc_bar.LoadToolBar(IDR_MISC_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	m_misc_bar.LoadTrueColorToolBar(25, IDR_MISC_TOOLBAR, IDR_MISC_TOOLBAR_HOT, IDR_MISC_TOOLBAR_DISABLED);
	m_misc_bar.AdjustLayout();
	m_misc_bar.AdjustSize();
	//---------------------------------------------------------------------------------------------
	if (!m_forms_bar.CreateEx(this, TBSTYLE_FLAT, toolbar_style, CRect(1, 1, 1, 1), 4006) || !m_forms_bar.LoadToolBar(IDR_FORMS_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	m_forms_bar.LoadTrueColorToolBar(25, IDR_FORMS_TOOLBAR, IDR_FORMS_TOOLBAR_HOT, IDR_FORMS_TOOLBAR_DISABLED);
	m_forms_bar.AdjustLayout();
	m_forms_bar.AdjustSize();
	//---------------------------------------------------------------------------------------------
	
	EnableDocking(CBRS_ALIGN_ANY);

	// m_fopen_bar, m_basic_bar, m_pagemode_bar, m_nav_bar, m_misc_bar, m_forms_bar
	m_forms_bar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_forms_bar);

	m_misc_bar.EnableDocking(CBRS_ALIGN_ANY);
	DockPaneLeftOf(&m_misc_bar, &m_forms_bar);

	m_nav_bar.EnableDocking(CBRS_ALIGN_ANY);
	DockPaneLeftOf(&m_nav_bar, &m_misc_bar);

	m_pagemode_bar.EnableDocking(CBRS_ALIGN_ANY);
	DockPaneLeftOf(&m_pagemode_bar, &m_nav_bar);

	m_basic_bar.EnableDocking(CBRS_ALIGN_ANY);
	DockPaneLeftOf(&m_basic_bar,&m_pagemode_bar);

	m_fopen_bar.EnableDocking(CBRS_ALIGN_ANY);
	DockPaneLeftOf(&m_fopen_bar, &m_basic_bar);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers


void CMainFrame::OnSelectall()
{
	// Get the active MDI child window.
	CMDIChildWnd *pChild = MDIGetActive();

	// Get the active view attached to the active MDI child window.
	CView *view = (CView *) pChild->GetActiveView();
	if (!view) return; 
	view->SendMessage(WM_COMMAND, ID_SELECTALL);
}

bool CMainFrame::SetPageNum(int page_num, bool reset)
{
	if (reset || page_num != _page_num) 
	{
#ifdef _UNICODE
		_itow(page_num, _buf, 10);
#else
		_itoa(page_num, _buf, 10);
#endif
		m_page_num.SetWindowText(_buf);
		_page_num = page_num;
		return true;
	}

	return false;
}

void CMainFrame::OnChangePageNumber()
{
	CChildFrame* pChild = (CChildFrame*) MDIGetActive();
	if (pChild) 
	{
		m_page_num.GetWindowText(_buf, sizeof(_buf));
#ifdef _UNICODE
		int page_num = _wtoi(_buf);
#else
		int page_num = atoi(_buf);
#endif

		if (!pChild->SetCurrentPage(page_num)) 
		{
#ifdef _UNICODE
			wchar_t buf[1024];
			_snwprintf(buf, 1022, L"There is no page numbered \'%s\' in this document", _buf);
#else
			char buf[1024];
			_snprintf(buf, 1022, "There is no page numbered \'%s\' in this document", _buf);
#endif
			MessageBox(buf, _T("PDFView"), MB_OK);

			SetPageNum(page_num, true);
		}
	}
}

void CMainFrame::OnCancelPageNumber()
{
	SetPageNum(_page_num, true);
}

void CMainFrame::OnUpdateBasicToolSelection(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(TRUE);
	pCmdUI->Enable(TRUE);
}

/*
Module : HLINKCTRL.CPP
Purpose: Implementation for a MFC class for a static text control class with hyperlink support
Created: PJN / 16-06-1997
History: PJN / 24-06-1997, hyperlinks are now coloured ala IE
         PJN / 15-07-1997, Now supports ShrinkToFit
         PJN / 11-09-1997, 1) Added support for a highlight look which includes a highlight color and
                              a shadowed look. Also fixed a bug relating to the Edit controls context menu.
                           2) Also included is a new context menu ala IE3
         PJN / 06-11-1997, 1) Improved drawing of control by handling WM_CTLCOLOR through MFC message reflection
                           2) class now derived from CStatic as it should have been from the begining
                           3) Dropped support for shrink to fit as now no need
                           4) Description is now taken from controls window text
                           5) Now using more standard visited and unvisted colors
                           6) Dropped support for optional underlying and drop shadow effects
                           7) streamlined some of the functions declarations
                           8) Wait cursor is now shown when a shortcut is being saved
                           9) Context menu for control can now be turned off if so desired
         PJN / 08-12-1997, 1) Removed ON_COMMAND(ID_POPUP_OPEN, OnOpen) from #define which was causing 
                              to the hyperlink correctly
                           2) Removed a level 4 warning which was being generated when building in release mode  
         PJN / 09-01-1998  1) Removed duplicate OnAddToFavorites and OnAddToDesktop functions
         PJN / 29-03-1999  1) General code tidy up
                           2) Unicode enabled all the source code
                           3) Now uses new IDC_HAND cursor in preference to my own hand cursor
                           4) Removed a number of unused member variables
         PJN / 11-12-1999  1) Now supports a disabled hyperlink cursor similar to the offline cursor in IE.
         PJN / 12-01-2000  1) The zip file lists disabled.cur in the home directory, not in the RES directory 
                           where it belongs.
                           2) When compiling for UNICODE, the copy-to-clipboard feature sets the incorrect clipboard 
                           type CF_TEXT, not CF_UNICODETEXT. This results in only the first character being available 
                           in the clipboard.
         PJN / 03-04-2002  1) Fixed a problem where the underlined style was not being set for the control when the 
                           "NO wrap" window style was applied.
                           2) Now includes copyright message in the source code and documentation.




Copyright (c) 1997 - 2002 by PJ Naughter.  

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/

/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"

#ifndef HLINK_NOOLE
#define INITGUID
#endif
#include "resource.h"
#include "hlinkctl.h"
#ifndef HLINK_NOOLE
#include <initguid.h>
#endif
#include <winnetwk.h>
#include <winnls.h>
#include <shlobj.h>
#ifndef HLINK_NOOLE
#include <intshcut.h>
#endif

//For those people who do not have the absolutely latest SDK header files
#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)   // Taken from VC6 WINUSER.H
#endif // IDC_HAND



/////////////////////////////////  Macros & Statics ///////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////// Implementation //////////////////////////////

BEGIN_MESSAGE_MAP(CHLinkCtrl, CStatic)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CHLinkCtrl)
  ON_WM_SETCURSOR()
  ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_POPUP_COPYSHORTCUT, OnCopyShortcut)
	ON_COMMAND(ID_POPUP_PROPERTIES, OnProperties)
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR_REFLECT()
  ON_COMMAND(ID_POPUP_OPEN, OnOpen)
#ifndef HLINK_NOOLE
	ON_COMMAND(ID_POPUP_ADDTOFAVORITES, OnAddToFavorites)
  ON_COMMAND(ID_POPUP_ADDTODESKTOP, OnAddToDesktop)
#endif
END_MESSAGE_MAP()

CHLinkCtrl::CHLinkCtrl()
{
	m_Color = RGB(0, 0, 255);
	m_VisitedColor = RGB(128, 0, 128);
	m_HighlightColor = RGB(255, 0, 0);

  m_bUseHighlight = TRUE;
	m_State = ST_NOT_VISITED;
  m_OldState = ST_NOT_VISITED;
  m_bShowingContext = FALSE;
  m_bAllowContextMenu = TRUE;
  m_bEnabledLink = TRUE;

	//Load up the hand cursor, First try to load 
  //the Win98 / Windows 2000 Hand Cursor
  CWinApp* pApp = AfxGetApp();
  m_hLinkCursor = pApp->LoadStandardCursor(IDC_HAND);
  // Still no cursor handle - load our own
  if (m_hLinkCursor == NULL)
    m_hLinkCursor = pApp->LoadCursor(IDC_HLINK);
  ASSERT(m_hLinkCursor);

  //Load up the disabled hand cursor
  m_hDisabledLinkCursor = pApp->LoadCursor(IDC_DISABLED_HLINK);
  ASSERT(m_hDisabledLinkCursor);
}

void CHLinkCtrl::SetHyperLink(const CString& sLink)
{
	m_sLink = sLink;
}

void CHLinkCtrl::SetEnabledLink(BOOL bEnabled)
{
  m_bEnabledLink = bEnabled;
  UpdateWindow();
}

BOOL CHLinkCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
  if (!m_bShowingContext)
  {
    if (m_bEnabledLink)
    {
      //Use the hand cursor
	    ::SetCursor(m_hLinkCursor);
    }
    else
    {
      //Use the disabled hand cursor
      ::SetCursor(m_hDisabledLinkCursor);
    }
	  return TRUE;
  }

  //Let the parent class do its thing
  return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CHLinkCtrl::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/) 
{
  PostMessage(WM_COMMAND, ID_POPUP_OPEN);
}

void CHLinkCtrl::OnOpen()
{
  if (Open())
	  m_State = ST_VISITED;
}

void CHLinkCtrl::SetLinkColor(const COLORREF& color) 
{ 
	m_Color = color; 
	UpdateWindow(); 
}

void CHLinkCtrl::SetVisitedLinkColor(const COLORREF& color) 
{ 
	m_VisitedColor = color; 
	UpdateWindow(); 
}

void CHLinkCtrl::SetHighlightLinkColor(const COLORREF& color) 
{ 
	m_HighlightColor = color; 
	UpdateWindow(); 
}

void CHLinkCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
  if (!m_bUseHighlight)
    return;

	CRect rc;
	GetClientRect(rc);
	if (rc.PtInRect(point))
	{
		if (m_State != ST_HIGHLIGHTED)
		{
			SetCapture();
			HighLight(TRUE);
		} 
	}
	else
	{
		if (m_State == ST_HIGHLIGHTED)
		{
			HighLight(FALSE);
			ReleaseCapture();
		}
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

void CHLinkCtrl::HighLight(BOOL state)
{
	if (state)
	{
		if (m_State != ST_HIGHLIGHTED)
		{
			m_OldState = m_State;
			m_State = ST_HIGHLIGHTED;
			Invalidate();
		}
	}
	else
	{
		if (m_State == ST_HIGHLIGHTED)
		{
			m_State = m_OldState;
			Invalidate();
		}
	}
}

void CHLinkCtrl::OnContextMenu(CWnd*, CPoint point)
{
  if (!m_bAllowContextMenu)
    return;

  HighLight(FALSE);
	ReleaseCapture();
  
	if (point.x == -1 && point.y == -1)
  {
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_HLINK_POPUP));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

  m_bShowingContext = TRUE;
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
  m_bShowingContext = FALSE;
}

void CHLinkCtrl::OnCopyShortcut() 
{
  if (OpenClipboard())
  {
    int nBytes = sizeof(TCHAR) * (m_sLink.GetLength() + 1);
    HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, nBytes);
    TCHAR* pData = (TCHAR*) GlobalLock(hMem);
    _tcscpy(pData, (LPCTSTR) m_sLink);
    GlobalUnlock(hMem);
    EmptyClipboard();
	#ifndef _UNICODE
  	SetClipboardData(CF_TEXT, hMem);
	#else
  	SetClipboardData(CF_UNICODETEXT, hMem);
	#endif
    CloseClipboard();
  }
}

void CHLinkCtrl::OnProperties() 
{
	ShowProperties();
}

void CHLinkCtrl::ShowProperties() const
{
	CHLinkSheet propSheet(IDS_HLINK_PROPERTIES, GetParent());
  propSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
  propSheet.SetBuddy(this);
  propSheet.DoModal();
}

BOOL CHLinkCtrl::Open() const
{
  if (!m_bEnabledLink)
    return FALSE;

  CWaitCursor cursor;

#ifndef HLINK_NOOLE
  //First try to open using IUniformResourceLocator
  BOOL bSuccess = OpenUsingCom();
  
  //As a last resort try ShellExecuting the URL, may
  //even work on Navigator!
  if (!bSuccess)
    bSuccess = OpenUsingShellExecute();
#else
  BOOL bSuccess = OpenUsingShellExecute();
#endif

  return bSuccess;
}

#ifndef HLINK_NOOLE
BOOL CHLinkCtrl::OpenUsingCom() const
{
  //Get the URL Com interface
  IUniformResourceLocator* pURL;
  HRESULT hres = CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (void**) &pURL);
  if (!SUCCEEDED(hres))
  {
    TRACE(_T("Failed to get the IUniformResourceLocator interface\n"));
    return FALSE;
  }

  hres = pURL->SetURL(m_sLink, IURL_SETURL_FL_GUESS_PROTOCOL);
  if (!SUCCEEDED(hres))
  {
    TRACE(_T("Failed in call to SetURL\n"));
    pURL->Release();
    return FALSE;
  }

  //Open the URL by calling InvokeCommand
  URLINVOKECOMMANDINFO ivci;
  ivci.dwcbSize = sizeof(URLINVOKECOMMANDINFO);
  ivci.dwFlags = IURL_INVOKECOMMAND_FL_ALLOW_UI;
  ivci.hwndParent = GetParent()->GetSafeHwnd();
  ivci.pcszVerb = _T("open");
  hres = pURL->InvokeCommand(&ivci);
  if (!SUCCEEDED(hres))
  {
    TRACE(_T("Failed to invoke URL using InvokeCommand\n"));
    pURL->Release();
    return FALSE;
  }

  // Release the pointer to IUniformResourceLocator.
  pURL->Release();

  return TRUE;
}
#endif

BOOL CHLinkCtrl::OpenUsingShellExecute() const
{
	__int64 hRun = (__int64) ShellExecute(GetParent()->GetSafeHwnd(), _T("open"), m_sLink, NULL, NULL, SW_SHOW);
    if (hRun <= 32)
    {
       TRACE(_T("Failed to invoke URL using ShellExecute\n"));
       return FALSE;
    }

	return TRUE;
}

#ifndef HLINK_NOOLE
BOOL CHLinkCtrl::AddToSpecialFolder(int nFolder) const
{
  //Give the user some feedback
  CWaitCursor cursor;

  // Get the shell's allocator. 
  IMalloc* pMalloc;
  if (!SUCCEEDED(SHGetMalloc(&pMalloc))) 
  {
    TRACE(_T("Failed to get the shell's IMalloc interface\n"));
    return FALSE;
  }

  //Get the location of the special Folder required
  LPITEMIDLIST pidlFolder;
  HRESULT hres = SHGetSpecialFolderLocation(NULL, nFolder, &pidlFolder); 
  if (!SUCCEEDED(hres))
  {
    TRACE(_T("Failed in call to SHGetSpecialFolderLocation\n"));
    pMalloc->Release();
    return FALSE;
  }

  //convert the PIDL to a file system name and
  //add an extension of URL to create an Internet 
  //Shortcut file
  TCHAR sFolder[_MAX_PATH];
  if (!SHGetPathFromIDList(pidlFolder, sFolder))
  {
    TRACE(_T("Failed in call to SHGetPathFromIDList"));
    pMalloc->Release();
    return FALSE;
  }
  TCHAR sShortcutFile[_MAX_PATH];  

  CString sLinkDescription;
  GetWindowText(sLinkDescription);
  _tmakepath(sShortcutFile, NULL, sFolder, sLinkDescription, _T("URL"));

  //Free the pidl
  pMalloc->Free(pidlFolder);

  //Do the actual saving
  BOOL bSuccess = Save(sShortcutFile);

  // Release the pointer to IMalloc
  pMalloc->Release(); 

  return bSuccess;
}
#endif

#ifndef HLINK_NOOLE
void CHLinkCtrl::OnAddToFavorites() 
{
  AddToSpecialFolder(CSIDL_FAVORITES);
} 
#endif

#ifndef HLINK_NOOLE
void CHLinkCtrl::OnAddToDesktop() 
{
  AddToSpecialFolder(CSIDL_DESKTOP);
} 
#endif

#ifndef HLINK_NOOLE
BOOL CHLinkCtrl::Save(const CString& sFilename) const
{
  //Get the URL Com interface
  IUniformResourceLocator* pURL;
  HRESULT hres = CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (void**) &pURL);
  if (!SUCCEEDED(hres))
  {
    TRACE(_T("Failed to get the IUniformResourceLocator interface\n"));
    return FALSE;
  }

  hres = pURL->SetURL(m_sLink, IURL_SETURL_FL_GUESS_PROTOCOL);
  if (!SUCCEEDED(hres))
  {
    TRACE(_T("Failed in call to SetURL\n"));
    pURL->Release();
    return FALSE;
  }

  // Get the IPersistFile interface for 
  // saving the shortcut in persistent storage.
  IPersistFile* ppf;
  hres = pURL->QueryInterface(IID_IPersistFile, (void **)&ppf);
  if (!SUCCEEDED(hres))
  {
    TRACE(_T("Failed to get the IPersistFile interface\n"));
    pURL->Release();
    return FALSE;
  }

  // Save the shortcut via the IPersistFile::Save member function.
  #ifndef _UNICODE
  WORD wsz[_MAX_PATH];
  MultiByteToWideChar(CP_ACP, 0, sFilename, -1, (LPWSTR)wsz, _MAX_PATH);
  hres = ppf->Save((LPCOLESTR)wsz, TRUE);
  #else
  hres = ppf->Save(sFilename, TRUE);
  #endif
  if (!SUCCEEDED(hres))
  {
    TRACE(_T("IPersistFile::Save failed!\n"));
    ppf->Release();
    pURL->Release();
    return FALSE;
  }

  // Release the pointer to IPersistFile.
  ppf->Release();

  // Release the pointer to IUniformResourceLocator.
  pURL->Release();

  return TRUE;
} 
#endif

#ifdef _DEBUG
HBRUSH CHLinkCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
#else
HBRUSH CHLinkCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
#endif
{
	ASSERT(nCtlColor == CTLCOLOR_STATIC);

	DWORD dwStyle = GetStyle();
	if (!(dwStyle & SS_NOTIFY)) 
  {
		//Turn on notify flag to get mouse messages and STN_CLICKED.
		//Otherwise, I'll never get any mouse clicks!
		::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
	}
	
	//Modify the font to be underline
  if (!((HFONT) m_font))
  {
		LOGFONT lf;
		GetFont()->GetObject(sizeof(lf), &lf);

		lf.lfUnderline = TRUE;
		m_font.CreateFontIndirect(&lf);
  }
	pDC->SelectObject(&m_font);


  //set the text colors
  switch (m_State)
  {
		case ST_NOT_VISITED:	pDC->SetTextColor(m_Color); break;
		case ST_VISITED:	  	pDC->SetTextColor(m_VisitedColor); break;
		case ST_HIGHLIGHTED:	pDC->SetTextColor(m_HighlightColor); break;
    default: ASSERT(FALSE);
  }
	pDC->SetBkMode(TRANSPARENT);

	// return hollow brush to preserve parent background color
	return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
}







IMPLEMENT_DYNCREATE(CHLinkPage, CPropertyPage)

CHLinkPage::CHLinkPage() : CPropertyPage(CHLinkPage::IDD)
{
	//{{AFX_DATA_INIT(CHLinkPage)
	//}}AFX_DATA_INIT
  m_pBuddy = NULL;
}

void CHLinkPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHLinkPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHLinkPage, CPropertyPage)
	//{{AFX_MSG_MAP(CHLinkPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





IMPLEMENT_DYNAMIC(CHLinkSheet, CPropertySheet)

CHLinkSheet::CHLinkSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
  AddPage(&m_page1);
}

CHLinkSheet::CHLinkSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
  AddPage(&m_page1);
}

BEGIN_MESSAGE_MAP(CHLinkSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CHLinkSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CHLinkPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	ASSERT(m_pBuddy);
  CString sDescription;
  m_pBuddy->GetWindowText(sDescription);
  GetDlgItem(IDC_NAME)->SetWindowText(sDescription);
  GetDlgItem(IDC_URL)->SetWindowText(m_pBuddy->GetHyperLink());

	return TRUE;
}




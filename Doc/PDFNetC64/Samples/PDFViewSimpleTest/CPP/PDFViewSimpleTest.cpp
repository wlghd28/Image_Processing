//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------
// This sample shows how to add PDF viewer control in a basic Win32 project. 
// The sample uses a number of built-in features from PDFViewCtrl to implement 
// document navigation, text highlighting, markup, and editing. 
//
// PDFViewCtrl offers built-in support for a number of PDF tools (including markup, 
// link creation, forms, text selection, etc) and user interface elements (such as 
// navigation panel, find text, etc). PDFViewCtrl is highly configurable allowing 
// users to implement custom tools or to modify behavior of built-in tools. 
// Default user interface elements such as the navigation panel can be replaced 
// with a custom implementation and users can override window events and various 
// document actions. For example, by default, PDFViewCtrl will launch URL links 
// in a web browser and will jump to a destination for 'GoTo' actions. The default 
// behavior can be overridden using a custom callback function (as shown in the 
// following sample). 
//
// Besides PDFViewCtrl, the C++ version of PDFNet SDK also includes a low-level class
// called PDFView. Because PDFView does not process any window messages and does not 
// implement built-in tools it is only suitable for very low-level control development 
// (e.g. if you would like to implement a control similar to PDFViewCtrl from scratch). 
// The basic use of PDFView is illustrated in PDFView sample project.
//---------------------------------------------------------------------------------------

#include "stdafx.h"
#include "resource.h"
#include <Common/Exception.h>
#include <commdlg.h>
#include <cassert>
#include <PDF/PDFViewCtrl.h>
#include <PDF/PDFDraw.h>
#include <PDF/PDFNet.h>
#include <stdlib.h>
#include <iostream>
#include <utility>
#include <algorithm>
#include <memory>

using namespace pdftron;
using namespace PDF;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
UString m_doc_name;
std::auto_ptr<PDF::PDFDoc> mp_doc;
std::auto_ptr<PDF::PDFViewCtrl> m_view;


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PDFVIEWSIMPLE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PDFVIEWSIMPLE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PDFVIEWSIMPLE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PDFVIEWSIMPLE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


typedef std::pair< int, PDF::PDFViewCtrl::ToolMode > ToolModeMapType;
const ToolModeMapType toolModeMap[] = {
	ToolModeMapType( IDM_TOOL_PAN                   , PDF::PDFViewCtrl::e_pan ),
	ToolModeMapType( IDM_TOOL_STRUCTURALSELECTION   , PDF::PDFViewCtrl::e_text_struct_select ),
	ToolModeMapType( IDM_TOOL_RECTANGLESELECTION    , PDF::PDFViewCtrl::e_text_rect_select ),
	ToolModeMapType( IDM_TOOL_RECTANGLEZOOMIN       , PDF::PDFViewCtrl::e_zoom_in ),
	ToolModeMapType( IDM_TOOL_RECTANGLEZOOMOUT      , PDF::PDFViewCtrl::e_zoom_out ),
	ToolModeMapType( IDM_TOOL_ANNOTATIONEDITING     , PDF::PDFViewCtrl::e_annot_edit ),
	ToolModeMapType( IDM_TOOL_TEXTANNOTCREATION     , PDF::PDFViewCtrl::e_text_annot_create ),
	ToolModeMapType( IDM_TOOL_LINECREATION          , PDF::PDFViewCtrl::e_line_create ),
	ToolModeMapType( IDM_TOOL_ARROWCREATION         , PDF::PDFViewCtrl::e_arrow_create ),
	ToolModeMapType( IDM_TOOL_RECTANGLECREATION     , PDF::PDFViewCtrl::e_rect_create ),
	ToolModeMapType( IDM_TOOL_OVALCREATION          , PDF::PDFViewCtrl::e_oval_create ),
	ToolModeMapType( IDM_TOOL_FREEHANDTOOL          , PDF::PDFViewCtrl::e_ink_create ),
	ToolModeMapType( IDM_TOOL_STAMPCREATIONTOOL     , PDF::PDFViewCtrl::e_stamp_create ),
	ToolModeMapType( ID_TOOL_HIGHLIGHTTOOL          , PDF::PDFViewCtrl::e_highlight_create ),
	ToolModeMapType( ID_TOOL_UNDERLINETOOL          , PDF::PDFViewCtrl::e_underline_create ),
	ToolModeMapType( ID_TOOL_STRIKEOUTTOOL          , PDF::PDFViewCtrl::e_strikeout_create ),
	ToolModeMapType( ID_TOOL_SQUIGGLYUNDERLINETOOL  , PDF::PDFViewCtrl::e_squiggly_create ),
	ToolModeMapType( ID_TOOL_CALLOUTTEXTBOXTOOL     , PDF::PDFViewCtrl::e_calloutbox_create ),
	ToolModeMapType( ID_TOOL_SIMPLETEXTBOXTOOL      , PDF::PDFViewCtrl::e_text_box_create ),
	ToolModeMapType( ID_TOOL_POLYGONTOOL            , PDF::PDFViewCtrl::e_polygon_create ),
	ToolModeMapType( ID_TOOL_POLYLINETOOL           , PDF::PDFViewCtrl::e_polyline_create ),
	ToolModeMapType( ID_TOOL_FILEATTACHMENTTOOL     , PDF::PDFViewCtrl::e_file_attachment ),
	ToolModeMapType( ID_TOOL_SOUNDATTACHMENTTOOL    , PDF::PDFViewCtrl::e_sound_attachment ),
	ToolModeMapType( ID_TOOL_MOVIEINSERTIONTOOL     , PDF::PDFViewCtrl::e_movie_attachment ),
	ToolModeMapType( ID_TOOL_CARETPLACEMENTTOOL     , PDF::PDFViewCtrl::e_caret_create ),
	ToolModeMapType( ID_TOOL_REDACTIONTOOL          , PDF::PDFViewCtrl::e_redaction_create ),
	ToolModeMapType( ID_TOOL_TEXTFIELDCREATION      , PDF::PDFViewCtrl::e_text_field_create ),
	ToolModeMapType( ID_TOOL_CHECKBOXCREATIONTOOL   , PDF::PDFViewCtrl::e_check_box_create ),
	ToolModeMapType( ID_TOOL_RADIOBUTTON            , PDF::PDFViewCtrl::e_radio_button_create ),
	ToolModeMapType( ID_TOOL_LISTBOX                , PDF::PDFViewCtrl::e_list_box_create ),
	ToolModeMapType( ID_TOOL_COMBOBOX               , PDF::PDFViewCtrl::e_combo_box_create ),
	ToolModeMapType( ID_TOOL_BUTTONCREATIONTOOL     , PDF::PDFViewCtrl::e_button_create )
};
const ToolModeMapType* const toolModeMapEnd = toolModeMap + sizeof(toolModeMap)/sizeof(ToolModeMapType);
struct ToolModePred {
	int m_code;
	ToolModePred( int c ) : m_code(c) {}
	bool operator()( ToolModeMapType tp ) { return tp.first == m_code; }
};



//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	PDFNet::Initialize();
	// PDFNet::SetColorManagement();

	HWND hWnd;
	hInst = hInstance; // Store instance handle in our global variable

		hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);


	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);

	// Optional: PDFViewCtrl can save its UI state across different sessions.
	// In this case the config file is save in the current working folder.
	//m_view->LoadCurrentConfiguration( "my.cfg" );  
	UpdateWindow(hWnd);

	return TRUE;
}

void ErrorCallback(const char* message, void* custom_data) 
{
	static bool reported_once = false;
	if (!reported_once) {
		reported_once = true;
		HWND hwnd = (HWND) custom_data;  // Case custom data to the expected argument.
		::MessageBoxA(hwnd, message, "Error", MB_OK);
	}
}

void DownloadCallback(DownloadedType type, PDFDoc* doc, int page_num, int obj_num, const char* message, void* custom_data)
{
	switch(type)
	{
		case PDF::e_downloadedtype_opened :
			// e_opened indicates that we have a valid, but incomplete PDFDoc.
			// the PDF should be treated as read only, and only simple functions
			// should be called on the doc, until e_finished has been called.
			break;
		case PDF::e_downloadedtype_page :
			// this indicates the entire page is downloaded and it is safe to modify
			// for example add a new annotation
			break;
		case PDF::e_downloadedtype_finished :
			{
				// we now have the full PDF file and it can be treated like any other
				HWND hwnd = (HWND) custom_data;  // Case custom data to the expected argument.
				if(::MessageBoxA(hwnd, "Download complete, would you like to save the PDF locally?", 
					"PDF Downloaded", MB_YESNO)==IDYES)
				{
					::PostMessage(hwnd, WM_COMMAND, MAKELONG(IDM_FILE_SAVEAS,0), 0);
				}
			}
			break;
		case PDF::e_downloadedtype_failed :
			{
				// downloading has stopped if this occurs
				HWND hwnd = (HWND) custom_data;  // Case custom data to the expected argument.
				::MessageBoxA(hwnd, message, "Error", MB_OK);
			}
			break;
	}
}

TRN_Bool MyActionHandler( TRN_Action act, void* custom_data )
{
	Action action(act);
	if (!action.IsValid()) return 1;

	HWND hwnd = (HWND) custom_data;  // Case custom data to the expected argument.

	Action::Type type = action.GetType();
	if (type == Action::e_GoTo) 
	{
		Destination dest = action.GetDest();
		if (!dest.IsValid()) {
			MessageBoxA(hwnd, "Destination is not valid", "MyURLActionHandler", MB_OK | MB_ICONEXCLAMATION);
		}
		else {
			int page_num = dest.GetPage().GetIndex();
			char buf[256];
			sprintf(buf, "  Links to: page number %d in this document", page_num);
			MessageBoxA(hwnd, buf, "MyURLActionHandler", MB_OK | MB_ICONEXCLAMATION);
			return 0;  // -> execute the default action handler.
		}
	}
	else if (type == Action::e_URI) 
	{						
		UString uri;
		action.GetSDFObj().Get("URI").Value().GetAsPDFText(uri);

		char buf[256];
		sprintf(buf, "  Links to: %s", uri.ConvertToAscii().c_str());
		MessageBoxA(hwnd, buf, "MyURLActionHandler", MB_OK);
	}
	else  
	{
		MessageBoxA(hwnd, "Other Action Type -> Handled by PDFViewCtrl", "MyURLActionHandler", MB_OK);
		return 0;  // -> execute the default action handler.
	}

	// If this function returns true, PDFViewCtrl will not execute the default 
	// action internally (in case of URL the default action is to open the URL 
	// link in a web browser).
	return 1;
}

TRN_Bool MyMouseDoubleClickEvent( PDFViewCtrl::MouseEvent* evt, void* custom_data) 
{
	if (!evt->m_pdfviewctrl_processed) // Display the message box before the message is processed by PDFNet?
	{
		HWND hwnd = (HWND) custom_data;  // Case custom data to the expected argument.
		MessageBoxA(hwnd, "Example of a custom event", "MyMouseDoubleClickEvent", MB_OK);
		return false;  // execute built-in event. To skip event processing in PDFViewCtrl return true.
	}
	return false;
}

BOOL OpenURLProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch( iMessage ){
	case WM_COMMAND:
		switch( LOWORD( wParam ) ){
			case IDOK :
				{
					char url[MAX_PATH];
					char pass[MAX_PATH];
					GetDlgItemTextA(hDlg, IDC_OPENURL_URL, url, MAX_PATH);
					GetDlgItemTextA(hDlg, IDC_OPENURL_PASSWORD, pass, MAX_PATH);
					m_view->CloseDoc();
					mp_doc.reset();
					// Open a PDF file at the given url. This works best with PDF's that
					// are linearized, as pages can be downloaded and viewed in random access order,
					// without the need to download the entire document. A viewing session can also be
					// persisted across multiple viewing/application sessions to remove redundant downloads 
					// and improve overall performance by using the optional cache_file parameter.
					m_view->OpenURLAsync(&url[0], "", &pass[0]);
					// IMPORTANT: PDFViewCtrl 'owns' downloaded documents, so don't take
					// ownership of the document (mp_doc == null)
				}
				// fall through
			case IDCANCEL:
				EndDialog(hDlg, wParam);
				return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static HMENU hMenu ;

	switch (message)
	{
	case WM_CREATE:
		{
			m_view = std::auto_ptr<pdftron::PDF::PDFViewCtrl>(new pdftron::PDF::PDFViewCtrl(hWnd, hInst));
			hMenu = LoadMenu (hInst, MAKEINTRESOURCE(IDC_PDFVIEWSIMPLE));
			hMenu = GetSubMenu (hMenu, 0);

			// m_view->SetProgressiveRendering(false);
			// m_view->SetPageBorderVisibility(false);
			// m_view->SetPageSpacing(0, 0, 0, 0);
			// m_view->SetDefaultPageColor(255, 255, 255);
			// m_view->ShowNavPanel(true);
			// m_view->ShowNavToolbar(true);
			// m_view->SetAntiAliasing( true );
			// m_view->SetGamma(0.01);

			m_view->SetErrorReportHandler(ErrorCallback, hWnd);
			m_view->SetActionHandler(MyActionHandler, hWnd);
			m_view->SetDownloadReportHandler(DownloadCallback, hWnd);

			PDFViewCtrl::EventHandlers my_handlers;
			my_handlers.mouse_left_dclick = MyMouseDoubleClickEvent;
			my_handlers.custom_data = hWnd;
			m_view->SetCustomEventHandlers( &my_handlers );
		}
		break;

	case WM_COMMAND:
		{
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);

			const ToolModeMapType* const wmId_it = std::find_if( toolModeMap, toolModeMapEnd, ToolModePred(wmId) );
			if( wmId_it != toolModeMapEnd )
				m_view->SetToolMode( wmId_it->second );
			else {

				switch (wmId)
				{
				case IDM_ABOUT:
					// DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				case IDM_FILE_PRINT:
					{
						if(m_view->GetDoc())
							m_view->Print();
					}
					break;
				case IDM_FILE_SAVEAS:
					{
						if(m_view->GetDoc())
						{
							TCHAR szFile[256] = { 0 };
							TCHAR *szFilter =TEXT("PDF Files (*.pdf)\0")
								TEXT("*.pdf\0")
								TEXT("All Files\0")
								TEXT("*.*\0");
							OPENFILENAME ofn;
							ZeroMemory(&ofn, sizeof(OPENFILENAME));
							ofn.lStructSize=sizeof(OPENFILENAME);
							ofn.lpstrFilter=szFilter;
							ofn.nFilterIndex=1;
							ofn.lpstrFile=szFile;
							ofn.nMaxFile=sizeof(szFile);
							ofn.lpstrTitle=_T("Save PDF File");
							ofn.nMaxFileTitle=sizeof(ofn.lpstrFile);
							//===Start the File Dialog
							if(GetSaveFileName(&ofn))
							{
								m_view->GetDoc()->Save(UString(ofn.lpstrFile), SDF::SDFDoc::e_linearized, 0);
							}
						}
					}
					break;
				case IDM_FILE_SAVE:
					{
						if(m_view->GetDoc())
						{
							if(m_view->GetDoc())
							{
								m_view->GetDoc()->Save(m_doc_name, SDF::SDFDoc::e_remove_unused, 0);	
							}
						}
					}
					break;
				case IDM_FILE_OPEN:
					{
						//===File path
						TCHAR szFile[256] = { 0 };
						//===Filter
						TCHAR *szFilter =TEXT("PDF Files (*.pdf)\0")
							TEXT("*.pdf\0")
							TEXT("All Files\0")
							TEXT("*.*\0");
						OPENFILENAME ofn;
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize=sizeof(OPENFILENAME);
						ofn.lpstrFilter=szFilter;
						ofn.nFilterIndex=1;
						ofn.lpstrFile=szFile;
						ofn.nMaxFile=sizeof(szFile);
						ofn.lpstrTitle=_T("Open PDF File");
						ofn.nMaxFileTitle=sizeof(ofn.lpstrFile);
						//===Start the File Dialog
						if(GetOpenFileName(&ofn))
						{
							m_doc_name=UString(ofn.lpstrFile);
							try{
								std::auto_ptr<PDF::PDFDoc> doc(new PDF::PDFDoc(m_doc_name));
								if(m_view->SetDoc(*doc)) 
								{
									mp_doc=doc;
									// m_view->ShowNavPanel(true);
									// m_view->ShowNavToolbar(true);
								}
							}
							catch(...)
							{
								::MessageBoxA(hWnd,"An error occurred while opening a file","PDFViewSimple Error",MB_OK);
							}
						}
					}
					break;
				case IDM_FILE_OPENURL:
					{
						::DialogBox(hInst, MAKEINTRESOURCE(IDD_OPENURL), hWnd, DLGPROC(OpenURLProc));
					}
					break;
				case IDM_PAGELAYOUT_SINGLEPAGE:
					{
						m_view->SetPagePresentationMode(PDF::PDFViewCtrl::e_single_page);
					}
					break;
				case IDM_PAGELAYOUT_SINGLECONTINUOUS:
					{
						if(m_view->GetDoc())
						{
							m_view->SetPagePresentationMode(PDF::PDFViewCtrl::e_single_continuous);
						}
					}
					break;
				case IDM_PAGELAYOUT_FACING:
					{
						m_view->SetPagePresentationMode(PDF::PDFViewCtrl::e_facing);
					}
					break;
				case IDM_PAGELAYOUT_FACINGCONTINUOUS:
					{
						if(m_view->GetDoc())
						{
							m_view->SetPagePresentationMode(PDF::PDFViewCtrl::e_facing_continuous);
						}
					}
					break;
				case IDM_PAGELAYOUT_FACING_COVER:
					{
						if(m_view->GetDoc())
						{
							m_view->SetPagePresentationMode(PDF::PDFViewCtrl::e_facing_cover);
						}
					}
					break;
				case IDM_PAGELAYOUT_FACINGCONTINUOUS_COVER:
					{
						if(m_view->GetDoc())
						{
							m_view->SetPagePresentationMode(PDF::PDFViewCtrl::e_facing_continuous_cover);
						}
					}
					break;
				case IDM_PAGEVIEWMODE_FITPAGE:
					{
						m_view->SetPageViewMode(PDF::PDFViewCtrl::e_fit_page);
					}
					break;
				case IDM_PAGEVIEWMODE_FITWIDTH:
					{
						m_view->SetPageViewMode(PDF::PDFViewCtrl::e_fit_width);
					}
					break;
				case IDM_VIEW_ZOOMIN:
					{
						if(m_view->GetDoc())
						{
							m_view->SetZoom(m_view->GetZoom()*2);
						}
					}
					break;
				case IDM_VIEW_ZOOMOUT:
					{
						if(m_view->GetDoc())
						{
							m_view->SetZoom(m_view->GetZoom()/2);
						}
					}
					break;
				case IDM_PAGENAVIGATION_GOTONEXTPAGE:
					{
						if(m_view->GetDoc())
						{
							m_view->GotoNextPage();
						}
					}
					break;
				case IDM_PAGENAVIGATION_GOTOPREVIOUSPAGE:
					{
						if(m_view->GetDoc())
						{
							m_view->GotoPreviousPage();
						}
					}
					break;
				case IDM_PAGENAVIGATION_GOT:
					{
						if(m_view->GetDoc())
						{
							m_view->GotoLastPage();
						}
					}
					break;
				case IDM_PAGENAVIGATION_GOTOFIRSTPAGE:
					{
						if(m_view->GetDoc())
						{
							m_view->GotoFirstPage();
						}
					}
					break;
				case IDM_EDIT_COPY:
					{
						if(m_view->GetDoc())
						{
							m_view->Copy();
						}
					}
					break;
				case IDM_EDIT_FIND:
					{
						if(m_view->GetDoc())
						{
							m_view->Find();
						}
					}
					break;
				case IDM_EDIT_SELECTALL:
					{
						if(m_view->GetDoc())
						{
							m_view->SelectAll();
						}
					}
					break;
				case ID_TOGGLE_NAV_PANEL:
					m_view->ShowNavPanel(!m_view->IsNavPanelVisible());
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		//m_view->SaveCurrentConfiguration("my.cfg");
		m_view.reset();
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		// this is never getting called, wxWidgets handles these events internally
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



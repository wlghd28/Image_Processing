//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------


//  * 
//  * The following sample is a simple example of how to use the PDFViewCtrl class
//  * with wxWidgets.  Since it depends on wxWidgets to run, some project settings
//  * will need to be changed so that the project references wxWidgets
//  *

// Change the following to 1 if wxWidgets library is set-up.
#define USE_WXWIDGETS 0

#if USE_WXWIDGETS
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <memory>


#include <PDF/PDFViewCtrl.h>
#include <PDF/PDFNet.h>

#include <wx/splitter.h>
#include <iostream>

using namespace pdftron;
using namespace PDF;

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnPrint(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);

	void OnCopy(wxCommandEvent& event);
	void OnFind(wxCommandEvent& event);

	void ZoomIn(wxCommandEvent& event);
	void ZoomOut(wxCommandEvent& event);
	void OnPresentationModeChange(wxCommandEvent& event);
	void OnViewModeChange(wxCommandEvent& event);


	void OnToolChange(wxCommandEvent& event);



	void OnClose(wxCloseEvent& event);

	void OnAbout(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);

	void FirstPage(wxCommandEvent& event);
	void PreviousPage(wxCommandEvent& event);
	void NextPage(wxCommandEvent& event);
	void LastPage(wxCommandEvent& event);

	void ShowNavPane(wxCommandEvent& event);

private:
	std::auto_ptr<PDFViewCtrl> m_view;
	std::auto_ptr<PDFDoc> m_doc;
	UString m_doc_name;
	bool m_show_nav_pane;
	DECLARE_EVENT_TABLE()
};


// IDs for the controls and the menu commands
enum
{
	// menu items

	PDFViewWX_Open = wxID_OPEN,
	PDFViewWX_Print = wxID_PRINT,
	PDFViewWX_Save = wxID_SAVE,
	PDFViewWX_SaveAs = wxID_SAVEAS,
	
	PDFViewWX_Quit = wxID_EXIT,


	PDFViewWX_Pan = wxID_LOWEST+800,
	PDFViewWX_StructSelect,
	PDFViewWX_RectSelect,
	PDFViewWX_ZoomInTool,
	PDFViewWX_ZoomOutTool,
	PDFViewWX_AnnotEdit,
	PDFViewWX_LineCreate,
	PDFViewWX_RectCreate,
	PDFViewWX_OvalCreate,
	PDFViewWX_ArrowCreate,
	PDFViewWX_Freehand,

	PDFViewWX_ZoomIn,
	PDFViewWX_ZoomOut,
	PDFViewWX_PresSingle,
	PDFViewWX_PresCont,
	PDFViewWX_PresFacing,
	PDFViewWX_PresFacingCont,
	PDFViewWX_ViewFitWidth,
	PDFViewWX_ViewFitPage,

	PDFViewWX_FirstPage,
	PDFViewWX_PrevPage,
	PDFViewWX_NextPage,
	PDFViewWX_LastPage,

	PDFViewWX_Copy,
	PDFViewWX_Find,

	PDFViewWX_ShowNavPane,

	PDFViewWX_About = wxID_ABOUT
};


// wxWidgets event table
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(PDFViewWX_Open, MainFrame::OnOpen)
EVT_MENU(PDFViewWX_Print, MainFrame::OnPrint)
EVT_MENU(PDFViewWX_Quit,  MainFrame::OnQuit)
EVT_MENU(PDFViewWX_Save,  MainFrame::OnSave)
EVT_MENU(PDFViewWX_SaveAs, MainFrame::OnSaveAs)
EVT_MENU(PDFViewWX_Copy, MainFrame::OnCopy)
EVT_MENU(PDFViewWX_Find, MainFrame::OnFind)
EVT_MENU_RANGE(	PDFViewWX_Pan, PDFViewWX_Freehand, MainFrame::OnToolChange)
EVT_MENU(PDFViewWX_ZoomIn, MainFrame::ZoomIn)
EVT_MENU(PDFViewWX_ZoomOut, MainFrame::ZoomOut)
EVT_CLOSE(MainFrame::OnClose) 
EVT_MENU_RANGE( PDFViewWX_PresSingle, PDFViewWX_PresFacingCont, MainFrame::OnPresentationModeChange)
EVT_MENU_RANGE( PDFViewWX_ViewFitWidth, PDFViewWX_ViewFitPage, MainFrame::OnViewModeChange)
EVT_MENU(PDFViewWX_FirstPage, MainFrame::FirstPage)
EVT_MENU(PDFViewWX_PrevPage, MainFrame::PreviousPage)
EVT_MENU(PDFViewWX_NextPage, MainFrame::NextPage)
EVT_MENU(PDFViewWX_LastPage, MainFrame::LastPage)
EVT_MENU(PDFViewWX_ShowNavPane, MainFrame::ShowNavPane)
EVT_MENU(PDFViewWX_About, MainFrame::OnAbout)


EVT_SIZE(MainFrame::OnSize)
END_EVENT_TABLE()


IMPLEMENT_APP(MyApp)

// the program execution "starts" here
bool MyApp::OnInit()
{
	
	if ( !wxApp::OnInit() )
		return false;

	// create the main application window and show it
	MainFrame *frame = new MainFrame(_T("PDFTron PDFViewWX"));
	frame->Show(true);

	try {
		PDFNet::Initialize();
	}
	catch (Common::Exception& ex) {
		std::cout << ex.what() << std::endl;
	} 
	return true;
}


// simple demonstration of a callback executed when action is triggered in a PDF doc
TRN_Bool trn_action_callback( TRN_Action action, void* custom_data ) {
    pdftron::PDF::Action act( (TRN_Action)action );
    Action::Type type = act.GetType();
    
    return 0;
}

// frame constructor
MainFrame::MainFrame(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(700,700)), 
//	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(700,700), (wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN)), 
	m_view(0)
{
	// create a menu bar
	wxMenu *fileMenu = new wxMenu;

	wxMenu *editMenu = new wxMenu;

	wxMenu *viewMenu = new wxMenu;

	wxMenu *navigationMenu=new wxMenu;


	wxMenu *toolMenu = new wxMenu;

	// the "About" item should be in the help menu
	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(PDFViewWX_About, _T("&About...\tF1"), _T("Show about dialog"));


	fileMenu->Append(PDFViewWX_Open, _T("&Open\tCtrl-O"), _T("Open a document"));
	fileMenu->Append(PDFViewWX_Save, _T("&Save\tCtrl-S"), _T("Save the document"));
	fileMenu->Append(PDFViewWX_SaveAs, _T("Save As"), _T("Save the document as"));
	
	fileMenu->Append(PDFViewWX_Print, _T("&Print\tCtrl-P"), _T("Print the Document"));
	fileMenu->Append(PDFViewWX_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));
	

	editMenu->Append(PDFViewWX_Copy, _T("&Copy\tCtrl-C"), _T("Copy the Selected Text"));
	editMenu->Append(PDFViewWX_Find, _T("&Find\tCtrl-F"), _T("Open the Find Dialog"));




	wxMenu *pageLayout = new wxMenu;
	pageLayout->AppendRadioItem(PDFViewWX_PresSingle, _T("Single Page"),_T("Single Page Mode"));
	pageLayout->AppendRadioItem(PDFViewWX_PresCont, _T("Single Continuous"),_T("Continuous Mode"));
	pageLayout->Check(PDFViewWX_PresCont, true);
	pageLayout->AppendRadioItem(PDFViewWX_PresFacing, _T("Facing"), _T("Facing Mode"));
	pageLayout->AppendRadioItem(PDFViewWX_PresFacingCont, _T("Facing Continuous"), _T("Facing Continuous Mode"));
	viewMenu->AppendSubMenu(pageLayout,_T("Page Layout"));

	wxMenu *viewMode = new wxMenu;
	viewMode->Append(PDFViewWX_ViewFitPage,_T("Fit Page"),_T("Make the current page entirely visible"));
	viewMode->Append(PDFViewWX_ViewFitWidth, _T("Fit Width"), _T("Fit the page width in the available region"));
	viewMenu->AppendSubMenu(viewMode, _T("Page View Mode")); 

	viewMenu->Append(PDFViewWX_ZoomIn,_T("Zoom In"));
	viewMenu->Append(PDFViewWX_ZoomOut, _T("Zoom Out"));
	

	navigationMenu->Append(PDFViewWX_FirstPage, _T("First Page"));
	navigationMenu->Append(PDFViewWX_PrevPage, _T("Previous Page"));
	navigationMenu->Append(PDFViewWX_NextPage, _T("Next Page"));
	navigationMenu->Append(PDFViewWX_LastPage, _T("Last Page"));
	navigationMenu->AppendSeparator();
	navigationMenu->Append(PDFViewWX_ShowNavPane, wxT("Show Navigation Pane"));


	toolMenu->AppendRadioItem(PDFViewWX_Pan, _T("Pan"), _T("Use the Pan Tool"));
	toolMenu->AppendRadioItem(PDFViewWX_StructSelect, _T("Structural Selection"), _T("Use the Structural Text Selection Tool"));
	toolMenu->AppendRadioItem(PDFViewWX_RectSelect, _T("Rectangular Selection"), _T("Use the Rectangular Text Selection Tool"));	
	toolMenu->AppendRadioItem(PDFViewWX_ZoomInTool, _T("Rectangular Zoom In"), _T("Use the Rectangular Zoom In Tool"));	
	toolMenu->AppendRadioItem(PDFViewWX_ZoomOutTool, _T("Rectangular Zoom Out"), _T("Use the Rectangular Zoom Out Tool"));	
	toolMenu->AppendRadioItem(PDFViewWX_AnnotEdit, _T("Annotation Editing"), _T("Use the Annotation Editing Tool"));	
	toolMenu->AppendRadioItem(PDFViewWX_LineCreate, _T("Line Creation"), _T("Use the Line Creation Tool"));	
	toolMenu->AppendRadioItem(PDFViewWX_RectCreate, _T("Rectangle Creation"), _T("Use the Rectangle Creation Tool"));
	toolMenu->AppendRadioItem(PDFViewWX_OvalCreate, _T("Oval Creation"), _T("Use the Oval Creation Tool"));
	toolMenu->AppendRadioItem(PDFViewWX_ArrowCreate, _T("Arrow Creation"), _T("Use the Arrow Creation Tool"));
	toolMenu->AppendRadioItem(PDFViewWX_Freehand, _T("Freehand Tool"), _T("Use the Freehand Drawing Tool"));

		
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, _T("&File"));
	menuBar->Append(editMenu, _T("&Edit"));
	menuBar->Append(viewMenu, _T("&View"));
	menuBar->Append(navigationMenu, _T("&Navigation"));
	menuBar->Append(toolMenu, _T("&Tool"));
	menuBar->Append(helpMenu, _T("&Help"));

	SetMenuBar(menuBar);
#ifdef __WXMSW__
	PDFViewCtrl * vcptr = new PDFViewCtrl(this->GetHWND(), wxGetApp().GetInstance());
#else
	PDFViewCtrl * vcptr = new PDFViewCtrl(this, 0 );
#endif
	m_view=std::auto_ptr<PDFViewCtrl>(vcptr);
	m_show_nav_pane = false;
	m_view->ShowNavPanel(m_show_nav_pane);
	m_view->ShowNavToolbar(m_show_nav_pane);
    m_view->SetActionHandler( trn_action_callback, 0 );	
	wxRect r=GetClientRect();
	m_view->OnSize(r.x,r.y,r.width,r.height);
}


// event handlers



void MainFrame::OnClose(wxCloseEvent& event)
{
	m_view.reset();
	wxFrame::OnCloseWindow(event);
}

void MainFrame::OnSize(wxSizeEvent& e)
{
	wxRect r=GetClientRect();
	if(m_view.get())
		m_view->OnSize(r.x,r.y,r.width,r.height);
}



void MainFrame::OnPrint(wxCommandEvent& event)
{
	if(m_view.get() && m_view->GetDoc())
		m_view->Print();
}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	m_view.reset();
	Close(true);
}

void MainFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog* d=new wxFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("*.*"), wxFD_OPEN);
	if(d->ShowModal()==wxID_OK)
	{
		wxString path=d->GetPath(); 
#if wxUSE_UNICODE
		m_doc_name=path.ToAscii().release();
#else
		m_doc_name=path.ToAscii();
#endif
		std::auto_ptr<PDFDoc> doc(new PDF::PDFDoc(m_doc_name));
		// replace the old document if the new one was set successfully
		if( m_view.get() && m_view->SetDoc(*doc) ) {
			m_doc=doc;
		}
	}
}


void MainFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	if( m_view.get() && m_view->GetDoc())
	{
		UString to_save(m_doc_name);
		m_view->DocLock(true);
		try {
		m_view->GetDoc()->Save(to_save, SDF::SDFDoc::e_remove_unused, 0);
		} catch (...) {}
		m_view->DocUnlock();
	}
}


void MainFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog* d=new wxFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("*.*"), wxFD_SAVE);
	if(d->ShowModal()==wxID_OK)
	{
		wxString path=d->GetPath();
#if wxUSE_UNICODE
		const char * name=path.ToAscii().release();
#else
		const char * name=path.ToAscii();
#endif
		UString to_save(name);
		if(m_view.get() && m_view->GetDoc())
		{
			m_view->DocLock(true);
			try {
				m_view->GetDoc()->Save(to_save, SDF::SDFDoc::e_remove_unused, 0);
			} catch (...) {}
			m_view->DocUnlock();
		}
	}
}

void MainFrame::OnCopy(wxCommandEvent& event)
{
	if( m_view.get() )
		m_view->Copy();
}

void MainFrame::OnFind(wxCommandEvent& event)
{
	if( m_view.get() )
		m_view->Find();
}


void MainFrame::ZoomIn(wxCommandEvent& event)
{
 if( m_view.get() && m_view->GetDoc())
	{
		m_view->SetZoom(m_view->GetZoom()*2);
	}
}

void MainFrame::ZoomOut(wxCommandEvent& event)
{
	if(  m_view.get() && m_view->GetDoc())
	{
		m_view->SetZoom(m_view->GetZoom()/2);
	}
}


void MainFrame::OnPresentationModeChange(wxCommandEvent& event)
{
	if( m_view.get() ) {
		switch(event.GetId())
		{
			case PDFViewWX_PresSingle:
				m_view->SetPagePresentationMode(PDFViewCtrl::e_single_page);
			break;
			case PDFViewWX_PresCont:
				m_view->SetPagePresentationMode(PDFViewCtrl::e_single_continuous);
			break;
			case PDFViewWX_PresFacing:
				m_view->SetPagePresentationMode(PDFViewCtrl::e_facing);
			break;
			case PDFViewWX_PresFacingCont:
				m_view->SetPagePresentationMode(PDFViewCtrl::e_facing_continuous);
			break;
		}
	}
}

void MainFrame::OnViewModeChange(wxCommandEvent& event)
{
	if( m_view.get() ) {
		switch(event.GetId())
		{
			case PDFViewWX_ViewFitPage:
				m_view->SetPageViewMode(PDFViewCtrl::e_fit_page);
			break;
			case PDFViewWX_ViewFitWidth:
				m_view->SetPageViewMode(PDFViewCtrl::e_fit_width);
			break;
		}
	}
}


void MainFrame::FirstPage(wxCommandEvent& event)
{
if( m_view.get() ) 
	m_view->GotoFirstPage();
}
void MainFrame::PreviousPage(wxCommandEvent& event)
{
	if( m_view.get() ) 
		m_view->GotoPreviousPage();
}
void MainFrame::NextPage(wxCommandEvent& event)
{
	if( m_view.get() ) 
		m_view->GotoNextPage();	
}
void MainFrame::LastPage(wxCommandEvent& event)
{
if( m_view.get() ) 
	m_view->GotoLastPage();
}


void MainFrame::OnToolChange(wxCommandEvent& event)
{
	if ( m_view.get() )
	{
		switch (event.GetId())
		{
		case PDFViewWX_Pan:
			m_view->SetToolMode(PDFViewCtrl::e_pan);
			break;
		case PDFViewWX_StructSelect:
			m_view->SetToolMode(PDFViewCtrl::e_text_struct_select);
			break;
		case PDFViewWX_RectSelect:
			m_view->SetToolMode(PDFViewCtrl::e_text_rect_select);
			break;
		case PDFViewWX_ZoomIn:
			m_view->SetToolMode(PDFViewCtrl::e_zoom_in);
			break;
		case PDFViewWX_ZoomOut:
			m_view->SetToolMode(PDFViewCtrl::e_zoom_out);
			break;
		case PDFViewWX_AnnotEdit:
			m_view->SetToolMode(PDFViewCtrl::e_annot_edit);
			break;
		case PDFViewWX_LineCreate:
			m_view->SetToolMode(PDFViewCtrl::e_line_create);
			break;
		case PDFViewWX_RectCreate:
			m_view->SetToolMode(PDFViewCtrl::e_rect_create);
			break;
		case PDFViewWX_OvalCreate:
			m_view->SetToolMode(PDFViewCtrl::e_oval_create);
			break;
		case PDFViewWX_ArrowCreate:
			m_view->SetToolMode(PDFViewCtrl::e_arrow_create);
			break;
		case PDFViewWX_Freehand:
			m_view->SetToolMode(PDFViewCtrl::e_ink_create);
			break;
		case PDFViewWX_ZoomInTool:
			m_view->SetToolMode(PDFViewCtrl::e_zoom_in);
			break;
		case PDFViewWX_ZoomOutTool:
			m_view->SetToolMode(PDFViewCtrl::e_zoom_out);
			break;
	
		default:
	
			break;
		}
	}
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox(wxString::Format(
		_T("Welcome to PDFViewWX!\n")
		_T("\n")
		_T("This is a simple demonstration of the new PDFViewCtrl class!\n")
		_T("running in wxWidgets.")
		),
		_T("About PDFViewWX"),
		wxOK | wxICON_INFORMATION,
		this);
}

void MainFrame::ShowNavPane(wxCommandEvent& WXUNUSED(event))
{
	m_show_nav_pane = !m_show_nav_pane;
	m_view->ShowNavPanel(m_show_nav_pane);
	m_view->ShowNavToolbar(m_show_nav_pane);
}
#else // USE_WXWIDGETS

#include <Windows.h>

int WINAPI WinMain(__in HINSTANCE, __in_opt HINSTANCE, __in LPSTR, __in int)
{
    return MessageBoxW(NULL, L"wxWidgets is required for this sample to compile and run. Please see readme.txt for more information.", L"PDFViewWXVC", MB_OK | MB_ICONWARNING);
}

#endif // USE_WXWIDGETS
// PDFView.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PDFViewApp.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "PDFViewDoc.h"
#include "LeftView.h"

#include "hlinkctl.h"	// link control for the about box
#include "StdSecurityHandlerUI.h"
#include <PDF/PDFNet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPDFViewApp

BEGIN_MESSAGE_MAP(CPDFViewApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinAppEx::OnFileOpen)
//	ON_UPDATE_COMMAND_UI(ID_SMALLTHUMBNAILS, OnUpdateSmallthumbnails)
//	ON_UPDATE_COMMAND_UI(ID_LARGETHUMBNAILS, OnUpdateLargethumbnails)
	ON_COMMAND(ID_FILE_CLOSEALL, OnFileCloseall)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)

	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CPDFViewApp construction

CPDFViewApp::CPDFViewApp()
{
//	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPDFViewApp object

CPDFViewApp theApp;

// For multi select dialog box
class CMultiOpenDocManager : public CDocManager
{
public:
	CMultiOpenDocManager() { }
	virtual void OnFileOpen();
	virtual BOOL DoPromptFileNames(CStringList& fileNames, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);
};


// CPDFViewApp initialization

BOOL CPDFViewApp::InitInstance()
{
	// For multi select dialog box
	m_pDocManager = new CMultiOpenDocManager();

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	SetRegistryKey(_T("PDFTron\\PDFView 4.0"));

	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_PDFTYPE,
		RUNTIME_CLASS(CPDFViewDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CLeftView));
	AddDocTemplate(pDocTemplate);

	try {
		PDFNet::Initialize();
	}
	catch (Common::Exception& ex) {
		::AfxMessageBox(CString(ex.GetMessage()), MB_OK, 0);
	}

	try	{
		// Optional: Set ICC color profiles to fine tune color conversion 
		// for PDF 'device' color spaces. You can use your own ICC profiles. 
		// Standard Adobe color profiles can be download from Adobes site: 
		// http://www.adobe.com/support/downloads/iccprofiles/iccprofiles_win.html
		//
		// Simply drop all *.icc files in PDFNet resource folder or you specify 
		// the full pathname.
		//---
		// PDFNet::SetColorManagement();
		// PDFNet::SetDefaultDeviceCMYKProfile("USWebCoatedSWOP.icc"); // will search in PDFNet resource folder.
		// PDFNet::SetDefaultDeviceRGBProfile("AdobeRGB1998.icc"); 

		// Optional: Set predefined font mappings to override default font 
		// substitution for documents with missing fonts. For example:
		//---
		// PDFNet::AddFontSubst("StoneSans-Semibold", "C:/WINDOWS/Fonts/comic.ttf");
		// PDFNet::AddFontSubst("StoneSans", "C:/WINDOWS/Fonts/comic.ttf");
		// PDFNet::AddFontSubst(PDFNet::e_Identity, "C:/WINDOWS/Fonts/arialuni.ttf");
		// PDFNet::AddFontSubst(PDFNet::e_Japan1, "C:/Program Files/Adobe/Acrobat 7.0/Resource/CIDFont/KozMinProVI-Regular.otf");
		// PDFNet::AddFontSubst(PDFNet::e_Japan2, "c:/myfonts/KozMinProVI-Regular.otf");
		//
		// If fonts are in PDFNet resource folder, it is not necessary to specify 
		// the full path name. For example,
		//---
		// PDFNet::AddFontSubst(PDFNet::e_Korea1, "AdobeMyungjoStd-Medium.otf");
		// PDFNet::AddFontSubst(PDFNet::e_CNS1, "AdobeSongStd-Light.otf");
		// PDFNet::AddFontSubst(PDFNet::e_GB1, "AdobeMingStd-Light.otf");
	} 
	catch (...) {
		::AfxMessageBox(_T("The specified color profile was not found"), MB_OK, 0);
	}

	// Register security handlers.
	PDFNet::RegisterSecurityHandler("Standard", "Standard Security", SecurityHandlerUI::Create);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	
	m_pMainWnd->DragAcceptFiles();	// Enable drag/drop open
	
	EnableShellOpen();				// Enable DDE Execute open
	RegisterShellFileTypes(TRUE);

	CCommandLineInfo cmdInfo;		// Parse command line for standard shell commands, DDE, file open
	ParseCommandLine(cmdInfo);

	// Do not open empty document at start
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) 
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	return TRUE;
}

int CPDFViewApp::ExitInstance()
{
	PDFNet::Terminate(); 
	return CWinAppEx::ExitInstance();
}


// CAboutDlg dialog used for App About
//------------------------------------------------------------------------
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHLinkCtrl 	m_web_link;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
 	//}}AFX_DATA_INIT
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_web_link.SetHyperLink(_T("http://www.pdftron.com"));
	return TRUE;  
}

void CAboutDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_WEB_LINK, m_web_link);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// App command to run the dialog
void CPDFViewApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CMultiOpenDocManager Implementation used to select multiple documents in FileOpen dlg box
//---------------------------------------------------------------------------

void CMultiOpenDocManager::OnFileOpen()
{
	CStringList newNames;
	if (!DoPromptFileNames(newNames, AFX_IDS_OPENFILE,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, TRUE, NULL))
		return; // open cancelled

	POSITION pos = newNames.GetHeadPosition();
	while (pos)
	{
		CString newName = newNames.GetNext(pos);
		AfxGetApp()->OpenDocumentFile(newName);
	}
}

void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	CDocTemplate* pTemplate, CString* pstrDefaultExt)
{
	ASSERT_VALID(pTemplate);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CString strFilterExt, strFilterName;
	if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) &&
	 !strFilterExt.IsEmpty() &&
	 pTemplate->GetDocString(strFilterName, CDocTemplate::filterName) &&
	 !strFilterName.IsEmpty())
	{
		// a file based document template - add to filter list
		ASSERT(strFilterExt[0] == '.');
		if (pstrDefaultExt != NULL)
		{
			// set the default extension
			*pstrDefaultExt = ((LPCTSTR)strFilterExt) + 1;  // skip the '.'
			ofn.lpstrDefExt = (LPTSTR)(LPCTSTR)(*pstrDefaultExt);
			ofn.nFilterIndex = ofn.nMaxCustFilter + 1;  // 1 based number
		}

		// add to filter
		filter += strFilterName;
		ASSERT(!filter.IsEmpty());  // must have a file type name
		filter += (TCHAR)'\0';  // next string please
		filter += (TCHAR)'*';
		filter += strFilterExt;
		filter += (TCHAR)'\0';  // next string please
		ofn.nMaxCustFilter++;
	}
}

BOOL CMultiOpenDocManager::DoPromptFileNames(CStringList& fileNames, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
{
	CFileDialog dlgFile(bOpenFileDialog, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL);

	CString title;
	VERIFY(title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;
	CString strDefault;
	if (pTemplate != NULL)
	{
		ASSERT_VALID(pTemplate);
		AppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate, &strDefault);
	}
	else
	{
		// do for all doc template
		POSITION pos = m_templateList.GetHeadPosition();
		BOOL bFirst = TRUE;
		while (pos != NULL)
		{
			CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
			AppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate,
				bFirst ? &strDefault : NULL);
			bFirst = FALSE;
		}
	}

	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;

	// --- Begin modifications ---
	// - use a big buffer for the file names 
	// (note that pre-SP2 versions of NT 4.0 will nevertheless
	// truncate the result)
	CString strFileNames;
	dlgFile.m_ofn.lpstrFile = strFileNames.GetBuffer(10*_MAX_PATH);
	dlgFile.m_ofn.nMaxFile = 10*_MAX_PATH;

	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	strFileNames.ReleaseBuffer();

	if (!bResult)
		return FALSE; // open cancelled

	// - copy the file names to a string list
	POSITION pos = dlgFile.GetStartPosition();
	while (pos)
	{
		fileNames.AddTail(dlgFile.GetNextPathName(pos));
	}
	return TRUE;
	// --- End modifications ---
}

void CPDFViewApp::OnFileCloseall()
{
	SaveAllModified();
	CloseAllDocuments(FALSE);
}

void CPDFViewApp::OnFileSaveAs()
{
	SaveAllModified();	
}

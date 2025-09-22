// PDFViewDoc.cpp : implementation of the CPDFViewDoc class
//

#include "stdafx.h"

// Undefine this macro since it collides with Common::Exception::GetMessage()
#undef GetMessage

#include "PDFViewApp.h"
#include "PDFViewDoc.h"
#include "Security.h"
#include "DocPropCreation.h"
#include "DocPropSecurity.h"
#include "AuthorizeDlg.h"

#include <Filters/MappedFile.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPDFViewDoc

IMPLEMENT_DYNCREATE(CPDFViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CPDFViewDoc, CDocument)
//	ON_UPDATE_COMMAND_UI(ID_SMALLTHUMBNAILS, OnUpdateSmallthumbnails)
ON_COMMAND(ID_FILE_DOCUMENTSECURITY, OnFileDocumentSecurity)
ON_COMMAND(ID_FILE_DOCUMENTPROPERTIES, OnFileDocumentProperties)
END_MESSAGE_MAP()


// CPDFViewDoc construction/destruction

CPDFViewDoc::CPDFViewDoc()
{
	// TODO: add one-time construction code here

}

CPDFViewDoc::~CPDFViewDoc()
{
}

BOOL CPDFViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// Add reinitialization code here
	mp_doc = std::auto_ptr<PDF::PDFDoc>(new PDF::PDFDoc());

	return TRUE;
}

// CPDFViewDoc diagnostics

#ifdef _DEBUG
void CPDFViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPDFViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

BOOL CPDFViewDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CWaitCursor wait;
	try	{	
		mp_doc = std::auto_ptr<PDF::PDFDoc>(new PDF::PDFDoc(UString(lpszPathName)));
		return mp_doc->InitSecurityHandler();
	}
	catch (Common::Exception& ex) {
		AfxGetApp()->m_pMainWnd->MessageBox(CString(ex.GetMessage()), _T("Error"), MB_OK | MB_ICONHAND);
	}
	catch (...) {
		AfxGetApp()->m_pMainWnd->MessageBox(_T("An unknown error occurred while opening the file."), _T("Error"), MB_OK | MB_ICONHAND);
	}

	return FALSE;	// Document not opened successfully
}

void CPDFViewDoc::DeleteContents()
{
	CWaitCursor wait;
	mp_doc = std::auto_ptr<PDF::PDFDoc>(0);
	CDocument::DeleteContents();
}

BOOL CPDFViewDoc::IsModified() 
{	
	if (mp_doc.get()) return mp_doc->IsModified(); 
	else return false;
}

void CPDFViewDoc::OnFileDocumentSecurity()
{
	CDocSecurity dlg(*mp_doc);
	dlg.DoModal();
}

BOOL CPDFViewDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// Read the Save preferences 
	UInt32 flags = 0; 
	bool incremental = false;
	bool remove_unused = false;

	if (incremental) {	
		flags |= SDF::SDFDoc::e_incremental;
	}
	else {
		remove_unused = true;
		if (remove_unused) flags |= SDF::SDFDoc::e_remove_unused;
	}

	try {
		mp_doc->Save(UString(lpszPathName), flags, 0);
	}
	catch (Common::Exception& e) {
		AfxGetApp()->m_pMainWnd->MessageBox(CString(e.GetMessage()), _T("Save Error"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	catch (...) {
		AfxGetApp()->m_pMainWnd->MessageBox(_T("An unknown error occurred while saving the data."), _T("Save Error"), MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	return TRUE; 
}

pdftron::UString GetPDFTextStr(const CString& str) 
{
#ifdef _UNICODE
	return pdftron::UString((LPCWSTR) str);
#else
	return pdftron::UString((LPCSTR) str);
#endif
}

void CPDFViewDoc::OnFileDocumentProperties()
{
	CPropertySheet doc_prop(_T("Document Properties"));
	
	CDocPropCreation creation;
	PDF::PDFDocInfo info = mp_doc->GetDocInfo();

	creation.m_autor = info.GetAuthor().ConvertToAscii().c_str(); 
	creation.m_creator = info.GetCreator().ConvertToAscii().c_str();
	creation.m_producer = info.GetProducer().ConvertToAscii().c_str();
	creation.m_subject = info.GetSubject().ConvertToAscii().c_str();
	creation.m_title = info.GetTitle().ConvertToAscii().c_str();
	creation.m_keywords = info.GetKeywords().ConvertToAscii().c_str();

	SDF::Obj info_dict = info.GetSDFObj();
	if (info_dict) {
		UString s;
		try { 
			info_dict.Get("CreationDate").Value().GetAsPDFText(s);
			creation.m_created_date = s.ConvertToAscii().c_str();
		} catch (...) {}

		try { 
			info_dict.Get("ModDate").Value().GetAsPDFText(s);
			creation.m_mod_date = s.ConvertToAscii().c_str();
		} catch (...) {}
	}

	creation.m_file = GetPathName();

	SDF::SecurityHandler sec = mp_doc->GetSecurityHandler();
	if (sec) {
		creation.m_security = "Document is secured.";
	}
	else {
		creation.m_security = "Document is not secured.";
	}

	doc_prop.AddPage(&creation);

	CDocPropSecurity security;

	if (sec) {
		security.m_is_secured = true;
		security.m_hdlr = mp_doc->GetSecurityHandler();
	}

	doc_prop.AddPage(&security);

	if (doc_prop.DoModal() == IDOK)
	{
		if (creation.m_is_data_modified) 
		{
			info.SetTitle(GetPDFTextStr(creation.m_title));
			info.SetSubject(GetPDFTextStr(creation.m_subject));
			info.SetAuthor(GetPDFTextStr(creation.m_autor));
			info.SetKeywords(GetPDFTextStr(creation.m_keywords));
			info.SetCreator(GetPDFTextStr(creation.m_creator));
			info.SetProducer(GetPDFTextStr(creation.m_producer));	
		}
	}
}

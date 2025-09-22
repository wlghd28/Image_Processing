// PDFViewDoc.h : interface of the CPDFViewDoc class
//


#pragma once

#include <memory>
#include <PDF/PDFDoc.h>
#include <PDF/PDFView.h>

using namespace pdftron;

class CPDFViewDoc : public CDocument
{
protected: // create from serialization only
	CPDFViewDoc();
	DECLARE_DYNCREATE(CPDFViewDoc)

// Attributes
public:

	BOOL IsModified();
	std::auto_ptr<PDF::PDFDoc> mp_doc;

	int GetCurrentPage() 
	{
		return 1;
	}

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();

// Implementation
public:
	virtual ~CPDFViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();

	afx_msg void OnFileDocumentSecurity();
	afx_msg void OnFileDocumentProperties();
};



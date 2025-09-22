#if !defined(AFX_DOCPROPCREATION_H__99D90E14_5DFC_4861_99B0_7CA75C11B5CE__INCLUDED_)
#define AFX_DOCPROPCREATION_H__99D90E14_5DFC_4861_99B0_7CA75C11B5CE__INCLUDED_

#pragma once
// DocPropCreation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDocPropCreation dialog

class CDocPropCreation : public CPropertyPage
{
	DECLARE_DYNCREATE(CDocPropCreation)

// Construction
public:
	CDocPropCreation();
	~CDocPropCreation();

// Dialog Data
	//{{AFX_DATA(CDocPropCreation)
	enum { IDD = IDD_DOCPROP_CREATION };
	CString	m_autor;
	CString	m_created_date;
	CString	m_creator;
	CString	m_file;
	CString	m_keywords;
	CString	m_mod_date;
	CString	m_producer;
	CString	m_security;
	CString	m_subject;
	CString	m_title;
	//}}AFX_DATA

	bool m_is_data_modified;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDocPropCreation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDocPropCreation)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCPROPCREATION_H__99D90E14_5DFC_4861_99B0_7CA75C11B5CE__INCLUDED_)

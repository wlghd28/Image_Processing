#if !defined(AFX_DOCPROPSECURITY_H__00C8EAD2_3EE6_4B9D_8511_B6576A67CB60__INCLUDED_)
#define AFX_DOCPROPSECURITY_H__00C8EAD2_3EE6_4B9D_8511_B6576A67CB60__INCLUDED_

#pragma once
// DocPropSecurity.h : header file
//

#include <SDF/SecurityHandler.h>

using namespace pdftron;

/////////////////////////////////////////////////////////////////////////////
// CDocPropSecurity dialog

class CDocPropSecurity : public CPropertyPage
{
	DECLARE_DYNCREATE(CDocPropSecurity)

// Construction
public:
	CDocPropSecurity();
	~CDocPropSecurity();

	bool m_is_secured;
	SDF::SecurityHandler m_hdlr;

// Dialog Data
	//{{AFX_DATA(CDocPropSecurity)
	enum { IDD = IDD_DOCPROP_SECURITY };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDocPropSecurity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDocPropSecurity)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCPROPSECURITY_H__00C8EAD2_3EE6_4B9D_8511_B6576A67CB60__INCLUDED_)

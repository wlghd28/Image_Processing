#if !defined(AFX_TABVIEW_H__207AF890_DCF0_42E0_81C4_A506BE71ABC7__INCLUDED_) 
#define AFX_TABVIEW_H__207AF890_DCF0_42E0_81C4_A506BE71ABC7__INCLUDED_ 


#pragma once
// TabView.h : header file 
// 


#ifdef _WIN32 
#define WC_TABVIEWA "SysTabControl32" 
#define WC_TABVIEWW L"SysTabControl32" 


#ifdef UNICODE 
#define WC_TABVIEW WC_TABVIEWW 
#else 
#define WC_TABVIEW WC_TABVIEWA 
#endif 


#else 
#define WC_TABVIEW "SysTabControl" 
#endif 


#include <afxtempl.h> 


///////////////////////////////////////////////////////////////////////////// 
// CTabView view 


class CTabView : public CCtrlView 
{ 
	DECLARE_DYNCREATE(CTabView) 


	// Construction 
public: 
	CTabView(); 
	virtual ~CTabView(); 


	// Attributes 
public: 
	CTabCtrl& GetTabCtrl() const; 


protected: 
	void RemoveImageList(); 


	// Operations 
public: 
	void HidePage(int nPage); 
	void RemoveView(int nItem); // Removes a view 
	void RemoveView(CView * pView); // Removes a view 
	UINT GetPageID(int nPage); // Get the ID of the page 
	void ActivateView(int nItem); // Activates a view (tab) 
	CView * GetView(int nView); // Return the corresponding view 

	// Insert a view at a given page 
	int Insert(int nPage, LPCTSTR lpszItemText, CRuntimeClass * pNewWnd, 
		CCreateContext * pContext = NULL); 
	// Insert a view as the last page 
	int Insert(LPCTSTR lpszItemText, CRuntimeClass *pNewWnd, 
		CCreateContext *pContext = NULL); 

	// Set the height of the tab font (9 is a good value) 
	void SetFontHeight(int nHeight); 

	// Overrides 
	// ClassWizard generated virtual function overrides 
	//{{AFX_VIRTUAL(CTabView) 
public: 
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct); 
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, 
		LRESULT* pLResult); 
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, 
		AFX_CMDHANDLERINFO* pHandlerInfo); 
protected: 
	virtual void PreSubclassWindow(); 
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint); 
	//}}AFX_VIRTUAL 


	// Implementation 
protected: 
	CFont m_Font; 
	CMapWordToPtr m_aPages; 
	CMap<int, int, UINT, UINT> m_aPageIDs; 


	UINT m_uLastID; 


	// Generated message map functions 
protected: 
	// CWnd * m_pDummyWnd; 
	CFrameWnd * m_pMainFrame; 
	void RecalcLayout(CWnd *pCurWnd = NULL); // Fits the view into the client area of the tab 
	UINT GetNewPageID(int nPage); // Gets a new available ID for a page 

	//{{AFX_MSG(CTabView) 
	afx_msg void OnDestroy(); 
	afx_msg void OnSize(UINT nType, int cx, int cy); 
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); 
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point); 
	//}}AFX_MSG 
	DECLARE_MESSAGE_MAP() 
}; 


inline CTabView::CTabView() : CCtrlView(WC_TABVIEW, AFX_WS_DEFAULT_VIEW) 
{ 
	m_uLastID = AFX_IDW_PANE_FIRST + 128; // Leave room for 127 other views :-) 
} 


inline CTabCtrl& CTabView::GetTabCtrl() const 
{ 
	return *(CTabCtrl*)this; 
} 

///////////////////////////////////////////////////////////////////////////// 

//{{AFX_INSERT_LOCATION}} 
// Microsoft Visual C++ will insert additional declarations immediately before the previous line. 


#endif




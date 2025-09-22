
// PrintDC_TESTDlg.h : 헤더 파일
//

#pragma once


// CPrintDC_TESTDlg 대화 상자
class CPrintDC_TESTDlg : public CDialogEx
{
// 생성입니다.
public:
	CPrintDC_TESTDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRINTDC_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 메서드
	afx_msg void OnBnClickedMfcbuttonPrint();

	// 헤드크기(벤딩길이) 픽셀단위
	int int_head_banding_length = 600;
	


};

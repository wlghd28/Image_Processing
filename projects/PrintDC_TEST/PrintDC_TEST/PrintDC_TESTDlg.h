
// PrintDC_TESTDlg.h : ��� ����
//

#pragma once


// CPrintDC_TESTDlg ��ȭ ����
class CPrintDC_TESTDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CPrintDC_TESTDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRINTDC_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// �޼���
	afx_msg void OnBnClickedMfcbuttonPrint();

	// ���ũ��(��������) �ȼ�����
	int int_head_banding_length = 600;
	


};

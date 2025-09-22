
// PrintDC_TESTDlg.cpp : ���� ����
//
#include "stdafx.h"
#include "PrintDC_TEST.h"
#include "PrintDC_TESTDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPrintDC_TESTDlg ��ȭ ����



CPrintDC_TESTDlg::CPrintDC_TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PRINTDC_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrintDC_TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPrintDC_TESTDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_MFCBUTTON_PRINT, &CPrintDC_TESTDlg::OnBnClickedMfcbuttonPrint)
END_MESSAGE_MAP()


// CPrintDC_TESTDlg �޽��� ó����

BOOL CPrintDC_TESTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CPrintDC_TESTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CPrintDC_TESTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CPrintDC_TESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPrintDC_TESTDlg::OnBnClickedMfcbuttonPrint()
{
	CPrintDialog dlgPrint(FALSE);		// ������ ���̾�α� 
	if (IDOK == dlgPrint.DoModal()) 
	{ 
		// todo .. : ���� ������ ���� ��� 
		DOCINFO docinfo; 
		memset(&docinfo, 0, sizeof(DOCINFO)); 
		docinfo.cbSize = sizeof(DOCINFO); 
		docinfo.lpszDocName = _T("Print Test"); // ���� ��Ī 
		docinfo.lpszOutput = NULL; 
		docinfo.lpszDatatype = NULL; 
		docinfo.fwType = 0; 
		DEVMODE *pDevMode = dlgPrint.GetDevMode(); // ������ ��� ��� 
		pDevMode->dmOrientation = DMORIENT_PORTRAIT; // ����&���� ��� ���� 
		HDC hdcPrint = dlgPrint.CreatePrinterDC(); // ������ DC ��� 
		int iPrintWidth = ::GetDeviceCaps( hdcPrint, HORZRES ); // ���� 
		int iPrintHeight = ::GetDeviceCaps( hdcPrint, VERTRES ); // ����

		HDC hMemDC;
		BITMAP bmp_info;
		HBITMAP hImage, hOldBitmap;
		// ��Ʈ�� �Ӽ����� ������ �ҷ����� HBITMAP���� ����ȯ�ؼ� hImage�� �����Ѵ�
		// fuLoad �Ӽ�: ���ҽ� ��� ���ϸ�����, ȣȯ ��Ʈ���� �ƴ� DIB ���� ��Ʈ������ �ҷ��´�
		hImage = (HBITMAP)LoadImage(NULL, TEXT("titan.bmp"),
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

		if (hImage == NULL)
		{
			AfxMessageBox("�̹��� �ε� ����!", MB_ICONSTOP);
			return;
		}
		//else
		//{
		//	AfxMessageBox("�̹��� �ε� ����!", MB_ICONSTOP);
		//	return;
		//}

		hMemDC = CreateCompatibleDC(hdcPrint); // �޸� DC�� �����

		// hImage�� ���õǱ� ���� �ڵ��� ������ �д�
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hImage);

		int int_res = ::GetObject(hImage, sizeof(BITMAP), (LPVOID)&bmp_info);
		int int_Image_Width = (int)bmp_info.bmWidth;
		int int_Image_Height = (int)bmp_info.bmHeight;
		int int_Current_Image_Line = int_head_banding_length;

		// ������ ������ 
		if ( ::StartDoc( hdcPrint, &docinfo ) ) 
		{ 
			//TRACE( _T( "StartDoc\n" ) ); // ������ ���� 
			// ����� ���� ������ �̹����� �߶� ����Ѵ�.
			// �̹��� ũ�⺸�� ������̰� Ŀ���� ���� �ݺ��� Ż��
			
			while (int_Current_Image_Line <= int_Image_Height)
			{
				if (TRUE == ::StartPage(hdcPrint))
				{
					//TRACE( _T( "StartPage\n" ) ); // todo .. : ������ dc�� �׸���. 
					::PatBlt(hdcPrint, 0, 0, iPrintWidth, iPrintHeight, WHITENESS); // �Ͼ�� ó��
					::BitBlt(hdcPrint, 0, int_Current_Image_Line - int_head_banding_length, int_Image_Width, int_head_banding_length, hMemDC, 0, int_Current_Image_Line - int_head_banding_length, SRCCOPY);
					//SelectObject(hMemDC, hOldBitmap); // hImage ������ �����ϱ� ���� hOldBitmap�� �����Ѵ�

					// ������ ���� 
					::EndPage(hdcPrint);
				}
				else
				{
					//TRACE( _T( "error StartPage\n" ) ); 
				}

				// ���� �ջ�
				int_Current_Image_Line += int_head_banding_length;
			}
			

			
			// �ݺ��� Ż�� �� �����ִ� �ܿ� �̹��� ���� ������ش�.
			if (TRUE == ::StartPage(hdcPrint))
			{
				//TRACE( _T( "StartPage\n" ) ); // todo .. : ������ dc�� �׸���. 
				::PatBlt(hdcPrint, 0, 0, iPrintWidth, iPrintHeight, WHITENESS); // �Ͼ�� ó��
				::BitBlt(hdcPrint, 0, int_Current_Image_Line - int_head_banding_length, int_Image_Width, int_Image_Height - (int_Current_Image_Line - int_head_banding_length), hMemDC, 0, int_Current_Image_Line - int_head_banding_length, SRCCOPY);
				//SelectObject(hMemDC, hOldBitmap); // hImage ������ �����ϱ� ���� hOldBitmap�� �����Ѵ�

				// ������ ���� 
				::EndPage(hdcPrint);
			}
			else
			{
				//TRACE( _T( "error StartPage\n" ) ); 
			}
			

			// ������ ������ 
			::EndDoc(hdcPrint);

		} 
		else 
		{ 
			//TRACE( _T( "error StartDoc\n" ) ); 
		}


		DeleteObject(hImage); // ���� ������ ��Ʈ���� �����Ѵ�
		DeleteDC(hMemDC); // �޸� DC�� �����Ѵ�
	}
}

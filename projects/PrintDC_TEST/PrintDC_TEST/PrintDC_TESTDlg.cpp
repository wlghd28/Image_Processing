
// PrintDC_TESTDlg.cpp : 구현 파일
//
#include "stdafx.h"
#include "PrintDC_TEST.h"
#include "PrintDC_TESTDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CPrintDC_TESTDlg 대화 상자



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


// CPrintDC_TESTDlg 메시지 처리기

BOOL CPrintDC_TESTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPrintDC_TESTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPrintDC_TESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPrintDC_TESTDlg::OnBnClickedMfcbuttonPrint()
{
	CPrintDialog dlgPrint(FALSE);		// 프린터 다이얼로그 
	if (IDOK == dlgPrint.DoModal()) 
	{ 
		// todo .. : 선택 프린터 정보 얻기 
		DOCINFO docinfo; 
		memset(&docinfo, 0, sizeof(DOCINFO)); 
		docinfo.cbSize = sizeof(DOCINFO); 
		docinfo.lpszDocName = _T("Print Test"); // 문서 명칭 
		docinfo.lpszOutput = NULL; 
		docinfo.lpszDatatype = NULL; 
		docinfo.fwType = 0; 
		DEVMODE *pDevMode = dlgPrint.GetDevMode(); // 프린터 모드 얻기 
		pDevMode->dmOrientation = DMORIENT_PORTRAIT; // 가로&세로 모드 설정 
		HDC hdcPrint = dlgPrint.CreatePrinterDC(); // 프린터 DC 얻기 
		int iPrintWidth = ::GetDeviceCaps( hdcPrint, HORZRES ); // 가로 
		int iPrintHeight = ::GetDeviceCaps( hdcPrint, VERTRES ); // 세로

		HDC hMemDC;
		BITMAP bmp_info;
		HBITMAP hImage, hOldBitmap;
		// 비트맵 속성으로 파일을 불러오고 HBITMAP으로 형변환해서 hImage에 저장한다
		// fuLoad 속성: 리소스 대신 파일명으로, 호환 비트맵이 아닌 DIB 섹션 비트맵으로 불러온다
		hImage = (HBITMAP)LoadImage(NULL, TEXT("titan.bmp"),
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

		if (hImage == NULL)
		{
			AfxMessageBox("이미지 로드 실패!", MB_ICONSTOP);
			return;
		}
		//else
		//{
		//	AfxMessageBox("이미지 로드 성공!", MB_ICONSTOP);
		//	return;
		//}

		hMemDC = CreateCompatibleDC(hdcPrint); // 메모리 DC를 만든다

		// hImage가 선택되기 전의 핸들을 저장해 둔다
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hImage);

		int int_res = ::GetObject(hImage, sizeof(BITMAP), (LPVOID)&bmp_info);
		int int_Image_Width = (int)bmp_info.bmWidth;
		int int_Image_Height = (int)bmp_info.bmHeight;
		int int_Current_Image_Line = int_head_banding_length;

		// 문서를 시작함 
		if ( ::StartDoc( hdcPrint, &docinfo ) ) 
		{ 
			//TRACE( _T( "StartDoc\n" ) ); // 페이지 시작 
			// 헤드밴딩 길이 단위로 이미지를 잘라서 출력한다.
			// 이미지 크기보다 밴딩길이가 커지는 순간 반복문 탈출
			
			while (int_Current_Image_Line <= int_Image_Height)
			{
				if (TRUE == ::StartPage(hdcPrint))
				{
					//TRACE( _T( "StartPage\n" ) ); // todo .. : 프린터 dc에 그린다. 
					::PatBlt(hdcPrint, 0, 0, iPrintWidth, iPrintHeight, WHITENESS); // 하얗게 처리
					::BitBlt(hdcPrint, 0, int_Current_Image_Line - int_head_banding_length, int_Image_Width, int_head_banding_length, hMemDC, 0, int_Current_Image_Line - int_head_banding_length, SRCCOPY);
					//SelectObject(hMemDC, hOldBitmap); // hImage 선택을 해제하기 위해 hOldBitmap을 선택한다

					// 페이지 종료 
					::EndPage(hdcPrint);
				}
				else
				{
					//TRACE( _T( "error StartPage\n" ) ); 
				}

				// 길이 합산
				int_Current_Image_Line += int_head_banding_length;
			}
			

			
			// 반복문 탈출 후 남아있는 잔여 이미지 마저 출력해준다.
			if (TRUE == ::StartPage(hdcPrint))
			{
				//TRACE( _T( "StartPage\n" ) ); // todo .. : 프린터 dc에 그린다. 
				::PatBlt(hdcPrint, 0, 0, iPrintWidth, iPrintHeight, WHITENESS); // 하얗게 처리
				::BitBlt(hdcPrint, 0, int_Current_Image_Line - int_head_banding_length, int_Image_Width, int_Image_Height - (int_Current_Image_Line - int_head_banding_length), hMemDC, 0, int_Current_Image_Line - int_head_banding_length, SRCCOPY);
				//SelectObject(hMemDC, hOldBitmap); // hImage 선택을 해제하기 위해 hOldBitmap을 선택한다

				// 페이지 종료 
				::EndPage(hdcPrint);
			}
			else
			{
				//TRACE( _T( "error StartPage\n" ) ); 
			}
			

			// 문서를 종료함 
			::EndDoc(hdcPrint);

		} 
		else 
		{ 
			//TRACE( _T( "error StartDoc\n" ) ); 
		}


		DeleteObject(hImage); // 선택 해제된 비트맵을 제거한다
		DeleteDC(hMemDC); // 메모리 DC를 제거한다
	}
}


// PrintDC_TEST.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CPrintDC_TESTApp:
// �� Ŭ������ ������ ���ؼ��� PrintDC_TEST.cpp�� �����Ͻʽÿ�.
//

class CPrintDC_TESTApp : public CWinApp
{
public:
	CPrintDC_TESTApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CPrintDC_TESTApp theApp;
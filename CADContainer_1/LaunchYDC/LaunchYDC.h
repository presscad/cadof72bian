// LaunchYDC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CLaunchYDCApp:
// �йش����ʵ�֣������ LaunchYDC.cpp
//

class CLaunchYDCApp : public CWinApp
{
public:
	CLaunchYDCApp();

// ��д
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLaunchYDCApp theApp;

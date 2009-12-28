// LaunchYDC.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CLaunchYDCApp:
// 有关此类的实现，请参阅 LaunchYDC.cpp
//

class CLaunchYDCApp : public CWinApp
{
public:
	CLaunchYDCApp();

// 重写
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CLaunchYDCApp theApp;

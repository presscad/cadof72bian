// LaunchCAD.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CLaunchCADApp:
// �йش����ʵ�֣������ LaunchCAD.cpp
//

class CLaunchCADApp : public CWinApp
{
public:
	CLaunchCADApp();

// ��д
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLaunchCADApp theApp;

// CmdPowerConfig.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CCmdPowerConfigApp:
// �йش����ʵ�֣������ CmdPowerConfig.cpp
//

class CCmdPowerConfigApp : public CWinApp
{
public:
	CCmdPowerConfigApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCmdPowerConfigApp theApp;

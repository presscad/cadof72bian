// CmdPowerConfig.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "CmdPowerConfig.h"
#include "CmdPowerConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCmdPowerConfigApp

BEGIN_MESSAGE_MAP(CCmdPowerConfigApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCmdPowerConfigApp ����

CCmdPowerConfigApp::CCmdPowerConfigApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CCmdPowerConfigApp ����

CCmdPowerConfigApp theApp;


// CCmdPowerConfigApp ��ʼ��

BOOL CCmdPowerConfigApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();


	CCmdPowerConfigDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO���ڴ˷��ô����ʱ�á�ȷ�������ر�
		//�Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO���ڴ˷��ô����ʱ�á�ȡ�������ر�
		//�Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

// LaunchCAD.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "LaunchCAD.h"
#include "LaunchCADDlg.h"
#include <tlhelp32.h>
#include "key.h"
#include <iostream>
#include <fstream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


long WINAPI SHLaunchApp(HANDLE &hProcess,char * AppName, char * ComLine, char * CurDir,long waitComplete);

BOOL WINAPI SHLaunchCAD();

long WINAPI SHGetAcadR16Path(const char * productName,char *path,long size);

long WINAPI SHGetAutodeskSupport(char *SupportPath);

BOOL CALLBACK EnumWindowsCAD(HWND hWnd,LPARAM lParam)
{
	CString str;
	DWORD dwProcessId;
	GetWindowThreadProcessId(hWnd,&dwProcessId);
	if(dwProcessId  !=  (DWORD)lParam)
	{
		return TRUE;//�ô��ڵ��߳�ID����CAD�����߳�ID
	}

	if(GetWindow(hWnd,GW_CHILD) == NULL)
	{
		return TRUE;//û���Ӵ��ڿ϶�����ϵͳ��������
	}

	GetWindowText(hWnd,str.GetBuffer(MAX_PATH+1),MAX_PATH);
	str.ReleaseBuffer();
	CString sTitle = "CadContainer";
	if(str.Find(sTitle) != -1)
	{
		if(IsIconic(hWnd))
		{
			ShowWindow(hWnd,SW_SHOWMAXIMIZED);
		}
		SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,
			SWP_NOMOVE|SWP_NOSIZE);
		SetWindowPos(hWnd,HWND_NOTOPMOST,0,0,0,0,
			SWP_NOMOVE|SWP_NOSIZE);
		return FALSE;//�ҵ���CAD�����ڣ������ټ���������
	}

	return TRUE;
}

void BringCADToTop()
{
	HANDLE hSnapshot=INVALID_HANDLE_VALUE;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return;
	}

	PROCESSENTRY32 ppe={ sizeof(ppe) };
	CString str;
	int nPos;
	BOOL bOk = Process32First(hSnapshot, &ppe);

	// Remove the "[System Process]" (PID = 0)
	while(bOk && (ppe.th32ProcessID == 0))
	{
		bOk = Process32Next(hSnapshot,&ppe);
	}

	while(bOk)
	{
		// Place the process name (without its path) & ID in the list
		if(ppe.szExeFile != NULL)
		{
			str=ppe.szExeFile;
			nPos=str.ReverseFind('\\');
			if(nPos != -1)
			{
				str=str.Mid(nPos+1);
			}

			if(str.CompareNoCase("acad.exe") == 0)
			{
				EnumWindows((WNDENUMPROC) EnumWindowsCAD, ppe.th32ProcessID);
			}
		}

		bOk = Process32Next(hSnapshot,&ppe);
		// Remove the "[System Process]" (PID = 0)
		while(bOk && (ppe.th32ProcessID == 0))
		{
			bOk = Process32Next(hSnapshot,&ppe); 
		}
	}
	CloseHandle(hSnapshot);
}

// CLaunchCADApp

BEGIN_MESSAGE_MAP(CLaunchCADApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLaunchCADApp ����

CLaunchCADApp::CLaunchCADApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CLaunchCADApp ����

CLaunchCADApp theApp;

HANDLE g_hEvent;
//CLaunchCADDlg dlg;
DWORD g_dwTickCount = 0;


VOID CALLBACK timer(HWND wnd, UINT a, UINT b, DWORD c)
{	
	//����������ɹ�,��ʱ(��Ϊ1����)�˻�û�������ɹ�,��������.
	if( (WaitForSingleObject(g_hEvent,0) == WAIT_OBJECT_0) ||
		(GetTickCount() - g_dwTickCount > 60000) )
	{
		KillTimer(NULL,b);
		//dlg.DestroyWindow();
		CloseHandle(g_hEvent);
	}		
}

// CLaunchCADApp ��ʼ��

BOOL CLaunchCADApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CoInitialize(NULL); // COM����

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ֧�ִ򿪶��
	//HANDLE h = CreateEvent ( NULL, TRUE, FALSE, "SH-CADSP-3AC01994-FD16-4F4F-9BA7-A5F27AE8CC76");
	//if (GetLastError() == ERROR_ALREADY_EXISTS)
	//{
	//	BringCADToTop();	//ϵͳ�Ѿ������У��������������л���ǰ̨��
	//	CloseHandle(h);
	//	return FALSE;
	//}

	//CloseHandle(h);

	//��ʾLOGO������AutodeskMap��

	//m_pMainWnd = &dlg;

	g_hEvent = CreateEvent(NULL,TRUE,FALSE,"hust_{62B6D001-7766-514f-D8BB-58549A777A7A}");	

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(g_hEvent);
		return FALSE;
	}

	if (!SHLaunchCAD())
	{
		return FALSE;
	}

	g_dwTickCount = GetTickCount();

	SetTimer(NULL,7766,300,(TIMERPROC)timer);

	//INT_PTR nResponse = dlg.DoModal();

	//CLaunchCADDlg dlg;
	//m_pMainWnd = &dlg;
	//INT_PTR nResponse = dlg.DoModal();
	//if (nResponse == IDOK)
	//{
	//	// TODO���ڴ˷��ô����ʱ�á�ȷ�������ر�
	//	//�Ի���Ĵ���
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO���ڴ˷��ô����ʱ�á�ȡ�������ر�
	//	//�Ի���Ĵ���
	//}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int CLaunchCADApp::ExitInstance()
{
	CoUninitialize(); // ���COM����

	return 0;
}

CString GetCurPath()
{
	CString sCurPath;

	GetModuleFileName(AfxGetResourceHandle(),sCurPath.GetBuffer(MAX_PATH),MAX_PATH);
	sCurPath.ReleaseBuffer();

	int n = sCurPath.ReverseFind('\\');
	if(n!=-1)
	{
		sCurPath = sCurPath.Left(n);
	}

	return sCurPath;
}

//�õ����ǵ��ӱ��������װ·��
long GetJzqPath(char *path,long size)
{
	long lRetVal = 0;

	if(path==NULL)
	{
		return FALSE;
	}

	char *s = new char[size+1];

	CString StrKey = "Software\\ZZSoft\\dzsp";

	CString StrVer,JzqLocation;

	CKey Key;

	if (!Key.Open(HKEY_LOCAL_MACHINE, StrKey))
	{
		return FALSE;
	}
	if(Key.GetStringValue(JzqLocation, "Path"))
	{
		lRetVal=1;
	}

	strcpy(s,JzqLocation);
	Key.Close();

	strcpy(path,s);

	delete []s;

	return lRetVal;
}

bool FileExists(LPCTSTR lpszFileName, bool bIsDirCheck)
{
	DWORD dwAttributes = GetFileAttributes(lpszFileName);
	if(dwAttributes == 0xFFFFFFFF)
	{
		return false;
	}

	if((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		return bIsDirCheck;
	}
	else
	{
		return !bIsDirCheck;
	}
}
BOOL WINAPI SHLaunchCAD()
{
	BOOL bRetVal = TRUE;

	CString strTmp;
	char AcadCADR16Path[_MAX_PATH];

	CString sSortPath;

	sSortPath = GetCurPath();

	bRetVal=SHGetAcadR16Path("AutoCAD 200",AcadCADR16Path,_MAX_PATH);
	if(bRetVal==FALSE)
	{
		bRetVal=SHGetAcadR16Path("Autodesk Map",AcadCADR16Path,_MAX_PATH);
		if(bRetVal==FALSE)
		{
			MessageBox(NULL,"�����ҵ�AutoCAD�İ�װ·����","����ϵͳ",MB_OK|MB_ICONSTOP);
			return FALSE;
		}
	}

	//�ж�����ֵ���һ���Ƿ�Ϊб��,�����Ǽ���б��
	int len = strlen(AcadCADR16Path);
	if(AcadCADR16Path[len - 1] != '\\' && AcadCADR16Path[len - 1] != '/')
		strcat(AcadCADR16Path,"\\");

	strTmp = AcadCADR16Path;
	strTmp += "acad.exe";
	CFileFind cf;
	if (cf.FindFile(strTmp) == 0)
	{
		MessageBox(NULL,"�����ҵ�AutoCAD��������acad.exe!","����ϵͳ",MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	char workPath[_MAX_PATH];
	strcpy(workPath,AcadCADR16Path);

	HANDLE h;
	CString app,cmd;
	app = AcadCADR16Path;
	app += "acad.exe";

	char JzqPath[_MAX_PATH];
	bRetVal=GetJzqPath(JzqPath,_MAX_PATH);
	if(bRetVal==TRUE)
	{
		//�ж�����ֵ���һ���Ƿ�Ϊб��,�����Ǽ���б��
		int len = strlen(JzqPath);
		if(JzqPath[len - 1] != '\\' && JzqPath[len - 1] != '/')
			strcat(JzqPath,"\\");

		//fstream file;
		//file.open(JzqPath,ios::in);
		//if(!file)
		if (FileExists(JzqPath, true))
		{
			cmd = " /s \"";
			//ȥ��б��
			CString strJzqPath = JzqPath;
			cmd += strJzqPath.Left(strJzqPath.GetLength()-1);
			cmd += "\";";

			char JzqArxPath[_MAX_PATH];
			strcpy(JzqArxPath, JzqPath);
			strcat(JzqArxPath, "ArxR16\\");
			//fstream file1;
			//file1.open(JzqArxPath,ios::in);
			//if(!file1)
			if (FileExists(JzqArxPath,true))
			{
				cmd += "\"";
				cmd += JzqPath;
				cmd += "ArxR16\";";
			}

			char JzqFontPath[_MAX_PATH];
			strcpy(JzqFontPath, JzqPath);
			strcat(JzqFontPath, "Fonts\\");
			//fstream file2;
			//file2.open(JzqFontPath,ios::in);
			//if(!file2)
			if (FileExists(JzqFontPath, true))
			{
				cmd += "\"";
				cmd += JzqPath;
				cmd += "Fonts\";";
			}
		}

		cmd += "\"";
		cmd += sSortPath;
		cmd += "\";";
	}
	else
	{
		cmd = " /s \"";
		cmd += sSortPath;
		cmd += "\";";
	}

	char SupportPath[_MAX_PATH];
	if (SHGetAutodeskSupport(SupportPath))
	{
		cmd += "\"";
		cmd += SupportPath;
		cmd += "\";";
	}

	cmd += "\"";
	cmd += AcadCADR16Path;
	cmd += "support\";";

	cmd += "\"";
	cmd += AcadCADR16Path;
	cmd += "fonts\";";

	cmd += "\"";
	cmd += AcadCADR16Path;
	cmd += "help\";";

	cmd += "\"";
	cmd += AcadCADR16Path;
	cmd += "express\";";

	cmd += "\"";
	cmd += AcadCADR16Path;
	cmd += "support\\color\";";

	if (SHLaunchApp(h,(LPSTR)(LPCSTR)app,(LPSTR)(LPCSTR)cmd,workPath,0) == -1)
	{
		AfxMessageBox("��������ǰ�˼�����ϵͳ��",MB_ICONERROR);
		bRetVal = -1;
	}

	return bRetVal;
}
long WINAPI SHLaunchApp(HANDLE &hProcess,char * AppName, char * ComLine, char * CurDir,long waitComplete)
{
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;

	memset(&StartupInfo,0,sizeof(STARTUPINFO));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.wShowWindow = SW_SHOW;

	if(CreateProcess(AppName,ComLine
		,NULL,NULL,FALSE,
		CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE|CREATE_NEW_PROCESS_GROUP,
		NULL,
		CurDir,
		&StartupInfo,
		&ProcessInfo)==0)
	{
		LPVOID lpMsgBuf;

		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,NULL,
			GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,0,NULL);

		::MessageBox( NULL, (char *)lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );
		LocalFree( lpMsgBuf );

		CString str;

		str = "AppName:";
		str += AppName;
		str +="   CommandLine:";
		str += ComLine;
		str += "  CurrentDirectory:";
		str += CurDir;
		AfxMessageBox(str);


		return -1;
	}

	hProcess = ProcessInfo.hProcess;

	if(waitComplete == 1)
		while((WaitForInputIdle(hProcess,INFINITE)==0));//INFINITE

	return 1;
}

void GetKeyListArray(CString StrKey,CStringArray &KeyArr)//ö�ٱ�ֵ
{
	KeyArr.RemoveAll(); 
	bool endr=true;
	for(int i=0;endr;i++)
	{
		CKey Key;
		Key.Open(HKEY_LOCAL_MACHINE, StrKey);
		CString str;
		if(!Key.GetRegEnumKey(str,i))
			endr=false;
		else
			KeyArr.Add(str); 

		Key.Close();
	}
}

long WINAPI SHGetAcadR16Path(const char * productName,char *path,long size)
{
	long lRetVal = 0;

	int nLen;
	if((productName==NULL)||(path==NULL))
	{
		return FALSE;
	}
	nLen=strlen(productName);

	char *s = new char[size+1];

	//CAD 2005
	CString StrKey = "Software\\Autodesk\\AutoCAD\\R16.0";
	CStringArray KeyArr;

	GetKeyListArray(StrKey,KeyArr);

	if (KeyArr.GetCount() == 0)
	{
		//CAD 2004
		StrKey = "Software\\Autodesk\\AutoCAD\\R16.0";
		GetKeyListArray(StrKey,KeyArr);

		if (KeyArr.GetCount() == 0)
		{
			//CAD 2006
			StrKey = "Software\\Autodesk\\AutoCAD\\R16.2";
			GetKeyListArray(StrKey,KeyArr);
		}
	}

	CString StrVer,ProductName,AcadLocation;

	CKey Key;

	for(int i=0;i<KeyArr.GetSize();i++)
	{
		StrVer=KeyArr.GetAt(i); 

		Key.Open(HKEY_LOCAL_MACHINE, StrKey + "\\" + StrVer);
		Key.GetStringValue(ProductName, "ProductName");

		if(ProductName.Left(nLen).CompareNoCase(productName)==0)
		{
			if(Key.GetStringValue(AcadLocation, "AcadLocation"))
			{
				lRetVal=1;
			}

			strcpy(s,AcadLocation);
			Key.Close();
			break;
		}
		Key.Close();
	}

	strcpy(path,s);

	delete []s;

	return lRetVal;
}

void GetKeyNameArray(CString StrKey,CStringArray &KeyArr)//ö�ٱ�ֵ
{
	KeyArr.RemoveAll(); 
	bool endr=true;
	for(int i=0;endr;i++)
	{
		CKey Key;
		Key.Open(HKEY_LOCAL_MACHINE, StrKey);
		CString str;
		if(!Key.GetRegEnumValue(str,i))
			endr=false;
		else
			KeyArr.Add(str); 

		Key.Close();
	}
}

long WINAPI SHGetAutodeskSupport(char *SupportPath)
{
	long lRetVal = 0;

	CString StrKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\Folders";

	CStringArray KeyArr;

	GetKeyNameArray(StrKey, KeyArr);

	CString strKeyName;

	for (int i = 0; i < KeyArr.GetCount(); i++)
	{
		strKeyName = KeyArr.GetAt(i);

		if (strKeyName.Find("Autodesk") != -1 && 
			strKeyName.Find("R16.1") != -1 && 
			strKeyName.Find("Support") != -1)
		{
			if (strKeyName.GetLength() == strKeyName.Find("Support") + 8)
			{
				strKeyName = strKeyName.Left(strKeyName.GetLength() - 1);
				lRetVal = 1;
				break;
			}
		}
	}

	strcpy(SupportPath, strKeyName);

	return lRetVal;
}
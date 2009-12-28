// LaunchCAD.cpp : 定义应用程序的类行为。
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
		return TRUE;//该窗口的线程ID不是CAD的主线程ID
	}

	if(GetWindow(hWnd,GW_CHILD) == NULL)
	{
		return TRUE;//没有子窗口肯定不是系统的主窗口
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
		return FALSE;//找到了CAD主窗口，不需再继续查找了
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


// CLaunchCADApp 构造

CLaunchCADApp::CLaunchCADApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CLaunchCADApp 对象

CLaunchCADApp theApp;

HANDLE g_hEvent;
//CLaunchCADDlg dlg;
DWORD g_dwTickCount = 0;


VOID CALLBACK timer(HWND wnd, UINT a, UINT b, DWORD c)
{	
	//如果已启动成功,或超时(现为1分钟)了还没有启动成功,就清理环境.
	if( (WaitForSingleObject(g_hEvent,0) == WAIT_OBJECT_0) ||
		(GetTickCount() - g_dwTickCount > 60000) )
	{
		KillTimer(NULL,b);
		//dlg.DestroyWindow();
		CloseHandle(g_hEvent);
	}		
}

// CLaunchCADApp 初始化

BOOL CLaunchCADApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CoInitialize(NULL); // COM声明

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 支持打开多个
	//HANDLE h = CreateEvent ( NULL, TRUE, FALSE, "SH-CADSP-3AC01994-FD16-4F4F-9BA7-A5F27AE8CC76");
	//if (GetLastError() == ERROR_ALREADY_EXISTS)
	//{
	//	BringCADToTop();	//系统已经在运行，把它的主窗口切换到前台。
	//	CloseHandle(h);
	//	return FALSE;
	//}

	//CloseHandle(h);

	//显示LOGO，启动AutodeskMap。

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
	//	// TODO：在此放置处理何时用“确定”来关闭
	//	//对话框的代码
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO：在此放置处理何时用“取消”来关闭
	//	//对话框的代码
	//}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

int CLaunchCADApp::ExitInstance()
{
	CoUninitialize(); // 清除COM声明

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

//得到众智电子报建软件安装路径
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
			MessageBox(NULL,"不能找到AutoCAD的安装路径！","数慧系统",MB_OK|MB_ICONSTOP);
			return FALSE;
		}
	}

	//判断所得值最后一个是否为斜杠,若不是加上斜杠
	int len = strlen(AcadCADR16Path);
	if(AcadCADR16Path[len - 1] != '\\' && AcadCADR16Path[len - 1] != '/')
		strcat(AcadCADR16Path,"\\");

	strTmp = AcadCADR16Path;
	strTmp += "acad.exe";
	CFileFind cf;
	if (cf.FindFile(strTmp) == 0)
	{
		MessageBox(NULL,"不能找到AutoCAD的主程序acad.exe!","数慧系统",MB_OK|MB_ICONSTOP);
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
		//判断所得值最后一个是否为斜杠,若不是加上斜杠
		int len = strlen(JzqPath);
		if(JzqPath[len - 1] != '\\' && JzqPath[len - 1] != '/')
			strcat(JzqPath,"\\");

		//fstream file;
		//file.open(JzqPath,ios::in);
		//if(!file)
		if (FileExists(JzqPath, true))
		{
			cmd = " /s \"";
			//去掉斜杠
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
		AfxMessageBox("不能启动前端检测入库系统！",MB_ICONERROR);
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

void GetKeyListArray(CString StrKey,CStringArray &KeyArr)//枚举表值
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

void GetKeyNameArray(CString StrKey,CStringArray &KeyArr)//枚举表值
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
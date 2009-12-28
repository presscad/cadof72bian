#include "stdAfx.h"
#include "DistSysFun.h"

//获取当前执行程序路径
CString gGetCurAppPath()
{
	TCHAR exeFullPath[MAX_PATH]; 
	CString strPath; 
	GetModuleFileName(NULL,exeFullPath,MAX_PATH); 
	strPath=(CString)exeFullPath; 
	int position=strPath.ReverseFind('\\'); 
	strPath=strPath.Left(position+1); 
	return strPath;
}

CString gGetCurArxAppPath(CString strArxName)
{
	TCHAR exeFullPath[MAX_PATH]; 
	CString strPath; 
	GetModuleFileName(GetModuleHandle(strArxName),exeFullPath,MAX_PATH); 
	strPath=(CString)exeFullPath; 
	int position=strPath.ReverseFind('\\'); 
	strPath=strPath.Left(position+1); 
	return strPath;
}
/*
CString gGetCurArxAppPath()
{
	CString appFileName = acedGetAppName();
	char dir[_MAX_DIR], drive[_MAX_DRIVE], path[_MAX_PATH];
	_splitpath(appFileName, drive, dir, NULL, NULL);
	_makepath(path, drive, dir, NULL, NULL);
	return path;
}
*/

BOOL gIsFileExists(const CString strFName)
{
	CFileStatus fileStat;
	return CFile::GetStatus(strFName, fileStat) != 0;
}



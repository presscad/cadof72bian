#ifndef __SHFOLDER__H__
#define __SHFOLDER__H__

#ifndef	   BIF_NEWDIALOGSTYLE
#define	   BIF_NEWDIALOGSTYLE		0x0040
#endif

#ifndef	   BIF_USENEWUI
#define	   BIF_USENEWUI			0x0050
#endif

//	��ʼ���ļ����趨�õĻص����� 
int CALLBACK _SHBrowseForFolderCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if(uMsg == BFFM_INITIALIZED)
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	
	return	0;
}


bool g_fSelectFolderDlg(CString* lpstrFolder,CString strIniFolder,bool bAvailNewFolder)
{
	bool			ret;
	char			lpszPath[MAX_PATH];
	LPMALLOC		lpMalloc;
	BROWSEINFO		sInfo;
	LPITEMIDLIST	lpidlRoot;
	LPITEMIDLIST	lpidlBrowse;
	
	if(lpstrFolder == NULL)
		return	false;
	
	if(::SHGetMalloc(&lpMalloc) != NOERROR)
		return	false;
	
	ret = false;
	
	if(strIniFolder != "")
	{
		if(strIniFolder.Right(1) == "\\")
			strIniFolder = strIniFolder.Left(strIniFolder.GetLength() - 1);			//ɾ��ĩβ��"\\"
		
	}
	
	::SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &lpidlRoot);	//ȡ��ѡ�����ļ�����
	
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot		= lpidlRoot;
	sInfo.pszDisplayName = lpszPath;
	sInfo.lpszTitle		= _T("��Ŀ����·��");
	sInfo.ulFlags		= BIF_RETURNONLYFSDIRS;
	if(bAvailNewFolder == true)
		sInfo.ulFlags	|= BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_USENEWUI;
	sInfo.lpfn			= _SHBrowseForFolderCallbackProc;
	sInfo.lParam		= (LPARAM)strIniFolder.GetBuffer(0);
	
	lpidlBrowse = ::SHBrowseForFolder(&sInfo);			//��ʾ�ļ���ѡ��Ի���
	
	if(lpidlBrowse != NULL)
	{
		if(::SHGetPathFromIDList(lpidlBrowse,lpszPath))	//ȡ���ļ�����		
		{
			*lpstrFolder = "";
			*lpstrFolder = lpszPath;
			
			if(*lpstrFolder != "")
			{
				if(lpstrFolder->Right(1) != "\\")
					*lpstrFolder += "\\";				//��ĩβʱ����"\\"
			}
		}
		
		ret = true;
	}
	
	if(lpidlBrowse != NULL)
		::CoTaskMemFree(lpidlBrowse);
	if(lpidlRoot != NULL)
		::CoTaskMemFree(lpidlRoot);
	
	lpMalloc->Release();
	
	return	ret;
}

#endif //__SHFOLDER__H__
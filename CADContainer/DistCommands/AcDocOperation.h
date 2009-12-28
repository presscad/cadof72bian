/************************************************************************
*说明：定义和实现一些与CAD中的文档相关的一些函数
*      
*
*
*

************************************************************************/

#pragma once


typedef CArray<AcApDocument* , AcApDocument*>  AcApDocuments;

class XAcDocOp
{
public:
	//枚举当前的已经打开的文档
	static Acad::ErrorStatus enumAcDocument(AcApDocuments &arr);

	//在CAD中新建一个CAD文件，返回一个状态和输出新建文档的指针
	static Acad::ErrorStatus newAcDocument(CString strTemplateName , 
										   AcApDocument *&pDoc , 
										   BOOL bKeepOldActive = FALSE);

	//在CAD中根据一个文件名打开一个文档，返回一个状态和输出打开的文档
	static Acad::ErrorStatus openAcDocument(CString strFilePathName , 
											AcApDocument *&pDoc , 
											BOOL bKeepOldActive = FALSE ,
											BOOL bAutoCloseOnlyReadWarning = FALSE	//是否自动关闭只读提示对话框
											);

private:
	struct _CREATEDOCUMENTINFO
	{
		TCHAR			pszName[_MAX_PATH];		//文档名字或者模块名字
		BOOL			bActiveOldDoc;			//是否激活老文档
		AcApDocument	*pOldDoc;				//老文档指针
	};

	 static void newDocCallBack(void *pData);	//新建文档的回调函数
	 static void openDocCallBack(void *pData);	//打开文档的回调函数

	 //////////////////////////////////////////////////////////////////////////
	 //与自动关闭只读提示对话框相关的提示
	 static	void	AutoCloseOnlyReadWarningDlg();
	 static	DWORD	sm_dwStartAutoClose;		//启用动态关闭的时间，以毫秒为单位
	 static	DWORD	sm_dwTimerId;				//定时器的ID

	 static void CALLBACK AutoCloseTimerProc(
						HWND hwnd,         // handle to window
						UINT uMsg,         // WM_TIMER message
						UINT_PTR idEvent,  // timer identifier
						DWORD dwTime       // current system time
						);


	 static void EnumCurrentThreadWindows();

	 static BOOL CALLBACK EnumThreadWndProc(
						HWND hwnd,      // handle to window
						LPARAM lParam   // application-defined value
						);

	 //线程的窗口集合
	 static CArray<HWND , HWND>	sm_arrThreadWindows;


};

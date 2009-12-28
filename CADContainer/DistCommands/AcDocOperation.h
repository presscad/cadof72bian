/************************************************************************
*˵���������ʵ��һЩ��CAD�е��ĵ���ص�һЩ����
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
	//ö�ٵ�ǰ���Ѿ��򿪵��ĵ�
	static Acad::ErrorStatus enumAcDocument(AcApDocuments &arr);

	//��CAD���½�һ��CAD�ļ�������һ��״̬������½��ĵ���ָ��
	static Acad::ErrorStatus newAcDocument(CString strTemplateName , 
										   AcApDocument *&pDoc , 
										   BOOL bKeepOldActive = FALSE);

	//��CAD�и���һ���ļ�����һ���ĵ�������һ��״̬������򿪵��ĵ�
	static Acad::ErrorStatus openAcDocument(CString strFilePathName , 
											AcApDocument *&pDoc , 
											BOOL bKeepOldActive = FALSE ,
											BOOL bAutoCloseOnlyReadWarning = FALSE	//�Ƿ��Զ��ر�ֻ����ʾ�Ի���
											);

private:
	struct _CREATEDOCUMENTINFO
	{
		TCHAR			pszName[_MAX_PATH];		//�ĵ����ֻ���ģ������
		BOOL			bActiveOldDoc;			//�Ƿ񼤻����ĵ�
		AcApDocument	*pOldDoc;				//���ĵ�ָ��
	};

	 static void newDocCallBack(void *pData);	//�½��ĵ��Ļص�����
	 static void openDocCallBack(void *pData);	//���ĵ��Ļص�����

	 //////////////////////////////////////////////////////////////////////////
	 //���Զ��ر�ֻ����ʾ�Ի�����ص���ʾ
	 static	void	AutoCloseOnlyReadWarningDlg();
	 static	DWORD	sm_dwStartAutoClose;		//���ö�̬�رյ�ʱ�䣬�Ժ���Ϊ��λ
	 static	DWORD	sm_dwTimerId;				//��ʱ����ID

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

	 //�̵߳Ĵ��ڼ���
	 static CArray<HWND , HWND>	sm_arrThreadWindows;


};

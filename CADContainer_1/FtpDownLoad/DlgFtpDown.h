#pragma once
#include "resource.h"

// CDlgFtpDown �Ի���

typedef CArray<int,int> LIST_POINT;
typedef CArray<HTREEITEM,HTREEITEM> TREEITEMARRAY;



class CDlgFtpDown : public CDialog
{
	DECLARE_DYNAMIC(CDlgFtpDown)

public:
	         CDlgFtpDown(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgFtpDown();
	void Init(CString strServer,CString strUserName,CString strPassword,int nPort,CString strProjectId);

public:

	void ConnectFtp();
	void DisplayFtpDir(HTREEITEM hOwnerItem);

	BOOL IsFileExists(const CString strFName);
	CString GetCurArxAppPath();
	CString GetCurAppPath();

	virtual BOOL OnInitDialog();
	void SizeChanged (CRect *lpRect, BOOL bFloating, int flags);

public:
	CTreeCtrl   m_tree;
private:
	CString   m_strServer;
	CString   m_strUserName;
	CString   m_strPassword;
	CString   m_strProjectId;
	int       m_nPort;

	CInternetSession *m_pInetSession;
	CFtpConnection *m_pFtpConnection;
	LIST_POINT      m_PointerArray;

	CImageList      m_img;

private:
	

// �Ի�������
	enum { IDD = IDD_DLG_FILELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
};

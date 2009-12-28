#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"
#include "..\SdeData\DistBaseInterface.h"

struct LAYERTREECFG
{
	int	      nId;			    // ���
	int	      nOwnerId;	        // �������
	CString	  strSdeLyName;	    // SDE������
	CString	  strNodeName;	    // ���ı���(��ʾ����)
	CString	  strPosOrder;	    // ͬ���ӽ�����ڴ���(Ĭ��Ϊ���)
	int       nPower;           // Ȩ�ޣ������ɼ�����ֻ��������д��
	CString   strCadLyName;     // ��Ӧ��CADͼ������
	HTREEITEM hItem;            // �ڵ�ָ��
};
typedef CArray<LAYERTREECFG,LAYERTREECFG&> LAYERTREECFGARRAY;


//-----------------------------------------------------------------------------
class CDlgToolTree : public CAcUiDialog 
{
	DECLARE_DYNAMIC (CDlgToolTree)

public:
	//***************  ���ⲿ���ýӿ�

	//����ͼ����
	int CreateTree(char* strServerName,char* strInstanceName,
		            char* strDBName,char* strUserName,char* strPassword);

	//����Ȩ��
	int SetTreeLyPowerAndReCreate(int* pIdArray,int* pPowerArray,int* pNum);

	//��ȡSDEͼ�����ƺ�Ȩ��ֵ
	int GetTreeLyPower(char*** strSdeLyNameArray,int** pPowerArray,int* pNum);

	//��ȡͼ������ѡ״̬
	CString GetCheckState();

	int SetCheckState(CStringArray* strArrayCheck);

public:
	CDlgToolTree (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;
	enum { IDD = IDD_DLG_TOOLTREE} ;

	virtual BOOL OnInitDialog();
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;
	afx_msg void OnSize (UINT nType, int cx, int cy) ;
	afx_msg void OnDestroy();
	afx_msg BOOL NotifyFunction( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );

	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnCheckAll();
	afx_msg void OnBnClickedBtnNoCheck();
	afx_msg void OnBnClickedBtnSort();

	void OnCloseAllLayer();
protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam) ;

private:
	int ReadTreeInfoFromDB(IDistConnection* pConnect);

	int CreateTreeByData();
	CString GetCurArxAppPath(CString strArxName);
	void ReadAndWriteToFile(CStringArray& strArrayCheck,int nTag=0); //0 ��  1 д
	void SetTreeCheck(HTREEITEM hTI = TVI_ROOT, BOOL bCheck = TRUE);

	void SetChildCheck(HTREEITEM hItem,BOOL bCheck);
	void SetParentCheck(HTREEITEM hItem,BOOL bCheck);

	void SaveTreeCfgToFile();

public:
	CToolBarCtrl       m_wndtoolbarctrl;
	CTreeCtrl          m_tree;
	CImageList         m_img;
	LAYERTREECFGARRAY  m_InfoList;

private:
	char** m_strSdeLyNameArray;
	int*   m_pPowerArray;

	DECLARE_MESSAGE_MAP()

	
public:
	afx_msg void OnNMClickTooltree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTooltree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
} ;

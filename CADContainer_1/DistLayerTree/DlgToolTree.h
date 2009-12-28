#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"
#include "..\SdeData\DistBaseInterface.h"

struct LAYERTREECFG
{
	int	      nId;			    // 编号
	int	      nOwnerId;	        // 父结点编号
	CString	  strSdeLyName;	    // SDE表名称
	CString	  strNodeName;	    // 中文别名(显示名称)
	CString	  strPosOrder;	    // 同级子结点所在次序(默认为编号)
	int       nPower;           // 权限（０不可见　１只读　２可写）
	CString   strCadLyName;     // 对应的CAD图层名称
	HTREEITEM hItem;            // 节点指针
};
typedef CArray<LAYERTREECFG,LAYERTREECFG&> LAYERTREECFGARRAY;


//-----------------------------------------------------------------------------
class CDlgToolTree : public CAcUiDialog 
{
	DECLARE_DYNAMIC (CDlgToolTree)

public:
	//***************  供外部调用接口

	//创建图层树
	int CreateTree(char* strServerName,char* strInstanceName,
		            char* strDBName,char* strUserName,char* strPassword);

	//设置权限
	int SetTreeLyPowerAndReCreate(int* pIdArray,int* pPowerArray,int* pNum);

	//获取SDE图层名称和权限值
	int GetTreeLyPower(char*** strSdeLyNameArray,int** pPowerArray,int* pNum);

	//获取图层树钩选状态
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
	void ReadAndWriteToFile(CStringArray& strArrayCheck,int nTag=0); //0 读  1 写
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

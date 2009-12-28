
#ifndef  __DLG__LAYERTREE__H__
#define  __DLG__LAYERTREE__H__


//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"
class CDlgLayerTree;
class CLayerTreeDef;
class CLayerManager;

typedef struct
{
	CString	Id;			// 编号
	CString	SDELayer;	// SDE表名称
	CString	AliasName;	// 中文别名(显示名称)
	CString	FatherId;	// 父结点编号
	CString NeedInf;	// 必选说明
	CString	OrderNum;	// 同级子结点所在次序(默认为编号)
}LAYERTREECONFIG;//LayerTreeConfig;

typedef CArray<LAYERTREECONFIG,LAYERTREECONFIG&> LAYERTREEINFOARRAY;




class CDlgLayerTree : public CAcUiDialog 
{
	DECLARE_DYNAMIC (CDlgLayerTree)

public:
	// 存储当前被选中的SDE图层名
	CStringArray m_CurSelSdeLayer;

	// 存储前一次被选中打开的SDE图层名
	CStringArray m_OldSelSdeLayer;

	// 存储当前被选中的SDE图层别名列表（树型控件ItemText）
	CStringArray m_SdeAliasNames;

	//工具条对象
	CToolBarCtrl m_wndtoolbarctrl;
	
	//功能树对象
	CTreeCtrl    m_tree;

public:
	CDlgLayerTree (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	//初始化TreeCtrl
	void InitializeTreeControl();
	
	void GetCurSdeLayerName();

	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) ;
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam) ;

	afx_msg LRESULT OnAcadKeepFocus (WPARAM wParam, LPARAM lParam) ;
	afx_msg void OnSize (UINT nType, int cx, int cy) ;
	afx_msg BOOL NotifyFunction( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg void OnBnClickedBtnAutoread();
	afx_msg void OnBnClickedBtnOpenall();
	afx_msg void OnBnClickedBtnCloseall();
	afx_msg void OnBnClickedBtnRefurbish();
	afx_msg void OnNMClickLayerTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickLayerTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedLayerTree(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;

	enum { IDD = IDD_DLG_LAYERTREE} ;
	DECLARE_MESSAGE_MAP()

private:
	HTREEITEM CreateTree(CString sName, HTREEITEM hParent = TVI_ROOT);
	void      SetTreeCheck(HTREEITEM hTI = TVI_ROOT, BOOL bCheck = TRUE);
	void      GetCurSelItemName(HTREEITEM hTI = TVI_ROOT);
	void      SetCurSelItemName(HTREEITEM hTI = TVI_ROOT);
	void      SaveSelLayer();
	void      ReadSelLayer();
	BOOL      CheckTreeItemName(CString sName, HTREEITEM hTI = TVI_ROOT);
} ;



//////////////////////////////////////////////////////////////////////////////////////////////////////
//==================================================================================================//
//==================================================================================================//
//////////////////////////////////////////////////////////////////////////////////////////////////////

class CLayerManager : public CAcUiDockControlBar {
	DECLARE_DYNAMIC (CLayerManager)

public:
	//----- Child dialog which will use the resource id supplied
	CDlgLayerTree *m_pChildDlg ;

public:
	         CLayerManager ();
	virtual ~CLayerManager ();


public:
	virtual BOOL Create (CWnd *pParent, LPCTSTR lpszTitle) ;
	        BOOL ShowDockPane(BOOL bShow = TRUE) ;

	// 存储当前被选中的SDE图层名
	CStringArray m_SdeLayer;

protected:
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) ;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	afx_msg void OnSysCommand (UINT nID, LPARAM lParam) ;
	afx_msg void OnSize (UINT nType, int cx, int cy) ;

	DECLARE_MESSAGE_MAP()
public:
	void GetSdeLayerName();

	void RedrawTreeWindow();
} ;



// 图层树型结构类说明：
// 从业务数据库的TB_LAYERTREEDEF表中读取图层树型结构配置信息
class CLayerTreeDef
{
public:
	         CLayerTreeDef(void);
	virtual ~CLayerTreeDef(void);

private:

    //结构数组(Id,SDELayer,AliasName,FatherId,NeedInf,OrderNum)
	static LAYERTREEINFOARRAY m_LayerTreeArray;  

public:
	
	// 释放数组空间
	static void Release();

	//从表 TB_LAYERTREEDEF 中读取信息，存放到 m_LayerTreeArray 数组中
	static BOOL GetLayerTreeInf(CString strURL);

	// 根据指定的父节点获取其对应的子节点数组
	static BOOL GetChildrenAliasName(CStringArray& ChildrenAliasNames, CStringArray& saNeed, const CString FatherAliasName = "");

	// 根据AliasName 别名，获取 SDE 图层名称
	static BOOL GetSDELayerName(CString& strSdeLyName, const CString strAliasName);

	// 根据 SDE 图层名称，获取 AliasName 别名
	static BOOL GetAliasName(CString& strAliasName, const CString strSdeLyName);

private:
	// 根据 AliasName 别名获取其 ID
	static BOOL GetIdByAliasName(CString& strId, const CString strAliasName = "");
};


#endif //__DLG__LAYERTREE__H__









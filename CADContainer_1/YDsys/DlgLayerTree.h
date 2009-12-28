
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
	CString	Id;			// ���
	CString	SDELayer;	// SDE������
	CString	AliasName;	// ���ı���(��ʾ����)
	CString	FatherId;	// �������
	CString NeedInf;	// ��ѡ˵��
	CString	OrderNum;	// ͬ���ӽ�����ڴ���(Ĭ��Ϊ���)
}LAYERTREECONFIG;//LayerTreeConfig;

typedef CArray<LAYERTREECONFIG,LAYERTREECONFIG&> LAYERTREEINFOARRAY;




class CDlgLayerTree : public CAcUiDialog 
{
	DECLARE_DYNAMIC (CDlgLayerTree)

public:
	// �洢��ǰ��ѡ�е�SDEͼ����
	CStringArray m_CurSelSdeLayer;

	// �洢ǰһ�α�ѡ�д򿪵�SDEͼ����
	CStringArray m_OldSelSdeLayer;

	// �洢��ǰ��ѡ�е�SDEͼ������б����Ϳؼ�ItemText��
	CStringArray m_SdeAliasNames;

	//����������
	CToolBarCtrl m_wndtoolbarctrl;
	
	//����������
	CTreeCtrl    m_tree;

public:
	CDlgLayerTree (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	//��ʼ��TreeCtrl
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

	// �洢��ǰ��ѡ�е�SDEͼ����
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



// ͼ�����ͽṹ��˵����
// ��ҵ�����ݿ��TB_LAYERTREEDEF���ж�ȡͼ�����ͽṹ������Ϣ
class CLayerTreeDef
{
public:
	         CLayerTreeDef(void);
	virtual ~CLayerTreeDef(void);

private:

    //�ṹ����(Id,SDELayer,AliasName,FatherId,NeedInf,OrderNum)
	static LAYERTREEINFOARRAY m_LayerTreeArray;  

public:
	
	// �ͷ�����ռ�
	static void Release();

	//�ӱ� TB_LAYERTREEDEF �ж�ȡ��Ϣ����ŵ� m_LayerTreeArray ������
	static BOOL GetLayerTreeInf(CString strURL);

	// ����ָ���ĸ��ڵ��ȡ���Ӧ���ӽڵ�����
	static BOOL GetChildrenAliasName(CStringArray& ChildrenAliasNames, CStringArray& saNeed, const CString FatherAliasName = "");

	// ����AliasName ��������ȡ SDE ͼ������
	static BOOL GetSDELayerName(CString& strSdeLyName, const CString strAliasName);

	// ���� SDE ͼ�����ƣ���ȡ AliasName ����
	static BOOL GetAliasName(CString& strAliasName, const CString strSdeLyName);

private:
	// ���� AliasName ������ȡ�� ID
	static BOOL GetIdByAliasName(CString& strId, const CString strAliasName = "");
};


#endif //__DLG__LAYERTREE__H__









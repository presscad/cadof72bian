#pragma once
#include "DistBasePara.h"
#include "acui.h"
#include "resource.h"
#include "..\SdeData\DistBaseInterface.h"


//定义CAD实体属性
struct DIST_CADENTITY_STRUCT
{
	CString strDKCode;      //地块编号    DKBH
	CString strYDType;      //用地类型    YDLX
	CString strArea;        //用地面积    YDMJ
	CString strTxtHeight;   //字高        ZG
	CString strLnWidth;     //线宽        XK
	CString strX;           //X 值        X
	CString strY;           //Y 值        Y
	CString strBZ;          //备注        BZ
	CString strObjId;       //实体ID
};
typedef CArray<DIST_CADENTITY_STRUCT,DIST_CADENTITY_STRUCT&> DIST_CADENTITY_ARRAY;





// CDlgLXInput 对话框

class CDlgLXInput : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgLXInput)

public:
	         CDlgLXInput(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLXInput();

	//设置初始信息
	void SetInfo(IDistConnection * pConnect,CString strCadLyNameDK,CString strCadLyNameXM,CString strSdeLyNameDk,
		         CString strSdeLyNameXM,CString strURL,CString strProcessId);

public:
	//初始化
	virtual BOOL OnInitDialog();

	// 对话框数据
	enum { IDD = IDD_DLG_LXINPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;
	afx_msg void OnBnClickedSelectYD();
	afx_msg void OnBnClickedSelectXM();
	afx_msg void OnBnClickedSelectPT();
	afx_msg void OnBnClickedSelectPT_T();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnModify();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnSde();
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnPos();

	DECLARE_MESSAGE_MAP()

private:

	BOOL GetValueFromControls(DIST_CADENTITY_STRUCT& Value);
	void SetValueToControls();
	void SetValueToXData(AcDbObjectId& ObjId,const char* strRegName,DIST_CADENTITY_STRUCT& Value);

	BOOL ReadEntityInfo();
	int ReadXmPos(AcGePoint3d& pt);

	//临时
	//void CallDap(HWND hDapHwnd,CString strXMBH,CString strURL,CString strZMJ,CString strJSYDMJ);

	void RestoreEntityInfo();

	BOOL IsXMDKInfoExists();
	void DeleteXMDKINFO();

	void DeleteBlkAttRef(CString strBlkName);

	void ReadAttrFromRef(CString& strValue,CString strTag,AcDbBlockReference* pBckRef);

	BOOL StringToDate(DIST_DATETIME_X* TempDate,char* strDateBuf);

	void ClearOldInfo();

	BOOL GetBlockNameByBlkRefId(CString &strName,AcDbObjectId objId);

	BOOL PointIsInPolygon(AcGePoint3d pt, AcGePoint3dArray& ptArr);
	BOOL PolygonIsInPolygon(AcGePoint3dArray& ptsA, AcGePoint3dArray& ptsB);

	BOOL CheckCrossOrContain(AcGePoint3dArray& ptArray,CString strSelfSdeLyName);
	BOOL ReadCfgLyNameArray(CStringArray& strLyNameArray,CString strTag);

public:
	CAcUiSelectButton	     m_sBtnYD;
	CAcUiSelectButton	     m_sBtnXM;
	CAcUiSelectButton	     m_sBtnPT;
	CAcUiSelectButton	     m_sBtnPT_T;
	CListCtrl                m_list_base;
	CListCtrl                m_list_ydxx;

private:
	IDistConnection *        m_pConnect;          //SDE 连接对象指针
	CString                  m_strCadLyNameDK;    //当前操作CAD图层
	CString                  m_strCadLyNameXM;    //当前操作CAD图层
	CString                  m_strSdeLyNameDK;    //当前操作SDE图层
	CString                  m_strSdeLyNameXM;    //当前操作SDE图层
	CString                  m_strURL;            //WebService地址
	CString                  m_strDapProcessId;

	DIST_CADENTITY_ARRAY     m_OldEntityArray;  //保存操作CAD实体信息,以备恢复操作
	BOOL                     m_bIsModify;
	DIST_CADENTITY_ARRAY     m_NewEntityArray;  //保存操作CAD实体信息

	DIST_PRJBASEINFO_STRUCT  m_PrjBaseInfo;     //项目基本信息

	CStringArray             m_strNameArrayDK;
	CStringArray             m_strNameArrayXM;
	CStringArray             m_strFNameArrayDK;
	CStringArray             m_strFNameArrayXM;

public:
	afx_msg void OnNMClickListYdxx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeCmbYdlx();
	afx_msg void OnBnClickedCheckXm();
	afx_msg void OnCbnSelchangeCmbDkbh();
	afx_msg void OnCbnSelchangeCmbZg();
	afx_msg void OnCbnSelchangeCmbXk();
	afx_msg void OnEnKillfocusEditBz();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};




#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"
#include "..\SdeData\DistBaseInterface.h"

//蓝线属性数据结构
struct DIST_LXSX_STRUCT
{
	CString m_strLCH;     //流程号
	CString m_strZH;      //证号
	CString m_strRQ;      //日期
	CString m_strXMMC;    //项目名称
	CString m_strJSDW;    //建设单位
	CString m_strYDXZ;    //用地性质
	CString m_strLXLX;    //蓝线类型
	CString m_strYDMJ;    //用地面积
	CString m_strYDLX;    //用地类型
	CString m_strDKBH;    //地块编号
	CString m_strZG;      //字高
	CString m_strXK;      //线宽
	CString m_strX;       //标注X值
	CString m_strY;       //标注Y值
	CString m_strJBR;     //经办人
	CString m_strBZ;      //备注

	CString m_strObjId;   //实体ID
};



//-----------------------------------------------------------------------------
class CDlgXmInput : public CAcUiDialog 
{
	DECLARE_DYNAMIC (CDlgXmInput)

public:
	CDlgXmInput (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	void SetInfo(IDistConnection * pConnect,CString strCadLyName,CString strSdeLyName,
		         CString strURL,CString strProjectId,CString strPrePrjId);

	enum { IDD = IDD_DLG_XMINPUT} ;
public:
	CAcUiSelectButton	m_sBtnYD;
	CAcUiSelectButton	m_sBtnPT;

	DIST_LXSX_STRUCT    m_LXSX; //数据结构

	CListCtrl m_list_base;
	CListCtrl m_list_ydxx;


public:

	//初始化
	virtual BOOL OnInitDialog();

	// 数据结构 <=> 显示控件
	void SetValueToControls();
	void GetValueFromControls();

	// 数据结构 <=> 扩展属性
	void SetValueToXData(AcDbObjectId& ObjId,const char* strRegName);
	void GetValueFromXData(AcDbObjectId& ObjId,const char* strRegName);

	void ReadDataToList(const char* strLCH,const char* strCadLyName,const char* strSdeLyName);

	void CallDap(HWND hDapHwnd,CString strXMBH,CString strURL,CString strValue);

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;
	afx_msg void OnBnClickedSelectYD();
	afx_msg void OnBnClickedSelectPT();


	DECLARE_MESSAGE_MAP()
private:
	CString GetAttrValue(CString sKey);



public:
	static CStringArray    m_FNameArray;       //字段名称
	static CStringArray    m_FAliasNameArray;  //字段名称别名
	static CStringArray    m_LxsxArray;        //项目属性数组

	CString       m_strCadLyName;
	CString       m_strSdeLyName;
	CString       m_strURL;
	CString       m_strProjectId;
	CString       m_strPrePrjId;

	IDistConnection * m_pConnect;

public:
	afx_msg void OnCbnSelchangeCmbYdlx();
	afx_msg void OnNMClickListYdxx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnModify();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnSde();
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnPos();
} ;

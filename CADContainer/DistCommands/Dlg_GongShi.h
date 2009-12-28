#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"

//-----------------------------------------------------------------------------
class CDlgGongShi : public CAcUiDialog {
	DECLARE_DYNAMIC (CDlgGongShi)

public:
	CDlgGongShi (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;

	enum { IDD = IDD_DLG_GONGSHI} ;

	CString m_strLSH; // 流水号
	CString m_strGSBH; // 公示编号
	CString m_strBJBH; // 报建编号
	CString m_strXMMC; // 项目名称
	CString m_strJSDW; // 建设单位
	CString m_strYDXZ; // 用地位置
	CString m_strGCXZ; // 工程性质

	CString m_strLXDH; // 联系电话
	CString m_strLXDZ; // 联系地址

	CString m_strWebSite; // 网站

	long m_nClass; // 公示类型
	CString m_strGSLX; // 公示类型
	long m_nGSLM; // 公示栏目
	CString m_strGSLM; // 公示栏目
	CString m_strGSXX; // 公示形式
	CDateTimeCtrl m_wndStartTime;
	CDateTimeCtrl m_wndEndTime;
	SYSTEMTIME m_tmStart;
	SYSTEMTIME m_tmEnd;

	int m_nLimit; // 公示期限

	CString m_strYDZMJ; // 地块总面积
	CString m_strYDJMJ; // 地块净面积
	CString m_strYDDZMJ; // 代征道路绿化面积
	CString m_strRJL; // 容积率
	CString m_strJZMD; // 建筑密度
	CString m_strJZGM; // 建筑规模
	CString m_strJZZG; // 建筑总高
	CString m_strLDL; // 绿地率

	CString m_strBHGZ; // 编号规则

	BOOL m_bExt; // 外部文件公示
//	CString m_strFileName;
	CListBox m_ctrlFileList;
	CStringArray m_strFileArray;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnBrowse();
	afx_msg void OnSelchangeCombo4();
	afx_msg void OnBnClickedBtnDeleteEx();
} ;

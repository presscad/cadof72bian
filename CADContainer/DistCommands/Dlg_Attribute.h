#pragma once
#include "resource.h"
#include ".\SortListCtrl\SortListCtrl.h"

struct COLUMN_VALUE
{
	CString strName;
	CString strAlias;
	CString strValue;
};


// CDlgAttribute 对话框

class CDlgAttribute : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgAttribute)

public:
	CDlgAttribute(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAttribute();

// 对话框数据
	enum { IDD = IDD_ATTRIBUTE };

	CSortListCtrl   m_wndList;

//	AcDbObjectId m_objId;
	AcDbObjectIdArray m_objIds;
	CString m_trCadLayerName;
	CString m_strSdeLayerName;
	BOOL m_bInsertOnly;

	CArray<COLUMN_VALUE, COLUMN_VALUE> paramList;

public:
	void SetValues(LPCTSTR lpCadLayerName, LPCTSTR lpSdeLayerName, BOOL bInsertOnly, AcDbObjectId& objId) { m_trCadLayerName = lpCadLayerName, m_strSdeLayerName = lpSdeLayerName; m_bInsertOnly = bInsertOnly; m_objIds.append(objId); }
	void SetValues(LPCTSTR lpCadLayerName, LPCTSTR lpSdeLayerName, BOOL bInsertOnly, AcDbObjectIdArray& objIds) { m_trCadLayerName = lpCadLayerName, m_strSdeLayerName = lpSdeLayerName; m_bInsertOnly = bInsertOnly; m_objIds.append(objIds); }

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT OnBeginEdit(WPARAM, LPARAM);
	afx_msg LRESULT OnEndEdit(WPARAM, LPARAM);
};

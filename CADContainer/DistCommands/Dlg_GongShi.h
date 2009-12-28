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

	CString m_strLSH; // ��ˮ��
	CString m_strGSBH; // ��ʾ���
	CString m_strBJBH; // �������
	CString m_strXMMC; // ��Ŀ����
	CString m_strJSDW; // ���赥λ
	CString m_strYDXZ; // �õ�λ��
	CString m_strGCXZ; // ��������

	CString m_strLXDH; // ��ϵ�绰
	CString m_strLXDZ; // ��ϵ��ַ

	CString m_strWebSite; // ��վ

	long m_nClass; // ��ʾ����
	CString m_strGSLX; // ��ʾ����
	long m_nGSLM; // ��ʾ��Ŀ
	CString m_strGSLM; // ��ʾ��Ŀ
	CString m_strGSXX; // ��ʾ��ʽ
	CDateTimeCtrl m_wndStartTime;
	CDateTimeCtrl m_wndEndTime;
	SYSTEMTIME m_tmStart;
	SYSTEMTIME m_tmEnd;

	int m_nLimit; // ��ʾ����

	CString m_strYDZMJ; // �ؿ������
	CString m_strYDJMJ; // �ؿ龻���
	CString m_strYDDZMJ; // ������·�̻����
	CString m_strRJL; // �ݻ���
	CString m_strJZMD; // �����ܶ�
	CString m_strJZGM; // ������ģ
	CString m_strJZZG; // �����ܸ�
	CString m_strLDL; // �̵���

	CString m_strBHGZ; // ��Ź���

	BOOL m_bExt; // �ⲿ�ļ���ʾ
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


#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "resource.h"
#include "..\SdeData\DistBaseInterface.h"

//�����������ݽṹ
struct DIST_LXSX_STRUCT
{
	CString m_strLCH;     //���̺�
	CString m_strZH;      //֤��
	CString m_strRQ;      //����
	CString m_strXMMC;    //��Ŀ����
	CString m_strJSDW;    //���赥λ
	CString m_strYDXZ;    //�õ�����
	CString m_strLXLX;    //��������
	CString m_strYDMJ;    //�õ����
	CString m_strYDLX;    //�õ�����
	CString m_strDKBH;    //�ؿ���
	CString m_strZG;      //�ָ�
	CString m_strXK;      //�߿�
	CString m_strX;       //��עXֵ
	CString m_strY;       //��עYֵ
	CString m_strJBR;     //������
	CString m_strBZ;      //��ע

	CString m_strObjId;   //ʵ��ID
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

	DIST_LXSX_STRUCT    m_LXSX; //���ݽṹ

	CListCtrl m_list_base;
	CListCtrl m_list_ydxx;


public:

	//��ʼ��
	virtual BOOL OnInitDialog();

	// ���ݽṹ <=> ��ʾ�ؼ�
	void SetValueToControls();
	void GetValueFromControls();

	// ���ݽṹ <=> ��չ����
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
	static CStringArray    m_FNameArray;       //�ֶ�����
	static CStringArray    m_FAliasNameArray;  //�ֶ����Ʊ���
	static CStringArray    m_LxsxArray;        //��Ŀ��������

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

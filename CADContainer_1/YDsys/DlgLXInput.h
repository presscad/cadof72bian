#pragma once
#include "DistBasePara.h"
#include "acui.h"
#include "resource.h"
#include "..\SdeData\DistBaseInterface.h"


//����CADʵ������
struct DIST_CADENTITY_STRUCT
{
	CString strDKCode;      //�ؿ���    DKBH
	CString strYDType;      //�õ�����    YDLX
	CString strArea;        //�õ����    YDMJ
	CString strTxtHeight;   //�ָ�        ZG
	CString strLnWidth;     //�߿�        XK
	CString strX;           //X ֵ        X
	CString strY;           //Y ֵ        Y
	CString strBZ;          //��ע        BZ
	CString strObjId;       //ʵ��ID
};
typedef CArray<DIST_CADENTITY_STRUCT,DIST_CADENTITY_STRUCT&> DIST_CADENTITY_ARRAY;





// CDlgLXInput �Ի���

class CDlgLXInput : public CAcUiDialog
{
	DECLARE_DYNAMIC(CDlgLXInput)

public:
	         CDlgLXInput(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLXInput();

	//���ó�ʼ��Ϣ
	void SetInfo(IDistConnection * pConnect,CString strCadLyNameDK,CString strCadLyNameXM,CString strSdeLyNameDk,
		         CString strSdeLyNameXM,CString strURL,CString strProcessId);

public:
	//��ʼ��
	virtual BOOL OnInitDialog();

	// �Ի�������
	enum { IDD = IDD_DLG_LXINPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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

	//��ʱ
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
	IDistConnection *        m_pConnect;          //SDE ���Ӷ���ָ��
	CString                  m_strCadLyNameDK;    //��ǰ����CADͼ��
	CString                  m_strCadLyNameXM;    //��ǰ����CADͼ��
	CString                  m_strSdeLyNameDK;    //��ǰ����SDEͼ��
	CString                  m_strSdeLyNameXM;    //��ǰ����SDEͼ��
	CString                  m_strURL;            //WebService��ַ
	CString                  m_strDapProcessId;

	DIST_CADENTITY_ARRAY     m_OldEntityArray;  //�������CADʵ����Ϣ,�Ա��ָ�����
	BOOL                     m_bIsModify;
	DIST_CADENTITY_ARRAY     m_NewEntityArray;  //�������CADʵ����Ϣ

	DIST_PRJBASEINFO_STRUCT  m_PrjBaseInfo;     //��Ŀ������Ϣ

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



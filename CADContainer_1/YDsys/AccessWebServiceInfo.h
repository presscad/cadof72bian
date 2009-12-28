#ifndef  __ACCESS__WEBSERVICE__INFO__H__
#define  __ACCESS__WEBSERVICE__INFO__H__ 
#include "StdAfx.h"
#include "DistBasePara.h"
class CPowerConfig;
class CPoltIndex;
class CYdxz3Part;
class CBaseCfgInfo;


//===============================================================================================//
#define POWER_HIDE          0x0000      // �����Է���Ȩ��
#define	POWER_SCAN			0x0001		// SDE������Ͳ鿴Ȩ��
#define POWER_EDIT			0x0002		// SDE���޸ĺ����Ȩ��

typedef struct
{
	CString	strTreeID;			// ���ڵ�ID
	CString	strSDEPower;		// Ȩ��
}POWERCONFIG;
typedef CArray<POWERCONFIG,POWERCONFIG&> POWERCONFIGARRAY;

// Ȩ�޿�����˵����
// ��ҵ�����ݿ��TBRIGHT_FORM���ж�ȡȨ�޿���������Ϣ
// �ֶ�PW_ROLE˵���û���ɫ���
// �ֶ�PW_RESOURCEID˵����Դ��Ϣ���߱��
// �ֶ�PW_POWER˵��Ȩ�ޣ�����16���ư�λ����
// �ֶ�PW_CATEGORYΪCADΪ��ģ��ʹ��
class CPowerConfig
{
public:
	         CPowerConfig(void);
	virtual ~CPowerConfig(void);

public:
	static POWERCONFIGARRAY m_PowerArray;

public:
	//��ʱ
	//static BOOL GetCurUserRight(CString strURL,CString strRoleId, CString strXMBH);
	static void Release();
};



//===============================================================================================//

typedef struct
{
	CString	Id;			// ���
	CString	SDELayer;	// SDE������
	CString	AliasName;	// ���ı���(��ʾ����)
	CString	FieldName;	// �����ֶ�����
	CString	OrderNum;	// ����(Ĭ��Ϊ���)
	CString Type;		// ��¼���(1��ʾ��Ŀ���)
}POLTINDEXCONFIG;
typedef CArray<POLTINDEXCONFIG,POLTINDEXCONFIG&> POLTINDEXARRAY;

class CPoltIndex
{
public:
	         CPoltIndex(void);
	virtual ~CPoltIndex(void);

private:

	static POLTINDEXARRAY m_PoltIndArray;

public:
	static void Realse();

	static BOOL GetPoltIndInf(const CString strURL);
	static BOOL GetLayerPoltIndexArray(CStringArray& strSdeLayerNameArray, CStringArray& strFieldNameArray);
	static BOOL GetXMBHInfo(CString& strSdeLayerName, CString& strFieldName);
};


//===============================================================================================//
//�õ���������
typedef struct
{
	int		Type_Id;      
	int		Super_Id;
	CString	Type_Code;
	CString	Type_Name;
	CString	Description;
}YDTYPECONFIG; 

typedef CArray<YDTYPECONFIG,YDTYPECONFIG&> YDTYPEARRAY;

class CYdxz3Part
{
public:
	         CYdxz3Part(void);
	virtual ~CYdxz3Part(void);

	static BOOL GetFromDataBase(const CString strURL);
	static void Release();

	static void GetRootNames(CStringArray& saRootName);
	static void GetParentNameByName(const CString sName, CString& sParentName);
	static void GetChildNameByName(const CString sName, CStringArray& saChildName);

	static void GetCodeByName(const CString sName, CString& sCode, const int sParentId = 0);
	static void GetNameByCode(const CString sCode, CString& sName);

	static int GetIdByName(const CString sName, const int sParentId = 0);


	static YDTYPEARRAY  m_YdTypeArray;
};



//===============================================================================================//
//SDE������Ϣ
typedef struct
{
	CString strServer;
	CString strInstance;
	CString strDatabase;
	CString strUserName;
	CString strPassword;
	CString strGDBOwner; 
}BASESDEINFO; 

typedef struct
{
	CString strServer;
	CString strUserName;
	CString strPassword;
	int nPort;
}BASEFTPINFO; 

//ҵ���������Ϣ
typedef struct
{
	CString strUserId;
	CString strUserName;
}BASEDAPINFO; 

//��Ŀ������Ϣ
typedef struct
{
	CString strXMBH;       //��Ŀ���
	CString strGLBH;       //�������
	CString strTFBH;       //ͼ�����
	CString strProcessId;

	//
	CString strLCH;        //���̺�
	CString strZH;     //֤��
	CString strRQ;     //��������
	CString strXMMC;   //��Ŀ����
	CString strJSDW;   //���赥λ
	CString strYDXZ;   //�õ�����
	CString strLXLX;   //��������

	CString strJSDZ;   //�����ַ
}BASEPROJECTINFO;




class CBaseCfgInfo
{
public:
	static BASESDEINFO              m_BaseSdeInfo;
	static BASEDAPINFO              m_BaseDapInfo;
	//static BASEFTPINFO              m_BaseFtpInfo;
	static DIST_PRJBASEINFO_STRUCT  m_basePrjInfo;

	static AcGePoint3dArray   m_FWpts;     //
	static AcGePoint3dArray   m_XMBHpts;   //��Ŀ��Ŷ�Ӧ�����߷�Χ

	//�������ƣ�	ReadSdeInfo
	//�������ܣ�	ͨ��WebService��ȡSde���ݿ�������Ϣ
	//				��ϵͳ������ʱ���ȡ����������Ϣ
	static BOOL ReadConnectInfo(CString strURL);
	//�������ƣ�	ReadSdeInfoByXML
	//�������ܣ�	ͨ��XML str��ȡSde���ݿ�������Ϣ
	//				��ϵͳ������ʱ���ȡ����������Ϣ
	static BOOL ReadConnectInfoByXML(CString strXML);

	static BOOL ReadFtpInfo(CString strURL);

	//�������ƣ�	ReadProjectBaseInfo
	//�������ܣ�	��ȡTBPROJECT_BASE���й�����Ŀ�Ļ�����Ϣ
	//				��ϵͳ������ʱ���ȡ������Ŀ��Ϣ
	static BOOL ReadProjectBaseInfo(CString strXMBH, CString strURL);

	//�������ƣ�	ReadUserInfo
	//�������ܣ�	��ȡTBUSER���й��ڵ�ǰ�û�����Ϣ
	//				��ϵͳ������ʱ���ȡ�����û���Ϣ
	static BOOL ReadUserInfo(CString strUserId, CString strURL);

	// ��ȡ��Ŀ��ŵĵ����飬�洢��m_XMBHpts
	BOOL GetXMBHPoints(AcGePoint3dArray& ptArray);

	// ��ȡ��ͼͼ���ŷ�Χ�ĵ����飬�洢��m_FWpts
	BOOL GetHTFWPoints();

};


#endif//__ACCESS__WEBSERVICE__INFO__H__
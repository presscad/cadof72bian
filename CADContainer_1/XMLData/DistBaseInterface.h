///////////////////////////////////////////////////////////////////////////////
//�Ϻ�����ϵͳ�������޹�˾ӵ�б�����֮��ȫ��Ȩ��δ���Ϻ�����ϵͳ�������޹�˾ //
//����ȷ������ɣ����ø��ơ�ȡ�á��ַ���Դ���롣                             //
//�κε�λ�͸�����δ����������£��������κ���ʽ���ơ��޸ĺͷ������������ //
//�β��֣�������Ϊ�Ƿ��ֺ����ҹ�˾����������׷�������ε�Ȩ������δ������ //
//����£��κ���֯����˷����Ĳ�Ʒ������ʹ��Dist�ı�־��Logo��               //
//                                                                           //
//support@dist.com.cn                                                        //
//www.dist.com.cn                                                            //
//                                                                           //
// ��Ȩ���� (C) 2007��2010 Dist Inc. ��������Ȩ����                          //
///////////////////////////////////////////////////////////////////////////////
#ifndef __BASE_INTERFACE_XML_H__
#define __BASE_INTERFACE_XML_H__





//=============================  �ӿڶ���˵�� ===============================//
//                                                                           //
// 1. ʵ��Database,SDE,XML,Shape������Դ����ͳһ��������������ȡ���ӿ�����   //
//    ������Ȼ���ڽӿڻ����ϣ����ݲ�ͬ������Դ�����в�ͬ�����װʵ�֡�ע��   //
//    ����Ҫʵ�ֵĽӿڷ�����������ʵ�����е�˽�в��֡�                       //
//                                                                           //
// 2. Ϊ�˷���ʹ�ã���������Դ�в�Σ��������������̷��Ĵ��ܷ�װ��         //
//    Connection ��������Դ������                                            //
//    Command    �������ݼ������ݱ������ɾ���ģ���Ȳ���                    //
//    RecordSet  �������ݼ��ϵĶ�ȡ�����������ҵȲ���                        //
//                                                                           //
//==============================  �ӿ�����  =================================//
#include <string.h>

#ifdef DIST_DATABASE_XML_EXPORTS
#define DIST_DATABASE_XML_API __declspec(dllexport)
#else
#define DIST_DATABASE_XML_API __declspec(dllimport)
#endif



struct IDistConnectionXML;
struct IDistCommandXML;
struct IDistRecordSetXML;
struct IDistParameterXML;

//============================== �������� ===================================//
//�������Ӷ���
// 
DIST_DATABASE_XML_API IDistConnectionXML* WINAPI CreateConnectObjcet(void); 

//��������ִ�ж���
// 
DIST_DATABASE_XML_API IDistCommandXML*  WINAPI CreateCommandObjcet(void); 



//============================== �Զ������� =================================//

//�������Ͷ���
typedef struct{
	int    tm_sec;       // seconds after the minute - [0,59]
	int    tm_min;       // minutes after the hour - [0,59]
	int    tm_hour;      // hours since midnight - [0,23] 
	int    tm_mday;      // day of the month - [1,31]
	int    tm_mon;       // months since January - [0,11] 
	int    tm_year;      // years since 1900 
	int    tm_wday;      // days since Sunday - [0,6] 
	int    tm_yday;      // days since January 1 - [0,365] 
	int    tm_isdst;     // daylight savings time flag
}DIST_DATETIME;


//����������ʽ����
struct DIST_BLOBINFO{
	long   blob_length;  // ������������
	char   *blob_buffer; // �������������� 

	DIST_BLOBINFO(void)
	{
		blob_length = 0;
		blob_buffer = NULL;
	}

	DIST_BLOBINFO& operator =  (const DIST_BLOBINFO& blob)
	{
		blob_length = blob.blob_length;
		if(NULL != blob_buffer)
			delete [] blob_buffer;
		blob_buffer = new char[blob_length];
		memcpy(blob_buffer,blob.blob_buffer,blob_length*sizeof(char));

		return *this;
	}

	~DIST_BLOBINFO(void)
	{
		if(NULL != blob_buffer)
			delete [] blob_buffer;
	}
};


//=============================  �ӿڶ���  ==================================//

//<<< 1. ���ӽӿڶ���
struct IDistConnectionXML
{
	enum PARAMETERTYPE    // ��������
	{
		kServer = 0,
		kInstance =1,
		kDbName = 2,
		kUserName = 3
	};

	enum DIST_ENUM_CONNECTRESULT   //����SDE����ֵ����
	{
		kSucess = 0,
		kFailure = -1,
		kSdeNotStarted = -5,
		kTasksExceeded = -7,
		kLoginNotAllowed = -8,
		kInvalidUser = -9,
		kNetFailure = -10,
		kNetTimeOut = -11,
		kNoAccess = -15,
		kInvalidPointer = -65,	
		kInvalidServer = -100,
		kIomgrNotAvailable = -101,  
		kDbmsDoesNotSupport = -1008
	};

	// ���ܣ���������
	// ������strConnect  �����ַ���
    // ���أ����ز������ 0��ʾ�ɹ�������ֵ����ο��ӿڶ���
	virtual LONG Connect(const char* strConnect) = 0;

	// ���ܣ���������
	// ������strServer    ����������
	//       strInstance  ����ʵ���������
	//       strDatabase  ���ݿ�����
	//       strUserName  �û�����
	//       strPassword  �û�����
	// ���أ����ز������ 0��ʾ�ɹ�������ֵ����ο��ӿڶ����е�DIST_ENUM_CONNECTRESULT
	virtual LONG Connect(const char* strServer,      
		                 const char* strInstance,
		                 const char* strDatabase, 
						 const char* strUserName,
					     const char* strPassword) = 0;

	// ���ܣ��Ͽ�����
	// ��������
	// ���أ���
	virtual void Disconnect(void)  = 0;

	// ���ܣ��������
	// ������IntervalTime��ⳬʱ
	// ���أ�TRUE ���ӳɹ���FALSE ����ʧ��
	virtual BOOL CheckConnect(int IntervalTime=60)  = 0;
	
	// ���ܣ���ȡ����
	// ������param        ��������
	//       strParameter ��������
	// ���أ���
	virtual void GetParameter(PARAMETERTYPE param, char* strParameter) = 0;
    
	// ���ܣ����ò���
	// ������param        ��������
	//       strParameter ��������
	// ���أ���
	virtual void SetParameter(PARAMETERTYPE param, char* strParameter)  = 0;

	// ���ܣ�����һ����Ӧ��command����
	// ������lplpCommand	���շ��ص�command�����ָ��
	// ���أ���
	virtual void QueryCommand(IDistCommandXML** lplpCommand) = 0;

	// ���ܣ��ͷŶ���
	// ��������
	// ���أ���
	virtual void Release(void)  = 0;
};



#define DIST_NIL_TYPE_MASK           (1L)
#define DIST_POINT_TYPE_MASK         (1L<<1)
#define DIST_LINE_TYPE_MASK          (1L<<2)
#define DIST_SIMPLE_LINE_TYPE_MASK   (1L<<3)
#define DIST_AREA_TYPE_MASK          (1L<<4)
#define DIST_MULTIPART_TYPE_MASK     (1L<<18)


//<<< 2. ���ݿ������ӿڶ���
struct IDistCommandXML
{
	// ���ܣ��ͷŶ���
	// ��������
	// ���أ���
	virtual void Release(void) = 0;


	//=====================��¼������ز���=======================//


	// ���ܣ������ݿ��в�ѯ����
	// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
	//       IRcdSet        ���ݲ�ѯ�������صļ�¼��
	//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
	// ���أ�TRUE ��ʾ�м�¼���أ��������ΪFALSE
	virtual LONG SelectData(const char* strSQL,IDistRecordSetXML** IRcdSet,void* pSpatialFilter=NULL) = 0;


	// ���ܣ������ݿ�ָ��������Ӽ�¼
	// ������strTableName   ����������
	//       IRcdSet        ��Ҫ��ӵ����еļ�¼
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	//virtual BOOL InsertData(const char* strTableName,char** strFldNames=NULL,...);
	virtual LONG InsertData(const char* strSQL,...) = 0;//��Ӽ�¼
	

	// ���ܣ��޸����ݿ����ָ����¼
	// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
	//       IRcdSet        ��¼�޸ĵ�����
	//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	//virtual BOOL UpdateData(const char* strSQL,IDistRecordSetXML* IRcdSet,void* pSpatialFilter=NULL);
	virtual LONG UpdateData(const char* strSQL,void* pSpatialFilter=NULL,...) = 0; //�޸ļ�¼

	// ���ܣ�ɾ�����ݿ���еļ�¼
	// ������strSQL         ����SQL��䣬������:������WHERE������Ϣ(��ʽ T:x,x,x,W:x#)
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG DeleteData(const char* strSQL) = 0; 


	//========================��ṹ����==========================//

	// ���ܣ���ȡ���ݱ�ṹ��Ϣ
	// ������strName         ������
	//       pParam          ���ر��ֶνṹ��������
	//       nparamNums      �����ֶνṹ����
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG GetTableInfo(const char* strName,IDistParameterXML** pParam,int& nPamramNum) = 0;

	// ���ܣ��������ݱ�
	// ������strName         �����ı�����
	//       pParam          ���ֶνṹ��������
	//       nparamNums      �ֶνṹ����
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG CreateTable(const char* strName,IDistParameterXML** pParam,int nPamramNum,const char* strKeyword="DEFAULTS") = 0;


	// ���ܣ�ɾ��ָ�����ݱ�
	// ������strName         ������
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG DropTable(const char* strName) = 0;

	// ���ܣ�����ͼ����Ϣ
	// ������strTableName       ͼ�����������
	//       strSpacialFldName  ͼ��ͱ�����Ŀռ��ֶ�
	//       pLyInfo            ͼ����ϢSE_LAYERINFOָ��
    // ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG GetLayerInfo(const char* strTableName,const char* strSpacialFldName,void** pLyInfo) = 0;

	// ���ܣ�����SDEͼ��(���CADʵ�����)
	// ������strTableName      SDE�������������ݱ�����
	//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ�㴴����������Ա������Ӹ��ֶ�)
	//       dScale            ͼ�������
	//       ox,oy             ����ϵԭ��
	//       dGridSize         �����С
	//       lTypeMask         ͼ������
	//       strKeyword        �ؼ���
    // ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG CreateLayer(const char* strTableName,
		                     const char* strSpacialFldName,
							 double dScale,
							 double ox = 0,
							 double oy = 0,
							 double dGridSize = 1000,
							 LONG   lTypeMask = DIST_NIL_TYPE_MASK | DIST_POINT_TYPE_MASK |
							                    DIST_LINE_TYPE_MASK | DIST_SIMPLE_LINE_TYPE_MASK |
							                    DIST_AREA_TYPE_MASK | DIST_MULTIPART_TYPE_MASK,
							 const char* strKeyword="DEFAULT") = 0;


    // ���ܣ�ɾ��SDEͼ��(���CADʵ�����)
	// ������strTableName      SDE�������������ݱ�����
	//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ��ɾ����������Ա���ɾ�����ֶ�)
    // ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG DropLayer(const char* strTableName,const char* strSpacialFldName) = 0;


	//==========================������============================//

	// ���ܣ�����ָ��ͼ������
	// ������strLyName        ͼ������
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG LockLayerData(const char* strLyName) = 0;

	// ���ܣ�����ָ�����ݱ�
	// ������strName          ������
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG LockTable(const char* strName) = 0;

	// ���ܣ�����ָ�����ݱ��е�һ����¼
	// ������strName          ������
	//       nIndexRow        ָ����
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG LockTableARow(const char* strName, int nIndexRow) = 0;

	// ���ܣ�����ָ���������
	// ������pObjs             ����ID����
	// ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual LONG LockApplicationObj(int* pObjs) = 0;



	//=======================��դͼ�����=========================//(����)

	//��������ϵ�͹�դͼ��Ķ���
	virtual LONG CreateCoordinate(void) = 0;
	virtual LONG ReadRasterData(void) = 0;
	virtual LONG WriteRasterData(void) = 0;
};


//<<< 3. ���ݿ��¼����ӿڶ���
struct IDistRecordSetXML
{
	//���ܣ�����ָ�������ƣ�ȡ��Ӧ�ֶε�ֵ
	//������strFldName �ֶ���
	//���أ������ֶ�ֵ(��ȡ��Ҫ����ʵ�����ͣ�ǿ��ת��)
	virtual void* operator[] (const char* strFldName) = 0;

	//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ
	//������nIndex ���
	//���أ������ֶ�ֵ(��ȡ��Ҫ����ʵ�����ͣ�ǿ��ת��)
	virtual void* operator[] (int nIndex) = 0;


	//��¼������

	//���ܣ����ζ�ȡ��ѯ���
	//��������
	//���أ��к�����¼����TRUE����¼���귵��FALSE
	virtual BOOL BatchRead(void) = 0;

	//���ܣ����ص�ǰ��ѯ�����µļ�¼���ĵ�һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual BOOL MoveFirst(void) = 0;

	//���ܣ����ص�ǰ��ѯ�����µļ�¼�������һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual BOOL MoveLast(void) = 0;

	//���ܣ����ص�ǰ��ѯ�����µļ�¼����ǰ��¼����һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual BOOL MoveNext(void) = 0;

	//���ܣ����ص�ǰ��ѯ�����µļ�¼����ǰ��¼����һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual BOOL MovePrevious(void) = 0;

	//���ܣ����ص�ǰ��ѯ������ָ����һ����¼
	//������nIndex   ָ����¼��(1 ... n)
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual BOOL MoveTo(int nIndex) = 0;

	//���ܣ�����ȡ��¼���ļ�¼����
	//��������
	//���أ����ؼ�¼����
	virtual int  GetRecordCount(void) = 0;

	//���ܣ�����ȡ��¼�����ֶ�����
	//��������
	//���أ������ֶ�����
	virtual int  GetFieldNum(void) = 0;

	//���ܣ��ͷŶ���
	//��������
	//���أ���
	virtual void Release(void) = 0;
};


//<<< 5. ���ݿ��ֶ���Ϣ�����ṹ
struct IDistParameterXML{
    char  fld_strName[32];   // the column name 
    long  fld_nType;         // the SDE data type 
    long  fld_nSize;         // the size of the column values 
    short fld_nDecimal;      // number of digits after decimal 
    BOOL  fld_bNullAllow;    // allow NULL values ? 
    short fld_nRowIdType;    // column's use as table's row id 

	IDistParameterXML(void)
	{
		memset(fld_strName,0,sizeof(char)*32);
	}

	IDistParameterXML(const char* strName,long nType,int nSize,short nDecimal,BOOL bNullAllow,short nRowIdType)
	{
		strcpy(fld_strName,strName);
		fld_nType = nType;
		fld_nSize = nSize;
		fld_nDecimal = nDecimal;
		fld_bNullAllow = bNullAllow;
		fld_nRowIdType = nRowIdType;//SE_REGISTRATION_ROW_ID_COLUMN_TYPE_SDE 
	}
	
	IDistParameterXML& operator =  (const IDistParameterXML& pm)
	{
		strcpy(fld_strName,pm.fld_strName);
		fld_nType = pm.fld_nType;
		fld_nSize = pm.fld_nSize;
		fld_nDecimal = pm.fld_nDecimal;
		fld_bNullAllow = pm.fld_bNullAllow;
		fld_nRowIdType = pm.fld_nRowIdType;

		return *this;
	}
};





#endif  //__BASE_INTERFACE_XML_H__
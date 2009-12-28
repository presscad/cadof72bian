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
//���� : ����                                                                //
//                                                                           //
// ��Ȩ���� (C) 2007��2010 Dist Inc. ��������Ȩ����                          //
///////////////////////////////////////////////////////////////////////////////
#ifndef __BASE_INTERFACE__H__
#define __BASE_INTERFACE__H__





//=============================  �ӿڶ���˵�� ===============================//
//                                                                           //
// 1. ʵ��Database,SDE,XML,Shape������Դ����ͳһ��������������ȡ���ӿ�����   //
//    ������Ȼ���ڽӿڻ����ϣ����ݲ�ͬ������Դ�����в�ͬ�����װʵ�֡�ע��   //
//    ����Ҫʵ�ֵĽӿڷ�����������ʵ�����е�˽�в��֡�                       //
//                                                                           //
// 2. Ϊ�˷���ʹ�ã���������Դ�в�Σ��������������̷������ܷ�װ��         //
//    Connection ��������Դ������                                            //
//    Command    �������ݼ������ݱ������ɾ���ģ���Ȳ���                    //
//    RecordSet  �������ݼ��ϵĶ�ȡ�����������ҵȲ���                        //
//                                                                           //
//==============================  �ӿ�����  =================================//
#include <string.h>

#ifdef DIST_DATABASE_EXPORTS
#define DIST_DATABASE_API __declspec(dllexport)
#else
#define DIST_DATABASE_API __declspec(dllimport)
#endif



//�ӿ�����
struct IDistConnection;
struct IDistCommand;
struct IDistRecordSet;
struct IDistParameter;
struct IDistPoint;




//============================== �������� ===================================//

//���ݴ����ŷ��ش�����ʾ
DIST_DATABASE_API char* WINAPI ReturnErrorCodeInfo(long nErrorCode);

//�������Ӷ���
DIST_DATABASE_API IDistConnection* WINAPI CreateSDEConnectObjcet(void); 

//��������ִ�ж���
// 
DIST_DATABASE_API IDistCommand*  WINAPI CreateSDECommandObjcet(void); 


//============================== �Զ������� =================================//

// ��ȡ�������ݿ��������
enum PARAMETERTYPE    
{
	kServer = 0,
	kInstance =1,
	kDbName = 2,
	kUserName = 3
};

//����SDE����ֵ����
enum DIST_ENUM_CONNECTRESULT   
{
	kSucess = 1,    //�޸�(0 -> 1   Ϊ����һ����÷���ƥ��)
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

//SDEͼ������
enum DIST_ENUM_SHAPETYPE     
{
	kShapeNil        = 1L,
	kShapePoint      = (1L<<1),
	kShapeLine       = (1L<<2),
	kShapeSimpleLine = (1L<<3),
	kShapeArea       = (1L<<4),
	kShapeMultiPart  = (1L<<18)
};

//RowId����(Ψһ�ֶ�)
enum DIST_ENUM_ROWIDTYPE   
{
	kRowIdColumnSde  = 1, 
	kRowIdColumnUser = 2,
	kRowIdColumnNone = 3
};

//SDE���ֶ�����
enum DIST_ENUM_FIELDTYPE   
{
	kInt16              = 1, //2-byte Integer
	kInt32              = 2, //4-byte Integer  SE_INTEGER_TYPE
	kFloat32            = 3, //4-byte Float
	kFloat64            = 4, //8-byte Float
	kString             = 5, //Null Term. Character Array
	kBLOB               = 6, //Variable Length Data
	kDate               = 7, //Struct tm Date
	kShape              = 8, //Shape geometry (SE_SHAPE)
	kRaster             = 9, //Raster  
	kXml                =10, //XML Document
	kInt64              =11, //8-byte Integer
	kUUID               =12, //A Universal Unique ID
	kCLOB               =13, //Character variable length data
	kNString            =14, //UNICODE Null Term. Character Array
	kNCLOB              =15 //UNICODE Character Large Object
};

enum DIST_ENUM_ACCESS_PRIV
{
	kSelectPrivilege = (1<<1),
	kUpdatePrivilege = (1<<2),
	kInsertPrivilege = (1<<3),
	kDeletePrivilege = (1<<4)
};

//=============================  �ӿڶ���  ==================================//

//<<< 1. ���ӽӿڶ���
struct IDistConnection
{
	// ���ܣ���������
	// ������strConnect  �����ַ���
    // ���أ����ز������ 0��ʾ�ɹ�������ֵ����ο��ӿڶ���
	virtual long Connect(const char* strConnect) = 0;

	// ���ܣ���������
	// ������strServer    ����������
	//       strInstance  ����ʵ���������
	//       strDatabase  ���ݿ�����
	//       strUserName  �û�����
	//       strPassword  �û�����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0������ֵ����ο��ӿڶ����е�DIST_ENUM_CONNECTRESULT
	virtual long Connect(const char* strServer,      
		                 const char* strInstance,
		                 const char* strDatabase, 
						 const char* strUserName,
					     const char* strPassword) = 0;

	// ���ܣ��Ͽ�����
	// ��������
	// ���أ���
	virtual long Disconnect(void)  = 0;

	// ���ܣ��������
	// ������IntervalTime��ⳬʱ
	// ���أ�TRUE ���ӳɹ���FALSE ����ʧ��
	virtual long CheckConnect(int IntervalTime=60)  = 0;
	
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

	// ���ܣ���ȡSDE���ݿ����Ӷ��� SE_CONNECTION
	// ��������
	// ���أ����ݿ����Ӷ���
	virtual void* GetConnectObjPointer(void) = 0;

	// ���ܣ��ͷŶ���
	// ��������
	// ���أ���
	virtual void Release(void)  = 0;
};

//<<< 2. ���ݿ������ӿڶ���
struct IDistCommand
{
	// ���ܣ��������ݿ����Ӷ���
	// ������pConnect  ���ݿ����Ӷ���
	// ���أ���
	virtual void SetConnectObj(IDistConnection* pConnect) = 0;


	// ���ܣ��ͷŶ���
	// ��������
	// ���أ���
	virtual void Release(void) = 0;


	//=====================��¼������ز���=======================//


	// ���ܣ������ݿ��в�ѯ����
	// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x,##)
	//       IRcdSet        ���ݲ�ѯ�������صļ�¼��
	//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long SelectData(const char* strSQL,IDistRecordSet** IRcdSet,void* pSpatialFilter=NULL) = 0;


	// ���ܣ������ݿ�ָ��������Ӽ�¼
	// ������strTableName   ����������
	//       pRowId         ������Ӽ�¼��IDֵ
	//       strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x,##)
	//       ...            �����ֶ�ֵ�б�(������F:x,x,x������Ӧ)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long InsertData(int* pRowId,const char* strSQL,...) = 0;//��Ӽ�¼


	// ���ܣ������ݿ�ָ��������Ӽ�¼
	// ������strTableName   ����������
	//       pRowId         ������Ӽ�¼��IDֵ
	//       strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x,##)
	//       strValueArray  ֵ����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long InsertDataAsString(int* pRowId,const char* strSQL,char** strValueArray)=0;
	

	// ���ܣ��޸����ݿ����ָ����¼
	// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x,##)
	//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
	//       ...            �����ֶ�ֵ�б�(������F:x,x,x������Ӧ)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long UpdateData(const char* strSQL,void* pSpatialFilter=NULL,...) = 0; //�޸ļ�¼


	// ���ܣ��޸����ݿ����ָ����¼
	// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x,##)
	//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
	//       ...            �����ֶ�ֵ�б�(����Ϊ�ַ���)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long UpdateDataAsString(const char *strSQL,char** strValueArray)= 0;


	// ���ܣ�ɾ�����ݿ���еļ�¼
	// ������strSQL         ����SQL��䣬������:������WHERE������Ϣ(��ʽ T:x,x,x,W:x,##)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DeleteData(const char* strSQL) = 0; 



	// ���ܣ���ʼ�������
	// ��������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long BeginTrans(void) = 0;

	// ���ܣ������ύ
	// ��������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long CommitTrans(void) = 0;

	// ���ܣ�����ع�
	// ��������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long RollbackTrans(void) = 0;


	//========================��ṹ����==========================//

	// ���ܣ���ȡ���ݱ�ṹ��Ϣ
	// ������pParam          ���ر��ֶνṹ��������
	//       nparamNums      �����ֶνṹ����
	//       strName         ������
	//       strOwnerGDB     GDB��ӵ����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	// ע�⣺�ú�������pParam�Ŀռ�������ں�������ɣ�����pParam�Ŀռ���CSdeCommand��
	//       �Լ����ͷţ����ⲿֻ����IDistCommand����������Χ�ڣ�ʹ��������ָ�룻���
	//       ��Ҫȫ����ʹ�ã���Ҫ�����ݸ��Ƴ��������������Υ�����
	virtual long GetTableInfo(IDistParameter** pParam,int* nPamramNum,const char* strName,const char* strOwnerGDB=NULL) = 0;

	// ���ܣ��������ݱ�
	// ������strName         �����ı�����
	//       pParam          ���ֶνṹ��������
	//       nparamNums      �ֶνṹ����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long CreateTable(const char* strName,IDistParameter* pParam,int nPamramNum,const char* strKeyword="DEFAULTS") = 0;


	// ���ܣ�ɾ��ָ�����ݱ�
	// ������strName         ������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DropTable(const char* strName) = 0;


	// ���ܣ�����SHAPE�ṹָ�룬��ȡ�������
	// ������pShapeType  ����SHAPE����
	//       ptArray     �������������
	//       nPtCount    ������������
	//       pShape      ����SHAPE��ָ��(ע����SE_SHAPE*)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GetShapePointArray(int* pShapeType,IDistPoint** ptArray,int* pPtCount,int* pShape)=0;


	// ���ܣ����ص�ǰ��½�û��ܷ�������ͼ�������
	// ������strLyNameArray   ����ͼ��������
	//       pCount           ������������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GetCurUserAllLyName(char*** strLyNameArray,long* pCount)=0;

	// ���ܣ�����ͼ����Ϣ
	// ������strTableName       ͼ�����������
	//       strSpacialFldName  ͼ��ͱ�����Ŀռ��ֶ�
	//       pLyInfo            ͼ����ϢSE_LAYERINFOָ��
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GetLayerInfo(const char* strTableName,const char* strSpacialFldName,void** pLyInfo) = 0;


	// ���ܣ�����ͼ����Ϣ
	// ������strLyName        ͼ������
	//       strShapeFldName  ͼ��ͱ�����Ŀռ��ֶ�
	//       pType            ����ͼ����������
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GetLayerType(const char* strLyName,const char* strShapeFldName,long* pType)=0;


	// ���ܣ�����ͼ������ϵ��Ϣ
	// ������strLyName        ͼ�����������
	//       strShapeFldName  ͼ��ͱ�����Ŀռ��ֶ�
	//       pCoordref        ͼ������ϵ��Ϣָ��
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GetLayerCoordref(const char* strLyName,const char* strShapeFldName,void** pCoordref)=0;


	// ���ܣ�����SDEͼ��(���CADʵ�����)
	// ������strTableName      SDE�������������ݱ�����
	//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ�㴴����������Ա������Ӹ��ֶ�)
	//       dScale            ͼ�������
	//       ox,oy             ����ϵԭ��
	//       dGridSize         �����С
	//       lTypeMask         ͼ������
	//       strKeyword        �ؼ���
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long CreateLayer(const char* strTableName,
		                     const char* strSpacialFldName,
							 double dScale,
							 double ox = 0,
							 double oy = 0,
							 double dGridSize = 1000,
							 long   lTypeMask = kShapeNil | kShapePoint |
							                    kShapeLine | kShapeSimpleLine|
							                    kShapeArea | kShapeMultiPart,
							 const char* strKeyword="DEFAULT") = 0;


    // ���ܣ�ɾ��SDEͼ��(���CADʵ�����)
	// ������strTableName      SDE�������������ݱ�����
	//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ��ɾ����������Ա���ɾ�����ֶ�)
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DropLayer(const char* strTableName,const char* strSpacialFldName) = 0;


	//========================== ����Ȩ�� ============================//

	// ���ܣ�Ϊ�û�����ͼ�����Ȩ��
	// ������strUser                  ��Ȩ�û�����
	//       strLayerName             ͼ������
	//       strFldNameArray          ��Ȩ�ֶ��б�(ֵΪNULʱ��ʾͼ���������ֶ�)
	//       nFldCount                �ֶ�����
	//       nPrivilege               ����Ȩ�ޣ��ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|'��
	//       bAllowPrivilegesToOther  �Ƿ�������Ȩ����Ȩ�������û�    
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GrantUserAccessLayerPrivilege(const char *strUser,const char *strLayerName,
		                                       char** strFldNameArray,int nFldCount,
		                                       long nPrivilege=kSelectPrivilege,
											   BOOL bAllowPrivilegesToOther=FALSE)=0;

	// ���ܣ�ɾ���û�����ͼ��ָ���ֶεķ���Ȩ��
	// ������strUser                  ��Ȩ�û�����
	//       strLayerName             ͼ������
	//       strFldNameArray          ��Ȩ�ֶ��б�(ֵΪNULʱ��ʾͼ���������ֶ�)
	//       nFldCount                �ֶ�����
	//       nPrivilege               ����Ȩ�ޣ��ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|'�� 
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long RemoveUserAccessLayerPrivilege(const char *strUser,const char *strLayerName,
		                                       char** strFldNameArray,int nFldCount,
		                                       long nPrivilege=kSelectPrivilege | kUpdatePrivilege | 
											                   kInsertPrivilege | kDeletePrivilege)=0;

	// ���ܣ�Ϊ�û����乤�������Ȩ��
	// ������strUser                  ��Ȩ�û�����
	//       strTableName             ����������
	//       nPrivilege               ����Ȩ�ޣ��ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|'��
	//       bAllowPrivilegesToOther  �Ƿ�������Ȩ����Ȩ�������û�    
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GrantUserAccessTablePrivilege(const char *strUser,const char* strTableName,
		                                       long nPrivilege=kSelectPrivilege,
		                                       BOOL bAllowPrivilegesToOther=FALSE)=0;

	// ���ܣ�ɾ���û�������ָ������Ȩ��
	// ������strUser                  ��Ȩ�û�����
	//       strTableName             ����������
	//       nPrivilege               ����Ȩ��(�ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|')
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long RemoveUserAccessTablePrivilege(const char *strUser,const char* strTableName,
		                                       long nPrivilege=kSelectPrivilege | kUpdatePrivilege | 
											                   kInsertPrivilege | kDeletePrivilege)=0;



	//==========================������============================//

	// ���ܣ�����ָ��ͼ������
	// ������strLyName        ͼ������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long LockLayerData(const char* strLyName) = 0;

	// ���ܣ�����ָ�����ݱ�
	// ������strName          ������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long LockTable(const char* strName) = 0;

	// ���ܣ�����ָ�����ݱ��е�һ����¼
	// ������strName          ������
	//       nIndexRow        ָ����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long LockTableARow(const char* strName, int nIndexRow) = 0;

	// ���ܣ�����ָ���������
	// ������pObjs             ����ID����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long LockApplicationObj(int* pObjs) = 0;



	//=======================��դͼ�����=========================//(����)

	//��������ϵ�͹�դͼ��Ķ���
	virtual long CreateCoordinate(void) = 0;
	virtual long ReadRasterData(void) = 0;
	virtual long WriteRasterData(void) = 0;
};


//<<< 3. ���ݿ��¼����ӿڶ���
struct IDistRecordSet
{
	//���ܣ�����ָ�������ƣ�ȡ��Ӧ�ֶε�ֵ
	//������strFldName �ֶ���
	//���أ������ֶ�ֵ(��ȡ��Ҫ����ʵ�����ͣ�ǿ��ת��)
	virtual void* operator[] (const char* strFldName) = 0;

	//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ
	//������nIndex ���
	//���أ������ֶ�ֵ(��ȡ��Ҫ����ʵ�����ͣ�ǿ��ת��)
	virtual void* operator[] (int nIndex) = 0;

	//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ,���ֵΪ�ַ���
	//������strValue ���ؽ���ַ��� 
	//      nIndex   ���
	//���أ�1 ��ʾ�ɹ���0 ��ʾʧ�ܣ������ο�����
	virtual long GetValueAsString(char *strValue,int nIndex)=0;

	//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ,���ֵΪ�ַ���
	//������strValue   ���ؽ���ַ��� 
	//      strFldName �ֶ���
	//���أ�1 ��ʾ�ɹ���0 ��ʾʧ�ܣ������ο�����
	virtual long GetValueAsString(char *strValue,const char* strFldName)=0;


	//��¼������

	//���ܣ����ζ�ȡ��ѯ���
	//��������
	//���أ��к�����¼����TRUE����¼���귵��FALSE
	virtual long BatchRead(void) = 0;

	//���ܣ����ص�ǰ��ѯ�����µļ�¼���ĵ�һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MoveFirst(void) = 0;

	//���ܣ����ص�ǰ��ѯ�����µļ�¼�������һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MoveLast(void) = 0;

	//���ܣ����ص�ǰ��ѯ�����µļ�¼����ǰ��¼����һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MoveNext(void) = 0;

	//���ܣ����ص�ǰ��ѯ�����µļ�¼����ǰ��¼����һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MovePrevious(void) = 0;

	//���ܣ����ص�ǰ��ѯ������ָ����һ����¼
	//������nIndex   ָ����¼��(1 ... n)
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MoveTo(int nIndex) = 0;

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


//<<< 4. ���ݿ��ֶ���Ϣ�����ṹ
struct IDistParameter
{
    char  fld_strName[32];   
    long  fld_nType;         // DIST_ENUM_FIELDTYPE
    long  fld_nSize;          
    short fld_nDecimal;       
    BOOL  fld_bNullAllow;    
    short fld_nRowIdType;    // DIST_ENUM_ROWIDTYPE
	char  fld_strAliasName[32]; //�����ֶ�

	IDistParameter(void)
	{
		memset(fld_strName,0,sizeof(char)*32);
		memset(fld_strAliasName,0,sizeof(char)*32);
	}

	IDistParameter(const char* strName,const char* strAliasName,long nType,int nSize,short nDecimal,BOOL bNullAllow,short nRowIdType)
	{
		strcpy(fld_strName,strName);
		strcpy(fld_strAliasName,strAliasName);
		fld_nType = nType;
		fld_nSize = nSize;
		fld_nDecimal = nDecimal;
		fld_bNullAllow = bNullAllow;
		fld_nRowIdType = nRowIdType;//SE_REGISTRATION_ROW_ID_COLUMN_TYPE_SDE 
	}
	
	IDistParameter& operator =  (const IDistParameter& pm)
	{
		strcpy(fld_strName,pm.fld_strName);
		strcpy(fld_strAliasName,pm.fld_strAliasName);

		fld_nType = pm.fld_nType;
		fld_nSize = pm.fld_nSize;
		fld_nDecimal = pm.fld_nDecimal;
		fld_bNullAllow = pm.fld_bNullAllow;
		fld_nRowIdType = pm.fld_nRowIdType;

		return *this;
	}
};

//�������Ͷ���
struct DIST_DATETIME_X{
	int    tm_sec;       // seconds after the minute - [0,59]
	int    tm_min;       // minutes after the hour - [0,59]
	int    tm_hour;      // hours since midnight - [0,23] 
	int    tm_mday;      // day of the month - [1,31]
	int    tm_mon;       // months since January - [0,11] 
	int    tm_year;      // years since 1900 
	int    tm_wday;      // days since Sunday - [0,6] 
	int    tm_yday;      // days since January 1 - [0,365] 
	int    tm_isdst;     // daylight savings time flag
};

struct IDistPoint
{
	double x;
	double y;
};

/*


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
};*/

#endif  //__BASE_INTERFACE__H__

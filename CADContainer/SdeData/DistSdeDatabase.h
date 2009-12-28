#ifndef  __DIST__SDE__DATABASE__H__
#define  __DIST__SDE__DATABASE__H__


#include "sdeerno.h"
#include "sdetype.h"
#include "sderaster.h"

#include "DistBaseInterface.h"
#include "DistIntArray.h"
/////////////////////////////////////////////////////////////////////////////
//˵�������ǵ��ӿڶ����ͨ���ԣ��ӿڶ������Щӷ�ף���ʵ�ֽӿ�ʱ��ֻ�����
//      �Լ���Ӧ������ѡ��ʵ�ֶ�Ӧ�ӿں�����������Ҫʵ�ֵĽӿں����ŵ�ʵ
//      �����˽�в��ּ��ɡ�
//�޸ļ�¼��1.�����˶�ȡ�ֶα����ĺ���
//          2.������GetLayerType��GetLayerCoordref��GetCurUserAllLyName�Ⱥ���
//          3.��չ�˿��ַ�����
//          4.������Ȩ�޲�������(��Ȩ�ޣ�ͼ��Ȩ��)
//==============================  ������  =================================//
//SDE���ݿ�ֱ�������
class CSdeConnection;
class CSdeCommand;
class CSdeRecordSet;

//����������
class CSdeRaster;



//==============================  �ඨ��  =================================//
//SDE���ݿ�������
class CSdeConnection : public IDistConnection
{	
public:
	//���������
             CSdeConnection();
	virtual ~CSdeConnection();

	// ���ܣ���ȡ���ݿ����Ӷ���ָ��
	// ��������
	// ���أ����ض���ָ��
	SE_CONNECTION GetConnectObj(void);

	// ���ܣ�ȡ��ǰ���ӵ����ݿ������û���
	// ������strDBName    ���ݿ�����
	//       strUserName   �û�����
	// ���أ���
	void GetUserDBInfo(char* strDBName,char* strUserName);

public:
	// ���ܣ���������
	// ������strServer    ����������
	//       strInstance  ����ʵ���������
	//       strDatabase  ���ݿ�����
	//       strUserName  �û�����
	//       strPassword  �û�����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο������е�DIST_ENUM_CONNECTRESULT
	virtual long Connect(const char* strServer,      
		                 const char* strInstance,
		                 const char* strDatabase, 
						 const char* strUserName,
					     const char* strPassword);

	// ���ܣ��Ͽ�����
	// ��������
	// ���أ���
	virtual long Disconnect(void);

	// ���ܣ��������
	// ������IntervalTime��ⳬʱ
	// ���أ�TRUE ���ӳɹ���FALSE ����ʧ��
	virtual long CheckConnect(int IntervalTime=60);
	
	// ���ܣ���ȡ����
	// ������param        ��������
	//       strParameter ��������
	// ���أ���
	virtual void GetParameter(PARAMETERTYPE param, char* strParameter);
    
	// ���ܣ����ò���
	// ������param        ��������
	//       strParameter ��������
	// ���أ���
	virtual void SetParameter(PARAMETERTYPE param, char* strParameter);

	// ���ܣ���ȡSDE���ݿ����Ӷ��� SE_CONNECTION
	// ��������
	// ���أ����ݿ����Ӷ���
	virtual void* GetConnectObjPointer(void);

	// ���ܣ��ͷŶ���
	// ��������
	// ���أ���
	virtual void Release(void);

private:

	//�ú�����XML�ļ�����ʹ�ã����ﲻʵ��
	virtual long Connect(const char* strConnect);

private:
	//���ݿ����Ӷ���ָ��
	SE_CONNECTION m_pConnectObj;  
	char m_strDBName[SE_MAX_DATABASE_LEN];
	char m_strUserName[SE_MAX_OWNER_LEN];
	
};


//SDE���ݿ����ִ����
class CSdeCommand : public IDistCommand
{
public:
	//���������
	         CSdeCommand(void);
	virtual ~CSdeCommand(void);

public:

	// ���ܣ��������ݿ����Ӷ���
	// ������pConnect  ���ݿ����Ӷ���
	// ���أ���
	virtual void SetConnectObj(IDistConnection* pConnect);


	// ���ܣ��ͷŶ���
	// ��������
	// ���أ���
	virtual void Release(void);


	//=====================��¼������ز���=======================//


	// ���ܣ������ݿ��в�ѯ����
	// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
	//       IRcdSet        ���ݲ�ѯ�������صļ�¼��
	//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long SelectData(const char* strSQL,IDistRecordSet** IRcdSet,void* pSpatialFilter=NULL);


	// ���ܣ������ݿ�ָ��������Ӽ�¼
	// ������strTableName   ����������
	//       pRowId         ������Ӽ�¼��IDֵ
	//       ...            �����ֶ�ֵ�б�(������F:x,x,x������Ӧ)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long InsertData(int* pRowId,const char* strSQL,...);//��Ӽ�¼



	// ���ܣ������ݿ�ָ��������Ӽ�¼
	// ������strTableName   ����������
	//       pRowId         ������Ӽ�¼��IDֵ
	//       strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
	//       strValueArray  ֵ����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long InsertDataAsString(int* pRowId,const char* strSQL,char** strValueArray);
	

	// ���ܣ��޸����ݿ����ָ����¼
	// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
	//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
	//       ...            �����ֶ�ֵ�б�(������F:x,x,x������Ӧ)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long UpdateData(const char* strSQL,void* pSpatialFilter=NULL,...); //�޸ļ�¼


	// ���ܣ��޸����ݿ����ָ����¼
	// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
	//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
	//       ...            �����ֶ�ֵ�б�(����Ϊ�ַ���)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long UpdateDataAsString(const char *strSQL,char** strValueArray);


	// ���ܣ�ɾ�����ݿ���еļ�¼
	// ������strSQL         ����SQL��䣬������:������WHERE������Ϣ(��ʽ T:x,x,x,W:x#)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DeleteData(const char* strSQL); 

	// ��ȡ����������
	virtual long GetServerTime(struct tm& tm_server);

	// ���ܣ���ʼ�������
	// ��������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long BeginTrans(void);

	// ���ܣ������ύ
	// ��������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long CommitTrans(void);

	// ���ܣ�����ع�
	// ��������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long RollbackTrans(void);


	//========================��ṹ����==========================//

	// ���ܣ���ȡ���ݱ�ṹ��Ϣ
	// ������pParam          ���ر��ֶνṹ��������
	//       nparamNums      �����ֶνṹ����
	//       strName         ������
	//       strOwnerGDB     GDB��ӵ����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	// ע�⣺�ú�������pParam�Ŀռ�������ں�������ɣ�����pParam�Ŀռ���CSdeCommand��
	//       �Լ����ͷţ����ⲿֻ����CSdeCommand����������Χ�ڣ�ʹ��������ָ�룬���
	//       ��Ҫȫ����ʹ�ã���Ҫ�����ݸ��Ƴ���
	virtual long GetTableInfo(IDistParameter** pParam,int* nPamramNum,const char* strName,const char* strOwnerGDB=NULL);

	// ���ܣ��������ݱ�
	// ������strName         �����ı�����
	//       pParam          ���ֶνṹ��������
	//       nparamNums      �ֶνṹ����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long CreateTable(const char* strName,IDistParameter* pParam,int nPamramNum,const char* strKeyword="DEFAULTS");


	// ���ܣ�ɾ��ָ�����ݱ�
	// ������strName         ������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DropTable(const char* strName);




	// ���ܣ������������Ϣ����SHAPE
	// ������pShape      ���ش���SHAPE��ָ��(��&Shpae)
	//       strLyName   SDEͼ������
	//       ptArray     ��������飨1�������㣬�������β�㲻�ȴ����ߣ��պϴ�����)
	//       nPtCount    ��������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long CreateShapeByCoord(int* pShape,const char* strLyName,IDistPoint* ptArray,int nPtCount);


	// ���ܣ�����ͼ����ָ�������ļ�¼(ֻ����������ͬ���ֶ�)
	// ������strSrcLyName    Դͼ������
	//       strTargeLyName  Ŀ��ͼ������
	//       strWhere        ָ������(�磺OBJECTID>100)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long CopyRecordByCondition(const char* strTargeLyName,const char* strSrcLyName,const char* strWhere, 
		bool bAddVersion = false, const char* strUserName = NULL, const char* strCurTime = NULL);

	// ��ͼ�㿽������
	virtual long CopyRecordsAcrossLayer(LPCTSTR lpDestLayer, LPCTSTR lpSrcLayer, CUIntArray& rowIds, LPCTSTR lpUserName, int nEditRowId);

	// ���ܣ�����SHAPE�ṹָ�룬��ȡ�������
	// ������pShapeType  ����SHAPE����
	//       ptArray     �������������
	//       nPtCount    ������������
	//       pShape      ����SHAPE��ָ��(ע����SE_SHAPE*)
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GetShapePointArray(int* pShapeType,IDistPoint** ptArray,int* pPtCount,int* pShape);


	//=========================ͼ�����===========================//

	// ���ܣ����ص�ǰ���ݿ�������ͼ�������
	// ������strLyNameArray   ����ͼ��������
	//       pCount           ������������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GetCurUserAllLyName(char*** strLyNameArray,long* pCount);

	// ���ܣ�����ͼ����Ϣ
	// ������strTableName       ͼ�����������
	//       strSpacialFldName  ͼ��ͱ�����Ŀռ��ֶ�
	//       pLyInfo            ͼ����ϢSE_LAYERINFOָ��
    // ���أ�TRUE �����ɹ���FALSE ����ʧ��
	virtual long GetLayerInfo(const char* strTableName,const char* strSpacialFldName,void** pLyInfo);


	// ���ܣ�����ͼ����Ϣ
	// ������strLyName        ͼ������
	//       strShapeFldName  ͼ��ͱ�����Ŀռ��ֶ�
	//       pType            ����ͼ����������
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GetLayerType(const char* strLyName,const char* strShapeFldName,long* pType);


	// ���ܣ�����ͼ������ϵ��Ϣ
	// ������strLyName        ͼ�����������
	//       strShapeFldName  ͼ��ͱ�����Ŀռ��ֶ�
	//       pCoordref        ͼ������ϵ��Ϣָ��
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GetLayerCoordref(const char* strLyName,const char* strShapeFldName,void** pCoordref);
	
	// ���ܣ�����SDEͼ��(���CADʵ�����)
	// ������strTableName      SDE�������������ݱ�����
	//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ�㴴����������Ա������Ӹ��ֶ�)
	//       dScale            ͼ�������
	//       ox,oy             ����ϵԭ��
	//       dGridSize         �����С
	//       lTypeMask         ͼ������
	//       strKeyword        �ؼ���
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long CreateLayer(const char* strTbleName,
		                     const char* strSpacialFldName,
							 double dScale,
							 double ox = 0,
							 double oy = 0,
							 double dGridSize = 1000,
							 LONG   lTypeMask = kShapeNil | kShapePoint |
							                    kShapeLine | kShapeSimpleLine|
							                    kShapeArea | kShapeMultiPart,
							 const char* strKeyword="DEFAULT");


    // ���ܣ�ɾ��SDEͼ��(���CADʵ�����)
	// ������strTableName      SDE�������������ݱ�����
	//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ��ɾ����������Ա���ɾ�����ֶ�)
    // ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DropLayer(const char* strTableName,const char* strSpacialFldName);




	//======================== Ȩ�޲��� ==========================//

	// ���ܣ�Ϊ�û�����ͼ�����Ȩ��
	// ������strUser                  ��Ȩ�û�����
	//       strLayerName             ͼ������
	//       strFldNameArray          ��Ȩ�ֶ��б�
	//       nFldCount                �ֶ�����
	//       nPrivilege               ����Ȩ�ޣ��ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|'��
	//       bAllowPrivilegesToOther  �Ƿ�������Ȩ����Ȩ�������û�    
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GrantUserAccessLayerPrivilege(const char *strUser,const char *strLayerName,
		                                       char** strFldNameArray,int nFldCount,
		                                       long nPrivilege=kSelectPrivilege,
											   BOOL bAllowPrivilegesToOther=FALSE);

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
											                   kInsertPrivilege | kDeletePrivilege);

	// ���ܣ�Ϊ�û����乤�������Ȩ��
	// ������strUser                  ��Ȩ�û�����
	//       strTableName             ����������
	//       nPrivilege               ����Ȩ�ޣ��ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|'��
	//       bAllowPrivilegesToOther  �Ƿ�������Ȩ����Ȩ�������û�    
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long GrantUserAccessTablePrivilege(const char *strUser,const char* strTableName,
		                                       long nPrivilege=kSelectPrivilege,
		                                       BOOL bAllowPrivilegesToOther=FALSE);


	// ���ܣ�ɾ���û�������ָ������Ȩ��
	// ������strUser                  ��Ȩ�û�����
	//       strTableName             ����������
	//       nPrivilege               ����Ȩ��(�ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|')
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long RemoveUserAccessTablePrivilege(const char *strUser,const char* strTableName,
		                                        long nPrivilege=kSelectPrivilege | kUpdatePrivilege | 
											                   kInsertPrivilege | kDeletePrivilege);




	//==========================������============================//

	// ���ܣ�����ָ��ͼ������
	// ������strLyName        ͼ������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long LockLayerData(const char* strLyName);

	// ���ܣ�����ָ�����ݱ�
	// ������strName          ������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long LockTable(const char* strName);

	// ���ܣ�����ָ�����ݱ��е�һ����¼
	// ������strName          ������
	//       nIndexRow        ָ����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long LockTableARow(const char* strName, int nIndexRow);

	// ���ܣ�����ָ���������
	// ������pObjs             ����ID����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long LockApplicationObj(int* pObjs);


	



	//=======================��դͼ�����=========================//(����)

	//��������ϵ�͹�դͼ��Ķ���
	virtual long CreateCoordinate(void);
	virtual long ReadRasterData(void);
	virtual long WriteRasterData(void);


private:
	//��������(ʵ��ʱ���)


	//���ܣ������Զ���SQL��䣬��ȡ�ֶΣ���where������Ϣ
	//������strSQL         ���������ַ���
	//      strTableName   ����������
	//      nTableNums     �����
	//      strFieldNames  �ֶ�������
	//      nFieldNums     �ֶ�����
	//      strWhere       WHERE����
	//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
	long ReduceSQL(const char* strSQL, 
		           char*** strTableNames,int& nTableNum, 
				   char*** strFieldNames,int& nFieldNum,
				   char** strWhere);

	long ReduceSQL( const char* strSQL, 
					char*** strTableNames,int& nTableNum, 
					char*** strFieldNames,int& nFieldNum,
					char** strWhere, char** strOrderby);

	//���ܣ����ݲ�ѯ�����������RowId�ֶβ�ѯ���д����ʱ�ļ���
	//������strTableName   ������
	//      strFldName     RowID�ֶ�����
	//      strWhere       Where����
	//      pSpatialFilter �ռ��������
	//���أ���
	void WriteRowIdToArray(const char* strTableName,const char* strFldName,
		                  const char* strWhere,void* pSpatialFilter,CDistIntArray* pData);

private:
	SE_CONNECTION m_pConnectObj;  //���ݿ����Ӷ���
	IDistParameter *m_pParams;
	int             m_nParamCount;

	char            m_strCurTableName[SE_MAX_TABLE_LEN ]; //��ǰ����������
	int             m_nFldTypeArray[SE_MAX_COLUMNS]; //�ֶ���������

	char**          m_strAllLayerNameArray;
	int             m_nLayerNameCount;

	char            m_strDBName[SE_MAX_DATABASE_LEN];  //��½���ݿ���
	char            m_strUserName[SE_MAX_OWNER_LEN];   //��½�û���

	SE_SHAPE        m_tempShape;    //���ⲿ��SHAPE
	IDistPoint*     m_pTempPtArray; //���ⲿʹ�õĵ�����
};


//SDE���ݿ��¼������
class CSdeRecordSet : public IDistRecordSet
{
public:
	//���������
	         CSdeRecordSet();
	virtual ~CSdeRecordSet();


	//���ܣ����ý��������ѯ��Ϣ���ռ������Ϣ(��CSdeCommandʹ��)
	//������pStream    �����
	//      sqlInfo    ��ѯ��Ϣ�ṹ
	//      pFilter    �ռ���˽ṹ
	//      pData      RowId����
	//���أ���
	void SetInfo(SE_STREAM* pStream,SE_QUERYINFO* sqlInfo,SE_FILTER* pFilter,CDistIntArray* pData);

public:
    
	//���ܣ�����ָ�������ƣ�ȡ��Ӧ�ֶε�ֵ
	//������strFldName �ֶ���
	//���أ������ֶ�ֵ(��ȡ��Ҫ����ʵ�����ͣ�ǿ��ת��)
	virtual void* operator[] (const char* strFldName);

	//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ
	//������nIndex ���
	//���أ������ֶ�ֵ(��ȡ��Ҫ����ʵ�����ͣ�ǿ��ת��)
	virtual void* operator[] (int nIndex);

	//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ,���ֵΪ�ַ���
	//������strValue ���ؽ���ַ��� 
	//      nIndex   ���
	//���أ�1 ��ʾ�ɹ���0 ��ʾʧ�ܣ������ο�����
	virtual long GetValueAsString(char *strValue,int nIndex);

	virtual long GetValueAsString(long& nType, char *strValue,int nIndex);


	//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ,���ֵΪ�ַ���
	//������strValue   ���ؽ���ַ��� 
	//      strFldName �ֶ���
	//���أ�1 ��ʾ�ɹ���0 ��ʾʧ�ܣ������ο�����
	virtual long GetValueAsString(char *strValue,const char* strFldName);
	
	virtual long GetValueAsString(long& nType, char *strValue,const char* strFldName);

	//��¼������

	//���ܣ����ζ�ȡ��ѯ���
	//��������
	//���أ��к�����¼����TRUE����¼���귵��FALSE
	virtual long BatchRead(void);

	//���ܣ����ص�ǰ��ѯ�����µļ�¼���ĵ�һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MoveFirst(void);

	//���ܣ����ص�ǰ��ѯ�����µļ�¼�������һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MoveLast(void);

	//���ܣ����ص�ǰ��ѯ�����µļ�¼����ǰ��¼����һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MoveNext(void);

	//���ܣ����ص�ǰ��ѯ�����µļ�¼����ǰ��¼����һ����¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MovePrevious(void);

	//���ܣ����ص�ǰ��ѯ������ָ����һ����¼
	//������nIndex   ָ����¼��(1 ... n)
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	virtual long MoveTo(int nIndex);

	//���ܣ�����ȡ��¼���ļ�¼����
	//��������
	//���أ����ؼ�¼����
	virtual int  GetRecordCount(void);

	//���ܣ�����ȡ��¼�����ֶ�����
	//��������
	//���أ������ֶ�����
	virtual int  GetFieldNum(void);

	//���ܣ��ͷŶ���
	//��������
	//���أ���
	virtual void Release(void);


private:

	//���ܣ������α�λ�ö�ȡ��Ӧ�ļ�¼
	//��������
	//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
	long ReadARow(void);

private:
	SE_CONNECTION    m_pConnectObj;       //���ݿ����Ӷ���
	SE_STREAM        m_pStream;           //��ѯ�����
	SE_COORDREF      m_coordref;          //����ϵ����(��Ҫ���SHAPE�ֶ�)


	SE_COLUMN_DEF   *m_pParams;           //�ֶ���������
	int              m_nParamNum;         //�ֶθ���

	SE_QUERYINFO     m_SqlInfo;           //��ǰ�������Ĳ�ѯ��������
	SE_FILTER*       m_pFilter;           //�ռ��������
    long             m_nTableNum;         //������
	
	char            *m_strOldWhere; //�ɵ�Where����
	int              m_nRecordCount;      //��¼����     

	CDistIntArray   *m_pRowIdData;       //��¼����RowId����
	char             m_strRowIdName[32];

	int              m_nPointer ;         //�α�λ��
	BOOL             m_bBatch;            //�Ƿ��������ζ�����


	//Ϊ����ֵ�ṩ��ʱ�洢λ��
	short            m_nValue16;
	int              m_nValue32;
	float            m_fValue;
	double           m_dValue;
	char             m_strValue[SE_MAX_MESSAGE_LENGTH];
	SE_WCHAR         m_strValueW[SE_MAX_MESSAGE_LENGTH];//���ַ�
	SE_BLOB_INFO     m_pBlob; // ��ʱ��������
	DIST_DATETIME_X    m_tmValue;
	SE_SHAPE         m_shape;
};

#endif //__DIST__SDE__DATABASE__H__

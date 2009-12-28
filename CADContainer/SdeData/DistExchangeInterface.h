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
//��Ȩ���� (C) 2007��2010 Dist Inc. ��������Ȩ����                           //
//���ߣ���                                                                   //
//���ڣ�2007��4��,�Ϻ�.                                                      //
//�޸ģ�                                                                     //
///////////////////////////////////////////////////////////////////////////////
#ifndef  __EXCHANGEDATAINTERFACE__H__
#define  __EXCHANGEDATAINTERFACE__H__

#include "DistBaseInterface.h"


//=============================  �ӿڶ���˵�� ===============================//
// �ýӿ���Ҫ���SDE���ݵ�CADͼ��ʵ��Ļ���ת��
#ifdef DIST_EXCHANGEDATA_EXPORTS
#define DIST_EXCHANGE_API __declspec(dllexport)
#else
#define DIST_EXCHANGE_API __declspec(dllimport)
#endif

struct IDistExchangeData;

//����SDE���CAD���ݻ���������
DIST_EXCHANGE_API IDistExchangeData* WINAPI CreateExchangeObjcet(void); 


//���ܣ� ���ָ�����귶Χ�Ƿ���ĳ��ͼ���ϵ�ʵ�彻����ص�
//������ pConnect
//       strSdeLyName
//       strSQL
//       ptArray[ע��պ�]
//       nPtCount  
//���أ�0 ʧ�ܣ�1 û�н����ص���2���ص�����
//"F:OBJECTID,T:%s,W:PROJECT_ID<>'%s',##"
DIST_EXCHANGE_API long WINAPI gCheckCrossOrContain(IDistConnection* pConnect,const char* strSdeLyName,
												   const char*strSQL, IDistPoint* ptArray,int nPtCount);


enum DIST_ENUM_SELECTTYPE
{
	kSM_ENVP            = 0 ,  // ENVELOPES OVERLAP 
	kSM_ENVP_BY_GRID    = 1 ,  // ENVELOPES OVERLAP 
	kSM_CP              = 2 ,  // COMMON POINT 
	kSM_LCROSS          = 3 ,  // LINE CROSS 
	kSM_COMMON          = 4 ,  // COMMON EDGE/LINE 
	kSM_CP_OR_LCROSS    = 5 ,  // COMMON POINT OR LINE CROSS 
	kSM_LCROSS_OR_CP    = 5 ,  // COMMON POINT OR LINE CROSS 
	kSM_ET_OR_AI        = 6 ,  // EDGE TOUCH OR AREA INTERSECT 
	kSM_AI_OR_ET        = 6 ,  // EDGE TOUCH OR AREA INTERSECT 
	kSM_ET_OR_II        = 6 ,  // EDGE TOUCH OR INTERIOR INTERSECT 
	kSM_II_OR_ET        = 6 ,  // EDGE TOUCH OR INTERIOR INTERSECT 
	kSM_AI              = 7 ,  // AREA INTERSECT 
	kSM_II              = 7 ,  // INTERIOR INTERSECT 
	kSM_AI_NO_ET        = 8 ,  // AREA INTERSECT AND NO EDGE TOUCH 
	kSM_II_NO_ET        = 8 ,  // INTERIOR INTERSECT AND NO EDGE TOUCH 
	kSM_PC              = 9 ,  // PRIMARY CONTAINED IN SECONDARY 
	kSM_SC              = 10,  // SECONDARY CONTAINED IN PRIMARY 
	kSM_PC_NO_ET        = 11,  // PRIM CONTAINED AND NO EDGE TOUCH 
	kSM_SC_NO_ET        = 12,  // SEC CONTAINED AND NO EDGE TOUCH 
	kkSM_PIP            = 13,  // FIRST POINT IN PRIMARY IN SEC 
	kSM_IDENTICAL       = 15,  // IDENTICAL 
	kSM_CBM				= 16   // Calculus-based method [Clementini] 
};
     

//�ռ����������λ�ýṹ
//�ṹ����˵����
//nEntityType  1 ��  2 ��  3Բ  4 ����  5�����
//nSelectType  ���庬��ο������DIST_ENUM_SELECTTYPE�Ķ���
//ptArray      �������ݸ���nEntityType��ֵ��ȷ��
//             1    һ����
//             2    ����������
//             3    �����㣻��һ����ΪԲ�ģ��ڶ������XֵΪ�뾶
//             4    �����㣻��һ����Ϊ������С�㣬�ڶ�����Ϊ��������
//             5    �ĸ������㣬����ĵ�һ���������һ�������
struct DIST_STRUCT_SELECTWAY 
{
	int nEntityType;       
	int nSelectType;
	AcGePoint3dArray ptArray;  
	       
};

struct IDistExchangeData
{
public:

	//���ܣ��ͷŶ���
	//��������
	//���أ���
	virtual void Release(void)=0;

	//���ܣ��������ݿⷴӦ������
	//������bOpen  �Ƿ�����ݿⷴӦ��
	//���أ���
	virtual void SetDbReactorStatues(bool bOpen=true)=0;

	//���ܣ���ȡ��ǰSDE���Ӧ��CAD������
	//����������CADͼ������
	//���أ���
	virtual void GetCurCadLyName(char* strCadLyName)=0;

	//���ܣ���ȡָ������������SHAPE�ĵ�����
	//������strSQL  ��������
	//      ptArray ����Shape�ṹ�ĵ�����
	//���أ���
	virtual void GetSearchShapePointArray(IDistConnection* pConnection, const char* strSQL,AcGePoint3dArray* ptArray) = 0;

	virtual int CopyRecordFromOneToAnother(IDistConnection* pConnection, LPCTSTR lpSrcTable, LPCTSTR lpDestTable, LPCTSTR lpWhere) = 0;

	virtual void GetSearchFieldArray(IDistConnection* pConnection, LPCTSTR strSQL, LPCTSTR lpTable, LPCTSTR lpField, DIST_STRUCT_SELECTWAY* pFilter, CStringArray& array) = 0;


	//���ܣ������Ż����ñ���Ϣ
	//������pConnect              ���ݿ����Ӷ���ָ��
	//      strSymbolTableName    ���Ż�������
	//      strCadMapSdeTableName SDE��CADͼ����ձ�
	//      strSdeLyName          SDEͼ������
	//      strXDataName          ��չ����XDataע������
	//      pXdataArray           ��Ҫд��ʵ����չ���Ե��ֶ�����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long ReadSymbolInfo(IDistConnection *pConnect,
		                        const char* strSymbolTableName,
		                        const char *strCadMapSdeTableName,
								const char *strSdeLyName,
								const char* strXDataName="DIST_XDATA",
								CStringArray* pXdataArray=NULL)=0;

	// ͨ��CADMAPTOSDE���ȡCADͼ�����ʽ����ɫ�����ͣ��߿�
	virtual long ReadCadLayerInfo(IDistConnection * pConnection, LPCTSTR lpCadLayer, int& nColorIndex, CString& lpLineType, AcDb::LineWeight& dLineWeigth) = 0;


	//���ܣ���SDE���ݵ�CAD����ʾ
	//������pObjIdArray  ���ػ���CADʵ���ID����(����ΪAcDbObjectIdArray)
	//      pFilter      �ռ��ѯ��ʽ
	//      strUserWhere �û��Զ����SQL��ѯ����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long ReadSdeDBToCAD(void* pObjIdArray,
		                        DIST_STRUCT_SELECTWAY* pFilter=NULL,
		                        const char* strUserWhere=NULL,BOOL bSingleToSingle = TRUE,BOOL bIsReadOnly=FALSE, BOOL bDeleteExistEntity=TRUE)=0;


	//���ܣ�����CADʵ����Ϣ��SDE��
	//������SelectWay           �ռ��ѯ��ʽ
	//      bAfterSaveToDelete  ������Ƿ��CADͼ����ɾ����Ӧ��ʵ��
	//���أ��ɹ����� 1��ʧ�ܷ��� 0��-1 ��ʾ���������㣬�����ο�����
	virtual long SaveEntityToSdeDB(DIST_STRUCT_SELECTWAY* pFilter=NULL,BOOL bAfterSaveToDelete= TRUE,BOOL bSingleToSingle = TRUE)=0;

	virtual long SaveEntityToSdeDB(AcDbObjectIdArray& objIDs, BOOL bAfterSaveToDelete=TRUE, BOOL bSingleToSingle = TRUE)=0;

	virtual long SaveEntityToSdeDB(AcDbObjectIdArray& objIDs, CString strXMID, BOOL bAfterSaveToDelete=TRUE, BOOL bSingleToSingle = TRUE)=0;

	// ������������
	virtual long SaveInsertEntityToSdeDB(AcDbObjectIdArray& objIds, BOOL bAfterSaveToDelete=TRUE, BOOL bSingleToSingle = TRUE)=0;

	// �����޸Ķ���
	virtual long SaveModifyEntityToSdeDB(AcDbObjectIdArray& objIds, BOOL bAfterSaveToDelete=TRUE, BOOL bSingleToSingle = TRUE)=0;
};

#endif//__EXCHANGEDATAINTERFACE__H__
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
//���ߣ����� <wanghui@dist.com.cn>                                           //
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

struct DIST_POINT_STRUCT
{
	double x;
	double y;
};

struct IDistExchangeData;

//����SDE���CAD���ݻ���������
DIST_EXCHANGE_API IDistExchangeData* WINAPI CreateExchangeObjcet(void); 

DIST_DATABASE_API void  WINAPI gChangeEmpty(BOOL bEmpty); 

DIST_DATABASE_API long WINAPI gSaveSelectEntiyToSDE(IDistConnection *pConnect,const char* strLyName,const char* strCadName);

DIST_DATABASE_API long WINAPI gSaveOneLXToSDE(IDistConnection *pConnect,const char* strSQL,const char* strSdeLyName,
											  char** strValueArray, int nFldCount,
											  DIST_POINT_STRUCT* ptArray,int nPtCount);

DIST_DATABASE_API long WINAPI gSaveOneXMToSDE(IDistConnection* pConnect,const char* strSQL,const char* strSdeLyName,
							                  char** strValueArray,int nFldCount,double x,double y);

DIST_DATABASE_API long WINAPI gCopyRcdFromOneToOther(IDistConnection *pConnect,const char* strSourceTName,
													 char* strRowId,const char* strTargeTName);

DIST_DATABASE_API long WINAPI gCheckSdeData(IDistConnection* pConnect,const char* strSdeLyName,const char*strSQL,
											DIST_POINT_STRUCT* ptArray,int nPtCount,int nCheckTag=1);

//������������Χ
DIST_DATABASE_API long WINAPI gGetDistrictRect(DIST_POINT_STRUCT** ptArray,int* nPtCount,IDistConnection* pConnect,
											   char* strSdeLyName,char* strFldName,char* strFldValue);

//
DIST_DATABASE_API long WINAPI gGetProjectRect(double* pmaxX,double* pmaxY,double* pminX,double* pminY,
											  IDistConnection* pConnect,
											  char* strSdeLyName,char* strFldName,char* strFldValue);

DIST_DATABASE_API long WINAPI gReadShapeInfoFromSDE(char*** pStrFldArray,int* pRcdNum,IDistConnection* pConnect,
								                    char* strSdeLyName,char* strCadYDLyName,char* strProjectCode);


//���ܣ� ���ָ�����귶Χ�Ƿ���ĳ��ͼ���ϵ�ʵ�彻����ص�
//������ pConnect
//       strSdeLyName
//       strSQL
//       ptArray[ע��պ�]
//       nPtCount  
//���أ�0 ʧ�ܣ�1 û�н����ص���2���ص�����
//"F:OBJECTID,T:%s,W:PROJECT_ID<>'%s',##"
DIST_DATABASE_API long WINAPI gCheckCrossOrContain(IDistConnection* pConnect,const char* strSdeLyName,
												   const char*strSQL,IDistPoint* ptArray,int nPtCount);


//���ܣ�����ͼ���ѯ״̬
//������strProjectID	��ĿID
//      strLayerName	ͼ������
//      pMinX			ѡ������
//      pMaxX			ѡ������
//      pMinY			ѡ������
//      pMaxY			ѡ������
//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
DIST_DATABASE_API long WINAPI gSaveQueryLayerToSDE(IDistConnection *pConnect,const char* strProjectID,const char* strLayerName,
													const double* MinX, const double* MaxX, const double* MinY, const double* MaxY);

//���ܣ�����ͼ����ѡ��״̬
//������strProjectID	��ĿID
//      strCheckState	ͼ����ѡ��״̬
//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
DIST_DATABASE_API long WINAPI gSaveTreeCheckStateToSDE(IDistConnection *pConnect,const char* strProjectID,const char* strCheckState);

//���ܣ���ȡͼ����ѡ��״̬
//������strProjectID	��ĿID
//���أ�ͼ����ѡ��״̬���飬ʧ�ܷ��ؿ�����
DIST_DATABASE_API CStringArray* WINAPI gGetTreeCheckStateInSDE(IDistConnection *pConnect,const char* strProjectID);

//���ܣ���ȡSDEͼ���ѯ״̬
//������strProjectID	��ĿID
//      strArraySDELayerNames   ����SDEͼ��������
//      strArrayMINXs			���ض�ӦSDEͼ��ѡ��Χ
//      strArrayMAXXs			���ض�ӦSDEͼ��ѡ��Χ
//      strArrayMINYs			���ض�ӦSDEͼ��ѡ��Χ
//      strArrayMAXYs			���ض�ӦSDEͼ��ѡ��Χ
//���أ��ɹ����� 1��ʧ�ܷ��� 0
DIST_DATABASE_API long WINAPI gGetQueryLayerInSDE(IDistConnection *pConnect,const char* strProjectID,
													CStringArray* strArraySDELayerNames, 
													CStringArray* strArrayMINXs, CStringArray* strArrayMAXXs, 
													CStringArray* strArrayMINYs, CStringArray* strArrayMAXYs);

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
	virtual void GetSearchShapePointArray(const char* strSQL,AcGePoint3dArray* ptArray)=0;


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


	//���ܣ���SDE���ݵ�CAD����ʾ
	//������pObjIdArray  ���ػ���CADʵ���ID����(����ΪAcDbObjectIdArray)
	//      pFilter      �ռ��ѯ��ʽ
	//      strUserWhere �û��Զ����SQL��ѯ����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long ReadSdeDBToCAD(void* pObjIdArray,
		                        DIST_STRUCT_SELECTWAY* pFilter=NULL,
		                        const char* strUserWhere=NULL,BOOL bSingleToSingle = TRUE,BOOL bIsReadOnly=FALSE)=0;


	//���ܣ�����CADʵ����Ϣ��SDE��
	//������SelectWay           �ռ��ѯ��ʽ
	//      bAfterSaveToDelete  ������Ƿ��CADͼ����ɾ����Ӧ��ʵ��
	//���أ��ɹ����� 1��ʧ�ܷ��� 0��-1 ��ʾ���������㣬�����ο�����
	virtual long SaveEntityToSdeDB(DIST_STRUCT_SELECTWAY* pFilter=NULL,BOOL bAfterSaveToDelete= TRUE,BOOL bSingleToSingle = TRUE)=0;

};

#endif//__EXCHANGEDATAINTERFACE__H__
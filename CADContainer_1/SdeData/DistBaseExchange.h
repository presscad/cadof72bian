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

#ifndef __CBASESDEENTITY__H__
#define __CBASESDEENTITY__H__

//////////////////////////////////////////////////////////////////////////////////////////
// ���ܽṹ��
// CExchangeData ����Խӿڵ�ʵ��
// CBaseExchange �����CExchangeData���ܵĲ��ʵ��
//                    CPointExchange�� CAnnotationExchange��...
//�޸ļ�¼��1.�����˶�㣬���ߣ�����Ĵ�������ʵ��֮�����ϵ
//          2.�޸�ReadSdeDBToCAD�������ṩ�û��Զ���SQL��ѯ���
//          3.������SDEͼ����CADͼ�㣬һ��һ��һ�Զ࣬���һ��ʶ��
//          4.�����˶Է��Ż���Ϣ��ģ������
//          5.SDEͼ��ͨ�����ʶ��

#include <sdeerno.h>
#include <sdetype.h>
#include <sderaster.h>
#include <pe.h>
#include <pedef.h>
#include <pef.h>


#include "DistBaseInterface.h"
#include "DistExchangeInterface.h"
#include "DistEntityTool.h"
#include "DistDatabaseReactor.h"



//������
class CBaseExchange;
class CPointExchange;
class CTextExchange;
class CPolylineExchange;
class CPolygonExchange;
class CMultpatchExchange;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct DIST_SYMBOLFLD;
struct DIST_FILLCOLORMAP;

typedef CArray<DIST_FILLCOLORMAP,DIST_FILLCOLORMAP&> DIST_COLORMAPARRAY;

DIST_POINT_STRUCT* gPtArray = NULL;


class CExchangeInfo
{
public:
	         CExchangeInfo();
	virtual ~CExchangeInfo();

public:
	IDistConnection    *m_pConnection;                    //���Ӷ���ָ��
	char                m_strCadLyName[160];              //CADͼ������
	char                m_strSdeLyName[160];              //SDEͼ����
	char                m_strSdeLyKind[60];               //SDEͼ������

	DIST_SYMBOLFLD     *m_pSymbolInfo;	                  //���Ż���Ϣ����	            
	int                 m_nSymbolCount;                   //�����С
	int                 m_nPos;                           //��ǵ�ǰ��ȡ���Ż���¼

	IDistParameter     *m_pParam;                         //�ֶ���Ϣ����
	int                 m_nParamCount;                    //�ֶ�����

	long                m_nShapeType;                     //ͼ������
	SE_COORDREF         m_Coordref;				          //����ϵ

	char                m_strRowIdFldName[60];            //RowId�ֶ�����
	char                m_strShapeFldName[60];            //Shape�ֶ�����

	CStringArray        m_strFldNameArray;                //���Ż��ֶ�
	CStringArray        m_strXDataArrayOut;               //���ӵ�ʵ����չ���Ե��ֶ�

	char                m_strXDataName[255];              //XData��չ����ע������

	CDistEntityTool     m_EntTool;                        //ʵ�幤�߶���

	DIST_COLORMAPARRAY  m_MapColorArray;                  //�õ����ʺ���ɫ���ձ�

private:

	//���ò���
	                    CExchangeInfo(const CExchangeInfo&);
	CExchangeInfo&	    operator=(const CExchangeInfo&);

};



//�ӿ�ʵ����
class CExchangeData : public IDistExchangeData
{
public:
	//���������
	         CExchangeData(void);
	virtual ~CExchangeData(void);

public:

	//���ܣ��ͷŶ���
	//��������
	//���أ���
	virtual void Release(void);

	//���ܣ���ȡ��ǰSDE���Ӧ��CAD������
	//����������CADͼ������
	//���أ���
	virtual void GetCurCadLyName(char* strCadLyName);

	//���ܣ���ȡָ������������SHAPE�ĵ�����
	//������strSQL  ��������
	//      ptArray ����Shape�ṹ�ĵ�����
	//���أ���
	virtual void GetSearchShapePointArray(const char* strSQL,AcGePoint3dArray* ptArray);

	//���ܣ��������ݿⷴӦ������
	//������bOpen  �Ƿ�����ݿⷴӦ��
	//���أ���
	virtual void SetDbReactorStatues(bool bOpen=true);

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
								CStringArray* pXdataArray=NULL);

	//���ܣ���SDE���ݵ�CAD����ʾ
	//������pObjIdArray  ���ػ���CADʵ���ID����(����ΪAcDbObjectIdArray)
	//      pFilter      �ռ��ѯ��ʽ
	//      strUserWhere �û��Զ����SQL��ѯ����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0��-1 ��ʾ���������㣬�����ο�����
	virtual long ReadSdeDBToCAD(void* pObjIdArray,
		                        DIST_STRUCT_SELECTWAY* pFilter=NULL,
		                        const char* strUserWhere=NULL,BOOL bSingleToSingle = TRUE,BOOL bIsReadOnly=FALSE);


	//���ܣ�����CADʵ����Ϣ��SDE��
	//������SelectWay           �ռ��ѯ��ʽ
	//      bAfterSaveToDelete  ������Ƿ��CADͼ����ɾ����Ӧ��ʵ��
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long SaveEntityToSdeDB(DIST_STRUCT_SELECTWAY* pFilter=NULL,BOOL bAfterSaveToDelete= TRUE,BOOL bSingleToSingle = TRUE);

private:
	
	//���ܣ������ṩ��λ�ýṹ��Ϣ�����ɿռ��������
	//������pFilterInfo  λ�ýṹ��Ϣ
	//      pFilter      �ռ��������
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long   CreateFilter(DIST_STRUCT_SELECTWAY* pFilterInfo,SE_FILTER *pFilter);

	//���ܣ�ɾ����ǰ��ѯ��Χ���е�����ʵ��(�����ظ�����)
	//������strCadName   CADͼ����
	//      strSdeName   SDEͼ����(���û��SDEͼ��������˵��CAD����SDE���Ƕ��һ��һ��һ)
	//      pFilter      �ռ��������
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long   DeleteEntityInCAD(const char* strCadName,const char* strSdeName=NULL,
		                     DIST_STRUCT_SELECTWAY* pFilter=NULL);

	//���ܣ�ѡ��ǰ��ѯ��Χ���е�����ʵ��(�����ظ��ύ)
	//������strCadName   CADͼ����
	//      strSdeName   SDEͼ����(���û��SDEͼ��������˵��CAD����SDE���Ƕ��һ��һ��һ)
	//      pFilter      �ռ��������
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long   SelectEntityInCAD(AcDbObjectIdArray& ObjIdArray,const char* strCadName,
		                     const char* strSdeName=NULL,DIST_STRUCT_SELECTWAY* pFilter=NULL);

	//���ܣ�����ͼ���ݿ���Ϣ����CADͼ��
	//������strCadMapSdeTableName SDE��CADͼ����ձ�
	//      strSdeLyName          SDEͼ������
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long   ReadInfoToCreateCadLayer(const char *strCadMapSdeTableName,const char* strSdeName);

	//���ܣ��ж������ַ����Ƿ�ͨ��(���磺abc_*_cd  == abc_sadfsadfsafsdf_cd)
	//������strSub  ���ַ���
	//      strTP   ͨ���ַ���
	//���أ����ֱ����ȷ���2��ͨ�䷵�� 1����ͨ�䷵��0�������ο�����
	long   StringLikeString(CString strSub,CString strTP);


	//���ܣ������Ż�������Ϣ���б�ṹ�б���
	//������strSymbolTableName    ���Ż�������
	//      strSdeLyName          SDEͼ������
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long  ReadSymbolListToSave(const char *strSymbolTableName,const char* strSdeName);

	//���ܣ����õ����ʺ���ɫ���ձ�
	//������strMapTName    ���ձ�����
	//      strSdeLyName   SDEͼ������
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long  ReadYdxzColorMap(const char* strMapTName,const char* strSdeLyName);

private:
	CExchangeInfo   m_info;  //��Ϣ�ṹ
	
};






//===============================================================================================================//
//���ű��ֶζ������£�
// ���  Sdeͼ����    ����       ��ѯ����      �ֶ�1   �ֶ�2   �ֶ�3   �ֶ�4   �ֶ�5   �ֶ�6  �ֶ�7  �ֶ�8 �ֶ�9 �ֶ�10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3    Fld4    Fld5    Fld6    Fld7  Fld8  Fld9   Fld10
class CBaseExchange 
{
public:
	
	//���������
	         CBaseExchange(void);
			 CBaseExchange(CExchangeInfo* pInfo);
	virtual ~CBaseExchange(void);

public:

    //���ܣ���CAD�л���ʵ��
	//������resultId    �������ʵ��ID
	//      pRcdSet     ��ѯ��¼����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//���ܣ���CADʵ�屣�浽SDE����
	//������pRowId,    ����SDE��¼��RowId
	//      pCommand   ���ݿ��������
	//      ObjId      CADʵ��ID
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);

	//���ܣ��жϵ�ǰ����ʵ���Ƿ����Ѿ�����ʵ�������д���
	//������tempObjId   �ж�ʵ��ID
	//���أ� ���ڷ��� 1�������ڷ��� 0 �������ο�����
	//��ע���ú�����Ҫ��Զಿ��SHAPE������һ��SHAPE��Ӧ������� CAD ʵ�壻Ϊ���Ч�ʣ��ύʱ������������һ��
	//      ��ʵ�壬�ͻὫ�������г�Աһ���ύ��Ȼ���������м�¼�����Ѿ��������������ظ�����
	long IsObjectHaveUsed(AcDbObjectId& tempObjId);

public:

	//�����ַ�����Ϣ����ȡ�Ƕ���Ϣ
	//������ dAngle   �Ƕȷ���ֵ���Ƕȣ�
	//       strAngle �ַ���Ϣ
	//���أ���
	//������ʽ���Ƕ�  ��  ����#
	static void ParseAngleInfo(double& dAngle,const char* strAngle);


	//�����ַ�����Ϣ����ȡ��ɫ���
	//������ nColorIndex   ������ɫ���
	//       strColor      �ַ���Ϣ
	//���أ���
	//������ʽ�����  ��  R,G,B#
	static void ParseColorInfo(int& nColorIndex,const char* strColor,DIST_COLORMAPARRAY* pData=NULL);



	//�����ַ�����Ϣ����ȡ�����
	//������ dSx,dSy,dSz   ��X��Y��Z����ı���ϵ��
	//       strScale      �ַ���Ϣ
	//���أ���
	//������ʽ��X������Y������Z���� ��������X����X��Y �� X��Y��Z��
	static void ParseScaleInfo(double& dSx,double& dSy,double& dSz,const char* strScale);


	//�����ַ�����Ϣ����ȡ���ָ߶�
	//������ dHeight       �������ָ߶�
	//       strTextHeight �ַ���Ϣ
	//���أ���
	//������ʽ�����ָ߶� �� ���ָ߶ȣ�����ϵ��#
	static void ParseTextHeight(double& dHeight,const char* strTextHeight);


	//�����ַ�����Ϣ����ȡCAD������ʽ
	//������ strStyle       ����������ʽ
	//       strTextStyle   �ַ���Ϣ
	//���أ���
	//������ʽ��CAD������ʽ  ��  ����#
	static void ParseTextStyle(char *strStyle,const char* strTextStyle);


	//�����ַ�����Ϣ����ȡ������뷽ʽ
	//������ nAlign       ���ض��뷽ʽ��0�����  1���Ķ���  2�Ҷ��룩
	//       strAlign     �ַ���Ϣ
	//���أ���
	//������ʽ�����У���  ��  Left��Mid��Right �� 0��1��2
	static void ParseTextAlign(int& nAlign,const char* strAlign);

	static void ParseLnWidth(double& dLnWidth,const char* strLnWidth);

protected:
	//������������������������������������������������������������//

	//���ܣ�����ʵ��ID����ȡ��չ�����е�RowID����Ϣ
	//������nSymbolPos  ���Ż���Ϣ���������е�λ��
	//      nRowId      ����SDE��¼���
	//      nPartNum    ���ز�������
	//      ObjIdArray  �������Բ��ֶ�ӦEntity��ObjectId
	//      ObjId       CADʵ��ID
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long CheckObjAndReadXDataInInfo(int& nIsExists,int& nSymbolPos,int& nRowId,int& nPartNum,AcDbObjectIdArray& ObjIdArray,AcDbObjectId& ObjId);


	//���ܣ����ڲ����ⲿʹ�õ���Ϣд��ʵ�����չ����
	//������strNameArray      �ڲ��ֶ�����
	//      nPos              �ڷ��Ż���Ϣ�ṹ�еĴ���
	//      strRowId          ʵ����SDE���ж�Ӧ��¼��Ψһ���
	//      strObjArray       �ڲ��ֶζ�Ӧֵ
	//      strOutXDataArray  �ⲿ�ֶζ�Ӧֵ
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	long WriteXDataToObjects(CStringArray& strNameArray,int nPos,const char* strRowId,CStringArray& strObjArray,CStringArray& strOutXDataArray);


	//���ܣ��������ݼ�¼�����ַ�����ʽ�ṩ�޸�ֵ��
	//������pCommand              ִ���������ָ��
	//      strFldNameArray       �޸ļ�¼�ֶ���
	//      strFldValueArray      �޸ļ�¼ֵ
	//      nRowId                ��¼�ı��
	//      nInsertOrUpdate       ���ܱ�ǣ�1 ��ʾ�������룬 2 ��ʾ�޸ĸ��£�
	//���أ���
	long SaveValuesToSDE(IDistCommand* pCommand,CStringArray& strFldNameArray,CStringArray& strFldValueArray,int& nRowId,int nInsertOrUpdate=2);

	//���ܣ��������л�ȡָ�������ֶε�ֵ
	//������strValue          ����ֵ
	//      strName           ָ������
	//      strFldNameArray   ָ����������
	//      strFldValueArray  ָ��ֵ����
	//���أ���
	void GetValueFromArray(CString& strValue,CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray);

	//���ܣ�����ָ�����Ƶ�ֵ��������
	//������strValue          ָ��ֵ
	//      strName           ָ������
	//      strFldNameArray   ָ����������
	//      strFldValueArray  ָ��ֵ����
	//���أ���
    void SetValueToArray(CString strValue,CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray);

	//���ܣ���������ɾ��ָ�����ƺ�ֵ
	//������strName           ָ������
	//      strFldNameArray   ָ����������
	//      strFldValueArray  ָ��ֵ����
	//���أ���
	void DeleteValueFromArray(CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray);

	//���ܣ��Ƚ��������Ƿ����
	//������ptA��ptB    ���Ƚϵ�������ά��
	//      dPrecision  ����Ƚϵľ���
	//���أ���ȷ��� 1������ȷ��� 0�������ο�����
	long IsPointEqual(AcGePoint3d ptA,AcGePoint3d ptB,double dPrecision=0.0001);

protected:

	CExchangeInfo*      m_pInfo;
	AcDbObjectIdArray   m_ObjIdArrayHaveUsed;
};

//===============================================================================================================//
//���ű��ֶζ������£�
// ���  Sdeͼ����    ����       ��ѯ����      �ֶ�1   �ֶ�2   �ֶ�3     �ֶ�4   �ֶ�5   �ֶ�6  �ֶ�7  �ֶ�8 �ֶ�9 �ֶ�10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3      Fld4    Fld5    Fld6    Fld7  Fld8  Fld9   Fld10
//                                             ��ɫ��  ������  ���ű���  ��ת�Ƕ�
class CPointExchange : public CBaseExchange
{
private:
	         CPointExchange(void);
public:
	         CPointExchange(CExchangeInfo* pInfo);
	virtual ~CPointExchange(void);

public:

	//���ܣ���CAD�л���ʵ��
	//������resultId    �������ʵ��ID
	//      pRcdSet     ��ѯ��¼����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//���ܣ���CADʵ�屣�浽SDE����
	//������pRowId,    ����SDE��¼��RowId
	//      pCommand   ���ݿ��������
	//      ObjId      CADʵ��ID
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);


};


//===============================================================================================================//
//���ű��ֶζ������£�
// ���  Sdeͼ����    ����       ��ѯ����      �ֶ�1   �ֶ�2   �ֶ�3  �ֶ�4     �ֶ�5     �ֶ�6  �ֶ�7  �ֶ�8 �ֶ�9 �ֶ�10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3   Fld4      Fld5      Fld6    Fld7  Fld8  Fld9   Fld10
//                                             ��ɫ��  ����    ����   ���ָ߶�  ��ת�Ƕ�  ���뷽ʽ
class CTextExchange : public CBaseExchange
{
private:
	CTextExchange(void);
public:
	         CTextExchange(CExchangeInfo* pInfo);
	virtual ~CTextExchange(void);

public:

	//���ܣ���CAD�л���ʵ��
	//������resultId    �������ʵ��ID
	//      pRcdSet     ��ѯ��¼����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//���ܣ���CADʵ�屣�浽SDE����
	//������pRowId,    ����SDE��¼��RowId
	//      pCommand   ���ݿ��������
	//      ObjId      CADʵ��ID
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);
};


//===============================================================================================================//
//���ű��ֶζ������£�
// ���  Sdeͼ����    ����       ��ѯ����      �ֶ�1   �ֶ�2   �ֶ�3  �ֶ�4     �ֶ�5     �ֶ�6  �ֶ�7  �ֶ�8 �ֶ�9 �ֶ�10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3   Fld4      Fld5      Fld6    Fld7  Fld8  Fld9   Fld10
//                                             ��ɫ��  ����    �߿�   ȫ���߿�  �Ƿ�պ�
class CPolylineExchange : public CBaseExchange
{
private:
	CPolylineExchange(void);
public:
	         CPolylineExchange(CExchangeInfo* pInfo);  
	virtual ~CPolylineExchange(void);

public:

	//���ܣ���CAD�л���ʵ��
	//������resultId    �������ʵ��ID
	//      pRcdSet     ��ѯ��¼����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//���ܣ���CADʵ�屣�浽SDE����
	//������pRowId,    ����SDE��¼��RowId
	//      pCommand   ���ݿ��������
	//      ObjId      CADʵ��ID
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);

};



//===============================================================================================================//
//���ű��ֶζ������£�
// ���  Sdeͼ����    ����       ��ѯ����      �ֶ�1   �ֶ�2   �ֶ�3  �ֶ�4     �ֶ�5     �ֶ�6    �ֶ�7    �ֶ�8    �ֶ�9  �ֶ�10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3   Fld4      Fld5      Fld6     Fld7     Fld8     Fld9   Fld10
//                                            ��ɫ��  ����     �߿�   ȫ���߿�  ���ɫ��  ͼ������ ͼ������ ͼ���Ƕ�
class CPolygonExchange : public CBaseExchange
{
private:
	CPolygonExchange(void);
public:
	         CPolygonExchange(CExchangeInfo* pInfo);
	virtual ~CPolygonExchange(void);

public:

	//���ܣ���CAD�л���ʵ��
	//������resultId    �������ʵ��ID
	//      pRcdSet     ��ѯ��¼����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//���ܣ���CADʵ�屣�浽SDE����
	//������pRowId,    ����SDE��¼��RowId
	//      pCommand   ���ݿ��������
	//      ObjId      CADʵ��ID
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);

private:
	long GetEntityHatch(AcDbObjectId& resultId,AcDbEntity* pEnt);

};


//===============================================================================================================//
// ���Ż����¼�ֶκ������£�
// ���    SDEͼ����   SDE����     ��ѯ����       �ֶ�1   �ֶ�2   �ֶ�3     �ֶ�4    �ֶ�5   �ֶ�6  �ֶ�7  �ֶ�8 �ֶ�9  �ֶ�10 
// Id      SdeLyName   SdeLyKind   SqlCondition   ��       ��      ��        ��       ��      ��     ��     ��	      
class CMultpatchExchange : public CBaseExchange
{
private:
	CMultpatchExchange(void);
public:
	         CMultpatchExchange(CExchangeInfo* pInfo);
	virtual ~CMultpatchExchange(void);

public:

	//���ܣ���CAD�л���ʵ��
	//������resultId    �������ʵ��ID
	//      pRcdSet     ��ѯ��¼����
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//���ܣ���CADʵ�屣�浽SDE����
	//������pRowId,    ����SDE��¼��RowId
	//      pCommand   ���ݿ��������
	//      ObjId      CADʵ��ID
	//���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);
};




//===============================================================================================================//
struct DIST_SYMBOLFLD
{
public:
	char m_strSqlCondition[2048];   //���Ż�SQL����
	char m_strFld[10][60];          //��̬�ֶ����飨10���ַ����������ָ���ֶ���������ֵ
	int  m_nPosArray[10];           //��ʾ��ָ���ֶ��л�ȡ��λ������
	int  m_nNum;                    //�������
	bool m_bIsDefault;              //�Ƿ�ΪĬ��Default����

	DIST_SYMBOLFLD()  //��ʼ������
	{
		memset(m_strSqlCondition,0,sizeof(char)*2048);
		for(int i=0; i<10; i++){
			memset(m_strFld[i],0,sizeof(char)*60);
			m_nPosArray[i] = -1;
		}
		m_nNum = 0;
		m_bIsDefault = false;
	}
};

struct DIST_FILLCOLORMAP
{
	CString  m_strDKYDXZ;
	int      m_nClrIndex;
	DIST_FILLCOLORMAP()
	{
		m_strDKYDXZ = "";
		m_nClrIndex = 256;
	}
};

class CSplitStr
{
private:
    //�зֵı�־����
    CString m_sSplitFlag;
    //�������зֵı�־���ŵ���һ����־����
    BOOL m_bSequenceAsOne;
    //���зֵ��ı�
    CString m_sData;
public:
    //�õ��зֺõ��ı���
    void GetSplitStrArray(CStringArray &array);
    //�õ����зֵ��ı�
    CString GetData();
    //���ñ��зֵ��ı�
    void SetData(CString sData);
    //�õ��зֲ���
    BOOL GetSequenceAsOne() {return m_bSequenceAsOne;};
    //�����зֲ���
    void SetSequenceAsOne(BOOL bSequenceAsOne) {m_bSequenceAsOne = bSequenceAsOne;};
    //�õ��зֱ�־
    CString GetSplitFlag() {return m_sSplitFlag;};
    //�����зֱ�־
    void SetSplitFlag(CString sSplitFlag) {m_sSplitFlag = sSplitFlag;};
    CSplitStr();
    virtual ~CSplitStr();
};

#endif // __CBASESDEENTITY__H__

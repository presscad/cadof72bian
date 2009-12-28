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

#ifndef __DIST_ENTITY_TOOL_H__
#define __DIST_ENTITY_TOOL_H__
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ļ�˵������������
//����˵����1.�Թ���Ϊ���������������ֵ���ͬ���ӹ�����
//          2.��������ʵ�־�����������ݣ����⺯�����ε��ã��������δ��ݣ����ͺ���֮������
//          3.�����������a> ��������ֵΪlong�ͣ�1�ɹ���0ʧ�ܣ�����ֵ�ο������壩
//                          b> ���ز������ں��������Ŀ�ʼλ��,Ȼ��ű������������Ĭ�ϲ���
//                          c> �����ϸ�������������������ͬ�ຯ����ͬ�������������λ�ô�������ƾ���һ��
//                          d> ��������Ҫ��׼ȷ����ݣ���������
//          4.�����д���Ҫһ�£���Ҫ����ν�ġ����¡�
//
//���ߣ����� <wanghui@dist.com.cn>
//���ڣ�2007��4��,�Ϻ�.
//�޸ģ� 

#include "StdAfx.h"

#define  CURDBPOINTER acdbHostApplicationServices()->workingDatabase()

class CDistEntityTool
{
public:
	               CDistEntityTool(void);
	virtual       ~CDistEntityTool(void);

public:

	//���ܣ�����ͼ��
	//������resultId    ���ش���ʵ���ID
	//      strLyName   ͼ����
	//      nClrIndex   ��ɫ��
	//      strLnType   ����
	//      dLineWeigth �߿�
	//      pDB         ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreateLayer(AcDbObjectId& resultId,const char *strLyName, int  nClrIndex = 7,const char *strLnType = "Continuous",
		             AcDb::LineWeight dLineWeigth = AcDb::kLnWtByLwDefault, AcDbDatabase* pDB = CURDBPOINTER);

	//���ܣ�����CAD��ǰ����ͼ��
	//������strLayerName   ͼ����
	//      pDB            ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long SetCurLayer(const char* strLayerName, AcDbDatabase* pDB= CURDBPOINTER);


	long SelectEntityInLayer(AcDbObjectIdArray& IdArray,const char *strLyName);

	//���ܣ�ɾ��ָ��ͼ���е�����ʵ��
	//������strLyName   ͼ����
	//      pDB            ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long DeleteAllEntityInLayer(char *strLyName,AcDbDatabase* pDB= CURDBPOINTER);


	//���ܣ����������ı�
	//������resultId       ���ش���ʵ���ID
	//      ptAlign        ���ֶ��������
	//      strText        �ı�����
	//      strLyName      ͼ����
	//      nClrIndex      ��ɫ��
	//      strFontType    ����
	//      nHorzAlignType ���ֺ�����뷽ʽ(0:�����,1:���ж���,2:�Ҷ���)
	//      nVertAlignType ����������뷽ʽ(0:�¶���,1:���ж���,2:�϶���)
	//      dHeight        ���ָ߶�
	//      dWidFactor     ���ֿ�߱�
	//      dAngle         ��ת�Ƕ�(����)
	//      pDB            ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreateText(AcDbObjectId& resultId,AcGePoint3d& ptAlign,char* strText,char* strLyName="0",int  nClrIndex=0,char* strFontType="Standard",
		            int nHorzAlignType=1,int nVertAlignType=0,double dHeight=3.0,double dWidFactor=0.8,double dAngle=0.0, AcDbDatabase* pDB = CURDBPOINTER);



	//���ܣ����������ı�
	//������resultId     ���ش���ʵ���ID
	//      pt           �ı������
	//      strText      �ı�����
	//      strLyName    ͼ����
	//      nClrIndex    ��ɫ��
	//      strFontType  ����
	//      nAlignType   ���뷽ʽ(0:�����,1:���ж���,2:�Ҷ���)
	//      dHeight      ���ָ߶�
	//      dAngle       ��ת�Ƕ�(����)
	//      pDB          ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreateMText(AcDbObjectId& resultId,AcGePoint3d& pt,char* strText,char* strLyName="0",int  nClrIndex=0, 
		             char* strFontType="Standard",int nAlignType=0,double dHeight=3.0,double dAngle=0.0,AcDbDatabase* pDB = CURDBPOINTER);

	//���ܣ�����CAD��ǰ������ʽ
	//������strTextStyleName  ������
	//      pDB               ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long SetCurTextStyle(const char* strTextStyleName, AcDbDatabase* pDB = CURDBPOINTER);

	//���ܣ�����������
	//������resultId      ���ش���ʵ���ID
	//      pt            �����ò����
	//      strBlockName  ������
	//      strLyName     ͼ����
	//      nClrIndex     ��ɫ��
	//      strScale      ���ű���(x,y,z)�ο���ʽ xxx#yyy#zzz#
	//      dAngle        ��ת�Ƕ�(����)
	//      pDB           ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreateBlockReference(AcDbObjectId& resultId,AcGePoint3d& pt,const char *strBlockName,const char *strLyName = "0",
		                     int  nClrIndex = 0,char *strScale ="1.0#1.0#1.0#",double dAngle = 0.0,AcDbDatabase* pDB = CURDBPOINTER); 

	//���ܣ�������
	//������resultId      ���ش���ʵ���ID
	//      pt            ������
	//      strLyName     ͼ����
	//      nClrIndex     ��ɫ��
	//      pDB           ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreatePoint(AcDbObjectId& resultId,AcGePoint3d& pt,const char *strLyName="0",int nClrIndex=0,AcDbDatabase *pDB = CURDBPOINTER);


	//���ܣ������߶�
	//������resultId    ���ش���ʵ���ID
	//      ptS,ptE     �߶ε������յ�
	//      strLyName   ͼ����
	//      nClrIndex   ��ɫ��
	//      strLnType   ����
	//      dLineWeigth �߿�
	//      dThickness  ���
	//      pDB         ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreateLine(AcDbObjectId& resultId,AcGePoint3d& ptS,AcGePoint3d& ptE,const char* strLyName = "0",int nClrIndex =0,
	                const char* strLnType = "Continuous",AcDb::LineWeight dLineWeigth = AcDb::kLnWtByLwDefault, 
					double dThickness = 0.0,AcDbDatabase* pDB = CURDBPOINTER);


	//���ܣ�������ά�����
	//������resultId      ���ش���ʵ���ID
	//      ptArray       ����ߵĵ㼯
	//      strLyName     ͼ����
	//      nClrIndex     ��ɫ��
	//      strLnType     ����
	//      dLineWeigth   �߿�
	//      dGlobeWeigth  ȫ�ֿ��
	//      bIsClosed,    �Ƿ�պ�
	//      dThickness    ���
	//      pDB           ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreatePoly2dline(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strLyName="0",int  nClrIndex=0,               
						  const char* strLnType="Continuous",AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,   
						  double dGlobeWeigth=0.0,bool   bIsClosed=false,double dThickness = 0.0,AcDbDatabase* pDB=CURDBPOINTER);


	//���ܣ����������
	//������resultId      ���ش���ʵ���ID
	//      ptArray       ����ߵĵ㼯
	//      strLyName     ͼ����
	//      nClrIndex     ��ɫ��
	//      strLnType     ����
	//      dLineWeigth   �߿�
	//      dGlobeWeigth  ȫ�ֿ��
	//      bIsClosed,    �Ƿ�պ�
	//      dThickness    ���
	//      pDB           ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreatePolyline(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strLyName="0",int  nClrIndex=0,               
		                const char* strLnType="Continuous",AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,   
		                double dGlobeWeigth=0.0,bool   bIsClosed=false,double dThickness = 0.0,AcDbDatabase* pDB=CURDBPOINTER);

	long CreatePolylineX(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strLyName="0",int  nClrIndex=0,               
		                const char* strLnType="Continuous",AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,   
		                double dGlobeWeigth=0.0,bool   bIsClosed=false,double dThickness = 0.0,AcDbDatabase* pDB=CURDBPOINTER);
 

	//���ܣ�����Բ
	//������resultId    ���ش���ʵ���ID
	//      ptCen       ���ĵ�
	//      dRadius     �뾶
	//      strLyName   ͼ����
	//      nClrIndex   ��ɫ��
	//      strLnType   ����
	//      dLineWeigth �߿�
	//      dThickness  ���
	//      pDB         ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreateCircle(AcDbObjectId& resultId,AcGePoint3d& ptCen,double dRadius,const char *strLyName="0",int  nClrIndex=0,
		              const char* strLnType="Continuous",AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,double dThickness = 0.0,AcDbDatabase* pDB = CURDBPOINTER);


	//���ܣ�����Բ��
	//������resultId    ���ش���ʵ���ID
	//      ptCen       ���ĵ�
	//      dRadius     �뾶
	//      dAngleStart ���Ƕ�(����)
	//      dAngleEnd   �յ�Ƕ�(����)
	//      strLyName   ͼ����
	//      nClrIndex   ��ɫ��
	//      strLnType   ����
	//      dLineWeigth �߿�
	//      dThickness  ���
	//      pDB         ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreateArc(AcDbObjectId& resultId,AcGePoint3d& ptCen,double dRadius,double dAngleStart,double dAngleEnd,
		           const char *strLyName="0",int  nClrIndex=0,const char* strLnType="Continuous",
				   AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,double dThickness = 0.0,AcDbDatabase* pDB = CURDBPOINTER);


	//���ܣ����ɶ�������(�޹����߽�)
	//������resultId      ���ش���ʵ���ID
	//      ptArray       ����������
	//      strHatchName  �����ʽ����
	//      dScale        ���ͼ������
	//      dAngle        ���ͼ����ת�Ƕ�
	//      strLyName     ͼ������
	//      nClrIndex     ��ɫ��
	//      pDB           ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreatePolyHatch(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strHatchName,double dScale=1.0,
		                 double dAngle=0,const char *strLyName="0",int  nClrIndex=0,AcDbDatabase* pDB = CURDBPOINTER);

	
	//���ܣ����ɶ�������(�����߽�)
	//������resultId      ���ش���ʵ���ID
	//      dbObjIds      ����ʵ��Id����
	//      strHatchName  �����ʽ����
	//      dScale        ���ͼ������
	//      dAngle        ���ͼ����ת�Ƕ�
	//      strLyName     ͼ������
	//      nClrIndex     ��ɫ��
	//      pDB           ���ݿ����ָ��
	//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
	long CreatePolyHatch(AcDbObjectId& resultId,AcDbObjectIdArray& dbObjIds,const char *strHatchName,double dScale=1.0,
		                 double dAngle=0,const char *strLyName="0",int  nClrIndex=0,AcDbDatabase* pDB= CURDBPOINTER);


public:

	//��ȡ��ǰDWG�ļ�������ͼ����
	long  GetAllLayerName(CStringArray& layerArray,AcDbDatabase* pDB = CURDBPOINTER);
	long  GetAllLayerName(CStringArray& layerArray,CStringArray& clrIndexArray,AcDbDatabase* pDB= CURDBPOINTER);

	//��дʵ�����չ����
	void SetXData(AcDbEntity* pEnt,CStringArray& strValue,const char* strRegName="DIST_XDATA");
	void GetXData(AcDbEntity* pEnt,CStringArray& strValue,const char* strRegName="DIST_XDATA");

public:
	//��������

	// ����CADʵ������
	long GetLineTypeId(AcDbObjectId& resultId,const char* strLnTypeName,AcDbDatabase* pDB = CURDBPOINTER);

	// ����CADʵ������
	long LoadLineType(AcDbObjectId& resultId,const char* strLnTypeName,AcDbDatabase* pDB = CURDBPOINTER);

	// �߿�����ת��λDouble
	double LnWeightToDouble(AcDb::LineWeight type);
	AcDb::LineWeight DoubleToLnWeight(double dLnWid);

	//����������ʽ��������ID
	long CreateTextStyle(AcDbObjectId& resultId,const char* csTsName,const char* szFlNm,const char* szBigFlNm,AcDbDatabase* pDB = CURDBPOINTER);

	//��ȡ������ʽID
	long GetTextStyleObjID(AcDbObjectId& resultId,const char* strTextStyleName,AcDbDatabase* pDB = CURDBPOINTER);

	//����������ʽID�õ���������
	long GetTextStyleName(char *strTextStyleName,AcDbObjectId& ObjId,AcDbDatabase *pDB = CURDBPOINTER);

	// ���ʵ�嵽ģ�Ϳռ�
	long AddToCurrentSpace(AcDbObjectId& resultId,AcDbEntity* pEnt,AcDbDatabase* pDB = CURDBPOINTER);

	//
	long GetPoly2dlinePts(AcDbObjectId poly2dlineId,AcGePoint3dArray& ptArray);
	long GetPoly2dlinePts(AcDb2dPolyline* pPolyline2d,AcGePoint3dArray& ptArray);

	long GetPolylinePts(AcDbObjectId polylineID,AcGePoint3dArray& ptArray);
	long GetPolylinePts(AcDbPolyline* pPolyline,AcGePoint3dArray& ptArray);

	long GetPoly3dlinePts(AcDbObjectId poly3dlineId,AcGePoint3dArray& ptArray);
	long GetPoly3dlinePts(AcDb3dPolyline* pPolyline3d,AcGePoint3dArray& ptArray);

	long GetSplinePts(AcGePoint3dArray& ptArray,AcDbSpline* pSpline,int nSegPartNum=10);
	long GetEllipsPts(AcGePoint3dArray& ptArray,AcDbEllipse* pE,int nSegPartNum=10);
	long GetArcPts(AcGePoint3dArray& ptArray,AcDbArc* pArc,int nSegPartNum=10);
	long GetCirclePts(AcGePoint3dArray& ptArray,AcDbCircle* pCircle,int nSegPartNum=10);

	void TransformToWcs(AcDbEntity* ent, AcDbDatabase* pDB);

	long DyZoom(AcGePoint2d& CenterPt,double dHeight,double dWidth);

	//�õ��ı��߽�
	long GetTextBoundary(AcGePoint3dArray& ptArray,AcDbObjectId& objectId,double dOffset=0);

public:
	AcGeVector3d Convert2dVectorTo3d(AcGeVector2d pt);
	AcGeVector2d Convert3dVectorTo2d(AcGeVector3d pt);
	AcGePoint3d Convert2dPointTo3d(AcGePoint2d pt);
	AcGePoint2d Convert3dPointTo2d(AcGePoint3d pt);
	void ReduceBulge(AcGePoint3dArray &ptDfArr,AcGePoint3d ptA,AcGePoint3d ptB,double dBulge,int nPartNum=10);

	//��֪һ�λ��������յ��Լ���͹��,����Բ��,�ͻ���
	long GetArcCenterPt(ads_point startPoint,ads_point endPoint,double bulge,ads_point& center,double& dArcLen);
	
	void PolylinePtUniteArc(AcDbObjectId objId, int Step);

	////////////////////////////////////////////////////////////////////////////////////////////////
	//�������ܣ����߶�BL,�߶�BR�ļнǣ���ȷ��BL��BR�Ǹ��߶�����࣬�Ǹ��߶����Ҳ�
	//��ڲ�����
	//          bx,by,lx,ly    �߶�BL 
	//          .. ..,rx,ry    �߶�BR
	//���ڲ�����
	//          dAngle         �нǴ�С(����)
	//����ֵ��-1:˳ʱ��   0 ����   1 ��ʱ��
	////////////////////////////////////////////////////////////////////////////////////////////////
	long AngleWithTowLineSect(double lx,double ly,double bx,double by,double rx,double ry,double& dAngle);

	long SimulationArc(AcGePoint3dArray& ptArray,AcGeDoubleArray& resultBlgArray);

	void OpenDwgFile(CString strFileName);

	void SendCommandToAutoCAD(CString cmd);

	void ShowCommandLineWindow(BOOL bShow = TRUE);

	BOOL OpenOrCloseLayer(BOOL bOpen, CStringArray &strCadLyNameArray);
	BOOL InsertBlkAttr(char* strBlkName,CStringArray& strAttrArray,char* strTitleName,int nColorIndex);
	BOOL InsertBlkAttrRef(AcDbObjectId& resultId,AcGePoint3d pt,char* strBlkName,char* strCadLyName,
		                  double dFontHeight,CStringArray& strTagArray,CStringArray& strTextArray);

	BOOL CreateBlkRefAttr(AcDbObjectId& resultId,AcGePoint3d basePt,char* strBlkName,char* strLyName,
		                  int  nClrIndex,char *strScale,double dAngle,//double dFontHeight,
		                  CStringArray& strTagArray,CStringArray& strTextArray,AcDbDatabase* pDB = CURDBPOINTER);

	AcDbViewTableRecord GetCurrentView();
	AcDbObjectId CreateBlockByDwgFile(CString strDwgFileName);

private:
	static void OpenDocHelper(void *pData);
};


#endif  //__DIST_ENTITY_TOOL_H__
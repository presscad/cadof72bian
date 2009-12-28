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

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//������  ��ʵ�ֶ�ͼ�εĶ���βü�
//ʵ��˼·��1.���ݶ�����������������ʵ�弯�ϣ���������A��
//          2.�ٸ��ݶ���������������ཻ��ʵ�弯�ϣ���������B��
//          3.����ѭ������B
//            a.������B��ȡһʵ��
//            b.����ת����AcDbCurve����
//            c.�������󽻣��õ�һ��������AcDbCurve
//            d.�ж�ÿһ����AcDbCurve�Ƿ��ڶ�����ڣ����ڶ�����ڵ���ʵ����ӵ�ģ�Ϳռ䣬
//              ����ObjectId��ӵ�����A�У��ظ�a,b,c,dֱ������������ʵ��
//          4.ɾ������B�е�ʵ��
//ʹ��˵����
//         CDistClipEntity::ReduceEntities(SelectPtArray,"LyName"); 
//��ע���ڲü�ʱ��ע���ӿڷ�Χ��������������ʵ��
//�޸ļ�¼��
//    ���ڿ�ʼ��ͼ����ֱ�ӻ��Ƶ�,��ʽ�ޱ仯,�����ԱȽϴ�;������Ϊʹ�ò������Կ�ķ�ʽ(�������Կ�ʱ����λ����Ҫ����)
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __DISTREDUCE__ENTITY__H__
#define  __DISTREDUCE__ENTITY__H__

#include "StdAfx.h"

typedef CArray<AcGePoint3dArray,AcGePoint3dArray&> RECT_ACGE_ARRAY;

class CDistClipEntity
{
public:
	         CDistClipEntity(void);
	virtual ~CDistClipEntity(void);

public:

	//���ܣ���������
	//��������
	//���أ���
	static BOOL Plot();

	static BOOL PlotEx();

	static BOOL PlotByPaperSpace();

	static BOOL GetBlockNameByBlkId(CString &strBlkName,AcDbObjectId blockId);
	static BOOL FindPaperSpaceAndInsertBlkRefInfo(AcDbObjectIdArray& ObjIdArray);
	static BOOL FindPaperSpaceBlkId(AcDbObjectIdArray& ObjIdArray);

	static BOOL SaveToDwgFile(CString strFName,AcDbObjectIdArray& ObjIdArray);
	static BOOL SaveToDwgFile(CString strPrjName,AcGePoint3d& ptCen,double dH,double dW);

	static BOOL DrawCustomGrid(AcGePoint3dArray& ptArray,AcDbObjectIdArray& ObjIdArray,char* strLyName,double dEdge,double dCross,double dCrossDist,CStringArray& strValueArray);
	//���ܣ����ݱ߽����ζ�ʵ����вü�����
	//������ObjIdArrayReturn ���زü���Χ�ڵ�ʵ��ID
	//      SelectPtArray    �߽�����(������һ����Ϊ�պϣ�������պϻ�����һ���׵㣻������Ҫֵ����)
	//      strLyName        ����ͼ�����ƣ����ΪNULL��ʾ��������ͼ��
	//���أ��ɹ����� TRUE ʧ�ܷ��� FALSE
	static BOOL ClipAndCopy(AcDbObjectIdArray& ObjIdArrayReturn,const char* strLyName,AcGePoint3dArray SelectPtArray,const char* strLyNameSelect=NULL);
	static void CloneBlockRefAttr(AcDbObjectIdArray& newObjIdArray,const char* strLyName,AcDbObjectIdArray oldObjIdArray);


	//����:ʵ�ֶ�����������,������������������е�����
	static void PolyIntersectWithRect(RECT_ACGE_ARRAY& resultArray,AcGePoint3dArray& polyPtArray,AcGePoint3dArray& rectPtArray);

	static void SelectDkAndHighLight();

	static void ReadDwgCT();

private:
	static void CalcRectByScale(double dScaleX,double dScaleY,AcGePoint3d basePt,AcGePoint3dArray& pts);
	static BOOL CreateGroup(AcDbObjectId& groupId,AcDbObjectIdArray& objIds,char* strGroupName);
	static BOOL GetBckRefInf(const AcDbObjectId & BckRefId, CStringArray &strTags, CStringArray &strContents);
	static BOOL GetBlockNameByBlkRefId(CString &strName,AcDbObjectId objId);
	static void GetProjectInfo(CStringArray& strValueArray,AcGePoint3dArray& SelectPtArray);
	
	static CString GetCurArxAppPath(CString strArxName);
	static void CreateBlockByFile();
	static void CloneEntity(AcDbObjectIdArray& newObjIdArray,const char* strLyName,AcDbObjectIdArray oldObjIdArray,AcDbObjectId* pOwnerObjId=NULL);
	//���ܣ��ж����߶��Ƿ��ཻ
	//������px1,py1,px2,py2   �߶�P1P2
	//      qx1,qy1,qx2,qy2   �߶�Q1Q2
	//���أ�����ཻ����TRUE ���򷵻�FALSE ��
	static BOOL IsLineSectIntersectLineSect(double px1,double py1,double px2,double py2,
		                             double qx1,double qy1,double qx2,double qy2);

	//���ܣ��жϵ��Ƿ����߶���
	//������px1,py1,px2,py2  �߶�P1P2
	//      qx,qy            ��Q
	//���أ�����������Ϸ��� TRUE  ���򷵻�FALSE ��
	static BOOL IsPointOnLine(double px1,double py1,double px2,double py2,double qx,double qy);


	//���ܣ��жϵ��Ƿ��ڶ������
	//������pts  ����ε㼯��
	//      x,y  ������
	//���أ�������ڶ�����ڷ��� TRUE ���򷵻� FALSE       
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	static BOOL  IsPointInPolygon(AcGePoint3dArray& pts,double x,double y);

    //���ܣ�����ǰʵ����ӵ�ģ�Ϳռ�
	//������resultId  �������ʵ���ID
	//      pEnt      ʵ��ָ��
	//      pDB       ��ǰ�ĵ����ݿ�ָ��
	//���أ����ʵ��ɹ����� TRUE ���򷵻� FALSE  
	static BOOL AddToCurrentSpace(AcDbObjectId& resultId,AcDbEntity* pEnt,AcDbDatabase* pDB=acdbHostApplicationServices()->workingDatabase());


	static AcDbObjectId FindBlkRef(CString strBlkName);
	//static BOOL UpdateAttrToBlkRef(AcDbObjectId ObjIdRef,CStringArray& strArrayTag,CStringArray& strArrayValue);
	static BOOL UpdateAttrToBlkRef(AcDbObjectId ObjIdRef,CStringArray& strArrayTag,CStringArray& strArrayValue,
		                           CStringArray& strArrayA1,CStringArray& strArrayA2,CStringArray& strArrayA3,CStringArray& strArrayA4);
	static BOOL ReadAttrFromRef(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen);
	//static BOOL ReadAttrFromRefDk(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen);
	static BOOL ReadAttrFromRefDk(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen,
		                          CStringArray& strArrayA1,CStringArray& strArrayA2,CStringArray& strArrayA3,CStringArray&strArrayA4);
};


#endif  //__DISTREDUCE__ENTITY__H__


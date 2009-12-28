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
//         CDistReduceEntity::ReduceEntities(SelectPtArray,"LyName"); 
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __DISTREDUCE__ENTITY__H__
#define  __DISTREDUCE__ENTITY__H__

#include "StdAfx.h"

class CDistReduceEntity
{
public:
	         CDistReduceEntity(void);
	virtual ~CDistReduceEntity(void);

public:


#ifdef _DEBUG
	//���ܣ���������
	//��������
	//���أ���
	static BOOL Test();
#endif

	
	//���ܣ����ݱ߽����ζ�ʵ����вü�
	//������SelectPtArray  �߽�����(������һ����Ϊ�պϣ�������պϻ�����һ���׵㣻������Ҫֵ����)
	//      strLyName      ����ͼ�����ƣ����ΪNULL��ʾ��������ͼ��
	//���أ��ɹ����� TRUE ʧ�ܷ��� FALSE
	static BOOL ReduceEntities(AcGePoint3dArray SelectPtArray,const char* strLyName=NULL);

	static BOOL ReduceEntitiesRect(AcGePoint3dArray SelectPtArray,const char* strLyName=NULL);

	
private:

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

	static void ArxZoom(ads_point pt,double magnf);
};


#endif  //__DISTREDUCE__ENTITY__H__


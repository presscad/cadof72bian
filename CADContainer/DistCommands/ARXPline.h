#if !defined(ARXPLINE_H__AB0016C8_6CB4_493C_BB6B_B6EA818D81A9__INCLUDED_)
#define      ARXPLINE_H__AB0016C8_6CB4_493C_BB6B_B6EA818D81A9__INCLUDED_
/////////////////////////////////////////////
//
//������Դ:R
/////////////////////////////////////////////
//#include "stdarx.h"

class ARXPline : public AcEdJig
{
public:
	ARXPline();
	~ARXPline();
    
	bool draw();
	void SetColorIndex(Adesk::UInt16);
	AcDbObjectId getObjectId();
    virtual DragStatus sampler();
    virtual Adesk::Boolean update();
    virtual AcDbEntity* entity() const;
	
	AcGePoint3d m_pt;
	
private:
    AcGePoint3d m_refPoint, m_LastPoint;
	AcDbPolyline * m_pEnt;//��ʱ������ʵ��
	AcDbPolyline *ppline;//Ҫ��ӵ����ݿ��е�ʵ��
	int  addflag;//��ʱ����Ŀ�ı��
	double bulge;//���ε�͹��(��ʱһֱΪ0)
	double last_bulge;//��ʱ��¼��������͹��
	double startWidth,endWidth;//��㡢�յ���߿�
	int drawflag;//��ǻ��Ļ��Ʒ�ʽ
	bool arcflag;//�µ�һ�����͵ı�ǣ�TRUEΪ����FALSEΪֱ�ߡ�
	bool ptflag;//���������ĵ��Ƿ���Ч����Ҫ���ڻ����ж��Ƿ��ܹ��ɻ�.
	AcGePoint2d second_pt;//��¼�������뻡��ʽ�еĵڶ���
	bool CEnter_flag;//Բ�ķ�ʽ����ı��
	bool Radius_flag;
	bool Angle_flag;
	bool Length_flag;
	double radius;
	double qiexian_jiaodu;
	AcGePoint2d center;
public:
	int numVerts;//������ɵ�ʵ��Ķ�����Ŀ
	AcDbObjectId objId;
	Adesk::UInt16 ColorIndex;//�ߵ���ɫ������
};



/////////////////////////////////////////////////////////
#endif
#if !defined(ARXPLINE_H__AB0016C8_6CB4_493C_BB6B_B6EA818D81A9__INCLUDED_)
#define      ARXPLINE_H__AB0016C8_6CB4_493C_BB6B_B6EA818D81A9__INCLUDED_
/////////////////////////////////////////////
//
//代码来源:R
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
	AcDbPolyline * m_pEnt;//临时创建的实体
	AcDbPolyline *ppline;//要添加到数据库中的实体
	int  addflag;//临时点数目的标记
	double bulge;//弧段的凸度(暂时一直为0)
	double last_bulge;//临时记录弧段起点的凸度
	double startWidth,endWidth;//起点、终点的线宽
	int drawflag;//标记弧的绘制方式
	bool arcflag;//新的一段线型的标记，TRUE为弧，FALSE为直线。
	bool ptflag;//标记新输入的点是否有效，主要用于弧，判断是否不能构成弧.
	AcGePoint2d second_pt;//记录三点输入弧方式中的第二点
	bool CEnter_flag;//圆心方式输入的标记
	bool Radius_flag;
	bool Angle_flag;
	bool Length_flag;
	double radius;
	double qiexian_jiaodu;
	AcGePoint2d center;
public:
	int numVerts;//最后生成的实体的顶点数目
	AcDbObjectId objId;
	Adesk::UInt16 ColorIndex;//线的颜色索引号
};



/////////////////////////////////////////////////////////
#endif
#pragma once

class CDkDataCheck
{
public:
	         CDkDataCheck(void);
	virtual ~CDkDataCheck(void);

public:
	BOOL CheckEntity(AcDbObjectId ObjId);
	BOOL DelRedundancyPointFromPolyline(AcDbObjectId ObjId);
	int TwoPolygonIsIntersect(AcDbObjectId RegIdA,AcDbObjectId RegIdB);

private:
	void FlashEntity(AcDbObjectId ObjId);
	void DrawErrorTag(AcGePoint3d cenPt,double dv);
};

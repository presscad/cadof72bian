#include "stdafx.h"
#include "dkdatacheck.h"
#include "DistEntityTool.h"

#define PRECISION 0.00001

CDkDataCheck::CDkDataCheck(void)
{
}

CDkDataCheck::~CDkDataCheck(void)
{
}

void CDkDataCheck::DrawErrorTag(AcGePoint3d cenPt,double dv)
{
	AcDbObjectId resultId;
	CDistEntityTool tempTool;
	AcGePoint3d ptS,ptE;
	ptS.x = cenPt.x - dv; ptS.y = cenPt.y - dv;
	ptE.x = cenPt.x + dv; ptE.y = cenPt.y + dv;
	tempTool.CreateLine(resultId,ptS,ptE,"0",1);

	ptS.x = cenPt.x - dv; ptS.y = cenPt.y + dv;
	ptE.x = cenPt.x + dv; ptE.y = cenPt.y - dv;
	tempTool.CreateLine(resultId,ptS,ptE,"0",1);
}


void CDkDataCheck::FlashEntity(AcDbObjectId ObjId) 
{ 
	AcDbEntity *pEnt=NULL; 
	AcTransaction* pTrans=actrTransactionManager->startTransaction();
	if(Acad::eOk != acdbOpenAcDbEntity(pEnt,ObjId,AcDb::kForWrite))
	{
		actrTransactionManager->endTransaction();
		return; 
	}
	pEnt->highlight(); 
	acedUpdateDisplay();


	DWORD dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->unhighlight(); 
	acedUpdateDisplay();

	dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->highlight(); 
	acedUpdateDisplay();
	dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->unhighlight(); 
	acedUpdateDisplay();

	dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->highlight(); 
	acedUpdateDisplay();
	dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=200) break;      
	}
	pEnt->unhighlight(); 
	acedUpdateDisplay();

	pEnt->close();

	actrTransactionManager->endTransaction();
}


BOOL CDkDataCheck::CheckEntity(AcDbObjectId ObjId)
{
	AcDbEntity* pEnt = NULL;
	if(Acad::eOk != acdbOpenObject(pEnt,ObjId,AcDb::kForRead))
	{
		AfxMessageBox("\n打开地块实体失败！"); return FALSE;
	}
	if(pEnt->isKindOf(AcDbPolyline::desc())==false)
	{
		AfxMessageBox("地块不是 PL 线，地块实体类型不正确！"); pEnt->close(); 
		FlashEntity(ObjId);
		return FALSE;
	}
	AcDbPolyline* pLine = (AcDbPolyline*)pEnt;

	if(pLine->isClosed()!=Adesk::kTrue)
	{
		AfxMessageBox("地块线不闭合！"); pLine->close(); 
		FlashEntity(ObjId); 
		return FALSE;
	}

	AcGeDoubleArray BlgArray;
	AcGePoint3dArray ptArray;
	CDistEntityTool tempTool;
	tempTool.GetPolylinePts(pLine,ptArray,BlgArray);
	int nCount = ptArray.length();
	for(int i=1; i<nCount-1; i++)
	{
		AcGePoint3d tempPtA = ptArray.at(i);
		for(int j=0; j<i; j++)
		{
			AcGePoint3d tempPtB = ptArray.at(j);
			if(tempPtA.distanceTo(tempPtB)<0.0001)
			{
				DrawErrorTag(tempPtA,20);
				AfxMessageBox("\n地块实体有重复点存在！"); pLine->close(); 
				FlashEntity(ObjId); return FALSE;
			}
		}
	}

	//不能构面的闭合曲线是自相交的
	AcDbVoidPtrArray curveSegments;
	curveSegments.append(pLine);
	AcDbVoidPtrArray regions;
	BOOL bCrossSelfOk = (AcDbRegion::createFromCurves(curveSegments, regions) == Acad::eOk);
	int nNum = regions.length();
	for(int k=0; k<nNum; k++)
		delete (AcDbRegion *)regions.at(k); 
	pEnt->close();
	if(!bCrossSelfOk){
		AfxMessageBox("\n地块实体自身相交或有重叠线！");
		FlashEntity(ObjId);
		return FALSE;
	}
	return TRUE;
}



BOOL CDkDataCheck::DelRedundancyPointFromPolyline(AcDbObjectId ObjId)
{
	AcDbPolyline* pLine = NULL;
	if(Acad::eOk != acdbOpenObject(pLine, ObjId, AcDb::kForWrite, Adesk::kFalse)) return FALSE;
	int nNumPt = pLine->numVerts();
	if(nNumPt < 3){
		pLine->close(); return TRUE;
	}

	for(int i=nNumPt-1; i>0; i--)
	{
		AcGePoint3d ptA,ptB;
		pLine->getPointAt(i,ptA); pLine->getPointAt(i-1,ptB);
		if (ptA.distanceTo(ptB) < 0.0001 )
		{
			double dA,dB;
			pLine->getBulgeAt(i,dA); pLine->getBulgeAt(i-1,dB);

			if(fabs(dA) < fabs(dB)) 
				pLine->removeVertexAt(i);
			else
				pLine->removeVertexAt(i-1);
		}
	}
	pLine->close();

	return TRUE;
}

int CDkDataCheck::TwoPolygonIsIntersect(AcDbObjectId LnIdA,AcDbObjectId LnIdB)
{
	AcDbPolyline *pLineA = NULL;
	AcDbPolyline *pLineB = NULL;

	if(Acad::eOk != acdbOpenObject(pLineA,LnIdA,AcDb::kForRead)){
		acutPrintf("\n打开实体失败！"); return FALSE;
	}

	if(!pLineA->isClosed()){
		pLineA->close();
		AfxMessageBox("\n实体不闭合！"); FlashEntity(LnIdA); return FALSE;
	}

	if(Acad::eOk != acdbOpenObject(pLineB,LnIdB,AcDb::kForRead)){
		acutPrintf("\n打开实体失败！"); pLineA->close(); return FALSE;
	}

	if(!pLineB->isClosed()){
		pLineB->close(); pLineA->close();
		AfxMessageBox("\n实体不闭合！"); FlashEntity(LnIdB); return FALSE;
	}
	
	//构面
	AcDbVoidPtrArray curveSegmentsA;
	curveSegmentsA.append(pLineA);
	AcDbVoidPtrArray regionsA;
	if(AcDbRegion::createFromCurves(curveSegmentsA, regionsA) != Acad::eOk)
	{
		AfxMessageBox("\n地块实体自身相交或有重叠线！");
		int nNum = regionsA.length();
		for(int k=0; k<nNum; k++)
			delete (AcDbRegion *)regionsA.at(k); 
		pLineA->close(); pLineB->close(); 
		FlashEntity(LnIdA); return FALSE;
	}
	AcDbRegion* pRegA = static_cast<AcDbRegion*>(regionsA[0]);  //regionsA.at(0);

	AcDbVoidPtrArray curveSegmentsB;
	curveSegmentsB.append(pLineB);
	AcDbVoidPtrArray regionsB;
	if(AcDbRegion::createFromCurves(curveSegmentsB, regionsB) != Acad::eOk)
	{
		AfxMessageBox("\n地块实体自身相交或有重叠线！");
		int nNum = regionsB.length();
		for(int k=0; k<nNum; k++)
			delete (AcDbRegion *)regionsB.at(k); 

		nNum = regionsA.length();
		for(k=0; k<nNum; k++)
			delete (AcDbRegion *)regionsA.at(k); 

		pLineA->close(); pLineB->close(); 
		FlashEntity(LnIdB); return FALSE;
	}
	AcDbRegion* pRegB = static_cast<AcDbRegion*>(regionsB[0]);  //regionsB.at(0);


	BOOL bResult ;
	if( Acad::eOk == pRegA->booleanOper(AcDb::kBoolIntersect,pRegB))
	{
		if ((pRegB->isNull() == Adesk::kTrue) && (pRegA->isNull() != Adesk::kTrue))
		{
			bResult = FALSE;  //相交返回 FALSE
			int nNum = regionsB.length();
			for(int kk=0; kk<nNum; kk++)
			{
				AcDbRegion* pRegion = (AcDbRegion *)regionsA.at(kk); 
				
				//设置原点坐标，设置X Y轴，

				AcGePlane plane;
				AcGePoint3d origin;
				AcGePoint2d centroid;
				AcGeVector2d prinAxes[2];
				AcGePoint2d extentsLow, extentsHigh;
				AcGeVector3d xAxis, yAxis;

				AcGePoint3d CenPt;
				double perimeter, area, prodInertia;
				double momInertia[2], prinMoments[2], radiiGyration[2];

				pRegion->getPlane(plane);
				plane.getCoordSystem(origin, xAxis, yAxis);

				//通过面域取中心点
				if (pRegion->getAreaProp(origin, xAxis, yAxis,perimeter, area, centroid,momInertia, 
					                     prodInertia, prinMoments, prinAxes, radiiGyration,extentsLow, extentsHigh) == Acad::eOk )
				{
					//得到行心坐标
					AcGeVector3d normal;
					pRegion->getNormal(normal);

					//映射标准坐标系原点为(0, 0,0)，轴为(1, 0,0) 和(0,1,0) 
					//到一个以原点为 origin轴为 xAxis, yAxis 和 zAxis坐标系的矩阵. 
					AcGeMatrix3d mat;
					mat.setCoordSystem(origin,xAxis,yAxis,normal);
					AcGePoint3d cpt(centroid[0],centroid[1],0.0);
					cpt.transformBy(mat);
					asDblArray(cpt);
					CDistEntityTool tempTool;
					double dv = sqrt(area);
					DrawErrorTag(cpt,dv);
				}
			}
		}
		else
			bResult = TRUE;   //不相交返回 TRUE
	}
	else
	{
		AfxMessageBox("求交失败！");
		bResult = FALSE;
	}

	int nCount = regionsB.length();
	for(int k=0; k<nCount; k++)
		delete (AcDbRegion *)regionsB.at(k); 

	nCount = regionsA.length();
	for(k=0; k<nCount; k++)
		delete (AcDbRegion *)regionsA.at(k); 

	pLineA->close(); pLineB->close();

	if(!bResult)
	{
		AfxMessageBox("\n两地块相交或包含!");
		FlashEntity(LnIdA); FlashEntity(LnIdB);  return FALSE;
	}

	return TRUE;
}










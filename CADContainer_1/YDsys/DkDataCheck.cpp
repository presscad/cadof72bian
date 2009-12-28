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
		AfxMessageBox("\n�򿪵ؿ�ʵ��ʧ�ܣ�"); return FALSE;
	}
	if(pEnt->isKindOf(AcDbPolyline::desc())==false)
	{
		AfxMessageBox("�ؿ鲻�� PL �ߣ��ؿ�ʵ�����Ͳ���ȷ��"); pEnt->close(); 
		FlashEntity(ObjId);
		return FALSE;
	}
	AcDbPolyline* pLine = (AcDbPolyline*)pEnt;

	if(pLine->isClosed()!=Adesk::kTrue)
	{
		AfxMessageBox("�ؿ��߲��պϣ�"); pLine->close(); 
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
				AfxMessageBox("\n�ؿ�ʵ�����ظ�����ڣ�"); pLine->close(); 
				FlashEntity(ObjId); return FALSE;
			}
		}
	}

	//���ܹ���ıպ����������ཻ��
	AcDbVoidPtrArray curveSegments;
	curveSegments.append(pLine);
	AcDbVoidPtrArray regions;
	BOOL bCrossSelfOk = (AcDbRegion::createFromCurves(curveSegments, regions) == Acad::eOk);
	int nNum = regions.length();
	for(int k=0; k<nNum; k++)
		delete (AcDbRegion *)regions.at(k); 
	pEnt->close();
	if(!bCrossSelfOk){
		AfxMessageBox("\n�ؿ�ʵ�������ཻ�����ص��ߣ�");
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
		acutPrintf("\n��ʵ��ʧ�ܣ�"); return FALSE;
	}

	if(!pLineA->isClosed()){
		pLineA->close();
		AfxMessageBox("\nʵ�岻�պϣ�"); FlashEntity(LnIdA); return FALSE;
	}

	if(Acad::eOk != acdbOpenObject(pLineB,LnIdB,AcDb::kForRead)){
		acutPrintf("\n��ʵ��ʧ�ܣ�"); pLineA->close(); return FALSE;
	}

	if(!pLineB->isClosed()){
		pLineB->close(); pLineA->close();
		AfxMessageBox("\nʵ�岻�պϣ�"); FlashEntity(LnIdB); return FALSE;
	}
	
	//����
	AcDbVoidPtrArray curveSegmentsA;
	curveSegmentsA.append(pLineA);
	AcDbVoidPtrArray regionsA;
	if(AcDbRegion::createFromCurves(curveSegmentsA, regionsA) != Acad::eOk)
	{
		AfxMessageBox("\n�ؿ�ʵ�������ཻ�����ص��ߣ�");
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
		AfxMessageBox("\n�ؿ�ʵ�������ཻ�����ص��ߣ�");
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
			bResult = FALSE;  //�ཻ���� FALSE
			int nNum = regionsB.length();
			for(int kk=0; kk<nNum; kk++)
			{
				AcDbRegion* pRegion = (AcDbRegion *)regionsA.at(kk); 
				
				//����ԭ�����꣬����X Y�ᣬ

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

				//ͨ������ȡ���ĵ�
				if (pRegion->getAreaProp(origin, xAxis, yAxis,perimeter, area, centroid,momInertia, 
					                     prodInertia, prinMoments, prinAxes, radiiGyration,extentsLow, extentsHigh) == Acad::eOk )
				{
					//�õ���������
					AcGeVector3d normal;
					pRegion->getNormal(normal);

					//ӳ���׼����ϵԭ��Ϊ(0, 0,0)����Ϊ(1, 0,0) ��(0,1,0) 
					//��һ����ԭ��Ϊ origin��Ϊ xAxis, yAxis �� zAxis����ϵ�ľ���. 
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
			bResult = TRUE;   //���ཻ���� TRUE
	}
	else
	{
		AfxMessageBox("��ʧ�ܣ�");
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
		AfxMessageBox("\n���ؿ��ཻ�����!");
		FlashEntity(LnIdA); FlashEntity(LnIdB);  return FALSE;
	}

	return TRUE;
}










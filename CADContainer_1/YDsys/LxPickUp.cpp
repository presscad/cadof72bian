#include "StdAfx.h"
#include "DistBasePara.h"
#include "LxPickUp.h"


CLxPickUp::CLxLine::CLxLine(void)
{
	pch = "0";
}

CLxPickUp::CLxLine::~CLxLine(void)
{
}

void CLxPickUp::CLxLine::GetLxLine()
{
	while (TRUE)
	{
		AcDbObjectIdArray Arr;

		if (!GetAllLine(Arr))
		{
			break;
		}

		SetObjectHighLight(Arr);
		m_LineArr.append(Arr);
		acutPrintf(" 选中%d个实体(回车结束选择,生成区域).\n", DocVars.docData().m_LxObjIdArr.logicalLength() + m_LineArr.logicalLength());
	}
}

void CLxPickUp::CLxLine::SetObjectHighLight(AcDbObjectIdArray& dbObjIdArr, BOOL bHighLight /* = TRUE */)
{
	for (int i = 0; i < dbObjIdArr.logicalLength(); i++)
	{
		AcDbObjectId objId = dbObjIdArr.at(i);

		AcDbEntity *pEnt = NULL;

		if (Acad::eOk != acdbOpenObject(pEnt, objId, AcDb::kForRead))
		{
			continue;
		}

		if (bHighLight)
		{
			pEnt->highlight();
		}
		else
		{
			pEnt->unhighlight();
		}

		pEnt->close();
	}
}

BOOL CLxPickUp::CLxLine::GetAllLine(AcDbObjectIdArray& dbObjIdArr)
{
	ads_name ssName;

	struct resbuf * preb = acutBuildList(RTDXF0, "LINE,POLYLINE,LWPOLYLINE,CIRCLE,ARC,ELLIPSE,SPLINE", 0);

	if (acedSSGet(":S", NULL, NULL, preb, ssName) != RTNORM)
	{
		acedSSFree(ssName);
		acutRelRb(preb);
		return FALSE;
	}
	acutRelRb(preb);

	long lNum = 0;
	acedSSLength(ssName, &lNum);

	AcDbObjectIdArray ArrObjId;

	AcDbObjectId dbObjId;
	ads_name ssTemp;
	for (long i = 0; i < lNum; i++)
	{
		acedSSName(ssName, i, ssTemp);

		acdbGetObjectId(dbObjId, ssTemp);

		ArrObjId.append(dbObjId);
	}

	acedSSFree(ssName);

	CopyAllLine(ArrObjId, dbObjIdArr);

	return TRUE;
}

void CLxPickUp::CLxLine::CopyAllLine(AcDbObjectIdArray& dbObjIdArr, AcDbObjectIdArray& arrObjIdOut)
{
	for (int i = 0; i < dbObjIdArr.logicalLength(); i++)
	{
		AcDbEntity * pEnt = NULL;

		if (Acad::eOk != acdbOpenObject(pEnt, dbObjIdArr.at(i), AcDb::kForRead))
		{
			continue;
		}

		pEnt->close();

		AcDbEntity * pNew = NULL;

		pNew = (AcDbEntity*)pEnt->clone();

		if (pNew == NULL)
		{
			continue;
		}
		
		
		pNew->setLayer("0");//pch
		pNew->setColorIndex(256);//ByLayer

		acedUpdateDisplay();
		Acad::ErrorStatus es;
		AcDbObjectId objId;

		if (Acad::eOk != EnterToDwg(objId, pNew))
		{
			pNew->close();
			continue;
		}


		arrObjIdOut.append(objId);
	}
}

Acad::ErrorStatus CLxPickUp::CLxLine::EnterToDwg(AcDbObjectId& pOutputId, AcDbEntity* pEntity)
{
	Acad::ErrorStatus es;

	AcDbDatabase			* pdbDatab = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable			* pBlockTable = NULL;
	AcDbBlockTableRecord	* pBlockTableRecord = NULL;

	es = pdbDatab->getSymbolTable(pBlockTable, AcDb::kForRead);
	if (es != Acad::eOk)
	{
		pEntity->close();
		return es;
	}

	es = pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	if (es != Acad::eOk)
	{
		pEntity->close();
		pBlockTable->close();
		return es;
	}

	es = pBlockTableRecord->appendAcDbEntity(pOutputId, pEntity);
	if (es != Acad::eOk)
	{
		pEntity->close();
		pBlockTable->close();
		pBlockTableRecord->close();
		return es;
	}

	pEntity->close();
	pBlockTable->close();
	pBlockTableRecord->close();
	return es;
}

//////////////////////////////////////////////////////////////////////////

CLxPickUp::CPickUpBorder::CPickUpBorder(void)
{
	pch = "0";
	m_Num = 100;
	m_dOffSetX = 0.0;
	m_dOffSetY = 0.0;
}

CLxPickUp::CPickUpBorder::~CPickUpBorder(void)
{
}

AcDbObjectId CLxPickUp::CPickUpBorder::GetOutPline(AcDbObjectIdArray dbObjIdArr)
{
	AcDbObjectIdArray dbObjIdArrOut;
	//copy 所有的线到特定层
	CopyAllLine(dbObjIdArr, dbObjIdArrOut);

	//创建交叉的线
	AcDbObjectIdArray dbObjIdArrOutNew;
	CreateCrossLine(dbObjIdArrOut, dbObjIdArrOutNew);

	//RemoveHangingLine(dbObjIdArrOutNew);

	int rejectCount = 0;
	AcDbVoidPtrArray curveSegs;
	AcDbEntity* ent;
	Acad::ErrorStatus es;

	int len = dbObjIdArrOutNew.logicalLength();
	for (int i = 0; i < len; i++) 
	{
		es = acdbOpenAcDbEntity(ent, dbObjIdArrOutNew[i], AcDb::kForRead);
		ent->close();    

		if (es == Acad::eOk) 
		{
			if (ent->isKindOf(AcDbCurve::desc()))
			{
				curveSegs.append(static_cast<void*>(ent));
			}
			else 
			{
				rejectCount++;
			}
		}
	}
	if (curveSegs.isEmpty()) 
	{
		return 0;
	}

	AcDbVoidPtrArray regions;
	es = AcDbRegion::createFromCurves(curveSegs, regions);

	if (regions.logicalLength() < 1)
	{
		//acutPrintf("\n计算轮廓线失败，请确认");
		RemoveEntity(dbObjIdArrOutNew);
		return 0;
	}

	AcDbRegion *ppReg = (AcDbRegion*)regions.at(0);

	for (i = 1; i < regions.logicalLength(); i++)
	{
		if (ppReg->booleanOper(AcDb::kBoolUnite, (AcDbRegion*)regions.at(i)) != Acad::eOk)
		{
		}
	}

	if (ppReg==NULL)
	{
		//acutPrintf("\n计算轮廓线失败，请确认");
		RemoveEntity(dbObjIdArrOutNew);
		return 0;
	}

	if (ppReg->isNull())
	{
		return 0;
	}

	AcDbVoidPtrArray entitySet ;
	if (ppReg->explode(entitySet)!= Acad::eOk)
	{
		//acutPrintf("\n计算轮廓线失败，请确认");
		RemoveEntity(dbObjIdArrOutNew);
		return 0;
	}

	for (int eNum=0; eNum<entitySet.logicalLength();eNum++)
	{
		if ( (!((AcDbEntity*)entitySet[eNum])->isKindOf(AcDbLine::desc()))
			&& (!((AcDbEntity*)entitySet[eNum])->isKindOf(AcDbArc::desc())) )
		{
			//acutPrintf("\n计算轮廓线失败，请确认");
			RemoveEntity(dbObjIdArrOutNew);
			return 0;
		}
	}

	AcDbDatabase * pdbDatab = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable			* pDbBT;
	AcDbBlockTableRecord	* pDbBTR;

	AcDbObjectId dbObjIdCreate;

	pdbDatab->getSymbolTable(pDbBT, AcDb::kForRead);
	pDbBT->getAt(ACDB_MODEL_SPACE, pDbBTR,AcDb::kForWrite);

	AcDbObjectIdArray Pl;

	for (int j = 0; j < entitySet.logicalLength(); j++)
	{
		AcDbObjectId dbObjId;

		pDbBTR->appendAcDbEntity(dbObjId, (AcDbEntity*)entitySet.at(j));
		((AcDbEntity*)entitySet.at(j))->setLayer(pch);
		((AcDbEntity*)entitySet.at(j))->close();
		Pl.append(dbObjId);
	}

	pDbBT->close();
	pDbBTR->close();

	ads_name ss1;
	ads_name ssTemp1;
	ads_name ssInput;

	acdbGetAdsName(ss1, Pl.at(0));

	for (j = 1; j < Pl.logicalLength(); j++)
	{
		if (acdbGetAdsName(ssTemp1, Pl.at(j)) == Acad::eOk) 
		{
			if (j == 1)
			{
				acedSSAdd(ssTemp1,NULL,ssInput);
			}
			else
			{
				if (acedSSAdd(ssTemp1, ssInput, ssInput) != RTNORM)
				{
				}
			}
		}
	}

	struct resbuf * rb_cmdecho_Off = acutBuildList(RTSHORT,0,0);
	struct resbuf * rb_cmdecho_On = acutBuildList(RTSHORT,1,0);
	acedSetVar("cmdecho", rb_cmdecho_Off);

	acedCommand(RTSTR, "PEDIT", RTENAME, ss1, RTSTR, "Y", RTSTR, "j", RTPICKS, ssInput, RTSTR, "", RTSTR, "", RTNONE);

	acedSetVar("cmdecho", rb_cmdecho_On);
	acutRelRb(rb_cmdecho_Off);
	acutRelRb(rb_cmdecho_On);

	acedSSFree(ssInput);

	ads_name pl;

	acdbEntLast(pl);

	AcDbObjectId dbPlCreate;
	AcDbObject* pPlCreate;
	acdbGetObjectId(dbPlCreate, pl);

	MoveEntity(dbPlCreate, -m_dOffSetX, -m_dOffSetY);

	acdbOpenObject(pPlCreate, dbPlCreate, AcDb::kForWrite);
	if (pPlCreate)
	{
		((AcDbEntity*)pPlCreate)->setLayer(pch);
		((AcDbPolyline*)pPlCreate)->setClosed( Adesk::kTrue);
		Check2dPoint(pPlCreate);

		pPlCreate->close();
	}

	RemoveEntity(dbObjIdArrOutNew);
	//acutPrintf("\n正确生成外轮廓\n");

	return dbPlCreate;
}

void CLxPickUp::CPickUpBorder::RemoveEntity(AcDbObjectIdArray&dbObjIdArrOutNew)
{
	for (int nObjNum =0; nObjNum<dbObjIdArrOutNew.logicalLength();nObjNum++)
	{
		AcDbObjectId dbObj;
		AcDbObject *pdbObj;
		acdbOpenObject(pdbObj,dbObjIdArrOutNew.at(nObjNum),AcDb::kForWrite);
		if(pdbObj)
		{
			pdbObj->erase();
			pdbObj->close();
		}
	}
}

void CLxPickUp::CPickUpBorder::Check2dPoint(AcDbObject *pDbO,AcDbDatabase *pDbDatab /* = acdbHostApplicationServices */)
{
	if (!pDbO->isKindOf(AcDbPolyline::desc())) 
	{
		return ;
	}

	long				lCount=0;
	AcGePoint3d			GePoi3d1,GePoi3d2;

	lCount=((AcDbPolyline *)pDbO)->numVerts();
	if (lCount<2) 
	{
		return ;
	}

	for (long lPnt=0;lPnt<lCount-1;lPnt++) 
	{
		((AcDbPolyline *)pDbO)->getPointAt(lPnt,GePoi3d1);
		for(int j=lPnt+1;j<lCount;j++)
		{
			((AcDbPolyline *)pDbO)->getPointAt(j,GePoi3d2);
			if (fabs(GePoi3d1.x - GePoi3d2.x) <  0.000001&& 
				fabs(GePoi3d1.y - GePoi3d2.y) <  0.000001&& 
				fabs(GePoi3d1.z - GePoi3d2.z) < 0.000001) 
			{
				pDbO->upgradeOpen();

				if (lCount<=2) 
				{
					pDbO->erase();
					pDbO->downgradeOpen();
					break;
				}
				else
				{
					((AcDbPolyline *)pDbO)->removeVertexAt(j);
				}
				pDbO->downgradeOpen();
				lCount--;
				j--;
			}
		}
	}
}

void CLxPickUp::CPickUpBorder::CopyAllLine(AcDbObjectIdArray& dbObjIdArr, AcDbObjectIdArray &arrObjIdOut)
{
	AcDbDatabase			* pdbDatab = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable			* pBlockTable = NULL;
	AcDbBlockTableRecord	* pBlockTableRecord = NULL;

	pdbDatab->getSymbolTable(pBlockTable, AcDb::kForRead);
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

	Acad::ErrorStatus es;

	long lNum = dbObjIdArr.logicalLength();
	AcDbObject *pdbObj = NULL;
	for (int i = 0; i < lNum; i++)
	{
		acdbOpenObject(pdbObj, dbObjIdArr.at(i), AcDb::kForRead);
		if (pdbObj)
		{
			if (pdbObj->isKindOf(AcDbLine::desc()))
			{
				AcDbObjectId dbObjId;
				AcDbLine *pLine = new AcDbLine();

				AcGePoint3d strPt = ((AcDbLine*)pdbObj)->startPoint();
				AcGePoint3d endPt = ((AcDbLine*)pdbObj)->endPoint();

				pLine->setStartPoint(strPt);
				pLine->setEndPoint(endPt);

				pBlockTableRecord->appendAcDbEntity(dbObjId,pLine);
				arrObjIdOut.append(dbObjId);

				//pLine->setLayer("规整图层");
				pLine->close();
			}
			else if (pdbObj->isKindOf(AcDb2dPolyline::desc()))
			{
				AcDbObjectIterator *pObjIter = ((AcDb2dPolyline *)pdbObj)->vertexIterator();

				if (pObjIter == NULL) {
					continue;
				}

				AcGePoint3dArray ptArr;

				AcDb2dVertex *pVertex;

				AcDb::Poly2dType type = ((AcDb2dPolyline *)pdbObj)->polyType();

				for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
				{
					if (pObjIter->entity()) {
						continue;
					}

					if (acdbOpenObject(pVertex, pObjIter->objectId(), AcDb::kForWrite) != Acad::eOk) {
						continue;
					}

					if (type == AcDb::Poly2dType::k2dSimplePoly) {
						if (pVertex->vertexType() != AcDb::k2dVertex) {
							pVertex->close();
							continue;
						}
					}
					else if ((type == AcDb::Poly2dType::k2dCubicSplinePoly) ||
						(type == AcDb::Poly2dType::k2dQuadSplinePoly))
					{
						if (pVertex->vertexType() != AcDb::k2dSplineCtlVertex) {
							pVertex->close();
							continue;
						}
					}
					else if (type == AcDb::Poly2dType::k2dFitCurvePoly) {
					}

					AcGePoint3d pt3d = pVertex->position();

					ptArr.append(pt3d);

					pVertex->close();
				}

				if (ptArr.length() < 2) {
					continue;
				}

				AcDbLine *pLine = NULL;

				for (int j = 0; j < ptArr.logicalLength() - 1; j++)
				{
					pLine = new AcDbLine();
					pLine->setStartPoint(ptArr.at(j));
					pLine->setEndPoint(ptArr.at(j + 1));

					AcDbObjectId dbObjId;
					pBlockTableRecord->appendAcDbEntity(dbObjId,pLine);
					arrObjIdOut.append(dbObjId);

					//pLine->setLayer("规整图层");
					pLine->close();
				}

				if (Adesk::kTrue == ((AcDb2dPolyline *)pdbObj)->isClosed())
				{
					pLine = new AcDbLine();
					pLine->setStartPoint(ptArr.at(ptArr.logicalLength() - 1));
					pLine->setEndPoint(ptArr.at(0));

					AcDbObjectId dbObjId;
					pBlockTableRecord->appendAcDbEntity(dbObjId,pLine);
					arrObjIdOut.append(dbObjId);

					//pLine->setLayer("规整图层");
					pLine->close();
				}
			}
			else if (pdbObj->isKindOf(AcDb3dPolyline::desc()))
			{
				AcDbObjectIterator *pObjIter = ((AcDb3dPolyline *)pdbObj)->vertexIterator();

				if (pObjIter == NULL) {
					continue;
				}

				AcGePoint3dArray ptArr;

				AcDb3dPolylineVertex  *pVertex;

				AcDb::Poly3dType type = ((AcDb3dPolyline *)pdbObj)->polyType();

				for (pObjIter->start(); !pObjIter->done(); pObjIter->step())
				{
					if (pObjIter->entity()) {
						continue;
					}

					if (acdbOpenObject(pVertex, pObjIter->objectId(), AcDb::kForWrite) != Acad::eOk) {
						continue;
					}

					if (type ==  AcDb::Poly3dType::k3dSimplePoly) {
						if (pVertex->vertexType() != AcDb::k3dSimpleVertex) {
							pVertex->close();
							continue;
						}
					}
					else if (pVertex->vertexType() != AcDb::k3dControlVertex) {
						pVertex->close();
						continue;
					}

					AcGePoint3d pt3d = pVertex->position();

					ptArr.append(AcGePoint3d(pt3d.x, pt3d.y, 0.0));

					pVertex->close();
				}

				if (ptArr.length() < 2) {
					continue;
				}

				AcDbLine *pLine = NULL;

				for (int j = 0; j < ptArr.logicalLength() - 1; j++)
				{
					pLine = new AcDbLine();
					pLine->setStartPoint(ptArr.at(j));
					pLine->setEndPoint(ptArr.at(j + 1));

					AcDbObjectId dbObjId;
					pBlockTableRecord->appendAcDbEntity(dbObjId,pLine);
					arrObjIdOut.append(dbObjId);

					//pLine->setLayer("规整图层");
					pLine->close();
				}

				if (Adesk::kTrue == ((AcDb3dPolyline *)pdbObj)->isClosed())
				{
					pLine = new AcDbLine();
					pLine->setStartPoint(ptArr.at(ptArr.logicalLength() - 1));
					pLine->setEndPoint(ptArr.at(0));

					AcDbObjectId dbObjId;
					pBlockTableRecord->appendAcDbEntity(dbObjId,pLine);
					arrObjIdOut.append(dbObjId);

					//pLine->setLayer("规整图层");
					pLine->close();
				}
			}
			else if (pdbObj->isKindOf(AcDbPolyline::desc()))
			{
				AcDbVoidPtrArray entitySet;
				if (Acad::eOk != ((AcDbPolyline*)pdbObj)->explode(entitySet))
				{
				}
				else
				{
					for (int j = 0; j < entitySet.logicalLength(); j++)
					{
						AcDbObjectId dbObjId = NULL;
						pBlockTableRecord->appendAcDbEntity(dbObjId, (AcDbEntity*)entitySet.at(j));
						AcDbEntity *pEntity = (AcDbEntity*)entitySet.at(j);

						arrObjIdOut.append(dbObjId);

						//pEntity->setLayer("规整图层");
						pEntity->close();
					}
				}
			}
			else if (pdbObj->isKindOf(AcDbCircle::desc()))
			{
				AcDbObjectId dbObjId;

				double dStAngle  =  0;
				double dEndAngle =  PI;
				AcGePoint3d PtCenter = ((AcDbCircle*)pdbObj)->center();
				double dRadius = ((AcDbCircle*)pdbObj)->radius();

				AcDbArc *pArc = new AcDbArc(PtCenter, dRadius, dStAngle, dEndAngle);

				pBlockTableRecord->appendAcDbEntity(dbObjId, pArc);

				arrObjIdOut.append(dbObjId);
				//pArc->setLayer("规整图层");
				pArc->close();

				pArc = new AcDbArc(PtCenter, dRadius, -dEndAngle, dStAngle);

				pBlockTableRecord->appendAcDbEntity(dbObjId, pArc);

				arrObjIdOut.append(dbObjId);
				//pArc->setLayer("规整图层");
				pArc->close();
			}
			else if (pdbObj->isKindOf(AcDbArc::desc()))
			{
				AcDbObjectId dbObjId;

				double dStAngle  =  ((AcDbArc*)pdbObj)->startAngle();
				double dEndAngle =  ((AcDbArc*)pdbObj)->endAngle();
				AcGePoint3d PtCenter = ((AcDbArc*)pdbObj)->center();
				double dRadius = ((AcDbArc*)pdbObj)->radius();

				AcDbArc *pArc = new AcDbArc(PtCenter, dRadius, dStAngle, dEndAngle);

				pBlockTableRecord->appendAcDbEntity(dbObjId, pArc);

				arrObjIdOut.append(dbObjId);
				//pArc->setLayer("规整图层");
				pArc->close();
			}
			else if (pdbObj->isKindOf(AcDbEllipse::desc()))
			{
				AcDbEllipse *pEli = (AcDbEllipse *)pdbObj;
				AcGePoint2d center = AcGePoint2d(pEli->center().x, pEli->center().y);
				AcGeEllipArc2d *pGEli = new AcGeEllipArc2d();
				pGEli->setCenter(center);
				pGEli->setAxes(AcGeVector2d(pEli->majorAxis().x, pEli->majorAxis().y),
					AcGeVector2d(pEli->minorAxis().x, pEli->minorAxis().y));
				AcGePoint3d temp;
				pEli->getClosestPointTo(pEli->center(), temp, Adesk::kTrue);
				AcGePoint2d pt1 = AcGePoint2d(temp.x, temp.y);
				pGEli->setMajorRadius(pt1.distanceTo(center) / pEli->radiusRatio());
				pGEli->setMinorRadius(pt1.distanceTo(center));

				double startAng = pEli->startAngle();
				double endAng = pEli->endAngle();
				double minAng = (startAng + endAng) / 2.0;
				pGEli->setAngles(startAng, minAng);
				AcGeCurve2d *pGeCurve = (AcGeCurve2d *)pGEli;

				AcGePoint2dArray ptArr, ptArrtemp;
				pGeCurve->getSamplePoints(m_Num/2 + 1, ptArrtemp);
				ptArr.append(ptArrtemp);
				ptArrtemp.setLogicalLength(0);

				pGEli->setAngles(minAng, endAng);
				pGeCurve = (AcGeCurve2d *)pGEli;

				pGeCurve->getSamplePoints(m_Num/2 + 1, ptArrtemp);
				ptArr.append(ptArrtemp);
				ptArrtemp.setLogicalLength(0);

				if (pGeCurve != NULL)
				{
					delete pGeCurve;
					pGeCurve = NULL;
				}

				if (ptArr.length() < 2) {
					continue;
				}

				AcDbLine *pLine = NULL;

				for (int j = 0; j < ptArr.logicalLength() - 1; j++)
				{
					pLine = new AcDbLine();
					pLine->setStartPoint(AcGePoint3d(ptArr.at(j).x, ptArr.at(j).y, 0.0));
					pLine->setEndPoint(AcGePoint3d(ptArr.at(j+1).x, ptArr.at(j+1).y, 0.0));

					AcDbObjectId dbObjId;
					es = pBlockTableRecord->appendAcDbEntity(dbObjId,pLine);
					arrObjIdOut.append(dbObjId);

					//pLine->setLayer("规整图层");
					pLine->close();
				}

				if (startAng + 2 * PI - endAng < 0.001)
				{
					pLine = new AcDbLine();
					pLine->setStartPoint(AcGePoint3d(ptArr.at(ptArr.logicalLength() - 1).x,
						ptArr.at(ptArr.logicalLength() - 1).y, 0.0));
					pLine->setEndPoint(AcGePoint3d(ptArr.at(0).x, ptArr.at(0).y, 0.0));

					AcDbObjectId dbObjId;
					es = pBlockTableRecord->appendAcDbEntity(dbObjId,pLine);
					arrObjIdOut.append(dbObjId);

					//pLine->setLayer("规整图层");
					pLine->close();
				}
			}
			else if (pdbObj->isKindOf(AcDbSpline::desc()))
			{
				AcDbSpline *pSL=(AcDbSpline *)pdbObj;
				if (!pSL)
					//return Acad::eNotImplemented;
					continue;
				if (pSL->isNull()==Adesk::kTrue)
					//return Acad::eNotImplemented;
					continue;

				int degree;
				Adesk::Boolean rational;
				Adesk::Boolean closed;
				Adesk::Boolean periodic;
				AcGePoint3dArray controlPoints;
				AcGeDoubleArray knots;
				AcGeDoubleArray weights;
				double controlPtTol;
				double knotTol;
				AcGeTol tol;
				Acad::ErrorStatus es;
				es=pSL->getNurbsData(degree,rational,closed,periodic,controlPoints,knots,weights,
					controlPtTol,knotTol);
				if (es!=Acad::eOk)
					//return Acad::eNotImplemented;
					continue;

				AcGeCurve2d * pGeCurve = NULL;
				//if (rational==Adesk::kTrue)
				{
					AcGePoint2dArray control2dPoints;
					AcGeDoubleArray weights2d;

					for(int i=0;i<controlPoints.length();i++)
					{
						AcGePoint2d pt(controlPoints.at(i).x,controlPoints.at(i).y);
						control2dPoints.append(pt);
						weights2d.append(0);
					}
					AcGeNurbCurve2d *pNurb=new AcGeNurbCurve2d(degree,knots,control2dPoints,weights2d,periodic);
					if (closed==Adesk::kTrue)
						pNurb->makeClosed();
					if (pSL->hasFitData()==Adesk::kTrue)
					{
						AcGePoint3dArray fitPoints;
						double fitTolerance;
						Adesk::Boolean tangentsExist;
						AcGeVector3d startTangent;
						AcGeVector3d endTangent;
						pSL->getFitData(fitPoints,degree,fitTolerance,tangentsExist,startTangent,endTangent);
						tol.setEqualPoint(fitTolerance);

						AcGePoint2dArray fit2dPoints;


						for(int i=0;i<fitPoints.length();i++)
						{
							AcGePoint2d pt(fitPoints.at(i).x,fitPoints.at(i).y);
							fit2dPoints.append(pt);
						}

						if (tangentsExist==Adesk::kTrue)
						{
							pNurb->setFitData(fit2dPoints, AcGeVector2d(startTangent.x, startTangent.y),
								AcGeVector2d(endTangent.x, endTangent.y), tol);
						}
						else
						{
							pNurb->setFitData(degree,fit2dPoints,tol);
						}

						pGeCurve=(AcGeCurve2d *)pNurb;
					}
					else
					{
						AcGeNurbCurve2d *pNurb=new AcGeNurbCurve2d(degree,knots,control2dPoints,periodic);
						if (closed==Adesk::kTrue)
							pNurb->makeClosed();
						if (pSL->hasFitData()==Adesk::kTrue)
						{
							AcGePoint3dArray fitPoints;
							double fitTolerance;
							Adesk::Boolean tangentsExist;
							AcGeVector3d startTangent;
							AcGeVector3d endTangent;
							pSL->getFitData(fitPoints,degree,fitTolerance,tangentsExist,startTangent,endTangent);
							tol.setEqualPoint(fitTolerance);

							AcGePoint2dArray fit2dPoints;

							for(int i=0;i<fitPoints.length();i++)
							{
								AcGePoint2d pt(fitPoints.at(i).x,fitPoints.at(i).y);
								fit2dPoints.append(pt);
							}


							if (tangentsExist==Adesk::kTrue)
							{
								pNurb->setFitData(fit2dPoints, AcGeVector2d(startTangent.x, startTangent.y),
									AcGeVector2d(endTangent.x, endTangent.y), tol);
							}
							else
							{
								pNurb->setFitData(degree,fit2dPoints,tol);
							}

						}
						pGeCurve=(AcGeCurve2d *)pNurb;
					}
				}
				if (pGeCurve == NULL)
				{
					continue;
				}

				AcGePoint2d endPt;

				pGeCurve->hasEndPoint(endPt);

				AcGePoint2dArray ptArr;

				//PartExec(pGeCurve, endPt, 10, ptArr);
				double inc_ArcLength;
				double endParam;
				es = pSL->getEndParam(endParam);
				es = pSL->getDistAtParam(endParam, inc_ArcLength);
				inc_ArcLength = inc_ArcLength / m_Num;
				if (inc_ArcLength < 0.005)
				{
					inc_ArcLength = 0.005;
				}
				PartExec(pGeCurve, endPt, inc_ArcLength, ptArr);

				if (ptArr.length() < 2) {
					continue;
				}

				AcDbLine *pLine = NULL;

				for (int j = 0; j < ptArr.logicalLength() - 1; j++)
				{
					pLine = new AcDbLine();
					pLine->setStartPoint(AcGePoint3d(ptArr.at(j).x, ptArr.at(j).y, 0.0));
					pLine->setEndPoint(AcGePoint3d(ptArr.at(j+1).x, ptArr.at(j+1).y, 0.0));

					AcDbObjectId dbObjId;
					es = pBlockTableRecord->appendAcDbEntity(dbObjId,pLine);
					arrObjIdOut.append(dbObjId);

					//pLine->setLayer("规整图层");
					pLine->close();
				}

				if (Adesk::kTrue == ((AcDbSpline *)pdbObj)->isClosed())
				{
					pLine = new AcDbLine();
					pLine->setStartPoint(AcGePoint3d(ptArr.at(ptArr.logicalLength() - 1).x,
						ptArr.at(ptArr.logicalLength() - 1).y, 0.0));
					pLine->setEndPoint(AcGePoint3d(ptArr.at(0).x, ptArr.at(0).y, 0.0));

					AcDbObjectId dbObjId;
					es = pBlockTableRecord->appendAcDbEntity(dbObjId,pLine);
					arrObjIdOut.append(dbObjId);

					//pLine->setLayer("规整图层");
					pLine->close();
				}
			}
		}
		pdbObj->close();
	}

	pBlockTable->close();
	pBlockTableRecord->close();

	AcDbObject * pTemp = NULL;
	if (arrObjIdOut.logicalLength() >= 1)
	{
		acdbOpenObject(pTemp, arrObjIdOut.at(0), AcDb::kForRead);
		if (pTemp)
		{
			pTemp->close();

			AcGePoint3d gePoi3d ;

			if (pTemp->isKindOf(AcDbLine::desc()))
			{
				gePoi3d = ((AcDbLine*)pTemp)->startPoint();
				m_dOffSetY = gePoi3d.x;
				m_dOffSetY = gePoi3d.y;
			}
			else if (pTemp->isKindOf(AcDbArc::desc()))
			{
				AcGePoint3d gePoi3dPt ;
				gePoi3dPt = ((AcDbArc*)pTemp)->center();
				m_dOffSetY = gePoi3dPt.x;
				m_dOffSetY = gePoi3dPt.y;
			}
		}

		for (int k = 0; k < arrObjIdOut.logicalLength(); k++)
		{
			MoveEntity(arrObjIdOut.at(k), m_dOffSetX, m_dOffSetY);
		}
	}
}

//获得起始点及长度
void CLxPickUp::CPickUpBorder::getStartPointAndLength(AcGeCurve2d *pCurve2d, AcGePoint2d startPt, AcGePoint2d endPt, double &startParam,
										   double &len_EllArc, AcGePoint2dArray &PartPtArray)
{
	if (PartPtArray.isEmpty())
	{
		PartPtArray.append(startPt);
	}	
	else if (!startPt.isEqualTo(PartPtArray[PartPtArray.length()-1]))
	{
		PartPtArray.append(startPt);
	}

	startParam = pCurve2d->paramOf(startPt);

	double endParam = pCurve2d->paramOf(endPt);

	if (pCurve2d->isClosed() && (pCurve2d->isKindOf(AcGe::kCircArc2d) || pCurve2d->isKindOf(AcGe::kEllipArc2d)))
	{
		len_EllArc = pCurve2d->length(0,2*PI);
	}
	else
	{
		if (pCurve2d->isKindOf(AcGe::kNurbCurve2d))
		{
			AcGeNurbCurve2d *pSPline;

			pSPline = (AcGeNurbCurve2d *)pCurve2d;

			endParam = pSPline->endParam();
		}
		len_EllArc = pCurve2d->length(startParam,endParam);
	}
}

// 获得点，并加入数组
void CLxPickUp::CPickUpBorder::addPt(AcGeCurve2d *pCurve2d, AcGePoint2d startPt, AcGePoint2d endPt, Adesk::Boolean posParamDir,
						  double inc_ArcLength, AcGePoint2dArray &PartPtArray)
{
	double startParam;

	double len_EllArc;

	getStartPointAndLength(pCurve2d,startPt,endPt,startParam,len_EllArc,PartPtArray);

	double ArcLength = inc_ArcLength;

	while (1) 
	{
		double InsParam;

		InsParam = pCurve2d->paramAtLength(startParam,ArcLength,posParamDir);

		AcGePoint2d InsPt;

		InsPt = pCurve2d->evalPoint(InsParam);

		PartPtArray.append(InsPt);

		ArcLength += inc_ArcLength;

		if (ArcLength >= len_EllArc) break;
	}
	PartPtArray.append(endPt);
}

//曲线分割的实行函数
void CLxPickUp::CPickUpBorder::PartExec(AcGeCurve2d *pCurve2d, AcGePoint2d pPoint, double inc_ArcLength, AcGePoint2dArray &PartPtArray)
{
	AcGePoint2d startPt,endPt;

	pCurve2d->hasStartPoint(startPt);

	pCurve2d->hasEndPoint(endPt);

	Adesk::Boolean posParamDir = true;

	if(!pCurve2d->isClosed())
	{
		if(endPt.isEqualTo(pPoint))
		{
			posParamDir = false;

			endPt = startPt;

			pCurve2d->hasEndPoint(startPt);
		}
	}

	addPt(pCurve2d,startPt,endPt,posParamDir,inc_ArcLength,PartPtArray);
}

void CLxPickUp::CPickUpBorder::MoveEntity(AcDbObjectId dbObjId ,double dOffsetX,double dOffsetY)
{
	AcGePoint2d gePoi2dPt;
	AcDbObjectId DbOId;
	AcGePoint3d			gePoi3dPt;

	AcDbObject * pObject = NULL;
	acdbOpenObject(pObject,dbObjId,AcDb::kForWrite);
	if(pObject)
	{
		if(pObject->isKindOf(AcDbEntity::desc()))
		{
			if(pObject->isKindOf(AcDbPolyline::desc()))
			{
				int nNumOfPoint=((AcDbPolyline*)pObject)->numVerts();
				for(int i=0;i<nNumOfPoint;i++)
				{
					((AcDbPolyline *)pObject)->getPointAt(i,gePoi2dPt);

					gePoi2dPt.x = gePoi2dPt.x -dOffsetX;
					gePoi2dPt.y = gePoi2dPt.y -dOffsetY;

					((AcDbPolyline*)pObject)->setPointAt(i,gePoi2dPt);
				}
			}
			else if(pObject->isKindOf(AcDb2dPolyline::desc()))
			{
				AcDb2dVertex			* pDb2dV;
				AcDbObjectIterator		* pDbOIt;
				pDbOIt = ((AcDb2dPolyline *)pObject)->vertexIterator();
				for(pDbOIt->start();!pDbOIt->done(); pDbOIt->step()) 
				{
					DbOId = pDbOIt->objectId();
					((AcDb2dPolyline *)pObject)->openVertex(pDb2dV, DbOId, AcDb::kForWrite);
					gePoi3dPt=pDb2dV->position();

					gePoi3dPt.x-=dOffsetX;
					gePoi3dPt.y-=dOffsetY;

					pDb2dV->setPosition(gePoi3dPt);
					pDb2dV->close();
				}
				delete pDbOIt;
				pObject->close();
			}
			else if(pObject->isKindOf(AcDb3dPolyline::desc()))
			{
				AcDbObjectId  DbOId;
				AcDb3dPolylineVertex	* pDb3dPV;
				AcDbObjectIterator		* pDbOIt;
				pDbOIt = ((AcDb3dPolyline *)pObject)->vertexIterator();
				for(pDbOIt->start();!pDbOIt->done(); pDbOIt->step())
				{
					DbOId = pDbOIt->objectId();
					((AcDb3dPolyline *)pObject)->openVertex(pDb3dPV, DbOId, AcDb::kForWrite);
					gePoi3dPt = pDb3dPV->position();

					gePoi3dPt.x-=dOffsetX;
					gePoi3dPt.y-=dOffsetY;

					pDb3dPV->setPosition(gePoi3dPt);
					pDb3dPV->close();		
				}
				delete pDbOIt;		
			}
			else if(pObject->isKindOf(AcDbLine::desc()))
			{
				AcGePoint3d geStartPt;
				AcGePoint3d geEndPt;

				geStartPt =((AcDbLine*) pObject)->startPoint();
				geStartPt.x -= dOffsetX;
				geStartPt.y -= dOffsetY;
				((AcDbLine*) pObject)->setStartPoint(geStartPt);

				geEndPt =  ((AcDbLine*) pObject)->endPoint();
				geEndPt.x -= dOffsetX;
				geEndPt.y -= dOffsetY;
				((AcDbLine*) pObject)->setEndPoint(geEndPt);
			}
			else if(pObject->isKindOf(AcDbBlockReference::desc()))
			{
				AcGePoint3d gePoi3d; 
				gePoi3d = ((AcDbBlockReference*) pObject)->position();
				gePoi3d.x -= dOffsetX;
				gePoi3d.y -= dOffsetY;
				((AcDbBlockReference*) pObject)->setPosition(gePoi3d);
			}
			else if(pObject->isKindOf(AcDbCircle::desc()))
			{
				AcGePoint3d gePoi3d;
				gePoi3d=((AcDbCircle*)pObject)->center();

				gePoi3d.x -= dOffsetX;
				gePoi3d.y -= dOffsetY;

				((AcDbCircle*)pObject)->setCenter(gePoi3d);
			}
			else if(pObject->isKindOf(AcDbCircle::desc()))
			{
				AcGePoint3d gePoi3d;
				gePoi3d=((AcDbCircle*)pObject)->center();

				gePoi3d.x -= dOffsetX;
				gePoi3d.y -= dOffsetY;

				((AcDbCircle*)pObject)->setCenter(gePoi3d);
			}
			else if(pObject->isKindOf(AcDbArc::desc()))
			{
				AcGePoint3d gePoi3d;
				gePoi3d=((AcDbArc*)pObject)->center();

				gePoi3d.x -= dOffsetX;
				gePoi3d.y -= dOffsetY;

				((AcDbArc*)pObject)->setCenter(gePoi3d);
			}
			else if(pObject->isKindOf(AcDbText::desc()))
			{
				if (((AcDbText *)pObject)->horizontalMode() == AcDb::kTextLeft) 
				{
					gePoi3dPt = ((AcDbText *)pObject)->position();

					gePoi3dPt.x-=dOffsetX;
					gePoi3dPt.y-=dOffsetY;

					((AcDbText*)pObject)->setPosition(gePoi3dPt);
				}
				else
				{
					gePoi3dPt=((AcDbText*)pObject)->alignmentPoint();

					gePoi3dPt.x-=dOffsetX;
					gePoi3dPt.y-=dOffsetY;

					((AcDbText*)pObject)->setAlignmentPoint(gePoi3dPt);
				}
			}
			else if(pObject->isKindOf(AcDbMText::desc()))
			{
				gePoi3dPt = ((AcDbMText*)pObject)->location();
				gePoi3dPt.x-=dOffsetX;
				gePoi3dPt.y-=dOffsetY;

				((AcDbMText*)pObject)->setLocation(gePoi3dPt);
			}
		}
		pObject->close();
	}
}

void CLxPickUp::CPickUpBorder::CreateCrossLine(AcDbObjectIdArray&dbObjIdArrOut, AcDbObjectIdArray&dbObjIdArrOutNew)
{
	AcDbObject *pdbObj = NULL;

	int nObjNum = dbObjIdArrOut.logicalLength();
	dbObjIdArrOutNew = dbObjIdArrOut;

	AcDbObjectIdArray dbIdRemove;

	for(int i=0 ;i<nObjNum ;i++)
	{
		acdbOpenObject(pdbObj,dbObjIdArrOut.at(i),AcDb::kForRead);
		pdbObj->close();

		CreateTempLine(pdbObj,dbObjIdArrOut,dbObjIdArrOutNew,dbIdRemove);
	}

	nObjNum = dbIdRemove.logicalLength();
	for(i=0 ;i<nObjNum;i++)
	{
		acdbOpenObject(pdbObj,dbIdRemove.at(i),AcDb::kForWrite);
		if(pdbObj)
		{
			pdbObj->erase();
			pdbObj->close();
		}
	}
}

BOOL CLxPickUp::CPickUpBorder::GetArcStAndEdPt(AcDbArc*pdbArc,AcGePoint3d &geStPt,AcGePoint3d &geEdPt)
{
	if(pdbArc)
	{
		if(pdbArc->isKindOf(AcDbArc::desc()))
		{
			double dStAngle = pdbArc->startAngle();
			double dEdAngle = pdbArc->endAngle();
			double dRadius  = pdbArc->radius();

			AcGePoint3d gePtCenter = pdbArc->center();

			geStPt.x = gePtCenter.x+dRadius*cos(dStAngle);
			geStPt.y = gePtCenter.y+dRadius*sin(dStAngle);

			geEdPt.x = gePtCenter.x+dRadius*cos(dEdAngle);
			geEdPt.y = gePtCenter.y+dRadius*sin(dEdAngle);

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

//去除所有的端点
void CLxPickUp::CPickUpBorder::RemoveAllVertPoint(AcDbObject*pdbObj, AcGePoint3dArray&insertPoints)
{
	if(pdbObj->isKindOf(AcDbLine::desc()))
	{
		AcGePoint3d gePoi3d ;
		AcGePoint3d gePoi3d1 ;
		gePoi3d = ((AcDbLine*)pdbObj)->startPoint();
		gePoi3d1 = ((AcDbLine*)pdbObj)->endPoint();

		for(int i=0;i<insertPoints.logicalLength();i++)
		{
			if((fabs(insertPoints.at(i).x-gePoi3d.x)<0.000001
				&&fabs(insertPoints.at(i).y-gePoi3d.y)<0.000001)
				||(fabs(insertPoints.at(i).x-gePoi3d1.x)<0.000001
				&&fabs(insertPoints.at(i).y-gePoi3d1.y)<0.000001))
			{
				insertPoints.removeAt(i);
				i--;
			}
		}
	}
	else
	{
		AcGePoint3d gePoi3d ;
		AcGePoint3d gePoi3d1 ;

		GetArcStAndEdPt((AcDbArc*)pdbObj,gePoi3d,gePoi3d1);

		for(int i=0;i<insertPoints.logicalLength();i++)
		{
			if((fabs(insertPoints.at(i).x-gePoi3d.x)<0.000001
				&&fabs(insertPoints.at(i).y-gePoi3d.y)<0.000001)
				||(fabs(insertPoints.at(i).x-gePoi3d1.x)<0.000001
				&&fabs(insertPoints.at(i).y-gePoi3d1.y)<0.000001))
			{
				insertPoints.removeAt(i);
				i--;
			}
		}
	}
}

AcDbObjectId CLxPickUp::CPickUpBorder::CreateDistLine(AcGePoint3d &gePoi3d,AcGePoint3d&gePoi3d1)
{
	AcDbDatabase * pDbDatab = acdbHostApplicationServices()->workingDatabase();
	AcDbObjectId			DbOId=NULL;
	AcDbBlockTable			* pDbBT;
	AcDbBlockTableRecord	* pDbBTR;
	AcDbEntity				* pDbEn;

	pDbDatab->getSymbolTable(pDbBT, AcDb::kForRead);
	pDbBT->getAt(ACDB_MODEL_SPACE, pDbBTR, AcDb::kForWrite);
	pDbBT->close();

	pDbEn = new AcDbLine();
	((AcDbLine*)pDbEn)->setStartPoint(gePoi3d);
	((AcDbLine*)pDbEn)->setEndPoint(gePoi3d1);

	pDbBTR->appendAcDbEntity(DbOId,pDbEn);
	pDbBTR->close();

	//pDbEn->setLayer("规整图层");
	pDbEn->close();

	return DbOId;
}

void CLxPickUp::CPickUpBorder::SortByDistance(AcGePoint3d &stPt, AcGePoint3dArray&gePoi3dArr)
{
	int nPtNum = gePoi3dArr.logicalLength();
	for(int i=nPtNum ;i>1;i--)
	{
		Bubble(stPt,gePoi3dArr,i);
	}
}
//创建直线
void CLxPickUp::CPickUpBorder::CreateFromPoints(AcDbObject*pdbObj, AcDbObjectIdArray &dbIdArray, AcGePoint3dArray &gePoi3dArr, AcDbObjectIdArray &dbIdRemove)
{
	int Num = gePoi3dArr.logicalLength();

	if(Num == 1)
	{
		AcDbObjectId dbObjId;

		AcGePoint3d gePoi3d ;
		gePoi3d = gePoi3dArr.at(0);

		AcGePoint3d gePoi3d1 = ((AcDbLine*)pdbObj)->startPoint();

		dbObjId = CreateDistLine(gePoi3d,((AcDbLine*)pdbObj)->startPoint());
		dbIdArray.append(dbObjId);

		gePoi3d1 = ((AcDbLine*)pdbObj)->endPoint();

		dbObjId = 	CreateDistLine(gePoi3d,((AcDbLine*)pdbObj)->endPoint());
		dbIdArray.append(dbObjId);

		dbObjId = pdbObj->objectId();
		dbIdArray.remove(dbObjId);

		dbIdRemove.append(dbObjId);
	}
	else
	{
		if(Num<1)
			return ;

		AcGePoint3d stPt = ((AcDbLine*)pdbObj)->startPoint();
		SortByDistance(stPt,gePoi3dArr);

		AcGePoint3d gePoi3d;
		gePoi3d = gePoi3dArr.at(0);

		AcDbObjectId dbObjId;

		dbObjId= 	CreateDistLine(stPt,gePoi3d);

		dbIdArray.append(dbObjId);

		for(int i=0;i<Num-1;i++)
		{
			AcDbObjectId dbObjId;
			AcGePoint3d gePoi3d1;
			AcGePoint3d gePoi3d2;
			gePoi3d1 = gePoi3dArr.at(i);
			gePoi3d2 = gePoi3dArr.at(i+1);

			dbObjId= 	CreateDistLine(gePoi3d1,gePoi3d2);

			dbIdArray.append(dbObjId);
		}

		gePoi3d = gePoi3dArr.at(Num-1);
		AcGePoint3d edPt = ((AcDbLine*)pdbObj)->endPoint();
		dbObjId= 	CreateDistLine(gePoi3d,edPt);

		dbIdArray.append(dbObjId);

		dbObjId = pdbObj->objectId();
		dbIdArray.remove(dbObjId);
		dbIdRemove.append(dbObjId);
	}
}

AcDbObjectId CLxPickUp::CPickUpBorder::CreateDistArc(double dStAngle,double dEndAngle,double dRadius,AcGePoint3d &gePoiCenter)
{
	AcDbDatabase * pDbDatab = acdbHostApplicationServices()->workingDatabase();
	AcDbObjectId			DbOId=NULL;
	AcDbBlockTable			* pDbBT;
	AcDbBlockTableRecord	* pDbBTR;
	AcDbEntity				* pDbEn;

	pDbDatab->getSymbolTable(pDbBT, AcDb::kForRead);
	pDbBT->getAt(ACDB_MODEL_SPACE, pDbBTR, AcDb::kForWrite);
	pDbBT->close();

	pDbEn = new AcDbArc(gePoiCenter,
		dRadius,
		dStAngle,
		dEndAngle);

	pDbBTR->appendAcDbEntity(DbOId,pDbEn);
	pDbBTR->close();

	//pDbEn->setLayer("规整图层");
	pDbEn->close();

	return DbOId;
}

void CLxPickUp::CPickUpBorder::Bubble(AcGePoint3d &stPt, AcGePoint3dArray &gePoi3dArr, int n)
{
	for(int i=0 ;i<n-1;i++)
	{
		if((pow((gePoi3dArr.at(i).x-stPt.x),2)+pow((gePoi3dArr.at(i).y-stPt.y),2))
			-(pow((gePoi3dArr.at(i+1).x-stPt.x),2)+pow((gePoi3dArr.at(i+1).y-stPt.y),2))>0.00001)
		{
			gePoi3dArr.swap(i,i+1);
		}
	}
}

void CLxPickUp::CPickUpBorder::BubbleByAngle(double dStAngle, AcArray<double> &AngArr, int n)
{
	for(int i=0 ;i<n-1;i++)
	{
		if(fabs((AngArr.at(i)-dStAngle))-fabs((AngArr.at(i+1)-dStAngle)) > 0.000001)
		{
			AngArr.swap(i,i+1);
		}
	}
}

void CLxPickUp::CPickUpBorder::SortByAngle(double dStAngle,AcArray<double> &AngleArr)
{
	int nPtNum = AngleArr.logicalLength();
	for(int i=nPtNum ;i>1;i--)
	{
		BubbleByAngle(dStAngle,AngleArr,i);
	}
}

void CLxPickUp::CPickUpBorder::CreateArcForPoints(AcDbObject*pdbObj, AcDbObjectIdArray &dbIdArray, AcGePoint3dArray &gePoi3dArr, AcDbObjectIdArray &dbIdRemove)
{
	int nNum = gePoi3dArr.logicalLength();

	if(nNum == 1)
	{
		AcDbObjectId dbObjId;

		AcGePoint3d gePoi3d;
		gePoi3d = gePoi3dArr.at(0);

		double dStAngle = ((AcDbArc*)pdbObj)->startAngle();
		double dEdAngle = ((AcDbArc*)pdbObj)->endAngle();

		AcGePoint3d gePointCenter = ((AcDbArc*)pdbObj)->center();
		double dRadius = ((AcDbArc*)pdbObj)->radius();

		//计算角度
		ads_point pt1,pt2;
		pt1[X] = gePointCenter.x;
		pt1[Y] = gePointCenter.y;
		pt1[Z] = 0.0;

		pt2[X] = gePoi3d.x;
		pt2[Y] = gePoi3d.y;
		pt2[Z] = 0.0;
		double dAngle = acutAngle(pt1,pt2);

		dbObjId = CreateDistArc(dStAngle,dAngle,dRadius,gePointCenter);
		dbIdArray.append(dbObjId);

		dbObjId = CreateDistArc(dAngle,dEdAngle,dRadius,gePointCenter);
		dbIdArray.append(dbObjId);

		dbObjId = pdbObj->objectId();
		dbIdArray.remove(dbObjId);
		dbIdRemove.append(dbObjId);
	}
	else
	{
		if(nNum<1)
			return;

		AcArray<double > AngleArr;
		ads_point pt1,pt2;
		AcGePoint3d gePointCenter = ((AcDbArc*)pdbObj)->center();
		double dEdAngle = ((AcDbArc*)pdbObj)->endAngle();

		double dRadius = ((AcDbArc*)pdbObj)->radius();

		pt1[X] = gePointCenter.x;
		pt1[Y] = gePointCenter.y;
		pt1[Z] = 0.0;

		double dAngle =0.0;

		for(int i=0; i<nNum;i++)
		{
			pt2[X] = gePoi3dArr.at(i).x;
			pt2[Y] = gePoi3dArr.at(i).y;
			pt2[Z] = 0.0;

			dAngle = acutAngle(pt1,pt2);
			AngleArr.append(dAngle);
		}

		double dStAngle = ((AcDbArc*)pdbObj)->startAngle();

		SortByAngle(dStAngle,AngleArr);

		AcDbObjectId dbObjId;
		dbObjId = CreateDistArc(dStAngle,AngleArr.at(0),dRadius,gePointCenter);
		dbIdArray.append(dbObjId);

		for(int j=0 ;j<nNum-1;j++)
		{
			dbObjId = CreateDistArc(AngleArr.at(j),AngleArr.at(j+1),dRadius,gePointCenter);
			dbIdArray.append(dbObjId);
		}

		dbObjId = CreateDistArc(AngleArr.at(nNum-1),dEdAngle,dRadius,gePointCenter);
		dbIdArray.append(dbObjId);

		dbObjId = pdbObj->objectId();
		dbIdArray.remove(dbObjId);
		dbIdRemove.append(dbObjId);
	}
}

void CLxPickUp::CPickUpBorder::CreateTempLine(AcDbObject*pdbObj, AcDbObjectIdArray& entity, AcDbObjectIdArray &dbIdArray, AcDbObjectIdArray&dbIdRemove)
{
	if(pdbObj->isKindOf(AcDbLine::desc()))
	{
		AcGePoint3dArray crossPointarr;

		AcDbObjectId dbObjId;
		AcDbObject   *pdbObjTemp;
		long lNum = entity.logicalLength();
		for(int i=0 ;i<lNum ;i++)
		{
			AcGePoint3dArray points;

			acdbOpenObject(pdbObjTemp,entity.at(i),AcDb::kForRead);
			pdbObjTemp->close();
			if(pdbObjTemp)
			{
				if(pdbObjTemp == pdbObj)
				{
					continue;
				}

				((AcDbEntity*)pdbObj)->intersectWith((AcDbEntity*)pdbObjTemp,
					AcDb::kOnBothOperands,
					points);

				crossPointarr.append(points);
			}
		}

		RemoveAllVertPoint(pdbObj,crossPointarr);
		CreateFromPoints(pdbObj,dbIdArray,crossPointarr,dbIdRemove);
	}
	else if(pdbObj->isKindOf(AcDbArc::desc()))
	{
		AcGePoint3dArray crossPointarr;

		AcDbObjectId dbObjId;
		AcDbObject   *pdbObjTemp;
		long lNum = entity.logicalLength();
		for(int i=0 ;i<lNum ;i++)
		{
			AcGePoint3dArray points;

			acdbOpenObject(pdbObjTemp,entity.at(i),AcDb::kForRead);
			pdbObjTemp->close();
			if(pdbObjTemp)
			{
				if(pdbObjTemp == pdbObj)
				{
					continue;
				}

				((AcDbEntity*)pdbObj)->intersectWith((AcDbEntity*)pdbObjTemp,
					AcDb::kOnBothOperands,
					points);
				crossPointarr.append(points);
			}
		}

		RemoveAllVertPoint(pdbObj,crossPointarr);
		CreateArcForPoints(pdbObj,dbIdArray,crossPointarr,dbIdRemove);
	}
}

//////////////////////////////////////////////////////////////////////////

CLxPickUp::CLxPickUp(void)
{
	pch = "0";
	m_OutId = NULL;
}

CLxPickUp::~CLxPickUp(void)
{
}

void CLxPickUp::mainFun()
{
	char * cMsg;
	if (DocVars.docData().m_LxObjIdArr.logicalLength() == 0)
	{
		cMsg = " 请选择边界线:\n";
	}
	else
	{
		cMsg = " 请继续选择边界线:\n";
	}
	acutPrintf(cMsg);

	if (!GetLine())
	{
		return;
	}

	if (DocVars.docData().m_LxObjIdArr.logicalLength() != 0)
	{
		GetOutPline();
	}
}

BOOL CLxPickUp::GetLine()
{
	CLxLine lxLine;
	lxLine.pch = pch;

	lxLine.GetLxLine();

	DocVars.docData().m_LxObjIdArr.append(lxLine.m_LineArr);

	if (lxLine.m_LineArr.logicalLength() != 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CLxPickUp::GetOutPline()
{
	CPickUpBorder pickUpBorder;
	pickUpBorder.pch = pch;

	m_OutId = pickUpBorder.GetOutPline(DocVars.docData().m_LxObjIdArr);

	if (m_OutId == NULL)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CLxPickUp::RemoveEntity()
{
	for (int nObjNum = 0; nObjNum < DocVars.docData().m_LxObjIdArr.logicalLength(); nObjNum++)
	{
		AcDbObjectId dbObj;
		AcDbObject *pdbObj;
		acdbOpenObject(pdbObj, DocVars.docData().m_LxObjIdArr.at(nObjNum), AcDb::kForWrite);
		if (pdbObj)
		{
			pdbObj->erase();
			pdbObj->close();
		}
	}

	DocVars.docData().m_LxObjIdArr.setLogicalLength(0);
}

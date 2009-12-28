#include "DistReduceEntity.h"
#include "DistSelSet.h" //for _DEBUG

static void OutRunInfo(const char* strMsg)
{
#ifdef _DEBUG
	acutPrintf(strMsg);
#endif

}


CDistReduceEntity::CDistReduceEntity(void)
{
}

CDistReduceEntity::~CDistReduceEntity(void)
{

}



#ifdef _DEBUG
//功能：测试例程
//参数：无
//返回：无
BOOL CDistReduceEntity::Test()
{
	AcGePoint3dArray SelectPtArray;
	SelectPtArray.setLogicalLength(0);

	//取得矩形范围的实体数据
	ads_point  startpt,endpt;    
	int nRet = acedGetPoint(NULL, "\n请指定矩形的左上角:",startpt);
	if (nRet != RTNORM)
		return FALSE;
	nRet = acedGetCorner(startpt, "\n请指定矩形的右下角:",endpt);
	if (nRet != RTNORM)
		return FALSE;

	AcGePoint3d pt;
	pt.x = startpt[0]; pt.y = startpt[1]; pt.z = 0;
	SelectPtArray.append(pt);
	
	pt.x = startpt[0]; pt.y = endpt[1]; pt.z = 0;
	SelectPtArray.append(pt);

	pt.x = endpt[0]; pt.y = endpt[1]; pt.z = 0;
	SelectPtArray.append(pt);

	pt.x = endpt[0]; pt.y = startpt[1]; pt.z = 0;
	SelectPtArray.append(pt);

	return ReduceEntities(SelectPtArray,"0");
}
#endif

BOOL CDistReduceEntity::ReduceEntitiesRect(AcGePoint3dArray SelectPtArray,const char* strLyName)
{
	AcGePoint3d ptA,ptB,ptTemp;
	ptA = SelectPtArray.at(0);
	ptB = SelectPtArray.at(1);

	double dMaxX,dMaxY,dMinX,dMinY;
	if(ptA.x>ptB.x)
	{
		dMaxX = ptA.x;
		dMinX = ptB.x;
	}
	else
	{
		dMaxX = ptB.x;
		dMinX = ptA.x;
	}

	if(ptA.y>ptB.y)
	{
		dMaxY = ptA.y;
		dMinY = ptB.y;
	}
	else
	{
		dMaxY = ptB.y;
		dMinY = ptA.y;
	}

	AcGePoint3dArray ptArrayTemp;
	ptArrayTemp.append(AcGePoint3d(dMinX,dMinY,0));
	ptArrayTemp.append(AcGePoint3d(dMinX,dMaxY,0));
	ptArrayTemp.append(AcGePoint3d(dMaxX,dMaxY,0));
	ptArrayTemp.append(AcGePoint3d(dMaxX,dMinY,0));
	ptArrayTemp.append(AcGePoint3d(dMinX,dMinY,0));

	return ReduceEntities(ptArrayTemp,strLyName);

}


//功能：根据边界多边形对实体进行裁减
//参数：SelectPtArray  边界数组(点数组一律视为闭合，如果不闭合会增加一个首点；所以需要值传递)
//      strLyName      操作图层名称，如果为NULL表示操作所有图层
//返回：成功返回 TRUE 失败返回 FALSE
BOOL CDistReduceEntity::ReduceEntities(AcGePoint3dArray SelectPtArray,const char* strLyName/*=NULL*/)
{
	double dminX=4294967295,dminY=4294967295,dmaxX=-4294967295,dmaxY=-4294967295;
	for(int n=0; n<SelectPtArray.logicalLength(); n++)
	{
		AcGePoint3d tempPt = SelectPtArray.at(n);
		if(tempPt.x > dmaxX)
			dmaxX = tempPt.x;
		if(tempPt.x < dminX)
			dminX = tempPt.x;
		if(tempPt.y > dmaxY)
			dmaxY = tempPt.y;
		if(tempPt.y < dminY)
			dminY = tempPt.y;
	}

	ads_point ptCenter;
	ptCenter[0] = (dmaxX-dminX)/2;
	ptCenter[1] = (dmaxY-dminY)/2;
	ptCenter[2] = 0;
	if(dmaxX-dminX > dmaxY - dminY)
		ArxZoom(ptCenter,dmaxX-dminX + 100);
	else
		ArxZoom(ptCenter,dmaxY - dminY + 100);

	/*AcDbViewTableRecord viewTR;
	AcGePoint2d ptCenter;
	ptCenter.x = (dmaxX-dminX)/2;
	ptCenter.y = (dmaxY-dminY)/2;
	viewTR.setCenterPoint(ptCenter);
	viewTR.setWidth(dmaxX-dminX+40);
	viewTR.setHeight(dmaxY-dminY+40);
	acedSetCurrentView(&viewTR, NULL);*/

	//确保点数组闭合
	if(SelectPtArray[0] != SelectPtArray[SelectPtArray.length()-1]) 
		SelectPtArray.append(SelectPtArray[0]);

	//合法性检查
	int nSelectPtCount = SelectPtArray.length();
	if(nSelectPtCount < 4) return FALSE;

	//创建过滤器
	struct resbuf *filter=NULL;
	if(NULL != strLyName)
	{
		filter=acutNewRb(AcDb::kDxfLayerName);
		filter->restype=AcDb::kDxfLayerName;
		filter->resval.rstring = (char*) malloc((strlen(strLyName)+1) * sizeof(char));
		strcpy(filter->resval.rstring,strLyName);
		filter->rbnext=NULL;
	}

	//选择交叉实体(原来方案是CP － WP 效率太低；修改为依次遍历每条边求交叉实体并剔除重复)
	int i,j,k,m;
	CDistSelSet TempSelect;
	AcDbObjectIdArray ObjArrayCross,tempObjArray;
	ObjArrayCross.setLogicalLength(0);
	
	for(i=0; i<nSelectPtCount-1; i++)
	{
		int nCrossLen = ObjArrayCross.length();
		AcGePoint3d ptA,ptB;
		ptA = SelectPtArray.at(i); ptB = SelectPtArray.at(i+1);
		if(TempSelect.CrossingSelect(ptA,ptB,filter) == CDistSelSet::kDistSelected)
		{
			tempObjArray.setLogicalLength(0);
			TempSelect.AsArray(tempObjArray);
			int nTempLen = tempObjArray.length();
			for(j=0;j<nTempLen;j++)
			{
				BOOL bFound = FALSE;
				AcDbObjectId tempId = tempObjArray.at(j);
				for(k=0; k<nCrossLen;k++)
				{
					if(ObjArrayCross.at(k) == tempId)
					{
						bFound = TRUE;
						break;
					}
				}
				if(bFound==FALSE)
					ObjArrayCross.append(tempId);
			}
		}
	}
	if(NULL != filter)
	{
		free(filter->resval.rstring);
		acutRelRb(filter);
		filter = NULL;
	}

	int nLen = ObjArrayCross.length();
	if(nLen == 0) return TRUE; //没有相交实体，直接退出

	AcDbObjectId tempId;
	//创建边界实体（计算求交）
	SelectPtArray.removeAt(SelectPtArray.length()-1);
	AcDb2dPolyline *pline=new AcDb2dPolyline(AcDb::k2dSimplePoly,SelectPtArray,0,Adesk::kTrue); 

	if(NULL == pline)
	{
		OutRunInfo("\n Malloc memory error!"); return FALSE;
	}

	
	//依次遍历每一个相交实体
	for(i=0; i<nLen; i++)
	{
		AcDbEntity* pEnt = NULL;
		tempId = ObjArrayCross.at(i);

		//打开实体
		if(Acad::eOk != acdbOpenObject(pEnt,tempId,AcDb::kForWrite))
		{
			char strInfo[255] ={0};
			sprintf(strInfo,"\n Open entity %d error!",tempId.asOldId());
			OutRunInfo(strInfo);continue;
		}

		//过滤实体类型，只处理从AcDbCurve类继承的子类               
		if((pEnt->isKindOf(AcDb2dPolyline::desc()) == false) && 
		   (pEnt->isKindOf(AcDb3dPolyline::desc()) == false) && 
		   (pEnt->isKindOf(AcDbArc::desc()) == false) && 
		   (pEnt->isKindOf(AcDbCircle::desc()) == false) && 
		   (pEnt->isKindOf(AcDbEllipse::desc()) == false) && 
		   (pEnt->isKindOf(AcDbLeader::desc()) == false) && 
		   (pEnt->isKindOf(AcDbLine::desc()) == false) && 
		   (pEnt->isKindOf(AcDbPolyline::desc()) == false) && 
		   (pEnt->isKindOf(AcDbRay::desc()) == false) && 
		   (pEnt->isKindOf(AcDbSpline::desc()) == false) && 
		   (pEnt->isKindOf(AcDbXline::desc()) == false))
		{
			pEnt->close(); continue;
		}

		//类型转换，操作的统一
		AcDbCurve* pCurve = NULL;
		if(pEnt->isKindOf(AcDb2dPolyline::desc()) || pEnt->isKindOf(AcDb3dPolyline::desc()))
		{
			AcDbSpline* spline = NULL;   //这两种类型实体需要转换，才能 getSplitCurves 成功
			pEnt->handOverTo(spline);
			pCurve = (AcDbCurve*) spline;
		}
		else
			pCurve = (AcDbCurve*)pEnt;  


		//曲线求交
		AcGePoint3dArray CrossPtArray;
		CrossPtArray.setLogicalLength(0);
		if(Acad::eOk != pline->intersectWith(pCurve,AcDb::kOnBothOperands ,CrossPtArray))
		{
			pEnt->close();
			OutRunInfo("\n Intersect entity error!"); continue;
		}
		int nLenCross = CrossPtArray.length();
		if(nLenCross == 0 ) 
		{
			pEnt->close();
			OutRunInfo("\n entities have no IntersectPoint!"); continue;
		}

		
		//根据坐标点，求 Param 并对结果排序（从小到大）
		double dParam = 0.0;
		AcGeDoubleArray parameterArray;
		for(k=0; k<nLenCross; k++)
		{
			pCurve->getParamAtPoint(CrossPtArray.at(k),dParam);
			parameterArray.append(dParam);

			for(m=k; m>0; m--)
			{
				if(parameterArray.at(m-1) - dParam > 0.001)
				{
					parameterArray.setAt(m,parameterArray.at(m-1));
					parameterArray.setAt(m-1,dParam);
				}
				else
					break;
			}
		}
		

		//求子曲线
		AcDbVoidPtrArray curveSeg;
		curveSeg.setLogicalLength(0);
		if(Acad::eOk != pCurve->getSplitCurves(parameterArray,curveSeg))  //使用该函数参数形式，准确性高
		{
			pEnt->close();
			OutRunInfo("\n Get split curves error!"); continue;
		}


		//将合法的子曲线添加到模型空间
		int nSegCount = curveSeg.length();
		for (j=0;j<nSegCount;j++) 
		{
			double dS =0.0,dE = 0.0;
			AcGePoint3d ptS,ptE,ptM;
			AcDbCurve* pSubCruve = (AcDbCurve*)curveSeg[j];
			pSubCruve->getStartPoint(ptS);
			pSubCruve->getEndPoint(ptE);
			pSubCruve->getDistAtPoint(ptS,dS);
			pSubCruve->getDistAtPoint(ptE,dE);
			pSubCruve->getPointAtDist((dE-dS)/2,ptM);  //求子曲线的距离中点

			if(IsPointInPolygon(SelectPtArray,ptM.x,ptM.y)==TRUE)  //判断点是否在多边形内
			{
				//根据母实体，设置子实体的相关属性
				pSubCruve->setLayer(pEnt->layer());
				pSubCruve->setLinetype(pEnt->linetype());
				pSubCruve->setColorIndex(pEnt->colorIndex());

				//实体添加到模型空间
				AcDbObjectId resultId;
				AddToCurrentSpace(resultId,pSubCruve);
			}
			pSubCruve->close();
		}
		pEnt->erase();
		pEnt->close();
	}
	pline->close();
	delete pline;  //释放范围线

	return TRUE;
}


//功能：将当前实体添加到模型空间
//参数：resultId  返回添加实体的ID
//      pEnt      实体指针
//      pDB       当前文档数据库指针
//返回：添加实体成功返回 TRUE 否则返回 FALSE  
BOOL CDistReduceEntity::AddToCurrentSpace(AcDbObjectId& resultId,AcDbEntity* pEnt,AcDbDatabase* pDB)
{	
	resultId.setNull();

	//打开表
	AcDbBlockTableRecord* pBlkRec = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pBlkRec, pDB->currentSpaceId(), AcDb::kForWrite);
	if (es != Acad::eOk)
	{
		OutRunInfo("\n Open model space error！");
		return FALSE;
	}

	//添加实体
	es = pBlkRec->appendAcDbEntity(resultId,pEnt);
	if (es != Acad::eOk) {
		OutRunInfo("\n Add entity to model space error！");
		return FALSE;
	}
	pBlkRec->close();

	return TRUE;
}

//函数功能：判断两线段是否相交
//入口参数：
//          px1,py1,px2,py2   线段P1P2
//          qx1,qy1,qx2,qy2   线段Q1Q2
//返回值  ：如果相交返回TRUE 否则返回FALSE 。
////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDistReduceEntity::IsLineSectIntersectLineSect(double px1,double py1,double px2,double py2,
								                    double qx1,double qy1,double qx2,double qy2)
{
	if((((px1 - qx1) * (qy2 - qy1) - (py1 - qy1) * (qx2 - qx1)) * 
		((qx2 - qx1) * (py2 - qy1) - (qy2 - qy1) * (px2 - qx1)) > 0) &&
		(((qx1 - px1) * (py2 - py1) - (qy1 - py1) * (px2 - px1)) * 
		((px2 - px1) * (qy2 - py1) - (py2 - py1) * (qx2 - px1)) > 0) )
		return TRUE;
	else
		return FALSE;
}


//函数功能：判断点是否在线段上
//入口参数：
//		px1,py1,px2,py2  线段P1P2
//      qx,qy            点Q
//返回值：如果点在线上返回TRUE   否则返回FALSE 。
///////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDistReduceEntity::IsPointOnLine(double px1,double py1,double px2,double py2,double qx,double qy)
{
	if(fabs((px1-qx)*(px1-qx)+(py1-qy)*(py1-qy) + (px2-qx)*(px2-qx)+(py2-qy)*(py2-qy) 
		- (px1-px2)*(px1-px2) -(py1-py2)*(py1-py2))<0.0001)
		return TRUE;
	else
		return FALSE;

}


//函数功能：判断点是否在多边形中
//入口参数：
//          pts  多边形点集合
//          x,y  点坐标
//返回值  ：如果点在多边形内返回 TRUE 否则返回 FALSE       
/////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL  CDistReduceEntity::IsPointInPolygon(AcGePoint3dArray& pts,double x,double y)
{
	int nCount  = 0;  //多边形顶点个数
	int nNumber = 0;  //记录射线与多边形交点的个数

	//沿点向左的射线L
	double l[4];
	l[0] = x;
	l[1] = y;
	l[2] = x -10000000; //射线长度
	l[3] = y;

	double S[4];//多边形的一条边

	nCount = pts.length();
	if(fabs(pts[0].x-pts[nCount-1].x)>0.001 || fabs(pts[0].y-pts[nCount-1].y)>0.001)
		pts.append(pts[0]);  //确保点数组闭合
	AcGePoint3d pt;
	double x0=0,y0=0;
	for(int i = 0;i<=nCount - 2;i++)
	{
		//取多边形的一条边（实际是两个点的坐标）
		pt = pts.at(i);
		S[0] = pt.x;
		S[1] = pt.y;
		pt = pts.at(i+1);
		S[2] = pt.x;
		S[3] = pt.y;

		//判断点(x,y)是否在多边形边上
		if(IsPointOnLine(S[0], S[1], S[2], S[3],x,y)==TRUE)
		{
			return TRUE;
		}

		if(S[1]!= S[3])
		{

			//取纵坐标较大的点
			if (S[1] > S[3])
			{
				x0 = S[0];
				y0 = S[1];
			}
			else
			{
				x0 = S[2];
				y0 = S[3];
			}
		}


		//判断该点是否在射线L上
		if(IsPointOnLine(l[0], l[1], l[2], l[3], x0,y0) == TRUE) 
			nNumber = nNumber + 1;

		//s和射线L相交
		if(IsLineSectIntersectLineSect(S[0],S[1],S[2],S[3],l[0],l[1],l[2],l[3])==TRUE)
			nNumber = nNumber + 1;
	}

	if(nNumber % 2 == 1)
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}


void CDistReduceEntity::ArxZoom(ads_point pt,double magnf)
{
	AcDbViewTableRecord view;
	struct resbuf rb;
	struct resbuf wcs, dcs, ccs; // acedTrans coord system flags
	ads_point vpDir;
	ads_point ptmax;
	AcGeVector3d viewDir;
	AcGePoint2d cenPt;
	ads_real lenslength, viewtwist, frontz, backz;
	ads_point target;
	int /*viewmode,*/tilemode, cvport; // Get window to zoom to:

	wcs.restype = RTSHORT; // WORLD coord system flag
	wcs.resval.rint = 0;
	ccs.restype = RTSHORT; // CURRENT coord system flag
	ccs.resval.rint = 1;
	dcs.restype = RTSHORT; // DEVICE coord system flag
	dcs.resval.rint = 2; // Get the 'VPOINT' direction vector

	acedGetVar("VIEWDIR", &rb);
	acedTrans(rb.resval.rpoint, &ccs, &wcs, 0, vpDir);
	viewDir.set(vpDir[X], vpDir[Y], vpDir[Z]);
	acedTrans(pt, &ccs, &dcs, 0, ptmax);

	// set center point of view
	cenPt[X] = ptmax[X];
	cenPt[Y] = ptmax[Y];
	view.setCenterPoint(cenPt);
	view.setHeight(magnf*1.2);
	view.setWidth(magnf*1.2);
	view.setViewDirection(viewDir);

	// get and set other properties
	acedGetVar("LENSLENGTH", &rb);
	lenslength = rb.resval.rreal;
	view.setLensLength(lenslength);

	acedGetVar("VIEWTWIST", &rb);
	viewtwist = rb.resval.rreal;
	view.setViewTwist(viewtwist);

	acedGetVar("FRONTZ", &rb);
	frontz = rb.resval.rreal;

	acedGetVar("BACKZ", &rb);
	backz = rb.resval.rreal;

	view.setPerspectiveEnabled(false);
	view.setFrontClipEnabled(false);
	view.setBackClipEnabled(false);
	view.setFrontClipAtEye(false);

	acedGetVar("TILEMODE", &rb);
	tilemode = rb.resval.rint;

	acedGetVar("CVPORT", &rb);
	cvport = rb.resval.rint;

	// Paperspace flag
	bool paperspace = ((tilemode == 0) && (cvport == 1)) ? Adesk::kTrue
		:Adesk::kFalse;
	view.setIsPaperspaceView(paperspace);
	if (Adesk::kFalse == paperspace) {
		view.setFrontClipDistance(frontz);
		view.setBackClipDistance(backz);
	}
	else {
		view.setFrontClipDistance(0.0);
		view.setBackClipDistance(0.0);
	}

	acedGetVar("TARGET", &rb);
	acedTrans(rb.resval.rpoint, &ccs, &wcs, 0, target);
	view.setTarget(AcGePoint3d(target[X], target[Y], target[Z]));

	// update view
	acedSetCurrentView( &view, NULL );
}







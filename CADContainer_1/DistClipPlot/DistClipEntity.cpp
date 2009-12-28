#include "StdAfx.h"
#include "DistClipEntity.h"
#include "DistSelSet.h" 
#include "DistEntityTool.h"
#include "..\SdeData\DistBaseInterface.h"
#include <time.h>
#include "DistXData.h"
#include "whXMLFile.h"

static void OutRunInfo(const char* strMsg)
{
#ifdef _DEBUG
	acutPrintf(strMsg);
#endif

}

static CString GetCurArxAppPath(CString strArxName)
{
	TCHAR exeFullPath[MAX_PATH]; 
	CString strPath; 
	GetModuleFileName(GetModuleHandle(strArxName),exeFullPath,MAX_PATH); 
	strPath=(CString)exeFullPath; 
	int position=strPath.ReverseFind('\\'); 
	strPath=strPath.Left(position+1); 
	return strPath;
}


BOOL  ReadCfgLyNameArray(CString& strFileCT,CString& strPathPrj)
{
	CString strPath;
	strPath.Format("%sPathInfo.xml",GetCurArxAppPath("shYDsys.arx"));
	CXMLFile file(strPath.GetBuffer(0));
	strFileCT = file.GetString("路径","草图文件","").c_str();
	strPathPrj = file.GetString("路径","项目路径","C:\\").c_str();

	return TRUE;
}




void OpenDocHelper(void* path)
{
	acDocManager->appContextOpenDocument((const char*)path);

}

void CloseDocHelper(void* vp)
{
	AcApDocument* pDoc = (AcApDocument*)vp;
	if(pDoc)
	{
		acDocManager->closeDocument(pDoc);
	}
}


//拖动实体预览并移动位置
static int Draggen(LPCTSTR lpszPrompt,AcDbObjectIdArray& aryids,AcGePoint3d& ptCenter);


CDistClipEntity::CDistClipEntity(void)
{
}

CDistClipEntity::~CDistClipEntity(void)
{
}


void CDistClipEntity::ReadDwgCT()
{
	CString strFileCT,strPathPrj;
	ReadCfgLyNameArray(strFileCT,strPathPrj);

	CFileStatus fileStat;
	if(CFile::GetStatus(strFileCT, fileStat) == 0)
	{
		struct resbuf *rb = acutNewRb(RTSTR);
		if(RTNORM == acedGetFileD("请选择工作草图文件","C:\\工作草图.dwg", "dwg", 0, rb))
		{
			strFileCT = rb->resval.rstring;
			acutRelRb(rb);

			CString strFName ;
			strFName.Format("%sPathInfo.xml",GetCurArxAppPath("DistClipPlot.arx"));
			CXMLFile temp(strFName);
			temp.SetString("路径","草图文件",strFileCT);
			temp.save();
		}
		else
		{
			acutPrintf("\n操作被放弃！"); acutRelRb(rb); return;
		}
	}


	if(acDocManager->isApplicationContext())
	{
		//CloseDocHelper((void*)curDoc());
		OpenDocHelper((void*)_T(strFileCT.GetBuffer(0)));
	}
	else
	{
		//acDocManager->executeInApplicationContext(CloseDocHelper, (void*)curDoc());
		acDocManager->executeInApplicationContext(OpenDocHelper, (void*)_T(strFileCT.GetBuffer(0)));
	}
	CDistEntityTool tempTool;
	tempTool.SendCommandToAutoCAD("_ZOOM E ");

}

BOOL CDistClipEntity::SaveToDwgFile(CString strFName,AcDbObjectIdArray& ObjIdArray)
{
	AcDbDatabase *pDb = new AcDbDatabase();
	if(pDb == NULL) return FALSE;
	
	if(Acad::eOk != acdbHostApplicationServices()->workingDatabase()->wblock(pDb,ObjIdArray,AcGePoint3d(0,0,0)))
	{
		delete pDb; return FALSE;	
	}

	if(Acad::eOk != pDb->saveAs(strFName))
	{
		delete pDb; return FALSE;
	}
	delete pDb;

	return TRUE;
}


BOOL CDistClipEntity::PlotEx()
{
	CFileDialog dlg(FALSE,"*.dwg","项目蓝线范围.dwg",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CAD文件|*.dwg");
	if( dlg.DoModal() != IDOK) return FALSE; 
	CString strFName = dlg.GetPathName();

	
	AcDbObjectId resultId;
	CDistEntityTool tempTool;
	tempTool.CreateLayer(resultId,"DIST_TK",5);

	AcGePoint3d ptMin,ptMax;
	if(acedGetPoint(NULL,"\n指定第一点:",asDblArray(ptMin))!=RTNORM)
		return FALSE;
	if(acedGetCorner(asDblArray(ptMin),"\n指定第二点",asDblArray(ptMax))!=RTNORM)
		return FALSE;
	double dmaxX,dmaxY,dminX,dminY;
	if(ptMin.x < ptMax.x){
		dminX = ptMin.x; dmaxX = ptMax.x;
	}
	else{
		dmaxX = ptMin.x; dminX = ptMax.x;
	}
	if(ptMin.y < ptMax.y){
		dminY = ptMin.y; dmaxY = ptMax.y;
	}
	else{
		dmaxY = ptMin.y; dminY = ptMax.y;
	}
	AcGePoint3dArray SelectPtArray;
	SelectPtArray.setPhysicalLength(0);
	double dScaleX = (dmaxX - dminX)/390.2713;
	double dScaleY = (dmaxY - dminY)/281.1951;

	AcGePoint3d basePt(dminX+3.8505*dScaleX,dminY+5.5550*dScaleY,0);
    CalcRectByScale(dScaleX,dScaleY,basePt,SelectPtArray);
	basePt.x = dminX; basePt.y = dminY;basePt.z = 0;
	

	CString strScale;	
	AcDbObjectIdArray ObjIdArray;
	ObjIdArray.setPhysicalLength(0);

	AcDbObjectIdArray tempObjIdArray;
	ClipAndCopy(tempObjIdArray,"DIST_TK",SelectPtArray,NULL);
	ObjIdArray.append(tempObjIdArray);


	CStringArray strTagArray,strTextArray;
	strTagArray.Add("日期");strTagArray.Add("流程号");strTagArray.Add("类型");strTagArray.Add("项目名称");
	strTagArray.Add("建设单位");strTagArray.Add("总面积");strTagArray.Add("建设用地面积");strTagArray.Add("道路面积");
	strTagArray.Add("绿化面积");strTagArray.Add("其它面积");strTagArray.Add("市规划局");

	strTextArray.RemoveAll();
	GetProjectInfo(strTextArray,SelectPtArray);


	// 临时
	strTextArray.Add("    年   月   日");strTextArray.Add("流程号：         ");strTextArray.Add("选址");strTextArray.Add("项目名称：                ");
	strTextArray.Add("建设单位：            ");strTextArray.Add("总面积 ：         ");strTextArray.Add("建设用地 A1：        ");strTextArray.Add("道路用地 A2：    .   ");
	strTextArray.Add("绿化用地 A3：0.00");strTextArray.Add("其它用地 A4：0.00");strTextArray.Add("    市规划局");



	if(SaveToDwgFile(strFName,ObjIdArray)!=TRUE)
		acutPrintf("\n输出文件失败!");
	else
		acutPrintf("\n输出文件成功!");


	Draggen("\n选择插入点",ObjIdArray,basePt);//ptCenter

	CreateBlockByFile();
	strScale.Format("%0.2f#%0.2f#%0.2f#",dScaleX,dScaleY,1.00);
	tempTool.CreateBlkRefAttr(resultId,basePt,"图框","DIST_TK",5,strScale.GetBuffer(0),0,strTagArray,strTextArray);
	ObjIdArray.append(resultId);


	static int gGroupX = 1;
	AcDbObjectId tempId;
	CString strGrpName;
	strGrpName.Format("WANG%d",gGroupX);
	if(CreateGroup(tempId,ObjIdArray,strGrpName.GetBuffer())) gGroupX++;

	return TRUE;


}




//功能：测试例程
//参数：无
//返回：无
static int gGroup = 0;
BOOL CDistClipEntity::Plot()
{
	CFileDialog dlg(FALSE,"*.dwg","项目蓝线范围.dwg",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"CAD文件|*.dwg");
	if( dlg.DoModal() != IDOK) return FALSE; 
	AcGePoint3dArray SelectPtArray;
	SelectPtArray.setLogicalLength(0);
	AcDbObjectIdArray ObjIdArray;

	AcDbObjectId resultId;
	CDistEntityTool tempTool;
	tempTool.CreateTextStyle(resultId,"宋体","宋体","宋体");
	tempTool.CreateLayer(resultId,"DIST_TK",5);

	CStringArray strValueArray;
	strValueArray.Add("[      ] 蓝线示意图        流程号");
	strValueArray.Add("项目名称");
	strValueArray.Add("建设单位");
	strValueArray.Add("总面积");
	strValueArray.Add("建设用地（A1）");
	strValueArray.Add("道路用地（A2）");
	strValueArray.Add("绿化用地（A3）");
	strValueArray.Add("其他用地（A4）");
	// 临时
	strValueArray.Add("    市规划局");
	strValueArray.Add("2007年08月24日");

	if(DrawCustomGrid(SelectPtArray,ObjIdArray,"DIST_TK",12,5,50,strValueArray)!=TRUE) return FALSE;

	AcDbObjectIdArray tempObjIdArray;
	ClipAndCopy(tempObjIdArray,"DIST_TK",SelectPtArray,NULL);


	if(SaveToDwgFile(dlg.GetPathName(),tempObjIdArray)!=TRUE)
		acutPrintf("\n输出文件失败!");


	ObjIdArray.append(tempObjIdArray);
	AcGePoint3d ptCenter = SelectPtArray.at(0);
	Draggen("\n选择插入点",ObjIdArray,ptCenter);


	AcDbObjectId tempId;
	CString strGrpName;
	strGrpName.Format("WANG%d",gGroup);
	if(CreateGroup(tempId,ObjIdArray,strGrpName.GetBuffer()))
		gGroup++;

	return TRUE;
}



//功能：根据边界多边形对实体进行裁减复制
//参数：ObjIdArrayReturn 返回裁减范围内的实体ID
//      SelectPtArray    边界数组(点数组一律视为闭合，如果不闭合会增加一个首点；所以需要值传递)
//      strLyName        操作图层名称，如果为NULL表示操作所有图层
//返回：成功返回 TRUE 失败返回 FALSE
BOOL CDistClipEntity::ClipAndCopy(AcDbObjectIdArray& ObjIdArrayReturn,const char* strLyName,AcGePoint3dArray SelectPtArray,const char* strLyNameSelect/*=NULL*/)
{
	AcAxDocLock lock;
	ObjIdArrayReturn.setLogicalLength(0);

	//确保点数组闭合(即：首尾点相等)
	if(SelectPtArray[0] != SelectPtArray[SelectPtArray.length()-1]) 
		SelectPtArray.append(SelectPtArray[0]);

	//合法性检查
	int nSelectPtCount = SelectPtArray.length();
	if(nSelectPtCount < 4) return FALSE;

	CStringArray strLyNameArray,strClrIndexArray;
	CDistEntityTool tempTool;
	tempTool.GetAllLayerName(strLyNameArray,strClrIndexArray);

	//创建过滤器(根据图层过滤)
	struct resbuf *filter=NULL;
	if(NULL != strLyNameSelect)
	{
		filter=acutNewRb(AcDb::kDxfLayerName);
		filter->restype=AcDb::kDxfLayerName;
		filter->resval.rstring = (char*) malloc((strlen(strLyName)+1) * sizeof(char));
		strcpy(filter->resval.rstring,strLyNameSelect);
		filter->rbnext=NULL;
	}


	//选择范围内实体，不包含交叉
	CDistSelSet TempSelect;
	AcDbObjectIdArray ObjArrayCross,tempObjArray;
	AcDbObjectIdArray ObjArrayIn,ObjArrayFrond;

	ObjArrayIn.setLogicalLength(0);
	if(TempSelect.WindowPolygonSelect(SelectPtArray,filter) == CDistSelSet::kDistSelected)
	{
		TempSelect.AsArray(ObjArrayIn);
		int nObjNum = ObjArrayIn.length();
		AcDbObjectId tempId;
		for(int kk=nObjNum-1; kk>=0; kk--)
		{
			tempId = ObjArrayIn.at(kk);
			AcDbEntity* pEnt = NULL;
			if(Acad::eOk != acdbOpenObject(pEnt,tempId,AcDb::kForRead)) continue;
			CString strLy = pEnt->layer();
			if(strLy.Find("蓝线")>=0) //主要是为了调整显示次序
			{
				ObjArrayFrond.append(tempId);
				ObjArrayIn.removeAt(kk);
			}
			pEnt->close();
		}
		ObjArrayIn.append(ObjArrayFrond);
	}
	

	//选择交叉实体(原来方案是CP － WP 效率太低；修改为依次遍历每条边求交叉实体并剔除重复)
	int i,j,k,m;
	ObjArrayCross.setLogicalLength(0);

	if(TempSelect.FenceSelect(SelectPtArray,filter)== CDistSelSet::kDistSelected)
	{
		TempSelect.AsArray(ObjArrayCross);

	}

	//释放空间
	if(NULL != filter)
	{
		free(filter->resval.rstring);
		filter->resval.rstring = NULL;
		acutRelRb(filter);
		filter = NULL;
	}
	int nLen = ObjArrayCross.length();
	if(nLen > 0)  
	{
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
				pEnt->close(); continue;
			}
			int nLenCross = CrossPtArray.length();
			if(nLenCross == 0 ) 
			{
				pEnt->close(); continue;
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
			int nClrIndex = 0;
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
					pSubCruve->setLayer(strLyName);
					pSubCruve->setLinetype(pEnt->linetype());
					nClrIndex = pEnt->colorIndex();
					if(nClrIndex == 256)
					{
						for(int q=0; q<strLyNameArray.GetCount(); q++)
						{
							CString strTempLyName = strLyNameArray.GetAt(q);
							char* pLyName = pEnt->layer();
							if(strTempLyName.CompareNoCase(pLyName) == 0)
							{
								nClrIndex = atoi(strClrIndexArray.GetAt(q)); break;
							}
						}
					}
					pSubCruve->setColorIndex(nClrIndex);

					//实体添加到模型空间
					AcDbObjectId resultId;
					AddToCurrentSpace(resultId,pSubCruve);
					ObjIdArrayReturn.append(resultId); //将实体添加到返回数组
				}
				pSubCruve->close();
			}
			pEnt->close();
		}
		pline->close();
		delete pline;  //释放范围线
	}

	
	nLen = ObjArrayIn.length();

	//将内部的块引用分离出来,单独复制
	AcDbObjectId tempId;
	AcDbObjectIdArray BlkRefArray,tempBlkRefArray;
	BlkRefArray.setLogicalLength(0);
	for(int kk =nLen-1 ;kk>=0; kk--)
	{
		AcDbEntity * pEnt = NULL;
		tempId = ObjArrayIn.at(kk);
		if(Acad::eOk != acdbOpenObject(pEnt,tempId,AcDb::kForRead)) continue;
		if(pEnt->isKindOf(AcDbBlockReference::desc()))
		{
			BlkRefArray.append(tempId);
			ObjArrayIn.removeAt(kk);
		}
		pEnt->close();
	}
	tempBlkRefArray.setLogicalLength(0);
	CloneBlockRefAttr(tempBlkRefArray,"DIST_TK",BlkRefArray);

	tempObjArray.setLogicalLength(0);
	CloneEntity(tempObjArray,"DIST_TK",ObjArrayIn,NULL);  //复制范围内部实体(不包含块引用)

	tempObjArray.append(tempBlkRefArray);
	ObjIdArrayReturn.append(tempObjArray);

	

	return TRUE;
}


void CDistClipEntity ::CloneEntity(AcDbObjectIdArray& newObjIdArray,const char* strLyName,AcDbObjectIdArray oldObjIdArray,AcDbObjectId* pOwnerObjId/*=NULL*/)
{
	newObjIdArray.setLogicalLength(0);
	if(oldObjIdArray.length()==0) return; //没有实体可复制

	CStringArray strLyNameArray,strClrIndexArray;
	CDistEntityTool tempTool;
	tempTool.GetAllLayerName(strLyNameArray,strClrIndexArray); //获取全部图层名和颜色

	AcDbDatabase *pDB=acdbHostApplicationServices()->workingDatabase();
	AcDbObjectId tempOwnerId;
	if(NULL == pOwnerObjId)
	{
		AcDbBlockTable *pBlockTable=NULL;
		pDB->getSymbolTable(pBlockTable, AcDb::kForRead);
		pBlockTable->getAt(ACDB_MODEL_SPACE, tempOwnerId);
		pBlockTable->close();
	}
	else
		tempOwnerId = *pOwnerObjId;

	AcDbIdPair IdPair;
	AcDbIdMapping Idmap;
	pDB->deepCloneObjects(oldObjIdArray,tempOwnerId,Idmap); 
    
    AcDbIdMappingIter iter(Idmap);
    for (iter.start(); !iter.done(); iter.next())
	{
        if (iter.getMap(IdPair)==false) continue; 

		AcDbObjectId CloneId = IdPair.value();
		AcDbObjectId KeyId = IdPair.key();

		AcDbEntity* pEnt = NULL;
		if(Acad::eOk == acdbOpenObject(pEnt,CloneId,AcDb::kForWrite))
		{
			int nClrIndex = pEnt->colorIndex();
			if(nClrIndex == 256)
			{
				for(int q=0; q<strLyNameArray.GetCount(); q++)
				{
					CString strTempLyName = strLyNameArray.GetAt(q);
					char* pLyName = pEnt->layer();
					if(strTempLyName.CompareNoCase(pLyName) == 0)
					{
						nClrIndex = atoi(strClrIndexArray.GetAt(q)); break;
					}
				}
			}
			pEnt->setColorIndex(nClrIndex);
			pEnt->setLayer(strLyName);
			pEnt->close();
			newObjIdArray.append(CloneId);
		}
    }  
} 


void CDistClipEntity::CloneBlockRefAttr(AcDbObjectIdArray& newObjIdArray,const char* strLyName,AcDbObjectIdArray oldObjIdArray)
{
	newObjIdArray.setLogicalLength(0);
	int nLen = oldObjIdArray.length();
	if(nLen == 0) return; //没有实体可复制

	
	CStringArray strLyNameArray,strClrIndexArray;
	CDistEntityTool tempTool;
	tempTool.GetAllLayerName(strLyNameArray,strClrIndexArray); //获取全部图层名和颜色


	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	AcDbObjectId tempId;
	for(int i=0; i<nLen; i++)
	{
		//打开源块引用
		AcDbBlockReference* pBlkRefOld = NULL;
		tempId = oldObjIdArray.at(i);
		if(Acad::eOk != acdbOpenObject(pBlkRefOld,tempId,AcDb::kForRead)) continue;
		AcDbObjectId blockId = pBlkRefOld->blockTableRecord();

		//新建块引用
		AcDbBlockReference *pBlkRefNew = new AcDbBlockReference;
		pBlkRefNew->setBlockTableRecord(blockId);//指定所引用的图块表记录的对象ID
		resbuf to,from;
		from.restype=RTSHORT;//插入图块要进行用户坐标与世界坐标的转换
		from.resval.rint=1;  //UCS
		to.restype=RTSHORT;
		to.resval.rint=0;    //WCS
		AcGeVector3d normal(0.0,0.0,1.0);
		acedTrans(&(normal.x),&from,&to,Adesk::kTrue,&(normal.x));//转换函数
		pBlkRefNew->setRotation(pBlkRefOld->rotation()); //旋转角度
		pBlkRefNew->setPosition(pBlkRefOld->position()); //指定基点
		pBlkRefNew->setNormal(normal);
		pBlkRefNew->setLayer(strLyName);//图层名称
		pBlkRefNew->setColorIndex(pBlkRefOld->colorIndex());
		pBlkRefNew->setScaleFactors(pBlkRefOld->scaleFactors());//放大比例


		//AddToCurrentSpace(tempId,pBlkRefNew,pDB);


		//添加到模型空间
		AcDbBlockTable *pBlockTable = NULL;
		pDB->getSymbolTable(pBlockTable,AcDb::kForRead);//取块表指针

		AcDbBlockTableRecord *pBlockTableRecord=NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
		pBlockTable->close();

		AcDbObjectId resultId;
		pBlockTableRecord->appendAcDbEntity(resultId,pBlkRefNew);
		pBlockTableRecord->close();
		newObjIdArray.append(resultId);

		
		AcDbBlockTableRecord *pBlkDefRcd=NULL;
		if(Acad::eOk != acdbOpenObject(pBlkDefRcd, blockId, AcDb::kForRead))
		{
			return;
		}
		if(pBlkDefRcd->hasAttributeDefinitions()==false) 
		{
			pBlkDefRcd->close();
			pBlkRefNew->close();
			pBlkRefOld->close();
			continue;
		}


		AcDbObjectIterator * pObjIter = pBlkRefOld->attributeIterator();
		if (pObjIter == NULL) 
		{
			pBlockTableRecord->close();
			pBlkRefNew->close();
			pBlkRefOld->close();
			continue;
		}

		for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
		{
			AcDbObject * pEnt = NULL;
			AcDbObjectId IdTmp = pObjIter->objectId();
			if (Acad::eOk != acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)) continue;

			AcDbAttributeDefinition * pAttrOld = NULL;

			pAttrOld = (AcDbAttributeDefinition*)pEnt;

			if (pAttrOld == NULL)
			{
				pEnt->close();
				continue;
			}


			// 创建一个新的属性对象
			AcDbAttribute *pAttNew = new AcDbAttribute();
			pAttNew->setPropertiesFrom(pAttrOld);   // 从属性定义获得属性对象的对象特性

			// 设置属性对象的其他特性
			pAttNew->setInvisible(pAttrOld->isInvisible());
			pAttNew->setPosition(pAttrOld->position());
			pAttNew->setHeight(pAttrOld->height());
			pAttNew->setRotation(pAttrOld->rotation());

			int nClrIndex = pAttrOld->colorIndex();
			if(nClrIndex == 256)
			{
				for(int q=0; q<strLyNameArray.GetCount(); q++)
				{
					CString strTempLyName = strLyNameArray.GetAt(q);
					char* pLyName = pAttrOld->layer();
					if(strTempLyName.CompareNoCase(pLyName) == 0)
					{
						nClrIndex = atoi(strClrIndexArray.GetAt(q)); 
						pAttNew->setColorIndex(nClrIndex);
						break;
					}
				}
			}
			else
				pAttNew->setColor(pAttrOld->color());

			// 获得属性对象的Tag、Prompt和TextString
			char *pStr = NULL;pStr = pAttrOld->tag(); pAttNew->setTag(pStr);  free(pStr);
			pAttNew->setFieldLength(60);
			pAttNew->setHeight(pAttrOld->height());
			pAttNew->setTextString(pAttrOld->textString());
			pAttNew->setLayer(strLyName);

			// 向块参照追加属性对象
			pBlkRefNew->appendAttribute(pAttNew);
			pAttNew->close();;
			pAttrOld->close();
		}

		pBlkRefNew->close();
		pBlkRefOld->close();
		pBlkDefRcd->close();
	}
}


//功能：将当前实体添加到模型空间
//参数：resultId  返回添加实体的ID
//      pEnt      实体指针
//      pDB       当前文档数据库指针
//返回：添加实体成功返回 TRUE 否则返回 FALSE  
BOOL CDistClipEntity::AddToCurrentSpace(AcDbObjectId& resultId,AcDbEntity* pEnt,AcDbDatabase* pDB)
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
		pBlkRec->close();
		OutRunInfo("\n Add entity to model space error！");
		return FALSE;
	}
	pBlkRec->close();

	return TRUE;
}




void ident_init(ads_matrix id) 
{ 
    int i, j; 
	
    for (i=0; i<=3; i++) 
        for (j=0; j<=3; j++) 
            id[i][j] = 0.0; 
		for (i=0; i<=3; i++) 
			id[i][i] = 1.0; 
} 

int dragsample(ads_point usrpt,ads_matrix matrix)
{
	ident_init(matrix);
	//acdbUcs2Wcs(usrpt, usrpt, Adesk::kFalse);	
	matrix[0][T] = usrpt[X]; 
    matrix[1][T] = usrpt[Y]; 
    matrix[2][T] = usrpt[Z]; 
	return RTNORM;	
}

int LJ_Draggen(ads_name ssname,CString strPrompt,ads_point pt,ads_point ptRet,int nCursor)
{
	LONG sslength;	
	if(ads_sslength(ssname,&sslength)!=RTNORM)
	{		
		return -1;
	}		
	
	return ads_draggen(ssname,strPrompt,nCursor,dragsample,ptRet);	
}


//拖动实体预览并移动位置
int Draggen(LPCTSTR lpszPrompt,AcDbObjectIdArray& aryids,AcGePoint3d& ptCenter)
{
	actrTransactionManager->queueForGraphicsFlush();
	actrTransactionManager->flushGraphics();
	ads_name ssname;
	ads_ssadd(NULL,NULL,ssname);
	for(int i=0;i<aryids.length();i++)
	{
		ads_name locname;
		AcDbObjectId id=aryids.at(i);
		
		acdbGetAdsName(locname,id);
		ads_redraw(locname,2);
		ads_ssadd(locname,ssname,ssname);
		ads_ssfree(locname);		
	}
	ads_point pt,ptRet;
	pt[X]=ptCenter.x;
	pt[Y]=ptCenter.y;
	pt[Z]=ptCenter.z;
		
	int nReturn=LJ_Draggen(ssname,lpszPrompt,pt,ptRet,0);
	if(nReturn!=RTNORM)
	{
		ads_ssfree(ssname);
		return -1;	
	}
	AcGeMatrix3d xform;
	xform.setToTranslation(AcGeVector3d(ptRet[X]-pt[X],ptRet[Y]-pt[Y],ptRet[Z]-pt[Z]));
	ads_xformss(ssname,xform.entry);
	ads_ssfree(ssname);
		
	ptCenter.x=ptRet[X];
	ptCenter.y=ptRet[Y];
	ptCenter.z=ptRet[Z];	

	actrTransactionManager->queueForGraphicsFlush();
	actrTransactionManager->flushGraphics();

	return nReturn;
}



//函数功能：判断两线段是否相交
//入口参数：
//          px1,py1,px2,py2   线段P1P2
//          qx1,qy1,qx2,qy2   线段Q1Q2
//返回值  ：如果相交返回TRUE 否则返回FALSE 。
////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDistClipEntity::IsLineSectIntersectLineSect(double px1,double py1,double px2,double py2,
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
BOOL CDistClipEntity::IsPointOnLine(double px1,double py1,double px2,double py2,double qx,double qy)
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
BOOL  CDistClipEntity::IsPointInPolygon(AcGePoint3dArray& pts,double x,double y)
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

//绘制自定义图幅框
BOOL CDistClipEntity::DrawCustomGrid(AcGePoint3dArray& ResultPtArray,AcDbObjectIdArray& ResultObjIdArray,
									 char* strLyName,double dEdge,double dCross,double dCrossDist,CStringArray& strValueArray) 
{
	ResultPtArray.setLogicalLength(0);
	ResultObjIdArray.setLogicalLength(0);

	AcGePoint3d ptMin,ptMax;
	if(acedGetPoint(NULL,"\n指定第一点:",asDblArray(ptMin))!=RTNORM)
		return FALSE;
	if(acedGetCorner(asDblArray(ptMin),"\n指定第二点",asDblArray(ptMax))!=RTNORM)
		return FALSE;
	double dmaxX,dmaxY,dminX,dminY;
	if(ptMin.x < ptMax.x){
		dminX = ptMin.x; dmaxX = ptMax.x;
	}
	else{
		dmaxX = ptMin.x; dminX = ptMax.x;
	}
	if(ptMin.y < ptMax.y){
		dminY = ptMin.y; dmaxY = ptMax.y;
	}
	else{
		dmaxY = ptMin.y; dminY = ptMax.y;
	}

	//if((dmaxX-dminX<100) || (dmaxY-dminY<100) ){
		//acutPrintf("\n范围太小，绘制图框失败!");
		//return FALSE;
	//}
	ResultPtArray.append(AcGePoint3d(dminX,dminY,0));
	ResultPtArray.append(AcGePoint3d(dmaxX,dminY,0));
	ResultPtArray.append(AcGePoint3d(dmaxX,dmaxY,0));
	ResultPtArray.append(AcGePoint3d(dminX,dmaxY,0));
	ResultPtArray.append(AcGePoint3d(dminX,dminY,0));

	CString strTempCadLyName = "项目标注";
	resbuf* fileter = acutBuildList(-4, "<or", 
		                            -4, "<and",RTDXF0, "INSERT", 100, "AcDbBlockReference",-4,"and>", 
		                            -4, "or>", 0);//-4, "<and",8,"项目标注", -4, "and>", 

	CDistSelSet tempSet;
	if(1 == tempSet.BoxSelect(AcGePoint3d(dminX,dminY,0),AcGePoint3d(dmaxX,dmaxY,0),fileter))
	{
		AcDbObjectIdArray ObjIdArray;
		AcDbObjectId ObjId;
		tempSet.AsArray(ObjIdArray);
		int nLen = ObjIdArray.length();
		for(int k=0; k<nLen; k++)
		{
			ObjId = ObjIdArray.at(k);
			CString strBlkName;
			GetBlockNameByBlkRefId(strBlkName,ObjId);

			if(strBlkName.CompareNoCase("_项目属性")!= 0) continue;
			CStringArray strTags,strContents;
			if(GetBckRefInf(ObjId,strTags,strContents)==FALSE) break;

			for(int jj =0;jj<strTags.GetSize(); jj++)
			{
				CString strTempValue;
				CString strTempTag = strTags.GetAt(jj);

				if(strTempTag.Find("流程号")>=0)
				{
					strTempValue.Format("[   ] 蓝线示意图        流程号 %s",strContents.GetAt(jj));
					strValueArray.SetAt(0,strTempValue);
				}
				else if(strTempTag.Find("项目名称")>=0)
				{
					strTempValue.Format("项目名称   %s",strContents.GetAt(jj));
					strValueArray.SetAt(1,strTempValue);
				}
				else if(strTempTag.Find("建设单位")>=0)
				{
					strTempValue.Format("建设单位   %s",strContents.GetAt(jj));
					strValueArray.SetAt(2,strTempValue);
				}
				else if(strTempTag.Find("总面积")>=0)
				{
					strTempValue.Format("总面积     %s",strContents.GetAt(jj));
					strValueArray.SetAt(3,strTempValue);
				}
				else if(strTempTag.Find("建设用地")>=0)
				{
					strTempValue.Format("建设用地（A1）   %s",strContents.GetAt(jj));
					strValueArray.SetAt(4,strTempValue);
				}
				else if(strTempTag.Find("道路用地")>=0)
				{
					strTempValue.Format("道路用地（A2）   %s",strContents.GetAt(jj));
					strValueArray.SetAt(5,strTempValue);
				}
				else if(strTempTag.Find("绿化用地")>=0)
				{
					strTempValue.Format("绿化用地（A3）   %s",strContents.GetAt(jj));
					strValueArray.SetAt(6,strTempValue);
				}
				else if(strTempTag.Find("其他用地")>=0)
				{;
					strTempValue.Format("其他用地（A4）   %s",strContents.GetAt(jj));
					strValueArray.SetAt(7,strTempValue);
				}
				else if(strTempTag.Find("审批时间")>=0)
				{;
					strTempValue = strContents.GetAt(jj);
					strValueArray.SetAt(9,strTempValue);
				}
				break;
			}
		}
	}
	

	//绘制外框
	AcDbObjectId resultId;
	CDistEntityTool tempTool;
	tempTool.CreateLine(resultId,AcGePoint3d(dminX-dEdge,dminY,0),AcGePoint3d(dmaxX+dEdge,dminY,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX-dEdge,dmaxY,0),AcGePoint3d(dmaxX+dEdge,dmaxY,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX,dminY-dEdge,0),AcGePoint3d(dminX,dmaxY+dEdge,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dmaxX,dminY-dEdge,0),AcGePoint3d(dmaxX,dmaxY+dEdge,0),strLyName,5);ResultObjIdArray.append(resultId);
	
	tempTool.CreateLine(resultId,AcGePoint3d(dminX-dEdge,dminY-dEdge,0),AcGePoint3d(dmaxX+dEdge,dminY-dEdge,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX-dEdge,dmaxY+dEdge,0),AcGePoint3d(dmaxX+dEdge,dmaxY+dEdge,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX-dEdge,dminY-dEdge,0),AcGePoint3d(dminX-dEdge,dmaxY+dEdge,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dmaxX+dEdge,dminY-dEdge,0),AcGePoint3d(dmaxX+dEdge,dmaxY+dEdge,0),strLyName,5);ResultObjIdArray.append(resultId);

	//绘制头说明框
	tempTool.CreateLine(resultId,AcGePoint3d(dminX,dmaxY+dEdge+3,0),AcGePoint3d(dminX+45,dmaxY+dEdge+3,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX,dmaxY+dEdge+3+8,0),AcGePoint3d(dminX+45,dmaxY+dEdge+3+8,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX,dmaxY+dEdge+3+16,0),AcGePoint3d(dminX+45,dmaxY+dEdge+3+16,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX,dmaxY+dEdge+3+24,0),AcGePoint3d(dminX+45,dmaxY+dEdge+3+24,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX,dmaxY+dEdge+3,0),AcGePoint3d(dminX,dmaxY+dEdge+3+24,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX+15,dmaxY+dEdge+3,0),AcGePoint3d(dminX+15,dmaxY+dEdge+3+24,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX+30,dmaxY+dEdge+3,0),AcGePoint3d(dminX+30,dmaxY+dEdge+3+24,0),strLyName,5);ResultObjIdArray.append(resultId);
	tempTool.CreateLine(resultId,AcGePoint3d(dminX+45,dmaxY+dEdge+3,0),AcGePoint3d(dminX+45,dmaxY+dEdge+3+24,0),strLyName,5);ResultObjIdArray.append(resultId);


	//绘制标题
	tempTool.CreateText(resultId,AcGePoint3d(dminX+(dmaxX-dminX)/2-60,dmaxY+dEdge+16,0),"项目蓝线图",strLyName,5,"宋体",0,0,20);ResultObjIdArray.append(resultId);
	tempTool.CreateText(resultId,AcGePoint3d(dmaxX-95,dmaxY+dEdge+8,0),"内部资料 严禁复制",strLyName,5,"宋体",0,0,10);ResultObjIdArray.append(resultId);


	//绘制会签
	AcGePoint3d ptS,ptE;
	double dx = dmaxX - 100;
	double dy = dminY;
	ptS.x = dx;ptS.y = dy;ptS.z = 0; ptE.x = dx;ptE.y = dy+88;
	tempTool.CreateLine(resultId,ptS,ptE,strLyName,5); ResultObjIdArray.append(resultId);
	ptS.x = dx+18;ptS.y = dy+24;ptE.x = dx+18;ptE.y = dy+80;
	tempTool.CreateLine(resultId,ptS,ptE,strLyName,5); ResultObjIdArray.append(resultId);

	ptS.x = dx;ptS.y = dy+24;ptE.x = dx+100;ptE.y = dy+24;
	tempTool.CreateLine(resultId,ptS,ptE,strLyName,5); ResultObjIdArray.append(resultId);
	for(int i=7; i<12; i++)
	{
		ptS.x = dx;ptS.y = dy+8*i;ptE.x = dx+100;ptE.y = dy+8*i;
		tempTool.CreateLine(resultId,ptS,ptE,strLyName,5); ResultObjIdArray.append(resultId);
	}
	for(i=4; i<7; i++)
	{
		ptS.x = dx+18;ptS.y = dy+8*i;ptE.x = dx+100;ptE.y = dy+8*i;
		tempTool.CreateLine(resultId,ptS,ptE,strLyName,5); ResultObjIdArray.append(resultId);
	}

	ptS.x = dx+1; ptS.y = dy+81;
	CString strTemp =strValueArray.GetAt(0);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);
	ptS.x = dx+1; ptS.y = dy+73; strTemp =strValueArray.GetAt(1);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);
	ptS.x = dx+1; ptS.y = dy+65;strTemp =strValueArray.GetAt(2);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);
	ptS.x = dx+1; ptS.y = dy+57;strTemp =strValueArray.GetAt(3);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);
	ptS.x = dx+1; ptS.y = dy+37;
	tempTool.CreateText(resultId,ptS," 其中 ",strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);

	ptS.x = dx+20; ptS.y = dy+49;strTemp =strValueArray.GetAt(4);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);
	ptS.x = dx+20; ptS.y = dy+41;strTemp =strValueArray.GetAt(5);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);
	ptS.x = dx+20; ptS.y = dy+33;strTemp =strValueArray.GetAt(6);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);
	ptS.x = dx+20; ptS.y = dy+25;strTemp =strValueArray.GetAt(7);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);

	ptS.x = dx+22; ptS.y = dy+10;strTemp =strValueArray.GetAt(8);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,6);ResultObjIdArray.append(resultId);

	ptS.x = dx+70; ptS.y = dy+1;strTemp =strValueArray.GetAt(9);
	tempTool.CreateText(resultId,ptS,strTemp.GetBuffer(),strLyName,5,"宋体",0,0,3);ResultObjIdArray.append(resultId);

	//绘制尾标题
	/*
	char strTitle[20]={0};
	sprintf(strTitle,"%0.2f",dminX);
	tempTool.CreateText(resultId,AcGePoint3d(dminX-dEdge+5,dminY,0),strTitle,strLyName,5,"宋体",0,0,5);ResultObjIdArray.append(resultId);
	sprintf(strTitle,"%0.2f",dminY);
	tempTool.CreateText(resultId,AcGePoint3d(dminX-dEdge,dminY-dEdge,0),strTitle,strLyName,5,"宋体",0,0,5);ResultObjIdArray.append(resultId);
	sprintf(strTitle,"%0.2f",dmaxX);
	tempTool.CreateText(resultId,AcGePoint3d(dmaxX-30,dmaxY+5,0),strTitle,strLyName,5,"宋体",0,0,5);ResultObjIdArray.append(resultId);
	sprintf(strTitle,"%0.2f",dmaxY);
	tempTool.CreateText(resultId,AcGePoint3d(dmaxX-20,dmaxY-6,0),strTitle,strLyName,5,"宋体",0,0,5);ResultObjIdArray.append(resultId);	
	tempTool.CreateText(resultId,AcGePoint3d(dminX,dminY-dEdge-10,0),"xxxx年xx月xx日测图",strLyName,5,"宋体",0,0,6);ResultObjIdArray.append(resultId);	

	tempTool.CreateText(resultId,AcGePoint3d(dmaxX-100,dminY-dEdge-10,0),"测量员",strLyName,5,"宋体",0,0,6);ResultObjIdArray.append(resultId);	
	tempTool.CreateText(resultId,AcGePoint3d(dmaxX-100,dminY-dEdge-20,0),"绘图员",strLyName,5,"宋体",0,0,6);ResultObjIdArray.append(resultId);	
	tempTool.CreateText(resultId,AcGePoint3d(dmaxX-100,dminY-dEdge-30,0),"检查员",strLyName,5,"宋体",0,0,6);ResultObjIdArray.append(resultId);	
	tempTool.CreateText(resultId,AcGePoint3d(dminX+(dmaxX-dminX)/2-40,dminY-dEdge-40,0),"图比例 1:1000",strLyName,5,"宋体",0,0,20);ResultObjIdArray.append(resultId);
	*/

	int nNumX=0,nNumY=0;
	double dTemp = (dmaxX-dminX)/50;
	nNumX = (int)dTemp;
	if(dTemp - nNumX > 0.05)
		nNumX = nNumX + 1;
	dTemp = (dmaxY - dminY)/50;
	nNumY = (int)dTemp;
	if(dTemp - nNumY> 0.05)
		nNumY = nNumY + 1;

	for(int i=0;i<=nNumX;i++)
	{
		for(int j=0; j<=nNumY; j++)
		{
			if(i==0 &&  j>0 && j<nNumY)
			{
				tempTool.CreateLine(resultId,AcGePoint3d(dminX,dminY+j*50,0),AcGePoint3d(dminX+2.5,dminY+j*50,0),strLyName,5);
			}
			else if(i==nNumX && j>0 && j<nNumY)
			{
				tempTool.CreateLine(resultId,AcGePoint3d(dmaxX,dminY+j*50,0),AcGePoint3d(dmaxX-2.5,dminY+j*50,0),strLyName,5);
			}
			else if(j==0 && i>0 && i<nNumX)
			{
				tempTool.CreateLine(resultId,AcGePoint3d(dminX+i*50,dminY,0),AcGePoint3d(dminX+i*50,dminY+2.5,0),strLyName,5);
			}
			else if(j==nNumY && i>0 && i<nNumX)
			{
				tempTool.CreateLine(resultId,AcGePoint3d(dminX+i*50,dmaxY,0),AcGePoint3d(dminX+i*50,dmaxY-2.5,0),strLyName,5);
			}
			else if(i>0 && i<nNumX && j>0 && j<nNumY)
			{
				tempTool.CreateLine(resultId,AcGePoint3d(dminX+i*50-2.5,dminY+j*50,0),AcGePoint3d(dminX+i*50+2.5,dminY+j*50,0),strLyName,5);
				ResultObjIdArray.append(resultId);
				tempTool.CreateLine(resultId,AcGePoint3d(dminX+i*50,dminY+j*50+2.5,0),AcGePoint3d(dminX+i*50,dminY+j*50-2.5,0),strLyName,5);
			}
			else
				continue;

			ResultObjIdArray.append(resultId);
		}
	}
	return TRUE;
}


//返回创建实体ID
//组成员ID集合          
//组名称		                                 
BOOL CDistClipEntity::CreateGroup(AcDbObjectId& groupId,AcDbObjectIdArray& objIds,char* strGroupName)                     
{
    AcDbGroup *pGroup = new AcDbGroup(strGroupName);
	if(NULL == pGroup) return FALSE;

    pGroup->append(objIds);

    AcDbDictionary *pGroupDict=NULL;
	AcDbDatabase *pDB=acdbHostApplicationServices()->workingDatabase();
	if(Acad::eOk == pDB->getGroupDictionary(pGroupDict, AcDb::kForWrite))
	{
		pGroupDict->setAt(strGroupName, pGroup, groupId);
		pGroupDict->close();
		pGroup->close();
		return TRUE;
	}
	else
	{
		pGroupDict->close();
		delete pGroup;
		return FALSE;
	}
	
}


// 得到一个属性块里面所有的标记和内容
BOOL CDistClipEntity::GetBckRefInf(const AcDbObjectId & BckRefId, CStringArray &strTags, CStringArray &strContents)
{
	strTags.RemoveAll(); strContents.RemoveAll();
	AcDbBlockReference * pBckRef = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pBckRef, BckRefId, AcDb::kForRead);

	if (Acad::eOk != es)  return FALSE;
	pBckRef->close();

	AcDbObjectIterator * pObjIter = pBckRef->attributeIterator();
	if (pObjIter == NULL) return FALSE;

	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		if (Acad::eOk != acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)) continue;
		pEnt->close();
		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL) continue;

		//ARX 2005
		//CString sTag = pAttr->tagConst();
		//CString sValue = pAttr->textStringConst();
		//ARX 2004
		CString sTag = pAttr->tag();
		CString sValue = pAttr->textString();

		strTags.Add(sTag);
		strContents.Add(sValue);
	}

	return (strTags.GetSize()>0);
}

BOOL CDistClipEntity::GetBlockNameByBlkRefId(CString &strName,AcDbObjectId objId)
{
	ads_name eName;
	struct resbuf* resBlockRef;
	acdbGetAdsName(eName, objId);
	resBlockRef = acdbEntGet(eName);
	acedSSFree(eName);

	while (resBlockRef)
	{
		if (resBlockRef->restype == 0)
		{
			if(stricmp(resBlockRef->resval.rstring, "insert") != 0)
			{
				acutRelRb(resBlockRef);
				return FALSE;
			}
		}

		if (resBlockRef->restype == 2) // 组码为2表示的是块名
		{
			strName.Format("%s", resBlockRef->resval.rstring);
			acutRelRb(resBlockRef);
			return TRUE;
		}

		resBlockRef = resBlockRef->rbnext;
	}

	acutRelRb(resBlockRef);
	return FALSE;
}


void CDistClipEntity::CalcRectByScale(double dScaleX,double dScaleY,AcGePoint3d basePt,AcGePoint3dArray& pts)
{
	AcGePoint3d tempPt;
	double dW1 = 390.2713,dH1 = 281.1951;
	double dW2 = 382,dH2 = 272;
	double dWA = 266.8717,dWB = 115.1283;
	double dHA = 72.0882,dHB = 199.9118;

	pts.setPhysicalLength(0);

	tempPt.x = basePt.x; tempPt.y = basePt.y; tempPt.z = 0;
	pts.append(tempPt);

	tempPt.x = basePt.x + dWA * dScaleX; tempPt.y = basePt.y;
	pts.append(tempPt);

	tempPt.x = basePt.x + dWA * dScaleX; tempPt.y = basePt.y + dHA * dScaleY; 
	pts.append(tempPt);

	tempPt.x = basePt.x + dW2 * dScaleX; tempPt.y = basePt.y + dHA * dScaleY; 
	pts.append(tempPt);

	tempPt.x = basePt.x + dW2 * dScaleX; tempPt.y = basePt.y + dH2 * dScaleY;
	pts.append(tempPt);

	tempPt.x = basePt.x; tempPt.y = basePt.y + dH2 * dScaleY;
	pts.append(tempPt);

	tempPt.x = basePt.x; tempPt.y = basePt.y;
	pts.append(tempPt);
}

void CDistClipEntity::GetProjectInfo(CStringArray& strValueArray,AcGePoint3dArray& SelectPtArray)
{

	CString strTempCadLyName = "项目标注";
	resbuf* fileter = acutBuildList(-4, "<or", 
		                            -4, "<and",RTDXF0, "INSERT", 100, "AcDbBlockReference",-4,"and>", 
									-4, "or>", 0);

	for(int i=0; i<11; i++)
		strValueArray.Add(" ");

	CDistSelSet tempSet;
	
	if(1 == tempSet.WindowPolygonSelect(SelectPtArray,fileter))
	{
		AcDbObjectIdArray ObjIdArray;
		AcDbObjectId ObjId;
		tempSet.AsArray(ObjIdArray);
		int nLen = ObjIdArray.length();
		for(int k=0; k<nLen; k++)
		{
			ObjId = ObjIdArray.at(k);
			CString strBlkName;
			GetBlockNameByBlkRefId(strBlkName,ObjId);

			if(strBlkName.CompareNoCase("_项目属性")!= 0) continue;
			CStringArray strTags,strContents;
			if(GetBckRefInf(ObjId,strTags,strContents)==FALSE) break;


			// 临时
			strValueArray.SetAt(10,"    市规划局");
			for(int jj =0;jj<strTags.GetSize(); jj++)
			{
				CString strTempValue;
				CString strTempTag = strTags.GetAt(jj);
				if(strTempTag.Find("时间")>=0)
				{
					strValueArray.SetAt(0,strContents.GetAt(jj));
				}
				else if(strTempTag.Find("流程号")>=0)
				{
					strTempValue.Format("流程号：%s",strContents.GetAt(jj));
					strValueArray.SetAt(1,strTempValue);
				}
				else if(strTempTag.Find("蓝线")>=0)
				{
					strTempValue = strContents.GetAt(jj);
					strValueArray.SetAt(2,strTempValue);
				}
				else if(strTempTag.Find("项目名称")>=0)
				{
					strTempValue.Format("项目名称：%s",strContents.GetAt(jj));
					strValueArray.SetAt(3,strTempValue);
				}
				else if(strTempTag.Find("建设单位")>=0)
				{
					strTempValue.Format("建设单位：%s",strContents.GetAt(jj));
					strValueArray.SetAt(4,strTempValue);
				}
				else if(strTempTag.Find("总面积")>=0)
				{
					strTempValue.Format("总面积 ：%s",strContents.GetAt(jj));
					strValueArray.SetAt(5,strTempValue);
				}
				else if(strTempTag.Find("建设")>=0)
				{
					strTempValue.Format("建设用地（A1）：%s",strContents.GetAt(jj));
					strValueArray.SetAt(6,strTempValue);
				}
				else if(strTempTag.Find("道路")>=0)
				{
					strTempValue.Format("道路用地（A2）：%s",strContents.GetAt(jj));
					strValueArray.SetAt(7,strTempValue);
				}
				else if(strTempTag.Find("绿化")>=0)
				{
					strTempValue.Format("绿化用地（A3）：%s",strContents.GetAt(jj));
					strValueArray.SetAt(8,strTempValue);
				}
				else if(strTempTag.Find("其他")>=0)
				{
					strTempValue.Format("其他用地（A4）：%s",strContents.GetAt(jj));
					strValueArray.SetAt(9,strTempValue);
				}

			}
		}
	}
}


void CDistClipEntity::CreateBlockByFile()
{
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable *pBlkTbl=NULL;
	if(Acad::eOk != pDB->getBlockTable(pBlkTbl, AcDb::kForRead)) return;

	if(pBlkTbl->has("图框")){
		pBlkTbl->close(); return ;
	}
	pBlkTbl->close();

	CString strFName;
	strFName.Format("%sBLOCKS\\图框.dwg",GetCurArxAppPath("DistClipPlot.arx"));

	AcDbDatabase * pBlockDB = new AcDbDatabase(Adesk::kFalse);
	if (Acad::eOk != pBlockDB->readDwgFile(strFName))
	{
		delete pBlockDB;

		CString sMsg = "没有找到插入源文件" + strFName + "!";
		AfxMessageBox(sMsg);
		return ;
	}

	AcDbObjectId tempId;
	Acad::ErrorStatus es = pDB->insert(tempId,"图框", pBlockDB);//acdbCurDwg()

	delete pBlockDB;
}

CString CDistClipEntity::GetCurArxAppPath(CString strArxName)
{
	TCHAR exeFullPath[MAX_PATH]; 
	CString strPath; 
	GetModuleFileName(GetModuleHandle(strArxName),exeFullPath,MAX_PATH); 
	strPath=(CString)exeFullPath; 
	int position=strPath.ReverseFind('\\'); 
	strPath=strPath.Left(position+1); 
	return strPath;
}

//功能:实现多边形与矩形求交,结果返回零个或多个多边行点数组
void CDistClipEntity::PolyIntersectWithRect(RECT_ACGE_ARRAY& resultArray,AcGePoint3dArray& polyPtArray,AcGePoint3dArray& rectPtArray)
{


}

/*
BOOL CDistClipEntity::UpdateAttrToBlkRef(AcDbObjectId ObjIdRef,CStringArray& strArrayTag,CStringArray& strArrayValue)
{
	int nCount = strArrayTag.GetCount();
	if(nCount==0 || nCount != strArrayValue.GetCount() || ObjIdRef.isNull())
	{
		acutPrintf("\n 参数有误!"); return FALSE;
	}

	AcDbBlockReference * pBckRef = NULL;
	if (Acad::eOk != acdbOpenObject(pBckRef, ObjIdRef, AcDb::kForRead)) return FALSE;
	pBckRef->close();

	AcDbObjectIterator * pObjIter = pBckRef->attributeIterator();
	if (pObjIter == NULL) return FALSE;

	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		if (Acad::eOk != acdbOpenObject(pEnt, IdTmp, AcDb::kForWrite)) continue;

		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL)
		{
			pEnt->close();continue;
		}

		CString strTemp = pAttr->tagConst();
		for (int i=0; i<nCount; i++)
		{
			if (strTemp.CompareNoCase(strArrayTag.GetAt(i)) == 0)
			{
				strTemp = strArrayValue.GetAt(i);
				pAttr->setTextString(strTemp.GetBuffer(0));
				break;
			}
		}
		pEnt->close();
	}

	return TRUE;
}
*/



AcDbObjectId CDistClipEntity::FindBlkRef(CString strBlkName)
{
	//选择图层中所有实体
	CDistSelSet tempSet;
	tempSet.AllSelect();
	AcDbObjectIdArray IdArray;
	tempSet.AsArray(IdArray);
	int nCount = IdArray.length();
	AcDbBlockReference* pBlkRef = NULL;
	AcDbObjectId resultId;
	for(int i=0; i<nCount; i++)
	{
		resultId = IdArray.at(i);
		if(Acad::eOk == acdbOpenObject(pBlkRef,resultId,AcDb::kForRead))
		{
			pBlkRef->close();
			CString strName;
			GetBlockNameByBlkRefId(strName,resultId);
			if(strName.CompareNoCase(strBlkName)==0)
				return resultId;
		}
	}

	resultId.setNull();
	return resultId;
}

//从块引用中获取属性信息
BOOL CDistClipEntity::ReadAttrFromRef(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen)
{
	strArrayTag.RemoveAll(); strArrayValue.RemoveAll();
	acutPrintf("选择项目信息属性块");
	ads_name ssName;
	struct resbuf* prebFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
	int nResult = acedSSGet(":S", NULL, NULL, prebFilter, ssName);//"X"
	acutRelRb(prebFilter);
	if(nResult != RTNORM) return FALSE;

	long lCount = 0;
	acedSSLength(ssName, &lCount);
	if(lCount ==0) return FALSE;

	ads_name ssEnt;
	acedSSName(ssName,0, ssEnt);

	AcDbObjectId ObjId;
	acdbGetObjectId(ObjId, ssEnt);
	AcDbBlockReference* pBlkRef = NULL;
	if(Acad::eOk != acdbOpenObject(pBlkRef,ObjId,AcDb::kForRead))
	{
		AcDbObjectId blkId = pBlkRef->blockId();
		CString strName;
		GetBlockNameByBlkRefId(strName,blkId);
		if(strName.CompareNoCase("_项目属性")!=0)
		{
			acutPrintf("\n请选择项目属性块!");
			pBlkRef->close(); return FALSE;
		}
	}
	acedSSFree(ssName);

	ptCen = pBlkRef->position();

	AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
	if (pObjIter == NULL)  return FALSE;

	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
		pEnt->close();

		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL) continue;

		//ARX 2005
		//CString strTemp = pAttr->tagConst();
		//ARX 2004
		CString strTemp = pAttr->tag();
		strArrayTag.Add(strTemp);
		//ARX 2005
		//strTemp = pAttr->textStringConst();
		//ARX 2004
		strTemp = pAttr->textString();
		strArrayValue.Add(strTemp);
	}
	pBlkRef->close();

	return (strArrayValue.GetCount()>0);
}


BOOL CDistClipEntity::PlotByPaperSpace()
{
	//1.读项目信息
	AcGePoint3d ptCen;
	CStringArray strArrayTag;
	CStringArray strArrayValue,strArrayA1,strArrayA2,strArrayA3,strArrayA4;
	if(ReadAttrFromRefDk(strArrayTag,strArrayValue,ptCen,strArrayA1,strArrayA2,strArrayA3,strArrayA4) == FALSE) 
	{
		acutPrintf("\n读项目信息失败!"); return FALSE;
	}



	CString strPrjName;
	//2.纠正属性块定义不一致
	int nCount = strArrayTag.GetCount();
	for(int i=0; i<nCount; i++)
	{
		CString strValue;
		CString strTempTag  = strArrayTag.GetAt(i);
		CString strTempValue= strArrayValue.GetAt(i);

		if(strTempTag.Compare("项目显示编号")==0)
		{
			strArrayTag.SetAt(i,"流程号");
			strValue.Format("流程号 %s",strTempValue);
			strArrayValue.SetAt(i,strValue);
		}
		else if(strTempTag.Compare("蓝线类型")==0)
		{
			strArrayTag.SetAt(i,"类型"); 
		}
		else if(strTempTag.Compare("登记日期")==0)
		{
			strArrayTag.SetAt(i,"日期");

			//time_t rawtime; 
			//struct tm * timeinfo=NULL; 

			//time(&rawtime); 
			//timeinfo = localtime ( &rawtime ); 
			//CString strDate;
			//strDate.Format("%d 年 %d 月 %d 日",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday);
			//strArrayValue.SetAt(i,strDate);
			strArrayValue.SetAt(i,"     年    月    日");
		}
		else if(strTempTag.Compare("总用地面积")==0)
		{
			strArrayTag.SetAt(i,"总面积");
			strValue.Format("总用地面积: %s (平方米)",strTempValue);
			strArrayValue.SetAt(i,strValue);
		}
		else if(strTempTag.Compare("建设用地面积")==0)
		{
			strValue.Format("建设用地: %s",strTempValue); 
			strArrayValue.SetAt(i,strValue);
		}
		else if(strTempTag.Compare("道路用地面积")==0)
		{
			strArrayTag.SetAt(i,"道路面积");
			strValue.Format("道路用地: %s",strTempValue);
			strArrayValue.SetAt(i,strValue);
		}
		else if(strTempTag.Compare("绿化用地面积")==0)
		{
			strArrayTag.SetAt(i,"绿化面积");
			strValue.Format("绿化用地: %s",strTempValue);
			strArrayValue.SetAt(i,strValue);
		}
		else if(strTempTag.Compare("其他用地面积")==0)
		{
			strArrayTag.SetAt(i,"其它用地面积");
			strValue.Format("其他用地: %s",strTempValue);
			strArrayValue.SetAt(i,strValue);
		}
		else if(strTempTag.Compare("建设单位")==0)
		{
			strValue.Format("建设单位: %s",strTempValue);
			strArrayValue.SetAt(i,strValue);
		}
		else if(strTempTag.Compare("项目名称")==0)
		{
			strPrjName = strTempValue;
			strValue.Format("项目名称: %s",strTempValue);
			strArrayValue.SetAt(i,strValue);
		}
	}
	if(strPrjName.IsEmpty())
		strPrjName = "项目名称";
	CDistEntityTool tempTool;
	//AcDbViewTableRecord TempCurView = tempTool.GetCurrentView();
	//double dH = TempCurView.height();
	//double dW = TempCurView.width();
	double dH=2000,dW = 2000;
	AcGePoint2d pt(ptCen.x,ptCen.y);



	AcDbLayoutManager* pLayOutManager = acdbHostApplicationServices()->layoutManager();

	


	//2.指定当前布局
	Acad::ErrorStatus es =acdbHostApplicationServices()->layoutManager()->setCurrentLayout("布局1");
	if (es != Acad::eOk)
	{
		es =acdbHostApplicationServices()->layoutManager()->setCurrentLayout("Layout1");
		if(es != Acad::eOk) return FALSE;
	}

	//3.设定当前操作为图纸空间,并满屏显示
	acedCommand(RTSTR,"PSPACE",0);
	acedCommand(RTSTR,"ZOOM",RTSTR,"E",0);


	//4.找打印图框
	AcDbObjectId tempId = FindBlkRef("TBA3");
	if(tempId.isNull())
	{
		acutPrintf("\n打印图框没有找到!"); return FALSE;
	}

	

	//更新属性块
	UpdateAttrToBlkRef(tempId,strArrayTag,strArrayValue,strArrayA1,strArrayA2,strArrayA3,strArrayA4);


	//切换到模型空间
	acedCommand(RTSTR,"_.MSPACE",0);
	acedCommand(RTSTR,"ZOOM",RTSTR,"E",0);

	//项目定位
	tempTool.DyZoom(pt,2000,2000);

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////


	//a.指定当前布局
	es =acdbHostApplicationServices()->layoutManager()->setCurrentLayout("布局2");
	if (es != Acad::eOk)
	{
		es =acdbHostApplicationServices()->layoutManager()->setCurrentLayout("Layout2");
		if(es != Acad::eOk) return FALSE;
	}

	//b.设定当前操作为图纸空间,并满屏显示
	acedCommand(RTSTR,"PSPACE",0);
	acedCommand(RTSTR,"ZOOM",RTSTR,"E",0);


	//c.找打印图框
	tempId = FindBlkRef("TBA4");
	if(tempId.isNull())
	{
		acutPrintf("\n打印图框没有找到!"); return FALSE;
	}

	//d.更新属性块
	UpdateAttrToBlkRef(tempId,strArrayTag,strArrayValue,strArrayA1,strArrayA2,strArrayA3,strArrayA4);

	//e.切换到模型空间
	acedCommand(RTSTR,"_.MSPACE",0);
	acedCommand(RTSTR,"ZOOM",RTSTR,"E",0);
	SaveToDwgFile(strPrjName,ptCen,dH,dW);  //保存文件


	//f.项目定位
	tempTool.DyZoom(pt,2000,2000);


	return TRUE;
}
//该函数必须保证[项目文件的目录]
BOOL CDistClipEntity::SaveToDwgFile(CString strPrjName,AcGePoint3d& ptCen,double dH,double dW)
{
	AcGePoint3d ptMax,ptMin;
	if(dH > 4000 || dH < 500) dH = 1000;
	if(dW > 4000 || dW < 500) dW = 1000;

	ptMax.x = ptCen.x+dW/2; ptMax.y = ptCen.y+dH/2;
	ptMin.x = ptCen.x-dW/2; ptMin.y = ptCen.y-dH/2;

	AcDbDatabase* pCurDb = acdbHostApplicationServices()->workingDatabase();
	AcDbDatabase *pOutDb = new AcDbDatabase();
	if(pOutDb == NULL) return FALSE;

	Acad::ErrorStatus es;
	es = pCurDb->wblock(pOutDb);
	if(es != Acad::eOk)
	{
		delete pOutDb; return FALSE;	
	}

	// 打开目标数据库
	AcDbBlockTable* pToTable = NULL;
	if (pOutDb->getBlockTable(pToTable, AcDb::kForRead) != Acad::eOk) 
	{
		delete pOutDb; return FALSE;
	}

	// 打开目标数据库模型空间
	AcDbBlockTableRecord* pToModel = NULL;
	if (pToTable->getAt(ACDB_MODEL_SPACE, pToModel, AcDb::kForRead) != Acad::eOk)
	{
		pToTable->close(); delete pOutDb;  return FALSE;
	}

	//枚举容器
	AcDbBlockTableRecordIterator* pIter = NULL;
	if ((es=pToModel->newIterator(pIter)) != Acad::eOk) 
	{
		pToTable->close();pToModel->close();  delete pOutDb; return FALSE;
	}

	AcDbObjectId newId;
	AcDbEntity  *pEnt = NULL;

	for (pIter->start(); !pIter->done(); pIter->step()) 
	{
		if (pIter->getEntity(pEnt, AcDb::kForWrite)== Acad::eOk)
		{
			AcDbExtents extents;
			pEnt->getGeomExtents(extents);
			if((extents.maxPoint().x<ptMin.x)||
			   (extents.maxPoint().y<ptMin.y)||
			   (extents.minPoint().x>ptMax.x)||
			   (extents.minPoint().y>ptMax.y))
			{
				pEnt->erase(); 
			}
			pEnt->close();
		}
	}
	delete pIter; pIter = NULL;

	pToTable->close();pToModel->close(); 

	CString strFileCT,strPathPrj;
	ReadCfgLyNameArray(strFileCT,strPathPrj);

	CString strToltalFile;
	strToltalFile = strPathPrj+"项目_"+strPrjName+".dwg";

	pOutDb->saveAs(strToltalFile);

	delete pOutDb; pOutDb = NULL;

	return TRUE;
}


/*
//从块引用中获取属性信息
BOOL CDistClipEntity::ReadAttrFromRefDk(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen)
{
	strArrayTag.RemoveAll(); strArrayValue.RemoveAll();
	acutPrintf("\n选择项目信息标注:");
	ads_name ssName;
	struct resbuf* prebFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
	int nResult = acedSSGet(":S", NULL, NULL, prebFilter, ssName);//"X"
	acutRelRb(prebFilter);
	if(nResult != RTNORM) return FALSE;

	long lCount = 0;
	acedSSLength(ssName, &lCount);
	if(lCount ==0) return FALSE;

	ads_name ssEnt;
	if(RTNORM != acedSSName(ssName,0, ssEnt)) return FALSE;

	AcDbObjectId ObjId;
	if(Acad::eOk != acdbGetObjectId(ObjId, ssEnt)) 
	{
		acedSSFree(ssName); return FALSE;
	}
	acedSSFree(ssName);

	AcDbBlockReference* pBlkRef = NULL;
	if(Acad::eOk == acdbOpenObject(pBlkRef,ObjId,AcDb::kForRead))
	{
		AcDbObjectId blkId = pBlkRef->blockTableRecord();
		CString strName;
		GetBlockNameByBlkId(strName,blkId);
		if(strName.CompareNoCase("_项目属性")!=0)
		{
			acutPrintf("\n请选择项目信息标注!"); pBlkRef->close(); return FALSE;
		}
	}
	ptCen = pBlkRef->position();

	AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
	if (pObjIter == NULL)  return FALSE;

	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
		pEnt->close();

		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL) continue;

		CString strTemp = pAttr->tagConst();
		strArrayTag.Add(strTemp);
		strTemp = pAttr->textStringConst();
		strArrayValue.Add(strTemp);
	}
	pBlkRef->close();

	CString strPrjCode;
	int nCount =strArrayTag.GetCount();
	for(int i=0; i<nCount; i++)
	{
		CString strTemp = strArrayTag.GetAt(i);
		if(strTemp.CompareNoCase("项目显示编号") == 0)
		{
			strPrjCode = strArrayValue.GetAt(i); break;
		}
	}

	struct resbuf* pFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
	AcDbObjectIdArray ObjIdArray;
	CDistSelSet tempSet;
	tempSet.AllSelect(pFilter); acutRelRb(pFilter);
	tempSet.AsArray(ObjIdArray);
	nCount = ObjIdArray.length();

	double dTotal = 0.0;
	CString strTotalA1,strTotalA2,strTotalA3,strTotalA4;
	AcDbObjectId tempId;
	for(int j=0; j<nCount; j++)
	{
		tempId = ObjIdArray.at(j);
		pBlkRef = NULL;
		if(Acad::eOk != acdbOpenObject(pBlkRef,tempId,AcDb::kForRead)) continue;

		AcDbObjectId blkIdX = pBlkRef->blockTableRecord();
		CString strNameX;
		GetBlockNameByBlkId(strNameX,blkIdX);
		if(strNameX.CompareNoCase("_地块属性")!=0)
		{
			pBlkRef->close(); continue;
		}

		AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
		if (pObjIter == NULL)  return FALSE;

		//项目显示编号
		CString strTempPrjCode,strTempDKBH,strTempYDMJ;
		for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
		{
			AcDbObject * pEnt = NULL;
			AcDbObjectId IdTmp = pObjIter->objectId();
			if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
			pEnt->close();

			AcDbAttributeDefinition * pAttr = NULL;
			pAttr = (AcDbAttributeDefinition*)pEnt;
			if (pAttr == NULL) continue;
			CString strTemp = pAttr->tagConst();
			{
				if(strTemp.CompareNoCase("项目显示编号") == 0)
					strTempPrjCode = pAttr->textStringConst();
				else if(strTemp.CompareNoCase("地块编号") == 0)
					strTempDKBH = pAttr->textStringConst();
				else if(strTemp.CompareNoCase("用地面积" )== 0)
					strTempYDMJ = pAttr->textStringConst();
			}	
		}
		pBlkRef->close();

		if(strTempPrjCode.CompareNoCase(strPrjCode) != 0) continue;

		if(strTempDKBH.Find("A1")>=0)
		{
			strTempYDMJ.Trim(); 
			dTotal = dTotal + atof(strTempYDMJ);
			strTotalA1 = strTotalA1 + strTempDKBH+":"+strTempYDMJ+" ";
		}
		else if(strTempDKBH.Find("A2")>=0)
		{
			strTempYDMJ.Trim();
			dTotal = dTotal + atof(strTempYDMJ);
			strTotalA2 = strTotalA2 + strTempDKBH+":"+strTempYDMJ+" ";
		}
		else if(strTempDKBH.Find("A3")>=0)
		{
			strTempYDMJ.Trim(); 
			dTotal = dTotal + atof(strTempYDMJ);
			strTotalA3 = strTotalA3 + strTempDKBH+":"+strTempYDMJ+" ";
		}
		else if(strTempDKBH.Find("A4")>=0)
		{
			strTempYDMJ.Trim(); 
			dTotal = dTotal + atof(strTempYDMJ);
			strTotalA4 = strTotalA4 + strTempDKBH+":"+strTempYDMJ+" ";
		}
	}
    CString strTotalArea;
	strTotalArea.Format("%0.3f",dTotal);
	int nTempLen = strArrayTag.GetCount();
	for(int ii=0; ii<nTempLen; ii++)
	{
		CString strTempII = strArrayTag.GetAt(ii);
		//if(strTempII.CompareNoCase("总用地面积")==0)
		//{
			//strArrayValue.SetAt(ii,strTotalArea);
		//}
		if(strTempII.CompareNoCase("建设用地面积")==0)
		{
			if(strTotalA1.IsEmpty()) strTotalA1 = "0.000";
			strArrayValue.SetAt(ii,strTotalA1);
		}
		if(strTempII.CompareNoCase("道路用地面积")==0)
		{
			if(strTotalA2.IsEmpty()) strTotalA2 = "0.000";
			strArrayValue.SetAt(ii,strTotalA2);
		}
		if(strTempII.CompareNoCase("绿化用地面积")==0)
		{
			if(strTotalA3.IsEmpty()) strTotalA3 = "0.000";
			strArrayValue.SetAt(ii,strTotalA3);
		}
		if(strTempII.CompareNoCase("其他用地面积")==0)
		{
			if(strTotalA4.IsEmpty()) strTotalA4 = "0.000";
			strArrayValue.SetAt(ii,strTotalA4);
		}
	}


	//strArrayTag.Add("总用地面积"); strArrayValue.Add(strTotalArea);
	//if(strTotalA1.IsEmpty()) strTotalA1 = "0.000";
	//strArrayTag.Add("建设用地面积"); strArrayValue.Add(strTotalA1);
	//if(strTotalA2.IsEmpty()) strTotalA2 = "0.000";
	//strArrayTag.Add("道路用地面积"); strArrayValue.Add(strTotalA2);
	//if(strTotalA3.IsEmpty()) strTotalA3 = "0.000";
	//strArrayTag.Add("绿化用地面积"); strArrayValue.Add(strTotalA3);
	//if(strTotalA4.IsEmpty()) strTotalA4 = "0.000";
	//strArrayTag.Add("其他用地面积"); strArrayValue.Add(strTotalA4);
	return TRUE;
}
*/

BOOL CDistClipEntity::GetBlockNameByBlkId(CString &strBlkName,AcDbObjectId blockId)
{
	strBlkName.Empty();
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable *pBlockTable = NULL ; 
	if(Acad::eOk != pDB->getBlockTable (pBlockTable, AcDb::kForRead)) return FALSE; 

	AcDbBlockTableIterator* pIterator = NULL;
	if(Acad::eOk != pBlockTable->newIterator(pIterator))
	{
		pBlockTable->close(); return FALSE;
	}

	for(pIterator->start();!pIterator->done();pIterator->step())
	{
		AcDbObjectId blkId;
		if(Acad::eOk != pIterator->getRecordId(blkId)) continue;
		if(blkId == blockId)
		{
			AcDbBlockTableRecord* pBlockTableRecord = NULL;
			if(Acad::eOk != pIterator->getRecord(pBlockTableRecord,AcDb::kForRead))
			{
				delete pIterator; pIterator = NULL;
				pBlockTable->close(); return FALSE;
			}
			char* strName = NULL;
			if(Acad::eOk !=pBlockTableRecord->getName(strName)) 
			{
				pBlockTable->close();
				pBlockTableRecord->close(); return FALSE;
			}
			delete pIterator; pIterator = NULL;
			strBlkName = strName;
			pBlockTable->close();
			pBlockTableRecord->close(); return TRUE;
		}
	}	
	delete pIterator; pIterator = NULL;
	pBlockTable->close();
	return FALSE;
}


BOOL CDistClipEntity::FindPaperSpaceBlkId(AcDbObjectIdArray& ObjIdArray)
{
	ObjIdArray.setPhysicalLength(0);
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

	AcDbBlockTable *pBlockTable = NULL ; 
	if(Acad::eOk != pDB->getBlockTable (pBlockTable, AcDb::kForRead)) return FALSE; 


	AcDbBlockTableIterator* pIterator = NULL;
	if(Acad::eOk != pBlockTable->newIterator(pIterator))
	{
		pBlockTable->close(); return FALSE;
	}

	for(pIterator->start();!pIterator->done();pIterator->step())
	{
		char* strName = NULL;
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		if(Acad::eOk != pIterator->getRecord(pBlockTableRecord,AcDb::kForRead)) continue;
		if(Acad::eOk !=pBlockTableRecord->getName(strName)) 
		{
			pBlockTableRecord->close(); continue;
		}

		CString strTempTable = strName;
		strTempTable.MakeUpper();
		if(strTempTable.Find("*PAPER_SPACE")>=0) 
		{
			AcDbObjectId tempId;
			if(Acad::eOk == pIterator->getRecordId(tempId))
				ObjIdArray.append(tempId);
		}
		pBlockTableRecord->close();
	}
	delete pIterator; pIterator = NULL;
	pBlockTable->close () ; 

	return TRUE;
}

BOOL CDistClipEntity::FindPaperSpaceAndInsertBlkRefInfo(AcDbObjectIdArray& ObjIdArray)
{
	ObjIdArray.setPhysicalLength(0);
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

	AcDbBlockTable *pBlockTable = NULL ; 
	if(Acad::eOk != pDB->getBlockTable (pBlockTable, AcDb::kForRead)) return FALSE; 


	AcDbBlockTableIterator* pIterator = NULL;
	if(Acad::eOk != pBlockTable->newIterator(pIterator))
	{
		pBlockTable->close(); return FALSE;
	}

	for(pIterator->start();!pIterator->done();pIterator->step())
	{
		char* strName = NULL;
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		if(Acad::eOk != pIterator->getRecord(pBlockTableRecord,AcDb::kForRead)) continue;
		if(Acad::eOk !=pBlockTableRecord->getName(strName)) 
		{
			pBlockTableRecord->close(); continue;
		}

		CString strTempTable = strName;
		strTempTable.MakeUpper();
		if(strTempTable.Find("*PAPER_SPACE")<0) 
		{
			pBlockTableRecord->close(); continue;
		}

		AcDbBlockTableRecordIterator *pIteratorRcd=NULL; 
		if(pBlockTableRecord->newIterator(pIteratorRcd)!=Acad::eOk)
		{
			pBlockTableRecord->close(); pBlockTable->close(); return FALSE;
		}

		for(pIteratorRcd->start();!pIteratorRcd->done();pIteratorRcd->step())
		{
			AcDbEntity* pEnt = NULL;
			if(pIteratorRcd->getEntity(pEnt,AcDb::kForRead) == Acad::eOk)
			{
				if(pEnt->isKindOf(AcDbBlockReference::desc()))
				{
					AcDbBlockReference* pBlkRef = (AcDbBlockReference*) pEnt;
					AcDbObjectId blkId = pBlkRef->blockTableRecord(); //!!!!!!!!
					AcDbObjectId tempId = pBlkRef->objectId();
					pEnt->close();
					CString strBlkName; 
					if(TRUE == GetBlockNameByBlkId(strBlkName,blkId))
					{	
						if((strBlkName.CompareNoCase("TBA3")==0) || (strBlkName.CompareNoCase("TBA4")==0))// 
						{
							ObjIdArray.append(tempId); continue;
						}
					}
				}
				else
					pEnt->close();	
			}
		}
		pBlockTableRecord->close();
		delete pIteratorRcd; pIteratorRcd = NULL;
	}
	delete pIterator; pIterator = NULL;
	pBlockTable->close () ; 

	return (ObjIdArray.length()>0);
}
/*
//从块引用中获取属性信息
BOOL CDistClipEntity::ReadAttrFromRefDk(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen)
{
strArrayTag.RemoveAll(); strArrayValue.RemoveAll();
acutPrintf("\n选择其中一地块属性标注:");
ads_name ssName;
struct resbuf* prebFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
int nResult = acedSSGet(":S", NULL, NULL, prebFilter, ssName);//"X"
acutRelRb(prebFilter);
if(nResult != RTNORM) return FALSE;

long lCount = 0;
acedSSLength(ssName, &lCount);
if(lCount ==0) return FALSE;

ads_name ssEnt;
if(RTNORM != acedSSName(ssName,0, ssEnt)) return FALSE;

AcDbObjectId ObjId;
acdbGetObjectId(ObjId, ssEnt);
AcDbBlockReference* pBlkRef = NULL;
if(Acad::eOk != acdbOpenObject(pBlkRef,ObjId,AcDb::kForRead))
{
AcDbObjectId blkId = pBlkRef->blockId();
CString strName;
GetBlockNameByBlkRefId(strName,blkId);
if(strName.CompareNoCase("_地块属性")!=0)
{
acutPrintf("\n请选择地块属性标注!");
pBlkRef->close(); return FALSE;
}
}
acedSSFree(ssName);
ptCen = pBlkRef->position();

CString strCadLyName = pBlkRef->layer();

AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
if (pObjIter == NULL)  return FALSE;

for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
{
AcDbObject * pEnt = NULL;
AcDbObjectId IdTmp = pObjIter->objectId();
if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
pEnt->close();

AcDbAttributeDefinition * pAttr = NULL;
pAttr = (AcDbAttributeDefinition*)pEnt;
if (pAttr == NULL) continue;

CString strTemp = pAttr->tagConst();
strArrayTag.Add(strTemp);
strTemp = pAttr->textStringConst();
strArrayValue.Add(strTemp);
}
pBlkRef->close();

CString strPrjCode;
int nCount =strArrayTag.GetCount();
for(int i=0; i<nCount; i++)
{
CString strTemp = strArrayTag.GetAt(i);
if(strTemp.CompareNoCase("项目显示编号") == 0)
{
strPrjCode = strArrayValue.GetAt(i); break;
}
}

struct resbuf* pFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
//filter.restype=AcDb::kDxfLayerName;
//filter.resval.rstring = strCadLyName.GetBuffer(0);
//filter.rbnext=NULL;

AcDbObjectIdArray ObjIdArray;
CDistSelSet tempSet;
tempSet.AllSelect(pFilter); acutRelRb(pFilter);
tempSet.AsArray(ObjIdArray);
nCount = ObjIdArray.length();

double dTotal = 0.0;
CString strTotalA1,strTotalA2,strTotalA3,strTotalA4;
AcDbObjectId tempId;
for(int j=0; j<nCount; j++)
{
tempId = ObjIdArray.at(j);
pBlkRef = NULL;
if(Acad::eOk != acdbOpenObject(pBlkRef,tempId,AcDb::kForRead)) continue;

AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
if (pObjIter == NULL)  return FALSE;

//项目显示编号
CString strTempPrjCode,strTempDKBH,strTempYDMJ;
for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
{
AcDbObject * pEnt = NULL;
AcDbObjectId IdTmp = pObjIter->objectId();
if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
pEnt->close();

AcDbAttributeDefinition * pAttr = NULL;
pAttr = (AcDbAttributeDefinition*)pEnt;
if (pAttr == NULL) continue;
CString strTemp = pAttr->tagConst();
{
if(strTemp.CompareNoCase("项目显示编号") == 0)
strTempPrjCode = pAttr->textStringConst();
else if(strTemp.CompareNoCase("地块编号") == 0)
strTempDKBH = pAttr->textStringConst();
else if(strTemp.CompareNoCase("用地面积" )== 0)
strTempYDMJ = pAttr->textStringConst();
}	
}
pBlkRef->close();

if(strTempPrjCode.CompareNoCase(strPrjCode) != 0) continue;

if(strTempDKBH.Find("A1")>=0)
{
strTempYDMJ.Trim(); 
dTotal = dTotal + atof(strTempYDMJ);
strTotalA1 = strTotalA1 + strTempDKBH+":"+strTempYDMJ+" ";
}
else if(strTempDKBH.Find("A2")>=0)
{
strTempYDMJ.Trim();
dTotal = dTotal + atof(strTempYDMJ);
strTotalA2 = strTotalA2 + strTempDKBH+":"+strTempYDMJ+" ";
}
else if(strTempDKBH.Find("A3")>=0)
{
strTempYDMJ.Trim(); 
dTotal = dTotal + atof(strTempYDMJ);
strTotalA3 = strTotalA3 + strTempDKBH+":"+strTempYDMJ+" ";
}
else if(strTempDKBH.Find("A4")>=0)
{
strTempYDMJ.Trim(); 
dTotal = dTotal + atof(strTempYDMJ);
strTotalA4 = strTotalA4 + strTempDKBH+":"+strTempYDMJ+" ";
}
}
CString strTotalArea;
strTotalArea.Format("%0.3f",dTotal);
strArrayTag.Add("总用地面积"); strArrayValue.Add(strTotalArea);
if(strTotalA1.IsEmpty()) strTotalA1 = "0.000";
strArrayTag.Add("建设用地面积"); strArrayValue.Add(strTotalA1);
if(strTotalA2.IsEmpty()) strTotalA2 = "0.000";
strArrayTag.Add("道路用地面积"); strArrayValue.Add(strTotalA2);
if(strTotalA3.IsEmpty()) strTotalA3 = "0.000";
strArrayTag.Add("绿化用地面积"); strArrayValue.Add(strTotalA3);
if(strTotalA4.IsEmpty()) strTotalA4 = "0.000";
strArrayTag.Add("其他用地面积"); strArrayValue.Add(strTotalA4);

return TRUE;
}

*/

void InsertDkInfoToShow(CStringArray& strArray,CString strDKBH,CString strDKMJ)
{
	CString strTemp;
	int nLen = strArray.GetCount();

	BOOL bFondInsert = FALSE;
	for(int i=0; i<nLen; i++)
	{
		strTemp = strArray.GetAt(i);
		CString strBuf = strTemp.Left(strTemp.Find(':'));//提取地块编号
		if(strBuf > strDKBH)
		{
			strTemp = strDKBH +":"+strDKMJ;
			strArray.InsertAt(i,strTemp);
			bFondInsert = TRUE;
			break;
		}
	}
	if(bFondInsert==FALSE)
	{
		strTemp = strDKBH +":"+strDKMJ;
		strArray.Add(strTemp);
	}
}

//从块引用中获取属性信息
BOOL CDistClipEntity::ReadAttrFromRefDk(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen,
										CStringArray& strArrayA1,CStringArray& strArrayA2,CStringArray& strArrayA3,CStringArray&strArrayA4)
{
	//获取项目信息
	strArrayTag.RemoveAll(); strArrayValue.RemoveAll();
	acutPrintf("\n选择项目信息标注");
	ads_name ssName;
	struct resbuf* prebFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
	int nResult = acedSSGet(":S", NULL, NULL, prebFilter, ssName);
	acutRelRb(prebFilter);
	if(nResult != RTNORM) return FALSE;

	long lCount = 0;
	acedSSLength(ssName, &lCount);
	if(lCount ==0) return FALSE;

	ads_name ssEnt;
	if(RTNORM != acedSSName(ssName,0, ssEnt)) return FALSE;

	AcDbObjectId ObjId;
	if(Acad::eOk != acdbGetObjectId(ObjId, ssEnt)) 
	{
		acedSSFree(ssName); return FALSE;
	}
	acedSSFree(ssName);
	
	//判断块属性的合法性
	AcDbBlockReference* pBlkRef = NULL;
	if(Acad::eOk == acdbOpenObject(pBlkRef,ObjId,AcDb::kForRead))
	{
		AcDbObjectId blkId = pBlkRef->blockTableRecord();
		CString strName;
		GetBlockNameByBlkId(strName,blkId);
		if(strName.CompareNoCase("_项目属性")!=0)
		{
			acutPrintf("\n请选择项目信息标注!"); pBlkRef->close(); return FALSE;
		}
	}
	ptCen = pBlkRef->position();

	//取项目属性块内容
	AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
	if (pObjIter == NULL)  return FALSE;
	CString strTotalA1,strTotalA2,strTotalA3,strTotalA4;
	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
		pEnt->close();

		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL) continue;

		//ARX 2005
		//CString strTemp = pAttr->tagConst();
		//ARX 2004
		CString strTemp = pAttr->tag();
		strArrayTag.Add(strTemp);
		if(strTemp.CompareNoCase("建设用地面积")==0)
		{
			//ARX 2005
			//strTotalA1 = pAttr->textStringConst();
			//ARX 2004
			strTotalA1 = pAttr->textString();
		}
		else if(strTemp.CompareNoCase("道路用地面积")==0)
		{
			//ARX 2005
			//strTotalA2 = pAttr->textStringConst();
			//ARX 2004
			strTotalA2 = pAttr->textString();
		}
		else if(strTemp.CompareNoCase("绿化用地面积")==0)
		{
			//ARX 2005
			//strTotalA3 = pAttr->textStringConst();
			//ARX 2004
			strTotalA3 = pAttr->textString();
		}
		else if(strTemp.CompareNoCase("其他用地面积")==0)
		{
			//ARX 2005
			//strTotalA4 = pAttr->textStringConst();
			//ARX 2004
			strTotalA4 = pAttr->textString();
		}
		//ARX 2005
		//strTemp = pAttr->textStringConst();
		//ARX 2004
		strTemp = pAttr->textString();
		strArrayValue.Add(strTemp);
	}
	pBlkRef->close();

	//提取项目显示编号
	CString strPrjCode;
	int nCount =strArrayTag.GetCount();
	for(int i=0; i<nCount; i++)
	{
		CString strTemp = strArrayTag.GetAt(i);
		if(strTemp.CompareNoCase("项目显示编号") == 0)
		{
			strPrjCode = strArrayValue.GetAt(i); break;
		}
	}

	//获取相关地块信息
	struct resbuf* pFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
	AcDbObjectIdArray ObjIdArray;
	CDistSelSet tempSet;
	tempSet.AllSelect(pFilter); acutRelRb(pFilter);
	tempSet.AsArray(ObjIdArray);
	nCount = ObjIdArray.length();

	double dTotal = 0.0;
	AcDbObjectId tempId;
	for(int j=0; j<nCount; j++)
	{
		tempId = ObjIdArray.at(j);
		pBlkRef = NULL;
		if(Acad::eOk != acdbOpenObject(pBlkRef,tempId,AcDb::kForRead)) continue;
		AcDbObjectId blkIdX = pBlkRef->blockTableRecord();
		CString strNameX;
		GetBlockNameByBlkId(strNameX,blkIdX);
		if(strNameX.CompareNoCase("_地块属性")!=0)
		{
			pBlkRef->close(); continue;
		}

		AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
		if (pObjIter == NULL)  return FALSE;

		//项目显示编号
		CString strTempPrjCode,strTempDKBH,strTempYDMJ;
		for( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
		{
			AcDbObject * pEnt = NULL;
			AcDbObjectId IdTmp = pObjIter->objectId();
			if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
			pEnt->close();

			AcDbAttributeDefinition * pAttr = NULL;
			pAttr = (AcDbAttributeDefinition*)pEnt;
			if (pAttr == NULL) continue;
			//ARX 2005
			//CString strTemp = pAttr->tagConst();
			//ARX 2004
			CString strTemp = pAttr->tag();

			if(strTemp.CompareNoCase("项目显示编号") == 0)
				//ARX 2005
				//strTempPrjCode = pAttr->textStringConst();
				//ARX 2004
				strTempPrjCode = pAttr->textString();
			else if(strTemp.CompareNoCase("地块编号") == 0)
				//ARX 2005
				//strTempDKBH = pAttr->textStringConst();
				//ARX 2004
				strTempDKBH = pAttr->textString();
			else if(strTemp.CompareNoCase("用地面积" )== 0)
				//ARX 2005
				//strTempYDMJ = pAttr->textStringConst();
				//ARX 2004
				strTempYDMJ = pAttr->textString();
		}
		pBlkRef->close();

		if(strTempPrjCode.CompareNoCase(strPrjCode) != 0) continue;

		if(strTempDKBH.Find("A1")>=0)
		{
			strTempYDMJ.Trim(); 
			dTotal = dTotal + atof(strTempYDMJ);
			InsertDkInfoToShow(strArrayA1,strTempDKBH,strTempYDMJ);
		}
		else if(strTempDKBH.Find("A2")>=0)
		{
			strTempYDMJ.Trim();
			dTotal = dTotal + atof(strTempYDMJ);
			InsertDkInfoToShow(strArrayA2,strTempDKBH,strTempYDMJ);
		}
		else if(strTempDKBH.Find("A3")>=0)
		{
			strTempYDMJ.Trim(); 
			dTotal = dTotal + atof(strTempYDMJ);
			InsertDkInfoToShow(strArrayA3,strTempDKBH,strTempYDMJ);
		}
		else if(strTempDKBH.Find("A4")>=0)
		{
			strTempYDMJ.Trim(); 
			dTotal = dTotal + atof(strTempYDMJ);
			InsertDkInfoToShow(strArrayA4,strTempDKBH,strTempYDMJ);
		}
	}

	CString strTotalArea;
	strTotalArea.Format("%0.3f",dTotal+0.0005);
	int nTempLen = strArrayTag.GetCount();
	double dTA1 = atof(strTotalA1);
	double dTA2 = atof(strTotalA2);
	double dTA3 = atof(strTotalA3);
	double dTA4 = atof(strTotalA4);
	for(int ii=0; ii<nTempLen; ii++)
	{
		CString strTempII = strArrayTag.GetAt(ii);
		//if(strTempII.CompareNoCase("总用地面积")==0)
		//{
		//strArrayValue.SetAt(ii,strTotalArea);
		//}
		if(strTempII.CompareNoCase("建设用地面积")==0)
		{
			if(dTA1<0.01) 
				strTotalA1 = "";
			strArrayValue.SetAt(ii,strTotalA1);
			strArrayA1.InsertAt(0,strTotalA1);
		}
		if(strTempII.CompareNoCase("道路用地面积")==0)
		{
			if(dTA2<0.01) 
				strTotalA2 = "";
			strArrayValue.SetAt(ii,strTotalA2);
			strArrayA2.InsertAt(0,strTotalA2);
		}
		if(strTempII.CompareNoCase("绿化用地面积")==0)
		{
			if(dTA3<0.01) 
				strTotalA3 = "";
			strArrayValue.SetAt(ii,strTotalA3);
			strArrayA3.InsertAt(0,strTotalA3);
		}
		if(strTempII.CompareNoCase("其他用地面积")==0)
		{
			if(dTA4<0.01) 
				strTotalA4 = "";
			strArrayValue.SetAt(ii,strTotalA4);
			strArrayA4.InsertAt(0,strTotalA4);
		}
	}

	return TRUE;
}


void SortDkInfo(CStringArray& strArrayResult,CStringArray& strArrayDK,CString strNameYD)
{
	int nLen = strArrayDK.GetCount();
	if(nLen < 2) return;
	CString strTemp;
	strTemp.Empty();
	for(int j=0; j<nLen; j++)
	{
		strTemp = strTemp+strArrayDK.GetAt(j)+" ";
		if((j+1)%3 == 0)
		{
			if(j>3) 
				strTemp = "    " + strTemp;
			else
				strTemp = strNameYD +":"+strTemp;
			strArrayResult.Add(strTemp);
			strTemp.Empty();
		}
	}
	if(!strTemp.IsEmpty())
	{
		if(nLen > 3)
			strTemp = "    " + strTemp;
		else
			strTemp = strNameYD +":"+strTemp;
		strArrayResult.Add(strTemp);
		strTemp.Empty();
	}
}

BOOL CDistClipEntity::UpdateAttrToBlkRef(AcDbObjectId ObjIdRef,CStringArray& strArrayTag,CStringArray& strArrayValue,
										 CStringArray& strArrayA1,CStringArray& strArrayA2,CStringArray& strArrayA3,CStringArray& strArrayA4)
{
	int nCount = strArrayTag.GetCount();
	if(nCount==0 || nCount != strArrayValue.GetCount() || ObjIdRef.isNull())
	{
		acutPrintf("\n 参数有误!"); return FALSE;
	}

	CStringArray strArrayResult;
	SortDkInfo(strArrayResult,strArrayA1,"建设用地");
	SortDkInfo(strArrayResult,strArrayA2,"道路用地");
	SortDkInfo(strArrayResult,strArrayA3,"绿化用地");
	SortDkInfo(strArrayResult,strArrayA4,"其它用地");


	AcDbBlockReference * pBckRef = NULL;
	if (Acad::eOk != acdbOpenObject(pBckRef, ObjIdRef, AcDb::kForRead)) return FALSE;
	pBckRef->close();

	AcDbObjectIterator * pObjIter = pBckRef->attributeIterator();
	if (pObjIter == NULL) return FALSE;

	int nResult = strArrayResult.GetCount();
	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		if (Acad::eOk != acdbOpenObject(pEnt, IdTmp, AcDb::kForWrite)) continue;

		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL)
		{
			pEnt->close();continue;
		}

		//建设用地面积 道路面积 绿化面积 其它用地面积
		//ARX 2005
		//CString strTemp = pAttr->tagConst();
		//ARX 2004
		CString strTemp = pAttr->tag();
		for (int i=0; i<nCount; i++)
		{
			if (strTemp.CompareNoCase(strArrayTag.GetAt(i)) == 0)
			{
				if(strTemp.CompareNoCase("建设用地面积")==0)
				{
					pAttr->setHeight(2.8);
					if(nResult>=1)
						pAttr->setTextString(strArrayResult.GetAt(0));
					else
						pAttr->setTextString("建设用地:");
				}
				else if(strTemp.CompareNoCase("道路面积")==0)
				{
					pAttr->setHeight(2.8);
					if(nResult>=2)
						pAttr->setTextString(strArrayResult.GetAt(1));
					else
						pAttr->setTextString("道路用地:");
				}
				else if(strTemp.CompareNoCase("绿化面积")==0)
				{
					pAttr->setHeight(2.8);
					if(nResult>=3)
						pAttr->setTextString(strArrayResult.GetAt(2));
					else
						pAttr->setTextString("绿化用地:");
				}
				else if(strTemp.CompareNoCase("其它用地面积")==0)
				{
					pAttr->setHeight(2.8);
					if(nResult>=4)
						pAttr->setTextString(strArrayResult.GetAt(3));
					else
						pAttr->setTextString("其它用地:");
				}
				else
				{
					strTemp = strArrayValue.GetAt(i);
					pAttr->setTextString(strTemp.GetBuffer(0));
				}	
				break;
			}
		}
		pEnt->close();
	}

	return TRUE;
}




//从块引用中获取属性信息
void CDistClipEntity::SelectDkAndHighLight()
{
	static AcDbObjectIdArray g_ObjIdArray;  //保存上次高亮显示的实体ID
	int nCountObj = g_ObjIdArray.length();
	for(int iii=0; iii<nCountObj; iii++)
	{
		AcDbEntity* pEnt = NULL;
		AcDbObjectId ttt = g_ObjIdArray.at(iii);
		if(Acad::eOk == acdbOpenObject(pEnt,ttt,AcDb::kForRead))
		{
			pEnt->unhighlight(); pEnt->close();
		}
	}
	g_ObjIdArray.setLogicalLength(0);

	//1.获取项目信息
	acutPrintf("\n选择项目信息标注");
	ads_name ssName;
	struct resbuf* prebFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
	int nResult = acedSSGet(":S", NULL, NULL, prebFilter, ssName);
	acutRelRb(prebFilter);
	if(nResult != RTNORM) return;

	long lCount = 0;
	acedSSLength(ssName, &lCount);
	if(lCount ==0) return;

	ads_name ssEnt;
	if(RTNORM != acedSSName(ssName,0, ssEnt)) return;

	AcDbObjectId ObjId;
	if(Acad::eOk != acdbGetObjectId(ObjId, ssEnt)) 
	{
		acedSSFree(ssName); return;
	}
	acedSSFree(ssName);

	//2.判断块属性的合法性
	AcDbBlockReference* pBlkRef = NULL;
	if(Acad::eOk == acdbOpenObject(pBlkRef,ObjId,AcDb::kForRead))
	{
		AcDbObjectId blkId = pBlkRef->blockTableRecord();
		CString strName;
		GetBlockNameByBlkId(strName,blkId);
		if(strName.CompareNoCase("_项目属性")!=0)
		{
			acutPrintf("\n请选择项目信息标注!"); pBlkRef->close(); return;
		}
	}

	//3.取项目属性块内容
	AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
	if (pObjIter == NULL)  return;
	CString strPrjCode;
	for ( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
	{
		AcDbObject * pEnt = NULL;
		AcDbObjectId IdTmp = pObjIter->objectId();
		if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
		pEnt->close();

		AcDbAttributeDefinition * pAttr = NULL;
		pAttr = (AcDbAttributeDefinition*)pEnt;
		if (pAttr == NULL) continue;

		//ARX 2005
		//CString strTemp = pAttr->tagConst();
		//ARX 2004
		CString strTemp = pAttr->tag();
		if(strTemp.CompareNoCase("项目显示编号")==0)  //提取项目显示编号
			//ARX 2005
			//strPrjCode = pAttr->textStringConst();
			//ARX 2004
			strPrjCode = pAttr->textString();
	}
	pBlkRef->close();

	
	//获取相关地块信息
	struct resbuf* pFilter = acutBuildList(RTDXF0, "INSERT", 100, "AcDbBlockReference", 0);
	AcDbObjectIdArray ObjIdArray;
	CDistSelSet tempSet;
	tempSet.AllSelect(pFilter); acutRelRb(pFilter);
	tempSet.AsArray(ObjIdArray);
	int nCount = ObjIdArray.length();

	CString strCadLyName="";
	CString strSdeLyName="";
	double dTotal = 0.0;
	AcDbObjectId tempId;
	for(int j=0; j<nCount; j++)
	{
		tempId = ObjIdArray.at(j);
		pBlkRef = NULL;
		if(Acad::eOk != acdbOpenObject(pBlkRef,tempId,AcDb::kForRead)) continue;
		AcDbObjectId blkIdX = pBlkRef->blockTableRecord();
		CString strNameX;
		GetBlockNameByBlkId(strNameX,blkIdX);
		if(strNameX.CompareNoCase("_地块属性")!=0)
		{
			pBlkRef->close(); continue;
		}

		AcDbObjectIterator * pObjIter = pBlkRef->attributeIterator();
		if (pObjIter == NULL)  return ;

		//项目显示编号
		CString strTempPrjCode;
		for( pObjIter->start(); !pObjIter->done(); pObjIter->step() )
		{
			AcDbObject * pEnt = NULL;
			AcDbObjectId IdTmp = pObjIter->objectId();
			if (acdbOpenObject(pEnt, IdTmp, AcDb::kForRead)!=Acad::eOk) continue;
			pEnt->close();

			AcDbAttributeDefinition * pAttr = NULL;
			pAttr = (AcDbAttributeDefinition*)pEnt;
			if (pAttr == NULL) continue;
			//ARX 2005
			//CString strTemp = pAttr->tagConst();
			//ARX 2004
			CString strTemp = pAttr->tag();

			if(strTemp.CompareNoCase("项目显示编号") == 0)
			{
				//ARX 2005
				//strTempPrjCode = pAttr->textStringConst();
				//ARX 2004
				strTempPrjCode = pAttr->textString();
				break;
			}
		}

		if(strTempPrjCode.CompareNoCase(strPrjCode) != 0) 
		{
			pBlkRef->close(); continue;
		}
		CDistXData tempXData(pBlkRef,"ROWID_OBJS");
		tempXData.Select("SDELYNAME",strSdeLyName);
		pBlkRef->highlight();
		g_ObjIdArray.append(pBlkRef->objectId());
		strCadLyName = pBlkRef->layer();
		pBlkRef->close();	
	}
	if(strCadLyName.IsEmpty()) return;

	int nFindPos = 0;
	if((nFindPos=strCadLyName.Find("地块标注"))<0) return;
	strCadLyName = strCadLyName.Left(nFindPos);
	if(strSdeLyName.IsEmpty()) return;

	
	CString strRegName = "DIST_"+strSdeLyName;
	struct resbuf filter;
	filter.restype=AcDb::kDxfLayerName;
	filter.resval.rstring = strCadLyName.GetBuffer(0);
	filter.rbnext=NULL;

	CDistSelSet tempSetX;
	tempSetX.AllSelect(&filter);
	AcDbObjectIdArray ObjIdArrayX;
	tempSetX.AsArray(ObjIdArrayX);
	for(int k=0; k<ObjIdArrayX.length(); k++)
	{
		AcDbObjectId tempId = ObjIdArrayX.at(k);
		AcDbEntity* pEnt = NULL;
		if(Acad::eOk != acdbOpenObject(pEnt,tempId,AcDb::kForRead)) continue;
		CDistXData tempXData(pEnt,strRegName);////
		CString strTempPrj="";
		tempXData.Select("PROJECT_CODE",strTempPrj);
		if(strPrjCode.CompareNoCase(strTempPrj)==0)
		{
			pEnt->highlight();
			g_ObjIdArray.append(pEnt->objectId());
		}
		pEnt->close();
	}

}



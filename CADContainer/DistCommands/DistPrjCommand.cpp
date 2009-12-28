#include "stdAfx.h"
#include "DistPrjCommand.h"

#include "..\SdeData\DistExchangeInterface.h"
#include "..\SdeData\DistBaseInterface.h"

#include "DistEntityTool.h"
#include "DistSelSet.h"
#include "DistXData.h"
#include "DistReduceEntity.h"
#include "DistPickUp.h"
#include "DistUiPrompts.h" 
#include "DistCreateLabel.h"
#include "DistBasePara.h"
#include "SdeDataOperate.h"
#include "DistPersistentObjReactor.h"

#include "DlgXmInput.h"
#include "DlgXmSearch.h"

//static CDistPersistentObjReactor* m_pPrReactor = CDistPersistentObjReactor::GetInstance();  //实体专用反应器对象

extern IDistConnection* g_pConnection;         //SDE数据库连接对象指针

char gXmType[5][10]={"选址","用地","红线","预选","储备"};

void gCreateBlockAttr()
{
	CStringArray strArray;
	// 项目属性名称定义
	strArray.RemoveAll();
	strArray.Add("流程号");strArray.Add("证号");strArray.Add("审批时间");strArray.Add("项目名称");
	strArray.Add("建设单位");strArray.Add("用地性质");strArray.Add("蓝线类型");strArray.Add("总面积");
	strArray.Add("建设用地面积");strArray.Add("道路用地面积");strArray.Add("绿化用地面积");strArray.Add("其他用地面积"); 

	CDistEntityTool tempTool;
	tempTool.InsertBlkAttr("_项目属性",strArray,"项目名称",5);

	// 蓝线属性定义（扩展字段名称别名）
	strArray.RemoveAll();	   
	strArray.Add("流程号");strArray.Add("证号");strArray.Add("日期");strArray.Add("项目名称"); 
	strArray.Add("建设单位");strArray.Add("用地性质");strArray.Add("蓝线类型");strArray.Add("用地面积"); 
	strArray.Add("用地类型");strArray.Add("地块编号");strArray.Add("字高");strArray.Add("线宽");	   
	strArray.Add("X 值");strArray.Add("Y 值");strArray.Add("经办人");strArray.Add("备注");	  

	tempTool.InsertBlkAttr("_蓝线属性",strArray,"地块编号",2);
}



AcDbObjectId GetPersistentObjReactor()
{
	static LPCTSTR dictEntryName = _T("DIST_PERSISTENT_OBJ_REACTOR");
	AcDbObjectId prId;
	prId.setNull();
	AcDbDictionary* prDict=NULL;
	AcDbDictionary* rootDict=NULL;

	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	Acad::ErrorStatus  es = pDB->getNamedObjectsDictionary(rootDict, AcDb::kForWrite);
	if (es == Acad::eOk) 
	{
		AcDbObject* pObj=NULL;
		Acad::ErrorStatus es;
		es = rootDict->getAt(dictEntryName, pObj, AcDb::kForWrite);
		if (es == Acad::eOk) 
		{
			prDict = AcDbDictionary::cast(pObj);
			if (prDict->getAt(dictEntryName, prId) == Acad::eOk) 
			{
				rootDict->close();
				prDict->close();
				return prId;
			}
			else
			{
				rootDict->close();
				return AcDbObjectId::kNull;
			}
		}
		else if (es == Acad::eKeyNotFound)
		{
			prDict = new AcDbDictionary;
			AcDbObjectId dictId;
			es = rootDict->setAt(dictEntryName, prDict, dictId);
			if (es != Acad::eOk) 
			{
				delete prDict;
				rootDict->close(); return AcDbObjectId::kNull;
			}
			else
				rootDict->close(); 
		}
		else
		{
			rootDict->close(); return AcDbObjectId::kNull;
		}


		CDistPersistentObjReactor* pr = new CDistPersistentObjReactor;
		es = prDict->setAt(dictEntryName, pr, prId);
		if (es != Acad::eOk) 
		{
			delete pr;
			prDict->close();
			return AcDbObjectId::kNull;
		}
		else
		{
			pr->close();
			prDict->close();
			return prId;
		}
	}
	else
		return AcDbObjectId::kNull;
}



// 功能：创建项目蓝线边界
// 参数：strLyName     图层名称
//       nColorIndex   颜色号
//       dWidth        全局线宽
// 返回：无
void gCmdCreateLXBound(CString strCadLyName,int nColorIndex,double dWidth)
{

	int nType=1;
	//交互选择操作类型
	CDistUiPrIntDef prIntType("\n选择类型 (1)提取边界 (2)选择边界 (3)新建边界   默认",NULL,CDistUiPrInt::kRange,1); 
	prIntType.SetRange(1,3);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return;
	nType = prIntType.GetValue();

	//得到蓝线实体（三个途径：提取，选择，绘制）
	AcDbObjectId ObjId;
	AcDbEntity * pEnt=NULL; 
	AcDbPolyline* pLine=NULL; 
	CDistEntityTool tempTool;
	tempTool.CreateLayer(ObjId,strCadLyName,nColorIndex);
	//a.提取边界
	if(1 == nType)  
	{
		ads_name en;
		if(acdbEntLast(en)!=RTNORM){  //获取数据库中最后一个实体
			acutPrintf("\n没有实体被选择！"); return;
		}

		ads_point adsPt;
		int nResult = acedGetPoint(NULL,"\n选择多边形内部点:", adsPt);
		if(nResult != RTNORM) return;
		char strPt[40]={0};
		sprintf(strPt,"%0.3f,%0.3f",adsPt[0],adsPt[1]);
		if(acedCommand(RTSTR,"-BOUNDARY",RTSTR,strPt,RTSTR,"",RTNONE)!=RTNORM) return; //调用命令生成边界(RTSTR,PAUSE)
		while(acdbEntNext(en,en)==RTNORM)//获取生成的边界
		{
			ObjId.setNull();
			if(Acad::eOk != acdbGetObjectId(ObjId, en)) 
			{
				ObjId.setNull();
				return;  //得到实体ObjectId
			}

			if(Acad::eOk != acdbOpenObject(pEnt, ObjId,AcDb::kForRead)) 
			{
				pEnt->close(); ObjId.setNull();
				return;
			}

			if(pEnt->isKindOf(AcDbPolyline::desc())==false)
			{
				pEnt->close(); continue;
			}
			else
			{
				pEnt->close(); break;
			}
		}
		if(ObjId.isNull()) return;
	}

	//b.选择边界
	else if(2 == nType) 
	{
		/*
		ads_point pt;  
		ads_name entName;
		

		if(acedEntSel("\n请选择一个闭合的实体： ", entName, pt)!=RTNORM) return;
		if(acdbGetObjectId(ObjId, entName)!=Acad::eOk) return;
		if(acdbOpenObject(pEnt, ObjId, AcDb::kForRead)!=Acad::eOk) return;
		if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			pLine = (AcDbPolyline*) pEnt;
			if(!pLine->isClosed())
			{
				acutPrintf("\n选择实体不闭合");
				pEnt->close(); return; 
			}
		}
		else
		{
			acutPrintf("\n选择实体类型不是Polyline线");
			pEnt->close(); return;
		}
		pEnt->close();
		*/

			
		ObjId.setNull();

		// 提取边界
		CLxPickUp temp;
		temp.pch = strCadLyName.GetBuffer(0);
		temp.mainFun();
		ObjId = temp.m_OutId;
		temp.RemoveEntity();	
		if(ObjId.isNull()) return;
	}

	//c.新建边界
	else if(3 == nType)  
	{
		int nCount = 0;
		AcGePoint3dArray ptArray;
		int i = 0;
		CDistUiPrPoint prPoint(_T("下一点"), NULL);
		prPoint.SetAllowNone(true);
		while (prPoint.Go() == CDistUiPrBase::kOk) 
		{
			ptArray.append(prPoint.GetValue());
			if (i > 0)
				acedGrDraw(asDblArray(ptArray[i-1]), asDblArray(ptArray[i]),nColorIndex, 0);
			i++;
			prPoint.SetBasePt(prPoint.GetValue());
		}

		nCount = ptArray.length();
		for(i=0; i<nCount-1; i++)
			acedGrDraw(asDblArray(ptArray[i]), asDblArray(ptArray[i+1]),nColorIndex, 0);

		if(fabs(ptArray[0].x - ptArray[nCount-1].x) < 0.001 && fabs(ptArray[0].y - ptArray[nCount-1].y) < 0.001)
			ptArray.removeAt(nCount-1);  //删除重复点
		nCount = ptArray.length();
		if (nCount < 3) {
			acutPrintf(_T("\n至少需要三个点!"));
			return;
		}
		if(1 !=tempTool.CreatePolyline(ObjId,ptArray,strCadLyName,nColorIndex,"Continuous",AcDb::kLnWtByLayer,dWidth,true)) return;
	}

	//根据实体ID修改其图层名称和线宽并高亮显示
	if(Acad::eOk != acdbOpenObject(pEnt, ObjId,AcDb::kForWrite)) return;
	if(pEnt->isKindOf(AcDbPolyline::desc())==false)
	{
		pEnt->close(); return;
	}
	pLine = (AcDbPolyline*) pEnt;
	if(pLine->isClosed()==false)
	{
		pLine->close(); return;
	}
	int nNumVert = pLine->numVerts(); //获取点数量
	for(int i=0; i<nNumVert; i++)
		pLine->setWidthsAt(i,dWidth,dWidth);

	pLine->setColorIndex(nColorIndex);
	pLine->setLayer(strCadLyName);
	
	double dArea =0.0;
	pLine->getArea(dArea);
	acutPrintf("\n当前地块面积为：%0.2f 平方米",dArea);

	AcDbObjectId tempId = GetPersistentObjReactor();

	if(tempId.isNull())
	{
		pLine->close(); return;
	}
	pLine->addPersistentReactor(tempId);

	AcDbObject* pObj = NULL;
	if(acdbOpenObject(pObj,tempId,kForWrite)!=Acad::eOk)
	{
		pLine->close(); return;
	}
	CDistPersistentObjReactor* pRector =(CDistPersistentObjReactor*)pObj;
	pRector->attachTo(pLine->objectId());
	pLine->close();
}


// 功能：创建蓝线坐标标注(依次遍历所有点)
// 参数：
// 返回：
void gCmdCreatLabelAll(CString strCadLyName)
{
	double dPrecision = 0.000001;
	ads_name ssName;
	struct resbuf * preb = acutBuildList(RTDXF0, "LWPOLYLINE", 0);
	if (acedSSGet(":S", NULL, NULL, preb, ssName) != RTNORM)
	{
		acedSSFree(ssName);
		acutRelRb(preb);
		return;
	}
	acutRelRb(preb);
	ads_name ssEnt;
	acedSSName(ssName, 0, ssEnt);

	AcDbObjectId dbObjId;
	if(Acad::eOk != acdbGetObjectId(dbObjId, ssEnt)) return;;

	acedSSFree(ssName);

	AcDbPolyline * pPline = NULL;
	if (Acad::eOk != acdbOpenObject(pPline, dbObjId, AcDb::kForRead))
	{
		return;
	}
	pPline->close();

	int nCount = pPline->numVerts();

	CCircleLabel tempLabel;
	for (int i = 0; i < nCount; i++)
	{
		AcGePoint3d pt3d, pt3dE;

		if (Acad::eOk != pPline->getPointAt(i, pt3d)) continue;

		ads_point ptSet, ptSetE;

		ptSet[0] = pt3d.x;
		ptSet[1] = pt3d.y;
		ptSet[2] = pt3d.z;

		tempLabel.CreateSpcoordLabel(ptSet, TRUE);

		if (Adesk::kTrue == pPline->hasBulges())
		{
			if (i == nCount - 1)
			{
				if (Acad::eOk != pPline->getPointAt(0, pt3dE)) continue;
			}
			else
			{
				if (Acad::eOk != pPline->getPointAt(i + 1, pt3dE)) continue;
			}

			ptSetE[0] = pt3dE.x;
			ptSetE[1] = pt3dE.y;
			ptSetE[2] = pt3dE.z;

			double Bulge = 0;

			if (Acad::eOk != pPline->getBulgeAt(i, Bulge))
			{
				continue;
			}

			if (Bulge < -dPrecision || Bulge > dPrecision)
			{
				tempLabel.CreatCircleLabel(ptSet, ptSetE, Bulge);
			}
		}
	}
}


void gCmdSearchXMXX(CString strCadLyName,CString strSdeLyName)
{
	CDlgXmSearch dlg;
	dlg.SetInfo(strCadLyName,strSdeLyName);
	dlg.DoModal();
}


// 功能：创建蓝线坐标标注(选择需要标注的点)
// 参数：
// 返回：
void gCmdCreatLabel(CString strCadLyName)
{
	CSpcoordLabel temp;
	while (temp.CreateSpcoordLabel());
}

// 功能：设置项目蓝线属性
// 参数：
// 返回：
void gCmdInputLXProperty(CString strCadLyName,CString strSdeLyName,HWND hDapHwnd,CString strURL,CString strXMBH)
{
	if(NULL == g_pConnection) return;
	CDlgXmInput dlg;
	dlg.SetInfo(g_pConnection,strCadLyName,strSdeLyName,hDapHwnd,strURL,strXMBH);
	dlg.DoModal();
}


// 功能：根据指定图层名称和访问权限读SDE图层数据
// 函数：
// 返回：
void gCmdReadSdeLys(CStringArray& strLyArray,int* pAccessPower,CString strCadLXName,CString strXMBH)
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n空间数据库没有连接！"); return;
	}
	int nCount = strLyArray.GetCount();
	if(0 == nCount ) return; //|| NULL == pAccessPower

	//交互选择操作类型
	int nType=1;
	CDistUiPrIntDef prIntType("\n选择类型 (1)给定范围 (2)项目编号 (3)全图  默认",NULL,CDistUiPrInt::kRange,1);  
	prIntType.SetRange(1,3);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return;
	nType = prIntType.GetValue();

	CSdeDataOperate tempOperate;
	double dminX=4294967295,dminY=4294967295,dmaxX=-4294967295,dmaxY=-4294967295;
	if(1 == nType) //给定范围
	{
		//交互取得矩形范围
		ads_point  startpt,endpt;    
		int nRet = acedGetPoint(NULL, "\n请指定矩形的左上角:",startpt);
		if (nRet != RTNORM) return;
		nRet = acedGetCorner(startpt, "\n请指定矩形的右下角:",endpt);
		if (nRet != RTNORM) return;

		//提取最小，最大点（为下一步创建过滤器做准备）
		if(startpt[0]>endpt[0]){
			dmaxX = startpt[0];
			dminX = endpt[0];
		}
		else{
			dmaxX = endpt[0];
			dminX = startpt[0];
		}

		if(startpt[1]>endpt[1]){
			dminY = endpt[1];
			dmaxY = startpt[1];
		}
		else{
			dmaxY = endpt[1];
			dminY = startpt[1];
		}
	}
	else if(2 == nType)  //项目编号
	{
		IDistCommand* pCommand = CreateSDECommandObjcet();
		pCommand->SetConnectObj(g_pConnection);

		char strSQL[1024]={0};
		sprintf(strSQL,"F:X,Y,T:TB_PROJECT,W:PROJECTID='%s',##",strXMBH);
		IDistRecordSet* pRcdSet = NULL;
		if(1 != pCommand->SelectData(strSQL,&pRcdSet))
		{
			pCommand->Release(); return;
		}
		double dX,dY;
		if(pRcdSet->BatchRead()==1)
		{
			CString strTemp;
			strTemp = (char*)(*pRcdSet)[0];
			dX = atof(strTemp);
			strTemp = (char*)(*pRcdSet)[1];
			dY = atof(strTemp);
			pCommand->Release(); pRcdSet->Release();
		}
		else 
		{
			pCommand->Release(); pRcdSet->Release();
			return;
		}

		dminX = dX - 1000; dmaxX = dX + 1000;
		dminY = dY - 1000; dmaxY = dY + 1000;
	}
	else if(3 == nType)  //全图
	{
	}

	//定义空间搜索条件
	DIST_STRUCT_SELECTWAY Filter,*pFilter=NULL;
	Filter.nEntityType = 4;
	Filter.nSelectType = kSM_AI;
	Filter.ptArray.append(AcGePoint3d(dminX,dminY,0));
	Filter.ptArray.append(AcGePoint3d(dmaxX,dmaxY,0));
	 if(3 != nType) 
		pFilter = &Filter;

	AcDbObjectIdArray ObjIdArray;
	bool bFound = false;
	CString strTempName;
	strTempName.Empty();
	for(int i=0; i<nCount; i++)  
	{
		CString strSdeLyName = strLyArray.GetAt(i);
		if((strSdeLyName.Find("SP_")>=0) && (strSdeLyName.Find("_ZB_POLYGON")>=0))
			strTempName = strSdeLyName;
		tempOperate.ReadAllSdeLayer(ObjIdArray,strSdeLyName.GetBuffer(0),pFilter,NULL,1);
	}
	if(strTempName.IsEmpty()==false)
	{	
		if(nType == 3)
			gCreateLXBlkDef(strCadLXName.GetBuffer(),strTempName.GetBuffer(),-1,-1,-1,-1);
		else
			gCreateLXBlkDef(strCadLXName.GetBuffer(),strTempName.GetBuffer(),dmaxX,dmaxY,dminX,dminY);
	}

}



void gCmdSaveCurXMObjectInfo(const char* strCadLyName,const char* strCurXMBH)
{
	//SaveSelectEntiyToSDE(IDistConnection *pConnect,const char* strLyName,const char* strCadName);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//运行信息输出
static int g_nOutTag = 1;

void gCmdChangeOutState()
{
	char strIn[1024]={0};
	memset(strIn,0,sizeof(char)*1024);
	if(RTNORM == acedGetString(0,"hi,what will you do?",strIn))
	{
		CString strTempA = "printf";
		CString strTempB = "write";
		if(strTempA.CompareNoCase(strIn)==0)
		{
			g_nOutTag = 1;
			acutPrintf("\n Info can be printed now!");
		}
		else if(strTempB.CompareNoCase(strIn)==0)
		{
			g_nOutTag = 2;
			acutPrintf("\n Info can be writed to file now!");
		}
		else
		{
			g_nOutTag = 0;
			acutPrintf("\nHave no info can be printed and  writed!");
		}
	}	
}

void gOutRunInfo (long rc, char *szComment)
{
	if(rc == 0 || rc == 1) return;
	if(g_nOutTag == 1)
	{
		char* strInfo = ReturnErrorCodeInfo(rc);
		acutPrintf("\nfunction:%s, Error code:%d Message:%s\n",szComment,rc,strInfo);
	}
	else if(g_nOutTag == 2)
	{
		CString strTemp;
		strTemp.Format("\nfunction:%s, Error code:%d Message:%s",szComment,rc,ReturnErrorCodeInfo(rc));
		FILE *pf = fopen("C:\\ErrorInfo.txt","a");
		if(NULL == pf) return;
		fwrite(strTemp.GetBuffer(),strTemp.GetLength(),1,pf);
		fclose(pf);
	}

}

void gCreateLXBlkDef(char* strCadName,char* strSdeName,double dmaxx,double dmaxy,double dminx,double dminy)
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n空间数据库没有连接！"); return;
	}

	bool bRect=true;
	if(dmaxx<0 && dmaxy<0 && dminx<0 && dminy<0)
		bRect = false;
	CStringArray strNameXMArray,strNameDKArray;
	strNameXMArray.RemoveAll();
	strNameXMArray.Add("流程号");strNameXMArray.Add("证号");strNameXMArray.Add("审批时间");strNameXMArray.Add("项目名称");
	strNameXMArray.Add("建设单位");strNameXMArray.Add("用地性质");strNameXMArray.Add("蓝线类型");strNameXMArray.Add("总面积");
	strNameXMArray.Add("建设用地面积");strNameXMArray.Add("道路用地面积");strNameXMArray.Add("绿化用地面积");strNameXMArray.Add("其他用地面积"); 

	// 蓝线属性定义（扩展字段名称别名）
	strNameDKArray.RemoveAll();	   
	strNameDKArray.Add("流程号");strNameDKArray.Add("证号");strNameDKArray.Add("日期");strNameDKArray.Add("项目名称"); 
	strNameDKArray.Add("建设单位");strNameDKArray.Add("用地性质");strNameDKArray.Add("蓝线类型");strNameDKArray.Add("用地面积"); 
	strNameDKArray.Add("用地类型");strNameDKArray.Add("地块编号");strNameDKArray.Add("字高");strNameDKArray.Add("线宽");	   
	strNameDKArray.Add("X 值");strNameDKArray.Add("Y 值");strNameDKArray.Add("经办人");strNameDKArray.Add("备注");	


	IDistCommand * pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(g_pConnection);
	if(NULL == pCommand) return;
	CString strSQL;
	strSQL.Format("F:LCH,ZH,RQ,XMMC,JSDW,YDXZ,LXLX,MJ,YDLX,DKBH,ZG,XK,X,Y,JBR,BZ,T:%s,##",strSdeName);
	IDistRecordSet* pRcdset = NULL;
	if( 1!= pCommand->SelectData(strSQL.GetBuffer(),&pRcdset))
	{
		pCommand->Release(); return;
	}
	int nCount = pRcdset->GetRecordCount();
	if(nCount == 0)
	{
		pCommand->Release(); pRcdset->Release(); return;
	}

	CDistEntityTool tempTool;
	CString strTempValue,strTemp;
	CStringArray strTempValueArray;
	CString strTempCadLyName;
	strTempCadLyName.Format("%s属性标注",strCadName);
	tempTool.DeleteAllEntityInLayer(strTempCadLyName.GetBuffer());
	while(pRcdset->BatchRead()==1)
	{
		strTempValueArray.RemoveAll();
		for(int i=0;i<16;i++)
		{
			strTempValue = (char*)(*pRcdset)[i];
			strTempValueArray.Add(strTempValue);
		}
		double dx,dy,dZG;
		dZG = atof(strTempValueArray[10]);
		dx = atof(strTempValueArray[12]);
		dy = atof(strTempValueArray[13]);
		
		if(bRect==true)
		{
			if(dx < dminx || dx > dmaxx  || dy < dminy || dy > dmaxy)  continue;
		}
		
		tempTool.InsertBlkAttrRef("_蓝线属性",strTempCadLyName.GetBuffer(),AcGePoint3d(dx,dy,0),dZG,strNameDKArray,strTempValueArray);
	}
	pRcdset->Release();

	strSQL = "F:LCH,ZH,RQ,XMMC,JSDW,YDXZ,LXLX,ZMJ,JSYDMJ,DLYDMJ,LHYDMJ,QTYDMJ,ZG,X,Y,T:TB_PROJECT,##";
	if(pCommand->SelectData(strSQL.GetBuffer(),&pRcdset) != 1)
	{
		pCommand->Release(); return;
	}

	strTempCadLyName.Format("%s项目标注",strCadName);
	tempTool.DeleteAllEntityInLayer(strTempCadLyName.GetBuffer());
	while(pRcdset->BatchRead()==1)
	{
		strTempValueArray.RemoveAll();
		for(int j=0; j<12; j++)
		{
			strTempValue = (char*)(*pRcdset)[j];
			strTempValueArray.Add(strTempValue);
		}
		double dx,dy,dZG;
		dZG = atof((char*)(*pRcdset)[12]);
		dx = atof((char*)(*pRcdset)[13]);
		dy = atof((char*)(*pRcdset)[14]);
		
		if(bRect==true)
		{
			if(dx < dminx || dx > dmaxx  || dy < dminy || dy > dmaxy)  continue;
		}
		tempTool.InsertBlkAttrRef("_项目属性",strTempCadLyName.GetBuffer(),AcGePoint3d(dx,dy,0),dZG,strNameXMArray,strTempValueArray);
	}
	pRcdset->Release();
	pCommand->Release();

}

BOOL gCopyDataFromOneToOther(CString strSdeSrc,CString strSdeTrg,CString strWhere)
{
	if(NULL == g_pConnection) return FALSE;
	IDistCommand* pCommand=CreateSDECommandObjcet();
	if(pCommand==NULL) return FALSE;
	pCommand->SetConnectObj(g_pConnection);

	int nParamCount = 0;
	IDistParameter* pParam = NULL;
	if(1 != pCommand->GetTableInfo(&pParam,&nParamCount,strSdeSrc.GetBuffer(0)))
	{
		pCommand->Release(); return FALSE;
	}

	CString strFldNames ="";
	for(int i=0; i<nParamCount;i++)
		strFldNames = strFldNames+pParam[i].fld_strName+",";

	char strSQL[1024]={0};
	sprintf(strSQL,"F:%sT:%s,W:%s,##",strFldNames.GetBuffer(0),strSdeSrc.GetBuffer(0),strWhere.GetBuffer(0));
	IDistRecordSet* pRcdSet = NULL;
	if(pCommand->SelectData(strSQL,&pRcdSet)!=1)
	{
		pCommand->Release(); 
		if(pRcdSet != NULL) pRcdSet->Release(); return FALSE;
	}

	int nRcdCount = pRcdSet->GetRecordCount();
	if(nRcdCount == 0)
	{
		pCommand->Release(); 
		if(pRcdSet != NULL) pRcdSet->Release(); return FALSE;
	}

	IDistCommand* pOther = CreateSDECommandObjcet();
	pOther->SetConnectObj(g_pConnection);
	pOther->BeginTrans();
	char strOtherSQL[1024]={0};
	sprintf(strOtherSQL,"F:%sT:%s,##",strFldNames.GetBuffer(0),strSdeTrg.GetBuffer(0));
	char** strValues = new char* [nParamCount];
	for(int j=0; j<nParamCount; j++)
	{
		strValues[j] = new char[500];
		memset(strValues[j],0,sizeof(char)*500);
	}
	while(pRcdSet->BatchRead()==1)
	{
		for(int k=0; k<nParamCount; k++)
			pRcdSet->GetValueAsString(strValues[k],k);
		int nRowId =0;
		if(1 != pOther->InsertDataAsString(&nRowId,strOtherSQL,strValues))
		{
			pOther->RollbackTrans();
			pOther->Release(); pCommand->Release(); pRcdSet->Release();
			for(j=0; j<nParamCount; j++)
			{
				delete[] strValues[j];
			}
			delete[] strValues;
			return FALSE;
		}
	}
	pOther->CommitTrans();
	pOther->Release(); pCommand->Release(); pRcdSet->Release();
	for(j=0; j<nParamCount; j++)
	{
		delete[] strValues[j];
	}
	delete[] strValues;

	return TRUE;
}






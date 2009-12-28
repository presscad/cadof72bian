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

#include "AccessWebServiceInfo.h"

#include "DlgLXInput.h"
#include "DlgXmSearch.h"

#include "FTPclient.h"
#include "DistSysFun.h"

extern BOOL g_bSupperPower;
static int gStaticFontHeight = 5;
//static CDistPersistentObjReactor* m_pPrReactor = CDistPersistentObjReactor::GetInstance();  //ʵ��ר�÷�Ӧ������

extern IDistConnection* g_pConnection;         //SDE���ݿ����Ӷ���ָ��

//��ȡͼ������ѡ״̬
__declspec(dllimport) CString WINAPI DistGetTreeCheckState();

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
				rootDict->close(); prDict->close(); return prId;
			}
			else
			{
				rootDict->close(); return AcDbObjectId::kNull;
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



// ���ܣ�������Ŀ���߽߱�
// ������strLyName     ͼ������
//       nColorIndex   ��ɫ��
//       dWidth        ȫ���߿�
// ���أ���
void gCmdCreateLXBound(CString strCadLyName,int nColorIndex,double dWidth,CString strSdeLyName)
{
	int nType=1;
	//����ѡ���������
	CDistUiPrIntDef prIntType("\nѡ������ (1)ѡ�������ȡ (2)ѡ���ڲ�����ȡ  (3)ѡ��ת������  Ĭ��",NULL,CDistUiPrInt::kRange,1); 
	prIntType.SetRange(1,3);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return;
	nType = prIntType.GetValue();

	//�õ�����ʵ�壨����;������ȡ��ѡ�񣬻��ƣ�
	AcDbObjectId ObjId;
	AcDbEntity * pEnt=NULL; 
	AcDbPolyline* pLine=NULL; 
	CDistEntityTool tempTool;
	tempTool.CreateLayer(ObjId,strCadLyName,nColorIndex);
	//gShowOrHideCadLayer(strCadLyName,FALSE);  //����ͼ��


	if(1 == nType) //a.ѡ�������ȡ
	{
		ObjId.setNull();

		/*
		// ��ȡ�߽�
		CLxPickUp temp;
		temp.pch = strCadLyName.GetBuffer(0);
		temp.mainFun();
		ObjId = temp.m_OutId;
		temp.RemoveEntity();	

		//gShowOrHideCadLayer(strCadLyName,TRUE);
		if(ObjId.isNull()) 
		{
			return;
		}
		*/
		
		ads_name ssEnt;
		if(acdbEntLast(ssEnt)!=RTNORM){  //��ȡ���ݿ������һ��ʵ��
			acutPrintf("\nû��ʵ�屻ѡ��"); return;
		}

		struct resbuf * pFilter = acutBuildList(RTDXF0, "LINE,POLYLINE,LWPOLYLINE,CIRCLE,ARC,ELLIPSE,SPLINE", 0);
		CDistSelSet tempSet;
		acutPrintf("\nѡ����߽�");
		if(CDistSelSet::kDistSelected != tempSet.UserSelect(pFilter)) return;
		acutRelRb(pFilter);

		ads_name ssResult;
		tempSet.AsAdsName(ssResult);

		ads_point ptIn;
		if(RTNORM != acedGetPoint(NULL,"\nʰȡ�߽��ڲ���",ptIn))
		{
			acutPrintf("\n������ȡ��"); return;
			acedSSFree(ssResult); return;
		}


		if(acedCommand(RTSTR,"-BOUNDARY",RTSTR,"a",RTSTR,"b",RTSTR,"n",RTPICKS,ssResult,RTSTR,"",RTSTR,"",RT3DPOINT,ptIn,RTSTR,"",RTNONE)!=RTNORM)//RTSTR,"",RTSTR,"",
		{
			acutPrintf("\nѡ��߽粻���");
			acedSSFree(ssResult);  return; //�����������ɱ߽�(RTSTR,PAUSE)
		}
		acedSSFree(ssResult); 


		while(acdbEntNext(ssEnt,ssEnt)==RTNORM)//��ȡ���ɵı߽�
		{
			ObjId.setNull();
			if(Acad::eOk != acdbGetObjectId(ObjId, ssEnt)) 
			{
				acedSSFree(ssEnt);
				ObjId.setNull(); return;  //�õ�ʵ��ObjectId
			}
			
			if(Acad::eOk != acdbOpenObject(pEnt, ObjId,AcDb::kForRead)) 
			{
				acedSSFree(ssEnt);ObjId.setNull(); return;
			}

			if(pEnt->isKindOf(AcDbPolyline::desc())==false)
			{
				ObjId.setNull();
				pEnt->close(); continue;
			}
			else
			{
				pEnt->close(); break;
			}
		}
		acedSSFree(ssEnt);

		if(ObjId.isNull()) 
		{
			acutPrintf("\nѡ��߽粻���"); return;
		}
	}
	else if(2 == nType)  //b.ѡ���ڲ�����ȡ
	{
		ads_name en;
		if(acdbEntLast(en)!=RTNORM){  //��ȡ���ݿ������һ��ʵ��
			acutPrintf("\nû��ʵ�屻ѡ��"); return;
		}

		ads_point adsPt;
		int nResult = acedGetPoint(NULL,"\nѡ�������ڲ���:", adsPt);
		if(nResult != RTNORM) 
		{
			acedSSFree(en);
			return;
		}
		char strPt[40]={0};
		sprintf(strPt,"%0.3f,%0.3f",adsPt[0],adsPt[1]);
		if(acedCommand(RTSTR,"-BOUNDARY",RTSTR,strPt,RTSTR,"",RTNONE)!=RTNORM)
		{
			acedSSFree(en); return; //�����������ɱ߽�(RTSTR,PAUSE)
		}

		while(acdbEntNext(en,en)==RTNORM)//��ȡ���ɵı߽�
		{
			ObjId.setNull();
			if(Acad::eOk != acdbGetObjectId(ObjId, en)) 
			{
				acedSSFree(en); ObjId.setNull();
				return;  //�õ�ʵ��ObjectId
			}

			if(Acad::eOk != acdbOpenObject(pEnt, ObjId,AcDb::kForRead)) 
			{
				acedSSFree(en); ObjId.setNull();
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
		acedSSFree(en);

		if(ObjId.isNull()) 
		{
			return;
		}
	}
	else if(3 == nType)  //c.ѡ��ת������
	{
		//gShowOrHideCadLayer(strCadLyName,TRUE);  //����ͼ��
		ads_name ssName;
		resbuf* filter= acutBuildList(RTDXF0, "LWPOLYLINE", 0);
	
		if (acedSSGet(":S", NULL, NULL, filter, ssName) != RTNORM)
		{
			acutPrintf("\n ѡ��ʵ������!"); acedSSFree(ssName); 
			 return;
		}

		AcDbObjectId tempObjId;
		ads_name ssTemp;
		acedSSName(ssName, 0, ssTemp);
		acdbGetObjectId(tempObjId, ssTemp);
		acedSSFree(ssName);
		acutRelRb(filter);

		AcDbPolyline* pline = NULL;
		if(Acad::eOk != acdbOpenObject(pline,tempObjId,AcDb::kForWrite))
		{
			acutPrintf("\n ѡ��ʵ������!");return;
		}

		if(pline->isClosed()==false)
		{
			acutPrintf("\n ѡ��ʵ�岻�պ�!");
			pline->close();return;
		}

		int nNumVert = pline->numVerts(); //��ȡ������
		for(int kk=0; kk<nNumVert; kk++)
			pline->setWidthsAt(kk,dWidth,dWidth);

		pline->setColorIndex(nColorIndex);
		pline->setLayer(strCadLyName);

		double dArea =0.0;
		pline->getArea(dArea);
		acutPrintf("\n��ǰ�ؿ����Ϊ��%0.3f ƽ����",dArea);

		AcDbObjectId tempId = GetPersistentObjReactor();

		if(tempId.isNull())
		{
			pline->close(); return;
		}
		pline->addPersistentReactor(tempId);  //�ӷ�Ӧ��

		AcDbObject* pObj = NULL;
		if(acdbOpenObject(pObj,tempId,kForWrite)!=Acad::eOk)
		{
			pline->close(); return;
		}
		CDistPersistentObjReactor* pRector =(CDistPersistentObjReactor*)pObj;
		pRector->attachTo(pline->objectId());
		pline->close();
		pObj->close();
			
		return;

		/*
		int nCount = 0;
		AcGePoint3dArray ptArray;
		int i = 0;
		CDistUiPrPoint prPoint(_T("��һ��"), NULL);
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
			ptArray.removeAt(nCount-1);  //ɾ���ظ���
		nCount = ptArray.length();
		if (nCount < 3) {
			acutPrintf(_T("\n������Ҫ������!"));
			return;
		}
		if(1 !=tempTool.CreatePolyline(ObjId,ptArray,strCadLyName,nColorIndex,"Continuous",AcDb::kLnWtByLayer,dWidth,true)) return;

		*/
	}

	AcDbObjectId OutObjId;
	AcDbPolyline *pTempLine = NULL;
	if(Acad::eOk != acdbOpenObject(pTempLine,ObjId,AcDb::kForRead)) return;
	pTempLine->close();

	CString strRegName;
	strRegName.Format("DIST_%s",strSdeLyName);
	gTurnArc(OutObjId,ObjId,strRegName.GetBuffer(0),5,2.0);
	if(!OutObjId.isNull())
		ObjId = OutObjId;

	//����ʵ��ID�޸���ͼ�����ƺ��߿�������ʾ
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
	//int nNumVert = pLine->numVerts(); //��ȡ������
	//for(int i=0; i<nNumVert; i++)
		//pLine->setWidthsAt(i,dWidth,dWidth);

	pLine->setColorIndex(nColorIndex);
	pLine->setLayer(strCadLyName);
	
	double dArea =0.0;
	pLine->getArea(dArea);
	acutPrintf("\n��ǰ�ؿ����Ϊ��%0.3f ƽ����",dArea);


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
	pObj->close();

}



void gCmdCreatLabelAll(CString strCadLyName)
{
	CDistUiPrIntDef prIntType("\n �������ָ�",NULL,CDistUiPrInt::kRange,gStaticFontHeight); 
	prIntType.SetRange(2,72);
	if(prIntType.Go()!=CDistUiPrBase::kOk)
	{
		acutPrintf("\n ������ȡ��!"); return;
	}
	gStaticFontHeight = prIntType.GetValue();

	double dPrecision = 0.000001;
	ads_name ssName;
	struct resbuf * preb = acutBuildList(RTDXF0, "LWPOLYLINE", 0);
	if (acedSSGet(":S", NULL, NULL, preb, ssName) != RTNORM)
	{
		acedSSFree(ssName); acutRelRb(preb); return;
	}
	acutRelRb(preb);
	ads_name ssEnt;
	acedSSName(ssName, 0, ssEnt);

	AcDbObjectId dbObjId;
	if(Acad::eOk != acdbGetObjectId(dbObjId, ssEnt)) return;;

	acedSSFree(ssName);

	CString strLyNameBZ;
	strLyNameBZ.Format("%s�����ע",strCadLyName);

	AcDbPolyline * pPline = NULL;
	if (Acad::eOk != acdbOpenObject(pPline, dbObjId, AcDb::kForRead))
	{
		return;
	}
	pPline->close();

	int nCount = pPline->numVerts(); //ȥ���ظ���!!!!
	AcGePoint3d pt0;
    pPline->getPointAt(0, pt0);

	CCircleLabel tempLabel;
	tempLabel.FontHeight = gStaticFontHeight;
	for (int i = 0; i < nCount; i++)
	{
		AcGePoint3d pt3d, pt3dE;
		if (Acad::eOk != pPline->getPointAt(i, pt3d)) continue;

		if(i>0)
		{
			if((fabs(pt3d.x-pt0.x)<0.001)&&(fabs(pt3d.y-pt0.y)<0.001)) continue;
		}

		ads_point ptSet, ptSetE;

		ptSet[0] = pt3d.x;
		ptSet[1] = pt3d.y;
		ptSet[2] = pt3d.z;

		if(acedUsrBrk()) break;

		tempLabel.CreateSpcoordLabel(strLyNameBZ,ptSet, TRUE);

		if (Adesk::kTrue == pPline->hasBulges())
		{
			double Bulge = 0;

			if (Acad::eOk != pPline->getBulgeAt(i, Bulge)) continue;

			if( fabs(Bulge) > dPrecision )
			{
				AcGePoint3d ptE,ptS;
				if (Acad::eOk != pPline->getPointAt(i, ptS)) continue;
				if(i+1>=nCount)
				{
					if (Acad::eOk != pPline->getPointAt(0, ptE)) continue;
				}
				else
				{
					if (Acad::eOk != pPline->getPointAt(i+1, ptE)) continue;
				}

				AcGeCircArc2d tempArc;
				if(Acad::eOk == pPline->getArcSegAt(i,tempArc))
				{
					AcGePoint3d ptCen(tempArc.center().x,tempArc.center().y,0);
					double dR = tempArc.radius();
					ads_point ps,pe;
					ps[0] = tempArc.center().x; ps[1] = tempArc.center().y;
					pe[0] = (ptS.x+ptE.x)/2.0; pe[1] = (ptS.y+ptE.y)/2.0;
					double dAngle = acutAngle(ps,pe);
					tempLabel.CreatCircleLabel(strLyNameBZ,ptCen,dR,dAngle);

				}
			}
		}
	}
}

/*

// ���ܣ��������������ע(���α������е�)
// ������
// ���أ�
void gCmdCreatLabelAll(CString strCadLyName)
{
	CDistUiPrIntDef prIntType("\n �������ָ�",NULL,CDistUiPrInt::kRange,gStaticFontHeight); 
	prIntType.SetRange(2,72);
	if(prIntType.Go()!=CDistUiPrBase::kOk)
	{
		acutPrintf("\n ������ȡ��!"); return;
	}
	gStaticFontHeight = prIntType.GetValue();

	double dPrecision = 0.000001;
	ads_name ssName;
	struct resbuf * preb = acutBuildList(RTDXF0, "LWPOLYLINE", 0);
	if (acedSSGet(":S", NULL, NULL, preb, ssName) != RTNORM)
	{
		acedSSFree(ssName); acutRelRb(preb); return;
	}
	acutRelRb(preb);
	ads_name ssEnt;
	acedSSName(ssName, 0, ssEnt);

	AcDbObjectId dbObjId;
	if(Acad::eOk != acdbGetObjectId(dbObjId, ssEnt)) return;;

	acedSSFree(ssName);

	CString strLyNameBZ;
	strLyNameBZ.Format("%s�����ע",strCadLyName);

	AcDbPolyline * pPline = NULL;
	if (Acad::eOk != acdbOpenObject(pPline, dbObjId, AcDb::kForRead))
	{
		return;
	}
	pPline->close();

	int nCount = pPline->numVerts()-1; //ȥ���ظ���

	CCircleLabel tempLabel;
	tempLabel.FontHeight = gStaticFontHeight;
	for (int i = 0; i < nCount; i++)
	{
		AcGePoint3d pt3d, pt3dE;

		if (Acad::eOk != pPline->getPointAt(i, pt3d)) continue;

		ads_point ptSet, ptSetE;

		ptSet[0] = pt3d.x;
		ptSet[1] = pt3d.y;
		ptSet[2] = pt3d.z;

		if(acedUsrBrk()) break;

		tempLabel.CreateSpcoordLabel(strLyNameBZ,ptSet, TRUE);

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
				tempLabel.CreatCircleLabel(strLyNameBZ,ptSet, ptSetE, Bulge);
			}
		}
	}
}
*/

void gCmdSearchXMXX(CString strCadLyName,CString strSdeLyName)
{
	CDlgXmSearch dlg;
	dlg.SetInfo(strCadLyName,strSdeLyName);
	dlg.DoModal();
}


// ���ܣ��������������ע(ѡ����Ҫ��ע�ĵ�)
// ������
// ���أ�
void gCmdCreatLabel(CString strCadLyName)
{
	CDistUiPrIntDef prIntType("\n �������ָ�",NULL,CDistUiPrInt::kRange,gStaticFontHeight); 
	prIntType.SetRange(2,72);
	if(prIntType.Go()!=CDistUiPrBase::kOk)
	{
		acutPrintf("\n ������ȡ��!"); return;
	}
	gStaticFontHeight = prIntType.GetValue();

	CString strLyNameBZ;
	strLyNameBZ.Format("%s�����ע",strCadLyName);
	CSpcoordLabel temp;
	temp.FontHeight = gStaticFontHeight;
	while (temp.CreateSpcoordLabel(strLyNameBZ));
}

void gCmdDeleteLabel(CString strCadLyName)
{
	CString strLyNameBZ;
	strLyNameBZ.Format("%s�����ע",strCadLyName);

	//���ù�����
	char strLyName[260]={0};
	memset(strLyName,0,sizeof(char)*260);
	strcpy(strLyName,strLyNameBZ.GetBuffer(0));
	struct resbuf filter;
	filter.restype=AcDb::kDxfLayerName;
	filter.resval.rstring = strLyName;
	filter.rbnext=NULL;

	while(1)
	{
		ads_name ssName;
		if (acedSSGet(":S", NULL, NULL, &filter, ssName) == RTCAN) 
		{
			acedSSFree(ssName); return;
		}
		acutPrintf("ѡ��ʵ�屻ɾ�� [��ESC������]!");


		long nCount = 0;
		acedSSLength(ssName,&nCount);
		for(int i=0; i<nCount; i++)
		{
			ads_name ssTemp;
			AcDbObjectId tempObjId;
			if(acedSSName(ssName, i, ssTemp) != RTNORM) continue; 
			if(Acad::eOk != acdbGetObjectId(tempObjId, ssTemp)) continue;
			AcDbEntity* pEnt = NULL;
			if(Acad::eOk == acdbOpenObject(pEnt,tempObjId,AcDb::kForWrite))
			{
				pEnt->erase(); pEnt->close();
			}
		}
		acedSSFree(ssName);
	}
}

// ���ܣ�������Ŀ��������
// ������
// ���أ�
void gCmdInputLXProperty(CString strCadLyName,CString strSdeLyName,CString strURL,CString strXMBH,CString strPrePrjId)
{
	//if(NULL == g_pConnection) return;
	//CDlgXmInput dlg;
	//dlg.SetInfo(g_pConnection,strCadLyName,strSdeLyName,strURL,strXMBH,strPrePrjId);
	//dlg.DoModal();
}

void gTestInput(CString strCadLyNameDK,CString strCadLyNameXM,
				CString strSdeLyNameDK,CString strSdeLyNameXM,CString strURL,CString strProcessId)
{
	if(NULL == g_pConnection) return;
	CDlgLXInput dlg;
	dlg.SetInfo(g_pConnection,strCadLyNameDK,strCadLyNameXM,strSdeLyNameDK,strSdeLyNameXM,strURL,strProcessId);
	dlg.DoModal();
}


// ���ܣ�����ָ��ͼ�����ƶ�SDEͼ������
// ������
// ���أ�
void gCmdReadSdeLys(CStringArray& strLyArray,/*int* pAccessPower,*/CString strSdeLyName,CString strXMBH/*,CString strXZQH*/)
{
	//"������,ͬ����,�谲��,������,������,˼����,����"

	//1.�Ϸ��Լ��
	if(NULL == g_pConnection){
		acutPrintf("\n�ռ����ݿ�û�����ӣ�"); return;
	}
	int nCount = strLyArray.GetCount();
	if(0 == nCount ){
		acutPrintf("\nû��ѡ��ͼ��!"); return; 
	}

	CDistEntityTool tempTool;
	AcGePoint2d CenterPt;
	double dHeight;double dWidth;

	//2.����ѡ���������
	int nType=1;
	// ��ʱ
	//CDistUiPrIntDef prIntType("\nѡ������ (1)������Χ (2)��ǰ��Ŀ��� (3)��ǰ��Ŀͼ���� (4)��ʾ�ο���Χ  Ĭ��",NULL,CDistUiPrInt::kRange,1);  
	//prIntType.SetRange(1,4);
	CDistUiPrIntDef prIntType("\nѡ������ (1)������Χ (2)��ǰ��Ŀ���  Ĭ��",NULL,CDistUiPrInt::kRange,1);  
	prIntType.SetRange(1,2);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return;
	nType = prIntType.GetValue();

	CSdeDataOperate tempOperate;
	double dminX=4294967295,dminY=4294967295,dmaxX=-4294967295,dmaxY=-4294967295;

	
	if(1 == nType) //������Χ
	{
		//����ȡ�þ��η�Χ
		ads_point  startpt,endpt;    
		int nRet = acedGetPoint(NULL, "\n��ָ�����ε����Ͻ�:",startpt);
		if (nRet != RTNORM) return;
		nRet = acedGetCorner(startpt, "\n��ָ�����ε����½�:",endpt);
		if (nRet != RTNORM) return;

		//��ȡ��С�����㣨Ϊ��һ��������������׼����
		if(startpt[0]>endpt[0]){
			dmaxX = startpt[0]; dminX = endpt[0];
		}
		else{
			dmaxX = endpt[0]; dminX = startpt[0];
		}

		if(startpt[1]>endpt[1]){
			dminY = endpt[1]; dmaxY = startpt[1];
		}
		else{
			dmaxY = endpt[1]; dminY = startpt[1];
		}

		if((dmaxX -dminX < 1) || (dmaxY - dminY < 1))
		{
			acutPrintf("\n ѡȡ��Χ̫խ,ѡȡ��Ч!");
			return;
		}
	}
	else if(2 == nType)  //��Ŀ���
	{

		if(1!=gGetProjectRect(&dmaxX,&dmaxY,&dminX,&dminY,g_pConnection,strSdeLyName.GetBuffer(0),"PROJECT_ID",strXMBH.GetBuffer(0)))
		{
			acutPrintf("\n ��Ŀ��Ϣ������!"); return;
		}

		dminX = dminX - 200; dmaxX = dmaxX + 200;
		dminY = dminY - 200; dmaxY = dmaxY + 200;
	}
	// ��ʱ
	//else if(3 == nType)  //��Ŀͼ����
	//{
	//	CString strTFH = CBaseCfgInfo::m_basePrjInfo.strTFBH;
	//	strTFH.Trim();

	//	if(strTFH.IsEmpty())
	//	{
	//		acutPrintf("\nͼ����Ϊ��!"); return;
	//	}
	//	if(1!=gGetProjectRect(&dmaxX,&dmaxY,&dminX,&dminY,g_pConnection,"XMGHSDE.XMGH_JTB_1K","ͼ��",strTFH.GetBuffer(0)))
	//	{
	//		acutPrintf("\n ��Ŀͼ���Ų�����!"); return;
	//	}

	//	double dVx = dmaxX-dminX;
	//	double dVy = dmaxY-dminY;

	//	dminX = dminX - dVx; dmaxX = dmaxX + dVx;
	//	dminY = dminY - dVy; dmaxY = dmaxY + dVy;
	//}
	//else if(4 == nType)  //��ʾ�ο���Χ
	//{
	//	///��ʱ
	//	// XM
	//	//CenterPt.x = (501000+430000)/2;
	//	//CenterPt.y = (2766000+2695000)/2;
	//	//tempTool.DyZoom(CenterPt,2766000-2695000,501000-430000);
	//	// TZ
	//	//CenterPt.x = (68000+111900)/2;
	//	//CenterPt.y = (53000+83500)/2;
	//	//tempTool.DyZoom(CenterPt,83500-53000,111900-68000);

	//	CStringArray strArray;
	//	if(gReadCfgLyNameArray(strArray,"[�ο���Χͼ��]=")==FALSE)
	//	{
	//		acutPrintf("\n��ͼ�㷶Χ�����ļ�ʧ��!"); return;
	//	}
	//	AcDbObjectIdArray tempIdArray;
	//	CString strTempSdeName;
	//	for(int i=0; i<strArray.GetCount(); i++)
	//	{
	//		strTempSdeName = strArray.GetAt(i);
	//		tempOperate.ReadAllSdeLayer(tempIdArray,strTempSdeName.GetBuffer(0),NULL,NULL,1);
	//	}

	//	return;
	//}

	///��ʱ
	// XM
	//// ȷ���߽���Ч
	//if((dmaxX < 430000) || (dminX > 501000) || (dmaxY < 2695000) || (dminY > 2766000))
	//{
	//	acutPrintf("\n ѡȡ��Χ�����߽�,ѡȡ��Ч!"); return;
	//}

	//if(dmaxX > 501000)  dmaxX = 501000;
	//if(dmaxY > 2766000) dmaxY = 2766000;
	//if(dminX < 430000)  dminX = 430000;
	//if(dminY < 2695000) dminY = 2695000;
	// TZ
	//if((dmaxX < 68000) || (dminX > 111900) || (dmaxY < 53000) || (dminY > 83500))
	//{
	//	acutPrintf("\n ѡȡ��Χ�����߽�,ѡȡ��Ч!"); return;
	//}

	//if(dmaxX > 111900)  dmaxX = 111900;
	//if(dmaxY > 83500)	dmaxY = 83500;
	//if(dminX < 68000)	dminX = 68000;
	//if(dminY < 53000)	dminY = 53000;
	
	///��ʱ 
	////������������Χ���ǳ����û���
	//if(g_bSupperPower==FALSE)
	//{
	//	double dAx,dAy,dBx,dBy;
	//	strXZQH.Trim();
	//	if(strXZQH !="����")
	//	{
	//		if(gReadSdeByRect("XMGHSDE.XMGH_XZQH_PY",strXZQH,dBx,dBy,dAx,dAy)==FALSE) 
	//		{
	//			acutPrintf("\n ������������Χʧ��!"); return;
	//		}

	//		OutputDebugString("������������Χ OK");

	//		if((dmaxX < dAx) || (dmaxY < dAy) || (dminX>dBx) || (dminY>dBy)){
	//			acutPrintf("\n��ǰ�û�û�и��������Ȩ��!"); return;
	//		}
	//		if(dmaxX > dBx) dmaxX = dBx;
	//		if(dmaxY > dBy) dmaxY = dBy;
	//		if(dminX < dAx) dminX = dAx;
	//		if(dminY < dAy) dminY = dAy;
	//	}
	//}


	
	//��λ
	CenterPt.x = (dmaxX+dminX)/2; CenterPt.y = (dmaxY+dminY)/2;
	dHeight = dmaxY-dminY; dWidth =dmaxX-dminX;
	tempTool.DyZoom(CenterPt,dHeight,dWidth);


	double dTempArea = dHeight * dWidth;
	if(dTempArea>2500*2000+10)  //���̫��,ȥ������ͼ��
	{
		CStringArray strXZArray;
		if(gReadCfgLyNameArray(strXZArray,"[��Χ����ͼ��]=")==FALSE)
		{
			acutPrintf("\n��ͼ�㷶Χ�����ļ�ʧ��!"); return;
		}
		OutputDebugString("��ͼ�㷶Χ�����ļ� OK");

		CString strTempSdeLyName;
		for(int kk=nCount-1; kk>=0; kk--)
		{
			strTempSdeLyName = strLyArray.GetAt(kk);
			for(int ll = 0; ll <strXZArray.GetCount(); ll++)
			{
				if(strTempSdeLyName.CompareNoCase(strXZArray.GetAt(ll))==0)
				{
					strLyArray.RemoveAt(kk);
					acutPrintf("\n��Χ̫��,ͼ��[%s]û�е���!",strTempSdeLyName);
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	//������,һ�㾭��ǰ��ļ���Ӧ��û��������
	if(dminX > dmaxX)
	{
		double ddTemp = dminX; dminX = dmaxX; dmaxX = ddTemp;
	}
	else if(dmaxX - dminX < 1)
	{
		acutPrintf("\n��Ч��Χ!"); return;
	}

	if(dminY > dmaxY)
	{
		double ddTemp = dminY; dminY = dmaxY; dmaxY = ddTemp;
	}
	else if(dmaxY - dminY < 1)
	{
		acutPrintf("\n��Ч��Χ!"); return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////


	//����ռ���������
	DIST_STRUCT_SELECTWAY Filter,*pFilter=NULL;
	Filter.nEntityType = 4;
	Filter.nSelectType = kSM_AI;
	Filter.ptArray.append(AcGePoint3d(dminX,dminY,0));
	Filter.ptArray.append(AcGePoint3d(dmaxX,dmaxY,0));
	pFilter = &Filter;
	
	OutputDebugString("����ռ��������� OK");

	AcDbObjectIdArray ObjIdArray;
	bool bFound = false;
	CString strTempName;

	OutputDebugString("��ʼ��... ");

	nCount = strLyArray.GetCount();
	for(int i=0; i<nCount; i++)  
	{
		if(acedUsrBrk()) break;
		strTempName.Empty();
		CString strSdeLyName = strLyArray.GetAt(i);
		strSdeLyName = strSdeLyName.MakeUpper();
		OutputDebugString(strSdeLyName);

		tempOperate.ReadAllSdeLayer(ObjIdArray,strSdeLyName.GetBuffer(0),pFilter,NULL,1);

		////�����ѯ״̬
		//gSaveQueryLayerToSDE(g_pConnection, strXMBH.GetBuffer(0), 
		//						strSdeLyName.GetBuffer(60),
		//						&dminX, &dmaxX, &dminY, &dmaxY);
		//strXMBH.ReleaseBuffer();
		//strSdeLyName.ReleaseBuffer();
	}

	//����ͼ������ѡ״̬
	CString CheckState = DistGetTreeCheckState();
	gSaveTreeCheckStateToSDE(g_pConnection, strXMBH.GetBuffer(0), CheckState.GetBuffer(1024));
	strXMBH.ReleaseBuffer();
	CheckState.ReleaseBuffer();

	OutputDebugString("������ OK");
}

// ���ܣ�����ָ��ͼ�����ƺͷ�Χ��SDEͼ������
// ������
// ���أ�
void gCmdReadSdeLayerByRect(CString strSDELayerName,CString strMINX,CString strMAXX,CString strMINY,CString strMAXY,CString strXMBH)
{
	//1.�Ϸ��Լ��
	if(g_pConnection == NULL){
		acutPrintf("\n�ռ����ݿ�û�����ӣ�"); return;
	}
	if(strSDELayerName == ""){
		acutPrintf("\nû��ѡ��ͼ��!"); return; 
	}

	CDistEntityTool tempTool;
	AcGePoint2d CenterPt;
	double dHeight;double dWidth;

	CSdeDataOperate tempOperate;
	double dminX=4294967295,dminY=4294967295,dmaxX=-4294967295,dmaxY=-4294967295;

	//������Χ

	//��ȡ��С�����㣨Ϊ��һ��������������׼����	
	dminX = atof(strMINX.GetBuffer(0));
	strMINX.ReleaseBuffer();
	dmaxX = atof(strMAXX.GetBuffer(0));
	strMAXX.ReleaseBuffer();
	dminY = atof(strMINY.GetBuffer(0));
	strMINY.ReleaseBuffer();
	dmaxY = atof(strMAXY.GetBuffer(0));
	strMAXY.ReleaseBuffer();

	if((dmaxX -dminX < 1) || (dmaxY - dminY < 1))
	{
		acutPrintf("\n ѡȡ��Χ̫խ,ѡȡ��Ч!");
		return;
	}

	///��ʱ
	// XM
	//// ȷ���߽���Ч
	//if((dmaxX < 430000) || (dminX > 501000) || (dmaxY < 2695000) || (dminY > 2766000))
	//{
	//	acutPrintf("\n ѡȡ��Χ�����߽�,ѡȡ��Ч!"); return;
	//}

	//if(dmaxX > 501000)  dmaxX = 501000;
	//if(dmaxY > 2766000) dmaxY = 2766000;
	//if(dminX < 430000)  dminX = 430000;
	//if(dminY < 2695000) dminY = 2695000;
	// TZ
	//// ȷ���߽���Ч
	//if((dmaxX < 68000) || (dminX > 111900) || (dmaxY < 53000) || (dminY > 83500))
	//{
	//	acutPrintf("\n ѡȡ��Χ�����߽�,ѡȡ��Ч!"); return;
	//}

	//if(dmaxX > 111900)  dmaxX = 111900;
	//if(dmaxY > 83500)	dmaxY = 83500;
	//if(dminX < 68000)	dminX = 68000;
	//if(dminY < 53000)	dminY = 53000;
		
	//��λ
	CenterPt.x = (dmaxX+dminX)/2; CenterPt.y = (dmaxY+dminY)/2;
	dHeight = dmaxY-dminY; dWidth =dmaxX-dminX;
	tempTool.DyZoom(CenterPt,dHeight,dWidth);

	double dTempArea = dHeight * dWidth;
	if(dTempArea>2500*2000+10)  //���̫��,ȥ������ͼ��
	{
		CStringArray strXZArray;
		if(gReadCfgLyNameArray(strXZArray,"[��Χ����ͼ��]=")==FALSE)
		{
			acutPrintf("\n��ͼ�㷶Χ�����ļ�ʧ��!"); return;
		}
		OutputDebugString("��ͼ�㷶Χ�����ļ� OK");

		for(int ll = 0; ll <strXZArray.GetCount(); ll++)
		{
			if(strSDELayerName.CompareNoCase(strXZArray.GetAt(ll))==0)
			{
				acutPrintf("\n��Χ̫��,ͼ��[%s]û�е���!",strSDELayerName); return;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	//������,һ�㾭��ǰ��ļ���Ӧ��û��������
	if(dminX > dmaxX)
	{
		double ddTemp = dminX; dminX = dmaxX; dmaxX = ddTemp;
	}
	else if(dmaxX - dminX < 1)
	{
		acutPrintf("\n��Ч��Χ!"); return;
	}

	if(dminY > dmaxY)
	{
		double ddTemp = dminY; dminY = dmaxY; dmaxY = ddTemp;
	}
	else if(dmaxY - dminY < 1)
	{
		acutPrintf("\n��Ч��Χ!"); return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////


	//����ռ���������
	DIST_STRUCT_SELECTWAY Filter,*pFilter=NULL;
	Filter.nEntityType = 4;
	Filter.nSelectType = kSM_AI;
	Filter.ptArray.append(AcGePoint3d(dminX,dminY,0));
	Filter.ptArray.append(AcGePoint3d(dmaxX,dmaxY,0));
	pFilter = &Filter;
	
	OutputDebugString("����ռ��������� OK");

	AcDbObjectIdArray ObjIdArray;
	bool bFound = false;
	CString strTempName;

	OutputDebugString("��ʼ��... ");

	strSDELayerName = strSDELayerName.MakeUpper();
	OutputDebugString(strSDELayerName);

	tempOperate.ReadAllSdeLayer(ObjIdArray,strSDELayerName.GetBuffer(0),pFilter,NULL,1);

	OutputDebugString("������ OK");
}

void gCmdSaveCurXMObjectInfo(const char* strCadLyName,const char* strCurXMBH)
{
	//SaveSelectEntiyToSDE(IDistConnection *pConnect,const char* strLyName,const char* strCadName);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
			pOther->Release(); pCommand->Release(); 
			if(NULL != pRcdSet) pRcdSet->Release();
			for(j=0; j<nParamCount; j++)
			{
				delete[] strValues[j];
			}
			delete[] strValues;
			return FALSE;
		}
	}
	pOther->CommitTrans();
	pOther->Release(); pCommand->Release();
	if(NULL != pRcdSet) pRcdSet->Release();
	for(j=0; j<nParamCount; j++)
	{
		delete[] strValues[j];
	}
	delete[] strValues;

	return TRUE;
}


BOOL IsXmDkLayer(CString strSdeLyName)
{
	// ��ʱ
	//if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_ZB_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_ZB_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_ZB_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_ZB_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_ZB_POLYGON")==0) return TRUE;

	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_DA_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_DA_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_DA_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_DA_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_DA_POLYGON")==0) return TRUE;

	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_LS_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_LS_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_LS_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_LS_POLYGON")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_LS_POLYGON")==0) return TRUE;

	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_ZB_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_ZB_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_ZB_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_ZB_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_ZB_PT")==0) return TRUE;

	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_DA_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_DA_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_DA_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_DA_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_DA_PT")==0) return TRUE;

	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_XZLX_LS_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YDLX_LS_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_HXLX_LS_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_YXLX_LS_PT")==0) return TRUE;
	//else if(strSdeLyName.CompareNoCase("XMGHSDE.SP_CBLX_LS_PT")==0) return TRUE;
	if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_ZB_POLYGON")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_ZB_POLYGON")==0) return TRUE;

	else if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_DA_POLYGON")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_DA_POLYGON")==0) return TRUE;

	else if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_LS_POLYGON")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_LS_POLYGON")==0) return TRUE;

	else if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_ZB_PT")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_ZB_PT")==0) return TRUE;

	else if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_DA_PT")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_DA_PT")==0) return TRUE;

	else if(strSdeLyName.CompareNoCase("SDE.SP_XZLX_LS_PT")==0) return TRUE;
	else if(strSdeLyName.CompareNoCase("SDE.SP_YDLX_LS_PT")==0) return TRUE;
	else
		return FALSE;
}


DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
	DWORD dwCount=GetTickCount(); 
	while(1)
	{
		if(GetTickCount()-dwCount>=5000) 
		{
			break;
		}
		Sleep(1);
	}

	CDistCommandManagerReactor* pTemp = (CDistCommandManagerReactor*)lpParam;
	pTemp->m_bSave = FALSE;

	return 0;
}

void gCmdSaveBackDwg(CDistCommandManagerReactor* pCmdReactor)
{
	AcDbDatabase* pCurDb = acdbHostApplicationServices()->workingDatabase();

	// ��Ŀ�����ݿ�
	AcDbBlockTable* pTable = NULL;
	if (pCurDb->getBlockTable(pTable, AcDb::kForRead) != Acad::eOk) return;

	// ��Ŀ�����ݿ�ģ�Ϳռ�
	AcDbBlockTableRecord* pModel = NULL;
	if (pTable->getAt(ACDB_MODEL_SPACE, pModel, AcDb::kForRead) != Acad::eOk)
	{
		pTable->close(); return;
	}

	//ö������
	Acad::ErrorStatus es;
	AcDbBlockTableRecordIterator* pIter = NULL;
	if ((es=pModel->newIterator(pIter)) != Acad::eOk) 
	{
		pTable->close();pModel->close(); return;
	}

	AcDbEntity  *pEnt = NULL;
	acedSetStatusBarProgressMeter("���ڱ����ͼ�����Ժ󣮣���",0,200);  //��ʼ��������
	int nPos = 0,nTimes = 0;

	for (pIter->start(); !pIter->done(); pIter->step()) 
	{
		if (pIter->getEntity(pEnt, AcDb::kForWrite)== Acad::eOk)
		{
			CString strSdeLyName;
			CDistXData tempXData(pEnt,"ROWID_OBJS");
			if(tempXData.GetRecordCount()>0)
			{
				tempXData.Select("SDELYNAME",strSdeLyName);
				if(IsXmDkLayer(strSdeLyName)==FALSE)
				{
					pEnt->erase(); 
				}

				nTimes++;
				if(nTimes == 30)
				{
					nTimes = 0;
					nPos++;
					if(nPos == 200) nPos = 1;
					acedSetStatusBarProgressMeterPos(nPos);			
				}
			}
			pEnt->close();
		}
	}
	delete pIter;pIter = NULL;
	pTable->close();pModel->close(); 
	acedRestoreStatusBar();

	pCmdReactor->m_bSave = TRUE; //�������
	CDistEntityTool tempTool;
	tempTool.SendCommandToAutoCAD("QSAVE ");	
	
	DWORD dwThreadId; 
	CreateThread(NULL,0,ThreadFunc,pCmdReactor,0,&dwThreadId);                
}


void NewDocHelper(void* path)
{
	acDocManager->appContextNewDocument((const char*)path);

}

void CloseDocHelper(void* vp)
{
	AcApDocument* pDoc = (AcApDocument*)vp;
	if(pDoc)
	{
		acDocManager->closeDocument(pDoc);
	}
}

void gCmdOpenBackDwg()
{
	CStringArray strPathArray;
	ReadCfgLyNameArray(strPathArray,"[��ͼ·��]=");

	CString strPathDwt;
	if(strPathArray.GetCount()==0)
	{
		strPathDwt.Format("%sWork\\��ͼ.dwt",gGetCurArxAppPath("shYDsys.arx"));
	}
	else
	{
		strPathDwt = strPathArray.GetAt(0)+"��ͼ.dwt";
	}
	
	if(gIsFileExists(strPathDwt)==FALSE) 
	{
		acutPrintf("\n��ͼ������!");
		return; //��ͼ������,ֱ���˳�
	}

	if(acDocManager->isApplicationContext())
	{
		CloseDocHelper((void*)curDoc());
		NewDocHelper((void*)_T(strPathDwt.GetBuffer(0)));
	}
	else
	{
		acDocManager->executeInApplicationContext(CloseDocHelper, (void*)curDoc());
		acDocManager->executeInApplicationContext(NewDocHelper, (void*)_T(strPathDwt.GetBuffer(0)));
	}
	CDistEntityTool tempTool;
	tempTool.SendCommandToAutoCAD("_ZOOM E ");
}

void gShowOrHideCadLayer(CString strCadLyName,BOOL bIsShow)
{
	CStringArray strArray;
	strArray.Add(strCadLyName); 
	CString strTemp;
	strTemp.Format("%s���Ա�ע",strCadLyName); strArray.Add(strTemp);
	strTemp.Format("%s��Ŀ��ע",strCadLyName); strArray.Add(strTemp);

	AcApDocument* pDoc = acDocManager->curDocument();
	if(acDocManager->lockDocument(pDoc)==Acad::eOk)
	{
		AcDbLayerTable *pLyTable = NULL;
		AcDbLayerTableRecord *pLyRcd = NULL;
		Acad::ErrorStatus es;
		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		for(int i=0; i<3; i++)
		{
			strTemp = strArray.GetAt(i);
			es = pDB->getSymbolTable(pLyTable,AcDb::kForRead);
			if(Acad::eOk == es) 
			{
				es= pLyTable->getAt(strTemp, pLyRcd, AcDb::kForWrite);
				pLyTable->close();
				if(Acad::eOk == es)  
				{
					if(bIsShow)
						pLyRcd->setIsOff(Adesk::kFalse);
					else
						pLyRcd->setIsOff(Adesk::kTrue);
					pLyRcd->close();
					
				}
			}
		}
		acedUpdateDisplay();
		es = acDocManager->unlockDocument(pDoc);
	}
}


BOOL gReadCfgLyNameArray(CStringArray& strLyNameArray,CString strTag)
{
	//���ļ�
	CFile file;
	CString strFName;
	strFName.Format("%sLayerInfo.ini",gGetCurArxAppPath("shYDsys.arx"));
	if(file.Open(strFName,CFile::modeRead)==FALSE) return FALSE; //���ļ�ʧ��,ֱ���˳�
	int nLen = file.GetLength();
	if(nLen == 0)
	{
		file.Close(); return FALSE;
	}
	char* strBuf = new char[nLen];
	memset(strBuf,0,sizeof(char)*nLen);
	file.Read(strBuf,nLen);
	file.Close();
	CString strValue = strBuf;
	delete [] strBuf;

	//��ȡ��Ϣ
	int nPos = 0;	
	char chTag[3]={0x0D,0x0A,0};
	int nTagLen = strTag.GetLength();
	while((nPos=strValue.Find(strTag))>=0)
	{
		int nPosDA = strValue.Find(chTag,nPos); //Ѱ�һس�����λ��
		if(nPosDA > 0)
		{
			CString strTemp= strValue.Mid(nPos+nTagLen,nPosDA-nPos-nTagLen); 
			strTemp.TrimLeft(); strTemp.TrimRight();
			strLyNameArray.Add(strTemp);
			if(strValue.GetLength()-nPosDA-1 <= 0) break;
			strValue = strValue.Right(strValue.GetLength()-nPosDA-1);
		}

	}

	return TRUE;
}



BOOL gReadSdeByRect(CString strSdeLyName,CString strXZQH,double& d_maxX,double& d_maxY,double& d_minX,double& d_minY)
{
	CStringArray strArray;
	if(strXZQH.Find("����")>=0)
	{
		strArray.Add("������");
		strArray.Add("˼����");
	}
	else
		strArray.Add(strXZQH);

	BOOL bFound = FALSE;
	d_maxX=-999999999;d_maxY=-999999999;d_minX=999999999;d_minY=999999999;
	for(int kk=0; kk<strArray.GetCount();kk++)
	{
		CString strValue = strArray.GetAt(kk);
		strValue.Trim();

		int nPtCount =0;
		DIST_POINT_STRUCT* ptArray = NULL;

		if(gGetDistrictRect(&ptArray,&nPtCount,g_pConnection,strSdeLyName.GetBuffer(0),"NAME",strValue.GetBuffer(0))!=1) 
		{
			 continue;
		}
		if(nPtCount == 0) continue;

		bFound = TRUE;
		for (int i=0;i<nPtCount;i++) 
		{
			if(ptArray[i].x>d_maxX) d_maxX = ptArray[i].x;
			if(ptArray[i].y>d_maxY) d_maxY = ptArray[i].y;
			if(ptArray[i].x<d_minX) d_minX = ptArray[i].x;
			if(ptArray[i].y<d_minY) d_minY = ptArray[i].y; 
		}
	}

	return bFound;
}


BOOL gTurnArc(AcDbObjectId& OutObjId,AcDbObjectId InObjId,char* strXDataRegName,int nMaxNum,double dPrecision)
{
	OutObjId.setNull();
	//��ȡʵ�������
	AcDbPolyline* pline = NULL;
	if(Acad::eOk != acdbOpenObject(pline,InObjId,AcDb::kForWrite)) return FALSE;

	AcGePoint3dArray ptArray;
	AcGeDoubleArray blgArray;
	CDistEntityTool tempTool;
	tempTool.GetPolylinePts(pline,ptArray,blgArray);  //��ȡ������
	int nCount = ptArray.length();
	if(nCount <5)
	{
		pline->close(); return FALSE;
	}

	char strLyName[260]={0};
	strcpy(strLyName,pline->layer());    //��ȡͼ��
	int nClrIndex = pline->colorIndex(); //��ȡ��ɫ��
	bool bIsClosed = pline->isClosed();  //�Ƿ�պ�
	double dGWidth =0.0;
	pline->getWidthsAt(0,dGWidth,dGWidth); //ȫ���߿�

	CStringArray strNameRArray,strValueRArray;
	CDistXData tempXDataB(pline,"ROWID_OBJS");
	tempXDataB.Select(strNameRArray,strValueRArray);
	tempXDataB.Close();

	CDistXData tempXDataA(pline,strXDataRegName);
	CStringArray strNameArray,strValueArray;
	tempXDataA.Select(strNameArray,strValueArray);
	tempXDataA.Close();

	pline->erase(); pline->close();//ɾ�����ر�

	AcGePoint3d pt1,pt2,pt3;
	double dr,cenx,ceny;
	double dr0,cenx0,ceny0;
	double drTemp,cenxTemp,cenyTemp;
	double dTotalCenx=0,dToatlCeny=0,dTotalR=0;

	int nNum = 0;
	int nStart=0,nEnd =0;
	double dTempBlg = 0.00;
	//���˱���
	for(int i=nCount-1; i>=2; i--)
	{
		pt1 = ptArray.at(i);  pt2 = ptArray.at(i-1);  pt3 = ptArray.at(i-2); //ȡ��

		//�����Բ�뾶��Բ������
		if(gGetCenterAndRadiusWith3Pt(drTemp,cenxTemp,cenyTemp,pt1.x,pt1.y,pt2.x,pt2.y,pt3.x,pt3.y)==false)
		{
			if(nNum > nMaxNum)
			{
				//��ƽ��
				dr = dTotalR / nNum; cenx = dTotalCenx / nNum; ceny = dToatlCeny / nNum;

				AcGePoint3d ptS,ptE,ptCen;
				ptS = ptArray.at(nStart); ptE = ptArray.at(nEnd); ptCen=AcGePoint3d(cenx,ceny,0.0);
				dr = int(dr+0.1);
				dTempBlg = gCalcBlg(ptCen.x,ptCen.y,ptS.x,ptS.y,ptE.x,ptE.y,dr);

				for(int kk = nStart-1; kk>nEnd; kk--)
				{
					ptArray.removeAt(kk);
					blgArray.removeAt(kk);
				}
				blgArray.setAt(nEnd,(-1)*dTempBlg);	
			}

			//ȫ������
			nNum=0; nStart =0; nEnd = 0; dToatlCeny=0; dTotalCenx=0; dTotalR=0;

			continue;
		}

		if(nNum ==0)
		{
			nStart = i;
			dr0 = drTemp; cenx0 = cenxTemp; ceny0 = cenyTemp;
			dTotalCenx += cenxTemp; dToatlCeny += cenyTemp; dTotalR += drTemp;

			nNum++;
			continue;
		}

		if((fabs(dr0-drTemp)<dPrecision) && (fabs(cenx0-cenxTemp) < 2.00) &&(fabs(ceny0-cenyTemp)<2.00))
		{
			dTotalCenx += cenxTemp; dToatlCeny += cenyTemp; dTotalR += drTemp;
			nEnd = i-2;
			nNum++;
		}
		else
		{
			if(nNum > nMaxNum)
			{
				//��ƽ��
				dr = dTotalR / nNum; cenx = dTotalCenx / nNum; ceny = dToatlCeny / nNum;

				AcGePoint3d ptS,ptE,ptCen;
				ptS = ptArray.at(nStart); ptE = ptArray.at(nEnd); ptCen=AcGePoint3d(cenx,ceny,0.0);

				//����0.99��һ,����0.01��ȥ
				INT64 nTempValue = INT64 (dr*100);
				if(nTempValue % 100 == 99)
				{
					double ddd = nTempValue+1;	
					dr = ddd/100.00;
				}
				else if(nTempValue % 100 == 1)
				{
					double ddd = nTempValue-1;
					dr = ddd/100.00;
				}

				dTempBlg = gCalcBlg(ptCen.x,ptCen.y,ptS.x,ptS.y,ptE.x,ptE.y,dr);

				for(int kk = nStart-1; kk>nEnd; kk--)
				{
					ptArray.removeAt(kk);
					blgArray.removeAt(kk);
				}
				blgArray.setAt(nEnd,(-1)*dTempBlg);	

			}
			//ȫ������
			nNum=0; nStart =0; nEnd = 0; dToatlCeny=0; dTotalCenx=0; dTotalR=0;

			//����ǰֵ��Ϊ��һ�������Ŀ�ʼֵ
			nStart = i;
			dr0 = drTemp; cenx0 = cenxTemp; ceny0 = cenyTemp;
			dTotalCenx += cenxTemp; dToatlCeny += cenyTemp; dTotalR += drTemp;
			nNum++;
		}
	}


	if(nNum > nMaxNum)
	{
		//��ƽ��
		dr = dTotalR / nNum; cenx = dTotalCenx / nNum; ceny = dToatlCeny / nNum;

		AcGePoint3d ptS,ptE,ptCen;
		ptS = ptArray.at(nStart); ptE = ptArray.at(nEnd); ptCen=AcGePoint3d(cenx,ceny,0.0);

		dr = int(dr+0.1);
		dTempBlg = gCalcBlg(ptCen.x,ptCen.y,ptS.x,ptS.y,ptE.x,ptE.y,dr);

		for(int kk = nStart-1; kk>nEnd; kk--)
		{
			ptArray.removeAt(kk);
			blgArray.removeAt(kk);
		}
		blgArray.setAt(nEnd,(-1)*dTempBlg);	
	}

	if(bIsClosed)
	{
		int nTotalNum = ptArray.length();
		if((fabs(ptArray[0].x-ptArray[nTotalNum-1].x)<0.01) && (fabs(ptArray[0].y-ptArray[nTotalNum-1].y)<0.01))
		{
			ptArray.removeAt(nTotalNum-1);
			blgArray.removeAt(nTotalNum-1);
		}
	}
	tempTool.CreatePolyline_(OutObjId,ptArray,blgArray,strLyName,nClrIndex,bIsClosed,dGWidth);

	if(strNameArray.GetCount()>0)
	{
		AcDbPolyline* pline=NULL;
		if(Acad::eOk == acdbOpenObject(pline,OutObjId,AcDb::kForWrite))
		{
			CDistXData tempXDataC(pline,strXDataRegName);
			tempXDataC.Add(strNameArray,strValueArray);
			tempXDataC.Update();
			pline->close();
		}
	}

	if(strNameRArray.GetCount()>0)
	{
		AcDbPolyline* pline=NULL;
		if(Acad::eOk == acdbOpenObject(pline,OutObjId,AcDb::kForWrite))
		{
			CDistXData tempXDataD(pline,"ROWID_OBJS");
			tempXDataD.Add(strNameRArray,strValueRArray);
			tempXDataD.Update();
			pline->close();
		}
	}

	return TRUE;
}


double gCalcBlg(double centX,double centY,double fromX,double fromY,double toX,double toY,double dR)
{
	if(fabs(dR) < PRECISION) return 0.00;
	double dChord = sqrt((fromX-toX)*(fromX-toX)+(fromY-toY)*(fromY-toY))/2.0000; //�ҳ���һ��
	double dHeight = sqrt(dR*dR - dChord*dChord);  //�Ҿ���Բ�ľ���
	double dBlug = (dR-dHeight)/dChord;

	if((fromX-centX)*(toY-centY)-(toX-centX)*(fromY-centY)< PRECISION)
		dBlug=(-1)*dBlug;
	return dBlug;
}

bool gGetCenterAndRadiusWith3Pt(double& r,double& a,double& b,double x0,double y0,double x1,double y1,double x2,double y2)
{
	double s1 = (x0*x0-x1*x1) + (y0*y0-y1*y1);
	double s2 = (x1*x1-x2*x2) + (y1*y1-y2*y2);

	if(fabs((x0-x1)*(y1-y2) - (x1-x2)*(y0-y1)) <PRECISION) return false;
	if(fabs((y0-y1)*(x1-x2) - (y1-y2)*(x0-x1)) <PRECISION) return false;

	a = (s1*(y1-y2)-s2*(y0-y1))/(2*((x0-x1)*(y1-y2)-(x1-x2)*(y0-y1)));
	b = (s1*(x1-x2)-s2*(x0-x1))/(2*((y0-y1)*(x1-x2)-(y1-y2)*(x0-x1)));
	r = sqrt((x0-a)*(x0-a)+(y0-b)*(y0-b));

	if(r<PRECISION) return false;

	return true;	
}

BOOL  ReadCfgLyNameArray(CStringArray& strLyNameArray,CString strTag)
{
	//���ļ�
	CFile file;
	CString strFName;
	strFName.Format("%sPathInfo.ini",gGetCurArxAppPath("shYDsys.arx"));
	if(file.Open(strFName,CFile::modeRead)==FALSE) return FALSE; //���ļ�ʧ��,ֱ���˳�
	int nLen = file.GetLength();
	if(nLen == 0)
	{
		file.Close(); return FALSE;
	}
	char* strBuf = new char[nLen];
	memset(strBuf,0,sizeof(char)*nLen);
	file.Read(strBuf,nLen);
	file.Close();
	CString strValue = strBuf;
	delete [] strBuf;

	//��ȡ��Ϣ
	int nPos = 0;	
	char chTag[3]={0x0D,0x0A,0};
	int nTagLen = strTag.GetLength();
	while((nPos=strValue.Find(strTag))>=0)
	{
		int nPosDA = strValue.Find(chTag,nPos); //Ѱ�һس�����λ��
		if(nPosDA > 0)
		{
			CString strTemp= strValue.Mid(nPos+nTagLen,nPosDA-nPos-nTagLen); 
			strTemp.TrimLeft(); strTemp.TrimRight();
			strLyNameArray.Add(strTemp);
			if(strValue.GetLength()-nPosDA-1 <=0) break;
			strValue = strValue.Right(strValue.GetLength()-nPosDA-1);
		}
	}

	return TRUE;
}


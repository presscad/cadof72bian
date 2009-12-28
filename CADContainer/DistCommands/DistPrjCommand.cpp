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

//static CDistPersistentObjReactor* m_pPrReactor = CDistPersistentObjReactor::GetInstance();  //ʵ��ר�÷�Ӧ������

extern IDistConnection* g_pConnection;         //SDE���ݿ����Ӷ���ָ��

char gXmType[5][10]={"ѡַ","�õ�","����","Ԥѡ","����"};

void gCreateBlockAttr()
{
	CStringArray strArray;
	// ��Ŀ�������ƶ���
	strArray.RemoveAll();
	strArray.Add("���̺�");strArray.Add("֤��");strArray.Add("����ʱ��");strArray.Add("��Ŀ����");
	strArray.Add("���赥λ");strArray.Add("�õ�����");strArray.Add("��������");strArray.Add("�����");
	strArray.Add("�����õ����");strArray.Add("��·�õ����");strArray.Add("�̻��õ����");strArray.Add("�����õ����"); 

	CDistEntityTool tempTool;
	tempTool.InsertBlkAttr("_��Ŀ����",strArray,"��Ŀ����",5);

	// �������Զ��壨��չ�ֶ����Ʊ�����
	strArray.RemoveAll();	   
	strArray.Add("���̺�");strArray.Add("֤��");strArray.Add("����");strArray.Add("��Ŀ����"); 
	strArray.Add("���赥λ");strArray.Add("�õ�����");strArray.Add("��������");strArray.Add("�õ����"); 
	strArray.Add("�õ�����");strArray.Add("�ؿ���");strArray.Add("�ָ�");strArray.Add("�߿�");	   
	strArray.Add("X ֵ");strArray.Add("Y ֵ");strArray.Add("������");strArray.Add("��ע");	  

	tempTool.InsertBlkAttr("_��������",strArray,"�ؿ���",2);
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



// ���ܣ�������Ŀ���߽߱�
// ������strLyName     ͼ������
//       nColorIndex   ��ɫ��
//       dWidth        ȫ���߿�
// ���أ���
void gCmdCreateLXBound(CString strCadLyName,int nColorIndex,double dWidth)
{

	int nType=1;
	//����ѡ���������
	CDistUiPrIntDef prIntType("\nѡ������ (1)��ȡ�߽� (2)ѡ��߽� (3)�½��߽�   Ĭ��",NULL,CDistUiPrInt::kRange,1); 
	prIntType.SetRange(1,3);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return;
	nType = prIntType.GetValue();

	//�õ�����ʵ�壨����;������ȡ��ѡ�񣬻��ƣ�
	AcDbObjectId ObjId;
	AcDbEntity * pEnt=NULL; 
	AcDbPolyline* pLine=NULL; 
	CDistEntityTool tempTool;
	tempTool.CreateLayer(ObjId,strCadLyName,nColorIndex);
	//a.��ȡ�߽�
	if(1 == nType)  
	{
		ads_name en;
		if(acdbEntLast(en)!=RTNORM){  //��ȡ���ݿ������һ��ʵ��
			acutPrintf("\nû��ʵ�屻ѡ��"); return;
		}

		ads_point adsPt;
		int nResult = acedGetPoint(NULL,"\nѡ�������ڲ���:", adsPt);
		if(nResult != RTNORM) return;
		char strPt[40]={0};
		sprintf(strPt,"%0.3f,%0.3f",adsPt[0],adsPt[1]);
		if(acedCommand(RTSTR,"-BOUNDARY",RTSTR,strPt,RTSTR,"",RTNONE)!=RTNORM) return; //�����������ɱ߽�(RTSTR,PAUSE)
		while(acdbEntNext(en,en)==RTNORM)//��ȡ���ɵı߽�
		{
			ObjId.setNull();
			if(Acad::eOk != acdbGetObjectId(ObjId, en)) 
			{
				ObjId.setNull();
				return;  //�õ�ʵ��ObjectId
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

	//b.ѡ��߽�
	else if(2 == nType) 
	{
		/*
		ads_point pt;  
		ads_name entName;
		

		if(acedEntSel("\n��ѡ��һ���պϵ�ʵ�壺 ", entName, pt)!=RTNORM) return;
		if(acdbGetObjectId(ObjId, entName)!=Acad::eOk) return;
		if(acdbOpenObject(pEnt, ObjId, AcDb::kForRead)!=Acad::eOk) return;
		if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			pLine = (AcDbPolyline*) pEnt;
			if(!pLine->isClosed())
			{
				acutPrintf("\nѡ��ʵ�岻�պ�");
				pEnt->close(); return; 
			}
		}
		else
		{
			acutPrintf("\nѡ��ʵ�����Ͳ���Polyline��");
			pEnt->close(); return;
		}
		pEnt->close();
		*/

			
		ObjId.setNull();

		// ��ȡ�߽�
		CLxPickUp temp;
		temp.pch = strCadLyName.GetBuffer(0);
		temp.mainFun();
		ObjId = temp.m_OutId;
		temp.RemoveEntity();	
		if(ObjId.isNull()) return;
	}

	//c.�½��߽�
	else if(3 == nType)  
	{
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
	}

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
	int nNumVert = pLine->numVerts(); //��ȡ������
	for(int i=0; i<nNumVert; i++)
		pLine->setWidthsAt(i,dWidth,dWidth);

	pLine->setColorIndex(nColorIndex);
	pLine->setLayer(strCadLyName);
	
	double dArea =0.0;
	pLine->getArea(dArea);
	acutPrintf("\n��ǰ�ؿ����Ϊ��%0.2f ƽ����",dArea);

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


// ���ܣ��������������ע(���α������е�)
// ������
// ���أ�
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


// ���ܣ��������������ע(ѡ����Ҫ��ע�ĵ�)
// ������
// ���أ�
void gCmdCreatLabel(CString strCadLyName)
{
	CSpcoordLabel temp;
	while (temp.CreateSpcoordLabel());
}

// ���ܣ�������Ŀ��������
// ������
// ���أ�
void gCmdInputLXProperty(CString strCadLyName,CString strSdeLyName,HWND hDapHwnd,CString strURL,CString strXMBH)
{
	if(NULL == g_pConnection) return;
	CDlgXmInput dlg;
	dlg.SetInfo(g_pConnection,strCadLyName,strSdeLyName,hDapHwnd,strURL,strXMBH);
	dlg.DoModal();
}


// ���ܣ�����ָ��ͼ�����ƺͷ���Ȩ�޶�SDEͼ������
// ������
// ���أ�
void gCmdReadSdeLys(CStringArray& strLyArray,int* pAccessPower,CString strCadLXName,CString strXMBH)
{
	if(NULL == g_pConnection)
	{
		acutPrintf("\n�ռ����ݿ�û�����ӣ�"); return;
	}
	int nCount = strLyArray.GetCount();
	if(0 == nCount ) return; //|| NULL == pAccessPower

	//����ѡ���������
	int nType=1;
	CDistUiPrIntDef prIntType("\nѡ������ (1)������Χ (2)��Ŀ��� (3)ȫͼ  Ĭ��",NULL,CDistUiPrInt::kRange,1);  
	prIntType.SetRange(1,3);
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
	else if(2 == nType)  //��Ŀ���
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
	else if(3 == nType)  //ȫͼ
	{
	}

	//����ռ���������
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



//������Ϣ���
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
		acutPrintf("\n�ռ����ݿ�û�����ӣ�"); return;
	}

	bool bRect=true;
	if(dmaxx<0 && dmaxy<0 && dminx<0 && dminy<0)
		bRect = false;
	CStringArray strNameXMArray,strNameDKArray;
	strNameXMArray.RemoveAll();
	strNameXMArray.Add("���̺�");strNameXMArray.Add("֤��");strNameXMArray.Add("����ʱ��");strNameXMArray.Add("��Ŀ����");
	strNameXMArray.Add("���赥λ");strNameXMArray.Add("�õ�����");strNameXMArray.Add("��������");strNameXMArray.Add("�����");
	strNameXMArray.Add("�����õ����");strNameXMArray.Add("��·�õ����");strNameXMArray.Add("�̻��õ����");strNameXMArray.Add("�����õ����"); 

	// �������Զ��壨��չ�ֶ����Ʊ�����
	strNameDKArray.RemoveAll();	   
	strNameDKArray.Add("���̺�");strNameDKArray.Add("֤��");strNameDKArray.Add("����");strNameDKArray.Add("��Ŀ����"); 
	strNameDKArray.Add("���赥λ");strNameDKArray.Add("�õ�����");strNameDKArray.Add("��������");strNameDKArray.Add("�õ����"); 
	strNameDKArray.Add("�õ�����");strNameDKArray.Add("�ؿ���");strNameDKArray.Add("�ָ�");strNameDKArray.Add("�߿�");	   
	strNameDKArray.Add("X ֵ");strNameDKArray.Add("Y ֵ");strNameDKArray.Add("������");strNameDKArray.Add("��ע");	


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
	strTempCadLyName.Format("%s���Ա�ע",strCadName);
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
		
		tempTool.InsertBlkAttrRef("_��������",strTempCadLyName.GetBuffer(),AcGePoint3d(dx,dy,0),dZG,strNameDKArray,strTempValueArray);
	}
	pRcdset->Release();

	strSQL = "F:LCH,ZH,RQ,XMMC,JSDW,YDXZ,LXLX,ZMJ,JSYDMJ,DLYDMJ,LHYDMJ,QTYDMJ,ZG,X,Y,T:TB_PROJECT,##";
	if(pCommand->SelectData(strSQL.GetBuffer(),&pRcdset) != 1)
	{
		pCommand->Release(); return;
	}

	strTempCadLyName.Format("%s��Ŀ��ע",strCadName);
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
		tempTool.InsertBlkAttrRef("_��Ŀ����",strTempCadLyName.GetBuffer(),AcGePoint3d(dx,dy,0),dZG,strNameXMArray,strTempValueArray);
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






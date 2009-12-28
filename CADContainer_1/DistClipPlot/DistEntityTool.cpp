#include "StdAfx.h"
#include "DistEntityTool.h"


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


CDistEntityTool::CDistEntityTool(void)
{
}

CDistEntityTool::~CDistEntityTool(void)
{
	AfxGetApp();
}



//���ܣ�����ͼ��
//������resultId    ���ش���ʵ���ID
//      strLyName   ͼ����
//      nClrIndex   ��ɫ��
//      strLnType   ����
//      dLineWeigth �߿�
//      pDB         ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
long CDistEntityTool::CreateLayer(AcDbObjectId& resultId,const char *strLyName, int  nClrIndex,const char *strLnType,
				                  AcDb::LineWeight dLineWeigth, AcDbDatabase* pDB)
										  
{
	AcDbLayerTable			*pLyTable=NULL;
	AcDbLayerTableRecord	*pLyRcd=NULL;
	AcDbObjectId			lnTypeId;
	AcCmColor				TempColor;
	Acad::ErrorStatus		es;

	resultId.setNull();
	lnTypeId.setNull();

	if ( 0 == GetLineTypeId(lnTypeId,strLnType))
	{
		LoadLineType(lnTypeId,strLnType);
	}

	pDB->getSymbolTable(pLyTable, AcDb::kForRead);
	if (!pLyTable->has(strLyName))
	{
		es = pLyTable->upgradeOpen();
		pLyRcd=new AcDbLayerTableRecord;
		if(NULL == pLyRcd) return 0;
		es = pLyRcd->setName(strLyName);
		es = pLyRcd->setIsFrozen(0);
		if(es != Acad::eOk) 
		{	
			pLyTable->close(); delete pLyRcd; return 0;
		}
		pLyRcd->setIsLocked(0);
		pLyRcd->setIsOff(0);
		pLyRcd->setVPDFLT(0);

		TempColor.setColorIndex(nClrIndex);
		pLyRcd->setColor(TempColor);
		es = pLyRcd->setLineWeight(dLineWeigth);

		
		if(!lnTypeId.isNull())
			pLyRcd->setLinetypeObjectId(lnTypeId);
		es = pLyTable->add(resultId, pLyRcd);
		es = pLyRcd->close();
		es = pLyTable->close();
	}
	else
	{
		es = pLyTable->getAt(strLyName, resultId);
		es = pLyTable->getAt(strLyName, pLyRcd, AcDb::kForWrite);
		pLyTable->close();
		if(es != Acad::eOk) return 0;
		es = TempColor.setColorIndex(nClrIndex);
		pLyRcd->setColor(TempColor);
		es = pLyRcd->setLineWeight(dLineWeigth);
		if(!lnTypeId.isNull())
			pLyRcd->setLinetypeObjectId(lnTypeId);
		es = pLyRcd->close();
	}

	//ϵͳ����
	/*struct resbuf *Value = acutBuildList(RTSTR,strLyName,0);

	if(acedSetVar("clayer", Value) != RTNORM)
	{
		acutRelRb(Value);
		return FALSE;
	}

	acutRelRb(Value);*/

	return 1;

}


long CDistEntityTool::SetCurLayer(const char* strLayerName, AcDbDatabase* pDB)
{
	AcDbLayerTableRecordPointer spRecord(strLayerName, pDB, AcDb::kForRead);
	Acad::ErrorStatus es = spRecord.openStatus();
	if (es == Acad::eOk)
	{
		es =  pDB->setClayer(spRecord->objectId());
	}

	if (es == Acad::eOk) return 1;

	return 0;
}


long CDistEntityTool::SelectEntityInLayer(AcDbObjectIdArray& IdArray,const char *strLyName)
{
	IdArray.setLogicalLength(0);

	int nlen = strlen(strLyName)+1;
	ads_name ents;
	struct resbuf *rb;
	rb=acutNewRb(AcDb::kDxfLayerName);
	rb->restype=AcDb::kDxfLayerName;
	rb->resval.rstring = (char*) malloc(nlen*sizeof(char));
	strcpy(rb->resval.rstring,strLyName);
	rb->rbnext=NULL;
	acedSSGet("X",NULL,NULL,rb,ents);
	long entNums=0;
	acedSSLength(ents,&entNums);
	if (entNums == 0)
		return 0;
	else
	{
		for (long a = 0; a < entNums ; a ++)
		{
			AcDbObjectId  objId;
			ads_name      ent;
			acedSSName(ents,a,ent);
			acdbGetObjectId(objId, ent);
			IdArray.append(objId);
		}
	}
	acedSSFree(ents);
	//free(rb->resval.rstring);
	acutRelRb(rb);

	return 1;
}


//ɾ��ָ��ͼ���е�����ʵ��
long CDistEntityTool::DeleteAllEntityInLayer(char *strLyName,AcDbDatabase* pDB)      
{  
	ASSERT(lpLayerName!=NULL);
	if(strLyName==NULL)
		return Acad::eInvalidInput;

	AcDbObjectIdArray IdArray;
	if(SelectEntityInLayer(IdArray,strLyName)<=0) //ȡ������ʵ��
		return Acad::eInvalidInput;

	int nLenNum=IdArray.logicalLength();
	if(nLenNum<=0)
		return 0;

	AcDbObjectId eId;
	AcDbEntity *pE;

	for(int i=0;i<nLenNum;i++)
	{
		eId=IdArray.at(i);
		if(Acad::eOk == acdbOpenObject(pE, eId, AcDb::kForWrite)) 
		{
            pE->erase(); pE->close();  
		}
	}

	IdArray.setLogicalLength(0);

	return 1;
}


//���ܣ����������ı�
//������resultId       ���ش���ʵ���ID
//      ptAlign        ���ֶ��������
//      strText        �ı�����
//      strLyName      ͼ����
//      nClrIndex      ��ɫ��
//      strFontType    ����
//      nHorzAlignType ���ֺ�����뷽ʽ(0:�����,1:���ж���,2:�Ҷ���)
//      nVertAlignType ����������뷽ʽ(0:�¶���,1:���ж���,2:�϶���)
//      dHeight        ���ָ߶�
//      dWidFactor     ���ֿ�߱�
//      dAngle         ��ת�Ƕ�
//      pDB            ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
long CDistEntityTool::CreateText(AcDbObjectId& resultId,AcGePoint3d& ptAlign,char* strText,char* strLyName,int  nClrIndex,char* strFontType,
				                 int nHorzAlignType,int nVertAlignType,double dHeight,double dWidFactor,double dAngle, AcDbDatabase* pDB)
{
	resultId.setNull();

	//��ȡָ���������Ƶ�ID
	AcDbObjectId IdTextStyle; IdTextStyle.setNull();
	AcDbTextStyleTableRecordPointer spRecord(strFontType, pDB, AcDb::kForRead); 
	if(Acad::eOk ==spRecord.openStatus())
		IdTextStyle =spRecord->objectId();  

	//�����ı�
	AcDbText *pText = new AcDbText(ptAlign,strText,IdTextStyle,dHeight,dAngle);
	if(NULL == pText) return 0;

	//���õ��ı�����
	pText->setColorIndex(nClrIndex); //������ɫ��
	pText->setWidthFactor(dWidFactor);  //���ÿ�߱�

	if(nVertAlignType == 1) //�����������
		pText->setVerticalMode(AcDb::kTextVertMid);
	else if(nVertAlignType == 2)
		pText->setVerticalMode(AcDb::kTextTop);
	else
		pText->setVerticalMode(AcDb::kTextBottom);

	if(nHorzAlignType == 1)//���ú������
		pText->setHorizontalMode(AcDb::kTextCenter);
	else if(nHorzAlignType == 2)
		pText->setHorizontalMode(AcDb::kTextRight);
	else
		pText->setHorizontalMode(AcDb::kTextLeft);
	pText->setAlignmentPoint(ptAlign);

	pText->setLayer(strLyName);  //����ͼ����

	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pText,pDB);
	pText->close();


	return 1;
}




//���ܣ����������ı�
//������resultId     ���ش���ʵ���ID
//      pt           �ı������
//      strText      �ı�����
//      strLyName    ͼ����
//      nClrIndex    ��ɫ��
//      strFontType  ����
//      nAlignType   ���뷽ʽ(1:�����,2:���ж���,3:�Ҷ���)
//      dHeight      ���ָ߶�
//      dAngle       ��ת�Ƕ�(����)
//      dWidthFactor ��߱�
//      pDB          ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
long CDistEntityTool::CreateMText(AcDbObjectId& resultId,AcGePoint3d& pt,char* strText,char* strLyName,int  nClrIndex, 
				                  char* strFontType,int nAlignType,double dHeight,double dAngle,AcDbDatabase* pDB)
{
	resultId.setNull();

	//�������ı�
	AcDbMText *pMText = new AcDbMText();
	if(NULL == pMText) return 0;

	//���ö��ı�����
	pMText->setLayer(strLyName);//����ͼ����
	pMText->setLocation(pt);  //���ò����
	CString csText = CString(strText);
	csText.Replace("\n","\\P");
	pMText->setContents(csText.GetBuffer(0)); //��������
	pMText->setTextHeight(dHeight); //�����ָ�
	pMText->setRotation(dAngle);//������ת�Ƕ�
	pMText->setColorIndex(nClrIndex);//������ɫ��

	if(nAlignType == 2)  //���ö��뷽ʽ
		pMText->setAttachment(AcDbMText::AttachmentPoint::kTopCenter);
	else if(nAlignType == 3)
		pMText->setAttachment(AcDbMText::AttachmentPoint::kTopRight);
	else
		pMText->setAttachment(AcDbMText::AttachmentPoint::kTopLeft);

	AcDbTextStyleTableRecordPointer spRecord(strFontType, pDB, AcDb::kForRead); //��������
	if(Acad::eOk ==spRecord.openStatus())
		pMText->setTextStyle(spRecord->objectId());

	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pMText,pDB);
	pMText->close();

	return 1;
}


long CDistEntityTool::SetCurTextStyle(const char* strTextStyleName, AcDbDatabase* pDB)
{
	AcDbTextStyleTableRecordPointer spRecord(strTextStyleName, pDB, AcDb::kForRead);
	Acad::ErrorStatus es = spRecord.openStatus();
	if (es == Acad::eOk)
	{
		es =  pDB->setTextstyle(spRecord->objectId());
	}

	if(es == Acad::eOk) return 1;

	return 0;
}



//���ܣ�����������
//������resultId      ���ش���ʵ���ID
//      pt            �����ò����
//      strBlockName  ������
//      strLyName     ͼ����
//      nClrIndex     ��ɫ��
//      strScale      ���ű���(x,y,z)�ο���ʽ xxx#yyy#zzz#
//      dAngle        ��ת�Ƕ�
//      pDB           ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο����� 
long CDistEntityTool::CreateBlockReference(AcDbObjectId& resultId,AcGePoint3d& pt,const char *strBlockName,const char *strLyName,
										   int  nClrIndex,char *strScale,double dAngle,AcDbDatabase* pDB)  
{
	resultId.setNull();

	AcDbBlockTable *pBlockTable = NULL;
	pDB->getSymbolTable(pBlockTable,AcDb::kForRead);//ȡ���ָ��


	//���ݿ������Ҫ����Ŀ��IDֵ
	AcDbObjectId blockId;
	if(Acad::eOk!=pBlockTable->getAt(strBlockName,blockId,AcDb::kForRead)) 
	{
		pBlockTable->close();
		//acutPrintf("\n�򿪿顾%s��ʧ�ܣ�",strBlockName);
		return 0;
	}

	//����飨�����ʵ���ǲ��������ã�
	try
	{
		AcDbBlockReference *pBlkRef=new AcDbBlockReference;
		pBlkRef->setBlockTableRecord(blockId);//ָ�������õ�ͼ����¼�Ķ���ID
		resbuf to,from;
		from.restype=RTSHORT;//����ͼ��Ҫ�����û����������������ת��
		from.resval.rint=1;//UCS
		to.restype=RTSHORT;
		to.resval.rint=0;//WCS
		AcGeVector3d normal(0.0,0.0,1.0);
		acedTrans(&(normal.x),&from,&to,Adesk::kTrue,&(normal.x));//ת������
		pBlkRef->setPosition(pt); //ָ������
		pBlkRef->setRotation(dAngle);//��ת�Ƕ�
		pBlkRef->setNormal(normal);
		pBlkRef->setLayer(strLyName);//ͼ������
		pBlkRef->setColorIndex(nClrIndex);


		//�ֽ������Ϣ
		double dScale[3]={1.0,1.0,1.0};
		int nLen = strlen(strScale);
		char cValue[20];
		int nIndex =0,nPos=0;
		memset(cValue,0,sizeof(char)*20);
		for(int i=0; i<nLen; i++)
		{
			if(strScale[i] == '#' && nPos > 0)
			{
				dScale[nIndex] = atof(cValue);
				nPos = 0; 
				nIndex++; 
				memset(cValue,0,sizeof(char)*20);
				continue;
			}
		
			cValue[nPos] = strScale[i]; 
			nPos++;	
		}

		AcGeScale3d dGeScale(dScale[0],dScale[1],dScale[2]);
		pBlkRef->setScaleFactors(dGeScale);//�Ŵ����

		AcDbBlockTableRecord *pBlockTableRecord = NULL;
		if(Acad::eOk == pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite))
		{
			pBlockTableRecord->appendAcDbEntity(resultId,pBlkRef);
			pBlockTableRecord->close();
		}
		pBlockTable->close();
		pBlkRef->close();
		return 1;
	}
	catch(...)
	{
		pBlockTable->close();
		return 0;
	}
}


long CDistEntityTool::CreatePoint(AcDbObjectId& resultId,AcGePoint3d& pt,const char *strLyName,int nClrIndex,AcDbDatabase *pDB)
{
	AcDbPoint* pPoint = new AcDbPoint;
	pPoint->setPosition(pt);
	pPoint->setColorIndex(nClrIndex);
	pPoint->setLayer(strLyName);

	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pPoint,pDB);
	pPoint->close();

	return 1;
}


//���ܣ������߶�
//������resultId    ���ش���ʵ���ID
//      ptS,ptE     �߶ε������յ�
//      strLyName   ͼ����
//      nClrIndex   ��ɫ��
//      strLnType   ����
//      dLineWeigth �߿�
//      dThickness  ���
//      pDB         ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
long CDistEntityTool::CreateLine(AcDbObjectId& resultId,AcGePoint3d& ptS,AcGePoint3d& ptE,const char* strLyName,int nClrIndex,
				                 const char* strLnType,AcDb::LineWeight dLineWeigth,double dThickness,AcDbDatabase* pDB)
{
	resultId.setNull();

	//�����߶�ʵ��
	AcDbLine *pLine = new AcDbLine(ptS, ptE);
	if(NULL == pLine) return 0;

	//�����߶�����
	AcDbObjectId LnStyleId;
	pLine->setLayer(strLyName);
	pLine->setColorIndex(nClrIndex);
	if( 0 == GetLineTypeId(LnStyleId,strLnType))
	{
		if( 1 == LoadLineType(LnStyleId,strLnType))
			pLine->setLinetype(LnStyleId);
	}
	else
		pLine->setLinetype(LnStyleId);

	pLine->setLineWeight(dLineWeigth);

	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pLine,pDB);
	pLine->close();

	return 1;

}




//���ܣ�������ά�����
//������resultId      ���ش���ʵ���ID
//      ptArray       ����ߵĵ㼯
//      strLyName     ͼ����
//      nClrIndex     ��ɫ��
//      strLnType     ����
//      dLineWeigth   �߿�
//      dGlobeWeigth  ȫ�ֿ��
//      bIsClosed,    �Ƿ�պ�
//      dThickness    ���
//      pDB         ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
long CDistEntityTool::CreatePoly2dline(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strLyName,              
					                  int  nClrIndex, const char* strLnType,AcDb::LineWeight dLineWeigth,   
					                  double dGlobeWeigth,bool bIsClosed,double dThickness,AcDbDatabase* pDB)

{
	resultId.setNull();

	//������ά�����
	AcDb2dPolyline *pNewPline=new AcDb2dPolyline(AcDb::k2dSimplePoly,ptArray,0,bIsClosed,dGlobeWeigth,dGlobeWeigth);

	if(NULL == pNewPline) return 0;

	//������ɫ
	pNewPline->setColorIndex(nClrIndex);

	//����ͼ��
	pNewPline->setLayer(strLyName);

	//��������
	AcDbObjectId LnStyleId;
	if( 0 == GetLineTypeId(LnStyleId,strLnType))
	{
		if( 1 == LoadLineType(LnStyleId,strLnType))
			pNewPline->setLinetype(LnStyleId);
	}
	else
		pNewPline->setLinetype(LnStyleId);

	//�����߿�
	pNewPline->setLineWeight(dLineWeigth);

	//����ȫ�ֿ��
	pNewPline->setDefaultStartWidth(dGlobeWeigth);
	pNewPline->setDefaultEndWidth(dGlobeWeigth);

	//���ú�ȣ������Կɴ��һЩ������Ϣ��
	pNewPline->setThickness(dThickness);

	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pNewPline,pDB);

	pNewPline->close();

	return 1;
}


//���ܣ����������
//������resultId      ���ش���ʵ���ID
//      ptArray       ����ߵĵ㼯
//      strLyName     ͼ����
//      nClrIndex     ��ɫ��
//      strLnType     ����
//      dLineWeigth   �߿�
//      dGlobeWeigth  ȫ�ֿ��
//      bIsClosed,    �Ƿ�պ�
//      dThickness    ���
//      pDB         ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
long CDistEntityTool::CreatePolyline(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strLyName,              
									 int  nClrIndex, const char* strLnType,AcDb::LineWeight dLineWeigth,   
									 double dGlobeWeigth,bool bIsClosed,double dThickness,AcDbDatabase* pDB)
{
	resultId.setNull();

	int ptCount = ptArray.length();
	if(ptCount < 2) return 0;

	AcDbPolyline* pNewPline = new AcDbPolyline(ptCount);    // optimized to know how many verts

	// append all the vertices (as 2D)
	Acad::ErrorStatus es;
	AcGePoint2d pt;
	for (int i=0;i<ptCount;i++) {
		pt.set(ptArray[i].x, ptArray[i].y);    // squash Z's in the current plane
		es = pNewPline->addVertexAt(i, pt,0,dGlobeWeigth,dGlobeWeigth);//����ȫ�ֿ��
	}

	// set elevation of pline from elevation of first point's Z (as does PLINE command)
	pNewPline->setElevation(ptArray[0].z);
	pNewPline->setDatabaseDefaults();
	TransformToWcs(pNewPline, pDB);


	
	//������ɫ
	pNewPline->setColorIndex(nClrIndex);

	//����ͼ��
	pNewPline->setLayer(strLyName);

	pNewPline->setClosed(bIsClosed);

	//��������
	AcDbObjectId LnStyleId;
	if( 0 == GetLineTypeId(LnStyleId,strLnType))
	{
		if( 1 == LoadLineType(LnStyleId,strLnType))
			pNewPline->setLinetype(LnStyleId);
	}
	else
		pNewPline->setLinetype(LnStyleId);


	

	//���ú�ȣ������Կɴ��һЩ������Ϣ��
	pNewPline->setThickness(dThickness);


	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pNewPline,pDB);

	pNewPline->close();

	return 1;
}



//���ܣ����������
//������resultId      ���ش���ʵ���ID
//      ptArray       ����ߵĵ㼯
//      strLyName     ͼ����
//      nClrIndex     ��ɫ��
//      strLnType     ����
//      dLineWeigth   �߿�
//      dGlobeWeigth  ȫ�ֿ��
//      bIsClosed,    �Ƿ�պ�
//      dThickness    ���
//      pDB         ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����

long CDistEntityTool::CreatePolylineX(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strLyName,int  nClrIndex,const char* strLnType,               
					 AcDb::LineWeight dLineWeigth,   double dGlobeWeigth,bool   bIsClosed,double dThickness,AcDbDatabase* pDB)
{
	resultId.setNull();

	int i,j,nNum,nCount;
	nCount = ptArray.length();
	if(nCount < 2) return 0;

	

	AcGeDoubleArray resultBlgArray;
	if(nCount >=9)//���С��9���������Ϊ����Բ��,û�б�Ҫ���
	{
		if(bIsClosed== true)
			ptArray.append(ptArray[0]);
		nCount = ptArray.length();

		int nSymbol,nTempSymbol;
		
		double dDist,dTempDist,dAngle,dTempAngle;
		AcGePoint3dArray resultPtArray,tempPtArray;

		resultPtArray.setLogicalLength(0);
		//Ϊ������룬�нǣ����Ÿ���ֵ
		dDist = sqrt( (ptArray[0].x-ptArray[1].x)*(ptArray[0].x-ptArray[1].x) + (ptArray[0].y-ptArray[1].y)*(ptArray[0].y-ptArray[1].y) ); 
		dAngle = -1;
		nSymbol = -2;

		tempPtArray.append(ptArray[0]);
		tempPtArray.append(ptArray[1]);


		double dX,dY,dR,dBugle,dArcAngle;
		for(i=0; i<nCount-2; i++)
		{
			//����|i+1,i+2| �ľ���
			dTempDist = sqrt((ptArray[i+1].x-ptArray[i+2].x)*(ptArray[i+1].x-ptArray[i+2].x)+
				(ptArray[i+1].y-ptArray[i+2].y)*(ptArray[i+1].y-ptArray[i+2].y));

			//����|i,i+1,i+2|֮��ļнǣ�������͹�ȹ���˳ʱ�����ʱ����ߣ�
			nTempSymbol = AngleWithTowLineSect(ptArray[i].x,ptArray[i].y,ptArray[i+1].x,ptArray[i+1].y,ptArray[i+2].x,ptArray[i+2].y,dTempAngle);

			tempPtArray.append(ptArray[i+2]);



			if(fabs(dTempDist-dDist)<=0.01) 
			{
				if(dAngle < 0 || nSymbol < -1)  //��һ�Σ�û�бȽ϶���ֱ����Ϊ�Ƿ���������
				{
					dAngle = dTempAngle;
					nSymbol = nTempSymbol;
					continue;
				}
			}

			if(fabs(dTempDist-dDist) > 0.001 || fabs(dTempAngle - dAngle) > 0.0001 || nSymbol != nTempSymbol || nTempSymbol == 0)
			{

				nNum = tempPtArray.length();
				if(nNum < 11)//�����㻡��Ҫ��ֱ�ӱ���
				{
					for(j=0; j<nNum-2; j++)
					{
						resultBlgArray.append(0.0);
						resultPtArray.append(tempPtArray[j]);
					}
					tempPtArray.removeSubArray(0,nNum-3);
				}
				else
				{
					//���㻡�ε�Բ���ĽǶ�
					dArcAngle = (3.14159 - dAngle)*(nNum-2)/2;


					if(dArcAngle > 3.14158)//����Բ
					{
						double dAn = (3.14159-dAngle)/2;
						dX= sqrt((tempPtArray[0].x-tempPtArray[1].x)*(tempPtArray[0].x-tempPtArray[1].x)+
							(tempPtArray[0].y-tempPtArray[1].y)*(tempPtArray[0].y-tempPtArray[1].y))/2;

						dR = dX/sin(dAn);
						dY = dR - dR*cos(dAn);
						dBugle = nSymbol * (dY / dX);

						ads_point ptA,ptB,ptC;
						ptA[0] = tempPtArray[0].x; ptA[1] = tempPtArray[0].y;
						ptB[0] = tempPtArray[1].x; ptB[1] = tempPtArray[1].y;

						double dArclen;
						GetArcCenterPt(ptA,ptB,dBugle,ptC,dArclen);

						AcGePoint3d tempPt3d(ptC[0],ptC[1],0);
						CreateCircle(resultId,tempPt3d,dR,strLyName,nClrIndex,strLnType);
						return 1;

					}

					if(dArcAngle < 1.570795)
					{
						dX= sqrt((tempPtArray[0].x-tempPtArray[nNum-1].x)*(tempPtArray[0].x-tempPtArray[nNum-1].x)+
							(tempPtArray[0].y-tempPtArray[nNum-1].y)*(tempPtArray[0].y-tempPtArray[nNum-1].y))/2;
						dR = dX/sin(dArcAngle);
						dY = dR - dR*cos(dArcAngle);
						dBugle = nSymbol * (dY / dX);

					}
					else
					{
						dArcAngle = 3.14159-dArcAngle;
						dX= sqrt((tempPtArray[0].x-tempPtArray[nNum-1].x)*(tempPtArray[0].x-tempPtArray[nNum-1].x)+
							(tempPtArray[0].y-tempPtArray[nNum-1].y)*(tempPtArray[0].y-tempPtArray[nNum-1].y))/2;
						dR = dX/sin(dArcAngle);
						dY = dR*cos(dArcAngle)+dR;
						dBugle = nSymbol * (dY / dX);
					}

					resultPtArray.append(tempPtArray[0]);
					resultBlgArray.append(dBugle);
					tempPtArray.removeSubArray(0,nNum-3);
				}

				dDist = dTempDist;
				dAngle = -1;
				nSymbol = -2;
			}
		}

		nNum = tempPtArray.length();
		if(nNum <10)
		{
			for(j=0; j<nNum; j++)
			{
				resultPtArray.append(tempPtArray[j]);
				resultBlgArray.append(0.0);
			}
		}
		else
		{
			dArcAngle = (3.14159-dAngle)*(nNum-1)/2;

			if(dArcAngle > 3.14158)//����Բ
			{
				double dAn = (3.14159-dAngle)/2;
				dX= sqrt((tempPtArray[0].x-tempPtArray[1].x)*(tempPtArray[0].x-tempPtArray[1].x)+
					     (tempPtArray[0].y-tempPtArray[1].y)*(tempPtArray[0].y-tempPtArray[1].y))/2;

				dR = dX/sin(dAn);
				dY = dR - dR*cos(dAn);
				dBugle = nSymbol * (dY / dX);

				ads_point ptA,ptB,ptC;
				ptA[0] = tempPtArray[0].x; ptA[1] = tempPtArray[0].y;
				ptB[0] = tempPtArray[1].x; ptB[1] = tempPtArray[1].y;

				double dArclen;
				GetArcCenterPt(ptA,ptB,dBugle,ptC,dArclen);

				AcGePoint3d tempPt3d(ptC[0],ptC[1],0);
				CreateCircle(resultId,tempPt3d,dR,strLyName,nClrIndex,strLnType);
				return 1;

			}

			if(dArcAngle < 1.570795)
			{
				dX= sqrt((tempPtArray[0].x-tempPtArray[nNum-1].x)*(tempPtArray[0].x-tempPtArray[nNum-1].x)+
					(tempPtArray[0].y-tempPtArray[nNum-1].y)*(tempPtArray[0].y-tempPtArray[nNum-1].y))/2;
				dR = dX/sin(dArcAngle);
				dY = dR - dR*cos(dArcAngle);
				dBugle = nSymbol * (dY / dX);
			}
			else
			{
				dArcAngle = 3.14159-dArcAngle;
				dX= sqrt((tempPtArray[0].x-tempPtArray[nNum-1].x)*(tempPtArray[0].x-tempPtArray[nNum-1].x)+
					(tempPtArray[0].y-tempPtArray[nNum-1].y)*(tempPtArray[0].y-tempPtArray[nNum-1].y))/2;
				dR = dX/sin(dArcAngle);
				dY = dR*cos(dArcAngle)+dR;
				dBugle = nSymbol * (dY / dX);

			}
			resultPtArray.append(tempPtArray[0]);
			resultBlgArray.append(dBugle);

			resultPtArray.append(tempPtArray[nNum-1]);
			resultBlgArray.append(0.0);

			tempPtArray.setLogicalLength(0);
		}

		
		if(bIsClosed== true)
			resultPtArray.removeAt(resultPtArray.length()-1);

		ptArray.setLogicalLength(0);
		ptArray.append(resultPtArray);
	}


	nCount = ptArray.length();
	AcDbPolyline* pNewPline = new AcDbPolyline(nCount);    // optimized to know how many verts

	int nTagCount = resultBlgArray.length();

	Acad::ErrorStatus es;
	AcGePoint2d pt;
	for (i=0;i<nCount;i++) {
		pt.set(ptArray[i].x, ptArray[i].y);    // squash Z's in the current plane
		
		if(nTagCount > 0)
			es = pNewPline->addVertexAt(i, pt,resultBlgArray[i],dGlobeWeigth,dGlobeWeigth);//����ȫ�ֿ��
		else
			es = pNewPline->addVertexAt(i, pt,0,dGlobeWeigth,dGlobeWeigth);//����ȫ�ֿ��
	}

	// set elevation of pline from elevation of first point's Z (as does PLINE command)
	pNewPline->setElevation(ptArray[0].z);
	pNewPline->setDatabaseDefaults();
	TransformToWcs(pNewPline, pDB);



	//������ɫ
	pNewPline->setColorIndex(nClrIndex);

	//����ͼ��
	pNewPline->setLayer(strLyName);

	pNewPline->setClosed(bIsClosed);

	//��������
	AcDbObjectId LnStyleId;
	if( 0 == GetLineTypeId(LnStyleId,strLnType))
	{
		if( 1 == LoadLineType(LnStyleId,strLnType))
			pNewPline->setLinetype(LnStyleId);
	}
	else
		pNewPline->setLinetype(LnStyleId);




	//���ú�ȣ������Կɴ��һЩ������Ϣ��
	pNewPline->setThickness(dThickness);


	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pNewPline,pDB);

	pNewPline->close();

	return 1;
}

//���ܣ�����Բ
//������resultId    ���ش���ʵ���ID
//      ptCen       ���ĵ�
//      dRadius     �뾶
//      strLyName   ͼ����
//      nClrIndex   ��ɫ��
//      strLnType   ����
//      dLineWeigth �߿�
//      dThickness  ���
//      pDB         ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
long CDistEntityTool::CreateCircle(AcDbObjectId& resultId,AcGePoint3d& ptCen,double dRadius,const char *strLyName,int  nClrIndex,
				                   const char* strLnType,AcDb::LineWeight dLineWeigth,double dThickness,AcDbDatabase* pDB)
{
	resultId.setNull();

	//����Բ
	AcGeVector3d vctNormal(0.0,0.0,1.0);
	AcDbCircle* pCirc = new AcDbCircle(ptCen,vctNormal,dRadius);
	if(NULL == pCirc) return 0;

	//��������
	pCirc->setColorIndex(nClrIndex);  //������ɫ��

	AcDbObjectId LnStyleId;
	if( 0 == GetLineTypeId(LnStyleId,strLnType)) //��������
	{
		if( 1 == LoadLineType(LnStyleId,strLnType))
			pCirc->setLinetype(LnStyleId);
	}
	else
		pCirc->setLinetype(LnStyleId);

	pCirc->setLineWeight(dLineWeigth); //�����߿�
	pCirc->setThickness(dThickness);


	pCirc->setLayer(strLyName); //����ͼ����


	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pCirc,pDB);

	pCirc->close();

	return 1;
}



//���ܣ�����Բ��
//������resultId    ���ش���ʵ���ID
//      ptCen       ���ĵ�
//      dRadius     �뾶
//      dAngleStart ���Ƕ�(����)
//      dAngleEnd   �յ�Ƕ�(����)
//      strLyName   ͼ����
//      nClrIndex   ��ɫ��
//      strLnType   ����
//      dLineWeigth �߿�
//      dThickness  ���
//      pDB         ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
long CDistEntityTool::CreateArc(AcDbObjectId& resultId,AcGePoint3d& ptCen,double dRadius,double dAngleStart,double dAngleEnd,const char *strLyName,
			                    int  nClrIndex,const char* strLnType,AcDb::LineWeight dLineWeigth,double dThickness,AcDbDatabase* pDB)
{
	resultId.setNull();

	//����Բ��
	AcDbArc *pArc = new AcDbArc(ptCen,dRadius,dAngleStart,dAngleEnd);
	if(NULL == pArc) return 0;

	//��������
	pArc->setColorIndex(nClrIndex);
	pArc->setLineWeight(dLineWeigth);
	pArc->setThickness(dThickness);

	AcDbObjectId LnStyleId;
	if( 0 == GetLineTypeId(LnStyleId,strLnType)) //��������
	{
		if( 1 == LoadLineType(LnStyleId,strLnType))
			pArc->setLinetype(LnStyleId);
	}
	else
		pArc->setLinetype(LnStyleId);

	pArc->setLayer(strLyName); //����ͼ����

	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pArc,pDB);

	pArc->close();

	return 1;
}





//���ܣ����ɶ�������(�޹����߽�)
//������resultId      ���ش���ʵ���ID
//      ptArray       ����������
//      strHatchName  �����ʽ����
//      dScale        ���ͼ������
//      dAngle        ���ͼ����ת�Ƕ�
//      strLyName     ͼ������
//      nClrIndex     ��ɫ��
//      pDB         ���ݿ����ָ��
//���أ��ɹ����� 1 ��ʧ�ܷ��� 0�������ο�����
long CDistEntityTool::CreatePolyHatch(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strHatchName,
					                  double dScale,double dAngle,const char *strLyName,int  nClrIndex,AcDbDatabase* pDB)
{
	//ͼ���ķ���ʸ������������ߣ����ű��������ͼ������䷽ʽ�����߽������
	resultId.setNull();

	int nCount = ptArray.length();
	if(nCount <= 0) return 0;

	AcDbHatch* pHatch = new AcDbHatch();
	if(NULL == pHatch) return 0;

	AcGeVector3d normal(0.0,0.0,1.0);
	pHatch->setNormal(normal);
	pHatch->setElevation(0.0);
	pHatch->setLayer(strLyName);

	pHatch->setAssociative(Adesk::kFalse);
	pHatch->setPatternAngle(dAngle);
	pHatch->setPatternScale(dScale);
	pHatch->setPattern(AcDbHatch::kPreDefined,strHatchName);
	pHatch->setHatchStyle(AcDbHatch::kNormal);
	pHatch->setColorIndex(nClrIndex);

	AcGePoint2dArray vertexPts;
	AcGeDoubleArray vertexBulges;
	vertexPts.setPhysicalLength(0).setLogicalLength(nCount+1);
	for(int j = 0;j<nCount;j++){
		vertexPts[j].set(ptArray[j][X], ptArray[j][Y]);
	}
	vertexPts[nCount].set(ptArray[0][X], ptArray[0][Y]);

	vertexBulges.setPhysicalLength(0).setLogicalLength(nCount+1);
	for(int i=0;i<nCount+1;i++)
		vertexBulges[i] = 0.0;

	pHatch->appendLoop(AcDbHatch::kExternal, vertexPts, vertexBulges); 
	pHatch->evaluateHatch();

	//���ʵ�嵽ģ�Ϳռ�
	AddToCurrentSpace(resultId,pHatch,pDB);
	pHatch->close();

	return 1;
}


long CDistEntityTool::CreatePolyHatch(AcDbObjectId& resultId,AcDbObjectIdArray& dbObjIds,const char *strHatchName,
									  double dScale,double dAngle,const char *strLyName,int  nClrIndex,AcDbDatabase* pDB)
{
	AcDbHatch* pHatch = new AcDbHatch();

	// Set hatch plane
	AcGeVector3d normal(0.0, 0.0, 1.0);
	pHatch->setNormal(normal);
	pHatch->setElevation(0.0);
	pHatch->setLayer(strLyName);

	pHatch->setAssociative(Adesk::kTrue);
	pHatch->setPatternAngle(dAngle);
	pHatch->setPatternScale(dScale);
	pHatch->setPattern(AcDbHatch::kPreDefined,strHatchName);
	pHatch->setHatchStyle(AcDbHatch::kOuter);//kNormal
	pHatch->setColorIndex(nClrIndex);


	if(dbObjIds.length() >1) //��Ҫ��һ���Ľ�
	{
		AcDbObjectIdArray tempArray;
		tempArray.append(dbObjIds.at(0));
		pHatch->appendLoop(AcDbHatch::kExternal, tempArray);
		dbObjIds.removeAt(0);
		pHatch->appendLoop(AcDbHatch::kDefault,dbObjIds);
	}
	else
		pHatch->appendLoop(AcDbHatch::kDefault, dbObjIds);

	// Elaborate hatch lines
	pHatch->evaluateHatch();

	dbObjIds.setLogicalLength(0);
	pHatch->getAssocObjIds(dbObjIds);

	// Post hatch entity to database
	AddToCurrentSpace(resultId,pHatch,pDB);
	pHatch->close();

	// Attach hatchId to all source boundary objects for notification.
	AcDbEntity *pEnt;
	int numObjs = dbObjIds.length();
	
	for (int i = 0; i < numObjs; i++) {
		Acad::ErrorStatus es = acdbOpenAcDbEntity(pEnt, dbObjIds[i], AcDb::kForWrite);
		if (es == Acad::eOk) {
			pEnt->addPersistentReactor(resultId);
			pEnt->close();
		}
	}
	return 1;
}




//------------------------------------------------------------------------------------------------------------------------

// ���� : ��ֵ���߿�ת��ΪCAD�߿�ֵ
AcDb::LineWeight CDistEntityTool::DoubleToLnWeight(double dLnWid)
{
	if(dLnWid <= -2.99)       return AcDb::kLnWtByLwDefault;
	else if(dLnWid <= -1.99)  return AcDb::kLnWtByBlock;
	else if(dLnWid < -0.99)   return AcDb::kLnWtByLayer;
	else if(dLnWid < 0.00001) return AcDb::kLnWt000;
	else if(dLnWid <= 0.05)   return AcDb::kLnWt005;
	else if(dLnWid <= 0.09)   return AcDb::kLnWt009;
	else if(dLnWid <= 0.13)   return AcDb::kLnWt013;
	else if(dLnWid <= 0.15)   return AcDb::kLnWt015;
	else if(dLnWid <= 0.18)   return AcDb::kLnWt018;
	else if(dLnWid <= 0.20)   return AcDb::kLnWt020;
	else if(dLnWid <= 0.25)   return AcDb::kLnWt025;
	else if(dLnWid <= 0.30)   return AcDb::kLnWt030;
	else if(dLnWid <= 0.35)   return AcDb::kLnWt035;
	else if(dLnWid <= 0.40)   return AcDb::kLnWt040;
	else if(dLnWid <= 0.50)   return AcDb::kLnWt050;
	else if(dLnWid <= 0.53)   return AcDb::kLnWt053;
	else if(dLnWid <= 0.60)   return AcDb::kLnWt060;
	else if(dLnWid <= 0.70)   return AcDb::kLnWt070;
	else if(dLnWid <= 0.80)   return AcDb::kLnWt080;
	else if(dLnWid <= 0.90)   return AcDb::kLnWt090;
	else if(dLnWid <= 1.00)   return AcDb::kLnWt100;
	else if(dLnWid <= 1.06)   return AcDb::kLnWt106;
	else if(dLnWid <= 1.20)   return AcDb::kLnWt120;
	else if(dLnWid <= 1.40)   return AcDb::kLnWt140;
	else if(dLnWid <= 1.58)   return AcDb::kLnWt158;
	else if(dLnWid <= 2.00)   return AcDb::kLnWt200;
	else if(dLnWid > 2.00)    return AcDb::kLnWt211;
	else  return AcDb::kLnWtByLayer;
}

// ��AcDb::lineWeight���͵�ֵת����Double
double CDistEntityTool::LnWeightToDouble(AcDb::LineWeight type)
{
	if (type == AcDb::kLnWt000)      return 0.00;
	else if (type == AcDb::kLnWt005) return 0.05;
	else if (type == AcDb::kLnWt009) return 0.09;
	else if (type == AcDb::kLnWt013) return 0.13;
	else if (type == AcDb::kLnWt015) return 0.15;
	else if (type == AcDb::kLnWt018) return 0.18;
	else if (type == AcDb::kLnWt020) return 0.20;
	else if (type == AcDb::kLnWt025) return 0.25;
	else if (type == AcDb::kLnWt030) return 0.30;
	else if (type == AcDb::kLnWt035) return 0.35;
	else if (type == AcDb::kLnWt040) return 0.40;
	else if (type == AcDb::kLnWt050) return 0.50;
	else if (type == AcDb::kLnWt053) return 0.53;
	else if (type == AcDb::kLnWt060) return 0.60;
	else if (type == AcDb::kLnWt070) return 0.70;
	else if (type == AcDb::kLnWt080) return 0.80;
	else if (type == AcDb::kLnWt090) return 0.90;
	else if (type == AcDb::kLnWt100) return 1.00;
	else if (type == AcDb::kLnWt106) return 1.06;
	else if (type == AcDb::kLnWt120) return 1.20;
	else if (type == AcDb::kLnWt140) return 1.40;
	else if (type == AcDb::kLnWt158) return 1.58;
	else if (type == AcDb::kLnWt200) return 2.00;
	else if (type == AcDb::kLnWt211) return 2.11;
	else if (type == AcDb::kLnWtByLayer) return -1;
	else if (type == AcDb::kLnWtByBlock) return -2;
	else if (type == AcDb::kLnWtByLwDefault) return -3;
	else  return 0.00;
}


// ����CADʵ������
long CDistEntityTool::GetLineTypeId(AcDbObjectId& resultId,const char* strLnTypeName,AcDbDatabase* pDB)
{
	resultId.setNull();

	AcDbLinetypeTable		*pDbLtT;
	pDB->getSymbolTable(pDbLtT, AcDb::kForRead);

	if (!pDbLtT->has(strLnTypeName))
	{
		pDbLtT->close();
		return 0;
	}
	else
	{
		pDbLtT->getAt(strLnTypeName, resultId);
		pDbLtT->close();
		return 1;
	}
}

// ����CADʵ������
long CDistEntityTool::LoadLineType(AcDbObjectId& resultId,const char* strLnTypeName,AcDbDatabase* pDB)
{

	AcDbLinetypeTable		* pDbLtT;
	Acad::ErrorStatus		es;

	resultId.setNull();

	es = pDB->loadLineTypeFile(strLnTypeName, ".\\acadiso.lin");
	if( es != Acad::eOk)
	{
		char charBuff[1024]={0};
		sprintf(charBuff, "\n������ʾ:û�з��� %s ����",strLnTypeName);
		//acutPrintf(charBuff);
		return 0;
	}
	else
	{
		es = pDB->getLinetypeTable(pDbLtT, AcDb::kForRead);
		if ( es == Acad::eOk)
		{
			pDbLtT->getAt(strLnTypeName, resultId);
			pDbLtT->close();
			return 1;
		}
		else
		{
			pDbLtT->close();
			char charBuff[1024]={0};
			sprintf(charBuff, "\n������ʾ:�������� %s ʧ��!",strLnTypeName);
			//acutPrintf(charBuff);
			return 0;
		}
	}

}



long CDistEntityTool::AddToCurrentSpace(AcDbObjectId& resultId,AcDbEntity* pEnt,AcDbDatabase* pDB)
{	
	resultId.setNull();

	//�򿪱�
	AcDbBlockTableRecord* pBlkRec = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pBlkRec, pDB->currentSpaceId(), AcDb::kForWrite);
	if (es != Acad::eOk)
	{
		//acutPrintf("\n ��ģ�Ϳռ�ʧ�ܣ�");
		return 0;
	}

	//���ʵ��
	es = pBlkRec->appendAcDbEntity(resultId,pEnt);
	if (es != Acad::eOk) {
		pEnt->close();
		return 0;
	}
	pBlkRec->close();

	return 1;
}




 long CDistEntityTool::CreateTextStyle(AcDbObjectId& resultId,const char* csTsName,const char* szFlNm,const char* szBigFlNm,AcDbDatabase* pDB)
{
	resultId.setNull();

	AcDbTextStyleTable *pTsTbl;
	if(pDB->getSymbolTable(pTsTbl,AcDb::kForWrite) != Acad::eOk){
		return 0;
	}

	if(Acad::eOk != pTsTbl->getAt(csTsName,resultId)){
		AcDbTextStyleTableRecord *pTstRcd = new AcDbTextStyleTableRecord;
		pTstRcd->setName(csTsName);
		pTstRcd->setFileName(szFlNm);
		pTstRcd->setBigFontFileName(szBigFlNm);
		pTstRcd->setTextSize(3.0);
		pTstRcd->setXScale(1.0);
		pTsTbl->add(resultId,pTstRcd);
		pTstRcd->close();
	}
	pTsTbl->close();

	return 1;
}


// ��ȡ������ʽ��ID
long CDistEntityTool::GetTextStyleObjID(AcDbObjectId& resultId,const char* strTextStyleName,AcDbDatabase* pDB)
{
	resultId.setNull();

	AcDbTextStyleTable *pTextStyleTable;

	pDB->getTextStyleTable(pTextStyleTable, AcDb::kForRead);

	pTextStyleTable->close();

	resultId.setNull();

	if (Acad::eOk != pTextStyleTable->getAt(strTextStyleName, resultId))
	{
		//acutPrintf("�Ҳ���\"%s\"������",strTextStyleName);

		return 0;
	}

	return 1;
}



long CDistEntityTool::GetTextStyleName(char *strTextStyleName,AcDbObjectId& ObjId,AcDbDatabase *pDB)
{
	Acad::ErrorStatus es;

	//��Text��
	AcDbTextStyleTable *pTsTable;
	es = pDB->getSymbolTable(pTsTable,AcDb::kForRead);
	if (es != Acad::eOk)  return 0;

	//����ö������
	AcDbTextStyleTableIterator *pTsIter;
	es = pTsTable->newIterator(pTsIter);
	if (es != Acad::eOk) {
		pTsTable->close();return 0;
	}

	//������¼�����Ҷ�ӦObjectIdʵ������ƣ�
	AcDbTextStyleTableRecord *pTsRecord = NULL;
	for (; !pTsIter->done(); pTsIter->step()) 
	{
		es = pTsIter->getRecord(pTsRecord,AcDb::kForRead);
		if (es != Acad::eOk) {  //��ȡ��¼ʧ�ܣ����ش���
			delete pTsIter;pTsTable->close();return 0;
		}
		
		char* pTempName = NULL;
		if(pTsRecord->id() == ObjId)
		{
			pTsRecord->getName(pTempName);
			strcpy(pTempName,strTextStyleName);
			pTsRecord->close();
			break;
		}
		pTsRecord->close();
	}
	delete pTsIter;
	es = pTsTable->close();

	return 1;
}


void CDistEntityTool::SetXData(AcDbEntity* pEnt,CStringArray& strValue,const char* strRegName)
{

	struct  resbuf  *pHead=NULL, *pTemp=NULL;
	int nNum = strValue.GetSize();
	if(NULL == pEnt || nNum < 1) return;


	pHead = pEnt->xData(strRegName);
	if (NULL == pHead)
		acdbRegApp(strRegName);

	pHead = acutNewRb(AcDb::kDxfRegAppName);
	pTemp = pHead;
	pTemp->resval.rstring = (char*) malloc(strlen(strRegName) + 1);
	strcpy(pTemp->resval.rstring, strRegName);


	for(int i=0; i<nNum; i++)
	{
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdAsciiString);
		pTemp = pTemp->rbnext;
		pTemp->resval.rstring = (char*) malloc(strValue[i].GetLength() + 1);
		strcpy(pTemp->resval.rstring, strValue[i].GetBuffer(0));
		pTemp->rbnext = NULL;
	}

	//pEnt->upgradeOpen(); //�� Read ��ʽ�� Write ��ʽ
	Acad::ErrorStatus tt = pEnt->setXData(pHead);

	//pEnt->close();   //�ɵ��ú����ر�
	acutRelRb(pHead);
}



void CDistEntityTool::GetXData(AcDbEntity* pEnt,CStringArray& strValue,const char* strRegName)
{
	struct resbuf *pHead = NULL;
	pHead = pEnt->xData(strRegName);
	if (pHead != NULL) 
	{
		while(NULL != pHead->rbnext)
		{
			CString str = pHead->rbnext->resval.rstring;
			strValue.Add(str);
			pHead = pHead->rbnext;
		}
		acutRelRb(pHead);
	} 

	//pEnt->close(); //�ɵ��ú����ر�
}


//���ܣ�ȡ������ͼ��
//������layerArray  ͼ������
//���أ�es 
long  CDistEntityTool::GetAllLayerName(CStringArray& layerArray,AcDbDatabase* pDB)
{
	Acad::ErrorStatus es=Acad::eOk;

	layerArray.RemoveAll();

	AcDbLayerTable *pLayerTable=NULL;
	if((es=pDB->getSymbolTable(pLayerTable,AcDb::kForRead))!=Acad::eOk)
	{
		pLayerTable->close();
		return 0;
	}
	//����һ����������
	AcDbLayerTableIterator *pLayerTableIterator;
	if ( (es =pLayerTable->newIterator (pLayerTableIterator) ) != Acad::eOk ) 
	{
		//acutPrintf ("\n���ܴ����������") ;
		pLayerTable->close () ;
		return 0;
	}
	pLayerTable->close () ;

	//ȡ��ͼ��
	char *pLayerName=NULL;
	CString strname;
	for(pLayerTableIterator->start ();!pLayerTableIterator->done();pLayerTableIterator->step())
	{
		AcDbLayerTableRecord *pLayerTableRecord=NULL;
		pLayerTableIterator->getRecord(pLayerTableRecord,AcDb::kForRead);
		pLayerTableRecord->getName(pLayerName);
		strname.Format("%s",pLayerName);
		pLayerTableRecord->close();
		layerArray.Add(strname);
	}

	if(pLayerName) 
		acutDelString(pLayerName);
	delete pLayerTableIterator;
	pLayerTableIterator=NULL;

	return 1;
}



long  CDistEntityTool::GetPoly2dlinePts(AcDb2dPolyline* pPolyline2d,AcGePoint3dArray& ptArray)
{
	AcDbObjectIterator *pObjIter = pPolyline2d->vertexIterator();
	if(pObjIter == NULL) return 0;

	AcDb2dVertex *pVertex;
	AcGePoint3dArray VertexArr;
	CArray<double,double> BugleArr;

	AcDb::Poly2dType PolyType = pPolyline2d->polyType();

	//�����㣬����ȡ��͹��
	for(pObjIter->start();!pObjIter->done();pObjIter->step())
	{
		if(pObjIter->entity())  return 0;

		if(acdbOpenObject(pVertex,pObjIter->objectId(),AcDb::OpenMode::kForRead) != Acad::eOk) return 0;

		if(PolyType == AcDb::Poly2dType::k2dSimplePoly){
			if(pVertex->vertexType() != AcDb::k2dVertex){
				pVertex->close();
				continue;
			}
		}
		else if((PolyType == AcDb::Poly2dType::k2dCubicSplinePoly) || (PolyType == AcDb::Poly2dType::k2dQuadSplinePoly))
		{
			if(pVertex->vertexType() != AcDb::k2dSplineCtlVertex){
				pVertex->close();
				continue;
			}
		}
		else if(PolyType == AcDb::Poly2dType::k2dFitCurvePoly)
		{

		}

		BugleArr.Add(pVertex->bulge());
		VertexArr.append(pVertex->position());

		pVertex->close();
	}


	int nCount = VertexArr.length();
	if(VertexArr.logicalLength()<2)  return 0;

	int nFind=-1;
	for(int j=0;j<nCount-1;j++)
	{
		if(fabs(BugleArr[j])<0.000001)
		{ 
			ptArray.append(AcGePoint3d(VertexArr[j].x,VertexArr[j].y,0));
		}
		else
		{
			AcGePoint3dArray tempPts;
			ReduceBulge(tempPts,VertexArr[j],VertexArr[j+1],BugleArr[j],10);
			for(int i=0;i<tempPts.logicalLength();i++)
			{
				nFind=-1;
				if(!ptArray.find(AcGePoint3d(tempPts[i].x,tempPts[i].y,0),nFind))
				{
					ptArray.append(tempPts[i]);
				}
			}
		}
	}


	nFind=-1;
	if(!ptArray.find(VertexArr[nCount-1],nFind))
	{
		ptArray.append(VertexArr[nCount-1]);
	}


	if(pPolyline2d->isClosed())
	{  
		ptArray.append(ptArray[0]);
	}

	return 1;
}


//������ID����ȡ������2d�㼯��
long CDistEntityTool::GetPoly2dlinePts(AcDbObjectId poly2dlineId,AcGePoint3dArray& ptArray)
{
	long nResult = 0;
	AcDb2dPolyline *pPoly2dLine = NULL;
	acdbOpenObject(pPoly2dLine, poly2dlineId, AcDb::kForRead);
	if(NULL == pPoly2dLine) return nResult;

	nResult = GetPoly2dlinePts(poly2dlineId,ptArray);

	pPoly2dLine->close();

	return nResult;
}



long CDistEntityTool::GetPoly3dlinePts(AcDb3dPolyline* pPolyline3d,AcGePoint3dArray& ptArray)
{
	AcDbObjectIterator *pObjIter=pPolyline3d->vertexIterator();
	if(pObjIter==NULL) return 0;

	AcDb3dPolylineVertex *pVertex;
	AcDb::Poly3dType nType = pPolyline3d->polyType();

	for(pObjIter->start();!pObjIter->done();pObjIter->step())
	{
		if(pObjIter->entity()) return 0;
		if(acdbOpenObject(pVertex,pObjIter->objectId(),AcDb::OpenMode::kForRead)!=Acad::eOk)
			return 0;
		if(nType==AcDb::Poly3dType::k3dSimplePoly)
		{
			if(pVertex->vertexType()!=AcDb::k3dSimpleVertex)
			{
				pVertex->close();
				continue;
			}
		}
		else if(pVertex->vertexType() != AcDb::k3dControlVertex)
		{
			pVertex->close();
			continue;
		}
		ptArray.append(pVertex->position());
		pVertex->close();
	}

	if(pPolyline3d->isClosed())
		ptArray.append(ptArray[0]);
	return 1;
}


long CDistEntityTool::GetPoly3dlinePts(AcDbObjectId poly3dlineId,AcGePoint3dArray& ptArray)
{
	AcDb3dPolyline *pPolyline=NULL;
	if(acdbOpenObject(pPolyline,poly3dlineId,AcDb::kForRead)!=Acad::eOk) return 0;

	return GetPoly3dlinePts(pPolyline,ptArray);
}




//������ID����ȡ�����ߵ㼯��
long CDistEntityTool::GetPolylinePts(AcDbObjectId polylineID,AcGePoint3dArray& ptArray)
{
	int nResult = 0;
	AcDbPolyline *pPolyline=NULL;
	if(acdbOpenObject(pPolyline,polylineID,AcDb::kForRead)!=Acad::eOk) return nResult;
	
	nResult = GetPolylinePts(pPolyline,ptArray);
	pPolyline->close();

	return nResult;
}


long  CDistEntityTool::GetPolylinePts(AcDbPolyline *pPolyline,AcGePoint3dArray& ptArray)
{
	
	double dBulge;
	AcGePoint3d ptAt;
	AcGeDoubleArray  VertBlgArray;
	AcGePoint3dArray VertPtArray;
	int nNumVert = pPolyline->numVerts(); //��ȡ������
	for(int i=0;i<nNumVert;i++)  //�����������͹��
	{

		pPolyline->getPointAt(i,ptAt);
		pPolyline->getBulgeAt(i,dBulge);
		VertPtArray.append(ptAt);
		VertBlgArray.append(dBulge);
	}
	if(pPolyline->isClosed())
	{
		VertPtArray.append(VertPtArray[0]);
		VertBlgArray.append(0);
	}
	if(VertPtArray.logicalLength()<2) return 0;

	

	int nFound = -1;
	AcGePoint3dArray tempPtArray; 
	for(i=0;i<VertPtArray.logicalLength()-1;i++)
	{
		if(fabs(VertBlgArray[i])<0.0001)  //û��͹��
		{ 
			ptArray.append(AcGePoint3d(VertPtArray[i].x,VertPtArray[i].y,0));
		}
		else
		{
			
			ads_point startPt,endPt,centerPt;
			startPt[0] = VertPtArray[i].x;startPt[1] = VertPtArray[i].y; startPt[2] = 0;
			endPt[0] = VertPtArray[i+1].x;endPt[1] = VertPtArray[i+1].y; endPt[2] = 0;

			double dArcLen = 0.0;
			GetArcCenterPt(startPt,endPt,VertBlgArray[i],centerPt,dArcLen);

			int nPart = dArcLen / 5;
			if(nPart <10) nPart = 10;

			tempPtArray.setLogicalLength(0); 
			ReduceBulge(tempPtArray,VertPtArray[i],VertPtArray[i+1],VertBlgArray[i],nPart);
			for(int i=0;i<tempPtArray.logicalLength();i++)
			{
				if(!ptArray.find(AcGePoint3d(tempPtArray[i].x,tempPtArray[i].y,0),nFound))
				{
					ptArray.append(AcGePoint3d(tempPtArray[i].x,tempPtArray[i].y,0));
				}
			}
		}

	}

	if(!ptArray.find(AcGePoint3d(VertPtArray[i].x,VertPtArray[i].y,0),nFound))
	{
		ptArray.append(AcGePoint3d(VertPtArray[i].x,VertPtArray[i].y,0));
	}
	if(pPolyline->isClosed())
	{  
		ptArray.append(ptArray[0]);
	}

	return 1;
}


void CDistEntityTool::TransformToWcs(AcDbEntity* ent, AcDbDatabase* pDB)
{
	ASSERT(ent->isWriteEnabled());
	ASSERT(db != NULL);

	AcGeMatrix3d m;
	if (!acdbUcsMatrix(m, pDB)) {
		m.setToIdentity();
		ASSERT(0);
	}

	Acad::ErrorStatus es = ent->transformBy(m);

	ASSERT(es == Acad::eOk);
}

long CDistEntityTool::DyZoom(AcGePoint2d& CenterPt,double dHeight,double dWidth)
{
	Acad::ErrorStatus es;
	AcDbViewTableRecord *pView = new AcDbViewTableRecord();
	pView->setCenterPoint(CenterPt);
	pView->setFrontClipAtEye(false);
	pView->setHeight(dHeight);
	pView->setWidth(dWidth);
	es=acedSetCurrentView(pView,NULL);
	pView->close();
	delete pView;

	if(es == Acad::eOk) return 1;

	return 0;
} 


//�õ��ı��߽�
long CDistEntityTool::GetTextBoundary(AcGePoint3dArray& ptArray,AcDbObjectId& objectId,double dOffset)
{
	AcDbExtents Ext;
	AcDbEntity *pEnt;
	if(Acad::eOk != acdbOpenObject(pEnt,objectId,AcDb::kForWrite)) return 0;
	if(pEnt->isKindOf(AcDbText::desc()))
	{
		AcDbText *pText=AcDbText::cast(pEnt);
		AcGePoint3d basePoint;
		basePoint=pText->position();
		double rotateAngle=pText->rotation();
		pText->setRotation(0);
		pText->getGeomExtents(Ext);
		AcGePoint3d minPt,maxPt;
		minPt=Ext.minPoint();
		maxPt=Ext.maxPoint();
		AcGePoint3dArray pointArray;
		AcGePoint3d point1,point2,point3,point4;
		point1.x=minPt.x-dOffset;point1.y=minPt.y-dOffset;point1.z=0;
		pointArray.append(point1);
		point2.x=maxPt.x+dOffset;point2.y=minPt.y-dOffset;point2.z=0;
		pointArray.append(point2);
		point3.x=maxPt.x+dOffset;point3.y=maxPt.y+dOffset;point3.z=0;
		pointArray.append(point3);
		point4.x=minPt.x-dOffset;point4.y=maxPt.y+dOffset;point4.z=0;
		pointArray.append(point4);
		pText->setRotation(rotateAngle);
	}
	else if(pEnt->isKindOf(AcDbMText::desc()))
	{
		AcDbMText *pMtext=AcDbMText::cast(pEnt);
		AcGePoint3d basePoint;
		basePoint=pMtext->location();
		double rotateAngle=pMtext->rotation();
		pMtext->setRotation(0);
		AcGePoint3dArray pointArray;
		double width=pMtext->actualWidth();
		double height=pMtext->actualHeight();
		AcGePoint3d point1,point2,point3,point4;
		point1.x=basePoint.x-dOffset;point1.y=basePoint.y+dOffset;point1.z=0;
		pointArray.append(point1);
		point2.x=basePoint.x+width+dOffset;point2.y=basePoint.y+dOffset;point2.z=0;
		pointArray.append(point2);
		point3.x=basePoint.x+width+dOffset;point3.y=basePoint.y-height-dOffset;point3.z=0;
		pointArray.append(point3);
		point4.x=basePoint.x-dOffset;point4.y=basePoint.y-height-dOffset;point4.z=0;
		pointArray.append(point4);
		pMtext->setRotation(rotateAngle);
	}
	pEnt->close();
	return 1;
}


AcGeVector3d CDistEntityTool::Convert2dVectorTo3d(AcGeVector2d pt)
{
	AcGeVector3d pm;
	pm.set(pt.x,pt.y,0);
	return pm;
}
AcGeVector2d CDistEntityTool::Convert3dVectorTo2d(AcGeVector3d pt)
{
	AcGeVector2d pm;
	pm.set(pt.x,pt.y);
	return pm;
}
AcGePoint3d CDistEntityTool::Convert2dPointTo3d(AcGePoint2d pt)
{
	AcGePoint3d pm;
	pm.set(pt.x,pt.y,0);
	return pm;
}
AcGePoint2d CDistEntityTool::Convert3dPointTo2d(AcGePoint3d pt)
{
	AcGePoint2d pm;
	pm.set(pt.x,pt.y);
	return pm;
}




void CDistEntityTool::ReduceBulge(AcGePoint3dArray &ptDfArr,AcGePoint3d ptA,AcGePoint3d ptB,double dBulge,int nPartNum)
{
	AcDbPolyline pLine;
	pLine.addVertexAt(0,AcGePoint2d(ptA.x,ptA.y),dBulge);
	pLine.addVertexAt(1,AcGePoint2d(ptB.x,ptB.y),0);
	double dEndParam;
	double dLength;
	double dDfLen=0;
	AcGePoint3d pt;
	pLine.getEndParam(dEndParam);
	pLine.getDistAtParam(dEndParam,dLength);
	dDfLen=dLength/nPartNum;
	for(int i=0; i<nPartNum; i++)
	{
		double dDist = dDfLen * i;
		pLine.getPointAtDist(dDist,pt); pt.z = 0;
		ptDfArr.append(pt);
	}

	pLine.getEndPoint(pt); pt.z = 0;
	ptDfArr.append(pt); 
}

long CDistEntityTool::GetSplinePts(AcGePoint3dArray& ptArray,AcDbSpline* pSpline,int nSegPartNum)
{
	int i,j;
	double segDis,startDis,endDis;
	AcGePoint3d startPt,endPt,tempPt;

	for(i=0;i<pSpline->numFitPoints()-1;i++)
	{
		pSpline->getFitPointAt(i,startPt);
		pSpline->getFitPointAt(i+1,endPt);
		pSpline->getDistAtPoint(startPt,startDis);
		pSpline->getDistAtPoint(endPt,endDis);
		segDis =(endDis-startDis)/nSegPartNum;
		ptArray.append(startPt);
		for(j=1;j<nSegPartNum;j++)
		{
			pSpline->getPointAtDist(startDis+segDis*j,tempPt);
			ptArray.append(tempPt);
		}
	}
	ptArray.append(endPt);

	if(pSpline->isClosed())
		ptArray.append(ptArray[0]);

	return 1;
}

long CDistEntityTool::GetEllipsPts(AcGePoint3dArray& ptArray,AcDbEllipse* pE,int nSegPartNum)
{
	double dEndParam;
	double dDist =0.0;
	AcGePoint3d tempPt;
	pE->getEndParam(dEndParam);
	pE->getDistAtParam(dEndParam,dDist);

	double segDis = dDist/nSegPartNum;

	for(int i=0; i<=nSegPartNum; i++)
	{
		pE->getPointAtDist(segDis*i,tempPt);
		ptArray.append(tempPt);
	}

	return 1;
}

long CDistEntityTool::GetArcPts(AcGePoint3dArray& ptArray,AcDbArc* pArc,int nSegPartNum)
{
	double dEndParam;
	double dDist =0.0;
	AcGePoint3d tempPt;
	pArc->getEndParam(dEndParam);
	pArc->getDistAtParam(dEndParam,dDist);

	double segDis = dDist/nSegPartNum;

	for(int i=0; i<=nSegPartNum; i++)
	{
		pArc->getPointAtDist(segDis*i,tempPt);
		ptArray.append(tempPt);
	}

	return 1;
}


long CDistEntityTool::GetCirclePts(AcGePoint3dArray& ptArray,AcDbCircle* pCircle,int nSegPartNum)
{
	double dEndParam;
	double dDist =0.0;
	AcGePoint3d tempPt;
	pCircle->getEndParam(dEndParam);
	pCircle->getDistAtParam(dEndParam,dDist);

	double segDis = dDist/nSegPartNum;

	for(int i=0; i<=nSegPartNum; i++)
	{
		pCircle->getPointAtDist(segDis*i,tempPt);
		ptArray.append(tempPt);
	}
	
	return 1;
}


//��֪һ�λ��������յ��Լ���͹��,����Բ��,�ͻ���
long CDistEntityTool::GetArcCenterPt(ads_point startPoint,ads_point endPoint,double bulge,ads_point& center,double& dArcLen)
{
	if (fabs(bulge) < 0.0001)
	{
		ads_point_set(startPoint,center);  return 0;
	}
	ads_point startPt,endPt;
	if (bulge < 0.0)
	{
		ads_point_set(endPoint,startPt);
		ads_point_set(startPoint,endPt);
		bulge=bulge*(-1.0);
	}
	else
	{
		ads_point_set(startPoint,startPt);
		ads_point_set(endPoint,endPt);
	}
	double distStartToEnd,distX,distY,radius;
	distStartToEnd=ads_distance(startPt,endPt);
	distX=distStartToEnd/2.0;
	distY=bulge*distX;
	radius=((distX*distX)+(distY*distY))/(2.0*distY);  //��Բ�뾶

	double tmpAng;
	ads_point tmpPt;

	tmpAng=ads_angle(startPt,endPt);
	ads_polar(startPt,tmpAng,distX,tmpPt);
	ads_polar(tmpPt,(tmpAng+(3.14159265358979/2.0)),(radius-distY),center);

	if(bulge < 1.0001)
	{
		dArcLen = asin(distY/radius)*2 * radius;
	}
	else
	{
		dArcLen = radius * (3.14159 - atan(bulge)*2)*2;
	}

	return 1;

}; 



////////////////////////////////////////////////////////////////////////////////////////////////
//�������ܣ����߶�BL,�߶�BR�ļнǣ���ȷ��BL��BR�Ǹ��߶�����࣬�Ǹ��߶����Ҳ�
//��ڲ�����
//          bx,by,lx,ly    �߶�BL 
//          .. ..,rx,ry    �߶�BR
//���ڲ�����
//          dAngle         �нǴ�С(����)
//����ֵ��-1:˳ʱ��   0 ����   1 ��ʱ��
////////////////////////////////////////////////////////////////////////////////////////////////
long CDistEntityTool::AngleWithTowLineSect(double lx,double ly,double bx,double by,double rx,double ry,double& dAngle)
{
	double dResult;
	double Sbl=0,Sbr=0,Slr=0;

	double dS = fabs((lx*by+bx*ry+rx*ly)-(ly*bx+by*rx+ry*lx));

	Sbl = (lx-bx)*(lx-bx)+(ly-by)*(ly-by);       // |BL|*|BL|
	Sbr = (rx-bx)*(rx-bx)+(ry-by)*(ry-by);       // |BR|*|BR|
	Slr = (lx-rx)*(lx-rx)+(ly-ry)*(ly-ry);       // |LR|*|LR|

	dAngle = asin(dS/(sqrt(Sbl) * sqrt(Sbr)));

	if(Slr > Sbl+Sbr)
		dAngle = 3.14159 - dAngle;

	dResult = (lx-bx)*(ry-by) - (rx-bx)*(ly-by);


	if(fabs(dResult) < 0.00001) //Ϊ��
	{
		dAngle = 3.14159;
		return 0;
	}
	else if( dResult < 0 ) //(�㣨lx,ly)�ڵ�(rx,ry)�����
		return 1;
	else
		return -1;
}


long CDistEntityTool::SimulationArc(AcGePoint3dArray& ptArray,AcGeDoubleArray& resultBlgArray)
{
	int i,j,nNum,nCount;
	nCount = ptArray.length();
	if(nCount < 9) return 1;  //���С��9���������Ϊ����Բ��

	int nSymbol,nTempSymbol;
	double dDist,dTempDist,dAngle,dTempAngle;
	AcGePoint3dArray resultPtArray,tempPtArray;

	resultPtArray.setLogicalLength(0);

	//Ϊ������룬�нǣ����Ÿ���ֵ
	dDist = sqrt( (ptArray[0].x-ptArray[1].x)*(ptArray[0].x-ptArray[1].x) + (ptArray[0].y-ptArray[1].y)*(ptArray[0].y-ptArray[1].y) ); 
    dAngle = -1;
	nSymbol = -2;

	tempPtArray.append(ptArray[0]);
	tempPtArray.append(ptArray[1]);


	double dX,dY,dR,dBugle,dArcAngle;
	for(i=0; i<nCount-2; i++)
	{
		//����|i+1,i+2| �ľ���
		dTempDist = sqrt((ptArray[i+1].x-ptArray[i+2].x)*(ptArray[i+1].x-ptArray[i+2].x)+
			             (ptArray[i+1].y-ptArray[i+2].y)*(ptArray[i+1].y-ptArray[i+2].y));

		//����|i,i+1,i+2|֮��ļнǣ�������͹�ȹ���˳ʱ�����ʱ����ߣ�
		nTempSymbol = AngleWithTowLineSect(ptArray[i].x,ptArray[i].y,ptArray[i+1].x,ptArray[i+1].y,ptArray[i+2].x,ptArray[i+2].y,dTempAngle);
		
		tempPtArray.append(ptArray[i+2]);

		

		if(fabs(dTempDist-dDist)<=0.01) 
		{
			if(dAngle < 0 || nSymbol < -1)  //��һ�Σ�û�бȽ϶���ֱ����Ϊ�Ƿ���������
			{
				dAngle = dTempAngle;
				nSymbol = nTempSymbol;
				continue;
			}
		}

		if(fabs(dTempDist-dDist) > 0.001 || fabs(dTempAngle - dAngle) > 0.0001 || nSymbol != nTempSymbol || nTempSymbol == 0)
		{
			
			nNum = tempPtArray.length();
			if(nNum < 11)//�����㻡��Ҫ��ֱ�ӱ���
			{
				for(j=0; j<nNum-2; j++)
				{
					resultBlgArray.append(0.0);
					resultPtArray.append(tempPtArray[j]);
				}
				tempPtArray.removeSubArray(0,nNum-3);
			}
			else
			{
				//���㻡�ε�Բ���ĽǶ�
				dArcAngle = (3.14159 - dAngle)*(nNum-2)/2;
				

				//if(dArcAngle > 3.14158) dArcAngle = 3.14;//?����Բ,��ʱ������

				if(dArcAngle < 1.570795)
				{
					dX= sqrt((tempPtArray[0].x-tempPtArray[nNum-1].x)*(tempPtArray[0].x-tempPtArray[nNum-1].x)+
						     (tempPtArray[0].y-tempPtArray[nNum-1].y)*(tempPtArray[0].y-tempPtArray[nNum-1].y))/2;
					dR = dX/sin(dArcAngle);
					dY = dR - dR*cos(dArcAngle);
					dBugle = nSymbol * (dY / dX);
						
				}
				else
				{
					dArcAngle = 3.14159-dArcAngle;
					dX= sqrt((tempPtArray[0].x-tempPtArray[nNum-1].x)*(tempPtArray[0].x-tempPtArray[nNum-1].x)+
						(tempPtArray[0].y-tempPtArray[nNum-1].y)*(tempPtArray[0].y-tempPtArray[nNum-1].y))/2;
					dR = dX/sin(dArcAngle);
					dY = dR*cos(dArcAngle)+dR;
					dBugle = nSymbol * (dY / dX);
				}

				resultPtArray.append(tempPtArray[0]);
				resultBlgArray.append(dBugle);
				tempPtArray.removeSubArray(0,nNum-3);
			}

			dDist = dTempDist;
			dAngle = -1;
			nSymbol = -2;
		}
	}

	nNum = tempPtArray.length();
	if(nNum <10)
	{
		for(j=0; j<nNum; j++)
		{
			resultPtArray.append(tempPtArray[j]);
			resultBlgArray.append(0.0);
		}
	}
	else
	{
		dArcAngle = (3.14159-dAngle)*(nNum-1)/2;

		//if(dArcAngle > 3.14158) dArcAngle = 3.14;//?����Բ,��ʱ������

		if(dArcAngle < 1.570795)
		{
			dX= sqrt((tempPtArray[0].x-tempPtArray[nNum-1].x)*(tempPtArray[0].x-tempPtArray[nNum-1].x)+
				(tempPtArray[0].y-tempPtArray[nNum-1].y)*(tempPtArray[0].y-tempPtArray[nNum-1].y))/2;
			dR = dX/sin(dArcAngle);
			dY = dR - dR*cos(dArcAngle);
			dBugle = nSymbol * (dY / dX);
		}
		else
		{
			dArcAngle = 3.14159-dArcAngle;
			dX= sqrt((tempPtArray[0].x-tempPtArray[nNum-1].x)*(tempPtArray[0].x-tempPtArray[nNum-1].x)+
				(tempPtArray[0].y-tempPtArray[nNum-1].y)*(tempPtArray[0].y-tempPtArray[nNum-1].y))/2;
			dR = dX/sin(dArcAngle);
			dY = dR*cos(dArcAngle)+dR;
			dBugle = nSymbol * (dY / dX);

		}
		resultPtArray.append(tempPtArray[0]);
		resultBlgArray.append(dBugle);

		resultPtArray.append(tempPtArray[nNum-1]);
		resultBlgArray.append(0.0);

		tempPtArray.setLogicalLength(0);
	}
	

	ptArray.setLogicalLength(0);
	ptArray.append(resultPtArray);

	return 1;
	
}

// �����߽ڵ����Ϊ����
void CDistEntityTool::PolylinePtUniteArc(AcDbObjectId objId, int Step)
{
	int i;
	if (Step < 1)
	{
		return;
	}

	//LockDocument _Lock;

	AcDbPolyline* pPline = NULL;

	if (Acad::eOk != acdbOpenObject(pPline, objId, AcDb::kForWrite))
	{
		return;
	}

	int iVertices = pPline->numVerts();

	if (iVertices < Step + 2)
	{
		pPline->close();

		return;
	}

	double * pBulge = new double[iVertices];

	for (i = 0; i < iVertices; i++)
	{
		pBulge[i] = 0;
	}

	for (i = 0; i < iVertices; i++)
	{
		double Bulge;

		pPline->getBulgeAt(i, Bulge);

		if (Bulge < -0.00001 || Bulge > 0.00001)
		{
			continue;
		}

		AcGePoint2d p0, p1, p2;

		pPline->getPointAt(i, p1);

		if (i < Step)
		{
			if (Adesk::kTrue == pPline->isClosed())
			{
				pPline->getPointAt(iVertices + i - Step, p0);
			}
			else
			{
				continue;
			}
		}
		else
		{
			pPline->getPointAt(i - Step, p0);
		}

		if (i > iVertices - Step)
		{
			if (Adesk::kTrue == pPline->isClosed())
			{
				pPline->getPointAt(i + Step - iVertices, p2);
			}
			else
			{
				continue;
			}
		}
		else
		{
			pPline->getPointAt(i + Step, p2);
		}

		double dTmp1 = p0.distanceTo(p1);

		double dTmp2 = p1.distanceTo(p2);

		if (dTmp1 - dTmp2 > 0.00001 ||
			dTmp1 - dTmp2 < -0.00001)
		{
			continue;
		}

		dTmp2 = p0.distanceTo(p2) / 2;

		double AsBulge = sqrt( dTmp1 * dTmp1 - dTmp2 * dTmp2 ) / dTmp2;

		if (acutAngle((double*)&p0,(double*)&p1) > acutAngle((double*)&p0,(double*)&p2))
		{
			AsBulge *= -1;

			if ((p1.x - p0.x) > 0 && (p2.x - p0.x) > 0 &&
				(p1.y - p0.y) * (p2.y - p0.y) < 0)
			{
				AsBulge *= -1;
			}
		}

		if (AsBulge < -0.0001 || AsBulge > 0.0001)
		{
			int iTmp;

			if (i < Step)
			{
				iTmp = iVertices - i - Step;
			}
			else
			{
				iTmp = i - Step;
			}

			pBulge[iTmp] = AsBulge;
		}
	}

	int Pos = iVertices - 1;

	double Bulge = pBulge[Pos - 1];

	for (i = iVertices - 2; i >= -1; i--)
	{
		double BlgTmp;
		if (i == -1)
		{
			BlgTmp = pBulge[iVertices - 1];
		}
		else
		{
			BlgTmp = pBulge[i];
		}

		if (BlgTmp - Bulge < 0.001 &&
			BlgTmp - Bulge > -0.001)// Bulge�仯
		{
			continue;
		}

		if (Bulge > 0.00001 ||
			Bulge < -0.00001)// Bulge��Ϊ��
		{
			// �����ڵ��Bulge
			double angle = atan(Bulge);

			angle *= (Pos - i) / (Step + 1);

			Bulge = tan(angle);

			if (Bulge > 0.00001 ||
				Bulge < -0.00001)// Bulge��Ϊ��
			{
				pPline->setBulgeAt(i + 1, Bulge);

				for (int j = Pos; j >= i + 2; j--)
				{
					pPline->removeVertexAt(j);
				}
			}
		}

		Pos = i + 1;

		Bulge = BlgTmp;
	}

	delete pBulge;

	pPline->close();
}

void CDistEntityTool::OpenDocHelper(void *pData)
{
	if(acDocManager->isApplicationContext())
	{
		CString* dwg = (CString*)pData;
		Acad::ErrorStatus result = acDocManager->appContextOpenDocument(*dwg);
		if(result != Acad::eOk)
			acutPrintf("\nERROR: %s\n", acadErrorStatusText(result));
	}
	else
		acutPrintf("\nERROR: in Document context :%s\n",acDocManager->curDocument()->fileName());
}

void CDistEntityTool::OpenDwgFile(CString strFileName)
{
	if(acDocManager->isApplicationContext())
	{
		Acad::ErrorStatus result = acDocManager->appContextOpenDocument(strFileName);
		if(result != Acad::eOk)
			acutPrintf("\nERROR: %s\n", acadErrorStatusText(result));
	}
	else
		acDocManager->executeInApplicationContext(OpenDocHelper, &strFileName);
}

void CDistEntityTool::SendCommandToAutoCAD(CString cmd)
{
	struct resbuf * rb_cmdecho_Off = acutBuildList(RTSHORT,0,0);
	struct resbuf * rb_cmdecho_On = acutBuildList(RTSHORT,1,0);
	acedSetVar("cmdecho", rb_cmdecho_Off);

	acedGetAcadFrame()->SetActiveWindow();

	CWnd *pWnd = acedGetAcadDockCmdLine();

	pWnd->SendMessage(WM_CHAR,VK_ESCAPE,0);
	pWnd->SendMessage(WM_CHAR,VK_ESCAPE,0);
	acedSetVar("cmdecho", rb_cmdecho_On);

	int len = cmd.GetLength();
	acedSetVar("cmdecho", rb_cmdecho_Off);

	for (int i=0; i < len ; i++)
	{
		pWnd->SendMessage(WM_CHAR, cmd.GetAt(i), 0);
	}

	acedSetVar("cmdecho", rb_cmdecho_On);
	acutRelRb(rb_cmdecho_Off);
	acutRelRb(rb_cmdecho_On);
}


void CDistEntityTool::ShowCommandLineWindow(BOOL bShow /*= TRUE*/)
{
	CWnd *pWnd = NULL;
	pWnd = acedGetAcadDockCmdLine();
	if(NULL == pWnd) return;
	pWnd = pWnd->GetParent();
	if(NULL == pWnd) return;
	pWnd = pWnd->GetParent();
	if(NULL == pWnd) return;
	
	if (bShow)
	{
		pWnd->ModifyStyle(0,WS_VISIBLE|WS_OVERLAPPED);
		pWnd->ShowWindow(SW_SHOWNORMAL);//SW_SHOW
	}
	else if (!bShow)
	{
		pWnd->ModifyStyle(WS_VISIBLE|WS_OVERLAPPED,0);
		pWnd->ShowWindow(SW_HIDE);
	}
	pWnd->RedrawWindow();
	acedGetAcadFrame()->RecalcLayout();
}

BOOL CDistEntityTool::OpenOrCloseLayer(BOOL bOpen, CStringArray &strCadLyNameArray)
{
	AcDbHostApplicationServices* pApp = NULL;
	AcDbDatabase* pDb = NULL;
	AcDbLayerTable* pLayerTable = NULL;
	AcDbObjectId layerId;
	Acad::ErrorStatus es;

	pApp = acdbHostApplicationServices();
	if (pApp)
	{
		pDb = pApp->workingDatabase();
	}

	if (pDb == NULL)
	{
		return FALSE;
	}

	es = pDb->getLayerTable(pLayerTable, AcDb::kForRead);

	if (es != Acad::eOk)
	{
		return FALSE;
	}

	AcDbLayerTableRecord *pLayerTableRecord = NULL;

	for (int i = 0; i < strCadLyNameArray.GetSize(); i++)
	{
		CString sTemp = strCadLyNameArray.GetAt(i);

		if (pLayerTable->getAt(sTemp, pLayerTableRecord, AcDb::kForWrite) == Acad::eOk)
		{
			if(bOpen)
				pLayerTableRecord->setIsOff(0);
			else
				pLayerTableRecord->setIsOff(1);

			pLayerTableRecord->close();
		}
	}
	pLayerTable->close();

	acedRedraw(NULL,0);
	return TRUE;
}


BOOL CDistEntityTool::InsertBlkAttr(char* strBlkName,CStringArray& strAttrArray,char* strTitleName,int nColorIndex)
{
	int nCount = strAttrArray.GetCount();
	if(nCount == 0) return FALSE;

	// ��õ�ǰͼ�����ݿ�Ŀ��
	AcDbBlockTable *pBlkTbl=NULL;
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	if(Acad::eOk != pDB->getBlockTable(pBlkTbl, AcDb::kForWrite)) return FALSE;

	if(pBlkTbl->has(strBlkName))
	{
		pBlkTbl->close(); return TRUE;
	}

	// �����µĿ���¼
	AcDbBlockTableRecord *pBlkTblRcd = NULL;
	pBlkTblRcd = new AcDbBlockTableRecord();
	if(NULL == pBlkTblRcd)
	{
		pBlkTbl->close();
		return FALSE;
	}	
	pBlkTblRcd->setName(strBlkName);// ���ÿ���¼������

	// ������¼��ӵ������
	AcDbObjectId blkDefId;
	pBlkTbl->add(blkDefId, pBlkTblRcd);
	pBlkTbl->close();

	// �����¼�����ʵ��
	AcGePoint3d pt(0,0,0);
	AcDbPoint *pPoint = new AcDbPoint(pt);
	AcDbObjectId entId;
	pBlkTblRcd->appendAcDbEntity(entId, pPoint);
	pPoint->close();
	// ����һ������ ����ֱ��
	for(int i=0; i<nCount; i++)
	{
		AcDbAttributeDefinition *pAttDef = new AcDbAttributeDefinition;
		pAttDef->setPosition(pt);
		pAttDef->setHeight(16);
		pAttDef->setRotation(0);
		pAttDef->setHorizontalMode(AcDb::kTextLeft);
		pAttDef->setVerticalMode(AcDb::kTextBase);
		pAttDef->setPrompt(strAttrArray[i]);
		pAttDef->setTextString(" ");
		pAttDef->setTag(strAttrArray[i]);

		CString strTemp = strAttrArray[i];
		if(strTemp.CompareNoCase(strTitleName)==0)
		{
			pAttDef->setTextString(strTitleName);
			pAttDef->setInvisible(Adesk::kFalse);
			pAttDef->setColorIndex(nColorIndex);
		}
		else
			pAttDef->setInvisible(Adesk::kTrue);
		pAttDef->setVerifiable(Adesk::kFalse);
		pAttDef->setPreset(Adesk::kFalse);
		pAttDef->setConstant(Adesk::kFalse);
		pAttDef->setFieldLength(60);
		pBlkTblRcd->appendAcDbEntity(entId, pAttDef);
		pAttDef->close();
	}

	// �ر�ʵ��Ϳ���¼
	pBlkTblRcd->close();

	return TRUE;
}


/*

BOOL CDistEntityTool::CreateBlkRefAttr(AcDbObjectId& resultId,AcGePoint3d basePt,char* strBlkName,char* strLyName,
									   int  nClrIndex,char *strScale,double dAngle,double dFontHeight,
									   CStringArray& strTagArray,CStringArray& strTextArray,AcDbDatabase* pDB)
{
	resultId.setNull();

	//1.ȡ���ָ��
	AcDbBlockTable *pBlockTable = NULL;
	if(Acad::eOk != pDB->getSymbolTable(pBlockTable,AcDb::kForRead))
	{
		return FALSE;
	}

	//2.���ݿ������Ҫ����鶨���IDֵ
	AcDbObjectId blockId;
	if(Acad::eOk!=pBlockTable->getAt(strBlkName,blockId,AcDb::kForRead)) 
	{
		pBlockTable->close(); return FALSE;
	}

	//3.����飨�����ʵ���ǲ��������ã�
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;
	pBlkRef->setBlockTableRecord(blockId);//ָ�������õ�ͼ����¼�Ķ���ID

	//����ͼ��Ҫ�����û����������������ת��
	resbuf to,from;
	from.restype=RTSHORT;
	from.resval.rint=1;  //UCS
	to.restype=RTSHORT;
	to.resval.rint=0;    //WCS
	AcGeVector3d normal(0.0,0.0,1.0);
	acedTrans(&(normal.x),&from,&to,Adesk::kTrue,&(normal.x));//ת������

	//���ÿ����õĻ�������
	pBlkRef->setRotation(dAngle);//��ת�Ƕ�
	pBlkRef->setPosition(basePt); //ָ������
	pBlkRef->setNormal(normal);
	pBlkRef->setLayer(strLyName);//ͼ������
	pBlkRef->setColorIndex(nClrIndex);


	//���÷Ŵ�������ֽ������Ϣ��
	double dScale[3]={1.0,1.0,1.0};
	int nLen = strlen(strScale);
	char cValue[20];
	int nIndex =0,nPos=0;
	memset(cValue,0,sizeof(char)*20);
	for(int i=0; i<nLen; i++)
	{
		if(strScale[i] == '#' && nPos > 0)
		{
			dScale[nIndex] = atof(cValue);
			nPos = 0;  nIndex++; memset(cValue,0,sizeof(char)*20); continue;
		}
		cValue[nPos] = strScale[i]; nPos++;
	}
	AcGeScale3d dGeScale(dScale[0],dScale[1],dScale[2]);
	pBlkRef->setScaleFactors(dGeScale);//�Ŵ����

	//����������ӵ�ģ�Ϳռ�
	AcDbBlockTableRecord *pBlockTableRecord=NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();
	pBlockTableRecord->appendAcDbEntity(resultId,pBlkRef);
	pBlockTableRecord->close();

	// �ж�ָ���Ŀ���¼�Ƿ�������Զ���
	AcDbBlockTableRecord *pBlkDefRcd=NULL;
	if(Acad::eOk != acdbOpenObject(pBlkDefRcd, blockId, AcDb::kForRead))
	{
		pBlkRef->close();
		pBlockTableRecord->close();
		return FALSE;
	}

	if(pBlkDefRcd->hasAttributeDefinitions()==false)
	{
		pBlkRef->close();
		pBlkDefRcd->close();
		pBlockTableRecord->close();
		return TRUE;
	}

	//��������Բ���
	AcDbBlockTableRecordIterator *pItr=NULL;
	pBlkDefRcd->newIterator(pItr);
	AcDbEntity *pEnt=NULL;	
	for (pItr->start(); !pItr->done(); pItr->step())//�����������е���������
	{
		if(Acad::eOk != pItr->getEntity(pEnt, AcDb::kForRead)) continue;
		AcDbAttributeDefinition *pAttDef = NULL;
		pAttDef = (AcDbAttributeDefinition*)pEnt;
		if(pAttDef == NULL){
			pEnt->close(); continue;
		}

		// ����һ���µ����Զ���
		AcDbAttribute *pAttNew = new AcDbAttribute();
		pAttNew->setPropertiesFrom(pAttDef);   // �����Զ��������Զ���Ķ�������

		// �������Զ������������
		pAttNew->setInvisible(pAttDef->isInvisible());
		AcGePoint3d ptBase = pAttDef->position(); ptBase += pBlkRef->position().asVector(); pAttNew->setPosition(ptBase);
		pAttNew->setHeight(pAttDef->height());
		pAttNew->setRotation(pAttDef->rotation());

		// ������Զ����Tag��Prompt��TextString
		CString strTempTag;
		char *pStr = NULL;
		pStr = pAttDef->tag(); pAttNew->setTag(pStr);  strTempTag = pStr; free(pStr);
		pAttNew->setFieldLength(60);

		pAttNew->setHeight(pAttDef->height());
		pAttNew->setTextString(" ");
		for(int k=0; k<strTagArray.GetCount();k++)
		{
			if(strTempTag.CompareNoCase(strTagArray.GetAt(k))==0)
			{
				CString strTempValue = strTextArray.GetAt(k);
				pAttNew->setTextString(strTempValue.GetBuffer(0));
				break;
			}
		}
		pAttNew->setLayer(strLyName);

		// ������׷�����Զ���
		pBlkRef->appendAttribute(pAttNew);
		pAttNew->close();
		pEnt->close();
	}
	delete pItr;

	// �ر����ݿ�Ķ���
	pBlkRef->close();
	pBlkDefRcd->close();
	pBlockTableRecord->close();

	return 1;


}
*/



BOOL CDistEntityTool::InsertBlkAttrRef(AcDbObjectId& resultId,AcGePoint3d pt,char* strBlkName,char* strCadLyName,
									   double dFontHeight,CStringArray& strTagArray,CStringArray& strTextArray)
{
	// ��õ�ǰ���ݿ�Ŀ��
	AcDbBlockTable *pBlkTbl;
	resultId.setNull();
	AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
	if(Acad::eOk != pDB->getBlockTable(pBlkTbl, AcDb::kForWrite)) return FALSE;

	// �����û�ָ���Ŀ鶨���Ƿ����
	if (!pBlkTbl->has(strBlkName))
	{
		acutPrintf("\n��ǰͼ����δ����ָ�����ƵĿ鶨�壡");
		pBlkTbl->close();
		return FALSE;
	}

	// ����û�ָ���Ŀ���¼
	AcDbObjectId blkDefId;
	pBlkTbl->getAt(strBlkName, blkDefId);

	// ��������ն���
	CDistEntityTool tempTool;
	tempTool.CreateLayer(resultId,strCadLyName);
	AcDbBlockReference *pBlkRef = new AcDbBlockReference(pt,blkDefId);
	pBlkRef->setLayer(strCadLyName);
	AcDbBlockTableRecord *pBlkTblRcd=NULL;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd,AcDb::kForWrite);
	pBlkTbl->close();
	resultId.setNull();
	if(Acad::eOk != pBlkTblRcd->appendAcDbEntity(resultId, pBlkRef))
	{
		delete pBlkRef;
		return FALSE;  // ���������ӵ�ģ�Ϳռ�
	}

	// �ж�ָ���Ŀ���¼�Ƿ�������Զ���
	AcDbBlockTableRecord *pBlkDefRcd;
	acdbOpenObject(pBlkDefRcd, blkDefId, AcDb::kForRead);
	if (pBlkDefRcd->hasAttributeDefinitions())
	{
		AcDbBlockTableRecordIterator *pItr;
		pBlkDefRcd->newIterator(pItr);
		AcDbEntity *pEnt;
		for (pItr->start(); !pItr->done(); pItr->step())
		{
			pItr->getEntity(pEnt, AcDb::kForRead);

			// ����Ƿ������Զ���
			if(pEnt->isKindOf(AcDbAttributeDefinition::desc()))
			{
				// ȡ��ʵ��
				AcDbAttributeDefinition *pAttDef = AcDbAttributeDefinition::cast(pEnt);

				// ����һ���µ����Զ���
				AcDbAttribute *pAttNew = new AcDbAttribute();
				pAttNew->setPropertiesFrom(pAttDef);   // �����Զ��������Զ���Ķ�������

				// �������Զ������������
				pAttNew->setInvisible(pAttDef->isInvisible());
				AcGePoint3d ptBase = pAttDef->position();
				ptBase += pBlkRef->position().asVector();
				pAttNew->setPosition(ptBase);
				pAttNew->setHeight(pAttDef->height());
				pAttNew->setRotation(pAttDef->rotation());

				// ������Զ����Tag��Prompt��TextString
				CString strTempTag;
				char *pStr = NULL;
				pStr = pAttDef->tag(); pAttNew->setTag(pStr);  strTempTag = pStr; free(pStr);
				pAttNew->setFieldLength(60);

				pAttNew->setHeight(dFontHeight);
				pAttNew->setTextString(" ");
				for(int k=0; k<strTagArray.GetCount();k++)
				{
					if(strTempTag.CompareNoCase(strTagArray.GetAt(k))==0)
					{
						CString strTempValue = strTextArray.GetAt(k);
						pAttNew->setTextString(strTempValue.GetBuffer(0));
						break;
					}
				}
				pAttNew->setLayer(strCadLyName);

				// ������׷�����Զ���
				pBlkRef->appendAttribute(pAttNew);
				pAttNew->close();
			}
			pEnt->close();
		}
		delete pItr;
	}
	// �ر����ݿ�Ķ���
	pBlkRef->close();
	pBlkTblRcd->close();
	pBlkDefRcd->close();

	return TRUE;
}


AcDbViewTableRecord CDistEntityTool::GetCurrentView()
{
	AcDbViewTableRecord view;
	struct resbuf rb;
	struct resbuf wcs, ucs, dcs; 
	wcs.restype = RTSHORT;
	wcs.resval.rint = 0;
	ucs.restype = RTSHORT;
	ucs.resval.rint = 1;
	dcs.restype = RTSHORT;
	dcs.resval.rint = 2;

	acedGetVar("VIEWMODE", &rb);
	view.setPerspectiveEnabled(rb.resval.rint & 1);
	view.setFrontClipEnabled(rb.resval.rint & 2);
	view.setBackClipEnabled(rb.resval.rint & 4);
	view.setFrontClipAtEye(!(rb.resval.rint & 16));

	acedGetVar("VIEWCTR", &rb);
	acedTrans(rb.resval.rpoint, &ucs, &dcs, 0, rb.resval.rpoint);
	view.setCenterPoint(AcGePoint2d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y]));

	acedGetVar("LENSLENGTH", &rb);
	view.setLensLength(rb.resval.rreal);

	acedGetVar("TARGET", &rb);
	acedTrans(rb.resval.rpoint, &ucs, &wcs, 0, rb.resval.rpoint);
	view.setTarget(AcGePoint3d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y], rb.resval.rpoint[Z]));

	acedGetVar("VIEWDIR", &rb);
	acedTrans(rb.resval.rpoint, &ucs, &wcs, 1, rb.resval.rpoint);
	view.setViewDirection(AcGeVector3d(rb.resval.rpoint[X],
	rb.resval.rpoint[Y], rb.resval.rpoint[Z]));

	acedGetVar("VIEWSIZE", &rb);
	view.setHeight(rb.resval.rreal);
	double height = rb.resval.rreal;

	acedGetVar("SCREENSIZE", &rb);
	view.setWidth(rb.resval.rpoint[X] / rb.resval.rpoint[Y] * height);

	acedGetVar("VIEWTWIST", &rb);
	view.setViewTwist(rb.resval.rreal);

	acedGetVar("TILEMODE", &rb);
	int tileMode = rb.resval.rint;

	acedGetVar("CVPORT", &rb);
	int cvport = rb.resval.rint;

	bool paperspace = ((tileMode == 0) && (cvport == 1)) ? true : false;
	view.setIsPaperspaceView(paperspace);
	if (!paperspace)
	{
		acedGetVar("FRONTZ", &rb);
		view.setFrontClipDistance(rb.resval.rreal);
		acedGetVar("BACKZ", &rb);
		view.setBackClipDistance(rb.resval.rreal);
	}
	else
	{
		view.setFrontClipDistance(0.0);
		view.setBackClipDistance(0.0);
	}

	return view;
}


//���ܣ�ȡ������ͼ��
//������layerArray  ͼ������
//���أ�es 
long  CDistEntityTool::GetAllLayerName(CStringArray& layerArray,CStringArray& clrIndexArray,AcDbDatabase* pDB)
{
	Acad::ErrorStatus es=Acad::eOk;

	layerArray.RemoveAll(); clrIndexArray.RemoveAll();

	AcDbLayerTable *pLayerTable=NULL;
	if((es=pDB->getSymbolTable(pLayerTable,AcDb::kForRead))!=Acad::eOk)
	{
		pLayerTable->close();
		return 0;
	}

	//����һ����������
	AcDbLayerTableIterator *pLayerTableIterator;
	if ( (es =pLayerTable->newIterator (pLayerTableIterator) ) != Acad::eOk ) 
	{
		//acutPrintf ("\n���ܴ����������") ;
		pLayerTable->close () ;
		return 0;
	}
	pLayerTable->close () ;

	//ȡ��ͼ��
	char *pLayerName=NULL;
	CString strname,strClr;
	for(pLayerTableIterator->start ();!pLayerTableIterator->done();pLayerTableIterator->step())
	{
		//ͼ����
		AcDbLayerTableRecord *pLayerTableRecord=NULL;
		pLayerTableIterator->getRecord(pLayerTableRecord,AcDb::kForRead);
		pLayerTableRecord->getName(pLayerName);
		strname.Format("%s",pLayerName);
		pLayerTableRecord->close();
		layerArray.Add(strname);

		//��ɫ
		AcCmColor clr = pLayerTableRecord->color();
		strClr.Format("%d",clr.colorIndex());
		clrIndexArray.Add(strClr);
	}

	if(pLayerName) 
		acutDelString(pLayerName);
	delete pLayerTableIterator;
	pLayerTableIterator=NULL;

	return 1;
}


/*
Acad::ErrorStatus    selectEntityInLayer(
CString nLayerName,
AcDbObjectIdArray& nIDs,
bool nModelSpace
)
{
Acad::ErrorStatus es=Acad::eOk; 

AcDbBlockTable*        pBlockTable=NULL;
AcDbBlockTableRecord*  pSpaceRecord=NULL;
if (acdbHostApplicationServices()->workingDatabase()==NULL)
return Acad::eNoDatabase;
if ((es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead))==Acad::eOk)
{
char entryName[13];
if (nModelSpace)
strcpy(entryName,ACDB_MODEL_SPACE);
else
strcpy(entryName,ACDB_PAPER_SPACE);
//Get the Model or Paper Space record and open it for read:  
if ((es = pBlockTable->getAt((const char*)entryName, pSpaceRecord, AcDb::kForRead))==Acad::eOk)
{
AcDbBlockTableRecordIterator* pIter;
if (pSpaceRecord->newIterator(pIter)==Acad::eOk)
{
for (pIter->start();!pIter->done();pIter->step())
{
char *name=NULL;
AcDbEntity* pEntity;
if (pIter->getEntity(pEntity,AcDb::kForRead)==Acad::eOk)
{
name=pEntity->layer();
if (nLayerName.CompareNoCase(name)==0)
nIDs.append(pEntity->objectId());

pEntity->close();
acutDelString(name); 
}
}
delete pIter;
}            
pSpaceRecord->close();
}   
pBlockTable->close();
}

return es;
}






//ȡ��ѡ�񼯰�Χ��
BOOL GetSSExtents(ads_name ss, ads_point &maxpt, ads_point &minpt)
{
long len;
int ret=ads_sslength(ss,&len);
if(ret!=RTNORM||len<1) return FALSE;
AcDbObjectId objId;
ads_name ename;
AcDbEntity *pEnt;
AcDbExtents extent;
AcGePoint3d pt1,pt2;
ads_ssname (ss, 0, ename);
acdbGetObjectId(objId,ename);
acdbOpenObject(pEnt,objId,AcDb::kForRead);
pEnt->getGeomExtents(extent);
pt1=extent.maxPoint();
pt2=extent.minPoint();
pEnt->close();
for(int i=0;igetGeomExtents(extent);
if(pt1[X]extent.minPoint()[X]||pt2[Y]>extent.minPoint()[Y])
pt2=extent.minPoint();
pEnt->close();
}
maxpt[X]=pt1[X];
maxpt[Y]=pt1[Y];
maxpt[Z]=pt1[Z];

minpt[X]=pt2[X];
minpt[Y]=pt2[Y];
minpt[Z]=pt2[Z];

return TRUE;

}



double CDrawFunction::getTextLength(AcDbObjectId textId)
{
Acad::ErrorStatus es=Acad::eOk;
AcDbEntity *pEnt=NULL;
if((es=acdbOpenObject(pEnt,textId,AcDb::kForRead))!=Acad::eOk)
return -1;
AcDbExtents Ext;
pEnt->getGeomExtents(Ext);
pEnt->close();
AcGePoint3d minPt,maxPt;
minPt=Ext.minPoint();
maxPt=Ext.maxPoint();
return acutDistance(asDblArray(minPt),asDblArray(maxPt));
}







�жϵ��Ƿ���Բ����

BOOL
IsAtArc(CAD_POINT firstPt,CAD_POINT secondPt,
		double radius,double direct,int More,CAD_POINT thePt)
		    // �ж�ĳ���Ƿ���Բ����
			//firstPt Բ�����
			//secondPt Բ���յ� 
			//radius �뾶
			//direct ƫ��( >=0__��ƫ <0__��ƫ )
			//More (More<0__СԲ��,More>0__��Բ��)
			//thePt �жϵ�
			//���أ�TRUE__��Բ���� FALSE__����Բ����
			
{
	CAD_POINT centerPt,sectionPt;
	CAD_POINT startPt,endPt;
	double startAngle,endAngle,chordAngle,vertAngle;
	double intLine,chordLine;
	    //centerPt Բ��Բ��
		//sectionPt �������ĵ�
		//startAngle Բ��������߽Ƕȣ����ȣ�
		//endAngle Բ���յ����߽Ƕȣ����ȣ�
		//chordAngle ���߽Ƕȣ����ȣ�
		//vertAngle �����ߴ�ֱ�Ĵ��߽Ƕȣ����ȣ�
		//intLine ����������Բ�ľ���
		//chordLine ���߳���
	
	sectionPt.x = (firstPt.x + secondPt.x)/2;
	sectionPt.y = (firstPt.y + secondPt.y)/2;

	chordLine = sqrt( pow( (secondPt.x-firstPt.x),2 ) + pow( (secondPt.y-firstPt.y),2 ) );
	intLine = sqrt((radius*radius-chordLine*chordLine/4) );

	chordAngle = ads_angle(asDblArray(firstPt),asDblArray(secondPt)); //���ߵĽǶ�

	if(direct>=0)//��ƫ
	{
		startPt=firstPt;
		endPt=secondPt;
		vertAngle=chordAngle+Pai/2;
	}
	else if(direct<0)//��ƫ
	{
		startPt=secondPt;
		endPt=firstPt;
		vertAngle=chordAngle-Pai/2;
	}

	if(More<=0)//СԲ��
	{
		centerPt.x=sectionPt.x+intLine*cos(vertAngle);
		centerPt.y=sectionPt.y+intLine*sin(vertAngle);
	}
	else//��Բ��
	{
		centerPt.x=sectionPt.x-intLine*cos(vertAngle);
		centerPt.y=sectionPt.y-intLine*sin(vertAngle);
	}

	if(fabs(centerPt.distanceTo(thePt)-radius)>1.0E-8)
		return FALSE;
	startAngle = ads_angle(asDblArray(centerPt),asDblArray(startPt)); 
	endAngle = ads_angle(asDblArray(centerPt),asDblArray(endPt)); 

	AcDbArc *pArc=new AcDbArc(centerPt,radius,startAngle,endAngle);
	AcDbLine *pLine=new AcDbLine(centerPt,thePt);
	AcGePoint3dArray Points;
	pLine->intersectWith(pArc,AcDb::kOnBothOperands,Points);
	if(Points.isEmpty()) return FALSE;

	return TRUE;
}



*/

BOOL CDistEntityTool::CreateBlkRefAttr(AcDbObjectId& resultId,AcGePoint3d basePt,char* strBlkName,
					                   char* strLyName,int  nClrIndex,char *strScale,double dAngle,
					                   CStringArray& strTagArray,CStringArray& strTextArray,AcDbDatabase* pDB)
{
	resultId.setNull();

	// ��õ�ǰ���ݿ�Ŀ��
	AcDbBlockTable *pBlkTbl=NULL;
	if(Acad::eOk != pDB->getBlockTable(pBlkTbl, AcDb::kForWrite)) return FALSE;

	// �����û�ָ���Ŀ鶨���Ƿ����
	AcDbObjectId blkDefId;
	if(Acad::eOk!=pBlkTbl->getAt(strBlkName,blkDefId,AcDb::kForRead)) 
	{
		CString strDwgFName;
		strDwgFName.Format("%sBlocks\\%s.dwg",GetCurArxAppPath("DistClipPlot.arx"),strBlkName);
		blkDefId = CreateBlockByDwgFile(strDwgFName);

		if(blkDefId.isNull()) 
		{
			pBlkTbl->close(); return FALSE;
		}
	}

	// ��������ն���
	AcDbBlockReference *pBlkRef = new AcDbBlockReference(basePt,blkDefId);

	//����ͼ��Ҫ�����û����������������ת��
	resbuf to,from;
	from.restype=RTSHORT;
	from.resval.rint=1;  //UCS
	to.restype=RTSHORT;
	to.resval.rint=0;    //WCS
	AcGeVector3d normal(0.0,0.0,1.0);
	acedTrans(&(normal.x),&from,&to,Adesk::kTrue,&(normal.x));//ת������

	//���ÿ����õĻ�������
	pBlkRef->setRotation(dAngle);//��ת�Ƕ�
	pBlkRef->setPosition(basePt); //ָ������
	pBlkRef->setNormal(normal);
	pBlkRef->setLayer(strLyName);//ͼ������
	pBlkRef->setColorIndex(nClrIndex);


	//���÷Ŵ�������ֽ������Ϣ��
	double dScale[3]={1.0,1.0,1.0};
	int nLen = strlen(strScale);
	char cValue[20];
	int nIndex =0,nPos=0;
	memset(cValue,0,sizeof(char)*20);
	for(int i=0; i<nLen; i++)
	{
		if(strScale[i] == '#' && nPos > 0)
		{
			dScale[nIndex] = atof(cValue);
			nPos = 0;  nIndex++; memset(cValue,0,sizeof(char)*20); continue;
		}
		cValue[nPos] = strScale[i]; nPos++;
	}
	AcGeScale3d dGeScale(dScale[0],dScale[1],dScale[2]);
	pBlkRef->setScaleFactors(dGeScale);//�Ŵ����


	// ���������ӵ�ģ�Ϳռ�
	AcDbBlockTableRecord *pBlkTblRcd=NULL;
	if(Acad::eOk != pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd,AcDb::kForWrite))
	{
		pBlkTbl->close(); return FALSE;
	}
	pBlkTbl->close();

	pBlkTblRcd->appendAcDbEntity(resultId, pBlkRef);

	


	// �ж�ָ���Ŀ���¼�Ƿ�������Զ���
	AcDbBlockTableRecord *pBlkDefRcd=NULL;
	acdbOpenObject(pBlkDefRcd, blkDefId, AcDb::kForRead);
	if (pBlkDefRcd->hasAttributeDefinitions())
	{
		AcDbBlockTableRecordIterator *pItr=NULL;
		pBlkDefRcd->newIterator(pItr);
		AcDbEntity *pEnt=NULL;
		for (pItr->start(); !pItr->done(); pItr->step())
		{
			pItr->getEntity(pEnt, AcDb::kForRead);

			// ����Ƿ������Զ���
			AcDbAttributeDefinition *pAttDef=NULL;
			pAttDef = AcDbAttributeDefinition::cast(pEnt);
			if (pAttDef != NULL)
			{
				// ����һ���µ����Զ���
				AcDbAttribute *pAtt = new AcDbAttribute();

				// �����Զ��������Զ���Ķ�������
				pAtt->setPropertiesFrom(pAttDef);

				// �������Զ������������
				pAtt->setInvisible(pAttDef->isInvisible());
				AcGePoint3d ptBase = pAttDef->position();
				ptBase.x = ptBase.x * dScale[0]; 
				ptBase.y = ptBase.y * dScale[1];

				//AcGePoint3d ptV = pBlkRef->position().asVector(); //ptV.x = ptV.x * dScale[0]; ptV.y = ptV.y*dScale[1];
				ptBase += pBlkRef->position().asVector();

				pAtt->setPosition(ptBase);
				pAtt->setHeight(pAttDef->height());
				pAtt->setRotation(pAttDef->rotation());
				pAtt->setLayer(strLyName);

				// ������Զ����Tag��Prompt��TextString
				CString strTempTag;
				char *pStr = NULL;
				pStr = pAttDef->tag(); 
				pAtt->setTag(pStr);  
				strTempTag = pStr; free(pStr);

				pAtt->setFieldLength(60);

				pAtt->setHeight(pAttDef->height()*dScale[1]);
				pAtt->setTextString(" ");
				for(int k=0; k<strTagArray.GetCount();k++)
				{
					if(strTempTag.CompareNoCase(strTagArray.GetAt(k))==0)
					{
						CString strTempValue = strTextArray.GetAt(k);
						pAtt->setTextString(strTempValue.GetBuffer(0));
						break;
					}
				}

				// ������׷�����Զ���
				pBlkRef->appendAttribute(pAtt);
				pAtt->close();
			}
			pEnt->close();
		}
		delete pItr;
	}
	// �ر����ݿ�Ķ���
	pBlkRef->close();
	pBlkTblRcd->close();
	pBlkDefRcd->close();

	return TRUE;
}


AcDbObjectId CDistEntityTool::CreateBlockByDwgFile(CString strDwgFileName)//��·�����ļ���(����Ϊ�ļ���������·��)
{
	int nLen = strDwgFileName.GetLength();
	int nPosA=-1,nPosB=-1;
	for(int i=nLen-1; i>=0; i--)
	{
		char ch = strDwgFileName.GetAt(i);
		if(ch == '.' && nPosB<0)
		{
			nPosB = i;
		}
		if(ch == '\\' && nPosA < 0)
		{
			nPosA = i+1;
		}
		if(nPosA>0 && nPosB >0) break;
	}

	CString strBlkName = strDwgFileName.Mid(nPosA,nPosB-nPosA);

	AcDbObjectId ObjId;
	ObjId.setNull();
	AcDbBlockTable *pTb=NULL;

	acdbCurDwg()->getBlockTable(pTb,AcDb::kForRead);
	if(pTb->getAt(strBlkName,ObjId)==Acad::eOk)
	{
		pTb->close(); return ObjId;
	}
	pTb->close();

	AcDbDatabase *pDb=new AcDbDatabase(Adesk::kFalse);

	if(pDb->readDwgFile(strDwgFileName)!=Acad::eOk)
	{
		delete pDb; return NULL;
	}
	acdbCurDwg()->insert(ObjId,strBlkName,pDb);

	delete pDb;
	return ObjId;
} 




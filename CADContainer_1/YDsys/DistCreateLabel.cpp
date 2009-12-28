#include "StdAfx.h"
#include "DistCreateLabel.h"
#include "DistBasePara.h"
#include "DistEntityTool.h"
#include "DistSelSet.h"


//////////////////////////////////////////////////////////////////////////
CSpcoordLabel::CSpcoordLabel(void)
{
	FontHeight = 5;
	// ��ȡ��׽����
	m_OldRbf = acutNewRb(RTREAL);
	acedGetVar("osmode", m_OldRbf);

	// �����²�׽�������㲶׽��
	struct resbuf *NewRbf = acutBuildList(RTSHORT, 9, 0);
	acedSetVar("osmode", NewRbf);
	acutRelRb(NewRbf);
}

CSpcoordLabel::~CSpcoordLabel(void)
{
	// ��ԭ��׽����
	acedSetVar("osmode", m_OldRbf);
	acutRelRb(m_OldRbf);
}


BOOL CSpcoordLabel::CreateSpcoordLabel(CString strCadLyName,ads_point SetPt /*= NULL*/, BOOL bInPut /*= FALSE*/)
{
	//�õ��������
	ads_point p1, p2 ,p3;
	if (bInPut == TRUE)
	{
		p1[0] = SetPt[0]; p1[1] = SetPt[1]; p1[2] = SetPt[2];
	}
	else
	{
		if (acedGetPoint(NULL, "\n��ѡ����Ҫ��ע�ĵ�", p1) != RTNORM)
		{
			return FALSE;
		}
	}
	if (acedGetPoint(p1, "\n��ѡ�������߶˵�", p2) != RTNORM)
	{
		return FALSE;
	}

	//������
	AcDbBlockTable *pBlockTable = NULL;
	acdbCurDwg()->getBlockTable(pBlockTable, AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord = NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	pBlockTable->close();

	AcDbObjectId TextID,ObjID;
	AcDbLine *pLine = NULL;
	AcGePoint3d startPt;
	AcGePoint3d endPt;

	startPt.x = p1[0]; startPt.y = p1[1]; startPt.z = p1[2];
	endPt.x = p2[0]; endPt.y = p2[1]; endPt.z = p2[2];
	pLine = new AcDbLine(startPt, endPt);
	pBlockTableRecord->appendAcDbEntity(ObjID, pLine);
	pLine->setLayer(strCadLyName);
	pLine->close();

	// ��������㣨����б�ߣ�
	//if (acedGetPoint(p2, "\n��ѡ���ע����", p3) != RTNORM)
	//{
	//	pBlockTableRecord->close();
	//	return FALSE;
	//}
	// ����ˮƽ��ע��
	p3[0] = 2 * p2[0] - p1[0];
	p3[1] = p2[1];
	p3[2] = p2[2];

	float fSpace = FontHeight * 0.5;
	//����p2,p3�ĽǶ�
	ads_real dAngle;
	dAngle = acutAngle(p2, p3);
	CString str;
	//str.Format("X=%0.3f", p1[0]);
	str.Format(" X=%.3f", p1[1]);// ����XY���꣬���С�������λ

	double distance = GetTextWidth(str, FontHeight) + 2*FontHeight;
	p3[0] = p2[0] + distance * cos(dAngle);
	p3[1] = p2[1] + distance * sin(dAngle);
	p3[2] = p2[2];

	//����ע��
	startPt.x = p2[0]; startPt.y = p2[1]; startPt.z = p2[2];
	endPt.x = p3[0]; endPt.y = p3[1]; endPt.z = p3[2];
	pLine = new AcDbLine(startPt, endPt);
	pBlockTableRecord->appendAcDbEntity(ObjID, pLine);
	pLine->setLayer(strCadLyName);
	pLine->close();

	//�������������
	//if (AddBigFontToCurDwg("", USER_TextStyle) == false)
		TextID = GetTextStyleObjID("Standard");
	//else TextID = GetTextStyleObjID(USER_TextStyle);

	if ((p2[0] < p3[0]) || (p2[0] == p3[0] && p2[1] > p3[1]))
	{
		//��p2�㿪ʼ���������ע���
		ads_real dLabelAngle;  //�����ע�����õĽǶ�
		CoverAngelP2(dAngle, dLabelAngle);
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = p2[0] + fSpace * cos(dLabelAngle);
		InsertPt.y = p2[1] + fSpace * sin(dLabelAngle);
		InsertPt.z = p2[2];
		CString str;
		//str.Format("X=%0.3f", p1[0]);
		str.Format(" X=%.3f", p1[1]);// ����XY���꣬���С�������λ
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->setLayer(strCadLyName);
		pText->close();

		InsertPt.x = p2[0] - (fSpace + FontHeight) * cos(dLabelAngle);
		InsertPt.y = p2[1] - (fSpace + FontHeight) * sin(dLabelAngle);
		InsertPt.z = p2[2];
		//str.Format("Y=%0.3f", p1[1]);
		str.Format(" Y=%.3f", p1[0]);// ����XY���꣬���С�������λ
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->setLayer(strCadLyName);
		pText->close();
	}
	else 
	{
		//��p3�㿪ʼ���������ע���
		ads_real dLabelAngle;  //�����ע�����õĽǶ�
		CoverAngelP3(dAngle, dLabelAngle);
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = p3[0] + fSpace * cos(dLabelAngle);
		InsertPt.y = p3[1] + fSpace * sin(dLabelAngle);
		InsertPt.z = p3[2];
		CString str;
		//str.Format("X=%0.3f", p1[0]);
		str.Format(" X=%.3f", p1[1]);// ����XY���꣬���С�������λ
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->setLayer(strCadLyName);
		pText->close();

		InsertPt.x = p3[0] - (fSpace + FontHeight) * cos(dLabelAngle);
		InsertPt.y = p3[1] - (fSpace + FontHeight) * sin(dLabelAngle);
		InsertPt.z = p3[2];
		//str.Format("Y=%0.3f", p1[1]);
		str.Format(" Y=%.3f", p1[0]);// ����XY���꣬���С�������λ
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->setLayer(strCadLyName);
		pText->close();
	}
	pBlockTableRecord->close();

	return TRUE;
}

void CSpcoordLabel::CoverAngelP2(ads_real &dAngle,ads_real &dLabelAngle)
{
	if(dAngle>=0 && dAngle<PI*0.5)
	{
		dLabelAngle = dAngle + PI*0.5;
	}
	else if(dAngle>=PI*0.5 && dAngle<PI*2)
	{
		dLabelAngle = dAngle - PI*1.5;
	}
}

void CSpcoordLabel::CoverAngelP3(ads_real &dAngle,ads_real &dLabelAngle)
{
	if (dAngle >= PI*0.5 && dAngle < PI)
	{
		dLabelAngle = dAngle - PI*0.5;
		dAngle = PI + dAngle;
	}
	else if (dAngle >= PI && dAngle <= PI*1.5)
	{
		dLabelAngle = dAngle - PI*0.5;
		dAngle = dAngle - PI;
	}
}

double CSpcoordLabel::GetTextWidth(LPCTSTR sText, double dHigh)
{
	if (strlen (sText)>1023 ) return 0;//����ط��ݴ�һ�� 
	char cBuffer[1024];
	//���ָ�
	struct resbuf *pZG = acutBuildList(RTREAL,dHigh,0);//�����ı���
	acedSetVar("textsize", pZG);
	acutRelRb(pZG);
	//������ÿ�е�����п�
	ads_point ptLB;
	ads_point ptRT;
	struct resbuf rb;
	rb.restype = 1;
	rb.resval.rstring = cBuffer;
	rb.rbnext = NULL;
	strcpy(cBuffer, sText);//����ط����ù���
	double dReturn = 0;
	if (acedTextBox(&rb, ptLB, ptRT) == RTNORM)
	{
		dReturn = ptRT[X];
	}
	return dReturn;
}

BOOL CSpcoordLabel::AddBigFontToCurDwg(CString BigFontFileName, CString TextStyleName)
{
	AcDbTextStyleTable *pTextStyleTable;
	acdbHostApplicationServices()->workingDatabase()
		->getTextStyleTable(pTextStyleTable, AcDb::kForWrite);

	AcDbTextStyleTableRecord *pTextStyleTableRecord;
	AcDbObjectId TextID;

	pTextStyleTableRecord = new AcDbTextStyleTableRecord;
	pTextStyleTableRecord->setBigFontFileName(BigFontFileName);
	pTextStyleTableRecord->setFileName("����");
	pTextStyleTableRecord->setName(TextStyleName); //���Ա�ער������
	if(pTextStyleTable->add(TextID, pTextStyleTableRecord)==Acad::eOk)
	{
		pTextStyleTableRecord->close();
	}
	else
	{
		delete pTextStyleTableRecord;
	}
	if(pTextStyleTable->getAt(TextStyleName, TextID)!=Acad::eOk)
	{
		char charBuff[1024];
		sprintf(charBuff, "�Ҳ���\"%s\"�ļ������Ա�ער���������", (LPCTSTR)BigFontFileName);
		MessageBoxA(NULL, charBuff, "������ʾ", MB_OK|MB_ICONINFORMATION);
		pTextStyleTableRecord->close();
		pTextStyleTable->close();
		return false;
	}

	pTextStyleTableRecord->close();
	pTextStyleTable->close();

	return true;
}

AcDbObjectId CSpcoordLabel::GetTextStyleObjID(CString TextStyleName)
{
	AcDbTextStyleTable *pTextStyleTable;
	acdbHostApplicationServices()->workingDatabase()
		->getTextStyleTable(pTextStyleTable, AcDb::kForRead);

	AcDbObjectId TextID;
	TextID.setNull();

	if(pTextStyleTable->getAt(TextStyleName, TextID)!=Acad::eOk)
	{
		char charBuff[1024];
		sprintf(charBuff, "�Ҳ���\"%s\"������", (LPCTSTR)TextStyleName);
		MessageBoxA(NULL, charBuff, "������ʾ", MB_OK|MB_ICONINFORMATION);
		pTextStyleTable->close();
		TextID.setNull();
		return TextID;
	}

	pTextStyleTable->close();

	return TextID;
}


BOOL CSpcoordLabel::CreateSpcoordLabelX(ads_point SetPt /*= NULL*/, BOOL bInPut /*= FALSE*/)
{
	//�õ��������
	ads_point p1, p2 ,p3;
	if (bInPut == TRUE)
	{
		p1[0] = SetPt[0]; p1[1] = SetPt[1]; p1[2] = SetPt[2];
	}
	else
	{
		if (acedGetPoint(NULL, "\n��ѡ����Ҫ��ע�ĵ�", p1) != RTNORM)
		{
			return FALSE;
		}
	}
	if (acedGetPoint(p1, "\n��ѡ�������߶˵�", p2) != RTNORM)
	{
		return FALSE;
	}
	acutPrintf("\n");

	//������
	AcDbBlockTable *pBlockTable;
	acdbCurDwg()->getBlockTable(pBlockTable, AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	pBlockTable->close();
	AcDbObjectId TextID;

	AcDbObjectId ObjID;
	AcDbLine *pLine;
	AcGePoint3d startPt;
	AcGePoint3d endPt;

	startPt.x = p1[0]; startPt.y = p1[1]; startPt.z = p1[2];
	endPt.x = p2[0]; endPt.y = p2[1]; endPt.z = p2[2];
	pLine = new AcDbLine(startPt, endPt);
	pBlockTableRecord->appendAcDbEntity(ObjID, pLine);
	pBlockTableRecord->close();
	pLine->close();

	CDistEntityTool tempTool;
	AcDbObjectId resultId;
	AcGePoint3d basePt(p2[0],p2[1],0);
	CStringArray strTagArray,strTextArray;
	strTagArray.Add("X");strTagArray.Add("Y");
	CString strTemp;
	strTemp.Format("X=%0.3f",p1[1]);  //X��
	strTextArray.Add(strTemp);
	strTemp.Format("Y=%0.3f",p1[0]);
	strTextArray.Add(strTemp);

	if(p2[0] > p1[0])
		tempTool.CreateBlkRefAttr(resultId,basePt,"�����ע_R","0",256,"3.0#3.0#1.0#",0,strTagArray,strTextArray);
	else
		tempTool.CreateBlkRefAttr(resultId,basePt,"�����ע_L","0",256,"3.0#3.0#1.0#",0,strTagArray,strTextArray);


	/*
	// ��������㣨����б�ߣ�
	//if (acedGetPoint(p2, "\n��ѡ���ע����", p3) != RTNORM)
	//{
	//	pBlockTableRecord->close();
	//	return FALSE;
	//}
	// ����ˮƽ��ע��
	p3[0] = 2 * p2[0] - p1[0];
	p3[1] = p2[1];
	p3[2] = p2[2];

	float fSpace = FontHeight * 0.5;
	//����p2,p3�ĽǶ�
	ads_real dAngle;
	dAngle = acutAngle(p2, p3);
	CString str;
	str.Format(" X=%.2f", p1[1]);// ����XY���꣬���С�������λ

	double distance = GetTextWidth(str, FontHeight) + 2*FontHeight;
	p3[0] = p2[0] + distance * cos(dAngle);
	p3[1] = p2[1] + distance * sin(dAngle);
	p3[2] = p2[2];
	//����ע��
	startPt.x = p2[0]; startPt.y = p2[1]; startPt.z = p2[2];
	endPt.x = p3[0]; endPt.y = p3[1]; endPt.z = p3[2];
	pLine = new AcDbLine(startPt, endPt);
	pBlockTableRecord->appendAcDbEntity(ObjID, pLine);
	pLine->close();

	//�������������
	TextID = GetTextStyleObjID("Standard");

	if ((p2[0] < p3[0]) || (p2[0] == p3[0] && p2[1] > p3[1]))
	{
		//��p2�㿪ʼ���������ע���
		ads_real dLabelAngle;  //�����ע�����õĽǶ�
		CoverAngelP2(dAngle, dLabelAngle);
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = p2[0] + fSpace * cos(dLabelAngle);
		InsertPt.y = p2[1] + fSpace * sin(dLabelAngle);
		InsertPt.z = p2[2];
		CString str;
		str.Format(" X=%.2f", p1[1]);// ����XY���꣬���С�������λ
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();

		InsertPt.x = p2[0] - (fSpace + FontHeight) * cos(dLabelAngle);
		InsertPt.y = p2[1] - (fSpace + FontHeight) * sin(dLabelAngle);
		InsertPt.z = p2[2];
		//str.Format("Y=%f", p1[1]);
		str.Format(" Y=%.2f", p1[0]);// ����XY���꣬���С�������λ
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
	}
	else 
	{
		//��p3�㿪ʼ���������ע���
		ads_real dLabelAngle;  //�����ע�����õĽǶ�
		CoverAngelP3(dAngle, dLabelAngle);
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = p3[0] + fSpace * cos(dLabelAngle);
		InsertPt.y = p3[1] + fSpace * sin(dLabelAngle);
		InsertPt.z = p3[2];
		CString str;
		//str.Format("X=%f", p1[0]);
		str.Format(" X=%.2f", p1[1]);// ����XY���꣬���С�������λ
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();

		InsertPt.x = p3[0] - (fSpace + FontHeight) * cos(dLabelAngle);
		InsertPt.y = p3[1] - (fSpace + FontHeight) * sin(dLabelAngle);
		InsertPt.z = p3[2];
		//str.Format("Y=%f", p1[1]);
		str.Format(" Y=%.2f", p1[0]);// ����XY���꣬���С�������λ
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
	}

	pBlockTableRecord->close();
	*/
	

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
CCircleLabel::CCircleLabel(void)
{
}

CCircleLabel::~CCircleLabel(void)
{
}

BOOL CCircleLabel::CreatCircleLabel(CString strCadLyName,ads_point StartPt, ads_point EndPt, double Bulge)
{
	//���Բ������ͻ����е�����
	ads_point ptCentre, ptMid;

	double dDist = acutDistance(StartPt, EndPt);
	double dAngle = acutAngle(EndPt, StartPt);
	double dRadius = dDist / (2.0 * sin(2.0 * atan(Bulge)));

	ads_point ptMidTmp;
	ptMidTmp[0] = (StartPt[0] + EndPt[0]) / 2.0;
	ptMidTmp[1] = (StartPt[1] + EndPt[1]) / 2.0;
	ptMidTmp[2] = (StartPt[2] + EndPt[2]) / 2.0;

	acutPolar(ptMidTmp, dAngle + PI / 2.0, dDist * Bulge / 2.0, ptMid);
	acutPolar(ptMid, dAngle - PI / 2.0, dRadius, ptCentre);

	//�õ��������
	ads_point pIn;

	if (acedGetPoint(ptCentre,"\n��ѡ�������߶˵�", pIn) != RTNORM)
	{
		return FALSE;
	}

	acutPolar(ptCentre, acutAngle(ptCentre, ptMid), acutDistance(pIn, ptCentre), pIn);

	//������
	AcDbBlockTable *pBlockTable;
	acdbCurDwg()->getBlockTable(pBlockTable, AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord=NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	pBlockTable->close();
	AcDbObjectId TextID;

	AcDbObjectId ObjID;
	AcDbLine *pLine;
	AcGePoint3d startPt;
	AcGePoint3d endPt;

	startPt.x = ptCentre[0]; startPt.y = ptCentre[1]; startPt.z = ptCentre[2];
	endPt.x = pIn[0]; endPt.y = pIn[1]; endPt.z = pIn[2];
	pLine = new AcDbLine(startPt, endPt);
	pBlockTableRecord->appendAcDbEntity(ObjID, pLine);
	pLine->setLayer(strCadLyName);
	pLine->close();

	float fSpace = FontHeight * 0.5;

	//�������������
	//if (AddBigFontToCurDwg("", USER_TextStyle) == false)
	TextID = GetTextStyleObjID("Standard");
	//else TextID = GetTextStyleObjID(USER_TextStyle);

	dRadius = fabs(dRadius);

	CString str;
	//str.Format("R%f", dRadius);
	str.Format(" R%.f", dRadius);// �����С����
	ads_real dLabelAngle = acutAngle(pIn, ptCentre);//�����ע�����õĽǶ�

	//�ڶ�������
	if (dLabelAngle >= PI / 2.0 && dLabelAngle < PI * 3.0 / 2.0)  
	{
		AcGePoint3d InsertPt;
		AcDbText *pText=NULL;
		double distance = GetTextWidth(str, FontHeight);
		//InsertPt.x = pIn[0] + fSpace * sin(dLabelAngle) + distance * cos(dLabelAngle);
		//InsertPt.y = pIn[1] - fSpace * cos(dLabelAngle) + distance * sin(dLabelAngle);
		//InsertPt.z = pIn[2];
		double dx,dy;
		if(dLabelAngle > 0)
		{
			dx = (-1)*fSpace;
			dy = 0.0;
		}
		else
		{
			dx = 1.0;
			dy = fSpace;

		}
		InsertPt.x = ptCentre[0]+dx;
		InsertPt.y = ptCentre[1]+dy;
		InsertPt.z = ptCentre[2];

		pText = new AcDbText(InsertPt,str,TextID,FontHeight,PI + dLabelAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->setLayer(strCadLyName);
		pText->close();
	}
	//��һ������
	else
	{
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = pIn[0] - fSpace * sin(dLabelAngle);
		InsertPt.y = pIn[1] + fSpace * cos(dLabelAngle);
		InsertPt.z = pIn[2];
		pText = new AcDbText(InsertPt,str,TextID,FontHeight,dLabelAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->setLayer(strCadLyName);
		pText->close();
	}

	pBlockTableRecord->close();

	return TRUE;
}

BOOL CCircleLabel::CreatCircleLabel(CString strCadLyName,AcGePoint3d ptCen,double dR,double dMAngle)
{
	AcGePoint3d ptM;
	AcGePoint3d ptText;
	ptM.x = ptCen.x+cos(dMAngle)*dR; 
	ptM.y = ptCen.y+sin(dMAngle)*dR;

	double ddx,ddy;
	ddx = cos(dMAngle+PI/2)*2;
	ddy = sin(dMAngle+PI/2)*2;
	ptText.x = ptCen.x + ddx;
	ptText.y = ptCen.y + ddy;

	AcDbBlockTable *pBlockTable = NULL;
	acdbCurDwg()->getBlockTable(pBlockTable, AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord = NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	pBlockTable->close();

	AcDbObjectId ObjId;
	AcDbLine* pLine = new AcDbLine(ptCen, ptM);
	pBlockTableRecord->appendAcDbEntity(ObjId, pLine);
	pLine->setLayer(strCadLyName);
	pLine->close();

	CString strTxt;
	strTxt.Format("R%0.3f",dR);
	//acutPrintf("\n Angle=%0.3f",dMAngle*180/PI);
	AcDbText* pText = new AcDbText(ptText,strTxt,AcDbObjectId::kNull,FontHeight,dMAngle);
	pBlockTableRecord->appendAcDbEntity(ObjId, pText);
	pText->setLayer(strCadLyName);
	pText->close();

	pBlockTableRecord->close();

	return TRUE;
}

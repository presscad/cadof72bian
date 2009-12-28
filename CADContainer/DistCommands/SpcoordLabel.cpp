#include "StdAfx.h"
#include "SpcoordLabel.h"
#include "DistXData.h"
#include "DataDefine.h"
#include "CommonFunc.h"
#include "..\SdeData\DistBaseInterface.h"

extern IDistConnection * Glb_pConnection;
extern ST_BIZZINFO Glb_BizzInfo; // 业务信息
extern ST_LAYERINFO Glb_LayerInfo; // 图层信息

double CSpcoordLabel::FontHeight = 5.0;

void CreatLabelPlineAllPt()
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

	ads_name ssTemp;

	acedSSName(ssName, 0, ssTemp);

	AcDbObjectId dbObjId;

	acdbGetObjectId(dbObjId, ssTemp);

	acedSSFree(ssName);

	AcDbPolyline * pPline = NULL;

	if (Acad::eOk != acdbOpenObject(pPline, dbObjId, AcDb::kForRead))
	{
		return;
	}

	pPline->close();

	int nCount = pPline->numVerts();

	CCircleLabel temp;

	for (int i = 0; i < nCount; i++)
	{
		AcGePoint3d pt3d, pt3dE;

		if (Acad::eOk != pPline->getPointAt(i, pt3d))
		{
			continue;
		}

		ads_point ptSet, ptSetE;

		ptSet[0] = pt3d.x;
		ptSet[1] = pt3d.y;
		ptSet[2] = pt3d.z;

		temp.CreateSpcoordLabel(ptSet, TRUE);

		if (Adesk::kTrue == pPline->hasBulges())
		{
			if (i == nCount - 1)
			{
				if (Acad::eOk != pPline->getPointAt(0, pt3dE))
				{
					continue;
				}
			}
			else
			{
				if (Acad::eOk != pPline->getPointAt(i + 1, pt3dE))
				{
					continue;
				}
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
				temp.CreatCircleLabel(ptSet, ptSetE, Bulge);
			}
		}
	}
}

// 创建红线标注
void CreateHXBZ(CString strLineLayer, CString strTextLayer)
{
	CCircleLabel temp;

	temp.CreateSpcoordLabel(strLineLayer, strTextLayer, NULL);
}


//////////////////////////////////////////////////////////////////////////
CSpcoordLabel::CSpcoordLabel(void)
{
	// 获取捕捉参数
	m_OldRbf = acutNewRb(RTREAL);
	acedGetVar("osmode", m_OldRbf);
	// 设置新捕捉参数（点捕捉）
	struct resbuf *NewRbf = acutBuildList(RTSHORT, 9, 0);
	acedSetVar("osmode", NewRbf);
	acutRelRb(NewRbf);
}

CSpcoordLabel::~CSpcoordLabel(void)
{
	// 还原捕捉参数
	acedSetVar("osmode", m_OldRbf);
	acutRelRb(m_OldRbf);
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
	if (strlen (sText)>1023 ) return 0;//这个地方容错一下 
	char cBuffer[1024];
	//设字高
	struct resbuf *pZG = acutBuildList(RTREAL,dHigh,0);//表体文本高
	acedSetVar("textsize", pZG);
	acutRelRb(pZG);
	//下面求每列的最大列宽
	ads_point ptLB;
	ads_point ptRT;
	struct resbuf rb;
	rb.restype = 1;
	rb.resval.rstring = cBuffer;
	rb.rbnext = NULL;
	strcpy(cBuffer, sText);//这个地方不用管了
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
	pTextStyleTableRecord->setFileName("宋体");
	pTextStyleTableRecord->setName(TextStyleName); //属性标注专用字体
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
		sprintf(charBuff, "找不到\"%s\"文件或属性标注专用字体对像", (LPCTSTR)BigFontFileName);
		MessageBoxA(NULL, charBuff, "数慧提示", MB_OK|MB_ICONINFORMATION);
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
		sprintf(charBuff, "找不到\"%s\"字体风格", (LPCTSTR)TextStyleName);
		MessageBoxA(NULL, charBuff, "数慧提示", MB_OK|MB_ICONINFORMATION);
		pTextStyleTable->close();
		TextID.setNull();
		return TextID;
	}

	pTextStyleTable->close();

	return TextID;
}


//////////////////////////////////////////////////////////////////////////
CCircleLabel::CCircleLabel(void)
{
}

CCircleLabel::~CCircleLabel(void)
{
}

BOOL CSpcoordLabel::CreateSpcoordLabel(ads_point SetPt /*= NULL*/, BOOL bInPut /*= FALSE*/)
{
	//得到输入参数
	ads_point p1, p2 ,p3;
	if (bInPut == TRUE)
	{
		p1[0] = SetPt[0];
		p1[1] = SetPt[1];
		p1[2] = SetPt[2];
	}
	else
	{
		if (acedGetPoint(NULL, "\n请选择所要标注的点", p1) != RTNORM)
		{
			return FALSE;
		}
	}
	if (acedGetPoint(p1, "\n请选择引出线端点", p2) != RTNORM)
	{
		return FALSE;
	}

	//画引线
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
	pLine->close();

	// 输入第三点（允许斜线）
	//if (acedGetPoint(p2, "\n请选择标注方向", p3) != RTNORM)
	//{
	//	pBlockTableRecord->close();
	//	return FALSE;
	//}
	// 设置水平标注线
	p3[0] = 2 * p2[0] - p1[0];
	p3[1] = p2[1];
	p3[2] = p2[2];

	float fSpace = FontHeight * 0.5;
	//计算p2,p3的角度
	ads_real dAngle;
	dAngle = acutAngle(p2, p3);
	CString str;
	//str.Format("X=%f", p1[0]);
	str.Format(" X=%.3f", p1[1]);// 交换XY坐标，输出小数点后三位

	double distance = GetTextWidth(str, FontHeight) + 2*FontHeight;
	p3[0] = p2[0] + distance * cos(dAngle);
	p3[1] = p2[1] + distance * sin(dAngle);
	p3[2] = p2[2];
	//画标注线
	startPt.x = p2[0]; startPt.y = p2[1]; startPt.z = p2[2];
	endPt.x = p3[0]; endPt.y = p3[1]; endPt.z = p3[2];
	pLine = new AcDbLine(startPt, endPt);
	pBlockTableRecord->appendAcDbEntity(ObjID, pLine);
	pLine->close();

	//添加新字体类型
	//if (AddBigFontToCurDwg("", USER_TextStyle) == false)
	TextID = GetTextStyleObjID("Standard");
	//else TextID = GetTextStyleObjID(USER_TextStyle);

	if ((p2[0] < p3[0]) || (p2[0] == p3[0] && p2[1] > p3[1]))
	{
		//从p2点开始计算坐标标注起点
		ads_real dLabelAngle;  //计算标注坐标用的角度
		CoverAngelP2(dAngle, dLabelAngle);
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = p2[0] + fSpace * cos(dLabelAngle);
		InsertPt.y = p2[1] + fSpace * sin(dLabelAngle);
		InsertPt.z = p2[2];
		CString str;
		//str.Format("X=%f", p1[0]);
		str.Format(" X=%.3f", p1[1]);// 交换XY坐标，输出小数点后三位
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
		str.Format(" Y=%.3f", p1[0]);// 交换XY坐标，输出小数点后三位
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
		//从p3点开始计算坐标标注起点
		ads_real dLabelAngle;  //计算标注坐标用的角度
		CoverAngelP3(dAngle, dLabelAngle);
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = p3[0] + fSpace * cos(dLabelAngle);
		InsertPt.y = p3[1] + fSpace * sin(dLabelAngle);
		InsertPt.z = p3[2];
		CString str;
		//str.Format("X=%f", p1[0]);
		str.Format(" X=%.3f", p1[1]);// 交换XY坐标，输出小数点后三位
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
		str.Format(" Y=%.3f", p1[0]);// 交换XY坐标，输出小数点后三位
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

	return TRUE;
}

BOOL CSpcoordLabel::AddProjectAtrr(AcDbObjectId& objectId, CString strCadLayerName, CString& strSdeLayerName)
{
	if (strSdeLayerName.IsEmpty())
	{
		if(Glb_pConnection==NULL)
		{
			acutPrintf("\n SDE 空间库没有连接！"); 
			return FALSE;
		}

		IDistCommand *pCommand=CreateSDECommandObjcet();
		pCommand->SetConnectObj(Glb_pConnection);
		char strSQL[255]={0};
		sprintf(strSQL,"F:SDELYNAME,T:CADMAPTOSDE,W:CADLYNAME='%s',##",strCadLayerName);
		IDistRecordSet *pRcdset = NULL;
		if(pCommand->SelectData(strSQL,&pRcdset) != 1)
		{
			pCommand->Release(); 
			return FALSE;
		}

		char strSdeName[255] = {0};
		if(pRcdset->BatchRead() != 1)
		{
			pRcdset->Release();
			pCommand->Release();
			return FALSE;
		}

		strcpy(strSdeName,(char*)(*pRcdset)["SDELYNAME"]);
		strSdeLayerName = strSdeName;

		pRcdset->Release();
		pCommand->Release();
	}

	char strXDataName[1024]={0};
	sprintf(strXDataName,"DIST_%s",strSdeLayerName);
	CDistXData tempXdata(objectId,strXDataName);
//	if(tempXdata.GetRecordCount() != 0) 
//	{
//		return FALSE;
//	}

	tempXdata.Add("XMBH", Glb_BizzInfo.strBizzCode);
	tempXdata.Add("XMMC", Glb_BizzInfo.strBizzName);
	tempXdata.Add("JSDW", Glb_BizzInfo.strBzOwner);

	tempXdata.Update();
	tempXdata.Close();

	return TRUE;
}

// 添加文字标注
BOOL CSpcoordLabel::CreateDimText(CString strTextLayer, ads_point pt /* = NULL*/)
{
	ads_point p1;

	CString strPrompt;
	strPrompt.Format("\n 请指定位置点或输入字高 <%.1f>", FontHeight);

	acedInitGet(INP_NNEG | RSG_OTHER, "1 2 3 4 5 6 7 8 9 .");

	switch (acedGetPoint(NULL, strPrompt, p1))
	{
	case RTNORM:
		break;
	case RTKWORD:
		{
			char szInput[128] = {0};
			acedGetInput(szInput);
			CString strInput = szInput;
			if (strInput.Find(',') < 0)
				FontHeight = atof(strInput);

			strPrompt = "\n 请输入下一个点";
			acedGetPoint(NULL, strPrompt, p1);
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

//#include "LocalFun.h"
BOOL CSpcoordLabel::CreateSpcoordLabel(CString strLineLayer, CString strTextLayer, ads_point startPt /*= NULL*/)
{
	//得到输入参数
	ads_point p1, p2 ,p3;
	if (startPt != NULL)
	{
		p1[0] = startPt[0];
		p1[1] = startPt[1];
		p1[2] = startPt[2];
	}
	else
	{
		//得到输入参数
		if (acedGetPoint(NULL, "\n指定起点", p1) != RTNORM)
			return FALSE;
	}
	// 输入第二点
	CString strPrompt;
	strPrompt.Format("\n 请指定下一个点或输入字高 <%.1f>", FontHeight);

	acedInitGet(INP_NNEG | RSG_OTHER, "1 2 3 4 5 6 7 8 9 .");

	switch (acedGetPoint(p1, strPrompt, p2))
	{
	case RTNORM:
		break;
	case RTKWORD:
		{
			char szInput[128] = {0};
			acedGetInput(szInput);
			CString strInput = szInput;
			if (strInput.Find(',') < 0)
				FontHeight = atof(strInput);

			strPrompt = "\n 请输入下一个点";
			acedGetPoint(p1, strPrompt, p2);
		}
		break;
	default:
		return FALSE;
	}

	// 第三个点
	p3[0] = 2 * p2[0] - p1[0];
	p3[1] = p2[1];
	p3[2] = p2[2];

	float fSpace = FontHeight * 0.5;
	//计算p2,p3的角度
	ads_real dAngle;
	dAngle = acutAngle(p2, p3);

	CString str;
	str.Format(" X=%.3f", p1[1]);// 交换XY坐标，输出小数点后三位

	double distance = GetTextWidth(str, FontHeight) + 2*FontHeight;

	p3[0] = p2[0] + distance * cos(dAngle);
	p3[1] = p2[1] + distance * sin(dAngle);
	p3[2] = p2[2];

	//画引线
	AcDbBlockTable *pBlockTable;
	acdbCurDwg()->getBlockTable(pBlockTable, AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	pBlockTable->close();
	AcDbObjectId TextID;

	AcDbObjectId ObjID;
	AcDbPolyline *pLine;
	AcGePoint2d pt1;
	AcGePoint2d pt2;
	AcGePoint2d pt3;

	pt1.x = p1[0]; pt1.y = p1[1]; 
	pt2.x = p2[0]; pt2.y = p2[1]; 
	pt3.x = p3[0]; pt3.y = p3[1]; 

	pLine = new AcDbPolyline(3);
	pLine->addVertexAt(0, pt1);
	pLine->addVertexAt(1, pt2);
	pLine->addVertexAt(2, pt3);

	pLine->setLayer(strLineLayer);

	pBlockTableRecord->appendAcDbEntity(ObjID, pLine);
	pLine->close();

//	AddProjectAtrr(ObjID, strLineLayer, Glb_LayerInfo.strAnnotionLine);

	//添加新字体类型
	//if (AddBigFontToCurDwg("", USER_TextStyle) == false)
	TextID = GetTextStyleObjID("Standard");
	//else TextID = GetTextStyleObjID(USER_TextStyle);

	CreateLayer(strTextLayer.GetBuffer());

	if ((p2[0] < p3[0]) || (p2[0] == p3[0] && p2[1] > p3[1]))
	{
		//从p2点开始计算坐标标注起点
		ads_real dLabelAngle;  //计算标注坐标用的角度
		CoverAngelP2(dAngle, dLabelAngle);
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = p2[0] + fSpace * cos(dLabelAngle);
		InsertPt.y = p2[1] + fSpace * sin(dLabelAngle);
		InsertPt.z = p2[2];
		CString str;
		str.Format(" X=%.3f", p1[1]);// 交换XY坐标，输出小数点后三位

		pText = new AcDbText(InsertPt, str, TextID, FontHeight, dAngle);
		pText->setLayer(strTextLayer);

		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
//		AddProjectAtrr(ObjID, strTextLayer, Glb_LayerInfo.strAnnotionText);

		InsertPt.x = p2[0] - (fSpace + FontHeight) * cos(dLabelAngle);
		InsertPt.y = p2[1] - (fSpace + FontHeight) * sin(dLabelAngle);
		InsertPt.z = p2[2];

		str.Format(" Y=%.3f", p1[0]);// 交换XY坐标，输出小数点后三位

		pText = new AcDbText(InsertPt, str, TextID, FontHeight, dAngle);
		pText->setLayer(strTextLayer);

		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
//		AddProjectAtrr(ObjID, strTextLayer, Glb_LayerInfo.strAnnotionText);
	}
	else 
	{
		//从p3点开始计算坐标标注起点
		ads_real dLabelAngle;  //计算标注坐标用的角度
		CoverAngelP3(dAngle, dLabelAngle);
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = p3[0] + fSpace * cos(dLabelAngle);
		InsertPt.y = p3[1] + fSpace * sin(dLabelAngle);
		InsertPt.z = p3[2];
		CString str;
		str.Format(" X=%.3f", p1[1]);// 交换XY坐标，输出小数点后三位

		pText = new AcDbText(InsertPt, str, TextID, FontHeight, dAngle);
		pText->setLayer(strTextLayer);

		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
//		AddProjectAtrr(ObjID, strTextLayer, Glb_LayerInfo.strAnnotionText);

		InsertPt.x = p3[0] - (fSpace + FontHeight) * cos(dLabelAngle);
		InsertPt.y = p3[1] - (fSpace + FontHeight) * sin(dLabelAngle);
		InsertPt.z = p3[2];

		str.Format(" Y=%.3f", p1[0]);// 交换XY坐标，输出小数点后三位

		pText = new AcDbText(InsertPt, str, TextID, FontHeight, dAngle);
		pText->setLayer(strTextLayer);

		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
//		AddProjectAtrr(ObjID, strTextLayer, Glb_LayerInfo.strAnnotionText);
	}
	pBlockTableRecord->close();

	return TRUE;
}

BOOL CCircleLabel::CreatCircleLabel(ads_point StartPt, ads_point EndPt, double Bulge)
{
	//获得圆心坐标和弧线中点坐标
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

	//得到输入参数
	ads_point pIn;

	if (acedGetPoint(ptCentre, "\n请选择引出线端点", pIn) != RTNORM)
	{
		return FALSE;
	}

	acutPolar(ptCentre, acutAngle(ptCentre, ptMid), acutDistance(pIn, ptCentre), pIn);

	//画引线
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

	startPt.x = ptCentre[0]; startPt.y = ptCentre[1]; startPt.z = ptCentre[2];
	endPt.x = pIn[0]; endPt.y = pIn[1]; endPt.z = pIn[2];
	pLine = new AcDbLine(startPt, endPt);
	pBlockTableRecord->appendAcDbEntity(ObjID, pLine);
	pLine->close();

	float fSpace = FontHeight * 0.5;

	//添加新字体类型
	//if (AddBigFontToCurDwg("", USER_TextStyle) == false)
	TextID = GetTextStyleObjID("Standard");
	//else TextID = GetTextStyleObjID(USER_TextStyle);

	dRadius = fabs(dRadius);
	CString str;
	//str.Format("R%f", dRadius);
	str.Format(" R%.f", dRadius);// 输出到小数点
	ads_real dLabelAngle = acutAngle(pIn, ptCentre);//计算标注坐标用的角度
	if (dLabelAngle >= PI / 2.0 && dLabelAngle < PI * 3.0 / 2.0)
	{
		AcGePoint3d InsertPt;
		AcDbText *pText;
		double distance = GetTextWidth(str, FontHeight);
		InsertPt.x = pIn[0] + fSpace * sin(dLabelAngle) + distance * cos(dLabelAngle);
		InsertPt.y = pIn[1] - fSpace * cos(dLabelAngle) + distance * sin(dLabelAngle);
		InsertPt.z = pIn[2];
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			PI + dLabelAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
	}
	else
	{
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = pIn[0] - fSpace * sin(dLabelAngle);
		InsertPt.y = pIn[1] + fSpace * cos(dLabelAngle);
		InsertPt.z = pIn[2];
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dLabelAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
	}

	pBlockTableRecord->close();

	return TRUE;
}

BOOL CCircleLabel::CreatCircleLabel(CString strLineLayer, CString strTextLayer, ads_point StartPt, ads_point EndPt, double Bulge)
{
	//获得圆心坐标和弧线中点坐标
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

	//得到输入参数
	ads_point pIn;

	if (acedGetPoint(ptCentre, "\n请选择引出线端点", pIn) != RTNORM)
	{
		return FALSE;
	}

	acutPolar(ptCentre, acutAngle(ptCentre, ptMid), acutDistance(pIn, ptCentre), pIn);

	//画引线
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

	startPt.x = ptCentre[0]; startPt.y = ptCentre[1]; startPt.z = ptCentre[2];
	endPt.x = pIn[0]; endPt.y = pIn[1]; endPt.z = pIn[2];
	pLine = new AcDbLine(startPt, endPt);
	pBlockTableRecord->appendAcDbEntity(ObjID, pLine);
	pLine->close();

	float fSpace = FontHeight * 0.5;

	//添加新字体类型
	//if (AddBigFontToCurDwg("", USER_TextStyle) == false)
	TextID = GetTextStyleObjID("Standard");
	//else TextID = GetTextStyleObjID(USER_TextStyle);

	dRadius = fabs(dRadius);
	CString str;
	//str.Format("R%f", dRadius);
	str.Format(" R%.f", dRadius);// 输出到小数点
	ads_real dLabelAngle = acutAngle(pIn, ptCentre);//计算标注坐标用的角度
	if (dLabelAngle >= PI / 2.0 && dLabelAngle < PI * 3.0 / 2.0)
	{
		AcGePoint3d InsertPt;
		AcDbText *pText;
		double distance = GetTextWidth(str, FontHeight);
		InsertPt.x = pIn[0] + fSpace * sin(dLabelAngle) + distance * cos(dLabelAngle);
		InsertPt.y = pIn[1] - fSpace * cos(dLabelAngle) + distance * sin(dLabelAngle);
		InsertPt.z = pIn[2];
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			PI + dLabelAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
	}
	else
	{
		AcGePoint3d InsertPt;
		AcDbText *pText;
		InsertPt.x = pIn[0] - fSpace * sin(dLabelAngle);
		InsertPt.y = pIn[1] + fSpace * cos(dLabelAngle);
		InsertPt.z = pIn[2];
		pText = new AcDbText(
			InsertPt,
			str,
			TextID,
			FontHeight,
			dLabelAngle);
		pBlockTableRecord->appendAcDbEntity(ObjID, pText);
		pText->close();
	}

	pBlockTableRecord->close();

	return TRUE;
}

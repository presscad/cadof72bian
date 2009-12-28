
#include "stdafx.h"
//#include "stdarx.h"
#include <adslib.h>
#include <dbjig.h>
#include <aced.h>
#include <gemat3d.h>
#include <dbents.h>
#include <AfxTempl.h>
#include <math.h>
#include <migrtion.h>
#include <dbpl.h>
#include "ARXPline.h"
ARXPline::ARXPline()
{
	m_pEnt = NULL;    //临时实体指针
	ppline = NULL;    //新实体指针
	addflag = 0;      //临时点数目标记
	bulge = 0.0;      //弧的凸度
	last_bulge = 0.0; //临时记录弧段起点的凸度
	startWidth = 0.0; //起点线宽
	endWidth = 0.0;   //终点线宽
	ColorIndex = 0;   //颜色号
	arcflag = FALSE;  //新的一段线型的标记，TRUE为弧，FALSE为直线。
	ptflag = TRUE;    //标记新输入的点是否有效，主要用于弧，判断是否不能构成弧.
	drawflag = 0;     //标记弧的绘制方式
	second_pt.x=0.0;
	second_pt.y=0.0;    //记录三点输入弧方式中的第二点
	CEnter_flag = FALSE;//圆心方式输入的标记
	Radius_flag = FALSE;//半径方式输入的标记
	Angle_flag = FALSE;
	Length_flag = FALSE;
	radius=0.0;         //半径
	qiexian_jiaodu=0.0; //上一段线的切线的弧度
	numVerts = 0;       //最后生成的实体的顶点数目
}

ARXPline::~ARXPline()
{
	if(m_pEnt != NULL)
	{
		delete m_pEnt;
	}
}

bool ARXPline::draw()
{
	bool result=FALSE;	
	ads_point adspt;
	AcGePoint2dArray ptarray;
	int res=RTERROR;
	char str[128];
	AcGePoint2d pt;

	m_pEnt=new AcDbPolyline();
	m_pEnt->setColorIndex(ColorIndex);


	res=acedGetPoint(NULL,"\n指定起点:",adspt);
	if(res!=RTNORM) return FALSE;
	pt.x=adspt[X]; pt.y=adspt[Y];
	m_pEnt->addVertexAt(0,pt);
	m_refPoint.x=pt.x;	m_refPoint.y=pt.y;	m_refPoint.z=0.0;
	m_LastPoint=m_refPoint;
	
	m_pEnt->addVertexAt(0,pt);
	ppline=AcDbPolyline::cast(m_pEnt->clone());
	m_pEnt->removeVertexAt(0);
	ppline->removeVertexAt(0);
	ppline->setWidthsAt(0,startWidth,endWidth);
	AcDbDatabase *db=acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable *BlockTable=NULL;
	db->getBlockTable(BlockTable,AcDb::kForRead);
	AcDbBlockTableRecord *BlockTableRecord;
	BlockTable->getAt(ACDB_MODEL_SPACE,BlockTableRecord,AcDb::kForWrite);
	BlockTable->close();
	BlockTableRecord->appendAcDbEntity(ppline);
	objId = ppline->objectId();
	ppline->close();
	BlockTableRecord->close();

	acutPrintf("\n当前线宽为 %.3f",startWidth);
	AcEdJig::DragStatus status = AcEdJig::kNormal;

	while(status != AcEdJig::kCancel)
	{
		if(arcflag)
		{
			if(drawflag ==0)
			{
				//setDispPrompt("\nSpecify endpoint of arc or [Angle/CEnter/CLose/Direction/Halfwidth/Line/Radius/Second pt/Undo/Width]:");
				setDispPrompt("\n指定圆弧的端点或[角度(A)/圆心(CE)/闭合(C)/方向(D)/半宽(H)/直线(L)/半径(R)/第二个点(S)/放弃(U)/宽度(W)]:");
			}
			else if(drawflag == 1)
			{
				if(CEnter_flag && !Radius_flag)
				{
					//setDispPrompt("\nSpecify center point of arc:");
					setDispPrompt("\n指定圆弧的圆心:");
				}
				else if(!CEnter_flag && !Radius_flag)
				{
					//setDispPrompt("\nSpecify endpoint of arc or [CEnter/Radius]:");
					setDispPrompt("\n指定圆弧的端点或 [圆心(CE)/半径(R)]:");
				}
				else if(!CEnter_flag && Radius_flag)
				{
					char tmp_prompt[256];
					double ang1=0;
					Acad::ErrorStatus es;
					AcGePoint2d tmp_endpt1,tmp_endpt2;
					double tmp_bulge=0;//凸度
					
					if((es=acdbOpenObject(ppline,objId,AcDb::kForRead)) != Acad::eOk)
					{
						acutPrintf("\nOpen entity error:%d",es);
						break;
					}
					ppline->getPointAt(ppline->numVerts()-1,tmp_endpt2);
					ppline->getPointAt(ppline->numVerts()-2,tmp_endpt1);
					ppline->getBulgeAt(ppline->numVerts()-2,tmp_bulge);
					ppline->close();
					if(tmp_bulge == 0)//上一段线是直线
					{
						ang1=((tmp_endpt2.asVector()-tmp_endpt1.asVector())/tmp_endpt2.distanceTo(tmp_endpt1)).angle();
						while(ang1 > 2*PI) ang1-=2*PI;
						while(ang1 < 0) ang1+=2*PI;
					}
					else//上一段线是弧
					{
						AcGeCircArc2d arc2d(tmp_endpt1,tmp_endpt2,tmp_bulge,Adesk::kFalse);
						if(tmp_bulge>0)
						{
							ang1=((tmp_endpt2.asVector()-arc2d.center().asVector())/arc2d.radius()).angle()+PI/2;
						}
						else
						{
							ang1=((tmp_endpt2.asVector()-arc2d.center().asVector())/arc2d.radius()).angle()-PI/2;
						}
						while(ang1 > 2*PI) ang1-=2*PI;
						while(ang1 < 0) ang1+=2*PI;
					}
					qiexian_jiaodu=ang1;
					//sprintf(tmp_prompt,"\nSpecify direction of chord for arc <%.0f>:",ang1*180/PI);
					sprintf(tmp_prompt,"\n指定圆弧的弦方向 <%.0f>:",ang1*180/PI);
					setDispPrompt(tmp_prompt);
				}
				else
				{
					//acutPrintf("\nFlag error!");
				}
			}
			else if(drawflag == 2)
			{
				if(Angle_flag && !Length_flag)
				{
					//setDispPrompt("\nSpecify included angle:");
					setDispPrompt("\n指定包含角:");
				}
				else if(!Angle_flag && Length_flag)
				{
					//setDispPrompt("\nSpecify length of chord:");
					setDispPrompt("\n指定圆弧的弦长度:");
				}
				else
				{
					//setDispPrompt("\nSpecify endpoint of arc or [Angle/Length]:");
					setDispPrompt("\n指定圆弧的端点或 [角度(A)/长度(L)]:");
				}
			}
			else if(drawflag == 3)
				setDispPrompt("");
			else if(drawflag == 4)
			{
				if(Angle_flag)
				{
					char tmp_prompt[256];
					double ang1=0.0;
					Acad::ErrorStatus es;
					AcGePoint2d tmp_endpt1,tmp_endpt2;
					double tmp_bulge=0;//凸度
					
					if((es=acdbOpenObject(ppline,objId,AcDb::kForRead)) != Acad::eOk)
					{
						acutPrintf("\nOpen entity error:%d",es);
						break;
					}
					ppline->getPointAt(ppline->numVerts()-1,tmp_endpt2);
					ppline->getPointAt(ppline->numVerts()-2,tmp_endpt1);
					ppline->getBulgeAt(ppline->numVerts()-2,tmp_bulge);
					ppline->close();
					if(tmp_bulge == 0)//上一段线是直线
					{
						ang1=((tmp_endpt2.asVector()-tmp_endpt1.asVector())/tmp_endpt2.distanceTo(tmp_endpt1)).angle();
						while(ang1 > 2*PI) ang1-=2*PI;
						while(ang1 < 0) ang1+=2*PI;
					}
					else//上一段线是弧
					{
						AcGeCircArc2d arc2d(tmp_endpt1,tmp_endpt2,tmp_bulge,Adesk::kFalse);
						if(tmp_bulge>0)
						{
							ang1=((tmp_endpt2.asVector()-arc2d.center().asVector())/arc2d.radius()).angle()+PI/2;
						}
						else
						{
							ang1=((tmp_endpt2.asVector()-arc2d.center().asVector())/arc2d.radius()).angle()-PI/2;
						}
						while(ang1 > 2*PI) ang1-=2*PI;
						while(ang1 < 0) ang1+=2*PI;
					}
					qiexian_jiaodu=ang1;
					//sprintf(tmp_prompt,"\nSpecify direction of chord for arc <%.0f>:",ang1*180/PI);
					sprintf(tmp_prompt,"\n指定圆弧的弦方向 <%.0f>:",ang1*180/PI);
					setDispPrompt(tmp_prompt);
				}
				else
				{
					//setDispPrompt("\nSpecify endpoint of arc or [Angle]:");
					setDispPrompt("\n指定圆弧的端点或 [角度(A)]:");
				}
			}
			else if(drawflag == 5)
			{
				//setDispPrompt("\nSpecify end point of arc:");
				setDispPrompt("\n指定圆弧的端点:");
			}
		}
		else
		{
			setDispPrompt("\n 指定下一个点或 [圆弧(A)/闭合(C)/半宽(H)/长度(L)/放弃(U)/宽度(W)]:");
			//setDispPrompt("\n Specify next point or [Arc/Close/Halfwidth/Length/Undo/Width]:");
		}
		status = drag();
		//DebugPrintf("\nstatus = %d",status);
		switch (status)
		{
		case AcEdJig::kKW1:
			if(arcflag)//Angle
			{
				if(drawflag == 0)//默认方式
				{
					drawflag = 1;
					ads_real tmp_jiaodu;
					ads_point tmp_adspt;
					AcGePoint2d tmp_pt;
					m_pEnt->getPointAt(0,tmp_pt);
					tmp_adspt[X]=tmp_pt.x;
					tmp_adspt[Y]=tmp_pt.y;
					acedInitGet(3,"");
					//acedGetAngle(tmp_adspt,"\nSpecify included angle:",&tmp_jiaodu);
					acedGetAngle(tmp_adspt,"\n指定包含角:",&tmp_jiaodu);
					CString str;
					CWnd *pwnd=acedGetAcadTextCmdLine();
					CWnd *pwnd2=pwnd->GetDlgItem(2);
					pwnd2->GetWindowText(str);

					if(str.Find('-')!=-1)
						last_bulge=-tan((2*PI-tmp_jiaodu)/4);
					else
						last_bulge=tan(tmp_jiaodu/4);
					m_pEnt->setBulgeAt(0,last_bulge);
				}
				else if(drawflag == 1)//CEnter
				{
					CEnter_flag = TRUE;
				}
				else if(drawflag == 2)//Angle
				{
					Angle_flag = TRUE;
				}
				else if(drawflag == 4)//Angle
				{
					ads_real tmp_jiaodu;
					ads_point tmp_adspt;
					AcGePoint2d tmp_pt;
					m_pEnt->getPointAt(0,tmp_pt);
					tmp_adspt[X]=tmp_pt.x;
					tmp_adspt[Y]=tmp_pt.y;
					acedInitGet(3,"");
					//acedGetAngle(tmp_adspt,"\nSpecify included angle:",&tmp_jiaodu);
					acedGetAngle(tmp_adspt,"\n指定包含角:",&tmp_jiaodu);
					CString str;
					CWnd *pwnd=acedGetAcadTextCmdLine();
					CWnd *pwnd2=pwnd->GetDlgItem(2);
					pwnd2->GetWindowText(str);

					if(str.Find('-')!=-1)
						last_bulge=-tan((2*PI-tmp_jiaodu)/4);
					else
						last_bulge=tan(tmp_jiaodu/4);
					m_pEnt->setBulgeAt(0,last_bulge);
					Angle_flag = TRUE;
				}
			}
			else//Arc
			{
				arcflag = TRUE;
			}
			break;

		case AcEdJig::kKW2:
			if(arcflag)
			{
				if(drawflag == 0)//CEnter
				{
					drawflag = 2;
					ads_point temp_adspt;
					//acedGetPoint(NULL,"\nSpecify center point of arc:",temp_adspt);
					acedGetPoint(NULL,"\n指定圆弧的圆心:",temp_adspt);
					m_refPoint.x=temp_adspt[X];
					m_refPoint.y=temp_adspt[Y];
					center.x=m_refPoint.x;		center.y=m_refPoint.y;
				}
				else if(drawflag == 1)//Radius
				{
					Radius_flag = TRUE;
					//acedGetDist(NULL,"\nSpecify radius of arc:",&radius);
					acedGetDist(NULL,"\n指定圆弧的半径:",&radius);
				}
				else if(drawflag == 2)//Length
				{
					Length_flag = TRUE;
					AcGePoint2d temp_pt;
					m_pEnt->getPointAt(0,temp_pt);
					m_refPoint.x=temp_pt.x;
					m_refPoint.y=temp_pt.y;
				}
			}
			else//Close
			{
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				if(ppline->numVerts()<2)
				{
					ppline->close();
					//acutPrintf("\nCannot close until two or more segments drawn.");
					acutPrintf("\n不能闭合.");
					break;
				}
				ppline->setClosed(Adesk::kTrue);
				numVerts = ppline->numVerts();
				ppline->close();
				setDispPrompt("\n命令:");
				status = AcEdJig::kCancel;
			}
			break;

		case AcEdJig::kKW3:
			if(arcflag)//CLose
			{
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				if(ppline->numVerts()<2)
				{
					ppline->close();
					//acutPrintf("\nCannot close until two or more segments drawn.");
					acutPrintf("\n不能闭合.");
					break;
				}
				ppline->setClosed(Adesk::kTrue);
				numVerts = ppline->numVerts();
				ppline->close();
				setDispPrompt("\n命令:");
				status = AcEdJig::kCancel;
			}
			else//Halfwidth
			{
				ads_point tmp_adspt;
				AcGePoint2d tmp_endpt2;
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				ppline->getPointAt(ppline->numVerts()-1,tmp_endpt2);
				ppline->close();
				tmp_adspt[X]=tmp_endpt2.x;
				tmp_adspt[Y]=tmp_endpt2.y;
				//sprintf(str,"\nSpecify starting half-width <%.3f>:",endWidth/2);
				sprintf(str,"\n指定起点半宽 <%.3f>:",endWidth/2);
				acedInitGet(64,"");
				if(acedGetDist(tmp_adspt,str,&startWidth) != RTNORM)
					startWidth = endWidth/2;
				//sprintf(str,"\nSpecify ending width <%.3f>:",startWidth);
				sprintf(str,"\n指定端点宽度 <%.3f>:",startWidth);
				acedInitGet(64,"");
				if(acedGetDist(tmp_adspt,str,&endWidth) != RTNORM)
					endWidth = startWidth;
				startWidth*=2;
				endWidth*=2;
				m_pEnt->setWidthsAt(m_pEnt->numVerts()-2,startWidth,endWidth);
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				ppline->setWidthsAt(ppline->numVerts()-1,startWidth,endWidth);
				ppline->close();
				startWidth = endWidth;
			}
			break;

		case AcEdJig::kKW4:
			if(arcflag)//Direction
			{
				drawflag = 3;
				AcGePoint2d temp_pt;
				ads_point temp_adspt;
				m_pEnt->getPointAt(0,temp_pt);
				temp_adspt[X]=temp_pt.x;	temp_adspt[Y]=temp_pt.y;
				//acedGetAngle(temp_adspt,"\nSpecify the tangent direction for the start point of arc:",&qiexian_jiaodu);
				acedGetAngle(temp_adspt,"\n指定圆弧的起点切向:",&qiexian_jiaodu);
			}
			else//Length
			{
				double tmp_length=0,tmp_bulge=0;
				AcGePoint2d tmp_endpt1,tmp_endpt2,next_pt;
				ads_point tmp_adspt;

				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				ppline->getPointAt(ppline->numVerts()-1,tmp_endpt2);
				ppline->getPointAt(ppline->numVerts()-2,tmp_endpt1);
				ppline->getBulgeAt(ppline->numVerts()-2,tmp_bulge);
				ppline->close();
				tmp_adspt[X]=tmp_endpt2.x;
				tmp_adspt[Y]=tmp_endpt2.y;
				acedInitGet(64,"");
				//acedGetDist(tmp_adspt,"\nSpecify length of line:",&tmp_length);
				acedGetDist(tmp_adspt,"\n指定直线的长度:",&tmp_length);
				//acutPrintf("\nThe length input is: %.3f ",tmp_length);
				double jiaodu=0;
				if(tmp_bulge == 0)//前一段是直线
				{
					AcGeVector2d Vector;
					jiaodu=tmp_endpt1.asVector().angleTo(tmp_endpt2.asVector());
					Vector=(tmp_endpt2.asVector()-tmp_endpt1.asVector())/tmp_endpt2.distanceTo(tmp_endpt1);//单位向量
					next_pt=tmp_endpt2+Vector*tmp_length;//应该新插入的点
					if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
					{
						setDispPrompt("\n命令:");
						status = AcEdJig::kCancel;
						break;
					}
					ppline->addVertexAt(ppline->numVerts(),next_pt,0,startWidth,endWidth);
					ppline->close();
					m_pEnt->removeVertexAt(0);
					m_pEnt->addVertexAt(0,next_pt,0,startWidth,endWidth);
					m_refPoint.x=next_pt.x;
					m_refPoint.y=next_pt.y;
				}
				else//前一段是弧
				{
				}
			}
			break;

		case AcEdJig::kKW5:
			if(arcflag)//Halfwidth
			{				
				ads_point tmp_adspt;
				AcGePoint2d tmp_endpt2;
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				ppline->getPointAt(ppline->numVerts()-1,tmp_endpt2);
				ppline->close();
				tmp_adspt[X]=tmp_endpt2.x;
				tmp_adspt[Y]=tmp_endpt2.y;
				//sprintf(str,"\nSpecify starting half-width <%.3f>:",endWidth/2);
				sprintf(str,"\n指定起点半宽 <%.3f>:",endWidth/2);
				acedInitGet(64,"");
				if(acedGetDist(tmp_adspt,str,&startWidth) != RTNORM)
					startWidth = endWidth/2;
				//sprintf(str,"\nSpecify ending width <%.3f>:",startWidth);
				sprintf(str,"\n指定端点宽度 <%.3f>:",startWidth);
				acedInitGet(64,"");
				if(acedGetDist(tmp_adspt,str,&endWidth) != RTNORM)
					endWidth = startWidth;
				startWidth*=2;
				endWidth*=2;
				m_pEnt->setWidthsAt(m_pEnt->numVerts()-2,startWidth,endWidth);
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				ppline->setWidthsAt(ppline->numVerts()-1,startWidth,endWidth);
				ppline->close();
				startWidth = endWidth;
			}
			else//undo
			{
				AcGePoint2d tmp_pt2d;
				double tmp_bulge,tmp_sWidth,tmp_eWidth;
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				if(ppline->numVerts()<2)
				{
					ppline->close();
					//acutPrintf("\nAll segments already undone.");
					acutPrintf("\n已经放弃所有线段。");
					break;
				}
				ppline->getPointAt(ppline->numVerts()-2,tmp_pt2d);
				ppline->getBulgeAt(ppline->numVerts()-2,tmp_bulge);
				ppline->getWidthsAt(ppline->numVerts()-2,tmp_sWidth,tmp_eWidth);
				ppline->removeVertexAt(ppline->numVerts()-1);
				ppline->close();
				m_pEnt->removeVertexAt(0);
				m_pEnt->addVertexAt(0,tmp_pt2d,tmp_bulge,tmp_sWidth,tmp_eWidth);
				m_refPoint.x=tmp_pt2d.x;
				m_refPoint.y=tmp_pt2d.y;
			}
			break;

		case AcEdJig::kKW6:
			if(arcflag)//Line
			{
				arcflag = FALSE;
				m_pEnt->setBulgeAt(0,0.0);
			}
			else//Width
			{
				ads_point tmp_adspt;
				AcGePoint2d tmp_endpt2;
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				ppline->getPointAt(ppline->numVerts()-1,tmp_endpt2);
				ppline->close();
				tmp_adspt[X]=tmp_endpt2.x;
				tmp_adspt[Y]=tmp_endpt2.y;

				//sprintf(str,"\nSpecify starting width <%.3f>:",endWidth);
				sprintf(str,"\n指定起点宽度 <%.3f>:",endWidth);
				acedInitGet(64,"");
				if(acedGetDist(tmp_adspt,str,&startWidth) != RTNORM)
					startWidth = endWidth;
				//sprintf(str,"\nSpecify ending width <%.3f>:",startWidth);
				sprintf(str,"\n指定端点宽度 <%.3f>:",startWidth);
				acedInitGet(64,"");
				if(acedGetDist(tmp_adspt,str,&endWidth) != RTNORM)
					endWidth = startWidth;
				m_pEnt->setWidthsAt(m_pEnt->numVerts()-2,startWidth,endWidth);
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				ppline->setWidthsAt(ppline->numVerts()-1,startWidth,endWidth);
				ppline->close();
				startWidth = endWidth;
			}
			break;

		case AcEdJig::kKW7:
			if(arcflag)//Radius
			{
				drawflag = 4;
				//acedGetDist(NULL,"\nSpecify radius of arc:",&radius);
				acedGetDist(NULL,"\n指定圆弧的半径:",&radius);
			}
			else
			{
			}
			break;

		case AcEdJig::kKW8:
			if(arcflag)//Second pt
			{
				drawflag = 5;
				ads_point tmp_adspt;
				//acedGetPoint(NULL,"\nSpecify second point on arc:",tmp_adspt);
				acedGetPoint(NULL,"\n指定圆弧上的第二个点:",tmp_adspt);
				second_pt.x=tmp_adspt[X];
				second_pt.y=tmp_adspt[Y];
			}
			else
			{
			}
			break;

		case AcEdJig::kKW9:
			if(arcflag)//Undo
			{
				AcGePoint2d tmp_pt2d;
				double tmp_bulge,tmp_sWidth,tmp_eWidth;
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				if(ppline->numVerts()<2)
				{
					ppline->close();
					//acutPrintf("\nAll segments already undone.");
					acutPrintf("\n已经放弃所有线段。");
					break;
				}
				ppline->getPointAt(ppline->numVerts()-2,tmp_pt2d);
				ppline->getBulgeAt(ppline->numVerts()-2,tmp_bulge);
				ppline->getWidthsAt(ppline->numVerts()-2,tmp_sWidth,tmp_eWidth);
				ppline->removeVertexAt(ppline->numVerts()-1);
				//acutPrintf("\nThe Vertexs number of ppline is: %d",ppline->numVerts());
				ppline->close();
				m_pEnt->removeVertexAt(0);
				m_pEnt->addVertexAt(0,tmp_pt2d,tmp_bulge,tmp_sWidth,tmp_eWidth);
				m_refPoint.x=tmp_pt2d.x;
				m_refPoint.y=tmp_pt2d.y;
				//acutPrintf("\nThe Vertexs number of m_pEnt is: %d",m_pEnt->numVerts());
			}
			else
			{
			}
			break;

		case 10:
			if(arcflag)//Width
			{
				ads_point tmp_adspt;
				AcGePoint2d tmp_endpt2;
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				ppline->getPointAt(ppline->numVerts()-1,tmp_endpt2);
				ppline->close();
				tmp_adspt[X]=tmp_endpt2.x;
				tmp_adspt[Y]=tmp_endpt2.y;

				//sprintf(str,"\nSpecify starting width <%.3f>:",endWidth);
				sprintf(str,"\n指定起点宽度 <%.3f>:",endWidth);
				acedInitGet(64,"");
				if(acedGetDist(tmp_adspt,str,&startWidth) != RTNORM)
					startWidth = endWidth;
				//sprintf(str,"\nSpecify ending width <%.3f>:",startWidth);
				sprintf(str,"\n指定端点宽度 <%.3f>:",startWidth);
				acedInitGet(64,"");
				if(acedGetDist(tmp_adspt,str,&endWidth) != RTNORM)
					endWidth = startWidth;
				m_pEnt->setWidthsAt(m_pEnt->numVerts()-2,startWidth,endWidth);
				if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
				{
					setDispPrompt("\n命令:");
					status = AcEdJig::kCancel;
					break;
				}
				ppline->setWidthsAt(ppline->numVerts()-1,startWidth,endWidth);
				ppline->close();
				startWidth = endWidth;
			}
			else
			{
			}
			break;

		case AcEdJig::kNull:
			if((drawflag == 1 && Radius_flag) || (drawflag == 4 && Angle_flag))
			{
				double dis=0,ang2=0;
				ads_point tmp_adspt,tmp_adsptr;
				AcGePoint3d tmp_endpt1;
				AcGePoint2d pt2d;
				if(Acad::eOk == m_pEnt->getPointAt(0,tmp_endpt1))
				{
					tmp_adspt[X]=tmp_endpt1.x;	tmp_adspt[Y]=tmp_endpt1.y;
					if(last_bulge>0)
					{
						ang2=2*atan(last_bulge);
					}
					else if(last_bulge<0)
					{
						ang2=2*atan(-last_bulge);
					}
					dis=2*radius*sin(ang2);
					acutPolar(tmp_adspt,qiexian_jiaodu,dis,tmp_adsptr);
					pt2d.x=tmp_adsptr[X];	pt2d.y=tmp_adsptr[Y];
					m_pEnt->removeVertexAt(1);
					m_pEnt->addVertexAt(1,pt2d,bulge,startWidth,endWidth);
					
					addflag--;
					AcGePoint2d tmp_pt2d;
					double tmp_bulge,tmp_sWidth,tmp_eWidth;
					m_pEnt->getPointAt(1,tmp_pt2d);
					m_pEnt->getBulgeAt(1,tmp_bulge);
					m_pEnt->getWidthsAt(1,tmp_sWidth,tmp_eWidth);
					if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
					{
						setDispPrompt("\n命令:");
						status = AcEdJig::kCancel;
						break;
					}
					ppline->addVertexAt(ppline->numVerts(),tmp_pt2d,tmp_bulge,tmp_sWidth,tmp_eWidth);
					m_pEnt->getBulgeAt(0,tmp_bulge);
					ppline->setBulgeAt(ppline->numVerts()-2,tmp_bulge);
					m_pEnt->removeVertexAt(0);
					ppline->close();
				}

				m_refPoint=m_LastPoint;

				drawflag = 0;
				if(Radius_flag)
				{
					Radius_flag = FALSE;
					m_pEnt->getPointAt(0,m_refPoint);
				}
				m_pEnt->getPointAt(0,m_refPoint);
				CEnter_flag = FALSE;
				Angle_flag = FALSE;
				Length_flag = FALSE;
				break;
			}
			if(addflag>0)
				m_pEnt->removeVertexAt(m_pEnt->numVerts()-1);
			result=TRUE;//返回值,标记有新生成的PLINE线
			setDispPrompt("\n命令:");
			status=AcEdJig::kCancel;
			acdbOpenObject(ppline,objId,AcDb::kForRead);
			numVerts = ppline->numVerts();
			ppline->close();
			break;

		case AcEdJig::kCancel:
			setDispPrompt("\n命令:");
			status=AcEdJig::kCancel;
			acdbOpenObject(ppline,objId,AcDb::kForWrite);
			ppline->erase();
			ppline->close();
			numVerts = 0;
			break;

		case AcEdJig::kOther:
			{
				if((drawflag == 1 && Radius_flag) || (drawflag == 4 && Angle_flag))
				{
					//DebugPrintf("\n111111");
					CString str;
					CWnd *pwnd=acedGetAcadTextCmdLine();
					CWnd *pwnd2=pwnd->GetDlgItem(2);
					pwnd2->GetWindowText(str);
					if(str.Find(':') != str.GetLength()-1)
					{
						str=str.Mid(str.Find(':')+1);
						
						double dis=0,ang1=0,ang2=0;
						ads_point tmp_adspt,tmp_adsptr;
						AcGePoint3d tmp_endpt1;
						AcGePoint2d pt2d;
						if(Acad::eOk ==  m_pEnt->getPointAt(0,tmp_endpt1))
						{
							tmp_adspt[X]=tmp_endpt1.x;	tmp_adspt[Y]=tmp_endpt1.y;
							if(last_bulge>0)
							{
								ang2=2*atan(last_bulge);
							}
							else if(last_bulge<0)
							{
								ang2=2*atan(-last_bulge);
							}
							dis=2*radius*sin(ang2);
							ang1=(fabs(atof(str))/180)*PI;
							//					acutPrintf("\n ang is %.3f",ang1);
							acutPolar(tmp_adspt,ang1,dis,tmp_adsptr);
							pt2d.x=tmp_adsptr[X];	pt2d.y=tmp_adsptr[Y];
							m_pEnt->removeVertexAt(1);
							m_pEnt->addVertexAt(1,pt2d,bulge,startWidth,endWidth);
							
							
							AcGePoint2d tmp_pt2d;
							double tmp_bulge,tmp_sWidth,tmp_eWidth;
							m_pEnt->getPointAt(1,tmp_pt2d);
							m_pEnt->getBulgeAt(1,tmp_bulge);
							m_pEnt->getWidthsAt(1,tmp_sWidth,tmp_eWidth);
							if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
							{
								setDispPrompt("\n命令:");
								status = AcEdJig::kCancel;
								break;
							}
							ppline->addVertexAt(ppline->numVerts(),tmp_pt2d,tmp_bulge,tmp_sWidth,tmp_eWidth);
							m_pEnt->getBulgeAt(0,tmp_bulge);
							ppline->setBulgeAt(ppline->numVerts()-2,tmp_bulge);
							m_pEnt->removeVertexAt(0);
							//				acutPrintf("\nThe Vertexs number of ppline and m_pEnt is: %d  %d",ppline->numVerts(),m_pEnt->numVerts());
							ppline->close();							
						}

						m_refPoint=m_LastPoint;
						
						drawflag = 0;
						if(Radius_flag)
						{
							Radius_flag = FALSE;
							m_pEnt->getPointAt(0,m_refPoint);
						}
						m_pEnt->getPointAt(0,m_refPoint);
						CEnter_flag = FALSE;
						Angle_flag = FALSE;
						Length_flag = FALSE;
						break;
					}
				}
				if(!ptflag)
				{
					//acutPrintf("\n*Valid*");
					//DebugPrintf("\n*Valid*");
					break;
				}
				if(arcflag)
				{
					//DebugPrintf("\narcflag = true");
					addflag--;
					AcGePoint2d tmp_pt2d;
					double tmp_bulge,tmp_sWidth,tmp_eWidth;
					if(Acad::eOk == m_pEnt->getPointAt(1,tmp_pt2d))
					{
						m_pEnt->getBulgeAt(1,tmp_bulge);
						m_pEnt->getWidthsAt(1,tmp_sWidth,tmp_eWidth);
						if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
						{
							setDispPrompt("\n命令:");
							status = AcEdJig::kCancel;
							break;
						}
						ppline->addVertexAt(ppline->numVerts(),tmp_pt2d,tmp_bulge,tmp_sWidth,tmp_eWidth);
						m_pEnt->getBulgeAt(0,tmp_bulge);
						ppline->setBulgeAt(ppline->numVerts()-2,tmp_bulge);
						m_pEnt->removeVertexAt(0);
						ppline->close();
					}
					m_refPoint=m_LastPoint;
				}
				else
				{
					//DebugPrintf("\narcflag = false");
					addflag--;
					AcGePoint2d tmp_pt2d;
					double tmp_bulge,tmp_sWidth,tmp_eWidth;
					if(Acad::eOk == m_pEnt->getPointAt(1,tmp_pt2d))
					{
						m_pEnt->getBulgeAt(1,tmp_bulge);
						m_pEnt->getWidthsAt(1,tmp_sWidth,tmp_eWidth);
						if(acdbOpenObject(ppline,objId,AcDb::kForWrite) != Acad::eOk)
						{
							setDispPrompt("\n命令:");
							status = AcEdJig::kCancel;
							break;
						}
						ppline->addVertexAt(ppline->numVerts(),tmp_pt2d,tmp_bulge,tmp_sWidth,tmp_eWidth);
						m_pEnt->getBulgeAt(0,tmp_bulge);
						ppline->setBulgeAt(ppline->numVerts()-2,tmp_bulge);
						m_pEnt->removeVertexAt(0);
						ppline->close();
					}
					m_refPoint=m_LastPoint;
				}
				//DebugPrintf("\n2222222222");
				drawflag = 0;
				if(CEnter_flag)
				{
					CEnter_flag = FALSE;//圆心方式输入的标记
				}
				if(Radius_flag)
				{
					Radius_flag = FALSE;
				}
				Angle_flag = FALSE;
				Length_flag = FALSE;
				m_pEnt->getPointAt(0,m_refPoint);
				break;
			}
		default:
			setDispPrompt("\n命令:");
			status=AcEdJig::kCancel;
			acdbOpenObject(ppline,objId,AcDb::kForRead);
			numVerts = ppline->numVerts();
			ppline->close();
			break;
		}
	}
	return result;
}

AcEdJig::DragStatus ARXPline::sampler()
{
    DragStatus stat;
	AcGePoint3d pt;
	if(arcflag)
		setSpecialCursorType(AcEdJig::kRubberBand);
	else
		setSpecialCursorType(AcEdJig::kCrosshair);//设置光标类型
	//控制输入方式
	setUserInputControls((UserInputControls)
		(kNullResponseAccepted|kAccept3dCoordinates | kDontUpdateLastPoint));
	if(arcflag)
	{
		if(drawflag == 0)
			setKeywordList("Angle CEnter CLose Direction Halfwidth Line Radius Second Undo Width");
		else if(drawflag == 1)
			setKeywordList("CEnter Radius");
		else if(drawflag == 2)
			setKeywordList("Angle Length");
		else if(drawflag == 3)
			setKeywordList("");
		else if(drawflag == 4)
			setKeywordList("Angle");
		else if(drawflag == 5)
			setKeywordList("");
	}
	else
	{
		setKeywordList("Arc Close Halfwidth Length Undo Width");
	}
	if(CEnter_flag)
		setSpecialCursorType(AcEdJig::kCrosshair);
	if(CEnter_flag)
		stat = acquirePoint(pt);
	else
		stat = acquirePoint(pt,m_refPoint);//返回用户的输入类型
	if(stat>0)//输入了关键字
	{
		return stat;
	}
	//////////////////////////////////////////////////////////////////////////
	static AcGePoint3d ptTemp ;
    if (ptTemp != pt)
        ptTemp = pt;
    else if (stat == AcEdJig::kNormal)
        return AcEdJig::kNoChange;
	//////////////////////////////////////////////////////////////////////////
	
    if (pt == m_LastPoint)      // if there is no work to be done,
	{
		if(stat == AcEdJig::kNormal)
		{
			return AcEdJig::kNoChange;
		}
		//DebugPrintf("\n --pt == m_LastPoint");
        return stat;          // exit early!
	}
    m_LastPoint = pt;
	
	if (stat == AcEdJig::kNormal)//得到正常输入的点
	{
		AcGePoint3d tmp_pt;
		if(addflag>0)
		{
			if(m_pEnt->removeVertexAt(m_pEnt->numVerts()-1) == Acad::eOk)//删除上一个临时点
			{
				addflag--;
			}
		}
		m_pEnt->getPointAt(m_pEnt->numVerts()-1,tmp_pt);
		if(fabs(tmp_pt.x-pt.x)<0.00001 && fabs(tmp_pt.y-pt.y)<0.00001)//判断两点是否重合，重合则返回
		{
			return AcEdJig::kNoChange;
		}
		AcGePoint2d pt2d(pt.x,pt.y);
		if(arcflag)
		{
			//DebugPrintf("\n---------------00000");
		}
		else
		{
			//DebugPrintf("\n---------------11111");
		}
		if(arcflag)//如果本段线是弧，计算凸度
		{
			Acad::ErrorStatus es;
			AcGePoint2d tmp_endpt1,tmp_endpt2;
			double tmp_bulge=0;//凸度
			double ang1,ang2,startang,endang;//
			//DebugPrintf("\n---drawflag ==== %d",drawflag);
			if(drawflag == 0)//默认的绘制圆弧的方式
			{
				
				if((es=acdbOpenObject(ppline,objId,AcDb::kForRead)) != Acad::eOk)
				{
					acutPrintf("\nOpen entity error:%d",es);
					return AcEdJig::kCancel;
				}
				ppline->getPointAt(ppline->numVerts()-1,tmp_endpt2);
				ppline->getPointAt(ppline->numVerts()-2,tmp_endpt1);
				ppline->getBulgeAt(ppline->numVerts()-2,tmp_bulge);
				ppline->close();
				//DebugPrintf("\n-------------tmp_bulge = %f",tmp_bulge);
				if(tmp_bulge == 0)//上一段线是直线
				{
					ang1=((tmp_endpt2.asVector()-tmp_endpt1.asVector())/tmp_endpt2.distanceTo(tmp_endpt1)).angle();
					ang2=((pt2d.asVector()-tmp_endpt2.asVector())/pt2d.distanceTo(tmp_endpt2)).angle();
					//acutPrintf("\nThe angle is %.2f and %.2f",ang1*180/PI,ang2*180/PI);
					if(fabs(ang2-ang1)<0.0001 || fabs(ang2-ang1-PI)<0.001 || fabs(ang2-ang1+PI)<0.001)//点在直线的方向上
					{
						//DebugPrintf("\n1----------ptflag = FALSE");
						ptflag = FALSE;
					}
					else
					{
						//DebugPrintf("\n1----------ptflag = TRUE");
						ptflag = TRUE;
					}
					while(ang1 > 2*PI) ang1-=2*PI;
					while(ang1 < 0) ang1+=2*PI;
					while(ang2 > 2*PI) ang2-=2*PI;
					while(ang2 < 0) ang2+=2*PI;
					if(ang2 < ang1) ang2+=2*PI;
					if(ang2>ang1 && ang2<(ang1+PI))//点在向量的左边
					{
						startang = ang1-PI/2;
						endang = (startang+PI)+2*(ang2-startang-PI);
						tmp_bulge = fabs(tan((endang-startang)/4));
					}
					else if(ang2>(ang1+PI) && ang2<(ang1+2*PI))//点在向量的右边
					{
						startang = ang1+PI/2;
						endang = (startang+PI)-2*(ang2-startang-PI);
						tmp_bulge = -fabs(tan((endang-startang)/4));
					}
					else
					{
						//acutPrintf("\nAngel count error!");
					}
					m_pEnt->setBulgeAt(0,tmp_bulge);
				}
				else//上一段线是弧
				{
					AcGeCircArc2d arc2d(tmp_endpt1,tmp_endpt2,tmp_bulge,Adesk::kFalse);
					if(tmp_bulge>0)
					{
						ang1=((tmp_endpt2.asVector()-arc2d.center().asVector())/arc2d.radius()).angle()+PI/2;
					}
					else
					{
						ang1=((tmp_endpt2.asVector()-arc2d.center().asVector())/arc2d.radius()).angle()-PI/2;
					}
					ang2=((pt2d.asVector()-tmp_endpt2.asVector())/pt2d.distanceTo(tmp_endpt2)).angle();
					if(fabs(ang2-ang1)<0.0001 || fabs(ang2-ang1-PI)<0.001 || fabs(ang2-ang1+PI)<0.001)//点在直线的方向上
					{
						//DebugPrintf("\n2----------ptflag = FALSE");
						ptflag =FALSE;
					}
					else 
					{
						ptflag =TRUE;
					}
					while(ang1 > 2*PI) ang1-=2*PI;
					while(ang1 < 0) ang1+=2*PI;
					while(ang2 > 2*PI) ang2-=2*PI;
					while(ang2 < 0) ang2+=2*PI;
					if(ang2 < ang1) ang2+=2*PI;
					if(ang2>ang1 && ang2<(ang1+PI))//点在向量的左边
					{
						startang = ang1-PI/2;
						endang = (startang+PI)+2*(ang2-startang-PI);
						tmp_bulge = fabs(tan((endang-startang)/4));
					}
					else if(ang2>(ang1+PI) && ang2<(ang1+2*PI))//点在向量的右边
					{
						startang = ang1+PI/2;
						endang = (startang+PI)-2*(ang2-startang-PI);
						tmp_bulge = -fabs(tan((endang-startang)/4));
					}
					else
					{
						//acutPrintf("\nAngel count error:ang2:%.3f   ang1:%.3f",ang2*180/PI,ang1*180/PI);
					}
					m_pEnt->setBulgeAt(0,tmp_bulge);
				}
			}
			else if(drawflag == 1)
			{
				m_pEnt->setBulgeAt(0,last_bulge);
				if(CEnter_flag)//圆心方式输入的标记
				{
					m_pEnt->getPointAt(0,tmp_endpt1);
					ang1=((tmp_endpt1.asVector()-pt2d.asVector())/pt2d.distanceTo(tmp_endpt1)).angle();
					while(ang1 > 2*PI) ang1-=2*PI;
					while(ang1 < 0) ang1+=2*PI;
					if(last_bulge>0)
					{
						ang2=ang1+4*atan(last_bulge);
					}
					else if(last_bulge<0)
					{
						ang2=ang1-4*atan(-last_bulge);
					}
					else
					{
						//acutPrintf("\nBulge error!");
					}
					while(ang2 > 2*PI) ang2-=2*PI;
					while(ang2 < 0) ang2+=2*PI;
					double dis=0;
					ads_point tmp_adspt,tmp_adsptr;
					dis=pt2d.distanceTo(tmp_endpt1);
					tmp_adspt[X]=pt.x;	tmp_adspt[Y]=pt.y;
					acutPolar(tmp_adspt,ang2,dis,tmp_adsptr);
					pt2d.x=tmp_adsptr[X];	pt2d.y=tmp_adsptr[Y];
				//	m_LastPoint.x=pt2d.x;	m_LastPoint.y=pt2d.y;
				}
				if(Radius_flag)//半径方式输入的标记
				{
					double dis=0;
					ads_point tmp_adspt,tmp_adsptr;
					m_pEnt->getPointAt(0,tmp_endpt1);
					tmp_adspt[X]=tmp_endpt1.x;	tmp_adspt[Y]=tmp_endpt1.y;
					ang1=((pt2d.asVector()-tmp_endpt1.asVector())/pt2d.distanceTo(tmp_endpt1)).angle();
					if(last_bulge>0)
					{
						ang2=2*atan(last_bulge);
					}
					else if(last_bulge<0)
					{
						ang2=2*atan(-last_bulge);
					}

					dis=2*radius*sin(ang2);
					acutPolar(tmp_adspt,ang1,dis,tmp_adsptr);
					pt2d.x=tmp_adsptr[X];	pt2d.y=tmp_adsptr[Y];
				}
			}
			else if(drawflag == 2)
			{
				double dis=0;
				ads_point tmp_adspt,tmp_adsptr;
				m_pEnt->getPointAt(0,tmp_endpt1);
				if(Angle_flag)
				{
					ang1=((tmp_endpt1.asVector()-center.asVector())/tmp_endpt1.distanceTo(center)).angle();
					ang2=((pt2d.asVector()-center.asVector())/pt2d.distanceTo(center)).angle();
					while(ang1 > 2*PI) ang1-=2*PI;
					while(ang1 < 0) ang1+=2*PI;
					while(ang2 > 2*PI) ang2-=2*PI;
					while(ang2 < 0) ang2+=2*PI;
					tmp_bulge=tan(ang2/4);
					m_pEnt->setBulgeAt(0,tmp_bulge);
					dis=tmp_endpt1.distanceTo(center);
					tmp_adspt[X]=center.x;	tmp_adspt[Y]=center.y;
					acutPolar(tmp_adspt,ang1+ang2,dis,tmp_adsptr);
					pt2d.x=tmp_adsptr[X];	pt2d.y=tmp_adsptr[Y];
				}
				else if(Length_flag)
				{
					double tmp_dis=0;
					ang1=((tmp_endpt1.asVector()-center.asVector())/tmp_endpt1.distanceTo(center)).angle();
					tmp_dis=pt2d.distanceTo(tmp_endpt1);//弦长
					dis=tmp_endpt1.distanceTo(center);//半径
					if(tmp_dis > 2*dis)
						tmp_dis = 2*dis;
			//		acutPrintf("\n Length is %.3f",tmp_dis);
					ang2=2*asin((tmp_dis/2)/dis);//弧的角度
					while(ang1 > 2*PI) ang1-=2*PI;
					while(ang1 < 0) ang1+=2*PI;
					while(ang2 > 2*PI) ang2-=2*PI;
					while(ang2 < 0) ang2+=2*PI;
					tmp_bulge = tan(ang2/4);
					m_pEnt->setBulgeAt(0,tmp_bulge);
					tmp_adspt[X]=center.x;	tmp_adspt[Y]=center.y;
					acutPolar(tmp_adspt,ang1+ang2,dis,tmp_adsptr);
					pt2d.x=tmp_adsptr[X];	pt2d.y=tmp_adsptr[Y];
				}
				else
				{					
					ang1=((tmp_endpt1.asVector()-center.asVector())/tmp_endpt1.distanceTo(center)).angle();
					ang2=((pt2d.asVector()-center.asVector())/pt2d.distanceTo(center)).angle();
					while(ang1 > 2*PI) ang1-=2*PI;
					while(ang1 < 0) ang1+=2*PI;
					while(ang2 > 2*PI) ang2-=2*PI;
					while(ang2 < 0) ang2+=2*PI;
					if(ang2 < ang1) ang2+=2*PI;
					tmp_bulge=tan((ang2-ang1)/4);
					m_pEnt->setBulgeAt(0,tmp_bulge);
					dis=tmp_endpt1.distanceTo(center);
					tmp_adspt[X]=center.x;	tmp_adspt[Y]=center.y;
					acutPolar(tmp_adspt,ang2,dis,tmp_adsptr);
					pt2d.x=tmp_adsptr[X];	pt2d.y=tmp_adsptr[Y];
				}
			}
			else if(drawflag == 3)
			{
				m_pEnt->getPointAt(0,tmp_endpt2);
				ang1=qiexian_jiaodu;
				ang2=((pt2d.asVector()-tmp_endpt2.asVector())/pt2d.distanceTo(tmp_endpt2)).angle();
				if(fabs(ang2-ang1)<0.0001 || fabs(ang2-ang1-PI)<0.001 || fabs(ang2-ang1+PI)<0.001)//点在直线的方向上
				{
					//DebugPrintf("\n3----------ptflag = FALSE");
					ptflag = FALSE;
				}
				else
				{
					ptflag = TRUE;
				}
				while(ang1 > 2*PI) ang1-=2*PI;
				while(ang1 < 0) ang1+=2*PI;
				while(ang2 > 2*PI) ang2-=2*PI;
				while(ang2 < 0) ang2+=2*PI;
				if(ang2 < ang1) ang2+=2*PI;
				if(ang2>ang1 && ang2<(ang1+PI))//点在向量的左边
				{
					startang = ang1-PI/2;
					endang = (startang+PI)+2*(ang2-startang-PI);
					tmp_bulge = fabs(tan((endang-startang)/4));
				}
				else if(ang2>(ang1+PI) && ang2<(ang1+2*PI))//点在向量的右边
				{
					startang = ang1+PI/2;
					endang = (startang+PI)-2*(ang2-startang-PI);
					tmp_bulge = -fabs(tan((endang-startang)/4));
				}
				//				acutPrintf("\nThe start and end angle is %.2f and %.2f",startang*180/PI,endang*180/PI);
				m_pEnt->setBulgeAt(0,tmp_bulge);
			}
			else if(drawflag == 4)
			{
				double tmp_dis=0;
				ads_point tmp_adspt,tmp_adsptr;
				m_pEnt->getPointAt(0,tmp_endpt1);
				
				if(Angle_flag)
				{
					m_pEnt->setBulgeAt(0,last_bulge);
					double dis=0;
					ads_point tmp_adspt,tmp_adsptr;
					m_pEnt->getPointAt(0,tmp_endpt1);
					tmp_adspt[X]=tmp_endpt1.x;	tmp_adspt[Y]=tmp_endpt1.y;
					ang1=((pt2d.asVector()-tmp_endpt1.asVector())/pt2d.distanceTo(tmp_endpt1)).angle();
					if(last_bulge>0)
					{
						ang2=2*atan(last_bulge);
					}
					else if(last_bulge<0)
					{
						ang2=2*atan(-last_bulge);
					}
					dis=2*radius*sin(ang2);
					acutPolar(tmp_adspt,ang1,dis,tmp_adsptr);
					pt2d.x=tmp_adsptr[X];	pt2d.y=tmp_adsptr[Y];
				}
				else
				{
					ang1=((pt2d.asVector()-tmp_endpt1.asVector())/tmp_endpt1.distanceTo(pt2d)).angle();
					tmp_dis=pt2d.distanceTo(tmp_endpt1);//弦长
					if(tmp_dis > 2*radius)
						tmp_dis = 2*radius;
			//		acutPrintf("\n Length is %.3f",tmp_dis);
					ang2=2*asin((tmp_dis/2)/radius);//弧的角度
					while(ang1 > 2*PI) ang1-=2*PI;
					while(ang1 < 0) ang1+=2*PI;
					while(ang2 > 2*PI) ang2-=2*PI;
					while(ang2 < 0) ang2+=2*PI;
					tmp_bulge = tan(ang2/4);
					m_pEnt->setBulgeAt(0,tmp_bulge);
					tmp_adspt[X]=tmp_endpt1.x;	tmp_adspt[Y]=tmp_endpt1.y;
					acutPolar(tmp_adspt,ang1,tmp_dis,tmp_adsptr);
					pt2d.x=tmp_adsptr[X];	pt2d.y=tmp_adsptr[Y];
				}
			}
			else if(drawflag == 5)
			{
				m_pEnt->getPointAt(0,tmp_endpt1);
				AcGeCircArc2d arc2d(tmp_endpt1,second_pt,pt2d);
				ang1 = ((second_pt.asVector()-tmp_endpt1.asVector())/tmp_endpt1.distanceTo(second_pt)).angle();
				ang2 = ((pt2d.asVector()-second_pt.asVector())/pt2d.distanceTo(second_pt)).angle();
				//lxf add 2003/9/29--start
				if(fabs(ang2-ang1)<0.0001 || fabs(ang2-ang1-PI)<0.001 || fabs(ang2-ang1+PI)<0.001)//点在直线的方向上
				{
					//DebugPrintf("\n5----------ptflag = FALSE");
					ptflag = FALSE;
				}
				else
				{
					//DebugPrintf("\n5----------ptflag = TRUE");
					ptflag = TRUE;
				}
				//lxf add 2003/9/29--end
				startang = ((tmp_endpt1.asVector()-arc2d.center().asVector())/arc2d.radius()).angle();
				endang = ((pt2d.asVector()-arc2d.center().asVector())/arc2d.radius()).angle();
			//	startang = ((arc2d.startPoint().asVector()-arc2d.center().asVector())/arc2d.radius()).angle();
			//	endang = ((arc2d.endPoint().asVector()-arc2d.center().asVector())/arc2d.radius()).angle();
				while(ang1 > 2*PI) ang1-=2*PI;
				while(ang1 < 0) ang1+=2*PI;
				while(ang2 > 2*PI) ang2-=2*PI;
				while(ang2 < 0) ang2+=2*PI;
				if(ang2 < ang1) ang2+=2*PI;
				while(startang > 2*PI) startang-=2*PI;
				while(startang < 0) startang+=2*PI;
				while(endang > 2*PI) endang-=2*PI;
				while(endang < 0) endang+=2*PI;
				if(ang2>ang1 && ang2<(ang1+PI))//点在向量的左边
				{
					if(endang < startang) endang+=2*PI;
					tmp_bulge = fabs(tan((endang-startang)/4));
				}
				else //点在向量的右边
				{
					if(startang < endang) startang+=2*PI;
					tmp_bulge = -fabs(tan((startang-endang)/4));
				}
				m_pEnt->setBulgeAt(0,tmp_bulge);
			}
		}
		else
		{
			ptflag = TRUE;
		}
		if(m_pEnt->addVertexAt(m_pEnt->numVerts(),pt2d,bulge,startWidth,endWidth) == Acad::eOk)//添加新的临时点
		{
			addflag++;
		}
//		m_LastPoint.x=pt2d.x;	m_LastPoint.y=pt2d.y;
	}
    return stat;
}

Adesk::Boolean ARXPline::update()
{
	return Adesk::kFalse;
}

AcDbEntity* ARXPline::entity() const
{

//	m_pEnt->setColorIndex(ColorIndex);//设置拖动线的颜色号
	return m_pEnt;
}

void ARXPline::SetColorIndex(Adesk::UInt16 color)
{
	ColorIndex = color;
}

AcDbObjectId ARXPline::getObjectId()
{
	return objId;
}
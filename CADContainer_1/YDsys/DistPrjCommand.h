///////////////////////////////////////////////////////////////////////////////
//�Ϻ�����ϵͳ�������޹�˾ӵ�б�����֮��ȫ��Ȩ��δ���Ϻ�����ϵͳ�������޹�˾ //
//����ȷ������ɣ����ø��ơ�ȡ�á��ַ���Դ���롣                             //
//�κε�λ�͸�����δ����������£��������κ���ʽ���ơ��޸ĺͷ������������ //
//�β��֣�������Ϊ�Ƿ��ֺ����ҹ�˾����������׷�������ε�Ȩ������δ������ //
//����£��κ���֯����˷����Ĳ�Ʒ������ʹ��Dist�ı�־��Logo��               //
//                                                                           //
//support@dist.com.cn                                                        //
//www.dist.com.cn                                                            //
//                                                                           //
//���� : ����                                                                //
//                                                                           //
// ��Ȩ���� (C) 2007��2010 Dist Inc. ��������Ȩ����                          //
///////////////////////////////////////////////////////////////////////////////

//�ļ�˵�������ļ���ֻ����ִ�����������Ҫ�����ȫ�ֱ���
#ifndef  __DIST__PRJ__COMMAND__H__
#define  __DIST__PRJ__COMMAND__H__
#include "DistDocManagerReactor.h"

//�������Կ�
//void gCreateBlockAttr();

// �������ܣ�������Ŀ���߽߱�
// ʵ�ֲ��裺
// ������ע��
void gCmdCreateLXBound(CString strCadLyName,int nColorIndex,double dWidth,CString strSdeLyName);

// ���ܣ�������������
// ������
// ���أ�
void gCmdInputLXProperty(CString strCadLyName,CString strSdeLyName,CString strURL,CString strXMBH,CString strPrePrjId);
void gTestInput(CString strCadLyNameDK,CString strCadLyNameXM,
				CString strSdeLyNameDK,CString strSdeLyNameXM,CString strURL,CString strProcessId);

void gCmdSearchXMXX(CString strCadLyName,CString strSdeLyName);

// ���ܣ��������������ע(���α������е�)
// ������
// ���أ�
void gCmdCreatLabelAll(CString strCadLyName);


// ���ܣ��������������ע
// ������
// ���أ�
void gCmdCreatLabel(CString strCadLyName);

void gCmdDeleteLabel(CString strCadLyName);


// ���ܣ�����ָ��ͼ�����ƶ�SDEͼ������
// ������
// ���أ�
void gCmdReadSdeLys(CStringArray& strLyArray,/*int* pAccessPower,*/CString strCadLXName,CString strXMBH/*,CString strXZQH*/);

// ���ܣ�����ָ��ͼ�����ƺͷ�Χ��SDEͼ������
// ������
// ���أ�
void gCmdReadSdeLayerByRect(CString strSDELayerName,CString strMINX,CString strMAXX,CString strMINY,CString strMAXY,CString strXMBH);

//
void gCmdSaveCurXMObjectInfo(CString strCadLyName,const char* strCurXMBH);


void gCmdSaveBackDwg(CDistCommandManagerReactor* pCmdReactor);

void gCmdOpenBackDwg();

void gShowOrHideCadLayer(CString strCadLyName,BOOL bIsShow);

BOOL gReadCfgLyNameArray(CStringArray& strLyNameArray,CString strTag);

BOOL gReadSdeByRect(CString strSdeLyName,CString strXZQH,double& d_maxX,double& d_maxY,double& d_minX,double& d_minY);


//����ָ���ľ�����϶����
BOOL gTurnArc(AcDbObjectId& OutObjId,AcDbObjectId InObjId,char* strXDataRegName,int nMaxNum,double dPrecision);

//��������������Բ�뾶��Բ��
bool gGetCenterAndRadiusWith3Pt(double& r,double& a,double& b,double x0,double y0,double x1,double y1,double x2,double y2);

//�������,�յ�,Բ�ĺͰ뾶����͹��
double gCalcBlg(double centX,double centY,double fromX,double fromY,double toX,double toY,double dR);
BOOL  ReadCfgLyNameArray(CStringArray& strLyNameArray,CString strTag);

#endif
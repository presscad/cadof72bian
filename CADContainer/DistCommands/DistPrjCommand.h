//�ļ�˵�������ļ���ֻ����ִ�����������Ҫ�����ȫ�ֱ���
#ifndef  __DIST__PRJ__COMMAND__H__
#define  __DIST__PRJ__COMMAND__H__
#include "DistBasePara.h"

// �������ܣ�������Ŀ���߽߱�
// ʵ�ֲ��裺
// ������ע��
void gCmdCreateLXBound(CString strCadLyName,int nColorIndex,double dWidth);

// ���ܣ�������������
// ������
// ���أ�
void gCmdInputLXProperty(CString strCadLyName,CString strSdeLyName,HWND hDapHwnd,CString strURL,CString strXMBH);


void gCmdSearchXMXX(CString strCadLyName,CString strSdeLyName);

// ���ܣ��������������ע(���α������е�)
// ������
// ���أ�
void gCmdCreatLabelAll(CString strCadLyName);


// ���ܣ��������������ע
// ������
// ���أ�
void gCmdCreatLabel(CString strCadLyName);


// ���ܣ�����ָ��ͼ�����ƺͷ���Ȩ�޶�SDEͼ������
// ������
// ���أ�
void gCmdReadSdeLys(CStringArray& strLyArray,int* pAccessPower,CString strCadLXName,CString strXMBH);


//
void gCmdSaveCurXMObjectInfo(CString strCadLyName,const char* strCurXMBH);


void gCreateLXBlkDef(char* strCadName,char* strSdeName,double dmaxx,double dmaxy,double dminx,double dminy);

void gCmdChangeOutState();
void gOutRunInfo (long rc, char *szComment);
#endif
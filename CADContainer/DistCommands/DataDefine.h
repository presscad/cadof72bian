#ifndef  _DATADEFINE_H_
#define  _DATADEFINE_H_

#include "StdAfx.h"

// CAD���ṹ���ñ�
struct STB_CONTENT_TREE
{
	long nId;
	long nOwnerId;
	CString strNodeName;
	CString strSdeLyName;
	CString strPosOrder;
	BOOL bIsLocalLayer;
	BOOL bIsReadBefore;

	STB_CONTENT_TREE()
	{
		nId = -1;
		nOwnerId = -1;
		strNodeName = "";
		strSdeLyName = "";
		strPosOrder = "";
		bIsLocalLayer = TRUE;
		bIsReadBefore = FALSE; // �Ƿ��ȡ��
	}
};

// SDE���Ӳ���
struct ST_SDE_CONNECTION
{
	CString strServer;
	CString strService;
	CString strDatabase;
	CString strUserName;
	CString strPassword;
};

// FTP���Ӳ���
struct ST_FTP_CONNECTION
{
	CString strServer;
	CString strUserName;
	CString strPassword;
	WORD	nPort;

	ST_FTP_CONNECTION()
	{
		strServer = "127.0.0.1";
		strUserName = "guest";
		strPassword = "";
		nPort = 21;
	}
};

// ҵ����Ϣ
struct ST_BIZZINFO
{
	CString strBizzCode; // ��Ŀ���
	CString strBizzSame; // �������
	CString strBizzName; // ��Ŀ����
	CString strBzOwner; // ���赥λ
	CString strBzIdId; // ҵ������
	CString strBzId; // ҵ������
	CString strBztmOver; // ����ʱ��
	CString strCurTime; // ��������ǰʱ��

	CString strUserId; // �û�ID
	CString strLoginUserName; // ��¼�û���
};

// ͼ����Ϣ
struct ST_LAYERINFO
{
	// ��ע������Ϣ
	CString strAnnotionLine; // ��ע�߲�
	CString strAnnotionText; // ��ע���
	
	// ��Χͼ������
	CString strLayer; // ��Χͼ����
	CString strField; // ��Ŀ����ֶ���

	// ����ͼͼ��������Ϣ
	CString strMapLayer; // ����ͼ����
	CString strIndexField; // �����ֶ���
};


struct POWER 
{
	long nPower; // 0������ 1��ֻ�� 2����д 3������
	long nPrior; // ֵԽ�����ȼ�Խ��
};

typedef map<long, long> MapLong2Long;

typedef map<CString, CString> MapString2String;

typedef map<long, POWER> MapLong2Power;

typedef map<CString, POWER> MapString2Power;

typedef CArray<STB_CONTENT_TREE, STB_CONTENT_TREE> ArrayLayerTree;

typedef map<CString, STB_CONTENT_TREE> MapString2ContentTree;

#endif
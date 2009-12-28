#pragma once
#include "StdAfx.h"
#include <sdeerno.h>
#include <sdetype.h>
#include <sderaster.h>
#include <pe.h>
#include <pedef.h>
#include <pef.h>

#ifdef DIST_CONNECTION_EXPORTS
#define DIST_CONNECTION_API __declspec(dllexport)
#else
#define DIST_CONNECTION_API __declspec(dllimport)
#endif

//���������������
typedef enum enC_KIND
{
	C_SDE	=  1,	//SDE
	C_MIF	=  2,	//MIF
	C_SHAPE	=  3,	//SHAPE
}C_KIND;

//============================== �������� ===================================//

//���ݴ����ŷ��ش�����ʾ
DIST_CONNECTION_API char* WINAPI ReturnErrorCodeInfo(long nErrorCode);

class CDistConnection
{
public:
	CDistConnection(void);
	~CDistConnection(void);

public:
	C_KIND m_enType;
	
	virtual long CheckConnected();

	// ���ܣ���������
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0
	virtual long Connect();

	// ���ܣ��Ͽ�����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0
	virtual long Disconnect();
};

class CDistConnectSDE :
	public CDistConnection
{
public:
	CDistConnectSDE(void);
	~CDistConnectSDE(void);

private:
	//���ݿ����Ӷ���ָ��
	SE_CONNECTION m_pConnectObj;
	char m_strDBName[SE_MAX_DATABASE_LEN];
	char m_strUserName[SE_MAX_OWNER_LEN];

public:

	// ���ܣ����ݷ��ش�����ʾ��Ӧ��Ϣ
	// ������ rc          ��������ֵ
	//        szComment   ���ú�������
	// ���أ�
	static void check_error (long rc, char *strComment);

	// ���ܣ���ȡ���ݿ����Ӷ���ָ��
	// ��������
	// ���أ����ض���ָ��
	SE_CONNECTION GetConnectObj(void);

	virtual long CheckConnected(int IntervalTime=60);

	// ���ܣ���������
	// ������strServer    ����������
	//       strInstance  ����ʵ���������
	//       strDatabase  ���ݿ�����
	//       strUserName  �û�����
	//       strPassword  �û�����
	// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο������е�DIST_ENUM_CONNECTRESULT
	virtual long Connect(const char* strServer,      
		                 const char* strInstance,
		                 const char* strDatabase, 
						 const char* strUserName,
					     const char* strPassword);

	// ���ܣ��Ͽ�����
	// ��������
	// ���أ���
	virtual long Disconnect(void);
};

class CDistConnectMIF :
	public CDistConnection
{
public:
	CDistConnectMIF(void);
	~CDistConnectMIF(void);
};

class CDistConnectSHAPE :
	public CDistConnection
{
public:
	CDistConnectSHAPE(void);
	~CDistConnectSHAPE(void);
};

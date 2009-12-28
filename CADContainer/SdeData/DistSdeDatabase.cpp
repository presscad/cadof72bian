#include "stdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> 
#include <string.h>
#include <time.h>

#include <sdeerno.h>
#include <sdetype.h>
#include <sderaster.h>
#include <pe.h>
#include <pedef.h>
#include <pef.h>

#include <locale.h>

#include "DistBaseInterface.h"
#include "DistSdeDatabase.h"
#include "DistIntArray.h"

IDistConnection* g_pConnect=NULL;
IDistCommand*    g_pCommand=NULL;

//================================== �������� =======================================//

// ���ܣ��������ݿ����Ӷ���
// ��������
// ���أ����ض���ָ��
IDistConnection* WINAPI CreateSDEConnectObjcet()
{
	return (IDistConnection*) new CSdeConnection;
}


// ���ܣ����������������
// ��������
// ���أ����ض���ָ��
IDistCommand*  WINAPI CreateSDECommandObjcet()
{
	return (IDistCommand*) new CSdeCommand;
}

extern "C"
{
	int WINAPI gBeginTrans(char* strConnectString)
	{
		if(NULL==strConnectString) return 0;
		char strConn[5][255]={0};
		int nPos=0;
		int nLen = strlen(strConnectString);
		int nStart=0;
		for(int i=0; i<nLen;i++)
		{
			if(strConnectString[i]==',')
			{
				memcpy(strConn[nPos],strConnectString+nStart,i-nStart);
				nPos++; nStart = i+1;
			}
		}
		if(nPos != 5) return 0;  //����������

		if(g_pConnect == NULL)
		{
			g_pConnect = CreateSDEConnectObjcet();
			if(NULL == g_pConnect) return 0;
			if(1 != g_pConnect->Connect(strConn[0],strConn[1],strConn[2],strConn[3],strConn[4]))
			{
				g_pConnect->Release(); 
				g_pConnect = NULL;
				return 0;
			}
		}

		if(g_pCommand ==NULL)
		{
			g_pCommand = CreateSDECommandObjcet();
			if(g_pCommand==NULL)
			{
				g_pConnect->Release(); 
				g_pConnect = NULL;
				return 0;
			}
			g_pCommand->SetConnectObj(g_pConnect);
		}


		if(g_pCommand->BeginTrans()!=1)
		{
			g_pCommand->Release(); g_pCommand = NULL;
			g_pConnect->Release(); g_pConnect = NULL;
			return 0;
		}

		return 1;
	}

	int WINAPI gCommitTrans()
	{
		if(g_pConnect == NULL) return 0;
		if(g_pCommand == NULL)
		{
			g_pConnect->Release(); g_pConnect=NULL; return 0;
		}
		g_pCommand->CommitTrans();

		g_pCommand->Release(); g_pCommand = NULL;
		g_pConnect->Release(); g_pConnect = NULL;

		return 1;
	}


	int WINAPI gRollbackTrans()
	{
		if(g_pConnect == NULL) return 0;
		if(g_pCommand == NULL)
		{
			g_pConnect->Release(); g_pConnect=NULL; return 0;
		}
		g_pCommand->RollbackTrans();

		g_pCommand->Release(); g_pCommand = NULL;
		g_pConnect->Release(); g_pConnect = NULL;

		return 1;
	}

	int WINAPI gDeleteRecord(char* strSdeName,char* strWhere)
	{
		if((NULL==strSdeName)||(NULL==strWhere)) return 0;

		char strSQL[1024]={0};
		sprintf(strSQL,"T:%s,W:%s,##",strSdeName,strWhere);

		return g_pCommand->DeleteData(strSQL);	
	}


	int WINAPI gCopyRecordFromOneToOther(const char* strSdeLyNameSrc,const char* strSdeLyNameTrg,const char* strWhere, bool bAddVersion, const char* strUserName, const char* strCurTime)
	{
		int ret = 0;
		if((NULL==strSdeLyNameSrc)|| (NULL==strSdeLyNameTrg)||(NULL==strWhere)) 
			return 0;

		ret = g_pCommand->CopyRecordByCondition(strSdeLyNameTrg,strSdeLyNameSrc,strWhere, bAddVersion, strUserName, strCurTime);

		return ret;
	}

	//FldName1,FldName2,...,FldNameN,
	//FldValue1,FldValue2,...,FldValueN,
	int WINAPI gUpdateCADInfo(char* strSdeName,char* strWhere,char* strFldNames,char* strFldValues)
	{
		if((NULL == strSdeName)|| (NULL == strFldNames)||
			(NULL == strWhere) || (NULL == strFldValues)) return 0;

		int nCount = 0;
		int nPos=0;
		char ch = strFldValues[nPos];
		while(ch != 0)
		{
			if(ch == ',')
				nCount++;
			nPos++;
			ch = strFldValues[nPos];
		}
		if(nCount == 0) return 0;

		//���ٿռ�
		char** strValueArray =  new char*[nCount];
		for(int i=0; i<nCount;i++)
		{
			strValueArray[i] = new char[60];
			strValueArray[i][0] = 0;
//			memset(strValueArray[i],0,sizeof(char)*60);
		}

		nPos = 0; nCount =0;
		ch = strFldValues[nPos];
		int nStart = 0;
		while(ch != 0)
		{
			if(ch == ',')
			{
				memcpy(strValueArray[nCount],strFldValues+nStart,nPos-nStart);
				nStart = nPos+1;
				nCount++;
			}
			nPos++;
			ch = strFldValues[nPos];
		}


		char strSQL[1024]={0};
//		memset(strSQL,0,sizeof(char)*1024);
		sprintf(strSQL,"F:%sT:%s,W:%s,##",strFldNames,strSdeName,strWhere);

		long rc = g_pCommand->UpdateDataAsString(strSQL,strValueArray);

		for(int j=0; j<nCount; j++)
		{
			delete[] strValueArray[j];
		}
		delete [] strValueArray;

		return rc;
	}
}
//====================================ȫ�ֺ���=======================================//

// ���ܣ��Ƚ������ַ���
// ������strOne   �ַ���A
//       strTwo   �ַ���B
//���أ�  0��ʾA=B����ֵ��ʾA>B����ֵ��ʾA<B
static int CompareString(const char* strOne,const char* strTwo)
{
	return strcmpi(strOne, strTwo);
/*
	char strA[1024];
	char strB[1024];
	memset(strA,0,sizeof(char)*1024);
	memset(strB,0,sizeof(char)*1024);
	strcpy(strA,strOne); strupr(strA);
	strcpy(strB,strTwo); strupr(strB);

	return strcmp(strA,strB);
*/
}

//���ڸ�ʽ��****-**-**
static BOOL StringToDate(tm& tempDate,char* strDateBuf)
{
	tempDate.tm_year = 0;
	tempDate.tm_mday = 0;
	tempDate.tm_mon = 0;
	if(strDateBuf == NULL) return FALSE;
	char strData[100]={0};
	//	memset(strData,0,sizeof(char)*100);
	strcpy(strData,strDateBuf);
	int nLen = strlen(strData);
	if(nLen==0) return FALSE;
	if(nLen < 8 || nLen > 10) return FALSE;
	int nPos =0;
	int nNum = 0;
	char strBuf[20]={0};
	//	memset(strBuf,0,sizeof(char)*20);
	int nValue = 0;
	for(int i=0; i<nLen; i++)
	{
		if((strData[i] == '-') &&(nNum == 0))
		{
			try
			{
				nValue = atoi(strBuf);
				if (nValue<=2000 || nValue > 2100) 
					nValue = 2008;

				tempDate.tm_year = nValue-1900;
			}
			catch (...)
			{
				tempDate.tm_year = 2008-1900;
			}

			nNum++; nPos = 0;
			memset(strBuf,0,sizeof(char)*20);
			continue;	
		}
		else if((strData[i] == '-') && (nNum == 1))
		{
			try
			{
				nValue = atoi(strBuf);
				if (nValue< 1 || nValue>12) 
					nValue = 1;
				tempDate.tm_mon = nValue-1;
			}
			catch (...)
			{
				tempDate.tm_mon = 0;
			}
			nNum++; nPos = 0;
			memset(strBuf,0,sizeof(char)*20);
			continue;	
		}
		else
		{
			strBuf[nPos] = strData[i];
			nPos++;
		}
	}

	if(nPos > 0)
	{
		try
		{
			nValue = atoi(strBuf);
			if (nValue<1 || nValue>31) 
				nValue = 1;
			tempDate.tm_mday = nValue;
		}
		catch (...)
		{
			tempDate.tm_mday = 1;
		}
		return TRUE;
	}
	return FALSE;
}


// ���ܣ����ݷ��ش�����ʾ��Ӧ��Ϣ
// ������ rc          ��������ֵ
//        szComment   ���ú�������
// ���أ�
static void check_error (long rc, char *strComment)
{
#ifdef _DEBUG
	if(rc == SE_SUCCESS) return;
	char strInfo[SE_MAX_MESSAGE_LENGTH] = {0};
	sprintf(strInfo,"\n function:%s, error code:%d message:%s\n",strComment,rc,ReturnErrorCodeInfo(rc));
	FILE *pf = fopen("C:\\ErrorInfo.txt","a");
	if(NULL == pf) return;
	fwrite(strInfo,strlen(strInfo),1,pf);
	fclose(pf);
#endif

}



//==============================class CSdeConnection===================================//


// ���ܣ����캯��
// ��������
// ���أ���
CSdeConnection::CSdeConnection()
{
	m_pConnectObj = NULL;
	memset(m_strUserName,0,sizeof(char)*SE_MAX_OWNER_LEN);
	memset(m_strDBName,0,sizeof(char)*SE_MAX_DATABASE_LEN);

}


// ���ܣ���������
// ��������
// ���أ���
CSdeConnection::~CSdeConnection()
{
	Disconnect();
}


// ���ܣ���������(��ʵ��)
// �ú�����XML�ļ�����ʹ��
long CSdeConnection::Connect(const char* strConnect)
{
	return -1;
}


// ���ܣ���������
// ������strServer    ����������
//       strInstance  ����ʵ���������
//       strDatabase  ���ݿ�����
//       strUserName  �û�����
//       strPassword  �û�����
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο������е�DIST_ENUM_CONNECTRESULT
long CSdeConnection::Connect(const char* strServer,
							 const char* strInstance,
							 const char* strDatabase,
		                     const char* strUserName,
						     const char* strPassword)
{
	SE_ERROR sdeError;

	//���ǵ���C-API��ȡ������Զ�̲��ܶ�ȡ������ԭ�������ڴ˱����Ա�����
	strcpy(m_strDBName,strDatabase);   
	strcpy(m_strUserName,strUserName);

	long rc = -1;
	try{
		rc = SE_connection_create(strServer,strInstance,strDatabase,strUserName,strPassword, &sdeError, &m_pConnectObj);
		check_error(rc,"Connect->SE_connection_create");
	}
	catch (...) {
		check_error(-1,"ϵͳ�쳣���������ݿ�ʧ��!");
		return -1;
	}

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;	
}   


// ���ܣ��Ͽ�����
// ��������
// ���أ���
long CSdeConnection::Disconnect(void)
{
	if(NULL != m_pConnectObj)
	{
		SE_connection_free(m_pConnectObj);
		m_pConnectObj = NULL;
	}
	
	return 1;
}


// ���ܣ��������
// ������IntervalTime��ⳬʱ
// ���أ�TRUE ���ӳɹ���FALSE ����ʧ��
long CSdeConnection::CheckConnect(int IntervalTime)
{
	long rc = SE_connection_test_server(m_pConnectObj, IntervalTime);
	
	if(rc == SE_SUCCESS)
		return 1;
	else
	{
		check_error(rc,"CheckConnect->SE_connection_test_server");
	    return rc;
	}
}


// ���ܣ���ȡ����
// ������param        ��������
//       strParameter ��������
// ���أ���
void CSdeConnection::GetParameter(PARAMETERTYPE param, char* strParameter)
{
	long rc;
	switch(param) {
	case kServer: rc = SE_connection_get_server_name(m_pConnectObj,strParameter);
		check_error(rc,"GetParameter->SE_connection_get_server_name");
		break;
	case kInstance: rc = SE_connection_get_instance_name(m_pConnectObj,strParameter);
		check_error(rc,"GetParameter->SE_connection_get_instance_name");
		break;
	case kDbName:rc = SE_connection_get_database(m_pConnectObj,strParameter);
		check_error(rc,"GetParameter->SE_connection_get_database");
		break;
	case kUserName:rc = SE_connection_get_user_name(m_pConnectObj,strParameter);
		check_error(rc,"GetParameter->SE_connection_get_user_name");
		break;
	default: strParameter = NULL;
	}
}


// ���ܣ����ò���
// ������param        ��������
//       strParameter ��������
// ���أ���
void CSdeConnection::SetParameter(PARAMETERTYPE param, char* strParameter)
{
	//����
}


// ���ܣ���ȡ���ݿ����Ӷ���ָ��
// ��������
// ���أ����ض���ָ��
SE_CONNECTION CSdeConnection::GetConnectObj(void)
{
	return m_pConnectObj;
}

// ���ܣ���ȡSDE���ݿ����Ӷ��� SE_CONNECTION
// ��������
// ���أ����ݿ����Ӷ���
void* CSdeConnection::GetConnectObjPointer(void)
{
	return (void*) m_pConnectObj;
}

// ���ܣ�ȡ��ǰ���ӵ����ݿ������û���
// ������strDBName    ���ݿ�����
//       strUserName   �û�����
// ���أ���
void CSdeConnection::GetUserDBInfo(char* strDBName,char* strUserName)
{
	memset(strUserName,0,sizeof(char)*SE_MAX_OWNER_LEN);
	memset(strDBName,0,sizeof(char)*SE_MAX_DATABASE_LEN);
	strcpy(strDBName,m_strDBName);
	strcpy(strUserName,m_strUserName);
}


// ���ܣ��ͷŶ���
// ��������
// ���أ���
void CSdeConnection::Release(void)
{
	delete this;
}






//==============================class CArxSdeCommand===================================//

// ���ܣ����캯��
// ��������
// ���أ���
CSdeCommand::CSdeCommand(void)
{
	m_pConnectObj = NULL;
	m_pParams = NULL;
	m_nParamCount = 0;
	memset(m_strCurTableName,0,sizeof(char)*SE_MAX_TABLE_LEN );
	m_strAllLayerNameArray = NULL;
	m_nLayerNameCount = 0;
	memset(m_strUserName,0,sizeof(char)*SE_MAX_OWNER_LEN);
	memset(m_strDBName,0,sizeof(char)*SE_MAX_DATABASE_LEN);

	m_tempShape = NULL;
	m_pTempPtArray = NULL;
}


// ���ܣ���������
// ��������
// ���أ���
CSdeCommand::~CSdeCommand(void)
{
	if(NULL != m_pParams)
	{
		delete [] m_pParams;
		m_pParams = NULL;
		m_nParamCount = 0;
	}
	memset(m_nFldTypeArray,0,sizeof(int)*SE_MAX_COLUMNS);

	if(NULL != m_strAllLayerNameArray)
	{
		for(int i=0; i<m_nLayerNameCount; i++)
			free(m_strAllLayerNameArray[i]);
		free(m_strAllLayerNameArray);
		m_nLayerNameCount = 0;
	}

	if(m_tempShape != NULL)
	{
		SE_shape_free(m_tempShape);
		m_tempShape = NULL;
	}
	if( NULL != m_pTempPtArray)
	{
		delete [] m_pTempPtArray;
		m_pTempPtArray = NULL;
	}
}


// ���ܣ��������ݿ����Ӷ���
// ������pConnect  ���ݿ����Ӷ���
// ���أ���
// ��ע�������������Ӷ�������е�������Ӧ���³�ʼ��
void CSdeCommand::SetConnectObj(IDistConnection* pConnect)
{
	CSdeConnection* pObj = (CSdeConnection*) pConnect;
	m_pConnectObj = pObj->GetConnectObj();

	//��ȡ��ǰ��½�����ݿ������û������������ȡ�ֶα���ʹ��
	pObj->GetUserDBInfo(m_strDBName,m_strUserName);

	//ɾ������ֶ���Ϣ����
	if(NULL != m_pParams)
	{
		delete [] m_pParams;
		m_pParams = NULL;
		m_nParamCount = 0;
	}

	//��յ�ǰ�����ֶ���Ϣ����Ӧ�ı�����(Ϊ���Ч�ʣ�����������ݱ���ͬ�����������»�ȡ�ֶ���Ϣ)
	memset(m_strCurTableName,0,sizeof(char)*SE_MAX_TABLE_LEN );

	//ɾ����ǰ�û����ܷ��ʵ�ͼ��������
	if(NULL != m_strAllLayerNameArray)
	{
		for(int i=0; i<m_nLayerNameCount; i++)
			free(m_strAllLayerNameArray[i]);
		free(m_strAllLayerNameArray);
		m_nLayerNameCount = 0;
		m_strAllLayerNameArray = NULL;
	}	
}


//���ܣ������Զ���SQL��䣬��ȡ�ֶΣ���where������Ϣ
//������strSQL         ���������ַ���
//      strTableName   ����������
//      nTableNums     �����
//      strFieldNames  �ֶ�������
//      nFieldNums     �ֶ�����
//      strWhere       WHERE����
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
//��ע���ֽ��ַ�������ʽ F:*,...,*,T:*,...,*,W:*,##
long CSdeCommand::ReduceSQL(const char* strSQL,
		                    char*** strTableNames,int& nTableNum, 
				            char*** strFieldNames,int& nFieldNum,
							char** strWhere)
{
	if(NULL == strSQL) return -1;
	int i,j;
	int nStrLen = strlen(strSQL);
	char *strF = "F:",*strT = "T:",*strW = "W:",*strE = "##";
	char  strTemp[2] ={0,0},cValue = 0;
	char strPF[50][SE_MAX_COLUMN_LEN]={0},strPT[10][SE_MAX_TABLE_LEN]={0};
	char strpW[SE_MAX_MESSAGE_LENGTH*4] ={0};
	bool  bF = false,bT = false,bW = false;

	int nPos = 0;
	nFieldNum = 0; nTableNum = 0;
	
	for(i=0; i<nStrLen; i++)
	{
		memcpy(strTemp,strSQL+i,sizeof(char)*2);
		if(strTemp[0] == strF[0] && strTemp[1] == strF[1]){ //F:
			bF = true; bT = false; bW = false;
			i++; nPos = 0; continue;
		}
		else if(strTemp[0] == strT[0] && strTemp[1] == strT[1]){ //T:
			bF = false; bT = true; bW = false;
			i++; nPos = 0; continue;
		}
		else if(strTemp[0] == strW[0] && strTemp[1] == strW[1]){ //W:
			bF = false; bT = false; bW = true;
			i++; nPos = 0; continue;
		}
		else if(strTemp[0] == strE[0] && strTemp[1] == strE[1]) break; //##
		
		cValue = strSQL[i];
		if(bF == true){
			if(cValue ==','){
				nFieldNum++;  nPos = 0;
			}
			else{
				strPF[nFieldNum][nPos] = cValue;  nPos++;	
			}
		}
		else if(bT == true){
			if(cValue ==','){
				nTableNum++;  nPos = 0;
			}
			else{
				strPT[nTableNum][nPos] = cValue;  nPos++;
			}
		}
		else if(bW == true){
			if(cValue ==','){
				bW = false; continue;
			}
			else{
				strpW[nPos] = cValue;  nPos++;	
			}
		}
	}


	if( 0 == nFieldNum)
	{
		*strFieldNames = NULL;
	}
	else
	{
		
		char** str = (char **) malloc (nFieldNum * sizeof(char *));
		for(j=0; j<nFieldNum; j++)
		{
			str[j] = (char*) malloc(sizeof(char)*SE_MAX_COLUMN_LEN);
			memcpy(str[j],strPF[j],sizeof(char)*SE_MAX_COLUMN_LEN);
		}
		*strFieldNames = str;
	}

	if( 0 == nTableNum)
	{
		*strTableNames = NULL;
	}
	else
	{
		char** str = (char **) malloc (nTableNum * sizeof(char *));
		for(j=0; j<nTableNum; j++)
		{
			str[j] = (char*) malloc(sizeof(char)*SE_MAX_TABLE_LEN);
			memcpy(str[j],strPT[j],sizeof(char)*SE_MAX_TABLE_LEN);
		}
		*strTableNames = str;
	}
	
	if(strpW[0] == 0)
	{
		*strWhere = NULL;
	}
	else
	{
		*strWhere = new char[SE_MAX_MESSAGE_LENGTH*4];
		*strWhere[0] = 0;
//		memset(*strWhere,0,sizeof(char)*SE_MAX_MESSAGE_LENGTH);
		memcpy(*strWhere,strpW,sizeof(char)*SE_MAX_MESSAGE_LENGTH*4);
	}
	

	return 1;
}


long CSdeCommand::ReduceSQL(const char* strSQL,
							char*** strTableNames,int& nTableNum, 
							char*** strFieldNames,int& nFieldNum,
							char** strWhere, char** strOrderby)
{
	if(NULL == strSQL) return -1;
	int i,j;
	int nStrLen = strlen(strSQL);
	char *strF = "F:",*strT = "T:",*strW = "W:",*strO = "O:",*strE = "##";
	char  strTemp[2] ={0,0},cValue = 0;
	char strPF[100][SE_MAX_COLUMN_LEN]={0},strPT[10][SE_MAX_TABLE_LEN]={0};
	char strpW[SE_MAX_MESSAGE_LENGTH*4] ={0},strpO[SE_MAX_MESSAGE_LENGTH]={0};
	bool  bF = false,bT = false,bO = false, bW = false;

	int nPos = 0;
	nFieldNum = 0; nTableNum = 0;

	for(i=0; i<nStrLen; i++)
	{
		memcpy(strTemp,strSQL+i,sizeof(char)*2);
		if(strTemp[0] == strF[0] && strTemp[1] == strF[1]){ //F:
			bF = true; bT = false; bO = false; bW = false;
			i++; nPos = 0; continue;
		}
		else if(strTemp[0] == strT[0] && strTemp[1] == strT[1]){ //T:
			bF = false; bT = true; bO = false; bW = false;
			i++; nPos = 0; continue;
		}
		else if(strTemp[0] == strW[0] && strTemp[1] == strW[1]){ //W:
			bF = false; bT = false; bO = false; bW = true;
			i++; nPos = 0; continue;
		}
		else if(strTemp[0] == strO[0] && strTemp[1] == strO[1]){ //O:
			bF = false; bT = false; bO = true; bW = false;
			i++; nPos = 0; continue;
		}
		else if(strTemp[0] == strE[0] && strTemp[1] == strE[1]) break; //##

		cValue = strSQL[i];
		if(bF == true){
			if(cValue ==','){
				nFieldNum++;  nPos = 0;
			}
			else{
				strPF[nFieldNum][nPos] = cValue;  nPos++;	
			}
		}
		else if(bT == true){
			if(cValue ==','){
				nTableNum++;  nPos = 0;
			}
			else{
				strPT[nTableNum][nPos] = cValue;  nPos++;
			}
		}
		else if(bO == true){
			if(cValue ==','){
				bO = false; continue;
			}
			else{
				strpO[nPos] = cValue;  nPos++;	
			}
		}
		else if(bW == true){
			if(cValue ==','){
				bW = false; continue;
			}
			else{
				strpW[nPos] = cValue;  nPos++;	
			}
		}
	}


	if( 0 == nFieldNum)
	{
		*strFieldNames = NULL;
	}
	else
	{

		char** str = (char **) malloc (nFieldNum * sizeof(char *));
		for(j=0; j<nFieldNum; j++)
		{
			str[j] = (char*) malloc(sizeof(char)*SE_MAX_COLUMN_LEN);
			memcpy(str[j],strPF[j],sizeof(char)*SE_MAX_COLUMN_LEN);
		}
		*strFieldNames = str;
	}

	if( 0 == nTableNum)
	{
		*strTableNames = NULL;
	}
	else
	{
		char** str = (char **) malloc (nTableNum * sizeof(char *));
		for(j=0; j<nTableNum; j++)
		{
			str[j] = (char*) malloc(sizeof(char)*SE_MAX_TABLE_LEN);
			memcpy(str[j],strPT[j],sizeof(char)*SE_MAX_TABLE_LEN);
		}
		*strTableNames = str;
	}

	if(strpO[0] == 0)
	{
		*strOrderby = NULL;
	}
	else
	{
		*strOrderby = new char[SE_MAX_MESSAGE_LENGTH];
		*strOrderby[0] = 0;
//		memset(*strOrderby,0,sizeof(char)*SE_MAX_MESSAGE_LENGTH);
		memcpy(*strOrderby,strpO,sizeof(char)*SE_MAX_MESSAGE_LENGTH);
	}

	if(strpW[0] == 0)
	{
		*strWhere = NULL;
	}
	else
	{
		*strWhere = new char[SE_MAX_MESSAGE_LENGTH];
		*strWhere[0] = 0;
//		memset(*strWhere,0,sizeof(char)*SE_MAX_MESSAGE_LENGTH);
		memcpy(*strWhere,strpW,sizeof(char)*SE_MAX_MESSAGE_LENGTH);
	}


	return 1;
}

//���ܣ����ݲ�ѯ�����������RowId�ֶβ�ѯ���д��RowId����
//������strFileName    ��ʱ�ļ���
//      strTableName   ������
//      strFldName     RowID�ֶ�����
//      strWhere       Where����
//      pSpatialFilter �ռ��������
//      pData          RowId����
//���أ���
void CSdeCommand::WriteRowIdToArray(const char* strTableName, const char* strFldName,
									const char* strWhere,
								    void* pSpatialFilter,CDistIntArray* pData)
{
	if(NULL == pData) return;

	// 1.����SQL��ѯ���
	SE_SQL_CONSTRUCT   *sqlc = NULL;    //SQL���ṹָ��
	int nFldCount =1;
	char **strFldNames = (char**) malloc (nFldCount * sizeof(char *));
	strFldNames[0] = (char*) malloc(SE_MAX_COLUMN_LEN*sizeof(char));
	strcpy(strFldNames[0],strFldName);
	
	int nTableCount = 1;
	long rc = SE_sql_construct_alloc (nTableCount, &sqlc);  // ����SQL���ռ� 
	sqlc->num_tables = nTableCount;
	strcpy (sqlc->tables[0], strTableName);
	sqlc->where = (char*) malloc(sizeof(char)*SE_MAX_MESSAGE_LENGTH*4);    
	sqlc->where[0] = 0;
//	memset(sqlc->where,0,sizeof(char)*SE_MAX_MESSAGE_LENGTH*4);
	if(NULL != strWhere) 
		strcpy (sqlc->where, strWhere);               // ���Where����


	// 3.�����ѯ���͹�����
	SE_STREAM  pStream = NULL;  //SDE������ָ�붨��
	rc = SE_stream_create(m_pConnectObj,&pStream);
	rc = SE_stream_query (pStream,nFldCount,(const char**)strFldNames, sqlc);
	check_error(rc, "WriteRowIdToArray->SE_stream_query");

	if(NULL != pSpatialFilter)// �趨�ռ��ѯ����(������)
	{
		rc = SE_stream_set_spatial_constraints(pStream, SE_SPATIAL_FIRST, FALSE, 1, (SE_FILTER*)pSpatialFilter);
		check_error(rc, "WriteRowIdToArray->SE_stream_set_spatial_constraints");
	}

	// 4.ִ�в�ѯ
	rc = SE_stream_execute (pStream);
	check_error(rc, "WriteRowIdToArray->SE_stream_execute");

	free (sqlc->where);
	SE_sql_construct_free (sqlc);
	free(strFldNames[0]);
	free(strFldNames);

	long nValue = 0;
	while(rc == SE_SUCCESS)
	{
		rc = SE_stream_fetch(pStream);
		if(rc == SE_SUCCESS)
		{
			rc = SE_stream_get_integer(pStream,1,&nValue);
			pData->Add(nValue);			
		}	
	}
	SE_stream_free(pStream);
}


// ���ܣ������ݿ��в�ѯ��¼����
// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
//       IRcdSet        ���ݲ�ѯ�������صļ�¼��
//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::SelectData(const char* strSQL,IDistRecordSet** IRcdSet,void* pSpatialFilter)
{
	int i;
	long rc =-1;
	char **strTableNames = NULL;         //��������
	int nTableNum = 0;                   //������
	char **strFieldNames = NULL;         //�ֶ�������
	int nFieldNum = 0;                   //�ֶ�����
	char *strWhere = NULL;               //where����
	char *strOrderby = NULL;

	// 1.�����Զ���SQL���(��ʽ F:*,...,*,T:*,...,*,W:*,##)���ֽ�ɣ������飬�ֶ����飬WHERE��������Ϣ
//	ReduceSQL(strSQL,&strTableNames,nTableNum,&strFieldNames,nFieldNum,&strWhere);
	ReduceSQL(strSQL,&strTableNames,nTableNum,&strFieldNames,nFieldNum,&strWhere,&strOrderby);
	if(0 == nTableNum || 0 == nFieldNum)  return -1;

	IDistParameter* pParam=NULL;
	int nParamNum = 0;
	if((rc=GetTableInfo(&pParam,&nParamNum,strTableNames[0],NULL))!=1)  return rc;

	BOOL bFoundId = FALSE;
	char strRowIdName[SE_MAX_COLUMN_LEN] ={0};  //coloum max 32 ,table max 160
	strRowIdName[0] = 0;
//	memset(strRowIdName,0,sizeof(char)*SE_MAX_COLUMN_LEN);
	for(i=0; i<nParamNum; i++) //���ֶ��в���RowId�ֶ�
	{ 
		if(pParam[i].fld_nRowIdType == SE_REGISTRATION_ROW_ID_COLUMN_TYPE_SDE)
		{
			if(pParam[i].fld_nType == kInt16 || pParam[i].fld_nType == kInt32)
			{
				bFoundId = TRUE;
				strcpy(strRowIdName,pParam[i].fld_strName);  break;
			}	
		}
	}

	CDistIntArray *pData = new CDistIntArray;
	if(NULL == pData)
	{
		check_error(-1,  "SelectData->alloc memory error!");
		for(i=0; i<nTableNum; i++)
			free(strTableNames[i]);
		free(strTableNames);
		for(i=0; i<nFieldNum; i++)
			free(strFieldNames[i]);
		free(strFieldNames);

		free(strOrderby);
		free(strWhere);
		return -1;
	}
	if(bFoundId == TRUE)
		WriteRowIdToArray(strTableNames[0],strRowIdName,strWhere,pSpatialFilter,pData);
	else
	{
		delete[] pData; pData = NULL;
	}

	// 2.����SQL��ѯ���
    SE_STREAM  pStream = NULL;  //SDE������ָ�붨��
	rc = SE_stream_create (m_pConnectObj, &pStream);
	check_error(rc,  "SelectData->SE_stream_create");

    SE_QUERYINFO queryinfo;
	rc = SE_queryinfo_create (&queryinfo);   //���ٲ�ѯ�ṹ�ռ�
	check_error (rc, "SelectData->SE_stream_create");

	rc = SE_queryinfo_set_tables(queryinfo,nTableNum,(const char **)strTableNames,NULL); 
	check_error (rc,  "SelectData->SE_queryinfo_set_tables");  //���ñ���

	rc = SE_queryinfo_set_columns (queryinfo,nFieldNum,(const char **)strFieldNames);
	check_error (rc, "SelectData->SE_queryinfo_set_columns");  //�����ֶ���

	rc = SE_queryinfo_set_where_clause(queryinfo,strWhere);  //����Where����
	check_error (rc, "SelectData->SE_queryinfo_set_where_clause"); 

	rc = SE_queryinfo_set_by_clause(queryinfo,strOrderby);  //����Order by����
	check_error (rc, "SelectData->SE_queryinfo_set_by_clause"); 

	rc = SE_stream_query_with_info(pStream, queryinfo);    //��ѯ����������������
	check_error (rc, "SelectData->SE_stream_query_with_info");


	// 3.�����ѯ���͹�����
	if(NULL != pSpatialFilter)// �趨�ռ��ѯ����(������)
	{
		rc = SE_stream_set_spatial_constraints(pStream, SE_SPATIAL_FIRST, FALSE, 1, (SE_FILTER*)pSpatialFilter);
		check_error(rc, "SelectData->SE_stream_set_spatial_constraints");
	}

	// 4.ִ�в�ѯ
	rc = SE_stream_execute (pStream);
	check_error(rc, "SelectData->SE_stream_execute");

	// 5.������¼�����󣬽���ѯ�������
	CSdeRecordSet* pRcdSet = new  CSdeRecordSet; 
	pRcdSet->SetInfo(&pStream,&queryinfo,(SE_FILTER *)pSpatialFilter,pData);
	*IRcdSet = (IDistRecordSet*)pRcdSet;


	// 6.�ͷ���ʱ�ռ�
	SE_queryinfo_free (queryinfo);

	for(i=0; i<nTableNum; i++)
		free(strTableNames[i]);
	free(strTableNames);
	for(i=0; i<nFieldNum; i++)
		free(strFieldNames[i]);
	free(strFieldNames);

//	free(strOrderby);
//	free(strWhere);
	delete[] strOrderby;
	delete[] strWhere;

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;
}



// ���ܣ������ݿ�ָ��������Ӽ�¼
// ������strTableName   ����������
//       IRcdSet        ��Ҫ��ӵ����еļ�¼
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::InsertData(int* pRowId,const char* strSQL,...)
{

	long rc;
	int i,j;
	char **strTableNames = NULL;         //��������
	int nTableNum = 0;                 //������
	char **strFieldNames = NULL;        //�ֶ�������
	int nFieldNum = 0;                 //�ֶ�����
	char *strWhere = NULL;              //where����

	//1.�ֽ�SQL�ַ�����Ϣ(���������飬���������ֶ��������飬�ֶ�������WHERE����)
	ReduceSQL(strSQL,&strTableNames,nTableNum,&strFieldNames,nFieldNum,&strWhere);
	if(0 == nTableNum || 0 == nFieldNum)
	{
		check_error(-1,"InsertData-> SQL Select condition Error!");
		return -1;
	}
	
	//2. �����ֶ�������Ϣ����ȡ��Ӧ������
	int  nTotalParam = 0;
	IDistParameter* pParam = NULL;
	if((rc=GetTableInfo(&pParam,&nTotalParam,strTableNames[0],NULL))!=1)  return rc;

	memset(m_nFldTypeArray,0,sizeof(int)*nFieldNum);
	
	for(i=0; i<nFieldNum; i++)
	{
		for(j=0; j<nTotalParam; j++)
		{
			if(0 == CompareString(pParam[j].fld_strName,strFieldNames[i]))
			{
				m_nFldTypeArray[i] = pParam[j].fld_nType;
				break;
			}
		}
	}


	//3.������ӿռ�
	SE_STREAM pStream = NULL;
	SE_stream_create(m_pConnectObj,&pStream);
	rc = SE_stream_insert_table (pStream,(const char*)strTableNames[0],(SHORT)nFieldNum,(const char **)strFieldNames);

	//4. �����ֶ���Ϣ���ӿ��������ṹ��ȡ����ֵ
	short int sValue=0; int nValue=0; float fValue=0; double dValue=0; char* strValue=NULL;

	SE_BLOB_INFO  *pBlob = NULL; SE_SHAPE pShape = NULL; tm tempDate; char strDateBuf[50]={0};
	SE_WCHAR strValueW[SE_MAX_MESSAGE_LENGTH];
    va_list lpParam;
	va_start (lpParam, strSQL);
	for(j=0; j<nFieldNum; j++)
	{
		switch(m_nFldTypeArray[j]) 
		{
		case kInt16:
			sValue = va_arg(lpParam, short);
			rc = SE_stream_set_smallint(pStream, j+1,(const SHORT*)&sValue);
			check_error(rc, "InsertData->SE_stream_set_smallint");
			break;
		case kInt32:
			nValue = va_arg(lpParam, long);
			rc = SE_stream_set_integer(pStream, j+1,(const LONG*)&nValue);
			check_error(rc, "InsertData->SE_stream_set_integer");
			break;
		case kFloat32:
			fValue = va_arg(lpParam, float);
			rc = SE_stream_set_float(pStream, j+1,(const FLOAT*)&fValue);
			check_error(rc, "InsertData->SE_stream_set_float");
			break;
		case kFloat64:
			dValue = va_arg(lpParam, double);
			rc = SE_stream_set_double(pStream, j+1,(const double*)&dValue);
			check_error(rc, "InsertData->SE_stream_set_double");
			break;
		case kString:
			strValue = va_arg(lpParam, char*);
			rc = SE_stream_set_string(pStream, j+1,(const char*)strValue);
			check_error(rc, "InsertData->SE_stream_set_string");
			break;
		case kNString:
			setlocale(LC_ALL, "chs");
			strValue = va_arg(lpParam, char*);
			strValueW[0] = 0;
//			memset(strValueW,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);
			mbstowcs(strValueW,strValue,SE_MAX_MESSAGE_LENGTH);
			rc = SE_stream_set_nstring(pStream,j+1,(const SE_WCHAR*)strValueW);
			setlocale(LC_ALL, "C");
			check_error(rc, "InsertData->SE_stream_set_nstring");
			break;
		case kBLOB:
		case kCLOB:
		case kNCLOB:
/*			pBlob = va_arg(lpParam, SE_BLOB_INFO*);
			rc = SE_stream_set_blob(pStream, j+1,(const SE_BLOB_INFO*)pBlob);
			check_error(rc, "InsertData->SE_stream_set_blob");
*/			break;
		case kDate:	        
			strDateBuf[0] = 0;
//			memset(strDateBuf,0,sizeof(char)*50);
			strcpy(strDateBuf,va_arg(lpParam, char*)); //���ڸ�ʽ��****-**-**
			memset(&tempDate,0,sizeof(tm));
			StringToDate(tempDate,strDateBuf);	
			rc = SE_stream_set_date(pStream, j+1,&tempDate);
			check_error(rc, "InsertData->SE_stream_set_date");
			break;
		case kShape:
			pShape = va_arg(lpParam, SE_SHAPE);
			rc = SE_stream_set_shape(pStream, j+1,(const SE_SHAPE)pShape);
			check_error(rc, "InsertData->SE_stream_set_shape");
			break;
		default:
			break;
		}
	}
    va_end ( lpParam );

	
	rc = SE_stream_execute (pStream);
	check_error(rc, "InsertData->SE_stream_execute");
	
	SE_stream_last_inserted_row_id(pStream,(long*)pRowId);

	free(strWhere);

	for(int k=0; k<nTableNum;k++)
		free(strTableNames[k]);
	free(strTableNames);

	for(k=0;k<nFieldNum;k++)
		free(strFieldNames[k]);
	free(strFieldNames);

	SE_stream_free(pStream);

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;
}

// ���ܣ������ݿ�ָ��������Ӽ�¼
// ������strTableName   ����������
//       pRowId         ������Ӽ�¼��IDֵ
//       strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
//       strValueArray  ֵ����
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::InsertDataAsString(int* pRowId,const char* strSQL,char** strValueArray)
{
	long rc;
	int i,j;
	char **strTableNames = NULL;         //��������
	int nTableNum = 0;                 //������
	char **strFieldNames = NULL;        //�ֶ�������
	int nFieldNum = 0;                 //�ֶ�����
	char *strWhere = NULL;              //where����

	//1.�ֽ�SQL�ַ�����Ϣ(���������飬���������ֶ��������飬�ֶ�������WHERE����)
	ReduceSQL(strSQL,&strTableNames,nTableNum,&strFieldNames,nFieldNum,&strWhere);
	if(0 == nTableNum || 0 == nFieldNum)
	{
		check_error(-1,"InsertDataAsString-> SQL Select condition Error!");
		return -1;
	}
	
	//2. �����ֶ�������Ϣ����ȡ��Ӧ������
	int  nTotalParam = 0;
	IDistParameter* pParam = NULL;
	if((rc=GetTableInfo(&pParam,&nTotalParam,strTableNames[0],NULL))!=1)  return rc;

	memset(m_nFldTypeArray,0,sizeof(int)*nFieldNum);
	
	for(i=0; i<nFieldNum; i++)
	{
		for(j=0; j<nTotalParam; j++)
		{
			if(0 == CompareString(pParam[j].fld_strName,strFieldNames[i]))
			{
				m_nFldTypeArray[i] = pParam[j].fld_nType;
				break;
			}
		}
	}


	//3.������ӿռ�
	SE_STREAM pStream = NULL;
	SE_stream_create(m_pConnectObj,&pStream);
	rc = SE_stream_insert_table (pStream,(const char*)strTableNames[0],(SHORT)nFieldNum,(const char **)strFieldNames);

	//4. �����ֶ���Ϣ���ӿ��������ṹ��ȡ����ֵ
	short int sValue=0; int nValue=0; float fValue=0; double dValue=0; char* strValue=NULL;
	SE_BLOB_INFO  *pBlob = NULL; tm tempDate; SE_SHAPE pShape = NULL;
	SE_WCHAR strValueW[SE_MAX_MESSAGE_LENGTH];

    char cValue[SE_MAX_CONFIG_STR_LEN];
	for(j=0; j<nFieldNum; j++)
	{
		cValue[0] = 0;
//		memset(cValue,0,sizeof(char)*SE_MAX_CONFIG_STR_LEN);
		strcpy(cValue,strValueArray[j]);

		switch(m_nFldTypeArray[j]) 
		{
		case kInt16:
			sValue = 0;
			if(cValue[0] != 0)
				sValue = atoi(cValue);
			rc = SE_stream_set_smallint(pStream, j+1,(const SHORT*)&sValue);
			check_error( rc, "InsertDataAsString->SE_stream_set_smallint");
			break;
		case kInt32:
			nValue = 0;
			if(cValue[0] != 0)
				nValue = atoi(cValue);
			rc = SE_stream_set_integer(pStream, j+1,(const LONG*)&nValue);
			check_error(rc, "InsertDataAsString->SE_stream_set_integer");
			break;
		case kFloat32:
			fValue = 0.0;
			if(cValue[0] != 0)
				fValue = (float)atof(cValue);
			rc = SE_stream_set_float(pStream, j+1,(const FLOAT*)&fValue);
			check_error(rc, "InsertDataAsString->SE_stream_set_float");
			break;
		case kFloat64:
			dValue = 0.0;
			if(cValue[0] != 0)
				dValue = atof(cValue);
			rc = SE_stream_set_double(pStream, j+1,(const double*)&dValue);
			check_error( rc, "InsertDataAsString->SE_stream_set_double");
			break;
		case kString:
			rc = SE_stream_set_string(pStream, j+1,(const char*)cValue);
			check_error(rc, "InsertDataAsString->SE_stream_set_string");
			break;
		case kNString:
			setlocale(LC_ALL, "chs");
			strValueW[0] = 0;
//			memset(strValueW,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);
			mbstowcs(strValueW,cValue,SE_MAX_MESSAGE_LENGTH); //����> ��
			rc = SE_stream_set_nstring(pStream, j+1,(const SE_WCHAR*)strValueW);
			setlocale(LC_ALL, "C");
			check_error(rc, "InsertDataAsString->SE_stream_set_string");
			break;
		case kBLOB:
		case kCLOB:
		case kNCLOB:
/*			pBlob = NULL;
			if(cValue[0] != 0)
			{
			nValue = atoi(cValue);
			pBlob = (SE_BLOB_INFO*) nValue;
			}
			rc = SE_stream_set_blob(pStream, j+1,(const SE_BLOB_INFO*)pBlob);
			check_error(rc, "InsertDataAsString->SE_stream_set_blob");
*/			break;
		case kDate:
			memset(&tempDate,0,sizeof(tm));
			if(!StringToDate(tempDate,cValue))
			{
				rc = SE_stream_set_date(pStream, j+1,NULL);
			}
			else
				rc = SE_stream_set_date(pStream, j+1,&tempDate);

			check_error(rc, "InsertDataAsString->SE_stream_set_date");
			break;
		case kShape:
			pShape = NULL;
			if(cValue[0] != 0)
			{
				nValue = atoi(cValue);
//				pShape = (SE_SHAPE*) nValue;
				pShape = (SE_SHAPE) nValue;
			}
			rc = SE_stream_set_shape(pStream, j+1,(const SE_SHAPE)pShape);
			check_error(rc, "InsertDataAsString->SE_stream_set_shape");
			break;
		default:
			break;
		}
	}

	
	rc = SE_stream_execute (pStream);
	check_error(rc, "InsertDataAsString->SE_stream_execute");
	
	SE_stream_last_inserted_row_id(pStream,(long*)pRowId);

	free(strWhere);

	for(int k=0; k<nTableNum;k++)
		free(strTableNames[k]);
	free(strTableNames);

	for(k=0;k<nFieldNum;k++)
		free(strFieldNames[k]);
	free(strFieldNames);

	SE_stream_free(pStream);

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;
}



// ���ܣ��޸����ݿ����ָ����¼
// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
//       IRcdSet        ��¼�޸ĵ�����
//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::UpdateData(const char* strSQL,void* pSpatialFilter,...)
{
	long rc;
	int i,j,k;
	char **strTableNames = NULL;         //��������
	int nTableNum = 0;                   //������
	char **strFieldNames = NULL;         //�ֶ�������
	int nFieldNum = 0;                   //�ֶ�����
	char *strWhere = NULL;               //where����

	
	//1.�ֽ�SQL�ַ�����Ϣ(���������飬���������ֶ��������飬�ֶ�������WHERE����)
	ReduceSQL(strSQL,&strTableNames,nTableNum,&strFieldNames,nFieldNum,&strWhere);
	if(0 == nTableNum || 0 == nFieldNum)
	{
		check_error(-1,"UpdateData-> SQL Select condition Error!");
		return -1;
	}
	
	//2. �����ֶ�������Ϣ����ȡ��Ӧ������
	int  nTotalParam = 0;
	IDistParameter* pParam = NULL;
	if((rc=GetTableInfo(&pParam,&nTotalParam,strTableNames[0],NULL))!=1)  return rc;

	memset(m_nFldTypeArray,0,sizeof(int)*nFieldNum);
	for(i=0; i<nFieldNum; i++)
	{
		for(j=0; j<nTotalParam; j++)
		{
			if(0 == CompareString(pParam[j].fld_strName,strFieldNames[i]))
			{
				m_nFldTypeArray[i] = pParam[j].fld_nType;
				break;
			}
		}
	}

	//3.������ӿռ�
	SE_STREAM pStream = NULL;
	rc = SE_connection_test_server(m_pConnectObj, 100);
	check_error(rc, "UpdateData->SE_connection_test_server");
	rc = SE_stream_create(m_pConnectObj,&pStream);
	check_error(rc, "UpdateData->SE_stream_create");

	rc = SE_stream_update_table (pStream,(const char*)strTableNames[0],(SHORT)nFieldNum,
                                         (const char **)strFieldNames,strWhere);
	check_error(rc, "UpdateData->SE_stream_update_table");

	if(NULL != pSpatialFilter)// �趨�ռ��ѯ����(������)
	{
		rc = SE_stream_set_spatial_constraints(pStream, SE_SPATIAL_FIRST, FALSE, 1, (SE_FILTER*)pSpatialFilter);
		check_error(rc, "UpdateData->SE_stream_set_spatial_constraints");
	}

	//4. �����ֶ���Ϣ���ӿ��������ṹ��ȡ����ֵ
	short int sValue=0; int nValue=0; float fValue=0; double dValue=0; char* strValue=NULL;
	SE_BLOB_INFO  *pBlob = NULL; tm tempDate; SE_SHAPE pShape = NULL; char strDateBuf[50]={0};
	SE_WCHAR strValueW[SE_MAX_MESSAGE_LENGTH] = {0};

    va_list lpParam;
	va_start (lpParam, pSpatialFilter);
	for(j=0; j<nFieldNum; j++)
	{
		switch(m_nFldTypeArray[j]) 
		{
		case kInt16:
			sValue = va_arg(lpParam, short);
			rc = SE_stream_set_smallint(pStream, j+1,(const SHORT*)&sValue);
			check_error(rc, "UpdateData->SE_stream_set_smallint");
			break;
		case kInt32:
			nValue = va_arg(lpParam, long);
			rc = SE_stream_set_integer(pStream, j+1,(const LONG*)&nValue);
			check_error(rc, "UpdateData->SE_stream_set_integer");
			break;
		case kFloat32:
			fValue = va_arg(lpParam, float);
			rc = SE_stream_set_float(pStream, j+1,(const FLOAT*)&fValue);
			check_error(rc, "UpdateData->SE_stream_set_float");
			break;
		case kFloat64:
			dValue = va_arg(lpParam, double);
			rc = SE_stream_set_double(pStream, j+1,(const double*)&dValue);
			check_error(rc, "UpdateData->SE_stream_set_double");
			break;
		case kString:
			strValue = va_arg(lpParam, char*);
			rc = SE_stream_set_string(pStream, j+1,(const char*)strValue);
			check_error(rc, "UpdateData->SE_stream_set_string");
			break;
		case kNString:
			setlocale(LC_ALL, "chs");
			strValue = va_arg(lpParam, char*);
			strValueW[0] = 0;
//			memset(strValueW,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);
			mbstowcs(strValueW,strValue,SE_MAX_MESSAGE_LENGTH); //�� -> ��
			rc = SE_stream_set_nstring(pStream, j+1,(const SE_WCHAR*)strValueW);
			setlocale(LC_ALL, "C");
			check_error(rc, "UpdateData->SE_stream_set_nstring");
			break;
		case kBLOB:
		case kCLOB:
		case kNCLOB:
/*			pBlob = va_arg(lpParam, SE_BLOB_INFO*);
			rc = SE_stream_set_blob(pStream, j+1,(const SE_BLOB_INFO*)pBlob);
			check_error(rc, "UpdateData->SE_stream_set_blob");
*/			break;
		case kDate:
			memset(strDateBuf,0,sizeof(char)*50);
			strcpy(strDateBuf,va_arg(lpParam, char*));
			memset(&tempDate,0,sizeof(tm));
			StringToDate(tempDate,strDateBuf);
			rc = SE_stream_set_date(pStream, j+1,&tempDate);
			check_error(rc, "UpdateData->SE_stream_set_date");
			break;
		case kShape:
			pShape = va_arg(lpParam, SE_SHAPE);
			rc = SE_stream_set_shape(pStream, j+1,(const SE_SHAPE)pShape);
			check_error(rc, "UpdateData->SE_stream_set_shape");
			break;
		default:
			break;
		}
	}
    va_end ( lpParam );

	
	rc = SE_stream_execute (pStream);
	check_error(rc, "UpdateData->SE_stream_execute");
	
	free(strWhere);

	for(k=0; k<nTableNum;k++)
		free(strTableNames[k]);
	free(strTableNames);

	for(k=0;k<nFieldNum;k++)
		free(strFieldNames[k]);
	free(strFieldNames);

	SE_stream_free(pStream);

	if(rc == SE_SUCCESS)
		return 1;
	return rc;
}


// ���ܣ��޸����ݿ����ָ����¼
// ������strSQL         ����SQL��䣬���������ֶΣ���WHERE������Ϣ(��ʽ F:x,x,x,T:x,x,x,W:x#)
//       pSpatialFilter �ռ���������(��Ҫ���SDEʵ��ռ��ѯ)
//       ...            �����ֶ�ֵ�б�(����Ϊ�ַ���)
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::UpdateDataAsString(const char *strSQL,char** strValueArray)
{
	long rc;
	int i,j;
	char **strTableNames = NULL;         //��������
	int nTableNum = 0;                 //������
	char **strFieldNames = NULL;        //�ֶ�������
	int nFieldNum = 0;                 //�ֶ�����
	char *strWhere = NULL;              //where����

	//1.�ֽ�SQL�ַ�����Ϣ(���������飬���������ֶ��������飬�ֶ�������WHERE����)
	ReduceSQL(strSQL,&strTableNames,nTableNum,&strFieldNames,nFieldNum,&strWhere);
	if(0 == nTableNum || 0 == nFieldNum)
	{
		check_error(-1,"UpdateDataAsString-> SQL Select condition Error!");
		OutputDebugString("Error::CSdeCommand::UpdateDataAsString :: ReduceSQL");
		return -1;
	}
	
	//2. �����ֶ�������Ϣ����ȡ��Ӧ������
	int  nTotalParam = 0;
	IDistParameter* pParam = NULL;
	if((rc=GetTableInfo(&pParam,&nTotalParam,strTableNames[0],NULL))!=1)  return rc;

	memset(m_nFldTypeArray,0,sizeof(int)*nFieldNum);
	
	for(i=0; i<nFieldNum; i++)
	{
		for(j=0; j<nTotalParam; j++)
		{
			if(0 == CompareString(pParam[j].fld_strName,strFieldNames[i]))
			{
				m_nFldTypeArray[i] = pParam[j].fld_nType;
				break;
			}
		}
	}

	//3.������ӿռ�
	SE_STREAM pStream = NULL;
	rc = SE_stream_create(m_pConnectObj,&pStream);
	check_error( rc, "UpdateDataAsString->SE_stream_create");

	rc = SE_stream_update_table (pStream,(const char*)strTableNames[0],(SHORT)nFieldNum,
                                (const char **)strFieldNames,strWhere);
	check_error( rc, "UpdateDataAsString->SE_stream_update_table");

	//4. �����ֶ���Ϣ���ӿ��������ṹ��ȡ����ֵ
	short int sValue=0; int nValue=0; float fValue=0; double dValue=0; char* strValue=NULL;
	SE_BLOB_INFO  *pBlob = NULL; tm tempDate; SE_SHAPE pShape = NULL; DIST_DATETIME_X* pDate = NULL;
	SE_WCHAR strValueW[SE_MAX_MESSAGE_LENGTH]={0}; 
//	memset(strValueW,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);
	try
	{
		char cValue[SE_MAX_CONFIG_STR_LEN];
		for(j=0; j<nFieldNum; j++)
		{
			cValue[0] = 0;
//			memset(cValue,0,sizeof(char)*SE_MAX_CONFIG_STR_LEN);
			strcpy(cValue,strValueArray[j]);

			switch(m_nFldTypeArray[j]) 
			{
			case kInt16:
				sValue = 0;
				if(cValue[0] != 0)
					sValue = atoi(cValue);
				rc = SE_stream_set_smallint(pStream, j+1,(const SHORT*)&sValue);
				check_error( rc, "UpdateDataAsString->SE_stream_set_smallint");
				break;
			case kInt32:
				nValue = 0;
				if(cValue[0] != 0)
					nValue = atoi(cValue);
				rc = SE_stream_set_integer(pStream, j+1,(const LONG*)&nValue);
				check_error(rc, "UpdateDataAsString->SE_stream_set_integer");
				break;
			case kFloat32:
				fValue = 0.0;
				if(cValue[0] != 0)
					fValue = (float)atof(cValue);
				rc = SE_stream_set_float(pStream, j+1,(const FLOAT*)&fValue);
				check_error(rc, "UpdateDataAsString->SE_stream_set_float");
				break;
			case kFloat64:
				dValue = 0.0;
				if(cValue[0] != 0)
					dValue = atof(cValue);
				rc = SE_stream_set_double(pStream, j+1,(const double*)&dValue);
				check_error( rc, "UpdateDataAsString->SE_stream_set_double");
				break;
			case kString:
				rc = SE_stream_set_string(pStream, j+1,(const char*)cValue);
				check_error(rc, "UpdateDataAsString->SE_stream_set_string");
				break;
			case kNString:
				setlocale(LC_ALL, "chs");
				strValueW[0] = 0;
//				memset(strValueW,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);
				mbstowcs(strValueW,cValue,SE_MAX_MESSAGE_LENGTH); //����> ��
				rc = SE_stream_set_nstring(pStream, j+1,(const SE_WCHAR*)strValueW);
				setlocale(LC_ALL, "C");
				check_error(rc, "UpdateDataAsString->SE_stream_set_string");
				break;
			case kBLOB:
			case kCLOB:
			case kNCLOB:
/*				pBlob = NULL;
				if(cValue[0] != 0)
				{
				nValue = atoi(cValue);
				pBlob = (SE_BLOB_INFO*) nValue;
				}
				rc = SE_stream_set_blob(pStream, j+1,(const SE_BLOB_INFO*)pBlob);
				check_error(rc, "UpdateDataAsString->SE_stream_set_blob");
*/				break;
			case kDate:
				memset(&tempDate,0,sizeof(tm));
				if(!StringToDate(tempDate,cValue))
				{
					rc = SE_stream_set_date(pStream, j+1,NULL);
				}
				else
					rc = SE_stream_set_date(pStream, j+1,&tempDate);
				check_error(rc, "UpdateDataAsString->SE_stream_set_date");
				break;
			case kShape:
				pShape = NULL;
				if(cValue[0] != 0)
				{
					nValue = atoi(cValue);
					pShape = (SE_SHAPE) nValue;
				}
				rc = SE_stream_set_shape(pStream, j+1,(const SE_SHAPE)pShape);
				check_error(rc, "UpdateDataAsString->SE_stream_set_shape");
				break;
			default:
				break;
			}
		}
	}
	catch (...)
	{
		free(strWhere);
		for(int k=0; k<nTableNum;k++)
			free(strTableNames[k]);
		free(strTableNames);
		for(k=0;k<nFieldNum;k++)
			free(strFieldNames[k]);
		free(strFieldNames);
		SE_stream_free(pStream);

		return -1;
	}
	
	rc = SE_stream_execute (pStream);
	check_error(rc, "UpdateDataAsString->SE_stream_execute");

	free(strWhere);

	for(int k=0; k<nTableNum;k++)
		free(strTableNames[k]);
	free(strTableNames);

	for(k=0;k<nFieldNum;k++)
		free(strFieldNames[k]);
	free(strFieldNames);

	SE_stream_free(pStream);


	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;


}


// ���ܣ�ɾ�����ݿ���еļ�¼
// ������strSQL         ����SQL��䣬������:������WHERE������Ϣ(��ʽ T:x,x,x,W:x#)
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::DeleteData(const char* strSQL)
{
	long rc;
	char** strTableNames = NULL;
	int nTableNum = 0;
	char** strFieldNames = NULL;
	int nFieldNum = 0;
	char* strWhere = NULL;

	//1.�ֽ�SQL�ַ�����Ϣ(���������飬���������ֶ��������飬�ֶ�������WHERE����)
	ReduceSQL(strSQL,&strTableNames,nTableNum,&strFieldNames,nFieldNum,&strWhere);
	if(0 == nTableNum)
	{
		check_error(-1,"DeleteData-> SQL Select condition Error!");
		return -1;
	}

	SE_STREAM pStream = NULL;
	rc = SE_stream_create(m_pConnectObj,&pStream);
    check_error( rc, "DeleteData->SE_stream_create"); 

	char *strTemp = new char[SE_MAX_SQL_MESSAGE_LENGTH];
	strTemp[0] = 0;
//	memset(strTemp,0,sizeof(char)*SE_MAX_SQL_MESSAGE_LENGTH);
	sprintf((char*)strTemp,"delete from %s where %s",strTableNames[0],strWhere);
	rc = SE_stream_prepare_sql (pStream, strTemp); 
	check_error(rc, "DeleteData->SE_stream_prepare_sql"); 
	rc = SE_stream_execute (pStream); 
	check_error(rc, "DeleteData->SE_stream_execute"); 
	delete[] strTemp;

	free(strWhere);

	for(int k=0; k<nTableNum;k++)
		free(strTableNames[k]);
	free(strTableNames);

	for(k=0;k<nFieldNum;k++)
		free(strFieldNames[k]);
	free(strFieldNames);

	SE_stream_free(pStream);


	if(rc == SE_SUCCESS)
		return 1;
	return rc;
}

// ��ȡ����������
long CSdeCommand::GetServerTime(struct tm& tm_server)
{
	time_t ltime = SE_connection_get_server_time(m_pConnectObj);

	struct tm * time = localtime( &ltime );

//	tm_server = time;
	memcpy(&tm_server, time, sizeof(tm));

	return 1;
}


// ���ܣ���ȡ���ݱ�ṹ��Ϣ
// ������pParam          ���ر��ֶνṹ��������
//       nparamNums      �����ֶνṹ����
//       strName         ������
//       strOwnerGDB     GDB��ӵ����
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
// ע�⣺�ú�������pParam�Ŀռ�������ں�������ɣ�����pParam�Ŀռ���CSdeCommand��
//       �Լ����ͷţ����ⲿֻ����CSdeCommand����������Χ�ڣ�ʹ��������ָ�룬���
//       ��Ҫȫ����ʹ�ã���Ҫ�����ݸ��Ƴ���
long CSdeCommand::GetTableInfo(IDistParameter** pParam,int* nPamramNum,const char* strName,const char* strOwnerGDB)
{
	*nPamramNum = 0;
	*pParam = NULL;

	//1.�ж����ݿ����Ӷ���ָ���Ƿ����
	if(NULL == m_pConnectObj) {
		check_error(-1,"GetTableInfo->Database not connected!");
		return -1;
	}

	//2.�ж��Ƿ������ͬ���ֶ���Ϣ��������ھ�û�б�Ҫ�����»�ȡ
	if(NULL != m_pParams)  
	{
		//����������б���ֶ���Ϣ��ֱ�ӷ��سɹ�
		if(CompareString(strName,m_strCurTableName)==0) //�Ƚ�SDE�����Ƿ�һ��
		{
			*pParam = m_pParams;
			*nPamramNum = m_nParamCount;
			return 1;
		}

		//��ͬ����ɾ��
		delete [] m_pParams; m_pParams = NULL;
		m_nParamCount = 0;
	}
	m_strCurTableName[0] = 0;
//	memset(m_strCurTableName,0,sizeof(char)*SE_MAX_TABLE_LEN);
	
	//3.��ȡָ������ֶ���Ϣ
	SE_COLUMN_DEF* pTemp = NULL;
	m_nParamCount = 0;
	long rc = SE_table_describe(m_pConnectObj, strName, (short*) &m_nParamCount, &pTemp);
    if(rc != SE_SUCCESS) 
	{
		check_error(rc,"GetTableInfo->SE_table_describe");
		if(NULL != pTemp)
			free(pTemp); 
		return  rc;
	}


	//4.���ֶ���Ϣ���Ƶ��Զ���ṹ
	m_pParams = new IDistParameter[m_nParamCount];
	if(NULL == m_pParams)
	{
		m_nParamCount = 0;
		check_error(-1,"GetTableInfo->Alloc memory failed!");
		return -1;
	}
	for(int i=0; i<m_nParamCount;i++)
	{
		m_pParams[i].fld_bNullAllow = pTemp[i].nulls_allowed;
		m_pParams[i].fld_nDecimal = pTemp[i].decimal_digits;
		m_pParams[i].fld_nRowIdType = pTemp[i].row_id_type;
		m_pParams[i].fld_nSize = pTemp[i].size;
		m_pParams[i].fld_nType = pTemp[i].sde_type;
		strcpy(m_pParams[i].fld_strName,pTemp[i].column_name);
		strcpy(m_pParams[i].fld_strAliasName,pTemp[i].column_name);
	}


	//5.Ϊ���ر�����ֵ
	*pParam = m_pParams;
	*nPamramNum = m_nParamCount;
	strcpy(m_strCurTableName,strName); //���浱ǰ����������

    //################################################################################//
	//########## ���ϴ����ȡָ������ֶ���Ϣ����������ǻ�ȡ��Ӧ�ֶεı��� ##########//
	//���˵��
	//����C-API����û���ṩ��ȡ���ݱ��ֶα����Ĺ��ܣ�����ֻ��ͨ��ֱ�ӷ������ݿ�ķ�ʽ��
	//��ȡ�ֶα�����Ϣ�����巽����ͨ�� OwnerName��SdeLayerName �ڱ� GDB_OBJECTCLASSES��
	//����Ӧ�ļ�¼��ȡ����ID(classid)���ڱ� GDB_FIELDINFO �У�����ID��FieldName����ȡ��
	//�α��� 
	// SQLServer -> Database.OwnerName.SdeLayerName(or SdeTableName)  
	// Orcale    -> OwnerName.SdeLayerName(or SdeTableName)  
	//################################################################################//

	//����ͼ������Ϣ���ֽ��ȡ����ӵ����
	char sOwnerName[160] = {0};
	char sSdeName[160] = {0};
//	memset(sOwnerName,0,sizeof(char)*160);
//	memset(sSdeName,0,sizeof(char)*160);

	int nLenName = strlen(strName);
	int nPosA = -1,nPosB = -1;  //��¼����'.'��λ��
	for(int k=0; k<nLenName; k++)
	{
		char ch = strName[k];
		if(ch == '.')
		{
			if(nPosA > 0)
			{
				nPosB = k;break;
			}
			else
				nPosA = k;
		}
	}
	if(nPosA > 0 && nPosB >0)
	{
		memcpy(sOwnerName,strName+nPosA+1,nPosB-nPosA-1);
		memcpy(sSdeName,strName+nPosB+1,nLenName-nPosB-1);
	}
	else if(nPosA >0)
	{
		memcpy(sOwnerName,strName,nPosA);
		memcpy(sSdeName,strName+nPosA+1,nLenName-nPosA-1);
	}
	else
	{
		strcpy(sSdeName,strName);//û��owner��DB��Ϣ
		strcpy(sOwnerName,m_strUserName);  //һ�㲻�ᷢ��(ֻ�����ƣ�û��ӵ���ߺ����ݿ���)
	}//m_strUserName ��ֵ�ǵ�ǰ���ݿ����Ӷ���ĵ�½�û���

	//6. �� GDB_OBJECTCLASSES ���л�ȡ��Ӧ���ͼ���ClassId

	//ע�⣺��ȡ�ֶα�������Ϊ���ݿ��ѯʱ����Where�������ַ���
	//      ���ִ�Сд�������Ȳ�ѯ�������м�¼��Ȼ�����αȽϡ�
	char strTempGDB[160];
	strTempGDB[0] = 0;
//	memset(strTempGDB,0,sizeof(char)*160);
	if(strOwnerGDB == NULL)
		strcpy(strTempGDB,"SDE"); //Ĭ��Ϊ��ǰ���ݿ����Ӷ���ĵ�½�û���
	else
		strcpy(strTempGDB,strOwnerGDB);   //��½�û��봴���û���һ��ʱ����Ҫָ��

	// ����SQL��ѯ��䣨����SDEͼ���������ݿ������������ClassId��
	int nFldCount = 3;  //������Ҫ��ѯ���ֶ�
	char** strFldNames = (char **) malloc (nFldCount * sizeof(char *));
	strFldNames[0] = (char*) malloc(sizeof(char)*SE_MAX_COLUMN_LEN);
	strcpy(strFldNames[0],"ID"); 
	strFldNames[1] = (char*) malloc(sizeof(char)*SE_MAX_COLUMN_LEN);
	strcpy(strFldNames[1],"OWNER"); 
	strFldNames[2] = (char*) malloc(sizeof(char)*SE_MAX_COLUMN_LEN);
	strcpy(strFldNames[2],"NAME"); 


	int nTableCount = 1;  //�����ѯ�ı�����
	char** strTables = (char **) malloc (nTableCount * sizeof(char *));
	strTables[0] = (char*) malloc(sizeof(char)*SE_MAX_TABLE_LEN);
	sprintf(strTables[0],"%s.GDB_OBJECTCLASSES",strTempGDB);//strTempGDB���û�ָ����Ĭ��Ϊ��ǰ�û�


	char strWhere[SE_MAX_MESSAGE_LENGTH]={0};
	sprintf(strWhere,"UPPER(OWNER)='%s' and UPPER(NAME)='%s'",_strupr(_strdup(sOwnerName)), _strupr(_strdup(sSdeName)));

	SE_STREAM  pStream = NULL;  //SDE������ָ�붨��
	rc = SE_stream_create (m_pConnectObj, &pStream);
	check_error(rc,  "GetTableInfo->SE_stream_create");

    SE_QUERYINFO queryinfo;
	rc = SE_queryinfo_create (&queryinfo);   //���ٲ�ѯ�ṹ�ռ�
	check_error ( rc, "GetTableInfo->SE_stream_create");
	rc = SE_queryinfo_set_tables(queryinfo,nTableCount,(const char**)strTables,NULL);  //���ñ���
	check_error (rc,  "GetTableInfo->SE_queryinfo_set_table");
	rc = SE_queryinfo_set_columns (queryinfo,nFldCount,(const char **)strFldNames);  //�����ֶ��� 
	check_error (rc, "GetTableInfo->SE_queryinfo_set_columns");
	rc = SE_queryinfo_set_where_clause(queryinfo,strWhere);  //����Where����
	check_error ( rc, "GetTableInfo->SE_queryinfo_set_where_clause"); 
	rc = SE_stream_query_with_info(pStream, queryinfo);    //��ѯ����������������
	check_error (rc, "GetTableInfo->SE_stream_query_with_info");
	rc = SE_stream_execute (pStream);
	check_error(rc, "GetTableInfo->SE_stream_execute");

	long nID = -1;
	long nDatasetId = -1;

	char strTempOwner[SE_MAX_MESSAGE_LENGTH]={0};
	char strTempName[SE_MAX_MESSAGE_LENGTH]={0};

	while (SE_SUCCESS == SE_stream_fetch(pStream))
	{
/*		memset(strTempOwner,0,sizeof(ACHAR)*SE_MAX_MESSAGE_LENGTH);
		memset(strTempName,0,sizeof(ACHAR)*SE_MAX_MESSAGE_LENGTH);

		rc = SE_stream_get_integer(pStream,1,&nID);//��ȡID������һ����ѯʹ��

		SE_WCHAR strOwner[SE_MAX_MESSAGE_LENGTH];
		memset(strOwner,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);

		rc = SE_stream_get_nstring(pStream,2,strOwner);  // "OWNER"
		check_error(rc, "GetTableInfo->SE_stream_get_string");

		setlocale(LC_ALL, "chs");
		wcstombs(strTempOwner,strOwner,SE_MAX_MESSAGE_LENGTH);

		SE_WCHAR strName[SE_MAX_MESSAGE_LENGTH];
		memset(strName,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);

		rc = SE_stream_get_nstring(pStream,3,strName);   // "NAME"
		check_error(rc, "GetTableInfo->SE_stream_get_string");

		wcstombs(strTempName,strName,SE_MAX_MESSAGE_LENGTH);
		setlocale(LC_ALL, "C");

		if(CompareString(sOwnerName,strTempOwner)==0 && 
		   CompareString(sSdeName,strTempName)==0) //�Ƚ��Ƿ��������
*/		{
			SE_stream_get_integer(pStream,1,&nID);//��ȡID������һ����ѯʹ��
			break;
		}
	}

	//�ͷ�
	for(i=0; i<nFldCount;i++)
		free(strFldNames[i]);
	free(strFldNames);

	free(strTables[0]);
	free(strTables);

	SE_stream_free(pStream);
	pStream = NULL;
	SE_queryinfo_free(queryinfo);
	queryinfo = NULL;

	if(nID < 1) return 1;  //û���ҵ���˵����ͼ����û��ע����Ϣ��Ҳû�б�����ֱ���˳�

	//7.����ClassIdֵ���ֶ������ڱ� GDB_FIELDINFO ��ȡ��Ӧ���ֶα���

	// ����SQL��ѯ��䣨����ClassIdȡ���ֶεı�����
	nFldCount = 2;
	strFldNames = (char **) malloc (nFldCount * sizeof(char*));
	strFldNames[0] = (char*) malloc(sizeof(char)*SE_MAX_COLUMN_LEN);
	strFldNames[1] = (char*) malloc(sizeof(char)*SE_MAX_ALIAS_LEN);
	strcpy(strFldNames[0],"FieldName");
	strcpy(strFldNames[1],"AliasName");

	nTableCount = 1;
	strTables = (char **) malloc (nTableCount * sizeof(char *));
	strTables[0] = (char*) malloc(sizeof(char)*SE_MAX_TABLE_LEN);
	sprintf(strTables[0],"%s.GDB_FIELDINFO",strTempGDB);//strTempGDB���û�ָ����Ĭ��Ϊ��ǰ�û�


//	char strWhere[SE_MAX_MESSAGE_LENGTH]={0};
	memset(strWhere, 0, SE_MAX_MESSAGE_LENGTH);
	sprintf(strWhere,"ClassID=%d",nID);
	rc = SE_stream_create (m_pConnectObj, &pStream);
	check_error(rc,  "GetTableInfo->SE_stream_create");
	rc = SE_queryinfo_create (&queryinfo);   //���ٲ�ѯ�ṹ�ռ�
	check_error (rc, "GetTableInfo->SE_stream_create");	
	rc = SE_queryinfo_set_tables(queryinfo,nTableCount,(const char**)strTables,NULL); 
	check_error (rc,  "GetTableInfo->SE_queryinfo_set_table");  //���ñ���
	rc = SE_queryinfo_set_columns (queryinfo,nFldCount,(const char **)strFldNames);
	check_error ( rc, "GetTableInfo->SE_queryinfo_set_columns");  //�����ֶ���
	rc = SE_queryinfo_set_where_clause(queryinfo,strWhere);  //����Where����
	check_error ( rc, "GetTableInfo->SE_queryinfo_set_where_clause"); 
	rc = SE_stream_query_with_info(pStream, queryinfo);    //��ѯ����������������
	check_error (rc, "GetTableInfo->SE_stream_query_with_info");
	rc = SE_stream_execute (pStream);
	check_error(rc, "GetTableInfo->SE_stream_execute");

	char strFName[SE_MAX_MESSAGE_LENGTH]={0};
	char strAFName[SE_MAX_MESSAGE_LENGTH]={0};

	setlocale(LC_ALL, "chs");
	while (SE_SUCCESS == SE_stream_fetch(pStream))
	{
		SE_WCHAR str[SE_MAX_MESSAGE_LENGTH];

		// "FieldName"
		memset(str,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);
		rc = SE_stream_get_nstring(pStream,1,str);  
		check_error(rc, "GetTableInfo->SE_stream_get_string");
		wcstombs(strFName,str,SE_MAX_MESSAGE_LENGTH);

		// "AliasName"
		memset(str,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);
		rc = SE_stream_get_nstring(pStream,2,str);  
		check_error(rc, "GetTableInfo->SE_stream_get_string");
		wcstombs(strAFName,str,SE_MAX_MESSAGE_LENGTH);

		for(i=0; i<m_nParamCount;i++)
		{
			if(CompareString(m_pParams[i].fld_strName,strFName)==0)
			{
				strcpy(m_pParams[i].fld_strAliasName,strAFName);//��ѯ����
				break;
			}
		}
	}
	setlocale(LC_ALL, "C");

	//�ͷ�
	for(i=0; i<nFldCount;i++)
		free(strFldNames[i]);
	free(strFldNames);

	free(strTables[0]);
	free(strTables);

	SE_stream_free(pStream);
	pStream = NULL;
	SE_queryinfo_free(queryinfo);
	queryinfo = NULL;

	return 1;

}


// ���ܣ��������ݱ�
// ������strName         �����ı�����
//       pParam          ���ֶνṹ��������
//       nparamNums      �ֶνṹ����
//       strKeyword      �ؼ���
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::CreateTable(const char* strName,IDistParameter* pParam,
							  int nPamramNum,const char* strKeyword)
{
	//�ж����ݿ����Ӷ���ָ���Ƿ����
	if(NULL == m_pConnectObj) 
	{
		check_error(-1,"\nDatabase not connected!"); 
		return -1;
	}
	if(NULL == strName || NULL == pParam || nPamramNum<=0)
	{
		check_error(-1,"\nInput Parameter is invalidation!"); 
		return -1;
	}

	//���������Ƿ���RowId�ֶ�
	char strRowIdName[SE_MAX_COLUMN_LEN];
	strRowIdName[0] = 0;
//	memset(strRowIdName,0,sizeof(char)*SE_MAX_COLUMN_LEN);
	BOOL bFound = FALSE;
	for(int i=0; i<nPamramNum; i++)
	{
		if(pParam[i].fld_nRowIdType ==SE_REGISTRATION_ROW_ID_COLUMN_TYPE_SDE)
		{
			bFound = TRUE;
			memcpy(strRowIdName,pParam[i].fld_strName,sizeof(char)*32);
			break;
		}
	}

	if(bFound== FALSE) 
	{
		check_error(-1,"The Table has not RowId Field!");	
		return -1;
	}

	// �����ֶ���Ϣ
	SE_COLUMN_DEF* pTemp = new SE_COLUMN_DEF[nPamramNum];
	if(NULL == pTemp)  return -1;
	for(int j=0; j<nPamramNum; j++)
	{
		strcpy(pTemp[j].column_name,pParam[j].fld_strName);
		pTemp[j].decimal_digits = pParam[j].fld_nDecimal;
		pTemp[j].nulls_allowed = pParam[j].fld_bNullAllow;
		pTemp[j].row_id_type = pParam[j].fld_nRowIdType;
		pTemp[j].sde_type = pParam[j].fld_nType;
		pTemp[j].size = pParam[j].fld_nSize;
	}
	// �����û���
	long rc = SE_table_create(m_pConnectObj, strName, nPamramNum,pTemp,strKeyword);
	if(rc != SE_SUCCESS)
	{
		check_error (rc, "CreateTable->SE_table_create");
		delete[] pTemp;
		return rc;
	}
	delete[] pTemp;

	

	//ע��RowId�ֶ���Ϣ
	SE_REGINFO reginfo;
	rc = SE_reginfo_create (&reginfo); //����ע��ṹ
	check_error (rc, "CreateTable->SE_reginfo_create");

	rc = SE_registration_get_info (m_pConnectObj, strName, reginfo);//��ȡ��ǰ��ע����Ϣ
	check_error (rc, "CreateTable->SE_registration_get_info");

	rc = SE_reginfo_set_rowid_column (reginfo, strRowIdName, SE_REGISTRATION_ROW_ID_COLUMN_TYPE_SDE);
	check_error (rc,"CreateTable->SE_reginfo_set_rowid_column");  //ע��RowId
 

	rc = SE_registration_alter (m_pConnectObj, reginfo);  //����ע���
	check_error (rc, "CreateTable->SE_registration_alter");

	SE_reginfo_free (reginfo);  //�ͷ�

    if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;
}


// ���ܣ�ɾ��ָ�����ݱ�
// ������strName         ������
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::DropTable(const char* strName)
{
	//�ж����ݿ����Ӷ���ָ���Ƿ����
	if(NULL == m_pConnectObj) {
		check_error(-1,"\nDatabase not connected!"); return -1;
	}
	long rc = SE_table_delete (m_pConnectObj, strName);
	check_error (rc, "DropTable->SE_table_delete");

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;
}


// ���ܣ������������Ϣ����SHAPE
// ������pShape      ���ش���SHAPE��ָ��
//       strLyName   SDEͼ������
//       ptArray     ��������飨1�������㣬�������β�㲻�ȴ����ߣ��պϴ�����)
//       nPtCount    ��������
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::CreateShapeByCoord(int* pShape,const char* strLyName,IDistPoint* ptArray,int nPtCount)
{
	long rc = -1;
	if(strLyName==NULL || ptArray == NULL || nPtCount<=0)
	{
		check_error (-1, "CreateShapeByCoord->fun input param is ivialed!");
		return 0;
	}

	//��ȡͼ��������Ϣ
	SE_LAYERINFO pLyInfo = NULL;
	rc = SE_layerinfo_create(NULL,&pLyInfo);
	check_error (rc, "CreateShapeByCoord->SE_layerinfo_create");
	rc = SE_layer_get_info(m_pConnectObj,strLyName,"SHAPE",pLyInfo);
	check_error (rc, "CreateShapeByCoord->SE_layer_get_info");

	//��ȡͼ��������Ϣ
	SE_COORDREF pCoordref=NULL;
	rc = SE_coordref_create(&pCoordref);
	check_error(rc,"CreateShapeByCoord->SE_coordref_create");
	rc = SE_layerinfo_get_coordref (pLyInfo,pCoordref);
	check_error(rc,"CreateShapeByCoord->SE_layerinfo_get_coordref");

	//����SHAPE
	if(m_tempShape != NULL)
	{
		SE_shape_free(m_tempShape);
		m_tempShape = NULL;
	}
	rc = SE_shape_create(pCoordref,&m_tempShape);
	check_error(rc,"CreateShapeByCoord->SE_shape_create");

	if(nPtCount == 1)
	{
		SE_POINT pt;
		pt.x = ptArray[0].x; pt.y = ptArray[0].y;
		rc = SE_shape_generate_point(1,&pt,NULL,NULL,m_tempShape);
		check_error(rc,"CreateShapeByCoord->SE_shape_generate_point");
	}
	else 
	{
		SE_POINT ptStart,ptEnd;
		ptStart.x = ptArray[0].x; ptStart.y = ptArray[0].y;
		ptEnd.x = ptArray[nPtCount-1].x; ptEnd.y = ptArray[nPtCount-1].y;

		if((fabs(ptStart.x-ptEnd.x)<0.001)&&(fabs(ptStart.y-ptEnd.y)<0.001))
		{
			if(nPtCount<4)
			{
				check_error (-1, "CreateShapeByCoord->fun input param(point array) is ivialed!");
				SE_coordref_free(pCoordref); SE_layerinfo_free(pLyInfo);
				SE_shape_free(m_tempShape); m_tempShape = NULL;
				return 0;
			}

			SE_POINT* pts = (SE_POINT*) malloc (nPtCount * sizeof(SE_POINT));
			for(int k=0; k<nPtCount; k++)
			{
				pts[k].x = ptArray[k].x; pts[k].y = ptArray[k].y;
			}
			rc = SE_shape_generate_polygon(nPtCount,1,NULL,pts,NULL,NULL,m_tempShape);
			check_error(rc,"CreateShapeByCoord->SE_shape_generate_polygon");
			free(pts);
		}
		else
		{
			SE_POINT* pts = (SE_POINT*) malloc (nPtCount * sizeof(SE_POINT));
			for(int k=0; k<nPtCount; k++)
			{
				pts[k].x = ptArray[k].x; pts[k].y = ptArray[k].y;
			}
			rc = SE_shape_generate_line(nPtCount,1,NULL,pts,NULL,NULL,m_tempShape);
			check_error(rc,"CreateShapeByCoord->SE_shape_generate_line");
			free(pts);
		}

	}
	SE_coordref_free(pCoordref);
	SE_layerinfo_free(pLyInfo);

	if(rc == SE_SUCCESS)
	{
		*pShape = (int)(&m_tempShape);
		return 1;
	}
	else
	{
		*pShape = 0;
		return 0;
	}	
}

// ���ܣ�����ͼ����ָ�������ļ�¼(ֻ����������ͬ���ֶ�)
// ������strSrcLyName    Դͼ������
//       strTargeLyName  Ŀ��ͼ������
//       strWhere        ָ������(�磺OBJECTID>100)
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::CopyRecordByCondition(const char* strTargeLyName,const char* strSrcLyName,const char* strWhere, bool bAddVersion, const char* strUserName, const char* strCurTime)
{
	if((NULL==strSrcLyName)|| (NULL==strTargeLyName)||(NULL==strWhere)) 
	{
		check_error(-1,"CopyRecordByCondition->Input param is invialde!");
		return 0;
	}

	//��Դͼ���ֶ���Ϣ
	short nSrcCount = 0;
	SE_COLUMN_DEF* pTempSrc = NULL;	
	long rc = SE_table_describe(m_pConnectObj, strSrcLyName,&nSrcCount, &pTempSrc);
	if(rc != SE_SUCCESS) 
	{
		check_error(rc,"CopyRecordByCondition->SE_table_describe");
		return  rc;
	}

	char** strSrcArray = new char*[nSrcCount];
	for(int i=0; i<nSrcCount; i++)
	{
		strSrcArray[i] = new char[SE_MAX_COLUMN_LEN];
		memset(strSrcArray[i],0,sizeof(char)*SE_MAX_COLUMN_LEN);
	}
	int nPos = 0;
	for(i=0; i<nSrcCount; i++)
	{
		if(pTempSrc[i].row_id_type == kRowIdColumnSde) continue;
		strcpy(strSrcArray[nPos],pTempSrc[i].column_name);
		nPos++;
	}

	//��Ŀ��ͼ���ֶ���Ϣ
	short nTagCount = 0;
	SE_COLUMN_DEF* pTempTag = NULL;
	rc = SE_table_describe(m_pConnectObj, strTargeLyName,&nTagCount, &pTempTag);
	if(rc != SE_SUCCESS) 
	{
		for(i=0; i<nSrcCount; i++)
			delete [] strSrcArray[i];
		delete[] strSrcArray;
		check_error(rc,"CopyRecordByCondition->SE_table_describe");
		return  rc;
	}
	char** strTagArray = new char*[nTagCount];
	for(i=0; i<nTagCount; i++)
	{
		strTagArray[i] = new char[SE_MAX_COLUMN_LEN];
		memset(strTagArray[i],0,sizeof(char)*SE_MAX_COLUMN_LEN);
	}
	nPos = 0;
	for(i=0; i<nTagCount; i++)
	{
		if(pTempTag[i].row_id_type == kRowIdColumnSde) continue;
		strcpy(strTagArray[nPos],pTempTag[i].column_name);
		nPos++;
	}

	//��ȡͼ��Ĺ����ֶ�
	int nFldCount = 0;
	if(nTagCount > nSrcCount)
		nFldCount = nSrcCount;
	else
		nFldCount = nTagCount;

	char** strFldArray = new char*[nFldCount];
	for(i=0; i<nFldCount; i++)
	{
		strFldArray[i] = new char[SE_MAX_COLUMN_LEN];
		strFldArray[i][0] = 0;
//		memset(strFldArray[i],0,sizeof(char)*SE_MAX_COLUMN_LEN);
	}

	int nFldNum = 0;
	for(i=0; i<nSrcCount; i++)
	{
		for(int j=0; j<nTagCount; j++)
		{
			if(strSrcArray[i][0] == 0) continue;
			if(CompareString(strSrcArray[i],strTagArray[j])==0)
			{
				strcpy(strFldArray[nFldNum],strSrcArray[i]);
				nFldNum++;
				break;
			}
		}
	}
	//�ͷ�
	for(i=0; i<nSrcCount; i++)
		delete [] strSrcArray[i];
	delete[] strSrcArray;
	for(i=0; i<nTagCount; i++)
		delete [] strTagArray[i];
	delete[] strTagArray;


	char strFldNames[2048]={0};
	strFldNames[0] = 0;
//	memset(strFldNames,0,sizeof(char)*2048);
	//���ɲ�ѯFLD�ֶ�����
	nPos =0;
	for(i=0; i<nFldNum;i++)
	{
		int nLen = strlen(strFldArray[i]);
		memcpy(strFldNames+nPos,strFldArray[i],nLen);
		nPos = nPos+nLen;
		strFldNames[nPos]=',';
		nPos++;
	}
	for(i=0; i<nFldCount; i++)
		delete [] strFldArray[i];
	delete[] strFldArray;

	//��ѯSDEԴ����
	char strSQL[1024]={0};
	sprintf(strSQL,"F:%sT:%s,W:%s,##",strFldNames,strSrcLyName,strWhere);
	IDistRecordSet* pRcdSet = NULL;
	if(SelectData(strSQL,&pRcdSet)!=1)
	{
		if(pRcdSet != NULL) pRcdSet->Release(); return 0;
	}
	int nRcdCount = pRcdSet->GetRecordCount();
	if(nRcdCount == 0)
	{
		if(pRcdSet != NULL) 
			pRcdSet->Release(); return 2;
	}

	if (bAddVersion) // ��Ӱ汾��Ϣ����д��ʷͼ��ʱ��Ҫ�õ���
	{
		long nVersion = 0;
		// ��ȡ���汾��
		char strSQL2[1024]={0};
		sprintf(strSQL2,"F:%s,T:%s,O:%s,W:%s,##","VERSION",strTargeLyName,"order by VERSION desc",strWhere);
		IDistRecordSet* pRcdSet2 = NULL;
		if(SelectData(strSQL2,&pRcdSet2)!=1)
		{
			if(pRcdSet2 != NULL) pRcdSet2->Release(); return 0;
		}
		int nRcdCount2 = pRcdSet2->GetRecordCount();
		if(nRcdCount2 > 0)
		{
			if (pRcdSet2->BatchRead())
			{
				char strValue[32] = {0};
				pRcdSet2->GetValueAsString(strValue,0);
				nVersion = atol(strValue);
			}
		}
		if(pRcdSet2 != NULL) 
			pRcdSet2->Release();

		if (nVersion < 1)
		{
			nVersion = 1;
		}
		else
			nVersion ++;

		char strOtherSQL[1024]={0};
		sprintf(strOtherSQL,"F:%s%s,T:%s,##",strFldNames,"VERSION,USERNAME",strTargeLyName);
		char** strValues = new char* [nFldNum+2]; // VERSION,USERNAME
		for(i=0; i<nFldNum+2; i++)
		{
			strValues[i] = new char[500];
			strValues[i][0] = 0;
//			memset(strValues[i],0,sizeof(char)*500);
		}

		while(pRcdSet->BatchRead()==1)
		{
			for(int k=0; k<nFldNum; k++)
				pRcdSet->GetValueAsString(strValues[k],k);

			sprintf(strValues[nFldNum], "%d", nVersion);
			lstrcpy(strValues[nFldNum+1],strUserName);

			int nRowId =0;
			if(1 != InsertDataAsString(&nRowId,strOtherSQL,strValues))
			{
				pRcdSet->Release();
				for(int j=0; j<nFldNum+2; j++)
				{
					delete[] strValues[j];
				}
				delete[] strValues;
				return 0;
			}
		}

		pRcdSet->Release();
		for(i=0; i<nFldNum+2; i++)
		{
			delete[] strValues[i];
		}
		delete[] strValues;
	}
	else
	{
		char strOtherSQL[1024]={0};
		sprintf(strOtherSQL,"F:%sT:%s,##",strFldNames,strTargeLyName);
		char** strValues = new char* [nFldNum];
		for(i=0; i<nFldNum; i++)
		{
			strValues[i] = new char[500];
			strValues[i][0] = 0;
//			memset(strValues[i],0,sizeof(char)*500);
		}

		while(pRcdSet->BatchRead()==1)
		{
			for(int k=0; k<nFldNum; k++)
				pRcdSet->GetValueAsString(strValues[k],k);

			int nRowId =0;
			if(1 != InsertDataAsString(&nRowId,strOtherSQL,strValues))
			{
				pRcdSet->Release();
				for(int j=0; j<nFldNum+1; j++)
				{
					delete[] strValues[j];
				}
				delete[] strValues;
				return 0;
			}
		}

		pRcdSet->Release();
		for(i=0; i<nFldNum; i++)
		{
			delete[] strValues[i];
		}
		delete[] strValues;
	}

	return 1;
}

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>

// ��ͼ�㿽������
long CSdeCommand::CopyRecordsAcrossLayer(LPCTSTR lpDestLayer, LPCTSTR lpSrcLayer, CUIntArray& rowIds, LPCTSTR lpUserName, int nEditId)
{
	if((NULL==lpSrcLayer)|| (NULL==lpDestLayer)) 
	{
		check_error(-1,"CopyRecordByCondition->Input param is invialde!");
		return 0;
	}

	//��Դͼ���ֶ���Ϣ
	short nSrcCount = 0;
	SE_COLUMN_DEF* pTempSrc = NULL;	
	long rc = SE_table_describe(m_pConnectObj, lpSrcLayer,&nSrcCount, &pTempSrc);
	if(rc != SE_SUCCESS) 
	{
		check_error(rc,"CopyRecordByCondition->SE_table_describe");
		return  rc;
	}

	char** strSrcArray = new char*[nSrcCount];
	for(int i=0; i<nSrcCount; i++)
	{
		strSrcArray[i] = new char[SE_MAX_COLUMN_LEN];
		strSrcArray[i][0] = 0;
//		memset(strSrcArray[i],0,sizeof(char)*SE_MAX_COLUMN_LEN);
	}
	int nPos = 0;
	for(i=0; i<nSrcCount; i++)
	{
		if(pTempSrc[i].row_id_type == kRowIdColumnSde) continue;
		strcpy(strSrcArray[nPos],pTempSrc[i].column_name);
		nPos++;
	}

	//��Ŀ��ͼ���ֶ���Ϣ
	short nTagCount = 0;
	SE_COLUMN_DEF* pTempTag = NULL;
	rc = SE_table_describe(m_pConnectObj, lpDestLayer,&nTagCount, &pTempTag);
	if(rc != SE_SUCCESS) 
	{
		for(i=0; i<nSrcCount; i++)
			delete [] strSrcArray[i];
		delete[] strSrcArray;
		check_error(rc,"CopyRecordByCondition->SE_table_describe");
		return  rc;
	}
	char** strTagArray = new char*[nTagCount];
	for(i=0; i<nTagCount; i++)
	{
		strTagArray[i] = new char[SE_MAX_COLUMN_LEN];
		strTagArray[i][0] = 0;
//		memset(strTagArray[i],0,sizeof(char)*SE_MAX_COLUMN_LEN);
	}
	nPos = 0;
	for(i=0; i<nTagCount; i++)
	{
		if(pTempTag[i].row_id_type == kRowIdColumnSde) continue;
		strcpy(strTagArray[nPos],pTempTag[i].column_name);
		nPos++;
	}

	//��ȡͼ��Ĺ����ֶ�
	int nFldCount = 0;
	if(nTagCount > nSrcCount)
		nFldCount = nSrcCount;
	else
		nFldCount = nTagCount;

	char** strFldArray = new char*[nFldCount];
	for(i=0; i<nFldCount; i++)
	{
		strFldArray[i] = new char[SE_MAX_COLUMN_LEN];
		strFldArray[i][0] = 0;
//		memset(strFldArray[i],0,sizeof(char)*SE_MAX_COLUMN_LEN);
	}

	int nFldNum = 0;
	for(i=0; i<nSrcCount; i++)
	{
		for(int j=0; j<nTagCount; j++)
		{
			if(strSrcArray[i][0] == 0) continue;
			if(CompareString(strSrcArray[i],strTagArray[j])==0)
			{
				strcpy(strFldArray[nFldNum],strSrcArray[i]);
				nFldNum++;
				break;
			}
		}
	}
	//�ͷ�
	for(i=0; i<nSrcCount; i++)
		delete [] strSrcArray[i];
	delete[] strSrcArray;
	for(i=0; i<nTagCount; i++)
		delete [] strTagArray[i];
	delete[] strTagArray;


	char strFldNames[2048]={0};
	memset(strFldNames,0,sizeof(char)*2048);
	//���ɲ�ѯFLD�ֶ�����
	nPos =0;
	for(i=0; i<nFldNum;i++)
	{
		int nLen = strlen(strFldArray[i]);
		memcpy(strFldNames+nPos,strFldArray[i],nLen);
		nPos = nPos+nLen;
		strFldNames[nPos]=',';
		nPos++;
	}
	for(i=0; i<nFldCount; i++)
		delete [] strFldArray[i];
	delete[] strFldArray;

//	time_t ltime = SE_connection_get_server_time(m_pConnectObj);

//	struct tm * time = localtime( &ltime );
	tm time;
	GetServerTime(time);

	CString strTime;
	strTime.Format("%4d%02d%02d%02d%02d%02d", time.tm_year+1900, time.tm_mon+1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
//	strTime.Format("%4d-%02d-%02d", time.tm_year+1900, time.tm_mon+1, time.tm_mday);

	CString strUserName;
	for (int kk = 0; kk< rowIds.GetCount(); kk++) 
	{
		CString strOldId;
		strOldId.Format("%d", rowIds.GetAt(kk));

		//��ѯSDEԴ����
		char strSQL[1024]={0};
		sprintf(strSQL,"F:%sT:%s,W:OBJECTID=%d,##",strFldNames,lpSrcLayer,rowIds.GetAt(kk));

		IDistRecordSet* pRcdSet = NULL;
		if(SelectData(strSQL,&pRcdSet)!=1)
		{
			if(pRcdSet != NULL) pRcdSet->Release(); return 0;
		}
		int nRcdCount = pRcdSet->GetRecordCount();
		if(nRcdCount == 0)
		{
			if(pRcdSet != NULL) 
				pRcdSet->Release(); return 2;
		}

		{
			char strOtherSQL[1024]={0};
			sprintf(strOtherSQL,"F:%s%s,T:%s,##",strFldNames,"OLDID,EDITUSER,EDITTIME,EDITROWID",lpDestLayer);
			char** strValues = new char* [nFldNum+4]; // OLDID,USERNAME,TIME,EDITROWID
			for(i=0; i<nFldNum+4; i++)
			{
				strValues[i] = new char[500];
				strValues[i][0] = 0;
//				memset(strValues[i],0,sizeof(char)*500);
			}

			while(pRcdSet->BatchRead()==1)
			{
				for(int k=0; k<nFldNum; k++)
					pRcdSet->GetValueAsString(strValues[k],k);

				lstrcpy(strValues[nFldNum], strOldId);
				lstrcpy(strValues[nFldNum+1],lpUserName);
				lstrcpy(strValues[nFldNum+2],strTime);

				CString strEditRowId;
				strEditRowId.Format("%d", nEditId);

				lstrcpy(strValues[nFldNum+3],strEditRowId);

				int nRowId =0;
				if(1 != InsertDataAsString(&nRowId,strOtherSQL,strValues))
				{
					pRcdSet->Release();
					for(int j=0; j<nFldNum+4; j++)
					{
						delete[] strValues[j];
					}
					delete[] strValues;
					return 0;
				}
			}

			pRcdSet->Release();
			for(i=0; i<nFldNum+4; i++)
			{
				delete[] strValues[i];
			}
			delete[] strValues;
		}
	}


	return 1;
}

// ���ܣ�����SHAPE�ṹָ�룬��ȡ�������
// ������pShapeType  ����SHAPE����
//       ptArray     �������������
//       nPtCount    ������������
//       pShape      ����SHAPE��ָ��(ע����SE_SHAPE*)
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::GetShapePointArray(int* pShapeType,IDistPoint** ptArray,int* pPtCount,int* pShape)
{
	//SG_NIL_SHAPE                       0
	//SG_POINT_SHAPE                     1
	//SG_LINE_SHAPE                      2
	//SG_SIMPLE_LINE_SHAPE               4
	//SG_AREA_SHAPE                      8
	//SG_SHAPE_CLASS_MASK              255
	//SG_SHAPE_MULTI_PART_MASK         256
	//SG_MULTI_POINT_SHAPE             257
	//SG_MULTI_LINE_SHAPE              258
	//SG_MULTI_SIMPLE_LINE_SHAPE       260
	//SG_MULTI_AREA_SHAPE              264
	SE_SHAPE* pTemp = (SE_SHAPE*)(*pShape);
	long rc = 0,ltype =0;
	rc = SE_shape_get_type (*pTemp, &ltype);
	check_error (rc, "GetShapePointArray->SE_shape_get_type");
	*pShapeType = ltype;

	SE_POINT* pts = NULL;
	long nAllPtNum = 0;

	rc = SE_shape_get_num_points(*pTemp,0,0,&nAllPtNum);  //��ȡShape�����е�����
	check_error (rc, "GetShapePointArray->SE_shape_get_num_points");
	pts = (SE_POINT*) malloc (nAllPtNum*sizeof(SE_POINT));
	rc = SE_shape_get_all_points(*pTemp,SE_DEFAULT_ROTATION,NULL,NULL,pts,NULL,NULL);
	check_error (rc, "GetShapePointArray->SE_shape_get_all_points");
	*pPtCount = nAllPtNum;
	if(*pPtCount == 0 || rc != SE_SUCCESS)
	{
		free(pts); return rc;
	}
	if(NULL != m_pTempPtArray) 
	{
		delete[] m_pTempPtArray;
		m_pTempPtArray = NULL;
	}
	m_pTempPtArray = new IDistPoint[*pPtCount];
	for(int i=0; i<*pPtCount; i++)
	{
		m_pTempPtArray[i].x = pts[i].x;
		m_pTempPtArray[i].y = pts[i].y;
	}
	free(pts); 

	*ptArray = m_pTempPtArray;

	return 1;
}




// ���ܣ����ص�ǰ���ݿ�������ͼ�������
// ������strLyNameArray   ����ͼ��������
//       pCount           ������������
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::GetCurUserAllLyName(char*** strLyNameArray,long* pCount)
{
	long rc =-1;
	*strLyNameArray = NULL; 
	*pCount = 0;

	//Ԥ����ɾ��
	if(NULL != m_strAllLayerNameArray)
	{
		for(int i=0; i<m_nLayerNameCount; i++)
			free(m_strAllLayerNameArray[i]);
		free(m_strAllLayerNameArray);
		m_strAllLayerNameArray = NULL;
		m_nLayerNameCount = 0;
	}

	//��ͼ����Ϣ�б�
	SE_LAYERINFO *layer_list=NULL;
	rc = SE_layer_get_info_list(m_pConnectObj,&layer_list,(long*)&m_nLayerNameCount);
	if(rc != SE_SUCCESS)
	{
		check_error (rc, "GetAllLyNameInDB->SE_layer_get_info_list");
		return rc;
	}

	//���ݿ���û�е�ǰ�û��ܷ��ʵ�ͼ��
	if(m_nLayerNameCount == 0) return -1;
	
	m_strAllLayerNameArray = (char**) malloc (sizeof(char*)*m_nLayerNameCount);
	if(NULL == m_strAllLayerNameArray)
	{
		SE_layer_free_info_list(m_nLayerNameCount,layer_list);
		m_nLayerNameCount = 0;
		return -1;
	}

	int nTempCount = m_nLayerNameCount;
	char strColumn[160] = {0};
	for(int i=0; i<m_nLayerNameCount; i++)
	{
		m_strAllLayerNameArray[i] = (char*) malloc (sizeof(char)*160);
		m_strAllLayerNameArray[i][0] = 0;
//		memset(m_strAllLayerNameArray[i],0,sizeof(char)*160);
		rc = SE_layerinfo_get_spatial_column(layer_list[i],m_strAllLayerNameArray[i],strColumn);
		if(rc != SE_SUCCESS)
		{
			for(int j=0; j<=i;j++)
				free(m_strAllLayerNameArray[j]);
			free(m_strAllLayerNameArray); 
			m_strAllLayerNameArray = NULL;
			m_nLayerNameCount = 0;
			break;
		}
	}
	SE_layer_free_info_list(nTempCount,layer_list);
	if(rc == SE_SUCCESS)
	{
		*strLyNameArray = m_strAllLayerNameArray;
		*pCount = m_nLayerNameCount;
		return 1;
	}
	else
		return rc;
}
 

// ���ܣ�����ͼ����Ϣ
// ������strTableName       ͼ�����������
//       strSpacialFldName  ͼ��ͱ�����Ŀռ��ֶ�
//       pLyInfo            ͼ����ϢSE_LAYERINFOָ��
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::GetLayerInfo(const char* strTableName,const char* strSpacialFldName,void** pLyInfo)
{
	long rc = SE_layer_get_info(m_pConnectObj,strTableName,strSpacialFldName,(SE_LAYERINFO)*pLyInfo);
	check_error (rc, "GetLayerInfo->SE_layer_get_info");

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;
}


// ���ܣ�����ͼ����Ϣ
// ������strLyName        ͼ������
//       strShapeFldName  ͼ��ͱ�����Ŀռ��ֶ�
//       pType            ����ͼ����������
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::GetLayerType(const char* strLyName,const char* strShapeFldName,long* pType)
{
	long rc = -1;
	SE_LAYERINFO layerInfo = NULL;
	rc = SE_layerinfo_create(NULL,&layerInfo);
	check_error (rc, "GetLayerType->SE_layerinfo_create");

	rc = SE_layer_get_info(m_pConnectObj,strLyName,strShapeFldName,layerInfo);
	check_error (rc, "GetLayerType->SE_layer_get_info");

	rc = SE_layerinfo_get_shape_types(layerInfo,pType);
	check_error (rc, "GetLayerType->SE_layerinfo_get_shape_types");

	SE_layerinfo_free(layerInfo);

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;
}

// ���ܣ�����ͼ������ϵ��Ϣ
// ������strLyName        ͼ�����������
//       strShapeFldName  ͼ��ͱ�����Ŀռ��ֶ�
//       pCoordref        ͼ������ϵ��Ϣָ��
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::GetLayerCoordref(const char* strLyName,const char* strShapeFldName,void** pCoordref)
{
	long rc = -1;
	SE_LAYERINFO pLyInfo = NULL;
	rc = SE_layerinfo_create(NULL,&pLyInfo);
	check_error (rc, "GetLayerCoordref->SE_layerinfo_create");

	rc = SE_layer_get_info(m_pConnectObj,strLyName,strShapeFldName,pLyInfo);
	check_error (rc, "GetLayerCoordref->SE_layer_get_info");
	
	SE_COORDREF pTempCoordref=NULL;
	rc = SE_coordref_create(&pTempCoordref);
	check_error(rc,"ReadSymbolInfo->SE_coordref_create");

	rc = SE_layerinfo_get_coordref (pLyInfo,pTempCoordref);
	check_error(rc,"ReadSymbolInfo->SE_layerinfo_get_coordref");

	rc = SE_coordref_duplicate(pTempCoordref,(SE_COORDREF)(*pCoordref));
	check_error(rc,"ReadSymbolInfo->SE_coordref_duplicate");

	SE_coordref_free(pTempCoordref);
	SE_layerinfo_free(pLyInfo);

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;
}



// ���ܣ�����SDEͼ��(���CADʵ�����)
// ������strTableName      SDE�������������ݱ�����
//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ�㴴����������Ա������Ӹ��ֶ�)
//       dScale            ͼ�������
//       ox,oy             ����ϵԭ��
//       dGridSize         �����С
//       lTypeMask         ͼ������
//       strKeyword        �ؼ���
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::CreateLayer(const char* strTableName,
							  const char* strSpacialFldName,
							  double dScale,
							  double ox,double oy,
							  double dGridSize,
							  long   lTypeMask,
							  const char* strKeyword)
{
	//�ж����ݿ����Ӷ���ָ���Ƿ����
	if(NULL == m_pConnectObj) {
		check_error(-1,"\nDatabase not connected!"); return -1;
	}

	SE_COORDREF  coordref;   //����ϵ����
    SE_LAYERINFO layerInfo;  //����Ϣ
	// �����������ϵ�ο�
	long rc = SE_coordref_create (&coordref);
	check_error (rc, "CreateLayer->SE_coordref_create");

	//���üٶ�����Դ���������
	rc = SE_coordref_set_xy (coordref,ox,oy,dScale);//��ԭ��x,y�ͱ�����
	check_error (rc, "CreateLayer->SE_coordref_set_xy");

	//���䲢��ʼ������Ϣ�ṹlayerInfo
	rc = SE_layerinfo_create (coordref, &layerInfo);
	check_error (rc, "CreateLayer->SE_layerinfo_create");

	//���û�ͼ����ߴ�
    rc = SE_layerinfo_set_grid_sizes (layerInfo,dGridSize,0,0);
	check_error (rc, "CreateLayer->SE_layerinfo_set_grid_sizes");

	//����ͼ������
	rc = SE_layerinfo_set_shape_types(layerInfo,lTypeMask);
	check_error (rc, "CreateLayer->SE_layerinfo_set_shape_types");

	//���ùؼ���
	rc = SE_layerinfo_set_creation_keyword (layerInfo, strKeyword);
	check_error (rc, "CreateLayer->SE_layerinfo_set_creation_keyword");

	//���Ա���ռ��ͨ��ָ���ֶν�������
	rc = SE_layerinfo_set_spatial_column (layerInfo, strTableName, strSpacialFldName); 
	check_error (rc, "CreateLayer->SE_layerinfo_set_spatial_column");

	//������
	rc = SE_layer_create (m_pConnectObj, layerInfo, 0,0);
	check_error(rc, "CreateLayer->SE_layer_create");

	SE_coordref_free(coordref);
	SE_layerinfo_free(layerInfo);

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;	
}


// ���ܣ�ɾ��SDEͼ��(���CADʵ�����)
// ������strTableName      SDE�������������ݱ�����
//       strSpacialFldName ͼ�������Ա�����Ŀռ��ֶ���(ͼ��ɾ����������Ա���ɾ�����ֶ�)
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::DropLayer(const char* strTableName,const char* strSpacialFldName)
{
	//�ж����ݿ����Ӷ���ָ���Ƿ����
	if(NULL == m_pConnectObj) {
		check_error(-1,"\nDatabase not connected!"); return -1;
	}

	long rc = SE_layer_delete (m_pConnectObj, strTableName,strSpacialFldName);
	check_error(rc, "DropLayer->SE_layer_delete");

	if(rc == SE_SUCCESS)
		return 1;
	else
		return rc;	
}



// ���ܣ�Ϊ�û�����ͼ�����Ȩ��
// ������strUser                  ��Ȩ�û�����
//       strLayerName             ͼ������
//       strFldNameArray          ��Ȩ�ֶ��б�
//       nFldCount                �ֶ�����
//       nPrivilege               ����Ȩ�ޣ��ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|'��
//       bAllowPrivilegesToOther  �Ƿ�������Ȩ����Ȩ�������û�    
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::GrantUserAccessLayerPrivilege(const char *strUser,const char *strLayerName,
		                                        char** strFldNameArray,int nFldCount,
		                                       long nPrivilege,BOOL bAllowPrivilegesToOther)
{
	if(NULL == strUser || NULL == strLayerName)
	{
		check_error(-1,"GrantUserAccessPrivilege->the function paramer is error!");
		return -1;
	}

	long rc = SE_connection_start_transaction (m_pConnectObj);

	if(NULL != strFldNameArray)
	{
		for(int i=0; i<nFldCount; i++)
		{
			rc = SE_layer_grant_access(m_pConnectObj,strLayerName,strFldNameArray[i],nPrivilege,bAllowPrivilegesToOther,strUser);
		}
	}
	else
	{
		IDistParameter *pParam = NULL;
		int nParamNum = 0;
		if((rc=GetTableInfo(&pParam,&nParamNum,strLayerName)) ==1)
		{
			for(int j=0; j<nParamNum; j++)
				rc = SE_layer_grant_access(m_pConnectObj,strLayerName,pParam[j].fld_strName,nPrivilege,bAllowPrivilegesToOther,strUser);
		}
	}


	if(rc == SE_SUCCESS)
	{
		rc = SE_connection_commit_transaction (m_pConnectObj);
		if(rc == SE_SUCCESS)
			rc = 1;
	}
	else
		SE_connection_rollback_transaction(m_pConnectObj);

	return rc;
}


// ���ܣ�ɾ���û�����ͼ��ָ���ֶεķ���Ȩ��
// ������strUser                  ��Ȩ�û�����
//       strLayerName             ͼ������
//       strFldNameArray          ��Ȩ�ֶ��б�(ֵΪNULʱ��ʾͼ���������ֶ�)
//       nFldCount                �ֶ�����
//       nPrivilege               ����Ȩ�ޣ��ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|'�� 
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::RemoveUserAccessLayerPrivilege(const char *strUser,const char *strLayerName,
		                                         char** strFldNameArray,int nFldCount,long nPrivilege)
{
	if(NULL == strUser || NULL == strLayerName)
	{
		check_error(-1,"RemoveUserAccessLayerPrivilege->the function paramer is error!");
		return -1;
	}

	long rc = SE_connection_start_transaction (m_pConnectObj);

	if(NULL != strFldNameArray)
	{
		for(int i=0; i<nFldCount; i++)
		{
			rc = SE_layer_revoke_access(m_pConnectObj,strLayerName,strFldNameArray[i],nPrivilege,strUser);
		}
	}
	else
	{
		IDistParameter *pParam = NULL;
		int nParamNum = 0;
		if((rc=GetTableInfo(&pParam,&nParamNum,strLayerName)) ==1)
		{
			for(int j=0; j<nParamNum; j++)
				rc = SE_layer_revoke_access(m_pConnectObj,strLayerName,pParam[j].fld_strName,nPrivilege,strUser);
		}
	}


	if(rc == SE_SUCCESS)
	{
		rc = SE_connection_commit_transaction (m_pConnectObj);
		if(rc == SE_SUCCESS)
			rc = 1;
	}
	else
		SE_connection_rollback_transaction(m_pConnectObj);

	return rc;
}




// ���ܣ�Ϊ�û����乤�������Ȩ��
// ������strUser                  ��Ȩ�û�����
//       strTableName             ����������
//       nPrivilege               ����Ȩ�ޣ��ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|'��
//       bAllowPrivilegesToOther  �Ƿ�������Ȩ����Ȩ�������û�    
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::GrantUserAccessTablePrivilege(const char *strUser,const char* strTableName,
		                                        long nPrivilege,BOOL bAllowPrivilegesToOther)
{
	long rc = -1;

	rc = SE_table_grant_access(m_pConnectObj,strTableName,nPrivilege,bAllowPrivilegesToOther,strUser);

	if(rc == SE_SUCCESS)
		rc = 1;

	return rc;
}


// ���ܣ�ɾ���û�������ָ������Ȩ��
// ������strUser                  ��Ȩ�û�����
//       strTableName             ����������
//       nPrivilege               ����Ȩ��(�ο�DIST_ENUM_ACCESS_PRIV����,���Ȩ������� '|')
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::RemoveUserAccessTablePrivilege(const char *strUser,const char* strTableName,long nPrivilege)
{
	long rc = -1;

	rc = SE_table_revoke_access(m_pConnectObj,strTableName,nPrivilege,strUser);

	if(rc == SE_SUCCESS)
		rc = 1;

	return rc;
}


// ���ܣ�����ָ��ͼ������
// ������strLyName        ͼ������
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::LockLayerData(const char* strLyName)
{
	return 1;
}


// ���ܣ�����ָ�����ݱ�
// ������strName          ������
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::LockTable(const char* strName)
{
	return 1;
}


// ���ܣ�����ָ�����ݱ��е�һ����¼
// ������strName          ������
//       nIndexRow        ָ����
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::LockTableARow(const char* strName, int nIndexRow)
{
	return 1;
}


// ���ܣ�����ָ���������
// ������pObjs             ����ID����
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::LockApplicationObj(int* pObjs)
{
	return 1;
}


//��������ϵ�͹�դͼ��Ķ���
long CSdeCommand::CreateCoordinate(void)
{
	return 1;
}

long CSdeCommand::ReadRasterData(void)
{
	return 1;
}

long CSdeCommand::WriteRasterData(void)
{
	return 1;
}


// ���ܣ���ʼ�������
// ��������
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::BeginTrans()
{
	long rc = SE_connection_start_transaction (m_pConnectObj);
	
	if(rc==SE_SUCCESS)
		rc = 1;
	else
		check_error(rc, "BeginTrans->SE_connection_start_transaction");

	return rc;
}


// ���ܣ������ύ
// ��������
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::CommitTrans()
{
	long rc = SE_connection_commit_transaction (m_pConnectObj);
	if(rc==SE_SUCCESS)
		rc = 1;
	else
		check_error(rc, "BeginTrans->SE_connection_commit_transaction");

	return rc;
}


// ���ܣ�����ع�
// ��������
// ���أ��ɹ����� 1��ʧ�ܷ��� 0�������ο�����
long CSdeCommand::RollbackTrans()
{
	long rc = SE_connection_rollback_transaction(m_pConnectObj);
	if(rc==SE_SUCCESS)
		rc = 1;
	else
		check_error(rc, "BeginTrans->SE_connection_rollback_transaction");

	return rc;
}

// ���ܣ��ͷŶ���
// ��������
// ���أ���
void CSdeCommand::Release(void)
{
	delete this;
}



//================================ CSdeRecordSet======================================//


//���ܣ����캯��
//��������
//���أ���
CSdeRecordSet::CSdeRecordSet()
{
	//��Ϣָ��
	m_pConnectObj = NULL;
	m_pStream     = NULL;
	m_pParams     = NULL;
	m_nParamNum   = 0;
	m_nTableNum   = 0;
	m_SqlInfo     = NULL;
	m_strOldWhere = new char[SE_MAX_SQL_MESSAGE_LENGTH];
	memset(m_strOldWhere,0,sizeof(char)*SE_MAX_SQL_MESSAGE_LENGTH);

	//��¼�������
	m_nPointer = 0;
	m_bBatch = FALSE;
	m_nRecordCount = 0;
	m_pRowIdData = NULL;

    //��ʱ����ռ�
	m_nValue16 = 0;
	m_nValue32 = 0;
	m_fValue = 0.0;
	m_dValue = 0.0;
	memset(m_strValue,0,sizeof(char)*SE_MAX_MESSAGE_LENGTH);
	memset(m_strValueW,0,sizeof(SE_WCHAR)*SE_MAX_MESSAGE_LENGTH);
	m_pBlob.blob_buffer = NULL;
	m_pBlob.blob_length = 0;
	memset(&m_tmValue,0,sizeof(tm));
	m_shape = NULL;//��Ҫ����
}


//���ܣ���������
//��������
//���أ���
CSdeRecordSet::~CSdeRecordSet()
{
	if(NULL != m_pParams)
		delete[] m_pParams;

	if(NULL != m_pStream)
		SE_stream_free(m_pStream);

	if(m_SqlInfo != NULL)
		SE_queryinfo_free(m_SqlInfo);

	if(NULL != m_pBlob.blob_buffer)
	{
		free(m_pBlob.blob_buffer);
		m_pBlob.blob_buffer = NULL;
		m_pBlob.blob_length = 0;
	}

	if(NULL != m_shape)
		SE_shape_free(m_shape);

	if(NULL != m_pRowIdData)
		delete[] m_pRowIdData;

	if(NULL != m_strOldWhere)
		delete[] m_strOldWhere;

}



//���ܣ����ý��������ѯ��Ϣ���ռ������Ϣ��(��CSdeCommandʹ��)
//������pStream    �����
//      sqlInfo    ��ѯ��Ϣ�ṹ
//      pFilter    �ռ���˽ṹ
//      pData      RowId����
//���أ���
void CSdeRecordSet::SetInfo(SE_STREAM* pStream,SE_QUERYINFO* sqlInfo,
							SE_FILTER* pFilter,CDistIntArray* pData)
{

	int i;
	long rc;

	//�����ѯ��¼��ָ��
	m_pStream   = *pStream; 

	//����ռ��������
	m_pFilter   =  pFilter;  
	
	//�����ѯ�ṹ��Ϣ
	if(m_SqlInfo != NULL){
		SE_queryinfo_free(m_SqlInfo);
		m_SqlInfo = NULL;
	}
	rc = SE_queryinfo_create(&m_SqlInfo);
	check_error(rc, "SetInfo->SE_queryinfo_create");
	rc = SE_queryinfo_duplicate(*sqlInfo,m_SqlInfo);//���Ʋ�ѯ�ṹ��Ϣ
	check_error(rc, "SetInfo->SE_queryinfo_duplicate");


	//��������ȡ���ݿ����Ӷ���ָ��
	rc = SE_stream_get_connection(m_pStream,&m_pConnectObj); 
	check_error(rc, "SetInfo->SE_stream_get_connection");

	//���ݲ�ѯ�ṹ��Ϣ��ȡWhere����
	m_strOldWhere[0] = 0;
//	memset(m_strOldWhere,0,sizeof(char)*SE_MAX_SQL_MESSAGE_LENGTH);
	rc = SE_queryinfo_get_where_clause(m_SqlInfo,m_strOldWhere);  
	check_error(rc, "SetInfo->SE_queryinfo_get_where_clause");

	//���ݲ�ѯ�ṹ��Ϣ��ȡ��ǰ���ֶ�����
	char** FldNames = NULL;
	rc = SE_queryinfo_get_columns(m_SqlInfo,(long*)&m_nParamNum,&FldNames);
	check_error(rc, "SetInfo->SE_queryinfo_get_columns");
    
	//���ݲ�ѯ�ṹ��Ϣ��ȡ��ǰ��������
	char** TableNames = NULL;
	rc = SE_queryinfo_get_tables(m_SqlInfo,&m_nTableNum,&TableNames);
	check_error(rc, "SetInfo->SE_queryinfo_get_tables");

	//�ͷ�ԭ�����ֶ�����
	if(NULL != m_pParams) {
		delete[] m_pParams; m_pParams = NULL;
	}
	m_pParams = new SE_COLUMN_DEF[m_nParamNum];

	//���SHAPE�ֶ�����
	char strShapeName[32] = {0};
//	memset(strShapeName,0,sizeof(char)*32);
	for(i=0; i<m_nParamNum; i++)
	{
		rc = SE_stream_describe_column(m_pStream,i+1,&m_pParams[i]); //ȡ��ǰ�����ֶ�������Ϣ
		check_error(rc, "SetInfo->SE_queryinfo_get_tables");
		if(m_pParams[i].sde_type == kShape)
		{
			strcpy(strShapeName,m_pParams[i].column_name); //�õ�Shape�����ֶ�����
		}
		if(m_pParams[i].row_id_type == kRowIdColumnSde)
		{
			strcpy(m_strRowIdName,m_pParams[i].column_name);
		}
	}
	//����ͼ����Ϣ��ȡ�ռ�����ϵ��Ϣ
	if( 0 != strShapeName[0])
	{
		SE_LAYERINFO layerInfo = NULL;
		rc = SE_layerinfo_create(NULL,&layerInfo);
		check_error(rc, "SetInfo->SE_layerinfo_create");
		
		//Ѱ����ռ��ֶ�����ı���
		char  strTemp[160] ={0};
		for(int k=0; k<m_nTableNum; k++)
		{
			strTemp[0] = 0;
//			memset(strTemp,0,sizeof(char)*160);
			strcpy(strTemp,TableNames[k]);
			if(SE_SUCCESS == SE_layer_get_info(m_pConnectObj,strTemp,strShapeName,layerInfo))
				break;
		}
		rc = SE_coordref_create(&m_coordref);
		check_error(rc, "SetInfo->SE_coordref_create");
		rc = SE_layerinfo_get_coordref (layerInfo,m_coordref);
		check_error(rc, "SetInfo->SE_layerinfo_get_coordref");
		SE_layerinfo_free(layerInfo);
	}
	//3.���ü�¼�����ͼ�¼RowId����
	if(NULL !=m_pRowIdData)
	{
		delete m_pRowIdData;
		m_pRowIdData = NULL;
	}
	m_pRowIdData = pData;
	//4.���÷����α��ʼֵ
	m_nPointer = 0;
	m_bBatch   = FALSE;
}

//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ
//������nIndex ���(��0��ʼ)
//���أ������ֶ�ֵ(��ȡ��Ҫ����ʵ�����ͣ�ǿ��ת��)
void* CSdeRecordSet::operator[] (int nIndex)
{
	//1. ��������Ϣ��ȡ��������ص��ֶ���Ϣ(�ֶε�����)
	long rc;
	short nColumnNum = 0;
	SE_COLUMN_DEF ColumnDef;

	nIndex = nIndex+1;  //ת����SDE����1��ſ�ʼ
	rc = SE_stream_describe_column(m_pStream,nIndex,&ColumnDef); //ȡ�ֶ�������Ϣ
	check_error( rc, "operator[int]->SE_stream_describe_column");
	
	if(SE_SUCCESS != rc) return NULL;  //nIndex�Ƿ���ֱ�ӷ���NULL
	tm tempDate;

	//2. ���ݲ�ͬ�����ͷ�����Ӧ��ֵ
	switch(ColumnDef.sde_type) {
	case kInt16: 	
			rc = SE_stream_get_smallint(m_pStream, nIndex,(SHORT*)&m_nValue16);
			if(rc == SE_NULL_VALUE)
				m_nValue16=0;
			else if(rc !=SE_SUCCESS)
				check_error(rc, "operator[int]->SE_stream_get_smallint");
			return (void*) &m_nValue16;
	case kInt32: 	
			rc = SE_stream_get_integer(m_pStream, nIndex,(LONG*)&m_nValue32);
			if(rc == SE_NULL_VALUE)
				m_nValue32=0;
			else if(rc !=SE_SUCCESS)
				check_error(rc, "operator[int]->SE_stream_get_integer");
			return (void*) &m_nValue32;
	case kFloat32: 
			rc = SE_stream_get_float(m_pStream, nIndex,(FLOAT*)&m_fValue);
			if(rc == SE_NULL_VALUE)
				m_fValue=0.000;
			else if(rc !=SE_SUCCESS)
				check_error(rc, "operator[int]->SE_stream_get_float");
			return (void*) &m_fValue;
	case kFloat64:
			rc = SE_stream_get_double(m_pStream, nIndex,(double*)&m_dValue);
			if(rc == SE_NULL_VALUE)
				m_dValue=0.000;
			else if(rc !=SE_SUCCESS)
				check_error(rc, "operator[int]->SE_stream_get_double");
			return (void*) &m_dValue;
	case kString: 
			rc = SE_stream_get_string(m_pStream, nIndex,m_strValue);
			if(rc == SE_NULL_VALUE)
				strcpy(m_strValue," ");
			else if(rc !=SE_SUCCESS)
				check_error(rc, "operator[int]->SE_stream_get_string");
			return (void*) m_strValue;
	case kNString: //ע���ǿ��ַ��������ת���ɵ��ַ�������
			setlocale(LC_ALL, "chs");
			rc = SE_stream_get_nstring(m_pStream, nIndex,m_strValueW);
			if(rc == SE_SUCCESS)
			{
				m_strValue[0] = 0;
//				memset(m_strValue,0,sizeof(char)*SE_MAX_MESSAGE_LENGTH);
				wcstombs(m_strValue,m_strValueW,SE_MAX_MESSAGE_LENGTH);
			}
			else if(rc == SE_NULL_VALUE)
				strcpy(m_strValue," "); 
			else 
				check_error(rc, "operator[int]->SE_stream_get_nstring");
			setlocale(LC_ALL, "C");
			return (void*) m_strValue;
	case kBLOB:
	case kCLOB:
	case kNCLOB:
/*			rc = SE_stream_get_blob(m_pStream, nIndex,&m_pBlob);
		check_error(rc, "operator[int]->SE_stream_get_blob");
		return (void*) &m_pBlob;
*/
		strcpy(m_strValue, "BLOB");
		return (void*) m_strValue;
	case kDate:	
		rc = SE_stream_get_date(m_pStream, nIndex,(tm*)&tempDate);
		sprintf(m_strValue,"%04d-%02d-%02d",tempDate.tm_year,tempDate.tm_mon+1,tempDate.tm_mday);
		check_error(rc, "operator[int]->SE_stream_get_date");
		return (void*) m_strValue;
	case kShape:
			if(NULL != m_shape)
				SE_shape_free(m_shape);
			rc = SE_shape_create(m_coordref,&m_shape);
			check_error(rc, "operator[int]->SE_shape_create");
			rc = SE_stream_get_shape(m_pStream, nIndex,m_shape);
			check_error(rc, "operator[int]->SE_stream_get_shape");
			return (void*) &m_shape;
	default:
			break;
	}

	return NULL;
}


//���ܣ�����ָ�������ƣ�ȡ��Ӧ�ֶε�ֵ
//������strFldName �ֶ���
//���أ������ֶ�ֵ(��ȡ��Ҫ����ʵ�����ͣ�ǿ��ת��)
void* CSdeRecordSet::operator[] (const char* strFldName)
{
	//1. ��������Ϣ��ȡ��������ص��ֶ���Ϣ(�ֶε�����)
	long rc;
	int nIndex =1;
	SE_COLUMN_DEF ColumnDef;
	
	while((rc = SE_stream_describe_column(m_pStream,nIndex,&ColumnDef)) ==SE_SUCCESS) //ȡ�ֶ�������Ϣ
	{
		check_error(rc, "operator[strFldName]->SE_stream_describe_column");
		if(0 == CompareString(ColumnDef.column_name,strFldName)) break;
		nIndex++;
	}

	if(rc != SE_SUCCESS) return NULL;

	return (*this)[nIndex-1];
}

long CSdeRecordSet::GetValueAsString(long& nType, char *strValue,int nIndex)
{
	//1. ��������Ϣ��ȡ��������ص��ֶ���Ϣ(�ֶε�����)
	long rc;
	short nColumnNum = 0;
	SE_COLUMN_DEF ColumnDef;

	nIndex = nIndex+1;
	rc = SE_stream_describe_column(m_pStream,nIndex,&ColumnDef); //ȡ�ֶ�������Ϣ
	check_error(rc, "GetValueAsString->SE_stream_describe_column");

	if(SE_SUCCESS != rc) return 0;  //nIndex�Ƿ���ֱ�ӷ���NULL

	nType = ColumnDef.sde_type;

	return GetValueAsString(strValue, nIndex);
}

//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ,���ֵΪ�ַ���
//������strValue ���ؽ���ַ��� 
//      nIndex   ���(��0��ʼ)
//���أ�1 ��ʾ�ɹ���0 ��ʾʧ�ܣ������ο�����
long CSdeRecordSet::GetValueAsString(char *strValue,int nIndex)
{
	//1. ��������Ϣ��ȡ��������ص��ֶ���Ϣ(�ֶε�����)
	long rc;
	short nColumnNum = 0;
	SE_COLUMN_DEF ColumnDef;

	nIndex = nIndex+1;
	rc = SE_stream_describe_column(m_pStream,nIndex,&ColumnDef); //ȡ�ֶ�������Ϣ
	check_error(rc, "GetValueAsString->SE_stream_describe_column");
	
	tm tempDate;

	if(SE_SUCCESS != rc) return 0;  //nIndex�Ƿ���ֱ�ӷ���NULL

	//2. ���ݲ�ͬ�����ͷ�����Ӧ��ֵ
	switch(ColumnDef.sde_type) {
	case kInt16: 	
			rc = SE_stream_get_smallint(m_pStream, nIndex,(SHORT*)&m_nValue16);
			if(rc == SE_SUCCESS)
			{
				sprintf(strValue,"%d",m_nValue16);
				return 1;
			}
			else if(rc == SE_NULL_VALUE)
				strcpy(strValue,"0");
			else
				check_error(rc, "GetValueAsString->SE_stream_get_smallint");
			return rc;

	case kInt32: 	
			rc = SE_stream_get_integer(m_pStream, nIndex,(LONG*)&m_nValue32);
			if(rc == SE_SUCCESS)
			{
				sprintf(strValue,"%d",m_nValue32);
				return 1;
			}
			else if(rc == SE_NULL_VALUE)
				strcpy(strValue,"0");
			else
				check_error(rc, "GetValueAsString->SE_stream_get_integer");
			return rc;
	case kFloat32: 
			rc = SE_stream_get_float(m_pStream, nIndex,(FLOAT*)&m_fValue);
			if(rc == SE_SUCCESS)
			{
				sprintf(strValue,"%0.3f",m_fValue);
				return 1;
			}
			else if(rc == SE_NULL_VALUE)
				strcpy(strValue,"0.000");
			else
				check_error(rc, "GetValueAsString->SE_stream_get_float");
			return rc;
	case kFloat64:
			rc = SE_stream_get_double(m_pStream, nIndex,(double*)&m_dValue);
			if(rc == SE_SUCCESS)
			{
				sprintf(strValue,"%0.3f",m_dValue);
				return 1;
			}
			else if(rc == SE_NULL_VALUE)
				strcpy(strValue,"0.000");
			else
				check_error(rc, "GetValueAsString->SE_stream_get_double");
			return rc;
	case kString: 
			rc = SE_stream_get_string(m_pStream, nIndex,m_strValue);
			if(rc == SE_SUCCESS)
			{
				strcpy(strValue,m_strValue);
				return 1;
			}
			else if(rc == SE_NULL_VALUE)
				strcpy(strValue," ");
			else
				check_error(rc, "GetValueAsString->SE_stream_get_string");
			return rc;

	case kNString: 
			setlocale(LC_ALL, "chs");
			rc = SE_stream_get_nstring(m_pStream, nIndex,m_strValueW);
			if(rc == SE_SUCCESS)
			{
				wcstombs(strValue,m_strValueW,SE_MAX_MESSAGE_LENGTH);//�� -> ��
				return 1;
			}
			else if(rc == SE_NULL_VALUE)
				strcpy(strValue," ");
			else
				check_error(rc, "GetValueAsString->SE_stream_get_nstring");
			setlocale(LC_ALL, "C");
			return rc;
	case kBLOB:
	case kCLOB:
	case kNCLOB:
/*
		rc = SE_stream_get_blob(m_pStream, nIndex,&m_pBlob);
		check_error(rc, "GetValueAsString->SE_stream_get_blob");
		m_nValue32 = int(&m_pBlob);
		sprintf(strValue,"%d",m_nValue32);
		if(rc == SE_SUCCESS)
		return 1;
		else
		return rc;
*/
		strcpy(strValue,"BLOB");
		return 1;
	case kDate:	
		rc = SE_stream_get_date(m_pStream, nIndex,(tm*)&tempDate);
		check_error( rc, "GetValueAsString->SE_stream_get_date");

		if(rc == SE_SUCCESS)
		{
			sprintf(strValue,"%04d-%02d-%02d",tempDate.tm_year+1900,tempDate.tm_mon+1,tempDate.tm_mday);
			return 1;
		}
		else if(rc == SE_NULL_VALUE)
		{
			strcpy(strValue,"2008-1-1");  //�д�����
		}
		return rc;
	case kShape:
			if(NULL != m_shape)
				SE_shape_free(m_shape);
			SE_shape_create(m_coordref,&m_shape);
			rc = SE_stream_get_shape(m_pStream, nIndex,m_shape);
			check_error(rc, "GetValueAsString->SE_stream_get_shape");
//			m_nValue32 = int(&m_shape);
			m_nValue32 = int(m_shape);
			sprintf(strValue,"%d",m_nValue32);
			if(rc == SE_SUCCESS)
				return 1;
			else
				return rc;
	default:
			break;
	}

	return 0;
}

long CSdeRecordSet::GetValueAsString(long& nType, char *strValue,const char* strFldName)
{
	//1. ��������Ϣ��ȡ��������ص��ֶ���Ϣ(�ֶε�����)
	int nIndex =1;
	SE_COLUMN_DEF ColumnDef;

	while(SE_stream_describe_column(m_pStream,nIndex,&ColumnDef) ==SE_SUCCESS) //ȡ�ֶ�������Ϣ
	{
		if(0 == CompareString(ColumnDef.column_name,strFldName))
		{
			nType = ColumnDef.sde_type;
			return GetValueAsString(strValue,nIndex-1);
		}
		nIndex++;
	}

	return 0;
}

//���ܣ�����ָ������ţ�ȡ��Ӧ�ֶε�ֵ,���ֵΪ�ַ���
//������strValue   ���ؽ���ַ��� 
//      strFldName �ֶ���
//���أ�1 ��ʾ�ɹ���0 ��ʾʧ�ܣ������ο�����
long CSdeRecordSet::GetValueAsString(char *strValue,const char* strFldName)
{
	//1. ��������Ϣ��ȡ��������ص��ֶ���Ϣ(�ֶε�����)
	int nIndex =1;
	SE_COLUMN_DEF ColumnDef;
	
	while(SE_stream_describe_column(m_pStream,nIndex,&ColumnDef) ==SE_SUCCESS) //ȡ�ֶ�������Ϣ
	{
		if(0 == CompareString(ColumnDef.column_name,strFldName))
			return GetValueAsString(strValue,nIndex-1);
		nIndex++;
	}

	return 0;
}


//���ܣ����ζ�ȡ��ѯ���
//��������
//���أ��к�����¼����1����¼���귵��0
long CSdeRecordSet::BatchRead(void)
{
	long rc = 0;
	if(FALSE == m_bBatch)
	{
		//�ͷ�ԭ�в�ѯ�����
		if(NULL != m_pStream) 
			SE_stream_free(m_pStream);

		//��������
		rc = SE_stream_create(m_pConnectObj,&m_pStream); 
		check_error (rc, "BatchRead->SE_stream_create");

		//����ԭ�е�Where����
		rc = SE_queryinfo_set_where_clause(m_SqlInfo,m_strOldWhere);

		//��ѯ����������������
		rc = SE_stream_query_with_info(m_pStream, m_SqlInfo);  
		check_error (rc, "BatchRead->SE_stream_query_with_info");

		// �����ѯ���͹�����
		if(NULL != m_pFilter)// �趨�ռ��ѯ����(������)
		{
			rc = SE_stream_set_spatial_constraints(m_pStream, SE_SPATIAL_FIRST, FALSE, 1, m_pFilter);
			check_error(rc, "BatchRead->SE_stream_set_spatial_constraints");
		}

		// ִ�в�ѯ
		rc = SE_stream_execute (m_pStream);
		check_error(rc, "BatchRead->SE_stream_execute");

		//�������������
		m_bBatch = TRUE;
	}

	//�ӽ������ȡһ����¼����
	rc = SE_stream_fetch(m_pStream);  
	if(SE_SUCCESS == rc)
		return 1;
	else if(SE_FINISHED != rc)
		check_error(rc, "BatchRead->SE_stream_fetch");

	return rc;
}


//���ܣ������α�λ�ö�ȡ��Ӧ�ļ�¼
//��������
//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
long CSdeRecordSet::ReadARow(void)
{
	long rc;
	//�α�Խ�磬ֱ�ӷ���FALSE
	if(m_nPointer < 0 || m_nPointer > m_pRowIdData->GetLength()-1)
		return -1;

	m_bBatch = FALSE;

	//���ݼ�¼��ţ���ȡ��Ӧ��RowId
	int  nRowId = 0;
	char strTableName[160]={0};
	char strFldName[32]={0};
	
	nRowId = m_pRowIdData->GetAt(m_nPointer);

	if(nRowId < 0) return -1;

	//�����µ�Where����(ע�ⵥ����Ͷ����Ĳ��)
	char *strNewWhere = new char[SE_MAX_SQL_MESSAGE_LENGTH];
	strNewWhere[0] = 0;
//	memset(strNewWhere,0,sizeof(char)*SE_MAX_SQL_MESSAGE_LENGTH);
	if(1 == m_nTableNum)
		sprintf(strNewWhere,"%s And %s = %d",m_strOldWhere,m_strRowIdName,nRowId);
	else
		sprintf(strNewWhere,"%s And %s.%s = %d",m_strOldWhere,strTableName,m_strRowIdName,nRowId);
	rc = SE_queryinfo_set_where_clause(m_SqlInfo,strNewWhere);
	check_error (rc, "ReadARow->SE_queryinfo_set_where_clause");
	delete[] strNewWhere;


	//�ͷ�ԭ��������������
	if(NULL != m_pStream)
		SE_stream_free(m_pStream);
	rc = SE_stream_create(m_pConnectObj,&m_pStream);
	check_error (rc, "ReadARow->SE_stream_create");

	//��ѯ����������������
	rc = SE_stream_query_with_info(m_pStream, m_SqlInfo);    
	check_error (rc, "ReadARow->SE_stream_query_with_info");


	// �����ѯ���͹�����
	if(NULL != m_pFilter)// �趨�ռ��ѯ����(������)
	{
		rc = SE_stream_set_spatial_constraints(m_pStream, SE_SPATIAL_FIRST, FALSE, 1, m_pFilter);
		check_error(rc, "ReadARow->SE_stream_set_spatial_constraints");
	}

	// ִ�в�ѯ
	rc = SE_stream_execute (m_pStream);
	check_error(rc, "ReadARow->SE_stream_execute");

	// ��ȡ��¼����
    rc = SE_stream_fetch(m_pStream);
	check_error(rc, "ReadARow->SE_stream_fetch");

	if(SE_SUCCESS == rc)
		return 1;
	else
		return rc;
}


//���ܣ����ص�ǰ��ѯ�����µļ�¼���ĵ�һ����¼
//��������
//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
long CSdeRecordSet::MoveFirst(void)
{	
	m_nPointer = 0; 
	return ReadARow();
}


//���ܣ����ص�ǰ��ѯ�����µļ�¼�������һ����¼
//��������
//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
long CSdeRecordSet::MoveLast(void)
{
	m_nPointer = m_nRecordCount-1; 
	return ReadARow();
}


//���ܣ����ص�ǰ��ѯ�����µļ�¼����ǰ��¼����һ����¼
//��������
//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
long CSdeRecordSet::MoveNext(void)
{
	m_nPointer++;
	return ReadARow();
}


//���ܣ����ص�ǰ��ѯ�����µļ�¼����ǰ��¼����һ����¼
//��������
//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
long CSdeRecordSet::MovePrevious(void)
{	
	m_nPointer--;	
	return ReadARow();
}


//���ܣ����ص�ǰ��ѯ������ָ����һ����¼
//������nIndex   ָ����¼��(0 ... n-1)
//���أ������ɷ���TRUE��ʧ�ܷ���FALSE
long CSdeRecordSet::MoveTo(int nIndex)
{
	m_nPointer = nIndex;	
	return ReadARow();
}


//���ܣ�����ȡ��¼���ļ�¼����
//��������
//���أ����ؼ�¼����
int  CSdeRecordSet::GetRecordCount(void)
{
	if(NULL == m_pRowIdData)
		return 0;
	return m_pRowIdData->GetLength();
}


//���ܣ�����ȡ��¼�����ֶ�����
//��������
//���أ������ֶ�����
int  CSdeRecordSet::GetFieldNum(void)
{
	return m_nParamNum;
}


//���ܣ��ͷŶ���
//��������
//���أ���
void CSdeRecordSet::Release(void)
{
	delete this;
}





//================================ export function ======================================//

char* WINAPI ReturnErrorCodeInfo(long nErrorCode)
{
	static char strInfo[SE_MAX_MESSAGE_LENGTH ] ={0};
//	memset(strInfo,0,sizeof(char)*SE_MAX_MESSAGE_LENGTH);
	switch(nErrorCode)
	{
	    case  0:strcpy(strInfo,"\n Success!");break;
		case  1:strcpy(strInfo,"\n Success!");break;
		case -1:strcpy(strInfo,"\n failure!");break;
		case -2:strcpy(strInfo,"\n LAYERINFO pointer not initialized!");break;
		case -3:strcpy(strInfo,"\n The given shape has no annotation!");break;
		case -4:strcpy(strInfo,"\n STREAM LOADING OF SHAPES FINISHED!");break;
		case -5:strcpy(strInfo,"\n SDE NOT STARTED, CANNOT PERFORM FUNCTION !");break;
		case -6:strcpy(strInfo,"\n THE SPECIFIED SHAPE WAS LEFT UNCHANGED!");break;
		case -7:strcpy(strInfo,"\n THE NUMBER OF SERVER TASKS/CONNECTIONS IS @ MAXIMUM !");break;
		case -8:strcpy(strInfo,"\n IOMGR NOT ACCEPTING CONNECTION REQUESTS!");break;
		case -9:strcpy(strInfo,"\n CANNOT VALIDATE THE SPECIFIED USER AND PASSWORD");break;
		case -10:strcpy(strInfo,"\n NETWORK I/O OPERATION FAILED!");break;
		case -11:strcpy(strInfo,"\n NETWORK I/O TIMEOUT!");break;
		case -12:strcpy(strInfo,"\n SERVER TASK CANNOT ALLOCATE NEEDED MEMORY!");break;
		case -13  :strcpy(strInfo,"\n CLIENT TASK CANNOT ALLOCATE NEEDED MEMORY !");break;
		case -14  :strcpy(strInfo,"\n FUNCTION CALL IS OUT OF CONTEXT !");break;
		case -15  :strcpy(strInfo,"\n NO ACCESS TO OBJECT !");break;
		case -16  :strcpy(strInfo,"\n Exceeded max_layers in giomgr.defs. !");break;
		case -17  :strcpy(strInfo,"\n MISSING LAYER SPECIFICATION !");break;
		case -18  :strcpy(strInfo,"\n SPECIFIED LAYER IS LOCKED !");break;
		case -19  :strcpy(strInfo,"\n SPECIFIED LAYER ALREADY EXISTS !");break;
		case -20  :strcpy(strInfo,"\n SPECIFIED LAYER DOES NOT EXIST !");break;
		case -21  :strcpy(strInfo,"\n SPECIFIED LAYER IS USE BY ANOTHER USER !");break;
		case -22  :strcpy(strInfo,"\n SPECIFIED SHAPE (LAYER:FID) DOESN'T EXIST OR SPECIFIED ROW DOESN'T EXIST!");break;
		case -23  :strcpy(strInfo,"\n SPECIFIED SHAPE (LAYER:FID) EXISTS  OR  SPECIFIED ROW EXISTS!");break;
		case -24  :strcpy(strInfo,"\n Both layers must be the same for this !");break;
		case -25  :strcpy(strInfo,"\n NO PERMISSION TO PERFORM OPERATION !");break;
		case -26  :strcpy(strInfo,"\n COLUMN HAS NOT NULL CONSTRAINT. !");break;
		case -27  :strcpy(strInfo,"\n INVALID SHAPE, CANNOT BE VERIFIED !");break;
		case -28  :strcpy(strInfo,"\n MAP LAYER NUMBER OUT OF RANGE !");break;
		case -29  :strcpy(strInfo,"\n INVALID ENTITY TYPE !");break;
		case -30  :strcpy(strInfo,"\n INVALID SEARCH METHOD !");break;
		case -31  :strcpy(strInfo,"\n INVALID ENTITY TYPE MASK !");break;
		case -32  :strcpy(strInfo,"\n BIND/SET/GET MIS-MATCH !");break;
		case -33  :strcpy(strInfo,"\n INVALID GRID SIZE !");break;
		case -34  :strcpy(strInfo,"\n INVALID LOCK MODE !");break;
		case -35  :strcpy(strInfo,"\n ENTITY TYPE OF SHAPE IS NOT ALLOWED IN LAYER !");break;
		case -36  :strcpy(strInfo,"\n Exceeded max points specified. or Alternate name of above !");break;
		case -37  :strcpy(strInfo,"\n DBMS TABLE DOES NOT EXIST !");break;
		case -38  :strcpy(strInfo,"\n SPECIFIED ATTRIBUTE COLUMN DOESN'T EXIST !");break;
		case -39  :strcpy(strInfo,"\n Underlying license manager problem. !");break;
		case -40  :strcpy(strInfo,"\n No more SDE licenses available. !");break;
		case -41  :strcpy(strInfo,"\n VALUE EXCEEDS VALID RANGE !");break;
		case -42  :strcpy(strInfo,"\n USER SPECIFIED WHERE CLAUSE IS INVALID  OR  USER SPECIFIED SQL CLAUSE IS INVALID !");break;
		case -43  :strcpy(strInfo,"\n SPECIFIED LOG FILE DOES NOT EXIST !");break;
		case -44  :strcpy(strInfo,"\n UNABLE TO ACCESS SPECIFIED LOGFILE !");break;
		case -45  :strcpy(strInfo,"\n SPECIFIED LOGFILE IS NOT OPEN FOR I/O !");break;
		case -46  :strcpy(strInfo,"\n I/O ERROR USING LOGFILE !");break;
		case -47  :strcpy(strInfo,"\n NO SHAPES SELECTED OR USED IN OPERATION !");break;
		case -48  :strcpy(strInfo,"\n NO LOCKS DEFINED !");break;
		case -49  :strcpy(strInfo,"\n LOCK REQUEST CONFLICTS W/ ANOTHER ESTABLISHED LOCK !");break; 
		case -50  :strcpy(strInfo,"\n MAXIMUM LOCKS ALLOWED BY SYSTEM ARE IN USE !");break;
		case -51  :strcpy(strInfo,"\n DATABASE LEVEL I/O ERROR OCCURRED !");break;
		case -52  :strcpy(strInfo,"\n SHAPE/FID STREAM NOT FINISHED, CAN'T EXECUTE !");break;
		case -53  :strcpy(strInfo,"\n INVALID COLUMN DATA TYPE !");break;
		case -54  :strcpy(strInfo,"\n TOPOLOGICAL INTEGRITY ERROR !");break;
		case -55  :strcpy(strInfo,"\n ATTRIBUTE CONVERSION ERROR !");break;
		case -56  :strcpy(strInfo,"\n INVALID COLUMN DEFINITION !");break;
		case -57  :strcpy(strInfo,"\n INVALID SHAPE ARRAY BUFFER SIZE !");break;
		case -58  :strcpy(strInfo,"\n ENVELOPE IS NULL, HAS NEGATIVE VALUES OR MIN > MAX !");break;
		case -59  :strcpy(strInfo,"\n TEMP FILE I/O ERROR, CAN'T OPEN OR RAN OUT OF DISK !");break;
		case -60  :strcpy(strInfo,"\n SPATIAL INDEX GRID SIZE IS TOO SMALL !");break;
		case -61  :strcpy(strInfo,"\n SDE RUN-TIME LICENSE HAS EXPIRED, NO LOGINS ALLOWED !");break;
		case -62  :strcpy(strInfo,"\n DBMS TABLE EXISTS !");break;
		case -63  :strcpy(strInfo,"\n INDEX WITH THE SPECIFIED NAME ALREADY EXISTS !");break;
		case -64  :strcpy(strInfo,"\n INDEX WITH THE SPECIFIED NAME DOESN'T EXIST !");break;
		case -65  :strcpy(strInfo,"\n SPECIFIED POINTER VALUE IS NULL OR INVALID !");break;
		case -66  :strcpy(strInfo,"\n SPECIFIED PARAMETER VALUE IS INVALID !");break;
		case -67  :strcpy(strInfo,"\n SLIVER FACTOR CAUSED ALL RESULTS TO BE SLIVERS !");break;
		case -68  :strcpy(strInfo,"\n USER SPECIFIED TRANSACTION IN PROGRESS !");break;
		case -69  :strcpy(strInfo,"\n The iomgr has lost its connection to the underlying DBMS. !");break;
		case -70  :strcpy(strInfo,"\n AN ARC (startpt,midpt,endpt) ALREADY EXISTS !");break;
		case -71  :strcpy(strInfo,"\n SE_ANNO pointer not initialized. !");break;
		case -72  :strcpy(strInfo,"\n SPECIFIED POINT DOESN'T EXIST IN FEAT !");break;
		case -73  :strcpy(strInfo,"\n SPECIFIED POINTS MUST BE ADJACENT !");break;
		case -74  :strcpy(strInfo,"\n SPECIFIED MID POINT IS INVALID !");break;
		case -75  :strcpy(strInfo,"\n SPECIFIED END POINT IS INVALID !");break;
		case -76  :strcpy(strInfo,"\n SPECIFIED RADIUS IS INVALID !");break;
		case -77  :strcpy(strInfo,"\n MAP LAYER IS LOAD ONLY MODE, OPERATION NOT ALLOWED !");break;
		case -78  :strcpy(strInfo,"\n LAYERS TABLE DOES NOT EXIST. !");break;
		case -79  :strcpy(strInfo,"\n Error writing or creating an output text file. !");break;
		case -80  :strcpy(strInfo,"\n Maximum BLOB size exceeded. !");break;
		case -81  :strcpy(strInfo,"\n Resulting corridor exceeds valid coordinate range !");break;
		case -82  :strcpy(strInfo,"\n MODEL INTEGRITY ERROR !");break;
		case -83  :strcpy(strInfo,"\n Function or option is not really  written yet. !");break;
		case -84  :strcpy(strInfo,"\n This shape has a cad. !");break;
		case -85  :strcpy(strInfo,"\n Invalid internal SDE Transaction ID. !");break;
		case -86  :strcpy(strInfo,"\n MAP LAYER NAME MUST NOT BE EMPTY !");break;
		case -87  :strcpy(strInfo,"\n Invalid Layer Configuration Keyword used. !");break;
		case -88  :strcpy(strInfo,"\n Invalid Release/Version of SDE server. !");break;
		case -89  :strcpy(strInfo,"\n VERSION table exists. !");break;
		case -90  :strcpy(strInfo,"\n Column has not been bound !");break;
		case -91  :strcpy(strInfo,"\n Indicator variable contains an invalid value !");break;
		case -92   :strcpy(strInfo,"\n The connection handle is NULL, closed or the wrong object. !");break;
		case -93   :strcpy(strInfo,"\n The DBA password is not correct. !");break;
		case -94   :strcpy(strInfo,"\n Coord path not found in shape edit op. !");break;
		case -95   :strcpy(strInfo,"\n No SDEHOME environment var set, and we need one. !");break;
		case -96   :strcpy(strInfo,"\n User must be table owner. !");break;
		case -97   :strcpy(strInfo,"\n The process ID specified does not correspond on an SDE server.  !");break;
		case -98   :strcpy(strInfo,"\n DBMS didn't accept user/password. !");break;
		case -99   :strcpy(strInfo,"\n Password received was sent > MAXTIMEDIFF seconds before. !");break;
		case -100  :strcpy(strInfo,"\n Server machine was not found !");break;
		case -101  :strcpy(strInfo,"\n IO Mgr task not started on server !");break;
		case -102  :strcpy(strInfo,"\n No SDE entry in the /etc/services file !");break;
		case -103  :strcpy(strInfo,"\n Tried statisitics on non-numeric !");break;
		case -104  :strcpy(strInfo,"\n Distinct stats on invalid type !");break;
		case -105  :strcpy(strInfo,"\n Invalid use of grant/revoke function !");break;
		case -106  :strcpy(strInfo,"\n The supplied SDEHOME path is invalid or NULL.  !");break;
		case -107  :strcpy(strInfo,"\n Stream does not exist !");break;
		case -108  :strcpy(strInfo,"\n Max number of streams exceeded !");break;
		case -109  :strcpy(strInfo,"\n Exceeded system's max number of mutexs. !");break;
		case -110  :strcpy(strInfo,"\n This connection is locked to a  different thread. !");break;
		case -111  :strcpy(strInfo,"\n This connection is being used at the moment by another thread. !");break;
		case -112  :strcpy(strInfo,"\n The SQL statement was not a select !");break;
		case -113  :strcpy(strInfo,"\n Function called out of sequence !");break;
		case -114  :strcpy(strInfo,"\n Get request on wrong column type !");break;
		case -115  :strcpy(strInfo,"\n This ptable is locked to a different thread. !");break;
		case -116  :strcpy(strInfo,"\n This ptable is being used at the moment by another thread. !");break;
		case -117  :strcpy(strInfo,"\n This stable is locked to a different thread. !");break;
		case -118  :strcpy(strInfo,"\n This stable is being used at the moment by another thread. !");break;
		case -119  :strcpy(strInfo,"\n Unrecognized spatial filter type. !");break;
		case -120  :strcpy(strInfo,"\n The given shape has no CAD. !");break;
		case -121  :strcpy(strInfo,"\n No instance running on server. !");break;
		case -122  :strcpy(strInfo,"\n Instance is a version previous to 2.0. !");break;
		case -123  :strcpy(strInfo,"\n Systems units < 1 or > 2147483647. !");break;
		case -124  :strcpy(strInfo,"\n FEET, METERS, DECIMAL_DEGREES or OTHER. !");break;
		case -125  :strcpy(strInfo,"\n SE_CAD pointer not initialized. !");break;
		case -126  :strcpy(strInfo,"\n Version not found. !");break;
		case -127  :strcpy(strInfo,"\n Spatial filters invalid for search !");break;
		case -128  :strcpy(strInfo,"\n Invalid operation for the given stream !");break;
		case -129  :strcpy(strInfo,"\n Column contains NOT NULL values during SE_layer_create() !");break;
		case -130  :strcpy(strInfo,"\n No spatial masks available.  !");break;
		case -131  :strcpy(strInfo,"\n Iomgr program not found. !");break;
		case -132  :strcpy(strInfo,"\n Operation can not possibly be run on this system -- it needs a server. !");break;
		case -133  :strcpy(strInfo,"\n Only one spatial column allowed !");break;
		case -134  :strcpy(strInfo,"\n The given shape object handle is invalid !");break;
		case -135  :strcpy(strInfo,"\n The specified shape part number does not exist !");break;
		case -136  :strcpy(strInfo,"\n The given shapes are of incompatible types !");break;
		case -137  :strcpy(strInfo,"\n The specified part offset is invalid !");break;
		case -138  :strcpy(strInfo,"\n The given coordinate references are incompatible !");break;
		case -139  :strcpy(strInfo,"\n The specified coordinate exceeds the valid coordinate range !");break;
		case -140  :strcpy(strInfo,"\n Max. Layers exceeded in cache !");break;
		case -141  :strcpy(strInfo,"\n The given coordinate reference  object handle is invalid !");break;
		case -142  :strcpy(strInfo,"\n The coordinate system identifier is invalid !");break;
		case -143  :strcpy(strInfo,"\n The coordinate system description is invalid !");break;
		case -144  :strcpy(strInfo,"\n SE_ROW_ID is not a valid ArcSDE row_id column for this table or layer !");break;
		case -145  :strcpy(strInfo,"\n Error projecting shape points !");break;
		case -146  :strcpy(strInfo,"\n Max array bytes exceeded !");break;
		case -147  :strcpy(strInfo,"\n 2 donuts or 2 outer shells overlap !");break;
		case -148  :strcpy(strInfo,"\n Numofpts is less than required for feature !");break;
		case -149 :strcpy(strInfo,"\n part separator in the wrong position !");break;
		case -150 :strcpy(strInfo,"\n polygon does not close properly !");break;
		case -151 :strcpy(strInfo,"\n A polygon outer shell does not completely enclose all donuts for the part !");break;
		case -152 :strcpy(strInfo,"\n Polygon shell has no area !");break;
		case -153 :strcpy(strInfo,"\n Polygon shell contains a vertical line !");break;
		case -154 :strcpy(strInfo,"\n Multipart area has overlapping parts !");break;
		case -155 :strcpy(strInfo,"\n Linestring or poly boundary is self-intersecting !");break;
		case -156 :strcpy(strInfo,"\n Export file is invalid !");break;
		case -157 :strcpy(strInfo,"\n Attempted to modify or free a read-only shape from an stable. !");break;
		case -158 :strcpy(strInfo,"\n Invalid data source !");break;
		case -159 :strcpy(strInfo,"\n Stream Spec parameter exceeds giomgr default !");break;
		case -160 :strcpy(strInfo,"\n Tried to remove cad or anno !");break;
		case -161 :strcpy(strInfo,"\n Spat col name same as table name !");break;
		case -162 :strcpy(strInfo,"\n Invalid database name !");break;
		case -163 :strcpy(strInfo,"\n Spatial SQL extension not present in underlying DBMS !");break;
		case -164 :strcpy(strInfo,"\n Obsolete SDE 3.0.2 error !");break;
		case -165 :strcpy(strInfo,"\n Obsolete SDE 3.0.2 error !");break;
		case -166 :strcpy(strInfo,"\n SDE 3.0.2 error !");break;
		case -167 :strcpy(strInfo,"\n Obsolete SDE 3.0.2 error!");break;
		case -168 :strcpy(strInfo,"\n Has R/O access to SE_ROW_ID !");break;
		case -169 :strcpy(strInfo,"\n The current table doesn't have a SDE-maintained rowid column. !");break;
		case -170 :strcpy(strInfo,"\n Column is not user-modifiable !");break;
		case -171 :strcpy(strInfo,"\n Illegal or blank version name !");break;
		case -172 :strcpy(strInfo,"\n A specified state is not in the VERSION_STATES table. !");break;
		case -173 :strcpy(strInfo,"\n STATEINFO object not initialized. !");break;
		case -174 :strcpy(strInfo,"\n Attempted to change version state, but already changed. !");break;
		case -175 :strcpy(strInfo,"\n Tried to open a state which has children. !");break;
		case -176 :strcpy(strInfo,"\n To create a state, the parent state must be closed. !");break;
		case -177 :strcpy(strInfo,"\n Version already exists. !");break;
		case -178 :strcpy(strInfo,"\n Table must be multiversion for this operation. !");break;
		case -179 :strcpy(strInfo,"\n Can't delete state being used by a version. !");break;
		case -180 :strcpy(strInfo,"\n VERSIONINFO object not  initialized. !");break;
		case -181 :strcpy(strInfo,"\n State ID out of range or not found. !");break;
		case -182 :strcpy(strInfo,"\n Environment var SDETRACELOC not set to a value   !");break;
		case -183 :strcpy(strInfo,"\n Error loading the SSA  !");break;
		case -184 :strcpy(strInfo,"\n This operation has more states than can fit in SQL. !");break;
		case -185 :strcpy(strInfo,"\n Function takes 2 <> states, but same one was given twice. !");break;
		case -186 :strcpy(strInfo,"\n Table has no usable row ID column. !");break;
		case -187 :strcpy(strInfo,"\n Call needs state to be set. !");break;
		case -188 :strcpy(strInfo,"\n Error executing SSA function !");break;
		case -189 :strcpy(strInfo,"\n REGINFO object !initialized. !");break;
		case -190 :strcpy(strInfo,"\n Attempting to trim between states on diff. branches !");break;
		case -191 :strcpy(strInfo,"\n State is being modified. !");break;
		case -192 :strcpy(strInfo,"\n Try to lock tree, and some state in tree already locked. !");break;
		case -193 :strcpy(strInfo,"\n Raster column has non-NULL values or used as row_id column !");break;
		case -194 :strcpy(strInfo,"\n Raster column already exists !");break;
		case -195 :strcpy(strInfo,"\n Unique indexes are not allowed on multiversion tables. !");break;
		case -196 :strcpy(strInfo,"\n Invalid layer storage type. !");break;
		case -197 :strcpy(strInfo,"\n No SQL type provided when  converting NIL shape to text !");break;
		case -198 :strcpy(strInfo,"\n Invalid byte order for  Well-Known Binary shape !");break;
		case -199 :strcpy(strInfo,"\n Shape type in the given shape is not a valid geometry type !");break;
		case -200 :strcpy(strInfo,"\n Number of measures in shape must be zero or equal to number  of points !");break;
		case -201 :strcpy(strInfo,"\n Number of parts in shape is incorrect for its geometry type !");break;
		case -202 :strcpy(strInfo,"\n Memory allocated for ESRI binary shape is too small !");break;
		case -203 :strcpy(strInfo,"\n Shape text exceeds the  supplied maximum string length !");break;
		case -204 :strcpy(strInfo,"\n Found syntax error in the  supplied shape text !");break;
		case -205 :strcpy(strInfo,"\n Number of parts in shape is more than expected for the given shape text !");break;
		case -206 :strcpy(strInfo,"\n Shape's SQL type is not as  expected when constructing  shape from text !");break;
		case -207 :strcpy(strInfo,"\n Found parentheses mismatch when parsing shape text !");break;
		case -208 :strcpy(strInfo,"\n NIL shape is not allowed for Well-Known Binary  represenation !");break;
		case -209 :strcpy(strInfo,"\n Tried to start already running SDE instance. !");break;
		case -210 :strcpy(strInfo,"\n The operation requested is  unsupported. !");break;
		case -211 :strcpy(strInfo,"\n Invalid External layer option. !");break; 
		case -212 :strcpy(strInfo,"\n Normalized layer dimension  table value not found. !");break;
		case -213 :strcpy(strInfo,"\n Invalid query type. !");break;
		case -214 :strcpy(strInfo,"\n No trace functions in this library !");break;
		case -215 :strcpy(strInfo,"\n Tried to enable tracing that was already on !");break; 
		case -216 :strcpy(strInfo,"\n Tried to disable tracing that was already off !");break; 
		case -217 :strcpy(strInfo,"\n SCL Compiler doesn't like the SCL stmt !");break;
		case -218 :strcpy(strInfo,"\n Table already registered. !");break;
		case -219 :strcpy(strInfo,"\n Registration ID out of range !");break;
		case -220 :strcpy(strInfo,"\n Table not registered. !");break;
		case -221 :strcpy(strInfo,"\n Exceeded max_registrations. !");break;
		case -222 :strcpy(strInfo,"\n This object can not be deleted,other objects depend on it. !");break;
		case -223 :strcpy(strInfo,"\n Row locking enabled      !");break;
		case -224 :strcpy(strInfo,"\n Row locking not enabled  !");break;
		case -225 :strcpy(strInfo,"\n Specified raster column is used by another user !");break;
		case -226 :strcpy(strInfo,"\n The specified raster column  does not exist !");break;
		case -227 :strcpy(strInfo,"\n Raster column number  out of range !");break;
		case -228 :strcpy(strInfo,"\n Maximum raster column  number exceeded !");break;
		case -229 :strcpy(strInfo,"\n Raster number out of range !");break;
		case -230 :strcpy(strInfo,"\n cannot determine  request status !");break;
		case -231 :strcpy(strInfo,"\n cannot open request results !");break; 
		case -232 :strcpy(strInfo,"\n Raster band already exists !");break;
		case -233 :strcpy(strInfo,"\n The specified raster band does not exist !");break;
		case -234 :strcpy(strInfo,"\n Raster already exists !");break;
		case -235 :strcpy(strInfo,"\n The specified raster  does not exist !");break;
		case -236 :strcpy(strInfo,"\n Maximum raster band number exceeded !");break; 
		case -237 :strcpy(strInfo,"\n Maximum raster number  exceeded !");break; 
		case -238 :strcpy(strInfo,"\n DBMS VIEW EXISTS !");break;
		case -239 :strcpy(strInfo,"\n DBMS VIEW NOT EXISTS !");break;
		case -240 :strcpy(strInfo,"\n Lock record exist !");break;
		case -241 :strcpy(strInfo,"\n Rowlocking mask conflict !");break;
		case -242 :strcpy(strInfo,"\n Raster band specified  not in a raster !");break;
		case -243 :strcpy(strInfo,"\n RASBANDINFO object not initialized !");break;
		case -244 :strcpy(strInfo,"\n RASCOLINFO object not initialized !");break;
		case -245 :strcpy(strInfo,"\n RASTERINFO object  not initialized !");break;
		case -246 :strcpy(strInfo,"\n Raster band number out of range !");break;
		case -247 :strcpy(strInfo,"\n Only one raster column allowed !");break;
		case -248 :strcpy(strInfo,"\n Table is being locked already !");break;
		case -249 :strcpy(strInfo,"\n SE_LOGINFO pointer not initialized. !");break;
		case -250 :strcpy(strInfo,"\n Operation generated a SQL statement too big to process.!");break;
		case -251 :strcpy(strInfo,"\n Not supported on a View.!");break;
		case -252 :strcpy(strInfo,"\n Specified log file exists already. !");break;
		case -253 :strcpy(strInfo,"\n Specified log file is open. !");break;
		case -254 :strcpy(strInfo,"\n Spatial reference entry exists. !");break;
		case -255 :strcpy(strInfo,"\n Spatial reference entry does not exist. !");break;
		case -256 :strcpy(strInfo,"\n Spatial reference entry is in use by one or more layers. !");break;
		case -257 :strcpy(strInfo,"\n The SE_SPATIALREFINFO object is not initialized. !");break;
		case -258 :strcpy(strInfo,"\n Raster band sequence number already exits. !");break;
		case -259 :strcpy(strInfo,"\n SE_QUERYINFO pointer not initialized. !");break;
		case -260 :strcpy(strInfo,"\n SE_QUERYINFO pointer is not prepared for query. !");break;
		case -261 :strcpy(strInfo,"\n RASTILEINFO object not initialized !");break;
		case -262 :strcpy(strInfo,"\n SE_RASCONSTRAINT object not initialized !");break;
		case -263 :strcpy(strInfo,"\n invalid record id number !");break;
		case -264 :strcpy(strInfo,"\n SE_METADATAINFO pointer not initialized !");break;
		case -265 :strcpy(strInfo,"\n unsupported object type !");break;
		case -266 :strcpy(strInfo,"\n SDEMETADATA table does not exist !");break;
		case -267 :strcpy(strInfo,"\n Metadata record does not exist. !");break;
		case -268 :strcpy(strInfo,"\n Geometry entry does not exist !");break;
		case -269 :strcpy(strInfo,"\n File path too long or invalid !");break;
		case -270 :strcpy(strInfo,"\n Locator object not initialized !");break;
		case -271 :strcpy(strInfo,"\n Locator cannot be validated !");break;
		case -272 :strcpy(strInfo,"\n Table has no associated locator !");break;
		case -273 :strcpy(strInfo,"\n Locator cateogry is not specified !");break;
		case -274 :strcpy(strInfo,"\n Invalid locator name !");break;
		case -275 :strcpy(strInfo,"\n Locator does not exist !");break;
		case -276 :strcpy(strInfo,"\n A locator with that name exists !");break;
		case -277 :strcpy(strInfo,"\n Unsupported Locator type !");break;
		case -278 :strcpy(strInfo,"\n No coordref defined !");break;
		case -279 :strcpy(strInfo,"\n Can't trim past a reconciled state. !");break;
		case -280 :strcpy(strInfo,"\n Fileinfo object does not exist. !");break;
		case -281 :strcpy(strInfo,"\n Fileinfo object already exists. !");break;
		case -282 :strcpy(strInfo,"\n Fileinfo object not initialized. !");break;
		case -283 :strcpy(strInfo,"\n Unsupported Fileinfo object type. !");break;
		case -284 :strcpy(strInfo,"\n No statistics available for this raster band. !");break;
		case -285 :strcpy(strInfo,"\n Can't delete a version with children. !");break;
		case -286 :strcpy(strInfo,"\n SQL type does not support ANNO or CAD at current release !");break;
		case -287 :strcpy(strInfo,"\n The DBTUNE file is missing  or unreadable. !");break;
		case -288 :strcpy(strInfo,"\n Logfile system tables do not exist. !");break;
		case -289 :strcpy(strInfo,"\n This app can't perform this operation on this object. !");break;
		case -290 :strcpy(strInfo,"\n The given geographic transformation object handle is invalid !");break;
		case -291 :strcpy(strInfo,"\n Column already exists !");break;
		case -292 :strcpy(strInfo,"\n SQL keyword violation. !");break;
		case -293 :strcpy(strInfo,"\n The supplied objectlock handle is bad. !");break;
		case -294 :strcpy(strInfo,"\n The raster buffer size is too small. !");break;
		case -295 :strcpy(strInfo,"\n Invalid raster data !");break;
		case -296 :strcpy(strInfo,"\n This operation is not allowed !");break;
		case -297 :strcpy(strInfo,"\n SE_RASTERATTR object not  initialized !");break;
		case -298 :strcpy(strInfo,"\n Version ID out of range. !");break;
		case -299 :strcpy(strInfo,"\n Attempting to make an MV table load-only !");break;
		case -300 :strcpy(strInfo,"\n The user-supplied table/column is invalid. !");break;
		case -301 :strcpy(strInfo,"\n The next row was not the row expected. !");break;
		case -302 :strcpy(strInfo,"\n The ArcSDE instance is  read-only !");break;
		case -303 :strcpy(strInfo,"\n Image mosaicking is not allowed !");break;
		case -304 :strcpy(strInfo,"\n Invalid raster bitmap  object !");break;
		case -305 :strcpy(strInfo,"\n The specified band sequence  number does not exist. !");break;
		case -306 :strcpy(strInfo,"\n Invalid SQLTYPE feature type (i.e. Rect, Arc, Circle) !");break; 
		case -307 :strcpy(strInfo,"\n DBMS Objects (Spatial, ADT's not supported !");break; 
		case -308 :strcpy(strInfo,"\n No columns found for binary conversion (LOB/LONGRAW) !");break;
		case -309 :strcpy(strInfo,"\n The raster band has no colormap. !");break;
		case -310 :strcpy(strInfo,"\n Invalid raster band bin  function. !");break;
		case -311 :strcpy(strInfo,"\n Invalid raster band statistics. !");break;
		case -312 :strcpy(strInfo,"\n Invalid raster band  colormap !");break;
		case -313 :strcpy(strInfo,"\n Invalid raster layer configuration keyword !");break;
		case -314 :strcpy(strInfo,"\n This sort of iomgr can't run on this sort of instance. !");break;
		case -315 :strcpy(strInfo,"\n Export file's volume info is invalid !");break;
		case -316 :strcpy(strInfo,"\n Invalid compression type !");break;
		case -317 :strcpy(strInfo,"\n Invalid index parameter !");break;
		case -318 :strcpy(strInfo,"\n Invalid index type !");break;
		case -319 :strcpy(strInfo,"\n Try to set conflicting value  in object !");break;
		case -320 :strcpy(strInfo,"\n Abstract Data types not supported !");break; 
		case -321 :strcpy(strInfo,"\n No spatial index !");break;
		case -322 :strcpy(strInfo,"\n Name not valid for DBMS !");break;
		case -323 :strcpy(strInfo,"\n ROWID for Oracle Spatial table already exists. !");break;
		case -324 :strcpy(strInfo,"\n gsrvr dll for direct could not be loaded. !");break;
		case -325 :strcpy(strInfo,"\n The table can not be registered. !");break;
		case -326 :strcpy(strInfo,"\n Operation not supported on multiversion table. !");break;
		case -327 :strcpy(strInfo,"\n No ArcSDE server license found !");break;
		case -328 :strcpy(strInfo,"\n Exportfile is not supported !");break;
		case -329 :strcpy(strInfo,"\n Specified table is in use  !");break;
		case -330 :strcpy(strInfo,"\n SE_XMLINDEXINFO object not  initialized !");break;
		case -331 :strcpy(strInfo,"\n Specified XML column exists !");break;
		case -332 :strcpy(strInfo,"\n Tag's data type or alias  doesn't match.!");break;
		case -333 :strcpy(strInfo,"\n XML index already exists. !");break;
		case -334 :strcpy(strInfo,"\n Specified XML index  does not exist. !");break;
		case -335 :strcpy(strInfo,"\n Tried to alter non-modifiable field. !");break;
		case -336 :strcpy(strInfo,"\n Cannot alter/delete template that's referenced by column. !");break;
		case -337 :strcpy(strInfo,"\n Must be owner to del/alter. !");break;
		case -338 :strcpy(strInfo,"\n SE_XMLDOC object not initialized !");break;
		case -339 :strcpy(strInfo,"\n Named tag not found. !");break; 
		case -340 :strcpy(strInfo,"\n Illegal XML document syntax !");break;
		case -341 :strcpy(strInfo,"\n the ZM column does not exists in the layers table !");break;
		case -342 :strcpy(strInfo,"\n Invalid date value !");break;
		case -343 :strcpy(strInfo,"\n Given XML column not found !");break;
		case -344 :strcpy(strInfo,"\n Both XML columns must be the  same for this operation. !");break;
		case -345 :strcpy(strInfo,"\n Incorrect xpath syntax. !");break;
		case -346 :strcpy(strInfo,"\n Input xpath tag not supported  in current release. !");break;
		case -347 :strcpy(strInfo,"\n SE_COLUMNINFO object not initialized !");break;
		case -348 :strcpy(strInfo,"\n SE_XMLTAGINFO object not initialized !");break;
		case -349 :strcpy(strInfo,"\n Wrong type of XML index !");break;
		case -350 :strcpy(strInfo,"\n The requested operation is unsupported by instance db.!");break;
		case -351 :strcpy(strInfo,"\n Incorrect xpath location  alias syntax. !");break;
		case -352 :strcpy(strInfo,"\n Incorrect xpath predicate  token.!");break;
		case -353 :strcpy(strInfo,"\n Incorrect xpath  predicate operator. !");break;
		case -354 :strcpy(strInfo,"\n Incorrect xpath predicate contain function syntax.!");break;
		case -355 :strcpy(strInfo,"\n Empty xpath element is not allowed.!");break;
		case -356 :strcpy(strInfo,"\n Incorrect xpath token. !");break;
		case -357 :strcpy(strInfo,"\n Environment var SDETMP not set to a value   !");break;
		case -358 :strcpy(strInfo,"\n SERVERINFO object not initialized. !");break;
		case -359 :strcpy(strInfo,"\n Ambiguity between a user name and group name when working with permissions !");break;
		case -360 :strcpy(strInfo,"\n Geocoding rule file already exists and force_overwrite is not specified !");break;
		case -361 :strcpy(strInfo,"\n Geocoding rule file is not found !");break;
		case -362 :strcpy(strInfo,"\n Geocoding rule is not found in geocoding rule table !");break;
		case -363 :strcpy(strInfo,"\n Geocoding rule already exist in the geocoding rule table and force_overwrite is not specified !");break;
		case -364 :strcpy(strInfo,"\n SE_XMLCOLUMNINFO object not initialized !");break;
		case -365 :strcpy(strInfo,"\n Invalid system group name !");break;
		case -366 :strcpy(strInfo,"\n Column contains NOT NULL values during SE_xmlcolumn_create() !");break;
		case -367 :strcpy(strInfo,"\n Locator property is too long or invalid !");break;
		case -368 :strcpy(strInfo,"\n Locator property has multiple values !");break;
		case -369 :strcpy(strInfo,"\n Search window completely outside  oracle spatial  geodetic extent !");break;
		case -370 :strcpy(strInfo,"\n Cannot use xpath searches on  non-indexed columns. !");break;
		case -371 :strcpy(strInfo,"\n The current table doesn't haveSDE-maintained UUID column. !");break;
		case -372 :strcpy(strInfo,"\n Invalid UUID column. !");break;
		case -373 :strcpy(strInfo,"\n Underlying RDBMS doesn't  support 64-bit layer. !");break;
		case -374 :strcpy(strInfo,"\n Invalid (rebuild) index mask!");break;
		case -375 :strcpy(strInfo,"\n Coordref/Layer precision not OK for requested operation. !");break;
		case -376 :strcpy(strInfo,"\n The SE_TRANSFORMINFO object  is not initialized !");break;      
		case -377 :strcpy(strInfo,"\n Invalid raster band  transform list !");break;
		case -378 :strcpy(strInfo,"\n The raster band has no transform list !");break;
		case -379 :strcpy(strInfo,"\n SE_QUERYCOLUMN object not initialized !");break;
		case -380 :strcpy(strInfo,"\n Multiple shapes set to SE_SHAPESOURCE shape !");break;
		case -381 :strcpy(strInfo,"\n SE_QUERYSELECT object not initialized !");break;
		case -382 :strcpy(strInfo,"\n SE_SHAPESOURCE object not initialized !");break;
		case -383 :strcpy(strInfo,"\n SE_SPATIALCONSTRAINT object not init'ized !");break;
		case -384 :strcpy(strInfo,"\n QUERYFROM join constraint missing !");break;
		case -385 :strcpy(strInfo,"\n QUERYFROM join conflict !");break;
		case -386 :strcpy(strInfo,"\n QUERYFROM join table missing !");break;
		case -387 :strcpy(strInfo,"\n SE_QUERYTABLE object not initialized !");break;
		case -388 :strcpy(strInfo,"\n SE_QUERYFROM object not initialized !");break;
		case -389 :strcpy(strInfo,"\n SE_XMLCONSTRAINT object not initialized !");break;
		case -390 :strcpy(strInfo,"\n SE_ATTRIBUTECONSTRAINT object not init'ized !");break;
		case -391 :strcpy(strInfo,"\n SE_QUERYWHERE object not initialized !");break;
		case -392 :strcpy(strInfo,"\n Error opening codepage converter !");break;
		case -393 :strcpy(strInfo,"\n The destination string buffer is too small !");break;
		case -394 :strcpy(strInfo,"\n Codepage conversion error !");break;
		case -395 :strcpy(strInfo,"\n Operation invalid on a closed state. !");break;
		case -396 :strcpy(strInfo,"\n The requested operation is invalid on the base state. !");break;
		case -397 :strcpy(strInfo,"\n The direct connect is not registered with an ArcSde server for license. !");break;
		case -398 :strcpy(strInfo,"\n The given range of ids is invalid !");break;
		case -399 :strcpy(strInfo,"\n This state object doesn't contain lineage info. !");break;
		case -400 :strcpy(strInfo,"\n OGCWKB type does not support ANNO, CAD, 3D, Measures or Curves. !");break;
		case -401 :strcpy(strInfo,"\n Not allowed to set image origin. !");break;
		case -402 :strcpy(strInfo,"\n Not a valid image origin. !");break;
		case -403 :strcpy(strInfo,"\n The raster band has no  GDB metadata. !");break;
		case -404 :strcpy(strInfo,"\n Invalid raster band GDB  metadata. !");break;
		case -405 :strcpy(strInfo,"\n Not allowed to create index on complex column. !");break;
		case -406 :strcpy(strInfo,"\n Can't start compress if compress is already running !");break;
		case -407 :strcpy(strInfo,"\n Schema is out of date. !");break;

		case -1000:strcpy(strInfo,"\n BASE NUMBER FOR WARNING CODES!");break;
		case -1001:strcpy(strInfo,"\n FUNCTION CHANGED ENTITY TYPE OF FEAT!");break;
		case -1002:strcpy(strInfo,"\n No rows were deleted!");break;
		case -1003:strcpy(strInfo,"\n Too many distinct values in stats!");break;
		case -1004:strcpy(strInfo,"\n Request column value is NULL!");break;
		case -1005:strcpy(strInfo,"\n No rows were updated !");break;
		case -1006:strcpy(strInfo,"\n No codepage conversion  !");break;
		case -1007:strcpy(strInfo,"\n Cannot find codepage directory!");break;
		case -1008:strcpy(strInfo,"\n DBMS does NOT support this function !");break;
		case -1009:strcpy(strInfo,"\n Invalid DBMS function id!");break;
		case -1010:strcpy(strInfo,"\n Update layer extent failed!");break;
		case -1011:strcpy(strInfo,"\n No localized error messages!");break;
		case -1012:strcpy(strInfo,"\n Spatial index not created,server inability to support SPIDX_PARAM specified!");break;
		case -1013:strcpy(strInfo,"\n SDE does NOT support this function  on this DBMS! ");break;
		case -1014:strcpy(strInfo,"\n SQL statement that is being re-executed was not found in the cache!");break;

		default: strcpy(strInfo,"\n Unkown!");break;
	}

	return strInfo;
}


//��¼������ز���
//SELECT A,B,C FROM D,E WHERE ID > 1
//INSERT INTO TABLE(A,B,C) VALUES(D,E,F)
//UPDATE TABLE SET A = D,B = E ,C = F WHERE ID > 1 AND ID <10 
//DELETE FROM TABLE WHERE ID > 1




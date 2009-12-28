#include "AccessWebServiceInfo.h"
#include "..\WSAccessor\WsAccessor_export.h"
#include "..\XMLData\DistBaseInterface.h"
#include "..\SdeData\DistExchangeInterface.h"


extern IDistConnection*    g_pConnection;         //SDE���ݿ����Ӷ���ָ��
extern IDistExchangeData*  g_pExchangeData;       //SDE<->CAD���ݽ�������ָ��

// �Ƚϰ���ͨ���*���ַ�����ƥ�䷵��TRUE�����򷵻�FALSE
static BOOL CmpTwoString(CString str1, CString str2)
{
	int pos1 = str1.Find('*');
	int pos2 = str2.Find('*');
	if (pos1 == -1 && pos2 == -1)
	{
		if (str1.Collate(str2) == 0)
			return TRUE;
		else
			return FALSE;
	}
	else if (pos1 != -1 && pos2 == -1)
	{
		CString sLeft = str1.Left(pos1);
		CString sRight = str1.Right(str1.GetLength() - pos1 - 1);
		if (str2.Left(sLeft.GetLength()).Collate(sLeft) != 0)
			return FALSE;
		else if (str2.Right(sRight.GetLength()).Collate(sRight) != 0)
			return FALSE;
		else
			return TRUE;
	}
	else if (pos1 == -1 && pos2 != -1)
	{
		CString sLeft = str2.Left(pos2);
		CString sRight = str2.Right(str2.GetLength() - pos2 - 1);
		if (str1.Left(sLeft.GetLength()).Collate(sLeft) != 0)
			return FALSE;
		else if (str1.Right(sRight.GetLength()).Collate(sRight) != 0)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		CString sLeft1 = str1.Left(pos1);
		CString sRight1 = str1.Right(str1.GetLength() - pos1 - 1);
		CString sLeft2 = str2.Left(pos2);
		CString sRight2 = str2.Right(str2.GetLength() - pos2 - 1);
		if ((sLeft1.Find(sLeft2) != -1 || sLeft2.Find(sLeft1) != -1) &&
			(sRight1.Find(sRight2) != -1 || sRight2.Find(sRight1) != -1))
			return TRUE;
		else
			return FALSE;
	}
}

//===============================================================================================//

POWERCONFIGARRAY CPowerConfig::m_PowerArray;
CPowerConfig::CPowerConfig(void)
{
}

CPowerConfig::~CPowerConfig(void)
{
}

//BOOL CPowerConfig::GetCurUserRight(CString strURL,CString strRoleId, CString strXMBH)
//{
//	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();
//	if (pWsAccessor == NULL) return FALSE;
//
//	TCHAR lpMsg[512]={0};
//	HRESULT hr = pWsAccessor->Connect(strURL.GetBuffer());
//	if( hr != S_OK )// ������ӳ���
//	{
//		//pWsAccessor->Answer(lpMsg);
//		//MessageBox(0, lpMsg ,"WebService",  0);
//		return FALSE;
//	}
//
//	_variant_t varResult;
//	_variant_t varParams[10];
//	varParams[1] = strXMBH.GetBuffer();
//	varParams[0] = strRoleId.GetBuffer();
//
//	// ���÷���
//	hr = pWsAccessor->CallMethod("GetCADPictureRightSetByProjectIDRoleID",varParams, &varResult);
//	if (hr != S_OK )// ���������ó�����Ϣ
//	{
//		//pWsAccessor->Answer(lpMsg);
//		return FALSE;
//	}
//
//	IDistConnectionXML* lpConn = CreateConnectObjcet();
//	if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
//	{
//		
//		//AfxMessageBox("Connect to file failed!\n");
//		return FALSE;
//	}
//
//	IDistCommandXML* lpCmd = NULL;
//	lpConn->QueryCommand(&lpCmd);
//
//	char strXmlPath[] = "/NewDataSet/Table";
//	IDistRecordSetXML* lpRst = NULL;
//	lpCmd->SelectData(strXmlPath, &lpRst);
//
//	if (NULL == lpRst)
//	{
//		lpCmd->Release();
//		lpConn->Release();
//		return FALSE;
//	}
//
//	m_PowerArray.RemoveAll();
//
//	BOOL bOK = lpRst->MoveFirst();
//	while (bOK)
//	{
//		POWERCONFIG temp;
//		temp.strTreeID= (char*)((*lpRst)[0]);//"TreeID"
//		temp.strSDEPower = (char*)((*lpRst)[1]);//"SDEPower"
//
//		m_PowerArray.Add(temp);
//		bOK = lpRst->MoveNext();
//	}
//
//	lpRst->Release();
//	lpCmd->Release();
//	lpConn->Release();
//
//	return TRUE;
//}

void CPowerConfig::Release()
{
	m_PowerArray.RemoveAll();
}

//===============================================================================================//

POLTINDEXARRAY CPoltIndex::m_PoltIndArray;

CPoltIndex::CPoltIndex(void)
{
}

CPoltIndex::~CPoltIndex(void)
{
}

/*
BOOL CPoltIndex::GetPoltIndInf(const CString strURL)
{
	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();

	if (pWsAccessor == NULL) return FALSE;

	HRESULT hr = pWsAccessor->Connect(strURL);
	TCHAR lpMsg[512]={0};

	// ������ӳ���
	if( hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg);
		//MessageBox(0, lpMsg ,"GetPoltIndInf",  0);
		return FALSE;
	}


	CString sSql;
	sSql = "select * from TB_POLTINDEX";

	// ע�⣬�������鳤��һ��Ҫ������һЩ
	_variant_t varParams[5];
	varParams[0] = sSql.GetBuffer();
	_variant_t varResult;

	// ���÷���
	hr = pWsAccessor->CallMethod("ExecSqlQuery",varParams, &varResult);

	// ���������ó�����Ϣ
	if (hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg);

		//MessageBox(0, lpMsg ,"GetPoltIndInf",  0);

		return FALSE;
	}

	IDistConnectionXML* lpConn = CreateConnectObjcet();
	if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
	{
		//AfxMessageBox("Connect to file failed!\n");
		return FALSE;
	}

	IDistCommandXML* lpCmd = NULL;
	lpConn->QueryCommand(&lpCmd);

	char strXmlPath[] = "/NewDataSet/Table";
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
#ifdef _DEBUG
		AfxMessageBox("TB_POLTINDEX����δ�ҵ�����");
#endif
		lpCmd->Release();
		lpConn->Release();
		return FALSE;
	}

	BOOL bOK = lpRst->MoveFirst();
	while (bOK)
	{
		POLTINDEXCONFIG temp;
		CString tmp = (char*)((*lpRst)["ID"]);
		temp.Id = tmp;
		tmp = (char*)((*lpRst)["SDELAYER"]);
		temp.SDELayer = tmp;
		tmp = (char*)((*lpRst)["ALIASNAME"]);
		if (tmp.IsEmpty())
		{
			tmp = temp.SDELayer;
		}
		temp.AliasName = tmp;
		tmp = (char*)((*lpRst)["FIELDNAME"]);
		temp.FieldName = tmp;
		tmp = (char*)((*lpRst)["ORDERNUM"]);
		if (tmp.IsEmpty())
		{
			tmp = temp.Id;
		}
		temp.OrderNum = tmp;

		tmp = (char*)((*lpRst)["TYPE"]);
		temp.Type = tmp;

		m_PoltIndArray.Add(temp);

		bOK = lpRst->MoveNext();
	}

	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();

	return TRUE;
}
*/

void CPoltIndex::Realse()
{
	m_PoltIndArray.RemoveAll();
}

BOOL CPoltIndex::GetLayerPoltIndexArray(CStringArray& strSdeLayerNameArray, CStringArray& strFieldNameArray)
{
	CStringArray strOrderArray;
	for (int i = 0; i < m_PoltIndArray.GetSize(); i++)
	{
		if (m_PoltIndArray.GetAt(i).Type.CompareNoCase("1") == 0)
			continue;

		int nOrder1 = atoi(m_PoltIndArray.GetAt(i).OrderNum);
		int pos = strOrderArray.GetSize();
		for (; pos > 0; pos--)
		{
			int nOrder2 = atoi(strOrderArray.GetAt(pos - 1));
			if (nOrder1 >= nOrder2)
			{
				break;
			}
		}

		strSdeLayerNameArray.InsertAt(pos, m_PoltIndArray.GetAt(i).SDELayer);
		strFieldNameArray.InsertAt(pos, m_PoltIndArray.GetAt(i).FieldName);
		strOrderArray.InsertAt(pos, m_PoltIndArray.GetAt(i).OrderNum);
	}

	return(strSdeLayerNameArray.GetSize() > 0);
}

BOOL CPoltIndex::GetXMBHInfo(CString& strSdeLayerName, CString& strFieldName)
{
	for (int i = 0; i < m_PoltIndArray.GetSize(); i++)
	{
		if (m_PoltIndArray.GetAt(i).Type.CompareNoCase("1") == 0)
		{
			strSdeLayerName = m_PoltIndArray.GetAt(i).SDELayer;
			strFieldName = m_PoltIndArray.GetAt(i).FieldName;
			return TRUE;
		}
	}

	return FALSE;
}


//===============================================================================================//
YDTYPEARRAY CYdxz3Part::m_YdTypeArray;
CYdxz3Part::CYdxz3Part(void)
{
}

CYdxz3Part::~CYdxz3Part(void)
{
}

/*
BOOL CYdxz3Part::GetFromDataBase(const CString strURL)
{
	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();

	if (pWsAccessor == NULL)		
	{
		return FALSE;
	}

	HRESULT hr = pWsAccessor->Connect(strURL);

	TCHAR lpMsg[512];

	// ������ӳ���
	if( hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg);

		//MessageBox(0, lpMsg ,"GetFromDataBase",  0);

		return FALSE;
	}


	CString sSql = "select * from TBUseLand_type";

	// ע�⣬�������鳤��һ��Ҫ������һЩ
	_variant_t varParams[5];
	varParams[0] = sSql.GetBuffer();

	_variant_t varResult;

	// ���÷���
	hr = pWsAccessor->CallMethod("ExecSqlQuery",varParams, &varResult);

	// ���������ó�����Ϣ
	if (hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg);

		//MessageBox(0, lpMsg ,"GetFromDataBase",  0);

		return FALSE;
	}

	IDistConnectionXML* lpConn = CreateConnectObjcet();
	if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
	{
		//AfxMessageBox("Connect to file failed!\n");
		return FALSE;
	}

	IDistCommandXML* lpCmd = NULL;
	lpConn->QueryCommand(&lpCmd);

	char strXmlPath[] = "/NewDataSet/Table";
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
#ifdef _DEBUG
		CString msg = "TBUseLand_type ����δ�ҵ�����";
		AfxMessageBox(msg);
#endif
		lpCmd->Release();
		lpConn->Release();
		return FALSE;
	}

	BOOL bOK = lpRst->MoveFirst();
	while (bOK)
	{
		YDTYPECONFIG temp;
		CString tmp = (char*)((*lpRst)["TYPE_ID"]);
		temp.Type_Id = atoi(tmp);
		tmp = (char*)((*lpRst)["SUPER_ID"]);
		temp.Super_Id = atoi(tmp);
		temp.Type_Code = (char*)((*lpRst)["TYPE_CODE"]);
		temp.Type_Name = (char*)((*lpRst)["TYPE_NAME"]);
		temp.Description = (char*)((*lpRst)["DESCRIPTION"]);

		m_YdTypeArray.Add(temp);

		bOK = lpRst->MoveNext();
	}

	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();

	return TRUE;
}
*/

void CYdxz3Part::Release()
{
	m_YdTypeArray.RemoveAll();
}

void CYdxz3Part::GetRootNames(CStringArray& saRootName)
{
	for (int i = 0; i < m_YdTypeArray.GetSize(); i++)
	{
		YDTYPECONFIG temp;

		temp = m_YdTypeArray.GetAt(i);

		if (temp.Super_Id == 0)
		{
			saRootName.Add(temp.Type_Name);
		}
	}
}

void CYdxz3Part::GetParentNameByName(const CString sName, CString& sParentName)
{
	int nPId = 0;

	for (int i = 0; i < m_YdTypeArray.GetSize(); i++)
	{
		YDTYPECONFIG temp;

		temp = m_YdTypeArray.GetAt(i);

		if (temp.Type_Name.CollateNoCase(sName) == 0)
		{
			nPId = temp.Super_Id;
			break;
		}
	}

	if (nPId == 0)
	{
		return;
	}

	for (int i = 0; i < m_YdTypeArray.GetSize(); i++)
	{
		YDTYPECONFIG temp;

		temp = m_YdTypeArray.GetAt(i);

		if (temp.Type_Id == nPId)
		{
			sParentName = temp.Type_Name;
			return;
		}
	}
}

void CYdxz3Part::GetChildNameByName(const CString sName, CStringArray& saChildName)
{
	int nPId = -1;

	for (int i = 0; i < m_YdTypeArray.GetSize(); i++)
	{
		YDTYPECONFIG temp;

		temp = m_YdTypeArray.GetAt(i);

		if (temp.Type_Name.CollateNoCase(sName) == 0)
		{
			nPId = temp.Type_Id;
			break;
		}
	}

	if (nPId == -1)
	{
		return;
	}

	for (int i = 0; i < m_YdTypeArray.GetSize(); i++)
	{
		YDTYPECONFIG temp;

		temp = m_YdTypeArray.GetAt(i);

		if (temp.Super_Id == nPId)
		{
			saChildName.Add(temp.Type_Name);
		}
	}
}

void CYdxz3Part::GetCodeByName(const CString sName, CString& sCode, const int sParentId /*= 0*/)
{
	for (int i = 0; i < m_YdTypeArray.GetSize(); i++)
	{
		YDTYPECONFIG temp;

		temp = m_YdTypeArray.GetAt(i);

		if (temp.Type_Name.CollateNoCase(sName) == 0 &&
			temp.Super_Id == sParentId)
		{
			sCode = temp.Type_Code;
			break;
		}
	}
}

void CYdxz3Part::GetNameByCode(const CString sCode, CString& sName)
{
	for (int i = 0; i < m_YdTypeArray.GetSize(); i++)
	{
		YDTYPECONFIG temp;

		temp = m_YdTypeArray.GetAt(i);

		if (temp.Type_Code.CollateNoCase(sCode) == 0)
		{
			sName = temp.Type_Name;
			break;
		}
	}
}

int CYdxz3Part::GetIdByName(const CString sName, const int sParentId /* = 0 */)
{
	for (int i = 0; i < m_YdTypeArray.GetSize(); i++)
	{
		YDTYPECONFIG temp;

		temp = m_YdTypeArray.GetAt(i);

		if (temp.Type_Name.CollateNoCase(sName) == 0 &&
			temp.Super_Id == sParentId)
		{
			return temp.Type_Id;
		}
	}

	return -1;
}



//===============================================================================================//

BASESDEINFO              CBaseCfgInfo::m_BaseSdeInfo;
BASEDAPINFO              CBaseCfgInfo::m_BaseDapInfo;
//BASEPROJECTINFO          CBaseCfgInfo::m_BaseProjectInfo;
//BASEFTPINFO              CBaseCfgInfo::m_BaseFtpInfo;
DIST_PRJBASEINFO_STRUCT  CBaseCfgInfo::m_basePrjInfo;

AcGePoint3dArray CBaseCfgInfo::m_FWpts;     //
AcGePoint3dArray CBaseCfgInfo::m_XMBHpts;   //



//��ȡSDE������Ϣ
BOOL CBaseCfgInfo::ReadConnectInfoByXML(CString strXML)
{
	// create
	std::string strtmp("<NewDataSet><Table>");
	strtmp+=strXML.GetBuffer(strXML.GetLength());
	strXML.ReleaseBuffer();
	strtmp+="</Table></NewDataSet>";

	IDistConnectionXML* lpConn = CreateConnectObjcet();
	if (S_OK != lpConn->Connect(NULL, (_bstr_t)(strtmp.c_str()), NULL, NULL, NULL))
	{
		//AfxMessageBox("Connect to file failed!\n");
		return FALSE;
	}

	IDistCommandXML* lpCmd = NULL;
	lpConn->QueryCommand(&lpCmd);

	char strXmlPath[] = "/NewDataSet/Table";
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
		lpCmd->Release();
		lpConn->Release();
		return FALSE;
	}

	BOOL bOK = lpRst->MoveFirst();
	if (bOK)
	{
		m_BaseSdeInfo.strServer    = (char*)((*lpRst)["SERVICE_NAME"]);
        m_BaseSdeInfo.strInstance  = (char*)((*lpRst)["SERVER_PORT"]);
		m_BaseSdeInfo.strDatabase  = (char*)((*lpRst)["DATABASE_NAME"]);
		m_BaseSdeInfo.strUserName  = (char*)((*lpRst)["LOGIN_USER"]);
		m_BaseSdeInfo.strPassword  = (char*)((*lpRst)["LOGIN_PASSWORD"]);
		m_BaseSdeInfo.strGDBOwner  = (char*)((*lpRst)["SDE_USER"]);
	}
	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();

	return TRUE;
}

//��ȡSDE������Ϣ
BOOL CBaseCfgInfo::ReadConnectInfo(CString strURL)
{
	/*
	m_BaseSdeInfo.strServer   = "map-server";
	m_BaseSdeInfo.strInstance = "5151";
	m_BaseSdeInfo.strDatabase = "orcl";
	m_BaseSdeInfo.strUserName = "tzgis";
	m_BaseSdeInfo.strPassword = "tzgis";
	m_BaseSdeInfo.strGDBOwner = "tzgis";
	return TRUE;
	*/

	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();
	if (pWsAccessor == NULL) return FALSE;

	TCHAR lpMsg[512]={0};
	HRESULT hr = pWsAccessor->Connect(strURL.GetBuffer());
	if( hr != S_OK )// ������ӳ���
	{
		pWsAccessor->Answer(lpMsg);
		MessageBox(0, lpMsg ,"WebService",  0);
		return FALSE;
	}

	_variant_t varResult;

	// ���÷���
	hr = pWsAccessor->CallMethod("GetCADLoginConfigInformation",(_variant_t*)NULL, &varResult);
	if (hr != S_OK )// ���������ó�����Ϣ
	{
		pWsAccessor->Answer(lpMsg);
		//MessageBox(0, lpMsg ,"GetCADLoginConfigInformation",  0);
		return FALSE;
	}


	IDistConnectionXML* lpConn = CreateConnectObjcet();
	if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
	{
		//AfxMessageBox("Connect to file failed!\n");
		return FALSE;
	}

	IDistCommandXML* lpCmd = NULL;
	lpConn->QueryCommand(&lpCmd);

	char strXmlPath[] = "/NewDataSet/Table";
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
		lpCmd->Release();
		lpConn->Release();
		return FALSE;
	}

	BOOL bOK = lpRst->MoveFirst();
	if (bOK)
	{
		m_BaseSdeInfo.strServer    = (char*)((*lpRst)["SERVICE_NAME"]);
        m_BaseSdeInfo.strInstance  = (char*)((*lpRst)["SERVER_PORT"]);
		m_BaseSdeInfo.strDatabase  = (char*)((*lpRst)["DATABASE_NAME"]);
		m_BaseSdeInfo.strUserName  = (char*)((*lpRst)["LOGIN_USER"]);
		m_BaseSdeInfo.strPassword  = (char*)((*lpRst)["LOGIN_PASSWORD"]);
		m_BaseSdeInfo.strGDBOwner  = (char*)((*lpRst)["SDE_USER"]);
	}
	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();

	return TRUE;
}

/*
//��ȡSDE������Ϣ
BOOL CBaseCfgInfo::ReadFtpInfo(CString strURL)
{
	//m_BaseFtpInfo.strServer   = "192.168.1.66";
	//m_BaseFtpInfo.strUserName = "AffixFtpRoot";
	//m_BaseFtpInfo.strPassword = "1";
	//m_BaseFtpInfo.nPort = 21;
	//return TRUE;

	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();
	if (pWsAccessor == NULL) return FALSE;

	HRESULT hr = pWsAccessor->Connect(strURL.GetBuffer());
	TCHAR lpMsg[512]={0};

	// ������ӳ���
	if( hr != S_OK )
	{
		pWsAccessor->Answer(lpMsg); MessageBox(0, lpMsg ,"ReadFtpInfo",  0);return FALSE;
	}

	CString sSql = "SELECT SERVICE_NAME,SERVER_PORT,LOGIN_USER,LOGIN_PASSWORD FROM TBSERVER_INFO WHERE REF_TYPE_ID=2";

	// ע�⣬�������鳤��һ��Ҫ������һЩ
	_variant_t varParams[5];
	varParams[0] = sSql.GetBuffer();

	_variant_t varResult;

	// ���÷���
	hr = pWsAccessor->CallMethod("ExecSqlQuery",varParams, &varResult);

	// ���������ó�����Ϣ
	if (hr != S_OK ){
		pWsAccessor->Answer(lpMsg); MessageBox(0, lpMsg ,"ReadFtpInfo",  0); return FALSE;
	}

	IDistConnectionXML* lpConn = CreateConnectObjcet();
	if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
	{
		AfxMessageBox("Connect to file failed!\n");
		return FALSE;
	}

	IDistCommandXML* lpCmd = NULL;
	lpConn->QueryCommand(&lpCmd);

	char strXmlPath[] = "/NewDataSet/Table";
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
#ifdef _DEBUG
		CString msg = "TBSERVER_INFO ����δ�ҵ�����";
		AfxMessageBox(msg);
#endif
		lpCmd->Release();
		lpConn->Release();
		return FALSE;
	}

	BOOL bOK = lpRst->MoveFirst();
	if (bOK)
	{
		m_BaseFtpInfo.strServer    = (char*)((*lpRst)["SERVICE_NAME"]);
		m_BaseFtpInfo.strUserName  = (char*)((*lpRst)["LOGIN_USER"]);
		m_BaseFtpInfo.strPassword  = (char*)((*lpRst)["LOGIN_PASSWORD"]);
		CString strTemp            = (char*)((*lpRst)["SERVER_PORT"]);
		m_BaseFtpInfo.nPort = atoi(strTemp);
	}
	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();

	return TRUE;

}
*/



BOOL CBaseCfgInfo::ReadProjectBaseInfo(CString strXMBH, CString strURL)
{
	if (strXMBH.IsEmpty())  //XMBH�ڳ�������ʱ��DAP���͹�����
	{
		//AfxMessageBox("��ǰ��Ŀ��Ų���Ϊ�գ�");
		return FALSE;
	}
	m_basePrjInfo.strPROJECT_CODE = strXMBH;

	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();
	if (pWsAccessor == NULL) return FALSE;

	TCHAR lpMsg[512]={0};
	HRESULT hr = pWsAccessor->Connect(strURL.GetBuffer());
	if( hr != S_OK )// ������ӳ���
	{
		//pWsAccessor->Answer(lpMsg);
		//MessageBox(0, lpMsg ,"WebService",  0);
		return FALSE;
	}

	_variant_t varResult;
	_variant_t varParams[5];
	varParams[0] = strXMBH.GetBuffer();

	// ���÷���
	hr = pWsAccessor->CallMethod("GetProjectBaseInfo",varParams, &varResult);
	if (hr != S_OK )// ���������ó�����Ϣ
	{
		pWsAccessor->Answer(lpMsg);
		//MessageBox(0, lpMsg ,"GetProjectBaseInfo",  0);
		return FALSE;
	}


	IDistConnectionXML* lpConn = CreateConnectObjcet();
	if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
	{
		//AfxMessageBox("Connect to file failed!\n");
		return FALSE;
	}

	IDistCommandXML* lpCmd = NULL;
	lpConn->QueryCommand(&lpCmd);

	char strXmlPath[] = "/NewDataSet/Table";
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
#ifdef _DEBUG
		AfxMessageBox("TBPROJECT_BASE����δ�ҵ�����");
#endif
		lpCmd->Release();
		lpConn->Release();
		return FALSE;
	}

	BOOL bOK = lpRst->MoveFirst();

	// ֻ�е���Ψһ����
	if (bOK)
	{	
		m_basePrjInfo.strPROJECT_ID= (char*)((*lpRst)["PROJECT_ID"]);            // ��Ŀ���
		m_basePrjInfo.strPROJECT_CODE= (char*)((*lpRst)["PROJECT_CODE"]);          // ��Ŀ��ʾ��
		m_basePrjInfo.strRELATION_CODE= (char*)((*lpRst)["RELATION_CODE"]);         // �������
		m_basePrjInfo.strREG_TIME= (char*)((*lpRst)["REG_TIME"]);              // �Ǽ�ʱ��
		m_basePrjInfo.strPROJECT_NAME= (char*)((*lpRst)["PROJECT_NAME"]);          // ��Ŀ����
		m_basePrjInfo.strPRE_PROJECT_ID= (char*)((*lpRst)["REF_PROJECTDEFINE_ID"]);        // ��һҵ����?????
		m_basePrjInfo.strBUILD_UNIT= (char*)((*lpRst)["BUILD_UNIT"]);            // ���赥λ
		m_basePrjInfo.strBUILD_UNIT_LINKMAN= (char*)((*lpRst)["BUILD_UNIT_LINKMAN"]);    // ��ϵ��
		m_basePrjInfo.strBUILD_UNIT_PHONE= (char*)((*lpRst)["BUILD_UNIT_PHONE"]);      // �绰
		m_basePrjInfo.strOVER_TIME= (char*)((*lpRst)["OVER_TIME"]);             // �᰸ʱ��
		m_basePrjInfo.strBUILD_ADDRESS= (char*)((*lpRst)["BUILD_ADDRESS"]);      // ����ص�
		m_basePrjInfo.strLICENCE_CODE= (char*)((*lpRst)["LICENSE_CODE"]);        // ��֤���
		m_basePrjInfo.strUSELAND_TYPE= (char*)((*lpRst)["USELAND_TYPE"]);          // �õ�����
		m_basePrjInfo.strUSELAND_TYPEID= (char*)((*lpRst)["USELAND_TYPEID"]);        // �õ����ʱ�
		m_basePrjInfo.strARCHIITECTUREPRO_TYPE= (char*)((*lpRst)["ARCHIITECTUREPRO_TYPE"]); // ���蹤����
		m_basePrjInfo.strHOURSEARCHI_TYPE= (char*)((*lpRst)["HOURSEARCHI_TYPE"]);      // ��ס������
		m_basePrjInfo.strINVEST_SUM= (char*)((*lpRst)["INVEST_SUM"]);            // Ͷ���ܹ�ģ
		m_basePrjInfo.strFLOOR_AREA_RATIO= (char*)((*lpRst)["FLOOR_AREA_RATIO"]);      // �ݻ���
		m_basePrjInfo.strBUILDING_DENSITY= (char*)((*lpRst)["BUILDING_DENSITY"]);      // �����ܶ�
		m_basePrjInfo.strPRO_STATUS= (char*)((*lpRst)["PROJECT_STATUS"]);            // ��Ŀ״̬
		m_basePrjInfo.strTFBH = (char*)((*lpRst)["MAP_CODE"]);                  // ͼ�����
		m_basePrjInfo.strJBRY = (char*)((*lpRst)["JBRY"]);                  // ������Ա
		m_basePrjInfo.strCADTYPE = (char*)((*lpRst)["CADTYPE"]);                  //��������
		m_basePrjInfo.strSUM_LAND_AREA_SUM = (char*)((*lpRst)["SUM_LAND_AREA_SUM"]);  //���õ�������ģ�
		m_basePrjInfo.strBUILD_LAND_AREA_SUM = (char*)((*lpRst)["BUILD_LAND_AREA_SUM"]); //�����õ���������ģ�
		m_basePrjInfo.strBUILD_AREA_SUM = (char*)((*lpRst)["BUILD_AREA_SUM"]); //������������ģ�

		if (lpRst->MoveNext()) // ����ҵ�������¼��˵�����ݿ������������⣬ֱ�ӷ���
		{
			lpRst->Release();
			lpCmd->Release();
			lpConn->Release();  

			return FALSE;
		}
	}
  
	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();

	return TRUE;
}

BOOL CBaseCfgInfo::ReadUserInfo(CString strUserId, CString strURL)
{
	if (strUserId.IsEmpty()) //USERID �ڳ�������ʱ��DAP���͹�����
	{
		//AfxMessageBox("��ǰ��½�û�ID����Ϊ�գ�");
		return FALSE;
	}
	m_BaseDapInfo.strUserId = strUserId;


	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();
	if (pWsAccessor == NULL) return FALSE;

	TCHAR lpMsg[512]={0};
	HRESULT hr = pWsAccessor->Connect(strURL.GetBuffer());
	if( hr != S_OK )// ������ӳ���
	{
		pWsAccessor->Answer(lpMsg);
		//MessageBox(0, lpMsg ,"WebService",  0);
		return FALSE;
	}

	_variant_t varResult;
	_variant_t varParams[5];
	varParams[0] = strUserId.GetBuffer();

	// ���÷���
	hr = pWsAccessor->CallMethod("GetUserDetailByUserID",varParams, &varResult);
	if (hr != S_OK )// ���������ó�����Ϣ
	{
		pWsAccessor->Answer(lpMsg);
		//MessageBox(0, lpMsg ,"GetUserDetailByUserID",  0);
		return FALSE;
	}


	IDistConnectionXML* lpConn = CreateConnectObjcet();
	if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
	{
		//AfxMessageBox("Connect to file failed!\n");
		return FALSE;
	}

	IDistCommandXML* lpCmd = NULL;
	lpConn->QueryCommand(&lpCmd);

	char strXmlPath[] = "/NewDataSet/Table";
	IDistRecordSetXML* lpRst = NULL;
	lpCmd->SelectData(strXmlPath, &lpRst);

	if (NULL == lpRst)
	{
#ifdef _DEBUG
		AfxMessageBox("TBUSER����δ�ҵ�����");
#endif
		lpCmd->Release();
		lpConn->Release();
		return FALSE;
	}

	BOOL bOK = lpRst->MoveFirst();

	// ֻ�е���Ψһ����
	CString sLOGIN_NAME, iJOB_STATUS, sUSER_NAME, iUSER_ID, sREMARK, dREG_TIME, sPASSWORD;
	if (bOK)
	{
		sLOGIN_NAME = (char*)((*lpRst)["LOGIN_NAME"]); // ��½����
		iJOB_STATUS = (char*)((*lpRst)["JOB_STATUS"]); // ��������
		sUSER_NAME = (char*)((*lpRst)["USER_NAME"]); // �û�����
		iUSER_ID = (char*)((*lpRst)["USER_ID"]); // �û�ID
		sREMARK = (char*)((*lpRst)["REMARK"]); // ��������
		dREG_TIME = (char*)((*lpRst)["REG_TIME"]); // ��½ʱ��
		sPASSWORD = (char*)((*lpRst)["PASSWORD"]); // ��������

		if (lpRst->MoveNext()) // ����ҵ�������¼��˵�����ݿ������������⣬ֱ�ӷ���
		{
			lpRst->Release();
			lpCmd->Release();
			lpConn->Release();

			return FALSE;
		}
	}

	m_BaseDapInfo.strUserName = sUSER_NAME;
	m_basePrjInfo.strJBRY = sUSER_NAME;

	lpRst->Release();
	lpCmd->Release();
	lpConn->Release();

	return TRUE;
}


// ��ȡ��Ŀ��ŵĵ����飬�洢��CDefineFile::g_XMBHpts
BOOL CBaseCfgInfo::GetXMBHPoints(AcGePoint3dArray& ptArray)
{
	if(NULL == g_pExchangeData) return FALSE;
	ptArray.setLogicalLength(0);
	CString strSdeLayerName, strFieldName;
	if (FALSE == CPoltIndex::GetXMBHInfo(strSdeLayerName, strFieldName))return FALSE;

	char strSQL[255]={0};
	sprintf(strSQL,"F:OBJECTID,SHAPE,T:%s,W:%s=%s,##", strSdeLayerName,strFieldName, m_basePrjInfo.strPROJECT_CODE);

	CStringArray strArray;
	strArray.Add("Project_ID");
	CString strRegName = "DIST_" + strSdeLayerName;
	g_pExchangeData->ReadSymbolInfo(g_pConnection, NULL,NULL,strSdeLayerName,strRegName,&strArray);
	g_pExchangeData->GetSearchShapePointArray(strSQL, &ptArray);

	return (!ptArray.isEmpty());
}


// ��ȡ��ͼͼ���ŷ�Χ�ĵ����飬�洢��CDefineFile::g_FWpts
BOOL CBaseCfgInfo::GetHTFWPoints()
{
	/*
	//CStringArray* psaSdeLayer = new CStringArray;
	//CStringArray* psaField = new CStringArray;
	CStringArray strSdeLyNameArray,strFldNameArray;
	if (FALSE == CPoltIndex::GetLayerPoltIndexArray(strSdeLyNameArray, strFldNameArray)) return FALSE;

	BOOL bFind = FALSE;
	for (int i = 0; i < strSdeLyNameArray.GetSize() && !bFind; i++)
	{
		CString sSql;
		sSql.Format("%s = \'%s\'", strFldNameArray.GetAt(i), m_BaseProjectInfo.strTFBH);

		AcDbObjectIdArray arrId;
		long lRt = CSdeDataAttr::ReadAllSdeLayer(arrId, strSdeLyNameArray.GetAt(i), NULL, sSql.GetBuffer());
		if (lRt == 1)
		{
			if (arrId.logicalLength() == 0)
			{
				continue;
			}
			else if (arrId.logicalLength() == 1)
			{
				CDefineFile::g_FWpts.setLogicalLength(0);

				AcDbObjectId objId = arrId.at(0);
				AcDbPolyline * pPline = NULL;
				if (Acad::eOk != acdbOpenObject(pPline, objId, AcDb::kForWrite))
				{
					continue;
				}
				int ptNum = pPline->numVerts();
				for (int pos = 0; pos < ptNum; pos++)
				{
					AcGePoint3d pt;
					if (Acad::eOk == pPline->getPointAt(pos, pt))
					{
						CDefineFile::g_FWpts.append(pt);
					}
				}
				if (pPline->isClosed() == Adesk::kTrue)
				{
					AcGePoint3d pt;
					if (Acad::eOk == pPline->getPointAt(0, pt))
					{
						CDefineFile::g_FWpts.append(pt);
					}
				}
				pPline->erase();
				pPline->close();

				bFind = TRUE;
				continue;
			}
			else
			{
				CDefineFile::g_FWpts.setLogicalLength(0);

				AcDbExtents Max_extents;
				for (int j = 0; j < arrId.logicalLength(); j++)
				{
					AcDbObjectId objId = arrId.at(j);
					AcDbEntity * pEnt = NULL;
					if (Acad::eOk != acdbOpenObject(pEnt, objId, AcDb::kForWrite))
					{
						continue;
					}
					pEnt->close();
					AcDbExtents extents;
					pEnt->getGeomExtents(extents);
					Max_extents.addExt(extents);
				}

				AcGePoint3d pt = AcGePoint3d(0, 0, 0);
				pt.x = Max_extents.minPoint().x;
				pt.y = Max_extents.minPoint().y;
				CDefineFile::g_FWpts.append(pt);// ���½ǵ�
				pt.x = Max_extents.maxPoint().x;
				pt.y = Max_extents.minPoint().y;
				CDefineFile::g_FWpts.append(pt);// ���½ǵ�
				pt.x = Max_extents.maxPoint().x;
				pt.y = Max_extents.maxPoint().y;
				CDefineFile::g_FWpts.append(pt);// ���Ͻǵ�
				pt.x = Max_extents.minPoint().x;
				pt.y = Max_extents.maxPoint().y;
				CDefineFile::g_FWpts.append(pt);// ���Ͻǵ�
				pt.x = Max_extents.minPoint().x;
				pt.y = Max_extents.minPoint().y;
				CDefineFile::g_FWpts.append(pt);// ���½ǵ�(��״)

				for (int j = 0; j < arrId.logicalLength(); j++)
				{
					AcDbObjectId objId = arrId.at(j);
					AcDbEntity * pEnt = NULL;
					if (Acad::eOk != acdbOpenObject(pEnt, objId, AcDb::kForWrite))
					{
						continue;
					}
					pEnt->erase();
					pEnt->close();
				}

				bFind = TRUE;
				continue;
			}
		}
	}

	acedRedraw(NULL,0);

	CStringArray* pCadLayer = new CStringArray;
	GetCadLayerBySdeLayer(psaSdeLayer, pCadLayer);
	for (int i = 0; i < pCadLayer->GetSize(); i++)
	{
		if (!DeleteLayer(pCadLayer->GetAt(i)))
		{
#ifdef _DEBUG
			AfxMessageBox("ɾ��ͼ��ʧ�ܣ�");
#endif
		}
	}
	delete pCadLayer;

	delete psaSdeLayer;
	delete psaField;
	return bFind;
	*/
	return TRUE;
}

/*
//��ȡSDE������Ϣ
BOOL CBaseCfgInfo::ReadConnectInfo(CString strURL)
{
m_BaseSdeInfo.strServer   = "dbserver01";
m_BaseSdeInfo.strInstance = "5151";
m_BaseSdeInfo.strDatabase = "xmghsde";
m_BaseSdeInfo.strUserName = "xmghsde";
m_BaseSdeInfo.strPassword = "xmghsde";
m_BaseSdeInfo.strGDBOwner = "xmghsde";

IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();
if (pWsAccessor == NULL) return FALSE;

HRESULT hr = pWsAccessor->Connect(strURL.GetBuffer());
TCHAR lpMsg[512]={0};

// ������ӳ���
if( hr != S_OK )
{
pWsAccessor->Answer(lpMsg); MessageBox(0, lpMsg ,"ReadConnectInfo",  0);return FALSE;
}

CString sSql = "SELECT SERVICE_NAME,SERVER_PORT,DATABASE_NAME,LOGIN_USER,LOGIN_PASSWORD,SDE_USER FROM TBSERVER_INFO WHERE REF_TYPE_ID=1";

// ע�⣬�������鳤��һ��Ҫ������һЩ
_variant_t varParams[5];
varParams[0] = sSql.GetBuffer();

_variant_t varResult;

// ���÷���
hr = pWsAccessor->CallMethod("ExecSqlQuery",varParams, &varResult);

// ���������ó�����Ϣ
if (hr != S_OK ){
pWsAccessor->Answer(lpMsg); MessageBox(0, lpMsg ,"ReadConnectInfo",  0); return FALSE;
}

IDistConnectionXML* lpConn = CreateConnectObjcet();
if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
{
AfxMessageBox("Connect to file failed!\n");
return FALSE;
}

IDistCommandXML* lpCmd = NULL;
lpConn->QueryCommand(&lpCmd);

char strXmlPath[] = "/NewDataSet/Table";
IDistRecordSetXML* lpRst = NULL;
lpCmd->SelectData(strXmlPath, &lpRst);

if (NULL == lpRst)
{
#ifdef _DEBUG
CString msg = "TBSERVER_INFO ����δ�ҵ�����";
AfxMessageBox(msg);
#endif
lpCmd->Release();
lpConn->Release();
return FALSE;
}

BOOL bOK = lpRst->MoveFirst();
if (bOK)
{
m_BaseSdeInfo.strServer    = (char*)((*lpRst)["SERVICE_NAME"]);
m_BaseSdeInfo.strInstance  = (char*)((*lpRst)["SERVER_PORT"]);
m_BaseSdeInfo.strDatabase  = (char*)((*lpRst)["DATABASE_NAME"]);
m_BaseSdeInfo.strUserName  = (char*)((*lpRst)["LOGIN_USER"]);
m_BaseSdeInfo.strPassword  = (char*)((*lpRst)["LOGIN_PASSWORD"]);
m_BaseSdeInfo.strGDBOwner  = (char*)((*lpRst)["SDE_USER"]);
}
lpRst->Release();
lpCmd->Release();
lpConn->Release();

return TRUE;

}


BOOL CBaseCfgInfo::ReadProjectBaseInfo(CString strXMBH, CString strURL)
{
if (strXMBH.IsEmpty())  //XMBH�ڳ�������ʱ��DAP���͹�����
{
AfxMessageBox("��ǰ��Ŀ��Ų���Ϊ�գ�");
return FALSE;
}
m_BaseProjectInfo.strXMBH = strXMBH;
int nProjectId = atoi(strXMBH);

IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();
if (pWsAccessor == NULL) return FALSE;

HRESULT hr = pWsAccessor->Connect(strURL);
TCHAR lpMsg[512]={0};

// ������ӳ���
if( hr != S_OK )
{
pWsAccessor->Answer(lpMsg);
MessageBox(0, lpMsg ,"ReadProjectBaseInfo",  0);
return FALSE;
}

CString sSql;
sSql.Format("select * from TBPROJECT_BASE where DELFLAG = 0 and PROJECT_ID = %d", nProjectId);

// ע�⣬�������鳤��һ��Ҫ������һЩ
_variant_t varParams[5];
varParams[0] = sSql.GetBuffer();

_variant_t varResult;

// ���÷���
hr = pWsAccessor->CallMethod("ExecSqlQuery",varParams, &varResult);

// ���������ó�����Ϣ
if (hr != S_OK )
{
pWsAccessor->Answer(lpMsg);

MessageBox(0, lpMsg ,"ReadProjectBaseInfo",  0);

return FALSE;
}

IDistConnectionXML* lpConn = CreateConnectObjcet();
if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
{
AfxMessageBox("Connect to file failed!\n");
return FALSE;
}

IDistCommandXML* lpCmd = NULL;
lpConn->QueryCommand(&lpCmd);

char strXmlPath[] = "/NewDataSet/Table";
IDistRecordSetXML* lpRst = NULL;
lpCmd->SelectData(strXmlPath, &lpRst);

if (NULL == lpRst)
{
#ifdef _DEBUG
AfxMessageBox("TBPROJECT_BASE����δ�ҵ�����");
#endif
lpCmd->Release();
lpConn->Release();
return FALSE;
}

BOOL bOK = lpRst->MoveFirst();
//while (bOK)
//{
//	bOK = lpRst->MoveNext();
//}
// ֻ�е���Ψһ����
CString sPROJECT_CODE, sLICENSE_CODE, sDRAW_TIME, dREG_TIME, sBUILD_UNIT, sBUILD_ADDRESS,
sPROJECT_NAME, sRELATION_CODE, sMAP_CODE, sAFFIX, dOVER_TIME, iPROJECT_STATUS,
sREF_PROJECTDEFINE_ID, iPROJECT_ID, sBUILD_UNIT_PHONE, sBUILD_UNIT_LINKMAN,
sLINKMAN_IDENTITYID, iDELFLAG, sREF_PACKAGE_ID;
if (bOK)
{
sPROJECT_CODE = (char*)((*lpRst)["PROJECT_CODE"]); // ��Ŀ��ʾ���
sLICENSE_CODE = (char*)((*lpRst)["LICENSE_CODE"]); // ��֤���
sDRAW_TIME = (char*)((*lpRst)["DRAW_TIME"]); // ��ȡʱ��
dREG_TIME = (char*)((*lpRst)["REG_TIME"]); // �Ǽ�ʱ��
sBUILD_UNIT = (char*)((*lpRst)["BUILD_UNIT"]); // ���赥λ����
sBUILD_ADDRESS = (char*)((*lpRst)["BUILD_ADDRESS"]); // �����ַ
sPROJECT_NAME = (char*)((*lpRst)["PROJECT_NAME"]); // ��Ŀ����
sRELATION_CODE = (char*)((*lpRst)["RELATION_CODE"]); // �������
sMAP_CODE = (char*)((*lpRst)["MAP_CODE"]); // ͼ�����
sAFFIX = (char*)((*lpRst)["AFFIX"]); // ���ϴ�
dOVER_TIME = (char*)((*lpRst)["OVER_TIME"]); // �᰸ʱ��
iPROJECT_STATUS = (char*)((*lpRst)["PROJECT_STATUS"]); // ��Ŀ״̬
sREF_PROJECTDEFINE_ID = (char*)((*lpRst)["REF_PROJECTDEFINE_ID"]); // ��Ŀ���ͱ��
iPROJECT_ID = (char*)((*lpRst)["PROJECT_ID"]); // ��Ŀ�������
sBUILD_UNIT_PHONE = (char*)((*lpRst)["BUILD_UNIT_PHONE"]); // ���赥λ�绰
sBUILD_UNIT_LINKMAN = (char*)((*lpRst)["BUILD_UNIT_LINKMAN"]); // ��ϵ��
sLINKMAN_IDENTITYID = (char*)((*lpRst)["LINKMAN_IDENTITYID"]); // ���֤��
iDELFLAG = (char*)((*lpRst)["DELFLAG"]); // �Ƿ�ɾ����־
sREF_PACKAGE_ID = (char*)((*lpRst)["REF_PACKAGE_ID"]); // ��Ŀ���ͱ��

if (lpRst->MoveNext()) // ����ҵ�������¼��˵�����ݿ������������⣬ֱ�ӷ���
{
lpRst->Release();
lpCmd->Release();
lpConn->Release();

return FALSE;
}
}

m_BaseProjectInfo.strGLBH	=	sRELATION_CODE;
m_BaseProjectInfo.strTFBH	=	sMAP_CODE;
m_BaseProjectInfo.strLCH	=	sPROJECT_CODE;
m_BaseProjectInfo.strZH  	=	sLICENSE_CODE;
m_BaseProjectInfo.strXMMC	=	sPROJECT_NAME;
m_BaseProjectInfo.strJSDW	=	sBUILD_UNIT;
m_BaseProjectInfo.strJSDZ	=	sBUILD_ADDRESS;
m_BaseProjectInfo.strYDXZ	=	sREF_PROJECTDEFINE_ID;


lpRst->Release();
lpCmd->Release();
lpConn->Release();

return TRUE;
}



BOOL CBaseCfgInfo::ReadUserInfo(CString strUserId, CString strURL)
{
if (strUserId.IsEmpty()) //USERID �ڳ�������ʱ��DAP���͹�����
{
AfxMessageBox("��ǰ��½�û�ID����Ϊ�գ�");
return FALSE;
}
m_BaseDapInfo.strUserId = strUserId;
int nUserId = atoi(strUserId);

IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();

if (pWsAccessor == NULL)		
{
return FALSE;
}

HRESULT hr = pWsAccessor->Connect(strURL); 

TCHAR lpMsg[512]={0};

// ������ӳ���
if( hr != S_OK )
{
pWsAccessor->Answer(lpMsg);
MessageBox(0, lpMsg ,"ReadUserInfo",  0);
return FALSE;
}


CString sSql;
sSql.Format("select * from TBUSER where USER_ID = %d", nUserId);

// ע�⣬�������鳤��һ��Ҫ������һЩ
_variant_t varParams[5];
varParams[0] = sSql.GetBuffer();

_variant_t varResult;

// ���÷���
hr = pWsAccessor->CallMethod("ExecSqlQuery",varParams, &varResult);

sSql.ReleaseBuffer();

// ���������ó�����Ϣ
if (hr != S_OK )
{
pWsAccessor->Answer(lpMsg);

MessageBox(0, lpMsg ,"ReadUserInfo",  0);

return FALSE;
}

IDistConnectionXML* lpConn = CreateConnectObjcet();
if (S_OK != lpConn->Connect(NULL, (_bstr_t)varResult, NULL, NULL, NULL))
{
AfxMessageBox("Connect to file failed!\n");
return FALSE;
}

IDistCommandXML* lpCmd = NULL;
lpConn->QueryCommand(&lpCmd);

char strXmlPath[] = "/NewDataSet/Table";
IDistRecordSetXML* lpRst = NULL;
lpCmd->SelectData(strXmlPath, &lpRst);

if (NULL == lpRst)
{
#ifdef _DEBUG
AfxMessageBox("TBUSER����δ�ҵ�����");
#endif
lpCmd->Release();
lpConn->Release();
return FALSE;
}

BOOL bOK = lpRst->MoveFirst();

// ֻ�е���Ψһ����
CString sLOGIN_NAME, iJOB_STATUS, sUSER_NAME, iUSER_ID, sREMARK, dREG_TIME, sPASSWORD;
if (bOK)
{
sLOGIN_NAME = (char*)((*lpRst)["LOGIN_NAME"]); // ��½����
iJOB_STATUS = (char*)((*lpRst)["JOB_STATUS"]); // ��������
sUSER_NAME = (char*)((*lpRst)["USER_NAME"]); // �û�����
iUSER_ID = (char*)((*lpRst)["USER_ID"]); // �û�ID
sREMARK = (char*)((*lpRst)["REMARK"]); // ��������
dREG_TIME = (char*)((*lpRst)["REG_TIME"]); // ��½ʱ��
sPASSWORD = (char*)((*lpRst)["PASSWORD"]); // ��������

if (lpRst->MoveNext()) // ����ҵ�������¼��˵�����ݿ������������⣬ֱ�ӷ���
{
lpRst->Release();
lpCmd->Release();
lpConn->Release();

return FALSE;
}
}

m_BaseDapInfo.strUserName = sUSER_NAME;

lpRst->Release();
lpCmd->Release();
lpConn->Release();

return TRUE;
}

*/







// WSAccessor.cpp: implementation of the WSAccessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WSAccessor.h"

//
#define RETURN_ON_FAILURE(x)    if (FAILED(x)) return (x)
#define GOTO_ON_FAILURE(x, lbl) if (FAILED(x)) goto lbl

#define RELEASE_INTERFACE(x) if (x) (x)->Release(); (x) = NULL;
#define SAFE_RELEASE_INTERFACE(x) __try{if (x) x->Release(); x = NULL;}__except(1){}

#define SAFE_FREESTRING(s) if(s){MemFree(s); s = NULL;}
#define CHECK_NULL_RETURN(var, err) if(NULL==(var)) return (err)

//////////////////////////////////////////////////////////////////////
/// WSParam
WSParam::WSParam()
{
	m_varParams = NULL;

	m_nSize = 0;
	m_nCount = 0;
}

WSParam::WSParam(int nSize)
{
	m_varParams = new _variant_t[nSize];
	m_nSize = nSize;
	m_nCount = 0; 
}

WSParam::~WSParam()
{
	if( m_varParams != NULL )
		delete[] m_varParams;
}

void WSParam::Add(LPCTSTR sName, LPCTSTR sValue)
{
	_vecName.push_back(sName);
	_vecValue.push_back(sValue);
}

void WSParam::Add(_variant_t& sValue)
{
	if( m_nCount >= m_nSize)
		throw "超出范围";
	
	m_varParams[m_nCount] = sValue;
	m_nCount++;
}
void WSParam::Clear()
{
	_vecName.clear();
	_vecValue.clear();
}

int WSParam::GetSize() 
{
	if( m_nCount>0)
		return m_nCount;
	
	return _vecName.size();
}

void WSParam::GetName(int idx, LPTSTR szName)
{
	lstrcpy(szName, _vecName[idx].c_str());
}

void WSParam::GetValue(int idx, LPTSTR szValue)
{
	lstrcpy(szValue, _vecValue[idx].c_str());
}

_variant_t& WSParam::GetValue(int idx)
{
	return m_varParams[idx];
}


//////////////////////////////////////////////////////////////////////
/// WSAccessor

WSAccessor::WSAccessor(LPCTSTR sEndPointURL, LPCTSTR sNameSpace, LPCTSTR sRpcUri)
{
	if (sEndPointURL[0] == 0x00)
	{
		throw "Invalid EndURL!";
	}
	_sEndPointURL = sEndPointURL;

	if (sNameSpace[0] == 0x00)
		_sNameSpace = sEndPointURL;
	else
		_sNameSpace = sNameSpace;

	if ('/' != _sNameSpace[_sNameSpace.size() - 1])
	{
		_sNameSpace += '/';
	}

	if (sRpcUri[0] == 0x00)
		_sRpcURI = "urn:SoapServer";
	else
		_sRpcURI = sRpcUri;

	initialize();
}

WSAccessor::~WSAccessor()
{
	if( m_pSoapClient != NULL )
	{
		m_pSoapClient->Release();
		m_pSoapClient = NULL;
	}
}

HRESULT WSAccessor::Access(LPCTSTR sMethod, IWSParam* lpParams, LPCTSTR  sSoapAction, LPCTSTR sRpcURI)
{
	_sAnswer = "";
	
	if (_sEndPointURL.size() == 0)
	{
		_sAnswer = "无效的目的地址！";
		return -1;
	}

	string sAction = (sSoapAction[0] == 0x00 ? (_sNameSpace + sMethod) : (sSoapAction));
	string sURI = (sRpcURI[0]==0x00 ? _sRpcURI : (sRpcURI));
	
	HRESULT hr = 0;
	
	try
	{
		if( !m_bConnected)
		{
			/// 创建连接
//			ISoapConnectorPtr Connector;
			
			if( m_pSoapConnector == NULL )
			{
				//m_pSoapConnector = ISoapClientPtr(__uuidof(SoapClient30));
				m_pSoapConnector.CreateInstance(__uuidof(HttpConnector30));
				if( m_pSoapConnector == NULL ) return S_FALSE;
			}

			//hr = Connector.CreateInstance(__uuidof(HttpConnector30));
			m_pSoapConnector->Property["EndPointURL"] = _sEndPointURL.c_str();
			m_pSoapConnector->Property["SoapAction"] = sAction.c_str();
			m_pSoapConnector->Property["Timeout"] = _variant_t(100L);
			
			hr = m_pSoapConnector->Connect();

			if (S_OK != hr)
			{
				_sAnswer = "创建连接失败！";
				return hr;
			}

			m_bConnected = TRUE;
		}
		
		/// 开始传送数据
		hr = m_pSoapConnector->BeginMessage();
		{
			/// 序列化数据
			ISoapSerializerPtr Serializer;
			hr = Serializer.CreateInstance(__uuidof(SoapSerializer30));
			hr = Serializer->Init(_variant_t((IUnknown*)m_pSoapConnector->InputStream));
			hr = Serializer->StartEnvelope("", "", "");
			{
				hr = Serializer->SoapNamespace("xsi", "http://www.w3.org/1999/XMLSchema-instance");
				hr = Serializer->SoapNamespace("xsd", "http://www.w3.org/1999/XMLSchema");
				hr = Serializer->StartBody("http://schemas.xmlsoap.org/soap/encoding/");
				{
					/// 开始方法
					hr = Serializer->StartElement(sMethod, sURI.c_str(), "", "ns1");
					{
						/// 方法的参数
						for (int i = 0; i < lpParams->GetSize(); i++)
						{
							TCHAR lpBuffer[4096] = {0};

							lpParams->GetName(i, lpBuffer);
							hr = Serializer->StartElement(lpBuffer, "", "", "");

							lpBuffer[0] = 0x00;
							lpParams->GetValue(i, lpBuffer);
							hr = Serializer->WriteString(lpBuffer);

							hr = Serializer->EndElement();
						}
						
					}	
					hr = Serializer->EndElement();
				}
				hr = Serializer->EndBody();
			}
			hr = Serializer->EndEnvelope();
		}
		/// 结束数据
		hr = m_pSoapConnector->EndMessage();
		
		/// 读取反馈
		ISoapReaderPtr Reader;
		Reader.CreateInstance(__uuidof(SoapReader30));
		Reader->Load(_variant_t((IUnknown*)m_pSoapConnector->OutputStream), "");
		if(Reader->Fault == NULL)
		{
			_sAnswer = (const char*)(Reader->RpcResult->text);
			return 0;
		}
		else 
		{
			_sAnswer = (const char*)(Reader->Fault->text);	
			return -1;
		}
	}
	catch(...)
	{
		_sAnswer = "发送数据时发生异常！";
		return -1;
	}
	
	return 0;
}

HRESULT WSAccessor::Answer(LPTSTR szResult)
{
	lstrcpy(szResult, _sAnswer.c_str());
	return S_OK;
}

WSAccessor::WSAccessor()
{
	initialize();
}

HRESULT WSAccessor::Connect(LPCTSTR szWsdlUrl)
{
	if( m_bConnected ) return S_OK;

	if( szWsdlUrl[0] == 0x00 )		
	{
		_sAnswer = "无效的URL";
		return S_FALSE;
	}

	try
	{
		HRESULT hr = CoCreateInstance(__uuidof(SoapClient30), NULL, CLSCTX_INPROC_SERVER, __uuidof(ISoapClient),(void**)&m_pSoapClient);
		if( hr != S_OK )
		{
			_sAnswer = "创建SoapClient30对象失败，请注册，regsvr32";
			return hr;
		}

		_bstr_t bstrUrl = szWsdlUrl;
		hr = m_pSoapClient->MSSoapInit(bstrUrl, "", "", "");

		if( hr != S_OK )
		{		
			_sAnswer = m_pSoapClient->FaultString;
			return hr;
		}
	}
	catch (...) {	
		_sAnswer = "连接异常";
		return S_FALSE;
	}

	m_bConnected = TRUE;
	return S_OK;
}

void WSAccessor::initialize()
{
	_sAnswer = "";

	m_pSoapConnector = NULL;

	m_bConnected = FALSE;
	m_pSoapClient = NULL;
}

HRESULT WSAccessor::CallMethod(LPCTSTR szMethod, IWSParam* wsParams, _variant_t* szResult)
{
	if(szMethod[0] == 0x00)	return S_FALSE;

	if( !m_bConnected )
	{
		_sAnswer = "请先connect";
		return S_FALSE;
	}

	_sAnswer = "";
	
	int nCount = wsParams->GetSize();
	
	_variant_t*	varParams = NULL;

	if( nCount > 0 )
	{
		varParams = new _variant_t[nCount];

		
		for(int i=0; i<nCount; i++)//for(int i=nCount-1; i>=0; i--)
		{
			//TCHAR	lpBuffer[4096] = {0};
			//wsParams->GetValue(i, lpBuffer);
			varParams[i] = wsParams->GetValue(i);//lpBuffer;
		}
	}

	_bstr_t bstrMethod = szMethod;
	
	HRESULT hr = AutoDispInvoke(m_pSoapClient, bstrMethod, 0, DISPATCH_METHOD, nCount, varParams, szResult);

	if( varParams )
		delete[] varParams;
	
	RETURN_ON_FAILURE(hr);


	_bstr_t bResult = (_bstr_t)*szResult;
    char* lpResult = bResult;

	_sAnswer = lpResult;
	
	return S_OK;
}

HRESULT WSAccessor::CallMethod(LPCSTR szMethod, _variant_t* szResult,...)
{
	return 0;
}

HRESULT WSAccessor::CallMethod(LPCSTR szMethod, _variant_t* varParams, _variant_t* varResult)
{
	if(szMethod[0] == 0x00)	return S_FALSE;

	if( !m_bConnected )
	{
		_sAnswer = "请先connect";
		return S_FALSE;
	}

	_sAnswer = "";
	
	_bstr_t bstrMethod = szMethod;

	int  nParam = 0;

	if( varParams != NULL )
	{
		while (varParams[nParam].vt != VT_EMPTY) nParam++;
	}

	HRESULT hr = AutoDispInvoke(m_pSoapClient, bstrMethod, 0, DISPATCH_METHOD, nParam, varParams, varResult);

	if( FAILED(hr) && 0x80020006 != hr)
	{
		*varResult = m_pSoapClient->GetFaultString();		
	}

	_bstr_t bResult = (_bstr_t)*varResult;
    char* lpResult = bResult;

	_sAnswer = lpResult;
	
	return hr;
}

HRESULT WSAccessor::AutoDispInvoke(LPDISPATCH pdisp, LPOLESTR pwszname, DISPID dspid,
					WORD wflags, DWORD cargs, VARIANT* rgargs, VARIANT* pvtret)
{
    HRESULT    hr;
    DISPID     dspidPut = DISPID_PROPERTYPUT;
    DISPPARAMS dspparm = {NULL, NULL, 0, 0};

	CHECK_NULL_RETURN(pdisp, E_POINTER);

    dspparm.rgvarg = rgargs;
    dspparm.cArgs = cargs;

	if ((wflags & DISPATCH_PROPERTYPUT) || (wflags & DISPATCH_PROPERTYPUTREF))
	{
		dspparm.rgdispidNamedArgs = &dspidPut;
		dspparm.cNamedArgs = 1;
	}

	try
	{
		if (pwszname)
		{
			hr = pdisp->GetIDsOfNames(IID_NULL, &pwszname, 1, LOCALE_USER_DEFAULT, &dspid);
			//RETURN_ON_FAILURE(hr);
			if( FAILED(hr))
			{
				*(_variant_t*)pvtret = L"方法名获取失败，可能拼写错误!";
				return hr;
			}
		}

        hr = pdisp->Invoke(dspid, IID_NULL, LOCALE_USER_DEFAULT,
            (WORD)(DISPATCH_METHOD | wflags), &dspparm, pvtret, NULL, NULL);

    }
    catch(...)
    {
        hr = S_FALSE;
    }

    return hr;
}


XSingletonFactory * XSingletonFactory::m_lpInstance = NULL;

XSingletonFactory * XSingletonFactory::Instance()
{
	if( m_lpInstance == NULL )
	{
		static XSingletonFactory	xxSingleton;
		m_lpInstance = &xxSingleton;
	}
	return m_lpInstance;
}

XSingletonFactory::XSingletonFactory()
{
	CoInitialize(NULL);
}

XSingletonFactory::~XSingletonFactory()
{
	CoUninitialize();
}

IWSAccessor*	XSingletonFactory::CreateWSAccessorObject(LPCTSTR sEndPointURL, LPCTSTR sNameSpace, LPCTSTR sRpcUri)
{
	return new WSAccessor(sEndPointURL, sNameSpace, sRpcUri);
}

IWSAccessor*	XSingletonFactory::CreateVarWSAccessorObject()
{
	return new WSAccessor();
}

IWSParam*		XSingletonFactory::CreateWSParamObject()
{
	return new WSParam();	
}
IWSParam*		XSingletonFactory::CreateVarWSParamObject()
{
	return new WSParam(16);
}

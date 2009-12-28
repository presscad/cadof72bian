/************************************************************************/
/* ����webservice��ط��ʽӿ�                                               */
/************************************************************************/

#ifndef AFX_WSACCESSOR_EXPORT_H__INCLUDED_ 
#define AFX_WSACCESSOR_EXPORT_H__INCLUDED_ 

#ifdef WSACCESSOR_EXPORTS
#define WSACCESSOR_API __declspec(dllexport)
#else
#define WSACCESSOR_API __declspec(dllimport)
#endif

#include <COMUTIL.h>


//////////////////////////////////////////////////////////////////////////
//����web�����з���������
//////////////////////////////////////////////////////////////////////////
struct IWSParam 
{
	/// ����һ��������������������ֵ
	virtual void Add(LPCTSTR szName,  LPCTSTR szValue)=0;

	/// ���Ӳ���������ֵѹջ
	virtual void Add(_variant_t& sValue)=0;
	
	//
	virtual void Clear()=0;

	virtual int GetSize()=0;

	virtual void GetName(int idx, LPTSTR szName)=0 ;

	virtual void GetValue(int idx, LPTSTR szValue)=0;
	virtual _variant_t& GetValue(int idx)=0;

	virtual void Release(void)=0;
};

//////////////////////////////////////////////////////////////////////////
// WebService�ķ�����
// ͨ��XSingletonFactory::Instance()->CreateVarWSAccessorObject����
//////////////////////////////////////////////////////////////////////////

struct IWSAccessor 
{
	//////////////////////////////////////////////////////////////////////////	
	// ����webService, szWsdlUrl--WebService��WSDL·��
	//////////////////////////////////////////////////////////////////////////	
	virtual HRESULT Connect(LPCTSTR szWsdlUrl)=0;

	// ����webservice�ķ���
	// szMethod ��������lpParams��������
	virtual HRESULT Access(LPCTSTR szMethod, IWSParam * lpParams, LPCTSTR szSoapAction, LPCTSTR szRpcURI)=0;

	// ����webservice�ķ���
	// szMethod ��������lpParams��������, varResult���ط���ֵ
	virtual HRESULT CallMethod(LPCSTR szMethod, IWSParam * lpParams, _variant_t* szResult)=0;

	// ����webservice�ķ���
	virtual HRESULT CallMethod(LPCSTR szMethod, _variant_t* szResult,...)=0;

	// ����webservice�ķ���
	// szMethod ��������varParams��������, varResult���ط���ֵ
	virtual HRESULT CallMethod(LPCSTR szMethod, _variant_t* varParams, _variant_t* varResult)=0;

	// ���webservice�ķ�������ֵ
	virtual HRESULT Answer(LPTSTR szResult)=0;	

	// �ӿ��ͷ�
	virtual HRESULT Release(void)=0;
};

//////////////////////////////////////////////////////////////////////////
// ����web����Ĺ�����
//////////////////////////////////////////////////////////////////////////
class WSACCESSOR_API XSingletonFactory  
{
public:
	virtual ~XSingletonFactory();
	static XSingletonFactory * Instance();

	// ����һ�ַ�ʽ���ã��ַ�����ʽ
	IWSAccessor*	CreateWSAccessorObject(LPCTSTR sEndPointURL, LPCTSTR sNameSpace, LPCTSTR sRpcUri);
	IWSParam*		CreateWSParamObject();

	//����һ�ַ�ʽ����,��variant
	IWSAccessor*	CreateVarWSAccessorObject();
	IWSParam*		CreateVarWSParamObject();

private:
	XSingletonFactory();

	void Init();
private:
	static XSingletonFactory * m_lpInstance;
};

#endif//AFX_WSACCESSOR_EXPORT_H__INCLUDED_ 

/************************************************************************/
/* �����ǵ�������

// ������Ӧͷ�ļ�
#include "WsAccessor_export.h"

// ����WsAccessor.lib
#pragma comment(lib, "WsAccessor")

void CallSoap(void)
{
	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();

	if( pWsAccessor )		
	{

		HRESULT hr = pWsAccessor->Connect("http://localhost/WebApplication3/WebService1.asmx?WSDL");
		
		TCHAR lpMsg[512];


		// ������ӳ���
		if( hr != S_OK )
		{
			pWsAccessor->Answer(lpMsg);

			MessageBox(0, lpMsg ,"",  0);

			pWsAccessor->Release();

			return;
		}

		
		// ע�⣬�������鳤��һ��Ҫ������һЩ
		_variant_t varParams[5];
		varParams[0] = (long)1111;
		varParams[1] = "2222";
		varParams[2] = "���ͷ�";
		varParams[3] = "ddd";

		_variant_t varResult;

		// ���÷���
		hr = pWsAccessor->CallMethod("HelloWorld",varParams, &varResult);

		// ���������ó�����Ϣ
		if (hr != S_OK )
		{
			pWsAccessor->Answer(lpMsg);

			MessageBox(0, lpMsg ,"",  0);

			//ע���ͷ�
			pWsAccessor->Release();

			return;
		}

		// �ɹ�
		MessageBox(0, (_bstr_t)varResult,"", 0);

		//ע���ͷ�
		pWsAccessor->Release();
	}
}

/************************************************************************/

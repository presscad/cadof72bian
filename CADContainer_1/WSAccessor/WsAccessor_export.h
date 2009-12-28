/************************************************************************/
/* 调用webservice相关访问接口                                               */
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
//调用web服务中方法参数类
//////////////////////////////////////////////////////////////////////////
struct IWSParam 
{
	/// 增加一个参数，参数明，参数值
	virtual void Add(LPCTSTR szName,  LPCTSTR szValue)=0;

	/// 增加参数，参数值压栈
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
// WebService的访问器
// 通过XSingletonFactory::Instance()->CreateVarWSAccessorObject创建
//////////////////////////////////////////////////////////////////////////

struct IWSAccessor 
{
	//////////////////////////////////////////////////////////////////////////	
	// 连接webService, szWsdlUrl--WebService的WSDL路径
	//////////////////////////////////////////////////////////////////////////	
	virtual HRESULT Connect(LPCTSTR szWsdlUrl)=0;

	// 调用webservice的方法
	// szMethod 方法名，lpParams参数数组
	virtual HRESULT Access(LPCTSTR szMethod, IWSParam * lpParams, LPCTSTR szSoapAction, LPCTSTR szRpcURI)=0;

	// 调用webservice的方法
	// szMethod 方法名，lpParams参数数组, varResult返回方法值
	virtual HRESULT CallMethod(LPCSTR szMethod, IWSParam * lpParams, _variant_t* szResult)=0;

	// 调用webservice的方法
	virtual HRESULT CallMethod(LPCSTR szMethod, _variant_t* szResult,...)=0;

	// 调用webservice的方法
	// szMethod 方法名，varParams参数数组, varResult返回方法值
	virtual HRESULT CallMethod(LPCSTR szMethod, _variant_t* varParams, _variant_t* varResult)=0;

	// 获得webservice的方法返回值
	virtual HRESULT Answer(LPTSTR szResult)=0;	

	// 接口释放
	virtual HRESULT Release(void)=0;
};

//////////////////////////////////////////////////////////////////////////
// 创建web服务的工厂类
//////////////////////////////////////////////////////////////////////////
class WSACCESSOR_API XSingletonFactory  
{
public:
	virtual ~XSingletonFactory();
	static XSingletonFactory * Instance();

	// 这是一种方式调用，字符串方式
	IWSAccessor*	CreateWSAccessorObject(LPCTSTR sEndPointURL, LPCTSTR sNameSpace, LPCTSTR sRpcUri);
	IWSParam*		CreateWSParamObject();

	//这是一种方式调用,用variant
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
/* 下面是调用例子

// 包含相应头文件
#include "WsAccessor_export.h"

// 连接WsAccessor.lib
#pragma comment(lib, "WsAccessor")

void CallSoap(void)
{
	IWSAccessor* pWsAccessor = XSingletonFactory::Instance()->CreateVarWSAccessorObject();

	if( pWsAccessor )		
	{

		HRESULT hr = pWsAccessor->Connect("http://localhost/WebApplication3/WebService1.asmx?WSDL");
		
		TCHAR lpMsg[512];


		// 如果连接出错！
		if( hr != S_OK )
		{
			pWsAccessor->Answer(lpMsg);

			MessageBox(0, lpMsg ,"",  0);

			pWsAccessor->Release();

			return;
		}

		
		// 注意，参数数组长度一定要多声明一些
		_variant_t varParams[5];
		varParams[0] = (long)1111;
		varParams[1] = "2222";
		varParams[2] = "都释放";
		varParams[3] = "ddd";

		_variant_t varResult;

		// 调用方法
		hr = pWsAccessor->CallMethod("HelloWorld",varParams, &varResult);

		// 如果出错，获得出错信息
		if (hr != S_OK )
		{
			pWsAccessor->Answer(lpMsg);

			MessageBox(0, lpMsg ,"",  0);

			//注意释放
			pWsAccessor->Release();

			return;
		}

		// 成功
		MessageBox(0, (_bstr_t)varResult,"", 0);

		//注意释放
		pWsAccessor->Release();
	}
}

/************************************************************************/

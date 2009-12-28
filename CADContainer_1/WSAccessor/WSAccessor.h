// WSAccessor.h: interface for the WSAccessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WSACCESSOR_H__86DF4E5D_585C_4374_9648_6062A948977B__INCLUDED_)
#define AFX_WSACCESSOR_H__86DF4E5D_585C_4374_9648_6062A948977B__INCLUDED_

#include "WsAccessor_export.h"

#import "msxml4.dll" 
using namespace MSXML2;

#import "mssoap30.dll" \
            exclude("IStream", "IErrorInfo", "ISequentialStream", "_LARGE_INTEGER", \
                    "_ULARGE_INTEGER", "tagSTATSTG", "_FILETIME")
using namespace MSSOAPLib30;


#pragma warning(disable:4786)

#include <string>
#include <vector>
using namespace std;

class WSACCESSOR_API WSParam : public IWSParam
{
public:
	WSParam();
	WSParam(int nSize);
	virtual ~WSParam();

	virtual void Add(LPCTSTR sName, LPCTSTR sValue);
	virtual void Add(_variant_t& sValue);

	virtual void Clear();
	virtual int	 GetSize();
	virtual void GetName(int idx, LPTSTR szName);
	virtual void GetValue(int idx, LPTSTR szValue);

	virtual _variant_t& GetValue(int idx);
	virtual void Release(){ delete this;}
	
protected:
	vector<string> _vecName;
	vector<string> _vecValue;

	_variant_t*	m_varParams;
	long	m_nCount, m_nSize;
};

class WSACCESSOR_API WSAccessor : public IWSAccessor
{
public:
	WSAccessor();
	WSAccessor(LPCTSTR sEndPointURL, LPCTSTR sNameSpace, LPCTSTR sRpcUri);

	virtual ~WSAccessor();

	virtual HRESULT Connect(LPCTSTR szWsdlUrl);

	virtual HRESULT Access(LPCTSTR sMethod, IWSParam* lpParams, LPCTSTR  sSoapAction, LPCTSTR sRpcURI);

	virtual HRESULT CallMethod(LPCTSTR szMethod, IWSParam* lpParams, _variant_t* szResult);

	virtual HRESULT CallMethod(LPCSTR szMethod, _variant_t* szResult,...);

	virtual HRESULT CallMethod(LPCSTR szMethod, _variant_t* varParams, _variant_t* varResult);

	virtual HRESULT Answer(LPTSTR szResult);

	virtual HRESULT	Release(){delete this; return 0;}
	
protected:
	/// WebService 服务地址
	string	_sEndPointURL;
	/// 命名空间（默认值设定为WebService地址）
	string	_sNameSpace;
	/// 
	string	_sRpcURI;
	
	/// 与WebService交互一次获得到应答消息，如果发生错误，则是错误提示内容
	string	_sAnswer;	

	ISoapConnectorPtr m_pSoapConnector;

	BOOL	m_bConnected;
	ISoapClient* m_pSoapClient;

	void initialize(void);
	HRESULT AutoDispInvoke(LPDISPATCH pdisp, LPOLESTR pwszname, DISPID dspid,
					WORD wflags, DWORD cargs, VARIANT* rgargs, VARIANT* pvtret);
};

#endif // !defined(AFX_WSACCESSOR_H__86DF4E5D_585C_4374_9648_6062A948977B__INCLUDED_)

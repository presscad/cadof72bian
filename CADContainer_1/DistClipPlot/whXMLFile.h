

// XMLFile.h v1.0
// Purpose: interface for the CXMLFile class.
// by hujinshan @2004-11-9 10:15:58

#ifndef _XMLFILE_H_
#define _XMLFILE_H_

#pragma warning(disable: 4786)

//#import "C:/WINDOWS/SYSTEM32/msxml4.dll" named_guids raw_interfaces_only
//#import <msxml4.dll>
#import <msxml3.dll>

#include <atlbase.h>
#include <string>
#include <map>
#include <vector>

//typedef const char* LPCTSTR;

//!  XMLFile class ��дXML�ļ� 
/*!
  ����������Ӧ�ó������á�
*/

class CXMLFile
{
/// Construction
public:
	CXMLFile();
	CXMLFile(const char* filename);
	
	virtual ~CXMLFile();

/// Implementation
public:
	int GetData(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nDefault);
	double GetData(LPCTSTR lpAppName, LPCTSTR lpKeyName,double dDefault);
	CString GetData(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString strDefault);
	
	BOOL GetData(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nDefault, int &nReturn);
	BOOL GetData(LPCTSTR lpAppName, LPCTSTR lpKeyName, double dDefault, double &dReturn);
	BOOL GetData(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString strDefault, CString& strReturne);	
	//---------------------------------------------------------------------------	
	BOOL SetData(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nData);
	BOOL SetData(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString);
	BOOL SetData(LPCTSTR lpAppName, LPCTSTR lpKeyName, double dData);

	///�õ��ڵ�ֵ(������)
	///����XML�ļ�
	/** 
		\param cstrBaseKeyName ������.
		\param cstrValueName ����(ȡֵ��).
		\param lDefaultValue Ĭ�ϳ���ֵ.
	*/
	long GetLong(const char* cstrBaseKeyName, const char* cstrValueName, long lDefaultValue);
	///���ýڵ�ֵ(������)
	/** 
		\param cstrBaseKeyName ������.
		\param cstrValueName ����(ȡ����ֵ��).
		\param lDefaultValue Ĭ�ϳ���ֵ.
	*/
	long SetLong(const char* cstrBaseKeyName, const char* cstrValueName, long lValue);

	///�õ��ڵ�ֵ(�ַ���)
	/** 
		\param cstrBaseKeyName ������.
		\param cstrValueName ����(ȡֵ��).
		\param cstrDefaultValue Ĭ���ַ���ֵ.
	*/
	std::string GetString(const char* cstrBaseKeyName, const char* cstrValueName, const char* cstrDefaultValue);
	///���ýڵ�ֵ(�ַ���)
	/** 
		\param cstrBaseKeyName ������.
		\param cstrValueName ����(����ֵ��).
		\param cstrDefaultValue Ĭ���ַ���ֵ.
	*/
	long SetString(const char* cstrBaseKeyName, const char* cstrValueName, const char* cstrValue);
	
	///�õ��ڵ�����
	/** 
		\param cstrBaseKeyName ������.
		\param cstrValueName ����(�������Լ���).
		\param cstrAttributeName ������(��������ֵ��).
		\param cstrDefaultAttributeValue Ĭ������ֵ.
	*/
	std::string GetAttribute(const char* cstrBaseKeyName, const char* cstrValueName, 
		const char* cstrAttributeName, const char* cstrDefaultAttributeValue);
	///���ýڵ�����
	long SetAttribute(const char* cstrBaseKeyName, const char* cstrValueName,
					const char* cstrAttributeName = NULL, const char* cstrAttributeValue = NULL);

	///�õ��ڵ�ֵ
	long GetNodeValue(const char* cstrBaseKeyName, const char* cstrValueName, 
		const char* cstrDefaultValue, std::string& strValue, const char* cstrAttributeName, 
		const char* cstrDefaultAttributeValue,std::string& strAttributeValue);
	
	///���ýڵ�ֵ
	long SetNodeValue(const char* cstrBaseKeyName, const char* cstrValueName, 
		const char* cstrValue=NULL, const char* cstrAttributeName=NULL,
		const char* cstrAttributeValue=NULL);

	///ɾ��ĳ�ڵ���������ӽڵ�
	/*!
      �����ӽڵ�ļ�ֵ���浽����keys_val��.
    */
	long DeleteSetting(const char* cstrBaseKeyName, const char* cstrValueName);

	///�õ�ĳ�ڵ���ӽڵ�ļ���
	/*!
      �����ӽڵ�ļ������浽����keys_val��.
    */
	long GetKeysValue(CString i_sBaseKey, CStringArray &o_arsChildKeys);

	long GetKeysValue(const char* cstrBaseKeyName, 
		std::map<std::string, std::string>& keys_val);

	///�õ�ĳ�ڵ���ӽڵ�ļ���
	long GetKeys(const char* cstrBaseKeyName, 
		std::vector<std::string>& keys);

	///����XML�ļ�
	/** 
		\param filename �����ļ���.
	*/
	bool save(const char* filename=NULL);
	
	///װ��XML�ļ�
	/** 
		\param filename װ���ļ���.
	*/
	//0 - û�а�װxml�� 1 - �����ļ�����xml�� 2 - �ɹ���load xml�ļ�
	int load(const char* filename, const char* root_name="xmlRoot");

	///������ı�
	void DiscardChanges();
	///�������
	void clear();

	//------------------------------------------------------------------------------------
	long GetRootElem(MSXML2::IXMLDOMElementPtr rootElem);
	long GetNode(const char* cstrKeyName, MSXML2::IXMLDOMNodePtr& foundNode);

protected:

	MSXML2::IXMLDOMDocument2Ptr XmlDocPtr;

	std::string xml_file_name, m_root_name;

	std::string* ParseKeys(const char* cstrFullKeyPath, int &iNumKeys);	

	MSXML2::IXMLDOMNodePtr FindNode(MSXML2::IXMLDOMNodePtr parentNode, std::string* pCStrKeys, int iNumKeys, bool bAddNodes = FALSE);

};

#endif // _XMLFILE_H_

// end of file 



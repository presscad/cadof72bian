//////////////////////////////////////////////////////////////////////////
/// 


#include "stdafx.h"
#include "XMLData.h"
#include "tinyxml.h"

#include <string>

using namespace std;

// ���ܣ��������ݿ����Ӷ���
// ��������
// ���أ����ض���ָ��
IDistConnectionXML* WINAPI CreateConnectObjcet(void)
{
	return (IDistConnectionXML*) new XMLDataConnection;
}

//////////////////////////////////////////////////////////////////////////
/// XMLDataConnection

XMLDataConnection::XMLDataConnection()
{
	_lpXmlDoc = NULL;
}

XMLDataConnection::~XMLDataConnection()
{
	if (NULL != _lpXmlDoc)
	{
		delete _lpXmlDoc;
	}
}

LONG XMLDataConnection::Connect(const char* strFile)
{
	Disconnect();

	_lpXmlDoc = new TiXmlDocument();
	if (!_lpXmlDoc->LoadFile(strFile))
	{
		delete _lpXmlDoc;
		_lpXmlDoc = NULL;
		return -1;
	}

	return 0;
}

LONG XMLDataConnection::Connect(const char* strFile, const char* strContent, 
								const char* lpReserved1, const char* lpReserved2, const char* lpReserved3)
{
	/// ���û���ṩ�ļ����ƣ��ⰴ����װ��
	if (NULL == strFile || 0 == strFile[0])
	{
		Disconnect();
		
		_lpXmlDoc = new TiXmlDocument();
		_lpXmlDoc->Parse(strContent);
		if (_lpXmlDoc->Error())
		{
			delete _lpXmlDoc;
			_lpXmlDoc = NULL;
			return -1;
		}
		
		return 0;
	}
	else
	{
		return Connect(strFile);
	}
}

void XMLDataConnection::Disconnect(void)
{
	if (NULL != _lpXmlDoc)
	{
		delete _lpXmlDoc;
		_lpXmlDoc = NULL;
	}
}

BOOL XMLDataConnection::CheckConnect(int)
{
	return NULL != _lpXmlDoc; 
}

void XMLDataConnection::Release(void)
{
	delete this;
}

void XMLDataConnection::QueryCommand(IDistCommandXML** lplpCommand)
{
	*lplpCommand = new XMLDataCommand(this);
}


//////////////////////////////////////////////////////////////////////////
/// XMLDataCommand

XMLDataCommand::XMLDataCommand(XMLDataConnection* lpConn)
{
	_lpXmlDataConn = lpConn;
}

XMLDataCommand::~XMLDataCommand()
{
}

LONG XMLDataCommand::SelectData(const char* strXmlPath, IDistRecordSetXML** lplpRcdSet, void*)
{
	if (NULL == _lpXmlDataConn || !_lpXmlDataConn->CheckConnect(0))
	{
		return -1;
	}

	/// ȡ��·��ָ���Ľڵ���
	int pos = 0;
	string nodeName, lastName;
	int idx = 0;
	TiXmlNode* lpNode = _lpXmlDataConn->_lpXmlDoc;
	while (0 != strXmlPath[pos])
	{
		/// ����ҵ��ڵ�ֶ�
		if ('/' == strXmlPath[pos] || '\\' == strXmlPath[pos])
		{
			if (!nodeName.empty())
			{
				lpNode = lpNode->FirstChild(nodeName.c_str());
				if (NULL == lpNode)
				{
					*lplpRcdSet = NULL;
					return -1;
				}

				lastName = nodeName;
				nodeName = "";
			}
		}
		else
		{
			nodeName += strXmlPath[pos];
		}

		pos++;
	}
	if (!nodeName.empty())
	{
		lpNode = lpNode->FirstChild(nodeName.c_str());
		if (NULL == lpNode)
		{					
			*lplpRcdSet = NULL;
			return -1;
		}
		lastName = nodeName;
	}

	/// �������ݼ�
	TiXmlElement* lpEle = lpNode->ToElement();
	if (NULL == lpEle)
	{
		*lplpRcdSet = NULL;
		return -1;
	}

	XMLRecordSet* lpXmlRst = new XMLRecordSet(lpEle, lastName.c_str());
	*lplpRcdSet = lpXmlRst;
	
	return 0;
}

void XMLDataCommand::Release(void)
{
	delete this;
}


//////////////////////////////////////////////////////////////////////////
/// XMLRecordSet

XMLRecordSet::XMLRecordSet(TiXmlElement* lpEle, const char* rowName)
{
	_lpFirstRow = lpEle;
	_lpCurRow = lpEle;

	strcpy(_rowName, rowName);
}

XMLRecordSet::~XMLRecordSet()
{
}

/// ���ֶ����Ʒ���ֵ
void* XMLRecordSet::operator[] (const char* strFldName)
{
	if (NULL != _lpCurRow)
	{
		TiXmlElement* lpEle = _lpCurRow->FirstChildElement(strFldName);
		if (NULL != lpEle)
		{
			return (void*)(lpEle->GetText());
		}
	}

	return NULL;
}

void* XMLRecordSet::operator[] (int nIndex)
{
	if (NULL != _lpCurRow && nIndex >= 0)
	{
		int idx = 0;
		TiXmlElement* lpEle = _lpCurRow->FirstChildElement();
		while (NULL != lpEle && idx < nIndex)
		{
			lpEle = lpEle->NextSiblingElement();
			idx++;
		}

		if (NULL != lpEle)
		{
			return (void*)(lpEle->GetText());
		}
	}
	
	
	return NULL;
}

BOOL XMLRecordSet::BatchRead(void)
{
	return TRUE;
}

/// �Ƶ���һ��
BOOL XMLRecordSet::MoveFirst(void)
{
	_lpCurRow = _lpFirstRow;

	return (NULL != _lpCurRow);
}

/// �Ƶ����һ��
BOOL XMLRecordSet::MoveLast(void)
{
	if (NULL == _lpCurRow)
	{
		return FALSE;
	}
	
	TiXmlElement* lpNext = _lpCurRow->NextSiblingElement(_rowName);
	while (NULL != lpNext)
	{
		_lpCurRow = lpNext;
		lpNext = _lpCurRow->NextSiblingElement(_rowName);
	}

	return TRUE;
}

/// �Ƶ���һ��
BOOL XMLRecordSet::MoveNext(void)
{
	if (NULL == _lpCurRow)
	{
		return FALSE;
	}

	TiXmlElement* lpNext = _lpCurRow->NextSiblingElement(_rowName);
	if (NULL != lpNext)
	{
		_lpCurRow = lpNext;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/// �Ƶ�ǰһ��
BOOL XMLRecordSet::MovePrevious(void)
{
	if (NULL == _lpCurRow)
	{
		return FALSE;
	}

	TiXmlNode* lpNode = _lpCurRow->PreviousSibling(_rowName);
	while (NULL != lpNode)
	{
		TiXmlElement* lpEle = lpNode->ToElement();
		if (NULL != lpEle)
		{
			_lpCurRow = lpEle;
			return TRUE;
		}

		lpNode = lpNode->PreviousSibling(_rowName);
	}
	
	return FALSE;
}

/// �Ƶ�ָ������
BOOL XMLRecordSet::MoveTo(int nIndex)
{
	if (NULL == _lpFirstRow || nIndex < 0)
	{
		return FALSE;
	}

	int idx = 0;
	TiXmlElement* lpEle = _lpFirstRow;
	while (NULL != lpEle && idx < nIndex)
	{
		lpEle = lpEle->NextSiblingElement(_rowName);
		idx++;
	}

	if (NULL != lpEle)
	{
		_lpCurRow = lpEle;
		return TRUE;
	}
	
	return FALSE;
}

/// ��������
int XMLRecordSet::GetRecordCount(void)
{
	int nRowCnt = 0;
	TiXmlElement* lpTmp = _lpFirstRow;
	while (NULL != lpTmp)
	{
		nRowCnt++;
		
		lpTmp = lpTmp->NextSiblingElement(_rowName);
	}
	
	return nRowCnt;
}

/// �����ֶ���
int XMLRecordSet::GetFieldNum(void)
{
	if (NULL == _lpCurRow)
	{
		return -1;
	}

	int nFldCnt = 0;
	TiXmlElement* lpTmp = _lpCurRow->FirstChildElement();
	while (NULL != lpTmp)
	{
		nFldCnt++;
		
		lpTmp = lpTmp->NextSiblingElement();
	}
	
	return nFldCnt;
}

//�ͷŶ���
void XMLRecordSet::Release(void)
{
	delete this;
}

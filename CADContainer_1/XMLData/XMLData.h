//////////////////////////////////////////////////////////////////////////
/// 


#ifndef __XML_DATA__ACCESSOR_H__
#define __XML_DATA__ACCESSOR_H__

#include "DistBaseInterface.h"

class TiXmlDocument;
class TiXmlElement;

/// 对XML文件进行数据访问的连接器
class XMLDataConnection : public IDistConnectionXML
{
	friend class XMLDataCommand;

public:
	XMLDataConnection();
	~XMLDataConnection();

	/// 连接XML文件
	LONG Connect(const char* strFile);
	LONG Connect(const char* strFile, const char* strContent, 
		const char* lpReserved1 = NULL, const char* lpReserved2 = NULL, const char* lpReserved3 = NULL);
	
	/// 断开与XML文件的连接
	void Disconnect(void);

	BOOL CheckConnect(int);

	void Release(void);

	void QueryCommand(IDistCommandXML** lplpCommand);

protected:
	void GetParameter(PARAMETERTYPE, LPSTR) {}
	void SetParameter(PARAMETERTYPE, LPSTR) {}
	
	TiXmlDocument* _lpXmlDoc;
};



//////////////////////////////////////////////////////////////////////////
/// 
class XMLDataCommand : public IDistCommandXML
{
	friend class XMLDataConnection;
protected:
	XMLDataCommand(XMLDataConnection* lpConn);
public:
	~XMLDataCommand();

	//记录数据相关操作(可能需要重载)
	LONG SelectData(const char*, IDistRecordSetXML**, void*);

	//释放对象
	void Release(void);
	
protected:
	LONG InsertData(const char*, ...) { return -1; }
	LONG UpdateData(const char*, void*, ...) { return -1; }
	LONG DeleteData(const char*) { return -1; }
	
	LONG GetTableInfo(const char* ,IDistParameterXML**, int&) { return -1; }
	LONG CreateTable(const char*, IDistParameterXML**, int, const char*) { return -1; }
	LONG DropTable(const char*) { return -1; }
	
	LONG GetLayerInfo(const char*, const char*, void**) { return -1; }
	LONG CreateLayer(const char*, const char*, double, double, double, double, LONG, const char*) { return -1; }
	LONG DropLayer(const char*, const char*) { return -1; }
	
	LONG LockLayerData(const char* strLyName) { return -1; }
	LONG LockTable(const char* strName) { return -1; }
	LONG LockTableARow(const char* strName, int nIndexRow) { return -1; }
	LONG LockApplicationObj(int* pObjs) { return -1; }
	
	LONG CreateCoordinate(void) { return -1; }
	LONG ReadRasterData(void) { return -1; }
	LONG WriteRasterData(void) { return -1; }

	XMLDataConnection* _lpXmlDataConn;
};


//////////////////////////////////////////////////////////////////////////
/// 
class XMLRecordSet : public IDistRecordSetXML
{
	friend class XMLDataCommand;
protected:
	XMLRecordSet(TiXmlElement* lpEle, const char* rowName);

public:
	~XMLRecordSet();


	void* operator[] (const char*);
	void* operator[] (int);

	BOOL BatchRead(void);
	
	BOOL MoveFirst(void);
	BOOL MoveLast(void);
	BOOL MoveNext(void);
	BOOL MovePrevious(void);
	BOOL MoveTo(int nIndex);
	int  GetRecordCount(void);
	int  GetFieldNum(void);
	
	//释放对象
	void Release(void);

protected:
	TiXmlElement* _lpFirstRow;
	TiXmlElement* _lpCurRow;

	char _rowName[256];
};

#endif // __XML_DATA__ACCESSOR_H__
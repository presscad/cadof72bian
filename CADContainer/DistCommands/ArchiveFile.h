// ArchiveFile.h  --- 存储到文件的文档序列化对象
///////////////////////////////////////////////////////////////////////

#ifndef  AFX_ARCHIVE_FILE_H__INCLUDED_
#define  AFX_ARCHIVE_FILE_H__INCLUDED_

// 文档序列化专用对象

class XArchiveFile 
{
public: 
	XArchiveFile(HANDLE hFile, BOOL bWrite);
	virtual ~XArchiveFile();

protected: 
	// 文件句柄
	HANDLE   m_hFile;

	// 存储还是获取
	BOOL     m_bWrite;

	// 访问缓冲区指针
	int      m_nIndex;
	unsigned char  m_lpBuffer[4096];

protected: 
	// 写入一定长度
	void  SaveData(const void * lpData, UINT nSize);

	// 读取一定长度
	void  LoadData(void * lpData, UINT nBuffer, UINT nSize);

public: 
	// 关闭当前文档
	void  Close();

	// 文件绝对定位
	void  Seek(long nMove, DWORD dwMode = FILE_BEGIN);

	int   GetCurPos();

public: 
	// 几种数据的序列化
	XArchiveFile& operator << (WORD  wdData);
	XArchiveFile& operator >> (WORD& wdData);

	XArchiveFile& operator << (long  lgData);
	XArchiveFile& operator >> (long& lgData);

	XArchiveFile& operator << (short  shData);
	XArchiveFile& operator >> (short& shData);

	XArchiveFile& operator << (DWORD  dwData);
	XArchiveFile& operator >> (DWORD& dwData);

	XArchiveFile& operator << (float  ftData);
	XArchiveFile& operator >> (float& ftData);

	XArchiveFile& operator << (double  dbData);
	XArchiveFile& operator >> (double& dbData);

	// 特殊数据的序列化
	void  WriteString(LPCTSTR lpString);
	void  ReadString(LPTSTR lpBuffer, UINT nMaxSize);

	void  WriteBuffer(const void * lpData, UINT nSize);
	UINT  ReadBuffer(void * lpData, UINT nMaxSize);

};


#endif // AFX_ARCHIVE_FILE_H__INCLUDED_

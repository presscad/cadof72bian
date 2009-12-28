// ArchiveFile.h  --- �洢���ļ����ĵ����л�����
///////////////////////////////////////////////////////////////////////

#ifndef  AFX_ARCHIVE_FILE_H__INCLUDED_
#define  AFX_ARCHIVE_FILE_H__INCLUDED_

// �ĵ����л�ר�ö���

class XArchiveFile 
{
public: 
	XArchiveFile(HANDLE hFile, BOOL bWrite);
	virtual ~XArchiveFile();

protected: 
	// �ļ����
	HANDLE   m_hFile;

	// �洢���ǻ�ȡ
	BOOL     m_bWrite;

	// ���ʻ�����ָ��
	int      m_nIndex;
	unsigned char  m_lpBuffer[4096];

protected: 
	// д��һ������
	void  SaveData(const void * lpData, UINT nSize);

	// ��ȡһ������
	void  LoadData(void * lpData, UINT nBuffer, UINT nSize);

public: 
	// �رյ�ǰ�ĵ�
	void  Close();

	// �ļ����Զ�λ
	void  Seek(long nMove, DWORD dwMode = FILE_BEGIN);

	int   GetCurPos();

public: 
	// �������ݵ����л�
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

	// �������ݵ����л�
	void  WriteString(LPCTSTR lpString);
	void  ReadString(LPTSTR lpBuffer, UINT nMaxSize);

	void  WriteBuffer(const void * lpData, UINT nSize);
	UINT  ReadBuffer(void * lpData, UINT nMaxSize);

};


#endif // AFX_ARCHIVE_FILE_H__INCLUDED_

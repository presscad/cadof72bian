// ArchiveFile.cpp : implementation of the XArchiveFile class
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ArchiveFile.h"

//////////////////////////////////////////////////////////////////////

XArchiveFile::XArchiveFile(HANDLE hFile, BOOL bWrite)
{
	// 文件句柄
	m_hFile = hFile;

	// 存储还是获取
	m_bWrite = bWrite;

	// 缓冲数据指针
	if( bWrite )  m_nIndex = 0;
	else       m_nIndex = 4096;
}

XArchiveFile::~XArchiveFile()
{
	// 如果还没有关闭
	Close();  m_hFile = NULL;
}

// 关闭并写入缓冲区
void XArchiveFile::Close()
{
	if(m_bWrite && m_nIndex > 0)
	{	
		DWORD  dwSize = 0;
		WriteFile(m_hFile, m_lpBuffer, m_nIndex, &dwSize, NULL);

		// 缓冲区清空
		m_nIndex = 0;
	}
}

// 在文件上绝对定位
void XArchiveFile::Seek(long nMove, DWORD dwMode)
{
	if( m_bWrite )
	{
		// 缓冲区中数据
		if(m_nIndex > 0)
		{
			DWORD  dwSize = 0;
			WriteFile(m_hFile, m_lpBuffer, m_nIndex, &dwSize, NULL);
		}

		// 缓冲区清空
		m_nIndex = 0;
		SetFilePointer(m_hFile, nMove, NULL, dwMode);
	}
	else 
	{
		if(dwMode == FILE_CURRENT)
		{
			// 相对当前读的位置
			nMove = m_nIndex + nMove - 4096;
		}

		// 缓冲区清空
		m_nIndex = 4096;
		SetFilePointer(m_hFile, nMove, NULL, dwMode);
	}
}

// 写入一定长度
void XArchiveFile::SaveData(const void * lpData, UINT nSize)
{
	if(nSize == 0)  return;

	// 写入一定长度
	if(m_nIndex + nSize > 4096)
	{
		UINT  nCount, nIdx = 0;
		const char * lpSrc = (const char *)lpData;

		// 拷贝所有的数据
		while(nIdx < nSize)
		{
			// 数据大小
			nCount = 4096 - m_nIndex;
			if(nCount > nSize - nIdx)  nCount = nSize - nIdx;

			// 拷贝数据
			if(nCount > 0) 
			memcpy(m_lpBuffer + m_nIndex, lpSrc + nIdx, nCount);

			// 位置相加
			nIdx += nCount;  m_nIndex += nCount;

			// 写入文件
			if(m_nIndex >= 4096)
			{
				DWORD  dwSize = 0;  m_nIndex = 0;
				WriteFile(m_hFile, m_lpBuffer, 4096, &dwSize, NULL);
			}
		}
	}
	else 
	{
		memcpy(m_lpBuffer + m_nIndex, lpData, nSize);
		m_nIndex += nSize;
	}
}

// 读取一定长度
void XArchiveFile::LoadData(void * lpData, UINT nBuffer, UINT nSize)
{
	// 拷贝数据大小
	if(nSize == 0)  return;
	if(nBuffer > nSize)  nBuffer = nSize;

	// 需要装载数据
	if(m_nIndex + nSize > 4096)
	{
		UINT  nCount, nIdx = 0;
		char  * lpSrc = (char *)lpData;

		// 读取所有的数据
		while(nIdx < nSize)
		{
			// 数据大小
			nCount = 4096 - m_nIndex;
			if(nCount > nSize - nIdx)  nCount = nSize - nIdx;

			// 拷贝数据
			if(nCount > 0) 
			memcpy(lpSrc + nIdx, m_lpBuffer + m_nIndex, nCount);

			// 位置相加
			nIdx += nCount;  m_nIndex += nCount;

			// 读取文件
			if(m_nIndex >= 4096)
			{
				DWORD  dwSize = 0;
				memset(m_lpBuffer, 0x00, 4096);
				ReadFile(m_hFile, m_lpBuffer, 4096, &dwSize, NULL);

				// 从头开始
				m_nIndex = 0;
			}
		}
	}
	else 
	{
		memcpy(lpData, m_lpBuffer + m_nIndex, nBuffer);
		m_nIndex += nSize;
	}
}

// 几种数据的序列化
XArchiveFile& XArchiveFile::operator << (WORD  wdData)
{
	SaveData(&wdData, sizeof(WORD));
	return *( this );
}

XArchiveFile& XArchiveFile::operator >> (WORD& wdData)
{
	LoadData(&wdData, sizeof(WORD), sizeof(WORD));
	return *( this );
}

XArchiveFile& XArchiveFile::operator << (long  lgData)
{
	SaveData(&lgData, sizeof(long));
	return *( this );
}

XArchiveFile& XArchiveFile::operator >> (long& lgData)
{
	LoadData(&lgData, sizeof(long), sizeof(long));
	return *( this );
}

XArchiveFile& XArchiveFile::operator << (short  shData)
{
	SaveData(&shData, sizeof(short));
	return *( this );
}

XArchiveFile& XArchiveFile::operator >> (short& shData)
{
	LoadData(&shData, sizeof(short), sizeof(short));
	return *( this );
}

XArchiveFile& XArchiveFile::operator << (DWORD  dwData)
{
	SaveData(&dwData, sizeof(DWORD));
	return *( this );
}

XArchiveFile& XArchiveFile::operator >> (DWORD& dwData)
{
	LoadData(&dwData, sizeof(DWORD), sizeof(DWORD));
	return *( this );
}

XArchiveFile& XArchiveFile::operator << (float  ftData)
{
	SaveData(&ftData, sizeof(float));
	return *( this );
}

XArchiveFile& XArchiveFile::operator >> (float& ftData)
{
	LoadData(&ftData, sizeof(float), sizeof(float));
	return *( this );
}

XArchiveFile& XArchiveFile::operator << (double  dbData)
{
	SaveData(&dbData, sizeof(double));
	return *( this );
}

XArchiveFile& XArchiveFile::operator >> (double& dbData)
{
	LoadData(&dbData, sizeof(double), sizeof(double));
	return *( this );
}

// 特殊数据的序列化
void XArchiveFile::WriteString(LPCTSTR lpString)
{
	UINT  nSize = lstrlen(lpString);

	// 保存数据大小
	nSize *= sizeof(TCHAR);
	SaveData(&nSize, sizeof(UINT));

	// 保存实际数据
	SaveData(lpString, nSize);
}

void XArchiveFile::ReadString(LPTSTR lpBuffer, UINT nMaxSize)
{
	// 数据的大小
	UINT  nSize = 0;
	LoadData(&nSize, sizeof(UINT), sizeof(UINT));

	// 读取实际数据
	nMaxSize *= sizeof(TCHAR);
	LoadData(lpBuffer, nMaxSize, nSize);

	// 使字符串结束
	nSize /= sizeof(TCHAR);
	nMaxSize /= sizeof(TCHAR);

	if(nSize < nMaxSize)  lpBuffer[nSize] = 0x00;
	else     lpBuffer[nMaxSize - 1] = 0x00;
}

void XArchiveFile::WriteBuffer(const void * lpData, UINT nSize)
{
	// 存储大小
	SaveData(&nSize, sizeof(UINT));

	// 存储数据
	SaveData(lpData, nSize);
}

UINT XArchiveFile::ReadBuffer(void * lpData, UINT nMaxSize)
{
	// 数据的大小
	UINT  nSize = 0;
	LoadData(&nSize, sizeof(UINT), sizeof(UINT));

	// 读取实际数据
	LoadData(lpData, nMaxSize, nSize);
	return  nSize;
}

int XArchiveFile::GetCurPos()
{
	return m_nIndex;
}

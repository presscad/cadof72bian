// ArchiveFile.cpp : implementation of the XArchiveFile class
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ArchiveFile.h"

//////////////////////////////////////////////////////////////////////

XArchiveFile::XArchiveFile(HANDLE hFile, BOOL bWrite)
{
	// �ļ����
	m_hFile = hFile;

	// �洢���ǻ�ȡ
	m_bWrite = bWrite;

	// ��������ָ��
	if( bWrite )  m_nIndex = 0;
	else       m_nIndex = 4096;
}

XArchiveFile::~XArchiveFile()
{
	// �����û�йر�
	Close();  m_hFile = NULL;
}

// �رղ�д�뻺����
void XArchiveFile::Close()
{
	if(m_bWrite && m_nIndex > 0)
	{	
		DWORD  dwSize = 0;
		WriteFile(m_hFile, m_lpBuffer, m_nIndex, &dwSize, NULL);

		// ���������
		m_nIndex = 0;
	}
}

// ���ļ��Ͼ��Զ�λ
void XArchiveFile::Seek(long nMove, DWORD dwMode)
{
	if( m_bWrite )
	{
		// ������������
		if(m_nIndex > 0)
		{
			DWORD  dwSize = 0;
			WriteFile(m_hFile, m_lpBuffer, m_nIndex, &dwSize, NULL);
		}

		// ���������
		m_nIndex = 0;
		SetFilePointer(m_hFile, nMove, NULL, dwMode);
	}
	else 
	{
		if(dwMode == FILE_CURRENT)
		{
			// ��Ե�ǰ����λ��
			nMove = m_nIndex + nMove - 4096;
		}

		// ���������
		m_nIndex = 4096;
		SetFilePointer(m_hFile, nMove, NULL, dwMode);
	}
}

// д��һ������
void XArchiveFile::SaveData(const void * lpData, UINT nSize)
{
	if(nSize == 0)  return;

	// д��һ������
	if(m_nIndex + nSize > 4096)
	{
		UINT  nCount, nIdx = 0;
		const char * lpSrc = (const char *)lpData;

		// �������е�����
		while(nIdx < nSize)
		{
			// ���ݴ�С
			nCount = 4096 - m_nIndex;
			if(nCount > nSize - nIdx)  nCount = nSize - nIdx;

			// ��������
			if(nCount > 0) 
			memcpy(m_lpBuffer + m_nIndex, lpSrc + nIdx, nCount);

			// λ�����
			nIdx += nCount;  m_nIndex += nCount;

			// д���ļ�
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

// ��ȡһ������
void XArchiveFile::LoadData(void * lpData, UINT nBuffer, UINT nSize)
{
	// �������ݴ�С
	if(nSize == 0)  return;
	if(nBuffer > nSize)  nBuffer = nSize;

	// ��Ҫװ������
	if(m_nIndex + nSize > 4096)
	{
		UINT  nCount, nIdx = 0;
		char  * lpSrc = (char *)lpData;

		// ��ȡ���е�����
		while(nIdx < nSize)
		{
			// ���ݴ�С
			nCount = 4096 - m_nIndex;
			if(nCount > nSize - nIdx)  nCount = nSize - nIdx;

			// ��������
			if(nCount > 0) 
			memcpy(lpSrc + nIdx, m_lpBuffer + m_nIndex, nCount);

			// λ�����
			nIdx += nCount;  m_nIndex += nCount;

			// ��ȡ�ļ�
			if(m_nIndex >= 4096)
			{
				DWORD  dwSize = 0;
				memset(m_lpBuffer, 0x00, 4096);
				ReadFile(m_hFile, m_lpBuffer, 4096, &dwSize, NULL);

				// ��ͷ��ʼ
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

// �������ݵ����л�
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

// �������ݵ����л�
void XArchiveFile::WriteString(LPCTSTR lpString)
{
	UINT  nSize = lstrlen(lpString);

	// �������ݴ�С
	nSize *= sizeof(TCHAR);
	SaveData(&nSize, sizeof(UINT));

	// ����ʵ������
	SaveData(lpString, nSize);
}

void XArchiveFile::ReadString(LPTSTR lpBuffer, UINT nMaxSize)
{
	// ���ݵĴ�С
	UINT  nSize = 0;
	LoadData(&nSize, sizeof(UINT), sizeof(UINT));

	// ��ȡʵ������
	nMaxSize *= sizeof(TCHAR);
	LoadData(lpBuffer, nMaxSize, nSize);

	// ʹ�ַ�������
	nSize /= sizeof(TCHAR);
	nMaxSize /= sizeof(TCHAR);

	if(nSize < nMaxSize)  lpBuffer[nSize] = 0x00;
	else     lpBuffer[nMaxSize - 1] = 0x00;
}

void XArchiveFile::WriteBuffer(const void * lpData, UINT nSize)
{
	// �洢��С
	SaveData(&nSize, sizeof(UINT));

	// �洢����
	SaveData(lpData, nSize);
}

UINT XArchiveFile::ReadBuffer(void * lpData, UINT nMaxSize)
{
	// ���ݵĴ�С
	UINT  nSize = 0;
	LoadData(&nSize, sizeof(UINT), sizeof(UINT));

	// ��ȡʵ������
	LoadData(lpData, nMaxSize, nSize);
	return  nSize;
}

int XArchiveFile::GetCurPos()
{
	return m_nIndex;
}

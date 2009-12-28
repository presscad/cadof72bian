// XdeFile.cpp: implementation of the CXdeFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XdeFile.h"

#include "ArchiveFile.h"
#include "fn_DesEncrypt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXdeFile::CXdeFile()
{

}

CXdeFile::~CXdeFile()
{

}

// === ���ܻ����� ===
void EncryptBuffer(const void * lpSource, void * lpBuffer, int nSize) 
{
	if(nSize > 7) 
	{
		char * xxSource = (char *) lpSource;
		char * xxBuffer = (char *) lpBuffer;
		
		while(nSize > 0) 
		{
			nSize -= 0x0008;
			
			// ��������
			Des_Encrypt(xxSource, xxBuffer, "[O_++_O]");
			
			// λ��ƫ��
			if(nSize > 7) 
			{
				xxSource += 8;  xxBuffer += 8;
			}
			else if(nSize > 0)
			{
				// ������λ
				if(xxBuffer != xxSource) 
				{
					// ������λ
					memcpy(xxBuffer + 8, xxSource + 8, nSize);
				}
				
				// ��λ����
				Des_Encrypt(xxBuffer + nSize, xxBuffer + nSize, "[O_++_O]");
				
				// ��֤����
				nSize = 0x0000;
			}
		}
	}
}

// === ���ܻ����� ===
void DecryptBuffer(const void * lpBuffer, void * lpSource, int nSize) 
{
	if(nSize > 7) 
	{
		char * xxSource = (char *) lpSource;
		char * xxBuffer = (char *) lpBuffer;
		
		while(nSize > 0) 
		{
			nSize -= 0x0008;
			
			// ��λ����
			if(nSize > 0 && nSize < 8) 
			{
				// ����λ��
				Des_Decrypt(xxBuffer + nSize, xxSource + nSize, "[O_++_O]");
				
				// ������λ
				if(xxBuffer != xxSource) 
				{
					// ������λ
					memcpy(xxSource, xxBuffer, nSize);
				}
				
				// ��λ����
				Des_Decrypt(xxSource, xxSource, "[O_++_O]");
				
				// ��֤����
				nSize = 0x0000;
			}
			else 
			{
				// �����ֽ�
				Des_Decrypt(xxBuffer, xxSource, "[O_++_O]");
				
				// λ��ƫ��
				xxSource += 0x0008;  xxBuffer += 0x0008;
			}
		}
	}
}

void CXdeFile::Serialize(CArchive& ar)
{
	if (ar.IsLoading())
	{
		ReadXdeFile((HANDLE)ar.GetFile()->m_hFile);
	}
	else
	{
		WriteXdeFile((HANDLE)ar.GetFile()->m_hFile);
	}
}

BOOL CXdeFile::ReadXdeFile(HANDLE  hFile)
{
	// ��ȡ������������
	XArchiveFile  ar(hFile, FALSE);
	
	TCHAR lpFileHead[128] = {0};
	ar.ReadString(lpFileHead, 128);

	if (strcmpi(lpFileHead, "XDEFILE") != 0)
	{
		ar.Close();
		return FALSE;
	}

	DWORD  dsize = 0;
	TCHAR  lpCipher[128] = {0};
	TCHAR  lpBuffer[128] = {0};

	ar.ReadBuffer(&dsize, 0x0004);
	ar.ReadBuffer(lpCipher, dsize);
	// ����
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strServer = lpBuffer;

	ar.ReadBuffer(&dsize, 0x0004);
	ar.ReadBuffer(lpCipher, dsize);
	// ����
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strService = lpBuffer;

	ar.ReadBuffer(&dsize, 0x0004);
	ar.ReadBuffer(lpCipher, dsize);
	// ����
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strDatabase = lpBuffer;

	ar.ReadBuffer(&dsize, 0x0004);
	ar.ReadBuffer(lpCipher, dsize);
	// ����
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strUserName = lpBuffer;

	ar.ReadBuffer(&dsize, 0x0004);
	ar.ReadBuffer(lpCipher, dsize);
	// ����
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strPassword = lpBuffer;

	ar.Close();

	return TRUE;
}

BOOL CXdeFile::WriteXdeFile(HANDLE  hFile)
{
	// ���뼸����������
	XArchiveFile  ar(hFile, TRUE);
	
	ar.WriteString("XDEFILE");
	
	DWORD  dwSize = (lstrlen(m_xxSdeData.strServer) + 1) * sizeof(TCHAR);
	if(dwSize < 8)  dwSize = 8;

	TCHAR  lpCipher[128] = {0};
	EncryptBuffer(m_xxSdeData.strServer, lpCipher, dwSize);
	
	ar.WriteBuffer(&dwSize, 0x0004);
	ar.WriteBuffer(lpCipher, dwSize);
	
	dwSize = (lstrlen(m_xxSdeData.strService) + 1) * sizeof(TCHAR);
	if(dwSize < 8)  dwSize = 8;
	
	lpCipher[0] = '\0';
	EncryptBuffer(m_xxSdeData.strService, lpCipher, dwSize);
	
	ar.WriteBuffer(&dwSize, 0x0004);
	ar.WriteBuffer(lpCipher, dwSize);

	dwSize = (lstrlen(m_xxSdeData.strDatabase) + 1) * sizeof(TCHAR);
	if(dwSize < 8)  dwSize = 8;
	
	lpCipher[0] = '\0';
	EncryptBuffer(m_xxSdeData.strDatabase, lpCipher, dwSize);
	
	ar.WriteBuffer(&dwSize, 0x0004);
	ar.WriteBuffer(lpCipher, dwSize);

	dwSize = (lstrlen(m_xxSdeData.strUserName) + 1) * sizeof(TCHAR);
	if(dwSize < 8)  dwSize = 8;
	
	lpCipher[0] = '\0';
	EncryptBuffer(m_xxSdeData.strUserName, lpCipher, dwSize);
	
	ar.WriteBuffer(&dwSize, 0x0004);
	ar.WriteBuffer(lpCipher, dwSize);

	dwSize = (lstrlen(m_xxSdeData.strPassword) + 1) * sizeof(TCHAR);
	if(dwSize < 8)  dwSize = 8;
	
	lpCipher[0] = '\0';
	EncryptBuffer(m_xxSdeData.strPassword, lpCipher, dwSize);
	
	ar.WriteBuffer(&dwSize, 0x0004);
	ar.WriteBuffer(lpCipher, dwSize);

	// �ر�ϵͳ�����ļ�
	ar.Close();
	
	return TRUE;
}

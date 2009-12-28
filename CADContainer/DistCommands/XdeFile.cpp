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

// === 加密缓冲区 ===
void EncryptBuffer(const void * lpSource, void * lpBuffer, int nSize) 
{
	if(nSize > 7) 
	{
		char * xxSource = (char *) lpSource;
		char * xxBuffer = (char *) lpBuffer;
		
		while(nSize > 0) 
		{
			nSize -= 0x0008;
			
			// 正常加密
			Des_Encrypt(xxSource, xxBuffer, "[O_++_O]");
			
			// 位置偏移
			if(nSize > 7) 
			{
				xxSource += 8;  xxBuffer += 8;
			}
			else if(nSize > 0)
			{
				// 补充余位
				if(xxBuffer != xxSource) 
				{
					// 复制余位
					memcpy(xxBuffer + 8, xxSource + 8, nSize);
				}
				
				// 移位加密
				Des_Encrypt(xxBuffer + nSize, xxBuffer + nSize, "[O_++_O]");
				
				// 保证结束
				nSize = 0x0000;
			}
		}
	}
}

// === 解密缓冲区 ===
void DecryptBuffer(const void * lpBuffer, void * lpSource, int nSize) 
{
	if(nSize > 7) 
	{
		char * xxSource = (char *) lpSource;
		char * xxBuffer = (char *) lpBuffer;
		
		while(nSize > 0) 
		{
			nSize -= 0x0008;
			
			// 错位计算
			if(nSize > 0 && nSize < 8) 
			{
				// 反向位数
				Des_Decrypt(xxBuffer + nSize, xxSource + nSize, "[O_++_O]");
				
				// 补充余位
				if(xxBuffer != xxSource) 
				{
					// 复制余位
					memcpy(xxSource, xxBuffer, nSize);
				}
				
				// 余位解密
				Des_Decrypt(xxSource, xxSource, "[O_++_O]");
				
				// 保证结束
				nSize = 0x0000;
			}
			else 
			{
				// 正常字节
				Des_Decrypt(xxBuffer, xxSource, "[O_++_O]");
				
				// 位置偏移
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
	// 读取几个基本参数
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
	// 解密
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strServer = lpBuffer;

	ar.ReadBuffer(&dsize, 0x0004);
	ar.ReadBuffer(lpCipher, dsize);
	// 解密
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strService = lpBuffer;

	ar.ReadBuffer(&dsize, 0x0004);
	ar.ReadBuffer(lpCipher, dsize);
	// 解密
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strDatabase = lpBuffer;

	ar.ReadBuffer(&dsize, 0x0004);
	ar.ReadBuffer(lpCipher, dsize);
	// 解密
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strUserName = lpBuffer;

	ar.ReadBuffer(&dsize, 0x0004);
	ar.ReadBuffer(lpCipher, dsize);
	// 解密
	DecryptBuffer(lpCipher, lpBuffer, dsize);
	m_xxSdeData.strPassword = lpBuffer;

	ar.Close();

	return TRUE;
}

BOOL CXdeFile::WriteXdeFile(HANDLE  hFile)
{
	// 存入几个基本参数
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

	// 关闭系统工程文件
	ar.Close();
	
	return TRUE;
}

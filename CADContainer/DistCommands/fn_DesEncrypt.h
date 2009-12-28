// fn_DesEncrypt.h --- interface for the DES encrypt
/////////////////////////////////////////////////////////////////

#ifndef  XIN_DESENCRYPT_H__INCLUDED_
#define  XIN_DESENCRYPT_H__INCLUDED_

#include "DesEncrypt.h"

// Desº”√‹”ÎΩ‚√‹
DESENCRYPT_API void Des_Encrypt(char lpSrc[8], char lpDest[8], char lpKey[8]);
DESENCRYPT_API void Des_Decrypt(char lpSrc[8], char lpDest[8], char lpKey[8]);


#endif // XIN_DESENCRYPT_H__INCLUDED_

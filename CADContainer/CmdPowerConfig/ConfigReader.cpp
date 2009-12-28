#include "stdafx.h"
#include "configreader.h"
#include <io.h>
IMPLEMENT_SERIAL(CJn_CommandPower,CObject,1)

void CJn_CommandPower::Serialize( CArchive& archive )
{
	CObject::Serialize( archive );
	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
		for (std::map<CString,BOOL>::iterator it = this->powerList.begin();
			it != this->powerList.end(); it ++)
		{
			CString strKey = it->first;
			int n = strKey.GetLength();
			archive << n;
			for (int i = 0 ; i < n ; i ++)
			{
				char c = strKey.GetAt(i);
				UINT ui = (UINT)c;
				ui <<= 2;
				archive << ui;
			}			
			archive<< it->second;
		}
	}
	else
	{			
		int n = this->powerList.size();
		for (int i = 0 ; i < n ; i ++)
		{
			CString key;
			
			BOOL state;
			int len;
			archive >> len;
			for (int j = 0; j < len; j ++)
			{
				char c;
				UINT uc;
				archive >> uc;
				uc >>= 2;
				c = (char)uc;
				key.AppendChar(c);
			}
			archive >> state;
			this->powerList[key] = state;
		}
	}
}
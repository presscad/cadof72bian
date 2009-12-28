#ifndef __XINIFILE_H
#define __XINIFILE_H

#include "stdafx.h"

class XIniFile
{
  protected:
    CString x_strIniFile;
    CString x_strSection;
  public:
    XIniFile(){};
    ~XIniFile(){};
    inline void OpenIniFile(LPCTSTR strFile)
    {
      x_strIniFile = strFile;
    };
    inline void OpenSection(LPCTSTR strSection)
    {
      x_strSection = strSection;
    };

	inline unsigned int WriteSection(LPCTSTR strKey)
    {
		return ::WritePrivateProfileSection(x_strSection, strKey, x_strIniFile);
    };
	
    inline unsigned int DeleteSection()
    {
      return DeleteValue(NULL);
    };

    inline unsigned int ReadValue(LPCTSTR strKey, LPTSTR strValue, LPCTSTR strDefaultValue, int nSize)const
    {
      return ::GetPrivateProfileString(x_strSection, strKey, strDefaultValue, strValue, nSize, x_strIniFile);
    };

    inline unsigned int WriteValue(LPCTSTR strKey, LPCTSTR strValue)
    {
      return ::WritePrivateProfileString(x_strSection,strKey,strValue,x_strIniFile);
    };

    inline unsigned int DeleteValue(LPCTSTR strKey)
    {
      return WriteValue(strKey, NULL);
    };
};

#endif

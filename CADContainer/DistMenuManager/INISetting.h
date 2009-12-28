#if !defined(SETTING_H_HYP_2002_7_10)

#define SETTING_H_HYP_2002_7_10

struct KeyAndValue
{
	CString sKey;

	CString sValue;
};

typedef CMap<int, int&, KeyAndValue, KeyAndValue&> CMapIntToKeyAndValue;

class INISetting
{
public:
	//取INI文件名
	CString GetFilePath();

	//设INI文件名
	bool SetFilePath(LPCTSTR szINIFilePath);

	//取设置值，如果应用程序名为空，则在 Setting 中找，注意没能办法能判断是否存在
	CString GetSetting(LPCTSTR sKey,LPCTSTR sDefValue,LPCTSTR sAppName="Setting");

	//写值,如果应用程序名为空，并且文件中没有，则与到 Setting
	bool SetSetting(LPCTSTR sKey,LPCTSTR sDefValue,LPCTSTR sAppName="Setting");

	//取所有值,键名都专为大写，键值与文件一样，返回个数
	int GetAllKeysAndValues(LPCTSTR sAppName,CStringArray & aKeys,CStringArray & aValues);

	void GetAllAppName(CStringArray & aAppNames);

	int Get00Strings(void *pBuffer,CStringArray & aSaver);

	bool ReadIni(LPCTSTR szAppName, CMapIntToKeyAndValue & mapKeyToVal,LPCTSTR szFileName);

	BOOL ReadIni(CString sFileName, CString sAppName, CStringArray& asKeyName, CStringArray& asKeyValue);

	bool WriteIni( CString sAppName, CStringArray& asKeyName, CStringArray& asKeyValue,CString sFileName);

public:
	//构造函数
	INISetting();

	INISetting(LPCTSTR szINIFilePath);

	~INISetting();

public:
	struct PtrStruct
	{
		CMapIntToKeyAndValue Data;

		CString  sAppName;
	};

public:
	CString m_sINIFilePath;

	CMapStringToPtr   m_mapStringToMap;

private:
	//读文件
	bool ReadFile();

	//清除
	void ClearMap();

	char *m_pBuffer;

	CStringArray m_aAppNameArray;//所得的应用程序名数组
};

bool GetINISetting(LPCTSTR szINIFileName, INISetting & iniset);

#endif


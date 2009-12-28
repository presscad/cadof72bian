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
	//ȡINI�ļ���
	CString GetFilePath();

	//��INI�ļ���
	bool SetFilePath(LPCTSTR szINIFilePath);

	//ȡ����ֵ�����Ӧ�ó�����Ϊ�գ����� Setting ���ң�ע��û�ܰ취���ж��Ƿ����
	CString GetSetting(LPCTSTR sKey,LPCTSTR sDefValue,LPCTSTR sAppName="Setting");

	//дֵ,���Ӧ�ó�����Ϊ�գ������ļ���û�У����뵽 Setting
	bool SetSetting(LPCTSTR sKey,LPCTSTR sDefValue,LPCTSTR sAppName="Setting");

	//ȡ����ֵ,������רΪ��д����ֵ���ļ�һ�������ظ���
	int GetAllKeysAndValues(LPCTSTR sAppName,CStringArray & aKeys,CStringArray & aValues);

	void GetAllAppName(CStringArray & aAppNames);

	int Get00Strings(void *pBuffer,CStringArray & aSaver);

	bool ReadIni(LPCTSTR szAppName, CMapIntToKeyAndValue & mapKeyToVal,LPCTSTR szFileName);

	BOOL ReadIni(CString sFileName, CString sAppName, CStringArray& asKeyName, CStringArray& asKeyValue);

	bool WriteIni( CString sAppName, CStringArray& asKeyName, CStringArray& asKeyValue,CString sFileName);

public:
	//���캯��
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
	//���ļ�
	bool ReadFile();

	//���
	void ClearMap();

	char *m_pBuffer;

	CStringArray m_aAppNameArray;//���õ�Ӧ�ó���������
};

bool GetINISetting(LPCTSTR szINIFileName, INISetting & iniset);

#endif


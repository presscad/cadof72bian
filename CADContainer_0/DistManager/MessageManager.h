#pragma once

class MessageManager
{
public:
	MessageManager(void);
	~MessageManager(void);

private:

	static 	WNDPROC	m_OldProc;              //����CAD�����崰�ں���ָ��
	//static  CDistCommandManagerReactor*     m_pCmdReactor;  //�ĵ���Ӧ��
	
	//DAP ϵͳ��Ϣ
	static CString m_strWebServiceURL;     // WebService���ʵ�ַ
    static CString m_strDapUserId;	       // ��¼�û�ID
	static CString m_strPROJECTID;         // ��ǰ��Ŀ���
	static CString m_strXMLX;              // ��Ŀ���ͣ�"ѡַ","�õ�","����","Ԥѡ","����"��
	static CString m_strRoleId;            // �û���ɫID
	static CString m_strPreProjectId;      // �Ͻ׶�����ID(ҵ��ϵͳҪ�����)
	static CString m_strXZQH;              // ������������(���Ʒ־ֵ���ʾ��Χ)
	static CString m_strDapProcessId;      // DAP����ID

private:
	///////////////////////////////////////// �������� //////////////////////////////////////

	static void ProcessDataFromDap(CString strData);
	static void ProcessCommandFromDap(CString strData);

public:

	//�Զ��崰�ں���
	static LRESULT CALLBACK DistProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//ϵͳ��ʼ������
	static void InitSystem();
};

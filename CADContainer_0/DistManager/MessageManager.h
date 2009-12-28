#pragma once

class MessageManager
{
public:
	MessageManager(void);
	~MessageManager(void);

private:

	static 	WNDPROC	m_OldProc;              //保存CAD主窗体窗口函数指针
	//static  CDistCommandManagerReactor*     m_pCmdReactor;  //文档反应器
	
	//DAP 系统信息
	static CString m_strWebServiceURL;     // WebService访问地址
    static CString m_strDapUserId;	       // 登录用户ID
	static CString m_strPROJECTID;         // 当前项目编号
	static CString m_strXMLX;              // 项目类型（"选址","用地","红线","预选","储备"）
	static CString m_strRoleId;            // 用户角色ID
	static CString m_strPreProjectId;      // 上阶段流程ID(业务系统要求添加)
	static CString m_strXZQH;              // 行政区划名称(控制分局的显示范围)
	static CString m_strDapProcessId;      // DAP进程ID

private:
	///////////////////////////////////////// 辅助函数 //////////////////////////////////////

	static void ProcessDataFromDap(CString strData);
	static void ProcessCommandFromDap(CString strData);

public:

	//自定义窗口函数
	static LRESULT CALLBACK DistProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//系统初始化函数
	static void InitSystem();
};

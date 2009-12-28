//�ļ�˵�����Զ�������

#ifndef __ARXBASEPARA__H__
#define __ARXBASEPARA__H__ 

//��ѧ�������任
#define PI               (3.141592653589793)
#define DEG2SEMI         (1.0/180.0)			 //degees to semisircles
#define SEMI2RAD         (PI)				     //semisircles to radians
#define DEG2RAD          (PI/180.0)			     //degrees to radians
#define SEC2RAD          (PI/180.0/3600.0)		 //seconds to radians
#define RAD2DEG          (180.0/PI)			     //radians to degrees
#define RAD2SEC          (3600.0*180.0/PI)       //radians to seconds
#define EPSILON          (1e-10)				 //minimax float value can be ignored

#define INFINITY         1e+10
#define PRECISION        1e-3   //�������ȽϾ���



//CAD�������ݸ�DAP�ṹ
struct DIST_WEBINTERACT_STRUCT
{
	CString	nmId;
	CString	ProcessId;
	CString	BdeData;	
	CString	Description;
};


struct DIST_GLOBINFO_STRUCT
{
	CString	m_XMBH			;  // ��Ŀ��� =	"XmBhTest0011"
	CString	m_GLBH			;  // ������� =	"GlBhTest0011"
	CString	m_Tfbh			;  // ͼ����� =	"V11-24"; ������GT07060001��������V11-24��
	CString	m_Lch			;  // ���̺�   =	"LchTest0011"
	CString	m_Zh			;  // ֤��     =	"ZhTest0011"
	CString	m_Rq			;  // ����     =	COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d"))
	CString	m_Xmmc			;  // ��Ŀ���� =	"�½���Ŀ"
	CString	m_Jsdw			;  // ���赥λ =	"JsdwTest00"
	CString m_Jsdz			;  // �����ַ =	"JsdzTest00"
	CString	m_Ydxz			;  // �õ����ʱ��=	"R11"
};


struct DIST_BASEPROJECT_STRUCT
{
	CString strPROJECT_CODE;             // ��Ŀ��ʾ���
	CString strLICENSE_CODE;             // ��֤���
	CString strDRAW_TIME;                // ��ȡʱ��
	CString strREG_TIME;                 // �Ǽ�ʱ��
	CString strBUILD_UNIT;               // ���赥λ����
	CString strBUILD_ADDRESS;            // �����ַ
	CString strPROJECT_NAME;             // ��Ŀ����
	CString strRELATION_CODE;            // �������
	CString strMAP_CODE;                 // ͼ�����
	CString strAFFIX;                    // ���ϴ�
	CString strOVER_TIME;                // �᰸ʱ��
	CString strPROJECT_STATUS;           // ��Ŀ״̬
	CString strREF_PROJECTDEFINE_ID;     // ��Ŀ���ͱ��
	CString strPROJECT_ID;               // ��Ŀ�������
	CString strBUILD_UNIT_PHONE;         // ���赥λ�绰
	CString strBUILD_UNIT_LINKMAN;       // ��ϵ��
	CString strLINKMAN_IDENTITYID;       // ���֤��
	CString strDELFLAG;                  // �Ƿ�ɾ����־
	CString strREF_PACKAGE_ID;           // ���ϴ����
};

struct DIST_CURUSERINFO_STRUCT
{
	CString strLOGIN_NAME ; // ��½����
	CString strJOB_STATUS;  // ��������
	CString strUSER_NAME;   // �û�����
	CString strUSER_ID;     // �û�ID
	CString strREMARK;      // ��������
	CString strREG_TIME;    // ��½ʱ��
	CString strPASSWORD;    // ��������
};

struct DIST_ACCESSPOWER_STRUCT
{
	CString	strLayerName;	// CADͼ����
	CString	Power;			// Ȩ��
	CString	Prior;			// ���ȼ�
};


struct DIST_POLTINDEXCFG_STRUCT
{
	CString	Id;			// ���
	CString	SDELayer;	// SDE������
	CString	AliasName;	// ���ı���(��ʾ����)
	CString	FieldName;	// �����ֶ�����
	CString	OrderNum;	// ����(Ĭ��Ϊ���)
	CString Type;		// ��¼���(1��ʾ��Ŀ���)
};


#endif// __ARXBASEPARA__H__ 

///////////////////////////////////////////////////////////////////////////////
//�Ϻ�����ϵͳ�������޹�˾ӵ�б�����֮��ȫ��Ȩ��δ���Ϻ�����ϵͳ�������޹�˾ //
//����ȷ������ɣ����ø��ơ�ȡ�á��ַ���Դ���롣                             //
//�κε�λ�͸�����δ����������£��������κ���ʽ���ơ��޸ĺͷ������������ //
//�β��֣�������Ϊ�Ƿ��ֺ����ҹ�˾����������׷�������ε�Ȩ������δ������ //
//����£��κ���֯����˷����Ĳ�Ʒ������ʹ��Dist�ı�־��Logo��               //
//                                                                           //
//support@dist.com.cn                                                        //
//www.dist.com.cn                                                            //
//                                                                           //
//���� : ����                                                                //
//                                                                           //
// ��Ȩ���� (C) 2007��2010 Dist Inc. ��������Ȩ����                          //
///////////////////////////////////////////////////////////////////////////////

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
#define PRECISION        1e-4   //�������ȽϾ���



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


//��Ŀ������Ϣ�ṹ
struct DIST_PRJBASEINFO_STRUCT
{
	CString strPROJECT_ID;            // ��Ŀ���
	CString strPROJECT_CODE;          // ��Ŀ��ʾ���
	CString strRELATION_CODE;         // �������
	CString strREG_TIME;              // �Ǽ�ʱ��
	CString strPROJECT_NAME;          // ��Ŀ����
	CString strPRE_PROJECT_ID;        // ��һҵ����ĿĿ���
	CString strBUILD_UNIT;            // ���赥λ
	CString strBUILD_UNIT_LINKMAN;    // ��ϵ��
	CString strBUILD_UNIT_PHONE;      // �绰
	CString strOVER_TIME;             // �᰸ʱ��
	CString strBUILD_ADDRESS;         // ����ص�
	CString strLICENCE_CODE;          // ��֤���
	CString strUSELAND_TYPE;          // �õ�����
	CString strUSELAND_TYPEID;        // �õ����ʱ��
	CString strARCHIITECTUREPRO_TYPE; // ���蹤������
	CString strHOURSEARCHI_TYPE;      // ��ס��������
	CString strINVEST_SUM;            // Ͷ���ܹ�ģ
	CString strFLOOR_AREA_RATIO;      // �ݻ���
	CString strBUILDING_DENSITY;      // �����ܶ�
	CString strPRO_STATUS;            // ��Ŀ״̬
	CString strTFBH;                  // ͼ�����
	CString strJBRY;                  // ������Ա

	CString strCADTYPE;               // ��������
	CString strSUM_LAND_AREA_SUM;     // ���õ�������ģ�
	CString strBUILD_LAND_AREA_SUM;   // �����õ���������ģ�
	CString strBUILD_AREA_SUM;        // ������������ģ�
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

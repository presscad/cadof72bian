//文件说明：自定义类型

#ifndef __ARXBASEPARA__H__
#define __ARXBASEPARA__H__ 

//数学常量及变换
#define PI               (3.141592653589793)
#define DEG2SEMI         (1.0/180.0)			 //degees to semisircles
#define SEMI2RAD         (PI)				     //semisircles to radians
#define DEG2RAD          (PI/180.0)			     //degrees to radians
#define SEC2RAD          (PI/180.0/3600.0)		 //seconds to radians
#define RAD2DEG          (180.0/PI)			     //radians to degrees
#define RAD2SEC          (3600.0*180.0/PI)       //radians to seconds
#define EPSILON          (1e-10)				 //minimax float value can be ignored

#define INFINITY         1e+10
#define PRECISION        1e-3   //浮点数比较精度



//CAD传送数据给DAP结构
struct DIST_WEBINTERACT_STRUCT
{
	CString	nmId;
	CString	ProcessId;
	CString	BdeData;	
	CString	Description;
};


struct DIST_GLOBINFO_STRUCT
{
	CString	m_XMBH			;  // 项目编号 =	"XmBhTest0011"
	CString	m_GLBH			;  // 关联编号 =	"GlBhTest0011"
	CString	m_Tfbh			;  // 图幅编号 =	"V11-24"; （扬州GT07060001）（厦门V11-24）
	CString	m_Lch			;  // 流程号   =	"LchTest0011"
	CString	m_Zh			;  // 证号     =	"ZhTest0011"
	CString	m_Rq			;  // 日期     =	COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d"))
	CString	m_Xmmc			;  // 项目名称 =	"新建项目"
	CString	m_Jsdw			;  // 建设单位 =	"JsdwTest00"
	CString m_Jsdz			;  // 建设地址 =	"JsdzTest00"
	CString	m_Ydxz			;  // 用地性质编号=	"R11"
};


struct DIST_BASEPROJECT_STRUCT
{
	CString strPROJECT_CODE;             // 项目显示编号
	CString strLICENSE_CODE;             // 发证编号
	CString strDRAW_TIME;                // 领取时间
	CString strREG_TIME;                 // 登记时间
	CString strBUILD_UNIT;               // 建设单位名称
	CString strBUILD_ADDRESS;            // 建设地址
	CString strPROJECT_NAME;             // 项目名称
	CString strRELATION_CODE;            // 关联编号
	CString strMAP_CODE;                 // 图幅编号
	CString strAFFIX;                    // 材料袋
	CString strOVER_TIME;                // 结案时间
	CString strPROJECT_STATUS;           // 项目状态
	CString strREF_PROJECTDEFINE_ID;     // 项目类型编号
	CString strPROJECT_ID;               // 项目索引编号
	CString strBUILD_UNIT_PHONE;         // 建设单位电话
	CString strBUILD_UNIT_LINKMAN;       // 联系人
	CString strLINKMAN_IDENTITYID;       // 身份证号
	CString strDELFLAG;                  // 是否删除标志
	CString strREF_PACKAGE_ID;           // 材料袋编号
};

struct DIST_CURUSERINFO_STRUCT
{
	CString strLOGIN_NAME ; // 登陆名称
	CString strJOB_STATUS;  // 工作部门
	CString strUSER_NAME;   // 用户姓名
	CString strUSER_ID;     // 用户ID
	CString strREMARK;      // 代理名称
	CString strREG_TIME;    // 登陆时间
	CString strPASSWORD;    // 加密密码
};

struct DIST_ACCESSPOWER_STRUCT
{
	CString	strLayerName;	// CAD图层名
	CString	Power;			// 权限
	CString	Prior;			// 优先级
};


struct DIST_POLTINDEXCFG_STRUCT
{
	CString	Id;			// 编号
	CString	SDELayer;	// SDE表名称
	CString	AliasName;	// 中文别名(显示名称)
	CString	FieldName;	// 索引字段名称
	CString	OrderNum;	// 次序(默认为编号)
	CString Type;		// 记录类别(1表示项目编号)
};


#endif// __ARXBASEPARA__H__ 

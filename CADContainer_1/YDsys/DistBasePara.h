///////////////////////////////////////////////////////////////////////////////
//上海数慧系统技术有限公司拥有本代码之完全版权，未经上海数慧系统技术有限公司 //
//的明确书面许可，不得复制、取用、分发此源代码。                             //
//任何单位和个人在未获得许可情况下，不得以任何形式复制、修改和发布本软件的任 //
//何部分，否则将视为非法侵害，我公司将保留依法追究其责任的权利。在未获得许可 //
//情况下，任何组织或个人发布的产品都不能使用Dist的标志和Logo。               //
//                                                                           //
//support@dist.com.cn                                                        //
//www.dist.com.cn                                                            //
//                                                                           //
//作者 : 王晖                                                                //
//                                                                           //
// 版权所有 (C) 2007－2010 Dist Inc. 保留所有权利。                          //
///////////////////////////////////////////////////////////////////////////////

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
#define PRECISION        1e-4   //浮点数比较精度



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


//项目基本信息结构
struct DIST_PRJBASEINFO_STRUCT
{
	CString strPROJECT_ID;            // 项目编号
	CString strPROJECT_CODE;          // 项目显示编号
	CString strRELATION_CODE;         // 关联编号
	CString strREG_TIME;              // 登记时间
	CString strPROJECT_NAME;          // 项目名称
	CString strPRE_PROJECT_ID;        // 上一业务项目目编号
	CString strBUILD_UNIT;            // 建设单位
	CString strBUILD_UNIT_LINKMAN;    // 联系人
	CString strBUILD_UNIT_PHONE;      // 电话
	CString strOVER_TIME;             // 结案时间
	CString strBUILD_ADDRESS;         // 建设地点
	CString strLICENCE_CODE;          // 发证编号
	CString strUSELAND_TYPE;          // 用地性质
	CString strUSELAND_TYPEID;        // 用地性质编号
	CString strARCHIITECTUREPRO_TYPE; // 建设工程性质
	CString strHOURSEARCHI_TYPE;      // 居住建筑类型
	CString strINVEST_SUM;            // 投资总规模
	CString strFLOOR_AREA_RATIO;      // 容积率
	CString strBUILDING_DENSITY;      // 建筑密度
	CString strPRO_STATUS;            // 项目状态
	CString strTFBH;                  // 图幅编号
	CString strJBRY;                  // 经办人员

	CString strCADTYPE;               // 蓝线类型
	CString strSUM_LAND_AREA_SUM;     // 总用地面积（文）
	CString strBUILD_LAND_AREA_SUM;   // 建设用地总面积（文）
	CString strBUILD_AREA_SUM;        // 建筑总面积（文）
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

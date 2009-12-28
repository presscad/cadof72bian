#ifndef  _COMMANDDEFINE_H_
#define  _COMMANDDEFINE_H_

// CAD命令对照表
const CString Glb_strCommandToId[] = 
{
	// 系统命令
	"80000001",	"new",		"new",
	"80000002",	"qnew",		"qnew",
	"80000003",	"open",		"open",
	"80000004",	"qsave",	"qsave",
	"80000005",	"saveas",	"saveas",
	"80000006",	"export",	"export",
	"80000007",	"WBLOCK",	"WBLOCK",
	"80000008",	"COPY",		"COPY",
	"80000009",	"COPYCLIP",	"COPYCLIP",
	"80000010",	"copybase",	"copybase",
	"80000011",	"CUTCLIP",	"CUTCLIP",
	"80000012",	"APPLOAD",	"APPLOAD",
	"80000013",	"ARX",		"ARX",
	"80000014",	"VBAIDE",	"VBAIDE",
	"80000015",	"VBARUN",	"VBARUN",
	"80000016",	"VBALOAD",	"VBALOAD",
	"80000017",	"VBAMAN",	"VBAMAN",
	"80000018",	"save",		"save",
	"80000019",	"quit",		"quit",

	// 自定义命令
	"90000001",	"ShowDockTreePane",	"显示目录树",
	"90000002",	"ReadSdeLayers",	"自动读取SDE图层数据",
	"90000003",	"ReadSdeLayersEx",	"手工查询SDE图层数据",
	"90000004",	"DistSelProject",	"相关项目查询",
	"90000005",	"ShowObjectAttr",	"查看编辑属性",
	"90000006",	"DrawXMFWX",		"项目范围入库",
	"90000007",	"DrawXMHX",			"制作项目红线",
	"90000008",	"SaveSdeLayers",	"保存SDE图层数据",
	"90000009",	"DrawDLHX",			"道路红线",
	"90000010",	"DrawCSLX",			"城市蓝线",
	"90000011",	"DrawCSHX",			"城市黄线",
	"90000012",	"DrawCSLVX",		"城市绿线",
	"90000013",	"DrawCSZX",			"城市紫线",
	"90000014",	"DrawCSCX",			"城市橙线",
};

#endif
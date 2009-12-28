/////////////////////////////////////////////
// AcApDocManagerReactor reactors.

#include "StdAfx.h" 
#include "DistCommandManagerReactor.h"
#include "DataDefine.h"
#include "CommandDefine.h"
#include "CommonFunc.h"
#include "configreader.h"

extern MapString2Power Glb_commandPower; // CAD命令权限对照表

DistCommandManagerReactor::DistCommandManagerReactor(const bool autoInitAndRelease)
{
	mbAutoInitAndRelease = autoInitAndRelease;
	if (mbAutoInitAndRelease)
		if (NULL != acDocManager)
			acDocManager->addReactor(this);
		else
			mbAutoInitAndRelease = false;
}

DistCommandManagerReactor::~DistCommandManagerReactor()
{
	if (mbAutoInitAndRelease)
		if (NULL != acDocManager)
			acDocManager->removeReactor(this);
}

void DistCommandManagerReactor::documentActivated(AcApDocument* pActivatedDoc)
{
	// TODO: implement this function.

}

void DistCommandManagerReactor::documentLockModeChanged(AcApDocument* x0,
													  AcAp::DocLockMode myPreviousMode,
													  AcAp::DocLockMode myCurrentMode,
													  AcAp::DocLockMode currentMode,
													  const char* pGlobalCmdName)
{
	CString strGlobalCmdName = pGlobalCmdName;

	if (strGlobalCmdName.CompareNoCase("close") == 0)
	{    
		//CString strProjectCode = DocVars.docData().GetProjectCode();

		//if (!strProjectCode.IsEmpty())
		CString strProjectId = DocVars.docData().GetProjectId();

		if (!strProjectId.IsEmpty())
		{
			veto(); 
			acutPrintf("\n当前文档不能关闭");
		}
		return;
	}

	if (strGlobalCmdName.CompareNoCase("qsave") == 0)
	{
		CString strProjectId = DocVars.docData().GetProjectId();

		if (!strProjectId.IsEmpty() && atoi(strProjectId) > 0)
		{
			veto(); 
			acutPrintf("\n保存工作图文件.");

			SaveCurDWGtoFtp();
		}
		else
		{
			veto(); 
			acutPrintf("\n当前命令已禁用:%s",strGlobalCmdName);				
		}
		return;
	}

	//读取权限。
	CCmdPwrConfigReader& cfgReader = CCmdPwrConfigReader::Instanse();
	CJn_CommandPower * pPower = cfgReader.GetCmdPwrConfig();
	if (pPower->powerList.find(strGlobalCmdName) != pPower->powerList.end())	//命令在管理列表中的会受权限控制
	{																			//命令不在管理列表中的不受限制
		if (pPower->powerList[strGlobalCmdName] == 0)
		{
			veto();
			acutPrintf("\n当前命令已禁用:%s",strGlobalCmdName);
		}
	}
/*
	// 如果没有配置权限，默认所有功能可用
	if (Glb_commandPower.size() <= 0)
		return;
*/
	// 默认将系统命令都禁掉
//	int nCount = 19*3;//sizeof(Glb_strCommandToId)/sizeof(CString);
//	for (int i=1; i<nCount; i+=3)
//	{
//		CString strCommand = Glb_strCommandToId[i];
//		if (strGlobalCmdName.CompareNoCase(strCommand) == 0)
//		{
////			POWER power = Glb_commandPower[strCommand];
//			
//			// power.nPrior 优先级 (最小值为1，值越大优先级越高)
//			// power.nPower 权限值 (0：隐藏 1：只读 2：可写 3：必填)
////			if ((power.nPrior > 0) && ((power.nPower == 0) || (power.nPower == 1)))
//			{
//				veto(); 
//				acutPrintf("\n当前命令已禁用:%s",strCommand);				
//			}
//		}
//	}

}

void DistCommandManagerReactor::documentLockModeChangeVetoed(AcApDocument* x0,
														   const char* pGlobalCmdName)
{


}

void DistCommandManagerReactor::documentLockModeWillChange(AcApDocument* x0,
														 AcAp::DocLockMode myCurrentMode,
														 AcAp::DocLockMode myNewMode,
														 AcAp::DocLockMode currentMode,
														 const char* pGlobalCmdName)
{
	// TODO: implement this function. 
}


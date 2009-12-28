/////////////////////////////////////////////
// AcApDocManagerReactor reactors.

#include "StdAfx.h" 
#include "DistCommandManagerReactor.h"
#include "DataDefine.h"
#include "CommandDefine.h"
#include "CommonFunc.h"
#include "configreader.h"

extern MapString2Power Glb_commandPower; // CAD����Ȩ�޶��ձ�

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
			acutPrintf("\n��ǰ�ĵ����ܹر�");
		}
		return;
	}

	if (strGlobalCmdName.CompareNoCase("qsave") == 0)
	{
		CString strProjectId = DocVars.docData().GetProjectId();

		if (!strProjectId.IsEmpty() && atoi(strProjectId) > 0)
		{
			veto(); 
			acutPrintf("\n���湤��ͼ�ļ�.");

			SaveCurDWGtoFtp();
		}
		else
		{
			veto(); 
			acutPrintf("\n��ǰ�����ѽ���:%s",strGlobalCmdName);				
		}
		return;
	}

	//��ȡȨ�ޡ�
	CCmdPwrConfigReader& cfgReader = CCmdPwrConfigReader::Instanse();
	CJn_CommandPower * pPower = cfgReader.GetCmdPwrConfig();
	if (pPower->powerList.find(strGlobalCmdName) != pPower->powerList.end())	//�����ڹ����б��еĻ���Ȩ�޿���
	{																			//����ڹ����б��еĲ�������
		if (pPower->powerList[strGlobalCmdName] == 0)
		{
			veto();
			acutPrintf("\n��ǰ�����ѽ���:%s",strGlobalCmdName);
		}
	}
/*
	// ���û������Ȩ�ޣ�Ĭ�����й��ܿ���
	if (Glb_commandPower.size() <= 0)
		return;
*/
	// Ĭ�Ͻ�ϵͳ�������
//	int nCount = 19*3;//sizeof(Glb_strCommandToId)/sizeof(CString);
//	for (int i=1; i<nCount; i+=3)
//	{
//		CString strCommand = Glb_strCommandToId[i];
//		if (strGlobalCmdName.CompareNoCase(strCommand) == 0)
//		{
////			POWER power = Glb_commandPower[strCommand];
//			
//			// power.nPrior ���ȼ� (��СֵΪ1��ֵԽ�����ȼ�Խ��)
//			// power.nPower Ȩ��ֵ (0������ 1��ֻ�� 2����д 3������)
////			if ((power.nPrior > 0) && ((power.nPower == 0) || (power.nPower == 1)))
//			{
//				veto(); 
//				acutPrintf("\n��ǰ�����ѽ���:%s",strCommand);				
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


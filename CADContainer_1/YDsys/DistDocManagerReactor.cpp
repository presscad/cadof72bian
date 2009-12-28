/////////////////////////////////////////////
// AcApDocManagerReactor reactors.

#include "StdAfx.h"
#include "DistDocManagerReactor.h"

extern BOOL g_bSupperPower;

CDistCommandManagerReactor::CDistCommandManagerReactor(const bool autoInitAndRelease)
{
	m_autoInitAndRelease = autoInitAndRelease;
	if (m_autoInitAndRelease)
	{
		if (NULL != acDocManager)
			acDocManager->addReactor(this);
		else
			m_autoInitAndRelease = false;
	}
	m_bSave=TRUE;
}

CDistCommandManagerReactor::~CDistCommandManagerReactor()
{
	if (m_autoInitAndRelease)
	{
		if (NULL != acDocManager)
			acDocManager->removeReactor(this);
	}
} 
 
void CDistCommandManagerReactor::documentLockModeChanged(AcApDocument* x0,
                                                     AcAp::DocLockMode myPreviousMode,
                                                     AcAp::DocLockMode myCurrentMode,
                                                     AcAp::DocLockMode currentMode,
                                                     const char* pGlobalCmdName)
{ 
	if((m_bSave==FALSE)&&(g_bSupperPower==FALSE)) //没有保存权限
	{
		CString str=pGlobalCmdName;
		if(str=="SAVE"||str=="SAVEAS"
			||str=="QSAVE"||str=="EXPORT"||str=="WBLOCK")
		{    
			veto(); 
			acutPrintf("\n该命令已经被禁止执行!");
		}
	}
	
}


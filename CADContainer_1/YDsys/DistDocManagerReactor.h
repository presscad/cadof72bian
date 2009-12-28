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

#ifndef  __DIST__DOCMANAGER__REACTOR__H__
#define  __DIST__DOCMANAGER__REACTOR__H__


class CDistCommandManagerReactor : public AcApDocManagerReactor
{
public:

	// Constructor / Destructor
	         CDistCommandManagerReactor(const bool autoInitAndRelease = true);
	virtual ~CDistCommandManagerReactor();

	//{{AFX_ARX_METHODS(DistCommandManagerReactor)
 	virtual void documentLockModeChanged(AcApDocument* x0,
	                                     AcAp::DocLockMode myPreviousMode,
	                                     AcAp::DocLockMode myCurrentMode,
	                                     AcAp::DocLockMode currentMode,
	                                     const char* pGlobalCmdName);	//}}AFX_ARX_METHODS

private:
	// Auto initialization and release flag.
	bool m_autoInitAndRelease;
public:
	BOOL m_bSave; //具有保存权限
};

#endif //__DIST__DOCMANAGER__REACTOR__H__
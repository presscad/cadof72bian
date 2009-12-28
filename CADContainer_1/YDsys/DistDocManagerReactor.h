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
	BOOL m_bSave; //���б���Ȩ��
};

#endif //__DIST__DOCMANAGER__REACTOR__H__
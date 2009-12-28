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

#ifndef DIST_PERSISTENTOBJREACTOR_H
#define DIST_PERSISTENTOBJREACTOR_H



class CDistPersistentObjReactor : public AcDbObject 
{

public:
    ACRX_DECLARE_MEMBERS(CDistPersistentObjReactor);

    //����������
	 CDistPersistentObjReactor()	{}
    virtual			~CDistPersistentObjReactor()	{}

	// ���ܣ���ȡ��Ψһʵ������ָ��
	// ��������
	// ���أ�ʵ������ָ��
	static CDistPersistentObjReactor* GetInstance();

	// ���ܣ��ͷŶ���
	// ��������
	// ���أ���
	static void Release();

public:

	void				getAttachedToObjs(AcDbObjectIdArray& objIds) const;
	Acad::ErrorStatus	attachTo(const AcDbObjectId& objId);
	Acad::ErrorStatus	detachFrom(const AcDbObjectId& objId);

    virtual void    cancelled(const AcDbObject* obj);
    virtual void    copied(const AcDbObject* obj, const AcDbObject* newObj);
    virtual void    erased(const AcDbObject* obj, Adesk::Boolean pErasing = Adesk::kTrue);
    virtual void    goodbye(const AcDbObject* obj);
    virtual void    openedForModify(const AcDbObject* obj);
    virtual void    modified(const AcDbObject* obj);
    virtual void    subObjModified(const AcDbObject* obj, const AcDbObject* subObj);
    virtual void    modifyUndone(const AcDbObject* obj);
    virtual void    modifiedXData(const AcDbObject* obj);
    virtual void    unappended(const AcDbObject* obj);
    virtual void    reappended(const AcDbObject* obj);
    virtual void    objectClosed(const AcDbObjectId objId);

    virtual Acad::ErrorStatus	dwgInFields(AcDbDwgFiler *filer);
    virtual Acad::ErrorStatus	dwgOutFields(AcDbDwgFiler *filer) const;
    virtual	Acad::ErrorStatus	dxfInFields(AcDbDxfFiler *filer);
    virtual	Acad::ErrorStatus	dxfOutFields(AcDbDxfFiler *filer) const;

protected:
		// helper functions
    virtual void	printReactorMessage(LPCTSTR event, const AcDbObject* obj) const;

private:
	//���ݳ�Ա
	AcDbObjectIdArray	m_attachedToObjs;
    static Adesk::Int16 m_version;

	static CDistPersistentObjReactor*  m_pInstance;         // Ψһʵ��

    //���ú���
	                            
								CDistPersistentObjReactor(const& CDistPersistentObjReactor);
    CDistPersistentObjReactor&	operator=(const& CDistPersistentObjReactor);        

};

#endif    //  DIST_PERSISTENTOBJREACTOR_H
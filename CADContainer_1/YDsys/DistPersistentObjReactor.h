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

#ifndef DIST_PERSISTENTOBJREACTOR_H
#define DIST_PERSISTENTOBJREACTOR_H



class CDistPersistentObjReactor : public AcDbObject 
{

public:
    ACRX_DECLARE_MEMBERS(CDistPersistentObjReactor);

    //构造与析构
	 CDistPersistentObjReactor()	{}
    virtual			~CDistPersistentObjReactor()	{}

	// 功能：获取类唯一实例对象指针
	// 参数：无
	// 返回：实例对象指针
	static CDistPersistentObjReactor* GetInstance();

	// 功能：释放对象
	// 参数：无
	// 返回：无
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
	//数据成员
	AcDbObjectIdArray	m_attachedToObjs;
    static Adesk::Int16 m_version;

	static CDistPersistentObjReactor*  m_pInstance;         // 唯一实例

    //禁用函数
	                            
								CDistPersistentObjReactor(const& CDistPersistentObjReactor);
    CDistPersistentObjReactor&	operator=(const& CDistPersistentObjReactor);        

};

#endif    //  DIST_PERSISTENTOBJREACTOR_H
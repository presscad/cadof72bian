#include "stdafx.h"
#include "DistPersistentObjReactor.h"


Adesk::Int16     CDistPersistentObjReactor::m_version = 0;     // MDI safe

CDistPersistentObjReactor*  CDistPersistentObjReactor::m_pInstance = NULL; 

ACRX_DXF_DEFINE_MEMBERS(CDistPersistentObjReactor, AcDbObject,
						AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
						0, DIST_PERSISTENT_OBJ_REACTOR, Dist);



// 功能：获取类唯一实例对象指针
// 参数：无
// 返回：实例对象指针
CDistPersistentObjReactor* CDistPersistentObjReactor::GetInstance()
{
	if(m_pInstance == NULL)
		m_pInstance = new CDistPersistentObjReactor;

	return m_pInstance;
}


// 功能：释放对象
// 参数：无
// 返回：无
void CDistPersistentObjReactor::Release()
{
	if(m_pInstance != NULL)
		delete m_pInstance;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CDistPersistentObjReactor::getAttachedToObjs(AcDbObjectIdArray& objIds) const
{
	assertReadEnabled();

	objIds = m_attachedToObjs;
}


Acad::ErrorStatus CDistPersistentObjReactor::attachTo(const AcDbObjectId& objId)
{
	if (m_attachedToObjs.contains(objId) == false) {
		assertWriteEnabled();
		m_attachedToObjs.append(objId);
	}

	return Acad::eOk;
}


Acad::ErrorStatus CDistPersistentObjReactor::detachFrom(const AcDbObjectId& objId)
{
	if (m_attachedToObjs.contains(objId) == false)
		return Acad::eKeyNotFound;

	assertWriteEnabled();
	m_attachedToObjs.remove(objId);

	return Acad::eOk;
}


Acad::ErrorStatus CDistPersistentObjReactor::dwgInFields(AcDbDwgFiler* filer)
{
    Acad::ErrorStatus es;
    if ((es = AcDbObject::dwgInFields(filer)) != Acad::eOk)
        return es;

    Adesk::Int16 version;
    filer->readItem(&version);
    if (version > m_version)
        return Acad::eMakeMeProxy;

	AcDbSoftPointerId tmpId;
	Adesk::Int32 count;
	filer->readItem(&count);
	
	m_attachedToObjs.setLogicalLength(0);
	for (int i=0; i<count; i++) {
		filer->readItem(&tmpId);
		if (tmpId.isNull() == false)
			m_attachedToObjs.append(tmpId);
	}

    return filer->filerStatus();
}


Acad::ErrorStatus CDistPersistentObjReactor::dwgOutFields(AcDbDwgFiler* filer) const
{
    Acad::ErrorStatus es;
    if ((es = AcDbObject::dwgOutFields(filer)) != Acad::eOk)
        return es;

    filer->writeItem(m_version);

	int len = m_attachedToObjs.length();
	filer->writeInt32(len);	// count of ids

	for (int i=0; i<len; i++)
		filer->writeSoftPointerId(m_attachedToObjs[i]);

    return filer->filerStatus();
}


Acad::ErrorStatus CDistPersistentObjReactor::dxfInFields(AcDbDxfFiler* filer)
{
    Acad::ErrorStatus es;
    if (((es = AcDbObject::dxfInFields(filer)) != Acad::eOk) ||
        (filer->atSubclassData(this->desc()->name()) != Adesk::kTrue))
        return filer->filerStatus();

    resbuf rb;
	m_attachedToObjs.setLogicalLength(0);

    while ((es == Acad::eOk) && ((es = filer->readResBuf(&rb)) == Acad::eOk)) {
        if (rb.restype == AcDb::kDxfSoftPointerId) {
            AcDbObjectId tmpId;
            tmpId.setFromOldId(rb.resval.rlong);
            if (tmpId.isNull() == false)
                m_attachedToObjs.append(tmpId);
        }
        else {
            filer->pushBackItem();
            es = Acad::eEndOfFile;
        }
    }

    if (es != Acad::eEndOfFile)
        return Acad::eInvalidResBuf;

    return filer->filerStatus();
}



Acad::ErrorStatus CDistPersistentObjReactor::dxfOutFields(AcDbDxfFiler* filer) const
{
    Acad::ErrorStatus es;
    if ((es = AcDbObject::dxfOutFields(filer)) != Acad::eOk)
        return es;

    filer->writeItem(AcDb::kDxfSubclass, this->desc()->name());

    // write out all the Ids
    int len = m_attachedToObjs.length();
    for (int i=0; i<len; i++) {
        if (m_attachedToObjs[i].isNull() == false)
            filer->writeItem(AcDb::kDxfSoftPointerId, m_attachedToObjs[i]);
    }

    return filer->filerStatus();
}



void CDistPersistentObjReactor::cancelled(const AcDbObject* obj)
{
}



void CDistPersistentObjReactor::copied(const AcDbObject* obj, const AcDbObject* newObj)
{

}



void CDistPersistentObjReactor::erased(const AcDbObject* obj, Adesk::Boolean pErasing)
{

}


void CDistPersistentObjReactor::goodbye(const AcDbObject* obj)
{

}


void CDistPersistentObjReactor::openedForModify(const AcDbObject* obj)
{
}


void CDistPersistentObjReactor::modified(const AcDbObject* obj)
{
	if(obj->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline* pline = (AcDbPolyline*)obj;
		double dArea =0.0;
		pline->getArea(dArea);
		acutPrintf("\n当前地块面积为：%0.3f 平方米",dArea);
	}	
}



void CDistPersistentObjReactor::subObjModified(const AcDbObject* obj, const AcDbObject* subObj)
{

}


void CDistPersistentObjReactor::modifyUndone(const AcDbObject* obj)
{
	if(obj->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline* pline = (AcDbPolyline*)obj;
		double dArea =0.0;
		pline->getArea(dArea);
		acutPrintf("\n当前地块面积为：%0.3f 平方米",dArea);
	}	
}



void CDistPersistentObjReactor::modifiedXData(const AcDbObject* obj)
{
}



void CDistPersistentObjReactor::unappended(const AcDbObject* obj)
{
}


void CDistPersistentObjReactor::reappended(const AcDbObject* obj)
{
}


void CDistPersistentObjReactor::objectClosed(const AcDbObjectId objId)
{
}



void CDistPersistentObjReactor::printReactorMessage(LPCTSTR event, const AcDbObject* obj) const
{
 
}


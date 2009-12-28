// (C) Copyright 2002-2003 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- SHDatabaseReactor.cpp : Implementation of SHDatabaseReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "SHDatabaseReactor.h"
#include "datacontainer.h"
#include "CommonFunc.h"
//#include "BasePub.h"

CDataContainer dataContainer;
//AcDbExtents SelRangeExtents;
//-----------------------------------------------------------------------------
ACRX_CONS_DEFINE_MEMBERS(SHDatabaseReactor, AcDbDatabaseReactor, 1)

void arx_getXdata( AcDbObject* pObj , const char *cAppName , int iDxf , CStringArray& aValue )
{

	struct resbuf *pRb , *pTemp ;
	pRb = pObj->xData( cAppName );

	if ( pRb != NULL )
	{
		for ( pTemp = pRb ; pTemp != NULL ; pTemp = pTemp->rbnext )
		{
			if ( pTemp->restype == iDxf ) aValue.Add( pTemp->resval.rstring );
		}
	}

	acutRelRb( pRb ) ;
}

//写扩展数据
Acad::ErrorStatus arx_setXdata( AcDbObjectId idObj , const char *cAppName , int iDxf , CStringArray& aValue)
{
	// 1获得实体链表数据
	LockDocument _lock;

	AcDbObject *pObj ;
	Acad::ErrorStatus es ;

	es = acdbOpenObject( pObj , idObj , AcDb::kForRead );
	if ( es != Acad::eOk )
	{
		CString strErrMessage;

		strErrMessage.Format("\n错误信息提示：%s" , acadErrorStatusText( es ));

		AfxMessageBox(strErrMessage);
		return es ;
	}

	struct resbuf *pRb , *pTemp ;
	pRb = pObj->xData( cAppName );

	// 2调整链表
	if ( pRb == NULL )
	{
		// 注册新的应用程序名
		int err = acdbRegApp( cAppName );
		pRb = acutNewRb( AcDb::kDxfRegAppName );
		pTemp = pRb ;
		pTemp->resval.rstring = ( char* )malloc( strlen( cAppName ) + 1 );
		strcpy( pTemp->resval.rstring , cAppName );
	}
	else
	{
		pTemp = pRb ; 
		//// 将指针指向表尾
		//for ( pTemp = pRb ; pTemp->rbnext != NULL ; pTemp = pTemp->rbnext )
	}


	for(int i = 0; i < aValue.GetCount(); i ++){
		CString ValueTmp = aValue.GetAt(i) ;

		if(pTemp->rbnext == NULL){
			// 3添加新的字符串
			pTemp->rbnext = acutNewRb( iDxf );
		}

		pTemp = pTemp->rbnext ;
		pTemp->resval.rstring = ( char* )malloc( strlen( ValueTmp ) + 1 );
		strcpy(pTemp->resval.rstring,ValueTmp) ;
	}

	// 4设置链表数据
	es = pObj->upgradeOpen();

	if(es != Acad::eOk){
		pObj->close();
		es = pObj->upgradeOpen();
	}

	if(es == Acad::eOk){
		pObj->setXData( pRb );
	}
	else{
		AfxMessageBox("写入扩展数据出错！");
	}

	// 5关闭对象，释放空间，返回信息
	pObj->close();
	acutRelRb( pRb );
	return Acad::eOk ;
}


//-----------------------------------------------------------------------------
SHDatabaseReactor::SHDatabaseReactor (AcDbDatabase *pDb) : AcDbDatabaseReactor(), mpDatabase(pDb) {
	if ( pDb )
		pDb->addReactor (this) ;
}

//-----------------------------------------------------------------------------
SHDatabaseReactor::~SHDatabaseReactor () {
	Detach () ;
}

//-----------------------------------------------------------------------------
void SHDatabaseReactor::Attach (AcDbDatabase *pDb) {
	Detach () ;
	if ( mpDatabase == NULL ) {
		if ( (mpDatabase =pDb) != NULL )
			pDb->addReactor (this) ;
	}
}

void SHDatabaseReactor::Detach () {
	if ( mpDatabase ) {
		mpDatabase->removeReactor (this) ;
		mpDatabase =NULL ;
	}
}

AcDbDatabase *SHDatabaseReactor::Subject () const {
	return (mpDatabase) ;
}

bool SHDatabaseReactor::IsAttached () const {
	return (mpDatabase != NULL) ;
}

void SHDatabaseReactor::objectAppended(const AcDbDatabase* dwg, const AcDbObject* dbObj)
{
	AcDbEntity *pEnt = NULL;
	pEnt = AcDbEntity::cast(dbObj); 
	if(pEnt == NULL) return;
	if((pEnt->isKindOf(AcDbVertex::desc()))||(pEnt->isKindOf(AcDbSequenceEnd::desc()))) return;

	CAttrDataClass newValue;
	newValue.m_fid = -1;
	newValue.m_lCadId = dbObj->id().asOldId();

	dataContainer.m_mapNewData.SetAt(dbObj->id().asOldId(),newValue);
}

void SHDatabaseReactor::objectOpenedForModify (const AcDbDatabase* dwg, const AcDbObject* dbObj)
{
	AcDbEntity *pEnt = NULL;
	pEnt = AcDbEntity::cast(dbObj); 
	if(pEnt == NULL) return;
	if((pEnt->isKindOf(AcDbVertex::desc()))||(pEnt->isKindOf(AcDbSequenceEnd::desc()))) return;

	CString strEntName = pEnt->isA()->name();
	//图层更改
	strEntityLayer = pEnt->layer();

	//使用_polygon命令生成的AcDb2dPolyline初始化成0个顶点，此时获取GripPoint会导致系统崩溃
	if(pEnt->isKindOf(AcDb2dPolyline::desc())){
		AcDb2dPolyline *p2dPline = NULL;
		p2dPline = AcDb2dPolyline::cast(pEnt);

		int nVertexNum = 0;

		AcDbObjectIterator *VertexItor;
		VertexItor = p2dPline->vertexIterator();

		for(VertexItor->start();!VertexItor->done();VertexItor->step()){
			nVertexNum ++;
		}

		if(nVertexNum == 0) return;
	}

	//图形更改
	AcDbIntArray osnapModes;
	AcDbIntArray geomIds;
	
	gripPoints.setLogicalLength(0);

	pEnt->getGripPoints(gripPoints,osnapModes,geomIds);//,AcDbGripOperations::kGripPointsOnly
}

void SHDatabaseReactor::objectModified(const AcDbDatabase* dwg, const AcDbObject* dbObj)
{
	//AfxMessageBox("modified!");
	AcDbEntity *pEnt = NULL;
	pEnt = AcDbEntity::cast(dbObj); 
	if(pEnt == NULL) return;
	if((pEnt->isKindOf(AcDbVertex::desc()))||(pEnt->isKindOf(AcDbSequenceEnd::desc()))) return;

	CString strCADLayerName=pEnt->layer();
	if((strCADLayerName.CompareNoCase("0") == 0)||
		(strCADLayerName.CompareNoCase("Defpoints") == 0)) return;

	if(gripPoints.length() == 0) return;

	//图形更改
	AcGePoint3dArray NewGripPoints;
	AcDbIntArray osnapModes;
	AcDbIntArray geomIds;
	NewGripPoints.setLogicalLength(0);

	pEnt->getGripPoints(NewGripPoints,osnapModes,geomIds);

	if(gripPoints == NewGripPoints) return;
	else{
		//获取FeatureId
		long fid = -1;
		char cSDELayerName[128];

		CStringArray aValue;
		arx_getXdata(pEnt,"ROWID_OBJS",AcDb::kDxfXdAsciiString,aValue);

		if (aValue.GetCount() > 5)
			fid = atoi(aValue[5]);

/*
		fid = GetFeatureId((AcDbObject* )dbObj,cSDELayerName);

		//没有FeatureId表示不是从数据库中查询出来的对象
		if(fid == -1) return;

		CAttrDataClass newValue;
		newValue.m_fid = fid;
		newValue.m_lCadId = dbObj->objectId().asOldId();
		newValue.m_strCADLayer = pEnt->layer();
		newValue.m_strSDELayer = cSDELayerName;
*/
		CAttrDataClass newValue;
		dataContainer.m_mapShapeEditData.Lookup(dbObj->objectId().asOldId(),newValue);

		//undo
		if(dbObj->isUndoing()){
			newValue.m_lShapeEditNum --;
		}
		else{
			newValue.m_lShapeEditNum ++;
		}

		if(newValue.m_lShapeEditNum == 0){//撤销到未调整的状态，删除记录
			dataContainer.m_mapShapeEditData.RemoveKey(dbObj->objectId().asOldId());

			return;
		}

		//保存SDE属性信息
		if(!dataContainer.m_mapNewData.Lookup(dbObj->objectId().asOldId(),newValue))
		{  
			newValue.m_fid = fid;
			newValue.m_strCADLayer = pEnt->layer();
			dataContainer.m_mapShapeEditData.SetAt(dbObj->objectId().asOldId(),newValue);
		}
	}
}

void SHDatabaseReactor::objectErased(const AcDbDatabase* dwg, const AcDbObject* dbObj, Adesk::Boolean pErased)
{
	if(dbObj->isKindOf(AcDbBlockTableRecord::desc())){
		//需要遍历删除表内的对象
		AcDbBlockTableRecord* pBlkTblRec = AcDbBlockTableRecord::cast(dbObj);
		AcDbBlockTableRecordIterator* pIter;
		pBlkTblRec->newIterator(pIter);

		for(pIter->start();!pIter->done();pIter->step()){
			AcDbObjectId entId;
			pIter->getEntityId(entId);

			dataContainer.m_mapNewData.RemoveKey(entId.asOldId());
			dataContainer.m_mapShapeEditData.RemoveKey(entId.asOldId());
			dataContainer.m_mapAttrEditData.RemoveKey(entId.asOldId());
		}

		delete pIter;
	}

	AcDbEntity *pEnt = NULL;
	pEnt = AcDbEntity::cast(dbObj);

	if(pEnt == NULL) return;
	if((pEnt->isKindOf(AcDbVertex::desc()))||
		(pEnt->isKindOf(AcDbSequenceEnd::desc()))||
		(pEnt->isKindOf(AcDbHatch::desc()))) return;

	//undo
	if(dbObj->isUndoing()){
		//还原新增的对象 
		if(dbObj->isErased() == Adesk::kTrue){
			dataContainer.m_mapNewData.RemoveKey(dbObj->objectId().asOldId());

			return;
		}

	}

	// 新增对象
	CAttrDataClass newValue;
	if(dataContainer.m_mapNewData.Lookup(dbObj->objectId().asOldId(),newValue))
	{
		if (pErased == Adesk::kTrue)
		{
			dataContainer.m_mapNewData.RemoveKey(dbObj->objectId().asOldId());
			return;
		}
	}

	//获取FeatureId
	long fid = -1;
	CStringArray aValue;
	arx_getXdata(pEnt,"ROWID_OBJS",AcDb::kDxfXdAsciiString,aValue);

	if (aValue.GetCount() > 5)
		fid = atoi(aValue[5]);

	if (fid < 0) return;

/*
	fid = GetFeatureId((AcDbObject* )dbObj,cSDELayerName);

	//没有FeatureId表示不是从数据库中查询出来的对象
	if(fid == -1) return;
*/
	if(pErased == Adesk::kTrue)
	{
		newValue.m_fid = fid;
		newValue.m_strCADLayer = pEnt->layer();
		dataContainer.m_mapDelData.SetAt(dbObj->objectId().asOldId(),newValue);
	}
	else
	{
		dataContainer.m_mapDelData.RemoveKey(dbObj->objectId().asOldId());
	}

}

//========================================================================================
// (C) Copyright 2002-2003 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- SHDatabaseReactor.cpp : Implementation of SHEntityReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "SHDatabaseReactor.h"

//-----------------------------------------------------------------------------
ACRX_CONS_DEFINE_MEMBERS(SHEntityReactor, AcDbEntityReactor, 1)

//-----------------------------------------------------------------------------
SHEntityReactor::SHEntityReactor () : AcDbEntityReactor() {
}

//-----------------------------------------------------------------------------
SHEntityReactor::~SHEntityReactor () {
}

void SHEntityReactor::modifiedGraphics (const AcDbEntity* dbObj)
{
	AfxMessageBox("Graphics");
}

void SHEntityReactor::openedForModify (const AcDbEntity* dbObj)
{
	AfxMessageBox("openedForModify");
	strEntityLayer = dbObj->layer();
}

void SHEntityReactor::modified (const AcDbEntity* dbObj)
{
	AfxMessageBox("modified");
	if(strEntityLayer.CompareNoCase(dbObj->layer()) != 0){
		AfxMessageBox("changeLayer");
	}
}

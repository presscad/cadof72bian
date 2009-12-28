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

//д��չ����
Acad::ErrorStatus arx_setXdata( AcDbObjectId idObj , const char *cAppName , int iDxf , CStringArray& aValue)
{
	// 1���ʵ����������
	LockDocument _lock;

	AcDbObject *pObj ;
	Acad::ErrorStatus es ;

	es = acdbOpenObject( pObj , idObj , AcDb::kForRead );
	if ( es != Acad::eOk )
	{
		CString strErrMessage;

		strErrMessage.Format("\n������Ϣ��ʾ��%s" , acadErrorStatusText( es ));

		AfxMessageBox(strErrMessage);
		return es ;
	}

	struct resbuf *pRb , *pTemp ;
	pRb = pObj->xData( cAppName );

	// 2��������
	if ( pRb == NULL )
	{
		// ע���µ�Ӧ�ó�����
		int err = acdbRegApp( cAppName );
		pRb = acutNewRb( AcDb::kDxfRegAppName );
		pTemp = pRb ;
		pTemp->resval.rstring = ( char* )malloc( strlen( cAppName ) + 1 );
		strcpy( pTemp->resval.rstring , cAppName );
	}
	else
	{
		pTemp = pRb ; 
		//// ��ָ��ָ���β
		//for ( pTemp = pRb ; pTemp->rbnext != NULL ; pTemp = pTemp->rbnext )
	}


	for(int i = 0; i < aValue.GetCount(); i ++){
		CString ValueTmp = aValue.GetAt(i) ;

		if(pTemp->rbnext == NULL){
			// 3����µ��ַ���
			pTemp->rbnext = acutNewRb( iDxf );
		}

		pTemp = pTemp->rbnext ;
		pTemp->resval.rstring = ( char* )malloc( strlen( ValueTmp ) + 1 );
		strcpy(pTemp->resval.rstring,ValueTmp) ;
	}

	// 4������������
	es = pObj->upgradeOpen();

	if(es != Acad::eOk){
		pObj->close();
		es = pObj->upgradeOpen();
	}

	if(es == Acad::eOk){
		pObj->setXData( pRb );
	}
	else{
		AfxMessageBox("д����չ���ݳ���");
	}

	// 5�رն����ͷſռ䣬������Ϣ
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
	//ͼ�����
	strEntityLayer = pEnt->layer();

	//ʹ��_polygon�������ɵ�AcDb2dPolyline��ʼ����0�����㣬��ʱ��ȡGripPoint�ᵼ��ϵͳ����
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

	//ͼ�θ���
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

	//ͼ�θ���
	AcGePoint3dArray NewGripPoints;
	AcDbIntArray osnapModes;
	AcDbIntArray geomIds;
	NewGripPoints.setLogicalLength(0);

	pEnt->getGripPoints(NewGripPoints,osnapModes,geomIds);

	if(gripPoints == NewGripPoints) return;
	else{
		//��ȡFeatureId
		long fid = -1;
		char cSDELayerName[128];

		CStringArray aValue;
		arx_getXdata(pEnt,"ROWID_OBJS",AcDb::kDxfXdAsciiString,aValue);

		if (aValue.GetCount() > 5)
			fid = atoi(aValue[5]);

/*
		fid = GetFeatureId((AcDbObject* )dbObj,cSDELayerName);

		//û��FeatureId��ʾ���Ǵ����ݿ��в�ѯ�����Ķ���
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

		if(newValue.m_lShapeEditNum == 0){//������δ������״̬��ɾ����¼
			dataContainer.m_mapShapeEditData.RemoveKey(dbObj->objectId().asOldId());

			return;
		}

		//����SDE������Ϣ
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
		//��Ҫ����ɾ�����ڵĶ���
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
		//��ԭ�����Ķ��� 
		if(dbObj->isErased() == Adesk::kTrue){
			dataContainer.m_mapNewData.RemoveKey(dbObj->objectId().asOldId());

			return;
		}

	}

	// ��������
	CAttrDataClass newValue;
	if(dataContainer.m_mapNewData.Lookup(dbObj->objectId().asOldId(),newValue))
	{
		if (pErased == Adesk::kTrue)
		{
			dataContainer.m_mapNewData.RemoveKey(dbObj->objectId().asOldId());
			return;
		}
	}

	//��ȡFeatureId
	long fid = -1;
	CStringArray aValue;
	arx_getXdata(pEnt,"ROWID_OBJS",AcDb::kDxfXdAsciiString,aValue);

	if (aValue.GetCount() > 5)
		fid = atoi(aValue[5]);

	if (fid < 0) return;

/*
	fid = GetFeatureId((AcDbObject* )dbObj,cSDELayerName);

	//û��FeatureId��ʾ���Ǵ����ݿ��в�ѯ�����Ķ���
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

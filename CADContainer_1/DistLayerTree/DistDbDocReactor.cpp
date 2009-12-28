#include "stdafx.h"
#include "sorttab.h"
#include "DistDbDocReactor.h"

bool getOrCreateSortEntsDictionary(AcDbSortentsTable *&pSortTab)
{
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	if(pDb == NULL) return false;
	
	AcDbBlockTable *pBT	= NULL;
	AcDbBlockTableRecord *pBTR	= NULL;
	Acad::ErrorStatus es;
	
	//获取块表指针
	if (Acad::eOk != pDb->getBlockTable(pBT,AcDb::kForRead))
	{
		acutPrintf("\nCouldn't get the block table"); return false;
	}

	acDocManager->lockDocument(acDocManager->document(pDb));
	//获取模型空间表记录指针
	if (Acad::eOk != pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForRead))
	{
		acutPrintf("\nCouldn't get the MODEL_SPACE block record opened");
		pBT->close();
		return false;

	}
	pBT->close();

	//获取图层排序表指针
	if (Acad::eOk !=pBTR->getSortentsTable(pSortTab, AcDb::kForWrite, true))
	{
		acutPrintf("\nCouldn't get or create AcDbSortEntsTable of Model Space");
		pBTR->close();
		return false;
	}
	pBTR->close();

	

	return true;
}


//put the Ids of all the entities on a certain layer in the array, "ents"
bool getAllEntitiesOnLayer(char* layerName, AcDbObjectIdArray &ents)
{
	//设置过虑器
	struct resbuf filter; 
	char strLyNameBuf[260]={0}; 
	filter.restype = 8;
	strcpy(strLyNameBuf, layerName); 
	filter.resval.rstring = strLyNameBuf; 
	filter.rbnext = NULL;
	
	//查询指定图层所有实体
	ads_name ss;
	if (RTNORM != acedSSGet("X",NULL,NULL,&filter,ss)) 
		return false;
	long nEnts=0;
	if(RTNORM != acedSSLength(ss, &nEnts ))
		return false;
	ents.setPhysicalLength(nEnts);
	for(int i = 0; i < nEnts; i++ ) {
		ads_name eName;
		if(RTNORM != acedSSName(ss, i, eName)) continue;
		AcDbObjectId id;
		if(Acad::eOk == acdbGetObjectId(id, eName))
			ents.append(id);
	}
	acedSSFree( ss );
	
	return true;
}


//Put the Ids of all the layers in the layer table in array, "arrLayers" 
bool getAllLayers(AcDbObjectIdArray &arrLayers)
{	
	arrLayers.setPhysicalLength(0);
	AcDbLayerTable *pLayerTable = NULL ;    
	Acad::ErrorStatus es ;   

	if (Acad::eOk !=  acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForRead))
	{       
		acutPrintf ("\nCouldn''t get the layer table"); return false;    
	}   
	
	AcDbLayerTableIterator *pLayerTableIterator = NULL ;   
	if (Acad::eOk != pLayerTable->newIterator (pLayerTableIterator))
	{       
		acutPrintf ("\nCouldn''t get a new layer table iterator") ;        
		pLayerTable->close () ;       
		return false ;    
	}    
	pLayerTable->close () ;  


	AcDbObjectId layerId ;    
	for (pLayerTableIterator->start();!pLayerTableIterator->done ();pLayerTableIterator->step()) 
	{        
		if (Acad::eOk != pLayerTableIterator->getRecordId(layerId)) continue;
		arrLayers.append(layerId);
	}
	delete pLayerTableIterator;
	
	return true;
}




ACRX_CONS_DEFINE_MEMBERS(adskMyDbReactor, AcDbDatabaseReactor, 1)

bool gbDraworderChanged = false;


//-----------------------------------------------------------------------------
adskMyDbReactor::adskMyDbReactor (AcDbDatabase *pDb) : AcDbDatabaseReactor(), mpDatabase(pDb) 
{
	if (NULL != pDb)
		pDb->addReactor (this) ;
}


//-----------------------------------------------------------------------------
adskMyDbReactor::~adskMyDbReactor () {
	Detach () ;
}


//-----------------------------------------------------------------------------
void adskMyDbReactor::Attach (AcDbDatabase *pDb) {
	Detach () ;
	if ( mpDatabase == NULL ) {
		if ( (mpDatabase = pDb) != NULL )
			pDb->addReactor (this) ;
	}
}


void adskMyDbReactor::Detach () {
	if ( mpDatabase ) {
		mpDatabase->removeReactor (this) ;
		mpDatabase = NULL ;
	}
}


AcDbDatabase *adskMyDbReactor::Subject () const {
	return (mpDatabase) ;
}


bool adskMyDbReactor::IsAttached () const {
	return (mpDatabase != NULL) ;
}


//When an new entity is added, set its draw order based on the draw order of other entities on the same layer.
void
adskMyDbReactor::objectAppended(const AcDbDatabase* db, const AcDbObject* pObj)
{
	AcDbEntity * pEnt = AcDbEntity::cast(pObj);
	Acad::ErrorStatus es;
	if (pEnt != NULL)
	{
		AcDbBlockTable *pBT = NULL;
		AcDbBlockTableRecord *pBTR	= NULL;
		
		//use transaction since the datbase is still open for write here.
		es=actrTransactionManager->enableGraphicsFlush(true);
		AcTransaction *pTrans = actrTransactionManager->startTransaction() ;
		
		if(NULL == pTrans)
		{
			acutPrintf("\nCan't start transaction!"); return;
		}
		AcDbObject *pObj = NULL ;
		AcDbDatabase *pDb = NULL;
		AcDbObjectId modelSpaceId;
		pDb = AcDbDatabase::cast(db);
		
		//get AcDbBlockTable for read
		if(Acad::eOk != pDb->getBlockTable(pBT, AcDb::kForRead))
		{
			acutPrintf("can't open block table for read");
			actrTransactionManager->abortTransaction();
			return;
		}
		
		//get the model space object id
		if(Acad::eOk != pBT->getAt(ACDB_MODEL_SPACE, modelSpaceId))
		{
			acutPrintf("\ncan't get model space Id");
			actrTransactionManager->abortTransaction();
			pBT->close();
			return;
		}
		
		pBT->close();
		
		
		//get model space block record for write from transaction
		if (Acad::eOk != pTrans->getObject((AcDbObject*&)pBTR, modelSpaceId, AcDb::kForWrite))
		{
			acutPrintf("\ncan't open model space block table record for write");
			actrTransactionManager->abortTransaction();
			return;
		}
		
		AcDbObjectIdArray eIds;
		
		//get AcDbSortEntsTable
		AcDbSortentsTable *pSortTab = NULL;
		if(Acad::eOk != pBTR->getSortentsTable(pSortTab, AcDb::kForWrite, false))
		{
			acutPrintf("\ncan't get AcDbSortEntsTable for write");
			pBTR->close();
			actrTransactionManager->abortTransaction(); return;
		}
		pBTR->close();

		//put objectIds of all the entities in an Id array.The order of objectIds in the array is the
		//same as their draworders from bottom to top.The newly created entity is always at the top and the last item
		//in the array
		if(Acad::eOk != pSortTab->getFullDrawOrder(eIds))
		{
			acutPrintf("\ncan't get full draworder");
			pSortTab->close();
			actrTransactionManager->abortTransaction();
			return;
		}
		AcDbEntity *pRefEnt = NULL;
		//iterate through the entities in the order of their draworders. If an entity on the same layer is found
		//insert the newly created entity before it.
		int i;
		for(i =0; i<eIds.length(); i++)
		{
			es = pTrans->getObject((AcDbObject*&)pRefEnt, eIds.at(i), AcDb::kForRead);
			if(pRefEnt->layerId() == pEnt->layerId())
				break;
		}	
		eIds.insertAt(i, pEnt->objectId());
		//remove the newly created entity from the end of the array
		eIds.removeLast();
		//reset draworder
		es = pSortTab->setRelativeDrawOrder(eIds);
		pSortTab->close();
		es=actrTransactionManager->endTransaction();
		actrTransactionManager->flushGraphics();

		//set flag for regen
		gbDraworderChanged = true;	
	}

}



//////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
ACRX_CONS_DEFINE_MEMBERS(adskDocReactor, AcApDocManagerReactor, 1)

//-----------------------------------------------------------------------------
adskDocReactor::adskDocReactor (const bool autoInitAndRelease) : AcApDocManagerReactor(), mbAutoInitAndRelease(autoInitAndRelease) {
	if ( autoInitAndRelease ) {
		if ( acDocManager )
			acDocManager->addReactor (this) ;
		else
			mbAutoInitAndRelease =false ;
	}
	
}

//-----------------------------------------------------------------------------
adskDocReactor::~adskDocReactor () {
	Detach () ;
}

//-----------------------------------------------------------------------------
void adskDocReactor::Attach () {
	Detach () ;
	if ( !mbAutoInitAndRelease ) {
		if ( acDocManager ) {
			acDocManager->addReactor (this) ;
			mbAutoInitAndRelease =true ;
		}
	}
}

void adskDocReactor::Detach () {
	if ( mbAutoInitAndRelease ) {
		if ( acDocManager ) {
			acDocManager->removeReactor (this) ;
			mbAutoInitAndRelease =false ;
		}
	}
}

AcApDocManager *adskDocReactor::Subject () const {
	return (acDocManager) ;
}

bool adskDocReactor::IsAttached () const {
	return (mbAutoInitAndRelease) ;
}  



//If the draworder has been changed, regen the drawing 
//
void  adskDocReactor::documentLockModeChanged(AcApDocument* pDoc, AcAp::DocLockMode myPreviousMode, 
			AcAp::DocLockMode myCurrentMode, AcAp::DocLockMode currentMode, const char* pGlobalCmdName)
{
	if(pGlobalCmdName[0]=='#'&& gbDraworderChanged == true)
	{
		ads_regen();
		acutPrintf("regened change mode");
		gbDraworderChanged = false;
	}
}
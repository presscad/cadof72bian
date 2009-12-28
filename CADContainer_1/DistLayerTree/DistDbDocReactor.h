#include "stdafx.h"
#include "dbmain.h"


const int kNameLength = 260;
//ARX 2005
//bool getOrCreateSortEntsDictionary(AcDbSortentsTable *&pSortTab);
bool getAllEntitiesOnLayer(char* layerName, AcDbObjectIdArray &ents);
bool getAllLayers(AcDbObjectIdArray &arrLayers);




class adskMyDbReactor : public AcDbDatabaseReactor {

public:
	ACRX_DECLARE_MEMBERS(adskMyDbReactor) ;

protected:
	//----- Pointer to the document this reactor instance belongs to.
	AcDbDatabase *mpDatabase ;

public:
	adskMyDbReactor (AcDbDatabase *pDb =NULL) ;
	virtual ~adskMyDbReactor () ;

	virtual void Attach (AcDbDatabase *pDb) ;
	virtual void Detach () ;
	virtual AcDbDatabase *Subject () const ;
	virtual bool IsAttached () const ;
	virtual void objectAppended(const AcDbDatabase* db, const AcDbObject* pObj);

} ;

#ifdef DRAWORDERSAMP_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(adskMyDbReactor)
#endif



class  adskDocReactor : public AcApDocManagerReactor {

public:
	ACRX_DECLARE_MEMBERS(adskDocReactor) ;

protected:
	//----- Auto initialization and release flag.
	bool mbAutoInitAndRelease ;

public:
	adskDocReactor (const bool autoInitAndRelease =true) ;
	virtual ~adskDocReactor () ;
	virtual void Attach () ;
	virtual void Detach () ;
	virtual AcApDocManager *Subject () const ;
	virtual bool IsAttached () const ;
	virtual void documentLockModeChanged(AcApDocument* pDoc,
							 AcAp::DocLockMode myPreviousMode,
							 AcAp::DocLockMode myCurrentMode,
							 AcAp::DocLockMode currentMode,
                             const char* pGlobalCmdName);


} ;

#ifdef DRAWORDERSAMP_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(adskDocReactor)
#endif
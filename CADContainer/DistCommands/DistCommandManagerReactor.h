//-----------------------------------------------------------------------------
//----- DistCommandManagerReactor.h : Declaration of the DistCommandManagerReactor
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------
#include "acdocman.h"

//-----------------------------------------------------------------------------
//----- Note: Uncomment the DLLIMPEXP symbol below if you wish exporting
//----- your class to other ARX/DBX modules
class /*DLLIMPEXP*/ DistCommandManagerReactor : public AcApDocManagerReactor {

protected:
	//----- Auto initialization and release flag.
	bool mbAutoInitAndRelease ;

public:
	DistCommandManagerReactor (const bool autoInitAndRelease =true) ;
	virtual ~DistCommandManagerReactor () ;

	//{{AFX_ARX_METHODS(ShCommandManagerReactor)
	virtual void documentActivated(AcApDocument* pActivatedDoc);

	virtual void documentLockModeWillChange(AcApDocument* x0,
		AcAp::DocLockMode myCurrentMode,
		AcAp::DocLockMode myNewMode,
		AcAp::DocLockMode currentMode,
		const char* pGlobalCmdName);
	virtual void documentLockModeChangeVetoed(AcApDocument* x0,
		const char* pGlobalCmdName);
	virtual void documentLockModeChanged(AcApDocument* x0,
		AcAp::DocLockMode myPreviousMode,
		AcAp::DocLockMode myCurrentMode,
		AcAp::DocLockMode currentMode,
		const char* pGlobalCmdName);
	//}}AFX_ARX_METHODS

} ;

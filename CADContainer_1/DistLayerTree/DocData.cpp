#include "StdAfx.h"

//-----------------------------------------------------------------------------
//----- The one and only document manager object. You can use the DocVars object to retrieve
//----- document specific data throughout your application
AcApDataManager<CDocData> DocVars ;

//-----------------------------------------------------------------------------
//----- Implementation of the document data class.
CDocData::CDocData () {
	//{{AFX_ARX_DATA_INIT(CDocData)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_ARX_DATA_INIT

	// TODO: add your own initialization.
}

//-----------------------------------------------------------------------------
CDocData::CDocData (const CDocData &data) {
	//{{AFX_ARX_DATA_INIT_COPY(CDocData)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_ARX_DATA_INIT_COPY

	// TODO: add your own initialization.
}

//-----------------------------------------------------------------------------
CDocData::~CDocData () {
	//{{AFX_ARX_DATA_DEL(CDocData)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_ARX_DATA_DEL

	// TODO: clean up.
}

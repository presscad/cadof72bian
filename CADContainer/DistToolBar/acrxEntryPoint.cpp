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
//----- acrxEntryPoint.h
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "DistToolbarManager.h"

#include "DistToolBar_Ext.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

DISTTOOLBAR_EXT BOOL Sys_LoadDistToolBar(LPCTSTR lpXml)
{
	LoadToolBars(lpXml);
	return TRUE;
}

DISTTOOLBAR_EXT BOOL Sys_UnLoadDistToolBar()
{
	CleanToolbars();
	return TRUE;
}

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CDistToolBarApp : public AcRxArxApp {

public:
	CDistToolBarApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		// TODO: Add your initialization code here

//		OutputDebugStringX("XXXXXXXXXXXXXXX :: º”‘ÿDistToolBar.arx :: %d:%d:%d", CTime::GetCurrentTime().GetHour(), CTime::GetCurrentTime().GetMinute(), CTime::GetCurrentTime().GetSecond());

		CreateToolbars();

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;
		// TODO: Add your code here
		CleanToolbars();

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CDistToolBarApp)


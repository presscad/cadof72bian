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
//----- DocData.cpp : Implementation file
//-----------------------------------------------------------------------------
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
	//m_strProjectCode="";
	m_strProjectId="";
	m_strFileId="";
}

//-----------------------------------------------------------------------------
CDocData::~CDocData () {
	//{{AFX_ARX_DATA_DEL(CDocData)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_ARX_DATA_DEL

	// TODO: clean up.
}

//// 设置项目编号
//void CDocData::SetProjectCode(CString strProjectCode)
//{
//	m_strProjectCode = strProjectCode;
//}
//
//// 获取项目编号
//CString CDocData::GetProjectCode()
//{
//	return m_strProjectCode;
//}

// 设置项目Id
void CDocData::SetProjectId(CString strProjectId)
{
	m_strProjectId = strProjectId;
}
// 获取项目Id
CString CDocData::GetProjectId()
{
	return m_strProjectId;
}

// 设置文件Id
void CDocData::SetFileId(CString strFileId)
{
	m_strFileId = strFileId;
}
// 获取文件Id
CString CDocData::GetFileId()
{
	return m_strFileId;
}

void CDocData::SetFileName(CString strFileName)
{
	m_strFileName = strFileName;
}

CString CDocData::GetFileName()
{
	return m_strFileName;
}


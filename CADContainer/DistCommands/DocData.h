#include <afxtempl.h>
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
//----- DocData.h : include file for document specific data. An instance of this
//----- class is automatically created and managed by the AsdkDataManager class.
//----- See the AsdkDmgr.h / DocData.cpp for more datails
//-----------------------------------------------------------------------------
#pragma once

//��dwgͼ�����ƣ�ȥ����׺����Ϊ�ؼ��֣������Ӧ�Ĳ��뵱ǰͼ��Ŀ�����Id
typedef CMap<CString,LPCSTR,AcDbObjectId,AcDbObjectId&> mapBlockObjectId;

typedef CMap<CString,LPCTSTR,CString,LPCTSTR> mapStringToString;

//-----------------------------------------------------------------------------
//----- Here you can store the document / database related data.
class CDocData {
public:
	CDocData () ;
	CDocData (const CDocData &data) ;
	~CDocData () ;

	//{{AFX_ARX_DATA(CDocData)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_ARX_DATA

public:
	//// ������Ŀ���
	//void SetProjectCode(CString strProjectCode);
	//// ��ȡ��Ŀ���
	//CString GetProjectCode();

	mapBlockObjectId m_mapBlockIds;//�Ѳ������ͼͼ���ID

	mapStringToString m_mapBizzInfo;

	// ������ĿId
	void SetProjectId(CString strProjectId);
	// ��ȡ��ĿId
	CString GetProjectId();

	// �����ļ�Id
	void SetFileId(CString strFileId);
	// ��ȡ�ļ�Id
	CString GetFileId();

	void SetFileName(CString strFileName);
	CString GetFileName();

	AcDbObjectIdArray m_LxObjIdArr;

protected:
	//CString m_strProjectCode; // ��Ŀ���
	CString m_strProjectId;		// ��ĿID
	CString m_strFileId;		// �ļ�ID
	CString m_strFileName;

	//----- TODO: here you can add your variables
} ;

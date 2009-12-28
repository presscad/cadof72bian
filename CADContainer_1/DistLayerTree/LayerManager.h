#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "DlgToolTree.h"

//-----------------------------------------------------------------------------
class CLayerManager : public CAcUiDockControlBar {
	DECLARE_DYNAMIC (CLayerManager)

public:
	//----- Child dialog which will use the resource id supplied
	CDlgToolTree *mpChildDlg ;

public:
	CLayerManager ();
	virtual ~CLayerManager ();
	virtual BOOL Create (CWnd *pParent, LPCTSTR lpszTitle) ;
	BOOL ShowDockPane(BOOL bShow = TRUE) ;

	// �洢��ǰ��ѡ�е�SDEͼ����
	CStringArray m_SdeLayer;

protected:
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) ;

	afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct) ;
	afx_msg void OnSysCommand (UINT nID, LPARAM lParam) ;
	afx_msg void OnSize (UINT nType, int cx, int cy) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
} ;

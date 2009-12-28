#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
#include "LayerTreeDlg.h"

//-----------------------------------------------------------------------------
class CLayerManager : public CAcUiDockControlBar {
	DECLARE_DYNAMIC (CLayerManager)

public:
	//----- Child dialog which will use the resource id supplied
	CLayerTreeDlg * m_pLayerTreeDlg;
	long m_nType; // nType： 项目ID 六线调整为0,电子展板为-1

public:
	CLayerManager ();
	virtual ~CLayerManager ();
	virtual BOOL Create (CWnd *pParent, LPCTSTR lpszTitle, long nType) ;
	BOOL ShowDockPane(BOOL bShow = TRUE) ;

protected:
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) ;
	virtual BOOL AddCustomMenuItems(LPARAM hMenu);

	afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct) ;
	afx_msg void OnSysCommand (UINT nID, LPARAM lParam) ;

	DECLARE_MESSAGE_MAP()
} ;

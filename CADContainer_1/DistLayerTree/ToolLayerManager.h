
#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"
//#include "LayerManagerChildDlg.h"

//-----------------------------------------------------------------------------
class CToolLayerManager : public CAcUiDockControlBar {
	DECLARE_DYNAMIC (CToolLayerManager)

public:
	//----- Child dialog which will use the resource id supplied
	//CLayerManagerChildDlg *mpChildDlg ;



public:
	         CToolLayerManager ();
	virtual ~CToolLayerManager ();
	virtual BOOL Create (CWnd *pParent, LPCTSTR lpszTitle) ;
	BOOL ShowDockPane(BOOL bShow = TRUE) ;

	// 存储当前被选中的SDE图层名
	CStringArray m_SdeLayer;

protected:
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) ;

	afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct) ;
	afx_msg void OnSysCommand (UINT nID, LPARAM lParam) ;
	afx_msg void OnSize (UINT nType, int cx, int cy) ;

	DECLARE_MESSAGE_MAP()
public:
	void GetSdeLayerName();

	void RedrawTreeWindow();
} ;

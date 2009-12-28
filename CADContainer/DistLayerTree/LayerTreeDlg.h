#pragma once

#include "acui.h"
#include "resource.h"
#include "XmlDocument.h"
#include "AcToolbar.h"	// Added by ClassView

//-----------------------------------------------------------------------------
class CLayerTreeDlg : public CAcUiDialog 
{
	DECLARE_DYNAMIC (CLayerTreeDlg)

public:
	CLayerTreeDlg (CWnd *pParent =NULL, HINSTANCE hInstance =NULL) ;
	enum { IDD = IDD_LAYERTREEDLG} ;

	virtual BOOL OnInitDialog();
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) ;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg BOOL NotifyFunction( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg LRESULT OnAcadKeepFocus (WPARAM, LPARAM) ;
	afx_msg void OnSize (UINT nType, int cx, int cy) ;
	afx_msg void OnDestroy();
	afx_msg void OnNMClickLayerTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSave();

	void OnTvSelChanged(NMHDR* pNMHDR);
	void OnTvDblClk(NMHDR* pNMHDR);
	void OnTvRClick(NMHDR* pNMHDR);

	BOOL SetCurrentLayer(CString strCADLayer);

	BOOL LoadXmlFromFile(LPCTSTR lpFile);
	BOOL LoadXmlFromBuffer(LPCTSTR lpBuffer);
	BOOL LoadXmlSubTree(CXmlElement *pElement, HTREEITEM hItem, BOOL bChecked = FALSE);

	void GetCheckedLayers(CStringArray& arrayLayers);
	void EnumCheckItems(HTREEITEM hItem, CStringArray& arrayLayers);

	void SetCheckedLayers(CStringArray& arrayLayers);
	void SetCheckItems(HTREEITEM hItem, CStringArray& arrayLayers);

	void ExpandTreeItem(HTREEITEM hItem);

	void SetTreeCheck(HTREEITEM hTI = TVI_ROOT, BOOL bCheck = TRUE);

	void ShowLayer(HTREEITEM hItem, BOOL bShow);
	void ShowLayer(CString strLayerName, BOOL bShow);

protected:
	virtual void DoDataExchange (CDataExchange *pDX) ;
	virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam) ;

public:
	//CToolBarCtrl m_wndtoolbarctrl;
	CImageList m_imagelist;
	CTreeCtrl  m_tree;
//	CXHtmlTree  m_tree;
	CXmlDocument  m_xmlDocument;
	long m_nType;
	CAcToolBar m_wndToolBar;

private:
	DECLARE_MESSAGE_MAP()
} ;

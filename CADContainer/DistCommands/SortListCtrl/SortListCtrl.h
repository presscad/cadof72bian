#ifndef SORTLISTCTRL_H
#define SORTLISTCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef SORTHEADERCTRL_H
	#include "SortHeaderCtrl.h"
#endif	// SORTHEADERCTRL_H

#include "EditCell.h"
#include "NumEdit.h"
#include "NewComboBox.h"

#include <afxtempl.h>

#ifdef _DEBUG
#define ASSERT_VALID_STRING( str ) ASSERT( !IsBadStringPtr( str, 0xfffff ) )
#else	//	_DEBUG
#define ASSERT_VALID_STRING( str ) ( (void)0 )
#endif	//	_DEBUG

#define WM_ON_CHKBOX	   (WM_APP + 1000)
#define WM_ON_ENDEDIT	   (WM_APP + 1001)
#define WM_ON_ADDFIELD	   (WM_APP + 1002)
#define WM_ON_MODIFYFIELD  (WM_APP + 1003)
#define WM_ON_DELETEFIELD  (WM_APP + 1004)
#define WM_ON_BEGINEDIT	   (WM_APP + 1005)

#define COLOR_INVALID	0xffffffff

#define ID_EDIT_INLISTCTRL 7000


class CEditCell;
class CNumEdit;
class CMaskEdit;

typedef struct tagCODEDOMAIN
{
	CString  domainValue;
	CString  domainName;
}XCodeDomain;

//typedef vector<XCodeDomain> VectCodeDomain;
typedef CTypedPtrArray<CPtrArray,XCodeDomain*> CArrayCodeDomain;

class CSortListCtrl : public CListCtrl
{
	// Construction
public:

	CSortListCtrl();

public:

	virtual ~CSortListCtrl();
	
// Stander Operations
public:

	void RefreshSortCtrl();


	DWORD SetExtendedStyle(DWORD dwNewStyle);

	BOOL  ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags);

	BOOL  ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags);
		

	void SetGridLines(BOOL bSet = TRUE);

	void SetCheckboxeStyle(BOOL bCheckBox = TRUE);

	void SetFullSelectedStyle(BOOL bFullSelect = TRUE);

	BOOL SetSortable(BOOL bSet = TRUE);
		
	

	BOOL SetHeadings( UINT uiStringID );

	BOOL SetHeadings( const CString& strHeadings );
	

	int AddItem( long nType, LPCTSTR pszText, ... );

	int AddItem(CStringList &listText);
		

	BOOL DeleteItem( int iItem );

	BOOL DeleteAllItems();


	int GetColumnCount() const;

	int AddColumn(LPCTSTR lpszColumnHeading, LPTSTR lpszDefaultValue = NULL,
					int iDefaultValueSize = 0,int nFormat = LVCFMT_LEFT,
					int nWidth = -1, int nSubItem = -1  );

	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, LPTSTR lpszDefaultValue = NULL,
					int iDefaultValueSize = 0,int nFormat = LVCFMT_LEFT, 
					int nWidth = -1, int nSubItem = -1 );
	

	int InsertColumn( int nCol, const LVCOLUMN* pColumn ,
		             LPTSTR lpszDefaultValue = NULL,int iDefaultValueSize = 0);
	

	void ModifyColumnText(int nCol, LPCTSTR lpszColumnHeading,int iHeadCaptionSize);
	

	BOOL DeleteColumn( int nCol ,BOOL bDeleteAll = FALSE );	

	void DeleteAllColumn();	
	

	BOOL SetItemText( int nItem, int nSubItem, LPCTSTR lpszText );

	BOOL SetItemTextColor(int nItem=-1, int nSubItem=-1,COLORREF textColor=COLOR_INVALID, BOOL bRedraw = TRUE);

	BOOL GetItemTextColor(int nItem, int nSubItem,COLORREF& textColor);

	BOOL SetItemBkColor(int nItem=-1, int nSubItem=-1,COLORREF bkColor=COLOR_INVALID, BOOL bRedraw = TRUE);
	

	void SetColumnSelectedTextColor(COLORREF crColor);

	COLORREF GetColumnSelectedTextColor();


	void SetColumnSelectedBkColor(COLORREF crColor);

	COLORREF GetColumnSelectedBkColor();
	

	void SetItemSelectedTextColor(COLORREF crColor);

	COLORREF GetItemSelectedTextColor();
	

	void SetItemSelectedBkColor(COLORREF crColor);

	COLORREF GetItemSelectedBkColor();


	void SetHGridLineColor(COLORREF crColor);

	COLORREF GetHGridLineColor();

	void SetVGridLineColor(COLORREF crColor);

	COLORREF GetVGridLineColor();
	

	void RedrawCheckItem(int nItem);

	void RedrawUnCheckItem(int nItem);
		
private:

	COLORREF m_crTextColor; 

	COLORREF m_crTextBkColor;
	

	COLORREF m_crColumnSelectedTextColor; 

	COLORREF m_crColumnSelectedBkColor;


	COLORREF m_crItemSelectedTextColor; 

	COLORREF m_crItemSelectedBkColor;


	COLORREF m_crHGridLine;

	COLORREF m_crVGridLine;
	

	void ReSetAllText();
//Edit Operations
public:

	BOOL  StartEdit (int Item, int Column);

	BOOL  EndEdit(BOOL bCommit = TRUE);
		

    int	  HitTestEx (CPoint& Point, int* Column);
	

	void SetAllowEdit(BOOL bAllowEdit = TRUE);

	BOOL GetAllowEdit();
	

	void SetEditTextColor(COLORREF crColor);

	COLORREF GetEditTextColor();
	

	void SetEditBkColor(COLORREF crColor);

	COLORREF GetEditBkColor();
	

	const CHeaderCtrl* GetHeaderCtrl() const;	

	DWORD GetHeaderTextFormat(int nColumn) const;


	CNumEdit* GetNumEditControl();

	CMaskEdit* GetDateEditControl();

	CEditCell* GetCellEditControl();

public:
	

	typedef struct tagFieldProperty
	{
		//数据 0:char;1:long,2:double,3:date,4:enum domain
		int m_iFieldType;

		//精度
		BOOL m_bPrecision;
		int m_iPrecision;
		
		//范围
		BOOL m_bRange;
		double m_dMinValue;
		double m_dMaxValue;

		COleDateTime m_dtMinDate;
		COleDateTime m_dtMaxDate;
		
		CArrayCodeDomain *m_pArrDomain;
		CString	m_strComboContext;
		tagFieldProperty()
		{
			m_iFieldType = 0;
			m_bPrecision = 0;
			m_iPrecision = 0;
			m_bRange = FALSE; 
			m_dMinValue = 0;
			m_dMaxValue = 0;
			m_pArrDomain = NULL;
			m_strComboContext = _T("");
		}
	}FIELDPROPERTY;


	CArray<FIELDPROPERTY,FIELDPROPERTY>  m_arrFieldProperty;
	
private:

//不同的EDit

	CNumEdit *m_pWndNumEdit;

	CMaskEdit * m_pWndDateEdit;

	CEditCell * m_pWndCellEdit;

	//基类指针；

	CEdit *m_pWndEdit;

	//Domain 

	CNewComboBox *m_pWndCombo;

	BOOL m_bComboBox;	


	int m_nItem;

	int m_nSubItem;

	BOOL m_bEscape;
	

	BOOL m_bAllowEdit;

	COLORREF m_crEditTextColor;

	COLORREF m_crEditBkColor;
	

	DWORD m_dwPrevEditFmt; // Format of previously edited subitem
	
//Sort & AutoSize
public:

	void Sort( int iColumn, BOOL bAscending );

	BOOL IsAscend(int iColumn);
	

	void AutoSizeColumn();

private:

	static int CALLBACK CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData );

	void FreeItemMemory( const int iItem );
	

	BOOL CSortListCtrl::SetTextArray( int iItem, LPTSTR* arrpsz );

	CPtrList* CSortListCtrl::GetTextArray( int iItem ) const;
	

	int m_iSortColumn;

	BOOL m_bSortAscending;
	
//Freeze/Unfreeze
public:

	void FreezeColumn(const int iColumn);

	void UnfreezeColumn(const int iColumn);

	BOOL IsFreeze(const int iColumn);
	
private:

	void RemoveFieldArray();

	CArray<DWORD, DWORD> m_dwFreezeColumn; 

	CUIntArray m_arrColumnColor;	
	
//other
protected:

	CSortHeaderCtrl m_ctlHeader;

	int m_iNumColumns;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortListCtrl)
	public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);	

	virtual void PreSubclassWindow();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	
	//{{AFX_MSG(CSortListCtrl)

	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnDestroy();
//	afx_msg void OnPaint();	

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);	

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // SORTLISTCTRL_H

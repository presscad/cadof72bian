
//

/////////////////////////////////////////////////////////////////////////////
// CMaskEdit class

#ifndef _MASKDEFINE_
#define _MASKDEFINE_

class  CMaskEdit : public CEdit
{
  DECLARE_DYNAMIC(CMaskEdit)

// Constructors
public:

  CMaskEdit();
  
// Attributes

  BOOL m_bUseMask;

  BOOL m_isdate; //added this


  CString m_str; //added this

  CString m_strMask;

  CString m_strLiteral;


  CString m_strValid;


  BOOL m_bMaskKeyInProgress;  

  //Êý¾Ý·¶Î§

  BOOL m_bRange;

  COleDateTime m_dtMinDate;

  COleDateTime m_dtMaxDate;
  
// Operations

  BOOL CheckChar(UINT nChar);

  void SetMask(LPCSTR lpMask, LPCSTR lpLiteral = NULL, LPCSTR lpValid = NULL);

  void SendChar(UINT nChar);

protected:
  // Generated message map functions
  //{{AFX_MSG(CMaskEdit)

  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

  afx_msg void OnKillFocus(CWnd* pNewWnd);  
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#endif

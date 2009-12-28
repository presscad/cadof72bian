#if !defined _INPLACE_EDIT
#define _INPLACE_EDIT
// SHInPlaceEdit.h : header file
//

#define WM_ENTER_MSG WM_USER + 11

/////////////////////////////////////////////////////////////////////////////
// CSHInPlaceEdit window

class CSHInPlaceEdit : public CEdit
{
	// Construction
public:
	CSHInPlaceEdit(int iItem, int iSubItem);
	
	// Attributes
public:
	enum {VALID = 0x00, OUT_OF_RANGE = 0x01, INVALID_CHAR = 0x02, MINUS_PLUS = 0x03, DOT = 0x04};
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSHInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
//	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	void SetButtonCtrl(CButton* pBtn);
	virtual void ChangeAmount(int step);
	virtual float GetDelta();
	virtual void SetDelta(float delta);
	virtual void GetRange(float &min, float &max)const;
	virtual void SetRange(float min, float max);
	virtual void Verbose(BOOL v);
	virtual BOOL Verbose()const;
	virtual int IsValid()const;
	virtual int IsValid(const CString &str)const;
	virtual BOOL SetValue(float val);
	virtual float GetValue()const;
	virtual ~CSHInPlaceEdit();
	void CalculateSize();
	BOOL m_bNum;
	
	// Generated message map functions
protected:
	CString& ConstructFormat(CString &str, float num);
	BYTE m_NumberOfNumberAfterPoint;
	BOOL m_Verbose;
	float m_Delta;
	float m_MinValue;
	float m_MaxValue;
	//{{AFX_MSG(CSHInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcDestroy();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CButton* m_pBtn;
	BOOL m_bEnterKeyPress;
	int m_iItem;
	int m_iSubItem;
	BOOL    m_bESC;         // To indicate whether ESC key was pressed
};

/////////////////////////////////////////////////////////////////////////////
#endif


//

#include "stdafx.h"
#include "MaskEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
#define WM_ON_ENDEDIT	   (WM_APP + 1001)

/////////////////////////////////////////////////////////////////////////////
// CMaskEdit class

IMPLEMENT_DYNAMIC(CMaskEdit, CEdit)

BEGIN_MESSAGE_MAP(CMaskEdit, CEdit)
  //{{AFX_MSG_MAP(CMaskEdit)
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_KILLFOCUS()	  
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//////////////////////////////////////////////////////////////////////////////////////////////
CMaskEdit::CMaskEdit()
{
  m_bUseMask = FALSE;
  m_isdate = FALSE;
  m_strMask = _T("");
  m_strLiteral = _T("");

  m_bMaskKeyInProgress = FALSE;
  
  m_strValid = _T("");

  //Use as DateTime
  m_isdate	     = TRUE;
  m_bUseMask       = TRUE;
  m_strMask        = "0000-00-00 00:00:00";
  m_strLiteral     = "____-__-__ __:__:__";

  COleDateTime time = COleDateTime::GetCurrentTime();
  m_str             = time.Format(_T("%Y-%m-%d %H:%M:%S"));  
}

//
//////////////////////////////////////////////////////////////////////////////////////
void CMaskEdit::SetMask(LPCSTR lpMask, LPCSTR lpLiteral, LPCSTR lpValid)
{
  m_bUseMask = FALSE;
  if (lpMask == NULL) return;

  m_strMask = lpMask;
  if (m_strMask.IsEmpty()) return;

  if (lpLiteral != NULL)
  {
    m_strLiteral = lpLiteral;
    if (m_strLiteral.GetLength() != m_strMask.GetLength())
	{
      m_strLiteral.Empty();
	}
  }
  else
  {
    m_strLiteral.Empty();
  }

  if (lpValid != NULL)
  {
    m_strValid = lpValid;
  }
  else
  {
    m_strValid.Empty();
  }
  
  m_bUseMask = TRUE;
}


//
////////////////////////////////////////////////////////////////////////////////////////////
void CMaskEdit::SendChar(UINT nChar)
{
	m_bMaskKeyInProgress = TRUE;
	
#ifdef WIN32
    AfxCallWndProc(this, m_hWnd, WM_CHAR, nChar, 1);
#else
    SendMessage(WM_CHAR, nChar, 1);
#endif

	m_bMaskKeyInProgress = FALSE;	
}

void CMaskEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (!m_bMaskKeyInProgress)
	{
		if (!CheckChar(nChar)) return;
	}

	if (m_bUseMask)
	{
		if (isprint(nChar))
		{
			// si un masque existe, on est en insert mode
			int startPos, endPos;
			GetSel(startPos, endPos);
			SetSel(startPos, endPos+1);
			Clear();

			if(m_str.GetLength() >= endPos)
			{
				m_str.SetAt(endPos, nChar); // added this
			}
		}
		else if (nChar == VK_BACK)
		{
			int startPos, endPos;
			GetSel(startPos, endPos);
			// sanity range check
			if ((startPos == endPos) && (startPos >= 1) && (startPos <= m_str.GetLength()))
			{
				// back space the cursor
				SendMessage(WM_KEYDOWN, VK_LEFT, 0);
			}
			else // out of range or have more than one char selected
				MessageBeep((UINT)-1);
			return;
		}
		else
		{
			;//
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CMaskEdit::CheckChar(UINT nChar)
{
  UINT c;

  // do not use mask
  if (!m_bUseMask) return TRUE;

  // control character, OK
  if (!isprint(nChar)) return TRUE;
	
  
  // unselect all selections, if any
  int startPos, endPos;
  GetSel(startPos, endPos);
  SetSel(-1, 0);
  SetSel(startPos, startPos);

  // check the key against the mask
  GetSel(startPos, endPos);

  // make sure the string is not longer than the mask
  if (endPos >= m_strMask.GetLength())
  {
    MessageBeep((UINT)-1);
    return FALSE;
  }

  
  // check to see if a literal is in this position
  c = '_';
  if (!m_strLiteral.IsEmpty())
    c = m_strLiteral.GetAt(endPos);

  if (c != '_')
  {
    SendChar(c);
    GetSel(startPos, endPos);
  }

  // check the valid string character
  if (m_strValid.Find(nChar) != -1) return TRUE;

  // check the key against the mask
  c = m_strMask.GetAt(endPos);
  BOOL doit = TRUE;

  COleDateTime time;
  CString strCurrentText = m_str;

  switch (c)
  {
    case '0':		// digit only //completely changed this
    {
		BOOL doit = TRUE;
		if(isdigit(nChar))
		{
			if(m_isdate)
			{
				//年份检查

				//月份检查
				if(endPos == 5)
				{
					strCurrentText.SetAt(5,nChar);
					if(!time.ParseDateTime(strCurrentText))
					{
						doit = FALSE;
					}
				}
				if(endPos == 6)
				{
					strCurrentText.SetAt(6,nChar);
					if(!time.ParseDateTime(strCurrentText))
					{
						doit = FALSE;
					}
				}

				//天数检查
				if(endPos == 8)
				{
					strCurrentText.SetAt(8,nChar);
					if(!time.ParseDateTime(strCurrentText))
					{
						doit = FALSE;
					}
				}
				if(endPos == 9)
				{
					strCurrentText.SetAt(9,nChar);
					if(!time.ParseDateTime(strCurrentText))
					{
						doit = FALSE;
					}
				}

				//小时检查
				if(endPos == 11)
				{
					if(nChar > '2')
						doit = FALSE;
					
					if(nChar == '2' && m_str.GetAt(12) > '3')
					{
						doit = FALSE;
					}
				}
				if(endPos == 12)
				{
					if(m_str.GetAt(11) == '2' && nChar > '3')
					{
						doit = FALSE;
					}
				}
				
				//分钟检查
				if(endPos == 14)
				{
					if(nChar > '5')
						doit = FALSE;
				}
				if(endPos == 15)
				{
					;
				}
				
				//秒检查
				if(endPos == 17)
				{
					if(nChar > '5')
						doit = FALSE;
				}
				if(endPos == 18)
				{
					;
				}
				
			}

	        return doit;
		}
      break;
    }
    case '9':		// digit or space
    {
      if (isdigit(nChar)) return TRUE;
      if (nChar != VK_SPACE) return TRUE;
      break;
    }
    case '#':		// digit or space or '+' or '-'
    {
      if (isdigit(nChar)) return TRUE;
      if (nChar == VK_SPACE || nChar == VK_ADD || nChar == VK_SUBTRACT) return TRUE;
      break;
    }
    case 'L':		// alpha only
    {
      if (isalpha(nChar)) return TRUE;
      break;
    }
    case '?':		// alpha or space
    {
      if (isalpha(nChar)) return TRUE;
      if (nChar == VK_SPACE) return TRUE;
      break;
    }
    case 'A':		// alpha numeric only
    {
      if (isalnum(nChar)) return TRUE;
      break;
    }
    case 'a':		// alpha numeric or space
    {
      if (isalnum(nChar)) return TRUE;
      if (nChar == VK_SPACE) return TRUE;
      break;
    }
    case '&':		// all print character only
    {
      if (isprint(nChar)) return TRUE;
      break;
    }
  }
  MessageBeep((UINT)-1);
  return FALSE;
}

void CMaskEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  // si un masque existe, tester les touches sp閏iales
  if (m_bUseMask)
  {
    switch (nChar)
    {
      case VK_DELETE:
      case VK_INSERT: return;
    }
  }
  
  CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMaskEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	
	//数据范围
	if(m_bRange)
	{
		CString str;
		GetWindowText(str);	
		COleDateTime time;
		time.ParseDateTime(str);
		
		//符合范围
		if(time >= m_dtMinDate && time <= m_dtMaxDate)
		{
			GetParent()->SendMessage(WM_ON_ENDEDIT,0,0);	
			return;
		}
		
		//不符合范围
		CString strError;
		strError.Format(_T("日期范围在%s～%s之间！"),m_dtMinDate.Format(_T("%Y-%m-%d %H:%M:%S")),
							m_dtMaxDate.Format(_T("%Y-%m-%d %H:%M:%S")));
		::MessageBox(m_hWnd,strError,_T("出错了！"),MB_OK|MB_ICONINFORMATION);
		
		SetFocus();
		Undo();
		SetSel(0,-1);
	}
	else
	{
		GetParent()->SendMessage(WM_ON_ENDEDIT,0,0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// COleDateTime read /write
/*
COleDateTime ReadCOleDateTime(LPCSTR lpszData)
{
	COleDateTime DateTime;
	DateTime.ParseDateTime(lpszData);
	return DateTime;
}

void FormatCOleDateTime(CString& strData, COleDateTime DateTime)
{
	strData = "";
	if (DateTime.m_dt == 0) 
	{
		return;
	}
	else
	{
		strData = DateTime.Format("%Y-%m-%d %H:%M:%S");
	}
}
*/

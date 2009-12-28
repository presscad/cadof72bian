
//

#include "stdafx.h"
#include "NumEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumEdit
#define WM_ON_ENDEDIT	   (WM_APP + 1001)

CNumEdit::CNumEdit()
{
	m_bIsDouble = FALSE;

	//����
	m_bHasPrecision = FALSE;
	m_iPrecssion = 0;
	
	//���ݷ�Χ
	m_bRange = FALSE;
	m_dMaxValue = 0.0;
	m_dMinValue = 0.0;	
}

CNumEdit::~CNumEdit()
{
}


BEGIN_MESSAGE_MAP(CNumEdit, CEdit)
	//{{AFX_MSG_MAP(CNumEdit)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumEdit message handlers

void CNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//�����ַ�
	switch (nChar)
	{
	case VK_BACK:
	case VK_RETURN:
	case 0x0A: // Shift+Enter (= linefeed)
	case VK_ESCAPE:
	case VK_TAB:
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			return;
		}
		break;
	default:
		break;
	}

	//��ĸ����
	if(isalpha(nChar) && nChar != '.' && nChar != '-')
	{
		return;
	}
	
	if(!m_bIsDouble && nChar == '.')
	{
		return;
	}

	CString str;
	GetWindowText(str);	
	int iFindDot = str.Find('.');
	int iFindMinus = str.Find('-');

	if(!isdigit(nChar))//���� ". -"
	{
		if(nChar == '.')
		{
			int startPos, endPos;
			GetSel(startPos, endPos);
			if(startPos == 0 && iFindMinus != -1)
			{
				return;
			}
			
			if(iFindDot != -1)
			{
				return;
			}
		}
		else if(nChar == '-')
		{			
			int startPos, endPos;
			GetSel(startPos, endPos);
			if(startPos != 0)
			{
				return;
			}
			
			if(iFindMinus != -1)
			{
				return;
			}
			
		}
		else if(nChar == '0')
		{
			//return;
		}
		else
		{
			return;
		}
	}
	else
	{		
		int startPos, endPos;
		GetSel(startPos, endPos);
		if(startPos == 0 && iFindMinus != -1)
		{
			return;
		}

		//����
		if(m_bHasPrecision && m_iPrecssion > 0)
		{
			str.TrimLeft();
			str.TrimRight();
			
			int iLength = str.GetLength();
			if(iFindDot != -1) iLength --;
			if(iFindMinus != -1) iLength --;

			if(iLength >= m_iPrecssion)
			{
				return;
			}
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CNumEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	//���ݷ�Χ
	if(m_bRange)
	{
		CString str;
		GetWindowText(str);	
		str.TrimLeft();
		str.TrimRight();
		int iLength = str.GetLength();
		
		double dResult = _tcstod(str,NULL);
		//���Ϸ�Χ
		if(dResult >= m_dMinValue && dResult <= m_dMaxValue)
		{
			GetParent()->SendMessage(WM_ON_ENDEDIT,0,0);	
			return;
		}

		//�����Ϸ�Χ
		CString strError;
		strError.Format(_T("��ֵ��Χ��%.4f��%.4f֮�䣡"),m_dMinValue,m_dMaxValue);
		::MessageBox(m_hWnd,strError,_T("�����ˣ�"),MB_OK|MB_ICONINFORMATION);
		
		SetFocus();
		Undo();
		SetSel(0,-1);
	}
	else
	{
		GetParent()->SendMessage(WM_ON_ENDEDIT,0,0);	
	}
}


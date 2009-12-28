// CmdPowerConfigDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "ConfigReader.h"



// CCmdPowerConfigDlg �Ի���
class CCmdPowerConfigDlg : public CDialog
{
private:
	CString m_strConfigFile;
	CJn_CommandPower m_cmdPower;
// ����
public:
	CCmdPowerConfigDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CMDPOWERCONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
};

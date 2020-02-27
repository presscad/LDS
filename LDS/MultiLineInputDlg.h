#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "XhCharString.h"

// CMultiLineInputDlg �Ի���

class CMultiLineInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CMultiLineInputDlg)

public:
	CString m_sSpaceValue;
public:
	CMultiLineInputDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMultiLineInputDlg();

// �Ի�������
	enum { IDD = IDD_MULTI_LINE_INPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_xRichCtrl;
	CString m_sRichSpaceValue;
public:
	afx_msg void OnBnClickedConfirmInput();
};

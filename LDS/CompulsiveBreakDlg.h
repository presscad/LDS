#pragma once
#include "resource.h"

// CCompulsiveBreakDlg �Ի���

class CCompulsiveBreakDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCompulsiveBreakDlg)

public:
	HANDLE m_hPipeSrcReadDup;
	CCompulsiveBreakDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCompulsiveBreakDlg();

// �Ի�������
	enum { IDD = IDD_COMPULSIVE_BREAK_LOADCAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnCancel();
	virtual void OnDestroy();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

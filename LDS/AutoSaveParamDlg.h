#pragma once
#include "afxwin.h"


// CAutoSaveFileParameterDlg �Ի���

class CAutoSaveParamDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAutoSaveParamDlg)

public:
	CAutoSaveParamDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAutoSaveParamDlg();

// �Ի�������
	enum { IDD = IDD_AUTO_SAVE_PARAM_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bAnymorePrompt;
	CString m_sAutoSaveTime;
	CComboBox m_cmbSvaeFileType;
	int m_iAutoSaveType;
	afx_msg void OnBnClickedOk();
};
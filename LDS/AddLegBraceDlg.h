#pragma once


// CAddLegBraceDlg �Ի���

class CAddLegBraceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddLegBraceDlg)

public:
	CAddLegBraceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddLegBraceDlg();

// �Ի�������
	enum { IDD = IDD_ADD_LEG_BRACE_RODS_SCRIPT_DLG };
	CString m_sLegScript;
	CString m_sNote;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnCreateSampleScript();
	afx_msg void OnImportLegBraceScriptFile();
	afx_msg void OnExportLegBraceScriptFile();
	afx_msg void OnBnConfirm();
};

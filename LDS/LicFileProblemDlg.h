#pragma once


// CLicFileProbleDlg �Ի���

class CLicFileProblemDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLicFileProblemDlg)

public:
	CLicFileProblemDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLicFileProblemDlg();

// �Ի�������
	enum { IDD = IDD_LIC_FILE_PROBLEM_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddLicFile();
	virtual BOOL OnInitDialog();
	CString m_sFileName;
};

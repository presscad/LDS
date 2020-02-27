#pragma once


// CLicFileProbleDlg 对话框

class CLicFileProblemDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLicFileProblemDlg)

public:
	CLicFileProblemDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLicFileProblemDlg();

// 对话框数据
	enum { IDD = IDD_LIC_FILE_PROBLEM_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddLicFile();
	virtual BOOL OnInitDialog();
	CString m_sFileName;
};

#pragma once


#ifdef __PART_DESIGN_INC_
// CAddEiffelDlg 对话框

class CAddEiffelDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddEiffelDlg)

public:
	CAddEiffelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddEiffelDlg();

// 对话框数据
	enum { IDD = IDD_ADD_EIFFIEL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCmbWorkcaseNo();
	double m_fEiffel;
	int m_iWorkNo;
	CString m_sWorkName;
};
#endif
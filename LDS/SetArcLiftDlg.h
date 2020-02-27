#pragma once


// CSetArcLiftDlg 对话框

class CSetArcLiftDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetArcLiftDlg)

public:
	CSetArcLiftDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetArcLiftDlg();

// 对话框数据
	enum { IDD = IDD_SET_ARC_LIFT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	long m_hArcLift;
	int m_iArcLift;
	afx_msg void OnBnClickedOk();
};

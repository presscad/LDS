#pragma once


// CSetArcLiftDlg �Ի���

class CSetArcLiftDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetArcLiftDlg)

public:
	CSetArcLiftDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetArcLiftDlg();

// �Ի�������
	enum { IDD = IDD_SET_ARC_LIFT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	long m_hArcLift;
	int m_iArcLift;
	afx_msg void OnBnClickedOk();
};

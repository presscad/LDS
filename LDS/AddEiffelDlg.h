#pragma once


#ifdef __PART_DESIGN_INC_
// CAddEiffelDlg �Ի���

class CAddEiffelDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddEiffelDlg)

public:
	CAddEiffelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddEiffelDlg();

// �Ի�������
	enum { IDD = IDD_ADD_EIFFIEL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
#pragma once


// CRelayoutScaleNodeDlg �Ի���

class CRelayoutScaleNodeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRelayoutScaleNodeDlg)

public:
	CRelayoutScaleNodeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	double m_fPivotScale;
	virtual ~CRelayoutScaleNodeDlg();

// �Ի�������
	enum { IDD = IDD_RELAYOUT_SCALE_NODE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int m_nSectCountNow;
	int m_nSectCountDest;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeESectCountDest();
	afx_msg void OnChangeESectCountNow();
	CString m_sPivotScale;
	virtual void OnOK();
};

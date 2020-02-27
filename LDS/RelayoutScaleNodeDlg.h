#pragma once


// CRelayoutScaleNodeDlg 对话框

class CRelayoutScaleNodeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRelayoutScaleNodeDlg)

public:
	CRelayoutScaleNodeDlg(CWnd* pParent = NULL);   // 标准构造函数
	double m_fPivotScale;
	virtual ~CRelayoutScaleNodeDlg();

// 对话框数据
	enum { IDD = IDD_RELAYOUT_SCALE_NODE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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

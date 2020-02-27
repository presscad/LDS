#pragma once


// CWireInfoInputDlg

class CWireInfoInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWireInfoInputDlg)

public:
	CWireInfoInputDlg();
	virtual ~CWireInfoInputDlg();

// 对话框数据
	enum { IDD = IDD_WIRE_BASIC_INFO_DLG };
public:
	int m_iWireType;
	int m_iHangStyle;
	int m_iPhaseSerial;
	int m_nPostCode;
	int m_iTentionType;
	BYTE m_ciWireDir;
	int m_iTensionCmb;
protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEPostCode();
	afx_msg void OnCbnSelchangeCmbPhaseSerial();
	afx_msg void OnCbnEditchangeCmbPhaseSerial();
	afx_msg void OnCbnSelchangeCmbPostionTag();
	afx_msg void OnCbnSelchangeCmbTensionType();
};



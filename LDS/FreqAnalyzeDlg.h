#pragma once


// CFreqAnalyzeDlg 对话框

class CFreqAnalyzeDlg : public CDialog
{
	DECLARE_DYNAMIC(CFreqAnalyzeDlg)

public:
	CString m_sModuleScope;
	CFreqAnalyzeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFreqAnalyzeDlg();

// 对话框数据
	enum { IDD = IDD_FREQ_ANALYZE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_sHeightNo;
	int m_iElemProcessMode;
	int m_nFreqOrderCount;
	BOOL m_bWeightAmplifyCoefIncBraceRods;
	BOOL m_bAreaAmplifyCoefIncBraceRods;
	afx_msg void OnChkWeightCoefIncBraceRods();
};

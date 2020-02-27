#pragma once


// CSelNailSeriesDlg 对话框

class CSelNailSeriesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelNailSeriesDlg)
	long m_idCommonNailSeries,m_idAntiThelftNailSeries;
public:
	CSelNailSeriesDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelNailSeriesDlg();

// 对话框数据
	enum { IDD = IDD_SEL_CLIMB_BOLT_SERIES_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	int m_iCommonNailSeries;
	int m_iAntiThelftNailSeries;
};

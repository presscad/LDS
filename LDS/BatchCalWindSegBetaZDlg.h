#pragma once

#ifdef __PART_DESIGN_INC_
#include "XhListCtrl.h"
// CBatchCalWindSegBetaZDlg 对话框

class CBatchCalWindSegBetaZDlg : public CDialog
{
	DECLARE_DYNAMIC(CBatchCalWindSegBetaZDlg)

public:
	VIBRAMODE_PROP *modes_arr;
	int m_nOrderNum;
	int m_iSelOrder;
	double m_fBetaZCalWindV;	//根据随机振动法计算风振系数时的基准风速（一般为结构工况中的最大风速）
	CBatchCalWindSegBetaZDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBatchCalWindSegBetaZDlg();

// 对话框数据
	enum { IDD = IDD_CAL_WINDSEG_BETAZ_DLG };
	CXhListCtrl	m_listVibraMode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_iBatchCalBetaZStyle;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnIntelliSelVibraMode();
	afx_msg void OnBnPreviewCurrVibraMode();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnItemchangedVibrationModeInfo(NMHDR *pNMHDR, LRESULT *pResult);
};
#endif
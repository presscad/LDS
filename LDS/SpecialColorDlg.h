#pragma once
#include "afxwin.h"
#include "ColorSelectComboBox.h"

// CSpecialColoeDlg 对话框

class CSpecialColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpecialColorDlg)
public:
	BOOL	m_arrIsCanUse[5];
	COLORREF m_arrValue[5];
	char m_sProp[50];

public:
	CSpecialColorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSpecialColorDlg();

// 对话框数据
	enum { IDD = IDD_SPECIAL_COLOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnMaterialS();
	afx_msg void OnMaterialH();
	afx_msg void OnMaterialG();
	afx_msg void OnMaterialP();
	afx_msg void OnMaterialT();
	afx_msg void OnCbnSelchangeCmbColorS();
	afx_msg void OnCbnSelchangeCmbColorH();
	afx_msg void OnCbnSelchangeCmbColorG();
	afx_msg void OnCbnSelchangeCmbColorP();
	afx_msg void OnCbnSelchangeCmbColorT();
	DECLARE_MESSAGE_MAP()
public:
	CColorSelectComboBox m_ComBoxS;
	CColorSelectComboBox m_ComBoxH;
	CColorSelectComboBox m_ComBoxG;
	CColorSelectComboBox m_ComBoxP;
	CColorSelectComboBox m_ComBoxT;
};

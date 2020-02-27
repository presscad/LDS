#pragma once

#include "resource.h"
#include "supergridctrl.h"
#include "TowerBom.h"

// CBomCfgDlg 对话框

class CBomCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CBomCfgDlg)

	void InitListCtrl(void);
public:
	CBomCfgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBomCfgDlg();
public:
	CSuperGridCtrl	m_listCtrlModule;
	CSuperGridCtrl	m_listCtrlPart;
	CSuperGridCtrl	m_listCtrlBolt;
	CTowerBOM m_xBomObj;

// 对话框数据
	enum { IDD = IDD_BOM_CFG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnImportTxtFile();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBnImportExcelFile();
	afx_msg void OnBnClickedBnEmptyData();
};

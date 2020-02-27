#pragma once

#include "resource.h"
#include "supergridctrl.h"
#include "TowerBom.h"

// CBomCfgDlg �Ի���

class CBomCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CBomCfgDlg)

	void InitListCtrl(void);
public:
	CBomCfgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBomCfgDlg();
public:
	CSuperGridCtrl	m_listCtrlModule;
	CSuperGridCtrl	m_listCtrlPart;
	CSuperGridCtrl	m_listCtrlBolt;
	CTowerBOM m_xBomObj;

// �Ի�������
	enum { IDD = IDD_BOM_CFG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnImportTxtFile();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBnImportExcelFile();
	afx_msg void OnBnClickedBnEmptyData();
};

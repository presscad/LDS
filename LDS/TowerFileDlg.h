#pragma once

#include "afxwin.h"
#include "ServerTowerType.h"

// CTowerFileDlg 对话框
#ifdef __CONNECT_REMOTE_SERVER_
class CTowerFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTowerFileDlg)

	CServerTowerFileGroup *m_pCurFileGroup;
	CServerTowerType *m_pTowerType;
	void InitFileGroupComobox();
public:
	CTowerFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTowerFileDlg();

// 对话框数据
	enum { IDD = IDD_TOWERFILE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	void Init(CServerTowerType *pTowerType,CServerTowerFileGroup *pFileGroup);
	CServerTowerFileGroup *GetCurFileGroup();
	CServerTowerType *GetTowerType();
public:
	BOOL m_bDisplayTowerType;
	CString m_sFileName;
	CComboBox m_cmbFileGroup;
	CComboBox m_cmbTowerType;
	afx_msg void OnCbnSelchangeCmbTowertype();
};
#endif
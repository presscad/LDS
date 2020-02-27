#pragma once

#include "TidCplus.h"
#include "resource.h"
#include "afxcmn.h"
#include "XhListCtrl.h"
#include "f_ent_list.h"
// CTidToDwgDlg 对话框

class CTidToDwgDlg : public CDialog
{
	DECLARE_DYNAMIC(CTidToDwgDlg)
public:
	ATOM_LIST<ITidHeightGroup*> xTidModuleList;
public:
	CTidToDwgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTidToDwgDlg();
	//
	void RefreshListCtrl();
	void ConfirmCreate(bool bOnlySelecteItems);
// 对话框数据
	enum { IDD = IDD_TID_TO_DWG_DLG };
	CString	m_sFilePath;
	BOOL	m_bDrawLine;
	BOOL	m_bDrawSolie;
	BOOL	m_bOneTaOneFile;
	CXhListCtrl m_listCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedChkImportTidFile();
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
};
//
extern ITidModel* gpTidModel;
void OpenTidFile(CString sFilePath);
void CloseTidFile();
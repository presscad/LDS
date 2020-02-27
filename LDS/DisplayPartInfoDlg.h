#include "supergridctrl.h"
#pragma once


// displayPartInfoDlg 对话框

class CDisplayPartInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDisplayPartInfoDlg)
public:
	 CSuperGridCtrl m_displayPartInfoListCtrl;
     PARTSET partSet;
public:
	CDisplayPartInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDisplayPartInfoDlg();
	CHashSet<CLDSPart*>hashDatumPartByHandle ;
// 对话框数据
	enum { IDD = IDD_DISPLAY_PART_DLG };
public:
	 virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();
	afx_msg void OnClickDisplayPartInfoListCtrl();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickDisplayPartInfoListCtrl(NMHDR *pNMHDR, LRESULT *pResult);
};

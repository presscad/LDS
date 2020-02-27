#pragma once


// CLsSubPartLibDlg 对话框
#include "f_ent_list.h"
#include "env_def.h"
#include "supergridctrl.h"
#include "TowerTreeCtrl.h"

class CLsSubPartLibDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLsSubPartLibDlg)
	//
	ATOM_LIST<TREEITEM_INFO> itemInfoList;
	void RefreshTreeCtrl();		//更新树控件
	void RefreshListCtrl();		//更新列表
	void ContextMenu(CWnd* pWnd, CPoint point);
public:
	CLsSubPartLibDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLsSubPartLibDlg();
	
// 对话框数据
	enum { IDD = IDD_LS_SUBPART_LIB_DLG };
	CSuperGridCtrl m_xListCtrl;
	CTowerTreeCtrl m_xTreeCtrl;
	//
	CImageList m_imageList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	//
	afx_msg void OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnDeleteAllItem();
	afx_msg void OnImportLsSubPartLibFile();
	afx_msg void OnExportLsSubPartLibFile();
};

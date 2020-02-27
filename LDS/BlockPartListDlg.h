#pragma once

#include "XhListCtrl.h"
#include "Block.h"
#include "afxwin.h"
// CBlockPartListDlg 对话框

class CBlockPartListDlg : public CDialog
{
	DECLARE_DYNAMIC(CBlockPartListDlg)
public:
	CBlockModel *m_pBlock;
	CListViewComparer listViewComparer;
public:
	CBlockPartListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBlockPartListDlg();

// 对话框数据
	enum { IDD = IDD_BLOCK_PARTLIST_DLG };
	CXhListCtrl	m_xPartList;
public:
	void SelectPart(int nCurSel);
	void DeleteSelectItems();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListPart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRclickListPart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
};

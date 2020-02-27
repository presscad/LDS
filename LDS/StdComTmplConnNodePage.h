#pragma once
#include "afxcmn.h"


// CStdComTmplConnNodePage 对话框

class CATLOG_TREEITEM;
class CStdComTmplConnNodePage : public CDialogEx
{
	DECLARE_DYNAMIC(CStdComTmplConnNodePage)
	CBitmap m_xSketchImg;
	UINT m_idStdComTmpl;	//当前选中的标准模板的Id
	HTREEITEM AddOrUpdateTreeItem(CATLOG_TREEITEM* pCatgItem,HTREEITEM hParentItem);
public:
	CStdComTmplConnNodePage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStdComTmplConnNodePage();
	UINT GetCurrSelStdComTmplId(){return m_idStdComTmpl;}

// 对话框数据
	enum { IDD = IDD_STD_COM_TMPL_CONN_NODE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	CTreeCtrl m_treeCtrl;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
};

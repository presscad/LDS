#pragma once
#include "afxcmn.h"


// CStdComTmplConnNodePage �Ի���

class CATLOG_TREEITEM;
class CStdComTmplConnNodePage : public CDialogEx
{
	DECLARE_DYNAMIC(CStdComTmplConnNodePage)
	CBitmap m_xSketchImg;
	UINT m_idStdComTmpl;	//��ǰѡ�еı�׼ģ���Id
	HTREEITEM AddOrUpdateTreeItem(CATLOG_TREEITEM* pCatgItem,HTREEITEM hParentItem);
public:
	CStdComTmplConnNodePage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStdComTmplConnNodePage();
	UINT GetCurrSelStdComTmplId(){return m_idStdComTmpl;}

// �Ի�������
	enum { IDD = IDD_STD_COM_TMPL_CONN_NODE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

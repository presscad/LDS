#pragma once


// CLsSubPartLibDlg �Ի���
#include "f_ent_list.h"
#include "env_def.h"
#include "supergridctrl.h"
#include "TowerTreeCtrl.h"

class CLsSubPartLibDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLsSubPartLibDlg)
	//
	ATOM_LIST<TREEITEM_INFO> itemInfoList;
	void RefreshTreeCtrl();		//�������ؼ�
	void RefreshListCtrl();		//�����б�
	void ContextMenu(CWnd* pWnd, CPoint point);
public:
	CLsSubPartLibDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLsSubPartLibDlg();
	
// �Ի�������
	enum { IDD = IDD_LS_SUBPART_LIB_DLG };
	CSuperGridCtrl m_xListCtrl;
	CTowerTreeCtrl m_xTreeCtrl;
	//
	CImageList m_imageList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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

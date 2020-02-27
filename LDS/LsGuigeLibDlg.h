#pragma once


// CLsGuigeLibDlg �Ի���
#include "f_ent_list.h"
#include "env_def.h"
#include "BoltLibrary.h"
#include "XhListCtrl.h"
#include "supergridctrl.h"
#include "PropertyList.h"
#include "PropListItem.h"
#include "RememberLocationDlg.h"

enum TREE_NODE_TYPE{
	LS_COMPONENT_GROUP	= 1,
	LS_COMPONENT_ITEM	= 2,
	LS_GUIGE_GROUP		= 3,
	LS_GUIGE_ITEM		= 4,
	ANCHOR_BOLTS_GROUP	= 5,
};
class CLsGuigeLibDlg : public CRememberLocationDlg
{
	DECLARE_DYNAMIC(CLsGuigeLibDlg)
	IBoltLibrary *m_pSrcBoltLib,*m_pBoltLib;
	ATOM_LIST<TREEITEM_INFO> itemInfoList;
	//
	void InitBoltLibrary();		//��ʼ����˨��
	void RefreshTreeCtrl();		//�������ؼ�
	void RefreshLsCompnTreeCtrl(HTREEITEM hGroup);
	void RefreshLsGuigeTreeCtrl(HTREEITEM hGroup);
	void RefreshAnchorBoltsTreeCtrl();
	void RefreshListCtrl();		//�����б�
	void RefreshLsCompntListCtrl();
	void ContextMenu(CWnd* pWnd, CPoint point);
	void RelayoutWnd();
public:
	CLsGuigeLibDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLsGuigeLibDlg();
	//
	CString MakeAttachItemStr(BYTE theft0_loose1=0,bool bFootNail=false);
	void DisplayProperty();
	int GetCurSelItmeType(DWORD* dwData=NULL);
// �Ի�������
	enum { IDD = IDD_LS_GUIGE_LIB_DLG };
	CXhListCtrl		m_xListCtrl;
	CXhListCtrl		m_xListAnchorBoltCtrl;
	CSuperGridCtrl	m_xLsComptListCtrl;
	CTreeCtrl		m_xTreeCtrl;
	CPropertyList	m_xPropList;
	//
	CImageList m_imageList;
	DECLARE_PROP_FUNC(CLsGuigeLibDlg)
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnSaveLsComponent();
	afx_msg void OnImportLsFamilyFile();
	afx_msg void OnExportLsFamilyFile();
	afx_msg void OnImportSysLsFamily();
	afx_msg void OnDeleteItem();
};

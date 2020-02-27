#pragma once
#include "afxcmn.h"
#include "env_def.h"
#include "PropertyList.h"
#include "GeneralNodeDesign.h"

// CRapid3dModelDlg �Ի���

class CRapid3dModelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRapid3dModelDlg)

	ATOM_LIST<TREEITEM_INFO>itemInfoList;
	CHashSet<TREEITEM_INFO*> hashItemInfos;
	CImageList m_listImages;
	CPropertyList m_propList;
	INTELLI_MODEL_GUIDE m_xInternalLocalGuide;
public:
	CRapid3dModelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRapid3dModelDlg();

// �Ի�������
	enum { IDD = IDD_RAPID_3D_MODELING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void RefreshTreeCtrl();
	void DisplayPropertyList();
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_xConfigTree;
	BOOL ModifyInteliiModelGuideProperty(CPropTreeItem* pItem, CString& valueStr);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTvnSelchangedConfigTree(NMHDR *pNMHDR, LRESULT *pResult);
};

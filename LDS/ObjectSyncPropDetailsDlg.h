#pragma once
#include "XhListCtrl.h"


// CObjectSyncPropDetailsDlg �Ի���

class CObjectSyncPropDetailsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CObjectSyncPropDetailsDlg)
public:
	MEMORY_BLOCK* m_pMPCL;
	PROPLIST_TYPE *m_pSyncPropList;
public:
	CObjectSyncPropDetailsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CObjectSyncPropDetailsDlg();

// �Ի�������
	enum { IDD = IDD_OBJECT_SYNC_PROP_DETAILS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void OnSetSyncItem();
	void OnRevokeSyncItem();
	void OnNMRClickListSyncPropDetails(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
public:
	CXhListCtrl m_listSyncDetails;
	virtual BOOL OnInitDialog();
};

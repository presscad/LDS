#pragma once
#include "resource.h"
#include "supergridctrl.h"
#include "PartSetCreatorDlg.h"
// CAddAngleByRemoveNailDlg �Ի���

class CAddAngleByRemoveNailDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddAngleByRemoveNailDlg)

public:
	CAddAngleByRemoveNailDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddAngleByRemoveNailDlg();

// �Ի�������
	enum { IDD = IDD_ADD_ANGLE_BY_REMOVE_NAIL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CSuperGridCtrl m_xAngleListCtrl;
	CXhPtrSet<CPartSetCreatorDlg::STDPART_ITEM> *m_pPartItemSet;
	virtual BOOL OnInitDialog();
};

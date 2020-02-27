#pragma once
#include "resource.h"
#include "supergridctrl.h"
#include "PartSetCreatorDlg.h"
// CAddAngleByRemoveNailDlg 对话框

class CAddAngleByRemoveNailDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddAngleByRemoveNailDlg)

public:
	CAddAngleByRemoveNailDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddAngleByRemoveNailDlg();

// 对话框数据
	enum { IDD = IDD_ADD_ANGLE_BY_REMOVE_NAIL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CSuperGridCtrl m_xAngleListCtrl;
	CXhPtrSet<CPartSetCreatorDlg::STDPART_ITEM> *m_pPartItemSet;
	virtual BOOL OnInitDialog();
};

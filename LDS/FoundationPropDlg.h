#pragma once
#include "PropertyList.h"
#include "ModelTree.h"
// CFoundationPropDlg 对话框

class CFoundationPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CFoundationPropDlg)
	FOUNDATION_PARA defaultPara;
public:
	CFoundationPropDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFoundationPropDlg();
	FOUNDATION_PARA *m_pFoundation;
// 对话框数据
	enum { IDD = IDD_FOUNDATION_PROP_DLG };
	CPropertyList	m_propList;
	void DisplayFoundationPropList();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};

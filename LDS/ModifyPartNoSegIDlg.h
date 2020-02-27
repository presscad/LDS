#pragma once
#include "env_def.h"
#include "resource.h"
// CModifyPartNoSegIDlg 对话框

class CModifyPartNoSegIDlg : public CDialog
{
	DECLARE_DYNAMIC(CModifyPartNoSegIDlg)

public:
	CModifyPartNoSegIDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModifyPartNoSegIDlg();

// 对话框数据
	enum { IDD = IDD_MODIFY_PARTNO_SEGI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CComboBox m_cmbSegI;
	CString m_sNewSegI;

	SEGI m_oldSegI,m_newSegI;
};

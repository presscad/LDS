#pragma once
#include "env_def.h"
#include "resource.h"
// CModifyPartNoSegIDlg �Ի���

class CModifyPartNoSegIDlg : public CDialog
{
	DECLARE_DYNAMIC(CModifyPartNoSegIDlg)

public:
	CModifyPartNoSegIDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CModifyPartNoSegIDlg();

// �Ի�������
	enum { IDD = IDD_MODIFY_PARTNO_SEGI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CComboBox m_cmbSegI;
	CString m_sNewSegI;

	SEGI m_oldSegI,m_newSegI;
};

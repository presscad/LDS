#pragma once

#include "resource.h"
// CExportSetDlg �Ի���

class CExportSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CExportSetDlg)

public:
	CExportSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CExportSetDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPORT_SET_DLG };
#endif
	int m_iModelLevel;
	int m_iLengthUnit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

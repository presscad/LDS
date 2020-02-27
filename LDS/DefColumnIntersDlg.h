#pragma once


// CDefColumnIntersDlg �Ի���
#include "PropertyList.h"
#include "CallBackDialog.h"
class CDefColumnIntersDlg : public CCallBackDialog
{
	DECLARE_DYNAMIC(CDefColumnIntersDlg)

public:
	CDefColumnIntersDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDefColumnIntersDlg();

// �Ի�������
	enum { IDD = IDD_DEFINE_COLUMN_INTERS_DLG };
public:
	CPropertyList m_propList;
	CLDSPoint m_columnPoint;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	void DisplayColumnPointProperty();
	afx_msg void OnBnClickedBtnConfirm();
};

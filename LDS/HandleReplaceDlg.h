#pragma once
#include "resource.h"
#include "CallBackDialog.h"

// CHandleReplaceDlg �Ի���

class CHandleReplaceDlg : public CCallBackDialog
{
	//DECLARE_DYNAMIC(CHandleReplaceDlg)

public:
	CHandleReplaceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHandleReplaceDlg();

// �Ի�������
	enum { IDD = IDD_HANDLE_REPLACE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnPickDestObj();
	CString m_hOldObject;
	CString m_hNewObject;
};

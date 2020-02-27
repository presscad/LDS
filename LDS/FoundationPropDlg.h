#pragma once
#include "PropertyList.h"
#include "ModelTree.h"
// CFoundationPropDlg �Ի���

class CFoundationPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CFoundationPropDlg)
	FOUNDATION_PARA defaultPara;
public:
	CFoundationPropDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFoundationPropDlg();
	FOUNDATION_PARA *m_pFoundation;
// �Ի�������
	enum { IDD = IDD_FOUNDATION_PROP_DLG };
	CPropertyList	m_propList;
	void DisplayFoundationPropList();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};

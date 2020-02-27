#pragma once
#include "afxwin.h"


// CNodeSegIDlg �Ի���

class CNodeIDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNodeIDlg)

public:
	CNodeIDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNodeIDlg();

// �Ի�������
	enum { IDD = IDD_NODE_I_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_iMinDummyNodeI;
	int m_iPriorMiddleVacancy;
	BOOL m_bInheritPracticalNode;
	BOOL m_bInheritDummyNode;
	BOOL m_bRecodeAll;
	//
	afx_msg void OnBnClickedRecodeAll();
	afx_msg void OnBnClickedInheritDummyNode();
	afx_msg void OnBnClickedParcticalNode();
};

#pragma once

#include "Resource.h"
// CInputMKRectDlg �Ի���

class CInputMKRectDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputMKRectDlg)

public:
	CInputMKRectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputMKRectDlg();

// �Ի�������
	enum { IDD = IDD_INPUT_MK_RECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nRectL;
	int m_nRectW;
	double m_fTextH;
};

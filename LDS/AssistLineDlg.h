#pragma once
#include "afxwin.h"


// CAssistLineDlg �Ի���

class CAssistLineDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAssistLineDlg)

public:
	CAssistLineDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAssistLineDlg();

// �Ի�������
	enum { IDD = IDD_ADD_ASSIST_LINE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_fLineStartY;
	double m_fLineStartX;
	double m_fLineEndX;
	double m_fLineEndY;
};

#pragma once
#include "afxwin.h"


// CAssistLineDlg 对话框

class CAssistLineDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAssistLineDlg)

public:
	CAssistLineDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAssistLineDlg();

// 对话框数据
	enum { IDD = IDD_ADD_ASSIST_LINE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_fLineStartY;
	double m_fLineStartX;
	double m_fLineEndX;
	double m_fLineEndY;
};

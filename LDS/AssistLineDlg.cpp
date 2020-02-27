// AssistLineDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "AssistLineDlg.h"
#include "afxdialogex.h"


// CAssistLineDlg 对话框

IMPLEMENT_DYNAMIC(CAssistLineDlg, CDialogEx)

CAssistLineDlg::CAssistLineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAssistLineDlg::IDD, pParent)
	, m_fLineStartY(0)
	, m_fLineStartX(0)
	, m_fLineEndX(0)
	, m_fLineEndY(0)
{

}

CAssistLineDlg::~CAssistLineDlg()
{
}

void CAssistLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_LINE_START_Y, m_fLineStartY);
	DDX_Text(pDX, IDC_E_LINE_START_X, m_fLineStartX);
	DDX_Text(pDX, IDC_E_LINE_END_X, m_fLineEndX);
	DDX_Text(pDX, IDC_E_LINE_END_Y, m_fLineEndY);
}


BEGIN_MESSAGE_MAP(CAssistLineDlg, CDialogEx)
END_MESSAGE_MAP()


// CAssistLineDlg 消息处理程序

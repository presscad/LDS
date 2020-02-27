// SelAssotiatedBoltsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SelAssotiatedBoltsDlg.h"
#include "afxdialogex.h"


// CSelAssotiatedBoltsDlg 对话框

IMPLEMENT_DYNAMIC(CSelAssotiatedBoltsDlg, CDialogEx)

CSelAssotiatedBoltsDlg::CSelAssotiatedBoltsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelAssotiatedBoltsDlg::IDD, pParent)
	, m_bOrdinaryBolt(TRUE)
	, m_bFootnailBolt(TRUE)
{

}

CSelAssotiatedBoltsDlg::~CSelAssotiatedBoltsDlg()
{
}

void CSelAssotiatedBoltsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_ORDINARY_BOLTS, m_bOrdinaryBolt);
	DDX_Check(pDX, IDC_CHK_FOOTNAIL_BOLTS, m_bFootnailBolt);
}

BOOL CSelAssotiatedBoltsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;
}
BEGIN_MESSAGE_MAP(CSelAssotiatedBoltsDlg, CDialogEx)
END_MESSAGE_MAP()


// CSelAssotiatedBoltsDlg 消息处理程序

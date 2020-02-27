// HandleReplaceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "HandleReplaceDlg.h"
#include "afxdialogex.h"


// CHandleReplaceDlg 对话框

//IMPLEMENT_DYNAMIC(CHandleReplaceDlg, CCallBackDialog)

CHandleReplaceDlg::CHandleReplaceDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CHandleReplaceDlg::IDD, pParent)
	, m_hOldObject(_T(""))
	, m_hNewObject(_T(""))
{

}

CHandleReplaceDlg::~CHandleReplaceDlg()
{
}

void CHandleReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_OLD_HANDLE, m_hOldObject);
	DDX_Text(pDX, IDC_E_NEW_HANDLE, m_hNewObject);
}


BEGIN_MESSAGE_MAP(CHandleReplaceDlg, CDialog)
	ON_BN_CLICKED(IDC_BN_PICK_DEST_OBJ, &CHandleReplaceDlg::OnBnPickDestObj)
END_MESSAGE_MAP()


// CHandleReplaceDlg 消息处理程序


BOOL CHandleReplaceDlg::OnInitDialog()
{
	CCallBackDialog::OnInitDialog();

	if(m_bInernalStart&&m_hPickObj>0x20)
	{
		m_hNewObject.Format("%X",m_hPickObj);
		UpdateData(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CHandleReplaceDlg::OnBnPickDestObj()
{
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	CDialog::OnOK();
}

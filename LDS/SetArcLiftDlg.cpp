// SetArcLiftDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SetArcLiftDlg.h"
#include "Tower.h"
#include "env_def.h"
#include ".\setarcliftdlg.h"


// CSetArcLiftDlg 对话框

IMPLEMENT_DYNAMIC(CSetArcLiftDlg, CDialog)
CSetArcLiftDlg::CSetArcLiftDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetArcLiftDlg::IDD, pParent)
	, m_hArcLift(0)
	, m_iArcLift(0)
{
}

CSetArcLiftDlg::~CSetArcLiftDlg()
{
}

void CSetArcLiftDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_CMB_ARC_LIFT, m_iArcLift);
}


BEGIN_MESSAGE_MAP(CSetArcLiftDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSetArcLiftDlg 消息处理程序

BOOL CSetArcLiftDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox *pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ARC_LIFT);
	if(pCmbBox==NULL)
		return FALSE;
	pCmbBox->ResetContent();
	for(CLDSArcLift* pLift=Ta.ArcLift.GetFirst();pLift;pLift=Ta.ArcLift.GetNext())
	{
		int iIndex=pCmbBox->AddString(pLift->name);
		pCmbBox->SetItemData(iIndex,pLift->handle);
		if(m_hArcLift==pLift->handle)
			m_iArcLift=iIndex;
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSetArcLiftDlg::OnBnClickedOk()
{
	UpdateData();
	CComboBox *pCmbBox=(CComboBox*)GetDlgItem(IDC_CMB_ARC_LIFT);
	if(m_iArcLift>=0)
		m_hArcLift=pCmbBox->GetItemData(m_iArcLift);
	OnOK();
}

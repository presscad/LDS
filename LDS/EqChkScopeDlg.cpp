// EqChkScopeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "EqChkScopeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEqChkScopeDlg dialog

#if defined(__LDS_)||defined(__TSA_)
CEqChkScopeDlg::CEqChkScopeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEqChkScopeDlg::IDD, pParent)
	, m_bUpdateBoltInfo(FALSE)
{
	//{{AFX_DATA_INIT(CEqChkScopeDlg)
	//}}AFX_DATA_INIT
}


void CEqChkScopeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEqChkScopeDlg)
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHK_UPDATE_BOLT_INFO, m_bUpdateBoltInfo);
}


BEGIN_MESSAGE_MAP(CEqChkScopeDlg, CDialog)
	//{{AFX_MSG_MAP(CEqChkScopeDlg)
	ON_BN_CLICKED(IDC_BN_DEL_WORKSTATUS, OnBnDelWorkstatus)
	ON_BN_CLICKED(IDC_BN_ADD_WORKSTATUS, OnBnAddWorkstatus)
	ON_LBN_DBLCLK(IDC_LIST_ATTACH_WORKSTATUS, OnDblclkListAttachWorkstatus)
	ON_LBN_DBLCLK(IDC_LIST_REMAIN_WORKSTATUS, OnDblclkListRemainWorkstatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEqChkScopeDlg message handlers

BOOL CEqChkScopeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char *csStatus=new char[m_sWorkStatusScope.GetLength()+1];
	_snprintf(csStatus,m_sWorkStatusScope.GetLength()+1,"%s",m_sWorkStatusScope);
	CComboBox *pModuleCMB=(CComboBox*)GetDlgItem(IDC_CMB_SEL_MODEL);
	CListBox *pStatusLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_WORKSTATUS);
	CListBox *pStatusRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_WORKSTATUS);
	pModuleCMB->ResetContent();
	pStatusLeft->ResetContent();
	pStatusRight->ResetContent();
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		int i=pModuleCMB->AddString(pModule->description);
		pModuleCMB->SetItemData(i,pModule->iNo);
		if(m_iSelModelNo==pModule->iNo)
			pModuleCMB->SetCurSel(i);
	}
	CString ss;
	for(CWorkStatus *pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
	{
		if(pStatus->m_cMainLoadType<5||pStatus->m_cMainLoadType>8)
			continue;	//只处理抗震验算工况,其余工况略去
		ss.Format("%4d",pStatus->iNo);
		pStatusRight->AddString(ss);
	}
	int iStatusNo=FindAdaptNo(csStatus);
	while(iStatusNo!=0)
	{
		ss.Format("%4d",iStatusNo);
		int ii=pStatusRight->FindString(0,ss);
		if(ii!=LB_ERR)	//找到此号并从右侧删除
		{
			pStatusRight->DeleteString(ii);
			pStatusLeft->AddString(ss);
		}
		iStatusNo=FindAdaptNo(NULL);
	}
	delete []csStatus;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEqChkScopeDlg::OnBnDelWorkstatus() 
{
	CListBox *pStatusLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_WORKSTATUS);
	CListBox *pStatusRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_WORKSTATUS);
	int nSel=pStatusLeft->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int i,*rgIndex=new int[nSel];
	nSel=pStatusLeft->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(i=0;i<nSel;i++)
	{
		pStatusLeft->GetText(rgIndex[i],ss);
		pStatusRight->AddString(ss);
		ss_arr.Add(ss);
	}
	delete []rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=pStatusLeft->FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			pStatusLeft->DeleteString(ii);
	}
}

void CEqChkScopeDlg::OnBnAddWorkstatus() 
{
	CListBox *pStatusLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_WORKSTATUS);
	CListBox *pStatusRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_WORKSTATUS);
	int nSel=pStatusRight->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int *rgIndex=new int[nSel];
	nSel=pStatusRight->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(int i=0;i<nSel;i++)
	{
		pStatusRight->GetText(rgIndex[i],ss);
		pStatusLeft->AddString(ss);
		ss_arr.Add(ss);
	}
	delete []rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=pStatusRight->FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			pStatusRight->DeleteString(ii);
	}
}

void CEqChkScopeDlg::OnDblclkListAttachWorkstatus() 
{
	OnBnDelWorkstatus();
}

void CEqChkScopeDlg::OnDblclkListRemainWorkstatus() 
{
	OnBnAddWorkstatus();
}

void CEqChkScopeDlg::OnOK() 
{
	CComboBox *pModuleCMB=(CComboBox*)GetDlgItem(IDC_CMB_SEL_MODEL);
	CListBox *pStatusLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_WORKSTATUS);
	int iSel=pModuleCMB->GetCurSel();
	if(iSel>=0)
		m_iSelModelNo=pModuleCMB->GetItemData(iSel);
	CString ss;
	m_sWorkStatusScope.Empty();
	int nCount=pStatusLeft->GetCount();
	for(int i=0;i<nCount;i++)
	{
		pStatusLeft->GetText(i,ss);
		ss.TrimLeft();
		if(i<nCount-1)
			m_sWorkStatusScope+=ss+",";
		else
			m_sWorkStatusScope+=ss;
	}
	CDialog::OnOK();
}
#endif
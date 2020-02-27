// CfgModuleScopeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "CfgModuleScopeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCfgModuleScopeDlg dialog


CCfgModuleScopeDlg::CCfgModuleScopeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfgModuleScopeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCfgModuleScopeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	CString ss;
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		ss.Format("%d,",pModule->iNo);
		m_sModuleScope+=ss;
	}
}


void CCfgModuleScopeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCfgModuleScopeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCfgModuleScopeDlg, CDialog)
	//{{AFX_MSG_MAP(CCfgModuleScopeDlg)
	ON_BN_CLICKED(IDC_BN_ADD_MODULE, OnBnAddModule)
	ON_BN_CLICKED(IDC_BN_DEL_MODULE, OnBnDelModule)
	ON_LBN_DBLCLK(IDC_LIST_ATTACH_MODULE, OnDblclkListAttachModule)
	ON_LBN_DBLCLK(IDC_LIST_REMAIN_MODULE, OnDblclkListRemainModule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCfgModuleScopeDlg message handlers

void CCfgModuleScopeDlg::OnBnAddModule() 
{
	CListBox *pModuleLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_MODULE);
	CListBox *pModuleRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_MODULE);
	int nSel=pModuleRight->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int i,*rgIndex=new int[nSel];
	nSel=pModuleRight->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(i=0;i<nSel;i++)
	{
		pModuleRight->GetText(rgIndex[i],ss);
		pModuleLeft->AddString(ss);
		ss_arr.Add(ss);
	}
	delete []rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=pModuleRight->FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			pModuleRight->DeleteString(ii);
	}
}

void CCfgModuleScopeDlg::OnDblclkListRemainModule() 
{
	OnBnAddModule();
	
}

void CCfgModuleScopeDlg::OnBnDelModule() 
{
	CListBox *pModuleLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_MODULE);
	CListBox *pModuleRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_MODULE);
	int nSel=pModuleLeft->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int i,*rgIndex=new int[nSel];
	nSel=pModuleLeft->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(i=0;i<nSel;i++)
	{
		pModuleLeft->GetText(rgIndex[i],ss);
		pModuleRight->AddString(ss);
		ss_arr.Add(ss);
	}
	delete []rgIndex;
	for(i=0;i<nSel;i++)
	{
		int ii=pModuleLeft->FindString(0,ss_arr[i]);
		if(ii!=LB_ERR)
			pModuleLeft->DeleteString(ii);
	}
}

void CCfgModuleScopeDlg::OnDblclkListAttachModule() 
{
	OnBnDelModule();
}

BOOL CCfgModuleScopeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char *csModule=new char[m_sModuleScope.GetLength()+1];
	_snprintf(csModule,m_sModuleScope.GetLength()+1,"%s",m_sModuleScope);
	int iModuleNo=FindAdaptNo(csModule);
	CListBox *pModuleLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_MODULE);
	CListBox *pModuleRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_MODULE);
	pModuleLeft->ResetContent();
	pModuleRight->ResetContent();
	CString ss;
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		ss.Format("%4d",pModule->iNo);
		pModuleRight->AddString(ss);
	}
	while(iModuleNo!=0)
	{
		ss.Format("%4d",iModuleNo);
		int ii=pModuleRight->FindString(0,ss);
		if(ii!=LB_ERR)	//找到此号并从右侧删除
		{
			pModuleRight->DeleteString(ii);
			pModuleLeft->AddString(ss);
		}
		iModuleNo=FindAdaptNo(NULL);
	}
	delete []csModule;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCfgModuleScopeDlg::OnOK() 
{
	CListBox *pModuleLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_MODULE);
	int nCount=pModuleLeft->GetCount();
	CString ss;
	m_sModuleScope.Empty();
	for(int i=0;i<nCount;i++)
	{
		pModuleLeft->GetText(i,ss);
		ss.TrimLeft();
		if(i<nCount-1)
			m_sModuleScope+=ss+",";
		else
			m_sModuleScope+=ss;
	}
	CDialog::OnOK();
}

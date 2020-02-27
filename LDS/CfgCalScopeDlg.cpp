// CfgCalScopeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "CfgCalScopeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCfgCalScopeDlg dialog

#if defined(__LDS_)||defined(__TSA_)
CCfgCalScopeDlg::CCfgCalScopeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfgCalScopeDlg::IDD, pParent)
	, m_bVerifyFoundationMode(FALSE)
	, m_iFoundationByStdLoad0(TRUE)
{
	//{{AFX_DATA_INIT(CCfgCalScopeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iCalMode=MYL_CAL;	//常规验算
	m_bUpdateBoltInfo=FALSE;
	CString ss;
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		ss.Format("%d,",pModule->iNo);
		m_sModuleScope+=ss;
	}
	ATOM_LIST<SEGI> segList;
	for(CWorkStatus *pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
	{
		if(pStatus->m_cMainLoadType>=5&&pStatus->m_cMainLoadType<=8)
			continue;	//常规验算中不处理抗震验算工况
		segList.append(pStatus->iNo);
	}
	m_sWorkStatusScope = CTower::SegNoListToString(segList);
}


void CCfgCalScopeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCfgCalScopeDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_E_CASE_SCOPE_STR, m_sWorkStatusScope);
	DDV_MaxChars(pDX, m_sWorkStatusScope, 200);
	DDX_Check(pDX, IDC_CHK_UPDATE_BOLT_INFO, m_bUpdateBoltInfo);
	DDX_Radio(pDX, IDC_RDO_MYL_STRESS_CAL, m_iCalMode);
	DDX_Check(pDX, IDC_CHK_FOUNDATION_MODE, m_bVerifyFoundationMode);
	DDX_Radio(pDX, IDC_RDO_FOUNDATION_BY_STDLOAD, m_iFoundationByStdLoad0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCfgCalScopeDlg, CDialog)
	//{{AFX_MSG_MAP(CCfgCalScopeDlg)
	ON_BN_CLICKED(IDC_BN_ADD_MODULE, OnBnAddModule)
	ON_BN_CLICKED(IDC_BN_DEL_MODULE, OnBnDelModule)
	ON_LBN_DBLCLK(IDC_LIST_ATTACH_MODULE, OnDblclkListAttachModule)
	ON_LBN_DBLCLK(IDC_LIST_REMAIN_MODULE, OnDblclkListRemainModule)
	ON_BN_CLICKED(IDC_RDO_MYL_STRESS_CAL, &CCfgCalScopeDlg::OnBnCalMode)
	ON_BN_CLICKED(IDC_RDO_UNI_SIZE_OPT_STRESS_CAL, &CCfgCalScopeDlg::OnBnCalMode)
	ON_BN_CLICKED(IDC_RDO_MODEL_STRESS_CHECK, &CCfgCalScopeDlg::OnBnCalMode)
	ON_BN_CLICKED(IDC_CHK_FOUNDATION_MODE, &CCfgCalScopeDlg::OnClickedChkFoundationMode)
	ON_BN_CLICKED(IDC_CHK_UPDATE_BOLT_INFO, &CCfgCalScopeDlg::OnBnClickedChkUpdateBoltInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCfgCalScopeDlg message handlers

void CCfgCalScopeDlg::OnBnAddModule() 
{
	CListBox *pModuleLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_MODULE);
	CListBox *pModuleRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_MODULE);
	int nSel=pModuleRight->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int *rgIndex=new int[nSel];
	nSel=pModuleRight->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(int i=0;i<nSel;i++)
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

void CCfgCalScopeDlg::OnDblclkListRemainModule() 
{
	OnBnAddModule();
	
}

void CCfgCalScopeDlg::OnBnDelModule() 
{
	CListBox *pModuleLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_MODULE);
	CListBox *pModuleRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_MODULE);
	int nSel=pModuleLeft->GetSelCount();
	if(nSel==LB_ERR)	//无被选中项
		return;
	int *rgIndex=new int[nSel];
	nSel=pModuleLeft->GetSelItems(nSel,rgIndex);
	CString ss;
	CStringArray ss_arr;
	for(int i=0;i<nSel;i++)
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

void CCfgCalScopeDlg::OnDblclkListAttachModule() 
{
	OnBnDelModule();
}

BOOL CCfgCalScopeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	OnBnCalMode();
	char *csModule=new char[m_sModuleScope.GetLength()+1];
	_snprintf(csModule,m_sModuleScope.GetLength()+1,"%s",m_sModuleScope);
	int iModuleNo=FindAdaptNo(csModule);
	CListBox *pModuleLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_MODULE);
	CListBox *pModuleRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_MODULE);
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

void CCfgCalScopeDlg::OnOK() 
{
	CListBox *pModuleLeft=(CListBox*)GetDlgItem(IDC_LIST_ATTACH_MODULE);
	int nCount=pModuleLeft->GetCount();
	CString ss;
	BYTE byteConst[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	m_sModuleScope.Empty();
	ARRAY_LIST<BYTE> moduleByteArr;
	moduleByteArr.SetSize(24);
	int maxModuleBytes=0,maxCaseBytes=0;
	for(int i=0;i<nCount;i++)
	{
		pModuleLeft->GetText(i,ss);
		ss.TrimLeft();
		int iNo=atoi(ss);
		int byte=(iNo-1)/8,bit=(iNo-1)%8;
		if(byte>=moduleByteArr.GetSize())
			moduleByteArr.SetSize(byte+1);
		if(byte+1>maxModuleBytes)
			maxModuleBytes=byte+1;
		moduleByteArr[byte]|=byteConst[bit];
	}
	CXhChar50 bodyStr;
	MakeBytesToString(moduleByteArr.m_pData,maxModuleBytes,bodyStr,51);
	m_sModuleScope=(char*)bodyStr;
	CDialog::OnOK();
}

void CCfgCalScopeDlg::UpdateFoundationVerifyModeUI()
{
	GetDlgItem(IDC_CHK_FOUNDATION_MODE)->EnableWindow(m_iCalMode==CHK_CAL);
	if(m_bUpdateBoltInfo)
	{
		m_bVerifyFoundationMode=FALSE;
		((CButton*)GetDlgItem(IDC_CHK_FOUNDATION_MODE))->SetCheck(FALSE);
		GetDlgItem(IDC_RDO_FOUNDATION_BY_STDLOAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_FOUNDATION_BY_DESIGNLOAD)->EnableWindow(FALSE);
	}
	else if(m_bVerifyFoundationMode)
	{
		GetDlgItem(IDC_RDO_FOUNDATION_BY_STDLOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_FOUNDATION_BY_DESIGNLOAD)->EnableWindow(TRUE);
	}
}
void CCfgCalScopeDlg::OnBnCalMode()
{
	UpdateData();
	GetDlgItem(IDC_CHK_UPDATE_BOLT_INFO)->EnableWindow(m_iCalMode==CHK_CAL);
	m_bUpdateBoltInfo|=(m_iCalMode!=CHK_CAL);
	UpdateFoundationVerifyModeUI();
	UpdateData(FALSE);
}

void CCfgCalScopeDlg::OnBnClickedChkUpdateBoltInfo()
{
	UpdateData();
	UpdateFoundationVerifyModeUI();
}

void CCfgCalScopeDlg::OnClickedChkFoundationMode()
{
	UpdateData();
	if(m_bVerifyFoundationMode)
	{
		m_bUpdateBoltInfo=FALSE;
		((CButton*)GetDlgItem(IDC_CHK_UPDATE_BOLT_INFO))->SetCheck(FALSE);
		GetDlgItem(IDC_RDO_FOUNDATION_BY_STDLOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_RDO_FOUNDATION_BY_DESIGNLOAD)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_RDO_FOUNDATION_BY_STDLOAD)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_FOUNDATION_BY_DESIGNLOAD)->EnableWindow(FALSE);
	}
}
#endif

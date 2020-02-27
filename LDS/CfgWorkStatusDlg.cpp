// CfgWorkStatusDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "CfgWorkStatusDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if defined(__TSA_)||defined(__LDS_)

/////////////////////////////////////////////////////////////////////////////
// CCfgWorkStatusDlg dialog


CCfgWorkStatusDlg::CCfgWorkStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCfgWorkStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCfgWorkStatusDlg)
#ifdef AFX_TARG_ENU_ENGLISH
	m_sLeftStr = _T("Model is suitable for working condition");
	m_sRightStr = _T("Model isn't suitable for working condition:");
#else 
	m_sLeftStr = _T("模型适用工况:");
	m_sRightStr = _T("模型不适用工况:");
#endif
	//}}AFX_DATA_INIT
	m_pListBoxAttach=m_pListBoxRemain=NULL;
}


void CCfgWorkStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCfgWorkStatusDlg)
	DDX_Text(pDX, IDC_STR_LEFT, m_sLeftStr);
	DDX_Text(pDX, IDC_STR_RIGHT, m_sRightStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCfgWorkStatusDlg, CDialog)
	//{{AFX_MSG_MAP(CCfgWorkStatusDlg)
	ON_BN_CLICKED(IDC_BN_ADD_WORK_STATUS, OnBnAddWorkStatus)
	ON_BN_CLICKED(IDC_BN_DEL_WORK_STATUS, OnBnDelWorkStatus)
	ON_LBN_DBLCLK(IDC_LIST_RELATION_WORK_STATUS, OnBnDelWorkStatus)
	ON_LBN_DBLCLK(IDC_LIST_UNRELATION_WORK_STATUS, OnBnAddWorkStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCfgWorkStatusDlg message handlers
BOOL CCfgWorkStatusDlg::OnInitDialog() 
{
	m_pListBoxAttach=(CListBox*)GetDlgItem(IDC_LIST_RELATION_WORK_STATUS);
	m_pListBoxRemain=(CListBox*)GetDlgItem(IDC_LIST_UNRELATION_WORK_STATUS);
	for(CWorkStatus *pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
	{
		BOOL bAdaptStatus=FALSE;
		for(THANDLE *pHandle=list.GetFirst();pHandle;pHandle=list.GetNext())
		{
			if(*pHandle==pStatus->handle)
			{
				bAdaptStatus=TRUE;
				break;
			}
		}
		if(bAdaptStatus)	//适用工况
		{
			int iCurSel=m_pListBoxAttach->AddString(pStatus->description);
			m_pListBoxAttach->SetItemData(iCurSel,pStatus->handle);
		}
		else
		{
			int iCurSel=m_pListBoxRemain->AddString(pStatus->description);
			m_pListBoxRemain->SetItemData(iCurSel,pStatus->handle);
		}
	}
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCfgWorkStatusDlg::OnBnAddWorkStatus() 
{
	m_pListBoxAttach=(CListBox*)GetDlgItem(IDC_LIST_RELATION_WORK_STATUS);
	m_pListBoxRemain=(CListBox*)GetDlgItem(IDC_LIST_UNRELATION_WORK_STATUS);
	int iCurSel = m_pListBoxRemain->GetCurSel();
	if(iCurSel>=0)
	{
		DWORD dw=m_pListBoxRemain->GetItemData(iCurSel);
		CString sText;
		m_pListBoxRemain->GetText(iCurSel,sText);
		m_pListBoxRemain->DeleteString(iCurSel);
		list.append((THANDLE)dw);
		int iSel=m_pListBoxAttach->AddString(sText);
		m_pListBoxAttach->SetItemData(iSel,dw);
	}
}

void CCfgWorkStatusDlg::OnBnDelWorkStatus() 
{
	m_pListBoxAttach=(CListBox*)GetDlgItem(IDC_LIST_RELATION_WORK_STATUS);
	m_pListBoxRemain=(CListBox*)GetDlgItem(IDC_LIST_UNRELATION_WORK_STATUS);
	if(list.GetNodeNum()==0)
		return;
	int iCurSel = m_pListBoxAttach->GetCurSel();
	if(iCurSel>=0)
	{
		DWORD dw=m_pListBoxAttach->GetItemData(iCurSel);
		CString sText;
		m_pListBoxAttach->GetText(iCurSel,sText);
		m_pListBoxAttach->DeleteString(iCurSel);
		for(THANDLE *pHandle=list.GetFirst();pHandle;pHandle=list.GetNext())
		{
			if((DWORD)*pHandle==dw)
			{
				list.DeleteCursor();
				break;
			}
		}
		int iSel=m_pListBoxRemain->AddString(sText);
		m_pListBoxRemain->SetItemData(iSel,dw);
	}
}
#endif

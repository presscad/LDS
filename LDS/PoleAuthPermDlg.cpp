// PoleAuthPermDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "PoleAuthPermDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPoleAuthPermDlg dialog


CPoleAuthPermDlg::CPoleAuthPermDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPoleAuthPermDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPoleAuthPermDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bEnableStartModify=m_bEnableBasicModify=m_bEnableEndModify=TRUE;
}


void CPoleAuthPermDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPoleAuthPermDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPoleAuthPermDlg, CDialog)
	//{{AFX_MSG_MAP(CPoleAuthPermDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoleAuthPermDlg message handlers

BOOL CPoleAuthPermDlg::OnInitDialog() 
{
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_POLE_START_USER);
	CString ss;
	int i,iCur,iSel=-1;
	for(i=0;i<31;i++)
	{
		if(strlen(user_arr[i].m_sUserName)>0)
		{
			ss.Format("%2d.%s",user_arr[i].m_nUserId,user_arr[i].m_sUserName);
			iCur=pListBox->AddString(ss);
			pListBox->SetItemData(iCur,user_arr[i].m_nUserId);
			if(m_dwStartPermission==GetPermWord(user_arr[i].m_nUserId))
				iSel=iCur;
		}
	}
	pListBox->SetCurSel(iSel);
	pListBox->EnableWindow(m_bEnableStartModify);

	pListBox=(CListBox*)GetDlgItem(IDC_LIST_POLE_BASIC_USER);
	iSel=-1;
	for(i=0;i<31;i++)
	{
		if(strlen(user_arr[i].m_sUserName)>0)
		{
			ss.Format("%2d.%s",user_arr[i].m_nUserId,user_arr[i].m_sUserName);
			iCur=pListBox->AddString(ss);
			pListBox->SetItemData(iCur,user_arr[i].m_nUserId);
			if(m_dwBasicPermission==GetPermWord(user_arr[i].m_nUserId))
				iSel=iCur;
		}
	}
	pListBox->SetCurSel(iSel);
	pListBox->EnableWindow(m_bEnableBasicModify);

	pListBox=(CListBox*)GetDlgItem(IDC_LIST_POLE_END_USER);
	iSel=-1;
	for(i=0;i<31;i++)
	{
		if(strlen(user_arr[i].m_sUserName)>0)
		{
			ss.Format("%2d.%s",user_arr[i].m_nUserId,user_arr[i].m_sUserName);
			iCur=pListBox->AddString(ss);
			pListBox->SetItemData(iCur,user_arr[i].m_nUserId);
			if(m_dwEndPermission==GetPermWord(user_arr[i].m_nUserId))
				iSel=iCur;
		}
	}
	pListBox->SetCurSel(iSel);
	pListBox->EnableWindow(m_bEnableEndModify);

	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPoleAuthPermDlg::OnOK() 
{
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_POLE_START_USER);
	m_dwStartPermission=GetPermWord(pListBox->GetItemData(pListBox->GetCurSel()));
	pListBox=(CListBox*)GetDlgItem(IDC_LIST_POLE_BASIC_USER);
	m_dwBasicPermission=GetPermWord(pListBox->GetItemData(pListBox->GetCurSel()));
	pListBox=(CListBox*)GetDlgItem(IDC_LIST_POLE_END_USER);
	m_dwEndPermission=GetPermWord(pListBox->GetItemData(pListBox->GetCurSel()));
	CDialog::OnOK();
}

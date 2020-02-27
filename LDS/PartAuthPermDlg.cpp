// PartAuthPermDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "PartAuthPermDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartAuthPermDlg dialog


CPartAuthPermDlg::CPartAuthPermDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartAuthPermDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartAuthPermDlg)
	//}}AFX_DATA_INIT
	m_bEnableModify=TRUE;
	m_dwPermission=0;
}


void CPartAuthPermDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartAuthPermDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartAuthPermDlg, CDialog)
	//{{AFX_MSG_MAP(CPartAuthPermDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartAuthPermDlg message handlers

BOOL CPartAuthPermDlg::OnInitDialog() 
{
	
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_USER);
	CString ss;
	int iSel=-1;
	for(int i=0;i<31;i++)
	{
		if(strlen(user_arr[i].m_sUserName)>0)
		{
			ss.Format("%2d.%s",user_arr[i].m_nUserId,user_arr[i].m_sUserName);
			int iCur=pListBox->AddString(ss);
			pListBox->SetItemData(iCur,user_arr[i].m_nUserId);
			if(GetPermWord(user_arr[i].m_nUserId)&m_dwPermission)
				iSel=iCur;
		}
	}
	pListBox->SetCurSel(iSel);
	pListBox->EnableWindow(m_bEnableModify);
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPartAuthPermDlg::OnOK() 
{
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_USER);
	m_dwPermission=GetPermWord(pListBox->GetItemData(pListBox->GetCurSel()));
	CDialog::OnOK();
}

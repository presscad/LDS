// UpdateUserDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "UpdateUserDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpdateUserDataDlg dialog

CUpdateUserDataDlg::CUpdateUserDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateUserDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUpdateUserDataDlg)
	m_sCurListString = _T("");
	m_sUserStatus = _T("");
	//}}AFX_DATA_INIT
	m_dwUpdateUser=0;
}


void CUpdateUserDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateUserDataDlg)
	DDX_LBString(pDX, IDC_LIST_USER, m_sCurListString);
	DDX_Text(pDX, IDC_S_USER_STATUS, m_sUserStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateUserDataDlg, CDialog)
	//{{AFX_MSG_MAP(CUpdateUserDataDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateUserDataDlg message handlers

BOOL CUpdateUserDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_USER);
	CString ss;

	for(int i=0;i<31;i++)
	{
		if(strlen(user_arr[i].m_sUserName)>0)
		{
			ss.Format("%2d.%s",user_arr[i].m_nUserId,user_arr[i].m_sUserName);
			int iCur=pListBox->AddString(ss);
			pListBox->SetItemData(iCur,i);
			if(i==0&&PERM_TOWER_ADMIN&m_dwUpdateUser)
				pListBox->SetSel(iCur);
			else if(GetSingleWord(i)&m_dwUpdateUser)
				pListBox->SetSel(iCur);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUpdateUserDataDlg::OnOK() 
{
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_USER);
	int nSel=pListBox->GetSelCount();
	int *sel_arr=new int[nSel];
	pListBox->GetSelItems(nSel,sel_arr);
	m_dwUpdateUser=0;
	for(int i=0;i<nSel;i++)
	{
		DWORD dw=pListBox->GetItemData(sel_arr[i]);
		if(dw==0)
			m_dwUpdateUser|=PERM_TOWER_ADMIN;
		else
			m_dwUpdateUser|=GetSingleWord(dw);
	}
	delete []sel_arr;
	CDialog::OnOK();
}

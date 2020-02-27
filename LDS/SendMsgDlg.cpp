// SendMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "SendMsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendMsgDlg dialog


CSendMsgDlg::CSendMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendMsgDlg)
	m_sMsgContent = _T("");
	m_iMsgReceiver = 0;
	//}}AFX_DATA_INIT
}


void CSendMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendMsgDlg)
	DDX_Text(pDX, IDC_E_MSG_CONTENT, m_sMsgContent);
	DDV_MaxChars(pDX, m_sMsgContent, 500);
	DDX_CBIndex(pDX, IDC_CMB_MSG_RECEIVER, m_iMsgReceiver);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CSendMsgDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendMsgDlg message handlers

BOOL CSendMsgDlg::OnInitDialog() 
{
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_MSG_RECEIVER);
	CString ss;
	BRIEF_USER user_arr[32];
	theApp.GetUserInfo(user_arr);
	for(int i=0;i<31;i++)
	{
		if(user_arr[i].m_bInUse&&strlen(user_arr[i].m_sUserName)>0)
		{
			ss.Format("%s",user_arr[i].m_sUserName);
			int iCur=pCMB->AddString(ss);
			if(ss.CompareNoCase(m_sMsgReceiver)==0)
				pCMB->SetCurSel(iCur);
		}
	}
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendMsgDlg::OnOK() 
{
	CDialog::OnOK();
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_MSG_RECEIVER);
	pCMB->GetLBText(m_iMsgReceiver,	m_sMsgReceiver);
}

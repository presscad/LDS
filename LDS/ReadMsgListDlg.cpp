// ReadMsgListDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "ReadMsgListDlg.h"
#include "SendMsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReadMsgListDlg dialog


CReadMsgListDlg::CReadMsgListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReadMsgListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReadMsgListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CReadMsgListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReadMsgListDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReadMsgListDlg, CDialog)
	//{{AFX_MSG_MAP(CReadMsgListDlg)
	ON_BN_CLICKED(ID_BN_READ_MSG, OnBnReadMsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadMsgListDlg message handlers

BOOL CReadMsgListDlg::OnInitDialog() 
{
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_MSG);
	CString ss;
	char head[11]="";
	for(MSG_STRU *pMsg=theApp.m_arrMsg.GetFirst();pMsg;pMsg=theApp.m_arrMsg.GetNext())
	{
		strncpy(head,pMsg->msg,10);
		if(strlen(pMsg->msg)>10)
			ss.Format("%s->%s...",pMsg->user,head);
		else
			ss.Format("%s->%s",pMsg->user,head);
		pListBox->AddString(ss);
	}
	pListBox->SetCurSel(0);
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CReadMsgListDlg::OnBnReadMsg() 
{
	CString ss;
	CListBox *pListBox=(CListBox*)GetDlgItem(IDC_LIST_MSG);
	int iSel=pListBox->GetCurSel();
#ifdef AFX_TARG_ENU_ENGLISH
	ss.Format("Information from %s：%s\n if reply？",theApp.m_arrMsg[iSel].user,theApp.m_arrMsg[iSel].msg);
	if(MessageBox(ss,"Short Information",MB_ICONINFORMATION|MB_YESNO)==IDYES)
#else 
	ss.Format("来自%s的消息：%s\n是否回复？",theApp.m_arrMsg[iSel].user,theApp.m_arrMsg[iSel].msg);
	if(MessageBox(ss,"短消息",MB_ICONINFORMATION|MB_YESNO)==IDYES)
#endif
	{
		CSendMsgDlg send_msg;;
		send_msg.m_sMsgReceiver=theApp.m_arrMsg[iSel].user;
		send_msg.m_sMsgContent=theApp.m_arrMsg[iSel].msg;
		if(send_msg.DoModal()==IDOK)
			theApp.SendMsgToOther(send_msg.m_sMsgReceiver,send_msg.m_sMsgContent);
	}
	pListBox->DeleteString(iSel);
	theApp.m_arrMsg.DeleteAt(iSel);
}

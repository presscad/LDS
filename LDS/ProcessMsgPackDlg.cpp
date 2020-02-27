//<LOCALE_TRANSLATE Confirm by hxr/>
// ProcessMsgPackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "ProcessMsgPackDlg.h"
#include "ReadMsgListDlg.h"
#include "Buffer.h"
#include "lds_part.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcessMsgPackDlg dialog


CProcessMsgPackDlg::CProcessMsgPackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessMsgPackDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcessMsgPackDlg)
#ifdef AFX_TARG_ENU_ENGLISH
	m_sReadMsg = _T("There is 0 unread message at present");
	m_sUpdateDataFile = _T("There is 0 user's data need to be updated at present");
#else
	m_sReadMsg = _T("目前有0条消息未阅读");
	m_sUpdateDataFile = _T("目前有0个用户的数据需要更新");
#endif
	//}}AFX_DATA_INIT
}


void CProcessMsgPackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessMsgPackDlg)
	DDX_Text(pDX, IDC_S_READ_MSG, m_sReadMsg);
	DDX_Text(pDX, IDC_S_UPDATE_DATA_FILE, m_sUpdateDataFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcessMsgPackDlg, CDialog)
	//{{AFX_MSG_MAP(CProcessMsgPackDlg)
	ON_BN_CLICKED(ID_BN_UPDATE_DATA_FILE, OnBnUpdateDataFile)
	ON_BN_CLICKED(ID_BN_READ_MSG, OnBnReadMsg)
	ON_BN_CLICKED(ID_BN_UPDATE_PERM, OnBnUpdatePerm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessMsgPackDlg message handlers

BOOL CProcessMsgPackDlg::OnInitDialog() 
{
	if(!theApp.m_bNeedUpdatePerm)
		GetDlgItem(ID_BN_UPDATE_PERM)->EnableWindow(FALSE);

	if(theApp.m_dwUpdateWord>0)
	{
		int n=0;
		for(int i=0;i<31;i++)
		{
			if(theApp.m_dwUpdateWord&GetSingleWord(i))
				n++;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		m_sUpdateDataFile.Format("There are %d user's data need to be updated at present",n);
#else
		m_sUpdateDataFile.Format("目前有%d个用户的数据需要更新",n);
#endif
	}
	else
		GetDlgItem(ID_BN_UPDATE_DATA_FILE)->EnableWindow(FALSE);
	if(theApp.m_arrMsg.GetNodeNum()>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_sReadMsg.Format("There are %d unread message at present",theApp.m_arrMsg.GetNodeNum());
#else
		m_sReadMsg.Format("目前有%d条消息未阅读",theApp.m_arrMsg.GetNodeNum());
#endif
	}
	else
		GetDlgItem(ID_BN_READ_MSG)->EnableWindow(FALSE);
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcessMsgPackDlg::OnBnUpdateDataFile() 
{
	theApp.GetLDSDoc()->UpdateDataFile();
	int n=0;
	for(int i=0;i<31;i++)
	{
		if(theApp.m_dwUpdateWord&GetPermWord(i))
			n++;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	m_sUpdateDataFile.Format("There are %d user's data need to be updated at present",n);
#else
	m_sUpdateDataFile.Format("目前有%d个用户的数据需要更新",n);
#endif
	if(n<=0)
		GetDlgItem(ID_BN_UPDATE_DATA_FILE)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CProcessMsgPackDlg::OnBnReadMsg() 
{
	CReadMsgListDlg readmsgdlg;
	readmsgdlg.DoModal();
	if(theApp.m_arrMsg.GetNodeNum()==0)
		GetDlgItem(ID_BN_READ_MSG)->EnableWindow(FALSE);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sReadMsg.Format("There are %d unread message at present",theApp.m_arrMsg.GetNodeNum());
#else
	m_sReadMsg.Format("目前有%d条消息未阅读",theApp.m_arrMsg.GetNodeNum());
#endif
	UpdateData(FALSE);
}

void CProcessMsgPackDlg::OnBnUpdatePerm() 
{
	theApp.GetLDSDoc()->UpdatePermList();
	theApp.m_bNeedUpdatePerm=FALSE;
	GetDlgItem(ID_BN_UPDATE_PERM)->EnableWindow(FALSE);
	CDialog::OnOK();
}

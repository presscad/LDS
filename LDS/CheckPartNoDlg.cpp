// CheckPartNoDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "CheckPartNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckPartNoDlg dialog


CCheckPartNoDlg::CCheckPartNoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckPartNoDlg::IDD, pParent)
	, m_bDisplayFlNoCtrl(true)
{
	//{{AFX_DATA_INIT(CCheckPartNoDlg)
	m_fScope = 0.5;
	m_sSegStr = _T("*");
	m_bReservFlNo = TRUE;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption="Part Number Check";
#else
	m_sCaption="¹¹¼þºÅ¼ì²é";
#endif
}


void CCheckPartNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckPartNoDlg)
	DDX_Text(pDX, IDC_E_SCOPE, m_fScope);
	DDX_Text(pDX, IDC_E_SEG_STR, m_sSegStr);
	DDX_Check(pDX, IDC_CHK_RESERVE_FLNO, m_bReservFlNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckPartNoDlg, CDialog)
	//{{AFX_MSG_MAP(CCheckPartNoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckPartNoDlg message handlers

BOOL CCheckPartNoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_sCaption);
#ifndef __COMPLEX_PART_INC_
	if(m_bDisplayFlNoCtrl)
		GetDlgItem(IDC_CHK_RESERVE_FLNO)->ShowWindow(SW_SHOW);
	else
		GetDlgItem(IDC_CHK_RESERVE_FLNO)->ShowWindow(SW_HIDE);
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

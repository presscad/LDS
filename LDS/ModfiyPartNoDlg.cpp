// ModfiyPartNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "ModfiyPartNoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModfiyPartNoDlg dialog


CModfiyPartNoDlg::CModfiyPartNoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModfiyPartNoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModfiyPartNoDlg)
	m_sType = _T("");
	m_sPartNo = _T("");
	//}}AFX_DATA_INIT
}


void CModfiyPartNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModfiyPartNoDlg)
	DDX_Control(pDX, IDC_EDIT1, m_editPartNo);
	DDX_CBString(pDX, IDC_COMBO1, m_sType);
	DDX_Text(pDX, IDC_EDIT1, m_sPartNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModfiyPartNoDlg, CDialog)
	//{{AFX_MSG_MAP(CModfiyPartNoDlg)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangePartNoType)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModfiyPartNoDlg message handlers

void CModfiyPartNoDlg::OnSelchangePartNoType() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_sType[0]=='N'||m_sType[0]=='R')
		m_editPartNo.SetReadOnly(TRUE);
	else
		m_editPartNo.SetReadOnly(FALSE);
}

BOOL CModfiyPartNoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_sType[0]=='N'||m_sType[0]=='R')
		m_editPartNo.SetReadOnly(TRUE);
	else
		m_editPartNo.SetReadOnly(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModfiyPartNoDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
}

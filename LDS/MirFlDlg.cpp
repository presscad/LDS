// MirFlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "MirFlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMirFlDlg dialog


CMirFlDlg::CMirFlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMirFlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMirFlDlg)
	m_bCreateElbowPlate = FALSE;
	m_bMirSideFace = FALSE;
	m_iMirMsg = 0;
	//}}AFX_DATA_INIT
}


void CMirFlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMirFlDlg)
	DDX_Check(pDX, IDC_CHK_CREATE_ELBOW_PLATE, m_bCreateElbowPlate);
	DDX_Check(pDX, IDC_CHK_MIR_SIDE_FACE, m_bMirSideFace);
	DDX_CBIndex(pDX, IDC_CMB_MIR_MSG, m_iMirMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMirFlDlg, CDialog)
	//{{AFX_MSG_MAP(CMirFlDlg)
	ON_BN_CLICKED(IDC_CHK_CREATE_ELBOW_PLATE, OnChkTypeModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMirFlDlg message handlers

void CMirFlDlg::OnChkTypeModify() 
{
	// TODO: Add your control notification handler code here
	
}

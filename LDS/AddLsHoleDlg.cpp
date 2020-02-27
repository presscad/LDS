// AddLsHoleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "AddLsHoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddLsHoleDlg dialog


CAddLsHoleDlg::CAddLsHoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddLsHoleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddLsHoleDlg)
	m_iLsHole = 2;
	m_fLsPosX = 0.0;
	m_fLsPosY = 0.0;
	//}}AFX_DATA_INIT
}


void CAddLsHoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddLsHoleDlg)
	DDX_CBIndex(pDX, IDC_CMB_LS_HOLE, m_iLsHole);
	DDX_Text(pDX, IDC_E_LS_HOLE_POS_X, m_fLsPosX);
	DDX_Text(pDX, IDC_E_LS_HOLE_POS_Y, m_fLsPosY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddLsHoleDlg, CDialog)
	//{{AFX_MSG_MAP(CAddLsHoleDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddLsHoleDlg message handlers

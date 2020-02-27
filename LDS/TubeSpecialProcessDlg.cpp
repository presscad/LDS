// TubeSpecialProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "TubeSpecialProcessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTubeSpecialProcessDlg dialog


CTubeSpecialProcessDlg::CTubeSpecialProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTubeSpecialProcessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTubeSpecialProcessDlg)
	m_bSlotC = FALSE;
	m_bPlaneCut = FALSE;
	m_bTwoSlot = FALSE;
	m_bTransect = FALSE;
	m_bSlotT = FALSE;
	m_bSlotU = FALSE;
	m_bSlotX = FALSE;
	//}}AFX_DATA_INIT
}


void CTubeSpecialProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTubeSpecialProcessDlg)
	DDX_Check(pDX, IDC_CHK_CSLOT, m_bSlotC);
	DDX_Check(pDX, IDC_CHK_PLANE_CUT, m_bPlaneCut);
	DDX_Check(pDX, IDC_CHK_TOW_SLOT, m_bTwoSlot);
	DDX_Check(pDX, IDC_CHK_TRANSECT, m_bTransect);
	DDX_Check(pDX, IDC_CHK_TSLOT, m_bSlotT);
	DDX_Check(pDX, IDC_CHK_USLOT, m_bSlotU);
	DDX_Check(pDX, IDC_CHK_XSLOT, m_bSlotX);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTubeSpecialProcessDlg, CDialog)
	//{{AFX_MSG_MAP(CTubeSpecialProcessDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTubeSpecialProcessDlg message handlers

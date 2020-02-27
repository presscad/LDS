// BackMirStyleDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "maptda.h"
#include "BackMirStyleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackMirStyleDlg dialog


CBackMirStyleDlg::CBackMirStyleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBackMirStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBackMirStyleDlg)
	m_iBackMirStyle = 0;
	m_bDrawRefLine = FALSE;
	m_bDimOnceRoundSamePart = TRUE;
	//}}AFX_DATA_INIT
}


void CBackMirStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackMirStyleDlg)
	DDX_Radio(pDX, IDC_RDO_MIR_AXIS_X, m_iBackMirStyle);
	DDX_Check(pDX, IDC_CHK_DRAW_REFLINE, m_bDrawRefLine);
	DDX_Check(pDX, IDC_CHK_DIM_ONCE_ROUND_SAME_PART, m_bDimOnceRoundSamePart);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackMirStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CBackMirStyleDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackMirStyleDlg message handlers

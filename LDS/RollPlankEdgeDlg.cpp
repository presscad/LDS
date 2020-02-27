// RollPlankEdgeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "RollPlankEdgeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRollPlankEdgeDlg dialog


CRollPlankEdgeDlg::CRollPlankEdgeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRollPlankEdgeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRollPlankEdgeDlg)
	m_fRollLineHeight = 50.0;
	m_fRollLineOffsetDist = 10.0;
	//}}AFX_DATA_INIT
}


void CRollPlankEdgeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRollPlankEdgeDlg)
	DDX_Text(pDX, IDC_E_ROLL_LINE_HEIGHT, m_fRollLineHeight);
	DDX_Text(pDX, IDC_E_ROLL_LINE_OFFSET_DIST, m_fRollLineOffsetDist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRollPlankEdgeDlg, CDialog)
	//{{AFX_MSG_MAP(CRollPlankEdgeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRollPlankEdgeDlg message handlers

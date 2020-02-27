// NewParallelPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "NewParallelPlateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewParallelPlateDlg dialog


CNewParallelPlateDlg::CNewParallelPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewParallelPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewParallelPlateDlg)
	m_iMaterial = 0;
	m_nPlankThick = 0;
	m_fVertOffsetDist = 0.0;
	m_sPartNo = _T("");
	//}}AFX_DATA_INIT
}


void CNewParallelPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewParallelPlateDlg)
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_PLANK_THICK, m_nPlankThick);
	DDX_Text(pDX, IDC_E_VERT_OFFSET_DIST, m_fVertOffsetDist);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewParallelPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CNewParallelPlateDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewParallelPlateDlg message handlers

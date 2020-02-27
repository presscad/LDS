// NewGradientPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "NewGradientPlateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewGradientPlateDlg dialog


CNewGradientPlateDlg::CNewGradientPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewGradientPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewGradientPlateDlg)
	m_iMaterial = 0;
	m_fGradientAngle = 90.0;
	m_sPartNo = _T("");
	m_nPlankThick = 10;
	m_fStartX = 0.0;
	m_fStartY = 0.0;
	m_fEndX = 0.0;
	m_fEndY = 100.0;
	m_fHigh = 50.0;
	//}}AFX_DATA_INIT
}


void CNewGradientPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewGradientPlateDlg)
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_GRADIENT_ANGLE, m_fGradientAngle);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDX_Text(pDX, IDC_E_PLANK_THICK, m_nPlankThick);
	DDX_Text(pDX, IDC_E_START_X, m_fStartX);
	DDX_Text(pDX, IDC_E_START_Y, m_fStartY);
	DDX_Text(pDX, IDC_E_END_X, m_fEndX);
	DDX_Text(pDX, IDC_E_END_Y, m_fEndY);
	DDX_Text(pDX, IDC_E_PLANK_HIGH, m_fHigh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewGradientPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CNewGradientPlateDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewGradientPlateDlg message handlers

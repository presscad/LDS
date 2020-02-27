// AddFillPlankDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "AddFillPlankDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddFillPlankDlg dialog


CAddFillPlankDlg::CAddFillPlankDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFillPlankDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddFillPlankDlg)
	m_fHigh = 0.0;
	m_sPartNo = _T("");
	m_fBaseX = 0.0;
	m_fBaseY = 0.0;
	m_fThick = 0.0;
	m_fWidth = 0.0;
	m_iPlankDirect = 0;
	m_fBaseZ = 0.0;
	//}}AFX_DATA_INIT
}


void CAddFillPlankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddFillPlankDlg)
	DDX_Text(pDX, IDC_E_HIGH, m_fHigh);
	DDX_Text(pDX, IDC_E_PART_NO, m_sPartNo);
	DDV_MaxChars(pDX, m_sPartNo, 8);
	DDX_Text(pDX, IDC_E_BASE_X, m_fBaseX);
	DDX_Text(pDX, IDC_E_BASE_Y, m_fBaseY);
	DDX_Text(pDX, IDC_E_THICK, m_fThick);
	DDX_Text(pDX, IDC_E_WIDTH, m_fWidth);
	DDX_Radio(pDX, IDC_RDO_FILL_PLANK_DIRECTION, m_iPlankDirect);
	DDX_Text(pDX, IDC_E_BASE_Z, m_fBaseZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddFillPlankDlg, CDialog)
	//{{AFX_MSG_MAP(CAddFillPlankDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddFillPlankDlg message handlers

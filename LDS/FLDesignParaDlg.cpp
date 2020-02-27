// FLDesignParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "FLDesignParaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFLDesignParaDlg dialog


CFLDesignParaDlg::CFLDesignParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFLDesignParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFLDesignParaDlg)
	m_bFlCoupleDesign = TRUE;
	m_bLayoutElbowPlate = TRUE;
	//}}AFX_DATA_INIT
	m_bCanFlCoupleDesign=TRUE;
}


void CFLDesignParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFLDesignParaDlg)
	DDX_Check(pDX, IDC_CHK_FL_COUPLE_DESIGN, m_bFlCoupleDesign);
	DDX_Check(pDX, IDC_CHK_LAYOUT_ELBOW_PLATE, m_bLayoutElbowPlate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFLDesignParaDlg, CDialog)
	//{{AFX_MSG_MAP(CFLDesignParaDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFLDesignParaDlg message handlers

BOOL CFLDesignParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(!m_bCanFlCoupleDesign)
		m_bFlCoupleDesign=FALSE;
	GetDlgItem(IDC_CHK_LAYOUT_ELBOW_PLATE)->EnableWindow(m_bLayoutElbowPlate);
	GetDlgItem(IDC_CHK_FL_COUPLE_DESIGN)->EnableWindow(m_bCanFlCoupleDesign);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

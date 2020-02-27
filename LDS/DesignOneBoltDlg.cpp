// DesignOneBoltDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "DesignOneBoltDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignOneBoltDlg dialog


CDesignOneBoltDlg::CDesignOneBoltDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignOneBoltDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignOneBoltDlg)
	m_bCheckUpLsNum = TRUE;
	m_bInterNodeBolt = TRUE;
	m_bSingleAngleEndBolt = TRUE;
	m_bSingleBoltNoPlate = TRUE;
	//}}AFX_DATA_INIT
}


void CDesignOneBoltDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignOneBoltDlg)
	DDX_Check(pDX, IDC_CHK_CHECK_UP_LS_NUM, m_bCheckUpLsNum);
	DDX_Check(pDX, IDC_CHK_INTER_NODE_BOLT, m_bInterNodeBolt);
	DDX_Check(pDX, IDC_CHK_SINGLE_ANGLE_END_BOLT, m_bSingleAngleEndBolt);
	DDX_Check(pDX, IDC_CHK_SINGLE_BOLT_NO_PLATE, m_bSingleBoltNoPlate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignOneBoltDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignOneBoltDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignOneBoltDlg message handlers

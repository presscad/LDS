// OddmentDesignDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "OddmentDesignDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COddmentDesignDlg dialog
COddmentDesignDlg::COddmentDesignDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(COddmentDesignDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COddmentDesignDlg)
	m_iOddCalStyle = 0;
	m_fCollideDist = g_sysPara.VertexDist;
	m_hRefPart1 = _T("");
	m_hRefPart2 = _T("");
	m_iPickObjType=CLS_LINEPART;
	m_bPauseBreakExit=FALSE;
	m_bDispSelectBtn=TRUE;
	//}}AFX_DATA_INIT
}


void COddmentDesignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COddmentDesignDlg)
	DDX_CBIndex(pDX, IDC_CMB_ODD_CAL_STYLE, m_iOddCalStyle);
	DDX_Text(pDX, IDC_E_COLLIDE_DIST, m_fCollideDist);
	DDX_Text(pDX, IDC_E_COLLIDE_REF_PART1, m_hRefPart1);
	DDX_Text(pDX, IDC_E_COLLIDE_REF_PART2, m_hRefPart2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COddmentDesignDlg, CDialog)
	//{{AFX_MSG_MAP(COddmentDesignDlg)
	ON_BN_CLICKED(IDC_BTN_REF_PART1, OnBtnSelRefPart1)
	ON_BN_CLICKED(IDC_BTN_REF_PART2, OnBtnSelRefPart2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COddmentDesignDlg message handlers
BOOL COddmentDesignDlg::OnInitDialog() 
{
	if(m_bInernalStart&&m_hPickObj>0)
	{
		if(m_idEventProp==1)
			m_hRefPart1.Format("0X%X",m_hPickObj);
		else if(m_idEventProp==2)
			m_hRefPart2.Format("0X%X",m_hPickObj);
	}
	if(m_bDispSelectBtn)
	{
		GetDlgItem(IDC_BTN_REF_PART1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_REF_PART2)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_BTN_REF_PART1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_REF_PART2)->ShowWindow(SW_HIDE);
	}
	CDialog::OnInitDialog();
	return TRUE;
}

void COddmentDesignDlg::OnBtnSelRefPart1()
{
	m_idEventProp=1;
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_sCmdPickPrompt.Format("请选择参照杆件1:");
	CDialog::OnOK();
}

void COddmentDesignDlg::OnBtnSelRefPart2()
{
	m_idEventProp=2;
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	m_sCmdPickPrompt.Format("请选择参照杆件2:");
	CDialog::OnOK();
}
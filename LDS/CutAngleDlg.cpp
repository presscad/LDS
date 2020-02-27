// CutAngleDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "CutAngleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCutAngleDlg dialog


CCutAngleDlg::CCutAngleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCutAngleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCutAngleDlg)
	m_fLen2 = 30.0;
	m_fRadius = 30.0;
	m_iOperType = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sName = _T("Leg cut radius:");
#else 
	m_sName = _T("ÇÐ½Ç°ë¾¶:");
#endif
	//}}AFX_DATA_INIT
}


void CCutAngleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCutAngleDlg)
	DDX_Text(pDX, IDC_E_LEN2, m_fLen2);
	DDX_Text(pDX, IDC_E_RADIUS, m_fRadius);
	DDX_Radio(pDX, IDC_RAD_DAO_JIAO, m_iOperType);
	DDX_Text(pDX, IDC_S_NAME, m_sName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCutAngleDlg, CDialog)
	//{{AFX_MSG_MAP(CCutAngleDlg)
	ON_BN_CLICKED(IDC_RAD_DAO_JIAO, OnOperType)
	ON_BN_CLICKED(IDC_RAD_CUT_ANGLE, OnOperType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCutAngleDlg message handlers

void CCutAngleDlg::OnOperType() 
{
	UpdateData();
#ifdef AFX_TARG_ENU_ENGLISH
	if(m_iOperType==0)
		m_sName.Format("Charmer radius:");
	else 
		m_sName.Format("Leg cut length 1:");
#else 
	if(m_iOperType==0)
		m_sName.Format("µ¹½Ç°ë¾¶:");
	else 
		m_sName.Format("ÇÐ½Ç³¤¶È1:");
#endif
	GetDlgItem(IDC_E_LEN2)->EnableWindow(m_iOperType==1);
	UpdateData(FALSE);
}

BOOL CCutAngleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
#ifdef AFX_TARG_ENU_ENGLISH
	if(m_iOperType==0)
		m_sName.Format("Charmer radius:");
	else 
		m_sName.Format("Leg cut length 1:");
#else 
	if(m_iOperType==0)
		m_sName.Format("µ¹½Ç°ë¾¶:");
	else 
		m_sName.Format("ÇÐ½Ç³¤¶È1:");
#endif
	GetDlgItem(IDC_E_LEN2)->EnableWindow(m_iOperType==1);
	return TRUE;
}

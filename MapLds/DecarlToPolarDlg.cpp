// DecarlToPolarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DecarlToPolarDlg.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDecarlToPolarDlg dialog


CDecarlToPolarDlg::CDecarlToPolarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDecarlToPolarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDecarlToPolarDlg)
	m_fA = 0.0;
	m_fP = 0.0;
	m_fPoleX = 0.0;
	m_fPoleY = 0.0;
	m_fX = 0.0;
	m_fY = 0.0;
	//}}AFX_DATA_INIT
}


void CDecarlToPolarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecarlToPolarDlg)
	DDX_Text(pDX, IDC_E_A, m_fA);
	DDX_Text(pDX, IDC_E_P, m_fP);
	DDX_Text(pDX, IDC_E_POLE_X, m_fPoleX);
	DDX_Text(pDX, IDC_E_POLE_Y, m_fPoleY);
	DDX_Text(pDX, IDC_E_X, m_fX);
	DDX_Text(pDX, IDC_E_Y, m_fY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDecarlToPolarDlg, CDialog)
	//{{AFX_MSG_MAP(CDecarlToPolarDlg)
	ON_BN_CLICKED(IDC_BN_DECARL_TO_POLAR, OnBnDecarlToPolar)
	ON_BN_CLICKED(IDC_BN_POLAR_TO_DECARL, OnBnPolarToDecarl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecarlToPolarDlg message handlers

void CDecarlToPolarDlg::OnBnDecarlToPolar() 
{
	UpdateData();
	double angle;
	angle = Cal2dLineAng(m_fPoleX,m_fPoleY,m_fX,m_fY);
	if(fabs(1+angle)<EPS)
	{
		m_fA = 0;
		m_fP = 0;
	}
	else
	{
		m_fA = angle*DEGTORAD_COEF;
		m_fP = sqrt(pow(m_fX-m_fPoleX,2)+pow(m_fY-m_fPoleY,2));
	}
	UpdateData(FALSE);
}

void CDecarlToPolarDlg::OnBnPolarToDecarl() 
{
	UpdateData();
	m_fX = m_fPoleX+m_fP*cos(m_fA*RADTODEG_COEF);
	m_fY = m_fPoleY+m_fP*sin(m_fA*RADTODEG_COEF);
	UpdateData(FALSE);
}

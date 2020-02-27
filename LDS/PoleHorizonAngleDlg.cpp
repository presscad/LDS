// PoleHorizonAngleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "PoleHorizonAngleDlg.h"
#include "f_alg_fun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPoleHorizonAngleDlg dialog


CPoleHorizonAngleDlg::CPoleHorizonAngleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPoleHorizonAngleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPoleHorizonAngleDlg)
	m_fVecY = 0.0;
	m_fVecX = 0.0;
	m_fVecZ = 0.0;
	m_sAngle = _T("");
	//}}AFX_DATA_INIT
}


void CPoleHorizonAngleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPoleHorizonAngleDlg)
	DDX_Text(pDX, IDC_E_VEC_Y, m_fVecY);
	DDX_Text(pDX, IDC_E_VEC_X, m_fVecX);
	DDX_Text(pDX, IDC_E_VEC_Z, m_fVecZ);
	DDX_Text(pDX, IDC_S_ANGLE, m_sAngle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPoleHorizonAngleDlg, CDialog)
	//{{AFX_MSG_MAP(CPoleHorizonAngleDlg)
	ON_EN_CHANGE(IDC_E_VEC_X, OnChangeVec)
	ON_EN_CHANGE(IDC_E_VEC_Y, OnChangeVec)
	ON_EN_CHANGE(IDC_E_VEC_Z, OnChangeVec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoleHorizonAngleDlg message handlers

void CPoleHorizonAngleDlg::OnChangeVec() 
{
	UpdateData();
	f3dPoint pt(m_fVecX,m_fVecY,m_fVecZ);
	double ang = asin(fabs(pt.z/pt.mod()))*180/Pi;
	m_sAngle.Format("Ë®Æ½¼Ð½Ç:%.0f¡ã",ang);
	UpdateData(FALSE);
}

BOOL CPoleHorizonAngleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnChangeVec();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

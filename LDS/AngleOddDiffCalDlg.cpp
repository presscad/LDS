// AngleOddDiffCalDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "AngleOddDiffCalDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifdef __ANGLE_PART_INC_

/////////////////////////////////////////////////////////////////////////////
// CAngleOddDiffCalDlg dialog


CAngleOddDiffCalDlg::CAngleOddDiffCalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAngleOddDiffCalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAngleOddDiffCalDlg)
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 1.0;
	m_fWingXG = 0.0;
	m_fWingYG = 0.0;
	m_sOddX = _T("");
	m_sOddY = _T("");
	m_hAngle = _T("");
	m_sAngleSpec = _T("");
	//}}AFX_DATA_INIT
	m_pBaseJg = NULL;
}


void CAngleOddDiffCalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAngleOddDiffCalDlg)
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_WING_X_G, m_fWingXG);
	DDX_Text(pDX, IDC_E_WING_Y_G, m_fWingYG);
	DDX_Text(pDX, IDC_S_ODD_X, m_sOddX);
	DDX_Text(pDX, IDC_S_ODD_Y, m_sOddY);
	DDX_Text(pDX, IDC_S_ANGLE_HANDLE, m_hAngle);
	DDX_Text(pDX, IDC_S_ANGLE_SPEC, m_sAngleSpec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAngleOddDiffCalDlg, CDialog)
	//{{AFX_MSG_MAP(CAngleOddDiffCalDlg)
	ON_BN_CLICKED(IDC_BN_CAL_ODDMENT, OnBnCalOddment)
	ON_BN_CLICKED(IDC_BN_PASTE, OnBnPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAngleOddDiffCalDlg message handlers

void CAngleOddDiffCalDlg::OnBnCalOddment() 
{
	f3dPoint norm,start,end,vec,wing_x_vec,wing_y_vec,inters_x,inters_y;
	f3dLine g_line_x,g_line_y;
	UpdateData();
	norm.Set(m_fNormX,m_fNormY,m_fNormZ);
	start = m_pBaseJg->Start();
	end = m_pBaseJg->End();
	vec = end-start;
	normalize(vec);
	wing_x_vec = m_pBaseJg->GetWingVecX();
	wing_y_vec = m_pBaseJg->GetWingVecY();
	g_line_x.startPt = start+wing_x_vec*m_fWingXG;
	g_line_x.endPt = end+wing_x_vec*m_fWingXG;
	Int3dlf(inters_x,g_line_x,start,norm);
	g_line_y.startPt = start+wing_y_vec*m_fWingYG;
	g_line_y.endPt = end+wing_y_vec*m_fWingYG;
	Int3dlf(inters_y,g_line_y,start,norm);
	f3dPoint prj_pos_x,prj_pos_y,odd_vec_x,odd_vec_y;
	SnapPerp(&prj_pos_x,start,end,inters_x);
	SnapPerp(&prj_pos_y,start,end,inters_y);
	odd_vec_x = start-prj_pos_x;
	odd_vec_y = start-prj_pos_y;
	double odd_x = odd_vec_x*vec;
	double odd_y = odd_vec_y*vec;
	if(odd_x<0)
		m_sOddX.Format("%.1f",odd_x);
	else
		m_sOddX.Format("+%.1f",odd_x);
	if(odd_y<0)
		m_sOddY.Format("%.1f",odd_y);
	else
		m_sOddY.Format("+%.1f",odd_y);
	UpdateData(FALSE);
}

void CAngleOddDiffCalDlg::OnBnPaste() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fNormX = point.x;
		m_fNormY = point.y;
		m_fNormZ = point.z;
		UpdateData(FALSE);
	}
}

BOOL CAngleOddDiffCalDlg::OnInitDialog() 
{
	if(m_pBaseJg==NULL)
		return FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_hAngle.Format("Handle:0X%X",m_pBaseJg->handle);
	m_sAngleSpec.Format("Specification:L%.0fX%.0f",m_pBaseJg->GetWidth(),m_pBaseJg->GetThick());
#else 
	m_hAngle.Format("¾ä±ú:0X%X",m_pBaseJg->handle);
	m_sAngleSpec.Format("¹æ¸ñ:L%.0fX%.0f",m_pBaseJg->GetWidth(),m_pBaseJg->GetThick());
#endif
	CDialog::OnInitDialog();
	
	OnBnCalOddment();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
#endif
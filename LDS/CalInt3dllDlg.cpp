// CalInt3dllDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "CalInt3dllDlg.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalInt3dllDlg dialog


CCalInt3dllDlg::CCalInt3dllDlg(CWnd* pWnd)
{
	//{{AFX_DATA_INIT(CCalInt3dllDlg)
	m_fEndX1 = 0.0;
	m_fEndX2 = 0.0;
	m_fEndY1 = 0.0;
	m_fEndY2 = 0.0;
	m_fEndZ1 = 0.0;
	m_fEndZ2 = 0.0;
	m_fIntersX = 0.0;
	m_fIntersY = 0.0;
	m_fIntersZ = 0.0;
	m_fStartX1 = 0.0;
	m_fStartX2 = 0.0;
	m_fStartY1 = 0.0;
	m_fStartY2 = 0.0;
	m_fStartZ1 = 0.0;
	m_fStartZ2 = 0.0;
	m_sResultReport = _T("");
	//}}AFX_DATA_INIT
	m_pWnd = pWnd;
}


void CCalInt3dllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalInt3dllDlg)
	DDX_Text(pDX, IDC_E_END_X, m_fEndX1);
	DDX_Text(pDX, IDC_E_END_X2, m_fEndX2);
	DDX_Text(pDX, IDC_E_END_Y, m_fEndY1);
	DDX_Text(pDX, IDC_E_END_Y2, m_fEndY2);
	DDX_Text(pDX, IDC_E_END_Z, m_fEndZ1);
	DDX_Text(pDX, IDC_E_END_Z2, m_fEndZ2);
	DDX_Text(pDX, IDC_E_INTERS_X, m_fIntersX);
	DDX_Text(pDX, IDC_E_INTERS_Y, m_fIntersY);
	DDX_Text(pDX, IDC_E_INTERS_Z, m_fIntersZ);
	DDX_Text(pDX, IDC_E_START_X, m_fStartX1);
	DDX_Text(pDX, IDC_E_START_X2, m_fStartX2);
	DDX_Text(pDX, IDC_E_START_Y, m_fStartY1);
	DDX_Text(pDX, IDC_E_START_Y2, m_fStartY2);
	DDX_Text(pDX, IDC_E_START_Z, m_fStartZ1);
	DDX_Text(pDX, IDC_E_START_Z2, m_fStartZ2);
	DDX_Text(pDX, ID_RESULT_REPORT, m_sResultReport);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalInt3dllDlg, CDialog)
	//{{AFX_MSG_MAP(CCalInt3dllDlg)
	ON_BN_CLICKED(ID_CAL_INTERS, OnCalInters)
	ON_BN_CLICKED(IDC_BN_PASTE_START1, OnBnPasteStart1)
	ON_BN_CLICKED(IDC_BN_PASTE_START2, OnBnPasteStart2)
	ON_BN_CLICKED(IDC_BN_PASTE_END1, OnBnPasteEnd1)
	ON_BN_CLICKED(IDC_BN_PASTE_END2, OnBnPasteEnd2)
	ON_BN_CLICKED(ID_COPY_INTERS_PT, OnCopyIntersPt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalInt3dllDlg message handlers

void CCalInt3dllDlg::OnCalInters() 
{
	UpdateData();
	f3dPoint inters;
	f3dLine line1,line2;
	line1.startPt.Set(m_fStartX1,m_fStartY1,m_fStartZ1);
	line1.endPt.Set(m_fEndX1,m_fEndY1,m_fEndZ1);
	line2.startPt.Set(m_fStartX2,m_fStartY2,m_fStartZ2);
	line2.endPt.Set(m_fEndX2,m_fEndY2,m_fEndZ2);
	int nRetCode = Int3dll(line1,line2,inters);
	m_fIntersX = inters.x;
	m_fIntersY = inters.y;
	m_fIntersZ = inters.z;
	switch(nRetCode)
	{
#ifdef AFX_TARG_ENU_ENGLISH
	case -2:
		m_sResultReport = "The intersectant point is invalid（The point on the outside of line）";
		break;
	case -1:
		m_sResultReport = "Line endpoint overlap, the definition is wrong";
		break;
	case 0:
		m_sResultReport = "L1 and  L2 is parallel,overlapping or different surface";
		break;
	case 1:
		m_sResultReport = "The intersectant point is interior point of  two line";
		break;
	case 2:
		m_sResultReport = "The intersectant point is line endpoint";
		break;
	default:
		m_sResultReport = "Calculate error";
#else 
	case -2:
		m_sResultReport = "交点无效（直线外点）";
		break;
	case -1:
		m_sResultReport = "直线端点重合，定义有误";
		break;
	case 0:
		m_sResultReport = "L1与L2平行、重合或异面";
		break;
	case 1:
		m_sResultReport = "交点为两直线段的内点";
		break;
	case 2:
		m_sResultReport = "交点为直线段端点";
		break;
	default:
		m_sResultReport = "计算错误";
#endif
	}
	UpdateData(FALSE);
}
BOOL CCalInt3dllDlg::Create()
{
	return CDialog::Create(CCalInt3dllDlg::IDD);
}
void CCalInt3dllDlg::OnOK() 
{
	DestroyWindow();
}

void CCalInt3dllDlg::OnCancel() 
{
	DestroyWindow();
}

void CCalInt3dllDlg::OnBnPasteStart1() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fStartX1 = point.x;
	m_fStartY1 = point.y;
	m_fStartZ1 = point.z;
	UpdateData(FALSE);
}

void CCalInt3dllDlg::OnBnPasteStart2() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fStartX2 = point.x;
	m_fStartY2 = point.y;
	m_fStartZ2 = point.z;
	UpdateData(FALSE);
}

void CCalInt3dllDlg::OnBnPasteEnd1() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fEndX1 = point.x;
	m_fEndY1 = point.y;
	m_fEndZ1 = point.z;
	UpdateData(FALSE);
}

void CCalInt3dllDlg::OnBnPasteEnd2() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fEndX2 = point.x;
	m_fEndY2 = point.y;
	m_fEndZ2 = point.z;
	UpdateData(FALSE);
}

void CCalInt3dllDlg::OnCopyIntersPt() 
{
	f3dPoint point;
	UpdateData(TRUE);
	point.x = m_fIntersX;
	point.y = m_fIntersY;
	point.z = m_fIntersZ;
	WritePointToClip(point);
}

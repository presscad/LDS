// 3dLFDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "3dLFDlg.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3dLFDlg dialog


C3dLFDlg::C3dLFDlg(CWnd* pWnd /*=NULL*/)
	//: CDialog(C3dLFDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(C3dLFDlg)
	m_fBaseX = 0.0;
	m_fBaseY = 0.0;
	m_fBaseZ = 0.0;
	m_fEndX = 0.0;
	m_fEndY = 0.0;
	m_fEndZ = 0.0;
	m_fIntersX = 0.0;
	m_fIntersY = 0.0;
	m_fIntersZ = 0.0;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_fStartX = 0.0;
	m_fStartY = 0.0;
	m_fStartZ = 0.0;
	m_sResultReport = _T("");
	//}}AFX_DATA_INIT
	m_pWnd = pWnd;
}


void C3dLFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C3dLFDlg)
	DDX_Text(pDX, IDC_E_BASE_X, m_fBaseX);
	DDX_Text(pDX, IDC_E_BASE_Y, m_fBaseY);
	DDX_Text(pDX, IDC_E_BASE_Z, m_fBaseZ);
	DDX_Text(pDX, IDC_E_END_X, m_fEndX);
	DDX_Text(pDX, IDC_E_END_Y, m_fEndY);
	DDX_Text(pDX, IDC_E_END_Z, m_fEndZ);
	DDX_Text(pDX, IDC_E_INTERS_X, m_fIntersX);
	DDX_Text(pDX, IDC_E_INTERS_Y, m_fIntersY);
	DDX_Text(pDX, IDC_E_INTERS_Z, m_fIntersZ);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_START_X, m_fStartX);
	DDX_Text(pDX, IDC_E_START_Y, m_fStartY);
	DDX_Text(pDX, IDC_E_START_Z, m_fStartZ);
	DDX_Text(pDX, ID_RESULT_REPORT, m_sResultReport);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(C3dLFDlg, CDialog)
	//{{AFX_MSG_MAP(C3dLFDlg)
	ON_BN_CLICKED(ID_BN_CAL_INTERS, OnBnCalInters)
	ON_BN_CLICKED(ID_BN_COPY_INTERS_PT, OnBnCopyIntersPt)
	ON_BN_CLICKED(IDC_BN_PASTE_BASE, OnBnPasteBase)
	ON_BN_CLICKED(IDC_BN_PASTE_END, OnBnPasteEnd)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnBnPasteNorm)
	ON_BN_CLICKED(IDC_BN_PASTE_START, OnBnPasteStart)
	ON_BN_CLICKED(IDC_BN_PASTE_PLANE, OnBnPastePlane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3dLFDlg message handlers

void C3dLFDlg::OnBnCopyIntersPt() 
{
	f3dPoint point;
	UpdateData(TRUE);
	point.x = m_fIntersX;
	point.y = m_fIntersY;
	point.z = m_fIntersZ;
	WritePointToClip(point);
}

void C3dLFDlg::OnBnPasteBase() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fBaseX = point.x;
	m_fBaseY = point.y;
	m_fBaseZ = point.z;
	UpdateData(FALSE);
}

void C3dLFDlg::OnBnPasteEnd() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fEndX = point.x;
	m_fEndY = point.y;
	m_fEndZ = point.z;
	UpdateData(FALSE);
}

void C3dLFDlg::OnBnPasteNorm() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fNormX = point.x;
	m_fNormY = point.y;
	m_fNormZ = point.z;
	UpdateData(FALSE);
}

void C3dLFDlg::OnBnPasteStart() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fStartX = point.x;
	m_fStartY = point.y;
	m_fStartZ = point.z;
	UpdateData(FALSE);
}

void C3dLFDlg::OnBnCalInters() 
{
	UpdateData();
	f3dLine line;
	f3dPoint inters,face_norm,face_base;
	face_norm.Set(m_fNormX,m_fNormY,m_fNormZ);
	face_base.Set(m_fBaseX,m_fBaseY,m_fBaseZ);
	line.startPt.Set(m_fStartX,m_fStartY,m_fStartZ);
	line.endPt.Set(m_fEndX,m_fEndY,m_fEndZ);
	int nRetCode = Int3dlf(inters,line,face_base,face_norm);
	m_fIntersX = inters.x;
	m_fIntersY = inters.y;
	m_fIntersZ = inters.z;
	switch(nRetCode)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		case -1:
			m_sResultReport = "Line or plane's definition is wrong";
			break;
		case 0:
			m_sResultReport = "The line parallel to the plane";
			break;
		case 1:
			m_sResultReport = "Get effective intersection";
			break;
		default:
			m_sResultReport = "Calculate mistakes";
#else
		case -1:
			m_sResultReport = "直线或平面定义有误";
			break;
		case 0:
			m_sResultReport = "直线与平面平行，无交点";
			break;
		case 1:
			m_sResultReport = "取得有效交点";
			break;
		default:
			m_sResultReport = "计算错误";
#endif
	}
	UpdateData(FALSE);
}

BOOL C3dLFDlg::Create()
{
	return CDialog::Create(C3dLFDlg::IDD);
}

void C3dLFDlg::OnOK() 
{
	DestroyWindow();
}

void C3dLFDlg::OnCancel() 
{
	DestroyWindow();
}

void C3dLFDlg::OnBnPastePlane() 
{
	f3dPoint face_base,face_norm;
	if(ReadClipPlane(face_base,face_norm))
	{
		m_fNormX=face_norm.x;
		m_fNormY=face_norm.y;
		m_fNormZ=face_norm.z;
		m_fBaseX=face_base.x;
		m_fBaseY=face_base.y;
		m_fBaseZ=face_base.z;
		UpdateData(FALSE);
	}
	
}

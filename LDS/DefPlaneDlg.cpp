//<LOCALE_TRANSLATE Confirm by hxr/>
// DefPlaneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DefPlaneDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefPlaneDlg dialog


CDefPlaneDlg::CDefPlaneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefPlaneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefPlaneDlg)
	m_fBaseX = 0.0;
	m_fBaseY = 0.0;
	m_fBaseZ = 0.0;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = -1.0;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sDlgCaption= _T("define folded plate bottom plane");
#else
	m_sDlgCaption= _T("定义折叠板底平面");
#endif
	m_bReadOnly=FALSE;
}


void CDefPlaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefPlaneDlg)
	DDX_Text(pDX, IDC_E_BASE_X, m_fBaseX);
	DDX_Text(pDX, IDC_E_BASE_Y, m_fBaseY);
	DDX_Text(pDX, IDC_E_BASE_Z, m_fBaseZ);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefPlaneDlg, CDialog)
	//{{AFX_MSG_MAP(CDefPlaneDlg)
	ON_BN_CLICKED(IDC_BN_COPY_BASE, OnBnCopyBase)
	ON_BN_CLICKED(IDC_BN_COPY_NORM, OnBnCopyNorm)
	ON_BN_CLICKED(IDC_BN_PASTE_BASE, OnBnPasteBase)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnBnPasteNorm)
	ON_BN_CLICKED(IDC_BNT_COPY_PLANE, OnCopyPlane)
	ON_BN_CLICKED(IDC_BNT_PASTE_PLANE, OnPastePlane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefPlaneDlg message handlers

BOOL CDefPlaneDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_sDlgCaption);
	if(m_bReadOnly)
	{	//仅仅用来查看或拷贝平面
		GetDlgItem(IDC_BN_PASTE_BASE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BN_PASTE_NORM)->EnableWindow(FALSE);
		GetDlgItem(IDC_BNT_PASTE_PLANE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_BASE_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_Z)->EnableWindow(FALSE);
	}
	return TRUE;
}

void CDefPlaneDlg::OnBnCopyBase() 
{
	f3dPoint point;
	UpdateData(TRUE);
	point.x = m_fBaseX;
	point.y = m_fBaseY;
	point.z = m_fBaseZ;
	WritePointToClip(point);
}

void CDefPlaneDlg::OnBnCopyNorm() 
{
	f3dPoint point;
	UpdateData(TRUE);
	point.x = m_fNormX;
	point.y = m_fNormY;
	point.z = m_fNormZ;
	WritePointToClip(point);
}

void CDefPlaneDlg::OnBnPasteBase() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fBaseX = point.x;
	m_fBaseY = point.y;
	m_fBaseZ = point.z;
	UpdateData(FALSE);
}

void CDefPlaneDlg::OnBnPasteNorm() 
{
	f3dPoint point;
	UpdateData(TRUE);
	ReadClipPoint(point);
	m_fNormX = point.x;
	m_fNormY = point.y;
	m_fNormZ = point.z;
	UpdateData(FALSE);
}

void CDefPlaneDlg::OnCopyPlane() 
{
	f3dPoint face_base,face_norm;
	UpdateData(TRUE);
	face_base.Set(m_fBaseX,m_fBaseY,m_fBaseZ);
	face_norm.Set(m_fNormX,m_fNormY,m_fNormZ);
	WritePlaneToClip(face_base,face_norm);
}

void CDefPlaneDlg::OnPastePlane() 
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

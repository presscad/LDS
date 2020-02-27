// InputDrawingUcsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GlobalFunc.h"
#include "InputDrawingUcsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDrawingUcsDlg dialog


CInputDrawingUcsDlg::CInputDrawingUcsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDrawingUcsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputDrawingUcsDlg)
	m_fAxisX_X = 1.0;
	m_fAxisX_Y = 0.0;
	m_fAxisX_Z = 0.0;
	m_fAxisZ_X = 0.0;
	m_fAxisZ_Y = 0.0;
	m_fAxisZ_Z = 1.0;
	m_fOriginPosX = 0.0;
	m_fOriginPosY = 0.0;
	m_fOriginPosZ = 0.0;
	//}}AFX_DATA_INIT
}

void CInputDrawingUcsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputDrawingUcsDlg)
	DDX_Text(pDX, IDC_E_AXIS_X_X, m_fAxisX_X);
	DDX_Text(pDX, IDC_E_AXIS_X_Y, m_fAxisX_Y);
	DDX_Text(pDX, IDC_E_AXIS_X_Z, m_fAxisX_Z);
	DDX_Text(pDX, IDC_E_AXIS_Z_X, m_fAxisZ_X);
	DDX_Text(pDX, IDC_E_AXIS_Z_Y, m_fAxisZ_Y);
	DDX_Text(pDX, IDC_E_AXIS_Z_Z, m_fAxisZ_Z);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_X, m_fOriginPosX);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Y, m_fOriginPosY);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Z, m_fOriginPosZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputDrawingUcsDlg, CDialog)
	//{{AFX_MSG_MAP(CInputDrawingUcsDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_X, OnBnPasteAxisX)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Z, OnBnPasteAxisZ)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN_POS, OnBnPasteOriginPos)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_X, OnBnCopyAxisX)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Z, OnBnCopyAxisZ)
	ON_BN_CLICKED(IDC_BN_COPY_ORIGIN_POS, OnBnCopyOriginPos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDrawingUcsDlg message handlers
void CInputDrawingUcsDlg::OnBnPasteAxisX() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisX_X = point.x;
		m_fAxisX_Y = point.y;
		m_fAxisX_Z = point.z;
		UpdateData(FALSE);
	}
}

void CInputDrawingUcsDlg::OnBnPasteAxisZ() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisZ_X = point.x;
		m_fAxisZ_Y = point.y;
		m_fAxisZ_Z = point.z;
		UpdateData(FALSE);
	}
}

void CInputDrawingUcsDlg::OnBnPasteOriginPos() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fOriginPosX = point.x;
		m_fOriginPosY = point.y;
		m_fOriginPosZ = point.z;
		UpdateData(FALSE);
	}
}

void CInputDrawingUcsDlg::OnBnCopyAxisX() 
{
	f3dPoint point;
	point.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	WritePointToClip(point);
}

void CInputDrawingUcsDlg::OnBnCopyAxisZ() 
{
	f3dPoint point;
	point.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WritePointToClip(point);
}

void CInputDrawingUcsDlg::OnBnCopyOriginPos() 
{
	f3dPoint point;
	point.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	WritePointToClip(point);
}

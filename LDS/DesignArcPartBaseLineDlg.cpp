// DesignArcPartBaseLineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "DesignArcPartBaseLineDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignArcPartBaseLineDlg dialog


CDesignArcPartBaseLineDlg::CDesignArcPartBaseLineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignArcPartBaseLineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignArcPartBaseLineDlg)
	m_fWorkNormX = 0.0;
	m_fWorkNormY = 0.0;
	m_fWorkNormZ = -1.0;
	m_fCenterX = 0.0;
	m_fCenterY = 0.0;
	m_fCenterZ = 0.0;
	m_fStartPosX = 1000.0;
	m_fStartPosY = 0.0;
	m_fStartPosZ = 0.0;
	m_fStartAngle = 0;
	m_fArcAngle = 90;
	//}}AFX_DATA_INIT
}


void CDesignArcPartBaseLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignArcPartBaseLineDlg)
	DDX_Text(pDX, IDC_E_WORK_NORM_X, m_fWorkNormX);
	DDX_Text(pDX, IDC_E_WORK_NORM_Y, m_fWorkNormY);
	DDX_Text(pDX, IDC_E_WORK_NORM_Z, m_fWorkNormZ);
	DDX_Text(pDX, IDC_E_CENTER_POS_X, m_fCenterX);
	DDX_Text(pDX, IDC_E_CENTER_POS_Y, m_fCenterY);
	DDX_Text(pDX, IDC_E_CENTER_POS_Z, m_fCenterZ);
	DDX_Text(pDX, IDC_E_START_POS_X, m_fStartPosX);
	DDX_Text(pDX, IDC_E_START_POS_Y, m_fStartPosY);
	DDX_Text(pDX, IDC_E_START_POS_Z, m_fStartPosZ);
	DDX_Text(pDX, IDC_E_START_ANGLE, m_fStartAngle);
	DDX_Text(pDX, IDC_E_ARC_ANGLE, m_fArcAngle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignArcPartBaseLineDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignArcPartBaseLineDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_WORK_NORM, OnBnPasteWorkNorm)
	ON_BN_CLICKED(IDC_BN_PASTE_CENTER_POS, OnBnPasteCenter)
	ON_BN_CLICKED(IDC_BN_PASTE_START_POS, OnBnPasteStartPos)
	ON_BN_CLICKED(IDC_BN_COPY_WORK_NORM, OnBnCopyWorkNorm)
	ON_BN_CLICKED(IDC_BN_COPY_CENTER_POS, OnBnCopyCenter)
	ON_BN_CLICKED(IDC_BN_COPY_START_POS, OnBnCopyStartPos)
	ON_EN_CHANGE(IDC_E_CENTER_POS_Z, OnChangeCenterPosZ)
	ON_EN_CHANGE(IDC_E_START_POS_Z, OnChangeStartPosZ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignArcPartBaseLineDlg message handlers

void CDesignArcPartBaseLineDlg::OnBnPasteWorkNorm() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fWorkNormX = point.x;
		m_fWorkNormY = point.y;
		m_fWorkNormZ = point.z;
		UpdateData(FALSE);
	}
}

void CDesignArcPartBaseLineDlg::OnBnPasteCenter() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fCenterX = point.x;
		m_fCenterY = point.y;
		m_fCenterZ = point.z;
		UpdateData(FALSE);
	}
}

void CDesignArcPartBaseLineDlg::OnBnPasteStartPos() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fStartPosX = point.x;
		m_fStartPosY = point.y;
		m_fStartPosZ = point.z;
		UpdateData(FALSE);
	}
}

void CDesignArcPartBaseLineDlg::OnBnCopyWorkNorm() 
{
	f3dPoint point;
	point.Set(m_fWorkNormX,m_fWorkNormY,m_fWorkNormZ);
	WritePointToClip(point);
}

void CDesignArcPartBaseLineDlg::OnBnCopyCenter() 
{
	f3dPoint point;
	point.Set(m_fCenterX,m_fCenterY,m_fCenterZ);
	WritePointToClip(point);
}

void CDesignArcPartBaseLineDlg::OnBnCopyStartPos() 
{
	f3dPoint point;
	point.Set(m_fStartPosX,m_fStartPosY,m_fStartPosZ);
	WritePointToClip(point);
}

void CDesignArcPartBaseLineDlg::OnChangeCenterPosZ() 
{
	UpdateData();
	m_fStartPosZ=m_fCenterZ;
	UpdateData(FALSE);
}

void CDesignArcPartBaseLineDlg::OnChangeStartPosZ() 
{
	UpdateData();
	m_fCenterZ=m_fStartPosZ;
	UpdateData(FALSE);
}

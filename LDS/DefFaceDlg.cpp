// DefFaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DefFaceDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefFaceDlg dialog


CDefFaceDlg::CDefFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefFaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefFaceDlg)
	m_fBasePosX = 0.0;
	m_fBasePosY = 0.0;
	m_fBasePosZ = 0.0;
	m_fFaceNormX = 0.0;
	m_fFaceNormY = 0.0;
	m_fFaceNormZ = 0.0;
	m_fBaseNormOffsetDist = 0.0;
	m_bSaveAnotherPlate = FALSE;
	//}}AFX_DATA_INIT
	m_bCutPlate=FALSE;
}


void CDefFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefFaceDlg)
	DDX_Text(pDX, IDC_E_BASE_POS_X, m_fBasePosX);
	DDX_Text(pDX, IDC_E_BASE_POS_Y, m_fBasePosY);
	DDX_Text(pDX, IDC_E_BASE_POS_Z, m_fBasePosZ);
	DDX_Text(pDX, IDC_E_FACE_NORM_X, m_fFaceNormX);
	DDX_Text(pDX, IDC_E_FACE_NORM_Y, m_fFaceNormY);
	DDX_Text(pDX, IDC_E_FACE_NORM_Z, m_fFaceNormZ);
	DDX_Text(pDX, IDC_E_BASE_NORM_OFFSET_DIST, m_fBaseNormOffsetDist);
	DDX_Check(pDX, IDC_CHK_SAVE_ANOTHER_PLATE, m_bSaveAnotherPlate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefFaceDlg, CDialog)
	//{{AFX_MSG_MAP(CDefFaceDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_BASE_POINT, OnBnPasteBasePoint)
	ON_BN_CLICKED(IDC_BN_PASTE_FACE_NORM, OnBnPasteFaceNorm)
	ON_BN_CLICKED(ID_BN_PASTE_PLANE, OnBnPastePlane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefFaceDlg message handlers

void CDefFaceDlg::OnBnPasteBasePoint() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fBasePosX = point.x;
		m_fBasePosY = point.y;
		m_fBasePosZ = point.z;
		UpdateData(FALSE);
	}
}

void CDefFaceDlg::OnBnPasteFaceNorm() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fFaceNormX = point.x;
		m_fFaceNormY = point.y;
		m_fFaceNormZ = point.z;
		UpdateData(FALSE);
	}
}

void CDefFaceDlg::OnBnPastePlane() 
{
	f3dPoint face_base,face_norm;
	if(ReadClipPlane(face_base,face_norm))
	{
		m_fFaceNormX=face_norm.x;
		m_fFaceNormY=face_norm.y;
		m_fFaceNormZ=face_norm.z;
		m_fBasePosX=face_base.x;
		m_fBasePosY=face_base.y;
		m_fBasePosZ=face_base.z;
		UpdateData(FALSE);
	}
}

BOOL CDefFaceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_bCutPlate)
		GetDlgItem(IDC_CHK_SAVE_ANOTHER_PLATE)->ShowWindow(SW_SHOW);
	else 
		GetDlgItem(IDC_CHK_SAVE_ANOTHER_PLATE)->ShowWindow(SW_HIDE);
	return TRUE;
}

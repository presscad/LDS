// DimSimPolyCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DimSimPolyCfgDlg.h"
#include "DrawUcsDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDimSimPolyCfgDlg dialog

CDimSimPolyCfgDlg::CDimSimPolyCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDimSimPolyCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDimSimPolyCfgDlg)
	m_bDimHuoQuFace = FALSE;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	//}}AFX_DATA_INIT
}


void CDimSimPolyCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDimSimPolyCfgDlg)
	DDX_Check(pDX, IDC_CHK_DIM_HUOQU_FACE, m_bDimHuoQuFace);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDimSimPolyCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CDimSimPolyCfgDlg)
	ON_BN_CLICKED(IDC_BN_MAP_DRAW_UCS, OnBnMapDrawUcs)
	ON_BN_CLICKED(IDC_BN_PASTE_WORK_NORM, OnBnPasteWorkNorm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDimSimPolyCfgDlg message handlers

void CDimSimPolyCfgDlg::OnBnMapDrawUcs() 
{
	CDrawUcsDlg dlg;
	dlg.ucs = base_face_ucs;
	if(dlg.DoModal()==IDOK)
		base_face_ucs=dlg.ucs;
}

void CDimSimPolyCfgDlg::OnBnPasteWorkNorm() 
{
	f3dPoint point;
	ReadClipPoint(point);
	m_fNormX = point.x;
	m_fNormY = point.y;
	m_fNormZ = point.z;
	UpdateData(FALSE);
}

// DrawingVectorDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "maptma.h"
#include "GlobalFunc.h"
#include "DrawingVectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawingVectorDlg dialog


CDrawingVectorDlg::CDrawingVectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawingVectorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawingVectorDlg)
	m_fAxisXX = 0.0;
	m_fAxisXY = 0.0;
	m_fAxisXZ = 0.0;
	m_fAxisZX = 0.0;
	m_fAxisZY = 0.0;
	m_fAxisZZ = 0.0;
	//}}AFX_DATA_INIT
}


void CDrawingVectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawingVectorDlg)
	DDX_Text(pDX, IDC_E_AXIS_X_X, m_fAxisXX);
	DDX_Text(pDX, IDC_E_AXIS_X_Y, m_fAxisXY);
	DDX_Text(pDX, IDC_E_AXIS_X_Z, m_fAxisXZ);
	DDX_Text(pDX, IDC_E_AXIS_Z_X, m_fAxisZX);
	DDX_Text(pDX, IDC_E_AXIS_Z_Y, m_fAxisZY);
	DDX_Text(pDX, IDC_E_AXIS_Z_Z, m_fAxisZZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrawingVectorDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawingVectorDlg)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_X, OnBnCopyAxisX)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_X, OnBnPasteAxisX)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Z, OnBnPasteAxisZ)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Z, OnBnCopyAxisZ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawingVectorDlg message handlers

void CDrawingVectorDlg::OnBnCopyAxisX() 
{
	f3dPoint point;
	point.Set(m_fAxisXX,m_fAxisXY,m_fAxisXZ);
	WritePointToClip(point);
}

void CDrawingVectorDlg::OnBnCopyAxisZ() 
{
	f3dPoint point;
	point.Set(m_fAxisZX,m_fAxisZY,m_fAxisZZ);
	WritePointToClip(point);
}

void CDrawingVectorDlg::OnBnPasteAxisX() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisXX = point.x;
		m_fAxisXY = point.y;
		m_fAxisXZ = point.z;
		axis_x.Set(m_fAxisXX,m_fAxisXY,m_fAxisXZ);
		UpdateData(FALSE);
	}
}

void CDrawingVectorDlg::OnBnPasteAxisZ() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisZX = point.x;
		m_fAxisZY = point.y;
		m_fAxisZZ = point.z;
		axis_z.Set(m_fAxisZX,m_fAxisZY,m_fAxisZZ);
		UpdateData(FALSE);
	}
}

void CDrawingVectorDlg::OnOK() 
{
	UpdateData();
	axis_x.Set(m_fAxisXX,m_fAxisXY,m_fAxisXZ);
	axis_z.Set(m_fAxisZX,m_fAxisZY,m_fAxisZZ);
	CDialog::OnOK();
}

BOOL CDrawingVectorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_fAxisXX=axis_x.x;
	m_fAxisXY=axis_x.y;
	m_fAxisXZ=axis_x.z;
	m_fAxisZX=axis_z.x;
	m_fAxisZY=axis_z.y;
	m_fAxisZZ=axis_z.z;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

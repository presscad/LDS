// FaceOffsetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "FaceOffsetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFaceOffsetDlg dialog


CFaceOffsetDlg::CFaceOffsetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFaceOffsetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFaceOffsetDlg)
	m_fFaceOffsetNormX = 0.0;
	m_fFaceOffsetNormY = 0.0;
	m_iFaceOffsetDatumLine = 0;
	m_iFaceOffsetDatumPosStyle = 0;
	m_fFaceOffsetNormZ = 0.0;
	m_iType = -1;
	//}}AFX_DATA_INIT
}


void CFaceOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFaceOffsetDlg)
	DDX_Text(pDX, IDC_E_FACE_OFFSET_NORM_X, m_fFaceOffsetNormX);
	DDX_Text(pDX, IDC_E_FACE_OFFSET_NORM_Y, m_fFaceOffsetNormY);
	DDX_Radio(pDX, IDC_RDO_FACE_OFFSET_DATUM_LINE, m_iFaceOffsetDatumLine);
	DDX_Radio(pDX, IDC_RDO_FACE_OFFSET_DATUM_POS_STYLE, m_iFaceOffsetDatumPosStyle);
	DDX_Text(pDX, IDC_E_FACE_OFFSET_NORM_Z, m_fFaceOffsetNormZ);
	DDX_CBIndex(pDX, IDC_CMB_TYPE, m_iType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFaceOffsetDlg, CDialog)
	//{{AFX_MSG_MAP(CFaceOffsetDlg)
	ON_CBN_SELCHANGE(IDC_CMB_TYPE, OnSelchangeCmbType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaceOffsetDlg message handlers

BOOL CFaceOffsetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_E_FACE_OFFSET_NORM_X)->EnableWindow(m_iType==3);
	GetDlgItem(IDC_E_FACE_OFFSET_NORM_Y)->EnableWindow(m_iType==3);
	GetDlgItem(IDC_E_FACE_OFFSET_NORM_Z)->EnableWindow(m_iType==3);
	return TRUE;  
}

void CFaceOffsetDlg::OnSelchangeCmbType() 
{
	UpdateData();
	GetDlgItem(IDC_E_FACE_OFFSET_NORM_X)->EnableWindow(m_iType==3);
	GetDlgItem(IDC_E_FACE_OFFSET_NORM_Y)->EnableWindow(m_iType==3);
	GetDlgItem(IDC_E_FACE_OFFSET_NORM_Z)->EnableWindow(m_iType==3);
	if(m_iType==0)
	{
		m_fFaceOffsetNormX=1;
		m_fFaceOffsetNormY=0;
		m_fFaceOffsetNormZ=0;
	}
	else if(m_iType==1)
	{
		m_fFaceOffsetNormX=0;
		m_fFaceOffsetNormY=1;
		m_fFaceOffsetNormZ=0;
	}
	else if(m_iType==2)
	{
		m_fFaceOffsetNormX=0;
		m_fFaceOffsetNormY=0;
		m_fFaceOffsetNormZ=1;
	}
	UpdateData(FALSE);
}

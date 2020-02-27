// NewConeTubeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "NewConeTubeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewConeTubeDlg dialog


CNewConeTubeDlg::CNewConeTubeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewConeTubeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewConeTubeDlg)
	m_sType = _T("");
	m_fDownRadius = 0.0;
	m_fUpRadius = 0.0;
	m_nFaceNum = 0;
	m_fFrontOpen = 0.0;
	m_fLongth = 0.0;
	m_fThick = 0.0;
	m_fX = 0.0;
	m_fY = 0.0;
	m_fZ = 0.0;
	m_sMaterial = _T("");
	m_fSideOpen = 0.0;
	//}}AFX_DATA_INIT
}


void CNewConeTubeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewConeTubeDlg)
	DDX_Control(pDX, IDC_SIDE_OPEN, m_editSideOpen);
	DDX_Control(pDX, IDC_FRONT_OPEN, m_editFrontOpen);
	DDX_Control(pDX, IDC_MATERIAL, m_cmbMaterial);
	DDX_Control(pDX, IDC_CONETUBE_TYPE, m_cmbType);
	DDX_CBString(pDX, IDC_CONETUBE_TYPE, m_sType);
	DDX_Text(pDX, IDC_DOWN_RADIUS, m_fDownRadius);
	DDX_Text(pDX, IDC_UP_RADIUS, m_fUpRadius);
	DDX_Text(pDX, IDC_FACE_NUM, m_nFaceNum);
	DDX_Text(pDX, IDC_FRONT_OPEN, m_fFrontOpen);
	DDX_Text(pDX, IDC_LONGTH, m_fLongth);
	DDX_Text(pDX, IDC_THICK, m_fThick);
	DDX_Text(pDX, IDC_X, m_fX);
	DDX_Text(pDX, IDC_Y, m_fY);
	DDX_Text(pDX, IDC_Z, m_fZ);
	DDX_CBString(pDX, IDC_MATERIAL, m_sMaterial);
	DDX_Text(pDX, IDC_SIDE_OPEN, m_fSideOpen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewConeTubeDlg, CDialog)
	//{{AFX_MSG_MAP(CNewConeTubeDlg)
	ON_CBN_SELCHANGE(IDC_CONETUBE_TYPE, OnSelchangeConetubeType)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewConeTubeDlg message handlers

void CNewConeTubeDlg::OnSelchangeConetubeType() 
{
	UpdateData();
	char sText[255];
	m_cmbType.GetLBText(m_cmbType.GetCurSel(),sText);
	if (strcmp(sText,"Ë«¸Ö¹Ü¸Ë") == 0)
	{
		m_editSideOpen.SetWindowText("0");
		m_editSideOpen.SetReadOnly(TRUE);
		m_editFrontOpen.SetReadOnly(FALSE);
	}
	else if(strcmp(sText,"ËÄ¸Ö¹Ü¸Ë") == 0)
	{
		m_editSideOpen.SetReadOnly(FALSE);
		m_editFrontOpen.SetReadOnly(FALSE);
	}
	else
	{
		m_editSideOpen.SetWindowText("0");
		m_editFrontOpen.SetWindowText("0");
		m_editFrontOpen.SetReadOnly(TRUE);
		m_editSideOpen.SetReadOnly(TRUE);
	}
}

BOOL CNewConeTubeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_editFrontOpen.SetReadOnly(TRUE);
	m_editSideOpen.SetReadOnly(TRUE);
	m_cmbType.SetCurSel(0);
	m_cmbMaterial.SetCurSel(0);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewConeTubeDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
}

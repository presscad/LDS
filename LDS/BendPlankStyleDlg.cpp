// BendPlankStyleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "BendPlankStyleDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBendPlankStyleDlg dialog


CBendPlankStyleDlg::CBendPlankStyleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBendPlankStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBendPlankStyleDlg)
	m_fBendAngle = 0.0;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_iBendFaceStyle = 0;
	//}}AFX_DATA_INIT
}


void CBendPlankStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBendPlankStyleDlg)
	DDX_Text(pDX, IDC_E_BEND_ANGLE, m_fBendAngle);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Radio(pDX, IDC_RDO_BEND_FACE_STYLE, m_iBendFaceStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBendPlankStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CBendPlankStyleDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnBnPasteNorm)
	ON_BN_CLICKED(IDC_RDO_BEND_FACE_STYLE, OnRdoBendFaceStyle)
	ON_BN_CLICKED(IDC_RADIO13, OnRdoBendFaceStyle)
	ON_BN_CLICKED(IDC_RADIO14, OnRdoBendFaceStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBendPlankStyleDlg message handlers

void CBendPlankStyleDlg::OnBnPasteNorm() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fNormX = point.x;
		m_fNormY = point.y;
		m_fNormZ = point.z;
		UpdateData(FALSE);
	}
}

BOOL CBendPlankStyleDlg::OnInitDialog() 
{
	if(m_iBendFaceStyle==0)	//�����Ƕ�ȷ��
	{
		GetDlgItem(IDC_E_BEND_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_NORM_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_Z)->EnableWindow(FALSE);
	}
	else	//����ȷ��
	{
		GetDlgItem(IDC_E_BEND_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_NORM_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_NORM_Z)->EnableWindow(TRUE);
	}
	
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBendPlankStyleDlg::OnRdoBendFaceStyle() 
{
	UpdateData();
	if(m_iBendFaceStyle==0)	//�����Ƕ�ȷ��
	{
		GetDlgItem(IDC_E_BEND_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_NORM_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_Z)->EnableWindow(FALSE);
	}
	else	//����ȷ��
	{
		GetDlgItem(IDC_E_BEND_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NORM_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_NORM_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_NORM_Z)->EnableWindow(TRUE);
	}

}

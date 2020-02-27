// DatumFaceOffsetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DatumFaceOffsetDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatumFaceOffsetDlg dialog


CDatumFaceOffsetDlg::CDatumFaceOffsetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDatumFaceOffsetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDatumFaceOffsetDlg)
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_iBaseStyle = 0;
	//}}AFX_DATA_INIT
}


void CDatumFaceOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatumFaceOffsetDlg)
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Radio(pDX, IDC_RDO_BASE_PT, m_iBaseStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDatumFaceOffsetDlg, CDialog)
	//{{AFX_MSG_MAP(CDatumFaceOffsetDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnBnPasteNorm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatumFaceOffsetDlg message handlers

void CDatumFaceOffsetDlg::OnBnPasteNorm() 
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

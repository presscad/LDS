// LinePartExtendVecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LinePartExtendVecDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinePartExtendVecDlg dialog


CLinePartExtendVecDlg::CLinePartExtendVecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLinePartExtendVecDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinePartExtendVecDlg)
	m_fVecX = 0.0;
	m_fVecY = 0.0;
	m_fVecZ = 0.0;
	m_bLinePartSolidVec = TRUE;
	m_bLinePartLineVec = FALSE;
	//}}AFX_DATA_INIT
	m_pLinePart=NULL;
}


void CLinePartExtendVecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinePartExtendVecDlg)
	DDX_Text(pDX, IDC_E_VEC_X, m_fVecX);
	DDX_Text(pDX, IDC_E_VEC_Y, m_fVecY);
	DDX_Text(pDX, IDC_E_VEC_Z, m_fVecZ);
	DDX_Check(pDX, IDC_CHK_LINEPART_SOLID_VEC, m_bLinePartSolidVec);
	DDX_Check(pDX, IDC_CHK_LINEPART_LINE_VEC, m_bLinePartLineVec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinePartExtendVecDlg, CDialog)
	//{{AFX_MSG_MAP(CLinePartExtendVecDlg)
	ON_BN_CLICKED(IDC_CHK_LINEPART_SOLID_VEC, OnLinePartSolidVec)
	ON_BN_CLICKED(IDC_CHK_LINEPART_LINE_VEC, OnLinePartLineVec)
	ON_BN_CLICKED(IDC_BN_REVERSE_VEC, OnReverseVec)
	ON_BN_CLICKED(IDC_BN_COPY_VEC, OnCopyVec)
	ON_BN_CLICKED(IDC_BN_PASTE_VEC, OnPasteVec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinePartExtendVecDlg message handlers

void CLinePartExtendVecDlg::CalLinePartExtendVec()
{
	if(m_bLinePartSolidVec)
	{
		f3dPoint Vector;
		Sub_Pnt(Vector,m_pLinePart->End(),m_pLinePart->Start());
		normalize(Vector);
		m_fVecX = Vector.x;
		m_fVecY = Vector.y;
		m_fVecZ = Vector.z;
	}
	else if(m_bLinePartLineVec)
	{
		f3dPoint Vector;
		Sub_Pnt(Vector,m_pLinePart->pEnd->Position(false),m_pLinePart->pStart->Position(false));
		normalize(Vector);
		m_fVecX = Vector.x;
		m_fVecY = Vector.y;
		m_fVecZ = Vector.z;
	}
	UpdateData(FALSE);
}

void CLinePartExtendVecDlg::OnLinePartSolidVec() 
{
	UpdateData();
	m_bLinePartLineVec=!m_bLinePartSolidVec;
	CalLinePartExtendVec();
	UpdateData(FALSE);
}

void CLinePartExtendVecDlg::OnLinePartLineVec() 
{
	UpdateData();
	m_bLinePartSolidVec=!m_bLinePartLineVec;
	CalLinePartExtendVec();
	UpdateData(FALSE);
}	

void CLinePartExtendVecDlg::OnReverseVec() 
{
	UpdateData();
	//反号时0不用反号 wht 10-10-20
	CString sNum="";
	sNum.Format("%f",m_fVecX);
	if(sNum.CompareNoCase("0")==0||sNum.CompareNoCase("-0")==0)
		m_fVecX=0;
	else
		m_fVecX *= -1;
	sNum.Format("%f",m_fVecY);
	if(sNum.CompareNoCase("0")==0||sNum.CompareNoCase("-0")==0)
		m_fVecY=0;
	else
		m_fVecY *= -1;
	sNum.Format("%f",m_fVecZ);
	if(sNum.CompareNoCase("0")==0||sNum.CompareNoCase("-0")==0)
		m_fVecZ=0;
	else
		m_fVecZ *= -1;
	UpdateData(FALSE);
}

void CLinePartExtendVecDlg::OnCopyVec() 
{
	f3dPoint vec;
	UpdateData();
	vec.Set(m_fVecX,m_fVecY,m_fVecZ);
	WritePointToClip(vec);
}

void CLinePartExtendVecDlg::OnPasteVec() 
{
	f3dPoint vec;
	if(ReadClipPoint(vec))
	{
		m_fVecX=vec.x;
		m_fVecY=vec.y;
		m_fVecZ=vec.z;
		UpdateData(FALSE);
	}
}

BOOL CLinePartExtendVecDlg::OnInitDialog() 
{
	if(m_pLinePart==NULL)
		return FALSE;
	if(!m_pLinePart->IsLinePart())
		return FALSE;
	CalLinePartExtendVec();
	CDialog::OnInitDialog();
	return TRUE;
}

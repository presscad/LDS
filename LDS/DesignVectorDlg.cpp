//<LOCALE_TRANSLATE Confirm by hxr/>
// DesLsWorkNormDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DesignVectorDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDesignVectorDlg dialog


CDesignVectorDlg::CDesignVectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignVectorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignVectorDlg)
	m_hBasePart = _T("");
	m_hCrossPart = _T("");
	m_iBaseJgWing = 0;
	m_iDirection = 0;
	m_fVecX = 0.0;
	m_fVecY = 0.0;
	m_fVecZ = 0.0;
	m_fNearVecX = 0.0;
	m_fNearVecY = 0.0;
	m_fNearVecZ = 0.0;
	m_iNormStyle = 0;
	//}}AFX_DATA_INIT
}


void CDesignVectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignVectorDlg)
	DDX_Text(pDX, IDC_E_BASE_JG_HANDLE, m_hBasePart);
	DDX_Text(pDX, IDC_E_CROSS_REF_JG_HANDLE, m_hCrossPart);
	DDX_Radio(pDX, IDC_RDO_JG_WING, m_iBaseJgWing);
	DDX_Radio(pDX, IDC_RDO_LS_OUT_DIR, m_iDirection);
	DDX_Text(pDX, IDC_E_VEC_X, m_fVecX);
	DDX_Text(pDX, IDC_E_VEC_Y, m_fVecY);
	DDX_Text(pDX, IDC_E_VEC_Z, m_fVecZ);
	DDX_Text(pDX, IDC_E_NEAR_VEC_X, m_fNearVecX);
	DDX_Text(pDX, IDC_E_NEAR_VEC_Y, m_fNearVecY);
	DDX_Text(pDX, IDC_E_NEAR_VEC_Z, m_fNearVecZ);
	DDX_Radio(pDX, IDC_RDO_NORM_STYLE, m_iNormStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignVectorDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignVectorDlg)
	ON_BN_CLICKED(ID_BN_COPY, OnBnCopy)
	ON_BN_CLICKED(ID_BN_PASTE, OnBnPaste)
	ON_BN_CLICKED(IDC_RDO_NORM_STYLE, OnRdoNormStyle)
	ON_BN_CLICKED(IDC_RDO_NORM_STYLE2, OnRdoNormStyle)
	ON_BN_CLICKED(IDC_RDO_NORM_STYLE3, OnRdoNormStyle)
	ON_BN_CLICKED(IDC_RDO_NORM_STYLE4, OnRdoNormStyle)
	ON_BN_CLICKED(IDC_RDO_NORM_STYLE5, OnRdoNormStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignVectorDlg message handlers

void CDesignVectorDlg::OnRdoNormStyle() 
{
	UpdateData();
	((CEdit*)GetDlgItem(IDC_E_VEC_X))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_VEC_Y))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_VEC_Z))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_NEAR_VEC_X))->SetReadOnly(m_iNormStyle!=2&&m_iNormStyle!=4);
	((CEdit*)GetDlgItem(IDC_E_NEAR_VEC_Y))->SetReadOnly(m_iNormStyle!=2&&m_iNormStyle!=4);
	((CEdit*)GetDlgItem(IDC_E_NEAR_VEC_Z))->SetReadOnly(m_iNormStyle!=2&&m_iNormStyle!=4);
	if(m_iNormStyle==3||m_iNormStyle==4)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_DATUM_PART_NAME)->SetWindowText("datum normal line part");
		GetDlgItem(IDC_S_VAR_TITLE)->SetWindowText("relative direction:");
#else
		GetDlgItem(IDC_S_DATUM_PART_NAME)->SetWindowText("基准法线构件");
		GetDlgItem(IDC_S_VAR_TITLE)->SetWindowText("相对方向:");
#endif
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_DATUM_PART_NAME)->SetWindowText("datum normal line angle");
		GetDlgItem(IDC_S_VAR_TITLE)->SetWindowText("approximate normal line:");
#else
		GetDlgItem(IDC_S_DATUM_PART_NAME)->SetWindowText("基准法线角钢");
		GetDlgItem(IDC_S_VAR_TITLE)->SetWindowText("近似法线:");
#endif
	}
	((CEdit*)GetDlgItem(IDC_E_BASE_JG_HANDLE))->SetReadOnly(m_iNormStyle==0);
	((CEdit*)GetDlgItem(IDC_E_CROSS_REF_JG_HANDLE))->SetReadOnly(m_iNormStyle!=2);
	GetDlgItem(IDC_RDO_JG_WING)->EnableWindow(m_iNormStyle==1);
	GetDlgItem(IDC_RADIO2)->EnableWindow(m_iNormStyle==1);
	GetDlgItem(IDC_RDO_LS_OUT_DIR)->EnableWindow(m_iNormStyle==1||m_iNormStyle==3);
	GetDlgItem(IDC_RADIO3)->EnableWindow(m_iNormStyle==1||m_iNormStyle==3);
}

BOOL CDesignVectorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	((CEdit*)GetDlgItem(IDC_E_VEC_X))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_VEC_Y))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_VEC_Z))->SetReadOnly(m_iNormStyle!=0);
	((CEdit*)GetDlgItem(IDC_E_NEAR_VEC_X))->SetReadOnly(!(m_iNormStyle==2||m_iNormStyle==4));
	((CEdit*)GetDlgItem(IDC_E_NEAR_VEC_Y))->SetReadOnly(!(m_iNormStyle==2||m_iNormStyle==4));
	((CEdit*)GetDlgItem(IDC_E_NEAR_VEC_Z))->SetReadOnly(!(m_iNormStyle==2||m_iNormStyle==4));
	if(m_iNormStyle==3||m_iNormStyle==4)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_DATUM_PART_NAME)->SetWindowText("datum normal line part");
		GetDlgItem(IDC_S_VAR_TITLE)->SetWindowText("relative direction:");
#else
		GetDlgItem(IDC_S_DATUM_PART_NAME)->SetWindowText("基准法线构件");
		GetDlgItem(IDC_S_VAR_TITLE)->SetWindowText("相对方向:");
#endif
	}
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_DATUM_PART_NAME)->SetWindowText("datum normal line angle");
		GetDlgItem(IDC_S_VAR_TITLE)->SetWindowText("approximate normal line:");
#else
		GetDlgItem(IDC_S_DATUM_PART_NAME)->SetWindowText("基准法线角钢");
		GetDlgItem(IDC_S_VAR_TITLE)->SetWindowText("近似法线:");
#endif
	}
	((CEdit*)GetDlgItem(IDC_E_BASE_JG_HANDLE))->SetReadOnly(m_iNormStyle==0);
	((CEdit*)GetDlgItem(IDC_E_CROSS_REF_JG_HANDLE))->SetReadOnly(m_iNormStyle!=2);
	GetDlgItem(IDC_RDO_JG_WING)->EnableWindow(m_iNormStyle==1);
	GetDlgItem(IDC_RADIO2)->EnableWindow(m_iNormStyle==1);
	GetDlgItem(IDC_RDO_LS_OUT_DIR)->EnableWindow(m_iNormStyle==1||m_iNormStyle==3);
	GetDlgItem(IDC_RADIO3)->EnableWindow(m_iNormStyle==1||m_iNormStyle==3);
	
	return TRUE;
}

void CDesignVectorDlg::OnBnCopy() 
{
	f3dPoint point;
	UpdateData();
	point.Set(m_fVecX,m_fVecY,m_fVecZ);
	WritePointToClip(point);
}

void CDesignVectorDlg::OnBnPaste() 
{
	f3dPoint point;
	ReadClipPoint(point);
	m_fVecX=point.x;
	m_fVecY=point.y;
	m_fVecZ=point.z;
	UpdateData(FALSE);
}

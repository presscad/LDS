// EditPlateDatumFaceDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "EditPlateDatumFaceDlg.h"
#include "DesignVectorPropDlg.h"
#include "DatumPointDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CEditPlateDatumFaceDlg dialog


CEditPlateDatumFaceDlg::CEditPlateDatumFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditPlateDatumFaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditPlateDatumFaceDlg)
	m_fBaseX = 0.0;
	m_fBaseY = 0.0;
	m_fBaseZ = 0.0;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	//}}AFX_DATA_INIT
	m_pPoint = NULL;
}


void CEditPlateDatumFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditPlateDatumFaceDlg)
	DDX_Text(pDX, IDC_E_BASE_X, m_fBaseX);
	DDX_Text(pDX, IDC_E_BASE_Y, m_fBaseY);
	DDX_Text(pDX, IDC_E_BASE_Z, m_fBaseZ);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditPlateDatumFaceDlg, CDialog)
	//{{AFX_MSG_MAP(CEditPlateDatumFaceDlg)
	ON_BN_CLICKED(IDC_BN_DES_BASE_POS, OnBnDesBasePos)
	ON_BN_CLICKED(IDC_BN_DES_BASE_NORM, OnBnDesBaseNorm)
	ON_BN_CLICKED(IDC_BN_COPY_BASE, OnBnCopyBase)
	ON_BN_CLICKED(IDC_BN_COPY_NORM, OnBnCopyNorm)
	ON_BN_CLICKED(IDC_BNT_COPY_PLANE2, OnBntCopyPlane2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditPlateDatumFaceDlg message handlers

void CEditPlateDatumFaceDlg::OnBnDesBasePos() 
{	
	CDatumPointDlg des_point_dlg;
	des_point_dlg.m_pPoint=m_pPoint;
#ifdef AFX_TARG_ENU_ENGLISH
	des_point_dlg.m_shDatumPoint.Format("Handle:None");
	des_point_dlg.m_sPointDescription = "The Locate datum point of connect plate";
#else 
	des_point_dlg.m_shDatumPoint.Format("句柄:无");
	des_point_dlg.m_sPointDescription = "连接板定位基点";
#endif
	if(des_point_dlg.DoModal()==IDOK)
	{
		m_pPoint=des_point_dlg.m_pPoint;
		m_fBaseX=m_pPoint->Position().x;
		m_fBaseY=m_pPoint->Position().y;
		m_fBaseZ=m_pPoint->Position().z;
		UpdateData(FALSE);
	}
}

void CEditPlateDatumFaceDlg::OnBnDesBaseNorm() 
{
	CDesignVectorPropDlg vector_dlg;
	vector_dlg.m_xVector=m_pLjPara->norm;
	if(vector_dlg.DoModal()==IDOK)
	{
		m_pLjPara->norm=vector_dlg.m_xVector;
		m_pLjPara->norm.UpdateVector(console.GetActiveModel());
		m_fNormX = m_pLjPara->norm.vector.x;
		m_fNormY = m_pLjPara->norm.vector.y;
		m_fNormZ = m_pLjPara->norm.vector.z;
		UpdateData(FALSE);
	}	
}
BOOL CEditPlateDatumFaceDlg::OnInitDialog()
{
	m_pPoint=&m_pLjPara->origin;
	if(m_pPoint->BelongModel()==NULL)
		m_pPoint->SetBelongModel(console.GetActiveModel());
	((CEdit*)GetDlgItem(IDC_E_BASE_X))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_BASE_Y))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_BASE_Z))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_NORM_X))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_NORM_Y))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_NORM_Z))->SetReadOnly(TRUE);
	m_fNormX=m_pLjPara->norm.vector.x;
	m_fNormY=m_pLjPara->norm.vector.y;
	m_fNormZ=m_pLjPara->norm.vector.z;
	m_fBaseX=m_pLjPara->origin.Position().x;
	m_fBaseY=m_pLjPara->origin.Position().y;
	m_fBaseZ=m_pLjPara->origin.Position().z;

	UpdateData(FALSE);
	return TRUE;
}

void CEditPlateDatumFaceDlg::OnBnCopyBase() 
{
	f3dPoint point;
	UpdateData(TRUE);
	point.x = m_fBaseX;
	point.y = m_fBaseY;
	point.z = m_fBaseZ;
	WritePointToClip(point);	
}

void CEditPlateDatumFaceDlg::OnBnCopyNorm() 
{
	f3dPoint point;
	UpdateData(TRUE);
	point.x = m_fNormX;
	point.y = m_fNormY;
	point.z = m_fNormZ;
	WritePointToClip(point);
}

void CEditPlateDatumFaceDlg::OnBntCopyPlane2() 
{
	f3dPoint face_base,face_norm;
	UpdateData(TRUE);
	face_base.Set(m_fBaseX,m_fBaseY,m_fBaseZ);
	face_norm.Set(m_fNormX,m_fNormY,m_fNormZ);
	WritePlaneToClip(face_base,face_norm);	
}

void CEditPlateDatumFaceDlg::OnOK() 
{	
	CDialog::OnOK();
}

void CEditPlateDatumFaceDlg::OnCancel() 
{
	CDialog::OnCancel();
}
#endif
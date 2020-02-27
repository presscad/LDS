// DefHuoquFaceDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "DefHuoquFaceDlg.h"
#include "GlobalFunc.h"
#include "DesignVectorPropDlg.h"
#include "env_def.h"
#include "DatumPointDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CDefHuoquFaceDlg dialog


CDefHuoquFaceDlg::CDefHuoquFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefHuoquFaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefHuoquFaceDlg)
	m_fEndPosX = 0.0;
	m_fEndPosY = 0.0;
	m_fEndPosZ = 0.0;
	m_fHuoquNormX = 0.0;
	m_fHuoquNormY = 0.0;
	m_fHuoquNormZ = 0.0;
	m_fStartPosX = 0.0;
	m_fStartPosZ = 0.0;
	m_fStartPosY = 0.0;
	m_iHuoquFaceType = 0;
	//}}AFX_DATA_INIT
}


void CDefHuoquFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefHuoquFaceDlg)
	DDX_Text(pDX, IDC_E_END_POS_X, m_fEndPosX);
	DDX_Text(pDX, IDC_E_END_POS_Y, m_fEndPosY);
	DDX_Text(pDX, IDC_E_END_POS_Z, m_fEndPosZ);
	DDX_Text(pDX, IDC_E_HUOQV_NORM_X, m_fHuoquNormX);
	DDX_Text(pDX, IDC_E_HUOQV_NORM_Y, m_fHuoquNormY);
	DDX_Text(pDX, IDC_E_HUOQV_NORM_Z, m_fHuoquNormZ);
	DDX_Text(pDX, IDC_E_START_POS_X, m_fStartPosX);
	DDX_Text(pDX, IDC_E_START_POS_Z, m_fStartPosZ);
	DDX_Text(pDX, IDC_E_START_POS_Y, m_fStartPosY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefHuoquFaceDlg, CDialog)
	//{{AFX_MSG_MAP(CDefHuoquFaceDlg)
	ON_BN_CLICKED(IDC_BN_COPY_VEC_END, OnBnCopyVecEnd)
	ON_BN_CLICKED(IDC_BN_COPY_VEC_START, OnBnCopyVecStart)
	ON_BN_CLICKED(IDC_BN_COPY_VEC_HUOQu, OnBNCOPYVECHUOQu)
	ON_BN_CLICKED(IDC_BN_EDIT_START_POS, OnBnEditStartPos)
	ON_BN_CLICKED(IDC_BN_EDIT_HUOQU, OnBnEditHuoqu)
	ON_BN_CLICKED(IDC_BN_EDIT_END_POS, OnBnEditEndPos)
	ON_BN_CLICKED(IDC_BN_COPY_FACE, OnBnCopyFace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefHuoquFaceDlg message handlers

void CDefHuoquFaceDlg::OnOK() 
{	
	CDialog::OnOK();
}

void CDefHuoquFaceDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CDefHuoquFaceDlg::OnBnCopyVecEnd() 
{
	UpdateData();
	f3dPoint point;
	point.Set(m_fEndPosX,m_fEndPosY,m_fEndPosZ);
	WritePointToClip(point);
}

BOOL CDefHuoquFaceDlg::OnInitDialog()
{
	((CEdit*)GetDlgItem(IDC_E_START_POS_X))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_START_POS_Y))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_START_POS_Z))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_END_POS_X))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_END_POS_Y))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_END_POS_Z))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_HUOQV_NORM_X))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_HUOQV_NORM_Y))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_E_HUOQV_NORM_Z))->SetReadOnly(TRUE);

	if(m_iHuoquFaceType==0)
	{
		m_fHuoquNormX=m_pLjPara->huoqufacenorm[0].vector.x;
		m_fHuoquNormY=m_pLjPara->huoqufacenorm[0].vector.y;
		m_fHuoquNormZ=m_pLjPara->huoqufacenorm[0].vector.z;
		m_fEndPosX=m_pLjPara->huoquline_end[0].Position().x;
		m_fEndPosY=m_pLjPara->huoquline_end[0].Position().y;
		m_fEndPosZ=m_pLjPara->huoquline_end[0].Position().z;
		m_fStartPosX=m_pLjPara->huoquline_start[0].Position().x;
		m_fStartPosY=m_pLjPara->huoquline_start[0].Position().y;
		m_fStartPosZ=m_pLjPara->huoquline_start[0].Position().z;
	}
	else if(m_iHuoquFaceType==1)
	{
		m_fHuoquNormX=m_pLjPara->huoqufacenorm[1].vector.x;
		m_fHuoquNormY=m_pLjPara->huoqufacenorm[1].vector.y;
		m_fHuoquNormZ=m_pLjPara->huoqufacenorm[1].vector.z;
		m_fEndPosX=m_pLjPara->huoquline_end[1].Position().x;
		m_fEndPosY=m_pLjPara->huoquline_end[1].Position().y;
		m_fEndPosZ=m_pLjPara->huoquline_end[1].Position().z;
		m_fStartPosX=m_pLjPara->huoquline_start[1].Position().x;
		m_fStartPosY=m_pLjPara->huoquline_start[1].Position().y;
		m_fStartPosZ=m_pLjPara->huoquline_start[1].Position().z;
	}

	UpdateData(FALSE);
	
	return TRUE;
}

void CDefHuoquFaceDlg::OnBnCopyVecStart() 
{
	UpdateData();
	f3dPoint point;
	point.Set(m_fStartPosX,m_fStartPosY,m_fStartPosZ);
	WritePointToClip(point);	
}

void CDefHuoquFaceDlg::OnBNCOPYVECHUOQu() 
{
	UpdateData();
	f3dPoint point;
	point.Set(m_fHuoquNormX,m_fHuoquNormY,m_fHuoquNormZ);
	WritePointToClip(point);	
}

void CDefHuoquFaceDlg::OnBnEditStartPos() 
{
	CDatumPointDlg des_point_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	des_point_dlg.m_shDatumPoint.Format("Handle:None");
#else 
	des_point_dlg.m_shDatumPoint.Format("句柄:无");
#endif
	if(m_iHuoquFaceType==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		des_point_dlg.m_sPointDescription ="The start point of first bending line ";
#else 
		des_point_dlg.m_sPointDescription ="第一火曲线起始点";
#endif
		des_point_dlg.m_pPoint=&m_pLjPara->huoquline_start[0];
	}
	else if(m_iHuoquFaceType==1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		des_point_dlg.m_sPointDescription ="The start point of second bending line ";
#else 
		des_point_dlg.m_sPointDescription ="第二火曲线起始点";
#endif
		des_point_dlg.m_pPoint=&m_pLjPara->huoquline_start[1];
	}
	UpdateData(FALSE);
	if(des_point_dlg.DoModal()==IDOK)
	{
		m_fStartPosX=m_pLjPara->huoquline_start[0].Position().x;
		m_fStartPosY=m_pLjPara->huoquline_start[0].Position().y;
		m_fStartPosZ=m_pLjPara->huoquline_start[0].Position().z;
		UpdateData(FALSE);
	}
}

void CDefHuoquFaceDlg::OnBnEditHuoqu() 
{
	CDesignVectorPropDlg vector_dlg;
	if(m_iHuoquFaceType==0)
	{
		vector_dlg.m_xVector=m_pLjPara->huoqufacenorm[0];
		if(vector_dlg.DoModal()==IDOK)
		{
			m_pLjPara->huoqufacenorm[0]=vector_dlg.m_xVector;
			m_pLjPara->huoqufacenorm[0].UpdateVector(console.GetActiveModel());
		}	
	}
	else if(m_iHuoquFaceType==1)
	{
		vector_dlg.m_xVector=m_pLjPara->huoqufacenorm[1];
		if(vector_dlg.DoModal()==IDOK)
		{
			m_pLjPara->huoqufacenorm[1]=vector_dlg.m_xVector;
			m_pLjPara->huoqufacenorm[1].UpdateVector(console.GetActiveModel());
		}	
	}
}

void CDefHuoquFaceDlg::OnBnEditEndPos() 
{
	CDatumPointDlg des_point_dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	des_point_dlg.m_shDatumPoint.Format("Handle:None");
#else 
	des_point_dlg.m_shDatumPoint.Format("句柄:无");
#endif
	if(m_iHuoquFaceType==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		des_point_dlg.m_sPointDescription ="The end point of first bending line ";
#else 
		des_point_dlg.m_sPointDescription ="第一火曲线终止点";
#endif
		des_point_dlg.m_pPoint=&m_pLjPara->huoquline_end[0];
	}
	else if(m_iHuoquFaceType==1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		des_point_dlg.m_sPointDescription ="The end point of second bending line";
#else 
		des_point_dlg.m_sPointDescription ="第二火曲线终止点";
#endif
		des_point_dlg.m_pPoint=&m_pLjPara->huoquline_end[1];
	}
	UpdateData(FALSE);
	if(des_point_dlg.DoModal()==IDOK)
	{
		if(m_iHuoquFaceType==0)
		{
			m_fEndPosX=m_pLjPara->huoquline_end[0].Position().x;
			m_fEndPosY=m_pLjPara->huoquline_end[0].Position().y;
			m_fEndPosZ=m_pLjPara->huoquline_end[0].Position().z;
		}
		else
		{
			m_fEndPosX=m_pLjPara->huoquline_end[1].Position().x;
			m_fEndPosY=m_pLjPara->huoquline_end[1].Position().y;
			m_fEndPosZ=m_pLjPara->huoquline_end[1].Position().z;			
		}
		UpdateData(FALSE);
	}
}

void CDefHuoquFaceDlg::OnBnCopyFace() 
{
	f3dPoint face_base,face_norm;
	UpdateData(TRUE);
	face_base.Set(m_fEndPosX,m_fEndPosY,m_fEndPosZ);
	face_norm.Set(m_fHuoquNormX,m_fHuoquNormY,m_fHuoquNormZ);
	WritePlaneToClip(face_base,face_norm);	
}
#endif
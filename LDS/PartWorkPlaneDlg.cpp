//<LOCALE_TRANSLATE Confirm by hxr/>
// PartWorkPlaneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "PartWorkPlaneDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartWorkPlaneDlg dialog
#ifndef __TSA_

CPartWorkPlaneDlg::CPartWorkPlaneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartWorkPlaneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartWorkPlaneDlg)
	m_fBaseX = 0.0;
	m_fBaseY = 0.0;
	m_fBaseZ = 0.0;
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_fOffsetDist = 0.0;
	m_fOffsetAngle = 0.0;
	m_iDatumPosType = 1;
	m_iNormType	 = 0;
	//}}AFX_DATA_INIT
	m_pDatumPart=NULL;
	m_bStart=TRUE;
}


void CPartWorkPlaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartWorkPlaneDlg)
	DDX_Control(pDX, IDC_CMB_DATUM_POS_TYPE, m_cmbDatumPosTyoe);
	DDX_Control(pDX, IDC_CMB_NORM_TYPE, m_cmbNormType);
	DDX_Text(pDX, IDC_E_BASE_X, m_fBaseX);
	DDX_Text(pDX, IDC_E_BASE_Y, m_fBaseY);
	DDX_Text(pDX, IDC_E_BASE_Z, m_fBaseZ);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_CBIndex(pDX, IDC_CMB_DATUM_POS_TYPE, m_iDatumPosType);
	DDX_CBIndex(pDX, IDC_CMB_NORM_TYPE, m_iNormType);
	DDX_Text(pDX, IDC_E_OFFSET_DIST, m_fOffsetDist);
	DDX_Text(pDX, IDC_E_OFFSET_ANGLE, m_fOffsetAngle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartWorkPlaneDlg, CDialog)
	//{{AFX_MSG_MAP(CPartWorkPlaneDlg)
	ON_BN_CLICKED(IDC_BN_COPY_BASE, OnCopyBase)
	ON_BN_CLICKED(IDC_BN_COPY_NORM, OnCopyNorm)
	ON_BN_CLICKED(IDC_BN_PASTE_BASE, OnPasteBase)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnPasteNorm)
	ON_BN_CLICKED(IDC_BNT_COPY_PLANE, OnCopyPlane)
	ON_EN_CHANGE(IDC_E_OFFSET_DIST, OnChangeOffsetDist)
	ON_EN_CHANGE(IDC_E_OFFSET_ANGLE, OnChangeOffsetAngle)
	ON_CBN_SELCHANGE(IDC_CMB_NORM_TYPE, OnSelChangleNormType)
	ON_CBN_SELCHANGE(IDC_CMB_DATUM_POS_TYPE, OnSelChangleDatumPosType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartWorkPlaneDlg message handlers

void CPartWorkPlaneDlg::OnCopyNorm() 
{
	UpdateData();
	f3dPoint point;
	point.Set(m_fNormX,m_fNormY,m_fNormZ);
	WritePointToClip(point);
}

void CPartWorkPlaneDlg::OnCopyBase() 
{
	UpdateData();
	f3dPoint point;
	point.Set(m_fBaseX,m_fBaseY,m_fBaseZ);
	WritePointToClip(point);	
}

void CPartWorkPlaneDlg::OnPasteNorm() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fNormX=point.x;
		m_fNormY=point.y;
		m_fNormZ=point.z;
		UpdateData(FALSE);
	}
}

void CPartWorkPlaneDlg::OnPasteBase() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fBaseX=point.x;
		m_fBaseY=point.y;
		m_fBaseZ=point.z;
		UpdateData(FALSE);
	}
}

void CPartWorkPlaneDlg::OnCopyPlane() 
{
	UpdateData();
	f3dPoint face_base,face_norm;
	face_base.Set(m_fBaseX,m_fBaseY,m_fBaseZ);
	face_norm.Set(m_fNormX,m_fNormY,m_fNormZ);
	WritePlaneToClip(face_base,face_norm);	
}

void CPartWorkPlaneDlg::OnSelChangleDatumPosType() 
{
	if(m_pDatumPart==NULL)
		return;
	UpdateData();
	if(m_pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		if(m_iDatumPosType==0)
			m_fOffsetDist=-m_pDatumPart->GetThick();
		else
			m_fOffsetDist=0;
		GetDlgItem(IDC_E_OFFSET_DIST)->EnableWindow(m_iDatumPosType==2);
	}
	else if(m_pDatumPart->GetClassTypeId()==CLS_PLATE
		||m_pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		double fNormOffset=0;
		if(m_pDatumPart->GetClassTypeId()==CLS_PLATE)
			fNormOffset=((CLDSPlate*)m_pDatumPart)->m_fNormOffset;
		if(m_iDatumPosType==0)
			m_fOffsetDist=m_pDatumPart->GetThick()+fNormOffset;
		else 
			m_fOffsetDist=fNormOffset;
		GetDlgItem(IDC_E_OFFSET_DIST)->EnableWindow(m_iDatumPosType==2);
	}
	else
		GetDlgItem(IDC_E_OFFSET_DIST)->EnableWindow(TRUE);
	UpdateData(FALSE);
	UpdateDatumPoint();
}

void CPartWorkPlaneDlg::OnSelChangleNormType() 
{
	UpdateDatumPoint();
}

void CPartWorkPlaneDlg::OnChangeOffsetDist() 
{
	CString strValue;
	GetDlgItemText(IDC_E_OFFSET_DIST,strValue);
	if(strValue.GetLength()==1&&strValue.CompareNoCase("-")==0)
		return;
	UpdateDatumPoint();
}

void CPartWorkPlaneDlg::OnChangeOffsetAngle() 
{
	CString strValue;
	GetDlgItemText(IDC_E_OFFSET_DIST,strValue);
	if(strValue.GetLength()==1&&strValue.CompareNoCase("-")==0)
		return;
	if(m_pDatumPart&&m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		UpdateDatumPoint();
}

void CPartWorkPlaneDlg::UpdateDatumPoint(BOOL bInit/*=FALSE*/)
{
	if(m_pDatumPart==NULL)
		return;
	if(!bInit)
		UpdateData();
	f3dPoint work_plane_norm,datum_pos;
	if(m_pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)m_pDatumPart;
		work_plane_norm=pLineAngle->get_norm_y_wing();
		if(m_iNormType==0)
			work_plane_norm=pLineAngle->get_norm_x_wing();
		datum_pos=pLineAngle->Start();
		if(!m_bStart)
			datum_pos=pLineAngle->End();
		if(bInit)
		{
			if(m_iDatumPosType==0)
				m_fOffsetDist=-pLineAngle->GetThick();
			else
				m_fOffsetDist=0;
		}
	}
	else if(m_pDatumPart->GetClassTypeId()==CLS_PLATE||m_pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(bInit)
		{
			double fNormOffset=0;
			if(m_pDatumPart->GetClassTypeId()==CLS_PLATE)
				fNormOffset=((CLDSPlate*)m_pDatumPart)->m_fNormOffset;
			if(m_iDatumPosType==0)
				m_fOffsetDist=m_pDatumPart->GetThick()+fNormOffset;
			else 
				m_fOffsetDist=fNormOffset;
		}
		datum_pos=m_pDatumPart->ucs.origin;
		if(m_iNormType==1)	//Y_Z平面法线
			work_plane_norm=m_pDatumPart->ucs.axis_x;
		else if(m_iNormType==2)	//X_Z平面法线
			work_plane_norm=m_pDatumPart->ucs.axis_y;
		else //if(m_iNormType==0)//X_Y平面法线
			work_plane_norm=m_pDatumPart->ucs.axis_z;
	}
	else if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)m_pDatumPart;
		pLineTube->BuildUCS();
		double fOffsetAngle=m_fOffsetAngle*RADTODEG_COEF;
		if(pLineTube->m_bHasWeldRoad&&m_iNormType==1)
			fOffsetAngle=pLineTube->CalWeldRoadAngle();
		work_plane_norm=pLineTube->ucs.axis_y;
		RotateVectorAroundVector(work_plane_norm,fOffsetAngle,pLineTube->ucs.axis_z);
		datum_pos=pLineTube->Start();
		if(!m_bStart)
			datum_pos=pLineTube->End();
	}
	m_fNormX=work_plane_norm.x;
	m_fNormY=work_plane_norm.y;
	m_fNormZ=work_plane_norm.z;
	datum_pos+=work_plane_norm*m_fOffsetDist;
	m_fBaseX=datum_pos.x;
	m_fBaseY=datum_pos.y;
	m_fBaseZ=datum_pos.z;
	UpdateData(FALSE);
}

BOOL CPartWorkPlaneDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_pDatumPart==NULL)
		return FALSE;
	RefreshDlgCtrl();
	UpdateDatumPoint(TRUE);
	return TRUE;
}

void CPartWorkPlaneDlg::RefreshDlgCtrl()
{
	if(m_pDatumPart==NULL)
		return;
	if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)m_pDatumPart;
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("Tube Work Plane");
		while(m_cmbNormType.GetCount()>0)
			m_cmbNormType.DeleteString(0);
		m_cmbNormType.AddString("Datum Edge");
		if(pLineTube->m_bHasWeldRoad)
			m_cmbNormType.AddString("Weld Line");
		while(m_cmbDatumPosTyoe.GetCount()>0)
			m_cmbDatumPosTyoe.DeleteString(0);
		m_cmbDatumPosTyoe.AddString("User-defined");
#else
		SetWindowText("钢管工作面");
		while(m_cmbNormType.GetCount()>0)
			m_cmbNormType.DeleteString(0);
		m_cmbNormType.AddString("基准边");
		if(pLineTube->m_bHasWeldRoad)
			m_cmbNormType.AddString("焊道线");
		while(m_cmbDatumPosTyoe.GetCount()>0)
			m_cmbDatumPosTyoe.DeleteString(0);
		m_cmbDatumPosTyoe.AddString("自定义");
#endif
		m_iDatumPosType=0;	
		GetDlgItem(IDC_S_ANGLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_OFFSET_ANGLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_OFFSET_DIST)->EnableWindow(TRUE);
	}
	else if(m_pDatumPart->GetClassTypeId()==CLS_PLATE
		||m_pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("plate work plane");
		while(m_cmbNormType.GetCount()>0)
			m_cmbNormType.DeleteString(0);
		m_cmbNormType.AddString("X_Y Plane");
		m_cmbNormType.AddString("Y_Z Plane");
		m_cmbNormType.AddString("X_Z Plane");
		while(m_cmbDatumPosTyoe.GetCount()>0)
			m_cmbDatumPosTyoe.DeleteString(0);
		m_cmbDatumPosTyoe.AddString("top plane");
		m_cmbDatumPosTyoe.AddString("bottom plane");
		m_cmbDatumPosTyoe.AddString("user defined");
#else
		SetWindowText("钢板工作面");
		while(m_cmbNormType.GetCount()>0)
			m_cmbNormType.DeleteString(0);
		m_cmbNormType.AddString("X_Y平面");
		m_cmbNormType.AddString("Y_Z平面");
		m_cmbNormType.AddString("X_Z平面");
		while(m_cmbDatumPosTyoe.GetCount()>0)
			m_cmbDatumPosTyoe.DeleteString(0);
		m_cmbDatumPosTyoe.AddString("顶面");
		m_cmbDatumPosTyoe.AddString("底面");
		m_cmbDatumPosTyoe.AddString("自定义");
#endif
		GetDlgItem(IDC_S_ANGLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_OFFSET_ANGLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_OFFSET_DIST)->EnableWindow(m_iDatumPosType==2);
	}
	else //if(m_pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		SetWindowText("angle leg work plane");
		while(m_cmbNormType.GetCount()>0)
			m_cmbNormType.DeleteString(0);
		m_cmbNormType.AddString("X leg plane");
		m_cmbNormType.AddString("Y leg plane");
		while(m_cmbDatumPosTyoe.GetCount()>0)
			m_cmbDatumPosTyoe.DeleteString(0);
		m_cmbDatumPosTyoe.AddString("inner plane");
		m_cmbDatumPosTyoe.AddString("outer plane");
		m_cmbDatumPosTyoe.AddString("user defined");
#else
		SetWindowText("角钢肢平面");
		while(m_cmbNormType.GetCount()>0)
			m_cmbNormType.DeleteString(0);
		m_cmbNormType.AddString("X肢平面");
		m_cmbNormType.AddString("Y肢平面");
		while(m_cmbDatumPosTyoe.GetCount()>0)
			m_cmbDatumPosTyoe.DeleteString(0);
		m_cmbDatumPosTyoe.AddString("内平面");
		m_cmbDatumPosTyoe.AddString("外平面");
		m_cmbDatumPosTyoe.AddString("自定义");
#endif
		GetDlgItem(IDC_S_ANGLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_OFFSET_ANGLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_OFFSET_DIST)->EnableWindow(m_iDatumPosType==2);
	}
}
#endif

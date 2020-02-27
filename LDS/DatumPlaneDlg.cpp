// DatumPlaneDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "DatumPlaneDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatumPlaneDlg dialog


CDatumPlaneDlg::CDatumPlaneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDatumPlaneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDatumPlaneDlg)
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 0.0;
	m_sPlaneDescription = _T("");
	m_sFirPointDescription = _T("");
	m_sSecPointDescription = _T("");
	m_sThirPointDescription = _T("");
	m_shDatumPlane = _T("");
	m_iFirstPoint = -1;
	m_iSecPoint = -1;
	m_iThirdPoint = -1;
	//}}AFX_DATA_INIT
	m_hFirstPoint=m_hSecondPoint=m_hThirdPoint=0;
}


void CDatumPlaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatumPlaneDlg)
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_PLANE_DESCRIP, m_sPlaneDescription);
	DDX_Text(pDX, IDC_S_DATUM_POINT_FIR_DESCRIP, m_sFirPointDescription);
	DDX_Text(pDX, IDC_S_DATUM_POINT_SEC_DESCRIP, m_sSecPointDescription);
	DDX_Text(pDX, IDC_S_DATUM_POINT_THIR_DESCRIP, m_sThirPointDescription);
	DDX_Text(pDX, IDC_S_H_DATUM_PLANE, m_shDatumPlane);
	DDX_CBIndex(pDX, IDC_CMB_H_DATUM_POINT_FIR, m_iFirstPoint);
	DDX_CBIndex(pDX, IDC_CMB_H_DATUM_POINT_SEC, m_iSecPoint);
	DDX_CBIndex(pDX, IDC_CMB_H_DATUM_POINT_THIR, m_iThirdPoint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDatumPlaneDlg, CDialog)
	//{{AFX_MSG_MAP(CDatumPlaneDlg)
	ON_BN_CLICKED(ID_BN_UPDATE_PLANE, OnBnUpdatePlane)
	ON_BN_CLICKED(ID_BN_COPY_PLANE, OnBnCopyPlane)
	ON_BN_CLICKED(IDC_BN_COPY_NORM, OnBnCopyNorm)
	ON_CBN_SELCHANGE(IDC_CMB_H_DATUM_POINT_FIR, OnSelchangeCmbHDatumPointFir)
	ON_CBN_SELCHANGE(IDC_CMB_H_DATUM_POINT_SEC, OnSelchangeCmbHDatumPointSec)
	ON_CBN_SELCHANGE(IDC_CMB_H_DATUM_POINT_THIR, OnSelchangeCmbHDatumPointThir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatumPlaneDlg message handlers

void CDatumPlaneDlg::OnBnUpdatePlane() 
{
	CLDSPlane plane;
	UpdateData();
	plane.hPointArr[0]=m_hFirstPoint;
	plane.hPointArr[1]=m_hSecondPoint;
	plane.hPointArr[2]=m_hThirdPoint;
	if( console.FromPointHandle(plane.hPointArr[0])==NULL||
		console.FromPointHandle(plane.hPointArr[1])==NULL||
		console.FromPointHandle(plane.hPointArr[2])==NULL)
		return;
	if(plane.UpdatePlane(console.GetActiveModel()))
	{
		m_fNormX=plane.GetWorkNorm().x;
		m_fNormY=plane.GetWorkNorm().y;
		m_fNormZ=plane.GetWorkNorm().z;
		UpdateData(FALSE);
	}
}

void CDatumPlaneDlg::OnOK() 
{
	UpdateData();
	if(m_hFirstPoint==m_hSecondPoint||
		m_hFirstPoint==m_hThirdPoint||
		m_hSecondPoint==m_hThirdPoint)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Exist duplicate nodes!");
#else 
		AfxMessageBox("存在重复点!");
#endif
		return;
	}
	if( console.FromPointHandle(m_hFirstPoint)==NULL||
		console.FromPointHandle(m_hSecondPoint)==NULL||
		console.FromPointHandle(m_hThirdPoint)==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The specified handle of datum point is illegal!");
#else 
		AfxMessageBox("指定基准点句柄非法!");
#endif
		return;
	}
	CDialog::OnOK();
}

BOOL CDatumPlaneDlg::OnInitDialog() 
{
	CString sPointHandle="";
	int iFirstCurSel=-1,iSecCurSel=-1,iThirCurSel=-1;
	CComboBox *pCMBFir = (CComboBox*)GetDlgItem(IDC_CMB_H_DATUM_POINT_FIR);
	CComboBox *pCMBSec = (CComboBox*)GetDlgItem(IDC_CMB_H_DATUM_POINT_SEC);
	CComboBox *pCMBThir = (CComboBox*)GetDlgItem(IDC_CMB_H_DATUM_POINT_THIR);
	for(CLDSPoint *pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
	{
		sPointHandle.Format("%X",pPoint->handle);
		int iCurSel=pCMBFir->AddString(sPointHandle);
		pCMBFir->SetItemDataPtr(iCurSel,pPoint);
		iCurSel=pCMBSec->AddString(sPointHandle);
		pCMBSec->SetItemDataPtr(iCurSel,pPoint);
		iCurSel=pCMBThir->AddString(sPointHandle);
		pCMBThir->SetItemDataPtr(iCurSel,pPoint);
		if(pPoint->handle==m_hFirstPoint)
		{
			iFirstCurSel=iCurSel;
			m_sFirPointDescription=pPoint->description;
		}
		if(pPoint->handle==m_hSecondPoint)
		{
			iSecCurSel=iCurSel;
			m_sSecPointDescription=pPoint->description;
		}
		if(pPoint->handle==m_hThirdPoint)
		{
			iThirCurSel=iCurSel;
			m_sThirPointDescription=pPoint->description;
		}
	}
	UpdateData(FALSE);
	pCMBFir->SetCurSel(iFirstCurSel);
	pCMBSec->SetCurSel(iSecCurSel);
	pCMBThir->SetCurSel(iThirCurSel);
	UpdateData();
	CDialog::OnInitDialog();
	return TRUE;
}

void CDatumPlaneDlg::OnBnCopyPlane() 
{
	CLDSPlane plane;
	UpdateData();
	plane.hPointArr[0]=m_hFirstPoint;
	plane.hPointArr[1]=m_hSecondPoint;
	plane.hPointArr[2]=m_hThirdPoint;
	if( console.FromPointHandle(plane.hPointArr[0])==NULL||
		console.FromPointHandle(plane.hPointArr[1])==NULL||
		console.FromPointHandle(plane.hPointArr[2])==NULL)
		return;
	if(plane.UpdatePlane(console.GetActiveModel()))
	{
		m_fNormX=plane.GetWorkNorm().x;
		m_fNormY=plane.GetWorkNorm().y;
		m_fNormZ=plane.GetWorkNorm().z;
		WritePlaneToClip(plane.GetBasePos(),plane.GetWorkNorm());
		UpdateData(FALSE);
	}
}

void CDatumPlaneDlg::OnBnCopyNorm() 
{
	f3dPoint pt(m_fNormX,m_fNormY,m_fNormZ);
	WritePointToClip(pt);
}

void CDatumPlaneDlg::OnSelchangeCmbHDatumPointFir() 
{
	UpdateData();
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_H_DATUM_POINT_FIR);
	CLDSPoint *pPoint=(CLDSPoint*)pCMB->GetItemDataPtr(pCMB->GetCurSel());
	if(pPoint)
	{
		m_sFirPointDescription=pPoint->description;
		m_hFirstPoint=pPoint->handle;
	}
	UpdateData(FALSE);
}

void CDatumPlaneDlg::OnSelchangeCmbHDatumPointSec() 
{
	UpdateData();
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_H_DATUM_POINT_SEC);
	CLDSPoint *pPoint=(CLDSPoint*)pCMB->GetItemDataPtr(pCMB->GetCurSel());
	if(pPoint)
	{
		m_sSecPointDescription=pPoint->description;
		m_hSecondPoint=pPoint->handle;
	}
	UpdateData(FALSE);
}

void CDatumPlaneDlg::OnSelchangeCmbHDatumPointThir() 
{
	UpdateData();
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_H_DATUM_POINT_THIR);
	CLDSPoint *pPoint=(CLDSPoint*)pCMB->GetItemDataPtr(pCMB->GetCurSel());
	if(pPoint)
	{
		m_sThirPointDescription=pPoint->description;
		m_hThirdPoint=pPoint->handle;
	}
	UpdateData(FALSE);
}

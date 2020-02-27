// PartUcsDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "PartUcsDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartUcsDlg dialog


CPartUcsDlg::CPartUcsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartUcsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartUcsDlg)
	m_fAxisX_X = 1.0;
	m_fAxisX_Y = 0.0;
	m_fAxisX_Z = 0.0;
	m_fAxisY_X = 0.0;
	m_fAxisY_Y = 1.0;
	m_fAxisY_Z = 0.0;
	m_fAxisZ_X = 0.0;
	m_fAxisZ_Y = 0.0;
	m_fAxisZ_Z = 1.0;
	m_fOriginPosX = 0.0;
	m_fOriginPosY = 0.0;
	m_fOriginPosZ = 0.0;
	m_bCanModifyAxisX = TRUE;
	m_bCanModifyAxisY = FALSE;
	m_bCanModifyAxisZ = TRUE;
	//}}AFX_DATA_INIT
	ucs.axis_x.Set(1.0,0.0,0.0);
	ucs.axis_y.Set(0.0,1.0,0.0);
	ucs.axis_z.Set(0.0,0.0,1.0);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption = "Coordinate System";
#else 
	m_sCaption = "坐标系";
#endif
	m_bCanModifyOrigin = TRUE;
}


void CPartUcsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartUcsDlg)
	DDX_Text(pDX, IDC_E_AXIS_X_X, m_fAxisX_X);
	DDX_Text(pDX, IDC_E_AXIS_X_Y, m_fAxisX_Y);
	DDX_Text(pDX, IDC_E_AXIS_X_Z, m_fAxisX_Z);
	DDX_Text(pDX, IDC_E_AXIS_Y_X, m_fAxisY_X);
	DDX_Text(pDX, IDC_E_AXIS_Y_Y, m_fAxisY_Y);
	DDX_Text(pDX, IDC_E_AXIS_Y_Z, m_fAxisY_Z);
	DDX_Text(pDX, IDC_E_AXIS_Z_X, m_fAxisZ_X);
	DDX_Text(pDX, IDC_E_AXIS_Z_Y, m_fAxisZ_Y);
	DDX_Text(pDX, IDC_E_AXIS_Z_Z, m_fAxisZ_Z);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_X, m_fOriginPosX);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Y, m_fOriginPosY);
	DDX_Text(pDX, IDC_E_ORIGIN_POS_Z, m_fOriginPosZ);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_X, m_bCanModifyAxisX);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Y, m_bCanModifyAxisY);
	DDX_Check(pDX, IDC_CHK_ENABLE_AXIS_Z, m_bCanModifyAxisZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartUcsDlg, CDialog)
	//{{AFX_MSG_MAP(CPartUcsDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_X, OnBnPasteAxisX)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Y, OnBnPasteAxisY)
	ON_BN_CLICKED(IDC_BN_PASTE_AXIS_Z, OnBnPasteAxisZ)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN_POS, OnBnPasteOriginPos)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_X, OnBnCopyAxisX)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Y, OnBnCopyAxisY)
	ON_BN_CLICKED(IDC_BN_COPY_AXIS_Z, OnBnCopyAxisZ)
	ON_BN_CLICKED(IDC_BN_COPY_ORIGIN_POS, OnBnCopyOriginPos)
	ON_BN_CLICKED(IDC_BN_CPY_UCS, OnBnCpyUcs)
	ON_BN_CLICKED(IDC_BN_PASTE_UCS, OnBnPasteUcs)
	ON_BN_CLICKED(IDC_BN_TO_RIGHT_HAND, OnBnToRightHand)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_X, OnChkEnableAxisX)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Y, OnChkEnableAxisY)
	ON_BN_CLICKED(IDC_CHK_ENABLE_AXIS_Z, OnChkEnableAxisZ)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartUcsDlg message handlers

BOOL CPartUcsDlg::OnInitDialog() 
{
	m_bCanModifyAxisX=FALSE;
	m_bCanModifyAxisY=!m_bCanModifyAxisX;
	m_bCanModifyAxisZ=TRUE;
	CDialog::OnInitDialog();
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);

	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);

	GetDlgItem(IDC_BN_PASTE_ORIGIN_POS)->EnableWindow(m_bCanModifyOrigin);
	GetDlgItem(IDC_E_ORIGIN_POS_X)->EnableWindow(m_bCanModifyOrigin);
	GetDlgItem(IDC_E_ORIGIN_POS_Y)->EnableWindow(m_bCanModifyOrigin);
	GetDlgItem(IDC_E_ORIGIN_POS_Z)->EnableWindow(m_bCanModifyOrigin);
	UpdateUcs(FALSE);
	//设置对话框标题
	SetWindowText(m_sCaption);
	return TRUE;
}

void CPartUcsDlg::OnBnPasteAxisX() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisX_X = point.x;
		m_fAxisX_Y = point.y;
		m_fAxisX_Z = point.z;
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		UpdateData(FALSE);
	}
}

void CPartUcsDlg::OnBnPasteAxisY() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisY_X = point.x;
		m_fAxisY_Y = point.y;
		m_fAxisY_Z = point.z;
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		UpdateData(FALSE);
	}
}

void CPartUcsDlg::OnBnPasteAxisZ() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fAxisZ_X = point.x;
		m_fAxisZ_Y = point.y;
		m_fAxisZ_Z = point.z;
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		UpdateData(FALSE);
	}
}

void CPartUcsDlg::OnBnPasteOriginPos() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fOriginPosX = point.x;
		m_fOriginPosY = point.y;
		m_fOriginPosZ = point.z;
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
		UpdateData(FALSE);
	}
}

void CPartUcsDlg::OnOK() 
{
	UpdateUcs();
	if(ucs.axis_x.IsZero()||ucs.axis_y.IsZero()||ucs.axis_z.IsZero())
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("CSYS is illegal(Axis direction cannot be zero)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("CSYS is illegal(Not right hand spiral orthogonal coordinate)");
#else 
		AfxMessageBox("坐标系不合法(坐标轴方向不能为零)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("坐标系不合法(不是右手螺旋正交的坐标系)");
#endif
	else
		CDialog::OnOK();
}

void CPartUcsDlg::UpdateUcs(BOOL bFlag)
{
	if(bFlag)	//交互界面向ucs更新
	{
		UpdateData(TRUE);
		ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
		ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
		ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
		ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	}
	else	//ucs向交互界面更新
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CPartUcsDlg::OnBnCopyAxisX() 
{
	f3dPoint point;
	point.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	WritePointToClip(point);
}

void CPartUcsDlg::OnBnCopyAxisY() 
{
	f3dPoint point;
	point.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	WritePointToClip(point);
}

void CPartUcsDlg::OnBnCopyAxisZ() 
{
	f3dPoint point;
	point.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WritePointToClip(point);
}

void CPartUcsDlg::OnBnCopyOriginPos() 
{
	f3dPoint point;
	point.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	WritePointToClip(point);
}

void CPartUcsDlg::OnBnCpyUcs() 
{
	UCS_STRU ucs;
	UpdateData();
	ucs.origin.Set(m_fOriginPosX,m_fOriginPosY,m_fOriginPosZ);
	ucs.axis_x.Set(m_fAxisX_X,m_fAxisX_Y,m_fAxisX_Z);
	ucs.axis_y.Set(m_fAxisY_X,m_fAxisY_Y,m_fAxisY_Z);
	ucs.axis_z.Set(m_fAxisZ_X,m_fAxisZ_Y,m_fAxisZ_Z);
	WriteUCSToClip(ucs);
}

void CPartUcsDlg::OnBnPasteUcs() 
{
	UCS_STRU;
	if(ReadClipUCS(ucs))
	{
		m_fAxisX_X = ucs.axis_x.x;
		m_fAxisX_Y = ucs.axis_x.y;
		m_fAxisX_Z = ucs.axis_x.z;

		m_fAxisY_X = ucs.axis_y.x;
		m_fAxisY_Y = ucs.axis_y.y;
		m_fAxisY_Z = ucs.axis_y.z;

		m_fAxisZ_X = ucs.axis_z.x;
		m_fAxisZ_Y = ucs.axis_z.y;
		m_fAxisZ_Z = ucs.axis_z.z;

		m_fOriginPosX = ucs.origin.x;
		m_fOriginPosY = ucs.origin.y;
		m_fOriginPosZ = ucs.origin.z;
		UpdateData(FALSE);
	}
}

void CPartUcsDlg::OnBnToRightHand() 
{
	UpdateUcs();
	if(!m_bCanModifyAxisZ)
	{	//以X-Y轴为基准	 X轴方向与用户输入方向保持一致，由XY计算出Z轴方向然后重新计算Y轴方向
		ucs.axis_z = ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		normalize(ucs.axis_x);
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
	}
	else if(!m_bCanModifyAxisY)
	{	//以Z-X轴为基准	 Z轴方向与用户输入方向保持一致
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
		normalize(ucs.axis_y);
		normalize(ucs.axis_x);
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
	}
	else if(!m_bCanModifyAxisX)
	{	//以Y-Z轴为基准	Y轴方向与用户输入方向保持一致
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
		ucs.axis_z = ucs.axis_x^ucs.axis_y;
	}
	UpdateUcs(FALSE);
}

void CPartUcsDlg::OnChkEnableAxisX() 
{
	UpdateData();
	if(!m_bCanModifyAxisX)
		m_bCanModifyAxisY=m_bCanModifyAxisZ=TRUE;
	else
	{
		m_bCanModifyAxisY = TRUE;
		m_bCanModifyAxisZ = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//重绘 wht 10-08-24
}

void CPartUcsDlg::OnChkEnableAxisY() 
{
	UpdateData();
	if(!m_bCanModifyAxisY)
		m_bCanModifyAxisX=m_bCanModifyAxisZ=TRUE;
	else
	{
		m_bCanModifyAxisX = TRUE;
		m_bCanModifyAxisZ = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//重绘 wht 10-08-24
}

void CPartUcsDlg::OnChkEnableAxisZ() 
{
	UpdateData();
	if(!m_bCanModifyAxisZ)
		m_bCanModifyAxisX=m_bCanModifyAxisY=TRUE;
	else
	{
		m_bCanModifyAxisX = TRUE;
		m_bCanModifyAxisY = FALSE;
	}
	GetDlgItem(IDC_BN_PASTE_AXIS_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_X)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Y)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_E_AXIS_X_Z)->EnableWindow(m_bCanModifyAxisX);
	GetDlgItem(IDC_BN_PASTE_AXIS_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_X)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Y)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_E_AXIS_Y_Z)->EnableWindow(m_bCanModifyAxisY);
	GetDlgItem(IDC_BN_PASTE_AXIS_Z)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_X)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Y)->EnableWindow(m_bCanModifyAxisZ);
	GetDlgItem(IDC_E_AXIS_Z_Z)->EnableWindow(m_bCanModifyAxisZ);
	UpdateData(FALSE);
	Invalidate(FALSE);	//重绘 wht 10-08-24
}

HBRUSH CPartUcsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(pWnd->GetDlgCtrlID()==IDC_S_RED)
		pDC->SetTextColor(RGB(255,0,0));
	else if(pWnd->GetDlgCtrlID()==IDC_S_BLUE)
		pDC->SetTextColor(RGB(0,130,0));
	else if(!m_bCanModifyAxisZ)
	{	//以X-Y轴为基准	 X轴方向与用户输入方向保持一致，由XY计算出Z轴方向然后重新计算Y轴方向
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_X)
			pDC->SetTextColor(RGB(255,0,0));	//不可变
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Y)
			pDC->SetTextColor(RGB(0,130,0));	//可变
	}
	else if(!m_bCanModifyAxisY)
	{	//以Z-X轴为基准	 Z轴方向与用户输入方向保持一致
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Z)
			pDC->SetTextColor(RGB(255,0,0));
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_X)
			pDC->SetTextColor(RGB(0,130,0));
	}
	else if(!m_bCanModifyAxisX)
	{	//以Y-Z轴为基准	Y轴方向与用户输入方向保持一致
		if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Y)
			pDC->SetTextColor(RGB(255,0,0));
		else if(pWnd->GetDlgCtrlID()==IDC_CHK_ENABLE_AXIS_Z)
			pDC->SetTextColor(RGB(0,130,0));
	}
	return hbr;
}

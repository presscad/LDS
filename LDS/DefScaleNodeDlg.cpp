// DefScaleNodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DefScaleNodeDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefScaleNodeDlg dialog


CDefScaleNodeDlg::CDefScaleNodeDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CDefScaleNodeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefScaleNodeDlg)
	m_fOriginX = 0.0;
	m_fOriginY = 0.0;
	m_fOriginZ = 0.0;
	m_nArrayNum = 1;
	m_bMirAxisX = FALSE;
	m_bMirAxisY = FALSE;
	m_bMirAxisZ = FALSE;
	m_bMirRotation = FALSE;
	m_nDivideByN = 2;
	m_bMirAxisXYZ = FALSE;
	m_nRotateAngle = 90;
	m_bAdvancedOption = FALSE;
	m_b45MirrorsSymmetry=FALSE;
	m_b135MirrorSymmetry=FALSE;
	//}}AFX_DATA_INIT
}


void CDefScaleNodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefScaleNodeDlg)
	DDX_Text(pDX, IDC_E_ORIGIN_X, m_fOriginX);
	DDX_Text(pDX, IDC_E_ORIGIN_Y, m_fOriginY);
	DDX_Text(pDX, IDC_E_ORIGIN_Z, m_fOriginZ);
	DDX_Text(pDX, IDC_E_ARRAY_NUM, m_nArrayNum);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_X, m_bMirAxisX);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Y, m_bMirAxisY);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Z, m_bMirAxisZ);
	DDX_Check(pDX, IDC_CHK_MIR_ROTATE, m_bMirRotation);
	DDX_Check(pDX, IDC_CHK_ADVANCED_OPTION, m_bAdvancedOption);
	DDX_Text(pDX, IDC_EDIT_DIVIDE_N, m_nDivideByN);
	DDV_MinMaxInt(pDX, m_nDivideByN, 1, 100);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_XYZ, m_bMirAxisXYZ);
	DDX_Text(pDX, IDC_E_ROTATE_ANGLE, m_nRotateAngle);
	DDV_MinMaxInt(pDX, m_nRotateAngle, -360, 360);
	DDX_Check(pDX, IDC_CHK_45_MIR,m_b45MirrorsSymmetry);
	DDX_Check(pDX,IDC_CHK_135_MIR,m_b135MirrorSymmetry);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefScaleNodeDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CDefScaleNodeDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN, OnBnPasteOrigin)
	ON_BN_CLICKED(IDC_CHK_MIR_ROTATE, OnChkMirRotate)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_XYZ, OnMirAxisXyz)
	ON_BN_CLICKED(IDC_BTN_OK, OnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_CHK_ADVANCED_OPTION, OnAdvancedOption)
	ON_BN_CLICKED(IDC_CHK_45_MIR, OnChkMir45Plane)
	ON_BN_CLICKED(IDC_CHK_135_MIR, OnChkMir135Plane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefScaleNodeDlg message handlers

BOOL CDefScaleNodeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_fOriginX = mirmsg.origin.x;
	m_fOriginY = mirmsg.origin.y;
	m_fOriginZ = mirmsg.origin.z;
	if(mirmsg.axis_flag&1)
		m_bMirAxisX=TRUE;
	else
		m_bMirAxisX=FALSE;
	if(mirmsg.axis_flag&2)
		m_bMirAxisY=TRUE;
	else
		m_bMirAxisY=FALSE;
	if(mirmsg.axis_flag&4)
		m_bMirAxisZ=TRUE;
	else
		m_bMirAxisZ=FALSE;
	if(mirmsg.axis_flag&8)
		m_bMirRotation=TRUE;
	else
		m_bMirRotation=FALSE;
	m_nArrayNum = mirmsg.array_num;
	m_nRotateAngle = mirmsg.rotate_angle;
	GetDlgItem(IDC_E_ROTATE_ANGLE)->EnableWindow(m_bMirRotation);
	GetDlgItem(IDC_E_ARRAY_NUM)->EnableWindow(m_bMirRotation);
	GetDlgItem(IDC_WORK_PANEL)->ShowWindow(SW_HIDE);
	if(m_bAdvancedOption==FALSE)	//对话框默认为展开，不启用高级设置需调整对话框
		AdjustWndPosition();
	//CDialog::OnInitDialog()在本行编译LMA则找不到IDC_E_ARRAY_NUM 编译TMA无问题 原因未知 wht09-09-05
	MoveWndToLocation();
	return TRUE;
}

void CDefScaleNodeDlg::OnChkMirRotate() 
{
	UpdateData();
	GetDlgItem(IDC_E_ROTATE_ANGLE)->EnableWindow(m_bMirRotation);
	GetDlgItem(IDC_E_ARRAY_NUM)->EnableWindow(m_bMirRotation);
	UpdateData(FALSE);
}

void CDefScaleNodeDlg::OnBnPasteOrigin() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fOriginX = point.x;
		m_fOriginY = point.y;
		m_fOriginZ = point.z;
		UpdateData(FALSE);
	}
}

void CDefScaleNodeDlg::OnMirAxisXyz() 
{
	UpdateData();
	m_bMirAxisX=m_bMirAxisY=m_bMirAxisZ=m_bMirAxisXYZ;
	UpdateData(FALSE);
}

void CDefScaleNodeDlg::OnOk() 
{
	CDialog::OnOK();
	mirmsg.origin.Set(m_fOriginX,m_fOriginY,m_fOriginZ);
	mirmsg.axis_flag=0;
	if(m_bMirAxisX)
		mirmsg.axis_flag|=1;
	if(m_bMirAxisY)
		mirmsg.axis_flag|=2;
	if(m_bMirAxisZ)
		mirmsg.axis_flag|=4;
	if(m_bMirRotation)
		mirmsg.axis_flag|=8;
	mirmsg.array_num = m_nArrayNum;
	mirmsg.rotate_angle = m_nRotateAngle;
}

void CDefScaleNodeDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CDefScaleNodeDlg::OnChkMir45Plane()
{
	UpdateData();
	if(m_b45MirrorsSymmetry)
		m_b135MirrorSymmetry=FALSE;
	UpdateData(FALSE);
}

void CDefScaleNodeDlg::OnChkMir135Plane()
{
	UpdateData();
	if(m_b135MirrorSymmetry)
		m_b45MirrorsSymmetry=FALSE;
	UpdateData(FALSE);
}

void CDefScaleNodeDlg::OnAdvancedOption() 
{
	UpdateData();
	AdjustWndPosition();
	UpdateData(FALSE);
}

void CDefScaleNodeDlg::AdjustWndPosition()
{
	CRect panel_rc;
	GetDlgItem(IDC_WORK_PANEL)->GetWindowRect(panel_rc);
	long nOffset=0;
	if(m_bAdvancedOption)
	{
		GetDlgItem(IDC_CHK_45_MIR)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_135_MIR)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_ORIGIN_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_ORIGIN_Y)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_E_ORIGIN_Z)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_ORIGIN_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_ORIGIN_Y)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_ORIGIN_Z)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BN_PASTE_ORIGIN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_S_MIR_ORIGIN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_ADVANCED_OPTION)->SetWindowText("-");
		nOffset=panel_rc.Height();
	}
	else
	{
		m_b45MirrorsSymmetry=FALSE;
		m_b135MirrorSymmetry=FALSE;
		GetDlgItem(IDC_CHK_45_MIR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_135_MIR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_ORIGIN_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_ORIGIN_Y)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_ORIGIN_Z)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_ORIGIN_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_ORIGIN_Y)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_ORIGIN_Z)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BN_PASTE_ORIGIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_S_MIR_ORIGIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_ADVANCED_OPTION)->SetWindowText("+");
		nOffset=-panel_rc.Height();
	}
	//调整对话框大小
	UpdateData();//调整窗口大小
	CRect full_rc;
	GetWindowRect(full_rc);
	full_rc.bottom+=nOffset;
	MoveWindow(full_rc);
	//调整按钮位置
	CWnd *pBtn=NULL;
	for(int i=0;i<2;i++)
	{
		if(i==0)
			pBtn=GetDlgItem(IDC_BTN_OK);
		else
			pBtn=GetDlgItem(IDC_BTN_CANCEL);
		CRect rcBtn;
		pBtn->GetWindowRect(rcBtn);
		ScreenToClient(rcBtn);
		rcBtn.top+=nOffset;
		rcBtn.bottom+=nOffset;
		pBtn->MoveWindow(rcBtn);
	}
}

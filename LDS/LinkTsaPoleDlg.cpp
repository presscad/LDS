// LinkTubeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LinkTsaPoleDlg.h"
#include "Query.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if defined(__TSA_)||defined(__LDS_)
/////////////////////////////////////////////////////////////////////////////
// CLinkTsaPoleDlg dialog

CLinkTsaPoleDlg::CLinkTsaPoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLinkTsaPoleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkTsaPoleDlg)
	m_iPart1stChar = 1;
	m_iPart2ndChar = 0;
	m_iPoleMat = 0;
	m_bMirSideSpace = FALSE;
	m_uStatMatNo = 0;
	m_bSelSize = TRUE;
	m_iStartForceType = 1;
	m_iStartJointType = 2;
	m_iEndForceType = 1;
	m_iEndJointType = 2;
	m_iCalLenType = 0;
	m_bMirAxisX = FALSE;
	m_bMirAxisXYZ = FALSE;
	m_bMirAxisY = FALSE;
	m_bMirAxisZ = FALSE;
	//}}AFX_DATA_INIT
	m_iPoleType = 0;
	memset(sPartNoArr,0,4*4*16);
}


void CLinkTsaPoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkTsaPoleDlg)
	DDX_Radio(pDX, IDC_HEAD_PART, m_iPart1stChar);
	DDX_Radio(pDX, IDC_MAIN_PART, m_iPart2ndChar);
	DDX_CBIndex(pDX, IDC_CMB_POLE_MATERIAL, m_iPoleMat);
	DDX_Check(pDX, IDC_CHK_MIR_SIDE_FACE, m_bMirSideSpace);
	DDX_Text(pDX, IDC_E_STAT_MAT_NO, m_uStatMatNo);
	DDX_Check(pDX, IDC_CHK_IS_SELSIZE, m_bSelSize);
	DDX_CBIndex(pDX, IDC_CMB_START_FORCE_TYPE, m_iStartForceType);
	DDX_CBIndex(pDX, IDC_CMB_START_JOINT_TYPE, m_iStartJointType);
	DDX_CBIndex(pDX, IDC_CMB_END_FORCE_TYPE, m_iEndForceType);
	DDX_CBIndex(pDX, IDC_CMB_END_JOINT_TYPE, m_iEndJointType);
	DDX_CBIndex(pDX, IDC_CMB_CAL_LEN_TYPE, m_iCalLenType);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_X, m_bMirAxisX);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_XYZ, m_bMirAxisXYZ);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Y, m_bMirAxisY);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Z, m_bMirAxisZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinkTsaPoleDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkTsaPoleDlg)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_XYZ, OnChkMirAxisXyz)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_X, OnMirAxisX)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Y, OnMirAxisY)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Z, OnMirAxisZ)
	ON_BN_CLICKED(IDC_BN_NEW_STAT_MAT_NO, OnBnNewStatMatNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkTsaPoleDlg message handlers

BOOL CLinkTsaPoleDlg::OnInitDialog() 
{
	if(m_pStartNode == NULL || m_pEndNode == NULL)
		return FALSE;
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_POLE_MATERIAL);
	AddSteelMatRecord(pCMB);
	xy1_yz2_xz3 = 0;	
	mirmsg=GetPoleValidMirMsg(m_pStartNode,m_pEndNode,&xy1_yz2_xz3);
	if(!(mirmsg.axis_flag&1))
	{	//不能进行X对称
		m_bMirAxisX=FALSE;
		GetDlgItem(IDC_CHK_MIR_AXIS_X)->EnableWindow(FALSE);
	}
	if(!(mirmsg.axis_flag&2))
	{	//不能进行Y对称
		m_bMirAxisY=FALSE;
		GetDlgItem(IDC_CHK_MIR_AXIS_Y)->EnableWindow(FALSE);
	}
	if(!(mirmsg.axis_flag&4))
	{	//不能进行Z对称
		m_bMirAxisZ=FALSE;
		GetDlgItem(IDC_CHK_MIR_AXIS_Z)->EnableWindow(FALSE);
	}
	if(m_bMirAxisXYZ)
	{
		if(mirmsg.axis_flag&1)	//允许X对称
			m_bMirAxisX = m_bMirAxisXYZ;
		if(mirmsg.axis_flag&2)	//允许Y对称
			m_bMirAxisY = m_bMirAxisXYZ;
		if(mirmsg.axis_flag&4)	//允许Z对称
			m_bMirAxisZ = m_bMirAxisXYZ;
		if(xy1_yz2_xz3==1)		//XY对称互斥
			m_bMirAxisY=FALSE;
		else if(xy1_yz2_xz3==2)	//YZ对称互斥
			m_bMirAxisY=FALSE;
		else if(xy1_yz2_xz3==3)	//XZ对称互斥
			m_bMirAxisX=FALSE;
	}
	//else
	//	m_bMirAxisX=m_bMirAxisY=m_bMirAxisZ=FALSE;
	CDialog::OnInitDialog();
	return TRUE;
}

void CLinkTsaPoleDlg::OnBnNewStatMatNo() 
{
	UpdateData();
	m_uStatMatNo=Ta.GetNewStatNo();
	UpdateData(FALSE);
}

void CLinkTsaPoleDlg::OnChkMirAxisXyz() 
{
	UpdateData();
	if(m_bMirAxisXYZ)
	{
		if(mirmsg.axis_flag&1)	//允许X对称
			m_bMirAxisX = m_bMirAxisXYZ;
		if(mirmsg.axis_flag&2)	//允许Y对称
			m_bMirAxisY = m_bMirAxisXYZ;
		if(mirmsg.axis_flag&4)	//允许Z对称
			m_bMirAxisZ = m_bMirAxisXYZ;
		if(xy1_yz2_xz3==1)		//XY对称互斥
			m_bMirAxisY=FALSE;
		else if(xy1_yz2_xz3==2)	//YZ对称互斥
			m_bMirAxisY=FALSE;
		else if(xy1_yz2_xz3==3)	//XZ对称互斥
			m_bMirAxisX=FALSE;
	}
	else
		m_bMirAxisX=m_bMirAxisY=m_bMirAxisZ=FALSE;
	UpdateData(FALSE);
}

void CLinkTsaPoleDlg::OnMirAxisX() 
{
	UpdateData();
	if(xy1_yz2_xz3==1)	//XY对称互斥
	{
		if(m_bMirAxisY)
			m_bMirAxisY=!m_bMirAxisX;
	}
	else if(xy1_yz2_xz3==3)	//XZ对称互斥
	{
		if(m_bMirAxisZ)
			m_bMirAxisZ=!m_bMirAxisX;
	}
	UpdateData(FALSE);	
}

void CLinkTsaPoleDlg::OnMirAxisY() 
{
	UpdateData();
	if(xy1_yz2_xz3==2)	//YZ对称互斥
	{
		if(m_bMirAxisZ)
			m_bMirAxisZ=!m_bMirAxisY;
	}
	else if(xy1_yz2_xz3==1)	//XY对称互斥
	{
		if(m_bMirAxisX)
			m_bMirAxisX=!m_bMirAxisY;
	}
	UpdateData(FALSE);
}

void CLinkTsaPoleDlg::OnMirAxisZ() 
{
	UpdateData();
	if(xy1_yz2_xz3==2)	//YZ对称互斥
	{
		if(m_bMirAxisY)
			m_bMirAxisY=!m_bMirAxisZ;
	}
	else if(xy1_yz2_xz3==3)	//XZ对称互斥
	{
		if(m_bMirAxisX)
			m_bMirAxisX=!m_bMirAxisZ;
	}
	UpdateData(FALSE);
}

void CLinkTsaPoleDlg::OnOK() 
{	
	UpdateData();
	mirmsg.origin.Set();
	mirmsg.axis_flag = 0;
	if(m_bMirAxisX)
		mirmsg.axis_flag|=1;
	if(m_bMirAxisY)
		mirmsg.axis_flag|=2;
	if(m_bMirAxisZ)
		mirmsg.axis_flag|=4;
	if(m_bMirSideSpace)
		mirmsg.axis_flag|=8;
	mirmsg.array_num = 1;
	mirmsg.rotate_angle = 90;
	CDialog::OnOK();
}
#endif
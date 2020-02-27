// DefOffsetNodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DefOffsetNodeDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefOffsetNodeDlg dialog


CDefOffsetNodeDlg::CDefOffsetNodeDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CDefOffsetNodeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDefOffsetNodeDlg)
	m_fOriginX = 0.0;
	m_fOriginY = 0.0;
	m_fOriginZ = 0.0;
	m_nArrayNum = 1;
	m_bMirAxisX = FALSE;
	m_bMirAxisY = FALSE;
	m_bMirAxisZ = FALSE;
	m_bMirRotation = FALSE;
	m_fUpOffset = 0.0;
	m_fDownOffset = 0.0;
	m_bMirAxisXYZ = FALSE;
	m_nRotateAngle = 90;
	m_bAdvancedOption = FALSE;
	m_iOffsetVec = 0;
	m_pDatumPart = NULL;
	m_pPrevDatumPart = NULL;
	m_b45MirrorsSymmetry=FALSE;
	m_b135MirrorSymmetry=FALSE;
	m_bMultipleOffset=FALSE;
	//}}AFX_DATA_INIT
}


void CDefOffsetNodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefOffsetNodeDlg)
	DDX_Text(pDX, IDC_E_ORIGIN_X, m_fOriginX);
	DDX_Text(pDX, IDC_E_ORIGIN_Y, m_fOriginY);
	DDX_Text(pDX, IDC_E_ORIGIN_Z, m_fOriginZ);
	DDX_Text(pDX, IDC_E_ARRAY_NUM, m_nArrayNum);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_X, m_bMirAxisX);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Y, m_bMirAxisY);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Z, m_bMirAxisZ);
	DDX_Check(pDX, IDC_CHK_MIR_ROTATE, m_bMirRotation);
	DDX_Text(pDX, IDC_ABOVE_OFFSET, m_fUpOffset);
	DDX_Text(pDX, IDC_UNDER_OFFSET, m_fDownOffset);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_XYZ, m_bMirAxisXYZ);
	DDX_Text(pDX, IDC_E_ROTATE_ANGLE, m_nRotateAngle);
	DDV_MinMaxInt(pDX, m_nRotateAngle, -360, 360);
	DDX_Check(pDX, IDC_CHK_ADVANCED_OPTION, m_bAdvancedOption);
	DDX_CBIndex(pDX, IDC_CMB_OFFSET_VEC, m_iOffsetVec);
	DDX_Check(pDX, IDC_CHK_45_MIR,m_b45MirrorsSymmetry);
	DDX_Check(pDX,IDC_CHK_135_MIR,m_b135MirrorSymmetry);
	DDX_Check(pDX, IDC_CHK_CONTINUE_OFFSET, m_bMultipleOffset);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefOffsetNodeDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CDefOffsetNodeDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN, OnBnPasteOrigin)
	ON_BN_CLICKED(IDC_CHK_MIR_ROTATE, OnChkMirRotate)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_XYZ, OnMirAxisXyz)
	ON_BN_CLICKED(IDC_CHK_ADVANCED_OPTION, OnChkAdvancedOption)
	ON_BN_CLICKED(IDC_BTN_OK, OnOK)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnCancel)
	ON_BN_CLICKED(IDC_CHK_45_MIR, OnChkMir45Plane)
	ON_BN_CLICKED(IDC_CHK_135_MIR, OnChkMir135Plane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefOffsetNodeDlg message handlers

BOOL CDefOffsetNodeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//m_iOffsetVec=0;	//默认沿长度方向偏移
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
	if(!m_bUpOffset)
		m_fUpOffset=0.0;
	if(!m_bDownOffset)
		m_fDownOffset=0.0;
	GetDlgItem(IDC_ABOVE_OFFSET)->EnableWindow(m_bUpOffset);
	GetDlgItem(IDC_UNDER_OFFSET)->EnableWindow(m_bDownOffset);
	GetDlgItem(IDC_E_ROTATE_ANGLE)->EnableWindow(m_bMirRotation);
	GetDlgItem(IDC_E_ARRAY_NUM)->EnableWindow(m_bMirRotation);
	GetDlgItem(IDC_WORK_PANEL)->ShowWindow(SW_HIDE);
	if(m_bAdvancedOption==FALSE)	//对话框默认为展开，不启用高级设置需调整对话框
		AdjustWndPosition();
	UpdateData(FALSE);
	//移动对话框到左上角
	MoveWndToLocation();
	return TRUE;
}

void CDefOffsetNodeDlg::OnChkMirRotate() 
{
	UpdateData();
	GetDlgItem(IDC_E_ROTATE_ANGLE)->EnableWindow(m_bMirRotation);
	GetDlgItem(IDC_E_ARRAY_NUM)->EnableWindow(m_bMirRotation);
	UpdateData(FALSE);
}

void CDefOffsetNodeDlg::OnBnPasteOrigin() 
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

void CDefOffsetNodeDlg::OnOK() 
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
	if(m_b45MirrorsSymmetry||m_b135MirrorSymmetry)
	{
		mirmsg.axis_flag|=0x10;
		if(m_b45MirrorsSymmetry)
			mirmsg.mir_norm.Set(1,-1,0);
		else if(m_b135MirrorSymmetry)
			mirmsg.mir_norm.Set(1,1,0);
	}
	else
		mirmsg.mir_norm.Set(0,0,1);
	mirmsg.array_num = m_nArrayNum;
	mirmsg.rotate_angle = m_nRotateAngle;
}

void CDefOffsetNodeDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CDefOffsetNodeDlg::OnMirAxisXyz() 
{
	UpdateData();
	m_bMirAxisX=m_bMirAxisY=m_bMirAxisZ=m_bMirAxisXYZ;
	UpdateData(FALSE);
}

void CDefOffsetNodeDlg::OnChkMir45Plane()
{
	UpdateData();
	if(m_b45MirrorsSymmetry)
		m_b135MirrorSymmetry=FALSE;
	UpdateData(FALSE);
}
void CDefOffsetNodeDlg::OnChkMir135Plane()
{
	UpdateData();
	if(m_b135MirrorSymmetry)
		m_b45MirrorsSymmetry=FALSE;
	UpdateData(FALSE);
}
void CDefOffsetNodeDlg::OnChkAdvancedOption() 
{
	UpdateData();
	AdjustWndPosition();
	UpdateData(FALSE);
}

void CDefOffsetNodeDlg::AdjustWndPosition()
{
	CRect panel_rc;
	GetDlgItem(IDC_WORK_PANEL)->GetWindowRect(panel_rc);
	long nOffset=0;
	if(m_bAdvancedOption)	//启用高级设置
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
	else		//关闭高级设置
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
void CDefOffsetNodeDlg::InitOffsetType(CLDSPart *pDatumPart)
{
	m_pDatumPart=pDatumPart;
	if(m_pDatumPart==NULL)
		return;
	if((m_pDatumPart&&m_pPrevDatumPart==NULL)||
		toupper(m_pPrevDatumPart->Layer(0))!=toupper(m_pDatumPart->Layer(0))||
		toupper(m_pPrevDatumPart->Layer(1))!=toupper(m_pDatumPart->Layer(1)))
	{	//初始化偏移类型
		if(m_pDatumPart->IsLinePart())
		{
			if(toupper(m_pDatumPart->Layer(0))=='S'&&toupper(m_pDatumPart->Layer(1))=='Z')
			{
				CLDSLinePart* pRod=(CLDSLinePart*)m_pDatumPart;
				GEPOINT len_vec=pRod->End()-pRod->Start();
				normalize(len_vec);
				if(fabs(len_vec*GEPOINT(0,0,1))<0.5)	//cos60度=0.5
					m_iOffsetVec=0;	//虽为塔身主材但与Z方向角度超过60度，默认为按长度偏移
				else
					m_iOffsetVec=3;	//塔身主材上偏移点，默认为Z方向偏移
			}
			else
				m_iOffsetVec=0;	//其余默认为长度方向偏移
		}
	}
	//else 保留前一次默认值
	m_pPrevDatumPart=m_pDatumPart;
}
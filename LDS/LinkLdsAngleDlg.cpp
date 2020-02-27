// LinkTubeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LinkLdsAngleDlg.h"
#include "GroupJgStyleDlg.h"
#include "Query.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkLdsAngleDlg dialog

CLinkLdsAngleDlg::CLinkLdsAngleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLinkLdsAngleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkLdsAngleDlg)
	m_nJgMaterial = 0;
	m_nStartLsNum = 2;
	m_nEndLsNum = 2;
	m_sEndLsGuiGe = _T("20");
	m_sStartLsGuiGe = _T("20");
	m_bMirSideFace = FALSE;
	m_sOriginPartNo = _T("");
	m_sMirXOriginPartNo = _T("");
	m_sMirYOriginPartNo = _T("");
	m_sMirDiagOriginPartNo = _T("");
	m_sJgGuiGe = _T("100X10");
	m_bGroupJg = FALSE;
	m_bStartHoriOffset = FALSE;
	m_bEndHoriOffset = FALSE;
	m_bStartToBer = FALSE;
	m_bEndToBer = FALSE;
	m_iSegNo = 0;
	m_iAngleGradeType = 0;
	m_iAngleLocation = 1;
	m_uStatMatNo = 0;
	m_bSelSize = TRUE;
	m_bAnalysisInc = TRUE;
	m_iStartForceType = 1;
	m_iStartJointType = 2;
	m_iEndForceType = 1;
	m_iEndJointType = 2;
	m_iCalLenType = 0;
	m_fLsHoleOut = 0.0;
	m_bMirAxisXYZ = FALSE;
	m_bMirAxisX = FALSE;
	m_bMirAxisY = FALSE;
	m_bMirAxisZ = FALSE;
	//}}AFX_DATA_INIT
	group_style=0;
	memset(sPartNoArr,0,4*4*16);
	m_fFillThick=12.0;
	mirmsg.axis_flag = 0; 
	xy1_yz2_xz3 = 0;
	m_pStartNode = NULL;
	m_pEndNode = NULL;
}


void CLinkLdsAngleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkLdsAngleDlg)
	DDX_CBIndex(pDX, IDC_CMB_JG_MATERIAL, m_nJgMaterial);
	DDX_Text(pDX, IDC_EDIT_START_LS_NUM, m_nStartLsNum);
	DDX_Text(pDX, IDC_EDIT_END_LS_NUM, m_nEndLsNum);
	DDX_CBString(pDX, IDC_CMB_END_LS_GUIEGE, m_sEndLsGuiGe);
	DDX_CBString(pDX, IDC_CMB_START_LS_GUIEGE, m_sStartLsGuiGe);
	DDX_Check(pDX, IDC_CHK_MIR_SIDE_FACE, m_bMirSideFace);
	DDX_Text(pDX, IDC_E_ORIGIN_PART_NO, m_sOriginPartNo);
	DDV_MaxChars(pDX, m_sOriginPartNo, 15);
	DDX_Text(pDX, IDC_E_MIR_X_ORIGIN_PART_NO, m_sMirXOriginPartNo);
	DDV_MaxChars(pDX, m_sMirXOriginPartNo, 15);
	DDX_Text(pDX, IDC_E_MIR_Y_ORIGIN_PART_NO, m_sMirYOriginPartNo);
	DDV_MaxChars(pDX, m_sMirYOriginPartNo, 15);
	DDX_Text(pDX, IDC_E_MIR_DIAG_ORIGIN_PART_NO, m_sMirDiagOriginPartNo);
	DDV_MaxChars(pDX, m_sMirDiagOriginPartNo, 15);
	DDX_CBString(pDX, IDC_CMB_JG_GUIEGE, m_sJgGuiGe);
	DDX_Check(pDX, IDC_CHK_GROUP_JG, m_bGroupJg);
	DDX_Check(pDX, IDC_CHK_START_HORI_OFFSET, m_bStartHoriOffset);
	DDX_Check(pDX, IDC_CHK_END_HORI_OFFSET, m_bEndHoriOffset);
	DDX_Check(pDX, IDC_CHK_START_TO_BER, m_bStartToBer);
	DDX_Check(pDX, IDC_CHK_END_TO_BER, m_bEndToBer);
	DDX_Text(pDX, IDC_E_SEG_NO, m_iSegNo);
	DDX_Radio(pDX, IDC_ANGLE_GRADE_TYPE, m_iAngleGradeType);
	DDX_Radio(pDX, IDC_HEAD, m_iAngleLocation);
	DDX_Text(pDX, IDC_E_STAT_MAT_NO, m_uStatMatNo);
	DDX_Check(pDX, IDC_CHK_IS_SELSIZE, m_bSelSize);
	DDX_Check(pDX, IDC_CHK_IS_ANALYSIS_INC, m_bAnalysisInc);
	DDX_CBIndex(pDX, IDC_CMB_START_FORCE_TYPE, m_iStartForceType);
	DDX_CBIndex(pDX, IDC_CMB_START_JOINT_TYPE, m_iStartJointType);
	DDX_CBIndex(pDX, IDC_CMB_END_FORCE_TYPE, m_iEndForceType);
	DDX_CBIndex(pDX, IDC_CMB_END_JOINT_TYPE, m_iEndJointType);
	DDX_CBIndex(pDX, IDC_CMB_CAL_LEN_TYPE, m_iCalLenType);
	DDX_Text(pDX, IDC_E_LS_HOLE_OUT, m_fLsHoleOut);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_XYZ, m_bMirAxisXYZ);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_X, m_bMirAxisX);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Y, m_bMirAxisY);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Z, m_bMirAxisZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinkLdsAngleDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkLdsAngleDlg)
	ON_WM_VSCROLL()
	ON_CBN_SELCHANGE(IDC_CMB_JG_GUIEGE, OnSelchangeCmbJgGuiege)
	ON_EN_CHANGE(IDC_E_ORIGIN_PART_NO, OnChangeEOriginPartNo)
	ON_EN_CHANGE(IDC_EDIT_START_LS_NUM, OnChangeEditStartLsNum)
	ON_EN_CHANGE(IDC_EDIT_END_LS_NUM, OnChangeEditEndLsNum)
	ON_CBN_EDITCHANGE(IDC_CMB_START_LS_GUIEGE, OnChangeCmbStartLsGuiege)
	ON_EN_CHANGE(IDC_E_MIR_X_ORIGIN_PART_NO, OnChangeEMirXOriginPartNo)
	ON_BN_CLICKED(IDC_BN_GROUP_STYLE, OnBnGroupStyle)
	ON_BN_CLICKED(IDC_CHK_GROUP_JG, OnChkGroupJg)
	ON_BN_CLICKED(IDC_BN_NEW_STAT_MAT_NO, OnBnNewStatMatNo)
	ON_CBN_SELCHANGE(IDC_CMB_START_FORCE_TYPE, OnSelchangeCmbStartForceType)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_XYZ, OnMirAxisXyz)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_X, OnMirAxisX)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Y, OnMirAxisY)
	ON_CBN_SELCHANGE(IDC_CMB_START_LS_GUIEGE, OnChangeCmbStartLsGuiege)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Z, OnMirAxisZ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkLdsAngleDlg message handlers

BOOL CLinkLdsAngleDlg::OnInitDialog() 
{
	if(m_pStartNode == NULL || m_pEndNode == NULL)
		return FALSE;
	if(m_bGroupJg)
		GetDlgItem(IDC_BN_GROUP_STYLE)->ShowWindow(SW_SHOW);
	else
		GetDlgItem(IDC_BN_GROUP_STYLE)->ShowWindow(SW_HIDE);
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_EDIT_START_LS_NUM); 
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_S);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(1, 50);
	pSpin->SetPos(m_nStartLsNum);
	pEdit = (CEdit*) GetDlgItem(IDC_EDIT_END_LS_NUM); 
	pSpin =	(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_E);
	pSpin->SetBuddy(pEdit);
	pSpin->SetRange(1, 50);
	pSpin->SetPos(m_nEndLsNum);

	CComboBox* pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_JG_GUIEGE);
	AddJgRecord(pCMBJg);
	CComboBox* pCMBMate = (CComboBox*)GetDlgItem(IDC_CMB_JG_MATERIAL);
	AddSteelMatRecord(pCMBMate);
	xy1_yz2_xz3=0;	
	mirmsg=GetPoleValidMirMsg(m_pStartNode,m_pEndNode,&xy1_yz2_xz3);
	if(!(mirmsg.axis_flag&1))
	{	//不能进行X对称
		m_bMirAxisX=FALSE;
		GetDlgItem(IDC_CHK_MIR_AXIS_X)->EnableWindow(FALSE);
	}
	else if(!(mirmsg.axis_flag&2))
	{	//不能进行Y对称
		m_bMirAxisY=FALSE;
		GetDlgItem(IDC_CHK_MIR_AXIS_Y)->EnableWindow(FALSE);
	}
	else if(!(mirmsg.axis_flag&4))
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
	else
		m_bMirAxisX=m_bMirAxisY=m_bMirAxisZ=FALSE;
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	return CDialog::OnInitDialog();
}

void CLinkLdsAngleDlg::OnSelchangeCmbJgGuiege() 
{
	CString sText;
	double wing_wide,wing_thick;
	CComboBox* pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_JG_GUIEGE);
	int iCurSel = pCMBJg->GetCurSel();
	pCMBJg->GetLBText(iCurSel,sText);
	restore_JG_guige(sText,wing_wide,wing_thick);
	CComboBox* pStartCMBLs = (CComboBox*)GetDlgItem(IDC_CMB_START_LS_GUIEGE);
	CComboBox* pEndCMBLs = (CComboBox*)GetDlgItem(IDC_CMB_END_LS_GUIEGE);
	if(wing_wide<63)
	{
		m_sStartLsGuiGe = "16";
		m_sEndLsGuiGe = "16";
		pStartCMBLs->SetWindowText("16");
		pEndCMBLs->SetWindowText("16");
	}
	else if(wing_wide<140)
	{
		m_sStartLsGuiGe = "20";
		m_sEndLsGuiGe = "20";
		pStartCMBLs->SetWindowText("20");
		pEndCMBLs->SetWindowText("20");
	}
	else
	{
		m_sStartLsGuiGe = "24";
		m_sEndLsGuiGe = "24";
		pStartCMBLs->SetWindowText("24");
		pEndCMBLs->SetWindowText("24");
	}
}

void CLinkLdsAngleDlg::OnChangeCmbStartLsGuiege() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_START_LS_GUIEGE);
	UpdateData();
	int iCurSel = pCMB->GetCurSel();
	pCMB->GetLBText(iCurSel,m_sStartLsGuiGe);
	m_sEndLsGuiGe = m_sStartLsGuiGe;
	UpdateData(FALSE);
}

void CLinkLdsAngleDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	
    if (nSBCode == SB_ENDSCROLL) {
        return; // Reject spurious messages
    }
    // Process scroll messages from IDC_SPIN only
    if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_S ||
		pScrollBar->GetDlgCtrlID() == IDC_SPIN_E )
	{
        CString strValue;
        strValue.Format("%d", nPos);
        ((CSpinButtonCtrl*) pScrollBar)->GetBuddy()
                                       ->SetWindowText(strValue);
    }
}

void CLinkLdsAngleDlg::OnChangeEditStartLsNum() 
{
	UpdateData();	
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_S);
	pSpin->SetPos(m_nStartLsNum);
	m_nEndLsNum = m_nStartLsNum;
	pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_E);
	pSpin->SetPos(m_nEndLsNum);
	UpdateData(FALSE);
}

void CLinkLdsAngleDlg::OnChangeEditEndLsNum() 
{
	UpdateData();	
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_E);
	pSpin->SetPos(m_nEndLsNum);
}

void CLinkLdsAngleDlg::OnOK() 
{
	UpdateData();
	CComboBox* pCMBJgM = (CComboBox*)GetDlgItem(IDC_CMB_JG_MATERIAL);
	//是否可以对称的信息
	mirmsg.origin.Set();
	mirmsg.axis_flag = 0;
	if(m_bMirAxisX)
		mirmsg.axis_flag|=1;
	if(m_bMirAxisY)
		mirmsg.axis_flag|=2;
	if(m_bMirAxisZ)
		mirmsg.axis_flag|=4;
	if(m_bMirSideFace)
		mirmsg.axis_flag|=8;
	mirmsg.array_num = 1;
	mirmsg.rotate_angle = -90;
	CDialog::OnOK();
}

void CLinkLdsAngleDlg::OnChkGroupJg() 
{
	UpdateData();
	if(m_bGroupJg)
	{
		GetDlgItem(IDC_BN_GROUP_STYLE)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_E_ORIGIN_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_START_TO_BER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_END_TO_BER)->EnableWindow(FALSE);
		m_bStartToBer=m_bEndToBer=TRUE;
	}
	else
	{
		GetDlgItem(IDC_BN_GROUP_STYLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_ORIGIN_PART_NO)->EnableWindow (TRUE);
		GetDlgItem(IDC_CHK_START_TO_BER)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_END_TO_BER)->EnableWindow(TRUE);
		
		GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
		GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
		GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	}
}

void CLinkLdsAngleDlg::OnChangeEOriginPartNo() 
{
	UpdateData();
	CString sNo=m_sOriginPartNo;
	if(m_sOriginPartNo.GetLength()>15)
	{
		AfxMessageBox("原始构件编号太长!");
		m_sOriginPartNo = sNo;
		return;
	}
	m_sMirDiagOriginPartNo = m_sOriginPartNo;
	if(m_sMirXOriginPartNo.GetLength()!=m_sOriginPartNo.GetLength())
		m_sMirXOriginPartNo=m_sMirYOriginPartNo=m_sOriginPartNo;
	for(int i=0;i<m_sOriginPartNo.GetLength();i++)
	{
		if(isdigit(m_sOriginPartNo[i]))
		{
			m_sMirXOriginPartNo.SetAt(i,m_sOriginPartNo[i]);
			m_sMirYOriginPartNo=m_sMirXOriginPartNo;
		}
	}
	int iSeg=0;
	if(ParsePartNo(sNo,iSeg,NULL))
		m_iSegNo=iSeg;
	UpdateData(FALSE);
}

void CLinkLdsAngleDlg::OnChangeEMirXOriginPartNo() 
{
	CString sNo=m_sMirXOriginPartNo;
	UpdateData();
	if(m_sOriginPartNo.GetLength()>15)
	{
		AfxMessageBox("原始构件编号太长!");
		m_sMirXOriginPartNo = sNo;
		return;
	}
	m_sMirYOriginPartNo=m_sMirXOriginPartNo;	
	UpdateData(FALSE);
}

void CLinkLdsAngleDlg::OnBnGroupStyle() 
{
	CGroupJgStyleDlg group_jg_style_dlg;
	group_jg_style_dlg.m_fFillThick=m_fFillThick;
	group_jg_style_dlg.m_iJgGroupStyle=group_style;
	if(group_jg_style_dlg.DoModal()==IDOK)
	{
		group_style=group_jg_style_dlg.m_iJgGroupStyle;
		m_fFillThick=group_jg_style_dlg.m_fFillThick;
	}
}

void CLinkLdsAngleDlg::OnBnNewStatMatNo() 
{
#ifdef __LDS_
	m_uStatMatNo=Ta.GetNewStatNo();
	UpdateData(FALSE);
#endif
}

void CLinkLdsAngleDlg::OnSelchangeCmbStartForceType() 
{
	UpdateData();
	m_iEndForceType=m_iStartForceType;
	UpdateData(FALSE);
}

void CLinkLdsAngleDlg::OnMirAxisXyz() 
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
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);
}

void CLinkLdsAngleDlg::OnMirAxisX() 
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
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);
}

void CLinkLdsAngleDlg::OnMirAxisY() 
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
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);	
}

void CLinkLdsAngleDlg::OnMirAxisZ() 
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
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);
}

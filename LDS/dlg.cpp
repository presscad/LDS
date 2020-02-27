//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "GroupJgStyleDlg.h"
#include "dlg.h"
#include "InputJgGDlg.h"
#include "DataCompare.h"
#include "LocalFeatureDef.h"
#include "LicFuncDef.h"

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CConfirmJgStyleDlg dialog
CConfirmJgStyleDlg::CConfirmJgStyleDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CConfirmJgStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmJgStyleDlg)
	m_nJgStyle = 1;
	//}}AFX_DATA_INIT
}

void CConfirmJgStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmJgStyleDlg)
	DDX_Radio(pDX, IDC_RDO_JG_STYLE, m_nJgStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmJgStyleDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CConfirmJgStyleDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmJgStyleDlg message handlers
BOOL CConfirmJgStyleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	MoveWndToLocation();
	return TRUE;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkJGDlg dialog
CHashList<char> CLinkJGDlg::hashPreferAngleMat;	//根据规格标识(如25020)判断的优先角钢材质简化字符
CLinkJGDlg::CLinkJGDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CLinkJGDlg::IDD, pParent)
	, m_nWingG(0)
	, m_iJgStyle(0)
	, m_bConnect1Wing(TRUE)
	, m_bConnect2Wing(FALSE)
	, m_iMirLayStyle(0)
{
	//{{AFX_DATA_INIT(CLinkJGDlg)
	m_bAutoMatchMat=TRUE;
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
	m_sSegNo = _T("");
	m_iAngleGradeType = 0;
	m_iAngleLocation = 1;
	m_bMirAxisX = FALSE;
	m_bMirAxisXYZ = FALSE;
	m_bMirAxisY = FALSE;
	m_bMirAxisZ = FALSE;
	//}}AFX_DATA_INIT
	wSectStyle=ciMasterIndex=0;
	memset(sPartNoArr,0,4*4*16);
	m_fFillThick=12.0;
	mirmsg.axis_flag = 0; 
	xy1_yz2_xz3 = 0;
	m_pStartNode = NULL;
	m_pEndNode = NULL;
	int d=SelBoltDByWingWide(100,m_sBoltGrade);
	m_sStartLsGuiGe.Format("%d",d);
	m_sEndLsGuiGe=m_sStartLsGuiGe;
	//strcpy(m_sBoltGrade,"");
}


void CLinkJGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkJGDlg)
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
	DDX_Check(pDX, IDC_CHK_START_TO_BER, m_bStartToBer);
	DDX_Check(pDX, IDC_CHK_END_TO_BER, m_bEndToBer);
	DDX_Text(pDX, IDC_E_SEG_NO, m_sSegNo);
	DDX_Radio(pDX, IDC_ANGLE_GRADE_TYPE, m_iAngleGradeType);
	DDX_Radio(pDX, IDC_HEAD, m_iAngleLocation);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_X, m_bMirAxisX);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_XYZ, m_bMirAxisXYZ);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Y, m_bMirAxisY);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Z, m_bMirAxisZ);
	DDX_Text(pDX, IDC_E_WING_G, m_nWingG);
	DDX_Radio(pDX, IDC_RDO_JG_STYLE, m_iJgStyle);
	DDX_Check(pDX, IDC_CHK_1WING_BOLT_N, m_bConnect1Wing);
	DDX_Check(pDX, IDC_CHK_2WING_BOLT_N, m_bConnect2Wing);
	DDX_Radio(pDX, IDC_RDO_MIR_LAY_STYLE, m_iMirLayStyle);
	DDX_Check(pDX, IDC_CHK_AUTO_MATCH_MATERIAL, m_bAutoMatchMat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinkJGDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CLinkJGDlg)
	ON_WM_VSCROLL()
	ON_CBN_SELCHANGE(IDC_CMB_JG_GUIEGE, OnSelchangeCmbJgGuiege)
	ON_CBN_EDITCHANGE(IDC_CMB_JG_GUIEGE,&CLinkJGDlg::OnEditChangeCmbJgGuiege)
	ON_EN_CHANGE(IDC_E_ORIGIN_PART_NO, OnChangeEOriginPartNo)
	ON_EN_CHANGE(IDC_EDIT_START_LS_NUM, OnChangeEditStartLsNum)
	ON_EN_CHANGE(IDC_EDIT_END_LS_NUM, OnChangeEditEndLsNum)
	ON_CBN_EDITCHANGE(IDC_CMB_START_LS_GUIEGE, OnChangeCmbStartLsGuiege)
	ON_EN_CHANGE(IDC_E_MIR_X_ORIGIN_PART_NO, OnChangeEMirXOriginPartNo)
	ON_BN_CLICKED(IDC_BN_GROUP_STYLE, OnBnGroupStyle)
	ON_BN_CLICKED(IDC_CHK_GROUP_JG, OnChkGroupJg)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_XYZ, OnMirAxisXyz)
	ON_CBN_SELCHANGE(IDC_CMB_START_LS_GUIEGE, OnChangeCmbStartLsGuiege)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_X, OnMirAxisX)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Y, OnMirAxisY)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Z, OnMirAxisZ)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_HEAD, &CLinkJGDlg::OnBnClickedJgPlaceStyle)
	ON_BN_CLICKED(IDC_SHANK, &CLinkJGDlg::OnBnClickedJgPlaceStyle)
	ON_BN_CLICKED(IDC_LEG, &CLinkJGDlg::OnBnClickedJgPlaceStyle)
	ON_BN_CLICKED(IDC_ANGLE_GRADE_TYPE, &CLinkJGDlg::OnBnClickedJgPlaceStyle)
	ON_BN_CLICKED(IDC_RADIO2, &CLinkJGDlg::OnBnClickedJgPlaceStyle)
	ON_BN_CLICKED(IDC_RADIO3, &CLinkJGDlg::OnBnClickedJgPlaceStyle)
	ON_BN_CLICKED(IDC_RADIO4, &CLinkJGDlg::OnBnClickedJgPlaceStyle)
	ON_BN_CLICKED(IDC_CHK_1WING_BOLT_N, &CLinkJGDlg::OnBnClickedChk1WingBoltN)
	ON_BN_CLICKED(IDC_CHK_2WING_BOLT_N, &CLinkJGDlg::OnBnClickedChk2WingBoltN)
	ON_EN_CHANGE(IDC_E_SEG_NO, &CLinkJGDlg::OnEnChangeESegNo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkJGDlg message handlers
void CLinkJGDlg::AddPreferAngleMat(DWORD nSpec,char cMaterial)
{
	char *pMatMark=hashPreferAngleMat.GetValue(nSpec);
	if(pMatMark==NULL)
		hashPreferAngleMat.SetValue(nSpec,cMaterial);
	else if(*pMatMark==cMaterial)
		return;
	else	//同一种角钢规格有两种或两种以上的不同材质类型
		hashPreferAngleMat.SetValue(nSpec,0);
}
void CLinkJGDlg::InitPreferAngleMat(IModel* pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		hashPreferAngleMat.Empty();
		return;
	}
	for(CLDSLineAngle* pAngle=(CLDSLineAngle*)pModel->EnumPartFirst(CLS_LINEANGLE);pAngle;
		pAngle=(CLDSLineAngle*)pModel->EnumPartNext(CLS_LINEANGLE))
	{
		DWORD nSpec=ftoi(pAngle->size_wide)*100+ftoi(pAngle->size_thick);
		AddPreferAngleMat(nSpec,pAngle->cMaterial);
	}
}
int CLinkJGDlg::SelectPreferAngleMatIndex(DWORD nSpec,int iMaterial)
{
	char *pMatMark=hashPreferAngleMat.GetValue(nSpec);
	if(pMatMark!=NULL && *pMatMark>0)
	{
		for(int i=0;i<CSteelMatLibrary::GetCount();i++)
		{
			if(*pMatMark==CSteelMatLibrary::RecordAt(i).cBriefMark)
				return i;
		}
	}
	return iMaterial;
}
BOOL CLinkJGDlg::OnInitDialog() 
{
	if(m_pStartNode == NULL|| m_pEndNode == NULL)
		return FALSE;
	m_bDisconnectSegIAndPartNo=FALSE;
	CWnd* pWnd = GetDlgItem(IDC_CHK_MIR_SIDE_FACE);
	if(m_bGroupJg)
		GetDlgItem(IDC_BN_GROUP_STYLE)->ShowWindow(SW_SHOW);
	else
		GetDlgItem(IDC_BN_GROUP_STYLE)->ShowWindow(SW_HIDE);
	m_bConnect2Wing=!m_bConnect1Wing;
	xy1_yz2_xz3=0;	
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
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);

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

	double wing_wide,wing_thick;
	restore_JG_guige(m_sJgGuiGe.GetString(),wing_wide,wing_thick);
	JGZJ jgzj;
	getjgzj(jgzj,wing_wide);
	m_nWingG = jgzj.g;
	m_iJgStyle=CalJgPosStyle(m_pStartNode,m_pEndNode,GetLayer(),m_iJgStyle);
	int nSpec=ftoi(wing_wide)*100+ftoi(wing_thick);
	m_nJgMaterial=SelectPreferAngleMatIndex(nSpec,m_nJgMaterial);
	//
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_START_LS_GUIEGE);
	AddLsRecord(pCMB,0,TRUE);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_END_LS_GUIEGE);
	AddLsRecord(pCMB,0,TRUE);
	MoveWndToLocation();
	BOOL bRetCode=CDialog::OnInitDialog();
#if defined(__TMA_)||defined(__LMA_)
	//If OnInitDialog returns nonzero, Windows sets the input focus to the first control in the dialog box.
	//The application can return 0 only if it has explicitly set the input focus to one of the controls in the dialog box.
	if(g_sysPara.ciLiveFocusCtrlInLinkAngleDlg==2)//::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0)
	{	//1.规格框优先;2.构件编号优先
		GetDlgItem(IDC_E_ORIGIN_PART_NO)->SetFocus();	//启用数据对比时，件号为默认焦点 wht 18-12-04
		bRetCode=FALSE;
	}
#endif
	return bRetCode;
}

void CLinkJGDlg::OnSelchangeCmbJgGuiege()
{
	CString sText;
	CComboBox* pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_JG_GUIEGE);
	int iCurSel = pCMBJg->GetCurSel();
	pCMBJg->GetLBText(iCurSel,sText);
	FireCmbAngleSizeChanged(sText);
}

void CLinkJGDlg::OnEditChangeCmbJgGuiege()
{
	CString sText;
	CComboBox* pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_JG_GUIEGE);
	pCMBJg->GetWindowText(sText);
	FireCmbAngleSizeChanged(sText);
}
void CLinkJGDlg::FireCmbAngleSizeChanged(const char* szSizeSpec)
{
	double wing_wide=0,wing_thick=0;
	restore_JG_guige(szSizeSpec, wing_wide, wing_thick);
	if (wing_thick<3)
		return;
	CComboBox* pStartCMBLs = (CComboBox*)GetDlgItem(IDC_CMB_START_LS_GUIEGE);
	CComboBox* pEndCMBLs = (CComboBox*)GetDlgItem(IDC_CMB_END_LS_GUIEGE);
	int d = SelBoltDByWingWide(wing_wide, m_sBoltGrade);
	m_sStartLsGuiGe.Format("%d", d);
	m_sEndLsGuiGe = m_sStartLsGuiGe;
	pStartCMBLs->SetWindowText(m_sStartLsGuiGe);
	pEndCMBLs->SetWindowText(m_sEndLsGuiGe);
	JGZJ jgzj;
	getjgzj(jgzj, wing_wide);
	m_nWingG = jgzj.g;
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_E_WING_G);
	pEdit->SetWindowText(CXhChar50("%d", m_nWingG));
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_JG_MATERIAL);
	int nSpec = ftoi(wing_wide) * 100 + ftoi(wing_thick);
	/*
	<63的规格一般只用Q235,Q345以上的小角钢市场上很难买到,实际工程也会用大规格代料 wjh-2015.2.5
	根据宝鸡需求，添加自动匹配材质开关 wxc-2016.4.1
	*/
	if(wing_wide > 63)
	{
		if (m_bAutoMatchMat && hashPreferAngleMat.GetValue(nSpec))
			m_nJgMaterial = SelectPreferAngleMatIndex(nSpec, m_nJgMaterial);
		pCMB->SetCurSel(m_nJgMaterial);
	}
	else
		pCMB->SetCurSel(0);
}

void CLinkJGDlg::OnChangeCmbStartLsGuiege() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_START_LS_GUIEGE);
	UpdateData();
	int iCurSel = pCMB->GetCurSel();
	pCMB->GetLBText(iCurSel,m_sStartLsGuiGe);
	m_sEndLsGuiGe = m_sStartLsGuiGe;
	UpdateData(FALSE);
}

void CLinkJGDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CLinkJGDlg::OnChangeEditStartLsNum() 
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

void CLinkJGDlg::OnChangeEditEndLsNum() 
{
	UpdateData();	
	CSpinButtonCtrl* pSpin =
		(CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_E);
	pSpin->SetPos(m_nEndLsNum);
}

void CLinkJGDlg::OnOK() 
{
	UpdateData();
	double wing_wide,wing_thick;
	restore_JG_guige(m_sJgGuiGe.GetString(),wing_wide,wing_thick);
	STEELMAT steelMat=CSteelMatLibrary::RecordAt(m_nJgMaterial);
	DWORD nSpec=ftoi(wing_wide)*100+ftoi(wing_thick);
	AddPreferAngleMat(nSpec,steelMat.cBriefMark);
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
void CLinkJGDlg::OnMirAxisXyz() 
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

void CLinkJGDlg::OnMirAxisX() 
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
	if(!m_bMirAxisX)
		m_bMirAxisXYZ=FALSE;
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);
}

void CLinkJGDlg::OnMirAxisY() 
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
	if(!m_bMirAxisY)
		m_bMirAxisXYZ=FALSE;
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);	
}

void CLinkJGDlg::OnMirAxisZ() 
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
	if(!m_bMirAxisZ)
		m_bMirAxisXYZ=FALSE;
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow (m_bMirAxisX);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisY);
	GetDlgItem(IDC_E_MIR_DIAG_ORIGIN_PART_NO)->EnableWindow(m_bMirAxisZ);
	UpdateData(FALSE);
}

CXhChar16 CLinkJGDlg::GetLayer()
{
	CXhChar16 layer;
	switch(m_iAngleLocation)
	{
	case 0:
		layer[0] = 'T';
		break;
	case 1:
		layer[0] = 'S';
		break;
	case 2:
		layer[0] = 'L';
	default:
		break;
	}
	switch(m_iAngleGradeType)
	{
	case 0:
		layer[1] = 'Z';
		break;
	case 1:
		layer[1] = 'X';
		break;
	case 2:
		layer[1] = 'H';
		break;
	case 3:
		layer[1] = 'F';
		break;
	default:
		break;
	}
	layer[2]=theApp.env.layer[2];
	return layer;
}
void CLinkJGDlg::OnBnClickedJgPlaceStyle()
{
	UpdateData();
	m_iJgStyle=CalJgPosStyle(m_pStartNode,m_pEndNode,GetLayer(),m_iJgStyle);
#ifdef __SMART_DRAW_INC_
	if(m_iAngleGradeType==0)	//主材均默认为双肢连接
		m_bConnect1Wing=FALSE;
	else
		m_bConnect1Wing=TRUE;
#endif
	m_bConnect2Wing=!m_bConnect1Wing;
	m_iMirLayStyle=0;
	CXhChar16 layer=GetLayer();
	if((layer[1]=='X'&&layer[0]!='L')||(layer[0]=='S'&&layer[1]=='F'))
		m_iMirLayStyle=1;
	UpdateData(FALSE);
}

void CLinkJGDlg::OnBnClickedChk1WingBoltN()
{
	UpdateData();
	m_bConnect2Wing=!m_bConnect1Wing;
	UpdateData(FALSE);
}

void CLinkJGDlg::OnBnClickedChk2WingBoltN()
{
	UpdateData();
	m_bConnect1Wing=!m_bConnect2Wing;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// COffsetPtDlg dialog


COffsetPtDlg::COffsetPtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COffsetPtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COffsetPtDlg)
	m_fUpOffset = 0.0;
	m_fDownOffset = 0.0;
	m_nMirMsg = 4;
	m_bOffsetByAxisZ = FALSE;
	//}}AFX_DATA_INIT
	m_bUpOffset = TRUE;
	m_bDownOffset = TRUE;
}


void COffsetPtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COffsetPtDlg)
	DDX_Control(pDX, IDC_UNDER_OFFSET, m_editDown);
	DDX_Control(pDX, IDC_ABOVE_OFFST, m_editUp);
	DDX_Text(pDX, IDC_ABOVE_OFFST, m_fUpOffset);
	DDX_Text(pDX, IDC_UNDER_OFFSET, m_fDownOffset);
	DDX_CBIndex(pDX, IDC_CMB_MIR_MSG, m_nMirMsg);
	DDX_Check(pDX, IDC_CHK_OFFSET_BY_AXIS_Z, m_bOffsetByAxisZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COffsetPtDlg, CDialog)
	//{{AFX_MSG_MAP(COffsetPtDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COffsetPtDlg message handlers

BOOL COffsetPtDlg::OnInitDialog() 
{
	if(!m_bUpOffset)
		m_fUpOffset=0.0;
	if(!m_bDownOffset)
		m_fDownOffset=0.0;
	CDialog::OnInitDialog();
	m_editUp.EnableWindow(m_bUpOffset);
	m_editDown.EnableWindow(m_bDownOffset);
	CRect rect;
	GetWindowRect(&rect);
	MoveWindow(0,0,rect.Width(),rect.Height());	//移到左上角
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CInterJointLsDlg dialog


CInterJointLsDlg::CInterJointLsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInterJointLsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInterJointLsDlg)
	m_sLsGuiGe = _T("4.8M16X45");
	//}}AFX_DATA_INIT
}


void CInterJointLsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInterJointLsDlg)
	DDX_CBString(pDX, IDC_CMB_LS_DIAMETER, m_sLsGuiGe);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInterJointLsDlg, CDialog)
	//{{AFX_MSG_MAP(CInterJointLsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInterJointLsDlg message handlers

BOOL CInterJointLsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CSelJdbFaceNDlg dialog


CSelJdbFaceNDlg::CSelJdbFaceNDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelJdbFaceNDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelJdbFaceNDlg)
	m_iJdbFaceType = 0;
	//}}AFX_DATA_INIT
}


void CSelJdbFaceNDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelJdbFaceNDlg)
	DDX_Radio(pDX, IDC_JDB_FACE_TYPE, m_iJdbFaceType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelJdbFaceNDlg, CDialog)
	//{{AFX_MSG_MAP(CSelJdbFaceNDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelJdbFaceNDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CSpecLsDesignSetDlg dialog


CSpecLsDesignSetDlg::CSpecLsDesignSetDlg( CWnd* pParent /*=NULL*/)
	: CDialog(CSpecLsDesignSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecLsDesignSetDlg)
	m_nZhunJu1 = 0;
	m_nZhunJu2 = 0;
	m_nJointPos = 0;
	m_nZhunJu = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sBaseJgGuiGe = _T("Unknown");
#else 
	m_sBaseJgGuiGe = _T("未知");
#endif
	//}}AFX_DATA_INIT
}


void CSpecLsDesignSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecLsDesignSetDlg)
	DDX_Text(pDX, IDC_ZHUNJU1, m_nZhunJu1);
	DDX_Text(pDX, IDC_ZHUNJU2, m_nZhunJu2);
	DDX_Text(pDX, IDC_JOINT_POS, m_nJointPos);
	DDX_Text(pDX, IDC_ZHUNJU, m_nZhunJu);
	DDX_Text(pDX, IDC_S_BASE_JG_GUIGE, m_sBaseJgGuiGe);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecLsDesignSetDlg, CDialog)
	//{{AFX_MSG_MAP(CSpecLsDesignSetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindPartDlg dialog


CFindPartDlg::CFindPartDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CFindPartDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindPartDlg)
	m_nPartHandle = 0;
	m_bDisplayProperty = TRUE;
	m_sHexPartHandle = _T("");
	//}}AFX_DATA_INIT
	m_bDisplayPropertyVisible = TRUE;
}


void CFindPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindPartDlg)
	DDX_Text(pDX, IDC_E_PART_HANDLE, m_nPartHandle);
	DDX_Check(pDX, IDC_CHK_PROCESS, m_bDisplayProperty);
	DDX_Text(pDX, IDC_E_PART_HANDLE_HEX, m_sHexPartHandle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindPartDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CFindPartDlg)
	ON_EN_CHANGE(IDC_E_PART_HANDLE_HEX, OnSynchronizeDec)
	ON_EN_CHANGE(IDC_E_PART_HANDLE, OnSynchronizeHex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindPartDlg message handlers

BOOL CFindPartDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_CHK_PROCESS)->ShowWindow(SW_HIDE);
	MoveWndToLocation();
	return TRUE;
}

void CFindPartDlg::OnSynchronizeDec() 
{
	UpdateData(TRUE);
	sscanf(m_sHexPartHandle,"%X",&m_nPartHandle);
	UpdateData(FALSE);
}
void CFindPartDlg::OnSynchronizeHex() 
{
	UpdateData(TRUE);
	m_sHexPartHandle.Format("%X",m_nPartHandle);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CHorizonNodeDlg dialog


CHorizonNodeDlg::CHorizonNodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHorizonNodeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHorizonNodeDlg)
	m_nMirType = 4;
	//}}AFX_DATA_INIT
}


void CHorizonNodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHorizonNodeDlg)
	DDX_Radio(pDX, IDC_RDO_MIR_TYPE, m_nMirType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHorizonNodeDlg, CDialog)
	//{{AFX_MSG_MAP(CHorizonNodeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHorizonNodeDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CSpecViewDirectDlg dialog


CSpecViewDirectDlg::CSpecViewDirectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecViewDirectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecViewDirectDlg)
	m_nViewDirect = 0;
	m_nWingLayStyle = 0;
	//}}AFX_DATA_INIT
}


void CSpecViewDirectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecViewDirectDlg)
	DDX_Radio(pDX, IDC_RDO_VIEW_DIRECT, m_nViewDirect);
	DDX_Text(pDX, IDC_E_WING_LAY_STYLE, m_nWingLayStyle);
	DDV_MinMaxInt(pDX, m_nWingLayStyle, 1, 4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecViewDirectDlg, CDialog)
	//{{AFX_MSG_MAP(CSpecViewDirectDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecViewDirectDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CVFaceJdbDlg dialog


CVFaceJdbDlg::CVFaceJdbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVFaceJdbDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVFaceJdbDlg)
	m_nBaseLsN = 0;
	m_fZhunOffset = 60.0;
	m_nMaterial = 0;
	m_sBaseLsGuiGe = _T("4.8M16");
	m_nPlankThick = 8;
	m_nHuoQuVecStyle = 0;
	m_fHuoQuOffset = 0.0;
	m_iZhun0HuoQu1 = 0;
	m_sOriginNo = _T("");
	//}}AFX_DATA_INIT
}


void CVFaceJdbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVFaceJdbDlg)
	DDX_Text(pDX, IDC_E_BASE_LS_N, m_nBaseLsN);
	DDV_MinMaxInt(pDX, m_nBaseLsN, 1, 10);
	DDX_Text(pDX, IDC_E_ZHUN_OFFSET, m_fZhunOffset);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_nMaterial);
	DDX_CBString(pDX, IDC_CMB_BASE_LS_GUIGE, m_sBaseLsGuiGe);
	DDX_Text(pDX, IDC_E_PLANK_THICK, m_nPlankThick);
	DDX_Radio(pDX, IDC_RDO_HUOQU_STYLE, m_nHuoQuVecStyle);
	DDX_Text(pDX, IDC_E_HUOQU_OFFSET, m_fHuoQuOffset);
	DDX_Radio(pDX, IDC_RDO_ZHUN, m_iZhun0HuoQu1);
	DDX_Text(pDX, IDC_E_ORIGIN_NO, m_sOriginNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVFaceJdbDlg, CDialog)
	//{{AFX_MSG_MAP(CVFaceJdbDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVFaceJdbDlg message handlers

BOOL CVFaceJdbDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CSharePlankDesignDlg dialog


CSharePlankDesignDlg::CSharePlankDesignDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CSharePlankDesignDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSharePlankDesignDlg)
	m_nLsN = 0;
	m_nLsSpace = 0;
	m_fOddment = 0.0;
	m_sLsGuiGe = _T("20");
	//}}AFX_DATA_INIT
}


void CSharePlankDesignDlg::OnChangeCmbLsGuige() 
{
	LSSPACE_STRU LsSpace;
	CString sText;
//	UpdateData();
	CComboBox *pCmb = (CComboBox*)GetDlgItem(IDC_CMB_LS_GUIGE);
	int iCur = pCmb->GetCurSel();
	pCmb->GetLBText(iCur,m_sLsGuiGe);
	int d;
	char ss[12];
	sprintf(ss,"%s",m_sLsGuiGe);
	sscanf(m_sLsGuiGe,"%d",&d);
	//1.1获得基准螺栓间距表
	if(GetLsSpace(LsSpace, d))
	{
		m_nLsSpace = LsSpace.SingleRowSpace;
		m_fOddment=2*LsSpace.EndSpace;
		sText.Format("%d",m_nLsSpace);
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LS_SPACE);
		pEdit->SetWindowText(sText);
		sText.Format("%.0f",m_fOddment);
		pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ODDMENT);
		pEdit->SetWindowText(sText);
	}
}

void CSharePlankDesignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSharePlankDesignDlg)
	DDX_Text(pDX, IDC_EDIT_LS_NUM, m_nLsN);
	DDX_Text(pDX, IDC_EDIT_LS_SPACE, m_nLsSpace);
	DDX_Text(pDX, IDC_EDIT_ODDMENT, m_fOddment);
	DDX_CBString(pDX, IDC_CMB_LS_GUIGE, m_sLsGuiGe);
	DDV_MaxChars(pDX, m_sLsGuiGe, 24);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSharePlankDesignDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CSharePlankDesignDlg)
	ON_CBN_EDITCHANGE(IDC_CMB_LS_GUIGE, OnChangeCmbLsGuige)
	ON_CBN_SELCHANGE(IDC_CMB_LS_GUIGE, OnChangeCmbLsGuige)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSharePlankDesignDlg message handlers
BOOL CSharePlankDesignDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	MoveWndToLocation();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CLsGuiGeDlg dialog


CLsGuiGeDlg::CLsGuiGeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLsGuiGeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLsGuiGeDlg)
	m_sLsGuiGe = _T("20");
	//}}AFX_DATA_INIT
}


void CLsGuiGeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLsGuiGeDlg)
	DDX_CBString(pDX, IDC_CMB_LS_GUIGE, m_sLsGuiGe);
	DDV_MaxChars(pDX, m_sLsGuiGe, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLsGuiGeDlg, CDialog)
	//{{AFX_MSG_MAP(CLsGuiGeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLsGuiGeDlg message handlers

BOOL CLsGuiGeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLsGuiGeDlg::OnOK() 
{
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CModifyPartHandleDlg dialog


CModifyPartHandleDlg::CModifyPartHandleDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CModifyPartHandleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyPartHandleDlg)
#ifdef AFX_TARG_ENU_ENGLISH
	m_sTitle = _T("Parent Rod Handle(0X):");
#else 
	m_sTitle = _T("父杆件句柄(0X):");
#endif
	m_iMirMsg = 4;
	m_sHandle = _T("");
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption = "Update Node Parent Rod";
#else 
	m_sCaption = "修改节点父杆件";
#endif
	m_bCanZero=TRUE;
	m_bDispSelectBtn=FALSE;
	m_bPauseBreakExit=FALSE;
	m_bEnableMir=TRUE;
	m_pInvalidNode=NULL;
}


void CModifyPartHandleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyPartHandleDlg)
	DDX_Text(pDX, IDC_S_TITLE, m_sTitle);
	DDX_CBIndex(pDX, IDC_CMB_MIR_MSG, m_iMirMsg);
	DDX_Text(pDX, IDC_E_HEX_HANDLE, m_sHandle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyPartHandleDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyPartHandleDlg)
	ON_BN_CLICKED(IDC_BTN_SELECT_OBJECT, OnSelectObject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyPartHandleDlg message handlers

BOOL CModifyPartHandleDlg::OnInitDialog() 
{
	if(m_bInernalStart&&abs(m_hPickObj)>0)
	{
		if(m_pInvalidNode)
		{
			bool overlapped=false;
			if(m_pInvalidNode->handle==m_hPickObj)
				overlapped=true;
			else
			{
				CLDSNode* pSelNode=Ta.Node.FromHandle(m_hPickObj);
				if(m_pInvalidNode->Position(false).IsEqual(pSelNode->Position(false)))
					overlapped=true;
			}
			if(overlapped)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The selected node coincides with another rod's node,Please select again!");
#else 
				AfxMessageBox("选择节点与杆件另一端节点重合,请重新选择!");
#endif
			else
				m_sHandle.Format("0X%X",m_hPickObj);
		}
		else
			m_sHandle.Format("0X%X",m_hPickObj);
	}
	
	SetWindowText(m_sCaption);
	if(m_bDispSelectBtn)
	{
		GetDlgItem(IDC_S_TITLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_SELECT_OBJECT)->ShowWindow(SW_SHOW);
		CString sName;
		sName.Format("%s=>",m_sTitle);
		GetDlgItem(IDC_BTN_SELECT_OBJECT)->SetWindowText(sName);
	}
	else
	{
		GetDlgItem(IDC_S_TITLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BTN_SELECT_OBJECT)->ShowWindow(SW_HIDE);
	}
	if(!m_bEnableMir)
	{
		RECT rect;
		m_iMirMsg=0;
		GetDlgItem(IDC_CMB_MIR_MSG)->EnableWindow(m_bEnableMir);
		GetDlgItem(IDOK)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		GetDlgItem(IDOK)->SetWindowPos(NULL,rect.left,rect.top-20,0,0,SWP_NOZORDER | SWP_NOSIZE);
		GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		GetDlgItem(IDCANCEL)->SetWindowPos(NULL,rect.left,rect.top-20,0,0,SWP_NOZORDER | SWP_NOSIZE);
 		GetWindowRect(&rect);
 		rect.bottom-=20;
 		MoveWindow(&rect);
	}
	GetDlgItem(IDC_CMB_MIR_MSG)->ShowWindow(m_bEnableMir);
	GetDlgItem(IDC_S_MIR_MSG)->ShowWindow(m_bEnableMir);
	CDialog::OnInitDialog();
	return TRUE;
}

void CModifyPartHandleDlg::OnSelectObject() 
{
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	SelectObject(m_iPickObjType);
	CDialog::OnOK();
}

void CModifyPartHandleDlg::SetPickObjTypeId(int cls_id)
{
	m_iPickObjType=cls_id;
}

void CModifyPartHandleDlg::SetCmdPickPromptStr(CString sPromptStr)
{
	m_sCmdPickPrompt=sPromptStr;
}

void CModifyPartHandleDlg::OnOK() 
{
	CLDSPart *pPart=NULL;
	CLDSNode *pNode=NULL;
	CLDSPlane *pPlane=NULL;
	CString sBakHandle = m_sHandle;
	UpdateData();
	sscanf(m_sHandle,"%X",&m_nHandle);
	if(m_iPickObjType==CLS_NODE)
		pNode = console.FromNodeHandle(m_nHandle);
	else if(m_iPickObjType==CLS_GEPLANE)
		pPlane=console.FromPlaneHandle(m_nHandle);
	else if(m_iPickObjType==CLS_HOLEWALL && m_nHandle<0)
	{
		OBJECT_HANDLER handler;
		g_pSolidDraw->GetSolidObject(m_nHandle,&handler);
		pPart=console.FromPartHandle(handler.hiberid.masterId);
	}
	else
		pPart=console.FromPartHandle(m_nHandle);
	if(pNode||pPlane||pPart)
		CDialog::OnOK();
	else
	{
		m_sHandle = "0X0";
		m_nHandle = 0;
		 if(!m_bCanZero)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Don't find the specified part");
#else 
			AfxMessageBox("没有找到指定的构件");
#endif
		 else
			 CDialog::OnOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
// CShankInputStyleDlg dialog


CShankInputStyleDlg::CShankInputStyleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShankInputStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShankInputStyleDlg)
	m_iShankInputStyle = 0;
	//}}AFX_DATA_INIT
}


void CShankInputStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShankInputStyleDlg)
	DDX_Radio(pDX, IDC_RDO_SHANK_STYLE, m_iShankInputStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShankInputStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CShankInputStyleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShankInputStyleDlg message handlers
BOOL CShankInputStyleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
// CMirMsgDlg dialog
CMirMsgDlg::CMirMsgDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CMirMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMirMsgDlg)
	m_fOriginX = 0.0;
	m_fOriginY = 0.0;
	m_fOriginZ = 0.0;
	m_nArrayNum = 0;
	m_bMirAxisX = FALSE;
	m_bMirAxisY = FALSE;
	m_bMirAxisZ = FALSE;
	m_bMirRotation = FALSE;
	m_bMirAxisXYZ = FALSE;
	m_nRotateAngle = 90;
	m_bUseMirrorSymmetry = FALSE;
	//}}AFX_DATA_INIT
}


void CMirMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMirMsgDlg)
	DDX_Text(pDX, IDC_E_ORIGIN_X, m_fOriginX);
	DDX_Text(pDX, IDC_E_ORIGIN_Y, m_fOriginY);
	DDX_Text(pDX, IDC_E_ORIGIN_Z, m_fOriginZ);
	DDX_Text(pDX, IDC_E_ARRAY_NUM, m_nArrayNum);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_X, m_bMirAxisX);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Y, m_bMirAxisY);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_Z, m_bMirAxisZ);
	DDX_Check(pDX, IDC_CHK_MIR_ROTATE, m_bMirRotation);
	DDX_Check(pDX, IDC_CHK_MIR_AXIS_XYZ, m_bMirAxisXYZ);
	DDX_Check(pDX, IDC_CHK_45_MIR,m_b45MirrorsSymmetry);
	DDX_Check(pDX,IDC_CHK_135_MIR,m_b135MirrorSymmetry);
	DDX_Check(pDX, IDC_CHK_ADVANCED_OPTION, m_bAdvancedOption);
	DDX_Text(pDX, IDC_E_ROTATE_ANGLE, m_nRotateAngle);
	DDV_MinMaxInt(pDX, m_nRotateAngle, -360, 360);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMirMsgDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CMirMsgDlg)
	ON_BN_CLICKED(IDC_BN_PASTE_ORIGIN, OnBnPasteOrigin)
	ON_BN_CLICKED(IDC_CHK_MIR_ROTATE, OnChkMirRotate)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_XYZ, OnChkMirAxisXyz)
	ON_BN_CLICKED(IDC_CHK_45_MIR, OnChkMir45Plane)
	ON_BN_CLICKED(IDC_CHK_135_MIR, OnChkMir135Plane)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_X, &CMirMsgDlg::OnChkMirAxisXyz2)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Y, &CMirMsgDlg::OnChkMirAxisXyz2)
	ON_BN_CLICKED(IDC_CHK_MIR_AXIS_Z, &CMirMsgDlg::OnChkMirAxisXyz2)
	ON_BN_CLICKED(IDC_CHK_ADVANCED_OPTION, OnAdvancedOption)
	ON_BN_CLICKED(IDC_BTN_OK, OnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMirMsgDlg message handlers

BOOL CMirMsgDlg::OnInitDialog() 
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
	GetDlgItem(IDC_E_ROTATE_ANGLE)->EnableWindow(m_bMirRotation);
	GetDlgItem(IDC_E_ARRAY_NUM)->EnableWindow(m_bMirRotation);
	GetDlgItem(IDC_WORK_PANEL)->ShowWindow(SW_HIDE);
	if(m_bAdvancedOption==FALSE)	//对话框默认为展开，不启用高级设置需调整对话框
		AdjustWndPosition();
	UpdateData(FALSE);
	MoveWndToLocation();
	return TRUE;
}

void CMirMsgDlg::AdjustWndPosition()
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
	{	//关闭高级选项后，45度对称与135度对称赋默认值 wht 16-05-24
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
void CMirMsgDlg::OnChkMirRotate() 
{
	UpdateData();
	GetDlgItem(IDC_E_ROTATE_ANGLE)->EnableWindow(m_bMirRotation);
	GetDlgItem(IDC_E_ARRAY_NUM)->EnableWindow(m_bMirRotation);
	UpdateData(FALSE);
}

void CMirMsgDlg::OnBnPasteOrigin() 
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

void CMirMsgDlg::OnOk() 
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


void CMirMsgDlg::OnChkMirAxisXyz() 
{
	UpdateData();
	m_bMirAxisX = m_bMirAxisXYZ;
	m_bMirAxisY = m_bMirAxisXYZ;
	m_bMirAxisZ = m_bMirAxisXYZ;
	UpdateData(FALSE);
}

void CMirMsgDlg::OnChkMirAxisXyz2()
{
	UpdateData();
	m_bMirAxisXYZ=m_bMirAxisX&&m_bMirAxisY&&m_bMirAxisZ;
	UpdateData(FALSE);
}
void CMirMsgDlg::OnChkMir45Plane()
{
	UpdateData();
	if(m_b45MirrorsSymmetry)
		m_b135MirrorSymmetry=FALSE;
	UpdateData(FALSE);
}
void CMirMsgDlg::OnChkMir135Plane()
{
	UpdateData();
	if(m_b135MirrorSymmetry)
		m_b45MirrorsSymmetry=FALSE;
	UpdateData(FALSE);
}

void CMirMsgDlg::OnAdvancedOption() 
{
	UpdateData();
	AdjustWndPosition();
	UpdateData(FALSE);
}

void CMirMsgDlg::OnCancel() 
{
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CHuoQuOffsetDlg dialog


CHuoQuOffsetDlg::CHuoQuOffsetDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CHuoQuOffsetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHuoQuOffsetDlg)
	m_fHuoQuOffset = 0.0;
	m_nByStart = 0;
	m_fHuoQuR = 0.0;
	m_iHuoQuBaseWing = 2;
	//}}AFX_DATA_INIT
}


void CHuoQuOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHuoQuOffsetDlg)
	DDX_Text(pDX, IDC_E_OFFSET, m_fHuoQuOffset);
	DDX_Radio(pDX, IDC_RDO_OFSSET_DIRECTION, m_nByStart);
	DDX_Text(pDX, IDC_E_HUOQU_R, m_fHuoQuR);
	DDX_Radio(pDX, IDC_RDO_HUOQU_BASE_WING, m_iHuoQuBaseWing);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHuoQuOffsetDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CHuoQuOffsetDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHuoQuOffsetDlg message handlers
BOOL CHuoQuOffsetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	MoveWndToLocation();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CLinkJGDlg message handlers
void CLinkJGDlg::OnChkGroupJg() 
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

void CLinkJGDlg::OnEnChangeESegNo()
{
	UpdateData();
	SEGI iOldSeg;
	CXhChar16 serial;
	//当前构件编号
	if(m_sOriginPartNo.GetLength()>0 && ParsePartNo(m_sOriginPartNo.GetString(),&iOldSeg,serial))
	{
		m_sOriginPartNo=m_sSegNo;
		if(serial.GetLength()>2)
			m_sOriginPartNo.Append("-");
		m_sOriginPartNo.Append(serial);
	}
	//X轴对称构件编号
	if(m_sMirXOriginPartNo.GetLength()>0 && ParsePartNo(m_sMirXOriginPartNo.GetString(),&iOldSeg,serial))
	{
		m_sMirXOriginPartNo=m_sSegNo;
		if(serial.GetLength()>2)
			m_sMirXOriginPartNo.Append("-");
		m_sMirXOriginPartNo.Append(serial);
	}
	//Y轴对称构件编号
	if(m_sMirYOriginPartNo.GetLength()>0 && ParsePartNo(m_sMirYOriginPartNo.GetString(),&iOldSeg,serial))
	{
		m_sMirYOriginPartNo=m_sSegNo;
		if(serial.GetLength()>2)
			m_sMirYOriginPartNo.Append("-");
		m_sMirYOriginPartNo.Append(serial);
	}
	//Z轴(对角)对称构件编号
	if(m_sMirDiagOriginPartNo.GetLength()>0 && ParsePartNo(m_sMirDiagOriginPartNo.GetString(),&iOldSeg,serial))
	{
		m_sMirDiagOriginPartNo=m_sSegNo;
		if(serial.GetLength()>2)
			m_sMirDiagOriginPartNo.Append("-");
		m_sMirDiagOriginPartNo.Append(serial);
	}
	m_bDisconnectSegIAndPartNo=TRUE;
	UpdateData(FALSE);
	m_bDisconnectSegIAndPartNo=FALSE;
}
void CLinkJGDlg::OnChangeEOriginPartNo() 
{
	CXhChar16 serial_str[4];
	char temStr[16]="",matStr[16]="",digitStr[16]="";
	int i,serial[4]={0};
	//当前构件编号
	if(m_sOriginPartNo.GetLength()>0)
		ParsePartNo(m_sOriginPartNo.GetString(),NULL,serial_str[0]);
	//X轴对称构件编号
	if(m_sMirXOriginPartNo.GetLength()>0)
		ParsePartNo(m_sMirXOriginPartNo.GetString(),NULL,serial_str[1]);
	//Y轴对称构件编号
	if(m_sMirYOriginPartNo.GetLength()>0)
		ParsePartNo(m_sMirYOriginPartNo.GetString(),NULL,serial_str[2]);
	//Z轴(对角)对称构件编号
	if(m_sMirDiagOriginPartNo.GetLength()>0)
		ParsePartNo(m_sMirDiagOriginPartNo.GetString(),NULL,serial_str[3]);
	for(i=0;i<4;i++)
	{	//考虑构件编号后面跟材质字符的情况
		StrCopy(temStr,serial_str[i],16);
		int nLen=strlen(temStr);
		if(nLen<1)
			continue;
		int iDigitFinal=0;
		for(int j=0;j<nLen;j++)
		{
			BOOL bDigit=isdigit((BYTE)temStr[j]);
			if(bDigit)
				iDigitFinal=j;
		}
		if(iDigitFinal<nLen-1)		//件号流水中有字符
		{	
			memcpy(digitStr,temStr,iDigitFinal+1);
			digitStr[iDigitFinal+1]=0;
			strcpy(serial_str[i],digitStr);
		}
		serial[i]=atoi(serial_str[i]);
	}
	
	UpdateData();
	CString sNo=m_sOriginPartNo;
	bool bChangeGuige=false;
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
		VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
		pPart=globalDataCmpModel.GetDrawingBomPart(sNo);
	if(pPart)
	{	//根据找到的BOM信息，更新该角钢的基本信息(段号、规格、材质)
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			m_sJgGuiGe.Format("%.0fX%.0f",pPart->wide,pPart->thick);
			bChangeGuige=true;			//在UpdateData(FALSE)之后触发规格改变函数
			m_bAutoMatchMat = FALSE;	//不自动匹配角钢材质
			m_nJgMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			JGZJ jgzj;
			getjgzj(jgzj,pPart->wide);
			m_nWingG = jgzj.g;
			CEdit* pEdit = (CEdit*) GetDlgItem(IDC_E_WING_G);
			pEdit->SetWindowText(CXhChar50("%d",m_nWingG));
		}
		//else
			//AfxMessageBox(CXhChar100("Bom中没有件号为#%s的角钢",(char*)pPart->sPartNo));
	}
#endif
	if(m_sOriginPartNo.GetLength()>15)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The original part's label is too long!");
#else 
		AfxMessageBox("原始构件编号太长!");
#endif
		m_sOriginPartNo = sNo;
		return;
	}
	int len,curr_serial=0,increment=0;
	SEGI iSeg;
	bool parsed=ParsePartNo(sNo.GetBuffer(),&iSeg,serial_str[0]);
	const char* separator=strchr(m_sOriginPartNo,'-');
	if(separator==NULL)
		separator=strchr(m_sOriginPartNo,'_');
	if(m_sOriginPartNo.GetLength()>0 && parsed)
	{
		strcpy(temStr,serial_str[0]);
		int  nLen=strlen(temStr);
		int iDigitFinal=0;
		for(int i=0;i<nLen;i++)
		{
			BOOL bDigit=isdigit((BYTE)temStr[i]);
			if(bDigit)
				iDigitFinal=i;
		}
		if(iDigitFinal<nLen-1)
		{	//将流水号拆分成数字部分和字符部分
			int len=nLen-iDigitFinal-1;
			memcpy(matStr,&temStr[iDigitFinal+1],len);	//字符部分
			matStr[len]=0;
			memcpy(digitStr,&temStr[0],iDigitFinal+1);	//数字部分
			digitStr[iDigitFinal+1]=0;
			strcpy(serial_str[0],digitStr);
		}
		curr_serial=atoi(serial_str[0]);
		increment=curr_serial-serial[0];
		m_sMirXOriginPartNo=m_sMirYOriginPartNo=m_sMirDiagOriginPartNo=iSeg.ToString();
		char spliter[2]={0,0};
		if(separator)//curr_serial>=100)
		{
			spliter[0]=*separator;
			m_sMirXOriginPartNo.Append(spliter);
		}
		m_sMirYOriginPartNo=m_sMirDiagOriginPartNo=m_sMirXOriginPartNo;
		CXhChar16 ss("%d",serial[1]+increment);
		len=max(2,ss.GetLength());
		ss.ResizeLength(len,'0',true);
		if(strlen(matStr)>0)
			ss.Append(CXhChar16("%s",matStr));
		m_sMirXOriginPartNo.Append(ss);
		
		ss.Printf("%d",serial[2]+increment);
		len=max(2,ss.GetLength());
		ss.ResizeLength(len,'0',true);
		if(strlen(matStr)>0)
			ss.Append(CXhChar16("%s",matStr));
		m_sMirYOriginPartNo.Append(ss);

		ss.Printf("%d",serial[3]+increment);
		len=max(2,ss.GetLength());
		ss.ResizeLength(len,'0',true);
		if(strlen(matStr)>0)
			ss.Append(CXhChar16("%s",matStr));
		m_sMirDiagOriginPartNo.Append(ss);
	}
	else
	{
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
	}
	if(parsed)
		m_sSegNo=iSeg.ToString();
	m_bDisconnectSegIAndPartNo=TRUE;
	UpdateData(FALSE);
	m_bDisconnectSegIAndPartNo=FALSE;
	if(bChangeGuige)	//规格改变之后需更新螺栓直径 wht 18-12-04
		OnSelchangeCmbJgGuiege();
}

void CLinkJGDlg::OnChangeEMirXOriginPartNo() 
{
	CString sNo=m_sMirXOriginPartNo;
	UpdateData();
	if(m_sOriginPartNo.GetLength()>15)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The original part's label is too long!");
#else 
		AfxMessageBox("原始构件编号太长!");
#endif
		m_sMirXOriginPartNo = sNo;
		return;
	}
	m_sMirYOriginPartNo=m_sMirXOriginPartNo;	
	UpdateData(FALSE);
}

void CLinkJGDlg::OnBnGroupStyle() 
{
	CGroupJgStyleDlg group_jg_style_dlg;
	group_jg_style_dlg.m_fFillThick=m_fFillThick;
	//m_iJgGroupStyle已改为GroupStyle和LayoutStyle，所以暂时没法对应，就先不赋值
	//group_jg_style_dlg.m_iJgGroupStyle=group_style;
	if(group_jg_style_dlg.DoModal()==IDOK)
	{
		wSectStyle=group_jg_style_dlg.wSectStyle;
		ciMasterIndex=group_jg_style_dlg.ciMasterIndex;
		m_fFillThick=group_jg_style_dlg.m_fFillThick;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExtendJgDlg dialog


CExtendJgDlg::CExtendJgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExtendJgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtendJgDlg)
	m_fExtendDist = 0.0;
	m_nExtendStyle = 0;
	//}}AFX_DATA_INIT
}


void CExtendJgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtendJgDlg)
	DDX_Text(pDX, IDC_EXTEND_DIST, m_fExtendDist);
	DDX_Radio(pDX, IDC_EXTEND_STYLE, m_nExtendStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtendJgDlg, CDialog)
	//{{AFX_MSG_MAP(CExtendJgDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtendJgDlg message handlers

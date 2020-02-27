// LsFeatDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "BoltPropDlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "CfgPartNoDlg.h"
#include "env_def.h"
#include "LmaDlg.h"
#include "LDSView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CBoltPropDlg dialog


CBoltPropDlg::CBoltPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBoltPropDlg::IDD, pParent)
	, m_iHoleFuncType(2)
{
	//{{AFX_DATA_INIT(CBoltPropDlg)
	m_sLsGuiGe = _T("6.8M20X50");
	m_fHoleD = 21.5;
	m_fPosX = 0.0;
	m_fPosY = 0.0;
	m_fPosZ = 0.0;
	m_hLs = _T("");
	m_fNormX = 0.0;
	m_fNormY = 0.0;
	m_fNormZ = 1.0;
	m_nWaistLen = 0;
	//}}AFX_DATA_INIT
	m_bCanModifyPos = TRUE;
	m_pLs=NULL;
	m_pWorkPart=NULL;
	m_dwRayNo=0;
	bak_des_ls_pos.datumPoint.datum_pos_style=0;	//无螺栓设计参数
	waist_vec.Set(1,0,0);
	m_bPunchHole=FALSE;
	m_bDrillHole=!m_bPunchHole;
}


void CBoltPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBoltPropDlg)
	DDX_CBString(pDX, IDC_CMB_LS_GUIGE, m_sLsGuiGe);
	DDX_Text(pDX, IDC_E_HOLE_D, m_fHoleD);
	DDX_Text(pDX, IDC_E_X, m_fPosX);
	DDX_Text(pDX, IDC_E_Y, m_fPosY);
	DDX_Text(pDX, IDC_E_Z, m_fPosZ);
	DDX_Text(pDX, IDC_S_LS_HANDLE, m_hLs);
	DDX_Text(pDX, IDC_E_NORM_X, m_fNormX);
	DDX_Text(pDX, IDC_E_NORM_Y, m_fNormY);
	DDX_Text(pDX, IDC_E_NORM_Z, m_fNormZ);
	DDX_Text(pDX, IDC_E_WAIST_LEN, m_nWaistLen);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHK_PUNCH, m_bPunchHole);
	DDX_Check(pDX, IDC_CHK_DRILL, m_bDrillHole);
	DDX_Check(pDX, IDC_CHK_ENLARGE, m_bEnlargeHole);
	DDX_CBIndex(pDX, IDC_CMB_HOLE_TYPE, m_iHoleFuncType);
}


BEGIN_MESSAGE_MAP(CBoltPropDlg, CDialog)
	//{{AFX_MSG_MAP(CBoltPropDlg)
	ON_CBN_EDITCHANGE(IDC_CMB_LS_GUIGE, OnChangeCmbLsGuige)
	ON_EN_CHANGE(IDC_E_Y, OnChangeEY)
	ON_BN_CLICKED(IDC_BN_COPY_POS, OnBnCopyPos)
	ON_BN_CLICKED(IDC_BN_COPY_NORM, OnBnCopyNorm)
	ON_BN_CLICKED(IDC_BN_PASTE_NORM, OnBnPasteNorm)
	ON_BN_CLICKED(IDC_BN_PASTE_POS, OnBnPastePos)
	ON_EN_KILLFOCUS(IDC_E_X, OnKillfocusEX)
	ON_CBN_SELCHANGE(IDC_CMB_LS_GUIGE, OnChangeCmbLsGuige)
	ON_BN_CLICKED(IDC_BTN_MODIFY_WAIST_VEC, OnModifyWaistVec)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHK_PUNCH, &CBoltPropDlg::OnClickedChkPunchHole)
	ON_BN_CLICKED(IDC_CHK_DRILL, &CBoltPropDlg::OnClickedChkDrillHole)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBoltPropDlg message handlers

BOOL CBoltPropDlg::OnInitDialog() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_LS_GUIGE);
	if(m_pLs)
	{
		AddLsRecord(pCMB,m_pLs->m_hFamily);
		bak_des_ls_pos=m_pLs->des_base_pos;
		bak_des_ls_norm=m_pLs->des_work_norm;
		m_dwRayNo=m_pLs->dwRayNo;
	}
	else	//新生成螺栓
	{
		AddLsRecord(pCMB,0);
		waist_vec.Set(1,0,0);	//初始化腰圆孔方向
		bak_des_ls_pos.norm_offset.EmptyThick(); //清空法向偏移量
		if(m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			bak_des_ls_norm.norm_style=1;
			bak_des_ls_norm.hVicePart=m_pWorkPart->handle;
			bak_des_ls_pos.hPart=m_pWorkPart->handle;
			if(bak_des_ls_norm.direction==0)
				bak_des_ls_pos.norm_offset.AddThick(-ftol(((CLDSLineAngle*)m_pWorkPart)->GetThick()));
		}
		else if(m_pWorkPart->GetClassTypeId()==CLS_PLATE||m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			bak_des_ls_norm.norm_style=4;	//基准构件上的相对方向
			bak_des_ls_norm.hVicePart=m_pWorkPart->handle;
			bak_des_ls_norm.vector=((CLDSPart*)m_pWorkPart)->ucs.axis_z;
			bak_des_ls_norm.nearVector=f3dPoint(0,0,1);
			bak_des_ls_pos.hPart=m_pWorkPart->handle;
			bak_des_ls_pos.norm_offset.AddThick(ftol(-0.5*((CLDSPart*)m_pWorkPart)->GetThick()));
		}
		else if(m_pWorkPart->GetClassTypeId()==CLS_ARCSLOT||m_pWorkPart->GetClassTypeId()==CLS_ARCANGLE||m_pWorkPart->GetClassTypeId()==CLS_ARCFLAT)
		{	//在环形构件上添加螺栓时，螺栓法向方向随螺栓位置自动调整
			((CEdit*)GetDlgItem(IDC_E_NORM_X))->SetReadOnly(TRUE);
			((CEdit*)GetDlgItem(IDC_E_NORM_Y))->SetReadOnly(TRUE);
			((CEdit*)GetDlgItem(IDC_E_NORM_Z))->SetReadOnly(TRUE);
			bak_des_ls_pos.norm_offset.AddThick(ftol(-((CLDSArcPart*)m_pWorkPart)->GetThick()));
		}
	}
	int iCurSel = pCMB->FindString(0,m_sLsGuiGe);
	if(iCurSel>=0)
		pCMB->SetCurSel(iCurSel);
	else
	{
		pCMB->AddString(m_sLsGuiGe);
		pCMB->SetCurSel(pCMB->GetCount()-1);
	}
	if(fabs(m_fPosX)<eps)
		m_fPosX =0;
	if(fabs(m_fPosY)<eps)
		m_fPosY =0;
	if(fabs(m_fPosZ)<eps)
		m_fPosZ =0;
	if(fabs(m_fNormX)<eps)
		m_fNormX =0;
	if(fabs(m_fNormY)<eps)
		m_fNormY =0;
	if(fabs(m_fNormZ)<eps)
		m_fNormZ =0;
	((CEdit*)GetDlgItem(IDC_E_X))->SetReadOnly(!m_bCanModifyPos);
	((CEdit*)GetDlgItem(IDC_E_Y))->SetReadOnly(!m_bCanModifyPos);
	((CEdit*)GetDlgItem(IDC_E_Z))->SetReadOnly(!m_bCanModifyPos);
	GetDlgItem(IDC_BN_PASTE_POS)->EnableWindow(m_bCanModifyPos);
	if(m_pLs)
	{
		//仅当螺栓法线设计类型为指定法线时才可以修改螺栓法线位置，否则修改后一到位将恢复到原来的值
		GetDlgItem(IDC_BN_PASTE_NORM)->EnableWindow(m_pLs->des_work_norm.norm_style==0);
		((CEdit*)GetDlgItem(IDC_E_NORM_X))->SetReadOnly(m_pLs->des_work_norm.norm_style!=0);
		((CEdit*)GetDlgItem(IDC_E_NORM_Y))->SetReadOnly(m_pLs->des_work_norm.norm_style!=0);
		((CEdit*)GetDlgItem(IDC_E_NORM_Z))->SetReadOnly(m_pLs->des_work_norm.norm_style!=0);
	}
	CDialog::OnInitDialog();
	OnChangeEY();
	return TRUE;
}

void CBoltPropDlg::OnChangeCmbLsGuige() 
{
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_LS_GUIGE);
	int iCurSel = pCMB->GetCurSel();
	if(iCurSel>=0)
		pCMB->GetLBText(iCurSel,m_sLsGuiGe);
	else
		return;

	CLDSBolt ls(&Ta);
	if(!restore_Ls_guige(m_sLsGuiGe,ls))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The specification of bolt is unreasonable!");
#else 
		AfxMessageBox("不合理的螺栓规格!");
#endif
		UpdateData();
		UpdateData(FALSE);
	}

	if(ls.get_d()==12)
		m_fHoleD = 13.5;
	else if(ls.get_d()==16)
		m_fHoleD = 17.5;
	else if(ls.get_d()==20)
		m_fHoleD = 21.5;
	else if(ls.get_d()==24)
		m_fHoleD = 25.5;
	else if(ls.get_d()==27)
		m_fHoleD = 35;
	else if(ls.get_d()==30)
		m_fHoleD = 40;
	else if(ls.get_d()==36)
		m_fHoleD = 45;
	else if(ls.get_d()==42)
		m_fHoleD = 55;
	else if(ls.get_d()==45)
		m_fHoleD = 60;
	else if(ls.get_d()==48)
		m_fHoleD = 60;
	else if(ls.get_d()==52)
		m_fHoleD = 65;
	else if(ls.get_d()==56)
		m_fHoleD = 70;
	else if(ls.get_d()==60)
		m_fHoleD = 75;
	else if(ls.get_d()==64)
		m_fHoleD = 80;
	CString ss;
	ss.Format("%.1f",m_fHoleD);
	GetDlgItem(IDC_E_HOLE_D)->SetWindowText(ss);
}

void CBoltPropDlg::OnOK() 
{
	CDialog::OnOK();
	OnKillfocusEX();
	if(m_pLs)
	{
		m_pLs->des_base_pos=bak_des_ls_pos;
		m_pLs->des_work_norm=bak_des_ls_norm;
		m_pLs->dwRayNo=m_dwRayNo;
	}
}

f3dPoint CBoltPropDlg::ToUcsPoint(f3dPoint from_point,f3dPoint ls_norm)
{
	if(m_pWorkPart==NULL)
		return from_point;
	if(m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle *pJg=(CLDSLineAngle*)m_pWorkPart;
		f3dPoint ls_pos = from_point;
		UCS_STRU ucs;
		pJg->getUCS(ucs);
		coord_trans(ls_pos,ucs,FALSE);
		double depth;
		double g = pJg->GetLsG(ls_norm,from_point,&depth);
		if(fabs(ls_norm*pJg->get_norm_x_wing())>
			fabs(ls_norm*pJg->get_norm_y_wing()))
		{	//X肢上的螺栓
			ls_pos.Set(ls_pos.z,-g,depth);
		}
		else
		{	//Y肢上的螺栓
			ls_pos.Set(ls_pos.z,g,depth);
		}
		coord_trans(ls_pos,work_ucs,FALSE);
		return ls_pos;
	}
	else if(m_pWorkPart->GetClassTypeId()==CLS_PLATE)
	{
		f3dPoint ls_pos;
		CLDSPlate *pPlank=(CLDSPlate*)m_pWorkPart;
		pPlank->SpreadLsPos(ls_norm,ls_pos);
		coord_trans(ls_pos,console.m_curWCS,FALSE);
		return ls_pos;
	}
	else
		return from_point;
}
void CBoltPropDlg::OnChangeEY() 
{
	CString ss;
	GetDlgItem(IDC_E_Y)->GetWindowText(ss);
	int nStrLen=ss.GetLength();
	if((nStrLen==1&&ss[0]=='-')||(nStrLen>1&&ss[nStrLen-1]=='.'))
		return;
	if(atof(ss)==m_fPosY)
		return;
	UpdateBoltNorm();
}
void CBoltPropDlg::UpdateBoltNorm()
{
	UpdateData();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(!(console.m_pWorkPart&&pView->m_curAppType==TA_ASSEMBLE_ENV))
	{	//构件编辑器环境执行以下代码 wht 10-09-09
		f3dPoint ls_pos(m_fPosX,m_fPosY,m_fPosZ);
		coord_trans(ls_pos,work_ucs,TRUE);
		if(m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle *pCommJg=(CLDSLineAngle*)m_pWorkPart;
			bak_des_ls_norm.hVicePart=m_pWorkPart->handle;
			if(ls_pos.y>0)
			{
				bak_des_ls_norm.norm_style=1;
				bak_des_ls_norm.norm_wing=1;
				m_fNormX=pCommJg->get_norm_y_wing().x;
				m_fNormY=pCommJg->get_norm_y_wing().y;
				m_fNormZ=pCommJg->get_norm_y_wing().z;
			}
			else
			{
				bak_des_ls_norm.norm_style=1;
				bak_des_ls_norm.norm_wing=0;
				m_fNormX=pCommJg->get_norm_x_wing().x;
				m_fNormY=pCommJg->get_norm_x_wing().y;
				m_fNormZ=pCommJg->get_norm_x_wing().z;
			}
		}
		else if(m_pWorkPart->GetClassTypeId()==CLS_PLATE
			||m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSGeneralPlate *pPart=(CLDSGeneralPlate*)m_pWorkPart;
			int iFaceNo= pPart->GetFaceNoByPos(ls_pos);
			GEPOINT ls_norm=pPart->ucs.axis_z;
			if (iFaceNo==2||iFaceNo==3)
			{
				bak_des_ls_norm.reside_plane=iFaceNo-1;
				bak_des_ls_pos.reside_plane=iFaceNo-1;
				ls_norm=pPart->GetHuoquFaceNorm(iFaceNo-2);
			}
			else
				bak_des_ls_norm.reside_plane=bak_des_ls_pos.reside_plane=0;
			m_fNormX=ls_norm.x;
			m_fNormY=ls_norm.y;
			m_fNormZ=ls_norm.z;
		}
		else if(m_pWorkPart->GetClassTypeId()==CLS_ARCSLOT||m_pWorkPart->GetClassTypeId()==CLS_ARCANGLE||m_pWorkPart->GetClassTypeId()==CLS_ARCFLAT)
		{
			CLDSArcPart *pArcPart=(CLDSArcPart*)m_pWorkPart;
			double rotAngel=ls_pos.x/pArcPart->baseline.Radius();
			f3dPoint norm=pArcPart->ucs.axis_x;
			RotateVectorAroundVector(norm,pArcPart->start_rot_angle+rotAngel,pArcPart->ucs.axis_z);
			normalize(norm);
			m_fNormX=norm.x;
			m_fNormY=norm.y;
			m_fNormZ=norm.z;
		}
	}
	UpdateData(FALSE);
}

void CBoltPropDlg::OnBnCopyPos() 
{
	UpdateData(TRUE);
	f3dPoint point(m_fPosX,m_fPosY,m_fPosZ);
	WritePointToClip(point);
}

void CBoltPropDlg::OnBnCopyNorm() 
{
	UpdateData(TRUE);
	f3dPoint point(m_fNormX,m_fNormY,m_fNormZ);
	WritePointToClip(point);
}

void CBoltPropDlg::OnBnPasteNorm() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fNormX = point.x;
		m_fNormY = point.y;
		m_fNormZ = point.z;
		UpdateData(FALSE);
	}
}

void CBoltPropDlg::OnBnPastePos() 
{
	f3dPoint point;
	if(ReadClipPoint(point))
	{
		m_fPosX = point.x;
		m_fPosY = point.y;
		m_fPosZ = point.z;
		UpdateData(FALSE);
	}
}

void CBoltPropDlg::OnKillfocusEX() 
{
	UpdateBoltNorm();
}

//修改腰圆孔方向即螺栓X轴方向
void CBoltPropDlg::OnModifyWaistVec() 
{
	UpdateData();
	CReportVectorDlg dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	dlg.m_sCaption="Modify Waist Round Hole Direction";
#else 
	dlg.m_sCaption="修改腰圆孔方向";
#endif
	dlg.m_fVectorX = waist_vec.x;
	dlg.m_fVectorY = waist_vec.y;
	dlg.m_fVectorZ = waist_vec.z;

	if(dlg.DoModal()==IDOK)
	{
		waist_vec.x = dlg.m_fVectorX;
		waist_vec.y = dlg.m_fVectorY;
		waist_vec.z = dlg.m_fVectorZ;
	}
	UpdateData(FALSE);
}
void CBoltPropDlg::OnClickedChkPunchHole()
{
	UpdateData();
	m_bDrillHole=!m_bPunchHole;
	UpdateData(FALSE);
}
void CBoltPropDlg::OnClickedChkDrillHole()
{
	UpdateData();
	m_bPunchHole=!m_bDrillHole;
	UpdateData(FALSE);
}
#endif





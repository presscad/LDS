// FootDesignDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "AddDiJiaoLsDlg.h"
#include "AddLeiBanDlg.h"
#include "Query.h"
#include "tower.h"
#include "GlobalFunc.h"
#include "UserDefOper.h"
#include "ScrTools.h"
#include "FootDesignDlg.h"
#include "DesignJdb.h"
#include "LayAngleBoltDlg.h"
#include "env_def.h"
#include "FootPlankWrapLenDlg.h"
//#include "DrawEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

static const double cos45=0.70710678118654752440084436210485;	//2的平方根的一半
/////////////////////////////////////////////////////////////////////////////
// CFootDesignDlg dialog

extern CEvent LButtonDown,RButtonDown,HaltProc;
CFootDesignDlg::CFootDesignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFootDesignDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFootDesignDlg)
	m_iCurOper = 3;
	m_bSaveData=FALSE;
	//}}AFX_DATA_INIT
	m_pFoot = NULL;
	//m_pDraw = NULL;
	//m_pCurSnapAtom = NULL;
	//m_pCurHighlightAtom = NULL;
	m_bLMouseDown = FALSE;
	oper_type  = OPER_OTHER;
	m_fScaleScreenToUser = 0.45;
	strcpy(foot_para.sDiBanPartNo,"");			//底板编号
	strcpy(foot_para.sUpXueBanPartNo0C,"");		//上侧  0°靴板编号
	strcpy(foot_para.sUpXueBanPartNo45C,"");	//上侧 45°靴板编号
	strcpy(foot_para.sUpXueBanPartNo90C,"");	//上侧 90°靴板编号
	strcpy(foot_para.sUpXueBanPartNo135C,"");	//上侧135°靴板编号
	strcpy(foot_para.sUpXueBanPartNo180C,"");	//上侧180°靴板编号
	strcpy(foot_para.sUpXueBanPartNo225C,"");	//上侧225°靴板编号
	strcpy(foot_para.sUpXueBanPartNo270C,"");	//上侧270°靴板编号
	strcpy(foot_para.sUpXueBanPartNo315C,"");	//上侧315°靴板编号
	strcpy(foot_para.sDownXueBanPartNo0C,"");	//下侧  0°靴板编号
	strcpy(foot_para.sDownXueBanPartNo45C,"");	//下侧 45°靴板编号
	strcpy(foot_para.sDownXueBanPartNo90C,"");	//下侧 90°靴板编号
	strcpy(foot_para.sDownXueBanPartNo135C,"");	//下侧135°靴板编号
	strcpy(foot_para.sDownXueBanPartNo180C,"");	//下侧180°靴板编号
	strcpy(foot_para.sDownXueBanPartNo225C,"");	//下侧225°靴板编号
	strcpy(foot_para.sDownXueBanPartNo270C,"");	//下侧270°靴板编号
	strcpy(foot_para.sDownXueBanPartNo315C,"");	//下侧315°靴板编号
	foot_para.m_iDiBanShape = 0;
	foot_para.m_iA=30;
	foot_para.m_iB=30;
	foot_para.m_iUpC=foot_para.m_iDownC=25;
	foot_para.m_iS = 170;
	foot_para.m_iUpLsLayStyle=foot_para.m_iDownLsLayStyle=0;
	foot_para.m_iWide=580;
	foot_para.m_iThick=30;
	foot_para.m_iUpXueBanThick=foot_para.m_iDownXueBanThick=0;
	foot_para.m_cDiBanMat=CSteelMatLibrary::Q235BriefMark();//'S';
	foot_para.m_cXueBanMat=CSteelMatLibrary::Q235BriefMark();//'S';
}


void CFootDesignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFootDesignDlg)
	DDX_Radio(pDX, IDC_RDO_CUR_OPER_STATE, m_iCurOper);
	DDX_Check(pDX, IDC_CHK_SAVE_DATA, m_bSaveData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFootDesignDlg, CDialog)
	//{{AFX_MSG_MAP(CFootDesignDlg)
	ON_BN_CLICKED(IDC_BN_ADD_DIJIAO_LS, OnBnAddDijiaoLs)
	ON_BN_CLICKED(IDC_BN_ADD_LEI_BAN, OnBnAddLeiBan)
	ON_BN_CLICKED(IDC_BN_MODIFY_LEI_BAN, OnBnModifyLeiBan)
	ON_BN_CLICKED(IDC_BN_MODIFY_LS, OnBnModifyLs)
	ON_BN_CLICKED(IDC_BN_PARAM_SET, OnBnParamSet)
	ON_BN_CLICKED(IDC_BN_IMPORT_XML_FILE, OnBnImPortXMLFile)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_BN_CLEAR_DES_RESULT, OnBnClearDesResult)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFootDesignDlg message handlers

BOOL CFootDesignDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_pFoot->CreateRefAxis();
	CLDSLineAngle *pJg=(CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(pJg&&foot_para.m_iUpXueBanThick==0)
		foot_para.m_iUpXueBanThick=ftoi(pJg->GetThick());
	pJg=(CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(pJg&&foot_para.m_iDownXueBanThick==0)
		foot_para.m_iDownXueBanThick=ftoi(pJg->GetThick());
	OnBnParamSet();
	CPoint pos(0,0),rgn_pos;	//客户区左上角坐标
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&m_rectValidRgn);	//绘图有效区的屏坐标区域
	//初始化绘图引擎
	/*RECT rectClient;
	rectClient.top=m_rectValidRgn.top;
	rectClient.bottom=m_rectValidRgn.bottom;
	rectClient.left=m_rectValidRgn.left;
	rectClient.right=m_rectValidRgn.right;
	ScreenToClient(&rectClient);
	m_pDraw = CDrawEngine::CreateEngine();
	m_pDraw->Init(GetSafeHwnd());
	m_pDraw->InitialUpdate(&rectClient);*/

	ClientToScreen(&pos);	//对话框客户区左上角坐标转为屏幕坐标
	//确定绘图区相对对话框客户区的相对坐标系
	rgn_ucs.origin.Set(m_rectValidRgn.CenterPoint().x-pos.x,m_rectValidRgn.CenterPoint().y-pos.y);
	rgn_ucs.axis_x.Set(1,0,0);
	rgn_ucs.axis_y.Set(0,-1,0);
	rgn_ucs.axis_z.Set(0,0,-1);
	int wide = m_rectValidRgn.Width();
	int high = m_rectValidRgn.Height();
	rgn_pos.x = m_rectValidRgn.left;
	rgn_pos.y = m_rectValidRgn.top;
	ScreenToClient(&rgn_pos);
	m_rectValidRgn.left = rgn_pos.x;
	m_rectValidRgn.top  = rgn_pos.y;
	m_rectValidRgn.right  = rgn_pos.x+wide;
	m_rectValidRgn.bottom = rgn_pos.y+high;
	//生成画笔
	if(solid_pen.GetSafeHandle()==0)
		solid_pen.CreatePen(PS_SOLID,1,RGB(0,0,200));
	if(wide_solid_pen.GetSafeHandle()==0)
		wide_solid_pen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	if(green_solid_pen.GetSafeHandle()==0)
		green_solid_pen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	if(green_dash_pen.GetSafeHandle()==0)
		green_dash_pen.CreatePen(PS_DASH,1,RGB(0,255,0));
	if(red_solid_pen.GetSafeHandle()==0)
		red_solid_pen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	if(dash_pen.GetSafeHandle()==0)
		dash_pen.CreatePen(PS_DASH,1,RGB(0,0,0));
	if(dash_dot_pen.GetSafeHandle()==0)
		dash_dot_pen.CreatePen(PS_DASHDOT,1,RGB(255,0,0));
	//dijiaols_list.Empty();
	InitGraph();
	m_hCursor = m_hOldCursor = AfxGetApp()->LoadCursor(IDC_ARROW_NORMAL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFootDesignDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	/*if(m_pDraw)
	{
		CDrawEngine::Destroy(m_pDraw->GetEngineHandle());
		m_pDraw=NULL;
	}*/
}

//转换超出顶点显示边界很多的二维线为有效线(否则WINDOWS显示不正常)
static void Valid2dLine(f2dLine &line,CRect rect)
{
	double angle;
	angle = Cal2dLineAng(line);
	if(line.startPt.x<rect.left)
	{
		line.startPt.y -= (line.startPt.x-rect.left)*tan(angle);
		line.startPt.x  = rect.left;
	}
	else if(line.startPt.x>rect.right)
	{
		line.startPt.y -= (line.startPt.x-rect.right)*tan(angle);
		line.startPt.x  = rect.right;
	}
	if(line.startPt.y<rect.top)
	{
		line.startPt.x-= (line.startPt.y-rect.top)*tan(Pi/2-angle);
		line.startPt.y = rect.top;
	}
	else if(line.startPt.y>rect.bottom)
	{
		line.startPt.x -= (line.startPt.y-rect.bottom)*tan(Pi/2-angle);
		line.startPt.y  = rect.bottom;
	}
	if(line.endPt.x<rect.left)
	{
		line.endPt.y -= (line.endPt.x-rect.left)*tan(angle);
		line.endPt.x  = rect.left;
	}
	else if(line.endPt.x>rect.right)
	{
		line.endPt.y -= (line.endPt.x-rect.right)*tan(angle);
		line.endPt.x  = rect.right;
	}
	if(line.endPt.y<rect.top)
	{
		line.endPt.x-= (line.endPt.y-rect.top)*tan(Pi/2-angle);
		line.endPt.y = rect.top;
	}
	else if(line.endPt.y>rect.bottom)
	{
		line.endPt.x -= (line.endPt.y-rect.bottom)*tan(Pi/2-angle);
		line.endPt.y = rect.bottom;
	}
}
static BOOL IsInvalidLine(f2dLine line_2d, CRect rect)
{
	if(line_2d.startPt.x<rect.left&&line_2d.endPt.x<rect.left)
		return TRUE;
	else if(line_2d.startPt.y<rect.top&&line_2d.endPt.y<rect.top)
		return TRUE;
	else if(line_2d.startPt.x>rect.right&&line_2d.endPt.x>rect.right)
		return TRUE;
	else if(line_2d.startPt.y>rect.bottom&&line_2d.endPt.y>rect.bottom)
		return TRUE;
	return FALSE;
}
void CFootDesignDlg::OnBnAddDijiaoLs() 
{
	static CAddDiJiaoLsDlg AddDiJiaoLs_dlg;
	int d=QueryDiBanParaByWide(foot_para.m_iWide,'G',m_pFoot->lj_by_weld);
	AddDiJiaoLs_dlg.m_sLsGuiGe.Format("%d",d);
	AddDiJiaoLs_dlg.m_fHoleD=QueryDiBanParaByWide(foot_para.m_iWide,'D',m_pFoot->lj_by_weld);
	AddDiJiaoLs_dlg.m_nDefaultS=QueryDiBanParaByWide(foot_para.m_iWide,'S',m_pFoot->lj_by_weld);
	if(fabs(AddDiJiaoLs_dlg.m_fX)<eps&&fabs(AddDiJiaoLs_dlg.m_fY)<eps)
		AddDiJiaoLs_dlg.m_fX=AddDiJiaoLs_dlg.m_fY=AddDiJiaoLs_dlg.m_nDefaultS;
	if(AddDiJiaoLs_dlg.DoModal()==IDOK)
	{
		f3dCircle ls_cir;
		char guige[4];
		if(AddDiJiaoLs_dlg.m_sLsGuiGe.GetLength()>3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The bolt's specification is wrong,The input string may be too long");
#else 
			AfxMessageBox("错误的螺栓规格,可能是输入字符串太长");
#endif
			return;
		}
		else
			_snprintf(guige,3,"%s",AddDiJiaoLs_dlg.m_sLsGuiGe);
		DIJIAO_LS ls;
		f3dPoint origin;
		ls_cir.centre.Set(AddDiJiaoLs_dlg.m_fX,AddDiJiaoLs_dlg.m_fY,0);
		double len=ls_cir.centre.mod();
		double angle=ACOS(AddDiJiaoLs_dlg.m_fX/len);
		if(AddDiJiaoLs_dlg.m_fY<0)
			angle*=-1.0;	//Y坐标为负 起始角也应该取负
		ls_cir.radius = AddDiJiaoLs_dlg.m_fHoleD/2.0;
		ls_cir.feature = atoi(guige);
		ls.d = ls_cir.feature;
		ls.hole_d = (int)(ls_cir.radius*2);
		for(int i=0;i<AddDiJiaoLs_dlg.m_nMirN;i++)
		{
			ls_cir.centre.Set(AddDiJiaoLs_dlg.m_fX,AddDiJiaoLs_dlg.m_fY,0);
			if(i>=0)
				ls_cir.centre=GetPtInPolar(origin,angle+2*i*Pi/AddDiJiaoLs_dlg.m_nMirN,len);
			ent_base.m_arrCircle3d.append(ls_cir);
			
			ls.pos = ls_cir.centre;
			dijiaols_list.append(ls);
		}
		RefreshRgn();
	}
}

CAddLeiBanDlg AddLeiBanDlg;
void CFootDesignDlg::OnBnAddLeiBan() 
{
	AddLeiBanDlg.m_bCircleDiBan=foot_para.m_iDiBanShape;
	if(AddLeiBanDlg.DoModal()==IDOK)
	{
		LEI_BAN leiban;
		leiban.cMaterial=CSteelMatLibrary::RecordAt(AddLeiBanDlg.m_iMaterial).cBriefMark ;
		leiban.fGradientAngle = AddLeiBanDlg.m_fGradientAngle;
		leiban.fRayDist=AddLeiBanDlg.m_fLeiBanRayDist;
		leiban.wide=AddLeiBanDlg.m_fLeiBanWide;
		leiban.thick = AddLeiBanDlg.m_iThick;
		leiban.high  = AddLeiBanDlg.m_iHigh;
		leiban.A=AddLeiBanDlg.m_nParaA;
		leiban.B=AddLeiBanDlg.m_nParaB;
		leiban.bEnableSecondDatumXueBan=AddLeiBanDlg.m_bEnableSecondXueBan;
		leiban.water_slot_wide=AddLeiBanDlg.m_fWaterSlotWide;
		leiban.datumXueBanAngle=atof(AddLeiBanDlg.m_sDatumXueBan);
		_snprintf(leiban.sPartNo,MAX_CHAR_GUIGE_16,"%s",AddLeiBanDlg.m_sPartNo);
		leiban_list.append(leiban);
		InitGraph();
		InvalidateRect(&m_rectValidRgn);
	}
	
}
void ModifyLeiBan(LEI_BAN *pLeiBan)
{
	if(pLeiBan==NULL)
		return;
	CAddLeiBanDlg dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	dlg.m_sCaption = "Modify Rid Plate";
#else 
	dlg.m_sCaption = "修改肋板";
#endif
	dlg.m_fLeiBanRayDist = pLeiBan->fRayDist;
	dlg.m_fLeiBanWide = pLeiBan->wide;
	dlg.m_iThick  = pLeiBan->thick;
	dlg.m_iHigh   = pLeiBan->high;		//终点Z坐标存储了肋板的高度
	dlg.m_sPartNo = pLeiBan->sPartNo;
	dlg.m_nParaA  = pLeiBan->A;
	dlg.m_nParaB  = pLeiBan->B;
	dlg.m_fGradientAngle = pLeiBan->fGradientAngle;
	dlg.m_iMaterial=QuerySteelMatIndex(pLeiBan->cMaterial);
	dlg.m_fWaterSlotWide = pLeiBan->water_slot_wide;
	dlg.m_bEnableSecondXueBan = pLeiBan->bEnableSecondDatumXueBan;
	if(pLeiBan->datumXueBanAngle-(int)pLeiBan->datumXueBanAngle>0)	//肋板基准靴板角度带小数,将被认为是圆形底板
		dlg.m_bCircleDiBan=TRUE;
	dlg.m_sDatumXueBan.Format("%.1f",fto_halfi(pLeiBan->datumXueBanAngle));
	SimplifiedNumString(dlg.m_sDatumXueBan);
	if(dlg.DoModal()==IDOK)
	{
		pLeiBan->fRayDist = dlg.m_fLeiBanRayDist;
		pLeiBan->wide = dlg.m_fLeiBanWide;
		pLeiBan->thick	 = dlg.m_iThick;  
		pLeiBan->high	 = dlg.m_iHigh;   	//终点Z坐标存储了肋板的高度
		pLeiBan->A		 = dlg.m_nParaA;
		pLeiBan->B		 = dlg.m_nParaB;
		pLeiBan->fGradientAngle = dlg.m_fGradientAngle;
		_snprintf(pLeiBan->sPartNo,MAX_CHAR_GUIGE_16,"%s",dlg.m_sPartNo);
		pLeiBan->cMaterial=CSteelMatLibrary::RecordAt(dlg.m_iMaterial).cBriefMark;
		pLeiBan->water_slot_wide = dlg.m_fWaterSlotWide;
		pLeiBan->bEnableSecondDatumXueBan = dlg.m_bEnableSecondXueBan;
		pLeiBan->datumXueBanAngle=atof(dlg.m_sDatumXueBan);
	}
}
void CFootDesignDlg::OnBnModifyLeiBan() 
{
	oper_type = OPER_OSNAP;
	snap_type = SNAP_LINE;
	m_iCurOper = 2;
	UpdateData(FALSE);
	m_hCursor = AfxGetApp()->LoadCursor(IDC_ARROW_OSNAP);
}
void ModifyLsCir(DIJIAO_LS *pLs)
{
	if(pLs==NULL)
		return;
	CAddDiJiaoLsDlg dlg;
	dlg.m_fX = pLs->pos.x;
	dlg.m_fY = pLs->pos.y;
	dlg.m_fHoleD = pLs->hole_d;
	dlg.m_sLsGuiGe.Format("%d",pLs->d);
#ifdef AFX_TARG_ENU_ENGLISH
	dlg.m_sCaption = "Modify Foot Bolt";
#else 
	dlg.m_sCaption = "修改底脚螺栓";
#endif
	dlg.m_bAddLs=FALSE;	//只修改不添加，不需输入对称指数
	if(dlg.DoModal()==IDOK)
	{
		pLs->pos.Set(dlg.m_fX,dlg.m_fY,0);
		pLs->hole_d = dlg.m_fHoleD;
		char guige[4];
		if(dlg.m_sLsGuiGe.GetLength()>3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The bolt's specification is wrong,The input string may be too long");
#else 
			AfxMessageBox("错误的螺栓规格,可能是输入字符串太长");
#endif
			return;
		}
		else
			_snprintf(guige,MAX_CHAR_GUIGE_16,"%s",dlg.m_sLsGuiGe);
		pLs->d = atoi(guige);
	}
}

void CFootDesignDlg::OnBnModifyLs() 
{
	oper_type = OPER_OSNAP;
	snap_type = SNAP_CIRCLE;
	m_iCurOper = 2;
	UpdateData(FALSE);
	m_hCursor = AfxGetApp()->LoadCursor(IDC_ARROW_OSNAP);
	SetCursor(m_hCursor);
}

void CFootDesignDlg::OnBnParamSet() 
{
	static CFootParaSetDlg FootParaSetDlg;
	FootParaSetDlg.m_iDiBanMaterial=QuerySteelMatIndex(foot_para.m_cDiBanMat);
	FootParaSetDlg.m_iXueBanMaterial=QuerySteelMatIndex(foot_para.m_cXueBanMat);
	CLDSLineAngle *pUpBaseJg=NULL,*pDownBaseJg=NULL;
	GetBaseJg(pUpBaseJg,pDownBaseJg);
	if(pUpBaseJg&&fabs(pUpBaseJg->GetWingVecX()*m_pFoot->ucs.axis_x)>fabs(pUpBaseJg->GetWingVecY()*m_pFoot->ucs.axis_x))
		m_bUpAxisXSameToBaseWingX=TRUE;
	if(pDownBaseJg&&fabs(pDownBaseJg->GetWingVecX()*m_pFoot->ucs.axis_x)>fabs(pDownBaseJg->GetWingVecY()*m_pFoot->ucs.axis_x))
		m_bDownAxisXSameToBaseWingX=TRUE;
	FootParaSetDlg.m_iLjStyle = foot_para.m_iLjStyle;
	FootParaSetDlg.m_iWide = foot_para.m_iWide;
	FootParaSetDlg.m_iThick = foot_para.m_iThick;
	FootParaSetDlg.m_iUpXueBanThick = foot_para.m_iUpXueBanThick;
	FootParaSetDlg.m_iDownXueBanThick = foot_para.m_iDownXueBanThick;
	FootParaSetDlg.m_iA = foot_para.m_iA;
	FootParaSetDlg.m_iB = foot_para.m_iB;
	FootParaSetDlg.m_iUpC = foot_para.m_iUpC;
	FootParaSetDlg.m_iDownC = foot_para.m_iDownC;
	FootParaSetDlg.m_iS = foot_para.m_iS;
	FootParaSetDlg.m_bWeldDiZuo = foot_para.m_bWeldDiZuo;
	FootParaSetDlg.m_pFoot = m_pFoot;
	FootParaSetDlg.m_iUpLsLayStyle=foot_para.m_iUpLsLayStyle;
	FootParaSetDlg.m_iDownLsLayStyle=foot_para.m_iDownLsLayStyle;
	if(m_pFoot->attach_jg_handle[0]>0x20&&m_pFoot->attach_jg_handle[1]>0x20)
		FootParaSetDlg.m_bUpAndDownSide=TRUE;
	else
		FootParaSetDlg.m_bUpAndDownSide=FALSE;
	FootParaSetDlg.m_iDiBanShape = foot_para.m_iDiBanShape;
	if(FootParaSetDlg.DoModal()==IDOK)
	{
		m_pFoot->lj_by_weld = foot_para.m_iLjStyle = FootParaSetDlg.m_iLjStyle;
		foot_para.m_cDiBanMat=CSteelMatLibrary::RecordAt(FootParaSetDlg.m_iDiBanMaterial).cBriefMark;
		foot_para.m_cXueBanMat=CSteelMatLibrary::RecordAt(FootParaSetDlg.m_iXueBanMaterial).cBriefMark;
		tm.m_iUpLsLayStyle		= FootParaSetDlg.m_iUpLsLayStyle;
		tm.m_iDownLsLayStyle	= FootParaSetDlg.m_iDownLsLayStyle;
		foot_para.m_iWide		= FootParaSetDlg.m_iWide;
		foot_para.m_iThick		= FootParaSetDlg.m_iThick;
		if(!FootParaSetDlg.m_bBreakUpXueBan0C)
			FootParaSetDlg.m_sUpXueBanPartNo0C=FootParaSetDlg.m_sUpXueBanPartNo180C;
		if(!FootParaSetDlg.m_bBreakUpXueBan90C)
			FootParaSetDlg.m_sUpXueBanPartNo90C=FootParaSetDlg.m_sUpXueBanPartNo270C;
		if(!FootParaSetDlg.m_bBreakDownXueBan0C)
			FootParaSetDlg.m_sDownXueBanPartNo0C=FootParaSetDlg.m_sDownXueBanPartNo180C;
		if(!FootParaSetDlg.m_bBreakDownXueBan90C)
			FootParaSetDlg.m_sDownXueBanPartNo90C=FootParaSetDlg.m_sDownXueBanPartNo270C;

		//从角钢肢向底脚板坐标轴的方向标定转换
		if(m_bUpAxisXSameToBaseWingX)
		{
			_snprintf(foot_para.sUpXueBanPartNo180C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo0C);
			_snprintf(foot_para.sUpXueBanPartNo135C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo45C);
			_snprintf(foot_para.sUpXueBanPartNo90C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo90C);
			_snprintf(foot_para.sUpXueBanPartNo45C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo135C);
			_snprintf(foot_para.sUpXueBanPartNo0C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo180C);
			_snprintf(foot_para.sUpXueBanPartNo315C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo225C);
			_snprintf(foot_para.sUpXueBanPartNo270C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo270C);
			_snprintf(foot_para.sUpXueBanPartNo225C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo315C);
			foot_para.m_bBreakUpXueBan0C=FootParaSetDlg.m_bBreakUpXueBan0C;
			foot_para.m_bBreakUpXueBan90C=FootParaSetDlg.m_bBreakUpXueBan90C;
		}
		else
		{
			_snprintf(foot_para.sUpXueBanPartNo270C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo0C);
			_snprintf(foot_para.sUpXueBanPartNo315C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo45C);
			_snprintf(foot_para.sUpXueBanPartNo0C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo90C);
			_snprintf(foot_para.sUpXueBanPartNo45C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo135C);
			_snprintf(foot_para.sUpXueBanPartNo90C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo180C);
			_snprintf(foot_para.sUpXueBanPartNo135C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo225C);
			_snprintf(foot_para.sUpXueBanPartNo180C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo270C);
			_snprintf(foot_para.sUpXueBanPartNo225C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sUpXueBanPartNo315C);
			foot_para.m_bBreakUpXueBan0C=FootParaSetDlg.m_bBreakUpXueBan90C;
			foot_para.m_bBreakUpXueBan90C=FootParaSetDlg.m_bBreakUpXueBan0C;
		}
		if(m_bDownAxisXSameToBaseWingX)
		{
			_snprintf(foot_para.sDownXueBanPartNo180C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo0C);
			_snprintf(foot_para.sDownXueBanPartNo135C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo45C);
			_snprintf(foot_para.sDownXueBanPartNo90C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo90C);
			_snprintf(foot_para.sDownXueBanPartNo45C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo135C);
			_snprintf(foot_para.sDownXueBanPartNo0C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo180C);
			_snprintf(foot_para.sDownXueBanPartNo315C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo225C);
			_snprintf(foot_para.sDownXueBanPartNo270C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo270C);
			_snprintf(foot_para.sDownXueBanPartNo225C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo315C);
			foot_para.m_bBreakDownXueBan0C=FootParaSetDlg.m_bBreakDownXueBan0C;
			foot_para.m_bBreakDownXueBan90C=FootParaSetDlg.m_bBreakDownXueBan90C;
		}
		else
		{
			_snprintf(foot_para.sDownXueBanPartNo270C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo0C);
			_snprintf(foot_para.sDownXueBanPartNo315C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo45C);
			_snprintf(foot_para.sDownXueBanPartNo0C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo90C);
			_snprintf(foot_para.sDownXueBanPartNo45C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo135C);
			_snprintf(foot_para.sDownXueBanPartNo90C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo180C);
			_snprintf(foot_para.sDownXueBanPartNo135C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo225C);
			_snprintf(foot_para.sDownXueBanPartNo180C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo270C);
			_snprintf(foot_para.sDownXueBanPartNo225C,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDownXueBanPartNo315C);
			foot_para.m_bBreakDownXueBan0C=FootParaSetDlg.m_bBreakDownXueBan90C;
			foot_para.m_bBreakDownXueBan90C=FootParaSetDlg.m_bBreakDownXueBan0C;
		}
		foot_para.m_iUpXueBanThick = FootParaSetDlg.m_iUpXueBanThick;
		foot_para.m_iDownXueBanThick = FootParaSetDlg.m_iDownXueBanThick;
		foot_para.m_iA = FootParaSetDlg.m_iA;
		foot_para.m_iB = FootParaSetDlg.m_iB;
		foot_para.m_iUpC = FootParaSetDlg.m_iUpC;
		foot_para.m_iDownC = FootParaSetDlg.m_iDownC;
		foot_para.m_iS = FootParaSetDlg.m_iS;
		foot_para.m_iUpLsLayStyle = FootParaSetDlg.m_iUpLsLayStyle;
		foot_para.m_iDownLsLayStyle = FootParaSetDlg.m_iDownLsLayStyle;
		foot_para.m_bWeldDiZuo = FootParaSetDlg.m_bWeldDiZuo;
		foot_para.m_iDiBanShape = FootParaSetDlg.m_iDiBanShape;
		_snprintf(foot_para.sDiBanPartNo,MAX_CHAR_GUIGE_16,"%s",FootParaSetDlg.m_sDiBanPartNo);
	}
	InitGraph();//	为了动态显示角钢与底板相对位置 -wjh 2003.08.11
	Invalidate();
}

void CFootDesignDlg::RefreshCurOper()
{
	UpdateData(TRUE);
	switch(m_iCurOper)
	{
	case 0:
		oper_type=OPER_PAN;
		m_hCursor = AfxGetApp()->LoadCursor(IDC_ARROW_PAN_MOVE);
		break;
	case 1:
		oper_type=OPER_ZOOM;
		m_hCursor = AfxGetApp()->LoadCursor(IDC_ARROW_ZOOM);
		break;
	case 2:
		oper_type=OPER_OSNAP;
		m_hCursor = AfxGetApp()->LoadCursor(IDC_ARROW_OSNAP);
		break;
	default:
		oper_type=OPER_OTHER;	//空闲
		m_hCursor = AfxGetApp()->LoadCursor(IDC_ARROW_NORMAL);
		break;
	}
	SetCursor(m_hCursor);
}

void CFootDesignDlg::RefreshRgn()
{
	CRect refresh_rect(m_rectValidRgn.left+1,m_rectValidRgn.top+1,m_rectValidRgn.right-1,m_rectValidRgn.bottom-1);
	InvalidateRect(refresh_rect);
}

struct XUEBANLINE{
	f3dLine upLine,btmLine;
};

void CFootDesignDlg::InitGraph()
{
	f3dRect rect;
	double wing_wide,wing_thick;
	f3dPoint vertex[6],xueban_vertex[4];
	ent_base.Empty();

	if(foot_para.m_iDiBanShape==1)
	{
		f3dCircle circle;
		circle.radius=foot_para.m_iWide/2.0;
		circle.norm.Set(0,0,1);
		ent_base.m_arrCircle3d.append(circle);
	}
	else
	{
		rect.SetRect(f3dPoint(-foot_para.m_iWide/2.0, foot_para.m_iWide/2.0,0),
			f3dPoint(foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0,0));
		ent_base.m_arrRect3d.append(rect);
	}
	XUEBANLINE upXueBan[8],downXueBan[8];
	//绘制角钢截面在底板上的投影图
	for(int i=0;i<2;i++)
	{
		CLDSLineAngle *pJg=(CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[i],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pJg==NULL)
			continue;
		wing_wide = pJg->GetWidth();
		wing_thick = pJg->GetThick();
		f3dPoint axis_z,wing_vec_x,wing_vec_y;
		if(pJg->pStart->handle==m_pFoot->base_node_handle)
			axis_z = pJg->End()-pJg->Start();
		else
			axis_z = pJg->Start()-pJg->End();
		normalize(axis_z);
		BOOL bUpJg;	//判断角钢在底板上还是下
		if(axis_z*m_pFoot->ucs.axis_z>0)	//上侧角钢
			bUpJg=TRUE;
		else
			bUpJg=FALSE;
		int XueBanThick;
		if(bUpJg)
			XueBanThick=foot_para.m_iUpXueBanThick;
		else
			XueBanThick=foot_para.m_iDownXueBanThick;
		wing_vec_x = pJg->GetWingVecX();
		wing_vec_y = pJg->GetWingVecY();
		CLDSLineAngle *pComJgArr[4]={NULL};
		if(pJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pJg;
			pComJgArr[0]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
			pComJgArr[1]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
			if(pGroupJg->group_style>1)	//四角钢组合
			{
				pComJgArr[2]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
				pComJgArr[3]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
			}
		}
		else
			pComJgArr[0]=pJg;
		for(int j=3;j>=0;j--)
		{	//逆序因为第一根角钢往往是组合角钢的特征（基准）角钢
			if(pComJgArr[j]==NULL)
				continue;
			if(pComJgArr[j]->pSolidBody==NULL)
				pComJgArr[j]->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			int k;
			for(k=0;k<6;k++)
			{
				Int3dlf(vertex[k],pComJgArr[j]->pSolidBody->GetKeyPointAt(k),axis_z,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
				coord_trans(vertex[k],m_pFoot->ucs,FALSE);
			}
			for(k=0;k<6;k++)
			{
				f3dLine line;
				line.startPt = vertex[k];
				line.endPt   = vertex[(k+1)%6];
				if(bUpJg)
					line.pen.style=PS_SOLID;
				else
					line.pen.style=PS_DASH;
				ent_base.m_arrLine3d.append(line);
			}
		}
		//绘制靴板截面在底板上的投影图
		f2dLine cut_line;
		f3dLine line;
		if(m_pFoot->lj_by_weld)	//靴板焊接到主材角钢上
		{
			if(fabs(dot_prod(wing_vec_x,m_pFoot->ucs.axis_y))>cos45)
			{	//主材角钢X肢与底板X轴一致
				//X轴向的靴板
				if(foot_para.m_iDiBanShape==0)
				{	//方形底板
					cut_line.startPt.Set(foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0);
					cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					Int2dll(f2dLine(vertex[0],vertex[1]),cut_line,xueban_vertex[0].x,xueban_vertex[0].y);
					Int2dll(f2dLine(vertex[2],vertex[3]),cut_line,xueban_vertex[1].x,xueban_vertex[1].y);
					cut_line.startPt.x = -cut_line.startPt.x;
					cut_line.endPt.x = -cut_line.endPt.x;
					Int2dll(f2dLine(vertex[2],vertex[3]),cut_line,xueban_vertex[2].x,xueban_vertex[2].y);
					Int2dll(f2dLine(vertex[0],vertex[1]),cut_line,xueban_vertex[3].x,xueban_vertex[3].y);
				}
				else
				{	//圆形底板
					f2dCircle circle;
					circle.radius=foot_para.m_iWide/2.0;
					Int2dlc(f2dLine(vertex[0],vertex[1]),circle,xueban_vertex[0].x,xueban_vertex[0].y,xueban_vertex[3].x,xueban_vertex[3].y);
					Int2dlc(f2dLine(vertex[2],vertex[3]),circle,xueban_vertex[1].x,xueban_vertex[1].y,xueban_vertex[2].x,xueban_vertex[2].y);
				}
				for(int k=0;k<4;k++)
				{
					line.startPt = xueban_vertex[k];
					line.endPt   = xueban_vertex[(k+1)%4];
					if(bUpJg)
						line.pen.style=PS_SOLID;
					else
						line.pen.style=PS_DASH;
					ent_base.m_arrLine3d.append(line);
				}
				if(xueban_vertex[0].x>0)
				{
					if(bUpJg)
					{
						upXueBan[0].upLine.endPt=xueban_vertex[0];
						upXueBan[0].upLine.startPt.Set(0,upXueBan[0].upLine.endPt.y);
						upXueBan[4].btmLine.endPt=xueban_vertex[3];
						upXueBan[4].btmLine.startPt.Set(0,xueban_vertex[3].y);
					}
					else
					{
						downXueBan[0].upLine.endPt=xueban_vertex[0];
						downXueBan[0].upLine.startPt.Set(0,xueban_vertex[0].y);
						downXueBan[4].btmLine.endPt=xueban_vertex[3];
						downXueBan[4].btmLine.startPt.Set(0,xueban_vertex[3].y);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[0].upLine.endPt=xueban_vertex[3];
						upXueBan[0].upLine.startPt.Set(0,xueban_vertex[3].y);
						upXueBan[4].btmLine.endPt=xueban_vertex[0];
						upXueBan[4].btmLine.startPt.Set(0,xueban_vertex[0].y);
					}
					else
					{
						downXueBan[0].upLine.endPt=xueban_vertex[3];
						downXueBan[0].upLine.startPt.Set(0,xueban_vertex[3].y);
						downXueBan[4].btmLine.endPt=xueban_vertex[0];
						downXueBan[4].btmLine.startPt.Set(0,xueban_vertex[0].y);
					}
				}
				if(xueban_vertex[1].x>0)
				{
					if(bUpJg)
					{
						upXueBan[0].btmLine.endPt=xueban_vertex[1];
						upXueBan[0].btmLine.startPt.Set(0,upXueBan[1].upLine.endPt.y);
						upXueBan[4].upLine.endPt=xueban_vertex[2];
						upXueBan[4].upLine.startPt.Set(0,xueban_vertex[2].y);
					}
					else
					{
						downXueBan[0].btmLine.endPt=xueban_vertex[1];
						downXueBan[0].btmLine.startPt.Set(0,xueban_vertex[1].y);
						downXueBan[4].upLine.endPt=xueban_vertex[2];
						downXueBan[4].upLine.startPt.Set(0,xueban_vertex[2].y);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[0].btmLine.endPt=xueban_vertex[2];
						upXueBan[0].btmLine.startPt.Set(0,xueban_vertex[2].y);
						upXueBan[4].upLine.endPt=xueban_vertex[1];
						upXueBan[4].upLine.startPt.Set(0,xueban_vertex[1].y);
					}
					else
					{
						downXueBan[0].btmLine.endPt=xueban_vertex[2];
						downXueBan[0].btmLine.startPt.Set(0,xueban_vertex[2].y);
						downXueBan[4].upLine.endPt=xueban_vertex[1];
						downXueBan[4].upLine.startPt.Set(0,xueban_vertex[1].y);
					}
				}

				//Y轴向的靴板
				if(foot_para.m_iDiBanShape==0)
				{	//方形底板
					cut_line.startPt.Set(-foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					Int2dll(f2dLine(vertex[0],vertex[5]),cut_line,xueban_vertex[0].x,xueban_vertex[0].y);
					Int2dll(f2dLine(vertex[4],vertex[3]),cut_line,xueban_vertex[1].x,xueban_vertex[1].y);
					cut_line.startPt.y = -cut_line.startPt.y;
					cut_line.endPt.y = -cut_line.endPt.y;
					Int2dll(f2dLine(vertex[4],vertex[3]),cut_line,xueban_vertex[2].x,xueban_vertex[2].y);
					Int2dll(f2dLine(vertex[0],vertex[5]),cut_line,xueban_vertex[3].x,xueban_vertex[3].y);
				}
				else
				{	//圆形底板
					f2dCircle circle;
					circle.radius=foot_para.m_iWide/2.0;
					Int2dlc(f2dLine(vertex[0],vertex[5]),circle,xueban_vertex[0].x,xueban_vertex[0].y,xueban_vertex[3].x,xueban_vertex[3].y);
					Int2dlc(f2dLine(vertex[4],vertex[3]),circle,xueban_vertex[1].x,xueban_vertex[1].y,xueban_vertex[2].x,xueban_vertex[2].y);
				}
				if(xueban_vertex[0].y>0)
				{
					if(bUpJg)
					{
						upXueBan[2].upLine.endPt=xueban_vertex[0];
						upXueBan[2].upLine.startPt.Set(xueban_vertex[0].x,0);
						upXueBan[6].btmLine.endPt=xueban_vertex[3];
						upXueBan[6].btmLine.startPt.Set(xueban_vertex[3].x,0);
					}
					else
					{
						downXueBan[2].upLine.endPt=xueban_vertex[0];
						downXueBan[2].upLine.startPt.Set(xueban_vertex[0].x,0);
						downXueBan[6].btmLine.endPt=xueban_vertex[3];
						downXueBan[6].btmLine.startPt.Set(xueban_vertex[3].x,0);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[2].upLine.endPt=xueban_vertex[3];
						upXueBan[2].upLine.startPt.Set(xueban_vertex[3].x,0);
						upXueBan[6].btmLine.endPt=xueban_vertex[0];
						upXueBan[6].btmLine.startPt.Set(xueban_vertex[0].x,0);
					}
					else
					{
						downXueBan[2].upLine.endPt=xueban_vertex[3];
						downXueBan[2].upLine.startPt.Set(xueban_vertex[3].x,0);
						downXueBan[6].btmLine.endPt=xueban_vertex[0];
						downXueBan[6].btmLine.startPt.Set(xueban_vertex[0].x,0);
					}
				}
				if(xueban_vertex[1].y>0)
				{
					if(bUpJg)
					{
						upXueBan[2].btmLine.endPt=xueban_vertex[0];
						upXueBan[2].btmLine.startPt.Set(xueban_vertex[0].x,0);
						upXueBan[6].upLine.endPt=xueban_vertex[3];
						upXueBan[6].upLine.startPt.Set(xueban_vertex[3].x,0);
					}
					else
					{
						downXueBan[2].btmLine.endPt=xueban_vertex[0];
						downXueBan[2].btmLine.startPt.Set(xueban_vertex[0].x,0);
						downXueBan[6].upLine.endPt=xueban_vertex[3];
						downXueBan[6].upLine.startPt.Set(xueban_vertex[3].x,0);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[2].btmLine.endPt=xueban_vertex[3];
						upXueBan[2].btmLine.startPt.Set(xueban_vertex[3].x,0);
						upXueBan[6].upLine.endPt=xueban_vertex[0];
						upXueBan[6].upLine.startPt.Set(xueban_vertex[0].x,0);
					}
					else
					{
						downXueBan[2].btmLine.endPt=xueban_vertex[3];
						downXueBan[2].btmLine.startPt.Set(xueban_vertex[3].x,0);
						downXueBan[6].upLine.endPt=xueban_vertex[0];
						downXueBan[6].upLine.startPt.Set(xueban_vertex[0].x,0);
					}
				}
				for(k=0;k<4;k++)
				{
					line.startPt = xueban_vertex[k];
					line.endPt   = xueban_vertex[(k+1)%4];
					if(bUpJg)
						line.pen.style=PS_SOLID;
					else
						line.pen.style=PS_DASH;
					ent_base.m_arrLine3d.append(line);
				}
			}
			else
			{	//主材角钢X肢与底板Y轴一致
				//Y轴向的靴板
				if(foot_para.m_iDiBanShape==0)
				{	//方形底板
					cut_line.startPt.Set(-foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					Int2dll(f2dLine(vertex[0],vertex[1]),cut_line,xueban_vertex[0].x,xueban_vertex[0].y);
					Int2dll(f2dLine(vertex[2],vertex[3]),cut_line,xueban_vertex[1].x,xueban_vertex[1].y);
					cut_line.startPt.y = -cut_line.startPt.y;
					cut_line.endPt.y = -cut_line.endPt.y;
					Int2dll(f2dLine(vertex[2],vertex[3]),cut_line,xueban_vertex[2].x,xueban_vertex[2].y);
					Int2dll(f2dLine(vertex[0],vertex[1]),cut_line,xueban_vertex[3].x,xueban_vertex[3].y);
				}
				else
				{	//圆形底板
					f2dCircle circle;
					circle.radius=foot_para.m_iWide/2.0;
					Int2dlc(f2dLine(vertex[0],vertex[1]),circle,xueban_vertex[0].x,xueban_vertex[0].y,xueban_vertex[3].x,xueban_vertex[3].y);
					Int2dlc(f2dLine(vertex[2],vertex[3]),circle,xueban_vertex[1].x,xueban_vertex[1].y,xueban_vertex[2].x,xueban_vertex[2].y);
				}
				if(xueban_vertex[0].y>0)
				{
					if(bUpJg)
					{
						upXueBan[2].upLine.endPt=xueban_vertex[0];
						upXueBan[2].upLine.startPt.Set(xueban_vertex[0].x,0);
						upXueBan[6].btmLine.endPt=xueban_vertex[3];
						upXueBan[6].btmLine.startPt.Set(xueban_vertex[3].x,0);
					}
					else
					{
						downXueBan[2].upLine.endPt=xueban_vertex[0];
						downXueBan[2].upLine.startPt.Set(xueban_vertex[0].x,0);
						downXueBan[6].btmLine.endPt=xueban_vertex[3];
						downXueBan[6].btmLine.startPt.Set(xueban_vertex[3].x,0);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[2].upLine.endPt=xueban_vertex[3];
						upXueBan[2].upLine.startPt.Set(xueban_vertex[3].x,0);
						upXueBan[6].btmLine.endPt=xueban_vertex[0];
						upXueBan[6].btmLine.startPt.Set(xueban_vertex[0].x,0);
					}
					else
					{
						downXueBan[2].upLine.endPt=xueban_vertex[3];
						downXueBan[2].upLine.startPt.Set(xueban_vertex[3].x,0);
						downXueBan[6].btmLine.endPt=xueban_vertex[0];
						downXueBan[6].btmLine.startPt.Set(xueban_vertex[0].x,0);
					}
				}
				if(xueban_vertex[1].y>0)
				{
					if(bUpJg)
					{
						upXueBan[2].btmLine.endPt=xueban_vertex[0];
						upXueBan[2].btmLine.startPt.Set(xueban_vertex[0].x,0);
						upXueBan[6].upLine.endPt=xueban_vertex[3];
						upXueBan[6].upLine.startPt.Set(xueban_vertex[3].x,0);
					}
					else
					{
						downXueBan[2].btmLine.endPt=xueban_vertex[0];
						downXueBan[2].btmLine.startPt.Set(xueban_vertex[0].x,0);
						downXueBan[6].upLine.endPt=xueban_vertex[3];
						downXueBan[6].upLine.startPt.Set(xueban_vertex[3].x,0);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[2].btmLine.endPt=xueban_vertex[3];
						upXueBan[2].btmLine.startPt.Set(xueban_vertex[3].x,0);
						upXueBan[6].upLine.endPt=xueban_vertex[0];
						upXueBan[6].upLine.startPt.Set(xueban_vertex[0].x,0);
					}
					else
					{
						downXueBan[2].btmLine.endPt=xueban_vertex[3];
						downXueBan[2].btmLine.startPt.Set(xueban_vertex[3].x,0);
						downXueBan[6].upLine.endPt=xueban_vertex[0];
						downXueBan[6].upLine.startPt.Set(xueban_vertex[0].x,0);
					}
				}
				int k;
				for(k=0;k<4;k++)
				{
					line.startPt = xueban_vertex[k];
					line.endPt   = xueban_vertex[(k+1)%4];
					if(bUpJg)
						line.pen.style=PS_SOLID;
					else
						line.pen.style=PS_DASH;
					ent_base.m_arrLine3d.append(line);
				}
				//X轴向的靴板
				if(foot_para.m_iDiBanShape==0)
				{	//方形底板
					cut_line.startPt.Set(foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0);
					cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					Int2dll(f2dLine(vertex[0],vertex[5]),cut_line,xueban_vertex[0].x,xueban_vertex[0].y);
					Int2dll(f2dLine(vertex[4],vertex[3]),cut_line,xueban_vertex[1].x,xueban_vertex[1].y);
					cut_line.startPt.x = -cut_line.startPt.x;
					cut_line.endPt.x = -cut_line.endPt.x;
					Int2dll(f2dLine(vertex[4],vertex[3]),cut_line,xueban_vertex[2].x,xueban_vertex[2].y);
					Int2dll(f2dLine(vertex[0],vertex[5]),cut_line,xueban_vertex[3].x,xueban_vertex[3].y);
				}
				else
				{	//圆形底板
					f2dCircle circle;
					circle.radius=foot_para.m_iWide/2.0;
					Int2dlc(f2dLine(vertex[0],vertex[5]),circle,xueban_vertex[0].x,xueban_vertex[0].y,xueban_vertex[3].x,xueban_vertex[3].y);
					Int2dlc(f2dLine(vertex[4],vertex[3]),circle,xueban_vertex[1].x,xueban_vertex[1].y,xueban_vertex[2].x,xueban_vertex[2].y);
				}
				if(xueban_vertex[0].x>0)
				{
					if(bUpJg)
					{
						upXueBan[0].upLine.endPt=xueban_vertex[0];
						upXueBan[0].upLine.startPt.Set(0,xueban_vertex[0].y);
						upXueBan[4].btmLine.endPt=xueban_vertex[3];
						upXueBan[4].btmLine.startPt.Set(0,xueban_vertex[3].y);
					}
					else
					{
						downXueBan[0].upLine.endPt=xueban_vertex[0];
						downXueBan[0].upLine.startPt.Set(0,xueban_vertex[0].y);
						downXueBan[4].btmLine.endPt=xueban_vertex[3];
						downXueBan[4].btmLine.startPt.Set(0,xueban_vertex[3].y);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[0].upLine.endPt=xueban_vertex[3];
						upXueBan[0].upLine.startPt.Set(0,xueban_vertex[3].y);
						upXueBan[4].btmLine.endPt=xueban_vertex[0];
						upXueBan[4].btmLine.startPt.Set(0,xueban_vertex[0].y);
					}
					else
					{
						downXueBan[0].upLine.endPt=xueban_vertex[3];
						downXueBan[0].upLine.startPt.Set(0,xueban_vertex[3].y);
						downXueBan[4].btmLine.endPt=xueban_vertex[0];
						downXueBan[4].btmLine.startPt.Set(0,xueban_vertex[0].y);
					}
				}
				if(xueban_vertex[1].x>0)
				{
					if(bUpJg)
					{
						upXueBan[0].btmLine.endPt=xueban_vertex[0];
						upXueBan[0].btmLine.startPt.Set(0,xueban_vertex[0].y);
						upXueBan[4].upLine.endPt=xueban_vertex[3];
						upXueBan[4].upLine.startPt.Set(0,xueban_vertex[3].y);
					}
					else
					{
						downXueBan[0].btmLine.endPt=xueban_vertex[0];
						downXueBan[0].btmLine.startPt.Set(0,xueban_vertex[0].y);
						downXueBan[4].upLine.endPt=xueban_vertex[3];
						downXueBan[4].upLine.startPt.Set(0,xueban_vertex[3].y);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[0].btmLine.endPt=xueban_vertex[3];
						upXueBan[0].btmLine.startPt.Set(0,xueban_vertex[3].y);
						upXueBan[4].upLine.endPt=xueban_vertex[0];
						upXueBan[4].upLine.startPt.Set(0,xueban_vertex[0].y);
					}
					else
					{
						downXueBan[0].btmLine.endPt=xueban_vertex[3];
						downXueBan[0].btmLine.startPt.Set(0,xueban_vertex[3].y);
						downXueBan[4].upLine.endPt=xueban_vertex[0];
						downXueBan[4].upLine.startPt.Set(0,xueban_vertex[0].y);
					}
				}
				for(k=0;k<4;k++)
				{
					line.startPt = xueban_vertex[k];
					line.endPt   = xueban_vertex[(k+1)%4];
					if(bUpJg)
						line.pen.style=PS_SOLID;
					else
						line.pen.style=PS_DASH;
					ent_base.m_arrLine3d.append(line);
				}
			}
		}
		else					//靴板螺栓连接或搭接到主材角钢上
		{
			if(fabs(dot_prod(wing_vec_x,m_pFoot->ucs.axis_y))>cos45)
			{	//主材角钢X肢与底板Y轴一致
				//X轴向的靴板
				if(foot_para.m_iDiBanShape==0)
				{	//方形底板
					cut_line.startPt.Set(foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0);
					cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					Int2dll(f2dLine(vertex[0],vertex[1]),cut_line,xueban_vertex[0].x,xueban_vertex[0].y);
					cut_line.startPt.x = -cut_line.startPt.x;
					cut_line.endPt.x = -cut_line.endPt.x;
					Int2dll(f2dLine(vertex[0],vertex[1]),cut_line,xueban_vertex[1].x,xueban_vertex[1].y);
				}
				else
				{	//圆形底板
					f2dCircle circle;
					circle.radius=foot_para.m_iWide/2.0;
					Int2dlc(f2dLine(vertex[0],vertex[1]),circle,xueban_vertex[0].x,xueban_vertex[0].y,xueban_vertex[1].x,xueban_vertex[1].y);
				}
				f3dPoint norm = pJg->get_norm_y_wing();
				vector_trans(norm,m_pFoot->ucs,FALSE);
				double angle = cal_angle_of_2vec(norm,f3dPoint(norm.x,norm.y,0));
				norm = norm.normalized()*fabs(1.0/cos(angle));
				xueban_vertex[2] = xueban_vertex[1]+norm*XueBanThick;
				xueban_vertex[3] = xueban_vertex[0]+norm*XueBanThick;
				xueban_vertex[2].z=xueban_vertex[3].z=0;
				if(xueban_vertex[0].x>0)
				{
					if(bUpJg)
					{
						upXueBan[0].upLine.endPt=xueban_vertex[3];
						upXueBan[0].upLine.startPt.Set(0,xueban_vertex[3].y);
						upXueBan[0].btmLine.endPt=xueban_vertex[0];
						upXueBan[0].btmLine.startPt.Set(0,xueban_vertex[0].y);

						upXueBan[4].upLine.endPt=xueban_vertex[1];
						upXueBan[4].upLine.startPt.Set(0,xueban_vertex[1].y);
						upXueBan[4].btmLine.endPt=xueban_vertex[2];
						upXueBan[4].btmLine.startPt.Set(0,xueban_vertex[2].y);
					}
					else
					{
						downXueBan[0].upLine.endPt=xueban_vertex[3];
						downXueBan[0].upLine.startPt.Set(0,xueban_vertex[3].y);
						downXueBan[0].btmLine.endPt=xueban_vertex[0];
						downXueBan[0].btmLine.startPt.Set(0,xueban_vertex[0].y);

						downXueBan[4].upLine.endPt=xueban_vertex[1];
						downXueBan[4].upLine.startPt.Set(0,xueban_vertex[1].y);
						downXueBan[4].btmLine.endPt=xueban_vertex[2];
						downXueBan[4].btmLine.startPt.Set(0,xueban_vertex[2].y);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[0].upLine.endPt=xueban_vertex[2];
						upXueBan[0].upLine.startPt.Set(0,xueban_vertex[2].y);
						upXueBan[0].btmLine.endPt=xueban_vertex[1];
						upXueBan[0].btmLine.startPt.Set(0,xueban_vertex[1].y);

						upXueBan[4].upLine.endPt=xueban_vertex[0];
						upXueBan[4].upLine.startPt.Set(0,xueban_vertex[0].y);
						upXueBan[4].btmLine.endPt=xueban_vertex[3];
						upXueBan[4].btmLine.startPt.Set(0,xueban_vertex[3].y);
					}
					else
					{
						downXueBan[0].upLine.endPt=xueban_vertex[2];
						downXueBan[0].upLine.startPt.Set(0,xueban_vertex[2].y);
						downXueBan[0].btmLine.endPt=xueban_vertex[1];
						downXueBan[0].btmLine.startPt.Set(0,xueban_vertex[1].y);

						downXueBan[4].upLine.endPt=xueban_vertex[0];
						downXueBan[4].upLine.startPt.Set(0,xueban_vertex[0].y);
						downXueBan[4].btmLine.endPt=xueban_vertex[3];
						downXueBan[4].btmLine.startPt.Set(0,xueban_vertex[3].y);
					}
				}
				for(int k=0;k<4;k++)
				{
					line.startPt = xueban_vertex[k];
					line.endPt   = xueban_vertex[(k+1)%4];
					if(bUpJg)
						line.pen.style=PS_SOLID;
					else
						line.pen.style=PS_DASH;
					ent_base.m_arrLine3d.append(line);
				}
				//Y轴向的靴板
				if(foot_para.m_iDiBanShape==0)
				{	//方形底板
					cut_line.startPt.Set(-foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					Int2dll(f2dLine(vertex[0],vertex[5]),cut_line,xueban_vertex[0].x,xueban_vertex[0].y);
					cut_line.startPt.y = -cut_line.startPt.y;
					cut_line.endPt.y = -cut_line.endPt.y;
					Int2dll(f2dLine(vertex[0],vertex[5]),cut_line,xueban_vertex[1].x,xueban_vertex[1].y);
				}
				else
				{	//圆形底板
					f2dCircle circle;
					circle.radius=foot_para.m_iWide/2.0;
					Int2dlc(f2dLine(vertex[0],vertex[5]),circle,xueban_vertex[0].x,xueban_vertex[0].y,xueban_vertex[1].x,xueban_vertex[1].y);
				}
				norm = pJg->get_norm_x_wing();
				angle = cal_angle_of_2vec(norm,f3dPoint(norm.x,norm.y,0));
				norm = norm*fabs(1.0/cos(angle));
				vector_trans(norm,m_pFoot->ucs,FALSE);
				xueban_vertex[2] = xueban_vertex[1]+norm*XueBanThick;
				xueban_vertex[3] = xueban_vertex[0]+norm*XueBanThick;
				xueban_vertex[2].z=xueban_vertex[3].z=0;
				if(xueban_vertex[0].y>0)
				{
					if(bUpJg)
					{
						upXueBan[2].upLine.endPt=xueban_vertex[3];
						upXueBan[2].upLine.startPt.Set(xueban_vertex[3].x,0);
						upXueBan[2].btmLine.endPt=xueban_vertex[0];
						upXueBan[2].btmLine.startPt.Set(xueban_vertex[0].x,0);

						upXueBan[6].upLine.endPt=xueban_vertex[1];
						upXueBan[6].upLine.startPt.Set(xueban_vertex[1].x,0);
						upXueBan[6].btmLine.endPt=xueban_vertex[2];
						upXueBan[6].btmLine.startPt.Set(xueban_vertex[2].x,0);
					}
					else
					{
						downXueBan[2].upLine.endPt=xueban_vertex[3];
						downXueBan[2].upLine.startPt.Set(xueban_vertex[3].x,0);
						downXueBan[2].btmLine.endPt=xueban_vertex[0];
						downXueBan[2].btmLine.startPt.Set(xueban_vertex[0].x,0);

						downXueBan[6].upLine.endPt=xueban_vertex[1];
						downXueBan[6].upLine.startPt.Set(xueban_vertex[1].x,0);
						downXueBan[6].btmLine.endPt=xueban_vertex[2];
						downXueBan[6].btmLine.startPt.Set(xueban_vertex[2].x,0);

					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[2].upLine.endPt=xueban_vertex[2];
						upXueBan[2].upLine.startPt.Set(xueban_vertex[2].x,0);
						upXueBan[2].btmLine.endPt=xueban_vertex[1];
						upXueBan[2].btmLine.startPt.Set(xueban_vertex[1].x,0);

						upXueBan[6].upLine.endPt=xueban_vertex[0];
						upXueBan[6].upLine.startPt.Set(xueban_vertex[0].x,0);
						upXueBan[6].btmLine.endPt=xueban_vertex[3];
						upXueBan[6].btmLine.startPt.Set(xueban_vertex[3].x,0);
					}
					else
					{
						downXueBan[2].upLine.endPt=xueban_vertex[2];
						downXueBan[2].upLine.startPt.Set(xueban_vertex[2].x,0);
						downXueBan[2].btmLine.endPt=xueban_vertex[1];
						downXueBan[2].btmLine.startPt.Set(xueban_vertex[1].x,0);

						downXueBan[6].upLine.endPt=xueban_vertex[0];
						downXueBan[6].upLine.startPt.Set(xueban_vertex[0].x,0);
						downXueBan[6].btmLine.endPt=xueban_vertex[3];
						downXueBan[6].btmLine.startPt.Set(xueban_vertex[3].x,0);
					}
				}
				for(k=0;k<4;k++)
				{
					line.startPt = xueban_vertex[k];
					line.endPt   = xueban_vertex[(k+1)%4];
					if(bUpJg)
						line.pen.style=PS_SOLID;
					else
						line.pen.style=PS_DASH;
					ent_base.m_arrLine3d.append(line);
				}
			}
			else
			{	//主材角钢X肢与底板X轴一致
				//Y轴向的靴板
				if(foot_para.m_iDiBanShape==0)
				{	//方形底板
					cut_line.startPt.Set(-foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					Int2dll(f2dLine(vertex[0],vertex[1]),cut_line,xueban_vertex[0].x,xueban_vertex[0].y);
					cut_line.startPt.y = -cut_line.startPt.y;
					cut_line.endPt.y = -cut_line.endPt.y;
					Int2dll(f2dLine(vertex[0],vertex[1]),cut_line,xueban_vertex[1].x,xueban_vertex[1].y);
				}
				else
				{	//圆形底板
					f2dCircle circle;
					circle.radius=foot_para.m_iWide/2.0;
					Int2dlc(f2dLine(vertex[0],vertex[1]),circle,xueban_vertex[0].x,xueban_vertex[0].y,xueban_vertex[1].x,xueban_vertex[1].y);
				}
				f3dPoint norm = pJg->get_norm_y_wing();
				vector_trans(norm,m_pFoot->ucs,FALSE);
				double angle = cal_angle_of_2vec(norm,f3dPoint(norm.x,norm.y,0));
				norm = norm.normalized()*fabs(1.0/cos(angle));
				xueban_vertex[2] = xueban_vertex[1]+norm*XueBanThick;
				xueban_vertex[3] = xueban_vertex[0]+norm*XueBanThick;
				xueban_vertex[2].z=xueban_vertex[3].z=0;
				if(xueban_vertex[0].y>0)
				{
					if(bUpJg)
					{
						upXueBan[2].upLine.endPt=xueban_vertex[3];
						upXueBan[2].upLine.startPt.Set(xueban_vertex[3].x,0);
						upXueBan[2].btmLine.endPt=xueban_vertex[0];
						upXueBan[2].btmLine.startPt.Set(xueban_vertex[0].x,0);

						upXueBan[6].upLine.endPt=xueban_vertex[1];
						upXueBan[6].upLine.startPt.Set(xueban_vertex[1].x,0);
						upXueBan[6].btmLine.endPt=xueban_vertex[2];
						upXueBan[6].btmLine.startPt.Set(xueban_vertex[2].x,0);
					}
					else
					{
						downXueBan[2].upLine.endPt=xueban_vertex[3];
						downXueBan[2].upLine.startPt.Set(xueban_vertex[3].x,0);
						downXueBan[2].btmLine.endPt=xueban_vertex[0];
						downXueBan[2].btmLine.startPt.Set(xueban_vertex[0].x,0);

						downXueBan[6].upLine.endPt=xueban_vertex[1];
						downXueBan[6].upLine.startPt.Set(xueban_vertex[1].x,0);
						downXueBan[6].btmLine.endPt=xueban_vertex[2];
						downXueBan[6].btmLine.startPt.Set(xueban_vertex[2].x,0);

					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[2].upLine.endPt=xueban_vertex[2];
						upXueBan[2].upLine.startPt.Set(xueban_vertex[2].x,0);
						upXueBan[2].btmLine.endPt=xueban_vertex[1];
						upXueBan[2].btmLine.startPt.Set(xueban_vertex[1].x,0);

						upXueBan[6].upLine.endPt=xueban_vertex[0];
						upXueBan[6].upLine.startPt.Set(xueban_vertex[0].x,0);
						upXueBan[6].btmLine.endPt=xueban_vertex[3];
						upXueBan[6].btmLine.startPt.Set(xueban_vertex[3].x,0);
					}
					else
					{
						downXueBan[2].upLine.endPt=xueban_vertex[2];
						downXueBan[2].upLine.startPt.Set(xueban_vertex[2].x,0);
						downXueBan[2].btmLine.endPt=xueban_vertex[1];
						downXueBan[2].btmLine.startPt.Set(xueban_vertex[1].x,0);

						downXueBan[6].upLine.endPt=xueban_vertex[0];
						downXueBan[6].upLine.startPt.Set(xueban_vertex[0].x,0);
						downXueBan[6].btmLine.endPt=xueban_vertex[3];
						downXueBan[6].btmLine.startPt.Set(xueban_vertex[3].x,0);
					}
				}
				int k;
				for(k=0;k<4;k++)
				{
					line.startPt = xueban_vertex[k];
					line.endPt   = xueban_vertex[(k+1)%4];
					if(bUpJg)
						line.pen.style=PS_SOLID;
					else
						line.pen.style=PS_DASH;
					ent_base.m_arrLine3d.append(line);
				}
				//X轴向的靴板
				if(foot_para.m_iDiBanShape==0)
				{	//方形底板
					cut_line.startPt.Set(foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0);
					cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
					Int2dll(f2dLine(vertex[0],vertex[5]),cut_line,xueban_vertex[0].x,xueban_vertex[0].y);
					cut_line.startPt.x = -cut_line.startPt.x;
					cut_line.endPt.x = -cut_line.endPt.x;
					Int2dll(f2dLine(vertex[0],vertex[5]),cut_line,xueban_vertex[1].x,xueban_vertex[1].y);
				}
				else
				{	//圆形底板
					f2dCircle circle;
					circle.radius=foot_para.m_iWide/2.0;
					Int2dlc(f2dLine(vertex[0],vertex[5]),circle,xueban_vertex[0].x,xueban_vertex[0].y,xueban_vertex[1].x,xueban_vertex[1].y);
				}
				norm = pJg->get_norm_x_wing();
				angle = cal_angle_of_2vec(norm,f3dPoint(norm.x,norm.y,0));
				norm = norm.normalized()*fabs(1.0/cos(angle));
				vector_trans(norm,m_pFoot->ucs,FALSE);
				xueban_vertex[2] = xueban_vertex[1]+norm*XueBanThick;
				xueban_vertex[3] = xueban_vertex[0]+norm*XueBanThick;
				xueban_vertex[2].z=xueban_vertex[3].z=0;
				if(xueban_vertex[0].x>0)
				{
					if(bUpJg)
					{
						upXueBan[0].upLine.endPt=xueban_vertex[3];
						upXueBan[0].upLine.startPt.Set(0,xueban_vertex[3].y);
						upXueBan[0].btmLine.endPt=xueban_vertex[0];
						upXueBan[0].btmLine.startPt.Set(0,xueban_vertex[0].y);

						upXueBan[4].upLine.endPt=xueban_vertex[1];
						upXueBan[4].upLine.startPt.Set(0,xueban_vertex[1].y);
						upXueBan[4].btmLine.endPt=xueban_vertex[2];
						upXueBan[4].btmLine.startPt.Set(0,xueban_vertex[2].y);
					}
					else
					{
						downXueBan[0].upLine.endPt=xueban_vertex[3];
						downXueBan[0].upLine.startPt.Set(0,xueban_vertex[3].y);
						downXueBan[0].btmLine.endPt=xueban_vertex[0];
						downXueBan[0].btmLine.startPt.Set(0,xueban_vertex[0].y);

						downXueBan[4].upLine.endPt=xueban_vertex[1];
						downXueBan[4].upLine.startPt.Set(0,xueban_vertex[1].y);
						downXueBan[4].btmLine.endPt=xueban_vertex[2];
						downXueBan[4].btmLine.startPt.Set(0,xueban_vertex[2].y);
					}
				}
				else
				{
					if(bUpJg)
					{
						upXueBan[0].upLine.endPt=xueban_vertex[2];
						upXueBan[0].upLine.startPt.Set(0,xueban_vertex[2].y);
						upXueBan[0].btmLine.endPt=xueban_vertex[1];
						upXueBan[0].btmLine.startPt.Set(0,xueban_vertex[1].y);

						upXueBan[4].upLine.endPt=xueban_vertex[0];
						upXueBan[4].upLine.startPt.Set(0,xueban_vertex[0].y);
						upXueBan[4].btmLine.endPt=xueban_vertex[3];
						upXueBan[4].btmLine.startPt.Set(0,xueban_vertex[3].y);
					}
					else
					{
						downXueBan[0].upLine.endPt=xueban_vertex[2];
						downXueBan[0].upLine.startPt.Set(0,xueban_vertex[2].y);
						downXueBan[0].btmLine.endPt=xueban_vertex[1];
						downXueBan[0].btmLine.startPt.Set(0,xueban_vertex[1].y);

						downXueBan[4].upLine.endPt=xueban_vertex[0];
						downXueBan[4].upLine.startPt.Set(0,xueban_vertex[0].y);
						downXueBan[4].btmLine.endPt=xueban_vertex[3];
						downXueBan[4].btmLine.startPt.Set(0,xueban_vertex[3].y);
					}
				}
				for(k=0;k<4;k++)
				{
					line.startPt = xueban_vertex[k];
					line.endPt   = xueban_vertex[(k+1)%4];
					if(bUpJg)
						line.pen.style=PS_SOLID;
					else
						line.pen.style=PS_DASH;
					ent_base.m_arrLine3d.append(line);
				}
			}
		}
		if(pJg->GetClassTypeId()==CLS_GROUPLINEANGLE&&foot_para.m_iDiBanShape==1)	//圆形底板
		{
			f3dPoint norm,ray_vec;
			f3dLine up_line,bottom_line,center_edge_line;
			f2dLine ray_line;
			f2dCircle circle;
			circle.radius=foot_para.m_iWide/2.0;
			for(i=0;i<4;i++)
			{
				if(i==0)
				{	//45度靴板
					norm.Set(cos45,-cos45);
					ray_vec.Set(cos45,cos45);
				}
				else if(i==1)
				{	//135度靴板
					norm.Set(cos45,cos45);
					ray_vec.Set(-cos45,cos45);
				}
				else if(i==2)
				{	//225度靴板
					norm.Set(-cos45,cos45);
					ray_vec.Set(-cos45,-cos45);
				}
				else if(i==3)
				{	//315度靴板
					norm.Set(-cos45,-cos45);
					ray_vec.Set(cos45,-cos45);
				}
				line.endPt=ray_vec*foot_para.m_iWide;
				line.startPt=-line.endPt;
				ray_line.endPt.Set(line.endPt.x,line.endPt.y);
				Int2dlc(f2dLine(line.startPt,line.endPt),circle,line.startPt.x,line.startPt.y,line.endPt.x,line.endPt.y);
				if(i%2>0)
					up_line.startPt=norm*(XueBanThick*0.5)+ray_vec*(XueBanThick*cos45+pJg->GetThick()*cos45*2+XueBanThick/2);
				else
					up_line.startPt=norm*(XueBanThick*0.5)+ray_vec*(XueBanThick*cos45+XueBanThick/2);
				up_line.endPt=norm*(XueBanThick*0.5)+line.startPt;
				bottom_line.startPt=up_line.startPt-norm*XueBanThick;
				bottom_line.endPt=up_line.endPt-norm*XueBanThick;
				center_edge_line.startPt=bottom_line.startPt;
				center_edge_line.endPt=up_line.startPt;
				ent_base.m_arrLine3d.append(up_line);
				ent_base.m_arrLine3d.append(bottom_line);
				ent_base.m_arrLine3d.append(center_edge_line);
				if(bUpJg)
				{
					upXueBan[i*2+1].upLine.startPt=up_line.startPt;
					upXueBan[i*2+1].upLine.endPt=up_line.endPt;
					upXueBan[i*2+1].btmLine.startPt=bottom_line.startPt;
					upXueBan[i*2+1].btmLine.endPt=bottom_line.endPt;
				}
				else
				{
					downXueBan[i*2+1].upLine.startPt=up_line.startPt;
					downXueBan[i*2+1].upLine.endPt=up_line.endPt;
					downXueBan[i*2+1].btmLine.startPt=bottom_line.startPt;
					downXueBan[i*2+1].btmLine.endPt=bottom_line.endPt;
				}
			}
		}
	}
	//底脚螺栓
	for(DIJIAO_LS *pLs=dijiaols_list.GetFirst();pLs;pLs=dijiaols_list.GetNext())
	{
		f3dCircle circle;
		circle.radius = pLs->hole_d/2.0;
		circle.centre = pLs->pos;
		ent_base.m_arrCircle3d.append(circle);
	}
	//肋板
	leiBanDatumLineList.Empty();
	for(LEI_BAN *pLeiBan=leiban_list.GetFirst();pLeiBan;pLeiBan=leiban_list.GetNext())
	{
		f3dPoint norm,inters,line_vec,line_pick,leiban_norm;
		f3dLine line,edge;
		line_pick.Set(pLeiBan->fRayDist,pLeiBan->wide);
		BOOL bUpperPos;
		if(pLeiBan->high>0)
			bUpperPos=TRUE;
		else
			bUpperPos=FALSE;
		if(!pLeiBan->bEnableSecondDatumXueBan)
		{	//单靴板基准
			int index=ftoi(pLeiBan->datumXueBanAngle)/45;
			f3dLine datum_line;
			if(pLeiBan->wide<0)
			{
				if(bUpperPos)
					datum_line=upXueBan[index].upLine;
				else
					datum_line=downXueBan[index].upLine;
				leiban_norm=datum_line.endPt-datum_line.startPt;
				normalize(leiban_norm);
				norm=leiban_norm^f3dPoint(0,0,1);
				line_pick=leiban_norm*pLeiBan->fRayDist;
				SnapPerp(&line.startPt,datum_line,line_pick);
				line.endPt=line.startPt-norm*pLeiBan->wide;
			}
			else
			{
				if(bUpperPos)
					datum_line=upXueBan[index].btmLine;
				else
					datum_line=downXueBan[index].btmLine;
				leiban_norm=datum_line.endPt-datum_line.startPt;
				normalize(leiban_norm);
				norm=leiban_norm^f3dPoint(0,0,1);
				line_pick=leiban_norm*pLeiBan->fRayDist;
				SnapPerp(&line.endPt,datum_line,line_pick);
				line.startPt=line.endPt-norm*pLeiBan->wide;
			}
		}
		else
		{	//双靴板基准
			int prev_xueban_i=(ftoi(pLeiBan->datumXueBanAngle-22.5)%360)/45;
			int next_xueban_i=(ftoi(pLeiBan->datumXueBanAngle+22.5)%360)/45;
			f3dLine prev_datum_line,next_datum_line;
			if(bUpperPos)
			{
				prev_datum_line=upXueBan[prev_xueban_i].btmLine;
				next_datum_line=upXueBan[next_xueban_i].upLine;
			}
			else
			{
				prev_datum_line=downXueBan[prev_xueban_i].btmLine;
				next_datum_line=downXueBan[next_xueban_i].upLine;
			}
			f3dPoint prev_ray_vec=prev_datum_line.endPt-prev_datum_line.startPt;
			f3dPoint next_ray_vec=next_datum_line.endPt-next_datum_line.startPt;
			//Int3dll(prev_datum_line,next_datum_line,inters);
			normalize(prev_ray_vec);
			normalize(next_ray_vec);
			leiban_norm=(prev_ray_vec+next_ray_vec);
			normalize(leiban_norm);
			double cos_halfa=leiban_norm*prev_ray_vec;
			double tan_halfa=sqrt(1-cos_halfa*cos_halfa)/cos_halfa;
			//double ray_dist=0.5*pLeiBan->wide/tan_halfa;
			line_pick=leiban_norm*pLeiBan->fRayDist;
			line_vec=leiban_norm^f3dPoint(0,0,1);
			Int3dll(prev_datum_line.startPt,prev_datum_line.endPt,line_pick-line_vec*pLeiBan->fRayDist,line_pick+line_vec*pLeiBan->wide,line.endPt);
			Int3dll(next_datum_line.startPt,next_datum_line.endPt,line_pick-line_vec*pLeiBan->fRayDist,line_pick+line_vec*pLeiBan->wide,line.startPt);
		}
		if(pLeiBan->high>0)
			edge.pen.style=line.pen.style=PS_SOLID;
		else
			edge.pen.style=line.pen.style=PS_DASH;
		f3dLine *pDatumLine=leiBanDatumLineList.append();
		*pDatumLine=line;
		pDatumLine->startPt+=leiban_norm*(0.5*pLeiBan->thick);
		pDatumLine->endPt+=leiban_norm*(0.5*pLeiBan->thick);
		ent_base.m_arrLine3d.append(line);
		edge.pen.style=PS_SOLID;
		edge.startPt=line.endPt;
		edge.endPt=edge.startPt+leiban_norm*pLeiBan->thick;
		ent_base.m_arrLine3d.append(edge);
		edge.startPt=edge.endPt;
		edge.endPt=line.startPt+leiban_norm*pLeiBan->thick;
		ent_base.m_arrLine3d.append(edge);
		edge.startPt=edge.endPt;
		edge.endPt=line.startPt;
		ent_base.m_arrLine3d.append(edge);
	}
}
double CFootDesignDlg::AngleFromFootToUpJg(double angle)
{
	if(m_bUpAxisXSameToBaseWingX)
	{
		angle=540-angle;
		double odd=angle-(int)angle;
		return ((int)angle)%360+odd;
	}
	else
	{
		angle=270+angle;
		double odd=angle-(int)angle;
		return ((int)angle)%360+odd;
	}
}
double CFootDesignDlg::AngleFromFootToDownJg(double angle)
{
	if(m_bDownAxisXSameToBaseWingX)
	{
		angle=720-angle;
		double odd=angle-(int)angle;
		return ((int)angle)%360+odd;
	}
	else
	{
		angle=270+angle;
		double odd=angle-(int)angle;
		return ((int)angle)%360+odd;
	}
}
//angle是角度制
f3dPoint CFootDesignDlg::RayVecFromUpJgAngle(double angle)
{
	if(m_bUpAxisXSameToBaseWingX)
	{
		angle=Pi*(540-angle)/180;
		return f3dPoint(cos(angle),sin(angle));
	}
	else
	{
		angle=Pi*(90+angle)/180;
		return f3dPoint(cos(angle),sin(angle));
	}
}
//angle是角度制
f3dPoint CFootDesignDlg::RayVecFromDownJgAngle(double angle)
{
	if(m_bDownAxisXSameToBaseWingX)
	{
		angle=Pi*(540-angle)/180;
		return f3dPoint(cos(angle),sin(angle));
	}
	else
	{
		angle=Pi*(90+angle)/180;
		return f3dPoint(cos(angle),sin(angle));
	}
}

void CFootDesignDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	/*m_pDraw->ClearGraphicDatabase();
	m_pDraw->NewPoint(25,25,3);
	m_pDraw->NewLine(0,0,0,80,80,0);
	m_pDraw->Draw(&dc);*/

	dc.SelectStockObject(HOLLOW_BRUSH);
	dc.SetBkMode(TRANSPARENT);
	CPen *pOldPen=dc.SelectObject(&dash_dot_pen);
//绘制坐标轴
	//X轴
	dc.MoveTo((int)(rgn_ucs.origin.x-125),(int)rgn_ucs.origin.y);
	dc.LineTo((int)(rgn_ucs.origin.x+125),(int)rgn_ucs.origin.y);

	dc.LineTo((int)(rgn_ucs.origin.x+105),(int)rgn_ucs.origin.y+15);
	dc.MoveTo((int)(rgn_ucs.origin.x+125),(int)rgn_ucs.origin.y);
	dc.LineTo((int)(rgn_ucs.origin.x+105),(int)rgn_ucs.origin.y-15);
#ifdef AFX_TARG_ENU_ENGLISH
	dc.TextOut((int)(rgn_ucs.origin.x+125),(int)rgn_ucs.origin.y,"X Axis");
#else 
	dc.TextOut((int)(rgn_ucs.origin.x+125),(int)rgn_ucs.origin.y,"X轴");
#endif
	//Y轴
	dc.MoveTo((int)rgn_ucs.origin.x,(int)(rgn_ucs.origin.y-125));
	dc.LineTo((int)rgn_ucs.origin.x,(int)(rgn_ucs.origin.y+125));

	dc.MoveTo((int)rgn_ucs.origin.x,(int)rgn_ucs.origin.y-125);
	dc.LineTo((int)rgn_ucs.origin.x-10,(int)rgn_ucs.origin.y-105);
	dc.MoveTo((int)rgn_ucs.origin.x,(int)rgn_ucs.origin.y-125);
	dc.LineTo((int)rgn_ucs.origin.x+10,(int)rgn_ucs.origin.y-105);
	dc.TextOut((int)rgn_ucs.origin.x+15,(int)rgn_ucs.origin.y-125,"Y");
#ifdef AFX_TARG_ENU_ENGLISH
	dc.TextOut((int)rgn_ucs.origin.x+15,(int)rgn_ucs.origin.y-110," Axis");
#else 
	dc.TextOut((int)rgn_ucs.origin.x+15,(int)rgn_ucs.origin.y-110,"轴");
#endif
	dc.SelectObject(pOldPen);
	for(f3dLine *pLine=ent_base.m_arrLine3d.GetFirst();pLine;pLine=ent_base.m_arrLine3d.GetNext())
	{
		f2dLine line2d;
		f3dLine line=*pLine;
		line.startPt = line.startPt*m_fScaleScreenToUser;
		line.endPt   = line.endPt*m_fScaleScreenToUser;
		coord_trans(line.startPt,rgn_ucs,TRUE);
		coord_trans(line.endPt,rgn_ucs,TRUE);
		line2d.startPt.Set(line.startPt.x,line.startPt.y);
		line2d.endPt.Set(line.endPt.x,line.endPt.y);
		if(IsInvalidLine(line2d,m_rectValidRgn))
			continue;
		Valid2dLine(line2d,m_rectValidRgn);

		if(line.is_highlight)
		{
			pOldPen=dc.SelectObject(&green_dash_pen);
			dc.MoveTo((int)line2d.startPt.x,(int)line2d.startPt.y);
			dc.LineTo((int)line2d.endPt.x,(int)line2d.endPt.y);
			dc.SelectObject(pOldPen);
		}
		else
		{
			if(line.pen.style==PS_DASH)
				dc.SelectObject(&dash_pen);
			else// if(line.pen.style==PS_SOLID)
				dc.SelectObject(&solid_pen);
			dc.MoveTo((int)line2d.startPt.x,(int)line2d.startPt.y);
			dc.LineTo((int)line2d.endPt.x,(int)line2d.endPt.y);
		}
	}
	dc.SelectObject(&solid_pen);
	for(f3dCircle *pCir=ent_base.m_arrCircle3d.GetFirst();pCir;pCir=ent_base.m_arrCircle3d.GetNext())
	{
		f3dRect rect;
		f3dCircle ls_cir=*pCir;
		ls_cir.centre = ls_cir.centre*m_fScaleScreenToUser;
		ls_cir.radius = ls_cir.radius*m_fScaleScreenToUser;
		coord_trans(ls_cir.centre,rgn_ucs,TRUE);
		if(!m_rectValidRgn.PtInRect(CPoint((int)ls_cir.centre.x,(int)ls_cir.centre.y)))
			continue;
		rect.topLeft.Set(ls_cir.centre.x-ls_cir.radius,ls_cir.centre.y-ls_cir.radius);
		rect.bottomRight.Set(ls_cir.centre.x+ls_cir.radius,ls_cir.centre.y+ls_cir.radius);
		//ls_cir.GetRect(&rect);

		if(ls_cir.is_highlight)
		{
			pOldPen=dc.SelectObject(&green_dash_pen);
			dc.Ellipse((int)rect.topLeft.x,(int)rect.topLeft.y,
				(int)rect.bottomRight.x,(int)rect.bottomRight.y);
			dc.SelectObject(pOldPen);
		}
		else
		{
			dc.Ellipse((int)rect.topLeft.x,(int)rect.topLeft.y,
				(int)rect.bottomRight.x,(int)rect.bottomRight.y);
		}
	}	
	for(f3dRect *pRect=ent_base.m_arrRect3d.GetFirst();pRect;pRect=ent_base.m_arrRect3d.GetNext())
	{
		f2dLine line2d[4];
		f3dRect rect=*pRect;
		rect.topLeft = rect.topLeft*m_fScaleScreenToUser;
		rect.bottomRight= rect.bottomRight*m_fScaleScreenToUser;
		coord_trans(rect.topLeft,rgn_ucs,TRUE);
		coord_trans(rect.bottomRight,rgn_ucs,TRUE);
		line2d[0].startPt.Set(rect.topLeft.x,rect.topLeft.y);
		line2d[0].endPt.Set(rect.topLeft.x,rect.bottomRight.y);
		line2d[1].startPt = line2d[0].endPt;
		line2d[1].endPt.Set(rect.bottomRight.x,rect.bottomRight.y);
		line2d[2].startPt = line2d[1].endPt;
		line2d[2].endPt.Set(rect.bottomRight.x,rect.topLeft.y);
		line2d[3].startPt = line2d[2].endPt;
		line2d[3].endPt	  = line2d[0].startPt;
		if(rect.is_highlight)
		{
			pOldPen=dc.SelectObject(&green_dash_pen);
			for(int j=0;j<4;j++)
			{
				if(IsInvalidLine(line2d[j],m_rectValidRgn))
					continue;
				Valid2dLine(line2d[j],m_rectValidRgn);
				
				dc.MoveTo((int)line2d[j].startPt.x,(int)line2d[j].startPt.y);
				dc.LineTo((int)line2d[j].endPt.x,(int)line2d[j].endPt.y);
			}
			dc.SelectObject(pOldPen);
		}
		else
		{
			for(int j=0;j<4;j++)
			{
				if(IsInvalidLine(line2d[j],m_rectValidRgn))
					continue;
				Valid2dLine(line2d[j],m_rectValidRgn);
				dc.MoveTo((int)line2d[j].startPt.x,(int)line2d[j].startPt.y);
				dc.LineTo((int)line2d[j].endPt.x,(int)line2d[j].endPt.y);
			}
		}
	}
}

void CFootDesignDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	f3dPoint pt(point.x,point.y);
	LButtonDown.SetEvent();
	m_bLMouseDown = TRUE;
	SetCapture();
	RefreshCurOper();
	m_curPoint = point;
	coord_trans(pt,rgn_ucs,FALSE);
	pt = pt*(1/m_fScaleScreenToUser);
	
	m_hOldCursor = m_hCursor;
	m_fStaticScaleScreenToUser = m_fScaleScreenToUser;
	SetCursor(m_hCursor);
	m_oldPoint = point;
	CDialog::OnLButtonDown(nFlags, point);
}

void CFootDesignDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	f3dPoint pt(point.x,point.y);
	RefreshCurOper();
	coord_trans(pt,rgn_ucs,FALSE);
	pt = pt*(1/m_fScaleScreenToUser);
	if(m_bLMouseDown)
	{
		ReleaseCapture();
		m_bLMouseDown = FALSE;
	}
	if(oper_type==OPER_OSNAP)
	{
		if(snap_type==SNAP_LINE)
		{
			f3dLine *pDatumLine=leiBanDatumLineList.GetFirst();
			for(LEI_BAN *pLeiBan=leiban_list.GetFirst();pLeiBan;pLeiBan=leiban_list.GetNext())
			{
				f3dLine leibanLine(pDatumLine->startPt,pDatumLine->endPt);
				f2dLine line2d;
				line2d.startPt.Set(pDatumLine->startPt.x,pDatumLine->startPt.y);
				line2d.endPt.Set(pDatumLine->endPt.x,pDatumLine->endPt.y);
				pDatumLine=leiBanDatumLineList.GetNext();
				double dd;
				if(Snap2dLine(pt,line2d,pLeiBan->thick*0.5,dd))
				{
					ModifyLeiBan(pLeiBan);
					InitGraph();
					RefreshRgn();
					break;
				}
			}
		}
		else if(snap_type==SNAP_CIRCLE)
		{
			for(DIJIAO_LS *pLs=dijiaols_list.GetFirst();pLs;pLs=dijiaols_list.GetNext())
			{
				f2dCircle circle2d(pLs->hole_d/2.0,pLs->pos);
				pt.z = pLs->pos.z;
				double dd;
				if(Snap2dCircle(pt,circle2d,10,dd))
				{
					ModifyLsCir(pLs);
					InitGraph();
					RefreshRgn();
					break;
				}
			}
		}
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CFootDesignDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	int offset_x,offset_y;
	f3dPoint pt(point.x,point.y);
	m_curPoint = point;
	coord_trans(pt,rgn_ucs,FALSE);
	pt = pt*(1/m_fScaleScreenToUser);
	SetCursor(m_hCursor);
	if(m_rectValidRgn.PtInRect(point))
	{
		RefreshCurOper();	//更新当前功能配置
		if(m_bLMouseDown)
		{
			offset_x = point.x-m_oldPoint.x;
			offset_y = point.y-m_oldPoint.y;
			if(oper_type==OPER_PAN)
			{
				rgn_ucs.origin.x+=offset_x;
				rgn_ucs.origin.y+=offset_y;
				m_oldPoint = point;
				RefreshRgn();
			}
			else if(oper_type==OPER_ZOOM)
			{
				m_fScaleScreenToUser=m_fStaticScaleScreenToUser*(1.0-0.005*offset_y);
				RefreshRgn();
			}
		}
		else
		{
		}
	}
	else
		m_hCursor = AfxGetApp()->LoadCursor(IDC_ARROW_NORMAL);

	CDialog::OnMouseMove(nFlags, point);
}


void CFootDesignDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_iCurOper=3;
	UpdateData(FALSE);
	RefreshCurOper();	//置于空状态
	
	CDialog::OnRButtonDown(nFlags, point);
}

void CFootDesignDlg::GetXOutsideFace(f3dPoint *face_pick, f3dPoint *face_norm,BOOL bUpper/*=TRUE*/)
{
	CLDSLineAngle *pBaseJg = (CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	BOOL bBaseJgInStart;
	f3dPoint vec;
	if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
	{
		vec =pBaseJg->End()-pBaseJg->Start();
		bBaseJgInStart = TRUE;
	}
	else
	{
		vec =pBaseJg->Start()-pBaseJg->End();
		bBaseJgInStart = FALSE;
	}
	if((bUpper&&vec*m_pFoot->ucs.axis_z<0)||(!bUpper&&vec*m_pFoot->ucs.axis_z>0))
	{
		pBaseJg=(CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
			bBaseJgInStart = TRUE;
		else
			bBaseJgInStart = FALSE;
	}

	if(fabs(pBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)
		>=fabs(pBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
		*face_norm = pBaseJg->get_norm_x_wing();
	else
		*face_norm = pBaseJg->get_norm_y_wing();
	int XueBanThick;
	if(bUpper)
		XueBanThick=foot_para.m_iUpXueBanThick;
	else
		XueBanThick=foot_para.m_iDownXueBanThick;
	if(bBaseJgInStart)
	{
		if(m_pFoot->lj_by_weld)	//焊接
			*face_pick = pBaseJg->Start();
		else if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			*face_pick = pBaseJg->Start()+(*face_norm)*XueBanThick*0.5;
		else
			*face_pick = pBaseJg->Start()+(*face_norm)*XueBanThick;
	}
	else
	{
		if(m_pFoot->lj_by_weld)
			*face_pick = pBaseJg->End();
		else if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			*face_pick = pBaseJg->End()+(*face_norm)*XueBanThick*0.5;
		else
			*face_pick = pBaseJg->End()+(*face_norm)*XueBanThick;
	}
}

void CFootDesignDlg::GetYOutsideFace(f3dPoint *face_pick, f3dPoint *face_norm,BOOL bUpper/*=TRUE*/)
{
	CLDSLineAngle *pBaseJg = (CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	BOOL bBaseJgInStart;
	f3dPoint vec;
	if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
	{
		vec =pBaseJg->End()-pBaseJg->Start();
		bBaseJgInStart = TRUE;
	}
	else
	{
		vec =pBaseJg->Start()-pBaseJg->End();
		bBaseJgInStart = FALSE;
	}
	if((bUpper&&vec*m_pFoot->ucs.axis_z<0)||(!bUpper&&vec*m_pFoot->ucs.axis_z>0))
	{
		pBaseJg=(CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
			bBaseJgInStart = TRUE;
		else
			bBaseJgInStart = FALSE;
	}
	if(fabs(pBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_x)
		>=fabs(pBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_x))
		*face_norm = pBaseJg->get_norm_x_wing();
	else
		*face_norm = pBaseJg->get_norm_y_wing();
	int XueBanThick;
	if(bUpper)
		XueBanThick=foot_para.m_iUpXueBanThick;
	else
		XueBanThick=foot_para.m_iDownXueBanThick;
	if(bBaseJgInStart)
	{
		if(m_pFoot->lj_by_weld)
			*face_pick = pBaseJg->Start();
		else if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			*face_pick = pBaseJg->Start()+(*face_norm)*XueBanThick*0.5;
		else
			*face_pick = pBaseJg->Start()+(*face_norm)*XueBanThick;
	}
	else
	{
		if(m_pFoot->lj_by_weld)
			*face_pick = pBaseJg->End();
		else if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			*face_pick = pBaseJg->End()+(*face_norm)*XueBanThick*0.5;
		else
			*face_pick = pBaseJg->End()+(*face_norm)*XueBanThick;
	}
}

void CFootDesignDlg::GetXInsideFace(f3dPoint *face_pick, f3dPoint *face_norm,BOOL bUpper/*=TRUE*/)
{
	CLDSLineAngle *pBaseJg = (CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	BOOL bBaseJgInStart;
	f3dPoint vec;
	if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
	{
		vec =pBaseJg->End()-pBaseJg->Start();
		bBaseJgInStart = TRUE;
	}
	else
	{
		vec =pBaseJg->Start()-pBaseJg->End();
		bBaseJgInStart = FALSE;
	}
	if((bUpper&&vec*m_pFoot->ucs.axis_z<0)||(!bUpper&&vec*m_pFoot->ucs.axis_z>0))
	{
		pBaseJg=(CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
			bBaseJgInStart = TRUE;
		else
			bBaseJgInStart = FALSE;
	}
	if(fabs(pBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)
		>=fabs(pBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
		*face_norm = pBaseJg->get_norm_x_wing();
	else
		*face_norm = pBaseJg->get_norm_y_wing();
	int XueBanThick;
	if(bUpper)
		XueBanThick=foot_para.m_iUpXueBanThick;
	else
		XueBanThick=foot_para.m_iDownXueBanThick;
	if(bBaseJgInStart)
	{
		if(m_pFoot->lj_by_weld)
			*face_pick = pBaseJg->Start()-(*face_norm)*XueBanThick;
		else if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			*face_pick = pBaseJg->Start()-(*face_norm)*XueBanThick*0.5;
		else
			*face_pick = pBaseJg->Start();
	}
	else
	{
		if(m_pFoot->lj_by_weld)
			*face_pick = pBaseJg->End()-(*face_norm)*XueBanThick;
		else if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			*face_pick = pBaseJg->End()-(*face_norm)*XueBanThick*0.5;
		else
			*face_pick = pBaseJg->End();
	}
}

void CFootDesignDlg::GetYInsideFace(f3dPoint *face_pick, f3dPoint *face_norm,BOOL bUpper/*=TRUE*/)
{
	CLDSLineAngle *pBaseJg = (CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	BOOL bBaseJgInStart;
	f3dPoint vec;
	if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
	{
		vec =pBaseJg->End()-pBaseJg->Start();
		bBaseJgInStart = TRUE;
	}
	else
	{
		vec =pBaseJg->Start()-pBaseJg->End();
		bBaseJgInStart = FALSE;
	}
	if((bUpper&&vec*m_pFoot->ucs.axis_z<0)||(!bUpper&&vec*m_pFoot->ucs.axis_z>0))
	{
		pBaseJg=(CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
			bBaseJgInStart = TRUE;
		else
			bBaseJgInStart = FALSE;
	}
	if(fabs(pBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_x)
		>=fabs(pBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_x))
		*face_norm = pBaseJg->get_norm_x_wing();
	else
		*face_norm = pBaseJg->get_norm_y_wing();
	int XueBanThick;
	if(bUpper)
		XueBanThick=foot_para.m_iUpXueBanThick;
	else
		XueBanThick=foot_para.m_iDownXueBanThick;
	if(bBaseJgInStart)
	{
		if(m_pFoot->lj_by_weld)
			*face_pick = pBaseJg->Start()-(*face_norm)*XueBanThick;
		else if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			*face_pick = pBaseJg->Start()-(*face_norm)*XueBanThick*0.5;
		else
			*face_pick = pBaseJg->Start();
	}
	else
	{
		if(m_pFoot->lj_by_weld)
			*face_pick = pBaseJg->End()-(*face_norm)*XueBanThick;
		else if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			*face_pick = pBaseJg->End()-(*face_norm)*XueBanThick*0.5;
		else
			*face_pick = pBaseJg->End();
	}
}

BOOL CFootDesignDlg::DesignFoot(CLDSNode *pMirNode/*=NULL*/,CLDSLineAngle *pMirJg/*=NULL*/)
{
	f3dPoint ls_pos;
	CLDSPlate *pPlank;
	if(m_pFoot==NULL)
		return FALSE;
	if(m_pFoot->plank_list)
		m_pFoot->plank_list->Empty();
//生成底座板
	if(!DesignDiBan())
		return FALSE;
//生成靴板
	if(!DesignXueBan())
		return FALSE;
//生成肋板
	if(!DesignLeiBan())
		return FALSE;
	CLDSNode *pBaseNode=console.FromNodeHandle(m_pFoot->base_node_handle);
	CLDSPlate *pDiBanPlate=m_pFoot->GetFirstPlank();
	for(pPlank=m_pFoot->GetFirstPlank();pPlank;pPlank=m_pFoot->GetNextPlank())
	{
		for(PROFILE_VERTEX *pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
			pVertex->vertex.feature = 1;
		pPlank->DesignProfile();
		if(pBaseNode)
			pPlank->iSeg=pBaseNode->iSeg;
		if(pPlank!=pDiBanPlate)
			pPlank->m_hPartWeldParent=pDiBanPlate->handle;
		pPlank->cfgword=pBaseNode->cfgword;
		pPlank->SetLayer(pBaseNode->layer());
		for(CLsRef* pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
		{
			pLsRef->GetLsPtr()->cfgword=pBaseNode->cfgword;
			pLsRef->GetLsPtr()->SetLayer(pBaseNode->layer());
		}
	}
	return TRUE;
}

void CFootDesignDlg::OnBnClearDesResult() 
{
	leiban_list.Empty();
	dijiaols_list.Empty();
	InitGraph();
	InvalidateRect(&m_rectValidRgn);
}

//生成底座板
BOOL CFootDesignDlg::DesignDiBan()
{
	CLDSPlate *pPlank;
	if(m_pFoot==NULL)
		return FALSE;
	pPlank = m_pFoot->AppendPlank();
	pPlank->m_cFuncType=CLDSPart::FUNC_FOOTPLATE;
	pPlank->cMaterial=foot_para.m_cDiBanMat;
	pPlank->SetPartNo(foot_para.sDiBanPartNo);
	pPlank->designInfo.m_hBasePart=m_pFoot->attach_jg_handle[0]; //钢板基准杆件出图时使用
	int i=0;
	for(i=0;i<2;i++)
	{	//保存脚底板基准杆件到连接杆件列表中 出组焊图时使用 wht 10-01-13
		if(m_pFoot->attach_jg_handle[i]<0x20)
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_pFoot->attach_jg_handle[0],CLS_LINEPART);
		if(pLinePart==NULL)
			continue;
		CDesignLjPartPara *pLjPartPara=pPlank->designInfo.partList.Add(pLinePart->handle);
		pLjPartPara->hPart=pLinePart->handle;
		pLjPartPara->iFaceNo=1;
		pLjPartPara->m_nClassTypeId=pLinePart->GetClassTypeId();
		if(pLinePart->pStart&&pLinePart->pStart->handle==m_pFoot->base_node_handle)
			pLjPartPara->start0_end1=0;
		else if(pLinePart->pStart&&pLinePart->pStart->handle==m_pFoot->base_node_handle)
			pLjPartPara->start0_end1=1;
		else 
			pLjPartPara->start0_end1=2;
		if(foot_para.m_bWeldDiZuo)
		{
			if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pLinePart;
				for(int j=0;j<4;j++)
				{
					CLDSLineAngle* pSonAngle=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[j]);
					if(pSonAngle)
						pSonAngle->m_hPartWeldParent=pPlank->handle;
				}
			}
			else
				pLinePart->m_hPartWeldParent=pPlank->handle;
		}
	}
	if(foot_para.m_iDiBanShape==1)
	{	//圆弧形底座板
		PROFILE_VERTEX *pVertex;
		double quad_angle=Pi/2;
		pPlank->vertex_list.append(PROFILE_VERTEX(-cos45*foot_para.m_iWide/2.0,-cos45*foot_para.m_iWide/2.0,0));
		pPlank->vertex_list.append(PROFILE_VERTEX( cos45*foot_para.m_iWide/2.0,-cos45*foot_para.m_iWide/2.0,0));
		pPlank->vertex_list.append(PROFILE_VERTEX( cos45*foot_para.m_iWide/2.0, cos45*foot_para.m_iWide/2.0,0));
		pPlank->vertex_list.append(PROFILE_VERTEX(-cos45*foot_para.m_iWide/2.0, cos45*foot_para.m_iWide/2.0,0));
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			pVertex->type=1;
			pVertex->sector_angle=quad_angle;
		}
	}
	else
	{
		pPlank->vertex_list.append(PROFILE_VERTEX(-foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0,0));
		pPlank->vertex_list.append(PROFILE_VERTEX( foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0,0));
		pPlank->vertex_list.append(PROFILE_VERTEX( foot_para.m_iWide/2.0, foot_para.m_iWide/2.0,0));
		pPlank->vertex_list.append(PROFILE_VERTEX(-foot_para.m_iWide/2.0, foot_para.m_iWide/2.0,0));
	}
	pPlank->ucs = m_pFoot->ucs;
	//pPlank->ucs.origin-=m_pFoot->ucs.axis_z*foot_para.m_iThick;	//下移一个底板厚
	pPlank->m_fNormOffset=-foot_para.m_iThick;
	pPlank->Thick=foot_para.m_iThick;
	for(DIJIAO_LS *pLs=dijiaols_list.GetFirst();pLs;pLs=dijiaols_list.GetNext())
	{
		CLDSBolt *pRealLs;
		pRealLs = (CLDSBolt*)console.AppendPart(CLS_BOLT);
		pRealLs->m_bVirtualPart=TRUE;
		pRealLs->set_norm(f3dPoint(0,0,-1));
		pRealLs->set_d(pLs->d);
		pRealLs->hole_d_increment = pLs->hole_d-pLs->d;
		pRealLs->AddL0Thick(foot_para.m_iThick,FALSE);
		f3dPoint dijiaols_pos=pLs->pos;
		coord_trans(dijiaols_pos,m_pFoot->ucs,TRUE);
		pRealLs->ucs.origin=dijiaols_pos;
		pRealLs->cfgword=pPlank->cfgword;	//调整螺栓配材号与基准构件配材号一致
		//填充螺栓位置设计参数
		pRealLs->des_base_pos.datumPoint.datum_pos_style=0;
		pRealLs->des_base_pos.datumPoint.SetPosition(dijiaols_pos);
		pRealLs->des_base_pos.hPart=pPlank->handle;
		pPlank->AppendLsRef(pRealLs->GetLsRef());
	}
	return TRUE;
}
BOOL CFootDesignDlg::DesignLeiBan()
{
	if(m_pFoot==NULL)
		return FALSE;
	//生成肋板
	for(LEI_BAN *pLeiBan=leiban_list.GetFirst();pLeiBan;pLeiBan=leiban_list.GetNext())
	{
		CLDSPlate *pPlank=m_pFoot->AppendPlank();
		pPlank->Thick=pLeiBan->thick;
		pPlank->cMaterial=pLeiBan->cMaterial;
		pPlank->SetPartNo(pLeiBan->sPartNo);
		pPlank->ucs.axis_y=m_pFoot->ucs.axis_z;
		f3dPoint inters,line_pick,face_pick,face_norm,vertice;
		if(!pLeiBan->bEnableSecondDatumXueBan)
		{	//以单一靴板为基准
			CLDSPlate *pXueBanPlank=NULL;
			double xueban_angle_degree,xueban_angle_rad;
			xueban_angle_rad=pLeiBan->datumXueBanAngle*Pi/180;
			pPlank->ucs.axis_z.Set(cos(xueban_angle_rad),sin(xueban_angle_rad));
			vector_trans(pPlank->ucs.axis_z,m_pFoot->ucs,TRUE);
			pPlank->ucs.axis_x=pPlank->ucs.axis_y^pPlank->ucs.axis_z;
			if(pLeiBan->high>0)
			{
				xueban_angle_degree=AngleFromFootToUpJg(pLeiBan->datumXueBanAngle);
				pXueBanPlank=tm.upXueBan[ftoi(xueban_angle_degree)/45];
			}
			else
			{
				xueban_angle_degree=AngleFromFootToDownJg(pLeiBan->datumXueBanAngle);
				pXueBanPlank=tm.downXueBan[ftoi(xueban_angle_degree)/45];
			}
			if(pXueBanPlank==NULL)
				continue;
			
			pPlank->ucs.origin=m_pFoot->ucs.origin+pPlank->ucs.axis_z*pLeiBan->fRayDist;
			RotateVectorAroundVector(pPlank->ucs.axis_y,(pLeiBan->fGradientAngle-90)*Pi/180,pPlank->ucs.axis_x);
			RotateVectorAroundVector(pPlank->ucs.axis_z,(pLeiBan->fGradientAngle-90)*Pi/180,pPlank->ucs.axis_x);
			if(pLeiBan->high<0)
				pPlank->ucs.origin-=m_pFoot->ucs.axis_z*foot_para.m_iThick;
			double dd=pPlank->ucs.axis_x*pXueBanPlank->ucs.axis_z;
			if((pLeiBan->wide>0&&dd>0) || (pLeiBan->wide<0&&dd<0))
				face_pick=pXueBanPlank->ucs.origin+pXueBanPlank->ucs.axis_z*pXueBanPlank->GetThick();
			else
				face_pick=pXueBanPlank->ucs.origin;
			face_norm=pXueBanPlank->ucs.axis_z;
			Int3dlf(inters,pPlank->ucs.origin,pPlank->ucs.axis_x,face_pick,face_norm);
			pPlank->vertex_list.append(inters);
			if(pLeiBan->high>0)
			{
				if(pLeiBan->wide>0)
				{
					vertice=inters+pPlank->ucs.axis_x*pLeiBan->wide;
					pPlank->vertex_list.append(vertice);
					vertice+=pPlank->ucs.axis_y*pLeiBan->A;
					pPlank->vertex_list.append(vertice);
					line_pick=pPlank->ucs.origin+pPlank->ucs.axis_y*pLeiBan->high;
					Int3dlf(inters,line_pick,pPlank->ucs.axis_x,face_pick,face_norm);
					vertice=inters+pPlank->ucs.axis_x*pLeiBan->B;
					pPlank->vertex_list.append(vertice);
					pPlank->vertex_list.append(inters);
				}
				else
				{
					line_pick=pPlank->ucs.origin+pPlank->ucs.axis_y*pLeiBan->high;
					Int3dlf(inters,line_pick,pPlank->ucs.axis_x,face_pick,face_norm);
					pPlank->vertex_list.append(inters);
					vertice=inters-pPlank->ucs.axis_x*pLeiBan->B;
					pPlank->vertex_list.append(vertice);
					PROFILE_VERTEX *pFirstInters=pPlank->vertex_list.GetFirst();
					vertice=pFirstInters->vertex+pPlank->ucs.axis_x*pLeiBan->wide;
					pPlank->vertex_list.append(PROFILE_VERTEX(vertice+pPlank->ucs.axis_y*pLeiBan->A));
					pPlank->vertex_list.append(PROFILE_VERTEX(vertice));
				}
			}
			else
			{
				if(pLeiBan->wide>0)
				{
					line_pick=pPlank->ucs.origin+pPlank->ucs.axis_y*pLeiBan->high;
					Int3dlf(inters,line_pick,pPlank->ucs.axis_x,face_pick,face_norm);
					pPlank->vertex_list.append(inters);
					vertice=inters+pPlank->ucs.axis_x*pLeiBan->B;
					pPlank->vertex_list.append(vertice);
					PROFILE_VERTEX *pFirstInters=pPlank->vertex_list.GetFirst();
					vertice=pFirstInters->vertex+pPlank->ucs.axis_x*pLeiBan->wide;
					pPlank->vertex_list.append(PROFILE_VERTEX(vertice-pPlank->ucs.axis_y*pLeiBan->A));
					pPlank->vertex_list.append(PROFILE_VERTEX(vertice));
				}
				else
				{
					vertice=inters+pPlank->ucs.axis_x*pLeiBan->wide;
					pPlank->vertex_list.append(PROFILE_VERTEX(vertice));
					vertice-=pPlank->ucs.axis_y*pLeiBan->A;
					pPlank->vertex_list.append(PROFILE_VERTEX(vertice));
					line_pick=pPlank->ucs.origin+pPlank->ucs.axis_y*pLeiBan->high;
					Int3dlf(inters,line_pick,pPlank->ucs.axis_x,face_pick,face_norm);
					vertice=inters-pPlank->ucs.axis_x*pLeiBan->B;
					pPlank->vertex_list.append(PROFILE_VERTEX(vertice));
					pPlank->vertex_list.append(PROFILE_VERTEX(inters));
				}
			}
		}
		else
		{
			f3dPoint prev_face_pick,prev_face_norm,prev_axis_x,next_face_pick,next_face_norm,next_axis_x,prev_check_axis_x,next_check_axis_x;
			CLDSPlate *pPrevPlank=NULL,*pNextPlank=NULL;
			double prev_angle=pLeiBan->datumXueBanAngle-22.5;
			double next_angle=pLeiBan->datumXueBanAngle+22.5;
			prev_check_axis_x.Set(cos(prev_angle*Pi/180),sin(prev_angle*Pi/180));
			next_check_axis_x.Set(cos(next_angle*Pi/180),sin(next_angle*Pi/180));
			vector_trans(prev_check_axis_x,m_pFoot->ucs,TRUE);
			vector_trans(next_check_axis_x,m_pFoot->ucs,TRUE);
			if(pLeiBan->high>0)
			{
				prev_angle=AngleFromFootToUpJg(prev_angle);
				next_angle=AngleFromFootToUpJg(next_angle);
				if(prev_angle<0)
					prev_angle+=360;
				if(next_angle<0)
					next_angle+=360;
				if(tm.upXueBan[(ftoi(prev_angle)/45)%8])
					pPrevPlank=tm.upXueBan[(ftoi(prev_angle)/45)%8];
				else if(tm.upXueBan[(ftoi(prev_angle+180)/45)%8])
					pPrevPlank=tm.upXueBan[(ftoi(prev_angle+180)/45)%8];
				if(tm.upXueBan[(ftoi(next_angle)/45)%8])
					pNextPlank=tm.upXueBan[(ftoi(next_angle)/45)%8];
				else if(tm.upXueBan[(ftoi(next_angle+180)/45)%8])
					pNextPlank=tm.upXueBan[(ftoi(next_angle+180)/45)%8];
			}
			else
			{
				prev_angle=AngleFromFootToDownJg(prev_angle);
				next_angle=AngleFromFootToDownJg(next_angle);
				if(prev_angle<0)
					prev_angle+=360;
				if(next_angle<0)
					next_angle+=360;
				if(tm.downXueBan[ftoi(prev_angle)/45])
					pPrevPlank=tm.downXueBan[ftoi(prev_angle)/45];
				else if(tm.downXueBan[ftoi(prev_angle+180)/45])
					pPrevPlank=tm.downXueBan[ftoi(prev_angle+180)/45];
				if(tm.downXueBan[ftoi(next_angle)/45])
					pNextPlank=tm.downXueBan[ftoi(next_angle)/45];
				else if(tm.downXueBan[ftoi(next_angle+180)/45])
					pNextPlank=tm.downXueBan[ftoi(next_angle+180)/45];
			}
			if(pPrevPlank==NULL || pNextPlank==NULL)
				continue;
			prev_angle=prev_angle*Pi/180;
			next_angle=next_angle*Pi/180;
			prev_face_norm=pPrevPlank->ucs.axis_z;
			next_face_norm=pNextPlank->ucs.axis_z;
			f3dPoint check_axis,check_norm;
			check_norm=prev_check_axis_x^f3dPoint(0,0,-1);
			if(check_norm*prev_face_norm<0)
			{
				prev_face_norm*=-1.0;
				prev_face_pick=pPrevPlank->ucs.origin+pPrevPlank->ucs.axis_z*pPrevPlank->GetThick();
			}
			else
				prev_face_pick=pPrevPlank->ucs.origin;
			check_norm=next_check_axis_x^f3dPoint(0,0,-1);
			if(check_norm*next_face_norm>0)
			{
				next_face_norm*=-1.0;
				next_face_pick=pNextPlank->ucs.origin+pNextPlank->ucs.axis_z*pNextPlank->GetThick();
			}
			else
				next_face_pick=pNextPlank->ucs.origin;
			Int3d2f(&line_pick,&prev_axis_x,prev_face_pick,prev_face_norm,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
			Int3d2f(&line_pick,&next_axis_x,next_face_pick,next_face_norm,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
			normalize(prev_axis_x);
			normalize(next_axis_x);
			if(prev_check_axis_x*prev_axis_x<0)
				prev_axis_x*=-1.0;
			if(next_check_axis_x*next_axis_x<0)
				next_axis_x*=-1.0;
			f3dPoint bottom_face_pick;
			if(pLeiBan->high>0)
				bottom_face_pick=m_pFoot->ucs.origin;
			else
				bottom_face_pick=m_pFoot->ucs.origin-m_pFoot->ucs.axis_z*foot_para.m_iThick;
			//Int3d3f(&inters,bottom_face_pick,m_pFoot->ucs.axis_z,prev_face_pick,prev_face_norm,
			//	next_face_pick,next_face_norm);
			double jiajiao=cal_angle_of_2vec(prev_axis_x,next_axis_x);
			pPlank->ucs.axis_z=prev_axis_x+next_axis_x;
			normalize(pPlank->ucs.axis_z);
			pPlank->ucs.axis_y=m_pFoot->ucs.axis_z;
			pPlank->ucs.axis_x=pPlank->ucs.axis_y^pPlank->ucs.axis_z;
			pPlank->ucs.origin=m_pFoot->ucs.origin+pLeiBan->fRayDist*pPlank->ucs.axis_z;
			RotateVectorAroundVector(pPlank->ucs.axis_y,(pLeiBan->fGradientAngle-90)*Pi/180,pPlank->ucs.axis_x);
			RotateVectorAroundVector(pPlank->ucs.axis_z,(pLeiBan->fGradientAngle-90)*Pi/180,pPlank->ucs.axis_x);
			f3dPoint line_pick,vertice;
			Int3dlf(inters,pPlank->ucs.origin,pPlank->ucs.axis_x,prev_face_pick,prev_face_norm);
			pPlank->vertex_list.append(inters);
			vertice=pPlank->ucs.origin-pPlank->ucs.axis_x*(pLeiBan->water_slot_wide*0.5);
			pPlank->vertex_list.append(vertice);
			vertice+=pPlank->ucs.axis_y*pLeiBan->water_slot_wide;
			pPlank->vertex_list.append(vertice);
			vertice=vertice+pPlank->ucs.axis_x*pLeiBan->water_slot_wide;
			pPlank->vertex_list.append(vertice);
			vertice-=pPlank->ucs.axis_y*pLeiBan->water_slot_wide;
			pPlank->vertex_list.append(vertice);
			Int3dlf(inters,pPlank->ucs.origin,pPlank->ucs.axis_x,next_face_pick,next_face_norm);
			pPlank->vertex_list.append(inters);
			line_pick=pPlank->ucs.origin+pPlank->ucs.axis_y*pLeiBan->high;
			Int3dlf(inters,line_pick,pPlank->ucs.axis_x,next_face_pick,next_face_norm);
			pPlank->vertex_list.append(inters);
			Int3dlf(inters,line_pick,pPlank->ucs.axis_x,prev_face_pick,prev_face_norm);
			pPlank->vertex_list.append(inters);
		}
		for(PROFILE_VERTEX *pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,pPlank->ucs,FALSE);
			pVertex->vertex.feature=1;
		}
	}
	return TRUE;
}
// DesXueBan Step1获得此底脚板(牛蹄板)所连接的角钢集
bool CFootDesignDlg::GetLinkJgSet(JGSET& jgset)
{
	CLDSModule *pActModule=Ta.GetActiveModule();
	if(pActModule==NULL)
		return false;
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_LINEANGLE&&pPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
			continue;
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pPart;
		if(pLineAngle->pStart==NULL||pLineAngle->pEnd==NULL)
			continue;
		if(pLineAngle->group_father_jg_h>0x20)
			continue;	//组合子角钢
		if(!pActModule->IsSonPart(pLineAngle))
			continue;
		if(pLineAngle->pStart->handle==m_pFoot->base_node_handle||pLineAngle->pEnd->handle==m_pFoot->base_node_handle)
			jgset.append(pLineAngle);
		else
			continue;
	}
	return true;
}
// DesXueBan Step2获得上下两根基准角钢
bool CFootDesignDlg::GetBaseJg(CLDSLineAngle *& pUpBaseJg, CLDSLineAngle *& pDownBaseJg)
{
	CLDSLineAngle *pBaseJg;
	for(int i=0;i<2;i++)
	{
		pBaseJg=(CLDSLineAngle*)console.FromPartHandle(m_pFoot->attach_jg_handle[i],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pBaseJg==NULL)
			continue;
		if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
		{
			f3dPoint vec=pBaseJg->End()-pBaseJg->Start();
			if(vec*m_pFoot->ucs.axis_z>0&&pUpBaseJg==NULL)
				pUpBaseJg = pBaseJg;
			else if(pDownBaseJg==NULL)
				pDownBaseJg = pBaseJg;
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Both datum Angle in the same side, the data is error!");
#else 
				AfxMessageBox("两根基准角钢都处于同一侧,数据错误!");
#endif
				return false;
			}
		}
		else
		{
			f3dPoint vec=pBaseJg->Start()-pBaseJg->End();
			if(vec*m_pFoot->ucs.axis_z>0&&pUpBaseJg==NULL)
				pUpBaseJg = pBaseJg;
			else if(pDownBaseJg==NULL)
				pDownBaseJg = pBaseJg;
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Both datum Angle in the same side, the data is error!");
#else 
				AfxMessageBox("两根基准角钢都处于同一侧,数据错误!");
#endif
				return false;
			}
		}
	}
	if(pUpBaseJg==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The date of foundation plates'angles is error!");
#else 
		AfxMessageBox("底脚板基准角钢,数据错误!");
#endif
		return false;
	}
	return true;
}

// DesXueBan Step3计算基准角钢的正负头及切角切肢
void CFootDesignDlg::CalBaseJgOddment(CLDSLineAngle * pBaseJg)
{
	f3dPoint vertice,vertex_x,vertex_y,vertex_up,vertex_down;
	normalize(tm.jg_vec);
	if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
	{
		// 1. 根据角钢最低顶点距底座要求计算主材角钢正负头
		double odd;
		vertex_x = pBaseJg->Start()+tm.wing_vec_x*tm.wing_wide;
		vertex_y = pBaseJg->Start()+tm.wing_vec_y*tm.wing_wide;
		if(foot_para.m_bWeldDiZuo)
		{	// 1.1主材焊接到底座
			f3dPoint ber_pos = pBaseJg->Start();
			Int3dlf(vertice,ber_pos,tm.jg_vec,tm.face_pick,m_pFoot->ucs.axis_z);
			Int3dlf(vertex_x,ber_pos,tm.jg_vec,tm.face_pick,m_pFoot->ucs.axis_z);
			Int3dlf(vertex_y,ber_pos,tm.jg_vec,tm.face_pick,m_pFoot->ucs.axis_z);
			double dd=DISTANCE(vertice,ber_pos);
			if((tm.face_pick.z-ber_pos.z)*tm.jg_vec.z<0)
				dd*=-1.0;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				if(pGroupJg->group_style==0)
				{
					f3dPoint bi_vec=pBaseJg->get_norm_x_wing()+pBaseJg->get_norm_y_wing();
					normalize(bi_vec);
					double offset=0.707106781*pGroupJg->jg_space*bi_vec.z/tm.jg_vec.z;
					CLDSLineAngle *pSonJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
					CLDSLineAngle *pSonJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
					if(pSonJg1)
						pSonJg1->SetStartOdd(dd+offset);
					if(pSonJg2)
						pSonJg2->SetStartOdd(dd-offset);
				}
				else if(((CLDSGroupLineAngle*)pBaseJg)->group_style==2)
				{
					f3dPoint bi_vec=pBaseJg->get_norm_x_wing()+pBaseJg->get_norm_y_wing();
					f3dPoint pa_vec=pBaseJg->get_norm_x_wing()-pBaseJg->get_norm_y_wing();
					normalize(bi_vec);
					double offseta=0.707106781*pGroupJg->jg_space*bi_vec.z/tm.jg_vec.z;
					double offsetb=0.707106781*pGroupJg->jg_space*pa_vec.z/tm.jg_vec.z;
					CLDSLineAngle *pSonJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
					CLDSLineAngle *pSonJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
					CLDSLineAngle *pSonJg3=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
					CLDSLineAngle *pSonJg4=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
					if(pSonJg1)
						pSonJg1->SetStartOdd(dd+offseta);
					if(pSonJg3)
						pSonJg3->SetStartOdd(dd-offseta);
					if(pSonJg4)
						pSonJg4->SetStartOdd(dd+offsetb);
					if(pSonJg2)
						pSonJg2->SetStartOdd(dd-offsetb);
				}
			}
			else
				pBaseJg->SetStartOdd(dd);

			//计算切肢量
			double ddX,ddY;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				f3dPoint inters,vecx,vecy;
				double ddx,ddy;
				int nSonJg=2;
				if(pGroupJg->group_style==2)
					nSonJg=4;
				for(int i=0;i<nSonJg;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg)
					{
						vertex_x=pSonJg->Start()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
						vertex_y=pSonJg->Start()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
						Int3dlf(inters,vertex_x,tm.jg_vec,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
						vecx=inters-vertex_x;
						Int3dlf(inters,vertex_y,tm.jg_vec,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
						vecy=inters-vertex_y;
						ddx=vecx*tm.jg_vec;
						ddy=vecy*tm.jg_vec;
						/*if(ddx>pSonJg1->startOdd()&&ddy>pSonJg1->startOdd())
						{
						//这种情况是不可能发生的
						}
						else*/
						if(ddx>pSonJg->startOdd())
						{
							pSonJg->cut_wing[0]=2;	//切Y肢
							pSonJg->cut_wing_para[0][0]=ddx-pSonJg->startOdd();
							pSonJg->cut_wing_para[0][1]=pSonJg->GetWidth();
							pSonJg->cut_wing_para[0][2]=ddx-ddy;
							pSonJg->SetStartOdd(ddx);
						}
						else if(ddy>pSonJg->startOdd())
						{
							pSonJg->cut_wing[0]=1;	//切X肢
							pSonJg->cut_wing_para[0][0]=ddy-pSonJg->startOdd();
							pSonJg->cut_wing_para[0][1]=pSonJg->GetWidth();
							pSonJg->cut_wing_para[0][2]=ddy-ddx;
							pSonJg->SetStartOdd(ddy);
						}
						else
						{
							pSonJg->cut_angle[0][0] = pSonJg->startOdd()-ddx;
							pSonJg->cut_angle[0][1] = tm.wing_wide;
							pSonJg->cut_angle[1][0] = pSonJg->startOdd()-ddy;
							pSonJg->cut_angle[1][1] = tm.wing_wide;
						}
					}
				}
			}
			else
			{
				ddX = DISTANCE(vertice,vertex_x);
				ddY = DISTANCE(vertice,vertex_y);
				dd  = ddX*ddX-tm.wing_wide*tm.wing_wide;
				if(dd>0)
				{
					pBaseJg->cut_angle[0][0] = sqrt(dd);
					pBaseJg->cut_angle[0][1] = tm.wing_wide;
				}
				dd  = ddY*ddY-tm.wing_wide*tm.wing_wide;
				if(dd>0)
				{
					pBaseJg->cut_angle[1][0] = sqrt(dd);
					pBaseJg->cut_angle[1][1] = tm.wing_wide;
				}
			}
		}
		else
		{	// 1.2保证正负头
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				vertex_up=vertex_down=pBaseJg->Start();
				for(int i=0;i<4;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg)
					{
						vertice=pSonJg->Start();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
						vertice=pSonJg->Start()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
						vertice=pSonJg->Start()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
					}
				}
			}
			else
			{
				if(vertex_x.z>vertex_y.z)
				{	//X肢更低
					vertex_up=vertex_y;
					vertex_down=vertex_x;
				}
				else
				{	//Y肢更低
					vertex_up=vertex_x;
					vertex_down=vertex_y;
				}
			}
			if(tm.C>0)
			{
				if(tm.bUpJg)	//上侧基准材
				{
					odd = vertex_down.z-tm.face_pick.z+tm.C;
					pBaseJg->SetStartOdd((int)-(odd*tm.jg_vec.mod()/tm.jg_vec.z));
				}
				else					//下侧基准材
				{
					odd = vertex_up.z-tm.face_pick.z-tm.C;
					pBaseJg->SetStartOdd((int)-(odd*tm.jg_vec.mod()/tm.jg_vec.z));
				}
			}
			else
				pBaseJg->SetStartOdd(tm.C);
		}
	}
	else
	{
		// 2.根据角钢最低顶点距底座要求计算主材角钢正负头
		double odd;
		vertex_x = pBaseJg->End()+tm.wing_vec_x*tm.wing_wide;
		vertex_y = pBaseJg->End()+tm.wing_vec_y*tm.wing_wide;
		if(foot_para.m_bWeldDiZuo)
		{	// 2.1
			f3dPoint ber_pos = pBaseJg->End();
			Int3dlf(vertice,ber_pos,tm.jg_vec,tm.face_pick,m_pFoot->ucs.axis_z);
			Int3dlf(vertex_x,vertex_x,tm.jg_vec,tm.face_pick,m_pFoot->ucs.axis_z);
			Int3dlf(vertex_y,vertex_y,tm.jg_vec,tm.face_pick,m_pFoot->ucs.axis_z);
			double dd=DISTANCE(vertice,ber_pos);
			if((tm.face_pick.z-ber_pos.z)*tm.jg_vec.z<0)
				dd*=-1.0;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				if(pGroupJg->group_style==0)
				{
					f3dPoint bi_vec=pBaseJg->get_norm_x_wing()+pBaseJg->get_norm_y_wing();
					normalize(bi_vec);
					double offset=0.707106781*pGroupJg->jg_space*bi_vec.z/tm.jg_vec.z;
					CLDSLineAngle *pSonJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
					CLDSLineAngle *pSonJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
					if(pSonJg1)
						pSonJg1->SetEndOdd(dd+offset);
					if(pSonJg2)				
						pSonJg2->SetEndOdd(dd-offset);
				}
				else if(((CLDSGroupLineAngle*)pBaseJg)->group_style==2)
				{
					f3dPoint bi_vec=pBaseJg->get_norm_x_wing()+pBaseJg->get_norm_y_wing();
					f3dPoint pa_vec=pBaseJg->get_norm_x_wing()-pBaseJg->get_norm_y_wing();
					normalize(bi_vec);
					double offseta=0.707106781*pGroupJg->jg_space*bi_vec.z/tm.jg_vec.z;
					double offsetb=0.707106781*pGroupJg->jg_space*pa_vec.z/tm.jg_vec.z;
					CLDSLineAngle *pSonJg1=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
					CLDSLineAngle *pSonJg2=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
					CLDSLineAngle *pSonJg3=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
					CLDSLineAngle *pSonJg4=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
					if(pSonJg1)
						pSonJg1->SetEndOdd(dd+offseta);
					if(pSonJg3)				 
						pSonJg3->SetEndOdd(dd-offseta);
					if(pSonJg4)
						pSonJg4->SetEndOdd(dd+offsetb);
					if(pSonJg2)				 
						pSonJg2->SetEndOdd(dd-offsetb);
				}
			}
			else
				pBaseJg->SetEndOdd(ftoi(dd));
			//计算切肢量
			double ddX,ddY;
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				double ddx,ddy;
				f3dPoint inters,vecx,vecy;
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;

				int nSonJg=2;
				if(pGroupJg->group_style==2)
					nSonJg=4;
				for(int i=0;i<nSonJg;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg)
					{
						vertex_x=pSonJg->End()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
						vertex_y=pSonJg->End()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
						Int3dlf(inters,vertex_x,tm.jg_vec,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
						vecx=inters-vertex_x;
						Int3dlf(inters,vertex_y,tm.jg_vec,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
						vecy=inters-vertex_y;
						ddx=vecx*tm.jg_vec;
						ddy=vecy*tm.jg_vec;
						/*if(ddx>pSonJg1->endOdd()&&ddy>pSonJg1->endOdd())
						{
						//这种情况是不可能发生的
						}
						else*/
						if(ddx>pSonJg->endOdd())
						{
							pSonJg->cut_wing[1]=2;	//切Y肢
							pSonJg->cut_wing_para[1][0]=ddx-pSonJg->endOdd();
							pSonJg->cut_wing_para[1][1]=pSonJg->GetWidth();
							pSonJg->cut_wing_para[1][2]=ddx-ddy;
							pSonJg->SetEndOdd(ddx);
						}
						else if(ddy>pSonJg->endOdd())
						{
							pSonJg->cut_wing[1]=1;	//切X肢
							pSonJg->cut_wing_para[1][0]=ddy-pSonJg->endOdd();
							pSonJg->cut_wing_para[1][1]=pSonJg->GetWidth();
							pSonJg->cut_wing_para[1][2]=ddy-ddx;
							pSonJg->SetEndOdd(ddy);
						}
						else
						{
							pSonJg->cut_angle[2][0] = pSonJg->endOdd()-ddx;
							pSonJg->cut_angle[2][1] = tm.wing_wide;
							pSonJg->cut_angle[3][0] = pSonJg->endOdd()-ddy;
							pSonJg->cut_angle[3][1] = tm.wing_wide;
						}
					}
				}
			}
			else
			{
				ddX = DISTANCE(vertice,vertex_x);
				ddY = DISTANCE(vertice,vertex_y);
				dd  = ddX*ddX-tm.wing_wide*tm.wing_wide;
				if(dd>0)
				{
					pBaseJg->cut_angle[2][0] = sqrt(dd);
					pBaseJg->cut_angle[2][1] = tm.wing_wide;
				}
				dd  = ddY*ddY-tm.wing_wide*tm.wing_wide;
				if(dd>0)
				{
					pBaseJg->cut_angle[3][0] = sqrt(dd);
					pBaseJg->cut_angle[3][1] = tm.wing_wide;
				}
			}
		}
		else
		{	// 2.2
			if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
				vertex_up=vertex_down=pBaseJg->End();
				int nSonJg=2;
				if(pGroupJg->group_style==2)
					nSonJg=4;
				for(int i=0;i<nSonJg;i++)
				{
					CLDSLineAngle *pSonJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
					if(pSonJg)
					{
						vertice=pSonJg->End();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
						vertice=pSonJg->End()+pSonJg->GetWingVecX()*pSonJg->GetWidth();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
						vertice=pSonJg->End()+pSonJg->GetWingVecY()*pSonJg->GetWidth();
						if(vertice.z>vertex_down.z)
							vertex_down=vertice;
						if(vertice.z<vertex_up.z)
							vertex_up=vertice;
					}
				}
			}
			else
			{
				if(vertex_x.z>vertex_y.z)
				{	//X肢更低
					vertex_up=vertex_y;
					vertex_down=vertex_x;
				}
				else
				{	//Y肢更低
					vertex_up=vertex_x;
					vertex_down=vertex_y;
				}
			}
			if(tm.C>0)
			{
				if(tm.bUpJg)	//上侧基准材
				{
					if(vertex_x.z>vertex_y.z)	//X肢更低
						odd = vertex_x.z-tm.face_pick.z+tm.C;
					else						//Y肢更低
						odd = vertex_y.z-tm.face_pick.z+tm.C;
				}
				else					//下侧基准材
				{
					if(vertex_x.z<vertex_y.z)	//X肢更高
						odd = vertex_x.z-tm.face_pick.z-tm.C;
					else						//Y肢更高
						odd = vertex_y.z-tm.face_pick.z-tm.C;
				}
				pBaseJg->SetEndOdd((int)-(odd*tm.jg_vec.mod()/tm.jg_vec.z));
			}
			else		//直接指定负头
				pBaseJg->SetEndOdd(tm.C);
		}
	}
}
// DesXueBan Step4计算基准角钢与底座板边缘的交点
void CFootDesignDlg::CalBaseJgPlankIntVertex(CLDSLineAngle *pBaseJg, f3dPoint vert_arr[6],f3dPoint xueban_vert_arr[4])
{
	f2dLine cut_line;
	f3dLine line;
	if(pBaseJg->GetClassTypeId()!=CLS_GROUPLINEANGLE&&m_pFoot->lj_by_weld)	//靴板与主材角钢对焊
	{	//2.1.1只适用于单角钢,组合角钢一般用螺栓连接不对焊
		if(pBaseJg->pSolidBody==NULL)
			pBaseJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		//计算基准材角钢在底板上的截面图
		for(int i=0;i<6;i++)
		{
			Int3dlf(vert_arr[i],pBaseJg->pSolidBody->GetKeyPointAt(i),tm.jg_vec,tm.face_pick,m_pFoot->ucs.axis_z);
			coord_trans(vert_arr[i],m_pFoot->ucs,FALSE);
		}
		
		//计算靴板与底座边缘的四个主要交点(位于主材角钢外皮上)
		if(fabs(dot_prod(tm.wing_vec_x,m_pFoot->ucs.axis_y))>cos45)
		{	//主材角钢X肢与底板Y轴一致
			//X轴向的靴板
			cut_line.startPt.Set(foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0);
			cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			Int2dll(f2dLine(vert_arr[0],vert_arr[1]),cut_line,xueban_vert_arr[0].x,xueban_vert_arr[0].y);
			cut_line.startPt.x = -cut_line.startPt.x;
			cut_line.endPt.x = -cut_line.endPt.x;
			Int2dll(f2dLine(vert_arr[0],vert_arr[1]),cut_line,xueban_vert_arr[1].x,xueban_vert_arr[1].y);
			//Y轴向的靴板
			cut_line.startPt.Set(-foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			Int2dll(f2dLine(vert_arr[0],vert_arr[5]),cut_line,xueban_vert_arr[2].x,xueban_vert_arr[2].y);
			cut_line.startPt.y = -cut_line.startPt.y;
			cut_line.endPt.y = -cut_line.endPt.y;
			Int2dll(f2dLine(vert_arr[0],vert_arr[5]),cut_line,xueban_vert_arr[3].x,xueban_vert_arr[3].y);
		}
		else
		{	//主材角钢X肢与底板X轴一致
			//Y轴向的靴板
			cut_line.startPt.Set(-foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			Int2dll(f2dLine(vert_arr[0],vert_arr[1]),cut_line,xueban_vert_arr[2].x,xueban_vert_arr[2].y);
			cut_line.startPt.y = -cut_line.startPt.y;
			cut_line.endPt.y = -cut_line.endPt.y;
			Int2dll(f2dLine(vert_arr[0],vert_arr[1]),cut_line,xueban_vert_arr[3].x,xueban_vert_arr[3].y);
			//X轴向的靴板
			cut_line.startPt.Set(foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0);
			cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			Int2dll(f2dLine(vert_arr[0],vert_arr[5]),cut_line,xueban_vert_arr[0].x,xueban_vert_arr[0].y);
			cut_line.startPt.x = -cut_line.startPt.x;
			cut_line.endPt.x = -cut_line.endPt.x;
			Int2dll(f2dLine(vert_arr[0],vert_arr[5]),cut_line,xueban_vert_arr[1].x,xueban_vert_arr[1].y);
		}
		if(foot_para.m_iDiBanShape==1)
		{	//圆形底座板一般需要添加对角线上的辅助靴板
		}
	}
	else			//靴板与主材角钢螺栓连接
	{	//2.1.2
		if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//组合角钢
			int nXueBanThick;
			if(tm.bUpJg)
				nXueBanThick=foot_para.m_iUpXueBanThick;
			else
				nXueBanThick=foot_para.m_iDownXueBanThick;
			pBaseJg->set_wing_thick(nXueBanThick*0.5);
			vert_arr[0]=pBaseJg->get_end_in_ber();
			pBaseJg->set_wing_thick(tm.wing_thick);
		}
		else//单角钢
			vert_arr[0] = pBaseJg->End();
		vert_arr[1] = vert_arr[0]+tm.wing_vec_y*tm.wing_wide;
		vert_arr[2] = vert_arr[0]+tm.wing_vec_x*tm.wing_wide;
		//计算基准材角钢在底板上的截面图
		for(int i=0;i<3;i++)
		{
			Int3dlf(vert_arr[i],vert_arr[i],tm.jg_vec,tm.face_pick,m_pFoot->ucs.axis_z);
			coord_trans(vert_arr[i],m_pFoot->ucs,FALSE);
		}
		//计算靴板与底座边缘的四个主要交点(位于主材角钢外皮上)
		if(fabs(dot_prod(tm.wing_vec_x,m_pFoot->ucs.axis_y))>cos45)
		{	//主材角钢X肢与底板Y轴一致
			//X轴向的靴板
			cut_line.startPt.Set(foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0);
			cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			Int2dll(f2dLine(vert_arr[0],vert_arr[1]),cut_line,xueban_vert_arr[0].x,xueban_vert_arr[0].y);
			cut_line.startPt.x = -cut_line.startPt.x;
			cut_line.endPt.x = -cut_line.endPt.x;
			Int2dll(f2dLine(vert_arr[0],vert_arr[1]),cut_line,xueban_vert_arr[1].x,xueban_vert_arr[1].y);
			//Y轴向的靴板
			cut_line.startPt.Set(-foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			Int2dll(f2dLine(vert_arr[0],vert_arr[2]),cut_line,xueban_vert_arr[2].x,xueban_vert_arr[2].y);
			cut_line.startPt.y = -cut_line.startPt.y;
			cut_line.endPt.y = -cut_line.endPt.y;
			Int2dll(f2dLine(vert_arr[0],vert_arr[2]),cut_line,xueban_vert_arr[3].x,xueban_vert_arr[3].y);
		}
		else
		{	//主材角钢X肢与底板X轴一致
			//Y轴向的靴板
			cut_line.startPt.Set(-foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			Int2dll(f2dLine(vert_arr[0],vert_arr[1]),cut_line,xueban_vert_arr[2].x,xueban_vert_arr[2].y);
			cut_line.startPt.y = -cut_line.startPt.y;
			cut_line.endPt.y = -cut_line.endPt.y;
			Int2dll(f2dLine(vert_arr[0],vert_arr[1]),cut_line,xueban_vert_arr[3].x,xueban_vert_arr[3].y);
			//X轴向的靴板
			cut_line.startPt.Set(foot_para.m_iWide/2.0,-foot_para.m_iWide/2.0);
			cut_line.endPt.Set(foot_para.m_iWide/2.0,foot_para.m_iWide/2.0);
			Int2dll(f2dLine(vert_arr[0],vert_arr[2]),cut_line,xueban_vert_arr[0].x,xueban_vert_arr[0].y);
			cut_line.startPt.x = -cut_line.startPt.x;
			cut_line.endPt.x = -cut_line.endPt.x;
			Int2dll(f2dLine(vert_arr[0],vert_arr[2]),cut_line,xueban_vert_arr[1].x,xueban_vert_arr[1].y);
		}
	}
}
//此函数计算具有内圆弧r的角钢内皮45°紧贴一厚为thick的钢板时，钢板边距离角钢内直角楞线的距离
double CalJgROffset(double r,double thick)
{
	if(thick>=r*cos45*2)	//紧贴在内圆弧外的角钢内皮上
		return thick/2;
	else
		return r*(2*cos45-sqrt(1-thick*thick/(r*r*4)));
}
f3dPoint CalVertVecOfParaA(const UCS_STRU& csXueBan)
{
	f3dPoint v_vec(0,0,1);
	vector_trans(v_vec,csXueBan,FALSE);
	v_vec.z=0;
	Standarized(v_vec);
	if(v_vec.y<0)
		v_vec.Set(-v_vec.x,-v_vec.y);
	return v_vec;
}
// 设定靴板的定位坐标系及轮廓顶点坐标等参数
void CFootDesignDlg::CalXueBanPara(CLDSLineAngle *pBaseJg)
{
	double cur_oddment;
	f3dPoint cur_end_pos;
	CConnectInfo ls_group;
	LSSPACE_STRU LsSpace;
	int nXueBanThick;
	CLDSPlate *pPlank0C=NULL, *pPlank45C=NULL, *pPlank90C=NULL, *pPlank135C=NULL;
	CLDSPlate *pPlank180C=NULL, *pPlank225C=NULL, *pPlank270C=NULL, *pPlank315C=NULL;
	char sXueBanPartNo0C[17],sXueBanPartNo45C[17],sXueBanPartNo90C[17],sXueBanPartNo135C[17];
	char sXueBanPartNo180C[17],sXueBanPartNo225C[17],sXueBanPartNo270C[17],sXueBanPartNo315C[17];
	if(tm.bUpJg)
	{	//设计上侧靴板
		nXueBanThick=foot_para.m_iUpXueBanThick;
		strcpy(sXueBanPartNo0C  ,foot_para.sUpXueBanPartNo0C);
		strcpy(sXueBanPartNo45C ,foot_para.sUpXueBanPartNo45C );
		strcpy(sXueBanPartNo90C ,foot_para.sUpXueBanPartNo90C );
		strcpy(sXueBanPartNo135C,foot_para.sUpXueBanPartNo135C);
		strcpy(sXueBanPartNo180C,foot_para.sUpXueBanPartNo180C);
		strcpy(sXueBanPartNo225C,foot_para.sUpXueBanPartNo225C);
		strcpy(sXueBanPartNo270C,foot_para.sUpXueBanPartNo270C);
		strcpy(sXueBanPartNo315C,foot_para.sUpXueBanPartNo315C);
	}
	else
	{	//设计下侧靴板
		nXueBanThick=foot_para.m_iDownXueBanThick;
		strcpy(sXueBanPartNo0C  ,foot_para.sDownXueBanPartNo0C);
		strcpy(sXueBanPartNo45C ,foot_para.sDownXueBanPartNo45C );
		strcpy(sXueBanPartNo90C ,foot_para.sDownXueBanPartNo90C );
		strcpy(sXueBanPartNo135C,foot_para.sDownXueBanPartNo135C);
		strcpy(sXueBanPartNo180C,foot_para.sDownXueBanPartNo180C);
		strcpy(sXueBanPartNo225C,foot_para.sDownXueBanPartNo225C);
		strcpy(sXueBanPartNo270C,foot_para.sDownXueBanPartNo270C);
		strcpy(sXueBanPartNo315C,foot_para.sDownXueBanPartNo315C);
	}
	if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
	{	//获得基准材螺栓间距信息
		cur_oddment=pBaseJg->startOdd();
		if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//组合角钢
			pBaseJg->set_wing_thick(nXueBanThick*0.5);
			cur_end_pos=pBaseJg->get_start_in_ber();
			pBaseJg->set_wing_thick(tm.wing_thick);
		}
		else						//单角钢
			cur_end_pos = pBaseJg->Start();
		ls_group=pBaseJg->connectStart;
	}
	else
	{	//获得基准材螺栓间距信息
		cur_oddment=pBaseJg->endOdd();
		if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//组合角钢
			pBaseJg->set_wing_thick(nXueBanThick*0.5);
			cur_end_pos=pBaseJg->get_end_in_ber();
			pBaseJg->set_wing_thick(tm.wing_thick);
		}
		else						//单角钢
			cur_end_pos = pBaseJg->End();
		ls_group=pBaseJg->connectEnd;
	}
	GetLsSpace(LsSpace,ls_group.d);
	if(ls_group.LsSpace.EndSpace>0&&ls_group.LsSpace.SingleRowSpace>0
		&&ls_group.LsSpace.DoubleRowSpace>0)
	{	//根据外部设定的螺栓间距及端距重新设置螺栓间距端距 wht 11-04-26
		LsSpace.EndSpace=ls_group.LsSpace.EndSpace;
		LsSpace.SingleRowSpace=ls_group.LsSpace.SingleRowSpace;
		LsSpace.doubleRowSpace=ls_group.LsSpace.DoubleRowSpace;
	}
	//2.2.1生成靴板的相对坐标系的坐标轴
	BOOL m_bBreakXueBan0C,m_bBreakXueBan90C,bAxisXSameToBaseWingX;
	if(tm.bUpJg)
	{
		m_bBreakXueBan0C = foot_para.m_bBreakUpXueBan0C;
		m_bBreakXueBan90C = foot_para.m_bBreakUpXueBan90C;
	}
	else
	{
		m_bBreakXueBan0C = foot_para.m_bBreakDownXueBan0C;
		m_bBreakXueBan90C = foot_para.m_bBreakDownXueBan90C;
	}
	if(	fabs(pBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_x)<
		fabs(pBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_x))
	{	//2.2.1.1X肢为牛蹄板X轴向
		bAxisXSameToBaseWingX=TRUE;
		//X轴向坐标系确定
		if(m_bBreakXueBan0C)
		{	//X轴向前后板分开
			pPlank180C=m_pFoot->AppendPlank();
			pPlank0C=m_pFoot->AppendPlank();
			pPlank180C->cMaterial=foot_para.m_cXueBanMat;
			pPlank0C->cMaterial=foot_para.m_cXueBanMat;
			pPlank180C->SetPartNo(sXueBanPartNo180C);
			pPlank0C->SetPartNo(sXueBanPartNo0C);
			pPlank0C->Thick=nXueBanThick;
			pPlank180C->Thick=nXueBanThick;

			pPlank0C->ucs.axis_z=pPlank180C->ucs.axis_z=pBaseJg->get_norm_x_wing();
			pPlank0C->ucs.axis_x=pPlank180C->ucs.axis_x=-tm.wing_vec_x;
		}
		else
		{	//X轴向前后板为一块板
			pPlank0C=m_pFoot->AppendPlank();
			pPlank0C->cMaterial=foot_para.m_cXueBanMat;
			pPlank0C->SetPartNo(sXueBanPartNo0C);
			pPlank0C->Thick=nXueBanThick;

			pPlank0C->ucs.axis_z=pBaseJg->get_norm_x_wing();
			pPlank0C->ucs.axis_x=-tm.wing_vec_x;
		}
		//Y轴向坐标系确定
		if(m_bBreakXueBan90C)
		{	//Y轴向前年板分开
			pPlank270C=m_pFoot->AppendPlank();
			pPlank90C=m_pFoot->AppendPlank();
			pPlank270C->cMaterial=foot_para.m_cXueBanMat;
			pPlank90C->cMaterial=foot_para.m_cXueBanMat;
			pPlank90C->SetPartNo(sXueBanPartNo90C);
			pPlank270C->SetPartNo(sXueBanPartNo270C);
			pPlank90C->Thick=nXueBanThick;
			pPlank270C->Thick=nXueBanThick;

			pPlank90C->ucs.axis_z=pPlank270C->ucs.axis_z=pBaseJg->get_norm_y_wing();
			pPlank90C->ucs.axis_x=pPlank270C->ucs.axis_x=tm.wing_vec_y;
		}
		else
		{	//Y轴向前后板为一块板
			pPlank90C=m_pFoot->AppendPlank();
			pPlank90C->cMaterial=foot_para.m_cXueBanMat;
			pPlank90C->SetPartNo(sXueBanPartNo90C);
			pPlank90C->Thick=nXueBanThick;

			pPlank90C->ucs.axis_z=pBaseJg->get_norm_y_wing();
			pPlank90C->ucs.axis_x=tm.wing_vec_y;
		}
	}
	else
	{	//2.2.1.2 X肢为牛蹄板Y轴向
		bAxisXSameToBaseWingX=FALSE;
		//Y轴向坐标系确定
		if(m_bBreakXueBan0C)
		{	//X肢向前后板分开
			pPlank0C=m_pFoot->AppendPlank();
			pPlank180C=m_pFoot->AppendPlank();
			pPlank0C->cMaterial=foot_para.m_cXueBanMat;
			pPlank180C->cMaterial=foot_para.m_cXueBanMat;
			pPlank0C->SetPartNo(sXueBanPartNo0C);
			pPlank180C->SetPartNo(sXueBanPartNo180C);
			pPlank0C->Thick=nXueBanThick;
			pPlank180C->Thick=nXueBanThick;

			pPlank0C->ucs.axis_z=pPlank180C->ucs.axis_z=pBaseJg->get_norm_x_wing();
			pPlank0C->ucs.axis_x=pPlank180C->ucs.axis_x=tm.wing_vec_x;
		}
		else
		{
			pPlank0C=m_pFoot->AppendPlank();
			pPlank0C->cMaterial=foot_para.m_cXueBanMat;
			pPlank0C->SetPartNo(sXueBanPartNo0C);
			pPlank0C->Thick=nXueBanThick;

			pPlank0C->ucs.axis_z=pBaseJg->get_norm_x_wing();
			pPlank0C->ucs.axis_x=tm.wing_vec_x;
		}
		//X轴向坐标系确定
		if(m_bBreakXueBan90C)
		{	//Y肢向前后板分开
			pPlank90C=m_pFoot->AppendPlank();
			pPlank270C=m_pFoot->AppendPlank();
			pPlank90C->cMaterial=foot_para.m_cXueBanMat;
			pPlank270C->cMaterial=foot_para.m_cXueBanMat;
			pPlank90C->SetPartNo(sXueBanPartNo90C);
			pPlank270C->SetPartNo(sXueBanPartNo270C);
			pPlank90C->Thick=nXueBanThick;
			pPlank270C->Thick=nXueBanThick;

			pPlank90C->ucs.axis_z=pPlank270C->ucs.axis_z=pBaseJg->get_norm_y_wing();
			pPlank90C->ucs.axis_x=pPlank270C->ucs.axis_x=-tm.wing_vec_y;
		}
		else
		{
			pPlank90C=m_pFoot->AppendPlank();
			pPlank90C->cMaterial=foot_para.m_cXueBanMat;
			pPlank90C->SetPartNo(sXueBanPartNo90C);
			pPlank90C->Thick=nXueBanThick;

			pPlank90C->ucs.axis_z= pBaseJg->get_norm_y_wing();
			pPlank90C->ucs.axis_x=-tm.wing_vec_y;
		}
	}
	//2.2.2生成靴板相对系原点
	if(m_bBreakXueBan0C)
	{
		pPlank0C->ucs.axis_y=pPlank180C->ucs.axis_y=pPlank180C->ucs.axis_z^pPlank180C->ucs.axis_x;
		//由于后面的外形轮廓点是按照默认位置计算的,所以原点偏移必须放在外形计算完毕后进行 wjh-2011.10.21
		if(m_pFoot->lj_by_weld==1)
			pPlank0C->ucs.origin=pPlank180C->ucs.origin=cur_end_pos;//-pPlank0C->ucs.axis_z*nXueBanThick;
		else
			pPlank0C->ucs.origin=pPlank180C->ucs.origin=cur_end_pos;
	}
	else
	{
		pPlank0C->ucs.axis_y=pPlank0C->ucs.axis_z^pPlank0C->ucs.axis_x;
		pPlank0C->ucs.origin=cur_end_pos;
	}
	if(m_bBreakXueBan90C)
	{
		pPlank90C->ucs.axis_y=pPlank270C->ucs.axis_y=pPlank270C->ucs.axis_z^pPlank270C->ucs.axis_x;
		if(m_pFoot->lj_by_weld==1)
			pPlank90C->ucs.origin=pPlank270C->ucs.origin=cur_end_pos;//-pPlank270C->ucs.axis_z*nXueBanThick;
		else
			pPlank90C->ucs.origin=pPlank270C->ucs.origin=cur_end_pos;
	}
	else
	{
		pPlank90C->ucs.axis_y=pPlank90C->ucs.axis_z^pPlank90C->ucs.axis_x;
		pPlank90C->ucs.origin=cur_end_pos;
	}

	//底脚板X轴向的靴板
	static double wrap_len=400;
	static CFootPlankWrapLenDlg wrapdlg;
	wrapdlg.weld_style = m_pFoot->lj_by_weld;
	wrapdlg.m_fRidgeWrapLen = wrap_len;
	wrapdlg.m_bBreakWingX=m_bBreakXueBan0C;
	wrapdlg.m_bBreakWingY=m_bBreakXueBan90C;
	if(m_pFoot->lj_by_weld&&wrapdlg.DoModal()==IDOK)
	{
		wrap_len = wrapdlg.m_fRidgeWrapLen;
		if(fabs(pBaseJg->GetWingVecX()*m_pFoot->ucs.axis_x)<fabs(pBaseJg->GetWingVecY()*m_pFoot->ucs.axis_x))
		{	//基准角钢X肢与底脚板的X轴方向不一致，需要参相应的参数进行转换
			double dd=wrapdlg.m_fXWingBackWrapWidth;
			wrapdlg.m_fXWingBackWrapWidth = wrapdlg.m_fYWingBackWrapWidth;
			wrapdlg.m_fYWingBackWrapWidth = dd;
			dd = wrapdlg.m_fXWingFrontOffset;
			wrapdlg.m_fXWingFrontOffset = wrapdlg.m_fYWingFrontOffset;
			wrapdlg.m_fYWingFrontOffset = dd;
		}
	}
	double origin_offset=0;
	f3dPoint tm_vertex;
	double B;
	if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		B=foot_para.m_iB+tm.wing_wide;
	else
		B=foot_para.m_iB;
	double A;
	if(tm.bUpJg)
		A=foot_para.m_iA;
	else	//牛蹄板下面的靴板
		A=-foot_para.m_iA;
	PROFILE_VERTEX *pVertex;
	if(m_pFoot->lj_by_weld==0)
	{	//螺栓连接
		origin_offset=0;
		//使用外部设定的螺栓间距及端距计算包容长度 wht 11-04-26
		if(ls_group.rows==1)//单排螺栓
		{
			wrap_len = 2*LsSpace.EndSpace+(ls_group.wnConnBoltN-1)*LsSpace.SingleRowSpace;
			wrap_len+=LsSpace.SingleRowSpace/2;	//螺栓包容长增加半个螺栓间距
		}
		else	//双排螺栓
			wrap_len = 2*LsSpace.EndSpace +(ls_group.wnConnBoltN-1)*LsSpace.doubleRowSpace/2;
		//wrap_len=GetLsGroupLen(ls_group.d,ls_group.N,ls_group.rows);
		//底脚板X轴向的肢
		if(bAxisXSameToBaseWingX)
		{
			if(pPlank180C)	//此轴向的靴板被截断
			{
				tm_vertex = tm.xueban_vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
				tm_vertex.z = 0;
				pPlank180C->vertex_list.append(tm_vertex);
				
				tm_vertex = tm.xueban_vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
				f3dPoint v_vec=CalVertVecOfParaA(pPlank180C->ucs);
				tm_vertex+=A*v_vec;
				//tm_vertex.y += A;
				pPlank180C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
				tm_vertex.x += nXueBanThick+B;
				tm_vertex.z = 0;
				pPlank180C->vertex_list.append(tm_vertex);

				tm_vertex.x -=B;
				pVertex=pPlank180C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
				f3dPoint h_vec(pPlank180C->ucs.axis_x.x,pPlank180C->ucs.axis_x.y);
				double cosa=h_vec.mod();
				vector_trans(h_vec,pPlank180C->ucs,FALSE);
				normalize(h_vec);
				tm_vertex += h_vec*(nXueBanThick/cosa);
				tm_vertex.z = 0;
				pVertex=pPlank180C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				tm_vertex.z = 0;
				pVertex=pPlank0C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				pPlank0C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len)-pPlank0C->ucs.axis_x*tm.wing_wide;
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				pPlank0C->vertex_list.append(tm_vertex);
				
				tm_vertex = tm.xueban_vert_arr[1];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				tm_vertex.z = 0;
				pVertex=pPlank0C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				pPlank180C->DesignProfile();
				pPlank0C->DesignProfile();
			}
			else
			{
				tm_vertex = tm.xueban_vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				tm_vertex.z = 0;
				pPlank0C->vertex_list.append(tm_vertex);
				
				tm_vertex = tm.xueban_vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				f3dPoint v_vec=CalVertVecOfParaA(pPlank0C->ucs);
				tm_vertex+=A*v_vec;
				//tm_vertex.y += A;
				pPlank0C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				tm_vertex.x += nXueBanThick+B;
				tm_vertex.z = 0;
				pPlank0C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len)-pPlank0C->ucs.axis_x*tm.wing_wide;
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				pPlank0C->vertex_list.append(tm_vertex);
				
				tm_vertex = tm.xueban_vert_arr[1];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				tm_vertex.z = 0;
				pVertex=pPlank0C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				pPlank0C->DesignProfile();
			}
		}
		else
		{
			if(pPlank180C)	//此轴向的靴板被截断
			{
				tm_vertex = tm.vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
				f3dPoint h_vec(pPlank180C->ucs.axis_x.x,pPlank180C->ucs.axis_x.y);
				double cosa=h_vec.mod();
				vector_trans(h_vec,pPlank180C->ucs,FALSE);
				normalize(h_vec);
				tm_vertex -= h_vec*(nXueBanThick/cosa);
				//tm_vertex.x += nXueBanThick;
				tm_vertex.z = 0;
				pVertex=pPlank180C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
				tm_vertex.x -= nXueBanThick;
				tm_vertex.z = 0;
				pPlank180C->vertex_list.append(tm_vertex);
				
				tm_vertex.x -=B;
				pPlank180C->vertex_list.append(tm_vertex);
				
				tm_vertex = tm.xueban_vert_arr[3];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
				f3dPoint v_vec=CalVertVecOfParaA(pPlank180C->ucs);
				tm_vertex+=A*v_vec;
				//tm_vertex.y += A;
				pPlank180C->vertex_list.append(tm_vertex);
				
				tm_vertex = tm.xueban_vert_arr[3];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
				tm_vertex.z = 0;
				pVertex=pPlank180C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				tm_vertex.z = 0;
				pVertex=pPlank0C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.xueban_vert_arr[2];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				tm_vertex.z = 0;
				pPlank0C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len)+pPlank0C->ucs.axis_x*tm.wing_wide;
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				pPlank0C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				pVertex=pPlank0C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				pPlank180C->DesignProfile();
				pPlank0C->DesignProfile();
			}
			else
			{
				tm_vertex = tm.xueban_vert_arr[3];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				f3dPoint v_vec=CalVertVecOfParaA(pPlank0C->ucs);
				tm_vertex+=A*v_vec;
				//tm_vertex.y += A;
				tm_vertex.z  = 0;
				pPlank0C->vertex_list.append(tm_vertex);
				tm_vertex-=A*v_vec;
				//tm_vertex.y -= A;
				pVertex=pPlank0C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.xueban_vert_arr[2];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				tm_vertex.z  = 0;
				pPlank0C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
				tm_vertex.x+= tm.wing_wide;
				tm_vertex.z = 0;
				pPlank0C->vertex_list.append(tm_vertex);
				tm_vertex.x-= tm.wing_wide+nXueBanThick+B;
				pPlank0C->vertex_list.append(tm_vertex);
				pPlank0C->DesignProfile();
			}
		}
		//底脚板Y轴向的肢
		if(bAxisXSameToBaseWingX)
		{
			if(pPlank270C)
			{
				tm_vertex = tm.vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
				f3dPoint h_vec(pPlank270C->ucs.axis_x.x,pPlank270C->ucs.axis_x.y);
				double cosa=h_vec.mod();
				vector_trans(h_vec,pPlank270C->ucs,FALSE);
				normalize(h_vec);
				tm_vertex -= h_vec*(nXueBanThick/cosa);
				//tm_vertex.x -= nXueBanThick;
				tm_vertex.z = 0;
				pVertex=pPlank270C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
				tm_vertex.x -= nXueBanThick;
				tm_vertex.z = 0;
				pPlank270C->vertex_list.append(tm_vertex);
				tm_vertex.x -=B;
				pPlank270C->vertex_list.append(tm_vertex);
				
				tm_vertex = tm.xueban_vert_arr[3];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
				f3dPoint v_vec=CalVertVecOfParaA(pPlank270C->ucs);
				tm_vertex+=A*v_vec;
				//tm_vertex.y += A;
				tm_vertex.z = 0;
				pPlank270C->vertex_list.append(tm_vertex);
				
				tm_vertex-=A*v_vec;
				//tm_vertex.y -= A;
				pVertex=pPlank270C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.xueban_vert_arr[2];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.x += tm.wing_wide;
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				tm_vertex.x -= tm.wing_wide;
				pVertex=pPlank90C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.z = 0;
				pVertex=pPlank90C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				pPlank270C->DesignProfile();
				pPlank90C->DesignProfile();
			}
			else
			{
				tm_vertex = tm.xueban_vert_arr[3];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				f3dPoint v_vec=CalVertVecOfParaA(pPlank90C->ucs);
				tm_vertex+=A*v_vec;
				//tm_vertex.y += A;
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				
				tm_vertex-=A*v_vec;
				//tm_vertex.y -= A;
				pVertex=pPlank90C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.xueban_vert_arr[2];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.x += tm.wing_wide;
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.x -= nXueBanThick+B;
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				
				pPlank90C->DesignProfile();
			}
		}
		else
		{
			if(pPlank270C)
			{
				tm_vertex = tm.vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
				f3dPoint h_vec(pPlank270C->ucs.axis_x.x,pPlank270C->ucs.axis_x.y);
				double cosa=h_vec.mod();
				vector_trans(h_vec,pPlank270C->ucs,FALSE);
				normalize(h_vec);
				tm_vertex += h_vec*(nXueBanThick/cosa);
				//tm_vertex.x += nXueBanThick;
				tm_vertex.z = 0;
				pVertex=pPlank270C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.xueban_vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
				tm_vertex.z = 0;
				pPlank270C->vertex_list.append(tm_vertex);
				f3dPoint v_vec=CalVertVecOfParaA(pPlank270C->ucs);
				tm_vertex+=A*v_vec;
				//tm_vertex.y += A;
				tm_vertex.z = 0;
				pPlank270C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
				tm_vertex.x +=nXueBanThick+B;
				tm_vertex.z = 0;
				pPlank270C->vertex_list.append(tm_vertex);
				tm_vertex.x -=B;
				pVertex=pPlank270C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.xueban_vert_arr[1];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.z = 0;
				pVertex=pPlank90C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.z = 0;
				pVertex=pPlank90C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				tm_vertex.x -= tm.wing_wide;
				pPlank90C->vertex_list.append(tm_vertex);
				
				pPlank270C->DesignProfile();
				pPlank90C->DesignProfile();
			}
			else
			{
				tm_vertex = tm.xueban_vert_arr[0];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				f3dPoint v_vec=CalVertVecOfParaA(pPlank90C->ucs);
				tm_vertex+=A*v_vec;
				//tm_vertex.y += A;
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				
				tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.x +=nXueBanThick+B;
				tm_vertex.z = 0;
				pPlank90C->vertex_list.append(tm_vertex);
				tm_vertex.x -=(B+nXueBanThick+tm.wing_wide);
				pPlank90C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				tm_vertex = tm.xueban_vert_arr[1];
				coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
				coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
				tm_vertex.z = 0;
				pVertex=pPlank90C->vertex_list.append(tm_vertex);
				pVertex->m_bWeldEdge=TRUE;
				
				pPlank90C->DesignProfile();
			}
		}
	}
	else if(m_pFoot->lj_by_weld==1)	//对焊
	{
		origin_offset=nXueBanThick;
		//底脚板Y轴向的肢
		tm_vertex = tm.vert_arr[0];
		coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
		coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
		tm_vertex.z=0;
		pVertex=pPlank270C->vertex_list.append(tm_vertex);
		pVertex->m_bWeldEdge=TRUE;

		tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
		coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
		tm_vertex.z=0;
		pPlank270C->vertex_list.append(tm_vertex);

		tm_vertex.x -=B;
		pPlank270C->vertex_list.append(tm_vertex);

		tm_vertex = tm.xueban_vert_arr[3];
		coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
		coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
		f3dPoint v_vec=CalVertVecOfParaA(pPlank270C->ucs);
		tm_vertex+=foot_para.m_iA*v_vec;
		//tm_vertex.y += foot_para.m_iA;
		tm_vertex.z=0;
		pPlank270C->vertex_list.append(tm_vertex);

		tm_vertex-=foot_para.m_iA*v_vec;
		//tm_vertex.y -= foot_para.m_iA;
		pVertex=pPlank270C->vertex_list.append(tm_vertex);
		pVertex->m_bWeldEdge=TRUE;

		f3dPoint pick;
		pick = cur_end_pos+tm.wing_wide*pPlank90C->ucs.axis_x;
		Int3dlf(tm_vertex,pick,tm.jg_vec,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
		coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
		tm_vertex.z=0;
		pVertex=pPlank90C->vertex_list.append(tm_vertex);
		pVertex->m_bWeldEdge=TRUE;

		tm_vertex = tm.xueban_vert_arr[2];
		coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
		coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
		tm_vertex.z=0;
		pPlank90C->vertex_list.append(tm_vertex);

		tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
		tm_vertex = tm_vertex+pPlank90C->ucs.axis_x*tm.wing_wide;
		coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
		tm_vertex.z=0;
		pVertex=pPlank90C->vertex_list.append(tm_vertex);
		pVertex->m_bWeldEdge=TRUE;

		//底脚板X轴向的肢
		tm_vertex = tm.vert_arr[0];
		coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
		coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
		tm_vertex.z=0;
		pVertex=pPlank180C->vertex_list.append(tm_vertex);
		pVertex->m_bWeldEdge=TRUE;

		tm_vertex = tm.xueban_vert_arr[0];
		coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
		coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
		tm_vertex.z=0;
		pPlank180C->vertex_list.append(tm_vertex);
		v_vec=CalVertVecOfParaA(pPlank180C->ucs);
		tm_vertex+=foot_para.m_iA*v_vec;
		//tm_vertex.y += foot_para.m_iA;
		pPlank180C->vertex_list.append(tm_vertex);

		tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
		coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
		tm_vertex.x +=B;
		tm_vertex.z=0;
		pPlank180C->vertex_list.append(tm_vertex);

		tm_vertex.x -=B;
		pVertex=pPlank180C->vertex_list.append(tm_vertex);
		pVertex->m_bWeldEdge=TRUE;

		pick = cur_end_pos-tm.wing_wide*pPlank0C->ucs.axis_x;
		Int3dlf(tm_vertex,pick,tm.jg_vec,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
		coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
		tm_vertex.z=0;
		pVertex=pPlank0C->vertex_list.append(tm_vertex);
		pVertex->m_bWeldEdge=TRUE;

		tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
		coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
		tm_vertex.x -= tm.wing_wide;
		tm_vertex.z=0;
		pPlank0C->vertex_list.append(tm_vertex);

		tm_vertex = tm.xueban_vert_arr[1];
		coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
		coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
		tm_vertex.z=0;
		pVertex=pPlank0C->vertex_list.append(tm_vertex);
		pVertex->m_bWeldEdge=TRUE;
		//更新内侧对焊靴板装配原点位置
		f3dPoint offset_vec(pPlank0C->ucs.axis_z.x,pPlank0C->ucs.axis_z.y);
		double sina2=offset_vec.x*offset_vec.x+offset_vec.y*offset_vec.y;
		pPlank0C->ucs.origin-=(nXueBanThick/sina2)*offset_vec;//pPlank180C->ucs.axis_z;
		pPlank0C->DesignProfile();
		offset_vec.Set(pPlank90C->ucs.axis_z.x,pPlank90C->ucs.axis_z.y);
		sina2=offset_vec.x*offset_vec.x+offset_vec.y*offset_vec.y;
		pPlank90C->ucs.origin-=(nXueBanThick/sina2)*offset_vec;//pPlank270C->ucs.axis_z;
		pPlank90C->DesignProfile();
		//更新焊接靴板装配原点位置
		offset_vec.Set(pPlank180C->ucs.axis_z.x,pPlank180C->ucs.axis_z.y);
		sina2=offset_vec.x*offset_vec.x+offset_vec.y*offset_vec.y;
		pPlank180C->ucs.origin-=((nXueBanThick+wrapdlg.m_fXWingFrontOffset)/sina2)*offset_vec;//pPlank180C->ucs.axis_z;
		pPlank180C->DesignProfile();
		offset_vec.Set(pPlank270C->ucs.axis_z.x,pPlank270C->ucs.axis_z.y);
		sina2=offset_vec.x*offset_vec.x+offset_vec.y*offset_vec.y;
		pPlank270C->ucs.origin-=((nXueBanThick+wrapdlg.m_fYWingFrontOffset)/sina2)*offset_vec;//pPlank270C->ucs.axis_z;
		pPlank270C->DesignProfile();
	}
	else //if(m_pFoot->lj_by_weld==2)	//搭焊
	{
		origin_offset=nXueBanThick;

		if(pPlank180C)
		{
			//底脚板X轴向的肢
			tm_vertex = tm.vert_arr[0];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
			tm_vertex.z=0;
			pVertex=pPlank180C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;

			tm_vertex = tm.xueban_vert_arr[0];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
			tm_vertex.z=0;
			pPlank180C->vertex_list.append(tm_vertex);
			f3dPoint v_vec=CalVertVecOfParaA(pPlank180C->ucs);
			tm_vertex+=foot_para.m_iA*v_vec;
			//tm_vertex.y += foot_para.m_iA;
			pPlank180C->vertex_list.append(tm_vertex);

			tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
			coord_trans(tm_vertex,pPlank180C->ucs,FALSE);
			tm_vertex.x +=B;
			tm_vertex.z=0;
			pPlank180C->vertex_list.append(tm_vertex);

			tm_vertex.x -=B;
			pVertex=pPlank180C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;

			f3dPoint pick;
			pick = cur_end_pos-(tm.wing_wide-wrapdlg.m_fXWingBackWrapWidth)*pPlank0C->ucs.axis_x;
			Int3dlf(tm_vertex,pick,tm.jg_vec,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
			coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
			tm_vertex.z=0;
			pVertex=pPlank0C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;

			tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
			coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
			tm_vertex.x -= tm.wing_wide-wrapdlg.m_fXWingBackWrapWidth;
			tm_vertex.z=0;
			pPlank0C->vertex_list.append(tm_vertex);
			if(wrapdlg.m_fXWingBackWrapWidth!=0)
			{
				tm_vertex.x-=wrapdlg.m_fXWingBackWrapWidth;
				pPlank0C->vertex_list.append(tm_vertex);
			}

			tm_vertex = tm.xueban_vert_arr[1];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
			tm_vertex.z=0;
			pVertex=pPlank0C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;
			
			f3dPoint offset_vec(pPlank180C->ucs.axis_z.x,pPlank180C->ucs.axis_z.y);
			double sina2=offset_vec.x*offset_vec.x+offset_vec.y*offset_vec.y;
			pPlank180C->ucs.origin-=(wrapdlg.m_fXWingFrontOffset/sina2)*offset_vec;//pPlank180C->ucs.axis_z;
			pPlank180C->DesignProfile();
		}
		else
		{
			//底脚板X轴向的肢
			tm_vertex = tm.xueban_vert_arr[0];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
			tm_vertex.z=0;
			pPlank0C->vertex_list.append(tm_vertex);
			f3dPoint v_vec=CalVertVecOfParaA(pPlank0C->ucs);
			tm_vertex+=foot_para.m_iA*v_vec;
			//tm_vertex.y += foot_para.m_iA;
			pPlank0C->vertex_list.append(tm_vertex);

			tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
			coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
			tm_vertex.x +=B;
			tm_vertex.z=0;
			pPlank0C->vertex_list.append(tm_vertex);

			tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
			coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
			tm_vertex.x -= B+tm.wing_wide;
			tm_vertex.z=0;
			pVertex=pPlank0C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;

			tm_vertex = tm.xueban_vert_arr[1];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank0C->ucs,FALSE);
			tm_vertex.z=0;
			pVertex=pPlank0C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;
		}
		//底脚板Y轴向的肢
		if(pPlank270C)
		{
			//底脚板Y轴向的肢
			tm_vertex = tm.vert_arr[0];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
			tm_vertex.z=0;
			pVertex=pPlank270C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;

			tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
			coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
			tm_vertex.z=0;
			pPlank270C->vertex_list.append(tm_vertex);

			tm_vertex.x -=B;
			pPlank270C->vertex_list.append(tm_vertex);

			tm_vertex = tm.xueban_vert_arr[3];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank270C->ucs,FALSE);
			f3dPoint v_vec=CalVertVecOfParaA(pPlank270C->ucs);
			tm_vertex+=foot_para.m_iA*v_vec;
			//tm_vertex.y += foot_para.m_iA;
			tm_vertex.z=0;
			pPlank270C->vertex_list.append(tm_vertex);

			tm_vertex-=foot_para.m_iA*v_vec;
			//tm_vertex.y -= foot_para.m_iA;
			pVertex=pPlank270C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;

			f3dPoint pick;
			pick = cur_end_pos+(tm.wing_wide-wrapdlg.m_fYWingBackWrapWidth)*pPlank90C->ucs.axis_x;
			Int3dlf(tm_vertex,pick,tm.jg_vec,m_pFoot->ucs.origin,m_pFoot->ucs.axis_z);
			coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
			tm_vertex.z=0;
			pVertex=pPlank90C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;

			tm_vertex = tm.xueban_vert_arr[2];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
			tm_vertex.z=0;
			pPlank90C->vertex_list.append(tm_vertex);

			tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
			tm_vertex = tm_vertex+pPlank90C->ucs.axis_x*tm.wing_wide;
			coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
			tm_vertex.z=0;
			pVertex=pPlank90C->vertex_list.append(tm_vertex);
			if(wrapdlg.m_fYWingBackWrapWidth!=0)
			{
				tm_vertex.x-=wrapdlg.m_fYWingBackWrapWidth;
				pVertex=pPlank90C->vertex_list.append(tm_vertex);
			}
			pVertex->m_bWeldEdge=TRUE;

			f3dPoint offset_vec(pPlank270C->ucs.axis_z.x,pPlank270C->ucs.axis_z.y);
			double sina2=offset_vec.x*offset_vec.x+offset_vec.y*offset_vec.y;
			pPlank270C->ucs.origin-=(wrapdlg.m_fYWingFrontOffset/sina2)*offset_vec;//pPlank270C->ucs.axis_z;
			pPlank270C->DesignProfile();
		}
		else
		{
			//底脚板Y轴向的肢
			tm_vertex = tm.xueban_vert_arr[2];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
			tm_vertex.z=0;
			pPlank90C->vertex_list.append(tm_vertex);

			tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
			tm_vertex = tm_vertex+pPlank90C->ucs.axis_x*tm.wing_wide;
			coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
			tm_vertex.z=0;
			pVertex=pPlank90C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;

			tm_vertex = cur_end_pos+tm.jg_vec*(cur_oddment-wrap_len);
			coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
			tm_vertex.x -=B;
			tm_vertex.z=0;
			pPlank90C->vertex_list.append(tm_vertex);

			tm_vertex = tm.xueban_vert_arr[3];
			coord_trans(tm_vertex,m_pFoot->ucs,TRUE);
			coord_trans(tm_vertex,pPlank90C->ucs,FALSE);
			f3dPoint v_vec=CalVertVecOfParaA(pPlank90C->ucs);
			tm_vertex+=foot_para.m_iA*v_vec;
			//tm_vertex.y += foot_para.m_iA;
			tm_vertex.z=0;
			pPlank90C->vertex_list.append(tm_vertex);

			tm_vertex-=foot_para.m_iA*v_vec;
			//tm_vertex.y -= foot_para.m_iA;
			pVertex=pPlank90C->vertex_list.append(tm_vertex);
			pVertex->m_bWeldEdge=TRUE;
		}
	}
	if(foot_para.m_iDiBanShape==1&&pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{	//对角组合角钢的圆塔脚板，一般有对角线上的辅助靴板
		pPlank45C =m_pFoot->AppendPlank();
		pPlank135C=m_pFoot->AppendPlank();
		pPlank225C=m_pFoot->AppendPlank();
		pPlank315C=m_pFoot->AppendPlank();
		pPlank45C->Thick=nXueBanThick;
		pPlank135C->Thick=nXueBanThick;
		pPlank225C->Thick=nXueBanThick;
		pPlank315C->Thick=nXueBanThick;
		pPlank45C->cMaterial=foot_para.m_cXueBanMat;
		pPlank135C->cMaterial=foot_para.m_cXueBanMat;
		pPlank225C->cMaterial=foot_para.m_cXueBanMat;
		pPlank315C->cMaterial=foot_para.m_cXueBanMat;
		pPlank45C->SetPartNo(sXueBanPartNo45C);
		pPlank135C->SetPartNo(sXueBanPartNo135C);
		pPlank225C->SetPartNo(sXueBanPartNo225C);
		pPlank315C->SetPartNo(sXueBanPartNo315C);

		if(tm.bUpJg)
			pPlank45C->ucs.axis_x=RayVecFromUpJgAngle(45);
		else
			pPlank45C->ucs.axis_x=RayVecFromDownJgAngle(45);
		pPlank45C->ucs.origin=pPlank45C->ucs.axis_x*(foot_para.m_iWide*0.5);
		if(!tm.bUpJg)
			pPlank45C->ucs.origin.z-=foot_para.m_iThick;
		coord_trans(pPlank45C->ucs.origin,m_pFoot->ucs,TRUE);
		vector_trans(pPlank45C->ucs.axis_x,m_pFoot->ucs,TRUE);
		pPlank45C->ucs.axis_y=-tm.jg_vec;
		pPlank45C->ucs.axis_z=pPlank45C->ucs.axis_x^pPlank45C->ucs.axis_y;
		normalize(pPlank45C->ucs.axis_z);
		normalize(pPlank45C->ucs.axis_x);
		pPlank45C->ucs.axis_y=pPlank45C->ucs.axis_z^pPlank45C->ucs.axis_x;
		pPlank45C->ucs.origin.x-=pPlank45C->ucs.axis_z.x*nXueBanThick*0.5;
		pPlank45C->ucs.origin.y-=pPlank45C->ucs.axis_z.y*nXueBanThick*0.5;

		if(tm.bUpJg)
			pPlank135C->ucs.axis_x=RayVecFromUpJgAngle(135);
		else
			pPlank135C->ucs.axis_x=RayVecFromDownJgAngle(135);
		pPlank135C->ucs.origin=pPlank135C->ucs.axis_x*(foot_para.m_iWide*0.5);
		if(!tm.bUpJg)
			pPlank135C->ucs.origin.z-=foot_para.m_iThick;
		coord_trans(pPlank135C->ucs.origin,m_pFoot->ucs,TRUE);
		vector_trans(pPlank135C->ucs.axis_x,m_pFoot->ucs,TRUE);
		pPlank135C->ucs.axis_y=-tm.jg_vec;
		pPlank135C->ucs.axis_z=pPlank135C->ucs.axis_x^pPlank135C->ucs.axis_y;
		normalize(pPlank135C->ucs.axis_z);
		normalize(pPlank135C->ucs.axis_x);
		pPlank135C->ucs.axis_y=pPlank135C->ucs.axis_z^pPlank135C->ucs.axis_x;
		pPlank135C->ucs.origin.x-=pPlank135C->ucs.axis_z.x*nXueBanThick*0.5;
		pPlank135C->ucs.origin.y-=pPlank135C->ucs.axis_z.y*nXueBanThick*0.5;

		if(tm.bUpJg)
			pPlank225C->ucs.axis_x=RayVecFromUpJgAngle(225);
		else
			pPlank225C->ucs.axis_x=RayVecFromDownJgAngle(225);
		pPlank225C->ucs.origin=pPlank225C->ucs.axis_x*(foot_para.m_iWide*0.5);
		if(!tm.bUpJg)
			pPlank225C->ucs.origin.z-=foot_para.m_iThick;
		coord_trans(pPlank225C->ucs.origin,m_pFoot->ucs,TRUE);
		vector_trans(pPlank225C->ucs.axis_x,m_pFoot->ucs,TRUE);
		pPlank225C->ucs.axis_y=-tm.jg_vec;
		pPlank225C->ucs.axis_z=pPlank225C->ucs.axis_x^pPlank225C->ucs.axis_y;
		normalize(pPlank225C->ucs.axis_z);
		normalize(pPlank225C->ucs.axis_x);
		pPlank225C->ucs.axis_y=pPlank225C->ucs.axis_z^pPlank225C->ucs.axis_x;
		pPlank225C->ucs.origin.x-=pPlank225C->ucs.axis_z.x*nXueBanThick*0.5;
		pPlank225C->ucs.origin.y-=pPlank225C->ucs.axis_z.y*nXueBanThick*0.5;

		if(tm.bUpJg)
			pPlank315C->ucs.axis_x=RayVecFromUpJgAngle(315);
		else
			pPlank315C->ucs.axis_x=RayVecFromDownJgAngle(315);
		pPlank315C->ucs.origin=pPlank315C->ucs.axis_x*(foot_para.m_iWide*0.5);
		if(!tm.bUpJg)
			pPlank315C->ucs.origin.z-=foot_para.m_iThick;
		coord_trans(pPlank315C->ucs.origin,m_pFoot->ucs,TRUE);
		vector_trans(pPlank315C->ucs.axis_x,m_pFoot->ucs,TRUE);
		pPlank315C->ucs.axis_y=-tm.jg_vec;
		pPlank315C->ucs.axis_z=pPlank315C->ucs.axis_x^pPlank315C->ucs.axis_y;
		normalize(pPlank315C->ucs.axis_z);
		normalize(pPlank315C->ucs.axis_x);
		pPlank315C->ucs.axis_y=pPlank315C->ucs.axis_z^pPlank315C->ucs.axis_x;
		pPlank315C->ucs.origin.x-=pPlank315C->ucs.axis_z.x*nXueBanThick*0.5;
		pPlank315C->ucs.origin.y-=pPlank315C->ucs.axis_z.y*nXueBanThick*0.5;

		PROFILE_VERTEX *pVertPntArr[6];
		double rAngle=FindJg_r(pBaseJg->GetWidth());
		double jg_space=((CLDSGroupLineAngle*)pBaseJg)->jg_space;
		f3dPoint vertical_offset=pPlank45C->ucs.axis_z^tm.jg_vec;
		normalize(vertical_offset);
		f3dPoint vertice=m_pFoot->ucs.origin+pPlank45C->ucs.axis_x*((jg_space+2*pBaseJg->GetThick())*cos45+CalJgROffset(rAngle,nXueBanThick));
		if(!tm.bUpJg)
			vertice-=m_pFoot->ucs.axis_z*foot_para.m_iThick;
		vertice.x-=pPlank45C->ucs.axis_z.x*pPlank45C->GetThick()*0.5;
		vertice.y-=pPlank45C->ucs.axis_z.y*pPlank45C->GetThick()*0.5;
		pVertPntArr[0]=pPlank45C->vertex_list.append(PROFILE_VERTEX(vertice));
		pVertPntArr[1]=pPlank45C->vertex_list.append(pPlank45C->ucs.origin);
		pVertPntArr[2]=pPlank45C->vertex_list.append(pVertPntArr[1]->vertex+pPlank45C->ucs.axis_y*A);
		vertice=pVertPntArr[0]->vertex-tm.jg_vec*(wrap_len-cur_oddment);
		pVertPntArr[3]=pPlank45C->vertex_list.append(vertice+pPlank45C->ucs.axis_x*B);
		pVertPntArr[4]=pPlank45C->vertex_list.append(PROFILE_VERTEX(vertice));
		pVertPntArr[0]->m_bWeldEdge=TRUE;
		pVertPntArr[4]->m_bWeldEdge=TRUE;
		int i;
		for(i=0;i<5;i++)
		{
			coord_trans(pVertPntArr[i]->vertex,pPlank45C->ucs,FALSE);
			pVertPntArr[i]->vertex.feature=1;
		}

		vertical_offset=pPlank135C->ucs.axis_z^tm.jg_vec;
		normalize(vertical_offset);
		vertice=m_pFoot->ucs.origin+pPlank135C->ucs.axis_x*(jg_space*cos45+nXueBanThick*0.5);
		if(!tm.bUpJg)
			vertice-=m_pFoot->ucs.axis_z*foot_para.m_iThick;
		vertice.x-=pPlank135C->ucs.axis_z.x*pPlank135C->GetThick()*0.5;
		vertice.y-=pPlank135C->ucs.axis_z.y*pPlank135C->GetThick()*0.5;
		pVertPntArr[0]=pPlank135C->vertex_list.append(PROFILE_VERTEX(vertice));
		pVertPntArr[1]=pPlank135C->vertex_list.append(PROFILE_VERTEX(pPlank135C->ucs.origin));
		pVertPntArr[2]=pPlank135C->vertex_list.append(pVertPntArr[1]->vertex+pPlank135C->ucs.axis_y*A);
		vertice=pVertPntArr[0]->vertex-tm.jg_vec*(wrap_len-cur_oddment);
		pVertPntArr[3]=pPlank135C->vertex_list.append(vertice+pPlank135C->ucs.axis_x*B);
		pVertPntArr[4]=pPlank135C->vertex_list.append(PROFILE_VERTEX(vertice));
		pVertPntArr[0]->m_bWeldEdge=TRUE;
		pVertPntArr[4]->m_bWeldEdge=TRUE;
		for(i=0;i<5;i++)
		{
			coord_trans(pVertPntArr[i]->vertex,pPlank135C->ucs,FALSE);
			pVertPntArr[i]->vertex.feature=1;
		}

		vertical_offset=pPlank225C->ucs.axis_z^tm.jg_vec;
		normalize(vertical_offset);
		vertice=m_pFoot->ucs.origin+pPlank225C->ucs.axis_x*((jg_space+2*pBaseJg->GetThick())*cos45+CalJgROffset(rAngle,nXueBanThick));
		if(!tm.bUpJg)
			vertice-=m_pFoot->ucs.axis_z*foot_para.m_iThick;
		vertice.x-=pPlank225C->ucs.axis_z.x*pPlank225C->GetThick()*0.5;
		vertice.y-=pPlank225C->ucs.axis_z.y*pPlank225C->GetThick()*0.5;
		pVertPntArr[0]=pPlank225C->vertex_list.append(PROFILE_VERTEX(vertice));
		pVertPntArr[1]=pPlank225C->vertex_list.append(PROFILE_VERTEX(pPlank225C->ucs.origin));
		pVertPntArr[2]=pPlank225C->vertex_list.append(PROFILE_VERTEX(pVertPntArr[1]->vertex+pPlank225C->ucs.axis_y*A));
		vertice=pVertPntArr[0]->vertex-tm.jg_vec*(wrap_len-cur_oddment);
		pVertPntArr[3]=pPlank225C->vertex_list.append(PROFILE_VERTEX(vertice+pPlank225C->ucs.axis_x*B));
		pVertPntArr[4]=pPlank225C->vertex_list.append(PROFILE_VERTEX(vertice));
		pVertPntArr[0]->m_bWeldEdge=TRUE;
		pVertPntArr[4]->m_bWeldEdge=TRUE;
		for(i=0;i<5;i++)
		{
			coord_trans(pVertPntArr[i]->vertex,pPlank225C->ucs,FALSE);
			pVertPntArr[i]->vertex.feature=1;
		}

		vertical_offset=pPlank315C->ucs.axis_z^tm.jg_vec;
		normalize(vertical_offset);
		vertice=m_pFoot->ucs.origin+pPlank315C->ucs.axis_x*(jg_space*cos45+nXueBanThick*0.5);
		if(!tm.bUpJg)
			vertice-=m_pFoot->ucs.axis_z*foot_para.m_iThick;
		vertice.x-=pPlank315C->ucs.axis_z.x*pPlank315C->GetThick()*0.5;
		vertice.y-=pPlank315C->ucs.axis_z.y*pPlank315C->GetThick()*0.5;
		pVertPntArr[0]=pPlank315C->vertex_list.append(PROFILE_VERTEX(vertice));
		pVertPntArr[1]=pPlank315C->vertex_list.append(PROFILE_VERTEX(pPlank315C->ucs.origin));
		pVertPntArr[2]=pPlank315C->vertex_list.append(PROFILE_VERTEX(pVertPntArr[1]->vertex+pPlank315C->ucs.axis_y*A));
		vertice=pVertPntArr[0]->vertex-tm.jg_vec*(wrap_len-cur_oddment);
		pVertPntArr[3]=pPlank315C->vertex_list.append(PROFILE_VERTEX(vertice+pPlank315C->ucs.axis_x*B));
		pVertPntArr[4]=pPlank315C->vertex_list.append(PROFILE_VERTEX(vertice));
		pVertPntArr[0]->m_bWeldEdge=TRUE;
		pVertPntArr[4]->m_bWeldEdge=TRUE;
		for(i=0;i<5;i++)
		{
			coord_trans(pVertPntArr[i]->vertex,pPlank315C->ucs,FALSE);
			pVertPntArr[i]->vertex.feature=1;
		}
	}

	if(pPlank180C==NULL)
		pPlank180C=pPlank0C;
	if(pPlank270C==NULL)
		pPlank270C=pPlank90C;
	if(tm.bUpJg)
	{	//设计上侧靴板
		tm.pXUpFrontPlank=pPlank180C;
		tm.pXUpBackPlank =pPlank0C;
		tm.pYUpFrontPlank=pPlank270C;
		tm.pYUpBackPlank =pPlank90C;
		tm.upXueBan[0]=pPlank0C;
		tm.upXueBan[1]=pPlank45C;
		tm.upXueBan[2]=pPlank90C;
		tm.upXueBan[3]=pPlank135C;
		tm.upXueBan[4]=pPlank180C;
		tm.upXueBan[5]=pPlank225C;
		tm.upXueBan[6]=pPlank270C;
		tm.upXueBan[7]=pPlank315C;
	}
	else
	{	//设计下侧靴板
		tm.pXDownFrontPlank=pPlank180C;
		tm.pXDownBackPlank =pPlank0C;
		tm.pYDownFrontPlank=pPlank270C;
		tm.pYDownBackPlank =pPlank90C;
		tm.downXueBan[0]=pPlank0C;
		tm.downXueBan[1]=pPlank45C;
		tm.downXueBan[2]=pPlank90C;
		tm.downXueBan[3]=pPlank135C;
		tm.downXueBan[4]=pPlank180C;
		tm.downXueBan[5]=pPlank225C;
		tm.downXueBan[6]=pPlank270C;
		tm.downXueBan[7]=pPlank315C;
	}
}

// 布置基准角钢上的螺栓
void CFootDesignDlg::LayBaseJgLs(CLDSLineAngle * pBaseJg)
{
	int iLsLayStyle;
	if(tm.bUpJg)
		iLsLayStyle=tm.m_iUpLsLayStyle;
	else
		iLsLayStyle=tm.m_iDownLsLayStyle;
	int nXueBanThick;
	if(tm.bUpJg)
		nXueBanThick=foot_para.m_iUpXueBanThick;
	else
		nXueBanThick=foot_para.m_iDownXueBanThick;
	//
	CConnectInfo ls_group;
	LSSPACE_STRU LsSpace;
	CLDSPoint datumPointX,datumPointY;
	if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
	{
		//获得基准材螺栓间距信息
		ls_group=pBaseJg->connectStart;
		//设置连接信息及螺栓端距,间距 wht 11-04-26
		GetLsSpace(LsSpace,ls_group.d);
		if(ls_group.LsSpace.DoubleRowSpace>0&&ls_group.LsSpace.EndSpace>0
			&&ls_group.LsSpace.SingleRowSpace>0)
		{
			LsSpace.EndSpace=ls_group.LsSpace.EndSpace;
			LsSpace.SingleRowSpace=ls_group.LsSpace.SingleRowSpace;
			LsSpace.doubleRowSpace=ls_group.LsSpace.DoubleRowSpace;
		}
		//布置主材螺栓
		if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)	//组合角钢
		{	//布置始端主材上的螺栓
			CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
			if(pGroupJg->group_style==0)	//对角组合型牛蹄板基准材
			{
				CLDSLineAngle *pJgArr[2];
				pJgArr[0]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
				pJgArr[1]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
				for(int ii=0;ii<2;ii++)
				{
					CLDSLineAngle *pSonBaseJg=pJgArr[ii];
					JGZJ jgzj_x,jgzj_y;
					f3dPoint wing_vec_x=pSonBaseJg->GetWingVecX();
					f3dPoint wing_vec_y=pSonBaseJg->GetWingVecY();
					if(!getjgzj(jgzj_x,tm.wing_wide))
					{
						char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Couldn't find the width %d of angle's leg from angle stadia library!",tm.wing_wide);
#else 
						_snprintf(sError,199,"找不到肢宽%d的角钢准距数据!",tm.wing_wide);
#endif
						throw sError;
					}
					jgzj_y=jgzj_x;
					if(pSonBaseJg->m_bEnableTeG)
					{
						jgzj_x = pSonBaseJg->xWingXZhunJu;
						jgzj_y = pSonBaseJg->xWingYZhunJu;
					}
					CLDSBolt ls_x(console.GetActiveModel()),ls_y(console.GetActiveModel());
					datumPointX.des_para.RODEND.offset_wing=0;
					datumPointY.des_para.RODEND.offset_wing=1;
					datumPointX.datum_pos_style					=datumPointY.datum_pos_style=1;
					datumPointX.des_para.RODEND.hRod			=datumPointY.des_para.RODEND.hRod=pSonBaseJg->handle;
					datumPointX.des_para.RODEND.direction		=datumPointY.des_para.RODEND.direction=0;
					datumPointX.des_para.RODEND.len_offset_dist	=datumPointY.des_para.RODEND.len_offset_dist=0;
					datumPointX.des_para.RODEND.bIncOddEffect	=datumPointY.des_para.RODEND.bIncOddEffect=TRUE;
					datumPointX.des_para.RODEND.wing_offset_dist=datumPointY.des_para.RODEND.wing_offset_dist=0;
					datumPointX.des_para.RODEND.wing_offset_style=datumPointY.des_para.RODEND.wing_offset_style=4;
					ls_x.set_d(ls_group.d);
					ls_y.set_d(ls_group.d);
					ls_x.AddL0Thick(nXueBanThick,FALSE);
					ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
					ls_y.AddL0Thick(nXueBanThick,FALSE);
					ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
					if(!ls_x.CalGuigeAuto())
					{
						char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Can't find the specification M%dX%d of the bolt!",ls_group.d,ls_x.get_L0());
#else 
						_snprintf(sError,199,"找不到M%dX%d规格的螺栓!",ls_group.d,ls_x.get_L0());
#endif
						throw sError;
					}
					ls_x.iSeg=pSonBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
					ls_y.CopyProperty(&ls_x);
					ls_x.set_norm(pSonBaseJg->get_norm_x_wing());
					ls_y.set_norm(pSonBaseJg->get_norm_y_wing());

					f3dPoint ls_pos_x,ls_pos_y;
					int offset = LsSpace.EndSpace;
					for(int j=0;j<pSonBaseJg->connectStart.wnConnBoltN;j++)
					{
						if(pSonBaseJg->connectStart.rows==1)
						{		//单排螺栓
							//ls_x.dwRayNo = 1;
							{	//单排螺栓螺栓正常排列
								if(iLsLayStyle==0)	//左高右底
								{
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
								else
								{
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
							}
							offset += LsSpace.SingleRowSpace;
						}
						else	//双排螺栓
						{
							if(iLsLayStyle==0)
							{	//靠近楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(2);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(3);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
							}
							else
							{	//背离楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
							}
							offset+=LsSpace.doubleRowSpace/2;
						}
						CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_x = ls_pos_x-pSonBaseJg->get_norm_x_wing()*tm.wing_thick;
						_snprintf(ls_x.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
						ls_x.ucs.origin=ls_pos_x;
						pRealLs->CopyProperty(&ls_x);
						pRealLs->cfgword=pSonBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointX;
						pRealLs->des_base_pos.offset_wing=0;
						pRealLs->des_base_pos.direction=0;
						pRealLs->des_base_pos.hPart=pRealLs->des_base_pos.datumPoint.des_para.RODEND.hRod=pSonBaseJg->handle;
						pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;

						pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=0;
						pRealLs->correct_pos();
						pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pXUpFrontPlank)
										tm.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pXDownFrontPlank)
										tm.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pYUpFrontPlank)
										tm.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pYDownFrontPlank)
										tm.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}

						pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_y = ls_pos_y-pSonBaseJg->get_norm_y_wing()*tm.wing_thick;
						_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
						ls_y.ucs.origin=ls_pos_y;
						pRealLs->CopyProperty(&ls_y);
						pRealLs->cfgword=pSonBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointY;
						pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
						pRealLs->des_base_pos.offset_wing=1;;
						pRealLs->des_base_pos.direction=0;
						pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;

						pRealLs->des_work_norm.norm_style=1;
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=1;
						pRealLs->correct_pos();
						pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pYUpFrontPlank)
										tm.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pYDownFrontPlank)
										tm.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pXUpFrontPlank)
										tm.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pXDownFrontPlank)
										tm.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
					}
				}
			}
			else if(pGroupJg->group_style==1)	//T型组合角钢
			{
#ifdef AFX_TARG_ENU_ENGLISH
				throw "System does not support the form of hoof plate temporarily!";
#else 
				throw "系统暂时不支持这种形式的底牛蹄板!";
#endif
			}
			else	//十字交叉型牛蹄板基准材
			{
				CLDSLineAngle *pJgArr[4];
				pJgArr[0]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
				pJgArr[1]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
				pJgArr[2]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
				pJgArr[3]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
				for(int ii=0;ii<3;ii+=2)
				{
					CLDSLineAngle *pSonBaseJg=pJgArr[ii];
					JGZJ jgzj_x,jgzj_y;
					f3dPoint wing_vec_x=pSonBaseJg->GetWingVecX();
					f3dPoint wing_vec_y=pSonBaseJg->GetWingVecY();
					if(!getjgzj(jgzj_x,tm.wing_wide))
					{
						char sError[200];
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Couldn't find the width %d of angle's leg from angle stadia library!",tm.wing_wide);
#else 
						_snprintf(sError,199,"找不到肢宽%d的角钢准距数据!",tm.wing_wide);
#endif
						throw sError;
					}
					jgzj_y=jgzj_x;
					if(pSonBaseJg->m_bEnableTeG)
					{
						jgzj_x = pSonBaseJg->xWingXZhunJu;
						jgzj_y = pSonBaseJg->xWingYZhunJu;
					}
					CLDSBolt ls_x(console.GetActiveModel()),ls_y(console.GetActiveModel());
					datumPointX.des_para.RODEND.offset_wing=0;
					datumPointY.des_para.RODEND.offset_wing=1;
					datumPointX.datum_pos_style					=datumPointY.datum_pos_style=1;
					datumPointX.des_para.RODEND.hRod			=datumPointY.des_para.RODEND.hRod=pSonBaseJg->handle;
					datumPointX.des_para.RODEND.direction		=datumPointY.des_para.RODEND.direction=0;
					datumPointX.des_para.RODEND.len_offset_dist	=datumPointY.des_para.RODEND.len_offset_dist=0;
					datumPointX.des_para.RODEND.bIncOddEffect	=datumPointY.des_para.RODEND.bIncOddEffect=TRUE;
					datumPointX.des_para.RODEND.wing_offset_dist=datumPointY.des_para.RODEND.wing_offset_dist=0;
					datumPointX.des_para.RODEND.wing_offset_style=datumPointY.des_para.RODEND.wing_offset_style=4;
					ls_x.set_d(ls_group.d);
					ls_y.set_d(ls_group.d);
					ls_x.AddL0Thick(nXueBanThick,FALSE);
					ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
					ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
					ls_y.AddL0Thick(nXueBanThick,FALSE);
					ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
					ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
					if(!ls_x.CalGuigeAuto())
					{
						char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Can't find the specification M%dX%d of the bolt!",ls_group.d,ls_x.get_L0());
#else 
						_snprintf(sError,199,"找不到M%dX%d规格的螺栓!",ls_group.d,ls_x.get_L0());
#endif
						throw sError;
					}
					ls_x.iSeg=pSonBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
					ls_y.CopyProperty(&ls_x);
					ls_x.set_norm(pSonBaseJg->get_norm_x_wing());
					ls_y.set_norm(pSonBaseJg->get_norm_y_wing());

					f3dPoint ls_pos_x,ls_pos_y;
					int offset = LsSpace.EndSpace;
					for(int j=0;j<pSonBaseJg->connectStart.wnConnBoltN;j++)
					{
						if(pSonBaseJg->connectStart.rows==1)
						{		//单排螺栓
							//ls_x.dwRayNo = 1;
							{	//单排螺栓螺栓正常排列
								if(iLsLayStyle==0)	//左高右底
								{
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
								else
								{
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
							}
							offset += LsSpace.SingleRowSpace;
						}
						else	//双排螺栓
						{
							if(iLsLayStyle==0)
							{	//靠近楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(2);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(3);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
							}
							else
							{	//背离楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(2);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->Start(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(3);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->Start(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
							}
							offset+=LsSpace.doubleRowSpace/2;
						}
						CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_x = ls_pos_x-pSonBaseJg->get_norm_x_wing()*tm.wing_thick;
						_snprintf(ls_x.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
						ls_x.ucs.origin=ls_pos_x;
						pRealLs->CopyProperty(&ls_x);
						pRealLs->cfgword=pSonBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointX;
						pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
						pRealLs->des_base_pos.offset_wing=0;
						pRealLs->des_base_pos.direction=0;
						pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;

						pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=0;
						pRealLs->correct_pos();
						pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());
						pJgArr[(ii+1)%4]->AppendStartLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pXUpFrontPlank)
										tm.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pXDownFrontPlank)
										tm.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pYUpFrontPlank)
										tm.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pYDownFrontPlank)
										tm.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}

						pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_y = ls_pos_y-pSonBaseJg->get_norm_y_wing()*tm.wing_thick;
						_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
						ls_y.ucs.origin=ls_pos_y;
						pRealLs->CopyProperty(&ls_y);
						pRealLs->cfgword=pSonBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointY;
						pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
						pRealLs->des_base_pos.offset_wing=1;
						pRealLs->des_base_pos.direction=0;
						pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;

						pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=1;
						pRealLs->correct_pos();
						pSonBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

						pJgArr[(ii+3)%4]->AppendStartLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pYUpFrontPlank)
										tm.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pYDownFrontPlank)
										tm.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pXUpFrontPlank)
										tm.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pXDownFrontPlank)
										tm.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
					}
				}
			}
		}
		else								//单角钢
		{	//布置始端主材上的螺栓
			JGZJ jgzj_x,jgzj_y;
			if(!getjgzj(jgzj_x,tm.wing_wide))
			{
				char sError[200];
#ifdef AFX_TARG_ENU_ENGLISH
				_snprintf(sError,199,"Couldn't find the width %d of angle's leg from angle stadia library!",tm.wing_wide);
#else 
				_snprintf(sError,199,"找不到肢宽%d的角钢准距数据!",tm.wing_wide);
#endif
				throw sError;
			}
			jgzj_y=jgzj_x;
			if(pBaseJg->m_bEnableTeG)
			{
				jgzj_x = pBaseJg->xWingXZhunJu;
				jgzj_y = pBaseJg->xWingYZhunJu;
			}
			CLDSBolt ls_x(console.GetActiveModel()),ls_y(console.GetActiveModel());
			datumPointX.des_para.RODEND.offset_wing=0;
			datumPointY.des_para.RODEND.offset_wing=1;
			datumPointX.datum_pos_style					=datumPointY.datum_pos_style=1;
			datumPointX.des_para.RODEND.hRod			=datumPointY.des_para.RODEND.hRod=pBaseJg->handle;
			datumPointX.des_para.RODEND.direction		=datumPointY.des_para.RODEND.direction=0;
			datumPointX.des_para.RODEND.len_offset_dist	=datumPointY.des_para.RODEND.len_offset_dist=0;
			datumPointX.des_para.RODEND.bIncOddEffect	=datumPointY.des_para.RODEND.bIncOddEffect=TRUE;
			datumPointX.des_para.RODEND.wing_offset_dist=datumPointY.des_para.RODEND.wing_offset_dist=0;
			datumPointX.des_para.RODEND.wing_offset_style=datumPointY.des_para.RODEND.wing_offset_style=4;
			ls_x.set_d(ls_group.d);
			ls_x.AddL0Thick(nXueBanThick,FALSE);
			ls_x.AddL0Thick(pBaseJg->handle,TRUE);
			ls_y.AddL0Thick(nXueBanThick,FALSE);
			ls_y.AddL0Thick(pBaseJg->handle,TRUE);
			if(!ls_x.CalGuigeAuto())
			{
				char sError[200];
#ifdef AFX_TARG_ENU_ENGLISH
				_snprintf(sError,199,"Can't find the specification M%dX%d of the bolt!",ls_x.get_d(),ls_x.get_L0());
#else 
				_snprintf(sError,199,"找不到M%dX%d规格的螺栓!",ls_x.get_d(),ls_x.get_L0());
#endif
				throw sError;
			}
			ls_x.iSeg=pBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
			ls_y.CopyProperty(&ls_x);
			ls_x.set_norm(pBaseJg->get_norm_x_wing());
			ls_y.set_norm(pBaseJg->get_norm_y_wing());

			f3dPoint ls_pos_x,ls_pos_y;
			int offset = LsSpace.EndSpace;
			for(int j=0;j<pBaseJg->connectStart.wnConnBoltN;j++)
			{
				if(pBaseJg->connectStart.rows==1)
				{		//单排螺栓
					{	//单排螺栓螺栓正常排列
						if(tm.m_iUpLsLayStyle==0)	//左高右底
						{
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
							ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->Start(),offset,jgzj_y.g);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
						}
						else
						{
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->Start(),offset,jgzj_x.g);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->Start(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
							ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
						}
					}
					offset += LsSpace.SingleRowSpace;
				}
				else	//双排螺栓
				{
					if(tm.m_iUpLsLayStyle==0)
					{	//靠近楞线一侧螺栓优先排列
						if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
						{
							ls_x.dwRayNo = GetSingleWord(2);
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->Start(),offset,jgzj_x.g1);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
							ls_y.dwRayNo = GetSingleWord(3);
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->Start(),offset,jgzj_y.g2);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
						}
						else		//奇数号螺栓位于背离楞线一侧	
						{
							ls_x.dwRayNo = GetSingleWord(3);
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->Start(),offset,jgzj_x.g2);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
							ls_y.dwRayNo = GetSingleWord(2);
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->Start(),offset,jgzj_y.g1);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
						}
					}
					else
					{	//背离楞线一侧螺栓优先排列
						if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
						{
							ls_x.dwRayNo = GetSingleWord(3);
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->Start(),offset,jgzj_x.g2);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
							ls_y.dwRayNo = GetSingleWord(2);
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->Start(),offset,jgzj_y.g1);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
						}
						else		//奇数号螺栓位于背离楞线一侧	
						{
							ls_x.dwRayNo = GetSingleWord(2);
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->Start(),offset,jgzj_x.g1);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
							ls_y.dwRayNo = GetSingleWord(3);
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->Start(),offset,jgzj_y.g2);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
						}
					}
					offset+=LsSpace.doubleRowSpace/2;
				}
				CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				ls_pos_x = ls_pos_x-pBaseJg->get_norm_x_wing()*tm.wing_thick;
				_snprintf(ls_x.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
				ls_x.ucs.origin=ls_pos_x;
				pRealLs->CopyProperty(&ls_x);
				pRealLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
				pRealLs->des_base_pos.datumPoint=datumPointX;
				pRealLs->des_base_pos.hPart=pBaseJg->handle;
				pRealLs->des_base_pos.offset_wing=0;
				pRealLs->des_base_pos.direction=0;
				pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
				pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;

				pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
				pRealLs->des_work_norm.direction=0;
				pRealLs->des_work_norm.hVicePart = pBaseJg->handle;
				pRealLs->des_work_norm.norm_wing=0;
				pRealLs->correct_pos();
				pBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

				if(fabs(pBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
					fabs(pBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
				{
					if(tm.bUpJg)
						tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
					else
						tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
				}
				else
				{
					if(tm.bUpJg)
						tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
					else
						tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
				}


				pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				ls_pos_y = ls_pos_y-pBaseJg->get_norm_y_wing()*tm.wing_thick;
				_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
				ls_y.ucs.origin=ls_pos_y;
				pRealLs->CopyProperty(&ls_y);
				pRealLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
				pRealLs->des_base_pos.datumPoint=datumPointY;
				pRealLs->des_base_pos.hPart=pBaseJg->handle;
				pRealLs->des_base_pos.offset_wing=1;
				pRealLs->des_base_pos.direction=0;
				pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
				pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;

				pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
				pRealLs->des_work_norm.direction=0;
				pRealLs->des_work_norm.hVicePart = pBaseJg->handle;
				pRealLs->des_work_norm.norm_wing=1;
				pRealLs->correct_pos();
				pBaseJg->AppendStartLsRef(pRealLs->GetLsRef());

				if(fabs(pBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
					fabs(pBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
				{
					if(tm.bUpJg)
						tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
					else
						tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
				}
				else
				{
					if(tm.bUpJg)
						tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
					else
						tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
				}
			}
		}
		pBaseJg->SetModified();
	}
	else
	{
		//获得基准材螺栓间距信息
		ls_group=pBaseJg->connectEnd;
		//设置连接信息及螺栓端距,间距 wht 11-04-26
		GetLsSpace(LsSpace,ls_group.d);
		if(ls_group.LsSpace.DoubleRowSpace>0&&ls_group.LsSpace.EndSpace>0
			&&ls_group.LsSpace.SingleRowSpace>0)
		{
			LsSpace.EndSpace=ls_group.LsSpace.EndSpace;
			LsSpace.SingleRowSpace=ls_group.LsSpace.SingleRowSpace;
			LsSpace.doubleRowSpace=ls_group.LsSpace.DoubleRowSpace;
		}
		//布置主材螺栓
		if(pBaseJg->GetClassTypeId()==CLS_GROUPLINEANGLE)	//组合角钢
		{	//布置终端主材上的螺栓
			CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pBaseJg;
			if(pGroupJg->group_style==0)	//对角组合型牛蹄板基准材
			{
				CLDSLineAngle *pJgArr[2];
				pJgArr[0]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
				pJgArr[1]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
				for(int ii=0;ii<2;ii++)
				{
					CLDSLineAngle *pSonBaseJg=pJgArr[ii];
					f3dPoint wing_vec_x=pSonBaseJg->GetWingVecX();
					f3dPoint wing_vec_y=pSonBaseJg->GetWingVecY();
					JGZJ jgzj_x,jgzj_y;
					if(!getjgzj(jgzj_x,tm.wing_wide))
					{
						char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Couldn't find the width %d of angle's leg from angle stadia library!",tm.wing_wide);
#else 
						_snprintf(sError,199,"找不到肢宽%d的角钢准距数据!",tm.wing_wide);
#endif
						throw sError;
					}
					jgzj_y=jgzj_x;
					if(pSonBaseJg->m_bEnableTeG)
					{
						jgzj_x = pSonBaseJg->xWingXZhunJu;
						jgzj_y = pSonBaseJg->xWingYZhunJu;
					}
					CLDSBolt ls_x(console.GetActiveModel()),ls_y(console.GetActiveModel());
					datumPointX.des_para.RODEND.offset_wing=0;
					datumPointY.des_para.RODEND.offset_wing=1;
					datumPointX.datum_pos_style					=datumPointY.datum_pos_style=1;
					datumPointX.des_para.RODEND.hRod			=datumPointY.des_para.RODEND.hRod=pSonBaseJg->handle;
					datumPointX.des_para.RODEND.direction		=datumPointY.des_para.RODEND.direction=1;
					datumPointX.des_para.RODEND.len_offset_dist	=datumPointY.des_para.RODEND.len_offset_dist=0;
					datumPointX.des_para.RODEND.bIncOddEffect	=datumPointY.des_para.RODEND.bIncOddEffect=TRUE;
					datumPointX.des_para.RODEND.wing_offset_dist=datumPointY.des_para.RODEND.wing_offset_dist=0;
					datumPointX.des_para.RODEND.wing_offset_style=datumPointY.des_para.RODEND.wing_offset_style=4;
					ls_x.set_d(ls_group.d);
					ls_y.set_d(ls_group.d);
					ls_x.AddL0Thick(nXueBanThick,FALSE);
					ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
					ls_y.AddL0Thick(nXueBanThick,FALSE);
					ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
					if(!ls_x.CalGuigeAuto())
					{
						char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Can't find the specification M%dX%d of the bolt!",ls_group.d,ls_x.get_L0());
#else 
						_snprintf(sError,199,"找不到M%dX%d规格的螺栓!",ls_group.d,ls_x.get_L0());
#endif
						throw sError;
					}
					ls_x.iSeg=pSonBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
					ls_y.CopyProperty(&ls_x);
					ls_x.set_norm(pSonBaseJg->get_norm_x_wing());
					ls_y.set_norm(pSonBaseJg->get_norm_y_wing());

					f3dPoint ls_pos_x,ls_pos_y;
					int offset = LsSpace.EndSpace;
					for(int j=0;j<pSonBaseJg->connectEnd.wnConnBoltN;j++)
					{
						if(pSonBaseJg->connectEnd.rows==1)
						{		//单排螺栓
							{	//单排螺栓螺栓正常排列
								if(iLsLayStyle==0)	//左高右底
								{
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
								else
								{
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
							}
							offset += LsSpace.SingleRowSpace;
						}
						else	//双排螺栓
						{
							if(iLsLayStyle==0)
							{	//靠近楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(2);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(3);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
							}
							else
							{	//背离楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(2);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(3);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
							}
							offset+=LsSpace.doubleRowSpace/2;
						}
						CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_x = ls_pos_x-pSonBaseJg->get_norm_x_wing()*tm.wing_thick;
						_snprintf(ls_x.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
						ls_x.ucs.origin=ls_pos_x;
						pRealLs->CopyProperty(&ls_x);
						pRealLs->cfgword=pSonBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointX;
						pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
						pRealLs->des_base_pos.offset_wing=0;
						pRealLs->des_base_pos.direction=1;
						pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;

						pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=0;
						pRealLs->correct_pos();
						pSonBaseJg->AppendEndLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pXUpFrontPlank)
										tm.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pXDownFrontPlank)
										tm.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pYUpFrontPlank)
										tm.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pYDownFrontPlank)
										tm.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}

						pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_y = ls_pos_y-pSonBaseJg->get_norm_y_wing()*tm.wing_thick;
						_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
						ls_y.ucs.origin=ls_pos_y;
						pRealLs->CopyProperty(&ls_y);
						pRealLs->cfgword=pSonBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointY;
						pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
						pRealLs->des_base_pos.offset_wing=1;
						pRealLs->des_base_pos.direction=1;
						pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;

						pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=1;
						pRealLs->correct_pos();
						pSonBaseJg->AppendEndLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pYUpFrontPlank)
										tm.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pYDownFrontPlank)
										tm.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pXUpFrontPlank)
										tm.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pXDownFrontPlank)
										tm.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
					}
				}
			}
			else if(pGroupJg->group_style==1)	//T型组合角钢
			{
#ifdef AFX_TARG_ENU_ENGLISH
				throw "System does not support the form of hoof plate temporarily!";
#else 
				throw "系统暂时不支持这种形式的底牛蹄板!";
#endif
			}
			else	//十字交叉型牛蹄板基准材
			{
				CLDSLineAngle *pJgArr[4];
				pJgArr[0]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
				pJgArr[1]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
				pJgArr[2]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
				pJgArr[3]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
				for(int ii=0;ii<3;ii+=2)
				{
					CLDSLineAngle *pSonBaseJg=pJgArr[ii];
					JGZJ jgzj_x,jgzj_y;
					f3dPoint wing_vec_x=pSonBaseJg->GetWingVecX();
					f3dPoint wing_vec_y=pSonBaseJg->GetWingVecY();
					if(!getjgzj(jgzj_x,tm.wing_wide))
					{
						char sError[200];
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Couldn't find the width %d of angle's leg from angle stadia library!",tm.wing_wide);
#else 
						_snprintf(sError,199,"找不到肢宽%d的角钢准距数据!",tm.wing_wide);
#endif
						throw sError;
					}
					jgzj_y=jgzj_x;
					if(pSonBaseJg->m_bEnableTeG)
					{
						jgzj_x = pSonBaseJg->xWingXZhunJu;
						jgzj_y = pSonBaseJg->xWingYZhunJu;
					}
					CLDSBolt ls_x(console.GetActiveModel()),ls_y(console.GetActiveModel());
					datumPointX.des_para.RODEND.offset_wing=0;
					datumPointY.des_para.RODEND.offset_wing=1;
					datumPointX.datum_pos_style					=datumPointY.datum_pos_style=1;
					datumPointX.des_para.RODEND.hRod			=datumPointY.des_para.RODEND.hRod=pSonBaseJg->handle;
					datumPointX.des_para.RODEND.direction		=datumPointY.des_para.RODEND.direction=1;
					datumPointX.des_para.RODEND.len_offset_dist	=datumPointY.des_para.RODEND.len_offset_dist=0;
					datumPointX.des_para.RODEND.bIncOddEffect	=datumPointY.des_para.RODEND.bIncOddEffect=TRUE;
					datumPointX.des_para.RODEND.wing_offset_dist=datumPointY.des_para.RODEND.wing_offset_dist=0;
					datumPointX.des_para.RODEND.wing_offset_style=datumPointY.des_para.RODEND.wing_offset_style=4;
					ls_x.set_d(ls_group.d);
					ls_y.set_d(ls_group.d);
					ls_x.AddL0Thick(nXueBanThick,FALSE);
					ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
					ls_x.AddL0Thick(pSonBaseJg->handle,TRUE);
					ls_y.AddL0Thick(nXueBanThick,FALSE);
					ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
					ls_y.AddL0Thick(pSonBaseJg->handle,TRUE);
					if(!ls_x.CalGuigeAuto())
					{
						char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
						_snprintf(sError,199,"Can't find the specification M%dX%d of the bolt!",ls_group.d,ls_x.get_L0());
#else 
						_snprintf(sError,199,"找不到M%dX%d规格的螺栓!",ls_group.d,ls_x.get_L0());
#endif
						throw sError;
					}
					ls_x.iSeg=pSonBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
					ls_y.CopyProperty(&ls_x);
					ls_x.set_norm(pSonBaseJg->get_norm_x_wing());
					ls_y.set_norm(pSonBaseJg->get_norm_y_wing());

					f3dPoint ls_pos_x,ls_pos_y;
					int offset = LsSpace.EndSpace;
					for(int j=0;j<pSonBaseJg->connectEnd.wnConnBoltN;j++)
					{
						if(pSonBaseJg->connectEnd.rows==1)
						{		//单排螺栓
							{	//单排螺栓螺栓正常排列
								if(iLsLayStyle==0)	//左高右底
								{
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
								else
								{
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
									ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
								}
							}
							offset += LsSpace.SingleRowSpace;
						}
						else	//双排螺栓
						{
							if(iLsLayStyle==0)
							{	//靠近楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(2);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(3);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
							}
							else
							{	//背离楞线一侧螺栓优先排列
								if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(3);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g2);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
									ls_y.dwRayNo = GetSingleWord(2);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g1);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
								}
								else		//奇数号螺栓位于背离楞线一侧	
								{
									ls_x.dwRayNo = GetSingleWord(2);
									ls_pos_x = get_pt_by_vec(-tm.jg_vec,wing_vec_x,
										pSonBaseJg->End(),offset,jgzj_x.g1);
									ls_x.des_base_pos.len_offset_dist = offset;
									ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
									ls_y.dwRayNo = GetSingleWord(3);
									ls_pos_y = get_pt_by_vec(-tm.jg_vec,wing_vec_y,
										pSonBaseJg->End(),offset,jgzj_y.g2);
									ls_y.des_base_pos.len_offset_dist = offset;
									ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
								}
							}
							offset+=LsSpace.doubleRowSpace/2;
						}
						CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_x = ls_pos_x-pSonBaseJg->get_norm_x_wing()*tm.wing_thick;
						_snprintf(ls_x.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
						ls_x.ucs.origin=ls_pos_x;
						pRealLs->CopyProperty(&ls_x);
						pRealLs->cfgword=pSonBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointX;
						pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
						pRealLs->des_base_pos.offset_wing=0;
						pRealLs->des_base_pos.direction=1;
						pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;

						pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=0;
						pRealLs->correct_pos();
						pSonBaseJg->AppendEndLsRef(pRealLs->GetLsRef());
						pJgArr[(ii+1)%4]->AppendEndLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pXUpFrontPlank)
										tm.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pXDownFrontPlank)
										tm.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pYUpFrontPlank)
										tm.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pYDownFrontPlank)
										tm.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}

						pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
						ls_pos_y = ls_pos_y-pSonBaseJg->get_norm_y_wing()*tm.wing_thick;
						_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
						ls_y.ucs.origin=ls_pos_y;
						pRealLs->CopyProperty(&ls_y);
						pRealLs->cfgword=pSonBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
						pRealLs->des_base_pos.datumPoint=datumPointY;
						pRealLs->des_base_pos.hPart=pSonBaseJg->handle;
						pRealLs->des_base_pos.offset_wing=1;
						pRealLs->des_base_pos.direction=1;
						pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
						pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;

						pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
						pRealLs->des_work_norm.direction=0;
						pRealLs->des_work_norm.hVicePart = pSonBaseJg->handle;
						pRealLs->des_work_norm.norm_wing=1;
						pRealLs->correct_pos();
						pSonBaseJg->AppendEndLsRef(pRealLs->GetLsRef());
						pJgArr[(ii+3)%4]->AppendEndLsRef(pRealLs->GetLsRef());

						if(fabs(pSonBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
							fabs(pSonBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pYUpFrontPlank)
										tm.pYUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pYDownFrontPlank)
										tm.pYDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
						else
						{
							if(pSonBaseJg->handle==pGroupJg->son_jg_h[0])
							{	//当前角钢为组合角钢特征角钢
								if(tm.bUpJg)
									tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								else
									tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
							}
							else
							{	//当前角钢非组合角钢特征角钢
								if(tm.bUpJg)
								{
									if(tm.pXUpFrontPlank)
										tm.pXUpFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
								else
								{
									if(tm.pXDownFrontPlank)
										tm.pXDownFrontPlank->AppendLsRef(pRealLs->GetLsRef());
									else
										tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
								}
							}
						}
					}
				}
			}
		}
		else								//单角钢
		{	//布置终端主材上的螺栓
			JGZJ jgzj_x,jgzj_y;
			if(!getjgzj(jgzj_x,tm.wing_wide))
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				_snprintf(sError,199,"Couldn't find the width %d of angle's leg from angle stadia library!",tm.wing_wide);
#else 
				_snprintf(sError,199,"找不到肢宽%d的角钢准距数据!",tm.wing_wide);
#endif
				throw sError;
			}
			jgzj_y=jgzj_x;
			if(pBaseJg->m_bEnableTeG)
			{
				jgzj_x = pBaseJg->xWingXZhunJu;
				jgzj_y = pBaseJg->xWingYZhunJu;
			}
			CLDSBolt ls_x(console.GetActiveModel()),ls_y(console.GetActiveModel());
			datumPointX.des_para.RODEND.offset_wing=0;
			datumPointY.des_para.RODEND.offset_wing=1;
			datumPointX.datum_pos_style					=datumPointY.datum_pos_style=1;
			datumPointX.des_para.RODEND.hRod			=datumPointY.des_para.RODEND.hRod=pBaseJg->handle;
			datumPointX.des_para.RODEND.direction		=datumPointY.des_para.RODEND.direction=1;
			datumPointX.des_para.RODEND.len_offset_dist	=datumPointY.des_para.RODEND.len_offset_dist=0;
			datumPointX.des_para.RODEND.bIncOddEffect	=datumPointY.des_para.RODEND.bIncOddEffect=TRUE;
			datumPointX.des_para.RODEND.wing_offset_dist=datumPointY.des_para.RODEND.wing_offset_dist=0;
			datumPointX.des_para.RODEND.wing_offset_style=datumPointY.des_para.RODEND.wing_offset_style=4;
			ls_x.set_d(ls_group.d);
			ls_x.AddL0Thick(nXueBanThick,FALSE);
			ls_x.AddL0Thick(pBaseJg->handle,TRUE);
			ls_y.AddL0Thick(nXueBanThick,FALSE);
			ls_y.AddL0Thick(pBaseJg->handle,TRUE);
			if(!ls_x.CalGuigeAuto())
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				_snprintf(sError,199,"Can't find the specification M%dX%d of the bolt!",ls_x.get_d(),ls_x.get_L0());
#else 
				_snprintf(sError,199,"找不到M%dX%d规格的螺栓!",ls_x.get_d(),ls_x.get_L0());
#endif
				throw sError;
			}
			ls_x.iSeg=pBaseJg->iSeg;	//主材螺栓段号 wht 10-09-15
			ls_y.CopyProperty(&ls_x);
			ls_x.set_norm(pBaseJg->get_norm_x_wing());
			ls_y.set_norm(pBaseJg->get_norm_y_wing());

			f3dPoint ls_pos_x,ls_pos_y;
			int offset = LsSpace.EndSpace;
			for(int j=0;j<pBaseJg->connectEnd.wnConnBoltN;j++)
			{
				if(pBaseJg->connectEnd.rows==1)
				{		//单排螺栓
					{	//单排螺栓螺栓正常排列
						if(tm.m_iUpLsLayStyle==0)	//左高右底
						{
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->End(),offset+LsSpace.SingleRowSpace/2,jgzj_x.g);
							ls_x.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->End(),offset,jgzj_y.g);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
						}
						else
						{
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->End(),offset,jgzj_x.g);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g;
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->End(),offset+LsSpace.SingleRowSpace/2,jgzj_y.g);
							ls_y.des_base_pos.len_offset_dist = offset+LsSpace.SingleRowSpace/2;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g;
						}
					}
					offset += LsSpace.SingleRowSpace;
				}
				else	//双排螺栓
				{
					if(tm.m_iUpLsLayStyle==0)
					{	//靠近楞线一侧螺栓优先排列
						if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
						{
							ls_x.dwRayNo = GetSingleWord(2);
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->End(),offset,jgzj_x.g1);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
							ls_y.dwRayNo = GetSingleWord(3);
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->End(),offset,jgzj_y.g2);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
						}
						else		//奇数号螺栓位于背离楞线一侧	
						{
							ls_x.dwRayNo = GetSingleWord(3);
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->End(),offset,jgzj_x.g2);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
							ls_y.dwRayNo = GetSingleWord(2);
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->End(),offset,jgzj_y.g1);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
						}
					}
					else
					{	//背离楞线一侧螺栓优先排列
						if(j%2==0)	//偶数号螺栓位于靠近楞线一侧	
						{
							ls_x.dwRayNo = GetSingleWord(3);
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->End(),offset,jgzj_x.g2);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g2;
							ls_y.dwRayNo = GetSingleWord(2);
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->End(),offset,jgzj_y.g1);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g1;
						}
						else		//奇数号螺栓位于背离楞线一侧	
						{
							ls_x.dwRayNo = GetSingleWord(2);
							ls_pos_x = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_x,
								pBaseJg->End(),offset,jgzj_x.g1);
							ls_x.des_base_pos.len_offset_dist = offset;
							ls_x.des_base_pos.wing_offset_dist = jgzj_x.g1;
							ls_y.dwRayNo = GetSingleWord(3);
							ls_pos_y = get_pt_by_vec(-tm.jg_vec,tm.wing_vec_y,
								pBaseJg->End(),offset,jgzj_y.g2);
							ls_y.des_base_pos.len_offset_dist = offset;
							ls_y.des_base_pos.wing_offset_dist = jgzj_y.g2;
						}
					}
					offset+=LsSpace.doubleRowSpace/2;
				}
				CLDSBolt *pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				ls_pos_x = ls_pos_x-pBaseJg->get_norm_x_wing()*tm.wing_thick;
				_snprintf(ls_x.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
				ls_x.ucs.origin=ls_pos_x;
				pRealLs->CopyProperty(&ls_x);
				pRealLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
				pRealLs->des_base_pos.datumPoint=datumPointX;
				pRealLs->des_base_pos.hPart=pBaseJg->handle;
				pRealLs->des_base_pos.offset_wing=0;
				pRealLs->des_base_pos.direction=1;
				pRealLs->des_base_pos.len_offset_dist=ls_x.des_base_pos.len_offset_dist;
				pRealLs->des_base_pos.wing_offset_dist = ls_x.des_base_pos.wing_offset_dist;

				pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
				pRealLs->des_work_norm.direction=0;
				pRealLs->des_work_norm.hVicePart = pBaseJg->handle;
				pRealLs->des_work_norm.norm_wing=0;
				pRealLs->correct_pos();
				pBaseJg->AppendEndLsRef(pRealLs->GetLsRef());

				if(fabs(pBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
					fabs(pBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
				{
					if(tm.bUpJg)
						tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
					else
						tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
				}
				else
				{
					if(tm.bUpJg) 
						tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
					else
						tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
				}

				pRealLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				ls_pos_y = ls_pos_y-pBaseJg->get_norm_y_wing()*tm.wing_thick;
				_snprintf(ls_y.des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"%.0f",-tm.wing_thick);
				ls_y.ucs.origin=ls_pos_y;
				pRealLs->CopyProperty(&ls_y);
				pRealLs->cfgword=pBaseJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
				pRealLs->des_base_pos.datumPoint=datumPointY;
				pRealLs->des_base_pos.hPart=pBaseJg->handle;
				pRealLs->des_base_pos.offset_wing=1;
				pRealLs->des_base_pos.direction=1;
				pRealLs->des_base_pos.len_offset_dist=ls_y.des_base_pos.len_offset_dist;
				pRealLs->des_base_pos.wing_offset_dist = ls_y.des_base_pos.wing_offset_dist;

				pRealLs->des_work_norm.norm_style=1;	//指定角钢肢法线
				pRealLs->des_work_norm.direction=0;
				pRealLs->des_work_norm.hVicePart = pBaseJg->handle;
				pRealLs->des_work_norm.norm_wing=1;
				pRealLs->correct_pos();
				pBaseJg->AppendEndLsRef(pRealLs->GetLsRef());

				if(fabs(pBaseJg->get_norm_x_wing()*m_pFoot->ucs.axis_y)>
					fabs(pBaseJg->get_norm_y_wing()*m_pFoot->ucs.axis_y))
				{
					if(tm.bUpJg)
						tm.pYUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
					else
						tm.pYDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
				}
				else
				{
					if(tm.bUpJg)
						tm.pXUpBackPlank->AppendLsRef(pRealLs->GetLsRef());
					else
						tm.pXDownBackPlank->AppendLsRef(pRealLs->GetLsRef());
				}
			}
		}
		pBaseJg->SetModified();
	}
}

bool CFootDesignDlg::LayRayAngleBolt(ATOM_LIST<CDesignLsPara> *pLsList,CLDSLineAngle *pRayAngle,
									 CLDSLineAngle *pBaseAngle,CLDSPlate *pBasePlate,double *ber_space,double *wing_space)
{
	f3dPoint work_norm;
	BOOL bInsideXieJg=FALSE;
	int xie_x_wing0_y_wing1=0;
	//获取连接面法线
	GetWorkNorm(pBaseAngle,pRayAngle,&work_norm);
	//根据连接面法线计算基准角钢及斜材角钢的当前连接肢及里外铁
	bInsideXieJg = IsInsideJg(pRayAngle,work_norm,&xie_x_wing0_y_wing1);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pRayAngle->handle);
	JGZJ jgzj;
	CLayAngleBoltDlg dlg;
	dlg.m_bIncPlateProfilePara = TRUE;
	getjgzj(jgzj,pRayAngle->GetWidth());
	if(pRayAngle->m_bEnableTeG)
	{
		if(xie_x_wing0_y_wing1==0)
			jgzj = pRayAngle->xWingXZhunJu;
		else
			jgzj = pRayAngle->xWingYZhunJu;
	}
	dlg.m_iOffsetWing=xie_x_wing0_y_wing1;
	dlg.m_nG=jgzj.g;
	dlg.m_nG1=jgzj.g1;
	dlg.m_nG2=jgzj.g2;
	CConnectInfo ls_group;
	if(pRayAngle->pStart->handle==m_pFoot->base_node_handle)	//始端连接
	{
		dlg.m_pNode=pRayAngle->pStart;
		ls_group=pRayAngle->connectStart;
		dlg.m_iRayDirection=0;
		dlg.m_iOddCalStyle=pRayAngle->desStartOdd.m_iOddCalStyle;
		if(dlg.m_iOddCalStyle!=1)
		{
			long hBaseAngle=pBaseAngle->handle;
			if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
				hBaseAngle=pBaseAngle->son_jg_h[0];
			if(pRayAngle->desStartOdd.m_hRefPart1<0x20)
				pRayAngle->desStartOdd.m_hRefPart1=hBaseAngle;
			else if(pRayAngle->desStartOdd.m_hRefPart1!=hBaseAngle&&pRayAngle->desStartOdd.m_hRefPart2<0x20)
				pRayAngle->desStartOdd.m_hRefPart2=hBaseAngle;
			pRayAngle->CalStartOddment();
		}
		dlg.m_fOddment=pRayAngle->startOdd();
	}
	else
	{
		dlg.m_pNode=pRayAngle->pEnd;
		ls_group=pRayAngle->connectEnd;
		dlg.m_iRayDirection=1;
		dlg.m_iOddCalStyle=pRayAngle->desEndOdd.m_iOddCalStyle;
		if(dlg.m_iOddCalStyle!=1)
		{
			long hBaseAngle=pBaseAngle->handle;
			if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
				hBaseAngle=pBaseAngle->son_jg_h[0];
			if(pRayAngle->desEndOdd.m_hRefPart1<0x20)
				pRayAngle->desEndOdd.m_hRefPart1=hBaseAngle;
			else if(pRayAngle->desEndOdd.m_hRefPart1!=hBaseAngle&&pRayAngle->desEndOdd.m_hRefPart2<0x20)
				pRayAngle->desEndOdd.m_hRefPart2=hBaseAngle;
			pRayAngle->CalEndOddment();
		}
		dlg.m_fOddment=pRayAngle->endOdd();
	}
	dlg.m_pLinePart=pRayAngle;
	dlg.m_bMirLayLs=FALSE;
	dlg.m_sBoltSize.Format("%d",ls_group.d);
	dlg.m_iBoltRows = ls_group.rows-1;
	dlg.m_pLsList=pLsList;
	dlg.m_sRayAngleSize.Format("L%.0fX%.0f",pRayAngle->GetWidth(),pRayAngle->GetThick());
	if(bInsideXieJg)
	{	//里铁
		dlg.m_iBoltNorm=0;	//螺栓朝外
		_snprintf(dlg.m_tBoltNormOffset.key_str,MAX_TEMP_CHAR_50,"-0X%X",pRayAngle->handle);
	}
	else
	{	//外铁
		dlg.m_iBoltNorm=1;	//螺栓朝内
		_snprintf(dlg.m_tBoltNormOffset.key_str,MAX_TEMP_CHAR_50,"-0X%X",pBasePlate->handle);
	}
	//螺栓间隙
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,ls_group.d);
	dlg.m_nLsEndSpace = LsSpace.EndSpace;
	dlg.m_nLsNum = ls_group.wnConnBoltN;
	if(ls_group.rows==1)
		dlg.m_nLsSpace	= LsSpace.SingleRowSpace;
	else
		dlg.m_nLsSpace	= LsSpace.doubleRowSpace;
	dlg.viewNorm = work_norm;
	if(dlg.DoModal()!=IDOK)
		return false;
	*ber_space=dlg.m_fBerSpace;
	*wing_space=dlg.m_fWingSpace;
	if(dlg.m_pNode==pRayAngle->pEnd)
	{
		pRayAngle->desEndOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
		pRayAngle->SetEndOdd(dlg.m_fOddment);
		if(dlg.m_iOffsetWing==0)	
			pRayAngle->desEndPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
		else
			pRayAngle->desEndPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
	}
	else if(dlg.m_pNode==pRayAngle->pStart)
	{
		pRayAngle->desStartOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
		pRayAngle->SetStartOdd(dlg.m_fOddment);
		if(dlg.m_iOffsetWing==0)
			pRayAngle->desStartPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
		else
			pRayAngle->desStartPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
	}
	pRayAngle->SetModified();
	pRayAngle->CalPosition();
	pRayAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pRayAngle->GetSolidPartObject());
	return true;
}

// 预设射线角钢摆放位置
double CFootDesignDlg::PresetRayJgEndPos(CLDSLineAngle *pAngle,CLDSLineAngle *pBaseAngle)
{
	double fNormOffset=0;
	f3dPoint work_norm;
	BOOL bInsideXieJg=FALSE;
	int xie_x_wing0_y_wing1=0,base_x_wing0_y_wing1=0;
	//获取连接面法线
	GetWorkNorm(pBaseAngle,pAngle,&work_norm);
	//根据连接面法线计算基准角钢及斜材角钢的当前连接肢及里外铁
	bInsideXieJg = IsInsideJg(pAngle,work_norm,&xie_x_wing0_y_wing1);
	IsInsideJg(pBaseAngle,work_norm,&base_x_wing0_y_wing1);
	if(pAngle->pStart->handle==m_pFoot->base_node_handle)
	{
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&pBaseAngle->GetClassTypeId()==CLS_LINEANGLE)
		{	//射线角钢为组合角钢基准角钢为普通角钢
			if(pAngle->desStartPos.jgber_cal_style!=1&&pAngle->desStartPos.origin_pos_style==0&&pAngle->desStartPos.spatialOperationStyle==0)
			{
				//判断基准角钢是否为平推
				int offset_style=0;
				/*if(pBaseAngle->desEndPos.jgber_cal_style==2
					||(pBaseAngle->desEndPos.jgber_cal_style==0
					&&((pBaseAngle->desEndPos.datum_pos_style>3&&pBaseAngle->desEndPos.datum_pos_style<7)
					||(pBaseAngle->desEndPos.datum_pos_style>9&&pBaseAngle->desEndPos.datum_pos_style<13)
					||pBaseAngle->desEndPos.datum_to_ber_style==1)))*/
				if(pBaseAngle->desEndPos.IsFaceOffset()&&pBaseAngle->desEndPos.spatialOperationStyle<15)
					offset_style=3;	//搭接平推
				if(base_x_wing0_y_wing1==0)
					pAngle->desStartPos.spatialOperationStyle=2+offset_style;
				else if(base_x_wing0_y_wing1==1)
					pAngle->desStartPos.spatialOperationStyle=3+offset_style;
			}
			if(xie_x_wing0_y_wing1==0)
			{
				pAngle->desStartPos.wing_x_offset.gStyle = 4;
				if(m_pFoot->lj_by_weld==1)	//对焊
					pAngle->desStartPos.wing_x_offset.offsetDist=-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				else
					pAngle->desStartPos.wing_x_offset.offsetDist=foot_para.m_iUpXueBanThick-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				fNormOffset=pAngle->desStartPos.wing_x_offset.offsetDist;
			}
			else if(xie_x_wing0_y_wing1==1)
			{
				pAngle->desStartPos.wing_y_offset.gStyle = 4;
				if(m_pFoot->lj_by_weld==1)	//对焊
					pAngle->desStartPos.wing_y_offset.offsetDist=-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				else
					pAngle->desStartPos.wing_y_offset.offsetDist=foot_para.m_iUpXueBanThick-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				fNormOffset=pAngle->desStartPos.wing_y_offset.offsetDist;
			}
		}
		else 
		{
			if((m_pFoot->lj_by_weld==1&&bInsideXieJg)	//对焊
				||(m_pFoot->lj_by_weld!=1&&!bInsideXieJg))
			{
				if(xie_x_wing0_y_wing1==0)
				{
					pAngle->desStartPos.wing_x_offset.gStyle = 4;
					pAngle->desStartPos.wing_x_offset.offsetDist=-foot_para.m_iUpXueBanThick;
					fNormOffset=pAngle->desStartPos.wing_x_offset.offsetDist;
				}
				else
				{
					pAngle->desStartPos.wing_y_offset.gStyle = 4;
					pAngle->desStartPos.wing_y_offset.offsetDist=-foot_para.m_iUpXueBanThick;
					fNormOffset=pAngle->desStartPos.wing_y_offset.offsetDist;
				}
			}
			else
			{
				if(bInsideXieJg)
				{	//里铁
					if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
						fNormOffset=-foot_para.m_iUpXueBanThick*0.5;
					else
						fNormOffset=0;
				}
				else
				{	//外铁
					if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
						fNormOffset=-foot_para.m_iUpXueBanThick*0.5;
					else
						fNormOffset=-foot_para.m_iUpXueBanThick;
				}
				if(xie_x_wing0_y_wing1==0)
				{
					pAngle->desStartPos.wing_x_offset.gStyle = 4;
					pAngle->desStartPos.wing_x_offset.offsetDist=fNormOffset;
				}
				else
				{
					pAngle->desStartPos.wing_y_offset.gStyle = 4;
					pAngle->desStartPos.wing_y_offset.offsetDist=fNormOffset;
				}
			}
		}
	}
	else
	{
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&pBaseAngle->GetClassTypeId()==CLS_LINEANGLE)
		{	//射线角钢为组合角钢基准角钢为普通角钢
			if(pAngle->desEndPos.jgber_cal_style!=1&&pAngle->desEndPos.origin_pos_style==0&&pAngle->desEndPos.spatialOperationStyle==0)
			{
				//判断基准角钢是否为平推
				int offset_style=0;
				/*if(pBaseAngle->desEndPos.jgber_cal_style==2
					||(pBaseAngle->desEndPos.jgber_cal_style==0
					&&((pBaseAngle->desEndPos.datum_pos_style>3&&pBaseAngle->desEndPos.datum_pos_style<7)
					||(pBaseAngle->desEndPos.datum_pos_style>9&&pBaseAngle->desEndPos.datum_pos_style<13)
					||pBaseAngle->desEndPos.datum_to_ber_style==1)))*/
				if(pBaseAngle->desEndPos.IsFaceOffset()&&pBaseAngle->desEndPos.spatialOperationStyle<15)
					offset_style=3;	//搭接平推
				if(base_x_wing0_y_wing1==0)
					pAngle->desEndPos.spatialOperationStyle=2+offset_style;
				else if(base_x_wing0_y_wing1==1)
					pAngle->desEndPos.spatialOperationStyle=3+offset_style;
			}
			if(xie_x_wing0_y_wing1==0)
			{
				pAngle->desEndPos.wing_x_offset.gStyle = 4;
				if(m_pFoot->lj_by_weld==1)	//对焊
					pAngle->desEndPos.wing_x_offset.offsetDist=-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				else
					pAngle->desEndPos.wing_x_offset.offsetDist=foot_para.m_iUpXueBanThick-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				fNormOffset=pAngle->desEndPos.wing_x_offset.offsetDist;
			}
			else if(xie_x_wing0_y_wing1==1)
			{
				pAngle->desEndPos.wing_y_offset.gStyle = 4;
				if(m_pFoot->lj_by_weld==1)	//对焊
					pAngle->desEndPos.wing_y_offset.offsetDist=-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				else
					pAngle->desEndPos.wing_y_offset.offsetDist=foot_para.m_iUpXueBanThick-((CLDSGroupLineAngle*)pAngle)->jg_space*0.5;
				fNormOffset=pAngle->desEndPos.wing_y_offset.offsetDist;
			}
		}
		else 
		{
			if((m_pFoot->lj_by_weld==1&&bInsideXieJg)	//对焊
				||(m_pFoot->lj_by_weld!=1&&!bInsideXieJg))
			{
				if(xie_x_wing0_y_wing1==0)
				{
					pAngle->desEndPos.wing_x_offset.gStyle = 4;
					pAngle->desEndPos.wing_x_offset.offsetDist=-foot_para.m_iUpXueBanThick;
					fNormOffset=pAngle->desEndPos.wing_x_offset.offsetDist;
				}
				else
				{
					pAngle->desEndPos.wing_y_offset.gStyle = 4;
					pAngle->desEndPos.wing_y_offset.offsetDist=-foot_para.m_iUpXueBanThick;
					fNormOffset=pAngle->desEndPos.wing_y_offset.offsetDist;
				}
			}
			else
			{
				if(bInsideXieJg)
				{	//里铁
					if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
						fNormOffset=-foot_para.m_iUpXueBanThick*0.5;
					else
						fNormOffset=0;
				}
				else
				{	//外铁
					if(pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
						fNormOffset=-foot_para.m_iUpXueBanThick*0.5;
					else
						fNormOffset=-foot_para.m_iUpXueBanThick;
				}
				if(xie_x_wing0_y_wing1==0)
				{
					pAngle->desEndPos.wing_x_offset.gStyle = 4;
					pAngle->desEndPos.wing_x_offset.offsetDist=fNormOffset;
				}
				else
				{
					pAngle->desEndPos.wing_y_offset.gStyle = 4;
					pAngle->desEndPos.wing_y_offset.offsetDist=fNormOffset;
				}
			}
		}
	}
	pAngle->ClearFlag();
	pAngle->CalPosition();
	if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		((CLDSGroupLineAngle*)pAngle)->UpdateSonJgPos();
	return fNormOffset;
}

// 设计底脚板(牛蹄板)处的射线材角钢
//the following function is from DesignJdb.cpp
int GetDeita23ByLsDiameter(int diameter);
bool CFootDesignDlg::DesRayJg(JGSET &jgset,CLDSLineAngle *pUpBaseJg,CLDSLineAngle *pDownBaseJg)
{
	//设计底脚板(牛蹄板)处的射线材角钢
	CLDSPlate *pFrontPlank=NULL,*pBackPlank=NULL;
	CDesignLjPartPara *pLjPartPara=NULL;
	CLDSLineAngle *pBaseJg=NULL,*pAngle=NULL;
	f3dPoint vec;
	for(pAngle=jgset.GetFirst();pAngle;pAngle=jgset.GetNext())
	{
		if(pAngle->handle==m_pFoot->attach_jg_handle[0])
			continue;
		if(pAngle->handle==m_pFoot->attach_jg_handle[1])
			continue;
		if(pAngle->pStart->handle==m_pFoot->base_node_handle)
			vec=pAngle->End()-pAngle->Start();
		else
			vec=pAngle->Start()-pAngle->End();
		normalize(vec);
		if(pAngle->layer(1)!='H'&&vec*m_pFoot->ucs.axis_z>eps2)	//上侧斜材
			pBaseJg=pUpBaseJg;
		else if(pAngle->layer(1)!='H'&&vec*m_pFoot->ucs.axis_z<-eps2)	//下侧斜材
			pBaseJg=pDownBaseJg;
		else							//横材不在此处理(单独处理)
			continue;
		if(pBaseJg==NULL)	//无下侧角钢易死机 WJH-2003.11.30
			continue;
		//切换到实体显示模式
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		g_pSolidDraw->Draw();
		g_pSolidDraw->ReleaseSnapStatus();
		typedef	CLDSLineAngle*	LINEANGLEPTR;
		LINEANGLEPTR angleArr[4]={NULL};
		if(pAngle->GetClassTypeId()==CLS_LINEANGLE)
			angleArr[0]=pAngle;
		else if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{	//组合角钢
			CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pAngle;
			for(int i=0;i<4;i++)
			{
				if(pGroupAngle->group_style<2&&i>=2)
					continue;	//个别四角钢转换为双拼角钢时，会把第三、四根子角钢转换为虚角钢存在
				CLDSLineAngle *pCommBaseJg=NULL;
				if(pGroupAngle->son_jg_h[i]>=0x20)
					pCommBaseJg=(CLDSLineAngle*)console.FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
				if(pCommBaseJg)
					angleArr[i]=pCommBaseJg;
			}
		}
		//设置斜材角钢位置设计参数
		PresetRayJgEndPos(pAngle,pBaseJg);

		int i=0;
		int base_x_wing0_y_wing1,xie_x_wing0_y_wing1;
		for(i=0;i<4;i++)
		{
			if(angleArr[i]==NULL)
				continue;
			pAngle=angleArr[i];
			BOOL bInFootX;
			f3dPoint work_norm;
			//获取连接面法线
			GetWorkNorm(pBaseJg,pAngle,&work_norm);
			double xie_jg_wing_wide,xie_jg_wing_thick;
			//根据连接面法线计算基准角钢及斜材角钢的当前连接肢及里外铁
			IsInsideJg(pAngle,work_norm,&xie_x_wing0_y_wing1);
			IsInsideJg(pBaseJg,work_norm,&base_x_wing0_y_wing1);
			g_pSolidDraw->SetEntSnapStatus(pAngle->handle);
			//判断当前肢
			if(fabs(dot_prod(work_norm,m_pFoot->ucs.axis_y))>
				fabs(dot_prod(work_norm,m_pFoot->ucs.axis_x)))
				bInFootX = TRUE;	//底脚板X轴向的肢
			else
				bInFootX = FALSE;	//底脚板Y轴向的肢
			
			f3dPoint xie_wing_vec;
			if(xie_x_wing0_y_wing1==0)
				xie_wing_vec = pAngle->GetWingVecX();
			else
				xie_wing_vec = pAngle->GetWingVecY();
			xie_jg_wing_wide = pAngle->GetWidth();
			xie_jg_wing_thick= pAngle->GetThick();
			//选择当前设计靴板
			if(pBaseJg==pUpBaseJg)	//上侧靴板
			{
				if(bInFootX)
				{
					pFrontPlank=tm.pXUpFrontPlank;
					pBackPlank=tm.pXUpBackPlank;
				}
				else
				{
					pFrontPlank=tm.pYUpFrontPlank;
					pBackPlank=tm.pYUpBackPlank;
				}
			}
			else	//下侧靴板
			{
				if(bInFootX)
				{
					pFrontPlank=tm.pXDownFrontPlank;
					pBackPlank=tm.pXDownBackPlank;
				}
				else
				{
					pFrontPlank=tm.pYDownFrontPlank;
					pBackPlank=tm.pYDownBackPlank;
				}
			}
			/*if(pFrontPlank)	//不知道为什么会留下这四行程序，这四行将导致板厚错误 WJH--2005/10/18
			pFrontPlank->set_thick(foot_para.m_iUpXueBanThick);
			if(pBackPlank)
			pBackPlank->set_thick(foot_para.m_iUpXueBanThick);*/
			//确定射线斜材角钢的设计参数
			double ber_space=0,wing_space=0;
			ATOM_LIST<CDesignLsPara>ls_list;
			LayRayAngleBolt(&ls_list,pAngle,pBaseJg,pBackPlank,&ber_space,&wing_space);
			
			f3dLine ber_line;
			LSSPACE_STRU LsSpace;
			f3dPoint ls_pos,base_pos,line_vec;
			double max_ber_dist=0,min_ber_dist=pAngle->GetWidth();
			double head_len=0,tail_len=0;	//头->尾方向与射线方向一致
			BOOL bFirstLs=TRUE;
			int head_bolt_d=20,end_bolt_d=20;
			ber_line.startPt=pAngle->Start();
			ber_line.endPt=pAngle->End();
			if(pAngle->pStart->handle==m_pFoot->base_node_handle)
			{
				head_len=tail_len=-pAngle->startOdd();
				base_pos=pAngle->Start();
				line_vec=pAngle->End()-pAngle->Start();
			}
			else
			{
				head_len=tail_len=-pAngle->endOdd();
				base_pos=pAngle->End();
				line_vec=pAngle->Start()-pAngle->End();
			}
			normalize(line_vec);
			//根据用户输入在角钢上布置螺栓
			int iInitRayNo=1;
			ATOM_LIST<RAYNO_RECORD>rayNoList;
			for(CDesignLsPara *pLsPara=ls_list.GetFirst();pLsPara;pLsPara=ls_list.GetNext())
			{
				CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
				pBolt->des_base_pos=pLsPara->des_base_pos;
				pBolt->des_work_norm=pLsPara->des_work_norm;
				pBolt->set_d(pLsPara->d);
				pBolt->iSeg = pLsPara->iSeg;
				pBolt->SetGrade(pLsPara->grade);
				pBolt->cfgword=pAngle->cfgword;	//调整螺栓配材号与基准构件配材号一致
				if(m_pFoot->base_node_handle==pAngle->pStart->handle)
					pAngle->AppendStartLsRef(pBolt->GetLsRef());
				else if(m_pFoot->base_node_handle==pAngle->pEnd->handle)
					pAngle->AppendEndLsRef(pBolt->GetLsRef());
				else
					pAngle->AppendMidLsRef(pBolt->GetLsRef());
				//向板构件及角钢中添加螺栓引用
				pBackPlank->AppendLsRef(pBolt->GetLsRef());
				pBolt->AddL0Thick(pAngle->handle,TRUE);
				pBolt->AddL0Thick(pBackPlank->handle,TRUE);
				pBolt->CalGuigeAuto();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				//
				f3dPoint ls_pos=pBolt->ucs.origin;
				if(xie_x_wing0_y_wing1==0)
					project_point(ls_pos,base_pos,pAngle->get_norm_x_wing());
				else
					project_point(ls_pos,base_pos,pAngle->get_norm_y_wing());
				double dist;
				SnapPerp(&ls_pos,ber_line,ls_pos,&dist);
				if(bFirstLs)
					max_ber_dist=min_ber_dist=dist;
				else if(dist>max_ber_dist)
					max_ber_dist=dist;
				else if(dist<min_ber_dist)
					min_ber_dist=dist;
				f3dPoint ls_offset_vec = ls_pos-base_pos;
				if(ls_offset_vec*line_vec>0)
					dist = ls_offset_vec.mod();//+xDesJgResult.oddment;
				else
					dist = -ls_offset_vec.mod();//+xDesJgResult.oddment;
				if(bFirstLs)
				{
					head_len=tail_len=dist;
					head_bolt_d=end_bolt_d=pBolt->get_d();
				}
				else if(dist>tail_len)
				{
					tail_len=dist;
					end_bolt_d=pBolt->get_d();
				}
				else if(dist<head_len)
				{
					head_len=dist;
					head_bolt_d=pBolt->get_d();
				}
				bFirstLs=FALSE;
				//
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				double g=pAngle->GetLsG(pBolt);
				RAYNO_RECORD *pRayNo;
				for(pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
				{
					if(ftoi(pRayNo->yCoord)==ftoi(g))
					{
						pBolt->dwRayNo=pRayNo->dwRayNo;
						break;
					}
				}
				if(pRayNo==NULL)
				{
					pRayNo=rayNoList.append();
					pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
					pRayNo->hPart=pBolt->des_base_pos.hPart;
					pRayNo->yCoord=g;
					pBolt->dwRayNo=pRayNo->dwRayNo;
					iInitRayNo++;
				}
			}
			//添加与钢板相关联的杆件 主要用于对称生成塔脚板时将螺栓引入相应的角钢上并调整角钢正负头 wht 09-09-07
			pLjPartPara=pBackPlank->designInfo.partList.Add(pAngle->handle);
			pLjPartPara->iFaceNo=1;
			if(pAngle->pStart&&pAngle->pStart->handle==m_pFoot->base_node_handle)
				pLjPartPara->start0_end1=0;
			else if(pAngle->pEnd&&pAngle->pEnd->handle==m_pFoot->base_node_handle)
				pLjPartPara->start0_end1=1;
			else
				pLjPartPara->start0_end1=2;
			pLjPartPara->angle.cur_wing_x0_y1=xie_x_wing0_y_wing1;
			
			//定制外形
			f3dPoint wing_vec,vertice,vv,vertex_up,vertex_down;
			if(xie_x_wing0_y_wing1==0)
				wing_vec=pAngle->GetWingVecX();
			else
				wing_vec=pAngle->GetWingVecY();
			GetLsSpace(LsSpace,end_bolt_d);
			vv=base_pos+line_vec*(tail_len+LsSpace.EndSpace);
			vertice=vv+wing_vec*(min_ber_dist-ber_space);
			coord_trans(vertice,pBackPlank->ucs,FALSE);
			vertice.z=0;
			vertex_up=vertice;		//用于计算焊接线位置
			PROFILE_VERTEX *pVertex=pBackPlank->vertex_list.append(PROFILE_VERTEX(vertice));
			vertice=vv+wing_vec*(max_ber_dist+wing_space);
			coord_trans(vertice,pBackPlank->ucs,FALSE);
			vertice.z=0;
			vertex_down=vertice;	//用于计算焊接线位置
			pVertex=pBackPlank->vertex_list.append(PROFILE_VERTEX(vertice));
			//
			if(m_pFoot->base_node_handle==pAngle->pStart->handle&&pAngle->desStartOdd.m_iOddCalStyle==1)
				pAngle->CalStartOddment();
			else if(m_pFoot->base_node_handle==pAngle->pEnd->handle&&pAngle->desEndOdd.m_iOddCalStyle==1)
				pAngle->CalEndOddment();
			pAngle->SetModified();
		}
		pLjPartPara=pBackPlank->designInfo.partList.Add(pBaseJg->handle);
		pLjPartPara->iFaceNo=1;
		if(pBaseJg->pStart&&pBaseJg->pStart->handle==m_pFoot->base_node_handle)
			pLjPartPara->start0_end1=0;
		else if(pBaseJg->pEnd&&pBaseJg->pEnd->handle==m_pFoot->base_node_handle)
			pLjPartPara->start0_end1=1;
		else
			pLjPartPara->start0_end1=2;
		pLjPartPara->angle.cur_wing_x0_y1=base_x_wing0_y_wing1;
		g_pSolidDraw->Draw();	
	}
	//添加与基准杆件到钢板关联构件列表 主要用于对称生成时将螺栓引入相应的角钢上并调整角钢正负头 wht 11-01-13
	CLDSPlate *platePtrArr[8]={tm.pXUpFrontPlank,tm.pXUpBackPlank,tm.pYUpFrontPlank,tm.pYUpBackPlank,
							   tm.pXDownFrontPlank,tm.pXDownBackPlank,tm.pYDownFrontPlank,tm.pYDownBackPlank};
	for(int i=0;i<8;i++)
	{
		if(platePtrArr[i]==NULL)
			continue;
		if(i<4)	//上侧基准角钢
			pBaseJg=pUpBaseJg;
		else	//下侧基准角钢
			pBaseJg=pDownBaseJg;
		if(pBaseJg==NULL)
			continue;
		if(!platePtrArr[i]->designInfo.FromHandle(pBaseJg->handle))
		{	//基准角钢未添加到钢板构件链表在此处添加
			pLjPartPara=platePtrArr[i]->designInfo.partList.Add(pBaseJg->handle);
			pLjPartPara->iFaceNo=1;
			if(pBaseJg->pStart&&pBaseJg->pStart->handle==m_pFoot->base_node_handle)
				pLjPartPara->start0_end1=0;
			else if(pBaseJg->pEnd&&pBaseJg->pEnd->handle==m_pFoot->base_node_handle)
				pLjPartPara->start0_end1=1;
			else
				pLjPartPara->start0_end1=2;
			int base_x_wing0_y_wing1=0;
			IsInsideJg(pBaseJg,platePtrArr[i]->ucs.axis_z,&base_x_wing0_y_wing1);
			pLjPartPara->angle.cur_wing_x0_y1=base_x_wing0_y_wing1;
		}
	}
	return true;
}
bool CFootDesignDlg::DesignXueBan()
{
	CTmaPtrList<CLDSLineAngle,CLDSLineAngle*> jgset;
	tm.pXUpFrontPlank=tm.pXUpBackPlank=tm.pYUpFrontPlank=tm.pYUpBackPlank=NULL;
	tm.pXDownFrontPlank=tm.pXDownBackPlank=tm.pYDownFrontPlank=tm.pYDownBackPlank=NULL;
	CLDSLineAngle *pBaseJg=NULL,*pUpBaseJg=NULL,*pDownBaseJg=NULL;
	//获得基准节点
	CLDSNode *pBaseNode = console.FromNodeHandle(m_pFoot->base_node_handle);
	if(!GetLinkJgSet(jgset))	//获得塔脚板连接角钢集
		return false;
	try{
		//1.获得上下两根基准角钢
		if(!GetBaseJg(pUpBaseJg,pDownBaseJg))
			return false;
		//2.设计靴板的基本信息及基准角钢
		for(int i=0;i<2;i++)
		{
			int m_iLsLayStyle;	//基准角钢上螺栓的布置方式
			// 2.1获取当前的设计参数信息
			tm.face_pick=m_pFoot->ucs.origin;
			if(pUpBaseJg&&pUpBaseJg->handle==m_pFoot->attach_jg_handle[i])
			{	//上侧基准材
				pBaseJg=pUpBaseJg;
				tm.bUpJg=true;
				tm.C=foot_para.m_iUpC;
				m_iLsLayStyle=foot_para.m_iUpLsLayStyle;
			}
			else if(pDownBaseJg&&pDownBaseJg->handle==m_pFoot->attach_jg_handle[i])
			{	//下侧基准材
				pBaseJg=pDownBaseJg;
				tm.bUpJg=false;
				tm.face_pick.z+=foot_para.m_iThick;	//加上一个底板厚
				tm.C=foot_para.m_iDownC;
				m_iLsLayStyle=foot_para.m_iDownLsLayStyle;
			}
			else
				pBaseJg=NULL;
			if(pBaseJg)
			{
				// 2.2获取当前基准角钢的基本信息
				if(pBaseJg->pStart->handle==m_pFoot->base_node_handle)
				{
					tm.jg_vec=pBaseJg->Start()-pBaseJg->End();
					normalize(tm.jg_vec);
				}
				else
				{
					tm.jg_vec=pBaseJg->End()-pBaseJg->Start();
					normalize(tm.jg_vec);
				}
				tm.wing_wide=pBaseJg->GetWidth();
				tm.wing_thick=pBaseJg->GetThick();
				tm.wing_vec_x = pBaseJg->GetWingVecX();
				tm.wing_vec_y = pBaseJg->GetWingVecY();

				//以上为搜集底脚板设计过程的参数信息
				// 2.3计算当前基准材的正负头及切角切肢情况
				CalBaseJgOddment(pBaseJg);
				// 2.4计算基准材与底座板边缘的交点
				CalBaseJgPlankIntVertex(pBaseJg,tm.vert_arr,tm.xueban_vert_arr);
				tm.xueban_vert_arr[0].z=tm.xueban_vert_arr[1].z=tm.xueban_vert_arr[2].z=tm.xueban_vert_arr[3].z=m_pFoot->ucs.origin.z-tm.face_pick.z;
				// 2.5设计靴板的坐标及外形信息
				CalXueBanPara(pBaseJg);
				// 2.6设计底脚板(牛蹄板)上的基准材螺栓信息
				if(!m_pFoot->lj_by_weld)	//只有非焊接连接需要螺栓,焊接连接时不需要螺栓
					LayBaseJgLs(pBaseJg);
			}
		}
		// 3.设计与底脚板(牛蹄板)相连接的射线材位置、螺栓及正负头信息
		DesRayJg(jgset,pUpBaseJg,pDownBaseJg);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	if(tm.pXUpFrontPlank)
		tm.pXUpFrontPlank->cMaterial=foot_para.m_cXueBanMat;
	if(tm.pXUpBackPlank)
		tm.pXUpBackPlank->cMaterial=foot_para.m_cXueBanMat;
	if(tm.pYUpFrontPlank)
		tm.pYUpFrontPlank->cMaterial=foot_para.m_cXueBanMat;
	if(tm.pYUpBackPlank)
		tm.pYUpBackPlank->cMaterial=foot_para.m_cXueBanMat;

	if(tm.pXDownFrontPlank)
		tm.pXDownFrontPlank->cMaterial=foot_para.m_cXueBanMat;
	if(tm.pXDownBackPlank)
		tm.pXDownBackPlank->cMaterial=foot_para.m_cXueBanMat;
	if(tm.pYDownFrontPlank)
		tm.pYDownFrontPlank->cMaterial=foot_para.m_cXueBanMat;
	if(tm.pYDownBackPlank)
		tm.pYDownBackPlank->cMaterial=foot_para.m_cXueBanMat;
	return TRUE;
}
BOOL CFootDesignDlg::XMLDataSerialize(CString sFileName, BOOL bWrite)
{
	CMarkup xml;
	if(bWrite)
	{	//导出XML文件
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		xml.AddElem("塔脚板");
		xml.AddAttrib("类型","角钢");
		ExportFootDataToXML(xml);
		xml.Save(sFileName);
	}
	else
	{	//导入XML文件
		if(!xml.Load(sFileName))
		{	//加载XML文件
			AfxMessageBox("读取文件失败!");
			return FALSE;
		}
		xml.ResetMainPos();
		ImportFootDataFromXML(xml,"基本参数");
		ImportFootDataFromXML(xml,"底板参数");
		ImportFootDataFromXML(xml,"上靴板参数");
		ImportFootDataFromXML(xml,"下靴板参数");
	}
	return TRUE;
}
bool CFootDesignDlg::ImportFootDataFromXML(CMarkup xml,CString sElemName,BOOL bUpXueBan/*=FALSE*/)
{
	CString sValue=_T("");
	while(xml.FindChildElem(sElemName))   
	{ 
		xml.IntoElem();
		if(sElemName.CompareNoCase("基本参数")==0)
		{
			sValue=xml.GetAttrib("A");
			foot_para.m_iA=atoi(sValue);
			sValue=xml.GetAttrib("B");
			foot_para.m_iB=atoi(sValue);
			sValue=xml.GetAttrib("上侧C");
			foot_para.m_iUpC=atoi(sValue);
			sValue=xml.GetAttrib("下侧C");
			foot_para.m_iDownC=atoi(sValue);
			sValue=xml.GetAttrib("靴板材质");
			foot_para.m_cXueBanMat=sValue[0];
		}
		else if(sElemName.CompareNoCase("底板参数")==0)
		{
			sValue=xml.GetAttrib("底板外形");
			if(sValue.CompareNoCase("方形")==0)
				foot_para.m_iDiBanShape=0;
			else
				foot_para.m_iDiBanShape=1;
			sValue=xml.GetAttrib("底板宽");
			foot_para.m_iWide=atoi(sValue);
			sValue=xml.GetAttrib("底板厚");
			foot_para.m_iThick=atoi(sValue);
			sValue=xml.GetAttrib("底板材质");
			foot_para.m_cDiBanMat=sValue[0];
			sValue=xml.GetAttrib("底板编号");
			strncpy(foot_para.sDiBanPartNo,sValue.GetBuffer(),16);
			sValue=xml.GetAttrib("底板孔距");
			foot_para.m_iS=atoi(sValue);
			sValue=xml.GetAttrib("焊接底板");
			if(sValue.CompareNoCase("是")==0)
				foot_para.m_bWeldDiZuo=TRUE;
			else
				foot_para.m_bWeldDiZuo=FALSE;
		}
		else if(sElemName.CompareNoCase("上靴板参数")==0)
		{
			bUpXueBan=TRUE;
			sValue=xml.GetAttrib("靴板厚");
			foot_para.m_iUpXueBanThick=atoi(sValue);
			sValue=xml.GetAttrib("螺栓布置方式");
			foot_para.m_iUpLsLayStyle=atoi(sValue);
			sValue=xml.GetAttrib("水平截断");
			if(sValue.CompareNoCase("是")==0)
				foot_para.m_bBreakUpXueBan0C=TRUE;
			else
				foot_para.m_bBreakUpXueBan0C=FALSE;
			sValue=xml.GetAttrib("竖直截断");
			if(sValue.CompareNoCase("是")==0)
				foot_para.m_bBreakUpXueBan90C=TRUE;
			else
				foot_para.m_bBreakUpXueBan90C=FALSE;
			ImportFootDataFromXML(xml,"X-PARTNO",TRUE);
			ImportFootDataFromXML(xml,"Y-PARTNO",TRUE);
		}
		else if(sElemName.CompareNoCase("下靴板参数")==0)
		{
			bUpXueBan=FALSE;
			sValue=xml.GetAttrib("靴板厚");
			foot_para.m_iDownXueBanThick=atoi(sValue);
			sValue=xml.GetAttrib("螺栓布置方式");
			foot_para.m_iDownLsLayStyle=atoi(sValue);
			sValue=xml.GetAttrib("水平截断");
			if(sValue.CompareNoCase("是")==0)
				foot_para.m_bBreakDownXueBan0C=TRUE;
			else
				foot_para.m_bBreakDownXueBan0C=FALSE;
			sValue=xml.GetAttrib("竖直截断");
			if(sValue.CompareNoCase("是")==0)
				foot_para.m_bBreakDownXueBan90C=TRUE;
			else
				foot_para.m_bBreakDownXueBan90C=FALSE;
			ImportFootDataFromXML(xml,"X-PARTNO",FALSE);
			ImportFootDataFromXML(xml,"Y-PARTNO",FALSE);
		}
		else if(sElemName.CompareNoCase("X-PARTNO")==0)
		{
			sValue=xml.GetAttrib("ANGLE_0");
			if(bUpXueBan)
				strncpy(foot_para.sUpXueBanPartNo0C,sValue,16);
			else
				strncpy(foot_para.sDownXueBanPartNo0C,sValue,16);
			sValue=xml.GetAttrib("ANGLE_45");
			if(bUpXueBan)
				strncpy(foot_para.sUpXueBanPartNo45C,sValue,16);
			else
				strncpy(foot_para.sDownXueBanPartNo45C,sValue,16);
			sValue=xml.GetAttrib("ANGLE_180");
			if(bUpXueBan)
				strncpy(foot_para.sUpXueBanPartNo180C,sValue,16);
			else
				strncpy(foot_para.sDownXueBanPartNo180C,sValue,16);
			sValue=xml.GetAttrib("ANGLE_225");
			if(bUpXueBan)
				strncpy(foot_para.sUpXueBanPartNo225C,sValue,16);
			else
				strncpy(foot_para.sDownXueBanPartNo225C,sValue,16);
		}
		else if(sElemName.CompareNoCase("Y-PARTNO")==0)
		{
			sValue=xml.GetAttrib("ANGLE_90");
			if(bUpXueBan)
				strncpy(foot_para.sUpXueBanPartNo90C,sValue,16);
			else
				strncpy(foot_para.sDownXueBanPartNo90C,sValue,16);
			sValue=xml.GetAttrib("ANGLE_135");
			if(bUpXueBan)
				strncpy(foot_para.sUpXueBanPartNo135C,sValue,16);
			else
				strncpy(foot_para.sDownXueBanPartNo135C,sValue,16);
			sValue=xml.GetAttrib("ANGLE_270");
			if(bUpXueBan)
				strncpy(foot_para.sUpXueBanPartNo270C,sValue,16);
			else
				strncpy(foot_para.sDownXueBanPartNo270C,sValue,16);
			sValue=xml.GetAttrib("ANGLE_315");
			if(bUpXueBan)
				strncpy(foot_para.sUpXueBanPartNo315C,sValue,16);
			else
				strncpy(foot_para.sDownXueBanPartNo315C,sValue,16);
		}
		xml.OutOfElem();
	}
	return true;
}
bool CFootDesignDlg::ExportFootDataToXML(CMarkup& xml)
{
	CString sText;
	//1、基本参数
	xml.IntoElem();
	xml.AddElem("基本参数");	//添加节间
	sText.Format("%d",foot_para.m_iA);
	xml.AddAttrib("A",sText);
	sText.Format("%d",foot_para.m_iB);
	xml.AddAttrib("B",sText);
	sText.Format("%d",foot_para.m_iUpC);
	xml.AddAttrib("上侧C",sText);
	sText.Format("%d",foot_para.m_iDownC);
	xml.AddAttrib("下侧C",sText);
	sText.Format("%c",foot_para.m_cXueBanMat);	//靴板材质
	xml.AddAttrib("靴板材质",sText);
	xml.OutOfElem();		//跳出该节间
	//2、底板残数
	xml.IntoElem();
	xml.AddElem("底板参数");
	if(foot_para.m_iDiBanShape==0)
		sText="方形";
	else if(foot_para.m_iDiBanShape==1)
		sText="圆形";
	xml.AddAttrib("底板外形",sText);
	sText.Format("%d",foot_para.m_iWide);		//底板宽
	xml.AddAttrib("底板宽",sText);
	sText.Format("%d",foot_para.m_iThick);		//底板厚
	xml.AddAttrib("底板厚",sText);
	sText.Format("%c",foot_para.m_cDiBanMat);	//底板材质
	xml.AddAttrib("底板材质",sText);
	sText.Format("%s",foot_para.sDiBanPartNo);	//底板编号
	xml.AddAttrib("底板编号",sText);
	sText.Format("%d",foot_para.m_iS);			//底板孔距
	xml.AddAttrib("底板孔距",sText);
	if(foot_para.m_bWeldDiZuo)					//是否焊接底板
		sText="是";
	else
		sText="否";
	xml.AddAttrib("焊接底板",sText);
	xml.OutOfElem();	
	//3、上靴板参数
	xml.IntoElem();
	xml.AddElem("上靴板参数");
	sText.Format("%d",foot_para.m_iUpXueBanThick);	//上靴板厚
	xml.AddAttrib("靴板厚",sText);
	sText.Format("%d",foot_para.m_iUpLsLayStyle);	//螺栓布置方式
	xml.AddAttrib("螺栓布置方式",sText);
	if(foot_para.m_bBreakUpXueBan0C)				//沿水平截断
		sText="是";
	else 
		sText="否";
	xml.AddAttrib("水平截断",sText);
	if(foot_para.m_bBreakUpXueBan90C)			//沿竖直截断
		sText="是";
	else
		sText="否";
	xml.AddAttrib("竖直截断",sText);
	//3.1、X肢靴板编号
	xml.IntoElem();
	xml.AddElem("X-PARTNO");
	sText.Format("%s",foot_para.sUpXueBanPartNo0C);
	xml.AddAttrib("ANGLE_0",sText);
	sText.Format("%s",foot_para.sUpXueBanPartNo45C);
	xml.AddAttrib("ANGLE_45",sText);
	sText.Format("%s",foot_para.sUpXueBanPartNo180C);
	xml.AddAttrib("ANGLE_180",sText);
	sText.Format("%s",foot_para.sUpXueBanPartNo225C);
	xml.AddAttrib("ANGLEJ_225",sText);
	xml.OutOfElem();
	//3.2、Y肢靴板编号
	xml.IntoElem();
	xml.AddElem("Y-PARTNO");
	sText.Format("%s",foot_para.sUpXueBanPartNo90C);
	xml.AddAttrib("ANGLE_90",sText);
	sText.Format("%s",foot_para.sUpXueBanPartNo135C);
	xml.AddAttrib("ANGLE_135",sText);
	sText.Format("%s",foot_para.sUpXueBanPartNo270C);
	xml.AddAttrib("ANGLE_270",sText);
	sText.Format("%s",foot_para.sUpXueBanPartNo315C);
	xml.AddAttrib("ANGLE_315",sText);
	xml.OutOfElem();
	xml.OutOfElem();
	//4、下靴板参数
	xml.IntoElem();
	xml.AddElem("下靴板参数");
	sText.Format("%d",foot_para.m_iDownXueBanThick);	//下靴板厚
	xml.AddAttrib("靴板厚",sText);
	sText.Format("%d",foot_para.m_iDownLsLayStyle);	//螺栓布置方式
	xml.AddAttrib("螺栓布置方式",sText);
	if(foot_para.m_bBreakDownXueBan0C)				//沿水平截断
		sText="是";
	else 
		sText="否";
	xml.AddAttrib("水平截断",sText);
	if(foot_para.m_bBreakDownXueBan90C)			//沿竖直截断
		sText="是";
	else
		sText="否";
	xml.AddAttrib("竖直截断",sText);
	//4.1、X肢靴板编号
	xml.IntoElem();
	xml.AddElem("X-PARTNO");
	sText.Format("%s",foot_para.sDownXueBanPartNo0C);
	xml.AddAttrib("ANGLE_0",sText);
	sText.Format("%s",foot_para.sDownXueBanPartNo45C);
	xml.AddAttrib("ANGLE_45",sText);
	sText.Format("%s",foot_para.sDownXueBanPartNo180C);
	xml.AddAttrib("ANGLE_180",sText);
	sText.Format("%s",foot_para.sDownXueBanPartNo225C);
	xml.AddAttrib("ANGLE_225",sText);
	xml.OutOfElem();
	//4.2、Y肢靴板编号
	xml.IntoElem();
	xml.AddElem("Y-PARTNO");
	sText.Format("%s",foot_para.sDownXueBanPartNo90C);
	xml.AddAttrib("ANGLE_90",sText);
	sText.Format("%s",foot_para.sDownXueBanPartNo135C);
	xml.AddAttrib("ANGLE_135",sText);
	sText.Format("%s",foot_para.sDownXueBanPartNo270C);
	xml.AddAttrib("ANGLE_270",sText);
	sText.Format("%s",foot_para.sDownXueBanPartNo315C);
	xml.AddAttrib("ANGLE_315",sText);
	xml.OutOfElem();
	xml.OutOfElem();
	return true;
}
void CFootDesignDlg::OnBnImPortXMLFile()
{
	char *sName="fll";
	char *sFileType="塔脚板(*.xml)|*.xml||";
	CFileDialog dlg(TRUE,sName,"塔脚板",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()!=IDOK)
		return;
	CString sFileName;
	sFileName = dlg.GetPathName();
	XMLDataSerialize(sFileName,FALSE);
	//
	InitGraph();
	Invalidate();
}
#endif

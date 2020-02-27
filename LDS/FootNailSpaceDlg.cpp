// FootnailSpaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "Query.h"
#include "FootnailSpaceDlg.h"
#include "DesignFootNailPlateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFootnailSpaceDlg dialog


int CFootnailSpaceDlg::nScrLocationX=0;
int CFootnailSpaceDlg::nScrLocationY=0;
CFootnailSpaceDlg::CFootnailSpaceDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CFootnailSpaceDlg::IDD, pParent)
	, m_bDualSide(0)
	, wing_wide(0)
	, m_bHorizonSlotNail(FALSE)
{
	m_bEnableWindowMoveListen=false;
	//{{AFX_DATA_INIT(CFootnailSpaceDlg)
	m_fFocusZoomCoef=g_sysPara.m_fMaxScaleScr2User;
	m_nNailSpace = m_nDefaultNailSpace = g_sysPara.FootNailDist;;
	m_fNailAngle = 0;
	m_fNailAngle2 = 120;
	m_fCurNailAngle = 0;
	m_fLenUseDist = 0;
	m_sCanUseLen = _T("");
	m_sCurJg = _T("");
	m_sSumLen = _T("");
	m_sNailGuiGe = _T("");
	m_bShiftWing = TRUE;
	m_iDatumLine = 0;
	//}}AFX_DATA_INIT
	m_bEnableTeG=FALSE;
	m_iLineType = 0; //0角钢 1钢管
	m_bHasWeldRoad = FALSE;
	m_pReplacerBolt=NULL;
	xNailBolt.handle=3;	//临时赋一随机<0x20的句柄标识号，否则无法实时显示实体
	xNailSlot.handle=4;	//同上
	xNailAngle.handle=4;//同上但不与槽钢同时出现
	xNailBolt2.handle=5;	//临时赋一随机<0x20的句柄标识号，否则无法实时显示实体
	xNailSlot2.handle=6;	//同上
	m_bSearchReplaceBolt=TRUE;
	m_fPreRodRemainingLen=0;
	hFirstLineSlot=0;
	pageOnAngle.m_pParentWnd=this;
	pageOnTube.m_pParentWnd=this;
	m_iOldLayWing=pageOnAngle.m_iLayWing;
	m_iOldNailType=pageOnTube.m_iNailType;
}


void CFootnailSpaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFootnailSpaceDlg)
	DDX_Text(pDX, IDC_E_NAIL_SPACE, m_nNailSpace);
	DDV_MinMaxInt(pDX, m_nNailSpace, 0, 2000000);
	DDX_Text(pDX, IDC_E_DEFAULT_NAIL_SPACE, m_nDefaultNailSpace);
	DDV_MinMaxInt(pDX, m_nDefaultNailSpace, 0, 2000000);
	DDX_Text(pDX, IDC_E_NAIL_ANGLE, m_fNailAngle);
	DDX_Text(pDX, IDC_E_NAIL_ANGLE2, m_fNailAngle2);
	DDX_Text(pDX, IDC_E_CUR_NAIL_ANGLE, m_fCurNailAngle);
	DDX_Text(pDX, IDC_S_CAN_USE_LEN, m_sCanUseLen);
	DDX_Text(pDX, IDC_S_CUR_JG, m_sCurJg);
	DDX_Text(pDX, IDC_S_SUM_LEN, m_sSumLen);
	DDX_CBString(pDX, IDC_CMB_FOOT_NAIL_GUIGE, m_sNailGuiGe);
	DDX_Check(pDX, IDC_CHK_SHIFT_WING, m_bShiftWing);
	DDX_Radio(pDX, IDC_RDO_DATUM_LINE, m_iDatumLine);
	DDX_Check(pDX, IDC_CHK_SEARCH_REPLACE_BOLT, m_bSearchReplaceBolt);
	DDX_Check(pDX, IDC_CHK_HORIZON_NAIL, m_bHorizonSlotNail);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHK_DUAL_SIDE, m_bDualSide);
}


BEGIN_MESSAGE_MAP(CFootnailSpaceDlg, CDialog)
	//{{AFX_MSG_MAP(CFootnailSpaceDlg)
	ON_BN_CLICKED(IDC_CHK_SHIFT_WING, OnChkShiftWing)
	ON_EN_CHANGE(IDC_E_NAIL_ANGLE, OnChangeNailAngle)
	ON_EN_CHANGE(IDC_E_NAIL_ANGLE2,OnChangeNailAngle2)
	ON_EN_CHANGE(IDC_E_CUR_NAIL_ANGLE,OnChangeCurNailAngle)
	ON_BN_CLICKED(IDC_RDO_DATUM_LINE, OnRdoDatumLine)
	ON_BN_CLICKED(IDC_RDO_DATUM_LINE2, OnRdoDatumLine)
	ON_BN_CLICKED(IDC_CHK_DUAL_SIDE, OnChkDualSide)
	ON_BN_CLICKED(IDC_BN_PICK_REPLACE_NAILBOLT, &CFootnailSpaceDlg::OnBnPickReplaceNailbolt)
	ON_EN_CHANGE(IDC_E_NAIL_SPACE, &CFootnailSpaceDlg::OnChangeENailSpace)
	ON_EN_CHANGE(IDC_E_DEFAULT_NAIL_SPACE, &CFootnailSpaceDlg::OnEnChangeDefaultNailSpace)
	ON_WM_MOVE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHK_SEARCH_REPLACE_BOLT, &CFootnailSpaceDlg::OnClickedChkSearchReplaceBolt)
	ON_BN_CLICKED(IDC_CHK_HORIZON_NAIL, &CFootnailSpaceDlg::OnChkHorizonNail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFootnailSpaceDlg message handlers

void CFootnailSpaceDlg::InitState(int hCurRod,double CanUseLen,double SumLen,double lenOffset,double fAngle,double fAngle2)
{
	m_sCurJg.Format("0X%X",hCurRod);
	m_sCanUseLen.Format("%.1f",CanUseLen);
	m_sSumLen.Format("%.1f",SumLen);
	m_fLenUseDist=lenOffset;
	m_fNailAngle = fAngle;
	m_fNailAngle2 = fAngle2;
	//if(!m_bHorizonSlotNail) 水平脚钉也应该支持交替布置 wht 15-12-18
	{
		if(pageOnTube.m_iLaySide==0)
			m_fCurNailAngle=m_fNailAngle;
		else
			m_fCurNailAngle=m_fNailAngle2+m_fNailAngle;
	}
}

CLDSBolt* CFootnailSpaceDlg::SearchReplaceBolt(double preferStepSpace/*=420*/,bool bUpdateNailSpace/*=true*/)
{
	f3dPoint inters;
	int wingx0_y1=pageOnAngle.m_iLayWing;	//钢管时wingx0_y1实际无效，必须赋值以保证一致
	CLDSBolt* pMergeBolt=NULL;
	preferStepSpace-=m_fPreRodRemainingLen;	//搜索代孔螺栓时考虑前一杆件的剩余长度
	for(CLsRef* pLsRef=m_pRod->GetFirstLsRef();pLsRef;pLsRef=m_pRod->GetNextLsRef())
	{
		if(m_pRod->GetBoltIntersPos(pLsRef->GetLsPtr(),inters,&wingx0_y1)&&wingx0_y1==pageOnAngle.m_iLayWing)
		{
			double space=ftoi(m_xDatumLenVec*(inters-m_xDatumStart)-m_fLenUseDist);
			if(fabs(space-preferStepSpace)<g_sysPara.fNailRepScope)
			{
				pMergeBolt=pLsRef->GetLsPtr();
				break;
			}
		}
	}
	if(bUpdateNailSpace&&pMergeBolt)
	{
		m_pMergeBolt=pMergeBolt;
		xNailBolt.ucs.origin=m_pMergeBolt->ucs.origin;
		xNailBolt.ucs.axis_z=m_pMergeBolt->ucs.axis_z;
		xNailBolt.set_d(m_pMergeBolt->get_d());
		xNailBolt.SetL0ThickPara(*m_pMergeBolt->GetL0ThickPara());
		f3dPoint inters;
		if(m_pRod->GetBoltIntersPos(m_pMergeBolt,inters))
		{
			m_nNailSpace=ftoi(m_xDatumLenVec*(inters-m_xDatumStart)-m_fLenUseDist);
			m_nNailSpace+=(int)m_fPreRodRemainingLen;	//根据代孔螺栓计算出的脚钉间距应加上前一杆件的剩余长度
		}
		GetDlgItem(IDC_S_REPLACER_BOLT)->SetWindowText(CXhChar16("0X%X",m_pMergeBolt->handle));
		UpdateData(FALSE);
	}
	return pMergeBolt;
}
void CFootnailSpaceDlg::InitLineAngleFootNail()
{
	restore_Ls_guige(m_sNailGuiGe,xNailBolt);
	xNailBolt.ucs.origin=m_xDatumStart+m_xDatumLenVec*(m_fLenUseDist+m_nNailSpace);
	if(pageOnAngle.m_iLayWing==0)
	{
		xNailBolt.ucs.axis_z=m_pRod.LineAnglePointer()->get_norm_x_wing();
		xNailBolt.ucs.origin+=pageOnAngle.m_nGDist*m_pRod.LineAnglePointer()->GetWingVecX();
		xNailBolt.ucs.origin-=m_pRod.LineAnglePointer()->get_norm_x_wing()*m_pRod->GetThick();
	}
	else
	{
		xNailBolt.ucs.axis_z=m_pRod.LineAnglePointer()->get_norm_y_wing();
		xNailBolt.ucs.origin+=pageOnAngle.m_nGDist*m_pRod.LineAnglePointer()->GetWingVecY();
		xNailBolt.ucs.origin-=m_pRod.LineAnglePointer()->get_norm_y_wing()*m_pRod->GetThick();
	}
	xNailBolt.SetL0KeyStr(CXhChar16("0X%X",m_pRod.LineAnglePointer()->handle));
	xNailBolt.CalGuigeAuto();
}
void CFootnailSpaceDlg::InitLineTubeFootNail(BOOL bNewCreate/*=FALSE*/)
{
	if(m_pRod==NULL || !m_pRod->IsTube())
		return;
	if(pageOnTube.m_iNailType==0)		//槽钢底座脚钉
	{	//1、初始化槽钢的外形参数
		CLDSLineTube* pTube=m_pRod.LineTubePointer();
		CLDSLineSlot *pStandardFootNailPlate = (CLDSLineSlot*)console.FromPartHandle(hFirstLineSlot,CLS_LINESLOT);
		restore_Ls_guige(m_sNailGuiGe,xNailBolt);
		if(pStandardFootNailPlate)
		{
			xNailSlot.CopyProperty(pStandardFootNailPlate);
			xNailSlot.feature=pStandardFootNailPlate->feature;	//脚钉的长度
		}
		else if(bNewCreate)
		{	//设置脚钉板参数
			CDesignFootNailPlateDlg FootNailPlateDlg;
			FootNailPlateDlg.m_iBoltD = xNailBolt.get_d();
			FootNailPlateDlg.m_pLineTube = pTube;
			FootNailPlateDlg.part_type=0;	//槽钢底座式脚钉
			if(FootNailPlateDlg.DoModal()!=IDOK)
				return;
			xNailSlot.size_wide = FootNailPlateDlg.m_fParamW;
			xNailSlot.feature = (long)FootNailPlateDlg.m_fParamL;//存储脚钉长度
			xNailSlot.size_height = FootNailPlateDlg.m_fParamH;
			xNailSlot.size_thick = FootNailPlateDlg.m_fParamT;
			xNailSlot.cMaterial = CSteelMatLibrary::RecordAt(FootNailPlateDlg.m_iMaterial).cBriefMark;
			xNailSlot.iSeg = SEGI(FootNailPlateDlg.m_sSegNo.GetBuffer());
			xNailSlot.SetPartNo(FootNailPlateDlg.m_sPartNo.GetBuffer());
			strcpy(xNailSlot.spec,FootNailPlateDlg.m_sGuiGe);//槽钢规格 
		}
		//2、计算槽钢的坐标系
		xNailSlot.ucs.axis_y=-pTube->ucs.axis_x;	
		f3dPoint origin,axis_z=pTube->End()-pTube->Start();
		double rot_angle = pTube->CalWeldRoadAngle();
		if(m_iDatumLine==1)//以基准边为基准还是以焊道线为基准
			RotateVectorAroundVector(xNailSlot.ucs.axis_y,rot_angle,axis_z);
		double cur_angle=m_fCurNailAngle*RADTODEG_COEF;
		RotateVectorAroundVector(xNailSlot.ucs.axis_y,cur_angle,axis_z);
		xNailSlot.ucs.axis_z = -pTube->ucs.axis_z;
		normalize(xNailSlot.ucs.axis_z);
		xNailSlot.ucs.axis_x = xNailSlot.ucs.axis_y^xNailSlot.ucs.axis_z;
		normalize(xNailSlot.ucs.axis_x);
		xNailSlot.m_iNormCalStyle=0;	//直接指定槽钢工作面法线 
		xNailSlot.SetWorkPlaneNorm(-xNailSlot.ucs.axis_y);
		//原点
		double H = xNailSlot.GetHeight();
		double W = xNailSlot.GetWidth();
		double T = xNailSlot.GetThick();
		double L = xNailSlot.feature;
		double dist = sqrt(SQR(0.5*pTube->GetDiameter())-SQR(0.5*H-T));
		origin = m_xDatumStart-xNailSlot.ucs.axis_y*(dist+W)-xNailSlot.ucs.axis_z*(L*0.5);
		Add_Pnt(xNailSlot.ucs.origin,origin, m_xDatumLenVec*ftol(m_fLenUseDist+m_nNailSpace));
		xNailSlot.SetStart(xNailSlot.ucs.origin);
		xNailSlot.SetEnd(xNailSlot.ucs.origin+xNailSlot.ucs.axis_z*L);
		//3、计算脚钉坐标系
		xNailBolt.ucs.axis_z=-xNailSlot.ucs.axis_y;
		xNailBolt.ucs.origin=m_xDatumStart+m_xDatumLenVec*(m_fLenUseDist+m_nNailSpace);
		Add_Pnt(xNailBolt.ucs.origin,xNailSlot.ucs.origin+xNailSlot.ucs.axis_z*L*0.5,xNailSlot.ucs.axis_y*T);
		if(m_bDualSide)
		{
			xNailSlot2.CopyProperty(&xNailSlot);
			xNailBolt2.CopyProperty(&xNailBolt);
			xNailSlot2.ucs.axis_y=-pTube->ucs.axis_x;	
			double rot_angle = pTube->CalWeldRoadAngle();
			if(m_iDatumLine==1)//以基准边为基准还是以焊道线为基准
				RotateVectorAroundVector(xNailSlot2.ucs.axis_y,rot_angle,axis_z);
			double cur_angle=0;
			m_fCurNailAngle2=m_fNailAngle;
			if(fabs(m_fCurNailAngle-m_fNailAngle)<EPS)
				m_fCurNailAngle2=m_fNailAngle+m_fNailAngle2;
			cur_angle=m_fCurNailAngle2*RADTODEG_COEF;
			RotateVectorAroundVector(xNailSlot2.ucs.axis_y,cur_angle,axis_z);
			xNailSlot2.ucs.axis_z = -pTube->ucs.axis_z;
			normalize(xNailSlot2.ucs.axis_z);
			xNailSlot2.ucs.axis_x = xNailSlot2.ucs.axis_y^xNailSlot2.ucs.axis_z;
			normalize(xNailSlot2.ucs.axis_x);
			xNailSlot2.m_iNormCalStyle=0;	//直接指定槽钢工作面法线 
			xNailSlot2.SetWorkPlaneNorm(-xNailSlot2.ucs.axis_y);
			//原点
			double H = xNailSlot.GetHeight();
			double W = xNailSlot.GetWidth();
			double T = xNailSlot.GetThick();
			double L = xNailSlot.feature;
			double dist = sqrt(SQR(0.5*pTube->GetDiameter())-SQR(0.5*H-T));
			origin = m_xDatumStart-xNailSlot2.ucs.axis_y*(dist+W)-xNailSlot2.ucs.axis_z*(L*0.5);
			Add_Pnt(xNailSlot2.ucs.origin,origin, m_xDatumLenVec*ftol(m_fLenUseDist+m_nNailSpace));
			xNailSlot2.SetStart(xNailSlot2.ucs.origin);
			xNailSlot2.SetEnd(xNailSlot2.ucs.origin+xNailSlot2.ucs.axis_z*L);
			//3、计算脚钉坐标系
			xNailBolt2.ucs.axis_z=-xNailSlot2.ucs.axis_y;
			xNailBolt2.ucs.origin=m_xDatumStart+m_xDatumLenVec*(m_fLenUseDist+m_nNailSpace);
			Add_Pnt(xNailBolt2.ucs.origin,xNailSlot2.ucs.origin+xNailSlot2.ucs.axis_z*L*0.5,xNailSlot2.ucs.axis_y*T);
		}
	}
	else if(pageOnTube.m_iNailType==1)	//角钢式脚钉
	{	//2、初始化角钢脚钉的外形参数
		CLDSLineTube* pTube=m_pRod.LineTubePointer();
		CLDSLineAngle *pStandardFootNailAngle = (CLDSLineAngle*)console.FromPartHandle(hFirstLineSlot,CLS_LINEANGLE);
		restore_Ls_guige(m_sNailGuiGe,xNailBolt);
		if(pStandardFootNailAngle)
		{
			xNailAngle.CopyProperty(pStandardFootNailAngle);
			xNailAngle.feature=pStandardFootNailAngle->feature;	//脚钉的长度
		}
		else if(bNewCreate)
		{	//设置脚钉板参数
			CDesignFootNailPlateDlg FootNailPlateDlg;
			//FootNailPlateDlg.m_iBoltD = xNailBolt.get_d();
			FootNailPlateDlg.m_pLineTube = pTube;
			FootNailPlateDlg.part_type=1;	//槽钢底座式脚钉
			if(FootNailPlateDlg.DoModal()!=IDOK)
				return;
			xNailAngle.size_wide = FootNailPlateDlg.m_fParamW;
			xNailAngle.feature = (long)FootNailPlateDlg.m_fParamL;//存储脚钉长度
			xNailAngle.size_height = FootNailPlateDlg.m_fParamH;
			xNailAngle.size_thick = FootNailPlateDlg.m_fParamT;
			xNailAngle.cMaterial = CSteelMatLibrary::RecordAt(FootNailPlateDlg.m_iMaterial).cBriefMark;
			xNailAngle.iSeg = SEGI(FootNailPlateDlg.m_sSegNo.GetBuffer());
			xNailAngle.SetPartNo(FootNailPlateDlg.m_sPartNo.GetBuffer());
			//strcpy(xNailAngle.spec,FootNailPlateDlg.m_sGuiGe);//槽钢规格 
		}
		//2、计算角钢的坐标系
		xNailAngle.set_norm_y_wing(-pTube->ucs.axis_z);
		xNailAngle.ucs.axis_z=pTube->ucs.axis_x;	
		f3dPoint origin,axis_z=pTube->End()-pTube->Start();
		double rot_angle = pTube->CalWeldRoadAngle();
		if(m_iDatumLine==1)//以基准边为基准还是以焊道线为基准
			RotateVectorAroundVector(xNailAngle.ucs.axis_z,rot_angle,axis_z);
		double cur_angle=m_fCurNailAngle*RADTODEG_COEF;
		RotateVectorAroundVector(xNailAngle.ucs.axis_z,cur_angle,axis_z);
		//原点
		normalize(xNailAngle.ucs.axis_z);
		origin=m_xDatumStart+xNailAngle.ucs.axis_z*(0.5*pTube->GetDiameter());
		origin+=m_xDatumLenVec*ftol(m_fLenUseDist+m_nNailSpace);
		GEPOINT xAngleStart=origin;
		GEPOINT xAngleEnd=origin+xNailAngle.ucs.axis_z*xNailAngle.feature;
		xNailAngle.SetStart(xAngleStart);
		xNailAngle.SetEnd(xAngleEnd);
		xNailAngle.des_norm_x.bByOtherWing=TRUE;
		xNailAngle.cal_x_wing_norm();
		xAngleStart+=(xNailAngle.vxWingNorm+xNailAngle.vyWingNorm)*(xNailAngle.size_wide*0.5);
		xAngleEnd  +=(xNailAngle.vxWingNorm+xNailAngle.vyWingNorm)*(xNailAngle.size_wide*0.5);
	}
}
BOOL CFootnailSpaceDlg::OnInitDialog() 
{
	if(m_pRod.IsNULL())
		return FALSE;
	xNailBolt.m_hFamily=CLsLibrary::GetFootNailFamily();//CLsFamily::N
	xNailBolt.SetBelongModel(m_pRod->BelongModel());
	xNailBolt2.m_hFamily=CLsLibrary::GetFootNailFamily();
	xNailBolt2.SetBelongModel(m_pRod->BelongModel());
	//周远征说他们不习惯输入默认脚钉间距功能，因此设定为０时，相当于关闭默认间距功能　wjh-2017.11.18
	if(m_nDefaultNailSpace>0)
		m_nNailSpace = m_nDefaultNailSpace;// 400;
	CDialog::OnInitDialog();
	//移动子窗口到合适区域
	RECT rcPanel;//=workpanelRc;
	GetDlgItem(IDC_SUB_NAIL_PANEL)->GetWindowRect(&rcPanel);
	ScreenToClient(&rcPanel);
	rcPanel.top-=2;
	rcPanel.bottom-=2;
	pageOnAngle.Create(IDD_NAIL_RESIDE_ANGLE_PAGE,this);
	pageOnTube.Create(IDD_NAIL_RESIDE_TUBE_PAGE,this);
	pageOnAngle.MoveWindow(&rcPanel);
	pageOnTube.MoveWindow(&rcPanel);
	if(m_iLineType==0)
	{	//角钢上的脚钉
		JGZJ jgzj;
		getjgzj(jgzj,wing_wide);
		if(!m_bEnableTeG)
			pageOnAngle.xWingXZhunJu=pageOnAngle.xWingYZhunJu=jgzj;
		else
		{
			pageOnAngle.xWingXZhunJu=xWingXZhunJu;
			pageOnAngle.xWingYZhunJu=xWingYZhunJu;
		}
	}
	m_pMergeBolt=NULL;
	restore_Ls_guige(m_sNailGuiGe,xNailBolt);
	if(m_bInernalStart&&m_hPickObj>0)
		m_pMergeBolt=(CLDSBolt*)Ta.Parts.FromHandle(m_hPickObj,CLS_BOLT);
	else
	{
		pageOnAngle.OnCbnSelchangeCmbGType();
		if(!m_bInernalStart&&m_bSearchReplaceBolt)
			m_pMergeBolt=SearchReplaceBolt(420,false);
	}
	RefreshDlgCtrlState();
	AddNailGuigeRecord();
	GetDlgItem(IDC_S_REPLACER_BOLT)->SetWindowText("0X0");
	if(m_pMergeBolt)
	{
		xNailBolt.ucs.origin=m_pMergeBolt->ucs.origin;
		xNailBolt.ucs.axis_z=m_pMergeBolt->ucs.axis_z;
		xNailBolt.set_d(m_pMergeBolt->get_d());
		xNailBolt.SetL0ThickPara(*m_pMergeBolt->GetL0ThickPara());
		f3dPoint inters;
		//根据代孔螺栓计算出的脚钉间距应加上前一杆件的剩余长度 wht 15-08-13
		if(m_pRod->GetBoltIntersPos(m_pMergeBolt,inters))	
			m_nNailSpace=ftoi(m_xDatumLenVec*(inters-m_xDatumStart)-m_fLenUseDist+m_fPreRodRemainingLen);
		GetDlgItem(IDC_S_REPLACER_BOLT)->SetWindowText(CXhChar16("0X%X",m_pMergeBolt->handle));
		UpdateData(FALSE);
	}
	else if(m_pRod->IsAngle())//GetClassTypeId()==CLS_LINEANGLE)
		InitLineAngleFootNail();
	else //if(m_pRod->IsTube())
		InitLineTubeFootNail(!m_bInernalStart);
	xNailBolt.CalGuigeAuto();
	PreviewNailBolt(!m_bInernalStart);	//内部启动时尽量保证原缩放比例不变
	m_bInernalStart=FALSE;
	//移动窗口到合适位置
	CRect rect;
	CWnd::GetWindowRect(rect);
	int width = rect.Width();
	int height=rect.Height();
	rect.left=nScrLocationX;
	rect.top=nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen=true;
	return TRUE;  
}

int CFootnailSpaceDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bEnableWindowMoveListen=false;

	return 0;
}

void CFootnailSpaceDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
void CFootnailSpaceDlg::AddNailGuigeRecord()
{
	CComboBox* pNailGuigeCmb=(CComboBox*)GetDlgItem(IDC_CMB_FOOT_NAIL_GUIGE);
	CLsFamily *pLsFamily=CLsLibrary::FromHandle(xNailBolt.m_hFamily);
	for(int i=0;i<pLsFamily->GetCount();i++)	
		pNailGuigeCmb->AddString(pLsFamily->RecordAt(i)->guige);
	if(m_sNailGuiGe.GetLength()==0)
	{
		pNailGuigeCmb->SetCurSel(0);
		m_sNailGuiGe=pLsFamily->RecordAt(0)->guige;
	}
}

void CFootnailSpaceDlg::RefreshDlgCtrlState()
{
	if(m_iLineType==1)
	{
		pageOnAngle.ShowWindow(SW_HIDE);
		pageOnTube.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHK_DUAL_SIDE)->EnableWindow(pageOnTube.m_iNailType==0);
		GetDlgItem(IDC_E_NAIL_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_NAIL_ANGLE2)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_CUR_NAIL_ANGLE)->EnableWindow(!m_bHorizonSlotNail);
		GetDlgItem(IDC_E_NAIL_ANGLE)->EnableWindow(!m_bHorizonSlotNail);
		GetDlgItem(IDC_E_NAIL_ANGLE2)->EnableWindow(!m_bHorizonSlotNail);
		GetDlgItem(IDC_RDO_DATUM_LINE2)->EnableWindow(m_bHasWeldRoad);
		GetDlgItem(IDC_CHK_SEARCH_REPLACE_BOLT)->EnableWindow(FALSE);
		GetDlgItem(IDC_S_REPLACER_BOLT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_HORIZON_NAIL)->ShowWindow(SW_SHOW);
	}
	else if(m_iLineType==0)
	{
		pageOnAngle.ShowWindow(SW_SHOW);
		pageOnTube.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_CHK_DUAL_SIDE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHK_SEARCH_REPLACE_BOLT)->EnableWindow(TRUE);
		GetDlgItem(IDC_S_REPLACER_BOLT)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_NAIL_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_NAIL_ANGLE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_CUR_NAIL_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_DATUM_LINE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RDO_DATUM_LINE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_HORIZON_NAIL)->ShowWindow(SW_HIDE);
	}
	UpdateData(FALSE);
}

void CFootnailSpaceDlg::OnChkShiftWing() 
{
	UpdateData();
	//if(m_bShiftWing)
	UpdateData(FALSE);
}

void CFootnailSpaceDlg::OnChkDualSide() 
{
	OnChangeENailSpace();
}

void CFootnailSpaceDlg::OnChangeNailAngle() 
{
	if(m_bHorizonSlotNail&&m_iLineType==1&&m_pRod->IsTube())
		return;
	UpdateData();
	if(pageOnTube.m_iLaySide)
		m_fCurNailAngle = m_fNailAngle + m_fNailAngle2;
	else
		m_fCurNailAngle = m_fNailAngle;
	//
	if(!m_bSearchReplaceBolt||SearchReplaceBolt(m_nNailSpace)==NULL)
	{
		m_pMergeBolt=NULL;
		GetDlgItem(IDC_S_REPLACER_BOLT)->SetWindowText("0X0");
		InitLineTubeFootNail();
		PreviewNailBolt();
	}
	UpdateData(FALSE);
}

void CFootnailSpaceDlg::OnChangeNailAngle2()
{
	if(m_bHorizonSlotNail&&m_iLineType==1&&m_pRod->IsTube())
		return;
	UpdateData();
	if(pageOnTube.m_iLaySide)
		m_fCurNailAngle = m_fNailAngle + m_fNailAngle2;
	else
		m_fCurNailAngle = m_fNailAngle;
	if(!m_bSearchReplaceBolt||SearchReplaceBolt(m_nNailSpace)==NULL)
	{
		m_pMergeBolt=NULL;
		GetDlgItem(IDC_S_REPLACER_BOLT)->SetWindowText("0X0");
		InitLineTubeFootNail();
		PreviewNailBolt();
	}
	UpdateData(FALSE);

}

void CFootnailSpaceDlg::OnChangeCurNailAngle()
{
	UpdateData();
	if(m_iLineType==1)
	{
		if(fabs(m_fCurNailAngle-m_fNailAngle-m_fNailAngle2)<1)
			pageOnTube.m_iLaySide=TRUE;
		else
			pageOnTube.m_iLaySide=FALSE;
	}
	if(!m_bSearchReplaceBolt||SearchReplaceBolt(m_nNailSpace)==NULL)
	{
		m_pMergeBolt=NULL;
		GetDlgItem(IDC_S_REPLACER_BOLT)->SetWindowText("0X0");
		InitLineTubeFootNail();
		PreviewNailBolt();
	}
}

void CFootnailSpaceDlg::OnRdoDatumLine()
{
	UpdateData();
	InitLineTubeFootNail();
	PreviewNailBolt();
}

void CFootnailSpaceDlg::OnBnPickReplaceNailbolt()
{
	SelectObject(CLS_BOLT);
	//m_bPauseBreakExit=TRUE;
	//m_bInernalStart=TRUE;
	//CDialog::OnOK();
}

void CFootnailSpaceDlg::OnDestroy()
{
	CCallBackDialog::OnDestroy();

	if(!m_bInernalStart)
	{
		g_pSolidDraw->DelEnt(xNailBolt.handle);
		g_pSolidDraw->DelEnt(xNailSlot.handle);
		g_pSolidDraw->DelEnt(xNailBolt2.handle);
		g_pSolidDraw->DelEnt(xNailSlot2.handle);
	}
}

void CFootnailSpaceDlg::PreviewNailBolt(BOOL bReFocus/*=TRUE*/)
{
	if(m_iLineType==0)//||pageOnTube.m_iNailType==0)	//钢管
	{
		xNailBolt.Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(xNailBolt.GetSolidPartObject());
		g_pSolidSet->SetRotOrg(xNailBolt.ucs.origin);
	}
	else if(m_iLineType==1&&pageOnTube.m_iNailType==0)	//槽钢
	{
		xNailBolt.Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(xNailBolt.GetSolidPartObject());
		xNailSlot.Create3dSolidModel(g_sysPara.bDisplayAllHole);
		g_pSolidDraw->NewSolidPart(xNailSlot.GetSolidPartObject());
		if(m_bDualSide)
		{
			xNailBolt2.Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(xNailBolt2.GetSolidPartObject());
			xNailSlot2.Create3dSolidModel(g_sysPara.bDisplayAllHole);
			g_pSolidDraw->NewSolidPart(xNailSlot2.GetSolidPartObject());
		}
		else
		{
			g_pSolidDraw->DelEnt(xNailBolt2.handle);
			g_pSolidDraw->DelEnt(xNailSlot2.handle);
		}
	}
	else if(m_iLineType==1&&pageOnTube.m_iNailType==1)	//角钢
	{
		g_pSolidDraw->DelEnt(xNailBolt.handle);
		g_pSolidDraw->DelEnt(xNailBolt2.handle);
		g_pSolidDraw->DelEnt(xNailSlot2.handle);
		xNailAngle.Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(xNailAngle.GetSolidPartObject());
	}
	//设置选中状态
	g_pSolidDraw->ReleaseSnapStatus();
	if(m_iLineType==0)
		g_pSolidDraw->SetEntSnapStatus(xNailBolt.handle);
	else if(m_iLineType==1)
	{
		if(pageOnTube.m_iNailType==0)
		{
			g_pSolidDraw->SetEntSnapStatus(xNailBolt.handle);
			g_pSolidDraw->SetEntSnapStatus(xNailSlot.handle);
		}
		else
			g_pSolidDraw->SetEntSnapStatus(xNailAngle.handle);
	}
	g_pSolidDraw->Draw();
	if(bReFocus)
	{	//将脚钉位置放大显示
		f3dPoint origin=xNailBolt.ucs.origin;
		SCOPE_STRU scope;
		//TODO:这里放大显示位置有问题
		if(m_iLineType==1&&pageOnTube.m_iNailType==1)
			origin=xNailAngle.ucs.origin;
		scope.VerifyVertex(origin);
		scope.fMinX-=300;
		scope.fMaxX+=300;
		scope.fMinY-=300;
		scope.fMaxY+=300;
		scope.fMinZ-=300;
		scope.fMaxZ+=300;
		m_fFocusZoomCoef=1.0/g_pSolidOper->GetScaleUserToScreen();
		g_pSolidOper->FocusTo(scope,m_fFocusZoomCoef);
	}
}

void CFootnailSpaceDlg::OnChangeENailSpace()
{
	int oldSpace=m_nNailSpace;
	BOOL bOldDualSide=m_bDualSide;
	//int iOldWing=(m_iLineType==0)?pageOnAngle.m_iLayWing:pageOnTube.m_iLaySide;
	UpdateData();
	bool bNailTypeModified=(m_iLineType==1)?pageOnTube.m_iNailType!=m_iOldNailType:false;
	if((oldSpace-m_nNailSpace==0) && (m_iOldLayWing==m_iLayWing) && !bNailTypeModified&&bOldDualSide==m_bDualSide)
		return;
	m_iOldNailType=pageOnTube.m_iNailType;
	m_iOldLayWing=(m_iLineType==1)?pageOnTube.m_iLaySide:pageOnAngle.m_iLayWing;
	if(!m_bSearchReplaceBolt||m_nNailSpace<190||SearchReplaceBolt(m_nNailSpace)==NULL)
	{	//不允许自动代孔或未找到代孔螺栓时，清空原有代孔螺栓和界面显示状态
		m_pMergeBolt=NULL;
		GetDlgItem(IDC_S_REPLACER_BOLT)->SetWindowText("0X0");
		//计算脚钉位置时脚钉间距值应减去前一杆件的剩余长度
		if(m_iLineType==0)
			InitLineAngleFootNail();
		else
			InitLineTubeFootNail(bNailTypeModified);
	}
	PreviewNailBolt();
}

void CFootnailSpaceDlg::OnEnChangeDefaultNailSpace()
{
	UpdateData();
	GetDlgItem(IDC_E_NAIL_SPACE)->SetWindowText(CXhChar16("%d",m_nDefaultNailSpace));
}

void CFootnailSpaceDlg::FireResideAngleDist()
{
	//计算脚钉位置时脚钉间距值应减去前一杆件的剩余长度
	if(m_iLineType!=0)
		return;
	InitLineAngleFootNail();
	PreviewNailBolt();
}

void CFootnailSpaceDlg::OnOK()
{
	if(m_iLineType==0)
		pageOnAngle.UpdateData();
	else
		pageOnTube.UpdateData();
	CDialog::OnOK();
}

void CFootnailSpaceDlg::FireRdoLayWing(int iOldLayWing)
{
	m_iOldLayWing=iOldLayWing;
	m_fCurNailAngle=(m_iLayWing==0)?m_fNailAngle:m_fNailAngle+m_fNailAngle2;
	GetDlgItem(IDC_E_CUR_NAIL_ANGLE)->SetWindowText(CXhChar16("%.2f",m_fCurNailAngle));
	OnChangeENailSpace();
	//m_iLayWing=m_iLineType==0?pageOnAngle.m_iLayWing:pageOnTube.m_iLaySide;
}

void CFootnailSpaceDlg::FireRdoNailType(int iOldNailType)
{
	m_iOldNailType=iOldNailType;
	GetDlgItem(IDC_CHK_DUAL_SIDE)->EnableWindow(pageOnTube.m_iNailType==0);
	OnChangeENailSpace();
}


void CFootnailSpaceDlg::OnClickedChkSearchReplaceBolt()
{
	UpdateData();
	if(m_bSearchReplaceBolt&&SearchReplaceBolt(m_nNailSpace))
		PreviewNailBolt();
}

void CFootnailSpaceDlg::OnChkHorizonNail()
{
	UpdateData();
	GetDlgItem(IDC_E_CUR_NAIL_ANGLE)->EnableWindow(!m_bHorizonSlotNail);
	GetDlgItem(IDC_E_NAIL_ANGLE)->EnableWindow(!m_bHorizonSlotNail);
	GetDlgItem(IDC_E_NAIL_ANGLE2)->EnableWindow(!m_bHorizonSlotNail);
	m_fNailAngle=0;
	m_fNailAngle2=120;
	if(m_bHorizonSlotNail&&m_iLineType==1&&m_pRod->IsTube())
	{
		CLDSLineTube* pTube=m_pRod.LineTubePointer();
		pTube->BuildUCS();
		GEPOINT edgeVec=pTube->ucs.axis_x;
		double rotAngle=m_fCurNailAngle*RADTODEG_COEF;
		if(m_iDatumLine==1)	//焊道边
			rotAngle+=pTube->CalWeldRoadAngle();
		RotateVectorAroundVector(edgeVec,rotAngle,pTube->ucs.axis_z);
		GEPOINT horiVec=f3dPoint(0,0,1)^pTube->ucs.axis_z;
		normalize(horiVec);
		if(horiVec*edgeVec<0)
			horiVec*=-1.0;
		vector_trans(horiVec,pTube->ucs,FALSE,TRUE);
		rotAngle=Cal2dLineAng(0,0,horiVec.x,horiVec.y);
		if(m_iDatumLine==1)	//焊道边
			rotAngle-=pTube->CalWeldRoadAngle();
		m_fCurNailAngle=rotAngle*DEGTORAD_COEF;
		m_fNailAngle=m_fCurNailAngle;
		m_fNailAngle2=180;
		InitLineTubeFootNail();
		PreviewNailBolt();
	}
	UpdateData(FALSE);
}

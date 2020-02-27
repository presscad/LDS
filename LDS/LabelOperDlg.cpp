// LabelOperDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "LabelOperDlg.h"
#include "afxdialogex.h"
#include "Query.h"
#include "DataCompare.h"
#include "LocalFeatureDef.h"
#include "env_def.h"
#include "MainFrm.h"

// CLabelOperDlg 对话框

IMPLEMENT_DYNAMIC(CLabelOperDlg, CDialog)
static BOOL _localSyncRotationParts=true;
CLabelOperDlg::CLabelOperDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CLabelOperDlg::IDD, pParent)
	, m_sSizeSpec(_T(""))
	, m_iMaterial(0)
	, m_nWorkWingG(0)
	, m_nIdleWingG(0)
{
	m_indexCurrOperPart=0;
	m_sOriginPartNo = _T("");
	m_sMirXOriginPartNo = _T("");
	m_sMirYOriginPartNo = _T("");
	m_sMirZOriginPartNo = _T("");
	m_sMirRotPartNo = _T("");
	m_sMirXnRotPartNo = _T("");
	m_sMirYnRotPartNo = _T("");
	m_sMirZnRotPartNo = _T("");
	m_sSegNo = _T("");
	m_pCurrPart=m_pMirXPart=m_pMirYPart=m_pMirZPart=NULL;
	m_pMirRotPart=m_pMirXnRotPart=m_pMirYnRotPart=m_pMirZnRotPart=NULL;
	m_bSyncPartSizeMat=TRUE;
	m_bSyncRotationParts=_localSyncRotationParts;
	m_ciErrorMsgMode=0;
	m_ciWorkWing=m_ciIdleWing=0;
	m_cQualityLevel=0;
}

CLabelOperDlg::~CLabelOperDlg()
{
}

void CLabelOperDlg::_InternalSetCurrPart(CLDSPart* pPart)
{
	m_pCurrPart=pPart;
	m_pMirXPart=pPart->GetMirPart(MIRMSG(1));
	m_pMirYPart=pPart->GetMirPart(MIRMSG(2));
	m_pMirZPart=pPart->GetMirPart(MIRMSG(3));
	if(m_pMirZPart==m_pMirXPart||m_pMirZPart==m_pMirYPart)
		m_pMirZPart=NULL;
	if(m_pMirXPart==m_pCurrPart)
		m_pMirXPart=NULL;	//自身X轴对称
	if(m_pMirYPart==m_pCurrPart)
		m_pMirYPart=NULL;	//自身Y轴对称
	if(m_pMirZPart==m_pCurrPart)
		m_pMirZPart=NULL;	//自身Z轴对称
	//更新件号信息
	m_sOriginPartNo=m_pCurrPart->Label;
	if(m_pMirXPart)
		m_sMirXOriginPartNo=m_pMirXPart->Label;
	if(m_pMirYPart)
		m_sMirYOriginPartNo=m_pMirYPart->Label;
	if(m_pMirZPart)
		m_sMirZOriginPartNo=m_pMirZPart->Label;
	m_sSegNo=m_pCurrPart->iSeg.ToString();
	//更新规格及材质信息
	if(m_pCurrPart->IsAngle())
	{
		int iStartAnchorWing,iEndAnchorWing;
		iStartAnchorWing=m_pCurrPart.pAngle->ciStartAnchorWing;
		if(iStartAnchorWing==1||iStartAnchorWing==2)
			m_ciWorkWing='W'+iStartAnchorWing;
		else
		{
			iEndAnchorWing=m_pCurrPart.pAngle->ciEndAnchorWing;
			if(iEndAnchorWing==1||iEndAnchorWing==2)
				m_ciWorkWing='W'+iEndAnchorWing;
			else
				m_ciWorkWing=0;
		}
		if(m_ciWorkWing==0&&iStartAnchorWing==3)
			m_ciWorkWing='A';
		else if(m_ciWorkWing==0&&iEndAnchorWing==3)
			m_ciWorkWing='A';
		if(m_ciWorkWing=='X'||m_ciWorkWing=='Y')
			m_ciIdleWing='Y'-m_ciWorkWing+'X';
		else
			m_ciIdleWing=0;
		if(m_ciWorkWing=='X')
			m_ctrlAnchorWingStyle.SetWindowText("X肢定位");
		else if(m_ciWorkWing=='Y')
			m_ctrlAnchorWingStyle.SetWindowText("Y肢定位");
		else if(m_ciWorkWing=='A')
			m_ctrlAnchorWingStyle.SetWindowText("双肢定位");
		else
			m_ctrlAnchorWingStyle.SetWindowText("智能定位");
		JGZJ jgzjX=m_pCurrPart.pAngle->GetZhunJu('X');
		JGZJ jgzjY=m_pCurrPart.pAngle->GetZhunJu('Y');
		if(m_ciWorkWing!='X'&&m_ciWorkWing!='Y')
		{
			GetDlgItem(IDC_S_PRIOR_WING_G)->SetWindowText("X肢心距：");
			GetDlgItem(IDC_S_SLAVE_WING_G)->SetWindowText("Y肢心距：");
			m_nWorkWingG=jgzjX.g;
			m_nIdleWingG=jgzjY.g;
		}
		else
		{
			GetDlgItem(IDC_S_PRIOR_WING_G)->SetWindowText("工作肢心距：");
			GetDlgItem(IDC_S_SLAVE_WING_G)->SetWindowText("另一肢心距：");
			m_nWorkWingG=m_ciWorkWing=='X'?jgzjX.g:jgzjY.g;
			m_nIdleWingG=m_ciWorkWing=='X'?jgzjY.g:jgzjX.g;;
		}

		CXhChar16 sizespec("%gX%g",m_pCurrPart->GetWidth(),m_pCurrPart->GetThick());
		m_sSizeSpec=(char*)sizespec;
		m_iMaterial=CSteelMatLibrary::GetMatIndex(m_pCurrPart->cMaterial);
	}
	else if(m_pCurrPart->IsTube())
	{
		CXhChar16 sizespec("%gX%g",m_pCurrPart->GetWidth(),m_pCurrPart->GetThick());
		m_sSizeSpec=(char*)sizespec;
		m_iMaterial=CSteelMatLibrary::GetMatIndex(m_pCurrPart->cMaterial);
	}
	else
		m_ciWorkWing=m_ciIdleWing=0;
	SmartPartPtr pSmartPart=pPart;
	MIRMSG xRotMir;
	xRotMir.axis_flag=0x08;
	m_pMirRotPart=pPart->GetMirPart(xRotMir);
	m_pMirRotPart  =pPart!=NULL?pPart->GetMirPart(xRotMir):NULL;
	m_pMirXnRotPart=m_pMirXPart!=NULL?m_pMirXPart->GetMirPart(xRotMir):NULL;
	m_pMirYnRotPart=m_pMirYPart!=NULL?m_pMirYPart->GetMirPart(xRotMir):NULL;
	m_pMirZnRotPart=m_pMirZPart!=NULL?m_pMirZPart->GetMirPart(xRotMir):NULL;
	if(m_pMirRotPart==m_pCurrPart||m_pMirRotPart==m_pMirXPart||m_pMirRotPart==m_pMirYPart||m_pMirRotPart==m_pMirZPart)
		m_pMirRotPart=NULL;
	if(m_pMirXnRotPart==m_pCurrPart||m_pMirXnRotPart==m_pMirXPart||m_pMirXnRotPart==m_pMirYPart||m_pMirXnRotPart==m_pMirZPart)
		m_pMirXnRotPart=NULL;
	if(m_pMirYnRotPart==m_pCurrPart||m_pMirYnRotPart==m_pMirXPart||m_pMirYnRotPart==m_pMirYPart||m_pMirYnRotPart==m_pMirZPart)
		m_pMirYnRotPart=NULL;
	if(m_pMirZnRotPart==m_pCurrPart||m_pMirZnRotPart==m_pMirXPart||m_pMirZnRotPart==m_pMirYPart||m_pMirZnRotPart==m_pMirZPart)
		m_pMirZnRotPart=NULL;
	//更新旋转侧面件号信息
	m_sMirRotPartNo  =m_pMirRotPart  !=NULL?m_pMirRotPart->Label:"";
	m_sMirXnRotPartNo=m_pMirXnRotPart!=NULL?m_pMirXnRotPart->Label:"";
	m_sMirYnRotPartNo=m_pMirYnRotPart!=NULL?m_pMirYnRotPart->Label:"";
	m_sMirZnRotPartNo=m_pMirZnRotPart!=NULL?m_pMirZnRotPart->Label:"";
}
#include "SortFunc.h"
#include "GlobalFunc.h"
static bool IsBelongToPanel(CLDSPart* pPart,char cPanelSymbol)
{
	if(pPart==NULL)
		return false;
	if(cPanelSymbol=='Q'&&(pPart->Layer(2)=='Q'||pPart->Layer(2)=='1'||pPart->Layer(2)=='2'))
		return true;
	else if(cPanelSymbol=='H'&&(pPart->Layer(2)=='H'||pPart->Layer(2)=='3'||pPart->Layer(2)=='4'))
		return true;
	else if(cPanelSymbol=='Z'&&(pPart->Layer(2)=='Z'||pPart->Layer(2)=='2'||pPart->Layer(2)=='4'))
		return true;
	else if(cPanelSymbol=='Y'&&(pPart->Layer(2)=='Y'||pPart->Layer(2)=='1'||pPart->Layer(2)=='3'))
		return true;
	else if(cPanelSymbol==pPart->Layer(2))
		return true;
	else
		return false;
}
static GEPOINT RodStart(CLDSLinePart* pRod,bool bPreferFrameLinePosition=true)
{
	if(bPreferFrameLinePosition&&pRod->pStart!=NULL)
		return pRod->pStart->xPreliftPos;
	else
		return pRod->Start();
}
static GEPOINT RodEnd(CLDSLinePart* pRod,bool bPreferFrameLinePosition=true)
{
	if(bPreferFrameLinePosition&&pRod->pEnd!=NULL)
		return pRod->pEnd->xPreliftPos;
	else
		return pRod->End();
}
static int _LocalCompareParts(const CLDSPartPtr& pPart1,const CLDSPartPtr& pPart2)
{
	if(pPart1->iSeg>pPart2->iSeg)
		return -1;	//大段号一般在下侧，优先录入编号
	else if(pPart1->iSeg<pPart2->iSeg)
		return 1;
	SmartPartPtr pSmartPart1=pPart1;
	SmartPartPtr pSmartPart2=pPart2;
	BYTEFLAG itemflag1=0,itemflag2=0;
	if(!pSmartPart1->IsLinePart())
		itemflag1.SetBitState(7);	//非杆件（如钢板）
	else
	{
		if(pSmartPart1.pRod->IsAuxPole())
			itemflag1.SetBitState(6);	//辅材
		else if(!pSmartPart1.pRod->IsPostRod())//&&pSmartPart1.pRod->IsPrimaryRod())
			itemflag1.SetBitState(5);	//斜材
	}
	if(pPart1->Layer(0)=='T')
		itemflag1.SetBitState(4);	//头部构件
	if(!IsBelongToPanel(pPart1,'Q')&&!IsBelongToPanel(pPart1,'H')&&!IsBelongToPanel(pPart1,'Z')&&!IsBelongToPanel(pPart1,'Y'))
		itemflag1.SetBitState(3);	//隔面
	else if(IsBelongToPanel(pPart1,'Q')||IsBelongToPanel(pPart1,'H'))
		itemflag1.SetBitState(2,false);	//前面
	else
		itemflag1.SetBitState(2);	//侧面
	if(!pSmartPart2->IsLinePart())
		itemflag2.SetBitState(7);	//非杆件（如钢板）
	else
	{
		if(pSmartPart2.pRod->IsAuxPole())
			itemflag2.SetBitState(6);	//辅材
		else if(!pSmartPart2.pRod->IsPostRod())//IsPrimaryRod())
			itemflag2.SetBitState(5);	//斜材
	}
	if(pPart2->Layer(0)=='T')
		itemflag2.SetBitState(4);	//头部构件
	char ciFront1Side2Else0=0;
	if(!IsBelongToPanel(pPart2,'Q')&&!IsBelongToPanel(pPart2,'H')&&!IsBelongToPanel(pPart2,'Z')&&!IsBelongToPanel(pPart2,'Y'))
		itemflag2.SetBitState(3);	//隔面
	else if(IsBelongToPanel(pPart2,'Q')||IsBelongToPanel(pPart2,'H'))
		ciFront1Side2Else0=1;
	else
	{
		ciFront1Side2Else0=2;
		itemflag2.SetBitState(2);	//侧面
	}
	if((BYTE)itemflag1>(BYTE)itemflag2)
		return 1;
	else if((BYTE)itemflag1<(BYTE)itemflag2)
		return -1;
	if(pSmartPart1->IsLinePart()&&pSmartPart2->IsLinePart())
	{
		GEPOINT pick1,pick2;
		if(pSmartPart1->Layer(0)=='T')
		{	//横担（由近到远）（主斜材取极值，横辅材取中值）
			if(pSmartPart1.pRod->IsAuxPole())
				pick1=(RodStart(pSmartPart1.pRod)+RodEnd(pSmartPart1.pRod))*0.5;
			else if(fabs(RodStart(pSmartPart1.pRod).x)+fabs(RodStart(pSmartPart1.pRod).y)<
				fabs(RodEnd(pSmartPart1.pRod).x)+fabs(RodEnd(pSmartPart1.pRod).y))
				pick1=RodStart(pSmartPart1.pRod);
			else
				pick1=RodEnd(pSmartPart1.pRod);
			if(pSmartPart2.pRod->IsAuxPole())
				pick2=(RodStart(pSmartPart2.pRod)+RodEnd(pSmartPart2.pRod))*0.5;
			else if(fabs(RodStart(pSmartPart2.pRod).x)+fabs(RodStart(pSmartPart2.pRod).y)<
				fabs(RodEnd(pSmartPart2.pRod).x)+fabs(RodEnd(pSmartPart2.pRod).y))
				pick2=RodStart(pSmartPart1.pRod);
			else
				pick2=RodEnd(pSmartPart1.pRod);
			if(fabs(pick1.x)+fabs(pick1.y)<fabs(pick2.x)+fabs(pick2.y)-1)
				return -1;
			else if(fabs(pick1.x)+fabs(pick1.y)>fabs(pick2.x)+fabs(pick2.y)+1)
				return  1;
			return 0;
		}
		else
		{
			if(pSmartPart1.pRod->IsAuxPole())
				pick1=(RodStart(pSmartPart1.pRod)+RodEnd(pSmartPart1.pRod))*0.5;
			else if(RodStart(pSmartPart1.pRod).z>RodEnd(pSmartPart1.pRod).z)
				pick1=RodStart(pSmartPart1.pRod);
			else //if(
				pick1=RodEnd(pSmartPart1.pRod);
			if(pSmartPart2.pRod->IsAuxPole())
				pick2=(RodStart(pSmartPart2.pRod)+RodEnd(pSmartPart2.pRod))*0.5;
			else if(RodStart(pSmartPart1.pRod).z>RodEnd(pSmartPart2.pRod).z)
				pick2=RodStart(pSmartPart2.pRod);
			else //if(
				pick2=RodEnd(pSmartPart2.pRod);
			if(pick1.z>pick2.z+1)
				return -1;
			else if(pick1.z<pick2.z-1)
				return 1;
			else if(ciFront1Side2Else0==1)
			{	//正侧录入构件，先左后右
				if(pick1.x<pick2.x)
					return -1;
				else if(pick1.x>pick2.x)
					return 1;
				else
					return 0;
			}
			else if(ciFront1Side2Else0==2)
			{	//右侧录入构件，先左后右
				if(pick1.y<pick2.y)
					return 1;
				else if(pick1.y>pick2.y)
					return -1;
				else
					return 0;
			}
			else
				return 0;
		}
	}
	else
	{
		if(pSmartPart1->ucs.origin.z>pSmartPart2->ucs.origin.z+1)
			return -1;
		else if(pSmartPart1->ucs.origin.z<pSmartPart2->ucs.origin.z-1)
			return 1;
		else if(pSmartPart1->ucs.origin.x>pSmartPart2->ucs.origin.x+1)
			return 1;
		else if(pSmartPart1->ucs.origin.x<pSmartPart2->ucs.origin.x-1)
			return -1;
		GEPOINT pick1=pSmartPart1->ucs.origin;
		GEPOINT pick2=pSmartPart2->ucs.origin;
		if(pSmartPart1->Layer(0)=='T')
		{	//横担（由近到远）（主斜材取极值，横辅材取中值）
			if(fabs(pick1.x)+fabs(pick1.y)<fabs(pick2.x)+fabs(pick2.y)-1)
				return -1;
			else if(fabs(pick1.x)+fabs(pick1.y)>fabs(pick2.x)+fabs(pick2.y)+1)
				return  1;
			return 0;
		}
		else
		{
			if(pick1.z>pick2.z+1)
				return -1;
			else if(pick1.z<pick2.z-1)
				return 1;
			else
				return 0;
		}
	}
	return 0;
}
void CLabelOperDlg::SetPartSet(IXhSet<CLDSPartPtr>* pPartSet)
{
	hashProcessedParts.Empty();
	if(GetSafeHwnd()!=NULL)
		GetDlgItem(IDC_BTN_NEXT_LABEL_GROUP)->EnableWindow(TRUE);
	int i=0;
	xPartArr.SetSize(pPartSet->Count);
	for(CLDSPart* pPart=pPartSet->MoveFirst();pPart;pPart=pPartSet->MoveNext(),i++)
		xPartArr[i]=pPart;
	CHeapSort<CLDSPartPtr>::HeapSort(xPartArr.m_pData,xPartArr.GetSize(),_LocalCompareParts);
	m_indexCurrOperPart=0;
}
bool CLabelOperDlg::UpdateCtrlStates()
{
	if(GetSafeHwnd()==NULL)
		return false;
	GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->EnableWindow(m_pMirXPart!=NULL);
	GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->EnableWindow(m_pMirYPart!=NULL);
	GetDlgItem(IDC_E_MIR_Z_ORIGIN_PART_NO)->EnableWindow(m_pMirZPart!=NULL);
	if(m_pMirXPart==NULL)
		GetDlgItem(IDC_E_MIR_X_ORIGIN_PART_NO)->SetWindowTextA("无");
	if(m_pMirYPart==NULL)
		GetDlgItem(IDC_E_MIR_Y_ORIGIN_PART_NO)->SetWindowTextA("无");
	if(m_pMirZPart==NULL)
		GetDlgItem(IDC_E_MIR_Z_ORIGIN_PART_NO)->SetWindowTextA("无");
	GetDlgItem(IDC_E_DIAGONAL_PART_NO)->EnableWindow(m_pMirRotPart!=NULL);
	GetDlgItem(IDC_E_MIR_DIAGONAL_X_PART_NO)->EnableWindow(m_pMirXnRotPart!=NULL);
	GetDlgItem(IDC_E_MIR_DIAGONAL_Y_PART_NO)->EnableWindow(m_pMirYnRotPart!=NULL);
	GetDlgItem(IDC_E_MIR_DIAGONAL_Z_PART_NO)->EnableWindow(m_pMirZnRotPart!=NULL);
	if(m_pMirRotPart==NULL)
		GetDlgItem(IDC_E_DIAGONAL_PART_NO)->SetWindowTextA("无");
	if(m_pMirXnRotPart==NULL)
		GetDlgItem(IDC_E_MIR_DIAGONAL_X_PART_NO)->SetWindowTextA("无");
	if(m_pMirYnRotPart==NULL)
		GetDlgItem(IDC_E_MIR_DIAGONAL_Y_PART_NO)->SetWindowTextA("无");
	if(m_pMirZnRotPart==NULL)
		GetDlgItem(IDC_E_MIR_DIAGONAL_Z_PART_NO)->SetWindowTextA("无");
	return true;
}

void CLabelOperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_CMB_SIZE_SPEC, m_sSizeSpec);
	DDX_CBIndex(pDX, IDC_CMB_MATERIAL, m_iMaterial);
	DDX_Text(pDX, IDC_E_SEG_NO, m_sSegNo);
	DDX_Text(pDX, IDC_E_ORIGIN_PART_NO, m_sOriginPartNo);
	DDX_Text(pDX, IDC_E_MIR_X_ORIGIN_PART_NO, m_sMirXOriginPartNo);
	DDX_Text(pDX, IDC_E_MIR_Y_ORIGIN_PART_NO, m_sMirYOriginPartNo);
	DDX_Text(pDX, IDC_E_MIR_Z_ORIGIN_PART_NO, m_sMirZOriginPartNo);
	DDX_Text(pDX, IDC_E_DIAGONAL_PART_NO, m_sMirRotPartNo);
	DDX_Text(pDX, IDC_E_MIR_DIAGONAL_X_PART_NO, m_sMirXnRotPartNo);
	DDX_Text(pDX, IDC_E_MIR_DIAGONAL_Y_PART_NO, m_sMirYnRotPartNo);
	DDX_Text(pDX, IDC_E_MIR_DIAGONAL_Z_PART_NO, m_sMirZnRotPartNo);
	DDX_Check(pDX, IDC_CHK_AUTO_MATCH_MATERIAL, m_bSyncPartSizeMat);
	DDX_Control(pDX, IDC_S_ERROR_MSG, m_ctrlErrorMsg);
	DDX_Control(pDX, IDC_S_ANCHOR_WING_STYLE, m_ctrlAnchorWingStyle);
	DDX_Text(pDX, IDC_ANGLE_WORK_WING_G, m_nWorkWingG);
	DDX_Text(pDX, IDC_ANGLE_IDLE_WING_G, m_nIdleWingG);
	DDX_Check(pDX, IDC_CHK_SYN_ROTATION, m_bSyncRotationParts);
}


BEGIN_MESSAGE_MAP(CLabelOperDlg, CRememberLocationDlg)
	ON_BN_CLICKED(IDC_BTN_NEXT_LABEL_GROUP, &CLabelOperDlg::OnBnNextLabelGroup)
	ON_EN_CHANGE(IDC_E_ORIGIN_PART_NO, &CLabelOperDlg::OnChangeEOriginPartNo)
	ON_EN_CHANGE(IDC_E_MIR_X_ORIGIN_PART_NO, &CLabelOperDlg::OnChangeEMirXOriginPartNo)
	ON_EN_CHANGE(IDC_ANGLE_WORK_WING_G, &CLabelOperDlg::OnChangeAngleWorkWingG)
	ON_CBN_SELCHANGE(IDC_CMB_SIZE_SPEC, &CLabelOperDlg::OnCbnSelchangeCmbSizeSpec)
	ON_BN_CLICKED(IDC_CHK_SYN_ROTATION, &CLabelOperDlg::OnBnClickedChkSynRotation)
	ON_EN_CHANGE(IDC_E_DIAGONAL_PART_NO, &CLabelOperDlg::OnEnChangeMirRotPartNo)
	ON_EN_CHANGE(IDC_E_MIR_DIAGONAL_X_PART_NO, &CLabelOperDlg::OnEnChangeMirXnRotPartNo)
END_MESSAGE_MAP()


// CLabelOperDlg 消息处理程序

static void FocusRod(CLDSPart* pPart)
{
	if(pPart==NULL||!pPart->IsLinePart())
		return;
	CLDSLinePart* pRod=(CLDSLinePart*)pPart;
	SCOPE_STRU scope;
	if(pRod->pStart)
		scope.VerifyVertex(pRod->pStart->xActivePos);
	else
		scope.VerifyVertex(pRod->Start());
	if(pRod->pEnd)
		scope.VerifyVertex(pRod->pEnd->xActivePos);
	else
		scope.VerifyVertex(pRod->End());
	scope.fMinX-=300;
	scope.fMaxX+=300;
	scope.fMinY-=300;
	scope.fMaxY+=300;
	scope.fMinZ-=300;
	scope.fMaxZ+=300;
	double fFocusZoomCoef=max(0.05,1.0/g_pSolidOper->GetScaleUserToScreen());
	g_pSolidOper->FocusTo(scope,fFocusZoomCoef);
	g_pSolidDraw->Draw();	//FocusTo后必须随后调用一次Draw()否则连续调用FocusTo会导致OpenGL内部混乱,近而缩放错误 wjh-2017.12.18
}

BOOL CLabelOperDlg::OnInitDialog()
{
	if(m_pCurrPart.IsHasPtr())
	{
		this->m_sOriginPartNo=m_pCurrPart->Label;
		FocusRod(m_pCurrPart);
	}
	if(m_pMirXPart)
		this->m_sMirXOriginPartNo=m_pMirXPart->Label;
	if(m_pMirYPart)
		this->m_sMirYOriginPartNo=m_pMirYPart->Label;
	if(m_pMirZPart)
		this->m_sMirZOriginPartNo=m_pMirZPart->Label;
	if(m_pMirRotPart)
		this->m_sMirRotPartNo=m_pMirRotPart->Label;
	if(m_pMirXnRotPart)
		this->m_sMirXnRotPartNo=m_pMirXnRotPart->Label;
	if(m_pMirYnRotPart)
		this->m_sMirYnRotPartNo=m_pMirYnRotPart->Label;
	if(m_pMirZnRotPart)
		this->m_sMirZnRotPartNo=m_pMirZnRotPart->Label;
	//
	CDialog::OnInitDialog();
	CComboBox* pCMBJg = (CComboBox*)GetDlgItem(IDC_CMB_SIZE_SPEC);
	AddJgRecord(pCMBJg);
	pCMBJg->SetCurSel(0);
	CComboBox* pCMBMateial = (CComboBox*)GetDlgItem(IDC_CMB_MATERIAL);
	AddSteelMatRecord(pCMBMateial);
	if(xPartArr.GetSize()>0)
	{
		m_indexCurrOperPart=0;
		_InternalSetCurrPart(xPartArr[0]);
		FocusRod(xPartArr[0]);
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pCurrPart->handle);
		CLDSView* pView=((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
		pView->UpdatePropertyPage(m_pCurrPart);
	}
	UpdateData(FALSE);	//更新件号信息
	UpdateCtrlStates();
	m_ctrlErrorMsg.EnableURL(FALSE);
	m_ctrlErrorMsg.EnableHover(FALSE);
	m_ctrlErrorMsg.SetColours(RGB(255,0,0),RGB(255,0,0),RGB(255,0,0));
	m_ctrlErrorMsg.SetWindowText("-------------------------------------------------------");//m_sErrorMsg
	m_ctrlErrorMsg.SetURL("");	//提示信息
	m_ctrlAnchorWingStyle.EnableURL(FALSE);
	m_ctrlAnchorWingStyle.EnableHover(FALSE);
	m_ctrlAnchorWingStyle.SetColours(RGB(255,0,0),RGB(255,0,0),RGB(255,0,0));
	m_ctrlAnchorWingStyle.SetURL("");
	m_ctrlErrorMsg.SetWindowText("智能判断");
	MoveWndToLocation();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLabelOperDlg::OnBnNextLabelGroup()
{
	//将对话中输入的信息填充得到当前杆件中,并更新状态
	UpdateData();
	UpdateCurGroupPartInfo();
	UpdatePrcoessStates();
	//开始操作下一组杆件
	int i=m_indexCurrOperPart;
	for(i=m_indexCurrOperPart+1;i<xPartArr.GetSize();i++)
	{
		if(hashProcessedParts.GetValue(xPartArr[i]->handle))
			continue;	//已处理过
		m_indexCurrOperPart=i;
		break;
	}
	//GetDlgItem(IDC_BTN_NEXT_LABEL_GROUP)->EnableWindow(i<xPartArr.GetSize()-1);
	if(i==xPartArr.GetSize())
	{
		MessageBox("编号录入结束!");
		return CDialog::OnOK();
	}
	else
	{
		_InternalSetCurrPart(xPartArr[m_indexCurrOperPart]);
		FocusRod(xPartArr[m_indexCurrOperPart]);
		UpdateCtrlStates();
		UpdateData(FALSE);
		CEdit* pEdit=(CEdit*)GetDlgItem(IDC_E_ORIGIN_PART_NO);
		pEdit->SetFocus();
		int nstr=m_sOriginPartNo.GetLength();
		if(nstr>1)
			pEdit->SetSel(0,nstr);	//刘伟称出于操作习惯，没有必要跳过编号中的首个段号字符SetSel(1,nstr) wjh-2018.1.26
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pCurrPart->handle);
		CLDSView* pView=((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
		pView->UpdatePropertyPage(m_pCurrPart);
	}
}

bool CLabelOperDlg::UpdateCurGroupPartInfo()
{
	STEELMAT steel=CSteelMatLibrary::RecordAt(m_iMaterial);
	double wing_wide,wing_thick;
	restore_JG_guige(m_sSizeSpec,wing_wide,wing_thick);
	if(m_pCurrPart.IsHasPtr()&&m_pCurrPart->IsLinePart())
	{
		m_pCurrPart->SetPartNo(m_sOriginPartNo);
		m_pCurrPart->cMaterial=steel.cBriefMark;
		if (m_cQualityLevel>0)
			m_pCurrPart->cQualityLevel=m_cQualityLevel;
		m_pCurrPart->iSeg=SEGI(m_sSegNo);
		m_pCurrPart.pRod->size_wide=wing_wide;
		m_pCurrPart.pRod->size_thick=wing_thick;
		if(m_pCurrPart->IsAngle())
		{
			m_pCurrPart.pAngle->connectStart.d=SelBoltDByWingWide(wing_wide,m_pCurrPart.pAngle->connectStart.grade);
			m_pCurrPart.pAngle->connectEnd.d=SelBoltDByWingWide(wing_wide,m_pCurrPart.pAngle->connectEnd.grade);
			JGZJ stdzj=m_pCurrPart.pAngle->GetAngleZJ(m_pCurrPart->GetWidth());
			m_pCurrPart.pAngle->xWingXZhunJu=m_pCurrPart.pAngle->xWingYZhunJu=stdzj;
			if(m_ciWorkWing=='X'||m_ciWorkWing!='Y')
			{
				m_pCurrPart.pAngle->xWingXZhunJu.g=m_nWorkWingG;
				m_pCurrPart.pAngle->xWingYZhunJu.g=m_nIdleWingG;
			}
			else //if(m_ciWorkWing=='Y')
			{
				m_pCurrPart.pAngle->xWingYZhunJu.g=m_nWorkWingG;
				m_pCurrPart.pAngle->xWingXZhunJu.g=m_nIdleWingG;
			}
			if(stdzj.g!=m_nWorkWingG||stdzj.g!=m_nIdleWingG)
				m_pCurrPart.pAngle->m_bEnableTeG=TRUE;
			m_pCurrPart->SyncMirProp("m_bEnableTeG");
		}
	}
	if(m_pMirXPart&&m_pMirXPart->IsLinePart())
	{
		m_pMirXPart->SetPartNo(m_sMirXOriginPartNo);
		m_pMirXPart->cMaterial=steel.cBriefMark;
		if (m_cQualityLevel>0)
			m_pMirXPart->cQualityLevel=m_cQualityLevel;
		m_pMirXPart->iSeg=SEGI(m_sSegNo);
		((CLDSLinePart*)m_pMirXPart)->size_wide=wing_wide;
		((CLDSLinePart*)m_pMirXPart)->size_thick=wing_thick;
		if(m_pMirXPart->IsAngle())
		{
			CLDSLinePart* pJg=(CLDSLineAngle*)m_pMirXPart;
			pJg->connectStart.d=SelBoltDByWingWide(wing_wide,pJg->connectStart.grade);
			pJg->connectEnd.d=SelBoltDByWingWide(wing_wide,pJg->connectEnd.grade);
		}
	}
	if(m_pMirYPart&&m_pMirYPart->IsLinePart())
	{
		m_pMirYPart->SetPartNo(m_sMirYOriginPartNo);
		m_pMirYPart->cMaterial=steel.cBriefMark;
		if (m_cQualityLevel>0)
			m_pMirYPart->cQualityLevel=m_cQualityLevel;
		m_pMirYPart->iSeg=SEGI(m_sSegNo);
		((CLDSLinePart*)m_pMirYPart)->size_wide=wing_wide;
		((CLDSLinePart*)m_pMirYPart)->size_thick=wing_thick;
		if(m_pMirYPart->IsAngle())
		{
			CLDSLinePart* pJg=(CLDSLineAngle*)m_pMirYPart;
			pJg->connectStart.d=SelBoltDByWingWide(wing_wide,pJg->connectStart.grade);
			pJg->connectEnd.d=SelBoltDByWingWide(wing_wide,pJg->connectEnd.grade);
		}
	}
	if(m_pMirZPart&&m_pMirZPart->IsLinePart())
	{
		m_pMirZPart->SetPartNo(m_sMirZOriginPartNo);
		m_pMirZPart->cMaterial=steel.cBriefMark;
		if (m_cQualityLevel>0)
			m_pMirZPart->cQualityLevel=m_cQualityLevel;
		m_pMirZPart->iSeg=SEGI(m_sSegNo);
		((CLDSLinePart*)m_pMirZPart)->size_wide=wing_wide;
		((CLDSLinePart*)m_pMirZPart)->size_thick=wing_thick;
		if(m_pMirZPart->IsAngle())
		{
			CLDSLinePart* pJg=(CLDSLineAngle*)m_pMirZPart;
			pJg->connectStart.d=SelBoltDByWingWide(wing_wide,pJg->connectStart.grade);
			pJg->connectEnd.d=SelBoltDByWingWide(wing_wide,pJg->connectEnd.grade);
		}
	}
	if(!m_bSyncRotationParts)
		return true;
	//同步正侧面旋转对称杆件
	if(m_pMirRotPart&&m_pMirRotPart->IsLinePart())
	{
		m_pMirRotPart->SetPartNo(m_sMirRotPartNo);
		m_pMirRotPart->cMaterial=steel.cBriefMark;
		if (m_cQualityLevel>0)
			m_pMirRotPart->cQualityLevel=m_cQualityLevel;
		m_pMirRotPart->iSeg=SEGI(m_sSegNo);
		((CLDSLinePart*)m_pMirRotPart)->size_wide=wing_wide;
		((CLDSLinePart*)m_pMirRotPart)->size_thick=wing_thick;
		if(m_pMirRotPart->IsAngle())
		{
			CLDSLinePart* pJg=(CLDSLineAngle*)m_pMirRotPart;
			pJg->connectStart.d=SelBoltDByWingWide(wing_wide,pJg->connectStart.grade);
			pJg->connectEnd.d=SelBoltDByWingWide(wing_wide,pJg->connectEnd.grade);
		}
	}
	if(m_pMirXnRotPart&&m_pMirXnRotPart->IsLinePart())
	{
		m_pMirXnRotPart->SetPartNo(m_sMirXnRotPartNo);
		m_pMirXnRotPart->cMaterial=steel.cBriefMark;
		if (m_cQualityLevel>0)
			m_pMirXnRotPart->cQualityLevel=m_cQualityLevel;
		m_pMirXnRotPart->iSeg=SEGI(m_sSegNo);
		((CLDSLinePart*)m_pMirXnRotPart)->size_wide=wing_wide;
		((CLDSLinePart*)m_pMirXnRotPart)->size_thick=wing_thick;
		if(m_pMirXnRotPart->IsAngle())
		{
			CLDSLinePart* pJg=(CLDSLineAngle*)m_pMirXnRotPart;
			pJg->connectStart.d=SelBoltDByWingWide(wing_wide,pJg->connectStart.grade);
			pJg->connectEnd.d=SelBoltDByWingWide(wing_wide,pJg->connectEnd.grade);
		}
	}
	if(m_pMirYnRotPart&&m_pMirYnRotPart->IsLinePart())
	{
		m_pMirYnRotPart->SetPartNo(m_sMirYnRotPartNo);
		m_pMirYnRotPart->cMaterial=steel.cBriefMark;
		if (m_cQualityLevel>0)
			m_pMirYnRotPart->cQualityLevel=m_cQualityLevel;
		m_pMirYnRotPart->iSeg=SEGI(m_sSegNo);
		((CLDSLinePart*)m_pMirYnRotPart)->size_wide=wing_wide;
		((CLDSLinePart*)m_pMirYnRotPart)->size_thick=wing_thick;
		if(m_pMirYnRotPart->IsAngle())
		{
			CLDSLinePart* pJg=(CLDSLineAngle*)m_pMirYnRotPart;
			pJg->connectStart.d=SelBoltDByWingWide(wing_wide,pJg->connectStart.grade);
			pJg->connectEnd.d=SelBoltDByWingWide(wing_wide,pJg->connectEnd.grade);
		}
	}
	if(m_pMirZnRotPart&&m_pMirZnRotPart->IsLinePart())
	{
		m_pMirZnRotPart->SetPartNo(m_sMirZnRotPartNo);
		m_pMirZnRotPart->cMaterial=steel.cBriefMark;
		if (m_cQualityLevel>0)
			m_pMirZnRotPart->cQualityLevel=m_cQualityLevel;
		m_pMirZnRotPart->iSeg=SEGI(m_sSegNo);
		((CLDSLinePart*)m_pMirZnRotPart)->size_wide=wing_wide;
		((CLDSLinePart*)m_pMirZnRotPart)->size_thick=wing_thick;
		if(m_pMirZnRotPart->IsAngle())
		{
			CLDSLinePart* pJg=(CLDSLineAngle*)m_pMirZnRotPart;
			pJg->connectStart.d=SelBoltDByWingWide(wing_wide,pJg->connectStart.grade);
			pJg->connectEnd.d=SelBoltDByWingWide(wing_wide,pJg->connectEnd.grade);
		}
	}
	return true;
}

bool CLabelOperDlg::UpdatePrcoessStates()
{
	if(m_pCurrPart.IsHasPtr())
		hashProcessedParts.SetValue(m_pCurrPart->handle,true);
	if(m_pMirXPart)
		hashProcessedParts.SetValue(m_pMirXPart->handle,true);
	if(m_pMirYPart)
		hashProcessedParts.SetValue(m_pMirYPart->handle,true);
	if(m_pMirZPart)
		hashProcessedParts.SetValue(m_pMirZPart->handle,true);
	/*暂时不处理角对称杆件
	if(m_pMirXnRotPart)
		hashProcessedParts.SetValue(m_pMirXnRotPart->handle,true);
	if(m_pMirYnRotPart)
		hashProcessedParts.SetValue(m_pMirYnRotPart->handle,true);
	if(m_pMirZnRotPart)
		hashProcessedParts.SetValue(m_pMirZnRotPart->handle,true);
	*/
	return true;
}
void CLabelOperDlg::UpdateAngleGBySizeSpec(double width)
{
	JGZJ stdzj=CLDSLineAngle::GetAngleZJ(width);
	if(width!=this->m_pCurrPart->GetWidth()||!m_pCurrPart.pAngle->m_bEnableTeG)
	{
		m_nWorkWingG=stdzj.g;
		m_nIdleWingG=stdzj.g;
	}
	else //if(m_pCurrPart.pAngle->m_bEnableTeG)
	{
		if(m_ciWorkWing=='X'||m_ciWorkWing!='Y')
		{
			m_nWorkWingG=m_pCurrPart.pAngle->xWingXZhunJu.g;
			m_nIdleWingG=m_pCurrPart.pAngle->xWingYZhunJu.g;
		}
		else //if(m_ciWorkWing=='Y')
		{
			m_nWorkWingG=m_pCurrPart.pAngle->xWingYZhunJu.g;
			m_nIdleWingG=m_pCurrPart.pAngle->xWingXZhunJu.g;
		}
	}
}
void CLabelOperDlg::OnChangeEOriginPartNo()
{
	CXhChar16 serial_str[4];
	char temStr[16]="",matStr[16]="",digitStr[16]="";
	int i,serial[4]={0};
	//当前构件编号
	if(m_sOriginPartNo.GetLength()>0)
		ParsePartNo(m_sOriginPartNo,NULL,serial_str[0]);
	//X轴对称构件编号
	if(m_sMirXOriginPartNo.GetLength()>0)
		ParsePartNo(m_sMirXOriginPartNo,NULL,serial_str[1]);
	//Y轴对称构件编号
	if(m_sMirYOriginPartNo.GetLength()>0)
		ParsePartNo(m_sMirYOriginPartNo,NULL,serial_str[2]);
	//Z轴(对角)对称构件编号
	if(m_sMirZOriginPartNo.GetLength()>0)
		ParsePartNo(m_sMirZOriginPartNo,NULL,serial_str[3]);
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
#if defined(__TMA_)||defined(__LMA_)
	BOMPART* pPart=NULL;
	//if(::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0)
		pPart=globalDataCmpModel.GetDrawingBomPart(m_sOriginPartNo);
	if(pPart)
	{	//根据找到的BOM信息，更新该角钢的基本信息(段号、规格、材质)
		m_cQualityLevel=pPart->cQualityLevel;
		if(pPart->cPartType==BOMPART::ANGLE)
		{
			m_sSizeSpec.Format("%.0fX%.0f",pPart->wide,pPart->thick);
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			UpdateAngleGBySizeSpec(pPart->wide);
			if(m_ciErrorMsgMode!=0)
				m_ctrlErrorMsg.SetWindowText("-------------------------------------------------------");//m_sErrorMsg
			m_ciErrorMsgMode=0;
		}
		else if(pPart->cPartType==BOMPART::TUBE)
		{
			m_sSizeSpec.Format("%gX%g",pPart->wide,pPart->thick);
			m_iMaterial=CSteelMatLibrary::GetMatIndex(pPart->cMaterial);
			if(m_ciErrorMsgMode!=0)
				m_ctrlErrorMsg.SetWindowText("-------------------------------------------------------");//m_sErrorMsg
			m_ciErrorMsgMode=0;
		}
		else
		{
			m_ctrlErrorMsg.SetWindowText(CXhChar100("Bom中没有件号为#%s的角钢",(char*)pPart->sPartNo));//m_sErrorMsg
			m_ciErrorMsgMode=1;
		}
	}
	else
	{
		if(strlen(m_pCurrPart->Label)==0)
			m_ctrlErrorMsg.SetWindowText("录入新编号");//m_sErrorMsg
		else if(m_sOriginPartNo.CompareNoCase(m_pCurrPart->Label)==0)
			m_ctrlErrorMsg.SetWindowText("保持原编号");//m_sErrorMsg
		else
			m_ctrlErrorMsg.SetWindowText("更改原编号");//m_sErrorMsg
		m_ciErrorMsgMode=2;
	}
#endif
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
		m_sMirXOriginPartNo=m_sMirYOriginPartNo=m_sMirZOriginPartNo=iSeg.ToString();
		char spliter[2]={0,0};
		if(separator)//curr_serial>=100)
		{
			spliter[0]=*separator;
			m_sMirXOriginPartNo.Append(spliter);
		}
		m_sMirYOriginPartNo=m_sMirZOriginPartNo=m_sMirXOriginPartNo;
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
		m_sMirZOriginPartNo.Append(ss);
	}
	else
	{
		m_sMirZOriginPartNo = m_sOriginPartNo;
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

	if(m_bSyncRotationParts&&m_pMirRotPart)
		m_sMirRotPartNo=m_sOriginPartNo;
	if(m_bSyncRotationParts&&m_pMirXnRotPart)
		m_sMirXnRotPartNo=m_sMirXOriginPartNo;
	if(m_bSyncRotationParts&&m_pMirYnRotPart)
		m_sMirYnRotPartNo=m_sMirYOriginPartNo;
	if(m_bSyncRotationParts&&m_pMirZnRotPart)
		m_sMirZnRotPartNo=m_sMirZOriginPartNo;
	UpdateData(FALSE);
	UpdateCtrlStates();
}
void CLabelOperDlg::OnChangeEMirXOriginPartNo()
{
	if(m_pMirXPart==NULL)
		return;	//防止设定编辑框为"无"时，激发死循环
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
	if(m_bSyncRotationParts&&m_pMirXnRotPart)
		m_sMirXnRotPartNo=m_sMirXOriginPartNo;
	if(m_bSyncRotationParts&&m_pMirYnRotPart)
		m_sMirYnRotPartNo=m_sMirYOriginPartNo;
	UpdateData(FALSE);
	UpdateCtrlStates();
}

void CLabelOperDlg::OnChangeAngleWorkWingG()
{
	UpdateData();
	m_nIdleWingG=m_nWorkWingG;
	CString gStr;
	GetDlgItem(IDC_ANGLE_WORK_WING_G)->GetWindowTextA(gStr);
	GetDlgItem(IDC_ANGLE_IDLE_WING_G)->SetWindowTextA(gStr);
}


void CLabelOperDlg::OnCbnSelchangeCmbSizeSpec()
{
	UpdateData();
	CComboBox* pCMBSize=(CComboBox*)GetDlgItem(IDC_CMB_SIZE_SPEC);
	if(pCMBSize==NULL||pCMBSize->GetSafeHwnd()==NULL)
		return;
	int iSel=pCMBSize->GetCurSel();
	double wing_wide,wing_thick;
	pCMBSize->GetLBText(iSel,m_sSizeSpec);
	restore_JG_guige(m_sSizeSpec,wing_wide,wing_thick);
	UpdateAngleGBySizeSpec(wing_wide);
	UpdateData(FALSE);
}

void CLabelOperDlg::OnBnClickedChkSynRotation()
{
	UpdateData();
	_localSyncRotationParts=m_bSyncRotationParts;
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_E_DIAGONAL_PART_NO);
	pEdit->SetReadOnly(!this->m_bSyncRotationParts);
	pEdit=(CEdit*)GetDlgItem(IDC_E_MIR_DIAGONAL_X_PART_NO);
	pEdit->SetReadOnly(!this->m_bSyncRotationParts);
	pEdit=(CEdit*)GetDlgItem(IDC_E_MIR_DIAGONAL_Y_PART_NO);
	pEdit->SetReadOnly(!this->m_bSyncRotationParts);
	pEdit=(CEdit*)GetDlgItem(IDC_E_MIR_DIAGONAL_Z_PART_NO);
	pEdit->SetReadOnly(!this->m_bSyncRotationParts);
	if(m_bSyncRotationParts&&m_pMirRotPart)
		m_sMirRotPartNo=m_sOriginPartNo;
	if(m_bSyncRotationParts&&m_pMirXnRotPart)
		m_sMirXnRotPartNo=m_sMirXOriginPartNo;
	if(m_bSyncRotationParts&&m_pMirYnRotPart)
		m_sMirYnRotPartNo=m_sMirYOriginPartNo;
	if(m_bSyncRotationParts&&m_pMirZnRotPart)
		m_sMirZnRotPartNo=m_sMirZOriginPartNo;
	UpdateData(FALSE);
}


void CLabelOperDlg::OnEnChangeMirRotPartNo()
{
	UpdateData();
	if(this->m_pMirXnRotPart)
		m_sMirXnRotPartNo=m_sMirRotPartNo;
	if(this->m_pMirYnRotPart)
		m_sMirYnRotPartNo=m_sMirRotPartNo;
	if(this->m_pMirZnRotPart)
		m_sMirZnRotPartNo=m_sMirRotPartNo;
	UpdateData(FALSE);
}


void CLabelOperDlg::OnEnChangeMirXnRotPartNo()
{
	UpdateData();
	if(this->m_pMirYnRotPart)
		m_sMirYnRotPartNo=m_sMirXnRotPartNo;
	UpdateData(FALSE);
}

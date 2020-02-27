//<LOCALE_TRANSLATE Confirm by hxr/>
// DesignRibbedPlateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "env_def.h"
#include "DesignRibbedPlateDlg.h"
#include "XhCharString.h"
#include "GlobalFunc.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "PartsDbDlg.h"
#include "btc.h"
#include "DataCompare.h"
#include "LocalFeatureDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
//
static BOOL FireCallBackCommand(CWnd* pCallWnd,UINT message,WPARAM wParam)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please Preview part(Press enter or esc key to end):","");
#else 
	pCmdLine->FillCmdLine("��תԤ��������ESC����س��������߰�Ի���:","");
#endif
	CString cmdStr;
	g_pSolidSet->SetOperType(OPER_ROTATE);
	pCmdLine->GetStrFromCmdLine(cmdStr);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CDesignRibbedPlateDlg dialog

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
int CDesignRibbedPlateDlg::nScrLocationX=0;
int CDesignRibbedPlateDlg::nScrLocationY=0;
CDesignRibbedPlateDlg::CDesignRibbedPlateDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignRibbedPlateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignRibbedPlateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDatumNode=NULL;
	m_pDatumBolt=NULL;
	m_pRibPlate=NULL;
	m_bNewRibPlate=TRUE;	//Ĭ�ϸöԻ������������߰�
	m_xRibPlate.bOnRelaPlateTopFace=FALSE;
	m_xRibPlate.m_cPlateType=m_xRibPlate.m_cSubType=0;
	m_xRibPlate.plate.fGradientDegAngle=90;
	m_bEnableWindowMoveListen=FALSE;
}

void CDesignRibbedPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignRibbedPlateDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_LIST_BOX_SUB, m_xSubPropList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignRibbedPlateDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignRibbedPlateDlg)
	ON_BN_CLICKED(IDC_BTN_CONFIRM, OnConfirm)
	ON_BN_CLICKED(IDC_BTN_PARTS_DB, OnPartsDb)
	ON_BN_CLICKED(IDC_BTN_MODEL_VIEW_OPER, OnBnModelViewOper)
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDesignRibbedPlateDlg message handlers
BOOL FirePropHelpPrompt(CPropertyList* pPropList,CPropTreeItem* pItem);
void UpdateRibPlateParaItems(CPropertyList* pPropList,BOOL bUpdate);
void UpdateRibBendPlaneParam(CPropertyList *pPropList,CPropTreeItem *pBendingItem,BOOL bUpdate)
{
	CDesignRibbedPlateDlg *pRibPlateDlg=(CDesignRibbedPlateDlg*)pPropList->GetParent();
	if(pRibPlateDlg==NULL)
		return;
	if(pRibPlateDlg->m_xRibPlate.plate.anchors[2].ciType!=0)
	{	//��������
		//����������ĩ�˶���
		CPropTreeItem *pPropItem=pRibPlateDlg->InsertItem(pPropList,pBendingItem,"RibPlate.plate.BendPlaneTip",PIT_COMBO,TRUE,-1,bUpdate);
		if(pRibPlateDlg->m_xRibPlate.plate.anchors[2].ciType==2)
		{
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendTipPointX"));
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendTipPointY"));
			pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.BendPlaneTipBolt",PIT_BUTTON,TRUE,-1,bUpdate);
		}
		else
		{
			pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.BendTipPointX",PIT_EDIT,TRUE,-1,bUpdate);
			pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.BendTipPointY",PIT_EDIT,TRUE,-1,bUpdate);
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendPlaneTipBolt"));
		}
		//��������������
		pPropItem=pRibPlateDlg->InsertItem(pPropList,pBendingItem,"RibPlate.plate.BendDirect",PIT_COMBO,TRUE,-1,bUpdate);
		if(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==1||pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==2)
		{	//0.�ظ˼������1.��˨��������
			pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.BendDirectVecPart",PIT_BUTTON,TRUE,-1,bUpdate);
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectPoint"));
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectPointX"));
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectPointY"));
		}
		else if(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==3)
		{	//ָ�����������
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectVecPart"));
			CPropTreeItem* pSubItem=pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.BendDirectPoint",PIT_BUTTON,TRUE,-1,bUpdate);
			pRibPlateDlg->InsertItem(pPropList,pSubItem,"RibPlate.plate.BendDirectPointX",PIT_EDIT,TRUE,-1,bUpdate);
			pRibPlateDlg->InsertItem(pPropList,pSubItem,"RibPlate.plate.BendDirectPointY",PIT_EDIT,TRUE,-1,bUpdate);
		}
		else //if(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==0)	//���ܶ���
		{
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectVecPart"));
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectPoint"));
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectPointX"));
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectPointY"));
		}

		pRibPlateDlg->InsertItem(pPropList,pBendingItem,"RibPlate.plate.fBendTipOffsetX",PIT_EDIT,TRUE,-1,bUpdate);
		pRibPlateDlg->InsertItem(pPropList,pBendingItem,"RibPlate.plate.fBendTipOffsetZ",PIT_EDIT,TRUE,-1,bUpdate);
	}
	else
		pPropList->DeleteAllSonItems(pBendingItem);
}
static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CDesignRibbedPlateDlg *pRibPlateDlg=(CDesignRibbedPlateDlg*)pPropList->GetParent();
	if(pRibPlateDlg==NULL)
		return FALSE;
	CLogErrorLife logErrLife;
	char sText[100]="";
	CItemInfo *lpInfo=NULL;
	CPropTreeItem *pPropItem=NULL;
	BOOL bReCalPlateWidth=FALSE;	//���¼���ְ����β���
	BOOL bReDesignPlate=FALSE;		//������Ƹְ�
	CLDSParamPlate *pRibPlate=pRibPlateDlg->GetRibPlate();
	if(CDesignRibbedPlateDlg::GetPropID("RibPlate.sPartNo")==pItem->m_idProp)
	{
		strncpy(pRibPlateDlg->m_xRibPlate.sPartNo,valueStr,15);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(valueStr,BOMPART::PLATE);
		if(pPart)
		{
			pRibPlateDlg->m_xRibPlate.nThick=ftoi(pPart->thick);
			pRibPlateDlg->m_xRibPlate.cMaterial=pPart->cMaterial;
			//
			CXhChar50 sValue("%.0f",pPart->thick);
			pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("RibPlate.nThick"),(char*)sValue);
			QuerySteelMatMark(pPart->cMaterial,sValue);
			pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("RibPlate.cMaterial"),(char*)sValue);
		}
#endif
		SEGI nSeg;
		if(ParsePartNo(pRibPlateDlg->m_xRibPlate.sPartNo,&nSeg,NULL))
			pRibPlateDlg->m_xRibPlate.nSegI=nSeg;
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("RibPlate.nSegI"),(char*)pRibPlateDlg->m_xRibPlate.nSegI.ToString());
		//�����߰�κ�
		if(pRibPlate)
		{
			pRibPlate->iSeg=nSeg;
			pRibPlate->SetPartNo(valueStr.GetBuffer(0));
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.nSegI")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.nSegI=SEGI(valueStr.GetBuffer());
		if(pRibPlate)
			pRibPlate->iSeg=pRibPlateDlg->m_xRibPlate.nSegI;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.nThick")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.nThick=atoi(valueStr);
		if(pRibPlate)
		{
			pRibPlate->thick=pRibPlateDlg->m_xRibPlate.nThick;
			pRibPlateDlg->ReDesignPlate();	//��������߰�
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.cMaterial")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.cMaterial=QuerySteelBriefMatMark(valueStr.GetBuffer(1));
		if(pRibPlate)
			pRibPlate->cMaterial=pRibPlateDlg->m_xRibPlate.cMaterial;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.m_cPlateType")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.m_cPlateType=valueStr[0]-'0';
		UpdateRibPlateParaItems(pPropList,TRUE);
		pRibPlateDlg->UpdateSketchMap();
		pRibPlateDlg->DisplaySubPropList(pItem,FALSE);
	}
	//��׼����Ϊ�ֹ�ʱʹ��
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.iDatumEdge")==pItem->m_idProp)
	{
		int iNewDatumEdge=valueStr[0]-'0';
		pRibPlateDlg->m_xRibPlate.tube.iDatumEdge=iNewDatumEdge;
		pRibPlateDlg->ModifyPlateParam('J',(long)pRibPlateDlg->m_xRibPlate.tube.iDatumEdge);
		bReCalPlateWidth=TRUE;	//���¼���ְ����Լ��߰�����ϵ
		//���½�����ʾ
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==4&&pRibPlateDlg->m_xRibPlate.tube.iDatumEdge<2)
			pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.tube.fInitOffsetAngle",PIT_EDIT,TRUE,-1,TRUE);
		if(pRibPlateDlg->m_xRibPlate.tube.iDatumEdge==2)
		{	
			CLDSLineTube *pTube=pRibPlateDlg->m_pDatumPart.LineTubePointer();
			pRibPlateDlg->ucs.axis_z=pTube->End()-pTube->Start();
			pRibPlateDlg->ucs.axis_x=pTube->ucs.axis_x;
			normalize(pRibPlateDlg->ucs.axis_x);
			normalize(pRibPlateDlg->ucs.axis_z);
			pRibPlateDlg->ucs.axis_y=pRibPlateDlg->ucs.axis_z^pRibPlateDlg->ucs.axis_x;
			normalize(pRibPlateDlg->ucs.axis_y);
			//
			pRibPlateDlg->ModifyPlateParam(KEY2C("Wx"),pRibPlateDlg->ucs.axis_x.x);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Wy"),pRibPlateDlg->ucs.axis_x.y);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Wz"),pRibPlateDlg->ucs.axis_x.z);
		}
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==4)
			pRibPlateDlg->DisplaySubPropList(pItem);
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.iWorkNormStyle")==pItem->m_idProp)
	{
		int iNewWorkNormStyle=valueStr[0]-'0';
		bReDesignPlate=TRUE;	//���¼���ְ����Լ��߰�����ϵ
		pRibPlateDlg->m_xRibPlate.tube.iWorkNormStyle=iNewWorkNormStyle;
		pRibPlateDlg->ModifyPlateParam(KEY2C("Nt"),(long)pRibPlateDlg->m_xRibPlate.tube.iWorkNormStyle);
		if(pRibPlateDlg->m_xRibPlate.tube.iWorkNormStyle==2)
		{	
			pRibPlateDlg->ModifyPlateParam(KEY2C("Nx"),pRibPlateDlg->ucs.axis_z.x);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Ny"),pRibPlateDlg->ucs.axis_z.y);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Nz"),pRibPlateDlg->ucs.axis_z.z);
		}
		//���½���
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->m_xRibPlate.tube.iWorkNormStyle!=0)
			pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.tube.fOffsetDistX",PIT_EDIT,TRUE,-1,TRUE);
		pRibPlateDlg->UpdateSketchMap();
		pRibPlateDlg->DisplaySubPropList(pItem);
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.nPlateNum")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.tube.nPlateNum=atoi(valueStr);
		pRibPlateDlg->m_xRibPlate.tube.fOffsetAngle=360.0/pRibPlateDlg->m_xRibPlate.tube.nPlateNum;
		if(pRibPlateDlg->m_pDatumPart->IsTube())
		{
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetAngle"));
			if(pRibPlateDlg->m_xRibPlate.tube.nPlateNum>1)
				pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem->m_pParent,"RibPlate.tube.fOffsetAngle",PIT_EDIT,-1,TRUE);
		}
		//if(pRibPlateDlg->m_pDatumPart->IsPlate() || pRibPlateDlg->m_xRibPlate.tube.nPlateNum==1)
		//{
		//	pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.bCreateOuterPlate"));
		//	pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateLen"));
		//	pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateHeight"));
		//	CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDesignRibbedPlateDlg::GetPropID("Profile"),NULL);
		//	if(pParentItem)
		//	{	//ͬʱ��������߰�
		//		pPropItem=pRibPlateDlg->InsertOrUpdateItem(pPropList,pParentItem,"RibPlate.bCreateOuterPlate",PIT_COMBO,TRUE,-1,TRUE);
		//		if(pRibPlateDlg->m_xRibPlate.bCreateOuterPlate)
		//		{	
		//			pRibPlateDlg->InsertOrUpdateItem(pPropList,pPropItem,"RibPlate.fOuterPlateLen",PIT_EDIT,TRUE,0,TRUE);
		//			pRibPlateDlg->InsertOrUpdateItem(pPropList,pPropItem,"RibPlate.fOuterPlateHeight",PIT_EDIT,TRUE,1,TRUE);
		//		}
		//	}
		//}
	}
	else if(pRibPlateDlg->m_pDatumPart->GetClassTypeId()==CLS_LINETUBE
		&&CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.extend_vec")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.tube.extend_vec=CStdVector::GetVector(valueStr[0]-'0');
		//���췽���޸ĺ���¶�λ�Ƕ�
		f3dPoint extend_vec=pRibPlateDlg->m_xRibPlate.tube.extend_vec;
		CLDSLineTube *pLineTube=pRibPlateDlg->m_pDatumPart.LineTubePointer();
		pLineTube->BuildUCS();
		vector_trans(extend_vec,pLineTube->ucs,FALSE);
		normalize(extend_vec);
		extend_vec.z=0;
		if(!extend_vec.IsZero())
		{
			double fAngle=Cal2dLineAng(f2dPoint(0,0),f2dPoint(extend_vec.x,extend_vec.y));
			pRibPlateDlg->m_xRibPlate.tube.fInitOffsetAngle=fAngle*DEGTORAD_COEF;
			long idProp=CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.iDatumEdge");
			if(pRibPlateDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			idProp=CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fInitOffsetAngle");
			if(pRibPlateDlg->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			pRibPlateDlg->ModifyPlateParam('J',(long)pRibPlateDlg->m_xRibPlate.tube.iDatumEdge);
			pRibPlateDlg->ModifyPlateParam('K',pRibPlateDlg->m_xRibPlate.tube.fInitOffsetAngle);
			bReCalPlateWidth=TRUE;	//���¼���ְ����Լ��߰�����ϵ
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fInitOffsetAngle")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.tube.fInitOffsetAngle=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('K',pRibPlateDlg->m_xRibPlate.tube.fInitOffsetAngle);
		bReCalPlateWidth=TRUE;	//���¼���ְ����Լ��߰�����ϵ
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetAngle")==pItem->m_idProp)
		pRibPlateDlg->m_xRibPlate.tube.fOffsetAngle=atof(valueStr);
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetDistM")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.tube.fOffsetDistM=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("DM"),atof(valueStr));
		bReDesignPlate=TRUE;
		//��׼����Ϊ�ֹ�ʱ�ſ��ܳ�������ƫ��ֵM,���������һ��Ϊ��ֵ
		//�ʲ�����ƫ��ֵ���䶯����Ҫ���¼��� wht 11-05-25
		//bReCalPlateWidth=TRUE;	
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetDistX")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.tube.fOffsetDistX=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("DX"),atof(valueStr));
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fOffsetDistX")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.plate.fOffsetDistX=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('X',atof(valueStr));
		//��׼����Ϊ�ְ����޹�������ʱ����Ҫ����X����ƫ��ֵ 
		//������¸ְ峤�Ȳ�����ƫ��ֵ�䶯���ı���������¼��� wht 11-05-25
		//bReCalPlateWidth=TRUE;		
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fOffsetDistZ")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.plate.fOffsetDistZ=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('Z',atof(valueStr));
		//��׼����Ϊ�ְ������ٴ���һ������ְ�ʱ���ȡ����ȿ��ܻ���Z��ƫ��ֵ�����仯������Ҫ���¼��� wht 11-05-25
		//�޸�Zƫ����ʱ����Ҫ���¼���ְ���,���β������û�����Ϊ׼ wht 16-10-25
		//bReCalPlateWidth=TRUE;
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.bOnDatumPlateTopFace")==pItem->m_idProp)
	{	//���׼����ͬ��
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			pRibPlateDlg->m_xRibPlate.plate.bOnDatumPlateTopFace=TRUE;
		else 
			pRibPlateDlg->m_xRibPlate.plate.bOnDatumPlateTopFace=FALSE;
		if(pRibPlateDlg->m_pDatumPart.IsHasPtr()&&pRibPlateDlg->m_pDatumPart->IsPlate())
		{	//��׼����Ϊ�ְ�ʱ��׼�������������ע�߰��ڸְ������ڲ� wht 11-05-04
			long hDatumPart=pRibPlateDlg->m_pDatumPart->handle;
			if(!pRibPlateDlg->m_xRibPlate.plate.bOnDatumPlateTopFace)
				hDatumPart*=-1;
			pRibPlateDlg->ModifyPlateParam('A',hDatumPart);
		}
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.bAlongVecPart")==pItem->m_idProp)
	{	//���׼����ͬ��
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			pRibPlateDlg->m_xRibPlate.plate.bAlongVecPart=TRUE;
		else 
			pRibPlateDlg->m_xRibPlate.plate.bAlongVecPart=FALSE;
		int cPlateType=pRibPlateDlg->m_xRibPlate.m_cPlateType;
		if(pRibPlateDlg->m_pDatumPart.IsHasPtr()&&pRibPlateDlg->m_pDatumPart->IsPlate())
		{
			long hVecPart=0;
			if(pRibPlateDlg->m_pVecPart.IsHasPtr())
				hVecPart=pRibPlateDlg->m_pVecPart->handle;
			if(!pRibPlateDlg->m_xRibPlate.plate.bAlongVecPart)
				hVecPart*=-1;
			pRibPlateDlg->ModifyPlateParam('V',hVecPart);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.bSpecOrg")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.plate.bSpecOrg=(valueStr[0]=='1');
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->m_xRibPlate.plate.bSpecOrg)
		{
			pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.fOriginX",PIT_EDIT,TRUE,-1,TRUE);
			pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.fOriginY",PIT_EDIT,TRUE,-1,TRUE);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Ox"),pRibPlateDlg->m_xRibPlate.plate.fOriginX);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Oy"),pRibPlateDlg->m_xRibPlate.plate.fOriginY);
			bReDesignPlate=TRUE;
		}
		else
		{
			pRibPlateDlg->InsertItem(pPropList,pItem,"m_pDatumBolt",PIT_BUTTON,TRUE,-1,TRUE);
			if(pRibPlateDlg->m_pDatumBolt)
			{
				pRibPlateDlg->DeletePlateParam(KEY2C("Ox"));
				pRibPlateDlg->DeletePlateParam(KEY2C("Oy"));
				pRibPlateDlg->ModifyPlateParam(KEY2C("O"),pRibPlateDlg->m_pDatumBolt->handle);
				bReDesignPlate=TRUE;
			}
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.bSpecAxisX")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			pRibPlateDlg->m_xRibPlate.plate.anchors[1].ciType=valueStr[0]-'0'+1;
			pRibPlateDlg->m_xRibPlate.plate.bSpecAxisX=(valueStr[0]=='3');
		}
		else
			pRibPlateDlg->m_xRibPlate.plate.bSpecAxisX=(valueStr[0]=='1');
		CLDSParamPlate *pRibPlate=pRibPlateDlg->GetRibPlate();
		f3dPoint axis_x(pRibPlateDlg->m_xRibPlate.plate.fXAxisX,pRibPlateDlg->m_xRibPlate.plate.fXAxisY);
		if(pRibPlateDlg->m_xRibPlate.plate.bSpecAxisX&&pRibPlateDlg&&axis_x.IsZero())
		{	//�л�Ϊ����X�᷽��ʱ��X�᷽��Ϊ0ʱ����X�᷽��Ĭ��ֵ
			axis_x=pRibPlate->ucs.axis_x;
			if(pRibPlateDlg->m_pDatumPart.IsHasPtr())
			{
				vector_trans(axis_x,pRibPlateDlg->m_pDatumPart->ucs,FALSE);
				pRibPlateDlg->m_xRibPlate.plate.fXAxisX=axis_x.x;
				pRibPlateDlg->m_xRibPlate.plate.fXAxisY=axis_x.y;
			}
		}
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->m_xRibPlate.plate.bSpecAxisX||pRibPlateDlg->m_xRibPlate.plate.anchors[1].ciType==3)
		{
			pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.plate.fXAxisX",PIT_EDIT,TRUE,0,TRUE);
			pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.plate.fXAxisY",PIT_EDIT,TRUE,1,TRUE);
			if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3&&pRibPlateDlg->m_xRibPlate.plate.anchors[1].ciType==3)
			{
				pRibPlateDlg->ModifyPlateParam(KEY2C("Ex"),pRibPlateDlg->m_xRibPlate.plate.anchors[1].locate.vertex.x);
				pRibPlateDlg->ModifyPlateParam(KEY2C("Ey"),pRibPlateDlg->m_xRibPlate.plate.anchors[1].locate.vertex.y);
			}
			else
			{
				pRibPlateDlg->ModifyPlateParam(KEY2C("Xx"),pRibPlateDlg->m_xRibPlate.plate.fXAxisX);
				pRibPlateDlg->ModifyPlateParam(KEY2C("Xy"),pRibPlateDlg->m_xRibPlate.plate.fXAxisY);
			}
			bReDesignPlate=TRUE;
		}
		else
		{
			pRibPlateDlg->InsertItem(pPropList,pItem,"m_pVecPart",PIT_BUTTON,TRUE,-1,TRUE);
			if(pRibPlateDlg->m_pVecPart.IsHasPtr())
			{
				long hVecPart=pRibPlateDlg->m_pVecPart->handle;
				if(pRibPlateDlg->m_pVecPart->IsLinePart())
				{
					pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.bAlongVecPart",PIT_COMBO,TRUE,-1,TRUE);
					if(!pRibPlateDlg->m_xRibPlate.plate.bAlongVecPart)
						hVecPart*=-1;
				}
				pRibPlateDlg->DeletePlateParam(KEY2C("Xx"));
				pRibPlateDlg->DeletePlateParam(KEY2C("Xy"));
				pRibPlateDlg->ModifyPlateParam(KEY2C("V"),hVecPart);
				bReDesignPlate=TRUE;
			}	
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.IsHasBendPlane")==pItem->m_idProp)
	{	//������","","0.��������|1.��������
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			pRibPlateDlg->m_xRibPlate.plate.anchors[2].ciType=valueStr[0]-'0';//"0.��������"
			UpdateRibBendPlaneParam(pPropList,pItem,TRUE);
			pRibPlateDlg->UpdateSketchMap();
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendPlaneTip")==pItem->m_idProp)
	{	//ĩ�˶�λ��","","0.��˨��λ|1.ֱ������
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3&&valueStr[0]=='0')
			pRibPlateDlg->m_xRibPlate.plate.anchors[2].ciType=2;//sText.Copy("0.��˨��λ");
		else if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3&&valueStr[0]=='1')
			pRibPlateDlg->m_xRibPlate.plate.anchors[2].ciType=3;//sText.Copy("1.ֱ������");
		if(pRibPlateDlg->m_xRibPlate.plate.anchors[2].ciType==2)
		{
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendTipPointX"));
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendTipPointY"));
			pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.BendPlaneTipBolt",PIT_BUTTON,TRUE,-1,TRUE);
		}
		else
		{
			pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.BendTipPointX",PIT_EDIT,TRUE,-1,TRUE);
			pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.BendTipPointY",PIT_EDIT,TRUE,-1,TRUE);
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendPlaneTipBolt"));
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendTipPointX")==pItem->m_idProp)
	{	//������ĩ��X
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			double Tx=atof(valueStr);
			pRibPlateDlg->m_xRibPlate.plate.anchors[2].locate.vertex.x=Tx;
			pRibPlateDlg->ModifyPlateParam(KEY2C("Tx"),Tx);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendTipPointY")==pItem->m_idProp)
	{	//������ĩ��Y
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			double Ty=atof(valueStr);
			pRibPlateDlg->m_xRibPlate.plate.anchors[2].locate.vertex.y=Ty;
			pRibPlateDlg->ModifyPlateParam(KEY2C("Ty"),Ty);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendPlaneTipBolt")==pItem->m_idProp)
	{	//������ĩ����˨��λ
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3&&pRibPlateDlg->m_xRibPlate.plate.anchors[2].ciType==2)
		{
			long hTipBolt=HexStrToLong(valueStr);
			pRibPlateDlg->m_xRibPlate.plate.anchors[2].locate.hBolt=hTipBolt;
			pRibPlateDlg->ModifyPlateParam(KEY2C("TB"),hTipBolt);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirect")==pItem->m_idProp)
	{	//����������","","0.���ܶ���|1.�ظ˼�����|2.��˨��������|3.���������"
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			if(valueStr[0]=='1')		//1.�ظ˼�����
				pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType=1;
			else if(valueStr[0]=='2')	//2.��˨��������
				pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType=2;
			else if(valueStr[0]=='3')	//3.���������
				pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType=3;
			else //if(valueStr[0]=='0')	//0.���ܶ���
				pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType=0;
			pPropList->DeleteAllSonItems(pItem);
			if(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==1||pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==2)
			{	//0.�ظ˼������1.��˨��������
				pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.BendDirectVecPart",PIT_BUTTON,TRUE,-1,TRUE);
			}
			else if(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==3)
			{	//ָ�����������
				CPropTreeItem* pSubItem=pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.BendDirectPoint",PIT_BUTTON,TRUE,-1,TRUE);
				pRibPlateDlg->InsertItem(pPropList,pSubItem,"RibPlate.plate.BendDirectPointX",PIT_EDIT,TRUE,-1,TRUE);
				pRibPlateDlg->InsertItem(pPropList,pSubItem,"RibPlate.plate.BendDirectPointY",PIT_EDIT,TRUE,-1,TRUE);
			}
			//else if(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==0)	//���ܶ���
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectVecPart")==pItem->m_idProp)
	{	//����������˨"
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3&&(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==1||pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==2))
		{
			long hTipVecPart=HexStrToLong(valueStr);
			if(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==1)
				pRibPlateDlg->m_xRibPlate.plate.anchors[3].locate.hVecRod=hTipVecPart;
			else
				pRibPlateDlg->m_xRibPlate.plate.anchors[3].locate.hBolt=hTipVecPart;
			pRibPlateDlg->ModifyPlateParam(KEY2C("BV"),hTipVecPart);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectPoint")==pItem->m_idProp)
	{	//BendDirectPoint"
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectPointX")==pItem->m_idProp)
	{	//���������"
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			double Bx=atof(valueStr);
			pRibPlateDlg->m_xRibPlate.plate.anchors[3].locate.vertex.x=Bx;
			pRibPlateDlg->ModifyPlateParam(KEY2C("Bx"),Bx);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectPointY")==pItem->m_idProp)
	{	//BendDirectPointY"
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			double By=atof(valueStr);
			pRibPlateDlg->m_xRibPlate.plate.anchors[3].locate.vertex.y=By;
			pRibPlateDlg->ModifyPlateParam(KEY2C("By"),By);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fBendTipOffsetX")==pItem->m_idProp)
	{	//X��ƫ�Ʀ�x"
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			pRibPlateDlg->m_xRibPlate.plate.fBendTipOffsetX=atof(valueStr);
			pRibPlateDlg->ModifyPlateParam(KEY2C("dx"),pRibPlateDlg->m_xRibPlate.plate.fBendTipOffsetX);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fBendTipOffsetZ")==pItem->m_idProp)
	{	//Z��ƫ�Ʀ�z"
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			pRibPlateDlg->m_xRibPlate.plate.fBendTipOffsetZ=atof(valueStr);
			pRibPlateDlg->ModifyPlateParam(KEY2C("dz"),pRibPlateDlg->m_xRibPlate.plate.fBendTipOffsetZ);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.iAxisXType")==pItem->m_idProp)
	{
		int iAxisXType=valueStr[0]-'0';
		pRibPlateDlg->m_xRibPlate.plate.iAxisXType=iAxisXType;
		if(iAxisXType==0 || iAxisXType==1)
		{
			pRibPlateDlg->m_xRibPlate.plate.fXAxisY=0;
			if(iAxisXType==0)
				pRibPlateDlg->m_xRibPlate.plate.fXAxisX=1;
			else
				pRibPlateDlg->m_xRibPlate.plate.fXAxisX=-1;
		}
		else if(iAxisXType==2 || iAxisXType==3)
		{
			pRibPlateDlg->m_xRibPlate.plate.fXAxisX=0;
			if(iAxisXType==2)
				pRibPlateDlg->m_xRibPlate.plate.fXAxisY=1;
			else
				pRibPlateDlg->m_xRibPlate.plate.fXAxisY=-1;
		}
		else if(iAxisXType==4)
		{
			CLDSParamPlate *pRibPlate=pRibPlateDlg->GetRibPlate();
			f3dPoint axis_x=pRibPlate->ucs.axis_x;;
			if(pRibPlateDlg->m_pDatumPart.IsHasPtr())
			{
				vector_trans(axis_x,pRibPlateDlg->m_pDatumPart->ucs,FALSE);
				pRibPlateDlg->m_xRibPlate.plate.fXAxisX=axis_x.x;
				pRibPlateDlg->m_xRibPlate.plate.fXAxisY=axis_x.y;
			}
		}
		//���½�����ʾ
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==0)
			pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.bOnRelaPlateTopFace",PIT_COMBO,TRUE,-1,TRUE);
		if(pRibPlateDlg->m_xRibPlate.plate.iAxisXType<5)
		{
			pPropItem=pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.fXAxisX",PIT_EDIT,TRUE,-1,TRUE);
			pPropItem->SetReadOnly(pRibPlateDlg->m_xRibPlate.plate.iAxisXType!=4);
			pPropItem=pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.fXAxisY",PIT_EDIT,TRUE,-1,TRUE);
			pPropItem->SetReadOnly(pRibPlateDlg->m_xRibPlate.plate.iAxisXType!=4);	
			pRibPlateDlg->ModifyPlateParam(KEY2C("Xx"),pRibPlateDlg->m_xRibPlate.plate.fXAxisX);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Xy"),pRibPlateDlg->m_xRibPlate.plate.fXAxisY);
		}
		else
		{
			pRibPlateDlg->DeletePlateParam(KEY2C("Xx"));
			pRibPlateDlg->DeletePlateParam(KEY2C("Xy"));
		}
		//�ж������X������ϵ�Ƿ����
		if(!pRibPlateDlg->IsValidAxisX())
		{
			logerr.Log("�߰巨���븨���ְ�1���߽���ƽ�У������X�᷽�򲻺���!");
			bReDesignPlate=FALSE;
		}
		else
			bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.bSpecAxisY")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.plate.bSpecAxisY=(valueStr[0]=='1');
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->m_xRibPlate.plate.bSpecAxisY)
		{
			pRibPlateDlg->InsertItem(pPropList,pItem,"RibPlate.plate.fGradientDegAngle",PIT_EDIT,TRUE,-1,TRUE);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Ga"),pRibPlateDlg->m_xRibPlate.plate.fGradientDegAngle);
			bReDesignPlate=TRUE;
		}
		else
		{
			pRibPlateDlg->DeletePlateParam(KEY2C("Ga"));
			if(pRibPlateDlg->m_xRibPlate.m_cPlateType==0||pRibPlateDlg->m_xRibPlate.m_cPlateType==1)
				pRibPlateDlg->InsertItem(pPropList,pItem,"m_pVecPart",PIT_BUTTON,TRUE,-1,TRUE);
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fGradientDegAngle")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.plate.fGradientDegAngle=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Ga"),pRibPlateDlg->m_xRibPlate.plate.fGradientDegAngle);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fOriginX")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.plate.fOriginX=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Ox"),pRibPlateDlg->m_xRibPlate.plate.fOriginX);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fOriginY")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.plate.fOriginY=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Oy"),pRibPlateDlg->m_xRibPlate.plate.fOriginY);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fXAxisX")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			pRibPlateDlg->m_xRibPlate.plate.anchors[1].locate.vertex.x=atof(valueStr);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Ex"),pRibPlateDlg->m_xRibPlate.plate.anchors[1].locate.vertex.x);
		}
		else
		{
			pRibPlateDlg->m_xRibPlate.plate.fXAxisX=atof(valueStr);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Xx"),pRibPlateDlg->m_xRibPlate.plate.fXAxisX);
		}
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fXAxisY")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_xRibPlate.m_cPlateType==3)
		{
			pRibPlateDlg->m_xRibPlate.plate.anchors[1].locate.vertex.y=atof(valueStr);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Ey"),pRibPlateDlg->m_xRibPlate.plate.anchors[1].locate.vertex.y);
		}
		else
		{
			pRibPlateDlg->m_xRibPlate.plate.fXAxisY=atof(valueStr);
			pRibPlateDlg->ModifyPlateParam(KEY2C("Xy"),pRibPlateDlg->m_xRibPlate.plate.fXAxisY);
		}
		bReDesignPlate=TRUE;
	}
	//
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.bOnRelaPlateTopFace")==pItem->m_idProp)
	{	//�븨������ͬ��
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			pRibPlateDlg->m_xRibPlate.bOnRelaPlateTopFace=TRUE;
		else 
			pRibPlateDlg->m_xRibPlate.bOnRelaPlateTopFace=FALSE;
		if(	pRibPlateDlg->m_pDatumPart.IsHasPtr()&&pRibPlateDlg->m_pDatumPart->GetClassTypeId()==CLS_LINETUBE
			&&pRibPlateDlg->m_pRelaPart1.IsNULL()&&pRibPlateDlg->m_pRelaPart2.IsNULL())
		{	//��׼����Ϊ�ֹ����޸����ְ�ʱ�ñ�����ʾ�Ƿ���ֹ�ͬ�� 
			long hDatumPart=0;
			if(pRibPlateDlg->m_pDatumPart.IsHasPtr())
				hDatumPart=pRibPlateDlg->m_pDatumPart->handle;
			if(!pRibPlateDlg->m_xRibPlate.bOnRelaPlateTopFace)
				hDatumPart*=-1;
			pRibPlateDlg->ModifyPlateParam('A',hDatumPart);
		}
		else if(pRibPlateDlg->m_pRelaPart1.IsHasPtr())
		{
			long hRelaPart1=pRibPlateDlg->m_pRelaPart1->handle;
			if(!pRibPlateDlg->m_xRibPlate.bOnRelaPlateTopFace)
				hRelaPart1*=-1;
			pRibPlateDlg->ModifyPlateParam('C',hRelaPart1);
		}
		bReCalPlateWidth=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.bAutoExtendOutterEdge")==pItem->m_idProp)
	{	//�Զ�������������Ե
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			pRibPlateDlg->m_xRibPlate.bAutoExtendOutterEdge=TRUE;
		else 
			pRibPlateDlg->m_xRibPlate.bAutoExtendOutterEdge=FALSE;
		if(pRibPlateDlg->m_xRibPlate.bAutoExtendOutterEdge
			&&pRibPlateDlg->m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		{	
			if(pRibPlateDlg->m_pRelaPart2.IsHasPtr()&&pRibPlateDlg->m_pRelaPart2->IsPlate())
			{	//�����⻷�뾶
				pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fTopWidth"));
				pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.tube.fTopOutterR",PIT_EDIT,TRUE,0,TRUE);
			}
			else 
			{	//�������
				pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fTopOutterR"));
				pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.fTopWidth",PIT_EDIT,TRUE,0,TRUE);
			}
			//�ײ����ΰ뾶
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fBottomWidth"));
			pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.tube.fBottomOutterR",PIT_EDIT,TRUE,1,TRUE);
		}
		else
		{	//�������
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fTopOutterR"));
			pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.fTopWidth",PIT_EDIT,TRUE,0,TRUE);
			//�ײ����(��׼����Ϊ�ֹܻ����ñ�Ե����ʱ�ײ������ϵͳ�Զ�����)
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fBottomOutterR"));
			pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.fBottomWidth",PIT_EDIT,TRUE,1,TRUE);
		}
		if(pRibPlateDlg->m_xRibPlate.bAutoExtendOutterEdge)
		{
			pRibPlateDlg->ModifyPlateParam('R',pRibPlateDlg->m_xRibPlate.tube.fBottomOutterR);
			pRibPlateDlg->ModifyPlateParam('r',pRibPlateDlg->m_xRibPlate.tube.fTopOutterR);
		}
		else 
		{	//�����ö����⻷�뾶���ײ��⻷�뾶,ֱ�����붥�������ײ����
			pRibPlateDlg->ModifyPlateParam('R',(double)-1); //�˴���Ӧ��ʹ�� long wht 11-06-01
			pRibPlateDlg->ModifyPlateParam('r',(double)-1);
		}
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fBottomOutterR")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.tube.fBottomOutterR=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('R',pRibPlateDlg->m_xRibPlate.tube.fBottomOutterR);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fTopOutterR")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.tube.fTopOutterR=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('r',pRibPlateDlg->m_xRibPlate.tube.fTopOutterR);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.bVerticalPlate")==pItem->m_idProp)
	{	//��ֱ�Ҵ��Զ���λ���߰�
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			pRibPlateDlg->m_xRibPlate.tube.bVerticalPlate=TRUE;
		else
			pRibPlateDlg->m_xRibPlate.tube.bVerticalPlate=FALSE;
		if(pRibPlateDlg->m_pRelaPart1.IsHasPtr()&&pRibPlateDlg->m_pRelaPart1->IsPlate()
			&&pRibPlateDlg->m_pRelaPart2.IsNULL())
		{
			pRibPlateDlg->ModifyPlateParam('S',(long)pRibPlateDlg->m_xRibPlate.tube.bVerticalPlate);
			bReDesignPlate=TRUE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.bAutoCalB")==pItem->m_idProp)
	{
		BOOL bAutoCalB=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			bAutoCalB=TRUE;
		else
			bAutoCalB=FALSE;
		pRibPlateDlg->m_xRibPlate.tube.bAutoCalB=bAutoCalB;
		pRibPlateDlg->m_xRibPlate.tube.margin=bAutoCalB?0:-1;
		pRibPlateDlg->ModifyPlateParam(KEY2C("mn"),(long)pRibPlateDlg->m_xRibPlate.tube.margin);
		bReDesignPlate=TRUE;
		//
		pPropList->DeleteAllSonItems(pItem);
		if(bAutoCalB)
			pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.tube.margin",PIT_EDIT,TRUE,1,TRUE);
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.margin")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.tube.margin=atoi(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("mn"),(long)pRibPlateDlg->m_xRibPlate.tube.margin);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.fTopWidth")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.fTopWidth=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('F',pRibPlateDlg->m_xRibPlate.fTopWidth);
		bReDesignPlate=TRUE;
		//
		if(pRibPlateDlg->m_pDatumPart&&pRibPlateDlg->m_pDatumPart->GetClassTypeId()==CLS_PLATE)
		{	//��׼����Ϊ�ְ�ʱ�����ݶ����߶���ײ��߶Ȳ�ֵ�ж��Ƿ���ʾ���߶����� wth 11-05-19
			if(fabs(pRibPlateDlg->m_xRibPlate.fTopWidth-pRibPlateDlg->m_xRibPlate.fBottomWidth)>EPS)
			{	//�����߶���ײ��߶Ȳ��ȸ�ʱ���߶���Ч wht 11-05-19
				CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDesignRibbedPlateDlg::GetPropID("Profile"),NULL);
				CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fHeight"),NULL);
				if(pFindItem==NULL&&pParentItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Outside height";
					lpInfo->m_strPropHelp = "length of the edge opposite to datum parts, when the height of top is not equal with the height of bottom, it is valid.";
#else
					lpInfo->m_strPropName = "���߶�";
					lpInfo->m_strPropHelp = "���׼����(�����ӵı�)��Ӧ�ıߵı߳�,�������߶���ײ��߶Ȳ��ȸ�ʱ�ñ�����Ч��";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = CDesignRibbedPlateDlg::GetPropID("RibPlate.fHeight");
					if(pRibPlateDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue=sText;
					//pPropItem->SetPropHelpPromptFunc(FirePropHelpPrompt);
				}
			}
			else 
				pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fHeight"));
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.fBottomWidth")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.fBottomWidth=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('E',pRibPlateDlg->m_xRibPlate.fBottomWidth);
		bReDesignPlate=TRUE;
		//
		if(pRibPlateDlg->m_pDatumPart&&pRibPlateDlg->m_pDatumPart->GetClassTypeId()==CLS_PLATE)
		{	//��׼����Ϊ�ְ�ʱ�����ݶ����߶���ײ��߶Ȳ�ֵ�ж��Ƿ���ʾ���߶����� wth 11-05-19
			if(fabs(pRibPlateDlg->m_xRibPlate.fTopWidth-pRibPlateDlg->m_xRibPlate.fBottomWidth)>EPS)
			{	//�����߶���ײ��߶Ȳ��ȸ�ʱ���߶���Ч wht 11-05-19
				CPropTreeItem *pParentItem=pPropList->FindItemByPropId(CDesignRibbedPlateDlg::GetPropID("Profile"),NULL);
				CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fHeight"),NULL);
				if(pFindItem==NULL&&pParentItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Outside height";
					lpInfo->m_strPropHelp = "length of the edge opposite to datum parts, when the height of top is not equal with the height of bottom, it is valid.";
#else
					lpInfo->m_strPropName = "���߶�";
					lpInfo->m_strPropHelp = "���׼����(�����ӵı�)��Ӧ�ıߵı߳�,�������߶���ײ��߶Ȳ��ȸ�ʱ�ñ�����Ч��";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
					pPropItem->m_idProp = CDesignRibbedPlateDlg::GetPropID("RibPlate.fHeight");
					if(pRibPlateDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue=sText;
					//pPropItem->SetPropHelpPromptFunc(FirePropHelpPrompt);
				}
			}
			else 
				pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fHeight"));
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.fLength")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.fLength=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('G',pRibPlateDlg->m_xRibPlate.fLength);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.fHeight")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.fHeight=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('H',pRibPlateDlg->m_xRibPlate.fHeight);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.fCirR")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.fCirR=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("DR"),pRibPlateDlg->m_xRibPlate.fCirR);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.iCutAngleMode")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.iCutAngleMode=valueStr[0]-'0';
		pRibPlateDlg->ModifyPlateParam('e',(long)pRibPlateDlg->m_xRibPlate.iCutAngleMode);
		if(pRibPlateDlg->m_xRibPlate.iCutAngleMode==1)
		{	//���µ�Բ���뾶
			pRibPlateDlg->m_xRibPlate.fCutAngleWidth=pRibPlateDlg->_InitWaterHoleRadiusByThick();
			pRibPlateDlg->ModifyPlateParam('I',pRibPlateDlg->m_xRibPlate.fCutAngleWidth);
		}
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.fCutAngleWidth")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.fCutAngleWidth=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam('I',pRibPlateDlg->m_xRibPlate.fCutAngleWidth);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.fCutAngleWidth2")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.fCutAngleWidth2=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("I2"),pRibPlateDlg->m_xRibPlate.fCutAngleWidth2);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.MF")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.MF=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("MF"),pRibPlateDlg->m_xRibPlate.MF);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.MG")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.MG=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("MG"),pRibPlateDlg->m_xRibPlate.MG);
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.MH")==pItem->m_idProp)
	{
		pRibPlateDlg->m_xRibPlate.MH=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("MH"),pRibPlateDlg->m_xRibPlate.MH);
		if(pRibPlateDlg->m_xRibPlate.MH>0)
		{
			pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.MF",PIT_EDIT,TRUE,0,TRUE);
			pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.MG",PIT_EDIT,TRUE,1,TRUE);
		}
		else
		{
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.MF"));
			pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.MG"));
		}
		bReDesignPlate=TRUE;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.bCreateOuterPlate")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			pRibPlateDlg->m_xRibPlate.bCreateOuterPlate=TRUE;
		else 
			pRibPlateDlg->m_xRibPlate.bCreateOuterPlate=FALSE;
		pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateLen"));
		pPropList->DeleteItemByPropId(CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateHeight"));
		if(pRibPlateDlg->m_xRibPlate.bCreateOuterPlate)
		{	
			pRibPlateDlg->m_xRibPlate.fOuterPlateLen=pRibPlateDlg->m_xRibPlate.fLength;
			pRibPlateDlg->m_xRibPlate.fOuterPlateHeight=pRibPlateDlg->m_xRibPlate.fBottomWidth;
			//����߰峤��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "outside rib plate length";
			lpInfo->m_strPropHelp = "length of outside rib plate, that is, length of the edge connected with datum plate";
#else
			lpInfo->m_strPropName = "����߰峤��";
			lpInfo->m_strPropHelp = "����߰峤��,�����׼�ְ������ߵĳ��ȡ�";
#endif
			pPropItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
			pPropItem->m_idProp = CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateLen");
			if(pRibPlateDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=sText;
			//����߰�߶�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "outside rib plate height";
			lpInfo->m_strPropHelp = "length of outside rib plate, that is, length of the edge connected with datum plate";
#else
			lpInfo->m_strPropName = "����߰�߶�";
			lpInfo->m_strPropHelp = "����߰�߶�,�����׼�ְ���Աߵĳ��ȡ�";
#endif
			pPropItem = pPropList->InsertItem(pItem,lpInfo,1,TRUE);
			pPropItem->m_idProp = CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateHeight");
			if(pRibPlateDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=sText;
		}
		//bReCalPlateWidth=TRUE; �������¼����߰��� 
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateLen")==pItem->m_idProp)
		pRibPlateDlg->m_xRibPlate.fOuterPlateLen=atof(valueStr);
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateHeight")==pItem->m_idProp)
		pRibPlateDlg->m_xRibPlate.fOuterPlateHeight=atof(valueStr);
	//��׼��������������
	else if(CDesignRibbedPlateDlg::GetPropID("m_pDatumPart")==pItem->m_idProp)
	{
		long h=0;
		sscanf(valueStr.GetBuffer(1),"%X",&h);
		CLDSPart *pPart=console.FromPartHandle(h);
		if(pPart&&(pPart->IsPlate()||pPart->GetClassTypeId()==CLS_LINETUBE||pPart->GetClassTypeId()==CLS_LINEANGLE))
			pRibPlateDlg->m_pDatumPart=pPart;
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The part handle you have just input is wrong��please input again!");
#else
			AfxMessageBox("���빹������������������룡");
#endif
			return FALSE;
		}
	}
	else if(CDesignRibbedPlateDlg::GetPropID("m_pRelaPart1")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("m_pRelaPart2")==pItem->m_idProp)
	{
		long h=0;
		sscanf(valueStr.GetBuffer(1),"%X",&h);
		CLDSPart *pPart=console.FromPartHandle(h);
		if(pPart&&(pPart->IsPlate()||pPart->GetClassTypeId()==CLS_LINETUBE))
		{
			if(CDesignRibbedPlateDlg::GetPropID("m_pRelaPart1")==pItem->m_idProp)
				pRibPlateDlg->m_pRelaPart1=pPart;
			else 
				pRibPlateDlg->m_pRelaPart2=pPart;
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The part handle you have just input is wrong,please input again!");
#else
			AfxMessageBox("���빹������������������룡");
#endif
			return FALSE;
		}
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("ucs.axis_z"))
	{
		pRibPlateDlg->ucs.axis_z=CStdVector::GetVector(valueStr[0]-'0');
		sprintf(sText,"%f",pRibPlateDlg->ucs.axis_z.x);
		SimplifiedNumString(sText);
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_z.x"),sText);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Nx"),pRibPlateDlg->ucs.axis_z.x);
		sprintf(sText,"%f",pRibPlateDlg->ucs.axis_z.y);
		SimplifiedNumString(sText);
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_z.y"),sText);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Ny"),pRibPlateDlg->ucs.axis_z.y);
		sprintf(sText,"%f",pRibPlateDlg->ucs.axis_z.z);
		SimplifiedNumString(sText);
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_z.z"),sText);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Nz"),pRibPlateDlg->ucs.axis_z.z);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("ucs.axis_x"))
	{
		pRibPlateDlg->ucs.axis_x=CStdVector::GetVector(valueStr[0]-'0');
		sprintf(sText,"%f",pRibPlateDlg->ucs.axis_x.x);
		SimplifiedNumString(sText);
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_x.x"),sText);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Wx"),pRibPlateDlg->ucs.axis_x.x);
		sprintf(sText,"%f",pRibPlateDlg->ucs.axis_x.y);
		SimplifiedNumString(sText);
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_x.y"),sText);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Wy"),pRibPlateDlg->ucs.axis_x.y);
		sprintf(sText,"%f",pRibPlateDlg->ucs.axis_x.z);
		SimplifiedNumString(sText);
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_x.z"),sText);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Wz"),pRibPlateDlg->ucs.axis_x.z);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("ucs.axis_z.x"))
	{	
		pRibPlateDlg->ucs.axis_z.x=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Nx"),pRibPlateDlg->ucs.axis_z.x);
		int vector_type=CStdVector::GetVectorType(pRibPlateDlg->ucs.axis_z);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(vector_type));
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_z"),sText);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("ucs.axis_z.y"))
	{
		pRibPlateDlg->ucs.axis_z.y=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Ny"),pRibPlateDlg->ucs.axis_z.y);
		int vector_type=CStdVector::GetVectorType(pRibPlateDlg->ucs.axis_z);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(vector_type));
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_z"),sText);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("ucs.axis_z.z"))
	{
		pRibPlateDlg->ucs.axis_z.z=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Nz"),pRibPlateDlg->ucs.axis_z.z);
		int vector_type=CStdVector::GetVectorType(pRibPlateDlg->ucs.axis_z);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(vector_type));
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_z"),sText);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("ucs.axis_x.x"))
	{
		pRibPlateDlg->ucs.axis_x.x=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Wx"),pRibPlateDlg->ucs.axis_x.x);
		int vector_type=CStdVector::GetVectorType(pRibPlateDlg->ucs.axis_x);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(vector_type));
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_x"),sText);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("ucs.axis_x.y"))
	{
		pRibPlateDlg->ucs.axis_x.y=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Wy"),pRibPlateDlg->ucs.axis_x.y);
		int vector_type=CStdVector::GetVectorType(pRibPlateDlg->ucs.axis_x);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(vector_type));
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_x"),sText);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("ucs.axis_x.z"))
	{
		pRibPlateDlg->ucs.axis_x.z=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Wz"),pRibPlateDlg->ucs.axis_x.z);
		int vector_type=CStdVector::GetVectorType(pRibPlateDlg->ucs.axis_x);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(vector_type));
		pPropList->SetItemPropValue(CDesignRibbedPlateDlg::GetPropID("ucs.axis_x"),sText);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.cLeftTopCornerType"))
	{
		pRibPlateDlg->m_xRibPlate.tube.cLeftTopCornerType=valueStr[0]-'0';
		pRibPlateDlg->ModifyPlateParam(KEY2C("Lt"),(long)pRibPlateDlg->m_xRibPlate.tube.cLeftTopCornerType);
		bReDesignPlate=TRUE;
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->m_xRibPlate.tube.cLeftTopCornerType!=0)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			if(pRibPlateDlg->m_xRibPlate.tube.cLeftTopCornerType==1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "cutting angle width";
				lpInfo->m_strPropHelp = "width of top cutting angle in X-axis direction";
#else
				lpInfo->m_strPropName = "X���н���";
				lpInfo->m_strPropHelp = "X+�򶥲��нǿ��";
#endif
			}
			else if(pRibPlateDlg->m_xRibPlate.tube.cLeftTopCornerType==2)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "cutting arc radius";
				lpInfo->m_strPropHelp = "top cutting arc radius in X+ direction";
#else
				lpInfo->m_strPropName = "�л��뾶";
				lpInfo->m_strPropHelp = "X+�򶥲��л��뾶";
#endif
			}
			pPropItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
			pPropItem->m_idProp = CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fLeftTopCornerWidth");
			if(pRibPlateDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=sText;
		}
		if(pRibPlateDlg->m_xRibPlate.tube.cLeftTopCornerType==1)
			pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.tube.fLeftTopCornerHeight",PIT_EDIT,TRUE,1,TRUE);
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fLeftTopCornerWidth"))
	{
		pRibPlateDlg->m_xRibPlate.tube.fLeftTopCornerWidth=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Lw"),pRibPlateDlg->m_xRibPlate.tube.fLeftTopCornerWidth);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fLeftTopCornerHeight"))
	{
		pRibPlateDlg->m_xRibPlate.tube.fLeftTopCornerHeight=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Lh"),pRibPlateDlg->m_xRibPlate.tube.fLeftTopCornerHeight);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.cRightTopCornerType"))
	{
		pRibPlateDlg->m_xRibPlate.tube.cRightTopCornerType=valueStr[0]-'0';
		pRibPlateDlg->ModifyPlateParam(KEY2C("Rt"),(long)pRibPlateDlg->m_xRibPlate.tube.cRightTopCornerType);
		bReDesignPlate=TRUE;
		pPropList->DeleteAllSonItems(pItem);
		if(pRibPlateDlg->m_xRibPlate.tube.cRightTopCornerType!=0)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			if(pRibPlateDlg->m_xRibPlate.tube.cRightTopCornerType==1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "cutting angle width";
				lpInfo->m_strPropHelp = "width of top cutting angle in X- direction";
#else
				lpInfo->m_strPropName = "X���н���";
				lpInfo->m_strPropHelp = "X-�򶥲��нǿ��";
#endif
			}
			else if(pRibPlateDlg->m_xRibPlate.tube.cRightTopCornerType==2)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "cutting arc radius";
				lpInfo->m_strPropHelp = "top cutting arc radius in X- direction";
#else
				lpInfo->m_strPropName = "�л��뾶";
				lpInfo->m_strPropHelp = "X-�򶥲��л��뾶";
#endif
			}
			pPropItem = pPropList->InsertItem(pItem,lpInfo,0,TRUE);
			pPropItem->m_idProp = CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fRightTopCornerWidth");
			if(pRibPlateDlg->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=sText;
			if(pRibPlateDlg->m_xRibPlate.tube.cRightTopCornerType==1)
				pRibPlateDlg->InsertOrUpdateItem(pPropList,pItem,"RibPlate.tube.fRightTopCornerHeight",PIT_EDIT,TRUE,1,TRUE);
		}
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fRightTopCornerWidth"))
	{
		pRibPlateDlg->m_xRibPlate.tube.fRightTopCornerWidth=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Rw"),pRibPlateDlg->m_xRibPlate.tube.fRightTopCornerWidth);
		bReDesignPlate=TRUE;
	}
	else if(pItem->m_idProp==CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fRightTopCornerHeight"))
	{
		pRibPlateDlg->m_xRibPlate.tube.fRightTopCornerHeight=atof(valueStr);
		pRibPlateDlg->ModifyPlateParam(KEY2C("Rh"),pRibPlateDlg->m_xRibPlate.tube.fRightTopCornerHeight);
		bReDesignPlate=TRUE;
	}
	if(bReCalPlateWidth)
	{	//���¼��������ʼֵ
		pRibPlateDlg->AutoCalRibPlateWidth();	
		//���½�����ʾ���߰���������
		long idProp=CDesignRibbedPlateDlg::GetPropID("RibPlate.fLength");
		if(pRibPlateDlg->GetPropValueStr(idProp,sText)>0)
			pRibPlateDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		idProp=CDesignRibbedPlateDlg::GetPropID("RibPlate.fHeight");
		if(pRibPlateDlg->GetPropValueStr(idProp,sText)>0)
			pRibPlateDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		idProp=CDesignRibbedPlateDlg::GetPropID("RibPlate.fCirR");
		if(pRibPlateDlg->GetPropValueStr(idProp,sText)>0)
			pRibPlateDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		idProp=CDesignRibbedPlateDlg::GetPropID("RibPlate.fBottomWidth");
		if(pRibPlateDlg->GetPropValueStr(idProp,sText)>0)
			pRibPlateDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		idProp=CDesignRibbedPlateDlg::GetPropID("RibPlate.fTopWidth");
		if(pRibPlateDlg->GetPropValueStr(idProp,sText)>0)
			pRibPlateDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		idProp=CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateLen");
		if(pRibPlateDlg->GetPropValueStr(idProp,sText)>0)
			pRibPlateDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		idProp=CDesignRibbedPlateDlg::GetPropID("RibPlate.fOuterPlateHeight");
		if(pRibPlateDlg->GetPropValueStr(idProp,sText)>0)
			pRibPlateDlg->m_xSubPropList.SetItemPropValue(idProp,sText);
		//���¸ְ����ֵ
		pRibPlateDlg->ModifyPlateParam('F',pRibPlateDlg->m_xRibPlate.fTopWidth);
		pRibPlateDlg->ModifyPlateParam('E',pRibPlateDlg->m_xRibPlate.fBottomWidth);
		pRibPlateDlg->ModifyPlateParam('G',pRibPlateDlg->m_xRibPlate.fLength);
		pRibPlateDlg->ModifyPlateParam('H',pRibPlateDlg->m_xRibPlate.fHeight);
		pRibPlateDlg->ModifyPlateParam('I',pRibPlateDlg->m_xRibPlate.fCutAngleWidth);
		if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fOffsetDistX")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fOffsetDistZ")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetDistM")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetDistX")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("ucs.axis_z.x")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("ucs.axis_z.y")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("ucs.axis_z.z")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("ucs.axis_x.x")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("ucs.axis_x.y")==pItem->m_idProp
			||CDesignRibbedPlateDlg::GetPropID("ucs.axis_x.z")==pItem->m_idProp)
		{
			UCS_STRU object_ucs,ucs;
			ucs = pRibPlateDlg->ucs;
			g_pSolidSet->GetObjectUcs(object_ucs);
			coord_trans(ucs.origin,object_ucs,TRUE);
			vector_trans(ucs.axis_x,object_ucs,TRUE);
			vector_trans(ucs.axis_y,object_ucs,TRUE);
			vector_trans(ucs.axis_z,object_ucs,TRUE);
			g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
			//g_pSolidSet->SetDispWorkUcs();
			CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
			pView->Invalidate(FALSE);
		}
	}
	//������Ƹְ����Σ�ʵʱ��ʾ wht 11-05-04
	if(bReCalPlateWidth||bReDesignPlate)
		pRibPlateDlg->ReDesignPlate();
	return TRUE;
}

static BOOL ButtonClickItem(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesignRibbedPlateDlg *pRibPlateDlg=(CDesignRibbedPlateDlg*)pPropList->GetParent();
	if(CDesignRibbedPlateDlg::GetPropID("m_pDatumPart")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("m_pDatumBolt")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("m_pRelaPart1")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("m_pRelaPart2")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("m_pVecPart")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendPlaneTipBolt")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectVecPart")==pItem->m_idProp)
	{	//ѡ�񹹼�
		pRibPlateDlg->SelectObject(CLS_PART,pItem->m_idProp);	
	}
	else if(CDesignRibbedPlateDlg::GetPropID("m_pDatumNode")==pItem->m_idProp)
		pRibPlateDlg->SelectObject(CLS_NODE,pItem->m_idProp);	//ѡ���׼�ڵ�
	return TRUE;
}
static BOOL FireSubPropHelpPrompt(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CDesignRibbedPlateDlg *pRibPlateDlg=(CDesignRibbedPlateDlg*)pPropList->GetParent();
	if(pRibPlateDlg==NULL||pView==NULL)
		return FALSE;
	g_pSolidDraw->DelCS(ISolidDraw::WORK_CS);
	pView->Invalidate(FALSE);
	pRibPlateDlg->RedrawSketchDrawing(pItem);
	return TRUE;
}
static BOOL FirePropHelpPrompt(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	if(pPropList->GetDlgCtrlID()!=IDC_LIST_BOX)
		return FALSE;	//�������������Բ���ӦFirePropHelpPrompt()����
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CDesignRibbedPlateDlg *pRibPlateDlg=(CDesignRibbedPlateDlg*)pPropList->GetParent();
	if(pRibPlateDlg==NULL||pView==NULL)
		return FALSE;
	//g_pSolidSet->SetDispWorkUcs(FALSE);
	g_pSolidDraw->DelCS(ISolidDraw::WORK_CS);
	pView->Invalidate(FALSE);
	long hCurPart=0;
	if(CDesignRibbedPlateDlg::GetPropID("m_pDatumPart")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pDatumPart.IsHasPtr())
			hCurPart=pRibPlateDlg->m_pDatumPart->handle;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("m_pDatumNode")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pDatumNode)
			hCurPart=pRibPlateDlg->m_pDatumNode->handle;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("m_pDatumBolt")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pDatumBolt)
			hCurPart=pRibPlateDlg->m_pDatumBolt->handle;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("m_pRelaPart1")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pRelaPart1.IsHasPtr())
			hCurPart=pRibPlateDlg->m_pRelaPart1->handle;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("m_pRelaPart2")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pRelaPart2.IsHasPtr())
			hCurPart=pRibPlateDlg->m_pRelaPart2->handle;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("m_pVecPart")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_pVecPart.IsHasPtr())
			hCurPart=pRibPlateDlg->m_pVecPart->handle;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendPlaneTipBolt")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_xRibPlate.plate.anchors[2].ciType==2)
			hCurPart=pRibPlateDlg->m_xRibPlate.plate.anchors[2].locate.hBolt;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.BendDirectVecPart")==pItem->m_idProp)
	{
		if(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==1)
			hCurPart=pRibPlateDlg->m_xRibPlate.plate.anchors[3].locate.hVecRod;
		else if(pRibPlateDlg->m_xRibPlate.plate.anchors[3].ciType==2)
			hCurPart=pRibPlateDlg->m_xRibPlate.plate.anchors[3].locate.hBolt;
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.bOnDatumPlateTopFace")==pItem->m_idProp)
	{	//���׼����ͬ��
		UCS_STRU object_ucs,ucs = pRibPlateDlg->m_pDatumPart->ucs;
		g_pSolidSet->GetObjectUcs(object_ucs);
		coord_trans(ucs.origin,object_ucs,TRUE);
		vector_trans(ucs.axis_x,object_ucs,TRUE);
		vector_trans(ucs.axis_y,object_ucs,TRUE);
		vector_trans(ucs.axis_z,object_ucs,TRUE);
		g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
		//g_pSolidSet->SetDispWorkUcs();
		pView->Invalidate(FALSE);
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.bOnRelaPlateTopFace")==pItem->m_idProp)
	{	//�븨������ͬ��
		UCS_STRU object_ucs,ucs;
		if(pRibPlateDlg->m_pRelaPart1.IsNULL())
			ucs = pRibPlateDlg->m_pDatumPart->ucs;
		else
			ucs = pRibPlateDlg->m_pRelaPart1->ucs;
		g_pSolidSet->GetObjectUcs(object_ucs);
		coord_trans(ucs.origin,object_ucs,TRUE);
		vector_trans(ucs.axis_x,object_ucs,TRUE);
		vector_trans(ucs.axis_y,object_ucs,TRUE);
		vector_trans(ucs.axis_z,object_ucs,TRUE);
		g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
		//g_pSolidSet->SetDispWorkUcs();
		pView->Invalidate(FALSE);
	}
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fOffsetDistX")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.fOffsetDistZ")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("RibPlate.plate.bAlongVecPart")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetDistM")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetDistX")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fInitOffsetAngle")==pItem->m_idProp
		||CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetAngle")==pItem->m_idProp)
	{
		UCS_STRU object_ucs,ucs;
		ucs = pRibPlateDlg->ucs;
		g_pSolidSet->GetObjectUcs(object_ucs);
		coord_trans(ucs.origin,object_ucs,TRUE);
		vector_trans(ucs.axis_x,object_ucs,TRUE);
		vector_trans(ucs.axis_y,object_ucs,TRUE);
		vector_trans(ucs.axis_z,object_ucs,TRUE);
		g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,ucs);
		//g_pSolidSet->SetDispWorkUcs();
		pView->Invalidate(FALSE);
	}
	pRibPlateDlg->DisplaySubPropList(pItem);
	pRibPlateDlg->RedrawSketchDrawing(pItem);
	//ѡ�ж�λ������ǰ������Ƶ��߰�
	if(hCurPart==0)
	{
		CLDSParamPlate *pRibPlate=pRibPlateDlg->GetRibPlate();
		if(pRibPlate)
			hCurPart=pRibPlate->handle;
	}
	//��Ҫ����ѡ��״̬�Ĺ����Ѵ���ѡ��״̬ʱ������������ѡ��״̬
	long *id_arr=NULL;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(!(n==1&&id_arr[0]==hCurPart)&&hCurPart>0x20)
	{	//
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(hCurPart);
	}
	return TRUE;
}
const DWORD HASHTABLESIZE = 500;
CHashStrList<PROPLIST_ITEM>  CDesignRibbedPlateDlg::propHashtable;
void CDesignRibbedPlateDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CXhChar100 propCmbStr,propName,propHelp;
#ifdef AFX_TARG_ENU_ENGLISH
	//////////////////////////////////////////////////////////////////////////
	//������Ϣ
	//////////////////////////////////////////////////////////////////////////
	AddPropItem("Basic",PROPLIST_ITEM(id++,"Basic info","plate's essential information"));
	//���
	AddPropItem("RibPlate.sPartNo",PROPLIST_ITEM(id++,"Nmuber","plate number"));
	//�κ�
	AddPropItem("RibPlate.nSegI",PROPLIST_ITEM(id++,"segment number","segment number of plate"));
	//����
	propCmbStr.Printf("%s",MakeMaterialMarkSetString().GetString());
	AddPropItem("RibPlate.cMaterial",PROPLIST_ITEM(id++,"material","plate material",propCmbStr));
	//���
	AddPropItem("RibPlate.nThick",PROPLIST_ITEM(id++,"Thickness","Thickness of  plate"));
	//�߰�����
	propCmbStr.Copy("0.Angle Rib Plate|1.Slot Rib Plate|2.Angle Steel Rib Plate|3.Plate Rib Plate|4.Tube Rib Plate|");
	AddPropItem("RibPlate.m_cPlateType",PROPLIST_ITEM(id++,"rib plate type","Type of rib plate",propCmbStr));
	//////////////////////////////////////////////////////////////////////////
	//������Ϣ
	//////////////////////////////////////////////////////////////////////////
	AddPropItem("RelaInfo",PROPLIST_ITEM(id++,"relative info","relative information"));
	//��׼����
	AddPropItem("m_pDatumPart",PROPLIST_ITEM(id++,"datum part","plate's positioning datum part"));
	//��������1
	AddPropItem("m_pRelaPart1",PROPLIST_ITEM(id++,"assistant part","plate positioning assistant part"));
	//��������2
	AddPropItem("m_pRelaPart2",PROPLIST_ITEM(id++,"assistant part 2","plate positioning assistant part2"));
	//��׼�ڵ�	
	AddPropItem("m_pDatumNode",PROPLIST_ITEM(id++,"Datum node","plate positioning Datum node"));
	//��˨��λ
	AddPropItem("m_pDatumBolt",PROPLIST_ITEM(id++,"positioning bolt","to determine RibPlate datum points"));
	//���򹹼�
	propHelp.Copy("parts (bolts or rods)used to determine rib plate's extending direction.");
	AddPropItem("m_pVecPart",PROPLIST_ITEM(id++,"Rib plate's parts in X-axis direction",propHelp));	
	//////////////////////////////////////////////////////////////////////////
	//����ϵ
	//////////////////////////////////////////////////////////////////////////
	AddPropItem("Position",PROPLIST_ITEM(id++,"Assembling Position","Assembling position parameter"));
	AddPropItem("preview",PROPLIST_ITEM(id++,"preview","",""));
	//��������
	AddPropItem("axis_z",PROPLIST_ITEM(id++,"normal direction","rib plate's normal direction"));
	AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Working normal","Working normal",CStdVector::MakeVectorSetStr()));
	//����x\y\z����
	AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X","Normal X component"));	
	AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y","Normal Y component"));
	AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z","Normal Z component"));
	//չ������
	AddPropItem("axis_x",PROPLIST_ITEM(id++,"Spreading direction","Rib plate's spreading direction"));
	AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"Rib plate axis Y","Rib plate axis Y",CStdVector::MakeVectorSetStr()));
	//չ������x\y\z����
	AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"",""));
	AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"",""));
	AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"",""));
	//////////////////////////////////////////////////////////////////////////
	//��׼����Ϊ�ֹ�ʱʹ�ò���
	//////////////////////////////////////////////////////////////////////////
	//��׼�� (0.������ 1.��׼�� 2.ָ������)
	propHelp.Copy("the way to determine plate's extend vector");
	propCmbStr.Copy("0.Datum Edge|1.Welding Line|2.Specify");
	AddPropItem("RibPlate.tube.iDatumEdge",PROPLIST_ITEM(id++,"Extend Vector Defined Standard",propHelp,propCmbStr));
	//���߶���
	propHelp.Copy("the way to determine plate's normal");
	propCmbStr.Copy("0.Perpendicular Tube Along Vector|1.Tube Axis Vector|2.Specify");
	AddPropItem("RibPlate.tube.iWorkNormStyle",PROPLIST_ITEM(id++,"Normal defined standard",propHelp,propCmbStr));
	//��ʼƫת�Ƕ�
	propHelp.Copy("offset angle between the first plate and datum edge.");
	AddPropItem("RibPlate.tube.fInitOffsetAngle",PROPLIST_ITEM(id++,"Initial offset angle",propHelp));
	//X+���򶥲��߽�����
	propHelp.Copy("rib plate's top edge angle in X+ direction type��define the appearance of plate");
	propCmbStr.Copy("0.Right-Angle|1.Chamfer Angle|2.Arc");
	AddPropItem("RibPlate.tube.cLeftTopCornerType",PROPLIST_ITEM(id++,"type of the top edge angle in X+ direction",propHelp,propCmbStr));
	//X+�нǿ��/�л��뾶
	propHelp.Copy("width of top cutting arc in X+ direction");
	AddPropItem("RibPlate.tube.fLeftTopCornerWidth",PROPLIST_ITEM(id++,"cutting angle width",propHelp));	
	propHelp.Copy("height of top cutting arc in X+ direction");
	AddPropItem("RibPlate.tube.fLeftTopCornerHeight",PROPLIST_ITEM(id++,"cutting angle height",propHelp));	
	//X-���򶥲��߽�����
	propHelp.Copy("rib plate's top edge angle type in X- direction,define the appearance of plate");
	propCmbStr.Copy("0.Right-Angle|1.Chamfer Angle|2.Arc");
	AddPropItem("RibPlate.tube.cRightTopCornerType",PROPLIST_ITEM(id++,"type of the top edge angle in X- direction",propHelp,propCmbStr));
	//X-�нǿ��/�л��뾶
	propHelp.Copy("width of top cutting angle in X- direction");
	AddPropItem("RibPlate.tube.fRightTopCornerWidth",PROPLIST_ITEM(id++,"cutting angle width",propHelp));	
	propHelp.Copy("height of top cutting angle in X- direction");
	AddPropItem("RibPlate.tube.fRightTopCornerHeight",PROPLIST_ITEM(id++,"cutting angle height",propHelp));	
	//�ظֹ�����ƫ�ƾ���M
	propHelp.Copy("axial offset distance in tube direction between plate coordinate system origin and datum point.");
	AddPropItem("RibPlate.tube.fOffsetDistM",PROPLIST_ITEM(id++,"axial offset distance M",propHelp));
	//X����ƫ�ƾ���
	propHelp.Copy("offset distance of plate's coordinate system origin from default location (pass datum point, the intersection point with  tube surface in Y axis direction) in X axis direction");
	AddPropItem("RibPlate.tube.fOffsetDistX",PROPLIST_ITEM(id++,"offset in axis X",propHelp));
	//�ֹ��߰����
	AddPropItem("RibPlate.tube.nPlateNum",PROPLIST_ITEM(id++,"number of plates","number of plates"));
	//ƫ�ƽǶ�
	propHelp.Copy("offset angle between plate and its adjoin plate");
	AddPropItem("RibPlate.tube.fOffsetAngle",PROPLIST_ITEM(id++,"offset angle",""));
	//��ֱ�Ҵ��Զ���λ���߰�
	propHelp.Copy("Rib plate is in the verticality of supplementary and automatically normotopia");
	propCmbStr.Copy("Yes|No");
	AddPropItem("RibPlate.tube.bVerticalPlate",PROPLIST_ITEM(id++,"Vertical and automatically normotopia",propHelp,propCmbStr));
	//
	AddPropItem("RibPlate.tube.bAutoCalB",PROPLIST_ITEM(id++,"","",""));
	AddPropItem("RibPlate.tube.margin",PROPLIST_ITEM(id++,"","",""));
	//�����⻷�뾶
	propHelp.Copy("The edge connected with supplementary plate 2  automatically extends to the input ring radius.");
	AddPropItem("RibPlate.tube.fTopOutterR",PROPLIST_ITEM(id++,"top rign radius",propHelp));
	//�ײ��⻷�뾶
	propHelp.Copy("edge connected with supplymentary plate 1  automatically extends to input ring radius.");
	AddPropItem("RibPlate.tube.fBottomOutterR",PROPLIST_ITEM(id++,"bottom ring radius",propHelp));
	//�����췽��
	propHelp.Copy("plate axis X direction, the plate used to locate extending direction rapidly.");
	AddPropItem("RibPlate.tube.extend_vec",PROPLIST_ITEM(id++,"plate's axis X",propHelp,CStdVector::MakeVectorSetStr()));
	//////////////////////////////////////////////////////////////////////////
	//��׼����Ϊ�ְ�ʱʹ�ò���
	//////////////////////////////////////////////////////////////////////////
	//�ڻ�׼�ְ嶥��һ��
	propHelp.Copy("Rib plate lies at the top side of the datum plate");
	propCmbStr.Copy("Yes|No");
	AddPropItem("RibPlate.plate.bOnDatumPlateTopFace",PROPLIST_ITEM(id++,"At the front of datum plate",propHelp,propCmbStr));	
	//��׼λ��
	propHelp.Copy("specify rib plate's coordinate system origin, input the relative coordinate in datum plate's coordinate system");
	propCmbStr.Copy("0.Specify Datum Bolt|1.Input Directly");
	AddPropItem("RibPlate.plate.bSpecOrg",PROPLIST_ITEM(id++,"datum location",propCmbStr));		//ָ������ԭ����ָ����λ��׼������ѡһ
	//��׼λ��x/y����
	AddPropItem("RibPlate.plate.fOriginX",PROPLIST_ITEM(id++,"X","plate coordinate system origin X coordinate component"));
	AddPropItem("RibPlate.plate.fOriginY",PROPLIST_ITEM(id++,"Y","plate coordinate system origin Y coordinate component"));
	//�ְ��߰�ָ��X�᷽����ָ����������˼���ѡһ
	propHelp.Copy("specify rib plate coordinate system X axis direction, input the relative coordinate in datum plate's coordinate system");
	propCmbStr.Copy("0.Calculate Automatically|1.Input Directly");
	AddPropItem("RibPlate.plate.bSpecAxisX",PROPLIST_ITEM(id++,"X axis",propHelp,propCmbStr));
	propCmbStr.Copy("0.X+|1.X-|2.Y+|3.Y-|4.Input...|5.Calculate Automatically");
	AddPropItem("RibPlate.plate.iAxisXType",PROPLIST_ITEM(id++,"X axis",propHelp,propCmbStr));
	//�߰�X�᷽��(��׼�ְ�����ϵ�µ��������)
	propHelp.Copy("plate X coordinate component in X axis direction,the relative direction in datum plate's coordinate system");
	AddPropItem("RibPlate.plate.fXAxisX",PROPLIST_ITEM(id++,"X",propHelp));
	//�߰�Y�᷽��(��׼�ְ�����ϵ�µ��������)
	propHelp.Copy("plate Y coordinate component in X axis direction,the relative direction in datum plate's coordinate system");
	AddPropItem("RibPlate.plate.fXAxisY",PROPLIST_ITEM(id++,"Y",propHelp));
	//
	propHelp.Copy("determine axis Y direction, through axis Y direction rod or the inclination angle��degree measure)input.");
	propCmbStr.Copy("0.Direction Rod|1.Inclination Angle");
	AddPropItem("RibPlate.plate.bSpecAxisY",PROPLIST_ITEM(id++,"Y axis",propHelp,propCmbStr));
	//��װ���б�Ƕȣ���Y����װ����X�����ת���뷽���׼�˼���ѡһ��
	propHelp.Copy("plate's inclination angle(degree measure), datum plate's axis Z direction is the initial direction, determine Y axis direction through rotating specified degree in X axis direction");
	AddPropItem("RibPlate.plate.fGradientDegAngle",PROPLIST_ITEM(id++,"plate's inclination angle",propHelp));
	//�ְ����췽���뷽��˼�ͬ��
	propHelp.Copy("Rib plate axis Y is in the same direction with direction rod.");
	propCmbStr.Copy("Yes|No");
	AddPropItem("RibPlate.plate.bAlongVecPart",PROPLIST_ITEM(id++,"In the same direction with direction rod",propHelp,propCmbStr));	
	//λ�ڸ����ְ�����
	propHelp.Copy("Rib plate is at the top side of supplementary plate.");
	propCmbStr.Copy("Yes|No");
	AddPropItem("RibPlate.bOnRelaPlateTopFace",PROPLIST_ITEM(id++,"at the front of supplementary plate",propHelp,propCmbStr));
	//X����ƫ�ƾ���
	propHelp.Copy("offset distance in X axis direction between plate coordinate origin and datum point");
	AddPropItem("RibPlate.plate.fOffsetDistX",PROPLIST_ITEM(id++,"offset distance in axis X",propHelp));
	//Z����ƫ�ƾ���
	propHelp.Copy("offset distance in Z axis direction between plate coordinate origin and datum point");
	AddPropItem("RibPlate.plate.fOffsetDistZ",PROPLIST_ITEM(id++,"offset distance in axis Z",propHelp));
	//////////////////////////////////////////////////////////////////////////
	//���β���
	//////////////////////////////////////////////////////////////////////////
	AddPropItem("Profile",PROPLIST_ITEM(id++,"profile design","configuration outline design parameter"));
	//�Զ�������������Ե
	propHelp.Copy("Automatically extend to the edge of supplementary plate.");
	AddPropItem("RibPlate.bAutoExtendOutterEdge",PROPLIST_ITEM(id++,"edge extending",propHelp,"Yes|No"));
	//�������
	propHelp.Copy("Width of rib plate, that is, the length of edge connected with supplementary plate 2, initial value automatically extends to the outer edge of supplementary plate 2.");
	AddPropItem("RibPlate.fTopWidth",PROPLIST_ITEM(id++,"Top width",propHelp));
	//�ײ����
	propHelp.Copy("bottom width,that is, length of edge connected with supplementary plate, initial value automatically extends to the outer edge of supplementary plate.");
	AddPropItem("RibPlate.fBottomWidth",PROPLIST_ITEM(id++,"bottom width B",propHelp));
	//����(Ϊ���ְ�а�ʱ�ó����Զ�����)
	propHelp.Copy("the length of edge connected with datum part, initial value automatically extends to the outer edge of datum plate.");
	AddPropItem("RibPlate.fLength",PROPLIST_ITEM(id++,"rib plate length A",propHelp));			
	//̨�׸߶�
	AddPropItem("RibPlate.fHeight",PROPLIST_ITEM(id++,"Parameter D","Top width"));			
	//�߰�Բ���뾶DR
	AddPropItem("RibPlate.fCirR",PROPLIST_ITEM(id++,"�߰�Բ���뾶DR","�߰�Բ���뾶DR��"));
	//�нǿ��
	AddPropItem("RibPlate.fCutAngleWidth",PROPLIST_ITEM(id++,"Parameter E","cut angle width of rib plate."));
	AddPropItem("RibPlate.fCutAngleWidth2",PROPLIST_ITEM(id++,"Parameter E","cut angle width of rib plate."));
	AddPropItem("RibPlate.MF",PROPLIST_ITEM(id++,"Hole distance F",""));
	AddPropItem("RibPlate.MG",PROPLIST_ITEM(id++,"Hole distance G",""));
	AddPropItem("RibPlate.MH",PROPLIST_ITEM(id++,"Hole diameter H",""));
	//ͬʱ��������߰�
	propHelp.Copy("simultaneously generating rib plate on the other side of auxiliary plate is supported.");
	propCmbStr.Copy("Yes|No");
	AddPropItem("RibPlate.bCreateOuterPlate",PROPLIST_ITEM(id++,"generate rib plate On the other side",propHelp,propCmbStr));
	//����߰峤��
	propHelp.Copy("length of outside rib plate, that is, the length of the edge connected with datum plate.");
	AddPropItem("RibPlate.fOuterPlateLen",PROPLIST_ITEM(id++,"length of outside rib plate",propHelp));	
	//����߰�߶�
	propHelp.Copy("height of outside rib plate, that is, the length of the edge opposite to datum plate");
	AddPropItem("RibPlate.fOuterPlateHeight",PROPLIST_ITEM(id++,"height of outside rib plate",propHelp));
#else
	//��������
	AddPropItem("Basic",PROPLIST_ITEM(id++,"������Ϣ","�߰��������"));
	AddPropItem("RibPlate.sPartNo",PROPLIST_ITEM(id++,"���","�߰���"));
	AddPropItem("RibPlate.nSegI",PROPLIST_ITEM(id++,"�κ�","�߰�κ�"));
	propCmbStr.Printf("%s",MakeMaterialMarkSetString().GetBuffer());
	AddPropItem("RibPlate.cMaterial",PROPLIST_ITEM(id++,"����","�߰����",propCmbStr));
	AddPropItem("RibPlate.nThick",PROPLIST_ITEM(id++,"���","�߰���"));
	propCmbStr.Copy("0.���߰�|1.���߰�|2.�Ǹ��߰�|3.�ְ��߰�|4.�ֹ��߰�|");
	AddPropItem("RibPlate.m_cPlateType",PROPLIST_ITEM(id++,"�߰�����","�߰�����",propCmbStr));
	//������Ϣ
	AddPropItem("RelaInfo",PROPLIST_ITEM(id++,"������Ϣ","�����߰���Ϣ"));
	AddPropItem("m_pDatumPart",PROPLIST_ITEM(id++,"��׼����","�߰嶨λ��׼����"));
	AddPropItem("m_pRelaPart1",PROPLIST_ITEM(id++,"��������","�߰嶨λ��������"));
	AddPropItem("m_pRelaPart2",PROPLIST_ITEM(id++,"��������2","�߰嶨λ��������2"));	
	AddPropItem("m_pDatumNode",PROPLIST_ITEM(id++,"��׼�ڵ�","�߰嶨λ��׼�ڵ�"));
	AddPropItem("m_pDatumBolt",PROPLIST_ITEM(id++,"��˨��λ","ȷ���߰嶨λ�������˨"));
	AddPropItem("m_pVecPart",PROPLIST_ITEM(id++,"�߰�X�᷽�򹹼�","���������߰����췽��Ĺ���(��˨��˼�)"));
	//����ϵ
	AddPropItem("Position",PROPLIST_ITEM(id++,"װ�䷽λ","װ�䷽λ����"));
	AddPropItem("axis_z",PROPLIST_ITEM(id++,"���߷���","�߰巨�߷���"));
	AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"��������","��������",CStdVector::MakeVectorSetStr()));
	AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X����","����X����"));
	AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y����","����Y����"));
	AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z����","����Z����"));
	AddPropItem("axis_x",PROPLIST_ITEM(id++,"չ������","�߰�չ������"));
	AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"�߰�Y�᷽��","�߰�Y�᷽��",CStdVector::MakeVectorSetStr()));
	AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X����","չ��X����"));
	AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y����","չ��Y����"));
	AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z����","չ��Z����"));
	AddPropItem("preview",PROPLIST_ITEM(id++,"Ԥ������","֧��Ԥ������","Ԥ��"));
	//��׼����Ϊ�ֹ�ʱʹ�ò���
	AddPropItem("RibPlate.tube.iDatumEdge",PROPLIST_ITEM(id++,"��λ��׼��","�ְ嶨λ��׼��","0.��׼��|1.������"));
	AddPropItem("RibPlate.tube.iWorkNormStyle",PROPLIST_ITEM(id++,"����������","�ְ幤�����ߵĶ����׼��ʽ","0.�ֹ�����|1.�ֹܽ���|2.ָ������"));
	AddPropItem("RibPlate.tube.fInitOffsetAngle",PROPLIST_ITEM(id++,"��ʼƫת��","��һ��ְ����׼��֮���ƫ�ƽǶȡ�"));
	AddPropItem("RibPlate.tube.cLeftTopCornerType",PROPLIST_ITEM(id++,"X-�򶥲��߽�����","�߰�X-�򶥲��߽����ͣ�ȷ��������Ρ�","0.ֱ��|1.����|2.Բ��"));
	AddPropItem("RibPlate.tube.fLeftTopCornerWidth",PROPLIST_ITEM(id++,"X���н���","X-�򶥲��нǿ��"));	
	AddPropItem("RibPlate.tube.fLeftTopCornerHeight",PROPLIST_ITEM(id++,"Y���н���","X-�򶥲��нǸ߶�"));	
	AddPropItem("RibPlate.tube.cRightTopCornerType",PROPLIST_ITEM(id++,"X+�򶥲��߽�����","�߰�X-�򶥲��߽����ͣ�ȷ��������Ρ�","0.ֱ��|1.����|2.Բ��"));
	AddPropItem("RibPlate.tube.fRightTopCornerWidth",PROPLIST_ITEM(id++,"X���н���","X+�򶥲��нǿ��"));	
	AddPropItem("RibPlate.tube.fRightTopCornerHeight",PROPLIST_ITEM(id++,"Y���н���","X+�򶥲��нǸ߶�"));	
	AddPropItem("RibPlate.tube.fOffsetDistM",PROPLIST_ITEM(id++,"����ƫ�ƾ���M","�ְ�����ϵԭ�����׼��֮���ظֹܵ�����ƫ�ƾ��롣"));
	AddPropItem("RibPlate.tube.fOffsetDistX",PROPLIST_ITEM(id++,"X�᷽��ƫ�ƾ���","�ְ�����ϵԭ����Ĭ��λ��(����׼��Y��������ܱڽ���)�ظְ�X���ƫ�ƾ��롣"));
	AddPropItem("RibPlate.tube.nPlateNum",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("RibPlate.tube.fOffsetAngle",PROPLIST_ITEM(id++,"���ƫת��","�ְ������ڸְ�֮���ƫת�Ƕȡ�"));
	AddPropItem("RibPlate.tube.bVerticalPlate",PROPLIST_ITEM(id++,"��ֱ���Զ���λ","�߰崹ֱ�ڸ����ְ����Զ���λ��","��|��"));
	AddPropItem("RibPlate.tube.bAutoCalB",PROPLIST_ITEM(id++,"�Զ����쵽��Ե","�߰�װ��Զ����쵽�װ�ı�Ե","��|��"));
	AddPropItem("RibPlate.tube.margin",PROPLIST_ITEM(id++,"�ױ���������","ָ����װ��Ե�ľ���",""));
	AddPropItem("RibPlate.tube.fTopOutterR",PROPLIST_ITEM(id++,"�������ΰ뾶","�븨���ְ�2�����ı��Զ�����������Ļ��ΰ뾶����"));
	AddPropItem("RibPlate.tube.fBottomOutterR",PROPLIST_ITEM(id++,"�ײ����ΰ뾶","�븨���ְ�1�����ı��Զ�����������Ļ��ΰ뾶����"));
	AddPropItem("RibPlate.tube.extend_vec",PROPLIST_ITEM(id++,"�ְ�X�᷽��","�ְ�X�᷽�����ڿ��ٶ�λ������չ����ĸְ�",CStdVector::MakeVectorSetStr()));
	//��׼����Ϊ�ְ�ʱʹ�ò���
	AddPropItem("RibPlate.plate.bOnDatumPlateTopFace",PROPLIST_ITEM(id++,"λ�ڻ�������","�߰��ڻ�׼�ְ嶥��һ��","��|��"));
	AddPropItem("RibPlate.plate.bSpecOrg",PROPLIST_ITEM(id++,"��׼λ��","ָ���߰�����ϵԭ��λ�ã���������Ϊ��׼�ְ�����ϵ�µ��������","0.��˨��λ|1.ֱ������"));
	AddPropItem("RibPlate.plate.fOriginX",PROPLIST_ITEM(id++,"X","�ְ�����ϵԭ��X�������"));
	AddPropItem("RibPlate.plate.fOriginY",PROPLIST_ITEM(id++,"Y","�ְ�����ϵԭ��Y�������"));
	propCmbStr.Copy("0.X+|1.X-|2.Y+|3.Y-|4.����...|5.�Զ�����");
	AddPropItem("RibPlate.plate.iAxisXType",PROPLIST_ITEM(id++,"X�᷽��","ָ���߰�����ϵX�᷽��",propCmbStr));
	AddPropItem("RibPlate.plate.bSpecAxisX",PROPLIST_ITEM(id++,"X�᷽��","ָ���߰�����ϵX�᷽����������Ϊ��׼�ְ�����ϵ�µ��������","0.���򹹼�|1.ֱ������"));
	AddPropItem("RibPlate.plate.fXAxisX",PROPLIST_ITEM(id++,"X","�ְ�X�᷽��X�����������׼�ְ�����ϵ�µ���Է���"));
	AddPropItem("RibPlate.plate.fXAxisY",PROPLIST_ITEM(id++,"Y","�ְ�X�᷽��Y�����������׼�ְ�����ϵ�µ���Է���"));	
	AddPropItem("RibPlate.plate.bSpecAxisY",PROPLIST_ITEM(id++,"Y�᷽��","ͨ��Y�᷽��˼����������б�Ƕ�(�Ƕ���)��ȷ��Y�᷽��","0.����˼�|1.��б�Ƕ�"));		
	AddPropItem("RibPlate.plate.fGradientDegAngle",PROPLIST_ITEM(id++,"�ְ���б�Ƕ�","�ְ���б�Ƕ�(�Ƕ���),�Ի�׼�ְ�Z�᷽��Ϊ��ʼ������X����תָ���Ƕ�ȷ��Y�᷽��"));	
	AddPropItem("RibPlate.plate.bAlongVecPart",PROPLIST_ITEM(id++,"�뷽��˼�ͬ��","�߰�Y�᷽���뷽��˼�ͬ��","��|��"));
	AddPropItem("RibPlate.plate.fOffsetDistX",PROPLIST_ITEM(id++,"X��ƫ�Ʀ�x","�ְ�����ϵԭ�����׼��֮���X�᷽���ϵ�ƫ�ƾ���"));
	AddPropItem("RibPlate.plate.fOffsetDistZ",PROPLIST_ITEM(id++,"Z��ƫ�Ʀ�z","�ְ�����ϵԭ�����׼��֮���Z�᷽���ϵ�ƫ�ƾ���"));
	//��չ�����߰幦�ܺ���������� wjh-2016.5.4
	AddPropItem("RibPlate.plate.IsHasBendPlane",PROPLIST_ITEM(id++,"������","","0.��������|1.��������"));		
	AddPropItem("RibPlate.plate.BendPlaneTip",PROPLIST_ITEM(id++,"ĩ�˶�λ��","","0.��˨��λ|1.ֱ������"));		
	AddPropItem("RibPlate.plate.BendTipPointX",PROPLIST_ITEM(id++,"������ĩ��X","������ĩ�˸��������",""));		
	AddPropItem("RibPlate.plate.BendTipPointY",PROPLIST_ITEM(id++,"������ĩ��Y","������ĩ�˸��������",""));		
	AddPropItem("RibPlate.plate.BendPlaneTipBolt",PROPLIST_ITEM(id++,"��˨��λ","������ĩ����˨��λ",""));		
	AddPropItem("RibPlate.plate.BendDirect",PROPLIST_ITEM(id++,"����������","","0.���ܶ���|1.�ظ˼�����|2.��˨��������|3.���������"));		
	AddPropItem("RibPlate.plate.BendDirectVecPart",PROPLIST_ITEM(id++,"����������˨","������ĩ�˸���������˨",""));		
	AddPropItem("RibPlate.plate.BendDirectPoint",PROPLIST_ITEM(id++,"���������","������ĩ�˸��������",""));		
	AddPropItem("RibPlate.plate.BendDirectPointX",PROPLIST_ITEM(id++,"���������X","������ĩ�˸��������",""));		
	AddPropItem("RibPlate.plate.BendDirectPointY",PROPLIST_ITEM(id++,"���������Y","������ĩ�˸��������",""));		
	AddPropItem("RibPlate.plate.fBendTipOffsetX",PROPLIST_ITEM(id++,"X��ƫ�Ʀ�x","�ְ�����ϵԭ�����׼��֮���X�᷽���ϵ�ƫ�ƾ���"));
	AddPropItem("RibPlate.plate.fBendTipOffsetZ",PROPLIST_ITEM(id++,"Z��ƫ�Ʀ�z","�ְ�����ϵԭ�����׼��֮���Z�᷽���ϵ�ƫ�ƾ���"));
	//�������	
	AddPropItem("Profile",PROPLIST_ITEM(id++,"�������","����������Ʋ���"));		
	AddPropItem("RibPlate.bOnRelaPlateTopFace",PROPLIST_ITEM(id++,"λ�ڸ�������","�߰��ڸ����ְ嶥��һ�ࡣ","��|��"));
	AddPropItem("RibPlate.bAutoExtendOutterEdge",PROPLIST_ITEM(id++,"��Ե����","�Զ����쵽�����ְ��Ե","��|��"));
	AddPropItem("RibPlate.fTopWidth",PROPLIST_ITEM(id++,"�������","�߰嶥�����,���븨���ְ�2�����ߵĵı߳�,��ʼֵ�Զ������������ְ�2������Ե��"));
	AddPropItem("RibPlate.fBottomWidth",PROPLIST_ITEM(id++,"�ײ����B","�ײ����,���븨���ְ������ߵı߳�,��ʼֵ�Զ����쵽�����ְ�����Ե��"));
	AddPropItem("RibPlate.fLength",PROPLIST_ITEM(id++,"�߰峤��A","���׼���������ߵı߳�,��ʼֵ�Զ���������׼�ְ������Ե��"));
	AddPropItem("RibPlate.fCirR",PROPLIST_ITEM(id++,"�߰�Բ���뾶DR","�߰�Բ���뾶DR��"));
	AddPropItem("RibPlate.fHeight",PROPLIST_ITEM(id++,"����C","̨�׸߶�"));
	AddPropItem("RibPlate.iCutAngleMode",PROPLIST_ITEM(id++,"����ģʽ","����ģʽ","0.�н�|1.�л�"));
	AddPropItem("RibPlate.fCutAngleWidth",PROPLIST_ITEM(id++,"����E","�߰��нǿ��"));
	AddPropItem("RibPlate.fCutAngleWidth2",PROPLIST_ITEM(id++,"C�൹��ֵ","�߰��ڸ����ְ�2���нǿ��"));
	AddPropItem("RibPlate.MF",PROPLIST_ITEM(id++,"�׾�F","�����׾�ױ߿׾�"));
	AddPropItem("RibPlate.MG",PROPLIST_ITEM(id++,"�׾�G","�����׾��߿׾�"));
	AddPropItem("RibPlate.MH",PROPLIST_ITEM(id++,"�׾�H","�����׿׾�"));
	AddPropItem("RibPlate.bCreateOuterPlate",PROPLIST_ITEM(id++,"������һ���߰�","֧��ͬʱ���ɸ����ְ���һ����߰�","��|��"));
	AddPropItem("RibPlate.fOuterPlateLen",PROPLIST_ITEM(id++,"����߰峤��","����߰峤��,�����׼�ְ������ߵĳ��ȡ�"));
	AddPropItem("RibPlate.fOuterPlateHeight",PROPLIST_ITEM(id++,"����߰�߶�","����߰�߶�,�����׼�ְ���Աߵĳ���"));
#endif
}

int CDesignRibbedPlateDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
UINT CDesignRibbedPlateDlg::_InitWaterHoleRadiusByThick()
{
	CLDSLinePart* pRelyRod=m_pDatumPart->IsLinePart()?m_pDatumPart.pRod:NULL;
	if(pRelyRod==NULL)
		return 25;
	if(pRelyRod->size_thick<=10)
		return 25;	//�ں�5mm��t��10mmʱ�� ����Բ���뾶R��25mm
	else if(pRelyRod->size_thick<=15)
		return 30;	//�ں�10mm��t��15mmʱ������Բ���뾶R��30mm
	else if(pRelyRod->size_thick<=20)
		return 35;	//�ں�15mm��t��20mmʱ������Բ���뾶R��35mm
	else //if(pRelyRod->size_thick<=25)
		return 40;	//�ں�20mm��t��25mmʱ������Բ���뾶R��40mm
}
void CDesignRibbedPlateDlg::InitRibPlatePara()
{	//��ʼ���ְ�����
	if(!m_bNewRibPlate)
		return;
	m_xRibPlate.nSegI=m_pDatumPart->iSeg;
	m_xRibPlate.cMaterial=m_pDatumPart->cMaterial;
	if(m_pDatumPart->IsPlate())
	{
		if(m_pDatumBolt==NULL)
		{
			m_xRibPlate.plate.bSpecOrg=TRUE;
			if(m_xRibPlate.m_cPlateType==3&&m_xRibPlate.plate.anchors[0].ciType==3)
			{
				m_xRibPlate.plate.fOriginX=m_xRibPlate.plate.anchors[0].locate.vertex.x;
				m_xRibPlate.plate.fOriginY=m_xRibPlate.plate.anchors[0].locate.vertex.y;
			}
		}
		if(m_xRibPlate.m_cPlateType==3)
		{
			if(!m_pVecPart.IsNULL())
				m_xRibPlate.plate.bSpecAxisX=FALSE;	//�ظ˼�����
			else
				m_xRibPlate.plate.bSpecAxisX=TRUE;	
		}
		if(m_xRibPlate.m_cPlateType==0||m_xRibPlate.m_cPlateType==1)
		{
			if(!m_pVecPart.IsNULL())
				m_xRibPlate.plate.bSpecAxisY=FALSE;	//�ظ˼�����
			else
				m_xRibPlate.plate.bSpecAxisY=TRUE;
			if(m_xRibPlate.plate.bSpecAxisX)
			{	//��ʼ��X�᷽��
				f3dPoint axisX=m_xRibPlate.bOnRelaPlateTopFace?m_pRelaPart1->ucs.axis_z:-m_pRelaPart1->ucs.axis_z;
				vector_trans(axisX,m_pDatumPart->ucs,FALSE);
				if(fabs(axisX.x)>fabs(axisX.y))
				{
					m_xRibPlate.plate.iAxisXType=0;
					m_xRibPlate.plate.fXAxisX=1;
					m_xRibPlate.plate.fXAxisY=0;
				}
				else
				{
					m_xRibPlate.plate.iAxisXType=2;
					m_xRibPlate.plate.fXAxisX=0;
					m_xRibPlate.plate.fXAxisY=1;
				}
			}
		}
		//}
		//�ж��߰�λ��(�ڻ�׼�ְ�����ְ嶥�滹�ǵ���)
		//if(m_pDatumPart->IsPlate())
		//{
		if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->IsPlate())
		{
			PROFILE_VERTEX *pVertex=NULL;
			if(m_pRelaPart1->GetClassTypeId()==CLS_PLATE)
				pVertex=m_pRelaPart1.PlatePointer()->vertex_list.GetFirst();
			else //if(m_pRelaPart1->GetClassTypeId()==CLS_PARAMPLATE)
				pVertex=m_pRelaPart1.ParamPlatePointer()->vertex_list.GetFirst();
			if(pVertex)
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,m_pRelaPart1->ucs,TRUE);
				coord_trans(vertex,m_pDatumPart->ucs,FALSE);
				if(vertex.z<0)
					m_xRibPlate.plate.bOnDatumPlateTopFace=FALSE;
				else
					m_xRibPlate.plate.bOnDatumPlateTopFace=TRUE;
			}
			if(m_bNewRibPlate&&m_pRelaPart2.IsHasPtr()&&m_pRelaPart2->IsPlate())
			{	//�½������߰�ʱĬ��Ϊ�Զ�����X��
				m_xRibPlate.plate.iAxisXType=5;
			}
		}
	}
	else if(m_pDatumPart->IsTube())
	{
		if(m_pDatumNode)	//�ֹ������߰�Ĭ���ظֹ�����
			m_xRibPlate.bOnRelaPlateTopFace=TRUE;
	}
	if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==2)
	{
		m_xRibPlate.tube.margin=m_xRibPlate.params.elbow.margin.iValue;
		m_xRibPlate.tube.bAutoCalB=(m_xRibPlate.tube.margin>=0)?TRUE:FALSE;
	}
	AutoCalRibPlateWidth();
	if(m_pRibPlate==NULL)
		m_pRibPlate=CreateRibPlate();
}
BOOL CDesignRibbedPlateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_sWndTitle.GetLength()>0)
		SetWindowText(m_sWndTitle);
	//��ʼ����˨�б��Լ�����ID
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xSubPropList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	InitPropHashtable();	//��ʼ�����Թ�ϣ��
	SetCallBackFunc(FireCallBackCommand);
	if(!m_bInernalStart)
	{
		if(!m_bNewRibPlate)
		{
			if(m_pRibPlate==NULL||m_pRibPlate->m_iParamType!=TYPE_RIBPLATE)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("You haven't choose the right rib plate!");
#else
				AfxMessageBox("δѡ����ȷ���߰壡");
#endif
				return FALSE;
			}
			//��ʼ����λ����
			long hDatumPart=0,hNode=0,hDatumBolt=0,hRelaPart1=0,hRelaPart2=0,hVecPart=0;
			LoadParamsFrom(m_pRibPlate);	//���ز������߰�ĸ�����Ʋ���
			m_pRibPlate->GetDesignItemValue('A',&hDatumPart);
			m_pRibPlate->GetDesignItemValue('B',&hNode);
			m_pRibPlate->GetDesignItemValue('C',&hRelaPart1);
			m_pRibPlate->GetDesignItemValue('D',&hRelaPart2);
			m_pRibPlate->GetDesignItemValue('O',&hDatumBolt);
			m_pRibPlate->GetDesignItemValue('V',&hVecPart);
			m_pDatumPart=console.FromPartHandle(abs(hDatumPart));
			m_pDatumNode=console.FromNodeHandle(hNode);
			m_pDatumBolt=(CLDSBolt*)console.FromPartHandle(hDatumBolt,CLS_BOLT);
			m_pRelaPart1=console.FromPartHandle(abs(hRelaPart1));
			m_pRelaPart2=console.FromPartHandle(abs(hRelaPart2));
			m_pVecPart=console.FromPartHandle(abs(hVecPart));
			StrCpy(m_xRibPlate.sPartNo,m_pRibPlate->GetPartNo());
			m_xRibPlate.nThick=(int)m_pRibPlate->GetThick();
			m_xRibPlate.cMaterial=m_pRibPlate->cMaterial;
			m_xRibPlate.nSegI=m_pRibPlate->iSeg;
			if(m_pRelaPart1.IsNULL()&&m_pRelaPart1.IsNULL())
			{
				if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
					m_xRibPlate.bOnRelaPlateTopFace=(hDatumPart>0);
				else 
					m_xRibPlate.plate.bOnDatumPlateTopFace=(hDatumPart>0);
				//
				if(m_pVecPart.IsHasPtr()&&m_pVecPart->IsLinePart())
					m_xRibPlate.plate.bAlongVecPart=(hVecPart>0);	
			}
			else if(m_pRelaPart1.IsHasPtr())
			{
				m_xRibPlate.bOnRelaPlateTopFace=(hRelaPart1>0);
				m_xRibPlate.plate.bOnDatumPlateTopFace=(hDatumPart>0);
			}
			//��ʼ�����β���
			if(m_pDatumPart.IsHasPtr()&&m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{	//��׼����Ϊ�ֹ�
				long h=0;
				m_pRibPlate->GetDesignItemValue('J',&h);
				m_xRibPlate.tube.iDatumEdge=(int)h;
				m_pRibPlate->GetDesignItemValue('S',&h);
				m_xRibPlate.tube.bVerticalPlate=(h>0);
				m_pRibPlate->GetDesignItemValue('K',&m_xRibPlate.tube.fInitOffsetAngle);
				m_pRibPlate->GetDesignItemValue('R',&m_xRibPlate.tube.fBottomOutterR);
				if(m_pRelaPart2.IsHasPtr()&&m_pRelaPart2->IsPlate())
					m_pRibPlate->GetDesignItemValue('r',&m_xRibPlate.tube.fTopOutterR);
				m_xRibPlate.bAutoExtendOutterEdge=(m_xRibPlate.tube.fBottomOutterR>0&&m_xRibPlate.tube.fTopOutterR>0);
				m_pRibPlate->GetDesignItemValue(KEY2C("DM"),&m_xRibPlate.tube.fOffsetDistM);
				m_pRibPlate->GetDesignItemValue(KEY2C("DX"),&m_xRibPlate.tube.fOffsetDistX);
				m_pRibPlate->GetDesignItemValue(KEY2C("Lt"),(long*)&m_xRibPlate.tube.cLeftTopCornerType);
				m_pRibPlate->GetDesignItemValue(KEY2C("Rt"),(long*)&m_xRibPlate.tube.cRightTopCornerType);
				m_pRibPlate->GetDesignItemValue(KEY2C("Lw"),&m_xRibPlate.tube.fLeftTopCornerWidth);
				m_pRibPlate->GetDesignItemValue(KEY2C("Lh"),&m_xRibPlate.tube.fLeftTopCornerHeight);
				m_pRibPlate->GetDesignItemValue(KEY2C("Rw"),&m_xRibPlate.tube.fRightTopCornerWidth);
				m_pRibPlate->GetDesignItemValue(KEY2C("Rh"),&m_xRibPlate.tube.fRightTopCornerHeight);
				if(m_xRibPlate.tube.iDatumEdge==2)
				{	
					m_pRibPlate->GetDesignItemValue(KEY2C("Nx"),&ucs.axis_z.x);	//����X����
					m_pRibPlate->GetDesignItemValue(KEY2C("Ny"),&ucs.axis_z.y);	//����Y����
					m_pRibPlate->GetDesignItemValue(KEY2C("Nz"),&ucs.axis_z.z);	//����Z����
					m_pRibPlate->GetDesignItemValue(KEY2C("Wx"),&ucs.axis_x.x);	//չ������(X��)X����
					m_pRibPlate->GetDesignItemValue(KEY2C("Wy"),&ucs.axis_x.y);	//չ������(X��)Y����
					m_pRibPlate->GetDesignItemValue(KEY2C("Wz"),&ucs.axis_x.z);	//չ������(X��)Z����
				}
				m_pRibPlate->GetDesignItemValue(KEY2C("Nt"),(long*)&m_xRibPlate.tube.iWorkNormStyle);
				if(m_xRibPlate.tube.iWorkNormStyle==2)
				{
					m_pRibPlate->GetDesignItemValue(KEY2C("Nx"),&ucs.axis_z.x);	//����X����
					m_pRibPlate->GetDesignItemValue(KEY2C("Ny"),&ucs.axis_z.y);	//����Y����
					m_pRibPlate->GetDesignItemValue(KEY2C("Nz"),&ucs.axis_z.z);	//����Z����
				}
				if(m_pRibPlate->GetDesignItemValue(KEY2C("mn"),(long*)&m_xRibPlate.tube.margin))
					m_xRibPlate.tube.bAutoCalB=(m_xRibPlate.tube.margin>=0)?TRUE:FALSE;
			}
			else if(m_pDatumPart.IsHasPtr()&&m_pDatumPart->IsPlate())
			{
				BOOL bSpecOrg=TRUE,bSpecAxisX=TRUE,bSpecAxisY=TRUE;
				bSpecOrg=bSpecOrg&&m_pRibPlate->GetDesignItemValue(KEY2C("Ox"),&m_xRibPlate.plate.fOriginX);
				bSpecOrg=bSpecOrg&&m_pRibPlate->GetDesignItemValue(KEY2C("Oy"),&m_xRibPlate.plate.fOriginY);
				bSpecAxisX=bSpecAxisX&&m_pRibPlate->GetDesignItemValue(KEY2C("Xx"),&m_xRibPlate.plate.fXAxisX);
				bSpecAxisX=bSpecAxisX&&m_pRibPlate->GetDesignItemValue(KEY2C("Xy"),&m_xRibPlate.plate.fXAxisY);
				bSpecAxisY=bSpecAxisY&&m_pRibPlate->GetDesignItemValue(KEY2C("Ga"),&m_xRibPlate.plate.fGradientDegAngle);
				if(m_pVecPart.IsHasPtr()&&(m_pVecPart->IsLinePart()||m_pVecPart->GetClassTypeId()==CLS_BOLT))
					bSpecAxisX=FALSE;
				int iType=CStdVector::GetVectorType(f3dPoint(m_xRibPlate.plate.fXAxisX,m_xRibPlate.plate.fXAxisY));
				if(iType==0&&bSpecAxisX)	//�û��Զ���
					m_xRibPlate.plate.iAxisXType=4;
				else if(iType>0&&iType<5)
					m_xRibPlate.plate.iAxisXType=iType-1;
				else
					m_xRibPlate.plate.iAxisXType=5;
				m_xRibPlate.plate.bSpecOrg=bSpecOrg;
				m_xRibPlate.plate.bSpecAxisX=bSpecAxisX;
				m_xRibPlate.plate.bSpecAxisY=bSpecAxisY;
				m_pRibPlate->GetDesignItemValue('Z',&m_xRibPlate.plate.fOffsetDistZ);
				m_pRibPlate->GetDesignItemValue('X',&m_xRibPlate.plate.fOffsetDistX);
				//----------VVVVVVVVVVVVVVVVVVVVV-----------��չ���ܺ���������� wjh-2016.5.4
				CLDSParamPlate::DESIGN_PARAM Ex,Ey,Bx,By,hTipBolt,Tx,Ty,hBendVecPart,TipOffsetX,TipOffsetZ;
				m_pRibPlate->GetDesignItemValue(KEY2C("Ex"),&Ex);		//ZO->EΪ�߰��׼�������ʼĩ����
				m_pRibPlate->GetDesignItemValue(KEY2C("Ey"),&Ey);		//ZO->EΪ�߰��׼�������ʼĩ����
				m_pRibPlate->GetDesignItemValue(KEY2C("TB"),&hTipBolt);//������ĩ��T��Ļ�׼��˨
				m_pRibPlate->GetDesignItemValue(KEY2C("Tx"),&Tx);		//������ĩ��T����û����붨��ֵ
				m_pRibPlate->GetDesignItemValue(KEY2C("Ty"),&Ty);		//������ĩ��T����û����붨��ֵ
				m_pRibPlate->GetDesignItemValue(KEY2C("BV"),&hBendVecPart);//'E'->TΪ�������߰�����ն˻�����E'->������ĩ��T��
				m_pRibPlate->GetDesignItemValue(KEY2C("Bx"),&Bx);
				m_pRibPlate->GetDesignItemValue(KEY2C("By"),&By);
				m_pRibPlate->GetDesignItemValue(KEY2C("dx"),&TipOffsetX);
				m_pRibPlate->GetDesignItemValue(KEY2C("dz"),&TipOffsetZ);
				memset(m_xRibPlate.plate.anchors,0,sizeof(ANCHOR_PARAM)*4);
				//��λλ��
				if(bSpecOrg)
				{
					m_xRibPlate.plate.anchors[0].ciType=3;
					m_xRibPlate.plate.anchors[0].locate.vertex.x=m_xRibPlate.plate.fOriginX;
					m_xRibPlate.plate.anchors[0].locate.vertex.y=m_xRibPlate.plate.fOriginY;
				}
				else if(hDatumBolt!=0)
				{
					m_xRibPlate.plate.anchors[0].ciType=2;
					m_xRibPlate.plate.anchors[0].locate.hBolt=hDatumBolt;
				}
				//X�᷽��
				if(Ex.bExist&&Ey.bExist)
				{
					m_xRibPlate.plate.anchors[1].ciType=3;
					m_xRibPlate.plate.anchors[1].locate.vertex.x=Ex.value.fValue;
					m_xRibPlate.plate.anchors[1].locate.vertex.y=Ey.value.fValue;
				}
				else if(m_pVecPart.IsHasPtr())
				{
					if(m_pVecPart->GetClassTypeId()==CLS_BOLT)
					{
						m_xRibPlate.plate.anchors[1].ciType=2;
						m_xRibPlate.plate.anchors[1].locate.hBolt=m_pVecPart->handle;
					}
					else if(m_pVecPart->IsLinePart())
					{
						m_xRibPlate.plate.anchors[1].ciType=1;
						m_xRibPlate.plate.anchors[1].locate.hVecRod=m_pVecPart->handle;
					}
				}
				if(Tx.bExist&&Ty.bExist)
				{
					m_xRibPlate.plate.anchors[2].ciType=3;
					m_xRibPlate.plate.anchors[2].locate.vertex.x=Tx.value.fValue;
					m_xRibPlate.plate.anchors[2].locate.vertex.y=Ty.value.fValue;
				}
				else if(hTipBolt.bExist)
				{
					m_xRibPlate.plate.anchors[2].ciType=2;
					m_xRibPlate.plate.anchors[2].locate.hBolt=hTipBolt.hValue;
				}
				if(Bx.bExist&&By.bExist)
				{
					m_xRibPlate.plate.anchors[3].ciType=3;
					m_xRibPlate.plate.anchors[3].locate.vertex.x=Bx.value.fValue;
					m_xRibPlate.plate.anchors[3].locate.vertex.y=By.value.fValue;
				}
				else if(hBendVecPart.bExist)
				{
					CLDSPart *pBendVecPart=console.FromPartHandle(hBendVecPart.hValue);
					if(pBendVecPart&&pBendVecPart->GetClassTypeId()==CLS_BOLT)
					{
						m_xRibPlate.plate.anchors[3].ciType=2;
						m_xRibPlate.plate.anchors[3].locate.hBolt=hBendVecPart.hValue;
					}
					else if(pBendVecPart&&pBendVecPart->IsLinePart())
					{
						m_xRibPlate.plate.anchors[3].ciType=1;
						m_xRibPlate.plate.anchors[3].locate.hVecRod=hBendVecPart.hValue;
					}
				}
				m_xRibPlate.plate.fBendTipOffsetX=m_xRibPlate.plate.fBendTipOffsetZ=0;
				if(TipOffsetX.bExist)
					m_xRibPlate.plate.fBendTipOffsetX=TipOffsetX.value.fValue;
				if(TipOffsetZ.bExist)
					m_xRibPlate.plate.fBendTipOffsetZ=TipOffsetZ.value.fValue;
				//---------^^^^^^^^^^^^^^^^^^^^^^^-----------------
			}
			m_pRibPlate->GetDesignItemValue('E',&m_xRibPlate.fBottomWidth);
			m_pRibPlate->GetDesignItemValue('F',&m_xRibPlate.fTopWidth);
			m_pRibPlate->GetDesignItemValue('G',&m_xRibPlate.fLength);
			m_pRibPlate->GetDesignItemValue('H',&m_xRibPlate.fHeight);
			m_pRibPlate->GetDesignItemValue('I',&m_xRibPlate.fCutAngleWidth);
			m_pRibPlate->GetDesignItemValue(KEY2C("MF"),&m_xRibPlate.MF);
			m_pRibPlate->GetDesignItemValue(KEY2C("MG"),&m_xRibPlate.MG);
			m_pRibPlate->GetDesignItemValue(KEY2C("MH"),&m_xRibPlate.MH);
		}
		if(m_pDatumPart.IsNULL())
			return FALSE;
		if(m_bNewRibPlate&&m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==2)
		{
			m_xRibPlate.bOnRelaPlateTopFace=m_xRibPlate.params.elbow.Z.value.iValue>0;//(hRelaPart1>0);
			//m_xRibPlate.plate.bOnDatumPlateTopFace=(hDatumPart>0);
		}
		else
		{	//�ֹ����������Χ��ʼ���߰�����
			if(m_pDatumPart->IsLinePart()&&m_pRelaPart1.IsNULL()
				&&m_pRelaPart2.IsNULL()&&m_pDatumNode==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("lack of rib plate's positioning datum node");
#else
				AfxMessageBox("ȱ���߰嶨λ��׼�ڵ㣡");
#endif
				return FALSE;
			}
			if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE
				&&(m_pRelaPart1.IsNULL()||m_pRelaPart1->GetClassTypeId()==CLS_LINETUBE)
				&&m_pRelaPart2.IsNULL()&&m_pDatumNode==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("lack of between-plate's positioning datum node!");
#else
				AfxMessageBox("�ְ�а�ȱ�ٶ�λ��׼�ڵ㣡");
#endif
				return FALSE;
			}
			//��ʼ���ְ�����
			if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart2.IsNULL())
			{
				m_xRibPlate.m_cPlateType=0;			//0.���߰�
				if(m_pDatumPart->IsPlate()&&m_pRelaPart1->GetClassTypeId()==CLS_LINETUBE)//������߰�
				{	//ͳһ������Ʋ���Ϊ����׼����Ϊ�ֹܣ��ְ�Ϊm_pRelaPart1
					CLDSPart* pTmpPart=m_pDatumPart;
					m_pDatumPart=m_pRelaPart1;
					m_pRelaPart1=pTmpPart;
				}
				if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE&&m_pRelaPart1->IsPlate())//������߰�
					m_xRibPlate.m_cSubType=CLDSParamPlate::RIBPLATE_ELBOW::SUB_TYPE;
				else	//���ְ�䳣����߰壬��Ǹ�������ѥ����װ����߰�
					m_xRibPlate.m_cSubType=CLDSParamPlate::RIBPLATE_CORNER::SUB_TYPE;
				if(m_bNewRibPlate)
				{
					m_xRibPlate.iCutAngleMode=1;
					m_xRibPlate.fCutAngleWidth=_InitWaterHoleRadiusByThick();
				}
				else if(m_pRibPlate)
					m_pRibPlate->GetDesignItemValue('e',&m_xRibPlate.iCutAngleMode);
			}
			else if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart2.IsHasPtr())
				m_xRibPlate.m_cPlateType=1;			//1.���߰�
			else if(m_pRelaPart1.IsNULL()&&m_pRelaPart2.IsNULL())
			{
				if(m_pDatumPart->IsAngle())
					m_xRibPlate.m_cPlateType=2;		//2.�Ǹ��߰�
				else if(m_pDatumPart->IsPlate())
				{
					m_xRibPlate.m_cPlateType=3;		//3.�ְ��߰�
					if(m_bNewRibPlate)	//�����߰�Ĭ��λ�ڸְ����� wjh-2016.5.06
						m_xRibPlate.plate.bOnDatumPlateTopFace=TRUE;
				}
				else if(m_pDatumPart->IsTube())
					m_xRibPlate.m_cPlateType=4;		//4.�ֹ��߰�
			}
		}
		if(m_bNewRibPlate&&m_pRibPlate==NULL)
			InitRibPlatePara();
		if(m_pRibPlate)		//�����߰�����ϵ
			ReCalRibPlateUCS(FALSE);
		ribPlateSet.Empty();	//����߰��б� wht 11-06-08
		//��ʾ�߰����
		DisplayPropertyList();
		DisplaySubPropList(NULL);
	}
	else if(m_bInernalStart)	//�ڲ�����
	{
		FinishSelectObjOper();	//���ѡ�����ĺ�������
		DisplayPropertyList();	//��ʾ�߰����
		//�ָ�ѡ����
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		m_propList.SetFocus();
		if(pItem)
			m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����	
		DisplaySubPropList(pItem,FALSE);	//��ʾ������
		CPropTreeItem *pSubItem=m_xSubPropList.FindItemByPropId(m_idSubEventProp,NULL);
		m_xSubPropList.SetFocus();
		if(pSubItem)	//ѡ��������
			m_xSubPropList.SetCurSel(pSubItem->m_iIndex);
		//�ڲ�����ʱ���Ѹ�����Ʋ��������߰壬����Ҫ��������
		m_bNewRibPlate=FALSE;
	}
	UpdateSketchMap();
	//�����Ի���λ��
	CRect rect;
	GetWindowRect(rect);
	int width=rect.Width();
	int height=rect.Height();
	rect.left = nScrLocationX;
	rect.top = nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect,TRUE);
	m_bEnableWindowMoveListen=TRUE;
	//�鿴���Բ���ʾ������ȷ�ϰ�ť
	if(!m_bNewRibPlate)
	{
		GetDlgItem(IDC_BTN_PARTS_DB)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_CONFIRM)->ShowWindow(SW_HIDE);
		CWnd *pCancelWnd=GetDlgItem(IDCANCEL);
#ifdef AFX_TARG_ENU_ENGLISH
		pCancelWnd->SetWindowText("Turn off");
#else
		pCancelWnd->SetWindowText("�ر�");
#endif
	}
	return TRUE;
}
bool CDesignRibbedPlateDlg::IsElbowRibPlate()
{
	if(m_xRibPlate.m_cPlateType==0&&m_pDatumPart.IsHasPtr()&&m_pDatumPart->IsTube()&&
		m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->IsPlate())
		return true;
	else
		return false;
}
//����ְ嶥������ʰȡ��(�������ְ����ͨ�ְ�)
static f3dPoint GetPlateDatumFacePickPt(CLDSPart *pDatumPlate,BOOL bTopFace)
{
	f3dPoint face_pick;
	if(!pDatumPlate->IsPlate())
		return face_pick;
	face_pick=pDatumPlate->ucs.origin;
	if(pDatumPlate->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)pDatumPlate;
		face_pick+=(pPlate->ucs.axis_z*pPlate->m_fNormOffset);
		if(bTopFace)
			face_pick+=(pPlate->ucs.axis_z*pPlate->GetThick());
	}
	else if(pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE) 
	{
		double offset=((CLDSParamPlate*)pDatumPlate)->m_fNormOffset;
		if(bTopFace)
			offset+=pDatumPlate->GetThick()*0.5;
		else
			offset-=pDatumPlate->GetThick()*0.5;
		if(((CLDSParamPlate*)pDatumPlate)->m_iParamType==TYPE_FL||
			((CLDSParamPlate*)pDatumPlate)->m_iParamType==TYPE_FLG||
			((CLDSParamPlate*)pDatumPlate)->m_iParamType==TYPE_FLR||
			((CLDSParamPlate*)pDatumPlate)->m_iParamType==TYPE_WATERSHIELDEDPLATE)	//����ʵ��ײ���ԭ��
			offset+=pDatumPlate->GetThick()*0.5;
		face_pick+=pDatumPlate->ucs.axis_z*offset;
	}
	return face_pick;
}
//�Ǹ��߰�: A.�Ǹ�+��׼�ڵ�
//�ֹ��߰�:	B.�ֹ�+��׼�ڵ�
//�ְ��߰�: C.�ְ�+��׼��˨+���򹹼�
//  ���߰�: D.�ְ�+�����ְ�+��˨��λ
//			E.�ֹ�+�����ְ�(��:�ֹ����)
//			F.�ֹ�+�ֹ�(��:�ֹܼа�)	
//  ���߰�: G.�ְ�+2�������ְ�+��˨��λ
//			H.�ֹ�+2�������ְ�
//bUpdateRibLengthParamΪ��ʱ��Ӳ���������Ӧ���ϵ������ط� wjh-2016.5.10
void CDesignRibbedPlateDlg::ReCalRibPlateUCS(BOOL bUpdateRibLengthParam/*=TRUE*/)	//���¼����߰�����ϵ
{
	if(m_pRibPlate)
		ucs=m_pRibPlate->ucs;
	else
	{
		CLDSParamPlate xPlate;
		FillParamToPlate(&xPlate);
		ucs=xPlate.ucs;
	}
}
//�ж������X�᷽���Ƿ����(��ʱֻ������ͨ���߰�)
BOOL CDesignRibbedPlateDlg::IsValidAxisX()
{
	if(!m_pDatumPart->IsPlate() || m_xRibPlate.m_cPlateType!=0)
		return FALSE;
	CLDSGeneralPlate* pDatumPlate=(CLDSGeneralPlate*)m_pDatumPart.GeneralPlatePointer();
	CLDSGeneralPlate *pAssisPlate1=NULL;
	if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->IsPlate())
	{	//Ĭ���Ը����ְ�ķ��߷�����Ϊ�߰��X����
		pAssisPlate1=m_pRelaPart1.GeneralPlatePointer();
		ucs.axis_y=m_pDatumPart->ucs.axis_z;
		if(!m_xRibPlate.plate.bOnDatumPlateTopFace)
			ucs.axis_y*=-1.0;
		if(m_xRibPlate.plate.iAxisXType<5)
		{
			ucs.axis_x.Set(m_xRibPlate.plate.fXAxisX,m_xRibPlate.plate.fXAxisY);
			normalize(ucs.axis_x);
			vector_trans(ucs.axis_x,m_pDatumPart->ucs,TRUE);
		}
		else
			ucs.axis_x=m_pRelaPart1->ucs.axis_z;
		ucs.axis_z=ucs.axis_x^ucs.axis_y;
		normalize(ucs.axis_z);
		double cons45=0.70710;
		if(fabs(ucs.axis_z*m_pRelaPart1->ucs.axis_z)>cons45)
			return FALSE;
	}
	return TRUE;
}
BOOL CDesignRibbedPlateDlg::AutoCalRibPlateWidth()
{
	//����ֹ��߰�����ϵ
	ReCalRibPlateUCS();
	if(!m_bNewRibPlate&&!(m_pDatumPart->IsPlate()&&m_pRelaPart1.IsHasPtr()))
		return FALSE;	//�鿴�Ѵ����߰�����,�ҷǸְ���߰����߰�ʱ������ְ��� wht 11-05-25
	if(m_pRelaPart1.IsHasPtr()&&!m_pRelaPart1->IsPlate())
		return FALSE;	//���ڸ�������1�Ҳ�Ϊ�ְ�
	if(m_pRelaPart2.IsHasPtr()&&!m_pRelaPart2->IsPlate())
		return FALSE;	//���ڸ�������2�Ҳ�Ϊ�ְ�
	if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{
		double tube_r=m_pDatumPart.LineTubePointer()->GetDiameter()*0.5;
		for(int i=0;i<2;i++)
		{
			CSuperSmartPtr<CLDSPart> pPart;
			if(i==0&&m_pRelaPart1.IsHasPtr())
				pPart=m_pRelaPart1;
			else if(i==1&&m_pRelaPart2.IsHasPtr())
				pPart=m_pRelaPart2;
			if(pPart.IsNULL())
				continue;
			double r=0,width=0;
			if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//
				CLDSPlate *pPlate=pPart.PlatePointer();
				double inner_r=0,outer_r=0;
				int nVertex=pPlate->vertex_list.GetNodeNum();
				for(int j=0;j<nVertex;j++)
				{
					PROFILE_VERTEX *pStartVertex=&pPlate->vertex_list[j];
					PROFILE_VERTEX *pEndVertex=&pPlate->vertex_list[(j+1)/nVertex];
					if(pStartVertex->type==0)
						continue;
					double cur_r=0;
					if(pStartVertex->type==1)	//ָ��Բ���н�
					{
						int sign=1;
						f3dArcLine arcline;
						if(pStartVertex->sector_angle<0)
							sign=-1;
						arcline.CreateMethod2(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,sign),sign*pStartVertex->sector_angle);
						cur_r=arcline.Radius();
					}
					else 
						cur_r=fabs(pStartVertex->radius);
					if(fabs(cur_r-tube_r)<10)	//10Ϊ���ļӹ���϶ֵ,�жϵ�ǰԲ���Ƿ���ֹ����� wht 11-08-29
						inner_r=cur_r;
					else 
						outer_r=max(outer_r,cur_r);
				}
				
				r=(int)(pPart.PlatePointer()->GetWidth()*0.5);
				if(fabs(inner_r)>0)
				{	//�ְ������ֹ���������Բ��
					if(outer_r>inner_r)
					{	//������Բ��
						r=(int)outer_r;
						width=r-inner_r;
					}
					else 
					{	//��������Բ��ʱ��Բ���뾶=�ְ���+��Բ�뾶
						width=r;
						r+=(int)inner_r;
					}
				}
				else
					width=r-tube_r;
			}
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE
				&&(pPart.ParamPlatePointer()->m_iParamType==TYPE_CIRCOVERPLATE
				||pPart.ParamPlatePointer()->m_iParamType==TYPE_CIRRIBPLATE))
				//||pPart.ParamPlatePointer()->m_iParamType==TYPE_CIRPLATE)
			{	//������ѡ������������߰���Ĭ��ֵ wht 11-05-12
				if(pPart.ParamPlatePointer()->m_iParamType==TYPE_CIRCOVERPLATE)
				{
					double W=0,R=0;
					pPart.ParamPlatePointer()->GetDesignItemValue('W',&W);
					pPart.ParamPlatePointer()->GetDesignItemValue('R',&R);
					r=R+W;
					width=W;
				}
				else if(pPart.ParamPlatePointer()->m_iParamType==TYPE_CIRRIBPLATE)
				{
					double W=0,Ro=0,V=0,D=0;
					pPart.ParamPlatePointer()->GetDesignItemValue('V',&V);
					pPart.ParamPlatePointer()->GetDesignItemValue('D',&D);
					pPart.ParamPlatePointer()->GetDesignItemValue('R',&Ro);
					double Ri=m_pDatumPart.LineTubePointer()->GetDiameter()*0.5+V;	//��Բ���뾶
					if(Ro>0)	//���ͷ����������Խ�������ָ������Բ���뾶����ֵ
						D=Ro-Ri;
					else if(D>0)
						Ro=Ri+D;
					r=Ro;
					width=D;
				}
			}
			else
			{
				r=(int)(m_pDatumPart.LineTubePointer()->GetDiameter()*0.5)+100;
				width=100;
			}
			if(i==0)
			{
				m_xRibPlate.tube.fBottomOutterR=r;
				m_xRibPlate.fBottomWidth=width;
			}
			else 
			{
				m_xRibPlate.tube.fTopOutterR=r;
				m_xRibPlate.fTopWidth=width;
			}
		}
	}
	else if(m_pDatumPart->IsPlate())
	{
		for(int i=0;i<2;i++)
		{
			f3dPoint plate_face_pick;
			CLDSGeneralPlate *pPlate=NULL;
			if(i==0&&m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->IsPlate())
			{
				pPlate=m_pRelaPart1.GeneralPlatePointer();
				plate_face_pick=GetPlateDatumFacePickPt(m_pRelaPart1,m_xRibPlate.bOnRelaPlateTopFace);
			}
			else if(i==1&&m_pRelaPart2.IsHasPtr()&&m_pRelaPart2->IsPlate())
			{
				pPlate=m_pRelaPart2.GeneralPlatePointer();
				plate_face_pick=GetPlateDatumFacePickPt(m_pRelaPart2,(ucs.axis_x*pPlate->ucs.axis_z)<0);
			}
			if(pPlate==NULL)
				continue;
			f3dPoint face_pick=ucs.origin;
			Int3dlf(face_pick,ucs.origin,ucs.axis_x,plate_face_pick,pPlate->ucs.axis_z);
			PROFILE_VERTEX *pPreVertex=NULL,*pVertex=NULL;
			f3dPoint inters_line_vec=ucs.axis_z^pPlate->ucs.axis_z;
			normalize(inters_line_vec);
			if(inters_line_vec*ucs.axis_y<0)
				inters_line_vec*=-1;
			pPreVertex=pPlate->vertex_list.GetTail(); //�ְ�ĵ�һ�������ǰһ������Ϊ���һ������
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				if(pPreVertex&&pPreVertex!=pVertex)
				{
					f3dLine line(pPreVertex->vertex,pVertex->vertex);
					//����������ʼ�ն˷ֱ���������������񣬱���Zƫ����δ����ǰ�޷���������
					f3dPoint line_vec=line.endPt-line.startPt;
					normalize(line_vec);
					line.startPt-=line_vec*m_xRibPlate.nThick*2;
					line.endPt+=line_vec*m_xRibPlate.nThick*2;
					coord_trans(line.startPt,pPlate->ucs,TRUE);
					coord_trans(line.endPt,pPlate->ucs,TRUE);
					f3dPoint inter_pos;
					int ret=Int3dDifferPlaneLL(line.startPt,line.endPt,face_pick,face_pick+inters_line_vec*10000,inter_pos);
					if(ret>0)
					{
						double dist=DISTANCE(inter_pos,face_pick);
						if(dist>m_pDatumPart->GetThick())
						{
							if(i==0)
								m_xRibPlate.fBottomWidth=(int)dist;
							else if(i==1)
								m_xRibPlate.fTopWidth=(int)dist;
							break;
						}
					}
					pPreVertex=pVertex;
				}
			}
			if(m_pRelaPart2.IsNULL()&&m_pDatumPart->IsPlate())
			{	//������߰����׼�������ߵĳ���
				CLDSGeneralPlate *pDatumPlate=m_pDatumPart.GeneralPlatePointer();
				face_pick=ucs.origin;
				f3dPoint axis_x=pDatumPlate->ucs.axis_z^ucs.axis_z;
				if(axis_x*ucs.axis_x<0)
					axis_x*=-1;
				pPreVertex=pDatumPlate->vertex_list.GetTail(); //�ְ�ĵ�һ�������ǰһ������Ϊ���һ������
				for(pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate->vertex_list.GetNext())
				{
					if(pPreVertex&&pPreVertex!=pVertex)
					{
						f3dLine line(pPreVertex->vertex,pVertex->vertex);
						//����������ʼ�ն˷ֱ���������������񣬱���Zƫ����δ����ǰ�޷���������
						f3dPoint line_vec=line.endPt-line.startPt;
						normalize(line_vec);
						line.startPt-=line_vec*m_xRibPlate.nThick*2;
						line.endPt+=line_vec*m_xRibPlate.nThick*2;
						coord_trans(line.startPt,m_pDatumPart->ucs,TRUE);
						coord_trans(line.endPt,m_pDatumPart->ucs,TRUE);
						f3dPoint inter_pos;
						int ret=Int3dDifferPlaneLL(face_pick,face_pick+axis_x*10000,line.startPt,line.endPt,inter_pos);
						if(ret>0)
						{
							double dist=DISTANCE(inter_pos,face_pick);
							if(dist>m_pDatumPart->GetThick())
							{
								m_xRibPlate.fLength=(int)(dist);
								m_xRibPlate.fHeight=30;	//(int)(0.4*dist);
								break;
							}
						}
						if(m_xRibPlate.bCreateOuterPlate)
						{	//��������߰峤��
							ret=Int3dll(line.startPt,line.endPt,face_pick,face_pick-axis_x*10000,inter_pos);
							if(ret>0)
							{
								double dist=DISTANCE(inter_pos,face_pick);
								if(dist>m_pDatumPart->GetThick())
								{
									m_xRibPlate.fOuterPlateLen=(int)(dist-m_pRelaPart1->GetThick());
									m_xRibPlate.fOuterPlateHeight=30;//(int)(0.4*dist);
									break;
								}
							}
						}
						pPreVertex=pVertex;
					}
				}
			}
		}
		m_xRibPlate.fBottomWidth=max(m_xRibPlate.fBottomWidth,50);
		m_xRibPlate.fTopWidth=max(m_xRibPlate.fTopWidth,30);
	}
	return TRUE;
}
long CDesignRibbedPlateDlg::GetPropID(char* propStr)
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(propStr);
	if(pItem)
		return pItem->id;
	else
	{
		/*char prop_str[500];
		strcpy(prop_str,"û�ҵ���Ϊ\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"������ID��";
		AfxMessageBox(prop_str);*/
		return 0;
	}
}

int CDesignRibbedPlateDlg::GetPropValueStr(long id, char *valueStr)
{
	CXhChar100 sText;
	if(GetPropID("RibPlate.sPartNo")==id)		//���
		sText.Copy(m_xRibPlate.sPartNo);
	else if(GetPropID("RibPlate.nSegI")==id)	//�κ�
		sText.Printf("%d",m_xRibPlate.nSegI);
	else if(GetPropID("RibPlate.nThick")==id)	//���
		sText.Printf("%d",m_xRibPlate.nThick);
	else if(GetPropID("RibPlate.cMaterial")==id)//����
	{
		char matStr[20]="";
		QuerySteelMatMark(m_xRibPlate.cMaterial,matStr);
		sText.Printf("%s",matStr);
	}
	else if(GetPropID("RibPlate.m_cPlateType")==id)
	{
		if(0==m_xRibPlate.m_cPlateType)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Angle Rib Plate"); 
		else if(1==m_xRibPlate.m_cPlateType)
			sText.Copy("1.Slot Rib Plate");
		else if(2==m_xRibPlate.m_cPlateType)
			sText.Copy("2.Angle Steel Rib Plate");
		else if(3==m_xRibPlate.m_cPlateType)
			sText.Copy("3.Plate Rib Plate");
		else if(4==m_xRibPlate.m_cPlateType)
			sText.Copy("4.Tube Rib Plate");
#else
			sText.Copy("0.���߰�"); 
		else if(1==m_xRibPlate.m_cPlateType)
			sText.Copy("1.���߰�");
		else if(2==m_xRibPlate.m_cPlateType)
			sText.Copy("2.�Ǹ��߰�");
		else if(3==m_xRibPlate.m_cPlateType)
			sText.Copy("3.�ְ��߰�");
		else if(4==m_xRibPlate.m_cPlateType)
			sText.Copy("4.�ֹ��߰�");
#endif
	}
	//��׼����Ϊ�ֹ�ʱʹ��
	else if(GetPropID("RibPlate.tube.iDatumEdge")==id)
	{
		if(m_xRibPlate.tube.iDatumEdge==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Datum Edge");
		else if(m_xRibPlate.tube.iDatumEdge==1)
			sText.Copy("1.Welding Line");
		else //if(m_xRibPlate.tube.iDatumEdge==2)
			sText.Copy("2.Specify");
#else
			sText.Copy("0.��׼��");
		else if(m_xRibPlate.tube.iDatumEdge==1)
			sText.Copy("1.������");
		else //if(m_xRibPlate.tube.iDatumEdge==2)
			sText.Copy("2.ָ������");
#endif
	}
	else if(GetPropID("RibPlate.tube.iWorkNormStyle")==id)
	{
		if(m_xRibPlate.tube.iWorkNormStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Perpendicular Tube Along Vector");
		else if(m_xRibPlate.tube.iWorkNormStyle==1)
			sText.Copy("1.Tube Axis Vector");
		else //if(m_xRibPlate.tube.iWorkNormStyle==2)
			sText.Copy("2.Specify");
#else
			sText.Copy("0.�ֹ�����");
		else if(m_xRibPlate.tube.iWorkNormStyle==1)
			sText.Copy("1.�ֹܽ���");
		else //if(m_xRibPlate.tube.iWorkNormStyle==2)
			sText.Copy("2.ָ������");
#endif
	}
	else if(GetPropID("RibPlate.tube.nPlateNum")==id)	//�ֹ��߰����
		sText.Printf("%d",m_xRibPlate.tube.nPlateNum);
	else if(GetPropID("RibPlate.tube.extend_vec")==id)	//�����췽��
		sText.Printf("%s",CStdVector::GetVecTypeString(m_xRibPlate.tube.extend_vec));
	else if(GetPropID("RibPlate.tube.fInitOffsetAngle")==id)//��ʼƫת�Ƕ�
	{
		sText.Printf("%f",m_xRibPlate.tube.fInitOffsetAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.tube.fOffsetAngle")==id)	//ƫ�ƽǶ�
	{
		sText.Printf("%f",m_xRibPlate.tube.fOffsetAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.tube.fOffsetDistM")==id)	//����ƫ�ƾ���M
	{
		sText.Printf("%f",m_xRibPlate.tube.fOffsetDistM);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.tube.fOffsetDistX")==id)	//Xƫ�ƾ���
	{
		sText.Printf("%f",m_xRibPlate.tube.fOffsetDistX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.plate.fOffsetDistX")==id)	//Xƫ�ƾ���
	{
		sText.Printf("%f",m_xRibPlate.plate.fOffsetDistX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.plate.fOffsetDistZ")==id)	//Z����ƫ�ƾ���
	{
		sText.Printf("%f",m_xRibPlate.plate.fOffsetDistZ);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.plate.bOnDatumPlateTopFace")==id)//�Զ�������������Ե
	{
		if(m_xRibPlate.plate.bOnDatumPlateTopFace)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("Yes");
		else 
			sText.Printf("No");
#else
			sText.Printf("��");
		else 
			sText.Printf("��");
#endif
	}
	else if(GetPropID("RibPlate.plate.bAlongVecPart")==id)//�ְ����췽���뷽��˼�ͬ��
	{
		if(m_xRibPlate.plate.bAlongVecPart)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("Yes");
		else 
			sText.Printf("No");
#else
			sText.Printf("��");
		else 
			sText.Printf("��");
#endif
	}
	else if(GetPropID("RibPlate.plate.bSpecOrg")==id)
	{
		if(m_xRibPlate.plate.bSpecOrg)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Input Directly");
		else
			sText.Copy("0.Specify Datum Part");
#else
			sText.Copy("1.ֱ������");
		else
			sText.Copy("0.��˨��λ");
#endif
	}
	else if(GetPropID("RibPlate.plate.bSpecAxisX")==id)
	{
		if(m_xRibPlate.m_cPlateType==3)
		{
			if(m_xRibPlate.plate.bSpecAxisX)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("3.Input Directly");
			else if(m_xRibPlate.plate.anchors[1].ciType==1)
				sText.Copy("0.Along Rod");
			else if(m_xRibPlate.plate.anchors[1].ciType==2)
				sText.Copy("1.Direct to Bolt");
			else if(m_xRibPlate.plate.anchors[1].ciType==3)
				sText.Copy("2.Direct to Point");
			else
				sText.Copy("3.Input Directly");
#else
				sText.Copy("3.ֱ������");
			else if(m_xRibPlate.plate.anchors[1].ciType==1)
				sText.Copy("0.�ظ˼�����");
			else if(m_xRibPlate.plate.anchors[1].ciType==2)
				sText.Copy("1.ָ����˨");
			else if(m_xRibPlate.plate.anchors[1].ciType==3)
				sText.Copy("2.�����");
			else
				sText.Copy("3.ֱ������");
#endif
		}
		else
		{
			if(m_xRibPlate.plate.bSpecAxisX)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("1.Input Directly");
			else
				sText.Copy("0.Calculate Automatically");
#else
				sText.Copy("1.ֱ������");
			else
				sText.Copy("0.���򹹼�");
#endif
		}
	}
	else if(GetPropID("RibPlate.plate.iAxisXType")==id)
	{
		if(m_xRibPlate.plate.iAxisXType==0)
			sText.Copy("0.X+");
		else if(m_xRibPlate.plate.iAxisXType==1)
			sText.Copy("1.X-");
		else if(m_xRibPlate.plate.iAxisXType==2)
			sText.Copy("2.Y+");
		else if(m_xRibPlate.plate.iAxisXType==3)
			sText.Copy("3.Y-");
		else if(m_xRibPlate.plate.iAxisXType==4)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("4.Input...");
		else if(m_xRibPlate.plate.iAxisXType==5)
			sText.Copy("5.Calculate Automatically");
#else
			sText.Copy("4.����...");
		else if(m_xRibPlate.plate.iAxisXType==5)
			sText.Copy("5.�Զ�����");
#endif
	}
	else if(GetPropID("RibPlate.plate.IsHasBendPlane")==id)
	{	//������","","0.��������|1.��������
		if(m_xRibPlate.m_cPlateType==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_xRibPlate.plate.anchors[2].ciType==0)
				sText.Copy("0.Not Have Bended Plane");
			else//if(m_xRibPlate.plate.anchors[2].ciType==0)	//��������
				sText.Copy("1.Have Bended Plane");
#else
			if(m_xRibPlate.plate.anchors[2].ciType==0)
				sText.Copy("0.��������");
			else//if(m_xRibPlate.plate.anchors[2].ciType==0)	//��������
				sText.Copy("1.��������");
#endif
		}
	}
	else if(GetPropID("RibPlate.plate.BendPlaneTip")==id)
	{	//ĩ�˶�λ��","","0.��˨��λ|1.ֱ������
		if(m_xRibPlate.m_cPlateType!=3)
			sText.Copy("");
#ifdef AFX_TARG_ENU_ENGLISH
		else if(m_xRibPlate.plate.anchors[2].ciType==2)
			sText.Copy("0.Specify Datum Bolt");
		else //if(m_xRibPlate.plate.anchors[2].ciType==3)
			sText.Copy("1.Input Directly");
#else
		else if(m_xRibPlate.plate.anchors[2].ciType==2)
			sText.Copy("0.��˨��λ");
		else //if(m_xRibPlate.plate.anchors[2].ciType==3)
			sText.Copy("1.ֱ������");
#endif
	}
	else if(GetPropID("RibPlate.plate.BendTipPointX")==id)
	{	//������ĩ��X
		if(m_xRibPlate.m_cPlateType!=3)
			sText.Copy("");
		else if(m_xRibPlate.plate.anchors[2].ciType==3)
			sText.Printf("%g",m_xRibPlate.plate.anchors[2].locate.vertex.x);
		else //
			sText.Copy("");//.Printf("%g",m_xRibPlate.plate.anchors[2].locate.vertex.x);
	}
	else if(GetPropID("RibPlate.plate.BendTipPointY")==id)
	{	//������ĩ��Y
		if(m_xRibPlate.m_cPlateType!=3)
			sText.Copy("");
		else if(m_xRibPlate.plate.anchors[2].ciType==3)
			sText.Printf("%g",m_xRibPlate.plate.anchors[2].locate.vertex.y);
		else //
			sText.Copy("");//.Printf("%g",m_xRibPlate.plate.anchors[2].locate.vertex.y);
	}
	else if(GetPropID("RibPlate.plate.BendPlaneTipBolt")==id)
	{	//������ĩ����˨��λ
		if(m_xRibPlate.m_cPlateType==3&&m_xRibPlate.plate.anchors[2].ciType==2)
			sText.Printf("0X%X",m_xRibPlate.plate.anchors[2].locate.hBolt);
	}
	else if(GetPropID("RibPlate.plate.BendDirect")==id)
	{	//����������","","0.���ܶ���|1.�ظ˼�����|2.��˨��������|3.���������"
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_xRibPlate.plate.anchors[3].ciType==1)
			sText.Copy("1.Along Rod");
		else if(m_xRibPlate.plate.anchors[3].ciType==2)
			sText.Copy("2.Direct to Bolt");
		else if(m_xRibPlate.plate.anchors[3].ciType==3)
			sText.Copy("3.Direct to Point");
		else
			sText.Copy("0.Calculate Automatically");
#else
		if(m_xRibPlate.plate.anchors[3].ciType==1)
			sText.Copy("1.�ظ˼�����");
		else if(m_xRibPlate.plate.anchors[3].ciType==2)
			sText.Copy("2.��˨��������");
		else if(m_xRibPlate.plate.anchors[3].ciType==3)
			sText.Copy("3.���������");
		else
			sText.Copy("0.���ܶ���");
#endif
	}
	else if(GetPropID("RibPlate.plate.BendDirectVecPart")==id)
	{	//����������˨"
		if(m_xRibPlate.m_cPlateType==3&&(m_xRibPlate.plate.anchors[3].ciType==1||m_xRibPlate.plate.anchors[3].ciType==2))
		{
			if(m_xRibPlate.plate.anchors[3].ciType==1)
				sText.Printf("0X%X",m_xRibPlate.plate.anchors[3].locate.hVecRod);
			else
				sText.Printf("0X%X",m_xRibPlate.plate.anchors[3].locate.hBolt);
		}
	}
	else if(GetPropID("RibPlate.plate.BendDirectPoint")==id)
	{	//BendDirectPoint"
	}
	else if(GetPropID("RibPlate.plate.BendDirectPointX")==id)
	{	//���������"
		if(m_xRibPlate.m_cPlateType==3&&m_xRibPlate.plate.anchors[3].ciType==3)
			sText.Printf("%g",m_xRibPlate.plate.anchors[3].locate.vertex.x);
	}
	else if(GetPropID("RibPlate.plate.BendDirectPointY")==id)
	{	//BendDirectPointY"
		if(m_xRibPlate.m_cPlateType==3&&m_xRibPlate.plate.anchors[3].ciType==3)
			sText.Printf("%g",m_xRibPlate.plate.anchors[3].locate.vertex.y);
	}
	else if(GetPropID("RibPlate.plate.fBendTipOffsetX")==id)
	{	//X��ƫ�Ʀ�x"
		if(m_xRibPlate.m_cPlateType==3&&m_xRibPlate.plate.anchors[2].ciType!=0)
			sText.Printf("%g",m_xRibPlate.plate.fBendTipOffsetX);
	}
	else if(GetPropID("RibPlate.plate.fBendTipOffsetZ")==id)
	{	//X��ƫ�Ʀ�z"
		if(m_xRibPlate.m_cPlateType==3&&m_xRibPlate.plate.anchors[2].ciType!=0)
			sText.Printf("%g",m_xRibPlate.plate.fBendTipOffsetZ);
	}
	else if(GetPropID("RibPlate.plate.bSpecAxisY")==id)
	{
		if(m_xRibPlate.plate.bSpecAxisY)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Inclination Angle");
#else
			sText.Copy("1.��б�Ƕ�");
#endif
		else 
		{
			if(m_xRibPlate.m_cPlateType==3)
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("0.Vertical Basic Plane");
			else 
				sText.Copy("0.Direction Rod");
#else
				sText.Copy("0.��ֱ����");
			else 
				sText.Copy("0.����˼�");
#endif
		}
	}
	else if(GetPropID("RibPlate.plate.fGradientDegAngle")==id)
	{
		sText.Printf("%f",m_xRibPlate.plate.fGradientDegAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.plate.fOriginX")==id)
	{
		sText.Printf("%f",m_xRibPlate.plate.fOriginX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.plate.fOriginY")==id)
	{
		sText.Printf("%f",m_xRibPlate.plate.fOriginY);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.plate.fXAxisX")==id)
	{
		if(m_xRibPlate.m_cPlateType==3)
			sText.Printf("%f",m_xRibPlate.plate.anchors[1].locate.vertex.x);
		else
			sText.Printf("%f",m_xRibPlate.plate.fXAxisX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.plate.fXAxisY")==id)
	{
		if(m_xRibPlate.m_cPlateType==3)
			sText.Printf("%f",m_xRibPlate.plate.anchors[1].locate.vertex.y);
		else
			sText.Printf("%f",m_xRibPlate.plate.fXAxisY);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.plate.BendPlaneTip")==id)
	{	//������ĩ��
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_xRibPlate.plate.anchors[2].ciType==0)
			sText.Copy("0.Not Have Bended Plane");
		else if(m_xRibPlate.plate.anchors[2].ciType==2)
			sText.Copy("1.Specify Datum Bolt");
		else if(m_xRibPlate.plate.anchors[2].ciType==3)
			sText.Copy("2.Input Directly");
#else
		if(m_xRibPlate.plate.anchors[2].ciType==0)
			sText.Copy("0.��������");
		else if(m_xRibPlate.plate.anchors[2].ciType==2)
			sText.Copy("1.��˨��λ");
		else if(m_xRibPlate.plate.anchors[2].ciType==3)
			sText.Copy("2.ֱ������");
#endif
		else
			sText.Copy("");
	}
	//
	else if(GetPropID("RibPlate.bAutoExtendOutterEdge")==id)//�Զ�������������Ե
	{
		if(m_xRibPlate.bAutoExtendOutterEdge)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("Yes");
		else 
			sText.Printf("No");
#else
			sText.Printf("��");
		else 
			sText.Printf("��");
#endif
	}
	else if(GetPropID("RibPlate.bOnRelaPlateTopFace")==id)//�Զ�������������Ե
	{
		if(m_xRibPlate.bOnRelaPlateTopFace)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("Yes");
		else 
			sText.Printf("No");
#else
			sText.Printf("��");
		else 
			sText.Printf("��");
#endif
	}
	else if(GetPropID("RibPlate.tube.bVerticalPlate")==id)//��ֱ�Ҵ��Զ���λ���߰�
	{
		if(m_xRibPlate.tube.bVerticalPlate)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("Yes");
		else 
			sText.Printf("No");
#else
			sText.Printf("��");
		else 
			sText.Printf("��");
#endif
	}
	else if(GetPropID("RibPlate.tube.bAutoCalB")==id)
	{
		if(m_xRibPlate.tube.bAutoCalB)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("Yes");
		else 
			sText.Printf("No");
#else
			sText.Printf("��");
		else 
			sText.Printf("��");
#endif
	}
	else if(GetPropID("RibPlate.tube.margin")==id)
		sText.Printf("%d",m_xRibPlate.tube.margin);
	else if(GetPropID("RibPlate.tube.fBottomOutterR")==id)	//�ײ��⻷�뾶
	{
		sText.Printf("%f",m_xRibPlate.tube.fBottomOutterR);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.tube.fTopOutterR")==id)	//�����⻷�뾶
	{
		sText.Printf("%f",m_xRibPlate.tube.fTopOutterR);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.fTopWidth")==id)		//�������
	{
		sText.Printf("%f",m_xRibPlate.fTopWidth);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.fBottomWidth")==id)		//�ײ����
	{
		sText.Printf("%f",m_xRibPlate.fBottomWidth);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.fLength")==id)			//����(Ϊ���ְ�а�ʱ�ó����Զ�����)
	{
		sText.Printf("%f",m_xRibPlate.fLength);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.fHeight")==id)			//�߶�
	{
		sText.Printf("%f",m_xRibPlate.fHeight);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("RibPlate.fCirR")==id)
	{
		sText.Printf("%f",m_xRibPlate.fCirR);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.fCutAngleWidth")==id)	//�нǿ��
	{
		sText.Printf("%f",m_xRibPlate.fCutAngleWidth);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.fCutAngleWidth2")==id)
	{
		sText.Printf("%f",m_xRibPlate.fCutAngleWidth2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.iCutAngleMode")==id)
	{
		if(m_xRibPlate.iCutAngleMode==0)
			sText.Copy("0.�н�");
		else if(m_xRibPlate.iCutAngleMode==1)
			sText.Copy("1.�л�");
	}
	else if(GetPropID("RibPlate.MF")==id)	//�׾�F
		sText.Printf("%g",m_xRibPlate.MF);
	else if(GetPropID("RibPlate.MG")==id)	//�׾�G
		sText.Printf("%g",m_xRibPlate.MG);
	else if(GetPropID("RibPlate.MH")==id)	//�׾�H
		sText.Printf("%g",m_xRibPlate.MH);
	else if(GetPropID("RibPlate.bCreateOuterPlate")==id)//ͬʱ��������߰�
	{
		if(m_xRibPlate.bCreateOuterPlate)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("Yes");
		else 
			sText.Printf("No");
#else
			sText.Printf("��");
		else 
			sText.Printf("��");
#endif
	}
	else if(GetPropID("RibPlate.fOuterPlateLen")==id)	//����߰峤��
	{
		sText.Printf("%f",m_xRibPlate.fOuterPlateLen);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.fOuterPlateHeight")==id)//����߰�߶�
	{
		sText.Printf("%f",m_xRibPlate.fOuterPlateHeight);
		SimplifiedNumString(sText);
	}
	//��׼��������������
	else if(GetPropID("m_pDatumPart")==id)	//��׼����
	{
		if(m_pDatumPart.IsHasPtr())
			sText.Printf("0X%X",m_pDatumPart->handle);
	}
	else if(GetPropID("m_pRelaPart1")==id)	//��������1
	{
		if(m_pRelaPart1.IsHasPtr())
			sText.Printf("0X%X",m_pRelaPart1->handle);
	}
	else if(GetPropID("m_pRelaPart2")==id)	//��������2
	{
		if(m_pRelaPart2.IsHasPtr())
			sText.Printf("0X%X",m_pRelaPart2->handle);
	}
	else if(GetPropID("m_pDatumBolt")==id)	//��˨��λ
	{
		if(m_pDatumBolt)
			sText.Printf("0X%X",m_pDatumBolt->handle);
		else
			sText.Copy("0X0");
	}
	else if(GetPropID("m_pDatumNode")==id)	//��׼�ڵ�
	{
		if(m_pDatumNode)
			sText.Printf("0X%X",m_pDatumNode->handle);
	}
	else if(GetPropID("m_pVecPart")==id)	//����˼�
	{
		if(m_pVecPart.IsHasPtr())
			sText.Printf("0X%X",m_pVecPart->handle);
		else
			sText.Copy("0X0");
	}
	else if(GetPropID("ucs.axis_z")==id)
	{
		int nVecType=CStdVector::GetVectorType(ucs.axis_z);
		sText.Printf("%s",CStdVector::GetVecTypeString(nVecType));
	}
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sText.Printf("%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sText.Printf("%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sText.Printf("%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x")==id)
	{
		int nVecType=CStdVector::GetVectorType(ucs.axis_x);
		sText.Printf("%s",CStdVector::GetVecTypeString(nVecType));
	}
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sText.Printf("%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sText.Printf("%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sText.Printf("%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.tube.cLeftTopCornerType")==id)
	{
		if(m_xRibPlate.tube.cLeftTopCornerType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Chamfer Angle");
		else if(m_xRibPlate.tube.cLeftTopCornerType==2)
			sText.Copy("2.Arc");
		else
			sText.Copy("0.Right-Angle");
#else
			sText.Copy("1.����");
		else if(m_xRibPlate.tube.cLeftTopCornerType==2)
			sText.Copy("2.Բ��");
		else
			sText.Copy("0.ֱ��");
#endif
	}
	else if(GetPropID("RibPlate.tube.fLeftTopCornerWidth")==id)
	{
		sText.Printf("%f",m_xRibPlate.tube.fLeftTopCornerWidth);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.tube.fLeftTopCornerHeight")==id)
	{
		sText.Printf("%f",m_xRibPlate.tube.fLeftTopCornerHeight);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.tube.cRightTopCornerType")==id)
	{
		if(m_xRibPlate.tube.cRightTopCornerType==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Chamfer Angle");
		else if(m_xRibPlate.tube.cRightTopCornerType==2)
			sText.Copy("2.Arc");
		else
			sText.Copy("0.Right-Angle");
#else
			sText.Copy("1.����");
		else if(m_xRibPlate.tube.cRightTopCornerType==2)
			sText.Copy("2.Բ��");
		else
			sText.Copy("0.ֱ��");
#endif
	}
	else if(GetPropID("RibPlate.tube.fRightTopCornerWidth")==id)
	{
		sText.Printf("%f",m_xRibPlate.tube.fRightTopCornerWidth);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("RibPlate.tube.fRightTopCornerHeight")==id)
	{
		sText.Printf("%f",m_xRibPlate.tube.fRightTopCornerHeight);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
bool CDesignRibbedPlateDlg::CorrectPropItemName(const char* propKey,CItemInfo* lpInfo)
{
	if(stricmp(propKey,"m_pDatumPart")==0)
	{
		if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_ELBOW::SUB_TYPE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="datum LineTube";
			lpInfo->m_strPropHelp="plate's positioning datum part";
#else
			lpInfo->m_strPropName="���Ӹֹ�";
			lpInfo->m_strPropHelp="�߰嶨λ��׼����";
#endif
		}
		else if(m_xRibPlate.m_cPlateType==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="datum plate";
			lpInfo->m_strPropHelp="plate's positioning datum part";
#else
			lpInfo->m_strPropName="��׼�ְ�";
			lpInfo->m_strPropHelp="�߰嶨λ��׼����";
#endif
		}
		else
			return false;
	}
	else if(stricmp(propKey,"m_pRelaPart1")==0)
	{
		if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_ELBOW::SUB_TYPE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Weld BasePlate";
			lpInfo->m_strPropHelp="plate positioning Supplementary part";
#else
			lpInfo->m_strPropName="���ӵװ�";
			lpInfo->m_strPropHelp="���ӵװ�";
#endif
		}
		else
		{	
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Supplementary part";
			lpInfo->m_strPropHelp = "plate positioning Supplementary part";
#else
			lpInfo->m_strPropName="��������";
			lpInfo->m_strPropHelp="��������";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.plate.fXAxisX")==0)
	{
		if(m_xRibPlate.m_cPlateType==3&&m_xRibPlate.plate.anchors[1].ciType==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Axis X's direction point X Coord ,by the datum plate local CS";
#else
			lpInfo->m_strPropHelp="�߰�X����ԭ����ʼ���ն�ָ���X�����������׼�ְ��µľֲ�����";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Axis X's X Coord ,by the datum plate local CS";
#else
			lpInfo->m_strPropHelp="�ְ�X�᷽��X�����������׼�ְ�����ϵ�µ���Է���";
#endif

		}
	}
	else if(stricmp(propKey,"RibPlate.plate.fXAxisY")==0)
	{
		if(m_xRibPlate.m_cPlateType==3&&m_xRibPlate.plate.anchors[1].ciType==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Axis X's direction point Y Coord ,by the datum plate local CS";
#else
			lpInfo->m_strPropHelp="�߰�X����ԭ����ʼ���ն�ָ���Y�����������׼�ְ��µľֲ�����";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Axis X's Y Coord ,by the datum plate local CS";
#else
			lpInfo->m_strPropHelp="�ְ�X�᷽��Y�����������׼�ְ�����ϵ�µ���Է���";
#endif

		}
	}
	else if(stricmp(propKey,"RibPlate.plate.bSpecAxisX")==0)
	{
		if(m_xRibPlate.m_cPlateType==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rib plate's parts in X-axis direction";
			lpInfo->m_strPropHelp = "parts (bolts or rods)used to determine rib plate's extending direction.";
			lpInfo->m_cmbItems="0.Along Rod|1.Direct to Bolt|2.Direct to Point|3.Input Directly";
#else
			lpInfo->m_strPropName = "X�᷽��";
			lpInfo->m_strPropHelp = "ָ���߰�����ϵX�᷽����������Ϊ��׼�ְ�����ϵ�µ�������ꡣ";
			lpInfo->m_cmbItems="0.�ظ˼�����|1.ָ����˨|2.�����|3.ֱ������";
#endif
		}
		else 
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rib plate's parts in Y-axis direction";
			lpInfo->m_strPropHelp = "parts (bolts or rods)used to determine rib plate's inclination degree.";
			lpInfo->m_cmbItems="0.Calculate Automatically|1.Input Directly";
#else
			lpInfo->m_strPropName = "�߰�Y�᷽�򹹼�";
			lpInfo->m_strPropHelp = "���������߰���б�ǶȵĹ���(��˨��˼�)��";
			lpInfo->m_cmbItems="0.���򹹼�|1.ֱ������";
#endif
		}
	}
	else if(stricmp(propKey,"m_pVecPart")==0)
	{
		if(m_xRibPlate.m_cPlateType==3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rib plate's parts in X-axis direction";
			lpInfo->m_strPropHelp = "parts (bolts or rods)used to determine rib plate's extending direction.";
			lpInfo->m_cmbItems="0.Along Rod|1.Direct to Bolt|2.Direct to Point|3.Input Directly";
#else
			//"�߰�X�᷽�򹹼�","���������߰����췽��Ĺ���(��˨��˼�)"
			if(m_xRibPlate.plate.anchors[1].ciType==1)
				lpInfo->m_strPropName = "�߰�X������˼�";
			else if(m_xRibPlate.plate.anchors[1].ciType==2)
				lpInfo->m_strPropName = "�߰�X��ָ����˨";
			else
				lpInfo->m_strPropName = "�߰�X�᷽�򹹼�";
			lpInfo->m_strPropHelp = "���������߰����췽��Ĺ���(��˨��˼�)��";
#endif
		}
		else 
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rib plate's parts in Y-axis direction";
			lpInfo->m_strPropHelp = "parts (bolts or rods)used to determine rib plate's inclination degree.";
			lpInfo->m_cmbItems="0.Calculate Automatically|1.Input Directly";
#else
			lpInfo->m_strPropName = "�߰�X�᷽�򹹼�";
			lpInfo->m_strPropHelp = "���������߰���б�ǶȵĹ���(��˨��˼�)��";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.plate.BendDirectVecPart")==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_xRibPlate.plate.anchors[3].ciType==1)
		{
			lpInfo->m_strPropName = "Vice Direct To Rod";
			lpInfo->m_strPropHelp = "Vice Direct To Rod";
		}
		else if(m_xRibPlate.plate.anchors[3].ciType==2)
		{
			lpInfo->m_strPropName = "Vice Direct To Bolt";
			lpInfo->m_strPropHelp = "Vice Direct To Bolt";
		}
#else
		if(m_xRibPlate.plate.anchors[3].ciType==1)
		{
			lpInfo->m_strPropName = "��������˼�";
			lpInfo->m_strPropHelp = "������ĩ�˸�������˼�";
		}
		else if(m_xRibPlate.plate.anchors[3].ciType==2)
		{
			lpInfo->m_strPropName = "����������˨";
			lpInfo->m_strPropHelp = "������ĩ�˸���������˨";
		}
#endif
	}
	else if(stricmp(propKey,"RibPlate.tube.iDatumEdge")==0)
	{
		if(m_pRelaPart1.IsNULL()&&m_pRelaPart2.IsNULL())
		{	//�ְ�չ�������׼
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="0.Datum Edge|1.Welding Line|2.Specify";
				lpInfo->m_strPropName = "Normal defined standard";
				lpInfo->m_strPropHelp = "the way to determine plate's normal";
#else
				lpInfo->m_cmbItems="0.��׼��|1.������|2.ָ������";
				lpInfo->m_strPropName = "��λ��׼��";
				lpInfo->m_strPropHelp = "�ְ嶨λ��׼�߻�ֱ��ָ������";
#endif
			}
		else
		{	//��׼��
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Datum Edge|1.Welding Line";
			lpInfo->m_strPropName = "datum line";
			lpInfo->m_strPropHelp = "plate positioning datum line";
#else
			lpInfo->m_cmbItems="0.��׼��|1.������";
			lpInfo->m_strPropName = "��λ��׼��";
			lpInfo->m_strPropHelp = "�ְ嶨λ��׼��";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.plate.bSpecAxisX")==0)
	{
		if(m_xRibPlate.m_cPlateType==3)
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Along Rod|1.Direct to Bolt|2.Direct to Point|3.Input Directly";
		else
			lpInfo->m_cmbItems="0.Calculate Automatically|1.Input Directly";
#else
			lpInfo->m_cmbItems="0.�ظ˼�����|1.ָ����˨|2.�����|3.ֱ������";
		else
			lpInfo->m_cmbItems="0.���򹹼�|1.ֱ������";
#endif
	}
	else if(stricmp(propKey,"RibPlate.plate.bSpecAxisY")==0)
	{
		if(m_xRibPlate.m_cPlateType!=3)
		{
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y axis";
		lpInfo->m_cmbItems="0.Direction Rod|1.Inclination Angle";
		lpInfo->m_strPropHelp = "determine axis Y direction, through axis Y direction rod or the inclination angle��degree measure)input.";
#else
		lpInfo->m_strPropName = "Y�᷽��";
		lpInfo->m_cmbItems="0.����˼�|1.��б�Ƕ�";
		lpInfo->m_strPropHelp = "ͨ��Y�᷽��˼����������б�Ƕ�(�Ƕ���)��ȷ��Y�᷽��";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y axis";
			lpInfo->m_cmbItems="0.Vertical Basic Plane|1.Inclination Angle";
			lpInfo->m_strPropHelp = "determine axis Y direction, through inputting inclination angle��degree measure).";
#else
			lpInfo->m_strPropName = "Y�᷽��";
			lpInfo->m_cmbItems="0.��ֱ����|1.��б�Ƕ�";
			lpInfo->m_strPropHelp = "ͨ���������б�Ƕ�(�Ƕ���)��ȷ��Y�᷽��";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.plate.bAlongVecPart")==0)
	{
		if(m_xRibPlate.m_cPlateType==3&&!m_xRibPlate.plate.bSpecAxisX)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "In the same direction with direction rod";
			lpInfo->m_strPropHelp = "Rib plate axis X is in the same direction with direction rod.";
#else
			lpInfo->m_strPropName = "�뷽��˼�ͬ��";
			lpInfo->m_strPropHelp = "�߰�X�᷽���뷽��˼�ͬ��";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "In the same direction with direction rod";
			lpInfo->m_strPropHelp = "Rib plate axis Y is in the same direction with direction rod.";
#else
			lpInfo->m_strPropName = "�뷽��˼�ͬ��";
			lpInfo->m_strPropHelp = "�߰�Y�᷽���뷽��˼�ͬ��";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.tube.fLeftTopCornerWidth")==0)
	{
		if(m_xRibPlate.tube.cLeftTopCornerType==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "cutting angle width";
			lpInfo->m_strPropHelp = "width of top cutting arc in X+ direction";
#else
			lpInfo->m_strPropName = "X���н���";
			lpInfo->m_strPropHelp = "X-�򶥲��нǿ��";
#endif
		}
		else if(m_xRibPlate.tube.cLeftTopCornerType==2)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "cutting arc radius";
			lpInfo->m_strPropHelp = "top cutting arc radius in X+ direction";
#else
			lpInfo->m_strPropName = "�л��뾶";
			lpInfo->m_strPropHelp = "X-�򶥲��л��뾶";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.tube.fRightTopCornerWidth")==0)
	{
		if(m_xRibPlate.tube.cLeftTopCornerType==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "cutting angle width";
			lpInfo->m_strPropHelp = "width of top cutting angle in X- direction";
#else
			lpInfo->m_strPropName = "X���н���";
			lpInfo->m_strPropHelp = "X+�򶥲��нǿ��";
#endif
		}
		else if(m_xRibPlate.tube.cLeftTopCornerType==2)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "cutting arc radius";
			lpInfo->m_strPropHelp = "top cutting arc radius in X- direction";
#else
			lpInfo->m_strPropName = "�л��뾶";
			lpInfo->m_strPropHelp = "X+�򶥲��л��뾶";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.fHeight")==0)
	{
		if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_CORNER::SUB_TYPE)
		{	//���߰�
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "top width D";
			lpInfo->m_strPropHelp = "Width of rib plate";
#else
			lpInfo->m_strPropName = "��������D";
			lpInfo->m_strPropHelp = "�߰嶥�����������";
#endif
		}
		else if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_ELBOW::SUB_TYPE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parameter C";
			lpInfo->m_strPropHelp = "Heigth of step";
#else
			lpInfo->m_strPropName = "̨�׸߶�C";
			lpInfo->m_strPropHelp = "̨�׸߶�";
#endif
		}
		else if(m_xRibPlate.m_cPlateType==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "top width D";
			lpInfo->m_strPropHelp = "Width of rib plate";
#else
			lpInfo->m_strPropName = "��������D";
			lpInfo->m_strPropHelp = "�߰嶥�����������";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.fCutAngleWidth")==0)
	{
		if(m_xRibPlate.m_cPlateType==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parameter E";
			lpInfo->m_strPropHelp = "cut angle width of rib plate";
#else
			lpInfo->m_strPropName = "A�൹��ֵ";
			lpInfo->m_strPropHelp = "�߰��ڸ����ְ�1�ĵ��ǿ��";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parameter E";
			lpInfo->m_strPropHelp = "cut angle width of rib plate";
#else
			lpInfo->m_strPropName = "���ǿ��E";
			lpInfo->m_strPropHelp = "�߰嵹�ǿ��";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.fTopWidth")==0)
	{
		if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_CORNER::SUB_TYPE)
		{	//���߰�
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parameter C";
			lpInfo->m_strPropHelp = "Heigth of step";
#else
			lpInfo->m_strPropName = "̨�׸߶�C";
			lpInfo->m_strPropHelp = "̨�׸߶�";
#endif
		}
		else if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_ELBOW::SUB_TYPE)
		{	//������߰�
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parameter D";
			lpInfo->m_strPropHelp = "Heigth of step";
#else
			lpInfo->m_strPropName = "��������D";
			lpInfo->m_strPropHelp = "����������";
#endif
		}
		else if(m_xRibPlate.m_cPlateType==1)
		{	//�����߰�
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "top width C";
			lpInfo->m_strPropHelp = "Width of rib plate, that is, the length of edge connected with supplementary plate 2, initial value automatically extends to the outer edge of supplementary plate 2.";
#else
			lpInfo->m_strPropName = "�������C";
			lpInfo->m_strPropHelp = "�߰嶥�����,���븨���ְ�2�����ߵĵı߳�,��ʼֵ�Զ������������ְ�2������Ե��";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.fBottomWidth")==0)
	{
		if(m_xRibPlate.m_cPlateType==0||m_xRibPlate.m_cPlateType==1)
		{	
			if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_ELBOW::SUB_TYPE)
			{	//���ʽ�߰�
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Parameter B";
				lpInfo->m_strPropHelp = "Width of bottom";
#else
				lpInfo->m_strPropName = "�ײ����B";
				lpInfo->m_strPropHelp = "���׼���������ߵı߳�,��ʼֵ�Զ���������׼�ְ������Ե��";
#endif
			}
			else
			{	//���߰�|�����߰�
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Parameter A";
					lpInfo->m_strPropHelp = "height of rib plate";
#else
					lpInfo->m_strPropName = "�߰峤��A";
					lpInfo->m_strPropHelp = "�븨������1�����ߵı߳�,��ʼֵ�Զ������������ְ������Ե";
#endif
			}
		}
		else if(m_xRibPlate.m_cPlateType==3)
		{	//�ְ��߰�
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "rib plate's height H";
			lpInfo->m_strPropHelp = "height of rib plate, that is, the length of edge which is vertical to datum part.";
#else
			lpInfo->m_strPropName = "�߰�߶�H";
			lpInfo->m_strPropHelp = "�߰�߶�,����ֱ�ڻ�׼�����ıߵĳ��ȡ�";
#endif
		}
		else if(m_xRibPlate.m_cPlateType==4)
		{	//�ֹ��߰�
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "rib plate width";
			lpInfo->m_strPropHelp = "Rib plate width, that is, the length of edge which is vertical to datum part.";
#else
			lpInfo->m_strPropName = "�߰���W";
			lpInfo->m_strPropHelp = "�ֹ��߰�Ŀ�ȡ�";
#endif
		}
	}
	else if(stricmp(propKey,"RibPlate.fLength")==0)
	{
		if(m_xRibPlate.m_cPlateType==0||m_xRibPlate.m_cPlateType==1)
		{
			if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_ELBOW::SUB_TYPE)
			{	//���ʽ�߰�
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Parameter A";
				lpInfo->m_strPropHelp = "height of rib plate";
#else
				lpInfo->m_strPropName = "�߰峤��A";
				lpInfo->m_strPropHelp = "�븨������1�����ߵı߳�,��ʼֵ�Զ������������ְ������Ե";
#endif
			}
			else
			{	//���߰�|�����߰�
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Parameter B";
				lpInfo->m_strPropHelp = "Width of bottom";
#else
				lpInfo->m_strPropName = "�ײ����B";
				lpInfo->m_strPropHelp = "���׼���������ߵı߳�,��ʼֵ�Զ���������׼�ְ������Ե��";
#endif
			}
		}
		else if(m_xRibPlate.m_cPlateType==3)
		{	//�ְ��߰�
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "rib plate width W";
			lpInfo->m_strPropHelp = "Parameter W, length of the edge connected with datum part.";
#else
			lpInfo->m_strPropName = "�߰���W";
			lpInfo->m_strPropHelp = "����W, ���׼���������ߵı߳���";
#endif
		}
		else if(m_xRibPlate.m_cPlateType==4)
		{	//�ֹ��߰�
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "rib plate height";
			lpInfo->m_strPropHelp = "minimum vertical distance in axis Z direction between datum plate's outer surface and rib plate's out edge.";
#else
			lpInfo->m_strPropName = "�߰�߶�H";
			lpInfo->m_strPropHelp = "��׼�ֹ���ھ��߰����Ե��СZ����ֱ���롣";
#endif
		}
	}
	else
		return false;
	return true;
}
CPropTreeItem* CDesignRibbedPlateDlg::InsertItem(CPropertyList* pPropList,CPropTreeItem *pParent, const char* propKey,
	int controlType/*=0*/,BOOL bUpdateValueText/*=TRUE*/, int iInsertIndex/*-1*/, BOOL bUpdate/*=0*/)
{
	PROPLIST_ITEM* pPropItem=propHashtable.GetValue(propKey);
	if(pPropItem==NULL)
		return NULL;
	CItemInfo* lpInfo = new CItemInfo();
	lpInfo->m_controlType=controlType;
	if(lpInfo->m_controlType==PIT_COMBO)
		lpInfo->m_cmbItems=pPropItem->propCmbItemsStr;
	else if(lpInfo->m_controlType==PIT_GRAYBUTTON)
		lpInfo->m_sButtonName=pPropItem->propCmbItemsStr;
	//
	lpInfo->m_strPropName=pPropItem->name;
	lpInfo->m_strPropHelp=pPropItem->propHelpStr;
	CorrectPropItemName(propKey,lpInfo);	//CorrectPropItemName����ֻ���°������ݣ�����Ӧ��֮ǰ����ʼֵ
	CXhChar50 sText;
	if(bUpdateValueText&&GetPropValueStr(pPropItem->id,sText)>0)
		lpInfo->m_strPropValue = sText;
	//CItemInfo* lpInfo=AppendPropItemInfo(propKey,controlType,&propId,bUpdateValueText);
	CPropTreeItem* pItem=pPropList->InsertItem(pParent,lpInfo,iInsertIndex,bUpdate);
	pItem->m_idProp=pPropItem->id;
	pItem->m_dwPropGroup=pParent->m_dwPropGroup;
	return pItem;
}
CPropTreeItem* CDesignRibbedPlateDlg::InsertOrUpdateItem(CPropertyList* pPropList,CPropTreeItem *pParent, const char* propKey,
	int controlType/*=0*/,BOOL bUpdateValueText/*=TRUE*/, int iInsertIndex/*-1*/, BOOL bUpdate/*=0*/)
{
	PROPLIST_ITEM* pPropItem=propHashtable.GetValue(propKey);
	if(pPropItem==NULL)
		return NULL;
	CItemInfo* lpInfo=NULL;
	CPropTreeItem* pItem=pPropList->FindItemByPropId(pPropItem->id,NULL);
	if(pItem)
		lpInfo=pItem->m_lpNodeInfo;
	else
		lpInfo = new CItemInfo();
	lpInfo->m_controlType=controlType;
	if(lpInfo->m_controlType==PIT_COMBO)
		lpInfo->m_cmbItems=pPropItem->propCmbItemsStr;
	//
	lpInfo->m_strPropName=pPropItem->name;
	lpInfo->m_strPropHelp=pPropItem->propHelpStr;
	CorrectPropItemName(propKey,lpInfo);	//CorrectPropItemName����ֻ���°������ݣ�����Ӧ��֮ǰ����ʼֵ
	CXhChar50 sText;
	if(bUpdateValueText&&GetPropValueStr(pPropItem->id,sText)>0)
		lpInfo->m_strPropValue = sText;
	if(pItem==NULL)
	{
		pItem=pPropList->InsertItem(pParent,lpInfo,iInsertIndex,bUpdate);
		pItem->m_idProp=pPropItem->id;
		pItem->m_dwPropGroup=pParent->m_dwPropGroup;
	}
	else if(bUpdate&&!pItem->IsHideSelf())
	{
		RECT rc;
		pPropList->GetItemRect(pItem->m_iIndex,&rc);
		pPropList->InvalidateRect(&rc);
	}
	return pItem;
}
static void UpdateRibPlateParaItems(CPropertyList *pPropList,BOOL bUpdate)
{
	CDesignRibbedPlateDlg *pRibPlateDlg=(CDesignRibbedPlateDlg*)pPropList->GetParent();
	CXhChar200 valueStr;
	CItemInfo *lpInfo=NULL;
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	CPropTreeItem *pLocationItem=pPropList->FindItemByPropId(CDesignRibbedPlateDlg::GetPropID("Position"),NULL);
	CPropTreeItem *pRelaInfoItem=pPropList->FindItemByPropId(CDesignRibbedPlateDlg::GetPropID("RelaInfo"),NULL);
	if(bUpdate)
	{	//ɾ������������װ���������
		pPropList->DeleteAllSonItems(pLocationItem);
		pPropList->DeleteAllSonItems(pRelaInfoItem);
	}
	//�߰����� 0.���߰� 1.���߰� 2.�Ǹ��߰� 3.�ְ��߰� 4.�ֹ��߰�
	BYTE cPlateType=pRibPlateDlg->m_xRibPlate.m_cPlateType;
	if(pRelaInfoItem)
	{	//��λ��׼����
		pPropItem=pRibPlateDlg->InsertItem(pPropList,pRelaInfoItem,"m_pDatumPart",PIT_BUTTON,TRUE,-1,bUpdate);
		pPropItem->SetReadOnly();
		//��λ��������
		if(0==cPlateType||1==cPlateType)	//0.���߰� //1.���߰�
		{	//��λ��������1
			pRibPlateDlg->InsertItem(pPropList,pRelaInfoItem,"m_pRelaPart1",PIT_BUTTON,TRUE,-1,bUpdate);
			//1.���߰�
			if(1==cPlateType)	//��λ��������2
				pRibPlateDlg->InsertItem(pPropList,pRelaInfoItem,"m_pRelaPart2",PIT_BUTTON,TRUE,-1,bUpdate);
		}
		//2.�Ǹ��߰� //4.�ֹ��߰�
		if(2==cPlateType||4==cPlateType)
			pRibPlateDlg->InsertItem(pPropList,pRelaInfoItem,"m_pDatumNode",PIT_BUTTON,TRUE,-1,bUpdate);	//��λ��׼�ڵ�
	}
	if(pLocationItem)
	{
		if(cPlateType==4)
		{	//�ֹ��߰�
			pPropItem=pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.tube.iDatumEdge",PIT_COMBO,TRUE,-1,TRUE);
			if(pRibPlateDlg->m_xRibPlate.tube.iDatumEdge!=2)
				pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.tube.fInitOffsetAngle",PIT_EDIT,TRUE,-1,TRUE);
			pPropItem=pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.tube.iWorkNormStyle",PIT_COMBO,TRUE,-1,TRUE);
			if(pRibPlateDlg->m_xRibPlate.tube.iWorkNormStyle!=0)
				pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.tube.fOffsetDistX",PIT_EDIT,TRUE,-1,TRUE);
			pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.tube.fOffsetDistM",PIT_EDIT,TRUE,-1,TRUE);
		}
		else if(pRibPlateDlg->m_pDatumPart.IsHasPtr()&&pRibPlateDlg->m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		{	//���|�ֹܲ��߰�
			if((pRibPlateDlg->m_pRelaPart1.IsHasPtr()&&pRibPlateDlg->m_pRelaPart2.IsNULL()&&pRibPlateDlg->m_pRelaPart1->IsPlate()))
				pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.bOnRelaPlateTopFace",PIT_COMBO,TRUE,-1,TRUE);
			pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.tube.iDatumEdge",PIT_COMBO,TRUE,-1,TRUE);
			pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.tube.extend_vec",PIT_COMBO,TRUE,-1,TRUE);
			pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.tube.fInitOffsetAngle",PIT_EDIT,TRUE,-1,TRUE);
			if(pRibPlateDlg->IsNewRibPlate())
			{	//�ֹ��߰����
				pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.tube.nPlateNum",PIT_EDIT,TRUE,-1,TRUE);
				if(pRibPlateDlg->m_xRibPlate.tube.nPlateNum>1)
					pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.tube.fOffsetAngle",PIT_EDIT,TRUE,-1,TRUE);
			}
		}
		else if(pRibPlateDlg->m_pDatumPart->IsPlate())
		{	//�Ƿ��ڸְ�����
			pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.bOnDatumPlateTopFace",PIT_COMBO,TRUE,-1,bUpdate);
			//��׼λ��
			pPropItem=pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.bSpecOrg",PIT_COMBO,TRUE,-1,bUpdate);
			if(pRibPlateDlg->m_xRibPlate.plate.bSpecOrg)
			{	//��׼λ�õ�X/Y ����
				pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.fOriginX",PIT_EDIT,TRUE,-1,bUpdate);
				pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.fOriginY",PIT_EDIT,TRUE,-1,bUpdate);	
			}
			else
				pRibPlateDlg->InsertItem(pPropList,pPropItem,"m_pDatumBolt",PIT_BUTTON,TRUE,-1,bUpdate);
			//���������
			if(0==cPlateType || 1==cPlateType)
			{	//X��
				pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.fOffsetDistZ",PIT_EDIT,TRUE,-1,bUpdate);
				pPropItem=pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.iAxisXType",PIT_COMBO,TRUE,-1,bUpdate);
				if(0==cPlateType)	//ָ��X�᷽��ʱ�������Զ��ж�λ�ڿ��Ӹ����ֵĶ��滹�ǵ���Ƚ����ѣ�ĿǰҲ��Ҫ�ֶ�ָ�����Ӳ� wjh-2015.9.19
					pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.bOnRelaPlateTopFace",PIT_COMBO,TRUE,-1,bUpdate);
				if(pRibPlateDlg->m_xRibPlate.plate.iAxisXType<5)
				{
					pSonPropItem=pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.fXAxisX",PIT_EDIT,TRUE,-1,bUpdate);
					pSonPropItem->SetReadOnly(pRibPlateDlg->m_xRibPlate.plate.iAxisXType!=4);
					pSonPropItem=pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.fXAxisY",PIT_EDIT,TRUE,-1,bUpdate);
					pSonPropItem->SetReadOnly(pRibPlateDlg->m_xRibPlate.plate.iAxisXType!=4);	
				}
				//Y��(0.�ط���˼�|1.��б�Ƕ�)
				pPropItem=pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.bSpecAxisY",PIT_COMBO,TRUE,-1,bUpdate);
				if(pRibPlateDlg->m_xRibPlate.plate.bSpecAxisY)	
					pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.fGradientDegAngle",PIT_EDIT,TRUE,-1,bUpdate);
				else
					pRibPlateDlg->InsertItem(pPropList,pPropItem,"m_pVecPart",PIT_BUTTON,TRUE,-1,bUpdate);
			}
			else if(3==cPlateType)
			{	//X��
				pPropItem=pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.bSpecAxisX",PIT_COMBO,TRUE,-1,bUpdate);
				if(pRibPlateDlg->m_xRibPlate.plate.bSpecAxisX||pRibPlateDlg->m_xRibPlate.plate.anchors[1].ciType==3)
				{
					pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.fXAxisX",PIT_EDIT,TRUE,-1,bUpdate);
					pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.fXAxisY",PIT_EDIT,TRUE,-1,bUpdate);
				}
				else
				{
					pRibPlateDlg->InsertItem(pPropList,pPropItem,"m_pVecPart",PIT_BUTTON,TRUE,-1,bUpdate);
					if(pRibPlateDlg->m_pVecPart.IsHasPtr()&&pRibPlateDlg->m_pVecPart->IsLinePart())
						pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.bAlongVecPart",PIT_COMBO,TRUE,-1,bUpdate);
				}
				//Y��
				pPropItem=pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.bSpecAxisY",PIT_COMBO,TRUE,-1,bUpdate);
				if(pRibPlateDlg->m_xRibPlate.plate.bSpecAxisY)
					pRibPlateDlg->InsertItem(pPropList,pPropItem,"RibPlate.plate.fGradientDegAngle",PIT_EDIT,TRUE,-1,bUpdate);
				pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.fOffsetDistX",PIT_EDIT,TRUE,-1,bUpdate);
				pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.fOffsetDistZ",PIT_EDIT,TRUE,-1,bUpdate);
				//��չ�����߰幦�ܺ���������� wjh-2016.5.4
				CPropTreeItem* pBendingItem=pRibPlateDlg->InsertItem(pPropList,pLocationItem,"RibPlate.plate.IsHasBendPlane",PIT_COMBO,TRUE,-1,bUpdate);
				UpdateRibBendPlaneParam(pPropList,pBendingItem,bUpdate);
			}
		}
	}
}

BOOL CDesignRibbedPlateDlg::LoadParamsFrom(CLDSParamPlate* pParamPlate)
{
	memset(&m_xRibPlate.params,0,sizeof(RIB_PLATE::RIB_PARAMS));
	long hDatumPart=0,hNode=0,hAssPart1=0,hAssPart2=0,work_norm_style=0;
	if(!pParamPlate->GetDesignItemValue('A',&hDatumPart))//��׼����
		return FALSE;
	pParamPlate->GetDesignItemValue('B',&hNode);		//��׼�ڵ�
	pParamPlate->GetDesignItemValue('C',&hAssPart1);	//��������1
	pParamPlate->GetDesignItemValue('D',&hAssPart2);	//��������2
	CLDSNode *pDatumNode=NULL;
	CLDSPart *pDatumPart=NULL,*pAssistantPart1=NULL,*pAssistantPart2=NULL;
	pDatumNode=pParamPlate->BelongModel()->FromNodeHandle(hNode);
	pDatumPart=pParamPlate->BelongModel()->FromPartHandle(abs(hDatumPart),CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
	pAssistantPart1=pParamPlate->BelongModel()->FromPartHandle(abs(hAssPart1),CLS_PLATE,CLS_PARAMPLATE);
	pAssistantPart2=pParamPlate->BelongModel()->FromPartHandle(hAssPart2,CLS_PLATE,CLS_PARAMPLATE);
	if( pDatumPart!=NULL&&pAssistantPart1!=NULL&&pAssistantPart2==NULL&&
		pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pAssistantPart1->IsPlate())
	{	//������߰�
		m_xRibPlate.params.elbow.LoadParams(pParamPlate,NULL);
		return TRUE;
	}
	if( pDatumPart!=NULL&&pDatumPart->IsPlate()&&
		pAssistantPart1==NULL&&pAssistantPart2==NULL)
	{	//�ְ��ϵ���ͨ�����߰�
		m_xRibPlate.params.platerib.LoadParams(pParamPlate,NULL);
		return TRUE;
	}
	if( pDatumPart!=NULL&&pDatumPart->GetClassTypeId()==CLS_LINETUBE&&
		pAssistantPart1==NULL&&pAssistantPart2==NULL)
	{	//�ֹ��ϵ���ͨ�����߰�
		m_xRibPlate.params.tuberib.LoadParams(pParamPlate,NULL);
		return TRUE;
	}
	if(pDatumPart!=NULL&&pDatumPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		m_xRibPlate.params.anglerib.LoadParams(pParamPlate,NULL);
		return TRUE;
	}
	if( pDatumPart!=NULL&&pDatumPart->IsPlate()&&(pAssistantPart1==NULL||
		(pAssistantPart1!=NULL&&pAssistantPart1->IsPlate())))
	{
		m_xRibPlate.params.corner.LoadParams(pParamPlate,NULL);
		return TRUE;
	}
	//if(pDatumPart==NULL)
		return FALSE;
}
void CDesignRibbedPlateDlg::DisplayPropertyList()
{
	CPropTreeItem *pPropItem,*pGroupItem,*pRoot=m_propList.GetRootItem();
	CItemInfo* lpInfo=NULL;
	char valueStr[100]="";
	m_propList.CleanTree();
	//�����������ص�����
	m_propList.SetModifyValueFunc(ModifyItemValue);
	m_propList.SetButtonClickFunc(ButtonClickItem);
	m_propList.SetPropHelpPromptFunc(FirePropHelpPrompt);
	//
	//if(m_bNewRibPlate)
	//{//������Ϣ
		pGroupItem=InsertItem(&m_propList,pRoot,"Basic",0,FALSE);
		pGroupItem->m_bHideChildren = FALSE;
		//�������
		InsertItem(&m_propList,pGroupItem,"RibPlate.sPartNo",PIT_EDIT);
		//�κ�
		InsertItem(&m_propList,pGroupItem,"RibPlate.nSegI",PIT_EDIT);
		//����
		InsertItem(&m_propList,pGroupItem,"RibPlate.cMaterial",PIT_COMBO);
		//���
		InsertItem(&m_propList,pGroupItem,"RibPlate.nThick",PIT_EDIT);
	//}
	//�߰�����
	pPropItem=InsertItem(&m_propList,pGroupItem,"RibPlate.m_cPlateType",PIT_COMBO);
	pPropItem->SetReadOnly(TRUE);
	//pPropItem->SetReadOnly(!m_bNewRibPlate);
	//������Ϣ
	pGroupItem=InsertItem(&m_propList,pRoot,"RelaInfo",0,FALSE);
	pGroupItem->m_bHideChildren = FALSE;
	//��λ����
	pGroupItem=InsertItem(&m_propList,pRoot,"Position",0,FALSE);
	pGroupItem->m_bHideChildren = FALSE;
	//�����߰������Ϣ�Ͷ�λ����
	UpdateRibPlateParaItems(&m_propList,FALSE);
	m_propList.Redraw();
}
void CDesignRibbedPlateDlg::DisplaySubPropList(CPropTreeItem *pItem,BOOL bUpdate/*=FALSE*/)
{
	CPropTreeItem *pPropItem,*pGroupItem,*pRoot=m_xSubPropList.GetRootItem();
	CItemInfo* lpInfo=NULL;
	char valueStr[100]="";
	m_xSubPropList.SetModifyValueFunc(ModifyItemValue);
	m_xSubPropList.SetButtonClickFunc(ButtonClickItem);
	m_xSubPropList.SetPropHelpPromptFunc(FireSubPropHelpPrompt);
	if(!bUpdate)
		m_xSubPropList.CleanTree();
	if(pItem&&pItem->m_idProp==GetPropID("RibPlate.tube.iDatumEdge")&&pItem->m_lpNodeInfo->m_strPropValue.GetLength()>0
		&&!pItem->IsReadOnly()&&m_xRibPlate.tube.iDatumEdge==2)	//������Ʋ���
	{	//չ������
		pGroupItem=InsertItem(&m_xSubPropList,pRoot,"axis_x");
		pPropItem=InsertItem(&m_xSubPropList,pGroupItem,"ucs.axis_x",PIT_COMBO);
		// X/Y/Z����
		InsertItem(&m_xSubPropList,pPropItem,"ucs.axis_x.x",PIT_EDIT);
		InsertItem(&m_xSubPropList,pPropItem,"ucs.axis_x.y",PIT_EDIT);
		InsertItem(&m_xSubPropList,pPropItem,"ucs.axis_x.z",PIT_EDIT);
	}
	else if(pItem&&pItem->m_idProp==GetPropID("RibPlate.tube.iWorkNormStyle")&&pItem->m_lpNodeInfo->m_strPropValue.GetLength()>0
		&&!pItem->IsReadOnly()&&m_xRibPlate.tube.iWorkNormStyle==2)	//������Ʋ���
	{	//���߷���
		pGroupItem=InsertItem(&m_xSubPropList,pRoot,"axis_z");
		pPropItem=InsertItem(&m_xSubPropList,pGroupItem,"ucs.axis_z",PIT_COMBO);
		// X/Y/Z����
		InsertItem(&m_xSubPropList,pPropItem,"ucs.axis_z.x",PIT_EDIT);
		InsertItem(&m_xSubPropList,pPropItem,"ucs.axis_z.y",PIT_EDIT);
		InsertItem(&m_xSubPropList,pPropItem,"ucs.axis_z.z",PIT_EDIT);
	}
	else 
	{	//��ʾ���β���
		pGroupItem=InsertItem(&m_xSubPropList,pRoot,"Profile");
		pGroupItem->m_bHideChildren = FALSE;
		if(m_pDatumPart->IsTube())
		{	//�ֹܰ뾶 wht 11-05-24
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "tube radius";
			lpInfo->m_strPropHelp = "datum tube radius";
#else
			lpInfo->m_strPropName = "�ֹܰ뾶";
			lpInfo->m_strPropHelp = "��׼�ֹܰ뾶��";
#endif
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_lpNodeInfo->m_strPropValue.Format("%.1f",m_pDatumPart.LineTubePointer()->GetDiameter()*0.5);
			pPropItem->SetReadOnly(TRUE);
		}
		//��ֱ�Ҵ��Զ���λ���߰�
		if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE&&m_pRelaPart1.IsHasPtr()
			&&m_pRelaPart1->IsPlate()&&m_pRelaPart2.IsNULL())
		{	//��ֱ�Ҵ��Զ���λ���߰�
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.tube.bVerticalPlate",PIT_COMBO);
			pPropItem=InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.tube.bAutoCalB",PIT_COMBO);
			if(m_xRibPlate.tube.bAutoCalB)
				InsertItem(&m_xSubPropList,pPropItem,"RibPlate.tube.margin",PIT_EDIT);
		}
		if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_ELBOW::SUB_TYPE)
		{	//����ͽ��߰�
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fLength",PIT_EDIT);			//�߰峤��(����A)
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fBottomWidth",PIT_EDIT);	//�ײ����(����B)
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fHeight",PIT_EDIT);			//̨�׸߶�(����C)
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fTopWidth",PIT_EDIT);		//�������(����D)
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.iCutAngleMode",PIT_COMBO);	//����ģʽ
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fCutAngleWidth",PIT_EDIT);	//�нǿ�ȣ�����E��
			CPropTreeItem* pHoleItem=InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.MH",PIT_EDIT);	//�����׿׾�H
			if(m_xRibPlate.MH>0)
			{
				InsertItem(&m_xSubPropList,pHoleItem,"RibPlate.MF",PIT_EDIT);	//�����׿׾����F
				InsertItem(&m_xSubPropList,pHoleItem,"RibPlate.MG",PIT_EDIT);	//�����׿׾����G
			}
		}
		else if(m_xRibPlate.m_cPlateType==0&&m_xRibPlate.m_cSubType==CLDSParamPlate::RIBPLATE_CORNER::SUB_TYPE)
		{	//��ͨ�ͽ��߰壬��������ѥ����װ����߰�
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fBottomWidth",PIT_EDIT);//�ײ����(����A��
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fLength",PIT_EDIT);		//����(����B)
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fTopWidth",PIT_EDIT);	//̨�׸߶�(����C)
			if(fabs(m_xRibPlate.fTopWidth-m_xRibPlate.fBottomWidth)>EPS)
			{	//�����߶���ײ��߶Ȳ��ȸ�ʱ���߶���Ч wht 11-05-19
				InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fHeight",PIT_EDIT);		//��������(����D)
			}
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.iCutAngleMode",PIT_COMBO);	//����ģʽ
			InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fCutAngleWidth",PIT_EDIT);	//�нǿ�ȣ�����E��
			CPropTreeItem* pHoleItem=InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.MH",PIT_EDIT);	//�����׿׾�H
			if(m_xRibPlate.MH>0)
			{
				InsertItem(&m_xSubPropList,pHoleItem,"RibPlate.MF",PIT_EDIT);	//�����׿׾����F
				InsertItem(&m_xSubPropList,pHoleItem,"RibPlate.MG",PIT_EDIT);	//�����׿׾����G
			}
		}
		else if(m_xRibPlate.m_cPlateType>=3 && m_xRibPlate.m_cPlateType<=4)
		{	//�ְ��߰�/�ֹ��߰� �Ǹ��߰�ʹ�õ����Ի�����ʾ����
			if(m_pDatumPart->IsPlate() || m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
			{
				InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fBottomWidth",PIT_EDIT);//�ײ����
				InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fLength",PIT_EDIT);		//�߰�߶�
				if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
				{	//�ֹ��߰�
					//X+�򶥲��߽�����
					pPropItem=InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.tube.cRightTopCornerType",PIT_COMBO,TRUE,-1,TRUE);		
					if(m_xRibPlate.tube.cRightTopCornerType!=0)	//�нǿ��/�л��뾶
						InsertItem(&m_xSubPropList,pPropItem,"RibPlate.tube.fRightTopCornerWidth",PIT_EDIT,TRUE,-1,TRUE);
					if(m_xRibPlate.tube.cRightTopCornerType==1)
						InsertItem(&m_xSubPropList,pPropItem,"RibPlate.tube.fRightTopCornerHeight",PIT_EDIT,TRUE,-1,TRUE);
					//X-�򶥲��߽�����
					pPropItem=InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.tube.cLeftTopCornerType",PIT_COMBO,TRUE,-1,TRUE);
					if(m_xRibPlate.tube.cLeftTopCornerType!=0)	//�нǿ��/�л��뾶
						InsertItem(&m_xSubPropList,pPropItem,"RibPlate.tube.fLeftTopCornerWidth",PIT_EDIT,TRUE,-1,TRUE);
					if(m_xRibPlate.tube.cLeftTopCornerType==1)
						InsertItem(&m_xSubPropList,pPropItem,"RibPlate.tube.fLeftTopCornerHeight",PIT_EDIT,TRUE,-1,TRUE);
				}
			}
		}
		else if(m_xRibPlate.m_cPlateType==1)
		{	//�����߰�(��������������)
			if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->IsPlate())
			{
				if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
					pPropItem=InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.bAutoExtendOutterEdge",PIT_COMBO);
				else //��׼�����ְ�ʱ��֧�ֱ�Ե����,Ϊ�������´���˴���Ҫ��pPropItem=pGroupItem wht 11-07-25
					pPropItem=pGroupItem;	
				//
				if(m_xRibPlate.bAutoExtendOutterEdge&&m_pDatumPart->IsTube())
				{	
					InsertItem(&m_xSubPropList,pPropItem,"RibPlate.tube.fTopOutterR",PIT_EDIT);		//�����⻷�뾶
					InsertItem(&m_xSubPropList,pPropItem,"RibPlate.tube.fBottomOutterR",PIT_EDIT);	//�ײ����ΰ뾶
				}
				else
				{	
					InsertItem(&m_xSubPropList,pPropItem,"RibPlate.fBottomWidth",PIT_EDIT);			//�ײ����(����A��
					InsertItem(&m_xSubPropList,pPropItem,"RibPlate.fTopWidth",PIT_EDIT);			//�������(����C)
				}
				pPropItem=InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fLength",PIT_EDIT);	//����(����B)
				pPropItem->SetReadOnly();
				if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE||fabs(m_xRibPlate.fTopWidth-m_xRibPlate.fBottomWidth)>EPS)
					InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fHeight",PIT_EDIT);			//��������D
				InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.iCutAngleMode",PIT_COMBO);		//����ģʽ
				InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fCutAngleWidth",PIT_EDIT);		//�нǿ��E
				InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.fCutAngleWidth2",PIT_EDIT);		//�нǿ��2
				CPropTreeItem* pHoleItem=InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.MH",PIT_EDIT);	//�����׿׾�H
				if(m_xRibPlate.MH>0)
				{
					InsertItem(&m_xSubPropList,pHoleItem,"RibPlate.MF",PIT_EDIT);	//�����׿׾����F
					InsertItem(&m_xSubPropList,pHoleItem,"RibPlate.MG",PIT_EDIT);	//�����׿׾����G
				}
			}
			else if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->GetClassTypeId()==CLS_LINETUBE)
			{	//�ֹܼа�	
				;
			}
		}
		if(m_bNewRibPlate)
		{	//�������߰�ʱ��ʾ�������� wht 11-05-04
			if(m_pDatumPart->IsPlate()||(m_pRelaPart1.IsHasPtr()
				&&m_pRelaPart1->IsPlate()&&m_xRibPlate.tube.nPlateNum==1))
			{	//ͬʱ��������߰�
				pPropItem=InsertItem(&m_xSubPropList,pGroupItem,"RibPlate.bCreateOuterPlate",PIT_COMBO);
				if(m_xRibPlate.bCreateOuterPlate)
				{	
					InsertItem(&m_xSubPropList,pPropItem,"RibPlate.fOuterPlateLen",PIT_EDIT);		//����߰峤��
					InsertItem(&m_xSubPropList,pPropItem,"RibPlate.fOuterPlateHeight",PIT_EDIT);	//����߰�߶�
				}
			}
		}
	}
	m_xSubPropList.Redraw();
}
BOOL CDesignRibbedPlateDlg::RedrawSketchDrawing(CPropTreeItem *pItem,UCS_STRU *pObjectCS/*=NULL*/,bool removeOldEntities/*=true*/)
{
	if(pItem==NULL)
		return FALSE;
	DelWorkPlaneSketch();
	if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.iDatumEdge")==pItem->m_idProp)
		WorkPlaneSketch(TYPE_PLANE_SPREAD);
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.iWorkNormStyle")==pItem->m_idProp)
		WorkPlaneSketch(TYPE_PLANE_NORMAL);
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fLeftTopCornerWidth")==pItem->m_idProp||
		CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fRightTopCornerWidth")==pItem->m_idProp||
		CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.cLeftTopCornerType")==pItem->m_idProp||
		CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.cRightTopCornerType")==pItem->m_idProp)
		WorkPlaneSketch(TYPE_X_AXIS_VECTOR);
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fLeftTopCornerHeight")==pItem->m_idProp||
		CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fRightTopCornerHeight")==pItem->m_idProp)
		WorkPlaneSketch(TYPE_Y_AXIS_VECTOR);
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetDistM")==pItem->m_idProp)
		WorkPlaneSketch(TYPE_TUBE_SPREAD);
	else if(CDesignRibbedPlateDlg::GetPropID("RibPlate.tube.fOffsetDistX")==pItem->m_idProp)
		WorkPlaneSketch(TYPE_X_AXIS_VECTOR);
	return TRUE;
}
void CDesignRibbedPlateDlg::OnOK() 
{
	//CDialog::OnOK();
}
//�Ǹ��߰�: A.�Ǹ�+��׼�ڵ�
//�ֹ��߰�:	B.�ֹ�+��׼�ڵ�
//�ְ��߰�: C.�ְ�+��׼��˨+���򹹼�
//  ���߰�: D.�ְ�+�����ְ�+��˨��λ
//			E.�ֹ�+�����ְ�(��:�ֹ����)
//			F.�ֹ�+�ֹ�(��:�ֹܼа�)	
//  ���߰�: G.�ְ�+2�������ְ�+��˨��λ
//			H.�ֹ�+2�������ְ�
void CDesignRibbedPlateDlg::FillParamToPlate(CLDSParamPlate *pRibPlate)
{
	pRibPlate->m_iParamType=TYPE_RIBPLATE;
	pRibPlate->m_hPartWeldParent=m_pDatumPart->handle;
	pRibPlate->cfgword=m_pDatumPart->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	pRibPlate->thick=m_xRibPlate.nThick;
	pRibPlate->iSeg=m_xRibPlate.nSegI;
	pRibPlate->cMaterial = m_xRibPlate.cMaterial;
	pRibPlate->SetPartNo(m_xRibPlate.sPartNo);
	int nDatumFlag=-1,nRelaFlag=-1;
	if(m_xRibPlate.bOnRelaPlateTopFace)
		nRelaFlag=1;
	if(m_xRibPlate.plate.bOnDatumPlateTopFace)
		nDatumFlag=1;
	if(m_pRelaPart1.IsNULL()&&m_pRelaPart2.IsNULL())
	{	//�ְ��߰��ֹ��߰�
		//��׼�ְ�Ϊ�ֹ����޸�������ʱ���������ʾ�ְ����췽���Ƿ���ֹ����췽����ͬ
		if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
		{
			pRibPlate->SetDesignItemValue('B',m_pDatumNode->handle,"",PARAM_POS,1);				//��׼�ڵ�
			pRibPlate->SetDesignItemValue('A',nRelaFlag*m_pDatumPart->handle,"",PARAM_POS,1);	//��׼����
		}
		else if(m_pDatumPart->IsPlate())
		{	//��׼����Ϊ�ְ����޸����ְ�ʱ��Ҫ��λ��׼��˨�Լ�����λ����
			pRibPlate->SetDesignItemValue('A',nDatumFlag*m_pDatumPart->handle,"",PARAM_POS,1);	//��׼����
			if(m_pVecPart.IsHasPtr()&&m_pVecPart->IsLinePart()&&!m_xRibPlate.plate.bAlongVecPart)
				pRibPlate->SetDesignItemValue('V',-1*m_pVecPart->handle,"",PARAM_POS,1);	//�߰�X�᷽��λ����(�˼�����˨)
			else if(m_pVecPart.IsHasPtr())
				pRibPlate->SetDesignItemValue('V',m_pVecPart->handle,"",PARAM_POS,1);		//�߰�X�᷽��λ����(�˼�����˨)
			if(m_pDatumBolt)
				pRibPlate->SetDesignItemValue('O',m_pDatumBolt->handle,"",PARAM_POS,1);		//�߰嶨λ��׼��˨
			else if(m_pDatumNode)
				pRibPlate->SetDesignItemValue('B',m_pDatumNode->handle,"",PARAM_POS,1);		//��׼�ڵ�
			if(m_xRibPlate.m_cPlateType==3)
			{	//��չ�ư��߰幦�ܺ���������� wjh-2016.5.4
				ANCHOR_PARAM* anchors=&m_xRibPlate.plate.anchors[0];
				if(anchors[0].ciType==3||m_pDatumBolt==NULL)
				{	//ָ���߰��׼�������(ʼ->ĩ)��λ��
					m_xRibPlate.plate.fOriginX=anchors[0].locate.vertex.x;
					m_xRibPlate.plate.fOriginY=anchors[0].locate.vertex.y;
					pRibPlate->SetDesignItemValue(KEY2C("Ox"),anchors[0].locate.vertex.x,"",PARAM_POS);	//O->EΪ�߰��׼�������ʼ->ĩ��λ����
					pRibPlate->SetDesignItemValue(KEY2C("Oy"),anchors[0].locate.vertex.y,"",PARAM_POS);	//O->EΪ�߰��׼�������ʼ->ĩ��λ����
				}
				if(anchors[1].ciType==3||m_pVecPart.IsNULL())
				{	//ָ���߰��׼�������(ʼ->ĩ)��λ��
					pRibPlate->SetDesignItemValue(KEY2C("Ex"),anchors[1].locate.vertex.x,"",PARAM_POS);	//O->EΪ�߰��׼�������ʼ->ĩ��λ����
					pRibPlate->SetDesignItemValue(KEY2C("Ey"),anchors[1].locate.vertex.y,"",PARAM_POS);	//O->EΪ�߰��׼�������ʼ->ĩ��λ����
				}
				if(anchors[2].ciType==2)
				{	//������ĩ��T��Ļ�׼��˨
					pRibPlate->SetDesignItemValue(KEY2C("TB"),anchors[2].locate.hBolt,"",PARAM_POS,1);	//B->TΪ�߰������������ʼ->ĩ��λ����
				}
				else if(anchors[2].ciType==3)
				{	//������ĩ��T����û����붨��ֵ
					pRibPlate->SetDesignItemValue(KEY2C("Tx"),anchors[2].locate.vertex.x,"",PARAM_POS);	//B->TΪ�߰������������ʼ->ĩ��λ����
					pRibPlate->SetDesignItemValue(KEY2C("Ty"),anchors[2].locate.vertex.y,"",PARAM_POS);	//B->TΪ�߰������������ʼ->ĩ��λ����
				}
				if(anchors[3].ciType==1||anchors[3].ciType==2)
				{	//ָ���߰������������(ʼ->ĩ)��λ��˨����˼�
					if(anchors[3].ciType==1)
						pRibPlate->SetDesignItemValue(KEY2C("BV"),anchors[3].locate.hVecRod,"",PARAM_POS,1);//B->TΪ�߰������������ʼ->ĩ��λ����
					else
						pRibPlate->SetDesignItemValue(KEY2C("BV"),anchors[3].locate.hBolt,"",PARAM_POS,1);	//B->TΪ�߰������������ʼ->ĩ��λ����
				}
				else if(anchors[3].ciType==3)
				{	//ָ���߰������������(ʼ->ĩ)��λ��
					pRibPlate->SetDesignItemValue(KEY2C("Bx"),anchors[3].locate.vertex.x,"",PARAM_POS);	//B->TΪ�߰������������ʼ->ĩ��λ����
					pRibPlate->SetDesignItemValue(KEY2C("By"),anchors[3].locate.vertex.y,"",PARAM_POS);	//B->TΪ�߰������������ʼ->ĩ��λ����
				}
				pRibPlate->SetDesignItemValue(KEY2C("X"),m_xRibPlate.plate.fOffsetDistX,"",PARAM_POS);		//X����ƫ����
				pRibPlate->SetDesignItemValue(KEY2C("Z"),m_xRibPlate.plate.fOffsetDistZ,"",PARAM_POS);		//Z����ƫ����
				pRibPlate->SetDesignItemValue(KEY2C("dx"),m_xRibPlate.plate.fBendTipOffsetX,"",PARAM_POS);
				pRibPlate->SetDesignItemValue(KEY2C("dz"),m_xRibPlate.plate.fBendTipOffsetZ,"",PARAM_POS);
			}
		}
	}
	else if(m_pRelaPart1.IsHasPtr())
	{	//���߰����߰�
		int signOfTubeDirect=nDatumFlag;
		if(m_pDatumPart->IsTube()&&m_pRelaPart1->IsPlate())
		{
			GEPOINT rodvec=m_pDatumPart.LineTubePointer()->End()-m_pDatumPart.LineTubePointer()->Start();
			if(nRelaFlag*(rodvec*m_pRelaPart1->ucs.axis_z)<0)
				signOfTubeDirect=-1;
			else
				signOfTubeDirect=1;
		}
		pRibPlate->SetDesignItemValue('A',signOfTubeDirect*m_pDatumPart->handle,"",PARAM_POS,1);	//��׼����
		pRibPlate->SetDesignItemValue('C',nRelaFlag*m_pRelaPart1->handle,"",PARAM_POS,1);			//�߰嶨λ��������1
		if(m_pRelaPart2.IsHasPtr())
			pRibPlate->SetDesignItemValue('D',m_pRelaPart2->handle,"",PARAM_POS,1);					//�߰嶨λ��������2
		if(m_pDatumPart->IsPlate()) //��׼����Ϊ�ְ�ʱ��Ҫ��˨��λ
		{
			if(m_pVecPart.IsHasPtr()&&m_pVecPart->IsLinePart())
				pRibPlate->SetDesignItemValue('V',m_pVecPart->handle,"",PARAM_POS,1);	//�߰�Y�᷽��λ����
			else 
				pRibPlate->SetDesignItemValue('V',0,"",PARAM_POS,1);					//�߰�Y�᷽��λ����
			if(m_pDatumBolt)
				pRibPlate->SetDesignItemValue('O',m_pDatumBolt->handle,"",PARAM_POS,1);	//�߰嶨λ��׼��˨
			else if(m_pDatumNode)
				pRibPlate->SetDesignItemValue('B',m_pDatumNode->handle,"",PARAM_POS,1);	//��׼�ڵ�
		}
	}
	if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{	//��׼����Ϊ�ֹ�
		int J=m_xRibPlate.tube.iDatumEdge;
		double rotAngle=m_xRibPlate.tube.fInitOffsetAngle;
		bool bElbowPlate=false;
		if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->IsPlate())
		{	//�ֹ����
			if(m_pDatumBolt)
				J=2;	//2.��λ��˨
			else
				J=m_xRibPlate.params.elbow.J.value.iValue;
			if(J<=1)
				rotAngle=m_xRibPlate.params.elbow.rotAngle.ToDouble();
			bElbowPlate=true;
		}
		if(bElbowPlate)
			pRibPlate->SetDesignItemValue(KEY2C("mn"),m_xRibPlate.params.elbow.margin.value.iValue,"",PARAM_SHAPE,1);	//margin
		if(!bElbowPlate)
		{
			pRibPlate->SetDesignItemValue(KEY2C("Nt"),m_xRibPlate.tube.iWorkNormStyle,"",PARAM_POS,1);	//�������ͣ�0.��ֱ�ֹ����� 1.�ֹ����� 2.�Զ���
			if(m_xRibPlate.tube.iWorkNormStyle==2)
			{
				pRibPlate->SetDesignItemValue(KEY2C("Nx"),ucs.axis_z.x,"",PARAM_POS);	//�߰巨��X����
				pRibPlate->SetDesignItemValue(KEY2C("Ny"),ucs.axis_z.y,"",PARAM_POS);	//�߰巨��Y����
				pRibPlate->SetDesignItemValue(KEY2C("Nz"),ucs.axis_z.z,"",PARAM_POS);	//�߰巨��Z����
			}
		}
		pRibPlate->SetDesignItemValue('J',J,"",PARAM_POS,1);	//�߰嶨λ��׼��:0.��׼�� 1.������ 2.ָ������
		if(J==2&&!bElbowPlate)
		{
			pRibPlate->SetDesignItemValue(KEY2C("Wx"),ucs.axis_x.x,"",PARAM_POS);	//�߰�չ������X����
			pRibPlate->SetDesignItemValue(KEY2C("Wy"),ucs.axis_x.y,"",PARAM_POS);	//�߰�չ������Y����
			pRibPlate->SetDesignItemValue(KEY2C("Wz"),ucs.axis_x.z,"",PARAM_POS);	//�߰�չ������Z����
		}
		else if(J<=1)
			pRibPlate->SetDesignItemValue('K',rotAngle,"",PARAM_POS,0);		//�߰����׼��֮���ƫ�ƽǶ�
		else if(J==2&&bElbowPlate)	//ָ����λ��˨
			pRibPlate->SetDesignItemValue('O',m_xRibPlate.params.elbow.hDatumBolt.hValue,"",PARAM_POS,1);	//�߰嶨λ��׼��˨
		else if(J==3&&bElbowPlate)	//ָ���ױ�����
		{
			pRibPlate->SetDesignItemValue(KEY2C("Xx"),m_xRibPlate.params.elbow.vAxisXVecX.value.fValue,"",PARAM_POS);	//�ױ߷���궨X����
			pRibPlate->SetDesignItemValue(KEY2C("Xy"),m_xRibPlate.params.elbow.vAxisXVecY.value.fValue,"",PARAM_POS);	//�ױ߷���궨Y����
			pRibPlate->SetDesignItemValue(KEY2C("Xz"),m_xRibPlate.params.elbow.vAxisXVecZ.value.fValue,"",PARAM_POS);	//�ױ߷���궨Z����
		}
		else if(J==4&&bElbowPlate)	//ָ���ױ�ʰȡ��
		{
			pRibPlate->SetDesignItemValue(KEY2C("Xx"),m_xRibPlate.params.elbow.vAxisXVecX.value.fValue,"",PARAM_POS);	//�ױ߷���궨X����
			pRibPlate->SetDesignItemValue(KEY2C("Xy"),m_xRibPlate.params.elbow.vAxisXVecY.value.fValue,"",PARAM_POS);	//�ױ߷���궨Y����
		}
		if(m_xRibPlate.bAutoExtendOutterEdge)
			pRibPlate->SetDesignItemValue('R',m_xRibPlate.tube.fBottomOutterR,"",PARAM_SHAPE);	//�߰�ײ��⻷�뾶
		else 
			pRibPlate->SetDesignItemValue('R',-1,"",PARAM_SHAPE);	//�߰�ײ��⻷�뾶,С��0��ʾ��ʹ���⻷�뾶����ײ���ȡ�
		if(m_pRelaPart2.IsHasPtr()&&m_pRelaPart2->IsPlate())
		{
			if(m_xRibPlate.bAutoExtendOutterEdge)
				pRibPlate->SetDesignItemValue('r',m_xRibPlate.tube.fTopOutterR,"",PARAM_SHAPE);	//�߰嶥���⻷�뾶
			else 
				pRibPlate->SetDesignItemValue('r',-1,"",PARAM_SHAPE);	//�߰嶥���⻷�뾶,С��0��ʾ��ʹ���⻷�뾶���㶥����ȡ�
		}
		if(m_pRelaPart1.IsNULL()&&m_pRelaPart2.IsNULL())
		{
			pRibPlate->SetDesignItemValue(KEY2C("DM"),m_xRibPlate.tube.fOffsetDistM,"",PARAM_POS,0);	//�߰�Ĭ��λ���ظֹ�����ƫ����M
			if(m_xRibPlate.tube.iWorkNormStyle!=0)
				pRibPlate->SetDesignItemValue(KEY2C("DX"),m_xRibPlate.tube.fOffsetDistX,"",PARAM_POS,0);	//�߰�ԭ�������Ĭ��λ���ظְ�X����ƫ����
			pRibPlate->SetDesignItemValue(KEY2C("Lt"),m_xRibPlate.tube.cLeftTopCornerType,"",PARAM_SHAPE,1);	//�߰�X-���򶥲���������
			pRibPlate->SetDesignItemValue(KEY2C("Lw"),m_xRibPlate.tube.fLeftTopCornerWidth,"",PARAM_SHAPE);		//�߰�X-���򶥲����ǰ뾶
			pRibPlate->SetDesignItemValue(KEY2C("Lh"),m_xRibPlate.tube.fLeftTopCornerHeight,"",PARAM_SHAPE);	//�߰�X-���򶥲����ǰ뾶
			pRibPlate->SetDesignItemValue(KEY2C("Rt"),m_xRibPlate.tube.cRightTopCornerType,"",PARAM_SHAPE,1);	//�߰�X+���򶥲���������
			pRibPlate->SetDesignItemValue(KEY2C("Rw"),m_xRibPlate.tube.fRightTopCornerWidth,"",PARAM_SHAPE);	//�߰�X+���򶥲����ǰ뾶
			pRibPlate->SetDesignItemValue(KEY2C("Rh"),m_xRibPlate.tube.fRightTopCornerHeight,"",PARAM_SHAPE);	//�߰�X+���򶥲����ǰ뾶
		}
	}
	else if(m_pDatumPart->IsPlate())
	{
		if((m_xRibPlate.m_cPlateType==0||m_xRibPlate.m_cPlateType==1)&&m_xRibPlate.plate.iAxisXType<5)
			m_xRibPlate.plate.bSpecAxisX=TRUE;
		if(m_xRibPlate.plate.bSpecOrg)
		{
			pRibPlate->SetDesignItemValue(KEY2C("Ox"),m_xRibPlate.plate.fOriginX,"",PARAM_POS,0);	//�߰�����ϵԭ��X�������(��׼�ְ��µ��������)
			pRibPlate->SetDesignItemValue(KEY2C("Oy"),m_xRibPlate.plate.fOriginY,"",PARAM_POS,0);	//�߰�����ϵԭ��Y�������(��׼�ְ��µ��������)
		}
		if(m_xRibPlate.plate.bSpecAxisX)
		{
			pRibPlate->SetDesignItemValue(KEY2C("Xx"),m_xRibPlate.plate.fXAxisX,"",PARAM_POS,0);	//�߰�����ϵX��X�������(��׼�ְ��µ��������)
			pRibPlate->SetDesignItemValue(KEY2C("Xy"),m_xRibPlate.plate.fXAxisY,"",PARAM_POS,0);	//�߰�����ϵX��Y�������(��׼�ְ��µ��������)
		}
		if(m_xRibPlate.plate.bSpecAxisY)
			pRibPlate->SetDesignItemValue(KEY2C("Ga"),m_xRibPlate.plate.fGradientDegAngle,"",PARAM_POS,0);	//�߰���б�Ƕ�(�Ƕ���)
		pRibPlate->SetDesignItemValue('Z',m_xRibPlate.plate.fOffsetDistZ,"",PARAM_POS,0);		//�߰�ԭ������ڻ�׼��Z�᷽���ƫ����
		if(m_pRelaPart1.IsNULL())
			pRibPlate->SetDesignItemValue('X',m_xRibPlate.plate.fOffsetDistX,"",PARAM_POS,0);	//�߰�ԭ������ڻ�׼��X�᷽���ƫ����
	}
	if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->IsPlate()&&m_pRelaPart2.IsNULL())
	{
		pRibPlate->SetDesignItemValue('S',m_xRibPlate.tube.bVerticalPlate,"",PARAM_SHAPE,1);	//�߰崹ֱ�븨���ְ����Զ���λ��
		pRibPlate->SetDesignItemValue(KEY2C("MF"),m_xRibPlate.MF,"",PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_DOUBLE);	//�׾�F
		pRibPlate->SetDesignItemValue(KEY2C("MG"),m_xRibPlate.MG,"",PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_DOUBLE);	//�׾�G
		pRibPlate->SetDesignItemValue(KEY2C("MH"),m_xRibPlate.MH,"",PARAM_SHAPE,DESIGN_PARAM_ITEM::TYPE_DOUBLE);	//
	}
	pRibPlate->SetDesignItemValue('E',m_xRibPlate.fBottomWidth,"",PARAM_SHAPE);	//�߰�ײ����
	pRibPlate->SetDesignItemValue('F',m_xRibPlate.fTopWidth,"",PARAM_SHAPE);	//�߰嶥�����
	pRibPlate->SetDesignItemValue('G',m_xRibPlate.fLength,"",PARAM_SHAPE);		//�߰��ڲ೤��
	pRibPlate->SetDesignItemValue('H',m_xRibPlate.fHeight,"",PARAM_SHAPE);		//�߰���೤��
	if(m_xRibPlate.m_cPlateType<=1)
		pRibPlate->SetDesignItemValue('e',m_xRibPlate.iCutAngleMode,"",PARAM_SHAPE,1);	//�߰嵹��ģʽ
	pRibPlate->SetDesignItemValue('I',m_xRibPlate.fCutAngleWidth,"",PARAM_SHAPE);		//�߰��нǿ��
	if(m_xRibPlate.m_cPlateType==1)
		pRibPlate->SetDesignItemValue(KEY2C("I2"),m_xRibPlate.fCutAngleWidth2,"",PARAM_SHAPE);	//�߰��нǿ��
	//
	pRibPlate->DesignPlate();
}

CLDSParamPlate* CDesignRibbedPlateDlg::CreateRibPlate()
{
	CLDSParamPlate *pRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	FillParamToPlate(pRibPlate);
	pRibPlate->DesignPlate();
	pRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pRibPlate->GetSolidPartObject());
	g_pSolidDraw->AddCS(pRibPlate->handle,pRibPlate->ucs);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pRibPlate->handle);
	g_pSolidDraw->Draw();
	return pRibPlate;
}

void CDesignRibbedPlateDlg::OnConfirm() 
{
	if(m_pRibPlate==NULL||!m_bNewRibPlate)
	{
		CDialog::OnOK();
		if(m_pRibPlate)
		{	//�鿴���߰�����رնԻ���֮������ѡ���߰�
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(m_pRibPlate->handle);
		}
		return;
	}
	DelWorkPlaneSketch();
	Ta.BeginUndoListen();
	int nDatumFlag=-1,nRelaFlag=-1;
	if(m_xRibPlate.bOnRelaPlateTopFace)
		nRelaFlag=1;
	if(m_xRibPlate.plate.bOnDatumPlateTopFace)
		nDatumFlag=1;
	if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE&&m_xRibPlate.tube.nPlateNum>1)
	{	//���ɶ��ֹ��߰�
		for(int i=1;i<m_xRibPlate.tube.nPlateNum;i++)
		{
			CLDSParamPlate *pTempRibPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pTempRibPlate->CopyProperty(m_pRibPlate);
			pTempRibPlate->m_hPartWeldParent=m_pDatumPart->handle;	//���Ӹ�����
			double fOffsetAngle=m_xRibPlate.tube.fInitOffsetAngle+m_xRibPlate.tube.fOffsetAngle*i;
#ifdef AFX_TARG_ENU_ENGLISH
			pTempRibPlate->SetDesignItemValue('K',fOffsetAngle,"offset angle between rib plate and datum edge",PARAM_POS,0);
#else
			pTempRibPlate->SetDesignItemValue('K',fOffsetAngle,"�߰����׼��֮���ƫ�ƽǶ�",PARAM_POS,0);
#endif
			pTempRibPlate->DesignPlate();
			pTempRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pTempRibPlate->GetSolidPartObject());
			ribPlateSet.append(pTempRibPlate);	//��¼ͨ����ת��Գ����ɵ��߰� wht 11-06-07
		}
	}
	else if(m_xRibPlate.bCreateOuterPlate)
	{	//��������߰�
		if(m_pRelaPart1.IsNULL()&&m_pRelaPart2.IsNULL())
		{	//�޸����ְ壬���ɻ�׼�ְ�����һ����߰�
			CLDSParamPlate *pRibPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pRibPlate1->CopyProperty(m_pRibPlate);
			pRibPlate1->m_hPartWeldParent=m_pDatumPart->handle;	//���Ӹ�����
#ifdef AFX_TARG_ENU_ENGLISH
			pRibPlate1->SetDesignItemValue('A',-nDatumFlag*m_pDatumPart->handle,"datum part",PARAM_POS,1);
			pRibPlate1->SetDesignItemValue('G',m_xRibPlate.fOuterPlateLen,"The inside length of rib plate",PARAM_SHAPE);
			pRibPlate1->SetDesignItemValue('H',m_xRibPlate.fOuterPlateHeight,"The outside length of rib plate",PARAM_SHAPE);
			pRibPlate1->SetDesignItemValue('Z',-m_xRibPlate.plate.fOffsetDistZ,"offset value between rib plate's origin and the datum point in Z axis direction",PARAM_POS,0);
#else
			pRibPlate1->SetDesignItemValue('A',-nDatumFlag*m_pDatumPart->handle,"��׼����",PARAM_POS,1);
			pRibPlate1->SetDesignItemValue('G',m_xRibPlate.fOuterPlateLen,"�߰��ڲ೤��",PARAM_SHAPE);
			pRibPlate1->SetDesignItemValue('H',m_xRibPlate.fOuterPlateHeight,"�߰���೤��",PARAM_SHAPE);
			pRibPlate1->SetDesignItemValue('Z',-m_xRibPlate.plate.fOffsetDistZ,"�߰�ԭ������ڻ�׼��Z�᷽���ƫ����",PARAM_POS,0);
#endif
			pRibPlate1->DesignPlate();
			pRibPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pRibPlate1->GetSolidPartObject());
		}
		else if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart2.IsNULL()&&m_pRelaPart1->IsPlate())
		{	//���������߰�
			CLDSParamPlate *pRibPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pRibPlate1->CopyProperty(m_pRibPlate);
#ifdef AFX_TARG_ENU_ENGLISH
			pRibPlate1->SetDesignItemValue('C',-1*nRelaFlag*m_pRelaPart1->handle,"rib plate positioning auxiliary part 1",PARAM_POS,1);
			pRibPlate1->SetDesignItemValue('G',m_xRibPlate.fOuterPlateLen,"The inside floor length of rib plate",PARAM_SHAPE);
			pRibPlate1->SetDesignItemValue('H',m_xRibPlate.fOuterPlateHeight,"The outside length of rib plate",PARAM_SHAPE);
			if(m_pDatumPart->IsPlate())
				pRibPlate1->SetDesignItemValue('Z',-m_xRibPlate.plate.fOffsetDistZ,"offset value between rib plate's origin and the datum point in Z axis direction",PARAM_POS,0);
#else
			pRibPlate1->SetDesignItemValue('C',-1*nRelaFlag*m_pRelaPart1->handle,"�߰嶨λ��������1",PARAM_POS,1);
			pRibPlate1->SetDesignItemValue('G',m_xRibPlate.fOuterPlateLen,"�߰��ڲ೤��",PARAM_SHAPE);
			pRibPlate1->SetDesignItemValue('H',m_xRibPlate.fOuterPlateHeight,"�߰���೤��",PARAM_SHAPE);
			if(m_pDatumPart->IsPlate())
				pRibPlate1->SetDesignItemValue('Z',-m_xRibPlate.plate.fOffsetDistZ,"�߰�ԭ������ڻ�׼��Z�᷽���ƫ����",PARAM_POS,0);
#endif
			pRibPlate1->DesignPlate();
			pRibPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pRibPlate1->GetSolidPartObject());
		}
		else if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->IsPlate()
				&&m_pRelaPart2.IsHasPtr()&&m_pRelaPart2->IsPlate())
		{	//���߰壬���ɸ����ְ���������߰�
			f3dPoint verfiy_vec=m_pRelaPart2->ucs.origin-m_pRelaPart1->ucs.origin;
			normalize(verfiy_vec);
			int nFlag=1;
			if(verfiy_vec*m_pRelaPart1->ucs.axis_z>0)
				nFlag=-1;
			CLDSParamPlate *pRibPlate1=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pRibPlate1->CopyProperty(m_pRibPlate);
			pRibPlate1->DeleteDesignItem('D');	//ɾ���ڶ�����������
#ifdef AFX_TARG_ENU_ENGLISH
			pRibPlate1->SetDesignItemValue('C',nFlag*m_pRelaPart1->handle,"rib plate positioning auxiliary part 1",PARAM_POS,1);
			pRibPlate1->SetDesignItemValue('G',m_xRibPlate.fOuterPlateLen,"The inside length of rib plate",PARAM_SHAPE);
			pRibPlate1->SetDesignItemValue('H',m_xRibPlate.fOuterPlateHeight,"The outside length of rib plate",PARAM_SHAPE);
			if(m_pDatumPart->IsPlate())
				pRibPlate1->SetDesignItemValue('Z',-m_xRibPlate.plate.fOffsetDistZ,"offset value between rib plate's origin and the datum point in Z axis direction",PARAM_POS,0);
#else
			pRibPlate1->SetDesignItemValue('C',nFlag*m_pRelaPart1->handle,"�߰嶨λ��������1",PARAM_POS,1);
			pRibPlate1->SetDesignItemValue('G',m_xRibPlate.fOuterPlateLen,"�߰��ڲ೤��",PARAM_SHAPE);
			pRibPlate1->SetDesignItemValue('H',m_xRibPlate.fOuterPlateHeight,"�߰���೤��",PARAM_SHAPE);
			if(m_pDatumPart->IsPlate())
				pRibPlate1->SetDesignItemValue('Z',-m_xRibPlate.plate.fOffsetDistZ,"�߰�ԭ������ڻ�׼��Z�᷽���ƫ����",PARAM_POS,0);
#endif
			pRibPlate1->DesignPlate();
			pRibPlate1->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pRibPlate1->GetSolidPartObject());
			//
			if(verfiy_vec*m_pRelaPart2->ucs.axis_z<0)
				nFlag=-1;
			else 
				nFlag=1;
			CLDSParamPlate *pRibPlate2=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pRibPlate2->CopyProperty(m_pRibPlate);
			pRibPlate2->DeleteDesignItem('D');	//ɾ���ڶ�����������
#ifdef AFX_TARG_ENU_ENGLISH
			pRibPlate2->SetDesignItemValue('C',nFlag*m_pRelaPart2->handle,"rib plate positioning auxiliary part 1",PARAM_POS,1);
			pRibPlate2->SetDesignItemValue('G',m_xRibPlate.fOuterPlateLen,"The inside length of rib plate",PARAM_SHAPE);
			pRibPlate2->SetDesignItemValue('H',m_xRibPlate.fOuterPlateHeight,"The outside length of rib plate",PARAM_SHAPE);
			if(m_pDatumPart->IsPlate())
				pRibPlate2->SetDesignItemValue('Z',-m_xRibPlate.plate.fOffsetDistZ,"offset value between rib plate's origin and the datum point in Z axis direction",PARAM_POS,0);
#else
			pRibPlate2->SetDesignItemValue('C',nFlag*m_pRelaPart2->handle,"�߰嶨λ��������1",PARAM_POS,1);
			pRibPlate2->SetDesignItemValue('G',m_xRibPlate.fOuterPlateLen,"�߰��ڲ೤��",PARAM_SHAPE);
			pRibPlate2->SetDesignItemValue('H',m_xRibPlate.fOuterPlateHeight,"�߰���೤��",PARAM_SHAPE);
			if(m_pDatumPart->IsPlate())
				pRibPlate2->SetDesignItemValue('Z',-m_xRibPlate.plate.fOffsetDistZ,"�߰�ԭ������ڻ�׼��Z�᷽���ƫ����",PARAM_POS,0);
#endif
			pRibPlate2->DesignPlate();
			pRibPlate2->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pRibPlate2->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();
	Ta.EndUndoListen();
	CDialog::OnOK();
}

void CDesignRibbedPlateDlg::OnPartsDb() 
{
	CPartsDbDlg partsDlg;
	partsDlg.m_bRefreshDlgCtrl = TRUE;
	partsDlg.m_bIncParamRibPlate=TRUE;
	if(partsDlg.DoModal()==IDOK)
	{
		CLDSParamPlate *pSrcParamPlate=(CLDSParamPlate*)partsDlg.m_pSelectPart;
		if(!pSrcParamPlate)
			return;
		if(pSrcParamPlate->m_iParamType!=TYPE_RIBPLATE)
			return;
		//TODO����Ҫ���¸�����elbow.LoadParams��������������� wjh-2018.3.12
		//CLDSParamPlate::RIBPLATE_ELBOW elbow;
		//elbow.LoadParams(pAttachPlate);//��Ҫ������elbow.LoadParams��������������� wjh-2018.3.12
		//AttachPlatePara.A=elbow.A;
		//AttachPlatePara.B=elbow.B;
		//AttachPlatePara.C=elbow.C;
		//AttachPlatePara.D=elbow.D;
		//AttachPlatePara.E=elbow.E;
		//AttachPlatePara.F=elbow.F;
		//AttachPlatePara.G=elbow.G;
		//AttachPlatePara.H=elbow.H;
		pSrcParamPlate->GetDesignItemValue('E',&m_xRibPlate.fBottomWidth);
		pSrcParamPlate->GetDesignItemValue('F',&m_xRibPlate.fTopWidth);
		pSrcParamPlate->GetDesignItemValue('G',&m_xRibPlate.fLength);
		pSrcParamPlate->GetDesignItemValue('H',&m_xRibPlate.fHeight);
		pSrcParamPlate->GetDesignItemValue('I',&m_xRibPlate.fCutAngleWidth);
		pSrcParamPlate->GetDesignItemValue('X',&m_xRibPlate.plate.fOffsetDistX);
		pSrcParamPlate->GetDesignItemValue('Z',&m_xRibPlate.plate.fOffsetDistZ);
		m_xRibPlate.plate.bSpecOrg=TRUE;
		m_xRibPlate.plate.bSpecOrg=m_xRibPlate.plate.bSpecOrg&&pSrcParamPlate->GetDesignItemValue(KEY2C("Ox"),&m_xRibPlate.plate.fOriginX);
		m_xRibPlate.plate.bSpecOrg=m_xRibPlate.plate.bSpecOrg&&pSrcParamPlate->GetDesignItemValue(KEY2C("Oy"),&m_xRibPlate.plate.fOriginY);
		m_xRibPlate.plate.bSpecAxisX=TRUE;
		m_xRibPlate.plate.bSpecAxisX=m_xRibPlate.plate.bSpecAxisX&&pSrcParamPlate->GetDesignItemValue(KEY2C("Xx"),&m_xRibPlate.plate.fXAxisX);
		m_xRibPlate.plate.bSpecAxisX=m_xRibPlate.plate.bSpecAxisX&&pSrcParamPlate->GetDesignItemValue(KEY2C("Yy"),&m_xRibPlate.plate.fXAxisY);
		m_xRibPlate.plate.bSpecAxisY=TRUE;
		m_xRibPlate.plate.bSpecAxisY=m_xRibPlate.plate.bSpecAxisY&&pSrcParamPlate->GetDesignItemValue(KEY2C("Ga"),&m_xRibPlate.plate.fGradientDegAngle);
		pSrcParamPlate->GetDesignItemValue(KEY2C("DM"),&m_xRibPlate.tube.fOffsetDistM);
		pSrcParamPlate->GetDesignItemValue(KEY2C("DX"),&m_xRibPlate.tube.fOffsetDistX);
		pSrcParamPlate->GetDesignItemValue(KEY2C("MF"),&m_xRibPlate.MF);
		pSrcParamPlate->GetDesignItemValue(KEY2C("MG"),&m_xRibPlate.MG);
		pSrcParamPlate->GetDesignItemValue(KEY2C("MH"),&m_xRibPlate.MH);
		long J=0;
		pSrcParamPlate->GetDesignItemValue('J',&J);
		m_xRibPlate.tube.iDatumEdge=(int)J;
		pSrcParamPlate->GetDesignItemValue('K',&m_xRibPlate.tube.fInitOffsetAngle);
		pSrcParamPlate->GetDesignItemValue('R',&m_xRibPlate.tube.fBottomOutterR);
		pSrcParamPlate->GetDesignItemValue('r',&m_xRibPlate.tube.fTopOutterR);
		m_xRibPlate.nSegI=pSrcParamPlate->iSeg;
		strcpy(m_xRibPlate.sPartNo,pSrcParamPlate->GetPartNo());
		m_xRibPlate.cMaterial=pSrcParamPlate->cMaterial;
		m_xRibPlate.nThick=pSrcParamPlate->thick;
		//
		CXhChar100 temp_str;
		long idProp=GetPropID("RibPlate.fBottomWidth");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.fTopWidth");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.fLength");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.fHeight");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.fCirR");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.iCutAngleMode");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.fCutAngleWidth");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.MF");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.MG");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.MH");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.plate.fOffsetDistX");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.plate.fOffsetDistZ");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.tube.iDatumEdge");		//0.��׼�� 1.������ 2.ָ������
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.tube.iWorkNormStyle");
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.tube.fInitOffsetAngle");	//��ʼƫת�Ƕ�
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.tube.fBottomOutterR");	//�ײ��⻷�뾶
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.tube.fTopOutterR");		//�����⻷�뾶
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.tube.fOffsetDistM");		//����ƫ�ƾ���M
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		idProp=GetPropID("RibPlate.tube.fOffsetDistX");		//Y����ƫ�ƾ���
		if(GetPropValueStr(idProp,temp_str)>0)
			m_propList.SetItemPropValue(idProp,temp_str);
		//
		if(m_pRibPlate)
		{
			m_pRibPlate->iSeg=m_xRibPlate.nSegI;
			m_pRibPlate->SetPartNo(m_xRibPlate.sPartNo);
			m_pRibPlate->cMaterial=m_xRibPlate.cMaterial;
			m_pRibPlate->thick=m_xRibPlate.nThick;
			ModifyPlateParam('E',m_xRibPlate.fBottomWidth);
			ModifyPlateParam('F',m_xRibPlate.fTopWidth);
			ModifyPlateParam('G',m_xRibPlate.fLength);
			ModifyPlateParam('H',m_xRibPlate.fHeight);
			ModifyPlateParam('I',m_xRibPlate.fCutAngleWidth);
			ModifyPlateParam('X',m_xRibPlate.plate.fOffsetDistX);
			ModifyPlateParam('Z',m_xRibPlate.plate.fOffsetDistZ);
			ModifyPlateParam(KEY2C("DM"),m_xRibPlate.tube.fOffsetDistM);
			ModifyPlateParam(KEY2C("DX"),m_xRibPlate.tube.fOffsetDistX);
			ModifyPlateParam('R',m_xRibPlate.tube.fBottomOutterR);
			ModifyPlateParam('r',m_xRibPlate.tube.fTopOutterR);
			ModifyPlateParam('K',m_xRibPlate.tube.fInitOffsetAngle);
			ModifyPlateParam('J',(long)m_xRibPlate.tube.iDatumEdge);
			if(m_xRibPlate.plate.bSpecOrg)
			{
				ModifyPlateParam(KEY2C("Ox"),m_xRibPlate.plate.fOriginX);
				ModifyPlateParam(KEY2C("Oy"),m_xRibPlate.plate.fOriginY);
			}
			if(m_xRibPlate.plate.bSpecAxisX)
			{
				ModifyPlateParam(KEY2C("Xx"),m_xRibPlate.plate.fXAxisX);
				ModifyPlateParam(KEY2C("Yy"),m_xRibPlate.plate.fXAxisY);
			}
			if(m_xRibPlate.plate.bSpecAxisY)
				ModifyPlateParam(KEY2C("Ga"),m_xRibPlate.plate.fGradientDegAngle);
		}
		ReDesignPlate();
	}
}

void CDesignRibbedPlateDlg::OnClose() 
{
	DelWorkPlaneSketch();
	if(m_bNewRibPlate&&m_pRibPlate)
	{
		g_pSolidDraw->DelCS(m_pRibPlate->handle);
		g_pSolidDraw->DelEnt(m_pRibPlate->handle);
		console.DispPartSet.DeleteNode(m_pRibPlate->handle);
		console.DeletePart(m_pRibPlate->handle);
		g_pSolidDraw->Draw();
	}
	else if(!m_bNewRibPlate&&m_pRibPlate)
	{	//�鿴���߰�����رնԻ���֮������ѡ���߰�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pRibPlate->handle);
	}
	CDialog::OnClose();
}
void CDesignRibbedPlateDlg::OnCancel() 
{
	DelWorkPlaneSketch();
	if(m_bNewRibPlate&&m_pRibPlate)
	{
		g_pSolidDraw->DelCS(m_pRibPlate->handle);
		g_pSolidDraw->DelEnt(m_pRibPlate->handle);
		console.DispPartSet.DeleteNode(m_pRibPlate->handle);
		console.DeletePart(m_pRibPlate->handle);
		g_pSolidDraw->Draw();
	}
	else if(!m_bNewRibPlate&&m_pRibPlate)
	{	//�鿴���߰�����رնԻ���֮������ѡ���߰�
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(m_pRibPlate->handle);
	}
	CDialog::OnCancel();
}
void CDesignRibbedPlateDlg::WorkPlaneSketch(int iType)
{
	DelWorkPlaneSketch();
	if(m_pDatumPart==NULL||m_pRibPlate==NULL)
		return;
	if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube* pDatumTube=m_pDatumPart.LineTubePointer();
		int nLen=(WORD)pDatumTube->GetDiameter()*2;
		GEPOINT base_pos,work_norm;
		base_pos=m_pRibPlate->ucs.origin;
		base_pos+=m_pRibPlate->ucs.axis_y*m_xRibPlate.fLength*0.5;
		g_pSolidDraw->DelCS(m_pRibPlate->handle);
		if(iType==TYPE_PLANE_SPREAD)
		{
			if(m_pDatumNode)
				base_pos=m_pDatumNode->Position();
			//��׼��չ������
			work_norm=pDatumTube->WorkPlaneNorm();
			m_xSpreadVec1=pDatumTube->ucs.axis_x;
			btc::SKETCH_PLANE sketch1(base_pos,work_norm,m_xSpreadVec1,nLen);
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),sketch1.pVertexArr,sketch1.VertexCount,m_xSpreadVec1,"��׼��",200);
			//������չ������
			double rotAngle=pDatumTube->CalWeldRoadAngle();
			m_xSpreadVec2.Set(cos(rotAngle),sin(rotAngle));
			vector_trans(m_xSpreadVec2,pDatumTube->ucs,TRUE);
			work_norm=pDatumTube->ucs.axis_z^m_xSpreadVec2;
			normalize(work_norm);
			btc::SKETCH_PLANE sketch2(base_pos,work_norm,m_xSpreadVec2,nLen);
			g_pSolidDraw->NewWorkPlaneSketch(2,RGB(152,152,152),sketch2.pVertexArr,sketch2.VertexCount,m_xSpreadVec2,"������",200);
		}
		else if(iType==TYPE_PLANE_NORMAL)
		{
			//�ֹ�����
			m_xSpreadVec1=pDatumTube->ucs.axis_z;
			btc::SKETCH_PLANE sketch1(base_pos,m_pRibPlate->ucs.axis_z,m_xSpreadVec1);
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),sketch1.pVertexArr,sketch1.VertexCount,m_xSpreadVec1,"�ֹ�����",200);
			//�ֹܽ���
			m_xSpreadVec2=m_pRibPlate->ucs.axis_z;
			btc::SKETCH_PLANE sketch2(base_pos,pDatumTube->ucs.axis_z,m_xSpreadVec1);
			g_pSolidDraw->NewWorkPlaneSketch(2,RGB(152,152,152),sketch2.pVertexArr,sketch2.VertexCount,m_xSpreadVec2,"�ֹܽ���",200);
		}
		else if(iType==TYPE_TUBE_SPREAD)
		{
			m_xSpreadVec1=pDatumTube->ucs.axis_y;
			btc::SKETCH_PLANE sketch(base_pos,pDatumTube->ucs.axis_z,m_xSpreadVec1);
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"�ֹ�����",200);
		}
		else if(iType==TYPE_X_AXIS_VECTOR)
		{
			base_pos+=m_pRibPlate->ucs.axis_x*m_xRibPlate.fBottomWidth*(-0.5);
			m_xSpreadVec1=m_pRibPlate->ucs.axis_x;
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),&base_pos,1,m_xSpreadVec1,"X+");
			//
			m_xSpreadVec2=-1*m_pRibPlate->ucs.axis_x;
			g_pSolidDraw->NewWorkPlaneSketch(2,RGB(152,152,152),&base_pos,1,m_xSpreadVec2,"X-");
		}
		else if(iType==TYPE_Y_AXIS_VECTOR)
		{
			base_pos+=m_pRibPlate->ucs.axis_x*m_xRibPlate.fBottomWidth*(-0.5);
			m_xSpreadVec1=m_pRibPlate->ucs.axis_y;
			g_pSolidDraw->NewWorkPlaneSketch(1,RGB(152,152,152),&base_pos,1,m_xSpreadVec1,"Y");
		}
	}
	g_pSolidDraw->Draw();
}
void CDesignRibbedPlateDlg::DelWorkPlaneSketch()
{
	if(!m_xSpreadVec1.IsZero())
		g_pSolidDraw->DelWorkPlaneSketch(1);
	if(!m_xSpreadVec2.IsZero())
		g_pSolidDraw->DelWorkPlaneSketch(2);
	g_pSolidDraw->Draw();
}
void CDesignRibbedPlateDlg::ModifyPlateParam(WORD wKey,double fValue,char *sNote/*=NULL*/)
{
	if(m_pRibPlate==NULL)
		return;
	DESIGN_PLATE_ITEM *pItem=m_pRibPlate->GetDesignItem(wKey);
	if(pItem)
		pItem->value.fValue=fValue;
	else
		m_pRibPlate->SetDesignItemValue(wKey,fValue,sNote,PARAM_SHAPE,0);
}
void CDesignRibbedPlateDlg::ModifyPlateParam(WORD wKey,long nValue,char *sNote/*=NULL*/)
{
	if(m_pRibPlate==NULL)
		return;
	DESIGN_PLATE_ITEM *pItem=m_pRibPlate->GetDesignItem(wKey);
	if(pItem)
		pItem->value.hPart=nValue;
	else
		m_pRibPlate->SetDesignItemValue(wKey,nValue,sNote,PARAM_SHAPE,1);
}
void CDesignRibbedPlateDlg::DeletePlateParam(WORD wKey)
{
	if(m_pRibPlate==NULL)
		return;
	m_pRibPlate->DeleteDesignItem(wKey);
}
void CDesignRibbedPlateDlg::ReDesignPlate()
{
	if(m_pRibPlate&&m_pRibPlate->DesignPlate())
	{
		m_pRibPlate->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(m_pRibPlate->GetSolidPartObject());
		g_pSolidDraw->AddCS(m_pRibPlate->handle,m_pRibPlate->ucs);
		g_pSolidDraw->Draw();
	}
}

void CDesignRibbedPlateDlg::SetRibPlate(CLDSParamPlate *pRibPlate)
{
	m_pRibPlate=pRibPlate;
	m_bNewRibPlate=FALSE;	//�鿴�߰�����
}
//���ѡ�����ĺ�������
void CDesignRibbedPlateDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	long hPart=0;
	if(GetEventPropId()==GetPropID("m_pDatumPart"))
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart&&(pPart->IsPlate()||pPart->IsAngle()||pPart->GetClassTypeId()==CLS_LINETUBE))
		{
			m_pDatumPart=pPart;
			hPart=pPart->handle;
			if(m_pRelaPart1.IsNULL()&&m_pRelaPart2.IsNULL())
			{
				if(m_pDatumPart->GetClassTypeId()==CLS_LINETUBE&&!m_xRibPlate.bOnRelaPlateTopFace)
					hPart*=-1;
			}
			else if(!m_xRibPlate.plate.bOnDatumPlateTopFace)
				hPart*=-1;
			ModifyPlateParam('A',hPart);
		}
	}
	else if(GetEventPropId()==GetPropID("m_pRelaPart1"))
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart&&pPart->IsPlate())
		{
			m_pRelaPart1=pPart;
			hPart=pPart->handle;
			if(!m_xRibPlate.bOnRelaPlateTopFace)
				hPart*=-1;
			ModifyPlateParam('C',hPart);
		}
	}
	else if(GetEventPropId()==GetPropID("m_pRelaPart2"))
	{
		CLDSPart *pPart=console.FromPartHandle(m_hPickObj);
		if(pPart&&pPart->IsPlate())
		{
			m_pRelaPart2=pPart;
			hPart=pPart->handle;
			ModifyPlateParam('D',hPart);
		}
	}
	else if(GetEventPropId()==GetPropID("m_pVecPart"))
	{
		CLDSPart *pVecPart=console.FromPartHandle(m_hPickObj,CLS_LINEPART,CLS_BOLT);
		if(pVecPart)
		{
			m_pVecPart=pVecPart;
			hPart=pVecPart->handle;
			if(!m_xRibPlate.plate.bAlongVecPart)
				hPart*=-1;
			ModifyPlateParam('V',hPart);
			DeletePlateParam(KEY2C("Ga"));
		}
	}
	else if(GetEventPropId()==GetPropID("m_pDatumBolt"))
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(m_hPickObj,CLS_BOLT);
		if(pBolt)
		{	//��˨�����������˨���ж�λ,ɾ���ֶ��������
			m_pDatumBolt=pBolt;
			hPart=pBolt->handle;
			ModifyPlateParam('O',hPart);
			DeletePlateParam(KEY2C("Ox"));
			DeletePlateParam(KEY2C("Oy"));
		}
	}
	else if(GetEventPropId()==GetPropID("m_pDatumNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			m_pDatumNode=pNode;
			hPart=pNode->handle;
			ModifyPlateParam('B',hPart);
		}
	}
	else if(GetEventPropId()==GetPropID("RibPlate.plate.BendPlaneTipBolt"))
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(m_hPickObj,CLS_BOLT);
		if(pBolt)
		{
			m_xRibPlate.plate.anchors[2].ciType=2;
			m_xRibPlate.plate.anchors[2].locate.hBolt=m_hPickObj;
			ModifyPlateParam(KEY2C("TB"),m_hPickObj);
		}
	}
	else if(GetEventPropId()==GetPropID("RibPlate.plate.BendDirectVecPart"))
	{
		CLDSPart *pPart=(CLDSPart*)console.FromPartHandle(m_hPickObj);
		if(pPart&&(pPart->GetClassTypeId()==CLS_BOLT||pPart->IsLinePart()))
		{
			if(pPart->IsLinePart())
			{
				m_xRibPlate.plate.anchors[3].ciType=1;
				m_xRibPlate.plate.anchors[3].locate.hVecRod=m_hPickObj;
			}
			else
			{
				m_xRibPlate.plate.anchors[3].ciType=2;
				m_xRibPlate.plate.anchors[3].locate.hBolt=m_hPickObj;
			}
			ModifyPlateParam(KEY2C("BV"),m_hPickObj);
		}
	}
	ReCalRibPlateUCS();	//���¼����߰�����ϵ
	ReDesignPlate();	//��������߰�
}
void CDesignRibbedPlateDlg::UpdateSketchMap()
{
	m_xCurPlateBmp.Detach();
	// 1.���߰�: D.�ְ�+�����ְ�+��˨��λ
	//			E.�ֹ�+�����ְ�(��:�ֹ����)
	//			F.�ֹ�+�ֹ�(��:�ֹܼа�)	
	if(0==m_xRibPlate.m_cPlateType)
	{	
		if(m_pDatumPart.IsHasPtr()&&m_pDatumPart->IsTube())
		{
			if(m_pRelaPart1.IsHasPtr()&&m_pRelaPart1->IsTube())
				m_xCurPlateBmp.LoadBitmap(IDB_BMP_TUBE_BETWEEN_PLATE);	//�ֹܼа�
			else 
				m_xCurPlateBmp.LoadBitmap(IDB_BMP_CORNER_RIB_PLATE);	//�ֹ����
		}
		else //if(m_pDatumPart.IsHasPtr()&&m_pDatumPart->IsPlate())
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_CORNER_RIB_PLATE2);
	}
	// 1.���߰�: G.�ְ�+2�������ְ�+��˨��λ
	//			H.�ֹ�+2�������ְ�
	else if(1==m_xRibPlate.m_cPlateType)
	{
		if(m_pDatumPart.IsHasPtr()&&m_pDatumPart->IsTube())
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_SLOT_RIB_PLATE);
		else
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_SLOT_RIB_PLATE2);
	}
	//2.�Ǹ��߰�
	else if(2==m_xRibPlate.m_cPlateType)
	{
		m_xCurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE);		//�Ǹ��ڲ��߰�
		//m_xCurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE1);	//�Ǹ��ڲ��Բ�����߰�
		//m_xCurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE2);	//�Ǹ�����߰�
	}
	//3.�ְ��߰�
	else if(3==m_xRibPlate.m_cPlateType)
	{
		if(m_xRibPlate.plate.anchors[2].ciType!=0)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_PLATE_RIB_BEND_PLATE);
		else
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_PLATE_RIB_PLATE);
	}
	//4.�ֹ��߰�
	else if(4==m_xRibPlate.m_cPlateType)
	{
		if(m_xRibPlate.tube.iWorkNormStyle!=0)
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_TUBE_RIB_PLATE2);
		else
			m_xCurPlateBmp.LoadBitmap(IDB_BMP_TUBE_RIB_PLATE1);
	}
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	pPic->SetBitmap(m_xCurPlateBmp);
}

void CDesignRibbedPlateDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
void CDesignRibbedPlateDlg::OnBnModelViewOper()
{	//��ʱ�ж�ת����ģ�Ͳ��������У��Ա����û�����άģ�����š���ת�鿴�Ȳ���
	ExecuteCommand(this,GetPropID("preview"));
}
#endif

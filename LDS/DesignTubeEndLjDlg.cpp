// DesignTubeEndLjDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "LDSDoc.h"
#include "DesignTubeEndLjDlg.h"
#include "XhCharString.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "DesignXEndRibPlateDlg.h"
#include "OddmentDesignDlg.h"
#include "LibraryQuery.h"
#include "ReportPointDlg.h"
#include "LmaDlg.h"
#include "PartsDbDlg.h"
#include "PropertyListOper.h"
#include "LayAngleBoltDlg.h"
#include "PlateBasicParaDlg.h"
#include "LocalFeatureDef.h"
#include "DataCompare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDesignTubeEndLjDlg dialog
CDesignTubeEndLjDlg::tagAttachPlatePara::tagAttachPlatePara()
{
	memset(this,0,sizeof(tagAttachPlatePara));
	cMaterial=CSteelMatLibrary::Q235BriefMark();
	if(cMaterial==0)	//ȫ�ֲ���ʱPartLib��δ��ʼ��
		cMaterial='S';
	m_nThick=8;
}
CDesignTubeEndLjDlg::tagNodePlatePara::tagNodePlatePara()
{
	memset(this,0,sizeof(tagNodePlatePara));
	cMaterial=CSteelMatLibrary::Q235BriefMark();
	if(cMaterial==0)	//ȫ�ֲ���ʱPartLib��δ��ʼ��
		cMaterial='S';
	m_nThick=8;
}
int CDesignTubeEndLjDlg::nScrLocationX=0;
int CDesignTubeEndLjDlg::nScrLocationY=0;
CDesignTubeEndLjDlg::CDesignTubeEndLjDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CDesignTubeEndLjDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDesignTubeEndLjDlg)
	m_iTubeEndLjStyle = 4;
	//}}AFX_DATA_INIT
	m_pFlDParam=NULL;			//�Ժ�����
	m_pFlDParam2=NULL;			//�ԽӵĶԺ�����
	m_pFlPParam=NULL;			//ƽ������
	m_pFlPParam2=NULL;			//�Խӵ�ƽ������
	m_pRigidFlParam=NULL;		//���Է���
	m_pRigidFlParam2=NULL;		//�Խӵĸ��Է���
	m_pFlexibleFlParam=NULL;	//���Է���
	m_pFlexibleFlParam2=NULL;	//�Խӵ����Է���
	m_pInsertPlateParam=NULL;	//��׼���
	m_arrPropGroupLabel.RemoveAll();
	m_bStartEnd=TRUE;
	m_bCoupleFLCreate=true;
	m_pBaseNode=NULL;
	m_pCurLineTube=NULL;//��ǰ�ֹ�
	m_pRelatedTube=NULL;
	m_iPartType=0;		//0.��ͨ���� 1.��׼����
	strcpy(sCoupleFlPlateNo,"");	//(��ͨ����)���Ⱦ��Խ�ʱ�Խӷ����ı��
	strcpy(sWeldPlatePartNo,"");	//ʮ�ֲ�崹����
	strcpy(sWrapPlatePartNo,"");	//ʮ�ֲ�������
	//strcpy(sCodeName,"");			//��׼��������
	m_cFlLevel1='1';
	m_cFlLevel2='1';
	m_iStdPartLevel=1;
	m_bFaceToFace = FALSE;			//�Խ����
	m_hDatumTube1=m_hDatumTube2=m_hWeldLinkObj=0;
	m_bSpecEndLjStyle=FALSE;
	m_bHasExistEndLj=FALSE;
	m_pParamPlate=m_pSrcParamPlate=NULL;
	m_bCreateAttachPlate=FALSE;
	m_bSpecialFlRayPole=FALSE;	//���η������ӵ����߸˼�
	m_iWrapPlateType=0;			//0.�ְ� 1.�Ǹ�
	m_bDesignNodePlate=FALSE;
	work_norm.Set();
	m_bCreateFillPlate=FALSE;
	setupInfo.m_fWeldHeight=20;
	setupInfo.workNorm.norm_style=5;
	setupInfo.workNorm.direction=2;	//�Ի�׼�ֹܵĻ�׼��ƽ��Ϊ��ת��׼
	setupInfo.workNorm.rot_ang=0;
	setupInfo.workNormUpVert.norm_style=4;	//ĸ��װ������ϵ�е���Է���
	setupInfo.workNormUpVert.nearVector.Set(0,-1);
	setupInfo.workNormDownVert.norm_style=4;	//ĸ��װ������ϵ�е���Է���
	setupInfo.workNormDownVert.nearVector.Set(0,1);
	m_bEnableWindowMoveListen=FALSE;
}

void CDesignTubeEndLjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignTubeEndLjDlg)
	DDX_Control(pDX, IDC_LIST_BOX_CHILD, m_xSubPropList);
	DDX_Control(pDX, IDC_BOLT_LIST, m_listBolt);
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_BOLT_PARA_LIST,m_listBoltParam);
	DDX_CBIndex(pDX, ID_TUBE_ENDLJ_STYLE, m_iTubeEndLjStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignTubeEndLjDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignTubeEndLjDlg)
	ON_CBN_SELCHANGE(ID_TUBE_ENDLJ_STYLE, OnSelchangeTubeEndljStyle)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_BN_CLICKED(ID_CONFIRM_CREATE, OnConfirmCreate)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_DEL_CUR_ENDLJ, OnDelCurEndlj)
	ON_BN_CLICKED(IDC_BTN_PARTS_DB, OnPartsDb)
	ON_NOTIFY(LVN_KEYDOWN, IDC_BOLT_LIST, OnKeydownBoltList)
	ON_NOTIFY(NM_CLICK, IDC_BOLT_LIST, OnClickBoltList)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_WM_MOVE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//��̬���ص���������
static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,NM_LISTVIEW* pNMListView)
{	//�޸���˨λ�ú��ػ�Ԥ��ͼ wht 10-09-13
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pListCtrl->GetParent();
	if(pTubeEndLjDlg==NULL || pTubeEndLjDlg->m_iTubeEndLjStyle<3)
		return FALSE;
	CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pSelLsPara=(CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA*)pItem->m_idProp;
	if(pSelLsPara==NULL)
		return FALSE;
	//ȡ������ѡ�����ѡ��״̬
	int nCount=pTubeEndLjDlg->m_listBolt.GetItemCount();
	pTubeEndLjDlg->m_listBolt.SetItemState(-1,0,LVNI_SELECTED);
	for(int i=0;i<nCount;i++)
	{	//
		DWORD dwFlag=pTubeEndLjDlg->m_listBolt.GetItemData(i);	//��˨��־λ
		int h=*((WORD*)&dwFlag);
		if(h>=100)
			h=h/100;
		if(h==pItem->GetIndex()+1)
			pTubeEndLjDlg->m_listBolt.SetItemState(i,LVNI_SELECTED|LVNI_FOCUSED,LVNI_SELECTED|LVNI_FOCUSED);
	}
	//������������ʾ
	pTubeEndLjDlg->LayoutBoltPara=*pSelLsPara;
	CPropTreeItem* pGroupItem=pTubeEndLjDlg->m_propList.FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("LayoutBoltParam"),NULL);
	if(pGroupItem)
		pTubeEndLjDlg->UpdateBoltProperty(pGroupItem);
	pTubeEndLjDlg->Redraw();
	return TRUE;
}
static BOOL FireModifyValue(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{	//�޸���˨λ�ú��ػ�Ԥ��ͼ
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pListCtrl->GetParent();
	if(pTubeEndLjDlg==NULL)
		return FALSE;
	CString sOldValue=_T("");
	if(pSelItem&&pSelItem->m_lpNodeInfo)
		sOldValue=pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	//��ǰ��Ԫ���������޸ģ����Ĺ���Ϊ�༭״̬
	if(sOldValue.CompareNoCase(sTextValue)!=0)
	{
		CXhChar100 sText;
		if(pTubeEndLjDlg->m_iTubeEndLjStyle>2)
		{	//���
			CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pLsPara=(CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA*)pSelItem->m_idProp;
			if(pLsPara==NULL)
				return FALSE;
			if(iSubItem==0)		//��˨����
				pLsPara->m_nBoltNum=atoi(sTextValue);
			else if(iSubItem==1)//����
				pLsPara->m_nRowNum=atoi(sTextValue);
			else if(iSubItem==2)//����
				pLsPara->m_nColNum=atoi(sTextValue);
			else if(iSubItem==3)//ֱ��
				pLsPara->m_nBoltD=atoi(sTextValue);
			else if(iSubItem==4)//�о�
				pLsPara->m_fRowSpace=atof(sTextValue);
			else if(iSubItem==5)//�о�
				pLsPara->m_fColSpace=atof(sTextValue);
			else if(iSubItem==6)//X
				pLsPara->m_posFirstBolt.x=atof(sTextValue);
			else if(iSubItem==7)//Y
				pLsPara->m_posFirstBolt.y=atof(sTextValue);
		}
		else if(pTubeEndLjDlg->m_iTubeEndLjStyle<=2)
		{
			int iCurSel=pSelItem->GetIndex();
			if(iCurSel>0)
				return FALSE;
			if(iSubItem==0)		//��˨ֱ��
				pTubeEndLjDlg->LayoutBoltPara.m_nBoltD=atoi(sTextValue);
			else if(iSubItem==1)//����
				pTubeEndLjDlg->LayoutBoltPara.m_nBoltNum=atoi(sTextValue);
			else if(iSubItem==2)//��˨����ֱ��
				pTubeEndLjDlg->LayoutBoltPara.m_fBoltLayoutD=atof(sTextValue);
			//������������˨��Ϣ��ʾ
			CPropTreeItem* pGroupItem=pTubeEndLjDlg->m_propList.FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("LayoutBoltParam"),NULL);
			if(pGroupItem)
				pTubeEndLjDlg->UpdateBoltProperty(pGroupItem);
		}
		//���²�����˨,ˢ�¶Ի�����ʾ
		pTubeEndLjDlg->RefreshBoltList();
		pTubeEndLjDlg->Redraw();
	}
	return TRUE;
}
static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem)
{	
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pListCtrl->GetParent();
	if(pTubeEndLjDlg==NULL || pTubeEndLjDlg->m_iTubeEndLjStyle<3 ||pTubeEndLjDlg->GetPartType()>0)
		return FALSE;	//pTubeEndLjDlg->GetPartType()>0��ʾ��׼������ʱ������ɾ����˨�� wjh-2018.3.9
	CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pLsPara=NULL,*pSelLsPara=(CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA*)pItem->m_idProp;
	if(pSelLsPara==NULL)
		return FALSE;
	for(pLsPara=pTubeEndLjDlg->LayoutBoltParaList.GetFirst();pLsPara;pLsPara=pTubeEndLjDlg->LayoutBoltParaList.GetNext())
	{
		if(pLsPara==pSelLsPara)
		{
			pTubeEndLjDlg->LayoutBoltParaList.DeleteCursor();
			break;
		}
	}
	pTubeEndLjDlg->LayoutBoltParaList.Clean();
	pTubeEndLjDlg->RefreshBoltList();	
	pTubeEndLjDlg->Invalidate(FALSE);
	return TRUE;
}

static void NewRibPlate(CLDSParamPlate *pParamPlate,CDesignTubeEndLjDlg::ATTACHPLATE_PARA *pAttachPlateParam,
						CLDSPlate *pDatumPlate,BOOL bUp,CLDSPlate *pWeldParentPlate)
{
	if(pDatumPlate==NULL)
		return;
	CLDSPlate *pRibPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pRibPlate->SetLayer(pParamPlate->layer()); //����ͼ����
	pRibPlate->cfgword=pParamPlate->cfgword;	//�����߰���ĺ�Ӧ��ʮ�ֲ����ĺ�һ�� 
	//ʮ�ֲ���߰庸�Ӹ�����ӦΪʮ�ֲ������Ľڵ�� wht 10-08-02
	if(pWeldParentPlate)
		pRibPlate->m_hPartWeldParent=pWeldParentPlate->handle;
	pRibPlate->iSeg=pParamPlate->iSeg;
	pRibPlate->cMaterial=pAttachPlateParam->cMaterial;
	pRibPlate->SetPartNo(pAttachPlateParam->sPartNo);
	pRibPlate->Thick=pAttachPlateParam->m_nThick;
	pRibPlate->layer(2)=pParamPlate->layer(2);
	pRibPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
	pRibPlate->m_fNormOffset=-0.5*pAttachPlateParam->m_nThick;
	pRibPlate->jdb_style=5;	//����ڻ�׼���������ĸְ�
	//�����߰�����ϵ
	CLsRef *pLsRef=NULL;
	double min_bolt_x=100000;
	int start1_end2_mid0=0;
	if(bUp)	//Z�������Ҳ��������ϵ���˨
		start1_end2_mid0=32;
	else	//Z�������Ҳ��������ϵ���˨
		start1_end2_mid0=42;
	for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{
		if(pLsRef->start1_end2_mid0!=start1_end2_mid0)
			continue;
		pRibPlate->AppendLsRef(*pLsRef);				//������˨
		(*pLsRef)->AddL0Thick(pRibPlate->handle,TRUE);	//������˨ͨ��
		(*pLsRef)->CalGuigeAuto();
		double len_offset=(*pLsRef)->des_base_pos.len_offset_dist;
		if(len_offset<min_bolt_x)
			min_bolt_x=len_offset;
	}
	pRibPlate->ucs=pDatumPlate->ucs;
	pRibPlate->ucs.origin+=(pRibPlate->ucs.axis_x*(min_bolt_x-pAttachPlateParam->E));
	//�ְ����λ�ü����߲���
	pRibPlate->designInfo.m_bEnableFlexibleDesign=TRUE;	//�������Ի����
	pRibPlate->designInfo.m_hBasePart=pDatumPlate->handle;
	pRibPlate->designInfo.norm.norm_style=4;	//��׼�����ϵ���Է���
	pRibPlate->designInfo.norm.hVicePart=pDatumPlate->handle;
	pRibPlate->designInfo.norm.nearVector.Set(0,0,1);
	pRibPlate->designInfo.origin.datum_pos_style=9;//��׼�����ϵ��������
	pRibPlate->designInfo.origin.des_para.hPart=pDatumPlate->handle;
	pRibPlate->designInfo.origin.datum_pos.Set(min_bolt_x-pAttachPlateParam->E,0,0);
	//�ְ�������
	pRibPlate->designInfo.iProfileStyle0123=3;	//����Ƹְ�����
	PROFILE_VERTEX *pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(0,0,0);
	pVertex->vertex.feature=1;
	pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(pAttachPlateParam->B,0,0);
	pVertex->vertex.feature=1;
	pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(pAttachPlateParam->B,pAttachPlateParam->C,0);
	pVertex->vertex.feature=1;
	pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(pAttachPlateParam->B-pAttachPlateParam->D,pAttachPlateParam->A,0);
	pVertex->vertex.feature=1;
	pVertex=pRibPlate->vertex_list.append();
	pVertex->vertex.Set(0,pAttachPlateParam->A,0);
	pVertex->vertex.feature=1;
	pRibPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(pRibPlate->GetSolidPartObject());
}

BOOL ButtonClickItem(CPropertyList* pPropList,CPropTreeItem* pItem);
BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr);
//�޸ĸ��Ӱ���ز��� 
static BOOL ModifAttachPlateValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{	//���Ӱ岼�ò���
	CXhChar100 tem_str;
	tem_str.Printf("%s",valueStr);
	CString sTempValue="";
	CPropTreeItem *pFindItem=NULL;
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	CPropertyListOper<CDesignTubeEndLjDlg> oper(pPropList,pTubeEndLjDlg);
 	if(CDesignTubeEndLjDlg::GetPropID("m_bCreateAttachPlate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pTubeEndLjDlg->m_bCreateAttachPlate=TRUE;
		else 
			pTubeEndLjDlg->m_bCreateAttachPlate=FALSE;
		pTubeEndLjDlg->UpdateAttachPlateProperty(pItem);	//���¸��Ӱ����
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.A")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.A=atof(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//ʮ�ֲ�帽���߰��������ɷ�ʽ ����Ƕ�ʱ����³��� wht 10-09-06
			if(pTubeEndLjDlg->AttachPlatePara.m_bUseAngle)
			{	//���ݽǶȸ���D
				double D=(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C)
					*tan(pTubeEndLjDlg->AttachPlatePara.m_fAngle*RADTODEG_COEF);
				pTubeEndLjDlg->AttachPlatePara.D=ftol(D*10)/10.0;	//����С�����һλ
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.D);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),sTempValue);
			}
			else
			{	//����A,D,C���½Ƕ�
				double tana=pTubeEndLjDlg->AttachPlatePara.A/(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C);
				pTubeEndLjDlg->AttachPlatePara.m_fAngle=atan(tana)*DEGTORAD_COEF;
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.m_fAngle);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),sTempValue);
			}
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.B")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.B=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.C")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.C=atof(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//ʮ�ֲ�帽���߰��������ɷ�ʽ ����Ƕ�ʱ����³��� wht 10-09-06
			if(pTubeEndLjDlg->AttachPlatePara.m_bUseAngle)
			{	//���ݽǶȸ���D
				double D=(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C)
					*tan(pTubeEndLjDlg->AttachPlatePara.m_fAngle*RADTODEG_COEF);
				pTubeEndLjDlg->AttachPlatePara.D=ftol(D*10)/10.0;	//����С�����һλ
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.D);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),sTempValue);
			}
			else
			{	//����A,D,C���½Ƕ�
				double tana=pTubeEndLjDlg->AttachPlatePara.A/(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C);
				pTubeEndLjDlg->AttachPlatePara.m_fAngle=atan(tana)*DEGTORAD_COEF;
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.m_fAngle);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),sTempValue);
			}
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.D=atof(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//ʮ�ֲ�帽���߰��������ɷ�ʽ ����Ƕ�ʱ����³��� wht 10-09-06
			//����A,D,C���½Ƕ�
			double tana=pTubeEndLjDlg->AttachPlatePara.A/(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C);
			pTubeEndLjDlg->AttachPlatePara.m_fAngle=atan(tana)*DEGTORAD_COEF;
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.m_fAngle);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),sTempValue);
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.E")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.E=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.F")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.F=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.G")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.G=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.H")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.H=atof(tem_str);
		if(pTubeEndLjDlg->AttachPlatePara.H>0)
		{
			oper.InsertEditPropItem(pItem,"AttachPlatePara.F","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"AttachPlatePara.G","","",-1,TRUE);
		}
		else
			pPropList->DeleteAllSonItems(pItem);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_iCutMode")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.m_iCutMode=tem_str[0]-'0';
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.cMaterial")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.sPartNo")==pItem->m_idProp)
	{
		strcpy(pTubeEndLjDlg->AttachPlatePara.sPartNo,tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(pTubeEndLjDlg->AttachPlatePara.sPartNo);
		if(pPart)
		{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
			if(pPart->cPartType==BOMPART::PLATE)
			{
				pTubeEndLjDlg->AttachPlatePara.cMaterial=pPart->cMaterial;
				pTubeEndLjDlg->AttachPlatePara.m_nThick=(int)pPart->thick;
				char sTemp[50]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_nThick");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
				idProp=CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.cMaterial");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
			}
			//else
			//AfxMessageBox(CXhChar100("Bom��û�м���Ϊ#%s�ĸְ�",(char*)pPart->sPartNo));
		}
#endif
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle")==pItem->m_idProp)
	{
		pTubeEndLjDlg->AttachPlatePara.m_fAngle=atof(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//ʮ�ֲ�帽���߰��������ɷ�ʽ ����Ƕ�ʱ����³��� wht 10-09-06
			double D=(pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C)
				*tan(pTubeEndLjDlg->AttachPlatePara.m_fAngle*RADTODEG_COEF);
			pTubeEndLjDlg->AttachPlatePara.D=ftol(D*10)/10.0;	//����С�����һλ
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.D);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),sTempValue);
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_nPlateNum")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.m_nPlateNum=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_nThick")==pItem->m_idProp)
		pTubeEndLjDlg->AttachPlatePara.m_nThick=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_bUseAngle")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pTubeEndLjDlg->AttachPlatePara.m_bUseAngle=TRUE;
		else 
			pTubeEndLjDlg->AttachPlatePara.m_bUseAngle=FALSE;
		//
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(pTubeEndLjDlg->AttachPlatePara.m_bUseAngle);
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),NULL);
		if(pFindItem)
			pFindItem->SetReadOnly(!pTubeEndLjDlg->AttachPlatePara.m_bUseAngle);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_bMirCreate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pTubeEndLjDlg->AttachPlatePara.m_bMirCreate=TRUE;
		else 
			pTubeEndLjDlg->AttachPlatePara.m_bMirCreate=FALSE;
	}
	//������
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCreateFillPlate")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pTubeEndLjDlg->m_bCreateFillPlate=TRUE;
		else 
			pTubeEndLjDlg->m_bCreateFillPlate=FALSE;
		pTubeEndLjDlg->UpdateFillPlateProperty(pItem);	//����������
	}
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.cMat")==pItem->m_idProp)
		pTubeEndLjDlg->FillPlatePara.cMat=QuerySteelBriefMatMark(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.sPartNo")==pItem->m_idProp)
	{
		strcpy(pTubeEndLjDlg->FillPlatePara.sPartNo,tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(pTubeEndLjDlg->FillPlatePara.sPartNo);
		if(pPart)
		{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
			if(pPart->cPartType==BOMPART::PLATE)
			{
				pTubeEndLjDlg->FillPlatePara.cMat=pPart->cMaterial;
				pTubeEndLjDlg->FillPlatePara.nThick=(int)pPart->thick;
				char sTemp[50]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nThick");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
				idProp=CDesignTubeEndLjDlg::GetPropID("FillPlatePara.cMat");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
			}
			//else
			//AfxMessageBox(CXhChar100("Bom��û�м���Ϊ#%s�ĸְ�",(char*)pPart->sPartNo));
		}
#endif
	}
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nThick")==pItem->m_idProp)
		pTubeEndLjDlg->FillPlatePara.nThick=atoi(tem_str);
	else
		return FALSE;
	pTubeEndLjDlg->Invalidate(FALSE);
	return TRUE;
}

static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	if(pItem->m_lpNodeInfo->m_strPropValue.CompareNoCase(valueStr)==0)
		return TRUE;	//ֵδ�����仯ʱ���������²��� wht 11-01-26
	CPropTreeItem *pFindItem=NULL,*pPropItem=NULL,*pRoot=pPropList->GetRootItem();
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	//���if(pTubeEndLjDlg->IsHasExistEndLj())��Ϊ�˱����޸�������ƣ�����2010�꺣�θ�ƫ�ˣ����ھ��� wjh-2017.12.28
	//if(pTubeEndLjDlg->IsHasExistEndLj()&&ModifAttachPlateValue(pPropList,pItem,valueStr))
	if(ModifAttachPlateValue(pPropList,pItem,valueStr))
		return TRUE;	//�Ѵ��ڶ��������
	CXhChar100 tem_str;
	tem_str.Printf("%s",valueStr);
	CXhChar100 sCurPara;
	CString sTempValue="";
	char sTextValue[100]="";
	BOOL bUpdateLayoutBoltPara=FALSE;
	int strLen=pTubeEndLjDlg->GetPropStr(pItem->m_idProp,sCurPara);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(pPropList,pTubeEndLjDlg);
	if(strLen==1&&((sCurPara[0]>='A'&&sCurPara[0]<='Z')||(sCurPara[0]>='a'&&sCurPara[0]<='z')))
	{	//����������Ʋ��� Ϊ A~Z��a~z֮����ַ�
		DESIGN_PLATE_ITEM *pDesignItem=pTubeEndLjDlg->m_paramPlate.GetDesignItem(sCurPara[0]);
		if(pDesignItem)
		{
			CXhChar200 sNotes;
			sNotes.Copy(pDesignItem->sNotes);
			if(pDesignItem->dataType==1)
			{
				long handle=0;
				sscanf(tem_str,"%X",&handle);
				pTubeEndLjDlg->m_paramPlate.SetDesignItemValue(pDesignItem->key,handle,sNotes,pDesignItem->paramType,1);
			}
			else
				pTubeEndLjDlg->m_paramPlate.SetDesignItemValue(pDesignItem->key,atof(tem_str),sNotes,pDesignItem->paramType);
		}
		//pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5&&(sCurPara[0]=='H'||sCurPara[0]=='S'||sCurPara[0]=='W'))
		{	//����ʮ�ֲ�帽�Ӱ����β���
			double H=0,T=0,S=0,W=0;
			T=pTubeEndLjDlg->m_paramPlate.thick;
			pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('H',&H);
			pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('S',&S);
			pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('W',&W);
			pTubeEndLjDlg->AttachPlatePara.A=0.5*(H-T);
			pTubeEndLjDlg->AttachPlatePara.B=S;
			pTubeEndLjDlg->AttachPlatePara.m_nThick=ftoi(T);
			pTubeEndLjDlg->AttachPlatePara.D=pTubeEndLjDlg->AttachPlatePara.A-pTubeEndLjDlg->AttachPlatePara.C;
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.A);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.A"),sTempValue);
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.B);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.B"),sTempValue);
			sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.D);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.D"),sTempValue);
			sTempValue.Format("%d",pTubeEndLjDlg->AttachPlatePara.m_nThick);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_nThick"),sTempValue);
		}
		else if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
		{	
			if(sCurPara[0]=='R')
			{	//��ͨ�����뾶����������ʾΪ����ֱ�������û��鿴�޸�
				double fInnerD=atof(tem_str);
				pTubeEndLjDlg->m_paramPlate.SetDesignItemValue('R',fInnerD*0.5);
			}
			else if(sCurPara[0]=='W')
			{	//���·��������B
				double R=0,W=0;
				pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('R',&R);
				pTubeEndLjDlg->m_paramPlate.GetDesignItemValue('W',&W);
				pTubeEndLjDlg->AttachPlatePara.B=ftoi(0.5*W-R);
				sTempValue.Format("%.f",pTubeEndLjDlg->AttachPlatePara.B);
				SimplifiedNumString(sTempValue);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.B"),sTempValue);
			}
		}
	}
	//��������
	else if(CDesignTubeEndLjDlg::GetPropID("m_iPartType")==pItem->m_idProp)
	{
		pTubeEndLjDlg->SetPartType(tem_str[0]-'0');
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2)
		{
			int iPartType=pTubeEndLjDlg->GetPartType();
			if(iPartType==1||iPartType==2)
				pTubeEndLjDlg->m_arrPropGroupLabel.SetSize(3);
			else
			{
				pTubeEndLjDlg->m_arrPropGroupLabel.SetSize(4);
				pTubeEndLjDlg->m_arrPropGroupLabel.SetAt(3,"���Ӱ�");
				pPropItem=oper.InsertCmbListPropItem(pRoot,"m_bCreateAttachPlate");
				pPropItem->m_dwPropGroup=GetSingleWord(4);
				pTubeEndLjDlg->UpdateAttachPlateProperty(pPropItem);
			}
			pTubeEndLjDlg->RefreshTabCtrl(0);	//ˢ��TabCtrl wht 12-05-16
		}
		//���²��ұ�׼��������,���������Ϣ
		pTubeEndLjDlg->LoadStdPlateParaByLevel();
		pTubeEndLjDlg->UpdateDesignParam(FALSE);
		//
		pTubeEndLjDlg->UpdatePartTypeProperty(pItem);
		//���»�������
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
		//���¸ֹܶ�����λ��
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("TubeEndPos"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateTubeEndProperty(pFindItem);
		//����װ�����
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("SetUpPara"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateSetUpProperty(pFindItem);
		//
		pTubeEndLjDlg->UpdateSketchMap();
		pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ
		bUpdateLayoutBoltPara=TRUE;			//������˨��Ϣ
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iWrapPlateType")==pItem->m_idProp)
		pTubeEndLjDlg->m_iWrapPlateType=tem_str[0]-'0';
	//��׼��������
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStdPartLevel")==pItem->m_idProp)
	{
   		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2)
			pTubeEndLjDlg->m_cFlLevel1=tem_str[0];
		else if(pTubeEndLjDlg->m_iTubeEndLjStyle>2)
			pTubeEndLjDlg->m_iStdPartLevel=atoi(tem_str);
		//�޸ı�׼��弶�� ���²��ұ�׼��������,���������Ϣ
		pTubeEndLjDlg->LoadStdPlateParaByLevel();
		pTubeEndLjDlg->UpdateDesignParam(FALSE);
		//���»�������
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
		pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ
		bUpdateLayoutBoltPara=TRUE;	//������˨��Ϣ
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStdPartLevel2")==pItem->m_idProp)
	{
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2)
			pTubeEndLjDlg->m_cFlLevel2=tem_str[0];
		//�޸ı�׼��弶�� ���²��ұ�׼��������,���������Ϣ
		pTubeEndLjDlg->LoadStdPlateParaByLevel();
		pTubeEndLjDlg->UpdateDesignParam(FALSE);
		//���»�������
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
	}
	//��׼��������
	else if(CDesignTubeEndLjDlg::GetPropID("m_cStdPartMat")==pItem->m_idProp)
	{	//���ݲ��ʲ��ұ��Ҳ��
		char cMat=QuerySteelBriefMatMark(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle>2)
		{
			int xLevelArr[9]={0};
			INSERT_PLATE_PARAM *pChaBanParam=pTubeEndLjDlg->QueryStdInsertPlateByMat(cMat,xLevelArr);
			if(pChaBanParam==NULL)
			{
				AfxMessageBox(CXhChar100("��׼������û���ҵ��ֹܲ���Ϊ%s�Ĳ��",(char*)tem_str));
				return FALSE;
			}
			pTubeEndLjDlg->m_arrStdPartLevel.RemoveAll();
			for(int i=0;i<9;i++)
			{
				if(xLevelArr[i]==0)
					break;
				pTubeEndLjDlg->m_arrStdPartLevel.Add(CXhChar16("%d",xLevelArr[i]));
			}
			pTubeEndLjDlg->m_iStdPartLevel=xLevelArr[0];
		}
		else if(pTubeEndLjDlg->m_iTubeEndLjStyle==2)
		{
			int xLevelArr[9]={0};
			FL_PARAM *pFlDPara=pTubeEndLjDlg->QueryStdFlDParaByMat(cMat,xLevelArr);
			if(pFlDPara==NULL)
			{
				AfxMessageBox(CXhChar100("��׼�Ժ���������û���ҵ�����Ϊ%s�ĶԺ�����",(char*)tem_str));
				return FALSE;
			}
			pTubeEndLjDlg->m_arrStdFlDLevelM.RemoveAll();
			for(int i=0;i<9;i++)
			{
				if(xLevelArr[i]==0)
					break;
				pTubeEndLjDlg->m_arrStdFlDLevelM.Add(CXhChar16("%d",xLevelArr[i]));
			}
			pTubeEndLjDlg->m_cFlLevel1=pFlDPara->cLevelMark;
		}
		pTubeEndLjDlg->m_cStdPartMat=cMat;
		pTubeEndLjDlg->LoadStdPlateParaByLevel();
		pTubeEndLjDlg->UpdateDesignParam(FALSE);
		//���»�������
		oper.UpdatePropItemValue("m_iStdPartLevel");
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
		pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ
		bUpdateLayoutBoltPara=TRUE;	//������˨��Ϣ
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCoupleFLCreate")==pItem->m_idProp)
	{
		pTubeEndLjDlg->m_bCoupleFLCreate=tem_str[0]!='0';
		//���»�������
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("basicInfo"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBaseInfoProperty(pFindItem);
	}
	//ƽ���и�
	else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.x")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.y")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.z")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y")
		||pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z"))
	{
		if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.x"))
			pTubeEndLjDlg->cutPlaneOrigin.x = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.y"))
			pTubeEndLjDlg->cutPlaneOrigin.y = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.z"))
			pTubeEndLjDlg->cutPlaneOrigin.z = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x"))
			pTubeEndLjDlg->cutPlaneNorm.x = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y"))
			pTubeEndLjDlg->cutPlaneNorm.y = atof(valueStr);
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z"))
			pTubeEndLjDlg->cutPlaneNorm.z = atof(valueStr);
		pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ
	}
	//�ֹ����
	else if(CDesignTubeEndLjDlg::GetPropID("m_bFaceToFace")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pTubeEndLjDlg->m_bFaceToFace=TRUE;
		else
			pTubeEndLjDlg->m_bFaceToFace=FALSE;
		pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube2"));
		if(!pTubeEndLjDlg->m_bFaceToFace)
		{	//���ֹ�2
			pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("TubeTransect"),NULL);
			if(pFindItem)
				oper.InsertBtnEditPropItem(pFindItem,"m_hDatumTube2","","",-1,TRUE);
		}
		pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ wht 11-05-10
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_hWeldLinkObj")==pItem->m_idProp)
		pTubeEndLjDlg->m_hWeldLinkObj=HexStrToLong(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube1")==pItem->m_idProp)
	{
		sscanf(tem_str,"%X",&pTubeEndLjDlg->m_hDatumTube1);
		pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ wht 11-05-10
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube2")==pItem->m_idProp)
	{
		sscanf(tem_str,"%X",&pTubeEndLjDlg->m_hDatumTube2);
		pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ wht 11-05-10
	}
	//������Ϣ
	else if(CDesignTubeEndLjDlg::GetPropID("sPartNo")==pItem->m_idProp)
	{
		pTubeEndLjDlg->m_paramPlate.SetPartNo(tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(pTubeEndLjDlg->m_paramPlate.Label);
		if(pPart)
		{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
			if(pPart->cPartType==BOMPART::PLATE)
			{
				pTubeEndLjDlg->m_paramPlate.cMaterial=pPart->cMaterial;
				pTubeEndLjDlg->m_paramPlate.thick=(int)pPart->thick;
				char sTemp[50]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("thick");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
				idProp=CDesignTubeEndLjDlg::GetPropID("cMaterial");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
			}
			//else
			//AfxMessageBox(CXhChar100("Bom��û�м���Ϊ#%s�ĸְ�",(char*)pPart->sPartNo));
		}
#endif
	}
	else if(CDesignTubeEndLjDlg::GetPropID("sCoupleFlPlateNo")==pItem->m_idProp)
		strcpy(pTubeEndLjDlg->sCoupleFlPlateNo,tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("sWeldPlatePartNo")==pItem->m_idProp)
		strcpy(pTubeEndLjDlg->sWeldPlatePartNo,tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("sWrapPlatePartNo")==pItem->m_idProp)
		strcpy(pTubeEndLjDlg->sWrapPlatePartNo,tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("iSeg")==pItem->m_idProp)
		pTubeEndLjDlg->m_paramPlate.iSeg=SEGI(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("cMaterial")==pItem->m_idProp)
		pTubeEndLjDlg->m_paramPlate.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_uStatMatCoef")==pItem->m_idProp)
		pTubeEndLjDlg->m_paramPlate.m_uStatMatCoef=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("thick")==pItem->m_idProp)
	{
		pTubeEndLjDlg->m_paramPlate.thick=atoi(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
		{	//��ͨ����
			double H=ftoi(0.5*pTubeEndLjDlg->m_paramPlate.thick);
			pTubeEndLjDlg->m_paramPlate.SetDesignItemValue('H',H);
			sTempValue.Format("%f",H);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("H"),sTempValue);
		}
	}
	//װ�����
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//�Զ��巽��
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x"),
				pTubeEndLjDlg->setupInfo.workNorm.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y"),
				pTubeEndLjDlg->setupInfo.workNorm.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z"),
				pTubeEndLjDlg->setupInfo.workNorm.norm_style!=0);
		}
		else
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z"));
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pTubeEndLjDlg->setupInfo.workNorm.norm_style=0;	//ָ����˿
			pTubeEndLjDlg->setupInfo.workNorm.vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x"),sText);
			sText.Format("%.f",vector.y);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y"),sText);
			sText.Format("%.f",vector.z);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z"),sText);
		}
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
		return TRUE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNorm.vector.x=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNorm.vector.y=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNorm.vector.z=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//�Զ��巽��
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x"),
				pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y"),
				pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z"),
				pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style!=0);
		}
		else
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z"));
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style=0;	//ָ����˿
			pTubeEndLjDlg->setupInfo.workNormUpVert.vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x"),sText);
			sText.Format("%.f",vector.y);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y"),sText);
			sText.Format("%.f",vector.z);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z"),sText);
		}
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
		return TRUE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormUpVert.vector.x=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormUpVert.vector.y=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormUpVert.vector.z=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm")==pItem->m_idProp)
	{
		if(valueStr[0]=='0')	//�Զ��巽��
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x"),
				pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y"),
				pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style!=0);
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z"),
				pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style!=0);
		}
		else
		{
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y"));
			pPropList->SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z"));
			f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
			pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style=0;	//ָ����˿
			pTubeEndLjDlg->setupInfo.workNormDownVert.vector=vector;
			CString sText;
			sText.Format("%.f",vector.x);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x"),sText);
			sText.Format("%.f",vector.y);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y"),sText);
			sText.Format("%.f",vector.z);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z"),sText);
		}
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
		return TRUE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormDownVert.vector.x=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormDownVert.vector.y=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.workNormDownVert.vector.z=atof(valueStr);
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.norm_style")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
		{
			//ת�����Ʒ���
			pTubeEndLjDlg->setupInfo.workNorm.norm_style=valueStr[0]-'0';
			if(pTubeEndLjDlg->setupInfo.workNorm.norm_style==2)	//���ת����
				pTubeEndLjDlg->setupInfo.workNorm.nearVector=pTubeEndLjDlg->setupInfo.workNorm.vector;
			else if(pTubeEndLjDlg->setupInfo.workNorm.norm_style==4)	//����ת���
			{
				CLDSPart* pDatumPart=NULL;
				if(pTubeEndLjDlg->setupInfo.workNorm.hVicePart>0x20)
					pDatumPart=console.FromPartHandle(pTubeEndLjDlg->setupInfo.workNorm.hVicePart);
				if(pDatumPart==NULL)
					pDatumPart=&pTubeEndLjDlg->m_paramPlate;
				pTubeEndLjDlg->setupInfo.workNorm.nearVector=pTubeEndLjDlg->setupInfo.workNorm.vector;
				vector_trans(pTubeEndLjDlg->setupInfo.workNorm.nearVector,pDatumPart->ucs,FALSE);
			}
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z"),valueStr[0]!='0');
		}
		else if(pPropList->m_nObjClassTypeId==1)
		{
			//ת�����Ʒ���
			pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style=valueStr[0]-'0';
			if(pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style==2)	//���ת����
				pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector=pTubeEndLjDlg->setupInfo.workNormUpVert.vector;
			else if(pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style==4)	//����ת���
			{
				CLDSPart* pDatumPart=NULL;
				if(pTubeEndLjDlg->setupInfo.workNormUpVert.hVicePart>0x20)
					pDatumPart=console.FromPartHandle(pTubeEndLjDlg->setupInfo.workNormUpVert.hVicePart);
				if(pDatumPart==NULL)
					pDatumPart=&pTubeEndLjDlg->m_paramPlate;
				pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector=pTubeEndLjDlg->setupInfo.workNormUpVert.vector;
				vector_trans(pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector,pDatumPart->ucs,FALSE);
			}
			pTubeEndLjDlg->setupInfo.workNormUpVert.norm_style=valueStr[0]-'0';
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z"),valueStr[0]!='0');
		}
		else if(pPropList->m_nObjClassTypeId==2)
		{
			//ת�����Ʒ���
			pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style=valueStr[0]-'0';
			if(pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style==2)	//���ת����
				pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector=pTubeEndLjDlg->setupInfo.workNormDownVert.vector;
			else if(pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style==4)	//����ת���
			{
				CLDSPart* pDatumPart=NULL;
				if(pTubeEndLjDlg->setupInfo.workNormDownVert.hVicePart>0x20)
					pDatumPart=console.FromPartHandle(pTubeEndLjDlg->setupInfo.workNormDownVert.hVicePart);
				if(pDatumPart==NULL)
					pDatumPart=&pTubeEndLjDlg->m_paramPlate;
				pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector=pTubeEndLjDlg->setupInfo.workNormDownVert.vector;
				vector_trans(pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector,pDatumPart->ucs,FALSE);
			}
			pTubeEndLjDlg->setupInfo.workNormDownVert.norm_style=valueStr[0]-'0';
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y"),valueStr[0]!='0');
			pTubeEndLjDlg->m_propList.SetItemReadOnly(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z"),valueStr[0]!='0');
		}
		pTubeEndLjDlg->DisplayChildPropertyList(NULL,TRUE);
		return TRUE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.norm_wing")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.norm_wing=valueStr[0]-'0';
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.norm_wing=valueStr[0]-'0';
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.norm_wing=valueStr[0]-'0';
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.direction")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.direction=valueStr[0]-'0';
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.direction=valueStr[0]-'0';
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.direction=valueStr[0]-'0';
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.rot_ang")==pItem->m_idProp)
	{
		double fRotAngle=0;
		f3dPoint view_norm=pTubeEndLjDlg->GetWorkNorm();
		f3dPoint tube_work_norm=pTubeEndLjDlg->m_pCurLineTube->WorkPlaneNorm();
		if(strcmp(tem_str,"����")==0)
		{	
			if(tube_work_norm*view_norm>0)
				fRotAngle=180;	//�����淨�߷�����ֹܻ�׼�淨�߷�����ͬ
			else 
				fRotAngle=0;
		}
		else if(strcmp(tem_str,"����")==0)
		{
			if(tube_work_norm*view_norm>0)
				fRotAngle=0;	//�����淨�߷�����ֹܻ�׼�淨�߷�����ͬ
			else 
				fRotAngle=180;
		}
		else
			fRotAngle=atof(tem_str);
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.rot_ang=fRotAngle;
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.rot_ang=fRotAngle;
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.rot_ang=fRotAngle;
		DESIGN_PLATE_ITEM *pDesignItem=pTubeEndLjDlg->m_paramPlate.GetDesignItem('D');
		pTubeEndLjDlg->m_paramPlate.SetDesignItemValue('D',fRotAngle,pDesignItem->sNotes,pDesignItem->paramType);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVectorType")==pItem->m_idProp)
	{
		if(valueStr[0]-'0'==0)	//�Զ��巽��
		{
			CXhChar100 sText;
			CItemInfo *lpInfo;
			CPropTreeItem *pSonItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X����";
				lpInfo->m_strPropHelp = "X����";
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x");
				if(pTubeEndLjDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pSonItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y����";
				lpInfo->m_strPropHelp = "Y����";
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y");
				if(pTubeEndLjDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
			
			pSonItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z����";
				lpInfo->m_strPropHelp = "Z����";
				pSonItem = pPropList->InsertItem(pItem,lpInfo, -1,TRUE);
				pSonItem->m_idProp =CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z");
				if(pTubeEndLjDlg->GetPropValueStr(pSonItem->m_idProp,sText)>0)
					pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		else
		{
			pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x"));
			pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y"));
			pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z"));
			if(pPropList->m_nObjClassTypeId==0)
				pTubeEndLjDlg->setupInfo.workNorm.nearVector=CStdVector::GetVector(valueStr[0]-'0');
			else if(pPropList->m_nObjClassTypeId==1)
				pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector=CStdVector::GetVector(valueStr[0]-'0');
			else if(pPropList->m_nObjClassTypeId==2)
				pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector=CStdVector::GetVector(valueStr[0]-'0');
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.nearVector.x=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector.x=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector.x=atof(valueStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.nearVector.y=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector.y=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector.y=atof(valueStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z")==pItem->m_idProp)
	{
		if(pPropList->m_nObjClassTypeId==0)
			pTubeEndLjDlg->setupInfo.workNorm.nearVector.z=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==1)
			pTubeEndLjDlg->setupInfo.workNormUpVert.nearVector.z=atof(valueStr);
		else if(pPropList->m_nObjClassTypeId==2)
			pTubeEndLjDlg->setupInfo.workNormDownVert.nearVector.z=atof(valueStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("H1")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.H1=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fNormOffset")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.m_fNormOffset=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fOddment")==pItem->m_idProp)
	{
		pTubeEndLjDlg->setupInfo.m_fOddment=atof(tem_str);
		//�û��ֶ�����ֹ�����ͷ�Ժ󣬲���֧���Զ���������ͷ wht 11-05-09
		if(pTubeEndLjDlg->m_bStartEnd)
			pTubeEndLjDlg->m_pCurLineTube->desStartOdd.m_iOddCalStyle=2;	//�ֶ�����
		else 
			pTubeEndLjDlg->m_pCurLineTube->desEndOdd.m_iOddCalStyle=2;		//�ֶ�����
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fWeldHeight")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.m_fWeldHeight=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fTubeNormOffset")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.m_fTubeNormOffset=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fTubeEccentricDist")==pItem->m_idProp)
		pTubeEndLjDlg->setupInfo.m_fTubeEccentricDist=atof(tem_str);
	//�ڵ�����
	else if(CDesignTubeEndLjDlg::GetPropID("NodePlatePara.cMaterial")==pItem->m_idProp)
		pTubeEndLjDlg->NodePlatePara.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("NodePlatePara.sPartNo")==pItem->m_idProp)
	{
		strcpy(pTubeEndLjDlg->NodePlatePara.sPartNo,tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(pTubeEndLjDlg->NodePlatePara.sPartNo);
		if(pPart)
		{	//�����ҵ���BOM��Ϣ�����¸øְ�Ļ�����Ϣ(�κš���񡢲���)
			if(pPart->cPartType==BOMPART::PLATE)
			{
				pTubeEndLjDlg->NodePlatePara.cMaterial=pPart->cMaterial;
				pTubeEndLjDlg->NodePlatePara.m_nThick=(int)pPart->thick;
				char sTemp[50]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("NodePlatePara.m_nThick");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
				idProp=CDesignTubeEndLjDlg::GetPropID("NodePlatePara.cMaterial");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sTemp)>0)
					pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sTemp);
			}
			//else
			//AfxMessageBox(CXhChar100("Bom��û�м���Ϊ#%s�ĸְ�",(char*)pPart->sPartNo));
		}
#endif
	}
	else if(CDesignTubeEndLjDlg::GetPropID("NodePlatePara.m_nRollHeight")==pItem->m_idProp)
		pTubeEndLjDlg->NodePlatePara.m_nRollHeight=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("NodePlatePara.m_nThick")==pItem->m_idProp)
	{
		pTubeEndLjDlg->NodePlatePara.m_nThick=atoi(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{	//���½ڵ���� wht 11-05-11
			pTubeEndLjDlg->FillPlatePara.nNodePlateThick=atoi(tem_str);
			char sThick[50]="";
			long idProp=CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nNodePlateThick");
			if(pTubeEndLjDlg->GetPropValueStr(idProp,sThick)>0)
				pTubeEndLjDlg->m_propList.SetItemPropValue(idProp,sThick);
		}
	}
	//��˨���ò���
	else if(CDesignTubeEndLjDlg::GetPropID("m_nBoltD")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_nBoltD=atoi(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
			pTubeEndLjDlg->UpdateLayoutBoltPara();
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iDriection")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_iDirection=tem_str[0]-'0';
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStartEdge")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_iStartEdge=tem_str[0]-'0';
	else if(CDesignTubeEndLjDlg::GetPropID("m_fOffsetAngle")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_fOffsetAngle=atof(tem_str);
		//��ͨ����ֱ�ӵ���ÿ����˨��ƫ�ƽǣ���׼������������X��ƫ�ƽǶ� wht 11-05-23
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
		{	//��˨���޸ĺ���·�����������Լ��׿�����ƫ�ƽǶ�
			pTubeEndLjDlg->AttachPlatePara.m_fAngle=pTubeEndLjDlg->LayoutBoltPara.m_fOffsetAngle
				+180.0/pTubeEndLjDlg->AttachPlatePara.m_nPlateNum;
			sTempValue.Format("%f",pTubeEndLjDlg->AttachPlatePara.m_fAngle);
			SimplifiedNumString(sTempValue);
			pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_fAngle"),sTempValue);
			pTubeEndLjDlg->UpdateLayoutBoltPara();
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fBoltLayoutD")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_fBoltLayoutD=atof(tem_str);
		pTubeEndLjDlg->UpdateLayoutBoltPara();
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_nBoltNum")==pItem->m_idProp)
	{	//�����ϵ���˨����
		int nBoltNum=atoi(tem_str);
		if(nBoltNum==pTubeEndLjDlg->LayoutBoltPara.m_nBoltNum)
			return FALSE;
		pTubeEndLjDlg->LayoutBoltPara.m_nBoltNum=nBoltNum;
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==2&&pTubeEndLjDlg->GetPartType()==0)
		{
			pTubeEndLjDlg->UpdateLayoutBoltPara();
			//��˨���޸ĺ���·�����������Լ��׿�����ƫ�ƽǶ�
			pTubeEndLjDlg->AttachPlatePara.m_nPlateNum=nBoltNum;
			pTubeEndLjDlg->AttachPlatePara.m_fAngle=pTubeEndLjDlg->LayoutBoltPara.m_fOffsetAngle+180.0/nBoltNum;
			oper.UpdatePropItemValue("AttachPlatePara.m_nPlateNum");
			oper.UpdatePropItemValue("AttachPlatePara.m_fAngle");
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_bStaggerLayout")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pTubeEndLjDlg->LayoutBoltPara.m_bStaggerLayout=TRUE;
		else
			pTubeEndLjDlg->LayoutBoltPara.m_bStaggerLayout=FALSE;
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_bMirLayout")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout=TRUE;
		else
			pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout=FALSE;
		pPropList->DeleteItemByPropId(CDesignTubeEndLjDlg::GetPropID("m_fSpecialSpace"));
		if(pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout)
			oper.InsertEditPropItem(pItem,"m_fSpecialSpace","","",-1,TRUE);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fSpecialSpace")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_fSpecialSpace=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_nRowNum")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_nRowNum=atoi(tem_str);
		if(pTubeEndLjDlg->m_iTubeEndLjStyle==5)
		{
			if(pTubeEndLjDlg->LayoutBoltPara.m_nRowNum!=1)
				pTubeEndLjDlg->LayoutBoltPara.m_bStaggerLayout=FALSE;
			pPropItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("m_bStaggerLayout"),NULL);
			if(pPropItem)
			{	//������ˮƽ������ֱ����˨
				oper.UpdatePropItemValue("m_bStaggerLayout");
				pPropItem->SetReadOnly(pTubeEndLjDlg->LayoutBoltPara.m_nRowNum!=1);
			}
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_nColNum")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_nColNum=atoi(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_fRowSpace")==pItem->m_idProp)
	{
		pTubeEndLjDlg->LayoutBoltPara.m_fRowSpace=atof(tem_str);
		pTubeEndLjDlg->LayoutBoltPara.m_posFirstBolt.y=-0.5*atof(tem_str);
		oper.UpdatePropItemValue("m_posFirstBolt.y");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fColSpace")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_fColSpace=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_posFirstBolt.x")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_posFirstBolt.x=atof(tem_str);
	else if(CDesignTubeEndLjDlg::GetPropID("m_posFirstBolt.y")==pItem->m_idProp)
		pTubeEndLjDlg->LayoutBoltPara.m_posFirstBolt.y=atof(tem_str);
	//��Ȼ��߲��Ͳ�峯�����仯ʱ���¸ֹܷ���ƫ����
	if(pTubeEndLjDlg->m_iTubeEndLjStyle==4&&pTubeEndLjDlg->NodePlatePara.m_nThick>0
		&&(CDesignTubeEndLjDlg::GetPropID("T")==pItem->m_idProp
		||CDesignTubeEndLjDlg::GetPropID("design_axis_z.rot_ang")==pItem->m_idProp
		||CDesignTubeEndLjDlg::GetPropID("NodePlatePara.m_nThick")==pItem->m_idProp))	//���Ͳ��
	{
		pTubeEndLjDlg->setupInfo.m_fTubeNormOffset=-(pTubeEndLjDlg->NodePlatePara.m_nThick+pTubeEndLjDlg->m_paramPlate.thick)*0.5;
		if(fabs(fabs(pTubeEndLjDlg->setupInfo.workNorm.rot_ang)-180)<EPS)	//���Ͳ����ת180��
			pTubeEndLjDlg->setupInfo.m_fTubeNormOffset*=-1.0;
		else if(fabs(pTubeEndLjDlg->setupInfo.workNorm.rot_ang)>EPS)
			pTubeEndLjDlg->setupInfo.m_fTubeNormOffset=0;	//��0���180��ʱƫ��������	
		sTempValue.Format("%f",pTubeEndLjDlg->setupInfo.m_fTubeNormOffset);
		SimplifiedNumString(sTempValue);
		pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("m_fTubeNormOffset"),sTempValue);
	}
	if(bUpdateLayoutBoltPara)
	{
		//������˨���ò���
		pTubeEndLjDlg->UpdateLayoutBoltPara();
		//��������������
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("Profile"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateProfileProperty(pFindItem);
		//��˨����������
		pFindItem=pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("LayoutBoltParam"),NULL);
		if(pFindItem)
			pTubeEndLjDlg->UpdateBoltProperty(pFindItem);
	}
	pTubeEndLjDlg->Invalidate(FALSE);
	return TRUE;
}

static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pListCtrl->GetParent();
	if(pTubeEndLjDlg==NULL)
		return FALSE;
	if(pTubeEndLjDlg->GetPartType()>0)
		return FALSE;	//��׼������֧�����ɾ����˨����
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING,ID_DELETE_BOLT_ITEM,"ɾ����˨");
	pMenu->AppendMenu(MF_STRING,ID_NEW_BOLT_ITEM,"�����˨");
	pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"ɾ��ȫ����˨");
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pTubeEndLjDlg);
	return TRUE;
}
static BOOL PropHelpPrompt(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	pTubeEndLjDlg->RefreshCtrlState(TRUE);
	if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm"))
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
	else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm"))
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
	else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm"))
		pTubeEndLjDlg->DisplayChildPropertyList(pItem,FALSE);
	return TRUE;
}
static BOOL ButtonClickItem(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	char valueStr[200]="";
	CPropTreeItem *pFindItem=NULL;
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("design_axis_z.hVicePart"))
		pTubeEndLjDlg->SelectObject(CLS_PART,pItem->m_idProp);		//ѡ���׼����
	else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("design_axis_z.hCrossPart"))
		pTubeEndLjDlg->SelectObject(CLS_LINEPART,pItem->m_idProp);	//ѡ���׼�˼�
	else if(!pTubeEndLjDlg->IsHasExistEndLj())	//�����ڶ��������
	{
		if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube1")==pItem->m_idProp
			||CDesignTubeEndLjDlg::GetPropID("m_hDatumTube2")==pItem->m_idProp)
			pTubeEndLjDlg->SelectObject(CLS_LINETUBE,pItem->m_idProp);	//ѡ���׼�ֹ�
		if(CDesignTubeEndLjDlg::GetPropID("m_hWeldLinkObj")==pItem->m_idProp)
			pTubeEndLjDlg->SelectObject(CLS_LINETUBE,pItem->m_idProp);	//ѡ���׼�ֹ�
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("m_fWeldHeight"))
		{	//���ݺ���߶����¼���ֹ�����ͷ wht 11-05-09
			CLDSLineTube *pLineTube=pTubeEndLjDlg->m_pCurLineTube;
			DESIGNODDMENT * pCurOddment=&(pLineTube->desEndOdd);
			if(pTubeEndLjDlg->m_bStartEnd)
				pCurOddment=&(pLineTube->desStartOdd);
			pCurOddment->m_fCollideDist=pTubeEndLjDlg->setupInfo.m_fWeldHeight+g_sysPara.VertexDist;
			BOOL bRetCode=TRUE;
			if(console.FromPartHandle(pCurOddment->m_hRefPart1)==NULL
				&&console.FromPartHandle(pCurOddment->m_hRefPart2)==NULL)
			{	//����ײ��׼�˼�ʱ��ʾ�û�ʹ�û�׼�ڵ㸸�˼�Ϊ��ײ��׼�˼� wht 11-05-11
				if(AfxMessageBox("δ������ײ���ո˼�,�޷�����ֹ�����ͷ,�Ƿ��Ի�׼�ڵ㸸�˼�Ϊ��ײ��׼�˼�?",MB_YESNO)==IDYES)
				{
					if(pTubeEndLjDlg->m_pBaseNode->hFatherPart!=pTubeEndLjDlg->m_pCurLineTube->handle)
						pCurOddment->m_hRefPart1=pTubeEndLjDlg->m_pBaseNode->hFatherPart;
				}
				else
					bRetCode=FALSE;
			}
			if(bRetCode)
			{	//������ײ��������ͷ,�˴����û������ť�����ʲ����ж��ж�����ͷ���㷽ʽ 
				pTubeEndLjDlg->setupInfo.m_fOddment=
					pLineTube->CalOddment(pTubeEndLjDlg->m_bStartEnd,&pTubeEndLjDlg->m_paramPlate,true);
				char sText[100]="";
				long idProp=CDesignTubeEndLjDlg::GetPropID("m_fOddment");
				if(pTubeEndLjDlg->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,sText);
			}
		}
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("m_fOddment"))
		{
			COddmentDesignDlg oddDlg;
			DESIGNODDMENT *pCurDesOddment=NULL;
			if(pTubeEndLjDlg->m_bStartEnd)
				pCurDesOddment=&pTubeEndLjDlg->m_pCurLineTube->desStartOdd;
			else
				pCurDesOddment=&pTubeEndLjDlg->m_pCurLineTube->desEndOdd;
			oddDlg.m_hRefPart1.Format("0X%X",pCurDesOddment->m_hRefPart1);
			oddDlg.m_hRefPart2.Format("0X%X",pCurDesOddment->m_hRefPart2);
			oddDlg.m_fCollideDist = pCurDesOddment->m_fCollideDist;
			oddDlg.m_iOddCalStyle = pCurDesOddment->m_iOddCalStyle;
			oddDlg.m_bDispSelectBtn=FALSE;	//
			if(oddDlg.DoModal()==IDOK)
			{
				char sText[100]="";
				_snprintf(sText,99,oddDlg.m_hRefPart1);
				sscanf(sText,"%X",&pCurDesOddment->m_hRefPart1);
				_snprintf(sText,99,oddDlg.m_hRefPart2);
				sscanf(sText,"%X",&pCurDesOddment->m_hRefPart2);
				pCurDesOddment->m_fCollideDist = oddDlg.m_fCollideDist;
				pCurDesOddment->m_iOddCalStyle = oddDlg.m_iOddCalStyle;
				if(pCurDesOddment->m_iOddCalStyle==0)
				{	//������ײ����ֹ�����ͷ wht 11-05-09
					pTubeEndLjDlg->setupInfo.m_fOddment=
						pTubeEndLjDlg->m_pCurLineTube->CalOddment(pTubeEndLjDlg->m_bStartEnd,&pTubeEndLjDlg->m_paramPlate,true);
					char valueStr[50]="";
					if(pTubeEndLjDlg->GetPropValueStr(pItem->m_idProp,valueStr)>0)
						pPropList->SetItemPropValue(pItem->m_idProp,CString(valueStr));
				}
			}
		}
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin"))
		{
			CReportPointDlg pointDlg;
			pointDlg.m_sCaption = "�ֹ��и����������";
			pointDlg.m_fPointX = pTubeEndLjDlg->cutPlaneOrigin.x;
			pointDlg.m_fPointY = pTubeEndLjDlg->cutPlaneOrigin.y;
			pointDlg.m_fPointZ = pTubeEndLjDlg->cutPlaneOrigin.z;
			if(pointDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",pointDlg.m_fPointZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.z"),CString(sText));
				pTubeEndLjDlg->cutPlaneOrigin.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
				pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ
			}
		}
		else if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm"))
		{
			CReportVectorDlg vecDlg;
			vecDlg.m_sCaption = "�ֹ��и��淨������";
			vecDlg.m_fVectorX = pTubeEndLjDlg->cutPlaneNorm.x;
			vecDlg.m_fVectorY = pTubeEndLjDlg->cutPlaneNorm.y;
			vecDlg.m_fVectorZ = pTubeEndLjDlg->cutPlaneNorm.z;
			pFindItem = pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x"),NULL);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y"),NULL);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z"),NULL);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z"),CString(sText));
				pTubeEndLjDlg->cutPlaneNorm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				pTubeEndLjDlg->UpdateTubeOddment();	//���¸ֹ�����ͷ
			}
		}
	}
	pTubeEndLjDlg->Invalidate(FALSE);
	g_pSolidDraw->Draw();
	return TRUE;
}
BOOL FirePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CDesignTubeEndLjDlg *pTubeEndLjDlg=(CDesignTubeEndLjDlg*)pPropList->GetParent();
	if(pTubeEndLjDlg==NULL)
		return FALSE;
	if(pItem->m_idProp==CDesignTubeEndLjDlg::GetPropID("LayoutBoltParam"))
	{
		pItem->m_lpNodeInfo->SetCheck(iMenu+1);
		if(iMenu==0)
		{	//�����˨���ò���
			int nBoltNum=pTubeEndLjDlg->LayoutBoltPara.m_nBoltNum;
			if(pTubeEndLjDlg->m_iTubeEndLjStyle==5&&pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout)
			{	//�ԳƲ���ʮ�ֲ����˨,��֤�������˨��Ϊż������wht 11-05-03
				if(nBoltNum%2!=0)
				{
					AfxMessageBox("�������˨������ż��������֧�ֶԳƲ�����˨!");
					return FALSE;
				}
				nBoltNum/=2;
			}
			CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA* pLayoutPara=NULL,*pMirLayoutPara=NULL;
			pLayoutPara=pTubeEndLjDlg->LayoutBoltParaList.append(pTubeEndLjDlg->LayoutBoltPara);
			if(pTubeEndLjDlg->m_iTubeEndLjStyle==5&&pTubeEndLjDlg->LayoutBoltPara.m_bMirLayout)
			{	//ʮ�ֲ�壬Ĭ�ϰ����в���
				double fColSpace=pTubeEndLjDlg->LayoutBoltPara.m_fColSpace;
				double fSpecialSpace=pTubeEndLjDlg->LayoutBoltPara.m_fSpecialSpace;
				pMirLayoutPara=pTubeEndLjDlg->LayoutBoltParaList.append(*pLayoutPara);
				pMirLayoutPara->m_posFirstBolt.x+=((nBoltNum-1)*fColSpace+fSpecialSpace);
			}
		}
		else	
		{	//������˨���ò���
			int iCurSel=pTubeEndLjDlg->m_listBoltParam.GetSelectedItem();
			CSuperGridCtrl::CTreeItem *pCurItem=pTubeEndLjDlg->m_listBoltParam.GetTreeItem(iCurSel);
			if(pCurItem==NULL)
				return FALSE;
			CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pSelLsPara=(CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA*)pCurItem->m_idProp;
			if(pSelLsPara==NULL)
			{
				AfxMessageBox("û��ѡ����˨���ò���������ʧ��!");
				return FALSE;
			}
			*pSelLsPara=pTubeEndLjDlg->LayoutBoltPara;
		}
		pTubeEndLjDlg->UpdateLayoutBoltPara();
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
// CDesignTubeEndLjDlg message handlers
IMPLEMENT_PROP_FUNC(CDesignTubeEndLjDlg);
const DWORD HASHTABLESIZE = 500;
void CDesignTubeEndLjDlg::InitPropHashtable()
{
	int i,id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
	//���β���
	AddPropItem("m_iPartType",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("m_iStdPartLevel",PROPLIST_ITEM(id++,"�����б�","�����Ӧ��Ӧ��ϵ����1��ʾӦ��ϵ��Ϊ100%,5��ʾӦ��ϵ��Ϊ%50��7��ʾӦ��ϵ��Ϊ70%��"));
	AddPropItem("m_iStdPartLevel2",PROPLIST_ITEM(id++,"�Խӷ����б�","�����Ӧ��Ӧ��ϵ����1��ʾӦ��ϵ��Ϊ100%,5��ʾӦ��ϵ��Ϊ%50��7��ʾӦ��ϵ��Ϊ70%��"));
	AddPropItem("m_cStdPartMat",PROPLIST_ITEM(id++,"ѡ�����","����ѡ�����(�ֹܲ���)",MakeMaterialMarkSetString()));
	AddPropItem("m_bCoupleFLCreate",PROPLIST_ITEM(id++,"������","�Ƿ���ƶԽӷ���","0.���෨��|1.�ɶ����"));
	AddPropItem("m_iWrapPlateType",PROPLIST_ITEM(id++,"��������","ʮ�ֲ���������","0.�ְ�|1.�Ǹ�"));
	AddPropItem("Profile",PROPLIST_ITEM(id++,"���β���","���β���"));
	for(i=0;i<26;i++)
	{
		AddPropItem(CXhChar16("%c",'A'+i),PROPLIST_ITEM(id++,"��������","��������"));
		AddPropItem(CXhChar16("%c",'a'+i),PROPLIST_ITEM(id++,"��������","��������"));
	}
	//ƽ���и�
	AddPropItem("m_hWeldLinkObj",PROPLIST_ITEM(id++,"��ͷ���ӹ���","��ֹ��и����ֱ�Ӻ��ӵĹ���"));
	AddPropItem("cutPlaneOrigin",PROPLIST_ITEM(id++,"�и������","�ֹ��и����������"));
	AddPropItem("cutPlaneOrigin.x",PROPLIST_ITEM(id++,"X����","�ֹ��и������X�᷽������"));
	AddPropItem("cutPlaneOrigin.y",PROPLIST_ITEM(id++,"Y����","�ֹ��и������Y�᷽������"));
	AddPropItem("cutPlaneOrigin.z",PROPLIST_ITEM(id++,"Z����","�ֹ��и������Z�᷽������"));
	AddPropItem("cutPlaneNorm",PROPLIST_ITEM(id++,"�и��淨��","�ֹ��и��淨�߷���"));
	AddPropItem("cutPlaneNorm.x",PROPLIST_ITEM(id++,"X����","�ֹ��и��淨�߷���X�᷽������"));
	AddPropItem("cutPlaneNorm.y",PROPLIST_ITEM(id++,"Y����","�ֹ��и��淨�߷���Y�᷽������"));
	AddPropItem("cutPlaneNorm.z",PROPLIST_ITEM(id++,"Z����","�ֹ��и��淨�߷���Z�᷽������"));
	//�ֹ����
	AddPropItem("TubeTransect",PROPLIST_ITEM(id++,"�ֹ����","�ֹ����"));
	AddPropItem("m_bFaceToFace",PROPLIST_ITEM(id++,"�Խ����","�Խ����","��|��"));
	AddPropItem("m_hDatumTube1",PROPLIST_ITEM(id++,"���ֹ�1","���ֹ�1"));
	AddPropItem("m_hDatumTube2",PROPLIST_ITEM(id++,"���ֹ�2","���ֹ�2"));
	//������Ϣ
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"������Ϣ","������Ϣ"));
	AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������","�������"));
	AddPropItem("sCodeName",PROPLIST_ITEM(id++,"������",""));
	AddPropItem("sCoupleFlPlateNo",PROPLIST_ITEM(id++,"�Խӷ������","�Խӷ������"));
	AddPropItem("sWeldPlatePartNo",PROPLIST_ITEM(id++,"������","������"));
	AddPropItem("sWrapPlatePartNo",PROPLIST_ITEM(id++,"������","������"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"�κ�"));
	AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����","����",MakeMaterialMarkSetString()));
	AddPropItem("thick",PROPLIST_ITEM(id++,"���","�����װ��ֹܲ����,mm"));
	AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��","ͳ��ϵ��"));
	//�ֹܶ����Ӳ���
	AddPropItem("TubeEndPos",PROPLIST_ITEM(id++,"�ֹܶ�����λ��"));
	AddPropItem("m_fWeldHeight",PROPLIST_ITEM(id++,"����߶�","����߶�"));
	AddPropItem("m_fNormOffset",PROPLIST_ITEM(id++,"�����淨��ƫ����","�����淨��ƫ����"));
	AddPropItem("m_fOddment",PROPLIST_ITEM(id++,"����ͷ","�ֹ�ʼ������ͷ,������Ϊ����ʱ�ֹ�����ͷ=�ֹܳ�ʼ������ͷ-�����߳��ֹܲ��ֵĸ߶�"));
	AddPropItem("m_fTubeNormOffset",PROPLIST_ITEM(id++,"����ƫ����","�ֹܷ���ƫ����"));
	AddPropItem("m_fTubeEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�","�ֹ�ƫ�ľ�"));
	//װ�����
	AddPropItem("SetUpPara",PROPLIST_ITEM(id++,"װ�����","װ�����"));
	AddPropItem("H1",PROPLIST_ITEM(id++,"H1","ƽ�������ֹܶ��뷨������ľ���"));
	AddPropItem("MotherPlateNorm",PROPLIST_ITEM(id++,"���幤������","���幤��������㷽ʽ",CStdVector::MakeVectorSetStr()));
	AddPropItem("MotherPlateNorm.x",PROPLIST_ITEM(id++,"X�������","���巨��X�������"));
	AddPropItem("MotherPlateNorm.y",PROPLIST_ITEM(id++,"Y�������","���巨��Y�������"));
	AddPropItem("MotherPlateNorm.z",PROPLIST_ITEM(id++,"Z�������","���巨��Z�������"));
	AddPropItem("UpVertPlateNorm",PROPLIST_ITEM(id++,"1�����幤������","��ֱ�ڻ����1�����幤��������㷽ʽ",CStdVector::MakeVectorSetStr()));
	AddPropItem("UpVertPlateNorm.x",PROPLIST_ITEM(id++,"X�������","1�����巨��X�������"));
	AddPropItem("UpVertPlateNorm.y",PROPLIST_ITEM(id++,"Y�������","1�����巨��Y�������"));
	AddPropItem("UpVertPlateNorm.z",PROPLIST_ITEM(id++,"Z�������","1�����巨��Z�������"));
	AddPropItem("DownVertPlateNorm",PROPLIST_ITEM(id++,"2�����幤������","��ֱ�ڻ����2�����幤��������㷽ʽ",CStdVector::MakeVectorSetStr()));
	AddPropItem("DownVertPlateNorm.x",PROPLIST_ITEM(id++,"X�������","2�����巨��X�������"));
	AddPropItem("DownVertPlateNorm.y",PROPLIST_ITEM(id++,"Y�������","2�����巨��Y�������"));
	AddPropItem("DownVertPlateNorm.z",PROPLIST_ITEM(id++,"Z�������","2�����巨��Z�������"));
	//������Ʋ���
	AddPropItem("design_axis_z",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.norm_style",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.hVicePart",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.hCrossPart",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.norm_wing",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.direction",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.rot_ang",PROPLIST_ITEM(id++,"��������","��������"));
	//AddPropItem("design_axis_z.VectorType",PROPLIST_ITEM(id++,"��������","��������"));
	//AddPropItem("design_axis_z.vector.x",PROPLIST_ITEM(id++,"��������","��������"));
	//AddPropItem("design_axis_z.vector.y",PROPLIST_ITEM(id++,"��������","��������"));
	//AddPropItem("design_axis_z.vector.z",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.nearVectorType",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.nearVector.x",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.nearVector.y",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("design_axis_z.nearVector.z",PROPLIST_ITEM(id++,"��������","��������"));
	//��˨���ò���
	AddPropItem("LayoutBoltParam",PROPLIST_ITEM(id++,"��˨������Ϣ","","�����˨����|������˨����"));
	AddPropItem("m_nBoltD",PROPLIST_ITEM(id++,"��˨ֱ��","��˨ֱ��","12|16|20|24|27|30|"));
	AddPropItem("m_iDriection",PROPLIST_ITEM(id++,"��˨����","��˨����","0.����|1.����"));
	AddPropItem("m_fTubeR",PROPLIST_ITEM(id++,"�ֹܰ뾶","�ֹܰ뾶"));
	AddPropItem("m_iStartEdge",PROPLIST_ITEM(id++,"��ʼ��","���÷�����˨ʱ����ʼ��,�Ҳ�ʾ��ͼ��,��ɫ���Ǳ�־λ��׼��λ�ã���ɫ���Ǳ�עΪ������λ��","0.��׼��|1.������"));
	AddPropItem("m_fOffsetAngle",PROPLIST_ITEM(id++,"����˨ƫ�ƽ�","����˨�����ʼ�ߵ�ƫ�ƽ�"));
	AddPropItem("m_fBoltLayoutD",PROPLIST_ITEM(id++,"����Բֱ��","��˨����Բֱ��D"));
	AddPropItem("m_nBoltNum",PROPLIST_ITEM(id++,"��˨����","��˨����"));
	AddPropItem("m_bStaggerLayout",PROPLIST_ITEM(id++,"���沼��","����˫����˨ʱ������ˮƽ������ֱ����˨","��|��"));
	AddPropItem("m_bMirLayout",PROPLIST_ITEM(id++,"�ԳƲ���","�ԳƲ���������������˨","��|��"));
	AddPropItem("m_fSpecialSpace",PROPLIST_ITEM(id++,"������","������"));
	AddPropItem("m_nRowNum",PROPLIST_ITEM(id++,"��˨����","��˨����"));
	AddPropItem("m_nColNum",PROPLIST_ITEM(id++,"��˨����","��˨����"));
	AddPropItem("m_fRowSpace",PROPLIST_ITEM(id++,"��˨�о�","��˨�о�"));
	AddPropItem("m_fColSpace",PROPLIST_ITEM(id++,"��˨�о�","��˨�о�"));
	AddPropItem("m_posFirstBolt.x",PROPLIST_ITEM(id++,"����˨X����","����˨X����"));
	AddPropItem("m_posFirstBolt.y",PROPLIST_ITEM(id++,"����˨Y����","����˨Y����"));
	//���Ӱ����
	AddPropItem("m_bCreateAttachPlate",PROPLIST_ITEM(id++,"�����߰�","�Ƿ������߰�","��|��"));
	AddPropItem("AttachPlateProfile",PROPLIST_ITEM(id++,"�߰����β���","�߰����β���"));
	AddPropItem("AttachPlatePara.A",PROPLIST_ITEM(id++,"��ֱ�߶�A"));
	AddPropItem("AttachPlatePara.B",PROPLIST_ITEM(id++,"�ױ߿��B"));
	AddPropItem("AttachPlatePara.C",PROPLIST_ITEM(id++,"̨�׸߶�C"));
	AddPropItem("AttachPlatePara.D",PROPLIST_ITEM(id++,"�������D"));
	AddPropItem("AttachPlatePara.E",PROPLIST_ITEM(id++,"�нǿ��E"));
	AddPropItem("AttachPlatePara.F",PROPLIST_ITEM(id++,"�׾���϶F"));
	AddPropItem("AttachPlatePara.G",PROPLIST_ITEM(id++,"�׾���϶G"));
	AddPropItem("AttachPlatePara.H",PROPLIST_ITEM(id++,"�׾�H"));
	AddPropItem("AttachPlatePara.m_iCutMode",PROPLIST_ITEM(id++,"����ģʽ","","0.�н�|1.�л�"));
	AddPropItem("AttachPlatePara.m_nThick",PROPLIST_ITEM(id++,"���","�߰���"));
	AddPropItem("AttachPlateBasicInfo",PROPLIST_ITEM(id++,"�߰������Ϣ","�߰������Ϣ"));
	AddPropItem("AttachPlatePara.cMaterial",PROPLIST_ITEM(id++,"����","�߰����",MakeMaterialMarkSetString()));
	AddPropItem("AttachPlatePara.sPartNo",PROPLIST_ITEM(id++,"���","�߰���"));
	AddPropItem("AttachPlatePara.m_fAngle",PROPLIST_ITEM(id++,"ƫ�ƽǶ�","�����߰��׿��߰�����ڻ�׼�ߵ�ƫ�ƽǶ�"));
	AddPropItem("AttachPlatePara.m_nPlateNum",PROPLIST_ITEM(id++,"�߰����","�߰����"));
	AddPropItem("AttachPlatePara.m_bUseAngle",PROPLIST_ITEM(id++,"�����ǶȲ�����","�����ǶȲ�����","��|��"));
	AddPropItem("AttachPlatePara.m_bMirCreate",PROPLIST_ITEM(id++,"�Գ�����","�Գ��������Բ����һ����߰�","��|��"));
	//������
	AddPropItem("m_bCreateFillPlate",PROPLIST_ITEM(id++,"���ɵ��","��ʮ�ֲ�������Ľڵ���ȴ��ڲ������ȣ���Ҫ���ɵ��","��|��"));
	AddPropItem("FillPlateBasicInfo",PROPLIST_ITEM(id++,"������","������"));
	AddPropItem("FillPlatePara.cMat",PROPLIST_ITEM(id++,"����","������",MakeMaterialMarkSetString()));
	AddPropItem("FillPlatePara.sPartNo",PROPLIST_ITEM(id++,"���","�������"));
	AddPropItem("FillPlatePara.nThick",PROPLIST_ITEM(id++,"���","�����"));
	AddPropItem("FillPlatePara.nNodePlateThick",PROPLIST_ITEM(id++,"���","��ʮ�ֲ�������Ľڵ����"));
	//�ڵ�����
	AddPropItem("NodePlateBasicInfo",PROPLIST_ITEM(id++,"��������","��������"));
	AddPropItem("NodePlatePara.cMaterial",PROPLIST_ITEM(id++,"����","�ڵ�����"));
	AddPropItem("NodePlatePara.sPartNo",PROPLIST_ITEM(id++,"���","�ڵ����"));
	AddPropItem("NodePlatePara.m_nThick",PROPLIST_ITEM(id++,"���","�ڵ����"));
	AddPropItem("NodePlatePara.m_nRollHeight",PROPLIST_ITEM(id++,"��������","��������"));
}
int CDesignTubeEndLjDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bEnableWindowMoveListen=FALSE;
	if (CCallBackDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
BOOL CDesignTubeEndLjDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_pCurLineTube==NULL||m_pBaseNode==NULL)
		return FALSE;
	//��ʼ��������
	InitPropHashtable();	//��ʼ�����Թ�ϣ��
	m_propList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xSubPropList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	//
	m_listBoltParam.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_listBoltParam.SetDblclkDisplayCellCtrl(TRUE);	//����Ϊ˫����ʾ�ɱ༭�ؼ�
	m_listBoltParam.SetModifyValueFunc(FireModifyValue);
	m_listBoltParam.SetItemChangedFunc(FireItemChanged);
	m_listBoltParam.SetDeleteItemFunc(FireDeleteItem);
	m_listBoltParam.SetImageWidth(0);	//������
	//
	m_paramPlate.SetBelongModel(console.GetActiveModel());	
	if(work_norm.IsZero())	//δ���ù����淨��ʱĬ�Ϲ����淨��Ϊ�ֹܵĹ����淨��
		work_norm=m_pCurLineTube->WorkPlaneNorm();
	TUBEJOINT *pCurJoint=NULL;
	DESIGNODDMENT* pCurOddment=NULL;
	if(m_pCurLineTube->pStart==m_pBaseNode)
	{
		m_bStartEnd=TRUE;
		m_pRelatedTube=FindTubeInLine(m_pCurLineTube,m_bStartEnd);
		setupInfo.m_fTubeEccentricDist=m_pCurLineTube->desStartPos.fEccentricDist;
		setupInfo.m_fTubeNormOffset=m_pCurLineTube->desStartPos.fNormOffset;
		setupInfo.m_fOddment=m_pCurLineTube->startOdd(true);
		pCurJoint=&m_pCurLineTube->m_tJointStart;
		pCurOddment=&m_pCurLineTube->desStartOdd;
	}
	else 
	{
		m_bStartEnd=FALSE;
		m_pRelatedTube=FindTubeInLine(m_pCurLineTube,m_bStartEnd);
		setupInfo.m_fTubeEccentricDist=m_pCurLineTube->desEndPos.fEccentricDist;
		setupInfo.m_fTubeNormOffset=m_pCurLineTube->desEndPos.fNormOffset;
		setupInfo.m_fOddment=m_pCurLineTube->endOdd(true);
		pCurJoint=&m_pCurLineTube->m_tJointEnd;
		pCurOddment=&m_pCurLineTube->desEndOdd;
	}
	if(!m_bInernalStart)
	{	//�жϵ�ǰ��ƶ��Ƿ��Ѵ��ڶ�����
		m_pParamPlate=NULL;
		m_bHasExistEndLj=FALSE;
		m_bCreateFillPlate=FALSE;
		m_bFaceToFace=FALSE;
		if(pCurJoint->hViceLinkObj<0)
			m_bFaceToFace=TRUE;
		cutPlaneNorm=pCurJoint->cutPlaneNorm;
		cutPlaneOrigin=pCurJoint->cutPlanePos;
		m_hWeldLinkObj=(pCurJoint->type==0)?pCurJoint->hLinkObj:0;	//��ǰ���Ӷ�Ϊƽ���и�ʱ��ֵ����
		m_hDatumTube1=pCurJoint->hLinkObj;
		if(pCurJoint->hViceLinkObj>0)
			m_hDatumTube2=pCurJoint->hViceLinkObj;
		if(pCurJoint->hLinkObj>0)
		{
			if(pCurJoint->type>1)
				m_pParamPlate=(CLDSParamPlate*)console.FromPartHandle(pCurJoint->hLinkObj,CLS_PARAMPLATE);
			else 
				m_bHasExistEndLj=TRUE;	
			if(m_pParamPlate)
				m_bHasExistEndLj=TRUE;
		}
		if(m_bHasExistEndLj)
			m_iTubeEndLjStyle=pCurJoint->type;	//����������
		UpdateDesignParam();	//��ʼ����׼������Ʋ���
		if(m_iTubeEndLjStyle==5)	//ʮ�ֲ��
		{
			setupInfo.workNorm.norm_style=5;
			setupInfo.workNorm.rot_ang=0;
			setupInfo.workNorm.hVicePart=m_pCurLineTube->handle;
			setupInfo.workNorm.UpdateVector(console.GetActiveModel());
			setupInfo.workNormUpVert.norm_style=setupInfo.workNormDownVert.norm_style=4;
			setupInfo.workNormUpVert.nearVector.Set(0,-1);
			setupInfo.workNormDownVert.nearVector.Set(0,1);
			setupInfo.workNormUpVert.vector=setupInfo.workNormUpVert.nearVector;
			setupInfo.workNormDownVert.vector=setupInfo.workNormDownVert.nearVector;
			if(m_pCurLineTube->pStart==m_pBaseNode)
				m_paramPlate.ucs.axis_x=m_pCurLineTube->Start()-m_pCurLineTube->End();
			else
				m_paramPlate.ucs.axis_x=m_pCurLineTube->End()-m_pCurLineTube->Start();
			m_paramPlate.ucs.axis_z=setupInfo.workNorm.vector;
			m_paramPlate.ucs.axis_y=m_paramPlate.ucs.axis_z^m_paramPlate.ucs.axis_x;
			normalize(m_paramPlate.ucs.axis_y);
			m_paramPlate.ucs.axis_x=m_paramPlate.ucs.axis_y^m_paramPlate.ucs.axis_z;
			normalize(m_paramPlate.ucs.axis_x);
			vector_trans(setupInfo.workNormUpVert.vector,m_paramPlate.ucs,TRUE);
			vector_trans(setupInfo.workNormDownVert.vector,m_paramPlate.ucs,TRUE);
		}
		if(m_iTubeEndLjStyle>=3&&m_iTubeEndLjStyle<=5&&
			pCurOddment->m_iOddCalStyle==0&&!IsHasExistEndLj())
		{
			if(pCurOddment->m_hRefPart1==0&&m_pCurLineTube->handle!=m_pBaseNode->hFatherPart)
				pCurOddment->m_hRefPart1=m_pBaseNode->hFatherPart;
			pCurOddment->m_fCollideDist=setupInfo.m_fWeldHeight+g_sysPara.VertexDist;
			setupInfo.m_fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate,true);
		}
		//�Ѵ��ڶ������Ҷ�����Ϊʮ�ֲ��ʱ�������ʮ�ֲ������Ľڵ��
		//����ʮ�ֲ�帽���߰�ʱ���߰�ĺ��Ӹ�����Ϊʮ�ֲ������Ľڵ�� wht 10-08-02
		if(m_pParamPlate&&m_pParamPlate->m_iParamType==TYPE_XEND)
		{
			CLDSPlate *pWeldPlate=NULL;
			CLDSModule *pActiveModule=Ta.GetActiveModule();
			for(pWeldPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pWeldPlate;pWeldPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
			{	
				if(pActiveModule&&!pActiveModule->IsSonPart(pWeldPlate))
					continue;	//�ǵ�ǰģ���еĹ��� 
				CDesignLjPartPara *pLjPara=pWeldPlate->designInfo.FromHandle(m_pCurLineTube->handle);
				if(pLjPara==NULL)
					continue;
				if((m_bStartEnd&&pLjPara->start0_end1!=0)||
					(!m_bStartEnd&&pLjPara->start0_end1!=1))
					continue;
				AttachPlatePara.m_pWeldParentPlate=pWeldPlate;
				break;
			}
		}
		UpdateLayoutBoltPara();
		DisplayPropertyList();
		RefreshCtrlState();
	}
	else
	{	//�ڲ����������ѡ�����ĺ�������
		FinishSelectObjOper();
		DisplayPropertyList();
		CPropTreeItem *pItem=m_propList.FindItemByPropId(m_idEventProp,NULL);
		if(pItem)
		{
			m_propList.SetFocus();	//��������ý���
			m_propList.SetCurSel(pItem->m_iIndex);	//����ѡ����
		}
		else
		{
			RefreshCtrlState(FALSE);
			pItem=m_xSubPropList.FindItemByPropId(m_idEventProp,NULL);
			m_xSubPropList.SetFocus();
			m_xSubPropList.SetCurSel(pItem->m_iIndex);
			DisplayChildPropertyList(NULL);
			if(m_xSubPropList.m_nObjClassTypeId==0)
			{
				pItem=m_propList.FindItemByPropId(GetPropID("MotherPlateNorm"),NULL);
				if(pItem)
					m_propList.SetCurSel(pItem->m_iIndex);
			}
			else if(m_xSubPropList.m_nObjClassTypeId==1)
			{
				pItem=m_propList.FindItemByPropId(GetPropID("UpVertPlateNorm"),NULL);
				if(pItem)
					m_propList.SetCurSel(pItem->m_iIndex);
			}
			else if(m_xSubPropList.m_nObjClassTypeId==1)
			{
				pItem=m_propList.FindItemByPropId(GetPropID("DownVertPlateNorm"),NULL);
				if(pItem)
					m_propList.SetCurSel(pItem->m_iIndex);
			}
		}
	}
	UpdateSketchMap();
	//���ɻ���
	if(solid_pen.GetSafeHandle()==0)
		solid_pen.CreatePen(PS_SOLID,1,RGB(0,0,200));
	if(dash_pen.GetSafeHandle()==0)
		dash_pen.CreatePen(PS_DASH,1,RGB(0,0,0));
	if(dash_dot_pen.GetSafeHandle()==0)
		dash_dot_pen.CreatePen(PS_DASHDOT,1,RGB(255,0,0));
	if(green_solid_pen.GetSafeHandle()==0)
		green_solid_pen.CreatePen(PS_SOLID,2,RGB(0,255,0));
	if(green_dash_pen.GetSafeHandle()==0)
		green_dash_pen.CreatePen(PS_DASH,1,RGB(0,255,0));
	if(red_solid_pen.GetSafeHandle()==0)
		red_solid_pen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	if(blue_solid_pen.GetSafeHandle()==0)
		blue_solid_pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	if(purple_solid_pen.GetSafeHandle()==0)	//��ɫ����
		purple_solid_pen.CreatePen(PS_SOLID,1,RGB(128,0,255));
	//�ƶ��Ի������Ͻ�
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
	return TRUE;  
}
INSERT_PLATE_PARAM* CDesignTubeEndLjDlg::QueryStdInsertPlate(int iLevel,char cMat)
{
	if(m_iTubeEndLjStyle==3)
		return FindUEndParam(m_pCurLineTube->GetDiameter(),m_pCurLineTube->GetThick(),cMat,iLevel);
	else if(m_iTubeEndLjStyle==4)
		return FindRollEndParam(m_pCurLineTube->GetDiameter(),m_pCurLineTube->GetThick(),cMat,iLevel);
	else if(m_iTubeEndLjStyle==5)
		return FindXEndParam(m_pCurLineTube->GetDiameter(),m_pCurLineTube->GetThick(),cMat,iLevel);
	else
		return NULL;
}
INSERT_PLATE_PARAM* CDesignTubeEndLjDlg::QueryStdInsertPlateByMat(char cMat,int* pLevelArr/*=NULL*/)
{
	INSERT_PLATE_PARAM* pInsertPlateParam=NULL;
	int index=0;
	for(int i=1;i<=9;i++)
	{	
		INSERT_PLATE_PARAM *pChaBanParam=QueryStdInsertPlate(i,cMat);
		if(pChaBanParam==NULL)
			continue;
		if(pLevelArr)
			pLevelArr[index++]=pChaBanParam->iLevelMark;
		if(pInsertPlateParam==NULL||i>pInsertPlateParam->iLevelMark)
			pInsertPlateParam=pChaBanParam;	//������С������(����С��
	}
	return pInsertPlateParam;
}
FL_PARAM* CDesignTubeEndLjDlg::QueryStdFlDParaByMat(char cMat,int* pLevelArr/*=NULL*/)
{
	FL_PARAM* pFlDPara=NULL;
	double tube_d_m=m_pCurLineTube->GetDiameter();
	double tube_d_c=m_pRelatedTube?m_pRelatedTube->GetDiameter():tube_d_m;
	if(tube_d_m>tube_d_c)
		tube_d_c=tube_d_m;
	int index=0;
	for(int i=1;i<9;i++)
	{
		char cLevel='0'+i;
		FL_PARAM* pFindFld=FindFlDParam(tube_d_m,tube_d_c,cMat,cLevel);
		if(pFindFld==NULL)
			continue;
		if(pLevelArr)
			pLevelArr[index++]=i;
		if(pFlDPara==NULL||pFindFld->cLevelMark>pFlDPara->cLevelMark)
			pFlDPara=pFindFld;
	}
	return pFlDPara;
}
//��ʼ����׼������Ʋ��� 
void CDesignTubeEndLjDlg::InitStdPartDesignParam()
{
	m_pFlDParam=m_pFlPParam=NULL;
	m_pFlDParam2=m_pFlPParam2=NULL;
	m_pRigidFlParam=m_pFlexibleFlParam=NULL;
	m_pRigidFlParam2=m_pFlexibleFlParam2=NULL;
	m_pInsertPlateParam=NULL;
	m_arrStdPartLevel.RemoveAll();
	m_arrStdFlDLevelM.RemoveAll();
	m_arrStdFlDLevelC.RemoveAll();
	m_bCoupleFLCreate=true;
	if(m_iTubeEndLjStyle==2)
	{	//����
		double tube_d=m_pCurLineTube->GetDiameter();
		double couple_d=m_pRelatedTube?m_pRelatedTube->GetDiameter():tube_d;
		FL_COUPLE couple;
		if(QueryFlCouple(tube_d,couple_d,'D',&couple))
		{
			m_pFlDParam=FindFlDParam(couple.mainFlName);
			m_pFlDParam2=FindFlDParam(couple.coupleFlName);
		}
		if(QueryFlCouple(tube_d,couple_d,'P',&couple))
		{	
			m_pFlPParam=FindFlPParam(couple.mainFlName);
			m_pFlPParam2=FindFlPParam(couple.coupleFlName);
		}
		if(QueryFlCouple(tube_d,couple_d,'G',&couple))
		{	
			m_pRigidFlParam=FindFlGParam(couple.mainFlName);
			m_pRigidFlParam2=FindFlGParam(couple.coupleFlName);
		}
		if(QueryFlCouple(tube_d,couple_d,'R',&couple))
		{	
			m_pFlexibleFlParam=FindFlRParam(couple.mainFlName);
			m_pFlexibleFlParam2=FindFlRParam(couple.coupleFlName);
		}
		if(m_pFlDParam!=NULL)
			m_iPartType=2;
		else if(m_pFlPParam!=NULL)
			m_iPartType=1;
		else if(m_pRigidFlParam!=NULL)
			m_iPartType=3;
		else if(m_pFlexibleFlParam!=NULL)
			m_iPartType=4;
		else if(m_pFlPParam==NULL&&m_pFlDParam==NULL&&m_pRigidFlParam==NULL&&m_pFlexibleFlParam==NULL)
			m_iPartType=0;	//δ�ҵ������׼����ʱ �������ͳ�ʼ��Ϊ0
		//��ƶԺ�����ʱ����ȡ�Ժ�����
		if(m_iPartType==2)
		{	
			double tube_d_m=0,tube_d_c=0;
			//ͳ�����ܶԺ������ļ���
			m_cFlLevel1=m_pFlDParam->cLevelMark;
			m_cStdPartMat=m_pFlDParam->cMaterial;
			tube_d_m=m_pCurLineTube->GetDiameter();
			tube_d_c=m_pRelatedTube?m_pRelatedTube->GetDiameter():tube_d_m;
			if(tube_d_m>tube_d_c)
				tube_d_c=tube_d_m;
			for(int i=1;i<9;i++)
			{
				char cLevel='0'+i;
				if(FindFlDParam(tube_d_m,tube_d_c,m_cStdPartMat,cLevel))
					m_arrStdFlDLevelM.Add(CXhChar16("%C",cLevel));
			}
			//ͳ�ƶԽӶԺ������ļ���
			if(m_pRelatedTube && m_pFlDParam2)
			{
				m_cFlLevel2=m_pFlDParam2->cLevelMark;
				tube_d_m=m_pRelatedTube->GetDiameter();
				tube_d_c=m_pCurLineTube->GetDiameter();
				if(tube_d_m>tube_d_c)
					tube_d_c=tube_d_m;
				for(int i=1;i<9;i++)
				{
					char cLevel='0'+i;
					if(FindFlDParam(tube_d_m,tube_d_c,cLevel))
						m_arrStdFlDLevelC.Add(CXhChar16("%C",cLevel));
				}
			}
		}
	}
	else if(m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4||m_iTubeEndLjStyle==5)
	{
		int xLevelArr[9]={0};
		m_pInsertPlateParam=QueryStdInsertPlateByMat(m_pCurLineTube->cMaterial,xLevelArr);
		if(m_pInsertPlateParam)	
		{	//��ǰ��׼��������
			m_iPartType=1;
			m_iStdPartLevel=m_pInsertPlateParam->iLevelMark;
			m_cStdPartMat=m_pInsertPlateParam->cTubeMat;	//���ұ�׼����������Ǹֹܲ���
			//ͳ�Ʋ�弶��
			for(int i=0;i<9;i++)
			{
				if(xLevelArr[i]==0)
					break;
				m_arrStdPartLevel.Add(CXhChar16("%d",xLevelArr[i]));
			}
		}
		else
			m_iPartType=0;	//δ�ҵ������׼����ʱ �������ͳ�ʼ��Ϊ0
	}
	if(m_pParamPlate&&m_bHasExistEndLj)
	{	//�Ѵ��ڶ�����,���±�׼��������
		m_bCoupleFLCreate=false;
		CLDSStdPartModel stdPartModel;
		if(m_pParamPlate->GetStdPartModel(&stdPartModel))
		{
			if(m_pParamPlate->m_iParamType==TYPE_FLG)
			{
				m_iPartType=3;
				m_pRigidFlParam=FindFlGParam(m_pParamPlate->GetPartNo());
			}
			else if(m_pParamPlate->m_iParamType==TYPE_FLR) 
			{
				m_iPartType=4;
				m_pFlexibleFlParam=FindFlRParam(m_pParamPlate->GetPartNo());
			}
			else if(m_pParamPlate->m_iParamType==TYPE_FLD)
			{
				m_iPartType=2;
				m_cFlLevel1=stdPartModel.param.fl_param.cLevelMark;
				m_pFlDParam=FindFlDParam(m_pParamPlate->GetPartNo());
			}
			else if(m_pParamPlate->m_iParamType==TYPE_FLP)
			{
				m_iPartType=1;
				m_pFlPParam=FindFlPParam(m_pParamPlate->GetPartNo());
			}
			else
			{	
				m_iPartType=1;
				m_iStdPartLevel=stdPartModel.param.insert_plate_param.iLevelMark;
				m_cStdPartMat=stdPartModel.param.insert_plate_param.cTubeMat;	//���ұ�׼����������Ǹֹܲ���
				if(m_pParamPlate->m_iParamType==TYPE_ROLLEND)
					m_pInsertPlateParam=FindRollEndParam(m_pParamPlate->GetPartNo());
				else if(m_pParamPlate->m_iParamType==TYPE_UEND)
					m_pInsertPlateParam=FindUEndParam(m_pParamPlate->GetPartNo());
				else if(m_pParamPlate->m_iParamType==TYPE_XEND)
					m_pInsertPlateParam=FindXEndParam(m_pParamPlate->GetPartNo());
			}
		}
		else
			m_iPartType=0;
	}
}

//��ʼ����Ʋ���
void CDesignTubeEndLjDlg::UpdateDesignParam(BOOL bInitPartType/*=TRUE*/)
{
	CLogErrorLife loglife;
	if(bInitPartType)
		InitStdPartDesignParam();
	//��ʼ����幤��ƽ����ת�Ƕȣ�Ӧ����ǰ�߳�ʼ�� wht 10-11-09 
	setupInfo.workNorm.rot_ang=0;
	if(m_iTubeEndLjStyle==4&&!m_bSpecialFlRayPole)
	{	//��֤���Ͳ���߷����� wht 10-10-21
		if(work_norm*m_pCurLineTube->WorkPlaneNorm()>0)
			setupInfo.workNorm.rot_ang=180;	//�����淨�߷�����ֹܻ�׼�淨�߷�����ͬ
		else 
			setupInfo.workNorm.rot_ang=0;		//�෴
	}
	//��ʼ�����β��� wht 10-08-02
	if(m_bHasExistEndLj&&m_pParamPlate)	//�Ѵ��ڶ�����
		UpdateDesignParamBySrcPlate(m_pParamPlate);
	else
	{
		double A=10,R=10,W=40,H=0,D=0,L=520,M=260,N=40,S=20,T=10;
		double fTubeD = m_pCurLineTube->GetDiameter();
		m_paramPlate.m_bStdPart=FALSE;	
		m_paramPlate.iSeg=m_pCurLineTube->iSeg;
		m_paramPlate.cMaterial=m_pCurLineTube->cMaterial;
		m_paramPlate.thick=(long)T;
		m_paramPlate.SetPartNo("");
		strcpy(sCoupleFlPlateNo,"");
		m_paramPlate.EmptyDesignItems();//�����Ʋ���		
		//
		LayoutBoltParaList.Empty();
		LayoutBoltPara.m_iStartEdge=m_pCurLineTube->m_bHasWeldRoad?1:0;
		LayoutBoltPara.m_fOffsetAngle=0;
		LayoutBoltPara.m_iDirection=0;
		if(m_bDesignNodePlate)
		{
			NodePlatePara.cMaterial=m_pCurLineTube->cMaterial;
			NodePlatePara.m_nThick=m_paramPlate.thick;
		}
		if(m_iTubeEndLjStyle==2)	//����
		{
			if(m_iPartType==0)
			{	//��ͨ����
				m_paramPlate.m_iParamType=TYPE_FL;
				H = 0.5*T;
				W=fTubeD+80;
				R=fTubeD*0.5+1;	//�����ڰ뾶
				//��˨��Ϣ
				if(m_pRigidFlParam)
				{	//���Է�����ʵ���ǲ��ֱ�׼������ͨ���߷��������Կ�����ΪĬ��ֵ wjh-2017.12.28
					W=m_pRigidFlParam->fOuterD;
					LayoutBoltPara.m_fBoltLayoutD=m_pRigidFlParam->fBoltCirD;
					LayoutBoltPara.m_nBoltD=m_pRigidFlParam->nBoltD;
					LayoutBoltPara.m_nBoltNum=m_pRigidFlParam->nBoltNum;
				}
				else
				{
					LayoutBoltPara.m_fBoltLayoutD=fTubeD+20;
					LayoutBoltPara.m_nBoltD=20;
					LayoutBoltPara.m_nBoltNum=8;
				}
				//�����߰���Ϣ
				AttachPlatePara.cMaterial=m_pCurLineTube->cMaterial;
				AttachPlatePara.m_nThick=m_paramPlate.thick;
				AttachPlatePara.m_nPlateNum=LayoutBoltPara.m_nBoltNum;
				AttachPlatePara.A=150;
				AttachPlatePara.B=0.5*W-R;
				AttachPlatePara.C=30;
				AttachPlatePara.D=30;
				AttachPlatePara.E=25;
				AttachPlatePara.F=30;
				AttachPlatePara.G=20;
				AttachPlatePara.H=0;
				AttachPlatePara.m_iCutMode=1;
				AttachPlatePara.m_fAngle=LayoutBoltPara.m_fOffsetAngle+180.0/LayoutBoltPara.m_nBoltNum;
			}
			else if(m_iPartType==1&&m_pFlPParam)
			{
				m_paramPlate.m_iParamType=TYPE_FLP;
				m_paramPlate.SetPartNo(m_pFlPParam->codeName);
				if(m_pFlPParam2)
					strcpy(sCoupleFlPlateNo,m_pFlPParam2->codeName);
				m_paramPlate.m_bStdPart=true;
				m_paramPlate.thick=(long)m_pFlPParam->C;
				m_paramPlate.cMaterial=m_pFlPParam->cMaterial;
				R=m_pFlPParam->B*0.5;
				W=m_pFlPParam->D;
				LayoutBoltPara.m_fBoltLayoutD=m_pFlPParam->K;
				LayoutBoltPara.m_nBoltD=m_pFlPParam->nBoltD;
				LayoutBoltPara.m_nBoltNum=m_pFlPParam->nBoltNum;
			}
			else if(m_iPartType==2&&m_pFlDParam)
			{
				m_paramPlate.m_iParamType=TYPE_FLD;
				FL_PARAM tem_fld=*m_pFlDParam;
				tem_fld.t=m_pCurLineTube->GetThick();
				tem_fld.UpdateCodeName();
				m_paramPlate.SetPartNo(tem_fld.codeName);
				if(m_pFlDParam2)
				{
					FL_PARAM tem_fld2=*m_pFlDParam2;
					tem_fld2.t=m_pRelatedTube?m_pRelatedTube->GetThick():tem_fld.t;
					tem_fld2.UpdateCodeName();
					strcpy(sCoupleFlPlateNo,tem_fld2.codeName);
				}
				m_paramPlate.m_bStdPart=true;
				m_paramPlate.thick=(long)m_pFlDParam->C;
				m_paramPlate.cMaterial=m_pFlDParam->cMaterial;
				R=m_pFlDParam->B*0.5;
				W=m_pFlDParam->D;
				LayoutBoltPara.m_fBoltLayoutD=m_pFlDParam->K;
				LayoutBoltPara.m_nBoltD=m_pFlDParam->nBoltD;
				LayoutBoltPara.m_nBoltNum=m_pFlDParam->nBoltNum;
			}
			else if((m_iPartType==3&&m_pRigidFlParam)||(m_iPartType==4&&m_pFlexibleFlParam))
			{
				FL_PARAM2 *pParam=NULL;
				if(m_iPartType==3)
				{
					m_paramPlate.m_iParamType=TYPE_FLG;
					pParam=m_pRigidFlParam;
					if(m_pRigidFlParam2)
						strcpy(sCoupleFlPlateNo,m_pRigidFlParam2->codeName);
				}
				else if(m_iPartType==4)
				{
					m_paramPlate.m_iParamType=TYPE_FLR;
					pParam=m_pFlexibleFlParam;
					if(m_pFlexibleFlParam2)
						strcpy(sCoupleFlPlateNo,m_pFlexibleFlParam2->codeName);
				}
				m_paramPlate.SetPartNo(pParam->codeName);
				m_paramPlate.m_bStdPart=true;
				m_paramPlate.thick=(long)pParam->fThick;
				m_paramPlate.cMaterial=pParam->cMaterial;
				R=pParam->fInnerD*0.5;
				W=pParam->fOuterD;
				LayoutBoltPara.m_fBoltLayoutD=pParam->fBoltCirD;
				LayoutBoltPara.m_nBoltD=pParam->nBoltD;
				LayoutBoltPara.m_nBoltNum=pParam->nBoltNum;
				//��ʼ���߰����
				if(pParam->nRibPlateNum>0)
				{
					m_bCreateAttachPlate=TRUE;
					AttachPlatePara.m_nPlateNum=pParam->nRibPlateNum;
					AttachPlatePara.A=pParam->para.A;
					AttachPlatePara.B=pParam->para.B;
					AttachPlatePara.C=pParam->para.C;
					AttachPlatePara.D=pParam->para.D;
					AttachPlatePara.E=pParam->para.E;
					AttachPlatePara.F=pParam->para.F;
					AttachPlatePara.G=pParam->para.G;
					AttachPlatePara.H=pParam->para.H;
					AttachPlatePara.m_fAngle=LayoutBoltPara.m_fOffsetAngle+180.0/pParam->nBoltNum;
				}
				else 
					m_bCreateAttachPlate=FALSE;
			}
			//��ӷ�����Ʋ���
			m_paramPlate.SetDesignItemValue('A',360,"���Ƕ�",PARAM_SHAPE); //�������Ƕ� wht 11-05-09
			m_paramPlate.SetDesignItemValue('R',R,"�����ڻ��뾶",PARAM_SHAPE);
			m_paramPlate.SetDesignItemValue('W',W,"�����⻷ֱ��",PARAM_SHAPE);
			m_paramPlate.SetDesignItemValue('H',H,"�����߳��ֹܵĸ߶�",PARAM_SHAPE);
			m_paramPlate.SetDesignItemValue('D',setupInfo.workNorm.rot_ang,"��׼X����Ըֹ�X����ת�Ƕ�");
			m_paramPlate.SetDesignItemValue('J',LayoutBoltPara.m_iStartEdge,"������׼��");
		}
		else if(m_iTubeEndLjStyle>=3&&m_iTubeEndLjStyle<=5)
		{
			if(m_pInsertPlateParam)
			{	//���β���
				A=m_pInsertPlateParam->A;
				R=m_pInsertPlateParam->R;
				W=m_pInsertPlateParam->W;
				N=m_pInsertPlateParam->N;
				S=m_pInsertPlateParam->S;
				H=m_pInsertPlateParam->H;
				L=m_pInsertPlateParam->L;
				M=m_pInsertPlateParam->M;
				//������Ϣ
				m_paramPlate.thick=m_pInsertPlateParam->Thick;
				m_paramPlate.cMaterial=m_pInsertPlateParam->cMaterial;
				m_paramPlate.SetPartNo(m_iPartType!=0?m_pInsertPlateParam->codeName:"");
				if(m_iTubeEndLjStyle==5&&m_iPartType!=0)
				{
					m_paramPlate.SetPartNo(m_pInsertPlateParam->datumPlatePartNo);
					strcpy(sWeldPlatePartNo,m_pInsertPlateParam->weldPlatePartNo);
					strcpy(sWrapPlatePartNo,m_pInsertPlateParam->wrapPlatePartNo);
					strcpy(sCodeName,m_pInsertPlateParam->codeName);
				}
				m_paramPlate.m_bStdPart=(m_iPartType!=0)?true:false;
				//��˨��Ϣ
				LayoutBoltPara.m_nBoltD=m_pInsertPlateParam->ls_param[0].iBoltD;
				for(int i=0;i<m_pInsertPlateParam->iBoltRect;i++)
				{
					LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
					pBoltPara->m_nBoltD=m_pInsertPlateParam->ls_param[i].iBoltD;
					pBoltPara->m_nBoltNum=m_pInsertPlateParam->ls_param[i].iBoltNum;
					pBoltPara->m_nColNum=m_pInsertPlateParam->ls_param[i].iColumn;
					pBoltPara->m_nRowNum=m_pInsertPlateParam->ls_param[i].iRow;
					pBoltPara->m_fColSpace=m_pInsertPlateParam->ls_param[i].fHorzSpace;
					pBoltPara->m_fRowSpace=m_pInsertPlateParam->ls_param[i].fVertSpace;
					pBoltPara->m_posFirstBolt.x=m_pInsertPlateParam->ls_param[i].fBoltPosX;
					pBoltPara->m_posFirstBolt.y=m_pInsertPlateParam->ls_param[i].fBoltPosY;
					pBoltPara->m_fSpecialSpace=pBoltPara->m_fColSpace;	//�������ʼֵӦ�����о� 
					if(m_pInsertPlateParam->ls_param[i].iColumn==-1
						||m_pInsertPlateParam->ls_param[i].iRow==-1)
						pBoltPara->m_bStaggerLayout=TRUE;
					else
						pBoltPara->m_bStaggerLayout=FALSE;
					pBoltPara->m_bMirLayout=FALSE;
				}
			}
			else 
			{	
				m_paramPlate.SetPartNo("");
				H = fTubeD+40;
				if(m_iTubeEndLjStyle==5)
				{   //��ͨʮ�ֲ��
					W = T;
					A = 0.5*(T+W);
					R = 0.5*T+W;
					S = L;
					N = (int)(0.5*H);
				}
				//��ʼ����˨��Ϣ
				double fHalfRowSpace=ftoi(fTubeD*0.8*0.5);
				LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
				pBoltPara->m_nBoltD=20;
				pBoltPara->m_nBoltNum=4;
				pBoltPara->m_nColNum=2;
				pBoltPara->m_nRowNum=2;
				pBoltPara->m_fColSpace=(L-M-80);
				pBoltPara->m_fRowSpace=fHalfRowSpace*2;
				pBoltPara->m_posFirstBolt.x=M+10+30;
				pBoltPara->m_posFirstBolt.y=-fHalfRowSpace;
				pBoltPara->m_posFirstBolt.z=0;
				if(m_iTubeEndLjStyle==5)
				{	//ʮ�ֲ����˨
					pBoltPara->m_bMirLayout=FALSE;
					pBoltPara->m_bStaggerLayout=FALSE;
					pBoltPara->m_fRowSpace=60;
					pBoltPara->m_fSpecialSpace=80;
					pBoltPara->m_posFirstBolt.x=30;
					pBoltPara->m_posFirstBolt.y=30;
					pBoltPara->m_posFirstBolt.z=0;
				}
			}
			if(m_iTubeEndLjStyle==3)	//U�Ͷ˰�
			{
				if(NodePlatePara.m_nThick>0)	//U�Ͳ��ֹ�ƫ��������
					setupInfo.m_fTubeNormOffset=0;
				m_paramPlate.SetDesignItemValue('W',W,"U�Ͳ����������ļ��",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('L',L,"�峤",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('H',H,"���",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('M',M,"U�Ͳ����ֹ��ཻ�ĳ���",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('N',N,"�нǳ���",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('S',S,"�нǿ��",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('D',setupInfo.workNorm.rot_ang,"��������Ըֹܻ�׼����ת�Ƕ�");
				m_paramPlate.m_iParamType=TYPE_UEND;
			}
			else if(m_iTubeEndLjStyle==4)	//���Ͳ��
			{
				if(NodePlatePara.m_nThick>0)	//���Ͳ��
				{
					setupInfo.m_fTubeNormOffset=-0.5*(m_paramPlate.thick+NodePlatePara.m_nThick);
					if(fabs(fabs(setupInfo.workNorm.rot_ang)-180)<EPS)	//���Ͳ����ת180��
						setupInfo.m_fTubeNormOffset=(NodePlatePara.m_nThick+m_paramPlate.thick)*0.5;
					else if(fabs(setupInfo.workNorm.rot_ang)>EPS)
						setupInfo.m_fTubeNormOffset=0;	//��0���180��ʱƫ��������
				}
				m_paramPlate.SetDesignItemValue('W',W,"��ߵĸ߶�",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('L',L,"�峤",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('H',H,"���",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('M',M,"����ֹ��ཻ�ĳ���",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('N',N,"�нǳ���",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('S',S,"�нǿ��",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('D',setupInfo.workNorm.rot_ang,"��������Ըֹܻ�׼����ת�Ƕ�");
				m_paramPlate.m_iParamType=TYPE_ROLLEND;
				//
				AttachPlatePara.A=L;
				AttachPlatePara.B=H*0.5;
				AttachPlatePara.C=30;
				AttachPlatePara.D=30;
				AttachPlatePara.E=30;
				AttachPlatePara.F=30;
			}
			else if(m_iTubeEndLjStyle==5)	//X�Ͳ��
			{	
				if(NodePlatePara.m_nThick>0)	//ʮ�ֲ��ֹ�ƫ��������
					setupInfo.m_fTubeNormOffset=0;
				m_paramPlate.SetDesignItemValue('L',L,"�峤",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('H',H,"���",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('M',M,"����ֹ��ཻ�ĳ���",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('W',W,"ʮ���Ͳ���ϸ��Ӱ�ĺ��",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('N',N,"ʮ���Ͳ���ϸ��Ӱ�Ŀ��",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('S',S,"ʮ���Ͳ���ϸ��Ӱ�ĳ���",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('A',A,"ʮ���Ͳ���ϸ��Ӱ�Ķ�λ����A",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('R',R,"ʮ���Ͳ���ϸ��Ӱ�Ķ�λ����R",PARAM_SHAPE);
				m_paramPlate.SetDesignItemValue('D',setupInfo.workNorm.rot_ang,"��������Ըֹܻ�׼����ת�Ƕ�");
				m_paramPlate.m_iParamType=TYPE_XEND;
				//
				AttachPlatePara.m_fAngle=45;
				AttachPlatePara.A=0.5*(H-T);
				AttachPlatePara.B=S;
				AttachPlatePara.C=30;
				AttachPlatePara.D=AttachPlatePara.A-30;
				AttachPlatePara.E=30;
			}
			DESIGNODDMENT * pCurOddment=&m_pCurLineTube->desEndOdd;
			if(m_bStartEnd)
				pCurOddment=&(m_pCurLineTube->desStartOdd);
			if(pCurOddment->m_iOddCalStyle==0)//������ײ��������ͷ wht 05-09
				setupInfo.m_fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate,true);
		}
		else if(m_iTubeEndLjStyle==0||m_iTubeEndLjStyle==1)
		{	//��ʼ��ƽ���и�����Լ��ֹ�������
			if(m_pCurLineTube->pStart==m_pBaseNode)
			{	//ʼ��
				if(m_pCurLineTube->desStartPos.endPosType==1)
					m_hDatumTube1=m_pCurLineTube->desStartPos.hDatumPart;
			}
			else
			{	//�ն�
				if(m_pCurLineTube->desEndPos.endPosType==1)
					m_hDatumTube1=m_pCurLineTube->desEndPos.hDatumPart;
			}
			UpdateTubeOddment();	//ƽ���и��ֹ���� wht 11-05-10
		}
	}
}
//������ѡ�������������ʾ���ʵ�ͼƬ
void CDesignTubeEndLjDlg::UpdateSketchMap(int iCurSel/*=0*/)
{
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
	if(m_iTubeEndLjStyle==2)
	{
		if(m_iPartType==1&&m_pFlPParam)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLP);
		else if(m_iPartType==2&&m_pFlDParam)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLD);
		else //if(m_iPartType==0)
		{	//��ͨ����
			if(iCurSel==0||iCurSel==1)	//����������
				m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL2);
			else if(iCurSel==2)
				m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL1);
			else if(iCurSel==3)	//��˨�븽�Ӱ�
				m_CurPlateBmp.LoadBitmap(IDB_BMP_ELBOW_PLATE1);
		}
	}
	else if(m_iTubeEndLjStyle==3)		//U�Ͷ˰�
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_UEND_BOLT);
	else if(m_iTubeEndLjStyle==4)	//���Ͳ��
	{
		if(iCurSel<3)	//���� ��˨
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_ROLLEND_BOLT);
		else if(iCurSel==3)			//���Ӱ�GROUP_ATTACH_PLATE-1
			m_CurPlateBmp.LoadBitmap(IDB_BMP_XEND_RIB_PLATE);
	}
	else if(m_iTubeEndLjStyle==5)	//X�Ͳ��
	{
		if(iCurSel<3)	//���� ��˨
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_XEND_BOLT);
		else if(iCurSel==3)			//���Ӱ�GROUP_ATTACH_PLATE-1
			m_CurPlateBmp.LoadBitmap(IDB_BMP_XEND_RIB_PLATE1);
	}
	else 
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_NONE);
	pPic->SetBitmap(m_CurPlateBmp);
}
int CDesignTubeEndLjDlg::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText,sCurParam;
	GetPropStr(id,sCurParam);
	CLDSVector xVector;
	if(m_xSubPropList.m_nObjClassTypeId==0)
		xVector=setupInfo.workNorm;
	else if(m_xSubPropList.m_nObjClassTypeId==1)
		xVector=setupInfo.workNormUpVert;
	else if(m_xSubPropList.m_nObjClassTypeId==2)
		xVector=setupInfo.workNormDownVert;
	//���β���
	if(sCurParam.GetLength()==1&&((sCurParam[0]>='A'&&sCurParam[0]<='Z')
		||(sCurParam[0]>='a'&&sCurParam[0]<='z')))
	{
		if(m_iTubeEndLjStyle==2&&((m_iPartType==1&&m_pFlPParam)
			||(m_iPartType==2&&m_pFlDParam)))
		{	//�߾�������Ӧ���� wht 11-01-21
			FL_PARAM fl_param;
			if(m_iPartType==1)
				fl_param=*m_pFlPParam;
			else 
				fl_param=*m_pFlDParam;
			if(sCurParam[0]=='A')
				sText.Printf("%f",fl_param.A);
			else if(sCurParam[0]=='B')
				sText.Printf("%f",fl_param.B);
			else if(sCurParam[0]=='C')
				sText.Printf("%f",fl_param.C);
			else if(sCurParam[0]=='D')
				sText.Printf("%f",fl_param.D);
			else if(sCurParam[0]=='K')
				sText.Printf("%f",fl_param.K);
			else if(sCurParam[0]=='L')
				sText.Printf("%f",fl_param.L);
			else if(sCurParam[0]=='H')
				sText.Printf("%f",fl_param.H);
			else if(sCurParam[0]=='R')
				sText.Printf("%f",fl_param.R);
			else if(sCurParam[0]=='N')
				sText.Printf("%f",fl_param.N);
			else if(sCurParam[0]=='S')
				sText.Printf("%f",fl_param.S);
			else if(sCurParam[0]=='d')
				sText.Printf("%f",fl_param.d);
			else if(sCurParam[0]=='t')
				sText.Printf("%f",fl_param.t);
		}
		else if(m_iTubeEndLjStyle==2&&((m_iPartType==3&&m_pRigidFlParam)
			||(m_iPartType==4&&m_pFlexibleFlParam)))
		{	//���Է��������Է�����Ӧ���� wht 12-05-15
			FL_PARAM2 fl_param;
			if(m_iPartType==3)
				fl_param=*m_pRigidFlParam;
			else 
				fl_param=*m_pFlexibleFlParam;
			if(sCurParam[0]=='A')
				sText.Printf("%f",fl_param.para.A);
			else if(sCurParam[0]=='B')
				sText.Printf("%f",fl_param.para.B);
			else if(sCurParam[0]=='C')
				sText.Printf("%f",fl_param.para.C);
			else if(sCurParam[0]=='D')
				sText.Printf("%f",fl_param.para.D);
			else if(sCurParam[0]=='E')
				sText.Printf("%f",fl_param.para.E);
			else if(sCurParam[0]=='F')
				sText.Printf("%f",fl_param.para.F);
			else if(sCurParam[0]=='G')
				sText.Printf("%f",fl_param.para.G);
			else if(sCurParam[0]=='H')
				sText.Printf("%f",fl_param.para.H);
			else if(sCurParam[0]=='d')
				sText.Printf("%f",fl_param.d);
			else if(sCurParam[0]=='T')
				sText.Printf("%f",fl_param.para.T);
			//�������β���
			else if(sCurParam[0]=='R')
				sText.Printf("%f",fl_param.fInnerD);
			else if(sCurParam[0]=='W')
				sText.Printf("%f",fl_param.fOuterD);
			else if(sCurParam[0]=='t')
				sText.Printf("%f",fl_param.fThick);
		}
		else if(m_iTubeEndLjStyle==2&&m_iPartType==0&&sCurParam[0]=='R')
		{
			DESIGN_PLATE_ITEM *pItem=m_paramPlate.GetDesignItem(sCurParam[0]);
			if(pItem)
				sText.Printf("%f",pItem->value.fValue*2);
		}
		else 
		{
			DESIGN_PLATE_ITEM *pItem=m_paramPlate.GetDesignItem(sCurParam[0]);
			if(pItem)
			{
				if(pItem->dataType==1)
				{
					if(pItem->value.hPart<0)
						sText.Printf("-0X%X",abs(pItem->value.hPart));
					else 
						sText.Printf("0X%X",pItem->value.hPart);
				}
				else
					sText.Printf("%f",pItem->value.fValue);
			}
		}
		SimplifiedNumString(sText);
	}
	//��������
	else if(CDesignTubeEndLjDlg::GetPropID("m_iPartType")==id) 
	{
		if(m_iTubeEndLjStyle==2)
		{
			if(m_iPartType==0)
				sText.Copy("0.��ͨ����");
			else if(m_iPartType==1)
				sText.Copy("1.ƽ������");
			else if(m_iPartType==2)
				sText.Copy("2.�Ժ�����");
			else if(m_iPartType==3)
				sText.Copy("3.���Է���");
			else if(m_iPartType==4)
				sText.Copy("4.���Է���");
		}
		else if(m_iTubeEndLjStyle>2)
		{
			if(m_iPartType==0)
				sText.Copy("0.�Զ�����");
			else if(m_iPartType==1)
				sText.Copy("1.��׼���");
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iWrapPlateType")==id) 
	{	//ʮ�ֲ���������
		if(m_iWrapPlateType==0)
			sText.Copy("0.�ְ�");
		else if(m_iWrapPlateType==1)
			sText.Copy("1.�Ǹ�");
	}
	//��׼��������
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStdPartLevel")==id)
	{
		if(m_iTubeEndLjStyle==2)
			sText.Printf("%C",m_cFlLevel1);
		else 
			sText.Printf("%d",m_iStdPartLevel);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_iStdPartLevel2")==id)
	{
		if(m_iTubeEndLjStyle==2)
			sText.Printf("%C",m_cFlLevel2);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_cStdPartMat")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(m_cStdPartMat,matStr))
			sText.Printf("%s",matStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCoupleFLCreate")==id)
	{
		if(!m_bCoupleFLCreate)
			sText.Printf("0.���෨��");
		else
			sText.Printf("1.�ɶ����");
	}
	//ƽ���и�
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.x")==id)
	{
		sText.Printf("%f",cutPlaneOrigin.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.y")==id)	
	{
		sText.Printf("%f",cutPlaneOrigin.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneOrigin.z")==id)
	{
		sText.Printf("%f",cutPlaneOrigin.z);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.x")==id)
	{
		sText.Printf("%f",cutPlaneNorm.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.y")==id)
	{
		sText.Printf("%f",cutPlaneNorm.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("cutPlaneNorm.z")==id)
	{
		sText.Printf("%f",cutPlaneNorm.z);
		SimplifiedNumString(sText);
	}
	//�ֹ����
	else if(CDesignTubeEndLjDlg::GetPropID("m_bFaceToFace")==id)
	{
		if(m_bFaceToFace)
			sText.Copy("��");
		else 
			sText.Copy("��");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_hWeldLinkObj")==id)
		sText.Printf("0X%X",m_hWeldLinkObj);
	else if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube1")==id)
		sText.Printf("0X%X",m_hDatumTube1);
	else if(CDesignTubeEndLjDlg::GetPropID("m_hDatumTube2")==id)
		sText.Printf("0X%X",m_hDatumTube2);
	//������Ϣ
	else if(CDesignTubeEndLjDlg::GetPropID("sPartNo")==id)
		sText.Printf("%s",(char*)m_paramPlate.GetPartNo());
	else if(CDesignTubeEndLjDlg::GetPropID("sCoupleFlPlateNo")==id)
		sText.Printf("%s",sCoupleFlPlateNo);
	else if(CDesignTubeEndLjDlg::GetPropID("sWeldPlatePartNo")==id)
		sText.Printf("%s",sWeldPlatePartNo);
	else if(CDesignTubeEndLjDlg::GetPropID("sWrapPlatePartNo")==id)
		sText.Printf("%s",sWrapPlatePartNo);
	else if(CDesignTubeEndLjDlg::GetPropID("sCodeName")==id)
		sText.Printf("%s",sCodeName);
	else if(CDesignTubeEndLjDlg::GetPropID("iSeg")==id)
		sText.Copy(m_paramPlate.iSeg.ToString());
	else if(CDesignTubeEndLjDlg::GetPropID("cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(m_paramPlate.cMaterial,matStr))
			sText.Printf("%s",matStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_uStatMatCoef")==id)
		sText.Printf("%d",m_paramPlate.m_uStatMatCoef);
	else if(CDesignTubeEndLjDlg::GetPropID("thick")==id)
		sText.Printf("%d",m_paramPlate.thick);
	//װ�����
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.rot_ang")==id)
	{
		sText.Printf("%f",setupInfo.workNorm.rot_ang);
		SimplifiedNumString(sText);
		if(m_iTubeEndLjStyle==4)
		{	//���Ͳ����Ҫ��ʾ��߳��� wht 10-10-29
			f3dPoint tube_work_norm=m_pCurLineTube->WorkPlaneNorm();
			if(ftol(setupInfo.workNorm.rot_ang)==0)
			{
				if(tube_work_norm*work_norm>0)
					strcpy(sText,"����"); //��ǰ�����淨����ֹܹ����淨��ͬ��
				else 
					strcpy(sText,"����"); //��ǰ�����淨����ֹܹ����淨�߷���
			}
			else if(ftol(setupInfo.workNorm.rot_ang)==180)
			{
				if(tube_work_norm*work_norm>0)
					strcpy(sText,"����"); //��ǰ�����淨����ֹܹ����淨��ͬ��
				else 
					strcpy(sText,"����"); //��ǰ�����淨����ֹܹ����淨�߷���
			}
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(setupInfo.workNorm.vector));
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.x")==id)
	{
		sText.Printf("%f",setupInfo.workNorm.vector.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.y")==id)
	{
		sText.Printf("%f",setupInfo.workNorm.vector.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("MotherPlateNorm.z")==id)
	{
		sText.Printf("%f",setupInfo.workNorm.vector.z);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(setupInfo.workNormUpVert.vector));
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.x")==id)
	{
		sText.Printf("%f",setupInfo.workNormUpVert.vector.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.y")==id)
	{
		sText.Printf("%f",setupInfo.workNormUpVert.vector.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("UpVertPlateNorm.z")==id)
	{
		sText.Printf("%f",setupInfo.workNormUpVert.vector.z);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(setupInfo.workNormDownVert.vector));
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.x")==id)
	{
		sText.Printf("%f",setupInfo.workNormDownVert.vector.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.y")==id)
	{
		sText.Printf("%f",setupInfo.workNormDownVert.vector.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("DownVertPlateNorm.z")==id)
	{
		sText.Printf("%f",setupInfo.workNormDownVert.vector.z);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("H1")==id)
	{
		sText.Printf("%f",setupInfo.H1);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fNormOffset")==id)
	{
		sText.Printf("%f",setupInfo.m_fNormOffset);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fOddment")==id)
	{
		sText.Printf("%f",setupInfo.m_fOddment);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fWeldHeight")==id)
	{
		sText.Printf("%f",setupInfo.m_fWeldHeight);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fTubeNormOffset")==id)
	{
		sText.Printf("%f",setupInfo.m_fTubeNormOffset);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("m_fTubeEccentricDist")==id)
	{
		sText.Printf("%f",setupInfo.m_fTubeEccentricDist);
		SimplifiedNumString(sText);
	}
	//������Ʋ���
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.norm_style")==id)
	{
		if(xVector.norm_style==0)
			sText.Copy("0.ָ������");
		else if(xVector.norm_style==1)
			sText.Copy("1.�Ǹ�֫����");
		else if(xVector.norm_style==2)
			sText.Copy("2.�˼������淨��");
		else if(xVector.norm_style==3)
			sText.Copy("3.������������");
		else if(xVector.norm_style==4)
			sText.Copy("4.�����ϵ���Է���");
		else if(xVector.norm_style==5)
			sText.Copy("5.�˼��ϵ���������");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.hVicePart")==id)
		sText.Printf("0X%X",xVector.hVicePart);
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.hCrossPart")==id)
		sText.Printf("0X%X",xVector.hCrossPart);
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.norm_wing")==id)
	{
		if(xVector.norm_wing==0)
			sText.Printf("%s","0.X֫����");
		else
			sText.Printf("%s","1.Y֫����");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.direction")==id)
	{
		if(xVector.norm_style==1)
		{
			if(xVector.direction==0)
				sText.Printf("%s","0.����");
			else
				sText.Printf("%s","1.����");
		}
		else if(xVector.norm_style==5)
		{
			if(xVector.direction==0)
				sText.Copy("0.ʼ->��");
			else if(xVector.direction==1)
				sText.Copy("1.��->ʼ");
			else if(xVector.direction==2)
				sText.Copy("2.�Ի�׼����ת");
			else if(xVector.direction==3)
				sText.Copy("3.�Ժ�������ת");
		}
		else
		{
			if(xVector.direction==0)
				sText.Printf("%s","0.����");
			else
				sText.Printf("%s","1.����");
		}
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.rot_ang")==id)
	{
		sText.Printf("%f",xVector.rot_ang);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVectorType")==id)
		sText.Printf("%s",CStdVector::GetVecTypeString(xVector.nearVector));
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.x")==id)
	{
		sText.Printf("%lf",xVector.nearVector.x);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.y")==id)
	{
		sText.Printf("%lf",xVector.nearVector.y);
		SimplifiedNumString(sText);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("design_axis_z.nearVector.z")==id)
	{
		sText.Printf("%lf",xVector.nearVector.z);
		SimplifiedNumString(sText);
	}
	//��˨���ò���
	else if(GetPropID("m_nBoltD")==id)
		sText.Printf("%d",LayoutBoltPara.m_nBoltD);
	else if(GetPropID("m_iDriection")==id)
	{
		if(LayoutBoltPara.m_iDirection==0)
			sText.Copy("0.����");
		else
			sText.Copy("1.����");
	}
	else if(GetPropID("m_iStartEdge")==id)
	{
		if(LayoutBoltPara.m_iStartEdge==0)
			sText.Copy("0.��׼��");
		else
			sText.Copy("1.������");
	}
	else if(GetPropID("m_fOffsetAngle")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_fOffsetAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fBoltLayoutD")==id)	
	{
		sText.Printf("%f",LayoutBoltPara.m_fBoltLayoutD);
		SimplifiedNumString(sText);
	}
	//���Ӱ����
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCreateAttachPlate")==id)
	{
		if(m_bCreateAttachPlate)
			sText.Copy("��");
		else 
			sText.Copy("��");
	}
	else if(GetPropID("AttachPlatePara.A")==id)
	{
		sText.Printf("%f",AttachPlatePara.A);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.B")==id)
	{
		sText.Printf("%f",AttachPlatePara.B);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.C")==id)
	{
		sText.Printf("%f",AttachPlatePara.C);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.D")==id)
	{
		sText.Printf("%f",AttachPlatePara.D);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.E")==id)
	{
		sText.Printf("%f",AttachPlatePara.E);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.m_iCutMode")==id)
	{
		if(AttachPlatePara.m_iCutMode==0)
			sText.Copy("0.�н�");
		else
			sText.Copy("1.�л�");
	}
	else if(GetPropID("AttachPlatePara.F")==id)
	{
		sText.Printf("%f",AttachPlatePara.F);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.G")==id)
	{
		sText.Printf("%f",AttachPlatePara.G);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.H")==id)
	{
		sText.Printf("%f",AttachPlatePara.H);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(AttachPlatePara.cMaterial,matStr))
		sText.Printf("%s",matStr);
	}
	else if(GetPropID("AttachPlatePara.sPartNo")==id)
		sText.Printf("%s",AttachPlatePara.sPartNo);
	else if(GetPropID("AttachPlatePara.m_fAngle")==id)
	{
		sText.Printf("%f",AttachPlatePara.m_fAngle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("AttachPlatePara.m_nPlateNum")==id)
		sText.Printf("%d",AttachPlatePara.m_nPlateNum);
	else if(GetPropID("AttachPlatePara.m_nThick")==id)
		sText.Printf("%d",AttachPlatePara.m_nThick);
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_bMirCreate")==id)
	{
		if(AttachPlatePara.m_bMirCreate)
			sText.Copy("��");
		else 
			sText.Copy("��");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("AttachPlatePara.m_bUseAngle")==id)
	{
		if(AttachPlatePara.m_bUseAngle)
			sText.Copy("��");
		else 
			sText.Copy("��");
	}
	//������
	else if(CDesignTubeEndLjDlg::GetPropID("m_bCreateFillPlate")==id)
	{
		if(m_bCreateFillPlate)
			sText.Copy("��");
		else 
			sText.Copy("��");
	}
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.cMat")==id)
	{
		char matStr[20]="";
		if(QuerySteelMatMark(FillPlatePara.cMat,matStr))
			sText.Printf("%s",matStr);
	}
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.sPartNo")==id)
		sText.Printf("%s",FillPlatePara.sPartNo);
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nThick")==id)
		sText.Printf("%d",FillPlatePara.nThick);
	else if(CDesignTubeEndLjDlg::GetPropID("FillPlatePara.nNodePlateThick")==id)
		sText.Printf("%d",FillPlatePara.nNodePlateThick);
	//�ڵ��
	else if(GetPropID("NodePlatePara.cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(NodePlatePara.cMaterial,matStr))
			sText.Printf("%s",matStr);
	}
	else if(GetPropID("NodePlatePara.sPartNo")==id)
		sText.Printf("%s",NodePlatePara.sPartNo);
	else if(GetPropID("NodePlatePara.m_nRollHeight")==id)
		sText.Printf("%d",NodePlatePara.m_nRollHeight);
	else if(GetPropID("NodePlatePara.m_nThick")==id)
		sText.Printf("%d",NodePlatePara.m_nThick);
	else if(GetPropID("m_nBoltNum")==id)
		sText.Printf("%d",LayoutBoltPara.m_nBoltNum);
	else if(GetPropID("m_bStaggerLayout")==id)
	{
		if(LayoutBoltPara.m_bStaggerLayout)
			sText.Copy("��");
		else
			sText.Copy("��");
	}
	else if(GetPropID("m_bMirLayout")==id)
	{
		if(LayoutBoltPara.m_bMirLayout)
			sText.Copy("��");
		else
			sText.Copy("��");
	}
	else if(GetPropID("m_fSpecialSpace")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_fSpecialSpace);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_nRowNum")==id)
		sText.Printf("%d",LayoutBoltPara.m_nRowNum);
	else if(GetPropID("m_nColNum")==id)
		sText.Printf("%d",LayoutBoltPara.m_nColNum);
	else if(GetPropID("m_fRowSpace")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_fRowSpace);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fColSpace")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_fColSpace);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_posFirstBolt.x")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_posFirstBolt.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_posFirstBolt.y")==id)
	{
		sText.Printf("%f",LayoutBoltPara.m_posFirstBolt.y);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
void CDesignTubeEndLjDlg::UpdateProfileProperty(CPropTreeItem *pParentItem)
{
	if(pParentItem==NULL)
		return;
	CString sPartName=GetTubeEndLjName();
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	m_propList.DeleteAllSonItems(pParentItem);
	//
	int iPartType=GetPartType();
	CPropTreeItem *pPropItem=NULL;
	if(m_iTubeEndLjStyle==2&&iPartType>2)
	{	//���Է������Է������β���
		//�����ھ�
		pPropItem=oper.InsertEditPropItem(pParentItem,"R","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "�ھ�B";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s�ھ�B",sPartName);
		pPropItem->SetReadOnly(TRUE);
		//�����⾶
		pPropItem=oper.InsertEditPropItem(pParentItem,"W","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "�⾶W";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s�⾶W",sPartName);
		pPropItem->SetReadOnly(TRUE);
		//�������
		pPropItem=oper.InsertEditPropItem(pParentItem,"t","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "���T";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s���T",sPartName);
		pPropItem->SetReadOnly(TRUE);
	}
	else if(m_iTubeEndLjStyle==2&&GetPartType()>0)
	{	//�����⾶/�ֹ���ֱ��
		pPropItem=oper.InsertEditPropItem(pParentItem,"A","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "A";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "A=�����⾶/�ֹ���ֱ��";
		pPropItem->SetReadOnly(TRUE);
		//�����ھ�
		pPropItem=oper.InsertEditPropItem(pParentItem,"B","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "B";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "B=�����ھ�";
		pPropItem->SetReadOnly(TRUE);
		//�����⾶
		pPropItem=oper.InsertEditPropItem(pParentItem,"D","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "D";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "D=�����⾶";
		pPropItem->SetReadOnly(TRUE);
		//�������
		pPropItem=oper.InsertEditPropItem(pParentItem,"C","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "C";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "C=�������";
		pPropItem->SetReadOnly(TRUE);
		//�����߶�
		pPropItem=oper.InsertEditPropItem(pParentItem,"H","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "H";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "H=�����߶�";
		pPropItem->SetReadOnly(TRUE);
		//��������ֱ��������R�ߴ�
		pPropItem=oper.InsertEditPropItem(pParentItem,"N","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "N";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "N=��������ֱ��(����R�ߴ�)";
		pPropItem->SetReadOnly(TRUE);
		//����������� //�Ժ�����ʱS��¼S��������ܺ�ȵ����� wht 10-01-20
		pPropItem=oper.InsertEditPropItem(pParentItem,"S","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "S";
		pPropItem->m_lpNodeInfo->m_strPropHelp =(GetPartType()==2)?"S=�����������-���ܺ�ȵĲ�ֵ":"S=�����������";
		pPropItem->SetReadOnly(TRUE);
		//�����������ǰ뾶
		pPropItem=oper.InsertEditPropItem(pParentItem,"R","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "R";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "R=�����������ǰ뾶";
		pPropItem->SetReadOnly(TRUE);
		//��˨�׾�
		pPropItem=oper.InsertEditPropItem(pParentItem,"L","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "L";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "L=��˨�׾�";
		pPropItem->SetReadOnly(TRUE);
	}
	else if(m_iTubeEndLjStyle==2&&GetPartType()==0)
	{
		oper.InsertEditPropItem(pParentItem,"R","�ھ�B","�����ھ�",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"W","�⾶W","�����⾶",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"H","����H","�ֹܲ��뵽���������",-1,TRUE);
	}
	else if(m_iTubeEndLjStyle>2&&m_iTubeEndLjStyle<6)
	{	//������β���
		//��峤��L
		pPropItem=oper.InsertEditPropItem(pParentItem,"L","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "��峤��L";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s����L",sPartName);
		pPropItem->SetReadOnly(iPartType>0);
		//���ӳ���
		pPropItem=oper.InsertEditPropItem(pParentItem,"M","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "���ӳ���M";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s���ӳ���M",sPartName);
		pPropItem->SetReadOnly(iPartType>0);
		//N
		pPropItem=oper.InsertEditPropItem(pParentItem,"N","","",-1,TRUE);
		if(m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4)	
		{	//U�Ͳ�� ���Ͳ��
			pPropItem->m_lpNodeInfo->m_strPropName = "�нǳ���N";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s�нǳ���N",sPartName);
		}
		else
		{	//ʮ�ֲ��
			pPropItem->m_lpNodeInfo->m_strPropName = "������N";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s������N",sPartName);
		}
		pPropItem->SetReadOnly(iPartType>0);
		//S
		pPropItem=oper.InsertEditPropItem(pParentItem,"S","","",-1,TRUE);
		if(m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4)	//U�Ͳ�� ���Ͳ��
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "�нǿ��S";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s�нǿ��S",sPartName);
		}
		else	//ʮ�ֲ��
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "���峤��S";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s���峤��S",sPartName);
		}
		pPropItem->SetReadOnly(iPartType>0);
		//H
		pPropItem=oper.InsertEditPropItem(pParentItem,"H","","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropName = "�����H";
		pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s���H",sPartName);
		pPropItem->SetReadOnly(iPartType>0);
		//W
		pPropItem=oper.InsertEditPropItem(pParentItem,"W","","",-1,TRUE);
		if(m_iTubeEndLjStyle==3)	//U�Ͳ��
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "U�Ͳ۸߶�W";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s���������ľ���W",sPartName);
		}
		else if(m_iTubeEndLjStyle==4) //��߲��
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "��߸߶�W";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s��߸߶�W",sPartName);
		}
		else	//ʮ�ֲ��	
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "������W";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s������W",sPartName);
		}
		pPropItem->SetReadOnly(iPartType>0);
		if(m_iTubeEndLjStyle==5)
		{	//ʮ�ֲ��
			//A
			pPropItem=oper.InsertEditPropItem(pParentItem,"A","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_strPropName = "���嶨λ����A";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s���嶨λ����A,������ֹܶ�֮��ľ���",sPartName);
			pPropItem->SetReadOnly(iPartType>0);
			//R
			pPropItem=oper.InsertEditPropItem(pParentItem,"R","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_strPropName = "���嶨λ����R";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s���嶨λ����R,���嵽�䴹ֱ����������֮��ľ���",sPartName);
			pPropItem->SetReadOnly(iPartType>0);
		}
	}
}
//���¸��Ӱ����
void CDesignTubeEndLjDlg::UpdateFillPlateProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//���Ӱ岼�ò���
	if(m_bCreateFillPlate&&m_iTubeEndLjStyle==5)
	{	
		//�ڵ����
		if(FillPlatePara.nNodePlateThick>0)
		{
			CPropTreeItem* pItem=oper.InsertEditPropItem(pParentItem,"FillPlatePara.nNodePlateThick","","",-1,TRUE);
			pItem->SetReadOnly();
		}
		//
		oper.InsertEditPropItem(pParentItem,"FillPlatePara.sPartNo","","",-1,TRUE);
		//
		oper.InsertCmbListPropItem(pParentItem,"FillPlatePara.cMat","","","",-1,TRUE);
		//
		oper.InsertEditPropItem(pParentItem,"FillPlatePara.nThick","","",-1,TRUE);
	}
}
//���¸��Ӱ����
void CDesignTubeEndLjDlg::UpdateAttachPlateProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	CPropTreeItem* pPropItem=NULL;
	if(!m_bCreateAttachPlate)
		return;
	//���Ӱ岼�ò���
	if(m_iTubeEndLjStyle==2)
	{	//����
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.sPartNo","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.cMaterial","","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_nThick","","",-1,TRUE);
		//�����߰����
		pPropItem=oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_nPlateNum","","",-1,TRUE);
		pPropItem->SetReadOnly();
		//���ƫ�ƽ�
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_fAngle","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.A","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.B","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.C","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.D","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.E","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.m_iCutMode","","","",-1,TRUE);
		pPropItem=oper.InsertEditPropItem(pParentItem,"AttachPlatePara.H","","",-1,TRUE);
		if(AttachPlatePara.H>0)
		{
			oper.InsertEditPropItem(pPropItem,"AttachPlatePara.F","","",-1,TRUE);
			oper.InsertEditPropItem(pPropItem,"AttachPlatePara.G","","",-1,TRUE);
		}
	}
	else if(m_iTubeEndLjStyle==4)
	{
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.sPartNo","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.cMaterial","","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_nThick","","",-1,TRUE);
		//
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.m_bMirCreate","","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.A","����L","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.B","���W","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.C","�нǳ�L1","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.D","�нǿ�H1","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.E","�нǳ�L2","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.F","�нǿ�H2","",-1,TRUE);
	}
	else if(m_iTubeEndLjStyle==5)
	{
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.sPartNo","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.cMaterial","","","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_nThick","","",-1,TRUE);
		//
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.m_bMirCreate","","","",-1,TRUE);
		oper.InsertCmbListPropItem(pParentItem,"AttachPlatePara.m_bUseAngle","","","",-1,TRUE);
		pPropItem=oper.InsertEditPropItem(pParentItem,"AttachPlatePara.m_fAngle","","",-1,TRUE);
		pPropItem->SetReadOnly(!AttachPlatePara.m_bUseAngle);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.A","����A","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.B","����B","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.C","����C","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.D","����D","",-1,TRUE);
		oper.InsertEditPropItem(pParentItem,"AttachPlatePara.E","����E","",-1,TRUE);
	}
}
//��ʾ������
void CDesignTubeEndLjDlg::DisplayChildPropertyList(CPropTreeItem *pItem,BOOL bUpdate/*=FALSE*/)
{
	RefreshCtrlState(FALSE);
	if(!bUpdate)
		m_xSubPropList.CleanTree();
	if(pItem)
	{
		if(pItem->m_idProp==GetPropID("MotherPlateNorm"))
			m_xSubPropList.m_nObjClassTypeId = 0;
		else if(pItem->m_idProp==GetPropID("UpVertPlateNorm"))
			m_xSubPropList.m_nObjClassTypeId = 1;
		else if(pItem->m_idProp==GetPropID("DownVertPlateNorm"))
			m_xSubPropList.m_nObjClassTypeId = 2;
	}
	CLDSVector xVector;
	if(m_xSubPropList.m_nObjClassTypeId==0)
		xVector=setupInfo.workNorm;
	else if(m_xSubPropList.m_nObjClassTypeId==1)
		xVector=setupInfo.workNormUpVert;
	else if(m_xSubPropList.m_nObjClassTypeId==2)
		xVector=setupInfo.workNormDownVert;
	else
		return;
	m_xSubPropList.SetModifyValueFunc(ModifyItemValue);
	m_xSubPropList.SetButtonClickFunc(ButtonClickItem);
	//
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pSonItem=NULL,*pRoot=m_xSubPropList.GetRootItem();
	char sText[MAX_TEMP_CHAR_100+1]="";
	CItemInfo* lpInfo = NULL;
	pGroupItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z"),NULL);
	if(pGroupItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_STATIC;
		lpInfo->m_strPropName = "����������Ʋ���";
		lpInfo->m_strPropHelp = "����������Ʋ���";
		pGroupItem = m_xSubPropList.InsertItem(pRoot,lpInfo, -1,bUpdate);
		pGroupItem->m_idProp =GetPropID("design_axis_z");
	}
	//���߼��㷽ʽ
	pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.norm_style"),NULL);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbItems = "0.ָ������|1.���Ǹ�֫����|2.�˼������淨��|3.������������|4.�����ϵ���Է���|5.�˼��ϵ���������";
		lpInfo->m_strPropName = "������㷽ʽ";
		lpInfo->m_strPropHelp = lpInfo->m_strPropName;
		pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
		pPropItem->m_idProp =GetPropID("design_axis_z.norm_style");
	}
	if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;

	//���Ǹ�֫����
	if(xVector.norm_style==0)
	{
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hVicePart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.direction"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVectorType"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
	}
	else if(xVector.norm_style==1)
	{
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVectorType"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "��׼�Ǹ�";
			lpInfo->m_strPropHelp = "��׼�Ǹ־��";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.norm_wing"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbItems ="0.X֫����|1.Y֫����";
			lpInfo->m_strPropName = "��׼֫";
			lpInfo->m_strPropHelp = "��׼֫";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.norm_wing");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.direction"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.direction");
		}
		pPropItem->m_lpNodeInfo->m_cmbItems ="0.����|1.����";
		pPropItem->m_lpNodeInfo->m_strPropName = "����";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "����";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(xVector.norm_style==2)
	{	//����˼�ƽ�涨��
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.direction"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "��׼�˼�";
			lpInfo->m_strPropHelp = "��׼�˼����";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hCrossPart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "������ո˼�";
			lpInfo->m_strPropHelp = "������ո˼����";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo,2,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hCrossPart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVectorType"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.nearVectorType");
		}
		pPropItem->m_lpNodeInfo->m_strPropName = "���Ʒ���";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "���Ʒ�������������㷽��Ϊ������������߻��Ǹ����߷���";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(xVector.nearVector);
		if(nearVectorType==0)	//�Զ���
		{
			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.x"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X����";
				lpInfo->m_strPropHelp = "X����";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.x");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.y"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y����";
				lpInfo->m_strPropHelp = "Y����";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.y");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.z"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z����";
				lpInfo->m_strPropHelp = "Z����";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.z");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		else
		{
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		}
	}
	else if(xVector.norm_style==3)
	{	//������������
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVectorType"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "��׼����";
			lpInfo->m_strPropHelp = "��׼����";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.direction"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.direction");
		}
		pPropItem->m_lpNodeInfo->m_cmbItems ="0.����|1.����";
		pPropItem->m_lpNodeInfo->m_strPropName = "����";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "������ָ��׼���������������߷��򣻷�����ָ��׼�����ĸ��������߷���";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else if(xVector.norm_style==4)
	{	//��������Է���
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.direction"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "��׼����";
			lpInfo->m_strPropHelp = "��׼����,ʮ�ֲ��ʱ0x0��ʾ��׼�����ǲ��Ļ���";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVectorType"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.nearVectorType");
		}
		pPropItem->m_lpNodeInfo->m_strPropName = "��Է���";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "��׼����װ������ϵ�µ���Է���";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;

		int nearVectorType=CStdVector::GetVectorType(xVector.nearVector);
		if(nearVectorType==0)	//�Զ���
		{
			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.x"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X����";
				lpInfo->m_strPropHelp = "X����";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.x");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.y"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y����";
				lpInfo->m_strPropHelp = "Y����";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.y");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;

			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.nearVector.z"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z����";
				lpInfo->m_strPropHelp = "Z����";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp =GetPropID("design_axis_z.nearVector.z");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		else
		{
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		}
	}
	else if(xVector.norm_style==5)
	{	//5.��׼�˼��ϵ���������
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.hCrossPart"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.norm_wing"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVectorType"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.x"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.y"));
		m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.nearVector.z"));
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.hVicePart"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_strPropName = "��׼�˼�";
			lpInfo->m_strPropHelp = "��׼�˼����";
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.hVicePart");
		}
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		//��������
		bool bNeedRotAnglePara=false;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(xVector.hVicePart,CLS_LINEPART);
		pPropItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.direction"),NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			pPropItem = m_xSubPropList.InsertItem(pGroupItem,lpInfo, -1,bUpdate);
			pPropItem->m_idProp =GetPropID("design_axis_z.direction");
		}
		pPropItem->m_lpNodeInfo->m_strPropName = "��������";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "0.�˼�ʼ->�շ���,1.�˼���->ʼ����,2.�Ǹ�X֫���߷���,3.�Ǹ�X֫���߸�����,4.�Ǹ�Y֫���߷���,5.�Ǹ�Y֫���߸�����";
		if(pLinePart&&pLinePart->IsAngle())
			pPropItem->m_lpNodeInfo->m_cmbItems ="0.ʼ->��|1.��->ʼ|2.X֫����+|3.X֫����-|4.Y֫����+|5.Y֫����-";
		else if(pLinePart&&pLinePart->IsTube())
		{
			pPropItem->m_lpNodeInfo->m_cmbItems = "0.ʼ->��|1.��->ʼ|2.�Ի�׼����ת|3.�Ժ�������ת";
			if(xVector.direction>=2)
				bNeedRotAnglePara=true;
		}
		else 
			pPropItem->m_lpNodeInfo->m_cmbItems ="0.ʼ->��|1.��->ʼ";
		if(GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bNeedRotAnglePara)
		{
			pSonItem=m_xSubPropList.FindItemByPropId(GetPropID("design_axis_z.rot_ang"),NULL);
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType=CDT_EDIT;
				lpInfo->m_cmbItems = "0|90|180|270";
				lpInfo->m_strPropName = "��ת�Ƕ�";
				if(xVector.direction==2)
					lpInfo->m_strPropHelp="�Ըֹܻ�׼��ƽ�淨�������ߵ���ת�Ƕȡ�";
				else 
					lpInfo->m_strPropHelp="�Ըֹܺ�����ƽ�淨�������ߵ���ת�Ƕȡ�";
				pSonItem = m_xSubPropList.InsertItem(pPropItem,lpInfo, -1,bUpdate);
				pSonItem->m_idProp = GetPropID("design_axis_z.rot_ang");
			}
			if(GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		}
		else
			m_xSubPropList.DeleteItemByPropId(GetPropID("design_axis_z.rot_ang"));
	}
	if(!bUpdate)
		m_xSubPropList.Redraw();
}
CString CDesignTubeEndLjDlg::GetTubeEndLjName()
{
	CString sPartName;
	if(m_iTubeEndLjStyle==0)
		sPartName = "ƽ���и�";
	else if(m_iTubeEndLjStyle==1)
		sPartName = "�ֹ����";
	if(m_iTubeEndLjStyle==2)
		sPartName = "����";
	else if(m_iTubeEndLjStyle==3)
		sPartName = "U�Ͳ��";
	else if(m_iTubeEndLjStyle==4)
		sPartName = "���Ͳ��";
	else if(m_iTubeEndLjStyle==5)
		sPartName = "ʮ�ֲ��";
	return sPartName;
}
void CDesignTubeEndLjDlg::UpdateBaseInfoProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//
	CPropTreeItem* pPropItem=NULL;
	if(m_iTubeEndLjStyle==5)
	{	
		if(m_iPartType>0)
		{	//��׼������
			pPropItem=oper.InsertEditPropItem(pParentItem,"sCodeName","������","",-1,TRUE);
			pPropItem->SetReadOnly(TRUE);
		}
		//������
		pPropItem=oper.InsertEditPropItem(pParentItem,"sPartNo","������","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
		//������
		pPropItem=oper.InsertEditPropItem(pParentItem,"sWeldPlatePartNo","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
		//������
		pPropItem=oper.InsertEditPropItem(pParentItem,"sWrapPlatePartNo","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
	}
	else
	{
		pPropItem=oper.InsertEditPropItem(pParentItem,"sPartNo","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
		//�Խӷ������ wht 10-09-29
		if(m_iTubeEndLjStyle==2&&m_bCoupleFLCreate&&m_pRelatedTube)
		{
			pPropItem=oper.InsertEditPropItem(pParentItem,"sCoupleFlPlateNo","","",-1,TRUE);
			pPropItem->SetReadOnly(m_iPartType>0);
		}
	}
	//
	oper.InsertEditPropItem(pParentItem,"iSeg","","",-1,TRUE);
	//
	pPropItem=oper.InsertCmbListPropItem(pParentItem,"cMaterial","","","",-1,TRUE);
	pPropItem->SetReadOnly(m_iPartType>0);
	//���	
	pPropItem=oper.InsertEditPropItem(pParentItem,"thick","","",-1,TRUE);
	pPropItem->SetReadOnly(m_iPartType>0);
	//
	oper.InsertEditPropItem(pParentItem,"m_uStatMatCoef","","",-1,TRUE);
}
void CDesignTubeEndLjDlg::UpdateTubeEndProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//
	oper.InsertBtnEditPropItem(pParentItem,"m_fWeldHeight","","",-1,TRUE);
	//
	CPropTreeItem* pPropItem=oper.InsertBtnEditPropItem(pParentItem,"m_fOddment","","",-1,TRUE);
	if(m_bStartEnd)
	{	//�ֹ�ʼ������ͷ
		pPropItem->m_lpNodeInfo->m_strPropName = "ʼ������ͷ";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "�ֹ�ʼ������ͷ,������Ϊ����ʱ�ֹ�����ͷ=�ֹܳ�ʼ������ͷ-�����߳��ֹܲ��ֵĸ߶ȡ�";
	}
	else
	{	//�ֹ��ն�����ͷ
		pPropItem->m_lpNodeInfo->m_strPropName = "�ն�����ͷ";
		pPropItem->m_lpNodeInfo->m_strPropHelp = "�ֹ��ն�����ͷ,������Ϊ����ʱ�ֹ�����ͷ=�ֹܳ�ʼ������ͷ-�����߳��ֹܲ��ֵĸ߶ȡ�";
	}
	pPropItem->SetReadOnly(IsHasExistEndLj());
	//�ֹܷ���ƫ����
	oper.InsertEditPropItem(pParentItem,"m_fTubeNormOffset","","",-1,TRUE);
	//�ֹ�ƫ�ľ�
	oper.InsertEditPropItem(pParentItem,"m_fTubeEccentricDist","","",-1,TRUE);
}
void CDesignTubeEndLjDlg::UpdateSetUpProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//
	CString sPartName=GetTubeEndLjName();
	CPropTreeItem* pPropItem=NULL;
	if(m_iTubeEndLjStyle==2)
	{	//H1

	}
	else if(m_iTubeEndLjStyle>2&&m_iTubeEndLjStyle<5)
	{	//������ʱ����ʾX����ת�Ƕ��Լ�����ƫ���� wht 10-07-01
		if(m_iTubeEndLjStyle==4)
		{
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"design_axis_z.rot_ang","","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_cmbItems="����|����|90|270|";
			pPropItem->m_lpNodeInfo->m_strPropName = "��߷���";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("���⳯�ڱ�ʾ���Ͳ���߳���(����ڵ�ǰ��ͼ����)���Ƕȱ�ʾ��幤���������ֹܻ�׼��֮�����ת�Ƕ�");
		}
		else
		{	//��������ת�Ƕ�
			pPropItem=oper.InsertCmbEditPropItem(pParentItem,"design_axis_z.rot_ang","","","",-1,TRUE);
			pPropItem->m_lpNodeInfo->m_cmbItems="0|90|180|270|";
			pPropItem->m_lpNodeInfo->m_strPropName = "��������ת�Ƕ�";
			pPropItem->m_lpNodeInfo->m_strPropHelp.Format("%s�����������ֹܻ�׼����ת�Ƕ�",sPartName);
		}
		//�����淨��ƫ����
		oper.InsertEditPropItem(pParentItem,"m_fNormOffset","","",-1,TRUE);
	}
	else if(m_iTubeEndLjStyle==5)
	{	//ʮ�ֲ��
		//���幤������
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"MotherPlateNorm","","","",-1,TRUE);
		pPropItem->m_bHideChildren=TRUE;
		//���巨��X�������
		CPropTreeItem* pSonPropItem=oper.InsertEditPropItem(pPropItem,"MotherPlateNorm.x","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//���巨��Y�������
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"MotherPlateNorm.y","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//���巨��Z�������
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"MotherPlateNorm.z","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//1��(��)���幤������
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"UpVertPlateNorm","","","",-1,TRUE);
		pPropItem->m_bHideChildren=TRUE;
		//1��(��)���巨��X�������
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"UpVertPlateNorm.x","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//1��(��)���巨��Y�������
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"UpVertPlateNorm.y","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//1��(��)���巨��Z�������
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"UpVertPlateNorm.z","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//2��(��)���幤������
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"DownVertPlateNorm","","","",-1,TRUE);
		pPropItem->m_bHideChildren=TRUE;
		//2��(��)���巨��X�������
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"DownVertPlateNorm.x","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//2��(��)���巨��Y�������
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"DownVertPlateNorm.y","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
		//2��(��)���巨��Z�������
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"DownVertPlateNorm.z","","",-1,TRUE);
		pSonPropItem->SetReadOnly();
	}
}
void CDesignTubeEndLjDlg::UpdateBoltProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	CPropTreeItem* pGroupItem=NULL,*pPropItem=NULL;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//��˨������Ϣ
	if(m_iTubeEndLjStyle==2)
	{	//�ֹܰ뾶
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_fTubeR","�ֹ��⾶","",-1,TRUE);
		pPropItem->m_lpNodeInfo->m_strPropValue.Format("%g",m_pCurLineTube->GetDiameter());
		pPropItem->SetReadOnly(TRUE);
	}
	//��˨���ò���
	if(m_iTubeEndLjStyle==2)
	{
		//��˨ֱ��
		pPropItem=oper.InsertCmbEditPropItem(pParentItem,"m_nBoltD","","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType!=0);
		//��˨����
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_nBoltNum","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType!=0);
		//��˨����Բ�뾶
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_fBoltLayoutD","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType!=0);
		//��˨����
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_iDriection","","",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType!=0);
		//��ʼ��
		if(m_pCurLineTube->m_bHasWeldRoad&&m_iPartType==0)
			oper.InsertCmbListPropItem(pParentItem,"m_iStartEdge","","","",-1,TRUE);
		//����˨ƫ�ƽ� 
		//��������˨ʱ��˨ƫ�ƽǶ�ָ����ڶ�λ��׼����X���ƫ��
		//�˴�������˨ƫ�ƽǶȱ�����'X'������ wht 11-05-26
		if(m_iPartType==0)
			oper.InsertEditPropItem(pParentItem,"m_fOffsetAngle","","",-1,TRUE);
	}
	else 
	{	//�����˨���ò���
		pPropItem=oper.InsertCmbEditPropItem(pParentItem,"m_nBoltD","","","",-1,TRUE);	//��˨ֱ��
		pPropItem->SetReadOnly(m_iPartType>0);
		//��˨����
		if(m_iTubeEndLjStyle==5)
			pPropItem=oper.InsertEditPropItem(pParentItem,"m_nBoltNum","1/4��˨��","1/4��˨��",-1,TRUE);
		else
			pPropItem=oper.InsertEditPropItem(pParentItem,"m_nBoltNum","��˨����","��˨����",-1,TRUE);
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_nRowNum","","",-1,TRUE);	//��˨����
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_nColNum","","",-1,TRUE);	//��˨����
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_fRowSpace","","",-1,TRUE);	//��˨�о�
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_fColSpace","","",-1,TRUE);	//��˨�о�
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_posFirstBolt.x","","",-1,TRUE);	//����˨X����
		pPropItem->SetReadOnly(m_iPartType>0);
		pPropItem=oper.InsertEditPropItem(pParentItem,"m_posFirstBolt.y","","",-1,TRUE);	//����˨Y����
		pPropItem->SetReadOnly(m_iPartType>0);
		if(m_iTubeEndLjStyle==5)
		{	//ʮ�ֲ��
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_bStaggerLayout","���в���ʱ֧�ֽ�����","","",-1,TRUE);	//������ˮƽ������ֱ����˨
			pPropItem->SetReadOnly(m_iPartType>0||LayoutBoltPara.m_nRowNum>1);
			//pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_bMirLayout","","","",-1,TRUE);	//�ԳƲ���
			//pPropItem->SetReadOnly(m_iPartType>0);
			//if(LayoutBoltPara.m_bMirLayout)
				//oper.InsertEditPropItem(pPropItem,"m_fSpecialSpace","","",-1,TRUE);	//������	
		}
		pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_iDriection","","","",-1,TRUE);	//��˨����
		pPropItem->SetReadOnly(m_iPartType!=0);
	}
}
void CDesignTubeEndLjDlg::UpdatePartTypeProperty(CPropTreeItem* pParentItem)
{
	if(pParentItem==NULL)
		return;
	m_propList.DeleteAllSonItems(pParentItem);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	//
	CPropTreeItem* pPropItem=NULL;
	//��ͨʮ�ֲ��ҲӦ��֧�ְ��ִ��������ʽ wht 10-10-18
	if(m_iTubeEndLjStyle==5)	//ʮ�ֲ��������� 
		oper.InsertCmbListPropItem(pParentItem,"m_iWrapPlateType","","","",-1,TRUE);
	if(m_iTubeEndLjStyle>2&&m_iPartType>0)	//���ѡ�����
		oper.InsertCmbListPropItem(pParentItem,"m_cStdPartMat","","","",-1,TRUE);
	if(m_iTubeEndLjStyle==2&&!m_bHasExistEndLj)
		oper.InsertCmbListPropItem(pParentItem,"m_bCoupleFLCreate","","","",-1,TRUE);
	//����
	if((m_iTubeEndLjStyle==2&&m_iPartType==2)||m_iTubeEndLjStyle>2)
	{
		if(m_iTubeEndLjStyle==2)
		{
			oper.InsertCmbListPropItem(pParentItem,"m_cStdPartMat","","ѡ�����","������ѡ�����(��������)",-1,TRUE);
			if(m_arrStdFlDLevelM.GetSize()>0)
			{
				CXhChar100 sLevelArr;
				pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_iStdPartLevel","","���ܷ�������","",-1,TRUE);
				for(int i=0;i<m_arrStdFlDLevelM.GetSize();i++)
				{
					CXhChar50 sLevel;
					sLevel.Printf("%s|",m_arrStdFlDLevelM.GetAt(i));
					sLevelArr.Append(sLevel);
				}
				pPropItem->m_lpNodeInfo->m_cmbItems.Format("%s",(char*)sLevelArr);
			}
			if(m_arrStdFlDLevelC.GetSize()>0)
			{
				CXhChar100 sLevelArr;
				pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_iStdPartLevel2","","�Խӷ�������","",-1,TRUE);
				pParentItem->SetReadOnly(!m_bCoupleFLCreate);
				for(int i=0;i<m_arrStdFlDLevelC.GetSize();i++)
				{
					CXhChar50 sLevel;
					sLevel.Printf("%s|",m_arrStdFlDLevelC.GetAt(i));
					sLevelArr.Append(sLevel);
				}
				pPropItem->m_lpNodeInfo->m_cmbItems.Format("%s",(char*)sLevelArr);
			}
		}
		else
		{
			CXhChar100 sLevelArr;
			pPropItem=oper.InsertCmbListPropItem(pParentItem,"m_iStdPartLevel","","��弶��","",-1,TRUE);
			for(int i=0;i<m_arrStdPartLevel.GetSize();i++)
			{
				CXhChar50 sLevel;
				sLevel.Printf("%s|",m_arrStdPartLevel.GetAt(i));
				sLevelArr.Append(sLevel);
			}
			pPropItem->m_lpNodeInfo->m_cmbItems.Format("%s",(char*)sLevelArr);
		}
	}
}
void CDesignTubeEndLjDlg::DisplayPropertyList(int iCurSel/*=0*/)
{
	const int GROUP_GENERAL=1,GROUP_PROFILE=2,GROUP_LAYOUT_BOLT=3,GROUP_ATTACH_PLATE=4;
	m_arrPropGroupLabel.RemoveAll();
	if(m_iTubeEndLjStyle<2)
		m_arrPropGroupLabel.SetSize(0);
	else //if(m_iTubeEndLjStyle>=2)
	{
		if(m_iTubeEndLjStyle==2)
		{
			if(!IsHasExistEndLj()&&(m_iPartType==0||m_iPartType==3||m_iPartType==4))
				m_arrPropGroupLabel.SetSize(4);
			else 
				m_arrPropGroupLabel.SetSize(3);
			m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
			m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"����");
			m_arrPropGroupLabel.SetAt(GROUP_LAYOUT_BOLT-1,"��˨");
			if(!IsHasExistEndLj()&&(m_iPartType==0||m_iPartType==3||m_iPartType==4))
				m_arrPropGroupLabel.SetAt(GROUP_ATTACH_PLATE-1,"���Ӱ�");
		}
		else 
		{
			int nGroupCount=0;
			if(!IsHasExistEndLj()&&(m_iTubeEndLjStyle==4||m_iTubeEndLjStyle==5))
				nGroupCount=4;
			else
				nGroupCount=3;
			if(!IsHasExistEndLj()&&m_bDesignNodePlate)
				nGroupCount++;
			m_arrPropGroupLabel.SetSize(nGroupCount);
			m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
			m_arrPropGroupLabel.SetAt(GROUP_PROFILE-1,"����");
			m_arrPropGroupLabel.SetAt(GROUP_LAYOUT_BOLT-1,"��˨");
			if(!IsHasExistEndLj()&&(m_iTubeEndLjStyle==4||m_iTubeEndLjStyle==5))	//��߲��|ʮ�ֲ����Ҫ��Ƹ��Ӱ�
				m_arrPropGroupLabel.SetAt(GROUP_ATTACH_PLATE-1,"���Ӱ�");
			if(!IsHasExistEndLj()&&m_bDesignNodePlate)		//�ڵ������λ�����һ�������� wht 11-04-12
				m_arrPropGroupLabel.SetAt(nGroupCount-1,"�ڵ��");
		}	
	}
	RefreshTabCtrl(iCurSel);
	//�����������ص�����
	m_propList.CleanTree();
	m_propList.SetModifyValueFunc(ModifyItemValue);
	m_propList.SetButtonClickFunc(ButtonClickItem);
	m_propList.SetPropHelpPromptFunc(PropHelpPrompt);
	m_propList.SetPopMenuClickFunc(FirePopMenuClick);
	CPropertyListOper<CDesignTubeEndLjDlg> oper(&m_propList,this);
	CPropTreeItem *pPropItem,*pGroupItem,*pRoot=m_propList.GetRootItem();
	CString sPartName=GetTubeEndLjName();
	if(m_iTubeEndLjStyle<=1)
	{	
		if(m_iTubeEndLjStyle==0)
		{	//ƽ���и�
			oper.InsertBtnEditPropItem(pRoot,"m_hWeldLinkObj");
			pGroupItem=oper.InsertGrayButtonPropItem(pRoot,"cutPlaneOrigin");
			pGroupItem->m_bHideChildren = FALSE;
			//X
			oper.InsertEditPropItem(pGroupItem,"cutPlaneOrigin.x");
			//Y
			oper.InsertEditPropItem(pGroupItem,"cutPlaneOrigin.y");
			//Z
			oper.InsertEditPropItem(pGroupItem,"cutPlaneOrigin.z");
			//���߷���
			pGroupItem=oper.InsertGrayButtonPropItem(pRoot,"cutPlaneNorm");
			pGroupItem->m_bHideChildren = FALSE;
			//X
			oper.InsertEditPropItem(pGroupItem,"cutPlaneNorm.x");
			//Y
			oper.InsertEditPropItem(pGroupItem,"cutPlaneNorm.y");
			//Z
			oper.InsertEditPropItem(pGroupItem,"cutPlaneNorm.z");
		}
		else
		{	//�ֹ����
			pGroupItem=oper.InsertPropItem(pRoot,"TubeTransect");
			pGroupItem->m_bHideChildren = FALSE;
			//�Խ����
			oper.InsertCmbListPropItem(pGroupItem,"m_bFaceToFace");
			//���ֹ�1
			oper.InsertBtnEditPropItem(pGroupItem,"m_hDatumTube1");
			if(!m_bFaceToFace)	//���ֹ�2
				oper.InsertBtnEditPropItem(pGroupItem,"m_hDatumTube2");
		}
		//�ֹܶ�����λ��
		pGroupItem=oper.InsertPropItem(pRoot,"TubeEndPos");
		pGroupItem->m_bHideChildren=FALSE;
		//����ͷ
		pPropItem=oper.InsertButtonPropItem(pGroupItem,"m_fOddment");
		pPropItem->m_lpNodeInfo->m_strPropName=(m_bStartEnd)?"ʼ������ͷ":"�ն�����ͷ";
		//�ֹ�ƫ�ľ�
		if(m_iTubeEndLjStyle==0)
			oper.InsertEditPropItem(pGroupItem,"m_fTubeEccentricDist");
	}
	else
	{
		if((m_iTubeEndLjStyle==2&&(m_pFlDParam||m_pFlPParam||m_pFlexibleFlParam||m_pRigidFlParam))
			||(m_iTubeEndLjStyle>2&&m_pInsertPlateParam))
		{	//���ڱ�׼����
			pGroupItem=oper.InsertCmbListPropItem(pRoot,"m_iPartType");
			pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if(m_iTubeEndLjStyle==2)
			{
				CXhChar100 strItem("0.��ͨ����|");
				if(m_pFlPParam)
					strItem.Append("1.ƽ������|");
				if(m_pFlDParam)
					strItem.Append("2.�Ժ�����|");
				if(m_pRigidFlParam)
					strItem.Append("3.���Է���|");
				if(m_pFlexibleFlParam)
					strItem.Append("4.���Է���|");
				pGroupItem->m_lpNodeInfo->m_cmbItems.Format("%s",(char*)strItem);
			}
			else 
				pGroupItem->m_lpNodeInfo->m_cmbItems = "0.�Զ�����|1.��׼���";
			//
			UpdatePartTypeProperty(pGroupItem);
		}
		//������Ϣ
		pGroupItem=oper.InsertPropItem(pRoot,"basicInfo");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		UpdateBaseInfoProperty(pGroupItem);
		//�ֹܶ�����λ��
		pGroupItem=oper.InsertPropItem(pRoot,"TubeEndPos");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		UpdateTubeEndProperty(pGroupItem);
		//װ�����
		pGroupItem=oper.InsertPropItem(pRoot,"SetUpPara");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		UpdateSetUpProperty(pGroupItem);
		//���β���
		pGroupItem=oper.InsertPropItem(pRoot,"Profile");
		pGroupItem->m_bHideChildren=FALSE;//(m_iPartType==0);
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_PROFILE);
		UpdateProfileProperty(pGroupItem);
		//��˨���ò���
		if(m_iTubeEndLjStyle>2)
			pGroupItem=oper.InsertGrayPopMenuItem(pRoot,"LayoutBoltParam");
		else
			pGroupItem=oper.InsertPropItem(pRoot,"LayoutBoltParam");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_LAYOUT_BOLT);
		UpdateBoltProperty(pGroupItem);
		//���Ӱ����
		if((m_iTubeEndLjStyle==2&&(m_iPartType==0||m_iPartType==3||m_iPartType==4))||
			m_iTubeEndLjStyle==5||m_iTubeEndLjStyle==4)
		{
			pGroupItem=oper.InsertCmbListPropItem(pRoot,"m_bCreateAttachPlate");
			pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_ATTACH_PLATE);
			pGroupItem->SetReadOnly(GetPartType()>2);	//���Է��������Է������������޸�
			UpdateAttachPlateProperty(pGroupItem);
			//������ wht 11-05-11
			if(m_iTubeEndLjStyle==5)
			{
				pGroupItem=oper.InsertCmbListPropItem(pRoot,"m_bCreateFillPlate");
				pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_ATTACH_PLATE);
				//��ʾ������
				UpdateFillPlateProperty(pGroupItem);
			}
		}
		//
		if(m_bDesignNodePlate)
		{	//�ڵ�����
			pGroupItem=oper.InsertPropItem(pRoot,"NodePlateBasicInfo");
			pGroupItem->m_bHideChildren=FALSE;
			pGroupItem->m_dwPropGroup=GetSingleWord(m_arrPropGroupLabel.GetSize());
			//
			oper.InsertEditPropItem(pGroupItem,"NodePlatePara.sPartNo");
			//
			oper.InsertEditPropItem(pGroupItem,"NodePlatePara.cMaterial");
			//
			oper.InsertEditPropItem(pGroupItem,"NodePlatePara.m_nThick");
		}
	}
	m_propList.Redraw();
}

void CDesignTubeEndLjDlg::OnSelchangeTubeEndljStyle() 
{
	int iOldStyle=m_iTubeEndLjStyle;
	UpdateData();
	if(m_bDesignNodePlate&&m_iTubeEndLjStyle<3)
	{	//���߸˼��������ò��
		m_iTubeEndLjStyle=iOldStyle;
		AfxMessageBox("���߸˼���������ʽֻ����Ϊ��壡");
		UpdateData(FALSE);
	}
	UpdateDesignParam();
	if(m_iTubeEndLjStyle==5&&setupInfo.workNorm.hVicePart==0)	//ʮ�ֲ��
	{
		setupInfo.workNorm.norm_style=5;
		setupInfo.workNorm.rot_ang=0;
		setupInfo.workNorm.hVicePart=m_pCurLineTube->handle;
		setupInfo.workNorm.UpdateVector(console.GetActiveModel());
		setupInfo.workNormUpVert.norm_style=setupInfo.workNormDownVert.norm_style=4;
		setupInfo.workNormUpVert.vector=setupInfo.workNormUpVert.nearVector;
		setupInfo.workNormDownVert.vector=setupInfo.workNormDownVert.nearVector;
		if(m_pCurLineTube->pStart==m_pBaseNode)
			m_paramPlate.ucs.axis_x=m_pCurLineTube->Start()-m_pCurLineTube->End();
		else
			m_paramPlate.ucs.axis_x=m_pCurLineTube->End()-m_pCurLineTube->Start();
		m_paramPlate.ucs.axis_z=setupInfo.workNorm.vector;
		m_paramPlate.ucs.axis_y=m_paramPlate.ucs.axis_z^m_paramPlate.ucs.axis_x;
		normalize(m_paramPlate.ucs.axis_y);
		m_paramPlate.ucs.axis_x=m_paramPlate.ucs.axis_y^m_paramPlate.ucs.axis_z;
		normalize(m_paramPlate.ucs.axis_x);
		vector_trans(setupInfo.workNormUpVert.vector,m_paramPlate.ucs,TRUE);
		vector_trans(setupInfo.workNormDownVert.vector,m_paramPlate.ucs,TRUE);
	}
	UpdateLayoutBoltPara();
	DisplayPropertyList();
	UpdateSketchMap();
	//
	RECT rc,rc1,rc2;
	GetDlgItem(IDC_S_PIC_PLATE_TYPE)->GetWindowRect(&rc1);
	GetDlgItem(IDC_S_MAP)->GetWindowRect(&rc2);
	//�����ܸ���5�����أ�����ˢ�������ֱ�ע�����Ĳ���
	rc.left = min(rc1.left,rc2.left)-5;
	rc.top = min(rc1.top,rc2.top)-5;
	rc.right=max(rc1.right,rc2.right)+5;
	rc.bottom=max(rc1.bottom,rc2.bottom)+5;
	ScreenToClient(&rc);
	UpdateData(FALSE);
	InvalidateRect(&rc,TRUE);
}

void CDesignTubeEndLjDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurSel=m_ctrlPropGroup.GetCurSel();
	m_propList.m_iPropGroup = iCurSel;
	m_propList.Redraw();
	//����ʾ��ͼ
	UpdateSketchMap(iCurSel);
	*pResult = 0;
}

void CDesignTubeEndLjDlg::RefreshTabCtrl(int iCurSel)
{
	m_ctrlPropGroup.DeleteAllItems();
	for(int i=0;i<m_arrPropGroupLabel.GetSize();i++)
		m_ctrlPropGroup.InsertItem(i,m_arrPropGroupLabel[i]);
	if(m_arrPropGroupLabel.GetSize()>0)
	{
		m_ctrlPropGroup.SetCurSel(iCurSel);
		m_propList.m_iPropGroup=iCurSel;
	}
	else //����Ҫ������ʾ
		m_propList.m_iPropGroup=0;
	
	//���ݷ�������������λ��
	RECT rcPropList,rcMainWnd;
	GetClientRect(&rcMainWnd);
	CWnd *pTopWnd = GetDlgItem(IDC_LIST_BOX);
	if(pTopWnd)
		pTopWnd->GetWindowRect(&rcPropList);
	ScreenToClient(&rcPropList);
	if(m_arrPropGroupLabel.GetSize()>0)
		rcPropList.top=rcMainWnd.top+60;
	else 
		rcPropList.top=rcMainWnd.top+35;
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcPropList);
}
void CDesignTubeEndLjDlg::Redraw()
{
	CRect rect;
	GetDlgItem(IDC_S_MAP)->GetWindowRect(&rect);
	rect.top-=100;
	InvalidateRect(rect,FALSE);
}
void CDesignTubeEndLjDlg::RefreshCtrlState(BOOL bShowBolt/*=TRUE*/)
{
	RECT rc;
	GetDlgItem(IDC_WORK_PANEL)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_xSubPropList.MoveWindow(&rc);
	//
	m_listBoltParam.ShowWindow(bShowBolt);
	m_listBolt.ShowWindow(bShowBolt);
	m_xSubPropList.ShowWindow(!bShowBolt);
	GetDlgItem(IDC_BTN_PARTS_DB)->ShowWindow(!m_bHasExistEndLj);
	GetDlgItem(IDC_BTN_DEL_CUR_ENDLJ)->ShowWindow(m_bHasExistEndLj);
	GetDlgItem(IDC_CMB_TUBE_ENDLJ_STYLE)->EnableWindow(!m_bSpecEndLjStyle&&!m_bHasExistEndLj);
}
void CDesignTubeEndLjDlg::RefreshBoltListColumn()
{
	//ˢ�±�����
	m_listBolt.m_arrHeader.RemoveAll();
	if(m_iTubeEndLjStyle==2)
	{	//
		m_listBolt.AddColumnHeader("ֱ��");
		m_listBolt.AddColumnHeader("R");
		m_listBolt.AddColumnHeader("�Ƕ�");
		long col_wide_arr[3]={50,100,100};
		m_listBolt.InitListCtrl(col_wide_arr);
	}
	else
	{
		m_listBolt.AddColumnHeader("ֱ��");
		m_listBolt.AddColumnHeader("X");
		m_listBolt.AddColumnHeader("Y");
		m_listBolt.AddColumnHeader("Z");
		long col_wide_arr[4]={50,70,70,60};
		m_listBolt.InitListCtrl(col_wide_arr);
	}
}
void CDesignTubeEndLjDlg::RefreshBoltParaListColumn()
{
	m_listBoltParam.EmptyColumnHeader();
	if(m_iTubeEndLjStyle==2)
	{	//����������
		m_listBoltParam.AddColumnHeader("��˨ֱ��");
		m_listBoltParam.AddColumnHeader("��˨����");
		m_listBoltParam.AddColumnHeader("����Բֱ��");
		long para_col_wide_arr[3]={92,92,92};
		m_listBoltParam.InitListCtrl(para_col_wide_arr);
	}
	else
	{	//��������
		m_listBoltParam.AddColumnHeader("S");
		m_listBoltParam.AddColumnHeader("R");
		m_listBoltParam.AddColumnHeader("C");
		m_listBoltParam.AddColumnHeader("ֱ��");
		m_listBoltParam.AddColumnHeader("�о�");
		m_listBoltParam.AddColumnHeader("�о�");
		m_listBoltParam.AddColumnHeader("X");
		m_listBoltParam.AddColumnHeader("Y");
		long para_col_wide_arr[8]={25,30,30,40,40,40,45,45};
		m_listBoltParam.InitListCtrl(para_col_wide_arr);
	}
	
}
void CDesignTubeEndLjDlg::RefreshBoltParamList()
{
	RefreshBoltParaListColumn();
	//
	CString sTextValue;
	m_listBoltParam.DeleteAllItems();
	if(m_iTubeEndLjStyle==2)
	{	//����
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		//ֱ��
		sTextValue.Format("%d",LayoutBoltPara.m_nBoltD);
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetControlType(0,GCT_CMB_EDIT);
		lpInfo->SetListItemsStr(0,"12|16|20|24|27|30|");
		lpInfo->SetSubItemReadOnly(0,m_iPartType>0);
		//��˨����
		sTextValue.Format("%d",LayoutBoltPara.m_nBoltNum);		
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetSubItemReadOnly(1,m_iPartType>0);
		//��˨����Բֱ��
		sTextValue.Format("%f",LayoutBoltPara.m_fBoltLayoutD);
		SimplifiedNumString(sTextValue);
		lpInfo->AddSubItemText(sTextValue);
		lpInfo->SetSubItemReadOnly(2,m_iPartType>0);
		//
		m_listBoltParam.InsertRootItem(lpInfo);
	}
	else if(m_iTubeEndLjStyle>2)
	{	//���
		for(LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.GetFirst();pBoltPara;pBoltPara=LayoutBoltParaList.GetNext())
		{
			CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
			//��˨����
			sTextValue.Format("%d",pBoltPara->m_nBoltNum);		
			lpInfo->AddSubItemText(sTextValue);	
			lpInfo->SetSubItemHelpText(0,"S��ʾ��˨����");
			lpInfo->SetSubItemReadOnly(0,m_iPartType>0);
			//��˨����
			sTextValue.Format("%d",pBoltPara->m_nRowNum);		
			lpInfo->AddSubItemText(sTextValue,m_iTubeEndLjStyle==5&&sTextValue.CompareNoCase("-1")==0);
			lpInfo->SetSubItemHelpText(1,"R==-1����C==-1��ʾˮƽ������ֱ�潻������˨");
			lpInfo->SetSubItemReadOnly(1,m_iPartType>0);
			//��˨����
			sTextValue.Format("%d",pBoltPara->m_nColNum);		
			lpInfo->AddSubItemText(sTextValue,m_iTubeEndLjStyle==5&&sTextValue.CompareNoCase("-1")==0);
			lpInfo->SetSubItemHelpText(2,"R==-1����C==-1��ʾˮƽ������ֱ�潻������˨");
			lpInfo->SetSubItemReadOnly(2,m_iPartType>0);
			//ֱ��
			sTextValue.Format("%d",pBoltPara->m_nBoltD);
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetControlType(3,GCT_CMB_EDIT);
			lpInfo->SetListItemsStr(3,"12|16|20|24|27|30|");
			lpInfo->SetSubItemReadOnly(3,m_iPartType>0);
			//�о�
			sTextValue.Format("%.f",pBoltPara->m_fRowSpace);
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetSubItemHelpText(4,"�о�");
			lpInfo->SetSubItemReadOnly(4,m_iPartType>0);
			//�о�
			sTextValue.Format("%.f",pBoltPara->m_fColSpace);		
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetSubItemHelpText(5,"�о�");
			lpInfo->SetSubItemReadOnly(5,m_iPartType>0);
			//X
			sTextValue.Format("%.f",pBoltPara->m_posFirstBolt.x);		
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetSubItemHelpText(6,"����˨X����");
			lpInfo->SetSubItemReadOnly(6,m_iPartType>0);
			//Y
			sTextValue.Format("%.f",pBoltPara->m_posFirstBolt.y);
			lpInfo->AddSubItemText(sTextValue);
			lpInfo->SetSubItemHelpText(7,"����˨Y����");
			lpInfo->SetSubItemReadOnly(7,m_iPartType>0);
			//
			CSuperGridCtrl::CTreeItem* pItem=m_listBoltParam.InsertRootItem(lpInfo);
			if(pItem)
				pItem->m_idProp=(DWORD)pBoltPara;;
		}
	}
	m_listBoltParam.Redraw();
}
void CDesignTubeEndLjDlg::RefreshBoltList()
{
	RefreshBoltListColumn();
	//
	m_listBolt.DeleteAllItems();//�����˨�б�
	CStringArray str_arr;
	if(m_iTubeEndLjStyle==2)
	{	//����
		str_arr.SetSize(3);
		double fInitAngle=(m_iPartType==0)?LayoutBoltPara.m_fOffsetAngle:0;;
		double fPieceAngle=360.0/LayoutBoltPara.m_nBoltNum;
		for(int i=0;i<LayoutBoltPara.m_nBoltNum;i++)
		{
			str_arr[0].Format("M%d",LayoutBoltPara.m_nBoltD);
			str_arr[1].Format("%f",LayoutBoltPara.m_fBoltLayoutD*0.5);
			SimplifiedNumString(str_arr[1]);
			str_arr[2].Format("%f",fInitAngle+fPieceAngle*i);
			SimplifiedNumString(str_arr[2]);
			m_listBolt.InsertItemRecord(-1,str_arr);
		}
	}
	else if(m_iTubeEndLjStyle>2)
	{	//���
		//������˨���ò���������˨����
		ATOM_LIST<f3dPoint> boltPts;
		LayoutBolt(boltPts);
		//
		str_arr.SetSize(4);
		int nBoltNum=0;
		for(f3dPoint *pPt=boltPts.GetFirst();pPt;pPt=boltPts.GetNext())
		{
			nBoltNum++;
			//������˨���ж���˨���ڵڼ������
			DWORD dwFlag=0;
			int nSumBolt=0,index=0;
			for(index=0;index<LayoutBoltParaList.GetNodeNum();index++)
			{
				if(m_iTubeEndLjStyle==5)	//ʮ�ֲ��
					nSumBolt+=LayoutBoltParaList[index].m_nBoltNum*4;
				else 
					nSumBolt+=LayoutBoltParaList[index].m_nBoltNum;
				if(nBoltNum<=nSumBolt)
				{	//��1��ʼ,��Ϊ0û������,�˴���Ҫʹ��������,��ʾ�ڼ�����˨
					dwFlag=index+1;
					break;
				}
			}
			if(m_iTubeEndLjStyle==5)	//ʮ�ֲ��,X-Y���ϵ���˨Ϊ����X-Z���ϵ���˨Ϊ��
			{
				WORD* pwFlg=(WORD*)&dwFlag;
				if(pPt->feature==31||pPt->feature==41)
					(*pwFlg)*=100;
				else
					(*pwFlg)*=1;
				*(pwFlg+1)=(WORD)pPt->feature;
			}
			//
			str_arr[0].Format("M%d",LayoutBoltParaList[index].m_nBoltD);
			str_arr[1].Format("%f",pPt->x);
			SimplifiedNumString(str_arr[1]);
			str_arr[2].Format("%f",pPt->y);
			SimplifiedNumString(str_arr[2]);
			str_arr[3].Format("%f",pPt->z);
			SimplifiedNumString(str_arr[3]);
			int iItem=m_listBolt.InsertItemRecord(-1,str_arr);
			m_listBolt.SetItemData(iItem,dwFlag);		//��˨λ�ñ�־
		}
	}
}
//���ɸ��Ӹְ�
void CDesignTubeEndLjDlg::CreateAttachPlate(CLDSParamPlate *pParamPalte,CLDSLineTube *pDatumTube,BOOL bFlCoupleDesign/*=FALSE*/)
{
	CXhChar100 sText;
	int nCount=AttachPlatePara.m_nPlateNum;
	double sector_angle=360.0/nCount;
	for(int i=0;i<nCount;i++)
	{
		CLDSParamPlate *pElbowPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
		pElbowPlate->SetLayer(pDatumTube->layer()); //����ͼ����
		pElbowPlate->cfgword=pDatumTube->cfgword;
		pElbowPlate->m_iParamType = TYPE_ELBOWPLATE;
		pElbowPlate->m_hPartWeldParent = pParamPalte->handle;
		pElbowPlate->cfgword=pParamPalte->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pElbowPlate->SetDesignItemValue('A',AttachPlatePara.A,"������A",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('B',AttachPlatePara.B,"������B",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('C',AttachPlatePara.C,"������C",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('D',AttachPlatePara.D,"������D",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('E',AttachPlatePara.E,"������E",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('F',AttachPlatePara.F,"������F",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('G',AttachPlatePara.G,"������G",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('H',AttachPlatePara.H,"������H",PARAM_SHAPE);
		pElbowPlate->SetDesignItemValue('e',AttachPlatePara.m_iCutMode,"",PARAM_SHAPE,1);
		if(pParamPalte->ucs.axis_z*pDatumTube->ucs.axis_z>0)
			pElbowPlate->SetDesignItemValue('K',-pDatumTube->handle,"����������Ļ�׼�ֹ�",PARAM_POS,1);
		else
			pElbowPlate->SetDesignItemValue('K',pDatumTube->handle,"����������Ļ�׼�ֹ�",PARAM_POS,1);
		pElbowPlate->SetDesignItemValue('L',pParamPalte->handle,"��׼�װ�",PARAM_POS,1);
		pElbowPlate->m_bDisplayHole=TRUE;
		//���Ⱦ��Խ�ʱ�����߰��� wht 10-09-29
		double offset_r=0.5*(m_pCurLineTube->GetDiameter()-pDatumTube->GetDiameter());
		if(offset_r>EPS2)
		{
			DESIGN_PLATE_ITEM *pItem=pElbowPlate->GetDesignItem('B');
			if(pItem)	//�߰���
				pElbowPlate->SetDesignItemValue(pItem->key,pItem->value.fValue+offset_r,pItem->sNotes,pItem->paramType);
		}
		pElbowPlate->SetPartNo(AttachPlatePara.sPartNo);	//���÷����߰��� wht 12-08-24
		//�趨����
		pElbowPlate->cMaterial=AttachPlatePara.cMaterial;
		//��λ��
		double angle=AttachPlatePara.m_fAngle+i*sector_angle;
		if(bFlCoupleDesign)
			angle*=-1;
		pElbowPlate->SetDesignItemValue('Y',angle,"��嶨λ����ǡ�");
		pElbowPlate->SetDesignItemValue('b',1,"��嶨λ�ĽǶȵ�����ʽ;0.�ɰ汾�Ըֹ�Ϊ��׼;1.�°汾�������ְ�Ϊ��׼");
		pElbowPlate->iSeg=pParamPalte->iSeg;
		pElbowPlate->SetLayer(pParamPalte->layer());	//��������ͼ����
		pElbowPlate->thick = AttachPlatePara.m_nThick;
		//ͳһ�����ת�����߰崦��  wxc-2017.5.26
		pElbowPlate->ShiftElbowAndRibPlateSchema(TYPE_RIBPLATE);
		pElbowPlate->DesignPlate();
		double scale_user2scr=g_pSolidOper->GetScaleUserToScreen();
		double sample_len=g_pSolidSet->GetArcSamplingLength();
		pElbowPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,scale_user2scr,sample_len,g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pElbowPlate->GetSolidPartObject());
		g_pSolidDraw->Draw();
		pParamPalte->AppendAttachPart(pElbowPlate);
	}
}

void CDesignTubeEndLjDlg::OnConfirmCreate() 
{
	if((m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4||m_iTubeEndLjStyle==5)&&!m_bHasExistEndLj)
	{	//�ж�����ͷ�������ײ��ʾ�û� wht 11-05-11
		DESIGNODDMENT * pCurOddment=&(m_pCurLineTube->desEndOdd);
		if(m_bStartEnd)
			pCurOddment=&(m_pCurLineTube->desStartOdd);
		pCurOddment->m_fCollideDist=setupInfo.m_fWeldHeight+g_sysPara.VertexDist;
		long hOldRefPart1=pCurOddment->m_hRefPart1;	//��¼�ɵ���ײ��׼�˼�
		if(console.FromPartHandle(pCurOddment->m_hRefPart1)==NULL
			&&console.FromPartHandle(pCurOddment->m_hRefPart2)==NULL)
		{	
			if(m_pBaseNode->hFatherPart!=m_pCurLineTube->handle)
				pCurOddment->m_hRefPart1=m_pBaseNode->hFatherPart;
		}
		//������ײ��������ͷ,�˴����û������ť�����ʲ����ж��ж�����ͷ���㷽ʽ 
		double fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate,true);
		pCurOddment->m_hRefPart1=hOldRefPart1;
		if(setupInfo.m_fOddment>fOddment)
		{
			if(AfxMessageBox("�����ֹܿ��ܷ�����ײ,�Ƿ�ͨ��������߶ȡ���İ�ť���㸺ͷ?",MB_YESNO)==IDYES)
			{	//���ݺ���߶�ֱ�Ӽ�������ͷ
				setupInfo.m_fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate,true);
			}
		}
	}
	if(m_iTubeEndLjStyle==5)
	{
		if(m_bDesignNodePlate)	//��ƽڵ��
			FillPlatePara.nNodePlateThick=NodePlatePara.m_nThick;
		if(FillPlatePara.nNodePlateThick>m_paramPlate.thick&&!m_bCreateFillPlate
			&&AfxMessageBox("�ڵ���ȴ���ʮ�ֲ������ȣ��Ƿ���Ҫ�ڸ��Ӱ�����ҳ������������?",MB_YESNO)==IDYES)
		{	//�л������Ӱ�����ҳ���������������� wht 11-05-11
			FillPlatePara.nThick=(int)(0.5*FillPlatePara.nNodePlateThick-0.5*m_paramPlate.thick);
			m_ctrlPropGroup.SetCurSel(2);
			m_propList.m_iPropGroup = 2;
			m_propList.Redraw();
			//����ʾ��ͼ
			CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
			m_CurPlateBmp.Detach();
			m_CurPlateBmp.LoadBitmap(IDB_BMP_XEND_RIB_PLATE1); //���Ӱ�
			pPic->SetBitmap(m_CurPlateBmp);
			//�����������
			m_bCreateFillPlate=TRUE;
			CPropTreeItem* pFindItem=m_propList.FindItemByPropId(CDesignTubeEndLjDlg::GetPropID("m_bCreateFillPlate"),NULL);
			if(pFindItem)
			{
				m_propList.SetItemPropValue(pFindItem->m_idProp,"��");
				UpdateFillPlateProperty(pFindItem);
			}
			return;
		}
	}
	//���η������߸˼� �Զ������ֹܹ����淨�߷���
	if(m_bSpecialFlRayPole&&(m_iTubeEndLjStyle>=3||m_iTubeEndLjStyle<=5))
	{
		m_pCurLineTube->m_iNormCalStyle=0;	//ֱ��ָ�������淨��
		m_pCurLineTube->SetWorkPlaneNorm(f3dPoint(0,0,1));
	}
	if(m_iTubeEndLjStyle==0)
	{	//ƽ���и�
		TUBEJOINT *pCurJoint=NULL;
		if(m_bStartEnd)
		{
			m_pCurLineTube->SetStartOdd(setupInfo.m_fOddment);
			m_pCurLineTube->desStartPos.fNormOffset=setupInfo.m_fNormOffset;
			m_pCurLineTube->desStartPos.fEccentricDist=setupInfo.m_fTubeEccentricDist;
			pCurJoint=&m_pCurLineTube->m_tJointStart;
		}
		else
		{
			m_pCurLineTube->SetEndOdd(setupInfo.m_fOddment);
			m_pCurLineTube->desEndPos.fNormOffset=setupInfo.m_fNormOffset;
			m_pCurLineTube->desEndPos.fEccentricDist=setupInfo.m_fTubeEccentricDist;
			pCurJoint=&m_pCurLineTube->m_tJointEnd;
		}
		pCurJoint->type=m_iTubeEndLjStyle;//ƽ���и�
		pCurJoint->cutPlanePos=cutPlaneOrigin;
		pCurJoint->cutPlaneNorm=cutPlaneNorm;
		pCurJoint->hLinkObj=m_hWeldLinkObj;
	}
	else if(m_iTubeEndLjStyle==1)
	{	//�ֹ����
		long hOldLinkTube=0,hOldViceLinkTube=0,hLinkObj=0,hViceLinkObj=0;
		TUBEJOINT *pCurrentJoint=NULL;
		f3dPoint cur_tube_vec,link_tube_vec,cutPlanePos;
		//ɾ���ֹܶԽ�ʱ���¶ԽӸֹܵĶ�����
		if(m_bStartEnd)
		{
			hOldLinkTube = m_pCurLineTube->m_tJointStart.hLinkObj;
			hOldViceLinkTube = m_pCurLineTube->m_tJointStart.hViceLinkObj;
			cur_tube_vec=m_pCurLineTube->End()-m_pCurLineTube->Start();
			pCurrentJoint=&m_pCurLineTube->m_tJointStart;
			cutPlanePos=m_pCurLineTube->Start();
		}
		else
		{
			hOldLinkTube = m_pCurLineTube->m_tJointEnd.hLinkObj;
			hOldViceLinkTube = m_pCurLineTube->m_tJointEnd.hViceLinkObj;
			cur_tube_vec=m_pCurLineTube->Start()-m_pCurLineTube->End();
			pCurrentJoint=&m_pCurLineTube->m_tJointEnd;
			cutPlanePos=m_pCurLineTube->End();
		}
		normalize(cur_tube_vec);
		hLinkObj=m_hDatumTube1;
		hViceLinkObj=m_hDatumTube2;	
		CLDSLineTube *pOldTube=(CLDSLineTube*)console.FromPartHandle(hOldLinkTube,CLS_LINETUBE);
		if(pOldTube&&hOldViceLinkTube<0)
		{	//�޸�ǰ�ǸֹܶԽ�,��ǰ�ǶԽ����
			if(!m_bFaceToFace||(hLinkObj==0))	//ɾ���ֹܶԽ�
			{	
				if(pOldTube->pStart==m_pBaseNode)
				{
					pOldTube->m_tJointStart.type=0;
					pOldTube->m_tJointStart.hLinkObj=0;
					pOldTube->m_tJointStart.hViceLinkObj=0;
					pOldTube->m_tJointStart.cutPlanePos.Set();
					pOldTube->m_tJointStart.cutPlaneNorm.Set();
					pOldTube->SetStartOdd(0);
				}
				else if(pOldTube->pEnd==m_pBaseNode)
				{
					pOldTube->m_tJointEnd.type=0;
					pOldTube->m_tJointEnd.hLinkObj=0;
					pOldTube->m_tJointEnd.hViceLinkObj=0;
					pOldTube->m_tJointEnd.cutPlanePos.Set();
					pOldTube->m_tJointEnd.cutPlaneNorm.Set();
					pOldTube->SetEndOdd(0);
				}
			}
		}
		//�ж�����ĶԽӸֹ��Ƿ����
		CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(hLinkObj,CLS_LINETUBE);
		CLDSLineTube *pViceLineTube = (CLDSLineTube*)console.FromPartHandle(hViceLinkObj,CLS_LINETUBE);
		if((hLinkObj&&!pLineTube)||(hViceLinkObj&&!pViceLineTube))	
		{	//��������Ϊ0������ĸֹܲ�����
			MessageBox("������󲻴��ڸøֹ�,����������!");
			return;
		}
		if(m_bFaceToFace&&pLineTube)//�Խ�����������˶ԽӸֹܾ��
		{
			if(m_pBaseNode!=pLineTube->pStart&&m_pBaseNode!=pLineTube->pEnd)
			{
				MessageBox("���ֹ������Ӳ���ʵ�ֶԽ�,����������!");
				return;	
			}
			if(pLineTube->GetDiameter()!=m_pCurLineTube->GetDiameter()
				||pLineTube->GetThick()!=m_pCurLineTube->GetThick())
			{	//�ֹܹ��ͬ
				MessageBox("���ֹܹ��ͬ����ʵ�ֶԽ�,����������!");
				return;
			}
			hViceLinkObj=-1;	//�Խ����
		}
		//���øֹ������ز���
		if(pCurrentJoint)
		{
			if(hLinkObj<0)
			{
				pCurrentJoint->cutPlaneNorm.Set();
				pCurrentJoint->cutPlanePos.Set();
				pCurrentJoint->type=0;
			}
			pCurrentJoint->type=1;
			pCurrentJoint->hLinkObj=hLinkObj;
			pCurrentJoint->hViceLinkObj=hViceLinkObj;
			m_pCurLineTube->m_hPartWeldParent=hLinkObj;
			//���øֹ�����ͷ wht 11-05-10
			if(m_bStartEnd)
				m_pCurLineTube->SetStartOdd(setupInfo.m_fOddment);
			else 
				m_pCurLineTube->SetEndOdd(setupInfo.m_fOddment);
		}
		if(m_bFaceToFace&&pLineTube&&pCurrentJoint)
		{
			//���ԽӸֹܵ���Ӧ������Ϊ�Խ����
			if(pLineTube->pStart==m_pBaseNode)
			{
				pLineTube->m_tJointStart.type=1;
				pLineTube->m_tJointStart.hLinkObj=m_pCurLineTube->handle;
				pLineTube->m_tJointStart.hViceLinkObj=-1;
				link_tube_vec=pLineTube->End()-pLineTube->Start();
			}
			else if(pLineTube->pEnd==m_pBaseNode)
			{
				pLineTube->m_tJointEnd.type=1;
				pLineTube->m_tJointEnd.hLinkObj=m_pCurLineTube->handle;
				pLineTube->m_tJointEnd.hViceLinkObj=-1;
				link_tube_vec=pLineTube->Start()-pLineTube->End();
			}
			normalize(link_tube_vec);
			//���öԽ������и�ƽ��
			f3dPoint mid_vec,vec,cutPlaneNorm;
			mid_vec=cur_tube_vec+link_tube_vec;
			normalize(mid_vec);
			vec=cur_tube_vec^link_tube_vec;
			normalize(vec);
			cutPlaneNorm=mid_vec^vec;
			normalize(cutPlaneNorm);
			//�����һ����С����ͷ
			double angle=cal_angle_of_2vec(cur_tube_vec,cutPlaneNorm);
			double tana=fabs(tan(angle));
			int min_dist=int(pLineTube->GetDiameter()*0.5*tana+1);
			//���õ�ǰ�ֹ��и���
			if(cutPlaneNorm*cur_tube_vec>0)
				cutPlaneNorm*=-1.0;
			pCurrentJoint->cutPlaneNorm=cutPlaneNorm;
			pCurrentJoint->cutPlanePos=cutPlanePos;
			//��������ͷ
			if(m_bStartEnd)
				m_pCurLineTube->SetStartOdd(min_dist);
			else
				m_pCurLineTube->SetEndOdd(min_dist);
			//���öԽӸֹܵ��и���
			if(m_pBaseNode==pLineTube->pStart)
			{
				pLineTube->SetStartOdd(min_dist);	//������ͷ
				pLineTube->m_tJointStart.cutPlaneNorm = -1.0*cutPlaneNorm;
				pLineTube->m_tJointStart.cutPlanePos = pLineTube->Start();
			}
			else if(m_pBaseNode==pLineTube->pEnd)
			{
				pLineTube->SetEndOdd(min_dist);	//������ͷ
				pLineTube->m_tJointEnd.cutPlaneNorm = -1.0*cutPlaneNorm;
				pLineTube->m_tJointEnd.cutPlanePos = pLineTube->End();
			}
		}
	}
	else if(m_iPartType>0&&!m_bHasExistEndLj)
	{	//��׼����
		if(m_iTubeEndLjStyle==2)
		{
			CLDSNode *pNode=m_bStartEnd?m_pCurLineTube->pStart:m_pCurLineTube->pEnd;
			if(m_pRelatedTube&&m_bCoupleFLCreate)
			{	//���ںϷ��ԽӸֹ�
				CLDSPart *pPart=NULL;
				if(m_pRelatedTube->pStart==pNode&&(m_pRelatedTube->m_tJointStart.hLinkObj>0X20||!m_pRelatedTube->m_tJointStart.hLinkObj))
					pPart = console.FromPartHandle(m_pRelatedTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				else if(m_pRelatedTube->pEnd==pNode&&(m_pRelatedTube->m_tJointEnd.hLinkObj>0X20||!m_pRelatedTube->m_tJointEnd.hLinkObj))
					pPart = console.FromPartHandle(m_pRelatedTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pPart)
				{
					m_bCoupleFLCreate=false;
					AfxMessageBox(CXhChar100("0x%X�ֹܵĶԽӸֹܶ�ͷ�������ӹ���,�����ظ���ƶԽӷ���!",m_pCurLineTube->handle));
				}
			}
			CLDSStdPartModel stdPartModel,coupleStdPartModel;
			globalLibrary.GetStdPartModel((char*)m_paramPlate.GetPartNo(),m_paramPlate.m_iParamType,&stdPartModel);
			stdPartModel.m_fRelateTubeThick=m_pCurLineTube->GetThick();
			if(m_bCoupleFLCreate&&m_pRelatedTube!=NULL)
			{
				globalLibrary.GetStdPartModel(sCoupleFlPlateNo,m_paramPlate.m_iParamType,&coupleStdPartModel);
				coupleStdPartModel.m_fRelateTubeThick=m_pRelatedTube->GetThick();
				NewStandardPart(&stdPartModel,&coupleStdPartModel);	
			}
			else
				NewStandardPart(&stdPartModel);	
		}
		else if(m_iTubeEndLjStyle>2&&m_pInsertPlateParam)
		{	//���
			CLDSStdPartModel stdPartModel;
			if(globalLibrary.GetStdPartModel(m_pInsertPlateParam->codeName,m_paramPlate.m_iParamType,&stdPartModel))
				NewStandardPart(&stdPartModel);
		}
	}
	else if(m_iTubeEndLjStyle>=2&&!m_bHasExistEndLj)
	{	//��ͨ����
		m_pParamPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);	
		m_pParamPlate->CopyProperty(&m_paramPlate);
		m_pParamPlate->workNorm=setupInfo.workNorm;
		m_pParamPlate->m_hPartWeldParent=m_pCurLineTube->handle;
		m_pParamPlate->SetLayer(m_pCurLineTube->layer()); //����ͼ����
		m_pParamPlate->cfgword=m_pCurLineTube->cfgword;
		if(m_bStartEnd)
		{
			m_pCurLineTube->m_tJointStart.type=m_iTubeEndLjStyle;
			m_pCurLineTube->m_tJointStart.hLinkObj=m_pParamPlate->handle;
			m_pCurLineTube->SetStartOdd(setupInfo.m_fOddment,true);
			m_pCurLineTube->desStartPos.fNormOffset=setupInfo.m_fTubeNormOffset;	//�ֹܻ�׼�淨��ƫ����
			m_pCurLineTube->DesignStartJoint();
		}
		else
		{
			m_pCurLineTube->m_tJointEnd.type=m_iTubeEndLjStyle;
			m_pCurLineTube->m_tJointEnd.hLinkObj=m_pParamPlate->handle;
			m_pCurLineTube->SetEndOdd(setupInfo.m_fOddment,true);
			m_pCurLineTube->desEndPos.fNormOffset=setupInfo.m_fTubeNormOffset;	//�ֹܻ�׼�淨��ƫ����
			m_pCurLineTube->DesignEndJoint();
		}
		double fOddment=0;
		BOOL bCanFlCoupleDesign = FALSE,bFlCoupleDesign=FALSE;
		if(m_iTubeEndLjStyle==2)	//��������
		{
			double H=0;
			m_pParamPlate->GetDesignItemValue('H',&H);
			CLDSNode *pNode;
			if(m_bStartEnd)
			{
				pNode=m_pCurLineTube->pStart;
				fOddment=m_pCurLineTube->startOdd();
			}
			else 
			{
				pNode=m_pCurLineTube->pEnd;
				fOddment=m_pCurLineTube->endOdd();
			}
			if(m_pRelatedTube&&m_pRelatedTube->m_bVirtualPart)//pRelatedTube->GetDiameter()>m_pCurLineTube->GetDiameter())
				m_pRelatedTube=NULL;	//��֧�ֻ�׼�ֹ�ֱ�Ӵ��ڶԽӸֹ�ֱ���Ĳ��Ⱦ������Խ� wht 10-09-29
			if(m_pRelatedTube)
			{
				CLDSParamPlate *pTmpPlate=NULL;
				if(m_pRelatedTube->pStart==pNode&&m_pRelatedTube->m_tJointStart.hLinkObj>0X20)
					pTmpPlate = (CLDSParamPlate*)console.FromPartHandle(m_pRelatedTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
				else if(m_pRelatedTube->pEnd==pNode&&m_pRelatedTube->m_tJointEnd.hLinkObj>0X20)
					pTmpPlate = (CLDSParamPlate*)console.FromPartHandle(m_pRelatedTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
				if(pTmpPlate==NULL)
					bCanFlCoupleDesign=TRUE;
			}
			if(bCanFlCoupleDesign)
				bFlCoupleDesign=(AfxMessageBox("�Ƿ���ƶԽӷ���Y/N?",MB_YESNO)==IDYES);
			//���ɷ����ϵ���˨
			CreateBolt(m_pParamPlate);
			//���ɷ����ϵ����
			if(m_bCreateAttachPlate)
				CreateAttachPlate(m_pParamPlate,m_pCurLineTube);
			if(bFlCoupleDesign)	//�й����νӸֹ�,�ɽ��гɶԷ������
			{
				if(m_pRelatedTube->pStart==pNode)
					m_pRelatedTube->SetStartOdd(-2*H-fOddment);
				else
					m_pRelatedTube->SetEndOdd(-2*H-fOddment);
				CLDSParamPlate *pCoupleFlPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
				pCoupleFlPlate->CopyProperty(m_pParamPlate);
				pCoupleFlPlate->m_hPartWeldParent = m_pRelatedTube->handle;
				pCoupleFlPlate->SetLayer(m_pRelatedTube->layer()); //����ͼ����
				pCoupleFlPlate->cfgword=m_pRelatedTube->cfgword;
				//���Ⱦ��Խ�ʱ���������ڻ��뾶 wht 10-09-29
				double offset_r=0.5*(m_pCurLineTube->GetDiameter()-m_pRelatedTube->GetDiameter());
				if(offset_r>EPS2)
				{
					pCoupleFlPlate->SetPartNo(sCoupleFlPlateNo);	//�Խӷ������ 
					DESIGN_PLATE_ITEM *pItem=pCoupleFlPlate->GetDesignItem('R');
					if(pItem)	//�޸ķ����ڻ��뾶
						pCoupleFlPlate->SetDesignItemValue(pItem->key,pItem->value.fValue-offset_r,pItem->sNotes,pItem->paramType);
				}
				if(m_pRelatedTube->pStart==pNode)
				{
					m_pRelatedTube->SetStartOdd(-2*H-fOddment);
					m_pRelatedTube->m_tJointStart.type=TYPE_FL;
					m_pRelatedTube->m_tJointStart.hLinkObj=pCoupleFlPlate->handle;
					m_pRelatedTube->DesignStartJoint();
				}
				else
				{
					m_pRelatedTube->SetEndOdd(-2*H-fOddment);
					m_pRelatedTube->m_tJointEnd.type=TYPE_FL;
					m_pRelatedTube->m_tJointEnd.hLinkObj=pCoupleFlPlate->handle;
					m_pRelatedTube->DesignEndJoint();
				}
				//�޸���˨ͨ�����¼�����
				for(CLsRef *pLsRef=m_pParamPlate->GetFirstLsRef();pLsRef;pLsRef=m_pParamPlate->GetNextLsRef())
				{
					pCoupleFlPlate->AppendLsRef(*pLsRef);
					pLsRef->GetLsPtr()->AddL0Thick(pCoupleFlPlate->handle,TRUE);
					pLsRef->GetLsPtr()->CalGuigeAuto();
					(*pLsRef)->SetModified();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
				//���ɳɶԵ����
				if(m_bCreateAttachPlate)
					CreateAttachPlate(pCoupleFlPlate,m_pRelatedTube,bFlCoupleDesign);
				m_pRelatedTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				pCoupleFlPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
				g_pSolidDraw->NewSolidPart(pCoupleFlPlate->GetSolidPartObject());
				g_pSolidDraw->NewSolidPart(m_pRelatedTube->GetSolidPartObject());
			}
		}
		else
		{
			//����U�Ͱ�/��߰��ϵ���˨
			CreateBolt(m_pParamPlate);
			m_pParamPlate->ResetSonBoltRayNo();		//���ò������˨���ߺ�
			m_pParamPlate->GetLsRefList()->ResetBoltRefRayNo(m_pParamPlate);	//���ò������˨�����ߺ�
		}
		//����ʮ�ֲ���ϵĸ��Ӱ�
		if(m_pParamPlate&&m_pParamPlate->m_iParamType==TYPE_XEND)
			LayoutXEndSubPlate(m_pParamPlate);
	}
	//�Ѵ��ڲ��ʱ�ڴ˴����ʮ�ֲ��ļӾ���
	if(m_bHasExistEndLj&&m_bCreateAttachPlate)
	{	
		CLDSPlate *pVertPlate[2]={NULL,NULL};
		ATTACHPART_ITEM *pItem=NULL;
		double M=0;
		m_pParamPlate->GetDesignItemValue('M',&M);
		for(pItem=m_pParamPlate->GetFirstAttachPart();pItem;pItem=m_pParamPlate->GetNextAttachPart())
		{
			CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
			if(pPlate==NULL)
				continue;
			f3dPoint plate_pos=pPlate->ucs.origin;
			//ת����ʮ�ֲ������ϵ��
			coord_trans(plate_pos,m_pParamPlate->ucs,FALSE);
			if(plate_pos.x>M)
				continue;
			if(plate_pos.x<EPS2&&plate_pos.z>0)
				pVertPlate[0]=pPlate;	//�ϲ�����
			else if(plate_pos.x<EPS2&&plate_pos.z<0)
				pVertPlate[1]=pPlate;	//�²�����
		}
		for(int i=0;i<2;i++)
		{
			if(pVertPlate[i]==NULL)
				continue;
			if(i==0)	//ʮ�ֲ��Z�������߰�
				NewRibPlate(m_pParamPlate,&AttachPlatePara,pVertPlate[i],TRUE,AttachPlatePara.m_pWeldParentPlate);
			else if(AttachPlatePara.m_bMirCreate)//�Գ�����ʮ�ֲ��Z�������߰�
				NewRibPlate(m_pParamPlate,&AttachPlatePara,pVertPlate[i],FALSE,AttachPlatePara.m_pWeldParentPlate);
			if(m_bStartEnd)
				m_pCurLineTube->m_tJointStart.bVertPlate=TRUE;
			else
				m_pCurLineTube->m_tJointEnd.bVertPlate=TRUE;
		}
	}
	//�ڴ˴����ʮ�ֲ����� wht 11-05-11
	if(m_pParamPlate&&m_pParamPlate->m_iParamType==TYPE_XEND&&m_bCreateFillPlate)
		DesignXEndFillPlate(m_pParamPlate);
	//�ڴ˴���Ʋ��Ͳ��ļӾ���
	if(m_pParamPlate&&m_pParamPlate->m_iParamType==TYPE_ROLLEND&&m_bCreateAttachPlate)
		DesignREndAttachPlate(m_pParamPlate);
	//����ֹ�λ��
	m_pCurLineTube->ClearFlag();	//��ձ�־
	m_pCurLineTube->CalPosition();
	m_pCurLineTube->SetModified();
	m_pCurLineTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(m_pCurLineTube->GetSolidPartObject());
	if(m_pParamPlate)
	{	//������Ʋ������ϵ
		if(m_bStartEnd)
			m_pCurLineTube->DesignStartJointUCS(m_pParamPlate);
		else 
			m_pCurLineTube->DesignEndJointUCS(m_pParamPlate);
		m_pParamPlate->DesignPlate();
		m_pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(m_pParamPlate->GetSolidPartObject());
	}
	g_pSolidDraw->ReleaseSnapStatus();
	m_pSrcParamPlate=NULL;	//���³�ʼ����¡�ְ�ʱ��Դ�ְ� wht 10-07-06
	CDialog::OnOK();	
}

BOOL CDesignTubeEndLjDlg::NewStandardPart(CLDSStdPartModel *pStdPartModel,CLDSStdPartModel *pCoupleStdPartModel/*=NULL*/)
{
	UpdateData();
	if(pStdPartModel==NULL)
		return FALSE;
	Ta.BeginUndoListen();
	m_pParamPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
	//��������ʱ������ȷ����ĺ�,�˴��Ƚ���ĺŸ�ֵ����ʱ�������� wht 10-08-02
	m_paramPlate.cfgword=m_pParamPlate->cfgword;	
	m_pParamPlate->CopyProperty(&m_paramPlate);
	m_pParamPlate->m_bStdPart=TRUE;	//��׼����
	m_pParamPlate->m_hPartWeldParent=m_pCurLineTube->handle;
	m_pParamPlate->m_fNormOffset=setupInfo.m_fNormOffset;//�����淨��ƫ�ľ�
	m_pParamPlate->SetLayer(m_pCurLineTube->layer()); //����ͼ����
	m_pParamPlate->cfgword=m_pCurLineTube->cfgword;
	m_pParamPlate->m_iParamType=pStdPartModel->m_iStdPartType;
	m_pParamPlate->workNorm=setupInfo.workNorm;
	if(m_iTubeEndLjStyle==2)	//��׼��������
	{
		//���ù���������ڸֹܻ�׼�����ת�Ƕ� wht 10-01-21
		//����ĸ'X'��ʾ��ת�Ƕȣ�D���׼������������ 
		//����δ����setupInfo.workNorm.rot_ang����,������˨ƫ�ƽǶȱ�����'X'������ wht 11-05-26
		m_pParamPlate->SetDesignItemValue('X',LayoutBoltPara.m_fOffsetAngle,"��׼X����Ըֹ�X����ת�Ƕ�");	//setupInfo.workNorm.rot_ang
		if(LayoutBoltPara.m_iStartEdge==1)	//�Ժ�����Ϊ��׼ m_iStartEdge��Jֵ�෴���׳����Ժ������һ�� wht 11-05-26
			m_pParamPlate->SetDesignItemValue('J',1,"��������˨ʱ:1��ʾ�Ժ�����Ϊ��׼��0��ʾ�Ի�׼��Ϊ��׼��");
		else								//�Ի�׼��Ϊ��׼
			m_pParamPlate->SetDesignItemValue('J',0,"��������˨ʱ:1��ʾ�Ժ�����Ϊ��׼��0��ʾ�Ի�׼��Ϊ��׼��");
		if(m_pParamPlate->m_iParamType==TYPE_FLP)	
			m_pParamPlate->SetDesignItemValue('O',setupInfo.H1,"�ֹܶ��뷨������ľ���");
		if(m_pParamPlate->m_iParamType==TYPE_FLD||m_pParamPlate->m_iParamType==TYPE_FLP)
			m_pParamPlate->cMaterial=pStdPartModel->param.fl_param.cMaterial;	//��������
		else if(m_pParamPlate->m_iParamType==TYPE_FLG||m_pParamPlate->m_iParamType==TYPE_FLR)
			m_pParamPlate->cMaterial=pStdPartModel->param.fl_param2.cMaterial;	//��������
		if(m_bStartEnd)
			m_pCurLineTube->m_tJointStart.hLinkObj=m_pParamPlate->handle;
		else 
			m_pCurLineTube->m_tJointEnd.hLinkObj=m_pParamPlate->handle;
		m_pParamPlate->DesignStdPartUcs();
		m_pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		pStdPartModel->CreateBoltInPlate(m_pParamPlate);//���÷����ϵ���˨
		//���ɶԽӷ���
		CLDSNode *pNode=NULL;
		if(m_bStartEnd)
			pNode=m_pCurLineTube->pStart;
		else
			pNode=m_pCurLineTube->pEnd;
		if(pCoupleStdPartModel&&pStdPartModel)
		{	//��ƶԽӷ���
			double oddment=-setupInfo.m_fOddment;
			CLDSParamPlate *pCoupleFlPlate=(CLDSParamPlate*)console.AppendPart(CLS_PARAMPLATE);
			pCoupleFlPlate->CopyProperty(m_pParamPlate);
			pCoupleFlPlate->m_hPartWeldParent = m_pRelatedTube->handle;
			pCoupleFlPlate->SetLayer(m_pRelatedTube->layer()); //����ͼ����
			pCoupleFlPlate->cfgword=m_pRelatedTube->cfgword; //����������ͼ����
			if(m_pParamPlate->m_iParamType==TYPE_FLD||m_pParamPlate->m_iParamType==TYPE_FLP)
			{
				pCoupleFlPlate->cMaterial=pCoupleStdPartModel->param.fl_param.cMaterial;	//��׼��������
				pCoupleFlPlate->SetPartNo(pCoupleStdPartModel->param.fl_param.codeName,true);
			}
			else //if(m_pParamPlate->m_iParamType==TYPE_FLG||m_pParamPlate->m_iParamType==TYPE_FLR)
			{
				pCoupleFlPlate->cMaterial=pCoupleStdPartModel->param.fl_param2.cMaterial;	//��׼��������
				pCoupleFlPlate->SetPartNo(pCoupleStdPartModel->param.fl_param2.codeName,true);
			}
			if(m_pRelatedTube->pStart==pNode)
			{
				m_pRelatedTube->m_tJointStart.type=m_iTubeEndLjStyle;
				m_pRelatedTube->m_tJointStart.hLinkObj=pCoupleFlPlate->handle;
				//�����������,�������m_tJointStart.typeδ��ֵ,��������ͷ�޷������������渳ֵ wjh-2015.3.20
				m_pRelatedTube->SetStartOdd(oddment,true);
			}
			else
			{
				m_pRelatedTube->m_tJointEnd.type=m_iTubeEndLjStyle;
				m_pRelatedTube->m_tJointEnd.hLinkObj=pCoupleFlPlate->handle;
				m_pRelatedTube->SetEndOdd(oddment,true);
			}
			//�޸���˨ͨ�����¼�����
			for(CLsRef *pLsRef=m_pParamPlate->GetFirstLsRef();pLsRef;pLsRef=m_pParamPlate->GetNextLsRef())
			{
				pCoupleFlPlate->AppendLsRef(*pLsRef);
				pLsRef->GetLsPtr()->AddL0Thick(pCoupleFlPlate->handle,TRUE);
				pLsRef->GetLsPtr()->CalGuigeAuto();
				(*pLsRef)->SetModified();
				(*pLsRef)->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
			}
			//
			pCoupleFlPlate->DesignStdPartUcs();
			m_pRelatedTube->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			pCoupleFlPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pCoupleFlPlate->GetSolidPartObject());
			g_pSolidDraw->NewSolidPart(m_pRelatedTube->GetSolidPartObject());	
			//���ɶԽӸֹ��ϵĸ��Է����������Է����ϵ����
			if(m_bCreateAttachPlate&&m_iPartType>=3)
				CreateAttachPlate(pCoupleFlPlate,m_pRelatedTube);
		}
	}
	else
	{	
		//���ù���������ڸֹܻ�׼�����ת�Ƕ� wht 10-01-21
		//����ĸ'X'��ʾ��ת�Ƕȣ�D���׼������������ 
		m_pParamPlate->SetDesignItemValue('X',setupInfo.workNorm.rot_ang,"��������Ըֹܻ�׼����ת�Ƕ�");
		m_pParamPlate->cMaterial=pStdPartModel->param.insert_plate_param.cMaterial;	//������
		if(m_pParamPlate->m_iParamType==TYPE_XEND)	//ʮ�ֲ���������ʮ�ֲ����Ų���ͬ wht 10-01-21
			m_pParamPlate->SetPartNo(pStdPartModel->param.insert_plate_param.datumPlatePartNo,true);
	}
	if(m_bStartEnd)
	{
		m_pCurLineTube->m_tJointStart.type=m_iTubeEndLjStyle;
		m_pCurLineTube->m_tJointStart.hLinkObj=m_pParamPlate->handle;
		m_pCurLineTube->desStartPos.fNormOffset = setupInfo.m_fTubeNormOffset;	//�ֹܻ�׼�淨��ƫ����
		m_pCurLineTube->SetStartOdd(setupInfo.m_fOddment,true);
	}
	else
	{
		m_pCurLineTube->m_tJointEnd.type=m_iTubeEndLjStyle;
		m_pCurLineTube->m_tJointEnd.hLinkObj=m_pParamPlate->handle;
		m_pCurLineTube->desEndPos.fNormOffset = setupInfo.m_fTubeNormOffset;	//�ֹܻ�׼�淨��ƫ����
		m_pCurLineTube->SetEndOdd(setupInfo.m_fOddment,true);
	}
	m_pCurLineTube->ClearFlag();	//��ձ�־
	m_pCurLineTube->CalPosition();	//����ֹ�λ��
	m_pParamPlate->DesignStdPartUcs();
	m_pParamPlate->DesignPlate();
	m_pParamPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
	g_pSolidDraw->NewSolidPart(m_pParamPlate->GetSolidPartObject());
	if(m_iTubeEndLjStyle>2)		//������˨�ѵ������ù�	wht 09-11-25
		pStdPartModel->CreateBoltInPlate(m_pParamPlate);//������˨U/C/X�Ͳ���ϵ���˨
	if(m_pParamPlate->m_iParamType==TYPE_XEND)
		LayoutXEndSubPlate(m_pParamPlate);
	//���ɸ��Է����������Է����ϵ����(�ֹ�����ͷ����֮�������ɸ��Ӱ壬����λ�ò���) 
	if(m_pParamPlate->IsFL()&&m_bCreateAttachPlate&&m_iPartType>=3)
		CreateAttachPlate(m_pParamPlate,m_pCurLineTube);
	//���Ʊ�׼�����ϵ���˨
	for(CLsRef *pLsRef=m_pParamPlate->GetFirstLsRef();pLsRef;pLsRef=m_pParamPlate->GetNextLsRef())
	{	
		(*pLsRef)->correct_worknorm();
		(*pLsRef)->correct_pos();
		(*pLsRef)->CalGuigeAuto();
		(*pLsRef)->SetModified();
		(*pLsRef)->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	m_pParamPlate->ResetSonBoltRayNo();		//���ò������˨���ߺ�
	m_pParamPlate->GetLsRefList()->ResetBoltRefRayNo(m_pParamPlate);	//���ò������˨�����ߺ�
	UpdateData(FALSE);
	Ta.EndUndoListen();
	return TRUE;
}

//���ݱ�׼�������� �������»�ñ�׼��������
void CDesignTubeEndLjDlg::LoadStdPlateParaByLevel()
{
	if(m_iTubeEndLjStyle==2 && m_iPartType==2)
	{	//�Ժ�����
		double tube_d_m=0,tube_d_c=0;
		//���ܷ���
		tube_d_m=m_pCurLineTube->GetDiameter();
		tube_d_c=m_pRelatedTube?m_pRelatedTube->GetDiameter():tube_d_m;
		if(tube_d_m>tube_d_c)
			tube_d_c=tube_d_m;
		m_pFlDParam=FindFlDParam(tube_d_m,tube_d_c,m_cStdPartMat,m_cFlLevel1);
		//�Խӷ���
		if(m_pRelatedTube)
		{
			tube_d_m=m_pRelatedTube->GetDiameter();
			tube_d_c=m_pCurLineTube->GetDiameter();
			if(tube_d_m>tube_d_c)
				tube_d_c=tube_d_m;
			m_pFlDParam2=FindFlDParam(tube_d_m,tube_d_c,m_cStdPartMat,m_cFlLevel2);
		}
	}
	else	//��׼���
		m_pInsertPlateParam=QueryStdInsertPlate(m_iStdPartLevel,m_cStdPartMat);
}

//����ʮ�ֲ���ϵĸ��Ӱ�
void CDesignTubeEndLjDlg::LayoutXEndSubPlate(CLDSParamPlate *pParamPlate,BOOL bIsCloneBolt/*=FALSE*/)
{
	if(pParamPlate==NULL)
		return;
	if(pParamPlate->m_iParamType!=TYPE_XEND)
		return;	//��ʮ�ֲ��
	CString sPartNo;
	double A=0,M=0,R=0,x=0,y=0,z=0,N=0,S=0,W=0,L=0,H=0;
	if(pParamPlate->m_bStdPart)
	{	//��׼����
		CLDSStdPartModel stdPartModel;
		if(!pParamPlate->GetStdPartModel(&stdPartModel))
			return;
		A=stdPartModel.param.insert_plate_param.A;
		R=stdPartModel.param.insert_plate_param.R;
		N=stdPartModel.param.insert_plate_param.N;
		S=stdPartModel.param.insert_plate_param.S;
		W=stdPartModel.param.insert_plate_param.W;
		H=stdPartModel.param.insert_plate_param.H;
		L=stdPartModel.param.insert_plate_param.L;
		M=stdPartModel.param.insert_plate_param.M;
	}
	else
	{
		pParamPlate->GetDesignItemValue('N',&N);
		pParamPlate->GetDesignItemValue('S',&S);
		pParamPlate->GetDesignItemValue('W',&W);
		pParamPlate->GetDesignItemValue('M',&M);
		pParamPlate->GetDesignItemValue('R',&R);
		pParamPlate->GetDesignItemValue('A',&A);
		pParamPlate->GetDesignItemValue('L',&L);
		pParamPlate->GetDesignItemValue('H',&H);
	}
	x=A+M;
	y=R;
	z=0.5*pParamPlate->GetThick();
	
	//ʮ�ֲ����Ҫ�������� ��׼ʮ�ֲ�������Ҳ�������� wht 10-01-24
	CLDSPlate *pUpVertPlate=NULL,*pDownVertPlate=NULL; //Z����������  Z����������
	//ʮ�ֲ�����巨����ƻ�׼����Ϊ����			//wht 10-03-06
	//X-Yƽ���ϵİ��弰��˨������ƻ�׼����Ϊ����
	//X-Zƽ���ϵİ��弰��˨������ƻ�׼����Ϊ��Ӧ������,���巨���޸ĺ���弰��˨�����ܼ�ʱ����
	int i;
	for(i=0;i<2;i++)
	{
		CLDSPlate *pVertPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		pVertPlate->iSeg=pParamPlate->iSeg;
		pVertPlate->cfgword=pParamPlate->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
		pVertPlate->cMaterial = pParamPlate->cMaterial;
		pVertPlate->SetLayer(pParamPlate->layer()); //����ͼ����
		pVertPlate->cfgword=pParamPlate->cfgword; //����������ͼ����
		pVertPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
		pVertPlate->Thick=ftoi(pParamPlate->GetThick());
		pVertPlate->SetPartNo(sWeldPlatePartNo);
		pVertPlate->jdb_style=5;	//��׼���������λ�ò���ĸ��Ӱ�
		//.................................................
		//�˴������⾡����ȥʹ��jdb_style �Ժ��޸� 
		//.................................................
		pVertPlate->designInfo.m_hBasePart=pParamPlate->handle;
		pVertPlate->designInfo.origin.datum_pos_style=9;//��׼�����ϵ��������
		pVertPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
		pVertPlate->m_fNormOffset=-z;
		if(i==0)
		{	//Z�����������
			pUpVertPlate=pVertPlate;	//��¼Z����������
			pVertPlate->designInfo.norm=setupInfo.workNormUpVert;
			if(setupInfo.workNormUpVert.norm_style==4&&setupInfo.workNormUpVert.hVicePart<=0X20)
				pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
			//pVertPlate->designInfo.norm.nearVector.Set(0,-1,0);
			pVertPlate->designInfo.origin.datum_pos.Set(0,0,z);
			//������Ӧ����˨��������
			for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				if(pLsRef->start1_end2_mid0==31)//������˨��Z������������
				{	
					pVertPlate->AppendLsRef(*pLsRef);
					(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
				}
			}
		}
		else if(i==1)
		{	//Z�����������
			pDownVertPlate=pVertPlate;	//��¼Z����������
			pVertPlate->designInfo.origin.datum_pos.Set(0,0,-z);
			pVertPlate->designInfo.norm=setupInfo.workNormDownVert;
			if(setupInfo.workNormDownVert.norm_style==4&&setupInfo.workNormDownVert.hVicePart<=0X20)
				pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
			//pVertPlate->designInfo.norm.nearVector.Set(0,1,0);
			//������Ӧ����˨��������
			for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
			{
				if(pLsRef->start1_end2_mid0==41)//������˨��Z������������
				{
					pVertPlate->AppendLsRef(*pLsRef);
					(*pLsRef)->AddL0Thick(pVertPlate->handle,TRUE);
				}
			}
		}
		pVertPlate->DesignSetupUcs();
		pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0.5*H-z,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0.5*H-z,0));
		pVertPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pVertPlate->GetSolidPartObject());
		//ʮ�ֲ��Ӿ���
		if(m_bCreateAttachPlate)
		{	
			if(i==0)	//ʮ�ֲ��Z�������߰�
				NewRibPlate(pParamPlate,&AttachPlatePara,pVertPlate,TRUE,AttachPlatePara.m_pWeldParentPlate);
			else if(AttachPlatePara.m_bMirCreate)//�Գ�����ʮ�ֲ��Z�������߰�
				NewRibPlate(pParamPlate,&AttachPlatePara,pVertPlate,FALSE,AttachPlatePara.m_pWeldParentPlate);
			if(m_bStartEnd)
				m_pCurLineTube->m_tJointStart.bVertPlate=TRUE;
			else
				m_pCurLineTube->m_tJointEnd.bVertPlate=TRUE;
		}
		//��������ӵ����ӹ����б���
		pParamPlate->AppendAttachPart(pVertPlate);
	}
	if(m_iWrapPlateType==1)
	{	//�ö̽Ǹִ������
		double fAngleY=0.5*pParamPlate->GetThick();	//�̽Ǹֶ�λY����
		for(i=0;i<4;i++)
		{	//�����ĸ��̽Ǹִ���˿����
			CLDSLineAngle *pShortJg=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
			pShortJg->iSeg=pParamPlate->iSeg;
			pShortJg->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
			pShortJg->cMaterial = pParamPlate->cMaterial;
			pShortJg->SetLayer(pParamPlate->layer()); //����ͼ����
			pShortJg->cfgword=pParamPlate->cfgword; //���������ͼ����
			////����Ϊ��˨���ӣ���Ӧ���к��Ӹ�����	wht 10-08-02
			//pShortJg->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
			pShortJg->set_wing_thick(W);	//֫��
			pShortJg->set_wing_wide(N+R-0.5*pParamPlate->GetThick());//֫��
			pShortJg->SetPartNo(sWrapPlatePartNo);
			//λ�ò���
			pShortJg->desStartPos.SetToDatumPointStyle();
			//pShortJg->desStartPos.datum_pos_style=15;	//ֱ��ָ������
			pShortJg->desStartPos.datumPoint.datum_pos_style=9;	//ָ�������ϵ��������
			pShortJg->desStartPos.datumPoint.des_para.hPart=pParamPlate->handle;
			pShortJg->desEndPos.SetToDatumPointStyle();
			//pShortJg->desEndPos.datum_pos_style=15;	//ֱ��ָ������
			pShortJg->desEndPos.datumPoint.datum_pos_style=9;	//ָ�������ϵ��������
			pShortJg->desEndPos.datumPoint.des_para.hPart=pParamPlate->handle;
			//���߲���
			pShortJg->des_norm_x.spec_norm.norm_style=4;//ָ�������ϵ���Է���
			pShortJg->des_norm_x.bByOtherWing=FALSE;
			pShortJg->des_norm_x.bSpecific=TRUE;
			pShortJg->des_norm_y.spec_norm.norm_style=4;//ָ�������ϵ���Է���
			pShortJg->des_norm_y.bByOtherWing=FALSE;
			pShortJg->des_norm_y.bSpecific=TRUE;
			if(i==0)
			{	//X-Yƽ��: Y��Z��
				pShortJg->desStartPos.datumPoint.datum_pos.Set(x,-fAngleY,z);
				pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,-fAngleY,z);
				//X֫���߷�����Z����������� Y֫���߷��������
				pShortJg->des_norm_x.spec_norm.hVicePart=pUpVertPlate->handle;
				pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
				pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
				pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,-1);
				//������˨
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
					if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
						||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
					{
						pShortJg->AppendMidLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
					}
				}
			}
			else if(i==1)
			{	//X-Yƽ��: Y��Z��
				pShortJg->desStartPos.datumPoint.datum_pos.Set(x,fAngleY,z);
				pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,fAngleY,z);
				//Y֫���߷�����Z����������� X֫���߷��������
				pShortJg->des_norm_y.spec_norm.hVicePart=pUpVertPlate->handle;
				pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
				pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
				pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
				//������˨
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
						||pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
					{
						pShortJg->AppendMidLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
					}
				}
			}
			else if(i==2)
			{	//X-Yƽ��: Y��Z��
				pShortJg->desStartPos.datumPoint.datum_pos.Set(x,-fAngleY,-z);
				pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,-fAngleY,-z);
				//X֫���߷�����Z����������� Y֫���߷��������
				pShortJg->des_norm_y.spec_norm.hVicePart=pDownVertPlate->handle;
				pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
				pShortJg->des_norm_x.spec_norm.hVicePart=pParamPlate->handle;
				pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,1);
				//������˨
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
					if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22
						||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
					{
						pShortJg->AppendMidLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
					}
				}
			} 
			else if(i==3)
			{	//X-Yƽ��: Y��Z��
				pShortJg->desStartPos.datumPoint.datum_pos.Set(x,fAngleY,-z);
				pShortJg->desEndPos.datumPoint.datum_pos.Set(x+S,fAngleY,-z);
				//Y֫���߷�����Z����������� X֫���߷��������
				pShortJg->des_norm_x.spec_norm.hVicePart=pDownVertPlate->handle;
				pShortJg->des_norm_x.spec_norm.nearVector.Set(0,0,-1);
				pShortJg->des_norm_y.spec_norm.hVicePart=pParamPlate->handle;
				pShortJg->des_norm_y.spec_norm.nearVector.Set(0,0,1);
				//������˨
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Yƽ��:Y������һ�����˨ X-Zƽ��:Z������һ�����˨
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12
						||pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
					{
						pShortJg->AppendMidLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pShortJg->handle,TRUE);
					}
				}
			}
			pShortJg->ClearFlag();
			pShortJg->CalPosition();
			pShortJg->cal_x_wing_norm();
			pShortJg->cal_y_wing_norm();
			pShortJg->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pShortJg->GetSolidPartObject());
			//��������ӵ����ӹ����б���
			pParamPlate->AppendAttachPart(pShortJg);
		}
	}
	else 
	{	//����Ϊ�ְ�
		for(i=0;i<8;i++)
		{
			CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pPlate->iSeg=pParamPlate->iSeg;
			pPlate->SetLayer(pParamPlate->layer()); //����ͼ����
			pPlate->cfgword=pParamPlate->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
			pPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
			pPlate->cMaterial = pParamPlate->cMaterial;
			//����Ϊ��˨���ӣ���Ӧ���к��Ӹ�����	wht 10-08-02
			//pPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
			pPlate->Thick=ftoi(W);
			pPlate->SetPartNo(sWrapPlatePartNo);
			pPlate->jdb_style=5;	//��׼���������λ�ò���ĸ��Ӱ�
			pPlate->designInfo.m_hBasePart=pParamPlate->handle;
			pPlate->designInfo.norm.norm_style=4;	//��׼�����ϵ���Է���
			pPlate->designInfo.origin.datum_pos_style=9;//��׼�����ϵ��������
			if(i==0)
			{	//X-Yƽ��: Y��Z��
				pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
				pPlate->designInfo.norm.nearVector.Set(0,0,-1);
				pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
				pPlate->designInfo.origin.datum_pos.Set(x,-y,z);
				pPlate->m_fNormOffset=-W;
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Yƽ��:Y������һ�����˨ 
					if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==1)
			{	//X-Yƽ��: Y��Z��
				pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
				pPlate->designInfo.norm.nearVector.Set(0,0,-1);
				pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
				pPlate->designInfo.origin.datum_pos.Set(x,-y,-z);
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Yƽ��:Y������һ�����˨
					if(pLsRef->start1_end2_mid0==21||pLsRef->start1_end2_mid0==22)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==2)
			{	//X-Yƽ��: Y��Z��
				pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
				pPlate->designInfo.origin.datum_pos.Set(x,y,z);
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Yƽ��:Y������һ�����˨
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==3)
			{	//X-Yƽ��: Y��Z��
				pPlate->designInfo.norm.hVicePart=pParamPlate->handle;
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
				pPlate->designInfo.origin.datum_pos.Set(x,y,-z);
				pPlate->m_fNormOffset=-W;
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Yƽ��:Y������һ�����˨
					if(pLsRef->start1_end2_mid0==11||pLsRef->start1_end2_mid0==12)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==4)
			{	//X-Zƽ��: Y��Z��
				pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//�ϲ�����
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//�ϲ�����
				pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=������
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Zƽ��:Z������һ�����˨
					if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==5)
			{	//X-Zƽ��: Y��Z��
				pPlate->designInfo.norm.hVicePart=pUpVertPlate->handle;			//�ϲ�����
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pUpVertPlate->handle;	//�ϲ�����
				pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=������
				pPlate->m_fNormOffset=-W;
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Zƽ��:Z������һ�����˨
					if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==6)
			{	//X-Zƽ��: Y��Z��
				pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//�²�����
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//�²�����
				pPlate->designInfo.origin.datum_pos.Set(x,y-z,-z);				//y=R z=������
				pPlate->m_fNormOffset=-W;
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Zƽ��:Z������һ�����˨
					if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			else if(i==7)
			{	//X-Zƽ��: Y��Z��
				pPlate->designInfo.norm.hVicePart=pDownVertPlate->handle;		//�²�����
				pPlate->designInfo.norm.nearVector.Set(0,0,1);
				pPlate->designInfo.origin.des_para.hPart=pDownVertPlate->handle;//�²�����
				pPlate->designInfo.origin.datum_pos.Set(x,y-z,z);				//y=R z=������
				for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
				{	//X-Zƽ��:Z������һ�����˨
					if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
					{
						pPlate->AppendLsRef(*pLsRef);
						(*pLsRef)->AddL0Thick(pPlate->handle,TRUE);
					}
				}
			}
			pPlate->DesignSetupUcs();
			pPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
			pPlate->vertex_list.append(PROFILE_VERTEX(S,0,0));
			pPlate->vertex_list.append(PROFILE_VERTEX(S,N,0));
			pPlate->vertex_list.append(PROFILE_VERTEX(0,N,0));
			pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
			g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			//��������ӵ����ӹ����б���
			pParamPlate->AppendAttachPart(pPlate);
		}
	}
	if(bIsCloneBolt)
		z=0;	//ͨ���Ժ����¡��������˨����Ҫ������˨ƫ��ֵ wht 11-05-27
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{	
		//����˨��������/����������˨����ƫ����
		if(m_pSrcParamPlate==NULL)	//ͨ����������ɲ��ʱ���õ�����˨����ƫ���� wht 10-07-06
			(*pLsRef)->des_base_pos.norm_offset.AddThick(-(long)W);
		//������˨ʱ��˨������˨���Ի���Ϊ��λ��׼�������в��ã�
		//���������,�����ϵ���˨��������Ϊ��λ��׼�����ʲ³��������µ���
		//����ʮ�ֲ����˨������Ʋ����Լ�λ����Ʋ���	wht 10-03-06
		if(pUpVertPlate&&pDownVertPlate)
		{	//X-Zƽ��:Z������һ�����˨
			if(pLsRef->start1_end2_mid0==31||pLsRef->start1_end2_mid0==32)
			{
				(*pLsRef)->des_work_norm.norm_style=4;	//��׼�����ϵ���Է���
				(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
				(*pLsRef)->des_work_norm.hVicePart=pUpVertPlate->handle;
				//����λ�ò���
				(*pLsRef)->des_base_pos.hPart=pUpVertPlate->handle;
				double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
				double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
				(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=������
				(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
			}
			//X-Zƽ��:Z������һ�����˨
			else if(pLsRef->start1_end2_mid0==41||pLsRef->start1_end2_mid0==42)
			{
				(*pLsRef)->des_work_norm.norm_style=4;	//��׼�����ϵ���Է���
				(*pLsRef)->des_work_norm.nearVector.Set(0,0,1);
				(*pLsRef)->des_work_norm.hVicePart=pDownVertPlate->handle;
				//����λ�ò���
				(*pLsRef)->des_base_pos.hPart=pDownVertPlate->handle;
				double wing_offset_dist=fabs((*pLsRef)->des_base_pos.wing_offset_dist);
				double len_offset_dist=fabs((*pLsRef)->des_base_pos.len_offset_dist);
				(*pLsRef)->des_base_pos.wing_offset_dist=wing_offset_dist-z; // z=������
				(*pLsRef)->des_base_pos.len_offset_dist=len_offset_dist;
			}
		}
		(*pLsRef)->SetModified();
		(*pLsRef)->correct_worknorm();
		(*pLsRef)->correct_pos();
		(*pLsRef)->CalGuigeAuto();
		(*pLsRef)->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
	}
}
void CDesignTubeEndLjDlg::DesignREndAttachPlate(CLDSParamPlate *pParamPlate)
{
	for(int i=0;i<2;i++)
	{
		if(i==1 && !AttachPlatePara.m_bMirCreate)
			break;
		double fOffZ=0.5*pParamPlate->GetThick();
		CLDSPlate *pVertPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		pVertPlate->iSeg=pParamPlate->iSeg;
		pVertPlate->cfgword=pParamPlate->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
		pVertPlate->m_uStatMatCoef=pParamPlate->m_uStatMatCoef;
		pVertPlate->cMaterial = pParamPlate->cMaterial;
		pVertPlate->SetLayer(pParamPlate->layer()); //����ͼ����
		pVertPlate->m_hPartWeldParent=pParamPlate->handle;//���ݸ������Ĳ�����ư���
		pVertPlate->Thick=ftoi(AttachPlatePara.m_nThick);
		pVertPlate->SetPartNo(AttachPlatePara.sPartNo);
		pVertPlate->m_fNormOffset=-fOffZ;
		pVertPlate->jdb_style=5;
		pVertPlate->designInfo.m_hBasePart=pParamPlate->handle;
		pVertPlate->designInfo.origin.datum_pos_style=9;//��׼�����ϵ��������
		pVertPlate->designInfo.origin.des_para.hPart=pParamPlate->handle;
		pVertPlate->designInfo.norm.norm_style=4;
		pVertPlate->designInfo.norm.hVicePart=pParamPlate->handle;
		if(i==0)
		{
			pVertPlate->designInfo.origin.datum_pos.Set(0,0,-fOffZ);
			pVertPlate->designInfo.norm.nearVector.Set(0,1,0);
		}
		else
		{
			pVertPlate->designInfo.origin.datum_pos.Set(0,0,fOffZ);
			pVertPlate->designInfo.norm.nearVector.Set(0,-1,0);
		}
		pVertPlate->DesignSetupUcs();
		//��ʼ��������
		double L=AttachPlatePara.A;
		double H=AttachPlatePara.B;
		pVertPlate->vertex_list.append(PROFILE_VERTEX(0,0,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(L,0,0));
		if(AttachPlatePara.F>0 && AttachPlatePara.F<H)
			pVertPlate->vertex_list.append(PROFILE_VERTEX(L,H-AttachPlatePara.F,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(L-AttachPlatePara.E,H,0));
		pVertPlate->vertex_list.append(PROFILE_VERTEX(0+AttachPlatePara.C,H,0));
		if(AttachPlatePara.D>0 && AttachPlatePara.D<H)
			pVertPlate->vertex_list.append(PROFILE_VERTEX(0,H-AttachPlatePara.D,0));
		pVertPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pVertPlate->GetSolidPartObject());
		//��������ӵ����ӹ����б���
		pParamPlate->AppendAttachPart(pVertPlate);
	}
}
//������˨
void CDesignTubeEndLjDlg::LayoutBolt(ATOM_LIST<f3dPoint>& bolt_pt_arr)
{
	f3dPoint bolt_pos;
	if(m_iTubeEndLjStyle==2)	//��������
	{	//����
		double init_angle=(m_iPartType!=0)?0:LayoutBoltPara.m_fOffsetAngle;
		double piece_angle=2*Pi/LayoutBoltPara.m_nBoltNum;
		for(int i=0;i<LayoutBoltPara.m_nBoltNum;i++)
		{
			double angle=init_angle*RADTODEG_COEF+i*piece_angle;
			bolt_pos.x=0.5*LayoutBoltPara.m_fBoltLayoutD*cos(angle);
			bolt_pos.y=0.5*LayoutBoltPara.m_fBoltLayoutD*sin(angle);
			bolt_pos.z=0;
			bolt_pt_arr.append(bolt_pos);
		}
	}
	else if(m_iTubeEndLjStyle>2)	//��������
	{	//���
		//��������ĵ�һ����˨��λ��,������,�Լ�����(��X��),��಼����˨
		//ʮ�ֲ���ϲ�����˨ʱ������Ҫ����˫����˨		wht 09-06-06
		//˫����˨�����ַ�ʽ����һ�־���Ĭ�ϵ�ÿ��С��������˨������ͬ��
		//					  �ڶ��־���ˮƽ������ϵ���˨����ֱ���ϵ���˨�ʽ�����
		for(LAYOUT_BOLT_PARA *pLayoutPara=LayoutBoltParaList.GetFirst();pLayoutPara;pLayoutPara=LayoutBoltParaList.GetNext())
		{
			if(m_paramPlate.m_iParamType==TYPE_XEND)
			{	//�ڰ�������ϵ�²���ʮ�ֲ����˨
				double H=0,N=0,A=0,M=0,R=0;
				m_paramPlate.GetDesignItemValue('H',&H);
				m_paramPlate.GetDesignItemValue('N',&N);
				m_paramPlate.GetDesignItemValue('A',&A);
				m_paramPlate.GetDesignItemValue('M',&M);
				m_paramPlate.GetDesignItemValue('R',&R);
				double fBoltPosX=pLayoutPara->m_posFirstBolt.x+A+M;
				double fBoltPosY=pLayoutPara->m_posFirstBolt.y+R;
				double fBoltPosZ=pLayoutPara->m_posFirstBolt.z;
				//����ʮ�ֲ����˨ʱ�û������Ϊ��԰��������˴���Ҫ����԰��������ת����ʮ�ֲ������ϵ��
				//ˮƽ������ֱ�潻����ʱ����˨Y������H�޹� wht 10-01-22
				double Y=R+(N-pLayoutPara->m_posFirstBolt.y);	//������ʱ����˨Y����
				double offset_dist=0;
				for(int i=0;i<pLayoutPara->m_nBoltNum;i++)
				{
					//X-Yƽ��: Y������һ�����˨
					bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
					bolt_pos.y=fBoltPosY+pLayoutPara->m_fRowSpace*(i%abs(pLayoutPara->m_nRowNum));
					bolt_pos.z=fBoltPosZ;
					bolt_pos.feature=11;
					bolt_pt_arr.append(bolt_pos);
					//X-Yƽ��: Y������һ�����˨
					bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
					bolt_pos.y=-(fBoltPosY+pLayoutPara->m_fRowSpace*(i%abs(pLayoutPara->m_nRowNum)));
					bolt_pos.z=fBoltPosZ;
					bolt_pos.feature=21;
					bolt_pt_arr.append(bolt_pos);
					if(pLayoutPara->m_bStaggerLayout)	//ˮƽ�����ֱ�潻������˨
					{	
						//X-Zƽ��: Z������һ�����˨
						bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
						bolt_pos.y=0;
						bolt_pos.z=Y;
						bolt_pos.feature=31;
						bolt_pt_arr.append(bolt_pos);
						//X-Zƽ��: Z������һ�����˨
						bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
						bolt_pos.y=0;
						bolt_pos.z=-Y;
						bolt_pos.feature=41;
						bolt_pt_arr.append(bolt_pos);
					}
					else
					{
						//X-Zƽ��: Z������һ�����˨
						bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
						bolt_pos.y=0;
						bolt_pos.z=fBoltPosY+pLayoutPara->m_fRowSpace*(i%abs(pLayoutPara->m_nRowNum));
						bolt_pos.feature=31;
						bolt_pt_arr.append(bolt_pos);
						//X-Zƽ��: Z������һ�����˨
						bolt_pos.x=fBoltPosX+pLayoutPara->m_fColSpace*(i/abs(pLayoutPara->m_nRowNum));
						bolt_pos.y=0;
						bolt_pos.z=-(fBoltPosY+pLayoutPara->m_fRowSpace*(i%abs(pLayoutPara->m_nRowNum)));
						bolt_pos.feature=41;
						bolt_pt_arr.append(bolt_pos);
					}
				}
			}
			else
			{
				for(int i=0;i<pLayoutPara->m_nBoltNum;i++)
				{
					bolt_pos.x=pLayoutPara->m_posFirstBolt.x+pLayoutPara->m_fColSpace*(i/pLayoutPara->m_nRowNum);
					bolt_pos.y=pLayoutPara->m_posFirstBolt.y+pLayoutPara->m_fRowSpace*(i%pLayoutPara->m_nRowNum);
					bolt_pos.z=pLayoutPara->m_posFirstBolt.z;
					bolt_pt_arr.append(bolt_pos);
				}
				int nLast=pLayoutPara->m_nBoltNum%pLayoutPara->m_nRowNum;
				if(nLast>0)	
				{	//���һ����˨λ��������
					int index=0;
					for (f3dPoint *pPt =bolt_pt_arr.GetTail();index<nLast;index++,pPt=bolt_pt_arr.GetPrev())
						pPt->y += 0.5*(pLayoutPara->m_nRowNum-nLast)*pLayoutPara->m_fRowSpace;
				}
			}
		}
	}
}

void CDesignTubeEndLjDlg::CreateBolt(CLDSParamPlate *pParamPlate)
{	
	f3dPoint pos;
	char sText[100];
	DWORD dwRayNo = 0;
	if(m_iPartType>0)
		return;	//��׼������˨�����ô˺���
	if(m_iTubeEndLjStyle==2)	//��������˨
	{
		if(LayoutBoltPara.m_iStartEdge==1) //�Ըֹܺ�����Ϊ��׼
			pParamPlate->SetDesignItemValue('J',1,"��������������˨ʱ:1��ʾ�Ժ�����Ϊ��׼��0��ʾ�Ի�׼��Ϊ��׼��");
		else //if(LayoutBoltPara.m_iStartEdge==0) //�Ըֹܻ�׼��Ϊ��׼
			pParamPlate->SetDesignItemValue('J',0,"��������������˨ʱ:1��ʾ�Ժ�����Ϊ��׼��0��ʾ�Ի�׼��Ϊ��׼��");
	}
	//��˨���ݶ�����
	ATOM_LIST<RAYNO_RECORD>rayNoList;
	int iInitRayNo=1;
	for(int i=0;i<m_listBolt.GetItemCount();i++)
	{
		m_listBolt.GetItemText(i,0,sText,100);
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		if(m_iTubeEndLjStyle==2)
			pBolt->m_hFamily=4;	//������Ӧѡ�÷�����˨���ϵ��
		pBolt->set_d(atoi(sText+1));
		pBolt->iSeg = pParamPlate->iSeg;
		pBolt->SetLayer(pParamPlate->layer());	//������˨��ͼ����
		pBolt->cfgword=pParamPlate->cfgword;	//������˨��ĺ����׼������ĺ�һ��
		pBolt->des_base_pos.hPart = pParamPlate->handle;
		pBolt->des_base_pos.cLocationStyle=TRUE; 
		pBolt->des_work_norm.norm_style=3;	//��������
		pBolt->des_work_norm.hVicePart=pParamPlate->handle;
		pBolt->des_work_norm.direction=LayoutBoltPara.m_iDirection;
		if(m_iTubeEndLjStyle>2)		//��������
		{
			m_listBolt.GetItemText(i,1,sText,100);
			pos.x=atof(sText);
			m_listBolt.GetItemText(i,2,sText,100);
			pos.y=atof(sText);
			m_listBolt.GetItemText(i,3,sText,100);
			pos.z=atof(sText);
			pBolt->des_base_pos.len_offset_dist=pos.x;
			double L=0;
			if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
			{	//ʮ�ֲ��	
				DWORD dwFlag=m_listBolt.GetItemData(i);	//��˨��־λ
				WORD* pw_h=(WORD*)&dwFlag;
				pos.feature=*(pw_h+1);
				pParamPlate->GetDesignItemValue('L',&L);
				pBolt->des_base_pos.wing_offset_dist=pos.y;
				if(pos.feature==11)
				{	//X-Yƽ��: Y������һ�����˨
					if(L>pos.x)//Y������һ��,���Ӳ����������˨
						pBolt->feature=11;
					else
						pBolt->feature=12;	//Y������һ��,���Ӱ����������������˨
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.z);
				}
				else if(pos.feature==21)
				{	//X-Yƽ��: Y������һ�����˨
					if(L>pos.x)//Y������һ��,���Ӳ����������˨
						pBolt->feature=21;
					else
						pBolt->feature=22;	//Y������һ��,���Ӱ����������������˨
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.z);
				}
				else if(pos.feature==31)	
				{	//X-Zƽ��: Z������һ��
					if(L>pos.x)
						pBolt->feature=31;	//Z������һ�࣬���������������������˨
					else
						pBolt->feature=32;	//Z������һ�࣬���Ӱ����������������˨
					pBolt->des_work_norm.norm_style=4;
					pBolt->des_work_norm.nearVector.Set(0,1,0);
					pBolt->des_base_pos.wing_offset_dist = pos.z;
					pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
					//ͨ�����������ʮ�ֲ��ʱ��ת�����굽��׼�ְ�����ϵ�£�
					//Z���������巨����Ϊ(0,-1,0) ����ƫ����ӦΪ��ֵ //wht 2010-07-06 �˴�Ϊ��ʱ��� 
					if(m_pSrcParamPlate==NULL)
						pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.y);
					else
						pBolt->des_base_pos.norm_offset.AddThick(-(LONG)pos.y);
				}
				else if(pos.feature==41)	
				{	//X-Zƽ��: Z������һ��
					if(L>pos.x)
						pBolt->feature=41;	//Z������һ�࣬���������������������˨
					else
						pBolt->feature=42;	//Z������һ�࣬���Ӱ����������������˨
					pBolt->des_work_norm.norm_style=4;
					pBolt->des_work_norm.nearVector.Set(0,1,0);
					pBolt->des_base_pos.wing_offset_dist = pos.z;
					pBolt->ucs.axis_z=pParamPlate->ucs.axis_y;
					pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.y);
				}
				if(pBolt->feature==11||pBolt->feature==21)	//��˨��ʮ�ֲ�巶Χ���޸���˨ͨ��
					pBolt->AddL0Thick(pParamPlate->handle,TRUE);
			}
			else
			{
				pBolt->AddL0Thick(pParamPlate->handle,TRUE);
				pBolt->des_base_pos.wing_offset_dist = pos.y;
				pBolt->des_base_pos.norm_offset.AddThick((LONG)pos.z);
			}
			pBolt->des_work_norm.direction=LayoutBoltPara.m_iDirection;	//0-���� 1-����
			//coord_trans(pos,pPart->ucs,TRUE);
			RAYNO_RECORD *pRayNo;
			for(pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
			{
				if(pRayNo->yCoord==pos.y)
				{	
					pBolt->dwRayNo=pRayNo->dwRayNo;
					break;
				}
			}
			if(pRayNo==NULL)
			{
				pRayNo=rayNoList.append();
				pRayNo->dwRayNo=GetSingleWord(iInitRayNo);
				pRayNo->yCoord=pos.y;
				pBolt->dwRayNo=pRayNo->dwRayNo;
				pRayNo->hPart=pParamPlate->handle;
				iInitRayNo++;
			}
		}
		else	//��������
		{
			pBolt->AddL0Thick(pParamPlate->handle,TRUE);//��˨ͨ��
			m_listBolt.GetItemText(i,2,sText,100);		//��λ��
			double angle = atof(sText)*RADTODEG_COEF;	
			pBolt->des_base_pos.offset_angle = angle;	//��˨����ڻ�׼�ֹܵ�ƫ�ƽǶ�
			m_listBolt.GetItemText(i,1,sText,100);		//��˨���׼�������ĵľ���
			pBolt->des_base_pos.R=atof(sText);			//�ñ���С��0��ʾ��˨Ϊ�ǻ�����
			pBolt->des_work_norm.direction=0;			//�����õ���˨Ĭ�ϳ���
		}
		CLsRef *pLsRef=NULL;
		pLsRef=pParamPlate->AppendLsRef(pBolt->GetLsRef());
		if(pLsRef)
			pLsRef->start1_end2_mid0=(BYTE)pBolt->feature;	//�����˨����λ��
		pBolt->SetModified();
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	//������˨�������ߺ�
	if(m_iTubeEndLjStyle>2)	//�ڲ��������Ͼ������в�����˨
		pParamPlate->GetLsRefList()->ResetBoltRefRayNo(pParamPlate);
	g_pSolidDraw->Draw();
}

//��ע�����ߴ� 
//iOffsetType -1.X�Ḻ���� -2.Y�Ḻ���� 1.X�������� 2.Y��������
static void DimText(CPaintDC *pDc,f2dPoint start,f2dPoint end,double fLen,
					int iOffsetType,int nOffsetDist=0)
{
	CString sLen;
	sLen.Format("%f",fLen);
	SimplifiedNumString(sLen);
	CSize sizeText=pDc->GetTextExtent(sLen);
	int nSpaceDist=5;	//��ע������������֮��ļ�϶ֵ
	f2dPoint dim_pos,dim_start,dim_end,mid_start,mid_end,ext_start,ext_end;
	dim_start=start; 
	dim_end=end;
	if(abs(iOffsetType)==1)
	{	//��ֱ��ע
		if(nOffsetDist==0)	//��עλ������ڱ�ע���ƫ�ƾ���
			nOffsetDist=ftoi(sizeText.cx*0.5)+4;
		dim_start.x+=(iOffsetType*nOffsetDist);
		dim_end.x+=(iOffsetType*nOffsetDist);
		//
		ext_start=dim_start;
		ext_end=dim_end;
		ext_start.x+=(iOffsetType*sizeText.cy*0.25);
		ext_end.x+=(iOffsetType*sizeText.cy*0.25);
		dim_pos.Set(dim_start.x,0.5*(dim_start.y+dim_end.y));
		//����������ע��ճ����ֿռ�
		int nFlag=1;
		if(dim_start.y<dim_end.y)
			nFlag=-1;
		mid_start.Set(dim_start.x,dim_pos.y+0.6*sizeText.cy*nFlag);
		mid_end.Set(dim_start.x,dim_pos.y-0.6*sizeText.cy*nFlag);
	}
	else if(abs(iOffsetType)==2)
	{	//ˮƽ��ע
		iOffsetType/=2;	//����2
		if(nOffsetDist==0)	//��עλ������ڱ�ע���ƫ�ƾ���
			nOffsetDist=ftoi(sizeText.cy*0.6);
		dim_start.y+=(iOffsetType*nOffsetDist);
		dim_end.y+=(iOffsetType*nOffsetDist);
		dim_pos.Set(0.5*(dim_start.x+dim_end.x),dim_start.y);
		//
		ext_start=dim_start;
		ext_end=dim_end;
		ext_start.y+=(iOffsetType*sizeText.cy*0.25);
		ext_end.y+=(iOffsetType*sizeText.cy*0.25);
		//����������ע��ճ����ֿռ�
		int nFlag=1;
		if(dim_start.x<dim_end.x)
			nFlag=-1;
		mid_start.Set(dim_pos.x+0.6*sizeText.cx*nFlag,dim_start.y);
		mid_end.Set(dim_pos.x-0.6*sizeText.cx*nFlag,dim_start.y);
		iOffsetType*=2;	//����2
	}
	//���Ʊ�ע��
	pDc->MoveTo(ftoi(start.x),ftoi(start.y));
	pDc->LineTo(ftoi(ext_start.x),ftoi(ext_start.y));
	pDc->MoveTo(ftoi(end.x),ftoi(end.y));
	pDc->LineTo(ftoi(ext_end.x),ftoi(ext_end.y));
	//
	int nDimSpace=(int)DISTANCE(f3dPoint(dim_start.x,dim_start.y,0),f3dPoint(dim_end.x,dim_end.y,0));
	if(nDimSpace<sizeText.cx)
	{
		pDc->MoveTo(ftoi(dim_start.x),ftoi(dim_start.y));
		pDc->LineTo(ftoi(dim_end.x),ftoi(dim_end.y));
		//���Ʊ�ע����
		dim_pos.x+=sizeText.cx;
		pDc->TextOut(ftoi(dim_pos.x-0.5*sizeText.cx),ftoi(dim_pos.y-sizeText.cy*0.5),sLen);
	}
	else
	{
		pDc->MoveTo(ftoi(dim_start.x),ftoi(dim_start.y));
		pDc->LineTo(ftoi(mid_start.x),ftoi(mid_start.y));
		pDc->MoveTo(ftoi(mid_end.x),ftoi(mid_end.y));
		pDc->LineTo(ftoi(dim_end.x),ftoi(dim_end.y));
		//���Ʊ�ע����
		pDc->TextOut(ftoi(dim_pos.x-0.5*sizeText.cx),ftoi(dim_pos.y-sizeText.cy*0.5),sLen);
	}
}

void CDesignTubeEndLjDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rectDrawRgn; //��Ȼ��ͼ����
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&rectDrawRgn);
	ScreenToClient(&rectDrawRgn);
	int width=rectDrawRgn.Width();
	int height=rectDrawRgn.Height();
	//
	double W=0,L=0,H=0,N=0,S=0,R=0,M=0,T=0,A=0;
	double real_width=0,real_height=0,scale=1;
	UCS_2dSTRU draw_ucs;	//��ͼ����ϵ
	T=m_paramPlate.GetThick();
	//
	if(m_iTubeEndLjStyle==2)
	{	//����
		if(!m_paramPlate.m_bStdPart||(m_paramPlate.m_bStdPart&&(m_iPartType==3||m_iPartType==4)))
		{	//��ͨ|����|���Է���
			m_paramPlate.GetDesignItemValue('W',&W);
			m_paramPlate.GetDesignItemValue('R',&R);
		}
		else 
		{	//ƽ��|�Ժ�����
			m_paramPlate.GetDesignItemValue('D',&W);
			m_paramPlate.GetDesignItemValue('B',&R);
			R*=0.5;
		}
		real_width=real_height=W+10;
		draw_ucs.origin.Set(rectDrawRgn.TopLeft().x+width*0.5,rectDrawRgn.TopLeft().y+height*0.5);
	}
	else if(m_iTubeEndLjStyle>2)
	{	//���
		m_paramPlate.GetDesignItemValue('L',&L);
		m_paramPlate.GetDesignItemValue('H',&H);
		m_paramPlate.GetDesignItemValue('M',&M);
		m_paramPlate.GetDesignItemValue('S',&S);
		m_paramPlate.GetDesignItemValue('N',&N);
		m_paramPlate.GetDesignItemValue('W',&W);
		m_paramPlate.GetDesignItemValue('A',&A);
		m_paramPlate.GetDesignItemValue('R',&R);
		double offset_dist_x=0,offset_dist_y=0.5*height;
		if(m_iTubeEndLjStyle==3)
		{	//U�Ͱ�
			real_width=L+3*W;
			real_height=H+8*W;
			offset_dist_x=((2*W)/(L+3*W))*width;
			offset_dist_y-=(W/(H+8*W))*height;
		}
		else if(m_iTubeEndLjStyle==4)
		{	//��߰�
			real_width=L+4*W;
			real_height=H+2*W;
			offset_dist_x=(W/(L+4*W))*width;
		}
		else if(m_iTubeEndLjStyle==5)
		{	//ʮ�ֲ��
			real_width=M+S+5*N;
			real_height=H+3*N;
			offset_dist_x=(N/(M+S+5*N))*width;
			offset_dist_y+=(N/(H+3*N))*height;
		}
		draw_ucs.origin.Set(rectDrawRgn.TopLeft().x+offset_dist_x,rectDrawRgn.TopLeft().y+offset_dist_y);
	}
	draw_ucs.axis_x.Set(1,0);
	draw_ucs.axis_y.Set(0,-1);
	double scale_x=width/real_width;
	double scale_y=height/real_height;
	scale=min(scale_x,scale_y);
	dc.SelectStockObject(LTGRAY_BRUSH); 
	dc.Rectangle(rectDrawRgn);
	dc.SelectStockObject(HOLLOW_BRUSH);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0,0,0));
	//��������ϵ
	CPen *pOldPen=dc.SelectObject(&red_solid_pen);
	if(m_iTubeEndLjStyle>=2)
	{
		f2dPoint origin=draw_ucs.origin;
		int axis_y_len=ftoi(0.3*height);
		int axis_x_len=axis_y_len;
		//��������ϵX��Y�᳤��ȡ0.8�뾶 ��ΪҪ���ƻ�׼���Լ������߱��
		if(m_iTubeEndLjStyle==2) 
			axis_x_len=axis_y_len=ftoi(W*0.5*scale)+3;
		//X��
		dc.MoveTo(ftoi(origin.x),ftoi(origin.y));
		dc.LineTo(ftoi(origin.x+axis_x_len),ftoi(origin.y));
		//���Ƽ�ͷ
		dc.LineTo(ftoi(origin.x+axis_x_len-8),ftoi(origin.y-4));
		dc.MoveTo(ftoi(origin.x+axis_x_len),ftoi(origin.y));
		dc.LineTo(ftoi(origin.x+axis_x_len-8),ftoi(origin.y+4));
		//Y��
		dc.SelectObject(&green_solid_pen);
		dc.MoveTo(ftoi(origin.x),ftoi(origin.y));
		dc.LineTo(ftoi(origin.x),ftoi(origin.y-axis_y_len));
		//���Ƽ�ͷ
		dc.LineTo(ftoi(origin.x-4),ftoi(origin.y-axis_y_len+8));
		dc.MoveTo(ftoi(origin.x),ftoi(origin.y-axis_y_len));
		dc.LineTo(ftoi(origin.x+4),ftoi(origin.y-axis_y_len+8));
		dc.SelectObject(pOldPen);
	}
	POINT topLeft,bottomRight;
	if(m_iTubeEndLjStyle==2)
	{	//����
		double fTubeR=R*scale-1;
		double fTubeThick=min(m_pCurLineTube->GetThick()*scale,8);
		int i;
		for(i=0;i<4;i++)
		{
			int nRadius=ftoi(0.5*W*scale);
			if(i==1)
				nRadius=ftoi(R*scale);
			else if(i==2)
			{
				pOldPen=dc.SelectObject(&green_dash_pen);
				nRadius=ftoi(LayoutBoltPara.m_fBoltLayoutD*0.5*scale);
			}
			//���Ƹֹ�ʾ��ͼ
			else if(i==3)
				nRadius=ftoi(fTubeR-fTubeThick);
			topLeft.x=ftoi(draw_ucs.origin.x)-nRadius;
			topLeft.y=ftoi(draw_ucs.origin.y)-nRadius;
			bottomRight.x=ftoi(draw_ucs.origin.x)+nRadius;
			bottomRight.y=ftoi(draw_ucs.origin.y)+nRadius;
			dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
			if(i==2)
				dc.SelectObject(pOldPen);
		}
		//��ע�����ߴ�
		pOldPen=dc.SelectObject(&purple_solid_pen);
		double tube_r=m_pCurLineTube->GetDiameter()*0.5;
		for(i=0;i<3;i++)
		{
			f2dPoint dim_start,dim_end;
			double cur_r=LayoutBoltPara.m_fBoltLayoutD*0.5;
			if(i==1)
				cur_r=R;
			else if(i==2)
				cur_r=W*0.5;
			dim_start.Set(0,cur_r*scale);
			dim_end.Set(0,-cur_r*scale);
			coord_trans(dim_start,draw_ucs,TRUE);
			coord_trans(dim_end,draw_ucs,TRUE);
			if(i==0)
				DimText(&dc,dim_start,dim_end,2*cur_r,-1,ftoi(cur_r*scale*1.2));
			else if(i==2)
				DimText(&dc,dim_start,dim_end,2*cur_r,-1,ftoi(cur_r*scale*1.5));
			else
				DimText(&dc,dim_start,dim_end,2*cur_r,-1,ftoi(cur_r*0.5*scale));
		}
		dc.SelectObject(pOldPen);
		//���Ƹֹܻ�׼���뺸���߱��
		for(i=0;i<2;i++)
		{
			if(!m_pCurLineTube->m_bHasWeldRoad&&i==1)
				continue;
			int nInnerR=ftoi(fTubeR-fTubeThick);
			int nOutterR=ftoi(fTubeR);
			CPoint ptVertex[3];
			f2dPoint tempVertexArr[3];
			UCS_2dSTRU temp_ucs=draw_ucs;
			f3dPoint rot_axis(0,0,1);
			if(m_bStartEnd)
				rot_axis*=-1.0;
			double fOffsetAngle=0;
			if(m_iPartType!=0&&m_iTubeEndLjStyle==2)	//��ָ���Ƕȵ�����׼����X�᷽�� wht 11-05-26
				fOffsetAngle=LayoutBoltPara.m_fOffsetAngle*RADTODEG_COEF;
			if(i==1)
			{	//������
				double fWeldRoadAngle=-m_pCurLineTube->CalWeldRoadAngle();
				if(LayoutBoltPara.m_iStartEdge==1)
					fWeldRoadAngle*=-1.0;
				fOffsetAngle+=fWeldRoadAngle;
			}
			if(fabs(fOffsetAngle)>EPS)
			{	//��ָ������תָ���Ƕ�
				f3dPoint axis(temp_ucs.axis_x.x,temp_ucs.axis_x.y,0);
				RotateVectorAroundVector(axis,fOffsetAngle,rot_axis);
				temp_ucs.axis_x.Set(axis.x,axis.y);
				axis.Set(temp_ucs.axis_y.x,temp_ucs.axis_y.y,0);
				RotateVectorAroundVector(axis,fOffsetAngle,rot_axis);
				temp_ucs.axis_y.Set(axis.x,axis.y);
			}
			tempVertexArr[0].Set(nInnerR,fTubeThick);
			tempVertexArr[1].Set(nInnerR,-fTubeThick);
			tempVertexArr[2].Set(nOutterR,0);
			int j;
			for(j=0;j<3;j++)
			{
				coord_trans(tempVertexArr[j],temp_ucs,TRUE);
				ptVertex[j].x=ftoi(tempVertexArr[j].x);
				ptVertex[j].y=ftoi(tempVertexArr[j].y);
			}
			for(j=0;j<3;j++)
			{
				dc.MoveTo(ftoi(tempVertexArr[j].x),ftoi(tempVertexArr[j].y));
				dc.LineTo(ftoi(tempVertexArr[(j+1)%3].x),ftoi(tempVertexArr[(j+1)%3].y));
			}
			CRgn rgn;
			rgn.CreatePolygonRgn(ptVertex,3,ALTERNATE);
			CBrush curBrush;
			if(LayoutBoltPara.m_iStartEdge==1)
			{	//�Ժ�����Ϊ��ʼ��
				if(i==0) //�����߱�־
					curBrush.CreateSolidBrush(RGB(0,0,0));
				else	 //��׼�߱�־
					curBrush.CreateSolidBrush(RGB(0,255,0));
			}
			else
			{	//�Ի�׼��Ϊ��ʼ��
				if(i==0) //��׼�߱�־	
					curBrush.CreateSolidBrush(RGB(0,255,0));
				else	 //�����߱�־
					curBrush.CreateSolidBrush(RGB(0,0,0));
			}
			dc.FillRgn(&rgn,&curBrush);
		}
		//������˨ ����˨�б��еĲ���������˨ wht 11-05-09
		pOldPen=dc.SelectObject(&red_solid_pen);
		int iCurSel=-1;
		POSITION pos = m_listBolt.GetFirstSelectedItemPosition();
		if(pos!=NULL)
			iCurSel = m_listBolt.GetNextSelectedItem(pos);
		int nCount=m_listBolt.GetItemCount();
		for(i=0;i<nCount;i++)
		{
			char sText[100]="";
			m_listBolt.GetItemText(i,0,sText,100);
			int nBoltR=ftoi(atoi(sText+1)*0.5*scale);
			m_listBolt.GetItemText(i,2,sText,100);		//��λ��
			double angle = atof(sText)*RADTODEG_COEF;	//��˨����ڻ�׼�ֹܵ�ƫ�ƽǶ�
			m_listBolt.GetItemText(i,1,sText,100);		//��˨���׼�������ĵľ���
			double R=atof(sText)*scale;			
			f3dPoint tempPoint(R,0,0);
			rotate_point_around_axis(tempPoint,angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
			f2dPoint curPoint(tempPoint.x,tempPoint.y);
			coord_trans(curPoint,draw_ucs,TRUE);
			topLeft.x=ftoi(curPoint.x)-nBoltR;
			topLeft.y=ftoi(curPoint.y)-nBoltR;
			bottomRight.x=ftoi(curPoint.x)+nBoltR;
			bottomRight.y=ftoi(curPoint.y)+nBoltR;
			if(iCurSel==i)	//��ǰѡ�е���˨
				dc.SelectObject(&green_solid_pen);
			else
				dc.SelectObject(&red_solid_pen);
			dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
		}
		dc.SelectObject(pOldPen);
		//���ɸ��Ӱ�ʾ��ͼ
		double sector_angle=(2*Pi)/LayoutBoltPara.m_nBoltNum;
		if(m_bCreateAttachPlate&&(m_iPartType==0||m_iPartType>2))
		{	
			double start_angle=AttachPlatePara.m_fAngle*RADTODEG_COEF;
			f3dPoint startPt(R*scale,0,0);
			f3dPoint endPt((R+AttachPlatePara.B)*scale,0,0);
			double fPlateThick=AttachPlatePara.m_nThick*scale;
			for(i=0;i<AttachPlatePara.m_nPlateNum;i++)
			{
				f2dPoint vertex_arr[4];
				f3dPoint tempStart=startPt;
				f3dPoint tempEnd=endPt;
				rotate_point_around_axis(tempStart,start_angle+i*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
				rotate_point_around_axis(tempEnd,start_angle+i*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
				f3dPoint len_vec=tempEnd-tempStart;
				normalize(len_vec);
				f3dPoint offset_vec(-len_vec.y,len_vec.x,0);
				f3dPoint temp_vertex=tempStart+offset_vec*fPlateThick*0.5;
				vertex_arr[0].Set(temp_vertex.x,temp_vertex.y);
				temp_vertex=tempStart-offset_vec*fPlateThick*0.5;
				vertex_arr[1].Set(temp_vertex.x,temp_vertex.y);
				temp_vertex=tempEnd-offset_vec*fPlateThick*0.5;
				vertex_arr[2].Set(temp_vertex.x,temp_vertex.y);
				temp_vertex=tempEnd+offset_vec*fPlateThick*0.5;
				vertex_arr[3].Set(temp_vertex.x,temp_vertex.y);
				for(int j=0;j<4;j++)
					coord_trans(vertex_arr[j],draw_ucs,TRUE);
				if(iCurSel==i)
					pOldPen=dc.SelectObject(&green_solid_pen);
				for(j=0;j<4;j++)
				{
					dc.MoveTo(ftoi(vertex_arr[j].x),ftoi(vertex_arr[j].y));
					dc.LineTo(ftoi(vertex_arr[(j+1)%4].x),ftoi(vertex_arr[(j+1)%4].y));
				}
				if(iCurSel==i)
					dc.SelectObject(pOldPen);
			}	
		}
	}
	else if(m_iTubeEndLjStyle>2)
	{	//���Ͳ�塢U�Ͳ�塢ʮ�ֲ��
		int i=0;
		f2dPoint dim_start,dim_end;	
		double fTubeR=m_pCurLineTube->GetDiameter()*0.5;
		double fTubeThick=max(m_pCurLineTube->GetThick(),8);
		f2dPoint vertex_arr[8];
		double fCutAngleLen=N,fCutAngleWidth=S;
		if(m_iTubeEndLjStyle==5)
			fCutAngleLen=fCutAngleWidth=0;
		vertex_arr[0].Set(0,0.5*H*scale);
		vertex_arr[1].Set(0,-0.5*H*scale);
		vertex_arr[2].Set((L-fCutAngleLen)*scale,-0.5*H*scale);
		vertex_arr[3].Set(L*scale,(fCutAngleWidth-0.5*H)*scale);
		vertex_arr[4].Set(L*scale,(-fCutAngleWidth+0.5*H)*scale);
		vertex_arr[5].Set((L-fCutAngleLen)*scale,0.5*H*scale);
		//�ֹܶ˵�
		vertex_arr[6].Set(M*scale,fTubeR*scale);
		vertex_arr[7].Set(M*scale,-fTubeR*scale);
		for(i=0;i<8;i++)
			coord_trans(vertex_arr[i],draw_ucs,TRUE);
		BOOL bOutside=FALSE;
		if((work_norm*m_pCurLineTube->WorkPlaneNorm()>0&&ftol(setupInfo.workNorm.rot_ang)==180)
			||(work_norm*m_pCurLineTube->WorkPlaneNorm()<0&&ftol(setupInfo.workNorm.rot_ang)==0))
			bOutside=TRUE;
		//���ƹ�������ͼ
		for(i=0;i<6;i++)
		{
			f2dPoint start=vertex_arr[i];
			f2dPoint end=vertex_arr[(i+1)%6];
			dc.MoveTo(ftoi(start.x),ftoi(start.y));
			dc.LineTo(ftoi(end.x),ftoi(end.y));
			if(m_iTubeEndLjStyle==4&&(i==1||i==5))
			{	//���ƾ��
				int nFlag=1;
				if(i==1)
					nFlag=-1;
				if(!bOutside)
					pOldPen=dc.SelectObject(&dash_pen);
				dc.MoveTo(ftoi(start.x),ftoi(start.y+nFlag*T*scale));
				dc.LineTo(ftoi(end.x),ftoi(end.y+nFlag*T*scale));
				if(!bOutside)
					dc.SelectObject(pOldPen);
			}
		}
		//��ע���������߳ߴ�
		pOldPen=dc.SelectObject(&purple_solid_pen);
		if(m_iTubeEndLjStyle>2)
		{	//H
			if(m_iTubeEndLjStyle==5)
				fCutAngleLen=A+S-(L-M);
			dim_start.Set(vertex_arr[2].x+fCutAngleLen*scale,vertex_arr[2].y);
			dim_end.Set(vertex_arr[5].x+fCutAngleLen*scale,vertex_arr[5].y);
			DimText(&dc,dim_start,dim_end,H,1);	
			//L
			dim_start.Set(vertex_arr[0].x,vertex_arr[0].y);
			dim_end.Set(vertex_arr[4].x,vertex_arr[4].y-fCutAngleWidth*scale);
			DimText(&dc,dim_start,dim_end,L,-2);
			//M
			dim_start.Set(vertex_arr[1].x,vertex_arr[1].y);
			dim_end.Set(vertex_arr[1].x+M*scale,vertex_arr[1].y);
			DimText(&dc,dim_start,dim_end,M,2);
			if(m_iTubeEndLjStyle==3||m_iTubeEndLjStyle==4)
			{	//N
				dim_start.Set(vertex_arr[2].x,vertex_arr[2].y);
				dim_end.Set(vertex_arr[2].x+N*scale,vertex_arr[2].y);
				DimText(&dc,dim_start,dim_end,N,2);
				//S
				dim_start.Set(vertex_arr[5].x,vertex_arr[5].y);
				dim_end.Set(vertex_arr[5].x,vertex_arr[5].y+S*scale);
				DimText(&dc,dim_start,dim_end,S,-1);
			}
			else if(m_iTubeEndLjStyle==5)
			{	//A
				dim_start.Set(vertex_arr[1].x+M*scale,vertex_arr[1].y);
				dim_end.Set(vertex_arr[1].x+(A+M)*scale,vertex_arr[1].y);
				DimText(&dc,dim_start,dim_end,A,2);	
			}
			//��ע�ֹ�ֱ��
			dim_start.Set(0,fTubeR*scale);
			dim_end.Set(0,-fTubeR*scale);
			coord_trans(dim_start,draw_ucs,TRUE);
			coord_trans(dim_end,draw_ucs,TRUE);
			DimText(&dc,dim_start,dim_end,2*fTubeR,-1);
		}
		dc.SelectObject(pOldPen);
		//���Ƹֹ�ʾ��ͼ
		double tube_offset_len=0.5*W;
		if(m_iTubeEndLjStyle==5)
			tube_offset_len=0.5*N;
		dc.MoveTo(ftoi(vertex_arr[6].x),ftoi(vertex_arr[6].y));
		dc.LineTo(ftoi(vertex_arr[7].x),ftoi(vertex_arr[7].y));
		dc.MoveTo(ftoi(vertex_arr[6].x),ftoi(vertex_arr[6].y));
		dc.LineTo(ftoi(vertex_arr[6].x-(M+tube_offset_len)*scale),ftoi(vertex_arr[6].y));
		dc.MoveTo(ftoi(vertex_arr[7].x),ftoi(vertex_arr[7].y));
		dc.LineTo(ftoi(vertex_arr[7].x-(M+tube_offset_len)*scale),ftoi(vertex_arr[7].y));
		pOldPen=dc.SelectObject(&green_dash_pen);
		dc.MoveTo(ftoi(vertex_arr[6].x),ftoi(vertex_arr[6].y+fTubeThick*scale));
		dc.LineTo(ftoi(vertex_arr[6].x-(M+tube_offset_len)*scale),ftoi(vertex_arr[6].y+fTubeThick*scale));
		dc.MoveTo(ftoi(vertex_arr[7].x),ftoi(vertex_arr[7].y-fTubeThick*scale));
		dc.LineTo(ftoi(vertex_arr[7].x-(M+tube_offset_len)*scale),ftoi(vertex_arr[7].y-fTubeThick*scale));
		dc.SelectObject(pOldPen);
		
		if(m_iTubeEndLjStyle==3)
		{	//����U�Ͳ�����ͼ
			vertex_arr[0].Set((0.5*W+T)*scale,(0.5*W+T)*scale);
			vertex_arr[1].Set(L*scale,(0.5*W+T)*scale);
			vertex_arr[2].Set(L*scale,0.5*W*scale);
			vertex_arr[3].Set((0.5*W+T)*scale,0.5*W*scale);
			vertex_arr[4].Set((0.5*W+T)*scale,-0.5*W*scale);
			vertex_arr[5].Set(L*scale,-0.5*W*scale);
			vertex_arr[6].Set(L*scale,-(0.5*W+T)*scale);
			vertex_arr[7].Set((0.5*W+T)*scale,-(0.5*W+T)*scale);
			//��ʱ����ϵ
			UCS_2dSTRU temp_ucs=draw_ucs;
			temp_ucs.origin.y+=(0.5*H+3*W)*scale;
			for(i=0;i<8;i++)
				coord_trans(vertex_arr[i],temp_ucs,TRUE);
			for(i=0;i<8;i++)
			{
				f2dPoint start=vertex_arr[i];
				f2dPoint end=vertex_arr[(i+1)%8];
				dc.MoveTo(ftoi(start.x),ftoi(start.y));
				dc.LineTo(ftoi(end.x),ftoi(end.y));
			}
			f2dPoint center((0.5*W+T)*scale,0);
			coord_trans(center,temp_ucs,TRUE);
			for(i=0;i<8;i++)
			{
				f2dPoint start=vertex_arr[i];
				f2dPoint end=vertex_arr[(i+1)%8];
				if(i==3)
					dc.AngleArc(ftoi(center.x),ftoi(center.y),ftoi(0.5*W*scale),90,180);
				else if(i==7)
					dc.AngleArc(ftoi(center.x),ftoi(center.y),ftoi((0.5*W+T)*scale),90,180);
				else
				{
					dc.MoveTo(ftoi(start.x),ftoi(start.y));
					dc.LineTo(ftoi(end.x),ftoi(end.y));
				}
			}
			//��עW�ߴ�
			pOldPen=dc.SelectObject(&purple_solid_pen);
			dim_start.Set(vertex_arr[2].x,vertex_arr[2].y);
			dim_end.Set(vertex_arr[2].x,vertex_arr[2].y+W*scale);
			DimText(&dc,dim_start,dim_end,W,1);
			dc.SelectObject(pOldPen);
		}
		else if(m_iTubeEndLjStyle==4) 
		{	//���Ͳ����ʾ��ͼ
			vertex_arr[0].Set(0,0.5*H*scale);
			vertex_arr[1].Set(0,-0.5*H*scale);
			vertex_arr[2].Set(W*scale,-0.5*H*scale);
			vertex_arr[3].Set(W*scale,(T-0.5*H)*scale);
			vertex_arr[4].Set(T*scale,(T-0.5*H)*scale);
			vertex_arr[5].Set(T*scale,(-T+0.5*H)*scale);
			vertex_arr[6].Set(W*scale,(-T+0.5*H)*scale);
			vertex_arr[7].Set(W*scale,0.5*H*scale);
			//��ʱ����ϵ
			UCS_2dSTRU temp_ucs=draw_ucs;
			temp_ucs.origin.x+=(L+1.5*W)*scale;
			for(i=0;i<8;i++)
				coord_trans(vertex_arr[i],temp_ucs,TRUE);
			for(i=0;i<8;i++)
			{
				f2dPoint start=vertex_arr[i];
				f2dPoint end=vertex_arr[(i+1)%8];
				dc.MoveTo(ftoi(start.x),ftoi(start.y));
				dc.LineTo(ftoi(end.x),ftoi(end.y));
			}
			//��עW�ߴ�
			pOldPen=dc.SelectObject(&purple_solid_pen);
			dim_start.Set(vertex_arr[1].x,vertex_arr[1].y);
			dim_end.Set(vertex_arr[1].x+W*scale,vertex_arr[1].y);
			DimText(&dc,dim_start,dim_end,W,2);
			dc.SelectObject(pOldPen);
		}
		else 
		{	//ʮ�ֲ���������
			double offset_x=M+A,offset_y=R;
			for(int j=0;j<16;j++)
			{
				//��ʱ����ϵ
				UCS_2dSTRU temp_ucs=draw_ucs;
				if(j==0)
				{	//����
					vertex_arr[0].Set(0,0.5*T*scale);
					vertex_arr[1].Set(0,-0.5*T*scale);
					vertex_arr[2].Set(L*scale,-0.5*T*scale);
					vertex_arr[3].Set(L*scale,0.5*T*scale);
				}
				else if(j==1||j==2)
				{	//ˮƽ����
					int nFlag=1;
					if(j==2)
						nFlag=-1;
					vertex_arr[0].Set(offset_x*scale,nFlag*offset_y*scale);
					vertex_arr[1].Set((offset_x+S)*scale,nFlag*offset_y*scale);
					vertex_arr[2].Set((offset_x+S)*scale,nFlag*(offset_y+N)*scale);
					vertex_arr[3].Set(offset_x*scale,nFlag*(offset_y+N)*scale);
				}
				else if(j==3||j==4)
				{	//��ֱ����
					int nFlag=1;
					if(j==4)
						nFlag=-1;
					vertex_arr[0].Set(offset_x*scale,nFlag*0.5*T*scale);
					vertex_arr[1].Set((offset_x+S)*scale,nFlag*0.5*T*scale);
					vertex_arr[2].Set((offset_x+S)*scale,nFlag*(0.5*T+W)*scale);
					vertex_arr[3].Set(offset_x*scale,nFlag*(0.5*T+W)*scale);
				}
				else if(j==5)
				{	//������ͼ
					vertex_arr[0].Set(offset_x*scale,offset_y*scale);
					vertex_arr[1].Set((offset_x+S)*scale,offset_y*scale);
					vertex_arr[2].Set((offset_x+S)*scale,(offset_y+N)*scale);
					vertex_arr[3].Set(offset_x*scale,(offset_y+N)*scale);
					//��������ϵ
					temp_ucs.origin.y-=2*N*scale;
				}
				//ʮ�ֲ�����ͼ
				else
				{	//��������ϵ
					temp_ucs.origin.x+=(M+A+S+2.5*N)*scale;
					if(j==6)
					{	
						vertex_arr[0].Set(-0.5*T*scale,0.5*H*scale);
						vertex_arr[1].Set(0.5*T*scale,0.5*H*scale);
						vertex_arr[2].Set(0.5*T*scale,-0.5*H*scale);
						vertex_arr[3].Set(-0.5*T*scale,-0.5*H*scale);
					}
					else if(j==7)
					{
						vertex_arr[0].Set(-0.5*H*scale,0.5*T*scale);
						vertex_arr[1].Set(0.5*H*scale,0.5*T*scale);
						vertex_arr[2].Set(0.5*H*scale,-0.5*T*scale);
						vertex_arr[3].Set(-0.5*H*scale,-0.5*T*scale);
					}
					else if(j>7&&j<12)
					{
						int nFlagX=1,nFlagY=1;
						if(j==9)
						{
							nFlagX=-1;
							nFlagY=1;
						}
						else if(j==10)
						{
							nFlagX=-1;
							nFlagY=-1;
						}
						else if(j==11)
						{
							nFlagX=1;
							nFlagY=-1;
						}
						vertex_arr[0].Set(nFlagX*0.5*T*scale,nFlagY*R*scale);
						vertex_arr[1].Set(nFlagX*(0.5*T+W)*scale,nFlagY*R*scale);
						vertex_arr[2].Set(nFlagX*(0.5*T+W)*scale,nFlagY*(R+N)*scale);
						vertex_arr[3].Set(nFlagX*0.5*T*scale,nFlagY*(R+N)*scale);
					}
					else
					{
						int nFlagX=1,nFlagY=1;
						if(j==13)
						{
							nFlagX=-1;
							nFlagY=1;
						}
						else if(j==14)
						{
							nFlagX=-1;
							nFlagY=-1;
						}
						else if(j==15)
						{
							nFlagX=1;
							nFlagY=-1;
						}
						vertex_arr[0].Set(nFlagX*R*scale,nFlagY*0.5*T*scale);
						vertex_arr[1].Set(nFlagX*R*scale,nFlagY*(0.5*T+W)*scale);
						vertex_arr[2].Set(nFlagX*(R+N)*scale,nFlagY*(0.5*T+W)*scale);
						vertex_arr[3].Set(nFlagX*(R+N)*scale,nFlagY*0.5*T*scale);
					}
				}
				for(i=0;i<4;i++)
					coord_trans(vertex_arr[i],temp_ucs,TRUE);
				for(i=0;i<4;i++)
				{
					f2dPoint start=vertex_arr[i];
					f2dPoint end=vertex_arr[(i+1)%4];
					dc.MoveTo(ftoi(start.x),ftoi(start.y));
					dc.LineTo(ftoi(end.x),ftoi(end.y));
				}
				//�ߴ��ע
				pOldPen=dc.SelectObject(&purple_solid_pen);
				if(j==5)
				{	//��עS�ߴ�
					dim_start.Set(vertex_arr[3].x,vertex_arr[3].y);
					dim_end.Set(vertex_arr[3].x+S*scale,vertex_arr[3].y);
					DimText(&dc,dim_start,dim_end,S,-2);
					//��עN�ߴ�
					dim_start.Set(vertex_arr[2].x,vertex_arr[2].y);
					dim_end.Set(vertex_arr[2].x,vertex_arr[2].y+N*scale);
					DimText(&dc,dim_start,dim_end,N,1);
				}
				else if(j==6)
				{	//��עR�ߴ�
					dim_start.Set(0,(0.5*T+W)*scale);
					dim_end.Set(R*scale,(0.5*T+W)*scale);
					coord_trans(dim_start,temp_ucs,TRUE);
					coord_trans(dim_end,temp_ucs,TRUE);
					DimText(&dc,dim_start,dim_end,R,-2,ftoi(0.5*H*scale)+10);
				}
				dc.SelectObject(pOldPen);
			}
		}
		//������˨
		pOldPen=dc.SelectObject(&red_solid_pen);
		int nCount=m_listBolt.GetItemCount();
		for(i=0;i<nCount;i++)
		{
			char sText[100]="";
			DWORD dwFlag=m_listBolt.GetItemData(i);	//��˨��־λ
			WORD h=*((WORD*)&dwFlag);
			BOOL bHighLight=m_listBolt.GetItemState(i,LVNI_ALL|LVNI_SELECTED);
			m_listBolt.GetItemText(i,0,sText,100);
			int nBoltR=ftoi(atoi(sText+1)*0.5*scale);
			f3dPoint pos;
			m_listBolt.GetItemText(i,1,sText,100);
			pos.x=atof(sText);
			m_listBolt.GetItemText(i,2,sText,100);
			pos.y=atof(sText);
			m_listBolt.GetItemText(i,3,sText,100);
			pos.z=atof(sText);
			if(m_paramPlate.m_iParamType==TYPE_XEND&&h>=100)
				continue;	//������ʮ�ֲ��XY����˨
			f2dPoint curPoint(pos.x*scale,pos.y*scale);
			if(m_paramPlate.m_iParamType==TYPE_XEND&&pos.y>0)
			{	//��ʱ����ϵ
				UCS_2dSTRU temp_ucs=draw_ucs;
				temp_ucs.origin.y-=2*N*scale;
				coord_trans(curPoint,temp_ucs,TRUE);
				topLeft.x=ftoi(curPoint.x)-nBoltR;
				topLeft.y=ftoi(curPoint.y)-nBoltR;
				bottomRight.x=ftoi(curPoint.x)+nBoltR;
				bottomRight.y=ftoi(curPoint.y)+nBoltR;
				if(bHighLight)
				{	//��ǰѡ�е���˨
					pOldPen=dc.SelectObject(&green_solid_pen);
					dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
					dc.SelectObject(pOldPen);
				}
				else
					dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
			}
			curPoint.Set(pos.x*scale,pos.y*scale);	//���¸�ֵ
			coord_trans(curPoint,draw_ucs,TRUE);
			topLeft.x=ftoi(curPoint.x)-nBoltR;
			topLeft.y=ftoi(curPoint.y)-nBoltR;
			bottomRight.x=ftoi(curPoint.x)+nBoltR;
			bottomRight.y=ftoi(curPoint.y)+nBoltR;
			if(bHighLight)
			{	//��ǰѡ�е���˨
				pOldPen=dc.SelectObject(&green_solid_pen);
				dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
				dc.SelectObject(pOldPen);
			}
			else
				dc.Ellipse(topLeft.x,topLeft.y,bottomRight.x,bottomRight.y);
		}
		dc.SelectObject(pOldPen);
	}
	dc.SelectObject(pOldPen);
}
//���¸ֹ�����ͷ��Ϣ
void CDesignTubeEndLjDlg::UpdateTubeOddment()
{	//�Զ�����ֹ�����ͷ
	if(m_iTubeEndLjStyle==0)
	{	//ƽ���и�
		f3dPoint curPos,tube_len_vec;
		if(m_bStartEnd)
		{
			curPos=m_pCurLineTube->Start();
			tube_len_vec=m_pCurLineTube->Start()-m_pCurLineTube->End();	//�ն�==>ʼ��
		}
		else
		{
			curPos=m_pCurLineTube->End();
			tube_len_vec=m_pCurLineTube->End()-m_pCurLineTube->Start();	//ʼ��==>�ն�
		}
		normalize(tube_len_vec);
		if(!cutPlaneNorm.IsZero()&&tube_len_vec*cutPlaneNorm>0)
		{	//���Ϊƽ���и��Զ�����һ����С�ĸ�ͷ
			f3dPoint inters,temp_vec;
			Int3dlf(inters,curPos,tube_len_vec,cutPlaneOrigin,cutPlaneNorm);
			double min_dist=DISTANCE(inters,curPos);
			double angle=cal_angle_of_2vec(tube_len_vec,cutPlaneNorm);
			double tana=fabs(tan(angle));
			temp_vec=curPos-inters;
			normalize(temp_vec);
			if(temp_vec*tube_len_vec>0)	//��Ҫ���ø�ͷ
				min_dist=-(min_dist-(m_pCurLineTube->GetDiameter()*0.5*tana));
			else
				min_dist+=(m_pCurLineTube->GetDiameter()*0.5*tana);
			setupInfo.m_fOddment=ftoi(min_dist);
		}
	}
	else if(m_iTubeEndLjStyle==1)
	{	//�ֹ����
		if(m_bFaceToFace)
			setupInfo.m_fOddment=0;	//�Խ��������ͷΪ0
		else 
		{	//�ǶԽ����ʱ����ֹ�����ͷ wht 11-05-10
			double min_dist=0;
			double fSamplingLength=2.5;
			int n = ftoi(Pi*m_pCurLineTube->GetDiameter()/fSamplingLength);
			int uSlices=max(n,8);
			uSlices=min(n,288);
			double piece_angle=2*Pi/uSlices;
			double radius=m_pCurLineTube->GetDiameter()*0.5;
			f3dPoint line_pick_start=m_pCurLineTube->End()+m_pCurLineTube->ucs.axis_x*radius;
			f3dPoint line_vec=m_pCurLineTube->End()-m_pCurLineTube->Start();
			f3dPoint end_pos=m_pCurLineTube->End();
			if(m_bStartEnd)
			{
				line_pick_start=m_pCurLineTube->Start()+m_pCurLineTube->ucs.axis_x*radius;
				line_vec=m_pCurLineTube->Start()-m_pCurLineTube->End();
				end_pos=m_pCurLineTube->Start();
			}
			normalize(line_vec);
			//�ֱ������������׼�ֹܼ��㵱ǰ�ֹܸ�ͷ 
			long hanld_arr[2]={m_hDatumTube1,m_hDatumTube2};
			for(int j=0;j<2;j++)
			{
				CLDSLineTube *pTransTube=(CLDSLineTube*)console.FromPartHandle(hanld_arr[j],CLS_LINETUBE);
				if(pTransTube==NULL)
					continue;
				double trans_radius=pTransTube->GetDiameter()*0.5;
				for(int i=0;i<uSlices;i++)
				{
					f3dPoint line_pick=line_pick_start;
					if(i>0)
						rotate_point_around_axis(line_pick,piece_angle*i,m_pCurLineTube->Start(),m_pCurLineTube->End());
					f3dPoint inters1,inters2;
					if(Int3dlc(&inters1,&inters2,line_pick,line_vec,pTransTube->Start(),pTransTube->End(),trans_radius,trans_radius)>0)
					{
						f3dPoint vec1=inters1-end_pos;
						f3dPoint vec2=inters2-end_pos;
						double dd1=vec1*line_vec;
						double dd2=vec2*line_vec;
						if(min_dist>=0)
							min_dist=dd1<dd2?dd1:dd2;
						else 
						{
							if(dd1<dd2&&dd1<0&&dd1>min_dist)
								min_dist=dd1;	//��һ��������Ч
							else if(dd1>dd2&&dd2<0&&dd2>min_dist)
								min_dist=dd2;	//�ڶ���������Ч
						}
					}
				}
			}
			//��5Բ����+10mm�ӹ���� wht 11-05-10
			if(min_dist<0)
				setupInfo.m_fOddment=min(0,(ftoi(min_dist)/5)*5+10);
		}
	}
	else if(m_iTubeEndLjStyle==2)
	{	//���·�������ͷ wht 11-01-23
		/* ��Ŀǰ�öԻ�������ʾ����ͷֵ�Ѹ�Ϊ���������������ͷ,����������޸�ʱ�Ѳ�Ӱ������ͷ��ʾֵ,�ʴ�ע�͵����´��� wjh-2015.3.14
		double fOddmentOffset=0;
		if(GetPartType()==0)		//��ͨ����
			m_paramPlate.GetDesignItemValue('H',&fOddmentOffset);
		else if(GetPartType()==1)	//ƽ������
			fOddmentOffset=m_pFlPParam->H1;
		else if(GetPartType()==2)	//�Ժ�����
			fOddmentOffset=m_pFlDParam->H;
		else if(GetPartType()==3)
			fOddmentOffset=m_pRigidFlParam->fH;
		else if(GetPartType()==4)
			fOddmentOffset=m_pFlexibleFlParam->fH;
		*/
		if(m_bStartEnd)
			setupInfo.m_fOddment=m_pCurLineTube->startOdd(true);//-fOddmentOffset;
		else
			setupInfo.m_fOddment=m_pCurLineTube->endOdd(true);//-fOddmentOffset;		
	}
	else if(m_iTubeEndLjStyle>2)
	{
		DESIGNODDMENT * pCurOddment=&(m_pCurLineTube->desEndOdd);
		if(m_bStartEnd)
			pCurOddment=&(m_pCurLineTube->desStartOdd);
		//��Ӧ���Զ��жϸֹ���ײ��׼�˼�,ĳЩ����û����ܲ�ϣ���Զ���������ͷ wht 11-05-09
		//long hNodeFather=m_pBaseNode->hFatherPart;
		//if(pCurOddment->m_hRefPart1<0x20&&hNodeFather!=m_pCurLineTube->handle)
		//	pCurOddment->m_hRefPart1=hNodeFather;
		if(pCurOddment->m_iOddCalStyle==0)	//������ײ��������ͷ
			setupInfo.m_fOddment=m_pCurLineTube->CalOddment(m_bStartEnd,&m_paramPlate);
	}
	CString sOddment="";
	sOddment.Format("%f",setupInfo.m_fOddment);
	SimplifiedNumString(sOddment);
	m_propList.SetItemPropValue(CDesignTubeEndLjDlg::GetPropID("m_fOddment"),sOddment);
}

void CDesignTubeEndLjDlg::OnDelCurEndlj() 
{
	if(MessageBox( "��Ҫ����ɾ��������(��)��(Y/N)?","LatticeTower",MB_YESNO)!=IDYES)
		return;
	if(m_pParamPlate)
	{
		BOLTSET boltSet;
		for(CLsRef *pLsRef=m_pParamPlate->GetFirstLsRef();pLsRef;pLsRef=m_pParamPlate->GetNextLsRef())
			boltSet.append(pLsRef->GetLsPtr());
		for(CLDSBolt *pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
			Ta.DelBoltAndRelativeBoltRef(pBolt->handle);
		Ta.DelTubeEndLj(m_pParamPlate->handle);
		//g_pSolidDraw->BuildDisplayList();
		g_pSolidDraw->Draw();
	}
	if(m_iTubeEndLjStyle==1)
	{	//�ֹ����
		m_hDatumTube1=m_hDatumTube2=0;
		m_propList.SetItemPropValue(GetPropID("m_hDatumTube1"),CString("0X0"));
		if(!m_bFaceToFace)
			m_propList.SetItemPropValue(GetPropID("m_hDatumTube2"),CString("0X0"));
		else
			m_bFaceToFace=FALSE;
	}
	else if(m_iTubeEndLjStyle==0)
	{	//�ֹ����
		m_hWeldLinkObj=0;
		m_propList.SetItemPropValue(GetPropID("m_hWeldLinkObj"),CString("0X0"));
	}
	else
	{
		if(m_bStartEnd)
		{
			m_pCurLineTube->m_tJointStart.hLinkObj=0;
			m_pCurLineTube->m_tJointStart.hViceLinkObj=0;
		}
		else
		{
			m_pCurLineTube->m_tJointEnd.hLinkObj=0;
			m_pCurLineTube->m_tJointEnd.hViceLinkObj=0;
		}
	}
	m_pParamPlate = NULL;
	m_bHasExistEndLj=FALSE;
	m_propList.SetItemReadOnly(GetPropID("m_fOddment"),FALSE);
	m_pInsertPlateParam=NULL;
	m_pFlDParam=m_pFlDParam=NULL;
	m_pFlexibleFlParam=m_pRigidFlParam=NULL;
	GetDlgItem(IDC_BTN_DEL_CUR_ENDLJ)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_PARTS_DB)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CMB_TUBE_ENDLJ_STYLE)->EnableWindow(!m_bSpecEndLjStyle&&!m_bHasExistEndLj);
	//
	UpdateDesignParam();
	UpdateLayoutBoltPara();
	DisplayPropertyList();
	UpdateSketchMap();
}
//����������еĲ��������ѡ�еĲ���������ȡ��Ϣ
void CDesignTubeEndLjDlg::UpdateDesignParamBySrcPlate(CLDSParamPlate* pSrcParamPlate)
{
	if(pSrcParamPlate==NULL)
		return;
	//��ȡ��׼������Ʋ���
	CLDSStdPartModel stdPartModel;
	if(pSrcParamPlate->m_bStdPart&&pSrcParamPlate->GetStdPartModel(&stdPartModel))
	{
		if(pSrcParamPlate->m_iParamType==TYPE_FLG)
			m_pRigidFlParam=FindFlGParam(pSrcParamPlate->GetPartNo());
		else if(pSrcParamPlate->m_iParamType==TYPE_FLR) 
			m_pFlexibleFlParam=FindFlRParam(pSrcParamPlate->GetPartNo());
		else if(pSrcParamPlate->m_iParamType==TYPE_FLD)
			m_pFlDParam=FindFlDParam(pSrcParamPlate->GetPartNo());
		else if(pSrcParamPlate->m_iParamType==TYPE_FLP)
			m_pFlPParam=FindFlPParam(pSrcParamPlate->GetPartNo());
		else
		{	
			m_iStdPartLevel=stdPartModel.param.insert_plate_param.iLevelMark;
			if(pSrcParamPlate->m_iParamType==TYPE_ROLLEND)
				m_pInsertPlateParam=FindRollEndParam(pSrcParamPlate->GetPartNo());
			else if(pSrcParamPlate->m_iParamType==TYPE_UEND)
				m_pInsertPlateParam=FindUEndParam(pSrcParamPlate->GetPartNo());
			else if(pSrcParamPlate->m_iParamType==TYPE_XEND)
				m_pInsertPlateParam=FindXEndParam(pSrcParamPlate->GetPartNo());
		}
	}
	//
	m_paramPlate.CopyProperty(pSrcParamPlate);
	if(m_iTubeEndLjStyle==2&&pSrcParamPlate->m_iParamType==TYPE_FL)	//��ͨ����
	{	//��ȡ��˨��Ϣ
		long J=0;
		pSrcParamPlate->GetDesignItemValue('J',&J);
		LayoutBoltPara.m_iStartEdge=J;
		LayoutBoltPara.m_nBoltNum=pSrcParamPlate->GetLsCount();
		CLsRef *pFirstLsRef=pSrcParamPlate->GetFirstLsRef();
		if(pFirstLsRef)
		{
			double fLayoutR=DistOf3dPtLine(pFirstLsRef->GetLsPtr()->ucs.origin,
				pSrcParamPlate->ucs.origin,pSrcParamPlate->ucs.origin+pSrcParamPlate->ucs.axis_z*100);
			LayoutBoltPara.m_nBoltD=pFirstLsRef->GetLsPtr()->get_d();	//��ȡ��˨ֱ��
			LayoutBoltPara.m_fBoltLayoutD=fLayoutR*2;
			//��ȡ����˨ƫ�ƽǶ�(��ȡ������˨����С��ƫ�ƽǶ�)
			LayoutBoltPara.m_fOffsetAngle=pFirstLsRef->GetLsPtr()->des_base_pos.offset_angle;
			for(CLsRef *pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				if(pLsRef->GetLsPtr()->des_base_pos.R<=0)
					continue;
				if(LayoutBoltPara.m_fOffsetAngle > pLsRef->GetLsPtr()->des_base_pos.offset_angle)
					LayoutBoltPara.m_fOffsetAngle = pLsRef->GetLsPtr()->des_base_pos.offset_angle;
			}
			LayoutBoltPara.m_fOffsetAngle*=DEGTORAD_COEF;	//ת��Ϊ�Ƕ���
		}
		//��ȡ�����߰���Ϣ
		m_bCreateAttachPlate=FALSE;
		CLDSParamPlate *pAttachPlate=NULL;
		for(ATTACHPART_ITEM *pItem=pSrcParamPlate->GetFirstAttachPart();pItem;pItem=pSrcParamPlate->GetNextAttachPart())
		{
			if(pItem->idClsType==CLS_PARAMPLATE)
			{
				pAttachPlate=(CLDSParamPlate*)console.FromPartHandle(pItem->h,CLS_PARAMPLATE);
				break;
			}
		}
		if(pAttachPlate)
		{
			m_bCreateAttachPlate=TRUE;
			strcpy(AttachPlatePara.sPartNo,pAttachPlate->GetPartNo());
			AttachPlatePara.cMaterial=pAttachPlate->cMaterial;
			AttachPlatePara.m_nThick=pAttachPlate->thick;
			AttachPlatePara.m_nPlateNum=LayoutBoltPara.m_nBoltNum;
			CLDSParamPlate::RIBPLATE_ELBOW elbow;
			elbow.LoadParams(pAttachPlate);//��Ҫ������elbow.LoadParams��������������� wjh-2018.3.12
			AttachPlatePara.A=elbow.A.ToDouble();
			AttachPlatePara.B=elbow.B.ToDouble();
			AttachPlatePara.C=elbow.C.ToDouble();
			AttachPlatePara.D=elbow.D.ToDouble();
			AttachPlatePara.E=elbow.E.ToDouble();
			AttachPlatePara.F=elbow.F.ToDouble();
			AttachPlatePara.G=elbow.G.ToDouble();
			AttachPlatePara.H=elbow.H.ToDouble();
			//pAttachPlate->GetDesignItemValue('A', &AttachPlatePara.A);
			//pAttachPlate->GetDesignItemValue('B', &AttachPlatePara.B);
			//pAttachPlate->GetDesignItemValue('C', &AttachPlatePara.C);
			//pAttachPlate->GetDesignItemValue('D', &AttachPlatePara.D);
			//pAttachPlate->GetDesignItemValue('E', &AttachPlatePara.E);
			//pAttachPlate->GetDesignItemValue('F', &AttachPlatePara.F);
			//pAttachPlate->GetDesignItemValue('G', &AttachPlatePara.G);
			//pAttachPlate->GetDesignItemValue('H', &AttachPlatePara.H);
			//������˨ƫ�ƽǶȼ���,���߰�ƫ�ƽǶ�
			AttachPlatePara.m_fAngle=LayoutBoltPara.m_fOffsetAngle+180.0/AttachPlatePara.m_nPlateNum;
		}
	}
	else if(m_iTubeEndLjStyle==5)
	{	//���塢������ wht 11-01-23
		ATTACHPART_ITEM *pItem=NULL;
		strcpy(sWeldPlatePartNo,"");
		strcpy(sWrapPlatePartNo,"");
		strcpy(sCodeName,"");
		double M=0,A=0;
		if(m_pInsertPlateParam)
		{
			M=m_pInsertPlateParam->M;
			A=m_pInsertPlateParam->A;
			strcpy(sWeldPlatePartNo,m_pInsertPlateParam->weldPlatePartNo);
			strcpy(sWrapPlatePartNo,m_pInsertPlateParam->wrapPlatePartNo);
			strcpy(sCodeName,m_pInsertPlateParam->codeName);
		}
		else
		{
			pSrcParamPlate->GetDesignItemValue('M',&M);
			pSrcParamPlate->GetDesignItemValue('A',&A);
			for(pItem=pSrcParamPlate->GetFirstAttachPart();pItem;pItem=pSrcParamPlate->GetNextAttachPart())
			{
				CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
				if(pPlate==NULL)
					continue;
				f3dPoint plate_pos=pPlate->ucs.origin;
				//ת����ʮ�ֲ������ϵ��
				coord_trans(plate_pos,pSrcParamPlate->ucs,FALSE);
				if(plate_pos.x<EPS2&&fabs(plate_pos.z)>0&&strlen(sWeldPlatePartNo)==0)
					strcpy(sWeldPlatePartNo,pPlate->GetPartNo());	//������
				else if(fabs(plate_pos.x-M-A)<EPS2&&strlen(sWrapPlatePartNo)==0)
					strcpy(sWrapPlatePartNo,pPlate->GetPartNo());
				if(strlen(sWeldPlatePartNo)>0&&strlen(sWrapPlatePartNo)>0)
					break;
			}
		}
	}
	//��ͨ��壬��ȡ����ϵ���˨���ò���
	if(m_iTubeEndLjStyle>2 && !pSrcParamPlate->m_bStdPart)
	{
		if(m_iTubeEndLjStyle<5)
		{	//U�Ͳ��|���Ͳ��
			LayoutBoltParaList.Empty();	//���ԭ����˨���ò���
			for(CLsRef* pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				CLDSBolt* pBolt=pLsRef->GetLsPtr();
				//
				LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
				pBoltPara->m_nBoltD=pBolt->get_d();
				pBoltPara->m_nBoltNum=1;
				pBoltPara->m_nColNum=1;
				pBoltPara->m_nRowNum=1;
				pBoltPara->m_fColSpace=0;
				pBoltPara->m_fRowSpace=0;
				pBoltPara->m_posFirstBolt.x=pBolt->des_base_pos.len_offset_dist;
				pBoltPara->m_posFirstBolt.y=pBolt->des_base_pos.wing_offset_dist;
				pBoltPara->m_iDirection=pBolt->des_work_norm.direction;	
				pBoltPara->m_fSpecialSpace=pBoltPara->m_fColSpace;	//�������ʼֵӦ�����о� 
				pBoltPara->m_bStaggerLayout=FALSE;
				pBoltPara->m_bMirLayout=FALSE;
			}
		}
		else if(m_iTubeEndLjStyle==5)
		{	//ʮ���Ͳ��
			BOOL bStaggerLayout=FALSE;
			CLDSBolt *pBolt=NULL,*pXYBolt=NULL,*pXZBolt=NULL;
			for(CLsRef* pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				pBolt=pLsRef->GetLsPtr();
				if(pBolt->feature==11)
					pXYBolt=pBolt;
				if(pBolt->feature==31)
					pXZBolt=pBolt;
				if(pXYBolt && pXZBolt)
				{
					double fY1=pXYBolt->des_base_pos.wing_offset_dist;
					double fY2=pXZBolt->des_base_pos.wing_offset_dist;
					if(fabs(fY1-fY2)>1)
						bStaggerLayout=TRUE;
					break;
				}
			}
			//��ȡʮ�ֲ����˨���ò���
			LayoutBoltParaList.Empty();	//���ԭ����˨���ò���
			double H=0,N=0,A=0,M=0,R=0;
			pSrcParamPlate->GetDesignItemValue('A',&A);
			pSrcParamPlate->GetDesignItemValue('M',&M);
			pSrcParamPlate->GetDesignItemValue('R',&R);
			for(CLsRef* pLsRef=pSrcParamPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcParamPlate->GetNextLsRef())
			{
				pBolt=pLsRef->GetLsPtr();
				if(pBolt->feature!=11&&pBolt->feature!=12)
					continue;
				double fBoltPosX=pBolt->des_base_pos.len_offset_dist-A-M;
				double fBoltPosY=pBolt->des_base_pos.wing_offset_dist-R;
				//
				LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
				pBoltPara->m_nBoltD=pBolt->get_d();
				pBoltPara->m_nBoltNum=1;
				pBoltPara->m_nColNum=1;
				pBoltPara->m_nRowNum=1;
				pBoltPara->m_fColSpace=0;
				pBoltPara->m_fRowSpace=0;
				pBoltPara->m_posFirstBolt.x=fBoltPosX;
				pBoltPara->m_posFirstBolt.y=fBoltPosY;
				pBoltPara->m_bStaggerLayout=bStaggerLayout;
				pBoltPara->m_bMirLayout=FALSE;
			}
		}
	}
}
void CDesignTubeEndLjDlg::OnPartsDb() 
{
	CPartsDbDlg partsDlg;
	partsDlg.m_bRefreshDlgCtrl = TRUE;
	if(m_iTubeEndLjStyle==2)
		partsDlg.m_bIncParamFl = TRUE;
	else if(m_iTubeEndLjStyle==3)
		partsDlg.m_bIncParamUend = TRUE;
	else if(m_iTubeEndLjStyle==4)
		partsDlg.m_bIncParamRollEnd = TRUE;
	else if(m_iTubeEndLjStyle==5)
		partsDlg.m_bIncParamXEnd = TRUE;
	if(partsDlg.DoModal()==IDOK)
	{
		m_pSrcParamPlate=(CLDSParamPlate*)partsDlg.m_pSelectPart;
		if(!m_pSrcParamPlate)
			return;
		if(m_pSrcParamPlate->m_bStdPart&&(m_pSrcParamPlate->IsFL()))
			return;
		CLDSStdPartModel stdPartModel;
		if(m_pSrcParamPlate->m_bStdPart&&!m_pSrcParamPlate->GetStdPartModel(&stdPartModel))
			return;
		m_iPartType=0;	//��ͨ����
		UpdateDesignParamBySrcPlate(m_pSrcParamPlate);
		setupInfo.m_fNormOffset=m_pSrcParamPlate->m_fNormOffset;//����ƫ��
		if(m_pCurLineTube)
			m_paramPlate.iSeg=m_pCurLineTube->iSeg;	//���κ�Ӧ�����׼�ֹܶκű���һ��
		UpdateTubeOddment();	//���¸ֹ�����ͷ
		UpdateLayoutBoltPara();
		DisplayPropertyList(m_ctrlPropGroup.GetCurSel());	//����������
		UpdateSketchMap(m_ctrlPropGroup.GetCurSel());		//����ͼƬ
		Invalidate(FALSE);
	}
}

void CDesignTubeEndLjDlg::OnKeydownBoltList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_listBolt.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_listBolt.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_UP)
		{	//��һ��UP��ѡ���������һλ����������������ڰ���UP��ʱ������
			//��ʱ����δ�ƶ��������ֶ�����ѡ�����λ��
			if(iCurSel>=1)
				iCurSel--;
			if(iCurSel>=0)
				m_listBolt.SetItemState(iCurSel,LVIS_SELECTED,LVIS_SELECTED);
		}
		else if(pLVKeyDow->wVKey==VK_UP||pLVKeyDow->wVKey==VK_DOWN)
		{	//��һ��UP��ѡ���������һλ����������������ڰ���DOWN��ʱ������
			//��ʱ����δ�ƶ��������ֶ�����ѡ�����λ��
			if(iCurSel<m_listBolt.GetItemCount())
				iCurSel++;
			if(iCurSel>=0)
				m_listBolt.SetItemState(iCurSel,LVIS_SELECTED,LVIS_SELECTED);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			if(m_iPartType>0||m_iTubeEndLjStyle==2)
				return;	//�������׼������˨�������б���ɾ��
			m_listBolt.DeleteItem(iCurSel);
			pos = m_listBolt.GetFirstSelectedItemPosition();
		}
		else
			break;
	}
	Invalidate(FALSE);
	*pResult = 0;
}

//������˨���ò���
void CDesignTubeEndLjDlg::UpdateLayoutBoltPara()
{
	if(m_iTubeEndLjStyle==2)
	{	//�����˨���ò���
		if((m_iPartType==1)&&m_pFlPParam)//||m_iPartType==0
		{	//ƽ������
			LayoutBoltPara.m_nBoltD=m_pFlPParam->nBoltD;
			LayoutBoltPara.m_nBoltNum=m_pFlPParam->nBoltNum;
			LayoutBoltPara.m_fBoltLayoutD=m_pFlPParam->K;
		}
		else if((m_iPartType==2)&&m_pFlDParam)//||m_iPartType==0
		{	//�Ժ�����
			LayoutBoltPara.m_nBoltD=m_pFlDParam->nBoltD;
			LayoutBoltPara.m_nBoltNum=m_pFlDParam->nBoltNum;
			LayoutBoltPara.m_fBoltLayoutD=m_pFlDParam->K;
		}
		else if(m_iPartType==3&&m_pRigidFlParam)
		{	//���Է���
			LayoutBoltPara.m_nBoltD=m_pRigidFlParam->nBoltD;
			LayoutBoltPara.m_nBoltNum=m_pRigidFlParam->nBoltNum;
			LayoutBoltPara.m_fBoltLayoutD=m_pRigidFlParam->fBoltCirD;
		}
		else if(m_iPartType==4&&m_pFlexibleFlParam)
		{	//���Է���
			LayoutBoltPara.m_nBoltD=m_pFlexibleFlParam->nBoltD;
			LayoutBoltPara.m_nBoltNum=m_pFlexibleFlParam->nBoltNum;
			LayoutBoltPara.m_fBoltLayoutD=m_pFlexibleFlParam->fBoltCirD;
		}
	}
	else
	{	//���
		if(m_iPartType>0&&m_pInsertPlateParam)
		{	//��׼���
			LayoutBoltParaList.Empty();	//���ԭ����˨���ò���
			LayoutBoltPara.m_nBoltD=m_pInsertPlateParam->ls_param[0].iBoltD;
			for(int i=0;i<m_pInsertPlateParam->iBoltRect;i++)
			{
				CDesignTubeEndLjDlg::LAYOUT_BOLT_PARA *pBoltPara=LayoutBoltParaList.append();
				pBoltPara->m_nBoltD=m_pInsertPlateParam->ls_param[i].iBoltD;
				pBoltPara->m_nBoltNum=m_pInsertPlateParam->ls_param[i].iBoltNum;
				pBoltPara->m_nColNum=m_pInsertPlateParam->ls_param[i].iColumn;
				pBoltPara->m_nRowNum=m_pInsertPlateParam->ls_param[i].iRow;
				pBoltPara->m_fColSpace=m_pInsertPlateParam->ls_param[i].fHorzSpace;
				pBoltPara->m_fRowSpace=m_pInsertPlateParam->ls_param[i].fVertSpace;
				pBoltPara->m_posFirstBolt.x=m_pInsertPlateParam->ls_param[i].fBoltPosX;
				pBoltPara->m_posFirstBolt.y=m_pInsertPlateParam->ls_param[i].fBoltPosY;
				pBoltPara->m_fSpecialSpace=pBoltPara->m_fColSpace;	//�������ʼֵӦ�����о� 
				if(m_pInsertPlateParam->ls_param[i].iColumn==-1
					||m_pInsertPlateParam->ls_param[i].iRow==-1)
					pBoltPara->m_bStaggerLayout=TRUE;
				else
					pBoltPara->m_bStaggerLayout=FALSE;
				pBoltPara->m_bMirLayout=FALSE;
			}
		}
	}
	//ˢ����˨�����б�
	RefreshBoltParamList();
	RefreshBoltList();	
	Invalidate(FALSE);
}

//�ж��Ƿ��Ѿ����ڶ�����
BOOL CDesignTubeEndLjDlg::IsHasExistEndLj()
{
	return (m_pParamPlate&&m_bHasExistEndLj);
}

//�õ���������
int CDesignTubeEndLjDlg::GetPartType() const
{
	if(m_iTubeEndLjStyle==2)
	{	//��ͷ��������
		if(m_iPartType==1&&m_pFlPParam)
			return 1;
		else if(m_iPartType==2&&m_pFlDParam)
			return 2;
		else if(m_iPartType==3&&m_pRigidFlParam)
			return 3;
		else if(m_iPartType==4&&m_pFlexibleFlParam)
			return 4;
	}
	else if(m_iTubeEndLjStyle>2)
	{	//��ͷ�������
		if(m_iPartType==1&&m_pInsertPlateParam)
			return 1;
	}
	return 0;	//��ͨ������������
}
//���ù�������
void CDesignTubeEndLjDlg::SetPartType(int iPartType)
{
	if(iPartType!=0&&(iPartType>4&&iPartType<1))
		iPartType=0;
	m_iPartType=iPartType;
}

void CDesignTubeEndLjDlg::OnClickBoltList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Invalidate(FALSE);
	*pResult = 0;
}

void CDesignTubeEndLjDlg::DesignLineAngle(CLDSLineAngle* pRayJg,CLDSPlate* pNodePlate)
{
	CDesignLjPartPara* pLinePartPara=pNodePlate->designInfo.partList.Add(pRayJg->handle);
	pLinePartPara->hPart=pRayJg->handle;
	pLinePartPara->m_nClassTypeId=CLS_LINEANGLE;
	pLinePartPara->iFaceNo=1;
	pLinePartPara->angle.bEndLjJg=0;
	//����Ǹ�����ͷ����
	double fNormOffset=-0.5*pNodePlate->GetThick();
	int x_wing0_y_wing1=0;
	BOOL bInsideJg=IsInsideJg(pRayJg,pNodePlate->ucs.axis_z,&x_wing0_y_wing1);
	int iLinkToStart0End1=1;
	if(pRayJg->pStart==m_pBaseNode)
	{
		iLinkToStart0End1=0;
		pRayJg->desStartOdd.m_hRefPart1=pNodePlate->designInfo.m_hBasePart;
		pRayJg->desStartOdd.m_fCollideDist=g_sysPara.VertexDist;
		pRayJg->CalStartOddment();
		if(x_wing0_y_wing1==0)
		{
			pRayJg->desStartPos.wing_x_offset.gStyle=4;
			pRayJg->desStartPos.wing_x_offset.offsetDist=fNormOffset;
		}
		else
		{
			pRayJg->desStartPos.wing_y_offset.gStyle=4;
			pRayJg->desStartPos.wing_y_offset.offsetDist=fNormOffset;
		}
		pRayJg->CalPosition();
	}
	else if(pRayJg->pEnd==m_pBaseNode)
	{
		iLinkToStart0End1=1;
		pRayJg->desEndOdd.m_hRefPart1=pNodePlate->designInfo.m_hBasePart;
		pRayJg->desEndOdd.m_fCollideDist=g_sysPara.VertexDist;
		pRayJg->CalEndOddment();
		if(x_wing0_y_wing1==0)
		{
			pRayJg->desEndPos.wing_x_offset.gStyle=4;
			pRayJg->desEndPos.wing_x_offset.offsetDist=fNormOffset;
		}
		else
		{
			pRayJg->desEndPos.wing_y_offset.gStyle=4;
			pRayJg->desEndPos.wing_y_offset.offsetDist=fNormOffset;
		}
		pRayJg->CalPosition();
	}
	pLinePartPara->start0_end1=iLinkToStart0End1;
	//���ýǸ���˨
	CLayAngleBoltDlg dlg;
	dlg.m_pNode=m_pBaseNode;
	dlg.m_hPlate=pNodePlate->handle;
	dlg.m_pLinePart=pRayJg;
	dlg.m_xiPreferSegI=pNodePlate->iSeg;//pNodePlate->iSeg.ToString();
	dlg.viewNorm=pNodePlate->ucs.axis_z;
	dlg.m_bMirLayLs = FALSE;
	dlg.m_bRayEndFaceOffset=FALSE;
	dlg.m_bTwoEdgeProfile=FALSE;
	if(bInsideJg)	//����
		dlg.m_tBoltNormOffset.AddThick(-pRayJg->handle,TRUE);
	else
		dlg.m_tBoltNormOffset.AddThick(-pNodePlate->handle,TRUE);
	dlg.m_iBoltNorm=x_wing0_y_wing1;
	pLinePartPara->angle.cur_wing_x0_y1=(BYTE)x_wing0_y_wing1;
	if(iLinkToStart0End1==0)
	{
		dlg.m_iOddCalStyle=pRayJg->desStartOdd.m_iOddCalStyle;
		dlg.m_fOddment=pRayJg->startOdd();
		dlg.m_sExtraOdd.Format("%.0f",pRayJg->GetStartExtraOdd(dlg.m_iBoltNorm));
		dlg.m_nLsNum=pRayJg->BoltsNumOn1Wing(TRUE);
		dlg.m_iBoltRows=pRayJg->connectStart.rows-1;
		dlg.m_sBoltSize.Format("%d",pRayJg->connectStart.d);
	}
	else if(iLinkToStart0End1==1)
	{
		dlg.m_iOddCalStyle=pRayJg->desEndOdd.m_iOddCalStyle;
		dlg.m_fOddment=pRayJg->endOdd();
		dlg.m_sExtraOdd.Format("%.0f",pRayJg->GetEndExtraOdd(dlg.m_iBoltNorm));
		dlg.m_nLsNum=pRayJg->BoltsNumOn1Wing(FALSE);
		dlg.m_iBoltRows=pRayJg->connectEnd.rows-1;
		dlg.m_sBoltSize.Format("%d",pRayJg->connectEnd.d);
	}
	ATOM_LIST<CDesignLsPara>boltList;
	dlg.m_pLsList=&boltList;
	dlg.m_bIncPlateProfilePara=TRUE;
	if(dlg.DoModal()==IDOK)
	{
		if(iLinkToStart0End1==0)	//ʼ������
		{
			if(x_wing0_y_wing1==0)
			{
				pRayJg->desStartPos.wing_x_offset.gStyle=4;
				pRayJg->desStartPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
			}
			else
			{
				pRayJg->desStartPos.wing_y_offset.gStyle=4;
				pRayJg->desStartPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
			}
		}
		else	//�ն�����
		{
			if(x_wing0_y_wing1==0)
			{
				pRayJg->desEndPos.wing_x_offset.gStyle=4;
				pRayJg->desEndPos.wing_x_offset.offsetDist=dlg.m_fAngleEndNormOffset;
			}
			else
			{
				pRayJg->desEndPos.wing_y_offset.gStyle=4;
				pRayJg->desEndPos.wing_y_offset.offsetDist=dlg.m_fAngleEndNormOffset;
			}
		}
		pRayJg->CalPosition();
		//
		pLinePartPara->angle.cbSpaceFlag=dlg.cbSpaceFlag;
		pLinePartPara->ber_space=dlg.m_fBerSpace;
		pLinePartPara->wing_space=dlg.m_fWingSpace;
		pLinePartPara->end_space=dlg.m_fPlateEdgeSpace;
		pLinePartPara->angle.bTwoEdge=dlg.m_bTwoEdgeProfile;
		if(pLinePartPara->start0_end1==0)
		{
			pRayJg->desStartOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
			pRayJg->SetStartOdd(dlg.m_fOddment);
		}
		else if(pLinePartPara->start0_end1==1)
		{
			pRayJg->desEndOdd.m_iOddCalStyle=dlg.m_iOddCalStyle;
			pRayJg->SetEndOdd(dlg.m_fOddment);
		}
		//int iInitRayNo=1;
		ATOM_LIST<RAYNO_RECORD>rayNoList;
		CObjNoGroup NoGroup(32);
		BOLTSET boltSet;
		for(CDesignLsPara *pBoltPara=boltList.GetFirst();pBoltPara;pBoltPara=boltList.GetNext())
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->iSeg = pBoltPara->iSeg;
			pBoltPara->DesignLs(pBolt);
			pBolt->CopyModuleInstanceInfo(pRayJg);
			pBolt->AddL0Thick(pNodePlate->handle,TRUE);
			pBolt->AddL0Thick(pRayJg->handle,TRUE);
			if(pLinePartPara->start0_end1==0)
			{
				pRayJg->connectStart.d=atoi(dlg.m_sBoltSize);
				pRayJg->SetBoltsNumOn1Wing(dlg.m_nLsNum,TRUE);
				pRayJg->connectStart.rows=dlg.m_iBoltRows+1;
				pRayJg->AppendStartLsRef(pBolt->GetLsRef());
			}
			else if(pLinePartPara->start0_end1==1)
			{
				pRayJg->connectEnd.d=atoi(dlg.m_sBoltSize);
				pRayJg->SetBoltsNumOn1Wing(dlg.m_nLsNum,FALSE);
				pRayJg->connectEnd.rows=dlg.m_iBoltRows+1;
				pRayJg->AppendEndLsRef(pBolt->GetLsRef());
			}
			
			pNodePlate->AppendLsRef(pBolt->GetLsRef());
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
		if(pLinePartPara->start0_end1==0&&pRayJg->desStartOdd.m_iOddCalStyle==1)
		{
			pRayJg->CalStartOddment();
			pRayJg->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
		}
		else if(pLinePartPara->start0_end1==1&&pRayJg->desEndOdd.m_iOddCalStyle==1)
		{
			pRayJg->CalEndOddment();
			pRayJg->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
		}
	}
}
void CDesignTubeEndLjDlg::InitPlateBasePara(CLDSPlate* pNodePlate,CLDSLineTube *pDatumLineTube)
{
	static CPlateBasicParaDlg basicParaDlg;
	pNodePlate->Thick=basicParaDlg.m_nPlankThick=8;
	basicParaDlg.m_iMaterial=QuerySteelMatIndex(pDatumLineTube->cMaterial);
	basicParaDlg.m_sSegI=pDatumLineTube->iSeg.ToString();
	basicParaDlg.m_bAdjustWeldLen=FALSE;
	if(basicParaDlg.DoModal()==IDOK)
	{
		pNodePlate->cMaterial=CSteelMatLibrary::RecordAt(basicParaDlg.m_iMaterial).cBriefMark;
		pNodePlate->SetPartNo(basicParaDlg.m_sPartNo);
		pNodePlate->iSeg=SEGI(basicParaDlg.m_sSegI.GetBuffer());
		pNodePlate->Thick=basicParaDlg.m_nPlankThick;
		pNodePlate->m_fNormOffset=-0.5*basicParaDlg.m_nPlankThick;
		pNodePlate->designInfo.weld_width=basicParaDlg.m_nWeldWidth;
		strcpy(pNodePlate->layer(),pDatumLineTube->layer());
		pNodePlate->layer(1)='P';
		pNodePlate->cfgword=pDatumLineTube->cfgword;
		pNodePlate->m_hPartWeldParent=pDatumLineTube->handle;
	}
}
CLDSPlate *CDesignTubeEndLjDlg::CreateNodePlate(CLDSLineTube *pDatumLineTube,CLDSLineAngle* pRayLineAngle)
{
	if(pDatumLineTube==NULL || pRayLineAngle==NULL)
		return NULL;
	CLDSPlate *pNodePlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	//
	InitPlateBasePara(pNodePlate,pDatumLineTube);
	//�����Ϣ
	pNodePlate->designInfo.iProfileStyle0123=1;
	pNodePlate->designInfo.m_bEnableFlexibleDesign=TRUE;
	pNodePlate->designInfo.m_hBaseNode=m_pBaseNode->handle;
	pNodePlate->designInfo.m_hBasePart=pDatumLineTube->handle;
	//��׼��
	pNodePlate->designInfo.origin.datum_pos_style = 2;
	pNodePlate->designInfo.origin.des_para.RODNODE.hNode =m_pBaseNode->handle;
	pNodePlate->designInfo.origin.des_para.RODNODE.hRod  =pDatumLineTube->handle;
	//����
	if(pDatumLineTube->pEnd==m_pBaseNode)
		pNodePlate->ucs.axis_y = pDatumLineTube->Start()-pDatumLineTube->End();
	else
		pNodePlate->ucs.axis_y = pDatumLineTube->End()-pDatumLineTube->Start();
	pNodePlate->ucs.axis_x = pRayLineAngle->End()-pRayLineAngle->Start();
	pNodePlate->ucs.axis_z = pNodePlate->ucs.axis_x^pNodePlate->ucs.axis_y;
	normalize(pNodePlate->ucs.axis_z);
	normalize(pNodePlate->ucs.axis_y);
	if(pNodePlate->ucs.axis_z*console.GetActiveView()->ucs.axis_z<0)
		pNodePlate->ucs.axis_z*=-1.0;
	pNodePlate->designInfo.norm.norm_style=2;	//�˼������淨��
	pNodePlate->designInfo.norm.hVicePart=pDatumLineTube->handle;
	pNodePlate->designInfo.norm.hCrossPart=pRayLineAngle->handle;
	pNodePlate->designInfo.norm.vector=pNodePlate->ucs.axis_z;
	//���Ӹ˼�
	CDesignLjPartPara *pLjPart=NULL;
	pLjPart=pNodePlate->designInfo.partList.Add(pDatumLineTube->handle);
	pLjPart->m_nClassTypeId=CLS_LINETUBE;
	pLjPart->hPart=pDatumLineTube->handle;
	pLjPart->iFaceNo=1;
	pLjPart->start0_end1=2;
	//
	DesignLineAngle(pRayLineAngle,pNodePlate);
	//
	pNodePlate->DesignPlate();
	pNodePlate->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pNodePlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pNodePlate;
}
//��ƽڵ��
CLDSPlate *CDesignTubeEndLjDlg::CreateNodePlate(CLDSLineTube *pDatumLineTube,
												CLDSLineTube *pRayLineTube/*=NULL*/,
												CLDSParamPlate *pParamPlate/*=NULL*/,
												CLDSLineTube *pHorizontalTube/*=NULL*/)	//���
{
	if(pParamPlate==NULL)
		pParamPlate=m_pParamPlate;
	if(pRayLineTube==NULL)
		pRayLineTube=m_pCurLineTube;
	if(pDatumLineTube==NULL||pRayLineTube==NULL||pParamPlate==NULL)
		return NULL;
	int start0_end1=(pRayLineTube->pStart==m_pBaseNode)?0:1;
	f3dPoint  ray_line_vec=(pRayLineTube->Start()-pRayLineTube->End()).normalized();
	if(start0_end1==0)
		ray_line_vec*=-1;
	f3dPoint base_line_vec;
	if(pDatumLineTube->pEnd==m_pBaseNode)
		base_line_vec=(pDatumLineTube->Start()-pDatumLineTube->End()).normalized();
	else
		base_line_vec=(pDatumLineTube->End()-pDatumLineTube->Start()).normalized();
	//
	CLDSPlate *pNodePlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pNodePlate->iSeg=pParamPlate->iSeg;
	pNodePlate->cMaterial=NodePlatePara.cMaterial;
	pNodePlate->SetPartNo(NodePlatePara.sPartNo);
	pNodePlate->Thick=NodePlatePara.m_nThick;
	strcpy(pNodePlate->layer(),pParamPlate->layer());
	pNodePlate->layer(1)='P';
	pNodePlate->cfgword=pParamPlate->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	pNodePlate->m_hPartWeldParent=pDatumLineTube->handle;
	pNodePlate->m_fNormOffset=-0.5*NodePlatePara.m_nThick;	//�ְ巨��ƫ����
	//pNodePlate->cfgword=m_pBaseNode->cfgword;	//�����ְ���ĺ����׼�������׼�ڵ���ĺ�һ��
	//
	pNodePlate->designInfo.iProfileStyle0123=1;	//��������
	pNodePlate->designInfo.m_bEnableFlexibleDesign=TRUE;//�����������������
	pNodePlate->designInfo.m_hBaseNode = m_pBaseNode->handle;
	pNodePlate->designInfo.m_hBasePart = pDatumLineTube->handle;
	//��׼��
	pNodePlate->designInfo.origin.datum_pos_style = 2;
	pNodePlate->designInfo.origin.des_para.RODNODE.hNode =m_pBaseNode->handle;
	pNodePlate->designInfo.origin.des_para.RODNODE.hRod  =pDatumLineTube->handle;
	//����
	pNodePlate->ucs.axis_y = base_line_vec;
	pNodePlate->ucs.axis_x = ray_line_vec;
	pNodePlate->ucs.axis_z = pNodePlate->ucs.axis_x^pNodePlate->ucs.axis_y;
	normalize(pNodePlate->ucs.axis_z);
	normalize(pNodePlate->ucs.axis_y);
	if(pNodePlate->ucs.axis_z*console.GetActiveView()->ucs.axis_z<0)
		pNodePlate->ucs.axis_z*=-1.0;
	pNodePlate->designInfo.norm.norm_style=2;	//�˼������淨��
	pNodePlate->designInfo.norm.hVicePart=pDatumLineTube->handle;
	pNodePlate->designInfo.norm.hCrossPart=pRayLineTube->handle;
	pNodePlate->designInfo.norm.nearVector = pNodePlate->ucs.axis_z;
	//���Ӹ˼��б�
	CDesignLjPartPara *pLjPart=NULL;
	pLjPart=pNodePlate->designInfo.partList.Add(pDatumLineTube->handle);
	pLjPart->iFaceNo=1;
	pLjPart->start0_end1=2;
	pLjPart->ber_space=0;
	pLjPart=pNodePlate->designInfo.partList.Add(pRayLineTube->handle);
	pLjPart->iFaceNo=1;
	pLjPart->start0_end1=start0_end1;
	pLjPart->ber_space=0;
	//��Ӻ�ļ���Ĺ�������ϵĹ�����˨ wht 11-06-07
	CLsRef *pLsRef=NULL;
	if(pHorizontalTube)
	{
		pLjPart=pNodePlate->designInfo.partList.Add(pHorizontalTube->handle);
		pLjPart->iFaceNo=1;
		if(pHorizontalTube->pStart==m_pBaseNode)
			start0_end1=0;
		else if(pHorizontalTube->pEnd==m_pBaseNode)
			start0_end1=1;
		pLjPart->start0_end1=start0_end1;
		pLjPart->ber_space=0;
		CLDSParamPlate *pPlate=pHorizontalTube->GetParamPlate(start0_end1);
		if(pPlate&&pPlate->m_iParamType==TYPE_XEND)
		{
			f3dPoint work_norm=pNodePlate->ucs.axis_z;
			CLDSPlate *pUpPlate=NULL,*pDownPlate=NULL,*pCurVertPlate=NULL;
			pPlate->GetVertPlateInXEndPlate(&pUpPlate,&pDownPlate);
			if(fabs(work_norm*pPlate->ucs.axis_z)<fabs(work_norm*pUpPlate->ucs.axis_z))
			{	//������ڵ�干��ʱ�����������ķ��߲���	 wxc-2018.3.17
				if(pUpPlate->ucs.axis_y*base_line_vec>pDownPlate->ucs.axis_y*base_line_vec)
					pCurVertPlate=pUpPlate;
				else
					pCurVertPlate=pDownPlate;
			}
			//
			for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{	//����ʮ�ֲ���ϲ�������˨
				double L=0;
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				f3dPoint ls_pos=pBolt->ucs.origin;
				coord_trans(ls_pos,pPlate->ucs,FALSE);
				pPlate->GetDesignItemValue('L',&L);
				if(fabs(pBolt->get_norm()*pNodePlate->ucs.axis_z)<EPS_COS2)
					continue;
				if(ls_pos.x<L)
					continue;
				if(pCurVertPlate&&((pCurVertPlate==pUpPlate&&ls_pos.z<0)
					||(pCurVertPlate==pDownPlate&&ls_pos.z>0)))
					continue;
				pBolt->AddL0Thick(pNodePlate->handle,TRUE,TRUE);
				pBolt->SetModified();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				pNodePlate->AppendLsRef(*pLsRef);		
			}
			if(pCurVertPlate)
			{
				//�������巨�����������淨�߷���һ��
				pCurVertPlate->designInfo.norm=pNodePlate->designInfo.norm;
				pCurVertPlate->designInfo.norm.nearVector=pCurVertPlate->ucs.axis_z;
				//������������嶨λ�İ���
				ATTACHPART_ITEM *pItem=NULL;
				for(pItem=pPlate->GetFirstAttachPart();pItem;pItem=pPlate->GetNextAttachPart())
				{
					CLDSPlate *pSonPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
					if(pPlate==NULL)
						continue;
					if((pSonPlate->designInfo.norm.hVicePart==pCurVertPlate->handle
						&&pSonPlate->designInfo.norm.norm_style>0)||pSonPlate==pCurVertPlate)
					{	
						pSonPlate->DesignSetupUcs();
						pSonPlate->DesignPlate();
						pSonPlate->SetModified();
						pSonPlate->Create3dSolidModel();
						g_pSolidDraw->NewSolidPart(pSonPlate->GetSolidPartObject());
					}
				}
				//������Ʋ�����ʱ���丽������˨�������
				for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					CLDSBolt *pBolt=pLsRef->GetLsPtr();
					pBolt->correct_worknorm();
					pBolt->correct_pos();
					pBolt->CalGuigeAuto();
					pBolt->SetModified();
					pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				}
			}
		}
		else if(pPlate)
		{
			for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(fabs(pBolt->get_norm()*pNodePlate->ucs.axis_z)<EPS_COS2)
					continue;
				pBolt->AddL0Thick(pNodePlate->handle,TRUE,TRUE);
				if(pBolt->des_base_pos.direction==0)	//����ƫ����
				{
					pBolt->des_base_pos.norm_offset.EmptyThick();
					pBolt->des_base_pos.norm_offset.AddThick(-pNodePlate->handle,TRUE);
				}
				pBolt->SetModified();
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->CalGuigeAuto();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
				pNodePlate->AppendLsRef(*pLsRef);
			}
		}
	}
	//�����˨����
	if(pParamPlate->m_iParamType==TYPE_XEND)
	{
		for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{	//����ʮ�ֲ���ϲ�������˨
			double L=0;
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			f3dPoint ls_pos=pBolt->ucs.origin;
			coord_trans(ls_pos,pParamPlate->ucs,FALSE);
			pParamPlate->GetDesignItemValue('L',&L);
			if(fabs(pBolt->get_norm()*pNodePlate->ucs.axis_z)<EPS_COS2)
				continue;
			if(ls_pos.x<L)
				continue;
			pBolt->AddL0Thick(pNodePlate->handle,TRUE,TRUE);
			pBolt->SetModified();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			pNodePlate->AppendLsRef(*pLsRef);		
		}
	}
	else
	{
		for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(fabs(pBolt->get_norm()*pNodePlate->ucs.axis_z)<EPS_COS2)
				continue;
			pBolt->AddL0Thick(pNodePlate->handle,TRUE,TRUE);
			if(pBolt->des_base_pos.direction==0)	//����ƫ����
			{
				pBolt->des_base_pos.norm_offset.EmptyThick();
				pBolt->des_base_pos.norm_offset.AddThick(-pNodePlate->handle,TRUE);
			}
			pBolt->SetModified();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->CalGuigeAuto();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			pNodePlate->AppendLsRef(*pLsRef);
		}
	}
	//��Ƹְ�
	pNodePlate->DesignPlate();
	pNodePlate->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pNodePlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pNodePlate;
}

//���ѡ�����ĺ�������
void CDesignTubeEndLjDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	char sValueText[100];
	if(GetPropID("m_hDatumTube1")==GetEventPropId())
	{
		if(console.FromPartHandle(m_hPickObj,CLS_LINETUBE))
			m_hDatumTube1=m_hPickObj;
		UpdateTubeOddment(); //���¸ֹ�����ͷ
		if(GetPropValueStr(GetPropID("m_hDatumTube1"),sValueText))
			m_propList.SetItemPropValue(GetPropID("m_hDatumTube1"),sValueText);
	}
	else if(GetPropID("m_hDatumTube2")==GetEventPropId())
	{
		if(console.FromPartHandle(m_hPickObj,CLS_LINETUBE))
			m_hDatumTube2=m_hPickObj;	
		UpdateTubeOddment(); //���¸ֹ�����ͷ
		if(GetPropValueStr(GetPropID("m_hDatumTube2"),sValueText))
			m_propList.SetItemPropValue(GetPropID("m_hDatumTube2"),sValueText);
	}
	else if(GetPropID("m_hWeldLinkObj")==GetEventPropId())
	{	//���ݺ������Ӱ壬�Զ�����ƽ���и��ͷ���
		if(console.FromPartHandle(m_hPickObj))
			m_hWeldLinkObj=m_hPickObj;
		f3dPoint tube_vec=(m_pCurLineTube->End()-m_pCurLineTube->Start()).normalized();
		tube_vec*=m_bStartEnd?1:-1;
		CLDSGeneralPlate* pWeldPlate=(CLDSGeneralPlate*)Ta.FromPartHandle(m_hWeldLinkObj);
		double fCosa=pWeldPlate?pWeldPlate->ucs.axis_z*tube_vec:0;
		if(fabs(fCosa)>EPS_COS2)
		{
			cutPlaneNorm=pWeldPlate->ucs.axis_z;
			if(fCosa>0)
				cutPlaneNorm*=-1;
			cutPlaneOrigin=pWeldPlate->ucs.origin;
			if(fCosa>0)
				cutPlaneOrigin+=pWeldPlate->ucs.axis_z*pWeldPlate->Thick;
			UpdateTubeOddment();	//���¸ֹ�����ͷ
		}
		if(GetPropValueStr(GetPropID("m_hWeldLinkObj"),sValueText))
			m_propList.SetItemPropValue(GetPropID("m_hWeldLinkObj"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneOrigin.x"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneOrigin.x"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneOrigin.y"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneOrigin.y"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneOrigin.z"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneOrigin.z"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneNorm.x"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneNorm.x"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneNorm.y"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneNorm.y"),sValueText);
		if(GetPropValueStr(GetPropID("cutPlaneNorm.z"),sValueText))
			m_propList.SetItemPropValue(GetPropID("cutPlaneNorm.z"),sValueText);
	}
	else if(GetPropID("design_axis_z.hVicePart")==GetEventPropId())
	{
		if(console.FromPartHandle(m_hPickObj)==NULL)
			m_hPickObj=0;
		if(m_xSubPropList.m_nObjClassTypeId==0)
			setupInfo.workNorm.hVicePart=m_hPickObj;	
		else if(m_xSubPropList.m_nObjClassTypeId==1)
			setupInfo.workNormUpVert.hVicePart=m_hPickObj;	
		else if(m_xSubPropList.m_nObjClassTypeId==2)
			setupInfo.workNormDownVert.hVicePart=m_hPickObj;	
		if(GetPropValueStr(GetPropID("design_axis_z.hVicePart"),sValueText))
			m_xSubPropList.SetItemPropValue(GetPropID("design_axis_z.hVicePart"),sValueText);
	}
	else if(GetPropID("design_axis_z.hCrossPart")==GetEventPropId())
	{
		if(console.FromPartHandle(m_hPickObj,CLS_LINEPART)==NULL)
			m_hPickObj=0;
		if(m_xSubPropList.m_nObjClassTypeId==0)
			setupInfo.workNorm.hCrossPart=m_hPickObj;	
		else if(m_xSubPropList.m_nObjClassTypeId==1)
			setupInfo.workNormUpVert.hCrossPart=m_hPickObj;	
		else if(m_xSubPropList.m_nObjClassTypeId==2)
			setupInfo.workNormDownVert.hCrossPart=m_hPickObj;	
		if(GetPropValueStr(GetPropID("design_axis_z.hCrossPart"),sValueText))
			m_xSubPropList.SetItemPropValue(GetPropID("design_axis_z.hCrossPart"),sValueText);
	}
}
//���ʮ�ֲ�����,ʹ�ð��ִ������ʱ�ݲ�֧��������(�����һ�㲻�����) wht 11-05-10
void CDesignTubeEndLjDlg::DesignXEndFillPlate(CLDSParamPlate *pParamPlate)
{
	if(pParamPlate==NULL||pParamPlate->m_iParamType!=TYPE_XEND)
		return;
	if(FillPlatePara.nThick<=0)
		return;
	double S=0,N=0,L=0,M=0,A=0;
	pParamPlate->GetDesignItemValue('S',&S);
	pParamPlate->GetDesignItemValue('N',&N);
	pParamPlate->GetDesignItemValue('L',&L);
	pParamPlate->GetDesignItemValue('M',&M);
	pParamPlate->GetDesignItemValue('A',&A);
	CLsRef *pLsRef=NULL;
	for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
		(*pLsRef)->feature=0;	//��ʼ����˨feature=0,���������˨�Ƿ��ѵ���������ƫ����
	ATTACHPART_ITEM *pItem=NULL;
	int nCount=0;	//������
	CLDSPlate *plate_ptr_arr[4]={NULL};	//��¼���ָ�룬�����ӵ�ʮ�ֲ�帽�Ӱ�������
	for(pItem=pParamPlate->GetFirstAttachPart();pItem;pItem=pParamPlate->GetNextAttachPart())
	{
		CLDSPlate *pPlate=(CLDSPlate*)console.FromPartHandle(pItem->h,CLS_PLATE);
		if(pPlate==NULL)
			continue;
		if(fabs(pPlate->ucs.axis_z*pParamPlate->ucs.axis_z)<EPS_COS)
			continue;	//��������巨�߲�ͬ
		double fLen=pPlate->GetLength();
		double fWidth=pPlate->GetWidth();
		if(!(fabs(fLen-S)<EPS&&fabs(fWidth-N)<EPS)
			&&!(fabs(fWidth-S)<EPS&&fabs(fLen-N)<EPS))
			continue;	//������������峤��ͬ�ĸ��Ӱ�
		//1.�������
		plate_ptr_arr[nCount]=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		plate_ptr_arr[nCount]->CopyProperty(pPlate);
		plate_ptr_arr[nCount]->cMaterial=FillPlatePara.cMat;		//����
		plate_ptr_arr[nCount]->SetPartNo(FillPlatePara.sPartNo);	//���
		plate_ptr_arr[nCount]->Thick=FillPlatePara.nThick;			//���
		if(pPlate->m_fNormOffset<0)							//����ƫ����
			plate_ptr_arr[nCount]->m_fNormOffset=-FillPlatePara.nThick;
		//��λ��׼��
		plate_ptr_arr[nCount]->designInfo.origin=pPlate->designInfo.origin;
		//������Ʋ���
		plate_ptr_arr[nCount]->designInfo.norm=pPlate->designInfo.norm;
		//����ϵ
		plate_ptr_arr[nCount]->ucs=pPlate->ucs;
		//���ְ�������
		plate_ptr_arr[nCount]->DesignSetupUcs();
		plate_ptr_arr[nCount]->vertex_list.append(PROFILE_VERTEX(0,0,0));
		plate_ptr_arr[nCount]->vertex_list.append(PROFILE_VERTEX(L-M-A,0,0));
		plate_ptr_arr[nCount]->vertex_list.append(PROFILE_VERTEX(L-M-A,N,0));
		plate_ptr_arr[nCount]->vertex_list.append(PROFILE_VERTEX(0,N,0));
		plate_ptr_arr[nCount]->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(plate_ptr_arr[nCount]->GetSolidPartObject());
		//2.���������λ��
		if(pPlate->m_fNormOffset<0)
			pPlate->m_fNormOffset-=FillPlatePara.nThick;
		else 
			pPlate->m_fNormOffset+=FillPlatePara.nThick;
		pPlate->SetModified();
		pPlate->Create3dSolidModel(); //��������λ�ú���������ʵ��
		g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
		//
		CLsRef *pLsRef=NULL;
		for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			//3.������˨�ķ���ƫ����
			if(pBolt->feature==0)
			{
				pBolt->des_base_pos.norm_offset.AddThick(-plate_ptr_arr[nCount]->handle,TRUE);
				pBolt->feature=1;	//��ʾ��˨�����ù�����ƫ����
			}
			f3dPoint pos=pBolt->ucs.origin;
			coord_trans(pos,pParamPlate->ucs,FALSE);
			if(pos.x<L)
			{	//4.�Զ����������˨��������˨ͨ��
				plate_ptr_arr[nCount]->AppendLsRef(pBolt->GetLsRef());
				pBolt->AddL0Thick(plate_ptr_arr[nCount]->handle,TRUE,TRUE);
			}
			pBolt->CalGuigeAuto();
			pBolt->correct_pos();
			pBolt->SetModified();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	//5.�������ӵ�ʮ�ֲ�帽�Ӱ���
	for(int i=0;i<nCount;i++)
		pParamPlate->AppendAttachPart(plate_ptr_arr[i]);
	g_pSolidDraw->Draw();
}
BOOL  CDesignTubeEndLjDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	CXhChar200 helpStr;
	if(m_iTubeEndLjStyle==0)
		sprintf(helpStr,"%s::/����ο�/menu_cmd_list.htm#ƽ���и�",theApp.m_pszHelpFilePath);
	else if(m_iTubeEndLjStyle==1)
		sprintf(helpStr,"%s::/����ο�/menu_cmd_list.htm#�ֹ����",theApp.m_pszHelpFilePath);
	else if(m_iTubeEndLjStyle==3)
		sprintf(helpStr,"%s::/����ο�/menu_cmd_list.htm#U�Ͳ��",theApp.m_pszHelpFilePath);
	else if(m_iTubeEndLjStyle==4)
		sprintf(helpStr,"%s::/����ο�/menu_cmd_list.htm#���Ͳ��",theApp.m_pszHelpFilePath);
	else if(m_iTubeEndLjStyle==5)
		sprintf(helpStr,"%s::/����ο�/menu_cmd_list.htm#ʮ���Ͳ��",theApp.m_pszHelpFilePath);
	else
	{
		sprintf(helpStr,"%s::/����ο�/menu_cmd_list.htm",theApp.m_pszHelpFilePath);
		AfxMessageBox("��ȱ�ٸ����Եİ������⣬��ֱ���������Ӧ����ϵ��");
	}
	HtmlHelp((DWORD_PTR)(char*)helpStr,HH_DISPLAY_TOPIC);
	return TRUE;
}

void CDesignTubeEndLjDlg::OnMove(int x, int y)
{
	CCallBackDialog::OnMove(x, y);

	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
#endif

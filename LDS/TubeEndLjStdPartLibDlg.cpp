// TubeEndLjStdPartLibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "TubeEndLjStdPartLibDlg.h"
#include "image.h"
#include "XhCharString.h"
#include "GlobalFunc.h"
#include "database.h"
#include "FlCoupleLevelDlg.h"
#include "SortFunc.h"
#include "PartsDbDlg.h"
#include "LDSDoc.h"
#include "PropertyListOper.h"
#include "ExcelOper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTubeEndLjStdPartLibDlg dialog

#ifndef __TSA_
CTubeEndLjStdPartLibDlg::CTubeEndLjStdPartLibDlg(CWnd* pParent /*=NULL*/)
	: CRememberLocationDlg(CTubeEndLjStdPartLibDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTubeEndLjStdPartLibDlg)
	m_listBolt.AddColumnHeader("ֱ��");
	m_listBolt.AddColumnHeader("X");
	m_listBolt.AddColumnHeader("Y");
	m_listBolt.AddColumnHeader("Z");
	//
	m_listBoltParam.AddColumnHeader("S");
	m_listBoltParam.AddColumnHeader("R");
	m_listBoltParam.AddColumnHeader("C");
	m_listBoltParam.AddColumnHeader("ֱ��");
	m_listBoltParam.AddColumnHeader("�о�");
	m_listBoltParam.AddColumnHeader("�о�");
	m_listBoltParam.AddColumnHeader("X");
	m_listBoltParam.AddColumnHeader("Y");
	//
	m_sOperPrompt = _T("��ǰδ�༭�κι���...");
	m_iStdPartType=0;
	m_bNewStdPart=FALSE;
	m_bModify=FALSE;
	m_bModifyBoltPos=FALSE;
	m_iCurDisplayPartType=-1;
	m_hCurEditItem=NULL;
	//}}AFX_DATA_INIT
}


void CTubeEndLjStdPartLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CRememberLocationDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTubeEndLjStdPartLibDlg)
	DDX_Control(pDX, IDC_BOLT_PARAM_LIST, m_listBoltParam);
	DDX_Control(pDX, IDC_TREE_CTRL, m_treeCtrl);
	DDX_Control(pDX, IDC_BOLT_LIST, m_listBolt);
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Text(pDX, IDC_S_OPER_PROMPT, m_sOperPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTubeEndLjStdPartLibDlg, CRememberLocationDlg)
	//{{AFX_MSG_MAP(CTubeEndLjStdPartLibDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CTRL, OnSelchangedTreeCtrl)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_CTRL, OnKeydownTreeCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_CTRL, OnDblclkTreeCtrl)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CTRL, OnRclickTreeCtrl)
	ON_COMMAND(IDC_BTN_UPDATE_LIB,OnBtnUpdateLib)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_CANCEL_MODIFY, OnCancelModify)
	ON_BN_CLICKED(IDC_BTN_SAVE_DATA, OnSaveData)
	ON_COMMAND(ID_IMPORT_EXCEL_FILE, OnImportStdPartLibExcelFile)
	ON_COMMAND(ID_IMPORT_XML_FILE, OnImportStdPartLibFile)
	ON_COMMAND(ID_EXPORT_XML_FILE, OnExportStdPartLibFile)
	ON_COMMAND(ID_NEW_ITEM_FOLDER, OnNewItem)
	ON_COMMAND(ID_DELETE_ALL_ITEM, OnDeleteAllItem)
	ON_COMMAND(ID_EDIT_ITEM_PROP_ITEM, OnEditItem)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	ON_COMMAND(ID_COPY_AND_NEW_ITEM, OnCopyAndNewItem)
	ON_COMMAND(ID_FL_COUPLE_LEVEL_TABLE, OnFlCoupleLevelTable)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(ID_CONFIRM, OnConfirm)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_BOLT_LIST, OnItemchangedBoltList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//�ص���������
static void CopyInsertPlateParam(INSERT_PLATE_PARAM &dest_param,const INSERT_PLATE_PARAM scr_param)
{
	dest_param.W=scr_param.W;
	dest_param.L=scr_param.L;
	dest_param.M=scr_param.M;
	dest_param.H=scr_param.H;
	dest_param.S=scr_param.S;
	dest_param.N=scr_param.N;
	dest_param.Thick=scr_param.Thick;
	dest_param.theroy_weight=scr_param.theroy_weight;
	dest_param.A=scr_param.A;
	dest_param.R=scr_param.R;
	dest_param.d=scr_param.d;
	dest_param.t=scr_param.t;
	dest_param.iLevelMark=scr_param.iLevelMark;
	dest_param.cMaterial=scr_param.cMaterial;
	dest_param.cTubeMat=scr_param.cTubeMat;
	strcpy(dest_param.codeName,scr_param.codeName);
	strcpy(dest_param.datumPlatePartNo,scr_param.datumPlatePartNo);
	strcpy(dest_param.weldPlatePartNo,scr_param.weldPlatePartNo);
	strcpy(dest_param.wrapPlatePartNo,scr_param.wrapPlatePartNo);
	dest_param.iBoltRect=scr_param.iBoltRect;
	for(int i=0;i<scr_param.iBoltRect;i++)
		dest_param.ls_param[i]=scr_param.ls_param[i];
}
static void CopyFlParam(FL_PARAM &dest_param,const FL_PARAM scr_param)
{
	dest_param.d=scr_param.d;
	dest_param.couple_d=scr_param.couple_d;
	dest_param.t=scr_param.t;
	dest_param.A=scr_param.A;
	dest_param.B=scr_param.B;
	dest_param.D=scr_param.D;
	dest_param.K=scr_param.K;
	dest_param.L=scr_param.L;
	dest_param.C=scr_param.C;
	dest_param.H=scr_param.H;
	dest_param.S=scr_param.S;
	dest_param.N=scr_param.N;
	dest_param.R=scr_param.R;
	dest_param.H1=scr_param.H1;
	dest_param.cLevelMark=scr_param.cLevelMark;
	dest_param.cMaterial=scr_param.cMaterial;
	strcpy(dest_param.codeName,scr_param.codeName);
	memcpy(dest_param.group,scr_param.group,sizeof(FL_PARAM::THICK_GROUP)*5);
	dest_param.nBoltD=scr_param.nBoltD;
	dest_param.nBoltNum=scr_param.nBoltNum;
}
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CTubeEndLjStdPartLibDlg *pStdPartLibDlg=(CTubeEndLjStdPartLibDlg*)pListCtrl->GetParent();
	if(pStdPartLibDlg==NULL)
		return FALSE;
	if(pStdPartLibDlg->GetStdPartType()>2)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING,ID_NEW_BOLT_ITEM,"�½���˨����");
	pMenu->AppendMenu(MF_STRING,ID_DELETE_BOLT_ITEM,"ɾ����˨����");
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pStdPartLibDlg);
	return TRUE;
}
static BOOL FireModifyValue(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{	//�޸���˨λ�ú��ػ�Ԥ��ͼ
	CTubeEndLjStdPartLibDlg *pStdPartLibDlg=(CTubeEndLjStdPartLibDlg*)pListCtrl->GetParent();
	if(pStdPartLibDlg==NULL)
		return FALSE;
	CString sOldValue=_T("");
	if(pSelItem&&pSelItem->m_lpNodeInfo)
		sOldValue=pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	//��ǰ��Ԫ���������޸ģ����Ĺ���Ϊ�༭״̬
	if(sOldValue.CompareNoCase(sTextValue)!=0)
	{
		CXhChar100 sText;
		if(pStdPartLibDlg->GetStdPartType()<3)
		{	//���
			LAYOUT_LS_PARAM *pLsPara=(LAYOUT_LS_PARAM*)pSelItem->m_idProp;
			if(pLsPara==NULL)
				return FALSE;
			if(iSubItem==0)		//��˨����
				pLsPara->iBoltNum=atoi(sTextValue);
			else if(iSubItem==1)//����
				pLsPara->iRow=atoi(sTextValue);
			else if(iSubItem==2)//����
				pLsPara->iColumn=atoi(sTextValue);
			else if(iSubItem==3)//ֱ��
				pLsPara->iBoltD=atoi(sTextValue);
			else if(iSubItem==4)//�о�
				pLsPara->fVertSpace=atof(sTextValue);
			else if(iSubItem==5)//�о�
				pLsPara->fHorzSpace=atof(sTextValue);
			else if(iSubItem==6)//X
				pLsPara->fBoltPosX=atof(sTextValue);
			else if(iSubItem==7)//Y
				pLsPara->fBoltPosY=atof(sTextValue);
			
		}
		else if(pStdPartLibDlg->GetStdPartType()>=3)
		{
			int iCurSel=pSelItem->GetIndex();
			if(iCurSel>0)
				return FALSE;
			if(iSubItem==0)		//��˨ֱ��
			{
				if(pStdPartLibDlg->GetStdPartType()<5)
					pStdPartLibDlg->fl_param.nBoltD=atoi(sTextValue);
				else
					pStdPartLibDlg->fl_param2.nBoltD=atoi(sTextValue);
			}
			else if(iSubItem==1)//����
			{
				if(pStdPartLibDlg->GetStdPartType()<5)
					pStdPartLibDlg->fl_param.nBoltNum=atoi(sTextValue);
				else
					pStdPartLibDlg->fl_param2.nBoltNum=atoi(sTextValue);
			}
			else if(iSubItem==2)//��˨����ֱ��
			{	
				if(pStdPartLibDlg->GetStdPartType()<5)
					pStdPartLibDlg->fl_param.K=atof(sTextValue);
				else
					pStdPartLibDlg->fl_param2.fBoltCirD=atof(sTextValue);
			}
			//������������˨��Ϣ��ʾ
			CPropTreeItem* pGroupItem=pStdPartLibDlg->m_propList.FindItemByPropId(CTubeEndLjStdPartLibDlg::GetPropID("LayputBoltParam"),NULL);
			if(pGroupItem)
				pStdPartLibDlg->UpdateBoltLayParaProperty(pGroupItem);
		}
		//���²�����˨,ˢ�¶Ի�����ʾ
		pStdPartLibDlg->RefreshBoltList();
		pStdPartLibDlg->Redraw();
		pStdPartLibDlg->UpdateStdPartState();
	}
	pStdPartLibDlg->m_bModifyBoltPos=TRUE;	//��˨λ�÷����仯
	return TRUE;
}

static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem)
{	
	CTubeEndLjStdPartLibDlg *pStdPartLibDlg=(CTubeEndLjStdPartLibDlg*)pListCtrl->GetParent();
	if(pStdPartLibDlg==NULL || pStdPartLibDlg->GetStdPartType()>2)
		return FALSE;
	LAYOUT_LS_PARAM *pSelLsPara=(LAYOUT_LS_PARAM*)pItem->m_idProp;
	if(pSelLsPara==NULL)
		return FALSE;
	for(LAYOUT_LS_PARAM *pLsPara=pStdPartLibDlg->lsParaList.GetFirst();pLsPara;pLsPara=pStdPartLibDlg->lsParaList.GetNext())
	{
		if(pLsPara==pSelLsPara)
		{
			pStdPartLibDlg->lsParaList.DeleteCursor();
			break;
		}
	}
	pStdPartLibDlg->lsParaList.Clean();
	//ˢ����˨�����б�
	pStdPartLibDlg->RefreshBoltList();
	//ˢ�½��������ʾ״̬
	pStdPartLibDlg->UpdateStdPartState();
	pStdPartLibDlg->Invalidate(FALSE);
	return TRUE;
}
static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,NM_LISTVIEW* pNMListView)
{	//�޸���˨λ�ú��ػ�Ԥ��ͼ wht 10-09-13
	CTubeEndLjStdPartLibDlg *pStdPartLibDlg=(CTubeEndLjStdPartLibDlg*)pListCtrl->GetParent();
	if(pStdPartLibDlg==NULL || pStdPartLibDlg->GetStdPartType()>2)
		return FALSE;
	LAYOUT_LS_PARAM *pSelLsPara=(LAYOUT_LS_PARAM*)pItem->m_idProp;
	if(pSelLsPara==NULL)
		return FALSE;
	//ȡ������ѡ�����ѡ��״̬
	int nCount=pStdPartLibDlg->m_listBolt.GetItemCount();
	pStdPartLibDlg->m_listBolt.SetItemState(-1,0,LVNI_SELECTED);
	for(int i=0;i<nCount;i++)
	{	//
		long nFlag=pStdPartLibDlg->m_listBolt.GetItemData(i);
		if(abs(nFlag)==pItem->GetIndex()+1)
			pStdPartLibDlg->m_listBolt.SetItemState(i,LVNI_SELECTED|LVNI_FOCUSED,LVNI_SELECTED|LVNI_FOCUSED);
	}
	//������������ʾ
	pStdPartLibDlg->LayoutBoltPara.CopyLayoutBoltPara(*pSelLsPara);
	CPropTreeItem* pGroupItem=pStdPartLibDlg->m_propList.FindItemByPropId(CTubeEndLjStdPartLibDlg::GetPropID("LayputBoltParam"),NULL);
	if(pGroupItem)
		pStdPartLibDlg->UpdateBoltLayParaProperty(pGroupItem);
	pStdPartLibDlg->Redraw();
	return TRUE;
}

static BOOL PropHelpPrompt(CPropertyList*  pPropList,CPropTreeItem* pItem)
{
	CTubeEndLjStdPartLibDlg *pStdPartLibDlg=(CTubeEndLjStdPartLibDlg*)pPropList->GetParent();
	ASSERT(pStdPartLibDlg);
	BOOL bDispRibPlateSketch=FALSE;
	//�жκ�ʱ��ʾ�߰�ʾ��ͼ
	if((CTubeEndLjStdPartLibDlg::GetPropID("nRibPlateNum")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("cRibPlateMat")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("ribPlateWeight")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("A")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("B")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("C")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("D")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("E")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("F")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("G")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("H")==pItem->m_idProp||
		CTubeEndLjStdPartLibDlg::GetPropID("T")==pItem->m_idProp)&&
		pStdPartLibDlg->GetStdPartType()>4)	//���Է��������Է���
		bDispRibPlateSketch=TRUE;
	pStdPartLibDlg->RefreshSketch(bDispRibPlateSketch);
	return TRUE;
}

static BOOL ButtonClickItem(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CTubeEndLjStdPartLibDlg *pStdPartLibDlg=(CTubeEndLjStdPartLibDlg*)pPropList->GetParent();
	ASSERT(pStdPartLibDlg);	
	if(CTubeEndLjStdPartLibDlg::GetPropID("theroy_weight")==pItem->m_idProp)
	{
		PARAM_PLATE_TYPE iPlateType;
		int iStdType=pStdPartLibDlg->GetStdPartType();
		double fWeightByCal=0,fWeight=0;
		if(iStdType>=0&&iStdType<3)
		{	//��������������
			if(iStdType==0)
				iPlateType=TYPE_ROLLEND;
			else if(iStdType==1)
				iPlateType=TYPE_UEND;
			else if(iStdType==2)
				iPlateType=TYPE_XEND;
			fWeight=pStdPartLibDlg->insert_plate_param.theroy_weight;
			fWeightByCal=pStdPartLibDlg->CalTheroyWeight(pStdPartLibDlg->insert_plate_param,iPlateType);
		}
		else if(iStdType==3||iStdType==4)
		{	//���㷨����������
			if(iStdType==3)
				iPlateType=TYPE_FLD;
			else 
			{
				iPlateType=TYPE_FLP;
				fWeight=pStdPartLibDlg->fl_param.Weight();
				fWeightByCal=pStdPartLibDlg->CalTheroyWeight(pStdPartLibDlg->fl_param,iPlateType);
			}
		}
		else if(iStdType==5||iStdType==6)
		{
			if(iStdType==5)
				iPlateType=TYPE_FLG;
			else 
				iPlateType=TYPE_FLR;
			fWeight=pStdPartLibDlg->fl_param2.theroy_weight;
			fWeightByCal=pStdPartLibDlg->CalTheroyWeight(pStdPartLibDlg->fl_param2,iPlateType);
		}
		double fError=fabs(fWeightByCal-fWeight);
		if(fError>0.001)
		{
			CXhChar100 sPropmt("����õ�����������Ϊ:%.3f,���ֵΪ%.3f,�Ƿ��滻��ǰ����(Y/N)��",fWeightByCal,fError);
			if(AfxMessageBox(sPropmt,MB_YESNO)==IDYES)
			{
				if(iStdType==3||iStdType==4)
					pStdPartLibDlg->fl_param.group[0].weight=fWeightByCal;
				else if(iStdType==5||iStdType==6)
					pStdPartLibDlg->fl_param2.theroy_weight=fWeightByCal;
				else 
					pStdPartLibDlg->insert_plate_param.theroy_weight=fWeightByCal;
				if(pStdPartLibDlg->GetPropValueStr(pItem->m_idProp,sPropmt)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,sPropmt);
				pStdPartLibDlg->UpdateStdPartState();	//���±�׼��������״̬
			}
		}
		else 
			MessageBox(NULL,"��ǰ�����������������������һ�£�","��ʾ",MB_OK);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("Profile")==pItem->m_idProp)
	{	//�����
		CPartsDbDlg partsDlg;
		partsDlg.m_bRefreshDlgCtrl = TRUE;
		if(pStdPartLibDlg->GetStdPartType()==0)
			partsDlg.m_bIncParamRollEnd=TRUE;
		else if(pStdPartLibDlg->GetStdPartType()==1)
			partsDlg.m_bIncParamUend=TRUE;
		else if(pStdPartLibDlg->GetStdPartType()==2)
			partsDlg.m_bIncParamXEnd=TRUE;
		else 
			return FALSE;
		if(partsDlg.DoModal()==IDOK)
		{
			CLDSParamPlate *pSrcParamPlate=(CLDSParamPlate*)partsDlg.m_pSelectPart;
			if(!pSrcParamPlate)
				return FALSE;
			if(pSrcParamPlate->m_iParamType!=TYPE_ROLLEND&&pSrcParamPlate->m_iParamType!=TYPE_UEND
				&&pSrcParamPlate->m_iParamType!=TYPE_XEND)
				return FALSE;
			if(pSrcParamPlate->m_bStdPart)	
			{	//����׼����
				CLDSStdPartModel stdPartModel;
				if(!pSrcParamPlate->GetStdPartModel(&stdPartModel))
					return FALSE;
				pStdPartLibDlg->insert_plate_param.W=stdPartModel.param.insert_plate_param.W;
				pStdPartLibDlg->insert_plate_param.L=stdPartModel.param.insert_plate_param.L;
				pStdPartLibDlg->insert_plate_param.M=stdPartModel.param.insert_plate_param.M;
				pStdPartLibDlg->insert_plate_param.H=stdPartModel.param.insert_plate_param.H;
				pStdPartLibDlg->insert_plate_param.N=stdPartModel.param.insert_plate_param.N;
				pStdPartLibDlg->insert_plate_param.S=stdPartModel.param.insert_plate_param.S;
				pStdPartLibDlg->insert_plate_param.A=stdPartModel.param.insert_plate_param.A;
				pStdPartLibDlg->insert_plate_param.R=stdPartModel.param.insert_plate_param.R;
				pStdPartLibDlg->insert_plate_param.Thick=stdPartModel.param.insert_plate_param.Thick;
			}
			else
			{
				pSrcParamPlate->GetDesignItemValue('W', &pStdPartLibDlg->insert_plate_param.W);
				pSrcParamPlate->GetDesignItemValue('L', &pStdPartLibDlg->insert_plate_param.L);
				pSrcParamPlate->GetDesignItemValue('M', &pStdPartLibDlg->insert_plate_param.M);
				pSrcParamPlate->GetDesignItemValue('H', &pStdPartLibDlg->insert_plate_param.H);
				pSrcParamPlate->GetDesignItemValue('N', &pStdPartLibDlg->insert_plate_param.N);
				pSrcParamPlate->GetDesignItemValue('S', &pStdPartLibDlg->insert_plate_param.S);
				pSrcParamPlate->GetDesignItemValue('A', &pStdPartLibDlg->insert_plate_param.A);
				pSrcParamPlate->GetDesignItemValue('R', &pStdPartLibDlg->insert_plate_param.R);
				pStdPartLibDlg->insert_plate_param.Thick=pSrcParamPlate->thick;
			}
			CXhChar100 sProfileParam;
			long idProp=CTubeEndLjStdPartLibDlg::GetPropID("W");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			idProp=CTubeEndLjStdPartLibDlg::GetPropID("L");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			idProp=CTubeEndLjStdPartLibDlg::GetPropID("M");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			idProp=CTubeEndLjStdPartLibDlg::GetPropID("H");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			idProp=CTubeEndLjStdPartLibDlg::GetPropID("N");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			idProp=CTubeEndLjStdPartLibDlg::GetPropID("S");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			idProp=CTubeEndLjStdPartLibDlg::GetPropID("A");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			idProp=CTubeEndLjStdPartLibDlg::GetPropID("R");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			idProp=CTubeEndLjStdPartLibDlg::GetPropID("T");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			//���ݲ����˨������˨���ò���
			pStdPartLibDlg->GetParamPlateLayoutBoltParam(pSrcParamPlate);
			//�Զ�����������
			PARAM_PLATE_TYPE ePlateType=(PARAM_PLATE_TYPE)0;
			int iStdType=pStdPartLibDlg->GetStdPartType();
			if(iStdType==0)
				ePlateType=TYPE_ROLLEND;
			else if(iStdType==1)
				ePlateType=TYPE_UEND;
			else if(iStdType==2)
				ePlateType=TYPE_XEND;
			pStdPartLibDlg->insert_plate_param.theroy_weight=
				pStdPartLibDlg->CalTheroyWeight(pStdPartLibDlg->insert_plate_param,ePlateType);
			idProp=CTubeEndLjStdPartLibDlg::GetPropID("theroy_weight");
			if(pStdPartLibDlg->GetPropValueStr(idProp,sProfileParam)>0)
				pPropList->SetItemPropValue(idProp,sProfileParam);
			pStdPartLibDlg->UpdateStdPartState();	//���±�׼��������״̬
		}
	}
	return TRUE;
}

static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CTubeEndLjStdPartLibDlg *pStdPartLibDlg=(CTubeEndLjStdPartLibDlg*)pPropList->GetParent();
	ASSERT(pStdPartLibDlg);
	CPropertyListOper<CTubeEndLjStdPartLibDlg> oper(pPropList,pStdPartLibDlg);
	int iStdPartType=pStdPartLibDlg->GetStdPartType();
	CXhChar100 sCurParam,tem_str("%s",valueStr);
	int strLen=pStdPartLibDlg->GetPropStr(pItem->m_idProp,sCurParam);
	if(strLen==1&&((sCurParam[0]>='A'&&sCurParam[0]<='Z')||(sCurParam[0]>='a'&&sCurParam[0]<='z')))
	{	//����������Ʋ��� Ϊ A~Z��a~z֮����ַ�
		if(iStdPartType<3)
		{	//������β���
			if(sCurParam[0]=='A')
				pStdPartLibDlg->insert_plate_param.A=atof(tem_str);
			else if(sCurParam[0]=='H')
				pStdPartLibDlg->insert_plate_param.H=atof(tem_str);
			else if(sCurParam[0]=='L')
				pStdPartLibDlg->insert_plate_param.L=atof(tem_str);
			else if(sCurParam[0]=='M')
				pStdPartLibDlg->insert_plate_param.M=atof(tem_str);
			else if(sCurParam[0]=='N')
				pStdPartLibDlg->insert_plate_param.N=atof(tem_str);
			else if(sCurParam[0]=='R')
				pStdPartLibDlg->insert_plate_param.R=atof(tem_str);
			else if(sCurParam[0]=='S')
				pStdPartLibDlg->insert_plate_param.S=atof(tem_str);
			else if(sCurParam[0]=='T')
				pStdPartLibDlg->insert_plate_param.Thick=atoi(tem_str);
			else if(sCurParam[0]=='W')
				pStdPartLibDlg->insert_plate_param.W=atof(tem_str);
			else if(sCurParam[0]=='d')
				pStdPartLibDlg->insert_plate_param.d=atof(tem_str);
			else if(sCurParam[0]=='t')
				pStdPartLibDlg->insert_plate_param.t=atof(tem_str);
		}
		else if(iStdPartType==3||iStdPartType==4)
		{	//�������β���
			if(sCurParam[0]=='A')
				pStdPartLibDlg->fl_param.A=atof(tem_str);
			else if(sCurParam[0]=='B')
				pStdPartLibDlg->fl_param.B=atof(tem_str);
			else if(sCurParam[0]=='C')
				pStdPartLibDlg->fl_param.C=atof(tem_str);
			else if(sCurParam[0]=='D')
				pStdPartLibDlg->fl_param.D=atof(tem_str);
			else if(sCurParam[0]=='K')
			{
				pStdPartLibDlg->fl_param.K=atof(tem_str);
				pStdPartLibDlg->RefreshBoltLayoutList();
				pStdPartLibDlg->RefreshBoltList();
			}
			else if(sCurParam[0]=='L')
				pStdPartLibDlg->fl_param.L=atof(tem_str);
			else if(sCurParam[0]=='H')
				pStdPartLibDlg->fl_param.H=atof(tem_str);
			else if(sCurParam[0]=='R')
				pStdPartLibDlg->fl_param.R=atof(tem_str);
			else if(sCurParam[0]=='N')
				pStdPartLibDlg->fl_param.N=atof(tem_str);
			else if(sCurParam[0]=='S')
				pStdPartLibDlg->fl_param.S=atof(tem_str);
			else if(sCurParam[0]=='d')
				pStdPartLibDlg->fl_param.d=atof(tem_str);
			else if(sCurParam[0]=='t')
				pStdPartLibDlg->fl_param.t=atof(tem_str);
		}
		else 
		{
			if(sCurParam[0]=='d')
				pStdPartLibDlg->fl_param2.d=atof(tem_str);
			else if(sCurParam[0]=='T')
				pStdPartLibDlg->fl_param2.para.T=atof(tem_str);
			else if(sCurParam[0]=='A')
				pStdPartLibDlg->fl_param2.para.A=atof(tem_str);
			else if(sCurParam[0]=='B')
				pStdPartLibDlg->fl_param2.para.B=atof(tem_str);
			else if(sCurParam[0]=='C')
				pStdPartLibDlg->fl_param2.para.C=atof(tem_str);
			else if(sCurParam[0]=='D')
				pStdPartLibDlg->fl_param2.para.D=atof(tem_str);
			else if(sCurParam[0]=='E')
				pStdPartLibDlg->fl_param2.para.E=atof(tem_str);
			else if(sCurParam[0]=='F')
				pStdPartLibDlg->fl_param2.para.F=atof(tem_str);
			else if(sCurParam[0]=='G')
				pStdPartLibDlg->fl_param2.para.G=atof(tem_str);
			else if(sCurParam[0]=='H')
				pStdPartLibDlg->fl_param2.para.H=atof(tem_str);
		}
		
		//�Զ�����������
		PARAM_PLATE_TYPE ePlateType=(PARAM_PLATE_TYPE)0;
		int iStdType=pStdPartLibDlg->GetStdPartType();
		if(iStdType>=0&&iStdType<3)
		{	//��������������
			if(iStdType==0)
				ePlateType=TYPE_ROLLEND;
			else if(iStdType==1)
				ePlateType=TYPE_UEND;
			else if(iStdType==2)
				ePlateType=TYPE_XEND;
			pStdPartLibDlg->insert_plate_param.theroy_weight=
				pStdPartLibDlg->CalTheroyWeight(pStdPartLibDlg->insert_plate_param,ePlateType);
		}
		else if(iStdType==3||iStdType==4)
		{	//���㷨����������
			if(iStdType==3)
				ePlateType=TYPE_FLD;
			else 
			{
				ePlateType=TYPE_FLP;
				pStdPartLibDlg->fl_param.group[0].weight=
					pStdPartLibDlg->CalTheroyWeight(pStdPartLibDlg->fl_param,ePlateType);
			}
		}
		else if(iStdType==5||iStdType==6)
		{
			if(iStdType==5)
				ePlateType=TYPE_FLG;
			else 
				ePlateType=TYPE_FLR;
			pStdPartLibDlg->fl_param2.theroy_weight=
				pStdPartLibDlg->CalTheroyWeight(pStdPartLibDlg->fl_param2,ePlateType);
		}
		long idProp=CTubeEndLjStdPartLibDlg::GetPropID("theroy_weight");
		if(pStdPartLibDlg->GetPropValueStr(idProp,tem_str)>0)
			pPropList->SetItemPropValue(idProp,tem_str);
		pStdPartLibDlg->UpdateStdPartState();	//���±�׼��������״̬
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("H1"))
		pStdPartLibDlg->fl_param.H1=atof(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("theroy_weight"))
	{
		if(iStdPartType==5||iStdPartType==6)
			pStdPartLibDlg->fl_param2.theroy_weight=atof(tem_str);
		else if(iStdPartType==4)	//ƽ������
			pStdPartLibDlg->fl_param.group[0].weight=atof(tem_str);
		else if(iStdPartType<=2)
			pStdPartLibDlg->insert_plate_param.theroy_weight=atof(tem_str);
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("tA"))
	{	//Ĭ�ϸ�ֵǰ����������ֵ
		pStdPartLibDlg->fl_param.group[0].t=atoi(tem_str);
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("wA"))
	{
		pStdPartLibDlg->fl_param.group[0].weight=pStdPartLibDlg->fl_param.group[1].weight=atof(tem_str);
		pPropList->SetItemPropValue(CTubeEndLjStdPartLibDlg::GetPropID("wB"),valueStr);
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("tB"))
		pStdPartLibDlg->fl_param.group[1].t=atoi(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("wB"))
		pStdPartLibDlg->fl_param.group[1].weight=atof(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("tC"))
		pStdPartLibDlg->fl_param.group[2].t=atoi(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("wC"))
		pStdPartLibDlg->fl_param.group[2].weight=atof(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("tD"))
		pStdPartLibDlg->fl_param.group[3].t=atoi(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("wD"))
		pStdPartLibDlg->fl_param.group[3].weight=atof(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("tE"))
		pStdPartLibDlg->fl_param.group[4].t=atoi(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("wE"))
		pStdPartLibDlg->fl_param.group[4].weight=atof(tem_str);
	//
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("cMaterial"))
	{
		if(iStdPartType>4)
			pStdPartLibDlg->fl_param2.cMaterial=QuerySteelBriefMatMark(tem_str);
		else if(iStdPartType>2)
			pStdPartLibDlg->fl_param.cMaterial=QuerySteelBriefMatMark(tem_str);
		else
			pStdPartLibDlg->insert_plate_param.cMaterial=QuerySteelBriefMatMark(tem_str);
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("sCodeName"))
	{
		if(iStdPartType>4)
			sprintf(pStdPartLibDlg->fl_param2.codeName,"%s",(char*)tem_str);
		else if(iStdPartType>2)
			sprintf(pStdPartLibDlg->fl_param.codeName,"%s",(char*)tem_str);
		else
			sprintf(pStdPartLibDlg->insert_plate_param.codeName,"%s",(char*)tem_str);
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("sPartNo"))
	{
		if(iStdPartType==2)
			sprintf(pStdPartLibDlg->insert_plate_param.datumPlatePartNo,"%s",(char*)tem_str);
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("sWeldPlatePartNo"))
	{
		if(iStdPartType==2)
			sprintf(pStdPartLibDlg->insert_plate_param.weldPlatePartNo,"%s",(char*)tem_str);
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("sWrapPlatePartNo"))
	{
		if(iStdPartType==2)
			sprintf(pStdPartLibDlg->insert_plate_param.wrapPlatePartNo,"%s",(char*)tem_str);
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("couple_d"))
	{
		if(iStdPartType==3||iStdPartType==4)	//�Խӷ����뺸�ӷ���
			pStdPartLibDlg->fl_param.couple_d=atof(tem_str);
		else if(iStdPartType>4)
			pStdPartLibDlg->fl_param2.couple_d=atof(tem_str);
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("fInnerD"))
		pStdPartLibDlg->fl_param2.fInnerD=atof(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("fOuterD"))
		pStdPartLibDlg->fl_param2.fOuterD=atof(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("fThick"))
		pStdPartLibDlg->fl_param2.fThick=atof(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("fH"))
		pStdPartLibDlg->fl_param2.fH=atof(tem_str);		
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("ribPlateWeight"))
		pStdPartLibDlg->fl_param2.para.weight=atof(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("cRibPlateMat"))
		pStdPartLibDlg->fl_param2.para.cMat=QuerySteelBriefMatMark(tem_str);
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("m_fBoltCirD"))
	{
		pStdPartLibDlg->fl_param2.fBoltCirD=atof(tem_str);
		pStdPartLibDlg->RefreshBoltLayoutList();
		pStdPartLibDlg->RefreshBoltList();
	}
	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("m_fBoltD"))
		pStdPartLibDlg->fl_param2.fBoltHoleD=atof(tem_str);

	else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("sMainTubeGuiGe")
		||pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("cMainTubeMat")
		||pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("m_iStdPartLevel")
		||pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("nRibPlateNum"))
	{
		pStdPartLibDlg->fl_param2.nRibPlateNum=atoi(tem_str);
		if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("sMainTubeGuiGe"))
		{
			if(iStdPartType==3)
				pStdPartLibDlg->fl_param.d=atof(tem_str);
			else if(iStdPartType==5||iStdPartType==6)
				pStdPartLibDlg->fl_param2.d=atof(tem_str);
			else
			{
				double d,t;
				restore_JG_guige(valueStr.GetBuffer(),d,t);
				if(iStdPartType==4)
				{	//ƽ����������Ҫ����ֹ�ֱ�� wht 10-12-09
					pStdPartLibDlg->fl_param.d=d;
					pStdPartLibDlg->fl_param.t=t;
				}
				else
				{	//���
					pStdPartLibDlg->insert_plate_param.d=d;
					pStdPartLibDlg->insert_plate_param.t=t;
				}
			}
		}
		else if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("m_iStdPartLevel"))
		{
			if(iStdPartType>4)
				pStdPartLibDlg->fl_param2.cLevelMark=valueStr[0];
			else if(iStdPartType>2)
				pStdPartLibDlg->fl_param.cLevelMark=valueStr[0];
			else
				pStdPartLibDlg->insert_plate_param.iLevelMark=atoi(valueStr);
		}
		else
		{	//����
			if(iStdPartType<3)
				pStdPartLibDlg->insert_plate_param.cTubeMat=QuerySteelBriefMatMark(tem_str);
		}
		/* ����Ŀǰ��׼��������׼��������ͳһ������ʱȥ���Զ����±�׼��������ƵĹ����wjh-2013-10-31
		if(iStdPartType>4)
		{
			//��׼������������ǰ��λ��ʾ�������� FG--���Է��� FR(FX)--���Է���
			//					������λΪ(�Խ�ʱֱ����С��)�ֹ�ֱ����λ��ʮλ����
			//					����λΪ / �Խӷָ���
			//					������λΪ�Խ�ʱֱ���ϴ�ĸֹ�ֱ����λ��ʮλ����
			//					�ڰ�(��)λΪ��������ͬһֱ���ֹܿ����м���ǿ�Ȳ�ͬ�ķ�����һ����A B C D��ʾ
			//					�ھ�λΪ1����2,1��ʾ�Խ�ʱ��ֱ����С�ĸֹ����ӣ�2��ʾ��ֱ���ϴ�ĸֹ�����
			int d_out_third=0,d_out_sec=0;	//ֱ���ϴ�ĸֹ�
			int d_in_third=0,d_in_sec=0;	//ֱ����С�ĸֹ�
			double main_d=pStdPartLibDlg->fl_param2.d;			//����ֱ��
			double couple_d=pStdPartLibDlg->fl_param2.couple_d;	//�ԽӸֹ�ֱ��
			char cLevel=pStdPartLibDlg->fl_param2.cLevelMark;	//���������־
			d_in_third=ftol(main_d)/100;
			d_in_sec=(ftol(main_d)%100)/10;
			//double couple_d=QueryCoupleTubeD(main_d,pStdPartLibDlg->fl_param.cLevelMark);
			//
			CString sCodeName=_T("");
			if(fabs(main_d-couple_d)<EPS)
			{	//�Ⱦ��Խ� 
				if(iStdPartType==5)	//���Է���
					sCodeName.Format("FG%d%d%C",d_in_third,d_in_sec,cLevel);
				else 				//���Է���
				{
					if(pStdPartLibDlg->fl_param2.nRibPlateNum>0)	//���߰�����Է���
						sCodeName.Format("FX%d%d%C",d_in_third,d_in_sec,cLevel);
					else 
						sCodeName.Format("FR%d%d%C",d_in_third,d_in_sec,cLevel);
				}
			}
			else if(couple_d>0)
			{	//���Ⱦ��Խ�
				d_out_third=ftol(couple_d)/100;
				d_out_sec=(ftol(couple_d)%100)/10;
				int iMark=1;
				int min_d_third=d_in_third,min_d_sec=d_in_sec;
				int max_d_third=d_out_third,max_d_sec=d_out_third;
				if(main_d>couple_d)
				{
					min_d_third=d_out_third;		
					min_d_sec=d_out_third;
					max_d_third=d_in_third;
					max_d_sec=d_in_sec;
					iMark=2;
				}
				if(iStdPartType==5)	//���Է���
					sCodeName.Format("FG%d%d/%d%d%c%d",min_d_third,min_d_sec,max_d_third,max_d_sec,cLevel,iMark);
				else 				//���Է���
				{
					if(pStdPartLibDlg->fl_param2.nRibPlateNum>0)	//���߰�����Է���
						sCodeName.Format("FX%d%d/%d%d%c%d",min_d_third,min_d_sec,max_d_third,max_d_sec,cLevel,iMark);
					else 
						sCodeName.Format("FR%d%d/%d%d%c%d",min_d_third,min_d_sec,max_d_third,max_d_sec,cLevel,iMark);
				}
			}
			sprintf(pStdPartLibDlg->fl_param2.codeName,"%s",sCodeName);
			pPropList->SetItemPropValue(CTubeEndLjStdPartLibDlg::GetPropID("sCodeName"),sCodeName);
		}
		else if(iStdPartType>2)
		{	//�����߾�����(�ֶԺ���ƽ������)�����Ѻ��������У��ʲ��ڴ˸������ơ�wjh-2013010-31
			//��׼������������ǰ��λ��ʾ�������� FD--�Ժ����� FP--ƽ������
			//					������λΪȷ�������⾶�ĸֹ�ֱ���İ�λ��ʮλ����
			//					������λΪȷ�������ھ��ĸֹ�ֱ���İ�λ��ʮλ����
			int d_out_third=0,d_out_sec=0;	//ȷ�������⾶�ĸֹ�ֱ���İ�λ��ʮλ����
			int d_in_third=0,d_in_sec=0;	//ȷ�������ھ��ĸֹ�ֱ���İ�λ��ʮλ����
			double main_d=pStdPartLibDlg->fl_param.d;
			d_in_third=ftol(main_d)/100;
			d_in_sec=(ftol(main_d)%100)/10;
			d_out_third=ftol(pStdPartLibDlg->fl_param.couple_d)/100;
			d_out_sec=(ftol(pStdPartLibDlg->fl_param.couple_d)%100)/10;
			CString sCodeName=_T("");
			if(iStdPartType==3)	//�Ժ�����
				sCodeName.Format("FD%d%d%d%d",d_out_third,d_out_sec,d_in_third,d_in_sec);
			else 				//ƽ������
				sCodeName.Format("FP%d%d%d%d",d_out_third,d_out_sec,d_in_third,d_in_sec);
			sprintf(pStdPartLibDlg->fl_param.codeName,"%s",sCodeName);
			pPropList->SetItemPropValue(CTubeEndLjStdPartLibDlg::GetPropID("sCodeName"),sCodeName);
		}
		else
		{
			//���������	��һλ������ʾ���������C,U,X��ȴ��	
			//				�ڶ�λȡ�ֹ�ֱ���İ�λ����
			//				����λȡ�ֹ�ֱ����ʮλ����
			//				����λȡ�ֹܺ�ȵ�ʮλ����
			//				����λȡ�ֹܺ�ȵĸ�λ����
			//				����λ��ʾ���ܲ��� Q235-S Q345-H��
			//				����λ��ʾ�ֹܵ�Ӧ��ϵ�� 1��ʾ100% 5��ʾ50% 7��ʾ70%
			//              Ҳ������������ͬһ���ֹ����ӵĲ�ͬ���͵Ĳ�� 
			int d_third,d_sec;	//ֱ���İ�λ�Լ�ʮλ����
			int t_sec,t_firs;	//��ȵ�ʮλ�Լ���λ����
			d_third=ftol(pStdPartLibDlg->insert_plate_param.d)/100;		//ȡֱ���İ�λ����
			d_sec=(ftol(pStdPartLibDlg->insert_plate_param.d)%100)/10;	//ȡֱ����ʮλ����
			t_sec=(ftol(pStdPartLibDlg->insert_plate_param.t)%100)/10;	//ȡ��ȵ�ʮλ����
			t_firs=ftol(pStdPartLibDlg->insert_plate_param.t)%10;		//ȡ��ȵĸ�λ����
			int iLevel=pStdPartLibDlg->insert_plate_param.iLevelMark;
			char cTubeMat=pStdPartLibDlg->insert_plate_param.cTubeMat;
			CString sCodeName=_T(""),sDatumPartNo=_T(""),sWeldPlateNo=_T(""),sWrapPlateNo=_T("");
			if(iStdPartType==0)
				sCodeName.Format("C%d%d%d%d%c%d",d_third,d_sec,t_sec,t_firs,cTubeMat,iLevel);
			else if(iStdPartType==1)
				sCodeName.Format("U%d%d%d%d%c%d",d_third,d_sec,t_sec,t_firs,cTubeMat,iLevel);
			else if(iStdPartType==2)
			{
				sCodeName.Format("X%d%d%d%d%c%d",d_third,d_sec,t_sec,t_firs,cTubeMat,iLevel);
				sDatumPartNo.Format("XK%d%d%d%d%c%d",d_third,d_sec,t_sec,t_firs,cTubeMat,iLevel);
				sWeldPlateNo.Format("XZ%d%d%d%d%c%d",d_third,d_sec,t_sec,t_firs,cTubeMat,iLevel);
				sWrapPlateNo.Format("XL%d%d%d%d%c%d",d_third,d_sec,t_sec,t_firs,cTubeMat,iLevel);
			}
			sprintf(pStdPartLibDlg->insert_plate_param.codeName,"%s",sCodeName);
			sprintf(pStdPartLibDlg->insert_plate_param.datumPlatePartNo,"%s",sDatumPartNo);
			sprintf(pStdPartLibDlg->insert_plate_param.weldPlatePartNo,"%s",sWeldPlateNo);
			sprintf(pStdPartLibDlg->insert_plate_param.wrapPlatePartNo,"%s",sWrapPlateNo);
			pPropList->SetItemPropValue(CTubeEndLjStdPartLibDlg::GetPropID("sCodeName"),sCodeName);
			pPropList->SetItemPropValue(CTubeEndLjStdPartLibDlg::GetPropID("sPartNo"),sDatumPartNo);
			pPropList->SetItemPropValue(CTubeEndLjStdPartLibDlg::GetPropID("sWeldPlatePartNo"),sWeldPlateNo);
			pPropList->SetItemPropValue(CTubeEndLjStdPartLibDlg::GetPropID("sWrapPlatePartNo"),sWrapPlateNo);
		}
		*/
	}
	//��˨���ò���
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_nBoltD")==pItem->m_idProp)
	{
		pStdPartLibDlg->LayoutBoltPara.m_nBoltD=atoi(tem_str);
		int iPartType=pStdPartLibDlg->GetStdPartType();
		if(iPartType==3||iPartType==4)
		{
			pStdPartLibDlg->fl_param.nBoltD=atoi(tem_str);
			pStdPartLibDlg->RefreshBoltLayoutList();
			pStdPartLibDlg->RefreshBoltList();
		}
		else if(iPartType==5||iPartType==6)
		{
			pStdPartLibDlg->fl_param2.nBoltD=atoi(tem_str);
			pStdPartLibDlg->RefreshBoltLayoutList();
			pStdPartLibDlg->RefreshBoltList();
		}
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_nBoltNum")==pItem->m_idProp)
	{
		pStdPartLibDlg->LayoutBoltPara.m_nBoltNum=atoi(tem_str);
		int iPartType=pStdPartLibDlg->GetStdPartType();
		if(iPartType==3||iPartType==4)
		{
			pStdPartLibDlg->fl_param.nBoltNum=atoi(tem_str);
			pStdPartLibDlg->RefreshBoltLayoutList();
			pStdPartLibDlg->RefreshBoltList();
		}
		else if(iPartType==5||iPartType==6)
		{
			pStdPartLibDlg->fl_param2.nBoltNum=atoi(tem_str);
			pStdPartLibDlg->RefreshBoltLayoutList();
			pStdPartLibDlg->RefreshBoltList();
		}
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_bAutoLayoutDoubleRow")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pStdPartLibDlg->LayoutBoltPara.m_bAutoLayoutDoubleRow=TRUE;
		else
			pStdPartLibDlg->LayoutBoltPara.m_bAutoLayoutDoubleRow=FALSE;
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_bStaggerLayout")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pStdPartLibDlg->LayoutBoltPara.m_bStaggerLayout=TRUE;
		else
			pStdPartLibDlg->LayoutBoltPara.m_bStaggerLayout=FALSE;
		if(iStdPartType==2)
		{	//ʮ�ֲ��
			if(pStdPartLibDlg->LayoutBoltPara.m_bStaggerLayout)		//���沼��
				pStdPartLibDlg->LayoutBoltPara.m_nRowNum=pStdPartLibDlg->LayoutBoltPara.m_nColNum=-1;
			else 
			{
				pStdPartLibDlg->LayoutBoltPara.m_nRowNum=1;
				pStdPartLibDlg->LayoutBoltPara.m_nColNum=pStdPartLibDlg->LayoutBoltPara.m_nBoltNum;
			}
			pPropList->SetItemPropValue(CTubeEndLjStdPartLibDlg::GetPropID("m_nRowNum"),
										"%d",pStdPartLibDlg->LayoutBoltPara.m_nRowNum);
			pPropList->SetItemPropValue(CTubeEndLjStdPartLibDlg::GetPropID("m_nColNum"),
										"%d",pStdPartLibDlg->LayoutBoltPara.m_nColNum);
			pPropList->SetItemReadOnly(CTubeEndLjStdPartLibDlg::GetPropID("m_nRowNum"),
										pStdPartLibDlg->LayoutBoltPara.m_bStaggerLayout);
			pPropList->SetItemReadOnly(CTubeEndLjStdPartLibDlg::GetPropID("m_nColNum"),
										pStdPartLibDlg->LayoutBoltPara.m_bStaggerLayout);
		}
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_bMirLayout")==pItem->m_idProp)
	{
		if(valueStr.CompareNoCase("��")==0)
			pStdPartLibDlg->LayoutBoltPara.m_bMirLayout=TRUE;
		else
			pStdPartLibDlg->LayoutBoltPara.m_bMirLayout=FALSE;
		//
		pPropList->DeleteItemByPropId(CTubeEndLjStdPartLibDlg::GetPropID("m_fSpecialSpace"));
		if(pStdPartLibDlg->LayoutBoltPara.m_bMirLayout)
			oper.InsertEditPropItem(pItem,"m_fSpecialSpace","","",-1,TRUE);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_fSpecialSpace")==pItem->m_idProp)
		pStdPartLibDlg->LayoutBoltPara.m_fSpecialSpace=atof(tem_str);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_nRowNum")==pItem->m_idProp)
		pStdPartLibDlg->LayoutBoltPara.m_nRowNum=atoi(tem_str);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_nColNum")==pItem->m_idProp)
		pStdPartLibDlg->LayoutBoltPara.m_nColNum=atoi(tem_str);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_fRowSpace")==pItem->m_idProp)
	{
		CString sPosValue=_T("");
		pStdPartLibDlg->LayoutBoltPara.m_fRowSpace=atof(tem_str);
		//�޸��о�ʱ��������˨Y���� wht 11-05-03
		pStdPartLibDlg->LayoutBoltPara.m_posFirstBolt.y=-0.5*atof(tem_str);
		long idProp=CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.y");
		if(pStdPartLibDlg->GetPropValueStr(idProp,tem_str)>0)
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_fColSpace")==pItem->m_idProp)
		pStdPartLibDlg->LayoutBoltPara.m_fColSpace=atof(tem_str);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.x")==pItem->m_idProp)
		pStdPartLibDlg->LayoutBoltPara.m_posFirstBolt.x=atof(tem_str);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.y")==pItem->m_idProp)
		pStdPartLibDlg->LayoutBoltPara.m_posFirstBolt.y=atof(tem_str);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.z")==pItem->m_idProp)
		pStdPartLibDlg->LayoutBoltPara.m_posFirstBolt.z=atof(tem_str);
	pStdPartLibDlg->Redraw();
	//
	if(CTubeEndLjStdPartLibDlg::GetPropID("m_nBoltD")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_nBoltNum")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_bAutoLayoutDoubleRow")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_bStaggerLayout")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_bMirLayout")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_fSpecialSpace")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_nRowNum")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_nColNum")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_fRowSpace")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_fColSpace")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.x")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.y")!=pItem->m_idProp
		&&CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.z")!=pItem->m_idProp)
		pStdPartLibDlg->UpdateStdPartState();
	return TRUE;
}
static BOOL FirePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CTubeEndLjStdPartLibDlg *pStdPartLibDlg=(CTubeEndLjStdPartLibDlg*)pPropList->GetParent();
	if(pStdPartLibDlg==NULL)
		return FALSE;
	CPropertyListOper<CTubeEndLjStdPartLibDlg> oper(pPropList,pStdPartLibDlg);
	if(pItem->m_idProp==CTubeEndLjStdPartLibDlg::GetPropID("LayputBoltParam"))
	{
		pItem->m_lpNodeInfo->SetCheck(iMenu+1);
		if(iMenu==0)		//�����˨����
			pStdPartLibDlg->InsertBoltLayoutParam();
		else //if(iMenu==1)	//������˨����
			pStdPartLibDlg->UpdateBoltLayoutParam();
		return TRUE;
	}
	return FALSE;
}
BOOL ModifyStdPartLibItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CTubeEndLjStdPartLibDlg::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
static BOOL CompareArr(void* arr1,void* arr2,int arr1Num,int arr2Num,int nTypeSize)
{
	if(arr2Num!=arr1Num)
		return FALSE;
	else if(memcmp(arr1,arr2,arr1Num*nTypeSize)!=0)
		return FALSE;
	else 
		return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CTubeEndLjStdPartLibDlg message handlers
IMPLEMENT_PROP_FUNC(CTubeEndLjStdPartLibDlg);
void CTubeEndLjStdPartLibDlg::InitPropHashtable()
{
	int id = 1;
	CTubeEndLjStdPartLibDlg::propHashtable.SetHashTableGrowSize(500);
	CTubeEndLjStdPartLibDlg::propStatusHashtable.CreateHashTable(50);
	//������Ϣ
	AddPropItem("BasicInfo",PROPLIST_ITEM(id++,"��������"));
	AddPropItem("m_iStdPartLevel",PROPLIST_ITEM(id++,"��������","�����Ӧ���Ӧ��ϵ����1��ʾӦ��ϵ��Ϊ100%,5��ʾӦ��ϵ��Ϊ%50��7��ʾӦ��ϵ��Ϊ70%","1|2|3|4|5|6|7|8|9|"));
	AddPropItem("m_iThickLevel",PROPLIST_ITEM(id++,"�ں�������"));
	AddPropItem("sMainTubeGuiGe",PROPLIST_ITEM(id++,"���ܹ��"));
	AddPropItem("cMainTubeMat",PROPLIST_ITEM(id++,"���ܲ���","",MakeMaterialMarkSetString()));
	AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����","",MakeMaterialMarkSetString()));
	AddPropItem("sCodeName",PROPLIST_ITEM(id++,"��������","���������ɷ�����������ֱ���Լ���Ⱦ����������û�ָ��"));
	AddPropItem("sPartNo",PROPLIST_ITEM(id++,"������","һ�����������ͬ"));
	AddPropItem("sWeldPlatePartNo",PROPLIST_ITEM(id++,"������"));
	AddPropItem("sWrapPlatePartNo",PROPLIST_ITEM(id++,"������"));
	AddPropItem("couple_d",PROPLIST_ITEM(id++,"�ԽӸֹ�ֱ��","",MakeTubeSetString(TRUE)));
	AddPropItem("tA",PROPLIST_ITEM(id++,"A�����ܱں�","��λmm"));
	AddPropItem("tB",PROPLIST_ITEM(id++,"B�����ܱں�","��λmm"));
	AddPropItem("tC",PROPLIST_ITEM(id++,"C�����ܱں�","��λmm"));
	AddPropItem("tD",PROPLIST_ITEM(id++,"D�����ܱں�","��λmm"));
	AddPropItem("tE",PROPLIST_ITEM(id++,"E�����ܱں�","��λmm"));
	AddPropItem("wA",PROPLIST_ITEM(id++,"��������","��λkg"));
	AddPropItem("wB",PROPLIST_ITEM(id++,"��������","��λkg"));
	AddPropItem("wC",PROPLIST_ITEM(id++,"��������","��λkg"));
	AddPropItem("wD",PROPLIST_ITEM(id++,"��������","��λkg"));
	AddPropItem("wE",PROPLIST_ITEM(id++,"��������","��λkg"));
	AddPropItem("theroy_weight",PROPLIST_ITEM(id++,"��������","����������λKg","����"));
	//���β���
	AddPropItem("Profile",PROPLIST_ITEM(id++,"���β���","","�����"));
	AddPropItem("fInnerD",PROPLIST_ITEM(id++,"�����ھ�","������Բֱ��"));
	AddPropItem("fOuterD",PROPLIST_ITEM(id++,"�����⾶","������Բֱ��"));
	AddPropItem("fThick",PROPLIST_ITEM(id++,"�������"));
	AddPropItem("fH",PROPLIST_ITEM(id++,"������","�ֹܶ��뷨���������"));
	AddPropItem("H1",PROPLIST_ITEM(id++,"�����߶�H1"));
	for(int i=0;i<26;i++)
	{
		AddPropItem(CXhChar50("%c",int('A'+i)),PROPLIST_ITEM(id++,""));
		AddPropItem(CXhChar50("%c",int('a'+i)),PROPLIST_ITEM(id++,""));
	}
	//�߰���Ϣ
	AddPropItem("RibPlate",PROPLIST_ITEM(id++,"�߰����",""));
	AddPropItem("nRibPlateNum",PROPLIST_ITEM(id++,"�߰����"));
	AddPropItem("ribPlateWeight",PROPLIST_ITEM(id++,"�߰�����"));
	AddPropItem("cRibPlateMat",PROPLIST_ITEM(id++,"�߰����","",MakeMaterialMarkSetString()));
	//��˨���ò���
	AddPropItem("LayputBoltParam",PROPLIST_ITEM(id++,"��˨���ò���","","�����˨����|������˨����"));
	AddPropItem("m_fBoltCirD",PROPLIST_ITEM(id++,"��˨����Բֱ��"));
	AddPropItem("m_fBoltD",PROPLIST_ITEM(id++,"��˨�׾�"));
	AddPropItem("m_nBoltD",PROPLIST_ITEM(id++,"��˨ֱ��","","12|16|20|24|27|30|"));
	AddPropItem("m_nBoltNum",PROPLIST_ITEM(id++,"��˨����"));
	AddPropItem("m_bStaggerLayout",PROPLIST_ITEM(id++,"������","����˫����˨ʱ������ˮƽ������ֱ����˨��","��|��"));
	AddPropItem("m_bMirLayout",PROPLIST_ITEM(id++,"�ԳƲ���","�ԳƲ���������������˨��","��|��"));
	AddPropItem("m_fSpecialSpace",PROPLIST_ITEM(id++,"������"));
	AddPropItem("m_nRowNum",PROPLIST_ITEM(id++,"��˨����R","��˨����,����ĸR��ʾ��˨����"));
	AddPropItem("m_nColNum",PROPLIST_ITEM(id++,"��˨����C","��˨����,����ĸC��ʾ��˨����"));
	AddPropItem("m_fRowSpace",PROPLIST_ITEM(id++,"��˨�о�"));
	AddPropItem("m_fColSpace",PROPLIST_ITEM(id++,"��˨�о�"));
	AddPropItem("m_posFirstBolt.x",PROPLIST_ITEM(id++,"����˨X����"));
	AddPropItem("m_posFirstBolt.y",PROPLIST_ITEM(id++,"����˨Y����"));
	AddPropItem("m_posFirstBolt.z",PROPLIST_ITEM(id++,"����˨Z����"));
}

BOOL CTubeEndLjStdPartLibDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//��ʼ������
	InitParam();
	//��ʼ״̬����ȡ����ť��������
	GetDlgItem(IDC_BTN_CANCEL_MODIFY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SAVE_DATA)->EnableWindow(FALSE);
	//���ݲ��Ͳ���Ƚ��ǽ��ø��¿ⰴť
	GetDlgItem(IDC_BTN_UPDATE_LIB)->EnableWindow(!CompareArr(g_orgPartLib.rollend_param_table.m_pData,
	(void*)rollend_param_table,g_orgPartLib.rollend_param_N,rollend_param_N,sizeof(INSERT_PLATE_PARAM)));
	//��ʼ��������
	m_propList.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	InitPropHashtable();	//��ʼ�����Թ�ϣ��
	//��ʼ����˨�б�
	long col_wide_arr[4]={70,70,70,60};
	m_listBolt.InitListCtrl(col_wide_arr);
	//��ʼ����˨���ò����б�
	long para_col_wide_arr[8]={25,25,25,40,40,40,40,40};
	m_listBoltParam.InitListCtrl(para_col_wide_arr);
	m_listBoltParam.m_hPromptWnd = GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_listBoltParam.SetDblclkDisplayCellCtrl(TRUE);	//����Ϊ˫����ʾ�ɱ༭�ؼ�
	m_listBoltParam.SetModifyValueFunc(FireModifyValue);
	//m_listBoltParam.SetContextMenuFunc(FireContextMenu);
	m_listBoltParam.SetItemChangedFunc(FireItemChanged);
	m_listBoltParam.SetDeleteItemFunc(FireDeleteItem);
	m_listBoltParam.SetImageWidth(0);	//������
	//��ʼ�����ؼ�
	m_imageList.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	m_treeCtrl.SetImageList(&m_imageList,TVSIL_NORMAL);
	m_treeCtrl.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS);
	RefreshTreeCtrl();		//ˢ�����ؼ�
	DisplayPropertyList();
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
	MoveWndToLocation();
	return TRUE;  
}
void CTubeEndLjStdPartLibDlg::InitParam()
{
	memset(&fl_param,0,sizeof(FL_PARAM));
	memset(&fl_param2,0,sizeof(FL_PARAM2));
	memset(&insert_plate_param,0,sizeof(INSERT_PLATE_PARAM));
	fl_param.cMaterial=CSteelMatLibrary::Q235BriefMark();//'S';
	fl_param.cLevelMark='A';
	//
	fl_param2.cMaterial=CSteelMatLibrary::Q235BriefMark();//'S';
	fl_param2.cLevelMark='A';
	//
	insert_plate_param.iLevelMark=1;
	insert_plate_param.cMaterial=CSteelMatLibrary::Q235BriefMark();//'S';
	insert_plate_param.cTubeMat=CSteelMatLibrary::Q235BriefMark();//'S';
	insert_plate_param.Thick=10;
	//
	LayoutBoltPara.m_nBoltNum=4;
	LayoutBoltPara.m_nBoltD=20;
	LayoutBoltPara.m_nColNum=2;
	LayoutBoltPara.m_nRowNum=2;
	LayoutBoltPara.m_fColSpace=80;
	LayoutBoltPara.m_fRowSpace=80;
	LayoutBoltPara.m_fSpecialSpace=100;
	LayoutBoltPara.m_posFirstBolt.x=160;
	LayoutBoltPara.m_posFirstBolt.y=-40;
}
void CTubeEndLjStdPartLibDlg::UpdateLsParas(BOOL bSrcPlateToLsList/*=TRUE*/)
{
	if(m_iStdPartType>2)
		return;	//��ǰ�����ǲ��
	if(bSrcPlateToLsList)
	{	//�������˨���ݸ��µ���˨�б�������
		lsParaList.Empty();
		for(int i=0;i<insert_plate_param.iBoltRect;i++)
			lsParaList.append(insert_plate_param.ls_param[i]);
	}
	else
	{	//����˨�б����ݸ��µ������˨������
		insert_plate_param.iBoltRect=lsParaList.GetNodeNum();
		for(int i=0;i<lsParaList.GetNodeNum();i++)
			insert_plate_param.ls_param[i]=lsParaList[i];
	}
}
TREEITEM_INFO* CTubeEndLjStdPartLibDlg::GetCurSelItemInfo(HTREEITEM* pSelItem/*=NULL*/,BOOL bInitPartType/*=FALSE*/)
{
	HTREEITEM hSelItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL;
	if(hSelItem)
		pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hSelItem);
	if(pSelItem)
		*pSelItem=hSelItem;
	if(pItemInfo && bInitPartType)
	{
		if(pItemInfo->itemType==TYPE_ROLLEND)
			m_iStdPartType=0;
		else if(pItemInfo->itemType==TYPE_UEND)
			m_iStdPartType=1;
		else if(pItemInfo->itemType==TYPE_XEND)
			m_iStdPartType=2;
		else if(pItemInfo->itemType==TYPE_FLD)
			m_iStdPartType=3;
		else if(pItemInfo->itemType==TYPE_FLP)
			m_iStdPartType=4;
		else if(pItemInfo->itemType==TYPE_FLG)
			m_iStdPartType=5;
		else if(pItemInfo->itemType==TYPE_FLR)
			m_iStdPartType=6;
	}
	return pItemInfo;
}
void CTubeEndLjStdPartLibDlg::IntelliSavePreModify()
{
	if(m_bNewStdPart||m_bModify)
	{	//�Ƿ񱣴��ǰһ���������޸�
		CXhChar200 sPrompt,sPartType;
		if(m_bNewStdPart)
			sPartType.Copy("�½�����");
		else 
			sPartType.Copy("ǰһ����");
		if(m_iStdPartType==3||m_iStdPartType==4)
			sPrompt.Printf("�Ƿ񱣴��%s%s�����޸�?",(char*)sPartType,fl_param.codeName);
		else if(m_iStdPartType>4)
			sPrompt.Printf("�Ƿ񱣴��%s%s�����޸�?",(char*)sPartType,fl_param2.codeName);
		else
			sPrompt.Printf("�Ƿ񱣴��%s%s�����޸�?",(char*)sPartType,insert_plate_param.codeName);
		if(AfxMessageBox(sPrompt,MB_YESNO)==IDYES)
		{
			if(!SaveData())
				return;
		}
		else
			OnCancelModify();
	}
}

//ˢ�����ؼ�
void CTubeEndLjStdPartLibDlg::RefreshTreeCtrl()
{
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo();
	int iSelItemType=pItemInfo?pItemInfo->itemType:0;
	//
	int i=0;
	itemInfoList.Empty();
	m_treeCtrl.DeleteAllItems();
	HTREEITEM hSonItem=NULL;
	HTREEITEM hStdPartItem=m_treeCtrl.InsertItem("��׼����",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
	CString sPartType=_T("");
	sPartType.Format("���Ͳ��(%d)",rollend_param_N);
	HTREEITEM hCEndLjItem=m_treeCtrl.InsertItem(sPartType,PRJ_IMG_C_INSERTPLATE,PRJ_IMG_C_INSERTPLATE,hStdPartItem);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_ROLLEND,-1));
	m_treeCtrl.SetItemData(hCEndLjItem,(DWORD)pItemInfo);
	for(i=0;i<rollend_param_N;i++)
	{
		hSonItem=m_treeCtrl.InsertItem(rollend_param_table[i].codeName,PRJ_IMG_PARA,PRJ_IMG_X_EDIT_STATE,hCEndLjItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_ROLLEND,i));
		m_treeCtrl.SetItemData(hSonItem,(DWORD)pItemInfo);
	}
	sPartType.Format("U�Ͳ��(%d)",uend_param_N);
	HTREEITEM hUEndLjItem=m_treeCtrl.InsertItem(sPartType,PRJ_IMG_U_INSERTPLATE,PRJ_IMG_U_INSERTPLATE,hStdPartItem);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_UEND,-1));
	m_treeCtrl.SetItemData(hUEndLjItem,(DWORD)pItemInfo);
	for(i=0;i<uend_param_N;i++)
	{
		hSonItem=m_treeCtrl.InsertItem(uend_param_table[i].codeName,PRJ_IMG_PARA,PRJ_IMG_X_EDIT_STATE,hUEndLjItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_UEND,i));
		m_treeCtrl.SetItemData(hSonItem,(DWORD)pItemInfo);
	}
	sPartType.Format("ʮ�ֲ��(%d)",xend_param_N);
	HTREEITEM hXEndLjItem=m_treeCtrl.InsertItem(sPartType,PRJ_IMG_X_INSERTPLATE,PRJ_IMG_X_INSERTPLATE,hStdPartItem);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_XEND,-1));
	m_treeCtrl.SetItemData(hXEndLjItem,(DWORD)pItemInfo);
	for(i=0;i<xend_param_N;i++)
	{
		hSonItem=m_treeCtrl.InsertItem(xend_param_table[i].codeName,PRJ_IMG_PARA,PRJ_IMG_X_EDIT_STATE,hXEndLjItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_XEND,i));
		m_treeCtrl.SetItemData(hSonItem,(DWORD)pItemInfo);
	}
	sPartType.Format("�Ժ�����(%d)",fld_param_N);
	HTREEITEM hFlDItem=m_treeCtrl.InsertItem(sPartType,PRJ_IMG_FL,PRJ_IMG_FL,hStdPartItem);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_FLD,-1));
	m_treeCtrl.SetItemData(hFlDItem,(DWORD)pItemInfo);
	for(i=0;i<fld_param_N;i++)
	{
		hSonItem=m_treeCtrl.InsertItem(fld_param_table[i].codeName,PRJ_IMG_PARA,PRJ_IMG_X_EDIT_STATE,hFlDItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_FLD,i));
		m_treeCtrl.SetItemData(hSonItem,(DWORD)pItemInfo);
	}
	sPartType.Format("ƽ������(%d)",flp_param_N);
	HTREEITEM hFlPItem=m_treeCtrl.InsertItem(sPartType,PRJ_IMG_FL,PRJ_IMG_FL,hStdPartItem);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_FLP,-1));
	m_treeCtrl.SetItemData(hFlPItem,(DWORD)pItemInfo);
	for(i=0;i<flp_param_N;i++)
	{
		hSonItem=m_treeCtrl.InsertItem(flp_param_table[i].codeName,PRJ_IMG_PARA,PRJ_IMG_X_EDIT_STATE,hFlPItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_FLP,i));
		m_treeCtrl.SetItemData(hSonItem,(DWORD)pItemInfo);
	}
	sPartType.Format("���Է���(%d)",flg_param_N);
	HTREEITEM hRigidFlItem=m_treeCtrl.InsertItem(sPartType,PRJ_IMG_FL,PRJ_IMG_FL,hStdPartItem);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_FLG,-1));
	m_treeCtrl.SetItemData(hRigidFlItem,(DWORD)pItemInfo);
	for(i=0;i<flg_param_N;i++)
	{
		hSonItem=m_treeCtrl.InsertItem(flg_param_tbl[i].codeName,PRJ_IMG_PARA,PRJ_IMG_X_EDIT_STATE,hRigidFlItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_FLG,i));
		m_treeCtrl.SetItemData(hSonItem,(DWORD)pItemInfo);
	}
	sPartType.Format("���Է���(%d)",flr_param_N);
	HTREEITEM hFlexibleFlItem=m_treeCtrl.InsertItem(sPartType,PRJ_IMG_FL,PRJ_IMG_FL,hStdPartItem);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_FLR,-1));
	m_treeCtrl.SetItemData(hFlexibleFlItem,(DWORD)pItemInfo);
	for(i=0;i<flr_param_N;i++)
	{
		hSonItem=m_treeCtrl.InsertItem(flr_param_tbl[i].codeName,PRJ_IMG_PARA,PRJ_IMG_X_EDIT_STATE,hFlexibleFlItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(TYPE_FLR,i));
		m_treeCtrl.SetItemData(hSonItem,(DWORD)pItemInfo);
	}
	//
	m_treeCtrl.Expand(hStdPartItem,TVE_EXPAND);
	if(iSelItemType==TYPE_ROLLEND)
		m_treeCtrl.SelectItem(hCEndLjItem);
	else if(iSelItemType==TYPE_UEND)
		m_treeCtrl.SelectItem(hUEndLjItem);
	else if(iSelItemType==TYPE_XEND)
		m_treeCtrl.SelectItem(hXEndLjItem);
	else if(iSelItemType==TYPE_FLD)
		m_treeCtrl.SelectItem(hFlDItem);
	else if(iSelItemType==TYPE_FLP)
		m_treeCtrl.SelectItem(hFlPItem);
	else if(iSelItemType==TYPE_FLG)
		m_treeCtrl.SelectItem(hRigidFlItem);
	else if(iSelItemType==TYPE_FLR)
		m_treeCtrl.SelectItem(hFlexibleFlItem);
	else
		m_treeCtrl.SelectItem(hCEndLjItem);
}

int CTubeEndLjStdPartLibDlg::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText,sCurParam;
	GetPropStr(id,sCurParam);
	//���β���
	if(sCurParam.GetLength()==1&&((sCurParam[0]>='A'&&sCurParam[0]<='Z')
		||(sCurParam[0]>='a'&&sCurParam[0]<='z')))
	{
		if(m_iStdPartType==3||m_iStdPartType==4)
		{
			if(sCurParam[0]=='A')
				sText.Printf("%f",fl_param.A);
			else if(sCurParam[0]=='B')
			{
				double B=fl_param.B;
				if(m_iStdPartType==3)	//�Ժ�����B�Ƕ������ɲ���
					B=fl_param.A-(2*fl_param.S+2*fl_param.group[0].t);
				sText.Printf("%f",B);
			}
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
		else if(m_iStdPartType==5||m_iStdPartType==6)
		{
			if(sCurParam[0]=='A')
				sText.Printf("%f",fl_param2.para.A);
			else if(sCurParam[0]=='B')
				sText.Printf("%f",fl_param2.para.B);
			else if(sCurParam[0]=='C')
				sText.Printf("%f",fl_param2.para.C);
			else if(sCurParam[0]=='D')
				sText.Printf("%f",fl_param2.para.D);
			else if(sCurParam[0]=='E')
				sText.Printf("%f",fl_param2.para.E);
			else if(sCurParam[0]=='F')
				sText.Printf("%f",fl_param2.para.F);
			else if(sCurParam[0]=='G')
				sText.Printf("%f",fl_param2.para.G);
			else if(sCurParam[0]=='H')
				sText.Printf("%f",fl_param2.para.H);
			else if(sCurParam[0]=='T')
				sText.Printf("%f",fl_param2.para.T);
			else if(sCurParam[0]=='d')
				sText.Printf("%f",fl_param2.d);
		}
		else
		{
			if(sCurParam[0]=='A')
				sText.Printf("%f",insert_plate_param.A);
			else if(sCurParam[0]=='H')
				sText.Printf("%f",insert_plate_param.H);
			else if(sCurParam[0]=='L')
				sText.Printf("%f",insert_plate_param.L);
			else if(sCurParam[0]=='M')
				sText.Printf("%f",insert_plate_param.M);
			else if(sCurParam[0]=='N')
				sText.Printf("%f",insert_plate_param.N);
			else if(sCurParam[0]=='R')
				sText.Printf("%f",insert_plate_param.R);
			else if(sCurParam[0]=='S')
				sText.Printf("%f",insert_plate_param.S);
			else if(sCurParam[0]=='T')
				sText.Printf("%d",insert_plate_param.Thick);
			else if(sCurParam[0]=='W')
				sText.Printf("%f",insert_plate_param.W);
			else if(sCurParam[0]=='d')
				sText.Printf("%f",insert_plate_param.d);
			else if(sCurParam[0]=='t')
				sText.Printf("%f",insert_plate_param.t);
		}
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("H1")==id)
	{
		sText.Printf("%f",fl_param.H1);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("theroy_weight")==id)
	{
		if(m_iStdPartType==4)	//ƽ������
			sText.Printf("%f",fl_param.group[0].weight);
		else if(m_iStdPartType==5||m_iStdPartType==6)
			sText.Printf("%f",fl_param2.theroy_weight);
		else if(m_iStdPartType<=2)
			sText.Printf("%f",insert_plate_param.theroy_weight);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("tA")==id)
		sText.Printf("%d",fl_param.group[0].t);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("wA")==id)
	{
		sText.Printf("%f",fl_param.group[0].weight);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("tB")==id)
		sText.Printf("%d",fl_param.group[1].t);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("wB")==id)
	{
		sText.Printf("%f",fl_param.group[1].weight);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("tC")==id)
		sText.Printf("%d",fl_param.group[2].t);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("wC")==id)
	{
		sText.Printf("%f",fl_param.group[2].weight);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("tD")==id)
		sText.Printf("%d",fl_param.group[3].t);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("wD")==id)
	{
		sText.Printf("%f",fl_param.group[3].weight);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("tE")==id)
		sText.Printf("%d",fl_param.group[4].t);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("wE")==id)
	{
		sText.Printf("%f",fl_param.group[4].weight);
		SimplifiedNumString(sText);
	}
	//������Ϣ
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_iStdPartLevel")==id)
	{	//��׼��������
		if(m_iStdPartType==3||m_iStdPartType==4)
			sText.Printf("%c",fl_param.cLevelMark);
		else if(m_iStdPartType==5||m_iStdPartType==6)
			sText.Printf("%c",fl_param2.cLevelMark);
		else 
			sText.Printf("%d",insert_plate_param.iLevelMark);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("sCodeName")==id)
	{
		if(m_iStdPartType==3||m_iStdPartType==4)
			sText.Printf("%s",fl_param.codeName);
		else if(m_iStdPartType==5||m_iStdPartType==6)
			sText.Printf("%s",fl_param2.codeName);
		else
			sText.Printf("%s",insert_plate_param.codeName);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("sPartNo")==id)
		sText.Printf("%s",insert_plate_param.datumPlatePartNo);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("sWeldPlatePartNo")==id)
		sText.Printf("%s",insert_plate_param.weldPlatePartNo);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("sWrapPlatePartNo")==id)
		sText.Printf("%s",insert_plate_param.wrapPlatePartNo);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("cMaterial")==id)
	{
		char cMat=insert_plate_param.cMaterial;
		if(m_iStdPartType==3||m_iStdPartType==4)
			cMat=fl_param.cMaterial;
		else if(m_iStdPartType==5||m_iStdPartType==6)
			cMat=fl_param2.cMaterial;
		char matStr[20];
		if(QuerySteelMatMark(cMat,matStr))
			sText.Printf("%s",matStr);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("sMainTubeGuiGe")==id)
	{
		CXhChar100 s_d,s_t;
		if(m_iStdPartType==3||m_iStdPartType==4)
		{
			s_d.Printf("%f",fl_param.d);
			s_t.Printf("%f",fl_param.t);
		}
		else if(m_iStdPartType>4)
			s_d.Printf("%f",fl_param2.d);
		else
		{
			s_d.Printf("%f",insert_plate_param.d);
			s_t.Printf("%f",insert_plate_param.t);
		}
		SimplifiedNumString(s_d);
		SimplifiedNumString(s_t);
		if(m_iStdPartType==3||m_iStdPartType>4)	//�Ժ���������ʾ�ֹ�ֱ�� wht 10-12-09
			sText.Printf("%s", (char*)s_d); 
		else
			sText.Printf("%sX%s", (char*)s_d,(char*)s_t); 
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("cMainTubeMat")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(insert_plate_param.cTubeMat,matStr))
			sText.Printf("%s",matStr);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("couple_d")==id)
	{
		if(m_iStdPartType==3||m_iStdPartType==4)	//�Խӷ����뺸�ӷ���
			sText.Printf("%f",fl_param.couple_d);
		else if(m_iStdPartType>4)					//���Է��������Է���
			sText.Printf("%f",fl_param2.couple_d);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("fInnerD")==id)
	{
		sText.Printf("%f",fl_param2.fInnerD);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("fOuterD")==id)
	{
		sText.Printf("%f",fl_param2.fOuterD);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("fThick")==id)
	{
		sText.Printf("%f",fl_param2.fThick);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("fH")==id)
	{
		sText.Printf("%f",fl_param2.fH);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("nRibPlateNum")==id)
		sText.Printf("%d",fl_param2.nRibPlateNum);
	else if(CTubeEndLjStdPartLibDlg::GetPropID("ribPlateWeight")==id)
	{
		sText.Printf("%f",fl_param2.para.weight);
		SimplifiedNumString(sText);
	}
	else if(CTubeEndLjStdPartLibDlg::GetPropID("cRibPlateMat")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(fl_param2.para.cMat,matStr))
			sText.Printf("%s",matStr);
	}
	//��˨���ò���
	else if(CTubeEndLjStdPartLibDlg::GetPropID("m_nBoltD")==id)
	{
		if(m_iStdPartType==3||m_iStdPartType==4)
			sText.Printf("%d",fl_param.nBoltD);
		else if(m_iStdPartType>4)
			sText.Printf("%d",fl_param2.nBoltD);
		else
			sText.Printf("%d",LayoutBoltPara.m_nBoltD);
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_nBoltNum"))
	{
		if(m_iStdPartType==3||m_iStdPartType==4)
			sText.Printf("%d",fl_param.nBoltNum);
		else if(m_iStdPartType>4)
			sText.Printf("%d",fl_param2.nBoltNum);
		else
			sText.Printf("%d",LayoutBoltPara.m_nBoltNum);
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_fBoltCirD"))
	{
		sText.Printf("%f",fl_param2.fBoltCirD);
		SimplifiedNumString(sText);
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_fBoltD"))
	{
		sText.Printf("%f",fl_param2.fBoltHoleD);
		SimplifiedNumString(sText);
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_bAutoLayoutDoubleRow"))
	{
		if(LayoutBoltPara.m_bAutoLayoutDoubleRow)
			sText.Copy("��");
		else
			sText.Copy("��");
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_bStaggerLayout"))
	{
		if(LayoutBoltPara.m_bStaggerLayout)
			sText.Copy("��");
		else
			sText.Copy("��");
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_bMirLayout"))
	{
		if(LayoutBoltPara.m_bMirLayout)
			sText.Copy("��");
		else
			sText.Copy("��");
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_fSpecialSpace"))
	{
		sText.Printf("%f",LayoutBoltPara.m_fSpecialSpace);
		SimplifiedNumString(sText);
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_nRowNum"))
		sText.Printf("%d",LayoutBoltPara.m_nRowNum);
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_nColNum"))
		sText.Printf("%d",LayoutBoltPara.m_nColNum);
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_fRowSpace"))
	{
		sText.Printf("%f",LayoutBoltPara.m_fRowSpace);
		SimplifiedNumString(sText);
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_fColSpace"))
	{
		sText.Printf("%f",LayoutBoltPara.m_fColSpace);
		SimplifiedNumString(sText);
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.x"))
	{
		sText.Printf("%f",LayoutBoltPara.m_posFirstBolt.x);
		SimplifiedNumString(sText);
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.y"))
	{
		sText.Printf("%f",LayoutBoltPara.m_posFirstBolt.y);
		SimplifiedNumString(sText);
	}
	else if(id==CTubeEndLjStdPartLibDlg::GetPropID("m_posFirstBolt.z"))
	{
		sText.Printf("%f",LayoutBoltPara.m_posFirstBolt.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
void CTubeEndLjStdPartLibDlg::UpdateBoltLayParaProperty(CPropTreeItem* pGroupItem)
{
	CPropertyListOper<CTubeEndLjStdPartLibDlg> oper(&m_propList,this);
	//��˨ֱ��
	oper.InsertCmbEditPropItem(pGroupItem,"m_nBoltD","","","",-1,TRUE);
	if(m_iStdPartType>2)
	{	//������˨���ò���
		//��˨�׾�
		if(m_iStdPartType==3||m_iStdPartType==4)
			oper.InsertEditPropItem(pGroupItem,"L","��˨�׾�L","L=��˨�׾�",-1,TRUE);
		else
			oper.InsertEditPropItem(pGroupItem,"m_fBoltD","","",-1,TRUE);
		//��˨����
		oper.InsertEditPropItem(pGroupItem,"m_nBoltNum","","",-1,TRUE);
		//��˨����Բ�뾶
		if(m_iStdPartType==3||m_iStdPartType==4)
			oper.InsertEditPropItem(pGroupItem,"K","��˨����Բֱ��k","K=��˨����Բֱ��",-1,TRUE);
		else
			oper.InsertEditPropItem(pGroupItem,"m_fBoltCirD","","",-1,TRUE);
	}
	else 
	{	//�����˨���ò���
		//��˨����
		if(m_iStdPartType==2)	//ʮ�ֲ��
			oper.InsertEditPropItem(pGroupItem,"m_nBoltNum","1/4��˨��S","1/4��˨��,����ĸS��ʾ1/4��˨��",-1,TRUE);
		else
			oper.InsertEditPropItem(pGroupItem,"m_nBoltNum","��˨����S","",-1,TRUE);
		if(m_iStdPartType!=2)
		{	//��˨����
			oper.InsertEditPropItem(pGroupItem,"m_nRowNum","","",-1,TRUE);
			//��˨����
			oper.InsertEditPropItem(pGroupItem,"m_nColNum","","",-1,TRUE);
			//��˨�о�
			oper.InsertEditPropItem(pGroupItem,"m_fRowSpace","","",-1,TRUE);
		}
		//��˨�о�
		oper.InsertEditPropItem(pGroupItem,"m_fColSpace","","",-1,TRUE);
		//����˨X����
		oper.InsertEditPropItem(pGroupItem,"m_posFirstBolt.x","","",-1,TRUE);
		//����˨Y����
		oper.InsertEditPropItem(pGroupItem,"m_posFirstBolt.y","","",-1,TRUE);
		if(m_iStdPartType==2)
		{	//ʮ�ֲ��
			//������ˮƽ������ֱ����˨
			oper.InsertCmbListPropItem(pGroupItem,"m_bStaggerLayout","","","",-1,TRUE);
			//�ԳƲ���
			CPropTreeItem* pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bMirLayout","","","",-1,TRUE);
			if(LayoutBoltPara.m_bMirLayout)
				oper.InsertEditPropItem(pPropItem,"m_fSpecialSpace","","",-1,TRUE);	//������
		}
	}
}

void CTubeEndLjStdPartLibDlg::DisplayPropertyList()
{
	CXhChar100 sName;
	if(m_iStdPartType==1)
		sName.Copy("U�Ͳ��");
	else if(m_iStdPartType==2)
		sName.Copy("ʮ���Ͳ��");
	else if(m_iStdPartType==3)
		sName.Copy("�Ժ�����");
	else if(m_iStdPartType==4)
		sName.Copy("ƽ������");
	else if(m_iStdPartType==5)
		sName.Copy("���Է���");
	else if(m_iStdPartType==6)
		sName.Copy("���Է���");
	else //if(m_iStdPartType==0)
		sName.Copy("���Ͳ��");
	if(m_iStdPartType!=m_iCurDisplayPartType)
	{	//�����������ص�����
		m_iCurDisplayPartType=m_iStdPartType;
		m_propList.CleanTree();
		m_propList.SetButtonClickFunc(ButtonClickItem);
		m_propList.SetModifyValueFunc(ModifyItemValue);
		m_propList.SetModifyStatusFunc(ModifyStdPartLibItemStatus);
		m_propList.SetPropHelpPromptFunc(PropHelpPrompt);
		m_propList.SetPopMenuClickFunc(FirePopMenuClick);
	}
	CPropertyListOper<CTubeEndLjStdPartLibDlg> oper(&m_propList,this);
	CPropTreeItem* pRoot=m_propList.GetRootItem();
	//��������
	CPropTreeItem* pGroupItem=oper.InsertPropItem(pRoot,"BasicInfo",CString(CXhChar100("%s��������",(char*)sName)));
	if(CTubeEndLjStdPartLibDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CTubeEndLjStdPartLibDlg::GetPropStatus(pGroupItem->m_idProp);
	//���ܹ��
	CPropTreeItem* pPropItem=oper.InsertCmbEditPropItem(pGroupItem,"sMainTubeGuiGe","","","",-1,TRUE);
	pPropItem->m_lpNodeInfo->m_cmbItems=MakeTubeSetString(m_iStdPartType==3||m_iStdPartType>4);   //���Է��������Է�������ʾ�ֹ�ֱ��
	//�ԽӸֹ�ֱ��
	if(m_iStdPartType>2)
		oper.InsertCmbEditPropItem(pGroupItem,"couple_d","","","",-1,TRUE);
	//����
	if(m_iStdPartType<3)
		oper.InsertCmbListPropItem(pGroupItem,"cMainTubeMat","","","",-1,TRUE);
	//����
	if(m_iStdPartType>2)
		oper.InsertCmbListPropItem(pGroupItem,"m_iStdPartLevel","1|2|3|4|5|6|7|8|9|","��������","",-1,TRUE);
	else
		oper.InsertCmbListPropItem(pGroupItem,"m_iStdPartLevel","1|2|3|4|5|6|7|8|9|","��弶��","",-1,TRUE);
	//��׼��������
	if(m_iStdPartType>2)
		oper.InsertEditPropItem(pGroupItem,"sCodeName","","",-1,TRUE);
	else
		oper.InsertEditPropItem(pGroupItem,"sCodeName","������","�������ɲ�����͡�����ֱ��������Լ����ܲ��ʾ�����Ҳ�����û�ָ��",-1,TRUE);
	if(m_iStdPartType==2)
	{	//ʮ�ֲ��
		oper.InsertEditPropItem(pGroupItem,"sPartNo","","",-1,TRUE);			//������
		oper.InsertEditPropItem(pGroupItem,"sWeldPlatePartNo","","",-1,TRUE);	//������
		oper.InsertEditPropItem(pGroupItem,"sWrapPlatePartNo","","",-1,TRUE);	//������
	}
	//����
	oper.InsertCmbListPropItem(pGroupItem,"cMaterial","","","",-1,TRUE);
	if(m_iStdPartType==3)
	{	//�Ժ�����
		pPropItem=oper.InsertEditPropItem(pGroupItem,"m_iThickLevel","","",-1,TRUE);
		pPropItem->SetReadOnly();
		pPropItem->m_bHideChildren=TRUE;
		if(GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren=GetPropStatus(pPropItem->m_idProp);
		//A��
		CPropTreeItem* pSonPropItem=oper.InsertEditPropItem(pPropItem,"tA","","",-1,TRUE);
		oper.InsertEditPropItem(pSonPropItem,"wA","","",-1,TRUE);
		//B��
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"tB","","",-1,TRUE);
		oper.InsertEditPropItem(pSonPropItem,"wB","","",-1,TRUE);
		//C��
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"tC","","",-1,TRUE);
		oper.InsertEditPropItem(pSonPropItem,"wC","","",-1,TRUE);
		//D��
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"tD","","",-1,TRUE);
		oper.InsertEditPropItem(pSonPropItem,"wD","","",-1,TRUE);
		//E��
		pSonPropItem=oper.InsertEditPropItem(pPropItem,"tE","","",-1,TRUE);
		oper.InsertEditPropItem(pSonPropItem,"wE","","",-1,TRUE);
	}
	else
		oper.InsertBtnEditPropItem(pGroupItem,"theroy_weight","","",-1,TRUE);
	//���β���
	if(m_iStdPartType<3)
		pGroupItem=oper.InsertGrayButtonPropItem(pRoot,"Profile");
	else
		pGroupItem=oper.InsertPropItem(pRoot,"Profile");
	if(CTubeEndLjStdPartLibDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CTubeEndLjStdPartLibDlg::GetPropStatus(pGroupItem->m_idProp);
	if(m_iStdPartType>4)
	{	//���Է��������Է������β���
		oper.InsertEditPropItem(pGroupItem,"fInnerD","","",-1,TRUE);//�����ھ�
		oper.InsertEditPropItem(pGroupItem,"fOuterD","","",-1,TRUE);//�����⾶
		oper.InsertEditPropItem(pGroupItem,"fThick","","",-1,TRUE);	//�������
		oper.InsertEditPropItem(pGroupItem,"fH","","",-1,TRUE);		//�ֹܶ��뷨���������
	}
	else if(m_iStdPartType>2)
	{	//�������β���
		//�����⾶/�ֹ���ֱ��
		oper.InsertEditPropItem(pGroupItem,"A","�����⾶A","A=�����⾶/�ֹ���ֱ��",-1,TRUE);
		//�����ھ�
		pPropItem=oper.InsertEditPropItem(pGroupItem,"B","�����ھ�B","B=�����ھ�",-1,TRUE);
		if(m_iStdPartType==3)
		{	//�Ժ��������ھ��Ƕ������ɲ���
			pPropItem->SetReadOnly();
			pPropItem->m_lpNodeInfo->m_strPropHelp="�Ժ������ھ���ʵʱ����,��ʽ:B=A-2*S-2*tube_T";
		}
		//�����⾶
		oper.InsertEditPropItem(pGroupItem,"D","�����⾶D","D=�����⾶",-1,TRUE);
		//�������
		oper.InsertEditPropItem(pGroupItem,"C","�������C","C=�������",-1,TRUE);
		//�����߶�
		oper.InsertEditPropItem(pGroupItem,"H","�����߶�H","H=�����߶�",-1,TRUE);
		//��������ֱ��������R�ߴ�
		oper.InsertEditPropItem(pGroupItem,"N","����ֱ��N","N=��������ֱ��(����R�ߴ�)",-1,TRUE);
		//����������� //�Ժ�����ʱS��¼S��������ܺ�ȵ����� wht 10-01-20
		if(m_iStdPartType==3)	//�Ժ�����
			oper.InsertEditPropItem(pGroupItem,"S","S��ֵ","S��ֵ=�����������-���ܺ�ȵĲ�ֵ",-1,TRUE);
		else
			oper.InsertEditPropItem(pGroupItem,"S","�������S","S=�����������",-1,TRUE);
		//���������߶� ƽ�������˲����ֹܶ��뷨������ľ��� 
		if(m_iStdPartType==3)
			oper.InsertEditPropItem(pGroupItem,"H1","�����߶�H1","H1=���������߶�",-1,TRUE);
		else
			oper.InsertEditPropItem(pGroupItem,"H1","�˲����H1","H1=�ֹܶ��뷨������ľ���",-1,TRUE);
		//�����������ǰ뾶
		oper.InsertEditPropItem(pGroupItem,"R","�������ǰ뾶R","R=�����������ǰ뾶",-1,TRUE);
	}
	else//������β��� 
	{	
		oper.InsertEditPropItem(pGroupItem,"T","�����T","T=�����",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"L","��峤��L","L=��峤��",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"M","���ӳ���M","M=���ӳ���",-1,TRUE);
		//N
		if(m_iStdPartType==0||m_iStdPartType==1)	//U�Ͳ�� ���Ͳ��
			oper.InsertEditPropItem(pGroupItem,"N","�нǳ���N","N=�нǳ���",-1,TRUE);
		else	//ʮ�ֲ��
			oper.InsertEditPropItem(pGroupItem,"N","������N","N=������",-1,TRUE);
		//S
		if(m_iStdPartType==0||m_iStdPartType==1)	//U�Ͳ�� ���Ͳ��
			oper.InsertEditPropItem(pGroupItem,"S","�нǿ��S","S=�нǿ��",-1,TRUE);
		else	//ʮ�ֲ��
			oper.InsertEditPropItem(pGroupItem,"S","���峤��S","S=���峤��",-1,TRUE);
		//H
		oper.InsertEditPropItem(pGroupItem,"H","�����H","H=�����",-1,TRUE);
		//W
		if(m_iStdPartType==0) //��߲��
			oper.InsertEditPropItem(pGroupItem,"W","��߸߶�W","W=��߸߶�",-1,TRUE);
		else if(m_iStdPartType==1)	//U�Ͳ��
			oper.InsertEditPropItem(pGroupItem,"W","����������W","W=U�Ͳ�����������ľ���",-1,TRUE);
		else	//ʮ�ֲ��	
			oper.InsertEditPropItem(pGroupItem,"W","������W","W=ʮ�ֲ�������",-1,TRUE);
		if(m_iStdPartType==2)
		{	//ʮ�ֲ��
			oper.InsertEditPropItem(pGroupItem,"A","��λ����A","A=���嶨λ����,������ֹܶ�֮��ľ���",-1,TRUE);
			oper.InsertEditPropItem(pGroupItem,"R","��λ����R","R=���嶨λ����,���嵽�䴹ֱ����������֮��ľ���",-1,TRUE);
		}
	}
	//��˨���ò���
	if(m_iStdPartType<3)
		pGroupItem=oper.InsertGrayPopMenuItem(pRoot,"LayputBoltParam");
	else
		pGroupItem=oper.InsertPropItem(pRoot,"LayputBoltParam");
	if(CTubeEndLjStdPartLibDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
		pGroupItem->m_bHideChildren = CTubeEndLjStdPartLibDlg::GetPropStatus(pGroupItem->m_idProp);
	UpdateBoltLayParaProperty(pGroupItem);
	//
	if(m_iStdPartType==5||m_iStdPartType==6)
	{	//���Է����߰����
		pGroupItem=oper.InsertPropItem(pRoot,"RibPlate");
		pGroupItem->m_bHideChildren=(fl_param2.nRibPlateNum<=0);
		if(CTubeEndLjStdPartLibDlg::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CTubeEndLjStdPartLibDlg::GetPropStatus(pGroupItem->m_idProp);
		//
		oper.InsertEditPropItem(pGroupItem,"nRibPlateNum","","",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"ribPlateWeight","","",-1,TRUE);
		oper.InsertCmbListPropItem(pGroupItem,"cRibPlateMat","","","",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"T","�߰���","�߰���",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"A","A","�߰����β���A",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"B","B","�߰����β���B",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"C","C","�߰����β���C",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"D","D","�߰����β���D",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"E","E","�߰����β���E",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"F","F","�߰����β���F",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"G","G","�߰����β���G",-1,TRUE);
		oper.InsertEditPropItem(pGroupItem,"H","H","�߰����β���H",-1,TRUE);
	}
	m_propList.Redraw();
}
//ˢ�²�ͼ
void CTubeEndLjStdPartLibDlg::RefreshSketch(BOOL bDispRibPlateSketch)
{
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
	if(m_iStdPartType==0)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_ROLLEND_BOLT);
	else if(m_iStdPartType==1)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_UEND_BOLT);
	else if(m_iStdPartType==2)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_XEND_BOLT);
	else if(m_iStdPartType==3)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLD);
	else if(m_iStdPartType==4)
		m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLP);
	else if(m_iStdPartType==5||m_iStdPartType==6)
	{
		if(bDispRibPlateSketch)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ELBOW_PLATE1);
		else
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL2);
	}
	else
		return;
	//����ʾ��ͼ
	pPic->SetBitmap(m_CurPlateBmp);
}

void CTubeEndLjStdPartLibDlg::OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo(NULL,TRUE);
	if(pItemInfo==NULL)
		return;
	int iCurIndex=(int)pItemInfo->dwRefData;
	// ����ѡ�еĿ��Ƿ���ͬ���Ƿ���ø��°�ť
	BOOL bSameArr=FALSE;
	if(pItemInfo->itemType==TYPE_ROLLEND)	//��߲��							  
		bSameArr=CompareArr(rollend_param_table,g_orgPartLib.rollend_param_table.m_pData,rollend_param_N,g_orgPartLib.rollend_param_N,sizeof(INSERT_PLATE_PARAM));	
	if(pItemInfo->itemType==TYPE_UEND)		//U�Ͳ��
		bSameArr=CompareArr(uend_param_table,g_orgPartLib.uend_param_table.m_pData,uend_param_N,g_orgPartLib.uend_param_N,sizeof(INSERT_PLATE_PARAM));
	if(pItemInfo->itemType==TYPE_XEND)		//ʮ�ֲ��
		bSameArr=CompareArr(xend_param_table,g_orgPartLib.xend_param_table.m_pData,xend_param_N,g_orgPartLib.xend_param_N,sizeof(INSERT_PLATE_PARAM));
	if(pItemInfo->itemType==TYPE_FLG)		//���Է���
		bSameArr=CompareArr(flg_param_tbl,g_orgPartLib.flg_param_tbl.m_pData,flg_param_N,g_orgPartLib.flg_param_N,sizeof(FL_PARAM2));
	if(pItemInfo->itemType==TYPE_FLR)		//���Է���
		bSameArr=CompareArr(flr_param_tbl,g_orgPartLib.flr_param_tbl.m_pData,flr_param_N,g_orgPartLib.flr_param_N,sizeof(FL_PARAM2));
	if(pItemInfo->itemType==TYPE_FLD)		//�Ժ�����
		bSameArr=CompareArr(fld_param_table,g_orgPartLib.fld_param_table.m_pData,fld_param_N,g_orgPartLib.fld_param_N,sizeof(FL_PARAM))
			     &&CompareArr(fl_couple_rule_table,g_orgPartLib.fl_couple_rule_table.m_pData,fl_couple_rule_N,g_orgPartLib.fl_couple_rule_N,sizeof(FL_COUPLE_RULE));
	if(pItemInfo->itemType==TYPE_FLP)		//ƽ������  
		bSameArr=CompareArr(flp_param_table,g_orgPartLib.flp_param_table.m_pData,flp_param_N,g_orgPartLib.flp_param_N,sizeof(FL_PARAM))
		         &&CompareArr(fl_couple_rule_table,g_orgPartLib.fl_couple_rule_table.m_pData,fl_couple_rule_N,g_orgPartLib.fl_couple_rule_N,sizeof(FL_COUPLE_RULE));
	GetDlgItem(IDC_BTN_UPDATE_LIB)->EnableWindow(!bSameArr);
	if(iCurIndex==-1)
	{
		RefreshSketch(FALSE);
		Invalidate(FALSE);
	}
	else
		OnEditItem();
	
	*pResult = 0;
}

void CTubeEndLjStdPartLibDlg::OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	
	*pResult = 0;
}

void CTubeEndLjStdPartLibDlg::OnDblclkTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hItem;
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo(&hItem);
	if(pItemInfo==NULL)
		return;
	int iCurIndex=(int)pItemInfo->dwRefData;
	if(iCurIndex==-1)
	{	//������ǰ��������ֵܽڵ㴦���£״̬
		HTREEITEM hParentItem=m_treeCtrl.GetParentItem(hItem);
		if(hParentItem==NULL)
			return;
		HTREEITEM hSonItem=m_treeCtrl.GetChildItem(hParentItem);
		while(hSonItem)
		{
			if(hSonItem!=hItem)
				m_treeCtrl.Expand(hSonItem,TVE_COLLAPSE);
			hSonItem=m_treeCtrl.GetNextSiblingItem(hSonItem);
		}
		return;
	}
	else
		OnEditItem();
	*pResult = 0;
}

void CTubeEndLjStdPartLibDlg::ContextMenu(CWnd* pWnd, CPoint point) 
{
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo();
	if(pItemInfo==NULL)
		return;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	if(pItemInfo->itemType==TYPE_ROLLEND)
	{
		if((int)pItemInfo->dwRefData==-1)
		{
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"�½����Ͳ��");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"��ղ��Ͳ���");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"������Ͳ����ļ�");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"�������Ͳ����ļ�");
		}
		else
		{
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�༭���Ͳ��");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ�����Ͳ��");
			pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"��¡���½����Ͳ��");
		}
	}
	else if(pItemInfo->itemType==TYPE_UEND)
	{
		if((int)pItemInfo->dwRefData==-1)
		{
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"�½�U�Ͳ��");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"���U�Ͳ���");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"����U�Ͳ����ļ�");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"����U�Ͳ����ļ�");
		}
		else
		{
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�༭U�Ͳ��");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��U�Ͳ��");
			pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"��¡���½�U�Ͳ��");
		}
	}
	else if(pItemInfo->itemType==TYPE_XEND)
	{
		if((int)pItemInfo->dwRefData==-1)
		{
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"�½�ʮ�ֲ��");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"���ʮ���Ͳ���");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"����ʮ�ֲ����ļ�");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"����ʮ�ֲ����ļ�");
		}
		else
		{
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�༭ʮ�ֲ��");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��ʮ�ֲ��");
			pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"��¡���½�ʮ�ֲ��");
		}
	}
	else if(pItemInfo->itemType==TYPE_FLD)
	{
		if((int)pItemInfo->dwRefData==-1)
		{
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"�½��Ժ�����");
			pMenu->AppendMenu(MF_STRING,ID_FL_COUPLE_LEVEL_TABLE,"�����ԽӼ����");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"��նԺ�������");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"����Ժ��������ļ�");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"�����Ժ��������ļ�");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_EXCEL_FILE,"����Ժ��������");
		}
		else
		{
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�༭�Ժ�����");
			pMenu->AppendMenu(MF_STRING,ID_FL_COUPLE_LEVEL_TABLE,"�����ԽӼ����");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ���Ժ�����");
			pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"��¡���½��Ժ�����");
		}
	}
	else if(pItemInfo->itemType==TYPE_FLP)
	{
		if((int)pItemInfo->dwRefData==-1)
		{
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"�½�ƽ������");
			pMenu->AppendMenu(MF_STRING,ID_FL_COUPLE_LEVEL_TABLE,"�����ԽӼ����");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"���ƽ��������");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"����ƽ���������ļ�");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"����ƽ���������ļ�");
		}
		else
		{
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�༭ƽ������");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��ƽ������");
			pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"��¡���½�ƽ������");
		}
	}
	else if(pItemInfo->itemType==TYPE_FLG)
	{
		if((int)pItemInfo->dwRefData==-1)
		{
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"�½����Է���");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"��ո��Է�����");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"������Է������ļ�");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"�������Է������ļ�");
		}
		else
		{
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�༭���Է���");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ�����Է���");
			pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"��¡���½����Է���");
		}
	}
	else if(pItemInfo->itemType==TYPE_FLR)
	{
		if((int)pItemInfo->dwRefData==-1)
		{
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"�½����Է���");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"������Է�����");
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"�������Է������ļ�");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"�������Է������ļ�");
		}
		else
		{
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�༭���Է���");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ�����Է���");
			pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"��¡���½����Է���");
		}
	}
	else
		return;
	CPoint scr_point = point;
	m_treeCtrl.ClientToScreen(&scr_point);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}
void  CTubeEndLjStdPartLibDlg::OnBtnUpdateLib()
{
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo();
	if(pItemInfo==NULL||pItemInfo->itemType==TYPE_ROLLEND)
	{
		rollend_param_N =g_orgPartLib.rollend_param_N;
		memcpy((void*)rollend_param_table,g_orgPartLib.rollend_param_table.m_pData,sizeof(INSERT_PLATE_PARAM)*g_orgPartLib.rollend_param_N);
	}
	if(pItemInfo==NULL||pItemInfo->itemType==TYPE_UEND)
	{
		uend_param_N=g_orgPartLib.uend_param_N;
		memcpy((void*)uend_param_table,g_orgPartLib.uend_param_table.m_pData,sizeof(INSERT_PLATE_PARAM)*g_orgPartLib.uend_param_N);
	}
	if(pItemInfo==NULL||pItemInfo->itemType==TYPE_XEND)
	{
		xend_param_N=g_orgPartLib.xend_param_N;
		memcpy((void*)xend_param_table,g_orgPartLib.xend_param_table.m_pData,sizeof(INSERT_PLATE_PARAM)*g_orgPartLib.xend_param_N);
	}
	if(pItemInfo==NULL||pItemInfo->itemType==TYPE_FLG)
	{
		flg_param_N=g_orgPartLib.flg_param_N;
		memcpy((void*)flg_param_tbl,g_orgPartLib.flg_param_tbl.m_pData,sizeof(FL_PARAM2)*g_orgPartLib.flg_param_N);
	}
	if(pItemInfo==NULL||pItemInfo->itemType==TYPE_FLR)
	{
		flr_param_N=g_orgPartLib.flr_param_N;
		memcpy((void*)flr_param_tbl,g_orgPartLib.flr_param_tbl.m_pData,sizeof(FL_PARAM2)*g_orgPartLib.flr_param_N);
	}
	if (pItemInfo==NULL||pItemInfo->itemType==TYPE_FLD)
	{
		fld_param_N=g_orgPartLib.fld_param_N;
		memcpy((void*)fld_param_table,g_orgPartLib.fld_param_table.m_pData,sizeof(FL_PARAM)*g_orgPartLib.fld_param_N);
		fl_couple_rule_N = g_orgPartLib.fl_couple_rule_N;
		memcpy((void*)fl_couple_rule_table,g_orgPartLib.fl_couple_rule_table.m_pData,sizeof(FL_COUPLE_RULE)*g_orgPartLib.fl_couple_rule_N);
	}
	if(pItemInfo==NULL||pItemInfo->itemType==TYPE_FLP)
	{
		flp_param_N=g_orgPartLib.flp_param_N;
		memcpy((void*)flp_param_table,g_orgPartLib.flp_param_table.m_pData,sizeof(FL_PARAM)*g_orgPartLib.flp_param_N);
		fl_couple_rule_N = g_orgPartLib.fl_couple_rule_N;
		memcpy((void*)fl_couple_rule_table,g_orgPartLib.fl_couple_rule_table.m_pData,sizeof(FL_COUPLE_RULE)*g_orgPartLib.fl_couple_rule_N);
	}
	GetDlgItem(IDC_BTN_UPDATE_LIB)->EnableWindow(FALSE);
}
void CTubeEndLjStdPartLibDlg::OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	m_treeCtrl.ScreenToClient(&HitTestInfo.pt);
	m_treeCtrl.HitTest(&HitTestInfo);
	m_treeCtrl.Select(HitTestInfo.hItem,TVGN_CARET);
	ContextMenu(this,HitTestInfo.pt);
	*pResult = 0;
}

//��������ĵ�һ����˨��λ��,�Լ�����(��X��),��/�м�಼����˨
void CTubeEndLjStdPartLibDlg::RefreshBoltList()
{
	//ˢ�±�����
	m_listBolt.m_arrHeader.RemoveAll();
	if(m_iStdPartType>2)
	{	//
		m_listBolt.AddColumnHeader("ֱ��");
		m_listBolt.AddColumnHeader("R");
		m_listBolt.AddColumnHeader("�Ƕ�");
		long col_wide_arr[3]={70,100,100};
		m_listBolt.InitListCtrl(col_wide_arr);
	}
	else
	{
		m_listBolt.AddColumnHeader("ֱ��");
		m_listBolt.AddColumnHeader("X");
		m_listBolt.AddColumnHeader("Y");
		m_listBolt.AddColumnHeader("Z");
		long col_wide_arr[4]={70,70,70,60};
		m_listBolt.InitListCtrl(col_wide_arr);
	}
	//
	CStringArray str_arr;
	m_listBolt.DeleteAllItems();//�����˨�б�
	if(m_iStdPartType>2)
	{	//��������˨
		str_arr.SetSize(3);
		int nBoltNum=0,nBoltD=0;
		double piece_angle=0,fBoltCirR=0;
		if(m_iStdPartType==3||m_iStdPartType==4)
		{
			nBoltNum=fl_param.nBoltNum;
			nBoltD=fl_param.nBoltD;
			fBoltCirR=fl_param.K*0.5;
		}
		else 
		{
			nBoltNum=fl_param2.nBoltNum;
			nBoltD=fl_param2.nBoltD;
			fBoltCirR=fl_param2.fBoltCirD*0.5;
		}
		piece_angle=2*Pi/nBoltNum*DEGTORAD_COEF;
		for(int i=0;i<nBoltNum;i++)
		{
			double angle=i*piece_angle;
			str_arr[0].Format("M%d",nBoltD);
			str_arr[1].Format("%f",fBoltCirR);
			SimplifiedNumString(str_arr[1]);
			str_arr[2].Format("%f",angle);
			SimplifiedNumString(str_arr[2]);
			int iItem=m_listBolt.InsertItemRecord(-1,str_arr);
		}
	}
	else if(m_iStdPartType<3)
	{
		CLDSStdPartModel stdPart;
		if(m_iStdPartType==0)
			stdPart.m_iStdPartType=TYPE_ROLLEND;
		else if(m_iStdPartType==1)
			stdPart.m_iStdPartType=TYPE_UEND;
		else 
			stdPart.m_iStdPartType=TYPE_XEND;
		UpdateLsParas(FALSE);
		CopyInsertPlateParam(stdPart.param.insert_plate_param,insert_plate_param);
		stdPart.LayoutBolt();	//������˨
		//
		f3dPoint *pPt=NULL;
		CStringArray str_arr;
		str_arr.SetSize(4);
		int nBoltNum=0;
		for(pPt=stdPart.GetFirstBoltPos();pPt;pPt=stdPart.GetNextBoltPos())
		{
			nBoltNum++;
			//������˨���ж���˨���ڵڼ������
			int nFlags=1,nSumBolt=0,index=0;
			for(index=0;index<insert_plate_param.iBoltRect;index++)
			{
				if(m_iStdPartType==2)	//ʮ�ֲ��
					nSumBolt+=insert_plate_param.ls_param[index].iBoltNum*4;
				else 
					nSumBolt+=insert_plate_param.ls_param[index].iBoltNum;
				if(nBoltNum<=nSumBolt)
				{	//��1��ʼ,��Ϊ0û������,�˴���Ҫʹ��������,��ʾ�ڼ�����˨
					nFlags=index+1;
					break;
				}
			}
			//
			str_arr[0].Format("M%d",insert_plate_param.ls_param[index].iBoltD);
			str_arr[1].Format("%f",pPt->x);
			SimplifiedNumString(str_arr[1]);
			str_arr[2].Format("%f",pPt->y);
			SimplifiedNumString(str_arr[2]);
			str_arr[3].Format("%f",pPt->z);
			SimplifiedNumString(str_arr[3]);
			int iItem=m_listBolt.InsertItemRecord(-1,str_arr);
			if(m_iStdPartType==2)
			{	//ʮ�ֲ��
				if(pPt->feature==31||pPt->feature==41)
					m_listBolt.SetItemData(iItem,-1*nFlags);	//X-Z���ϵ���˨
				else 
					m_listBolt.SetItemData(iItem,nFlags);	//X-Y���ϵ���˨
			}
			else //���Ͳ�壬U�Ͳ��
				m_listBolt.SetItemData(iItem,nFlags);		//��˨λ�ñ�־
		}
	}
}
typedef struct tagBoltColParam
{
	double fCoordX;
	double CoordY[20];
	int nBoltD;
	int nCount;
	tagBoltColParam(){memset(this,0,sizeof(tagBoltColParam));}
}BOLT_COL_PARAM;
//
static int CompareBoltColPara(const BOLT_COL_PARAM &colParam1,const BOLT_COL_PARAM &colParam2)
{	//�Ȱ���˨�������� Ȼ����˨X�����С����
	if(colParam1.nCount>colParam2.nCount)
		return -1;
	else if(colParam1.nCount<colParam2.nCount)
		return 1;
	else if(colParam1.fCoordX>colParam2.fCoordX)
		return -1;
	else if(colParam1.fCoordX<colParam2.fCoordX)
		return 1;
	else 
		return 0;
}
//������˨�б�����˨���ò��������ܴ��������ֶ�����
void CTubeEndLjStdPartLibDlg::GetParamPlateLayoutBoltParam(CLDSParamPlate *pParamPlate)
{
	if(pParamPlate==NULL||m_iStdPartType>2)
		return;
	if(!((m_iStdPartType==0&&pParamPlate->m_iParamType==TYPE_ROLLEND)
		||(m_iStdPartType==1&&pParamPlate->m_iParamType==TYPE_UEND)
		||(m_iStdPartType==2&&pParamPlate->m_iParamType==TYPE_XEND)))
		return;
	int i=0,j=0,nColNum=0;
	BOLT_COL_PARAM colParamArr[20];
	CLsRef *pLsRef=NULL;
	for(pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{
		CLDSBolt *pBolt=pLsRef->GetLsPtr();
		if(fabs(pBolt->ucs.axis_z*pParamPlate->ucs.axis_z)<EPS_COS2)
			continue;	//��˨�������巨�߲�һ��
		f3dPoint pos=pBolt->ucs.origin;
		coord_trans(pos,pParamPlate->ucs,FALSE);
		char sText[100]="";
		if(m_iStdPartType==2)
		{	//ʮ�ֲ��
			if(pos.y<=0)	//��ȡX-Yƽ��Y�����������˨
				continue;
		}
		int nBoltD=pBolt->get_d();
		for(j=0;j<nColNum;j++)
		{
			if(fabs(pos.x-colParamArr[j].fCoordX)<EPS)
			{	//Y���갴��С����˳������
				for(int m=colParamArr[j].nCount;m>=0;m--)
				{
					if(pos.y>colParamArr[j].CoordY[m-1])
					{
						colParamArr[j].CoordY[m]=pos.y;
						break;
					}
					else if(m>0)
						colParamArr[j].CoordY[m]=colParamArr[j].CoordY[m-1];
					else 
						colParamArr[j].CoordY[m]=pos.y;
				}
				colParamArr[j].nCount++;
				break;
			}
		}
		if(j==nColNum)
		{
			colParamArr[j].fCoordX=pos.x;
			colParamArr[j].CoordY[0]=pos.y;
			colParamArr[j].nCount=1;
			colParamArr[j].nBoltD=nBoltD;
			nColNum++;
		}
	}
	CHeapSort<BOLT_COL_PARAM>::HeapSort(colParamArr,nColNum,CompareBoltColPara);
	int iBoltRect=0;	//�������
	for(i=0;i<nColNum;i++)
	{	//
		double fHorzSpace=0,fVertSpace=0,fMinBoltX=10000,fMinBoltY=10000;
		//�������½���˨
		fMinBoltX=colParamArr[i].fCoordX;
		for(int k=0;k<colParamArr[i].nCount;k++)
		{
			if(fMinBoltY>colParamArr[i].CoordY[k])
				fMinBoltY=colParamArr[i].CoordY[k];
			if(k==1)
				fVertSpace=colParamArr[i].CoordY[1]-colParamArr[i].CoordY[0];
		}
		for(j=i+1;j<nColNum;j++)
		{	
			if(colParamArr[i].nCount!=colParamArr[j].nCount)
				break;
			double fNewHorzSpace=fabs(colParamArr[j].fCoordX-colParamArr[j-1].fCoordX);
			if(j==(i+1)) //�����о���
				fHorzSpace=fabs(colParamArr[i+1].fCoordX-colParamArr[i].fCoordX);
			else if(fabs(fNewHorzSpace-fHorzSpace)>EPS)
				break;	//��ǰ�о���ǰһ���о಻һ�£��޷�һ���� wht 10-11-10
			int k=0;
			for(k=0;k<colParamArr[i].nCount;k++)
			{	//�ж�����������˨Y�����Ƿ���ȫһ��
				if(fabs(colParamArr[i].CoordY[k]-colParamArr[j].CoordY[k])>EPS2)
					break;
			}
			if(k<colParamArr[i].nCount)
				break;	//Y���겻��ȫƥ�䣬�޷�һ����
			if(colParamArr[j].fCoordX<fMinBoltX)
				fMinBoltX=colParamArr[j].fCoordX;
		}
		//���һ����˨�����ò��� 
		double offset_x=0,offset_y=0;
		if(m_iStdPartType==2)	
		{	//Ӧ�õ���ʮ�ֲ������˨λ�õ���������ϵ�� wht 10-11-10
			offset_x=insert_plate_param.M+insert_plate_param.A;
			offset_y=insert_plate_param.R;
		}
		insert_plate_param.ls_param[iBoltRect].fBoltPosX=fMinBoltX-offset_x;
		insert_plate_param.ls_param[iBoltRect].fBoltPosY=fMinBoltY-offset_y; 
		insert_plate_param.ls_param[iBoltRect].fHorzSpace=fHorzSpace;
		insert_plate_param.ls_param[iBoltRect].fVertSpace=fVertSpace;
		insert_plate_param.ls_param[iBoltRect].iRow=colParamArr[i].nCount;
		insert_plate_param.ls_param[iBoltRect].iColumn=j-i;
		insert_plate_param.ls_param[iBoltRect].iBoltNum=colParamArr[i].nCount*(j-i);
		insert_plate_param.ls_param[iBoltRect].iBoltD=colParamArr[i].nBoltD;
		iBoltRect++;
		insert_plate_param.iBoltRect=iBoltRect;
		i+=(j-i-1);
	}
	//������˨���ò����б�
	UpdateLsParas();
	RefreshBoltLayoutList();
	//������˨����������˨
	RefreshBoltList();
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

void CTubeEndLjStdPartLibDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rectDrawRgn; //��ǰ��ͼ����
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&rectDrawRgn);
	ScreenToClient(&rectDrawRgn);
	int width=rectDrawRgn.Width()-10;
	int height=rectDrawRgn.Height()-10;
	//
	double D=0,B=0,W=0,L=0,H=0,N=0,S=0,R=0,M=0,T=0,A=0,d=0,t=0;
	double real_width=0,real_height=0,scale=1;
	double fCirD=0;
	int nBoltNum=0,nBoltD=0,nRibPlateNum=0;
	UCS_2dSTRU draw_ucs;	//��ͼ����ϵ
	//
	if(m_iStdPartType==3||m_iStdPartType==4)
	{	//����
		D=fl_param.D;
		B=fl_param.B;
		if(m_iStdPartType==3)	//�Ժ�����B�Ƕ������ɲ���
			B=fl_param.A-(2*fl_param.S+2*fl_param.group[0].t);
		R=0.5*B;
		T=fl_param.C;
		d=fl_param.d;
		t=fl_param.t;
		fCirD=fl_param.K;
		nBoltD=fl_param.nBoltD;
		nBoltNum=fl_param.nBoltNum;
		real_width=real_height=D+10;
		draw_ucs.origin.Set(rectDrawRgn.TopLeft().x+width*0.5,rectDrawRgn.TopLeft().y+height*0.5);
	}
	else if(m_iStdPartType>4)
	{
		D=fl_param2.fOuterD;
		B=fl_param2.fInnerD;
		R=0.5*B;
		T=fl_param2.fThick;
		d=fl_param2.d;
		t=1;
		fCirD=fl_param2.fBoltCirD;
		nBoltD=fl_param2.nBoltD;
		nBoltNum=fl_param2.nBoltNum;
		nRibPlateNum=fl_param2.nRibPlateNum;
		real_height=real_width=D+10;
		draw_ucs.origin.Set(rectDrawRgn.TopLeft().x+width*0.5,rectDrawRgn.TopLeft().y+height*0.5);
	}
	else 
	{	//���
		L=insert_plate_param.L;
		H=insert_plate_param.H;
		M=insert_plate_param.M;
		S=insert_plate_param.S;
		N=insert_plate_param.N;
		W=insert_plate_param.W;
		A=insert_plate_param.A;
		R=insert_plate_param.R;
		T=insert_plate_param.Thick;
		d=insert_plate_param.d;
		t=insert_plate_param.t;
		double offset_dist_x=0,offset_dist_y=0.5*height;
		if(m_iStdPartType==1)
		{	//U�Ͱ�
			real_width=L+3*W;
			real_height=H+8*W;
			offset_dist_x=((2*W)/(L+3*W))*width;
			offset_dist_y-=(W/(H+8*W))*height;
		}
		else if(m_iStdPartType==2)
		{	//ʮ�ֲ��
			real_width=M+S+5*N;
			real_height=H+3*N;
			offset_dist_x=(N/(M+S+5*N))*width;
			offset_dist_y+=(N/(H+3*N))*height;
		}
		else //if(m_iStdPartType==0)
		{	//��߰�
			real_width=L+4*W;
			real_height=H+2*W;
			offset_dist_x=(W/(L+4*W))*width;
		}
		CSize sizeText=dc.GetTextExtent("1234");
		offset_dist_x=max(offset_dist_x,sizeText.cx); //��֤����ע�����ڻ�ͼ������
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
	f2dPoint origin=draw_ucs.origin;
	int axis_y_len=ftoi(0.3*height);
	int axis_x_len=axis_y_len;
	//��������ϵX��Y�᳤��ȡ0.8�뾶 ��ΪҪ���ƻ�׼���Լ������߱��
	if(m_iStdPartType>2) 
		axis_x_len=axis_y_len=ftoi(D*0.5*scale)+3;
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
	//
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo();
	if(pItemInfo==NULL||((int)pItemInfo->dwRefData)<0)
		return;		//û��ѡ�о����׼����
	POINT topLeft,bottomRight;
	if(m_iStdPartType>2)
	{	//����
		double fTubeR=R*scale-1;
		double fTubeThick=min(t*scale,8);
		for(int i=0;i<4;i++)
		{
			int nRadius=ftoi(0.5*D*scale);
			if(i==1)
				nRadius=ftoi(R*scale);
			else if(i==2)
			{
				pOldPen=dc.SelectObject(&green_dash_pen);
				nRadius=ftoi(fCirD*0.5*scale);
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
		double tube_r=d*0.5;
		for(i=0;i<3;i++)
		{
			f2dPoint dim_start,dim_end;
			double cur_r=fCirD*0.5;
			if(i==1)
				cur_r=R;
			else if(i==2)
				cur_r=D*0.5;
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
		//������˨
		pOldPen=dc.SelectObject(&red_solid_pen);
		f3dPoint startPoint(fCirD*0.5*scale,0,0);
		double sector_angle=(2*Pi)/nBoltNum;
		int nBoltR=ftoi(nBoltD*0.5*scale);
		//
		int iCurSel=-1;
		POSITION pos = m_listBolt.GetFirstSelectedItemPosition();
		if(pos!=NULL)
			iCurSel = m_listBolt.GetNextSelectedItem(pos);
		for(i=0;i<nBoltNum;i++)
		{
			f3dPoint tempPoint=startPoint;
			rotate_point_around_axis(tempPoint,i*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,100));
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
		//�����߰�
		if(m_iStdPartType>4)
		{
			if(nRibPlateNum>0)
			{	
				double sector_angle=(2*Pi)/nRibPlateNum;
				double R=fl_param2.fInnerD*0.5;
				double start_angle=0.5*(2*Pi)/fl_param2.nBoltNum;
				f3dPoint startPt(R*scale,0,0);
				f3dPoint endPt((R+fl_param2.para.B)*scale,0,0);
				double fPlateThick=fl_param2.para.T*scale;
				for(i=0;i<nRibPlateNum;i++)
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
	}
	else 
	{	//���Ͳ�塢U�Ͳ�塢ʮ�ֲ��
		int i=0;
		f2dPoint dim_start,dim_end;	
		double fTubeR=d*0.5;
		double fTubeThick=max(t,8);
		f2dPoint vertex_arr[8];
		double fCutAngleLen=N,fCutAngleWidth=S;
		if(m_iStdPartType==2)
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
		//���ƹ�������ͼ
		for(i=0;i<6;i++)
		{
			f2dPoint start=vertex_arr[i];
			f2dPoint end=vertex_arr[(i+1)%6];
			dc.MoveTo(ftoi(start.x),ftoi(start.y));
			dc.LineTo(ftoi(end.x),ftoi(end.y));
			if(m_iStdPartType==0&&(i==1||i==5))
			{	//���ƾ��
				int nFlag=1;
				if(i==1)
					nFlag=-1;
				pOldPen=dc.SelectObject(&dash_pen);
				dc.MoveTo(ftoi(start.x),ftoi(start.y+nFlag*T*scale));
				dc.LineTo(ftoi(end.x),ftoi(end.y+nFlag*T*scale));
				dc.SelectObject(pOldPen);
			}
		}
		//��ע���������߳ߴ�
		pOldPen=dc.SelectObject(&purple_solid_pen);
		if(m_iStdPartType<3)
		{	//H
			if(m_iStdPartType==2)	//ʮ�ֲ��
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
			if(m_iStdPartType==0||m_iStdPartType==1)
			{	//N
				dim_start.Set(vertex_arr[2].x,vertex_arr[2].y);
				dim_end.Set(vertex_arr[2].x+N*scale,vertex_arr[2].y);
				DimText(&dc,dim_start,dim_end,N,2);
				//S
				dim_start.Set(vertex_arr[5].x,vertex_arr[5].y);
				dim_end.Set(vertex_arr[5].x,vertex_arr[5].y+S*scale);
				DimText(&dc,dim_start,dim_end,S,-1);
			}
			else //if(m_iStdPartType==3)
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
		if(m_iStdPartType==2)
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
		
		if(m_iStdPartType==1)
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
		else if(m_iStdPartType==0) 
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
			long h=(long)m_listBolt.GetItemData(i);	//��˨��־λ
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
			if(m_iStdPartType==2&&h<0)
				continue;	//������ʮ�ֲ��XY����˨
			f2dPoint curPoint(pos.x*scale,pos.y*scale);
			if(m_iStdPartType==2&&pos.y>0)
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

//���±�׼����״̬
void CTubeEndLjStdPartLibDlg::UpdateStdPartState()
{
	m_bModify=TRUE;	//���ڱ༭״̬
	GetDlgItem(IDC_BTN_CANCEL_MODIFY)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SAVE_DATA)->EnableWindow(TRUE);
	CXhChar100 sPartType;
	if(m_bNewStdPart)
		sPartType.Copy("�½�");
	if(m_iStdPartType==0)
		m_sOperPrompt.Format("%s���Ͳ��%s�����ѵ���...",(char*)sPartType,insert_plate_param.codeName);
	else if(m_iStdPartType==1)
		m_sOperPrompt.Format("%sU�Ͳ��%s�����ѵ���...",(char*)sPartType,insert_plate_param.codeName);
	else if(m_iStdPartType==2)
		m_sOperPrompt.Format("%sʮ�ֲ��%s�����ѵ���...",(char*)sPartType,insert_plate_param.codeName);
	else if(m_iStdPartType==3)
		m_sOperPrompt.Format("%s�Ժ�����%s�����ѵ���...",(char*)sPartType,fl_param.codeName);
	else if(m_iStdPartType==4)
		m_sOperPrompt.Format("%s�Ժ�����%s�����ѵ���...",(char*)sPartType,fl_param.codeName);
	else if(m_iStdPartType==5)
		m_sOperPrompt.Format("%s���Է���%s�����ѵ���...",(char*)sPartType,fl_param2.codeName);
	else if(m_iStdPartType==6)
		m_sOperPrompt.Format("%s���Է���%s�����ѵ���...",(char*)sPartType,fl_param2.codeName);
	UpdateData(FALSE);
}
void CTubeEndLjStdPartLibDlg::Redraw()
{
	//Invalidate();
	CRect rect;
	GetDlgItem(IDC_S_MAP)->GetWindowRect(&rect);
	rect.top-=100;
	InvalidateRect(rect,FALSE);
}
//����Ժ�������EXCEL���
//��ȡExcel����¼��Ժ���������
static CXhChar100 VariantToString(VARIANT value)
{
	CXhChar100 sValue;
	if(value.vt==VT_BSTR)
		return sValue.Copy(CString(value.bstrVal));
	else if(value.vt==VT_R8)
		return sValue.Copy(CXhChar100(value.dblVal));
	else if(value.vt==VT_R4)
		return sValue.Copy(CXhChar100(value.fltVal));
	else if(value.vt==VT_INT)
		return sValue.Copy(CXhChar100(value.intVal));
	else 
		return sValue;
}
void CTubeEndLjStdPartLibDlg::OnImportStdPartLibExcelFile()
{
	if(m_iStdPartType!=3)
		return;
	CFileDialog fileDlg(TRUE,"xls","�Ժ�������",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"�Ժ�����(*.xls)|*.xls|All Files(*.*)|*.*||");
	if(fileDlg.DoModal()!=IDOK)
		return;
	CVariant2dArray sheetContentMap(1,1);
	if(CExcelOper::GetExcelContentOfSpecifySheet(fileDlg.GetPathName(),sheetContentMap,1)==FALSE)
		return;
	//���ԭ�з�����¼
	for(int i=0;i<500;i++)
		memset(&fld_param_table[i],0,sizeof(FL_PARAM));
	//����������¼����(����������)
	fld_param_N=0;
	for(int i=1;i<sheetContentMap.RowsCount();i++)
	{
		VARIANT value;
		sheetContentMap.GetValueAt(i,0,value);
		if(value.vt==VT_EMPTY||atof(VariantToString(value))<=0)
			continue;
		if(i>499)
		{
			AfxMessageBox("�Ժ�������¼����������500");
			break;
		}
		//���ܹܾ�
		fld_param_table[fld_param_N].d=atof(VariantToString(value));
		//�Խӹܾ�
		sheetContentMap.GetValueAt(i,1,value);
		fld_param_table[fld_param_N].couple_d=atof(VariantToString(value));
		//�ֹܺ��
		sheetContentMap.GetValueAt(i,2,value);
		fld_param_table[fld_param_N].t=atof(VariantToString(value));
		//����
		sheetContentMap.GetValueAt(i,3,value);
		fld_param_table[fld_param_N].cMaterial=VariantToString(value)[0];
		//�������
		sheetContentMap.GetValueAt(i,4,value);
		strcpy(fld_param_table[fld_param_N].codeName,VariantToString(value));
		//��������
		sheetContentMap.GetValueAt(i,5,value);
		fld_param_table[fld_param_N].cLevelMark=VariantToString(value)[0];
		//���Ӷ��⾶A
		sheetContentMap.GetValueAt(i,6,value);
		fld_param_table[fld_param_N].A=atof(VariantToString(value));
		//�ڱ�B
		sheetContentMap.GetValueAt(i,7,value);
		fld_param_table[fld_param_N].B=atof(VariantToString(value));
		//�����⾶D
		sheetContentMap.GetValueAt(i,8,value);
		fld_param_table[fld_param_N].D=atof(VariantToString(value));
		//��˨��λֱ��K
		sheetContentMap.GetValueAt(i,9,value);
		fld_param_table[fld_param_N].K=atof(VariantToString(value));
		//�׾�L
		sheetContentMap.GetValueAt(i,10,value);
		fld_param_table[fld_param_N].L=atof(VariantToString(value));
		//��˨����n
		sheetContentMap.GetValueAt(i,11,value);
		fld_param_table[fld_param_N].nBoltNum=atoi(VariantToString(value));
		//��˨���
		sheetContentMap.GetValueAt(i,12,value);
		fld_param_table[fld_param_N].nBoltD=atoi(VariantToString(value));
		//�������C
		sheetContentMap.GetValueAt(i,13,value);
		fld_param_table[fld_param_N].C=atof(VariantToString(value));
		//�����߶�H
		sheetContentMap.GetValueAt(i,14,value);
		fld_param_table[fld_param_N].H=atof(VariantToString(value));
		//����ֱ��N
		sheetContentMap.GetValueAt(i,15,value);
		fld_param_table[fld_param_N].N=atof(VariantToString(value));
		//��ֵS
		sheetContentMap.GetValueAt(i,16,value);
		fld_param_table[fld_param_N].S=atof(VariantToString(value));
		//���ǰ뾶R
		sheetContentMap.GetValueAt(i,17,value);
		fld_param_table[fld_param_N].R=atof(VariantToString(value));
		//�����߶�H1
		sheetContentMap.GetValueAt(i,18,value);
		fld_param_table[fld_param_N].H1=atof(VariantToString(value));
		//��Ӧ���ܱں�����
		int nGroup=0;
		//A
		sheetContentMap.GetValueAt(i,19,value);
		fld_param_table[fld_param_N].group[0].t=atoi(VariantToString(value));
		if(fld_param_table[fld_param_N].group[0].t>0)
			nGroup++;
		sheetContentMap.GetValueAt(i,20,value);
		fld_param_table[fld_param_N].group[0].weight=atof(VariantToString(value));
		//B
		sheetContentMap.GetValueAt(i,21,value);
		fld_param_table[fld_param_N].group[1].t=atoi(VariantToString(value));
		if(fld_param_table[fld_param_N].group[1].t>0)
			nGroup++;
		sheetContentMap.GetValueAt(i,22,value);
		fld_param_table[fld_param_N].group[1].weight=atof(VariantToString(value));
		//C
		sheetContentMap.GetValueAt(i,23,value);
		fld_param_table[fld_param_N].group[2].t=atoi(VariantToString(value));
		sheetContentMap.GetValueAt(i,24,value);
		fld_param_table[fld_param_N].group[2].weight=atof(VariantToString(value));
		//D
		sheetContentMap.GetValueAt(i,25,value);
		fld_param_table[fld_param_N].group[3].t=atoi(VariantToString(value));
		sheetContentMap.GetValueAt(i,26,value);
		fld_param_table[fld_param_N].group[3].weight=atof(VariantToString(value));
		//E
		sheetContentMap.GetValueAt(i,27,value);
		fld_param_table[fld_param_N].group[4].t=atoi(VariantToString(value));
		sheetContentMap.GetValueAt(i,28,value);
		fld_param_table[fld_param_N].group[4].weight=atof(VariantToString(value));
		//���¶Ժ������Ĵ��ź��ڱ�B
		fld_param_table[fld_param_N].t=fld_param_table[fld_param_N].group[0].t;
		fld_param_table[fld_param_N].B=fld_param_table[fld_param_N].d-(fld_param_table[fld_param_N].S+fld_param_table[fld_param_N].t)*2;
		sprintf(fld_param_table[fld_param_N].codeName,"%s?%c%c",fld_param_table[fld_param_N].codeName,fld_param_table[fld_param_N].cLevelMark,fld_param_table[fld_param_N].cMaterial);
		//
		fld_param_N++;	
	}
	//�����Ժ���Ҫˢ���б����� 
	RefreshTreeCtrl();
}

void CTubeEndLjStdPartLibDlg::OnImportStdPartLibFile() 
{
	CString sFileName;
	char *sName="cipl";
	char *sFileType="���Ͳ����ļ�(*.cipl)|*.cipl||";
	if(m_iStdPartType==1)
	{
		sName="uipl";
		sFileType="U�Ͳ����ļ�(*.uipl)|*.uipl||";
	}
	else if(m_iStdPartType==2)
	{
		sName="xipl";
		sFileType="ʮ���Ͳ����ļ�(*.xipl)|*.xipl||";
	}
	else if(m_iStdPartType==3)
	{
		sName="fld";
		sFileType="�Ժ��������ļ�(*.fld)|*.fld||";
	}
	else if(m_iStdPartType==4)
	{
		sName="flp";
		sFileType="ƽ�����������ļ�(*.flp)|*.flp||";
	}
	else if(m_iStdPartType==5)
	{
		sName="flg";
		sFileType="���Է��������ļ�(*.flg)|*.flg||";
	}
	else if(m_iStdPartType==6)
	{
		sName="flr";
		sFileType="���Է��������ļ�(*.flr)|*.flr||";
	}
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		if(m_iStdPartType==1)
			UInsertPlateSerialize(sFileName,FALSE);
		else if(m_iStdPartType==2)
			XInsertPlateSerialize(sFileName,FALSE);
		else if(m_iStdPartType==3)
			FlDSerialize(sFileName,FALSE);
		else if(m_iStdPartType==4)
			FlPSerialize(sFileName,FALSE);
		else if(m_iStdPartType==5)
			FlGSerialize(sFileName,FALSE);
		else if(m_iStdPartType==6)
			FlRSerialize(sFileName,FALSE);
		else //if(m_iStdPartType==0)
			CInsertPlateSerialize(sFileName,FALSE);
		//�����Ժ���Ҫˢ���б����� 
		RefreshTreeCtrl();
	}
}

void CTubeEndLjStdPartLibDlg::OnExportStdPartLibFile() 
{
	CString sFileName;
	char *sName="cipl";
	char *sFileType="���Ͳ����ļ�(*.cipl)|*.cipl||";
	if(m_iStdPartType==1)
	{
		sName="uipl";
		sFileType="U�Ͳ����ļ�(*.uipl)|*.uipl||";
	}
	else if(m_iStdPartType==2)
	{
		sName="xipl";
		sFileType="ʮ���Ͳ����ļ�(*.xipl)|*.xipl||";
	}
	else if(m_iStdPartType==3)
	{
		sName="fld";
		sFileType="�Ժ��������ļ�(*.fld)|*.fld||";
	}
	else if(m_iStdPartType==4)
	{
		sName="flp";
		sFileType="ƽ�����������ļ�(*.flp)|*.flp||";
	}
	else if(m_iStdPartType==5)
	{
		sName="flg";
		sFileType="���Է��������ļ�(*.flg)|*.flg||";
	}
	else if(m_iStdPartType==6)
	{
		sName="flr";
		sFileType="���Է��������ļ�(*.flr)|*.flr||";
	}
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		if(m_iStdPartType==1)
			UInsertPlateSerialize(sFileName,TRUE);
		else if(m_iStdPartType==2)
			XInsertPlateSerialize(sFileName,TRUE);
		else if(m_iStdPartType==3)
			FlDSerialize(sFileName,TRUE);
		else if(m_iStdPartType==4)
			FlPSerialize(sFileName,TRUE);
		else if(m_iStdPartType==5)
			FlGSerialize(sFileName,TRUE);
		else if(m_iStdPartType==6)
			FlRSerialize(sFileName,TRUE);
		else //if(m_iStdPartType==0)
			CInsertPlateSerialize(sFileName,TRUE);
	}
}

//����������Ƿ�Ϸ���Ŀǰ��Ҫ���߾� wht 11-04-16
static BOOL CheckPlateLsEdgeSpace(INSERT_PLATE_PARAM insert_plate_param,PARAM_PLATE_TYPE iParamType)
{
	if(insert_plate_param.iBoltRect<0)
		return TRUE;
	CLDSParamPlate paramPlate;
	paramPlate.SetBelongModel(console.GetActiveModel());
	paramPlate.m_iParamType=iParamType;
	//strcpy(paramPlate.sPartNo,insert_plate_param.codeName);
	//paramPlate.m_bStdPart=TRUE;
	//�˴�Ӧ�������β������ɷǱ�׼��壬����ݱ�����ɲ����������Ϣ���ܲ������µ� wht 11-06-08
	if(iParamType==TYPE_UEND)
	{	//U�Ͷ˰�
		paramPlate.SetDesignItemValue('W',insert_plate_param.W,"U�Ͳ����������ļ��",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('L',insert_plate_param.L,"�峤",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('H',insert_plate_param.H,"���",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('M',insert_plate_param.M,"U�Ͳ����ֹ��ཻ�ĳ���",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('N',insert_plate_param.N,"�нǳ���",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('S',insert_plate_param.S,"�нǿ��",PARAM_SHAPE);
	}
	else if(iParamType==TYPE_ROLLEND)
	{	//���Ͳ��
		paramPlate.SetDesignItemValue('W',insert_plate_param.W,"��ߵĸ߶�",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('L',insert_plate_param.L,"�峤",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('H',insert_plate_param.H,"���",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('M',insert_plate_param.M,"����ֹ��ཻ�ĳ���",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('N',insert_plate_param.N,"�нǳ���",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('S',insert_plate_param.S,"�нǿ��",PARAM_SHAPE);
	}
	else if(iParamType==TYPE_XEND)
	{	//X�Ͳ��
		paramPlate.SetDesignItemValue('L',insert_plate_param.L,"�峤",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('H',insert_plate_param.H,"���",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('M',insert_plate_param.M,"����ֹ��ཻ�ĳ���",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('W',insert_plate_param.W,"ʮ���Ͳ���ϸ��Ӱ�ĺ��",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('N',insert_plate_param.N,"ʮ���Ͳ���ϸ��Ӱ�Ŀ��",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('S',insert_plate_param.S,"ʮ���Ͳ���ϸ��Ӱ�ĳ���",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('A',insert_plate_param.A,"ʮ���Ͳ���ϸ��Ӱ�Ķ�λ����A",PARAM_SHAPE);
		paramPlate.SetDesignItemValue('R',insert_plate_param.R,"ʮ���Ͳ���ϸ��Ӱ�Ķ�λ����R",PARAM_SHAPE);
	}
	paramPlate.DesignPlate(false);
	if(paramPlate.vertex_list.GetNodeNum()<3)
		return TRUE;
	if(iParamType!=TYPE_ROLLEND&&iParamType!=TYPE_XEND&&iParamType!=TYPE_UEND)
		return TRUE;
	int i=0,n=paramPlate.vertex_list.GetNodeNum();
	f3dPoint *pnt_arr=new f3dPoint[n];
	for(PROFILE_VERTEX *pVertex=paramPlate.vertex_list.GetFirst();pVertex;pVertex=paramPlate.vertex_list.GetNext())
	{
		pnt_arr[i]=pVertex->vertex;
		i++;
	}
	double epsilon=1.0;
	fPolygonRgn rgn;
	f3dPoint ls_pos,perp;
	double dist=0,spaceLim=0;
	fPtList boltPosList[2];	//0.ʮ�ֲ��Y�����������˨ 1.ʮ�ֲ��Y�Ḻ�������˨
	if(rgn.CreatePolygonRgn(pnt_arr,n))
	{
		LSSPACE_STRU xLsSpace;
		GetLsSpace(xLsSpace,insert_plate_param.ls_param[0].iBoltD);
		spaceLim=xLsSpace.PlateEdgeSpace;
		CLDSStdPartModel stdPart;
		stdPart.m_iStdPartType=iParamType;
		CopyInsertPlateParam(stdPart.param.insert_plate_param,insert_plate_param);
		stdPart.LayoutBolt();
		//
		f3dPoint *pPt=NULL;
		fPolygonRgn *pPolyRgn=NULL;
		for(pPt=stdPart.GetFirstBoltPos();pPt;pPt=stdPart.GetNextBoltPos())
		{
			pPolyRgn=&rgn;
			if(iParamType==TYPE_XEND)
			{	//ʮ�ֲ��
				if(pPt->feature!=11&&pPt->feature!=21)
					continue;	//������X-Yƽ���ڵ���˨
				if(pPt->x>insert_plate_param.L)
				{
					if(pPt->y>0)
						boltPosList[0].append(*pPt);
					else 
						boltPosList[1].append(*pPt);
					continue;	//����X�������L����˨
				}
			}
			if(pPolyRgn==NULL)
				continue;
			f3dPoint ls_pos=*pPt;
			if(pPolyRgn->PtInRgn(ls_pos)!=1)
			{
				delete []pnt_arr;
				return FALSE;
			}
			for(i=0;i<n;i++)
			{
				if(paramPlate.vertex_list[i].type==0)
				{
					f3dLine line(pnt_arr[i],pnt_arr[(i+1)%n]);
					if(SnapPerp(&perp,line,ls_pos,&dist))
					{
						int retCode=line.PtInLine(perp);
						if(retCode>0&&dist<spaceLim-epsilon)	//��������߶���
						{
							delete []pnt_arr;
							return FALSE;
						}
						else if(retCode==-1&&DISTANCE(ls_pos,line.endPt)<spaceLim-epsilon)
						{
							delete []pnt_arr;
							return FALSE;
						}
						else if(retCode==-2&&DISTANCE(ls_pos,line.startPt)<spaceLim-epsilon)
						{
							delete []pnt_arr;
							return FALSE;
						}
					}
				}
			}
		}
	}
	else
	{
		delete []pnt_arr;
		return FALSE;
	}
	delete []pnt_arr;
	//��鲻�ڻ��������ڵģ�ʮ�ֲ����˨�߾��Ƿ�Ϸ� wht 11-05-19
	if(iParamType==TYPE_XEND)
	{	
		fPolygonRgn wrap_plate_rgn[2];
		f3dPoint point_arr[2][4];
		//Y���������
		point_arr[0][0].Set(insert_plate_param.M+insert_plate_param.A,insert_plate_param.R);
		point_arr[0][1].Set(point_arr[0][0].x+insert_plate_param.S,point_arr[0][0].y);
		point_arr[0][2].Set(point_arr[0][1].x,point_arr[0][1].y+insert_plate_param.N);
		point_arr[0][3].Set(point_arr[0][0].x,point_arr[0][0].y+insert_plate_param.N);	
		//Y�Ḻ�����
		point_arr[1][0].Set(insert_plate_param.M+insert_plate_param.A,-insert_plate_param.R);
		point_arr[1][1].Set(point_arr[1][0].x+insert_plate_param.S,point_arr[1][0].y);
		point_arr[1][2].Set(point_arr[1][1].x,point_arr[1][1].y-insert_plate_param.N);
		point_arr[1][3].Set(point_arr[1][0].x,point_arr[1][0].y-insert_plate_param.N);
		for(int i=0;i<2;i++)
		{
			if(!wrap_plate_rgn[i].CreatePolygonRgn(&(point_arr[i][0]),4))
				return FALSE;
			f3dPoint *pPt=NULL;
			for(pPt=boltPosList[i].GetFirst();pPt;pPt=boltPosList[i].GetNext())
			{
				ls_pos=*pPt;
				if(wrap_plate_rgn[i].PtInRgn(ls_pos)!=1)
					return FALSE;
				for(int j=0;j<4;j++)
				{
					f3dLine line(point_arr[i][j],point_arr[i][(j+1)%4]);
					if(!SnapPerp(&perp,line,ls_pos,&dist))
						continue;
					int retCode=line.PtInLine(perp);
					if(retCode>0&&dist<spaceLim-epsilon)	//��������߶���
						return FALSE;
					else if(retCode==-1&&DISTANCE(ls_pos,line.endPt)<spaceLim-epsilon)
						return FALSE;
					else if(retCode==-2&&DISTANCE(ls_pos,line.startPt)<spaceLim-epsilon)
						return FALSE;
				}
			}
		}
	}
	return TRUE;	//�߾�Ϸ�
}

void CTubeEndLjStdPartLibDlg::OnSaveData() 
{
	SaveData();
}

BOOL CTubeEndLjStdPartLibDlg::SaveData()
{
	if(m_bModify==NULL&&m_bNewStdPart==NULL)
		return FALSE;	//���Ǳ༭���½�״̬ 
	HTREEITEM hCurItem=m_treeCtrl.GetSelectedItem();	//��ǰѡ�����һ�����ǵ�ǰ�༭��
	if(m_hCurEditItem&&hCurItem!=m_hCurEditItem)
		hCurItem=m_hCurEditItem;
	TREEITEM_INFO *pCurItemInfo=NULL;
	if(hCurItem)
		pCurItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hCurItem);
	if(pCurItemInfo==NULL)
		return FALSE;
	int iCurSel=(int)pCurItemInfo->dwRefData;
	if((m_bNewStdPart&&iCurSel!=-2)||(m_bModify&&iCurSel<0&&iCurSel!=-2))
	{
		GetDlgItem(IDC_BTN_CANCEL_MODIFY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_SAVE_DATA)->EnableWindow(FALSE);
		m_sOperPrompt = _T("��ǰδ�༭�κι���...");
		m_bModifyBoltPos=m_bNewStdPart=m_bModify=FALSE;
		UpdateData(FALSE);
		return FALSE;	//-2��ʾ����Ϊ�½���
	}
	FL_PARAM *pFlParam=NULL;
	FL_PARAM2 *pFlParam2=NULL;
	INSERT_PLATE_PARAM *pInsertPlateParam=NULL;
	HTREEITEM hParentItem=m_treeCtrl.GetParentItem(hCurItem);
	if(m_iStdPartType==0&&iCurSel<rollend_param_N)
	{
		for(int i=0;i<rollend_param_N;i++)
		{
			if(i!=iCurSel&&stricmp(insert_plate_param.codeName,rollend_param_table[i].codeName)==0)
			{
				AfxMessageBox("�Ѵ��ڸô��ŵĲ��Ͳ�壬��������ź��ٱ��棡");
				return FALSE;
			}
		}
		if(m_bNewStdPart)
		{
			if(rollend_param_N==200)
			{
				AfxMessageBox("��Ǹ��ϵͳ��ǰ����֧��200����¼!");
				return FALSE;
			}
			pCurItemInfo->dwRefData=rollend_param_N;
			pInsertPlateParam=&rollend_param_table[rollend_param_N];
			rollend_param_N++;
		}
		else 
			pInsertPlateParam=&rollend_param_table[iCurSel];
	}
	else if(m_iStdPartType==1&&iCurSel<uend_param_N)
	{
		for(int i=0;i<uend_param_N;i++)
		{
			if(i!=iCurSel&&stricmp(insert_plate_param.codeName,uend_param_table[i].codeName)==0)
			{
				AfxMessageBox("�Ѵ��ڸô��ŵ�U�Ͳ�壬��������ź��ٱ��棡");
				return FALSE;
			}
		}
		if(m_bNewStdPart)
		{
			if(uend_param_N==200)
			{
				AfxMessageBox("��Ǹ��ϵͳ��ǰ����֧��200����¼!");
				return FALSE;
			}
			pCurItemInfo->dwRefData=uend_param_N;
			pInsertPlateParam=&uend_param_table[uend_param_N];
			uend_param_N++;
		}
		else 
			pInsertPlateParam=&uend_param_table[iCurSel];
	}
	else if(m_iStdPartType==2&&iCurSel<xend_param_N)
	{
		for(int i=0;i<xend_param_N;i++)
		{
			if(i!=iCurSel&&stricmp(insert_plate_param.codeName,xend_param_table[i].codeName)==0)
			{
				AfxMessageBox("�Ѵ��ڸô��ŵ�ʮ�ֲ�壬��������ź��ٱ��棡");
				return FALSE;
			}
		}
		if(m_bNewStdPart)
		{
			if(xend_param_N==200)
			{
				AfxMessageBox("��Ǹ��ϵͳ��ǰ����֧��200����¼!");
				return FALSE;
			}
			pCurItemInfo->dwRefData=xend_param_N;
			pInsertPlateParam=&xend_param_table[xend_param_N];
			xend_param_N++;
		}
		else 
			pInsertPlateParam=&xend_param_table[iCurSel];
	}
	else if(m_iStdPartType==3&&iCurSel<fld_param_N)
	{
		for(int i=0;i<fld_param_N;i++)
		{
			if(i!=iCurSel&&stricmp(fl_param.codeName,fld_param_table[i].codeName)==0)
			{
				AfxMessageBox("�Ѵ��ڸô��ŵĶԺ���������������ź��ٱ��棡");
				return FALSE;
			}
		}
		if(m_bNewStdPart)
		{
			if(fld_param_N==200)
			{
				AfxMessageBox("��Ǹ��ϵͳ��ǰ����֧��200����¼!");
				return FALSE;
			}
			pCurItemInfo->dwRefData=fld_param_N;
			pFlParam=&fld_param_table[fld_param_N];
			fld_param_N++;
		}
		else 
			pFlParam=&fld_param_table[iCurSel];
	}
	else if(m_iStdPartType==4&&iCurSel<flp_param_N)
	{
		for(int i=0;i<flp_param_N;i++)
		{
			if(i!=iCurSel&&stricmp(fl_param.codeName,flp_param_table[i].codeName)==0)
			{
				AfxMessageBox("�Ѵ��ڸô��ŵ�ƽ����������������ź��ٱ��棡");
				return FALSE;
			}
		}
		if(m_bNewStdPart)
		{
			if(flp_param_N==200)
			{
				AfxMessageBox("��Ǹ��ϵͳ��ǰ����֧��200����¼!");
				return FALSE;
			}
			pCurItemInfo->dwRefData=flp_param_N;
			pFlParam=&flp_param_table[flp_param_N];
			flp_param_N++;
		}
		else 
			pFlParam=&flp_param_table[iCurSel];
	}
	else if(m_iStdPartType==5&&iCurSel<flg_param_N)
	{
		for(int i=0;i<flg_param_N;i++)
		{
			if(i!=iCurSel&&stricmp(fl_param2.codeName,flg_param_tbl[i].codeName)==0)
			{
				AfxMessageBox("�Ѵ��ڸô��ŵĸ��Է�������������ź��ٱ��棡");
				return FALSE;
			}
		}
		if(m_bNewStdPart)
		{
			if(flg_param_N==200)
			{
				AfxMessageBox("��Ǹ��ϵͳ��ǰ����֧��200����¼!");
				return FALSE;
			}
			pCurItemInfo->dwRefData=flg_param_N;
			pFlParam2=&flg_param_tbl[flg_param_N];
			flg_param_N++;
		}
		else 
			pFlParam2=&flg_param_tbl[iCurSel];
	}
	else if(m_iStdPartType==6&&iCurSel<flr_param_N)
	{
		for(int i=0;i<flr_param_N;i++)
		{
			if(i!=iCurSel&&stricmp(fl_param2.codeName,flr_param_tbl[i].codeName)==0)
			{
				AfxMessageBox("�Ѵ��ڸô��ŵ����Է�������������ź��ٱ��棡");
				return FALSE;
			}
		}
		if(m_bNewStdPart)
		{
			if(flr_param_N==200)
			{
				AfxMessageBox("��Ǹ��ϵͳ��ǰ����֧��200����¼!");
				return FALSE;
			}
			pCurItemInfo->dwRefData=flr_param_N;
			pFlParam2=&flr_param_tbl[flr_param_N];
			flr_param_N++;
		}
		else 
			pFlParam2=&flr_param_tbl[iCurSel];
	}
	//�������˨�߾�
	if(m_iStdPartType<3)
	{
		PARAM_PLATE_TYPE iPlateType;
		if(m_iStdPartType==0)
			iPlateType=TYPE_ROLLEND;
		else if(m_iStdPartType==1)
			iPlateType=TYPE_UEND;
		else 
			iPlateType=TYPE_XEND;
		if(m_bNewStdPart)	//�½���׼����ʱ���ڴ˴�������ز�����������һ��������˨�߾��� wht 11-05-23
			CopyInsertPlateParam(*pInsertPlateParam,insert_plate_param);
		if(!CheckPlateLsEdgeSpace(insert_plate_param,iPlateType))
		{
			if(AfxMessageBox("��˨�߾಻�Ϸ�,�Ƿ��������(��/��)?",MB_YESNO)==IDNO)
			{	//��˨�߾಻�Ϸ�ʱ��ʾ�û������������������ wht 11-06-08
				if(m_bNewStdPart)
				{	//��˨��಻�Ϸ���ɾ���¼ӵļ�¼ wht 11-05-23
					if(m_iStdPartType==0)
						rollend_param_N--;
					else if(m_iStdPartType==1)
						uend_param_N--;
					else 
						xend_param_N--;
					pCurItemInfo->dwRefData=-2; //-2��ʾΪ�½���
				}
				return FALSE;
			}
		}
	}
	//�����޸ĺ�Ĳ���
	if(pInsertPlateParam)
	{
		CopyInsertPlateParam(*pInsertPlateParam,insert_plate_param);
		//�����½�������
		m_treeCtrl.SetItemText(hCurItem,pInsertPlateParam->codeName);
	}
	else if(pFlParam)
	{
		CopyFlParam(*pFlParam,fl_param);
		//�����½�������
		m_treeCtrl.SetItemText(hCurItem,pFlParam->codeName);
	}
	else if(pFlParam2)
	{
		pFlParam2->CopyPropertye(fl_param2);
		//�����½�������
		m_treeCtrl.SetItemText(hCurItem,pFlParam2->codeName);
	}
	if(m_bNewStdPart&&m_iStdPartType>=0&&m_iStdPartType<=6)
	{	//���±�׼�������� wht 11-05-23
		CString sPartCount=_T("");
		if(m_iStdPartType==0)
			sPartCount.Format("���Ͳ��(%d)",rollend_param_N);
		else if(m_iStdPartType==1)
			sPartCount.Format("U�Ͳ��(%d)",uend_param_N);
		else if(m_iStdPartType==2)
			sPartCount.Format("ʮ�ֲ��(%d)",xend_param_N);
		else if(m_iStdPartType==3)
			sPartCount.Format("�Ժ�����(%d)",fld_param_N);
		else if(m_iStdPartType==4)
			sPartCount.Format("ƽ������(%d)",flp_param_N);
		else if(m_iStdPartType==5)
			sPartCount.Format("���Է���(%d)",flg_param_N);
		else if(m_iStdPartType==6)
			sPartCount.Format("���Է���(%d)",flr_param_N);
		if(hParentItem)
			m_treeCtrl.SetItemText(hParentItem,sPartCount);
	}
	GetDlgItem(IDC_BTN_CANCEL_MODIFY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SAVE_DATA)->EnableWindow(FALSE);
	m_sOperPrompt = _T("��ǰδ�༭�κι���...");
	m_bModifyBoltPos=m_bNewStdPart=m_bModify=FALSE;
	UpdateData(FALSE);
	return TRUE;
}

void CTubeEndLjStdPartLibDlg::OnCancelModify() 
{
	if(m_bNewStdPart)
	{
		HTREEITEM hCurItem=m_treeCtrl.GetSelectedItem();	//��ǰѡ�����һ�����ǵ�ǰ�༭��
		if(m_hCurEditItem&&hCurItem!=m_hCurEditItem)
			hCurItem=m_hCurEditItem;
		TREEITEM_INFO *pCurItemInfo=NULL;
		if(hCurItem)
			pCurItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hCurItem);
		if(pCurItemInfo==NULL)
			return;
		int iCurSel=(int)pCurItemInfo->dwRefData;
		if(iCurSel==-2)
			m_treeCtrl.DeleteItem(hCurItem);	//ɾ����ʱ������½���
	}
	GetDlgItem(IDC_BTN_CANCEL_MODIFY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SAVE_DATA)->EnableWindow(FALSE);
	m_sOperPrompt = _T("��ǰδ�༭�κι���...");
	m_bModifyBoltPos=m_bNewStdPart=m_bModify=FALSE;
	UpdateData(FALSE);
}

void CTubeEndLjStdPartLibDlg::OnEditItem()
{
	HTREEITEM hItem;
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo(&hItem,TRUE);
	if(pItemInfo==NULL)
		return;
	//�����ϴ��޸�
	IntelliSavePreModify();
	int iCurIndex=(int)pItemInfo->dwRefData;
	if(iCurIndex<0)
		return;
	//
	m_hCurEditItem=hItem;	//��¼��ǰ�ɱ༭��
	FL_PARAM *pFlParam=NULL;
	FL_PARAM2 *pFlParam2=NULL;
	INSERT_PLATE_PARAM *pInsertPlateParam=NULL;
	if(pItemInfo->itemType==TYPE_ROLLEND&&iCurIndex<rollend_param_N)
	{
		pInsertPlateParam=&rollend_param_table[iCurIndex];
		CopyInsertPlateParam(insert_plate_param,*pInsertPlateParam);
		m_sOperPrompt.Format("���Ͳ��%s���ڿɱ༭״̬...",pInsertPlateParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_UEND&&iCurIndex<uend_param_N)
	{
		pInsertPlateParam=&uend_param_table[iCurIndex];
		CopyInsertPlateParam(insert_plate_param,*pInsertPlateParam);
		m_sOperPrompt.Format("U�Ͳ��%s���ڿɱ༭״̬...",pInsertPlateParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_XEND&&iCurIndex<xend_param_N)
	{
		pInsertPlateParam=&xend_param_table[iCurIndex];
		CopyInsertPlateParam(insert_plate_param,*pInsertPlateParam);
		m_sOperPrompt.Format("ʮ�ֲ��%s���ڿɱ༭״̬...",pInsertPlateParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_FLD&&iCurIndex<fld_param_N)
	{
		pFlParam=&fld_param_table[iCurIndex];
		CopyFlParam(fl_param,*pFlParam);
		m_sOperPrompt.Format("�Ժ�����%s���ڿɱ༭״̬...",pFlParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_FLP&&iCurIndex<flp_param_N)
	{
		pFlParam=&flp_param_table[iCurIndex];
		CopyFlParam(fl_param,*pFlParam);
		m_sOperPrompt.Format("ƽ������%s���ڿɱ༭״̬...",pFlParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_FLG&&iCurIndex<flg_param_N)
	{
		pFlParam2=&flg_param_tbl[iCurIndex];
		fl_param2.CopyPropertye(*pFlParam2);
		m_sOperPrompt.Format("���Է���%s���ڿɱ༭״̬...",pFlParam2->codeName);
	}
	else if(pItemInfo->itemType==TYPE_FLR&&iCurIndex<flr_param_N)
	{
		pFlParam2=&flr_param_tbl[iCurIndex];
		fl_param2.CopyPropertye(*pFlParam2);
		m_sOperPrompt.Format("���Է���%s���ڿɱ༭״̬...",pFlParam2->codeName);
	}
	m_bModify=!(pFlParam||pInsertPlateParam||pFlParam2);
	GetDlgItem(IDC_BTN_CANCEL_MODIFY)->EnableWindow(m_bModify);
	GetDlgItem(IDC_BTN_SAVE_DATA)->EnableWindow(m_bModify);
	UpdateLsParas();
	//���������б�
	DisplayPropertyList();
	//������˨���ò���
	RefreshBoltLayoutList();
	RefreshBoltList();
	RefreshSketch(FALSE);
	Invalidate(FALSE);
	UpdateData(FALSE);
}

void CTubeEndLjStdPartLibDlg::OnDeleteItem()
{
	HTREEITEM hItem;
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo(&hItem);
	if(pItemInfo==NULL)
		return;
	int iCurIndex=(int)pItemInfo->dwRefData;
	if(iCurIndex!=-2&&iCurIndex<0)
		return;
	if(AfxMessageBox("ȷ��Ҫɾ����ǰ��׼������?",MB_YESNO)==IDNO)
		return;
	CString sPartType=_T("");
	HTREEITEM hParentItem=m_treeCtrl.GetParentItem(hItem);
	m_treeCtrl.DeleteItem(hItem);	
	if(iCurIndex==-2)
	{
		m_bNewStdPart=FALSE; //ɾ���½�����½���־���³�ʼ�� wht 11-05-12
		m_sOperPrompt = _T("��ǰδ�༭�κι���...");
		UpdateData(FALSE);
		return; //ɾ���½�����ʱ���˼��ɷ���
	}
	//ɾ����׼�������е���������� ��������׼������
	if(m_iStdPartType==0)
	{	//���Ͳ��
		//ɾ����nCurSel����¼�Ժ󣬺�ߵļ�¼����ǰ��һλ
		for(int i=iCurIndex;i<rollend_param_N-1;i++)
			CopyInsertPlateParam(rollend_param_table[i],rollend_param_table[i+1]);
		rollend_param_N--;
		sPartType.Format("���Ͳ��(%d)",rollend_param_N);
		if(hParentItem)
			m_treeCtrl.SetItemText(hParentItem,sPartType);
	}
	else if(m_iStdPartType==1)
	{	//U�Ͳ��
		//ɾ����nCurSel����¼�Ժ󣬺�ߵļ�¼����ǰ��һλ
		for(int i=iCurIndex;i<uend_param_N-1;i++)
			CopyInsertPlateParam(uend_param_table[i],uend_param_table[i+1]);
		uend_param_N--;
		sPartType.Format("U�Ͳ��(%d)",uend_param_N);
		if(hParentItem)
			m_treeCtrl.SetItemText(hParentItem,sPartType);
	}
	else if(m_iStdPartType==2)
	{	//ʮ�ֲ��
		//ɾ����nCurSel����¼�Ժ󣬺�ߵļ�¼����ǰ��һλ
		for(int i=iCurIndex;i<xend_param_N-1;i++)
			CopyInsertPlateParam(xend_param_table[i],xend_param_table[i+1]);	
		xend_param_N--;
		sPartType.Format("ʮ�ֲ��(%d)",xend_param_N);
		if(hParentItem)
			m_treeCtrl.SetItemText(hParentItem,sPartType);
	}
	else if(m_iStdPartType==3)
	{	//ɾ����iCurIndex����¼�Ժ󣬺�ߵļ�¼����ǰ��һλ
		for(int i=iCurIndex;i<fld_param_N-1;i++)
			CopyFlParam(fld_param_table[i],fld_param_table[i+1]);
		fld_param_N--;
		sPartType.Format("�Ժ�����(%d)",fld_param_N);
		if(hParentItem)
			m_treeCtrl.SetItemText(hParentItem,sPartType);
	}
	else if(m_iStdPartType==4)
	{
		//ɾ����iCurIndex����¼�Ժ󣬺�ߵļ�¼����ǰ��һλ
		for(int i=iCurIndex;i<flp_param_N-1;i++)
			CopyFlParam(flp_param_table[i],flp_param_table[i+1]);
		flp_param_N--;
		sPartType.Format("ƽ������(%d)",flp_param_N);
		if(hParentItem)
			m_treeCtrl.SetItemText(hParentItem,sPartType);
	}
	else if(m_iStdPartType==5)
	{
		//ɾ����iCurIndex����¼�Ժ󣬺�ߵļ�¼����ǰ��һλ
		for(int i=iCurIndex;i<flg_param_N-1;i++)
			flg_param_tbl[i].CopyPropertye(flg_param_tbl[i+1]);
		flg_param_N--;
		sPartType.Format("���Է���(%d)",flg_param_N);
		if(hParentItem)
			m_treeCtrl.SetItemText(hParentItem,sPartType);
	}
	else if(m_iStdPartType==6)
	{
		//ɾ����iCurIndex����¼�Ժ󣬺�ߵļ�¼����ǰ��һλ
		for(int i=iCurIndex;i<flr_param_N-1;i++)
			flr_param_tbl[i].CopyPropertye(flr_param_tbl[i+1]);
		flr_param_N--;
		sPartType.Format("���Է���(%d)",flr_param_N);
		if(hParentItem)
			m_treeCtrl.SetItemText(hParentItem,sPartType);
	}
}

void CTubeEndLjStdPartLibDlg::OnCopyAndNewItem()
{
	HTREEITEM hItem;
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo(&hItem,TRUE);
	if(pItemInfo==NULL)
		return;
	//�����ϴ��޸�
	IntelliSavePreModify();
	int iCurIndex=(int)pItemInfo->dwRefData;
	if(iCurIndex<0)
		return;
	//������ǰ�������ͣ���ʱ����һ���׼�����б���
	FL_PARAM *pFlParam=NULL;
	FL_PARAM2 *pFlParam2=NULL;
	INSERT_PLATE_PARAM *pInsertPlateParam=NULL;
	if(pItemInfo->itemType==TYPE_ROLLEND&&iCurIndex<rollend_param_N)
	{
		pInsertPlateParam=&rollend_param_table[iCurIndex];
		CopyInsertPlateParam(insert_plate_param,*pInsertPlateParam);
		m_sOperPrompt.Format("��¡%s�������½����Ͳ��...",pInsertPlateParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_UEND&&iCurIndex<uend_param_N)
	{
		pInsertPlateParam=&uend_param_table[iCurIndex];
		CopyInsertPlateParam(insert_plate_param,*pInsertPlateParam);
		m_sOperPrompt.Format("��¡%s�������½�U�Ͳ��...",pInsertPlateParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_XEND&&iCurIndex<xend_param_N)
	{
		pInsertPlateParam=&xend_param_table[iCurIndex];
		CopyInsertPlateParam(insert_plate_param,*pInsertPlateParam);
		m_sOperPrompt.Format("��¡%s�������½�ʮ�ֲ��...",pInsertPlateParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_FLD&&iCurIndex<fld_param_N)
	{
		pFlParam=&fld_param_table[iCurIndex];
		m_sOperPrompt.Format("��¡%s�������½��Ժ�����...",pFlParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_FLP&&iCurIndex<flp_param_N)
	{
		pFlParam=&flp_param_table[iCurIndex];
		CopyFlParam(fl_param,*pFlParam);
		m_sOperPrompt.Format("��¡%s�������½�ƽ������...",pFlParam->codeName);
	}
	else if(pItemInfo->itemType==TYPE_FLG&&iCurIndex<flg_param_N)
	{
		pFlParam2=&flg_param_tbl[iCurIndex];
		fl_param2.CopyPropertye(*pFlParam2);
		m_sOperPrompt.Format("��¡%s�������½����Է���...",pFlParam2->codeName);
	}
	else if(pItemInfo->itemType==TYPE_FLR&&iCurIndex<flr_param_N)
	{
		pFlParam2=&flr_param_tbl[iCurIndex];
		fl_param2.CopyPropertye(*pFlParam2);
		m_sOperPrompt.Format("��¡%s�������½����Է���...",pFlParam2->codeName);
	}
	else
		return;
	//ˢ����˨�����б� wht 11-05-23
	UpdateLsParas();
	RefreshBoltLayoutList();
	RefreshBoltList();
	//
	HTREEITEM hParentItem=m_treeCtrl.GetParentItem(hItem);
	if(hParentItem==NULL)
		return;
	HTREEITEM hNewItem=m_treeCtrl.InsertItem("�½�����",PRJ_IMG_PARA,PRJ_IMG_X_EDIT_STATE,hParentItem,TVI_FIRST);
	TREEITEM_INFO *pNewItemInfo=itemInfoList.append(TREEITEM_INFO(pItemInfo->itemType,-2));
	m_treeCtrl.SetItemData(hNewItem,(DWORD)pNewItemInfo);
	m_treeCtrl.SelectItem(hNewItem);
	m_hCurEditItem=hNewItem;	//��¼��ǰ�ɱ༭��
	//
	DisplayPropertyList();	//����������
	//
	m_bNewStdPart=TRUE;
	Invalidate(FALSE);
	UpdateData(FALSE);
}

void CTubeEndLjStdPartLibDlg::OnDeleteAllItem()
{
	HTREEITEM hItem;
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo(&hItem);
	if(pItemInfo==NULL)
		return;
	int iCurIndex=(int)pItemInfo->dwRefData;
	if(iCurIndex!=-1)
		return;
	if(AfxMessageBox("ȷ��Ҫ��յ�ǰ��׼����?",MB_YESNO)==IDNO)
		return;
	CString sPartType=_T("");
	if(pItemInfo->itemType==TYPE_ROLLEND)
	{	//���Ͳ��
		ZeroMemory(rollend_param_table,sizeof(INSERT_PLATE_PARAM)*rollend_param_N);	//��ձ�׼������ wht 11-06-10
		rollend_param_N=0;
		sPartType.Format("���Ͳ��(0)");
	}
	if(pItemInfo->itemType==TYPE_UEND)
	{	//U�Ͳ��
		ZeroMemory(uend_param_table,sizeof(INSERT_PLATE_PARAM)*uend_param_N);
		uend_param_N=0;
		sPartType.Format("U�Ͳ��(0)");
	}
	if(pItemInfo->itemType==TYPE_XEND)
	{	//ʮ�ֲ��
		ZeroMemory(xend_param_table,sizeof(INSERT_PLATE_PARAM)*xend_param_N);
		xend_param_N=0;
		sPartType.Format("ʮ�ֲ��(0)");
	}
	if(pItemInfo->itemType==TYPE_FLD)
	{
		ZeroMemory(fld_param_table,sizeof(FL_PARAM)*fld_param_N);
		fld_param_N=0;
		sPartType.Format("�Ժ�����(0)");
	}
	if(pItemInfo->itemType==TYPE_FLP)
	{
		ZeroMemory(flp_param_table,sizeof(FL_PARAM)*flp_param_N);
		flp_param_N=0;
		sPartType.Format("ƽ������(0)");
	}
	if(pItemInfo->itemType==TYPE_FLG)
	{
		ZeroMemory(flg_param_tbl,sizeof(FL_PARAM2)*flg_param_N);
		flg_param_N=0;
		sPartType.Format("���Է���(0)");
	}
	if(pItemInfo->itemType==TYPE_FLR)
	{
		ZeroMemory(flr_param_tbl,sizeof(FL_PARAM2)*flr_param_N);
		flr_param_N=0;
		sPartType.Format("���Է���(0)");
	}
	if(sPartType.GetLength()>0)
	{
		m_treeCtrl.SetItemText(hItem,sPartType);
		HTREEITEM hChildItem=m_treeCtrl.GetChildItem(hItem);
		while(hChildItem)
		{
			HTREEITEM hCurItem=hChildItem;
			hChildItem=m_treeCtrl.GetNextSiblingItem(hChildItem);
			m_treeCtrl.DeleteItem(hCurItem);
		}
	}
}

void CTubeEndLjStdPartLibDlg::OnNewItem()
{
	HTREEITEM hItem;
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo(&hItem,TRUE);
	if(pItemInfo==NULL)
		return;
	//�����ϴ��޸�
	IntelliSavePreModify();
	if(((int)pItemInfo->dwRefData)!=-1)
		return;
	//������ǰ�������ͣ���ʱ����һ���׼�����б���
	if(pItemInfo->itemType==TYPE_ROLLEND)
		m_sOperPrompt.Format("�½����Ͳ��...");
	else if(pItemInfo->itemType==TYPE_UEND)
		m_sOperPrompt.Format("�½�U�Ͳ��...");
	else if(pItemInfo->itemType==TYPE_XEND)
		m_sOperPrompt.Format("�½�ʮ�ֲ��...");
	else if(pItemInfo->itemType==TYPE_FLD)
		m_sOperPrompt.Format("�½��Ժ�����...");
	else if(pItemInfo->itemType==TYPE_FLP)
		m_sOperPrompt.Format("�½�ƽ������...");
	else if(pItemInfo->itemType==TYPE_FLG)
		m_sOperPrompt.Format("�½����Է���...");
	else if(pItemInfo->itemType==TYPE_FLR)
		m_sOperPrompt.Format("�½����Է���...");
	HTREEITEM hNewItem=m_treeCtrl.InsertItem("�½�����",PRJ_IMG_PARA,PRJ_IMG_X_EDIT_STATE,hItem,TVI_FIRST);
	TREEITEM_INFO *pNewItemInfo=itemInfoList.append(TREEITEM_INFO(pItemInfo->itemType,-2));
	m_treeCtrl.SetItemData(hNewItem,(DWORD)pNewItemInfo);
	m_treeCtrl.SelectItem(hNewItem);
	
	m_hCurEditItem=hNewItem;	//��¼��ǰ�ɱ༭��
	//��ʼ����׼��������
	InitParam();
	//���������б�
	DisplayPropertyList();
	//�����˨�����б��Լ���˨�б�
	m_listBolt.DeleteAllItems();
	m_listBoltParam.DeleteAllItems();

	m_bNewStdPart=TRUE;
	Invalidate(FALSE);
	UpdateData(FALSE);
}

//�����ԽӼ����
void CTubeEndLjStdPartLibDlg::OnFlCoupleLevelTable()
{
	CFlCoupleLevelDlg dlg;
	dlg.DoModal();
}

HBRUSH CTubeEndLjStdPartLibDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->GetDlgCtrlID()==IDC_S_OPER_PROMPT)
	{
		if(m_bModify||m_bNewStdPart)
			pDC->SetTextColor(RGB(255,0,0));
		else 
			pDC->SetTextColor(RGB(0,0,0));
	}
	else
		pDC->SetTextColor(RGB(0,0,0));
	return hbr;
}

void CTubeEndLjStdPartLibDlg::OnConfirm() 
{
	TREEITEM_INFO *pItemInfo=GetCurSelItemInfo();
	if(pItemInfo==NULL)
		return;
	IntelliSavePreModify();
	CDialog::OnOK();
}

void CTubeEndLjStdPartLibDlg::OnOK()
{
	//ȷ������
}

//���ݲ�����μ�������������
double CTubeEndLjStdPartLibDlg::CalTheroyWeight(INSERT_PLATE_PARAM param,PARAM_PLATE_TYPE iType)
{
	double fWidth=0,fLenght=0;
	if(iType==TYPE_UEND||iType==TYPE_XEND)	
	{
		fWidth=param.H;			//U/X�Ͳ��
		fLenght=param.L;
		if(iType==TYPE_UEND)	//δ���ǻ�������
			fLenght=2*param.L+(param.W+param.Thick)*Pi*0.5;
		//�԰��1/2Ϊ���������Բ�
		//fLenght=(param.L-param.Thick)*2+(Pi-2)*(param.Thick+param.W)/2;
	}
	else if(iType==TYPE_ROLLEND)
	{
		//fWidth=param.H+2*param.W-8*param.Thick/3;	//�԰��1/3Ϊ���������Բ�
		fWidth=param.H+2*param.W;
		fLenght=param.L;
	}
	double fAttachPlateWeight=0;
	if(iType==TYPE_XEND)	//ʮ�ֲ��������Ӱ�����
	{	
		fAttachPlateWeight+=param.S*param.N*param.W*7.85e-6*8;					//��������
		fAttachPlateWeight+=(param.H-param.Thick)*param.L*param.Thick*7.85e-6;	//��������
	}
	return fLenght*fWidth*param.Thick*7.85e-6+fAttachPlateWeight;
}
//���ݷ������μ��㷨����������
double CTubeEndLjStdPartLibDlg::CalTheroyWeight(FL_PARAM param,PARAM_PLATE_TYPE iType)
{
	double R0=param.D*0.5;	//��Բ�뾶
	double R=param.B*0.5;	//��Բ�뾶
	double fLenght=Pi*(R0*R0-R*R)/(R0-R);	//����
	double fCrossSectionArea=0;				//���������
	//��������������������:��Ƚ���+���������ν���+����Բ������
	double fArcArea=param.R*param.R*0.5;//param.R*param.R-Pi*param.R*param.R*0.25;
	fCrossSectionArea=param.C*(R0-R)+param.S*(param.H-param.C)+fArcArea;
	return fCrossSectionArea*fLenght*7.85e-6;
}
//���ݷ������μ��㷨����������
double CTubeEndLjStdPartLibDlg::CalTheroyWeight(FL_PARAM2 param,PARAM_PLATE_TYPE iType)
{
	double R0=param.fOuterD*0.5;	//��Բ�뾶
	double R=param.fInnerD*0.5;		//��Բ�뾶
	double thick=param.fThick;
	double fLenght=Pi*(R0*R0-R*R)/(R0-R);	//����
	double fCrossSectionArea=thick*(R0-R);	//���������
	return fCrossSectionArea*fLenght*7.85e-6;
}

//ˢ����˨���ò����б�
void CTubeEndLjStdPartLibDlg::RefreshBoltLayoutList()
{
	//ˢ�±�����
	m_listBoltParam.EmptyColumnHeader();
	if(m_iStdPartType<3)
	{	//��������
		m_listBoltParam.AddColumnHeader("S");
		m_listBoltParam.AddColumnHeader("R");
		m_listBoltParam.AddColumnHeader("C");
		m_listBoltParam.AddColumnHeader("ֱ��");
		m_listBoltParam.AddColumnHeader("�о�");
		m_listBoltParam.AddColumnHeader("�о�");
		m_listBoltParam.AddColumnHeader("X");
		m_listBoltParam.AddColumnHeader("Y");
		long para_col_wide_arr[8]={25,25,25,40,40,40,40,40};
		m_listBoltParam.InitListCtrl(para_col_wide_arr);
	}
	else
	{	//����������
		m_listBoltParam.AddColumnHeader("��˨ֱ��");
		m_listBoltParam.AddColumnHeader("��˨����");
		m_listBoltParam.AddColumnHeader("����Բֱ��");
		long para_col_wide_arr[3]={92,92,92};
		m_listBoltParam.InitListCtrl(para_col_wide_arr);
	}
	//ˢ������
	CString sTextValue;
	m_listBoltParam.DeleteAllItems();
	if(m_iStdPartType==3||m_iStdPartType==4)
	{
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		sTextValue.Format("%d",fl_param.nBoltD);
		lpInfo->AddSubItemText(sTextValue);	//ֱ��
		lpInfo->SetControlType(0,GCT_CMB_EDIT);
		lpInfo->SetListItemsStr(0,"12|16|20|24|27|30|");
		sTextValue.Format("%d",fl_param.nBoltNum);		
		lpInfo->AddSubItemText(sTextValue);	//��˨����
		sTextValue.Format("%f",fl_param.K);
		SimplifiedNumString(sTextValue);
		lpInfo->AddSubItemText(sTextValue);	//��˨����ֱ��
		m_listBoltParam.InsertRootItem(lpInfo);
	}
	else if(m_iStdPartType==5||m_iStdPartType==6)
	{
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		sTextValue.Format("%d",fl_param2.nBoltD);
		lpInfo->AddSubItemText(sTextValue);	//ֱ��
		lpInfo->SetControlType(0,GCT_CMB_EDIT);
		lpInfo->SetListItemsStr(0,"12|16|20|24|27|30|");
		sTextValue.Format("%d",fl_param2.nBoltNum);		
		lpInfo->AddSubItemText(sTextValue);	//��˨����
		sTextValue.Format("%f",fl_param2.fBoltCirD);
		SimplifiedNumString(sTextValue);
		lpInfo->AddSubItemText(sTextValue);	//��˨����ֱ��
		m_listBoltParam.InsertRootItem(lpInfo);
	}
	else if(m_iStdPartType<3)
	{
		for(LAYOUT_LS_PARAM* pLsPara=lsParaList.GetFirst();pLsPara;pLsPara=lsParaList.GetNext())
		{
			CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
			sTextValue.Format("%d",pLsPara->iBoltNum);		
			lpInfo->AddSubItemText(sTextValue);	//��˨����
			lpInfo->SetSubItemHelpText(0,"S��ʾ��˨����");
			sTextValue.Format("%d",pLsPara->iRow);		
			lpInfo->AddSubItemText(sTextValue,m_iStdPartType==2&&sTextValue.CompareNoCase("-1")==0);	//��˨����
			lpInfo->SetSubItemHelpText(1,"R==-1����C==-1��ʾˮƽ������ֱ�潻������˨");
			sTextValue.Format("%d",pLsPara->iColumn);		
			lpInfo->AddSubItemText(sTextValue,m_iStdPartType==2&&sTextValue.CompareNoCase("-1")==0);	//��˨����
			sTextValue.Format("%d",pLsPara->iBoltD);
			lpInfo->SetSubItemHelpText(2,"R==-1����C==-1��ʾˮƽ������ֱ�潻������˨");
			lpInfo->AddSubItemText(sTextValue);	//ֱ��
			lpInfo->SetControlType(3,GCT_CMB_EDIT);
			lpInfo->SetListItemsStr(3,"12|16|20|24|27|30|");
			sTextValue.Format("%.f",pLsPara->fVertSpace);		
			lpInfo->AddSubItemText(sTextValue);	//�о�
			lpInfo->SetSubItemHelpText(1,"�о�");
			sTextValue.Format("%.f",pLsPara->fHorzSpace);		
			lpInfo->AddSubItemText(sTextValue);	//�о�
			lpInfo->SetSubItemHelpText(1,"�о�");
			sTextValue.Format("%.f",pLsPara->fBoltPosX);		
			lpInfo->AddSubItemText(sTextValue);	//X
			lpInfo->SetSubItemHelpText(1,"����˨X����");
			sTextValue.Format("%.f",pLsPara->fBoltPosY);
			lpInfo->AddSubItemText(sTextValue);	//Y
			lpInfo->SetSubItemHelpText(1,"����˨Y����");
			CSuperGridCtrl::CTreeItem* pItem=m_listBoltParam.InsertRootItem(lpInfo);
			if(pItem)
				pItem->m_idProp=(DWORD)pLsPara;
		}
	}
	m_listBoltParam.Redraw();
}
//������˨���ò���
void CTubeEndLjStdPartLibDlg::InsertBoltLayoutParam()
{
	if(m_iStdPartType>2)
		return;
	if(lsParaList.GetNodeNum()>=5)
	{
		AfxMessageBox("���֧�����6����˨���ò���!");
		return;
	}
	int nBoltNum=LayoutBoltPara.m_nBoltNum;
	if(m_iStdPartType==2&&LayoutBoltPara.m_bMirLayout)
	{	//�ԳƲ���ʮ�ֲ����˨,��֤�������˨��Ϊż������wht 11-05-03
		if(nBoltNum%2!=0)
		{
			AfxMessageBox("�������˨������ż��������֧�ֶԳƲ�����˨!");
			return;
		}
		nBoltNum/=2;
	}
	LAYOUT_LS_PARAM* pLsPara=lsParaList.append();
	pLsPara->iBoltNum=LayoutBoltPara.m_nBoltNum;
	pLsPara->iBoltD=LayoutBoltPara.m_nBoltD;
	pLsPara->fHorzSpace=LayoutBoltPara.m_fColSpace;
	pLsPara->fVertSpace=LayoutBoltPara.m_fRowSpace;
	pLsPara->fBoltPosX=LayoutBoltPara.m_posFirstBolt.x;
	pLsPara->fBoltPosY=LayoutBoltPara.m_posFirstBolt.y;
	pLsPara->iRow=LayoutBoltPara.m_nRowNum;
	pLsPara->iColumn=LayoutBoltPara.m_nColNum;
	if(m_iStdPartType==2)
	{	//ʮ�ֲ�壬Ĭ�ϰ����в���
		if(LayoutBoltPara.m_bStaggerLayout) //���沼��˫����˨
			pLsPara->iRow=pLsPara->iColumn=-1;
		else 
		{
			pLsPara->iRow=1;
			pLsPara->iColumn=pLsPara->iBoltNum;
		}
		if(LayoutBoltPara.m_bMirLayout)
		{	//������˨��������������ӶԳƵ���һ����˨���ò���
			pLsPara=lsParaList.append(*pLsPara);	
			pLsPara->fBoltPosX+=((nBoltNum-1)*LayoutBoltPara.m_fColSpace+LayoutBoltPara.m_fSpecialSpace);
		}
	}
	//ˢ����˨�����б�
	RefreshBoltLayoutList();
	RefreshBoltList();
	//ˢ�½��������ʾ״̬
	Redraw();
	UpdateStdPartState();
}
//����ѡ�е���˨���ò���
void CTubeEndLjStdPartLibDlg::UpdateBoltLayoutParam()
{
	int iCurSel=m_listBoltParam.GetSelectedItem();
	CSuperGridCtrl::CTreeItem *pCurItem=m_listBoltParam.GetTreeItem(iCurSel);
	if(pCurItem==NULL)
		return;
	LAYOUT_LS_PARAM *pSelLsPara=(LAYOUT_LS_PARAM*)pCurItem->m_idProp;
	if(pSelLsPara==NULL)
	{
		AfxMessageBox("û��ѡ����˨���ò���������ʧ��!");
		return;
	}
	pSelLsPara->iBoltNum=LayoutBoltPara.m_nBoltNum;
	pSelLsPara->iBoltD	=LayoutBoltPara.m_nBoltD;
	pSelLsPara->iRow	=LayoutBoltPara.m_nRowNum;
	pSelLsPara->iColumn	=LayoutBoltPara.m_nColNum;
	pSelLsPara->fHorzSpace=LayoutBoltPara.m_fColSpace;
	pSelLsPara->fVertSpace=LayoutBoltPara.m_fRowSpace;
	pSelLsPara->fBoltPosX =LayoutBoltPara.m_posFirstBolt.x;
	pSelLsPara->fBoltPosY =LayoutBoltPara.m_posFirstBolt.y;
	//ˢ����˨�����б�
	RefreshBoltLayoutList();
	RefreshBoltList();
	//ˢ�½��������ʾ״̬
	Redraw();
	UpdateStdPartState();
}
void CTubeEndLjStdPartLibDlg::OnItemchangedBoltList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	Invalidate(FALSE);
	*pResult = 0;
}


#endif

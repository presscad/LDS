//<LOCALE_TRANSLATE Confirm by hxr/>
// SectionVeiwPropertyDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "SectionViewPropertyDlg.h"
#include "XhCharString.h"
#include "env_def.h"
#include "DatumPointPropertyDlg.h"
#include "PartUcsDlg.h"
#include "LmaDlg.h"
#include "DesignVectorPropDlg.h"
#include "MainFrm.h"
#include "LDSView.h"
#ifdef __COMMON_PART_INC_

// CSectionViewPropertyDlg �Ի���

IMPLEMENT_DYNAMIC(CSectionViewPropertyDlg, CDialog)
CSectionViewPropertyDlg::CSectionViewPropertyDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CSectionViewPropertyDlg::IDD, pParent)
{
	m_pWorkPartGroup=NULL;
	m_pWorkView=NULL;
	m_pSrcView=NULL;
	m_iCurDesParaType=-1;
#ifdef AFX_TARG_ENU_ENGLISH
	m_xPartList.AddColumnHeader("Handle",60);
	m_xPartList.AddColumnHeader("Type",100);
	m_xPartList.AddColumnHeader("Forced To Draw",130);
#else
	m_xPartList.AddColumnHeader("���",60);
	m_xPartList.AddColumnHeader("����",60);
	m_xPartList.AddColumnHeader("����ƽ��",75);
	m_xPartList.AddColumnHeader("ǿ�Ȼ���",70);
#endif
	m_xPartList.m_bEnableSortItems=TRUE;
}

CSectionViewPropertyDlg::~CSectionViewPropertyDlg()
{
}

void CSectionViewPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BOX, m_xPropList);
	DDX_Control(pDX, IDC_LIST_PART, m_xPartList);
}


BEGIN_MESSAGE_MAP(CSectionViewPropertyDlg, CDialog)
	ON_BN_CLICKED(ID_BTN_CONFIRM, OnBnClickedBtnConfirm)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PART, OnNMClickListPart)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PART, OnLvnKeydownListPart)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PART, OnNMRclickListPart)
	ON_COMMAND(ID_DRAW_BY_SELPART,OnDrawBySelPart)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PART, OnNMDblclkListPart)
END_MESSAGE_MAP()


// CSectionViewPropertyDlg ��Ϣ�������
BOOL ModifySectionViewProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CSectionViewPropertyDlg *pViewDlg = (CSectionViewPropertyDlg*)pPropList->GetParent();
	CSectionView *pCurView=pViewDlg->m_pWorkView;
	if(pItem->m_idProp==CSectionView::GetPropID("sName"))
		strcpy(pCurView->sName,valueStr);
	else if(pItem->m_idProp==CSectionView::GetPropID("m_iViewType"))
		pCurView->m_iViewType=(SECTION_VIEW_TYPE)(valueStr[0]-'0');
	else if(pItem->m_idProp==CSectionView::GetPropID("des_cs.m_eDatumPlane"))
	{
		CParaCS::PARAMETRIC_INFO param=pCurView->des_cs.GetParamInfo();
		param.m_eDatumPlane=valueStr[0]-'0';
		pCurView->des_cs.SetParamInfo(param);
		char sText[100]="";
		long idProp=CSectionView::GetPropID("ucs.axis_x");
		if(pCurView->GetPropValueStr(idProp,sText)>0)
			pPropList->SetItemPropValue(idProp,sText);
		idProp=CSectionView::GetPropID("ucs.axis_y");
		if(pCurView->GetPropValueStr(idProp,sText)>0)
			pPropList->SetItemPropValue(idProp,sText);
		idProp=CSectionView::GetPropID("ucs.axis_z");
		if(pCurView->GetPropValueStr(idProp,sText)>0)
			pPropList->SetItemPropValue(idProp,sText);
	}
	else if(pCurView->GetViewTypeId()==CSectionView::CLS_SLAVE_VIEW)
	{
		CSlaveSectionView *pSlaveView=(CSlaveSectionView*)pCurView;
		if(CSectionView::GetPropID("m_idHostView")==pItem->m_idProp)
		{
			int nIdStartPos=valueStr.ReverseFind(':')+1;
			CString subStr=valueStr.Mid(nIdStartPos,valueStr.GetLength()-1-nIdStartPos);
			int idView=atoi(subStr);
			CLDSPartGroup *pPartGroup=pSlaveView->BelongPartGroup();
			if(pPartGroup)
			{
				CSectionView *pHostView=pPartGroup->SonViews.GetValue(idView);
				if(pHostView)
					pSlaveView->m_idHostView=pHostView->GetId();
			}
		}
		else if(CSectionView::GetPropID("direct_offset")==pItem->m_idProp)//ƫ����
			pSlaveView->direct_offset=atof(valueStr);
		else if(CSectionView::GetPropID("symbol_dist")==pItem->m_idProp)//����ͼ������
			pSlaveView->symbol_dist=atof(valueStr);
		else if(CSectionView::GetPropID("symbol")==pItem->m_idProp)//����ͼ������
			strncpy(pSlaveView->symbol,valueStr,2);

	}
	return TRUE;
}

static void RefreshUcsAxisItemValue(CPropertyList *pPropList,CSectionView *pView)
{
	char sText[MAX_TEMP_CHAR_100+1]="";
	if(pView->GetPropValueStr(CSectionView::GetPropID("ucs.axis_x.x"),sText)>0)
		pPropList->SetItemPropValue(CSectionView::GetPropID("ucs.axis_x.x"),sText);
	if(pView->GetPropValueStr(CSectionView::GetPropID("ucs.axis_x.y"),sText)>0)
		pPropList->SetItemPropValue(CSectionView::GetPropID("ucs.axis_x.y"),sText);
	if(pView->GetPropValueStr(CSectionView::GetPropID("ucs.axis_x.z"),sText)>0)
		pPropList->SetItemPropValue(CSectionView::GetPropID("ucs.axis_x.z"),sText);
	//
	if(pView->GetPropValueStr(CSectionView::GetPropID("ucs.axis_y.x"),sText)>0)
		pPropList->SetItemPropValue(CSectionView::GetPropID("ucs.axis_y.x"),sText);
	if(pView->GetPropValueStr(CSectionView::GetPropID("ucs.axis_y.y"),sText)>0)
		pPropList->SetItemPropValue(CSectionView::GetPropID("ucs.axis_y.y"),sText);
	if(pView->GetPropValueStr(CSectionView::GetPropID("ucs.axis_y.z"),sText)>0)
		pPropList->SetItemPropValue(CSectionView::GetPropID("ucs.axis_y.z"),sText);
	//
	if(pView->GetPropValueStr(CSectionView::GetPropID("ucs.axis_z.x"),sText)>0)
		pPropList->SetItemPropValue(CSectionView::GetPropID("ucs.axis_z.x"),sText);
	if(pView->GetPropValueStr(CSectionView::GetPropID("ucs.axis_z.y"),sText)>0)
		pPropList->SetItemPropValue(CSectionView::GetPropID("ucs.axis_z.y"),sText);
	if(pView->GetPropValueStr(CSectionView::GetPropID("ucs.axis_z.z"),sText)>0)
		pPropList->SetItemPropValue(CSectionView::GetPropID("ucs.axis_z.z"),sText);
}

BOOL FireCallBackCommand(CWnd* pCallWnd,UINT message,WPARAM wParam)
{
	//CSectionViewPropertyDlg *pViewDlg = (CSectionViewPropertyDlg*)wParam;
	CPropertyList* pPropList = (CPropertyList*)pCallWnd;
	CSectionView* pCurView=(CSectionView*)wParam;//pViewDlg->m_pWorkView;
	
	if( (message==CSectionView::GetPropID("ucs.axis_x")&&pCurView->des_cs.IsDesAxisX())||
		(message==CSectionView::GetPropID("ucs.axis_y")&&pCurView->des_cs.IsDesAxisY())||
		(message==CSectionView::GetPropID("ucs.axis_z")&&pCurView->des_cs.IsDesAxisZ()))
	{
		CDesignVectorPropDlg desVecDlg;
		CParaCS::PARAMETRIC_INFO param=pCurView->des_cs.GetParamInfo();
		if(message==CSectionView::GetPropID("ucs.axis_x"))
			desVecDlg.m_xVector = param.desAxisX;
		else if(message==CSectionView::GetPropID("ucs.axis_y"))
			desVecDlg.m_xVector = param.desAxisY;
		else 
			desVecDlg.m_xVector = param.desAxisZ;
		if(desVecDlg.DoModal()==IDOK)
		{
			if(message==CSectionView::GetPropID("ucs.axis_x"))
				param.desAxisX=desVecDlg.m_xVector;
			else if(message==CSectionView::GetPropID("ucs.axis_y"))
				param.desAxisY=desVecDlg.m_xVector;
			else 
				param.desAxisZ=desVecDlg.m_xVector;
			pCurView->des_cs.SetParamInfo(param);
			pCurView->des_cs.UpdateACS(console.GetActiveModel());
			pCurView->ucs=pCurView->des_cs.GetACS();
		}
	}
	else if(message==CSectionView::GetPropID("ucs.origin"))
	{
		CDatumPointPropertyDlg desPointDlg;
		CParaCS::PARAMETRIC_INFO param=pCurView->des_cs.GetParamInfo();
		desPointDlg.m_xPoint=param.desOrg;
		if(desPointDlg.DoModal()==IDOK)
		{
			param.desOrg=desPointDlg.m_xPoint;
			pCurView->des_cs.SetParamInfo(param);
			pCurView->des_cs.UpdateACS(console.GetActiveModel());
			pCurView->ucs=pCurView->des_cs.GetACS();
		}
	}
	//pViewDlg->m_iCurDesParaType=-1;	//��ʼ��
	return TRUE;
}

BOOL SectionViewButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CSectionViewPropertyDlg *pViewDlg = (CSectionViewPropertyDlg*)pPropList->GetParent();
	CSectionView *pCurView=pViewDlg->m_pWorkView;
	CPropTreeItem *pFindItem=NULL;
	char sText[MAX_TEMP_CHAR_100+1]="";

	if(pItem->m_idProp==CSectionView::GetPropID("ucs"))
	{
		CPartUcsDlg part_ucs_dlg;
		part_ucs_dlg.ucs=pCurView->ucs;
		part_ucs_dlg.DoModal();
	}
	else if(pItem->m_idProp==CSectionView::GetPropID("ucs.origin"))
	{	
		pViewDlg->m_iCurDesParaType=3;
		pViewDlg->ExecuteCommand(pPropList,pItem->m_idProp,(WPARAM)pCurView);
	}
	else if(pItem->m_idProp==CSectionView::GetPropID("ucs.axis_x"))
	{
		if(pCurView->des_cs.IsDesAxisX())
		{
			pViewDlg->m_iCurDesParaType=0;
			pViewDlg->ExecuteCommand(pPropList,pItem->m_idProp,(WPARAM)pCurView);
		}
		else
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "X Axis";
#else
			vecDlg.m_sCaption = "X�᷽��";
#endif
			vecDlg.m_fVectorX = pCurView->ucs.axis_x.x;
			vecDlg.m_fVectorY = pCurView->ucs.axis_x.y;
			vecDlg.m_fVectorZ = pCurView->ucs.axis_x.z;
			vecDlg.m_bReadOnlyX=TRUE;
			vecDlg.m_bReadOnlyY=TRUE;
			vecDlg.m_bReadOnlyZ=TRUE;
			vecDlg.DoModal();
		}
	}
	else if(pItem->m_idProp==CSectionView::GetPropID("ucs.axis_y"))
	{
		if(pCurView->des_cs.IsDesAxisY())
		{
			pViewDlg->m_iCurDesParaType=1;
			pViewDlg->ExecuteCommand(pPropList,pItem->m_idProp,(WPARAM)pCurView);
		}
		else
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Y Axis";
#else
			vecDlg.m_sCaption = "Y�᷽��";
#endif
			vecDlg.m_fVectorX = pCurView->ucs.axis_y.x;
			vecDlg.m_fVectorY = pCurView->ucs.axis_y.y;
			vecDlg.m_fVectorZ = pCurView->ucs.axis_y.z;
			vecDlg.m_bReadOnlyX=TRUE;
			vecDlg.m_bReadOnlyY=TRUE;
			vecDlg.m_bReadOnlyZ=TRUE;
			vecDlg.DoModal();
		}	
	}
	else if(pItem->m_idProp==CSectionView::GetPropID("ucs.axis_z"))
	{
		if(pCurView->des_cs.IsDesAxisZ())
		{
			pViewDlg->m_iCurDesParaType=2;
			pViewDlg->ExecuteCommand(pPropList,pItem->m_idProp,(WPARAM)pCurView);
		}
		else
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Z Axis";
#else
			vecDlg.m_sCaption = "Z�᷽��";
#endif
			vecDlg.m_fVectorX = pCurView->ucs.axis_z.x;
			vecDlg.m_fVectorY = pCurView->ucs.axis_z.y;
			vecDlg.m_fVectorZ = pCurView->ucs.axis_z.z;
			vecDlg.m_bReadOnlyX=TRUE;
			vecDlg.m_bReadOnlyY=TRUE;
			vecDlg.m_bReadOnlyZ=TRUE;
			vecDlg.DoModal();
		}	
	}
	else if(pCurView->GetViewTypeId()==CSectionView::CLS_SLAVE_VIEW)
	{
		CSlaveSectionView *pSlaveView=(CSlaveSectionView*)pCurView;
		if(pItem->m_idProp==CSectionView::GetPropID("direction"))
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "the direction of cut away symbol";
#else
			vecDlg.m_sCaption = "����ͼ������";
#endif
			vecDlg.m_fVectorX = pSlaveView->direction.x;
			vecDlg.m_fVectorY = pSlaveView->direction.y;
			vecDlg.m_fVectorZ = pSlaveView->direction.z;
			if(vecDlg.DoModal()==IDOK)
			{
				pSlaveView->direction.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				if(pSlaveView->GetPropValueStr(pItem->m_idProp,sText)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
			}
		}
		else if(pItem->m_idProp==CSectionView::GetPropID("location"))
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "the location of cut away symbol";
#else
			vecDlg.m_sCaption = "����ͼ��λ��";
#endif
			vecDlg.m_fVectorX = pSlaveView->location.x;
			vecDlg.m_fVectorY = pSlaveView->location.y;
			vecDlg.m_fVectorZ = pSlaveView->location.z;
			if(vecDlg.DoModal()==IDOK)
			{
				pSlaveView->location.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
				if(pSlaveView->GetPropValueStr(pItem->m_idProp,sText)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
			}
		}
	}
	return TRUE;
}


BOOL CSectionViewPropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetCallBackFunc(NativeCallBackCommand);//FireCallBackCommand);
	//��ʼ������ID
	//CSectionView::InitViewPropHashtable();
	
	m_xPropList.SetDividerScale(0.5);
	m_xPropList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	
	//long col_wide_arr[2]={60,80};
	m_xPartList.InitListCtrl();//col_wide_arr);
	
	if(m_pWorkPartGroup&&m_pSrcView)
	{	//��¡���½���ͼ
		//m_pWorkView = m_pWorkPartGroup->SonViews.append();
		m_pWorkView = m_pWorkPartGroup->SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
		m_pWorkView->CopyProperty(m_pSrcView);
		sprintf(m_pWorkView->sName,"%s1",m_pSrcView->sName);
		if(m_pWorkView->m_iViewType==VIEW_MAIN)
			m_pWorkView->m_iViewType=VIEW_FRONT;
	}
	if(m_pWorkView==NULL||m_pWorkPartGroup==NULL)
		return FALSE;
	//��ʾϵͳ����
	//DisplaySectionViewProperty();
	//
	m_xPartItemHashHandle.Empty();
	for(CSectionView::PART_ITEM *pItem=m_pWorkView->SonParts.GetFirst();pItem;pItem=m_pWorkView->SonParts.GetNext())
	{
		m_xPartItemHashHandle.SetValue(pItem->hSonPart,pItem);
		CLDSPart *pPart = console.FromPartHandle(pItem->hSonPart);
		CString tempStr;
		CStringArray str_arr;
		if(pPart==NULL)
			continue;
		tempStr.Format("0X%X",pItem->hSonPart);
		str_arr.Add(tempStr);
		str_arr.Add(pPart->GetPartTypeName());
		CDrawingAssistant* pAssit=m_pWorkView->hashAssistPlanes.GetValue(pItem->iResidePlaneNo);
		if(pAssit==NULL)
			str_arr.Add("����");
		else
			str_arr.Add("չ����");
		if(pItem->cFlag==DRAWFLAG::AUTO_DRAW)
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr.Add("��");
#else
			str_arr.Add("��");
#endif
		else
			str_arr.Add("");
		int iCur=m_xPartList.InsertItemRecord(-1,str_arr);
		m_xPartList.SetItemData(iCur,(DWORD)pItem);
	}
	if(m_bInernalStart)	
	{	//�ָ�ѡ����
		CPropTreeItem *pItem=m_xPropList.FindItemByPropId(m_idEventProp,NULL);
		m_xPropList.SetFocus();
		if(pItem)
			m_xPropList.SetCurSel(pItem->m_iIndex);	//����ѡ����	
		m_xPropList.FinishSelectObjOper(m_hPickObj,m_idEventProp);
	}
	else
	{
		m_xPropList.InitPropHashtable();
		m_xPropList.CopyFrom(m_pWorkView);
	}
	m_xPropList.DisplayPropertyList();
	return TRUE;  
}

//�ַ�����ʽΪ:��ͼ����[id:0x156]
static CString MakeHostPartGroupViewCmbItemStr(CLDSPartGroup *pPartGroup)
{
	CString strCmbItem = "";
	for(CSectionView *pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
	{
		if(!pView->IsHostView())
			continue;
		if(strCmbItem.GetLength()>0)
			strCmbItem+="|";
		strCmbItem.AppendFormat("%s%s(Id:%d)",strCmbItem,pView->sName,pView->GetId());
	}
	return strCmbItem;
}

static void InsertPointPropItem(CPropertyList *pPropList,CPropTreeItem *pParentItem,CSectionView *pView,
								char *sPropID,CString sPropName,CString sPropHelp)
{
	CXhChar100 sText;
	//װ��ʱX�᷽��
	CItemInfo *lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_strPropName = sPropName;
	lpInfo->m_strPropHelp = sPropHelp;
	CPropTreeItem *pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
	pPropItem->m_idProp = CSectionView::GetPropID(sPropID);
	pPropItem->m_bHideChildren = TRUE;
	if(pView->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵX��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp.Format("%s X coordinate component.",sPropHelp);
#else
	lpInfo->m_strPropHelp.Format("%sX���������",sPropHelp);
#endif
	CPropTreeItem *pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("%s.x",sPropID);
	if(pView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp.Format("%s Y coordinate component.",sPropHelp);
#else
	lpInfo->m_strPropHelp.Format("%sY���������",sPropHelp);
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("%s.y",sPropID);
	if(pView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropHelp.Format("%s Z coordinate component.",sPropHelp);
#else
	lpInfo->m_strPropHelp.Format("%sZ���������",sPropHelp);
#endif
	pLeafItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("%s.z",sPropID);
	if(pView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
}

void CSectionViewPropertyDlg::DisplaySectionViewProperty()
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pLeafItem=NULL,*pRootItem=m_xPropList.GetRootItem();

	m_xPropList.CleanTree();
	m_xPropList.m_nObjClassTypeId = 0;
	//���ûص�����
	m_xPropList.SetModifyValueFunc(ModifySectionViewProperty);
	m_xPropList.SetButtonClickFunc(SectionViewButtonClick);

	//��ͼ������Ϣ
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "view basic info";
	lpInfo->m_strPropHelp = "view basic information";
#else
	lpInfo->m_strPropName = "��ͼ������Ϣ";
	lpInfo->m_strPropHelp = "��ͼ������Ϣ";
#endif
	pGroupItem = m_xPropList.InsertItem(pRootItem,lpInfo, -1);
	pGroupItem->m_idProp = CSectionView::GetPropID("basicInfo");
	//
	/*
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "������ͼ����";
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CSectionView::GetPropID("index");
	if(m_pWorkView->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->SetReadOnly();*/
	//����
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "name";
	lpInfo->m_strPropHelp = "name of additional view";
#else
	lpInfo->m_strPropName = "����";
	lpInfo->m_strPropHelp = "������ͼ����";
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CSectionView::GetPropID("sName");
	if(m_pWorkView->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "view type";
	lpInfo->m_strPropHelp = "type of additional view";
	lpInfo->m_cmbItems="1.Main View|2.Front View|3.Node Plate View|4.Circular Over Plate View|5.Circular Rib Plate View|6.Across Linking Plate View|7.Flange View|8.Circular Plate View|";
	lpInfo->m_cmbItems+="9.Insert Plate View|10.Foot Nail Angle View|11.Foot Nail Slot Plate View|12.Foot Plate View|13.Joint Section View|14.Relative Joint Tube View|15.K Node Intersecting View|100.User-defined View|";
#else
	lpInfo->m_strPropName = "��ͼ����";
	lpInfo->m_strPropHelp = "������ͼ����";
	lpInfo->m_cmbItems="1.����ͼ|2.����ͼ|3.�ڵ�����ͼ|4.��������ͼ|5.�����߰���ͼ|6.��Խ������ͼ|7.������ͼ|8.�������ͼ|";
	lpInfo->m_cmbItems+="9.�����ͼ|10.�ǸֽŶ�|11.�۸ֽŶ���|12.���Ű�|13.֧�ܶ���ͼ|14.�ԽӸֹܼ�ͼ|15.K�ڵ�����ͼ|100.�û�������ͼ|";
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CSectionView::GetPropID("m_iViewType");
	if(m_pWorkView->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pPropItem->SetReadOnly();
	
	//������ͼ
	if(m_pWorkView->GetViewTypeId()==CSectionView::CLS_SLAVE_VIEW)
	{	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "host main view";
		lpInfo->m_strPropHelp = "host main view corresponding with current view";
#else
		lpInfo->m_strPropName = "������ͼ";
		lpInfo->m_strPropHelp = "��ǰ��ͼ��Ӧ��������ͼ";
#endif
		lpInfo->m_cmbItems=MakeHostPartGroupViewCmbItemStr(m_pWorkPartGroup);
		pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CSectionView::GetPropID("m_idHostView");
		if(m_pWorkView->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		
		//����ͼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "cutaway view symbol";
		lpInfo->m_strPropHelp = "Child view's corresponding cutaway view symbol location and parameters in host main view";
#else
		lpInfo->m_strPropName = "����ͼ��";
		lpInfo->m_strPropHelp = "����ͼ��������ͼ�ж�Ӧ������ͼ��λ�ü�������";
#endif
		pGroupItem = m_xPropList.InsertItem(pRootItem,lpInfo, -1);
		pGroupItem->m_idProp = CSectionView::GetPropID("slaveViewPara");
		//
#ifdef AFX_TARG_ENU_ENGLISH
		InsertPointPropItem(&m_xPropList,pGroupItem,m_pWorkView,"location","datum position","draw datum position with cutaway view symbol");
		InsertPointPropItem(&m_xPropList,pGroupItem,m_pWorkView,"direction","direction","direction of cutaway view symbol");
#else
		InsertPointPropItem(&m_xPropList,pGroupItem,m_pWorkView,"location","��׼λ��","����ͼ���Ż��ƻ�׼λ��");
		InsertPointPropItem(&m_xPropList,pGroupItem,m_pWorkView,"direction","����","����ͼ���ų���");
#endif
		//ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "offset";
		lpInfo->m_strPropHelp = "offset value of cutaway view symbol' drawing position in cutaway view symbol's direction.";
#else
		lpInfo->m_strPropName = "ƫ����";
		lpInfo->m_strPropHelp = "����ͼ������λ��������ͼ���������ƫ������";
#endif
		pLeafItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
		pLeafItem->m_idProp = CSectionView::GetPropID("direct_offset");
		if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		//����ͼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "width";
		lpInfo->m_strPropHelp = "Name of cutaway view symbol";
#else
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "����ͼ�����ơ�";
#endif
		pLeafItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
		pLeafItem->m_idProp = CSectionView::GetPropID("symbol_dist");
		if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		//����ͼ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Name of cutaway view symbol";
		lpInfo->m_strPropHelp = "Name displayed when drawing cutaway view symbol, maximum 2 characters.";
#else
		lpInfo->m_strPropName = "����ͼ������";
		lpInfo->m_strPropHelp = "��������ͼ����ʱ��ʾ�����ƣ����Ϊ�����ַ���";
#endif
		pLeafItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
		pLeafItem->m_idProp = CSectionView::GetPropID("symbol");
		if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;	
	}
	
	//װ�䷽λ
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "assemble location";
	lpInfo->m_strPropHelp = "parts assemble location and assemble direction.";
#else
	lpInfo->m_strPropName = "װ�䷽λ";
	lpInfo->m_strPropHelp = "������װ��λ�ü�װ��ʱ��װ�䷽��";
#endif
	pGroupItem = m_xPropList.InsertItem(pRootItem,lpInfo, -1);
	pGroupItem->m_idProp = CSectionView::GetPropID("ucs");

	//װ��ƽ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_COMBO;
	lpInfo->m_cmbItems=CParaCS::GetDatumPlaneCmbItems();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "datum plane";
	lpInfo->m_strPropHelp = "specify two axes direction,automatically calculate vertical and current plane's direction, modify the second axis direction with the first axis's direction unchanged. for example:X-Yplane, first calculate Z axis direction according to X and Y direction, modify Y axis direction according to X and Z axes.";
#else
	lpInfo->m_strPropName = "��׼��";
	lpInfo->m_strPropHelp = "ָ�������᷽���Զ��������ֱ�뵱ǰ��ķ��򣬵�һ���᷽�򲻱������ڶ����᷽����:X-Y��,����X��Y�����Z�᷽����X��Z������Y�᷽��";
#endif
	pLeafItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("des_cs.m_eDatumPlane");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;

	//װ��λ��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "position";
	lpInfo->m_strPropHelp = "position of assemble datum point.";
#else
	lpInfo->m_strPropName = "λ��";
	lpInfo->m_strPropHelp = "װ������λ�á�";
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CSectionView::GetPropID("ucs.origin");
	pPropItem->m_bHideChildren = TRUE;

	//װ������ϵԭ��λ��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assemble coordinate system origin position X coordinate component.";
#else
	lpInfo->m_strPropName = "X����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��X���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.origin.x");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵԭ��λ��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assemble coordinate system origin position Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Y���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.origin.y");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵԭ��λ��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assemble coordinate system origin position Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z����";
	lpInfo->m_strPropHelp = "װ������ϵԭ��λ��Z���������";
#endif
	pLeafItem= m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.origin.z");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱX�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X Axis";
	lpInfo->m_strPropHelp = "in X axis direction when assembling";
#else
	lpInfo->m_strPropName = "X�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱX�᷽��";
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CSectionView::GetPropID("ucs.axis_x");
	pPropItem->m_bHideChildren = TRUE;
	if(m_pWorkView->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵX��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assemble coordinate system X axis X coordinate component.";
#else
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵX��X���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.axis_x.x");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assemble coordinate system X axis Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵX��Y���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.axis_x.y");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵX��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assemble coordinate system X axis Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵX��Z���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.axis_x.z");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱY�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y Axis";
	lpInfo->m_strPropHelp = "in Y axis direction when assembling";
#else
	lpInfo->m_strPropName = "Y�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱY�᷽��";
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CSectionView::GetPropID("ucs.axis_y");
	pPropItem->m_bHideChildren = TRUE;
	if(m_pWorkView->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	
	//װ������ϵY��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assemble coordinate system Y axis X coordinate component.";
#else
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵY��X���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.axis_y.x");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵY��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assemble coordinate system Y axis Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵY��Y���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.axis_y.y");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵY��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assemble coordinate system Y axis Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵY��Z���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.axis_y.z");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ��ʱZ�᷽��
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z Axis";
	lpInfo->m_strPropHelp = "in Z axis direction when assembling";
#else
	lpInfo->m_strPropName = "Z�᷽��";
	lpInfo->m_strPropHelp = "װ��ʱZ�᷽��";
#endif
	pPropItem = m_xPropList.InsertItem(pGroupItem,lpInfo, -1);
	pPropItem->m_idProp = CSectionView::GetPropID("ucs.axis_z");
	pPropItem->m_bHideChildren = TRUE;
	if(m_pWorkView->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//װ������ϵZ��X�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "X";
	lpInfo->m_strPropHelp = "assemble coordinate system Z axis X coordinate component.";
#else
	lpInfo->m_strPropName = "X�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��X���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.axis_z.x");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵZ��Y�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Y";
	lpInfo->m_strPropHelp = "assemble coordinate system Z axis Y coordinate component.";
#else
	lpInfo->m_strPropName = "Y�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��Y���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.axis_z.y");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();
	//װ������ϵZ��Z�������
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Z";
	lpInfo->m_strPropHelp = "assemble coordinate system Z axis Z coordinate component.";
#else
	lpInfo->m_strPropName = "Z�������";
	lpInfo->m_strPropHelp = "װ������ϵZ��Z���������";
#endif
	pLeafItem = m_xPropList.InsertItem(pPropItem,lpInfo, -1);
	pLeafItem->m_idProp = CSectionView::GetPropID("ucs.axis_z.z");
	if(m_pWorkView->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
		pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
	pLeafItem->SetReadOnly();

	m_xPropList.Redraw();
}

void CSectionViewPropertyDlg::OnOK()
{
	m_xPropList.CopyTo(m_pWorkView);
}

void CSectionViewPropertyDlg::OnBnClickedBtnConfirm()
{
	m_xPropList.CopyTo(m_pWorkView);
	//���������麸����ͼ����
	m_pWorkView->des_cs.UpdateACS(&Ta);
	m_pWorkView->ucs=m_pWorkView->des_cs.GetACS();
	UCS_STRU ucs=m_pWorkView->ucs;
	if(ucs.axis_x.IsZero()||ucs.axis_y.IsZero()||ucs.axis_z.IsZero())
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("coordinate system is illegal(axis direction cann't be zero)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("coordinate system is illegal( not right hand spiral orthogonal coordinate)");
#else
		AfxMessageBox("����ϵ���Ϸ�(�����᷽����Ϊ��)");
	else if(!is_ucs_valid(ucs))
		AfxMessageBox("����ϵ���Ϸ�(����������������������ϵ)");
#endif
	else
		CDialog::OnOK();
}

void CSectionViewPropertyDlg::SelectPart(int nCurSel) 
{
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	char tem_str[100];
	m_xPartList.GetItemText(nCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hCurPart);
	pLDSView->UpdatePropertyPage();
}

void CSectionViewPropertyDlg::OnNMClickListPart(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iCurSel = m_xPartList.GetSelectedItem();
	if(iCurSel>=0)
		SelectPart(iCurSel);
	*pResult = 0;
}

void CSectionViewPropertyDlg::OnNMDblclkListPart(NMHDR *pNMHDR, LRESULT *pResult)
{	//�麸ͼ������ͼ����ʱ�ݲ�֧��ָ������������ wht 11-07-18
	POSITION pos = m_xPartList.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_xPartList.GetNextSelectedItem(pos);
		if(iCurSel>=0)
		{
			CSectionView::PART_ITEM *pItem=(CSectionView::PART_ITEM*)m_xPartList.GetItemData(iCurSel);
			if(pItem->cFlag==DRAWFLAG::AUTO_JUSTIFY)
				pItem->cFlag=DRAWFLAG::AUTO_DRAW;	//ǿ�Ȼ���
			else
				pItem->cFlag=DRAWFLAG::AUTO_JUSTIFY;//��ȫ�Զ��ж�
			if(pItem->cFlag==DRAWFLAG::AUTO_DRAW)
#ifdef AFX_TARG_ENU_ENGLISH
				m_xPartList.SetItemText(iCurSel,3,"��");
#else
				m_xPartList.SetItemText(iCurSel,3,"��");
#endif
			else
				m_xPartList.SetItemText(iCurSel,3,"");
		}
	}
	*pResult = 0;
}

void CSectionViewPropertyDlg::OnLvnKeydownListPart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	POSITION pos = m_xPartList.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_xPartList.GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_DELETE)
		{
			while(iCurSel>=0)
			{
				CSectionView::PART_ITEM *pPartItem=(CSectionView::PART_ITEM*)m_xPartList.GetItemData(iCurSel);
				if(pPartItem)
					m_xPartItemHashHandle.DeleteNode(pPartItem->hSonPart);
				m_xPartList.DeleteItem(iCurSel);
				pos=m_xPartList.GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_xPartList.GetNextSelectedItem(pos);
			}
		}
	}
	//����ͼ���Ƴ�ѡ�й�����������ʾ��ͼ
	//�ӹ����б����Ƴ���ɾ���Ĺ���
	for(CSectionView::PART_ITEM *pItem=m_pWorkView->SonParts.GetFirst();pItem;pItem=m_pWorkView->SonParts.GetNext())
	{
		if(m_xPartItemHashHandle.GetValue(pItem->hSonPart)==NULL)
			m_pWorkView->SonParts.DeleteCursor();
	}
	m_pWorkView->SonParts.Clean();
	//
	CTowerPartGroupDlg *pPartGroupDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPartGroupPage();
	if(pPartGroupDlg)
		pPartGroupDlg->DisplaySectionView(m_pWorkPartGroup,m_pWorkView);
	*pResult = 0;
}

void CSectionViewPropertyDlg::ContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint scr_point = point;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_DRAW_BY_SELPART,"draw with the selected parts as datum");
#else
	pMenu->AppendMenu(MF_STRING,ID_DRAW_BY_SELPART,"��ѡ�й���Ϊ��׼��ͼ");
#endif
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

void CSectionViewPropertyDlg::OnNMRclickListPart(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iCurSel = m_xPartList.GetSelectedItem();
	if(iCurSel<0)
		return;
	CPoint point;
	GetCursorPos(&point);
	CRect rect;
	m_xPartList.GetWindowRect(&rect);
	if(rect.PtInRect(point))
		ContextMenu(this,point);
	*pResult = 0;
}

void CSectionViewPropertyDlg::OnDrawBySelPart()
{
	int iCurSel=m_xPartList.GetSelectedItem();
	if(iCurSel<0)
		return;
	CSectionView::PART_ITEM *pItem=(CSectionView::PART_ITEM*)m_xPartList.GetItemData(iCurSel);
	if(pItem==NULL)
		return;
	CLDSPart *pPart=console.FromPartHandle(pItem->hSonPart);
	if(pPart)
	{
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			if(fabs(pPart->ucs.axis_z*m_pWorkView->ucs.axis_z)>EPS_COS)
				return;
			//ѡ�й���Ϊ�ְ�ʱ�Ըְ�Z�᷽��Ϊ��ͼ����ϵZ�᷽��
			CParaCS::PARAMETRIC_INFO param=m_pWorkView->des_cs.GetParamInfo();
			param.m_eDatumPlane=CParaCS::DP_ZX;	//Z-X
			param.desAxisZ.norm_style=3;
			param.desAxisZ.hVicePart=pPart->handle;
			m_pWorkView->des_cs.SetParamInfo(param);
			m_pWorkView->des_cs.UpdateACS(console.GetActiveModel());
			m_pWorkView->ucs=m_pWorkView->des_cs.GetACS();
			DisplaySectionViewProperty();
		}
	}
	CTowerPartGroupDlg *pPartGroupDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPartGroupPage();
	if(pPartGroupDlg)
		pPartGroupDlg->DisplaySectionView(m_pWorkPartGroup,m_pWorkView);
}
#endif

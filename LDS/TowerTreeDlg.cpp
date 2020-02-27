// TowerTreeDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "macro.h"
#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)||defined(__TDA_)
#include "LDS.h"
#include "LDSView.h"
#elif defined(__LDS_GROUP_CONTEXT_)//defined(__LDSGROUP_)||defined(__TSAGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)||defined(__TDAGROUP_)
#include "LdsGroup.h"
#include "LdsGroupView.h"
#endif
#include "MainFrm.h"
#include "TowerTreeDlg.h"
#include "Tower.h"
#include "image.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "LmaDlg.h"
#include "XhCharString.h"
#ifdef _RELATIVE_DLG
#include "ModelPropDlg.h"
#include "FilePropertyDlg.h"
#include "DatumPointDlg.h"
#include "DatumLineDlg.h"
#include "DatumPlaneDlg.h"
#include "LayerViewDlg.h"
#include "DefinedViewPropDlg.h"
#include "DefinedViewDlg.h"
#include "FolderNameDlg.h"
#include "PartAuthPermDlg.h"
#include "CfgWorkStatusDlg.h"
#include "SetNodeRestrictDlg.h"
#include "WindLoadSegDlg.h"
#include "SelectWindLoadDlg.h"
#include "WorkStatusDlg.h"
#include "CreateFace.h"
#include "ComparePartNoString.h"
#include "DatumPointPropertyDlg.h"
#include "ArcLiftDlg.h"
#include "InputAnValDlg.h"
#endif
#if defined(__LDS_)||defined(__TSA_)
#include "CfgCalScopeDlg.h"
#include "EqChkScopeDlg.h"
#include "AttachWindLoadDlg.h"
#include "CalWindBetaZ.h"
#include "CalWindBetaZDlg.h"
#include "BatchCalWindSegBetaZDlg.h"
#include "WorkStatusListDlg.h"
#include "SelectWorkCaseDlg.h"
#include "NodeLoadListDlg.h"
#include "WireLoadDatabase.h"
#include "PreProcessor.h"
#include "SortFunc.h"
#endif
#include "DatumLinePropertyDlg.h"
#include "DatumPlanePropertyDlg.h"
#include "UserDefMsg.h"
#include "LicFuncDef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
static DROPEFFECT DragOverFunc(CTreeCtrl *pTreeCtrl, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TVHITTESTINFO HitTestInfo;
	HitTestInfo.pt = point;
	HTREEITEM hItem = pTreeCtrl->HitTest(&HitTestInfo);
	if(hItem&&pDataObject->IsDataAvailable(CF_TEXT))
	{	
		DWORD dwData=pTreeCtrl->GetItemData(hItem);
		TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
		if(pItemInfo&&pItemInfo->itemType==VIEW_CASE)
		{
			for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
			{
				if(pView==(CDisplayView*)pItemInfo->dwRefData&&pView->m_iViewType==1)
				{
					pTreeCtrl->SelectDropTarget(hItem);
					return DROPEFFECT_LINK;
				}
			}
		}
		else if(pItemInfo&&pItemInfo->itemType==BLOCK_CASE)
		{
			CBlockModel *pBlock=(CBlockModel*)pItemInfo->dwRefData;
			if(pBlock&&pBlock->IsEmbeded())
			{
				pTreeCtrl->SelectDropTarget(hItem);
				return DROPEFFECT_LINK;
			}
		}
	}
	return DROPEFFECT_NONE;
}
static BOOL DropFunc(CTreeCtrl *pTreeCtrl, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	long h,num=0;
	TVHITTESTINFO HitTestInfo;
	HitTestInfo.pt = point;
	HTREEITEM hItem = pTreeCtrl->HitTest(&HitTestInfo);
	if(hItem==NULL||!pDataObject->IsDataAvailable(CF_TEXT))
		return FALSE;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL||(pItemInfo->itemType!=VIEW_CASE&&pItemInfo->itemType!=BLOCK_CASE))
		return FALSE;
	FORMATETC fmtetc = {CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM};
	STGMEDIUM stgmed;
	ZeroMemory(&stgmed, sizeof(STGMEDIUM));
	BOOL bGet = pDataObject->GetData(CF_TEXT,&stgmed,&fmtetc);
	if(!bGet || stgmed.tymed!=TYMED_ISTREAM)
		return FALSE;
	//
	GlobalLock(stgmed.pstm); //�����ڴ��
	LARGE_INTEGER dlibMove;
	dlibMove.QuadPart=0;
	stgmed.pstm->Seek(dlibMove,STREAM_SEEK_SET,NULL);
	HRESULT hr=stgmed.pstm->Read(&num,sizeof(long),NULL);
	//�����͹��������ݼ�����Ӧ��ͼ���߲�����
	CDisplayView *pView=NULL;
	CBlockModel *pBlock=NULL;
	CHashList<long> viewPartHashId;
	if(pItemInfo->itemType==VIEW_CASE)
	{
		pView=(CDisplayView*)pItemInfo->dwRefData;
		if(pView&&pView->m_iViewType!=1)
			return FALSE;
		if(pView)
		{
			CViewPart *pViewPart = NULL;
			for(pViewPart=pView->GetFirstViewPart();pViewPart;pViewPart=pView->GetNextViewPart())
				viewPartHashId.SetValue(pViewPart->handle,pViewPart->handle);
		}
	}
	else if(pItemInfo->itemType==BLOCK_CASE)
	{
		pBlock=(CBlockModel*)pItemInfo->dwRefData;
		if(pBlock&&pBlock->IsEmbeded()==FALSE)
			return FALSE;
		for(CBlockReference *pBlockRef=Ta.BlockRef.GetFirst();pBlockRef;pBlockRef=Ta.BlockRef.GetNext())
		{
			if(pBlockRef->m_hBlock==pBlock->handle)
				pBlockRef->SetModified();
		}
	}
	if(pBlock==NULL&&pView==NULL)
		return FALSE;
	for(int i=0;i<num;i++)
	{
		stgmed.pstm->Read(&h,sizeof(long),NULL);
		CLDSDbObject *pObj=(CLDSDbObject*)console.FromHandle(h,TRUE);
		if(pObj==NULL)
			continue;
		if(pView)
		{	//��ͼ
			if(viewPartHashId.GetValue(h))
				continue;
			CViewPart *pViewPart = pView->AppendViewPart(pObj->handle);
			pViewPart->idClassType=pObj->GetClassTypeId();
	#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupLineAngle=(CLDSGroupLineAngle*)pObj;
				for(int j=0;j<4;j++)
				{
					if(pGroupLineAngle->group_style<2&&j>=2)
						continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ��� wht 11-08-05
					if(pGroupLineAngle->son_jg_h[j]>0)
					{
						pViewPart=pView->AppendViewPart(pGroupLineAngle->son_jg_h[j]);
						pViewPart->idClassType=CLS_LINEANGLE;
					}
				}
			}
	#endif
		}
		else if(pBlock)
		{	//����
			pObj->m_hBlock=pBlock->handle;
#ifdef __ANGLE_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupLineAngle=(CLDSGroupLineAngle*)pObj;
				for(int j=0;j<4;j++)
				{
					if(pGroupLineAngle->son_jg_h[j]<0x20)
						continue;
					CLDSLineAngle *pLineAngle = (CLDSLineAngle*)console.FromPartHandle(pGroupLineAngle->son_jg_h[j],CLS_LINEANGLE);
					if(pLineAngle)
						pLineAngle->m_hBlock=pBlock->handle;
				}
			}
#endif	
		}
	}
	GlobalUnlock((HGLOBAL)stgmed.pstm);
	// Release the stream.
	ReleaseStgMedium(&stgmed);
	return TRUE;
}

static BOOL VisitViewItem(CTowerTreeCtrl *pTreeCtrl,HTREEITEM hItem)
{	//����������ͼ�ڵ�Ϊ��ͨ��ʽ
	TREEITEM_INFO *pItem=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItem&&pItem->itemType==VIEW_CASE)
		pTreeCtrl->SetItemState(hItem,0,TVIS_BOLD);
	return FALSE;
}

static BOOL IsValidDragS(CTowerTreeCtrl *pTreeCtrl, HTREEITEM hItemDragS)
{
	if(hItemDragS==NULL)
		return FALSE;
	//�����϶���ͼ�Լ�������ͼ��
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItemDragS);
	if(pItemInfo==NULL)
		return FALSE;
	if(pItemInfo->itemType!=VIEW_CASE&&pItemInfo->itemType!=VIEW_DEFINEDFOLDER)
		return FALSE;
	return TRUE;
}

static BOOL IsValidDragD(CTowerTreeCtrl *pTreeCtrl, HTREEITEM hItemDragS, HTREEITEM hItemDragD)
{
	if(hItemDragS==NULL||hItemDragD==NULL)
		return FALSE;
	TREEITEM_INFO *pItemInfoS=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItemDragS);
	TREEITEM_INFO *pItemInfoD=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItemDragD);
	if(pItemInfoS==NULL||pItemInfoD==NULL)
		return FALSE;
	CDisplayView *pViewS=(CDisplayView*)pItemInfoS->dwRefData;
	CDisplayView *pViewD=(CDisplayView*)pItemInfoD->dwRefData;
	if(pViewS==NULL)
		return FALSE;
	if(pItemInfoS->itemType==VIEW_CASE&&pItemInfoD->itemType==VIEW_LAYERSET)
	{	//ͼ����ͼ�Ϸŵ�ͼ����ͼ������
		if(pViewS->m_iViewType==0)
			return TRUE;
	}
	else if(pItemInfoS->itemType==VIEW_CASE&&pItemInfoD->itemType==VIEW_DEFINEDSET)
	{	//������ͼ�Ϸŵ�������ͼ������
		if(pViewS->m_iViewType==1)
			return TRUE;
	}
	else if(pItemInfoS->itemType==VIEW_CASE&&pItemInfoD->itemType==VIEW_DEFINEDFOLDER)
	{	//������ͼ�Ϸŵ�������ͼ����
		if(pViewS->m_iViewType==1)
			return TRUE;
	}
	else if((pItemInfoS->itemType==VIEW_CASE&&pItemInfoD->itemType==VIEW_CASE)||
		(pItemInfoS->itemType==VIEW_DEFINEDFOLDER&&pItemInfoD->itemType==VIEW_DEFINEDFOLDER))
	{	//ͬ��ڵ�֮������Ϸ�
		if(pViewD==NULL)
			return FALSE;
		if(pViewS->m_iViewType==pViewD->m_iViewType)
			return TRUE;
	}
	return FALSE;
}

static int CompareDataLevel(CTowerTreeCtrl *pTreeCtrl, HTREEITEM hItemDragS, HTREEITEM hItemDragD)
{
	if(hItemDragS==NULL||hItemDragD==NULL)
		return 0;
	TREEITEM_INFO *pItemInfoS=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItemDragS);
	TREEITEM_INFO *pItemInfoD=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItemDragD);
	if(pItemInfoS==NULL||pItemInfoD==NULL)
		return 0;
	if(pItemInfoS->itemType==pItemInfoD->itemType)
	{	//��ʱ�ڴ˴��޸�
		if(pItemInfoS->itemType==VIEW_CASE&&pItemInfoD->itemType==VIEW_CASE)
		{	//��һ����ͼ�Ϸŵ���һ���ļ��е�ͬʱӦ���޸���ͼ�����ļ��е�����
			CDisplayView *pViewS=(CDisplayView*)pItemInfoS->dwRefData;
			CDisplayView *pViewD=(CDisplayView*)pItemInfoD->dwRefData;
			if(pViewS&&pViewD)
				strcpy(pViewS->folderName,pViewD->folderName);
		}
		return 0;
	}
	else if((pItemInfoS->itemType==VIEW_CASE&&pItemInfoD->itemType==VIEW_DEFINEDFOLDER)||
			(pItemInfoS->itemType==VIEW_CASE&&pItemInfoD->itemType==VIEW_DEFINEDSET))
	{	//��ʱ�ڴ˴��޸� 
		//�Ϸ���ͼ���ļ����»���ļ����ϳ�����Ҫ�޸�������� ���򱣴治�������޸� 
		if(pItemInfoS->itemType==VIEW_CASE&&pItemInfoD->itemType==VIEW_DEFINEDFOLDER)
		{	//��һ����ͼ�Ϸŵ�һ���ļ��е�ͬʱӦ���޸���ͼ�����ļ��е�����
			CDisplayView *pViewS=(CDisplayView*)pItemInfoS->dwRefData;
			CDisplayView *pViewD=(CDisplayView*)pItemInfoD->dwRefData;
			if(pViewS&&pViewD)
				strcpy(pViewS->folderName,pViewD->folderName);
		}
		else if(pItemInfoS->itemType==VIEW_CASE&&pItemInfoD->itemType==VIEW_DEFINEDSET)
		{	//����ͼ�ϵ���Ŀ¼�� �ļ��������ÿ�
			CDisplayView *pViewS=(CDisplayView*)pItemInfoS->dwRefData;
			if(pViewS)
				strcpy(pViewS->folderName,"");
		}
		return 1;
	}
	return 0;
}

// CTowerTreeDlg dialog
IMPLEMENT_DYNCREATE(CTowerTreeDlg, CDialog)

CTowerTreeDlg::CTowerTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTowerTreeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTowerTreeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hDataRootItem=m_hActiveModuleItem=m_hActiveBlockItem=m_hActiveViewItem=NULL;
	m_hServerContentItem=m_hBodyItem=NULL;
	m_hLayerViewItem=NULL;
}


void CTowerTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTowerTreeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
		DDX_Control(pDX, IDC_TREE_CTRL, m_treeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTowerTreeDlg, CDialog)
	//{{AFX_MSG_MAP(CTowerTreeDlg)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_CTRL, OnDblclkTreeCtrl)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CTRL, OnRclickTreeCtrl)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_CTRL, OnExpandingTreeCtrl)
	ON_COMMAND(ID_EDIT_ITEM_PROP_ITEM, OnEditItemPropItem)
	ON_COMMAND(ID_SET_ACTIVE_ITEM, OnSetActiveItem)
	ON_COMMAND(ID_COPY_ITEM, OnCopyItem)
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_NEW_ITEM_FOLDER, OnNewItemFolder)
	ON_COMMAND(ID_RETRIEVE_UDF_VIEW, OnRetrieveUdfViews)
	ON_COMMAND(ID_RETRIEVE_DRAWING_VIEW, OnRetrieveDrawingViews)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	ON_COMMAND(ID_REFRESH_TREE, OnRefreshTree)
#ifdef __RAPID_ROUGH_3DMODEL_
	ON_COMMAND(ID_INTELLI_CORRECT_OBJ_LAYER,OnIntelliCorrectModelObjsLayer)
#endif
#ifdef __COMMON_PART_INC_
	ON_COMMAND(ID_UNIFY_ANGLE_BOLT_G,OnUnifyAngleBoltsParamG)
#endif
#if defined(__SMART_DRAW_INC_)
	ON_COMMAND(ID_GENERATE_ROUGH_3D_MODEL,OnRelayoutRodModel)
#endif
//#ifdef __PART_DESIGN_INC_
	ON_COMMAND(ID_SORT_MODULE_BY_HEIGHT,OnSortModuleByHeight)
	ON_COMMAND(ID_SET_HEIGHT_GROUP_NAME,OnSetModuleNameByHeight)
//#endif
	ON_COMMAND(ID_MODEL_DESIGN_CAL, OnModelDesignCal)
	ON_COMMAND(ID_MODEL_EQ_CHECK, OnModelEqCheck)
	ON_COMMAND(IDD_ATTACH_WINDLOAD, OnAttachWindload)
	ON_COMMAND(ID_CAL_WIND_BETA_Z, OnCalWindBetaZ)
	ON_NOTIFY(TVN_KEYDOWN,IDC_TREE_CTRL, OnKeydownTreeCtrl)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CTRL, OnSelchangedTreeCtrl)
	ON_COMMAND(ID_SORT_ITEM, OnSortItem)
	ON_COMMAND(ID_HANDOVER_PERMISSION, OnHandoverPermission)
	ON_COMMAND(ID_ITEM_LIST_PROP, OnItemListProp)
	//ON_COMMAND(ID_NODELOAD_LIST, OnNodeLoadList)
	//ON_COMMAND(ID_NEW_ITEM_SET, OnNewItemSet)
	//ON_COMMAND(ID_CAL_WIRE_LOAD, OnCalWireLoad)
	ON_COMMAND(ID_RETRIEVE_BODY_WIND_SEGMENT, OnRetrieveBodyWindSegments)
	ON_COMMAND(ID_EXPORT_LOADCAL_REPORT, OnExportLoadcalReport)
	ON_COMMAND(ID_LAYERVIEW_SEG_PROP_ITEM, OnLayerviewSegPropItem)
	ON_COMMAND(ID_IMPORT_BLOCK_FILE, OnImportBlockFile)
	ON_COMMAND(ID_EXPORT_BLOCK_FILE, OnExportBlockFile)
	ON_COMMAND(ID_UPDATE_BLOCK,OnUpdateBlock)
	ON_COMMAND(ID_EXPLODE_BLOCKREF,OnExplodedBlockRef)
	ON_COMMAND(ID_BLOCKREF_INTEGRATE,OnIntegrateBlockRef)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTowerTreeDlg message handlers

//ѡ��ָ���ڵ�ʱ���Խڵ㴥���Ĳ�������
static int _FireSelectItem(CTowerTreeCtrl *pTreeCtrl, HTREEITEM hItem)
{
	CLDSView* pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	TREEITEM_INFO* pInfo=hItem!=NULL?(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem):NULL;
	if(pInfo&&pInfo->itemType==BLOCK_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
	{	//��������
		if(pInfo->dwRefData!=0)
		{
			pLdsView->m_pSelectBlock=(CBlockModel*)pInfo->dwRefData;
			pLdsView->DisplayBlockModelProperty(TRUE);
		}
		return TRUE;
	}
	else if(pInfo&&pInfo->itemType==BLOCKREF_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
	{	//����װ���������
		if(pInfo->dwRefData!=0)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			pLdsView->UpdatePropertyPage((CBlockReference*)pInfo->dwRefData);
		}
		return TRUE;
	}
	else
	{
		CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
		CPropertyList *pPropList = pPropDlg->GetPropertyList();
		if(pPropList->m_nObjClassTypeId!=CLS_BLOCK&&pPropList->m_nObjClassTypeId!=CLS_BLOCKREF)
			return 0;	
		long *id_arr=NULL;
		int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
		if(n>0)
			pLdsView->UpdatePropertyPage();
	}
	return 0;
}

BOOL CTowerTreeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	RECT rc;
	GetClientRect(&rc);
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	if(pTreeCtrl)
		pTreeCtrl->MoveWindow(&rc);
	pTreeCtrl->SetSelectItemFunc(_FireSelectItem);
	m_ModelImages.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	pTreeCtrl->SetImageList(&m_ModelImages,TVSIL_NORMAL);
	pTreeCtrl->ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS);
	pTreeCtrl->m_dropTarget.Register(pTreeCtrl);
	m_treeCtrl.SetDragOverFunc(DragOverFunc);
	m_treeCtrl.SetDropFunc(DropFunc);
	m_treeCtrl.SetIsValidDragSFunc(IsValidDragS);
	m_treeCtrl.SetIsValidDragDFunc(IsValidDragD);
	m_treeCtrl.SetCompareDataLevelFunc(CompareDataLevel);
	RefreshTreeCtrl();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

TREEITEM_INFO* CTowerTreeDlg::InsertOrUpdateItemInfo(TREEITEM_INFO& iteminfo)
{
	UINT id=iteminfo.dwRefData>0?iteminfo.dwRefData:iteminfo.itemType;
	//TODO:���ӷ���ز�����������
	TREEITEM_INFO* pItemInfo=NULL;//hashItemInfos.GetValue(id);
	if(pItemInfo==NULL)
	{
		pItemInfo=itemInfoList.append(iteminfo);
		//pItemInfo=hashItemInfos.SetValue(id,pItemInfo);
	}
	else
		*pItemInfo=iteminfo;
	return pItemInfo;
}
CTowerTreeCtrl *CTowerTreeDlg::GetTreeCtrl()
{
	return &m_treeCtrl;
}

void CTowerTreeDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	CWnd *pWnd = GetTreeCtrl();
	if(pWnd->GetSafeHwnd())
		pWnd->MoveWindow(0,0,cx,cy);
}
#ifdef __PART_DESIGN_INC_
static int LocalCompareNodePointI(const CLDSNodePtr& node1,const CLDSNodePtr& node2)
{
	if(node1->point_i>node2->point_i)
		return 1;
	else if(node1->point_i<node2->point_i)
		return -1;
	else
		return 0;
}
HTREEITEM CTowerTreeDlg::InsertWorkStatusItem(CWorkStatus *pStatus,HTREEITEM hParentItem,HTREEITEM hInsertAfterItem)
{
	CString ss;
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	ss.Format("%s(%d)",pStatus->description,pStatus->iNo);
	HTREEITEM hLoadItem=pTreeCtrl->InsertItem(ss,
		PRJ_IMG_STATUS,PRJ_IMG_STATUS,hParentItem);
	TREEITEM_INFO* pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_CASE,(DWORD)pStatus));
	pTreeCtrl->SetItemData(hLoadItem,(DWORD)pItemInfo);
	if(m_iCurStatusNo==pStatus->iNo)	//��ǰ�״̬����
		pTreeCtrl->SetItemState(hLoadItem,TVIS_BOLD,TVIS_BOLD);
	ss.Format("�ɱ�������ϵ��=%.2f",pStatus->Kesai());
	HTREEITEM hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hLoadItem);
	ss.Format("���ú����鷶Χ=%s",pStatus->limit_str);
	hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hLoadItem);
	pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_SCOPE,0));
	pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);

	ss.Format("�����������=%.2fmm",pStatus->ice_thick);
	hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hLoadItem);
	pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_ICE_THICK,0));
	pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);
	hSubItem=pTreeCtrl->InsertItem("���к���",
		PRJ_IMG_POINTLOAD,PRJ_IMG_POINTLOAD,hLoadItem);
	pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_POINTLOAD,0));
	pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);
	CLDSNode *pNode;
	ARRAY_LIST<CLDSNodePtr> loadnodes(0,8);
	for(pNode=Ta.Node.GetFirst();pNode;pNode=pNode=Ta.Node.GetNext())
	{
		CExternalNodeLoad *pLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
		if(pLoad==NULL)
			continue;	//����Ӻ���
		GEPOINT Fxyz=pLoad->Fxyz;
		if(pStatus->m_bLoadIncFactor)
			Fxyz=pLoad->designLoad;
		if(Fxyz.IsZero())
			continue;	//����Ӻ���
		loadnodes.append(pNode);
	}
	//��ǰ���ڵ�����С��������
	CQuickSort<CLDSNodePtr>::QuickSort(loadnodes.m_pData,loadnodes.GetSize(),LocalCompareNodePointI);
	for(int i=0;i<loadnodes.GetSize();i++)
	{
		pNode=loadnodes[i];
		CExternalNodeLoad *pLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
		//if(pLoad==NULL||pLoad->Fxyz.mod2()<EPS)
		//	continue;	//����Ӻ���
		ss.Format("0X%X�ڵ�(%d)",pNode->handle,pNode->point_i);
		HTREEITEM hSonItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOAD,PRJ_IMG_POINTLOAD,hSubItem);
		pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_POINTLOAD_CASE,(DWORD)pLoad));
		pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
		if(pStatus->m_bLoadIncFactor)
			ss.Format("X:%f",pLoad->designLoad.x/1000);
		else
			ss.Format("X:%f",pLoad->Fxyz.x/1000);
		SimplifiedNumString(ss);
		ss+="(kN)";
		pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOADITEM,PRJ_IMG_POINTLOADITEM,hSonItem);
		if(pStatus->m_bLoadIncFactor)
			ss.Format("Y:%f",pLoad->designLoad.y/1000);
		else
			ss.Format("Y:%f",pLoad->Fxyz.y/1000);
		SimplifiedNumString(ss);
		ss+="(kN)";
		pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOADITEM,PRJ_IMG_POINTLOADITEM,hSonItem);
		if(pStatus->m_bLoadIncFactor)
			ss.Format("Z:%f",pLoad->designLoad.z/1000);
		else
			ss.Format("Z:%f",pLoad->Fxyz.z/1000);
		SimplifiedNumString(ss);
		ss+="(kN)";
		pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOADITEM,PRJ_IMG_POINTLOADITEM,hSonItem);
		ss.Format("Z_2:%f",pLoad->permanentFz/1000);
		SimplifiedNumString(ss);
		ss+="(kN)";
		pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOADITEM,PRJ_IMG_POINTLOADITEM,hSonItem);
	}
	HTREEITEM hItem=pTreeCtrl->InsertItem("�����",PRJ_IMG_WINDLOAD,PRJ_IMG_WINDLOAD,hLoadItem);
	pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_WINDLOAD,0));
	pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
	ss.Format("����=%f",pStatus->WindLoad.velocity);
	SimplifiedNumString(ss);
	ss+="m/s";
	hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
	ss.Format("�����=%d��",pStatus->WindLoad.azimuth);
	hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
	ss.Format("���Ӻ���");
	hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_ATTACH_WINDLOAD,PRJ_IMG_ATTACH_WINDLOAD,hLoadItem);
	pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_ATTACH_WINDLOADREF,0));
	pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
	for(CWorkStatus::WINDLOAD_REFPARAM *pRefParam=pStatus->AttachWindLoad.GetFirst();pRefParam;pRefParam=pStatus->AttachWindLoad.GetNext())
	{
		CAttachWindLoad *pWindLoad=Ta.AttachWindLoad.FromHandle(pStatus->AttachWindLoad.GetCursorKey());
		if(pWindLoad)
		{
			ss.Format("%s(%d)",pWindLoad->name,pWindLoad->iNo);
			hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_ATTACH_WINDLOAD,PRJ_IMG_ATTACH_WINDLOAD,hItem);
			pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_ATTACH_WINDLOADREFITEM,(DWORD)pWindLoad));
			pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);
		}
	}
	//hItem=pTreeCtrl->InsertItem("�������",PRJ_IMG_EARTHQUAKE,PRJ_IMG_EARTHQUAKE,hLoadItem);
	//dwData=TREE_ITEM_WORKSTATUS+WORKSTATUS_EARTHQUAKE;
	//pTreeCtrl->SetItemData(hItem,dwData);
	return hLoadItem;
}
#endif

void DeleteAllSubItems(CTreeCtrl* pTreeCtrl,HTREEITEM hParentItem)
{
	if (pTreeCtrl->ItemHasChildren(hParentItem))
	{
		HTREEITEM hChildItem = pTreeCtrl->GetChildItem(hParentItem);
		while (hChildItem != NULL)
		{
			HTREEITEM hNextItem = pTreeCtrl->GetNextItem(hChildItem, TVGN_NEXT);
			pTreeCtrl->DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}
}

void UpdateDisplayViewFlag(CDisplayView *pView)
{
#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
	CLDSView *pWorkView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
#elif defined(__LDS_GROUP_CONTEXT_)// defined(__LDSGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)
	CTmaGroupView *pWorkView=(CTmaGroupView*)theApp.GetTmaGroupDoc()->GetView(RUNTIME_CLASS(CTmaGroupView));
#endif
	if(pView==NULL)
		pView=console.GetActiveView();
	if(pView==NULL||pWorkView==NULL)
		return;
	//������ͼ��־ wht 08-02-21
	if(((strchr(pView->filter.ThirdCharSet,'Q')||strchr(pView->filter.ThirdCharSet,'q'))&&
		(strchr(pView->filter.ThirdCharSet,'H')||strchr(pView->filter.ThirdCharSet,'h'))&&
		(strchr(pView->filter.ThirdCharSet,'Z')||strchr(pView->filter.ThirdCharSet,'z'))&&
		(strchr(pView->filter.ThirdCharSet,'Y')||strchr(pView->filter.ThirdCharSet,'y')))||
		strchr(pView->filter.ThirdCharSet,'?'))
		pWorkView->m_eViewFlag = PERSPECTIVE;
	else
	{
		if(strchr(pView->filter.ThirdCharSet,'Q') || strchr(pView->filter.ThirdCharSet,'q'))
			pWorkView->m_eViewFlag = FRONT_VIEW;
		else if(strchr(pView->filter.ThirdCharSet,'H') || strchr(pView->filter.ThirdCharSet,'h'))
			pWorkView->m_eViewFlag = BACK_VIEW;
		else if(strchr(pView->filter.ThirdCharSet,'Z') || strchr(pView->filter.ThirdCharSet,'z'))
			pWorkView->m_eViewFlag = LEFT_VIEW;
		else if(strchr(pView->filter.ThirdCharSet,'Y') || strchr(pView->filter.ThirdCharSet,'y'))
			pWorkView->m_eViewFlag = RIGHT_VIEW;
	}
}
void CTowerTreeDlg::RefreshViewGroup(HTREEITEM hViewGroupItem,IModel* pModel)
{
	CString ss;
	HTREEITEM hUdfViewItem,hItem;
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	DeleteAllSubItems(pTreeCtrl,hViewGroupItem);	//���ԭ���Ӽ����ڵ�
#ifdef AFX_TARG_ENU_ENGLISH
	m_hLayerViewItem = pTreeCtrl->InsertItem("Layer View",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hViewGroupItem);
#else 
	m_hLayerViewItem = pTreeCtrl->InsertItem("ͼ����ͼ",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hViewGroupItem);
#endif
	TREEITEM_INFO *pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_LAYERSET,0));
	pTreeCtrl->SetItemData(m_hLayerViewItem,(DWORD)pItemInfo);
#ifdef AFX_TARG_ENU_ENGLISH
	hUdfViewItem = pTreeCtrl->InsertItem("Custom View",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hViewGroupItem);
#else 
	hUdfViewItem = pTreeCtrl->InsertItem("������ͼ",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hViewGroupItem);
#endif
	pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_DEFINEDSET,0));
	pTreeCtrl->SetItemData(hUdfViewItem,(DWORD)pItemInfo);
	pTreeCtrl->Expand(hViewGroupItem,TVE_EXPAND);
	for(CDisplayView *pView=pModel->EnumViewFirst();pView;pView=pModel->EnumViewNext())
	{	//���������б�ǰ ������ɵ�����Ϣ��
		if(pView->m_iViewType==2)
			console.DeleteDisplayView(pView->handle);
	}
	for(pView=pModel->EnumViewFirst();pView;pView=pModel->EnumViewNext())
	{
		ss.Format("%s",pView->name);
		if(pView->m_iViewType==0)		//ͼ����ͼ
			hItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,m_hLayerViewItem);
		else if(pView->m_iViewType==1)	//������ͼ
		{
			HTREEITEM hFolderItem=NULL;
			CString sFolderName=pView->folderName;
			for(pItemInfo=itemInfoList.GetFirst();pItemInfo;pItemInfo=itemInfoList.GetNext())
			{	//�����Ƿ��Ѿ�������ͬ�Ķ�����ͼ�ļ���
				CDisplayView *pFolder=(CDisplayView*)pItemInfo->dwRefData;
				if(pFolder==NULL)
					continue;
				if(pItemInfo->itemType==VIEW_DEFINEDFOLDER&&sFolderName.CompareNoCase(pFolder->folderName)==0)
					break;
			}
			if(pItemInfo)
			{	//�����ڣ������ڱ��ļ��еĶ�����ͼ��Ϊ���������
				HTREEITEM hTempItem=m_treeCtrl.GetChildItem(hUdfViewItem);
				while(hTempItem)
				{
					TREEITEM_INFO *pItem=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hTempItem);
					if(pItem&&pItem==pItemInfo)
					{
						hFolderItem=hTempItem;
						break;
					}
					hTempItem=m_treeCtrl.GetNextItem(hTempItem,TVGN_NEXT);
				}
			}
			else if(sFolderName.GetLength()>0)
			{	//�������ڣ��½���
				Ta.View.push_stack();
				//ÿ��������ͼ������ͼ�б��ж���Ӧһ����ͼ����������ͼ����������ʱ��������
				//����������ڸ���Ķ�����ͼ����λ��,���Ա�����ʱ���޷��Ը�����в���
				CDisplayView *pViewFolder = Ta.AppendDisplayView();
				pViewFolder->m_iViewType=2;	//�½�������ͼ��ʱʹ�� ���洢
				strcpy(pViewFolder->folderName,pView->folderName);
				Ta.View.pop_stack();
				hFolderItem = pTreeCtrl->InsertItem(pViewFolder->folderName,PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hUdfViewItem);
				pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_DEFINEDFOLDER,(DWORD)pViewFolder));
				pTreeCtrl->SetItemData(hFolderItem,(DWORD)pItemInfo);
			}
			if(hFolderItem)
				hItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hFolderItem);
			else
				hItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hUdfViewItem);
		}
		else if(pView->m_iViewType==2)
			continue;
		else
		{
			if(pView==console.GetActiveView())
				console.SetActiveView(0);
			console.DeleteDisplayView(pView->handle);
			hItem=NULL;
		}
		if(pView==console.GetActiveView())	//��ǰ�״̬��ͼ
		{
			//����Ϊ�Ӵָ�ʽ
			ShiftActiveItemState(hItem,ACTIVE_DISPVIEW);
			pView=console.SetActiveView(pView->handle);
			//������ͼ��־ wht 15-03-10
			UpdateDisplayViewFlag(pView);
#ifdef __LDS_GROUP_CONTEXT_//defined(__TMA_)||defined(__LMA_)||defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
			strcpy(theApp.env.layer,pView->defaultLayer);
#endif
		}
		if(hItem)
		{
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_CASE,(DWORD)pView));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
		}
	}
	if(pModel->IsTowerModel())
	{
		pTreeCtrl->Expand(m_hLayerViewItem,TVE_EXPAND);
		pTreeCtrl->Expand(hUdfViewItem,TVE_EXPAND);
	}
}
void CTowerTreeDlg::ShiftActiveItemState(HTREEITEM hActiveItem,int nType)
{
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	if(nType==ACTIVE_MODULE||nType==ACTIVE_DISPVIEW)//���߼���,��ͼ����
	{
		HTREEITEM hObjSetItem=m_treeCtrl.GetParentItem(hActiveItem);
		HTREEITEM hSonItem=m_treeCtrl.GetChildItem(hObjSetItem);
		while(hSonItem)
		{
			if(hSonItem==hActiveItem)
				m_treeCtrl.SetItemState(hSonItem,TVIS_BOLD,TVIS_BOLD);
			else
				m_treeCtrl.SetItemState(hSonItem,0,TVIS_BOLD);
			hSonItem=m_treeCtrl.GetNextSiblingItem(hSonItem);
		}
		if(nType==ACTIVE_MODULE)
		{
			m_hActiveModuleItem=hActiveItem;
			if(m_hActiveBlockItem!=NULL)
			{
				pTreeCtrl->SetItemState(m_hActiveBlockItem,0,TVIS_BOLD);
				m_hActiveBlockItem=NULL;
			}
		}
		else if(nType==ACTIVE_DISPVIEW)
			m_hActiveViewItem=hActiveItem;
	}
	else if(nType==ACTIVE_BLOCK)//��������
	{
		if(m_hActiveBlockItem!=NULL&&m_hActiveBlockItem!=hActiveItem)
			pTreeCtrl->SetItemState(m_hActiveBlockItem,0,TVIS_BOLD);
		if(m_hActiveBlockItem!=hActiveItem)
		{
			pTreeCtrl->SetItemState(hActiveItem,TVIS_BOLD,TVIS_BOLD);
			m_hActiveBlockItem=hActiveItem;
		}
		else
			pTreeCtrl->SetItemState(m_hActiveBlockItem,TVIS_BOLD,TVIS_BOLD);
		if(m_hActiveModuleItem!=NULL)
		{
			pTreeCtrl->SetItemState(m_hActiveModuleItem,0,TVIS_BOLD);
			m_hActiveModuleItem=NULL;
		}
	}
	pTreeCtrl->RedrawWindow();
}
//���������򲿼�����Ҷ�Ӧ����ͼ��
static HTREEITEM SearchViewItem(CTreeCtrl *pTreeCtrl,HTREEITEM hModelItem)
{
	HTREEITEM hItem=pTreeCtrl->GetChildItem(hModelItem);
	while(hItem!=NULL)
	{
		TREEITEM_INFO *pItemInfo=NULL;
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
		if(pItemInfo&&pItemInfo->itemType==VIEW_CASE)
		{
			CDisplayView *pView=(CDisplayView*)pItemInfo->dwRefData;
			if(pView==console.GetActiveView())
				return hItem;
		}
		if(pTreeCtrl->ItemHasChildren(hItem))
		{
			HTREEITEM hViewItem=SearchViewItem(pTreeCtrl,hItem);
			if(hViewItem)
				return hViewItem;
		}
		hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
	}
	return NULL;
}
//���Һ���ǰ������߶�Ӧ��Item
static HTREEITEM SearchModuleItem(CTreeCtrl *pTreeCtrl)
{
	HTREEITEM hRootItem=pTreeCtrl->GetRootItem();
	HTREEITEM hTaItem=pTreeCtrl->GetChildItem(hRootItem);
	HTREEITEM hModuleItem=pTreeCtrl->GetChildItem(hTaItem);
	while(hModuleItem)
	{
		TREEITEM_INFO *pItemInfo=NULL;
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hModuleItem);
		if(pItemInfo && pItemInfo->itemType==MODEL_CASE)
		{
			CLDSModule *pModule=(CLDSModule*)pItemInfo->dwRefData;
			if(pModule->handle==Ta.m_hActiveModule)
				return hModuleItem;
		}
		hModuleItem=pTreeCtrl->GetNextItem(hModuleItem,TVGN_NEXT);
	}
	return NULL;
}
static HTREEITEM FindTreeNodeByAttachData(CTowerTreeCtrl *pTreeCtrl,HTREEITEM hItem,TREEITEM_INFO &itemInfo)
{
	HTREEITEM  hChildItem = pTreeCtrl->GetChildItem(hItem);
	HTREEITEM hFindItem = NULL;
	TREEITEM_INFO *pInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pInfo&&pInfo->itemType==itemInfo.itemType&&itemInfo.dwRefData==NULL)
		hFindItem=hItem;
	else if(itemInfo.dwRefData!=NULL&&(pInfo&&pInfo->itemType==itemInfo.itemType&&pInfo->dwRefData==itemInfo.dwRefData))
		hFindItem=hItem;
	else
	{
		while(hChildItem!=NULL)
		{
			hFindItem=FindTreeNodeByAttachData(pTreeCtrl,hChildItem,itemInfo);
			if(hFindItem)
				break;
			hChildItem = pTreeCtrl->GetNextSiblingItem(hChildItem);
		}
	}
	return hFindItem;
}

HTREEITEM CTowerTreeDlg::SearchTreeItem(TREEITEM_TYPE itemType,CLDSObject* pObj/*=NULL*/)
{
	HTREEITEM hRootItem=m_treeCtrl.GetRootItem();
	return FindTreeNodeByAttachData(&m_treeCtrl,hRootItem,TREEITEM_INFO(itemType,(DWORD)pObj));
}
//У��ʱ�л����ߣ�����TTA�ļ�ʱȱ�ٻ�׼ͼԪ�������׼�棬��������
static void RemoveObsoleteSiblingTreeItems(CTowerTreeCtrl *pTreeCtrl,HTREEITEM hStartObsoleteItem)
{	//�Ƴ���ʱ�����ڵ�
	HTREEITEM hNextItem=NULL,hItem=hStartObsoleteItem;
	while(hItem)
	{
		hNextItem=pTreeCtrl->GetNextSiblingItem(hItem);
		pTreeCtrl->DeleteItem(hItem);
		hItem=hNextItem;
	}
}
bool CTowerTreeDlg::UpdateTreeItemByType(TREEITEM_TYPE itemType,CLDSObject* pBoldFontObj/*=NULL*/)
{
	HTREEITEM hObjSetItem=SearchTreeItem(itemType);
	if(hObjSetItem==NULL)
		return false;
	CTowerTreeCtrl* pTreeCtrl=GetTreeCtrl();
	TREEITEM_INFO* pItemInfo;
	char strdata[64]={0};
	CXhString xhstr(strdata,64);
	if(itemType==MODEL_DATUM_POINT_GROUP)
	{	//���»�׼�㼯�Ͻڵ�
		//����Ta.Point���ϸ���hObjSetItem�µ��ӽڵ㣨֮ǰ�и��£�֮ǰû����ӣ�֮ǰ����û���Ƴ�)
		CLDSPoint *pPoint=Ta.EnumPointFirst();
		HTREEITEM hSonItem=pTreeCtrl->GetChildItem(hObjSetItem);
		do{
			xhstr.Printf("%s_0X%X",pPoint->description,pPoint->handle);//,pPoint->iNo);
			if(hSonItem==NULL)
				hSonItem=pTreeCtrl->InsertItem(xhstr,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hObjSetItem);
			else
				pTreeCtrl->SetItemText(hSonItem,xhstr);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pPoint));
			pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
			hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
			pPoint=Ta.EnumPointNext();
		}while(pPoint);
		RemoveObsoleteSiblingTreeItems(pTreeCtrl,hSonItem);//�Ƴ���ʱ�����ڵ�
		return true;
	}
	else if(itemType==MODEL_DATUM_LINE_GROUP)
	{	//���»�׼�߼��Ͻڵ�
		//����Ta.Line���ϸ���hObjSetItem�µ��ӽڵ㣨֮ǰ�и��£�֮ǰû����ӣ�֮ǰ����û���Ƴ�)
		CLDSLine* pLine=Ta.EnumLineFirst();
		HTREEITEM hSonItem=pTreeCtrl->GetChildItem(hObjSetItem);
		do{
			xhstr.Printf("%s_0X%X",pLine->description,pLine->handle);//,pPoint->iNo);
			if(hSonItem==NULL)
				hSonItem=pTreeCtrl->InsertItem(xhstr,PRJ_IMG_DATUM_LINE,PRJ_IMG_DATUM_LINE,hObjSetItem);
			else
				pTreeCtrl->SetItemText(hSonItem,xhstr);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_LINE_CASE,(DWORD)pLine));
			pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
			hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
			pLine=Ta.EnumLineNext();
		}while(pLine);
		RemoveObsoleteSiblingTreeItems(pTreeCtrl,hSonItem);//�Ƴ���ʱ�����ڵ�
		return true;
	}
	else if(itemType==MODEL_DATUM_PLANE_GROUP)
	{	//���»�׼�漯�Ͻڵ�
		//����Ta.Plane���ϸ���hObjSetItem�µ��ӽڵ㣨֮ǰ�и��£�֮ǰû����ӣ�֮ǰ����û���Ƴ�)
		CLDSPlane* pPlane=Ta.EnumPlaneFirst();
		HTREEITEM hSonItem=pTreeCtrl->GetChildItem(hObjSetItem);
		do{
			xhstr.Printf("%s_0X%X",pPlane->description,pPlane->handle);//,pPoint->iNo);
			if(hSonItem==NULL)
				hSonItem=pTreeCtrl->InsertItem(xhstr,PRJ_IMG_DATUM_LINE,PRJ_IMG_DATUM_LINE,hObjSetItem);
			else
				pTreeCtrl->SetItemText(hSonItem,xhstr);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_PLANE_CASE,(DWORD)pPlane));
			pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
			hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
			pPlane=Ta.EnumPlaneNext();
		}while(pPlane);
		RemoveObsoleteSiblingTreeItems(pTreeCtrl,hSonItem);//�Ƴ���ʱ�����ڵ�
		return true;
	}
	else if(itemType==MODEL_ARCLIFT_GROUP)
	{	//���»�׼�㼯�Ͻڵ�
		//TODO:����Ta.Point���ϸ���hObjSetItem�µ��ӽڵ㣨֮ǰ�и��£�֮ǰû����ӣ�֮ǰ����û���Ƴ�)
		CLDSArcLift* pArcLift=Ta.ArcLift.GetFirst();
		HTREEITEM hSonItem=pTreeCtrl->GetChildItem(hObjSetItem);
		do{
			xhstr.Printf("%s(%d)",pArcLift->name,pArcLift->iNo);;//,pPoint->iNo);
			if(hSonItem==NULL)
				hSonItem=pTreeCtrl->InsertItem(xhstr,PRJ_IMG_ARC_LIFT_CASE,PRJ_IMG_ARC_LIFT_CASE,hObjSetItem);
			else
				pTreeCtrl->SetItemText(hSonItem,xhstr);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_ARCLIFT_CASE,(DWORD)pArcLift));
			pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
			hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
			pArcLift=Ta.ArcLift.GetNext();
		}while(pArcLift);
		RemoveObsoleteSiblingTreeItems(pTreeCtrl,hSonItem);//�Ƴ���ʱ�����ڵ�
		return true;
	}
	else if(itemType==VIEW_GROUP||itemType==VIEW_LAYERSET||itemType==VIEW_DEFINEDSET)
	{	//������ͼ���Ͻڵ�
		//TODO:����Ta.View���ϸ���hObjSetItem�µ��ӽڵ㣨֮ǰ�и��£�֮ǰû����ӣ�֮ǰ����û���Ƴ�)
		//��Ҫ�Ӵֵ�ǰ������ͼ��VIEW_GROUPʱ����ݹ���ø���UpdateTreeItemByType(VIEW_LAYERSET)��UpdateTreeItemByType(VIEW_DEFINEDSET)
		return false;
	}
	else if(itemType==MODEL_GROUP)
	{	//���»�׼�㼯�Ͻڵ�
		//����Ta.Module���ϸ���hObjSetItem�µ��ӽڵ㣨֮ǰ�и��£�֮ǰû����ӣ�֮ǰ����û���Ƴ�)
		CLDSModule* pModule=Ta.Module.GetFirst();
		HTREEITEM hSonItem=pTreeCtrl->GetChildItem(hObjSetItem);
		HTREEITEM hBoldFontItem=NULL;
		do{
			xhstr.Printf("%s",pModule->description);
			if(hSonItem==NULL)
				hSonItem=pTreeCtrl->InsertItem(xhstr,PRJ_IMG_MODULECASE,PRJ_IMG_MODULECASE,hObjSetItem);
			else
				pTreeCtrl->SetItemText(hSonItem,xhstr);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_CASE,(DWORD)pModule));
			pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
			RefreshHeightGroupItem(hSonItem);
			if(pModule==pBoldFontObj)
				hBoldFontItem=hSonItem;
			hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
			pModule=Ta.Module.GetNext();
		}while(pModule);
		RemoveObsoleteSiblingTreeItems(pTreeCtrl,hSonItem);//�Ƴ���ʱ�����ڵ�
		if(hBoldFontItem!=NULL)
		{	//��Ҫ�Ӵֵ�ǰ�������
			hSonItem=pTreeCtrl->GetChildItem(hObjSetItem);
			while(hSonItem)
			{
				if(hSonItem==hBoldFontItem)
					pTreeCtrl->SetItemState(hSonItem,TVIS_BOLD,TVIS_BOLD);
				else
					pTreeCtrl->SetItemState(hSonItem,0,TVIS_BOLD);
				hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
			}
		}
		return true;
	}
#ifdef __PART_DESIGN_INC_
	else if(itemType==WORKSTATUS_GROUP)
	{	//���¹������Ͻڵ�
		//����Ta.WorkStatus���ϸ���hObjSetItem�µ��ӽڵ㣨֮ǰ�и��£�֮ǰû����ӣ�֮ǰ����û���Ƴ�)
		CWorkStatus* pStatus=Ta.WorkStatus.GetFirst();
		HTREEITEM hSonItem=pTreeCtrl->GetChildItem(hObjSetItem);
		HTREEITEM hBoldFontItem=NULL;
		do{
			xhstr.Printf("%s(%d)",pStatus->description,pStatus->iNo);
			if(hSonItem==NULL)
				hSonItem=pTreeCtrl->InsertItem(xhstr,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hObjSetItem);
			else
				pTreeCtrl->SetItemText(hSonItem,xhstr);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_CASE,(DWORD)pStatus));
			pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
			//TODO:��Ҫ����RefreshWorkCaseItem(hSonItem);
			if(pStatus==pBoldFontObj)
				hBoldFontItem=hSonItem;
			hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
			pStatus=Ta.WorkStatus.GetNext();
		}while(pStatus);
		RemoveObsoleteSiblingTreeItems(pTreeCtrl,hSonItem);//�Ƴ���ʱ�����ڵ�
		if(hBoldFontItem!=NULL)
		{	//��Ҫ�Ӵֵ�ǰ�����
			hSonItem=pTreeCtrl->GetChildItem(hObjSetItem);
			while(hSonItem)
			{
				if(hSonItem==hBoldFontItem)
					pTreeCtrl->SetItemState(hSonItem,TVIS_BOLD,TVIS_BOLD);
				else
					pTreeCtrl->SetItemState(hSonItem,0,TVIS_BOLD);
				hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
			}
		}
		return true;
	}
#endif
	return false;
}
bool CTowerTreeDlg::ShiftActiveItemByObj(CLDSObject* pObj)
{
	TREEITEM_TYPE itemType=MODEL_GROUP;
	if(pObj->GetClassTypeId()==CLS_HEIGHTMODULE)
		itemType=MODEL_CASE;
	//else if (pObj->GetClassTypeId()==CLS_DISPLAYVIEW)
	//	itemType = VIEW_DEFINEDFOLDER;
	//else if (pObj->GetClassTypeId()==CLS_GEPOINT)
	//	itemType=MODEL_DATUM_POINT_CASE;
	//else if (pObj->GetClassTypeId()==CLS_GELINE)
	//	itemType=MODEL_DATUM_LINE_CASE;
	//else if (pObj->GetClassTypeId()==CLS_GEPLANE)
	//	itemType=MODEL_DATUM_PLANE_CASE;
	//else if (pObj->GetClassTypeId()==CLS_ARCLIFT)
	//	itemType=MODEL_ARCLIFT_CASE;
	//else if (pObj->GetClassTypeId()==CLS_BLOCKREF)
	//	itemType=BLOCKREF_CASE;
	else if (pObj->GetClassTypeId()==CLS_WORKCASE)
		itemType=WORKSTATUS_CASE;
	else
		return false;
	HTREEITEM hObjItem=SearchTreeItem(itemType,pObj);
	if(hObjItem==NULL)
		return false;
	if(pObj->GetClassTypeId()==CLS_HEIGHTMODULE)
	{	//��Ҫ�Ӵֵ�ǰ�������
		HTREEITEM hObjSetItem=m_treeCtrl.GetParentItem(hObjItem);
		HTREEITEM hSonItem=m_treeCtrl.GetChildItem(hObjSetItem);
		while(hSonItem)
		{
			if(hSonItem==hObjItem)
				m_treeCtrl.SetItemState(hSonItem,TVIS_BOLD,TVIS_BOLD);
			else
				m_treeCtrl.SetItemState(hSonItem,0,TVIS_BOLD);
			hSonItem=m_treeCtrl.GetNextSiblingItem(hSonItem);
		}
		return true;
	}
	else if(pObj->GetClassTypeId()==CLS_WORKCASE)
	{	//��Ҫ�Ӵֵ�ǰ�������
		HTREEITEM hObjSetItem=m_treeCtrl.GetParentItem(hObjItem);
		HTREEITEM hSonItem=m_treeCtrl.GetChildItem(hObjSetItem);
		while(hSonItem)
		{
			if(hSonItem==hObjItem)
				m_treeCtrl.SetItemState(hSonItem,TVIS_BOLD,TVIS_BOLD);
			else
				m_treeCtrl.SetItemState(hSonItem,0,TVIS_BOLD);
			hSonItem=m_treeCtrl.GetNextSiblingItem(hSonItem);
		}
		return true;
	}
	return false;
}
bool CTowerTreeDlg::AddOrUpdateHeightItem(CLDSModule* pHeightModule,
	bool bEnusureVisible/* = false*/, bool bActivate/* = false*/)
{
	CTowerTreeCtrl* pTreeCtrl = &m_treeCtrl;
	HTREEITEM hHeightItem = SearchTreeItem(MODEL_CASE, pHeightModule);
	if (hHeightItem == NULL)
	{
		CString ss;
#ifdef __PART_DESIGN_INC_// defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
		ss.Format("%s(%d)", pHeightModule->description, pHeightModule->iNo);
#else
		ss.Format("%s", pHeightModule->description);
#endif
		HTREEITEM hHeightGroupItem = SearchTreeItem(MODEL_GROUP, NULL);
		hHeightItem = m_treeCtrl.InsertItem(ss, PRJ_IMG_MODULECASE, PRJ_IMG_MODULECASE, hHeightGroupItem);
		TREEITEM_INFO* pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_CASE, (DWORD)pHeightModule));
		m_treeCtrl.SetItemData(hHeightItem, (DWORD)pItemInfo);
	}
	this->RefreshHeightGroupItem(hHeightItem);
	if (bActivate)	//��ǰ�״̬ģ��
	{
		ShiftActiveItemState(hHeightItem, ACTIVE_MODULE);
		Ta.m_hActiveModule = pHeightModule->handle;
	}
	pTreeCtrl->Expand(hHeightItem, TVE_EXPAND);
	return true;
}
bool CTowerTreeDlg::AddOrUpdateTreeItemByObj(CLDSObject* pObj, bool bEnsureVisible/* = false*/, bool bActivate/* = false*/)
{
	if (pObj->GetClassTypeId() == CLS_HEIGHTMODULE)
	{
		return AddOrUpdateHeightItem((CLDSModule*)pObj, bEnsureVisible, bActivate);
	}
	/*else if (pObj->GetClassTypeId()==CLS_DISPLAYVIEW)
		itemType = VIEW_DEFINEDFOLDER;
	else if (pObj->GetClassTypeId()==CLS_GEPOINT)
		itemType=MODEL_DATUM_POINT_CASE;
	else if (pObj->GetClassTypeId()==CLS_GELINE)
		itemType=MODEL_DATUM_LINE_CASE;
	else if (pObj->GetClassTypeId()==CLS_GEPLANE)
		itemType=MODEL_DATUM_PLANE_CASE;
	else if (pObj->GetClassTypeId()==CLS_ARCLIFT)
		itemType=MODEL_ARCLIFT_CASE;
	else if (pObj->GetClassTypeId()==CLS_BLOCKREF)
		itemType=BLOCKREF_CASE;
	else if (pObj->GetClassTypeId()==CLS_WORKCASE)
		itemType=WORKSTATUS_CASE;*/
	else
		return false;

}
bool CTowerTreeDlg::UpdateTreeItemByObj(CLDSObject* pObj,bool bEnusureVisible/*=false*/,bool bSetBoldFont/*=false*/)
{
	TREEITEM_TYPE itemType=MODEL_GROUP;
	if(pObj->GetClassTypeId()==CLS_HEIGHTMODULE)
		itemType=MODEL_CASE;
	else if (pObj->GetClassTypeId()==CLS_DISPLAYVIEW)
		itemType = VIEW_DEFINEDFOLDER;
	else if (pObj->GetClassTypeId()==CLS_GEPOINT)
		itemType=MODEL_DATUM_POINT_CASE;
	else if (pObj->GetClassTypeId()==CLS_GELINE)
		itemType=MODEL_DATUM_LINE_CASE;
	else if (pObj->GetClassTypeId()==CLS_GEPLANE)
		itemType=MODEL_DATUM_PLANE_CASE;
	else if (pObj->GetClassTypeId()==CLS_ARCLIFT)
		itemType=MODEL_ARCLIFT_CASE;
	else if (pObj->GetClassTypeId()==CLS_BLOCKREF)
		itemType=BLOCKREF_CASE;
	else if (pObj->GetClassTypeId()==CLS_WORKCASE)
		itemType=WORKSTATUS_CASE;
	//else if (pObj->GetClassTypeId()==)			//TODO �����
		//itemType=WORKSTATUS_ATTACH_WINDLOADITEM;
	else
		return false;
	HTREEITEM hObjItem=SearchTreeItem(itemType,pObj);
	if(hObjItem==NULL)
		return false;
	else if(pObj->GetClassTypeId()==CLS_WORKCASE)
	{
		HTREEITEM hObjSetItem=m_treeCtrl.GetParentItem(hObjItem);
		HTREEITEM hSonItem=m_treeCtrl.GetChildItem(hObjSetItem);
		while(hSonItem)
		{
			if(hSonItem==hObjItem)
				m_treeCtrl.SetItemState(hSonItem,TVIS_BOLD,TVIS_BOLD);
			else
				m_treeCtrl.SetItemState(hSonItem,0,TVIS_BOLD);
			hSonItem=m_treeCtrl.GetNextSiblingItem(hSonItem);
		}
	}
	return false;
}
void CTowerTreeDlg::RefreshActiveItemStateAndActiveModel(IModel *pModel,HTREEITEM hItem)
{
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	//1.������Ҫ�Ӵ���ʾ����
	HTREEITEM hViewItem=NULL,hModuleItem=NULL;
	HTREEITEM hCurModelItem=pTreeCtrl->GetChildItem(m_hDataRootItem);	//Ĭ�ϼ���ģ����Ϊ����
	if(pItemInfo->itemType==VIEW_CASE)//��ͼ
	{
		hViewItem=hItem;
		if(!pModel->IsTowerModel()&&!((CBlockModel*)pModel)->IsEmbeded())
		{	//�����ͼ���ڵ���ʽ���������Ҽ���ģ����
			HTREEITEM hViewParentItem=pTreeCtrl->GetParentItem(hItem);
			for(int i=0;i<2;i++)
				hViewParentItem=pTreeCtrl->GetParentItem(hViewParentItem);
			TREEITEM_INFO* pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hViewParentItem);
			if(pItemInfo->itemType==VIEW_GROUP)
			{
				hViewParentItem=pTreeCtrl->GetParentItem(hViewParentItem);
				pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hViewParentItem);
			}
			if(pItemInfo->itemType==BLOCK_CASE)
				hCurModelItem=hViewParentItem;
		}
	}
	else if(pItemInfo->itemType==MODEL_CASE)
		hModuleItem=hItem;
	else if(pItemInfo->itemType==BLOCK_CASE)
		hCurModelItem=hItem;
	else if(pItemInfo->itemType!=BLOCKREF_CASE)
		return;
	//2.�Ӵ���ʾָ����
	if(!pModel->IsTowerModel()&&!((CBlockModel*)pModel)->IsEmbeded()&&hCurModelItem)
		ShiftActiveItemState(hCurModelItem,ACTIVE_BLOCK);		//�Ӵֲ�����
	else 
	{	//��ǰ����ģ��Ϊ����ʱ��������Ҫ����ĺ�����Ӵ�
		if(hModuleItem==NULL)	
			hModuleItem=SearchModuleItem(pTreeCtrl);			
		if(hModuleItem)
			ShiftActiveItemState(hModuleItem,ACTIVE_MODULE);	//�Ӵֺ�����
	}
	//�����ǰ��ͼ��ΪNULL����Ҫ���ݲ��ҵ�ǰ����ģ������Ҫ�Ӵֵ���ͼ��
	if(hViewItem==NULL)
		hViewItem=SearchViewItem(pTreeCtrl,hCurModelItem);
	ShiftActiveItemState(hViewItem,ACTIVE_DISPVIEW);			//�Ӵ���ͼ��
	//3.����ģ�ͣ���ΪActiveModel��ʹ��m_hActiveViewItem�õ���ǰ��������ͼ����Ӧ��m_hActiveViewItem����֮�����
	ActiveModel(pModel);
	//4.���m_hActiveViewItem==NULLʱ��ActiveModel()ʱ������һ��Ĭ�ϼ�����ͼ���ڴ˴���Ӵ���ʾĬ�ϼ�����ͼ��
	if(m_hActiveViewItem==NULL)
	{	//�Ӵ�Ĭ����ͼ��
		hViewItem=SearchViewItem(pTreeCtrl,hCurModelItem);
		ShiftActiveItemState(hViewItem,ACTIVE_DISPVIEW);			
	}
}
HTREEITEM CTowerTreeDlg::GetFirstHeightGroupItem()
{
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetNextItem(m_hBodyItem,TVGN_CHILD);
	while(hItem!=NULL)
	{
		TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
		if(pItemInfo==NULL||pItemInfo->itemType!=MODEL_CASE)
			continue;
		else	//�����ҵ��ĵ�һ��������ڵ�
			return hItem;
		hItem=pTreeCtrl->GetNextSiblingItem(hItem);
	}
	return NULL;
}
HTREEITEM CTowerTreeDlg::GetNextHeightGroupItem(HTREEITEM hItem)
{
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	hItem=pTreeCtrl->GetNextSiblingItem(hItem);
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo!=NULL&&pItemInfo->itemType==MODEL_CASE)
		return hItem;	//�����ҵ��ĵ�һ��������ڵ�
	else
		return NULL;
}

#ifdef __FEM_ANALYZE_INC_//  defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
void CTowerTreeDlg::RefreshHeightWindSegSetItem(HTREEITEM hWindSegSetItem,CLDSModule* pHeightGroup)
{
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	DeleteAllSubItems(pTreeCtrl,hWindSegSetItem);
	for(CWindSegment *pWindSeg=Ta.WindLoadSeg.GetFirst();pWindSeg;pWindSeg=Ta.WindLoadSeg.GetNext())
	{
		CXhChar50 ss;
		CFGWORD cfgword=pWindSeg->StatCfgword(&Ta);
		if(!cfgword.IsHasBodyNo(pHeightGroup->GetBodyNo()))
			continue;
		CXhChar50 heightStr;
		CFGLEG cfgleg=cfgword.ToCfgLeg();
		cfgleg.GetBodyScopeStr(heightStr);
		if(pWindSeg->cType!=1)	//����
		{
			CXhChar16 nodestr("");
			if(pWindSeg->nodePtrArr[0]&&pWindSeg->nodePtrArr[1])
			{
				nodestr.Printf("#%d",pWindSeg->nodePtrArr[0]->pointI);
				nodestr.Append(CXhChar16("%d",pWindSeg->nodePtrArr[1]->pointI),'-');
				if(!pWindSeg->nodePtrArr[1]->IsLegObj())
				{
					cfgleg.GetBodyScopeStr(heightStr);
					ss.Printf("S-%d{%s%s}",pWindSeg->iNo,(char*)heightStr,(char*)nodestr);
				}
				else
				{
					CXhChar16 legStr;
					cfgleg.GetBodyScopeStr(heightStr);
					cfgleg.GetLegScopeStr(legStr);
					heightStr.Append(legStr);
					ss.Printf("L-%d{%s%s}",pWindSeg->iNo,(char*)heightStr,(char*)nodestr);
				}
			}
			else
				ss.Printf("L-%d{?}",pWindSeg->iNo,(char*)heightStr);
		}
		else
			ss.Printf("A-%d{%s}",pWindSeg->iNo,(char*)heightStr);
		HTREEITEM hItem,hSubItem;
		hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_WINDSEG,PRJ_IMG_WINDSEG,hWindSegSetItem);
		TREEITEM_INFO *pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WINDSEG_CASE,(DWORD)pWindSeg));
		pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
		if(pWindSeg->cMirType==1)
			ss.Copy("Y��Գ�");
		else if(pWindSeg->cMirType==2)
			ss.Copy("X��Գ�");
		else
			ss.Copy("�޶Գ�");
		hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
		ss.Printf("��ѹ������=%d",pWindSeg->cType);
		if(pWindSeg->cType==0)
			ss.Printf("��ѹ������=0.�˵������������");
		else if(pWindSeg->cType==1)
			ss.Printf("��ѹ������=1.�Զ����ڵ�����");
		else //if(pWindSeg->cType==2)
			ss.Printf("��ѹ������=2.����������������");
		hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
		if(pWindSeg->cCalStyle==0)
			ss.Printf("���㷽ʽ=����ʽ");
		else if(pWindSeg->cCalStyle==1)
			ss.Printf("���㷽ʽ=X����ᵣ����");
		else// if(pWindSeg->cCalStyle==2)
			ss.Printf("���㷽ʽ=Y����ᵣ����");
		pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
		int i,n=pWindSeg->StatValidNodeCount(console.GetActiveModel());
		pWindSeg->InitNodePtrArr(console.GetActiveModel());
		for(i=0;i<n;i++)
		{
			if(pWindSeg->nodePtrArr[i])
			{
				ss.Printf("�ڵ�%d:0X%X(%d)",i+1,pWindSeg->nodeArr[i],pWindSeg->nodePtrArr[i]->point_i);
				hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			}
		}
		ss.Printf("����ϵ��(����)=%.2f",pWindSeg->BetaZ);
		hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
		ss.Printf("����ϵ��(����)=%.2f",pWindSeg->BetaZForFoundation);
		hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
		ss.Printf("����ЧӦϵ��=%.2f",pWindSeg->CqiFront);
		hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
		ss.Printf("����ЧӦϵ��=%.2f",pWindSeg->CqiSide);
		hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
		ss.Printf("���������״ϵ��=%.2f",pWindSeg->K);
		hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
	}
	//return NULL;
}
#endif

bool CTowerTreeDlg::RefreshHeightGroupItem(HTREEITEM hHeightItem)
{
	CString ss_leg[4],sTemp;
	CTowerTreeCtrl* pTreeCtrl=GetTreeCtrl();
	TREEITEM_INFO* pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hHeightItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=MODEL_CASE||pItemInfo->dwRefData==0)
		return false;
	CLDSModule* pModule=(CLDSModule*)pItemInfo->dwRefData;
	char strdata[64]={0};
	CXhString xhstr(strdata,64);
#ifdef AFX_TARG_ENU_ENGLISH
	xhstr.Printf("Anti-theft Bolt Z Scope:%.0f~%.0f",pModule->m_fBurglarLsStartZ,pModule->m_fBurglarLsEndZ);
#else 
	xhstr.Printf("������˨Z���귶Χ:%.0f~%.0f",pModule->m_fBurglarLsStartZ,pModule->m_fBurglarLsEndZ);
#endif
	HTREEITEM hSonItem=pTreeCtrl->GetChildItem(hHeightItem);
	if(hSonItem==NULL)
		hSonItem=pTreeCtrl->InsertItem(xhstr,PRJ_IMG_MAT_BOLT,PRJ_IMG_MAT_BOLT,hHeightItem);
	else
		pTreeCtrl->SetItemText(hSonItem,xhstr);
	pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_BURGLAR_LS_SCOPE,0));
	pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
#ifdef AFX_TARG_ENU_ENGLISH
	xhstr.Printf("Body No:%d",pModule->GetBodyNo());
#else 
	xhstr.Printf("�����:%d",pModule->GetBodyNo());
#endif
	hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
	if(hSonItem==NULL)
		hSonItem=pTreeCtrl->InsertItem(xhstr,PRJ_IMG_TOWERBODY,PRJ_IMG_TOWERBODY,hHeightItem);
	else
		pTreeCtrl->SetItemText(hSonItem,xhstr);
	pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_BODY_LEGNO,0));
	pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
	int nMaxLegs=CFGLEG::MaxLegOfBody(pModule->idBodyNo);
	int niStartLegSerial=CFGLEG::BitAddrIndexOf(pModule->idBodyNo)+1;//(pModule->idBodyNo-1)*nMaxLegs+1;
	for(int i=1;i<=192;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(pModule->m_dwLegCfgWord.IsHasNo(i))
			{
				if(i==pModule->m_arrActiveQuadLegNo[j])
					sTemp.Format("%C(*),",(i-niStartLegSerial)%nMaxLegs+'A');
				else
					sTemp.Format("%C,",(i-niStartLegSerial)%nMaxLegs+'A');
				ss_leg[j]+=sTemp;
			}
		}
	}
	for(int j=0;j<4;j++)
	{
		ss_leg[j]=ss_leg[j].Left(ss_leg[j].GetLength()-1);	//ȥ�Ҳ��','
#ifdef AFX_TARG_ENU_ENGLISH
		sTemp.Format("Joint Leg(%d)Model Flag:",j+1);
#else 
		sTemp.Format("����(%d)��ĺ�:",j+1);
#endif
		ss_leg[j]=sTemp+ss_leg[j];
		hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
		if(hSonItem==NULL)
			hSonItem=pTreeCtrl->InsertItem(ss_leg[j],PRJ_IMG_TOWERLEG,PRJ_IMG_TOWERLEG,hHeightItem);
		else
			pTreeCtrl->SetItemText(hSonItem,xhstr);
		if(j==0)
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD1LEG_CFGNO,0));
		else if(j==1)
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD2LEG_CFGNO,0));
		else if(j==2)
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD3LEG_CFGNO,0));
		else if(j==3)
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD4LEG_CFGNO,0));
		else
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(0,0));
		pTreeCtrl->SetItemData(hSonItem,(DWORD)pItemInfo);
	}
#ifdef __FEM_ANALYZE_INC_//  defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
	if (!theApp.m_bDrawingWorkEnvOnly)
	{
		HTREEITEM hRestrictIem = pTreeCtrl->GetNextSiblingItem(hSonItem);
		if (hRestrictIem == NULL)
			hRestrictIem = pTreeCtrl->InsertItem("Լ������", PRJ_IMG_BOUDARY, PRJ_IMG_BOUDARY, hHeightItem);
		pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_RESTRICT, 0));
		pTreeCtrl->SetItemData(hSonItem, (DWORD)pItemInfo);
		//TODO:���´����������ʱ�������ƣ����ڲ��ż�,Ҳ�����á�wjh-2017.4.7
		/*pTower->m_hActiveModule=pModule->handle;
		for(CLDSNode *pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
		{
			if(!pModule->IsSonPart(pNode))
				continue;
			if(pNode->IsHasRestrict(ALL_RESTRICT))
			{
				ss.Format("0X%X(%d)��",pNode->handle,pNode->point_i);
				int iOrder=0;
				if(pNode->IsHasRestrict(X_RESTRICT))
				{
					ss+="Rx";
					iOrder++;
				}
				if(pNode->IsHasRestrict(Y_RESTRICT))
				{
					if(iOrder>0)
						ss+=";Ry";
					else
						ss+="Ry";
					iOrder++;
				}
				if(pNode->IsHasRestrict(Z_RESTRICT))
				{
					if(iOrder>0)
						ss+=";Rz";
					else
						ss+="Rz";
				}
				hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hSubItem);
				pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_RESTRICT_NODE,(DWORD)pNode));
				pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
			}
		}
		HTREEITEM hWindSegSetItem = pTreeCtrl->InsertItem("��ѹ��",PRJ_IMG_WINDSEG,PRJ_IMG_WINDSEG,hParentItem);
		pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WINDSEG_GROUP,0));
		pTreeCtrl->SetItemData(hWindSegSetItem,(DWORD)pItemInfo);
		RefreshHeightWindSegSetItem(hWindSegSetItem,pModule);
		*/
	}
#endif
	return true;
}
struct MODULE_HEIGHT{
	CLDSModule* m_pModule;
	CFGWORD bodyword;
	CMaxDouble lowestZ;
	MODULE_HEIGHT(CLDSModule* pModule=NULL){m_pModule=pModule;}
};
struct LEG_HEIGHT{
	int legserial;	//1 based serial
	CMaxDouble lowestZ;
	LEG_HEIGHT(){legserial=0;}
};
#include "SortFunc.h"
static int CompareModuleSerial(const MODULE_HEIGHT& height1,const MODULE_HEIGHT& height2)
{
	if(height1.m_pModule->iNo>height2.m_pModule->iNo)
		return  1;
	else if(height1.m_pModule->iNo<height2.m_pModule->iNo)
		return -1;
	else
		return 0;
}
void CTowerTreeDlg::RefreshModelItem(HTREEITEM hModelItem,IModel* pModel)
{
	CString ss;
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	CTower* pTower=NULL;
	CBlockModel* pBlock=NULL;
	if(pModel->IsTowerModel())
		pTower=(CTower*)pModel;
	else
		pBlock=(CBlockModel*)pModel;
	DeleteAllSubItems(pTreeCtrl,hModelItem);
	HTREEITEM hParentItem,hItem,hDatumPointItem,hDatumLineItem,hDatumPlaneItem;
	HTREEITEM hSubItem=NULL,hArcLiftItem=NULL;
	//ģ�ͣ���������ͼ�㼶
#ifdef AFX_TARG_ENU_ENGLISH
	HTREEITEM hViewGroupItem = pTreeCtrl->InsertItem("View",PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hModelItem);
#else 
	HTREEITEM hViewGroupItem = pTreeCtrl->InsertItem("��ͼ",PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hModelItem);
#endif
	TREEITEM_INFO *pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_GROUP,0));
	pTreeCtrl->SetItemData(hViewGroupItem,(DWORD)pItemInfo);
	RefreshViewGroup(hViewGroupItem,pModel);

	if( pModel->IsTowerModel()||
		pModel->GetObjNum(CLS_GEPOINT)+pModel->GetObjNum(CLS_GELINE)+pModel->GetObjNum(CLS_GEPLANE)>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		hParentItem=pTreeCtrl->InsertItem("Geometric Primitives",PRJ_IMG_DATUM_PRIMITIVE_ELEMENT_GROUP,
#else 
		hParentItem=pTreeCtrl->InsertItem("��׼ͼԪ",PRJ_IMG_DATUM_PRIMITIVE_ELEMENT_GROUP,
#endif
			PRJ_IMG_DATUM_PRIMITIVE_ELEMENT_GROUP,hModelItem);
		if(pModel->IsTowerModel()||pModel->GetObjNum(CLS_GEPOINT)>0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			hItem=pTreeCtrl->InsertItem("Datum Point",PRJ_IMG_DATUM_POINT_GROUP,PRJ_IMG_DATUM_POINT_GROUP,hParentItem);
#else 
			hItem=pTreeCtrl->InsertItem("��׼��",PRJ_IMG_DATUM_POINT_GROUP,PRJ_IMG_DATUM_POINT_GROUP,hParentItem);
#endif
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_GROUP,0));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
			for(CLDSPoint *pPoint=pModel->EnumPointFirst();pPoint;pPoint=pModel->EnumPointNext())
			{
				ss.Format("%s_0X%X",pPoint->description,pPoint->handle);//,pPoint->iNo);
				hDatumPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hItem);
				pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pPoint));
				pTreeCtrl->SetItemData(hDatumPointItem,(DWORD)pItemInfo);
			}
		}
		if(pModel->IsTowerModel()||pModel->GetObjNum(CLS_GELINE)>0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			hItem=pTreeCtrl->InsertItem("Datum Line",PRJ_IMG_DATUM_LINE_GROUP,PRJ_IMG_DATUM_LINE_GROUP,hParentItem);
#else 
			hItem=pTreeCtrl->InsertItem("��׼��",PRJ_IMG_DATUM_LINE_GROUP,PRJ_IMG_DATUM_LINE_GROUP,hParentItem);
#endif
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_LINE_GROUP,0));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
			for(CLDSLine *pLine=pModel->EnumLineFirst();pLine;pLine=pModel->EnumLineNext())
			{
				ss.Format("%s_0X%X",pLine->description,pLine->handle);//,pLine->iNo);
				hDatumLineItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_LINE,PRJ_IMG_DATUM_LINE,hItem);
				pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_LINE_CASE,(DWORD)pLine));
				pTreeCtrl->SetItemData(hDatumLineItem,(DWORD)pItemInfo);
			}
		}

		if(pModel->IsTowerModel()||pModel->GetObjNum(CLS_GEPLANE)>0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			hItem=pTreeCtrl->InsertItem("Datum Plane",PRJ_IMG_DATUM_PLANE_GROUP,PRJ_IMG_DATUM_PLANE_GROUP,hParentItem);
#else 
			hItem=pTreeCtrl->InsertItem("��׼��",PRJ_IMG_DATUM_PLANE_GROUP,PRJ_IMG_DATUM_PLANE_GROUP,hParentItem);
#endif
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_PLANE_GROUP,0));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
			for(CLDSPlane *pPlane=pModel->EnumPlaneFirst();pPlane;pPlane=pModel->EnumPlaneNext())
			{
				ss.Format("%s_0X%X",pPlane->description,pPlane->handle);//,pPlane->iNo);
				hDatumPlaneItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_PLANE,PRJ_IMG_DATUM_PLANE,hItem);
				pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_PLANE_CASE,(DWORD)pPlane));
				pTreeCtrl->SetItemData(hDatumPlaneItem,(DWORD)pItemInfo);
			}
		}
	}
#if defined(__COMMON_PART_INC_) && !defined(__LDS_TSA_)
	if(pTower&&PRODUCT_FUNC::IsHasAutoArcLift())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		hArcLiftItem = pTreeCtrl->InsertItem("Arc Prelift",PRJ_IMG_ARC_LIFT_GROUP,PRJ_IMG_ARC_LIFT_GROUP,hModelItem);
#else 
		hArcLiftItem = pTreeCtrl->InsertItem("Բ��Ԥ����",PRJ_IMG_ARC_LIFT_GROUP,PRJ_IMG_ARC_LIFT_GROUP,hModelItem);
#endif
		pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_ARCLIFT_GROUP,0));
		pTreeCtrl->SetItemData(hArcLiftItem,(DWORD)pItemInfo);
		for(CLDSArcLift *pLift=pTower->ArcLift.GetFirst();pLift;pLift=pTower->ArcLift.GetNext())
		{
			ss.Format("%s(%d)",pLift->name,pLift->iNo);
			hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_ARC_LIFT_CASE,PRJ_IMG_ARC_LIFT_CASE,hArcLiftItem);
			pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_ARCLIFT_CASE,(DWORD)pLift));
			pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);
		}
	}
#endif
	if(pTower)
	{
		if(PRODUCT_FUNC::IsHasBlockSetup())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			HTREEITEM hBlockRefGroupItem = pTreeCtrl->InsertItem("Block Assembly",PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,hModelItem);
#else 
			HTREEITEM hBlockRefGroupItem = pTreeCtrl->InsertItem("��������",PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,hModelItem);
#endif
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(BLOCKREF_GROUP,0));
			pTreeCtrl->SetItemData(hBlockRefGroupItem,(DWORD)pItemInfo);
			for(CBlockReference *pBlockRef=pTower->BlockRef.GetFirst();pBlockRef;pBlockRef=pTower->BlockRef.GetNext())
			{
				HTREEITEM hBlockRefItem=NULL;
				ss.Format("%s(0x%X)",pBlockRef->descript,pBlockRef->handle);
				if(pBlockRef->IsExploded())
					hBlockRefItem= pTreeCtrl->InsertItem(ss,PRJ_IMG_EXPLODED_BLOCKREF_CASE,PRJ_IMG_EXPLODED_BLOCKREF_CASE,hBlockRefGroupItem);
				else
					hBlockRefItem= pTreeCtrl->InsertItem(ss,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,hBlockRefGroupItem);
				pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(BLOCKREF_CASE,(DWORD)pBlockRef));
				pTreeCtrl->SetItemData(hBlockRefItem,(DWORD)pItemInfo);
			}
		}
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
		if(!theApp.m_bDrawingWorkEnvOnly)
		{
			hItem=pTreeCtrl->InsertItem("���Ӻ���",PRJ_IMG_ATTACH_WINDLOAD,PRJ_IMG_ATTACH_WINDLOAD,hModelItem);
			pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_ATTACH_WINDLOAD,0));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
			for(CAttachWindLoad *pWindLoad=pTower->AttachWindLoad.GetFirst();pWindLoad;pWindLoad=pTower->AttachWindLoad.GetNext())
			{
				ss.Format("%s(%d)",pWindLoad->name,pWindLoad->iNo);
				hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_ATTACH_WINDLOAD,PRJ_IMG_ATTACH_WINDLOAD,hItem);
				pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_ATTACH_WINDLOADITEM,(DWORD)pWindLoad));
				pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);
			}
		}
#endif
		THANDLE hActModule=Ta.m_hActiveModule;
		ARRAY_LIST<MODULE_HEIGHT>heightsArr(0,pTower->Module.GetNodeNum());
		CLDSModule *pModule;
		for(pModule=pTower->Module.GetFirst();pModule;pModule=pTower->Module.GetNext())
			heightsArr.append(MODULE_HEIGHT(pModule));
		CBubbleSort<MODULE_HEIGHT>::BubSort(heightsArr.m_pData,heightsArr.GetSize(),CompareModuleSerial);
		for(int i=0;i<heightsArr.GetSize();i++)
		{
			pModule=heightsArr[i].m_pModule;
#ifdef __PART_DESIGN_INC_// defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
			ss.Format("%s(%d)",pModule->description,pModule->iNo);
#else
			ss.Format("%s",pModule->description);
#endif
			hParentItem =pTreeCtrl->InsertItem(ss,PRJ_IMG_MODULECASE,PRJ_IMG_MODULECASE,hModelItem);
			//RefreshHeightGroupItem(hParentItem,&Ta);
			if(pModule->handle==hActModule)	//��ǰ�״̬ģ��
				ShiftActiveItemState(hParentItem,ACTIVE_MODULE);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_CASE,(DWORD)pModule));
			pTreeCtrl->SetItemData(hParentItem,(DWORD)pItemInfo);

			CString ss_leg[4],sTemp;
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("Anti-theft Bolt Z Scope:%.0f~%.0f",pModule->m_fBurglarLsStartZ,pModule->m_fBurglarLsEndZ);
#else 
			ss.Format("������˨Z���귶Χ:%.0f~%.0f",pModule->m_fBurglarLsStartZ,pModule->m_fBurglarLsEndZ);
#endif
			hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_MAT_BOLT,PRJ_IMG_MAT_BOLT,hParentItem);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_BURGLAR_LS_SCOPE,0));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("Body No:%d",pModule->GetBodyNo());
#else 
			ss.Format("�����:%d",pModule->GetBodyNo());
#endif
			hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_TOWERBODY,PRJ_IMG_TOWERBODY,hParentItem);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_BODY_LEGNO,0));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
			int nMaxLegs=CFGLEG::MaxLegOfBody(pModule->idBodyNo);	//CFGLEG::MaxLegs();
			int niStartLegSerial=CFGLEG::BitAddrIndexOf(pModule->idBodyNo)+1;//(pModule->idBodyNo-1)*nMaxLegs+1;
			for(int i=1;i<=192;i++)
			{
				for(int j=0;j<4;j++)
				{
					if(pModule->m_dwLegCfgWord.IsHasNo(i))
					{
						if(i==pModule->m_arrActiveQuadLegNo[j])
							sTemp.Format("%C(*),",(i-niStartLegSerial)%nMaxLegs+'A');
						else
							sTemp.Format("%C,",(i-niStartLegSerial)%nMaxLegs+'A');
						ss_leg[j]+=sTemp;
					}
				}
			}
			for(int j=0;j<4;j++)
			{
				ss_leg[j]=ss_leg[j].Left(ss_leg[j].GetLength()-1);	//ȥ�Ҳ��','
#ifdef AFX_TARG_ENU_ENGLISH
				sTemp.Format("Joint Leg(%d)Model Flag:",j+1);
#else 
				sTemp.Format("����(%d)��ĺ�:",j+1);
#endif
				ss_leg[j]=sTemp+ss_leg[j];
				hItem=pTreeCtrl->InsertItem(ss_leg[j],PRJ_IMG_TOWERLEG,PRJ_IMG_TOWERLEG,hParentItem);
				if(j==0)
					pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD1LEG_CFGNO,0));
				else if(j==1)
					pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD2LEG_CFGNO,0));
				else if(j==2)
					pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD3LEG_CFGNO,0));
				else if(j==3)
					pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD4LEG_CFGNO,0));
				else
					pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(0,0));
				pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
			}
#ifdef __FEM_ANALYZE_INC_//  defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
			if(!theApp.m_bDrawingWorkEnvOnly)
			{
				hSubItem=pTreeCtrl->InsertItem("Լ������",PRJ_IMG_BOUDARY,PRJ_IMG_BOUDARY,hParentItem );
				pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_RESTRICT,0));
				pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);
				pTower->m_hActiveModule=pModule->handle;
				for(CLDSNode *pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
				{
					if(!pModule->IsSonPart(pNode))
						continue;
					if(pNode->IsHasRestrict(ALL_RESTRICT))
					{
						ss.Format("0X%X(%d)��",pNode->handle,pNode->point_i);
						int iOrder=0;
						if(pNode->IsHasRestrict(X_RESTRICT))
						{
							ss+="Rx";
							iOrder++;
						}
						if(pNode->IsHasRestrict(Y_RESTRICT))
						{
							if(iOrder>0)
								ss+=";Ry";
							else
								ss+="Ry";
							iOrder++;
						}
						if(pNode->IsHasRestrict(Z_RESTRICT))
						{
							if(iOrder>0)
								ss+=";Rz";
							else
								ss+="Rz";
						}
						hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hSubItem);
						pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_RESTRICT_NODE,(DWORD)pNode));
						pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
					}
				}
				HTREEITEM hWindSegSetItem = pTreeCtrl->InsertItem("��ѹ��",PRJ_IMG_WINDSEG,PRJ_IMG_WINDSEG,hParentItem);
				pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WINDSEG_GROUP,0));
				pTreeCtrl->SetItemData(hWindSegSetItem,(DWORD)pItemInfo);
				RefreshHeightWindSegSetItem(hWindSegSetItem,pModule);
			}
#endif
		}
		pTower->m_hActiveModule=hActModule;
		pTreeCtrl->Expand(hModelItem,TVE_EXPAND);
#if defined(__PART_DESIGN_INC_)&&!defined(__LDS_TDA_)//defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
		if(!theApp.m_bDrawingWorkEnvOnly)
		{
			hParentItem = pTreeCtrl->InsertItem("���ع���",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hModelItem);
			pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_GROUP,0));
			pTreeCtrl->SetItemData(hParentItem,(DWORD)pItemInfo);
			CWorkStatus *pStatus=Ta.GetActiveWorkStatus();
			if(pStatus)
				m_iCurStatusNo=pStatus->iNo;
			HTREEITEM hStatusItem=TVI_LAST;
			for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
				hStatusItem=InsertWorkStatusItem(pStatus,hParentItem,hStatusItem);
			hParentItem = pTreeCtrl->InsertItem("��ѹ��",PRJ_IMG_WINDSEG,PRJ_IMG_WINDSEG,hModelItem);
			pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WINDSEG_GROUP,0));
			pTreeCtrl->SetItemData(hParentItem,(DWORD)pItemInfo);
			for(CWindSegment *pWindSeg=pTower->WindLoadSeg.GetFirst();pWindSeg;pWindSeg=pTower->WindLoadSeg.GetNext())
			{
				CXhChar50 heightStr;
				CFGWORD cfgword=pWindSeg->StatCfgword(&Ta);
				CFGLEG cfgleg=cfgword.ToCfgLeg();
				cfgleg.GetBodyScopeStr(heightStr);
				CXhChar50 ss;
				if(pWindSeg->cType!=1)	//����
				{
					CXhChar16 nodestr("");
					if(pWindSeg->nodePtrArr[0]&&pWindSeg->nodePtrArr[1])
					{
						nodestr.Printf("#%d",pWindSeg->nodePtrArr[0]->pointI);
						nodestr.Append(CXhChar16("%d",pWindSeg->nodePtrArr[1]->pointI),'-');
						if(!pWindSeg->nodePtrArr[1]->IsLegObj())
						{
							cfgleg.GetBodyScopeStr(heightStr);
							ss.Printf("S-%d{%s%s}",pWindSeg->iNo,(char*)heightStr,(char*)nodestr);
						}
						else
						{
							CXhChar16 legStr;
							cfgleg.GetBodyScopeStr(heightStr);
							cfgleg.GetLegScopeStr(legStr);
							heightStr.Append(legStr);
							ss.Printf("L-%d{%s%s}",pWindSeg->iNo,(char*)heightStr,(char*)nodestr);
						}
					}
					else
						ss.Printf("L-%d{?}",pWindSeg->iNo,(char*)heightStr);
				}
				else
					ss.Printf("A-%d{%s}",pWindSeg->iNo,(char*)heightStr);
				hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_WINDSEG,PRJ_IMG_WINDSEG,hParentItem);
				pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WINDSEG_CASE,(DWORD)pWindSeg));
				pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
				if(pWindSeg->cMirType==1)
					ss.Copy("Y��Գ�");
				else if(pWindSeg->cMirType==2)
					ss.Copy("X��Գ�");
				else
					ss.Copy("�޶Գ�");
				hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				ss.Printf("��ѹ������=%d",pWindSeg->cType);
				if(pWindSeg->cType==0)
					ss.Printf("��ѹ������=0.�˵������������");
				else if(pWindSeg->cType==1)
					ss.Printf("��ѹ������=1.�Զ����ڵ�����");
				else //if(pWindSeg->cType==2)
					ss.Printf("��ѹ������=2.����������������");
				hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				if(pWindSeg->cCalStyle==0)
					ss.Printf("���㷽ʽ=����ʽ");
				else if(pWindSeg->cCalStyle==1)
					ss.Printf("���㷽ʽ=X����ᵣ����");
				else// if(pWindSeg->cCalStyle==2)
					ss.Printf("���㷽ʽ=Y����ᵣ����");
				pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				int i,n=pWindSeg->StatValidNodeCount(console.GetActiveModel());
				pWindSeg->InitNodePtrArr(console.GetActiveModel());
				for(i=0;i<n;i++)
				{
					if(pWindSeg->nodePtrArr[i])
					{
						ss.Printf("�ڵ�%d:0X%X(%d)",i+1,pWindSeg->nodeArr[i],pWindSeg->nodePtrArr[i]->point_i);
						hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
					}
				}
				ss.Printf("����ϵ��(����)=%.2f",pWindSeg->BetaZ);
				hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				ss.Printf("����ϵ��(����)=%.2f",pWindSeg->BetaZForFoundation);
				hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				ss.Printf("����ЧӦϵ��=%.2f",pWindSeg->CqiFront);
				hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				ss.Printf("����ЧӦϵ��=%.2f",pWindSeg->CqiSide);
				hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				ss.Printf("���������״ϵ��=%.2f",pWindSeg->K);
				hSubItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			}
		}
#endif
	}
	//������ͼ����
	hItem=pTreeCtrl->GetRootItem();
	HTREEITEM hDefinedViewSetItem=NULL;
	while(hItem)
	{
		TREEITEM_INFO *pItem=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hItem);
		if(pItem&&pItem->itemType==VIEW_DEFINEDSET)
		{
			hDefinedViewSetItem=hItem;
			pTreeCtrl->Expand(hItem,TVE_EXPAND);
		}
		if(pItem&&(pItem->itemType==VIEW_DEFINEDFOLDER||pItem->itemType==VIEW_DEFINEDSET))
			m_treeCtrl.SortChildren(hItem);
		hItem=m_treeCtrl.GetNextVisibleItem(hItem);
	}
	SortItem(hDefinedViewSetItem);
}
void CTowerTreeDlg::RefreshTreeCtrl()
{
	TREEITEM_INFO *pItemInfo;
	itemInfoList.Empty();
	hashItemInfos.Empty();
	CString ss;
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	pTreeCtrl->DeleteAllItems();
#ifdef AFX_TARG_ENU_ENGLISH
	m_hDataRootItem = pTreeCtrl->InsertItem("Data Model",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,TVI_ROOT);
#else 
	m_hDataRootItem = pTreeCtrl->InsertItem("����ģ��",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,TVI_ROOT);
#endif
	pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(DATA_SPACE,0));
	pTreeCtrl->SetItemData(m_hDataRootItem,(DWORD)pItemInfo);
	//ģ�ͣ������㼶
#ifdef AFX_TARG_ENU_ENGLISH
	m_hBodyItem = pTreeCtrl->InsertItem("Tower Model",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,m_hDataRootItem);
#else 
	m_hBodyItem = pTreeCtrl->InsertItem("����",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,m_hDataRootItem);
#endif
	pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_GROUP,(DWORD)&Ta));
	pTreeCtrl->SetItemData(m_hBodyItem,(DWORD)pItemInfo);
	RefreshModelItem(m_hBodyItem,&Ta);
	if(PRODUCT_FUNC::IsHasBlockSetup())
	{
		//ģ�ͣ������㼶
		//HTREEITEM hBlockGroupItem = pTreeCtrl->InsertItem("����",PRJ_IMG_BLOCK_CASE,PRJ_IMG_BLOCK_CASE,TVI_ROOT);
		//pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(BLOCK_GROUP,0));
		//pTreeCtrl->SetItemData(hBlockGroupItem,(DWORD)pItemInfo);
		for(CBlockModel *pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
		{
			CString ss;
			ss.Format("%s(0X%X)",pBlock->name,pBlock->handle);
			HTREEITEM hBlockItem;
			if(pBlock->IsEmbeded())//��Ƕ
				hBlockItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_EMBED_BLOCK_CASE,PRJ_IMG_EMBED_BLOCK_CASE,m_hDataRootItem);
			else//����Ƕ
				hBlockItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_IMPORT_BLOCK_CASE,PRJ_IMG_IMPORT_BLOCK_CASE,m_hDataRootItem);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(BLOCK_CASE,(DWORD)pBlock));
			pTreeCtrl->SetItemData(hBlockItem,(DWORD)pItemInfo);
			RefreshModelItem(hBlockItem,pBlock);
			//InsertBlockItem(pBlock,hModelItem);//hBlockGroupItem);
		}
	}
	pTreeCtrl->Expand(m_hDataRootItem,TVE_EXPAND);
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
	if(!theApp.m_bDrawingWorkEnvOnly)
	{
		HTREEITEM hParentItem =pTreeCtrl->InsertItem("��˨",PRJ_IMG_MAT_BOLT,PRJ_IMG_MAT_BOLT,TVI_ROOT);
		pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(BOLT_GROUP,0));
		pTreeCtrl->SetItemData(hParentItem,(DWORD)pItemInfo);
		for(int i=0;i<LsTypeCount;i++)
		{
			HTREEITEM hSubItem=pTreeCtrl->InsertItem(ls_table[i].grade,PRJ_IMG_MAT_BOLT,PRJ_IMG_MAT_BOLT,hParentItem);
			pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(BOLT_TYPE_RECORD,(DWORD)&ls_table[i]));
			pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);
			CString ss;
			ss.Format("����ǿ�����ֵf =%.0f(MPa)",ls_table[i].f);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hSubItem);
			ss.Format("����ǿ�����ֵfv=%.0f(MPa)",ls_table[i].fv);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hSubItem);
			ss.Format("�ױڳ�ѹǿ�����ֵfc=%.0f(MPa)",ls_table[i].fc);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hSubItem);
		}
	}
#endif
}
void CTowerTreeDlg::RefreshMatTypeItem()
{
/*	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	HTREEITEM hMatGroupItem=pTreeCtrl->GetRootItem();
	while(hMatGroupItem!=NULL)
	{
		TREEITEM_INFO *pInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hMatGroupItem);
		if(pInfo&&pInfo->itemType==MATERIAL_GROUP)
			break;
		hMatGroupItem=pTreeCtrl->GetNextItem(hMatGroupItem,TVGN_NEXT);
	}
	if(hMatGroupItem==NULL)
		return;
	HTREEITEM hMatItem=pTreeCtrl->GetNextItem(hMatGroupItem,TVGN_CHILD);
	while(hMatItem)
	{
		TREEITEM_INFO *pInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hMatItem);
		if(pInfo&&pInfo->itemType==MATERIAL_RECORD)
		{
			STEELMAT *pSteelMat=(STEELMAT*)pInfo->dwRefData;
			//�����Ҳ�ģ�����е�ǰ�������͵�������
			HTREEITEM hSubItem=pTreeCtrl->GetNextItem(hMatItem,TVGN_CHILD);	//��ʶ��
			CString ss;
			ss.Format("��ʶ��='%C'",pSteelMat->cBriefMark);
			pTreeCtrl->SetItemText(hSubItem,ss);
			hSubItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_NEXT);	//�۸�ϵ��
			ss.Format("�۸�ϵ��=%.2f ",pSteelMat->price_coef);
			pTreeCtrl->SetItemText(hSubItem,ss);
			hSubItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_NEXT);	//����ģ��
			ss.Format("E=%.0f MPa",pSteelMat->E);
			pTreeCtrl->SetItemText(hSubItem,ss);
			hSubItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_NEXT);	//����ǿ�ȱ�׼ֵfy
			HTREEITEM hLeafItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_CHILD);	//��һ������ǿ�ȱ�׼ֵ
			ss.Format(" 0��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[0],pSteelMat->fy(pSteelMat->t_lim[0]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);	//�ڶ�������ǿ�ȱ�׼ֵ
			ss.Format("%d��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[0],pSteelMat->t_lim[1],pSteelMat->fy(pSteelMat->t_lim[1]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);	//����������ǿ�ȱ�׼ֵ
			ss.Format("%d��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[1],pSteelMat->t_lim[2],pSteelMat->fy(pSteelMat->t_lim[2]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hSubItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_NEXT);	//����ѹ��ǿ�����ֵf
			hLeafItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_CHILD);	//��һ����ѹ��ǿ�����ֵf
			ss.Format(" 0��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[0],pSteelMat->f(pSteelMat->t_lim[0]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);	//�ڶ�����ѹ��ǿ�����ֵf
			ss.Format("%d��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[0],pSteelMat->t_lim[1],pSteelMat->f(pSteelMat->t_lim[1]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);	//��������ѹ��ǿ�����ֵf
			ss.Format("%d��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[1],pSteelMat->t_lim[2],pSteelMat->f(pSteelMat->t_lim[2]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hSubItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_NEXT);	//����ǿ�����ֵfv
			hLeafItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_CHILD);	//��һ�鿹��ǿ�����ֵfv
			ss.Format(" 0��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[0],pSteelMat->fv(pSteelMat->t_lim[0]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);	//�ڶ��鿹��ǿ�����ֵfv
			ss.Format("%d��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[0],pSteelMat->t_lim[1],pSteelMat->fv(pSteelMat->t_lim[1]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);	//�����鿹��ǿ�����ֵfv
			ss.Format("%d��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[1],pSteelMat->t_lim[2],pSteelMat->fv(pSteelMat->t_lim[2]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hSubItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_NEXT);	//�ױڳ�ѹǿ�����ֵfc
			hLeafItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_CHILD);	//��һ��ױڳ�ѹǿ�����ֵfc
			ss.Format(" 0��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[0],pSteelMat->fc(pSteelMat->t_lim[0]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);	//�ڶ���ױڳ�ѹǿ�����ֵfc
			ss.Format("%d��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[0],pSteelMat->t_lim[1],pSteelMat->fc(pSteelMat->t_lim[1]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
			hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);	//������ױڳ�ѹǿ�����ֵfc
			ss.Format("%d��t��%dmm:%.0f(MPa)",pSteelMat->t_lim[1],pSteelMat->t_lim[2],pSteelMat->fc(pSteelMat->t_lim[2]));
			pTreeCtrl->SetItemText(hLeafItem,ss);
		}
		//��һ�ֲ�������
		hMatItem=pTreeCtrl->GetNextItem(hMatItem,TVGN_NEXT);
	}*/
}
BOOL CTowerTreeDlg::ModifyTreeItem(TREEITEM_INFO *pItemInfo, TOWER_TREE_CMD cmd, void *pData)
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	if(pItemInfo->itemType==MODEL_CASE)
	{
		if(cmd==TREE_ITEM_ADD)
		{
			CLDSModule *pModule=(CLDSModule*)pData;
			HTREEITEM hRootItem=pTreeCtrl->GetRootItem();
			if(pTreeCtrl->ItemHasChildren(hRootItem))
			{
				HTREEITEM hNextItem = pTreeCtrl->GetChildItem(hRootItem);
				while(hNextItem!=NULL)
				{
					hNextItem = pTreeCtrl->GetNextItem(hNextItem, TVGN_NEXT);
					DWORD data=pTreeCtrl->GetItemData(hNextItem);
					if(data==MODEL_CASE)
						pTreeCtrl->InsertItem(pModule->description,PRJ_IMG_MODULECASE,PRJ_IMG_MODULECASE,hNextItem);
				}
			}
		}
		else if(cmd==TREE_ITEM_DELETE)
		{
		}
		else// if(cmd==TREE_ITEM_REFRESH)
		{
		}
	}
	return TRUE;
}

void CTowerTreeDlg::OnDblclkTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnSetActiveItem();//�����Ӧѡ��
	*pResult = 0;
}

void CTowerTreeDlg::OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	if(pTVKeyDown->wVKey==VK_DELETE)
	{
		CTreeCtrl *pTreeCtrl = GetTreeCtrl();
		HTREEITEM hItem=pTreeCtrl->GetSelectedItem();		//Ŀǰ�ڹ�����һ��
		if(hItem)
			DeleteItem(hItem);
	}
	*pResult = 0;
}
#if defined(__LDS_)||defined(__TSA_)
static void DarwWindSegBox(CWindSegElemPicker *pSegPicker)
{
	int i,vertex_n=pSegPicker->xarrVertices.GetSize();
	fBody body;
	ARRAY_LIST<f3dPoint> vertex_arr;
	vertex_arr.SetSize(vertex_n*2);
	double maxZ=pSegPicker->MaxZ();
	double minZ=pSegPicker->MinZ();
	if(maxZ-minZ<2)
	{
		maxZ+=1;
		minZ-=1;
	}
	for (i=0;i<vertex_n;i++)
	{
		vertex_arr[i]=pSegPicker->xarrVertices[i].xPlanePos;
		vertex_arr[i].z=maxZ;	//���嶥��
		vertex_arr[i+vertex_n]=vertex_arr[i];
		vertex_arr[i+vertex_n].z=minZ;//�������
	}
	for (i=0;i<vertex_arr.GetSize();i++)
	{
		f3dPoint *pVertex=body.vertex.append(vertex_arr[i]);
		coord_trans(*pVertex,pSegPicker->ucs,TRUE);
	}
	CCreateFace createFace;
	createFace.InitVertexList(&body);
	f3dPolyFace *pFace=NULL;
	//�������
	for(i=0;i<vertex_n;i++)
	{
		pFace=body.faceList.append();
		pFace->material = RGB(0,0,125);
		createFace.NewOutterEdgeLine(pFace,i,(i+1)%vertex_n);
		createFace.NewOutterEdgeLine(pFace,i+vertex_n);
		createFace.NewOutterEdgeLine(pFace,(i+1)%vertex_n+vertex_n);
		createFace.NewOutterEdgeLine(pFace,(i+1)%vertex_n);
	}
	//���嶥�漰���洴��
	pFace=body.faceList.append();	//����
	pFace->material = RGB(0,0,125);
	for(i=0;i<vertex_n;i++)
		createFace.NewOutterEdgeLine(pFace,(i+1)%vertex_n,i);
	pFace=body.faceList.append();	//����
	pFace->material = RGB(0,0,125);
	for(i=0;i<vertex_n;i++)
		createFace.NewOutterEdgeLine(pFace,2*vertex_n-i-1,vertex_n+(vertex_n-i)%vertex_n);
	CSolidPart solidPart;
	solidPart.m_hPart = 1;
	CSolidBody solidbody;
	solidbody.ConvertFrom(&body);
	solidPart.pBody = &solidbody;
	solidPart.m_idClassType = CLS_PLATE;
	g_pSolidDraw->NewSolidPart(solidPart);
}
#endif
void CTowerTreeDlg::OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(itemInfoList.GetNodeNum()==0)
		return;	//�����
	g_pSolidDraw->DelEnt(1);
	if(theApp.m_bAnalysisWorkEnv)
	{
		g_pSolidSet->SetDisplayType(DISP_LINE);
		g_pSolidDraw->Draw();
	}
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItemOld=pNMTreeView->itemOld.hItem;
	HTREEITEM hItemNew=pNMTreeView->itemNew.hItem;
	if(hItemOld)
	{
		TREEITEM_INFO* pInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItemOld);
		if(pInfo&&pInfo->itemType==MODEL_GROUP&&pInfo->itemType==MODEL_RESTRICT_NODE)
		{
			CString ss;
			ss=pTreeCtrl->GetItemText(hItemOld);
			THANDLE hNode;
			sscanf(ss,"%X",&hNode);
			g_pSolidDraw->SetEntSnapStatus(hNode,FALSE);
		}
		else if(pInfo&&pInfo->itemType==WORKSTATUS_CASE&&pInfo->itemType==WORKSTATUS_POINTLOAD_CASE)
		{
			CString ss;
			ss=pTreeCtrl->GetItemText(hItemOld);
			THANDLE hNode;
			sscanf(ss,"%X",&hNode);
			g_pSolidDraw->SetEntSnapStatus(hNode,FALSE);
		}
		else if(pInfo&&pInfo->itemType==WINDSEG_CASE)
		{
			if (hItemNew)
			{
				TREEITEM_INFO* pInfo1=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItemNew);
				if(!pInfo1 || pInfo1->itemType != WINDSEG_CASE)
				{
					CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
					pView->RedrawAll();
				}	
			}
			else
			{
				CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
				pView->RedrawAll();
			}
		}
	}
	if(hItemNew)
	{
		TREEITEM_INFO* pInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItemNew);
		if(pInfo&&pInfo->itemType==BLOCK_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
		{	//��������
			if(pInfo->dwRefData!=0)
			{
				CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
				pLdsView->m_pSelectBlock=(CBlockModel*)pInfo->dwRefData;
				pLdsView->DisplayBlockModelProperty(TRUE);
			}
		}
		else if(pInfo&&pInfo->itemType==BLOCKREF_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
		{	//����װ���������
			if(pInfo->dwRefData!=0)
			{
				g_pSolidDraw->ReleaseSnapStatus();
				CLDSView* pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
				pLdsView->UpdatePropertyPage((CBlockReference*)pInfo->dwRefData);
			}
		}
#if defined(__LDS_)||defined(__TSA_)
		else if(pInfo&&pInfo->itemType==MODEL_RESTRICT_NODE)
		{
			CString ss;
			ss=pTreeCtrl->GetItemText(hItemNew);
			THANDLE hNode;
			sscanf(ss,"%X",&hNode);
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(hNode);
		}
		else if(pInfo&&pInfo->itemType==WORKSTATUS_POINTLOAD_CASE)
		{
			CString ss;
			ss=pTreeCtrl->GetItemText(hItemNew);
			THANDLE hNode;
			sscanf(ss,"%X",&hNode);
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(hNode);
		}
		else if(pInfo&&pInfo->itemType==WINDSEG_CASE)
		{
			CWindSegment *pSeg=(CWindSegment*)pInfo->dwRefData;
			NODESET nodeset;
			CWindSegElemPicker picker;
			pSeg->StatProfileNodeSet(console.GetActiveModel(),nodeset);
			if(picker.InitElemPicker(nodeset,pSeg->xProjPlaneOrg,pSeg->iNo))
				DarwWindSegBox(&picker);
			if(pSeg->cType==0)
			{
				nodeset.Empty();
				pSeg->StatProfileNodeSet(console.GetActiveModel(),nodeset);
				if(picker.InitElemPicker(nodeset,pSeg->xProjPlaneOrg, pSeg->iNo))
					DarwWindSegBox(&picker);
			}
			WINDSEG_FILTER filter;
			if(filter.InitFilter(pSeg))
			{
				ARRAY_LIST<long> rods(0,100);
				for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
				{
					if(filter.IsIncludeRod(pRod))
						rods.append(pRod->handle);
				}
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(rods.m_pData,rods.GetSize());
			}
			CLDSModule* pModule=Ta.GetActiveModule();
			for(CLDSNode* pNode=nodeset.GetFirst();pNode&&pModule;pNode=nodeset.GetNext())
			{
				if(!pModule->IsSonPart(pNode))
				{
					CXhChar50 scopeStr;
					if(pNode->IsLegObj())
					{
						pNode->cfgword.GetLegScopeStr(scopeStr,51,true);
						AfxMessageBox(CXhChar200("�ڵ�%d����ķ�Χ{%s}���ڵ�ǰ���߼�����ȷ�Χ��",pNode->point_i,(char*)scopeStr));
					}
					else
					{
						pNode->cfgword.GetBodyScopeStr(scopeStr,51);
						AfxMessageBox(CXhChar200("�ڵ�%d����ķ�Χ{%s}���ڵ�ǰ���߷�Χ��",pNode->point_i,(char*)scopeStr));
					}
					break;
				}
			}
			//g_pSolidSet->SetDisplayType(DISP_SOLID);
			g_pSolidDraw->Draw();
		}
#endif
		//OnSetActiveItem();
	}
	*pResult = 0;
}

//�Ƿ���Ҫ��ʾ�Ҽ��˵�
BOOL IsDisplayContextMenu(CTreeCtrl *pTreeCtrl,HTREEITEM hItem)
{
	HTREEITEM hBlockItem=NULL;
	if(hItem==NULL)
		return TRUE;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL&&hItem)
		return FALSE;
	if(pItemInfo->itemType==VIEW_GROUP)
	{
		hBlockItem=pTreeCtrl->GetParentItem(hItem);
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hBlockItem);
		if(pItemInfo->itemType==BLOCK_CASE)
			return FALSE;
	}
	else if(pItemInfo->itemType==VIEW_LAYERSET || pItemInfo->itemType==VIEW_DEFINEDSET||pItemInfo->itemType==MODEL_DATUM_POINT_GROUP 
		|| pItemInfo->itemType==MODEL_DATUM_LINE_GROUP || pItemInfo->itemType==MODEL_DATUM_PLANE_GROUP)
	{
		hBlockItem=pTreeCtrl->GetParentItem(hItem);
		hBlockItem=pTreeCtrl->GetParentItem(hBlockItem);
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hBlockItem);
		if(pItemInfo->itemType==BLOCK_CASE)
			return FALSE;
	}
	else if(pItemInfo->itemType==VIEW_DEFINEDFOLDER||pItemInfo->itemType==MODEL_DATUM_POINT_CASE
		||pItemInfo->itemType==MODEL_DATUM_LINE_CASE||pItemInfo->itemType==MODEL_DATUM_PLANE_CASE)
	{
		hBlockItem=pTreeCtrl->GetParentItem(hItem);
		for(int i=0;i<2;i++)
			hBlockItem=pTreeCtrl->GetParentItem(hBlockItem);
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hBlockItem);
		if(pItemInfo->itemType==BLOCK_CASE)
			return FALSE;
	}
	else if(pItemInfo->itemType==VIEW_CASE)
	{
		CDisplayView* pView=(CDisplayView*)pItemInfo->dwRefData;
		if(!pView->BelongModel()->IsTowerModel())
			return FALSE;
	}
	return TRUE;
}
void  CTowerTreeDlg::DeleteBlockRefItem(CBlockReference * pBlockRef)
{
	//�ҵ�ѡ�в��������ϵ�����
	CBlockReference * pInTreeBlockRef=NULL;
	HTREEITEM hItem=m_treeCtrl.GetRootItem();
	while(hItem)
	{
		TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hItem);
		if(pItemInfo&&pItemInfo->itemType==BLOCKREF_CASE)
			pInTreeBlockRef = (CBlockReference *)pItemInfo->dwRefData;
		if(pBlockRef&&pInTreeBlockRef&&pBlockRef==pInTreeBlockRef)
			DeleteItem(hItem);
		hItem=m_treeCtrl.GetNextVisibleItem(hItem);
	}
}
CHashStrList<PROPLIST_ITEM> CTowerTreeDlg::propMenuHashtable;	//�����ַ�����ϣ��
CHashSet<PROPLIST_ITEM*> CTowerTreeDlg::hashMenuHelpItemById;
void CTowerTreeDlg::InitMenuHelpHashtable()
{	//��ʼ��ģ�����Ľڵ�����Ҽ��˵�
	int id=1;
	CTowerTreeDlg::propMenuHashtable.SetHashTableGrowSize(100);//HASHTABLESIZE);
	AddMenuHelpItem("DATA_SPACE",PROPLIST_ITEM(id++,"����ģ��"));
	AddMenuHelpItem("DATA_SPACE.ID_IMPORT_BLOCK_FILE",PROPLIST_ITEM(id++,"����ģ��->���벿��"));
	AddMenuHelpItem("MODEL_GROUP",PROPLIST_ITEM(id++,"����"));
	AddMenuHelpItem("MODEL_GROUP.ID_REFRESH_TREE",PROPLIST_ITEM(id++,"����->ˢ��"));
	AddMenuHelpItem("MODEL_GROUP.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"����->����"));
	AddMenuHelpItem("MODEL_GROUP.ID_NEW_ITEM",PROPLIST_ITEM(id++,"����->�½�����"));
	//��ͼ
	AddMenuHelpItem("VIEW_GROUP",PROPLIST_ITEM(id++,"��ͼ"));
	AddMenuHelpItem("VIEW_GROUP.ID_REFRESH_TREE",PROPLIST_ITEM(id++,"��ͼ->ˢ��"));
	AddMenuHelpItem("VIEW_LAYERSET",PROPLIST_ITEM(id++,"ͼ����ͼ"));
	AddMenuHelpItem("VIEW_LAYERSET.ID_NEW_ITEM",PROPLIST_ITEM(id++,"ͼ����ͼ->����ͼ����ͼ"));
	AddMenuHelpItem("VIEW_DEFINEDSET",PROPLIST_ITEM(id++,"������ͼ"));
	AddMenuHelpItem("VIEW_DEFINEDSET.ID_SORT_ITEM",PROPLIST_ITEM(id++,"������ͼ->����"));
	AddMenuHelpItem("VIEW_DEFINEDSET.ID_NEW_ITEM_FOLDER",PROPLIST_ITEM(id++,"������ͼ->�½���ͼ��"));
	AddMenuHelpItem("VIEW_DEFINEDSET.ID_NEW_ITEM",PROPLIST_ITEM(id++,"������ͼ->�½�������ͼ"));
	AddMenuHelpItem("VIEW_DESINEDSET.ID_RETRIEVE_UDF_VIEW",PROPLIST_ITEM(id++,"������ͼ->������ȡ˾��ͼ"));
	AddMenuHelpItem("VIEW_DESINEDSET.ID_RETRIEVE_DRAWING_VIEW",PROPLIST_ITEM(id++,"������ͼ->������ȡ�ṹͼ"));	
	AddMenuHelpItem("VIEW_DEFINEDFOLDER",PROPLIST_ITEM(id++,"������"));
	AddMenuHelpItem("VIEW_DEFINEDFOLDER.ID_SORT_ITEM",PROPLIST_ITEM(id++,"������->����"));
	AddMenuHelpItem("VIEW_DEFINEDFOLDER.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"������->����"));
	AddMenuHelpItem("VIEW_DEFINEDFOLDER.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"������->ɾ��"));
	AddMenuHelpItem("VIEW_CASE",PROPLIST_ITEM(id++,"��ͼ��"));
	AddMenuHelpItem("VIEW_CASE.ID_SET_ACTIVE_ITEM",PROPLIST_ITEM(id++,"��ͼ��->������ͼ"));
	AddMenuHelpItem("VIEW_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"��ͼ��->��ͼ����"));
	AddMenuHelpItem("VIEW_CASE.ID_LAYERVIEW_SEG_PROP_ITEM",PROPLIST_ITEM(id++,"��ͼ��->��ͼ�ֶ�����"));
	AddMenuHelpItem("VIEW_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"��ͼ��->ɾ����ͼ"));
	AddMenuHelpItem("VIEW_CASE.ID_HANDOVER_PERMISSION",PROPLIST_ITEM(id++,"��ͼ��->��ͼ��Ȩ"));
	//��׼ͼԪ
	AddMenuHelpItem("MODEL_DATUM_POINT_GROUP",PROPLIST_ITEM(id++,"��׼��"));
	AddMenuHelpItem("MODEL_DATUM_POINT_GROUP.ID_NEW_ITEM",PROPLIST_ITEM(id++,"��׼��->�½���׼��"));
	AddMenuHelpItem("MODEL_DATUM_POINT_CASE",PROPLIST_ITEM(id++,"��׼��Ԫ"));
	AddMenuHelpItem("MODEL_DATUM_POINT_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"��׼��Ԫ->����"));
	AddMenuHelpItem("MODEL_DATUM_POINT_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"��׼��Ԫ->ɾ����׼��"));
	AddMenuHelpItem("MODEL_DATUM_LINE_GROUP",PROPLIST_ITEM(id++,"��׼��"));
	AddMenuHelpItem("MODEL_DATUM_LINE_GROUP.ID_NEW_ITEM",PROPLIST_ITEM(id++,"��׼��->�½���׼��"));
	AddMenuHelpItem("MODEL_DATUM_LINE_CASE",PROPLIST_ITEM(id++,"��׼��Ԫ"));
	AddMenuHelpItem("MODEL_DATUM_LINE_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"��׼��Ԫ->����"));
	AddMenuHelpItem("MODEL_DATUM_LINE_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"��׼��Ԫ->ɾ����׼��"));
	AddMenuHelpItem("MODEL_DATUM_PLANE_GROUP",PROPLIST_ITEM(id++,"��׼��"));
	AddMenuHelpItem("MODEL_DATUM_PLANE_GROUP.ID_NEW_ITEM",PROPLIST_ITEM(id++,"��׼��->�½���׼��"));
	AddMenuHelpItem("MODEL_DATUM_PLANE_CASE",PROPLIST_ITEM(id++,"��׼��Ԫ"));
	AddMenuHelpItem("MODEL_DATUM_PLANE_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"��׼��Ԫ->����"));
	AddMenuHelpItem("MODEL_DATUM_PLANE_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"��׼��Ԫ->ɾ����׼��"));
	//Բ��Ԥ��
	AddMenuHelpItem("MODEL_ARCLIFT_GROUP",PROPLIST_ITEM(id++,"Բ��Ԥ����"));
	AddMenuHelpItem("MODEL_ARCLIFT_GROUP.ID_NEW_ITEM",PROPLIST_ITEM(id++,"Բ��Ԥ��->�½�Բ��Ԥ����"));
	AddMenuHelpItem("MODEL_ARCLIFT_CASE",PROPLIST_ITEM(id++,"Ԥ����"));
	AddMenuHelpItem("MODEL_ARCLIFT_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"Ԥ����->Բ��Ԥ������"));
	AddMenuHelpItem("MODEL_ARCLIFT_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"Ԥ����->ɾ��Բ��Ԥ����"));
	//��������
	AddMenuHelpItem("BLOCKREF_GROUP",PROPLIST_ITEM(id++,"����������"));
	AddMenuHelpItem("BLOCKREF_CASE",PROPLIST_ITEM(id++,"����������"));
	AddMenuHelpItem("BLOCKREF_CASE.ID_BLOCKREF_INTEGRATE",PROPLIST_ITEM(id++,"��������->����ģ��"));
	AddMenuHelpItem("BLOCKREF_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"��������->����"));
	AddMenuHelpItem("BLOCKREF_CASE.ID_EXPLODE_BLOCKREF",PROPLIST_ITEM(id++,"��������->ը��/ȡ��ը��"));
	AddMenuHelpItem("BLOCKREF_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"��������->ɾ����������"));
	//���Ӻ���
	AddMenuHelpItem("WORKSTATUS_ATTACH_WINDLOAD",PROPLIST_ITEM(id++,"���Ӻ�����"));
	AddMenuHelpItem("WORKSTATUS_ATTACH_WINDLOAD.ID_NEW_ITEM",PROPLIST_ITEM(id++,"���Ӻ�����->�½����Ӻ���"));
	AddMenuHelpItem("WORKSTATUS_ATTACH_WINDLOADITEM",PROPLIST_ITEM(id++,"������"));
	AddMenuHelpItem("WORKSTATUS_ATTACH_WINDLOADITEM.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"������->�޸ĸ��Ӻ���"));
	AddMenuHelpItem("WORKSTATUS_ATTACH_WINDLOADITEM.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"������->ɾ�����Ӻ���"));
	//������
	AddMenuHelpItem("MODEL_CASE",PROPLIST_ITEM(id++,"����"));
	AddMenuHelpItem("MODEL_CASE.ID_SET_ACTIVE_ITEM",PROPLIST_ITEM(id++,"����->�������"));
	AddMenuHelpItem("MODEL_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"����->��������"));
	AddMenuHelpItem("MODEL_CASE.ID_MODEL_DESIGN_CAL",PROPLIST_ITEM(id++,"����->ѡ������"));
	AddMenuHelpItem("MODEL_CASE.ID_MODEL_EQ_CHECK",PROPLIST_ITEM(id++,"����->��������"));
	AddMenuHelpItem("MODEL_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"����->ɾ������"));
	AddMenuHelpItem("MODEL_BURGLAR_LS_SCOPE",PROPLIST_ITEM(id++,"������˨"));
	AddMenuHelpItem("MODEL_LEG_CFGNO",PROPLIST_ITEM(id++,"������ĺ�"));
	AddMenuHelpItem("MODEL_LEG_CFGNO.ID_SET_ACTIVE_ITEM",PROPLIST_ITEM(id++,"������ĺ�->�������"));
	//Լ������
	AddMenuHelpItem("MODEL_RESTRICT",PROPLIST_ITEM(id++,"Լ��������"));
	AddMenuHelpItem("MODEL_RESTRICT.ID_NEW_ITEM",PROPLIST_ITEM(id++,"Լ��������->���Լ���ڵ�"));
	AddMenuHelpItem("MODEL_RESTRICT_NODE",PROPLIST_ITEM(id++,"Լ��������"));
	AddMenuHelpItem("MODEL_RESTRICT_NODE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"Լ��������->�޸�Լ��"));
	//���ع���
	AddMenuHelpItem("WORKSTATUS_GROUP",PROPLIST_ITEM(id++,"���ع�����"));
	AddMenuHelpItem("WORKSTATUS_GROUP.ID_NEW_ITEM",PROPLIST_ITEM(id++,"���ع�����->�½�����"));
	AddMenuHelpItem("WORKSTATUS_GROUP.ID_NEW_ITEM_SET",PROPLIST_ITEM(id++,"���ع�����->��Ϲ���"));
	AddMenuHelpItem("WORKSTATUS_CASE",PROPLIST_ITEM(id++,"���ع�����"));
	AddMenuHelpItem("WORKSTATUS_CASE.ID_SET_ACTIVE_ITEM",PROPLIST_ITEM(id++,"���ع�����->�����"));
	AddMenuHelpItem("WORKSTATUS_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"���ع�����->��������"));
	AddMenuHelpItem("WORKSTATUS_CASE.ID_COPY_ITEM",PROPLIST_ITEM(id++,"���ع�����->���ƹ���"));
	AddMenuHelpItem("WORKSTATUS_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"���ع�����->ɾ������"));
	//��ѹ��
	AddMenuHelpItem("WINDSEG_GROUP",PROPLIST_ITEM(id++,"��ѹ����"));
	AddMenuHelpItem("WINDSEG_GROUP.ID_NEW_ITEM",PROPLIST_ITEM(id++,"��ѹ����->��ӷ�ѹ��"));
	AddMenuHelpItem("WINDSEG_GROUP.ID_CAL_WIND_BETA_Z",PROPLIST_ITEM(id++,"��ѹ����->�������ϵ��"));
	//TODO:δ��ɰ����ĵ� wjh-2017.2.10
	AddMenuHelpItem("WINDSEG_GROUP.ID_RETRIEVE_BODY_WIND_SEGMENT",PROPLIST_ITEM(id++,"��ѹ����->��ȡ�����ѹ��"));
	AddMenuHelpItem("WINDSEG_GROUP.ID_EXPORT_LOADCAL_REPORT",PROPLIST_ITEM(id++,"��ѹ����->���ܷ���ؼ�����"));
	AddMenuHelpItem("WINDSEG_CASE",PROPLIST_ITEM(id++,"��ѹ����"));
	AddMenuHelpItem("WINDSEG_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"��ѹ����->��ѹ������"));
	AddMenuHelpItem("WINDSEG_CASE.ID_CAL_WIND_BETA_Z",PROPLIST_ITEM(id++,"��ѹ����->�������ϵ��"));
	AddMenuHelpItem("WINDSEG_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"��ѹ����->ɾ����ѹ��"));
	//����
	AddMenuHelpItem("BLOCK_CASE",PROPLIST_ITEM(id++,"����"));
	AddMenuHelpItem("BLOCK_CASE.ID_SET_ACTIVE_ITEM",PROPLIST_ITEM(id++,"����->�����"));
	AddMenuHelpItem("BLOCK_CASE.ID_EDIT_ITEM_PROP_ITEM",PROPLIST_ITEM(id++,"����->����"));
	AddMenuHelpItem("BLOCK_CASE.ID_NEW_ITEM",PROPLIST_ITEM(id++,"����->���ò���"));
	AddMenuHelpItem("BLOCK_CASE.ID_UPDATE_BLOCK",PROPLIST_ITEM(id++,"����->����"));
	AddMenuHelpItem("BLOCK_CASE.ID_EXPORT_BLOCK_FILE",PROPLIST_ITEM(id++,"����->����"));
	AddMenuHelpItem("BLOCK_CASE.ID_DELETE_ITEM",PROPLIST_ITEM(id++,"����->ɾ��"));
	//��˨��
	AddMenuHelpItem("BOLT_GROUP",PROPLIST_ITEM(id++,"��˨��"));
}
PROPLIST_ITEM* CTowerTreeDlg::AddMenuHelpItem(char* keyStr,PROPLIST_ITEM item)
{
	return AddUniversalPropItem(keyStr,item,&propMenuHashtable,&hashMenuHelpItemById);
}
void CTowerTreeDlg::ContextMenu(CWnd *pWnd, CPoint point)
{
	CPoint scr_point = point;
	CTreeCtrl *pTreeCtrl=GetTreeCtrl();
	pTreeCtrl->ClientToScreen(&scr_point);
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hItem);
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	TREEITEM_INFO *pItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL&&hItem)
		return;
	if(!IsDisplayContextMenu(pTreeCtrl,hItem))
		return;
	if(hItem==NULL||pItemInfo->itemType==VIEW_GROUP)
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_TREE,"Update");
	else if(pItemInfo->itemType==VIEW_LAYERSET)
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Layer View");
#else 
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_TREE,"ˢ��");
	else if(pItemInfo->itemType==VIEW_LAYERSET)
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½�ͼ����ͼ");
#endif
#if defined(__COMMON_PART_INC_) && !defined(__LDS_TSA_)
	else if(pItemInfo->itemType==MODEL_ARCLIFT_GROUP&&PRODUCT_FUNC::IsHasAutoArcLift())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Arc Prelift");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½�Բ��Ԥ����");
#endif
	}
	else if(pItemInfo->itemType==MODEL_ARCLIFT_CASE&&PRODUCT_FUNC::IsHasAutoArcLift())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Arc Prelift Parameter");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Arc Prelift");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Բ��Ԥ������");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��Բ��Ԥ����");
#endif
		if(theApp.m_bCooperativeWork)
		{
			pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"Authorize Arc Prelift");
#else 
		    pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"Բ��Ԥ������Ȩ");
#endif
		}
	}
#endif
	else if(pItemInfo->itemType==VIEW_DEFINEDSET)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_SORT_ITEM,"Sort");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"New Group Views");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Custom View");
		pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
		if(theApp.m_bAnalysisWorkEnv)
			pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_UDF_VIEW,"Retrieve UDF Mechanics Views");
		else
			pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_DRAWING_VIEW,"Retrieve Structure Views");
#else 
		pMenu->AppendMenu(MF_STRING,ID_SORT_ITEM,"����");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"�½���ͼ��");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½�������ͼ");
		pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
		if(theApp.m_bAnalysisWorkEnv)
			pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_UDF_VIEW,"������ȡ˾��ͼ");
		else
			pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_DRAWING_VIEW,"������ȡ�ṹͼ");
#endif
		//pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_UDF_VIEW,"������ȡ�ṹͼ");
	}
	else if(pItemInfo->itemType==VIEW_DEFINEDFOLDER)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_SORT_ITEM,"Sort");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Property");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete");
#else 
		pMenu->AppendMenu(MF_STRING,ID_SORT_ITEM,"����");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"����");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��");
#endif
	}
	else if(pItemInfo->itemType==VIEW_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Active View");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"View Property");
#else 
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"������ͼ");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"��ͼ����");
#endif
	#if defined(__SMART_DRAW_INC_)&&defined(__LDS_CONTEXT_)// __LDS_
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hItem);
		TREEITEM_INFO *pParentItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		if(pParentItemInfo->itemType==VIEW_LAYERSET)	//Ŀǰֻ��LDS��֧��ͼ����ͼ�ķֶ�����
			pMenu->AppendMenu(MF_STRING,ID_LAYERVIEW_SEG_PROP_ITEM,"��ͼ�ֶ�����");
	#endif
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete View");
#else 
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ����ͼ");
#endif
		if(theApp.m_bCooperativeWork)
		{
			pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"Authorize View");
#else 
			pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"��ͼ��Ȩ");
#endif
		}
	}
	else if(pItemInfo->itemType==BLOCK_CASE)
	{
		if(PRODUCT_FUNC::IsHasBlockSetup())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Active Block");
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Property");
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"Block Assembly");
			pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
			pMenu->AppendMenu(MF_STRING,ID_UPDATE_BLOCK,"Update");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_BLOCK_FILE,"Export");
			pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete");
#else 
			pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"�����");
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"����");
			pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"װ�䲿��");
			pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
			pMenu->AppendMenu(MF_STRING,ID_UPDATE_BLOCK,"����");
			pMenu->AppendMenu(MF_STRING,ID_EXPORT_BLOCK_FILE,"����");
			pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��");
#endif
			if(theApp.m_bCooperativeWork)
			{
				pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
#ifdef AFX_TARG_ENU_ENGLISH
				pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"Authorize Block");
#else 
				pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"������Ȩ");
#endif
			}
		}
	}
	else if(pItemInfo->itemType==BLOCKREF_CASE)
	{
		if(PRODUCT_FUNC::IsHasBlockSetup())
		{
			CBlockReference* pBlockRef=(CBlockReference*)pItemInfo->dwRefData;
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_BLOCKREF_INTEGRATE,"Integrate Model");
			pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Block Assembly Property");
			if(pBlockRef->IsExploded())
				pMenu->AppendMenu(MF_STRING,ID_EXPLODE_BLOCKREF,"Cancel Explode");
			else
				pMenu->AppendMenu(MF_STRING,ID_EXPLODE_BLOCKREF,"Explode");
			pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Block Assembly");
#else 
			pMenu->AppendMenu(MF_STRING,ID_BLOCKREF_INTEGRATE,"����ģ��");
			pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
			pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"������������");
			if(pBlockRef->IsExploded())
				pMenu->AppendMenu(MF_STRING,ID_EXPLODE_BLOCKREF,"ȡ��ը��");
			else
				pMenu->AppendMenu(MF_STRING,ID_EXPLODE_BLOCKREF,"ը��");
			pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ����������");
#endif
			if(theApp.m_bCooperativeWork)
			{
				pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
#ifdef AFX_TARG_ENU_ENGLISH
				pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"Authorize Block Assembly");
#else 
				pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"����������Ȩ");
#endif
			}
		}
	}
	else if(pItemInfo->itemType==DATA_SPACE)
	{
		if(PRODUCT_FUNC::IsHasBlockSetup())
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_BLOCK_FILE,"Import Block");
#else 
			pMenu->AppendMenu(MF_STRING,ID_IMPORT_BLOCK_FILE,"���벿��");
#endif
	}
	//����ģ��
	else if(pItemInfo->itemType==MODEL_GROUP)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_TREE,"Update");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Property");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Model");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_INTELLI_CORRECT_OBJ_LAYER,"Layer Intelli Correction");
#ifdef __COMMON_PART_INC_
		pMenu->AppendMenu(MF_STRING,ID_UNIFY_ANGLE_BOLT_G,"Unify Bolt's G Parameter");
#endif
#else 
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_TREE,"ˢ��");
		//pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"����");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"����");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½�����");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_INTELLI_CORRECT_OBJ_LAYER,"��������ͼ����");
#ifdef __COMMON_PART_INC_
		pMenu->AppendMenu(MF_STRING,ID_UNIFY_ANGLE_BOLT_G,"�淶��˨�ľ����");
#endif
#endif
#ifdef __SMART_DRAW_INC_
		pMenu->AppendMenu(MF_STRING,ID_GENERATE_ROUGH_3D_MODEL,"���ø˼���λ");
#endif
#ifdef __PART_DESIGN_INC_
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_MODEL_DESIGN_CAL,"ѡ������");
#endif
		pMenu->AppendMenu(MF_STRING,ID_SORT_MODULE_BY_HEIGHT,"��������");
//#ifdef __PART_DESIGN_INC_
		pMenu->AppendMenu(MF_STRING,ID_SET_HEIGHT_GROUP_NAME,"���߱���");
//#endif
	}
	else if(pItemInfo->itemType==MODEL_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Active Model");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Model Property");
#else 
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"�������");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"��������");
#endif
#if defined(__LDS_)||defined(__TSA_)
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_MODEL_DESIGN_CAL,"ѡ������");
		pMenu->AppendMenu(MF_STRING,ID_MODEL_EQ_CHECK,"��������");
#endif
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Model");
#else 
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ������");
#endif
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(pItemInfo->itemType==MODEL_QUAD1LEG_CFGNO)
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Active Joint Leg");
	else if(pItemInfo->itemType==MODEL_QUAD2LEG_CFGNO)
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Active Joint Leg");
	else if(pItemInfo->itemType==MODEL_QUAD3LEG_CFGNO)
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Active Joint Leg");
	else if(pItemInfo->itemType==MODEL_QUAD4LEG_CFGNO)
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Active Joint Leg");
#else 
	else if(pItemInfo->itemType==MODEL_QUAD1LEG_CFGNO)
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"�������");
	else if(pItemInfo->itemType==MODEL_QUAD2LEG_CFGNO)
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"�������");
	else if(pItemInfo->itemType==MODEL_QUAD3LEG_CFGNO)
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"�������");
	else if(pItemInfo->itemType==MODEL_QUAD4LEG_CFGNO)
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"�������");
#endif
	else if(pItemInfo->itemType==MODEL_BURGLAR_LS_SCOPE||pItemInfo->itemType==MODEL_BODY_LEGNO)
		;
	else if(pItemInfo->itemType==MODEL_DATUM_POINT_GROUP)
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Datum Point");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½���׼��");
#endif
	else if(pItemInfo->itemType==MODEL_DATUM_POINT_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Property");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Datum Point");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"����");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ����׼��");
#endif
		if(theApp.m_bCooperativeWork)
		{
			pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"Authorize Datum Point");
#else 
			pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"��׼����Ȩ");
#endif
		}
	}
	else if(pItemInfo->itemType==MODEL_DATUM_LINE_GROUP)
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Datum Line");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½���׼��");
#endif
	else if(pItemInfo->itemType==MODEL_DATUM_LINE_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Property");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Datum Line");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"����");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ����׼��");
#endif
		if(theApp.m_bCooperativeWork)
		{
			pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"Authorize Datum Line");
#else 
			pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"��׼����Ȩ");
#endif
		}
	}                            
	else if(pItemInfo->itemType==MODEL_DATUM_PLANE_GROUP)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Datum Plane");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½���׼��");
#endif
	}
	else if(pItemInfo->itemType==MODEL_DATUM_PLANE_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Property");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Datum Plane");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"����");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ����׼��");
#endif
		if(theApp.m_bCooperativeWork)
		{
			pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"Authorize Datum Plane");
#else 
			pMenu->AppendMenu(MF_STRING,ID_HANDOVER_PERMISSION,"��׼����Ȩ");
#endif
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	else if(pItemInfo->itemType==MODEL_RESTRICT)
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"Add Constraint Node");
	else if(pItemInfo->itemType==MODEL_RESTRICT_NODE)
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Modify Constraint");
#else 
	else if(pItemInfo->itemType==MODEL_RESTRICT)
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"���Լ���ڵ�");
	else if(pItemInfo->itemType==MODEL_RESTRICT_NODE)
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�޸�Լ��");
#endif
	else if(pItemInfo->itemType==WORKSTATUS_GROUP)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Work Condition");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_SET,"Group Work Condition");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½�����");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_SET,"��Ϲ���");
#endif
		//pMenu->AppendMenu(MF_STRING,ID_ITEM_LIST_PROP,"�����嵥");
	}
	else if(pItemInfo->itemType==WIRELAYOUT_GROUP)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Load Scheme");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½����ط���");
#endif
	}
	else if(pItemInfo->itemType==WIRELAYOUT_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Rename");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_CAL_WIRE_LOAD,"Calculate Wire Load");
		pMenu->AppendMenu(MF_STRING,ID_NODELOAD_LIST,"Add Concentrated Load");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_LOADCAL_REPORT,"Load Calculate Book");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"������");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_CAL_WIRE_LOAD,"������������");
		pMenu->AppendMenu(MF_STRING,ID_NODELOAD_LIST,"��Ӽ��к���");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_LOADCAL_REPORT,"���ؼ�����");
#endif
	}
	else if(pItemInfo->itemType==WIRELOAD_MODEL_GROUP)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Wire Load");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_SET,"Generate Wire Load");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½���������");
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_SET,"���ɳ�����������");
#endif
	}
	else if(pItemInfo->itemType==WIRELOAD_MODEL_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Property");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"����");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��");
#endif
	}
	else if(pItemInfo->itemType==WORKSTATUS_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Active Work Condition");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Work Condition Property");
		pMenu->AppendMenu(MF_STRING,ID_COPY_ITEM,"Copy Work Condition");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Work Condition");
#else 
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"�����");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"��������");
		pMenu->AppendMenu(MF_STRING,ID_COPY_ITEM,"���ƹ���");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ������");
#endif
	}
	else if(pItemInfo->itemType==WORKSTATUS_WEIGHTLOAD)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Modify Gravity Load Coefficient");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�޸���������ϵ��");
#endif
	}
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOAD)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"New Attached Load");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"�½����Ӻ���");
#endif
	}
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOADREF)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,IDD_ATTACH_WINDLOAD,"Attached Load");
#else 
		pMenu->AppendMenu(MF_STRING,IDD_ATTACH_WINDLOAD,"���Ӻ���");
#endif
	}
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOADITEM)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Modify Attached Load");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Attached Load");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�޸ĸ��Ӻ���");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ�����Ӻ���");
#endif
	}
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOADREFITEM)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Modify Shelt Wind Coefficient");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Attached Load");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"�޸ĵ���ϵ��");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"�Ƴ����Ӻ���");
#endif
	}
	else if(pItemInfo->itemType==WINDSEG_GROUP)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(hParentItem==m_hBodyItem)
		{
			pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_BODY_WIND_SEGMENT,"Retrieve trunk wind segments");
			pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
		}
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"Delete Wind Pressure");
		pMenu->AppendMenu(MF_STRING,ID_CAL_WIND_BETA_Z,"Calculate Wind Vibrate Coefficient");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_LOADCAL_REPORT,"Wind Load Calculate Book");
#else 
		if(hParentItem==m_hBodyItem)
		{
			pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_BODY_WIND_SEGMENT,"�Զ���ȡ�����ѹ��");
			pMenu->AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
		}
		//else hParentItem.itemType=MODEL_CASE �����ܷ�ѹ�νڵ��µ����Ҽ��˵����������µ��ӷ�ѹ�������ʾ wjh-2017.2.10
		pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"��ӷ�ѹ��");
		pMenu->AppendMenu(MF_STRING,ID_CAL_WIND_BETA_Z,"�������ϵ��");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_LOADCAL_REPORT,"���ܷ���ؼ�����");
#endif
	}
	else if(pItemInfo->itemType==WINDSEG_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"Wind Pressure Property");
		pMenu->AppendMenu(MF_STRING,ID_CAL_WIND_BETA_Z,"Calculate Wind Vibrate Coefficient");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Wind Pressure");
#else 
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"��ѹ������");
		pMenu->AppendMenu(MF_STRING,ID_CAL_WIND_BETA_Z,"�������ϵ��");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ����ѹ��");
#endif
	}
	else
		return;

	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

void CTowerTreeDlg::OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	pTreeCtrl->ScreenToClient(&HitTestInfo.pt);
	pTreeCtrl->HitTest(&HitTestInfo);
	BOOL bSelect=FALSE;
	TREEITEM_INFO* pSelInfo=(HitTestInfo.hItem==NULL)?NULL:(TREEITEM_INFO*)pTreeCtrl->GetItemData(HitTestInfo.hItem);
	for(HTREEITEM hSelectItem=pTreeCtrl->GetFirstSelectedItem();hSelectItem;hSelectItem=pTreeCtrl->GetNextSelectedItem(hSelectItem))
	{
		TREEITEM_INFO* pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSelectItem);
		if(pItemInfo==NULL||pSelInfo==NULL)
			continue;
		if(pSelInfo->itemType!=pItemInfo->itemType)
		{
			bSelect=TRUE;
			break;
		}
	}
	if(pTreeCtrl->GetSelectedCount()==1||bSelect)
		pTreeCtrl->Select(HitTestInfo.hItem,TVGN_CARET);
	//else
	//{
	//	int count=pTreeCtrl->GetSelectedCount();
	//	HTREEITEM hCurrSelItem=pTreeCtrl->GetSelectedItem();
	//	pTreeCtrl->SetItemState(hCurrSelItem,TVIS_SELECTED,TVIS_SELECTED);
	//}
	ContextMenu(this,HitTestInfo.pt);
	*pResult = 0;
}
void CTowerTreeDlg::OnExpandingTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	HTREEITEM hParentItem=hItem!=NULL?pTreeCtrl->GetParentItem(hItem):NULL;
	TREEITEM_INFO *pItemInfo=hItem!=NULL?(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem):NULL;
	TREEITEM_INFO *pParentItemInfo=hParentItem!=NULL?(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem):NULL;
#ifdef __FEM_ANALYZE_INC_
	if(pItemInfo!=NULL&&pItemInfo->itemType==MODEL_CASE)
	{
		HTREEITEM hSubItem=pTreeCtrl->GetNextItem(hItem,TVGN_CHILD);
		while(hSubItem!=NULL)
		{
			TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
			if(pItemInfo!=NULL&&pItemInfo->itemType==WINDSEG_GROUP)
				break;
			hSubItem=pTreeCtrl->GetNextSiblingItem(hSubItem);
		}
		if(hSubItem!=NULL)
			RefreshHeightWindSegSetItem(hSubItem,(CLDSModule*)pItemInfo->dwRefData);
	}
	else if(pItemInfo!=NULL&&pItemInfo->itemType==WINDSEG_GROUP&&
		pParentItemInfo!=NULL&&pParentItemInfo->itemType==MODEL_CASE)
		RefreshHeightWindSegSetItem(hItem,(CLDSModule*)pParentItemInfo->dwRefData);
#endif
}
//<DEVELOP_PROCESS_MARK nameId="CTowerTreeDlg.OnEditItemPropItem">
void CTowerTreeDlg::OnEditItemPropItem() 
{
#ifdef _RELATIVE_DLG
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL,*pSubItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	//����ģ��
	if(pItemInfo->itemType==MODEL_GROUP)
	{
		CFilePropertyDlg dlg;
		dlg.DoModal();
	}
	else if(pItemInfo->itemType==MODEL_ARCLIFT_CASE)
	{	//�༭Ԥ�������
#ifdef __ANGLE_PART_INC_
		CLDSArcLift *pArcLift=(CLDSArcLift*)pItemInfo->dwRefData;
		if(pArcLift)
		{
			CArcLiftDlg arcLiftDlg;
			arcLiftDlg.m_xArcLift=*pArcLift;
			if(arcLiftDlg.DoModal()==IDOK)
			{
				*pArcLift=arcLiftDlg.m_xArcLift;
				//����Ԥ��������
				CString ss;
				ss.Format("%s(%d)",pArcLift->name,pArcLift->iNo);
				pTreeCtrl->SetItemText(hItem,ss);
			}
		}
#endif
	}
	else if(pItemInfo->itemType==MODEL_CASE)
	{	//ģ��ʵ��
		CModelPropDlg dlg;
		CLDSModule *pModule=(CLDSModule*)pItemInfo->dwRefData;
		if(pModule)
		{
			dlg.m_sModelName =pModule->description;
			dlg.m_iBodyLegNo=pModule->GetBodyNo();
			dlg.m_dwLegCfgNo=pModule->m_dwLegCfgWord;
			dlg.m_fLegLevelHeightDiff=pModule->m_wnLevelHeight*0.001;
			dlg.m_fBurglarLsStartZ = pModule->m_fBurglarLsStartZ;
			dlg.m_fBurglarLsEndZ   = pModule->m_fBurglarLsEndZ;
			dlg.m_fLockPadStartZ = pModule->m_fLockPadStartZ;
			dlg.m_fLockPadEndZ = pModule->m_fLockPadEndZ;
#if defined(__LDS_)||defined(__TSA_)
			dlg.m_fLandHigh = pModule->land_high;
#endif
			dlg.m_pRelaHeight = pModule;
			dlg.m_sIncPartSegStr   = pModule->segStr;
			if(dlg.DoModal()==IDOK)
			{
				sprintf(pModule->description,"%s",dlg.m_sModelName);
				_snprintf(pModule->segStr,99,"%s",dlg.m_sIncPartSegStr);
				pModule->SetBodyNo(dlg.m_iBodyLegNo);
				pModule->m_dwLegCfgWord=dlg.m_dwLegCfgNo;
				pModule->uiLevelHeight=ftoi(dlg.m_fLegLevelHeightDiff*1000);
				pModule->m_fBurglarLsStartZ = dlg.m_fBurglarLsStartZ;
				pModule->m_fBurglarLsEndZ   = dlg.m_fBurglarLsEndZ;
				pModule->m_fLockPadStartZ = dlg.m_fLockPadStartZ;
				pModule->m_fLockPadEndZ = dlg.m_fLockPadEndZ;
			#if defined(__LDS_)||defined(__TSA_)
				pModule->land_high = dlg.m_fLandHigh;
			#endif
				if(pModule==Ta.GetActiveModule())
				{	//����ˢ���Ҳ���ʾ�����ڵ���ʾ״̬
				#ifdef __LDS_CONTEXT_// defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
					CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
					pView->RedrawAll();
				#elif defined(__LDS_GROUP_CONTEXT_)//defined(__LDSGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)||defined(__TSAGROUP_)
					CTmaGroupView *pView=(CTmaGroupView*)theApp.GetTmaGroupDoc()->GetView(RUNTIME_CLASS(CTmaGroupView));
					pView->RedrawAll();
				#endif
				}
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
				dlg.m_sModelName.Format("%s(%d)",pModule->description,pModule->iNo);
#else
				dlg.m_sModelName.Format("%s",pModule->description);
#endif
				pTreeCtrl->SetItemText(hItem,dlg.m_sModelName);
				HTREEITEM hSubItem=pTreeCtrl->GetChildItem(hItem);
				while(hSubItem)
				{
					pSubItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSubItem);
					CString ss, sTemp;
					int nMaxLegs=CFGLEG::MaxLegs();
					if(pSubItemInfo->itemType==MODEL_QUAD1LEG_CFGNO)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						ss="Joint Leg(1)Model Flag:";
#else 
						ss="����(1)��ĺ�:";
#endif
						for(int i=1;i<=192;i++)
						{
							if(pModule->m_dwLegCfgWord.IsHasNo(i))
							{
								if(!(pModule->m_dwLegCfgWord.IsHasNo(pModule->m_arrActiveQuadLegNo[0])))
									pModule->m_arrActiveQuadLegNo[0]=i;
								if(pModule->m_arrActiveQuadLegNo[0]==i)
									sTemp.Format("%C(*),",(i-1)%nMaxLegs+'A');
								else
									sTemp.Format("%C,",(i-1)%nMaxLegs+'A');
								ss+=sTemp;
							}
						}
						ss=ss.Left(ss.GetLength()-1);
						pTreeCtrl->SetItemText(hSubItem,ss);
					}
					else if(pSubItemInfo->itemType==MODEL_QUAD2LEG_CFGNO)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						ss="Joint Leg(2)Model Flag:";
#else 
						ss="����(2)��ĺ�:";
#endif
						for(int i=1;i<=192;i++)
						{
							if(pModule->m_dwLegCfgWord.IsHasNo(i))
							{
								if(!(pModule->m_dwLegCfgWord.IsHasNo(pModule->m_arrActiveQuadLegNo[1])))
									pModule->m_arrActiveQuadLegNo[1]=i;
								if(pModule->m_arrActiveQuadLegNo[1]==i)
									sTemp.Format("%C(*),",(i-1)%nMaxLegs+'A');
								else
									sTemp.Format("%C,",(i-1)%nMaxLegs+'A');
								ss+=sTemp;
							}
						}
						ss=ss.Left(ss.GetLength()-1);
						pTreeCtrl->SetItemText(hSubItem,ss);
					}
					else if(pSubItemInfo->itemType==MODEL_QUAD3LEG_CFGNO)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						ss="Joint Leg(3)Model Flag:";
#else 
						ss="����(3)��ĺ�:";
#endif
						for(int i=1;i<=192;i++)
						{
							if(pModule->m_dwLegCfgWord.IsHasNo(i))
							{
								if(!(pModule->m_dwLegCfgWord.IsHasNo(pModule->m_arrActiveQuadLegNo[2])))
									pModule->m_arrActiveQuadLegNo[2]=i;
								if(pModule->m_arrActiveQuadLegNo[2]==i)
									sTemp.Format("%C(*),",(i-1)%nMaxLegs+'A');
								else
									sTemp.Format("%C,",(i-1)%nMaxLegs+'A');
								ss+=sTemp;
							}
						}
						ss=ss.Left(ss.GetLength()-1);
						pTreeCtrl->SetItemText(hSubItem,ss);
					}
					else if(pSubItemInfo->itemType==MODEL_QUAD4LEG_CFGNO)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						ss="Joint Leg(4)Model Flag:";
#else 
						ss="����(4)��ĺ�:";
#endif
						for(int i=1;i<=192;i++)
						{
							if(pModule->m_dwLegCfgWord.IsHasNo(i))
							{
								if(!(pModule->m_dwLegCfgWord.IsHasNo(pModule->m_arrActiveQuadLegNo[3])))
									pModule->m_arrActiveQuadLegNo[3]=i;
								if(pModule->m_arrActiveQuadLegNo[3]==i)
									sTemp.Format("%C(*),",(i-1)%nMaxLegs+'A');
								else
									sTemp.Format("%C,",(i-1)%nMaxLegs+'A');
								ss+=sTemp;
							}
						}
						ss=ss.Left(ss.GetLength()-1);
						pTreeCtrl->SetItemText(hSubItem,ss);
					}
					else if(pSubItemInfo->itemType==MODEL_BODY_LEGNO)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						ss.Format("Body No:%d",dlg.m_iBodyLegNo);
#else 
						ss.Format("�����:%d",dlg.m_iBodyLegNo);
#endif
						pTreeCtrl->SetItemText(hSubItem,ss);
					}
#ifdef __COMMON_PART_INC_//defined(__TSA_)||defined(__TSA_FILE_)
					else if(pSubItemInfo->itemType==MODEL_BURGLAR_LS_SCOPE)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						ss.Format("Anti-theft Bolt Z Scope:%.0f~%.0f",dlg.m_fBurglarLsStartZ,dlg.m_fBurglarLsEndZ);
#else 
						ss.Format("������˨Z���귶Χ:%.0f~%.0f",dlg.m_fBurglarLsStartZ,dlg.m_fBurglarLsEndZ);
#endif
						pTreeCtrl->SetItemText(hSubItem,ss);
					}
#endif
					else if(pSubItemInfo->itemType==MODEL_LAND_HIGH)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						ss.Format("Geography Height��%f",dlg.m_fLandHigh);
#else 
						ss.Format("���θߣ�%f",dlg.m_fLandHigh);
#endif
						SimplifiedNumString(ss);
						pTreeCtrl->SetItemText(hSubItem,ss);
					}
					hSubItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_NEXT);
				}
				if(pModule->handle==Ta.m_hActiveModule)
				{
					for(int iLeg=0;iLeg<4;iLeg++)
						Ta.m_arrActiveQuadLegNo[iLeg]=pModule->m_arrActiveQuadLegNo[iLeg];
				}
#ifdef __COMMON_PART_INC_//!defined(__TSA_)&&!defined(__TSA_FILE_)
				Ta.RefreshBurglarLs();	//ˢ�·�����˨
				Ta.RefreshLockPad();	//ˢ�·��ɵ�Ȧ
#endif
			}
			else
				return;
		}
	}
	else if(pItemInfo->itemType==MODEL_QUAD1LEG_CFGNO)
	{	//1���޽�����ĺ�
	}
	else if(pItemInfo->itemType==MODEL_QUAD2LEG_CFGNO)
	{	//2���޽�����ĺ�
	}
	else if(pItemInfo->itemType==MODEL_QUAD3LEG_CFGNO)
	{	//3���޽�����ĺ�
	}
	else if(pItemInfo->itemType==MODEL_QUAD4LEG_CFGNO)
	{	//4���޽�����ĺ�
	}
	else if(pItemInfo->itemType==MODEL_BODY_LEGNO)
	{	//�������
	}
	else if(pItemInfo->itemType==MODEL_DATUM_POINT_CASE)
	{	//�༭��׼��
		CDatumPointPropertyDlg dlg;
		for(CLDSPoint *pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
		{
			if(pPoint==(void*)pItemInfo->dwRefData)
				break;
		}
		if(pPoint)
		{
			//dlg.m_shDatumPoint.Format("��׼����:0X%X",pPoint->handle);
			//dlg.m_sPointDescription =pPoint->description;
			//dlg.m_iDatumPosCalStyle =pPoint->datum_pos_style;
			dlg.m_xPoint=*pPoint;
			if(dlg.DoModal()==IDOK)
			{
				*pPoint=dlg.m_xPoint;
				//sprintf(pPoint->description,"%s",dlg.m_sPointDescription);
				pPoint->UpdatePos(console.GetActiveModel());
				CString ss;
				ss.Format("%s_0X%X",pPoint->description,pPoint->handle);//,pPoint->iNo);
				pTreeCtrl->SetItemText(hItem,ss);
			}
		}
	}
	else if(pItemInfo->itemType==MODEL_DATUM_LINE_CASE)
	{	//�༭��׼��
		//CDatumLineDlg dlg;
		CDatumLinePropertyDlg	dlg;
		for(CLDSLine *pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
		{
			if(pLine==(void*)pItemInfo->dwRefData)
				break;
		}
		dlg.m_datumLine=*pLine;
		if(pLine && dlg.DoModal()==IDOK)
		{
			HTREEITEM hLineGroupItem=pTreeCtrl->GetParentItem(hItem);
			HTREEITEM hPointGroupItem=pTreeCtrl->GetPrevSiblingItem(hLineGroupItem);	//��������ͼ�µĻ�׼��ڵ�
			TREEITEM_INFO *pItem=NULL;
			CLDSPoint* pDatumPoint=NULL;
			CString ss;
			if(dlg.m_bNewStartPoint)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(dlg.m_startPoint.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			if(dlg.m_bNewEndPoint)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(dlg.m_endPoint.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			*pLine=dlg.m_datumLine;
			ss.Format("%s_0X%X",pLine->description,pLine->handle);//,pLine->iNo);
			pTreeCtrl->SetItemText(hItem,ss);
		}
	}
	else if(pItemInfo->itemType==MODEL_DATUM_PLANE_CASE)
	{	//�༭��׼��
		//CDatumPlaneDlg dlg;
		CDatumPlanePropertyDlg dlg;
		for(CLDSPlane *pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
		{
			if(pPlane==(void*)pItemInfo->dwRefData)
				break;
		}
		dlg.m_datumPlane=*pPlane;
		if(pPlane && dlg.DoModal()==IDOK)
		{
			HTREEITEM hPlaneGroupItem=pTreeCtrl->GetParentItem(hItem);
			HTREEITEM hLineGroupItem=pTreeCtrl->GetPrevSiblingItem(hPlaneGroupItem);	//��������ͼ�µĻ�׼�߽ڵ�
			HTREEITEM hPointGroupItem=pTreeCtrl->GetPrevSiblingItem(hLineGroupItem);	//��������ͼ�µĻ�׼��ڵ�
			TREEITEM_INFO *pItem=NULL;
			CLDSPoint* pDatumPoint=NULL;
			CString ss;
			if(dlg.m_bNewPoint1)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(dlg.m_datumPoint1.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			if(dlg.m_bNewPoint2)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(dlg.m_datumPoint2.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			if(dlg.m_bNewPoint3)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(dlg.m_datumPoint3.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			*pPlane=dlg.m_datumPlane;
			pPlane->UpdatePlane(console.GetActiveModel());
			ss.Format("%s_0X%X",pPlane->description,pPlane->handle);
			pTreeCtrl->SetItemText(hItem,ss);
		}
	}
	else if(pItemInfo->itemType==VIEW_CASE)
	{	//��ͼ����
		for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView==(void*)pItemInfo->dwRefData)
				break;
		}
		if(pView&&pView->m_iViewType==0)
		{
			CLayerViewDlg layerViewdlg;
			layerViewdlg.m_sViewName = pView->name;
			layerViewdlg.ucs = pView->ucs;
			layerViewdlg.m_sDefaultLayer = pView->defaultLayer;
			layerViewdlg.m_sFirstCharSet = pView->filter.FirstCharSet;
			layerViewdlg.m_sSecondCharSet = pView->filter.SecondCharSet;
			layerViewdlg.m_sThirdCharSet = pView->filter.ThirdCharSet;
			layerViewdlg.m_pCurView=pView;
			if(layerViewdlg.DoModal()==IDOK)
			{
				sprintf(pView->filter.FirstCharSet, "%s", layerViewdlg.m_sFirstCharSet);
				sprintf(pView->filter.SecondCharSet, "%s", layerViewdlg.m_sSecondCharSet);
				sprintf(pView->filter.ThirdCharSet, "%s", layerViewdlg.m_sThirdCharSet);
				_snprintf(pView->defaultLayer,3,"%s",layerViewdlg.m_sDefaultLayer);
				sprintf(pView->name, "%s", layerViewdlg.m_sViewName);
				pView->ucs=layerViewdlg.ucs;
				//��ʾ��ͼ
				CString ss;
				ss.Format("%s",pView->name);
				pTreeCtrl->SetItemText(hItem,ss);
				pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_CASE,(DWORD)pView));
				pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
				//�޸���ͼ���Ժ�Ӧ�ü����޸ĵ���ͼ
				pTreeCtrl->SelectItem(hItem);
				//���ǰ��ͼ
				OnSetActiveItem();
			}
		}
		else if(pView&&pView->m_iViewType==1)
		{
			CDefinedViewPropDlg definedViewDlg;
			/*definedViewDlg.m_sViewName = pView->name;
			definedViewDlg.ucs=pView->ucs;
			definedViewDlg.m_sFolderName = pView->folderName;
			definedViewDlg.m_sDefaultLayer = pView->defaultLayer;*/
			definedViewDlg.m_pDefinedView = pView;
			if(definedViewDlg.DoModal()==IDOK)
			{
				//_snprintf(pView->defaultLayer,3,"%s",definedViewDlg.m_sDefaultLayer);
				//_snprintf(pView->folderName,16,"%s",definedViewDlg.m_sFolderName);
				//sprintf(pView->name, "%s", definedViewDlg.m_sViewName);
				//pView->ucs = definedViewDlg.ucs;
				//��ʾ��ͼ
				CString ss;
				ss.Format("%s",pView->name);
				pTreeCtrl->SetItemText(hItem,ss);
				pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_CASE,(DWORD)pView));
				pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
				//�޸���ͼ���Ժ�Ӧ�ü����޸ĵ���ͼ
				pTreeCtrl->SelectItem(hItem);
				//���ǰ��ͼ
				OnSetActiveItem();
			}
		}
	}
	else if(pItemInfo->itemType==VIEW_DEFINEDFOLDER)
	{	//������
		for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView==(void*)pItemInfo->dwRefData)
				break;
		}
		if(pView&&pView->m_iViewType==2)
		{
			CString ss;
			CFolderNameDlg folderNameDlg;
			folderNameDlg.m_sFolderName = pView->folderName;
			folderNameDlg.m_pCurView=pView;
			folderNameDlg.m_nNameMaxLen = 16;
			folderNameDlg.m_nDlgType = 1;	//������ͼ������
			if(folderNameDlg.DoModal()==IDOK)
			{
				ss.Format("%s",folderNameDlg.m_sFolderName);
				pTreeCtrl->SetItemText(hItem,ss);
				CString sFolderName=pView->folderName;
				for(CDisplayView *pTempView=Ta.GetFirstDisplayView();pTempView;pTempView=Ta.GetNextDisplayView())
				{	//�����޸ĺ�ͬʱ�޸�(���ڸ����)��ͼ������
					if(sFolderName.CompareNoCase(pTempView->folderName)==0)
						_snprintf(pTempView->folderName,16,"%s",folderNameDlg.m_sFolderName);
				}
			}
		}
	}
#if defined(__LDS_)||defined(__TSA_)
	else if(pItemInfo->itemType==MODEL_RESTRICT_NODE)
	{	//ģ��Լ���ڵ�
		CSetNodeRestrictDlg dlg;
		CString ss=pTreeCtrl->GetItemText(hItem);
		THANDLE hNode;
		sscanf(ss,"%X",&hNode);
		CLDSNode *pNode=console.FromNodeHandle(hNode);

		HTREEITEM hParent=pTreeCtrl->GetParentItem(hItem);	//����Լ������һ��
		hParent=pTreeCtrl->GetParentItem(hParent);	//����ģ��һ��
		TREEITEM_INFO *pParentInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParent);
		if(pParentInfo==NULL)
			return;
		CLDSModule *pModule=(CLDSModule*)pParentInfo->dwRefData;
		if(pModule)
		{
			if(pNode)
			{
				dlg.m_bRestrictX=pNode->IsHasRestrict(pModule->handle,X_RESTRICT);
				dlg.m_bRestrictY=pNode->IsHasRestrict(pModule->handle,Y_RESTRICT);
				dlg.m_bRestrictZ=pNode->IsHasRestrict(pModule->handle,Z_RESTRICT);
				dlg.m_hNode.Format("%X",pNode->handle);
				dlg.m_nNodeI=pNode->point_i;
				if(dlg.DoModal()==IDOK)
				{
					pNode->SetRestrict(pModule->handle,X_RESTRICT,dlg.m_bRestrictX);
					pNode->SetRestrict(pModule->handle,Y_RESTRICT,dlg.m_bRestrictY);
					pNode->SetRestrict(pModule->handle,Z_RESTRICT,dlg.m_bRestrictZ);
					if(dlg.m_bRestrictX||dlg.m_bRestrictY||dlg.m_bRestrictZ)
					{
						ss.Format("0X%X(%d)��",pNode->handle,pNode->point_i);
						int iOrder=0;
						if(pNode->IsHasRestrict(pModule->handle,X_RESTRICT))
						{
							ss+="Rx";
							iOrder++;
						}
						if(pNode->IsHasRestrict(pModule->handle,Y_RESTRICT))
						{
							if(iOrder>0)
								ss+=";Ry";
							else
								ss+="Ry";
							iOrder++;
						}
						if(pNode->IsHasRestrict(pModule->handle,Z_RESTRICT))
						{
							if(iOrder>0)
								ss+=";Rz";
							else
								ss+="Rz";
						}
						pTreeCtrl->SetItemText(hItem,ss);
					}
					else
						pTreeCtrl->DeleteItem(hItem);
				}
			}
			else
				AfxMessageBox("ѡ��Լ���ڵ㲻���ڿ����ѱ�ɾ��!");
		}
	}
	/*else if(pItemInfo->itemType==WIRELAYOUT_CASE)
	{
		CWireLayout *pWireLay=(CWireLayout*)pItemInfo->dwRefData;
		if(pWireLay)
		{
			CString name=pWireLay->name;
			Input("�º��ط�������:",&name,'s');
			strncpy(pWireLay->name,name,49);
			name.Format("%s(%d)",pWireLay->name,pWireLay->iNo);
			pTreeCtrl->SetItemText(hItem,name);
			Ta.SetDocModifiedFlag();
		}
	}*/
	else if(pItemInfo->itemType==WORKSTATUS_GROUP)
	{	//������
	}
	/*else if(pItemInfo->itemType==WIRELOAD_MODEL_CASE)
	{	//����ʵ��
		CWorkEnvLoadModelDlg dlg;
		CWireLoadModel *pModel = (CWireLoadModel *)pItemInfo->dwRefData;
		dlg.wireModel.CopyWireLoadModel(pModel);
		if(IDOK == dlg.DoModal())
			pModel->CopyWireLoadModel(&dlg.wireModel);
	}*/
	else if(pItemInfo->itemType==WORKSTATUS_CASE)
	{	//����ʵ��
		if(pTreeCtrl->GetSelectedCount()==1)
		{
			CWorkStatusDlg dlg;
			CWorkStatus *pStatus=(CWorkStatus*)pItemInfo->dwRefData;
			if(pStatus)
			{
				dlg.m_pWorkStatus=pStatus;
				dlg.m_sStatusName=pStatus->description;
				dlg.m_sModuleScope=pStatus->limit_str;
				dlg.m_nWindAzimuth=pStatus->WindLoad.azimuth;
				dlg.m_fWindVelocity=pStatus->WindLoad.velocity;
				dlg.m_fIceThick=pStatus->ice_thick;
				if(dlg.DoModal()==IDOK)
				{
					CString ss;
					if(dlg.m_sStatusName.IsEmpty())
						ss.Format("%d(%d)",pStatus->iNo,pStatus->iNo);
					else
						ss.Format("%s(%d)",dlg.m_sStatusName,pStatus->iNo);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetChildItem(hItem);
					ss.Format("�ɱ�������ϵ��=%.2f",pStatus->Kesai());
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("���ú����鷶Χ=%s",pStatus->limit_str);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);//�����������
					ss.Format("�����������=%.2fmm",pStatus->ice_thick);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);//���к���
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);//�����
					HTREEITEM hLeafItem=pTreeCtrl->GetNextItem(hItem,TVGN_CHILD);//����
					ss.Format("����=%f",pStatus->WindLoad.velocity);
					SimplifiedNumString(ss);
					ss+="m/s";
					pTreeCtrl->SetItemText(hLeafItem,ss);
					hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);//�����
					ss.Format("�����=%d��",pStatus->WindLoad.azimuth);
					pTreeCtrl->SetItemText(hLeafItem,ss);
				}
				return;
			}
		}
		else
		{
			CSelectWorkCaseDlg dlg;
			CWorkStatus *pStatus=NULL;
			CHashList<HTREEITEM> hashSelItems;	//��ֵΪ�������
			HTREEITEM hSelectItem=pTreeCtrl->GetFirstSelectedItem();
			do 
			{
				pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSelectItem);
				if(pItemInfo==NULL||pItemInfo->itemType!=WORKSTATUS_CASE)
					return;
				if((pStatus=(CWorkStatus*)pItemInfo->dwRefData)==NULL)
					return;
				hashSelItems.SetValue(pStatus->handle,hSelectItem);
				dlg.m_selectList.append(pStatus);
				hSelectItem=pTreeCtrl->GetNextSelectedItem(hSelectItem);
			} while(hSelectItem);
			if(dlg.m_selectList.GetNodeNum()<=0 || dlg.DoModal()!=IDOK)
				return;
			for(CWorkStatus* pSelStatus=dlg.m_selectList.GetFirst();pSelStatus;pSelStatus=dlg.m_selectList.GetNext())
			{
				HTREEITEM* phSelectItem=hashSelItems.GetValue(pSelStatus->handle);
				CString ss;
				if(strlen(pSelStatus->description)==0)
					ss.Format("%d(%d)",pSelStatus->iNo,pSelStatus->iNo);
				else
					ss.Format("%s(%d)",pSelStatus->description,pSelStatus->iNo);
				pTreeCtrl->SetItemText(*phSelectItem,ss);
				hItem=pTreeCtrl->GetChildItem(*phSelectItem);
				ss.Format("�ɱ�������ϵ��=%.2lf",pSelStatus->Kesai());
				pTreeCtrl->SetItemText(hItem,ss);
				hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
				ss.Format("���ú����鷶Χ=%s",pSelStatus->limit_str);
				pTreeCtrl->SetItemText(hItem,ss);
				hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);//�����������
				ss.Format("�����������=%.2lfmm",pSelStatus->ice_thick);
				pTreeCtrl->SetItemText(hItem,ss);
				hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);//���к���
				hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);//�����
				HTREEITEM hLeafItem=pTreeCtrl->GetNextItem(hItem,TVGN_CHILD);//����
				ss.Format("����=%lf",pSelStatus->WindLoad.velocity);
				SimplifiedNumString(ss);
				ss+="m/s";
				pTreeCtrl->SetItemText(hLeafItem,ss);
				hLeafItem=pTreeCtrl->GetNextItem(hLeafItem,TVGN_NEXT);//�����
				ss.Format("�����=%d��",pSelStatus->WindLoad.azimuth);
				pTreeCtrl->SetItemText(hLeafItem,ss);
			}
			return;
		}
	}
	else if(pItemInfo->itemType==WORKSTATUS_POINTLOAD)
	{	//�����غ�
	}
	else if(pItemInfo->itemType==WORKSTATUS_WINDLOAD)
	{	//�����
	}
	else if(pItemInfo->itemType==WORKSTATUS_EARTHQUAKE)
	{	//�������
	}
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOADITEM||pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOADREFITEM)
	{
		CAttachWindLoad *pWindLoad=(CAttachWindLoad*)pItemInfo->dwRefData;
		if(pWindLoad)
		{
			CAttachWindLoadDlg wind_dlg;
			CWorkStatus *pRelateWorkStatus=NULL;
			if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOADREFITEM)
			{
				wind_dlg.m_bWorkStatusMode=TRUE;
				HTREEITEM hParent=pTreeCtrl->GetParentItem(hItem);	//���˸��Ӻ��ؼ���һ��
				hParent=pTreeCtrl->GetParentItem(hParent);	//���˹���һ��
				TREEITEM_INFO* pStatusInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParent);
				if(pStatusInfo&&pStatusInfo->itemType==WORKSTATUS_CASE&&pStatusInfo->dwRefData)
				{
					pRelateWorkStatus=(CWorkStatus*)pStatusInfo->dwRefData;
					wind_dlg.m_fShieldCoef=pRelateWorkStatus->AttachWindLoad[pWindLoad->handle].shieldCoef;
				}
			}
			else
				wind_dlg.m_bWorkStatusMode=FALSE;
			wind_dlg.m_sAttachWindLoadName=pWindLoad->name;
			wind_dlg.m_fShieldArea=pWindLoad->m_fShieldArea;
			wind_dlg.m_fProfileCoef=pWindLoad->m_fProfileCoef;
			wind_dlg.m_fAttachWeight=pWindLoad->m_fAttachWeight;
			wind_dlg.m_fBetaZ=pWindLoad->m_fBetaZ;
			wind_dlg.m_fBetaZForFoundation=pWindLoad->m_fBetaZForFoundation;
			for(long *pH=pWindLoad->GetNodeList()->GetFirst();pH;pH=pWindLoad->GetNodeList()->GetNext())
			{
				CLDSNode *pNode=console.FromNodeHandle(*pH);
				if(pNode)
				{
					NODE_POINTI item;
					item.h=pNode->handle;
					item.point_i=pNode->point_i;
					wind_dlg.node_arr.Add(item);
				}
			}
			if(wind_dlg.DoModal()==IDOK)
			{
				if(wind_dlg.m_bWorkStatusMode&&pRelateWorkStatus)
					pRelateWorkStatus->AttachWindLoad[pWindLoad->handle].shieldCoef=wind_dlg.m_fShieldCoef;
				else
				{
					sprintf(pWindLoad->name,"%s",wind_dlg.m_sAttachWindLoadName);
					pWindLoad->m_fShieldArea=wind_dlg.m_fShieldArea;
					pWindLoad->m_fProfileCoef=wind_dlg.m_fProfileCoef;
					pWindLoad->m_fAttachWeight=wind_dlg.m_fAttachWeight;
					pWindLoad->m_fBetaZ=wind_dlg.m_fBetaZ;
					pWindLoad->m_fBetaZForFoundation=wind_dlg.m_fBetaZForFoundation;
					pWindLoad->GetNodeList()->Empty();
					for(long i=0;i<wind_dlg.node_arr.GetSize();i++)
						pWindLoad->GetNodeList()->append(wind_dlg.node_arr[i].h);
					CString sItem;
					sItem.Format("%s(%d)",pWindLoad->name,pWindLoad->iNo);
					pTreeCtrl->SetItemText(hItem,sItem);
				}
			}
		}
	}
	else if(pItemInfo->itemType==WINDSEG_CASE)
	{
		if(pTreeCtrl->GetSelectedCount()==1)
		{	//ѡ��һ����ѹ�ε������
			pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
			CWindSegment *pWindSeg=(CWindSegment*)pItemInfo->dwRefData;
			if(pWindSeg)
			{
				pWindSeg->InitNodePtrArr(console.GetActiveModel());
				CWindLoadSegDlg dlg;
				dlg.m_pWindSeg=pWindSeg;
				if(dlg.DoModal()==IDOK)
				{
					hItem=pTreeCtrl->GetChildItem(hItem);
					CString ss;
					if(pWindSeg->cMirType==1)
						ss="Y��Գ�";
					else if(pWindSeg->cMirType==2)
						ss="X��Գ�";
					else
						ss="�޶Գ�";
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					if(dlg.m_iSegType==0)
						ss="��ѹ������=0.�˵������������";
					else if(dlg.m_iSegType==1)
						ss="��ѹ������=1.�Զ����ڵ�����";
					else //if(dlg.m_iSegType==2)
						ss="��ѹ������=2.����������������";
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					if(dlg.m_iCalStyle==0)
						ss="���㷽ʽ=����ʽ";
					else if(dlg.m_iCalStyle==1)
						ss="���㷽ʽ=X����ᵣ����";
					else// if(dlg.m_iCalStyle==2)
						ss="���㷽ʽ=Y����ᵣ����";
					pTreeCtrl->SetItemText(hItem,ss);
					int n=pWindSeg->StatValidNodeCount(console.GetActiveModel());
					for(int i=0;i<n;i++)
					{
						hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
						char sText[100]="";
						sprintf(sText,"�ڵ�%d:0X%X(%d)",i+1,pWindSeg->nodeArr[i],pWindSeg->nodePtrArr[i]->point_i);
						pTreeCtrl->SetItemText(hItem,sText);
					}
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("����ϵ��(����)=%.2f",pWindSeg->BetaZ);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("����ϵ��(����)=%.2f",pWindSeg->BetaZForFoundation);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("����ЧӦϵ��=%.2f",pWindSeg->CqiFront);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("����ЧӦϵ��=%.2f",pWindSeg->CqiSide);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("���������״ϵ��=%.2f",pWindSeg->K);
					pTreeCtrl->SetItemText(hItem,ss);
				}
			}
		}
		else
		{	//�����޸�ʱ
			CSelectWindLoadDlg dlg;
			CWindSegment* pWindSeg;
			CHashList<HTREEITEM> hashSelItems;	//	��ֵΪpWindSeg->iNo
			HTREEITEM hSelectItem=pTreeCtrl->GetFirstSelectedItem();
			do{
				pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSelectItem);
				if(pItemInfo==NULL||pItemInfo->itemType!=WINDSEG_CASE)
					return;
				if((pWindSeg=(CWindSegment*)pItemInfo->dwRefData)==NULL)
					return;
				hashSelItems.SetValue(pWindSeg->iNo,hSelectItem);
				pWindSeg->InitNodePtrArr(console.GetActiveModel());
				dlg.m_selectList.append(pWindSeg);
				hSelectItem=pTreeCtrl->GetNextSelectedItem(hSelectItem);
			} while(hSelectItem);
			if(dlg.m_selectList.GetNodeNum()<=0 || dlg.DoModal()==IDOK)
			{
				for(CWindSegment* pSelWindSeg=dlg.m_selectList.GetFirst();pSelWindSeg;pSelWindSeg=dlg.m_selectList.GetNext())
				{
					HTREEITEM* phSelectItem=hashSelItems.GetValue(pSelWindSeg->iNo);
					hItem=pTreeCtrl->GetChildItem(*phSelectItem);
					CString ss;
					if(pSelWindSeg->cMirType==1)
						ss="Y��Գ�";
					else if(pSelWindSeg->cMirType==2)
						ss="X��Գ�";
					else
						ss="�޶Գ�";
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					if(pSelWindSeg->cType==0)
						ss="��ѹ������=0.�˵������������";
					else if(pSelWindSeg->cType==1)
						ss="��ѹ������=1.�Զ����ڵ�����";
					else //if(pSelWindSeg->cType==2)
						ss="��ѹ������=2.����������������";
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					if(pSelWindSeg->cCalStyle==0)
						ss="���㷽ʽ=����ʽ";
					else if(pSelWindSeg->cCalStyle==1)
						ss="���㷽ʽ=X����ᵣ����";
					else// if(pSelWindSeg->cCalStyle==2)
						ss="���㷽ʽ=Y����ᵣ����";
					pTreeCtrl->SetItemText(hItem,ss);
					int n=pSelWindSeg->StatValidNodeCount(console.GetActiveModel());
					for(int i=0;i<n;i++)
					{
						hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
						char sText[100]="";
						sprintf(sText,"�ڵ�%d:0X%X(%d)",i+1,pSelWindSeg->nodeArr[i],pSelWindSeg->nodePtrArr[i]->point_i);
						pTreeCtrl->SetItemText(hItem,sText);
					}
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("����ϵ��(����)=%.2f",pSelWindSeg->BetaZ);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("����ϵ��(����)=%.2f",pSelWindSeg->BetaZForFoundation);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("����ЧӦϵ��=%.2f",pSelWindSeg->CqiFront);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("����ЧӦϵ��=%.2f",pSelWindSeg->CqiSide);
					pTreeCtrl->SetItemText(hItem,ss);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
					ss.Format("���������״ϵ��=%.2f",pSelWindSeg->K);
					pTreeCtrl->SetItemText(hItem,ss);
				}
			}
		}
		return;
	}
	else if(pItemInfo->itemType==MATERIAL_GROUP)
	{	//������
	}
	else if(pItemInfo->itemType==MATERIAL_RECORD)
	{	//���ϼ�¼
	}
	else if(pItemInfo->itemType==TYPE_ANGLE_IRON)
	{	//��֫�Ǹ�
	}
	else if(pItemInfo->itemType==TYPE_COMPOSITE_ANGLE_IRON)
	{	//��ϽǸ�
	}
	else if(pItemInfo->itemType==TYPE_STEEL_TUBE)
	{	//�ֹ�
	}
	else if(pItemInfo->itemType==TYPE_SLOT_IRON)
	{	//�۸�
	}
	else if(pItemInfo->itemType==TYPE_FLAT_IRON)
	{	//����
	}
	else if(pItemInfo->itemType==TYPE_STEEL_GUY)
	{	//����
	}
#endif
	else if(pItemInfo->itemType==BLOCK_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
	{	//��������
		CBlockModel* pBlock=(CBlockModel*)pItemInfo->dwRefData;
		if(pBlock)
		{
			CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
			pLdsView->m_pSelectBlock=pBlock;
			pLdsView->DisplayBlockModelProperty(TRUE);
		}
	}
	else if(pItemInfo->itemType==BLOCKREF_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
	{
		//������������
		CBlockReference* pBlockRef = (CBlockReference*)pItemInfo->dwRefData;
		if(pBlockRef)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			CLDSView* pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
			pLdsView->UpdatePropertyPage(pBlockRef);
		}
	}
	else
		return;
#endif
}
//</DEVELOP_PROCESS_MARK>
void CTowerTreeDlg::OnLayerviewSegPropItem() 
{
#if defined(__SMART_DRAW_INC_)&&defined(__LDS_CONTEXT_)// defined(__LDS_)
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL,*pSubItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	
	if(pItemInfo->itemType==VIEW_CASE)
	{	//��ͼ����
		for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView==(void*)pItemInfo->dwRefData)
				break;
		}
		if(pView&&pView->m_iViewType==0)
		{
			CDefinedViewDlg definedViewDlg;
			definedViewDlg.m_sViewName = pView->name;
			definedViewDlg.ucs=pView->ucs;
			definedViewDlg.m_sFolderName = "ͼ����ͼ";
			definedViewDlg.m_sDefaultLayer = pView->defaultLayer;
			definedViewDlg.m_pDefinedView = pView;
			definedViewDlg.m_bLayerSegView=TRUE;
			definedViewDlg.DoModal();	//��ģʽ���������ø�����ͼԪ��
		}
	}
	else if(pItemInfo->itemType==VIEW_DEFINEDFOLDER)
	{	//������
		for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView==(void*)pItemInfo->dwRefData)
				break;
		}
		if(pView&&pView->m_iViewType==2)
		{
			CString ss;
			CFolderNameDlg folderNameDlg;
			folderNameDlg.m_sFolderName = pView->folderName;
			folderNameDlg.m_pCurView=pView;
			folderNameDlg.m_nNameMaxLen = 16;
			folderNameDlg.m_nDlgType = 1;	//������ͼ������
			if(folderNameDlg.DoModal()==IDOK)
			{
				ss.Format("%s",folderNameDlg.m_sFolderName);
				pTreeCtrl->SetItemText(hItem,ss);
				CString sFolderName=pView->folderName;
				for(CDisplayView *pTempView=Ta.GetFirstDisplayView();pTempView;pTempView=Ta.GetNextDisplayView())
				{	//�����޸ĺ�ͬʱ�޸�(���ڸ����)��ͼ������
					if(sFolderName.CompareNoCase(pTempView->folderName)==0)
						_snprintf(pTempView->folderName,16,"%s",folderNameDlg.m_sFolderName);
				}
			}
		}
	}
#endif
}

void CTowerTreeDlg::ActiveModel(IModel *pModel)
{
	CWaitCursor wait;
	CBlockModel *pPrevBlock=NULL;
	CTowerTreeCtrl* pTreeCtrl = GetTreeCtrl();
	if(!console.GetActiveModel()->IsTowerModel())
		pPrevBlock=(CBlockModel*)console.GetActiveModel();
	
	long hView=0;
	IModel *pActiveModel=pModel;
	if(pModel->IsTowerModel()||((CBlockModel*)pModel)->IsEmbeded())
		pActiveModel=&Ta;
	if(m_hActiveViewItem)
	{	//ͨ����ǰ������ͼ���ȡ������ͼ���,������ͼ�����ڼ���ģ��ʱ����Ч
		TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(m_hActiveViewItem);
		CDisplayView* pView=(CDisplayView*)pItemInfo->dwRefData;
		if(pView&&pView->BelongModel()==pActiveModel&&pView->handle>0)
			hView=pView->handle;
	}
	if(console.GetActiveModel()==pModel)
		console.SetActiveView(hView);	//����ģ��δ�����仯ʱ����Ҫ����ģ���л���ֻ��Ҫ�л���ͼ
	else 
	{
		if(pModel->IsTowerModel()||((CBlockModel*)pModel)->IsEmbeded())
		{	//������
			if(pPrevBlock!=NULL&&!pPrevBlock->IsEmbeded())
			{	//��״̬��������Ϊ�ⲿ����ʽ������
				while(g_pSolidSet->GetEnvTypeId()!=0&&g_pSolidOper->PopEnv());//�ָ�����ͼ�β�������
			}
			g_pSolidDraw->DelCS(ISolidDraw::BLK_LOCATE_CS);
		}
		else //if(!((CBlockModel*)pModel)->IsEmbeded())
		{	//����벿��ģ��
			if(pPrevBlock==NULL || pPrevBlock->IsEmbeded())
			{	//��״̬��������Ϊ����
				g_pSolidOper->PushEnv();
				g_pSolidSet->SetEnvTypeId(2);
			}
			CBlockModel *pBlock=(CBlockModel*)pModel;
			if (g_sysPara.m_bDispBlockLCS)
				g_pSolidDraw->AddCS(ISolidDraw::BLK_LOCATE_CS,pBlock->GetLCS(),"Xa","Ya","Za");
			else 
				g_pSolidDraw->DelCS(ISolidDraw::BLK_LOCATE_CS);
		}
		console.ActivateModel(pActiveModel,hView);	//����ģ��
	}
	//ˢ�¹�����ʾ��
	console.Display(NULL);
	//����Ƕ�벿������Ϊѡ��״̬
	if(!pModel->IsTowerModel()&&((CBlockModel*)pModel)->IsEmbeded())
	{	//��ǰ����ģ��Ϊ��Ƕʽ�����飬��ʱӦ�ڹ�����ѡ�в�������ȫ������
		g_pSolidDraw->ReleaseSnapStatus();
		ARRAY_LIST<long> id_arr;
		for(CLDSPart *pPart=Ta.EnumPartFirst();pPart;pPart=Ta.EnumPartNext())
		{
			if(pPart->m_hBlock==((CBlockModel*)pModel)->handle)
				id_arr.append(pPart->handle);
		}
		for(CLDSNode *pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext())
		{
			if(pNode->m_hBlock==((CBlockModel*)pModel)->handle)
				id_arr.append(pNode->handle);
		}
		g_pSolidDraw->SetEntSnapStatus(id_arr.m_pData,id_arr.GetSize(),TRUE);
	}
}
#include "ActiveHeightLegsDlg.h"
void CTowerTreeDlg::OnSetActiveItem() 
{
	CTowerTreeCtrl* pTreeCtrl = GetTreeCtrl();
	HTREEITEM hCurItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL;
	if(hCurItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hCurItem);
	if(pItemInfo==NULL)
		return;
	CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	if(PRODUCT_FUNC::IsHasBlockSetup())
		pLdsView->m_pSelectBlock=NULL;	//��ʼ����ǰ����
	if(pItemInfo->itemType>=MODEL_QUAD1LEG_CFGNO&&pItemInfo->itemType<=MODEL_QUAD4LEG_CFGNO)
	{
		CActiveHeightLegsDlg dlg;
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hCurItem);
		TREEITEM_INFO *pItem=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pItem&&pModule==(CLDSModule*)pItem->dwRefData)
				break;
		}
		if(pModule==NULL)
			return;
		int nMaxLegs=CFGLEG::MaxLegOfBody(pModule->idBodyNo);
		dlg.m_sHeightName.Format("���ߣ�%s",pModule->description);
		//dlg.m_siInitLegHeight=pModule->m_siInitLegHeight;
		dlg.m_siLegLevelDiff=pModule->uiLevelHeight;
		dlg.m_iOrderOfLegQuadD1=(pModule->m_arrActiveQuadLegNo[0]-1)%nMaxLegs;
		dlg.m_iOrderOfLegQuadA2=(pModule->m_arrActiveQuadLegNo[1]-1)%nMaxLegs;
		dlg.m_iOrderOfLegQuadC3=(pModule->m_arrActiveQuadLegNo[2]-1)%nMaxLegs;
		dlg.m_iOrderOfLegQuadB4=(pModule->m_arrActiveQuadLegNo[3]-1)%nMaxLegs;
		dlg.m_dwIncLegItemFlag=pModule->m_dwLegCfgWord.GetBodyLegs(pModule->idBodyNo);
		if(dlg.DoModal()==IDOK)
		{
			pModule->m_arrActiveQuadLegNo[0]=(dlg.m_iOrderOfLegQuadD1+1)+(pModule->idBodyNo-1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[1]=(dlg.m_iOrderOfLegQuadA2+1)+(pModule->idBodyNo-1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[2]=(dlg.m_iOrderOfLegQuadC3+1)+(pModule->idBodyNo-1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[3]=(dlg.m_iOrderOfLegQuadB4+1)+(pModule->idBodyNo-1)*nMaxLegs;
			HTREEITEM hSubItem=pTreeCtrl->GetNextItem(hParentItem,TVGN_CHILD);
			while(hSubItem)
			{
				TREEITEM_INFO *pSubItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSubItem);
				int ciQuadSerial=0;
				if(pSubItemInfo->itemType>=MODEL_QUAD1LEG_CFGNO&&pSubItemInfo->itemType<=MODEL_QUAD4LEG_CFGNO)
					ciQuadSerial=pSubItemInfo->itemType-MODEL_QUAD1LEG_CFGNO+1;
				if(ciQuadSerial==0)
				{
					hSubItem=pTreeCtrl->GetNextSiblingItem(hSubItem);
					continue;
				}
			//ˢ�����ڵ��еĸ����޽��ȵĵ�ǰ�������Ⱥ� wjh-2019.4.21
				CXhChar200 ss;
				for(int j=1;j<=192;j++)
				{
					if(pModule->m_dwLegCfgWord.IsHasNo(j))
					{
						if(j==pModule->m_arrActiveQuadLegNo[ciQuadSerial-1])
							ss.Append(CXhChar16("%C(*)",(j-1)%nMaxLegs+'A'),',');
						else
							ss.Append(CXhChar16("%C",(j-1)%nMaxLegs+'A'),',');
					}
				}
#ifdef AFX_TARG_ENU_ENGLISH
				ss.InsertBefore(CXhChar50("Joint Leg(%d)Model Flag:",ciQuadSerial));
#else 
				ss.InsertBefore(CXhChar50("����(%d)��ĺ�:",ciQuadSerial));
#endif
				pTreeCtrl->SetItemText(hSubItem,ss);
				hSubItem=pTreeCtrl->GetNextSiblingItem(hSubItem);
			}
#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
			CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
			pView->RedrawAll();
			theApp.GetLDSDoc()->SetModifiedFlag();
#elif defined(__LDS_GROUP_CONTEXT_)// defined(__LDSGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)
			CTmaGroupView *pView=(CTmaGroupView*)theApp.GetTmaGroupDoc()->GetView(RUNTIME_CLASS(CTmaGroupView));
			pView->RedrawAll();
			theApp.GetTmaGroupDoc()->SetModifiedFlag();
#endif
			Ta.m_hActiveModule=pModule->handle;
			RefreshActiveItemStateAndActiveModel(&Ta,hParentItem);
		}
	}
	/*if(pItemInfo->itemType==MODEL_QUAD1LEG_CFGNO)
	{
		CSetActiveItemDlg dlg;
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hCurItem);
		TREEITEM_INFO *pItem=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pItem&&pModule==(CLDSModule*)pItem->dwRefData)
				break;
		}
		if(pModule==NULL)
			return;
		dlg.m_sActiveItem.Format("%C",(pModule->m_arrActiveQuadLegNo[0]-1)%nMaxLegs+'A');
		CString ss;
		for(int i=1;i<=192;i++)
		{
			if(pModule->m_dwLegCfgWord.IsHasNo(i))
			{
				ss.Format("%C",(i-1)%nMaxLegs+'A');
				dlg.m_arrStrList.Add(ss);
			}
		}
		if(dlg.DoModal()==IDOK)
		{
			pModule->m_arrActiveQuadLegNo[0]=dlg.m_sActiveItem[0]-'@'+(pModule->GetBodyNo()-1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[1]=pModule->m_arrActiveQuadLegNo[0];
			pModule->m_arrActiveQuadLegNo[2]=pModule->m_arrActiveQuadLegNo[0];
			pModule->m_arrActiveQuadLegNo[3]=pModule->m_arrActiveQuadLegNo[0];
			if(Ta.m_hActiveModule==pModule->handle)
			{
				Ta.m_arrActiveQuadLegNo[0]=pModule->m_arrActiveQuadLegNo[0];
				Ta.m_arrActiveQuadLegNo[1]=pModule->m_arrActiveQuadLegNo[1];
				Ta.m_arrActiveQuadLegNo[2]=pModule->m_arrActiveQuadLegNo[2];
				Ta.m_arrActiveQuadLegNo[3]=pModule->m_arrActiveQuadLegNo[3];
			}
			for(i=0;i<4;i++)
			{
				CString sTemp;
#ifdef AFX_TARG_ENU_ENGLISH
				ss="Joint Leg(1)Model Flag:";
#else 
				ss="����(1)��ĺ�:";
#endif
				for(int j=1;j<=192;j++)
				{
					if(pModule->m_dwLegCfgWord.IsHasNo(j))
					{
						if(j==pModule->m_arrActiveQuadLegNo[0])
							sTemp.Format("%C(*),",(j-1)%nMaxLegs+'A');
						else
							sTemp.Format("%C,",(j-1)%nMaxLegs+'A');
						ss+=sTemp;
					}
				}
				ss=ss.Left(ss.GetLength()-1);
				pTreeCtrl->SetItemText(hCurItem,ss);
				HTREEITEM hTempItem = hCurItem;
				while(hTempItem = pTreeCtrl->GetNextSiblingItem(hTempItem))
				{
					TREEITEM_INFO *pTempInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hTempItem);
					if(pTempInfo==NULL)
						continue;
					if(pTempInfo->itemType==MODEL_QUAD2LEG_CFGNO)
						ss.SetAt(5,'2');
					else if(pTempInfo->itemType==MODEL_QUAD3LEG_CFGNO)
						ss.SetAt(5,'3');
					else if(pTempInfo->itemType==MODEL_QUAD4LEG_CFGNO)
						ss.SetAt(5,'4');
					else
						continue;
					pTreeCtrl->SetItemText(hTempItem,ss);
				}
			}
			#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
				CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
				pView->RedrawAll();
				theApp.GetLDSDoc()->SetModifiedFlag();
			#elif defined(__LDS_GROUP_CONTEXT_)// defined(__LDSGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)
				CTmaGroupView *pView=(CTmaGroupView*)theApp.GetTmaGroupDoc()->GetView(RUNTIME_CLASS(CTmaGroupView));
				pView->RedrawAll();
				theApp.GetTmaGroupDoc()->SetModifiedFlag();
			#endif
			TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
			if(pItemInfo&&pItemInfo->itemType==MODEL_CASE)
			{
				CLDSModule* pModule=(CLDSModule*)pItemInfo->dwRefData;
				Ta.m_hActiveModule=pModule->handle;
				RefreshActiveItemStateAndActiveModel(&Ta,hParentItem);
			}
		}
	}
	else if(pItemInfo->itemType==MODEL_QUAD2LEG_CFGNO)
	{
		CSetActiveItemDlg dlg;
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hCurItem);
		TREEITEM_INFO *pItem=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pItem&&pModule==(CLDSModule*)pItem->dwRefData)
				break;
		}
		if(pModule==NULL)
			return;
		dlg.m_sActiveItem.Format("%C",(pModule->m_arrActiveQuadLegNo[1]-1)%nMaxLegs+'A');
		CString ss;
		for(int i=1;i<=192;i++)
		{
			if(pModule->m_dwLegCfgWord.IsHasNo(i))
			{
				ss.Format("%C",(i-1)%nMaxLegs+'A');
				dlg.m_arrStrList.Add(ss);
			}
		}
		if(dlg.DoModal()==IDOK)
		{
			pModule->m_arrActiveQuadLegNo[1]=dlg.m_sActiveItem[0]-'@'+(pModule->GetBodyNo()-1)*nMaxLegs;
			if(Ta.m_hActiveModule==pModule->handle)	//��ģ�ͼ�Ϊ��ǰ�ģ��
				Ta.m_arrActiveQuadLegNo[1]=pModule->m_arrActiveQuadLegNo[1];
			CString sTemp;
#ifdef AFX_TARG_ENU_ENGLISH
			ss="Joint Leg(2)Model Flag:";
#else 
			ss="����(2)��ĺ�:";
#endif
			for(i=1;i<=192;i++)
			{
				if(pModule->m_dwLegCfgWord.IsHasNo(i))
				{
					if(i==pModule->m_arrActiveQuadLegNo[1])
						sTemp.Format("%C(*),",(i-1)%nMaxLegs+'A');
					else
						sTemp.Format("%C,",(i-1)%nMaxLegs+'A');
					ss+=sTemp;
				}
			}
			ss=ss.Left(ss.GetLength()-1);
			pTreeCtrl->SetItemText(hCurItem,ss);
			#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
				CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
				pView->RedrawAll();
				theApp.GetLDSDoc()->SetModifiedFlag();
			#elif defined(__LDSGROUP_)||defined(__TSAGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)
				CTmaGroupView *pView=(CTmaGroupView*)theApp.GetTmaGroupDoc()->GetView(RUNTIME_CLASS(CTmaGroupView));
				pView->RedrawAll();
				theApp.GetTmaGroupDoc()->SetModifiedFlag();
			#endif
			TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
			if(pItemInfo&&pItemInfo->itemType==MODEL_CASE)
			{
				CLDSModule* pModule=(CLDSModule*)pItemInfo->dwRefData;
				Ta.m_hActiveModule=pModule->handle;
				RefreshActiveItemStateAndActiveModel(&Ta,hParentItem);
			}
		}
	}
	else if(pItemInfo->itemType==MODEL_QUAD3LEG_CFGNO)
	{
		CSetActiveItemDlg dlg;
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hCurItem);
		TREEITEM_INFO *pItem=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pItem&&pModule==(CLDSModule*)pItem->dwRefData)
				break;
		}
		if(pModule==NULL)
			return;
		dlg.m_sActiveItem.Format("%C",(pModule->m_arrActiveQuadLegNo[2]-1)%nMaxLegs+'A');
		CString ss;
		for(int i=1;i<=192;i++)
		{
			if(pModule->m_dwLegCfgWord.IsHasNo(i))
			{
				ss.Format("%C",(i-1)%nMaxLegs+'A');
				dlg.m_arrStrList.Add(ss);
			}
		}
		if(dlg.DoModal()==IDOK)
		{
			pModule->m_arrActiveQuadLegNo[2]=dlg.m_sActiveItem[0]-'@'+(pModule->GetBodyNo()-1)*nMaxLegs;
			if(Ta.m_hActiveModule==pModule->handle)	//��ģ�ͼ�Ϊ��ǰ�ģ��
				Ta.m_arrActiveQuadLegNo[2]=pModule->m_arrActiveQuadLegNo[2];
			CString sTemp;
#ifdef AFX_TARG_ENU_ENGLISH
			ss="Joint Leg(3)Model Flag:";
#else 
			ss="����(3)��ĺ�:";
#endif
			for(i=1;i<=192;i++)
			{
				if(pModule->m_dwLegCfgWord.IsHasNo(i))
				{
					if(i==pModule->m_arrActiveQuadLegNo[2])
						sTemp.Format("%C(*),",(i-1)%nMaxLegs+'A');
					else
						sTemp.Format("%C,",(i-1)%nMaxLegs+'A');
					ss+=sTemp;
				}
			}
			ss=ss.Left(ss.GetLength()-1);
			pTreeCtrl->SetItemText(hCurItem,ss);
		#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
			CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
			pView->RedrawAll();
			theApp.GetLDSDoc()->SetModifiedFlag();
		#elif defined(__LDS_GROUP_CONTEXT_)// defined(__LDSGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)
			CTmaGroupView *pView=(CTmaGroupView*)theApp.GetTmaGroupDoc()->GetView(RUNTIME_CLASS(CTmaGroupView));
			pView->RedrawAll();
			theApp.GetTmaGroupDoc()->SetModifiedFlag();
		#endif
			TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
			if(pItemInfo&&pItemInfo->itemType==MODEL_CASE)
			{
				CLDSModule* pModule=(CLDSModule*)pItemInfo->dwRefData;
				Ta.m_hActiveModule=pModule->handle;
				RefreshActiveItemStateAndActiveModel(&Ta,hParentItem);
			}
		}
	}
	else if(pItemInfo->itemType==MODEL_QUAD4LEG_CFGNO)
	{
		CSetActiveItemDlg dlg;
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hCurItem);
		TREEITEM_INFO *pItem=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pItem&&pModule==(CLDSModule*)pItem->dwRefData)
				break;
		}
		if(pModule==NULL)
			return;
		dlg.m_sActiveItem.Format("%C",(pModule->m_arrActiveQuadLegNo[3]-1)%nMaxLegs+'A');
		CString ss;
		for(int i=1;i<=192;i++)
		{
			if(pModule->m_dwLegCfgWord.IsHasNo(i))
			{
				ss.Format("%C",(i-1)%nMaxLegs+'A');
				dlg.m_arrStrList.Add(ss);
			}
		}
		if(dlg.DoModal()==IDOK)
		{
			pModule->m_arrActiveQuadLegNo[3]=dlg.m_sActiveItem[0]-'@'+(pModule->GetBodyNo()-1)*nMaxLegs;
			if(Ta.m_hActiveModule==pModule->handle)	//��ģ�ͼ�Ϊ��ǰ�ģ��
				Ta.m_arrActiveQuadLegNo[3]=pModule->m_arrActiveQuadLegNo[3];
			CString sTemp;
#ifdef AFX_TARG_ENU_ENGLISH
			ss="Joint Leg(4)Model Flag:";
#else 
			ss="����(4)��ĺ�:";
#endif
			for(i=1;i<=192;i++)
			{
				if(pModule->m_dwLegCfgWord.IsHasNo(i))
				{
					if(i==pModule->m_arrActiveQuadLegNo[3])
						sTemp.Format("%C(*),",(i-1)%nMaxLegs+'A');
					else
						sTemp.Format("%C,",(i-1)%nMaxLegs+'A');
					ss+=sTemp;
				}
			}
			ss=ss.Left(ss.GetLength()-1);
			pTreeCtrl->SetItemText(hCurItem,ss);
		#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
			CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
			pView->RedrawAll();
			theApp.GetLDSDoc()->SetModifiedFlag();
		#elif defined(__LDS_GROUP_CONTEXT_)// defined(__LDSGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)
			CTmaGroupView *pView=(CTmaGroupView*)theApp.GetTmaGroupDoc()->GetView(RUNTIME_CLASS(CTmaGroupView));
			pView->RedrawAll();
			theApp.GetTmaGroupDoc()->SetModifiedFlag();
		#endif
			TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
			if(pItemInfo&&pItemInfo->itemType==MODEL_CASE)
			{
				CLDSModule* pModule=(CLDSModule*)pItemInfo->dwRefData;
				Ta.m_hActiveModule=pModule->handle;
				RefreshActiveItemStateAndActiveModel(&Ta,hParentItem);
			}
		}
	}*/
	else if(pItemInfo->itemType==MODEL_GROUP)
	{
	}
	else if(pItemInfo->itemType==MODEL_CASE)
	{
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule==(void*)pItemInfo->dwRefData)
				break;
		}
		if(pModule==NULL)
			return;
	#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
		CLDSView *pWorkView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
		pWorkView->PushView();
	#endif
		Ta.m_hActiveModule=pModule->handle;
		RefreshActiveItemStateAndActiveModel(&Ta,hCurItem);
		for(int iLeg=0;iLeg<4;iLeg++)
			Ta.m_arrActiveQuadLegNo[iLeg]=pModule->m_arrActiveQuadLegNo[iLeg];
#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
		CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
#elif defined(__LDS_GROUP_CONTEXT_)// defined(__LDSGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)
		CTmaGroupView *pView=(CTmaGroupView*)theApp.GetTmaGroupDoc()->GetView(RUNTIME_CLASS(CTmaGroupView));
#endif
		//pView->RedrawAll();����϶�����Ҫȫ��ˢ��ʵ�����ݣ���ֱ��ע�͵����д��۲�
	}
	else if(pItemInfo->itemType==BLOCK_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
	{
		CBlockModel *pBlockModel=NULL;
		for(pBlockModel=Ta.Block.GetFirst();pBlockModel;pBlockModel=Ta.Block.GetNext())
		{
			if(pBlockModel==(void*)pItemInfo->dwRefData)
				break;
		}
		if(pBlockModel==NULL)
			return;
		RefreshActiveItemStateAndActiveModel(pBlockModel,hCurItem);
		//ˢ�½��沢�������������ʾ��������
		CLDSView *pView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
		pView->m_pSelectBlock=pBlockModel;
		pView->DisplayBlockModelProperty(TRUE);
	}
	else if(pItemInfo->itemType==BLOCKREF_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
	{	
		//������ģ��
		CWaitCursor waiter;
		RefreshActiveItemStateAndActiveModel(&Ta,hCurItem);
		//˫����������ʱ��ѡ�в�������
		CBlockReference *pBlockRef=(CBlockReference*)pItemInfo->dwRefData;
		if(pBlockRef==NULL)
			return;
		g_pSolidDraw->ReleaseSnapStatus();
		if(pBlockRef->IsExploded())
		{
			ARRAY_LIST<long> id_arr;
			for(CLDSPart *pPart=console.EnumPartFirst();pPart;pPart=console.EnumPartNext())
			{
				if(pPart->IsMemberOfBlockRef(pBlockRef))
					id_arr.append(pPart->handle);
			}
			g_pSolidDraw->SetEntSnapStatus(id_arr.m_pData,id_arr.GetSize(),TRUE);
		}
		else 
		{
			pBlockRef->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBlockRef->GetSolidPartObject());
			g_pSolidDraw->Draw();
			g_pSolidDraw->SetEntSnapStatus(pBlockRef->handle,TRUE);
		}
		//�������������ʾ������������
		pLdsView->UpdatePropertyPage(pBlockRef);
	}
	else if(pItemInfo->itemType==VIEW_CASE)
	{	//������ͼ
		CDisplayView *pPrevView = console.GetActiveView();
	#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
		CLDSView *pWorkView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	#elif defined(__LDS_GROUP_CONTEXT_)// defined(__LDSGROUP_)||defined(__LMAGROUP_)||defined(__TMAGROUP_)
		CTmaGroupView *pWorkView=(CTmaGroupView*)theApp.GetTmaGroupDoc()->GetView(RUNTIME_CLASS(CTmaGroupView));
	#endif
		if(pPrevView&&pPrevView->m_iViewType==1&&pPrevView!=(void*)pItemInfo->dwRefData)
			pWorkView->SaveCursorView();	//������ͼǰ���ǰһ�����ͼΪ������ͼ���Ƚ��Ķ����浽������ͼ
		
		CDisplayView* pView=(CDisplayView*)pItemInfo->dwRefData;
		if(pView==NULL)
			return;
		HTREEITEM hItem=pTreeCtrl->GetRootItem();
		//���÷���ÿ���ڵ�Ļص����� ������������Ӧ�÷�Χ̫С
		pTreeCtrl->SetVisitItemFunc(VisitViewItem);	
		//������
		pTreeCtrl->TraverseItem(hItem);	
		//
		IModel* pModel=pView->BelongModel();
		if(pModel==NULL)
			pModel=&Ta;
	#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
		pWorkView->PushView();
	#endif
		RefreshActiveItemStateAndActiveModel(pModel,hCurItem);
	#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
		strcpy(theApp.env.layer,pView->defaultLayer);	//����ϵͳ��ǰ����ͼ��
#endif
		//������ͼ��־ wht 08-02-21
		UpdateDisplayViewFlag(pView);
	#ifdef __LDS_CONTEXT_//defined(__LDS_)||defined(__TSA_)||defined(__LMA_)||defined(__TMA_)
		pWorkView->UpdatePropertyPage();
	#endif
	}
	else if(pItemInfo->itemType==WORKSTATUS_CASE)
	{
		CWorkStatus *pStatus=(CWorkStatus*)pItemInfo->dwRefData;
		if(pStatus==NULL)
			return;
		HTREEITEM hItem=pTreeCtrl->GetParentItem(hCurItem);
		hItem=pTreeCtrl->GetChildItem(hItem);	//��õ�һ�������ڵ�
		while(hItem)
		{
			pTreeCtrl->SetItemState(hItem,0,TVIS_BOLD);
			hItem=pTreeCtrl->GetNextSiblingItem(hItem);
		}
		pTreeCtrl->SetItemState(hCurItem,TVIS_BOLD,TVIS_BOLD);
		Ta.m_hActiveStatus=pStatus->handle;
		CMainFrame *pMainFrm=(CMainFrame*)AfxGetApp()->m_pMainWnd;
		//if(pMainFrm->m_bDisplayDeformedTower)//Ϊ�˸ı���ʾ���ؼ�ͷ����ҲҪ����һ��
		{
			pLdsView->RedrawAll();
		}
		theApp.GetLDSDoc()->SetModifiedFlag();
	}
}

void CTowerTreeDlg::OnCopyItem()
{
#ifdef __PART_DESIGN_INC_
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==WORKSTATUS_CASE)
	{
		CWorkStatus* pStatus=(CWorkStatus*)pItemInfo->dwRefData;
		CWorkStatus* pCopyStatus=Ta.WorkStatus.append();
		pCopyStatus->iWireLayoutNo=pStatus->iWireLayoutNo;
		CBuffer buf;
		pStatus->ToBuffer(buf);
		buf.SeekToBegin();
		pCopyStatus->FromBuffer(buf);
		CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
		pCopyStatus->iNo=pNoGroup->EnumIdleNo();
		pNoGroup->SetNoState(pCopyStatus->iNo);
		if(strlen(pCopyStatus->description)<96)
			strcat(pCopyStatus->description,"����");
		//�����ڵ�������¸��ƹ����ڵ�
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hItem);
		InsertWorkStatusItem(pCopyStatus,hParentItem,hItem);
	}
#endif
}

//����Ԥ����
long CTowerTreeDlg::InsertArcLiftItem(HTREEITEM hArcLiftGropItem,f3dPoint* pRodOrg/* =NULL */,f3dPoint* pDatumLiftPos/* =NULL */)
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	if(hArcLiftGropItem==NULL)
	{	//Ԥ������Ϊ��ʱ,���������ͼ����Ԥ������
		HTREEITEM hTowerItem=pTreeCtrl->GetChildItem(pTreeCtrl->GetRootItem());
		hArcLiftGropItem=pTreeCtrl->GetChildItem(hTowerItem);
		while(hArcLiftGropItem)
		{
			TREEITEM_INFO *pItem=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hArcLiftGropItem);
			if(pItem&&pItem->itemType==MODEL_ARCLIFT_GROUP)
				break;
			hArcLiftGropItem=pTreeCtrl->GetNextItem(hArcLiftGropItem,TVGN_NEXT);
		}
	}
	if(hArcLiftGropItem==NULL)
		return 0;
	//����Ԥ����
	CArcLiftDlg arcLiftDlg;
	if(pRodOrg)	
		arcLiftDlg.m_xArcLift.m_xRotOrg=*pRodOrg;
	if(pDatumLiftPos)
		arcLiftDlg.m_xArcLift.m_xDatumLiftPos=*pDatumLiftPos;
	CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(MODEL_ARCLIFT_GROUP);
	long iArcLiftNo=pNoGroup->EnumIdleNo();
	arcLiftDlg.m_xArcLift.iNo=iArcLiftNo;
	sprintf(arcLiftDlg.m_xArcLift.name,"%d",iArcLiftNo);
	if(theApp.m_bCooperativeWork)
		arcLiftDlg.m_xArcLift.dwPermission=GetSingleWord(theApp.m_nUserId);
	if(arcLiftDlg.DoModal()==IDOK)
	{
		CLDSArcLift *pLift=Ta.ArcLift.append();
		arcLiftDlg.m_xArcLift.handle=pLift->handle;
		DWORD dwPermission=pLift->dwPermission;
		*pLift=arcLiftDlg.m_xArcLift;
		pLift->dwPermission=dwPermission;
		pLift->iNo=iArcLiftNo;
		pNoGroup->SetNoState(pLift->iNo);
		//�����ɵ�Ԥ����嵽Ԥ��������
		CString ss;
		ss.Format("%s(%d)",pLift->name,pLift->iNo);
		HTREEITEM hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_ARC_LIFT_CASE,PRJ_IMG_ARC_LIFT_CASE,hArcLiftGropItem);
		TREEITEM_INFO *pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_ARCLIFT_CASE,(DWORD)pLift));
		pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
	}
	return arcLiftDlg.m_xArcLift.handle;
}
//�õ���ͼ�����Ļ�ͼ���
char GetViewPartFlag(CDisplayView* pView,CLDSPart* pPart)
{
	char cFlag=0;
	if(pPart==NULL)
		return 0;
	if(pPart->IsLinePart())
	{	//����˼��뵱ǰ��ͼ����ƽ�淨�ߵļн�С��45ʱ,����ͼ�����Ļ�ͼ�������ΪZ����
		CLDSLinePart* pLinePart=(CLDSLinePart*)pPart;
		f3dPoint line_vec=pLinePart->End()-pLinePart->Start();
		normalize(line_vec);
		if(fabs(line_vec*console.GetActiveView()->ucs.axis_z)>0.707106781)	//cos45��= 0.707106781
			cFlag='Z';
	}
	else if(pPart->GetClassTypeId()==CLS_PLATE)
	{

	}
	return cFlag;
}
//<DEVELOP_PROCESS_MARK nameId="CTowerTreeDlg.OnNewItem">
void CTowerTreeDlg::OnNewItem() 
{
#ifdef _RELATIVE_DLG
	CTowerTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItem,*pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	//if(
	//
	/*if(pItemInfo->itemType==BLOCK_GROUP)
	{
		HTREEITEM hBlockItem = InsertBlockItem(pBlock,hItem);
		pTreeCtrl->Expand(hItem,TVE_EXPAND);
		pTreeCtrl->SelectItem(hBlockItem);
		//�������ʾ��������
		CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
		pLdsView->m_pSelectBlock=pBlock;
		pLdsView->DisplayBlockModelProperty(TRUE);
	}
	else */
	if(pItemInfo->itemType==BLOCK_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
	{	//���ɲ�������
		CBlockModel* pBlock=(CBlockModel*)pItemInfo->dwRefData;
		if(pBlock==NULL)
			return;
#ifdef __LDS_CONTEXT_
		CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
#endif
		//1.����µĲ�������,��ѡ�е�ǰ��������
		CWaitCursor wait;
		CBlockReference *pBlkRef=Ta.BlockRef.append();
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(pBlkRef->descript,"%s Block Assembly",pBlock->name);
#else 
		sprintf(pBlkRef->descript,"%s����",pBlock->name);
#endif
		pBlkRef->m_hBlock=pBlock->handle;
		pBlkRef->UpdateAcs();
		pBlkRef->Create3dSolidModel();
		if(!pBlkRef->IsExploded())
			g_pSolidDraw->NewSolidPart(pBlkRef->GetSolidPartObject());
		else
		{
			pBlkRef->Exploded(FALSE);
#ifdef __LDS_CONTEXT_
			pLdsView->ExplodeBlockRef(pBlkRef);
#endif
		}
		g_pSolidDraw->Draw();
		//ѡ�в�������ת����
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pBlkRef->handle,TRUE);
		//2.�ڲ������ýڵ�������½ڵ�
		HTREEITEM hTowerItem=pTreeCtrl->GetChildItem(m_hDataRootItem);
		HTREEITEM hChild = pTreeCtrl->GetChildItem(hTowerItem);
		while(hChild!=NULL)
		{
			TREEITEM_INFO *pChildItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hChild);
			if(pChildItemInfo && pChildItemInfo->itemType==BLOCKREF_GROUP)
				break;
			hChild = pTreeCtrl->GetNextSiblingItem(hChild);
		}
		CString ss;
		ss.Format("%s(0X%X)",pBlkRef->descript,pBlkRef->handle);
		HTREEITEM hBlockRefItem = NULL;
		if(pBlkRef->IsExploded())
			hBlockRefItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_EXPLODED_BLOCKREF_CASE,PRJ_IMG_EXPLODED_BLOCKREF_CASE,hChild);
		else
			hBlockRefItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,hChild);
		pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(BLOCKREF_CASE,(DWORD)pBlkRef));
		pTreeCtrl->SetItemData(hBlockRefItem,(DWORD)pItemInfo);
		pTreeCtrl->Expand(hChild,TVE_EXPAND);
		pTreeCtrl->SelectItem(hBlockRefItem);
#ifdef __LDS_CONTEXT_
		//3.�������������ʾ������������
		pLdsView->UpdatePropertyPage(pBlkRef);
#endif
	}
	else if(pItemInfo->itemType==MODEL_GROUP)
	{
		CModelPropDlg dlg;
#ifdef AFX_TARG_ENU_ENGLISH
		dlg.m_sModelName="New Model";
#else 
		dlg.m_sModelName="�º���";
#endif
		if(dlg.DoModal()==IDOK)
		{
			CString ss;
			CLDSModule *pModule=Ta.Module.append();
			CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
			pModule->iNo=pNoGroup->EnumIdleNo();
			pNoGroup->SetNoState(pModule->iNo);
			sprintf(pModule->description,"%s",dlg.m_sModelName);
			pModule->SetBodyNo(dlg.m_iBodyLegNo);
			pModule->m_dwLegCfgWord=dlg.m_dwLegCfgNo;
			pModule->m_fBurglarLsStartZ = dlg.m_fBurglarLsStartZ;
			pModule->m_fBurglarLsEndZ   = dlg.m_fBurglarLsEndZ;
			_snprintf(pModule->segStr,99,"%s",dlg.m_sIncPartSegStr);
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDSGROUP_)||defined(__TSA_)||defined(__TSAGROUP_)
			ss.Format("%s(%d)",pModule->description,pModule->iNo);
#else
			ss.Format("%s",pModule->description);
#endif
			HTREEITEM hModel=NULL,hChild = pTreeCtrl->GetChildItem(hItem);
			while(hChild!=NULL)
			{
				TREEITEM_INFO *pChildItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hChild);
				if(pChildItemInfo && pChildItemInfo->itemType==MODEL_CASE)
					hModel=hChild;
				else if(hModel)
					break;
				hChild = pTreeCtrl->GetNextSiblingItem(hChild);
			}
			HTREEITEM hParent=pTreeCtrl->InsertItem(ss,PRJ_IMG_MODULECASE,PRJ_IMG_MODULECASE,hItem,hModel);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_CASE,(DWORD)pModule));
			pTreeCtrl->SetItemData(hParent,(DWORD)pItem);
			ShiftActiveItemState(hParent,ACTIVE_MODULE);

			CString ss_leg[4],sTemp;
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("Geography Height=%.1f",pModule->land_high);
#else 
			ss.Format("���θ�=%.1f",pModule->land_high);
#endif
			hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hParent);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_LAND_HIGH,0));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItem);
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("Anti-theft Bolt Z Scope:%.0f~%.0f",pModule->m_fBurglarLsStartZ,pModule->m_fBurglarLsEndZ);
#else 
			ss.Format("������˨Z���귶Χ:%.0f~%.0f",pModule->m_fBurglarLsStartZ,pModule->m_fBurglarLsEndZ);
#endif
			hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hParent);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_BURGLAR_LS_SCOPE,0));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItem);
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("Body No:%d",pModule->GetBodyNo());
#else 
			ss.Format("�����:%d",pModule->GetBodyNo());
#endif
			hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_TOWERBODY,PRJ_IMG_TOWERBODY,hParent);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_BODY_LEGNO,0));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItem);
			int nMaxLegs=CFGLEG::MaxLegs();
			for(int i=1;i<=192;i++)
			{
				for(int j=0;j<4;j++)
				{
					if(pModule->m_dwLegCfgWord.IsHasNo(i))
					{
						if(ss_leg[j].GetLength()==0)//��һ�����Ⱥ�
						{
							pModule->m_arrActiveQuadLegNo[j]=i;
							sTemp.Format("%C(*)",(i-1)%nMaxLegs+'A');
						}
						else
							sTemp.Format(",%C",(i-1)%nMaxLegs+'A');
						ss_leg[j]+=sTemp;
					}
				}
			}
			for(int j=0;j<4;j++)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sTemp.Format("Joint Leg(%d)Model Flag:",j+1);
#else 
				sTemp.Format("����(%d)��ĺ�:",j+1);
#endif
				ss_leg[j]=sTemp+ss_leg[j];
				hItem=pTreeCtrl->InsertItem(ss_leg[j],PRJ_IMG_TOWERLEG,PRJ_IMG_TOWERLEG,hParent);
				if(j==0)
					pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD1LEG_CFGNO,0));
				else if(j==1)
					pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD2LEG_CFGNO,0));
				else if(j==2)
					pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD3LEG_CFGNO,0));
				else if(j==3)
					pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_QUAD4LEG_CFGNO,0));
				else 
					pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(0,0));
				pTreeCtrl->SetItemData(hItem,(DWORD)pItem);
			}
		}
	}
	else if(pItemInfo->itemType==MODEL_DATUM_POINT_GROUP)
	{	//�½���׼��
		CDatumPointPropertyDlg new_point_dlg;
		if(new_point_dlg.DoModal()==IDOK)
		{
			CLDSPoint *pPoint=console.AppendPoint();
			new_point_dlg.m_xPoint.handle=pPoint->handle;
			DWORD dwPermission=pPoint->dwPermission;
			*pPoint=new_point_dlg.m_xPoint;
			pPoint->dwPermission=dwPermission;
			//CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(MODEL_DATUM_POINT_GROUP);
			//pPoint->iNo=pNoGroup->EnumIdleNo();
			//pNoGroup->SetNoState(pPoint->iNo);
			pPoint->UpdatePos(console.GetActiveModel());
			CString ss;
			ss.Format("%s_0X%X",pPoint->description,pPoint->handle);//,pPoint->iNo);
			HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hItem);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pPoint));
			pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
		}
		//else
		//	Ta.Point.DeleteNode(new_point_dlg.m_pPoint->handle);
	}
	else if(pItemInfo->itemType==MODEL_DATUM_LINE_GROUP)
	{	//�½���׼��
		//CDatumLineDlg new_line_dlg;
		CDatumLinePropertyDlg	new_line_dlg;
		CString ss;
		if(new_line_dlg.DoModal()==IDOK)
		{
			HTREEITEM hPointGroupItem=pTreeCtrl->GetPrevSiblingItem(hItem);	//��������ͼ�µĻ�׼��ڵ�
			CLDSPoint* pDatumPoint=NULL;
			if(new_line_dlg.m_bNewStartPoint)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(new_line_dlg.m_startPoint.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			if(new_line_dlg.m_bNewEndPoint)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(new_line_dlg.m_endPoint.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			CLDSLine* pLine=console.AppendLine();
			CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(MODEL_DATUM_LINE_GROUP);
			pLine->iNo=pNoGroup->EnumIdleNo();
			pNoGroup->SetNoState(pLine->iNo);
			sprintf(pLine->description,"%s",new_line_dlg.m_datumLine.description);
			pLine->hPointArr[0]=new_line_dlg.m_datumLine.hPointArr[0];
			pLine->hPointArr[1]=new_line_dlg.m_datumLine.hPointArr[1];
			//��������׼�߸��µ������б���,���뵽��׼�߽ڵ���
			ss.Format("%s_0X%X",pLine->description,pLine->handle);//,pLine->iNo);
			HTREEITEM hLineItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_LINE,PRJ_IMG_DATUM_LINE,hItem);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_LINE_CASE,(DWORD)pLine));
			pTreeCtrl->SetItemData(hLineItem,(DWORD)pItem);
		}
	}
	else if(pItemInfo->itemType==MODEL_DATUM_PLANE_GROUP)
	{	//�½���׼��
		HTREEITEM hLineGroupItem=pTreeCtrl->GetPrevSiblingItem(hItem);	//��������ͼ�µĻ�׼��ڵ�
		HTREEITEM hPointGroupItem=pTreeCtrl->GetPrevSiblingItem(hLineGroupItem);
		CDatumPlanePropertyDlg new_plane_dlg;
		CLDSPoint* pDatumPoint=NULL;
		CString ss;
		if(new_plane_dlg.DoModal()==IDOK)
		{
			if(new_plane_dlg.m_bNewPoint1)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(new_plane_dlg.m_datumPoint1.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			if(new_plane_dlg.m_bNewPoint2)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(new_plane_dlg.m_datumPoint2.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			if(new_plane_dlg.m_bNewPoint3)
			{	//��������׼����뵽��׼�ڵ���
				pDatumPoint=Ta.Point.FromHandle(new_plane_dlg.m_datumPoint3.handle);
				ss.Format("%s_0X%X",pDatumPoint->description,pDatumPoint->handle);
				HTREEITEM hPointItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_POINT,PRJ_IMG_DATUM_POINT,hPointGroupItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_POINT_CASE,(DWORD)pDatumPoint));
				pTreeCtrl->SetItemData(hPointItem,(DWORD)pItem);
			}
			CLDSPlane *pPlane=console.AppendPlane();
			sprintf(pPlane->description,"%s",new_plane_dlg.m_datumPlane.description);
			pPlane->hPointArr[0]=new_plane_dlg.m_datumPlane.hPointArr[0];
			pPlane->hPointArr[1]=new_plane_dlg.m_datumPlane.hPointArr[1];
			pPlane->hPointArr[2]=new_plane_dlg.m_datumPlane.hPointArr[2];
			pPlane->UpdatePlane(console.GetActiveModel());
			//��������׼����µ������б���,���뵽��׼��ڵ���
			ss.Format("%s_0X%X",pPlane->description,pPlane->handle);//,pLine->iNo);
			HTREEITEM hPlaneItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_DATUM_PLANE,PRJ_IMG_DATUM_PLANE,hItem);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_DATUM_PLANE_CASE,(DWORD)pPlane));
			pTreeCtrl->SetItemData(hPlaneItem,(DWORD)pItem);
		}
	}
	else if(pItemInfo->itemType==MODEL_ARCLIFT_GROUP)
	{	//�½�Ԥ����
#ifdef __ANGLE_PART_INC_
		InsertArcLiftItem(hItem);
#endif
	}
	else if(pItemInfo->itemType==VIEW_LAYERSET)
	{	//�½�ͼ����ͼ
		CLayerViewDlg new_view_dlg;
		CString ss;
		CDisplayView *pCurView = console.GetActiveView();
		if(pCurView)
			new_view_dlg.ucs = pCurView->ucs;
		else
			LoadDefaultUCS(&new_view_dlg.ucs);
#ifdef AFX_TARG_ENU_ENGLISH
		new_view_dlg.m_sViewName = "New Layer View";
#else 
		new_view_dlg.m_sViewName = "�½�ͼ����ͼ";
#endif
		new_view_dlg.m_sDefaultLayer = pCurView->defaultLayer;
		if(IDOK==new_view_dlg.DoModal())
		{
			//�����ͼ���Ե�����
			CDisplayView *pView = Ta.AppendDisplayView();
			_snprintf(pView->defaultLayer,3,"%s",new_view_dlg.m_sDefaultLayer);
			sprintf(pView->filter.FirstCharSet, "%s", new_view_dlg.m_sFirstCharSet);
			sprintf(pView->filter.SecondCharSet, "%s", new_view_dlg.m_sSecondCharSet);
			sprintf(pView->filter.ThirdCharSet, "%s", new_view_dlg.m_sThirdCharSet);
			sprintf(pView->name, "%s", new_view_dlg.m_sViewName);
			pView->m_iViewType = 0;		//��ͼ���� 0 ͼ����ͼ 1 ������ͼ
			strcpy(pView->defaultLayer,theApp.env.layer);
			pView->ucs=new_view_dlg.ucs;
			//��ʾ��ͼ
			ss.Format("%s",pView->name);
			hItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hItem);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_CASE,(DWORD)pView));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItem);
		}
	}
	else if(pItemInfo->itemType==VIEW_DEFINEDSET)
	{	//�½�������ͼ
		CString ss;
		CDefinedViewPropDlg defined_view_dlg; 
		CDisplayView *pCurView = console.GetActiveView();
		defined_view_dlg.ucs = pCurView->ucs;
		CXhChar16 folderName(Ta.m_iCurSeg.ToString());	//��ǰ������ͼ��Ӧ�Ķκ� wht 10-12-27
		//defined_view_dlg.m_sFolderName=Ta.m_iCurSeg.ToString();	//��ǰ������ͼ��Ӧ�Ķκ� wht 10-12-27
		//ͳ�Ƶ�ǰ����ͼ����Ԥ������ͼ
		int i=1;
		CDisplayView *pTempView=NULL;
		for(pTempView=Ta.GetFirstDisplayView();pTempView;pTempView=Ta.GetNextDisplayView())
		{
			if(pTempView->m_iViewType!=1)
				continue;
			//if(pTempView->GetSegI()!=Ta.m_iCurSeg)
			//	continue;	
			if(!folderName.EqualNoCase(pTempView->folderName))
				continue;	//�����뵱ǰ��ͼ������ͬ�Ķ�����ͼ���� wht 11-07-09
			i++;
		}
		//defined_view_dlg.m_sViewName.Format("%d-%d",i,i);
		//defined_view_dlg.m_sDefaultLayer = pCurView->defaultLayer;
		//�����ͼ���Ե����� wht 10-12-27
		CDisplayView *pView = Ta.AppendDisplayView();
		sprintf(pView->name,"%d-%d",i,i);
		memcpy(pView->defaultLayer,pCurView->defaultLayer,4);
		pView->m_iViewType=1;	//������ͼ wht 11-07-09
		for(CLDSPart *pPart=console.DispPartSet.GetFirst(); pPart; pPart=console.DispPartSet.GetNext())
		{
			if(!pPart->is_visible)
				continue;		//�������ȥ����
			CViewPart *pViewPart = pView->AppendViewPart(pPart->handle);
			pViewPart->idClassType=pPart->GetClassTypeId();
			pViewPart->cFlag=GetViewPartFlag(pView,pPart);	//��ͼ��������ͼ���
		}
		for(CLDSNode *pNode=console.DispNodeSet.GetFirst(); pNode; pNode=console.DispNodeSet.GetNext())
		{
			if(!pNode->is_visible)
				continue;		//�������ȥ����
			CViewPart *pViewPart = pView->AppendViewPart(pNode->handle);
			pViewPart->idClassType=CLS_NODE;
		}
		if(pView==NULL)
			return;
		defined_view_dlg.m_pDefinedView=pView;
		if(defined_view_dlg.DoModal()==IDOK)
		{	//������ͼ�������
			//_snprintf(pView->defaultLayer,3,"%s",defined_view_dlg.m_sDefaultLayer);
			//_snprintf(pView->folderName,16,"%s",defined_view_dlg.m_sFolderName);
			//sprintf(pView->name, "%s", defined_view_dlg.m_sViewName);
			pView->m_iViewType = 1;		//��ͼ���� 0 ͼ����ͼ 1 ������ͼ
			strcpy(pView->defaultLayer,theApp.env.layer);
			pView->ucs=defined_view_dlg.ucs;
			//��ʾ��ͼ
			HTREEITEM hFolderItem=NULL,hChildItem=pTreeCtrl->GetChildItem(hItem);
			CString sFolderName=pView->folderName;
			while(hChildItem&&sFolderName.GetLength()>0)
			{
				TREEITEM_INFO *pChildItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hChildItem);
				CDisplayView *pChildView=(CDisplayView*)pChildItemInfo->dwRefData;
				if(pChildItemInfo&&pChildView&&pChildItemInfo->itemType==VIEW_DEFINEDFOLDER
					&&sFolderName.CompareNoCase(pChildView->folderName)==0)
				{	//�Ѵ����½���ͼ�����飬��ֱ����ӵ���Ӧ����
					hFolderItem=hChildItem;
					break;
				}
				hChildItem=pTreeCtrl->GetNextSiblingItem(hChildItem);
			}
			if(hFolderItem==NULL)
			{	//������ָ���ķ���,�½����� wht 10-12-27
				CDisplayView *pViewFolder = Ta.AppendDisplayView();
				pViewFolder->m_iViewType = 2;	//��ͼ���� 0 ͼ����ͼ 1 ������ͼ 2 ���洢������ͼ
				_snprintf(pViewFolder->folderName,16,"%s",pView->folderName);
				ss.Format("%s",pViewFolder->folderName);
				hFolderItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hItem);
				pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_DEFINEDFOLDER,(DWORD)pViewFolder));
				pTreeCtrl->SetItemData(hFolderItem,(DWORD)pItem);
			}
			ss.Format("%s",pView->name);
			if(hFolderItem)
				hItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hFolderItem);
			else
				hItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hItem);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_CASE,(DWORD)pView));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItem);
			//�½�������ͼ��Ӧ������Ϊ����״̬
			pTreeCtrl->SelectItem(hItem);
			//���ǰ��ͼ
			OnSetActiveItem();
		}
		else	//��ȡ������ɾ���½�����ͼ
			console.DeleteDisplayView(pView->handle);
	}
#if defined(__LDS_)||defined(__TSA_)
	else if(pItemInfo->itemType==MODEL_RESTRICT)
	{	//ģ��Լ���ڵ�
		static CSetNodeRestrictDlg dlg;
		dlg.m_bNewRestrictNode=TRUE;
		if(dlg.DoModal()!=IDOK)
			return;
		CString ss=pTreeCtrl->GetItemText(hItem);
		THANDLE hNode=0;
		sscanf(dlg.m_hNode,"%X",&hNode);
		CLDSNode *pNode=console.FromNodeHandle(hNode);
		if(pNode==NULL)	//�������δ�ҵ��Ľڵ�
		{
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(pNode->point_i!=0&&pNode->point_i==dlg.m_nNodeI)
					break;
			}
		}
		if(pNode)
		{
			if(pNode->IsHasRestrict(ALL_RESTRICT))
			{
				AfxMessageBox("�˽ڵ�����Լ���ڵ��б���,��ֱ���޸�!");
				return;
			}
			pNode->SetRestrict(X_RESTRICT,dlg.m_bRestrictX);
			pNode->SetRestrict(Y_RESTRICT,dlg.m_bRestrictY);
			pNode->SetRestrict(Z_RESTRICT,dlg.m_bRestrictZ);
			if(dlg.m_bRestrictX||dlg.m_bRestrictY||dlg.m_bRestrictZ)
			{
				ss.Format("0X%X(%d)��",pNode->handle,pNode->point_i);
				int iOrder=0;
				if(pNode->IsHasRestrict(X_RESTRICT))
				{
					ss+="Rx";
					iOrder++;
				}
				if(pNode->IsHasRestrict(Y_RESTRICT))
				{
					if(iOrder>0)
						ss+=";Ry";
					else
						ss+="Ry";
					iOrder++;
				}
				if(pNode->IsHasRestrict(Z_RESTRICT))
				{
					if(iOrder>0)
						ss+=";Rz";
					else
						ss+="Rz";
				}
				hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				TREEITEM_INFO *pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(MODEL_RESTRICT_NODE,(DWORD)pNode));
				pTreeCtrl->SetItemData(hItem,(DWORD)pNewInfo);
			}
		}
		else
			AfxMessageBox("ѡ��Լ���ڵ㲻���ڿ����ѱ�ɾ��!");
	}
	/*else if(pItemInfo->itemType==WIRELAYOUT_GROUP)
	{
		CString name;
		Input("�º��ط�������:",&name,'s');
		CWireLayout *pWireLay=Ta.groupWireLayout.append();
		strncpy(pWireLay->name,name,49);
		CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WIRELAYOUT_GROUP);
		pWireLay->iNo=pNoGroup->EnumIdleNo();
		pNoGroup->SetNoState(pWireLay->iNo);
		name.Format("%s(%d)",pWireLay->name,pWireLay->iNo);
		//�������ڵ�
		HTREEITEM hWireLayoutItem=pTreeCtrl->InsertItem(name,PRJ_IMG_WIRELAYOUT,PRJ_IMG_WIRELAYOUT,hItem);
		pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WIRELAYOUT_CASE,(DWORD)pWireLay));
		pTreeCtrl->SetItemData(hWireLayoutItem,(DWORD)pItemInfo);
		HTREEITEM hSubItem = pTreeCtrl->InsertItem("�������ط���",PRJ_IMG_WIRELOADSET,PRJ_IMG_WIRELOADSET,hWireLayoutItem);
		pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WIRELOAD_MODEL_GROUP,0));
		pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);
		hSubItem = pTreeCtrl->InsertItem("���ع���",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hWireLayoutItem);
		pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_GROUP,0));
		pTreeCtrl->SetItemData(hSubItem,(DWORD)pItemInfo);
		pTreeCtrl->Expand(hWireLayoutItem,TVE_EXPAND);
		Ta.SetDocModifiedFlag();
	}
	else if(pItemInfo->itemType==WIRELOAD_MODEL_GROUP)
	{
		static CWorkEnvLoadModelDlg dlg;
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hItem);
		TREEITEM_INFO* pParentInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		CWireLayout *pWireLayout=(CWireLayout*)pParentInfo->dwRefData;
		Ta.InitWeather(&dlg.wireModel);
		if(dlg.DoModal()==IDOK)
		{
			CWireLoadModel *pWireModel;
			CObjNoGroup groupNo;
			groupNo.InitMaxNo(1000);
			for(pWireModel=pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=pWireModel=pWireLayout->hashWireLoadModel.GetNext())
				groupNo.SetNoState(pWireModel->iNo/10);
			dlg.wireModel.iNo += groupNo.EnumIdleNo()*10;
			pWireModel=pWireLayout->hashWireLoadModel.Add(dlg.wireModel.iNo);
			pWireModel->CopyWireLoadModel(&dlg.wireModel);
			pWireModel->ResetWorkEnvLoadSet();
			pWireModel->CalWorkEnvLoad(Ta.m_cLandscape,Ta.m_cLineRoadType);
			CString ss;
			ss.Format("%s(%d)",pWireModel->name,pWireModel->iNo);
			hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_WIRELOAD,PRJ_IMG_WIRELOAD,hItem);
			TREEITEM_INFO* pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WIRELOAD_MODEL_CASE,(DWORD)pWireModel));
			pTreeCtrl->SetItemData(hItem,(DWORD)pNewInfo);
		}
	}*/
	else if(pItemInfo->itemType==WORKSTATUS_GROUP)
	{
		static CWorkStatusDlg dlg;
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hItem);
		//TREEITEM_INFO* pParentInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		//CWireLayout *pWireLay=(CWireLayout*)pParentInfo->dwRefData;

		CWorkStatus workCond;
		dlg.m_pWorkStatus=&workCond;
		CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
		int iNo=pNoGroup->EnumIdleNo();
		pNoGroup->SetNoState(iNo);
		dlg.m_sStatusName.Format("%d",iNo);
		if(dlg.DoModal()==IDOK)
		{
			CWorkStatus *pStatus=Ta.WorkStatus.append();
			pStatus->iNo=iNo;
			//pStatus->iWireLayoutNo=pWireLay->iNo;
			CString ss;
			if(dlg.m_sStatusName.IsEmpty())
			{
				ss.Format("%d(%d)",pStatus->iNo,pStatus->iNo);
				sprintf(pStatus->description,"%d",pStatus->iNo);
			}
			else
			{
				ss.Format("(%d)",pStatus->iNo);
				ss=dlg.m_sStatusName+ss;
				sprintf(pStatus->description,"%s",dlg.m_sStatusName);
			}
			pStatus->WindLoad=workCond.WindLoad;
			strncpy(pStatus->limit_str,workCond.limit_str,100);
			pStatus->ice_thick=workCond.ice_thick;
			hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hItem);
			TREEITEM_INFO* pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_CASE,(DWORD)pStatus));
			pTreeCtrl->SetItemData(hItem,(DWORD)pNewInfo);
			ss.Format("�ɱ�������ϵ��=%.2f",pStatus->Kesai());
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			ss.Format("���ú����鷶Χ=%s",dlg.m_sModuleScope);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			ss.Format("�����������=%.2fmm",pStatus->ice_thick);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			HTREEITEM hPointLoadItem=pTreeCtrl->InsertItem("���к���",PRJ_IMG_POINTLOAD,PRJ_IMG_POINTLOAD,hItem);
			pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_POINTLOAD,0));
			pTreeCtrl->SetItemData(hPointLoadItem,(DWORD)pNewInfo);
			HTREEITEM hWindItem=pTreeCtrl->InsertItem("�����",PRJ_IMG_WINDLOAD,PRJ_IMG_WINDLOAD,hItem);
			pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_WINDLOAD,0));
			ss.Format("����=%f",dlg.m_fWindVelocity);
			SimplifiedNumString(ss);
			ss+="m/s";
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hWindItem);
			ss.Format("�����=%d��",dlg.m_nWindAzimuth);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hWindItem);
			ss.Format("���Ӻ���");
			hWindItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_ATTACH_WINDLOAD,PRJ_IMG_ATTACH_WINDLOAD,hItem);
			pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_ATTACH_WINDLOADREF,0));
			pTreeCtrl->SetItemData(hWindItem,(DWORD)pNewInfo);
			pStatus->m_cMainLoadType=workCond.m_cMainLoadType;
			pStatus->m_cHoriEarthQuakeAxis=workCond.m_cHoriEarthQuakeAxis;
		}
	}
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOAD)
	{
		CAttachWindLoadDlg dlg;
		if(dlg.DoModal()!=IDOK)
			return;
		CAttachWindLoad *pAttachLoad=Ta.AttachWindLoad.append();
		CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_ATTACH_WINDLOAD);
		pAttachLoad->iNo=pNoGroup->EnumIdleNo();
		pNoGroup->SetNoState(pAttachLoad->iNo);
		sprintf(pAttachLoad->name,"%s",dlg.m_sAttachWindLoadName);
		pAttachLoad->m_fShieldArea=dlg.m_fShieldArea;			//�������
		pAttachLoad->m_fShieldCoef=dlg.m_fShieldCoef;			//����ϵ��
		pAttachLoad->m_fProfileCoef=dlg.m_fProfileCoef;	//����ϵ��
		pAttachLoad->m_fAttachWeight=dlg.m_fAttachWeight;			//����߶�
		pAttachLoad->m_fBetaZ=dlg.m_fBetaZ;
		pAttachLoad->m_fBetaZForFoundation=dlg.m_fBetaZForFoundation;
		for(int i=0;i<dlg.node_arr.GetSize();i++)
			pAttachLoad->GetNodeList()->append(dlg.node_arr[i].h);
		CString ss;
		ss.Format("%s(%d)",pAttachLoad->name,pAttachLoad->iNo);
		hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_ATTACH_WINDLOAD,PRJ_IMG_ATTACH_WINDLOAD,hItem);
		TREEITEM_INFO* pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_ATTACH_WINDLOADITEM,(DWORD)pAttachLoad));
		pTreeCtrl->SetItemData(hItem,(DWORD)pNewInfo);
	}
	else if(pItemInfo->itemType==WINDSEG_GROUP)
	{
		static CWindLoadSegDlg dlg;
		static CWindSegment seg;
		memset(seg.nodeArr,0,seg.GetMaxNodeCount()*sizeof(long));
		memset(seg.nodePtrArr,0,seg.GetMaxNodeCount()*sizeof(long));
		CWindSegment *pSeg=Ta.WindLoadSeg.append(seg);
		CObjNoGroup* pObjNoGroup=Ta.NoManager.FromGroupID(WINDSEG_GROUP);
		dlg.m_pWindSeg=pSeg;
		if(dlg.DoModal()==IDOK)
		{
			pSeg->iNo=pObjNoGroup->EnumIdleNo();
			pObjNoGroup->SetNoState(pSeg->iNo);
			CString ss;
			ss.Format("%d",pSeg->iNo);
			hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_WINDSEG,PRJ_IMG_WINDSEG,hItem);
			TREEITEM_INFO* pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WINDSEG_CASE,(DWORD)pSeg));
			pTreeCtrl->SetItemData(hItem,(DWORD)pNewInfo);
			if(pSeg->cMirType==1)
				ss="Y��Գ�";
			else if(pSeg->cMirType==2)
				ss="X��Գ�";
			else
				ss="�޶Գ�";
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			if(dlg.m_iSegType==0)
				ss="��ѹ������=0.�˵������������";
			else if(dlg.m_iSegType==1)
				ss="��ѹ������=1.�Զ����ڵ�����";
			else //if(dlg.m_iSegType==2)
				ss="��ѹ������=2.����������������";
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			if(dlg.m_iCalStyle==0)
				ss="���㷽ʽ=����ʽ";
			else if(dlg.m_iCalStyle==1)
				ss="���㷽ʽ=X����ᵣ";
			else// if(dlg.m_iCalStyle==2)
				ss="���㷽ʽ=Y����ᵣ";
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			int n=pSeg->StatValidNodeCount(console.GetActiveModel());
			for(int i=0;i<n;i++)
			{
				ss.Format("�ڵ�%d:0X%X(%d)",i+1,pSeg->nodePtrArr[i]->handle,pSeg->nodePtrArr[i]->point_i);
				pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			}
			ss.Format("����ϵ��(����)=%.2f",dlg.m_fBetaZ);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			ss.Format("����ϵ��(����)=%.2f",dlg.m_fBetaZForFoundation);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			ss.Format("����ЧӦϵ��=%.2f",dlg.m_fFrontCoef);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			ss.Format("����ЧӦϵ��=%.2f",dlg.m_fSideCoef);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			ss.Format("���������״ϵ��=%.2f",dlg.m_fCoefK);
			pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
			seg=*pSeg;
		}
		else
			Ta.WindLoadSeg.DeleteCursor(TRUE);
	}
#endif
#endif
}
//</DEVELOP_PROCESS_MARK>
void CTowerTreeDlg::DeleteItem(HTREEITEM hItem) 
{
#ifdef _RELATIVE_DLG
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	if(AfxMessageBox("Are you sure to delete selected item��",MB_YESNO)==IDNO)
#else 
	if(AfxMessageBox("ȷ��Ҫɾ��ѡ������",MB_YESNO)==IDNO)
#endif
		return;
	m_treeCtrl.SelectItem(pTreeCtrl->GetParentItem(hItem));
	if(pItemInfo->itemType==MODEL_CASE)
	{
		if(Ta.Module.GetNodeNum()<=1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The last model group instance cannot be deleted��");
#else 
			AfxMessageBox("���һ�����������ʵ������ɾ����");
#endif
			return;
		}
		CLDSModule *pModule=(CLDSModule*)pItemInfo->dwRefData;
		if(pModule)
		{
			BOOL bChangeActiveModule=FALSE;
			if(pModule->handle==Ta.m_hActiveModule)
				bChangeActiveModule=TRUE;
			Ta.Module.DeleteNode(pModule->handle);
			CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
			pGroup->SetNoState(pModule->iNo,FALSE);
			pTreeCtrl->DeleteItem(hItem);
			if(bChangeActiveModule)
			{	//�ı䵱ǰ����ģ��
				pModule=Ta.Module.GetFirst();
				Ta.m_hActiveModule=pModule->handle;
				hItem=pTreeCtrl->GetRootItem();
				hItem=pTreeCtrl->GetChildItem(hItem);
				while(hItem)
				{
					TREEITEM_INFO *pItem=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hItem);
					if(pItem&&pItem->itemType==MODEL_CASE&&pItem->dwRefData==(DWORD)pModule)
						pTreeCtrl->SetItemState(hItem,TVIS_BOLD,TVIS_BOLD);
					hItem=pTreeCtrl->GetNextItem(hItem,TVGN_NEXT);
				}
			}
		}
	}
	else if(pItemInfo->itemType==MODEL_ARCLIFT_CASE)
	{	//ɾ��Ԥ�������
#ifdef __ANGLE_PART_INC_
		CLDSArcLift *pArcLift=(CLDSArcLift*)pItemInfo->dwRefData;
		if(pArcLift&&theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pArcLift->dwPermission))
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("No corresponding deletion permission!");
#else
			AfxMessageBox("û����Ӧ��ɾ��Ȩ�ޣ�");
#endif
		else if(pArcLift)
		{
			Ta.ArcLift.DeleteNode(pArcLift->handle);
			CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_ARCLIFT_GROUP);
			pGroup->SetNoState(pArcLift->iNo,FALSE);
			pTreeCtrl->DeleteItem(hItem);
		}
#endif
	}
	else if(pItemInfo->itemType==MODEL_DATUM_POINT_CASE)
	{	//ɾ����׼��
		for(CLDSPoint *pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
		{
			if(pPoint==(void*)pItemInfo->dwRefData)
			{
				Ta.Point.DeleteNode(pPoint->handle);
				pTreeCtrl->DeleteItem(hItem);
				break;
			}
		}
	}
	else if(pItemInfo->itemType==MODEL_DATUM_LINE_CASE)
	{	//ɾ����׼��
		for(CLDSLine *pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
		{
			if(pLine==(void*)pItemInfo->dwRefData)
			{
				Ta.Line.DeleteNode(pLine->handle);
				pTreeCtrl->DeleteItem(hItem);
				break;
			}
		}
	}
	else if(pItemInfo->itemType==MODEL_DATUM_PLANE_CASE)
	{	//ɾ����׼��
		for(CLDSPlane *pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
		{
			if(pPlane==(void*)pItemInfo->dwRefData)
			{
				Ta.Plane.DeleteNode(pPlane->handle);
				pTreeCtrl->DeleteItem(hItem);
				break;
			}
		}
	}
	else if(pItemInfo->itemType==VIEW_CASE)
	{	//ɾ����ͼ
		CDisplayView *pView=(CDisplayView*)pItemInfo->dwRefData;
		if(pView)
		{
			if(console.DeleteDisplayView(pView->handle))
				pTreeCtrl->DeleteItem(hItem);
			else
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("Can't delete without permission or current displayed view!!");
#else 
				AfxMessageBox("����ɾ��û��Ȩ�޻�ǰ������ʾ����ͼ!!");
#endif
		}
	}
	else if(pItemInfo->itemType==VIEW_DEFINEDFOLDER)
	{	//ɾ����ͼ�� �Լ����ڸ������ͼ
		CString sFolderName,ss;
		CDisplayView *pFolder=(CDisplayView*)pItemInfo->dwRefData;
		if(pFolder)
			sFolderName=pFolder->folderName;
		bool reserved=false;
		HTREEITEM hSubItem=pTreeCtrl->GetNextItem(hItem,TVGN_CHILD);
		while(hSubItem!=NULL)
		{
			HTREEITEM hNextItem=pTreeCtrl->GetNextItem(hSubItem,TVGN_NEXT);
			pTreeCtrl->DeleteItem(hSubItem);
			hSubItem=hNextItem;
		};
		for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView->m_iViewType==1&&sFolderName.CompareNoCase(pView->folderName)==0)
			{
				if(!console.DeleteDisplayView(pView->handle))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Can't delete without permission or current displayed view!!");
#else 
					AfxMessageBox("����ɾ��û��Ȩ�޻�ǰ������ʾ����ͼ!!");
#endif
					//���²��벻��ɾ������ͼ��������ͼ��
					ss.Format("%s",pView->name);
					HTREEITEM hParentItem = pTreeCtrl->GetParentItem(hItem);
					HTREEITEM hNewItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hParentItem);
					TREEITEM_INFO *pNewItem=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_CASE,(DWORD)pView));
					pTreeCtrl->SetItemData(hNewItem,(DWORD)pNewItem);
					reserved=true;
					continue;
				}
			}
		}
		if(!reserved)
			pTreeCtrl->DeleteItem(hItem);	//ɾ�����Լ����Ա
	}
#if defined(__LDS_)||defined(__TSA_)
	else if(pItemInfo->itemType==WORKSTATUS_CASE)
	{
		if(Ta.WorkStatus.GetNodeNum()==1)
		{
			AfxMessageBox("���һ�����㹤��ʵ������ɾ����");
			return;
		}
		CWorkStatus *pStatus=(CWorkStatus*)pItemInfo->dwRefData;
		if(pStatus)
		{
			CObjNoGroup *pStatusGroup=Ta.NoManager.FromGroupID(WORKSTATUS_GROUP);
			pStatusGroup->SetNoState(pStatus->iNo,FALSE);
			pTreeCtrl->DeleteItem(hItem);
			Ta.WorkStatus.DeleteNode(pStatus->handle);
			return;
		}
		AfxMessageBox("δ�ҵ��˹���!");
		return;
	}
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOADITEM)
	{ 
		pTreeCtrl->DeleteItem(hItem);
		CAttachWindLoad *pAttachLoad=(CAttachWindLoad*)pItemInfo->dwRefData;
		if(pAttachLoad)
		{
			for(CWorkStatus *pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
				pStatus->AttachWindLoad.DeleteNode(pAttachLoad->handle);
				Ta.AttachWindLoad.DeleteNode(pAttachLoad->handle);
			CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_ATTACH_WINDLOAD);
			pNoGroup->SetNoState(pAttachLoad->iNo,FALSE);
		}
	}
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOADREFITEM)
	{ 
		HTREEITEM hParent=pTreeCtrl->GetParentItem(hItem);	//���˼��к���һ��
		hParent=pTreeCtrl->GetParentItem(hParent);	//���˹���һ��
		TREEITEM_INFO* pParentInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParent);
		if(pParentInfo==NULL)
			return;
		pTreeCtrl->DeleteItem(hItem);
		CWorkStatus *pStatus=(CWorkStatus*)pParentInfo->dwRefData;
		if(pStatus)
		{
			CAttachWindLoad *pAttachLoad=(CAttachWindLoad*)pItemInfo->dwRefData;
			if(pAttachLoad)
			{
				for(CWorkStatus::WINDLOAD_REFPARAM *pRefParam=pStatus->AttachWindLoad.GetFirst();pRefParam;
				pRefParam=pStatus->AttachWindLoad.GetNext())
				{
					if(pStatus->AttachWindLoad.GetCursorKey()==pAttachLoad->handle)
					{
						pStatus->AttachWindLoad.DeleteCursor();
						break;
					}
				}
			}
		}
	}
	else if(pItemInfo->itemType==WINDSEG_CASE)
	{
		CWindSegment *pSeg=(CWindSegment*)pItemInfo->dwRefData;
		if(pSeg)
		{
			pTreeCtrl->DeleteItem(hItem);
			CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WINDSEG_GROUP);
			pNoGroup->SetNoState(pSeg->iNo,FALSE);
			for(CWindSegment *pSegTemp=Ta.WindLoadSeg.GetFirst();pSegTemp;pSegTemp=Ta.WindLoadSeg.GetNext())
			{
				if(pSegTemp->iNo==pSeg->iNo)
				{
					Ta.WindLoadSeg.DeleteCursor();
					break;
				}
			}
		}
	}
	/*else if(pItemInfo->itemType==WIRELAYOUT_CASE)
	{
		CWireLayout *pWireLayout=(CWireLayout*)pItemInfo->dwRefData;
		if(pWireLayout&&Ta.groupWireLayout.GetNodeNum()>1)
		{
			for(CWireLayout *pTempWireLayout=Ta.groupWireLayout.GetFirst();pTempWireLayout;pTempWireLayout=Ta.groupWireLayout.GetNext())
			{
				if(pTempWireLayout==pWireLayout)
				{
					Ta.groupWireLayout.DeleteCursor();
					pTreeCtrl->DeleteItem(hItem);
					Ta.SetDocModifiedFlag();
					break;
				}
			}
		}
	}
	else if(pItemInfo->itemType==WIRELOAD_MODEL_CASE)
	{
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hItem);
		hParentItem=pTreeCtrl->GetParentItem(hParentItem);
		TREEITEM_INFO* pParentInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		CWireLayout *pWireLayout=NULL;
		if(pParentInfo->itemType==WIRELAYOUT_CASE)
			pWireLayout=(CWireLayout*)pParentInfo->dwRefData;
		CWireLoadModel *pModel = (CWireLoadModel *)pItemInfo->dwRefData;
		if(pWireLayout&&pModel)
		{
			for(CWireLoadModel *pWireModel=pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=pWireLayout->hashWireLoadModel.GetNext())
			{
				if (pWireModel == pModel)
				{
					pWireLayout->hashWireLoadModel.DeleteNode(pWireLayout->hashWireLoadModel.GetCursorKey());
					pTreeCtrl->DeleteItem(hItem);
					Ta.SetDocModifiedFlag();
					break;
				}
			}
		}
	}*/
#endif
	else if(pItemInfo->itemType==BLOCK_CASE&&PRODUCT_FUNC::IsHasBlockSetup())
	{
		CBlockModel *pBlock=(CBlockModel*)pItemInfo->dwRefData;
		if(pBlock==NULL)
			return;
		for(CBlockReference* pBlockRef=Ta.BlockRef.GetFirst();pBlockRef;pBlockRef=Ta.BlockRef.GetNext())
		{
			if(pBlock->handle==pBlockRef->m_hBlock)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The block reference in block asembly,can't delete!");
#else 
				AfxMessageBox("�����������д˲��������ã�����ɾ��!");
#endif
				return;
			}
		}
		if(console.GetActiveModel()==pBlock)
			ActiveModel(&Ta);	//��ǰҪɾ���Ĳ���Ϊ����״̬ʱ��Ҫ���л�����ģ��ΪTa,�ٽ���ɾ�� wht 13-04-09
		Ta.Block.DeleteNode(pBlock->handle);
		pTreeCtrl->DeleteItem(hItem);
		CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
		pLdsView->UpdatePropertyPage();
	}
	else if(pItemInfo->itemType==BLOCKREF_CASE)
	{
		CBlockReference* pBlockRef=(CBlockReference*)pItemInfo->dwRefData;
		if(pBlockRef)
		{
			if(pBlockRef->IsExploded())
			{
				for(CLDSPart *pPart=console.EnumPartFirst();pPart;pPart=console.EnumPartNext())
				{
					if(!pPart->IsMemberOfBlockRef(pBlockRef))
						continue;
					g_pSolidDraw->DelEnt(pPart->handle);
					console.DeletePart(pPart->handle);
				}
				for(CLDSNode* pNode=console.EnumNodeFirst();pNode;pNode=console.EnumNodeNext())
				{
					if(!pNode->IsMemberOfBlockRef(pBlockRef))
						continue;
					g_pSolidDraw->DelEnt(pNode->handle);
					console.DeleteNode(pNode->handle);
				}
			}
			else 
			{
				g_pSolidDraw->DelEnt(pBlockRef->handle);
				console.DispBlockRefSet.DeleteNode(pBlockRef->handle);
			}
			//
			Ta.BlockRef.DeleteNode(pBlockRef->handle);
			pTreeCtrl->DeleteItem(hItem);
			CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
			pLdsView->UpdatePropertyPage();
			g_pSolidDraw->Draw();
		}
	}
#endif
}

void CTowerTreeDlg::OnDeleteItem() 
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();		//Ŀǰ�ڹ�����һ��
	if(hItem)
		DeleteItem(hItem);
}

void CTowerTreeDlg::OnRefreshTree() 
{
	RefreshTreeCtrl();
}

HTREEITEM CTowerTreeDlg::GetModuleItemByNo(long iModuleNo)
{
	CString ss;
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hRoot=pTreeCtrl->GetRootItem();			//������ģ��һ��
	HTREEITEM hNextItem=pTreeCtrl->GetChildItem(hRoot);	//��ģ��ʵ��һ��
	while(hNextItem)
	{
		DWORD dwData=pTreeCtrl->GetItemData(hNextItem);
		long iNo=dwData%TREE_ITEM_GRADE;
		if(iNo==iModuleNo)
			return hNextItem;
		else
			hNextItem=pTreeCtrl->GetNextSiblingItem(hNextItem);
	}
	return NULL;
}

void CTowerTreeDlg::OnOK() 
{
	//ȷ������������
}

void CTowerTreeDlg::OnCancel() 
{
}

void CTowerTreeDlg::OnNewItemFolder() 
{
#ifdef _RELATIVE_DLG
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItem,*pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==VIEW_DEFINEDSET)
	{	//�½�������ͼ��
		CString ss;
		CFolderNameDlg folderNameDlg;
		folderNameDlg.m_nNameMaxLen=16;
#ifdef AFX_TARG_ENU_ENGLISH
		folderNameDlg.m_sFolderName = "Group Views";
#else 
		folderNameDlg.m_sFolderName = "��ͼ��";
#endif
		folderNameDlg.m_nDlgType = 1;	//������ͼ������
		//�����ͼ���Ե�����
		if(folderNameDlg.DoModal()==IDOK)
		{
			CDisplayView *pViewFolder = Ta.AppendDisplayView();
			pViewFolder->m_iViewType = 2;	//��ͼ���� 0 ͼ����ͼ 1 ������ͼ 2 ���洢������ͼ
			_snprintf(pViewFolder->folderName,16,"%s",folderNameDlg.m_sFolderName);
			ss.Format("%s",pViewFolder->folderName);
			hItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hItem);
			pItem=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_DEFINEDFOLDER,(DWORD)pViewFolder));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItem);
		}
	}
#endif
}
//<DEVELOP_PROCESS_MARK nameId="CTowerTreeDlg.OnRetrieveUdfViews">
void CTowerTreeDlg::OnRetrieveUdfViews()
{
	//1.������ȡ
	Ta.RetrieveUdfViews();
	//2.��ȡ������ͼ���ڵ�
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hUdfViewItem=pTreeCtrl->GetChildItem(m_hDataRootItem);
	while(hUdfViewItem!=NULL)
	{
		TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hUdfViewItem);
		if(pItemInfo && pItemInfo->itemType==VIEW_DEFINEDSET)
			break;
		else if(pItemInfo && pItemInfo->itemType==VIEW_LAYERSET)
			hUdfViewItem=pTreeCtrl->GetNextSiblingItem(hUdfViewItem);
		else
			hUdfViewItem=pTreeCtrl->GetChildItem(hUdfViewItem);
	}
	//3������������ͼ���¶�����ͼ���ڵ�
	CString ss;
	for(CDisplayView *pView=Ta.EnumViewFirst();pView;pView=Ta.EnumViewNext())
	{
		if(pView->m_iViewType!=1)		//������ͼ
			continue;
		ss.Format("%s",pView->name);
		CString sFolderName=pView->folderName;
		BOOL bHaveSameView=FALSE;
		HTREEITEM hFolderItem=NULL,hItem=NULL;
		HTREEITEM hTempItem=m_treeCtrl.GetChildItem(hUdfViewItem);
		TREEITEM_INFO *pItemInfo=NULL;
		while(hTempItem)
		{	//�����Ƿ��Ѿ�������ͬ�Ķ�����ͼ�ļ���
			pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hTempItem);
			CDisplayView *pFolder=(CDisplayView*)pItemInfo->dwRefData;
			if(pItemInfo&&pItemInfo->itemType==VIEW_DEFINEDFOLDER&&sFolderName.CompareNoCase(pFolder->folderName)==0)
			{
				hFolderItem=hTempItem;
				break;
			}
			hTempItem=m_treeCtrl.GetNextItem(hTempItem,TVGN_NEXT);
		}
		if(hFolderItem)
		{	//�����ڣ����Ҹ������Ƿ���ͬ���Ķ�����ͼ
			HTREEITEM hCaseItem=m_treeCtrl.GetChildItem(hFolderItem);
			while(hCaseItem)
			{
				pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hCaseItem);
				CDisplayView *pCaseView=(CDisplayView*)pItemInfo->dwRefData;
				if(pItemInfo&&pItemInfo->itemType==VIEW_CASE&&ss.CompareNoCase(pCaseView->name)==0)
				{	//������ͬ��������ͼ�������µ�view�滻�ɵ�view
					bHaveSameView=TRUE;
					pItemInfo->dwRefData=(DWORD)pView;
					pTreeCtrl->SetItemData(hCaseItem,(DWORD)pItemInfo);
					break;
				}
				hCaseItem=m_treeCtrl.GetNextItem(hCaseItem,TVGN_NEXT);
			}
		}
		else
		{	//�������ڣ��½���
			Ta.View.push_stack();
			CDisplayView *pViewFolder = Ta.AppendDisplayView();
			pViewFolder->m_iViewType=2;	//�½�������ͼ��ʱʹ�� ���洢
			strcpy(pViewFolder->folderName,pView->folderName);
			Ta.View.pop_stack();
			hFolderItem = pTreeCtrl->InsertItem(pViewFolder->folderName,PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hUdfViewItem);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_DEFINEDFOLDER,(DWORD)pViewFolder));
			pTreeCtrl->SetItemData(hFolderItem,(DWORD)pItemInfo);
		}
		if(hFolderItem && !bHaveSameView)
		{	//�з����Ҳ����ڸö�����ͼ
			hItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hFolderItem);
			TREEITEM_INFO *pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_CASE,(DWORD)pView));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
		}
	}
}
//</DEVELOP_PROCESS_MARK>
void CTowerTreeDlg::OnRetrieveDrawingViews()
{
	//1.������ȡ
	Ta.RetrieveDrawingViews();
	//2.��ȡ������ͼ���ڵ�
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hUdfViewItem=pTreeCtrl->GetChildItem(m_hDataRootItem);
	while(hUdfViewItem!=NULL)
	{
		TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hUdfViewItem);
		if(pItemInfo && pItemInfo->itemType==VIEW_DEFINEDSET)
			break;
		else if(pItemInfo && pItemInfo->itemType==VIEW_LAYERSET)
			hUdfViewItem=pTreeCtrl->GetNextSiblingItem(hUdfViewItem);
		else
			hUdfViewItem=pTreeCtrl->GetChildItem(hUdfViewItem);
	}
	//3������������ͼ���¶�����ͼ���ڵ�
	CString ss;
	for(CDisplayView *pView=Ta.EnumViewFirst();pView;pView=Ta.EnumViewNext())
	{
		if(pView->m_iViewType!=1)		//������ͼ
			continue;
		ss.Format("%s",pView->name);
		CString sFolderName=pView->folderName;
		BOOL bHaveSameView=FALSE;
		HTREEITEM hFolderItem=NULL,hItem=NULL;
		HTREEITEM hTempItem=m_treeCtrl.GetChildItem(hUdfViewItem);
		TREEITEM_INFO *pItemInfo=NULL;
		while(hTempItem)
		{	//�����Ƿ��Ѿ�������ͬ�Ķ�����ͼ�ļ���
			pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hTempItem);
			CDisplayView *pFolder=(CDisplayView*)pItemInfo->dwRefData;
			if(pItemInfo&&pItemInfo->itemType==VIEW_DEFINEDFOLDER&&sFolderName.CompareNoCase(pFolder->folderName)==0)
			{
				hFolderItem=hTempItem;
				break;
			}
			hTempItem=m_treeCtrl.GetNextItem(hTempItem,TVGN_NEXT);
		}
		if(hFolderItem)
		{	//�����ڣ����Ҹ������Ƿ���ͬ���Ķ�����ͼ
			HTREEITEM hCaseItem=m_treeCtrl.GetChildItem(hFolderItem);
			while(hCaseItem)
			{
				pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hCaseItem);
				CDisplayView *pCaseView=(CDisplayView*)pItemInfo->dwRefData;
				if(pItemInfo&&pItemInfo->itemType==VIEW_CASE&&ss.CompareNoCase(pCaseView->name)==0)
				{	//������ͬ��������ͼ�������µ�view�滻�ɵ�view
					bHaveSameView=TRUE;
					pItemInfo->dwRefData=(DWORD)pView;
					pTreeCtrl->SetItemData(hCaseItem,(DWORD)pItemInfo);
					break;
				}
				hCaseItem=m_treeCtrl.GetNextItem(hCaseItem,TVGN_NEXT);
			}
		}
		else
		{	//�������ڣ��½���
			Ta.View.push_stack();
			CDisplayView *pViewFolder = Ta.AppendDisplayView();
			pViewFolder->m_iViewType=2;	//�½�������ͼ��ʱʹ�� ���洢
			strcpy(pViewFolder->folderName,pView->folderName);
			Ta.View.pop_stack();
			hFolderItem = pTreeCtrl->InsertItem(pViewFolder->folderName,PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hUdfViewItem);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_DEFINEDFOLDER,(DWORD)pViewFolder));
			pTreeCtrl->SetItemData(hFolderItem,(DWORD)pItemInfo);
		}
		if(hFolderItem && !bHaveSameView)
		{	//�з����Ҳ����ڸö�����ͼ
			hItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hFolderItem);
			TREEITEM_INFO *pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(VIEW_CASE,(DWORD)pView));
			pTreeCtrl->SetItemData(hItem,(DWORD)pItemInfo);
		}
	}
}
static int CALLBACK 
CompareTreeItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort contains a pointer to the tree control.
	// The lParam of an item is just its handle.
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*) lParamSort;
	if(pTreeCtrl==NULL)
		return 0;
	TREEITEM_INFO *pInfo1=(TREEITEM_INFO*)lParam1;
	TREEITEM_INFO *pInfo2=(TREEITEM_INFO*)lParam2;
	if(pInfo1==NULL||pInfo2==NULL)
		return 0;
	if(pInfo1->itemType!=pInfo2->itemType)
		return 0;
	
	CString  strItem1="",strItem2="";
	if(pInfo1->itemType==VIEW_CASE)
	{	//������ͼ
		if(pInfo1->dwRefData)
			strItem1=((CDisplayView*)pInfo1->dwRefData)->name;
		if(pInfo2->dwRefData)
			strItem2=((CDisplayView*)pInfo2->dwRefData)->name;
	}
	else if(pInfo1->itemType==VIEW_DEFINEDFOLDER)
	{	//������ͼ��
		if(pInfo1->dwRefData)
			strItem1=((CDisplayView*)pInfo1->dwRefData)->folderName;
		if(pInfo2->dwRefData)
			strItem2=((CDisplayView*)pInfo2->dwRefData)->folderName;
	}
	int nRetCode=CompareMultiSectionString(strItem1.GetBuffer(),strItem2.GetBuffer());
	return nRetCode;
}

void CTowerTreeDlg::SortItem(HTREEITEM hSelItem)
{	//���ûص�������ָ���ڵ��ӽڵ����� wht 11-01-13
	if(hSelItem==NULL)
		return;
	TVSORTCB tvs;
	tvs.hParent = hSelItem;
	tvs.lpfnCompare = CompareTreeItem;
	tvs.lParam = (LPARAM)&m_treeCtrl;
	m_treeCtrl.SortChildrenCB(&tvs);
}

void CTowerTreeDlg::OnSortItem() 
{		
	HTREEITEM hSelItem=m_treeCtrl.GetSelectedItem();
	SortItem(hSelItem);
}
static void SetObjAuthorize(THANDLE hObjHandle,DWORD &dwObjPermission)
{
	CPartAuthPermDlg permDlg;
	permDlg.m_dwPermission=dwObjPermission;
	theApp.GetUserInfo(permDlg.user_arr);
	if(theApp.IsHasModifyPerm(dwObjPermission))
		permDlg.m_bEnableModify=TRUE;
	else
		permDlg.m_bEnableModify=FALSE;
	if(permDlg.DoModal()==IDOK&&theApp.IsHasModifyPerm(dwObjPermission))
	{
		DWORD dwModifyUsersWord=dwObjPermission|permDlg.m_dwPermission;
		dwObjPermission=permDlg.m_dwPermission;
		CBuffer buffer;
		dwObjPermission=permDlg.m_dwPermission;
		buffer.WriteInteger(1);
		buffer.WriteInteger(hObjHandle);
		buffer.WriteDword(dwObjPermission);
		buffer.WriteDword(dwObjPermission);
		buffer.WriteDword(dwObjPermission);
		DWORD dwWritten;
		LDS_MSGHEAD msg_head;
		msg_head.command_id=ASK_PERM_MODIFY_IN;
		msg_head.src_code=(BYTE)theApp.m_nUserId;
		msg_head.msg_length=0;
		WriteFile(theApp.m_hPipeServerListen,&msg_head,9,&dwWritten,NULL);
		BYTE memory_pool[PACK_SIZE];
		//д����Ϣ�����ݲ�����
		buffer.SeekToBegin();
		while(buffer.GetRemnantSize()>PACK_SIZE-1)
		{
			buffer.Read(&memory_pool,PACK_SIZE-1);
			memory_pool[PACK_SIZE-1]=1;	//�������
			WriteFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwWritten,NULL);
		}
		buffer.Read(memory_pool,buffer.GetRemnantSize());
		memory_pool[PACK_SIZE-1]=0;		//���ݽ���
		WriteFile(theApp.m_hPipeServerListen,memory_pool,PACK_SIZE,&dwWritten,NULL);
	}
}


void CTowerTreeDlg::OnHandoverPermission() 
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();		//Ŀǰ�ڹ�����һ��
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	/*
		һ�����������Ѿ��޸ģ����޸�Ȩ��ǰ����ǿ����ǰ�����ݵ�����������������һ�ڵ������ɣ�������һ�˼���
		Ȼ�󽫽ڵ�Ȩ��ת������û�����ʱ��ǰ�����޴˽ڵ�Ȩ�ޣ�����ʱ�˽ڵ�Ҳ����浽Эͬ���ģ���˾ͳ�Ϊ������
	*/
	if(Ta.IsDataModified())
	{
		BeginWaitCursor();
		theApp.GetLDSDoc()->SaveDataFile();
		Ta.SetDocModifiedFlag(FALSE);
		theApp.GetLDSDoc()->SetModifiedFlag(FALSE);
		EndWaitCursor();
	}
	if(pItemInfo->itemType==VIEW_CASE)
	{	//ɾ����ͼ
		for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
		{
			if(pView==(void*)pItemInfo->dwRefData)
			{
				SetObjAuthorize(pView->handle,pView->dwPermission);
				break;
			}
		}
	}
	else if	(pItemInfo->itemType==MODEL_DATUM_PLANE_CASE)
	{
		for(CLDSPlane *pPlane=Ta.Plane.GetFirst();pPlane;pPlane=Ta.Plane.GetNext())
		{
			if(pPlane==(void*)pItemInfo->dwRefData)
			{
				SetObjAuthorize(pPlane->handle,pPlane->dwPermission);
				break;
			}
		}
	}
	else if (pItemInfo->itemType==MODEL_DATUM_LINE_CASE)
	{
		for(CLDSLine *pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
		{
			if(pLine==(void*)pItemInfo->dwRefData)
			{
				SetObjAuthorize(pLine->handle,pLine->dwPermission);
				break;
			}
		}
	}
	else if (pItemInfo->itemType==MODEL_DATUM_POINT_CASE)
	{
		for(CLDSPoint *pPoint=Ta.Point.GetFirst();pPoint;pPoint=Ta.Point.GetNext())
		{
			if(pPoint==(void*)pItemInfo->dwRefData)
			{
				SetObjAuthorize(pPoint->handle,pPoint->dwPermission);
				break;
			}
		}
	}
	else if (pItemInfo->itemType==MODEL_ARCLIFT_CASE)
	{
		for(CLDSArcLift *pArcLift=Ta.ArcLift.GetFirst();pArcLift;pArcLift=Ta.ArcLift.GetNext())
		{
			if(pArcLift==(void*)pItemInfo->dwRefData)
			{
				SetObjAuthorize(pArcLift->handle,pArcLift->dwPermission);
				break;
			}
		}
	}
	else if(pItemInfo->itemType==BLOCK_CASE)
	{
		for(CBlockModel *pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
		{
			if(pBlock==(void*)pItemInfo->dwRefData)
			{
				SetObjAuthorize(pBlock->handle,pBlock->dwPermission);
				break;
			}
		}
	}
	else if (pItemInfo->itemType==BLOCKREF_CASE)
	{
		for(CBlockReference *pBlockRef=Ta.BlockRef.GetFirst();pBlockRef;pBlockRef=Ta.BlockRef.GetNext())
		{
			if(pBlockRef==(void*)pItemInfo->dwRefData)
			{
				SetObjAuthorize(pBlockRef->handle,pBlockRef->dwPermission);
				break;
			}
		}
	}
}
void CTowerTreeDlg::OnCalWindBetaZ() 
{
#if defined(__LDS_)||defined(__TSA_)
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO* pInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pInfo==NULL || pInfo->itemType != WINDSEG_GROUP)
		return;
	if (Ta.WindLoadSeg.GetNodeNum() == 0)
	{
		AfxMessageBox("ȱ�ٷ�ѹ������!");
		return;
	}
	CBatchCalWindSegBetaZDlg batchcaldlg;
	batchcaldlg.m_fBetaZCalWindV = 0;	//ɸѡ������
	for (CWorkStatus *pStatus = Ta.WorkStatus.GetFirst(); pStatus; pStatus = Ta.WorkStatus.GetNext())
		batchcaldlg.m_fBetaZCalWindV = max(batchcaldlg.m_fBetaZCalWindV, pStatus->WindLoad.velocity);
	//������������
	char file_path[MAX_PATH];
	theApp.GetFileName(file_path);
	strcat(file_path, ".frq");
	//_snprintf(file_path,MAX_PATH-1,"%sfreqs.frq",theApp.work_path);
	FILE* fp = fopen(file_path, "rb");
	CSelfCloseFile selflife(fp);	//�Զ��ر��ļ�ָ��
	long i = 0;
	double frequency = Ta.ReadVibraMode(fp, 0, true);
	if (frequency <= 0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Didn't find vibration mode calculation results file!");
#else
		AfxMessageBox("û���ҵ����ͼ������ļ���");
#endif
		return;
	}
	batchcaldlg.modes_arr = Ta.EarthQuake.modes_arr;
	batchcaldlg.m_nOrderNum = Ta.EarthQuake.nFreqsRank;
	if(batchcaldlg.DoModal()!=IDOK)
		return;
	CLogErrorLife loglife(&logerr);
	if(batchcaldlg.m_iBatchCalBetaZStyle==2)
	{
		SCOPE_STRU scope;
		Ta.GetTaSizeScope(scope);
		for(CWindSegment *pWindSeg=Ta.WindLoadSeg.GetFirst();pWindSeg;pWindSeg=Ta.WindLoadSeg.GetNext())
		{	//���ݷ�ѹ�θ߲�ֵȡֵ
			double highz=(scope.fMaxZ-pWindSeg->nodePtrArr[0]->Position(false).z)/1000;
			pWindSeg->BetaZ=g_sysPara.Beta_z_InterpByHigh(highz,false);
			pWindSeg->BetaZForFoundation=g_sysPara.Beta_z_InterpByHigh(highz,true);
		}
		return;
	}
	double dfMaxHeight = 0;
	CLDSModule *pModule,*pHighestModule =NULL;
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
	{
		double height= pModule->GetModuleScopeZ();
		if (pHighestModule == NULL)
		{
			pHighestModule = pModule;
			dfMaxHeight = height;
		}
		else if (height > dfMaxHeight)
		{
			pHighestModule = pModule;
			dfMaxHeight = height;
		}
	}
	pModule=pHighestModule;
	MODULE_LEG multilegs[24];

	int nLegCount=pModule->UpdateValidLegs(multilegs,24);

	//����һ�����ͺ�����������Զ��������ѹ������ϵ��
	frequency = Ta.ReadVibraMode(fp, batchcaldlg.m_iSelOrder);
	selflife.EarlyCloseFile();
	//�������ϵ��ʱ������ߺ��ߣ����������
	int nMaxLegs=CFGLEG::MaxLegs();
	pModule->m_arrActiveQuadLegNo[0] = pModule->m_arrActiveQuadLegNo[1] =
		pModule->m_arrActiveQuadLegNo[2] = pModule->m_arrActiveQuadLegNo[3] = nMaxLegs*(pModule->idBodyNo-1)+multilegs[0].uiLegSerial;

	CWindSegment arrWindSegs[8];
	ARRAY_LIST<CWindSegment> xarrWindSegs(arrWindSegs, 8);
	for (CWindSegment* pWindSeg = Ta.WindLoadSeg.GetFirst(); pWindSeg; pWindSeg = Ta.WindLoadSeg.GetNext())
	{
		if (pWindSeg->cCalStyle >= 0 && pWindSeg->cMirType > 0)
		{
			CWindSegment* pMirWindSeg = xarrWindSegs.append();
			if (pWindSeg->MirToWindSeg(pMirWindSeg, pWindSeg->cMirType, &Ta))
				pWindSeg->pMirArmWindSeg = pMirWindSeg;
			else
				xarrWindSegs.SetSize(xarrWindSegs.Size() - 1);
		}
	}

	CWindSegDispatcher dispatcher;
	WINDCALSEG::CURR_VIBR_FAI_TYPE = WINDCALSEG::FAI_TYPE_X;	//Ӧ���ݽ����������û�ѡ��
	if (!preprocessor.CalTowerWindSegBetaZ(&Ta, pModule, batchcaldlg.m_fBetaZCalWindV,frequency, &dispatcher))
	{
		AfxMessageBox("����ϵ������ʧ��");
		return;
	}
	//�������ܷ���ϵ��������
	char* pchDot = SearchChar(file_path, '.', true);
	char pszExt[8] = "@X.btz";
	if (WINDCALSEG::CURR_VIBR_FAI_TYPE == WINDCALSEG::FAI_TYPE_Y)
		strcpy(pszExt, "@Y.btz");
	if (pchDot)
		strcpy(pchDot, pszExt);
	else
		strcat(file_path, pszExt);
	if((fp = fopen(file_path, "wt"))!=NULL)
	{
		CSelfCloseFile selffile(fp);
		dispatcher.CreateBetaZCalReport(fp, pModule, frequency, batchcaldlg.m_fBetaZCalWindV, &Ta);
		selffile.EarlyCloseFile();
		WinExec(CXhChar200("notepad.exe %s", file_path), SW_SHOW);
	}
#endif
}
void DeleteSubTreeItems(CTreeCtrl *pTreeCtrl,HTREEITEM hItem)
{
	HTREEITEM hSubItem=pTreeCtrl->GetNextItem(hItem,TVGN_CHILD);
	ATOM_LIST<HTREEITEM> listItems;
	while(hSubItem!=NULL)
	{
		listItems.append(hSubItem);
		hSubItem=pTreeCtrl->GetNextSiblingItem(hSubItem);
	}
	for(HTREEITEM *pItem=listItems.GetFirst();pItem;pItem=listItems.GetNext())
		pTreeCtrl->DeleteItem(*pItem);
}
void CTowerTreeDlg::OnAttachWindload() 
{
#if defined(__LDS_)||defined(__TSA_)
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	HTREEITEM hParent=pTreeCtrl->GetParentItem(hItem);
	TREEITEM_INFO* pInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParent);
	if(pInfo==0)
		return;
	CWorkStatus *pStatus=(CWorkStatus*)pInfo->dwRefData;
	if(pStatus)
	{
		CSelAttachWindLoadDlg SelLoadDlg;
		for(CWorkStatus::WINDLOAD_REFPARAM *pRefParam=pStatus->AttachWindLoad.GetFirst();pRefParam;
		pRefParam=pStatus->AttachWindLoad.GetNext())
		SelLoadDlg.hashIncWindLoads.SetValue(pStatus->AttachWindLoad.GetCursorKey(),pStatus->AttachWindLoad.GetCursorKey());
		if(SelLoadDlg.DoModal()==IDOK)
		{
			CString sItem;
			DeleteSubTreeItems(pTreeCtrl,hItem);//���ԭ���ӷ����

			pStatus->AttachWindLoad.Empty();
			for(long *pH=SelLoadDlg.hashIncWindLoads.GetFirst();pH;pH=SelLoadDlg.hashIncWindLoads.GetNext())
			{
				CWorkStatus::WINDLOAD_REFPARAM *pWindLoadRef=pStatus->AttachWindLoad.Add(*pH);
				CAttachWindLoad *pLoad=Ta.AttachWindLoad.FromHandle(*pH);
				if(pLoad!=NULL)
				{
					sItem.Format("%s(%d)",pLoad->name,pLoad->iNo);
					HTREEITEM hSonItem=pTreeCtrl->InsertItem(sItem,PRJ_IMG_ATTACH_WINDLOAD,PRJ_IMG_ATTACH_WINDLOAD,hItem);
					TREEITEM_INFO *pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_ATTACH_WINDLOADREFITEM,(DWORD)pLoad));
					pTreeCtrl->SetItemData(hSonItem,(DWORD)pNewInfo);
				}
			}
		}
	}
#endif
}

void CTowerTreeDlg::OnIntelliCorrectModelObjsLayer()
{
#ifdef __RAPID_ROUGH_3DMODEL_
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	pLDSView->IntelliCorrectModelObjsLayer();
#endif
}
void CTowerTreeDlg::OnUnifyAngleBoltsParamG()
{
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	pLDSView->UnifyAngleBoltsParamG();
}
void CTowerTreeDlg::OnRelayoutRodModel()
{
#ifdef __SMART_DRAW_INC_
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	pLDSView->RelayoutRodModel();
#endif
}
static int CompareModuleHeight(const MODULE_HEIGHT& height1,const MODULE_HEIGHT& height2)
{
	if(!height1.lowestZ.IsInited()&&height2.lowestZ.IsInited())
		return  1;
	else if(height1.lowestZ.IsInited()&&!height2.lowestZ.IsInited())
		return -1;
	else if(!height1.lowestZ.IsInited()&&!height2.lowestZ.IsInited())
		return 0;
	if(height1.lowestZ.number>height2.lowestZ.number)
		return  1;
	else if(height1.lowestZ.number<height2.lowestZ.number)
		return -1;
	else
		return 0;
}
static int CompareLegHeight(const LEG_HEIGHT& leg1,const LEG_HEIGHT& leg2)
{
	if(!leg1.lowestZ.IsInited()&&leg2.lowestZ.IsInited())
		return  1;
	else if(leg1.lowestZ.IsInited()&&!leg2.lowestZ.IsInited())
		return -1;
	else if(!leg1.lowestZ.IsInited()&&!leg2.lowestZ.IsInited())
		return 0;
	if(leg1.lowestZ.number>leg2.lowestZ.number)
		return -1;
	else if(leg1.lowestZ.number<leg2.lowestZ.number)
		return  1;
	else
		return 0;
}
bool ResetModuleNameByHeight()
{
	CLogErrorLife life(&logerr);
	CLDSModule* pModule;
	ARRAY_LIST<MODULE_HEIGHT> heightsArr(0,Ta.Module.GetNodeNum());
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		MODULE_HEIGHT height(pModule);
		height.bodyword.AddBodyLegs(pModule->GetBodyNo());
		heightsArr.append(height);
	}
	int i;
	double lowestWireNodeZ=Ta.NamedHeightZeroZ;
	CLdsPartListStack stack(&Ta);
	//����ǰ���ݽڵ��жϸ�Ϊ���ݸ˼��˵��жϸ�׼ȷһЩ�����Ա���һЩ�����ڵ㵼�µ����� wjh-2019.4.25
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(/*!pRod->IsLegObj()||*/pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart)
			continue;
		if( pRod->pStart->xOriginalPos.x<0||pRod->pEnd->xOriginalPos.x<0||
			pRod->pStart->xOriginalPos.y<0||pRod->pEnd->xOriginalPos.y<0)
			continue;	//�˼����ڵ�һ����
		SmartRodPtr pSmartRod=pRod;
		if(pSmartRod->IsCompositeSonAngle())//>GetClassTypeId()==CLS_LINEANGLE&&pSmartRod.pAngle->group_father_jg_h>0x20)
			continue;
		double fLowRodZ=max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
		for(i=0;i<heightsArr.GetSize();i++)
		{
			if(heightsArr[i].bodyword.And(pRod->cfgword))
				heightsArr[i].lowestZ.Update(fLowRodZ);
		}
	}
	for(pModule=Ta.Module.GetFirst(),i=0;pModule;pModule=Ta.Module.GetNext(),i++)
	{
		if(lowestWireNodeZ<1000 && !Ta.m_bUserSpecifyNamedHeightZeroZ)
			logerr.Log("%s������ĺ��Ƹ߼�����ʼ��Zֵ<1000,�����ϳ���",pModule->description);
		double height=fto_halfi((heightsArr[i].lowestZ-lowestWireNodeZ)*0.001);
		CXhChar50 heightname(height);
		heightname.Append('m');
		StrCopy(pModule->description,heightname,51);
	}
	return true;
}
void CTowerTreeDlg::OnSetModuleNameByHeight() 
{
	if (ResetModuleNameByHeight())
		RefreshTreeCtrl();
}
static CFGWORD MappingLegWord(CFGWORD cfgword,const int* fromLegSerialArr,const int* toLegSerialArr,int nLegs,int iCurrBodyNo/*=1*/)
{
	if(!cfgword.IsHasBodyNo(iCurrBodyNo))
		return cfgword;
	PRESET_ARRAY32<BYTE> mapping;
	int i;
	for(i=1;i<32;i++)
		mapping.Set(i,i);
	for(i=0;i<nLegs;i++)
		mapping.Set(fromLegSerialArr[i],toLegSerialArr[i]);
	CFGLEG legcfg=cfgword.ToCfgLeg();
	DWORD legword=0;
	for(i=1;i<=CFGLEG::MaxLegs();i++)
	{
		if(legcfg.IsHasNo(iCurrBodyNo,i,true))
		{
			BYTE ciMappingLegSerial=mapping.At(i);
			legword|=GetSingleWord(ciMappingLegSerial);
		}
	}
	CFGWORD dest;
	dest.AddBodyLegs(iCurrBodyNo,legword);
	return dest;
}
//����ֵ��True,��ʾ�������仯��False,��ʾ����δ�����仯
bool SortModuleByHeight()
{
	bool order_modified=false;
	CLDSModule* pModule;
	ARRAY_LIST<MODULE_HEIGHT> heightsArr(0,Ta.Module.GetNodeNum());
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		MODULE_HEIGHT height(pModule);
		height.bodyword.AddBodyLegs(pModule->GetBodyNo());
		heightsArr.append(height);
	}
	CLDSNode* pNode;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		for(int i=0;i<heightsArr.GetSize();i++)
		{
			if(heightsArr[i].bodyword.And(pNode->cfgword))
				heightsArr[i].lowestZ.Update(pNode->Position('Z',false));
		}
	}
	CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	pGroup->EmptyNo();
	CBubbleSort<MODULE_HEIGHT>::BubSort(heightsArr.m_pData,heightsArr.GetSize(),CompareModuleHeight);
	DYN_ARRAY<int> fromHeightArr(heightsArr.GetSize()), toHeightArr(heightsArr.GetSize());
	bool modified=false;
	for(int j=0;j<heightsArr.GetSize();j++)
	{
		fromHeightArr[j]=heightsArr[j].m_pModule->GetBodyNo();
		toHeightArr[j]=j+1;
		if(fromHeightArr[j]!=toHeightArr[j])
			modified=true;
		heightsArr[j].m_pModule->iNo=j+1;
		pGroup->SetNoState(j+1);
	}
	order_modified|=modified;
	if(modified)
		Ta.ReplaceObjBodyNo(fromHeightArr,toHeightArr,heightsArr.GetSize());
	//else	//��������ǰ��δ�����仯
	for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
	{
		LEG_HEIGHT lowestLegZ[32];
		int i,iBodyNo=pModule->GetBodyNo();
		for(i=1;i<=CFGLEG::MaxLegs();i++)
			lowestLegZ[i].legserial=i;
		//����ǰ���ݽڵ��жϸ�Ϊ���ݸ˼��˵��жϸ�׼ȷһЩ�����Ա���һЩ�����ڵ㵼�µ����� wjh-2019.4.25
		for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(/*!pRod->IsLegObj()||*/pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart)
				continue;
			if(!pModule->IsSonPart(pRod,NULL,false))
				continue;
			SmartRodPtr pSmartRod=pRod;
			if(pSmartRod->IsCompositeSonAngle())//>GetClassTypeId()==CLS_LINEANGLE&&pSmartRod.pAngle->group_father_jg_h>0x20)
				continue;
			double fLowRodZ=max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
			CFGLEG cfgleg=pRod->cfgword.ToCfgLeg();
			for(int i=1;i<=CFGLEG::MaxLegs();i++)
			{
				if(!cfgleg.IsHasNo(pModule->GetBodyNo(),i,true))
					continue;
				lowestLegZ[i].lowestZ.Update(fLowRodZ);
			}
		}
		CBubbleSort<LEG_HEIGHT>::BubSort(lowestLegZ,32,CompareLegHeight);
		PRESET_ARRAY32<int> initlegs,sortedlegs;
		modified=false;
		for(i=0;i<32;i++)
		{
			if(!lowestLegZ[i].lowestZ.IsInited())
				break;
			initlegs.Set(i,lowestLegZ[i].legserial);
			sortedlegs.Set(i,i+1);
			if(lowestLegZ[i].legserial!=i+1)
				modified=true;
		}
		if(!modified)
			continue;	//��ǰ���ߵĽ�������ǰ��δ�����仯
		order_modified|=modified;
		for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(!pNode->IsLegObj()||!pModule->IsSonPart(pNode,NULL,false))
				continue;	//�˴��������pNode->IsLegObj()����pNode->cfgword=MappingLegWord�ᵼ����������ĺŴ��� wjh-2019.5.20
			pNode->cfgword=MappingLegWord(pNode->cfgword,initlegs.pPresetObjs,sortedlegs.pPresetObjs,initlegs.Count,iBodyNo);
			pNode->SetModified(true,false);
		}
		for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(!pPart->IsLegObj()||!pModule->IsSonPart(pPart,NULL,false))
				continue;	//�˴��������pNode->IsLegObj()����pNode->cfgword=MappingLegWord�ᵼ����������ĺŴ��� wjh-2019.5.20
			pPart->cfgword=MappingLegWord(pPart->cfgword,initlegs.pPresetObjs,sortedlegs.pPresetObjs,initlegs.Count,iBodyNo);
			pPart->SetModified(true,false);
		}
#ifdef __COMMON_PART_INC_
		for(CSupplementPart* pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
		{
			if(!pSuppPart->IsLegObj())
				continue;
			pSuppPart->cfgword=MappingLegWord(pSuppPart->cfgword,initlegs.pPresetObjs,sortedlegs.pPresetObjs,initlegs.Count,iBodyNo);
			pSuppPart->SetModified(true,false);
		}
#endif
	}
	return order_modified;
}
void CTowerTreeDlg::OnSortModuleByHeight() 
{
	if(SortModuleByHeight())
		RefreshTreeCtrl();
}
void CTowerTreeDlg::OnModelDesignCal() 
{
#if defined(__LDS_)||defined(__TSA_)
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	if(hItem==NULL)
		return;
	TREEITEM_INFO* pInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pInfo==NULL)
		return;
	CLDSModule *pModule=(CLDSModule*)pInfo->dwRefData;
	if(pModule)
		((CMainFrame*)AfxGetMainWnd())->ModelDesignCal(pModule->iNo);
#endif
}
void CTowerTreeDlg::OnModelEqCheck()
{
#if defined(__LDS_)||defined(__TSA_)
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	if(hItem==NULL)
		return;
	TREEITEM_INFO* pInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pInfo==NULL)
		return;
	CLDSModule *pModule=(CLDSModule*)pInfo->dwRefData;
	if(pModule)
		((CMainFrame*)AfxGetMainWnd())->ModelDesignCal(pModule->iNo,true);
#endif
}

void CTowerTreeDlg::OnItemListProp() 
{
#if defined(__LDS_)||defined(__TSA_)
	CWorkStatusListDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		CTreeCtrl *pTreeCtrl = GetTreeCtrl();
		HTREEITEM hParentItem=pTreeCtrl->GetSelectedItem();
		TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		if(pItemInfo==NULL)
			return;
		for(CWorkStatus *pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			//���ȸ����Ѵ��ڵĹ���
			HTREEITEM hItem=pTreeCtrl->GetChildItem(hParentItem);
			while(hItem)
			{	
				TREEITEM_INFO *pSonItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
				if(pSonItemInfo&&pSonItemInfo->itemType==WORKSTATUS_CASE&&
					(CWorkStatus*)(pSonItemInfo->dwRefData)==pStatus)
					break;
				hItem=pTreeCtrl->GetNextSiblingItem(hItem);
			}
			CString ss;
			if(strlen(pStatus->description)>0)
				ss.Format("%s(%d)",pStatus->description,pStatus->iNo);
			else
				ss.Format("%d(%d)",pStatus->iNo,pStatus->iNo);
			if(hItem)
			{	//�����Ѵ��ڹ���
				pTreeCtrl->SetItemText(hItem,ss);
				HTREEITEM hSonItem=pTreeCtrl->GetChildItem(hItem);
				while(hSonItem)
				{	
					TREEITEM_INFO *pSonItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSonItem);
					if(pSonItemInfo&&pSonItemInfo->itemType==WORKSTATUS_ICE_THICK)
					{	//���¸������
						ss.Format("�����������=%.2fmm",pStatus->ice_thick);
						pTreeCtrl->SetItemText(hSonItem,ss);
					}
					else if(pSonItemInfo&&pSonItemInfo->itemType==WORKSTATUS_WINDLOAD)
					{	//���·����Լ������
						HTREEITEM hLeafItem=pTreeCtrl->GetChildItem(hSonItem);
						ss.Format("����=%f",pStatus->WindLoad.velocity);
						SimplifiedNumString(ss);
						ss+="m/s";
						pTreeCtrl->SetItemText(hLeafItem,ss);
						hLeafItem=pTreeCtrl->GetNextSiblingItem(hLeafItem);
						ss.Format("�����=%d��",pStatus->WindLoad.azimuth);
						pTreeCtrl->SetItemText(hLeafItem,ss);
					}
					hSonItem=pTreeCtrl->GetNextSiblingItem(hSonItem);
				}
			}
			else
			{
				hItem=pTreeCtrl->InsertItem(ss,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hParentItem);
				TREEITEM_INFO* pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_CASE,(DWORD)pStatus));
				pTreeCtrl->SetItemData(hItem,(DWORD)pNewInfo);
				ss.Format("�ɱ�������ϵ��=%.2f",pStatus->Kesai());
				pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				ss.Format("���ú����鷶Χ=%s",pStatus->limit_str);
				pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				ss.Format("�����������=%.2fmm",pStatus->ice_thick);
				pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hItem);
				HTREEITEM hPointLoadItem=pTreeCtrl->InsertItem("���к���",PRJ_IMG_POINTLOAD,PRJ_IMG_POINTLOAD,hItem);
				pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_POINTLOAD,0));
				pTreeCtrl->SetItemData(hPointLoadItem,(DWORD)pNewInfo);
				HTREEITEM hWindItem=pTreeCtrl->InsertItem("�����",PRJ_IMG_WINDLOAD,PRJ_IMG_WINDLOAD,hItem);
				pNewInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_WINDLOAD,0));
				ss.Format("����=%f",pStatus->WindLoad.velocity);
				SimplifiedNumString(ss);
				ss+="m/s";
				pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hWindItem);
				ss.Format("�����=%d��",pStatus->WindLoad.azimuth);
				pTreeCtrl->InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hWindItem);
				ss.Format("���Ӻ���");
				hWindItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_ATTACH_WINDLOAD,PRJ_IMG_ATTACH_WINDLOAD,hItem);
				pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_ATTACH_WINDLOADREF,0));
				pTreeCtrl->SetItemData(hWindItem,(DWORD)pNewInfo);
			}
		}
		if(Ta.WorkStatus.GetNodeNum()>0)
		{
			int i=0;
			HTREEITEM hItem=pTreeCtrl->GetChildItem(hParentItem);	//��õ�һ�������ڵ�
			while(hItem)
			{
				if(i==0)
					pTreeCtrl->SetItemState(hItem,TVIS_BOLD,TVIS_BOLD);
				else
					pTreeCtrl->SetItemState(hItem,0,TVIS_BOLD);
				hItem=pTreeCtrl->GetNextSiblingItem(hItem);
				i++;
			}
			Ta.m_hActiveStatus=Ta.WorkStatus.GetFirst()->handle;	//���õ�һ������Ϊ����״̬
			CMainFrame *pMainFrm=(CMainFrame*)AfxGetApp()->m_pMainWnd;
			//if(pMainFrm->m_bDisplayDeformedTower)//Ϊ�˸ı���ʾ���ؼ�ͷ����ҲҪ����һ��
			{
				CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
				pLdsView->RedrawAll();
			}
			theApp.GetLDSDoc()->SetModifiedFlag();
		}
	}
#endif
}

/*���ڸ��ĺ��ع��߷�����WireLoad.exe���˹��ܸо����岻����ʱ�رգ���������Ҫ�ٸĽ�CNodeLoadListDlg
void CTowerTreeDlg::OnNodeLoadList() 
{
#if defined(__LDS_)||defined(__TSA_)
	CNodeLoadListDlg dlg;
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=WIRELAYOUT_CASE||pItemInfo->dwRefData==0)
		return;
	CWireLayout *pWireLayout=(CWireLayout*)pItemInfo->dwRefData;
	dlg.m_iWireLayoutNo=pWireLayout->iNo;
	if (IDOK == dlg.DoModal())
	{
		CTreeCtrl *pTreeCtrl = GetTreeCtrl();
		HTREEITEM hParentItem=pTreeCtrl->GetSelectedItem();
		HTREEITEM hItem=pTreeCtrl->GetChildItem(hParentItem);
		while (hItem)
		{	//��������
			TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
			if(pItemInfo==NULL||(pItemInfo&&pItemInfo->itemType!=WORKSTATUS_CASE))
			{
				hItem = pTreeCtrl->GetNextSiblingItem(hItem);
				continue;
			}
			CWorkStatus *pCurWorkStatus=NULL;
			pCurWorkStatus=(CWorkStatus*)pItemInfo->dwRefData;
			if(pCurWorkStatus==NULL)
			{
				hItem = pTreeCtrl->GetNextSiblingItem(hItem);
				continue;
			}
			//���Ҽ��к��ض�Ӧ�ڵ�
			HTREEITEM hSonItem=pTreeCtrl->GetChildItem(hItem);
			while (hSonItem)
			{	
				TREEITEM_INFO *pSonItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSonItem);
				if(pSonItemInfo==NULL||(pSonItemInfo&&WORKSTATUS_POINTLOAD!=pSonItemInfo->itemType))
				{
					hSonItem = pTreeCtrl->GetNextSiblingItem(hSonItem);
					continue;
				}
				//
				HTREEITEM hSon2Item=pTreeCtrl->GetChildItem(hSonItem);
				while(hSon2Item)
				{	//ɾ����ǰ���ӽڵ�
					pTreeCtrl->DeleteItem(hSon2Item);
					hSon2Item=pTreeCtrl->GetChildItem(hSonItem);
				}
				for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=pNode=Ta.Node.GetNext())
				{
					CString ss;
					CExternalNodeLoad *pLoad=pCurWorkStatus->hashNodeLoad.GetValue(pNode->handle);
					if(pLoad==NULL||pLoad->Fxyz.mod2()<EPS)
						continue;	//����Ӻ���
					ss.Format("0X%X�ڵ�(%d)",pNode->handle,pNode->point_i);
					HTREEITEM hSonItem3=pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOAD,PRJ_IMG_POINTLOAD,hSonItem);
					pItemInfo = InsertOrUpdateItemInfo(TREEITEM_INFO(WORKSTATUS_POINTLOAD_CASE,(DWORD)pLoad));
					pTreeCtrl->SetItemData(hSonItem3,(DWORD)pItemInfo);
					ss.Format("X:%.2f",pLoad->Fxyz.x/1000);
					SimplifiedNumString(ss);
					ss+="(kN)";
					pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOADITEM,PRJ_IMG_POINTLOADITEM,hSonItem3);
					ss.Format("Y:%.2f",pLoad->Fxyz.y/1000);
					SimplifiedNumString(ss);
					ss+="(kN)";
					pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOADITEM,PRJ_IMG_POINTLOADITEM,hSonItem3);
					ss.Format("Z:%.2f",pLoad->Fxyz.z/1000);
					SimplifiedNumString(ss);
					ss+="(kN)";
					pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOADITEM,PRJ_IMG_POINTLOADITEM,hSonItem3);
					ss.Format("Z_2:%.2f",pLoad->permanentFz/1000);
					SimplifiedNumString(ss);
					ss+="(kN)";
					pTreeCtrl->InsertItem(ss,PRJ_IMG_POINTLOADITEM,PRJ_IMG_POINTLOADITEM,hSonItem3);
				}
				hSonItem = pTreeCtrl->GetNextSiblingItem(hSonItem);
			}
			hItem = pTreeCtrl->GetNextSiblingItem(hItem);
		}
	}
#endif
}

void CTowerTreeDlg::OnNewItemSet() 
{
#if defined(__LDS_)||defined(__TSA_)
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	HTREEITEM hSubItem;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==WIRELOAD_MODEL_GROUP)
	{
		static CNewClassicWireLoadDlg dlg;
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hItem);
		TREEITEM_INFO* pParentInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		CWireLayout *pWireLayout=(CWireLayout*)pParentInfo->dwRefData;

		if(dlg.DoModal()==IDOK)
		{
			if(dlg.m_bClearLegacy)
			{
				hSubItem=pTreeCtrl->GetNextItem(hItem,TVGN_CHILD);
				while(hSubItem!=NULL)
				{
					TREEITEM_INFO* pSubItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hSubItem);
					CWireLoadModel *pWireModel=(CWireLoadModel*)pSubItemInfo->dwRefData;
					pTreeCtrl->DeleteItem(hSubItem);
					pWireLayout->hashWireLoadModel.DeleteNode(pWireModel->iNo);
					hSubItem=pTreeCtrl->GetNextItem(hItem,TVGN_CHILD);
				}
			}
			CWireInfo daoxian,dixian;
			CWireLoadDatabase db;
			char file_path[MAX_PATH];
			GetSysPath(file_path,NULL,NULL,NULL);
			strcat(file_path,"wireload.d");
			if(db.OpenDbFile(file_path))
			{
				CXhChar50 wireType,spec;
				wireType.Printf("%s",dlg.m_sDaoXianType);
				spec.Printf("%s",dlg.m_sDaoXianSpec);
				db.QueryWire(wireType,spec,&daoxian.wire);
				wireType.Printf("%s",dlg.m_sDiXianType);
				spec.Printf("%s",dlg.m_sDiXianSpec);
				db.QueryWire(wireType,spec,&dixian.wire);
			}
			CString sText;
			long hNewKey=1,nLoadNum=0;
			CWireLoadModel *pWireModel;
			do
			{
				for(pWireModel=pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=pWireLayout->hashWireLoadModel.GetNext())
				{
					if(pWireModel->iNo/10==hNewKey)
					{
						hNewKey++;
						break;
					}
				}
			}while(pWireModel!=NULL);
			if(dlg.m_iCircuitNum==0)		//����·
				nLoadNum=2;
			else if(dlg.m_iCircuitNum==1)	//˫��·
				nLoadNum=4;
			else //if(dlg.m_iCircuitNum==2)	//�Ļ�·
				nLoadNum=7;
			CString sPrefix,sXianType;
			hNewKey=1;
			for(int i=0;i<nLoadNum;i++)
			{
				long iNo=hNewKey*10;
				if(!dlg.m_bClearLegacy)
				{
					do
					{
						for(pWireModel=pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=pWireLayout->hashWireLoadModel.GetNext())
						{
							if(pWireModel->iNo/10==hNewKey||pWireModel->iNo==iNo||pWireModel->iNo==iNo+1)
							{
								hNewKey++;
								break;
							}
						}
					}while(pWireModel!=NULL);
				}
				pWireModel=pWireLayout->hashWireLoadModel.Add(hNewKey*10);
				Ta.InitWeather(pWireModel);
				pWireModel->iNo=hNewKey*10;
				if(dlg.m_iGuaXianType==1)	//����
					sPrefix="ǰ��";
				else
					sPrefix="";
				if(i==0)
				{
					sXianType="����";
					pWireModel->wireInfo.m_cXianType='E';
				}
				else
				{
					sXianType.Format("%d���ർ��",i);
					pWireModel->wireInfo.m_cXianType='C';
					if(dlg.m_iSplitNum==0)
						pWireModel->wireInfo.m_nDaoXianFengLieNum=1;
					else if(dlg.m_iSplitNum==1)
						pWireModel->wireInfo.m_nDaoXianFengLieNum=2;
					else if(dlg.m_iSplitNum==2)
						pWireModel->wireInfo.m_nDaoXianFengLieNum=4;
					else
						pWireModel->wireInfo.m_nDaoXianFengLieNum=6;
				}
				if(dlg.m_iGuaXianType!=0)
				{
					pWireModel->wireInfo.m_bNaiZhang=true;
					pWireModel->wireInfo.m_fAchorLineAngle=45;
					pWireModel->wireInfo.m_fTractionLineAngle=20;
				}
				else
				{
					pWireModel->wireInfo.m_bNaiZhang=false;
					pWireModel->wireInfo.m_fAchorLineAngle=20;
					pWireModel->wireInfo.m_fTractionLineAngle=20;
				}
				if(Ta.m_cLineRoadType==0)	//ƽ����·
				{
					if(!pWireModel->wireInfo.m_bNaiZhang)
					{	//ֱ����
						pWireModel->wireInfo.xianlu.horizon_space_allotcoef=0.5;
						pWireModel->wireInfo.xianlu.vertical_space_allotcoef=0.5;
					}
					else
					{	//������
						pWireModel->wireInfo.xianlu.horizon_space_allotcoef=0.5;
						pWireModel->wireInfo.xianlu.vertical_space_allotcoef=0.7;
					}
				}
				else	//�����ɽ����·
				{
					if(!pWireModel->wireInfo.m_bNaiZhang)
					{	//ֱ����
						pWireModel->wireInfo.xianlu.horizon_space_allotcoef=0.5;
						pWireModel->wireInfo.xianlu.vertical_space_allotcoef=0.6;
					}
					else
					{	//������
						pWireModel->wireInfo.xianlu.horizon_space_allotcoef=0.5;
						pWireModel->wireInfo.xianlu.vertical_space_allotcoef=0.8;
					}
				}
				if(i==0)
				{
					pWireModel->wireInfo.wire=dixian.wire;
					_snprintf(pWireModel->wireInfo.wire.specWireType,50,"%s",dlg.m_sDiXianType);
					_snprintf(pWireModel->wireInfo.wire.specWire,50,"%s",dlg.m_sDiXianSpec);
				}
				else
				{
					pWireModel->wireInfo.wire=daoxian.wire;
					_snprintf(pWireModel->wireInfo.wire.specWireType,50,"%s",dlg.m_sDaoXianType);
					_snprintf(pWireModel->wireInfo.wire.specWire,50,"%s",dlg.m_sDaoXianSpec);
				}
				pWireModel->wireInfo.wire.m_fMaxUseTension=pWireModel->wireInfo.wire.m_fForcePermit/pWireModel->wireInfo.wire.m_fSafeCoef;
				pWireModel->wireInfo.m_nVoltGrade=dlg.m_nVoltage;
				pWireModel->wireInfo.xianlu.HtoL=dlg.m_fHtoL;
				pWireModel->wireInfo.xianlu.xianlu_angle=dlg.m_fXianluAngle*0.5;
				pWireModel->wireInfo.xianlu.horizon_space=dlg.m_fHoriSpace;
				pWireModel->wireInfo.xianlu.vertical_space=dlg.m_fVertSpace;
				pWireModel->wireInfo.xianlu.min_vert_space=dlg.m_fMinVertSpace;
				pWireModel->wireInfo.xianlu.represent_space=dlg.m_fXuanLianCalSpace;
				pWireModel->ResetWorkEnvLoadSet();
				pWireModel->CalWorkEnvLoad(Ta.m_cLandscape,Ta.m_cLineRoadType);
				sText.Format("%s%s(%d)",sPrefix,sXianType,pWireModel->iNo);
				_snprintf(pWireModel->name,50,"%s%s",sPrefix,sXianType);
				hSubItem=pTreeCtrl->InsertItem(sText,PRJ_IMG_WIRELOAD,PRJ_IMG_WIRELOAD,hItem,TVI_LAST);
				TREEITEM_INFO* pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WIRELOAD_MODEL_CASE,(DWORD)pWireModel));
				pTreeCtrl->SetItemData(hSubItem,(DWORD)pNewInfo);
				if(dlg.m_iGuaXianType==1)	//����
				{
					sPrefix="���";
					pWireModel=pWireLayout->hashWireLoadModel.Add(hNewKey*10+2);
					Ta.InitWeather(pWireModel);
					pWireModel->iNo=hNewKey*10+2;
					if(i==0)
						pWireModel->wireInfo.m_cXianType='E';
					else
					{
						pWireModel->wireInfo.m_cXianType='C';
						if(dlg.m_iSplitNum==0)
							pWireModel->wireInfo.m_nDaoXianFengLieNum=1;
						else if(dlg.m_iSplitNum==1)
							pWireModel->wireInfo.m_nDaoXianFengLieNum=2;
						else
							pWireModel->wireInfo.m_nDaoXianFengLieNum=4;
					}
					pWireModel->wireInfo.m_bNaiZhang=true;
					pWireModel->wireInfo.m_fAchorLineAngle=45;
					pWireModel->wireInfo.m_fTractionLineAngle=20;
					if(Ta.m_cLineRoadType==0)	//ƽ����·
					{
						if(!pWireModel->wireInfo.m_bNaiZhang)
						{	//ֱ����
							pWireModel->wireInfo.xianlu.horizon_space_allotcoef=0.5;
							pWireModel->wireInfo.xianlu.vertical_space_allotcoef=0.5;
						}
						else
						{	//������
							pWireModel->wireInfo.xianlu.horizon_space_allotcoef=0.5;
							pWireModel->wireInfo.xianlu.vertical_space_allotcoef=0.3;
						}
					}
					else	//�����ɽ����·
					{
						if(!pWireModel->wireInfo.m_bNaiZhang)
						{	//ֱ����
							pWireModel->wireInfo.xianlu.horizon_space_allotcoef=0.5;
							pWireModel->wireInfo.xianlu.vertical_space_allotcoef=0.4;
						}
						else
						{	//������
							pWireModel->wireInfo.xianlu.horizon_space_allotcoef=0.5;
							pWireModel->wireInfo.xianlu.vertical_space_allotcoef=0.2;
						}
					}
					if(i==0)
					{
						pWireModel->wireInfo.wire=dixian.wire;
						_snprintf(pWireModel->wireInfo.wire.specWireType,50,"%s",dlg.m_sDiXianType);
						_snprintf(pWireModel->wireInfo.wire.specWire,50,"%s",dlg.m_sDiXianSpec);
					}
					else
					{
						pWireModel->wireInfo.wire=daoxian.wire;
						_snprintf(pWireModel->wireInfo.wire.specWireType,50,"%s",dlg.m_sDaoXianType);
						_snprintf(pWireModel->wireInfo.wire.specWire,50,"%s",dlg.m_sDaoXianSpec);
					}
					pWireModel->wireInfo.wire.m_fMaxUseTension=pWireModel->wireInfo.wire.m_fForcePermit/pWireModel->wireInfo.wire.m_fSafeCoef;
					pWireModel->wireInfo.m_nVoltGrade=dlg.m_nVoltage;
					pWireModel->wireInfo.xianlu.HtoL=dlg.m_fHtoL;
					pWireModel->wireInfo.xianlu.xianlu_angle=180-dlg.m_fXianluAngle*0.5;
					pWireModel->wireInfo.xianlu.horizon_space=dlg.m_fHoriSpace;
					pWireModel->wireInfo.xianlu.vertical_space=dlg.m_fVertSpace;
					pWireModel->wireInfo.xianlu.min_vert_space=dlg.m_fMinVertSpace;
					pWireModel->wireInfo.xianlu.represent_space=dlg.m_fXuanLianCalSpace;
					pWireModel->ResetWorkEnvLoadSet();
					pWireModel->CalWorkEnvLoad(Ta.m_cLandscape,Ta.m_cLineRoadType);
					sText.Format("%s%s(%d)",sPrefix,sXianType,pWireModel->iNo);
					_snprintf(pWireModel->name,50,"%s%s",sPrefix,sXianType);
					hSubItem=pTreeCtrl->InsertItem(sText,PRJ_IMG_WIRELOAD,PRJ_IMG_WIRELOAD,hItem,TVI_LAST);
					TREEITEM_INFO* pNewInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WIRELOAD_MODEL_CASE,(DWORD)pWireModel));
					pTreeCtrl->SetItemData(hSubItem,(DWORD)pNewInfo);
				}
				hNewKey++;
			}
		}
	}
	else if(pItemInfo->itemType==WORKSTATUS_GROUP)
	{
		static CTransformWorkStatusDlg dlg;
		HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hItem);
		TREEITEM_INFO* pParentInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hParentItem);
		dlg.m_pWireLayout=(CWireLayout*)pParentInfo->dwRefData;
		dlg.m_pTower=&Ta;
		if(dlg.DoModal()==IDOK)
		{
			Ta.SynchronizeWorkStatus(dlg.m_pWireLayout);
			RefreshTreeCtrl();
		}
	}

#endif
}

void CTowerTreeDlg::OnCalWireLoad() 
{
#if defined(__LDS_)||defined(__TSA_)
	char error_info[101];
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=WIRELAYOUT_CASE||pItemInfo->dwRefData==0)
		return;
	CWireLayout *pWireLayout=(CWireLayout*)pItemInfo->dwRefData;
	if(Ta.CalWireLoad(pWireLayout,error_info))
		RefreshTreeCtrl();
	else
		AfxMessageBox(error_info);
#endif
}
*/
#if defined(__LDS_)||defined(__TSA_)
static int compare_windnode_func(const WINDLOADNODE& node1,const WINDLOADNODE& node2)
{
	if(node1.nodeI>node2.nodeI)
		return 1;
	else if(node1.nodeI<node2.nodeI)
		return -1;
	else
		return 0;
}
#endif
#include "SnapTypeVerify.h"
static int LocalCompareNodePosZ(const CLDSNodePtr& pNode1,const CLDSNodePtr& pNode2)
{
	if(pNode1->Position('Z')>pNode2->Position('Z'))
		return 1;
	else if(pNode1->Position('Z')<pNode2->Position('Z'))
		return -1;
	else
		return 0;
}
struct TRUNK_WINDSEG{
	CLDSNode *pTop,*pBtm;
};
int DetectNextAdaptWindSegBtmNode(CHashSet<CLDSNodePtr>& hashSlopeNodes,
	CLDSNodePtr* slopenodes,int sumcount,int fromTopI,CLDSNodePtr* excludeNodeArr,int excludecount)
{	//̽����һ�����ʵ������ѹ�νڵ�
	int MIN_WINDSEG_HEIGHT =5500;
	CLDSNode* pTopNode=slopenodes[fromTopI];
	for(WORD to=fromTopI+1;to<sumcount;to++)
	{
		bool lowestTowerSegNode=true;
		if(to+1<sumcount&&slopenodes[to]->cfgword.IsEqual(slopenodes[to+1]->cfgword))
			lowestTowerSegNode=false;
		if(!lowestTowerSegNode&&slopenodes[to]->Position('Z')-pTopNode->Position('Z')<MIN_WINDSEG_HEIGHT&&to<sumcount-1)
			continue;	//������ͬ��ĺ������ѹ���е���ͽڵ�ʱ�����ٱ�֤��ѹ�β�����5.5m wjh-2017.2.16
		if(!pTopNode->ModuleTogetherWith(slopenodes[to]))
			continue;	//������ڵ�
		int k;
		for(k=0;k<excludecount;k++)
		{
			if(excludeNodeArr[k]==slopenodes[to]||excludeNodeArr[k]->ModuleTogetherWith(slopenodes[to]))
				break;
		}
		if(k<excludecount)
			continue;	//���ų��ڵ���ͬ���ص�������ͬһ��������
		for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if( pRod->pStart==NULL||hashSlopeNodes.GetValue(pRod->pStart->handle)==NULL||
				pRod->pEnd==NULL||hashSlopeNodes.GetValue(pRod->pEnd->handle)==NULL)
				continue;
			if(pRod->pStart==slopenodes[to]||pRod->pEnd==slopenodes[to])
				break;	//��ǰ��ѹ�εײ��ڵ�Ϊ���Ķ˵�
		}
		if(pRod==NULL)
			continue;	//��ǰ�յ�����Ķ˵㣬����
		return to;
	}
	return -1;
}
int AppendWindSegBranch(CXhSimpleList<TRUNK_WINDSEG>& listWindSegments,CHashSet<CLDSNodePtr>& hashSlopeNodes,
	CLDSNodePtr* slopenodes,int sumcount,int fromTopI,int toBtmI)
{
	int addcount=1;
	//��ӵ�ǰ��ѹ��
	TRUNK_WINDSEG* pNewWindSeg=listWindSegments.AttachObject();
	pNewWindSeg->pTop=slopenodes[fromTopI];
	pNewWindSeg->pBtm=slopenodes[toBtmI];
	//�̳�����������ѹ�η�֧
	int from=toBtmI,indexSegBtmNode=toBtmI+1;
	ARRAY_LIST<CLDSNodePtr> excludeNodesArr(0,4);
	do{
		if((indexSegBtmNode=DetectNextAdaptWindSegBtmNode(hashSlopeNodes,slopenodes,sumcount,
			from,excludeNodesArr.m_pData,excludeNodesArr.GetSize()))>0)
		{
			addcount+=AppendWindSegBranch(listWindSegments,hashSlopeNodes,slopenodes,sumcount,from,indexSegBtmNode);
			excludeNodesArr.append(slopenodes[indexSegBtmNode]);
		}
	}while(indexSegBtmNode>=from);
	return addcount;
}
void CTowerTreeDlg::OnRetrieveBodyWindSegments() 
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hGroupItem=pTreeCtrl->GetSelectedItem();
	HTREEITEM hParentItem=pTreeCtrl->GetParentItem(hGroupItem);
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hGroupItem);
	if(	pItemInfo==NULL||pItemInfo->itemType!=WINDSEG_GROUP||hParentItem!=m_hBodyItem)
		return;
#if defined(__LDS_)||defined(__TSA_)
	//1.��ȡ���������ѹ�β������ؼ���ɾ��
	HTREEITEM hWindSegItem=pTreeCtrl->GetNextItem(hGroupItem,TVGN_CHILD);
	bool hasLegacyBodyItem=false;
	CXhSimpleList<HTREEITEM>listBodyItems;
	while(hWindSegItem!=NULL)
	{
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hWindSegItem);
		if(pItemInfo==NULL||pItemInfo->itemType!=WINDSEG_CASE)
			continue;
		CWindSegment *pWindSeg=(CWindSegment*)pItemInfo->dwRefData;
		if(/*pWindSeg->cCalStyle==0&&*/pWindSeg->cType==0)	//����
		{
			listBodyItems.AttachObject(hWindSegItem);
			hasLegacyBodyItem=true;
		}
		hWindSegItem=pTreeCtrl->GetNextSiblingItem(hWindSegItem);
	}
	if(hasLegacyBodyItem&&AfxMessageBox("���ھ�����(��)��ѹ�����ݣ��������������ԭ������(��)��ѹ������",MB_YESNO)!=IDYES)
		return;
	// ɾ�������߽ڵ��µ��ӷ�ѹ�����е������ѹ��
	HTREEITEM hModuleItem=pTreeCtrl->GetNextItem(m_hBodyItem,TVGN_CHILD);
	CXhSimpleList<HTREEITEM>listModelCaseItems;
	do{
		TREEITEM_INFO* pModuleItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hModuleItem);
		if(pModuleItemInfo&&pModuleItemInfo->itemType==MODEL_CASE)
		{
			listModelCaseItems.AttachObject(hModuleItem);
			HTREEITEM hWindSegGroupItem=pTreeCtrl->GetNextItem(hModuleItem,TVGN_CHILD);
			while(hWindSegGroupItem!=NULL){
				TREEITEM_INFO* pSegGroupItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hWindSegGroupItem);
				if(pSegGroupItemInfo&&pSegGroupItemInfo->itemType==WINDSEG_GROUP)
				{
					hWindSegItem=pTreeCtrl->GetNextItem(hWindSegGroupItem,TVGN_CHILD);
					while(hWindSegItem!=NULL){
						TREEITEM_INFO* pWindSegItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hWindSegItem);
						if(pWindSegItemInfo&&pWindSegItemInfo->itemType==WINDSEG_CASE)
						{
							CWindSegment* pSegment=(CWindSegment*)pWindSegItemInfo->dwRefData;
							if(pSegment->cType==0)	//����
								listBodyItems.AttachObject(hWindSegItem);
						}
						hWindSegItem=pTreeCtrl->GetNextSiblingItem(hWindSegItem);
					}
				}
				hWindSegGroupItem=pTreeCtrl->GetNextSiblingItem(hWindSegGroupItem);
			}
		}
		hModuleItem=pTreeCtrl->GetNextSiblingItem(hModuleItem);
	}while(hModuleItem!=NULL);
	for(HTREEITEM* pDelItem=listBodyItems.EnumObjectFirst();pDelItem;pDelItem=listBodyItems.EnumObjectNext())
		pTreeCtrl->DeleteItem(*pDelItem);
	CObjNoGroup* pNoGroup=Ta.NoManager.FromGroupID(WINDSEG_GROUP);
	for(CWindSegment* pSeg=Ta.WindLoadSeg.GetFirst();pSeg;pSeg=Ta.WindLoadSeg.GetNext())
	{
		if(pSeg->cCalStyle==0&&pSeg->cType==0)
		{
			pNoGroup->SetNoState(pSeg->iNo,FALSE);
			Ta.WindLoadSeg.DeleteCursor();
		}
	}
	Ta.WindLoadSeg.Clean();
	//2.��ģ�����Զ���ȡ�����ѹ��
	//-----VVVVVVV-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	CCmdLockObject cmdlock(pLdsView);
	if(!cmdlock.LockSuccessed())
		return;
	g_pSolidDraw->ReleaseSnapStatus();
	CSysPromptObject prompt;
	//pCmdLine->FillCmdLine("��������������ѡ��1���������������¿ڼ������������ƽڵ�:","");
	prompt.Prompt("��������������ѡ��1���������������¿ڼ������������ƽڵ㡣ѡ����ϣ��س���ո��ȷ�Ͻ���!");
	pCmdLine->FillCmdLine("RetrieveTrunkWindSegments ��������������ѡ��1���������������¿ڼ������������ƽڵ�,�س���ո��ȷ�Ͻ���:","");
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prPoint);
	CLDSNode* pNode,*pSelNode;
	//CDisplayNodeAtFrontLife displayNode;
	//displayNode.DisplayNodeAtFront();
	NODESET nodeset;
	//PEEKCMDLINE cmd_detect("W|V|P",pCmdLine);
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		pSelNode= console.FromNodeHandle(dwhObj);
		if(pSelNode&&pSelNode->IsNode())
		{
			nodeset.append(pSelNode);
			g_pSolidDraw->SetEntSnapStatus(pSelNode->handle);
			pCmdLine->FinishCmdLine(CXhChar16("�ڵ�0x%X",pSelNode->handle));
		}
		if(obj.ciTriggerType==3)
		{
			pCmdLine->FinishCmdLine("*����*");
			break;	//�س���ȷ�Ͻ���
		}
		else
			pCmdLine->FillCmdLine("RetrieveTrunkWindSegments ��������������ѡ��1���������������¿ڼ������������ƽڵ�,�س���ո��ȷ�Ͻ���:","");
	}
	UINT i=0,k=0;
	DYN_ARRAY<CLDSNode*>ctrlnodes(nodeset.GetNodeNum());
	for(pSelNode=nodeset.GetFirst();pSelNode;pSelNode=nodeset.GetNext(),i++)
		ctrlnodes[i]=pSelNode;
	CBubbleSort<CLDSNodePtr>::BubSort(ctrlnodes,ctrlnodes.Size(),LocalCompareNodePosZ);
	//��ѹ�δ����������5.5m
	CXhSimpleList<TRUNK_WINDSEG>listWindSegments;
	for(i=0;i<ctrlnodes.Size()-1;i++)
	{
		CPtInLineCheck check(ctrlnodes[i]->Position(false),ctrlnodes[i+1]->Position(false));
		CHashSet<CLDSNodePtr> hashSlopeNodes;
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			bool online=false;
			if(pNode==ctrlnodes[i]||pNode==ctrlnodes[i+1])
				hashSlopeNodes.SetValue(pNode->handle,pNode);
			double scale2start=check.CheckPointEx(pNode->Position(false),&online);
			if(!online)
				continue;
			//���һ�ο������漰�����߻��ȣ��������¶����յ�
			if(i==ctrlnodes.Size()-2&&scale2start<0)
				continue;	//����
			else if(i<ctrlnodes.Size()-2&&(scale2start<0||scale2start>1))
				continue;	//����
			hashSlopeNodes.SetValue(pNode->handle,pNode);
		}
		DYN_ARRAY<CLDSNode*>slopenodes(hashSlopeNodes.GetNodeNum());
		for(pNode=hashSlopeNodes.GetFirst(),k=0;pNode;pNode=hashSlopeNodes.GetNext(),k++)
			slopenodes[k]=pNode;
		CBubbleSort<CLDSNodePtr>::BubSort(slopenodes,slopenodes.Size(),LocalCompareNodePosZ);
		int from=0,indexSegBtmNode=1;
		ARRAY_LIST<CLDSNodePtr> excludeNodesArr(0,4);
		do{
			if((indexSegBtmNode=DetectNextAdaptWindSegBtmNode(hashSlopeNodes,slopenodes,slopenodes.Size(),
				from,excludeNodesArr.m_pData,excludeNodesArr.GetSize()))>0)
			{
				AppendWindSegBranch(listWindSegments,hashSlopeNodes,slopenodes,slopenodes.Size(),from,indexSegBtmNode);
				excludeNodesArr.append(slopenodes[indexSegBtmNode]);
			}
		}while(indexSegBtmNode>=from);
	}
	//���ɷ�ѹ�ζ�����Ŀ¼���������Ӧ�Ľڵ�
	TRUNK_WINDSEG* pRawSeg=NULL;
	for(pRawSeg=listWindSegments.EnumObjectFirst();pRawSeg;pRawSeg=listWindSegments.EnumObjectNext())
	{
		CWindSegment* pWindSeg=Ta.WindLoadSeg.append();
		pWindSeg->iNo=pNoGroup->EnumIdleNo();
		pNoGroup->SetNoState(pWindSeg->iNo,TRUE);
		pWindSeg->cCalStyle=0;
		pWindSeg->cMirType=0;
		pWindSeg->cType=0;
		pWindSeg->nodeArr[0]=pRawSeg->pTop->handle;
		pWindSeg->nodeArr[1]=pRawSeg->pBtm->handle;
		//����������ѹ�β������ؼ�
		CXhChar50 heightStr;
		CFGWORD cfgword=pWindSeg->StatCfgword(&Ta);
		CFGLEG cfgleg=cfgword.ToCfgLeg();
		CXhChar50 segname;
		CXhChar16 nodestr("");
		if(pWindSeg->nodePtrArr[0]&&pWindSeg->nodePtrArr[1])
		{
			nodestr.Printf("#%d",pWindSeg->nodePtrArr[0]->pointI);
			nodestr.Append(CXhChar16("%d",pWindSeg->nodePtrArr[1]->pointI),'-');
		}
		if(!pWindSeg->nodePtrArr[1]->IsLegObj())
		{
			cfgleg.GetBodyScopeStr(heightStr);
			segname.Printf("S-%d{%s%s}",pWindSeg->iNo,(char*)heightStr,(char*)nodestr);
		}
		else
		{
			CXhChar16 legStr;
			cfgleg.GetBodyScopeStr(heightStr);
			cfgleg.GetLegScopeStr(legStr);
			heightStr.Append(legStr);
			segname.Printf("L-%d{%s%s}",pWindSeg->iNo,(char*)heightStr,(char*)nodestr);
		}
		pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WINDSEG_CASE,(DWORD)pWindSeg));
		//pItemInfo->itemType=WINDSEG_CASE;
		//pItemInfo->dwRefData=(DWORD)pWindSeg;
		hWindSegItem=pTreeCtrl->InsertItem(segname,PRJ_IMG_WINDSEG,PRJ_IMG_WINDSEG,hGroupItem);
		pTreeCtrl->SetItemData(hWindSegItem,(DWORD)pItemInfo);
		//����������ӷ�ѹ��������·�ѹ�νڵ�
		HTREEITEM hModuleItem;
		for(HTREEITEM *phModuleItem=listModelCaseItems.EnumObjectFirst();phModuleItem;phModuleItem=listModelCaseItems.EnumObjectNext())
		{
			hModuleItem=*phModuleItem;
			HTREEITEM hWindSegGroupItem=pTreeCtrl->GetNextItem(hModuleItem,TVGN_CHILD);
			TREEITEM_INFO* pModuleItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hModuleItem);
			if(pModuleItemInfo&&pModuleItemInfo->itemType==MODEL_CASE)
			{
				CLDSModule* pModule=(CLDSModule*)pModuleItemInfo->dwRefData;
				if(!cfgword.IsHasBodyNo(pModule->GetBodyNo()))
					continue;//�÷�ѹ�β��ڵ�ǰ��������
			}
			do{
				TREEITEM_INFO* pSegGroupItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hWindSegGroupItem);
				if(pSegGroupItemInfo&&pSegGroupItemInfo->itemType==WINDSEG_GROUP)
					break;
				hWindSegGroupItem=pTreeCtrl->GetNextSiblingItem(hWindSegGroupItem);
			}while(hWindSegGroupItem!=NULL);
			hWindSegItem=pTreeCtrl->GetNextItem(hWindSegGroupItem,TVGN_CHILD);
			pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(WINDSEG_CASE,(DWORD)pWindSeg));
			//pItemInfo->itemType=WINDSEG_CASE;
			//pItemInfo->dwRefData=(DWORD)pWindSeg;
			hWindSegItem=pTreeCtrl->InsertItem(segname,PRJ_IMG_WINDSEG,PRJ_IMG_WINDSEG,hWindSegGroupItem);
			pTreeCtrl->SetItemData(hWindSegItem,(DWORD)pItemInfo);
		}
	}
#endif
}
void CTowerTreeDlg::OnExportLoadcalReport() 
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
#if defined(__LDS_)||defined(__TSA_)
	if(pItemInfo->itemType==WIRELAYOUT_CASE)
	{
		if(pItemInfo->dwRefData==0)
			return;
	}
	else if(pItemInfo->itemType==WINDSEG_GROUP)
	{
		CWorkStatus *pStatus=Ta.GetActiveWorkStatus();
		CLDSModule *pModule=Ta.GetActiveModule();
		if(pStatus==NULL||pModule==NULL||Ta.WindLoadSeg.GetNodeNum()==0)
			return;
		char filename[MAX_PATH];
		sprintf(filename,"windload_%d_%d",pModule->iNo,pStatus->iNo);
		CFileDialog report_dlg(FALSE,"wind",filename,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"���ܷ���ؼ�����(*.wind)|*.wind||");
		if(report_dlg.DoModal()==IDOK)
		{
			FILE *fp=fopen(report_dlg.GetFileName(),"wt");
			if(fp==NULL)
			{
				AfxMessageBox("�򲻿�ָ���ļ���");
				return;
			}
			pModule->GetModuleScopeZ();
			CString sTime;
			CTime t=CTime::GetCurrentTime();
			sTime=t.Format("%Y��%m��%d��%Hʱ%M��");
			fprintf(fp,"************************************************************************\n");
			fprintf(fp,"*                                                                      *\n");
			fprintf(fp,"*  LDS��������ؼ���ϵͳ - (C) 2002-2019 �����ź��������Ƽ����޹�˾  *\n");
			fprintf(fp,"*                                                                      *\n");
			fprintf(fp,"*                        ��������ؼ�����                              *\n");
			fprintf(fp,"************************************************************************\n");
			fprintf(fp,"\n");
			fprintf(fp,"�����ͺţ�%s\n",Ta.m_sTaType);
			fprintf(fp,"���ߣ�%s\n",pModule->description);
			fprintf(fp,"������%d#%s\n",pStatus->iNo,pStatus->description);
			fprintf(fp,"����ǣ�%d��\n",pStatus->WindLoad.azimuth);
			fprintf(fp,"���٣�%.1fm/s\n",pStatus->WindLoad.velocity);
			if(Ta.m_cLandscape=='A')
				fprintf(fp,"��ò���ͣ�A:������ɳĮƽ̹����\n");
			else if(Ta.m_cLandscape=='B')
				fprintf(fp,"��ò���ͣ�B:��½������ϡ�����\n");
			else if(Ta.m_cLandscape=='C')
				fprintf(fp,"��ò���ͣ�C:���ܼ�����Ⱥ����\n");
			else //if(Ta.m_cLandscape=='D')
				fprintf(fp,"��ò���ͣ�D:���ܼ�����Ⱥ�͸߲㽨������\n");
			//Ԥ�ݼ��㵱ǰ�����¸���ѹ�εķ����ֵ
			CFGWORD cfgwordBody,cfgwordLeg;
			cfgwordBody=CFGWORD(pModule->GetBodyNo(),0);
			cfgwordLeg.AddSpecWord(pModule->m_dwLegCfgWord);
			try{
				int i=0;
				WINDSEGEXPORT_LIST exportList;
				preprocessor.PrepareTower(&Ta,cfgwordBody,cfgwordLeg,false);
				//CWindSegDispatcher dispatcher;
				preprocessor.InitWindSegElemPicker(&Ta);
				preprocessor.CalWindAndAttachLoad(&Ta,pModule,pStatus,false,pStatus->Bw_I(),&exportList);
				CWindSegment *pWindSeg;
				CHashSet<CWindSegment*> hashWindSegments;
				for (pWindSeg = Ta.WindLoadSeg.GetFirst(); pWindSeg; pWindSeg = Ta.WindLoadSeg.GetNext())
					hashWindSegments.SetValue(pWindSeg->iNo,pWindSeg);

				for(WINDSEGLOADCAL_EXPORT *pExport=exportList.GetFirst();pExport;pExport=exportList.GetNext())
				{
					pWindSeg=hashWindSegments.GetValue(pExport->windSegI);
					//����ֱ�۵ķ�ѹ������
					char cSymbol = 'S';
					if (pWindSeg->cType == 1)
						cSymbol = 'A';
					else if (pWindSeg->nodePtrArr[1] != NULL && pWindSeg->nodePtrArr[1]->IsLegObj())
						cSymbol = 'L';
					CXhChar50 szLabel("%C-%d", cSymbol, pWindSeg->iNo);
					szLabel.Append('{');
					int i,nNodeCount=pWindSeg->StatValidNodeCount(&Ta);
					double dfSummH=0;
					for(i=0;i<nNodeCount;i++)
					{
						dfSummH+=pWindSeg->nodePtrArr[i]->xOriginalPos.z;
						CXhChar16 nodelabel("%d",pWindSeg->nodePtrArr[i]->pointI);
						szLabel.Append(nodelabel,(i>0?'-':0));
					}
					szLabel.Append('}');

					fprintf(fp,"\n������������������ѹ��[%s]������������������������\n",(char*)szLabel);
					fprintf(fp,"\nһ��������Ϣ\n");
					if(pExport->cType==0)
						fprintf(fp,"�����涨�巽ʽ���˵���������ѹ��\n");
					else if(pExport->cType==1)
						fprintf(fp,"�����涨�巽ʽ������������ѹ��\n");
					else
						fprintf(fp,"�����涨�巽ʽ�����������ѹ��\n");
					if(pExport->cCalStyle==0)
						fprintf(fp,"�������ͣ�����\n");
					else if(pExport->cCalStyle==1)
						fprintf(fp,"�������ͣ�X��ˮƽ�ᵣ\n");
					else if(pExport->cCalStyle==2)
						fprintf(fp,"�������ͣ�Y��ˮƽ�ᵣ\n");
					if(pExport->cMirType==0)
						fprintf(fp,"�ԳƷ�ʽ����\n");
					else if(pExport->cMirType==1)
						fprintf(fp,"�ԳƷ�ʽ��Y��ԳƷ�ѹ��\n");
					else if(pExport->cMirType==2)
						fprintf(fp,"�ԳƷ�ʽ��X��ԳƷ�ѹ��\n");
					fprintf(fp,"\n�������洹ֱ����ؼ���\n");
					fprintf(fp,"                      �˼���ϸ�嵥\n");
					fprintf(fp,"�˼���ʶ     ���   ����(mm) �������(m2) ����ϵ�� ���� ����\n");
					fprintf(fp,"------------------------------------------------------------------------\n");
					double dfFrontArea=0,dfSideArea=0;//��dfSummRawMass=0,dfSummMassIncRaiseCoef=0
					//for(i=0;i<pExport->elemrecs.();i++)
					for(WINDSEGLOADCAL_EXPORT::ELEMREC* pRec=pExport->elemrecs.GetFirst();pRec;pRec=pExport->elemrecs.GetNext())
					{
						CXhChar16 spec("%3.0fx%2.0f%C",pRec->size.wide,pRec->size.thick,pRec->cMaterial);
						if(pRec->size.idClassType==CLS_LINEANGLE||pRec->size.idClassType==CLS_GROUPLINEANGLE)
						{
							spec.InsertBefore(" L",0);
							if(pRec->size.cSubClassType!='L')
								spec[0]=pRec->size.cSubClassType;
						}
						else if(pRec->size.idClassType==CLS_LINETUBE)
							spec.InsertBefore("��",0);//��
						else if(pRec->size.idClassType==CLS_LINESLOT)
							spec.InsertBefore(" [",0);
						else
							spec.InsertBefore(" U",0);
						//pRec->area=0.001*ftoi(0.001*pRec->area);
						pRec->area*=1e-6;
						pRec->efficArea=0.01*ftoi(100*pRec->efficArea);
						char front=pRec->frontwind?'V':' ';
						char side=pRec->sidewind?'V':' ';
						fprintf(fp,"%4d-%4d %s %5.0f      %6.3f      %.2f     %c    %c \n",pRec->startPointI,pRec->endPointI,(char*)spec,
							pRec->length,pRec->area,pRec->muS,front,side);
						if(pRec->frontwind)
							dfFrontArea+=pRec->area;
						if(pRec->sidewind)
							dfSideArea+=pRec->area;
					}
					fprintf(fp,"\n");
					double dfAreaOfAngles=0,dfAreaOfTubes=0;
					dfAreaOfAngles=pExport->areaOfFrontAngles*1e-6;
					dfAreaOfTubes =pExport->areaOfFrontTubes*1e-6;
					fprintf(fp,"�ϼ�  ���浲�����:  %8.3f m2,���� �Ǹ����: %8.3f m2, �ֹ����: %8.3f m2\n",dfFrontArea,dfAreaOfAngles,dfAreaOfTubes);
					dfAreaOfAngles=pExport->areaOfSideAngles*1e-6;
					dfAreaOfTubes =pExport->areaOfSideTubes*1e-6;
					fprintf(fp,"      ���浲�����:  %8.3f m2,���� �Ǹ����: %8.3f m2, �ֹ����: %8.3f m2\n",dfSideArea,dfAreaOfAngles,dfAreaOfTubes);

					fprintf(fp,"��׼��ѹ��%.3fkN/m2\n",pExport->Wo);
					fprintf(fp,"BH  =%7.3fm;��ظ�\n", pExport->dfNamedPosH);
					fprintf(fp,"��z =%.3f   ;��ѹ�߶ȱ仯ϵ��\n",pExport->muZ);
					fprintf(fp,"A_a =%6.3fm2;�����������\n",pExport->A_a);
					fprintf(fp,"A_b =%6.3fm2;�����������\n",pExport->A_b);
					fprintf(fp,"Af_a=%.3fm2 ;������ܷ�ѹͶӰ���(������ЧӦϵ��Cqi),m2\n",pExport->Af_a*pExport->Cqi_a);
					fprintf(fp,"Af_b=%.3fm2 ;������ܷ�ѹͶӰ���(������ЧӦϵ��Cqi),m2\n",pExport->Af_b*pExport->Cqi_b);
					fprintf(fp,"Cqia=%5.3f   ;���浲���������ϵ��\n",pExport->Cqi_a);
					fprintf(fp,"Cqib=%5.3f   ;���浲���������ϵ��\n",pExport->Cqi_b);
					fprintf(fp,"a   =%.3f   ;����ӭ���������,m\n", pExport->b2aFR.a*0.001);
					fprintf(fp,"b   =%.3f   ;����ӭ����������,m\n", pExport->b2aFR.b*0.001);
					fprintf(fp,"b/a_a=%.3f  ;����ӭ�������/ӭ������\n",pExport->b_a_a);
					fprintf(fp,"b/a_b=%.3f  ;����ӭ�������/ӭ������\n",pExport->b_a_b);
					fprintf(fp,"��_a=%.3f   ;���浲��ϵ��\n",pExport->fai_a);
					fprintf(fp,"��_b=%.3f   ;���浲��ϵ��\n",pExport->fai_b);
					fprintf(fp,"��_a=%.3f   ;���汳������ؽ���ϵ��\n",pExport->yita_a);
					fprintf(fp,"��_b=%.3f   ;���汳������ؽ���ϵ��\n",pExport->yita_b);
					fprintf(fp,"��sa=%.3f   ;��������ϵ�����Ѻ�1+�ǣ�\n",pExport->muSa);
					fprintf(fp,"��sb=%.3f   ;��������ϵ�����Ѻ�1+�ǣ�\n",pExport->muSb);
					fprintf(fp,"��z =%.3f   ;����ص���ϵ��\n",pExport->betaZ);
					fprintf(fp," K  =%.3f   ;�����������״ϵ��\n",pExport->K);
					if(pExport->cCalStyle==0)
					{
						fprintf(fp,"Wsa=Wo*K*��z*��s*��z*Af_a=%.3fkN\n",pExport->Wsa);
						fprintf(fp,"Wsb=Wo*K*��z*��s*��z*Af_b=%.3fkN\n",pExport->Wsb);
					}
					else if(pExport->cCalStyle==1)
						fprintf(fp,"Wsc=Wo*��z*��s*��z*Af_b=%.3fkN\n",pExport->Wsb);
					else if(pExport->cCalStyle==2)
						fprintf(fp,"Wsc=Wo*��z*��s*��z*Af_a=%.3fkN\n",pExport->Wsa);
					fprintf(fp,"\n�����Ƕȷ紵���ܷ����\n");
					if(pExport->cCalStyle==0)
					{
						fprintf(fp,"����ϵ��   Wsa   Wsb\n");
						fprintf(fp," Xcoef   %6.3f%6.3f\n",pExport->Xcoef_Wsa,pExport->Xcoef_Wsb);
						fprintf(fp," Ycoef   %6.3f%6.3f\n",pExport->Ycoef_Wsa,pExport->Ycoef_Wsb);
						fprintf(fp,"Wx=Xcoef_a*Wsa+Xcoef_b*Wsb=%5.3fkN\n",pExport->Wx);
						fprintf(fp,"Wy=Ycoef_a*Wsa+Ycoef_b*Wsb=%5.3fkN\n",pExport->Wy);
					}
					else
					{
						fprintf(fp,"����ϵ��   Wsc\n");
						fprintf(fp," Xcoef    %4.2f\n",pExport->Xcoef_Wsc);
						fprintf(fp," Ycoef    %4.2f\n",pExport->Ycoef_Wsc);
						fprintf(fp,"Wx=Xcoef_c*Wsc=%5.3fkN\n",pExport->Wx);
						fprintf(fp,"Wy=Ycoef_c*Wsc=%5.3fkN\n",pExport->Wy);
					}
					fprintf(fp,"\n�ġ���Ч��ѹ���ؽڵ�\n");
					fprintf(fp,"Node   X(kN)   Y(kN)\n");
					DYN_ARRAY<WINDLOADNODE> loadnode_arr(pExport->loadNodeList.GetNodeNum());
					i=0;
					for(WINDLOADNODE *pLoadNode=pExport->loadNodeList.GetFirst();pLoadNode;pLoadNode=pExport->loadNodeList.GetNext(),i++)
						loadnode_arr[i]=*pLoadNode;
					CHeapSort<WINDLOADNODE>::HeapSort(loadnode_arr,pExport->loadNodeList.GetNodeNum(),compare_windnode_func);
					for(i=0;i<(int)pExport->loadNodeList.GetNodeNum();i++)
						fprintf(fp,"%4d  %6.3f  %6.3f\n",loadnode_arr[i].nodeI,loadnode_arr[i].Fwx,loadnode_arr[i].Fwy);
				}
				fclose(fp);
				WinExec(CXhChar200("notepad.exe %s",report_dlg.GetFileName()),SW_SHOW);
			}
			catch(char* sError)
			{
				fclose(fp);
				AfxMessageBox(sError);
			}
			catch(CXhChar100 sError)
			{
				fclose(fp);
				AfxMessageBox(sError);
			}
			catch(CString sError)
			{
				fclose(fp);
				AfxMessageBox(sError);
			}
		}
	}
#endif	
}

HTREEITEM CTowerTreeDlg::InsertBlockItem(CBlockModel *pBlock)
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	CXhChar50 blkname("%s(0X%X)",pBlock->name,pBlock->handle);
	HTREEITEM hBlockItem;
	if(pBlock->IsEmbeded())//��Ƕ
		hBlockItem = pTreeCtrl->InsertItem(blkname,PRJ_IMG_EMBED_BLOCK_CASE,PRJ_IMG_EMBED_BLOCK_CASE,m_hDataRootItem);
	else 
		hBlockItem = pTreeCtrl->InsertItem(blkname,PRJ_IMG_IMPORT_BLOCK_CASE,PRJ_IMG_IMPORT_BLOCK_CASE,m_hDataRootItem);
	TREEITEM_INFO *pItemInfo=InsertOrUpdateItemInfo(TREEITEM_INFO(BLOCK_CASE,(DWORD)pBlock));
	pTreeCtrl->SetItemData(hBlockItem,(DWORD)pItemInfo);
	return hBlockItem;
}
void CTowerTreeDlg::OnImportBlockFile()
{
	if(!PRODUCT_FUNC::IsHasBlockSetup())
		return;
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL,*pSubItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=DATA_SPACE)
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	char* pszFilter="Tower Block File(*.sgt)|*.sgt||";
	if(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT))
		pszFilter="Tower Block File(*.sgt)|*.sgt|Metal fittings XML File(*.xml)|*.xml||";
#else 
	char* pszFilter="�����������ļ�(*.sgt)|*.sgt||";
	if(VerifyValidFunction(LICFUNC::FUNC_IDENTITY_TIDMODEL_EXPORT))
		pszFilter="�����������ļ�(*.sgt)|*.sgt|��ߴ�XML�ļ�(*.xml)|*.xml||";
#endif
	CFileDialog dlg(TRUE,"sgt","segment",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,pszFilter);
	if(dlg.DoModal()!=IDOK)
		return;
	HTREEITEM hTailBlockItem=NULL;
	CBlockModel *pTailBlock=NULL;
	POSITION pos=dlg.GetStartPosition();
	while(pos)
	{
		CXhChar200 fname("%s",dlg.GetNextPathName(pos));
		char ext[MAX_PATH];
		_splitpath(fname,NULL,NULL,NULL,ext);
		CBlockModel *pBlock=pTailBlock=Ta.Block.append();
		if(stricmp(ext,".xml")==0)
		{
			pBlock->ImportFromXMLFile(fname);
			pBlock->SetEmbeded(FALSE);
		}
		else
			pBlock->ImportFromBlkFile(fname);
		HTREEITEM hBlockItem=hTailBlockItem=InsertBlockItem(pBlock);
		RefreshModelItem(hBlockItem,pBlock);
		//hBlockItem = InsertBlockItem(pBlock,hItem);
	}
	pTreeCtrl->Expand(hItem,TVE_EXPAND);
	if(hTailBlockItem!=NULL)
	{
		pTreeCtrl->SelectItem(hTailBlockItem);
		//��ʾ��������
		CLDSView *pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
		pLdsView->m_pSelectBlock=pTailBlock;
		pLdsView->DisplayBlockModelProperty(TRUE);
	}
}

void CTowerTreeDlg::OnExportBlockFile()
{
	if(!PRODUCT_FUNC::IsHasBlockSetup())
		return;
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL,*pSubItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=BLOCK_CASE)
		return;
	CBlockModel *pBlock=(CBlockModel*)pItemInfo->dwRefData;
	if(pBlock==NULL)
		return;
	if(pBlock->IsEmbeded())
	{	//�����Ƕʽ�����ڵ�������
		NODESET nodeSet;
		BOOL bRetCode=pBlock->CheckNodeIntegrality(console.GetActiveModel(),nodeSet);
		if(!bRetCode)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(AfxMessageBox("Some rods of block lost end nodes,whether to continue to export after auto-complete��",MB_YESNO)==IDYES)
#else 
			if(AfxMessageBox("������ĳЩ�˼���ʧ�˽ڵ㣬�Ƿ��Զ���������������",MB_YESNO)==IDYES)
#endif
			{
				for(CLDSNode *pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
					pNode->m_hBlock=pBlock->handle;	
			}
			else 
				return;
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(FALSE,"sgt",pBlock->name,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Tower Block File(*.sgt)|*.sgt||");
#else 
	CFileDialog dlg(FALSE,"sgt",pBlock->name,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"�����������ļ�(*.sgt)|*.sgt||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	CXhChar200 fname("%s",dlg.GetPathName());
	CXhChar50 file_version;
	theApp.GetAppVersion(file_version,NULL);
	long nVersion=FromStringVersion(file_version);
	pBlock->ExportToBlkFile(fname,nVersion,&Ta);
}
void CTowerTreeDlg::OnUpdateBlock()
{
	if(!PRODUCT_FUNC::IsHasBlockSetup())
		return;
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL,*pSubItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=BLOCK_CASE)
		return;
	CBlockModel *pBlock=(CBlockModel*)pItemInfo->dwRefData;
	if(pBlock==NULL)
		return;
	CFileDialog dlg(TRUE,"sgt","segment",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Tower Block File(*.sgt)|*.sgt||");
#else 
		"�����������ļ�(*.sgt)|*.sgt||");
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	pBlock->Empty();
	CXhChar200 fname("%s",dlg.GetPathName());
	pBlock->ImportFromBlkFile(fname);
	console.Display(NULL);	//��ֹ��δ������ʾ�����б���������Чָ�뵼������
	//���²������ݺ�ˢ�¹����Ĳ�������
	for(CBlockReference *pBlockRef=Ta.BlockRef.GetFirst();pBlockRef;pBlockRef=Ta.BlockRef.GetNext())
	{
		if(pBlockRef->m_hBlock!=pBlock->handle)
			continue;
		pBlockRef->SetModified();
		pBlockRef->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBlockRef->GetSolidPartObject());
	}
	g_pSolidDraw->Draw();
	RefreshModelItem(hItem,pBlock);
}
void CTowerTreeDlg::OnIntegrateBlockRef()
{
	if(!PRODUCT_FUNC::IsHasBlockSetup())
		return;
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=BLOCKREF_CASE)
		return;
	CBlockReference* pBlockRef=(CBlockReference*)pItemInfo->dwRefData;
	CLDSView* pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	if(pLdsView&&pBlockRef&&pLdsView->IntegrateBlockRef(pBlockRef))
		pTreeCtrl->DeleteItem(hItem);
}
void CTowerTreeDlg::OnExplodedBlockRef()
{
	if(!PRODUCT_FUNC::IsHasBlockSetup())
		return;
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType!=BLOCKREF_CASE)
		return;
	CBlockReference* pBlockRef=(CBlockReference*)pItemInfo->dwRefData;
	CLDSView* pLdsView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	if(pLdsView&&pBlockRef)
		pLdsView->ExplodeBlockRef(pBlockRef);
	//�ı䲿������ͼ��
	if(pBlockRef->IsExploded())
		pTreeCtrl->SetItemImage(hItem,PRJ_IMG_EXPLODED_BLOCKREF_CASE,PRJ_IMG_EXPLODED_BLOCKREF_CASE);
	else
		pTreeCtrl->SetItemImage(hItem,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE,PRJ_IMG_UNEXPLODED_BLOCKREF_CASE);
}
BOOL CTowerTreeDlg::RenameSelectedItem(CString sName)
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	if(hItem==NULL)
		return FALSE;
	pTreeCtrl->SetItemText(hItem,sName);
	return TRUE;
}
PROPLIST_ITEM* CTowerTreeDlg::GetSelectHelpItem(int menuId/*=0*/)
{
	CTreeCtrl *pTreeCtrl = GetTreeCtrl();
	HTREEITEM hItem=pTreeCtrl->GetSelectedItem();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pTreeCtrl->GetItemData(hItem);
	if(pItemInfo==NULL)
		return NULL;
	CXhChar100 sKey;
	if(pItemInfo->itemType==DATA_SPACE)	
		sKey.Copy("DATA_SPACE");	//����ģ��
	else if(pItemInfo->itemType==MODEL_GROUP)
		sKey.Copy("MODEL_GROUP");	//����
	else if(pItemInfo->itemType==VIEW_GROUP)	
		sKey.Copy("VIEW_GROUP");	//��ͼ
	else if(pItemInfo->itemType==VIEW_LAYERSET)		
		sKey.Copy("VIEW_LAYERSET");	//ͼ����ͼ
	else if(pItemInfo->itemType==VIEW_DEFINEDSET)	
		sKey.Copy("VIEW_DEFINEDSET");	//������ͼ
	else if(pItemInfo->itemType==VIEW_DEFINEDFOLDER)	
		sKey.Copy("VIEW_DEFINEDFOLDER");	//������
	else if(pItemInfo->itemType==VIEW_CASE)	
		sKey.Copy("VIEW_CASE");	//��ͼ��
	else if(pItemInfo->itemType==MODEL_DATUM_POINT_GROUP)	
		sKey.Copy("MODEL_DATUM_POINT_GROUP");	//��׼��
	else if(pItemInfo->itemType==MODEL_DATUM_POINT_CASE)	
		sKey.Copy("MODEL_DATUM_POINT_CASE");	//��׼����
	else if(pItemInfo->itemType==MODEL_DATUM_LINE_GROUP)	
		sKey.Copy("MODEL_DATUM_LINE_GROUP");	//��׼��
	else if(pItemInfo->itemType==MODEL_DATUM_LINE_CASE)		
		sKey.Copy("MODEL_DATUM_LINE_CASE");		//��׼��Ԫ
	else if(pItemInfo->itemType==MODEL_DATUM_PLANE_GROUP)	
		sKey.Copy("MODEL_DATUM_PLANE_GROUP");	//��׼��
	else if(pItemInfo->itemType==MODEL_DATUM_PLANE_CASE)	
		sKey.Copy("MODEL_DATUM_PLANE_CASE");	//��׼��Ԫ
	else if(pItemInfo->itemType==MODEL_ARCLIFT_GROUP)		
		sKey.Copy("MODEL_ARCLIFT_GROUP");	//Բ��Ԥ��
	else if(pItemInfo->itemType==MODEL_ARCLIFT_CASE)	
		sKey.Copy("MODEL_ARCLIFT_CASE");	//Ԥ����
	else if(pItemInfo->itemType==BLOCKREF_GROUP)	
		sKey.Copy("BLOCKREF_GROUP");	//����������
	else if(pItemInfo->itemType==BLOCKREF_CASE)
		sKey.Copy("BLOCKREF_CASE");		//����������
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOAD)
		sKey.Copy("WORKSTATUS_ATTACH_WINDLOAD");	//���Ӻ�����
	else if(pItemInfo->itemType==WORKSTATUS_ATTACH_WINDLOADITEM)
		sKey.Copy("WORKSTATUS_ATTACH_WINDLOADITEM");	//������
	else if(pItemInfo->itemType==MODEL_CASE)	
		sKey.Copy("MODEL_CASE");	//����
	else if(pItemInfo->itemType==MODEL_BURGLAR_LS_SCOPE)
		sKey.Copy("MODEL_BURGLAR_LS_SCOPE");	//������˨
	else if(pItemInfo->itemType==MODEL_QUAD1LEG_CFGNO||pItemInfo->itemType==MODEL_QUAD2LEG_CFGNO||
		pItemInfo->itemType==MODEL_QUAD3LEG_CFGNO||pItemInfo->itemType==MODEL_QUAD4LEG_CFGNO)
		sKey.Copy("MODEL_LEG_CFGNO");	//������ĺ�
	else if(pItemInfo->itemType==MODEL_RESTRICT)
		sKey.Copy("MODEL_RESTRICT");	//Լ��������
	else if(pItemInfo->itemType==MODEL_RESTRICT_NODE)
		sKey.Copy("MODEL_RESTRICT_NODE");	//Լ���ڵ�
	else if(pItemInfo->itemType==WORKSTATUS_GROUP)
		sKey.Copy("WORKSTATUS_GROUP");	//���ع�����
	else if(pItemInfo->itemType==WINDSEG_GROUP)
		sKey.Copy("WINDSEG_GROUP");		//��ѹ����
	else if(pItemInfo->itemType==WINDSEG_CASE)
		sKey.Copy("WINDSEG_CASE");	//��ѹ����	
	else if(pItemInfo->itemType==BLOCK_CASE)
		sKey.Copy("BLOCK_CASE");	//����
	else if(pItemInfo->itemType==BOLT_GROUP)
		sKey.Copy("BOLT_GROUP");	//��˨
	//ѡ�в˵���
	if(menuId==ID_IMPORT_BLOCK_FILE)
		sKey.Append(".ID_IMPORT_BLOCK_FILE");
	else if(menuId==ID_REFRESH_TREE)
		sKey.Append(".ID_REFRESH_TREE");
	else if(menuId==ID_EDIT_ITEM_PROP_ITEM)
		sKey.Append(".ID_EDIT_ITEM_PROP_ITEM");
	else if(menuId==ID_NEW_ITEM)
		sKey.Append(".ID_NEW_ITEM");
	else if(menuId==ID_COPY_ITEM)
		sKey.Append(".ID_COPY_ITEM");
	else if(menuId==ID_SORT_ITEM)
		sKey.Append(".ID_SORT_ITEM");
	else if(menuId==ID_NEW_ITEM_FOLDER)
		sKey.Append(".ID_NEW_ITEM_FOLDER");
	else if(menuId==ID_RETRIEVE_UDF_VIEW)
		sKey.Append(".ID_RETRIEVE_UDF_VIEW");
	else if(menuId==ID_RETRIEVE_DRAWING_VIEW)
		sKey.Append(".ID_RETRIEVE_DRAWING_VIEW");
	else if(menuId==ID_DELETE_ITEM)
		sKey.Append(".ID_DELETE_ITEM");
	else if(menuId==ID_SET_ACTIVE_ITEM)
		sKey.Append(".ID_SET_ACTIVE_ITEM");
	else if(menuId==ID_LAYERVIEW_SEG_PROP_ITEM)
		sKey.Append(".ID_LAYERVIEW_SEG_PROP_ITEM");
	else if(menuId==ID_HANDOVER_PERMISSION)
		sKey.Append(".ID_HANDOVER_PERMISSION");
	else if(menuId==ID_BLOCKREF_INTEGRATE)
		sKey.Append(".ID_BLOCKREF_INTEGRATE");
	else if(menuId==ID_EXPLODE_BLOCKREF)
		sKey.Append(".ID_EXPLODE_BLOCKREF");
	else if(menuId==ID_MODEL_DESIGN_CAL)
		sKey.Append(".ID_MODEL_DESIGN_CAL");
	else if(menuId==ID_MODEL_EQ_CHECK)
		sKey.Append(".ID_MODEL_EQ_CHECK");
	else if(menuId==ID_CAL_WIND_BETA_Z)
		sKey.Append(".ID_CAL_WIND_BETA_Z");
	else if(menuId==ID_EXPORT_LOADCAL_REPORT)
		sKey.Append(".ID_EXPORT_LOADCAL_REPORT");
	else if(menuId==ID_UPDATE_BLOCK)
		sKey.Append(".ID_UPDATE_BLOCK");
	else if(menuId==ID_EXPORT_BLOCK_FILE)
		sKey.Append(".ID_EXPORT_BLOCK_FILE");
	return CTowerTreeDlg::propMenuHashtable.GetValue(sKey);
}
BOOL CTowerTreeDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	CXhChar200 helpStr;
	PROPLIST_ITEM* pHelpItem=NULL;
	if(pHelpInfo->iContextType==HELPINFO_MENUITEM)
		pHelpItem=GetSelectHelpItem(pHelpInfo->iCtrlId);
	else
		pHelpItem=GetSelectHelpItem();
	if(pHelpItem==NULL)
		pHelpItem=CLDSApp::hashDialogHelpItemById.GetValue(m_nIDHelp);
	if(pHelpItem!=NULL)
	{
		sprintf(helpStr,"%s::/%s",theApp.m_pszHelpFilePath,(char*)pHelpItem->Url);
		HtmlHelp((DWORD_PTR)(char*)helpStr,HH_DISPLAY_TOPIC);
	}
	return true;
}

BOOL IsMatchedView(CDisplayView *pView,int iViewType);	//From ShiftView.cpp
BOOL CTowerTreeDlg::ActivateLayerViewItem(int iViewType)
{
	if(m_hLayerViewItem==NULL)
		return FALSE;
	HTREEITEM  hChildItem = m_treeCtrl.GetChildItem(m_hLayerViewItem);
	while(hChildItem!=NULL)
	{
		TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_treeCtrl.GetItemData(hChildItem);
		if(pItemInfo->itemType!=VIEW_CASE)
			continue;
		CDisplayView *pView=(CDisplayView*)pItemInfo->dwRefData;
		if(pView&&IsMatchedView(pView,iViewType))
		{	//ѡ����ͼ��
			m_treeCtrl.SelectItem(hChildItem);
			//���ǰ��ͼ
			OnSetActiveItem();
			break;
		}
		hChildItem = m_treeCtrl.GetNextSiblingItem(hChildItem);
	}
	return TRUE;
}

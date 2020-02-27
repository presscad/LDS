//<LOCALE_TRANSLATE Confirm by hxr/>
// TowerPartGroupDLg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "TowerPartGroupDlg.h"
#include "image.h"
#include "Tower.h"
#include "PartGroupDlg.h"
#include "SectionViewDlg.h"
#include "env_def.h"
#include "SortFunc.h"
#include "FolderNameDlg.h"
#include "PartGroupLocatePointDlg.h"
#include "CmdLineDlg.h"
#include "MainFrm.h"
#include "PartGroupAngularDlg.h"
#include "PartGroupCheckLineDlg.h"
#include "ArrayList.h"
#include "ComparePartNoString.h"
#include "XhCharString.h"
#include "GlobalFunc.h"
#include "SectionViewPropertyDlg.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CTowerPartGroupDlg dialog
CSectionView* TREEITEM_PARTGROUP_INFO::GetVerifiedSectionView(CLDSPartGroup** ppPartGroup)
{
	if(itemType!=PARTGROUP_VIEWCASE)
		return NULL;
	for(CLDSPartGroup *pTempPartGroup=Ta.PartGroup.GetFirst();pTempPartGroup;pTempPartGroup=Ta.PartGroup.GetNext())
	{
		if(pTempPartGroup==pObj)
			break;
	}
	CSectionView* pView=NULL;
	if(pTempPartGroup!=NULL)
		pView=pTempPartGroup->SonViews.GetValue(handle);
	if(ppPartGroup)
		*ppPartGroup=pTempPartGroup;
	return pView;
}

static DROPEFFECT DragOverFunc(CTreeCtrl *pTreeCtrl, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TVHITTESTINFO HitTestInfo;
	HitTestInfo.pt = point;
	HTREEITEM hItem = pTreeCtrl->HitTest(&HitTestInfo);
	if(hItem&&pDataObject->IsDataAvailable(CF_TEXT))
	{	
		TREEITEM_PARTGROUP_INFO *pItemInfo=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItem);
		if(pItemInfo&&pItemInfo->itemType==PARTGROUP_VIEWCASE)
		{
			pTreeCtrl->SelectDropTarget(hItem);
			return DROPEFFECT_LINK;
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
	if(hItem&&pDataObject->IsDataAvailable(CF_TEXT))
	{	
		TREEITEM_PARTGROUP_INFO *pItemInfo=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItem);
		if(pItemInfo&&pItemInfo->itemType==PARTGROUP_VIEWCASE)
		{
			CSectionView *pView=pItemInfo->GetVerifiedSectionView();
			if(pView)
			{
				FORMATETC fmtetc = {CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_ISTREAM};
				STGMEDIUM stgmed;
				ZeroMemory(&stgmed, sizeof(STGMEDIUM));
				BOOL bGet = pDataObject->GetData(CF_TEXT,&stgmed,&fmtetc);
				if(bGet && stgmed.tymed==TYMED_ISTREAM)
				{
					GlobalLock(stgmed.pstm); //锁定内存块
					LARGE_INTEGER dlibMove;
					dlibMove.QuadPart=0;
					stgmed.pstm->Seek(dlibMove,STREAM_SEEK_SET,NULL);
					HRESULT hr=stgmed.pstm->Read(&num,sizeof(long),NULL);
					//将传送过来的数据加入相应视图中
					for(int i=0;i<num;i++)
					{
						stgmed.pstm->Read(&h,sizeof(long),NULL);
						CSectionView::PART_ITEM *pPartItem = NULL;
						for(pPartItem=pView->SonParts.GetFirst();pPartItem;pPartItem=pView->SonParts.GetNext())
						{
							if(pPartItem->hSonPart==h)
								break;
						}
						if(pPartItem)
							continue;
						CLDSPart *pPart=(CLDSPart*)console.FromPartHandle(h);
						if(pPart)
						{
							pPartItem = pView->SonParts.Add(pPart->handle);
							pPartItem->cFlag=0;
						}	
					}
					GlobalUnlock((HGLOBAL)stgmed.pstm);
					// Release the stream.
					ReleaseStgMedium(&stgmed);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

static BOOL IsValidDragS(CTowerTreeCtrl *pTreeCtrl, HTREEITEM hItemDragS)
{
	if(hItemDragS==NULL)
		return FALSE;
	//拖放源节点类型可为视图，文件夹，组合焊件
	TREEITEM_PARTGROUP_INFO *pItemInfo=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItemDragS);
	if(pItemInfo==NULL||(pItemInfo&&pItemInfo->itemType!=PARTGROUP_VIEWCASE&&
	   pItemInfo->itemType!=PARTGROUP_CASE&&pItemInfo->itemType!=PARTGROUP_FOLDER))
		return FALSE;
	return TRUE;
}

static BOOL IsValidDragD(CTowerTreeCtrl *pTreeCtrl, HTREEITEM hItemDragS, HTREEITEM hItemDragD)
{
	if(hItemDragS==NULL||hItemDragD==NULL)
		return FALSE;	//hItemDragD==NULL时调用GetItemData()死机 wht 13-12-09
	TREEITEM_PARTGROUP_INFO *pItemInfoS=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItemDragS);
	TREEITEM_PARTGROUP_INFO *pItemInfoD=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItemDragD);
	if(pItemInfoS==NULL||pItemInfoD==NULL)
		return FALSE;
	if(pItemInfoS->itemType==PARTGROUP_VIEWCASE)
	{	//视图-->视图 视图-->视图集合	
		if(pItemInfoD->itemType==PARTGROUP_VIEWSET||pItemInfoD->itemType==PARTGROUP_VIEWCASE)
			return TRUE;
	}
	else if(pItemInfoS->itemType==PARTGROUP_CASE)
	{	//组合焊件-->组合焊件 组合焊件-->组合焊件文件夹
		if(pItemInfoD->itemType==PARTGROUP_CASE||pItemInfoD->itemType==PARTGROUP_FOLDER)
			return TRUE;
	}
	else if(pItemInfoS->itemType==PARTGROUP_FOLDER)
	{	//组合焊件文件夹-->组合焊件文件夹 组合焊件文件夹-->组合焊件集合
		if(pItemInfoD->itemType==PARTGROUP_SET||pItemInfoD->itemType==PARTGROUP_FOLDER)
			return TRUE;
	}
	return FALSE;
}

static int CompareDataLevel(CTowerTreeCtrl *pTreeCtrl, HTREEITEM hItemDragS, HTREEITEM hItemDragD)
{
	if(hItemDragS==NULL||hItemDragD==NULL)
		return 0;
	TREEITEM_PARTGROUP_INFO *pItemInfoS=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItemDragS);
	TREEITEM_PARTGROUP_INFO *pItemInfoD=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItemDragD);
	if(pItemInfoS==NULL||pItemInfoD==NULL)
		return 0;
	if(pItemInfoS->itemType==pItemInfoD->itemType)
	{
		if(pItemInfoS->itemType==PARTGROUP_CASE&&pItemInfoD->itemType==PARTGROUP_CASE)
		{	//将一个组合焊件拖放到一个文件夹的同时应该修改组合焊件所在文件夹的名字
			if(pItemInfoS->pObj&&pItemInfoD->pObj)
				strcpy(((CLDSPartGroup*)pItemInfoS->pObj)->sFolderName,((CLDSPartGroup*)pItemInfoD->pObj)->sFolderName);
		}
		return 0;
	}
	else if((pItemInfoS->itemType==PARTGROUP_VIEWCASE&&pItemInfoD->itemType==PARTGROUP_VIEWSET)||
			(pItemInfoS->itemType==PARTGROUP_CASE&&pItemInfoD->itemType==PARTGROUP_FOLDER)||
			(pItemInfoS->itemType==PARTGROUP_FOLDER&&pItemInfoD->itemType==PARTGROUP_SET))
	{	//暂时在此处修改 
		//拖放组合焊件到文件夹下或从文件夹拖出都需要修改相关属性 否则保存不上所做修改 
		if(pItemInfoS->itemType==PARTGROUP_CASE&&pItemInfoD->itemType==PARTGROUP_FOLDER)
		{	//将一个组合焊件拖放到一个文件夹的同时应该修改组合焊件所在文件夹的名字
			if(pItemInfoS->pObj&&pItemInfoD->pObj)
				strcpy(((CLDSPartGroup*)pItemInfoS->pObj)->sFolderName,((CLDSPartGroup*)pItemInfoD->pObj)->sFolderName);
		}
		else if(pItemInfoS->itemType==PARTGROUP_CASE&&pItemInfoD->itemType==PARTGROUP_SET)
		{	//将组合焊件拖到跟目录下 文件夹名字置空
			if(pItemInfoS->pObj)
				strcpy(((CLDSPartGroup*)pItemInfoS->pObj)->sFolderName,"");
		}
		return 1;
	}
	return 0;
}

IMPLEMENT_DYNCREATE(CTowerPartGroupDlg, CDialog)
CTowerPartGroupDlg::CTowerPartGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTowerPartGroupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTowerPartGroupDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTowerPartGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTowerPartGroupDlg)
	DDX_Control(pDX, IDC_TREE_CTRL, m_treeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTowerPartGroupDlg, CDialog)
	//{{AFX_MSG_MAP(CTowerPartGroupDlg)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CTRL, OnRclickTreeCtrl)
	ON_COMMAND(ID_REFRESH_PART_GROUP, OnRefreshPartGroup)
	ON_COMMAND(ID_STAT_PART_GROUP_AUTO, OnStatPartGroupAuto)
	ON_COMMAND(ID_RETRIEVE_WELDLINE, OnSmartRetrieveWeldline)
	ON_COMMAND(ID_SORT_PARTGROUP, OnSortPartGroup)
	ON_COMMAND(ID_DELETE_PARTGROUP, OnDeletePartgroup)
	ON_COMMAND(ID_RETRIEVE_PARTGROUP_VIEWS,OnRetrievePartgroupViews)
	ON_COMMAND(ID_EDIT_PARTGROUP, OnEditPartgroup)
	ON_COMMAND(ID_NEW_PARTGROUP, OnNewPartgroup)
	ON_COMMAND(ID_NEW_PARTGROUP_FOLDER, OnNewPartgroupFolder)
	ON_COMMAND(ID_SET_ACTIVE_PARTGROUP, OnSetActivePartgroup)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_CTRL, OnDblclkTreeCtrl)
	ON_COMMAND(ID_PARTGROUP_LOCATE_POINT, OnPartGroupLocatePoint)
	ON_COMMAND(ID_PARTGROUP_CHECK_LINE, OnPartGroupCheckLine)
	ON_COMMAND(ID_PARTGROUP_ANGULAR, OnPartGroupAngular)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_CTRL, OnKeydownTreeCtrl)
	ON_COMMAND(ID_COPY_AND_NEW_ITEM, OnCopyAndNewItem)
	ON_COMMAND(ID_CHECK_PARTGROUP_VALID, OnCheckPartGroupValid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTowerPartGroupDlg message handlers

BOOL CTowerPartGroupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	RECT rc;
	GetClientRect(&rc);
	if(m_treeCtrl.GetSafeHwnd())
		m_treeCtrl.MoveWindow(&rc);
	m_ModelImages.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	m_treeCtrl.SetImageList(&m_ModelImages,TVSIL_NORMAL);
	m_treeCtrl.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS);
	m_treeCtrl.m_dropTarget.Register(&m_treeCtrl);
	m_treeCtrl.SetDragOverFunc(DragOverFunc);
	m_treeCtrl.SetDropFunc(DropFunc);
	m_treeCtrl.SetIsValidDragSFunc(IsValidDragS);
	m_treeCtrl.SetIsValidDragDFunc(IsValidDragD);
	m_treeCtrl.SetCompareDataLevelFunc(CompareDataLevel);
	RefreshModel();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTowerPartGroupDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(m_treeCtrl.GetSafeHwnd())
		m_treeCtrl.MoveWindow(0,0,cx,cy);
}
void CTowerPartGroupDlg::RefreshPartGroupViewSetNode(HTREEITEM hViewSetItem,CLDSPartGroup *pPartGroup)
{
	//1.清空指定结点所有子节点及子节点对应的附加数据
	HTREEITEM hItem=m_treeCtrl.GetChildItem(hViewSetItem);
	while(hItem!=NULL){
		TREEITEM_PARTGROUP_INFO *pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
		for(TREEITEM_PARTGROUP_INFO *pInfo=itemInfoList.GetFirst();pInfo;pInfo=itemInfoList.GetNext())
		{
			if(pItemInfo==pInfo)
			{
				itemInfoList.DeleteCursor(TRUE);
				break;
			}
		}
		m_treeCtrl.DeleteItem(hItem);
		hItem=m_treeCtrl.GetNextItem(hViewSetItem,TVGN_CHILD);
	};

	TREEITEM_PARTGROUP_INFO *pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hViewSetItem);
	for(CSectionView *pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
	{
		CXhChar100 sxName;
		if(pView->IsSlaveView())
			sxName.Printf("<%s-%s>",pView->Symbol,pView->Symbol);
		sxName.Append(pView->sName);
		HTREEITEM hViewItem=m_treeCtrl.InsertItem(sxName,PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hViewSetItem);
		pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWCASE,pPartGroup,pView->GetId()));
		m_treeCtrl.SetItemData(hViewItem,(DWORD)pItemInfo);
	}
	m_treeCtrl.Expand(hViewSetItem,TVE_EXPAND);
}
void CTowerPartGroupDlg::RefreshPartGroupTreeNode(HTREEITEM hPartGroupItem,CLDSPartGroup *pPartGroup,bool removeOld/*=true*/)
{
	CString ss="";
	//1.清空指定结点所有子节点及子节点对应的附加数据
	if(removeOld)
	{	//由RefreshModel调用时，由于原本就没有数据，所以不需要清除旧数据
		HTREEITEM hItem=m_treeCtrl.GetChildItem(hPartGroupItem);
		do{
			ss=m_treeCtrl.GetItemText(hItem);
			TREEITEM_PARTGROUP_INFO *pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
			for(TREEITEM_PARTGROUP_INFO *pInfo=itemInfoList.GetFirst();pInfo;pInfo=itemInfoList.GetNext())
			{
				if(pItemInfo==pInfo)
				{
					itemInfoList.DeleteCursor(TRUE);
					break;
				}
			}
			m_treeCtrl.DeleteItem(hItem);
			hItem=m_treeCtrl.GetNextItem(hPartGroupItem,TVGN_CHILD);
		}while(hItem!=NULL);
	}
	//2.更新组焊件子节点
	//hPartGroupItem原本就有，不是新加节点故不需要重复添加pItemInfo wjh-2014.5.22
	//TREEITEM_PARTGROUP_INFO *pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_CASE,pPartGroup,pPartGroup->handle));
	//m_treeCtrl.SetItemData(hPartGroupItem,(DWORD)pItemInfo);
	CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
	if(pRootPart)
#ifdef AFX_TARG_ENU_ENGLISH
		ss.Format("basic parts=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
	else
		ss.Format("basic parts=(0X%X)",pPartGroup->handle);
#else
		ss.Format("基构件=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
	else
		ss.Format("基构件=(0X%X)",pPartGroup->handle);
#endif
	HTREEITEM hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
	TREEITEM_PARTGROUP_INFO *pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_ROOTPART,pPartGroup));
	m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);

#ifdef AFX_TARG_ENU_ENGLISH
	ss.Format("single number=%d piece",pPartGroup->m_uStatMatCoef);
#else
	ss.Format("单基数=%d件",pPartGroup->m_uStatMatCoef);
#endif
	hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
	pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_STATNUM,NULL));
	m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);

#ifdef AFX_TARG_ENU_ENGLISH
	HTREEITEM hViewItem=m_treeCtrl.InsertItem("additional view",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#else
	HTREEITEM hViewItem=m_treeCtrl.InsertItem("附加视图",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#endif
	pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWSET,NULL));
	m_treeCtrl.SetItemData(hViewItem,(DWORD)pItemInfo);
	RefreshPartGroupViewSetNode(hViewItem,pPartGroup);	//单独提取成函数以便重复调用
	m_treeCtrl.Expand(hViewItem,TVE_EXPAND);
}
void CTowerPartGroupDlg::RefreshModel()
{
	BOOL bTowerDataModified=Ta.m_bDataModified;
	itemInfoList.Empty();
	CString ss;
	m_treeCtrl.DeleteAllItems();
	HTREEITEM hGroupSetItem,hPartGroupItem,hFolderItem=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	hGroupSetItem = m_treeCtrl.InsertItem("combined weld parts",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
#else
	hGroupSetItem = m_treeCtrl.InsertItem("组合焊件",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
#endif
	TREEITEM_PARTGROUP_INFO *pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_SET,NULL));
	m_treeCtrl.SetItemData(hGroupSetItem,(DWORD)pItemInfo);
	for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{	//重新生成列表前 先清除旧的组信息项
		if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_FOLDER)
			Ta.PartGroup.DeleteCursor();
	}
	Ta.PartGroup.Clean();
	for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		CString sFolderName=pPartGroup->sFolderName;
		for(pItemInfo=itemInfoList.GetFirst();pItemInfo;pItemInfo=itemInfoList.GetNext())
		{	//查找是否已经存在相同的组合焊件文件夹
			if(pItemInfo->itemType==PARTGROUP_FOLDER&&
				sFolderName.CompareNoCase(((CLDSPartGroup*)pItemInfo->pObj)->sFolderName)==0)
				break;
		}
		if(pItemInfo)
		{	//若存在，则将属于本文件夹的组合焊件做为该组的子项
			HTREEITEM hItem=m_treeCtrl.GetRootItem();
			hItem=m_treeCtrl.GetChildItem(hItem);
			while(hItem)
			{
				TREEITEM_PARTGROUP_INFO *pItem=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
				if(pItem&&pItem==pItemInfo)
				{
					hFolderItem=hItem;
					break;
				}
				hItem=m_treeCtrl.GetNextItem(hItem,TVGN_NEXT);
			}
		}
		else if(sFolderName.GetLength()>0)
		{	//若不存在，新建组
			Ta.PartGroup.push_stack();
			//每个组在组合焊件列表中都对应一个组合焊件，但这类组合焊件仅仅用来临时保存组名
			//如果仅靠属于该组的组合焊件来定位组,组成员被清空时，无法对该组进行操作
			CLDSPartGroup *pFolder = Ta.PartGroup.append();
			pFolder->m_iPartGroupType=CLDSPartGroup::GROUP_FOLDER;	//新建组合焊件组时使用 不存储
			strcpy(pFolder->sFolderName,pPartGroup->sFolderName);
			Ta.PartGroup.pop_stack();
			hFolderItem = m_treeCtrl.InsertItem(pFolder->sFolderName,PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hGroupSetItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_FOLDER,pFolder));
			m_treeCtrl.SetItemData(hFolderItem,(DWORD)pItemInfo);
		}
		if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_FOLDER)
			continue;
		if(hFolderItem)
			hPartGroupItem = m_treeCtrl.InsertItem(pPartGroup->sGroupName,PRJ_IMG_PARTGROUP,PRJ_IMG_PARTGROUP,hFolderItem);
		else
			hPartGroupItem = m_treeCtrl.InsertItem(pPartGroup->sGroupName,PRJ_IMG_PARTGROUP,PRJ_IMG_PARTGROUP,hGroupSetItem);
		pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_CASE,pPartGroup,pPartGroup->handle));
		m_treeCtrl.SetItemData(hPartGroupItem,(DWORD)pItemInfo);
		RefreshPartGroupTreeNode(hPartGroupItem,pPartGroup,false);
		/* 以下代码与函数RefreshPartGroupTreeNode()功能相同
		HTREEITEM hViewItem,hLeafItem;
		CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
		if(pRootPart)
			ss.Format("基构件=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
		else
			ss.Format("基构件=(0X%X)",pPartGroup->handle);
		hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
		pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_ROOTPART,pPartGroup));
		m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);

		ss.Format("单基数=%d件",pPartGroup->m_uStatMatCoef);
		hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
		pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_STATNUM,NULL));
		m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);

		hViewItem=m_treeCtrl.InsertItem("附加视图",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
		pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWSET,NULL));
		m_treeCtrl.SetItemData(hViewItem,(DWORD)pItemInfo);
		for(CSectionView *pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
		{
			hLeafItem=m_treeCtrl.InsertItem(pView->sName,PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hViewItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWCASE,pPartGroup,pPartGroup->handle));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
		}
		*/
	}
	//展开组合焊件树
	m_treeCtrl.Expand(hGroupSetItem,TVE_EXPAND);
	//排序
	HTREEITEM hItem=m_treeCtrl.GetRootItem();
	m_treeCtrl.SortChildren(hItem);
	hItem=m_treeCtrl.GetChildItem(hItem);
	while(hItem)
	{
		TREEITEM_PARTGROUP_INFO *pItem=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
		if(pItem&&pItem->itemType==PARTGROUP_FOLDER)
			m_treeCtrl.SortChildren(hItem);
		hItem=m_treeCtrl.GetNextItem(hItem,TVGN_NEXT);
	}
	Ta.m_bDataModified=bTowerDataModified;
}
void CTowerPartGroupDlg::ContextMenu(CWnd *pWnd, CPoint point)
{
	CPoint scr_point = point;
	m_treeCtrl.ClientToScreen(&scr_point);
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType==PARTGROUP_SET)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_STAT_PART_GROUP_AUTO,"Automatic statistics");
		pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_WELDLINE,"Intelligently recognition of weld seam");
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_PART_GROUP,"Update weld Part groups");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_SORT_PARTGROUP,"classification and ordination");
		pMenu->AppendMenu(MF_STRING,ID_CHECK_PARTGROUP_VALID,"check combined weld parts");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP,"add combined weld parts");
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP_FOLDER,"combined weld parts groups");
#else
		pMenu->AppendMenu(MF_STRING,ID_STAT_PART_GROUP_AUTO,"自动统计");
		pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_WELDLINE,"智能识别焊缝");
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_PART_GROUP,"更新组焊件");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_SORT_PARTGROUP,"分类排序");
		pMenu->AppendMenu(MF_STRING,ID_CHECK_PARTGROUP_VALID,"校核组焊件");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP,"添加组焊件");
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP_FOLDER,"添加组焊件组");
#endif
	}
	else if(pItemInfo==NULL||pItemInfo->itemType==PARTGROUP_FOLDER)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_SORT_PARTGROUP,"sort");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"edit");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"delete");
#else
		pMenu->AppendMenu(MF_STRING,ID_SORT_PARTGROUP,"排序");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"编辑");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"删除");
#endif
	} 
	else if(pItemInfo->itemType==PARTGROUP_CASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_PARTGROUP,"activate");
		pMenu->AppendMenu(MF_STRING,ID_CHECK_PARTGROUP_VALID,"check");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"edit");
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_PART_GROUP,"statistics and extraction");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"delete");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"clone and newly creat");
#else
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_PARTGROUP,"激活");
		pMenu->AppendMenu(MF_STRING,ID_CHECK_PARTGROUP_VALID,"校核");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"编辑");
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_PART_GROUP,"统计提取");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"删除");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"克隆并新建");
#endif
	}
	else if(pItemInfo->itemType==PARTGROUP_VIEWSET)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP,"add");
		pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_PARTGROUP_VIEWS,"automatically extract");
#else
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP,"添加");
		pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_PARTGROUP_VIEWS,"自动提取");
#endif
	}
	else if(pItemInfo->itemType==PARTGROUP_VIEWCASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_PARTGROUP,"activate");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"edit");
#else
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_PARTGROUP,"激活");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"编辑");
#endif
		//目前不支持在建模环境中对标定点检测线以及标注角的操作 wht 10-06-22
		//pMenu->AppendMenu(MF_STRING,ID_PARTGROUP_LOCATE_POINT,"标定点");
		//pMenu->AppendMenu(MF_STRING,ID_PARTGROUP_CHECK_LINE,"检测线");
		//pMenu->AppendMenu(MF_STRING,ID_PARTGROUP_ANGULAR,"标注角");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"delete");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"clone and newly creat");
#else
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"删除");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"克隆并新建");
#endif
	}
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

void CTowerPartGroupDlg::OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	m_treeCtrl.ScreenToClient(&HitTestInfo.pt);
	m_treeCtrl.HitTest(&HitTestInfo);
	m_treeCtrl.Select(HitTestInfo.hItem,TVGN_CARET);

	ContextMenu(this,HitTestInfo.pt);

	*pResult = 0;
}

static int CALLBACK ItemCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CTowerTreeCtrl* pTreeCtrl = (CTowerTreeCtrl*)lParamSort;
	TREEITEM_PARTGROUP_INFO *pItemInfo1=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData((HTREEITEM)lParam1);
	TREEITEM_PARTGROUP_INFO *pItemInfo2=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData((HTREEITEM)lParam2);
	CString sFolderName;
	if(pItemInfo1)
		sFolderName=((CLDSPartGroup*)pItemInfo1->pObj)->sFolderName;
	if(pItemInfo2)
		return sFolderName.CompareNoCase(((CLDSPartGroup*)pItemInfo2->pObj)->sFolderName);
	return 0;
}

void CTowerPartGroupDlg::OnSmartRetrieveWeldline()
{
	CLogErrorLife life(&logerr);
	Ta.RetrieveWeldLineInfo();
}
HTREEITEM CTowerPartGroupDlg::SearchPartGroupItem(long hPart,HTREEITEM hStartItem,UINT nCode/*=0*/)
{
	HTREEITEM hItem,hFindedItem=NULL;
	if(nCode==TVGN_NEXT)
	{
		hItem=m_treeCtrl.GetNextSiblingItem(hStartItem);
		if(hItem!=NULL)
		{
			TREEITEM_PARTGROUP_INFO* pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
			if(pItemInfo!=NULL&&pItemInfo->itemType==PARTGROUP_CASE&&pItemInfo->handle==hPart)
				return hItem;
			if((hFindedItem=SearchPartGroupItem(hPart,hStartItem,TVGN_CHILD))!=NULL)
				return hFindedItem;
			else if((hFindedItem=SearchPartGroupItem(hPart,hStartItem,TVGN_NEXT))!=NULL)
				return hFindedItem;
		}
	}
	else if(nCode==TVGN_CHILD)
	{
		hItem=m_treeCtrl.GetNextItem(hStartItem,TVGN_CHILD);
		while(hItem!=NULL)
		{
			TREEITEM_PARTGROUP_INFO* pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
			if(pItemInfo!=NULL&&pItemInfo->itemType==PARTGROUP_CASE&&pItemInfo->handle==hPart)
				return hItem;
			if((hFindedItem=SearchPartGroupItem(hPart,hItem,TVGN_CHILD))!=NULL)
				return hFindedItem;
			hItem=m_treeCtrl.GetNextSiblingItem(hItem);
		}
	}
	return NULL;
}
void CTowerPartGroupDlg::OnStatPartGroupAuto() 
{	//清空原有组焊件列表 重新统计组焊件
	CString segIncStr="*";
#ifdef AFX_TARG_ENU_ENGLISH
	if(Input("segment number range:",&segIncStr,'s')<=0)
#else
	if(Input("段号范围：",&segIncStr,'s')<=0)
#endif
		return;
	//分解段号字符串
	CHashList<SEGI>hashSegTbl;
	if(segIncStr.GetLength()>0&&segIncStr.Find('*',0)<0)	//指定段号范围
		GetSegNoHashTblBySegStr(segIncStr,hashSegTbl);
	if(segIncStr.Find('*',0)>=0)
	{
		Ta.PartGroup.Empty();
		itemInfoList.Empty();
		//必须与itemInfoList跟进一致，否则可能会导致树节点附加信息指针为空导致异常死机
		m_treeCtrl.DeleteAllItems();
	}
	else if(hashSegTbl.GetNodeNum()>0)
	{
		for(CLDSPartGroup* pGroup=Ta.PartGroup.GetFirst();pGroup;pGroup=Ta.PartGroup.GetNext())
		{
			CLDSPart* pPart=Ta.Parts.FromHandle(pGroup->handle);
			if(pPart==NULL||hashSegTbl.GetValue(pPart->iSeg)==NULL)
				continue;
			HTREEITEM hItem=SearchPartGroupItem(pGroup->handle,m_treeCtrl.GetRootItem(),TVGN_CHILD);
			if(hItem!=NULL)
			{
				TREEITEM_PARTGROUP_INFO *pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
				for(TREEITEM_PARTGROUP_INFO* pInfo=itemInfoList.GetFirst();pInfo;pInfo=itemInfoList.GetNext())
				{
					if(pInfo==pItemInfo)
					{
						itemInfoList.DeleteCursor();
						break;
					}
				}
				m_treeCtrl.DeleteItem(hItem);
			}
			Ta.PartGroup.DeleteCursor();
		}
	}
	RefreshPartGroup(segIncStr);	
}

void CTowerPartGroupDlg::OnRefreshPartGroup()
{
	RefreshPartGroup(NULL);
}
void CTowerPartGroupDlg::RefreshPartGroup(const char* incSegStr/*=NULL*/)
{	//刷新组焊件列表
	CLogErrorLife logErrorLife;
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(m_treeCtrl.GetSelectedCount()>0)
	{
		HTREEITEM hSelItem=m_treeCtrl.GetSelectedItem();
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hSelItem);
		if(pItemInfo==NULL)
			return;
		if(pItemInfo->itemType==PARTGROUP_CASE)
		{	//刷新指定组焊件
			PARTSET partset;
			CLDSPartGroup *pPartGroup=Ta.PartGroup.FromHandle(pItemInfo->handle);
			if(pPartGroup==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X combined weld parts lost its parent parts!",pItemInfo->handle);
#else
				logerr.Log("0X%X组焊件的母构件已丢失!",pItemInfo->handle);
#endif
				return;
			}
			CLDSPart* pRootPart=Ta.Parts.FromHandle(pPartGroup->handle);
			if(pRootPart)
				strcpy(pPartGroup->sRootPartNo,pRootPart->GetPartNo());
			else
				strcpy(pPartGroup->sRootPartNo,"");
			Ta.GetPartGroupPartSet(pPartGroup->handle,partset);
			pPartGroup->SonViews.Empty();
			pPartGroup->AddDefaultAttachViews();
			/*
			for(CSectionView *pSectionView=pPartGroup->SonViews.GetFirst();pSectionView;pSectionView=pPartGroup->SonViews.GetNext())
			{
				if(pSectionView->m_bLocked)
					continue;	//视图已锁定 不需要更新视图
				pSectionView->SonParts.Empty();
				CSectionView::PART_ITEM *pPartItem = NULL;
				pPartItem = pSectionView->SonParts.Add(pPartGroup->handle);
				pPartItem->cFlag = 0;
				//更新视图构件集合
				for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				{
					pPartItem = pSectionView->SonParts.Add(pPart->handle);
					pPartItem->cFlag = 0;
				}
			}
			*/
			RefreshPartGroupTreeNode(hSelItem,pPartGroup,true);
			return;
		}
	}
	CString segIncStr="*";
	if(incSegStr!=NULL)
		segIncStr=incSegStr;
	else
	{
#ifdef AFX_TARG_ENU_ENGLISH
	if(Input("segment number range:",&segIncStr,'s')<=0)
#else
	if(Input("段号范围：",&segIncStr,'s')<=0)
#endif
		return;
	}
	//分解段号字符串
	CHashList<SEGI>hashSegTbl;
	if(segIncStr.GetLength()>0&&segIncStr.Find('*',0)<0)	//指定段号范围
	{
		if(GetSegNoHashTblBySegStr(segIncStr,hashSegTbl)==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("segment number cannot be empty,please confirm the segment number of combined weld parts!");
#else
			logerr.Log("段号不能为空，请确定组焊件段号！");
#endif
			return;
		}
	}

	Ta.RefreshPartGroups(hashSegTbl,TRUE);
	CLDSPartGroup *pPartGroup=NULL;
	int nCurProcess=0,nSumProcess=Ta.PartGroup.GetNodeNum();
#ifdef AFX_TARG_ENU_ENGLISH
	Ta.DisplayProcess(0,"combined weld parts statistics");
#else
	Ta.DisplayProcess(0,"统计组焊件");
#endif
	for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		nCurProcess++;
#ifdef AFX_TARG_ENU_ENGLISH
		Ta.DisplayProcess(ftoi(100*nCurProcess/nSumProcess),"combined weld parts statistics");
#else
		Ta.DisplayProcess(ftoi(100*nCurProcess/nSumProcess),"统计组焊件");
#endif
		CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
		if(pRootPart==NULL||(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pRootPart->iSeg.iSeg)==NULL))
			continue;	//根构件不在当前指定段号范围内，跳过不进行分析统计
		BOOL bRetOk=FALSE;
		for(TREEITEM_PARTGROUP_INFO *pInfo=itemInfoList.GetFirst();pInfo;pInfo=itemInfoList.GetNext())
		{	//根据组焊件句柄查找组焊件，使用pInfo->pObj==pPartGroup查找，在重新统计组焊件后失效，因为重新分配了内存
			if(pInfo->itemType==PARTGROUP_CASE&&pInfo->handle==pPartGroup->handle)
			{	//组焊件树中已存在该组合焊件
				bRetOk=TRUE; 
				break;
			}
		}
		if(bRetOk)
		{	//更新已存在的组焊件项的附加视图
			PARTSET partset;
			Ta.GetPartGroupPartSet(pPartGroup->handle,partset);
			if(pPartGroup->SonViews.GetNodeNum()<=0)
				pPartGroup->AddDefaultAttachViews();
			//CLDSPart *pDatumPart=console.FromPartHandle(pPartGroup->handle);
			/**实践证明CSectionView::m_bLocked没有实际使用价值，用户很少设定，
			  *而且对于一些需要过滤子构件的视图还会起反作用 wjh-2018.3.6
			for(CSectionView *pSectionView=pPartGroup->SonViews.GetFirst();pSectionView;pSectionView=pPartGroup->SonViews.GetNext())
			{
				if(pSectionView->m_bLocked)
					continue;	//视图已锁定 不需要更新视图
				pSectionView->SonParts.Empty();
				CSectionView::PART_ITEM *pPartItem = NULL;
				pPartItem = pSectionView->SonParts.Add(pPartGroup->handle);
				pPartItem->cFlag = 0;
				//更新视图构件集合
				for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				{
					pPartItem = pSectionView->SonParts.Add(pPart->handle);
					pPartItem->cFlag = 0;
				}
				//更新附加视图的标定点，检测线，标注角	未完成.............
				//pSectionView->LocatePoints.Empty();
				//pSectionView->CheckLines.Empty();
				//pSectionView->Angulars.Empty();
				//pSectionView->AddDefaultLocatePoints(pDatumPart,pPartGroup->SuperiorTower());	//添加默认的标定点
				//pSectionView->AddDefaultCheckLines(pDatumPart,pPartGroup->SuperiorTower());		//添加默认的检测线
				//pSectionView->AddDefaultAngulars(pDatumPart,pPartGroup->SuperiorTower());		//添加默认的角度标注线
			}
			*/
		}
		else if(pPartGroup->m_iPartGroupType!=CLDSPartGroup::GROUP_FOLDER)//添加新的组焊项的附加视图
			pPartGroup->AddDefaultAttachViews();
	}
#ifdef AFX_TARG_ENU_ENGLISH
	Ta.DisplayProcess(100,"combined weld parts statistics");
#else
	Ta.DisplayProcess(100,"统计组焊件");
#endif
	RefreshModel();
}

void CTowerPartGroupDlg::OnSortPartGroup() 
{		
	HTREEITEM hSelItem=m_treeCtrl.GetSelectedItem();
	if(hSelItem)
		m_treeCtrl.SortChildren(hSelItem);
	/*
	// Sort the tree control's items using callback procedure.
	TVSORTCB tvs;
	tvs.hParent=TVI_ROOT;
	tvs.lpfnCompare=ItemCompareProc;
	tvs.lParam=(LPARAM)&m_treeCtrl;
	m_treeCtrl.SortChildrenCB(&tvs);
	*/
}


void CTowerPartGroupDlg::DeletePartGroup(HTREEITEM hSelItem) 
{
	TREEITEM_PARTGROUP_INFO *pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hSelItem);
	if(pItemInfo==NULL)
		return;
	BOOL bDeleteOK=FALSE;
	CString sFolderName;
	if(pItemInfo->itemType==PARTGROUP_CASE)
	{	//删除组合焊件
		for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
			{
				//Ta.PartGroup.DeleteCursor(TRUE);
				Ta.PartGroup.DeleteNode(pPartGroup->handle);
				bDeleteOK=TRUE;
				break;
			}
		}
	}
	else if(pItemInfo->itemType==PARTGROUP_VIEWCASE)
	{	//删除组合焊件附加视图
		CLDSPartGroup* pPartGroup = NULL;
		CSectionView *pView=pItemInfo->GetVerifiedSectionView(&pPartGroup);
		bDeleteOK=TRUE;
		if(pView!=NULL&&pView->m_iViewType==VIEW_MAIN)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("No permission to delete main view !");
#else
			AfxMessageBox("不允许删除主视图!");
#endif
			return;
		}
		else if(pView!=NULL)
			pPartGroup->SonViews.DeleteNode(pView->GetId());
	}
	else if(pItemInfo->itemType==PARTGROUP_FOLDER)
	{	//删除组合焊件组 以及属于该组的组合焊件
		if(pItemInfo->pObj)
			sFolderName=((CLDSPartGroup*)pItemInfo->pObj)->sFolderName;
		for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(sFolderName.CompareNoCase(pPartGroup->sFolderName)==0)
			{
				//Ta.PartGroup.DeleteCursor(TRUE);
				Ta.PartGroup.DeleteNode(pPartGroup->handle);
				bDeleteOK=TRUE;
			}
		}
	}
	
	if(bDeleteOK)
	{
		for(TREEITEM_PARTGROUP_INFO *pInfo=itemInfoList.GetFirst();pInfo;pInfo=itemInfoList.GetNext())
		{
			if(pInfo->pObj==NULL)
				continue;
			if(pItemInfo->itemType==PARTGROUP_FOLDER)
			{	//删除组合焊件组以及组合焊件组子节点相关项
				if(sFolderName.CompareNoCase(((CLDSPartGroup*)pInfo->pObj)->sFolderName)==0)
					itemInfoList.DeleteCursor(TRUE);
			}
			else 
			{
				if(pItemInfo==pInfo)
				{
					itemInfoList.DeleteCursor(TRUE);
					break;
				}
			}
		}
		m_treeCtrl.DeleteItem(hSelItem);	
	}
}
void CTowerPartGroupDlg::OnRetrievePartgroupViews()
{
	CLDSPartGroup *pPartGroup = NULL;
	CSectionView *pCurView = NULL;
	CPartGroupDlg partDlg;
	CFolderNameDlg folderNameDlg;
	CString ss,sName;
	HTREEITEM hParentItem=NULL;
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem)
		hParentItem = m_treeCtrl.GetParentItem(hItem);
	if(hParentItem)
	{
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hParentItem);
		sName = m_treeCtrl.GetItemText(hParentItem);
	}
	CLogErrorLife errlife;
	if(pItemInfo&&pItemInfo->itemType==PARTGROUP_CASE)
	{	
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
				break;
		}
		if(pPartGroup==NULL)
			return;
		pPartGroup->SonViews.Empty();
		pPartGroup->AddDefaultAttachViews();
		//更新附加视图树结点
		RefreshPartGroupViewSetNode(hItem,pPartGroup);
		m_treeCtrl.Expand(hItem,TVE_EXPAND);
	}
}
void CTowerPartGroupDlg::OnDeletePartgroup() 
{	//删除选中项 
	int i=0,nSelectCount=m_treeCtrl.GetSelectedCount();
	HTREEITEM hSelItem=NULL;
	for(i=0;i<nSelectCount;i++)
	{
		hSelItem=m_treeCtrl.GetFirstSelectedItem();
		if(hSelItem)
			DeletePartGroup(hSelItem);
	}
	/*HTREEITEM hSelItem=m_treeCtrl.GetSelectedItem();
	if(hSelItem)
		DeletePartGroup(hSelItem);*/
}

void CTowerPartGroupDlg::OnEditPartgroup() 
{
	CLDSPartGroup *pPartGroup = NULL;
	CSectionView *pCurView = NULL;
	CPartGroupDlg partDlg;
	CFolderNameDlg folderNameDlg;
	CString ss,sName;
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem)
	{
		sName = m_treeCtrl.GetItemText(hItem);
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	}
	if(pItemInfo&&pItemInfo->itemType==PARTGROUP_CASE)
	{	
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
				break;
		}
		if(pPartGroup==NULL)
			return;
		partDlg.m_sPartGroupName.Format("%s",pPartGroup->sGroupName);
		partDlg.m_sFolderName = pPartGroup->sFolderName;
		partDlg.m_uStatNum = pPartGroup->m_uStatMatCoef;
		CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
		if(pRootPart)
			partDlg.m_sRootPartHandle.Format("%X",pPartGroup->handle);
		partDlg.ucs = pPartGroup->ucs;
		partDlg.m_pPartGroup=pPartGroup;
		partDlg.m_bTowerFoot=pPartGroup->m_bFootPartGroup;
		BOOL bOldFootPartGroup=pPartGroup->m_bFootPartGroup;
		if(IDOK==partDlg.DoModal())
		{
			_snprintf(pPartGroup->sGroupName,MAX_TEMP_CHAR_50,"%s",partDlg.m_sPartGroupName);
			_snprintf(pPartGroup->sFolderName,MAX_CHAR_GUIGE_16,"%s",partDlg.m_sFolderName);
			pPartGroup->m_uStatMatCoef = partDlg.m_uStatNum;
			pPartGroup->ucs = partDlg.ucs;
			sscanf(partDlg.m_sRootPartHandle,"%X",&pPartGroup->handle);
			m_treeCtrl.SetItemText(hItem,pPartGroup->sGroupName);
			HTREEITEM hViewSetItem=NULL,hLeafItem=m_treeCtrl.GetChildItem(hItem);
			do{
				pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hLeafItem);
				if(pItemInfo->itemType==PARTGROUP_STATNUM)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					ss.Format("single number=%d pieces",pPartGroup->m_uStatMatCoef);
#else
					ss.Format("单基数=%d件",pPartGroup->m_uStatMatCoef);
#endif
					m_treeCtrl.SetItemText(hLeafItem,ss);
				}
				else if(pItemInfo->itemType==PARTGROUP_ROOTPART)
				{
					CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
					if(pRootPart)
#ifdef AFX_TARG_ENU_ENGLISH
						ss.Format("basic parts=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
					else
						ss.Format("basic parts=(0X%X)",pPartGroup->handle);
#else
						ss.Format("基构件=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
					else
						ss.Format("基构件=(0X%X)",pPartGroup->handle);
#endif
					m_treeCtrl.SetItemText(hLeafItem,ss);
				}
				else if(pItemInfo->itemType==PARTGROUP_VIEWSET)
					hViewSetItem=hLeafItem;	//附加视图集合
				hLeafItem=m_treeCtrl.GetNextSiblingItem(hLeafItem);
			}while(hLeafItem!=NULL);
			pPartGroup->m_bFootPartGroup=partDlg.m_bTowerFoot;	//塔脚板组焊件 wht 10-01-14
			if((partDlg.m_hOldRootPart!=0&&partDlg.m_hOldRootPart!=pPartGroup->handle)	//基准构件句柄发生变化
				||pPartGroup->m_bFootPartGroup!=bOldFootPartGroup)	//或者组件是否为塔脚板的属性发生变化时
			{	//需要更新附加视图
				pPartGroup->SonViews.Empty();
				pPartGroup->AddDefaultAttachViews();//添加默认附加视图
				//更新界面显示 wht 10-01-14
				hLeafItem=m_treeCtrl.GetChildItem(hViewSetItem);
				do{
					m_treeCtrl.DeleteItem(hLeafItem);
					hLeafItem=m_treeCtrl.GetNextItem(hViewSetItem,TVGN_CHILD);
				}while(hLeafItem!=NULL);
				TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
				for(pItemInfo=itemInfoList.GetFirst();pItemInfo;pItemInfo=itemInfoList.GetNext())
				{	//删除原有附加视图相关信息
					if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj&&pItemInfo->itemType==PARTGROUP_VIEWCASE)
						itemInfoList.DeleteCursor();
				}
				//添加新视图到组焊件树
				for(CSectionView *pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
				{
					hLeafItem=m_treeCtrl.InsertItem(pView->sName,PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hViewSetItem);
					pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWCASE,pPartGroup,pView->GetId()));
					m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
				}
			}
		}
	}
	else if(pItemInfo&&pItemInfo->itemType==PARTGROUP_VIEWCASE)
	{	
		CLDSPartGroup* pPartGroup = NULL;
		CSectionView *pCurView=pItemInfo->GetVerifiedSectionView(&pPartGroup);
		if(pCurView)
		{
			CSectionViewPropertyDlg viewDlg;
			viewDlg.m_pWorkPartGroup=pPartGroup;
			viewDlg.m_pWorkView = pCurView;
			if(IDOK==viewDlg.DoModal())
				m_treeCtrl.SetItemText(hItem,pCurView->sName);
		}
	}
	else if(pItemInfo&&pItemInfo->itemType==PARTGROUP_FOLDER)
	{	
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
				break;
		}
		if(pPartGroup==NULL)
			return;
		folderNameDlg.m_nNameMaxLen=16;
		folderNameDlg.m_sFolderName=pPartGroup->sFolderName;
		folderNameDlg.m_pCurPartGroup=pPartGroup;
		folderNameDlg.m_nDlgType = 2;	//组合焊件组名称
		if(IDOK!=folderNameDlg.DoModal())
			return;
		CString sFolderName=pPartGroup->sFolderName;
		for(CLDSPartGroup *pGroup=Ta.PartGroup.GetFirst();pGroup;pGroup=Ta.PartGroup.GetNext())
		{	//组名修改后将同时修改(属于该组的)组合焊件的组名
			if(sFolderName.CompareNoCase(pGroup->sFolderName)==0)
				strcpy(pGroup->sFolderName,folderNameDlg.m_sFolderName);
		}
		_snprintf(pPartGroup->sFolderName,16,"%s",folderNameDlg.m_sFolderName);
		m_treeCtrl.SetItemText(hItem,pPartGroup->sFolderName);
	}
	else
		return;
}

void CTowerPartGroupDlg::DisplaySectionView(CLDSPartGroup *pPartGroup,CSectionView *pSectionView)
{
	for(CDisplayView *pView=Ta.GetFirstDisplayView();pView;pView=Ta.GetNextDisplayView())
	{
		if(pView->m_iViewType==2)
			break;
	}
	if(pView==NULL)	//应该和附加视图相关联，不然每次激活都要添加一个新的视图
		pView = console.AppendView();
	pView->ucs = pSectionView->ucs;
	if(pPartGroup)
		strcpy(pView->folderName,pPartGroup->sGroupName);
	strcpy(pView->name,pSectionView->sName);
	memcpy(&pView->feature,&pSectionView,4);
	pView->m_iViewType = 2;	//组合构件附加视图，该类视图不保存
	pView->EmptyPartSet();
	for(CSectionView::PART_ITEM *pItem=pSectionView->SonParts.GetFirst();pItem;pItem=pSectionView->SonParts.GetNext())
	{	
		CLDSPart *pPart = console.FromPartHandle(pItem->hSonPart);
		if(pPart==NULL)
			continue;
		pView->AppendViewPart(CViewPart(pPart->handle,pPart->GetClassTypeId()));
		CLsRefList *pLsRefList=NULL;
		double L=1000000000;	//预设一个较大值
		CLDSParamPlate *pParamPlate=NULL;
		if(pPart->GetClassTypeId()==CLS_PLATE)
			pLsRefList=((CLDSPlate*)pPart)->GetLsRefList();
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			pParamPlate=(CLDSParamPlate*)pPart;
			pLsRefList=pParamPlate->GetLsRefList();
			pParamPlate->GetDesignItemValue('L',&L);
		}
		else if(pPart->IsLinePart())
			pLsRefList=((CLDSLinePart*)pPart)->GetLsRefList();
		if(pLsRefList)
		{	//将构件上的螺栓暂时显示出来 方便添加标注线
			for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
				{	//不显示不在基板范围内的螺栓 wht 10-08-06
					f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
					coord_trans(ls_pos,pParamPlate->ucs,FALSE);
					if(ls_pos.x>L)
						continue;
				}
				pView->AppendViewPart(CViewPart(pLsRef->GetLsPtr()->handle,CLS_BOLT));
			}
		}
	}
	console.SetActiveView(pView->handle);
	pView->SelectDispSet(console.DispNodeSet,console.DispPartSet,console.DispBlockRefSet,console.DispFoundationSet);
	g_pSolidDraw->BuildDisplayList(theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView)));
	g_pSolidOper->ZoomAll(0.95);
	g_pSolidOper->ReSetView();
}

void CTowerPartGroupDlg::OnSetActivePartgroup()
{
	PARTSET partset;
	CString ss,sName;
	CSectionView *pCurView = NULL;
	CLDSPartGroup *pPartGroup = NULL;
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem)
	{
		sName=m_treeCtrl.GetItemText(hItem);
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	}
	//切换到实体显示模式
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	if(pItemInfo&&pItemInfo->itemType==PARTGROUP_CASE)
	{	
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
				break;
		}
		if(pPartGroup)
		{
			Ta.GetPartGroupPartSet(pPartGroup->handle,partset);
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pPartGroup->handle);
			ARRAY_LIST<long> id_arr;
			for(CLDSPart *pPart=partset.GetFirst(); pPart; pPart=partset.GetNext())
				id_arr.append(pPart->handle);
			g_pSolidDraw->SetEntSnapStatus(id_arr.m_pData,id_arr.GetSize(),TRUE);
		}
	}
	else if(pItemInfo&&pItemInfo->itemType==PARTGROUP_VIEWCASE)
	{
		CLDSPartGroup* pPartGroup = NULL;
		CSectionView *pCurView=pItemInfo->GetVerifiedSectionView(&pPartGroup);
		if(pCurView)
			DisplaySectionView(pPartGroup,pCurView);
	}
}

void CTowerPartGroupDlg::OnNewPartgroup() 
{
	CLDSPartGroup *pPartGroup=NULL;
	CPartGroupDlg partDlg;
	CString ss;
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem==NULL)	//空白处点击鼠标右键
		hItem=m_treeCtrl.GetRootItem();
	if(hItem)
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType==PARTGROUP_SET)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		partDlg.m_sPartGroupName = "newly created combined weld parts";
#else
		partDlg.m_sPartGroupName = "新建组合焊件";
#endif
		partDlg.m_uStatNum = 0;
		partDlg.m_sRootPartHandle = _T("0X0");
		if(IDOK==partDlg.DoModal())
		{
			CLDSPartGroup *pPartGroup = Ta.PartGroup.append();	
			sprintf(pPartGroup->sGroupName,"%s",partDlg.m_sPartGroupName);
			sprintf(pPartGroup->sFolderName,"%s",partDlg.m_sFolderName);
			pPartGroup->m_uStatMatCoef = partDlg.m_uStatNum;
			sscanf(partDlg.m_sRootPartHandle,"%X",&pPartGroup->handle);
			pPartGroup->ucs = partDlg.ucs;
			//Insert new CLDSPartGroup item
			HTREEITEM hPartGroupItem=NULL,hFolderItem=NULL,hChildItem=m_treeCtrl.GetChildItem(hItem);
			CString sFolderName=pPartGroup->sFolderName;
			while(hChildItem&&sFolderName.GetLength()>0)
			{
				TREEITEM_PARTGROUP_INFO *pChildItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hChildItem);
				CLDSPartGroup *pChildPartGroup=(CLDSPartGroup*)pChildItemInfo->pObj;
				if(pChildItemInfo&&pChildPartGroup&&pChildItemInfo->itemType==PARTGROUP_FOLDER
					&&sFolderName.CompareNoCase(pChildPartGroup->sFolderName)==0)
				{	//已存在新建的组合焊件所属组，则直接添加到相应组中
					hFolderItem=hChildItem;
					m_treeCtrl.Expand(hFolderItem,TVE_EXPAND);	//展开组
					break;
				}
				hChildItem=m_treeCtrl.GetNextSiblingItem(hChildItem);
			}
			if(hFolderItem)
				hPartGroupItem= m_treeCtrl.InsertItem(partDlg.m_sPartGroupName,PRJ_IMG_PARTGROUP,PRJ_IMG_PARTGROUP,hFolderItem);
			else
				hPartGroupItem= m_treeCtrl.InsertItem(partDlg.m_sPartGroupName,PRJ_IMG_PARTGROUP,PRJ_IMG_PARTGROUP,hItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_CASE,pPartGroup));
			m_treeCtrl.SetItemData(hPartGroupItem,(DWORD)pItemInfo);
			CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
			if(pRootPart)
#ifdef AFX_TARG_ENU_ENGLISH
				ss.Format("basic parts=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
			else
				ss.Format("basic parts=(0X%X)",pPartGroup->handle);
#else
				ss.Format("基构件=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
			else
				ss.Format("基构件=(0X%X)",pPartGroup->handle);
#endif
			HTREEITEM hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_ROOTPART,pPartGroup));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("single number=%d pieces",pPartGroup->m_uStatMatCoef);
#else
			ss.Format("单基数=%d件",pPartGroup->m_uStatMatCoef);
#endif
			hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_STATNUM,NULL));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			
#ifdef AFX_TARG_ENU_ENGLISH
			HTREEITEM hViewItem=m_treeCtrl.InsertItem("additional view",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#else
			HTREEITEM hViewItem=m_treeCtrl.InsertItem("附加视图",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#endif
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWSET,NULL));
			m_treeCtrl.SetItemData(hViewItem,(DWORD)pItemInfo);
			for(CSectionView *pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
			{
				hLeafItem=m_treeCtrl.InsertItem(pView->sName,PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hViewItem);
				pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWCASE,pPartGroup,pView->GetId()));
				m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			}
		}
	}
	else if(pItemInfo==NULL||pItemInfo->itemType==PARTGROUP_VIEWSET)
	{
		HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hItem);
		if(hParentItem)
			pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hParentItem);
		if(!pItemInfo)
			return;
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
				break;
		}
		if(pPartGroup==NULL)
			return;
		
		CSectionViewPropertyDlg viewDlg;
		CSectionView tempView;
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(tempView.sName,"newly-created view");
#else
		strcpy(tempView.sName,"新建视图");
#endif
		viewDlg.m_pWorkPartGroup = pPartGroup;
		viewDlg.m_pWorkView = &tempView;
		if(IDOK==viewDlg.DoModal())
		{
			//TODO: 这样改还存在隐患将来应在派生新视图类时，一并完善 wjh-2014.3.29
			//CSectionView *pSectionView = pPartGroup->SonViews.append();
			CSectionView *pSectionView = pPartGroup->SonViews.Add(0,CSectionView::CLS_BASIC_VIEW);
			pSectionView->CopyProperty(&tempView);	
			//Insert new CSectionView item
			HTREEITEM hLeafItem=m_treeCtrl.InsertItem(pSectionView->sName,PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWCASE,pPartGroup,pSectionView->GetId()));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
		}
	} 
}

void CTowerPartGroupDlg::OnCopyAndNewItem()
{
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL,*pParentItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	if(hItem==NULL||pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==PARTGROUP_CASE)
	{	//克隆并新建组焊件 wht 10-11-08
		CLDSPartGroup *pPartGroup=NULL;
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
				break;
		}
		if(pPartGroup==NULL)
			return;
		HTREEITEM hParentItem=m_treeCtrl.GetParentItem(hItem);	//父节点
		if(hParentItem==NULL)
			return;
		//
		CPartGroupDlg partDlg;
		partDlg.m_bCopyAndNew=TRUE;	//通过克隆当前组焊件生成新的组焊件
		partDlg.m_sPartGroupName.Format("%s",pPartGroup->sGroupName);
		partDlg.m_sFolderName = pPartGroup->sFolderName;
		partDlg.m_uStatNum = pPartGroup->m_uStatMatCoef;
		CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
		if(pRootPart)
			partDlg.m_sRootPartHandle.Format("%X",pPartGroup->handle);
		partDlg.ucs = pPartGroup->ucs;
		partDlg.m_pPartGroup=pPartGroup;
		partDlg.m_bTowerFoot=pPartGroup->m_bFootPartGroup;
		BOOL bOldFootPartGroup=pPartGroup->m_bFootPartGroup;
		if(IDOK==partDlg.DoModal())
		{	//添加组焊件
			CLDSPartGroup *pNewPartGroup = Ta.PartGroup.append();	
			sprintf(pNewPartGroup->sGroupName,"%s",partDlg.m_sPartGroupName);
			sprintf(pNewPartGroup->sFolderName,"%s",partDlg.m_sFolderName);
			pNewPartGroup->m_uStatMatCoef = partDlg.m_uStatNum;
			sscanf(partDlg.m_sRootPartHandle,"%X",&pNewPartGroup->handle);
			pNewPartGroup->ucs = partDlg.ucs;
			//更新组焊件视图
			if(pNewPartGroup->handle!=pPartGroup->handle)//基准构件句柄发生变化
			{	//需要更新附加视图
				pNewPartGroup->SonViews.Empty();
				pNewPartGroup->AddDefaultAttachViews();//添加默认附加视图
			}
			//更新界面显示
			CString sFolderName=pNewPartGroup->sFolderName;
			HTREEITEM hPartGroupItem=NULL,hFolderItem=NULL,hChildItem=m_treeCtrl.GetChildItem(hParentItem);	
			if(sFolderName.GetLength()>0&&sFolderName.CompareNoCase(pPartGroup->sGroupName)!=0)
			{	//
				while(hChildItem)
				{
					TREEITEM_PARTGROUP_INFO *pChildItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hChildItem);
					CLDSPartGroup *pChildPartGroup=(CLDSPartGroup*)pChildItemInfo->pObj;
					if(pChildItemInfo&&pChildPartGroup&&pChildItemInfo->itemType==PARTGROUP_FOLDER
						&&sFolderName.CompareNoCase(pChildPartGroup->sFolderName)==0)
					{	//已存在新建的组合焊件所属组，则直接添加到相应组中
						hFolderItem=hChildItem;
						m_treeCtrl.Expand(hFolderItem,TVE_EXPAND);	//展开组
						break;
					}
					hChildItem=m_treeCtrl.GetNextSiblingItem(hChildItem);
				}
			}
			else if(sFolderName.GetLength()<=0)
				strcpy(pNewPartGroup->sGroupName,pPartGroup->sGroupName);
			if(hFolderItem) //插入到指定分组下
				hPartGroupItem= m_treeCtrl.InsertItem(partDlg.m_sPartGroupName,PRJ_IMG_PARTGROUP,PRJ_IMG_PARTGROUP,hFolderItem);
			else			//插入到当前分组下
				hPartGroupItem= m_treeCtrl.InsertItem(partDlg.m_sPartGroupName,PRJ_IMG_PARTGROUP,PRJ_IMG_PARTGROUP,hParentItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_CASE,pNewPartGroup));
			m_treeCtrl.SetItemData(hPartGroupItem,(DWORD)pItemInfo);
			CLDSPart *pRootPart=console.FromPartHandle(pNewPartGroup->handle);
			CString ss;
			if(pRootPart)
#ifdef AFX_TARG_ENU_ENGLISH
				ss.Format("basic parts=%s(0X%X)",(char*)pRootPart->GetPartNo(),pNewPartGroup->handle);
			else
				ss.Format("basic parts=(0X%X)",pNewPartGroup->handle);
#else
				ss.Format("基构件=%s(0X%X)",(char*)pRootPart->GetPartNo(),pNewPartGroup->handle);
			else
				ss.Format("基构件=(0X%X)",pNewPartGroup->handle);
#endif
			HTREEITEM hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_ROOTPART,pNewPartGroup));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("single number=%d pieces",pNewPartGroup->m_uStatMatCoef);
#else
			ss.Format("单基数=%d件",pNewPartGroup->m_uStatMatCoef);
#endif
			hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_STATNUM,NULL));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			
#ifdef AFX_TARG_ENU_ENGLISH
			HTREEITEM hViewItem=m_treeCtrl.InsertItem("additional view",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#else
			HTREEITEM hViewItem=m_treeCtrl.InsertItem("附加视图",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#endif
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWSET,NULL));
			m_treeCtrl.SetItemData(hViewItem,(DWORD)pItemInfo);
			for(CSectionView *pView=pNewPartGroup->SonViews.GetFirst();pView;pView=pNewPartGroup->SonViews.GetNext())
			{
				hLeafItem=m_treeCtrl.InsertItem(pView->sName,PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hViewItem);
				pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWCASE,pNewPartGroup,pView->GetId()));
				m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			}
		}
	}
	else if(pItemInfo->itemType==PARTGROUP_VIEWCASE)
	{	//克隆并新建附加视图
		CLDSPartGroup* pPartGroup = NULL;
		CSectionView *pCurView=pItemInfo->GetVerifiedSectionView(&pPartGroup);
		if(pCurView!=NULL)
		{
			CSectionViewPropertyDlg viewDlg;
			viewDlg.m_pWorkPartGroup = pPartGroup;
			viewDlg.m_pSrcView = pCurView;
			if(IDOK==viewDlg.DoModal() && viewDlg.m_pWorkView)
			{	
				pCurView=viewDlg.m_pWorkView;	//克隆新建的视图
				HTREEITEM hParentItem=m_treeCtrl.GetParentItem(hItem);
				HTREEITEM hLeafItem=m_treeCtrl.InsertItem(pCurView->sName,PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hParentItem);
				pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWCASE,pPartGroup,pCurView->GetId()));
				m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			}
		}
	}
}

void CTowerPartGroupDlg::OnDblclkTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString ss,sName;
	CPartGroupDlg partDlg;
	CLDSPartGroup *pPartGroup = NULL;
	HTREEITEM hParentItem=NULL,hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem)
	{	
		sName=m_treeCtrl.GetItemText(hItem);
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	}
	if(pItemInfo&&pItemInfo->itemType==PARTGROUP_ROOTPART)
	{	//修改组焊件基构件句柄
		hParentItem=m_treeCtrl.GetParentItem(hItem);
		if(hParentItem==NULL)
			return;
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
				break;
		}
		if(pPartGroup==NULL)
			return;
		partDlg.m_sPartGroupName.Format("%s",pPartGroup->sGroupName);
		partDlg.m_sFolderName = pPartGroup->sFolderName;
		partDlg.m_uStatNum = pPartGroup->m_uStatMatCoef;
		CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
		if(pRootPart)
			partDlg.m_sRootPartHandle.Format("%X",pPartGroup->handle);
		partDlg.ucs = pPartGroup->ucs;
		partDlg.m_pPartGroup=pPartGroup;
		if(IDOK==partDlg.DoModal())
		{
			_snprintf(pPartGroup->sGroupName,MAX_TEMP_CHAR_50,"%s",partDlg.m_sPartGroupName);
			_snprintf(pPartGroup->sFolderName,MAX_CHAR_GUIGE_16,"%s",partDlg.m_sFolderName);
			pPartGroup->m_uStatMatCoef = partDlg.m_uStatNum;
			pPartGroup->ucs = partDlg.ucs;
			sscanf(partDlg.m_sRootPartHandle,"%X",&pPartGroup->handle);
			m_treeCtrl.SetItemText(hItem,pPartGroup->sGroupName);
			HTREEITEM hLeafItem=m_treeCtrl.GetChildItem(hParentItem);
			do{
				pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hLeafItem);
				if(pItemInfo->itemType==PARTGROUP_STATNUM)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					ss.Format("single number=%d pieces",pPartGroup->m_uStatMatCoef);
#else
					ss.Format("单基数=%d件",pPartGroup->m_uStatMatCoef);
#endif
					m_treeCtrl.SetItemText(hLeafItem,ss);
					break;
				}
				else if(pItemInfo->itemType==PARTGROUP_ROOTPART)
				{
					CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
					if(pRootPart)
#ifdef AFX_TARG_ENU_ENGLISH
						ss.Format("basic parts=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
					else
						ss.Format("basic parts=(0X%X)",pPartGroup->handle);
#else
						ss.Format("基构件=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
					else
						ss.Format("基构件=(0X%X)",pPartGroup->handle);
#endif
					m_treeCtrl.SetItemText(hLeafItem,ss);
					break;
				}
				hLeafItem=m_treeCtrl.GetNextSiblingItem(hLeafItem);
			}while(hLeafItem!=NULL);
			if(partDlg.m_hOldRootPart!=0&&partDlg.m_hOldRootPart!=pPartGroup->handle)
			{	//基准构件句柄发生变化需要更新附加视图
				pPartGroup->SonViews.Empty();
				//添加默认附加视图
				pPartGroup->AddDefaultAttachViews();
			}
		}
	}
	else
		OnSetActivePartgroup();	//激活组合焊件
	*pResult = 0;
}

void CTowerPartGroupDlg::OnNewPartgroupFolder()
{
	CLDSPartGroup *pPartGroup=NULL;
	CFolderNameDlg folderNameDlg;
	folderNameDlg.m_nNameMaxLen=16;
	folderNameDlg.m_nDlgType = 2;	//组合焊件组名称
	CString ss;
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType==PARTGROUP_SET)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		folderNameDlg.m_sFolderName = "newly-created group";
#else
		folderNameDlg.m_sFolderName = "新建组";
#endif
		if(IDOK==folderNameDlg.DoModal())
		{
			CLDSPartGroup *pPartGroup = Ta.PartGroup.append();
			pPartGroup->m_iPartGroupType=CLDSPartGroup::GROUP_FOLDER;	//新建组合焊件组时使用 不存储
			_snprintf(pPartGroup->sFolderName,16,"%s",folderNameDlg.m_sFolderName);
			//Insert new CLDSPartGroup Folder item
			HTREEITEM hPartGroupItem = m_treeCtrl.InsertItem(folderNameDlg.m_sFolderName,PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_FOLDER,pPartGroup));
			m_treeCtrl.SetItemData(hPartGroupItem,(DWORD)pItemInfo);
		}
	}
}

void CTowerPartGroupDlg::OnPartGroupLocatePoint() 
{
	CString sName;
	CSectionView *pCurView = NULL;
	CLDSPartGroup *pPartGroup = NULL;
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem)
	{
		sName=m_treeCtrl.GetItemText(hItem);
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	}
	//切换到实体显示模式
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	if(pItemInfo&&pItemInfo->itemType==PARTGROUP_VIEWCASE)
	{
		CLDSPartGroup* pPartGroup = NULL;
		CSectionView *pCurView=pItemInfo->GetVerifiedSectionView(&pPartGroup);
		if(pCurView)
		{
			CPartGroupLocatePointDlg locate_point_dlg;
			locate_point_dlg.m_pWorkPartGroup=pPartGroup;
			locate_point_dlg.m_pWorkView=pCurView;
			BOOL bRetOk=TRUE;
			CString cmdStr;
			CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
			while(1)
			{
				bRetOk=locate_point_dlg.DoModal();
				if(locate_point_dlg.m_bPauseBreakExit)
				{
					GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL));
					scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
					scr_part_para.cmdStr="please choose the datum parts that the fixed point locates,press enter button to confirm:";
					scr_part_para.cmdErrorFeedbackStr="No proper parts selected ,please choose the datum parts that the fixed point locates again.:";
#else
					scr_part_para.cmdStr="请选择标定点所在基准构件,<回车键>确认:";
					scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件,请重新选择标定点所在基准构件,<回车键>确认:";
#endif
					if(!GetPartsFromScr(scr_part_para,FALSE))
						return ;
					if(scr_part_para.pResultObjsArr[0]&&scr_part_para.pResultObjsArr[0]->IsPart())
						locate_point_dlg.m_sDatumPart.Format("0X%X",scr_part_para.pResultObjsArr[0]->handle);
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("command:","");
#else
					pCmdLine->FillCmdLine("命令:","");
#endif
				}
				else
					break;
			}
			if(!bRetOk)
			{
				pCmdLine->CancelCmdLine();
				return;
			}
		}
	}
}

void CTowerPartGroupDlg::OnPartGroupCheckLine() 
{
	CString sName;
	CSectionView *pCurView = NULL;
	CLDSPartGroup *pPartGroup = NULL;
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	CLDSView *pLDSView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(hItem)
	{
		sName=m_treeCtrl.GetItemText(hItem);
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	}
	//切换到实体显示模式
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	if(pItemInfo&&pItemInfo->itemType==PARTGROUP_VIEWCASE)
	{
		CLDSPartGroup* pPartGroup = NULL;
		CSectionView *pCurView=pItemInfo->GetVerifiedSectionView(&pPartGroup);
		if(pCurView)
		{
			CPartGroupCheckLineDlg check_line_dlg;
			check_line_dlg.m_pWorkPartGroup=pPartGroup;
			check_line_dlg.m_pWorkView=pCurView;
			BOOL bRetOk=TRUE;
			CString cmdStr;
			CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
			CLDSPart *pEndPart=NULL;
			while(1)
			{
				bRetOk=check_line_dlg.DoModal();
				if(check_line_dlg.m_bPauseBreakExit)
				{
					//切换到实体显示模式
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					Invalidate(FALSE);
					g_pSolidDraw->ReleaseSnapStatus();
					if(check_line_dlg.m_bBeforeWeld)
					{	//焊接前检测线
						long *id_arr=NULL;
						CLDSPart *pPart=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
						pCmdLine->FillCmdLine("please choose checking line's starting parts (bolts) before welding, press enter to confirm:","");
#else
						pCmdLine->FillCmdLine("请选择焊前检测线起始构件(螺栓),按回车键确认:","");
#endif
						g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
						pLDSView->OpenWndSel();	//开窗选择螺栓
						if(pCmdLine->GetStrFromCmdLine(cmdStr))
						{
							//根据句柄字符串引入构件
							long *id_arr1=NULL,arr1_len=0;;
							char sText[100]="";
							if(cmdStr.GetLength()>0)
							{
								id_arr1=new long[cmdStr.GetLength()];
								sprintf(sText,"%s",cmdStr);
								for(char* key=strtok(sText,",");key;key=strtok(NULL,","))
								{
									long h;
									sscanf(key,"%X",&h);
									pPart=console.FromPartHandle(h);
									id_arr1[arr1_len]=h;
									arr1_len++;
								}
							}
							int n = g_pSolidSnap->GetLastSelectEnts(id_arr);
							for(int i=0;i<arr1_len+n;i++)
							{
								if(i<arr1_len)
									pPart=console.FromPartHandle(id_arr1[i]);
								else
									pPart=console.FromPartHandle(id_arr[i-arr1_len]);
								if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
								{
									check_line_dlg.boltList.append((CLDSBolt*)pPart);
								}
							}
							if(id_arr1)
								delete []id_arr1;
						}
						else
						{
							pCmdLine->CancelCmdLine();
							return;
						}
						pLDSView->OperOther();	//取消开窗选择
#ifdef AFX_TARG_ENU_ENGLISH
						pCmdLine->FillCmdLine("please choose checking line's ending parts (plates, tubes) before welding:","");
#else
						pCmdLine->FillCmdLine("请选择焊前检测线终止构件(钢板、钢管):","");
#endif
						g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECT_ALL));
						g_pSolidDraw->ReleaseSnapStatus();
						while(1)
						{
							if(pCmdLine->GetStrFromCmdLine(cmdStr))
							{
								long *id_arr;
								if(g_pSolidSnap->GetLastSelectEnts(id_arr)>0)
								{
									pPart=console.FromPartHandle(id_arr[0]);
									if(pPart&&(pPart->GetClassTypeId()==CLS_LINETUBE
										||pPart->GetClassTypeId()==CLS_PLATE
										||pPart->GetClassTypeId()==CLS_PARAMPLATE))
									{
										pEndPart=pPart;
										check_line_dlg.m_sEndPartHandle.Format("0X%X",pPart->handle);
										break;
									}
									else
									{
										g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
										pCmdLine->FillCmdLine("wrong parts selected,please choose checking line's ending parts (plates, tubes) before welding again:","");		
#else
										pCmdLine->FillCmdLine("选择了错误的构件，请重新选择焊前检测线终止构件(钢板、钢管):","");		
#endif
										continue;
									}
								}
							}
							else
							{
								if(g_pSolidSnap)
									g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
								pCmdLine->CancelCmdLine();
								return;
							}
						}
					}
					else
					{	//焊接后检测线
						GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL),0,0,2);
						scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
						scr_part_para.cmdStr="please choose checking line's positioning parts for starting and ending ends, press enter button to confirm:";
						scr_part_para.cmdErrorFeedbackStr="No proper parts selected, please choose checking line's positioning parts for starting and ending ends, press enter button to confirm:";
#else
						scr_part_para.cmdStr="请选择检测线始端定位构件以及终端定位构件,<回车键>确认:";
						scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件,请重新选择检测线的始端定位构件以及终端定位构件,<回车键>确认:";
#endif
						if(!GetPartsFromScr(scr_part_para,FALSE))
							return ;
						if(scr_part_para.nResultObjs>=1&&scr_part_para.pResultObjsArr[0]&&scr_part_para.pResultObjsArr[0]->IsPart())
							check_line_dlg.m_sStartPartHandle.Format("0X%X",scr_part_para.pResultObjsArr[0]->handle);
						if(scr_part_para.nResultObjs>=2&&scr_part_para.pResultObjsArr[1]&&scr_part_para.pResultObjsArr[1]->IsPart())
							check_line_dlg.m_sEndPartHandle.Format("0X%X",scr_part_para.pResultObjsArr[1]->handle);
					}
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("command:","");
#else
					pCmdLine->FillCmdLine("命令:","");
#endif
					if(pEndPart&&check_line_dlg.boltList.GetNodeNum()>0)
					{	//直接将焊接前检测线添加到检测线列表中
						for(CLDSBolt *pBolt=check_line_dlg.boltList.GetFirst();pBolt;pBolt=check_line_dlg.boltList.GetNext())
						{
							CCheckLineDimension *pCheckLine=NULL;
							pCheckLine=pCurView->CheckLines.append();
							pCheckLine->m_bBeforeWeld=TRUE;
							pCheckLine->m_hStartPart=pBolt->handle;
							pCheckLine->m_hEndPart=pEndPart->handle;
							pCheckLine->m_bSpecEndLocatePart=pCheckLine->m_bSpecStartLocatePart=TRUE;
						}
					}
				}
				else
					break;
			}
			if(!bRetOk)
			{
				pCmdLine->CancelCmdLine();
				return;
			}
		}
	}
}

void CTowerPartGroupDlg::OnPartGroupAngular() 
{
	CString sName;
	CSectionView *pCurView = NULL;
	CLDSPartGroup *pPartGroup = NULL;
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem)
	{
		sName=m_treeCtrl.GetItemText(hItem);
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	}
	//切换到实体显示模式
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	Invalidate(FALSE);
	if(pItemInfo&&pItemInfo->itemType==PARTGROUP_VIEWCASE)
	{
		CLDSPartGroup* pPartGroup = NULL;
		CSectionView *pCurView=pItemInfo->GetVerifiedSectionView(&pPartGroup);
		if(pCurView)
		{
			CPartGroupAngularDlg angular_dlg;
			angular_dlg.m_pWorkPartGroup=pPartGroup;
			angular_dlg.m_pWorkView=pCurView;
			BOOL bRetOk=TRUE;
			CString cmdStr;
			CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
			while(1)
			{
				bRetOk=angular_dlg.DoModal();
				if(angular_dlg.m_bPauseBreakExit)
				{
					GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL),0,0,2);
					scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
					scr_part_para.cmdStr="please choose two datum parts which need to mark angle lines, press enter button to confirm:";
					scr_part_para.cmdErrorFeedbackStr="No proper parts selected,please choose two datum parts which need to mark angle lines again, press enter button to confirm:";
#else
					scr_part_para.cmdStr="请选择要标注角度线的两个基准构件,<回车键>确认:";
					scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件,请重新选择要标注角度线的两个基准构件,<回车键>确认:";
#endif
					if(!GetPartsFromScr(scr_part_para,FALSE))
						return ;
					CLDSDbObject *pObj1=scr_part_para.pResultObjsArr[0];
					CLDSDbObject *pObj2=scr_part_para.pResultObjsArr[1];
					if(scr_part_para.nResultObjs==2&&pObj1&&pObj2&&pObj1->IsPart()&&pObj2->IsPart())
					{
						angular_dlg.m_sStartPartHandle.Format("0X%X",pObj1->handle);
						angular_dlg.m_sEndPartHandle.Format("0X%X",pObj2->handle);
						CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(((CLDSPart*)pObj1)->m_hPartWeldParent,CLS_LINETUBE);
						if(pLineTube)
						{
							angular_dlg.m_fDimR=pLineTube->GetDiameter();
							angular_dlg.m_bStartLineIsWeldRoad=FALSE;	//默认基准边位钢管基准边
							angular_dlg.m_bHasWeldRoad=pLineTube->m_bHasWeldRoad;
						}
					}
					else if(scr_part_para.nResultObjs==1&&pObj1&&pObj1->IsPart())
					{	
						angular_dlg.m_sStartPartHandle.Format("0X%X",pObj1->handle);
						angular_dlg.m_sEndPartHandle.Format("0X%X",0);
						CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(((CLDSPart*)pObj1)->m_hPartWeldParent,CLS_LINETUBE);
						if(pLineTube)
						{
							angular_dlg.m_fDimR=pLineTube->GetDiameter();
							angular_dlg.m_bStartLineIsWeldRoad=FALSE;	//默认基准边位钢管基准边
							angular_dlg.m_bHasWeldRoad=pLineTube->m_bHasWeldRoad;
						}
					}
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("command:","");
#else
					pCmdLine->FillCmdLine("命令:","");
#endif
				}
				else
					break;
			}
			if(!bRetOk)
			{
				pCmdLine->CancelCmdLine();
				return;
			}
		}
	}
}

void CTowerPartGroupDlg::OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	if(pTVKeyDown->wVKey==VK_DELETE)
	{
		int i=0,nSelectCount=m_treeCtrl.GetSelectedCount();
		HTREEITEM hSelItem=NULL;
		for(i=0;i<nSelectCount;i++)
		{
			hSelItem=m_treeCtrl.GetFirstSelectedItem();
			if(hSelItem)
				DeletePartGroup(hSelItem);
		}
	}
	*pResult = 0;
}

//检查组焊件的合法性
typedef struct tagPART_POS
{
	CLDSPart *pPart;	//构件
	f3dPoint pos;		//构件位置
	UCS_STRU ucs;		//仅在排序时使用
	tagPART_POS(){pPart=NULL;}
}PART_POS;
typedef struct tagPARTGROUP_INFO
{
	CLDSLineTube *pLineTube;//主管
	UCS_STRU ucs;			//组焊件坐标系
	TUBESET tubeSet;		//存储与pLineTube属于同一组的钢管
	char sError[800];		//用来存储新的组焊件与第一个组焊件不相同的原因
	ARRAY_LIST<PART_POS> partList;	//钢板(插板或法兰)构件列表
	int nPartNum;			//该主管对应组焊件的构件数量
	tagPARTGROUP_INFO(){memset(sError,0,800); pLineTube=NULL; nPartNum=0; }
}PARTGROUP_INFO;
static int ComparePartPos(const PART_POS &pPartPos1,const PART_POS &pPartPos2)
{
	if(pPartPos1.pos.z>pPartPos2.pos.z)
		return 1;
	else if(pPartPos1.pos.z<pPartPos2.pos.z)
		return -1;
	else if(pPartPos1.pPart&&pPartPos2.pPart)
	{	//两构件Z坐标相等时按编号比较
		int nRetCode=ComparePartNoString(pPartPos1.pPart->GetPartNo(),pPartPos2.pPart->GetPartNo());
		if(nRetCode==0)
		{	//Z坐标相等且编号也相同，由于节点板一般都平行于钢管轴线方向，在节点板上任取一点转换到相对坐标系下
			if(pPartPos1.pPart->GetClassTypeId()==CLS_PLATE&&pPartPos2.pPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate1=(CLDSPlate*)pPartPos1.pPart;
				CLDSPlate *pPlate2=(CLDSPlate*)pPartPos2.pPart;
				if(pPlate1->vertex_list.GetNodeNum()>0&&pPlate2->vertex_list.GetNodeNum()>0)
				{
					f3dPoint tempPos1=pPlate1->vertex_list.GetFirst()->vertex;
					coord_trans(tempPos1,pPlate1->ucs,TRUE);
					coord_trans(tempPos1,pPartPos1.ucs,FALSE);
					f3dPoint tempPos2=pPlate2->vertex_list.GetFirst()->vertex;
					coord_trans(tempPos2,pPlate2->ucs,TRUE);
					coord_trans(tempPos2,pPartPos2.ucs,FALSE);
					if(tempPos1.y>tempPos2.y)
						return 1;
					else 
						return -1;
				}
				else 
					return 0;
			}
			else 
				return 0;
		}
		else 
			return nRetCode;
	}
	return 0;
}

//比较两个链表中的构件位置或法线方向是否完全一致
static BOOL ComparePartGroupInfo(PARTGROUP_INFO &pInfo1,PARTGROUP_INFO &pInfo2,char *sError)
{
	PART_POS *pPartPos=NULL,*pTempPos=NULL;
	pTempPos=pInfo2.partList.GetFirst();
	for(pPartPos=pInfo1.partList.GetFirst();pPartPos;pPartPos=pInfo1.partList.GetNext())
	{
		if(pTempPos==NULL)
			break;
		if(pTempPos->pPart==NULL||pPartPos->pPart==NULL)
			break;
		if(pTempPos->pPart->GetClassTypeId()!=pPartPos->pPart->GetClassTypeId())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"{part 0X%X and 0X%X part are of different type!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#else
			sprintf(sError,"{0X%X与0X%X构件类型不同!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
			break;	//构件类型不一致
		}
		//if(pTempPos&&(pTempPos->pos!=pPartPos->pos)
		if(fabs(pTempPos->pos.x-pPartPos->pos.x)>EPS||fabs(pTempPos->pos.y-pPartPos->pos.y)>EPS
			||fabs(pTempPos->pos.z-pPartPos->pos.z)>EPS)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"{part 0X%X and part 0X%X have different locations!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#else
			sprintf(sError,"{0X%X与0X%X构件位置不同!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
			break;	//构件位置不同
		}
		if(stricmp(pPartPos->pPart->GetPartNo(),pTempPos->pPart->GetPartNo())!=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"{part 0X%X(%s) and part 0X%X(%s) have the same location, but their numbers are differnt!}",pTempPos->pPart->handle
#else
			sprintf(sError,"{0X%X(%s)与0X%X(%s)同一位置构件编号不同!}",pTempPos->pPart->handle
#endif
				,(char*)pTempPos->pPart->GetPartNo(),pPartPos->pPart->handle,(char*)pPartPos->pPart->GetPartNo());
			break;	//同一位置构件编号不同
		}
		if(pPartPos->pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{	//参数化钢板
			CLDSParamPlate *pParamPlate1=(CLDSParamPlate*)pPartPos->pPart;
			CLDSParamPlate *pParamPlate2=(CLDSParamPlate*)pTempPos->pPart;
			if(pParamPlate1->m_iParamType!=pParamPlate2->m_iParamType)
				break;
			if(pParamPlate1->m_iParamType==TYPE_ROLLEND)
			{	//判断槽型插板卷边方向是否一致
				double D1=0,D2=0;
				pParamPlate1->GetDesignItemValue('D',&D1);
				pParamPlate2->GetDesignItemValue('D',&D2);
				f3dPoint plate_norm1=pParamPlate1->ucs.axis_z;
				vector_trans(plate_norm1,pPartPos->ucs,FALSE);
				f3dPoint plate_norm2=pParamPlate2->ucs.axis_z;
				vector_trans(plate_norm2,pTempPos->ucs,FALSE);
				if(!(plate_norm1*plate_norm2>EPS_COS&&fabs(D1-D2)<EPS)
					&&!(plate_norm1*plate_norm2<-EPS_COS&&fabs(fabs(D1-D2)-180)<EPS))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"{slot plate 0X%X and 0X%X have different rolling edge direction!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#else
					sprintf(sError,"{槽型插板0X%X与0X%X卷边方向不同!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
					break;	//槽型插板卷边方向不一致
				}
			}
		}
		else if(pPartPos->pPart->GetClassTypeId()==CLS_PLATE)
		{	//普通钢板 比较螺栓位置是否完全一致
			CLDSPlate *pPlate1=(CLDSPlate*)pPartPos->pPart;
			CLDSPlate *pPlate2=(CLDSPlate*)pTempPos->pPart;
			//比较两钢板螺栓数
			if(pPlate1->GetLsCount()!=pPlate2->GetLsCount())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"{Node plate 0X%X and 0X%X have different bolts number!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#else
				sprintf(sError,"{节点板0X%X与0X%X螺栓个数不同!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
				break; //同一位置的两钢板上的螺栓个数不同
			}
			//比较两钢板法线方向是否相同
			f3dPoint plate_norm1=pPlate1->ucs.axis_z;
			f3dPoint plate_norm2=pPlate2->ucs.axis_z;
			vector_trans(plate_norm1,pPartPos->ucs,FALSE);
			vector_trans(plate_norm2,pTempPos->ucs,FALSE);
			double cosa=fabs(plate_norm1*plate_norm2);
			//cos1°=0.99984769515639123915701155881391
			double cos1=0.99984769515639123915701155881391;
			if(cosa<cos1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"{node plate 0X%X and normal 0X%X have different direction,error degree is %.2f°!}"
#else
				sprintf(sError,"{节点板0X%X与0X%X法线(相对)方向不同,误差度数为%.2f°!}"
#endif
					,pTempPos->pPart->handle,pPartPos->pPart->handle,ACOS(cosa)*DEGTORAD_COEF);
				break; //相对法向方向不同
			}
			fPtList boltPosList1,boltPosList2;
			f3dPoint *pPt=NULL;
			CLsRef *pLsRef=NULL;
			for(pLsRef=pPlate1->GetFirstLsRef();pLsRef;pLsRef=pPlate1->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				f3dPoint ls_pos=pBolt->ucs.origin;
				//将螺栓位置转换到钢板坐标系下
				coord_trans(ls_pos,pPlate1->ucs,FALSE);
				ls_pos.z=0; //Z坐标清零
				coord_trans(ls_pos,pPlate1->ucs,TRUE);
				//将螺栓位置转换到pInfo1->ucs坐标系下
				coord_trans(ls_pos,pInfo1.ucs,FALSE);
				for(pPt=boltPosList1.GetFirst();pPt;pPt=boltPosList1.GetNext())
				{
					if(ls_pos.z<pPt->z)
					{	//按Z坐标从小到大排序
						boltPosList1.insert(ls_pos);
						break;
					}
					else if(fabs(ls_pos.z-pPt->z)<EPS)
					{	//Z坐标相等时按X坐标大小排序
						if(ls_pos.x<pPt->x)
						{
							boltPosList1.insert(ls_pos);
							break;
						}
					}
				}
				if(pPt==NULL)
					boltPosList1.append(ls_pos);
			}
			//
			for(pLsRef=pPlate2->GetFirstLsRef();pLsRef;pLsRef=pPlate2->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				f3dPoint ls_pos=pBolt->ucs.origin;
				//将螺栓位置转换到钢板坐标系下
				coord_trans(ls_pos,pPlate2->ucs,FALSE);
				ls_pos.z=0; //Z坐标清零
				coord_trans(ls_pos,pPlate2->ucs,TRUE);
				//将螺栓位置转换到pInfo2->ucs坐标系下
				coord_trans(ls_pos,pInfo2.ucs,FALSE);
				for(pPt=boltPosList2.GetFirst();pPt;pPt=boltPosList2.GetNext())
				{
					if(ls_pos.z<pPt->z)
					{	//按Z坐标从小到大排序
						boltPosList2.insert(ls_pos);
						break;
					}
					else if(fabs(ls_pos.z-pPt->z)<EPS)
					{	//Z坐标相等时按X坐标大小排序
						if(ls_pos.x<pPt->x)
						{
							boltPosList2.insert(ls_pos);
							break;
						}
					}
				}
				if(pPt==NULL)
					boltPosList2.append(ls_pos);
			}
			TRACE("boltList1-------------------\n");
			for(pPt=boltPosList1.GetFirst();pPt;pPt=boltPosList1.GetNext())
				TRACE("%.3f,%.3f,%.3f\n",pPt->x,pPt->y,pPt->z);
			TRACE("boltList2---------------------\n");
			for(pPt=boltPosList2.GetFirst();pPt;pPt=boltPosList2.GetNext())
				TRACE("%.3f,%.3f,%.3f\n",pPt->x,pPt->y,pPt->z);
			f3dPoint *pPt2=boltPosList2.GetFirst();
			for(pPt=boltPosList1.GetFirst();pPt;pPt=boltPosList1.GetNext())
			{
				if(pPt2==NULL)
					break;	
				//if(*pPt!=*pPt2)
				//	break;
				//工程误差取值为 1
				double eps_value=2;
				if(fabs(pPt->x-pPt2->x)>eps_value||fabs(pPt->y-pPt2->y)>eps_value||fabs(pPt->z-pPt2->z)>eps_value)
					break;
				pPt2=boltPosList2.GetNext();
			}
			if(pPt)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"{node plate 0X%X does not match 0X%X bolt's hole location!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#else
				sprintf(sError,"{节点板0X%X与0X%X螺栓孔位置不匹配!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
				break; //两钢板螺栓孔不重合
			}
		}
		pTempPos=pInfo2.partList.GetNext();
	}
	if(pPartPos==NULL)
		return TRUE;	//两组焊件构件位置完全相同
	return FALSE;
}

BOOL CTowerPartGroupDlg::CheckPartGroupValidAndCorrect(CLDSPartGroup *pPartGroup,BOOL bAutoCorrect,char *sError)
{
	if(pPartGroup==NULL)
		return FALSE;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)console.FromPartHandle(pPartGroup->handle,CLS_LINETUBE);
	if(pDatumTube==NULL)
		return FALSE;
	if(strlen(pDatumTube->GetPartNo())<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sError,"{main tube 0X%X number is empty，please confirm!\n}");
#else
		sprintf(sError,"{主管0X%X编号为空，请确认!\n}");
#endif
		return FALSE;
	}
	pDatumTube->BuildUCS();
	PARTSET partSet;
	//srcList	存储除基准钢管外其他钢管对应的PARTGROUP_INFO,作为比较的原始列表
	//destList	存储分类以后的组焊件,srcList中的每个节点都将于destList
	//			列表中的元素逐个匹配，如都不匹配则添加到destList表尾作为一个新的分组

	//1.将基准钢管对应的PARTGROUP_INFO信息添加到destList列表中
	ATOM_LIST<PARTGROUP_INFO> srcList,destList;
	CLDSLineTube *pLineTube=NULL;
	PARTGROUP_INFO *pInfo=destList.append();
	pInfo->pLineTube=pDatumTube;
	pInfo->ucs=pDatumTube->ucs;
	pInfo->ucs.origin=pDatumTube->Start()-pDatumTube->startOdd()*pDatumTube->ucs.axis_z;
	//1.1将节点板、插板以及法兰这些主要构件添加到PARTGROUP_INFO的构件列表中，作为比较对象
	Ta.GetPartGroupPartSet(pDatumTube->handle,partSet);
	pInfo->nPartNum=partSet.GetNodeNum();	//组焊件数量
	CLDSPart *pPart=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		//仅添加节点板或钢管端头连接的构件(插板或法兰)
		if(pPart->GetClassTypeId()!=CLS_PLATE&&pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{	//仅添加钢管端连接构件
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			if(!pParamPlate->IsFL()||!pParamPlate->IsInsertPlate())
				continue;
		}
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{	//仅添加节点板
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(pPlate->designInfo.m_hBaseNode<0x20||pPlate->designInfo.m_hBasePart<0x20)
				continue;
		}
		PART_POS partPos;
		partPos.pPart=pPart;
		partPos.pos=pPart->ucs.origin;
		coord_trans(partPos.pos,pInfo->ucs,FALSE);
		partPos.ucs=pInfo->ucs;
		pInfo->partList.append(partPos);
	}
	//1.2将PARTGROUP_INFO的构件列表按pos排序
	CQuickSort<PART_POS>::QuickSort(pInfo->partList.m_pData,pInfo->partList.GetSize(),ComparePartPos);

	//2.查找构件编号相同的所有组焊件添加到srcList中
	for(pLineTube=(CLDSLineTube*)Ta.Parts.GetFirst(CLS_LINETUBE);pLineTube;pLineTube=(CLDSLineTube*)Ta.Parts.GetNext(CLS_LINETUBE))
	{
		if(stricmp(pLineTube->GetPartNo(),pDatumTube->GetPartNo())!=0)
			continue;
		if(pLineTube->handle==pDatumTube->handle)
			continue;
		pLineTube->BuildUCS();
		pInfo=srcList.append();
		pInfo->pLineTube=pLineTube;
		pInfo->ucs=pLineTube->ucs;
		pInfo->ucs.origin=pLineTube->Start()-pLineTube->startOdd()*pLineTube->ucs.axis_z;
		partSet.Empty();
		//1.1将节点板、插板以及法兰这些主要构件添加到PARTGROUP_INFO的构件列表中，作为比较对象
		Ta.GetPartGroupPartSet(pLineTube->handle,partSet);
		pInfo->nPartNum=partSet.GetNodeNum();	//组焊件数量
		for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
		{
			//仅添加节点板或钢管端头连接的构件(插板或法兰)
			if(pPart->GetClassTypeId()!=CLS_PLATE&&pPart->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{	//仅添加钢管端连接构件
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
				if(!pParamPlate->IsFL()||!pParamPlate->IsInsertPlate())
					continue;
			}
			if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//仅添加节点板
				CLDSPlate *pPlate=(CLDSPlate*)pPart;
				if(pPlate->designInfo.m_hBaseNode<0x20||pPlate->designInfo.m_hBasePart<0x20)
					continue;
			}
			PART_POS partPos;
			partPos.pPart=pPart;
			partPos.pos=pPart->ucs.origin;
			coord_trans(partPos.pos,pInfo->ucs,FALSE);
			partPos.ucs=pInfo->ucs;
			pInfo->partList.append(partPos);
		}
		//2.2将PARTGROUP_INFO的构件列表按pos排序
		CQuickSort<PART_POS>::QuickSort(pInfo->partList.m_pData,pInfo->partList.GetSize(),ComparePartPos);
	}
	//3.将srcList中的节点这个与destList中的节点进行比较然后分类
	for(pInfo=srcList.GetFirst();pInfo;pInfo=srcList.GetNext())
	{
		PARTGROUP_INFO *pTempInfo=NULL;
		if(pInfo->pLineTube==NULL)
			continue;
		int nPart=pInfo->partList.GetSize();
		CXhChar500 sCompareInfo;
		for(pTempInfo=destList.GetFirst();pTempInfo;pTempInfo=destList.GetNext())
		{
			CXhChar100 sSubInfo,sInfo[8];
			//3.0组焊件构件数量不同
			if(pInfo->nPartNum!=pTempInfo->nPartNum)
			{	
#ifdef AFX_TARG_ENU_ENGLISH
				CXhChar100 sTemp("0x%X and 0x%X information compared:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
				sCompareInfo.Append(sTemp);
				sSubInfo.Printf("\t{0X%X(%d pieces) and 0X%X(%d pieces) tube combined weld quantities are different!}\n"
#else
				CXhChar100 sTemp("0x%X与0x%X对比信息:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
				sCompareInfo.Append(sTemp);
				sSubInfo.Printf("\t{0X%X(%d个)与0X%X(%d个)钢管组焊件构件数量不同!}\n"
#endif
					,pInfo->pLineTube->handle,pInfo->nPartNum,pTempInfo->pLineTube->handle,pTempInfo->nPartNum);
				sCompareInfo.Append(sSubInfo);
				continue;
			}
			//3.1组焊件构件关键构件数量不同
			if(nPart!=pTempInfo->partList.GetSize())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				CXhChar100 sTemp("0x%X and 0x%X information compared:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
				sCompareInfo.Append(sTemp);
				sSubInfo.Printf("numbers of \t{0X%X(%d piece)and 0X%X(%d piece)tube combined weld part (node plate, insertion plate and flange) are diffent!}\n"
#else
				CXhChar100 sTemp("0x%X与0x%X对比信息:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
				sCompareInfo.Append(sTemp);
				sSubInfo.Printf("\t{0X%X(%d个)与0X%X(%d个)钢管组焊件关键构件(节点板、插板以及法兰)数量不同!}\n"
#endif
					,pInfo->pLineTube->handle,nPart,pTempInfo->pLineTube->handle,pTempInfo->partList.GetSize());
				sCompareInfo.Append(sSubInfo);
				continue;
			}
			if(pTempInfo->pLineTube==NULL)
				continue;
			//3.2比较两个链表是否完全一致
			PART_POS *pTempPos=NULL;
			PARTGROUP_INFO tempInfo,Info;	//临时的PARTGROUP_INFO
			tempInfo.pLineTube=pTempInfo->pLineTube;
			if(toupper(pDatumTube->layer(1))=='Z'&&pDatumTube->m_bHasWeldRoad)
			{	//存在焊道线的主管，以焊道线方向为X轴方向
				//始终端对齐
				tempInfo.ucs=pTempInfo->ucs;
				pTempInfo->pLineTube->CalWeldRoadAngle(); 
				tempInfo.ucs.axis_x=pTempInfo->pLineTube->nearWeldRoadVec;
				normalize(tempInfo.ucs.axis_x);
				tempInfo.ucs.axis_y=tempInfo.ucs.axis_z^tempInfo.ucs.axis_x;
				normalize(tempInfo.ucs.axis_y);
				tempInfo.ucs.axis_x=tempInfo.ucs.axis_y^tempInfo.ucs.axis_z;
				normalize(tempInfo.ucs.axis_x);
				for(pTempPos=pTempInfo->partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					PART_POS *pNewPos=tempInfo.partList.append(*pTempPos);
					pNewPos->ucs=tempInfo.ucs;
					coord_trans(pNewPos->pos,pTempPos->ucs,TRUE);
					coord_trans(pNewPos->pos,tempInfo.ucs,FALSE);
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				//
				PARTGROUP_INFO Info;	//临时的PARTGROUP_INFO
				Info.pLineTube=pInfo->pLineTube;
				Info.ucs=pInfo->ucs;
				pInfo->pLineTube->CalWeldRoadAngle();
				Info.ucs.axis_x=pInfo->pLineTube->nearWeldRoadVec;
				normalize(Info.ucs.axis_x);
				Info.ucs.axis_y=Info.ucs.axis_z^Info.ucs.axis_x;
				normalize(Info.ucs.axis_y);
				Info.ucs.axis_x=Info.ucs.axis_y^Info.ucs.axis_z;
				normalize(Info.ucs.axis_x);
				for(pTempPos=pInfo->partList.GetFirst();pTempPos;pTempPos=pInfo->partList.GetNext())
				{
					PART_POS *pNewPos=Info.partList.append(*pTempPos);
					pNewPos->ucs=Info.ucs;
					coord_trans(pNewPos->pos,pTempPos->ucs,TRUE);
					coord_trans(pNewPos->pos,Info.ucs,FALSE);
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(Info.partList.m_pData,Info.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(Info,tempInfo,sInfo[0]))
				{
					pTempInfo->tubeSet.append(Info.pLineTube);
					break;
				}
				//始终端颠倒
				tempInfo.ucs=pTempInfo->ucs;
				tempInfo.ucs.axis_z*=-1.0;
				tempInfo.ucs.origin=pTempInfo->pLineTube->End()
					+pTempInfo->pLineTube->ucs.axis_z*pTempInfo->pLineTube->endOdd();
				pTempInfo->pLineTube->CalWeldRoadAngle();
				tempInfo.ucs.axis_x=pTempInfo->pLineTube->nearWeldRoadVec;
				normalize(tempInfo.ucs.axis_x);
				tempInfo.ucs.axis_y=tempInfo.ucs.axis_z^tempInfo.ucs.axis_x;
				normalize(tempInfo.ucs.axis_y);
				tempInfo.ucs.axis_x=tempInfo.ucs.axis_y^tempInfo.ucs.axis_z;
				normalize(tempInfo.ucs.axis_x);
				for(pTempPos=pTempInfo->partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					PART_POS *pNewPos=tempInfo.partList.append(*pTempPos);
					pNewPos->ucs=tempInfo.ucs;
					coord_trans(pNewPos->pos,pTempPos->ucs,TRUE);
					coord_trans(pNewPos->pos,tempInfo.ucs,FALSE);
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				//
				Info.ucs=pInfo->ucs;
				Info.ucs.axis_z*=-1.0;	//Z轴反向
				Info.ucs.origin=pInfo->pLineTube->End()
					+pInfo->pLineTube->ucs.axis_z*pInfo->pLineTube->endOdd();
				Info.pLineTube->CalWeldRoadAngle();
				Info.ucs.axis_x=pInfo->pLineTube->nearWeldRoadVec;
				normalize(Info.ucs.axis_x);
				Info.ucs.axis_y=Info.ucs.axis_z^Info.ucs.axis_x;
				normalize(Info.ucs.axis_y);
				Info.ucs.axis_x=Info.ucs.axis_y^Info.ucs.axis_z;
				normalize(Info.ucs.axis_x);
				for(pTempPos=pInfo->partList.GetFirst();pTempPos;pTempPos=pInfo->partList.GetNext())
				{
					PART_POS *pNewPos=Info.partList.append(*pTempPos);
					pNewPos->ucs=Info.ucs;
					coord_trans(pNewPos->pos,pTempPos->ucs,TRUE);
					coord_trans(pNewPos->pos,Info.ucs,FALSE);
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(Info.partList.m_pData,Info.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(Info,tempInfo,sInfo[5]))
				{
					pTempInfo->tubeSet.append(Info.pLineTube);
					break;
				}
			}
			else
			{
				//3.2.1始终端对齐
				tempInfo.ucs=pTempInfo->ucs;
				for(pTempPos=pTempInfo->partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					PART_POS *pNewPos=tempInfo.partList.append(*pTempPos);
					pNewPos->ucs=tempInfo.ucs;
				}
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[0]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.2始终端对齐，然后绕Z轴翻转90°//X轴=Y轴和Y轴=-X轴
				tempInfo.ucs=pTempInfo->ucs;
				tempInfo.ucs.axis_x=pTempInfo->ucs.axis_y;
				tempInfo.ucs.axis_y=-1.0*pTempInfo->ucs.axis_x;
				tempInfo.partList.Empty();
				for(pTempPos=pTempInfo->partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					PART_POS *pNewPos=tempInfo.partList.append(*pTempPos);
					pNewPos->pos.x=pTempPos->pos.y;
					pNewPos->pos.y=-1.0*pTempPos->pos.x;
					pNewPos->ucs=tempInfo.ucs;
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[1]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.3始终端对齐，然后绕Z轴翻转180°//X轴和Y轴反向
				tempInfo.ucs=pTempInfo->ucs;
				tempInfo.ucs.axis_x*=-1.0;
				tempInfo.ucs.axis_y*=-1.0;
				tempInfo.partList.Empty();
				for(pTempPos=pTempInfo->partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					PART_POS *pNewPos=tempInfo.partList.append(*pTempPos);
					pNewPos->pos.x*=-1.0;
					pNewPos->pos.y*=-1.0;
					pNewPos->ucs=tempInfo.ucs;
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[2]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.4始终端对齐，然后绕Z轴翻转270°//X轴=-Y轴和Y轴=X轴
				tempInfo.ucs=pTempInfo->ucs;
				tempInfo.ucs.axis_x=-1.0*pTempInfo->ucs.axis_y;
				tempInfo.ucs.axis_y=pTempInfo->ucs.axis_x;
				tempInfo.partList.Empty();
				for(pTempPos=pTempInfo->partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					PART_POS *pNewPos=tempInfo.partList.append(*pTempPos);
					pNewPos->pos.x=-1.0*pTempPos->pos.y;
					pNewPos->pos.y=pTempPos->pos.x;
					pNewPos->ucs=tempInfo.ucs;
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[3]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.5始终端颠倒  //修改ucs.origin,Z轴和X轴方向
				tempInfo.ucs=pTempInfo->ucs;
				tempInfo.ucs.origin=pTempInfo->pLineTube->End()
					+pTempInfo->pLineTube->ucs.axis_z*pTempInfo->pLineTube->endOdd();
				tempInfo.ucs.axis_x*=-1.0;
				tempInfo.ucs.axis_z*=-1.0;
				//因为Z轴已反向故需要从链表末尾开始遍历
				tempInfo.partList.Empty();
				for(pTempPos=pTempInfo->partList.GetTail();pTempPos;pTempPos=pTempInfo->partList.GetPrev())
				{
					PART_POS newPos;
					newPos.pPart=pTempPos->pPart;
					newPos.pos=pTempPos->pos;
					coord_trans(newPos.pos,pTempInfo->ucs,TRUE);
					coord_trans(newPos.pos,tempInfo.ucs,FALSE);
					newPos.ucs=tempInfo.ucs;
					tempInfo.partList.append(newPos);
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[4]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.6始终端颠倒，然后绕Z轴翻转90° X轴=Y轴和Y轴=-X轴
				//在3.2.5的tempInfo.partList基础上调整
				UCS_STRU temp_ucs=tempInfo.ucs;
				tempInfo.ucs.axis_x=temp_ucs.axis_y;
				tempInfo.ucs.axis_y=-1.0*temp_ucs.axis_x;
				for(pTempPos=tempInfo.partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					f3dPoint temp_pos=pTempPos->pos;
					pTempPos->pos.x=temp_pos.y;
					pTempPos->pos.y=-temp_pos.x;
					pTempPos->ucs.axis_x=temp_ucs.axis_y;
					pTempPos->ucs.axis_y=-1.0*temp_ucs.axis_x;
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[5]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.7始终端颠倒，然后绕Z轴翻转180°
				//在3.2.5的tempInfo.partList基础上调整
				tempInfo.ucs.axis_x*=-1.0;
				tempInfo.ucs.axis_y*=-1.0;
				for(pTempPos=tempInfo.partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					pTempPos->pos.x*=-1.0;
					pTempPos->pos.y*=-1.0;
					pTempPos->ucs.axis_x*=-1.0;
					pTempPos->ucs.axis_y*=-1.0;
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[6]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.8始终端颠倒，然后绕Z轴翻转270° X轴=-Y轴和Y轴=X轴
				//在3.2.5的tempInfo.partList基础上调整
				tempInfo.ucs.axis_x=-1.0*temp_ucs.axis_y;
				tempInfo.ucs.axis_y=temp_ucs.axis_x;
				for(pTempPos=tempInfo.partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					f3dPoint temp_pos=pTempPos->pos;
					pTempPos->pos.x=-temp_pos.y;
					pTempPos->pos.y=temp_pos.x;
					pTempPos->ucs.axis_x=-1.0*temp_ucs.axis_y;
					pTempPos->ucs.axis_y=temp_ucs.axis_x;
				}
				//将PARTGROUP_INFO的构件列表按pos重新排序
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[7]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
			}
			//记录该组组焊件组第一组组焊件不同之处
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sTemp("0x%X and 0x%X information compared:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
#else
			CXhChar100 sTemp("0x%X与0x%X对比信息:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
#endif
			sCompareInfo.Append(sTemp);
			char sType[9]="ABCDEFGH";
			for(int i=0;i<8;i++)
			{
				if(sInfo[i].GetLength()<=0)
					continue;
				CXhChar16 sInfoType("%C",sType[i]);
				CXhChar500 sSumInfo=sInfo[i];
				for(int j=i+1;j<8;j++)
				{
					if(stricmp(sInfo[i],sInfo[j])==0)
					{
						sInfo[j].Copy("");
						sInfoType.Append(",");
						CXhChar16 sCurType("%c",sType[j]);						
						sInfoType.Append(sCurType);
					}
				}
				sCompareInfo.Append("\t");
				sCompareInfo.Append(sInfoType);
				sCompareInfo.Append(":");
				sCompareInfo.Append(sSumInfo);
				sCompareInfo.Append("\n");
			}
		}
		if(pTempInfo==NULL) 
		{	//3.3未在destList中找到匹配的组焊件,将pInfo添加到destList中
			pTempInfo=destList.append();
			pTempInfo->ucs=pInfo->ucs;
			pTempInfo->pLineTube=pInfo->pLineTube;
			pTempInfo->nPartNum=pInfo->nPartNum;
			pTempInfo->tubeSet.Empty();
			pTempInfo->partList.Empty();
			PART_POS *pPartPos=NULL;
			for(pPartPos=pInfo->partList.GetFirst();pPartPos;pPartPos=pInfo->partList.GetNext())
				pTempInfo->partList.append(*pPartPos);
			strcpy(pTempInfo->sError,sCompareInfo);
		}
	}
	//4.该组焊件分组大于1时输出分组信息
	if(destList.GetNodeNum()>1&&sError)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sError,"%s error information of combined weld parts:\n",pPartGroup->sGroupName);
		CXhChar200 sGroupInfo("%s group weld parts group information:\n",pPartGroup->sGroupName);
#else
		sprintf(sError,"%s组焊件错误信息:\n",pPartGroup->sGroupName);
		CXhChar200 sGroupInfo("%s组焊件分组信息:\n",pPartGroup->sGroupName);
#endif
		for(pInfo=destList.GetFirst();pInfo;pInfo=destList.GetNext())
		{
			CXhChar500 sTubeList;
			CLDSLineTube *pLineTube=NULL;
			sTubeList.Printf("\t0X%X",pInfo->pLineTube->handle);
			for(pLineTube=pInfo->tubeSet.GetFirst();pLineTube;pLineTube=pInfo->tubeSet.GetNext())
			{
				CXhChar50 ss(",0X%X",pLineTube->handle);
				sTubeList.Append(ss);
			}
			sTubeList.Append("\n");
			//错误信息
			strcat(sError,pInfo->sError);
			//分组信息
			sGroupInfo.Append(sTubeList);
		}
		strcat(sError,sGroupInfo);	//拼接错误信息与分组信息
	}
	return FALSE;
}

void CTowerPartGroupDlg::OnCheckPartGroupValid()
{
	FILE *fp = fopen("PartGroupValid.txt","wt");
	if(fp==NULL)
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	fprintf(fp,"combined weld parts compared mode:\n");
	fprintf(fp,"\tA:starting and ending alignment \n");
	fprintf(fp,"\tB:starting and ending alignment，Then turn 90°around main tube's Z axis direction \n");
	fprintf(fp,"\tC:starting and ending alignment，Then turn 180°around main tube's Z axis direction \n");
	fprintf(fp,"\tD:starting and ending alignment，Then turn 270°around main tube's Z axis direction \n");
	fprintf(fp,"\tE:starting and ending reverse\n");
	fprintf(fp,"\tF:starting and ending reverse, then turn 90°around main tube's Z axis direction\n");
	fprintf(fp,"\tG:starting and ending reverse，Then turn 180°around main tube's Z axis direction \n");
	fprintf(fp,"\tH:starting and ending reverse，Then turn 270°around main tube's Z axis direction\n");
#else
	fprintf(fp,"组焊件对比方式:\n");
	fprintf(fp,"\tA:始终端对齐\n");
	fprintf(fp,"\tB:始终端对齐，然后绕主管Z轴方向翻转90°\n");
	fprintf(fp,"\tC:始终端对齐，然后绕主管Z轴方向翻转180°\n");
	fprintf(fp,"\tD:始终端对齐，然后绕主管Z轴方向翻转270°\n");
	fprintf(fp,"\tE:始终端颠倒\n");
	fprintf(fp,"\tF:始终端颠倒，然后绕主管Z轴方向翻转90°\n");
	fprintf(fp,"\tG:始终端颠倒，然后绕主管Z轴方向翻转180°\n");
	fprintf(fp,"\tH:始终端颠倒，然后绕主管Z轴方向翻转270°\n");
#endif
	//
	int iError=1;
	BOOL bSuccess=TRUE;
	CLDSPartGroup *pPartGroup=NULL;
	HTREEITEM hItem=m_treeCtrl.GetSelectedItem();
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType==PARTGROUP_SET)
	{	//校核所有的组焊件	
		Ta.DisplayProcess(0,NULL);
		int nCurProcess=0,nSum=Ta.PartGroup.GetNodeNum();
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			nCurProcess++;
			//if(stricmp(pPartGroup->sGroupName,"2816")!=0)
			//	continue;
			int n=(nCurProcess*100)/nSum;
			Ta.DisplayProcess((nCurProcess*100)/nSum,NULL);
			char sError[1000]="";
			CheckPartGroupValidAndCorrect(pPartGroup,FALSE,sError);
			if(strlen(sError)>0)
			{
				bSuccess=FALSE;
				fprintf(fp,"\n");
				fprintf(fp,"%d.%s",iError,sError);
				iError++;
			}
		}
		Ta.DisplayProcess(100,NULL);
	}
	else if(pItemInfo->itemType==PARTGROUP_CASE)
	{	//校核指定的组焊件
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
				break;
		}
		if(pPartGroup==NULL)
			return;
		char sError[1000]="";
		CheckPartGroupValidAndCorrect(pPartGroup,FALSE,sError);
		if(strlen(sError)>0)
		{
			bSuccess=FALSE;
			fprintf(fp,"\n");
			fprintf(fp,"%d.%s",iError,sError);
		}
	}
	fclose(fp);
	if(bSuccess)
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("pass combined weld parts legality check!");
#else
		AfxMessageBox("通过组焊件合法性检查!");
#endif
	else
		WinExec("notepad.exe PartGroupValid.txt",SW_SHOW);
}
void CTowerPartGroupDlg::OnOK()
{

}

void CTowerPartGroupDlg::OnCancel()
{

}
#endif
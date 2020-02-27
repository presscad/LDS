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
					GlobalLock(stgmed.pstm); //�����ڴ��
					LARGE_INTEGER dlibMove;
					dlibMove.QuadPart=0;
					stgmed.pstm->Seek(dlibMove,STREAM_SEEK_SET,NULL);
					HRESULT hr=stgmed.pstm->Read(&num,sizeof(long),NULL);
					//�����͹��������ݼ�����Ӧ��ͼ��
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
	//�Ϸ�Դ�ڵ����Ϳ�Ϊ��ͼ���ļ��У���Ϻ���
	TREEITEM_PARTGROUP_INFO *pItemInfo=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItemDragS);
	if(pItemInfo==NULL||(pItemInfo&&pItemInfo->itemType!=PARTGROUP_VIEWCASE&&
	   pItemInfo->itemType!=PARTGROUP_CASE&&pItemInfo->itemType!=PARTGROUP_FOLDER))
		return FALSE;
	return TRUE;
}

static BOOL IsValidDragD(CTowerTreeCtrl *pTreeCtrl, HTREEITEM hItemDragS, HTREEITEM hItemDragD)
{
	if(hItemDragS==NULL||hItemDragD==NULL)
		return FALSE;	//hItemDragD==NULLʱ����GetItemData()���� wht 13-12-09
	TREEITEM_PARTGROUP_INFO *pItemInfoS=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItemDragS);
	TREEITEM_PARTGROUP_INFO *pItemInfoD=(TREEITEM_PARTGROUP_INFO*)pTreeCtrl->GetItemData(hItemDragD);
	if(pItemInfoS==NULL||pItemInfoD==NULL)
		return FALSE;
	if(pItemInfoS->itemType==PARTGROUP_VIEWCASE)
	{	//��ͼ-->��ͼ ��ͼ-->��ͼ����	
		if(pItemInfoD->itemType==PARTGROUP_VIEWSET||pItemInfoD->itemType==PARTGROUP_VIEWCASE)
			return TRUE;
	}
	else if(pItemInfoS->itemType==PARTGROUP_CASE)
	{	//��Ϻ���-->��Ϻ��� ��Ϻ���-->��Ϻ����ļ���
		if(pItemInfoD->itemType==PARTGROUP_CASE||pItemInfoD->itemType==PARTGROUP_FOLDER)
			return TRUE;
	}
	else if(pItemInfoS->itemType==PARTGROUP_FOLDER)
	{	//��Ϻ����ļ���-->��Ϻ����ļ��� ��Ϻ����ļ���-->��Ϻ�������
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
		{	//��һ����Ϻ����Ϸŵ�һ���ļ��е�ͬʱӦ���޸���Ϻ��������ļ��е�����
			if(pItemInfoS->pObj&&pItemInfoD->pObj)
				strcpy(((CLDSPartGroup*)pItemInfoS->pObj)->sFolderName,((CLDSPartGroup*)pItemInfoD->pObj)->sFolderName);
		}
		return 0;
	}
	else if((pItemInfoS->itemType==PARTGROUP_VIEWCASE&&pItemInfoD->itemType==PARTGROUP_VIEWSET)||
			(pItemInfoS->itemType==PARTGROUP_CASE&&pItemInfoD->itemType==PARTGROUP_FOLDER)||
			(pItemInfoS->itemType==PARTGROUP_FOLDER&&pItemInfoD->itemType==PARTGROUP_SET))
	{	//��ʱ�ڴ˴��޸� 
		//�Ϸ���Ϻ������ļ����»���ļ����ϳ�����Ҫ�޸�������� ���򱣴治�������޸� 
		if(pItemInfoS->itemType==PARTGROUP_CASE&&pItemInfoD->itemType==PARTGROUP_FOLDER)
		{	//��һ����Ϻ����Ϸŵ�һ���ļ��е�ͬʱӦ���޸���Ϻ��������ļ��е�����
			if(pItemInfoS->pObj&&pItemInfoD->pObj)
				strcpy(((CLDSPartGroup*)pItemInfoS->pObj)->sFolderName,((CLDSPartGroup*)pItemInfoD->pObj)->sFolderName);
		}
		else if(pItemInfoS->itemType==PARTGROUP_CASE&&pItemInfoD->itemType==PARTGROUP_SET)
		{	//����Ϻ����ϵ���Ŀ¼�� �ļ��������ÿ�
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
	//1.���ָ����������ӽڵ㼰�ӽڵ��Ӧ�ĸ�������
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
	//1.���ָ����������ӽڵ㼰�ӽڵ��Ӧ�ĸ�������
	if(removeOld)
	{	//��RefreshModel����ʱ������ԭ����û�����ݣ����Բ���Ҫ���������
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
	//2.�����麸���ӽڵ�
	//hPartGroupItemԭ�����У������¼ӽڵ�ʲ���Ҫ�ظ����pItemInfo wjh-2014.5.22
	//TREEITEM_PARTGROUP_INFO *pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_CASE,pPartGroup,pPartGroup->handle));
	//m_treeCtrl.SetItemData(hPartGroupItem,(DWORD)pItemInfo);
	CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
	if(pRootPart)
#ifdef AFX_TARG_ENU_ENGLISH
		ss.Format("basic parts=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
	else
		ss.Format("basic parts=(0X%X)",pPartGroup->handle);
#else
		ss.Format("������=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
	else
		ss.Format("������=(0X%X)",pPartGroup->handle);
#endif
	HTREEITEM hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
	TREEITEM_PARTGROUP_INFO *pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_ROOTPART,pPartGroup));
	m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);

#ifdef AFX_TARG_ENU_ENGLISH
	ss.Format("single number=%d piece",pPartGroup->m_uStatMatCoef);
#else
	ss.Format("������=%d��",pPartGroup->m_uStatMatCoef);
#endif
	hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
	pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_STATNUM,NULL));
	m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);

#ifdef AFX_TARG_ENU_ENGLISH
	HTREEITEM hViewItem=m_treeCtrl.InsertItem("additional view",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#else
	HTREEITEM hViewItem=m_treeCtrl.InsertItem("������ͼ",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#endif
	pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_VIEWSET,NULL));
	m_treeCtrl.SetItemData(hViewItem,(DWORD)pItemInfo);
	RefreshPartGroupViewSetNode(hViewItem,pPartGroup);	//������ȡ�ɺ����Ա��ظ�����
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
	hGroupSetItem = m_treeCtrl.InsertItem("��Ϻ���",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
#endif
	TREEITEM_PARTGROUP_INFO *pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_SET,NULL));
	m_treeCtrl.SetItemData(hGroupSetItem,(DWORD)pItemInfo);
	for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{	//���������б�ǰ ������ɵ�����Ϣ��
		if(pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_FOLDER)
			Ta.PartGroup.DeleteCursor();
	}
	Ta.PartGroup.Clean();
	for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		CString sFolderName=pPartGroup->sFolderName;
		for(pItemInfo=itemInfoList.GetFirst();pItemInfo;pItemInfo=itemInfoList.GetNext())
		{	//�����Ƿ��Ѿ�������ͬ����Ϻ����ļ���
			if(pItemInfo->itemType==PARTGROUP_FOLDER&&
				sFolderName.CompareNoCase(((CLDSPartGroup*)pItemInfo->pObj)->sFolderName)==0)
				break;
		}
		if(pItemInfo)
		{	//�����ڣ������ڱ��ļ��е���Ϻ�����Ϊ���������
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
		{	//�������ڣ��½���
			Ta.PartGroup.push_stack();
			//ÿ��������Ϻ����б��ж���Ӧһ����Ϻ�������������Ϻ�������������ʱ��������
			//����������ڸ������Ϻ�������λ��,���Ա�����ʱ���޷��Ը�����в���
			CLDSPartGroup *pFolder = Ta.PartGroup.append();
			pFolder->m_iPartGroupType=CLDSPartGroup::GROUP_FOLDER;	//�½���Ϻ�����ʱʹ�� ���洢
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
		/* ���´����뺯��RefreshPartGroupTreeNode()������ͬ
		HTREEITEM hViewItem,hLeafItem;
		CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
		if(pRootPart)
			ss.Format("������=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
		else
			ss.Format("������=(0X%X)",pPartGroup->handle);
		hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
		pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_ROOTPART,pPartGroup));
		m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);

		ss.Format("������=%d��",pPartGroup->m_uStatMatCoef);
		hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
		pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_STATNUM,NULL));
		m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);

		hViewItem=m_treeCtrl.InsertItem("������ͼ",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
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
	//չ����Ϻ�����
	m_treeCtrl.Expand(hGroupSetItem,TVE_EXPAND);
	//����
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
		pMenu->AppendMenu(MF_STRING,ID_STAT_PART_GROUP_AUTO,"�Զ�ͳ��");
		pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_WELDLINE,"����ʶ�𺸷�");
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_PART_GROUP,"�����麸��");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_SORT_PARTGROUP,"��������");
		pMenu->AppendMenu(MF_STRING,ID_CHECK_PARTGROUP_VALID,"У���麸��");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP,"����麸��");
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP_FOLDER,"����麸����");
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
		pMenu->AppendMenu(MF_STRING,ID_SORT_PARTGROUP,"����");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"�༭");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"ɾ��");
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
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_PARTGROUP,"����");
		pMenu->AppendMenu(MF_STRING,ID_CHECK_PARTGROUP_VALID,"У��");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"�༭");
		pMenu->AppendMenu(MF_STRING,ID_REFRESH_PART_GROUP,"ͳ����ȡ");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"ɾ��");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"��¡���½�");
#endif
	}
	else if(pItemInfo->itemType==PARTGROUP_VIEWSET)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP,"add");
		pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_PARTGROUP_VIEWS,"automatically extract");
#else
		pMenu->AppendMenu(MF_STRING,ID_NEW_PARTGROUP,"���");
		pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_PARTGROUP_VIEWS,"�Զ���ȡ");
#endif
	}
	else if(pItemInfo->itemType==PARTGROUP_VIEWCASE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_PARTGROUP,"activate");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"edit");
#else
		pMenu->AppendMenu(MF_STRING,ID_SET_ACTIVE_PARTGROUP,"����");
		pMenu->AppendMenu(MF_STRING,ID_EDIT_PARTGROUP,"�༭");
#endif
		//Ŀǰ��֧���ڽ�ģ�����жԱ궨�������Լ���ע�ǵĲ��� wht 10-06-22
		//pMenu->AppendMenu(MF_STRING,ID_PARTGROUP_LOCATE_POINT,"�궨��");
		//pMenu->AppendMenu(MF_STRING,ID_PARTGROUP_CHECK_LINE,"�����");
		//pMenu->AppendMenu(MF_STRING,ID_PARTGROUP_ANGULAR,"��ע��");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"delete");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"clone and newly creat");
#else
		pMenu->AppendMenu(MF_STRING,ID_DELETE_PARTGROUP,"ɾ��");
		pMenu->AppendMenu(MF_SEPARATOR,NULL,"");
		pMenu->AppendMenu(MF_STRING,ID_COPY_AND_NEW_ITEM,"��¡���½�");
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
{	//���ԭ���麸���б� ����ͳ���麸��
	CString segIncStr="*";
#ifdef AFX_TARG_ENU_ENGLISH
	if(Input("segment number range:",&segIncStr,'s')<=0)
#else
	if(Input("�κŷ�Χ��",&segIncStr,'s')<=0)
#endif
		return;
	//�ֽ�κ��ַ���
	CHashList<SEGI>hashSegTbl;
	if(segIncStr.GetLength()>0&&segIncStr.Find('*',0)<0)	//ָ���κŷ�Χ
		GetSegNoHashTblBySegStr(segIncStr,hashSegTbl);
	if(segIncStr.Find('*',0)>=0)
	{
		Ta.PartGroup.Empty();
		itemInfoList.Empty();
		//������itemInfoList����һ�£�������ܻᵼ�����ڵ㸽����Ϣָ��Ϊ�յ����쳣����
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
{	//ˢ���麸���б�
	CLogErrorLife logErrorLife;
	TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
	if(m_treeCtrl.GetSelectedCount()>0)
	{
		HTREEITEM hSelItem=m_treeCtrl.GetSelectedItem();
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hSelItem);
		if(pItemInfo==NULL)
			return;
		if(pItemInfo->itemType==PARTGROUP_CASE)
		{	//ˢ��ָ���麸��
			PARTSET partset;
			CLDSPartGroup *pPartGroup=Ta.PartGroup.FromHandle(pItemInfo->handle);
			if(pPartGroup==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X combined weld parts lost its parent parts!",pItemInfo->handle);
#else
				logerr.Log("0X%X�麸����ĸ�����Ѷ�ʧ!",pItemInfo->handle);
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
					continue;	//��ͼ������ ����Ҫ������ͼ
				pSectionView->SonParts.Empty();
				CSectionView::PART_ITEM *pPartItem = NULL;
				pPartItem = pSectionView->SonParts.Add(pPartGroup->handle);
				pPartItem->cFlag = 0;
				//������ͼ��������
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
	if(Input("�κŷ�Χ��",&segIncStr,'s')<=0)
#endif
		return;
	}
	//�ֽ�κ��ַ���
	CHashList<SEGI>hashSegTbl;
	if(segIncStr.GetLength()>0&&segIncStr.Find('*',0)<0)	//ָ���κŷ�Χ
	{
		if(GetSegNoHashTblBySegStr(segIncStr,hashSegTbl)==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("segment number cannot be empty,please confirm the segment number of combined weld parts!");
#else
			logerr.Log("�κŲ���Ϊ�գ���ȷ���麸���κţ�");
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
	Ta.DisplayProcess(0,"ͳ���麸��");
#endif
	for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		nCurProcess++;
#ifdef AFX_TARG_ENU_ENGLISH
		Ta.DisplayProcess(ftoi(100*nCurProcess/nSumProcess),"combined weld parts statistics");
#else
		Ta.DisplayProcess(ftoi(100*nCurProcess/nSumProcess),"ͳ���麸��");
#endif
		CLDSPart *pRootPart=console.FromPartHandle(pPartGroup->handle);
		if(pRootPart==NULL||(hashSegTbl.GetNodeNum()>0&&hashSegTbl.GetValue(pRootPart->iSeg.iSeg)==NULL))
			continue;	//���������ڵ�ǰָ���κŷ�Χ�ڣ����������з���ͳ��
		BOOL bRetOk=FALSE;
		for(TREEITEM_PARTGROUP_INFO *pInfo=itemInfoList.GetFirst();pInfo;pInfo=itemInfoList.GetNext())
		{	//�����麸����������麸����ʹ��pInfo->pObj==pPartGroup���ң�������ͳ���麸����ʧЧ����Ϊ���·������ڴ�
			if(pInfo->itemType==PARTGROUP_CASE&&pInfo->handle==pPartGroup->handle)
			{	//�麸�������Ѵ��ڸ���Ϻ���
				bRetOk=TRUE; 
				break;
			}
		}
		if(bRetOk)
		{	//�����Ѵ��ڵ��麸����ĸ�����ͼ
			PARTSET partset;
			Ta.GetPartGroupPartSet(pPartGroup->handle,partset);
			if(pPartGroup->SonViews.GetNodeNum()<=0)
				pPartGroup->AddDefaultAttachViews();
			//CLDSPart *pDatumPart=console.FromPartHandle(pPartGroup->handle);
			/**ʵ��֤��CSectionView::m_bLockedû��ʵ��ʹ�ü�ֵ���û������趨��
			  *���Ҷ���һЩ��Ҫ�����ӹ�������ͼ���������� wjh-2018.3.6
			for(CSectionView *pSectionView=pPartGroup->SonViews.GetFirst();pSectionView;pSectionView=pPartGroup->SonViews.GetNext())
			{
				if(pSectionView->m_bLocked)
					continue;	//��ͼ������ ����Ҫ������ͼ
				pSectionView->SonParts.Empty();
				CSectionView::PART_ITEM *pPartItem = NULL;
				pPartItem = pSectionView->SonParts.Add(pPartGroup->handle);
				pPartItem->cFlag = 0;
				//������ͼ��������
				for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
				{
					pPartItem = pSectionView->SonParts.Add(pPart->handle);
					pPartItem->cFlag = 0;
				}
				//���¸�����ͼ�ı궨�㣬����ߣ���ע��	δ���.............
				//pSectionView->LocatePoints.Empty();
				//pSectionView->CheckLines.Empty();
				//pSectionView->Angulars.Empty();
				//pSectionView->AddDefaultLocatePoints(pDatumPart,pPartGroup->SuperiorTower());	//���Ĭ�ϵı궨��
				//pSectionView->AddDefaultCheckLines(pDatumPart,pPartGroup->SuperiorTower());		//���Ĭ�ϵļ����
				//pSectionView->AddDefaultAngulars(pDatumPart,pPartGroup->SuperiorTower());		//���Ĭ�ϵĽǶȱ�ע��
			}
			*/
		}
		else if(pPartGroup->m_iPartGroupType!=CLDSPartGroup::GROUP_FOLDER)//����µ��麸��ĸ�����ͼ
			pPartGroup->AddDefaultAttachViews();
	}
#ifdef AFX_TARG_ENU_ENGLISH
	Ta.DisplayProcess(100,"combined weld parts statistics");
#else
	Ta.DisplayProcess(100,"ͳ���麸��");
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
	{	//ɾ����Ϻ���
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
	{	//ɾ����Ϻ���������ͼ
		CLDSPartGroup* pPartGroup = NULL;
		CSectionView *pView=pItemInfo->GetVerifiedSectionView(&pPartGroup);
		bDeleteOK=TRUE;
		if(pView!=NULL&&pView->m_iViewType==VIEW_MAIN)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("No permission to delete main view !");
#else
			AfxMessageBox("������ɾ������ͼ!");
#endif
			return;
		}
		else if(pView!=NULL)
			pPartGroup->SonViews.DeleteNode(pView->GetId());
	}
	else if(pItemInfo->itemType==PARTGROUP_FOLDER)
	{	//ɾ����Ϻ����� �Լ����ڸ������Ϻ���
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
			{	//ɾ����Ϻ������Լ���Ϻ������ӽڵ������
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
		//���¸�����ͼ�����
		RefreshPartGroupViewSetNode(hItem,pPartGroup);
		m_treeCtrl.Expand(hItem,TVE_EXPAND);
	}
}
void CTowerPartGroupDlg::OnDeletePartgroup() 
{	//ɾ��ѡ���� 
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
					ss.Format("������=%d��",pPartGroup->m_uStatMatCoef);
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
						ss.Format("������=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
					else
						ss.Format("������=(0X%X)",pPartGroup->handle);
#endif
					m_treeCtrl.SetItemText(hLeafItem,ss);
				}
				else if(pItemInfo->itemType==PARTGROUP_VIEWSET)
					hViewSetItem=hLeafItem;	//������ͼ����
				hLeafItem=m_treeCtrl.GetNextSiblingItem(hLeafItem);
			}while(hLeafItem!=NULL);
			pPartGroup->m_bFootPartGroup=partDlg.m_bTowerFoot;	//���Ű��麸�� wht 10-01-14
			if((partDlg.m_hOldRootPart!=0&&partDlg.m_hOldRootPart!=pPartGroup->handle)	//��׼������������仯
				||pPartGroup->m_bFootPartGroup!=bOldFootPartGroup)	//��������Ƿ�Ϊ���Ű�����Է����仯ʱ
			{	//��Ҫ���¸�����ͼ
				pPartGroup->SonViews.Empty();
				pPartGroup->AddDefaultAttachViews();//���Ĭ�ϸ�����ͼ
				//���½�����ʾ wht 10-01-14
				hLeafItem=m_treeCtrl.GetChildItem(hViewSetItem);
				do{
					m_treeCtrl.DeleteItem(hLeafItem);
					hLeafItem=m_treeCtrl.GetNextItem(hViewSetItem,TVGN_CHILD);
				}while(hLeafItem!=NULL);
				TREEITEM_PARTGROUP_INFO *pItemInfo=NULL;
				for(pItemInfo=itemInfoList.GetFirst();pItemInfo;pItemInfo=itemInfoList.GetNext())
				{	//ɾ��ԭ�и�����ͼ�����Ϣ
					if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj&&pItemInfo->itemType==PARTGROUP_VIEWCASE)
						itemInfoList.DeleteCursor();
				}
				//�������ͼ���麸����
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
		folderNameDlg.m_nDlgType = 2;	//��Ϻ���������
		if(IDOK!=folderNameDlg.DoModal())
			return;
		CString sFolderName=pPartGroup->sFolderName;
		for(CLDSPartGroup *pGroup=Ta.PartGroup.GetFirst();pGroup;pGroup=Ta.PartGroup.GetNext())
		{	//�����޸ĺ�ͬʱ�޸�(���ڸ����)��Ϻ���������
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
	if(pView==NULL)	//Ӧ�ú͸�����ͼ���������Ȼÿ�μ��Ҫ���һ���µ���ͼ
		pView = console.AppendView();
	pView->ucs = pSectionView->ucs;
	if(pPartGroup)
		strcpy(pView->folderName,pPartGroup->sGroupName);
	strcpy(pView->name,pSectionView->sName);
	memcpy(&pView->feature,&pSectionView,4);
	pView->m_iViewType = 2;	//��Ϲ���������ͼ��������ͼ������
	pView->EmptyPartSet();
	for(CSectionView::PART_ITEM *pItem=pSectionView->SonParts.GetFirst();pItem;pItem=pSectionView->SonParts.GetNext())
	{	
		CLDSPart *pPart = console.FromPartHandle(pItem->hSonPart);
		if(pPart==NULL)
			continue;
		pView->AppendViewPart(CViewPart(pPart->handle,pPart->GetClassTypeId()));
		CLsRefList *pLsRefList=NULL;
		double L=1000000000;	//Ԥ��һ���ϴ�ֵ
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
		{	//�������ϵ���˨��ʱ��ʾ���� ������ӱ�ע��
			for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
				{	//����ʾ���ڻ��巶Χ�ڵ���˨ wht 10-08-06
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
	//�л���ʵ����ʾģʽ
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
	if(hItem==NULL)	//�հ״��������Ҽ�
		hItem=m_treeCtrl.GetRootItem();
	if(hItem)
		pItemInfo=(TREEITEM_PARTGROUP_INFO*)m_treeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType==PARTGROUP_SET)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		partDlg.m_sPartGroupName = "newly created combined weld parts";
#else
		partDlg.m_sPartGroupName = "�½���Ϻ���";
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
				{	//�Ѵ����½�����Ϻ��������飬��ֱ����ӵ���Ӧ����
					hFolderItem=hChildItem;
					m_treeCtrl.Expand(hFolderItem,TVE_EXPAND);	//չ����
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
				ss.Format("������=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
			else
				ss.Format("������=(0X%X)",pPartGroup->handle);
#endif
			HTREEITEM hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_ROOTPART,pPartGroup));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("single number=%d pieces",pPartGroup->m_uStatMatCoef);
#else
			ss.Format("������=%d��",pPartGroup->m_uStatMatCoef);
#endif
			hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_STATNUM,NULL));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			
#ifdef AFX_TARG_ENU_ENGLISH
			HTREEITEM hViewItem=m_treeCtrl.InsertItem("additional view",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#else
			HTREEITEM hViewItem=m_treeCtrl.InsertItem("������ͼ",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
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
		strcpy(tempView.sName,"�½���ͼ");
#endif
		viewDlg.m_pWorkPartGroup = pPartGroup;
		viewDlg.m_pWorkView = &tempView;
		if(IDOK==viewDlg.DoModal())
		{
			//TODO: �����Ļ�������������Ӧ����������ͼ��ʱ��һ������ wjh-2014.3.29
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
	{	//��¡���½��麸�� wht 10-11-08
		CLDSPartGroup *pPartGroup=NULL;
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup==(CLDSPartGroup*)pItemInfo->pObj)
				break;
		}
		if(pPartGroup==NULL)
			return;
		HTREEITEM hParentItem=m_treeCtrl.GetParentItem(hItem);	//���ڵ�
		if(hParentItem==NULL)
			return;
		//
		CPartGroupDlg partDlg;
		partDlg.m_bCopyAndNew=TRUE;	//ͨ����¡��ǰ�麸�������µ��麸��
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
		{	//����麸��
			CLDSPartGroup *pNewPartGroup = Ta.PartGroup.append();	
			sprintf(pNewPartGroup->sGroupName,"%s",partDlg.m_sPartGroupName);
			sprintf(pNewPartGroup->sFolderName,"%s",partDlg.m_sFolderName);
			pNewPartGroup->m_uStatMatCoef = partDlg.m_uStatNum;
			sscanf(partDlg.m_sRootPartHandle,"%X",&pNewPartGroup->handle);
			pNewPartGroup->ucs = partDlg.ucs;
			//�����麸����ͼ
			if(pNewPartGroup->handle!=pPartGroup->handle)//��׼������������仯
			{	//��Ҫ���¸�����ͼ
				pNewPartGroup->SonViews.Empty();
				pNewPartGroup->AddDefaultAttachViews();//���Ĭ�ϸ�����ͼ
			}
			//���½�����ʾ
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
					{	//�Ѵ����½�����Ϻ��������飬��ֱ����ӵ���Ӧ����
						hFolderItem=hChildItem;
						m_treeCtrl.Expand(hFolderItem,TVE_EXPAND);	//չ����
						break;
					}
					hChildItem=m_treeCtrl.GetNextSiblingItem(hChildItem);
				}
			}
			else if(sFolderName.GetLength()<=0)
				strcpy(pNewPartGroup->sGroupName,pPartGroup->sGroupName);
			if(hFolderItem) //���뵽ָ��������
				hPartGroupItem= m_treeCtrl.InsertItem(partDlg.m_sPartGroupName,PRJ_IMG_PARTGROUP,PRJ_IMG_PARTGROUP,hFolderItem);
			else			//���뵽��ǰ������
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
				ss.Format("������=%s(0X%X)",(char*)pRootPart->GetPartNo(),pNewPartGroup->handle);
			else
				ss.Format("������=(0X%X)",pNewPartGroup->handle);
#endif
			HTREEITEM hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_ROOTPART,pNewPartGroup));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			
#ifdef AFX_TARG_ENU_ENGLISH
			ss.Format("single number=%d pieces",pNewPartGroup->m_uStatMatCoef);
#else
			ss.Format("������=%d��",pNewPartGroup->m_uStatMatCoef);
#endif
			hLeafItem=m_treeCtrl.InsertItem(ss,PRJ_IMG_PARA,PRJ_IMG_PARA,hPartGroupItem);
			pItemInfo=itemInfoList.append(TREEITEM_PARTGROUP_INFO(PARTGROUP_STATNUM,NULL));
			m_treeCtrl.SetItemData(hLeafItem,(DWORD)pItemInfo);
			
#ifdef AFX_TARG_ENU_ENGLISH
			HTREEITEM hViewItem=m_treeCtrl.InsertItem("additional view",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
#else
			HTREEITEM hViewItem=m_treeCtrl.InsertItem("������ͼ",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hPartGroupItem);
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
	{	//��¡���½�������ͼ
		CLDSPartGroup* pPartGroup = NULL;
		CSectionView *pCurView=pItemInfo->GetVerifiedSectionView(&pPartGroup);
		if(pCurView!=NULL)
		{
			CSectionViewPropertyDlg viewDlg;
			viewDlg.m_pWorkPartGroup = pPartGroup;
			viewDlg.m_pSrcView = pCurView;
			if(IDOK==viewDlg.DoModal() && viewDlg.m_pWorkView)
			{	
				pCurView=viewDlg.m_pWorkView;	//��¡�½�����ͼ
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
	{	//�޸��麸�����������
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
					ss.Format("������=%d��",pPartGroup->m_uStatMatCoef);
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
						ss.Format("������=%s(0X%X)",(char*)pRootPart->GetPartNo(),pPartGroup->handle);
					else
						ss.Format("������=(0X%X)",pPartGroup->handle);
#endif
					m_treeCtrl.SetItemText(hLeafItem,ss);
					break;
				}
				hLeafItem=m_treeCtrl.GetNextSiblingItem(hLeafItem);
			}while(hLeafItem!=NULL);
			if(partDlg.m_hOldRootPart!=0&&partDlg.m_hOldRootPart!=pPartGroup->handle)
			{	//��׼������������仯��Ҫ���¸�����ͼ
				pPartGroup->SonViews.Empty();
				//���Ĭ�ϸ�����ͼ
				pPartGroup->AddDefaultAttachViews();
			}
		}
	}
	else
		OnSetActivePartgroup();	//������Ϻ���
	*pResult = 0;
}

void CTowerPartGroupDlg::OnNewPartgroupFolder()
{
	CLDSPartGroup *pPartGroup=NULL;
	CFolderNameDlg folderNameDlg;
	folderNameDlg.m_nNameMaxLen=16;
	folderNameDlg.m_nDlgType = 2;	//��Ϻ���������
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
		folderNameDlg.m_sFolderName = "�½���";
#endif
		if(IDOK==folderNameDlg.DoModal())
		{
			CLDSPartGroup *pPartGroup = Ta.PartGroup.append();
			pPartGroup->m_iPartGroupType=CLDSPartGroup::GROUP_FOLDER;	//�½���Ϻ�����ʱʹ�� ���洢
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
	//�л���ʵ����ʾģʽ
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
					scr_part_para.cmdStr="��ѡ��궨�����ڻ�׼����,<�س���>ȷ��:";
					scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ���,������ѡ��궨�����ڻ�׼����,<�س���>ȷ��:";
#endif
					if(!GetPartsFromScr(scr_part_para,FALSE))
						return ;
					if(scr_part_para.pResultObjsArr[0]&&scr_part_para.pResultObjsArr[0]->IsPart())
						locate_point_dlg.m_sDatumPart.Format("0X%X",scr_part_para.pResultObjsArr[0]->handle);
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("command:","");
#else
					pCmdLine->FillCmdLine("����:","");
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
	//�л���ʵ����ʾģʽ
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
					//�л���ʵ����ʾģʽ
					g_pSolidSet->SetDisplayType(DISP_SOLID);
					Invalidate(FALSE);
					g_pSolidDraw->ReleaseSnapStatus();
					if(check_line_dlg.m_bBeforeWeld)
					{	//����ǰ�����
						long *id_arr=NULL;
						CLDSPart *pPart=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
						pCmdLine->FillCmdLine("please choose checking line's starting parts (bolts) before welding, press enter to confirm:","");
#else
						pCmdLine->FillCmdLine("��ѡ��ǰ�������ʼ����(��˨),���س���ȷ��:","");
#endif
						g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
						pLDSView->OpenWndSel();	//����ѡ����˨
						if(pCmdLine->GetStrFromCmdLine(cmdStr))
						{
							//���ݾ���ַ������빹��
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
						pLDSView->OperOther();	//ȡ������ѡ��
#ifdef AFX_TARG_ENU_ENGLISH
						pCmdLine->FillCmdLine("please choose checking line's ending parts (plates, tubes) before welding:","");
#else
						pCmdLine->FillCmdLine("��ѡ��ǰ�������ֹ����(�ְ塢�ֹ�):","");
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
										pCmdLine->FillCmdLine("ѡ���˴���Ĺ�����������ѡ��ǰ�������ֹ����(�ְ塢�ֹ�):","");		
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
					{	//���Ӻ�����
						GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL),0,0,2);
						scr_part_para.disp_type=DISP_SOLID;
#ifdef AFX_TARG_ENU_ENGLISH
						scr_part_para.cmdStr="please choose checking line's positioning parts for starting and ending ends, press enter button to confirm:";
						scr_part_para.cmdErrorFeedbackStr="No proper parts selected, please choose checking line's positioning parts for starting and ending ends, press enter button to confirm:";
#else
						scr_part_para.cmdStr="��ѡ������ʼ�˶�λ�����Լ��ն˶�λ����,<�س���>ȷ��:";
						scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ���,������ѡ�����ߵ�ʼ�˶�λ�����Լ��ն˶�λ����,<�س���>ȷ��:";
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
					pCmdLine->FillCmdLine("����:","");
#endif
					if(pEndPart&&check_line_dlg.boltList.GetNodeNum()>0)
					{	//ֱ�ӽ�����ǰ�������ӵ�������б���
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
	//�л���ʵ����ʾģʽ
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
					scr_part_para.cmdStr="��ѡ��Ҫ��ע�Ƕ��ߵ�������׼����,<�س���>ȷ��:";
					scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ���,������ѡ��Ҫ��ע�Ƕ��ߵ�������׼����,<�س���>ȷ��:";
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
							angular_dlg.m_bStartLineIsWeldRoad=FALSE;	//Ĭ�ϻ�׼��λ�ֹܻ�׼��
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
							angular_dlg.m_bStartLineIsWeldRoad=FALSE;	//Ĭ�ϻ�׼��λ�ֹܻ�׼��
							angular_dlg.m_bHasWeldRoad=pLineTube->m_bHasWeldRoad;
						}
					}
					pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("command:","");
#else
					pCmdLine->FillCmdLine("����:","");
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

//����麸���ĺϷ���
typedef struct tagPART_POS
{
	CLDSPart *pPart;	//����
	f3dPoint pos;		//����λ��
	UCS_STRU ucs;		//��������ʱʹ��
	tagPART_POS(){pPart=NULL;}
}PART_POS;
typedef struct tagPARTGROUP_INFO
{
	CLDSLineTube *pLineTube;//����
	UCS_STRU ucs;			//�麸������ϵ
	TUBESET tubeSet;		//�洢��pLineTube����ͬһ��ĸֹ�
	char sError[800];		//�����洢�µ��麸�����һ���麸������ͬ��ԭ��
	ARRAY_LIST<PART_POS> partList;	//�ְ�(������)�����б�
	int nPartNum;			//�����ܶ�Ӧ�麸���Ĺ�������
	tagPARTGROUP_INFO(){memset(sError,0,800); pLineTube=NULL; nPartNum=0; }
}PARTGROUP_INFO;
static int ComparePartPos(const PART_POS &pPartPos1,const PART_POS &pPartPos2)
{
	if(pPartPos1.pos.z>pPartPos2.pos.z)
		return 1;
	else if(pPartPos1.pos.z<pPartPos2.pos.z)
		return -1;
	else if(pPartPos1.pPart&&pPartPos2.pPart)
	{	//������Z�������ʱ����űȽ�
		int nRetCode=ComparePartNoString(pPartPos1.pPart->GetPartNo(),pPartPos2.pPart->GetPartNo());
		if(nRetCode==0)
		{	//Z��������ұ��Ҳ��ͬ�����ڽڵ��һ�㶼ƽ���ڸֹ����߷����ڽڵ������ȡһ��ת�����������ϵ��
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

//�Ƚ����������еĹ���λ�û��߷����Ƿ���ȫһ��
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
			sprintf(sError,"{0X%X��0X%X�������Ͳ�ͬ!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
			break;	//�������Ͳ�һ��
		}
		//if(pTempPos&&(pTempPos->pos!=pPartPos->pos)
		if(fabs(pTempPos->pos.x-pPartPos->pos.x)>EPS||fabs(pTempPos->pos.y-pPartPos->pos.y)>EPS
			||fabs(pTempPos->pos.z-pPartPos->pos.z)>EPS)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"{part 0X%X and part 0X%X have different locations!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#else
			sprintf(sError,"{0X%X��0X%X����λ�ò�ͬ!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
			break;	//����λ�ò�ͬ
		}
		if(stricmp(pPartPos->pPart->GetPartNo(),pTempPos->pPart->GetPartNo())!=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"{part 0X%X(%s) and part 0X%X(%s) have the same location, but their numbers are differnt!}",pTempPos->pPart->handle
#else
			sprintf(sError,"{0X%X(%s)��0X%X(%s)ͬһλ�ù�����Ų�ͬ!}",pTempPos->pPart->handle
#endif
				,(char*)pTempPos->pPart->GetPartNo(),pPartPos->pPart->handle,(char*)pPartPos->pPart->GetPartNo());
			break;	//ͬһλ�ù�����Ų�ͬ
		}
		if(pPartPos->pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{	//�������ְ�
			CLDSParamPlate *pParamPlate1=(CLDSParamPlate*)pPartPos->pPart;
			CLDSParamPlate *pParamPlate2=(CLDSParamPlate*)pTempPos->pPart;
			if(pParamPlate1->m_iParamType!=pParamPlate2->m_iParamType)
				break;
			if(pParamPlate1->m_iParamType==TYPE_ROLLEND)
			{	//�жϲ��Ͳ���߷����Ƿ�һ��
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
					sprintf(sError,"{���Ͳ��0X%X��0X%X��߷���ͬ!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
					break;	//���Ͳ���߷���һ��
				}
			}
		}
		else if(pPartPos->pPart->GetClassTypeId()==CLS_PLATE)
		{	//��ͨ�ְ� �Ƚ���˨λ���Ƿ���ȫһ��
			CLDSPlate *pPlate1=(CLDSPlate*)pPartPos->pPart;
			CLDSPlate *pPlate2=(CLDSPlate*)pTempPos->pPart;
			//�Ƚ����ְ���˨��
			if(pPlate1->GetLsCount()!=pPlate2->GetLsCount())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"{Node plate 0X%X and 0X%X have different bolts number!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#else
				sprintf(sError,"{�ڵ��0X%X��0X%X��˨������ͬ!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
				break; //ͬһλ�õ����ְ��ϵ���˨������ͬ
			}
			//�Ƚ����ְ巨�߷����Ƿ���ͬ
			f3dPoint plate_norm1=pPlate1->ucs.axis_z;
			f3dPoint plate_norm2=pPlate2->ucs.axis_z;
			vector_trans(plate_norm1,pPartPos->ucs,FALSE);
			vector_trans(plate_norm2,pTempPos->ucs,FALSE);
			double cosa=fabs(plate_norm1*plate_norm2);
			//cos1��=0.99984769515639123915701155881391
			double cos1=0.99984769515639123915701155881391;
			if(cosa<cos1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"{node plate 0X%X and normal 0X%X have different direction,error degree is %.2f��!}"
#else
				sprintf(sError,"{�ڵ��0X%X��0X%X����(���)����ͬ,������Ϊ%.2f��!}"
#endif
					,pTempPos->pPart->handle,pPartPos->pPart->handle,ACOS(cosa)*DEGTORAD_COEF);
				break; //��Է�����ͬ
			}
			fPtList boltPosList1,boltPosList2;
			f3dPoint *pPt=NULL;
			CLsRef *pLsRef=NULL;
			for(pLsRef=pPlate1->GetFirstLsRef();pLsRef;pLsRef=pPlate1->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				f3dPoint ls_pos=pBolt->ucs.origin;
				//����˨λ��ת�����ְ�����ϵ��
				coord_trans(ls_pos,pPlate1->ucs,FALSE);
				ls_pos.z=0; //Z��������
				coord_trans(ls_pos,pPlate1->ucs,TRUE);
				//����˨λ��ת����pInfo1->ucs����ϵ��
				coord_trans(ls_pos,pInfo1.ucs,FALSE);
				for(pPt=boltPosList1.GetFirst();pPt;pPt=boltPosList1.GetNext())
				{
					if(ls_pos.z<pPt->z)
					{	//��Z�����С��������
						boltPosList1.insert(ls_pos);
						break;
					}
					else if(fabs(ls_pos.z-pPt->z)<EPS)
					{	//Z�������ʱ��X�����С����
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
				//����˨λ��ת�����ְ�����ϵ��
				coord_trans(ls_pos,pPlate2->ucs,FALSE);
				ls_pos.z=0; //Z��������
				coord_trans(ls_pos,pPlate2->ucs,TRUE);
				//����˨λ��ת����pInfo2->ucs����ϵ��
				coord_trans(ls_pos,pInfo2.ucs,FALSE);
				for(pPt=boltPosList2.GetFirst();pPt;pPt=boltPosList2.GetNext())
				{
					if(ls_pos.z<pPt->z)
					{	//��Z�����С��������
						boltPosList2.insert(ls_pos);
						break;
					}
					else if(fabs(ls_pos.z-pPt->z)<EPS)
					{	//Z�������ʱ��X�����С����
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
				//�������ȡֵΪ 1
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
				sprintf(sError,"{�ڵ��0X%X��0X%X��˨��λ�ò�ƥ��!}",pTempPos->pPart->handle,pPartPos->pPart->handle);
#endif
				break; //���ְ���˨�ײ��غ�
			}
		}
		pTempPos=pInfo2.partList.GetNext();
	}
	if(pPartPos==NULL)
		return TRUE;	//���麸������λ����ȫ��ͬ
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
		sprintf(sError,"{main tube 0X%X number is empty��please confirm!\n}");
#else
		sprintf(sError,"{����0X%X���Ϊ�գ���ȷ��!\n}");
#endif
		return FALSE;
	}
	pDatumTube->BuildUCS();
	PARTSET partSet;
	//srcList	�洢����׼�ֹ��������ֹܶ�Ӧ��PARTGROUP_INFO,��Ϊ�Ƚϵ�ԭʼ�б�
	//destList	�洢�����Ժ���麸��,srcList�е�ÿ���ڵ㶼����destList
	//			�б��е�Ԫ�����ƥ�䣬�綼��ƥ������ӵ�destList��β��Ϊһ���µķ���

	//1.����׼�ֹܶ�Ӧ��PARTGROUP_INFO��Ϣ��ӵ�destList�б���
	ATOM_LIST<PARTGROUP_INFO> srcList,destList;
	CLDSLineTube *pLineTube=NULL;
	PARTGROUP_INFO *pInfo=destList.append();
	pInfo->pLineTube=pDatumTube;
	pInfo->ucs=pDatumTube->ucs;
	pInfo->ucs.origin=pDatumTube->Start()-pDatumTube->startOdd()*pDatumTube->ucs.axis_z;
	//1.1���ڵ�塢����Լ�������Щ��Ҫ������ӵ�PARTGROUP_INFO�Ĺ����б��У���Ϊ�Ƚ϶���
	Ta.GetPartGroupPartSet(pDatumTube->handle,partSet);
	pInfo->nPartNum=partSet.GetNodeNum();	//�麸������
	CLDSPart *pPart=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		//����ӽڵ���ֹܶ�ͷ���ӵĹ���(������)
		if(pPart->GetClassTypeId()!=CLS_PLATE&&pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{	//����Ӹֹܶ����ӹ���
			CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
			if(!pParamPlate->IsFL()||!pParamPlate->IsInsertPlate())
				continue;
		}
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{	//����ӽڵ��
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
	//1.2��PARTGROUP_INFO�Ĺ����б�pos����
	CQuickSort<PART_POS>::QuickSort(pInfo->partList.m_pData,pInfo->partList.GetSize(),ComparePartPos);

	//2.���ҹ��������ͬ�������麸����ӵ�srcList��
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
		//1.1���ڵ�塢����Լ�������Щ��Ҫ������ӵ�PARTGROUP_INFO�Ĺ����б��У���Ϊ�Ƚ϶���
		Ta.GetPartGroupPartSet(pLineTube->handle,partSet);
		pInfo->nPartNum=partSet.GetNodeNum();	//�麸������
		for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
		{
			//����ӽڵ���ֹܶ�ͷ���ӵĹ���(������)
			if(pPart->GetClassTypeId()!=CLS_PLATE&&pPart->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			{	//����Ӹֹܶ����ӹ���
				CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
				if(!pParamPlate->IsFL()||!pParamPlate->IsInsertPlate())
					continue;
			}
			if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//����ӽڵ��
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
		//2.2��PARTGROUP_INFO�Ĺ����б�pos����
		CQuickSort<PART_POS>::QuickSort(pInfo->partList.m_pData,pInfo->partList.GetSize(),ComparePartPos);
	}
	//3.��srcList�еĽڵ������destList�еĽڵ���бȽ�Ȼ�����
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
			//3.0�麸������������ͬ
			if(pInfo->nPartNum!=pTempInfo->nPartNum)
			{	
#ifdef AFX_TARG_ENU_ENGLISH
				CXhChar100 sTemp("0x%X and 0x%X information compared:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
				sCompareInfo.Append(sTemp);
				sSubInfo.Printf("\t{0X%X(%d pieces) and 0X%X(%d pieces) tube combined weld quantities are different!}\n"
#else
				CXhChar100 sTemp("0x%X��0x%X�Ա���Ϣ:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
				sCompareInfo.Append(sTemp);
				sSubInfo.Printf("\t{0X%X(%d��)��0X%X(%d��)�ֹ��麸������������ͬ!}\n"
#endif
					,pInfo->pLineTube->handle,pInfo->nPartNum,pTempInfo->pLineTube->handle,pTempInfo->nPartNum);
				sCompareInfo.Append(sSubInfo);
				continue;
			}
			//3.1�麸�������ؼ�����������ͬ
			if(nPart!=pTempInfo->partList.GetSize())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				CXhChar100 sTemp("0x%X and 0x%X information compared:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
				sCompareInfo.Append(sTemp);
				sSubInfo.Printf("numbers of \t{0X%X(%d piece)and 0X%X(%d piece)tube combined weld part (node plate, insertion plate and flange) are diffent!}\n"
#else
				CXhChar100 sTemp("0x%X��0x%X�Ա���Ϣ:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
				sCompareInfo.Append(sTemp);
				sSubInfo.Printf("\t{0X%X(%d��)��0X%X(%d��)�ֹ��麸���ؼ�����(�ڵ�塢����Լ�����)������ͬ!}\n"
#endif
					,pInfo->pLineTube->handle,nPart,pTempInfo->pLineTube->handle,pTempInfo->partList.GetSize());
				sCompareInfo.Append(sSubInfo);
				continue;
			}
			if(pTempInfo->pLineTube==NULL)
				continue;
			//3.2�Ƚ����������Ƿ���ȫһ��
			PART_POS *pTempPos=NULL;
			PARTGROUP_INFO tempInfo,Info;	//��ʱ��PARTGROUP_INFO
			tempInfo.pLineTube=pTempInfo->pLineTube;
			if(toupper(pDatumTube->layer(1))=='Z'&&pDatumTube->m_bHasWeldRoad)
			{	//���ں����ߵ����ܣ��Ժ����߷���ΪX�᷽��
				//ʼ�ն˶���
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				//
				PARTGROUP_INFO Info;	//��ʱ��PARTGROUP_INFO
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(Info.partList.m_pData,Info.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(Info,tempInfo,sInfo[0]))
				{
					pTempInfo->tubeSet.append(Info.pLineTube);
					break;
				}
				//ʼ�ն˵ߵ�
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				//
				Info.ucs=pInfo->ucs;
				Info.ucs.axis_z*=-1.0;	//Z�ᷴ��
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(Info.partList.m_pData,Info.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(Info,tempInfo,sInfo[5]))
				{
					pTempInfo->tubeSet.append(Info.pLineTube);
					break;
				}
			}
			else
			{
				//3.2.1ʼ�ն˶���
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
				//3.2.2ʼ�ն˶��룬Ȼ����Z�ᷭת90��//X��=Y���Y��=-X��
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[1]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.3ʼ�ն˶��룬Ȼ����Z�ᷭת180��//X���Y�ᷴ��
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[2]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.4ʼ�ն˶��룬Ȼ����Z�ᷭת270��//X��=-Y���Y��=X��
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[3]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.5ʼ�ն˵ߵ�  //�޸�ucs.origin,Z���X�᷽��
				tempInfo.ucs=pTempInfo->ucs;
				tempInfo.ucs.origin=pTempInfo->pLineTube->End()
					+pTempInfo->pLineTube->ucs.axis_z*pTempInfo->pLineTube->endOdd();
				tempInfo.ucs.axis_x*=-1.0;
				tempInfo.ucs.axis_z*=-1.0;
				//��ΪZ���ѷ������Ҫ������ĩβ��ʼ����
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[4]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.6ʼ�ն˵ߵ���Ȼ����Z�ᷭת90�� X��=Y���Y��=-X��
				//��3.2.5��tempInfo.partList�����ϵ���
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[5]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.7ʼ�ն˵ߵ���Ȼ����Z�ᷭת180��
				//��3.2.5��tempInfo.partList�����ϵ���
				tempInfo.ucs.axis_x*=-1.0;
				tempInfo.ucs.axis_y*=-1.0;
				for(pTempPos=tempInfo.partList.GetFirst();pTempPos;pTempPos=pTempInfo->partList.GetNext())
				{
					pTempPos->pos.x*=-1.0;
					pTempPos->pos.y*=-1.0;
					pTempPos->ucs.axis_x*=-1.0;
					pTempPos->ucs.axis_y*=-1.0;
				}
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[6]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
				//3.2.8ʼ�ն˵ߵ���Ȼ����Z�ᷭת270�� X��=-Y���Y��=X��
				//��3.2.5��tempInfo.partList�����ϵ���
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
				//��PARTGROUP_INFO�Ĺ����б�pos��������
				CQuickSort<PART_POS>::QuickSort(tempInfo.partList.m_pData,tempInfo.partList.GetSize(),ComparePartPos);
				if(ComparePartGroupInfo(*pInfo,tempInfo,sInfo[7]))
				{
					pTempInfo->tubeSet.append(pInfo->pLineTube);
					break;
				}
			}
			//��¼�����麸�����һ���麸����֮ͬ��
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sTemp("0x%X and 0x%X information compared:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
#else
			CXhChar100 sTemp("0x%X��0x%X�Ա���Ϣ:\n",pInfo->pLineTube->handle,pTempInfo->pLineTube->handle);
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
		{	//3.3δ��destList���ҵ�ƥ����麸��,��pInfo��ӵ�destList��
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
	//4.���麸���������1ʱ���������Ϣ
	if(destList.GetNodeNum()>1&&sError)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sError,"%s error information of combined weld parts:\n",pPartGroup->sGroupName);
		CXhChar200 sGroupInfo("%s group weld parts group information:\n",pPartGroup->sGroupName);
#else
		sprintf(sError,"%s�麸��������Ϣ:\n",pPartGroup->sGroupName);
		CXhChar200 sGroupInfo("%s�麸��������Ϣ:\n",pPartGroup->sGroupName);
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
			//������Ϣ
			strcat(sError,pInfo->sError);
			//������Ϣ
			sGroupInfo.Append(sTubeList);
		}
		strcat(sError,sGroupInfo);	//ƴ�Ӵ�����Ϣ�������Ϣ
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
	fprintf(fp,"\tB:starting and ending alignment��Then turn 90��around main tube's Z axis direction \n");
	fprintf(fp,"\tC:starting and ending alignment��Then turn 180��around main tube's Z axis direction \n");
	fprintf(fp,"\tD:starting and ending alignment��Then turn 270��around main tube's Z axis direction \n");
	fprintf(fp,"\tE:starting and ending reverse\n");
	fprintf(fp,"\tF:starting and ending reverse, then turn 90��around main tube's Z axis direction\n");
	fprintf(fp,"\tG:starting and ending reverse��Then turn 180��around main tube's Z axis direction \n");
	fprintf(fp,"\tH:starting and ending reverse��Then turn 270��around main tube's Z axis direction\n");
#else
	fprintf(fp,"�麸���Աȷ�ʽ:\n");
	fprintf(fp,"\tA:ʼ�ն˶���\n");
	fprintf(fp,"\tB:ʼ�ն˶��룬Ȼ��������Z�᷽��ת90��\n");
	fprintf(fp,"\tC:ʼ�ն˶��룬Ȼ��������Z�᷽��ת180��\n");
	fprintf(fp,"\tD:ʼ�ն˶��룬Ȼ��������Z�᷽��ת270��\n");
	fprintf(fp,"\tE:ʼ�ն˵ߵ�\n");
	fprintf(fp,"\tF:ʼ�ն˵ߵ���Ȼ��������Z�᷽��ת90��\n");
	fprintf(fp,"\tG:ʼ�ն˵ߵ���Ȼ��������Z�᷽��ת180��\n");
	fprintf(fp,"\tH:ʼ�ն˵ߵ���Ȼ��������Z�᷽��ת270��\n");
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
	{	//У�����е��麸��	
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
	{	//У��ָ�����麸��
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
		AfxMessageBox("ͨ���麸���Ϸ��Լ��!");
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
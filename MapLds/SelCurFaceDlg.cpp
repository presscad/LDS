//<LOCALE_TRANSLATE/>
// SelCurFaceDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "maptma.h"
#include "SelCurFaceDlg.h"
#include "DrawTaMap.h"
#include "image.h"
#include ".\selcurfacedlg.h"
#include "ParseAdaptNo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelCurFaceDlg dialog
CSelCurFaceDlg::CSelCurFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelCurFaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelCurFaceDlg)
	m_sSegNo = _T("1");
	//}}AFX_DATA_INIT
	m_pSelectedView=NULL;
}


void CSelCurFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelCurFaceDlg)
	DDX_CBString(pDX, IDC_CMB_SEG_NO, m_sSegNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelCurFaceDlg, CDialog)
	//{{AFX_MSG_MAP(CSelCurFaceDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DISPLAY_VIEW, OnDblclkTreeDisplayView)
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DISPLAY_VIEW, OnTvnSelchangedTreeDisplayView)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelCurFaceDlg message handlers

void CSelCurFaceDlg::RefreshViewGroup(HTREEITEM hModelItem,IModel* pModel)
{
	CTreeCtrl* pTreeCtrl=(CTreeCtrl*)GetDlgItem(IDC_TREE_DISPLAY_VIEW);
	TREEITEMDISPLAYVIEW_INFO *pItemInfo=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	HTREEITEM hViewGroupItem=pTreeCtrl->InsertItem("view",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hModelItem);
	pItemInfo=itemInfoList.append(TREEITEMDISPLAYVIEW_INFO(VIEW_DEFINEDFOLDER,NULL));
	pTreeCtrl->SetItemData(hViewGroupItem,(DWORD)pItemInfo);
	HTREEITEM hLayerViewSetItem=pTreeCtrl->InsertItem("layer view",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hViewGroupItem);
	pItemInfo=itemInfoList.append(TREEITEMDISPLAYVIEW_INFO(VIEW_LAYERSET,NULL));
	pTreeCtrl->SetItemData(hLayerViewSetItem,(DWORD)pItemInfo);
	HTREEITEM hDefinedViewSetItem=pTreeCtrl->InsertItem("defined view",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hViewGroupItem);
#else
	HTREEITEM hViewGroupItem=pTreeCtrl->InsertItem("视图",PRJ_IMG_VIEWSET,PRJ_IMG_VIEWSET,hModelItem);
	pItemInfo=itemInfoList.append(TREEITEMDISPLAYVIEW_INFO(VIEW_DEFINEDFOLDER,NULL));
	pTreeCtrl->SetItemData(hViewGroupItem,(DWORD)pItemInfo);
	HTREEITEM hLayerViewSetItem=pTreeCtrl->InsertItem("图层视图",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hViewGroupItem);
	pItemInfo=itemInfoList.append(TREEITEMDISPLAYVIEW_INFO(VIEW_LAYERSET,NULL));
	pTreeCtrl->SetItemData(hLayerViewSetItem,(DWORD)pItemInfo);
	HTREEITEM hDefinedViewSetItem=pTreeCtrl->InsertItem("定制视图",PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hViewGroupItem);
#endif
	pItemInfo=itemInfoList.append(TREEITEMDISPLAYVIEW_INFO(VIEW_DEFINEDSET,NULL));
	pTreeCtrl->SetItemData(hDefinedViewSetItem,(DWORD)pItemInfo);
	for(CDisplayView *pView=pModel->EnumViewFirst();pView;pView=pModel->EnumViewNext())
	{
		HTREEITEM hFolderItem=NULL,hViewItem=NULL;
		if(pView->m_iViewType==0)		//图层视图
			hViewItem=pTreeCtrl->InsertItem(pView->name,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hLayerViewSetItem);
		else if(pView->m_iViewType==1)	//定制视图
		{
			hFolderItem=pTreeCtrl->GetNextItem(hDefinedViewSetItem,TVGN_CHILD);
			while(hFolderItem!=NULL)
			{
				pItemInfo=(TREEITEMDISPLAYVIEW_INFO*)pTreeCtrl->GetItemData(hFolderItem);
				if(pItemInfo==NULL||pItemInfo->itemType!=0)
				{
					hFolderItem=pTreeCtrl->GetNextSiblingItem(hFolderItem);
					continue;	//其它类型的节点
				}
				else
				{	//视图夹节点
					CString sFolderName=pTreeCtrl->GetItemText(hFolderItem);
					if(sFolderName.CompareNoCase(pView->folderName)==0)
					{
						hViewItem=pTreeCtrl->InsertItem(pView->name,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hFolderItem,TVI_SORT);
						break;
					}
				}
				hFolderItem=pTreeCtrl->GetNextSiblingItem(hFolderItem);
			}
			if(hViewItem==NULL)
			{
				if(strlen(pView->folderName)==0)
					hViewItem=pTreeCtrl->InsertItem(pView->name,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hDefinedViewSetItem,TVI_SORT);
				else
				{
					hFolderItem=pTreeCtrl->InsertItem(pView->folderName,PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hDefinedViewSetItem);
					pItemInfo=itemInfoList.append(TREEITEMDISPLAYVIEW_INFO(0,NULL));
					pTreeCtrl->SetItemData(hFolderItem,(DWORD)pItemInfo);
					hViewItem=pTreeCtrl->InsertItem(pView->name,PRJ_IMG_STATUS,PRJ_IMG_STATUS,hFolderItem,TVI_SORT);
				}
			}
		}
		pItemInfo=itemInfoList.append(TREEITEMDISPLAYVIEW_INFO(1,pView));
		pTreeCtrl->SetItemData(hViewItem,(DWORD)pItemInfo);
	}
	if(pModel->IsTowerModel())
	{
		pTreeCtrl->Expand(hViewGroupItem,TVE_EXPAND);
		pTreeCtrl->Expand(hLayerViewSetItem,TVE_EXPAND);
		pTreeCtrl->Expand(hDefinedViewSetItem,TVE_EXPAND);
	}
}
BOOL CSelCurFaceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
#ifndef __TSA_FILE_
	//初始化段号下拉框
	ATOM_LIST<SEGI> segNoList;
	int nSegNum=Ta.GetSegNoList(segNoList);
	if(nSegNum>0)
	{	//
		CString sItem;
		CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_SEG_NO);
		for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
		{
			sItem.Format("%s",(char*)pSegI->ToString());
			pCMB->AddString(sItem);	
			if(Ta.m_iCurSeg.iSeg<=0)
				Ta.m_iCurSeg=*pSegI;
		}
	}
	m_sSegNo.Format("%s",(char*)Ta.m_iCurSeg.ToString());
	UpdateData(FALSE);
#else
	GetDlgItem(IDC_CMB_SEG_NO)->EnableWindow(FALSE);
#endif
	//
	CTreeCtrl *pTreeCtrl=(CTreeCtrl*)GetDlgItem(IDC_TREE_DISPLAY_VIEW);
	m_ModelImages.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	pTreeCtrl->SetImageList(&m_ModelImages,TVSIL_NORMAL);
	pTreeCtrl->DeleteAllItems();
	itemInfoList.Empty();
	TREEITEMDISPLAYVIEW_INFO* pItemInfo=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	HTREEITEM hBodyItem = pTreeCtrl->InsertItem("tower",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,TVI_ROOT);
#else
	HTREEITEM hBodyItem = pTreeCtrl->InsertItem("杆塔",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,TVI_ROOT);
#endif
	pItemInfo=itemInfoList.append(TREEITEMDISPLAYVIEW_INFO(MODEL_GROUP,NULL));
	pTreeCtrl->SetItemData(hBodyItem,(DWORD)pItemInfo);
	RefreshViewGroup(hBodyItem,&Ta);
	pTreeCtrl->Expand(hBodyItem,TVE_EXPAND);
	for(CBlockModel *pBlock=Ta.Block.GetFirst();pBlock;pBlock=Ta.Block.GetNext())
	{
		CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
		ss.Format("block(0X%X)",pBlock->name,pBlock->handle);
#else
		ss.Format("部件(0X%X)",pBlock->name,pBlock->handle);
#endif
		HTREEITEM hBlockItem;
		if(!pBlock->IsEmbeded())//导入式部件有视图
			hBlockItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_IMPORT_BLOCK_CASE,PRJ_IMG_IMPORT_BLOCK_CASE,TVI_ROOT);
		else
			hBlockItem = pTreeCtrl->InsertItem(ss,PRJ_IMG_IMPORT_BLOCK_CASE,PRJ_IMG_EMBED_BLOCK_CASE,TVI_ROOT);
		pItemInfo=itemInfoList.append(TREEITEMDISPLAYVIEW_INFO(BLOCK_CASE,NULL));
		pTreeCtrl->SetItemData(hBlockItem,(DWORD)pItemInfo);
		RefreshViewGroup(hBlockItem,pBlock);
	}
	return TRUE;
}

void CSelCurFaceDlg::OnOK() 
{
	UpdateData();
	m_pSelectedView=NULL;
	CTreeCtrl *pTreeCtrl=(CTreeCtrl*)GetDlgItem(IDC_TREE_DISPLAY_VIEW);
	HTREEITEM hViewItem=pTreeCtrl->GetSelectedItem();
	if(hViewItem==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("none drawing view selected!!");
#else
		AfxMessageBox("需要选中一个待绘制视图!");
#endif
		return;
	}
	TREEITEMDISPLAYVIEW_INFO *pItemInfo=(TREEITEMDISPLAYVIEW_INFO*)pTreeCtrl->GetItemData(hViewItem);
	if(pItemInfo)
		m_pSelectedView=pItemInfo->pView;
#ifndef __TSA_FILE_
	//判断输入的段号是否合法
	m_xSegNoList.Empty();
	if(m_sSegNo[0]=='*')
		Ta.GetSegNoList(m_xSegNoList);
	else
	{
		CHashList<SEGI> segTbl;
		GetSegNoHashTblBySegStr(m_sSegNo,segTbl);
		for(SEGI *pSegI=segTbl.GetFirst();pSegI;pSegI=segTbl.GetNext())
			m_xSegNoList.append(*pSegI);
	}
	if(m_xSegNoList.GetNodeNum()<=0)
	{
		Ta.m_iCurSeg = SEGI(1);
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("the segment number is illegal,please input again!");
#else
		AfxMessageBox("输入了非法段号,请重新输入!");
#endif
		GetDlgItem(IDC_CMB_SEG_NO)->SetFocus();
		return;
	}
	else
		Ta.m_iCurSeg = *m_xSegNoList.GetFirst();	//记录选中段
#endif
	CDialog::OnOK();
}

void CSelCurFaceDlg::OnDblclkTreeDisplayView(NMHDR* pNMHDR, LRESULT* pResult) 
{	//双击生成选中视图
	UpdateData();
	m_pSelectedView=NULL;
	CTreeCtrl *pTreeCtrl=(CTreeCtrl*)GetDlgItem(IDC_TREE_DISPLAY_VIEW);
	HTREEITEM hViewItem=pTreeCtrl->GetSelectedItem();
	TREEITEMDISPLAYVIEW_INFO *pItemInfo=(TREEITEMDISPLAYVIEW_INFO*)pTreeCtrl->GetItemData(hViewItem);
	if(pItemInfo&&pItemInfo->itemType==1)
	{
		m_pSelectedView=pItemInfo->pView;
#ifndef __TSA_FILE_
		//判断输入的段号是否合法
		m_xSegNoList.Empty();
		if(m_sSegNo[0]=='*')
			Ta.GetSegNoList(m_xSegNoList);
		else
		{
			CHashList<SEGI> segTbl;
			GetSegNoHashTblBySegStr(m_sSegNo,segTbl);
			for(SEGI *pSegI=segTbl.GetFirst();pSegI;pSegI=segTbl.GetNext())
				m_xSegNoList.append(*pSegI);
		}
		if(m_xSegNoList.GetNodeNum()<=0)
		{
			Ta.m_iCurSeg = SEGI(1);
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the segment number is illegal,please input again!");
#else
			AfxMessageBox("输入了非法段号,请重新输入!");
#endif
			GetDlgItem(IDC_CMB_SEG_NO)->SetFocus();
			return;
		}
#endif
		CDialog::OnOK();
	}
	*pResult = 0;
}
void CSelCurFaceDlg::OnTvnSelchangedTreeDisplayView(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	CTreeCtrl *pTreeCtrl=(CTreeCtrl*)GetDlgItem(IDC_TREE_DISPLAY_VIEW);
	HTREEITEM hViewItem=pTreeCtrl->GetSelectedItem();
	if(hViewItem==NULL)
		return;
	TREEITEMDISPLAYVIEW_INFO *pItemInfo=(TREEITEMDISPLAYVIEW_INFO*)pTreeCtrl->GetItemData(hViewItem);
	HTREEITEM hItem=NULL;
	if(pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==MODEL_GROUP)
	{
		pTreeCtrl->Expand(hViewItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetChildItem(hViewItem);
		pTreeCtrl->Expand(hItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetChildItem(hItem);
		pTreeCtrl->Expand(hItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetNextSiblingItem(hItem);
		pTreeCtrl->Expand(hItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetNextSiblingItem(hViewItem);
		pTreeCtrl->Expand(hItem,TVE_COLLAPSE);
	}
	else if(pItemInfo->itemType==BLOCK_CASE)
	{
		pTreeCtrl->Expand(hViewItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetChildItem(hViewItem);
		pTreeCtrl->Expand(hItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetChildItem(hItem);
		pTreeCtrl->Expand(hItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetNextSiblingItem(hItem);
		pTreeCtrl->Expand(hItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetRootItem();
		pTreeCtrl->Expand(hItem,TVE_COLLAPSE);
	}
	else if(pItemInfo->itemType==VIEW_LAYERSET)
	{
		pTreeCtrl->Expand(hViewItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetNextSiblingItem(hViewItem);
		pTreeCtrl->Expand(hItem,TVE_COLLAPSE);
	}
	else if(pItemInfo->itemType==VIEW_DEFINEDSET)
	{
		pTreeCtrl->Expand(hViewItem,TVE_EXPAND);
		hItem=pTreeCtrl->GetParentItem(hViewItem);
		hItem=pTreeCtrl->GetChildItem(hItem);
		pTreeCtrl->Expand(hItem,TVE_COLLAPSE);
	}
	*pResult = 0;
}

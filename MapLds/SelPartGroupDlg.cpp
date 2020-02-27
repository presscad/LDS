//<LOCALE_TRANSLATE/>
// SelPartGroupDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "maptma.h"
#include "SelPartGroupDlg.h"
#include "Tower.h"
#include "DrawTaMap.h"
#include "image.h"
#include "SortFunc.h"
#include "SysPara.h"
#include "MenuFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifndef __TSA_FILE_
/////////////////////////////////////////////////////////////////////////////
// CSelPartGroupDlg dialog

CSelPartGroupDlg::CSelPartGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelPartGroupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelPartGroupDlg)
	m_sCurrentPartGroup = _T("");
	m_sCurrentSectionView = _T("");
	m_bIgnoreStdPartGroup = FALSE;	//忽略标准组焊件不统计
	//}}AFX_DATA_INIT
	m_hSelPartGroup=0;
	m_pSelSectionView=NULL;
	m_bPartGroupBOM=FALSE;	//生成构件明细表
	m_pWeldPartSet=NULL;
	m_bIncProcessCard = sys.general_set.bProcessCardTbl;
	m_sProcessCardPathFile = sys.general_set.m_sWeldPartGroupCard;
}


void CSelPartGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelPartGroupDlg)
	DDX_Check(pDX, IDC_CHK_INC_PROCESS_CARD, m_bIncProcessCard);
	DDX_Text (pDX, IDC_E_SPEC_WELDPART_CARD_PATHFILE, m_sProcessCardPathFile);
	DDX_Check(pDX, IDC_CHK_IGNORE_STD_PARTGROUP, m_bIgnoreStdPartGroup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelPartGroupDlg, CDialog)
	//{{AFX_MSG_MAP(CSelPartGroupDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PART_GROUP, OnSelchangedTreePartGroup)
	ON_BN_CLICKED(IDC_CHK_IGNORE_STD_PARTGROUP, OnIgnoreStdPartgroup)
	ON_BN_CLICKED(IDC_BN_SPEC_WELDPART_CARD_PATHFILE, &CSelPartGroupDlg::OnBnSpecWeldpartCardPathfile)
	ON_BN_CLICKED(IDC_CHK_INC_PROCESS_CARD, &CSelPartGroupDlg::OnChkIncProcessCard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelPartGroupDlg message handlers

BOOL CSelPartGroupDlg::OnInitDialog() 
{
	m_bIncProcessCard=sys.general_set.bProcessCardTbl;
	m_sProcessCardPathFile=sys.general_set.m_sWeldPartGroupCard;
	GetDlgItem(IDC_E_SPEC_WELDPART_CARD_PATHFILE)->EnableWindow(m_bIncProcessCard);
	GetDlgItem(IDC_BN_SPEC_WELDPART_CARD_PATHFILE)->EnableWindow(m_bIncProcessCard);
	CDialog::OnInitDialog();
	RefreshModel();
	return TRUE; 
}

void CSelPartGroupDlg::OnSelchangedTreePartGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hSelItem=pNMTreeView->itemNew.hItem;
	CTreeCtrl *pPartGroupTreeCtrl=(CTreeCtrl*)GetDlgItem(IDC_TREE_PART_GROUP);
	TREEITEMPARTGROUP_INFO *pItemInfo=(TREEITEMPARTGROUP_INFO*)pPartGroupTreeCtrl->GetItemData(hSelItem);
	CLDSPartGroup *pSelPartGroup=NULL;
	CSectionView *pSelView=NULL;
	m_sFolderName = "";
	if(pItemInfo&&pItemInfo->itemType==1)		//当前选中的是组合焊件
	{
		pSelPartGroup=(CLDSPartGroup*)pItemInfo->pObj;
	}
	else if(pItemInfo&&pItemInfo->itemType==2)	//当前选中的是视图
	{
		pSelView=(CSectionView*)pItemInfo->pObj;
		HTREEITEM hPartGroupItem=pPartGroupTreeCtrl->GetParentItem(hSelItem);
		if(hPartGroupItem!=NULL)
		{
			pItemInfo=(TREEITEMPARTGROUP_INFO*)pPartGroupTreeCtrl->GetItemData(hPartGroupItem);
			if(pItemInfo&&pItemInfo->itemType==1)
				pSelPartGroup=(CLDSPartGroup*)pItemInfo->pObj;
		}
	}
	if(pSelPartGroup)
	{
		m_hSelPartGroup=pSelPartGroup->handle;
		m_sCurrentPartGroup=pSelPartGroup->sGroupName;
	}
	else
	{
		m_sFolderName = pPartGroupTreeCtrl->GetItemText(hSelItem);
		m_hSelPartGroup=0;
		m_sCurrentPartGroup="";
	}
	if(pSelView)
	{
		m_pSelSectionView=pSelView;
		m_sCurrentSectionView=pSelView->sName;
	}
	else
	{
		m_pSelSectionView=NULL;
		m_sCurrentSectionView="";
	}
	UpdateData(FALSE);
	*pResult = 0;
}

/*void CSelPartGroupDlg::OnSelchangePartGroupList() 
{
	CListBox *pPartGroupList=(CListBox*)GetDlgItem(IDC_PART_GROUP_LIST);
	CListBox *pSectionViewList=(CListBox*)GetDlgItem(IDC_SECTION_VIEW_LIST);
	UpdateData();
	long h=(long)pPartGroupList->GetItemData(m_iPartGroup);
	pSectionViewList->ResetContent();
	pSectionViewList->AddString("主视图");
	for(CLDSPartGroup *pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		if(pPartGroup->handle==h)
		{
			for(CSectionView *pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
				pSectionViewList->AddString(pView->sName);
		}
	}
}*/
#include "ComparePartNoString.h"
typedef CLDSPartGroup* CLDSPartGroupPtr;
static int ComparePartGroupLabel(const CLDSPartGroupPtr& item1,const CLDSPartGroupPtr& item2)
{
	return ComparePartNoString(item1->sGroupName,item2->sGroupName,"H");
}
void CSelPartGroupDlg::RefreshModel()
{
	CTreeCtrl *pPartGroupTreeCtrl=(CTreeCtrl*)GetDlgItem(IDC_TREE_PART_GROUP);
	m_ModelImages.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	pPartGroupTreeCtrl->SetImageList(&m_ModelImages,TVSIL_NORMAL);
	pPartGroupTreeCtrl->DeleteAllItems();

	HTREEITEM hRootItem,hFolderItem,hNewPartGroupItem;
#ifdef AFX_TARG_ENU_ENGLISH
	hRootItem=pPartGroupTreeCtrl->InsertItem("group weld parts",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
#else
	hRootItem=pPartGroupTreeCtrl->InsertItem("组合焊件",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
#endif
	CLDSPartGroup *pPartGroup=NULL;
	ARRAY_LIST<CLDSPartGroup*>listPartGroups(0,Ta.PartGroup.GetNodeNum());
	for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		if(m_bIgnoreStdPartGroup&&pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ENDCONN)
			continue;	//忽略标准组焊件不统计
		listPartGroups.append(pPartGroup);
	}
	CHeapSort<CLDSPartGroup*>::HeapSort(listPartGroups.m_pData,listPartGroups.GetSize(),ComparePartGroupLabel);
	for(int i=0;i<listPartGroups.GetSize();i++)
	{
		pPartGroup=listPartGroups[i];
		hNewPartGroupItem=NULL;
		hFolderItem=pPartGroupTreeCtrl->GetNextItem(hRootItem,TVGN_CHILD);
		while(hFolderItem!=NULL)
		{
			CString sFolderName=pPartGroupTreeCtrl->GetItemText(hFolderItem);
			int retCode=sFolderName.CompareNoCase(pPartGroup->sFolderName);
			if(retCode==0)
			{	//应把当前组合焊件加到此文件夹下
				hNewPartGroupItem=pPartGroupTreeCtrl->InsertItem(pPartGroup->sGroupName,PRJ_IMG_PARTGROUP,PRJ_IMG_PARTGROUP,hFolderItem);//,TVI_SORT);
				TREEITEMPARTGROUP_INFO *pItemInfo=itemInfoList.append(TREEITEMPARTGROUP_INFO(1,pPartGroup));
				pPartGroupTreeCtrl->SetItemData(hNewPartGroupItem,(DWORD)pItemInfo);
				break;
			}
			else
				hFolderItem=pPartGroupTreeCtrl->GetNextSiblingItem(hFolderItem);
		}
		if(hFolderItem==NULL)
		{	//新文夹名称
			hFolderItem=pPartGroupTreeCtrl->InsertItem(pPartGroup->sFolderName,PRJ_IMG_STATUSSET,PRJ_IMG_STATUSSET,hRootItem,TVI_SORT);
			TREEITEMPARTGROUP_INFO *pItemInfo=itemInfoList.append(TREEITEMPARTGROUP_INFO(0,NULL));
			pPartGroupTreeCtrl->SetItemData(hFolderItem,(DWORD)pItemInfo);
			hNewPartGroupItem=pPartGroupTreeCtrl->InsertItem(pPartGroup->sGroupName,PRJ_IMG_PARTGROUP,PRJ_IMG_PARTGROUP,hFolderItem);
			pItemInfo=itemInfoList.append(TREEITEMPARTGROUP_INFO(1,pPartGroup));
			pPartGroupTreeCtrl->SetItemData(hNewPartGroupItem,(DWORD)pItemInfo);
		}
		if(hNewPartGroupItem!=NULL)
		{	//添加各组合焊件的视图节点
			CSectionView *pView=NULL;
			CTmaPtrList<CSectionView,CSectionView*> viewPtrList;
			for(pView=pPartGroup->SonViews.GetFirst();pView;pView=pPartGroup->SonViews.GetNext())
			{
				CSectionView *pTmpView=NULL;
				for(pTmpView=viewPtrList.GetFirst();pTmpView;pTmpView=viewPtrList.GetNext())
				{
					if(pTmpView->index>pView->index)
					{
						viewPtrList.insert(pView);
						break;
					}
				}
				if(pTmpView==NULL)
					viewPtrList.append(pView);
			}
			for(pView=viewPtrList.GetFirst();pView;pView=viewPtrList.GetNext())
			{
				CXhChar100 sxName;
				if(pView->IsSlaveView())
					sxName.Printf("<%s-%s>",pView->Symbol,pView->Symbol);
				sxName.Append(pView->sName);
				HTREEITEM hViewItem=pPartGroupTreeCtrl->InsertItem(sxName,PRJ_IMG_LAYERVIEW,PRJ_IMG_LAYERVIEW,hNewPartGroupItem,TVI_LAST);
				TREEITEMPARTGROUP_INFO *pItemInfo=itemInfoList.append(TREEITEMPARTGROUP_INFO(2,pView));
				pPartGroupTreeCtrl->SetItemData(hViewItem,(DWORD)pItemInfo);
			}
		}
	}
	pPartGroupTreeCtrl->Expand(hRootItem,TVE_EXPAND);
	hFolderItem=pPartGroupTreeCtrl->GetNextItem(hRootItem,TVGN_CHILD);
	while(hFolderItem!=NULL)
	{
		pPartGroupTreeCtrl->Expand(hFolderItem,TVE_EXPAND);
		hFolderItem=pPartGroupTreeCtrl->GetNextSiblingItem(hFolderItem);
	}
	UpdateData(FALSE);	
}

void CSelPartGroupDlg::OnIgnoreStdPartgroup() 
{	
	UpdateData();
	RefreshModel();
	UpdateData(FALSE);
}

void CSelPartGroupDlg::OnOK() 
{
	if(m_pSelSectionView==NULL&&m_sFolderName.GetLength()<=0
		&&m_hSelPartGroup<0x20&&m_sSectionViewName.GetLength()<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("No proper group weld parts selected，please select again!");
#else
		AfxMessageBox("没选中合适的组焊件，请重新选择!");
#endif
		return;
	}
	CDialog::OnOK();
	if(m_pWeldPartSet==NULL)
		return;
	//根据当前选中树节点填充组焊件集合
	CLDSPartGroup *pPartGroup=NULL;
	if(m_sFolderName.GetLength()>0)
	{
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(m_bIgnoreStdPartGroup&&pPartGroup->m_iPartGroupType==CLDSPartGroup::GROUP_ENDCONN)
				continue;	//忽略标准组焊件
#ifdef AFX_TARG_ENU_ENGLISH
			if(m_sFolderName.CompareNoCase(pPartGroup->sFolderName)==0||m_sFolderName.CompareNoCase("weld parts group")==0)
#else
			if(m_sFolderName.CompareNoCase(pPartGroup->sFolderName)==0||m_sFolderName.CompareNoCase("组合焊件")==0)
#endif
			{	//如果选中【组合焊件】则生成所有的组焊件 
				CLDSPartGroup *pTempPartGroup=NULL;
				for(pTempPartGroup=m_pWeldPartSet->GetFirst();pTempPartGroup;pTempPartGroup=m_pWeldPartSet->GetNext())
				{
					//if(stricmp(pTempPartGroup->sGroupName,pPartGroup->sGroupName)>0)
					if(ComparePartNoString(pTempPartGroup->sGroupName,pPartGroup->sGroupName)>0)
					{
						m_pWeldPartSet->insert(pPartGroup);
						break;
					}
				}
				if(pTempPartGroup==NULL)
					m_pWeldPartSet->append(pPartGroup);
			}
		}
	}
	else
	{
		for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
		{
			if(pPartGroup->handle==m_hSelPartGroup)
				break;
		}
		if(pPartGroup)
			m_pWeldPartSet->append(pPartGroup);
	}
	//保存工艺卡图框配置
	SysSetExportDefault(FALSE);
}

void CSelPartGroupDlg::OnChkIncProcessCard()
{
	UpdateData();
	sys.general_set.bProcessCardTbl=m_bIncProcessCard;
	GetDlgItem(IDC_E_SPEC_WELDPART_CARD_PATHFILE)->EnableWindow(m_bIncProcessCard);
	GetDlgItem(IDC_BN_SPEC_WELDPART_CARD_PATHFILE)->EnableWindow(m_bIncProcessCard);
}
void CSelPartGroupDlg::OnBnSpecWeldpartCardPathfile()
{
	CFileDialog cardFileDlg(TRUE,"dwg",sys.general_set.m_sWeldPartGroupCard,NULL,"组焊件工艺卡(*.dwg)|*.dwg||");
	cardFileDlg.m_ofn.lpstrTitle = "Select file";
	if(IDOK!=cardFileDlg.DoModal())
		return;
	StrCopy(sys.general_set.m_sWeldPartGroupCard,cardFileDlg.GetPathName(),260);
	m_sProcessCardPathFile=sys.general_set.m_sWeldPartGroupCard;
	UpdateData(FALSE);
}
#endif

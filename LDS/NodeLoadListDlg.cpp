// NodeLoadListDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "lds.h"
#include "NodeLoadListDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__TSA_)||defined(__LDS_)
/////////////////////////////////////////////////////////////////////////////
// CNodeLoadListDlg dialog
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CNodeLoadListDlg *pNodeLoadListDlg=(CNodeLoadListDlg*)pListCtrl->GetParent();
	if(pNodeLoadListDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	int nCurRow = pListCtrl->GetSelectedItem();
	int nCurCol = pListCtrl->GetCurSubItem();
	CRect rectItem;
	pListCtrl->GetItemRect(nCurRow,rectItem,LVIR_BOUNDS);
	if(!rectItem.PtInRect(point))
		return FALSE;	//鼠标不在当前选中项上
	if(nCurRow==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_LOAD_NODE,"New Load Node");
		pMenu->AppendMenu(MF_STRING,ID_EXPAND_ITEM,"Spread All Node");
		pMenu->AppendMenu(MF_STRING,ID_COLLAPSE_ITEM,"Close All Node");
#else 
		pMenu->AppendMenu(MF_STRING,ID_NEW_LOAD_NODE,"新建荷载节点");
		pMenu->AppendMenu(MF_STRING,ID_EXPAND_ITEM,"展开所有节点");
		pMenu->AppendMenu(MF_STRING,ID_COLLAPSE_ITEM,"关闭所有展开节点");
#endif
	}
	else if(nCurRow!=0&&nCurCol==0)
	{
		CSuperGridCtrl::CTreeItem *pCurItem=pListCtrl->GetTreeItem(pListCtrl->GetSelectedItem());
		if (pListCtrl->GetRootItem(0)==pListCtrl->GetParentItem(pCurItem))
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Load Node");
#else 
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除荷载节点");
#endif
	}
	else if (nCurRow>0&&nCurCol>1)
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"Paste Load");
#else 
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"粘贴荷载");
#endif
	else
		return FALSE;
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pNodeLoadListDlg);
	return TRUE;
}
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	if(iSubItem==0)
	{
		CLDSNode* pNode = NULL;
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			char sNodeID[5] = {'\0'};
			sprintf(sNodeID,"%d",pNode->point_i);
			if (sTextValue == sNodeID)
			{
				int nNum = pListCtrl->GetItemCount();
				for(int i = 0;i<nNum;i++)
				{
					CString sID = pListCtrl->GetItemText(i,0);
					if (sID == sTextValue)
					{	
#ifdef AFX_TARG_ENU_ENGLISH
						MessageBox(NULL,"Repeat to input node number,please input again!","ERROR",MB_OK);
#else 
						MessageBox(NULL,"输入重复节点号,请重新输入!","ERROR",MB_OK);
#endif
						return FALSE;
					}
				}
				break;
			}
		}
		if(pNode==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox(NULL,"The input node number is wrong,please input again!","ERROR",MB_OK);
#else 
			MessageBox(NULL,"输入节点号有误,请重新输入!","ERROR",MB_OK);
#endif
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL FireButtonClick(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem)
{
	return TRUE;
}

CNodeLoadListDlg::CNodeLoadListDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CNodeLoadListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNodeLoadListDlg)
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrl.AddColumnHeader("Node Number");
	m_listCtrl.AddColumnHeader("Work Condition Name");
	m_listCtrl.AddColumnHeader("X Component(kN)");
	m_listCtrl.AddColumnHeader("Y Component(kN)");
	m_listCtrl.AddColumnHeader("Z(kN)");
	m_listCtrl.AddColumnHeader("Gravity(kN)");
#else 
	m_listCtrl.AddColumnHeader("节点号");
	m_listCtrl.AddColumnHeader("工况名称");
	m_listCtrl.AddColumnHeader("X分量(kN)");
	m_listCtrl.AddColumnHeader("Y分量(kN)");
	m_listCtrl.AddColumnHeader("Z量(kN)");
	m_listCtrl.AddColumnHeader("重力(kN)");
#endif
	m_iWireLayoutNo=1;
}


void CNodeLoadListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNodeLoadListDlg)
	DDX_Control(pDX, IDC_LIST_NODE_LOAD, m_listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNodeLoadListDlg, CDialog)
	//{{AFX_MSG_MAP(CNodeLoadListDlg)
	ON_COMMAND(ID_NEW_LOAD_NODE, OnNewData)
	ON_COMMAND(ID_PASTE_ITEM, OnPasteData)
	ON_COMMAND(ID_EXPAND_ITEM, OnExpandAll)
	ON_COMMAND(ID_COLLAPSE_ITEM, OnShutAll)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteNode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNodeLoadListDlg message handlers

BOOL CNodeLoadListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	long col_wide_arr[6]={80,150,80,80,80,80};
	m_listCtrl.InitListCtrl(col_wide_arr);
	m_listCtrl.SetModifyValueFunc(FireValueModify);
	m_listCtrl.SetButtonClickFunc(FireButtonClick);
	m_listCtrl.SetContextMenuFunc(FireContextMenu);
	CLDSNode *pNode=NULL;
	CTmaPtrList<CLDSNode,CLDSNode*>  NodeSet;
	for (pNode = Ta.Node.GetFirst();pNode;pNode = Ta.Node.GetNext())
	{
		for(CWorkStatus *pWorkStatus=Ta.WorkStatus.GetFirst();pWorkStatus;pWorkStatus=Ta.WorkStatus.GetNext())
		{
			if(pWorkStatus->iWireLayoutNo!=m_iWireLayoutNo)
				continue;
			CExternalNodeLoad *pLoad=pWorkStatus->hashNodeLoad.GetValue(pNode->handle);
			if (!(pLoad==NULL||pLoad->Fxyz.mod2()<EPS))
			{
				NodeSet.append(pNode);
				break;
			}
		}
	}
	CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpRoot->AddSubItemText("Node Collection",TRUE);
#else 
	lpRoot->AddSubItemText("节点集合",TRUE);
#endif
	static CSuperGridCtrl::CTreeItem * pMainRoot = NULL;
	pMainRoot = m_listCtrl.InsertRootItem(lpRoot);
	if(pMainRoot==NULL)
		return FALSE;
	m_listCtrl.SetItemState(pMainRoot->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
	//
	for (pNode=NodeSet.GetFirst();pNode;pNode=NodeSet.GetNext())
	{
		BOOL bRoot = TRUE;
		for (CWorkStatus *pWorkStatus=Ta.WorkStatus.GetFirst();pWorkStatus;pWorkStatus = Ta.WorkStatus.GetNext())
		{
			if(pWorkStatus->iWireLayoutNo!=m_iWireLayoutNo)
				continue;
			CExternalNodeLoad *pLoad=pWorkStatus->hashNodeLoad.GetValue(pNode->handle);
			if (pLoad==NULL||pLoad->Fxyz.mod2()<EPS)
				continue;
			else
			{
				CListCtrlItemInfo* lpSon = new CListCtrlItemInfo();
				CString ss;
				if (bRoot)
				{
					ss.Format("%d",pNode->point_i);
					lpSon->AddSubItemText(ss);
				}
				else
					lpSon->AddSubItemText(_T(""),TRUE);
				lpSon->AddSubItemText(CString(pWorkStatus->description),TRUE);
				ss.Format("%.3f",pLoad->Fxyz.x/1000);
				SimplifiedNumString(ss);
				lpSon->AddSubItemText(ss);
				ss.Format("%.3f",pLoad->Fxyz.y/1000);
				SimplifiedNumString(ss);
				lpSon->AddSubItemText(ss);
				ss.Format("%.3f",pLoad->Fxyz.z/1000);
				SimplifiedNumString(ss);
				lpSon->AddSubItemText(ss);
				ss.Format("%.3f",pLoad->permanentFz/1000);
				SimplifiedNumString(ss);
				lpSon->AddSubItemText(ss);
				static CSuperGridCtrl::CTreeItem * pRoot = NULL;
				if (bRoot)
				{
					pRoot = m_listCtrl.InsertItem(pMainRoot,lpSon,TRUE);
					if(pRoot==NULL)
						return FALSE;
					if(pWorkStatus)
						pRoot->m_idProp=pWorkStatus->iNo;
					m_listCtrl.SetItemState(pRoot->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
				}
				else
				{
					CSuperGridCtrl::CTreeItem * pSonItem = m_listCtrl.InsertItem(pRoot,lpSon,TRUE);
					if(pSonItem==NULL)
						return FALSE;
					m_listCtrl.SetItemState(pSonItem->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
					
				}
				bRoot = FALSE;
			}
		}
	}	
	return TRUE; 
}

void CNodeLoadListDlg::OnNewData()
{
	CWorkStatus* pWorkStatus=NULL;
	double nNum = Ta.WorkStatus.GetNodeNum();
	CListCtrlItemInfo* lp = new CListCtrlItemInfo();
	lp->AddSubItemText(_T(""));
	pWorkStatus=Ta.WorkStatus.GetFirst();
	if(pWorkStatus)
		lp->AddSubItemText(CString(pWorkStatus->description),TRUE);
	else
		lp->AddSubItemText(_T(""),TRUE);
	lp->AddSubItemText(_T("0"));
	lp->AddSubItemText(_T("0"));
	lp->AddSubItemText(_T("0"));
	lp->AddSubItemText(_T("0"));
	CSuperGridCtrl::CTreeItem * pRoot = m_listCtrl.InsertItem(m_listCtrl.GetRootItem(0),lp,TRUE);
	if(pRoot==NULL)
		return;
	if(pWorkStatus)
		pRoot->m_idProp=pWorkStatus->iNo;
	m_listCtrl.SetItemState(pRoot->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
	int i=0;
	for(i=1;i<nNum;i++)
	{
		pWorkStatus=Ta.WorkStatus.GetNext();
		CListCtrlItemInfo* lpSon = new CListCtrlItemInfo();
		lpSon->AddSubItemText(_T(""),TRUE);
		lpSon->AddSubItemText(CString(pWorkStatus->description),TRUE);
		lpSon->AddSubItemText(_T("0"));
		lpSon->AddSubItemText(_T("0"));
		lpSon->AddSubItemText(_T("0"));
		lpSon->AddSubItemText(_T("0"));
		CSuperGridCtrl::CTreeItem * pSonItem = m_listCtrl.InsertItem(pRoot,lpSon,TRUE);
		if(pSonItem==NULL)
			return;
		m_listCtrl.SetItemState(pSonItem->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
	}
	m_listCtrl.Redraw();
}

void CNodeLoadListDlg::OnPasteData()
{
	m_listCtrl.PasteDataFromClipboard(6,NULL,TRUE);
}

void CNodeLoadListDlg:: OnShutAll()
{
	m_listCtrl.Collapse(m_listCtrl.GetRootItem(0));
	m_listCtrl.Redraw();
}

void CNodeLoadListDlg:: OnDeleteNode()
{
	int iCurSel=m_listCtrl.GetSelectedItem();
	m_listCtrl.DeleteItemEx(m_listCtrl.GetTreeItem(iCurSel),iCurSel);
}

void CNodeLoadListDlg:: OnExpandAll()
{
	int nNum = 0;
	m_listCtrl.ExpandAll(m_listCtrl.GetRootItem(0),nNum);
	m_listCtrl.Redraw();
}

void CNodeLoadListDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CNodeLoadListDlg::OnSelectObject() 
{
	m_bPauseBreakExit=TRUE;
	m_bInernalStart=TRUE;
	CDialog::OnOK();
}

void CNodeLoadListDlg::SetPickObjTypeId(int cls_id)
{
	m_iPickObjType=cls_id;
}

void CNodeLoadListDlg::SetCmdPickPromptStr(CString sPromptStr)
{
	m_sCmdPickPrompt=sPromptStr;
}

void CNodeLoadListDlg::OnOK() 
{
	CString sWorkStatus(m_listCtrl.GetItemText(0,1));
	CString sID(m_listCtrl.GetItemText(0,0));
	double nX = atoi(m_listCtrl.GetItemText(0,2));
	double nY = atoi(m_listCtrl.GetItemText(0,3));
	double nZ = atoi(m_listCtrl.GetItemText(0,4));
	double permanentFz = atoi(m_listCtrl.GetItemText(0,5));
	int nNum = m_listCtrl.GetCount();
	int i=0;
	for(i=0;i<nNum;i++)
	{
		if (i>=1)
		{
			sWorkStatus = m_listCtrl.GetItemText(i,1);
			sID = m_listCtrl.GetItemText(i,0);
			nX = atof(m_listCtrl.GetItemText(i,2));
			nY = atof(m_listCtrl.GetItemText(i,3));
			nZ = atof(m_listCtrl.GetItemText(i,4));
			permanentFz = atof(m_listCtrl.GetItemText(i,5));
			int j = i;
			//当前行sID为空时查找上一行sID,直到得到合法sID为止
			while (sID == "")
				sID = m_listCtrl.GetItemText(--j,0);
		}
		CWorkStatus *pStatus = NULL;
		CLDSNode *pNode = NULL;
		for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			if(pStatus->iWireLayoutNo!=m_iWireLayoutNo)
				continue;
			if(sWorkStatus.CompareNoCase(pStatus->description)==0)
			{
				for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
				{
					char sNodeID[5] = {'\0'};
					sprintf(sNodeID,"%d",pNode->point_i);
					if (sNodeID == sID)
					{
						CExternalNodeLoad *pLoad=pStatus->hashNodeLoad.Add(pNode->handle);
						pLoad->Fxyz.x = nX*1000;
						pLoad->Fxyz.y = nY*1000;
						pLoad->Fxyz.z = nZ*1000;
						pLoad->permanentFz = permanentFz*1000; 
					}
				}
			}
		}
	}
	CDialog::OnOK();
}

#endif

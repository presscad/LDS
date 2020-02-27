// TransformWorkStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "GlobalFunc.h"
#include "TransformWorkStatusDlg.h"
#include "Markup.h"
#include "AddNewWorkStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransformWorkStatusDlg dialog

#if defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
CTransformWorkStatusDlg::CTransformWorkStatusDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CTransformWorkStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransformWorkStatusDlg)
	m_sModelLimStr = _T("1");
	//}}AFX_DATA_INIT
	m_iTabGroup=0;
	m_pTower=NULL;
}


void CTransformWorkStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransformWorkStatusDlg)
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_CTRL, m_listCtrl);
	DDX_Text(pDX, IDC_E_MODEL_LIM_STR, m_sModelLimStr);
	DDV_MaxChars(pDX, m_sModelLimStr, 50);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransformWorkStatusDlg, CDialog)
	//{{AFX_MSG_MAP(CTransformWorkStatusDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, OnSelchangeTabGroup)
	ON_BN_CLICKED(IDC_BN_IMPORT_TEMPLATE_XML, OnBnImportTemplateXml)
	ON_COMMAND(ID_EDIT_ITEM_PROP_ITEM, OnEditItemPropItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransformWorkStatusDlg message handlers
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CTransformWorkStatusDlg *pTransformWorkStatusDlg=(CTransformWorkStatusDlg*)pListCtrl->GetParent();
	if(pTransformWorkStatusDlg==NULL)
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
		return FALSE;	//��겻�ڵ�ǰѡ������
	if (pSelItem->m_idProp == 2)
	{
		pMenu->AppendMenu(MF_STRING,ID_EDIT_ITEM_PROP_ITEM,"����");
		//pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ���˹���");
	}
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pTransformWorkStatusDlg);
	return TRUE;
}

static BOOL FireValueModify(CSuperGridCtrl *pListCtrl,CSuperGridCtrl::CTreeItem* pItem,
							int iSubItem, CString &valueStr)
{
	CTransformWorkStatusDlg *pTransformWorkStatusDlg=(CTransformWorkStatusDlg*)pListCtrl->GetParent();
	CString sTextOld = pItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	pListCtrl->SetSubItemText(pItem,iSubItem,valueStr);
	CString sText;
	CHashList<LOADNODEROOM_STRU>* pHashNodeRoom=&pTransformWorkStatusDlg->m_pWireLayout->groupWorkStatus.hashNodeRoom;
	CHashList<RAWWORKSTATUS_STRU>* pHashWorkStatus=&pTransformWorkStatusDlg->m_pWireLayout->groupWorkStatus.hashWorkStatus;
	if (pTransformWorkStatusDlg->m_iTabGroup==0)
	{
		for(LOADNODEROOM_STRU *pNodeRoom=pHashNodeRoom->GetFirst();pNodeRoom;pNodeRoom=pHashNodeRoom->GetNext())
		{
			sText.Format("%d��λ",pHashNodeRoom->GetCursorKey());
			if(iSubItem==1&&sText.CompareNoCase(pItem->m_lpNodeInfo->GetSubItemText(0))==0)
			{
				for(CWireLoadModel *pWireModel=pTransformWorkStatusDlg->m_pWireLayout->hashWireLoadModel.GetFirst();
				pWireModel;pWireModel=pTransformWorkStatusDlg->m_pWireLayout->hashWireLoadModel.GetNext())
				{
					if(valueStr.CompareNoCase(pWireModel->name)==0)
					{
						pNodeRoom->m_iRawWireLoadNo=pWireModel->iNo;
						break;
					}
				}
			}
			else if (iSubItem!=1&&strcmp(sText,pListCtrl->GetParentItem(pItem)->m_lpNodeInfo->GetSubItemText(0))==0)
			{	
				if (iSubItem == 0)
				{
					long oldkey = atoi(sTextOld); 
					long newkey = atoi(valueStr);
					WIRENODE_STRU* pNodeStru = pNodeRoom->hashNode.ModifyKeyId(oldkey,newkey);
					if(pNodeStru)
					{
						for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
						{
							if(pNode->point_i==newkey)
							{
								pNodeStru->hNode=pNode->handle;
								break;
							}
						}
					}
					else
						AfxMessageBox("�ڵ��滻ʧ��!");
					/*
					if(oldkey!=newkey)
					{
						WIRENODE_STRU* pNodeStru = pNodeRoom->hashNode.GetValue(oldkey);
						WIRENODE_STRU* pTempNode = pNodeRoom->hashNode.Add(newkey);
						if(pNodeStru)
						{
							pTempNode->fLoadAllocCoefX = pNodeStru->fLoadAllocCoefX;
							pTempNode->fLoadAllocCoefY = pNodeStru->fLoadAllocCoefY;
							pTempNode->fLoadAllocCoefZ = pNodeStru->fLoadAllocCoefZ;
						}
						pNodeRoom->hashNode.DeleteNode(oldkey);
						for(long *pWorkEnvKey=pNodeRoom->hashWorkEnv.GetFirst();pWorkEnvKey;pWorkEnvKey=pNodeRoom->hashWorkEnv.GetNext())
							pTempNode->hashWorkEnv.SetValue(pNodeRoom->hashWorkEnv.GetCursorKey(),*pWorkEnvKey);
					}
					*/
				}
				else if(iSubItem == 2)
				{	//���ط���ϵ��
					sText = pItem->m_lpNodeInfo->GetSubItemText(0);
					WIRENODE_STRU* pNodeStru = pNodeRoom->hashNode.GetValue(atoi(sText));
					if (pNodeStru == NULL)
					{
						WIRENODE_STRU* pNode = pNodeRoom->hashNode.Add(atoi(sText));
						pNode->fLoadAllocCoefX = atof(valueStr);
					}
					else
						pNodeStru->fLoadAllocCoefX = atof(valueStr);
				}
				else if(iSubItem == 3)
				{	//���ط���ϵ��
					sText = pItem->m_lpNodeInfo->GetSubItemText(0);
					WIRENODE_STRU* pNodeStru = pNodeRoom->hashNode.GetValue(atoi(sText));
					if (pNodeStru == NULL)
					{
						WIRENODE_STRU* pNode = pNodeRoom->hashNode.Add(atoi(sText));
						pNode->fLoadAllocCoefY = atof(valueStr);
					}
					else
						pNodeStru->fLoadAllocCoefY = atof(valueStr);
				}
				else if(iSubItem == 4)
				{	//���ط���ϵ��
					sText = pItem->m_lpNodeInfo->GetSubItemText(0);
					WIRENODE_STRU* pNodeStru = pNodeRoom->hashNode.GetValue(atoi(sText));
					if (pNodeStru == NULL)
					{
						WIRENODE_STRU* pNode = pNodeRoom->hashNode.Add(atoi(sText));
						pNode->fLoadAllocCoefZ = atof(valueStr);
					}
					else
						pNodeStru->fLoadAllocCoefZ = atof(valueStr);
				}
				break;
			}	
		}
	}
	else if(pTransformWorkStatusDlg->m_iTabGroup==1)
	{
		if (iSubItem==1&&pItem->m_idProp==3)
		{	//���ؽڵ��¼
			sText=pListCtrl->GetParentItem(pItem)->m_lpNodeInfo->GetSubItemText(0);
			long nWorkStatusKey;
			for(RAWWORKSTATUS_STRU *pRawStatus=pHashWorkStatus->GetFirst();pRawStatus;pRawStatus=pHashWorkStatus->GetNext())
			{
				if(strcmp(pRawStatus->name,sText)==0)
				{
					nWorkStatusKey=pHashWorkStatus->GetCursorKey();
					break;
				}
			}
			LOADNODEROOM_STRU *pNodeRoom = pHashNodeRoom->GetValue(pItem->m_lpNodeInfo->m_lParam);
			if(pNodeRoom==NULL)
				return FALSE;
			sText = pItem->m_lpNodeInfo->GetSubItemText(0);
			WIRENODE_STRU* pNodeStru = pNodeRoom->hashNode.GetValue(atoi(sText));
			if(pNodeStru==NULL)
				return FALSE;
			long *pWorkStyle = pNodeStru->hashWorkEnv.GetValue(nWorkStatusKey);
			if(valueStr == "���")
				*pWorkStyle = WORKENVLOAD_WIND ;
			else if(valueStr == "����")
				*pWorkStyle = WORKENVLOAD_ICE;
			else if(valueStr == "����")
				*pWorkStyle = WORKENVLOAD_LOWTEMP;
			else if(valueStr == "�������ѱ�")
				*pWorkStyle = WORKENVLOAD_ICEDIFF;
			else if(valueStr == "�ѱ������ٱ�")
				*pWorkStyle = WORKENVLOAD_ICELESS;
			else if(valueStr == "�ѱ��������")
				*pWorkStyle = WORKENVLOAD_ICEMORE;
			else if(valueStr == "����������")
				*pWorkStyle = WORKENVLOAD_DUANXIANRUNNING;
			else if(valueStr == "�����¹���")
				*pWorkStyle = WORKENVLOAD_DUANXIANBREAK;
			else if(valueStr == "�Ѱ�װ��")
				*pWorkStyle = WORKENVLOAD_SETUPSUSPEND;
			else if(valueStr == "������")
				*pWorkStyle = WORKENVLOAD_SETUPSUSPENDING;
			else if(valueStr == "�������ѹ�")
				*pWorkStyle = WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH;
			else if(valueStr == "������δ��")
				*pWorkStyle = WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY;
			else if(valueStr == "��ê��")
				*pWorkStyle = WORKENVLOAD_ANCHORFINISHED;
			else if(valueStr == "��ê��")
				*pWorkStyle = WORKENVLOAD_SETUPANCHORING;
			else if(valueStr == "��������")
				*pWorkStyle = WORKENVLOAD_EARTHQUAKE;
			else
				*pWorkStyle = 0;	//δ����
		}
	}
	return TRUE;
}
static BOOL FireButtonClick(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem)
{
	CString sText;
	CTransformWorkStatusDlg *pTransformWorkStatusDlg=(CTransformWorkStatusDlg*)pListCtrl->GetParent();
	CHashList<LOADNODEROOM_STRU>* pHashNodeRoom=&pTransformWorkStatusDlg->m_pWireLayout->groupWorkStatus.hashNodeRoom;
	if(pTransformWorkStatusDlg->m_iTabGroup==0)
	{
		if(iSubItem==0)
		{
			pTransformWorkStatusDlg->SelectObject(CLS_NODE,pSelItem->m_idProp);
		}
		else if(iSubItem==5)
		{
			if (pListCtrl->GetParentItem(pSelItem)==NULL)//���
			{
				CListCtrlItemInfo *lpInfo = new CListCtrlItemInfo();
				lpInfo->SetSubItemText(0,"0",FALSE);
				lpInfo->SetControlType(0,GCT_EDIT);
				lpInfo->SetSubItemText(2,"1",FALSE);
				lpInfo->SetControlType(2,GCT_EDIT);
				lpInfo->SetSubItemHelpText(2,"����X��������ϵ��");
				lpInfo->SetSubItemText(3,"1",FALSE);
				lpInfo->SetControlType(3,GCT_EDIT);
				lpInfo->SetSubItemHelpText(3,"����Y��������ϵ��");
				lpInfo->SetSubItemText(4,"1",FALSE);
				lpInfo->SetControlType(4,GCT_EDIT);
				lpInfo->SetSubItemHelpText(4,"����Z��������ϵ��");
				lpInfo->SetSubItemText(5,"ɾ��",FALSE);
				lpInfo->SetControlType(5,GCT_GRAYBUTTON);
				CSuperGridCtrl::CTreeItem* pNewItem=pTransformWorkStatusDlg->m_listCtrl.InsertItem(pSelItem,lpInfo,-1,true);
				pListCtrl->SelectItem(pNewItem,0,TRUE);
			}
			else//ɾ��
			{
				sText=pListCtrl->GetParentItem(pSelItem)->m_lpNodeInfo->GetSubItemText(0);
				int nNodeRoom=atoi(sText);		
				LOADNODEROOM_STRU *pNodeRoom = pHashNodeRoom->GetValue(nNodeRoom);
				sText = pSelItem->m_lpNodeInfo->GetSubItemText(0);
				pNodeRoom->hashNode.DeleteNode(atoi(sText));
				pListCtrl->DeleteItemEx(pSelItem,pSelItem->GetIndex());
				pListCtrl->Redraw();
			}
		}
	}
	else if(pTransformWorkStatusDlg->m_iTabGroup==1 && iSubItem==2)
	{
		CHashList<RAWWORKSTATUS_STRU>* pHashWorkStatus=&pTransformWorkStatusDlg->m_pWireLayout->groupWorkStatus.hashWorkStatus;
		if (pSelItem->m_idProp==1)//��ӹ���
		{
			CAddNewWorkStatus newDlg;
			int key = pTransformWorkStatusDlg->m_pWireLayout->groupWorkStatus.EnumIdleRawStatusNo();
			RAWWORKSTATUS_STRU *pWorkStatus = pHashWorkStatus->Add(key);
			CString sText = pSelItem->m_lpNodeInfo->GetSubItemText(0);//���ҹ�������
			if(0==strcmp(sText,"��繤��"))
			{
				pWorkStatus->m_cMainLoadType = 0;
				newDlg.m_sWorkStatus="���";
			}
			else if(0==strcmp(sText,"��������"))
			{
				pWorkStatus->m_cMainLoadType = 1;
				newDlg.m_sWorkStatus="����";
			}
			
			else if (0==strcmp(sText,"���¹���"))
			{
				pWorkStatus->m_cMainLoadType = 2;
				newDlg.m_sWorkStatus="�������";	
			}
			else if(0==strcmp(sText,"�¹ʹ���"))
			{
				pWorkStatus->m_cMainLoadType = 3;
				newDlg.m_sWorkStatus="����";
			}
			else if(0==strcmp(sText,"��װ����"))
			{
				pWorkStatus->m_cMainLoadType = 4;
				newDlg.m_sWorkStatus="��װ";
			}
			else if(0==strcmp(sText,"���㹤��"))
			{
				pWorkStatus->m_cMainLoadType = 5;
				newDlg.m_sWorkStatus="����";
			}
			newDlg.m_fIceThick = pTransformWorkStatusDlg->m_pTower->weather.m_fMaxIceThick;
			newDlg.m_fMaxWind = pTransformWorkStatusDlg->m_pTower->weather.m_fMaxWindV;
			
			if(IDOK != newDlg.DoModal())
			{
				pHashWorkStatus->DeleteNode(key);
				return FALSE;
			}
			strncpy(pWorkStatus->lim_str,newDlg.m_sModelLimStr,50);
			pWorkStatus->ice_thick = newDlg.m_fIceThick;
			strcpy(pWorkStatus->name,newDlg.m_sWorkStatus);
			pWorkStatus->m_bMinVertSpace=(newDlg.m_bMinVertSpace!=FALSE);
			pWorkStatus->WindLoad.azimuth = newDlg.m_fWindAngle;
			pWorkStatus->WindLoad.velocity = newDlg.m_fMaxWind;
			if(pWorkStatus->m_cMainLoadType>=5)
			{
				pWorkStatus->m_cHoriEarthQuakeAxis=newDlg.m_cHoriEqAxisAzimuth;//'X';
				strncpy(pWorkStatus->vibration_mode_str,newDlg.m_sModelLimStr,50);
			}

			CString sNaiZhangWorkEnvItems="δ����|���|����|����|�ѱ������ٱ�|�ѱ��������|����������|�����¹���|�������ѹ�|������δ��|�Ѱ�װ��|��ê��|��ê��|��������";
			CString sXuanChuiWorkEnvItems="δ����|���|����|�������ѱ�|����������|�����¹���|�Ѱ�װ��|��װ������|��ê��|��ê��|��������";
			CStringArray workEnvNameArr;
			workEnvNameArr.SetAtGrow(0,"δ����");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_LOWTEMP,"����");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_ICE,"����");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_WIND,"���");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_ICEDIFF,"�������ѱ�");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_ICELESS,"�ѱ������ٱ�");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_ICEMORE,"�ѱ��������");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_DUANXIANRUNNING,"����������");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_DUANXIANBREAK,"�����¹���");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPSUSPEND,"��װ�Ѱ�װ��");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPSUSPENDING,"��װ������");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH,"�������ѹ�");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY,"������δ��");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_ANCHORFINISHED,"��ê��");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPANCHORING,"��ê��");
			workEnvNameArr.SetAtGrow(WORKENVLOAD_EARTHQUAKE,"��������");

			CListCtrlItemInfo *lpInfo = new CListCtrlItemInfo();
			lpInfo->SetSubItemText(0,pWorkStatus->name,TRUE);
			lpInfo->SetControlType(0,GCT_STATIC);
			lpInfo->SetSubItemText(2,"ɾ������");
			lpInfo->SetControlType(2,GCT_GRAYBUTTON);
			CSuperGridCtrl::CTreeItem* pStatusItem=pTransformWorkStatusDlg->m_listCtrl.InsertItem(pSelItem,lpInfo);
			pStatusItem->m_idProp=2;	//����
			pStatusItem->m_bHideChildren=TRUE;
			long workenv_key=pSelItem->m_lpNodeInfo->m_lParam;
			for(LOADNODEROOM_STRU *pNodeRoom=pHashNodeRoom->GetFirst();pNodeRoom;pNodeRoom=pHashNodeRoom->GetNext())
			{
				pNodeRoom->hashWorkEnv.SetValue(pHashWorkStatus->GetCursorKey(),workenv_key);
				for(WIRENODE_STRU *pLoadNode=pNodeRoom->hashNode.GetFirst();pLoadNode;pLoadNode=pNodeRoom->hashNode.GetNext())
				{
					pLoadNode->hashWorkEnv.SetValue(pHashWorkStatus->GetCursorKey(),workenv_key);
					lpInfo = new CListCtrlItemInfo();
					sText.Format("%d",pNodeRoom->hashNode.GetCursorKey());
					lpInfo->SetSubItemText(0,sText,TRUE);
					lpInfo->SetControlType(0,GCT_STATIC);
					CWireLoadModel *pWireModel=pTransformWorkStatusDlg->m_pWireLayout->hashWireLoadModel.GetValue(pNodeRoom->m_iRawWireLoadNo);
					if(pWireModel==NULL)
						continue;
					if(pWireModel->wireInfo.m_bNaiZhang)	//������
					{
						lpInfo->SetSubItemText(1,workEnvNameArr[workenv_key],FALSE);
						lpInfo->SetControlType(1,GCT_CMB_LIST);
						lpInfo->SetListItemsStr(1, sNaiZhangWorkEnvItems);
					}
					else
					{
						lpInfo->SetSubItemText(1,workEnvNameArr[workenv_key],FALSE);
						lpInfo->SetControlType(1,GCT_CMB_LIST);
						lpInfo->SetListItemsStr(1, sXuanChuiWorkEnvItems);
					}
					pTransformWorkStatusDlg->m_listCtrl.InsertItem(pStatusItem,lpInfo);
				}
			}
		}
		else//ɾ��
		{
			for(RAWWORKSTATUS_STRU *pRawStatus=pHashWorkStatus->GetFirst();pRawStatus;pRawStatus=pHashWorkStatus->GetNext())
			{
				if (0==strcmp(pSelItem->m_lpNodeInfo->GetSubItemText(0),pRawStatus->name))
				{
					pHashWorkStatus->DeleteCursor();
					break;
				}	
			}
			pListCtrl->DeleteItemEx(pSelItem,pSelItem->GetIndex());
		}
		pListCtrl->Redraw();
	}
	return TRUE;
}

BOOL CTransformWorkStatusDlg::OnInitDialog() 
{
	if(m_pTower==NULL)
		return FALSE;
	CDialog::OnInitDialog();
	long col_wide_arr[1]={80};
	m_listCtrl.InsertColumn(0,"��Ŀ");
	m_listCtrl.InitListCtrl(col_wide_arr);
	m_listCtrl.SetModifyValueFunc(FireValueModify);
	m_listCtrl.SetButtonClickFunc(FireButtonClick);
	m_listCtrl.SetContextMenuFunc(FireContextMenu);
	m_listCtrl.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	
	m_ctrlPropGroup.InsertItem(0,"�ҵ�λ");
	m_ctrlPropGroup.InsertItem(1,"����");
	m_ctrlPropGroup.SetCurSel(0);
	//����Ч�ڵ�����ڵ��ŵ�ͬ��
	CHashTable<CLDSNode*>hashNodeH;
	hashNodeH.CreateHashTable(m_pTower->Node.GetNodeNum());
	for(CLDSNode* pNode=m_pTower->Node.GetFirst();pNode;pNode=m_pTower->Node.GetNext())
		hashNodeH.SetValueAt(pNode->handle,pNode);
	for(LOADNODEROOM_STRU *pRoom=m_pWireLayout->groupWorkStatus.hashNodeRoom.GetFirst();pRoom;
	pRoom=m_pWireLayout->groupWorkStatus.hashNodeRoom.GetNext())
	{
		WIRENODE_STRU *pRawNode,*pNewRawNode;
		CHashList<WIRENODE_STRU>hashNewNode;
		for(pRawNode=pRoom->hashNode.GetFirst();pRawNode;pRawNode=pRoom->hashNode.GetNext())
		{
			long key=pRoom->hashNode.GetCursorKey();
			CLDSNode *pNode;
			if(hashNodeH.GetValueAt(pRawNode->hNode,pNode))
				key=pNode->point_i;
			pNewRawNode=hashNewNode.Add(key);
			pNewRawNode->fLoadAllocCoefX=pRawNode->fLoadAllocCoefX;
			pNewRawNode->fLoadAllocCoefY=pRawNode->fLoadAllocCoefY;
			pNewRawNode->fLoadAllocCoefZ=pRawNode->fLoadAllocCoefZ;
			pNewRawNode->hNode=pRawNode->hNode;
			for(long *pWorkEnvKey=pRawNode->hashWorkEnv.GetFirst();pWorkEnvKey;pWorkEnvKey=pRawNode->hashWorkEnv.GetNext())
				pNewRawNode->hashWorkEnv.SetValue(pRawNode->hashWorkEnv.GetCursorKey(),*pWorkEnvKey);
		}
		pRoom->hashNode.Empty();
		for(pNewRawNode=hashNewNode.GetFirst();pNewRawNode;pNewRawNode=hashNewNode.GetNext())
		{
			pRawNode=pRoom->hashNode.Add(hashNewNode.GetCursorKey());
			pRawNode->fLoadAllocCoefX=pNewRawNode->fLoadAllocCoefX;
			pRawNode->fLoadAllocCoefY=pNewRawNode->fLoadAllocCoefY;
			pRawNode->fLoadAllocCoefZ=pNewRawNode->fLoadAllocCoefZ;
			pRawNode->hNode=pNewRawNode->hNode;
			for(long *pWorkEnvKey=pNewRawNode->hashWorkEnv.GetFirst();pWorkEnvKey;pWorkEnvKey=pNewRawNode->hashWorkEnv.GetNext())
				pRawNode->hashWorkEnv.SetValue(pNewRawNode->hashWorkEnv.GetCursorKey(),*pWorkEnvKey);
		}
	}
	m_iTabGroup=0;
	//�ڲ�����
	if(m_bInernalStart)	
		FinishSelectObjOper();	//���ѡ�����ĺ�������
	RefreshListCtrl(m_iTabGroup);
	if(m_bInernalStart)	
	{	//�ָ�ѡ����
		CSuperGridCtrl::CTreeItem *pItem=m_listCtrl.FindItemByPropId(m_idEventProp,NULL);
		m_listCtrl.SetFocus();
		if(pItem)
		{
			int index=m_listCtrl.GetCurIndex(pItem);
			m_listCtrl.SetItemState(index,LVIS_SELECTED,LVIS_SELECTED);	//����ѡ����	
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CTransformWorkStatusDlg::RefreshListCtrl(int iTabGroup)
{
	m_ctrlPropGroup.SetCurSel(iTabGroup);
	CString sText;
	CListCtrlItemInfo* lpInfo;
	CSuperGridCtrl::CTreeItem *pRootItem,*pStatusItem,*pItem;
	// Delete all of the items and columns.
	m_listCtrl.DeleteAllItems();
	m_listCtrl.EmptyColumnHeader();
	CHashList<LOADNODEROOM_STRU>* pHashNodeRoom=&m_pWireLayout->groupWorkStatus.hashNodeRoom;
	CHashList<RAWWORKSTATUS_STRU>* pHashWorkStatus=&m_pWireLayout->groupWorkStatus.hashWorkStatus;
	if(iTabGroup==0)
	{
		m_listCtrl.AddColumnHeader("�ڵ��(λ)",80);
		m_listCtrl.AddColumnHeader("�������ط���",100);
		m_listCtrl.AddColumnHeader("X.coef",60);
		m_listCtrl.AddColumnHeader("Y.coef",60);
		m_listCtrl.AddColumnHeader("Z.coef",60);
		m_listCtrl.AddColumnHeader("����",80);
		m_listCtrl.RefreshListViewHeader();
		CString itemsStr;
		CWireLoadModel *pWireModel;
		int item_serial=1;
		for(pWireModel=m_pWireLayout->hashWireLoadModel.GetFirst();pWireModel;pWireModel=m_pWireLayout->hashWireLoadModel.GetNext())
		{
			if(itemsStr.GetLength()==0)
				itemsStr=pWireModel->name;
			else
			{
				itemsStr+="|";
				itemsStr+=pWireModel->name;
			}
		}

		for(LOADNODEROOM_STRU *pNodeRoom=pHashNodeRoom->GetFirst();pNodeRoom;pNodeRoom=pHashNodeRoom->GetNext())
		{
			lpInfo = new CListCtrlItemInfo();
			sText.Format("%d��λ",pHashNodeRoom->GetCursorKey());
			lpInfo->SetSubItemText(0,sText,TRUE);
			lpInfo->SetControlType(0,GCT_STATIC);
			pWireModel=m_pWireLayout->hashWireLoadModel.GetValue(pNodeRoom->m_iRawWireLoadNo);
			if(pWireModel)
				sText=pWireModel->name;
			else
				sText="";
			lpInfo->SetSubItemText(1,sText,FALSE,"����ģ��");
			lpInfo->SetControlType(1,GCT_CMB_LIST);
			lpInfo->SetListItemsStr(1,itemsStr);
			lpInfo->SetSubItemText(5,"���ӽڵ�","����½ڵ�");
			lpInfo->SetControlType(5,GCT_GRAYBUTTON);
			pRootItem=m_listCtrl.InsertRootItem(lpInfo);
			pRootItem->m_idProp=item_serial++;
			for(WIRENODE_STRU *pLoadNode=pNodeRoom->hashNode.GetFirst();pLoadNode;pLoadNode=pNodeRoom->hashNode.GetNext())
			{
				sText.Format("%d",pNodeRoom->hashNode.GetCursorKey());
				lpInfo = new CListCtrlItemInfo();
				lpInfo->SetSubItemText(0,sText,FALSE);
				lpInfo->SetControlType(0,GCT_BTN_EDIT);//GCT_EDIT);
				sText.Format("%f",pLoadNode->fLoadAllocCoefX);
				SimplifiedNumString(sText);
				lpInfo->SetSubItemText(2,sText,FALSE);
				lpInfo->SetControlType(2,GCT_EDIT);
				lpInfo->SetSubItemHelpText(2,"����X��������ϵ��");
				sText.Format("%f",pLoadNode->fLoadAllocCoefY);
				SimplifiedNumString(sText);
				lpInfo->SetSubItemText(3,sText,FALSE);
				lpInfo->SetControlType(3,GCT_EDIT);
				lpInfo->SetSubItemHelpText(3,"����Y��������ϵ��");
				sText.Format("%f",pLoadNode->fLoadAllocCoefZ);
				SimplifiedNumString(sText);
				lpInfo->SetSubItemText(4,sText,FALSE);
				lpInfo->SetControlType(4,GCT_EDIT);
				lpInfo->SetSubItemHelpText(4,"����Z��������ϵ��");
				lpInfo->SetSubItemText(5,"ɾ��",FALSE,"ɾ����ǰ�ڵ�");
				lpInfo->SetControlType(5,GCT_GRAYBUTTON);
				pItem=m_listCtrl.InsertItem(pRootItem,lpInfo);
				pItem->m_idProp=item_serial++;
			}
		}
		m_listCtrl.Redraw();
	}
	else if(iTabGroup==1)
	{
		CString sNaiZhangWorkEnvItems="δ����|���|����|����|�ѱ������ٱ�|�ѱ��������|����������|�����¹���|�������ѹ�|������δ��|�Ѱ�װ��|��ê��|��ê��|��������";
		CString sXuanChuiWorkEnvItems="δ����|���|����|�������ѱ�|����������|�����¹���|�Ѱ�װ��|��װ������|��ê��|��ê��|��������";
		CStringArray workEnvNameArr;
		workEnvNameArr.SetAtGrow(0,"δ����");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_WIND,"���");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_ICE,"����");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_LOWTEMP,"����");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_ICEDIFF,"�������ѱ�");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_ICELESS,"�ѱ������ٱ�");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_ICEMORE,"�ѱ��������");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_DUANXIANRUNNING,"����������");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_DUANXIANBREAK,"�����¹���");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPSUSPEND,"�Ѱ�װ��");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPSUSPENDING,"��װ������");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH,"�������ѹ�");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY,"������δ��");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_ANCHORFINISHED,"��ê��");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_SETUPANCHORING,"��ê��");
		workEnvNameArr.SetAtGrow(WORKENVLOAD_EARTHQUAKE,"��������");
		long col_wide_arr[5]={200,120,80};
		m_listCtrl.AddColumnHeader("��Ϲ���(/�ڵ��)");
		m_listCtrl.AddColumnHeader("��������");
		m_listCtrl.AddColumnHeader("����");
		m_listCtrl.RefreshListViewHeader(col_wide_arr);

		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,"���¹���",TRUE);
		lpInfo->SetControlType(0,GCT_STATIC);
		lpInfo->SetSubItemText(2,"���ӹ���",FALSE);
		lpInfo->SetControlType(2,GCT_GRAYBUTTON);
		lpInfo->m_lParam=WORKENVLOAD_LOWTEMP;
		CSuperGridCtrl::CTreeItem* pLowestTempItem=m_listCtrl.InsertRootItem(lpInfo);
		pLowestTempItem->m_idProp=1;//��������

		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,"��繤��",TRUE);
		lpInfo->SetControlType(0,GCT_STATIC);
		lpInfo->SetSubItemText(2,"���ӹ���",FALSE);
		lpInfo->SetControlType(2,GCT_GRAYBUTTON);
		lpInfo->m_lParam=WORKENVLOAD_WIND;
		CSuperGridCtrl::CTreeItem* pMaxWindItem=m_listCtrl.InsertRootItem(lpInfo);
		pMaxWindItem->m_idProp=1;//��������

		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,"��������",TRUE);
		lpInfo->SetControlType(0,GCT_STATIC);
		lpInfo->SetSubItemText(2,"���ӹ���",FALSE);
		lpInfo->SetControlType(2,GCT_GRAYBUTTON);
		lpInfo->m_lParam=WORKENVLOAD_ICE;
		CSuperGridCtrl::CTreeItem* pMaxIceItem=m_listCtrl.InsertRootItem(lpInfo);
		pMaxIceItem->m_idProp=1;//��������

		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,"�¹ʹ���",TRUE);
		lpInfo->SetControlType(0,GCT_STATIC);
		lpInfo->SetSubItemText(2,"���ӹ���",FALSE);
		lpInfo->SetControlType(2,GCT_GRAYBUTTON);
		lpInfo->m_lParam=WORKENVLOAD_DUANXIANRUNNING;
		CSuperGridCtrl::CTreeItem* pDuanXianItem=m_listCtrl.InsertRootItem(lpInfo);
		pDuanXianItem->m_idProp=1;//��������

		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,"��װ����",TRUE);
		lpInfo->SetControlType(0,GCT_STATIC);
		lpInfo->SetSubItemText(2,"���ӹ���",FALSE);
		lpInfo->SetControlType(2,GCT_GRAYBUTTON);
		lpInfo->m_lParam=WORKENVLOAD_ANCHORFINISHED;
		CSuperGridCtrl::CTreeItem* pSetupItem=m_listCtrl.InsertRootItem(lpInfo);
		pSetupItem->m_idProp=1;//��������

		lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,"���㹤��",TRUE);
		lpInfo->SetControlType(0,GCT_STATIC);
		lpInfo->SetSubItemText(2,"���ӹ���",FALSE);
		lpInfo->SetControlType(2,GCT_GRAYBUTTON);
		CSuperGridCtrl::CTreeItem* pCheckItem=m_listCtrl.InsertRootItem(lpInfo);
		pCheckItem->m_idProp=1;//��������

		for(RAWWORKSTATUS_STRU *pRawStatus=pHashWorkStatus->GetFirst();pRawStatus;pRawStatus=pHashWorkStatus->GetNext())
		{
			if(pRawStatus->m_cMainLoadType==0)
				pRootItem=pMaxWindItem;
			else if(pRawStatus->m_cMainLoadType==1)
				pRootItem=pMaxIceItem;
			else if(pRawStatus->m_cMainLoadType==2)
				pRootItem=pLowestTempItem;
			else if(pRawStatus->m_cMainLoadType==3)
				pRootItem=pDuanXianItem;
			else if(pRawStatus->m_cMainLoadType==4)
				pRootItem=pSetupItem;
			else
				pRootItem=pCheckItem;
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetSubItemText(0,pRawStatus->name,TRUE);
			lpInfo->SetControlType(0,GCT_STATIC);
			lpInfo->SetSubItemText(2,"ɾ������");
			lpInfo->SetControlType(2,GCT_GRAYBUTTON);
			pStatusItem=m_listCtrl.InsertItem(pRootItem,lpInfo);
			pStatusItem->m_bHideChildren=TRUE;
			pStatusItem->m_idProp=2;	//����
			//
			long nWorkStatusKey=pHashWorkStatus->GetCursorKey();
			for(LOADNODEROOM_STRU *pNodeRoom=pHashNodeRoom->GetFirst();pNodeRoom;pNodeRoom=pHashNodeRoom->GetNext())
			{
				//TODO: ������ڴ�й© wjh-2013-8-13
				CWireLoadModel *pWireModel=m_pWireLayout->hashWireLoadModel.GetValue(pNodeRoom->m_iRawWireLoadNo);
				if(pWireModel==NULL)
					continue;
				for(WIRENODE_STRU *pLoadNode=pNodeRoom->hashNode.GetFirst();pLoadNode;pLoadNode=pNodeRoom->hashNode.GetNext())
				{
					lpInfo = new CListCtrlItemInfo();
					sText.Format("%d",pNodeRoom->hashNode.GetCursorKey());
					lpInfo->SetSubItemText(0,sText,TRUE);
					lpInfo->SetControlType(0,GCT_STATIC);
					long *pWorkEnv=pLoadNode->hashWorkEnv.GetValue(nWorkStatusKey);
					if(pWorkEnv&&*pWorkEnv>=0&&*pWorkEnv<workEnvNameArr.GetSize())
						sText=workEnvNameArr[*pWorkEnv];
					else
						sText="";
					lpInfo->SetSubItemText(1,sText,FALSE);
					lpInfo->SetControlType(1,GCT_CMB_LIST);
					if(pWireModel->wireInfo.m_bNaiZhang)
						lpInfo->SetListItemsStr(1, sNaiZhangWorkEnvItems);
					else
						lpInfo->SetListItemsStr(1, sXuanChuiWorkEnvItems);
					pItem=m_listCtrl.InsertItem(pStatusItem,lpInfo);
					lpInfo->m_lParam=pNodeRoom->m_iRawWireLoadNo;
					pItem->m_idProp=3;	//�ڵ�
				}
			}
		}
		m_listCtrl.Redraw();
	}
}

void CTransformWorkStatusDlg::OnSelchangeTabGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_iTabGroup = m_ctrlPropGroup.GetCurSel();
	RefreshListCtrl(m_iTabGroup);
	*pResult = 0;
}

//��λ����:NODEPLACESET 
//��λ:NODEPLACE 
//������������:WORKENVSET 
//��������:WORKENV 
//��������:WORKSTATUSSET 
//����:WORKSTATUS
BOOL CTransformWorkStatusDlg::ImportXMLToGroupWorkStatus(CMarkup xml,CWorkStatusGroupInfo &groupWorkStatus,CString sElemName,
														 CStringKeyHashTable<long> *pWorkEnvHashtable/*=NULL*/,long *pWorkStatusKey/*=NULL*/)
{	
	if((sElemName.CompareNoCase("WORKENV")==0||sElemName.CompareNoCase("WORKSTATUS")==0)&&pWorkEnvHashtable==NULL)
		return FALSE;
	BOOL bFirst=TRUE;
	while (xml.FindChildElem(sElemName))   
	{ 
		xml.IntoElem();
		if(sElemName.CompareNoCase("NODEPLACESET")==0)		//��λ����
			ImportXMLToGroupWorkStatus(xml,groupWorkStatus,"NODEPLACE",pWorkEnvHashtable);
		else if(sElemName.CompareNoCase("WORKENVSET")==0)	//������������
			ImportXMLToGroupWorkStatus(xml,groupWorkStatus,"WORKENV",pWorkEnvHashtable); 
		else if(sElemName.CompareNoCase("WORKSTATUSSET")==0)//��������
			ImportXMLToGroupWorkStatus(xml,groupWorkStatus,"WORKSTATUS",pWorkEnvHashtable);
		else if(sElemName.CompareNoCase("NODEPLACE")==0)	//��λ
		{	//��ȡ����ӵ�λ��Ϣ
			CString sSerial=_T(""),sRawWireLoad=_T(""),sHelpString=_T("");
			sSerial=xml.GetAttrib("serial");
			sRawWireLoad=xml.GetAttrib("RAWWIRELOAD");
			sHelpString=xml.GetAttrib("helpString");
			//��������
			int keyNodeRoom=atoi(sSerial);
			LOADNODEROOM_STRU *pLoadNodeRoom=groupWorkStatus.hashNodeRoom.Add(keyNodeRoom);
			pLoadNodeRoom->m_iRawWireLoadNo=atoi(sRawWireLoad);
			strcpy(pLoadNodeRoom->helpString,sHelpString);
			//���Ĭ�ϵĽڵ�
			//if(pLoadNodeRoom->hashNode.GetNodeNum()==0)
			WIRENODE_STRU *pRawLoadNode=NULL;
			while (xml.FindChildElem("NODE"))
			{
				xml.IntoElem();
				CString pointI=xml.GetAttrib("serial");
				pRawLoadNode=pLoadNodeRoom->hashNode.Add(atoi(pointI));
				pRawLoadNode->fLoadAllocCoefX=atof(xml.GetAttrib("Xcoef"));
				pRawLoadNode->fLoadAllocCoefY=atof(xml.GetAttrib("Ycoef"));
				pRawLoadNode->fLoadAllocCoefZ=atof(xml.GetAttrib("Zcoef"));
				xml.OutOfElem();
			}
			if(pRawLoadNode==NULL)
				pRawLoadNode=pLoadNodeRoom->hashNode.Add(keyNodeRoom);
		}
		else if(sElemName.CompareNoCase("NODEPLACEREF")==0)	//��λ
		{		//ָ������key
			long nKye=*pWorkStatusKey;	//����key
			CString sID=_T(""),sWorkEnv=_T("");
			sID=xml.GetAttrib("serial");
			sWorkEnv=xml.GetAttrib("workenv");
			long nWorkEnvID=0;
			pWorkEnvHashtable->GetValueAt(sWorkEnv.GetBuffer(1),nWorkEnvID);
			LOADNODEROOM_STRU *pLoadNodeRoom=NULL;
			if(sID.CompareNoCase("*")==0)
			{	//*�������е�λ�Ĺ���������ΪsWorkEnv
				for(pLoadNodeRoom=groupWorkStatus.hashNodeRoom.GetFirst();pLoadNodeRoom;
					pLoadNodeRoom=groupWorkStatus.hashNodeRoom.GetNext())
				{
					pLoadNodeRoom->hashWorkEnv.SetValue(nKye,nWorkEnvID);
					for(WIRENODE_STRU *pLoadNode=pLoadNodeRoom->hashNode.GetFirst();pLoadNode;pLoadNode=pLoadNodeRoom->hashNode.GetNext())
						pLoadNode->hashWorkEnv.SetValue(nKye,nWorkEnvID);
				}
			}
			else
			{	//����sID���ҵ�λ
				long nID=atoi(sID);
				pLoadNodeRoom=groupWorkStatus.hashNodeRoom.GetValue(nID);
				if(pLoadNodeRoom)
				{	//����nKey���Ҷ�Ӧ�Ĺ�������,�������޸ĵ�λ����,�����������һ��
					pLoadNodeRoom->hashWorkEnv.SetValue(nKye,nWorkEnvID);
					for(WIRENODE_STRU *pLoadNode=pLoadNodeRoom->hashNode.GetFirst();pLoadNode;pLoadNode=pLoadNodeRoom->hashNode.GetNext())
						pLoadNode->hashWorkEnv.SetValue(nKye,nWorkEnvID);
				}
			}
		}
		else if(sElemName.CompareNoCase("WORKENV")==0)		//��������
		{
			CString sSerial=_T(""),sName=_T("");
			sName=xml.GetAttrib("name");
			sSerial=xml.GetAttrib("serial");
			pWorkEnvHashtable->SetValueAt(sName.GetBuffer(1),atoi(sSerial));
		}
		else if(sElemName.CompareNoCase("WORKSTATUS")==0)	//����
		{
			CString sID=_T(""),sMainLoadType=_T(""),model_str=_T(""),sName=_T("");
			CString sIceThick,sWindV,sAzimuth,sMinVertSpace,sVibrationMode,sHoriEQAxis;
			sMainLoadType=xml.GetAttrib("MainLoadType");
			model_str=xml.GetAttrib("model_str");
			sName=xml.GetAttrib("name");
			sIceThick=xml.GetAttrib("ice_thick");
			sWindV=xml.GetAttrib("wind_velocity");
			sAzimuth=xml.GetAttrib("azimuth");
			sMinVertSpace=xml.GetAttrib("min_vert_space");
			sVibrationMode=xml.GetAttrib("vibration_mode_str");
			sHoriEQAxis=xml.GetAttrib("hori_eq_axis");
			//��������
			long key=groupWorkStatus.hashWorkStatus.GetNodeNum()+1;	//����key
			RAWWORKSTATUS_STRU *pWorkStatus=groupWorkStatus.hashWorkStatus.Add(key);
			strncpy(pWorkStatus->name,sName,100);
			if(model_str.GetLength()==0)
				strncpy(pWorkStatus->lim_str,this->m_sModelLimStr,50);
			else
				strncpy(pWorkStatus->lim_str,model_str,50);
			pWorkStatus->m_cMainLoadType=atoi(sMainLoadType);
			if(sMinVertSpace.CompareNoCase("true")==0)
				pWorkStatus->m_bMinVertSpace=true;
			else
				pWorkStatus->m_bMinVertSpace=false;
			if(sIceThick.GetLength()==0)
			{
				if(pWorkStatus->m_cMainLoadType==1||pWorkStatus->m_cMainLoadType==3)	//�����򲻾��ȸ���
					pWorkStatus->ice_thick=m_pTower->weather.m_fMaxIceThick;
				else
					pWorkStatus->ice_thick=0;
			}
			else
				pWorkStatus->ice_thick=atof(sIceThick);
			if(sWindV.GetLength()==0)
			{
				if(pWorkStatus->m_cMainLoadType==0)			//���
					pWorkStatus->WindLoad.velocity=m_pTower->weather.m_fMaxWindV;
				else if(pWorkStatus->m_cMainLoadType==1)	//����
					pWorkStatus->WindLoad.velocity=m_pTower->weather.m_fMaxIceWindV;
				else if(pWorkStatus->m_cMainLoadType==4)	//��װ
					pWorkStatus->WindLoad.velocity=m_pTower->weather.m_fSetupWindV;
				else if(pWorkStatus->m_cMainLoadType>=5&&pWorkStatus->m_cMainLoadType<=8)
					pWorkStatus->WindLoad.velocity=m_pTower->weather.m_fMaxWindV;	//��������ʱ����ذ�30%�����أ��޷��ޱ���
				else
					pWorkStatus->WindLoad.velocity=0;
			}
			else
				pWorkStatus->WindLoad.velocity=atof(sWindV);
			pWorkStatus->WindLoad.azimuth=atoi(sAzimuth);
			//��������
			strncpy(pWorkStatus->vibration_mode_str,sVibrationMode,MAX_TEMP_CHAR_50);
			if(sHoriEQAxis.GetLength()>0)
			{
				if(sHoriEQAxis[0]=='X'||sHoriEQAxis[0]=='x')
					pWorkStatus->m_cHoriEarthQuakeAxis=0;
				else if(sHoriEQAxis[0]=='Y'||sHoriEQAxis[0]=='y')
					pWorkStatus->m_cHoriEarthQuakeAxis=90;
				else
					pWorkStatus->m_cHoriEarthQuakeAxis=(BYTE)atoi(sHoriEQAxis);
			}
			ImportXMLToGroupWorkStatus(xml,groupWorkStatus,"NODEPLACEREF",pWorkEnvHashtable,&key);
		}
		else 
			return FALSE;
		xml.OutOfElem();
	}
	return TRUE;
}

BOOL CTransformWorkStatusDlg::ExportGroupWorkStatusToXML(CMarkup &xml,CWorkStatusGroupInfo &groupWorkStatus,
														 CString sElemName,const void *pObj/*=NULL*/)
{
	xml.IntoElem();
	if(sElemName.CompareNoCase("NODEPLACESET")==0)
	{
		xml.AddElem(sElemName);
		LOADNODEROOM_STRU *pLoadNodeRoom=NULL;
		for(pLoadNodeRoom=groupWorkStatus.hashNodeRoom.GetFirst();pLoadNodeRoom;pLoadNodeRoom=groupWorkStatus.hashNodeRoom.GetNext())
			ExportGroupWorkStatusToXML(xml,groupWorkStatus,"NODEPLACE",pLoadNodeRoom);
	}
	else if(sElemName.CompareNoCase("WORKENVSET")==0)
	{
		xml.AddElem(sElemName);
		//ExportGroupWorkStatusToXML(xml,groupWorkStatus,"WORKENV");
		CStringArray str_arr;
		str_arr.Add("���");
		str_arr.Add("����");
		str_arr.Add("����");
		str_arr.Add("�ѱ������ٱ�");
		str_arr.Add("�ѱ��������");
		str_arr.Add("�¹�������");
		str_arr.Add("�¹���");
		str_arr.Add("��װ������δ��");
		str_arr.Add("��װ�������ѹ�");
		str_arr.Add("�Ѱ�װ��");
		str_arr.Add("��ê��");
		str_arr.Add("��ê��");
		str_arr.Add("��������");
		for(int i=0;i<str_arr.GetSize();i++)
		{
			xml.IntoElem();
			xml.AddElem("WORKENV");
			xml.AddAttrib("name",str_arr[i]);
			xml.AddAttrib("serial",i+1);
			xml.OutOfElem();
		}
	}
	else if(sElemName.CompareNoCase("WORKSTATUSSET")==0)
	{	
		xml.AddElem(sElemName);
		RAWWORKSTATUS_STRU *pWorkStatus=NULL;
		for(pWorkStatus=groupWorkStatus.hashWorkStatus.GetFirst();pWorkStatus;pWorkStatus=groupWorkStatus.hashWorkStatus.GetNext())
			ExportGroupWorkStatusToXML(xml,groupWorkStatus,"WORKSTATUS",pWorkStatus);
	}
	else if(sElemName.CompareNoCase("NODEPLACEREF")==0&&pObj)
	{
		xml.AddElem(sElemName);
		LOADNODEROOM_STRU *pLoadNodeRoom=(LOADNODEROOM_STRU*)pObj;
		long nKey=groupWorkStatus.hashNodeRoom.GetKey(pLoadNodeRoom);
		xml.AddAttrib("serial",nKey);
		xml.AddAttrib("RAWWIRELOAD",pLoadNodeRoom->m_iRawWireLoadNo);
		xml.AddAttrib("helpString",pLoadNodeRoom->helpString);
	}
	else if(sElemName.CompareNoCase("WORKSTATUS")==0&&pObj)
	{
		xml.AddElem(sElemName);
		RAWWORKSTATUS_STRU *pWorkStatus=(RAWWORKSTATUS_STRU*)pObj;
		long nWortStatusKey=groupWorkStatus.hashWorkStatus.GetKey(pWorkStatus);
		xml.AddAttrib("id",nWortStatusKey);
		xml.AddAttrib("MainLoadType",(int)pWorkStatus->m_cMainLoadType);
		xml.AddAttrib("model_str",pWorkStatus->lim_str);
		xml.AddAttrib("name",pWorkStatus->name);
		xml.AddAttrib("azimuth",pWorkStatus->WindLoad.azimuth);
		char axis_str[4]="";
		sprintf(axis_str,"%d",pWorkStatus->m_cHoriEarthQuakeAxis);
		xml.AddAttrib("hori_eq_axis",axis_str);
		xml.AddAttrib("vibration_mode",pWorkStatus->vibration_mode_str);
		LOADNODEROOM_STRU *pLoadNodeRoom=NULL;
		for(pLoadNodeRoom=groupWorkStatus.hashNodeRoom.GetFirst();pLoadNodeRoom;pLoadNodeRoom=groupWorkStatus.hashNodeRoom.GetNext())
		{
			xml.IntoElem();
			xml.AddElem("NODEPLACE");
			long nKey=groupWorkStatus.hashNodeRoom.GetKey(pLoadNodeRoom);
			xml.AddAttrib("id",nKey);
			long *pWorkEnv=pLoadNodeRoom->hashWorkEnv.GetValue(nWortStatusKey);
			if(pWorkEnv)
				xml.AddAttrib("workenv",*pWorkEnv);
			else
				xml.AddAttrib("workenv",0);
			xml.OutOfElem();
		}
	}
	xml.OutOfElem();
	return TRUE;
}

BOOL CTransformWorkStatusDlg::GroupWorkStatusXMLSerialize(CString sFileName,BOOL bWrite,CWorkStatusGroupInfo &groupWorkStatus)
{
	UpdateData();
	CMarkup xml;
	if(bWrite)
	{	//����XML�ļ�
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		xml.AddElem("ROOT");
		ExportGroupWorkStatusToXML(xml,groupWorkStatus,"NODEPLACESET");
		ExportGroupWorkStatusToXML(xml,groupWorkStatus,"WORKENVSET");
		ExportGroupWorkStatusToXML(xml,groupWorkStatus,"WORKSTATUSSET");
		xml.Save(sFileName);
	}
	else
	{	//����XML�ļ�
		xml.SetDocFlags(16);//MDF_READFILE);
		if(!xml.Load(sFileName))
		{	//����XML�ļ�
			AfxMessageBox("��ȡ�ļ�ʧ��!");
			return FALSE;
		}
		xml.ResetMainPos();
		if(!xml.FindElem("ROOT"))
			return FALSE;
		CStringKeyHashTable<long> workEnvHashtable;
		workEnvHashtable.CreateHashTable(200);
		groupWorkStatus.hashNodeRoom.Empty();
		groupWorkStatus.hashWorkStatus.Empty();
		ImportXMLToGroupWorkStatus(xml,groupWorkStatus,"NODEPLACESET",NULL);
		ImportXMLToGroupWorkStatus(xml,groupWorkStatus,"WORKENVSET",&workEnvHashtable);
		ImportXMLToGroupWorkStatus(xml,groupWorkStatus,"WORKSTATUSSET",&workEnvHashtable);
	}
	return TRUE;
}

void CTransformWorkStatusDlg::OnBnImportTemplateXml() 
{
	CString sFileName;
	char *sName="��Ϲ���";
	char *sFileType="��Ϲ���(*.xml)|*.xml||";
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()!=IDOK)
		return;
	//������Ϲ�����Ϣ
	GroupWorkStatusXMLSerialize(dlg.GetPathName(),FALSE,m_pWireLayout->groupWorkStatus);
	//����Ϲ�����Ϣ��ʾ���б����
	RefreshListCtrl(0);
	
	//����ǰ��Ϲ�����Ϣ���뵽XML�ļ���
	//GroupWorkStatusXMLSerialize("D:\\Test.xml",TRUE,m_pWireLayout->groupWorkStatus);
}

void CTransformWorkStatusDlg::OnOK() 
{
	UpdateData();
	for(RAWWORKSTATUS_STRU *pRawStatus=m_pWireLayout->groupWorkStatus.hashWorkStatus.GetFirst();pRawStatus;pRawStatus=m_pWireLayout->groupWorkStatus.hashWorkStatus.GetNext())
	{
		if(strlen(pRawStatus->lim_str)==0)
		{
			if(m_sModelLimStr.GetLength()==0)
				strcpy(pRawStatus->lim_str,"1");
			else
				strncpy(pRawStatus->lim_str,m_sModelLimStr,50);
		}
	}
	CDialog::OnOK();
}

void CTransformWorkStatusDlg::OnEditItemPropItem()
{
	CAddNewWorkStatus newDlg;
	
	CHashList<RAWWORKSTATUS_STRU>* pHashWorkStatus=&m_pWireLayout->groupWorkStatus.hashWorkStatus;
	for(RAWWORKSTATUS_STRU *pWorkStatus = pHashWorkStatus->GetFirst();pWorkStatus;pWorkStatus = pHashWorkStatus->GetNext())
	{
		CSuperGridCtrl::CTreeItem *pItem = m_listCtrl.GetTreeItem(m_listCtrl.GetSelectedItem());
		CString sText = pItem->m_lpNodeInfo->GetSubItemText(0);
		if(strcmp(sText,pWorkStatus->name)==0)
		{
			newDlg.m_sModelLimStr.Format("%s",pWorkStatus->lim_str);
			newDlg.m_sVibrationMode=pWorkStatus->vibration_mode_str;
			newDlg.m_fIceThick = pWorkStatus->ice_thick;
			newDlg.m_sWorkStatus.Format("%s",pWorkStatus->name);
			newDlg.m_fWindAngle = pWorkStatus->WindLoad.azimuth;
			newDlg.m_fMaxWind = pWorkStatus->WindLoad.velocity;
			newDlg.m_bMinVertSpace=pWorkStatus->m_bMinVertSpace;
			newDlg.m_cHoriEqAxisAzimuth=pWorkStatus->m_cHoriEarthQuakeAxis;
			if(IDOK != newDlg.DoModal())
				return;
			strncpy(pWorkStatus->lim_str,newDlg.m_sModelLimStr,50);
			pWorkStatus->ice_thick = newDlg.m_fIceThick;
			strncpy(pWorkStatus->name,newDlg.m_sWorkStatus,100);
			pWorkStatus->m_bMinVertSpace=(newDlg.m_bMinVertSpace!=FALSE);
			pWorkStatus->WindLoad.azimuth = newDlg.m_fWindAngle;
			pWorkStatus->WindLoad.velocity = newDlg.m_fMaxWind;
			pWorkStatus->m_cHoriEarthQuakeAxis=newDlg.m_cHoriEqAxisAzimuth;
			strncpy(pWorkStatus->vibration_mode_str,newDlg.m_sVibrationMode,50);
			break;
		}
	}
}
//���ѡ�����ĺ�������
void CTransformWorkStatusDlg::FinishSelectObjOper()
{
	if(!m_bInernalStart)
		return;
	else //if(GetEventPropId()==GetPropID("des_para.hNode"))
	{
		CSuperGridCtrl::CTreeItem* pItem,*pParentItem=NULL;
		pItem=m_listCtrl.FindItemByPropId(GetEventPropId(),&pParentItem);
		if(pItem==NULL||pParentItem==NULL)
			return;
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode==NULL)
			return;
		CHashList<LOADNODEROOM_STRU>* pHashNodeRoom=&m_pWireLayout->groupWorkStatus.hashNodeRoom;
		for(LOADNODEROOM_STRU *pNodeRoom=pHashNodeRoom->GetFirst();pNodeRoom;pNodeRoom=pHashNodeRoom->GetNext())
		{
			CXhChar50 sText("%d��λ",pHashNodeRoom->GetCursorKey());
			if(pParentItem->m_lpNodeInfo->GetSubItemText(0).CompareNoCase(sText)==0)
				break;
		}
		if(pNodeRoom==NULL)
			return;
		
		long oldkey = atoi(pItem->m_lpNodeInfo->GetSubItemText(0)); 
		WIRENODE_STRU* pNodeStru = pNodeRoom->hashNode.ModifyKeyId(oldkey,pNode->point_i);
		if(pNodeStru)
			pNodeStru->hNode=pNode->handle;
		else
			AfxMessageBox("�ڵ��滻ʧ��!");
	}
}

#endif

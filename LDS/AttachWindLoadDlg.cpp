// AttachWindLoadDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "AttachWindLoadDlg.h"
#include "TowerTreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttachWindLoadDlg dialog

#if defined(__LDS_)||defined(__TSA_)
CAttachWindLoadDlg::CAttachWindLoadDlg(CWnd* pParent /*=NULL*/)
: CCallBackDialog(CAttachWindLoadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAttachWindLoadDlg)
#ifdef AFX_TARG_ENU_ENGLISH
	m_sAttachWindLoadName = _T("Attached Load");
#else 
	m_sAttachWindLoadName = _T("附加荷载");
#endif
	m_fShieldArea = 0.0;
	m_fShieldCoef = 1.0;
	m_fProfileCoef = 1.0;
	m_fAttachWeight = 0.0;
	m_fBetaZ = 1.0;
	m_hNode = _T("");
	m_nPointI = 0;
	m_fBetaZForFoundation = 1.0;
	//}}AFX_DATA_INIT
	m_bWorkStatusMode=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("Handle");
	m_arrHeaderLabel.Add("Label Code");
#else 
	m_arrHeaderLabel.Add("句柄");
	m_arrHeaderLabel.Add("编号");
#endif
}


void CAttachWindLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttachWindLoadDlg)
	DDX_Text(pDX, IDC_E_ATTACH_WIND_LOAD_NAME, m_sAttachWindLoadName);
	DDV_MaxChars(pDX, m_sAttachWindLoadName, 50);
	DDX_Text(pDX, IDC_E_SHIELD_WIND_AREA, m_fShieldArea);
	DDX_Text(pDX, IDC_E_SHIELD_WIND_COEF, m_fShieldCoef);
	DDX_Text(pDX, IDC_E_PROFILE_WIND_COEF, m_fProfileCoef);
	DDX_Text(pDX, IDC_E_ATTACH_MASS_WEIGHT, m_fAttachWeight);
	DDX_Text(pDX, IDC_E_WIND_BETA_Z, m_fBetaZ);
	DDX_Text(pDX, IDC_E_NODE_H, m_hNode);
	DDX_Text(pDX, IDC_E_NODE_POINT_I, m_nPointI);
	DDX_Text(pDX, IDC_E_WIND_BETA_Z_FOR_FOUNDATION, m_fBetaZForFoundation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttachWindLoadDlg, CDialog)
	//{{AFX_MSG_MAP(CAttachWindLoadDlg)
	ON_BN_CLICKED(IDC_BN_ADD_LOAD_NODE, OnBnAddLoadNode)
	ON_BN_CLICKED(IDC_BN_MODIFY_LOAD_NODE, OnBnModifyLoadNode)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_NODE_LIST, OnKeydownListCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_NODE_LIST, OnDblclkListCtrl)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BN_ADD_ITEM_RECORD, OnBnClickedBnAddItemRecord)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttachWindLoadDlg message handlers

BOOL CAttachWindLoadDlg::OnInitDialog() 
{
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_NODE_LIST);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	CStringArray str_arr;
	str_arr.SetSize(2);
	if(node_arr.GetSize()==0)
		GetDlgItem(IDC_BN_DEL_LOAD_NODE)->EnableWindow(FALSE);
	else
	{
		for(int i=0;i<node_arr.GetSize();i++)
		{
			str_arr[0].Format("0X%X",node_arr[i].h);
			str_arr[1].Format("%d",node_arr[i].point_i);
			AppendRecord(-1,str_arr);
		}
	}
	if(m_bInernalStart&&m_hPickObj>0)
	{
		CLDSNode *pNode=console.FromNodeHandle(m_hPickObj);
		if(pNode)
		{
			if(!CheckNodeValid(m_hPickObj,pNode->point_i))
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The load node has added!");
#else 
				AfxMessageBox("已添加此荷载节点!");
#endif
			else 
			{
				node_arr.Add(NODE_POINTI(pNode->handle,pNode->point_i));
				str_arr[0].Format("0X%X",pNode->handle);
				str_arr[1].Format("%d",pNode->point_i);
				AppendRecord(-1,str_arr);
			}
		}
	}
	CDialog::OnInitDialog();
	GetDlgItem(IDC_E_ATTACH_WIND_LOAD_NAME)->EnableWindow(!m_bWorkStatusMode);
	GetDlgItem(IDC_E_SHIELD_WIND_AREA)->EnableWindow(!m_bWorkStatusMode);
	GetDlgItem(IDC_E_SHIELD_WIND_COEF)->EnableWindow(m_bWorkStatusMode);
	GetDlgItem(IDC_E_WIND_BETA_Z)->EnableWindow(!m_bWorkStatusMode);
	GetDlgItem(IDC_E_WIND_BETA_Z_FOR_FOUNDATION)->EnableWindow(!m_bWorkStatusMode);
	GetDlgItem(IDC_E_PROFILE_WIND_COEF)->EnableWindow(!m_bWorkStatusMode);
	GetDlgItem(IDC_E_ATTACH_MASS_WEIGHT)->EnableWindow(!m_bWorkStatusMode);
	GetDlgItem(IDC_BN_ADD_LOAD_NODE)->EnableWindow(!m_bWorkStatusMode);
	GetDlgItem(IDC_BN_MODIFY_LOAD_NODE)->EnableWindow(!m_bWorkStatusMode);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CAttachWindLoadDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	CHeaderCtrl *pHeader = m_pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();

	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	{
	   m_pListCtrl->DeleteColumn(0);
	}
	if(m_arrHeaderLabel.GetSize()>0)
		NUM_COLUMNS = m_arrHeaderLabel.GetSize();
	else
		NUM_COLUMNS = 0;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeaderLabel[i].GetBuffer(1);//_gszColumnLabel[i];
		lvc.cx = 80;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
void CAttachWindLoadDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
// insert items

	LV_ITEM lvi;
	if(iItem==-1)
		iItem=m_pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = m_pListCtrl->InsertItem(&lvi);
// set item text for additional columns
	int nCount = m_arrHeaderLabel.GetSize();
	nCount = __min(nCount,str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		m_pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	m_pListCtrl->RedrawItems(iItem,iItem);
}
void CAttachWindLoadDlg::OnBnAddLoadNode() 
{
	UpdateData();
	CStringArray str_arr;
	str_arr.SetSize(2);
	long h=0;
	sscanf(m_hNode,"%X",&h);
	CLDSNode *pNode=console.FromNodeHandle(h);
	if(pNode==NULL)
	{
		Ta.Node.push_stack();
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pNode->point_i==m_nPointI)
			{
				h=pNode->handle;
				break;
			}
		}
		Ta.Node.pop_stack();
	}
	else
		m_nPointI=pNode->point_i;
	str_arr[0].Format("0X%X",h);
	str_arr[1].Format("%d",m_nPointI);
#ifdef AFX_TARG_ENU_ENGLISH
	if(pNode==NULL)
		AfxMessageBox("The content of node number is invalid !");
	else if(CheckNodeValid(h,m_nPointI))
		AppendRecord(-1,str_arr);
	else
		AfxMessageBox("The load node has added!");
#else 
	if(pNode==NULL)
		AfxMessageBox("节点号内容无效!");
	else if(CheckNodeValid(h,m_nPointI))
		AppendRecord(-1,str_arr);
	else
		AfxMessageBox("已添加此荷载节点!");
#endif
}

void CAttachWindLoadDlg::DeleteItem(int nCurSel) 
{
	m_pListCtrl->DeleteItem(nCurSel);
}

bool CAttachWindLoadDlg::CheckNodeValid(long h, int point_i)
{
	NODE_POINTI item_prev;
	char tem_str[50];
	for(int i=0;i<m_pListCtrl->GetItemCount();i++)
	{
		m_pListCtrl->GetItemText(i,0,tem_str,50);
		sscanf(tem_str,"%X",&item_prev.h);
		m_pListCtrl->GetItemText(i,1,tem_str,50);
		sscanf(tem_str,"%d",&item_prev.point_i);
		if(item_prev.h==h||item_prev.point_i==point_i)
			return false;
	}
	return true;
}

void CAttachWindLoadDlg::OnBnModifyLoadNode() 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		NODE_POINTI item;
		item.h=item.point_i=0;
		sscanf(m_hNode,"%X",&item.h);
		item.point_i=m_nPointI;
#ifdef AFX_TARG_ENU_ENGLISH
		if(item.h==0&&item.point_i==0)
			AfxMessageBox("The content of node number is invalid !");
		else if(!CheckNodeValid(item.h,item.point_i))
			AfxMessageBox("The load node has added!");
#else 
		if(item.h==0&&item.point_i==0)
			AfxMessageBox("节点号内容无效!");
		else if(!CheckNodeValid(item.h,item.point_i))
			AfxMessageBox("已添加此荷载节点!");
#endif
		else
		{
			CString sText;
			CLDSNode *pNode=console.FromNodeHandle(item.h);
			if(pNode==NULL)
			{
				Ta.Node.push_stack();
				for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
				{
					if(pNode->point_i==item.point_i)
					{
						item.h=pNode->handle;
						break;
					}
				}
				Ta.Node.pop_stack();
			}
			else
				item.point_i=pNode->point_i;
			if(pNode)
			{
				sText.Format("0X%X",item.h);
				m_pListCtrl->SetItemText(iCurSel,0,sText);
				sText.Format("%d",item.point_i);
				m_pListCtrl->SetItemText(iCurSel,1,sText);
				m_pListCtrl->RedrawItems(iCurSel,iCurSel);
			}
		}
	}
}

void CAttachWindLoadDlg::OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_DELETE)
		{
			if(iCurSel>=0)
				DeleteItem(iCurSel);
		}
	}
	*pResult = 0;
}

void CAttachWindLoadDlg::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
		{
			char tem_str[50];
			m_pListCtrl->GetItemText(iCurSel,0,tem_str,50);
			m_hNode=tem_str;
			m_pListCtrl->GetItemText(iCurSel,1,tem_str,50);
			sscanf(tem_str,"%d",&m_nPointI);
			UpdateData(FALSE);
		}
	}
	
	*pResult = 0;
}

void CAttachWindLoadDlg::OnOK() 
{
	CDialog::OnOK();
	int nCount=m_pListCtrl->GetItemCount();
	char tem_str[50]="";
	node_arr.RemoveAll();
	for(int i=0;i<nCount;i++)
	{
		NODE_POINTI item;
		m_pListCtrl->GetItemText(i,0,tem_str,50);
		sscanf(tem_str,"%X",&item.h);
		m_pListCtrl->GetItemText(i,1,tem_str,50);
		sscanf(tem_str,"%d",&item.point_i);
		node_arr.Add(item);
	}
}

void CAttachWindLoadDlg::OnBnClickedBnAddItemRecord()
{
	UpdateData();
	m_bInernalStart=TRUE;
	m_iPickObjType=CLS_NODE;	//新添加节点
	m_bPauseBreakExit = TRUE;
	m_dwSelectPartsType = GetSingleWord(SELECTINDEX_NODE);
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCmdPickPrompt = "Please select the node which will be added and press enter key to ensure it";
#else 
	m_sCmdPickPrompt = "请选择需要添加的节点，<回车键>确认";
#endif
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CSelAttachWindLoadDlg dialog


CSelAttachWindLoadDlg::CSelAttachWindLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelAttachWindLoadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelAttachWindLoadDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelAttachWindLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelAttachWindLoadDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelAttachWindLoadDlg, CDialog)
	//{{AFX_MSG_MAP(CSelAttachWindLoadDlg)
	ON_BN_CLICKED(IDC_BN_ADD_ATTACH_WINDLOAD, OnBnAddAttachWindload)
	ON_BN_CLICKED(IDC_BN_DEL_ATTACH_WINDLOAD, OnBnDelAttachWindload)
	ON_BN_CLICKED(ID_BN_VIEW_PROP, OnBnViewProp)
	ON_BN_CLICKED(ID_BN_DELETE_ITEM, OnBnDeleteItem)
	ON_LBN_DBLCLK(IDC_LIST_REMAIN_WINDLOAD, OnBnAddAttachWindload)
	ON_LBN_DBLCLK(IDC_LIST_HAVE_WINDLOAD, OnBnDelAttachWindload)
	ON_BN_CLICKED(IDC_BN_ADD_ATTACH_WINDLOAD_BYCOPY, OnBnAddAttachWindloadBycopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelAttachWindLoadDlg message handlers

BOOL CSelAttachWindLoadDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CListBox *pListBoxRight=(CListBox*)GetDlgItem(IDC_LIST_HAVE_WINDLOAD);
	CListBox *pListBoxLeft=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_WINDLOAD);
	int iSel;
	CString sName;
	for(CAttachWindLoad *pWindLoad=Ta.AttachWindLoad.GetFirst();pWindLoad;pWindLoad=Ta.AttachWindLoad.GetNext())
	{
		sName.Format("%s(%d)",pWindLoad->name,pWindLoad->iNo);
		if(hashIncWindLoads.GetValue(pWindLoad->handle))
		{
			iSel=pListBoxRight->AddString(sName);
			pListBoxRight->SetItemData(iSel,pWindLoad->handle);
		}
		else
		{
			iSel=pListBoxLeft->AddString(sName);
			pListBoxLeft->SetItemData(iSel,pWindLoad->handle);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelAttachWindLoadDlg::OnBnAddAttachWindload() 
{
	CListBox *pListBoxLeft=(CListBox*)GetDlgItem(IDC_LIST_HAVE_WINDLOAD);
	CListBox *pListBoxRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_WINDLOAD);
	int nSel=pListBoxRight->GetSelCount();
	if(nSel<=0)
		return;
	int *rgIndex=new int[nSel];
	pListBoxRight->GetSelItems(nSel,rgIndex);
	for(int i=0;i<nSel;i++)
	{
		long h=(long)pListBoxRight->GetItemData(rgIndex[i]-i);
		for(CAttachWindLoad *pWindLoad=Ta.AttachWindLoad.GetFirst();pWindLoad;pWindLoad=Ta.AttachWindLoad.GetNext())
		{
			if(pWindLoad->handle==h)
			{
				CString sName;
				pListBoxRight->GetText(rgIndex[i]-i,sName);
				pListBoxRight->DeleteString(rgIndex[i]-i);
				int iSel=pListBoxLeft->AddString(sName);
				pListBoxLeft->SetItemData(iSel,h);
				hashIncWindLoads.SetValue(h,h);
				break;
			}
		}
	}
	delete []rgIndex;
}

void CSelAttachWindLoadDlg::OnBnDelAttachWindload() 
{
	CListBox *pListBoxLeft=(CListBox*)GetDlgItem(IDC_LIST_HAVE_WINDLOAD);
	CListBox *pListBoxRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_WINDLOAD);
	int nSel=pListBoxLeft->GetSelCount();
	if(nSel<=0)
		return;
	int *rgIndex=new int[nSel];
	pListBoxLeft->GetSelItems(nSel,rgIndex);
	for(int i=0;i<nSel;i++)
	{
		long h=(long)pListBoxLeft->GetItemData(rgIndex[i]-i);
		CString sName;
		pListBoxLeft->GetText(rgIndex[i]-i,sName);
		pListBoxLeft->DeleteString(rgIndex[i]-i);
		int iSel=pListBoxRight->AddString(sName);
		pListBoxRight->SetItemData(iSel,h);
		hashIncWindLoads.DeleteNode(h);
	}
}

void CSelAttachWindLoadDlg::OnBnViewProp() 
{
	CListBox *pListBoxRight=(CListBox*)GetDlgItem(IDC_LIST_HAVE_WINDLOAD);
	int iSel=pListBoxRight->GetCurSel();
	long h=(long)pListBoxRight->GetItemData(iSel);
	CAttachWindLoad *pWindLoad=Ta.AttachWindLoad.FromHandle(h);
	if(pWindLoad)
	{
		CAttachWindLoadDlg wind_dlg;
		wind_dlg.m_sAttachWindLoadName=pWindLoad->name;
		wind_dlg.m_fShieldArea=pWindLoad->m_fShieldArea;
		wind_dlg.m_fShieldCoef=pWindLoad->m_fShieldCoef;
		wind_dlg.m_fProfileCoef=pWindLoad->m_fProfileCoef;
		wind_dlg.m_fAttachWeight=pWindLoad->m_fAttachWeight;
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
			sprintf(pWindLoad->name,"%s",wind_dlg.m_sAttachWindLoadName);
			CString sName;
			sName.Format("%s(%d)",pWindLoad->name,pWindLoad->iNo);
			pListBoxRight->DeleteString(iSel);
			iSel=pListBoxRight->InsertString(iSel,sName);
			pListBoxRight->SetItemData(iSel,pWindLoad->handle);
			pWindLoad->m_fShieldArea=wind_dlg.m_fShieldArea;
			pWindLoad->m_fShieldCoef=wind_dlg.m_fShieldCoef;
			pWindLoad->m_fProfileCoef=wind_dlg.m_fProfileCoef;
			pWindLoad->m_fAttachWeight=wind_dlg.m_fAttachWeight;
			pWindLoad->GetNodeList()->Empty();
			for(long i=0;i<wind_dlg.node_arr.GetSize();i++)
				pWindLoad->GetNodeList()->append(wind_dlg.node_arr[i].h);
		}
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't find the item's attached load！");
#else 
		AfxMessageBox("找不到此项附加荷载！");
#endif
}

void CSelAttachWindLoadDlg::OnBnDeleteItem() 
{
	CListBox *pListBoxRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_WINDLOAD);
	int nSel=pListBoxRight->GetSelCount();
	int *rgIndex=new int[nSel];
	pListBoxRight->GetSelItems(nSel,rgIndex);
	CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_ATTACH_WINDLOAD);
	for(int i=0;i<nSel;i++)
	{
		long h=(long)pListBoxRight->GetItemData(rgIndex[i]-i);
		CAttachWindLoad *pWindLoad=Ta.AttachWindLoad.FromHandle(h);
		if(pWindLoad)
			pNoGroup->SetNoState(pWindLoad->iNo,FALSE);
		Ta.AttachWindLoad.DeleteNode(h);
		pListBoxRight->DeleteString(rgIndex[i]-i);
		for(CWorkStatus *pWorkStatus=Ta.WorkStatus.GetFirst();pWorkStatus;pWorkStatus=Ta.WorkStatus.GetNext())
		{
			for(void* pShieldCoef=pWorkStatus->AttachWindLoad.GetFirst();pShieldCoef;
					pShieldCoef=pWorkStatus->AttachWindLoad.GetNext())
			{
				if(pWorkStatus->AttachWindLoad.GetCursorKey()==h)
				{
					pWorkStatus->AttachWindLoad.DeleteCursor();
					break;
				}
			}
		}
	}
	delete []rgIndex;
}

void CSelAttachWindLoadDlg::OnBnAddAttachWindloadBycopy() 
{
	CListBox *pListBoxLeft=(CListBox*)GetDlgItem(IDC_LIST_HAVE_WINDLOAD);
	CListBox *pListBoxRight=(CListBox*)GetDlgItem(IDC_LIST_REMAIN_WINDLOAD);
	int nSel=pListBoxRight->GetSelCount();
	if(nSel<=0)
		return;
	int *rgIndex=new int[nSel];
	pListBoxRight->GetSelItems(nSel,rgIndex);
	CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(WORKSTATUS_ATTACH_WINDLOAD);
	for(int i=0;i<nSel;i++)
	{
		long h=(long)pListBoxRight->GetItemData(rgIndex[i]);
		for(CAttachWindLoad *pWindLoad=Ta.AttachWindLoad.GetFirst();pWindLoad;pWindLoad=Ta.AttachWindLoad.GetNext())
		{
			if(pWindLoad->handle==h)
			{
				CAttachWindLoad *pNewLoad=Ta.AttachWindLoad.append();
				pNewLoad->iNo=pNoGroup->EnumIdleNo();
				pNoGroup->SetNoState(pNewLoad->iNo);
				pNewLoad->m_fAttachWeight=pWindLoad->m_fAttachWeight;
				pNewLoad->m_fProfileCoef=pWindLoad->m_fProfileCoef;
				pNewLoad->m_fShieldArea=pWindLoad->m_fShieldArea;
				pNewLoad->m_fShieldCoef=pWindLoad->m_fShieldCoef;
				pNewLoad->m_fBetaZ=pWindLoad->m_fBetaZ;
				pNewLoad->m_fBetaZForFoundation=pWindLoad->m_fBetaZForFoundation;
				strcpy(pNewLoad->name,pWindLoad->name);
				for(long *pH=pWindLoad->GetNodeList()->GetFirst();pH;pH=pWindLoad->GetNodeList()->GetNext())
					pNewLoad->GetNodeList()->append(*pH);
				CString sName;
				sName.Format("%s(%d)",pNewLoad->name,pNewLoad->iNo);
				int iSel=pListBoxLeft->AddString(sName);
				pListBoxLeft->SetItemData(iSel,pNewLoad->handle);
				hashIncWindLoads.SetValue(pNewLoad->handle,pNewLoad->handle);
				break;
			}
		}
	}
	delete []rgIndex;
}

#endif


//<LOCALE_TRANSLATE/>
// StatStdPartGroupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tower.h"
#include "StatStdPartGroupDlg.h"
#include "GlobalFunc.h"
#include "DrawTaMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatStdPartGroupDlg dialog

#ifndef __TSA_FILE_
CStatStdPartGroupDlg::CStatStdPartGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatStdPartGroupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatStdPartGroupDlg)
	m_sSegStr = _T("*");
	m_iStartPartType = 0;
	m_iEndPartType = 0;
	m_bStatAllType = FALSE;
	//}}AFX_DATA_INIT
	m_pCurSelItem = NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("part No.");
	m_arrHeaderLabel.Add("Spec");
	m_arrHeaderLabel.Add("seg num");
	m_arrHeaderLabel.Add("amount");
#else
	m_arrHeaderLabel.Add("构件编号");
	m_arrHeaderLabel.Add("规格");
	m_arrHeaderLabel.Add("段号");
	m_arrHeaderLabel.Add("件数");
#endif
	listViewComparer.SetMaxColumnCount(4);
}


void CStatStdPartGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatStdPartGroupDlg)
	DDX_Text(pDX, IDC_E_SEG_STR, m_sSegStr);
	DDX_CBIndex(pDX, IDC_CMB_START_PART_TYPE, m_iStartPartType);
	DDX_CBIndex(pDX, IDC_CMB_END_PART_TYPE, m_iEndPartType);
	DDX_Check(pDX, IDC_CHK_STAT_ALL_TYPE, m_bStatAllType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatStdPartGroupDlg, CDialog)
	//{{AFX_MSG_MAP(CStatStdPartGroupDlg)
	ON_BN_CLICKED(IDC_BN_REBUILD_PARTSET, OnRebuildPartset)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CTRL, OnColumnclickListCtrl)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CTRL, OnKeydownListCtrl)
	ON_CBN_SELCHANGE(IDC_CMB_START_PART_TYPE, OnSelChangeStartPartType)
	ON_CBN_SELCHANGE(IDC_CMB_END_PART_TYPE, OnSelChangeEndPartType)
	ON_BN_CLICKED(IDC_CHK_STAT_ALL_TYPE, OnStatAllType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatStdPartGroupDlg message handlers

BOOL CStatStdPartGroupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	m_pListCtrl->DeleteAllItems();
	partList.Empty();
	RefreshListView();
	return TRUE;
}
void CStatStdPartGroupDlg::RefreshListViewHeader()
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
#ifdef AFX_TARG_ENU_ENGLISH
		if(i==0) 
			lvc.cx=75;
		else if(i==1)
			lvc.cx=110;
		else if(i==2)
			lvc.cx=80;
		else
			lvc.cx =80;
#else 
		if(i==0) 
			lvc.cx=70;
		else if(i==1)
			lvc.cx=110;
		else if(i==2)
			lvc.cx=40;
		else
			lvc.cx = 40;
#endif
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}

int CStatStdPartGroupDlg::AppendRecord(int iItem, CStringArray &str_arr)
{
	//insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=m_pListCtrl->GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = m_pListCtrl->InsertItem(&lvi);
	//set item text for additional columns
	int nCount = m_arrHeaderLabel.GetSize();
	nCount = __min(nCount,str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		m_pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	m_pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

void CStatStdPartGroupDlg::OnRebuildPartset() 
{
	UpdateData();
	RefreshListView();
}

//判断当前参数化板是否与指定的类型匹配
static BOOL PlateIsSpecType(CLDSParamPlate *pPlate,int iType)
{
	if(pPlate==NULL)
		return FALSE;
	if( (pPlate->IsInsertPlate()&&iType==0)||
		(pPlate->m_iParamType==TYPE_ROLLEND&&iType==1)||
		(pPlate->m_iParamType==TYPE_UEND&&iType==2)||
		(pPlate->m_iParamType==TYPE_XEND&&iType==3)||
		(pPlate->IsFL()&&iType==4)||
		(pPlate->m_iParamType==TYPE_FL&&iType==5)||
		(pPlate->m_iParamType==TYPE_FLP&&iType==6)||
		(pPlate->m_iParamType==TYPE_FLD&&iType==7)||
		(pPlate->m_iParamType==TYPE_FLG&&iType==8)||
		(pPlate->m_iParamType==TYPE_FLR&&iType==9))
		return TRUE;
	return FALSE;
}

void CStatStdPartGroupDlg::RefreshListView()
{
	UpdateData();
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(m_sSegStr,segTbl);
	CStringArray str_arr;
	str_arr.SetSize(4);
	CStringKeyHashTable<DWORD> hashTable;
	hashTable.CreateHashTable(Ta.PartGroup.GetNodeNum()/2);
	m_pListCtrl->DeleteAllItems();
	partList.Empty();
	CLDSPartGroup *pPartGroup=NULL;
	CLDSLineTube *pLineTube=NULL;
	CLDSParamPlate *pStartPlate=NULL,*pEndPlate=NULL;
	for(pPartGroup=Ta.PartGroup.GetFirst();pPartGroup;pPartGroup=Ta.PartGroup.GetNext())
	{
		//if(!pPartGroup->m_bStdPartGroup)
		if(pPartGroup->m_iPartGroupType!=CLDSPartGroup::GROUP_ENDCONN)
			continue;	//非标准组焊件
		pLineTube=(CLDSLineTube*)Ta.FromPartHandle(pPartGroup->handle,CLS_LINETUBE);
		if(pLineTube==NULL)
			continue;
		if(pLineTube->GetClassTypeId()!=CLS_LINETUBE)
			continue;
		if(pLineTube->m_bVirtualPart)
			continue;
		if(strlen(pLineTube->GetPartNo())==0)
			continue;	//未编号构件不引入构件库
		else if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pLineTube->iSeg.iSeg)==NULL)
			continue;	//分段号不在当前所选范围内
		pStartPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		pEndPlate=(CLDSParamPlate*)Ta.FromPartHandle(pLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pStartPlate==NULL||pEndPlate==NULL)
			continue;	//始端或终端没有连接参数板
		if((PlateIsSpecType(pStartPlate,m_iStartPartType)&&PlateIsSpecType(pEndPlate,m_iEndPartType))||	//始--始 终--终
			(PlateIsSpecType(pStartPlate,m_iEndPartType)&&PlateIsSpecType(pEndPlate,m_iStartPartType))||//始--终 终-始
			m_bStatAllType) //统计所有类型的组焊件
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->pPart=pLineTube;
			pItem->num=pPartGroup->m_uStatMatCoef;
			char spec[200]="";
			str_arr[0]=pLineTube->GetPartNo();
			pLineTube->GetSpecification(spec,TRUE);
			str_arr[1]=spec;
			str_arr[2].Format("%s",(char*)pLineTube->iSeg.ToString());
			str_arr[3].Format("%d",pPartGroup->m_uStatMatCoef);
			int iCur=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(iCur,(DWORD)pItem);
		}
	}
}

void CStatStdPartGroupDlg::OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	if(pLVKeyDow->wVKey==VK_DELETE)
	{
		POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
			STDPART_ITEM *pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(iCurSel);
			if(pItem)
			{
				for(STDPART_ITEM *pSelItem=partList.GetFirst();pSelItem;pSelItem=partList.GetNext())
				{
					if(pSelItem==pItem)
					{
						partList.DeleteCursor();
						break;
					}
				}
			}
			m_pListCtrl->DeleteItem(iCurSel);
			pos=m_pListCtrl->GetFirstSelectedItemPosition();
		}
	}
	*pResult = 0;
}

void CStatStdPartGroupDlg::OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CStatStdPartGroupDlg::OnOK() 
{
	//按屏幕当前显示排序
	STDPART_ITEM *pItem;
	ATOM_LIST<STDPART_ITEM>tmpartlist;
	int nCount=m_pListCtrl->GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(i);
		tmpartlist.append(*pItem);
	}
	partList.Empty();
	for(pItem=tmpartlist.GetFirst();pItem;pItem=tmpartlist.GetNext())
		partList.append(*pItem);
	CDialog::OnOK();
}

void CStatStdPartGroupDlg::OnSelChangeStartPartType() 
{
	UpdateData();
	RefreshListView();
	UpdateData(FALSE);
}

void CStatStdPartGroupDlg::OnSelChangeEndPartType() 
{
	UpdateData();
	RefreshListView();
	UpdateData(FALSE);
}

void CStatStdPartGroupDlg::OnStatAllType() 
{
	UpdateData();
	RefreshListView();
	UpdateData(FALSE);
}
#endif
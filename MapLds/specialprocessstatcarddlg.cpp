//<LOCALE_TRANSLATE/>
// SpecialProcessStatCard.cpp : implementation file
//

#include "stdafx.h"
#include "Tower.h"
#include "SpecialProcessStatCardDlg.h"
#include "GlobalFunc.h"
#include "DrawTaMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecialProcessStatCardDlg dialog

#ifndef __TSA_FILE_
CSpecialProcessStatCardDlg::CSpecialProcessStatCardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecialProcessStatCardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecialProcessStatCardDlg)
	m_sSegStr = _T("*");
	m_iSpecialProcessType = 0;
	m_iStartType = 1;
	m_iEndType = 0;
	m_bAllSlot = FALSE;
	//}}AFX_DATA_INIT
	m_pCurSelItem = NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("part No.");
	m_arrHeaderLabel.Add("size");
	m_arrHeaderLabel.Add("seg num");
	m_arrHeaderLabel.Add("amount");
	m_arrHeaderLabel.Add("notes");
#else
	m_arrHeaderLabel.Add("构件编号");
	m_arrHeaderLabel.Add("规格");
	m_arrHeaderLabel.Add("段号");
	m_arrHeaderLabel.Add("件数");
	m_arrHeaderLabel.Add("备注");
#endif
	listViewComparer.SetMaxColumnCount(5);
}


void CSpecialProcessStatCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecialProcessStatCardDlg)
	DDX_Text(pDX, IDC_E_SEG_STR, m_sSegStr);
	DDX_Radio(pDX, IDC_RDO_SPECIAL_PROCESS_TYPE, m_iSpecialProcessType);
	DDX_CBIndex(pDX, IDC_CMB_START_TYPE, m_iStartType);
	DDX_CBIndex(pDX, IDC_CMB_END_TYPE, m_iEndType);
	DDX_Check(pDX, IDC_CHK_ALL_SLOT, m_bAllSlot);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecialProcessStatCardDlg, CDialog)
	//{{AFX_MSG_MAP(CSpecialProcessStatCardDlg)
	ON_BN_CLICKED(IDC_BN_REBUILD_PARTSET, OnBnRebuildPartset)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CTRL, OnColumnclickListCtrl)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CTRL, OnKeydownListCtrl)
	ON_BN_CLICKED(IDC_RDO_SPECIAL_PROCESS_TYPE, OnSpecialProcessType)
	ON_CBN_SELCHANGE(IDC_CMB_START_TYPE, OnSelchangeSlotType)
	ON_BN_CLICKED(IDC_RADIO10, OnSpecialProcessType)
	ON_CBN_SELCHANGE(IDC_CMB_END_TYPE, OnSelchangeSlotType)
	ON_BN_CLICKED(IDC_CHK_ALL_SLOT, OnAllSlot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecialProcessStatCardDlg message handlers

BOOL CSpecialProcessStatCardDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	m_pListCtrl->DeleteAllItems();
	partList.Empty();
	GetDlgItem(IDC_CMB_START_TYPE)->EnableWindow(m_iSpecialProcessType);
	GetDlgItem(IDC_CMB_END_TYPE)->EnableWindow(m_iSpecialProcessType);
	OnBnRebuildPartset();
	return TRUE;
}
void CSpecialProcessStatCardDlg::RefreshListViewHeader()
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
			lvc.cx=75;
		else if(i==3)
			lvc.cx = 75;
		else
			lvc.cx = 110;
#else 
		if(i==0)
			lvc.cx=69;
		else if(i==1) 
			lvc.cx=110;
		else if(i==2||i==3)
			lvc.cx=40;
		else
			lvc.cx = 110;
#endif
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}

int CSpecialProcessStatCardDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CSpecialProcessStatCardDlg::OnBnRebuildPartset() 
{
	UpdateData();
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(m_sSegStr,segTbl);
	DWORD key;
	CStringArray str_arr;
	str_arr.SetSize(5);
	CStringKeyHashTable<DWORD> hashTable;
	hashTable.CreateHashTable(Ta.Parts.GetNodeNum()/2);
	partList.Empty();
	m_pListCtrl->DeleteAllItems();
	CSuperSmartPtr<CLDSPart>pPart;
	for(pPart=Ta.Parts.GetFirst(CLS_LINETUBE);pPart.IsHasPtr();pPart=Ta.Parts.GetNext(CLS_LINETUBE))
	{
		if(pPart->m_bVirtualPart)
			continue;
		int idClsType=pPart->GetClassTypeId();
		if(idClsType!=CLS_LINETUBE)
			continue;
		if(strlen(pPart->GetPartNo())==0)
			continue;	//未编号构件不引入构件库
		else if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
			continue;	//分段号不在当前所选范围内
		CString sNotes="";
		BOOL bValidTube=FALSE;
		SKETCH_TYPE nSketchType=SKETCH_OTHER;
		int iStartType=pPart.LineTubePointer()->m_tJointStart.type;
		int iEndType=pPart.LineTubePointer()->m_tJointEnd.type;
		if(m_iSpecialProcessType==0)
		{
			if(iStartType==1||iEndType==1)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="intersected tube";
#else
				sNotes="相贯钢管";
#endif
				bValidTube=TRUE;	//相贯
				nSketchType=SKETCH_TUBETRANS;
			}
		}
		else if(m_iSpecialProcessType==1)
		{	//T型开口槽暂时未处理
			if(((iStartType==4&&iEndType<1)||(iEndType==4&&iStartType<1))&&
				((m_iStartType==0&&m_iEndType==1)||(m_iStartType==1&&m_iEndType==0)||m_bAllSlot))
			{
				bValidTube=TRUE;	//仅有一端开一型槽
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="linear shape slot";
#else
				sNotes="一型槽";
#endif
				nSketchType=SKETCH_RSLOT;
			}
			if(((iStartType==3&&iEndType<1)||(iEndType==3&&iStartType<1))&&
				((m_iStartType==0&&m_iEndType==3)||(m_iStartType==3&&m_iEndType==0)||m_bAllSlot))
			{
				bValidTube=TRUE;	//仅有一端开U型槽
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="U-shape slot";
#else
				sNotes="U型槽";
#endif
				nSketchType=SKETCH_USLOT;
			}
			if(((iStartType==5&&iEndType<1)||(iEndType==5&&iStartType<1))&&
				((m_iStartType==0&&m_iEndType==4)||(m_iStartType==4&&m_iEndType==0)||m_bAllSlot))
			{
				bValidTube=TRUE;	//仅有一端开十字型槽
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="cross slot";
#else
				sNotes="十字槽";
#endif
				nSketchType=SKETCH_XSLOT;
			}
			if(iStartType==4&&iEndType==4&&((m_iStartType==1&&m_iEndType==1)||m_bAllSlot))
			{
				bValidTube=TRUE;	//两端开一型槽
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="cutting linear shape slot both ends";
#else
				sNotes="两端均开一型槽";
#endif
				nSketchType=SKETCH_RSLOT;
			}
			if(iStartType==3&&iEndType==3&&((m_iStartType==3&&m_iEndType==3)||m_bAllSlot))
			{
				bValidTube=TRUE;	//两端开U型槽
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="cutting u-shape slot both ends";
#else
				sNotes="两端均开U型槽";
#endif
				nSketchType=SKETCH_USLOT;
			}
			if(iStartType==5&&iEndType==5&&((m_iStartType==4&&m_iEndType==4)||m_bAllSlot))
			{
				bValidTube=TRUE;	//两端开十字型槽
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="cutting cross slot both ends";
#else
				sNotes="两端均开十字槽";
#endif
				nSketchType=SKETCH_XSLOT;
			}
			if(((iStartType==4&&iEndType==3)||(iEndType==4&&iStartType==3))&&
				((m_iStartType==3&&m_iEndType==1)||(m_iStartType==1&&m_iEndType==3)||m_bAllSlot))
			{
				bValidTube=TRUE;	//一端开一型槽/另一端开U型槽
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="linear shape slot/U-shape slot";
#else
				sNotes="一型槽/U型槽";
#endif
				nSketchType=SKETCH_RUSLOT;
			}
			if(((iStartType==4&&iEndType==5)||(iEndType==4&&iStartType==5))&&
				((m_iStartType==4&&m_iEndType==1)||(m_iStartType==1&&m_iEndType==4)||m_bAllSlot))
			{
				bValidTube=TRUE;	//一端开一型槽/另一端开十字槽
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="linear shape slot/cross slot";
#else
				sNotes="一型槽/十字槽";
#endif
				nSketchType=SKETCH_RXSLOT;
			}
			if(((iStartType==5&&iEndType==3)||(iEndType==5&&iStartType==3))&&
				((m_iStartType==3&&m_iEndType==4)||(m_iStartType==4&&m_iEndType==3)||m_bAllSlot))
			{
				bValidTube=TRUE;	//一端开U型槽/另一端开十字槽
#ifdef AFX_TARG_ENU_ENGLISH
				sNotes="U-shape slot/cross slot";
#else
				sNotes="U型槽/十字槽";
#endif
				nSketchType=SKETCH_UXSLOT;
			}
		}
		if(!bValidTube)
			continue;
		if(hashTable.GetValueAt(pPart->GetPartNo(),key))
		{
			STDPART_ITEM *pItem=(STDPART_ITEM*)key;
			pItem->num++;
		}
		else
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->pPart=pPart;
			pItem->num=pPart->m_uStatMatCoef;
			pItem->nSketchType=nSketchType;
			hashTable.SetValueAt(pPart->GetPartNo(),(DWORD)pItem);
			char spec[200]="";
			str_arr[0]=pPart->GetPartNo();
			pPart->GetSpecification(spec,TRUE);
			str_arr[1]=spec;
			str_arr[2].Format("%s",(char*)pPart->iSeg.ToString());
			str_arr[3]="1";
			str_arr[4]=sNotes;
			int iCur=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(iCur,(DWORD)pItem);
		}
	}
	/*polypartlist.Empty();
	Ta.RetrievePolyPartList(polypartlist);
	for(CLDSPolyPart *pPolyPart=polypartlist.GetFirst();pPolyPart;pPolyPart=polypartlist.GetNext())
	{
		CLDSPart *pSonPart=pPolyPart->segset.GetFirst();
		int idClsType=pSonPart->GetClassTypeId();
		if(idClsType!=CLS_LINEANGLE||!m_bIncAngle)
			continue;
		if(!(m_sPartNo.CompareNoCase(pSonPart->GetPartNo())==0||m_sPartNo=="*"))
			continue;	//在逐个生成构件图时 如果不是指定构件编号则继续
		if(strlen(pSonPart->GetPartNo())==0)
			continue;	//未编号构件不引入构件库
		if(segTbl.GetHashTableSize()>0&&!segTbl.GetValueAt(pSonPart->iSeg,seg_i))
			continue;	//分段号不在当前所选范围内
		if(hashTable.GetValueAt(pSonPart->GetPartNo(),key))
		{
			STDPART_ITEM *pItem=(STDPART_ITEM*)key;
			pItem->num++;
		}
		else
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->pPart=pPolyPart;
			pItem->num=pSonPart->m_uStatMatCoef;
			hashTable.SetValueAt(pSonPart->GetPartNo(),(DWORD)pItem);
			str_arr[0]=pSonPart->GetPartNo();
			str_arr[1].Format("%d",pSonPart->iSeg);
			str_arr[2]="制弯角钢";
			str_arr[3]="1";
			int iCur=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(iCur,(DWORD)pItem);
		}
	}*/
	int nCount=m_pListCtrl->GetItemCount();
	for(int j=0;j<nCount;j++)
	{
		STDPART_ITEM *pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(j);
		str_arr[3].Format("%d",pItem->num);
		m_pListCtrl->SetItemText(j,3,str_arr[3]);
	}
}

void CSpecialProcessStatCardDlg::OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CSpecialProcessStatCardDlg::OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CSpecialProcessStatCardDlg::OnOK() 
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

void CSpecialProcessStatCardDlg::OnSpecialProcessType() 
{	//类型变化时更新列表
	UpdateData();
	GetDlgItem(IDC_CMB_START_TYPE)->EnableWindow(m_iSpecialProcessType);
	GetDlgItem(IDC_CMB_END_TYPE)->EnableWindow(m_iSpecialProcessType);
	OnBnRebuildPartset();
	UpdateData(FALSE);
}

void CSpecialProcessStatCardDlg::OnSelchangeSlotType() 
{
	UpdateData();
	OnBnRebuildPartset();
	UpdateData(FALSE);
}

void CSpecialProcessStatCardDlg::OnAllSlot() 
{
	UpdateData();
	OnBnRebuildPartset();
	UpdateData(FALSE);
}
#endif
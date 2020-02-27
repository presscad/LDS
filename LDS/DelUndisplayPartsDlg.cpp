//<LOCALE_TRANSLATE Confirm by hxr/>
// DelUndisplayPartsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "DelUndisplayPartsDlg.h"
#include "env_def.h"
#include "TowerPropertyDlg.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "PropertyList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

/////////////////////////////////////////////////////////////////////////////
// CDelUndisplayPartsDlg dialog


CDelUndisplayPartsDlg::CDelUndisplayPartsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDelUndisplayPartsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelUndisplayPartsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("handle");
	m_arrHeaderLabel.Add("part type");
	m_arrHeaderLabel.Add("wrong type");
#else
	m_arrHeaderLabel.Add("句柄");
	m_arrHeaderLabel.Add("构件类型");
	m_arrHeaderLabel.Add("错误类型");
#endif
	listViewComparer.SetMaxColumnCount(3);
	m_pListCtrl=NULL;
}


void CDelUndisplayPartsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelUndisplayPartsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelUndisplayPartsDlg, CDialog)
	//{{AFX_MSG_MAP(CDelUndisplayPartsDlg)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_UNDISPLAY_PARTS, OnColumnclickListUndisplayParts)
	ON_NOTIFY(NM_CLICK, IDC_LIST_UNDISPLAY_PARTS, OnClickListUndisplayParts)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_UNDISPLAY_PARTS, OnDblclkListUndisplayParts)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_UNDISPLAY_PARTS, OnKeydownListUndisplayParts)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelUndisplayPartsDlg message handlers

BOOL CDelUndisplayPartsDlg::OnInitDialog()
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_UNDISPLAY_PARTS);
	if(m_pListCtrl==NULL)
		return FALSE;
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	m_pListCtrl->DeleteAllItems();
	int nHits=0;
	CLDSPart *pPart=NULL;
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart==NULL)
			continue;
		CString tempStr;
		CStringArray str_arr;
		if(pPart->GetClassTypeId()==CLS_BOLT&&pPart->m_bVirtualPart)
			((CLDSBolt*)pPart)->GetUCS();	//更新虚拟螺栓坐标系 wht 11-01-25
		if(pPart->GetClassTypeId()==CLS_BOLT&&pPart->ucs.axis_z.IsZero())
		{
			tempStr.Format("0X%X",pPart->handle);
			str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr.Add("bolts");
			str_arr.Add("normal direction is empty");
#else
			str_arr.Add("螺栓");
			str_arr.Add("法线方向为空");
#endif
			nHits++;
			int nItem=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(nItem,pPart->handle);
		}
		else if(pPart->ucs.axis_x.IsZero()||pPart->ucs.axis_y.IsZero()||pPart->ucs.axis_z.IsZero())
		{
			CString sClassName,sError;
			if(pPart->ucs.axis_x.IsZero())
#ifdef AFX_TARG_ENU_ENGLISH
				sError="empty in X axis direction";
			else if(pPart->ucs.axis_y.IsZero())
				sError="empty in Y axis direction";
			else if(pPart->ucs.axis_z.IsZero())
				sError="empty in Z axis direction";
			if(pPart->GetClassTypeId()==CLS_PLATE)
				sClassName="Plate";
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				sClassName="parametric plate";
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				sClassName="tube";
			else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				sClassName="angle";
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				sClassName="slot steel";
			else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
				sClassName="flat steel";
			else if(pPart->GetClassTypeId()==CLS_BOLT)
				sClassName="bolts";
			else 
				sClassName="other parts";
#else
				sError="X轴方向为空";
			else if(pPart->ucs.axis_y.IsZero())
				sError="Y轴方向为空";
			else if(pPart->ucs.axis_z.IsZero())
				sError="Z轴方向为空";
			if(pPart->GetClassTypeId()==CLS_PLATE)
				sClassName="钢板";
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
				sClassName="参数化板";
			else if(pPart->GetClassTypeId()==CLS_LINETUBE)
				sClassName="钢管";
			else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				sClassName="角钢";
			else if(pPart->GetClassTypeId()==CLS_LINESLOT)
				sClassName="槽钢";
			else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
				sClassName="扁钢";
			else if(pPart->GetClassTypeId()==CLS_BOLT)
				sClassName="螺栓";
			else 
				sClassName="其它构件";
#endif
			tempStr.Format("0X%X",pPart->handle);
			str_arr.Add(tempStr);
			str_arr.Add(sClassName);
			str_arr.Add(sError);
			nHits++;
			int nItem=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(nItem,pPart->handle);
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate *pPlate=(CLDSPlate*)pPart;
			if(pPlate->vertex_list.GetNodeNum()<3)
			{
				tempStr.Format("0X%X",pPart->handle);
				str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("plate");
				str_arr.Add("number of vertex is less than 3");
#else
				str_arr.Add("钢板");
				str_arr.Add("顶点数小于3");
#endif
				nHits++;
				int nItem=AppendRecord(-1,str_arr);
				m_pListCtrl->SetItemData(nItem,pPart->handle);
			}
			else if(pPlate->GetThick()<=0)
			{
				tempStr.Format("0X%X",pPart->handle);
				str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("plate");
				str_arr.Add("plate's thickness is 0");
#else
				str_arr.Add("钢板");
				str_arr.Add("钢板厚度为0");
#endif
				nHits++;
				int nItem=AppendRecord(-1,str_arr);
				m_pListCtrl->SetItemData(nItem,pPart->handle);
			}
			else if(pPlate->GetLength()<=0)
			{
				tempStr.Format("0X%X",pPart->handle);
				str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("plate");
				str_arr.Add("plate's length is 0");
#else
				str_arr.Add("钢板");
				str_arr.Add("钢板长度为0");
#endif
				nHits++;
				int nItem=AppendRecord(-1,str_arr);
				m_pListCtrl->SetItemData(nItem,pPart->handle);
			}
			else if(pPlate->GetWidth()<=0)
			{
				tempStr.Format("0X%X",pPart->handle);
				str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("plate");
				str_arr.Add("plate's width is 0");
#else
				str_arr.Add("钢板");
				str_arr.Add("钢板宽度为0");
#endif
				nHits++;
				int nItem=AppendRecord(-1,str_arr);
				m_pListCtrl->SetItemData(nItem,pPart->handle);
			}
		}
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPart)->vertex_list.GetNodeNum()<3)
		{
			((CLDSParamPlate*)pPart)->DesignPlate();
			if(((CLDSParamPlate*)pPart)->vertex_list.GetNodeNum()<3)
			{
				tempStr.Format("0X%X",pPart->handle);
				str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr.Add("parametric plate");
				str_arr.Add("number of vertex is less than 3");
#else
				str_arr.Add("参数化钢板");
				str_arr.Add("顶点数小于3");
#endif
				nHits++;
				int nItem=AppendRecord(-1,str_arr);
				m_pListCtrl->SetItemData(nItem,pPart->handle);
			}
		}
		else if(pPart->IsLinePart()&&((CLDSLinePart*)pPart)->GetLength()==0)
		{
			tempStr.Format("0X%X",pPart->handle);
			str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr.Add("rod");
			str_arr.Add("length is 0");
#else
			str_arr.Add("杆件");
			str_arr.Add("长度为0");
#endif
			int nItem=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(nItem,pPart->handle);
		}
		else if(pPart->pSolidBody&&pPart->pSolidBody->KeyPointNum()<2)
		{
			if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
				continue;	//跳过组合角钢,组合角钢没有对应的实体只有子角
			tempStr.Format("0X%X",pPart->handle);
			str_arr.Add(tempStr);
#ifdef AFX_TARG_ENU_ENGLISH
			str_arr.Add("other parts");
			str_arr.Add("number of vertex is less than 3");
#else
			str_arr.Add("其它构件");
			str_arr.Add("顶点数小于3");
#endif
			nHits++;
			int nItem=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(nItem,pPart->handle);
		}
	}
	CDialog::OnInitDialog();
	return TRUE;
}

void CDelUndisplayPartsDlg::RefreshListViewHeader()
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
		if(i==0)
			lvc.cx = 60;
		else if(i==1)
			lvc.cx = 80;
		else if(i==2)
			lvc.cx = 150;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}

int CDelUndisplayPartsDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CDelUndisplayPartsDlg::DeleteItem(int nCurSel) 
{
	char tem_str[101];
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	long handle=0;
	sscanf(tem_str,"%X",&handle);
	CLDSPart *pPart=NULL;
	pPart=console.FromPartHandle(handle);
	if(pPart)
	{
		if(pPart->GetClassTypeId()==CLS_BOLT)
			Ta.DelBoltAndRelativeBoltRef(pPart->handle);
		else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
			Ta.DelTubeEndLj(pPart->handle);
		else
		{	
			console.DispPartSet.DeleteNode(pPart->handle);
			g_pSolidDraw->DelEnt(pPart->handle);
			console.DeletePart(pPart->handle);
		}
	}
	m_pListCtrl->DeleteItem(nCurSel);
}

void CDelUndisplayPartsDlg::SelectPart(int nCurSel) 
{
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CLDSView *pLatticeTowerView=(CLDSView*)theApp.GetLDSDoc()->GetView(RUNTIME_CLASS(CLDSView));
	char tem_str[101];
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	long hCurPart=0;
	sscanf(tem_str,"%X",&hCurPart);
	CLDSPart *pPart=console.FromPartHandle(hCurPart);
	if(pPart==NULL)
		return;
	//更新属性框
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(hCurPart);
	pLatticeTowerView->UpdatePropertyPage(pPart);
}

void CDelUndisplayPartsDlg::OnColumnclickListUndisplayParts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CDelUndisplayPartsDlg::OnClickListUndisplayParts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			SelectPart(iCurSel);
	}
	*pResult = 0;
}

void CDelUndisplayPartsDlg::OnDblclkListUndisplayParts(NMHDR* pNMHDR, LRESULT* pResult)
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			SelectPart(iCurSel);
	}
	CDialog::OnOK();
	*pResult = 0;
}

void CDelUndisplayPartsDlg::OnKeydownListUndisplayParts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_DELETE)
		{
			while(iCurSel>=0)
			{
				DeleteItem(iCurSel);
				pos=m_pListCtrl->GetFirstSelectedItemPosition();
				if(pos==NULL)
					break;
				iCurSel=m_pListCtrl->GetNextSelectedItem(pos);
			}
		}
	}
	*pResult = 0;
}

void CDelUndisplayPartsDlg::OnOK() 
{
	int i=0;
	for(i=0;i<m_pListCtrl->GetItemCount();)
		DeleteItem(i);
	CDialog::OnOK();
}

void CDelUndisplayPartsDlg::OnCancel() 
{	
	CDialog::OnCancel();
}
#endif
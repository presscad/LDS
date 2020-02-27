//<LOCALE_TRANSLATE Confirm by hxr/>
// PartsDbDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "env_def.h"
#include "PartsDbDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CPartsDbDlg dialog
CPartsDbDlg::CPartsDbDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPartsDbDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartsDbDlg)
	m_bIncPlate = FALSE;
	m_bIncParamUend = FALSE;
	m_bIncParamRollEnd = FALSE;
	m_bIncParamFl = FALSE;
	m_bIncParamElbow = FALSE;
	m_bIncParamCirRib = FALSE;
	m_bIncParamCirLid = FALSE;
	m_bIncParamXEnd = FALSE;	
	m_bIncParamAngleRib = FALSE;
	m_bIncParamRibPlate = FALSE;
	m_sSegStr =_T("*");
	m_bIncParamRibPlate = FALSE;
	//}}AFX_DATA_INIT
	m_bRefreshDlgCtrl = FALSE;
	m_dwSelectPartsType=SELECT_ALL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("part No.");
	m_arrHeaderLabel.Add("type");
	m_arrHeaderLabel.Add("current pieces");
#else
	m_arrHeaderLabel.Add("构件编号");
	m_arrHeaderLabel.Add("类型");
	m_arrHeaderLabel.Add("当前件数");
#endif
	m_pSelectPart=NULL;
	listViewComparer.SetMaxColumnCount(3);
}


void CPartsDbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartsDbDlg)
	DDX_Check(pDX, IDC_CHK_PLATE, m_bIncPlate);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_UEND, m_bIncParamUend);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_ROLLEND, m_bIncParamRollEnd);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_FL, m_bIncParamFl);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_ELBOW, m_bIncParamElbow);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_CIRRIB, m_bIncParamCirRib);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_CIRLID, m_bIncParamCirLid);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_XEND, m_bIncParamXEnd);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_LINEANGLE_RIB, m_bIncParamAngleRib);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_RIBPLATE, m_bIncParamRibPlate);
	DDX_Check(pDX, IDC_CHK_PARAM_TYPE_WATEPLATE, m_bIncParamWaterPlate);
	DDX_Text(pDX, IDC_E_SEG_STR, m_sSegStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartsDbDlg, CDialog)
	//{{AFX_MSG_MAP(CPartsDbDlg)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_ELBOW, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_XEND, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_FL, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_ROLLEND, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_UEND, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_CIRRIB, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_CIRLID, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_LINEANGLE_RIB, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_RIBPLATE, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PARAM_TYPE_WATEPLATE, OnChkTypeModify)
	ON_BN_CLICKED(IDC_CHK_PLATE, OnChkTypeModify)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CTRL, OnDblclkListCtrl)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CTRL, OnColumnclickListCtrl)
	ON_EN_CHANGE(IDC_E_SEG_STR, OnChangeSegStr)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_EXPORT_PART, &CPartsDbDlg::OnBnClickedBtnExportPart)
	ON_BN_CLICKED(IDC_BTN_IMPORT_PART, &CPartsDbDlg::OnBnClickedBtnImportPart)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartsDbDlg message handlers

BOOL CPartsDbDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	partList.Empty();
	m_pSelectPart=NULL;
	if(m_bRefreshDlgCtrl)
		RefreshDlgCtrlState();
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CTRL);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	m_pListCtrl->DeleteAllItems();
	ReadLocalStdPartLibary();
	RefreshPartsDb();
	return TRUE;  
}

void CPartsDbDlg::RefreshListViewHeader()
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

int CPartsDbDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CPartsDbDlg::OnOK() 
{
	POSITION pos=m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iSel=m_pListCtrl->GetNextSelectedItem(pos);
		STDPART_ITEM *pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(iSel);
		if(pItem)
			m_pSelectPart=pItem->pPart;
	}
	
	CDialog::OnOK();
}

void CPartsDbDlg::OnChkTypeModify() 
{
	UpdateData();
	RefreshPartsDb();
}
void CPartsDbDlg::ReadLocalStdPartLibary()
{
	if(!m_bIncParamUend && !m_bIncParamRollEnd && !m_bIncParamXEnd && !m_bIncParamFl)
		return;
	CAttachBuffer *pLibraryBuffer=Ta.AttachBuffer.GetValue(CTower::PRJPARAMPART_LIBRAY);
	if(Ta.localStdPartLibray.GetNodeNum()==0&&pLibraryBuffer!=NULL&&pLibraryBuffer->GetLength()>0)
	{
		int n=0,thick=0,iParamType=0;
		DWORD iSeg;					//段号
		char cMaterial;				//材质
		pLibraryBuffer->SeekToBegin();
		pLibraryBuffer->ReadInteger(&n);
		for(int i=0;i<n;i++)
		{
			CXhChar16 label;
			CLDSParamPlate* pLocalStdParamPlate=Ta.localStdPartLibray.append();
			pLibraryBuffer->ReadString(label,17);	 pLocalStdParamPlate->SetPartNo(label);
			pLibraryBuffer->ReadByte(&cMaterial);	 pLocalStdParamPlate->cMaterial=cMaterial;
			pLibraryBuffer->ReadInteger(&iParamType);pLocalStdParamPlate->m_iParamType=iParamType;
			pLibraryBuffer->ReadInteger(&thick);	 pLocalStdParamPlate->thick=thick;
			pLibraryBuffer->ReadDword(&iSeg);		 pLocalStdParamPlate->iSeg=iSeg;
			int param_n=0;
			pLibraryBuffer->ReadInteger(&param_n);
			pLocalStdParamPlate->params.Empty();
			for(int j=0;j<param_n;j++)
			{
				KEY2C key;
				pLibraryBuffer->ReadWord((WORD*)&key);
				DESIGN_PLATE_ITEM* pItem=pLocalStdParamPlate->params.Add(key);
				pLibraryBuffer->ReadByte(&pItem->dataType);
				if(pItem->dataType==1)
					pLibraryBuffer->ReadInteger(&pItem->value.hPart);
				else if(pItem->dataType==2)
					pLibraryBuffer->ReadInteger(&pItem->value.iType);
				else
					pLibraryBuffer->ReadDouble(&pItem->value.fValue);
				pLibraryBuffer->ReadString(pItem->sNotes);
				pLibraryBuffer->ReadByte(&pItem->paramType);
			}
		}
	}
}

static long classTypeIdTable[32];
void CPartsDbDlg::RefreshPartsDb()
{
	UpdateData();
	double X=0;
	CHashList<SEGI>segTbl;
	GetSegNoHashTblBySegStr(m_sSegStr,segTbl);
	DWORD key;
	CStringArray str_arr;
	str_arr.SetSize(3);
	CHashStrTable<DWORD> hashTable;
	hashTable.CreateHashTable(Ta.Parts.GetNodeNum()/2);
	classTypeIdTable[SELECTINDEX_NODE-1]=CLS_NODE;
	classTypeIdTable[SELECTINDEX_LINEANGLE-1]=CLS_LINEANGLE;
	classTypeIdTable[SELECTINDEX_LINETUBE-1]=CLS_LINETUBE;
	classTypeIdTable[SELECTINDEX_LINEFLAT-1]=CLS_LINEFLAT;
	classTypeIdTable[SELECTINDEX_LINESLOT-1]=CLS_LINESLOT;
	classTypeIdTable[SELECTINDEX_PLATE-1]=CLS_PLATE;
	classTypeIdTable[SELECTINDEX_PARAMPLATE-1]=CLS_PARAMPLATE;
	classTypeIdTable[SELECTINDEX_BOLT-1]=CLS_BOLT;
	classTypeIdTable[SELECTINDEX_ARCANGLE-1]=CLS_ARCANGLE;
	classTypeIdTable[SELECTINDEX_ARCFLAT-1]=CLS_ARCFLAT;
	classTypeIdTable[SELECTINDEX_ARCSLOT-1]=CLS_ARCSLOT;
	m_pListCtrl->DeleteAllItems();
	partList.Empty();
	CLDSPart *pPart;
	PARTSET partset;
	for(pPart=Ta.localStdPartLibray.GetFirst();pPart;pPart=Ta.localStdPartLibray.GetNext())
		partset.append(pPart);
	for(pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		partset.append(pPart);
	for(CLDSPart *pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
	{
		/*for(int i=0;i<32;i++)
		{
			DWORD flag=GetSingleWord(i+1);
			if(classTypeIdTable[i]==pPart->m_nClassTypeId&&(m_dwSelectPartsType&flag))
				break;
		}
		if(i==32)
			continue;*/
		CLDSParamPlate *pParamPlate=NULL;
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			pParamPlate=(CLDSParamPlate*)pPart;
			if(pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE&&!m_bIncParamWaterPlate)
				continue;	//遮水板
			if(pParamPlate->IsFL()&&!m_bIncParamFl)
				continue;
			else if(pParamPlate->m_iParamType==TYPE_UEND&&!m_bIncParamUend)
				continue;
			else if(pParamPlate->m_iParamType==TYPE_ROLLEND&&!m_bIncParamRollEnd)
				continue;
			else if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE&&!m_bIncParamElbow)
				continue;	//环向肘板
			else if((pParamPlate->m_iParamType==TYPE_TUBERIBPLATE||pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)&&
				!m_bIncParamCirRib)
				continue;	//钢管跨越连板或环向肋板（均属于环向板）
			else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE&&!m_bIncParamCirLid)
				continue;	//环向封板
			else if(pParamPlate->m_iParamType==TYPE_XEND&&!m_bIncParamXEnd)//||!pParamPlate->GetDesignItemValue('X',&X)))
				continue;	//十字插板及其包板看做一个整体,目前已不须用'X'参数标识主板，该代码会导致无法筛选出十字插板，故去掉 wjh-2014.5.5
			else if(pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE&&!m_bIncParamAngleRib)
				continue;
			else if(pParamPlate->m_iParamType==TYPE_RIBPLATE&&!m_bIncParamRibPlate)
				continue;	//普通角肋板
		}
		else if(pPart->GetClassTypeId()==CLS_PLATE&&!m_bIncPlate)
			continue;
		else if(pPart->GetClassTypeId()!=CLS_PLATE&&pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		if(segTbl.GetNodeNum()>0&&segTbl.GetValue(pPart->iSeg.iSeg)==NULL)
			continue;	//分段号不在当前所选范围内
		if(strlen(pPart->GetPartNo())<=0)	//只有已编号的构件才会统计到零件库中
			continue;
		if(hashTable.GetValueAt(pPart->GetPartNo(),key))
		{
			STDPART_ITEM *pItem=(STDPART_ITEM*)key;
			if(pPart->handle>0x20)
				pItem->num++;
			//else 	工程临时使用参数化零件库构件，不是真实的构件
		}
		else
		{
			STDPART_ITEM *pItem=partList.append();
			pItem->pPart=pPart;
			pItem->num=pPart->m_uStatMatCoef;
			hashTable.SetValueAt(pPart->GetPartNo(),(DWORD)pItem);
			str_arr[0]=pPart->GetPartNo();
			if(pPart->GetClassTypeId()==CLS_PLATE)
#ifdef AFX_TARG_ENU_ENGLISH
				str_arr[1]="Ordinary Plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FL)
				str_arr[1]="Flange";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_UEND)
				str_arr[1]="U Plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_ROLLEND)
				str_arr[1]="Slot Plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
				str_arr[1]="cross plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
				str_arr[1]="elbow plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
				str_arr[1]="circular rib plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				str_arr[1]="ring sealing plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				str_arr[1]="Span Connecting Plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
				str_arr[1]="angle rib plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_RIBPLATE)
				str_arr[1]="rib plate";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FLD)
				str_arr[1]="Welding Neck Flange";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FLP)
				str_arr[1]="flat?welding?flange";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FLG)
				str_arr[1]="rigid flange";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FLR)
				str_arr[1]="flexible flange";
#else
				str_arr[1]="普通钢板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FL)
				str_arr[1]="法兰";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_UEND)
				str_arr[1]="U型插板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_ROLLEND)
				str_arr[1]="槽型插板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
				str_arr[1]="十字插板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
				str_arr[1]="肘板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
				str_arr[1]="环向肋板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
				str_arr[1]="环向封板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
				str_arr[1]="跨越钢管连板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
				str_arr[1]="角钢肋板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_RIBPLATE)
				str_arr[1]="肋板";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FLD)
				str_arr[1]="对焊法兰";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FLP)
				str_arr[1]="平焊法兰";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FLG)
				str_arr[1]="刚性法兰";
			else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_FLR)
				str_arr[1]="柔性法兰";
#endif
			else
				str_arr[1]="";
			str_arr[2]="1";
			int iCur=AppendRecord(-1,str_arr);
			m_pListCtrl->SetItemData(iCur,(DWORD)pItem);
		}
	}
	int nCount=m_pListCtrl->GetItemCount();
	for(int j=0;j<nCount;j++)
	{
		STDPART_ITEM *pItem=(STDPART_ITEM*)m_pListCtrl->GetItemData(j);
		str_arr[2].Format("%d",pItem->num);
		m_pListCtrl->SetItemText(j,2,str_arr[2]);
	}
}

void CPartsDbDlg::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPartsDbDlg::OnOK();
	*pResult = 0;
}

void CPartsDbDlg::OnColumnclickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}

void CPartsDbDlg::RefreshDlgCtrlState()
{
	GetDlgItem(IDC_CHK_PLATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_UEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_ROLLEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_FL)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_CIRRIB)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_CIRLID)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_ELBOW)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_XEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_LINEANGLE_RIB)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_RIBPLATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_PARAM_TYPE_WATEPLATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EXPORT_PART)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_IMPORT_PART)->EnableWindow(FALSE);
	if(m_bIncPlate)
		GetDlgItem(IDC_CHK_PLATE)->EnableWindow(TRUE);
	else if(m_bIncParamUend)
		GetDlgItem(IDC_CHK_PARAM_TYPE_UEND)->EnableWindow(TRUE);
	else if(m_bIncParamRollEnd)
		GetDlgItem(IDC_CHK_PARAM_TYPE_ROLLEND)->EnableWindow(TRUE);
	else if(m_bIncParamFl)
		GetDlgItem(IDC_CHK_PARAM_TYPE_FL)->EnableWindow(TRUE);
	else if(m_bIncParamCirRib)
		GetDlgItem(IDC_CHK_PARAM_TYPE_CIRRIB)->EnableWindow(TRUE);
	else if(m_bIncParamCirLid)
		GetDlgItem(IDC_CHK_PARAM_TYPE_CIRLID)->EnableWindow(TRUE);
	else if(m_bIncParamElbow)
		GetDlgItem(IDC_CHK_PARAM_TYPE_ELBOW)->EnableWindow(TRUE);
	else if(m_bIncParamXEnd)
		GetDlgItem(IDC_CHK_PARAM_TYPE_XEND)->EnableWindow(TRUE);
	else if(m_bIncParamAngleRib)
		GetDlgItem(IDC_CHK_PARAM_TYPE_LINEANGLE_RIB)->EnableWindow(TRUE);
	else if(m_bIncParamRibPlate)
		GetDlgItem(IDC_CHK_PARAM_TYPE_RIBPLATE)->EnableWindow(TRUE);
	else if(m_bIncParamWaterPlate)
		GetDlgItem(IDC_CHK_PARAM_TYPE_WATEPLATE)->EnableWindow(TRUE);
	if(m_bIncParamUend || m_bIncParamRollEnd || m_bIncParamXEnd || m_bIncParamFl)
	{
		GetDlgItem(IDC_BTN_EXPORT_PART)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_IMPORT_PART)->EnableWindow(TRUE);
	}
}

void CPartsDbDlg::OnChangeSegStr() 
{
	UpdateData();
	m_pListCtrl->DeleteAllItems();
	RefreshPartsDb();
}

//导出法兰、各种插板到本地文件
#include "BufferStack.h"
void CPartsDbDlg::OnBnClickedBtnExportPart()
{
	char sName[50]="",sFileType[50]="";
	strcpy(sName,"ParamPlate");
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(sFileType,"project parts warehouse file(*.lpt)|*.lpt||");
#else
	strcpy(sFileType,"工程零件库文件(*.lpt)|*.lpt||");
#endif
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()!=IDOK)
		return;
	CBuffer buffer;
	int nNum=0;
	CBufferStack bufstack;
	buffer.AttachStack(&bufstack);
	int level_0=buffer.PushPositionStack();
	buffer.WriteInteger(0);
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		CLDSParamPlate *pParaPlate=NULL;
		if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		pParaPlate=(CLDSParamPlate*)pPart;
		if(pParaPlate->m_bStdPart && pParaPlate->IsFL())
			continue;
		if( pParaPlate->m_iParamType!=TYPE_UEND && pParaPlate->m_iParamType!=TYPE_XEND &&
			pParaPlate->m_iParamType!=TYPE_ROLLEND && pParaPlate->m_iParamType!=TYPE_FL)
			continue;
		buffer.WriteString(pParaPlate->GetPartNo());	
		buffer.WriteByte(pParaPlate->cMaterial);
		buffer.WriteInteger(pParaPlate->m_iParamType);
		buffer.WriteInteger(pParaPlate->thick);
		buffer.WriteDword(pParaPlate->iSeg);			
		int level_1=buffer.PushPositionStack();
		buffer.WriteInteger(0);
		for(DESIGN_PLATE_ITEM* pItem=pParaPlate->params.GetFirst();pItem;pItem=pParaPlate->params.GetNext())
		{	//设计参数
			buffer.WriteWord(pItem->key);
			buffer.WriteByte(pItem->dataType);
			if(pItem->dataType==1)
				buffer.WriteInteger(pItem->value.hPart);
			else if(pItem->dataType==2)
				buffer.WriteInteger(pItem->value.iType);
			else
				buffer.WriteDouble(pItem->value.fValue);
			buffer.WriteString(pItem->sNotes);
			buffer.WriteByte(pItem->paramType);
			buffer.IncrementLevelCount(level_1);
		}
		buffer.PopPositionStack(level_1);
		buffer.WriteInteger(buffer.LevelCount(level_1));
		buffer.RestoreNowPosition();
		buffer.IncrementLevelCount(level_0);
	}
	buffer.PopPositionStack(level_0);
	buffer.WriteInteger(buffer.LevelCount(level_0));
	buffer.RestoreNowPosition();

	FILE *fp=NULL;
	if((fp=fopen(dlg.GetPathName(),"wb"))==NULL)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox(CXhChar200("project parts warehouse file cannot be generated'%s'",dlg.GetPathName()));
#else
		AfxMessageBox(CXhChar200("无法生成工程零件库文件'%s'",dlg.GetPathName()));
#endif
		return;
	}
	long version=0;
	ULONG bufsize=buffer.GetLength();
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar50 doctype("LMA project parts warehouse");
#else
	CXhChar50 doctype("LMA工程零件库");
#endif
	BYTE len=doctype.GetLength();
	fwrite(&len,1,1,fp);
	fwrite(&doctype,len,1,fp);
	fwrite(&version,4,1,fp);
	fwrite(&bufsize,4,1,fp);
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
}
//从本地文件导入零件库
void CPartsDbDlg::OnBnClickedBtnImportPart()
{
	char sName[50]="",sFileType[50]="";
	strcpy(sName,"ParamPlate");
#ifdef AFX_TARG_ENU_ENGLISH
	strcpy(sFileType,"project parts warehouse file(*.lpt)|*.lpt||");
#else
	strcpy(sFileType,"工程零件库文件(*.lpt)|*.lpt||");
#endif
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()!=IDOK)
		return;
	FILE *fp=NULL;
	if((fp=fopen(dlg.GetPathName(),"rb"))==NULL)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox(CXhChar200("project parts warehouse file can not open'%s'",dlg.GetPathName()));
#else
		AfxMessageBox(CXhChar200("无法打开工程零件库文件'%s'",dlg.GetPathName()));
#endif
		return;
	}
	CXhChar50 doctype;//("LMA工程零件库");
	BYTE len=0;
	fread(&len,1,1,fp);
	fread(&doctype,min(len,50),1,fp);
#ifdef AFX_TARG_ENU_ENGLISH
	if(!doctype.EqualNoCase("LMA project parts warehouse"))
	{
		AfxMessageBox("wrong file type");
#else
	if(!doctype.EqualNoCase("LMA工程零件库"))
	{
		AfxMessageBox("错误的文件类型");
#endif
		fclose(fp);
		return;
	}
	long version=0;
	ULONG bufsize;
	fread(&version,4,1,fp);
	fread(&bufsize,4,1,fp);
	CAttachBuffer* pBuff=Ta.AttachBuffer.Add(CTower::PRJPARAMPART_LIBRAY);
	pBuff->ClearContents();
	pBuff->WriteAt(0,NULL,bufsize);
	fread(pBuff->GetBufferPtr(),bufsize,1,fp);
	fclose(fp);
	ReadLocalStdPartLibary();
	RefreshPartsDb();
}
#endif

//<LOCALE_TRANSLATE Confirm by hxr/>
// PlateDesignParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "ListViewComparer.h"
#include "PlateDesignParamDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////////////////////////
// CPlateDesignParamDlg dialog
CPlateDesignParamDlg::CPlateDesignParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlateDesignParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlateDesignParamDlg)
	m_sParamName = _T("A");
	m_sParamNotes = _T("A");
	m_sParamValue = _T("");
	m_iDataType = 0;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("para name");
	m_arrHeaderLabel.Add("para");
	m_arrHeaderLabel.Add("para type");
	m_arrHeaderLabel.Add("notes");
#else
	m_arrHeaderLabel.Add("参数名");
	m_arrHeaderLabel.Add("参数值");
	m_arrHeaderLabel.Add("参数值类型");
	m_arrHeaderLabel.Add("备注");
#endif
	m_pWorkPart = NULL;
	m_bAddItem = FALSE;
	listViewComparer.SetMaxColumnCount(4);
}


void CPlateDesignParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlateDesignParamDlg)
	DDX_Text(pDX, IDC_E_PARAM_NAME, m_sParamName);
	DDX_Text(pDX, IDC_E_PARAM_NOTES, m_sParamNotes);
	DDX_Text(pDX, IDC_E_PARAM_VALUE, m_sParamValue);
	DDX_CBIndex(pDX, IDC_CMB_DATA_TYPE, m_iDataType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlateDesignParamDlg, CDialog)
	//{{AFX_MSG_MAP(CPlateDesignParamDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PLATE_DESIGN_PARAM, OnDblclkListPlateDesignParam)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_PLATE_DESIGN_PARAM, OnKeydownListPlateDesignParam)
	ON_BN_CLICKED(IDC_BTN_MODIFY, OnBtnModify)
	ON_BN_CLICKED(IDC_BTN_ADD_ITEM, OnBtnAddItem)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PLATE_DESIGN_PARAM, OnColumnclickListPlateDesignParam)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlateDesignParamDlg message handlers

BOOL CPlateDesignParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CButton *pBtnAdd = (CButton*)GetDlgItem(IDC_BTN_ADD_ITEM);
	if(m_bAddItem)
		pBtnAdd->ShowWindow(SW_SHOW);
	else
		pBtnAdd->ShowWindow(SW_HIDE);
	if(m_pWorkPart==NULL)
		return FALSE;
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_PLATE_DESIGN_PARAM);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	m_pListCtrl->DeleteAllItems();
	if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)m_pWorkPart;
		CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
		m_CurPlateBmp.Detach();
		
		if(pParamPlate->m_iParamType==TYPE_FL||
			pParamPlate->m_iParamType==TYPE_FLG||
			pParamPlate->m_iParamType==TYPE_FLR)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_FL2);
		else if(pParamPlate->m_iParamType==TYPE_UEND)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_UEND_BOLT);
		else if(pParamPlate->m_iParamType==TYPE_ROLLEND)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_ROLLEND_BOLT);
		else if(pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_COVERPLATE);
		else if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_ELBOW_PLATE);
		else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_CIRCULAR_RIBPLATE);
		else if(pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_TUBE_RIB_PLATE);
		else if(pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_LINEANGLE_RIB_PLATE);
		else if(pParamPlate->m_iParamType==TYPE_XEND)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_XEND_BOLT);
		else if(pParamPlate->m_iParamType==TYPE_FLD)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLD);
		else if(pParamPlate->m_iParamType==TYPE_FLP)
			m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLP);
		else
			m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_NONE);
		pPic->SetBitmap(m_CurPlateBmp);
		if(pParamPlate->m_bStdPart)	//标准构件
			GetDlgItem(IDC_BTN_MODIFY)->EnableWindow(FALSE);
		//将已经设置好的钢板参数加入列表中
		if(pParamPlate->params.GetNodeNum()>0)
		{
			for(DESIGN_PLATE_ITEM *pItem=pParamPlate->params.GetFirst();pItem;pItem=pParamPlate->params.GetNext())
			{
				CString ss;
				CStringArray str_arr;
				ss.Format("%s", pItem->key.Label());
				str_arr.Add(ss);
				if(pItem->dataType==0)
				{
					ss.Format("%f", pItem->value.fValue);
					SimplifiedNumString(ss);
					str_arr.Add(ss);
#ifdef AFX_TARG_ENU_ENGLISH
					str_arr.Add("Real Type");
#else
					str_arr.Add("实数类型");
#endif
				}
				else if(pItem->dataType==1)
				{
					if(pItem->value.hPart<0)
						ss.Format("-0X%X", abs(pItem->value.hPart));
					else
						ss.Format("0X%X", pItem->value.hPart);
					str_arr.Add(ss);
#ifdef AFX_TARG_ENU_ENGLISH
					str_arr.Add("Handle Type");
#else
					str_arr.Add("句柄类型");
#endif
				}
				ss.Format("%s", pItem->sNotes);
				str_arr.Add(ss);
				AppendRecord(-1,str_arr);
			}
		}
	}
	return TRUE;
}

void CPlateDesignParamDlg::RefreshListViewHeader()
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
		else if(i==1||i==2)
			lvc.cx =80;
		else if(i==3)
			lvc.cx = 240;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
int CPlateDesignParamDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CPlateDesignParamDlg::DeleteItem(int nCurSel) 
{
	char cName, tem_str[101];
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	cName = *tem_str;
	if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
		((CLDSParamPlate*)m_pWorkPart)->DeleteDesignItem(cName);
	m_pListCtrl->DeleteItem(nCurSel);
}

void CPlateDesignParamDlg::OnDblclkListPlateDesignParam(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_PLATE_DESIGN_PARAM);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		CString ss;
		char sText[51];
		m_pListCtrl->GetItemText(iCurSel,0,sText,50);
		m_sParamName.Format("%s",sText);
		m_pListCtrl->GetItemText(iCurSel,2,sText,50);
		ss.Format("%s",sText);
#ifdef AFX_TARG_ENU_ENGLISH
		if(ss.CompareNoCase("Real Type")==0)
#else
		if(ss.CompareNoCase("实数类型")==0)
#endif
		{
			m_iDataType = 0;
			m_pListCtrl->GetItemText(iCurSel,1,sText,50);
			m_sParamValue.Format("%s",sText);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(ss.CompareNoCase("Handle Type")==0)
#else
		else if(ss.CompareNoCase("句柄类型")==0)
#endif
		{
			long h=0;
			m_iDataType = 1;
			m_pListCtrl->GetItemText(iCurSel,1,sText,50);
			if(sText[0]=='-')
			{	
				sscanf(sText+1,"%X",&h);
				m_sParamValue.Format("-0X%X",h);
			}
			else
			{
				sscanf(sText,"%X",&h);
				m_sParamValue.Format("0X%X",h);
			}
		}
		m_pListCtrl->GetItemText(iCurSel,3,sText,50);
		m_sParamNotes.Format("%s",sText);
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CPlateDesignParamDlg::OnKeydownListPlateDesignParam(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CPlateDesignParamDlg::OnBtnModify() 
{
	m_pListCtrl=(CListCtrl*)GetDlgItem(IDC_LIST_PLATE_DESIGN_PARAM);
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		char ss[101];
		sprintf(ss,"%s",m_sParamName);
		m_pListCtrl->SetItemText(iCurSel,0,ss);		
		if(m_iDataType==0)
		{
			sprintf(ss,"%s",m_sParamValue);
#ifdef AFX_TARG_ENU_ENGLISH
			m_pListCtrl->SetItemText(iCurSel,2,"Real Type");
#else
			m_pListCtrl->SetItemText(iCurSel,2,"实数类型");
#endif
		}
		else if(m_iDataType==1)
		{
			sprintf(ss,"%s",m_sParamValue);
#ifdef AFX_TARG_ENU_ENGLISH
			m_pListCtrl->SetItemText(iCurSel,2,"Handle Type");
#else
			m_pListCtrl->SetItemText(iCurSel,2,"句柄类型");
#endif
		}
		SimplifiedNumString(ss);
		m_pListCtrl->SetItemText(iCurSel,1,ss);
		sprintf(ss,"%s",m_sParamNotes);
		m_pListCtrl->SetItemText(iCurSel,3,ss);
		m_pListCtrl->SetItemData(iCurSel,1);	//表示该项已修改过
	}
}

void CPlateDesignParamDlg::OnOK() 
{	//暂时未启用该对话框
	if(m_pWorkPart&&m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pPlate = (CLDSParamPlate*)m_pWorkPart;
		pPlate->params.Empty();
		int nCount=m_pListCtrl->GetItemCount();
		for(int i=0;i<nCount;i++)
		{
			CString ss;
			char cName,sText[101],sNotes[101],dataType[101];
			BOOL bModify=FALSE;
			if(m_pListCtrl->GetItemData(i)==1)
				bModify=TRUE;
			m_pListCtrl->GetItemText(i,0,sText,100);
			cName = *sText;			
			m_pListCtrl->GetItemText(i,1,sText,100);
			m_pListCtrl->GetItemText(i,2,dataType,100);
			ss.Format("%s",dataType);
			m_pListCtrl->GetItemText(i,3,sNotes,100);
#ifdef AFX_TARG_ENU_ENGLISH
			if(ss.CompareNoCase("Real Type")==0)
				pPlate->SetDesignItemValue(cName,atof(sText),sNotes,0,bModify);
			else if(ss.CompareNoCase("Handle Type")==0)
#else
			if(ss.CompareNoCase("实数类型")==0)
				pPlate->SetDesignItemValue(cName,atof(sText),sNotes,0,bModify);
			else if(ss.CompareNoCase("句柄类型")==0)
#endif
			{
				long h=0;
				sscanf(sText,"%X",&h);
				pPlate->SetDesignItemValue(cName,h,sNotes,1,bModify);
			}
		}
		m_pWorkPart = pPlate;
	}
	CDialog::OnOK();
}

void CPlateDesignParamDlg::OnBtnAddItem() 
{
	CString ss;
	CStringArray str_arr;
	UpdateData();
	ss.Format("%s", m_sParamName);
	str_arr.Add(ss);
	if(m_iDataType==0)
	{
		ss.Format("%s", m_sParamValue);
		str_arr.Add(ss);
#ifdef AFX_TARG_ENU_ENGLISH
		str_arr.Add("Real Type");
#else
		str_arr.Add("实数类型");
#endif
	}
	else if(m_iDataType==1)
	{
		ss.Format("%s", m_sParamValue);
		str_arr.Add(ss);
#ifdef AFX_TARG_ENU_ENGLISH
		str_arr.Add("Handle Type");
#else
		str_arr.Add("句柄类型");
#endif
	}
	ss.Format("%s", m_sParamNotes);
	str_arr.Add(ss);
	int iItem = AppendRecord(-1,str_arr);
	m_pListCtrl->SetItemData(iItem,1);	//表示该项已修改过
}

void CPlateDesignParamDlg::OnColumnclickListPlateDesignParam(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	listViewComparer.m_pListCtrl = m_pListCtrl;
	listViewComparer.m_iColumn = pNMListView->iSubItem;
	listViewComparer.sortOrderAscending[pNMListView->iSubItem]=!listViewComparer.sortOrderAscending[pNMListView->iSubItem];
	m_pListCtrl->SortItems(CompareItemProc,(DWORD)&listViewComparer);
	*pResult = 0;
}
#endif
// CutPlaneSetDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "CutPlaneSetDlg.h"
#include "DefPlaneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCutPlaneSetDlg dialog


CCutPlaneSetDlg::CCutPlaneSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCutPlaneSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCutPlaneSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_arrHeaderLabel.Add("Datum point X coordinate");
	m_arrHeaderLabel.Add("Datum point Y coordinate");
	m_arrHeaderLabel.Add("Datum point Z coordinate");
	m_arrHeaderLabel.Add("Nomal X coordinate");
	m_arrHeaderLabel.Add("Nomal Y coordinate");
	m_arrHeaderLabel.Add("Nomal Z coordinate");
#else 
	m_arrHeaderLabel.Add("基点X坐标");
	m_arrHeaderLabel.Add("基点Y坐标");
	m_arrHeaderLabel.Add("基点Z坐标");
	m_arrHeaderLabel.Add("法线X坐标");
	m_arrHeaderLabel.Add("法线Y坐标");
	m_arrHeaderLabel.Add("法线Z坐标");
#endif
}


void CCutPlaneSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCutPlaneSetDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCutPlaneSetDlg, CDialog)
	//{{AFX_MSG_MAP(CCutPlaneSetDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CUT_PLANE, OnDblclkListCutPlane)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CUT_PLANE, OnKeydownListCutPlane)
	ON_BN_CLICKED(IDC_BN_ADD_ITEM, OnBnAddItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCutPlaneSetDlg message handlers

BOOL CCutPlaneSetDlg::OnInitDialog() 
{
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_CUT_PLANE);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	return CDialog::OnInitDialog();
}

void CCutPlaneSetDlg::RefreshListViewHeader()
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
void CCutPlaneSetDlg::AppendRecord(int iItem, CStringArray &str_arr)
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

void CCutPlaneSetDlg::DeleteItem(int nCurSel) 
{
	m_pListCtrl->DeleteItem(nCurSel);
}

void CCutPlaneSetDlg::ModifyItem(int nCurSel) 
{
	CDefPlaneDlg dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	dlg.m_sDlgCaption = "Modified Tube Cut Cylinder";
#else 
	dlg.m_sDlgCaption = "修改钢管切割面";
#endif
	CString sText;
	char tem_str[101];
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	dlg.m_fBaseX = atof(tem_str);	
	m_pListCtrl->GetItemText(nCurSel,1,tem_str,100);
	dlg.m_fBaseY = atof(tem_str);		
	m_pListCtrl->GetItemText(nCurSel,2,tem_str,100);
	dlg.m_fBaseZ  = atof(tem_str);	
	m_pListCtrl->GetItemText(nCurSel,3,tem_str,100);
	dlg.m_fNormX = atof(tem_str);	
	m_pListCtrl->GetItemText(nCurSel,4,tem_str,100);
	dlg.m_fNormY = atof(tem_str);		
	m_pListCtrl->GetItemText(nCurSel,5,tem_str,100);
	dlg.m_fNormZ  = atof(tem_str);
	if(dlg.DoModal()==IDOK)
	{
		sText.Format("%.f",dlg.m_fBaseX);
		m_pListCtrl->SetItemText(nCurSel,0,sText);
		sText.Format("%.f",dlg.m_fBaseY);
		m_pListCtrl->SetItemText(nCurSel,1,sText);
		sText.Format("%.f",dlg.m_fBaseZ);
		m_pListCtrl->SetItemText(nCurSel,2,sText);
		sText.Format("%.f",dlg.m_fNormX);
		m_pListCtrl->SetItemText(nCurSel,3,sText);
		sText.Format("%.f",dlg.m_fNormY);
		m_pListCtrl->SetItemText(nCurSel,4,sText);
		sText.Format("%.f",dlg.m_fNormZ);
		m_pListCtrl->SetItemText(nCurSel,5,sText);
		m_pListCtrl->RedrawItems(nCurSel,nCurSel);
	}
}

void CCutPlaneSetDlg::OnDblclkListCutPlane(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(iCurSel>=0)
			ModifyItem(iCurSel);
	}
	
	*pResult = 0;
}

void CCutPlaneSetDlg::OnKeydownListCutPlane(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	POSITION pos = m_pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = m_pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_F3)
		{
			if(iCurSel>=0)
				ModifyItem(iCurSel);
		}
		else if(pLVKeyDow->wVKey==VK_DELETE)
		{
			if(iCurSel>=0)
				DeleteItem(iCurSel);
		}
	}
	*pResult = 0;
}

void CCutPlaneSetDlg::OnBnAddItem()
{
	CDefPlaneDlg dlg;
#ifdef AFX_TARG_ENU_ENGLISH
	dlg.m_sDlgCaption = "Define Tube Cut Cylinder";
#else 
	dlg.m_sDlgCaption = "定义钢管切割面";
#endif
	CString TemStr;
	CStringArray str_arr;
	if(dlg.DoModal()==IDOK)
	{
		TemStr.Format("%.f",dlg.m_fBaseX);
		str_arr.Add(TemStr);
		TemStr.Format("%.f",dlg.m_fBaseY);
		str_arr.Add(TemStr);
		TemStr.Format("%.f",dlg.m_fBaseZ);
		str_arr.Add(TemStr);
		TemStr.Format("%.f",dlg.m_fNormX);
		str_arr.Add(TemStr);
		TemStr.Format("%.f",dlg.m_fNormY);
		str_arr.Add(TemStr);
		TemStr.Format("%.f",dlg.m_fNormZ);
		str_arr.Add(TemStr);
	}
	if(m_pListCtrl->GetItemCount()>100)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Applications can only process data within 100");
#else 
		AfxMessageBox("程序只能处理100个以内的数据");
#endif
		return;
	}
	AppendRecord(-1,str_arr);
	Invalidate();
}

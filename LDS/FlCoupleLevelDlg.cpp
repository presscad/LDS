// FlCoupleLevelDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "FlCoupleLevelDlg.h"
#include "Query.h"
#include "database.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlCoupleLevelDlg dialog


CFlCoupleLevelDlg::CFlCoupleLevelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlCoupleLevelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlCoupleLevelDlg)
	m_fMainTubeD = 219;
	m_fParamA = 219;
	m_fParamB = 273;
	m_fParamC = 325;
	m_fParamD = 375;
	//}}AFX_DATA_INIT
}


void CFlCoupleLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlCoupleLevelDlg)
	DDX_Text(pDX, IDC_E_PARAM_A, m_fParamA);
	DDX_Text(pDX, IDC_E_PARAM_B, m_fParamB);
	DDX_Text(pDX, IDC_E_PARAM_C, m_fParamC);
	DDX_Text(pDX, IDC_E_PARAM_D, m_fParamD);
	DDX_Text(pDX, IDC_E_MAIN_TUBE_D, m_fMainTubeD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlCoupleLevelDlg, CDialog)
	//{{AFX_MSG_MAP(CFlCoupleLevelDlg)
	ON_BN_CLICKED(IDC_BN_IMPORT_LIB_FILE, OnImportLibFile)
	ON_BN_CLICKED(IDC_BN_EXPORT_LIB_FILE, OnExportLibFile)
	ON_BN_CLICKED(IDC_BN_ADD_ITEM, OnAddItem)
	ON_BN_CLICKED(IDC_BN_MODIFY_ITEM, OnModifyItem)
	ON_NOTIFY(NM_DBLCLK, IDC_FL_COUPLE_LEVEL_LIST, OnDblclkFlCoupleLevelList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_FL_COUPLE_LEVEL_LIST, OnKeydownFlCoupleLevelList)
	ON_EN_CHANGE(IDC_E_PARAM_A, OnChangeParamA)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlCoupleLevelDlg message handlers

BOOL CFlCoupleLevelDlg::OnInitDialog() 
{
	RefreshListViewHeader();
	RefreshListView();
	return CDialog::OnInitDialog();  
}

void CFlCoupleLevelDlg::RefreshListView()
{
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_FL_COUPLE_LEVEL_LIST);
	pListCtrl->DeleteAllItems();
	for(int i=0;i<fl_couple_rule_N;i++)
	{
		CStringArray str_arr;
		str_arr.SetSize(5);
		str_arr[0].Format("%.f",fl_couple_rule_table[i].d);
		str_arr[1].Format("%.f",fl_couple_rule_table[i].A);
		str_arr[2].Format("%.f",fl_couple_rule_table[i].B);
		str_arr[3].Format("%.f",fl_couple_rule_table[i].C);
		str_arr[4].Format("%.f",fl_couple_rule_table[i].D);
		AppendRecord(pListCtrl,-1,str_arr);
	}	
}

int CFlCoupleLevelDlg::AppendRecord(CListCtrl *pListCtrl, int iItem, CStringArray &str_arr)
{
	// insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=pListCtrl->GetItemCount();
	
	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = pListCtrl->InsertItem(&lvi);
	// set item text for additional columns
	int nCount = str_arr.GetSize();
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		pListCtrl->SetItemText(iItem,j,str_arr[j]);
	}
	pListCtrl->RedrawItems(iItem,iItem);
	return iItem;
}

void CFlCoupleLevelDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i,nColumnCount;
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_FL_COUPLE_LEVEL_LIST);
	pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES| LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	CHeaderCtrl* pHeader = pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();
	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	   pListCtrl->DeleteColumn(0);
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
#ifdef AFX_TARG_ENU_ENGLISH
	lvc.pszText = "Main Tube Diameter d";
#else 
	lvc.pszText = "主管直径d";//_gszColumnLabel[i];
#endif//_gszColumnLabel[i];
	lvc.cx = 80;
	lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
	pListCtrl->InsertColumn(0,&lvc);
	lvc.iSubItem = 1;
	lvc.pszText = "A";//_gszColumnLabel[i];
	lvc.cx = 58;
	pListCtrl->InsertColumn(1,&lvc);
	lvc.iSubItem = 2;
	lvc.pszText = "B";//_gszColumnLabel[i];
	lvc.cx = 58;
	pListCtrl->InsertColumn(2,&lvc);
	lvc.iSubItem = 3;
	lvc.pszText = "C";//_gszColumnLabel[i];
	lvc.cx = 58;
	pListCtrl->InsertColumn(3,&lvc);
	lvc.iSubItem = 4;
	lvc.pszText = "D";//_gszColumnLabel[i];
	lvc.cx = 58;
	pListCtrl->InsertColumn(4,&lvc);
}

void CFlCoupleLevelDlg::OnImportLibFile() 
{
	CString sFileName;
	char *sName="fll";
#ifdef AFX_TARG_ENU_ENGLISH
	char *sFileType="Flange Dock Grade(*.fll)|*.fll||";
#else 
	char *sFileType="法兰对接级别(*.fll)|*.fll||";
#endif
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlCoupleLevelSerialize(sFileName,FALSE);
		//导入以后需要刷新列表内容 
		RefreshListView();
	}
}

void CFlCoupleLevelDlg::OnExportLibFile() 
{
	CString sFileName;
	char *sName="fll";
#ifdef AFX_TARG_ENU_ENGLISH
	char *sFileType="Flange Dock Grade(*.fll)|*.fll||";
#else 
	char *sFileType="法兰对接级别(*.fll)|*.fll||";
#endif
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		FlCoupleLevelSerialize(sFileName,TRUE);
	}
}


void CFlCoupleLevelDlg::OnAddItem()
{
	UpdateData();
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_FL_COUPLE_LEVEL_LIST);
	CStringArray str_arr;
	str_arr.SetSize(5);
	str_arr[0].Format("%.f",m_fMainTubeD);
	str_arr[1].Format("%.f",m_fParamA);
	str_arr[2].Format("%.f",m_fParamB);
	str_arr[3].Format("%.f",m_fParamC);
	str_arr[4].Format("%.f",m_fParamD);
	AppendRecord(pListCtrl,-1,str_arr);
}

void CFlCoupleLevelDlg::OnModifyItem()
{
	UpdateData();
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_FL_COUPLE_LEVEL_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		char ss[MAX_TEMP_CHAR_100+1];
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fMainTubeD);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,0,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fParamA);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,1,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fParamB);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,2,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fParamC);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,3,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fParamD);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,4,ss);
	}
}

void CFlCoupleLevelDlg::OnDblclkFlCoupleLevelList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_FL_COUPLE_LEVEL_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		char sText[50];
		pListCtrl->GetItemText(iCurSel,0,sText,50);
		m_fMainTubeD=atof(sText);
		pListCtrl->GetItemText(iCurSel,1,sText,50);
		m_fParamA=atof(sText);
		pListCtrl->GetItemText(iCurSel,2,sText,50);
		m_fParamB=atof(sText);
		pListCtrl->GetItemText(iCurSel,3,sText,50);
		m_fParamC=atof(sText);
		pListCtrl->GetItemText(iCurSel,4,sText,50);
		m_fParamD=atof(sText);
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CFlCoupleLevelDlg::DeleteItem(CListCtrl *pListCtrl,int nCurSel) 
{
	pListCtrl->DeleteItem(nCurSel);
}

void CFlCoupleLevelDlg::OnKeydownFlCoupleLevelList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_FL_COUPLE_LEVEL_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_DELETE)
		{
			DeleteItem(pListCtrl,iCurSel);
			pos = pListCtrl->GetFirstSelectedItemPosition();
		}
		else
			break;
	}
	*pResult = 0;
}

void CFlCoupleLevelDlg::OnOK()
{
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_FL_COUPLE_LEVEL_LIST);
	int i,nCount=pListCtrl->GetItemCount();
	if(nCount>200)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(MessageBox("System can only handle 200 records at the same time.","Flange Dock Grade Definition",MB_YESNO)==IDYES)
#else 
		if(MessageBox("系统只能同时处理200条记录","法兰对接级别定义",MB_YESNO)==IDYES)
#endif
			nCount=200;
		else
			return;
	}
	fl_couple_rule_N=nCount;
	for(i=0;i<nCount;i++)
	{
		char sText[50];
		pListCtrl->GetItemText(i,0,sText,50);
		fl_couple_rule_table[i].d=atof(sText);
		pListCtrl->GetItemText(i,1,sText,50);
		fl_couple_rule_table[i].A=atof(sText);
		pListCtrl->GetItemText(i,2,sText,50);
		fl_couple_rule_table[i].B=atof(sText);
		pListCtrl->GetItemText(i,3,sText,50);
		fl_couple_rule_table[i].C=atof(sText);
		pListCtrl->GetItemText(i,4,sText,50);
		fl_couple_rule_table[i].D=atof(sText);
	}
	CDialog::OnOK();
}

void CFlCoupleLevelDlg::OnChangeParamA()
{
	UpdateData();
	m_fMainTubeD=m_fParamA;
	UpdateData(FALSE);
}
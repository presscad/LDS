// StdPartFlTableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "StdPartFlTableDlg.h"
#include "Query.h"
#include "database.h"
#include "GlobalFunc.h"
#include "FlCoupleLevelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStdPartFlTableDlg dialog


CStdPartFlTableDlg::CStdPartFlTableDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStdPartFlTableDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStdPartFlTableDlg)
	m_fFlParaA = 221;
	m_fFlParaB = 205;
	m_fFlParaK = 300;
	m_fFlParaD = 350;
	m_fFlParaN = 245;
	m_fFlParaS = 8;
	m_fFlParaR = 10;
	m_fFlParaC = 20;
	m_fFlParaH = 62;
	m_fFlParaH1= 18;
	m_fFlParaL = 26;
	m_fWeight = 0;	
	m_sBoltD = _T("20");
	m_sFlName = _T("FLD2121");
	m_sFlLevel = _T("A");
	m_sMainTubeGuiGe = _T("219");
	m_iPartType = 0;
	m_iFlMaterial = 1;
	m_iFlBoltNum = 8;
	m_fFlParaSIncrement = 2.0;
	//}}AFX_DATA_INIT
	m_bModify=FALSE;
}


void CStdPartFlTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStdPartFlTableDlg)
	DDX_Text(pDX, IDC_E_FL_PARA_A, m_fFlParaA);
	DDX_Text(pDX, IDC_E_FL_PARA_B, m_fFlParaB);
	DDX_Text(pDX, IDC_E_FL_PARA_K, m_fFlParaK);
	DDX_Text(pDX, IDC_E_FL_PARA_D, m_fFlParaD);
	DDX_Text(pDX, IDC_E_FL_PARA_N, m_fFlParaN);
	DDX_Text(pDX, IDC_E_FL_PARA_S, m_fFlParaS);
	DDX_Text(pDX, IDC_E_FL_PARA_R, m_fFlParaR);
	DDX_Text(pDX, IDC_E_FL_PARA_C, m_fFlParaC);
	DDX_Text(pDX, IDC_E_FL_PARA_H, m_fFlParaH);
	DDX_Text(pDX, IDC_E_FL_PARA_H1, m_fFlParaH1);
	DDX_Text(pDX, IDC_E_FL_PARA_L, m_fFlParaL);
	DDX_Text(pDX, IDC_E_WEIGHT, m_fWeight);
	DDX_CBString(pDX, IDC_CMB_BOLT_D, m_sBoltD);
	DDX_Text(pDX, IDC_E_FL_NAME, m_sFlName);
	DDX_CBString(pDX, IDC_CMB_FL_LEVEL, m_sFlLevel);
	DDX_CBString(pDX, IDC_CMB_MAIN_TUBE_GUIGE, m_sMainTubeGuiGe);
	DDX_CBIndex(pDX, IDC_CMB_PART_TYPE, m_iPartType);
	DDX_CBIndex(pDX, IDC_CMB_FL_MATERIAL, m_iFlMaterial);
	DDX_Text(pDX, IDC_E_FL_BOLT_NUM, m_iFlBoltNum);
	DDX_Text(pDX, IDC_E_FL_PARA_S_INCREMENT, m_fFlParaSIncrement);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStdPartFlTableDlg, CDialog)
	//{{AFX_MSG_MAP(CStdPartFlTableDlg)
	ON_BN_CLICKED(IDC_BN_IMPORT_FL_PARAM_LIB_FILE, OnImportFlParamLibFile)
	ON_BN_CLICKED(IDC_BN_EXPORT_FL_PARAM_LIB_FILE, OnExportFlParamLibFile)
	ON_BN_CLICKED(IDC_BN_FL_COUPLE_LEVEL, OnFlCoupleLevel)
	ON_BN_CLICKED(IDC_BN_ADD_ITEM, OnAddItem)
	ON_BN_CLICKED(IDC_BN_MODIFY_ITEM, OnModifyItem)
	ON_CBN_SELCHANGE(IDC_CMB_PART_TYPE, OnSelchangePartType)
	ON_CBN_SELCHANGE(IDC_CMB_FL_LEVEL, OnSelchangeFlLevel)
	ON_CBN_SELCHANGE(IDC_CMB_MAIN_TUBE_GUIGE, OnSelchangeMainTubeGuige)
	ON_NOTIFY(NM_DBLCLK, IDC_STD_PART_FL_LIB_LIST, OnDblclkStdPartFlLibList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_STD_PART_FL_LIB_LIST, OnKeydownStdPartFlLibList)
	ON_CBN_KILLFOCUS(IDC_CMB_MAIN_TUBE_GUIGE, OnKillfocusMainTubeGuige)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStdPartFlTableDlg message handlers

BOOL CStdPartFlTableDlg::OnInitDialog() 
{
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_FL_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iFlMaterial);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MAIN_TUBE_GUIGE);
	AddTubeRecord(pCMB);
	pCMB->SetCurSel(0);
	RefreshListViewHeader();
	RefreshListView();
	return CDialog::OnInitDialog();  
}

void CStdPartFlTableDlg::RefreshListView()
{
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_STD_PART_FL_LIB_LIST);
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PART_TYPE);
	m_CurPlateBmp.Detach();
	pListCtrl->DeleteAllItems();
	CComboBox *pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MAIN_TUBE_GUIGE);
	while(pCMB->GetCount()>0)
		pCMB->DeleteString(0);
	if(m_iPartType==0)
	{	//对焊法兰
		m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLD);
		GetDlgItem(IDC_E_FL_PARA_S)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_FL_PARA_S_INCREMENT)->EnableWindow(TRUE);
		AddTubeRecord(pCMB,TRUE);
		pCMB->SetCurSel(0);
		for(int i=0;i<fld_param_N;i++)
		{
			CStringArray str_arr;
			str_arr.SetSize(19);
			int t=ftol(fld_param_table[i].t);
			str_arr[0].Format("%s",fld_param_table[i].codeName);
			str_arr[1].Format("%.f",fld_param_table[i].d);
			str_arr[2].Format("t");
			str_arr[3].Format("%c",fld_param_table[i].cMaterial);
			str_arr[4].Format("%c",fld_param_table[i].cLevelMark);
			str_arr[5].Format("%.f",fld_param_table[i].A);
			str_arr[6].Format("%.f",fld_param_table[i].B);
			str_arr[7].Format("%.f",fld_param_table[i].D);
			str_arr[8].Format("%.f",fld_param_table[i].K);
			str_arr[9].Format("%.f",fld_param_table[i].L);
			str_arr[10].Format("%d",fld_param_table[i].nBoltNum);
			str_arr[11].Format("%d",fld_param_table[i].nBoltD);
			str_arr[12].Format("%.f",fld_param_table[i].C);
			str_arr[13].Format("%.f",fld_param_table[i].H);
			str_arr[14].Format("%.f",fld_param_table[i].N);
			if(t<=0)	//新版本中参数S中保存的为S与t之间的差值
				str_arr[15].Format("t+%.f",fld_param_table[i].S);
			else		//旧版本中S中保存S的实际数值
				str_arr[15].Format("t+%.f",fld_param_table[i].S-fld_param_table[i].t);
			str_arr[16].Format("%.f",fld_param_table[i].R);
			str_arr[17].Format("%.f",fld_param_table[i].H1);
			str_arr[18].Format("%.2f",fld_param_table[i].group[0].weight);//theroy_weight);	未完全考虑升级为若干壁厚组情况 wjh 2011.11.21
			AppendRecord(pListCtrl,-1,str_arr);
		}
	}
	else if(m_iPartType==1)
	{	//平焊法兰
		m_CurPlateBmp.LoadBitmap(IDB_BMP_STD_PART_FLP);
		GetDlgItem(IDC_E_FL_PARA_S)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_FL_PARA_S_INCREMENT)->EnableWindow(FALSE);
		AddTubeRecord(pCMB);
		for(int i=0;i<flp_param_N;i++)
		{
			CStringArray str_arr;
			str_arr.SetSize(19);
			str_arr[0].Format("%s",flp_param_table[i].codeName);
			str_arr[1].Format("%.f",flp_param_table[i].d);
			str_arr[2].Format("%.f",flp_param_table[i].t);
			str_arr[3].Format("%c",flp_param_table[i].cMaterial);
			str_arr[4].Format("%c",flp_param_table[i].cLevelMark);
			str_arr[5].Format("%.f",flp_param_table[i].A);
			str_arr[6].Format("%.f",flp_param_table[i].B);
			str_arr[7].Format("%.f",flp_param_table[i].D);
			str_arr[8].Format("%.f",flp_param_table[i].K);
			str_arr[9].Format("%.f",flp_param_table[i].L);
			str_arr[10].Format("%d",flp_param_table[i].nBoltNum);
			str_arr[11].Format("%d",flp_param_table[i].nBoltD);
			str_arr[12].Format("%.f",flp_param_table[i].C);
			str_arr[13].Format("%.f",flp_param_table[i].H);
			str_arr[14].Format("%.f",flp_param_table[i].N);
			str_arr[15].Format("%.f",flp_param_table[i].S);
			str_arr[16].Format("%.f",flp_param_table[i].R);
			str_arr[17].Format("%.f",flp_param_table[i].H1);
			str_arr[18].Format("%.2f",flp_param_table[i].group[0].weight);
			AppendRecord(pListCtrl,-1,str_arr);
		}
	}
	pCMB->SetCurSel(0);
	pPic->SetBitmap(m_CurPlateBmp);
}

int CStdPartFlTableDlg::AppendRecord(CListCtrl *pListCtrl, int iItem, CStringArray &str_arr)
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

void CStdPartFlTableDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i,nColumnCount;
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_STD_PART_FL_LIB_LIST);
	pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES| LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	CHeaderCtrl* pHeader = pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();
	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	   pListCtrl->DeleteColumn(0);
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = "法兰代号";//_gszColumnLabel[i];
	lvc.cx = 70;
	lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
	pListCtrl->InsertColumn(0,&lvc);
	lvc.iSubItem = 1;
	lvc.pszText = "管径";//_gszColumnLabel[i];
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
	pListCtrl->InsertColumn(1,&lvc);
	lvc.iSubItem = 2;
	lvc.pszText = "管厚";//_gszColumnLabel[i];
	lvc.cx = 50;
	pListCtrl->InsertColumn(2,&lvc);
	lvc.iSubItem = 3;
	lvc.pszText = "材质";//_gszColumnLabel[i];
	lvc.cx = 50;
	pListCtrl->InsertColumn(3,&lvc);
	lvc.iSubItem = 4;
	lvc.pszText = "级别";//_gszColumnLabel[i];
	lvc.cx = 50;
	pListCtrl->InsertColumn(4,&lvc);
	lvc.iSubItem = 5;
	lvc.pszText = "A";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(5,&lvc);
	lvc.iSubItem = 6;
	lvc.pszText = "B";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(6,&lvc);
	lvc.iSubItem = 7;
	lvc.pszText = "D";//_gszColumnLabel[i];
	lvc.cx = 40;
	pListCtrl->InsertColumn(7,&lvc);
	lvc.iSubItem = 8;
	lvc.pszText = "K";//_gszColumnLabel[i];
	lvc.cx = 40;
	pListCtrl->InsertColumn(8,&lvc);
	lvc.iSubItem = 9;
	lvc.pszText = "L";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(9,&lvc);
	lvc.iSubItem = 10;
	lvc.pszText = "n";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(10,&lvc);
	lvc.iSubItem = 11;
	lvc.pszText = "M";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(11,&lvc);
	lvc.iSubItem = 12;
	lvc.pszText = "C";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(12,&lvc);
	lvc.iSubItem = 13;
	lvc.pszText = "H";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(13,&lvc);
	lvc.iSubItem = 14;
	lvc.pszText = "N";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(14,&lvc);
	lvc.iSubItem = 15;
	lvc.pszText = "S";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(15,&lvc);
	lvc.iSubItem = 16;
	lvc.pszText = "R";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(16,&lvc);
	lvc.iSubItem = 17;
	lvc.pszText = "H1";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(17,&lvc);
	lvc.iSubItem = 18;
	lvc.pszText = "理论重量";//_gszColumnLabel[i];
	lvc.cx = 80;
	pListCtrl->InsertColumn(18,&lvc);
}

void CStdPartFlTableDlg::OnImportFlParamLibFile() 
{
	CString sFileName;
	char *sName="fld";
	char *sFileType="对焊法兰(*.fld)|*.fld||";
	if(m_iPartType==1)
	{
		sName="flp";
		sFileType="平焊法兰规格库文件(*.flp)|*.flp||";
	}
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		if(m_iPartType==0)
			FlDSerialize(sFileName,FALSE);
		else if(m_iPartType==1)
			FlPSerialize(sFileName,FALSE);
		//导入以后需要刷新列表内容 
		RefreshListView();
	}
}

void CStdPartFlTableDlg::OnExportFlParamLibFile() 
{
	CString sFileName;
	char *sName="fld";
	char *sFileType="对焊法兰库文件(*.fld)|*.fld||";
	if(m_iPartType==1)
	{
		sName="flp";
		sFileType="平焊法兰规格库文件(*.flp)|*.flp||";
	}
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		DoSave();	//保存数据
		sFileName = dlg.GetPathName();
		if(m_iPartType==0)
			FlDSerialize(sFileName,TRUE);
		else if(m_iPartType==1)
			FlPSerialize(sFileName,TRUE);
	}
}


void CStdPartFlTableDlg::OnAddItem()
{
	UpdateData();
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_STD_PART_FL_LIB_LIST);
	CStringArray str_arr;
	double d,t;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	GetDlgItem(IDC_CMB_MAIN_TUBE_GUIGE)->GetWindowText(m_sMainTubeGuiGe);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",m_sMainTubeGuiGe);
	if(m_iPartType==0)
		restore_JG_guige(tem_str,d,t);
	else 
		d=atof(tem_str);
	str_arr.SetSize(19);
	str_arr[0].Format("%s",m_sFlName);
	str_arr[1].Format("%.f",d);
	if(m_iPartType==0)	//对焊法兰
		str_arr[2].Format("t");
	else				//平焊法兰
		str_arr[2].Format("%d",(int)t);
	CString sMaterial,ss;
	GetDlgItem(IDC_CMB_FL_MATERIAL)->GetWindowText(sMaterial);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sMaterial);
	char cMat=QuerySteelBriefMatMark(tem_str);
	str_arr[3].Format("%c",cMat);
	GetDlgItem(IDC_CMB_FL_LEVEL)->GetWindowText(sMaterial);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sMaterial);
	char cMark=tem_str[0];
	str_arr[4].Format("%c",tem_str[0]);
	str_arr[5].Format("%.f",m_fFlParaA);
	str_arr[6].Format("%.f",m_fFlParaB);
	str_arr[7].Format("%.f",m_fFlParaD);
	str_arr[8].Format("%.f",m_fFlParaK);
	str_arr[9].Format("%.f",m_fFlParaL);
	str_arr[10].Format("%d",m_iFlBoltNum);
	sprintf(tem_str,"%s",m_sBoltD);
	str_arr[11].Format("%d",atoi(tem_str));
	str_arr[12].Format("%.f",m_fFlParaC);
	str_arr[13].Format("%.f",m_fFlParaH);
	str_arr[14].Format("%.f",m_fFlParaN);
	if(m_iPartType==0)	//对焊法兰	S=t+增量
		str_arr[15].Format("t+%.f",m_fFlParaSIncrement);
	else				//平焊法兰
		str_arr[15].Format("%.f",m_fFlParaS);
	str_arr[16].Format("%.f",m_fFlParaR);
	str_arr[17].Format("%.f",m_fFlParaH1);
	str_arr[18].Format("%.2f",m_fWeight);
	int i,nCount=pListCtrl->GetItemCount();
	if(nCount>200)
	{
		if(MessageBox("系统只能同时处理200种记录","法兰规格库",MB_YESNO)==IDYES)
			return;
	}
	for(i=0;i<nCount;i++)
	{
		CString ss;
		double tube_d=0, tube_t=0;
		char sText[50],s_t[50],sName[50]="";
		pListCtrl->GetItemText(i,0,sName,50);
		pListCtrl->GetItemText(i,1,sText,50);
		pListCtrl->GetItemText(i,2,s_t,50);
		tube_d=atof(sText);
		tube_t=atof(s_t);
		pListCtrl->GetItemText(i,3,sText,50);
		char cMat1=sText[0];
		fld_param_table[i].cMaterial=cMat;
		pListCtrl->GetItemText(i,4,sText,50);
		char cMark1=sText[0];
		if(m_sFlName.CompareNoCase(sName)==0)
		{
			MessageBox("已存在该名称法兰,不能重复添加!");
			return;
		}
		else if(m_iPartType==0&&tube_d==d&&cMat==cMat1&&cMark==cMark1)
		{	//对焊法兰
			MessageBox("法兰规格库中已存在此规格法兰,不能重复添加!");
			return;
		}
		else if(m_iPartType==0&&tube_d==d&&tube_t==t&&cMat==cMat1&&cMark==cMark1)
		{	//平焊法兰
			MessageBox("法兰规格库中已存在此规格法兰,不能重复添加!");
			return;
		}
	}
	m_bModify=TRUE;	//设置修改标志位为TRUE
	AppendRecord(pListCtrl,-1,str_arr);
}

void CStdPartFlTableDlg::OnModifyItem()
{
	UpdateData();
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_STD_PART_FL_LIB_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		UpdateData();
		double d,t;
		char ss[MAX_TEMP_CHAR_100+1]="";
		sprintf(ss,"%s",m_sFlName);
		pListCtrl->SetItemText(iCurSel,0,ss);
		GetDlgItem(IDC_CMB_MAIN_TUBE_GUIGE)->GetWindowText(m_sMainTubeGuiGe);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%s",m_sMainTubeGuiGe);
		if(m_iPartType==0)
			restore_JG_guige(ss,d,t);
		else 
			d=atof(ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",d);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,1,ss);
		if(m_iPartType==1)
		{	//平焊法兰
			_snprintf(ss,MAX_TEMP_CHAR_100,"%d",(int)t);
			SimplifiedNumString(ss);
		}
		else
			strcpy(ss,"t");
		pListCtrl->SetItemText(iCurSel,2,ss);
		CString sMaterial;
		GetDlgItem(IDC_CMB_FL_MATERIAL)->GetWindowText(sMaterial);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%s",sMaterial);
		char cMat=QuerySteelBriefMatMark(ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%c",cMat);
		pListCtrl->SetItemText(iCurSel,3,ss);
		GetDlgItem(IDC_CMB_FL_LEVEL)->GetWindowText(sMaterial);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%s",sMaterial);
		pListCtrl->SetItemText(iCurSel,4,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaA);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,5,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaB);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,6,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaD);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,7,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaK);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,8,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaL);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,9,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%d",m_iFlBoltNum);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,10,ss);
		CString sBoltD;
		_snprintf(ss,MAX_TEMP_CHAR_100,"%s",m_sBoltD);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,11,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaC);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,12,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaH);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,13,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaN);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,14,ss);
		if(m_iPartType==0)	//对焊法兰
			_snprintf(ss,MAX_TEMP_CHAR_100,"t+%.f",m_fFlParaSIncrement);
		else				//平焊法兰
		{
			_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaS);
			SimplifiedNumString(ss);
		}
		pListCtrl->SetItemText(iCurSel,15,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaR);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,16,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fFlParaH1);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,17,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.2f",m_fWeight);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,18,ss);
		m_bModify=TRUE;	//设置修改标志位为TRUE
	}
}

void CStdPartFlTableDlg::OnDblclkStdPartFlLibList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_STD_PART_FL_LIB_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		CString ss;
		double d=0, t=0;
		char sText[50],s_t[50];
		pListCtrl->GetItemText(iCurSel,0,sText,50);
		m_sFlName.Format("%s",sText);
		pListCtrl->GetItemText(iCurSel,1,sText,50);
		d=atof(sText);
		pListCtrl->GetItemText(iCurSel,2,s_t,50);
		t=atof(s_t);
		if(m_iPartType==0)	//对焊法兰
			m_sMainTubeGuiGe.Format("%s",sText);
		else				//平焊法兰
			m_sMainTubeGuiGe.Format("%sX%s",sText,s_t);
		pListCtrl->GetItemText(iCurSel,3,sText,50);
		char cMat=sText[0];
		m_iFlMaterial=QuerySteelMatIndex(cMat);	//材质
		pListCtrl->GetItemText(iCurSel,4,sText,50);
		m_sFlLevel.Format("%s",sText);			//级别
		pListCtrl->GetItemText(iCurSel,5,sText,50);
		m_fFlParaA=atof(sText);
		pListCtrl->GetItemText(iCurSel,6,sText,50);
		m_fFlParaB=atof(sText);
		pListCtrl->GetItemText(iCurSel,7,sText,50);
		m_fFlParaD=atof(sText);
		pListCtrl->GetItemText(iCurSel,8,sText,50);
		m_fFlParaK=atof(sText);
		pListCtrl->GetItemText(iCurSel,9,sText,50);
		m_fFlParaL=atof(sText);
		pListCtrl->GetItemText(iCurSel,10,sText,50);
		m_iFlBoltNum=atoi(sText);
		pListCtrl->GetItemText(iCurSel,11,sText,50);
		m_sBoltD.Format("%s",sText);
		pListCtrl->GetItemText(iCurSel,12,sText,50);
		m_fFlParaC=atof(sText);
		pListCtrl->GetItemText(iCurSel,13,sText,50);
		m_fFlParaH=atof(sText);
		pListCtrl->GetItemText(iCurSel,14,sText,50);
		m_fFlParaN=atof(sText);
		pListCtrl->GetItemText(iCurSel,15,sText,50);
		if(m_iPartType==0)	//对焊法兰
		{	//wht 10-01-20
			if(t>0)
			{	//兼容以前的旧法兰库 计算S与t之间的差值
				m_fFlParaS=atof(sText);
				m_fFlParaSIncrement=m_fFlParaS-t;
			}
			else
			{	//新对焊法兰库
				strncpy(s_t,sText+2,20);
				m_fFlParaSIncrement=atof(s_t);
			}
		}
		else				//平焊法兰
			m_fFlParaS=atof(sText);
		pListCtrl->GetItemText(iCurSel,16,sText,50);
		m_fFlParaR=atof(sText);
		pListCtrl->GetItemText(iCurSel,17,sText,50);
		m_fFlParaH1=atof(sText);
		pListCtrl->GetItemText(iCurSel,18,sText,50);
		m_fWeight=atof(sText);
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CStdPartFlTableDlg::DeleteItem(CListCtrl *pListCtrl,int nCurSel) 
{
	pListCtrl->DeleteItem(nCurSel);
}

void CStdPartFlTableDlg::OnKeydownStdPartFlLibList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_STD_PART_FL_LIB_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		if(pLVKeyDow->wVKey==VK_DELETE)
		{
			DeleteItem(pListCtrl,iCurSel);
			pos = pListCtrl->GetFirstSelectedItemPosition();
			m_bModify=TRUE;	//设置修改标志位为TRUE
		}
		else
			break;
	}
	*pResult = 0;
}

void CStdPartFlTableDlg::OnSelchangePartType()
{
	//如果数据已修改过 询问用户是否保存
	if(m_bModify)
	{	
		if(AfxMessageBox("是否保存修改过的内容?",MB_YESNO)==IDYES)
			DoSave();
		m_bModify=FALSE;	//重置数据修改标志位
	}
	UpdateData();
	RefreshListView();
	UpdateFlName();
	UpdateData(FALSE);
}

BOOL CStdPartFlTableDlg::DoSave()
{	//保存数据
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_STD_PART_FL_LIB_LIST);
	int i,nCount=pListCtrl->GetItemCount();
	if(nCount>200)
	{
		if(MessageBox("系统只能同时处理200种记录","法兰规格库",MB_YESNO)==IDYES)
			nCount=200;
		else
			return FALSE;
	}
	if(m_iPartType==0)
	{	//对焊法兰
		fld_param_N=nCount;
		for(i=0;i<nCount;i++)
		{
			CString ss;
			double d=0, t=0;
			char sText[50],s_t[50];
			pListCtrl->GetItemText(i,0,fld_param_table[i].codeName,16);
			pListCtrl->GetItemText(i,1,sText,50);
			pListCtrl->GetItemText(i,2,s_t,50);
			d=atof(sText);
			fld_param_table[i].d=atof(sText);
			//对焊法兰钢管壁厚记录为0 
			fld_param_table[i].t=0;		//atof(s_t);
			pListCtrl->GetItemText(i,3,sText,50);
			char cMat=sText[0];
			fld_param_table[i].cMaterial=cMat;
			pListCtrl->GetItemText(i,4,sText,50);
			char cMark=sText[0];
			fld_param_table[i].cLevelMark=cMark;
			pListCtrl->GetItemText(i,5,sText,50);
			fld_param_table[i].A=atof(sText);
			pListCtrl->GetItemText(i,6,sText,50);
			fld_param_table[i].B=atof(sText);
			pListCtrl->GetItemText(i,7,sText,50);
			fld_param_table[i].D=atof(sText);
			pListCtrl->GetItemText(i,8,sText,50);
			fld_param_table[i].K=atof(sText);
			pListCtrl->GetItemText(i,9,sText,50);
			fld_param_table[i].L=atof(sText);
			pListCtrl->GetItemText(i,10,sText,50);
			fld_param_table[i].nBoltNum=atoi(sText);
			pListCtrl->GetItemText(i,11,sText,50);
			fld_param_table[i].nBoltD=atoi(sText);
			pListCtrl->GetItemText(i,12,sText,50);
			fld_param_table[i].C=atof(sText);
			pListCtrl->GetItemText(i,13,sText,50);
			fld_param_table[i].H=atof(sText);
			pListCtrl->GetItemText(i,14,sText,50);
			fld_param_table[i].N=atof(sText);
			pListCtrl->GetItemText(i,15,sText,50);
			//对焊法兰S参数中记录 相对于钢管后的的增量
			strncpy(s_t,sText+2,20);
			fld_param_table[i].S=atof(s_t);
			pListCtrl->GetItemText(i,16,sText,50);
			fld_param_table[i].R=atof(sText);
			pListCtrl->GetItemText(i,17,sText,50);
			fld_param_table[i].H1=atof(sText);
			pListCtrl->GetItemText(i,18,sText,50);
			fld_param_table[i].group[0].weight=atof(sText);	//未完全考虑升级为若干壁厚组情况 wjh 2011.11.21
			//m_sFlName.Format("FLD%.f%c",d,cMark);
			//strcpy(fld_param_table[i].codeName,m_sFlName);
		}
	}
	else if(m_iPartType==1)
	{	//平焊法兰
		flp_param_N=nCount;
		for(i=0;i<nCount;i++)
		{
			CString ss;
			double d=0, t=0;
			char sText[50],s_t[50];
			pListCtrl->GetItemText(i,0,fld_param_table[i].codeName,16);
			pListCtrl->GetItemText(i,1,sText,50);
			pListCtrl->GetItemText(i,2,s_t,50);
			d=atof(sText);
			flp_param_table[i].d=atof(sText);
			flp_param_table[i].t=atof(s_t);
			pListCtrl->GetItemText(i,3,sText,50);
			char cMat=sText[0];
			flp_param_table[i].cMaterial=cMat;
			pListCtrl->GetItemText(i,4,sText,50);
			char cMark=sText[0];
			flp_param_table[i].cLevelMark=cMark;
			pListCtrl->GetItemText(i,5,sText,50);
			flp_param_table[i].A=atof(sText);
			pListCtrl->GetItemText(i,6,sText,50);
			flp_param_table[i].B=atof(sText);
			pListCtrl->GetItemText(i,7,sText,50);
			flp_param_table[i].D=atof(sText);
			pListCtrl->GetItemText(i,8,sText,50);
			flp_param_table[i].K=atof(sText);
			pListCtrl->GetItemText(i,9,sText,50);
			flp_param_table[i].L=atof(sText);
			pListCtrl->GetItemText(i,10,sText,50);
			flp_param_table[i].nBoltNum=atoi(sText);
			pListCtrl->GetItemText(i,11,sText,50);
			flp_param_table[i].nBoltD=atoi(sText);
			pListCtrl->GetItemText(i,12,sText,50);
			flp_param_table[i].C=atof(sText);
			pListCtrl->GetItemText(i,13,sText,50);
			flp_param_table[i].H=atof(sText);
			pListCtrl->GetItemText(i,14,sText,50);
			flp_param_table[i].N=atof(sText);
			pListCtrl->GetItemText(i,15,sText,50);
			flp_param_table[i].S=atof(sText);
			pListCtrl->GetItemText(i,16,sText,50);
			flp_param_table[i].R=atof(sText);
			pListCtrl->GetItemText(i,17,sText,50);
			flp_param_table[i].H1=atof(sText);
			pListCtrl->GetItemText(i,18,sText,50);
			flp_param_table[i].group[0].weight=atof(sText);
			//m_sFlName.Format("FLP%.f%c",d,cMark);
			//strcpy(fld_param_table[i].codeName,m_sFlName);
		}
	}
	return TRUE;
}

void CStdPartFlTableDlg::OnOK()
{
	if(DoSave())
		CDialog::OnOK();
}

void CStdPartFlTableDlg::OnFlCoupleLevel()
{
	CFlCoupleLevelDlg dlg;
	dlg.DoModal();
}

void CStdPartFlTableDlg::UpdateFlName()
{
	UpdateData();
	double main_d,couple_d,t;
	int d_out_third=0,d_out_sec=0;	//确定法兰外径的钢管直径的百位与十位数字
	int d_in_third=0,d_in_sec=0;	//确定法兰内径的钢管直径的百位与十位数字
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MAIN_TUBE_GUIGE);
	int iCurSel = pCMB->GetCurSel();
	if(iCurSel>0)
		pCMB->GetLBText(iCurSel,m_sMainTubeGuiGe);
	else
		pCMB->GetWindowText(m_sMainTubeGuiGe);
	//标准法兰命名规则：前两位表示法兰类型 FD--对焊法兰 FP--平焊法兰
	//					第三四位为确定法兰外径的钢管直径的百位与十位数字
	//					第五六位为确定法兰内径的钢管直径的百位与十位数字
	restore_JG_guige(m_sMainTubeGuiGe.GetBuffer(),main_d,t);
	d_in_third=ftol(main_d)/100;
	d_in_sec=(ftol(main_d)%100)/10;
	CString sMark;
	GetDlgItem(IDC_CMB_FL_LEVEL)->GetWindowText(sMark);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sMark);
	if(strlen(tem_str)>0)
		couple_d=QueryCoupleTubeD(main_d,tem_str[0]);
	if(couple_d<=0)
		return;
	d_out_third=ftol(couple_d)/100;
	d_out_sec=(ftol(couple_d)%100)/10;
	if(m_iPartType==0)	//对焊法兰
		m_sFlName.Format("FD%d%d%d%d",d_out_third,d_out_sec,d_in_third,d_in_sec);
	else if(m_iPartType==1)	//平焊法兰
		m_sFlName.Format("FP%d%d%d%d",d_out_third,d_out_sec,d_in_third,d_in_sec);
	UpdateData(FALSE);
}

void CStdPartFlTableDlg::OnSelchangeMainTubeGuige()
{
	UpdateFlName();
}

void CStdPartFlTableDlg::OnSelchangeFlLevel()
{
	UpdateFlName();
}

void CStdPartFlTableDlg::OnKillfocusMainTubeGuige() 
{
	UpdateFlName();	
}

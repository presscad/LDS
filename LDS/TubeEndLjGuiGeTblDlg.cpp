// TubeEndLjGuiGeTblDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "TubeEndLjGuiGeTblDlg.h"
#include "Query.h"
#include "database.h"
#include "GlobalFunc.h"
#include "XhCharString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTubeEndLjGuiGeTblDlg dialog


CTubeEndLjGuiGeTblDlg::CTubeEndLjGuiGeTblDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTubeEndLjGuiGeTblDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTubeEndLjGuiGeTblDlg)
	m_fParaA  = 10;
	m_fParaR  = 15;
	m_fParamL = 520;
	m_fParamM = 260;
	m_fParamH = 180;
	m_fParamW = 40;
	m_fParamN = 40;
	m_fParamS = 20;
	m_fHorizSpace = 80;
	m_fVertiSpace = 80;
	m_fBoltPosX = -40;
	m_fBoltPosY = 200;
	m_fWeight = 0;
	m_sBoltD = _T("20");
	m_sInsertPlateName = _T("C0804S1");
	m_sMainTubeGuiGe = _T("89X4");
	m_nParamT = 10;
	m_iInsertPlateType = 0;
	m_iInsertPlateMaterial = 0;
	m_nBoltNum = 4;
	m_nRowNum = 2;
	m_nColNum = 2;
	m_bDoubleRow = FALSE;
	m_iInsertPlateLevel = 0; //插板级别
	m_iTubeMaterial = 0;
	m_sKPlateNo = _T("");
	m_sLPlateNo = _T("");
	m_sZPlateNo = _T("");
	//}}AFX_DATA_INIT
}


void CTubeEndLjGuiGeTblDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTubeEndLjGuiGeTblDlg)
	DDX_Text(pDX, IDC_E_PARA_A, m_fParaA);
	DDX_Text(pDX, IDC_E_PARA_R, m_fParaR);
	DDX_Text(pDX, IDC_E_PARAM_L, m_fParamL);
	DDX_Text(pDX, IDC_E_PARAM_M, m_fParamM);
	DDX_Text(pDX, IDC_E_PARAM_H, m_fParamH);
	DDX_Text(pDX, IDC_E_PARAM_W, m_fParamW);
	DDX_Text(pDX, IDC_E_PARAM_N, m_fParamN);
	DDX_Text(pDX, IDC_E_PARAM_S, m_fParamS);
	DDX_Text(pDX, IDC_E_HORIZ_SPACE, m_fHorizSpace);
	DDX_Text(pDX, IDC_E_VERTI_SPACE, m_fVertiSpace);
	DDX_Text(pDX, IDC_E_BOLT_POS_X, m_fBoltPosX);
	DDX_Text(pDX, IDC_E_BOLT_POS_Y, m_fBoltPosY);
	DDX_Text(pDX, IDC_E_WEIGHT, m_fWeight);
	DDX_CBString(pDX, IDC_CMB_BOLT_D, m_sBoltD);
	DDX_Text(pDX, IDC_E_INSERT_PLATE_NAME, m_sInsertPlateName);
	DDX_CBString(pDX, IDC_CMB_MAIN_TUBE_GUIGE, m_sMainTubeGuiGe);
	DDX_Text(pDX, IDC_E_PARAM_T, m_nParamT);
	DDX_CBIndex(pDX, IDC_CMB_INSERT_PLATE_TYPE, m_iInsertPlateType);
	DDX_CBIndex(pDX, IDC_CMB_INSERT_PLATE_MATERIAL, m_iInsertPlateMaterial);
	DDX_Text(pDX, IDC_E_BOLT_NUM, m_nBoltNum);
	DDX_Text(pDX, IDC_E_ROW_NUM, m_nRowNum);
	DDX_Text(pDX, IDC_E_COL_NUM, m_nColNum);
	DDX_Check(pDX, IDC_CHK_DOUBLE_ROW, m_bDoubleRow);
	DDX_CBIndex(pDX, IDC_CMB_INSERT_PLATE_LEVEL, m_iInsertPlateLevel);
	DDX_CBIndex(pDX, IDC_CMB_TUBE_MATERIAL, m_iTubeMaterial);
	DDX_Text(pDX, IDC_E_K_PLATE_NO, m_sKPlateNo);
	DDX_Text(pDX, IDC_E_L_PLATE_NO, m_sLPlateNo);
	DDX_Text(pDX, IDC_E_Z_PLATE_NO, m_sZPlateNo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTubeEndLjGuiGeTblDlg, CDialog)
	//{{AFX_MSG_MAP(CTubeEndLjGuiGeTblDlg)
	ON_BN_CLICKED(IDC_BN_IMPORT_INSERT_PLATE_FILE, OnImportInsertPlateFile)
	ON_BN_CLICKED(IDC_BN_EXPORT_INSERT_PLATE_FILE, OnExportInsertPlateFile)
	ON_BN_CLICKED(IDC_BN_ADD_NEW_BOLT_ITEM, OnAddNewBoltItem)
	ON_BN_CLICKED(IDC_BN_MODIFY_BOLT_ITEM, OnModifyBoltItem)
	ON_BN_CLICKED(IDC_BN_ADD_NEW_ITEM, OnAddNewItem)
	ON_BN_CLICKED(IDC_BN_MODIFY_ITEM, OnModifyItem)
	ON_EN_CHANGE(IDC_E_PARAM_N, OnChangeParamN)
	ON_CBN_SELCHANGE(IDC_CMB_INSERT_PLATE_TYPE, OnSelchangeInsertPlateType)
	ON_NOTIFY(NM_DBLCLK, IDC_LAYOUT_BOLT_PARA_LIST, OnDblclkLayOutBoltParaList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LAYOUT_BOLT_PARA_LIST, OnKeydownLayOutBoltParaList)
	ON_NOTIFY(NM_DBLCLK, IDC_INSERT_PLATE_LIB_LIST, OnDblclkInsertPlateLibList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_INSERT_PLATE_LIB_LIST, OnKeydownInsertPlateLibList)
	ON_CBN_SELCHANGE(IDC_CMB_TUBE_MATERIAL, OnSelchangeTubeMaterial)
	ON_CBN_SELCHANGE(IDC_CMB_MAIN_TUBE_GUIGE, OnSelchangeMainTubeGuige)
	ON_BN_CLICKED(IDC_CHK_DOUBLE_ROW, OnDoubleRowLs)
	ON_CBN_KILLFOCUS(IDC_CMB_MAIN_TUBE_GUIGE, OnKillfocusMainTubeGuige)
	ON_CBN_SELCHANGE(IDC_CMB_INSERT_PLATE_LEVEL, OnSelchangeInsertPlateLevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTubeEndLjGuiGeTblDlg message handlers

BOOL CTubeEndLjGuiGeTblDlg::OnInitDialog() 
{
	CComboBox*pCMB = (CComboBox*)GetDlgItem(IDC_CMB_INSERT_PLATE_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iInsertPlateMaterial);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_TUBE_MATERIAL);
	AddSteelMatRecord(pCMB);
	pCMB->SetCurSel(m_iTubeMaterial);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MAIN_TUBE_GUIGE);
	AddTubeRecord(pCMB);
	pCMB->SetCurSel(0);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_INSERT_PLATE_TYPE);
	pCMB->AddString("C型插板");
	pCMB->AddString("U型插板");
	pCMB->AddString("X型插板");
	pCMB->SetCurSel(0);
	pCMB = (CComboBox*)GetDlgItem(IDC_CMB_BOLT_D);
	pCMB->AddString("16");
	pCMB->AddString("20");
	pCMB->AddString("24");
	pCMB->AddString("27");
	pCMB->SetCurSel(1);
	//默认的是槽型所以默认不可用
	GetDlgItem(IDC_E_K_PLATE_NO)->EnableWindow(FALSE);
	GetDlgItem(IDC_E_Z_PLATE_NO)->EnableWindow(FALSE);
	GetDlgItem(IDC_E_L_PLATE_NO)->EnableWindow(FALSE);
	//GetDlgItem(IDC_E_INSERT_PLATE_NAME)->EnableWindow(FALSE);
	RefreshListView();
	return CDialog::OnInitDialog();  
}

void CTubeEndLjGuiGeTblDlg::RefreshListView()
{
	RefreshListViewHeader();
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_INSERT_PLATE_LIB_LIST);
	CListCtrl *pBoltList=(CListCtrl*)GetDlgItem(IDC_LAYOUT_BOLT_PARA_LIST);
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_PIC_PLATE_TYPE);
	m_CurPlateBmp.Detach();
	pListCtrl->DeleteAllItems();
	pBoltList->DeleteAllItems();
	//规格库中的记录与显示在列表中的记录相对应 即规格库中的第N条记录在列表中也显示在第N个位置
	if(m_iInsertPlateType==0)
	{	//C型插板
		GetDlgItem(IDC_CHK_DOUBLE_ROW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_COL_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_ROW_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_PARA_A)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PARA_R)->EnableWindow(FALSE);
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_ROLLEND_BOLT);
		for(int i=rollend_param_N;i<200;i++)
			rollend_param_table[i].iBoltRect=0;
		for(i=0;i<rollend_param_N;i++)
		{
			CStringArray str_arr;
			str_arr.SetSize(14);
			str_arr[0].Format("%s",rollend_param_table[i].codeName);
			str_arr[1].Format("%c",rollend_param_table[i].cMaterial);
			str_arr[2].Format("%.f",rollend_param_table[i].d);
			str_arr[3].Format("%.f",rollend_param_table[i].t);
			str_arr[4].Format("%c",rollend_param_table[i].cTubeMat);	//增加钢管材质
			int iLevelMark=rollend_param_table[i].iLevelMark;
			if(iLevelMark>=9)
				iLevelMark=1;
			str_arr[5].Format("%d",iLevelMark);
			str_arr[6].Format("%.f",rollend_param_table[i].L);
			str_arr[7].Format("%.f",rollend_param_table[i].M);
			str_arr[8].Format("%.f",rollend_param_table[i].H);
			str_arr[9].Format("%.f",rollend_param_table[i].W);
			str_arr[10].Format("%.f",rollend_param_table[i].S);
			str_arr[11].Format("%.f",rollend_param_table[i].N);
			str_arr[12].Format("%d",rollend_param_table[i].Thick);
			str_arr[13].Format("%.2f",rollend_param_table[i].theroy_weight);
			AppendRecord(pListCtrl,-1,str_arr);
		}
	}
	else if(m_iInsertPlateType==1)
	{	//U型插板
		GetDlgItem(IDC_CHK_DOUBLE_ROW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_E_COL_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_ROW_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_PARA_A)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_PARA_R)->EnableWindow(FALSE);
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_UEND_BOLT);
		for(int i=uend_param_N;i<200;i++)
			uend_param_table[i].iBoltRect=0;
		for(i=0;i<uend_param_N;i++)
		{
			CStringArray str_arr;
			str_arr.SetSize(14);
			str_arr[0].Format("%s",uend_param_table[i].codeName);
			str_arr[1].Format("%c",uend_param_table[i].cMaterial);
			str_arr[2].Format("%.f",uend_param_table[i].d);
			str_arr[3].Format("%.f",uend_param_table[i].t);
			str_arr[4].Format("%c",uend_param_table[i].cTubeMat);
			int iLevelMark=uend_param_table[i].iLevelMark;
			if(iLevelMark>=9)
				iLevelMark=1;
			str_arr[5].Format("%d",iLevelMark);
			str_arr[6].Format("%.f",uend_param_table[i].L);
			str_arr[7].Format("%.f",uend_param_table[i].M);
			str_arr[8].Format("%.f",uend_param_table[i].H);
			str_arr[9].Format("%.f",uend_param_table[i].W);
			str_arr[10].Format("%.f",uend_param_table[i].S);
			str_arr[11].Format("%.f",uend_param_table[i].N);
			str_arr[12].Format("%d",uend_param_table[i].Thick);
			str_arr[13].Format("%.2f",uend_param_table[i].theroy_weight);
			AppendRecord(pListCtrl,-1,str_arr);
		}
	}
	else if(m_iInsertPlateType==2)
	{	//X型插板
		GetDlgItem(IDC_E_PARA_A)->EnableWindow(TRUE);	
		GetDlgItem(IDC_E_PARA_R)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_DOUBLE_ROW)->ShowWindow(SW_SHOW);//十字插板才需要交错布置螺栓
		GetDlgItem(IDC_E_COL_NUM)->EnableWindow(!m_bDoubleRow);
		GetDlgItem(IDC_E_ROW_NUM)->EnableWindow(!m_bDoubleRow);
		if(m_bDoubleRow)
			m_nColNum=m_nRowNum=-1;
		m_CurPlateBmp.LoadBitmap(IDB_BMP_PLATE_XEND_BOLT);
		for(int i=xend_param_N;i<200;i++)
			xend_param_table[i].iBoltRect=0;
		for(i=0;i<xend_param_N;i++)
		{
			CStringArray str_arr;
			str_arr.SetSize(19);
			str_arr[0].Format("%s",xend_param_table[i].codeName);
			str_arr[1].Format("%s",xend_param_table[i].datumPlatePartNo);
			str_arr[2].Format("%s",xend_param_table[i].weldPlatePartNo);
			str_arr[3].Format("%s",xend_param_table[i].wrapPlatePartNo);
			str_arr[4].Format("%c",xend_param_table[i].cMaterial);
			str_arr[5].Format("%.f",xend_param_table[i].d);
			str_arr[6].Format("%.f",xend_param_table[i].t);
			str_arr[7].Format("%c",xend_param_table[i].cTubeMat);
			int iLevelMark=xend_param_table[i].iLevelMark;
			if(iLevelMark>=9)
				iLevelMark=1;
			str_arr[8].Format("%d",iLevelMark);
			str_arr[9].Format("%.f",xend_param_table[i].L);
			str_arr[10].Format("%.f",xend_param_table[i].M);
			str_arr[11].Format("%.f",xend_param_table[i].H);
			str_arr[12].Format("%.f",xend_param_table[i].W);
			str_arr[13].Format("%.f",xend_param_table[i].S);
			str_arr[14].Format("%.f",xend_param_table[i].N);
			str_arr[15].Format("%d" ,xend_param_table[i].Thick);
			str_arr[16].Format("%.f",xend_param_table[i].A);
			str_arr[17].Format("%.f",xend_param_table[i].R);
			str_arr[18].Format("%.2f",xend_param_table[i].theroy_weight);
			AppendRecord(pListCtrl,-1,str_arr);
		}
	}
	pPic->SetBitmap(m_CurPlateBmp);
}

int CTubeEndLjGuiGeTblDlg::AppendRecord(CListCtrl *pListCtrl, int iItem, CStringArray &str_arr)
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

void CTubeEndLjGuiGeTblDlg::RefreshListViewHeader()
{
	LV_COLUMN lvc;
	int i,nColumnCount,iOffect=0;
	//----------下面为插板库列表-------------
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_INSERT_PLATE_LIB_LIST);
	pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES| LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	CHeaderCtrl* pHeader = pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();
	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	   pListCtrl->DeleteColumn(0);
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = "插板代号";//_gszColumnLabel[i];
	lvc.cx = 70;
	lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
	pListCtrl->InsertColumn(0,&lvc);
	if(m_iInsertPlateType == 2)
	{
		lvc.iSubItem = 1;
		lvc.pszText = "基板编号";//_gszColumnLabel[i];
		lvc.cx = 70;
		pListCtrl->InsertColumn(1,&lvc);
		lvc.iSubItem = 2;
		lvc.pszText = "立板编号";//_gszColumnLabel[i];
		lvc.cx = 70;
		pListCtrl->InsertColumn(2,&lvc);
		lvc.iSubItem = 3;
		lvc.pszText = "包板编号";//_gszColumnLabel[i];
		lvc.cx = 70;
		pListCtrl->InsertColumn(3,&lvc);
		iOffect=3;
	}
	else
		iOffect=0;
	lvc.iSubItem = 1+iOffect;
	lvc.pszText = "材质";//_gszColumnLabel[i];
	lvc.cx = 45;
	pListCtrl->InsertColumn(1+iOffect,&lvc);
	lvc.iSubItem = 2+iOffect;
	lvc.pszText = "管径";//_gszColumnLabel[i];
	lvc.cx = 45;
	lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
	pListCtrl->InsertColumn(2+iOffect,&lvc);
	lvc.iSubItem = 3+iOffect;
	lvc.pszText = "管厚";//_gszColumnLabel[i];
	lvc.cx = 45;
	pListCtrl->InsertColumn(3+iOffect,&lvc);
	lvc.iSubItem = 4+iOffect;
	lvc.pszText = "管材质";//_gszColumnLabel[i];
	lvc.cx = 55;
	pListCtrl->InsertColumn(4+iOffect,&lvc);
	lvc.iSubItem = 5+iOffect;
	lvc.pszText = "级别";//_gszColumnLabel[i];
	lvc.cx = 45;
	pListCtrl->InsertColumn(5+iOffect,&lvc);
	lvc.iSubItem = 6+iOffect;
	lvc.pszText = "L";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(6+iOffect,&lvc);
	lvc.iSubItem = 7+iOffect;
	lvc.pszText = "M";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(7+iOffect,&lvc);
	lvc.iSubItem = 8+iOffect;
	lvc.pszText = "H";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(8+iOffect,&lvc);
	lvc.iSubItem = 9+iOffect;
	lvc.pszText = "W";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(9+iOffect,&lvc);
	lvc.iSubItem = 10+iOffect;
	lvc.pszText = "S";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(10+iOffect,&lvc);
	lvc.iSubItem = 11+iOffect;
	lvc.pszText = "N";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(11+iOffect,&lvc);
	lvc.iSubItem = 12+iOffect;
	lvc.pszText = "T";//_gszColumnLabel[i];
	lvc.cx = 37;
	pListCtrl->InsertColumn(12+iOffect,&lvc);
	if(m_iInsertPlateType==2)
	{
		lvc.iSubItem = 13+iOffect;
		lvc.pszText = "A";//_gszColumnLabel[i];
		lvc.cx = 37;
		pListCtrl->InsertColumn(13+iOffect,&lvc);
		lvc.iSubItem = 14+iOffect;
		lvc.pszText = "R";//_gszColumnLabel[i];
		lvc.cx = 37;
		pListCtrl->InsertColumn(14+iOffect,&lvc);
		lvc.iSubItem = 15+iOffect;
		lvc.pszText = "理论重量";//_gszColumnLabel[i];
		lvc.cx = 80;
		pListCtrl->InsertColumn(15+iOffect,&lvc);
	}
	else
	{
		lvc.iSubItem = 13+iOffect;
		lvc.pszText = "理论重量";//_gszColumnLabel[i];
		lvc.cx = 80;
		pListCtrl->InsertColumn(13+iOffect,&lvc);
	}
	//----------下面为布置螺栓参数列表----------------------
	pListCtrl=(CListCtrl*)GetDlgItem(IDC_LAYOUT_BOLT_PARA_LIST);
	pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES| LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	pHeader = pListCtrl->GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();
	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	   pListCtrl->DeleteColumn(0);
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = "直径";//_gszColumnLabel[i];
	lvc.cx = 40;
	lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
	pListCtrl->InsertColumn(0,&lvc);
	lvc.iSubItem = 1;
	lvc.pszText = "个数";//_gszColumnLabel[i];
	lvc.cx = 40;
	lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
	pListCtrl->InsertColumn(1,&lvc);
	lvc.iSubItem = 2;
	lvc.pszText = "行数";//_gszColumnLabel[i];
	lvc.cx = 40;
	lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
	pListCtrl->InsertColumn(2,&lvc);
	lvc.iSubItem = 3;
	lvc.pszText = "列数";//_gszColumnLabel[i];
	lvc.cx = 40;
	pListCtrl->InsertColumn(3,&lvc);
	lvc.iSubItem = 4;
	lvc.pszText = "列间距";//_gszColumnLabel[i];
	lvc.cx = 60;
	pListCtrl->InsertColumn(4,&lvc);
	lvc.iSubItem = 5;
	lvc.pszText = "行间距";//_gszColumnLabel[i];
	lvc.cx = 60;
	pListCtrl->InsertColumn(5,&lvc);
	lvc.iSubItem = 6;
	lvc.pszText = "X";//_gszColumnLabel[i];
	lvc.cx = 50;
	pListCtrl->InsertColumn(6,&lvc);
	lvc.iSubItem = 7;
	lvc.pszText = "Y";//_gszColumnLabel[i];
	lvc.cx = 50;
	pListCtrl->InsertColumn(7,&lvc);
}

void CTubeEndLjGuiGeTblDlg::OnImportInsertPlateFile() 
{
	CString sFileName;
	char *sName="cipl";
	char *sFileType="槽型插板库文件(*.cipl)|*.cipl||";
	if(m_iInsertPlateType==1)
	{
		sName="uipl";
		sFileType="U型插板库文件(*.uipl)|*.uipl||";
	}
	else if(m_iInsertPlateType==2)
	{
		sName="xipl";
		sFileType="十字型插板库文件(*.xipl)|*.xipl||";
	}
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		if(m_iInsertPlateType==0)
			CInsertPlateSerialize(sFileName,FALSE);
		else if(m_iInsertPlateType==1)
			UInsertPlateSerialize(sFileName,FALSE);
		else if(m_iInsertPlateType==2)
			XInsertPlateSerialize(sFileName,FALSE);
		//导入以后需要刷新列表内容 
		RefreshListView();
	}
}

void CTubeEndLjGuiGeTblDlg::OnExportInsertPlateFile() 
{
	CString sFileName;
	char *sName="cipl";
	char *sFileType="槽型插板库文件(*.cipl)|*.cipl||";
	if(m_iInsertPlateType==1)
	{
		sName="uipl";
		sFileType="U型插板库文件(*.uipl)|*.uipl||";
	}
	else if(m_iInsertPlateType==2)
	{
		sName="xipl";
		sFileType="十字型插板库文件(*.xipl)|*.xipl||";
	}
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		if(m_iInsertPlateType==0)
			CInsertPlateSerialize(sFileName,TRUE);
		else if(m_iInsertPlateType==1)
			UInsertPlateSerialize(sFileName,TRUE);
		else if(m_iInsertPlateType==2)
			XInsertPlateSerialize(sFileName,TRUE);
	}
}

void CTubeEndLjGuiGeTblDlg::OnAddNewBoltItem()
{
	UpdateData();
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_LAYOUT_BOLT_PARA_LIST);
	CStringArray str_arr;
	str_arr.SetSize(8);
	str_arr[0].Format("%s",m_sBoltD);
	str_arr[1].Format("%d",m_nBoltNum);
	str_arr[2].Format("%d",m_nRowNum);
	str_arr[3].Format("%d",m_nColNum);
	str_arr[4].Format("%.f",m_fHorizSpace);
	str_arr[5].Format("%.f",m_fVertiSpace);
	str_arr[6].Format("%.f",m_fBoltPosX);
	str_arr[7].Format("%.f",m_fBoltPosY);
	if(pListCtrl->GetItemCount()==6)
	{
		MessageBox("抱歉!系统暂时最多只能支持6组螺栓布置参数!");
		return;
	}
	AppendRecord(pListCtrl,-1,str_arr);
	pListCtrl=(CListCtrl*)GetDlgItem(IDC_INSERT_PLATE_LIB_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{	//将布置螺栓的参数保存到相应的位置
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);	//向第iCurSel条插板参数记录中添加螺栓布置信息
		if(m_iInsertPlateType==0)
		{	//槽型插板
			int iBoltRect=rollend_param_table[iCurSel].iBoltRect;	//向第i条插板参数记录中添加第iBoltRect条螺栓布置信息
			rollend_param_table[iCurSel].ls_param[iBoltRect].iBoltD=atoi(m_sBoltD);
			rollend_param_table[iCurSel].ls_param[iBoltRect].iBoltNum=m_nBoltNum;
			rollend_param_table[iCurSel].ls_param[iBoltRect].iRow=m_nRowNum;
			rollend_param_table[iCurSel].ls_param[iBoltRect].iColumn=m_nColNum;
			rollend_param_table[iCurSel].ls_param[iBoltRect].fHorzSpace=m_fHorizSpace;
			rollend_param_table[iCurSel].ls_param[iBoltRect].fVertSpace=m_fVertiSpace;
			rollend_param_table[iCurSel].ls_param[iBoltRect].fBoltPosX=m_fBoltPosX;
			rollend_param_table[iCurSel].ls_param[iBoltRect].fBoltPosY=m_fBoltPosY;
			rollend_param_table[iCurSel].iBoltRect++;
		}
		else if(m_iInsertPlateType==1)
		{	//U型插板
			int iBoltRect=uend_param_table[iCurSel].iBoltRect;	//向第iCurSel条插板参数记录中添加第iBoltRect条螺栓布置信息
			uend_param_table[iCurSel].ls_param[iBoltRect].iBoltD=atoi(m_sBoltD);
			uend_param_table[iCurSel].ls_param[iBoltRect].iBoltNum=m_nBoltNum;
			uend_param_table[iCurSel].ls_param[iBoltRect].iRow=m_nRowNum;
			uend_param_table[iCurSel].ls_param[iBoltRect].iColumn=m_nColNum;
			uend_param_table[iCurSel].ls_param[iBoltRect].fHorzSpace=m_fHorizSpace;
			uend_param_table[iCurSel].ls_param[iBoltRect].fVertSpace=m_fVertiSpace;
			uend_param_table[iCurSel].ls_param[iBoltRect].fBoltPosX=m_fBoltPosX;
			uend_param_table[iCurSel].ls_param[iBoltRect].fBoltPosY=m_fBoltPosY;
			uend_param_table[iCurSel].iBoltRect++;
		}
		else if(m_iInsertPlateType==2)
		{	//十字插板
			//十字型插板存储布置螺栓参数时,如果第一组布置螺栓参数的行数列数均为-1，
			//则表示该十字插板上的螺栓为双排螺栓,且竖直面和水平面呈交错布置 wht 09-06-06
			int iBoltRect=xend_param_table[iCurSel].iBoltRect;	//向第iCurSel条插板参数记录中添加第iBoltRect条螺栓布置信息
			xend_param_table[iCurSel].ls_param[iBoltRect].iBoltD=atoi(m_sBoltD);
			xend_param_table[iCurSel].ls_param[iBoltRect].iBoltNum=m_nBoltNum;
			xend_param_table[iCurSel].ls_param[iBoltRect].iRow=m_nRowNum;
			xend_param_table[iCurSel].ls_param[iBoltRect].iColumn=m_nColNum;
			xend_param_table[iCurSel].ls_param[iBoltRect].fHorzSpace=m_fHorizSpace;
			xend_param_table[iCurSel].ls_param[iBoltRect].fVertSpace=m_fVertiSpace;
			xend_param_table[iCurSel].ls_param[iBoltRect].fBoltPosX=m_fBoltPosX;
			xend_param_table[iCurSel].ls_param[iBoltRect].fBoltPosY=m_fBoltPosY;
			xend_param_table[iCurSel].iBoltRect++;
		}
	}
}

void CTubeEndLjGuiGeTblDlg::OnModifyBoltItem()
{
	UpdateData();	
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_LAYOUT_BOLT_PARA_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{	
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		char ss[MAX_TEMP_CHAR_100+1]="";
		_snprintf(ss,MAX_TEMP_CHAR_100,"%s",m_sBoltD);
		pListCtrl->SetItemText(iCurSel,0,m_sBoltD);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%d",m_nBoltNum);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,1,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%d",m_nRowNum);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,2,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%d",m_nColNum);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,3,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fHorizSpace);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,4,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fVertiSpace);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,5,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fBoltPosX);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,6,ss);
		_snprintf(ss,MAX_TEMP_CHAR_100,"%.f",m_fBoltPosY);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,7,ss);
		CListCtrl *pListCtrl1=(CListCtrl*)GetDlgItem(IDC_INSERT_PLATE_LIB_LIST);
		POSITION pos1 = pListCtrl1->GetFirstSelectedItemPosition();
		if(pos1!=NULL)
		{	//确定当前选中的插板记录将修改后的值保存到列表中
			int nCurSel = pListCtrl1->GetNextSelectedItem(pos1);//第nCurSel条插板记录
			_snprintf(ss,MAX_TEMP_CHAR_100,"%s",m_sBoltD);
			//修改第nCurSel条插板记录中的第iCurSel条螺栓布置信息
			if(m_iInsertPlateType==0)
			{	//槽型插板
				rollend_param_table[nCurSel].ls_param[iCurSel].iBoltD=atoi(ss);
				rollend_param_table[nCurSel].ls_param[iCurSel].iBoltNum=m_nBoltNum;
				rollend_param_table[nCurSel].ls_param[iCurSel].iRow=m_nRowNum;
				rollend_param_table[nCurSel].ls_param[iCurSel].iColumn=m_nColNum;
				rollend_param_table[nCurSel].ls_param[iCurSel].fHorzSpace=m_fHorizSpace;
				rollend_param_table[nCurSel].ls_param[iCurSel].fVertSpace=m_fVertiSpace;
				rollend_param_table[nCurSel].ls_param[iCurSel].fBoltPosX=m_fBoltPosX;
				rollend_param_table[nCurSel].ls_param[iCurSel].fBoltPosY=m_fBoltPosY;
			}
			else if(m_iInsertPlateType==1)
			{	//U型插板
				uend_param_table[nCurSel].ls_param[iCurSel].iBoltD=atoi(ss);
				uend_param_table[nCurSel].ls_param[iCurSel].iBoltNum=m_nBoltNum;
				uend_param_table[nCurSel].ls_param[iCurSel].iRow=m_nRowNum;
				uend_param_table[nCurSel].ls_param[iCurSel].iColumn=m_nColNum;
				uend_param_table[nCurSel].ls_param[iCurSel].fHorzSpace=m_fHorizSpace;
				uend_param_table[nCurSel].ls_param[iCurSel].fVertSpace=m_fVertiSpace;
				uend_param_table[nCurSel].ls_param[iCurSel].fBoltPosX=m_fBoltPosX;
				uend_param_table[nCurSel].ls_param[iCurSel].fBoltPosY=m_fBoltPosY;
			}
			else if(m_iInsertPlateType==2)
			{	//十字插板
				xend_param_table[nCurSel].ls_param[iCurSel].iBoltD=atoi(ss);
				xend_param_table[nCurSel].ls_param[iCurSel].iBoltNum=m_nBoltNum;
				xend_param_table[nCurSel].ls_param[iCurSel].iRow=m_nRowNum;
				xend_param_table[nCurSel].ls_param[iCurSel].iColumn=m_nColNum;
				xend_param_table[nCurSel].ls_param[iCurSel].fHorzSpace=m_fHorizSpace;
				xend_param_table[nCurSel].ls_param[iCurSel].fVertSpace=m_fVertiSpace;
				xend_param_table[nCurSel].ls_param[iCurSel].fBoltPosX=m_fBoltPosX;
				xend_param_table[nCurSel].ls_param[iCurSel].fBoltPosY=m_fBoltPosY;
			}
		}
	}
}

void CTubeEndLjGuiGeTblDlg::OnAddNewItem()
{
	UpdateData();
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_INSERT_PLATE_LIB_LIST);
	CStringArray str_arr;
	double d,t;
	int iOffect=0;
	CString sMaterial,sLevel;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	GetDlgItem(IDC_CMB_MAIN_TUBE_GUIGE)->GetWindowText(m_sMainTubeGuiGe);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",m_sMainTubeGuiGe);
	restore_JG_guige(tem_str,d,t);
	if(m_iInsertPlateType==2)	//十字插板
		str_arr.SetSize(19);
	else
		str_arr.SetSize(14);
	str_arr[0].Format("%s",m_sInsertPlateName);
	GetDlgItem(IDC_CMB_INSERT_PLATE_MATERIAL)->GetWindowText(sMaterial);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sMaterial);
	char cMat=QuerySteelBriefMatMark(tem_str);
	if(m_iInsertPlateType==2)
	{
		str_arr[1].Format("%s",m_sKPlateNo);
		str_arr[2].Format("%s",m_sLPlateNo);
		str_arr[3].Format("%s",m_sZPlateNo);
		iOffect=3;
	}
	else
		iOffect=0;
	str_arr[1+iOffect].Format("%c",cMat);
	str_arr[2+iOffect].Format("%.f",d);
	str_arr[3+iOffect].Format("%d",(int)t);
	GetDlgItem(IDC_CMB_TUBE_MATERIAL)->GetWindowText(sMaterial);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sMaterial);
	char cTubeMat=QuerySteelBriefMatMark(tem_str);
	str_arr[4+iOffect].Format("%c",cTubeMat);
	GetDlgItem(IDC_CMB_INSERT_PLATE_LEVEL)->GetWindowText(sLevel);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sLevel);
	int iLevel=atoi(tem_str);
	str_arr[5+iOffect].Format("%d",iLevel);	//插板级别
	str_arr[6+iOffect].Format("%.f",m_fParamL);
	str_arr[7+iOffect].Format("%.f",m_fParamM);
	str_arr[8+iOffect].Format("%.f",m_fParamH);
	str_arr[9+iOffect].Format("%.f",m_fParamW);
	str_arr[10+iOffect].Format("%.f",m_fParamS);
	str_arr[11+iOffect].Format("%.f",m_fParamN);
	str_arr[12+iOffect].Format("%d",m_nParamT);
	if(m_iInsertPlateType==2)
	{	//十字插板
		str_arr[16].Format("%.f",m_fParaA);
		str_arr[17].Format("%.f",m_fParaR);
		str_arr[18].Format("%.2f",m_fWeight);
	}
	else
		str_arr[13].Format("%.2f",m_fWeight);
	if(rollend_param_N>200||uend_param_N>200||xend_param_N>200)
	{
		MessageBox("系统只能同时处理200种记录");
		return;
	}
	INSERT_PLATE_PARAM *pInsertPlateParam=NULL;
	if(m_iInsertPlateType==0)		//槽型插板
		pInsertPlateParam=FindRollEndParam(d,t,cTubeMat,iLevel);
	else if(m_iInsertPlateType==1)	//U型插板
		pInsertPlateParam=FindUEndParam(d,t,cTubeMat,iLevel);
	else if(m_iInsertPlateType==2)	//十字插板
		pInsertPlateParam=FindXEndParam(d,t,cTubeMat,iLevel);
	if(pInsertPlateParam)
	{
		MessageBox("插板规格库中已存在此规格插板,不能重复添加!");
		return;
	}
	AppendRecord(pListCtrl,-1,str_arr);
	if(m_iInsertPlateType==0)
	{	//槽型插板
		rollend_param_table[rollend_param_N].d=d;
		rollend_param_table[rollend_param_N].t=t;
		rollend_param_table[rollend_param_N].cMaterial=cMat;
		rollend_param_table[rollend_param_N].cTubeMat=cTubeMat;
		rollend_param_table[rollend_param_N].iLevelMark=iLevel;
		rollend_param_table[rollend_param_N].L=m_fParamL;
		rollend_param_table[rollend_param_N].M=m_fParamM;
		rollend_param_table[rollend_param_N].N=m_fParamN;
		rollend_param_table[rollend_param_N].H=m_fParamH;
		rollend_param_table[rollend_param_N].W=m_fParamW;
		rollend_param_table[rollend_param_N].S=m_fParamS;
		rollend_param_table[rollend_param_N].Thick=m_nParamT;
		rollend_param_table[rollend_param_N].iBoltRect=0;
		rollend_param_table[rollend_param_N].theroy_weight=m_fWeight;
		strncpy(rollend_param_table[rollend_param_N].codeName,m_sInsertPlateName,15);
		rollend_param_N++;
	}
	else if(m_iInsertPlateType==1)
	{	//U型插板
		uend_param_table[uend_param_N].d=d;
		uend_param_table[uend_param_N].t=t;
		uend_param_table[uend_param_N].cMaterial=cMat;
		uend_param_table[uend_param_N].cTubeMat=cTubeMat;
		uend_param_table[uend_param_N].iLevelMark=iLevel;
		uend_param_table[uend_param_N].L=m_fParamL;
		uend_param_table[uend_param_N].M=m_fParamM;
		uend_param_table[uend_param_N].N=m_fParamN;
		uend_param_table[uend_param_N].H=m_fParamH;
		uend_param_table[uend_param_N].W=m_fParamW;
		uend_param_table[uend_param_N].S=m_fParamS;
		uend_param_table[uend_param_N].Thick=m_nParamT;
		uend_param_table[uend_param_N].iBoltRect=0;
		uend_param_table[uend_param_N].theroy_weight=m_fWeight;
		strncpy(rollend_param_table[uend_param_N].codeName,m_sInsertPlateName,15);
		uend_param_N++;
	}
	else if(m_iInsertPlateType==2)
	{	//十字插板
		xend_param_table[xend_param_N].d=d;
		xend_param_table[xend_param_N].t=t;
		xend_param_table[xend_param_N].cMaterial=cMat;
		xend_param_table[xend_param_N].cTubeMat=cTubeMat;
		xend_param_table[xend_param_N].iLevelMark=iLevel;
		strncpy(xend_param_table[xend_param_N].datumPlatePartNo,m_sKPlateNo,15);
		strncpy(xend_param_table[xend_param_N].weldPlatePartNo,m_sLPlateNo,15);
		strncpy(xend_param_table[xend_param_N].wrapPlatePartNo,m_sZPlateNo,15);
		xend_param_table[xend_param_N].L=m_fParamL;
		xend_param_table[xend_param_N].M=m_fParamM;
		xend_param_table[xend_param_N].N=m_fParamN;
		xend_param_table[xend_param_N].H=m_fParamH;
		xend_param_table[xend_param_N].W=m_fParamW;
		xend_param_table[xend_param_N].S=m_fParamS;
		xend_param_table[xend_param_N].Thick=m_nParamT;
		xend_param_table[xend_param_N].A=m_fParaA;
		xend_param_table[xend_param_N].R=m_fParaR;
		xend_param_table[xend_param_N].iBoltRect=0;
		xend_param_table[xend_param_N].theroy_weight=m_fWeight;
		strncpy(rollend_param_table[xend_param_N].codeName,m_sInsertPlateName,15);
		xend_param_N++;
	}

}

void CTubeEndLjGuiGeTblDlg::OnModifyItem()
{
	UpdateData();
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_INSERT_PLATE_LIB_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		double d,t;
		int iOffset=0;
		CString sMaterial,sLevel;
		CXhChar100 ss;
		ss.Printf("%s",m_sInsertPlateName);
		pListCtrl->SetItemText(iCurSel,0,ss);
		if(m_iInsertPlateType==2)
		{
			ss.Printf("%s",m_sKPlateNo);
			pListCtrl->SetItemText(iCurSel,1,ss);
			ss.Printf("%s",m_sZPlateNo);
			pListCtrl->SetItemText(iCurSel,2,ss);
			ss.Printf("%s",m_sLPlateNo);
			pListCtrl->SetItemText(iCurSel,3,ss);
			iOffset=3;
		}
		else
			iOffset=0;
		GetDlgItem(IDC_CMB_INSERT_PLATE_MATERIAL)->GetWindowText(sMaterial);
		ss.Printf("%s",sMaterial);
		char cMat=QuerySteelBriefMatMark(ss);
		ss.Printf("%c",cMat);
		pListCtrl->SetItemText(iCurSel,1+iOffset,ss);
		GetDlgItem(IDC_CMB_MAIN_TUBE_GUIGE)->GetWindowText(m_sMainTubeGuiGe);
		ss.Printf("%s",m_sMainTubeGuiGe);
		restore_JG_guige(ss,d,t);
		ss.Printf("%.f",d);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,2+iOffset,ss);
		ss.Printf("%d",(int)t);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,3+iOffset,ss);
		GetDlgItem(IDC_CMB_TUBE_MATERIAL)->GetWindowText(sMaterial);
		ss.Printf("%s",sMaterial);
		char cTubeMat=QuerySteelBriefMatMark(ss);
		ss.Printf("%c",cTubeMat);
		pListCtrl->SetItemText(iCurSel,4+iOffset,ss);
		//插板级别
		GetDlgItem(IDC_CMB_INSERT_PLATE_LEVEL)->GetWindowText(sLevel);
		ss.Printf("%s",sLevel);
		int iLevel=atoi(ss);
		pListCtrl->SetItemText(iCurSel,5+iOffset,ss);
		ss.Printf("%.f",m_fParamL);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,6+iOffset,ss);
		ss.Printf("%.f",m_fParamM);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,7+iOffset,ss);
		ss.Printf("%.f",m_fParamH);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,8+iOffset,ss);
		ss.Printf("%.f",m_fParamW);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,9+iOffset,ss);
		ss.Printf("%.f",m_fParamS);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,10+iOffset,ss);
		ss.Printf("%.f",m_fParamN);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,11+iOffset,ss);
		ss.Printf("%d",m_nParamT);
		SimplifiedNumString(ss);
		pListCtrl->SetItemText(iCurSel,12+iOffset,ss);
		if(m_iInsertPlateType==2)
		{	//十字插板
			ss.Printf("%.f",m_fParaA);
			SimplifiedNumString(ss);
			pListCtrl->SetItemText(iCurSel,16,ss);
			ss.Printf("%.f",m_fParaR);
			SimplifiedNumString(ss);
			pListCtrl->SetItemText(iCurSel,17,ss);
			ss.Printf("%.2f",m_fWeight);
			SimplifiedNumString(ss);
			pListCtrl->SetItemText(iCurSel,18,ss);
		}
		else
		{	//U型插板/槽型插板
			ss.Printf("%.2f",m_fWeight);
			SimplifiedNumString(ss);
			pListCtrl->SetItemText(iCurSel,13,ss);
		}
		//将修改结果保存到表中
		if(m_iInsertPlateType==0)
		{	//槽型插板
			strncpy(rollend_param_table[iCurSel].codeName,m_sInsertPlateName,15);
			rollend_param_table[iCurSel].d=d;
			rollend_param_table[iCurSel].t=t;
			rollend_param_table[iCurSel].cMaterial=cMat;
			rollend_param_table[iCurSel].cTubeMat=cTubeMat;
			rollend_param_table[iCurSel].iLevelMark=iLevel;
			rollend_param_table[iCurSel].L=m_fParamL;
			rollend_param_table[iCurSel].M=m_fParamM;
			rollend_param_table[iCurSel].N=m_fParamN;
			rollend_param_table[iCurSel].H=m_fParamH;
			rollend_param_table[iCurSel].W=m_fParamW;
			rollend_param_table[iCurSel].S=m_fParamS;
			rollend_param_table[iCurSel].Thick=m_nParamT;
			rollend_param_table[iCurSel].theroy_weight=m_fWeight;
		}
		else if(m_iInsertPlateType==1)
		{	//U型插板
			strncpy(uend_param_table[iCurSel].codeName,m_sInsertPlateName,15);
			uend_param_table[iCurSel].d=d;
			uend_param_table[iCurSel].t=t;
			uend_param_table[iCurSel].cMaterial=cMat;
			uend_param_table[iCurSel].cTubeMat=cTubeMat;
			uend_param_table[iCurSel].iLevelMark=iLevel;
			uend_param_table[iCurSel].L=m_fParamL;
			uend_param_table[iCurSel].M=m_fParamM;
			uend_param_table[iCurSel].N=m_fParamN;
			uend_param_table[iCurSel].H=m_fParamH;
			uend_param_table[iCurSel].W=m_fParamW;
			uend_param_table[iCurSel].S=m_fParamS;
			uend_param_table[iCurSel].Thick=m_nParamT;
			uend_param_table[iCurSel].theroy_weight=m_fWeight;
		}
		else if(m_iInsertPlateType==2)
		{	//十字插板
			strncpy(xend_param_table[iCurSel].codeName,m_sInsertPlateName,15);
			strncpy(xend_param_table[iCurSel].datumPlatePartNo,m_sKPlateNo,15);
			strncpy(xend_param_table[iCurSel].weldPlatePartNo,m_sLPlateNo,15);
			strncpy(xend_param_table[iCurSel].wrapPlatePartNo,m_sZPlateNo,15);
			xend_param_table[iCurSel].d=d;
			xend_param_table[iCurSel].t=t;
			xend_param_table[iCurSel].cMaterial=cMat;
			xend_param_table[iCurSel].cTubeMat=cTubeMat;
			xend_param_table[iCurSel].iLevelMark=iLevel;
			xend_param_table[iCurSel].L=m_fParamL;
			xend_param_table[iCurSel].M=m_fParamM;
			xend_param_table[iCurSel].N=m_fParamN;
			xend_param_table[iCurSel].H=m_fParamH;
			xend_param_table[iCurSel].W=m_fParamW;
			xend_param_table[iCurSel].S=m_fParamS;
			xend_param_table[iCurSel].Thick=m_nParamT;
			xend_param_table[iCurSel].A=m_fParaA;
			xend_param_table[iCurSel].R=m_fParaR;
			xend_param_table[iCurSel].theroy_weight=m_fWeight;
		}
	}
}

void CTubeEndLjGuiGeTblDlg::OnDblclkLayOutBoltParaList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_LAYOUT_BOLT_PARA_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		CString ss;
		char sText[50];
		pListCtrl->GetItemText(iCurSel,0,sText,50);
		m_sBoltD.Format("%s",sText);
		pListCtrl->GetItemText(iCurSel,1,sText,50);
		m_nBoltNum=atoi(sText);
		pListCtrl->GetItemText(iCurSel,2,sText,50);
		m_nRowNum=atoi(sText);
		pListCtrl->GetItemText(iCurSel,3,sText,50);
		m_nColNum=atoi(sText);
		pListCtrl->GetItemText(iCurSel,4,sText,50);
		m_fHorizSpace=atof(sText);
		pListCtrl->GetItemText(iCurSel,5,sText,50);
		m_fVertiSpace=atof(sText);
		pListCtrl->GetItemText(iCurSel,6,sText,50);
		m_fBoltPosX=atof(sText);
		pListCtrl->GetItemText(iCurSel,7,sText,50);
		m_fBoltPosY=atof(sText);
		if(m_nRowNum==-1&&m_nColNum==-1)
			m_bDoubleRow=TRUE;
		else
			m_bDoubleRow=FALSE;
		if(m_iInsertPlateType==2)
		{	//十字型插板
			GetDlgItem(IDC_E_ROW_NUM)->EnableWindow(!m_bDoubleRow);
			GetDlgItem(IDC_E_COL_NUM)->EnableWindow(!m_bDoubleRow);
		}
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CTubeEndLjGuiGeTblDlg::OnDblclkInsertPlateLibList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_INSERT_PLATE_LIB_LIST);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel = pListCtrl->GetNextSelectedItem(pos);
		int iOffect=0;
		CString ss;
		double d=0, t=0;
		char sText[50],s_t[50];
		pListCtrl->GetItemText(iCurSel,0,sText,50);
		m_sInsertPlateName.Format("%s",sText);
		if(m_iInsertPlateType==2)
		{
			pListCtrl->GetItemText(iCurSel,1,sText,50);
			m_sKPlateNo.Format("%s",sText);
			pListCtrl->GetItemText(iCurSel,2,sText,50);
			m_sZPlateNo.Format("%s",sText);
			pListCtrl->GetItemText(iCurSel,3,sText,50);
			m_sLPlateNo.Format("%s",sText);
			iOffect=3;
		}
		else
			iOffect=0;
		pListCtrl->GetItemText(iCurSel,1+iOffect,sText,50);
		char cMat=sText[0];
		m_iInsertPlateMaterial=QuerySteelMatIndex(cMat);//材质
		pListCtrl->GetItemText(iCurSel,2+iOffect,sText,50);
		pListCtrl->GetItemText(iCurSel,3+iOffect,s_t,50);
		d=atof(sText);
		t=atof(s_t);
		m_sMainTubeGuiGe.Format("%sX%s",sText,s_t);
		pListCtrl->GetItemText(iCurSel,4+iOffect,sText,50);
		char cTubeMat=sText[0];
		m_iTubeMaterial=QuerySteelMatIndex(cTubeMat);//材质
		//插板级别
		pListCtrl->GetItemText(iCurSel,5+iOffect,sText,50);
		m_iInsertPlateLevel=atoi(sText)-1;	//插板级别为 1-9的数字
		pListCtrl->GetItemText(iCurSel,6+iOffect,sText,50);
		m_fParamL=atof(sText);
		pListCtrl->GetItemText(iCurSel,7+iOffect,sText,50);
		m_fParamM=atof(sText);
		pListCtrl->GetItemText(iCurSel,8+iOffect,sText,50);
		m_fParamH=atof(sText);
		pListCtrl->GetItemText(iCurSel,9+iOffect,sText,50);
		m_fParamW=atof(sText);
		pListCtrl->GetItemText(iCurSel,10+iOffect,sText,50);
		m_fParamS=atof(sText);
		pListCtrl->GetItemText(iCurSel,11+iOffect,sText,50);
		m_fParamN=atof(sText);
		pListCtrl->GetItemText(iCurSel,12+iOffect,sText,50);
		m_nParamT=atoi(sText);
		if(m_iInsertPlateType==2)
		{	//十字插板
			pListCtrl->GetItemText(iCurSel,13+iOffect,sText,50);
			m_fParaA=atof(sText);
			pListCtrl->GetItemText(iCurSel,14+iOffect,sText,50);
			m_fParaR=atof(sText);
			pListCtrl->GetItemText(iCurSel,15+iOffect,sText,50);
			m_fWeight=atof(sText);
		}
		else
		{
			pListCtrl->GetItemText(iCurSel,13,sText,50);
			m_fWeight=atof(sText);
		}
		//显示选中板的布置螺栓参数
		INSERT_PLATE_PARAM *pInsertPlateParam=NULL;
		if(m_iInsertPlateType==0)		//槽型插板
			pInsertPlateParam=FindRollEndParam(d,t,cTubeMat,m_iInsertPlateLevel+1);
		else if(m_iInsertPlateType==1)	//U型插板
			pInsertPlateParam=FindUEndParam(d,t,cTubeMat,m_iInsertPlateLevel+1);
		else if(m_iInsertPlateType==2)	//十字插板
			pInsertPlateParam=FindXEndParam(d,t,cTubeMat,m_iInsertPlateLevel+1);
		if(pInsertPlateParam)
		{
			pListCtrl=(CListCtrl*)GetDlgItem(IDC_LAYOUT_BOLT_PARA_LIST);
			pListCtrl->DeleteAllItems();
			for(int i=0;i<pInsertPlateParam->iBoltRect;i++)
			{
				CStringArray str_arr;
				str_arr.SetSize(8);
				str_arr[0].Format("%d",pInsertPlateParam->ls_param[i].iBoltD);
				str_arr[1].Format("%d",pInsertPlateParam->ls_param[i].iBoltNum);
				str_arr[2].Format("%d",pInsertPlateParam->ls_param[i].iRow);
				str_arr[3].Format("%d",pInsertPlateParam->ls_param[i].iColumn);
				str_arr[4].Format("%.f",pInsertPlateParam->ls_param[i].fHorzSpace);
				str_arr[5].Format("%.f",pInsertPlateParam->ls_param[i].fVertSpace);
				str_arr[6].Format("%.f",pInsertPlateParam->ls_param[i].fBoltPosX);
				str_arr[7].Format("%.f",pInsertPlateParam->ls_param[i].fBoltPosY);
				AppendRecord(pListCtrl,-1,str_arr);
			}
		}
		UpdateData(FALSE);
	}
	*pResult = 0;
}

void CTubeEndLjGuiGeTblDlg::DeleteItem(CListCtrl *pListCtrl,int nCurSel) 
{
	CListCtrl *pListCtrl1=(CListCtrl*)GetDlgItem(IDC_INSERT_PLATE_LIB_LIST);
	CListCtrl *pListCtrl2=(CListCtrl*)GetDlgItem(IDC_LAYOUT_BOLT_PARA_LIST);
	if(pListCtrl==pListCtrl1)
	{	//删除插板库中的记录
		if(m_iInsertPlateType==0)
		{	//槽型插板
			for(int i=nCurSel;i<rollend_param_N-1;i++)
			{	//删除第nCurSel条记录以后，后边的记录均向前移一位
				strcpy(rollend_param_table[i].codeName,rollend_param_table[i+1].codeName);
				rollend_param_table[i].cMaterial=rollend_param_table[i+1].cMaterial;
				rollend_param_table[i].cTubeMat=rollend_param_table[i+1].cTubeMat;
				rollend_param_table[i].iLevelMark=rollend_param_table[i+1].iLevelMark;	//插板级别
				rollend_param_table[i].d=rollend_param_table[i+1].d;
				rollend_param_table[i].t=rollend_param_table[i+1].t;
				rollend_param_table[i].L=rollend_param_table[i+1].L;
				rollend_param_table[i].M=rollend_param_table[i+1].M;
				rollend_param_table[i].N=rollend_param_table[i+1].N;
				rollend_param_table[i].S=rollend_param_table[i+1].S;
				rollend_param_table[i].W=rollend_param_table[i+1].W;
				rollend_param_table[i].H=rollend_param_table[i+1].H;
				rollend_param_table[i].Thick=rollend_param_table[i+1].Thick;
				rollend_param_table[i].theroy_weight=rollend_param_table[i+1].theroy_weight;
				rollend_param_table[i].iBoltRect=rollend_param_table[i+1].iBoltRect;
				for(int j=0;j<rollend_param_table[i+1].iBoltRect;j++)
					rollend_param_table[i].ls_param[j]=rollend_param_table[i+1].ls_param[j];
			}
			rollend_param_N--;
		}
		else if(m_iInsertPlateType==1)
		{	//U型插板
			for(int i=nCurSel;i<uend_param_N-1;i++)
			{	//删除第nCurSel条记录以后，后边的记录均向前移一位
				strcpy(uend_param_table[i].codeName,uend_param_table[i+1].codeName);
				uend_param_table[i].cMaterial=uend_param_table[i+1].cMaterial;
				uend_param_table[i].cTubeMat=uend_param_table[i+1].cTubeMat;
				uend_param_table[i].iLevelMark=uend_param_table[i+1].iLevelMark;	//插板级别
				uend_param_table[i].d=uend_param_table[i+1].d;
				uend_param_table[i].t=uend_param_table[i+1].t;
				uend_param_table[i].L=uend_param_table[i+1].L;
				uend_param_table[i].M=uend_param_table[i+1].M;
				uend_param_table[i].N=uend_param_table[i+1].N;
				uend_param_table[i].S=uend_param_table[i+1].S;
				uend_param_table[i].W=uend_param_table[i+1].W;
				uend_param_table[i].H=uend_param_table[i+1].H;
				uend_param_table[i].Thick=uend_param_table[i+1].Thick;
				uend_param_table[i].theroy_weight=uend_param_table[i+1].theroy_weight;
				uend_param_table[i].iBoltRect=uend_param_table[i+1].iBoltRect;
				for(int j=0;j<uend_param_table[i+1].iBoltRect;j++)
					uend_param_table[i].ls_param[j]=uend_param_table[i+1].ls_param[j];
			}
			uend_param_N--;
		}
		else if(m_iInsertPlateType==2)
		{	//十字插板
			for(int i=nCurSel;i<xend_param_N-1;i++)
			{	//删除第nCurSel条记录以后，后边的记录均向前移一位
				strcpy(xend_param_table[i].codeName,xend_param_table[i+1].codeName);
				xend_param_table[i].cMaterial=xend_param_table[i+1].cMaterial;
				strcpy(xend_param_table[i].datumPlatePartNo,xend_param_table[i+1].datumPlatePartNo);
				strcpy(xend_param_table[i].weldPlatePartNo,xend_param_table[i+1].weldPlatePartNo);
				strcpy(xend_param_table[i].wrapPlatePartNo,xend_param_table[i+1].wrapPlatePartNo);
				xend_param_table[i].cTubeMat=xend_param_table[i+1].cTubeMat;
				xend_param_table[i].iLevelMark=xend_param_table[i+1].iLevelMark;	//插板级别
				xend_param_table[i].d=xend_param_table[i+1].d;
				xend_param_table[i].t=xend_param_table[i+1].t;
				xend_param_table[i].L=xend_param_table[i+1].L;
				xend_param_table[i].M=xend_param_table[i+1].M;
				xend_param_table[i].N=xend_param_table[i+1].N;
				xend_param_table[i].S=xend_param_table[i+1].S;
				xend_param_table[i].W=xend_param_table[i+1].W;
				xend_param_table[i].H=xend_param_table[i+1].H;
				xend_param_table[i].Thick=xend_param_table[i+1].Thick;
				xend_param_table[i].theroy_weight=xend_param_table[i+1].theroy_weight;
				xend_param_table[i].iBoltRect=xend_param_table[i+1].iBoltRect;
				for(int j=0;j<3;j++)
					xend_param_table[i].ls_param[j]=xend_param_table[i+1].ls_param[j];
				xend_param_table[i].A=xend_param_table[i+1].A;
				xend_param_table[i].R=xend_param_table[i+1].R;
			}
			xend_param_N--;
		}
	}
	else//if(pListCtrl==pListCtrl2)
	{	//删除插板上的螺栓布置记录
		int iCurSel=0;
		POSITION pos = pListCtrl1->GetFirstSelectedItemPosition();
		if(pos!=NULL)	
			iCurSel = pListCtrl1->GetNextSelectedItem(pos);//当前查看的为第iCurSel条插板记录上的螺栓布置参数
		if(m_iInsertPlateType==0)
		{	//槽型插板
			for(int j=nCurSel;j<rollend_param_table[iCurSel].iBoltRect-1;j++)
			{	//删除第nCurSel条记录后，后边的所有记录均前移一位
				rollend_param_table[iCurSel].ls_param[j].iBoltNum=rollend_param_table[iCurSel].ls_param[j+1].iBoltNum;
				rollend_param_table[iCurSel].ls_param[j].iBoltD=rollend_param_table[iCurSel].ls_param[j+1].iBoltD;
				rollend_param_table[iCurSel].ls_param[j].iRow=rollend_param_table[iCurSel].ls_param[j+1].iRow;
				rollend_param_table[iCurSel].ls_param[j].iColumn=rollend_param_table[iCurSel].ls_param[j+1].iColumn;
				rollend_param_table[iCurSel].ls_param[j].fBoltPosX=rollend_param_table[iCurSel].ls_param[j+1].fBoltPosX;
				rollend_param_table[iCurSel].ls_param[j].fBoltPosY=rollend_param_table[iCurSel].ls_param[j+1].fBoltPosY;
				rollend_param_table[iCurSel].ls_param[j].fHorzSpace=rollend_param_table[iCurSel].ls_param[j+1].fHorzSpace;
				rollend_param_table[iCurSel].ls_param[j].fVertSpace=rollend_param_table[iCurSel].ls_param[j+1].fVertSpace;
			}
			rollend_param_table[iCurSel].iBoltRect--;
		}
		else if(m_iInsertPlateType==1)
		{	//U型插板
			for(int j=nCurSel;j<uend_param_table[iCurSel].iBoltRect-1;j++)
			{	//删除第nCurSel条记录后，后边的所有记录均前移一位
				uend_param_table[iCurSel].ls_param[j].iBoltNum=uend_param_table[iCurSel].ls_param[j+1].iBoltNum;
				uend_param_table[iCurSel].ls_param[j].iBoltD=uend_param_table[iCurSel].ls_param[j+1].iBoltD;
				uend_param_table[iCurSel].ls_param[j].iRow=uend_param_table[iCurSel].ls_param[j+1].iRow;
				uend_param_table[iCurSel].ls_param[j].iColumn=uend_param_table[iCurSel].ls_param[j+1].iColumn;
				uend_param_table[iCurSel].ls_param[j].fBoltPosX=uend_param_table[iCurSel].ls_param[j+1].fBoltPosX;
				uend_param_table[iCurSel].ls_param[j].fBoltPosY=uend_param_table[iCurSel].ls_param[j+1].fBoltPosY;
				uend_param_table[iCurSel].ls_param[j].fHorzSpace=uend_param_table[iCurSel].ls_param[j+1].fHorzSpace;
				uend_param_table[iCurSel].ls_param[j].fVertSpace=uend_param_table[iCurSel].ls_param[j+1].fVertSpace;
			}
			uend_param_table[iCurSel].iBoltRect--;
		}
		else if(m_iInsertPlateType==2)
		{	//十字插板
			for(int j=nCurSel;j<xend_param_table[iCurSel].iBoltRect-1;j++)
			{	//删除第nCurSel条记录后，后边的所有记录均前移一位
				xend_param_table[iCurSel].ls_param[j].iBoltNum=xend_param_table[iCurSel].ls_param[j+1].iBoltNum;
				xend_param_table[iCurSel].ls_param[j].iBoltD=xend_param_table[iCurSel].ls_param[j+1].iBoltD;
				xend_param_table[iCurSel].ls_param[j].iRow=xend_param_table[iCurSel].ls_param[j+1].iRow;
				xend_param_table[iCurSel].ls_param[j].iColumn=xend_param_table[iCurSel].ls_param[j+1].iColumn;
				xend_param_table[iCurSel].ls_param[j].fBoltPosX=xend_param_table[iCurSel].ls_param[j+1].fBoltPosX;
				xend_param_table[iCurSel].ls_param[j].fBoltPosY=xend_param_table[iCurSel].ls_param[j+1].fBoltPosY;
				xend_param_table[iCurSel].ls_param[j].fHorzSpace=xend_param_table[iCurSel].ls_param[j+1].fHorzSpace;
				xend_param_table[iCurSel].ls_param[j].fVertSpace=xend_param_table[iCurSel].ls_param[j+1].fVertSpace;
			}
			xend_param_table[iCurSel].iBoltRect--;
		}
	}
	pListCtrl->DeleteItem(nCurSel);
}

void CTubeEndLjGuiGeTblDlg::OnKeydownLayOutBoltParaList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_LAYOUT_BOLT_PARA_LIST);
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

void CTubeEndLjGuiGeTblDlg::OnKeydownInsertPlateLibList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	CListCtrl *pListCtrl=(CListCtrl*)GetDlgItem(IDC_INSERT_PLATE_LIB_LIST);
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

void CTubeEndLjGuiGeTblDlg::OnSelchangeInsertPlateType()
{
	UpdateData();
	RefreshListView();
	UpdateInsertPlateName();
	if(m_iInsertPlateType!=2)
	{
		m_sKPlateNo="";
		m_sLPlateNo="";
		m_sZPlateNo="";
		GetDlgItem(IDC_E_K_PLATE_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_Z_PLATE_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_E_L_PLATE_NO)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_E_K_PLATE_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_Z_PLATE_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_E_L_PLATE_NO)->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
}

void CTubeEndLjGuiGeTblDlg::OnChangeParamN()
{
	UpdateData();
	if(m_iInsertPlateType==2)
		m_fBoltPosY=m_fParamN*0.5;
	UpdateData(FALSE);
}

void CTubeEndLjGuiGeTblDlg::UpdateInsertPlateName()
{
	UpdateData();
	double d,t;
	int d_third,d_sec;	//直径的百位以及十位数字
	int t_sec,t_firs;	//厚度的十位以及个位数字
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	CComboBox* pCMB = (CComboBox*)GetDlgItem(IDC_CMB_MAIN_TUBE_GUIGE);
	int iCurSel = pCMB->GetCurSel();
	if(iCurSel>=0)
		pCMB->GetLBText(iCurSel,m_sMainTubeGuiGe);
	else
		pCMB->GetWindowText(m_sMainTubeGuiGe);
	//插板命令规格	第一位用来表示插板类型用C,U,X来却分	
	//				第二位取钢管直径的百位数字
	//				第三位取钢管直径的十位数字
	//				第四位取钢管厚度的十位数字
	//				第五位取钢管厚度的个位数字
	//				第六位表示主管材质 Q235-S Q345-H等
	//				第七位表示钢管的应力系数 1表示100% 5表示50% 7表示70%
	//              也可以用来区分同一规格钢管连接的不同类型的插板 
	restore_JG_guige(m_sMainTubeGuiGe.GetBuffer(),d,t);
	d_third=ftol(d)/100;	//取直径的百位数字
	d_sec=(ftol(d)%100)/10;	//取直径的十位数字
	t_sec=(ftol(t)%100)/10;	//取厚度的十位数字
	t_firs=ftol(t)%10;		//取厚度的个位数字
	CString sMaterial;
	GetDlgItem(IDC_CMB_TUBE_MATERIAL)->GetWindowText(sMaterial);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sMaterial);
	char cTubeMat=QuerySteelBriefMatMark(tem_str);
	CString sLevel;
	GetDlgItem(IDC_CMB_INSERT_PLATE_LEVEL)->GetWindowText(sLevel);
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sLevel);
	if(m_iInsertPlateType==0)
		m_sInsertPlateName.Format("C%d%d%d%d%c%s",d_third,d_sec,t_sec,t_firs,cTubeMat,tem_str);
	else if(m_iInsertPlateType==1)
		m_sInsertPlateName.Format("U%d%d%d%d%c%s",d_third,d_sec,t_sec,t_firs,cTubeMat,tem_str);
	else if(m_iInsertPlateType==2)
	{
		m_sInsertPlateName.Format("X%d%d%d%d%c%s",d_third,d_sec,t_sec,t_firs,cTubeMat,tem_str);
		m_sKPlateNo.Format("XK%d%d%d%d%c%s",d_third,d_sec,t_sec,t_firs,cTubeMat,tem_str);
		m_sLPlateNo.Format("XL%d%d%d%d%c%s",d_third,d_sec,t_sec,t_firs,cTubeMat,tem_str);
		m_sZPlateNo.Format("XZ%d%d%d%d%c%s",d_third,d_sec,t_sec,t_firs,cTubeMat,tem_str);
	}
	UpdateData(FALSE);
}

void CTubeEndLjGuiGeTblDlg::OnSelchangeMainTubeGuige()
{
	UpdateInsertPlateName();
}

void CTubeEndLjGuiGeTblDlg::OnSelchangeTubeMaterial()
{
	UpdateInsertPlateName();
}

void CTubeEndLjGuiGeTblDlg::OnDoubleRowLs() 
{
	UpdateData();
	if(m_iInsertPlateType!=2)
		return;	//非十字插板则返回
	GetDlgItem(IDC_E_COL_NUM)->EnableWindow(!m_bDoubleRow);
	GetDlgItem(IDC_E_ROW_NUM)->EnableWindow(!m_bDoubleRow);
	if(m_bDoubleRow)
		m_nColNum=m_nRowNum=-1;
	UpdateData(FALSE);
}

void CTubeEndLjGuiGeTblDlg::OnKillfocusMainTubeGuige() 
{
	UpdateInsertPlateName();
}

void CTubeEndLjGuiGeTblDlg::OnSelchangeInsertPlateLevel() 
{
	UpdateInsertPlateName();
}

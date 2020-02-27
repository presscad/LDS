//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "BodyCtrlPtDlg.h"
#include "OpenSizeDlg.h"
#include "NewFileDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CShankCtrlPtDlg dialog

CShankCtrlPtDlg::CShankCtrlPtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShankCtrlPtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShankCtrlPtDlg)
	m_fFrontOpen = 0.0;
	m_fSideOpen = 0.0;
	m_fVerticalDist = 0.0;
	m_iMainPartType = 0;
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption = _T("Define Leg Members");
	m_arrHeaderLabel.Add("Vertical Height");
	m_arrHeaderLabel.Add("Front Open");
	m_arrHeaderLabel.Add("Side Open");
#else 
	m_sCaption = _T("定义塔身主材");
	m_arrHeaderLabel.Add("垂直高度");
	m_arrHeaderLabel.Add("正面开口");
	m_arrHeaderLabel.Add("侧面开口");
#endif
}


void CShankCtrlPtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShankCtrlPtDlg)
	DDX_Text(pDX, IDC_EDIT_FRONT_OPENING, m_fFrontOpen);
	DDV_MinMaxDouble(pDX, m_fFrontOpen, 1., 100000.);
	DDX_Text(pDX, IDC_EDIT_SIDE_OPENING, m_fSideOpen);
	DDV_MinMaxDouble(pDX, m_fSideOpen, 1., 100000.);
	DDX_Text(pDX, IDC_EDIT_VERTICAL_HEIGHT, m_fVerticalDist);
	DDX_Radio(pDX, IDC_RDO_MAIN_PART_TYPE, m_iMainPartType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShankCtrlPtDlg, CDialog)
	//{{AFX_MSG_MAP(CShankCtrlPtDlg)
	ON_BN_CLICKED(IDC_BN_ADD_ITEM, OnBnAddItem)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_BODY_OPENING, OnKeydownListCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BODY_OPENING, OnDblclkListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShankCtrlPtDlg message handlers

BOOL CShankCtrlPtDlg::OnInitDialog() 
{
	SetWindowText(m_sCaption);
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_BODY_OPENING);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();
	return CDialog::OnInitDialog();
}

void CShankCtrlPtDlg::RefreshListViewHeader()
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
void CShankCtrlPtDlg::AppendRecord(int iItem, CStringArray &str_arr)
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
void CShankCtrlPtDlg::OnBnAddItem() 
{
	CString TemStr;
	CStringArray str_arr;
	UpdateData();

	TemStr.Format("%.f",m_fVerticalDist);
	str_arr.Add(TemStr);
	TemStr.Format("%.f",m_fFrontOpen);
	str_arr.Add(TemStr);
	TemStr.Format("%.f",m_fSideOpen);
	str_arr.Add(TemStr);
	if(m_pListCtrl->GetItemCount()>100)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Procedure can only handle datas within 100");
#else 
		AfxMessageBox("程序只能处理100个以内的数据");
#endif
		return;
	}
	AppendRecord(-1,str_arr);
}

void CShankCtrlPtDlg::DeleteItem(int nCurSel) 
{
	m_pListCtrl->DeleteItem(nCurSel);
}

void CShankCtrlPtDlg::ModifyItem(int nCurSel) 
{
	COpenSizeDlg dlg;
	CString sText;
	char tem_str[101];
	m_pListCtrl->GetItemText(nCurSel,0,tem_str,100);
	dlg.m_fVerticalHigh = atof(tem_str);	//垂高
	m_pListCtrl->GetItemText(nCurSel,1,tem_str,100);
	dlg.m_fFrontOpen = atof(tem_str);		//正面开口
	m_pListCtrl->GetItemText(nCurSel,2,tem_str,100);
	dlg.m_fSideOpen  = atof(tem_str);		//侧面开口
	if(dlg.DoModal()==IDOK)
	{
		sText.Format("%.f",dlg.m_fVerticalHigh);
		m_pListCtrl->SetItemText(nCurSel,0,sText);
		sText.Format("%.f",dlg.m_fFrontOpen);
		m_pListCtrl->SetItemText(nCurSel,1,sText);
		sText.Format("%.f",dlg.m_fSideOpen);
		m_pListCtrl->SetItemText(nCurSel,2,sText);
		m_pListCtrl->RedrawItems(nCurSel,nCurSel);
	}
}

/*void CShankCtrlPtDlg::OnSelchangeListBodyOpening() 
{
	char tem_str[101];
	int iCurSel = m_pListCtrl->GetHotItem();
	m_pListCtrl->GetItemText(iCurSel,0,tem_str,100);
	m_fVerticalDist = atof(tem_str);
	m_pListCtrl->GetItemText(iCurSel,1,tem_str,100);
	m_fFrontOpen = atof(tem_str);
	m_pListCtrl->GetItemText(iCurSel,2,tem_str,100);
	m_fSideOpen = atof(tem_str);
	UpdateData(FALSE);
}*/

void CShankCtrlPtDlg::OnOK() 
{
	char tem_str[101];
	OpenListNum = m_pListCtrl->GetItemCount();
	for(int i=0;i<OpenListNum;i++)
	{
		m_pListCtrl->GetItemText(i,0,tem_str,100);
		OpenList[i][0] = atof(tem_str);
		m_pListCtrl->GetItemText(i,1,tem_str,100);
		OpenList[i][1] = atof(tem_str);
		m_pListCtrl->GetItemText(i,2,tem_str,100);
		OpenList[i][2] = atof(tem_str);
	}
	
	CDialog::OnOK();
}

void CShankCtrlPtDlg::OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CShankCtrlPtDlg::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
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

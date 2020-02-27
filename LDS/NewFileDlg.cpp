//<LOCALE_TRANSLATE Confirm by hxr/>
// NewFileDlgTest.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "NewFileDlg.h"
#include "OpenSizeDlg.h"
#include "CutPlaneSetDlg.h"
#include "GlobalFunc.h"
#include "NewTowerDataOper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewFileDlg dialog


CNewFileDlg::CNewFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewFileDlg)
	m_fFrontOpen = 0;
	m_fSideOpen = 0;
	m_fVerticalDist = 0;
	m_iMainPartType = 1;
	m_iSideNum = 1;
	m_iUnit = 1;	
	//}}AFX_DATA_INIT
#ifdef AFX_TARG_ENU_ENGLISH
	m_sCaption = _T("Define Leg Member");
	m_arrHeaderLabel.Add("vertical height");
	m_arrHeaderLabel.Add("front open-size");
	m_arrHeaderLabel.Add("side open-size");
#else
	m_sCaption = _T("定义塔身主材");
	m_arrHeaderLabel.Add("垂直高度");
	m_arrHeaderLabel.Add("正面开口");
	m_arrHeaderLabel.Add("侧面开口");
#endif
	m_iOldUnit=m_iUnit;
}

void CNewFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewFileDlg)
	DDX_Text(pDX, IDC_EDIT_FRONT_OPENING, m_fFrontOpen);
	DDX_Text(pDX, IDC_EDIT_SIDE_OPENING, m_fSideOpen);
	DDX_Text(pDX, IDC_EDIT_VERTICAL_HEIGHT, m_fVerticalDist);
	DDX_Radio(pDX, IDC_RDO_MAIN_PART_TYPE, m_iMainPartType);
	DDX_CBIndex(pDX, IDC_COMBO_TOWER_SIDE_NUM, m_iSideNum);
	DDX_CBIndex(pDX, IDC_CMB_UNIT, m_iUnit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewFileDlg, CDialog)
	//{{AFX_MSG_MAP(CNewFileDlg)
	ON_BN_CLICKED(IDC_BN_ADD_ITEM, OnBnAddItem)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BODY_OPENING, OnDblclkListCtrl)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_BODY_OPENING, OnKeydownListCtrl)
	ON_CBN_SELCHANGE(IDC_COMBO_TOWER_SIDE_NUM, OnSelchangeTowerSideNum)
	ON_CBN_SELCHANGE(IDC_CMB_UNIT, OnSelchangeUnit)
	ON_EN_CHANGE(IDC_EDIT_FRONT_OPENING, OnChangeEditFrontOpening)
	ON_BN_CLICKED(IDC_BN_SWITCH_TO_STANDARD_UI, OnBtnSwitchToStandardUi)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewFileDlg message handlers

void CNewFileDlg::OnBnAddItem() 
{
	CString temStr;
	CStringArray str_arr;
	CComboBox *pCom;
	pCom = (CComboBox*)GetDlgItem(IDC_COMBO_TOWER_SIDE_NUM);
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_SIDE_OPENING);
	UpdateData();
	temStr.Format("%f",m_fVerticalDist);
	SimplifiedNumString(temStr);
	str_arr.Add(temStr);
	temStr.Format("%f",m_fFrontOpen);
	SimplifiedNumString(temStr);
	str_arr.Add(temStr);
	temStr.Format("%f",m_fSideOpen);
	SimplifiedNumString(temStr);
	str_arr.Add(temStr);
	if(m_pListCtrl->GetItemCount()>100)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Application can only handle data within 100");
#else
		AfxMessageBox("程序只能处理100个以内的数据");
#endif
		return;
	}
	AppendRecord(-1,str_arr);
	if(m_iSideNum==0)
	{
		if(m_pListCtrl->GetItemCount()>0)
			pEdit->EnableWindow(FALSE);
		else
			pEdit->EnableWindow(TRUE);
	}
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_VERTICAL_HEIGHT);
	pEdit->SetFocus();
	pEdit->SetSel(0,32);
	Invalidate();
}

void CNewFileDlg::OnOK() 
{
	UpdateData();
	UpdateFrameDef();
	CDialog::OnOK();
}

BOOL CNewFileDlg::OnInitDialog() 
{
	SetWindowText(m_sCaption);
	m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_BODY_OPENING);
	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
		| LVS_EX_HEADERDRAGDROP);
	RefreshListViewHeader();

	CWnd* pWnd = GetDlgItem(IDC_COMBO_TOWER_SIDE_NUM);
	pWnd->SetFocus();
#ifdef __TMA_
	GetDlgItem(IDC_S_PART_TYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_S_PART_TYPE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RDO_MAIN_PART_TYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RDO_MAIN_PART_TYPE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RDO_MAIN_PART_TYPE2)->EnableWindow(FALSE);
	GetDlgItem(IDC_RDO_MAIN_PART_TYPE2)->ShowWindow(SW_HIDE);
	RECT rect;
	GetDlgItem(IDC_BN_ADD_ITEM)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top-=40;
	rect.bottom-=40;
	GetDlgItem(IDC_BN_ADD_ITEM)->MoveWindow(&rect);

	GetDlgItem(IDOK)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top-=40;
	rect.bottom-=40;
	GetDlgItem(IDOK)->MoveWindow(&rect);

	GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top-=40;
	rect.bottom-=40;
	GetDlgItem(IDCANCEL)->MoveWindow(&rect);

	GetWindowRect(&rect);
	rect.bottom-=40;
	MoveWindow(&rect);
	m_iMainPartType=1;
#endif
	CDialog::OnInitDialog();
	//
	if(m_xFrameDef.GetSizeNum()>0)
		RefreshListCtrl();
	UpdateData(FALSE);
	return TRUE;
}

void CNewFileDlg::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CNewFileDlg::OnKeydownListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CNewFileDlg::RefreshListViewHeader()
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
		lvc.cx = 127;
#else 
		lvc.cx = 80;
#endif
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		m_pListCtrl->InsertColumn(i,&lvc);
	}
}
void CNewFileDlg::AppendRecord(int iItem, CStringArray &str_arr)
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


void CNewFileDlg::DeleteItem(int nCurSel) 
{
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_SIDE_OPENING);
	m_pListCtrl->DeleteItem(nCurSel);
	if(m_iSideNum==0)
	{
		if(m_pListCtrl->GetItemCount()>0)
			pEdit->EnableWindow(FALSE);
		else
			pEdit->EnableWindow(TRUE);
	}
	Invalidate(FALSE);
}

void CNewFileDlg::ModifyItem(int nCurSel) 
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

//选择的塔边数不是3或4的时候灰掉侧口边长的文本框使其不可用
void CNewFileDlg::OnSelchangeTowerSideNum() 
{
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_SIDE_OPENING);
	UpdateData();
	switch(m_iSideNum)
	{
	case 0:	//三边
		if(m_pListCtrl->GetItemCount()>0)
			pEdit->EnableWindow(FALSE);
		else
			pEdit->EnableWindow(TRUE);
		break;
	case 1:	//四边
		pEdit->EnableWindow(TRUE);
		break;
	case 2:	//五边
		pEdit->EnableWindow(FALSE);
		break;
	case 3:	//六边
		pEdit->EnableWindow(FALSE);
		break;
	case 4:	//八边
		pEdit->EnableWindow(FALSE);
		break;
	default:
		break;
	}
	Invalidate();
}

//塔边数为3时  第N(N>1)次输入的侧口边长根据第N-1次输入的数据进行计算得出
void CNewFileDlg::OnChangeEditFrontOpening() 
{
	char tem_str[101];
	int nCurItemCount = m_pListCtrl->GetItemCount();
	double fSideOpen = 0.0;
	double fFrontOpen = m_fFrontOpen;
	UpdateData();
	if(m_fFrontOpen==fFrontOpen)	//未改变数值
		return;
	
	if(m_iSideNum==0)
	{
		if(nCurItemCount>0)
		{
			m_pListCtrl->GetItemText(nCurItemCount-1,1,tem_str,100);
			fFrontOpen = atof(tem_str);
			m_pListCtrl->GetItemText(nCurItemCount-1,2,tem_str,100);
			fSideOpen = atof(tem_str);
			m_fSideOpen = (m_fFrontOpen*fSideOpen)/fFrontOpen;
		}
		else
			m_fSideOpen = m_fFrontOpen;
	}
	else
		m_fSideOpen = m_fFrontOpen;
	UpdateData(FALSE);
}

//单位
void CNewFileDlg::OnSelchangeUnit()
{
	UpdateData();
	//0.米 1.毫米
	if(m_iOldUnit!=m_iUnit)
	{
		double fScale=1;
		if(m_iOldUnit==0)
			fScale=1000;	//米==>毫米
		else 
			fScale=0.001;	//毫米==>米
		m_fFrontOpen*=fScale;
		m_fSideOpen*=fScale;
		m_fVerticalDist*=fScale;
		int nCount=m_pListCtrl->GetItemCount();
		for(int i=0;i<nCount;i++)
		{
			CString sText;
			char tem_str[101]="";
			for(int j=0;j<3;j++)
			{
				m_pListCtrl->GetItemText(i,j,tem_str,100);
				sText.Format("%f",atof(tem_str)*fScale);
				SimplifiedNumString(sText);
				m_pListCtrl->SetItemText(i,j,sText);
			}
		}
	}
	m_iOldUnit=m_iUnit;	//记录前一次使用的单位索引
	UpdateData(FALSE);
}
//
void CNewFileDlg::RefreshListCtrl()
{
	m_pListCtrl->DeleteAllItems();
	//
	CString temStr;
	for(int i=0;i<m_xFrameDef.GetSizeNum();i++)
	{
		CStringArray str_arr;
		temStr.Format("%f",m_xFrameDef.m_xOpenArr[i].m_fVH);
		SimplifiedNumString(temStr);
		str_arr.Add(temStr);
		temStr.Format("%f",m_xFrameDef.m_xOpenArr[i].m_fFW);
		SimplifiedNumString(temStr);
		str_arr.Add(temStr);
		temStr.Format("%f",m_xFrameDef.m_xOpenArr[i].m_fSW);
		SimplifiedNumString(temStr);
		str_arr.Add(temStr);
		AppendRecord(-1,str_arr);
	}
}
//
void CNewFileDlg::UpdateFrameDef()
{
	char tem_str[101]="";
	double fScale=(m_iUnit==0)?1000:1;	//输出数据时统一按毫米处理
	CFrameDefine::OPEN_SIZE xSize;
	int nNum=m_pListCtrl->GetItemCount();
	m_xFrameDef.Empty();
	m_xFrameDef.m_iMainPartType=m_iMainPartType;
	m_xFrameDef.m_xOpenArr.SetSize(0,nNum);
	for(int i=0;i<nNum;i++)
	{
		m_pListCtrl->GetItemText(i,0,tem_str,100);
		xSize.m_fVH=atof(tem_str)*fScale;
		m_pListCtrl->GetItemText(i,1,tem_str,100);
		xSize.m_fFW=atof(tem_str)*fScale;
		m_pListCtrl->GetItemText(i,2,tem_str,100);
		xSize.m_fSW=atof(tem_str)*fScale;
		//
		m_xFrameDef.m_xOpenArr.append(xSize);
	}
}
//切换对话框
void CNewFileDlg::OnBtnSwitchToStandardUi()
{
	UpdateFrameDef();
	CNewTowerDataOper::m_bSwitchMode=TRUE;
	return CDialog::OnCancel();
}

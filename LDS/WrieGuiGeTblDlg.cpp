// WrieGuiGeTblDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "WrieGuiGeTblDlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWrieGuiGeTblDlg dialog
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	CWrieGuiGeTblDlg *pWrieGuiGeTblDlg=(CWrieGuiGeTblDlg*)pListCtrl->GetParent();
	CString val;
	if (pSelItem)
		val = pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	if (val.CompareNoCase(sTextValue)!=0)
		pWrieGuiGeTblDlg->m_bDataChange = TRUE;
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	int num = pListCtrl->GetItemCount();
	if (pSelItem->m_idProp==num && sTextValue.GetLength()!=0)
	{
		CSuperGridCtrl::CTreeItem* pItemArr;
		CString itemText = "";
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->AddSubItemText(itemText,TRUE);
		CSuperGridCtrl::CTreeItem* pLastItem = pListCtrl->GetTreeItem(num-1);
		pItemArr = pListCtrl->InsertRootItem(lpRoot);
		pItemArr->m_idProp=num+1;
		for (int j=0;j<7;j++)
			pListCtrl->SetSubItemText(pItemArr,j,itemText);
		pItemArr->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
	}
	return TRUE;
}
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CWrieGuiGeTblDlg *pNodeLoadListDlg=(CWrieGuiGeTblDlg*)pListCtrl->GetParent();
	if(pNodeLoadListDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	int nCurRow = pListCtrl->GetSelectedItem();
	int nCurCol = pListCtrl->GetCurSubItem();
	CRect rectItem;
	pListCtrl->GetItemRect(nCurRow,rectItem,LVIR_BOUNDS);
	if(!rectItem.PtInRect(point))
		return FALSE;	//鼠标不在当前选中项上
	if (nCurCol>=0)
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"粘贴");
	if (nCurCol==0)
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除此型号");
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pNodeLoadListDlg);
	return TRUE;
}

void CWrieGuiGeTblDlg::OnPasteData()
{
	m_listCtrl.PasteDataFromClipboard(9,NULL,TRUE);
}

void CWrieGuiGeTblDlg::OnDelData()//需要调试
{
	CXhChar100 wire;
	m_listCtrl.GetItemText(m_listCtrl.GetSelectedItem(),0,wire,100);
	char code[100] = "";
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_WIRE_TYPE);
	pCMB->GetWindowText(code,100);
	db.DelWire(code,wire);
	m_listCtrl.DeleteItem(m_listCtrl.GetSelectedItem());
}

CWrieGuiGeTblDlg::CWrieGuiGeTblDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWrieGuiGeTblDlg::IDD, pParent)
{
	m_sWireType = _T("GJ");
	m_listCtrl.m_arrHeaderLabel.Add("规格型号");
	m_listCtrl.m_arrHeaderLabel.Add("外径(mm)");
	m_listCtrl.m_arrHeaderLabel.Add("截面积(mm2)");
	m_listCtrl.m_arrHeaderLabel.Add("单位长重量(kg/km)");
	m_listCtrl.m_arrHeaderLabel.Add("额定抗拉力(N)");
	m_listCtrl.m_arrHeaderLabel.Add("弹性系数(MPa)");
	m_listCtrl.m_arrHeaderLabel.Add("温度线膨胀系数");
	//{{AFX_DATA_INIT(CWrieGuiGeTblDlg)
	m_sWireType = _T("");
	//}}AFX_DATA_INIT
}


void CWrieGuiGeTblDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWrieGuiGeTblDlg)
	DDX_Control(pDX, IDC_LIST_WIRE_PARAS, m_listCtrl);
	DDX_CBString(pDX, IDC_CMB_WIRE_TYPE, m_sWireType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWrieGuiGeTblDlg, CDialog)
	//{{AFX_MSG_MAP(CWrieGuiGeTblDlg)
	ON_WM_CANCELMODE()
	ON_CBN_SELCHANGE(IDC_CMB_WIRE_TYPE, OnSelchangeCmbWireType)
	ON_BN_CLICKED(IDC_BN_SAVE_WIRE_TYPE, OnBnSaveWireType)
	ON_BN_CLICKED(IDC_BN_DEL_WIRE_TYPE, OnBnDelWireType)
	ON_COMMAND(ID_PASTE_ITEM, OnPasteData)
	ON_COMMAND(ID_DELETE_ITEM, OnDelData)
	ON_BN_CLICKED(IDOK2, OnOk2)
	ON_CBN_EDITCHANGE(IDC_CMB_WIRE_TYPE, OnEditchangeCmbWireType)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DELETEALL, OnDeleteall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWrieGuiGeTblDlg message handlers

BOOL CWrieGuiGeTblDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char file_path[MAX_PATH];
	GetSysPath(file_path);
	strcat(file_path,"wireload.d");
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_WIRE_TYPE);
	m_bDataChange = FALSE;
	BOOL bFirst = TRUE;
	long col_wide_arr[9]={200,80,100,140,120,115,120};
	m_listCtrl.InitListCtrl(col_wide_arr);
	m_listCtrl.SetContextMenuFunc(FireContextMenu);
	m_listCtrl.SetModifyValueFunc(FireValueModify);
	if(db.OpenDbFile(file_path))
	{
		char wr[200];
		BOOL finded=db.QueryFirstWireType(wr);
		while(finded)
		{
			pCMB->AddString(wr);
			if (bFirst)
			{
				pCMB->SetWindowText(wr);
				DIANXIAN_STRU wire;
				CSuperGridCtrl::CTreeItem* pItemArr[100]={NULL};
				int i=0;
				BOOL findIt=db.QueryFirstWire(wr,&wire);
				while(findIt)
				{
					CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
					lpRoot->SetSubItemColor(0,RGB(192,192,192));
					lpRoot->AddSubItemText(wire.specWire,TRUE);
					pItemArr[i] = m_listCtrl.InsertRootItem(lpRoot);
					pItemArr[i]->m_idProp=i+1;
					CString itemText;
					//外径
					itemText.Format("%f",wire.m_fDiameter);
					SimplifiedNumString(itemText);
					m_listCtrl.SetSubItemText(pItemArr[i],1,itemText);
					pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
					//截面积
					itemText.Format("%f",wire.m_fArea);
					SimplifiedNumString(itemText);
					m_listCtrl.SetSubItemText(pItemArr[i],2,itemText);
					pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
					//单位长重量
					itemText.Format("%f",wire.m_fUnitWeight);
					SimplifiedNumString(itemText);
					m_listCtrl.SetSubItemText(pItemArr[i],3,itemText);
					pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
					//额定抗拉力
					itemText.Format("%f",wire.m_fForcePermit);
					SimplifiedNumString(itemText);
					m_listCtrl.SetSubItemText(pItemArr[i],4,itemText);
					pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
					//弹性系数
					itemText.Format("%f",wire.m_fElasticCoef);
					SimplifiedNumString(itemText);
					m_listCtrl.SetSubItemText(pItemArr[i],5,itemText);
					pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
					//温度线膨胀系数
					itemText.Format("%f",wire.m_fExpansionCoef);
					SimplifiedNumString(itemText);
					m_listCtrl.SetSubItemText(pItemArr[i],6,itemText);
					pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
					i++;
					findIt=db.QueryNextWire(&wire);
					if(findIt==0)
					{
						itemText = "";
						lpRoot = new CListCtrlItemInfo();
						lpRoot->AddSubItemText(itemText,TRUE);
						pItemArr[i] = m_listCtrl.InsertRootItem(lpRoot);
						pItemArr[i]->m_idProp=i+1;
						for (int j=0;j<7;j++)
							m_listCtrl.SetSubItemText(pItemArr[i],j,itemText);
						pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
					}
				}
				bFirst = FALSE;
			}
			finded=db.QueryNextWireType(wr);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWrieGuiGeTblDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CWrieGuiGeTblDlg::UpdateWireInfoList(char* wireType)
{
	m_listCtrl.DeleteAllItems();
	char wr[200];
	BOOL finded=db.QueryFirstWireType(wr);
	BOOL bFind = FALSE;
	while (finded)
	{
		if (strcmp(wr,wireType)==0)
		{
			bFind = TRUE;
			DIANXIAN_STRU wire;
			CSuperGridCtrl::CTreeItem* pItemArr[100]={NULL};
			int i=0;
			BOOL findIt=db.QueryFirstWire(wr,&wire);
			while(findIt)
			{
				CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
				lpRoot->SetSubItemColor(0,RGB(192,192,192));
				lpRoot->AddSubItemText(wire.specWire,TRUE);
				pItemArr[i] = m_listCtrl.InsertRootItem(lpRoot);
				pItemArr[i]->m_idProp=i+1;
				CString itemText;
				//外径
				itemText.Format("%f",wire.m_fDiameter);
				SimplifiedNumString(itemText);
				m_listCtrl.SetSubItemText(pItemArr[i],1,itemText);
				pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//截面积
				itemText.Format("%f",wire.m_fArea);
				SimplifiedNumString(itemText);
				m_listCtrl.SetSubItemText(pItemArr[i],2,itemText);
				pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//单位长重量
				itemText.Format("%f",wire.m_fUnitWeight);
				SimplifiedNumString(itemText);
				m_listCtrl.SetSubItemText(pItemArr[i],3,itemText);
				pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//额定抗拉力
				itemText.Format("%f",wire.m_fForcePermit);
				SimplifiedNumString(itemText);
				m_listCtrl.SetSubItemText(pItemArr[i],4,itemText);
				pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//弹性系数
				itemText.Format("%f",wire.m_fElasticCoef);
				SimplifiedNumString(itemText);
				m_listCtrl.SetSubItemText(pItemArr[i],5,itemText);
				pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				//温度线膨胀系数
				itemText.Format("%f",wire.m_fExpansionCoef);
				SimplifiedNumString(itemText);
				m_listCtrl.SetSubItemText(pItemArr[i],6,itemText);
				pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				i++;
				findIt=db.QueryNextWire(&wire);
				if(findIt==0)
				{
					itemText = "";
					lpRoot = new CListCtrlItemInfo();
					lpRoot->AddSubItemText(itemText,TRUE);
					pItemArr[i] = m_listCtrl.InsertRootItem(lpRoot);
					pItemArr[i]->m_idProp=i+1;
					for (int j=0;j<7;j++)
						m_listCtrl.SetSubItemText(pItemArr[i],j,itemText);
					pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
				}
			}
		}
		finded=db.QueryNextWireType(wr);
	}
	if(!bFind)
	{
		CString itemText = "";
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->AddSubItemText(itemText,TRUE);
		CSuperGridCtrl::CTreeItem* pItemArr = m_listCtrl.InsertRootItem(lpRoot);
		pItemArr->m_idProp=1;
		for (int j=0;j<7;j++)
			m_listCtrl.SetSubItemText(pItemArr,j,itemText);
		pItemArr->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
	}
}

void CWrieGuiGeTblDlg::OnSelchangeCmbWireType() 
{
	if (m_bDataChange)
	{
		if (IDOK==MessageBox("存在未保存的数据，是否保存","注意!!!",MB_OKCANCEL))
			OnBnSaveWireType();
	}
	UpdateData();
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_WIRE_TYPE);
	CXhChar200 wireType;
	pCMB->GetLBText(pCMB->GetCurSel(),wireType);
	if (m_sWireType == wireType)
		return;
	db.CloseAllSubEntry();
	UpdateWireInfoList(wireType);
}


void CWrieGuiGeTblDlg::OnBnSaveWireType() 
{
	// TODO: Add your control notification handler code here
	m_bDataChange = FALSE;
	char code[100] = "";
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_WIRE_TYPE);
	pCMB->GetWindowText(code,100);
	int i = pCMB->GetCount();
	BOOL bSame = FALSE;
	int num = m_listCtrl.GetItemCount();
	for (int j=0;j<i;j++)
	{
		char str[100] = "";
		pCMB->GetLBText(j,str);
		if (strcmp(str,code)==0)
		{
			db.DelWireType(code);
			bSame=TRUE;
		}
	}
	char strWire[100] = "";
	for (int k=0;k<num;k++)
	{
		DIANXIAN_STRU wire;
		m_listCtrl.GetItemText(k,1,strWire,100);
		wire.m_fDiameter = atof(strWire);
		m_listCtrl.GetItemText(k,2,strWire,100);
		wire.m_fArea = atof(strWire);
		m_listCtrl.GetItemText(k,3,strWire,100);
		wire.m_fUnitWeight = atof(strWire);
		m_listCtrl.GetItemText(k,4,strWire,100);
		wire.m_fForcePermit = atof(strWire);
		m_listCtrl.GetItemText(k,5,strWire,100);
		wire.m_fElasticCoef = atof(strWire);
		m_listCtrl.GetItemText(k,6,strWire,100);
		wire.m_fExpansionCoef = atof(strWire);
		m_listCtrl.GetItemText(k,0,strWire,100);
		if (strlen(strWire)!=0)
		{
			strcpy(wire.specWire,strWire);
			db.SetWire(code,strWire,wire);
		}
	}
	if (!bSame)
		pCMB->AddString(code);
}

void CWrieGuiGeTblDlg::OnBnDelWireType() 
{
	// TODO: Add your control notification handler code here
	char code[100] = "";
	CComboBox *pCMB=(CComboBox*)GetDlgItem(IDC_CMB_WIRE_TYPE);
	pCMB->GetWindowText(code,100);
	db.DelWireType(code);
	pCMB->DeleteString(pCMB->GetCurSel());
	pCMB->UpdateData();
	if (pCMB->GetCount()>0)
	{
		char sCode[100] = "";
		pCMB->SetCurSel(0);
		pCMB->GetWindowText(sCode,100);
		UpdateWireInfoList(sCode);
	}	
}

void CWrieGuiGeTblDlg::OnOk2() 
{
	if (m_bDataChange)
	{
		if (IDOK==MessageBox("存在未保存的数据，是否保存","注意!!!",MB_OKCANCEL))
			OnBnSaveWireType();
	}
	else
		m_bDataChange = FALSE;
	CDialog::OnOK();
}

void CWrieGuiGeTblDlg::OnEditchangeCmbWireType() 
{
	// TODO: Add your control notification handler code here
	if (m_listCtrl.GetItemCount()==0)
	{
		CString itemText = "";
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->AddSubItemText(itemText,TRUE);
		CSuperGridCtrl::CTreeItem* pItemArr = m_listCtrl.InsertRootItem(lpRoot);
		pItemArr->m_idProp=1;
		for (int j=0;j<7;j++)
			m_listCtrl.SetSubItemText(pItemArr,j,itemText);
		pItemArr->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
	}
}

void CWrieGuiGeTblDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bDataChange)
	{
		if (IDOK==MessageBox("存在未保存的数据，是否保存","注意!!!",MB_OKCANCEL))
			OnBnSaveWireType();
	}
	else
		m_bDataChange = FALSE;
	CDialog::OnOK();
}

void CWrieGuiGeTblDlg::OnDeleteall() 
{
	// TODO: Add your control notification handler code here
	m_listCtrl.DeleteAllItems();
	CString itemText = "";
	CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
	lpRoot->AddSubItemText(itemText,TRUE);
	CSuperGridCtrl::CTreeItem* pItemArr = m_listCtrl.InsertRootItem(lpRoot);
	pItemArr->m_idProp=1;
	for (int j=0;j<7;j++)
		m_listCtrl.SetSubItemText(pItemArr,j,itemText);
	pItemArr->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
}

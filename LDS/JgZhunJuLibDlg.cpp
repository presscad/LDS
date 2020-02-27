// JgGuiGeLibDlg.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "lds.h"
#include "JgZhunJuLibDlg.h"
#include "XHCharString.h"
#include "database.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJgZhunJuLibDlg dialog
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CJgZhunJuLibDlg *pTubeGuiGeLibDlg=(CJgZhunJuLibDlg*)pListCtrl->GetParent();
	if(pTubeGuiGeLibDlg==NULL)
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
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"Stick");
#else 
		pMenu->AppendMenu(MF_STRING,ID_PASTE_ITEM,"粘贴");
#endif
	if (nCurCol==0)
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Specification");
#else 
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除此规格");
#endif
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pTubeGuiGeLibDlg);
	return TRUE;
}

static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	if(sTextValue=="0"&&iSubItem==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(NULL,"Data can't be 0","ERROR",MB_OK);	
#else 
		MessageBox(NULL,"数据不能为0","ERROR",MB_OK);	
#endif
		return FALSE;
	}
	CString oldText = pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	if (oldText.CompareNoCase(sTextValue)==0)
		return FALSE;
	if(iSubItem>0)
	{
		CString sSelectD = pSelItem->m_lpNodeInfo->GetSubItemText(0);
		CString sSelectT = pSelItem->m_lpNodeInfo->GetSubItemText(1);
		if (sSelectD == "")
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox(NULL,"Please firstly fill in the leg width/stadia","ERROR",MB_OK);
#else 
			MessageBox(NULL,"请先填写肢宽/准距","ERROR",MB_OK);
#endif
			return FALSE;
		}
	}
	CJgZhunJuLibDlg *pTubeGuiGeLibDlg=(CJgZhunJuLibDlg*)pListCtrl->GetParent();
	int num = pListCtrl->GetItemCount();
		pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	//检验是否已经存在
	if (iSubItem==0)
	{
		CString sSelectD = pSelItem->m_lpNodeInfo->GetSubItemText(0);
		if (sSelectD != "")
		{
			for (int i=0;i<num-2;i++)//不和自己比较也不和最后的空数据比较
			{
				CXhChar100 sD; 
				pListCtrl->GetItemText(i,0,sD,100);
				if (sSelectD.CompareNoCase(sD)==0)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"The specification is already exists","ERROR",MB_OK);
#else 
					MessageBox(NULL,"此规格已经存在","ERROR",MB_OK);
#endif
					CString sSelectD = "";
					pListCtrl->SetSubItemText(pSelItem,iSubItem,sSelectD);
					return FALSE;
				}
			}
		}
	}
	long nIndex = pSelItem->m_idProp-1;
	if (pSelItem->m_idProp==num && sTextValue!="")
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

void CJgZhunJuLibDlg::OnPasteData()
{
	m_JgZhunjuList.PasteDataFromClipboard(7,NULL,TRUE);
}

void CJgZhunJuLibDlg::OnDelData()//需要调试
{
	int culItem = m_JgZhunjuList.GetSelectedItem();
	m_JgZhunjuList.DeleteItem(culItem);
	m_JgZhunjuList.UpdateData();
}

CJgZhunJuLibDlg::CJgZhunJuLibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJgZhunJuLibDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJgZhunJuLibDlg)
#ifdef AFX_TARG_ENU_ENGLISH
	m_JgZhunjuList.AddColumnHeader("Leg Wide/G");
#else 
	m_JgZhunjuList.AddColumnHeader("肢宽/准距");
#endif
	m_JgZhunjuList.AddColumnHeader("g");
	m_JgZhunjuList.AddColumnHeader("g1");
	m_JgZhunjuList.AddColumnHeader("g2");
	m_JgZhunjuList.AddColumnHeader("g3");
	//}}AFX_DATA_INIT
}


void CJgZhunJuLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJgZhunJuLibDlg)
	DDX_Control(pDX, IDC_LIST_JG_LIB, m_JgZhunjuList);
	DDX_Control(pDX, IDC_BN_IMPORT, m_Import);
	DDX_Control(pDX, IDC_BN_EXPORT, m_Export);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJgZhunJuLibDlg, CDialog)
	//{{AFX_MSG_MAP(CJgZhunJuLibDlg)
	ON_COMMAND(ID_PASTE_ITEM, OnPasteData)
	ON_COMMAND(ID_DELETE_ITEM, OnDelData)
	ON_BN_CLICKED(IDC_BN_IMPORT, OnBnImport)
	ON_BN_CLICKED(IDC_BN_EXPORT, OnBnExport)
	ON_BN_CLICKED(IDC_CANNEL, OnCannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJgZhunJuLibDlg message handlers

BOOL CJgZhunJuLibDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CDialog::OnInitDialog();
	long col_wide_arr[5]={89,86,86,86,86};
	m_JgZhunjuList.InitListCtrl(col_wide_arr);
	m_JgZhunjuList.SetContextMenuFunc(FireContextMenu);
	m_JgZhunjuList.SetModifyValueFunc(FireValueModify);
	Refresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJgZhunJuLibDlg::OnBnImport() 
{
	// TODO: Add your control notification handler code here
	CString sFileName;
	CFileDialog dlg(TRUE,"jgg","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Angle Stadia File(*.jgg)|*.jgg||");
#else 
		"角钢准距文件(*.jgg)|*.jgg||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		JgZhunJuSerialize(sFileName,FALSE);	
		Refresh();
	}
}

void CJgZhunJuLibDlg::OnBnExport() 
{
	// TODO: Add your control notification handler code here
	WriteDataBase();
	CString sFileName;
	CFileDialog dlg(FALSE,"jgg","Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
#ifdef AFX_TARG_ENU_ENGLISH
		"Angle Stadia File(*.jgg)|*.jgg||");
#else 
		"角钢准距文件(*.jgg)|*.jgg||");
#endif
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		JgZhunJuSerialize(sFileName,TRUE);
	}
}

BOOL CJgZhunJuLibDlg::Refresh()
{
	CString sText,sError;
	try
	{
		if (m_JgZhunjuList.GetItemCount()>0)
			m_JgZhunjuList.DeleteAllItems();
		CSuperGridCtrl::CTreeItem* pItemArr[100]={NULL};
		int i=0;
		CString itemText;
		for (i=0;i<zhunju_N;i++)
		{
			CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
			lpRoot->SetSubItemColor(0,RGB(192,192,192));
			//直径
			itemText.Format("%d",zhunju_table[i].wide);
			SimplifiedNumString(itemText);
			lpRoot->AddSubItemText(itemText,TRUE);
			pItemArr[i] = m_JgZhunjuList.InsertRootItem(lpRoot);
			pItemArr[i]->m_idProp=i+1;
			//壁厚
			itemText.Format("%d",zhunju_table[i].g);
			SimplifiedNumString(itemText);
			m_JgZhunjuList.SetSubItemText(pItemArr[i],1,itemText);
			pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
			//截面积
			itemText.Format("%d",zhunju_table[i].g1);
			SimplifiedNumString(itemText);
			m_JgZhunjuList.SetSubItemText(pItemArr[i],2,itemText);
			pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
			//理论重量
			itemText.Format("%d",zhunju_table[i].g2);
			SimplifiedNumString(itemText);
			m_JgZhunjuList.SetSubItemText(pItemArr[i],3,itemText);
			pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
			//惯性距
			itemText.Format("%d",zhunju_table[i].g3);
			SimplifiedNumString(itemText);
			m_JgZhunjuList.SetSubItemText(pItemArr[i],4,itemText);
			pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
		}
		itemText = "";
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->AddSubItemText(itemText,TRUE);
		pItemArr[i] = m_JgZhunjuList.InsertRootItem(lpRoot);
		pItemArr[i]->m_idProp=i+1;
		for (int j=0;j<5;j++)
			m_JgZhunjuList.SetSubItemText(pItemArr[i],j,itemText);
		pItemArr[i]->m_lpNodeInfo->SetSubItemReadOnly(0,FALSE);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}

void CJgZhunJuLibDlg::WriteDataBase()
{
	int nNum = m_JgZhunjuList.GetItemCount();
	zhunju_N = nNum-1;
	for (int i=0;i<zhunju_N;i++)
	{
		CXhChar100 sText;
		m_JgZhunjuList.GetItemText(i,0,sText,100);
		zhunju_table[i].wide = atoi(sText);
		m_JgZhunjuList.GetItemText(i,1,sText,100);
		zhunju_table[i].g = atoi(sText);
		m_JgZhunjuList.GetItemText(i,2,sText,100);
		zhunju_table[i].g1 = atoi(sText);
		m_JgZhunjuList.GetItemText(i,3,sText,100);
		zhunju_table[i].g2 = atoi(sText);
		m_JgZhunjuList.GetItemText(i,4,sText,100);
		zhunju_table[i].g3 = atoi(sText);
	}
}

void CJgZhunJuLibDlg::OnOK() 
{
	// TODO: Add extra validation here
	WriteDataBase();
	CDialog::OnOK();
}

void CJgZhunJuLibDlg::OnCannel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

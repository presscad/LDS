//<LOCALE_TRANSLATE/>
// SumTaMatTblDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SumTaMatTblCfgDlg.h"
#include "GlobalFunc.h"
#include "RxTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSumTaMatTblCfgDlg dialog


CSumTaMatTblCfgDlg::CSumTaMatTblCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSumTaMatTblCfgDlg::IDD, pParent)
{
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrl.AddColumnHeader("module name");
	m_listCtrl.AddColumnHeader("tower body seg num");
	m_listCtrl.AddColumnHeader("tower leg seg num");
	m_listCtrl.AddColumnHeader("tower leg amount");
#else
	m_listCtrl.AddColumnHeader("呼高名称");
	m_listCtrl.AddColumnHeader("塔身段号");
	m_listCtrl.AddColumnHeader("塔腿段号");
	m_listCtrl.AddColumnHeader("塔腿个数");
#endif
}


void CSumTaMatTblCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SUMTAMAT, m_listCtrl);
	//{{AFX_DATA_MAP(CSumTaMatTblCfgDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSumTaMatTblCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CSumTaMatTblCfgDlg)
	ON_WM_CANCELMODE()
	ON_COMMAND(ID_NEW_TOWERBODY,OnNewTowerBody)
	ON_COMMAND(ID_NEW_TOWERLEG,OnNewLeg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSumTaMatTblCfgDlg message handlers
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CSumTaMatTblCfgDlg *pNewTowerDlg=(CSumTaMatTblCfgDlg*)pListCtrl->GetParent();
	if(pNewTowerDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_SCRIPT);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	if(pMenu==NULL)
		return FALSE;
	while(pMenu->GetMenuItemCount()>0)
		pMenu->DeleteMenu(0,MF_BYPOSITION);
	BOOL bInSelItemRect=FALSE;
	if(pSelItem)
	{
		CRect rectItem;
		pListCtrl->GetItemRect(pSelItem->GetIndex(),rectItem,LVIR_BOUNDS);
		if(rectItem.PtInRect(point))
			bInSelItemRect=TRUE;	//鼠标不在当前选中项上
	}
	if(!bInSelItemRect)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_TOWERBODY,"new tower body data");
#else
		pMenu->AppendMenu(MF_STRING,ID_NEW_TOWERBODY,"新建本体数据");
#endif
	//	pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"导入塔身定义文件");
	//	pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"导出塔身定义文件");
	}
	else //if(bInSelItemRect)
	{
		if(pSelItem==NULL)
			return FALSE;
		LISTITEM_TYPE data_type=(LISTITEM_TYPE)pSelItem->m_idProp;
		if(data_type==DATA_TOWER) //塔身
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_NEW_TOWERLEG,"new tower leg");
			pMenu->AppendMenu(MF_STRING,ID_NEW_TOWERBODY,"new tower body data");
#else
			pMenu->AppendMenu(MF_STRING,ID_NEW_TOWERLEG,"新建塔腿");
			pMenu->AppendMenu(MF_STRING,ID_NEW_TOWERBODY,"新建本体数据");
#endif
		}
		else if (data_type==DATA_LEG)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_NEW_TOWERBODY,"new tower body data");
#else
			pMenu->AppendMenu(MF_STRING,ID_NEW_TOWERBODY,"新建本体数据");
#endif
		}
		else
			return FALSE;
	}
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pNewTowerDlg);
	return TRUE;
}
//
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{	
	pSelItem->m_lpNodeInfo->SetSubItemText(iSubItem,sTextValue);
	return TRUE;
}

BOOL CSumTaMatTblCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
#ifdef AFX_TARG_ENU_ENGLISH
	long col_wide_arr[4]={110,145,135,130};
#else 
	long col_wide_arr[4]={80,80,80,80};
#endif
	m_listCtrl.InitImageList(IDR_SUM_MAT_CFG);//-1 
	m_listCtrl.InitListCtrl(col_wide_arr);
	m_listCtrl.SetContextMenuFunc(FireContextMenu);
	m_listCtrl.SetModifyValueFunc(FireValueModify);
	int n = Ta.AttachBuffer[CTower::BOM_CONFIG].GetLength();
	if (n>0)
	{
		char sBufferType[50]="";
		Ta.AttachBuffer[CTower::BOM_CONFIG].SeekToBegin();
		Ta.AttachBuffer[CTower::BOM_CONFIG].ReadString(sBufferType);
		//尝试读取数据，检查保存的数据是否为材料汇总配置文件 wht 11-01-10
		if(strcmp(sBufferType,"材料汇总配置文件")==0)
		{
			char *pStr = new char[n];
			Ta.AttachBuffer[CTower::BOM_CONFIG].ReadString(pStr);
			CMarkup xml;
			xml.Load("",pStr);
			delete [] pStr;
			xml.ResetMainPos();
			if(xml.FindElem("TOWERSET"))
			{
				m_listCtrl.DeleteAllItems();
				ImprotXMLChildElemToList(xml,DATA_TOWER,NULL);
			}
		}
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("unexpected material summary configuration file format!");
#else
			AfxMessageBox("非预期的材料汇总配置文件格式!");
#endif
	}
	return TRUE;
}

void CSumTaMatTblCfgDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
}

void CSumTaMatTblCfgDlg::OnNewTowerBody()
{
	CListCtrlItemInfo* lp = new CListCtrlItemInfo();
	lp->SetImage(2);
	lp->AddSubItemText(_T(""));
	lp->AddSubItemText(_T(""));
	CSuperGridCtrl::CTreeItem * pRoot = m_listCtrl.InsertRootItem(lp);
	if( pRoot == NULL )
		return;	
	m_listCtrl.SetItemState(pRoot->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
	pRoot->m_idProp=DATA_TOWER;
}

void CSumTaMatTblCfgDlg::OnNewLeg()
{
	int iCurSel=m_listCtrl.GetSelectedItem();
	if(iCurSel<0)
		return;
	CSuperGridCtrl::CTreeItem *pCurItem=m_listCtrl.GetTreeItem(iCurSel);
	if(pCurItem==NULL)
		return;
	if(pCurItem->GetChildCount()==4)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("there can be four different height legs on every tower footing!");
#else
		AfxMessageBox("每基塔最多允许有四个不同高度的腿!");
#endif
		return;
	}
	CListCtrlItemInfo* lp = new CListCtrlItemInfo();
	lp->SetImage(3);
	CString sValue=_T("");
	lp->AddSubItemText(_T(""));
	lp->SetSubItemText(2,_T(""));
	lp->SetSubItemText(3,_T("4"));	//默认4个腿
	CSuperGridCtrl::CTreeItem * pSonItem = m_listCtrl.InsertItem(pCurItem,lp,-1,true);
	if( pSonItem)
	{
		m_listCtrl.SetItemState(pSonItem->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
		pSonItem->m_idProp=DATA_LEG;	
	}
}

CSuperGridCtrl::CTreeItem *CSumTaMatTblCfgDlg::ExprotListChildItemToXML(
CMarkup &xml,LISTITEM_TYPE data_type,CSuperGridCtrl::CTreeItem *pParentItem)
{
	CSuperGridCtrl::CTreeItem *pSonItem=NULL;
	if(pParentItem==NULL&&data_type!=DATA_TOWER)
		return NULL;
	POSITION pos=NULL;
	if(data_type==DATA_TOWER)
		pos=m_listCtrl.GetRootHeadPosition();
	else 
		pos=m_listCtrl.GetHeadPosition(pParentItem);
	xml.IntoElem();
	while(pos)
	{
		if(DATA_TOWER==data_type)
			pSonItem=(CSuperGridCtrl::CTreeItem*)m_listCtrl.GetNextRoot(pos); 
		else 
			pSonItem=(CSuperGridCtrl::CTreeItem*)m_listCtrl.GetNextChild(pParentItem,pos);
		if(pSonItem==NULL)
			continue;
		LISTITEM_TYPE nCurItemType=(LISTITEM_TYPE)pSonItem->m_idProp;
		if(nCurItemType!=data_type)
			continue;	//类型不匹配
		CListCtrlItemInfo* lp = pSonItem->m_lpNodeInfo;
		if(lp==NULL)
			continue;
		int i=0;
		CString sTextValue=_T("");
		if(data_type==DATA_TOWER)
		{	//坡段数据
			xml.AddElem("TOWER");
			for(i=0;i<lp->GetItemCount();i++)
			{
				if(i==0)
					xml.AddAttrib("high",lp->GetSubItemText(i));
				else if(i==1)
					xml.AddAttrib("seg",lp->GetSubItemText(i));
				else 
					break;
			}
			//导出子项 DATA_LEG
			ExprotListChildItemToXML(xml,DATA_LEG,pSonItem);	
		}
		else if(data_type==DATA_LEG)
		{	//坡段内分段数据
			xml.AddElem("LEG");
			xml.AddAttrib("name",lp->GetSubItemText(0));
			xml.AddAttrib("seg",lp->GetSubItemText(2));
			xml.AddAttrib("coef",lp->GetSubItemText(3));
		}
	}
	xml.OutOfElem();
	return pSonItem;
}

CSuperGridCtrl::CTreeItem *CSumTaMatTblCfgDlg::ImprotXMLChildElemToList(
CMarkup xml,LISTITEM_TYPE data_type,CSuperGridCtrl::CTreeItem *pParentItem)
{
	CString sElemName=_T("");
	if(data_type==DATA_TOWER)
		sElemName="TOWER";
	else if(data_type==DATA_LEG)
		sElemName="LEG";
	else
		return NULL;
	CSuperGridCtrl::CTreeItem *pNewItem=NULL;
	if(pParentItem==NULL&&data_type!=DATA_TOWER)
		return NULL;
	CString sModuleName=_T(""),sTowerSeg=_T(""),sCoef=_T(""),sSeg=_T("");
	while (xml.FindChildElem(sElemName))   
	{ 
		xml.IntoElem();
		if(sElemName.CompareNoCase("TOWER")==0)
		{
			sModuleName=xml.GetAttrib("high");
			sTowerSeg=xml.GetAttrib("seg");
		}
		else if(sElemName.CompareNoCase("LEG")==0)
		{
			sModuleName=xml.GetAttrib("name");
			sSeg=xml.GetAttrib("seg");
			sCoef=xml.GetAttrib("coef");
		}
		else
			return NULL;
		//
		CListCtrlItemInfo* lp = new CListCtrlItemInfo();
		if(sElemName.CompareNoCase("TOWER")==0)
		{
			lp->SetImage(2);
			lp->SetSubItemText(0,sModuleName);
			lp->SetSubItemText(1,sTowerSeg);
			pNewItem=m_listCtrl.InsertRootItem(lp);
			pNewItem->m_idProp=DATA_TOWER;
		}
		else
		{
			lp->SetImage(3);
			lp->SetSubItemText(0,sModuleName);
			lp->SetSubItemText(2,sSeg);
			lp->SetSubItemText(3,sCoef);
			pNewItem=m_listCtrl.InsertItem(pParentItem,lp,-1,true);
			pNewItem->m_idProp=DATA_LEG;
		}
		if(pNewItem)
		{
			if(data_type==DATA_TOWER)		
				ImprotXMLChildElemToList(xml,DATA_LEG,pNewItem);
		}
		xml.OutOfElem();
	}
	return pNewItem;
}

//From DataFile.cpp
void SaveTma();	
void CSumTaMatTblCfgDlg::OnOK() 
{
	if(m_listCtrl.GetItemCount()>0)
	{	//材料汇总配置文件有记录,保存材料汇总配置文件
		CMarkup xml;
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		xml.AddElem("TOWERSET");
		ExprotListChildItemToXML(xml,DATA_TOWER,NULL);
		int n = xml.GetDoc().GetLength();
		char *pStr = new char[n];
		xml.Save("",pStr);
		Ta.AttachBuffer[CTower::BOM_CONFIG].ClearBuffer();
		//
		Ta.AttachBuffer[CTower::BOM_CONFIG].WriteString("材料汇总配置文件");
		Ta.AttachBuffer[CTower::BOM_CONFIG].WriteString(pStr);
		delete [] pStr;
	}
	else	//材料汇总配置文件无记录,清空配置文件缓存区
		Ta.AttachBuffer[CTower::BOM_CONFIG].ClearBuffer();
	//保存材料汇总表配置文件 wht 11-08-08
	SaveTma();
	CDialog::OnOK();
}

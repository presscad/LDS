// LsSubPartLibDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "LsSubPartLibDlg.h"
#include "afxdialogex.h"
#include "image.h"
#include "GlobalFunc.h"
#include "database.h"

//////////////////////////////////////////////////////////////////////////
//�ص���������
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	CLsSubPartLibDlg *pLsSubPartLibDlg=(CLsSubPartLibDlg*)pListCtrl->GetParent();
	if(sTextValue.GetLength()<=0)
		return FALSE;
	CString oldText = pSelItem->m_lpNodeInfo->GetSubItemText(iSubItem);
	if(oldText.CompareNoCase(sTextValue)==0)
		return FALSE;
	if(iSubItem==0)
	{	//��˨ֱ����������˨����Χ�ڣ���ǰ�����12��48��
		if (atoi(sTextValue)<12||atoi(sTextValue)>48)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			MessageBox(NULL,"bolt's diameter is not in the specified range!","ERROR",MB_OK);
#else
			MessageBox(NULL,"��˨ֱ�����ڹ涨��Χ��!","ERROR",MB_OK);
#endif
			return FALSE;
		}
	}
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	//
	int num = pListCtrl->GetItemCount();
	long nIndex=pSelItem->m_idProp-1;
	if(pSelItem->m_idProp==num && sTextValue!="")
	{
		CListCtrlItemInfo* lpRoot = new CListCtrlItemInfo();
		lpRoot->AddSubItemText("");
		lpRoot->AddSubItemText("");
		lpRoot->AddSubItemText("");
		CSuperGridCtrl::CTreeItem* pItem=pListCtrl->InsertRootItem(lpRoot);
		pItem->m_idProp=num+1;	
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CLsSubPartLibDlg �Ի���
IMPLEMENT_DYNAMIC(CLsSubPartLibDlg, CDialogEx)

CLsSubPartLibDlg::CLsSubPartLibDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLsSubPartLibDlg::IDD, pParent)
{
	m_xListCtrl.AddColumnHeader("��˨ֱ��(mm)");
	m_xListCtrl.AddColumnHeader("������(mm)");
	m_xListCtrl.AddColumnHeader("�������(kg)");
}

CLsSubPartLibDlg::~CLsSubPartLibDlg()
{
}

void CLsSubPartLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_xListCtrl);
	DDX_Control(pDX, IDC_TREE_CTRL, m_xTreeCtrl);
}


BEGIN_MESSAGE_MAP(CLsSubPartLibDlg, CDialogEx)
	ON_WM_PAINT()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CTRL, OnSelchangedTreeCtrl)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_CTRL, OnKeydownTreeCtrl)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CTRL, OnRclickTreeCtrl)
	//ON_COMMAND(ID_NEW_ITEM_FOLDER, OnNewItem)
	ON_COMMAND(ID_DELETE_ALL_ITEM, OnDeleteAllItem)
	ON_COMMAND(ID_IMPORT_XML_FILE, OnImportLsSubPartLibFile)
	ON_COMMAND(ID_EXPORT_XML_FILE, OnExportLsSubPartLibFile)
END_MESSAGE_MAP()


// CLsSubPartLibDlg ��Ϣ�������
BOOL CLsSubPartLibDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//��ʼ�����ؼ�
	m_imageList.Create(IDB_IL_PROJECT, 16, 1, RGB(0,255,0));
	m_xTreeCtrl.SetImageList(&m_imageList,TVSIL_NORMAL);
	m_xTreeCtrl.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS);
	RefreshTreeCtrl();		//ˢ�����ؼ�
	//
	long col_wide_arr[3]={100,100,100};
	m_xListCtrl.InitListCtrl(col_wide_arr);
	m_xListCtrl.SetModifyValueFunc(FireValueModify);
	RefreshListCtrl();
	//�ƶ��Ի������Ͻ�
	CRect rect;
	GetWindowRect(rect);
	rect.right = rect.Width();
	rect.left = 0;
	rect.bottom = rect.Height();
	rect.top = 0;
	MoveWindow(rect,TRUE);
	return TRUE;
}
//ˢ�����ؼ�
void CLsSubPartLibDlg::RefreshTreeCtrl()
{
	itemInfoList.Empty();
	m_xTreeCtrl.DeleteAllItems();
	//
	TREEITEM_INFO *pItemInfo=NULL;
	HTREEITEM hRootItem=NULL,hTreeItem=NULL,hComNutItem=NULL;
	hRootItem=m_xTreeCtrl.InsertItem("��˨�����",PRJ_IMG_ROOT,PRJ_IMG_ROOT,TVI_ROOT);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(0,0));
	m_xTreeCtrl.SetItemData(hRootItem,(DWORD)pItemInfo);
	for(CLsSubPartFamily* pLsSubPartFamily=CLsLibrary::GetFirstLsSubPartFamily();pLsSubPartFamily;pLsSubPartFamily=CLsLibrary::GetNextLsSubPartFamily())
	{
		CXhChar50 sName("%s(%d)",pLsSubPartFamily->name,pLsSubPartFamily->GetCount());
		hTreeItem=m_xTreeCtrl.InsertItem(sName,PRJ_IMG_FL,PRJ_IMG_FL,hRootItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(pLsSubPartFamily->m_iType,(DWORD)pLsSubPartFamily));
		m_xTreeCtrl.SetItemData(hTreeItem,(DWORD)pItemInfo);
		if(hComNutItem==NULL)
			hComNutItem=hTreeItem;
	}
	//
	m_xTreeCtrl.Expand(hRootItem,TVE_EXPAND);
	if(hComNutItem)
		m_xTreeCtrl.SelectItem(hComNutItem);
	else
		m_xTreeCtrl.SelectItem(hRootItem);
}
//
void CLsSubPartLibDlg::RefreshListCtrl()
{
	HTREEITEM hSelItem=m_xTreeCtrl.GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL;
	if(hSelItem)
		pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hSelItem);
	int iSelItemType=0;
	if(pItemInfo)
		iSelItemType=pItemInfo->itemType;
	CLsSubPartFamily* pLsSubPartFamliy=(iSelItemType>0)?(CLsSubPartFamily*)pItemInfo->dwRefData:NULL;
	//
	m_xListCtrl.DeleteAllItems();
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	if(pLsSubPartFamliy)
	{
		int i=0;
		for(i=0;i<pLsSubPartFamliy->GetCount();i++)
		{
			LS_SUB_PART* pLsSubPart=pLsSubPartFamliy->RecordAt(i);
			if(pLsSubPart)
			{
				CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
				CXhChar50 sTextValue("%d",pLsSubPart->wiLsD);
				lpInfo->AddSubItemText(sTextValue);	//ֱ��
				sTextValue.Printf("%d",pLsSubPart->wiThick);		
				lpInfo->AddSubItemText(sTextValue);	//���
				sTextValue.Printf("%f",pLsSubPart->weight);
				SimplifiedNumString(sTextValue);
				lpInfo->AddSubItemText(sTextValue);	//����
				//
				pItem=m_xListCtrl.InsertRootItem(lpInfo);
				pItem->m_idProp=i+1;
			}
		}
		//ĩβ׷��һ���ռ�¼�������û���Ӽ�¼
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		lpInfo->AddSubItemText("");
		lpInfo->AddSubItemText("");
		lpInfo->AddSubItemText("");
		pItem=m_xListCtrl.InsertRootItem(lpInfo);
		pItem->m_idProp=i+1;
	}
}
void CLsSubPartLibDlg::ContextMenu(CWnd* pWnd, CPoint point) 
{
	HTREEITEM hItem=m_xTreeCtrl.GetSelectedItem();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	//
	if(pItemInfo->itemType==0)
	{
		//pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM_FOLDER,"�½���˨�����");
	}
	else if(pItemInfo->itemType==CLsSubPartFamily::TYPE_COMMON_NUT)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"�����ͨ��ĸ��");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"������ͨ��ĸ���ļ�");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"������ͨ��ĸ���ļ�");
	}
	else if(pItemInfo->itemType==CLsSubPartFamily::TYPE_FASTEN_NUT)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"��տ۽���ĸ��");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"����۽���ĸ���ļ�");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"�����۽���ĸ���ļ�");
	}
	else if(pItemInfo->itemType==CLsSubPartFamily::TYPE_FANGDAO_NUT)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"��շ�����ĸ��");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"���������ĸ���ļ�");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"����������ĸ���ļ�");
	}
	else if(pItemInfo->itemType==CLsSubPartFamily::TYPE_THIN_NUT)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"��ձ���ĸ��");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"���뱡��ĸ���ļ�");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"��������ĸ���ļ�");
	}
	else if(pItemInfo->itemType==CLsSubPartFamily::TYPE_COMMON_GASKET)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"���ƽ��Ȧ��");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"����ƽ��Ȧ���ļ�");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"����ƽ��Ȧ���ļ�");
	}
	else if(pItemInfo->itemType==CLsSubPartFamily::TYPE_SPRING_GASKET)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"��յ��ɵ�Ȧ��");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"���뵯�ɵ�Ȧ���ļ�");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"�������ɵ�Ȧ���ļ�");
	}
	else if(pItemInfo->itemType==CLsSubPartFamily::TYPE_LOCK_CLASP)
	{
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ALL_ITEM,"��շ��ɿۿ�");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"������ɿۿ��ļ�");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"�������ɿۿ��ļ�");
	}
	else
		return;
	CPoint scr_point = point;
	m_xTreeCtrl.ClientToScreen(&scr_point);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}
//��������
void CLsSubPartLibDlg::OnOK() 
{
	HTREEITEM hItem=m_xTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		return;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL||pItemInfo->itemType==0)
		return;
	CLsSubPartFamily* pLsSubPartFamily=(CLsSubPartFamily*)pItemInfo->dwRefData;
	if(pLsSubPartFamily==NULL)
		return;
	int nNum = m_xListCtrl.GetItemCount();
	if(nNum-1<=0)
		return;
	CXhChar100 sText;
	pLsSubPartFamily->CreateLsSubPartFamily(nNum-1);
	for(int i=0;i<nNum-1;i++)
	{
		LS_SUB_PART *pSubPart=pLsSubPartFamily->RecordAt(i);
		m_xListCtrl.GetItemText(i,0,sText,100);
		pSubPart->wiLsD=atoi(sText);
		m_xListCtrl.GetItemText(i,1,sText,100);
		pSubPart->wiThick=atoi(sText);
		m_xListCtrl.GetItemText(i,2,sText,100);
		pSubPart->weight=atof(sText);
	}
	//
	CXhChar50 sPartType("%s(%d)",pLsSubPartFamily->name,pLsSubPartFamily->GetCount());
	m_xTreeCtrl.SetItemText(hItem,sPartType);
}
//////////////////////////////////////////////////////////////////////////
//��Ϣ�¼�����
void CLsSubPartLibDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CLsSubPartLibDlg::OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem=m_xTreeCtrl.GetSelectedItem();
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	RefreshListCtrl();
	*pResult = 0;
}

void CLsSubPartLibDlg::OnKeydownTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

	*pResult = 0;
}

void CLsSubPartLibDlg::OnRclickTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	m_xTreeCtrl.ScreenToClient(&HitTestInfo.pt);
	m_xTreeCtrl.HitTest(&HitTestInfo);
	m_xTreeCtrl.Select(HitTestInfo.hItem,TVGN_CARET);
	ContextMenu(this,HitTestInfo.pt);
	*pResult = 0;
}

void CLsSubPartLibDlg::OnDeleteAllItem()
{
	HTREEITEM hSelItem=m_xTreeCtrl.GetSelectedItem();
	if(hSelItem==NULL)
		return;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hSelItem);
	if(pItemInfo==NULL||pItemInfo->itemType==0)
		return;
	CLsSubPartFamily* pFamily=(CLsSubPartFamily*)pItemInfo->dwRefData;
	if(pFamily==NULL)
		return;
	if(AfxMessageBox("ȷ��Ҫ��յ�ǰ��˨�������?",MB_YESNO)==IDNO)
		return;
	pFamily->CleanSubParts();
	CXhChar50 sPartType("%s(0)",pFamily->name);
	m_xTreeCtrl.SetItemText(hSelItem,sPartType);
}

void CLsSubPartLibDlg::OnImportLsSubPartLibFile()
{
	HTREEITEM hSelItem=m_xTreeCtrl.GetSelectedItem();
	if(hSelItem==NULL)
		return;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hSelItem);
	if(pItemInfo==NULL||pItemInfo->itemType==0)
		return;
	CLsSubPartFamily* pFamily=(CLsSubPartFamily*)pItemInfo->dwRefData;
	if(pFamily==NULL)
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(TRUE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bolt specification file(*.lssp)|*.lssp||");
#else
	CFileDialog dlg(TRUE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"��˨����ļ�(*.lssp)|*.lssp||");
		
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	CString sFileName=dlg.GetPathName();
	LsSubPartSerialize(pFamily,sFileName,FALSE);
	//�����Ժ���Ҫˢ���б�����
	CXhChar50 sPartType("%s(%d)",pFamily->name,pFamily->GetCount());
	m_xTreeCtrl.SetItemText(hSelItem,sPartType);
	RefreshListCtrl();
}

void CLsSubPartLibDlg::OnExportLsSubPartLibFile()
{
	HTREEITEM hSelItem=m_xTreeCtrl.GetSelectedItem();
	if(hSelItem==NULL)
		return;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hSelItem);
	if(pItemInfo==NULL||pItemInfo->itemType==0)
		return;
	CLsSubPartFamily* pFamily=(CLsSubPartFamily*)pItemInfo->dwRefData;
	if(pFamily==NULL)
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	CFileDialog dlg(FALSE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bolt specification file(*.lssp)|*.lssp||");	
#else
	CFileDialog dlg(FALSE,"lssp","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"��˨����ļ�(*.lssp)|*.lssp||");
		
#endif
	if(dlg.DoModal()!=IDOK)
		return;
	CString sFileName = dlg.GetPathName();
	LsSubPartSerialize(pFamily,sFileName,TRUE);
}
	
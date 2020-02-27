// StdComTmplStruPage.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "afxdialogex.h"
#include "StdComTmplStruPage.h"
#include "StdComTmplLibraryDlg.h"


// CStdComTmplStruPage 对话框

IMPLEMENT_DYNAMIC(CStdComTmplStruPage, CDialogEx)

CStdComTmplStruPage::CStdComTmplStruPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStdComTmplStruPage::IDD, pParent)
{
	m_idStdComTmpl=0;
}

CStdComTmplStruPage::~CStdComTmplStruPage()
{
}

void CStdComTmplStruPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_STD_COM_TMPL, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CStdComTmplStruPage, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_STD_COM_TMPL, OnSelchangedTreeCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CStdComTmplStruPage 消息处理程序
#include "env_def.h"
void CStdComTmplStruPage::OnSelchangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hSelItem=m_treeCtrl.GetSelectedItem();
	DWORD dwAttachObjData=m_treeCtrl.GetItemData(hSelItem);
	//TREEITEM_INFO *pItemInfo=hSelItem==NULL?NULL:(TREEITEM_INFO*)m_treeCtrl.GetItemData(hSelItem);
	//if(pItemInfo==NULL)
	//	return;
	//dwAttachObjData=pItemInfo->dwRefData;
	m_idStdComTmpl=0;
	CStatic *pSketchPanel=(CStatic*)GetDlgItem(IDC_S_COM_TMPL_SKETCH);
	CATLOG_ITEM* pCatgItem=(CATLOG_ITEM*)dwAttachObjData;
	if(pCatgItem!=NULL)
	{
		CStdComTempl* pStdComTmpl=(CStdComTempl*)globalStdComTmplLibrary.LoadStdComTempl(pCatgItem->idRelaObj);
		if(pStdComTmpl)
			m_idStdComTmpl=pStdComTmpl->GetTmplId();
		if(pStdComTmpl&&pStdComTmpl->LoadSketchImage())
		{
			if(m_xSketchImg.GetSafeHandle()!=NULL)
				m_xSketchImg.Detach();
			m_xSketchImg.CreateBitmapIndirect(&pStdComTmpl->sketchimg);
			//CStatic *pPic=(CStatic*)GetDlgItem(IDC_S_MAP);
			HBITMAP hBitmap=(HBITMAP)m_xSketchImg.GetSafeHandle();
			pSketchPanel->SetBitmap(hBitmap);
		}
		else
			pSketchPanel->SetBitmap(NULL);
		// 根据选中的库是否相同来是否禁用更新按钮
		//BOOL bSameArr=FALSE;
		//if(pItemInfo->itemType==TYPE_ROLLEND)	//卷边插板							  
		//	bSameArr=CompareArr(rollend_param_table,g_orgPartLib.rollend_param_table.m_pData,rollend_param_N,g_orgPartLib.rollend_param_N,sizeof(INSERT_PLATE_PARAM));	
	}
	*pResult = 0;
}
HTREEITEM CStdComTmplStruPage::AddOrUpdateTreeItem(CATLOG_TREEITEM* pCatgItem,HTREEITEM hParentItem)
{
	HTREEITEM hItem=m_treeCtrl.InsertItem(pCatgItem->pCatlog->name,hParentItem);
	m_treeCtrl.SetItemData(hItem,(DWORD)pCatgItem->pCatlog);
	for(CATLOG_TREEITEM* pSonCatgItem=pCatgItem->MoveSonItemFirst();pSonCatgItem;pSonCatgItem=pCatgItem->MoveSonItemNext())
	{	//添加子节点
		HTREEITEM hSonItem=AddOrUpdateTreeItem(pSonCatgItem,hItem);
	}
	return hItem;
}
void CStdComTmplStruPage::OnOK(){
	CWnd* pParentWnd=GetParent();
	for(int i=0;i<4;i++){
		if(stricmp(pParentWnd->GetRuntimeClass()->m_lpszClassName,"CStdComTmplLibraryDlg")==0)
			break;
		pParentWnd=pParentWnd->GetParent();
	}
	if(i==4)
		return;
	CStdComTmplLibraryDlg* pParentDlg=(CStdComTmplLibraryDlg*)pParentWnd;
	pParentDlg->OnOK();
}
void CStdComTmplStruPage::OnCancel(){
	CWnd* pParentWnd=GetParent();
	for(int i=0;i<4;i++){
		if(stricmp(pParentWnd->GetRuntimeClass()->m_lpszClassName,"CStdComTmplLibraryDlg")==0)
			break;
		pParentWnd=pParentWnd->GetParent();
	}
	if(i==4)
		return;
	CStdComTmplLibraryDlg* pParentDlg=(CStdComTmplLibraryDlg*)pParentWnd;
	pParentDlg->OnCancel();
}
BOOL CStdComTmplStruPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HTREEITEM hRoot=m_treeCtrl.InsertItem(globalStdComTmplLibrary.xStruCatlog.pCatlog->name);	//"结构部件"
	CATLOG_TREEITEM* pRootCatg=&globalStdComTmplLibrary.xStruCatlog;
	CATLOG_TREEITEM* pCatgItem;
	for(pCatgItem=pRootCatg->MoveSonItemFirst();pCatgItem;pCatgItem=pRootCatg->MoveSonItemNext())
	{	//添加子节点
		HTREEITEM hItem=AddOrUpdateTreeItem(pCatgItem,hRoot);
	}
	m_treeCtrl.Expand(hRoot,TVE_EXPAND);
	RECT rc;
	m_treeCtrl.GetClientRect(&rc);
	int width=rc.right;
	this->GetClientRect(&rc);
	m_treeCtrl.MoveWindow(0,0,width,rc.bottom);
	//m_treeCtrl.Expand(

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CStdComTmplStruPage::OnSize(UINT nType, int cx, int cy)
{
	if(m_treeCtrl.GetSafeHwnd()==NULL||GetSafeHwnd()==NULL)
		return;
	RECT rc;
	m_treeCtrl.GetClientRect(&rc);
	int width=rc.right;
	this->GetClientRect(&rc);
	m_treeCtrl.MoveWindow(0,0,width,rc.bottom);
	CDialog::OnSize(nType,cx,cy);
}

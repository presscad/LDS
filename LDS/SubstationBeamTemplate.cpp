// SubstationBeamTemplate.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "SubstationBeamTemplate.h"
#include "afxdialogex.h"
#include "supergridctrl.h"


// CSubstationBeamTemplateDlg 对话框

IMPLEMENT_DYNAMIC(CSubstationBeamTemplateDlg, CDialogEx)

CSubstationBeamTemplateDlg::CSubstationBeamTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSubstationBeamTemplateDlg::IDD, pParent)
{
	m_listCtrlSegmentLeftEnd.AddColumnHeader("1",40);
	m_listCtrlSegmentLeftEnd.AddColumnHeader("2",40);
}

CSubstationBeamTemplateDlg::~CSubstationBeamTemplateDlg()
{
}

void CSubstationBeamTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEGMENTS, m_listCtrlSegmentLeftEnd);
}


BEGIN_MESSAGE_MAP(CSubstationBeamTemplateDlg, CDialogEx)
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
END_MESSAGE_MAP()


// CSubstationBeamTemplateDlg 消息处理程序

static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	pListCtrl->SetSubItemText(pSelItem,iSubItem,sTextValue);
	int nItemCount=pSelItem->m_lpNodeInfo->GetItemCount();
	int nColCount=pListCtrl->GetHeaderCtrl()->GetItemCount();
	if(nColCount==iSubItem+1 && sTextValue.GetLength()>0)
	{
		pSelItem->m_lpNodeInfo->AddSubItemText("");
		LV_COLUMN lvc;
		lvc.iSubItem = nColCount;
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		lvc.cx=40;
		lvc.pszText=CXhChar16("%d",nColCount+1);
		pListCtrl->InsertColumn(nColCount,&lvc);
		pListCtrl->RedrawItems(0,1);
		//pListCtrl->SetColumnWidth(lvc.iSubItem,lvc.cx);
	}
	return TRUE;
}
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CSubstationBeamTemplateDlg *pBeamDlg=(CSubstationBeamTemplateDlg*)pListCtrl->GetParent();
	if(pBeamDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING,ID_NEW_ITEM,"插入节间");
	pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除节间");
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pBeamDlg);
	return TRUE;
}
static BOOL DeleteColSegmentItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem)
{
	CSubstationBeamTemplateDlg *pBeamDlg=(CSubstationBeamTemplateDlg*)pListCtrl->GetParent();
	if(pBeamDlg==NULL)
		return FALSE;
	pBeamDlg->DeleteSegmentItem();
	return FALSE;
}
BOOL CSubstationBeamTemplateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_listCtrlSegmentLeftEnd.InitListCtrl(NULL);
	m_listCtrlSegmentLeftEnd.SetModifyValueFunc(FireValueModify);
	m_listCtrlSegmentLeftEnd.SetContextMenuFunc(FireContextMenu);
	m_listCtrlSegmentLeftEnd.SetDeleteItemFunc(DeleteColSegmentItem);
	m_listCtrlSegmentLeftEnd.GetColumnCount();
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	lpInfo->SetSubItemText(0,_T("500"),FALSE);
	lpInfo->SetSubItemText(1,_T(""),FALSE);
	//lpInfo->AddSubItemText(theApp.GetPropValueStr(CLDSApp::GetPropID("Ta.m_fIceDensity")));
	CSuperGridCtrl::CTreeItem* pGroupItem = m_listCtrlSegmentLeftEnd.InsertRootItem(lpInfo);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CSubstationBeamTemplateDlg::OnNewItem()
{
	//int iCurRow = m_listCtrlSegmentLeftEnd.GetSelectedItem();
	int iCurCol = m_listCtrlSegmentLeftEnd.GetCurSubItem();
	m_listCtrlSegmentLeftEnd._InsertColumn(iCurCol,CXhChar16("%d",iCurCol+1),40);
	int nColCount=m_listCtrlSegmentLeftEnd.GetHeaderCtrl()->GetItemCount();
	for(int i=iCurCol+1;i<nColCount;i++)
		m_listCtrlSegmentLeftEnd.SetColumnHeaderTitle(i,CXhChar16("%d",i+1));
	m_listCtrlSegmentLeftEnd.RedrawItems(0,1);
}
void CSubstationBeamTemplateDlg::DeleteSegmentItem()
{
	int iCurCol = m_listCtrlSegmentLeftEnd.GetCurSubItem();
	int nColCount=m_listCtrlSegmentLeftEnd.GetHeaderCtrl()->GetItemCount();
	if(nColCount<=1)
	{
		AfxMessageBox("不能删除最后一列");
		return;
	}
	m_listCtrlSegmentLeftEnd._DeleteColumn(iCurCol);
	nColCount--;
	for(int i=iCurCol;i<nColCount;i++)
		m_listCtrlSegmentLeftEnd.SetColumnHeaderTitle(i,CXhChar16("%d",i+1));
	m_listCtrlSegmentLeftEnd.RedrawItems(0,1);
}
void CSubstationBeamTemplateDlg::OnDeleteItem()
{
	DeleteSegmentItem();
}

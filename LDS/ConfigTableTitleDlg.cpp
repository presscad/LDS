// ConfigTableTitleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "ConfigTableTitleDlg.h"
#include "afxdialogex.h"
#include "BomTblTitleCfg.h"

static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CConfigTableTitleDlg *pCfgDlg=(CConfigTableTitleDlg*)pListCtrl->GetParent();
	if(pCfgDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	BOOL bInSelItemRect=FALSE;
	CHeaderCtrl *pHeadCtrl=pListCtrl->GetHeaderCtrl();
	HDHITTESTINFO info={0};
	info.pt=point;
	pHeadCtrl->HitTest(&info);
	if(info.iItem>=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Delete Item");
#else
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除列标题");
#endif
	}
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pCfgDlg);
	return TRUE;
}

// CConfigTableTitleDlg 对话框

IMPLEMENT_DYNAMIC(CConfigTableTitleDlg, CDialog)

CConfigTableTitleDlg::CConfigTableTitleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigTableTitleDlg::IDD, pParent)
	, m_nStartRow(2)
{
	m_pSheetContextMap=NULL;
	m_iCurTitleIndex=-1;
}

CConfigTableTitleDlg::~CConfigTableTitleDlg()
{
}

void CConfigTableTitleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TITLE, m_listTitle);
	DDX_Control(pDX, IDC_LIST_CONTENT, m_listContent);
	DDX_Text(pDX, IDC_CMB_START_ROW, m_nStartRow);
}


BEGIN_MESSAGE_MAP(CConfigTableTitleDlg, CDialog)
	ON_WM_SIZE()
	ON_COMMAND(ID_DELETE_ITEM,OnDeleteHeaderTitle)
END_MESSAGE_MAP()


// CConfigTableTitleDlg 消息处理程序


BOOL CConfigTableTitleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitListCtrl(m_pSheetContextMap);
	return TRUE;
}

static CXhChar100 VariantToString(VARIANT value)
{
	CXhChar100 sValue;
	if(value.vt==VT_BSTR)
		return sValue.Copy(CString(value.bstrVal));
	else if(value.vt==VT_R8)
		return sValue.Copy(CXhChar100(value.dblVal));
	else if(value.vt==VT_R4)
		return sValue.Copy(CXhChar100(value.fltVal));
	else if(value.vt==VT_INT)
		return sValue.Copy(CXhChar100(value.intVal));
	else 
		return sValue;
}

void CConfigTableTitleDlg::InitListCtrl(CVariant2dArray *pSheetContextMap)
{
	const int COL_WIDTH=85;
	int nColCount=(pSheetContextMap!=NULL)?pSheetContextMap->ValidColsCount():CBomTblTitleCfg::T_COL_COUNT;
	//初始化列表框
	m_listTitle.EmptyColumnHeader();
	m_listTitle.AddColumnHeader(CBomTblTitleCfg::T_PART_NO,COL_WIDTH);
	m_listTitle.AddColumnHeader(CBomTblTitleCfg::T_METERIAL,COL_WIDTH);		
	m_listTitle.AddColumnHeader(CBomTblTitleCfg::T_LEN,COL_WIDTH);
	m_listTitle.AddColumnHeader(CBomTblTitleCfg::T_SPEC,COL_WIDTH);
	m_listTitle.AddColumnHeader(CBomTblTitleCfg::T_SING_NUM,COL_WIDTH);
	m_listTitle.AddColumnHeader(CBomTblTitleCfg::T_SING_WEIGHT,COL_WIDTH);
	m_listTitle.AddColumnHeader(CBomTblTitleCfg::T_NOTES,COL_WIDTH);
	m_listTitle.AddColumnHeader(CBomTblTitleCfg::T_WIDE,COL_WIDTH);
	m_listTitle.AddColumnHeader(CBomTblTitleCfg::T_PARTTYPE,COL_WIDTH);
	for(int i=CBomTblTitleCfg::T_COL_COUNT;i<nColCount;i++)
		m_listTitle.AddColumnHeader("-",COL_WIDTH);
	m_listTitle.InitListCtrl();
	m_listTitle.SetContextMenuFunc(FireContextMenu);
	m_listTitle.SetExtendedStyle(m_listTitle.GetExtendedStyle()|LVS_EX_HEADERDRAGDROP);
	//
	m_listContent.EmptyColumnHeader();
	for(int i=0;i<nColCount;i++)
		m_listContent.AddColumnHeader(CXhChar16("A%d",i+1),COL_WIDTH);
	m_listContent.InitListCtrl();
	m_listContent.SetExtendedStyle(m_listContent.GetExtendedStyle() | LVS_NOCOLUMNHEADER);
	if(pSheetContextMap!=NULL)
	{
		VARIANT value;
		CXhChar100 sValue;
		int nColCount=pSheetContextMap->ValidColsCount();
		int nRowCount=min(10,pSheetContextMap->RowsCount());
		for(int i=0;i<nColCount;i++)
		{
			CListCtrlItemInfo *lpInfo=new CListCtrlItemInfo();
			for(int j=0;j<nColCount;j++)
			{
				pSheetContextMap->GetValueAt(i,j,value);
				sValue=VariantToString(value);
				lpInfo->SetSubItemText(j,sValue,TRUE);
			}
			m_listContent.InsertRootItem(lpInfo,FALSE);
		}
		m_listContent.Redraw();
	}
}

void CConfigTableTitleDlg::OnOK()
{
	UpdateData();
	CHeaderCtrl *pHeaderCtrl=m_listTitle.GetHeaderCtrl();
	if(m_pSheetContextMap&&pHeaderCtrl)
	{
		int nColCount=m_pSheetContextMap->ValidColsCount();
		HDITEM item;
		TCHAR szText[ 256 ];
		item.mask = HDI_TEXT | HDI_FORMAT;
		item.pszText = szText;
		item.cchTextMax = 255;
		CHashStrList<DWORD> hashColIndexByName;
		DYN_ARRAY<int> orderArr(pHeaderCtrl->GetItemCount());
		pHeaderCtrl->GetOrderArray(orderArr,pHeaderCtrl->GetItemCount());
		for(int i=0;i<pHeaderCtrl->GetItemCount();i++)
		{
			pHeaderCtrl->GetItem(orderArr[i],&item);
			if(item.pszText==NULL||strlen(item.pszText)<=0)
				continue;
			hashColIndexByName.SetValue(item.pszText,i+1);
		}
		
		int colIndexArr[CBomTblTitleCfg::T_COL_COUNT]={0};
		CXhChar100 titleArr[CBomTblTitleCfg::T_COL_COUNT]={CBomTblTitleCfg::T_PART_NO,CBomTblTitleCfg::T_METERIAL,
														   CBomTblTitleCfg::T_LEN,CBomTblTitleCfg::T_SPEC,
														   CBomTblTitleCfg::T_SING_NUM,CBomTblTitleCfg::T_SING_WEIGHT,
														   CBomTblTitleCfg::T_NOTES,CBomTblTitleCfg::T_WIDE,
														   CBomTblTitleCfg::T_PARTTYPE};
		for(int i=0;i<CBomTblTitleCfg::T_COL_COUNT;i++)
		{
			DWORD *pColIndex=hashColIndexByName.GetValue(titleArr[i]);
			if(pColIndex)
				colIndexArr[i]=*pColIndex;
			else
				colIndexArr[i]=0;
		}
		CXhChar500 sTitleArr;
		if(m_nStartRow>1)
		{
			VARIANT var;
			int iTitleRow=m_nStartRow-2;
			for(int i=0;i<nColCount;i++)
			{
				m_pSheetContextMap->GetValueAt(iTitleRow,i,var);
				CXhChar100 sTitle=VariantToString(var);
				sTitleArr.Append(sTitle);
				sTitleArr.Append("|");
			}
		}
		m_xTblCfg.Init(colIndexArr,sTitleArr,8,m_nStartRow);
	}
	CDialog::OnOK();
}


void CConfigTableTitleDlg::OnSize(UINT nType, int cx, int cy)
{
	RECT rcFull,rcListTitle,rcListContent;
	GetClientRect(&rcFull);
	CWnd *pListTitle=GetDlgItem(IDC_LIST_TITLE);
	CWnd *pListContent=GetDlgItem(IDC_LIST_CONTENT);
	if(pListTitle==NULL||pListContent==NULL)
		return;
	pListTitle->GetWindowRect(&rcListTitle);
	pListContent->GetWindowRect(&rcListContent);
	ScreenToClient(&rcListTitle);
	ScreenToClient(&rcListContent);
	int nFullWidth=rcFull.right-rcListContent.left;
	int nFullHeight=rcFull.bottom-rcListContent.top;
	rcListTitle.right=rcListTitle.left+(nFullWidth-20);
	pListTitle->MoveWindow(&rcListTitle);
	rcListContent.right=rcListContent.left+(nFullWidth-20);
	rcListContent.bottom=rcListContent.top+(nFullHeight);
	pListContent->MoveWindow(&rcListContent);

	m_listTitle.RefreshListViewHeader();

	CDialog::OnSize(nType, cx, cy);
}

void CConfigTableTitleDlg::ListTitleHeaderContextMenu(CPoint point)
{
	CPoint scr_point = point;
	m_listTitle.ClientToScreen(&scr_point);
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	while(pMenu->GetMenuItemCount()>0)
		pMenu->DeleteMenu(0,MF_BYPOSITION);
	pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除列标题");
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

BOOL CConfigTableTitleDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if ((((LPNMHDR)lParam)->code == NM_RCLICK))
	{
		CPoint pt, pt2;
		GetCursorPos(&pt);
		pt2 = pt;
		m_iCurTitleIndex=-1;
		m_listTitle.ScreenToClient(&pt);
		CWnd* pWnd = m_listTitle.ChildWindowFromPoint(pt);
		CHeaderCtrl* pHeader = m_listTitle.GetHeaderCtrl();
		if(pWnd && (pWnd->GetSafeHwnd() == pHeader->GetSafeHwnd()))
		{
			HDHITTESTINFO info = {0};
			info.pt = pt;
			pHeader->SendMessage(HDM_HITTEST, 0, (LPARAM)&info);
			if(info.iItem>=0)
			{
				ListTitleHeaderContextMenu(pt);
				m_iCurTitleIndex=info.iItem;
			}
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CConfigTableTitleDlg::OnDeleteHeaderTitle()
{
	if(m_iCurTitleIndex==-1)
		return;
	m_listTitle.SetColumnHeaderTitle(m_iCurTitleIndex,"-");
}
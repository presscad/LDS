// ObjectSyncPropDetailsDlg.cpp : ʵ���ļ�
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "ObjectSyncPropDetailsDlg.h"
#include "afxdialogex.h"


// CObjectSyncPropDetailsDlg �Ի���

IMPLEMENT_DYNAMIC(CObjectSyncPropDetailsDlg, CDialogEx)

CObjectSyncPropDetailsDlg::CObjectSyncPropDetailsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CObjectSyncPropDetailsDlg::IDD, pParent)
{
	m_pSyncPropList=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	m_listSyncDetails.AddColumnHeader("Property Name",200);
	m_listSyncDetails.AddColumnHeader("State",60);
#else 
	m_listSyncDetails.AddColumnHeader("��������",200);
	m_listSyncDetails.AddColumnHeader("״̬",60);
#endif
}

CObjectSyncPropDetailsDlg::~CObjectSyncPropDetailsDlg()
{
}

void CObjectSyncPropDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SYNC_PROP_DETAILS, m_listSyncDetails);
}


BEGIN_MESSAGE_MAP(CObjectSyncPropDetailsDlg, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SYNC_PROP_DETAILS, &OnNMRClickListSyncPropDetails)
	ON_COMMAND(ID_NEW_ITEM, OnSetSyncItem)
	ON_COMMAND(ID_DELETE_ITEM, OnRevokeSyncItem)
END_MESSAGE_MAP()


// CObjectSyncPropDetailsDlg ��Ϣ�������
static BOOL DblclkListSyncDetails(CXhListCtrl* pListCtrl,int iItem,int iSubItem)
{
	CObjectSyncPropDetailsDlg *pDlg=(CObjectSyncPropDetailsDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return FALSE;
	PROPLIST_TYPE* pListProps=pDlg->m_pSyncPropList;
	if(pListProps==NULL)
		return FALSE;
	long idItem=pListCtrl->GetItemData(iItem);
	PROPLIST_ITEM* pItem=pListProps->GetPropItem(idItem);
	if(pItem==NULL)
		return FALSE;
	if(pDlg->m_pMPCL->GetBitState((WORD)(pItem->id-1)))
	{
		pDlg->m_pMPCL->SetBitState((WORD)(pItem->id-1),false);
		pListCtrl->SetItemText(iItem,1,"");
	}
	else
	{
		pDlg->m_pMPCL->SetBitState((WORD)(pItem->id-1),true);
		pListCtrl->SetItemText(iItem,1,"��");
	}
	return TRUE;
}

BOOL CObjectSyncPropDetailsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if(m_pSyncPropList==NULL)
		return FALSE;
	m_listSyncDetails.InitListCtrl();
	m_listSyncDetails.SetDblclkFunc(DblclkListSyncDetails);
	CStringArray str_arr;
	str_arr.SetSize(2);
	for(PROPLIST_ITEM* pItem=m_pSyncPropList->EnumFirst();pItem;pItem=m_pSyncPropList->EnumNext())
	{
		str_arr[0]=(char*)pItem->name;
		if(m_pMPCL->GetBitState((WORD)(pItem->id-1)))
			str_arr[1]="��";
		else
			str_arr[1]="";
		int iCurr=m_listSyncDetails.InsertItemRecord(-1,str_arr);
		if(iCurr>=0)
			m_listSyncDetails.SetItemData(iCurr,pItem->id);
	}
	return TRUE;
}


//0.ȡ��ͬ�� 1.����ͬ�� 2.�ָ�Ĭ��ֵ
static void UpdateSelectedItemsState(CXhListCtrl *pListCtrl,BYTE cOper)
{
	CObjectSyncPropDetailsDlg *pDlg=(CObjectSyncPropDetailsDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return;
	PROPLIST_TYPE* pListProps=pDlg->m_pSyncPropList;
	if(pListProps==NULL)
		return;
	//
	bool bState=(cOper==0)?false:true;
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iItem=pListCtrl->GetNextSelectedItem(pos);
		long idItem=pListCtrl->GetItemData(iItem);
		PROPLIST_ITEM* pItem=pListProps->GetPropItem(idItem);
		if(pItem==NULL)
			continue;
		pDlg->m_pMPCL->SetBitState((WORD)(pItem->id-1),bState);
		if(!bState)
			pListCtrl->SetItemText(iItem,1,"");
		else 
			pListCtrl->SetItemText(iItem,1,"��");
	}
}

void CObjectSyncPropDetailsDlg::OnSetSyncItem()
{
	UpdateSelectedItemsState(&m_listSyncDetails,1);
}
void CObjectSyncPropDetailsDlg::OnRevokeSyncItem()
{
	UpdateSelectedItemsState(&m_listSyncDetails,0);
}
void CObjectSyncPropDetailsDlg::OnNMRClickListSyncPropDetails(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//1.��ȡ��ǰ���λ��
	CPoint point;
	GetCursorPos(&point);
	//2.��ȡListCtrl��Ӧ�ľ�������
	CRect rect;
	m_listSyncDetails.GetWindowRect(&rect);
	//3.��ListCtrl��Χ�ڵ���Ҽ�ʱ��ʾ�Ҽ��˵�
	if(rect.PtInRect(point))
	{
		CMenu popMenu;
		popMenu.LoadMenu(IDR_ITEM_CMD_POPUP); //���ز˵���
		popMenu.GetSubMenu(0)->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_NEW_ITEM,"Synchronize ");
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_DELETE_ITEM,"Cancel Synchronize ");
#else 
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_NEW_ITEM,"ͬ��");
		popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ȡ��ͬ��");
#endif
		//popMenu.GetSubMenu(0)->AppendMenu(MF_STRING,ID_SET_ACTIVE_ITEM,"Ĭ��");
		//�����Ҽ��˵�
		popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
	}
	*pResult = 0;
}
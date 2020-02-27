// displayPartInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "displayPartInfoDlg.h"
#include "afxdialogex.h"
#include "env_def.h"
#include "SortFunc.h"

// displayPartInfoDlg 对话框

IMPLEMENT_DYNAMIC(CDisplayPartInfoDlg, CDialogEx)

CDisplayPartInfoDlg::CDisplayPartInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDisplayPartInfoDlg::IDD, pParent)
{

#ifdef AFX_TARG_ENU_ENGLISH
	m_displayPartInfoListCtrl.AddColumnHeader("handle");
	m_displayPartInfoListCtrl.AddColumnHeader("segI");
	m_displayPartInfoListCtrl.AddColumnHeader("now segI");
	m_displayPartInfoListCtrl.AddColumnHeader("part No");
	m_displayPartInfoListCtrl.AddColumnHeader("specification");
#else
	m_displayPartInfoListCtrl.AddColumnHeader("句柄");
	m_displayPartInfoListCtrl.AddColumnHeader("段号");
	m_displayPartInfoListCtrl.AddColumnHeader("现段号");
	m_displayPartInfoListCtrl.AddColumnHeader("件号");
	m_displayPartInfoListCtrl.AddColumnHeader("规格");
#endif	
}

CDisplayPartInfoDlg::~CDisplayPartInfoDlg()
{
}

void CDisplayPartInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISPLAY_PART_INFO_LIST_CTRL, m_displayPartInfoListCtrl);
}
BEGIN_MESSAGE_MAP(CDisplayPartInfoDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_DISPLAY_PART_INFO_LIST_CTRL, &CDisplayPartInfoDlg::OnNMClickDisplayPartInfoListCtrl)
END_MESSAGE_MAP()


// displayPartInfoDlg 消息处理程序


static bool IsBriefMark(char cMat)
{
	if( CSteelMatLibrary::Q235BriefMark()==cMat||
		CSteelMatLibrary::Q345BriefMark()==cMat||
		CSteelMatLibrary::Q390BriefMark()==cMat||
		CSteelMatLibrary::Q420BriefMark()==cMat||
		CSteelMatLibrary::Q460BriefMark()==cMat)
		return true;
	else
		return false;
}
SEGI RetrieveSegIByPartNo(CLDSPart *pPart,CHashSet<CLDSPart*> &hashDatumPartByHandle)
{
	CLDSPart *pCurPart=pPart;
	if(pPart->GetClassTypeId()==CLS_BOLT)
	{
		pCurPart=Ta.FromPartHandle(((CLDSBolt*)pPart)->des_base_pos.hPart);
		if(pCurPart&&hashDatumPartByHandle.GetValue(pCurPart->handle)!=NULL)
			pCurPart=hashDatumPartByHandle.GetValue(pCurPart->handle);
	}
	else if(pPart->GetClassTypeId()==CLS_LINESLOT&&((CLDSLineSlot*)pPart)->IsFootNailPlate())
	{
		CLDSPart *pFatherPart=Ta.FromPartHandle(pPart->m_hPartWeldParent);
		if(pFatherPart)
			pCurPart=pFatherPart;
	}
	if(pCurPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pCurPart;
		if(pParamPlate->IsFL()||pParamPlate->IsInsertPlate())
			pCurPart=Ta.FromPartHandle(pParamPlate->m_hPartWeldParent);
	}
	SEGI segI;
	if(pCurPart&&ParsePartNo(pCurPart->GetPartNo(),&segI,NULL))
	{
		if(IsBriefMark(segI.key.prefix[0]))
			segI.key.prefix[0]=' ';
	}
	return segI;
}
int  FireCompareItem(const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem1,const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem2,DWORD lPara)
{
	COMPARE_FUNC_EXPARA* pExPara=(COMPARE_FUNC_EXPARA*)lPara;
	int iSubItem=0;
	BOOL bAscending=true;
	if(pExPara)
	{
		iSubItem=pExPara->iSubItem;
		bAscending=pExPara->bAscending;
	}
	CString sText1=pItem1->m_lpNodeInfo->GetSubItemText(iSubItem);
	CString sText2=pItem2->m_lpNodeInfo->GetSubItemText(iSubItem);
	if(StrCmp(sText1,sText2)==0)
		return 0;
	else if(StrCmp(sText1,sText2)>0)
		return 1;
	else 
		return -1;
}
int  FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	if(iSubItem==1)
	{
		POSITION pos =pListCtrl->GetFirstSelectedItemPosition();
		if(pos!=NULL)
		{
			int iCurSel = pListCtrl->GetNextSelectedItem(pos);
			pListCtrl->SetItemText(iCurSel,1,sTextValue);
			pListCtrl->SetItemState(iCurSel,0,LVIS_SELECTED);
		}
		pListCtrl->SetSubItemText(pSelItem,1,sTextValue);
	}
	return 0;
}


BOOL CDisplayPartInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//初始化listctrl
	long col_wide_arr[5]={80,70,80,80,90};
	m_displayPartInfoListCtrl.InitListCtrl(col_wide_arr);
	m_displayPartInfoListCtrl.EnableSortItems();
	m_displayPartInfoListCtrl.SetCompareItemFunc(FireCompareItem);
	m_displayPartInfoListCtrl.SetModifyValueFunc(FireValueModify);
	m_displayPartInfoListCtrl.DeleteAllItems();
	CLDSPart *pPart=NULL;
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
		if(pParamPlate->IsFL()||pParamPlate->IsInsertPlate())
		{
			for(ATTACHPART_ITEM* pItem=pParamPlate->AttachPart.GetFirst();pItem;pItem=pParamPlate->AttachPart.GetNext())
				hashDatumPartByHandle.SetValue(pItem->h,pParamPlate);
		}
	}
	//
	for(pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{	//插入一行
		CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
		lpInfo->SetSubItemText(0,CXhChar16("0x%X",pPart->handle),TRUE);	//句柄
		lpInfo->SetSubItemText(1,RetrieveSegIByPartNo(pPart,hashDatumPartByHandle).ToString(),FALSE);
		lpInfo->SetSubItemText(2,pPart->iSeg.ToString(),TRUE);
		//件号
		if(pPart->GetClassTypeId() == CLS_BOLT)//螺栓
		{
			CLDSPart *pLocationPart= Ta.FromPartHandle(((CLDSBolt*)pPart)->des_base_pos.hPart);
			if(pLocationPart)
				lpInfo->SetSubItemText(3,pLocationPart->GetPartNo(),TRUE);
		}
		else
			lpInfo->SetSubItemText(3,pPart->GetPartNo(),TRUE);
		//规格
		CXhChar100 sSpec;
		pPart->GetSpecification(sSpec);
		lpInfo->SetSubItemText(4,_T(sSpec),TRUE);
		CSuperGridCtrl::CTreeItem *pItem=m_displayPartInfoListCtrl.InsertRootItem(lpInfo);
		pItem->m_idProp= (DWORD)pPart;
	}
	UpdateData();
	return TRUE;
}
void CDisplayPartInfoDlg::OnOK()
{
	int	 nItem = m_displayPartInfoListCtrl.GetCount();
	for(int i =0; i<nItem;i++)
	{
		CString str=m_displayPartInfoListCtrl.GetItemText(i,1);
		if(str.IsEmpty())
			continue;
		CSuperGridCtrl::CTreeItem* pItem=m_displayPartInfoListCtrl.GetTreeItem(i);
		CLDSPart* pPart = (CLDSPart*)pItem->m_idProp;
		int Segi=0;
		sscanf(str,"%s",&Segi);
		if(pPart)
			pPart->iSeg= Segi;
	}
	UpdateData();
	CDialogEx::OnOK();
}



void CDisplayPartInfoDlg::OnNMClickDisplayPartInfoListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	SetFocus();
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	int iItem = m_displayPartInfoListCtrl.GetSelectedItem();
	CSuperGridCtrl::CTreeItem* pItem=m_displayPartInfoListCtrl.GetTreeItem(iItem);
	CLDSPart* pPart = (CLDSPart*)pItem->m_idProp;
	// 构件是否在十字插板或者法兰上
	CLDSPart* pDatumPart=hashDatumPartByHandle.GetValue(pPart->handle);
	CLDSPart* pCurPart=NULL;
	if(pDatumPart==NULL)
	{
		CLDSPart* pCurPart=Ta.FromPartHandle(pPart->handle);
		if(pCurPart&&pCurPart->GetClassTypeId()==CLS_BOLT)
		{
			pCurPart=Ta.FromPartHandle(((CLDSBolt*)pCurPart)->des_base_pos.hPart);
			if(pCurPart&&hashDatumPartByHandle.GetValue(pCurPart->handle)!=NULL)
				pDatumPart=hashDatumPartByHandle.GetValue(pCurPart->handle);
		}
	}	
	//相同的法兰和十字插板 设置选中
	int nItemCoun = m_displayPartInfoListCtrl.GetCount();
	for (int i=0;i<nItemCoun;i++)
	{
		pItem=m_displayPartInfoListCtrl.GetTreeItem(iItem);
		CLDSPart* pItemPart = (CLDSPart*)pItem->m_idProp;
		if(pDatumPart!=NULL)
		{
			if(pItemPart->GetClassTypeId()==CLS_BOLT)
			{
				pCurPart=Ta.FromPartHandle(((CLDSBolt*)pCurPart)->des_base_pos.hPart);
				if(pCurPart&&hashDatumPartByHandle.GetValue(pCurPart->handle)!=NULL)
					pDatumPart=hashDatumPartByHandle.GetValue(pCurPart->handle);
				else
					pCurPart=hashDatumPartByHandle.GetValue(pItemPart->handle);
			}
			if(pCurPart&&pDatumPart==pCurPart)
				m_displayPartInfoListCtrl.SetItemState(i,LVIS_SELECTED|LVNI_FOCUSED,LVIS_SELECTED|LVNI_FOCUSED);
		}
	}
	UpdateData(FALSE);
}


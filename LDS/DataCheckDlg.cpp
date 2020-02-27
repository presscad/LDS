// DataCheckDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "DataCheckDlg.h"
#include "afxdialogex.h"
#include "OriginalData.h"
#include "SortFunc.h"
#include "env_def.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "CheckPartDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
static int FireCompareItem(const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem1,const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem2,DWORD lPara)
{
	COMPARE_FUNC_EXPARA* pExPara=(COMPARE_FUNC_EXPARA*)lPara;
	int iSubItem=0,result=0;
	BOOL bAscending=true;
	if(pExPara)
	{
		iSubItem=pExPara->iSubItem;
		bAscending=pExPara->bAscending;
	}
	CString sText1=pItem1->m_lpNodeInfo->GetSubItemText(iSubItem);
	CString sText2=pItem2->m_lpNodeInfo->GetSubItemText(iSubItem);
	result=sText1.Compare(sText2);	
	if(!bAscending)
		result*=-1;
	return result;
}
static void InsertOrUpdatePartTreeItem(CSuperGridCtrl &xListCtrl,CDataCheckModel::PART_CHECK_ITEM *pPartCheckItem,
												CSuperGridCtrl::CTreeItem *pParentItem=NULL)
{
	COLORREF clr1=RGB(192,192,192),clr2=RGB(176,224,230);
	CSuperGridCtrl::CTreeItem *pSonItem=NULL;
	if(pParentItem)
	{
		xListCtrl.DeleteAllSonItems(pParentItem);
		for(int i=1;i<10;i++)	//去除背景色
			pParentItem->m_lpNodeInfo->SetSubItemColor(i,-1);
	}
	//
	for(int i=0;i<1;i++)	//暂不能显示放样模型中的数据，否则影响显示下一条目　wjh-2016.12.07
	{
		if(i==1 && !pPartCheckItem->StateMessage.GetLength())
			break;
		//CDataCheckModel::PART_DATA* pCurrData=(i==0)? &pPartCheckItem->model : &pPartCheckItem->drawing;
		CDataCheckModel::PART_DATA* pCurrData=&pPartCheckItem->drawing;
		if(i==0)
		{
			CListCtrlItemInfo *lpInfo=NULL;
			if(pParentItem)
				lpInfo=pParentItem->m_lpNodeInfo;
			else
				lpInfo=new CListCtrlItemInfo();
			//状态
			lpInfo->SetSubItemText(0,"",TRUE);
			lpInfo->SetSubItemText(0,pPartCheckItem->StateMessage,TRUE);
			//编号
			lpInfo->SetSubItemText(1,pCurrData->m_sPartNo,TRUE);
			lpInfo->SetSubItemColor(1,clr1);
			//材质
			lpInfo->SetSubItemText(2,pPartCheckItem->GetMatStr(),TRUE);
			if(pPartCheckItem->m_dwFlag&MARK_CMAT)
				lpInfo->SetSubItemColor(2,clr1);
			//规格
			lpInfo->SetSubItemText(3,pPartCheckItem->GetGuige(),TRUE);
			if(pPartCheckItem->m_dwFlag&MARK_SIZE || pPartCheckItem->m_dwFlag&MARK_SIZE)
				lpInfo->SetSubItemColor(3,clr1);
			//单基数
			lpInfo->SetSubItemText(4,CXhChar16("%d",pCurrData->nCount),TRUE);
			if(pPartCheckItem->m_dwFlag&MARK_COUNT)
				lpInfo->SetSubItemColor(4,clr1);
			//长度
			lpInfo->SetSubItemText(5,CXhChar16("%.2f",pCurrData->m_fLength),TRUE);
			if(pPartCheckItem->m_dwFlag&MARK_LENGTH)
				lpInfo->SetSubItemColor(5,clr1);
			//M16
			lpInfo->SetSubItemText(6,CXhChar16("%d",pCurrData->nM16Bolt),TRUE);
			if(pPartCheckItem->m_dwFlag&MARK_M16BOLT)
				lpInfo->SetSubItemColor(6,clr1);
			//M20
			lpInfo->SetSubItemText(7,CXhChar16("%d",pCurrData->nM20Bolt),TRUE);
			if(pPartCheckItem->m_dwFlag&MARK_M20BOLT)
				lpInfo->SetSubItemColor(7,clr1);
			//M24
			lpInfo->SetSubItemText(8,CXhChar16("%d",pCurrData->nM24Bolt),TRUE);
			if(pPartCheckItem->m_dwFlag&MARK_M24BOLT)
				lpInfo->SetSubItemColor(8,clr1);
			//其他
			lpInfo->SetSubItemText(9,CXhChar16("%d",pCurrData->nElseBolt),TRUE);
			if(pPartCheckItem->m_dwFlag&MARK_ELSEBOLT)
				lpInfo->SetSubItemColor(9,clr1);
			if(pParentItem==NULL)
			{
				pParentItem=xListCtrl.InsertRootItem(lpInfo);
				pParentItem->m_idProp=(long)pPartCheckItem;
			}
			else
				xListCtrl.RefreshItem(pParentItem);
		}
		else
		{	
			BOOL bAddItemInfo=FALSE;
			CListCtrlItemInfo *lpInfo=new CListCtrlItemInfo();
			if(pPartCheckItem->m_dwFlag&MARK_CMAT && pPartCheckItem->drawing.cMaterial!=pPartCheckItem->model.cMaterial)
			{	//材质
				bAddItemInfo=TRUE;
				lpInfo->SetSubItemText(2,pPartCheckItem->GetMatStr(FALSE),TRUE);
				lpInfo->SetSubItemColor(2,clr2);
			}
			if((pPartCheckItem->m_dwFlag&MARK_SIZE&&pPartCheckItem->drawing.m_fThick!=pPartCheckItem->model.m_fThick)
				||(pPartCheckItem->m_dwFlag&MARK_SIZE&&pPartCheckItem->drawing.m_fWidth!=pPartCheckItem->model.m_fWidth))
			{	//规格
				bAddItemInfo=TRUE;
				lpInfo->SetSubItemText(3,pPartCheckItem->GetGuige(FALSE),TRUE);
				lpInfo->SetSubItemColor(3,clr2);
			}
			if(pPartCheckItem->m_dwFlag&MARK_COUNT&&pPartCheckItem->drawing.nCount!=pPartCheckItem->model.nCount)
			{	//数量
				bAddItemInfo=TRUE;
				lpInfo->SetSubItemText(4,CXhChar16("%d",pPartCheckItem->drawing.nCount),TRUE);
				lpInfo->SetSubItemColor(4,clr2);
			}
			if(pPartCheckItem->m_dwFlag&MARK_LENGTH&&fabs(pPartCheckItem->drawing.m_fLength-pPartCheckItem->model.m_fLength)>5)
			{	//长度
				bAddItemInfo=TRUE;
				lpInfo->SetSubItemText(5,CXhChar16("%.2f",pPartCheckItem->drawing.m_fLength),TRUE);
				lpInfo->SetSubItemColor(5,clr2);
			}
			if(pPartCheckItem->m_dwFlag&MARK_M16BOLT&&pPartCheckItem->drawing.nM16Bolt!=pPartCheckItem->model.nM16Bolt)
			{	//M16螺栓数
				bAddItemInfo=TRUE;
				lpInfo->SetSubItemText(6,CXhChar16("%d",pPartCheckItem->drawing.nM16Bolt),TRUE);
				lpInfo->SetSubItemColor(6,clr2);
			}
			if(pPartCheckItem->m_dwFlag&MARK_M20BOLT&&pPartCheckItem->drawing.nM20Bolt!=pPartCheckItem->model.nM20Bolt)
			{	//M20螺栓数
				bAddItemInfo=TRUE;
				lpInfo->SetSubItemText(7,CXhChar16("%d",pPartCheckItem->drawing.nM20Bolt),TRUE);
				lpInfo->SetSubItemColor(7,clr2);
			}
			if(pPartCheckItem->m_dwFlag&MARK_M24BOLT&&pPartCheckItem->drawing.nM24Bolt!=pPartCheckItem->model.nM24Bolt)
			{	//M24螺栓数
				bAddItemInfo=TRUE;
				lpInfo->SetSubItemText(8,CXhChar16("%d",pPartCheckItem->drawing.nM24Bolt),TRUE);
				lpInfo->SetSubItemColor(8,clr2);
			}
			if(pPartCheckItem->m_dwFlag&MARK_ELSEBOLT&&pPartCheckItem->drawing.nElseBolt!=pPartCheckItem->model.nElseBolt)
			{	//其他螺栓数
				bAddItemInfo=TRUE;
				lpInfo->SetSubItemText(9,CXhChar16("%d",pPartCheckItem->drawing.nElseBolt),TRUE);
				lpInfo->SetSubItemColor(9,clr2);
			}
			if(bAddItemInfo)
			{
				pParentItem->m_bHideChildren=FALSE;
				pSonItem=xListCtrl.InsertItem(pParentItem,lpInfo,-1,true);
				pSonItem->m_idProp=(long)pPartCheckItem;
			}
			else
				delete lpInfo;
		}
	}
}
//设置选中模式
SCOPE_STRU GetPartScope(CLDSPart* pPart,SCOPE_STRU *pScope=NULL);
void SelectPart(CLDSPart* pPart)
{
	if(pPart==NULL)
		return;	
	SCOPE_STRU scope;
	f3dPoint center;
	CLDSView* pView=(CLDSView*)((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
	pView->UpdatePropertyPage(pPart);
	scope=GetPartScope(pPart);
	scope.centre(&center.x,&center.y,&center.z);
	g_pSolidDraw->SetEntSnapStatus(pPart->handle);
	g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
	g_pSolidSet->SetOperType(OPER_ROTATE);
	g_pSolidSet->SetZoomStyle(ROT_CENTER);
	g_pSolidSet->SetRotOrg(center);
	g_pSolidDraw->Draw();
}
static BOOL _LocalKeyDownItemFunc(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,LV_KEYDOWN* pLVKeyDow)
{
	if(pItem==NULL)
		return FALSE;
	CDataCheckModel::PART_CHECK_ITEM* pPartCheckItem=NULL;
	pPartCheckItem=(CDataCheckModel::PART_CHECK_ITEM*)pItem->m_idProp;
	CLDSPart* pPart=pPartCheckItem!=NULL?(CLDSPart*)Ta.FromPartNo(pPartCheckItem->m_sKey):NULL;
	if(pPart==NULL)
		return FALSE;
	SelectPart(pPart);
	CCheckPartDataDlg dlg;
	dlg.SetPart(pPart);
	if(dlg.DoModal()==IDOK)
	{
		pPartCheckItem->RefreshLoftPartInfo();
		InsertOrUpdatePartTreeItem(*pListCtrl,pPartCheckItem,pItem);
	}
	g_pSolidDraw->ReleaseSnapStatus();
	pListCtrl->SetFocus();
	CSuperGridCtrl::CTreeItem* pNextItem=pListCtrl->GetTreeItem(pItem->GetIndex()+1);
	if(pNextItem)
		pListCtrl->SelectItem(pNextItem,0,FALSE,TRUE);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// CDataCheckDlg 对话框
IMPLEMENT_DYNCREATE(CDataCheckDlg, CDialog)
CDataCheckDlg::CDataCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataCheckDlg::IDD, pParent)
{
	m_sSeg="*";
	m_sHelp="校审项共0项，已校审0项";
	m_bSelAngle=TRUE;
	m_bSelPlate=TRUE;
}

CDataCheckDlg::~CDataCheckDlg()
{
}

void CDataCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_CHECK_TYPE, m_cmbCheckType);
	DDX_Control(pDX, IDC_LIST_DATA, m_xDataListCtrl);
	DDX_Text(pDX, IDC_E_CHECK_ISEG,m_sSeg);
	DDX_Text(pDX, IDC_E_HELP,m_sHelp);
	DDX_Check(pDX, IDC_CHECK_ANGLE, m_bSelAngle);
	DDX_Check(pDX, IDC_CHECK_PLATE, m_bSelPlate);
}


BEGIN_MESSAGE_MAP(CDataCheckDlg, CDialog)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_CMB_CHECK_TYPE, &CDataCheckDlg::OnCbnSelchangeCmbCheckType)
	ON_EN_CHANGE(IDC_E_CHECK_ISEG, &CDataCheckDlg::OnEnChangeECheckIseg)
	ON_BN_CLICKED(IDC_BN_UPDATE, &CDataCheckDlg::OnBnClickedBnUpdate)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DATA, &CDataCheckDlg::OnNMRClickCheckDataList)
	ON_COMMAND(ID_MARK_NORMAL, &CDataCheckDlg::OnMarkCheck)
	ON_COMMAND(ID_CANCEL_MARK, &CDataCheckDlg::OnCancelMark)
	ON_BN_CLICKED(IDC_CHECK_ANGLE, &CDataCheckDlg::OnBnClickedCheckAngle)
	ON_BN_CLICKED(IDC_CHECK_PLATE, &CDataCheckDlg::OnBnClickedCheckPlate)
END_MESSAGE_MAP()

// CDataCheckDlg 消息处理程序
BOOL CDataCheckDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	RelayoutWnd();
	//
#ifdef AFX_TARG_ENU_ENGLISH
	m_cmbCheckType.AddString("0.Control Data");
	m_cmbCheckType.AddString("1.Part Data");
#else
	m_cmbCheckType.AddString("0.控制尺寸");
	m_cmbCheckType.AddString("1.构件数据");
#endif
	m_cmbCheckType.SetCurSel(1);
	//
	m_xDataListCtrl.EnableSortItems();
	m_xDataListCtrl.SetCompareItemFunc(FireCompareItem);
	m_xDataListCtrl.SetKeyDownItemFunc(_LocalKeyDownItemFunc);
	RefreshList();
	m_xDataListCtrl.EnableSortItems();
	m_xDataListCtrl.SetCompareItemFunc(FireCompareItem);
	return TRUE;
}
void CDataCheckDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	RelayoutWnd();
}
void CDataCheckDlg::OnOK()
{	//实现OnOK函数，否则对话框处于激活状态时按Enter键会关闭对话框

}
void CDataCheckDlg::OnCancel() 
{	//实现OnCancel函数，否则对话框处于激活状态时按ECS键会关闭对话框
}
void CDataCheckDlg::OnCbnSelchangeCmbCheckType()
{
	RelayoutWnd();
	RefreshList();
}

void CDataCheckDlg::OnEnChangeECheckIseg()
{
	UpdateData(TRUE);
	RefreshList(FALSE);
}
void CDataCheckDlg::OnBnClickedBnUpdate()
{
	if(m_cmbCheckType.GetCurSel()==0)
		xDataCheckModel.ExtractCoordCheckItems();
	else
		xDataCheckModel.ExtractPartCheckItems();
	RefreshList(FALSE);
}
void CDataCheckDlg::OnNMRClickCheckDataList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurSel = m_xDataListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
	if(iCurSel<0)
		return;
	CPoint point;
	GetCursorPos(&point);
	ContextMenu(this,point);
	*pResult = 0;
}
void CDataCheckDlg::OnMarkCheck()
{
	CDataCheckModel::PART_CHECK_ITEM* pPartCheck=NULL;
	POSITION pos = m_xDataListCtrl.GetFirstSelectedItemPosition();
	g_pSolidDraw->ReleaseSnapStatus();
	while(pos!=NULL)
	{
		int iCurSel = m_xDataListCtrl.GetNextSelectedItem(pos);
		CSuperGridCtrl::CTreeItem *pItem=m_xDataListCtrl.GetTreeItem(iCurSel);
		pPartCheck=(CDataCheckModel::PART_CHECK_ITEM*)pItem->m_idProp;
		CLDSPart* pPart=(CLDSPart*)Ta.FromPartNo(pPartCheck->m_sKey);
		if(pPart==NULL)
			continue;
		SelectPart(pPart);
		CCheckPartDataDlg dlg;
		dlg.SetPart(pPart);
		if(dlg.DoModal()!=IDOK)
			return;
		pPartCheck->RefreshLoftPartInfo();
		InsertOrUpdatePartTreeItem(m_xDataListCtrl,pPartCheck,pItem);
		g_pSolidDraw->ReleaseSnapStatus();
	}
	RefreshHelpStr();
}
void CDataCheckDlg::OnCancelMark()
{
	CDataCheckModel::PART_CHECK_ITEM* pPartCheck=NULL;
	POSITION pos = m_xDataListCtrl.GetFirstSelectedItemPosition();
	while(pos!=NULL)
	{
		int iCurSel = m_xDataListCtrl.GetNextSelectedItem(pos);
		CSuperGridCtrl::CTreeItem *pItem=m_xDataListCtrl.GetTreeItem(iCurSel);
		pPartCheck=(CDataCheckModel::PART_CHECK_ITEM*)pItem->m_idProp;
		if(pPartCheck->m_dwFlag>0)
		{
			pPartCheck->CleanFlag();
			InsertOrUpdatePartTreeItem(m_xDataListCtrl,pPartCheck,pItem);
		}
	}
	RefreshHelpStr();
}
void CDataCheckDlg::OnBnClickedCheckAngle()
{
	m_bSelAngle=!m_bSelAngle;
	RefreshList(FALSE);
}
void CDataCheckDlg::OnBnClickedCheckPlate()
{
	m_bSelPlate=!m_bSelPlate;
	RefreshList(FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CDataCheckDlg::RelayoutWnd()
{
	int iCurSel=0,iStart=0;
	CRect rc,rcTop,rcTop1,rcTop2,rcTop3,rcTop4,rcButton,rcCMB,rcBtm;
	GetClientRect(&rc);
	CWnd *pTopWnd  = GetDlgItem(IDC_CMB_CHECK_TYPE);
	CWnd *pTopWnd1 = GetDlgItem(IDC_S_ISEG);
	CWnd *pTopWnd2 = GetDlgItem(IDC_E_CHECK_ISEG);
	CWnd *pTopWnd3 = GetDlgItem(IDC_CHECK_ANGLE);
	CWnd *pTopWnd4 = GetDlgItem(IDC_CHECK_PLATE);
	CWnd *pTopBtn  = GetDlgItem(IDC_BN_UPDATE);
	CWnd *pCmbWnd  = GetDlgItem(IDC_LIST_DATA);
	CWnd *pBtmWnd  = GetDlgItem(IDC_E_HELP);
	if(pTopWnd)
	{
		pTopWnd->GetWindowRect(&rcTop);
		ScreenToClient(&rcTop);
		iCurSel=m_cmbCheckType.GetCurSel();
	}
	if(pTopWnd1)
	{	
		pTopWnd1->GetWindowRect(&rcTop1);
		ScreenToClient(&rcTop1);
	}
	if(pTopWnd2)
	{
		pTopWnd2->GetWindowRect(&rcTop2);
		ScreenToClient(&rcTop2);
	}
	if(pTopWnd3)
	{
		pTopWnd3->GetWindowRect(&rcTop3);
		ScreenToClient(&rcTop3);
	}
	if(pTopWnd4)
	{
		pTopWnd4->GetWindowRect(&rcTop4);
		ScreenToClient(&rcTop4);
	}
	if(pTopBtn)
	{
		pTopBtn->GetWindowRect(&rcButton);
		ScreenToClient(&rcButton);
	}
	if(pCmbWnd)
	{	
		pCmbWnd->GetWindowRect(&rcCMB);
		ScreenToClient(&rcCMB);
	}
	if(pBtmWnd)
	{
		pBtmWnd->GetWindowRect(&rcBtm);
		ScreenToClient(&rcBtm);
	}
	//
	int nWidth=0,nHeigh=0;
	nWidth=rcTop.right-rcTop.left;
	nHeigh=rcTop.bottom-rcTop.top;
	rcTop.top=rcTop.left=0;
	rcTop.bottom=nHeigh;
	rcTop.right=nWidth;
	if(pTopWnd)
		pTopWnd->MoveWindow(&rcTop);
	nWidth=rcTop1.right-rcTop1.left;
	nHeigh=rcTop1.bottom-rcTop1.top;
	rcTop1.left=rcTop.right+1;
	rcTop1.right=rcTop1.left+nWidth;
	rcTop1.top=4;
	rcTop1.bottom=rcTop1.top+nHeigh;
	if(pTopWnd1)
		pTopWnd1->MoveWindow(&rcTop1);
	nWidth=rcTop2.right-rcTop2.left;
	nHeigh=rcTop2.bottom-rcTop2.top;
	rcTop2.left=rcTop1.right+1;
	rcTop2.right=rcTop2.left+nWidth;
	rcTop2.top=0;
	rcTop2.bottom=nHeigh;
	if(pTopWnd2)
		pTopWnd2->MoveWindow(&rcTop2);
	if(iCurSel==1)
	{
		int nMaxH=rcTop2.Height();
		nWidth=rcTop3.right-rcTop3.left;
		nHeigh=rcTop3.bottom-rcTop3.top;
		rcTop3.left=rcTop2.right+1;
		rcTop3.right=rcTop3.left+nWidth;
		rcTop3.top=(int)((nMaxH-nHeigh)*0.5);
		rcTop3.bottom=rcTop3.top+nHeigh;
		if(pTopWnd3)
			pTopWnd3->MoveWindow(&rcTop3);
		nWidth=rcTop4.right-rcTop4.left;
		nHeigh=rcTop4.bottom-rcTop4.top;
		rcTop4.left=rcTop3.right+1;
		rcTop4.right=rcTop4.left+nWidth;
		rcTop4.top=(int)((nMaxH-nHeigh)*0.5);
		rcTop4.bottom=rcTop4.top+nHeigh;
		if(pTopWnd4)
			pTopWnd4->MoveWindow(&rcTop4);
		iStart=rcTop4.right+3;
		if(pTopWnd3)
			pTopWnd3->ShowWindow(SW_NORMAL);
		if(pTopWnd4)
			pTopWnd4->ShowWindow(SW_NORMAL);
	}
	else
	{
		iStart=rcTop2.right;
		if(pTopWnd3)
			pTopWnd3->ShowWindow(SW_HIDE);
		if(pTopWnd4)
			pTopWnd4->ShowWindow(SW_HIDE);
	}
	nWidth=rcButton.right-rcButton.left;
	nHeigh=rcButton.bottom-rcButton.top;
	rcButton.left=iStart+1;
	rcButton.right=rcButton.left+nWidth;
	rcButton.top=0;
	rcButton.bottom=nHeigh;
	if(pTopBtn)
		pTopBtn->MoveWindow(&rcButton);
	nHeigh=rcBtm.bottom-rcBtm.top;
	rcBtm.left=0;
	rcBtm.right=rc.right;
	rcBtm.bottom=rc.bottom;
	rcBtm.top=rcBtm.bottom-nHeigh;
	if(pBtmWnd)
		pBtmWnd->MoveWindow(rcBtm);
	rcCMB.left=0;
	rcCMB.right=rc.right;
	rcCMB.top=rcTop2.bottom+2;
	rcCMB.bottom=rcBtm.top-2;
	if(pCmbWnd)
		pCmbWnd->MoveWindow(&rcCMB);
}

//更新列表值
void CDataCheckDlg::RefreshList(BOOL bRefreshColumn/*=TRUE*/)
{
	if(bRefreshColumn)
		RefreshColumnHeader();
	m_xDataListCtrl.DeleteAllItems();
	int iCurSelType=m_cmbCheckType.GetCurSel();
	//添加项
	CString sItemStr;
	if(iCurSelType==0)
	{	//添加控制尺寸内容
		CDataCheckModel::SIZE_DATA* pCoordData=NULL;
		for(pCoordData=xDataCheckModel.EnumFirstCoordSizeItem();pCoordData;pCoordData=xDataCheckModel.EnumNextCoordSizeItem())
		{
			CListCtrlItemInfo *lpInfo=new CListCtrlItemInfo();
			if(pCoordData->ciSizeType==0)
			{
				sItemStr.Format("关键坐标,%s",(char*)pCoordData->idStr);
			}
			else if(pCoordData->ciSizeType==1)
			{
				CString sItem;
				if(pCoordData->ciClassifyType==0)
					sItem="节间垂距";
				else if(pCoordData->ciClassifyType==1)
					sItem="分段间距";
				else if(pCoordData->ciClassifyType==2)
					sItem="横担挂线点间垂直间距";
				else if(pCoordData->ciClassifyType==3)
					sItem="塔高";
				else if(pCoordData->ciClassifyType==4)
					sItem="塔身坡段开口尺寸";
				else
					sItem="塔头(含挂线点)水平间距";
				sItemStr.Format("%s,%s",sItem,(char*)pCoordData->idStr);
			}
			else if(pCoordData->ciSizeType==2)
				sItemStr="铁塔根开";
			else if(pCoordData->ciSizeType==3)
				sItemStr="基础根开";
			else if(pCoordData->ciSizeType==4||pCoordData->ciSizeType==5)
				sItemStr="孔距";
			else if(pCoordData->ciSizeType==6)
				sItemStr="最小间隙";//"关键点到构件最小距离";
			else if(pCoordData->ciSizeType==7)
				sItemStr="最小间隙";//"构件到构件最小距离";
			else
				sItemStr="未知";
			lpInfo->SetSubItemText(1,sItemStr,TRUE);
			CSuperGridCtrl::CTreeItem* pItem=m_xDataListCtrl.InsertRootItem(lpInfo);
			pItem->m_idProp=(DWORD)pCoordData;
		}
	}
	else if(iCurSelType==1)
	{	//添加构件校审项
		CDataCheckModel::PART_CHECK_ITEM* pPartItem=NULL;
		for(pPartItem=xDataCheckModel.EnumFirPartItem();pPartItem;pPartItem=xDataCheckModel.EnumNextPartItem())
		{
			if(!m_bSelAngle&&pPartItem->model.ciPartType==1)
				continue;
			if(!m_bSelPlate&&pPartItem->model.ciPartType==3)
				continue;
			if(m_sSeg!=""&&m_sSeg!="*"&&m_sSeg!=(char*)(pPartItem->model.iSeg.ToString()))
				continue;
			InsertOrUpdatePartTreeItem(m_xDataListCtrl,pPartItem);
		}
	}
	RefreshHelpStr();
}
//更新列标题
void CDataCheckDlg::RefreshColumnHeader()
{
	m_xDataListCtrl.DeleteAllItems();
	m_xDataListCtrl.EmptyColumnHeader();
	int iCurSelType=m_cmbCheckType.GetCurSel();
	if(iCurSelType==0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_xDataListCtrl.AddColumnHeader("state");
		m_xDataListCtrl.AddColumnHeader("description");
#else
		m_xDataListCtrl.AddColumnHeader("状态");
		m_xDataListCtrl.AddColumnHeader("说明");
#endif
		long col_wide_arr[2]={50,180};
		m_xDataListCtrl.InitListCtrl(col_wide_arr,FALSE);
	}
	else if(iCurSelType==1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		m_xDataListCtrl.AddColumnHeader("state");
		m_xDataListCtrl.AddColumnHeader("part num");
		m_xDataListCtrl.AddColumnHeader("Material");
		m_xDataListCtrl.AddColumnHeader("Number of single-base");
		m_xDataListCtrl.AddColumnHeader("specifications");
		m_xDataListCtrl.AddColumnHeader("length");
		m_xDataListCtrl.AddColumnHeader("M16 bolts number");
		m_xDataListCtrl.AddColumnHeader("M20 bolts number");
		m_xDataListCtrl.AddColumnHeader("M24 bolts number");
		m_xDataListCtrl.AddColumnHeader("else bolts number");
#else
		m_xDataListCtrl.AddColumnHeader("状态",120);
		m_xDataListCtrl.AddColumnHeader("件号");
		m_xDataListCtrl.AddColumnHeader("材质");
		m_xDataListCtrl.AddColumnHeader("规格");
		m_xDataListCtrl.AddColumnHeader("数量");
		m_xDataListCtrl.AddColumnHeader("长度");
		m_xDataListCtrl.AddColumnHeader("M16");
		m_xDataListCtrl.AddColumnHeader("M20");
		m_xDataListCtrl.AddColumnHeader("M24");
		m_xDataListCtrl.AddColumnHeader("其它");
#endif
		long col_wide_arr[10]={45,75,45,60,45,75,45,45,45,45};
		m_xDataListCtrl.InitListCtrl(col_wide_arr,FALSE);
	}
}
void CDataCheckDlg::RefreshHelpStr()
{
	int nSumCheckItem=0,nMarkCheckItem=0;
	if(m_cmbCheckType.GetCurSel()==0)
	{

	}
	else if(m_cmbCheckType.GetCurSel()==1)
	{
		CDataCheckModel::PART_CHECK_ITEM* pPartCheckItem=NULL;
		POSITION pos=m_xDataListCtrl.GetRootHeadPosition();
		while(pos)
		{
			CSuperGridCtrl::CTreeItem *pItem=m_xDataListCtrl.GetNextRoot(pos);
			pPartCheckItem=(CDataCheckModel::PART_CHECK_ITEM*)pItem->m_idProp;
			if(pPartCheckItem->m_dwFlag>0)
				nMarkCheckItem++;
			nSumCheckItem++;
		}
	}
	m_sHelp.Format("校审项共%d项，已校审%d项",nSumCheckItem,nMarkCheckItem);
	UpdateData(FALSE);
}
void CDataCheckDlg::InitDataList()
{
	xDataCheckModel.ExtractPartCheckItems();
	m_cmbCheckType.SetCurSel(1);
	RefreshList();
}
void CDataCheckDlg::MarkPartData(const char* sKey)
{
	if(m_cmbCheckType.GetCurSel()==0)
		return;
	int nSumCheckItem=0,nMarkCheckItem=0;
	CDataCheckModel::PART_CHECK_ITEM* pPartCheckItem=NULL;
	POSITION pos=m_xDataListCtrl.GetRootHeadPosition();
	while(pos)
	{
		CSuperGridCtrl::CTreeItem *pItem=m_xDataListCtrl.GetNextRoot(pos);
		pPartCheckItem=(CDataCheckModel::PART_CHECK_ITEM*)pItem->m_idProp;
		if(pPartCheckItem->m_sKey.Equal(sKey))
		{
			pPartCheckItem->RefreshLoftPartInfo();
			InsertOrUpdatePartTreeItem(m_xDataListCtrl,pPartCheckItem,pItem);
			m_xDataListCtrl.SelectItem(pItem);
			break;
		}
	}
	RefreshHelpStr();
}
void CDataCheckDlg::ContextMenu(CWnd* pWnd, CPoint point) 
{
	if(GetCurSelType()>=4||GetCurSelType()<=0)
		return;
	CPoint scr_point = point;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
#ifdef AFX_TARG_ENU_ENGLISH
	pMenu->AppendMenu(MF_STRING,ID_MARK_NORMAL,"normal mark");
	pMenu->AppendMenu(MF_STRING,ID_CANCEL_MARK,"cancel mark");
#else
	pMenu->AppendMenu(MF_STRING,ID_MARK_NORMAL,"标记校审");
	pMenu->AppendMenu(MF_STRING,ID_CANCEL_MARK,"取消标记");
#endif
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}

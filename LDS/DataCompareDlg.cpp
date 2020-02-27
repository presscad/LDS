// DataCompareDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "DataCompareDlg.h"
#include "afxdialogex.h"
#include "image.h"
#include "LogFile.h"
#include "GlobalFunc.h"
#include "SortFunc.h"
#include "ComparePartNoString.h"
#include "EnlargeFileDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#if defined(__TMA_)||defined(__LMA_)
//////////////////////////////////////////////////////////////////////////
//�ص���������
static int ModifyPartItemValue(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	BOMPART *pPart=(BOMPART*)pSelItem->m_idProp;
	if(pPart==NULL)
		return 0;
	if(iSubItem==0)			//���
		pPart->sPartNo.Copy(sTextValue);
	else if(iSubItem==1)	//���
	{
		pPart->sSpec.Copy(sTextValue);
		CDataCmpModel::RestoreSpec(sTextValue,&pPart->wide,&pPart->thick);
	}
	else if(iSubItem==2)	//����
		pPart->cMaterial=QuerySteelBriefMatMark(CXhChar16("%s",sTextValue));
	else if(iSubItem==3)	//����
		pPart->length=atof(sTextValue);
	else if(iSubItem==4)	//����
		pPart->fPieceWeight=atof(sTextValue);
	else if(iSubItem==5)	//������
	{
		pPart->AddPart(0);
		pPart->AddPart(atoi(sTextValue));
	}
	return 1;
}
SCOPE_STRU GetPartScope(CLDSPart* pPart,SCOPE_STRU *pScope=NULL);
static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pItem,NM_LISTVIEW* pNMListView)
{	
	CDataCompareDlg *pDlg=(CDataCompareDlg*)pListCtrl->GetParent();
	if(pDlg==NULL)
		return FALSE;
	BOMPART *pPart=(BOMPART*)pItem->m_idProp;
	if(pPart==NULL)
		return FALSE;
	CLDSDbObject *pDbObj=(CLDSDbObject*)Ta.FromPartNo(pPart->sPartNo);
	if(pDbObj==NULL)
		return FALSE;
	CLDSView* pView=(CLDSView*)((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
	if(pView)
		pView->UpdatePropertyPage(pDbObj,FALSE,pDlg->m_hWnd);
	g_pSolidDraw->ReleaseSnapStatus();
	long hCurPart=pDbObj->handle;
	if(!g_pSolidDraw->SetEntSnapStatus(hCurPart))
	{
		if(pDbObj&&pDbObj->IsPart())
		{	
			g_pSolidDraw->NewSolidPart(((CLDSPart*)pDbObj)->GetSolidPartObject());
			g_pSolidDraw->SetEntSnapStatus(hCurPart);
		}
	}
	if(pDbObj&&(pDbObj->IsPart()||pDbObj->IsNode()))
	{
		SCOPE_STRU scope=GetPartScope((CLDSPart*)pDbObj);
		if(g_sysPara.m_bDispPartACS&&pView->m_curAppType==TA_ASSEMBLE_ENV&&((CLDSPart*)pDbObj)->IsPlate())
		{
			g_pSolidDraw->BatchClearCS(4);
			g_pSolidDraw->AddCS(pDbObj->handle,((CLDSPart*)pDbObj)->ucs);
		}
		g_pSolidOper->FocusTo(scope,g_sysPara.m_fMaxScaleScr2User);
		//
		f3dPoint center;
		g_pSolidSet->SetOperType(OPER_ROTATE);
		g_pSolidSet->SetZoomStyle(ROT_CENTER);
		scope.centre(&center.x,&center.y,&center.z);
		g_pSolidSet->SetRotOrg(center);
		g_pSolidDraw->Draw();
	}
	return TRUE;
}
//
static CSuperGridCtrl::CTreeItem *InsertPartToList(CSuperGridCtrl &list,CSuperGridCtrl::CTreeItem *pParentItem,
	BOMPART *pPart,CHashStrList<BOOL> *pHashBoolByPropName=NULL,BOOL bReadOnly=TRUE)
{
	COLORREF clr=RGB(230,100,230);
	CListCtrlItemInfo *lpInfo=new CListCtrlItemInfo();
	//���
	lpInfo->SetSubItemText(0,pPart->sPartNo,bReadOnly);
	//���
	lpInfo->SetSubItemText(1,pPart->sSpec,bReadOnly);
	lpInfo->SetControlType(1,GCT_CMB_EDIT);
	lpInfo->SetListItemsStr(1,MakeAngleSetString('L',true));
	if(pHashBoolByPropName&&pHashBoolByPropName->GetValue("spec"))
		lpInfo->SetSubItemColor(1,clr);
	//����
	CXhChar16 sMaterial;
	QuerySteelMatMark(pPart->cMaterial,sMaterial);
	lpInfo->SetSubItemText(2,(char*)sMaterial,bReadOnly);
	lpInfo->SetControlType(2,GCT_CMB_LIST);
	lpInfo->SetListItemsStr(2,MakeMaterialMarkSetString());
	if(pHashBoolByPropName&&pHashBoolByPropName->GetValue("cMaterial"))
		lpInfo->SetSubItemColor(2,clr);
	//����
	lpInfo->SetSubItemText(3,CXhChar50("%.f",pPart->length),bReadOnly);	
	if(pHashBoolByPropName&&pHashBoolByPropName->GetValue("m_fLength"))
		lpInfo->SetSubItemColor(3,clr);
	//������
	lpInfo->SetSubItemText(4,CXhChar50("%d",pPart->GetPartNum()),bReadOnly);
	if(pHashBoolByPropName&&pHashBoolByPropName->GetValue("m_nDanJiNum"))
		lpInfo->SetSubItemColor(4,clr);
	//����
	CXhChar50 sValue("%.2f",pPart->fPieceWeight);
	lpInfo->SetSubItemText(5,sValue,bReadOnly);
	if(pHashBoolByPropName&&pHashBoolByPropName->GetValue("m_fWeight"))
		lpInfo->SetSubItemColor(5,clr);
	//�ӹ���
	lpInfo->SetSubItemText(6,CXhChar50("%d",0),bReadOnly);
	if(pHashBoolByPropName&&pHashBoolByPropName->GetValue("feature"))
		lpInfo->SetSubItemColor(6,clr);
	CSuperGridCtrl::CTreeItem *pItem=NULL;
	if(pParentItem)
		pItem=list.InsertItem(pParentItem,lpInfo,-1,false);
	else
		pItem=list.InsertRootItem(lpInfo,FALSE);
	pItem->m_idProp=(long)pPart;
	return pItem;
}
static int _LocalFireCompareItem(const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem1,const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem2,DWORD lPara)
{
	int iSubItem=0;
	BOOL bAscending=true;
	COMPARE_FUNC_EXPARA* pExPara=(COMPARE_FUNC_EXPARA*)lPara;
	if(pExPara)
	{
		iSubItem=pExPara->iSubItem;
		bAscending=pExPara->bAscending;
	}
	CString strItem1=pItem1->m_lpNodeInfo->GetSubItemText(iSubItem);
	CString strItem2=pItem2->m_lpNodeInfo->GetSubItemText(iSubItem);
	int status=0;
	if(iSubItem==0)
		status=ComparePartNoString(strItem1,strItem2);	
	else if(iSubItem==1)
		status=CompareMultiSectionString(strItem1,strItem2);	
	//�������������ַ�����Ӧ���ڷǿ��ַ���֮�� wht 12-05-25
	else if(strItem1.GetLength()==0&&strItem2.GetLength()>0)
		return 1;	
	else if(strItem1.GetLength()>0&&strItem2.GetLength()==0)
		return -1;
	else if(iSubItem==3)	//����
	{	//����������
		int nItem1=atoi(strItem1);
		int nItem2=atoi(strItem2);
		if(nItem1>nItem2)
			status=1;
		else if(nItem1<nItem2)
			status=-1;
		else
			status=0;
	}
	else if(iSubItem==5||iSubItem==6)	//����
	{	//������������
		double fItem1=atof(strItem1);
		double fItem2=atof(strItem2);
		if(fItem1>fItem2)
			status=1;
		else if(fItem1<fItem2)
			status=-1;
		else
			status=0;
	}
	else//���ַ�������
		status=strItem1.CompareNoCase(strItem2);
	if(bAscending)
		return status;	//����
	else
		return -status;//����
}
//////////////////////////////////////////////////////////////////////////
// CDataCompareDlg �Ի���
CDataCompareDlg *g_pDataCompareDlg;
IMPLEMENT_DYNAMIC(CDataCompareDlg, CDialog)

int CDataCompareDlg::nScrLocationX=0;
int CDataCompareDlg::nScrLocationY=0;
CDataCompareDlg::CDataCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataCompareDlg::IDD, pParent)
	, m_fTolerance(5)
	, m_sSeg(_T("*"))
{
	m_nRecordNum=0;
	m_bEnableWindowMoveListen=false;
	m_fWeightTolernace=0.5;
	m_hRootItem=NULL;
	m_hExterDataGroupItem=NULL;
	m_hModelItem=NULL;
}

CDataCompareDlg::~CDataCompareDlg()
{
}

void CDataCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_xListCtrl);
	DDX_Control(pDX, IDC_TREE_CTRL, m_xTreeCtrl);
	DDX_Control(pDX, IDC_TAB_CTRL, m_xTabCtrl);
	DDX_Text(pDX, IDC_E_RECORD_NUM, m_nRecordNum);
	DDX_Text(pDX, IDC_E_COMPARE_TOLERANCE, m_fTolerance);
	DDX_Text(pDX, IDC_E_COMPARE_WEIGHT_TOLERANCE, m_fWeightTolernace);
	DDX_Text(pDX, IDC_E_SEG, m_sSeg);
}


BEGIN_MESSAGE_MAP(CDataCompareDlg, CDialog)
	ON_WM_MOVE()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CTRL, &OnNMRClickTreeCtrl)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CTRL, &OnTvnSelchangedTreeContrl)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CTRL, &OnSelchangeTabCtrl)
	ON_COMMAND(ID_IMPORT_LOFT_DATA,OnImportExterBomFile)
	ON_COMMAND(ID_COMPARE_DATA,OnCompareData)
	ON_COMMAND(ID_EXPORT_RESULT,OnExportCompResult)
	ON_COMMAND(ID_RETRIEVE_DATA,OnRetrieveData)
	ON_COMMAND(ID_DELETE_ITEM,OnDeleteItem)
	ON_EN_CHANGE(IDC_E_SEG,OnEnChangeESeg)
	ON_BN_CLICKED(ID_BTN_IMPORT_DATA, &CDataCompareDlg::OnBnClickedBtnImportData)
	ON_BN_CLICKED(ID_BTN_EXPORT_RESULT, &CDataCompareDlg::OnBnClickedBtnExportResult)
	ON_BN_CLICKED(ID_BTN_LOAD_MODEL_DATA, &CDataCompareDlg::OnBnClickedBtnLoadModelData)
	ON_BN_CLICKED(ID_BTN_DATA_COMPARE, &CDataCompareDlg::OnBnClickedBtnDataCompare)
	ON_EN_CHANGE(IDC_E_COMPARE_TOLERANCE, OnChangeCompareTolerance)
	ON_EN_CHANGE(IDC_E_COMPARE_WEIGHT_TOLERANCE, OnChangeCompareTolerance)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CDataCompareDlg ��Ϣ�������
BOOL CDataCompareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CAttachBuffer* pAttachBuff=Ta.AttachBuffer.GetValue(CTower::BOM_COMPARE_MODEL);
	if(pAttachBuff)
	{
		pAttachBuff->SeekToBegin();
		globalDataCmpModel.ReadFromDrawAndBomBuffer(pAttachBuff);
		globalDataCmpModel.ComparePartData(m_fTolerance,m_fWeightTolernace);	//�ӻ���������ݺ�Ӧ���¶Ա����� wht 18-11-14
	}
	else
		globalDataCmpModel.Empty();
	//
	((CEdit*)GetDlgItem(IDC_E_RECORD_NUM))->SetReadOnly(TRUE);
	//((CEdit*)GetDlgItem(IDC_E_COMPARE_TOLERANCE))->SetReadOnly(TRUE);
	//��ʼ���б��
	m_xListCtrl.EmptyColumnHeader();
	m_xListCtrl.AddColumnHeader("�������",75);	
	m_xListCtrl.AddColumnHeader("���",80);	
	m_xListCtrl.AddColumnHeader("����",50);	
	m_xListCtrl.AddColumnHeader("����",55);	
	m_xListCtrl.AddColumnHeader("������",55);
	m_xListCtrl.AddColumnHeader("����",65);
	m_xListCtrl.AddColumnHeader("����",65);
	m_xListCtrl.EnableSortItems(false,true);
	m_xListCtrl.SetHeaderFontHW(13,0);
	m_xListCtrl.SetGridLineColor(RGB(220,220,220));
	m_xListCtrl.SetEvenRowBackColor(RGB(224,237,236));
	m_xListCtrl.InitListCtrl();
	//�趨�������ݵ�������
	m_xListCtrl.SetCompareItemFunc(_LocalFireCompareItem);
	m_xListCtrl.SetModifyValueFunc(ModifyPartItemValue);
	m_xListCtrl.SetItemChangedFunc(FireItemChanged);
	//��ʼ�����б�
	m_xTreeCtrl.ModifyStyle(0,TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT|TVS_SHOWSELALWAYS|TVS_FULLROWSELECT|TVS_DISABLEDRAGDROP);
	//
	m_xTabCtrl.DeleteAllItems();
	m_xTabCtrl.InsertItem(0,"ģ��ȱ��");
	m_xTabCtrl.InsertItem(1,"ģ�Ͷ��");
	m_xTabCtrl.InsertItem(2,"ͬ�Ų���");
	m_xTabCtrl.SetCurSel(0);
	//
	RefreshTreeCtrl();
	UpdateData(FALSE);
	//
	//�ƶ����ڵ�����λ��
	CRect rect;
	CWnd::GetWindowRect(rect);
	int width = rect.Width();
	int height=rect.Height();
	rect.left=nScrLocationX;
	rect.top=nScrLocationY;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;
	MoveWindow(rect, TRUE);
	m_bEnableWindowMoveListen=true;
	return TRUE;
}

typedef BOMPART*	BOMPART_PTR;
static int CompareBomPartPtr(const BOMPART_PTR &item1,const BOMPART_PTR &item2)
{
	CXhChar16 sPartNo1=item1->sPartNo;
	CXhChar16 sPartNo2=item2->sPartNo;
	return ComparePartNoString(sPartNo1,sPartNo2);
}
void CDataCompareDlg::RefreshListCtrl()
{
	TREEITEM_INFO *pItemInfo=GetCurItem();
	if(pItemInfo==NULL)
		return;
	m_xListCtrl.DeleteAllItems();
	if(pItemInfo->itemType==DATA_COMPARE)
	{	//�ԱȽ��
		m_nRecordNum=0;
		if(globalDataCmpModel.GetResultCount()<=0)
			return;
		//
		CHashList<SEGI> segTable;
		GetSegNoHashTblBySegStr(m_sSeg,segTable);
		int iCurSel=m_xTabCtrl.GetCurSel();
		CDataCmpModel::COMPARE_PART_RESULT *pResult=NULL;
		for(pResult=globalDataCmpModel.EnumFirstResult();pResult;pResult=globalDataCmpModel.EnumNextResult())
		{
			int iSeg=pResult->pOrgPart?pResult->pOrgPart->iSeg:pResult->pLoftPart->iSeg;
			if(segTable.GetNodeNum()>0&&segTable.GetValue(iSeg)==NULL)
				continue;
			if(iCurSel==0 && pResult->pOrgPart&&pResult->pLoftPart==NULL)
			{	//ģ��ȱ����ͼֽר�У�
				CSuperGridCtrl::CTreeItem *pItem=InsertPartToList(m_xListCtrl,NULL,pResult->pOrgPart);
				pItem->m_idProp=(long)pResult->pOrgPart;
				m_nRecordNum++;
			}
			else if(iCurSel==1 && pResult->pOrgPart==NULL&&pResult->pLoftPart)
			{	//ģ�Ͷ��������ר�У�	
				CSuperGridCtrl::CTreeItem *pItem=InsertPartToList(m_xListCtrl,NULL,pResult->pLoftPart);
				pItem->m_idProp=(long)pResult->pLoftPart;
				m_nRecordNum++;
			}
			else if(iCurSel==2 && pResult->pOrgPart&&pResult->pLoftPart)
			{	//ͼֽ�������ͬ
				CSuperGridCtrl::CTreeItem *pItem=InsertPartToList(m_xListCtrl,NULL,pResult->pOrgPart);
				pItem->m_idProp=(long)pResult->pOrgPart;
				InsertPartToList(m_xListCtrl,pItem,pResult->pLoftPart,&pResult->hashBoolByPropName);
				m_nRecordNum++;
			}
		}
	}
	else if(pItemInfo->itemType==DRAWING_BOM_GROUP)
	{	//��ʾ�������ݻ��ܱ�
		m_nRecordNum=0;
		for(CTowerBOM* pDrawBom=globalDataCmpModel.EnumFirstDrawingData();pDrawBom;pDrawBom=globalDataCmpModel.EnumNextDrawingData())
		{
			for(BOMPART *pPart=pDrawBom->EnumFirstPart();pPart;pPart=pDrawBom->EnumNextPart())
			{
				InsertPartToList(m_xListCtrl,NULL,pPart,NULL);
				m_nRecordNum++;
			}
		}
	}
	else if(pItemInfo->itemType==DRAWING_BOM_FILE)
	{	//��ʾͼֽ����
		m_nRecordNum=0;
		CTowerBOM* pDrawBom=(CTowerBOM*)pItemInfo->dwRefData;
		if(pDrawBom==NULL)
		{
			logerr.Log("ͼֽ��������!");
			return;
		}
		m_nRecordNum=pDrawBom->GetPartNum();
		ARRAY_LIST<BOMPART*> partPtrList;
		for(BOMPART *pPart=pDrawBom->EnumFirstPart();pPart;pPart=pDrawBom->EnumNextPart())
			partPtrList.append(pPart);
		CHeapSort<BOMPART*>::HeapSort(partPtrList.m_pData,partPtrList.GetSize(),CompareBomPartPtr);
		for(int i=0;i<partPtrList.GetSize();i++)
			InsertPartToList(m_xListCtrl,NULL,partPtrList[i],NULL);
	}
	else if(pItemInfo->itemType==CURRENT_MODEL)
	{	//��ʾ������������
		m_nRecordNum=0;
		CTowerBOM* pLoftBom=(CTowerBOM*)pItemInfo->dwRefData;
		if(pLoftBom==NULL)
		{
			logerr.Log("������������!");
			return;
		}
		m_nRecordNum=pLoftBom->GetPartNum();
		ARRAY_LIST<BOMPART*> partPtrList;
		for(BOMPART *pPart=pLoftBom->EnumFirstPart();pPart;pPart=pLoftBom->EnumNextPart())
			partPtrList.append(pPart);
		CHeapSort<BOMPART*>::HeapSort(partPtrList.m_pData,partPtrList.GetSize(),CompareBomPartPtr);
		for(int i=0;i<partPtrList.GetSize();i++)
			InsertPartToList(m_xListCtrl,NULL,partPtrList[i],NULL);
	}
	m_xListCtrl.Redraw();
}
void CDataCompareDlg::RefreshTreeCtrl()
{
	itemInfoList.Empty();
	m_xTreeCtrl.DeleteAllItems();
	//
	TREEITEM_INFO *pItemInfo=NULL;
	m_hRootItem=m_xTreeCtrl.InsertItem("���ݶԱ�",PRJ_IMG_CALMODULE,PRJ_IMG_CALMODULE,TVI_ROOT);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(DATA_COMPARE,NULL));
	m_xTreeCtrl.SetItemData(m_hRootItem,(DWORD)pItemInfo);
	//
	m_hExterDataGroupItem=m_xTreeCtrl.InsertItem("��������",PRJ_IMG_MODULECASE,PRJ_IMG_MODULECASE,m_hRootItem);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(DRAWING_BOM_GROUP,NULL));
	m_xTreeCtrl.SetItemData(m_hExterDataGroupItem,(DWORD)pItemInfo);
	for(CTowerBOM *pDrawBom=globalDataCmpModel.EnumFirstDrawingData();pDrawBom;pDrawBom=globalDataCmpModel.EnumNextDrawingData())
	{
		HTREEITEM hBomItem=m_xTreeCtrl.InsertItem(pDrawBom->m_sBomName,PRJ_IMG_PARA,PRJ_IMG_PARA,m_hExterDataGroupItem);
		pItemInfo=itemInfoList.append(TREEITEM_INFO(DRAWING_BOM_FILE,(DWORD)pDrawBom));
		m_xTreeCtrl.SetItemData(hBomItem,(DWORD)pItemInfo);
	}
	//
	m_hModelItem=m_xTreeCtrl.InsertItem("ģ������",PRJ_IMG_TOWERBODY,PRJ_IMG_TOWERBODY,m_hRootItem);
	pItemInfo=itemInfoList.append(TREEITEM_INFO(CURRENT_MODEL,(DWORD)&(globalDataCmpModel.m_loftData)));
	m_xTreeCtrl.SetItemData(m_hModelItem,(DWORD)pItemInfo);
	//�趨�����ڵ��״̬
	m_xTreeCtrl.Expand(m_hExterDataGroupItem,TVE_EXPAND);
	m_xTreeCtrl.Expand(m_hRootItem,TVE_EXPAND);
	m_xTreeCtrl.SelectItem(m_hRootItem);
}
void CDataCompareDlg::ContextMenu(CWnd *pWnd, CPoint point)
{
	//��ʼ���˵�
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	while(pMenu->GetMenuItemCount()>0)
		pMenu->DeleteMenu(0,MF_BYPOSITION);
	//��Ӳ˵���
	CPoint scr_point = point;
	m_xTreeCtrl.ClientToScreen(&scr_point);
	HTREEITEM hItem=m_xTreeCtrl.GetSelectedItem();
	TREEITEM_INFO *pItemInfo=NULL;
	if(hItem)
		pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==DATA_COMPARE)
	{
		pMenu->AppendMenu(MF_STRING,ID_COMPARE_DATA,"�������ݶԱ�");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_RESULT,"�����ԱȽ��");
	}
	else if(pItemInfo->itemType==DRAWING_BOM_GROUP)
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_LOFT_DATA,"����ͼֽ����");
	else if(pItemInfo->itemType==CURRENT_MODEL)
		pMenu->AppendMenu(MF_STRING,ID_RETRIEVE_DATA,"���ط�������");
	else if(pItemInfo->itemType==DRAWING_BOM_FILE)
		pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"ɾ��");
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
}
TREEITEM_INFO* CDataCompareDlg::GetCurItem()
{
	HTREEITEM hItem = m_xTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		return NULL;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return NULL;
	return pItemInfo;
}
//////////////////////////////////////////////////////////////////////////
//��Ϣ�¼�����
void CDataCompareDlg::OnNMRClickTreeCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	TVHITTESTINFO HitTestInfo;
	GetCursorPos(&HitTestInfo.pt);
	m_xTreeCtrl.ScreenToClient(&HitTestInfo.pt);
	m_xTreeCtrl.HitTest(&HitTestInfo);
	m_xTreeCtrl.Select(HitTestInfo.hItem,TVGN_CARET);
	ContextMenu(&m_xTreeCtrl,HitTestInfo.pt);
	*pResult = 0;
}
void CDataCompareDlg::OnTvnSelchangedTreeContrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hItem = m_xTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		return;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	m_xTabCtrl.DeleteAllItems();
	if(pItemInfo->itemType==DATA_COMPARE)
	{
		m_xTabCtrl.InsertItem(0,"ģ��ȱ��");
		m_xTabCtrl.InsertItem(1,"ģ�Ͷ��");
		m_xTabCtrl.InsertItem(2,"ͬ�Ų���");
	}
	else if(pItemInfo->itemType==CURRENT_MODEL)
		m_xTabCtrl.InsertItem(0,"ģ������");
	else if(pItemInfo->itemType==DRAWING_BOM_GROUP)
		m_xTabCtrl.InsertItem(0,"��������-����");
	else if(pItemInfo->itemType==DRAWING_BOM_FILE)
		m_xTabCtrl.InsertItem(0,CXhChar100("��������-%s",m_xTreeCtrl.GetItemText(hItem)));
	//GetDlgItem(IDC_E_SEG)->EnableWindow(pItemInfo->itemType==DATA_COMPARE);
	//GetDlgItem(IDC_E_COMPARE_TOLERANCE)->EnableWindow(pItemInfo->itemType==DATA_COMPARE);
	RefreshListCtrl();
	UpdateData(FALSE);
	*pResult = 0;
}
void CDataCompareDlg::OnDeleteItem()
{
	HTREEITEM hItem = m_xTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		return;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)m_xTreeCtrl.GetItemData(hItem);
	if(pItemInfo==NULL)
		return;
	if(pItemInfo->itemType==DRAWING_BOM_FILE)
	{
		CTowerBOM *pBom=(CTowerBOM*)pItemInfo->dwRefData;
		if(pBom!=NULL)
		{
			globalDataCmpModel.DeleteDwgBomFile(pBom);
			m_xTreeCtrl.DeleteItem(hItem);
		}
	}
}
//��ȡԭʼ����
void CDataCompareDlg::OnRetrieveData()
{
	TREEITEM_INFO *pItemInfo=GetCurItem();
	if(pItemInfo==NULL||pItemInfo->itemType!=CURRENT_MODEL)
		return;
	CTowerBOM *pLoftBom=(CTowerBOM*)pItemInfo->dwRefData;
	pLoftBom->RunSummarize(&Ta);
	RefreshListCtrl();
	UpdateData(FALSE);
}
static int CompareBomFilePath(const CXhChar500 &item1,const CXhChar500 &item2)
{
	CXhChar16 sLabel1,sLable2;
	CXhChar100 sName1,sName2;
	CXhChar500 sPrefix1,sPrefix2;
	_splitpath(item1,NULL,NULL,sName1,NULL);
	_splitpath(item2,NULL,NULL,sName2,NULL);
	sPrefix1.Copy(strtok(sName1,"#"));
	if(sPrefix1.GetLength()>0)
		sLabel1.Copy(strtok(NULL,"~"));
	else
		sLabel1.Copy(strtok(sName1,"~"));
	sPrefix2.Copy(strtok(sName2,"#"));
	if(sPrefix2.GetLength()>0)
		sLable2.Copy(strtok(NULL,"~"));
	else
		sLable2.Copy(strtok(sName2,"~"));
	if(sPrefix1.GetLength()==0)
		sPrefix1.Copy("NULL");
	if(sPrefix2.GetLength()==0)
		sPrefix2.Copy("NULL");
	//�ȱȽ�ǰ׺�ַ�����ǰ׺һ�±�ʾ����ͬһ��ͼֽ������ͬһ��
	SEGI segI1(sPrefix1),segI2(sPrefix2);
	int nRetCode=CompareSegI(segI1,segI2);
	if(nRetCode==0)
		nRetCode=ComparePartNoString(sLabel1,sLable2);
	return nRetCode;
}
//�����������
void CDataCompareDlg::OnImportExterBomFile()
{
	CLogErrorLife logErrLife;
	//�򿪷�������
	CXhChar500 filter("Excel�ļ�(*.xls)|*.xls|Excel�ļ�(*.xlsx)|*.xlsx");
	filter.Append("|Dwg��ȡ��ͼֽ����(*.bomd)|*.bomd");
	filter.Append("|ͼֽ����(*.tbom)|*.tbom");
	filter.Append("|�����ļ�(*.*)|*.*||");
	CFileDialog dlg(TRUE,"Excel�ļ�","xls",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,filter);
	dlg.m_ofn.lpstrTitle="����ͼֽ����";
	CEnlargeFileDialog enlarge(&dlg);
	if(dlg.DoModal()!=IDOK)
		return;
	//����ļ�
	ARRAY_LIST<CXhChar500> filePathList;
	POSITION pos=dlg.GetStartPosition();
	while(pos)
	{
		CString sFileName=dlg.GetNextPathName(pos);		//��ȡ�ļ��� 
		filePathList.append(CXhChar500(sFileName));
	}
	CHeapSort<CXhChar500>::HeapSort(filePathList.m_pData,filePathList.Size(),CompareBomFilePath);
	HTREEITEM hCurItem=NULL;
	for(CXhChar500 *pFileName=filePathList.GetFirst();pFileName;pFileName=filePathList.GetNext())
	{
		CXhChar500 sFileName(*pFileName);
		CXhChar16 sBomName,sExtension;
		_splitpath(sFileName,NULL,NULL,sBomName,sExtension);
		//��ȡ����
		CTowerBOM* pDrawBom=NULL;
		if(sExtension.EqualNoCase(".tbom"))
			pDrawBom=globalDataCmpModel.AppendDrawingDataFromTBOM(sFileName);
		else if(sExtension.EqualNoCase(".bomd"))
			pDrawBom=globalDataCmpModel.AppendDrawingDataFromBOMD(sFileName);
		else if(sExtension.EqualNoCase(".xls")||sExtension.EqualNoCase(".xlsx"))
			pDrawBom=globalDataCmpModel.AppendDrawingDataFromExce(sFileName);
		if(pDrawBom==NULL)
		{
			logerr.Log("��ȡͼֽ����ʧ��");	
			return ;
		}
		pDrawBom->m_sBomName=sBomName;
		//�����������
		HTREEITEM hSelectedItem=m_xTreeCtrl.GetSelectedItem();
		hCurItem=m_xTreeCtrl.InsertItem(pDrawBom->m_sBomName,PRJ_IMG_PARA,PRJ_IMG_PARA,hSelectedItem);
		TREEITEM_INFO* pItemInfo=itemInfoList.append(TREEITEM_INFO(DRAWING_BOM_FILE,(DWORD)pDrawBom));
		m_xTreeCtrl.SetItemData(hCurItem,(DWORD)pItemInfo);
		m_xTreeCtrl.Expand(hSelectedItem,TVE_EXPAND);
	}
	m_xTreeCtrl.SelectItem(hCurItem);	//����OnTvnSelchangedTreeContrl();
	//RefreshListCtrl();
	//UpdateData(FALSE);
}
//���ݶԱ�
void CDataCompareDlg::OnCompareData()
{
	TREEITEM_INFO *pItemInfo=GetCurItem();
	if(pItemInfo==NULL || pItemInfo->itemType!=DATA_COMPARE)
		return;
	UpdateData();
	globalDataCmpModel.ComparePartData(m_fTolerance,m_fWeightTolernace);
	RefreshListCtrl();
	UpdateData(FALSE);
}
//�����ԱȽ��
void CDataCompareDlg::OnExportCompResult()
{
	TREEITEM_INFO *pItemInfo=GetCurItem();
	if(pItemInfo==NULL || pItemInfo->itemType!=DATA_COMPARE)
		return;
	globalDataCmpModel.ExportCompareResult();
}
void CDataCompareDlg::OnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	TREEITEM_INFO *pItemInfo=GetCurItem();
	if(pItemInfo==NULL || pItemInfo->itemType!=DATA_COMPARE)
		return;
	RefreshListCtrl();
	UpdateData(FALSE);
}
void CDataCompareDlg::OnEnChangeESeg()
{
	UpdateData(TRUE);
	RefreshListCtrl();
	UpdateData(FALSE);
}
BOOL CDataCompareDlg::Create()
{
	return CDialog::Create(CDataCompareDlg::IDD);
}
void CDataCompareDlg::InitDataCmpDlg()
{
	m_bEnableWindowMoveListen=false;
	if(GetSafeHwnd()==0)
		Create();
	else
		OnInitDialog();
	UpdateData(FALSE);
}
void CDataCompareDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	if(m_bEnableWindowMoveListen)
	{
		nScrLocationX=x;
		nScrLocationY=y;
	}
}
void CDataCompareDlg::OnOK()
{
	CAttachBuffer* pAttachBuff=Ta.AttachBuffer.Add(CTower::BOM_COMPARE_MODEL);
	globalDataCmpModel.WriteToDrawAndBomBuffer(pAttachBuff);
	DestroyWindow();
}
void CDataCompareDlg::OnCancel()
{	//���������ӻ���
	CAttachBuffer* pAttachBuff=Ta.AttachBuffer.Add(CTower::BOM_COMPARE_MODEL);
	pAttachBuff->SeekToBegin();
	globalDataCmpModel.WriteToDrawAndBomBuffer(pAttachBuff);
	DestroyWindow();
}

void CDataCompareDlg::OnBnClickedBtnDataCompare()
{
	globalDataCmpModel.ComparePartData(m_fTolerance,m_fWeightTolernace);
	m_xTreeCtrl.SelectItem(m_hRootItem);
	RefreshListCtrl();
}

void CDataCompareDlg::OnBnClickedBtnImportData()
{
	if(m_hExterDataGroupItem==NULL)
		return;
	m_xTreeCtrl.SelectItem(m_hExterDataGroupItem);
	OnImportExterBomFile();
}

void CDataCompareDlg::OnBnClickedBtnExportResult()
{
	if(m_hRootItem==NULL)
		return;
	m_xTreeCtrl.SelectItem(m_hRootItem);
	OnExportCompResult();
}

void CDataCompareDlg::OnBnClickedBtnLoadModelData()
{
	if(m_hModelItem==NULL)
		return;
	m_xTreeCtrl.SelectItem(m_hModelItem);
	OnRetrieveData();
}

void CDataCompareDlg::OnSize(UINT nType, int cx, int cy)
{
	RECT rcFull,rcList,rcTree;
	GetClientRect(&rcFull);
	CWnd *pListWnd=GetDlgItem(IDC_LIST_CTRL);
	CWnd *pTreeWnd=GetDlgItem(IDC_TREE_CTRL);
	if(pListWnd==NULL||pTreeWnd==NULL)
		return;
	pListWnd->GetWindowRect(&rcList);
	pTreeWnd->GetWindowRect(&rcTree);
	ScreenToClient(&rcList);
	ScreenToClient(&rcTree);
	int nTreeWidth=rcTree.right-rcTree.left;
	int nFullWidth=rcFull.right-rcTree.left;
	int nFullHeight=rcFull.bottom-rcTree.top;
	rcList.right=rcList.left+(nFullWidth-nTreeWidth-20);
	rcList.bottom=rcList.top+(nFullHeight-55);
	pListWnd->MoveWindow(&rcList);

	RefreshListCtrl();
}
void CDataCompareDlg::OnChangeCompareTolerance()
{
	UpdateData();
}
#endif


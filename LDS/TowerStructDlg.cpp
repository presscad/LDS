// TowerStructDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LDS.h"
#include "TowerStructDlg.h"
#include "afxdialogex.h"
#include "GlobalFunc.h"
#include "FoundationPropDlg.h"
#include "ModelTree.h"
#include "UnifyModelLayer.h"
#include "ExcelOper.h"

#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__LDS_CONTEXT_)
#include "XhRptTbl.h"

static int COL_INDEX_HEIGHT = 0;
static int COL_INDEX_TZ		= 1;
static int COL_INDEX_SEGI	= 2;
static int COL_INDEX_RODSIZE= 3;
static int COL_INDEX_ACTUALG= 4;
static int COL_INDEX_PREFERG= 5;
static int COL_INDEX_TOPSIZEXY		= 6;
static int COL_INDEX_BTMSIZEXY		= 7;
static int COL_INDEX_BASE_SIZEXY	= 8;

typedef CModelHeightNode* MODELHEIGHT_PTR;
static int compare_func(const MODELHEIGHT_PTR& item1, const MODELHEIGHT_PTR& item2)
{
	return compare_double(item1->zLongestLeg, item2->zLongestLeg);
}

static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	TREEITEM_INFO* pItemInfo=(TREEITEM_INFO*)pSelItem->m_idProp;
	if(pItemInfo==NULL)
		return FALSE;
	if(pItemInfo->itemType==CTowerStructDlg::DATA_SEGMENT)
	{
		CModelSegmentNode* pSegment=(CModelSegmentNode*)pItemInfo->dwRefData;
		if(iSubItem==COL_INDEX_ACTUALG)
		{
			if(pSegment->pPostRod->GetClassTypeId()!=CLS_LINEANGLE)
				return FALSE;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pSegment->pPostRod;
			WORD wSpecifyG=atoi(sTextValue);
			pAngle->xWingXZhunJu=pAngle->xWingYZhunJu=CLDSLineAngle::GetAngleZJ(pAngle->size_wide);// library->GetAngleZJ(pAngle - )
			pAngle->xWingXZhunJu.g=pAngle->xWingYZhunJu.g=wSpecifyG;
			pAngle->m_bEnableTeG=true;
			pAngle->SyncMirProp("m_bEnableTeG");	//同步更新四个角上对称的主材角钢心距
		}
	}
	if(pItemInfo->itemType==CTowerStructDlg::DATA_SUBLEG)
	{
		CModelSubLegNode* pSubLeg=(CModelSubLegNode*)pItemInfo->dwRefData;
		if(iSubItem==COL_INDEX_ACTUALG)
		{
			if(pSubLeg->pPostRod->GetClassTypeId()!=CLS_LINEANGLE)
				return FALSE;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pSubLeg->pPostRod;
			WORD wSpecifyG=atoi(sTextValue);
			pAngle->xWingXZhunJu=pAngle->xWingYZhunJu=CLDSLineAngle::GetAngleZJ(pAngle->size_wide);// library->GetAngleZJ(pAngle - )
			pAngle->xWingXZhunJu.g=pAngle->xWingYZhunJu.g=wSpecifyG;
			pAngle->m_bEnableTeG=true;
			pAngle->SyncMirProp("m_bEnableTeG");	//同步更新四个角上对称的主材角钢心距
		}
	}
	return TRUE;
}

static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	return TRUE;
}

static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,NM_LISTVIEW* pNMListView)
{
	CTowerStructDlg *pDlg=(CTowerStructDlg*)pListCtrl->GetParent();
	if(!pDlg->m_bFireListItemChange)
		return FALSE;	//未启用ItemChange
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CTowerStructDlg 对话框

static double TOLERANCE=5.0;
IMPLEMENT_DYNAMIC(CTowerStructDlg, CDialog)
CTowerStructDlg::CTowerStructDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTowerStructDlg::IDD, pParent)
{
	m_bFireListItemChange=false;
	m_listBodyOpening.AddColumnHeader("坡段/分段/接腿",160);
	m_listBodyOpening.AddColumnHeader("Tz(m)",60);
	m_listBodyOpening.AddColumnHeader("段号",50);
	m_listBodyOpening.AddColumnHeader("主材规格",75);
	m_listBodyOpening.AddColumnHeader("当前心距",75);
	m_listBodyOpening.AddColumnHeader("推荐心距",75);
	m_listBodyOpening.AddColumnHeader("上开口",75);
	m_listBodyOpening.AddColumnHeader("下开口/根开",120);
	m_listBodyOpening.AddColumnHeader("基础根开",90);
	//m_listBodyOpening.AddColumnHeader("",20);
	m_fColinarTolerance = TOLERANCE;
}

CTowerStructDlg::~CTowerStructDlg()
{
}
TREEITEM_INFO* CTowerStructDlg::AddOrUpdateItemInfo(TREEITEM_INFO& iteminfo)
{
	UINT id=iteminfo.dwRefData>0?iteminfo.dwRefData:iteminfo.itemType;
	//TODO:附加风荷载不能这样处理
	TREEITEM_INFO* pItemInfo=NULL;//hashItemInfos.GetValue(id);
	if(pItemInfo==NULL)
	{
		pItemInfo=itemInfoList.append(iteminfo);
		//pItemInfo=hashItemInfos.SetValue(id,pItemInfo);
	}
	else
		*pItemInfo=iteminfo;
	return pItemInfo;
}

void CTowerStructDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BODY_OPENING, m_listBodyOpening);
	DDX_Text(pDX, IDC_E_COLINEAR_TOLERANCE, m_fColinarTolerance);
}


BEGIN_MESSAGE_MAP(CTowerStructDlg, CDialog)
	//ON_WM_SIZE()
	ON_BN_CLICKED(ID_BTN_UPDATE_TOWER_MODEL_TREE, &CTowerStructDlg::OnBnClickedBtnUpdateTowerModelTree)
	ON_BN_CLICKED(ID_BTN_ALLOC_SEGMENT_NUMBER, &CTowerStructDlg::OnBnClickedBtnAllocSegmentNumber)
	ON_BN_CLICKED(IDC_BN_EXPORT, &CTowerStructDlg::OnBnClickedBtnExportExcel)
	ON_BN_CLICKED(IDOK, &CTowerStructDlg::OnOK)
END_MESSAGE_MAP()


// CTowerStructDlg 消息处理程序
BOOL CTowerStructDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_listBodyOpening.InitListCtrl(NULL);
	//设置修改回调函数
	m_listBodyOpening.SetModifyValueFunc(FireValueModify);
	//设置帮助回调函数
	m_listBodyOpening.SetItemChangedFunc(FireItemChanged);
	//设置右键菜单回调函数
	m_listBodyOpening.SetContextMenuFunc(FireContextMenu);
	//-1表示无图像链表
	m_listBodyOpening.InitImageList(IDR_IL_LIST);//-1 
	//UpdateDialogSize(0,0);

	RetrievedTowerModelTree();
	UpdateListOpenCtrlByModelTree(NULL,NULL,NULL,NULL);
	return TRUE; 
}

void CTowerStructDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UpdateDialogSize(cx,cy);
}

void CTowerStructDlg::UpdateDialogSize(int cx, int cy)
{
	if(m_listBodyOpening.GetSafeHwnd()==NULL)
		return;
	CRect rc;
	//初始化窗口控件大小,并移动对话框到左上角
	CSize screen,increExt;
	GetWindowRect(rc);
	if(cx==0&&cy==0)	//表示全屏显示
	{
		screen.cx=GetSystemMetrics(SM_CXMAXIMIZED)-10;
		screen.cy=GetSystemMetrics(SM_CYMAXIMIZED)-10;
	}
	else
	{
		screen.cx=cx;
		screen.cy=cy;
	}
	increExt.cx=screen.cx-rc.Width();
	increExt.cy=screen.cy-rc.Height();
	rc.left = 0;
	rc.top = 0;
	rc.right = screen.cx;
	rc.bottom = screen.cy;
	MoveWindow(rc,TRUE);
	//调整左侧主体开口列表框至最大
	m_listBodyOpening.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.right+=increExt.cx;
	rc.bottom+=increExt.cy;
	m_listBodyOpening.MoveWindow(&rc);
}

static CXhChar50 TwoNumberToStr(int num1,int num2)
{
	CXhChar50 sValue;
	if(num1==num2)
		sValue.Printf("%d",num1);
	else 
		sValue.Printf("%d/%d",num1,num2);
	return sValue;
}
static CXhChar50 TwoNumberToStr(double num1,double num2)
{
	CXhChar50 sValue;
	num1=fto_halfi(num1);
	num2=fto_halfi(num2);
	if(num1==num2)
		sValue.Printf("%g",num1);
	else 
		sValue.Printf("%g/%g",num1,num2);
	return sValue;
}

static bool ParseTwoNumberFormStr(const char* sValue,int &num1,int &num2)
{
	CXhChar50 ss(sValue);
	if(ss.Replace('/',' ')>0)
		sscanf(ss,"%d %d",&num1,&num2);
	else
		num1=num2=atoi(sValue);
	return true;
}

static bool ParseTwoNumberFormStr(const char* sValue,double &num1,double &num2)
{
	CXhChar50 ss(sValue);
	if(ss.Replace('/',' ')>0)
		sscanf(ss,"%lf %lf",&num1,&num2);
	else
		num1=num2=atof(sValue);
	return true;
}

CSuperGridCtrl::CTreeItem* CTowerStructDlg::InsertModelSegmentNode(CSuperGridCtrl *pListCtrl,CModelSegmentNode *pSeg,CSuperGridCtrl::CTreeItem *pParentItem)
{
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	lpInfo->SetImage(CTowerStructDlg::DATA_SEGMENT-1);	//分段
	lpInfo->SetSubItemText(COL_INDEX_HEIGHT,pSeg->fHeight>0?CXhChar16("%.f",pSeg->fHeight):"",TRUE);
	lpInfo->SetSubItemText(COL_INDEX_TZ,CXhChar16("%gm",pSeg->fTopZ*0.001),TRUE);
	lpInfo->SetSubItemText(COL_INDEX_SEGI,(char*)SEGI(pSeg->iSeg).ToString(),TRUE);
	CXhChar50 sSpec;
	pSeg->pPostRod->GetSpecification(sSpec,FALSE);
	lpInfo->SetSubItemText(COL_INDEX_RODSIZE,sSpec,TRUE);
	WORD wFrontAngleG=pSeg->wActualFrontAngleG,wSideAngleG=pSeg->wActualSideAngleG,wPreferAngleG=pSeg->wPreferAngleG;
	lpInfo->SetSubItemText(COL_INDEX_ACTUALG,TwoNumberToStr(wFrontAngleG,wSideAngleG),FALSE);
	if(!pSeg->syncflag.IsSpcecifyAngleG())
	{
		if(wFrontAngleG!=wPreferAngleG)
			lpInfo->SetSubItemColor(COL_INDEX_ACTUALG,RGB(255,222,106));
		else
			lpInfo->SetSubItemColor(COL_INDEX_ACTUALG,RGB(215,215,215));
	}
	else
		lpInfo->SetSubItemColor(COL_INDEX_ACTUALG,RGB(0,0,0));
	//推荐准据
	lpInfo->SetSubItemText(COL_INDEX_PREFERG,CXhChar16("%d",wPreferAngleG),TRUE);
	//上开口、下开口
	lpInfo->SetSubItemText(COL_INDEX_TOPSIZEXY,TwoNumberToStr(pSeg->fTopSizeX,pSeg->fTopSizeY),TRUE);
	lpInfo->SetSubItemText(COL_INDEX_BTMSIZEXY,TwoNumberToStr(pSeg->fBtmSizeX,pSeg->fBtmSizeY),TRUE);
	//塔脚基础根开
	lpInfo->SetSubItemText(COL_INDEX_BASE_SIZEXY,"",TRUE);
	CSuperGridCtrl::CTreeItem *pItem=pListCtrl->InsertItem(pParentItem,lpInfo);
	TREEITEM_INFO* pItemInfo=AddOrUpdateItemInfo(TREEITEM_INFO(DATA_SEGMENT,(DWORD)pSeg));
	pItem->m_idProp=(long)pItemInfo;
	return pItem;
}

CSuperGridCtrl::CTreeItem* CTowerStructDlg::InsertModelHeightNode(CSuperGridCtrl *pListCtrl,CModelHeightNode *pHeight,CSuperGridCtrl::CTreeItem *pParentItem)
{
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	lpInfo->SetImage(CTowerStructDlg::DATA_HEIGHT-1);	//呼高组
	CXhChar50 heightstr=pHeight->name;
	double zTransitHoriPlane=pHeight->pTreeModel!=NULL?pHeight->pTreeModel->m_fNamedHeightZeroZ:0;
	double zh=(pHeight->zTransitHoriPlane-zTransitHoriPlane)*0.001;
	heightstr.Append(CXhChar50("(=%gm+L)",zh));
	lpInfo->SetSubItemText(COL_INDEX_HEIGHT,heightstr,TRUE);
	CSuperGridCtrl::CTreeItem *pItem=pListCtrl->InsertItem(pParentItem,lpInfo);
	TREEITEM_INFO* pItemInfo=AddOrUpdateItemInfo(TREEITEM_INFO(DATA_HEIGHT,(DWORD)pHeight));
	pItem->m_idProp=(long)pItemInfo;
	return pItem;
}

CSuperGridCtrl::CTreeItem* CTowerStructDlg::InsertModelSubLegNode(CSuperGridCtrl *pListCtrl,CModelSubLegNode *pLeg,CSuperGridCtrl::CTreeItem *pParentItem)
{
	CListCtrlItemInfo* lpInfo = new CListCtrlItemInfo();
	lpInfo->SetImage(CTowerStructDlg::DATA_SUBLEG-1);	//塔腿
	lpInfo->SetSubItemText(COL_INDEX_HEIGHT,pLeg->fLegHeight>0?CXhChar16("%gm",pLeg->fLegHeight*0.001):"",TRUE);
	//lpInfo->SetSubItemText(COL_INDEX_TZ,"",TRUE);
	lpInfo->SetSubItemText(COL_INDEX_SEGI,(char*)SEGI(pLeg->iSeg).ToString(),TRUE);
	CXhChar50 sSpec;
	pLeg->pPostRod->GetSpecification(sSpec,FALSE);
	lpInfo->SetSubItemText(COL_INDEX_RODSIZE,sSpec,TRUE);
	WORD wFrontAngleG=pLeg->wActualFrontAngleG,wSideAngleG=pLeg->wActualSideAngleG,wPreferAngleG=pLeg->wPreferAngleG;
	lpInfo->SetSubItemText(COL_INDEX_ACTUALG,TwoNumberToStr(wFrontAngleG,wSideAngleG),FALSE);
	if(!pLeg->syncflag.IsSpcecifyAngleG())
	{
		if(wFrontAngleG!=wPreferAngleG)
			lpInfo->SetSubItemColor(COL_INDEX_ACTUALG,RGB(255,222,106));
		else
			lpInfo->SetSubItemColor(COL_INDEX_ACTUALG,RGB(195,195,195));
	}
	else
		lpInfo->SetSubItemColor(COL_INDEX_ACTUALG,RGB(0,0,0));
	//推荐准据
	lpInfo->SetSubItemText(COL_INDEX_PREFERG,CXhChar16("%d",wPreferAngleG),TRUE);
	//上开口
	lpInfo->SetSubItemText(COL_INDEX_TOPSIZEXY,TwoNumberToStr(pLeg->fTopSizeX,pLeg->fTopSizeY),TRUE);
	//铁塔半根开
	lpInfo->SetSubItemText(COL_INDEX_BTMSIZEXY,TwoNumberToStr(pLeg->fBtmSizeX,pLeg->fBtmSizeY),TRUE);
	//基础半根开
	lpInfo->SetSubItemText(COL_INDEX_BASE_SIZEXY,TwoNumberToStr(pLeg->fBaseSizeX,pLeg->fBaseSizeY),TRUE);
	CSuperGridCtrl::CTreeItem *pItem=pListCtrl->InsertItem(pParentItem,lpInfo);
	TREEITEM_INFO* pItemInfo=AddOrUpdateItemInfo(TREEITEM_INFO(DATA_SUBLEG,(DWORD)pLeg));
	pItem->m_idProp=(long)pItemInfo;
	return pItem;
}

bool CTowerStructDlg::UpdateListOpenCtrlByModelTree(void *pObj,CModelSlopeNode *pDownSlope,CSuperGridCtrl::CTreeItem *pParentItem,BYTE cObjType)
{
	CString sText;
	CListCtrlItemInfo* lpInfo=NULL;
	CSuperGridCtrl::CTreeItem *pItem=NULL;
	if(pObj==NULL&&pParentItem==NULL)
	{
		m_listBodyOpening.DeleteAllItems();
		CModelSlopeNode *pSlope=gxModelTree.listSlopes.EnumObjectFirst();
		CModelSlopeNode *pNextSlope=pSlope;
		for(pSlope=pNextSlope;pSlope;pSlope=pNextSlope)
		{
			pNextSlope=gxModelTree.listSlopes.EnumObjectNext();
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_SLOPE-1);	//坡段
			pItem=m_listBodyOpening.InsertRootItem(lpInfo);
			TREEITEM_INFO* pItemInfo=AddOrUpdateItemInfo(TREEITEM_INFO(DATA_SLOPE,(DWORD)pSlope));
			pItem->m_idProp=(long)pItemInfo;
			UpdateListOpenCtrlByModelTree(pSlope,pNextSlope,pItem,OBJ_TYPE_SLOPE);
		}
		m_listBodyOpening.Redraw();
	}
	else if(cObjType==OBJ_TYPE_SLOPE)
	{
		CModelSlopeNode *pSlope=(CModelSlopeNode*)pObj;
		for(CModelHeightNode *pHeight=pSlope->hashRootHeights.GetFirst();pHeight;pHeight=pSlope->hashRootHeights.GetNext())
		{
			pItem=InsertModelHeightNode(&m_listBodyOpening,pHeight,pParentItem);
			UpdateListOpenCtrlByModelTree(pHeight,pDownSlope,pItem,OBJ_TYPE_HEIGHT);
		}
		for(CModelSegmentNode *pSeg=pSlope->hashSegments.GetFirst();pSeg;pSeg=pSlope->hashSegments.GetNext())
		{
			pItem=InsertModelSegmentNode(&m_listBodyOpening,pSeg,pParentItem);
			UpdateListOpenCtrlByModelTree(pSeg,pDownSlope,pItem,OBJ_TYPE_SEGMENT);
		}
	}
	else if(cObjType==OBJ_TYPE_SEGMENT)
	{
		CModelSegmentNode *pSeg=(CModelSegmentNode*)pObj;
		for(CModelSegmentNode *pMutexSeg=pSeg->hashMutexSubSegments.GetFirst();pMutexSeg;pMutexSeg=pSeg->hashMutexSubSegments.GetNext())
		{
			pItem=InsertModelSegmentNode(&m_listBodyOpening,pMutexSeg,pParentItem);
			UpdateListOpenCtrlByModelTree(pMutexSeg,pDownSlope,pItem,OBJ_TYPE_SEGMENT);
		}
		for(CModelHeightNode *pHeight=pSeg->hashHeightSet.GetFirst();pHeight;pHeight=pSeg->hashHeightSet.GetNext())
		{
			if ( pDownSlope!=NULL&&pDownSlope->hashRootHeights.GetValue(pHeight->Id)!=NULL&&
				fabs(pSeg->fTopZ+pSeg->fHeight-pDownSlope->fTopZ)<1)
				continue;	//避免上坡段末尾段与下坡段顶部直接挂接的呼高重复 wjh-2020.2.1
			pItem=InsertModelHeightNode(&m_listBodyOpening,pHeight,pParentItem);
			UpdateListOpenCtrlByModelTree(pHeight,pDownSlope,pItem,OBJ_TYPE_HEIGHT);
		}
	}
	else if(cObjType==OBJ_TYPE_HEIGHT)
	{
		CModelHeightNode *pHeight=(CModelHeightNode*)pObj;
		for(CModelSegmentNode *pSeg=pHeight->hashSegments.GetFirst();pSeg;pSeg=pHeight->hashSegments.GetNext())
		{
			pItem=InsertModelSegmentNode(&m_listBodyOpening,pSeg,pParentItem);
			UpdateListOpenCtrlByModelTree(pSeg,pDownSlope,pItem,OBJ_TYPE_SEGMENT);
		}
		for(int i=0;i<CFGLEG::MaxLegs();i++)
		{
			if(pHeight->xarrSubLegs[i].Id<=0)
				continue;
			InsertModelSubLegNode(&m_listBodyOpening,&pHeight->xarrSubLegs[i],pParentItem);
		}
	}
	return true;
}

void CTowerStructDlg::RetrievedTowerModelTree()
{
	UpdateData();
	CLogErrorLife life(&logerr);
	CUnifyModelLayer intelli;
	TOLERANCE=m_fColinarTolerance;
	COLINEAR_TOLERANCE tolerance(m_fColinarTolerance,0.001);
	intelli.UnifyObjsLayer(tolerance);
	intelli.RetrievedTowerModeTree(gxModelTree);
}

void CTowerStructDlg::OnBnClickedBtnUpdateTowerModelTree()
{
	RetrievedTowerModelTree();
	UpdateListOpenCtrlByModelTree(NULL,NULL,NULL,NULL);
}
static void LiveDisplayActiveFoundation(CLDSFoundation* pFoundation)
{
	DISPLAY_TYPE disp_type;
	g_pSolidSet->GetDisplayType(&disp_type);
	if(disp_type!=DISP_SOLID)
		return;
	CLDSModule *pModule=Ta.GetActiveModule();
	if(pModule==NULL)
		return;
	BYTE ciCurrQuadActiveLegSerial=(pModule->m_arrActiveQuadLegNo[pFoundation->ciLegQuad-1]-1)%CFGLEG::MaxLegs()+1;
	if(pFoundation->ciHeightSerial!=pModule->idBodyNo||pFoundation->ciLegSerial!=ciCurrQuadActiveLegSerial)
		return;
	pFoundation->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pFoundation->GetSolidPartObject());
	for(WORD i=0;i<pFoundation->anchors.Count;i++)
	{
		CLDSAnchorBolt* pAnchorBolt=Ta.FromAnchorBolt(pFoundation->anchors[i].hAnchorBolt);
		if(pAnchorBolt==NULL)
			continue;
		pAnchorBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pAnchorBolt->GetSolidPartObject());
	}
}
void CTowerStructDlg::OnOK()
{
	CDialog::OnOK();
	//1.修改角钢准据
	for (CModelSegmentNode *pBodySeg=gxModelTree.hashSegments.GetFirst();pBodySeg;pBodySeg=gxModelTree.hashSegments.GetNext())
	{
		if (!pBodySeg->pPostRod->IsAngle())
			continue;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pBodySeg->pPostRod;
		if (pAngle->m_bEnableTeG&&(pAngle->xWingXZhunJu.g1==0||pAngle->xWingYZhunJu.g1==0))
		{
			JGZJ jgzj=CLDSLineAngle::GetAngleZJ(pAngle->size_wide);
			jgzj.g=pAngle->xWingXZhunJu.g;
			pAngle->xWingXZhunJu=jgzj;
			jgzj.g=pAngle->xWingYZhunJu.g;
			pAngle->xWingYZhunJu=jgzj;
			pAngle->SyncMirProp("m_bEnableTeG");
		}
	}
	//2.清除多重映射的地脚螺栓，否则会导致来回更新位置 wjh-2019.8.16
	CLDSFoundation * pFoundation;
	CHashList<long>hashAnchorBolts;
	for (pFoundation=Ta.Foundations.GetFirst();pFoundation;pFoundation=Ta.Foundations.GetNext())
	{
		for (WORD i=0;i<pFoundation->anchors.Count;i++)
		{
			if (hashAnchorBolts.GetValue(pFoundation->anchors[i].hAnchorBolt)!=NULL)
				pFoundation->anchors[i].hAnchorBolt=0;
			else
				hashAnchorBolts.SetValue(pFoundation->anchors[i].hAnchorBolt,pFoundation->handle);
		}
	}

		//3.生成铁塔基础
	for(CModelHeightNode *pHeight=gxModelTree.hashHeights.GetFirst();pHeight;pHeight=gxModelTree.hashHeights.GetNext())
	{
		for(int i=0;i<24;i++)
		{
			if(pHeight->xarrSubLegs[i].Id<=0)
				continue;
			if (pHeight->xarrSubLegs[i].pPostRod->IsAngle())
			{
				CLDSLineAngle* pAngle=(CLDSLineAngle*)pHeight->xarrSubLegs[i].pPostRod;
				if (pAngle->m_bEnableTeG&&(pAngle->xWingXZhunJu.g1==0||pAngle->xWingYZhunJu.g1==0))
				{
					JGZJ jgzj=CLDSLineAngle::GetAngleZJ(pAngle->size_wide);
					jgzj.g=pAngle->xWingXZhunJu.g;
					pAngle->xWingXZhunJu=jgzj;
					jgzj.g=pAngle->xWingYZhunJu.g;
					pAngle->xWingYZhunJu=jgzj;
					pAngle->SyncMirProp("m_bEnableTeG");
				}
			}
			for(int iLegQuad=1;iLegQuad<=4;iLegQuad++)
			{
				BYTE ciHeightSerial=pHeight->Id;
				BYTE ciLegSerial=pHeight->xarrSubLegs[i].Id;
				BYTE ciLegQuad=iLegQuad;
				//KEY4C k(ciHeightSerial,ciLegSerial,ciLegQuad);
				pFoundation=Ta.AddFoundation(ciHeightSerial,ciLegSerial,ciLegQuad);
				pHeight->xarrSubLegs[i].m_hRelaFoundation=pFoundation->handle;
				pHeight->xarrSubLegs[i].UpdateFoundationPosition();	//更新接腿的基础根开默认值
				pFoundation->location.Set(pHeight->xarrSubLegs[i].fHalfFrontBaseWidth,pHeight->xarrSubLegs[i].fHalfSideBaseWidth,pHeight->xarrSubLegs[i].zFoundationTopPlane);
				if(iLegQuad==2||iLegQuad==4)
					pFoundation->location.x*=-1.0;
				if(iLegQuad==3||iLegQuad==4)
					pFoundation->location.y*=-1.0;
				pFoundation->ciAnchorBoltD=(BYTE)Ta.m_xFoundation.wiBoltD;
				pFoundation->LayoutAnchorBolts(Ta.m_xFoundation.biLayoutType,Ta.m_xFoundation.wiBoltSapce);
				pFoundation->AddOrUpdateAnchorBolts(Ta.m_xFoundation.wiBasePlateThick,false);
				for (WORD j=0;j<pFoundation->anchors.Count;j++)
					hashAnchorBolts.SetValue(pFoundation->anchors[j].hAnchorBolt,pFoundation->handle);
				pFoundation->SetModified();
				pFoundation->wiWidth=Ta.m_xFoundation.wiFoundationWidth;
				LiveDisplayActiveFoundation(pFoundation);
			}
		}
	}
	for (CLDSPart* pAnchorPart=Ta.EnumPartFirst(CLS_ANCHORBOLT);pAnchorPart;pAnchorPart=Ta.EnumPartNext(CLS_ANCHORBOLT))
	{
		if (hashAnchorBolts.GetValue(pAnchorPart->handle)==NULL)
			Ta.Parts.DeleteCursor(FALSE);
	}
	g_pSolidDraw->Draw();
}
#include "ScrTools.h"
void CTowerStructDlg::OnBnClickedBtnAllocSegmentNumber()
{
	long liInitSegNumber=9;
	Input("输入塔身的起始段号:",&liInitSegNumber,'l');
	gxModelTree.AllocBodySegmentNumber(liInitSegNumber);
	CLDSNode* pNode;
	SmartPartPtr pSmartPart;
	double TOLERANCE=1.0;	//坡面上杆件与节点判断时的误差范围
	//更新全塔的塔身/腿对象的段号
	int i,count=m_listBodyOpening.GetItemCount();
	for(i=0;i<count;i++)
	{
		CSuperGridCtrl::CTreeItem* pItem=m_listBodyOpening.GetTreeItem(i);
		if(pItem->m_idProp==NULL)
			continue;
		TREEITEM_INFO* pItemInfo=(TREEITEM_INFO*)pItem->m_idProp;
		if(pItemInfo->itemType==CTowerStructDlg::DATA_SEGMENT)
		{
			CModelSegmentNode* pSegment=(CModelSegmentNode*)pItemInfo->dwRefData;
			SEGI segi(pSegment->iSeg);
			m_listBodyOpening.SetSubItemText(pItem,COL_INDEX_SEGI,segi.ToString());

			if (pSegment->pPostRod == NULL || pSegment->pPostRod->pStart == NULL || pSegment->pPostRod->pEnd == NULL)
				continue;
			TAPER_SCOPE taper;
			taper.VerifyVertex(pSegment->pPostRod->pStart->xOriginalPos);
			taper.VerifyVertex(pSegment->pPostRod->pEnd->xOriginalPos);
			taper.VerifyVertex(-pSegment->pPostRod->pStart->xOriginalPos.x,-pSegment->pPostRod->pStart->xOriginalPos.y,pSegment->pPostRod->pStart->xOriginalPos.z);
			taper.VerifyVertex(-pSegment->pPostRod->pEnd->xOriginalPos.x,-pSegment->pPostRod->pEnd->xOriginalPos.y,pSegment->pPostRod->pEnd->xOriginalPos.z);
			for (pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
			{
				if (!pSegment->pPostRod->ModuleTogetherWith(pNode))
					continue;
				if (taper.IsIncludePoint(pNode->xOriginalPos,TOLERANCE))
					pNode->iSeg = pSegment->iSeg;
			}
			for (pSmartPart = Ta.EnumPartFirst(); pSmartPart.blValid; pSmartPart = Ta.EnumPartNext())
			{
				if (!pSegment->pPostRod->ModuleTogetherWith(pSmartPart))
					continue;
				if (pSmartPart->IsLinePart() && pSmartPart.pRod->pStart != NULL && pSmartPart.pRod->pEnd != NULL)
				{
					if (taper.IsIncludePoint(pSmartPart.pRod->pStart->xOriginalPos,TOLERANCE) &&
						taper.IsIncludePoint(pSmartPart.pRod->pEnd->xOriginalPos,TOLERANCE))
						pSmartPart->iSeg = pSegment->iSeg;
				}
			}
		}
		if(pItemInfo->itemType==CTowerStructDlg::DATA_SUBLEG)
		{
			CModelSubLegNode* pSubLeg=(CModelSubLegNode*)pItemInfo->dwRefData;
			SEGI segi(pSubLeg->iSeg);
			m_listBodyOpening.SetSubItemText(pItem,COL_INDEX_SEGI,segi.ToString());

			CLDSLinePart* pPostRod = pSubLeg->m_arrPostRods[0];
			if (pPostRod == NULL || pPostRod->pStart == NULL || pPostRod->pEnd == NULL)
				continue;
			TAPER_SCOPE taper;
			taper.VerifyVertex(pPostRod->pStart->xOriginalPos);
			taper.VerifyVertex(pPostRod->pEnd->xOriginalPos);
			taper.VerifyVertex(-pPostRod->pStart->xOriginalPos.x,-pPostRod->pStart->xOriginalPos.y,pPostRod->pStart->xOriginalPos.z);
			taper.VerifyVertex(-pPostRod->pEnd->xOriginalPos.x,-pPostRod->pEnd->xOriginalPos.y,pPostRod->pEnd->xOriginalPos.z);
			for (pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
			{
				if (!pPostRod->ModuleTogetherWith(pNode))
					continue;
				if (taper.IsIncludePoint(pNode->xOriginalPos,TOLERANCE))
					pNode->iSeg = pSubLeg->iSeg;
			}
			for (pSmartPart = Ta.EnumPartFirst(); pSmartPart.blValid; pSmartPart = Ta.EnumPartNext())
			{
				if (!pPostRod->ModuleTogetherWith(pSmartPart))
					continue;
				if (pSmartPart->IsLinePart() && pSmartPart.pRod->pStart != NULL && pSmartPart.pRod->pEnd != NULL)
				{
					if (taper.IsIncludePoint(pSmartPart.pRod->pStart->xOriginalPos,TOLERANCE) &&
						taper.IsIncludePoint(pSmartPart.pRod->pEnd->xOriginalPos,TOLERANCE))
						pSmartPart->iSeg = pSubLeg->iSeg;
				}
			}
		}
	}
}
void CTowerStructDlg::OnBnClickedBtnExportExcel()
{
	//对呼高从小到大排序
	ARRAY_LIST<MODELHEIGHT_PTR> arrModelHeight;
	arrModelHeight.SetSize(gxModelTree.hashHeights.GetNodeNum());
	int i = 0, nSumLeg = 0;
	for (CModelHeightNode* pHeight = gxModelTree.hashHeights.GetFirst(); pHeight; pHeight = gxModelTree.hashHeights.GetNext())
	{
		nSumLeg += pHeight->wLegCount;
		arrModelHeight[i] = pHeight;
		i++;
	}
	CQuickSort<MODELHEIGHT_PTR>::QuickSort(arrModelHeight.m_pData, arrModelHeight.GetSize(), compare_func);
	//设计表格
	double fRowH = CXhTable::TransRowHightToCad(13.5);
	double fColW = CXhTable::TransColWidthToCad(10);
	double fText = CXhTable::TransRowHightToCad(10.5);
	int nRow = nSumLeg + 2, nCol = 13;
	CXhTable xTable;
	xTable.m_sTblName.Copy("根开表");
	xTable.Create(nRow, nCol, (float)fRowH, (float)fColW);
	xTable.SetFontSize((float)fText);	//设置文字高度
	//添加标题行
	int iRowIndex = 0;
	xTable.GetGridAt(iRowIndex, 0)->data.SetGridString("铁塔基础根开表");
	xTable.SetRowFontSize(iRowIndex, (float)CXhTable::TransRowHightToCad(20));
	xTable.SetRowHigh(iRowIndex, CXhTable::TransRowHightToCad(40));
	for (int iCol = 1; iCol < nCol; iCol++)
		xTable.GetGridAt(iRowIndex, iCol)->m_bHGridVirtual = TRUE;	//水平合并
	//添加标题列
	iRowIndex = 1;
	xTable.GetGridAt(iRowIndex, 0)->data.SetGridString("塔型");
	xTable.GetGridAt(iRowIndex, 1)->data.SetGridString("呼高(m)");
	xTable.GetGridAt(iRowIndex, 2)->data.SetGridString("接身高(m)");
	xTable.GetGridAt(iRowIndex, 3)->data.SetGridString("减腿(m)");
	xTable.GetGridAt(iRowIndex, 4)->data.SetGridString("接腿长(m)");
	xTable.GetGridAt(iRowIndex, 5)->data.SetGridString("接腿呼高(m)");
	xTable.GetGridAt(iRowIndex, 6)->data.SetGridString("正面半根开(mm)");
	xTable.GetGridAt(iRowIndex, 7)->data.SetGridString("对角半根开(mm)");
	xTable.GetGridAt(iRowIndex, 8)->data.SetGridString("基础半根开(mm)");
	xTable.GetGridAt(iRowIndex, 9)->data.SetGridString("基础半根开(mm)");
	xTable.GetGridAt(iRowIndex, 10)->data.SetGridString("地脚螺栓规格");
	xTable.GetGridAt(iRowIndex, 11)->data.SetGridString("地脚螺栓间距(mm)");
	xTable.GetGridAt(iRowIndex, 12)->data.SetGridString("单面坡度");
	xTable.SetRowHigh(iRowIndex, CXhTable::TransRowHightToCad(40));		//标题行
	//添加实时数据
	//塔型
	iRowIndex = 2;
	xTable.GetGridAt(iRowIndex, 0)->data.SetGridString(Ta.m_sTaType);
	for (int iRow = iRowIndex+1; iRow < nRow; iRow++)
		xTable.GetGridAt(iRow, 0)->m_bVGridVirtual = TRUE;
	//地脚螺栓规格
	int nLsNum = (Ta.m_xFoundation.biLayoutType == 0) ? 4 : 8;
	CXhChar16 sLsGuige("%dM%d", nLsNum, Ta.m_xFoundation.wiBoltD);
	xTable.GetGridAt(iRowIndex, 10)->data.SetGridString(sLsGuige);
	for (int iRow = iRowIndex + 1; iRow < nRow; iRow++)
		xTable.GetGridAt(iRow, 10)->m_bVGridVirtual = TRUE;
	//地脚螺栓间距
	double fLsSpace = Ta.m_xFoundation.wiBoltSapce * 2;
	xTable.GetGridAt(iRowIndex, 11)->data.SetGridString(CXhChar50("%g", fLsSpace));
	for (int iRow = iRowIndex + 1; iRow < nRow; iRow++)
		xTable.GetGridAt(iRow, 11)->m_bVGridVirtual = TRUE;
	//单面坡度
	CModelSlopeNode *pBtmSlope = gxModelTree.listSlopes.EnumObjectTail();
	if (pBtmSlope)
	{
		double fSlope = pBtmSlope->fSlopeRatio;
		xTable.GetGridAt(iRowIndex, 12)->data.SetGridString(CXhChar50("%g", fSlope));
		for (int iRow = iRowIndex + 1; iRow < nRow; iRow++)
			xTable.GetGridAt(iRow, 12)->m_bVGridVirtual = TRUE;
	}
	for (int i = 0; i < arrModelHeight.GetSize(); i++)
	{
		CModelHeightNode* pHeight = arrModelHeight[i];
		//呼高
		double fHuGaoH = pHeight->fNamedHeight*0.001;
		CXhChar16 sHuGao = pHeight->name;
		if (strchr(sHuGao, 'm'))
			sHuGao.Remove('m');
		xTable.GetGridAt(iRowIndex, 1)->data.SetGridString(sHuGao);
		for (int iRow = iRowIndex + 1; iRow < iRowIndex + pHeight->wLegCount; iRow++)
			xTable.GetGridAt(iRow, 1)->m_bVGridVirtual = TRUE;
		//接身高
		double fBodyH = pHeight->fLinkBodyHeight*0.001;
		xTable.GetGridAt(iRowIndex, 2)->data.SetGridString(CXhChar50("%g", fBodyH));
		for (int iRow = iRowIndex + 1; iRow < iRowIndex + pHeight->wLegCount; iRow++)
			xTable.GetGridAt(iRow, 2)->m_bVGridVirtual = TRUE;
		//
		for (int i = 0; i < CFGLEG::MaxLegs(); i++)
		{
			if (pHeight->xarrSubLegs[i].Id <= 0)
				continue;
			CModelSubLegNode *pLeg = &pHeight->xarrSubLegs[i];
			//减腿
			double fSubLegH = (pLeg->maxz - pHeight->zLongestLeg)*0.001;
			xTable.GetGridAt(iRowIndex, 3)->data.SetGridString(CXhChar50("%g", fSubLegH));
			//接腿长
			double fLegH = pLeg->fLegHeight*0.001;
			xTable.GetGridAt(iRowIndex, 4)->data.SetGridString(CXhChar50("%g", fLegH));
			//接腿呼高
			double fLegHuGaoH = fHuGaoH + fSubLegH;
			xTable.GetGridAt(iRowIndex, 5)->data.SetGridString(CXhChar50("%g", fLegHuGaoH));
			//正面半根开
			double fFrontHalfGK = pLeg->fBtmSizeX;
			xTable.GetGridAt(iRowIndex, 6)->data.SetGridString(TwoNumberToStr(fFrontHalfGK, fFrontHalfGK));
			//正面对角半根开
			double fFrontDiagonalHalfGK = fFrontHalfGK * 1.4142;
			xTable.GetGridAt(iRowIndex, 7)->data.SetGridString(TwoNumberToStr(fFrontDiagonalHalfGK, fFrontDiagonalHalfGK));
			//基础半根开
			double fBaseHalfGK = pLeg->fBaseSizeX;
			xTable.GetGridAt(iRowIndex, 8)->data.SetGridString(TwoNumberToStr(fBaseHalfGK, fBaseHalfGK));
			//基础对角半根开
			double fBaseDiagonalHalfGK = fBaseHalfGK * 1.4142;
			xTable.GetGridAt(iRowIndex, 9)->data.SetGridString(TwoNumberToStr(fBaseDiagonalHalfGK, fBaseDiagonalHalfGK));
			//
			iRowIndex++;
		}
	}
	CExcelOper::ExportExcle(&xTable);
}
#endif

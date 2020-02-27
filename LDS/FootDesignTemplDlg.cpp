// FootParaSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LDS.h"
#include "lds_co_part.h"
#include "FootDesignTemplDlg.h"
#include "coorddefdlg.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "AddDiJiaoLsDlg.h"
#include "DatumPointPropertyDlg.h"
#include "DesignVectorPropDlg.h"
#include "PropertyListOper.h"
#include "LocalFeatureDef.h"
#include "DataCompare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

ISolidDraw *g_pDraw;
ISolidSet *g_pSet;
ISolidSnap *g_pSnap;
ISolidOper *g_pOper;

//////////////////////////////////////////////////////////////////////////
//回调函数
//连接角钢列表
static BOOL ModifyLinkAngleValue(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	CFootDesignTemplDlg* pTempDlg=(CFootDesignTemplDlg*)pListCtrl->GetParent();
	if(pTempDlg==NULL)
		return FALSE;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pSelItem->m_idProp;
	if(pItemInfo==NULL)
		return FALSE;
	CXhChar100 tem_str;
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sTextValue);
	if(pItemInfo->itemType==CFootDesignTemplDlg::ITEM_TYPE_LINKANGLE)
	{
		CFootTempl::LINKANGLE* pLinkAngle=(CFootTempl::LINKANGLE*)pItemInfo->dwRefData;
		if(pLinkAngle==NULL)
			return FALSE;
		pListCtrl->SetItemText(pSelItem->GetIndex(),iSubItem,sTextValue.GetBuffer(1));
		if(iSubItem==0)	//位置标识
		{	
		}
		else if(iSubItem==1)	//角钢句柄
		{
			long handle=HexStrToLong(sTextValue);
			CLDSLineAngle* pAngle=(CLDSLineAngle*)Ta.FromPartHandle(handle,CLS_LINEANGLE);
			if(pAngle)
				pLinkAngle->pAngle=pAngle;
		}
		else if(iSubItem==2)	//螺栓直径
		{
			memmove(tem_str,tem_str+1,99);
			pLinkAngle->bolt.d=(BYTE)atol(tem_str);
		}
		else if(iSubItem==3)	//螺栓数量
		{
			pLinkAngle->bolt.num=(BYTE)atoi(tem_str);
			if(pLinkAngle->bolt.num>=5)
				pLinkAngle->bolt.rows=1;
			else if(pLinkAngle->bolt.num<=3)
				pLinkAngle->bolt.rows=0;
		}
		else if(iSubItem==4)	//螺栓排数
			pLinkAngle->bolt.rows=tem_str[0]-'0'+1;
		else if(iSubItem==5)	//螺栓布置方式
			pLinkAngle->bolt.layout=tem_str[0]-'0';
	}
	else if(pItemInfo->itemType==CFootDesignTemplDlg::ITEM_TYPE_RIBPLATE)
	{
		CFootTempl::RIBPLATE* pRibPlate=pTempDlg->foottempl.listRibPlates.GetValue(pItemInfo->dwRefData);
		if(pRibPlate==NULL)
			return FALSE;
		CXhChar50 sValue;
		pListCtrl->SetItemText(pSelItem->GetIndex(),iSubItem,sTextValue.GetBuffer(1));
		if(iSubItem==1)			//材质
		{
			pRibPlate->cMaterial=QuerySteelBriefMatMark(tem_str);
			//更新列表相应内容
			int nCount=pListCtrl->GetRootCount();
			for(int i=0;i<nCount;i++)
			{
				CSuperGridCtrl::CTreeItem* pItem=pListCtrl->GetTreeItem(i);
				if(pItem==NULL)
					continue;
				TREEITEM_INFO *pOtherItemInfo=(TREEITEM_INFO*)pItem->m_idProp;
				if(pOtherItemInfo==NULL||pOtherItemInfo->itemType!=CFootDesignTemplDlg::ITEM_TYPE_RIBPLATE)
					continue;
				CFootTempl::RIBPLATE* pPlate=(CFootTempl::RIBPLATE*)pTempDlg->foottempl.listRibPlates.GetValue(pOtherItemInfo->dwRefData);
				if(pPlate==NULL||pPlate==pRibPlate)
					continue;
				pPlate->cMaterial=pRibPlate->cMaterial;
				STEELMAT *pSteelMat=QuerySteelMatByBriefMark(pPlate->cMaterial);
				sValue.Copy(pSteelMat->mark);
				pListCtrl->SetItemText(i,iSubItem,sValue);
			}
		}
		else if(iSubItem==2)	//编号
			sprintf(pRibPlate->sPartNo,"%s",&tem_str);
		else if(iSubItem==3)	//厚度
		{
			pRibPlate->thick=atoi(tem_str);
			//更新列表相应内容
			int nCount=pListCtrl->GetRootCount();
			for(int i=0;i<nCount;i++)
			{
				CSuperGridCtrl::CTreeItem* pItem=pListCtrl->GetTreeItem(i);
				if(pItem==NULL)
					continue;
				TREEITEM_INFO *pOtherItemInfo=(TREEITEM_INFO*)pItem->m_idProp;
				if(pOtherItemInfo==NULL||pOtherItemInfo->itemType!=CFootDesignTemplDlg::ITEM_TYPE_RIBPLATE)
					continue;
				CFootTempl::RIBPLATE* pPlate=(CFootTempl::RIBPLATE*)pTempDlg->foottempl.listRibPlates.GetValue(pItemInfo->dwRefData);
				if(pPlate==NULL||pPlate==pRibPlate)
					continue;
				pPlate->thick=pRibPlate->thick;
				sValue.Printf("%d",pPlate->thick);
				pListCtrl->SetItemText(i,iSubItem,sValue);
			}
		}
		pTempDlg->UpdateRibPlateProperty(NULL,pRibPlate);
	}
	return TRUE;
}
static BOOL DeleteLinkAngleItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem)
{
	CFootDesignTemplDlg* pTempDlg=(CFootDesignTemplDlg*)pListCtrl->GetParent();
	if(pTempDlg==NULL)
		return FALSE;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pSelItem->m_idProp;
	if(pItemInfo==NULL)
		return FALSE;
	if(pItemInfo->itemType==CFootDesignTemplDlg::ITEM_TYPE_RIBPLATE)
	{
		CFootTempl::RIBPLATE* pSelRibPlate=pTempDlg->foottempl.listRibPlates.GetValue(pItemInfo->dwRefData);
		if(pSelRibPlate==NULL)
			return FALSE;
		pTempDlg->foottempl.listRibPlates.DeleteNode(pItemInfo->dwRefData);
		pTempDlg->DeleteItemInfo(pItemInfo);
		//更新属性栏
		pTempDlg->m_idActiveRibPlate=0;
		pTempDlg->UpdateRibPlateProperty(NULL);
	}
	else if(pItemInfo->itemType==CFootDesignTemplDlg::ITEM_TYPE_LINKANGLE)
	{
		CFootTempl::LINKANGLE* pSelLinkAngle=(CFootTempl::LINKANGLE*)pItemInfo->dwRefData;
		if(pSelLinkAngle==NULL)
			return FALSE;
		if(pTempDlg->m_iDispType==CFootDesignTemplDlg::DISP_BASE_PLATE)
		{
			pTempDlg->foottempl.base.linkAngles.DeleteNode(pSelLinkAngle->label);
			pTempDlg->DeleteItemInfo(pItemInfo);
		}
		else if(pTempDlg->m_iDispType==CFootDesignTemplDlg::DISP_SHOE_PLATE)
		{	//获取杆件所属靴板
			CFootTempl::SHOEPLATE* pShoePlate=(CFootTempl::SHOEPLATE*)pSelLinkAngle->pLinkPlate;
			if(pShoePlate)
			{
				pShoePlate->linkAngles.DeleteNode(pSelLinkAngle->label);
				pTempDlg->DeleteItemInfo(pItemInfo);
			}
		}
	}
	g_pDraw->BuildDisplayList(pTempDlg);
	return TRUE;
}
static BOOL LinkAngleContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CFootDesignTemplDlg *pTempDlg=(CFootDesignTemplDlg*)pListCtrl->GetParent();
	if(pTempDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
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
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_ADD_LINKANGLE,"Add connection angles");
#else
		pMenu->AppendMenu(MF_STRING,ID_ADD_LINKANGLE,"添加连接杆件");
#endif
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pTempDlg);
	return TRUE;
}
static BOOL FireLinkAnglesItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,NM_LISTVIEW* pNMListView)
{
	CFootDesignTemplDlg *pTempDlg=(CFootDesignTemplDlg*)pListCtrl->GetParent();
	if(pTempDlg==NULL || pSelItem==NULL)
		return FALSE;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pSelItem->m_idProp;
	if(pItemInfo==NULL||pItemInfo->itemType!=CFootDesignTemplDlg::ITEM_TYPE_RIBPLATE)
		return FALSE;
	CFootTempl::RIBPLATE* pRibPlate=(CFootTempl::RIBPLATE*)pTempDlg->foottempl.listRibPlates.GetValue(pItemInfo->dwRefData);
	if(pRibPlate)
	{
		pTempDlg->UpdateRibPlateProperty(NULL,pRibPlate);
		g_pDraw->BuildDisplayList(pTempDlg);
	}
	return TRUE;
}
//靴板列表
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,int iSubItem,CString& sTextValue)
{
	CFootDesignTemplDlg* pTempDlg=(CFootDesignTemplDlg*)pListCtrl->GetParent();
	if(pTempDlg==NULL)
		return FALSE;
	CXhChar100 tem_str;
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",sTextValue);
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pSelItem->m_idProp;
	if(pItemInfo==NULL)
		return FALSE;
	if(pItemInfo->itemType==CFootDesignTemplDlg::ITEM_TYPE_HOLE)
	{
		LIST_NODE<CFootTempl::BASEPLATE::HOLE> *pHole=(LIST_NODE<CFootTempl::BASEPLATE::HOLE>*)pItemInfo->dwRefData;
		if(pHole==NULL)
			return FALSE;
		pListCtrl->SetItemText(pSelItem->GetIndex(),iSubItem,sTextValue.GetBuffer(1));
		if(iSubItem==0)		//规格
		{
			memmove(tem_str,tem_str+1,99);
			pHole->data.d=atoi(tem_str);
		}
		else if(iSubItem==1)//孔径
			pHole->data.hole_d=atoi(tem_str);
		else if(iSubItem==2)//X
			pHole->data.x=atoi(tem_str);
		else if(iSubItem==3)//Y
			pHole->data.y=atoi(tem_str);
		if(iSubItem==0||iSubItem==1)
		{	//底板螺栓孔规格及孔径修改时，所有螺栓应同时修改
			int nCount=pListCtrl->GetRootCount();
			for(int i=0;i<nCount;i++)
			{
				CSuperGridCtrl::CTreeItem* pItem=pListCtrl->GetTreeItem(i);
				if(pItem==NULL||pItem==pSelItem)
					continue;
				TREEITEM_INFO *pOtherItemInfo=(TREEITEM_INFO*)pItem->m_idProp;
				if(pOtherItemInfo==NULL||pOtherItemInfo->itemType!=CFootDesignTemplDlg::ITEM_TYPE_HOLE)
					continue;
				LIST_NODE<CFootTempl::BASEPLATE::HOLE>* pOtherHole=(LIST_NODE<CFootTempl::BASEPLATE::HOLE>*)pItemInfo->dwRefData;
				if(iSubItem==0)
				{
					pOtherHole->data.d=pHole->data.d;
					tem_str.Printf("M%d",pHole->data.d);
				}
				else 
				{
					pOtherHole->data.hole_d=pHole->data.hole_d;
					tem_str.Printf("%d",pOtherHole->data.hole_d);
				}
				pListCtrl->SetItemText(i,iSubItem,tem_str);
			}
		}
	}
	else if(pItemInfo->itemType==CFootDesignTemplDlg::ITEM_TYPE_SHOEPLATE)
	{
		CFootTempl::SHOEPLATE* pShoePlate=(CFootTempl::SHOEPLATE*)pItemInfo->dwRefData;
		if(pShoePlate==NULL)
			return FALSE;
		CXhChar50 sValue;
		pListCtrl->SetItemText(pSelItem->GetIndex(),iSubItem,sTextValue.GetBuffer(1));
		if(iSubItem==1)			//材质
		{
			pShoePlate->cMaterial=QuerySteelBriefMatMark(tem_str);
			pTempDlg->UpdateShoePlateInfo(pShoePlate);
			//更新列表相应内容
			int nCount=pListCtrl->GetRootCount();
			for(int i=0;i<nCount;i++)
			{
				CSuperGridCtrl::CTreeItem* pItem=pListCtrl->GetTreeItem(i);
				if(pItem==NULL)
					return FALSE;
				TREEITEM_INFO *pOtherItemInfo=(TREEITEM_INFO*)pItem->m_idProp;
				if(pOtherItemInfo==NULL||pOtherItemInfo->itemType!=CFootDesignTemplDlg::ITEM_TYPE_SHOEPLATE)
					continue;
				CFootTempl::SHOEPLATE* pPlate=(CFootTempl::SHOEPLATE*)pOtherItemInfo->dwRefData;
				if(pPlate->label.CharAt(0)!=pShoePlate->label.CharAt(0))
					continue;
				STEELMAT *pSteelMat=QuerySteelMatByBriefMark(pPlate->cMaterial);
				sValue.Copy(pSteelMat->mark);
				pListCtrl->SetItemText(i,iSubItem,sValue);
			}
		}
		else if(iSubItem==2)	//编号
		{
			sprintf(pShoePlate->sPartNo,"%s",&tem_str);
#if defined(__TMA_)||defined(__LMA_)
			BOMPART* pPart=NULL;
			if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
				VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
				pPart=globalDataCmpModel.GetDrawingBomPart(pShoePlate->sPartNo,BOMPART::PLATE);
			if(pPart)
			{
				pShoePlate->thick=ftoi(pPart->thick);
				pShoePlate->cMaterial=pPart->cMaterial;
				//
				QuerySteelMatMark(pPart->cMaterial,sValue);
				pListCtrl->SetItemText(pSelItem->GetIndex(),1,sValue);
				pListCtrl->SetItemText(pSelItem->GetIndex(),3,CXhChar50("%.0f",pPart->thick));
			}
#endif
		}
		else if(iSubItem==3)	//厚度
		{
			pShoePlate->thick=atoi(tem_str);
			pTempDlg->UpdateShoePlateInfo(pShoePlate);
			//更新列表相应内容
			int nCount=pListCtrl->GetRootCount();
			for(int i=0;i<nCount;i++)
			{
				CSuperGridCtrl::CTreeItem* pItem=pListCtrl->GetTreeItem(i);
				if(pItem==NULL)
					return FALSE;
				TREEITEM_INFO *pOtherItemInfo=(TREEITEM_INFO*)pItem->m_idProp;
				if(pOtherItemInfo==NULL||pOtherItemInfo->itemType!=CFootDesignTemplDlg::ITEM_TYPE_SHOEPLATE)
					continue;
				CFootTempl::SHOEPLATE* pPlate=(CFootTempl::SHOEPLATE*)pOtherItemInfo->dwRefData;
				if(pPlate->label.CharAt(0)!=pShoePlate->label.CharAt(0))
					continue;
				sValue.Printf("%d",pPlate->thick);
				pListCtrl->SetItemText(i,iSubItem,sValue);
			}
		}
		else if(iSubItem==4)
			pShoePlate->fCutAngleWidth=atof(tem_str);
		else if(iSubItem==5)
			pShoePlate->norm_offset=atoi(tem_str);
	}
	g_pDraw->BuildDisplayList(pTempDlg);
	return TRUE;
}
static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem)
{
	CFootDesignTemplDlg* pTempDlg=(CFootDesignTemplDlg*)pListCtrl->GetParent();
	if(pTempDlg==NULL)
		return FALSE;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pSelItem->m_idProp;
	if(pItemInfo==NULL)
		return FALSE;
	if(pItemInfo->itemType==CFootDesignTemplDlg::ITEM_TYPE_HOLE)
	{
		LIST_NODE<CFootTempl::BASEPLATE::HOLE> *pHole=(LIST_NODE<CFootTempl::BASEPLATE::HOLE>*)pItemInfo->dwRefData;
		if(pHole==NULL)
			return FALSE;
		for(LIST_NODE<CFootTempl::BASEPLATE::HOLE>* pListNode=pTempDlg->foottempl.base.listHoles.EnumFirst();
			pListNode;pListNode=pTempDlg->foottempl.base.listHoles.EnumNext())
		{
			if(pListNode==pHole)
			{
				pTempDlg->DeleteItemInfo(pItemInfo);
				pTempDlg->foottempl.base.listHoles.DeleteCursor();
				g_pDraw->BuildDisplayList(pTempDlg);
				return TRUE;
			}
		}
	}
	else if(pItemInfo->itemType==CFootDesignTemplDlg::ITEM_TYPE_SHOEPLATE)
	{
		CFootTempl::SHOEPLATE* pShoePlate=(CFootTempl::SHOEPLATE*)pItemInfo->dwRefData;
		if(pShoePlate==NULL)
			return FALSE;
		pTempDlg->DeleteItemInfo(pItemInfo);
		pTempDlg->foottempl.listShoePlates.DeleteNode(pShoePlate->label);
	}
	return TRUE;
}
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CFootDesignTemplDlg *pTempDlg=(CFootDesignTemplDlg*)pListCtrl->GetParent();
	if(pTempDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	BOOL bInSelItemRect=FALSE;
	if(pSelItem)
	{
		CRect rectItem;
		pListCtrl->GetItemRect(pSelItem->GetIndex(),rectItem,LVIR_BOUNDS);
		if(rectItem.PtInRect(point))
			bInSelItemRect=TRUE;	//鼠标不在当前选中项上
	}
	if(pTempDlg->m_iDispType==CFootDesignTemplDlg::DISP_BASE_PLATE||pTempDlg->m_iDispType==CFootDesignTemplDlg::DISP_RIB_PLATE)
	{
		if(!bInSelItemRect)
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_ADD_HOLE,"Add Base Holes");
#else
			pMenu->AppendMenu(MF_STRING,ID_ADD_HOLE,"添加底孔");
#endif
	}
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pTempDlg);
	return TRUE;
}
static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,NM_LISTVIEW* pNMListView)
{
	CFootDesignTemplDlg *pTempDlg=(CFootDesignTemplDlg*)pListCtrl->GetParent();
	if(pTempDlg==NULL || pSelItem==NULL)
		return FALSE;
	TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pSelItem->m_idProp;
	if(pItemInfo==NULL||pItemInfo->itemType!=CFootDesignTemplDlg::ITEM_TYPE_SHOEPLATE)
		return FALSE;
	CFootTempl::SHOEPLATE* pShoePlate=(CFootTempl::SHOEPLATE*)pItemInfo->dwRefData;
	if(pShoePlate)
		pTempDlg->RefreshLinkAngleListCtrl(pShoePlate);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//绘制图形的回调函数
struct XUEBANLINE{
	f3dLine upLineB,btmLineB;
	f3dLine upLineF,btmLineF;
};
static void DrawGroupJgWeldShoeGraph(f2dLine* cutLineArr,f2dLine* sonLineArr,int shoePlateX0_Y1,CFootTempl& foottempl,
								BOOL bUpShoePlate,XUEBANLINE& shoeLines)
{
	f3dPoint vertexF0,vertexF1,vertexB0,vertexB1;
	f3dPoint vertexFM0,vertexFM1,vertexBM0,vertexBM1;
	f2dLine left_line,rignt_line,up_line,down_line;
	if(foottempl.base.shape==0)	//方形底座
	{
		if(shoePlateX0_Y1==0)	//X轴向靴板
		{
			left_line.startPt.Set(-foottempl.base.XB,-foottempl.base.YB);
			left_line.endPt.Set(-foottempl.base.XB,foottempl.base.YA);
			rignt_line.startPt.Set(foottempl.base.XA,-foottempl.base.YB);
			rignt_line.endPt.Set(foottempl.base.XA,foottempl.base.YA);
			if(cutLineArr[0].endPt.x>0)
			{
				Int2dll(cutLineArr[0],rignt_line,vertexF0.x,vertexF0.y);
				vertexFM0.Set(cutLineArr[0].endPt.x,cutLineArr[0].endPt.y);
				Int2dll(cutLineArr[1],rignt_line,vertexF1.x,vertexF1.y);
				vertexFM1.Set(cutLineArr[1].endPt.x,cutLineArr[1].endPt.y);
				Int2dll(sonLineArr[0],left_line,vertexB0.x,vertexB0.y);
				vertexBM0.Set(sonLineArr[0].endPt.x,sonLineArr[0].endPt.y);
				Int2dll(sonLineArr[1],left_line,vertexB1.x,vertexB1.y);
				vertexBM1.Set(sonLineArr[1].endPt.x,sonLineArr[1].endPt.y);
			}
			else
			{
				Int2dll(sonLineArr[0],rignt_line,vertexF0.x,vertexF0.y);
				vertexFM0.Set(sonLineArr[0].endPt.x,sonLineArr[0].endPt.y);
				Int2dll(sonLineArr[1],rignt_line,vertexF1.x,vertexF1.y);
				vertexFM1.Set(sonLineArr[1].endPt.x,sonLineArr[1].endPt.y);
				Int2dll(cutLineArr[0],left_line,vertexB0.x,vertexB0.y);
				vertexBM0.Set(cutLineArr[0].endPt.x,cutLineArr[0].endPt.y);
				Int2dll(cutLineArr[1],left_line,vertexB1.x,vertexB1.y);
				vertexBM1.Set(cutLineArr[1].endPt.x,cutLineArr[1].endPt.y);
			}
		}
		else	//Y轴向靴板
		{
			up_line.startPt.Set(-foottempl.base.XB,foottempl.base.YA);
			up_line.endPt.Set(foottempl.base.XA,foottempl.base.YA);
			down_line.startPt.Set(-foottempl.base.XB,-foottempl.base.YB);
			down_line.endPt.Set(foottempl.base.XA,-foottempl.base.YB);
			if(cutLineArr[0].endPt.y>0)
			{
				Int2dll(cutLineArr[0],up_line,vertexB0.x,vertexB0.y);
				vertexBM0.Set(cutLineArr[0].endPt.x,cutLineArr[0].endPt.y);
				Int2dll(cutLineArr[1],up_line,vertexB1.x,vertexB1.y);
				vertexBM1.Set(cutLineArr[1].endPt.x,cutLineArr[1].endPt.y);
				Int2dll(sonLineArr[0],down_line,vertexF0.x,vertexF0.y);
				vertexFM0.Set(sonLineArr[0].endPt.x,sonLineArr[0].endPt.y);
				Int2dll(sonLineArr[1],down_line,vertexF1.x,vertexF1.y);
				vertexFM1.Set(sonLineArr[1].endPt.x,sonLineArr[1].endPt.y);
			}
			else
			{
				Int2dll(sonLineArr[0],up_line,vertexB0.x,vertexB0.y);
				vertexBM0.Set(sonLineArr[0].endPt.x,sonLineArr[0].endPt.y);
				Int2dll(sonLineArr[1],up_line,vertexB1.x,vertexB1.y);
				vertexBM1.Set(sonLineArr[1].endPt.x,sonLineArr[1].endPt.y);
				Int2dll(cutLineArr[0],down_line,vertexF0.x,vertexF0.y);
				vertexFM0.Set(cutLineArr[0].endPt.x,cutLineArr[0].endPt.y);
				Int2dll(cutLineArr[1],down_line,vertexF1.x,vertexF1.y);
				vertexFM1.Set(cutLineArr[1].endPt.x,cutLineArr[1].endPt.y);
			}
		}
	}
	else		//圆形底座
	{
		f2dCircle circle;
		circle.radius=foottempl.base.fR;
		f3dPoint temPt1,temPt2;
		Int2dlc(cutLineArr[0],circle,temPt1.x,temPt1.y,temPt2.x,temPt2.y);
		if(shoePlateX0_Y1==0&&cutLineArr[0].endPt.x>0)
		{
			vertexF0=(temPt1.x>0)?temPt1:temPt2;
			vertexFM0.Set(cutLineArr[0].endPt.x,cutLineArr[0].endPt.y);
		}
		else if(shoePlateX0_Y1==0&&cutLineArr[0].endPt.x<0)
		{
			vertexB0=(temPt1.x<0)?temPt1:temPt2;
			vertexBM0.Set(cutLineArr[0].endPt.x,cutLineArr[0].endPt.y);
		}
		else if(shoePlateX0_Y1==1&&cutLineArr[0].endPt.y>0)
		{
			vertexB0=(temPt1.y>0)?temPt1:temPt2;
			vertexBM0.Set(cutLineArr[0].endPt.x,cutLineArr[0].endPt.y);
		}
		else if(shoePlateX0_Y1==1&&cutLineArr[0].endPt.y<0)
		{
			vertexF0=(temPt1.y<0)?temPt1:temPt2;
			vertexFM0.Set(cutLineArr[0].endPt.x,cutLineArr[0].endPt.y);
		}
		//
		Int2dlc(cutLineArr[1],circle,temPt1.x,temPt1.y,temPt2.x,temPt2.y);
		if(shoePlateX0_Y1==0&&cutLineArr[1].endPt.x>0)
		{
			vertexF1=(temPt1.x>0)?temPt1:temPt2;
			vertexFM1.Set(cutLineArr[1].endPt.x,cutLineArr[1].endPt.y);
		}
		else if(shoePlateX0_Y1==0&&cutLineArr[1].endPt.x<0)
		{	
			vertexB1=(temPt1.x<0)?temPt1:temPt2;
			vertexBM1.Set(cutLineArr[1].endPt.x,cutLineArr[1].endPt.y);
		}
		else if(shoePlateX0_Y1==1&&cutLineArr[1].endPt.y>0)
		{	
			vertexB1=(temPt1.y>0)?temPt1:temPt2;
			vertexBM1.Set(cutLineArr[1].endPt.x,cutLineArr[1].endPt.y);
		}
		else if(shoePlateX0_Y1==1&&cutLineArr[1].endPt.y<0)
		{	
			vertexF1=(temPt1.y<0)?temPt1:temPt2;
			vertexFM1.Set(cutLineArr[1].endPt.x,cutLineArr[1].endPt.y);
		}
		//
		Int2dlc(sonLineArr[0],circle,temPt1.x,temPt1.y,temPt2.x,temPt2.y);
		if(shoePlateX0_Y1==0&&sonLineArr[0].endPt.x>0)
		{	
			vertexF0=(temPt1.x>0)?temPt1:temPt2;
			vertexFM0.Set(sonLineArr[0].endPt.x,sonLineArr[0].endPt.y);
		}
		else if(shoePlateX0_Y1==0&&sonLineArr[0].endPt.x<0)
		{	
			vertexB0=(temPt1.x<0)?temPt1:temPt2;
			vertexBM0.Set(sonLineArr[0].endPt.x,sonLineArr[0].endPt.y);
		}
		else if(shoePlateX0_Y1==1&&sonLineArr[0].endPt.y>0)
		{	
			vertexB0=(temPt1.y>0)?temPt1:temPt2;
			vertexBM0.Set(sonLineArr[0].endPt.x,sonLineArr[0].endPt.y);
		}
		else if(shoePlateX0_Y1==1&&sonLineArr[0].endPt.y<0)
		{	
			vertexF0=(temPt1.y<0)?temPt1:temPt2;
			vertexFM0.Set(sonLineArr[0].endPt.x,sonLineArr[0].endPt.y);
		}
		//
		Int2dlc(sonLineArr[1],circle,temPt1.x,temPt1.y,temPt2.x,temPt2.y);
		if(shoePlateX0_Y1==0&&sonLineArr[1].endPt.x>0)
		{	
			vertexF1=(temPt1.x>0)?temPt1:temPt2;
			vertexFM1.Set(sonLineArr[1].endPt.x,sonLineArr[1].endPt.y);
		}
		else if(shoePlateX0_Y1==0&&sonLineArr[1].endPt.x<0)
		{	
			vertexB1=(temPt1.x<0)?temPt1:temPt2;
			vertexBM1.Set(sonLineArr[1].endPt.x,sonLineArr[1].endPt.y);
		}
		else if(shoePlateX0_Y1==1&&sonLineArr[1].endPt.y>0)
		{	
			vertexB1=(temPt1.y>0)?temPt1:temPt2;
			vertexBM1.Set(sonLineArr[1].endPt.x,sonLineArr[1].endPt.y);
		}
		else if(shoePlateX0_Y1==1&&sonLineArr[1].endPt.y<0)
		{	
			vertexF1=(temPt1.y<0)?temPt1:temPt2;
			vertexFM1.Set(sonLineArr[1].endPt.x,sonLineArr[1].endPt.y);
		}
	}
	//绘制靴板
	f3dLine line;
	line.pen.width=1;
	line.pen.style=PS_SOLID;
	line.pen.crColor=RGB(0,0,200);
	if(!bUpShoePlate)
	{	
		line.pen.style=PS_DASH;
		line.pen.crColor=RGB(128,0,128);
	}
	f3dPoint vertexArr[4];
	shoeLines.upLineB.startPt=vertexArr[0]=vertexB1;
	shoeLines.upLineB.endPt=vertexArr[1]=vertexBM1;
	shoeLines.btmLineB.startPt=vertexArr[3]=vertexB0;
	shoeLines.btmLineB.endPt=vertexArr[2]=vertexBM0;
	for(int i=0;i<4;i++)
	{
		line.startPt = vertexArr[i];
		line.endPt   = vertexArr[(i+1)%4];
		g_pDraw->NewLine(line);
	}
	//
	shoeLines.upLineF.startPt=vertexArr[0]=vertexFM1;
	shoeLines.upLineF.endPt=vertexArr[1]=vertexF1;
	shoeLines.btmLineF.startPt=vertexArr[3]=vertexFM0;
	shoeLines.btmLineF.endPt=vertexArr[2]=vertexF0;
	for(int i=0;i<4;i++)
	{
		line.startPt = vertexArr[i];
		line.endPt   = vertexArr[(i+1)%4];
		g_pDraw->NewLine(line);
	}
}
static void DrawWeldShoeGraph(f2dLine* cutLineArr,int shoePlateX0_Y1,CFootTempl& foottempl,f3dPoint norm,
								BOOL bUpShoePlate,XUEBANLINE& shoeLines)
{
	f3dPoint vertexF0,vertexF1,vertexB0,vertexB1,min_vertex0,min_vertex1;
	f2dLine edge_line;
	if(foottempl.base.shape==0)	//方形底座
	{
		if(shoePlateX0_Y1==0)	//X轴向靴板
		{
			edge_line.startPt.Set(foottempl.base.XA,-foottempl.base.YB);
			edge_line.endPt.Set(foottempl.base.XA,foottempl.base.YA);
			Int2dll(cutLineArr[0],edge_line,vertexF0.x,vertexF0.y);
			Int2dll(cutLineArr[1],edge_line,vertexF1.x,vertexF1.y);
			edge_line.startPt.Set(-foottempl.base.XB,-foottempl.base.YB);
			edge_line.endPt.Set(-foottempl.base.XB,foottempl.base.YA);
			Int2dll(cutLineArr[0],edge_line,vertexB0.x,vertexB0.y);
			Int2dll(cutLineArr[1],edge_line,vertexB1.x,vertexB1.y);
		}
		else	//Y轴向靴板
		{
			edge_line.startPt.Set(-foottempl.base.XB,-foottempl.base.YB);
			edge_line.endPt.Set(foottempl.base.XA,-foottempl.base.YB);
			Int2dll(cutLineArr[0],edge_line,vertexF0.x,vertexF0.y);
			Int2dll(cutLineArr[1],edge_line,vertexF1.x,vertexF1.y);
			edge_line.startPt.Set(-foottempl.base.XB,foottempl.base.YA);
			edge_line.endPt.Set(foottempl.base.XA,foottempl.base.YA);
			Int2dll(cutLineArr[0],edge_line,vertexB0.x,vertexB0.y);
			Int2dll(cutLineArr[1],edge_line,vertexB1.x,vertexB1.y);
		}
	}
	else		//圆形底座
	{
		f2dCircle circle;
		circle.radius=foottempl.base.fR;
		Int2dlc(cutLineArr[0],circle,vertexB0.x,vertexB0.y,vertexF0.x,vertexF0.y);
		Int2dlc(cutLineArr[1],circle,vertexB1.x,vertexB1.y,vertexF1.x,vertexF1.y);
	}
	//
	edge_line.startPt.Set(vertexB0.x,vertexB0.y);
	edge_line.endPt.Set(vertexF0.x,vertexF0.y);
	Int2dll(cutLineArr[2],edge_line,min_vertex0.x,min_vertex0.y);
	edge_line.startPt.Set(vertexB1.x,vertexB1.y);
	edge_line.endPt.Set(vertexF1.x,vertexF1.y);
	Int2dll(cutLineArr[2],edge_line,min_vertex1.x,min_vertex1.y);
	//
	UCS_STRU ucs;
	foottempl.GetUCS(ucs,false);
	ucs.origin=foottempl.origin.Position();
	vector_trans(norm,ucs,FALSE);
	double angle = cal_angle_of_2vec(norm,f3dPoint(norm.x,norm.y,0));
	norm = norm.normalized()*fabs(1.0/cos(angle));
	int norm_offset=0;
	CFootTempl::SHOEPLATE* pShoePlate=NULL;
	if(bUpShoePlate)
	{
		if(shoePlateX0_Y1==0)
			pShoePlate=foottempl.listShoePlates.GetValue(KEY4C("UXF"));
		else
			pShoePlate=foottempl.listShoePlates.GetValue(KEY4C("UYF"));
	}
	else
	{
		if(shoePlateX0_Y1==0)
			pShoePlate=foottempl.listShoePlates.GetValue(KEY4C("DXF"));
		else
			pShoePlate=foottempl.listShoePlates.GetValue(KEY4C("DYF"));
	}
	if(pShoePlate)
		norm_offset=pShoePlate->norm_offset;
	PEN_STRU pen;
	pen.width=1;
	if(bUpShoePlate)
	{
		pen.style=PS_SOLID;
		pen.crColor=RGB(0,0,200);
	}
	else
	{	
		pen.style=PS_DASH;
		pen.crColor=RGB(128,0,128);
	}
	f3dLine line;
	line.pen=pen;
	f3dPoint vertexArr[4];
	//绘制角钢肢翼侧靴板
	shoeLines.upLineB.startPt=vertexArr[0]=vertexB1;
	shoeLines.upLineB.endPt=vertexArr[1]=min_vertex1;
	shoeLines.btmLineB.startPt=vertexArr[3]=vertexB0;
	shoeLines.btmLineB.endPt=vertexArr[2]=min_vertex0;
	for(int i=0;i<4;i++)
	{
		line.startPt = vertexArr[i];
		line.endPt   = vertexArr[(i+1)%4];
		g_pDraw->NewLine(line);
	}
	//绘制角钢楞线侧靴板
	shoeLines.upLineF.startPt=min_vertex1+norm*norm_offset;
	shoeLines.upLineF.startPt.z=0;
	vertexArr[0]=shoeLines.upLineF.startPt;
	shoeLines.upLineF.endPt=vertexF1+norm*norm_offset;
	shoeLines.upLineF.endPt.z=0;
	vertexArr[1]=shoeLines.upLineF.endPt;
	shoeLines.btmLineF.startPt=min_vertex0+norm*norm_offset;
	shoeLines.btmLineF.startPt.z=0;
	vertexArr[3]=shoeLines.btmLineF.startPt;
	shoeLines.btmLineF.endPt=vertexF0+norm*norm_offset;
	shoeLines.btmLineF.endPt.z=0;
	vertexArr[2]=shoeLines.btmLineF.endPt;
	for(int i=0;i<4;i++)
	{
		line.startPt = vertexArr[i];
		line.endPt   = vertexArr[(i+1)%4];
		g_pDraw->NewLine(line);
	}
}
static void DrawBoltShoeGraph(f2dLine cut_line,int shoePlateX0_Y1,CFootTempl& foottempl,f3dPoint norm,
								BOOL bUpShoePlate,XUEBANLINE& shoeLines)
{
	f3dPoint xueban_vertex[4];
	f2dLine edge_line;
	if(foottempl.base.shape==0)
	{	//方形底板
		if(shoePlateX0_Y1==0)	//X轴向靴板
		{
			edge_line.startPt.Set(foottempl.base.XA,-foottempl.base.YB);
			edge_line.endPt.Set(foottempl.base.XA,foottempl.base.YA);
			Int2dll(cut_line,edge_line,xueban_vertex[0].x,xueban_vertex[0].y);
			edge_line.startPt.Set(-foottempl.base.XB,-foottempl.base.YB);
			edge_line.endPt.Set(-foottempl.base.XB,foottempl.base.YA);
			Int2dll(cut_line,edge_line,xueban_vertex[1].x,xueban_vertex[1].y);
		}
		else	//Y轴向靴板
		{
			edge_line.startPt.Set(-foottempl.base.XB,foottempl.base.YA);
			edge_line.endPt.Set(foottempl.base.XA,foottempl.base.YA);
			Int2dll(cut_line,edge_line,xueban_vertex[0].x,xueban_vertex[0].y);
			edge_line.startPt.Set(-foottempl.base.XB,-foottempl.base.YB);
			edge_line.endPt.Set(foottempl.base.XA,-foottempl.base.YB);
			Int2dll(cut_line,edge_line,xueban_vertex[1].x,xueban_vertex[1].y);
		}
	}
	else
	{	//圆形底板
		f2dCircle circle;
		circle.radius=foottempl.base.fR;
		//cut_line分别向始终端侧延伸底板圆直径长度，否则当底板半径小于角钢肢宽度时仅可求得一个交点
		f3dPoint cut_line_vec(cut_line.endPt.x-cut_line.startPt.x,cut_line.endPt.y-cut_line.startPt.y,0);
		f3dPoint startPt=f3dPoint(cut_line.startPt.x,cut_line.startPt.y,0)+cut_line_vec*foottempl.base.fR*2;
		f3dPoint endPt=f3dPoint(cut_line.endPt.x,cut_line.endPt.y,0)-cut_line_vec*foottempl.base.fR*2;
		cut_line.startPt.Set(startPt.x,startPt.y);
		cut_line.endPt.Set(endPt.x,endPt.y);
		Int2dlc(cut_line,circle,xueban_vertex[0].x,xueban_vertex[0].y,xueban_vertex[1].x,xueban_vertex[1].y);
	}
	UCS_STRU ucs;
	foottempl.GetUCS(ucs,false);
	ucs.origin=foottempl.origin.Position();
	vector_trans(norm,ucs,FALSE);
	double angle = cal_angle_of_2vec(norm,f3dPoint(norm.x,norm.y,0));
	norm = norm.normalized()*fabs(1.0/cos(angle));
	CFootTempl::SHOEPLATE* pShoePlate=NULL;
	if(bUpShoePlate)
		pShoePlate=foottempl.listShoePlates.GetValue(KEY4C("UXB"));
	else
		pShoePlate=foottempl.listShoePlates.GetValue(KEY4C("DXB"));
	int xuebanThick=10;
	if(pShoePlate!=NULL)
		xuebanThick=pShoePlate->thick;
	else if(bUpShoePlate)
		logerr.Log("UXB靴板缺失!");
	else
		logerr.Log("DXB靴板缺失!");
	xueban_vertex[2] = xueban_vertex[1]+norm*xuebanThick;
	xueban_vertex[3] = xueban_vertex[0]+norm*xuebanThick;
	xueban_vertex[2].z=xueban_vertex[3].z=0;
	//
	shoeLines.upLineB.startPt=xueban_vertex[1];
	shoeLines.upLineB.endPt=xueban_vertex[0];
	shoeLines.btmLineB.startPt=xueban_vertex[2];
	shoeLines.btmLineB.endPt=xueban_vertex[3];
	shoeLines.upLineF=shoeLines.upLineB;
	shoeLines.btmLineF=shoeLines.btmLineB;
	//
	f3dLine line;
	line.pen.crColor=RGB(0,0,200);
	for(int i=0;i<4;i++)
	{
		line.startPt = xueban_vertex[i];
		line.endPt   = xueban_vertex[(i+1)%4];
		if(bUpShoePlate)
		{
			line.pen.style=PS_SOLID;
			line.pen.crColor=RGB(0,0,200);
		}
		else
		{
			line.pen.style=PS_DASH;
			line.pen.crColor=RGB(128,0,128);
		}
		g_pDraw->NewLine(line);
	}
}
static const double cos45=0.70710678118654752440084436210485;	//2的平方根的一半
static void DrawGraph(void* pContext)
{
	CFootDesignTemplDlg* pDlg=(CFootDesignTemplDlg*)pContext;
	if(pDlg==NULL)
		return;
	ATOM_LIST<f3dPoint> vertexs,son_vertexs;
	f3dPoint pos,vertex;
	f3dLine line,axis_line;
	line.pen.crColor=RGB(0,0,200);
	line.pen.width=1;
	line.pen.style=PS_SOLID;
	//1、绘制底板的坐标轴
	axis_line.pen.crColor=RGB(255,0,0);
	axis_line.pen.width=1;
	axis_line.pen.style=PS_DASHDOT;
	//X轴
	axis_line.startPt.Set(-225,0,0);
	axis_line.endPt.Set(225,0,0);
	g_pDraw->NewLine(axis_line);
	axis_line.startPt.Set(205,15,0);
	g_pDraw->NewLine(axis_line);
	axis_line.startPt.Set(205,-15,0);
	g_pDraw->NewLine(axis_line);
	pos.Set(225,0,0);
#ifdef AFX_TARG_ENU_ENGLISH
	g_pDraw->NewText("Axis X",pos,30,f3dPoint(0.0),f3dPoint(0.0),ISolidDraw::TEXT_ALIGN_BTMLEFT,2);
#else
	g_pDraw->NewText("X轴",pos,30,f3dPoint(0.0),f3dPoint(0.0),ISolidDraw::TEXT_ALIGN_BTMLEFT,2);
#endif
	//Y轴
	axis_line.startPt.Set(0,-225,0);
	axis_line.endPt.Set(0,225,0);
	g_pDraw->NewLine(axis_line);
	axis_line.startPt.Set(15,205,0);
	g_pDraw->NewLine(axis_line);
	axis_line.startPt.Set(-15,205,0);
	g_pDraw->NewLine(axis_line);
	pos.Set(0,225,0);
#ifdef AFX_TARG_ENU_ENGLISH
	g_pDraw->NewText("Axis Y",pos,30,f3dPoint(0.0),f3dPoint(0.0),ISolidDraw::TEXT_ALIGN_BTMLEFT,2);
#else
	g_pDraw->NewText("Y轴",pos,30,f3dPoint(0.0),f3dPoint(0.0),ISolidDraw::TEXT_ALIGN_BTMLEFT,2);
#endif
	//2、绘制底板图形
	f3dCircle circle;
	circle.pen.crColor=RGB(0,0,200);
	circle.pen.width=1;
	circle.pen.style=PS_SOLID;
	if(pDlg->foottempl.base.shape==1)	//圆形
	{
		circle.radius=pDlg->foottempl.base.fR;
		circle.centre.Set(0,0,0);
		circle.norm.Set(0,0,1);
		g_pDraw->NewCircle(circle);
	}
	else		//方形
	{
		int XA=pDlg->foottempl.base.XA;
		int XB=pDlg->foottempl.base.XB;
		int YA=pDlg->foottempl.base.YA;
		int YB=pDlg->foottempl.base.YB;
		vertexs.append(f3dPoint(XA-pDlg->foottempl.base.XC[0],-YB,0));
		if(pDlg->foottempl.base.YC[0]>0)
			vertexs.append(f3dPoint(XA,-YB+pDlg->foottempl.base.YC[0],0));
		vertexs.append(f3dPoint(XA,YA-pDlg->foottempl.base.YC[2],0));
		if(pDlg->foottempl.base.XC[2]>0)
			vertexs.append(f3dPoint(XA-pDlg->foottempl.base.XC[2],YA,0));
		vertexs.append(f3dPoint(-XB+pDlg->foottempl.base.XC[3],YA,0));
		if(pDlg->foottempl.base.YC[3]>0)
			vertexs.append(f3dPoint(-XB,YA-pDlg->foottempl.base.YC[3],0));
		vertexs.append(f3dPoint(-XB,-YB+pDlg->foottempl.base.YC[1],0));
		if(pDlg->foottempl.base.XC[1]>0)
			vertexs.append(f3dPoint(-XB+pDlg->foottempl.base.XC[1],-YB,0));
		int nNum=vertexs.GetNodeNum();
		for(int i=0;i<nNum;i++)
		{
			line.startPt = vertexs[i];
			line.endPt   = vertexs[(i+1)%nNum];
			g_pDraw->NewLine(line);
		}
	}
	//2、绘制角钢和靴板在底板上的投影图
	UCS_STRU ucs;
	pDlg->foottempl.GetUCS(ucs,false);
	ucs.origin=pDlg->foottempl.origin.Position();
	XUEBANLINE UXShoeLine,UYShoeLine,DXShoeLine,DYShoeLine;
	XUEBANLINE RibPlateArr[4];	//45度,135度,225度,315度肋板轮廓
	CFootTempl::LINKSHOEPALTE* pLinkSP=NULL;
	for(int i=0;i<2;i++)
	{
		//2.1 绘制角钢在底板上的投影图
		CLDSLineAngle* pJg=NULL;
		BOOL bUpJg;
		if(i==0)
		{
			pJg=pDlg->foottempl.primaryAngle.pAngle;
			pLinkSP=&pDlg->foottempl.upLinkSP;
			bUpJg=TRUE;
		}
		else
		{	
			pJg=pDlg->foottempl.primaryAngleDown.pAngle;
			pLinkSP=&pDlg->foottempl.downLinkSP;
			bUpJg=FALSE;
		}
		if(pJg==NULL)
			continue;
		double wing_wide = pJg->GetWidth();
		double wing_thick = pJg->GetThick();
		f3dPoint axis_z,wing_vec_x,wing_vec_y;
		if(pJg->pStart->handle==pDlg->foottempl.m_hBaseNode)
			axis_z = pJg->End()-pJg->Start();
		else
			axis_z = pJg->Start()-pJg->End();
		normalize(axis_z);
		wing_vec_x = pJg->GetWingVecX();
		wing_vec_y = pJg->GetWingVecY();
		CLDSLineAngle *pComJgArr[4]={NULL};
		WORD group_style=0;
		if(pJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pJg;
			group_style=pGroupJg->group_style;
			pComJgArr[0]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[0],CLS_LINEANGLE);
			pComJgArr[1]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[1],CLS_LINEANGLE);
			if(pGroupJg->group_style>1)	//四角钢组合
			{
				pComJgArr[2]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[2],CLS_LINEANGLE);
				pComJgArr[3]=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[3],CLS_LINEANGLE);
			}
		}
		else
			pComJgArr[0]=pJg;
		if(pComJgArr[0]==NULL)
			return;	//基准角钢出错，无法继续(如:组合角钢子角钢句柄无效)
		for(int j=3;j>=0;j--)
		{	//逆序因为第一根角钢往往是组合角钢的特征（基准）角钢
			if(pComJgArr[j]==NULL)
				continue;
			if(pComJgArr[j]->pSolidBody==NULL)
				pComJgArr[j]->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			vertexs.Empty();
			int k;
			for(k=0;k<6;k++)
			{
				Int3dlf(vertex,pComJgArr[j]->pSolidBody->GetKeyPointAt(k),axis_z,ucs.origin,ucs.axis_z);
				coord_trans(vertex,ucs,FALSE);
				vertexs.append(vertex);
				if(j!=0)
					son_vertexs.append(vertex);
			}
			for(k=0;k<6;k++)
			{
				line.startPt = vertexs[k];
				line.endPt   = vertexs[(k+1)%6];
				if(bUpJg)
				{
					line.pen.style=PS_SOLID;
					line.pen.crColor=RGB(0,0,200);
				}
				else
				{
					line.pen.style=PS_DASH;
					line.pen.crColor=RGB(128,0,128);
				}
				g_pDraw->NewLine(line);
			}
		}
		//2.2 绘制靴板截面在底板上的投影图
		XUEBANLINE shoeLine;
		if(pLinkSP->style_bolt0_weld1==1&&pJg->GetClassTypeId()==CLS_LINEANGLE)	
		{	//靴板焊接到主材角钢上
			f3dPoint work_norm;
			f2dLine lineArr[3];
			if(fabs(dot_prod(wing_vec_x,ucs.axis_y))>cos45)
			{	//主材角钢X肢与底板Y轴一致
				work_norm=pJg->get_norm_y_wing();
				lineArr[0]=f2dLine(vertexs[0],vertexs[1]);
				lineArr[1]=f2dLine(vertexs[3],vertexs[2]);
				lineArr[2]=f2dLine(vertexs[0],vertexs[5]);
				DrawWeldShoeGraph(lineArr,0,pDlg->foottempl,work_norm,bUpJg,shoeLine);	//X轴向的靴板
				if(bUpJg)
					UXShoeLine=shoeLine;
				else
					DXShoeLine=shoeLine;
				work_norm=pJg->get_norm_x_wing();
				lineArr[0]=f2dLine(vertexs[0],vertexs[5]);
				lineArr[1]=f2dLine(vertexs[3],vertexs[4]);
				lineArr[2]=f2dLine(vertexs[0],vertexs[1]);
				DrawWeldShoeGraph(lineArr,1,pDlg->foottempl,work_norm,bUpJg,shoeLine);	//Y轴向的靴板
				if(bUpJg)
					UYShoeLine=shoeLine;
				else
					DYShoeLine=shoeLine;
			}
			else	
			{	//主材角钢X肢与底板X轴一致
				work_norm=pJg->get_norm_y_wing();
				lineArr[0]=f2dLine(vertexs[0],vertexs[1]);
				lineArr[1]=f2dLine(vertexs[3],vertexs[2]);
				lineArr[2]=f2dLine(vertexs[0],vertexs[5]);
				DrawWeldShoeGraph(lineArr,1,pDlg->foottempl,work_norm,bUpJg,shoeLine);	//Y轴向的靴板
				if(bUpJg)
					UYShoeLine=shoeLine;
				else
					DYShoeLine=shoeLine;
				work_norm=pJg->get_norm_x_wing();
				lineArr[0]=f2dLine(vertexs[0],vertexs[5]);
				lineArr[1]=f2dLine(vertexs[3],vertexs[4]);
				lineArr[2]=f2dLine(vertexs[0],vertexs[1]);
				DrawWeldShoeGraph(lineArr,0,pDlg->foottempl,work_norm,bUpJg,shoeLine);	//X轴向的靴板
				if(bUpJg)
					UXShoeLine=shoeLine;
				else
					DXShoeLine=shoeLine;
			}
		}
		else if(pLinkSP->style_bolt0_weld1==1&&pJg->GetClassTypeId()==CLS_GROUPLINEANGLE&&group_style==0)
		{	//靴板焊接到对角组合角钢上
			f3dPoint work_norm;
			f2dLine lineArr1[2],lineArr2[2];
			if(fabs(dot_prod(wing_vec_x,ucs.axis_y))>cos45)
			{	//主材角钢X肢与底板Y轴一致
				lineArr1[0]=f2dLine(vertexs[0],vertexs[1]);
				lineArr1[1]=f2dLine(vertexs[3],vertexs[2]);
				lineArr2[0]=f2dLine(son_vertexs[0],son_vertexs[1]);
				lineArr2[1]=f2dLine(son_vertexs[3],son_vertexs[2]);
				DrawGroupJgWeldShoeGraph(lineArr1,lineArr2,0,pDlg->foottempl,bUpJg,shoeLine);	//X轴向的靴板
				if(bUpJg)
					UXShoeLine=shoeLine;
				else
					DXShoeLine=shoeLine;
				lineArr1[0]=f2dLine(vertexs[0],vertexs[5]);
				lineArr1[1]=f2dLine(vertexs[3],vertexs[4]);
				lineArr2[0]=f2dLine(son_vertexs[0],son_vertexs[5]);
				lineArr2[1]=f2dLine(son_vertexs[3],son_vertexs[4]);
				DrawGroupJgWeldShoeGraph(lineArr1,lineArr2,1,pDlg->foottempl,bUpJg,shoeLine);	//Y轴向的靴板
				if(bUpJg)
					UYShoeLine=shoeLine;
				else
					DYShoeLine=shoeLine;
			}
			else	
			{	//主材角钢X肢与底板X轴一致
				lineArr1[0]=f2dLine(vertexs[0],vertexs[1]);
				lineArr1[1]=f2dLine(vertexs[3],vertexs[2]);
				lineArr2[0]=f2dLine(son_vertexs[0],son_vertexs[1]);
				lineArr2[1]=f2dLine(son_vertexs[3],son_vertexs[2]);
				DrawGroupJgWeldShoeGraph(lineArr1,lineArr2,1,pDlg->foottempl,bUpJg,shoeLine);	//Y轴向的靴板
				if(bUpJg)
					UYShoeLine=shoeLine;
				else
					DYShoeLine=shoeLine;
				lineArr1[0]=f2dLine(vertexs[0],vertexs[5]);
				lineArr1[1]=f2dLine(vertexs[3],vertexs[4]);
				lineArr2[0]=f2dLine(son_vertexs[0],son_vertexs[5]);
				lineArr2[1]=f2dLine(son_vertexs[3],son_vertexs[4]);
				DrawGroupJgWeldShoeGraph(lineArr1,lineArr2,0,pDlg->foottempl,bUpJg,shoeLine);	//X轴向的靴板
				if(bUpJg)
					UXShoeLine=shoeLine;
				else
					DXShoeLine=shoeLine;
			}
		}
		else					
		{	//靴板螺栓连接或搭接到主材角钢上
			if(fabs(dot_prod(wing_vec_x,ucs.axis_y))>cos45)	
			{	//主材角钢X肢与底板Y轴一致
				DrawBoltShoeGraph(f2dLine(vertexs[0],vertexs[1]),0,pDlg->foottempl,pJg->get_norm_y_wing(),bUpJg,shoeLine);	//X轴向的靴板
				if(bUpJg)
					UXShoeLine=shoeLine;
				else
					DXShoeLine=shoeLine;
				DrawBoltShoeGraph(f2dLine(vertexs[0],vertexs[5]),1,pDlg->foottempl,pJg->get_norm_x_wing(),bUpJg,shoeLine);	//Y轴向的靴板
				if(bUpJg)
					UYShoeLine=shoeLine;
				else
					DYShoeLine=shoeLine;
			}
			else		
			{	//主材角钢X肢与底板X轴一致
				DrawBoltShoeGraph(f2dLine(vertexs[0],vertexs[5]),0,pDlg->foottempl,pJg->get_norm_x_wing(),bUpJg,shoeLine);	//X轴向的靴板
				if(bUpJg)
					UXShoeLine=shoeLine;
				else
					DXShoeLine=shoeLine;
				DrawBoltShoeGraph(f2dLine(vertexs[0],vertexs[1]),1,pDlg->foottempl,pJg->get_norm_y_wing(),bUpJg,shoeLine);	//Y轴向的靴板
				if(bUpJg)
					UYShoeLine=shoeLine;
				else
					DYShoeLine=shoeLine;
			}
		}
	}
	//3、绘制底脚螺栓
	LIST_NODE<CFootTempl::BASEPLATE::HOLE> *pHole=NULL;
	for(pHole=pDlg->foottempl.base.listHoles.EnumFirst();pHole;pHole=pDlg->foottempl.base.listHoles.EnumNext())
	{
		circle.radius=pHole->data.hole_d/2.0;
		circle.centre.Set(pHole->data.x,pHole->data.y);
		circle.norm.Set(0,0,1);
		g_pDraw->NewCircle(circle);
	}
	//4、绘制肋板投影图
	CFootTempl::RIBPLATE* pRibPlate=NULL,*pCurSelRibPlate=NULL;	
	if(pDlg->m_iDispType==CFootDesignTemplDlg::DISP_RIB_PLATE)
	{
		int iCurSel=pDlg->m_listCtrlLinkAngles.GetSelectedItem();
		if(iCurSel>=0)
		{
			CSuperGridCtrl::CTreeItem *pSelItem=pDlg->m_listCtrlLinkAngles.GetTreeItem(iCurSel);
			TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pSelItem->m_idProp;
			if(pItemInfo&&pItemInfo->itemType==CFootDesignTemplDlg::ITEM_TYPE_RIBPLATE)
				pCurSelRibPlate=(CFootTempl::RIBPLATE*)pDlg->foottempl.listRibPlates.GetValue(pItemInfo->dwRefData);
		}
	}
	if(pDlg->foottempl.base.shape==1)
	{	//圆形底板,首先绘制45°135°225°315°的四块类似靴板的肋板
		f3dPoint norm,ray_vec;
		f3dLine up_line,bottom_line,center_edge_line;
		up_line.pen.crColor=bottom_line.pen.crColor=center_edge_line.pen.crColor=RGB(0,0,0);
		up_line.pen.width=bottom_line.pen.width=center_edge_line.pen.width=1;
		up_line.pen.style=bottom_line.pen.style=center_edge_line.pen.style=PS_SOLID;
		f2dLine ray_line;
		f2dCircle circle;
		circle.radius=pDlg->foottempl.base.fR;
		double jg_space=0;
		if(pDlg->foottempl.primaryAngle.pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			jg_space=((CLDSGroupLineAngle*)pDlg->foottempl.primaryAngle.pAngle)->jg_space;
		for(pRibPlate=pDlg->foottempl.listRibPlates.GetFirst();pRibPlate;pRibPlate=pDlg->foottempl.listRibPlates.GetNext())
		{
			if(pRibPlate->axisXType!=0)
				continue;
			int iPalte=0;
			if(pRibPlate->pos_para.PARA2.angle==45)
			{	//45度靴板
				norm.Set(cos45,-cos45);
				ray_vec.Set(cos45,cos45);
				iPalte=0;
			}
			else if(pRibPlate->pos_para.PARA2.angle==135)
			{	//135度靴板
				norm.Set(cos45,cos45);
				ray_vec.Set(-cos45,cos45);
				iPalte=1;
			}
			else if(pRibPlate->pos_para.PARA2.angle==225)
			{	//225度靴板
				norm.Set(-cos45,cos45);
				ray_vec.Set(-cos45,-cos45);
				iPalte=2;
			}
			else if(pRibPlate->pos_para.PARA2.angle==315)
			{	//315度靴板
				norm.Set(-cos45,-cos45);
				ray_vec.Set(cos45,-cos45);
				iPalte=3;
			}
			else
				continue;
			line.endPt=ray_vec*pDlg->foottempl.base.fR*2;
			line.startPt=-line.endPt;
			ray_line.endPt.Set(line.endPt.x,line.endPt.y);
			Int2dlc(f2dLine(line.startPt,line.endPt),circle,line.startPt.x,line.startPt.y,line.endPt.x,line.endPt.y);
			if(pDlg->foottempl.quadModes[iPalte])
				up_line.startPt=norm*(pRibPlate->thick*0.5)+ray_vec*((pRibPlate->thick*0.5+jg_space/cos45*0.5)+(pDlg->foottempl.primaryAngle.pAngle->GetThick()/cos45));
			else
				up_line.startPt=norm*(pRibPlate->thick*0.5)+ray_vec*(pRibPlate->thick*0.5+jg_space/cos45*0.5);
			up_line.endPt=norm*(pRibPlate->thick*0.5)+line.startPt;
			bottom_line.startPt=up_line.startPt-norm*pRibPlate->thick;
			bottom_line.endPt=up_line.endPt-norm*pRibPlate->thick;
			center_edge_line.startPt=bottom_line.startPt;
			center_edge_line.endPt=up_line.startPt;
			up_line.pen.crColor=bottom_line.pen.crColor=center_edge_line.pen.crColor=(pCurSelRibPlate==pRibPlate)?RGB(255,0,255):RGB(0,0,0);
			g_pDraw->NewLine(up_line);
			g_pDraw->NewLine(bottom_line);
			g_pDraw->NewLine(center_edge_line);
			RibPlateArr[iPalte].upLineB=up_line;
			RibPlateArr[iPalte].btmLineB=bottom_line;
			char lable[3]="";
			sprintf(lable,"%d",pRibPlate->Serial());
			f3dPoint pos;
			pos.x=0.5*(up_line.startPt.x+up_line.endPt.x);
			pos.y=0.5*(up_line.startPt.y+bottom_line.endPt.y);
			g_pDraw->NewText(lable,pos,30,f3dPoint(0.0),f3dPoint(0.0),ISolidDraw::TEXT_ALIGN_BTMLEFT,2);
		}
	}
	for(pRibPlate=pDlg->foottempl.listRibPlates.GetFirst();pRibPlate;pRibPlate=pDlg->foottempl.listRibPlates.GetNext())
	{
		vertexs.Empty();
		f3dPoint rib_norm,offset_vec,pick_vec,pick_pos;
		f3dLine datum_line;
		int tag=1;
		pick_pos.Set();
		if(pRibPlate->axisXType>=1&&pRibPlate->axisXType<=4)
		{	//指定X轴方向,以单靴板基准
			pick_pos.Set(pRibPlate->pos_para.PARA1.OX,pRibPlate->pos_para.PARA1.OY);
			if(pRibPlate->axisXType==1)			//X+
			{
				pick_vec.Set(1,0,0);
				if(pRibPlate->pos_para.PARA1.OY<0)
					tag=-1;
				if(pRibPlate->upper)//A>0)
				{
					datum_line=UYShoeLine.btmLineB;
					if(tag==-1)
						datum_line=UYShoeLine.btmLineF;
				}
				else
				{
					datum_line=DYShoeLine.btmLineB;
					if(tag==-1)
						datum_line=DYShoeLine.btmLineF;
				}
			}
			else if(pRibPlate->axisXType==2)	//X-
			{
				pick_vec.Set(-1,0,0);
				if(pRibPlate->pos_para.PARA1.OY<0)
					tag=-1;
				if(pRibPlate->upper)//A>0)
				{
					datum_line=UYShoeLine.upLineB;
					if(tag==-1)
						datum_line=UYShoeLine.upLineF;
				}
				else
				{
					datum_line=DYShoeLine.upLineB;
					if(tag==-1)
						datum_line=DYShoeLine.upLineF;
				}
			}
			else if(pRibPlate->axisXType==3)	//Y+
			{
				pick_vec.Set(0,1,0);
				if(pRibPlate->pos_para.PARA1.OX<0)
					tag=-1;
				if(pRibPlate->upper)//A>0)
				{
					datum_line=UXShoeLine.upLineF;
					if(tag==-1)
						datum_line=UXShoeLine.upLineB;
				}
				else
				{	
					datum_line=DXShoeLine.upLineF;
					if(tag==-1)
						datum_line=DXShoeLine.upLineB;
				}
			}
			else if(pRibPlate->axisXType==4)	//Y-
			{
				pick_vec.Set(0,-1,0);
				if(pRibPlate->pos_para.PARA1.OX<0)
					tag=-1;
				if(pRibPlate->upper)//A>0)
				{
					datum_line=UXShoeLine.btmLineF;
					if(tag==-1)
						datum_line=UXShoeLine.btmLineB;
				}
				else
				{
					datum_line=DXShoeLine.btmLineF;
					if(tag==-1)
						datum_line=DXShoeLine.btmLineB;
				}
			}
			rib_norm=datum_line.endPt-datum_line.startPt;
			normalize(rib_norm);
			offset_vec=rib_norm^f3dPoint(0,0,1);
			if(offset_vec*pick_vec<0)
				offset_vec*=-1;
			SnapPerp(&vertex,datum_line,pick_pos);	//点到线的投影
			vertexs.append(vertex);
			vertexs.append(vertex+offset_vec*pRibPlate->B);
			vertexs.append(vertex+offset_vec*pRibPlate->B+rib_norm*tag*pRibPlate->thick);
			vertexs.append(vertex+rib_norm*tag*pRibPlate->thick);
		}
		else  if(pRibPlate->axisXType==5)
		{	//指定射线角度，以双靴板基准
			int nAngleIndex=ftoi(pRibPlate->pos_para.PARA3.angle/22.5);
			rib_norm.Set(cos(pRibPlate->pos_para.PARA3.angle*RADTODEG_COEF),sin(pRibPlate->pos_para.PARA3.angle*RADTODEG_COEF));
			offset_vec=rib_norm^f3dPoint(0,0,1);
			pick_pos=rib_norm*pRibPlate->pos_para.PARA3.fRayDist;
			f3dLine datumLineArr[16]={UXShoeLine.upLineF,RibPlateArr[0].upLineB,
									  RibPlateArr[0].btmLineB,UYShoeLine.btmLineB,
									  UYShoeLine.upLineB,RibPlateArr[1].upLineB,
									  RibPlateArr[1].btmLineB,UXShoeLine.upLineB,
									  UXShoeLine.btmLineB,RibPlateArr[2].upLineB,
									  RibPlateArr[2].btmLineB,UYShoeLine.upLineF,
									  UYShoeLine.btmLineF,RibPlateArr[3].upLineB,
									  RibPlateArr[3].btmLineB,UXShoeLine.btmLineF};
			if(nAngleIndex>=1&&nAngleIndex<=15)
			{
				f3dPoint startPt,endPt;
				Int3dll(datumLineArr[nAngleIndex-1],f3dLine(pick_pos,pick_pos+offset_vec*10000),startPt);
				Int3dll(datumLineArr[nAngleIndex],f3dLine(pick_pos,pick_pos-offset_vec*10000),endPt);
				vertexs.append(startPt);
				vertexs.append(startPt+rib_norm*pRibPlate->thick);
				vertexs.append(endPt+rib_norm*pRibPlate->thick);
				vertexs.append(endPt);
			}
		}
		//
		if(vertexs.GetNodeNum()<3)
			continue;
		int nNum=vertexs.GetNodeNum();
		if(pRibPlate->upper)//A>0)
		{
			line.pen.style=PS_SOLID;
			line.pen.crColor=RGB(0,0,200);
		}
		else
		{
			line.pen.style=PS_DASH;
			line.pen.crColor=RGB(128,0,128);
		}
		if(pRibPlate==pCurSelRibPlate)
			line.pen.crColor=RGB(255,0,255);
		for(int i=0;i<nNum;i++)
		{
			line.startPt = vertexs[i];
			line.endPt   = vertexs[(i+1)%nNum];
			g_pDraw->NewLine(line);
		}
		char lable[3]="";
		sprintf(lable,"%d",pRibPlate->Serial());
		f3dPoint pos;
		pos.x=0.5*(vertexs[0].x+vertexs[1].x);
		pos.y=0.5*(vertexs[1].y+vertexs[2].y);
		g_pDraw->NewText(lable,pos,30,f3dPoint(0.0),f3dPoint(0.0),ISolidDraw::TEXT_ALIGN_BTMLEFT,2);
	}
	g_pOper->ZoomAll(0.8);
}
/////////////////////////////////////////////////////////////////////////////
// CFootDesignTemplDlg dialog

CFootDesignTemplDlg::CFootDesignTemplDlg(CWnd* pParent /*=NULL*/)
	: CCallBackDialog(CFootDesignTemplDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFootDesignTemplDlg)
	m_bUpAndDownSide=FALSE;
	m_iDispType=DISP_OTHER;
	m_bSaveData=FALSE;
	m_idActiveRibPlate=0;
	m_iCmdParaType=-1;
	m_iActiveTabPage=0;
	//角钢列表
#ifdef AFX_TARG_ENU_ENGLISH
	m_listCtrlLinkAngles.AddColumnHeader("Pos Label",105);
	m_listCtrlLinkAngles.AddColumnHeader("Handle",55);
	m_listCtrlLinkAngles.AddColumnHeader("Bolt Size",65);
	m_listCtrlLinkAngles.AddColumnHeader("Quantity",40);
	m_listCtrlLinkAngles.AddColumnHeader("Rows",55);
	m_listCtrlLinkAngles.AddColumnHeader("Layout",80);
#else
	m_listCtrlLinkAngles.AddColumnHeader("位置标识",105);
	m_listCtrlLinkAngles.AddColumnHeader("句柄",55);
	m_listCtrlLinkAngles.AddColumnHeader("螺栓规格",65);
	m_listCtrlLinkAngles.AddColumnHeader("数量",40);
	m_listCtrlLinkAngles.AddColumnHeader("排数",55);
	m_listCtrlLinkAngles.AddColumnHeader("布孔方式",80);
#endif
	//}}AFX_DATA_INIT
	m_pDrawSolid=NULL;
}
CFootDesignTemplDlg::~CFootDesignTemplDlg()   // standard destructor
{
}

void CFootDesignTemplDlg::OnClose()
{
	if(m_pDrawSolid && CDrawSolidFactory::Destroy(m_pDrawSolid->GetSerial()))
		m_pDrawSolid=NULL;
	//由于当前激活的OpenGL引擎m_pDrawSolid已销毁，如不重新激活全局工作环境中的OpenGL引擎
	//会导致glViewPort等操作函数执先失败 wjh-2015.11.18
	g_pSolidSet->Activate();
	CCallBackDialog::OnClose();
}

void CFootDesignTemplDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFootDesignTemplDlg)
	DDX_Control(pDX, IDC_TAB_GROUP, m_ctrlPropGroup);
	DDX_Control(pDX, IDC_LIST_BOX, m_xPropList);
	DDX_Control(pDX, IDC_LIST_SHOE_PLATES, m_listCtrlShoePlates);
	DDX_Control(pDX, IDC_LIST_ANGLES, m_listCtrlLinkAngles);
	DDX_Check(pDX, IDC_CHK_SAVE_DATA, m_bSaveData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFootDesignTemplDlg, CDialog)
	//{{AFX_MSG_MAP(CFootDesignTemplDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GROUP, &CFootDesignTemplDlg::OnTcnSelchangeTabGroup)
	ON_COMMAND(ID_ADD_LINKANGLE, &CFootDesignTemplDlg::OnAddLinkAngle)
	ON_COMMAND(ID_ADD_HOLE, &CFootDesignTemplDlg::OnAddHole)
	ON_BN_CLICKED(IDC_BN_ADD_BASE_HOLE, &CFootDesignTemplDlg::OnAddHole)
	ON_BN_CLICKED(IDC_BN_ADD_RIB_PLATE, &CFootDesignTemplDlg::OnBnClickedBnAddRibPlate)
	ON_BN_CLICKED(IDC_BN_PAN, OnBnPan)
	ON_BN_CLICKED(IDC_BN_OSNAP,OnBnOsnap)
	ON_BN_CLICKED(IDC_BN_ZOOM,OnBnZoom)
	ON_BN_CLICKED(IDC_BN_OTHER,OnBnOther)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFootDesignTemplDlg message handlers
static BOOL LocalFootDesignCallBackCommand(CWnd* pCallWnd,UINT message,WPARAM wParam)
{
	CPropertyList* pPropList = (CPropertyList*)pCallWnd;
	CFootTempl* pFootTempl=(CFootTempl*)wParam;//pViewDlg->m_pWorkView;
	//CFootDesignTemplDlg *pTemplDlg=(CFootDesignTemplDlg*)pPropList->GetParent();
	//CXhChar50 tem_str;
	if(message==CFootDesignTemplDlg::GetPropID("base.origin"))	//定位原点（牛蹄板使用）
	{
		CDatumPointPropertyDlg desPointDlg;
		desPointDlg.m_xPoint=pFootTempl->origin;
		if(desPointDlg.DoModal()==IDOK)
		{
			pFootTempl->origin=desPointDlg.m_xPoint;
			pFootTempl->origin.UpdatePos(console.GetActiveModel());
		}
	}
	else if(message==CFootDesignTemplDlg::GetPropID("base.worknorm"))	//定位原点（牛蹄板使用）
	{
		CDesignVectorPropDlg desvecDlg;
		desvecDlg.m_xVector.vector=pFootTempl->axis_z;
		if(desvecDlg.DoModal()==IDOK)
		{
			desvecDlg.m_xVector.UpdateVector(console.GetActiveModel());
			pFootTempl->axis_z=desvecDlg.m_xVector.vector;
			GEPOINT lenBaseVec(0,0,-1);
			if(pFootTempl->primaryAngle.pAngle&&pFootTempl->primaryAngleDown.pAngle==NULL)
			{	//单侧塔脚需要验证输出法线方向正负方向 wjh-2016.3.28
				lenBaseVec=pFootTempl->primaryAngle.pAngle->Start()-pFootTempl->primaryAngle.pAngle->End();
				if(pFootTempl->m_hBaseNode==pFootTempl->primaryAngle.pAngle->pStart->handle)
					lenBaseVec*=-1;
			}
			if(lenBaseVec*pFootTempl->axis_z<0)
				pFootTempl->axis_z*=-1.0;
		}
	}
	else
		return FALSE;
	return TRUE;
}
BOOL CFootDesignTemplDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	SetCallBackFunc(LocalFootDesignCallBackCommand);
	UCS_STRU ucs;
	//内部启动
	InitPropHashtable();
	if(m_bInernalStart)
		FinishSelectObjOper(m_hPickObj,GetEventPropId());
	else
	{
		foottempl.GetUCS(ucs);
		foottempl.origin.SetPosition(ucs.origin);
	}
	if( foottempl.quadModes[0]&& foottempl.quadModes[2]&&
	   !foottempl.quadModes[1]&&!foottempl.quadModes[3])
		m_uidDiBanSketchBmp=IDB_BMP_FOOT_SERIES_RIGHT2;
	else
		m_uidDiBanSketchBmp=IDB_BMP_FOOT_SERIES_RIGHT;
	//初始化绘图引擎
	if(m_pDrawSolid==NULL)
		m_pDrawSolid=CDrawSolidFactory::CreateDrawEngine();
	g_pDraw=m_pDrawSolid->SolidDraw();
	g_pSet=m_pDrawSolid->SolidSet();
	g_pSnap=m_pDrawSolid->SolidSnap();
	g_pOper=m_pDrawSolid->SolidOper();
	InitDrawSolidInfo();
	//标签栏设置
	m_ctrlPropGroup.DeleteAllItems();
#ifdef AFX_TARG_ENU_ENGLISH
	m_ctrlPropGroup.InsertItem(0,"Basic");
	m_ctrlPropGroup.InsertItem(1,"Base");
	m_ctrlPropGroup.InsertItem(2,"Shoe");
	m_ctrlPropGroup.InsertItem(3,"Rib");
#else
	m_ctrlPropGroup.InsertItem(0,"基本");
	m_ctrlPropGroup.InsertItem(1,"底板");
	m_ctrlPropGroup.InsertItem(2,"靴板");
	m_ctrlPropGroup.InsertItem(3,"肋板");
#endif
	m_ctrlPropGroup.SetCurSel(m_iActiveTabPage);
	if(!m_bInernalStart)
	{
	foottempl.RetrieveLinkAngles();
	foottempl.RetrieveShoePlates();
	for(CFootTempl::SHOEPLATE* pShoePlate=foottempl.listShoePlates.GetFirst();pShoePlate;pShoePlate=foottempl.listShoePlates.GetNext())
		foottempl.RetrieveLinkAngles(pShoePlate);
	}
	//列表框设置
	m_listCtrlLinkAngles.InitListCtrl();
	m_listCtrlLinkAngles.EnableSortItems(false);
	m_listCtrlLinkAngles.SetModifyValueFunc(ModifyLinkAngleValue);
	m_listCtrlLinkAngles.SetContextMenuFunc(LinkAngleContextMenu);
	m_listCtrlLinkAngles.SetDeleteItemFunc(DeleteLinkAngleItem);
	m_listCtrlLinkAngles.SetItemChangedFunc(FireLinkAnglesItemChanged);
	RefreshLinkAngleListCtrl();
	//
	m_listCtrlShoePlates.InitListCtrl();
	m_listCtrlShoePlates.EnableSortItems(false);
	m_listCtrlShoePlates.SetModifyValueFunc(FireValueModify);
	m_listCtrlShoePlates.SetContextMenuFunc(FireContextMenu);
	m_listCtrlShoePlates.SetDeleteItemFunc(FireDeleteItem);
	m_listCtrlShoePlates.SetItemChangedFunc(FireItemChanged);
	RefreshShoePlateListCtrl();
	if(m_iDispType==DISP_BASE_PLATE||m_iDispType==DISP_RIB_PLATE)
	{
		GetDlgItem(IDC_BN_ADD_BASE_HOLE)->ShowWindow(SW_SHOW);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("Base Hole List:");
#else
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("底脚孔列表:");
#endif
	}
	else
	{
		GetDlgItem(IDC_BN_ADD_BASE_HOLE)->ShowWindow(SW_HIDE);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("Shoe Plate List:");
#else
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("靴板列表:");
#endif
	}
	if(m_iDispType==DISP_RIB_PLATE)
	{
		GetDlgItem(IDC_BN_ADD_RIB_PLATE)->ShowWindow(SW_SHOW);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("Rib Plate List:");
#else
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("肋板列表:");
#endif
	}
	else
	{
		GetDlgItem(IDC_BN_ADD_RIB_PLATE)->ShowWindow(SW_HIDE);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("Connection Angle List:");
#else
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("连接角钢列表:");
#endif
	}
	UpdateSketchMap();

	//初始化模板数据
	if(!m_bInernalStart)
	{
	foottempl.UpdateLinkAngleOddment(foottempl.primaryAngle);
	if(foottempl.primaryAngle.g==0||foottempl.primaryAngle.pAngle->size_wide!=foottempl.primaryAngle.angleWidth)
		foottempl.UpdateLinkAngleZhunJu(foottempl.primaryAngle);
	if(foottempl.primaryAngleDown.pAngle)
		foottempl.UpdateLinkAngleOddment(foottempl.primaryAngleDown);
	if( foottempl.primaryAngleDown.g==0||(
		foottempl.primaryAngleDown.pAngle&&foottempl.primaryAngleDown.pAngle->size_wide!=foottempl.primaryAngleDown.angleWidth))	
		foottempl.UpdateLinkAngleZhunJu(foottempl.primaryAngleDown);
	if(foottempl.primaryAngle.bolt.siBoltSpace==0 || foottempl.primaryAngle.bolt.siEndSpace==0)
		foottempl.UpdateBoltEndSpace(foottempl.primaryAngle);
	if(foottempl.primaryAngleDown.bolt.siBoltSpace==0 || foottempl.primaryAngleDown.bolt.siEndSpace==0)
		foottempl.UpdateBoltEndSpace(foottempl.primaryAngleDown);
	}
	//属性栏设置
	if(foottempl.primaryAngleDown.pAngle)
		m_bUpAndDownSide=TRUE;
	m_xPropList.SetDividerScale(0.5);
	m_xPropList.m_hPromptWnd=GetDlgItem(IDC_E_PROP_HELP_STR)->GetSafeHwnd();
	m_xPropList.m_iPropGroup=m_iActiveTabPage;
	DisplayDesignFootProperty();
	if(m_bInernalStart)	
	{	//恢复选中项
		CPropTreeItem *pItem=m_xPropList.FindItemByPropId(m_idEventProp,NULL);
		m_xPropList.SetFocus();
		if(pItem)
			m_xPropList.SetCurSel(pItem->m_iIndex);	//设置选中项	
		CXhChar50 tem_str;
		CFootTempl* pFootTempl=&foottempl;
		if(m_iBreakType=1&&m_iCmdParaType==CFootDesignTemplDlg::GetPropID("base.origin"))	//定位原点（牛蹄板使用）
		{
			if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.origin.x"),tem_str)>0)
				m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.origin.x"),tem_str);
			m_xPropList.SetItemReadOnly(CFootDesignTemplDlg::GetPropID("base.origin.x"),pFootTempl->origin.datum_pos_style!=0);
			if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.origin.y"),tem_str)>0)
				m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.origin.y"),tem_str);
			m_xPropList.SetItemReadOnly(CFootDesignTemplDlg::GetPropID("base.origin.y"),pFootTempl->origin.datum_pos_style!=0);
			if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.origin.z"),tem_str)>0)
				m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.origin.z"),tem_str);
			m_xPropList.SetItemReadOnly(CFootDesignTemplDlg::GetPropID("base.origin.z"),pFootTempl->origin.datum_pos_style!=0);
			if(fabs(foottempl.axis_z.x)+fabs(foottempl.axis_z.y)>0)
			{	//非标准斜塔脚，更改定位基点X,Y分量时其Z分量应根据原始平面重新计算
				GEPOINT org;
				CLDSNode *pNode = console.FromNodeHandle(foottempl.m_hBaseNode);
				if(pNode!=NULL)
				{
					Int3dlf(org,foottempl.origin.Position(),GEPOINT(0,0,1),pNode->Position(),foottempl.axis_z);
					foottempl.origin.SetPositionZ(org.z);
					UpdateBasePlateOriginProperty();
				}
			}
		}
		else if(m_iBreakType=1&&m_iCmdParaType==CFootDesignTemplDlg::GetPropID("base.worknorm"))	//定位原点（牛蹄板使用）
		{
			if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.worknorm.x"),tem_str)>0)
				m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.worknorm.x"),tem_str);
			if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.worknorm.y"),tem_str)>0)
				m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.worknorm.y"),tem_str);
			if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.worknorm.z"),tem_str)>0)
				m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.worknorm.z"),tem_str);
			foottempl.GetUCS(ucs);	//当法线发生更改时应自动调整底板中心定位点坐标 wjh-2016.8.4
			UpdateBasePlateOriginProperty();
		}
	}
	//初始绘图区域
	g_pDraw->BuildDisplayList(this);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CFootDesignTemplDlg::UpdateBasePlateOriginProperty()
{
	CXhChar50 tem_str;
	if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.origin"),tem_str)>0)
		m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.origin"),tem_str);
	if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.origin.x"),tem_str)>0)
		m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.origin.x"),tem_str);
	if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.origin.y"),tem_str)>0)
		m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.origin.y"),tem_str);
	if(GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.origin.z"),tem_str)>0)
		m_xPropList.SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.origin.z"),tem_str);
}
void CFootDesignTemplDlg::UpdateSketchMap()
{
	m_xCurSketchBmp.Detach();
	if(m_iDispType==DISP_RIB_PLATE)
		m_xCurSketchBmp.LoadBitmap(IDB_BMP_FOOT_RIBPLATE);
	else				//底板示意图
		m_xCurSketchBmp.LoadBitmap(m_uidDiBanSketchBmp);
	CStatic *pPic=(CStatic*)GetDlgItem(IDC_PIC_SKETCH);
	pPic->SetBitmap(m_xCurSketchBmp);
}

void CFootDesignTemplDlg::InitDrawSolidInfo()
{
	CWnd *pWnd = GetDlgItem(IDC_S_MAP);
	g_pSet->Init(pWnd->GetSafeHwnd(),GetSafeHwnd());
	g_pDraw->InitialUpdate();
	g_pSet->SetBkColor(RGB(192,192,192));	//设置背景颜色
	//设置坐标系
	CRect rect;
	pWnd->GetWindowRect(&rect);	//绘图有效区的屏坐标区域
	ScreenToClient(&rect);
	rgn_ucs.origin.Set(rect.CenterPoint().x,rect.CenterPoint().y);
	rgn_ucs.axis_x.Set(1,0,0);
	rgn_ucs.axis_y.Set(0,1,0);
	rgn_ucs.axis_z.Set(0,0,1);
	g_pSet->SetObjectUcs(rgn_ucs);
	//设置画笔
	PEN_STRU pen;
	pen.crColor = RGB(200,0,200);
	pen.width = 1;
	pen.style = PS_SOLID;
	g_pSet->SetPen(pen);
	//常规设置
	g_pSet->SetDisplayType(DISP_LINE);	
	g_pSet->SetDisplayFunc(DrawGraph);
	g_pSet->SetSolidAndLineStatus(FALSE);
	g_pSet->SetArcSamplingLength(5);
	g_pSet->SetSmoothness(36);
	g_pSet->SetDisplayLineVecMark(FALSE);//显示杆件起始端标记
	g_pSet->SetDispObjectUcs(FALSE);
	g_pSet->SetRotOrg(f3dPoint(0,0,0));
	g_pSet->SetZoomStyle(ROT_CENTER);
	g_pOper->ZoomAll(0.95);		//需进行缩放，否则界面显示过小而看不清
	//字体设置
	char sFontFile[MAX_PATH],sBigFontFile[MAX_PATH],sAppPath[MAX_PATH];
	GetSysPath(sAppPath);
	sprintf(sFontFile,"%ssys\\simplex.shx",sAppPath);
	g_pSet->SetShxFontFile(sFontFile);
	sprintf(sBigFontFile,"%ssys\\GBHZFS.shx",sAppPath);
	g_pSet->SetBigFontFile(sBigFontFile);
}
const DWORD HASHTABLESIZE = 500;
IMPLEMENT_PROP_FUNC(CFootDesignTemplDlg);
void CFootDesignTemplDlg::InitPropHashtable()
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);
#ifdef AFX_TARG_ENU_ENGLISH
	//基本信息
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Basic Parameters",""));
	AddPropItem("C",PROPLIST_ITEM(id++,"Collision gap","Angle's tip point to base plate"));
	AddPropItem("m_hBaseNode",PROPLIST_ITEM(id++,"Depend Node","Assemble point of base plates"));
	AddPropItem("m_bWeldDiZuo",PROPLIST_ITEM(id++,"Weld To Base","Weld angle and base plate together","YES|NO"));
	AddPropItem("primaryAngle",PROPLIST_ITEM(id++,"Primary Angle","The Primary connection angle of base plates group"));
	AddPropItem("primaryAngle.oddment",PROPLIST_ITEM(id++,"Work Ends",""));
	AddPropItem("primaryAngle.ZhunJu",PROPLIST_ITEM(id++,"G table","Primary angle's G table"));
	AddPropItem("primaryAngle.g",PROPLIST_ITEM(id++,"G table","Primary angle's G table"));
	AddPropItem("primaryAngle.g1",PROPLIST_ITEM(id++,"G1 table","Primary angle's G1 table"));
	AddPropItem("primaryAngle.g2",PROPLIST_ITEM(id++,"G2 table","Primary angle's G2 table"));
	AddPropItem("primaryAngle.bolt.d",PROPLIST_ITEM(id++,"Bolt Diameter","","M12|M16|M20|M24"));
	AddPropItem("primaryAngle.bolt.num",PROPLIST_ITEM(id++,"Number",""));
	AddPropItem("primaryAngle.bolt.rows",PROPLIST_ITEM(id++,"Rows","","0.Singular|1.Double"));
	AddPropItem("primaryAngle.bolt.layout",PROPLIST_ITEM(id++,"Layout","","0.Left High Right Low|1.Left Low Right High"));
	AddPropItem("primaryAngle.bolt.EndSpace",PROPLIST_ITEM(id++,"螺栓端距",""));
	AddPropItem("primaryAngle.bolt.BoltSpace",PROPLIST_ITEM(id++,"螺栓间距",""));
	AddPropItem("primaryAngleDown",PROPLIST_ITEM(id++,"Under Primary Angle",""));
	AddPropItem("primaryAngleDown.oddment",PROPLIST_ITEM(id++,"Work Ends",""));
	AddPropItem("primaryAngleDown.ZhunJu",PROPLIST_ITEM(id++,"G table","Downside primary angle's G table"));
	AddPropItem("primaryAngleDown.g",PROPLIST_ITEM(id++,"G table","Downside Primary angle's G table"));
	AddPropItem("primaryAngleDown.g1",PROPLIST_ITEM(id++,"G1 table","Downside Primary angle's G1 table"));
	AddPropItem("primaryAngleDown.g2",PROPLIST_ITEM(id++,"G2 table","Downside Primary angle's G2 table"));
	AddPropItem("primaryAngleDown.bolt.d",PROPLIST_ITEM(id++,"Bolt Diameter","","M12|M16|M20|M24"));
	AddPropItem("primaryAngleDown.bolt.num",PROPLIST_ITEM(id++,"Number",""));
	AddPropItem("primaryAngleDown.bolt.rows",PROPLIST_ITEM(id++,"Rows","","0.Singular|1.Double"));
	AddPropItem("primaryAngleDown.bolt.layout",PROPLIST_ITEM(id++,"Layout","","0.Left High Right Low|1.Left Low Right High"));
	AddPropItem("primaryAngleDown.bolt.EndSpace",PROPLIST_ITEM(id++,"螺栓端距",""));
	AddPropItem("primaryAngleDown.bolt.BoltSpace",PROPLIST_ITEM(id++,"螺栓间距",""));
	//底板信息
	AddPropItem("basePlateInfo",PROPLIST_ITEM(id++,"Bottom Plate Parameters",""));
	AddPropItem("base.sPartNo",PROPLIST_ITEM(id++,"Label",""));
	AddPropItem("base.cMaterial",PROPLIST_ITEM(id++,"Material","",MakeMaterialMarkSetString()));
	AddPropItem("base.shape",PROPLIST_ITEM(id++,"Profile","","0.Circle|1.Rectangle"));
	AddPropItem("base.thick",PROPLIST_ITEM(id++,"Thickness",""));
	AddPropItem("base.nNormOffsetType",PROPLIST_ITEM(id++,"Normal Offset Ttype","","0.Upper plane|1.Under plane|2.Specified offset"));
	AddPropItem("base.nNormOffset",PROPLIST_ITEM(id++,"Normal Offset",""));
	AddPropItem("base.origin",PROPLIST_ITEM(id++,"Location",""));
	AddPropItem("base.origin.x",PROPLIST_ITEM(id++,"X:",""));
	AddPropItem("base.origin.y",PROPLIST_ITEM(id++,"Y:",""));
	AddPropItem("base.origin.z",PROPLIST_ITEM(id++,"Z:",""));
	AddPropItem("base.worknorm",PROPLIST_ITEM(id++,"Normal",""));
	AddPropItem("base.worknorm.x",PROPLIST_ITEM(id++,"X:",""));
	AddPropItem("base.worknorm.y",PROPLIST_ITEM(id++,"Y:",""));
	AddPropItem("base.worknorm.z",PROPLIST_ITEM(id++,"Z:",""));
	AddPropItem("base.R",PROPLIST_ITEM(id++,"Radius",""));
	AddPropItem("base.XA",PROPLIST_ITEM(id++,"X+ Width",""));//底板水平正向尺寸"));
	AddPropItem("base.XB",PROPLIST_ITEM(id++,"X- Width",""));//底板水平负向尺寸"));
	AddPropItem("base.XC0",PROPLIST_ITEM(id++,"Ⅳ Fillet Width",""));
	AddPropItem("base.XC2",PROPLIST_ITEM(id++,"Ｉ Fillet Width",""));
	AddPropItem("base.XC1",PROPLIST_ITEM(id++,"Ⅲ Fillet Width",""));
	AddPropItem("base.XC3",PROPLIST_ITEM(id++,"Ⅱ Fillet Width",""));
	AddPropItem("base.YA",PROPLIST_ITEM(id++,"Y+ Fillet Width",""));
	AddPropItem("base.YB",PROPLIST_ITEM(id++,"Y- Fillet Width",""));
	AddPropItem("base.YC0",PROPLIST_ITEM(id++,"Ⅳ Fillet Width",""));
	AddPropItem("base.YC2",PROPLIST_ITEM(id++,"Ｉ Fillet Width",""));
	AddPropItem("base.YC1",PROPLIST_ITEM(id++,"Ⅲ Fillet Width",""));
	AddPropItem("base.YC3",PROPLIST_ITEM(id++,"Ⅱ Fillet Width",""));
	//靴板信息
	AddPropItem("upLinkSP",PROPLIST_ITEM(id++,"Shoe Plate Parameters",""));
	AddPropItem("upLinkSP.A",PROPLIST_ITEM(id++,"Parameter A",""));
	AddPropItem("upLinkSP.B",PROPLIST_ITEM(id++,"Parameter B ",""));
	AddPropItem("upLinkSP.C",PROPLIST_ITEM(id++,"Parameter C",""));
	AddPropItem("upLinkSP.D",PROPLIST_ITEM(id++,"Parameter D",""));
	AddPropItem("upLinkSP.E",PROPLIST_ITEM(id++,"Parameter E",""));
	AddPropItem("upLinkSP.F",PROPLIST_ITEM(id++,"Parameter F",""));
	AddPropItem("upLinkSP.M",PROPLIST_ITEM(id++,"Shoe Plate's Extend Remainder","0:Extend to base plate edge, else: the remainder to edge"));
	AddPropItem("upLinkSP.AL",PROPLIST_ITEM(id++,"Extend Length Of Angle Direction","Extend Length Of Datum Angle Direction，0:calculate the length by plate's bolts"));
	AddPropItem("upLinkSP.CM",PROPLIST_ITEM(id++,"Cut_Angle Mode","","0.Cut Angle|1.Cur Arc"));
	AddPropItem("upLinkSP.style_bolt0_weld1",PROPLIST_ITEM(id++,"Connecion Style","Connection style of shoe plate with primary angle","0.Bolt connect|1.Butt Welding|2.Lap Welding"));
	AddPropItem("upLinkSP.m_bBreakXueBan0C",PROPLIST_ITEM(id++,"Break upper X shoe","","0.Across|1.Break"));
	AddPropItem("upLinkSP.m_bBreakXueBan90C",PROPLIST_ITEM(id++,"Break upper Y shoe","","0.Across|1.Break"));
	AddPropItem("downLinkSP",PROPLIST_ITEM(id++,"Shoe Plate Parameters",""));
	AddPropItem("downLinkSP.A",PROPLIST_ITEM(id++,"Parameter A",""));
	AddPropItem("downLinkSP.B",PROPLIST_ITEM(id++,"Parameter B",""));
	AddPropItem("downLinkSP.C",PROPLIST_ITEM(id++,"Parameter C",""));
	AddPropItem("downLinkSP.D",PROPLIST_ITEM(id++,"Parameter D",""));
	AddPropItem("downLinkSP.E",PROPLIST_ITEM(id++,"Parameter E",""));
	AddPropItem("downLinkSP.F",PROPLIST_ITEM(id++,"Parameter F",""));
	AddPropItem("downLinkSP.M",PROPLIST_ITEM(id++,"Shoe Plate's Extend Remainder","0:Extend to base plate edge, else: the remainder to edge"));
	AddPropItem("downLinkSP.AL",PROPLIST_ITEM(id++,"Extend Length Of Angle Direction","Extend Length Of Datum Angle Direction，0:calculate the length by plate's bolts"));
	AddPropItem("downLinkSP.CM",PROPLIST_ITEM(id++,"Cut_Angle Mode","","0.Cut Angle|1.Cur Arc"));
	AddPropItem("downLinkSP.style_bolt0_weld1",PROPLIST_ITEM(id++,"Connecion Style","Connection style of shoe plate with primary angle","0.Bolt connect|1.Butt Welding|2.Lap Welding"));
	AddPropItem("downLinkSP.m_bBreakXueBan0C",PROPLIST_ITEM(id++,"Break upper X shoe","","0.Across|1.Break"));
	AddPropItem("downLinkSP.m_bBreakXueBan90C",PROPLIST_ITEM(id++,"Break upper Y shoe","","0.Across|1.Break"));
	//肋板信息
	AddPropItem("ribPlateInfo",PROPLIST_ITEM(id++,"Rib's Parameters",""));
	AddPropItem("RibPlate.Serial",PROPLIST_ITEM(id++,"Serial",""));
	AddPropItem("RibPlate.cMaterial",PROPLIST_ITEM(id++,"Material","material of rib plate",MakeMaterialMarkSetString()));
	AddPropItem("RibPlate.PartNo",PROPLIST_ITEM(id++,"Label","Rib's part label"));
	AddPropItem("RibPlate.origin",PROPLIST_ITEM(id++,"Location Point","Location point of rib plate"));
	AddPropItem("RibPlate.OX",PROPLIST_ITEM(id++,"Ox","Location point's x coordinate"));
	AddPropItem("RibPlate.OY",PROPLIST_ITEM(id++,"Oy","Location point's y coordinate"));
	AddPropItem("RibPlate.axisXType",PROPLIST_ITEM(id++,"welding line direction","The location style of welding line(axis x of rib plate) between rib and base plate","0.Specified angle|1.X+|2.X-|3.Y+|4.Y-|5.Specified vertical line angle"));
	AddPropItem("RibPlate.axisX_x",PROPLIST_ITEM(id++,"X:",""));
	AddPropItem("RibPlate.axisX_y",PROPLIST_ITEM(id++,"Y:",""));
	AddPropItem("RibPlate.angle",PROPLIST_ITEM(id++,"Ray line angle",""));
	AddPropItem("RibPlate.fRayDist",PROPLIST_ITEM(id++,"Ray distance",""));
	AddPropItem("RibPlate.thick",PROPLIST_ITEM(id++,"Thickness","Rib plate thickness"));
	AddPropItem("RibPlate.upper",PROPLIST_ITEM(id++,"Upside","The position of rib plates","upside|downside"));
	AddPropItem("RibPlate.A",PROPLIST_ITEM(id++,"Height A","A Pamrameter"));
	AddPropItem("RibPlate.B",PROPLIST_ITEM(id++,"Width B","B Pamrameter"));
	AddPropItem("RibPlate.C",PROPLIST_ITEM(id++,"Rise height C","C Parameter"));
	AddPropItem("RibPlate.D",PROPLIST_ITEM(id++,"Top width D","D Parameter"));
	AddPropItem("RibPlate.iCutMode",PROPLIST_ITEM(id++,"Cut_Angle Mode","","0.Cut Angle|1.Cut Arc"));
	AddPropItem("RibPlate.fCutAngleWidth",PROPLIST_ITEM(id++,"Inner fillet width E",""));
	AddPropItem("RibPlate.fGradientAngle",PROPLIST_ITEM(id++,"Gradient Angle",""));
	AddPropItem("RibPlate.MH",PROPLIST_ITEM(id++,"Parameter H","mm"));
	AddPropItem("RibPlate.MF",PROPLIST_ITEM(id++,"Parameter F","mm"));
	AddPropItem("RibPlate.MG",PROPLIST_ITEM(id++,"Parameter G","mm"));
	AddPropItem("RibPlate.slotW",PROPLIST_ITEM(id++,"Water-slot width",""));
#else
	//基本信息
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"基本参数","涉及塔脚整体设计的参数"));
	AddPropItem("C",PROPLIST_ITEM(id++,"碰撞间隙C","角钢端楞点到底板距离"));
	AddPropItem("m_hBaseNode",PROPLIST_ITEM(id++,"依赖节点","底脚板的装配节点"));
	AddPropItem("m_bWeldDiZuo",PROPLIST_ITEM(id++,"焊接到底座","连接主材角钢端与底板焊在一起","是|否"));
	AddPropItem("primaryAngle",PROPLIST_ITEM(id++,"主材角钢","塔脚的连接主材角钢"));
	AddPropItem("primaryAngle.oddment",PROPLIST_ITEM(id++,"连接端负头","根据碰撞间隙实时计算的连接端负头"));
	AddPropItem("primaryAngle.ZhunJu",PROPLIST_ITEM(id++,"布孔准距","螺栓在基准材上的布置准距"));
	AddPropItem("primaryAngle.g",PROPLIST_ITEM(id++,"g","布置单排螺栓,在基准材上的准距g"));
	AddPropItem("primaryAngle.g1",PROPLIST_ITEM(id++,"g1","布置双排螺栓,在基准材上的准距g1"));
	AddPropItem("primaryAngle.g2",PROPLIST_ITEM(id++,"g2","布置双排螺栓,在基准材上的准距g2"));
	AddPropItem("primaryAngle.bolt.d",PROPLIST_ITEM(id++,"螺栓规格","主材角钢塔脚连接端的螺栓直径","M12|M16|M20|M24"));
	AddPropItem("primaryAngle.bolt.num",PROPLIST_ITEM(id++,"螺栓数量","主材角钢塔脚连接端的单肢连接数量"));
	AddPropItem("primaryAngle.bolt.rows",PROPLIST_ITEM(id++,"螺栓排数","主材角钢塔脚连接端的螺栓排数","0.单排|1.双排"));
	AddPropItem("primaryAngle.bolt.layout",PROPLIST_ITEM(id++,"布孔方式","沿主材角钢射线方向看螺栓的布置方式","0.左高右低|1.左低右高"));
	AddPropItem("primaryAngle.bolt.EndSpace",PROPLIST_ITEM(id++,"螺栓端距",""));
	AddPropItem("primaryAngle.bolt.BoltSpace",PROPLIST_ITEM(id++,"螺栓间距",""));
	AddPropItem("primaryAngleDown",PROPLIST_ITEM(id++,"下侧主材角钢","牛蹄板下侧的连接主材角钢"));
	AddPropItem("primaryAngleDown.oddment",PROPLIST_ITEM(id++,"连接端负头","根据碰撞间隙实时计算的连接端负头"));
	AddPropItem("primaryAngleDown.ZhunJu",PROPLIST_ITEM(id++,"布孔准距","螺栓在基准材上的布置准距"));
	AddPropItem("primaryAngleDown.g",PROPLIST_ITEM(id++,"g","布置单排螺栓,在基准材上的准距g"));
	AddPropItem("primaryAngleDown.g1",PROPLIST_ITEM(id++,"g1","布置双排螺栓,在基准材上的准距g1"));
	AddPropItem("primaryAngleDown.g2",PROPLIST_ITEM(id++,"g2","布置双排螺栓,在基准材上的准距g2"));
	AddPropItem("primaryAngleDown.bolt.d",PROPLIST_ITEM(id++,"螺栓规格","主材角钢塔脚连接端的螺栓直径","M12|M16|M20|M24"));
	AddPropItem("primaryAngleDown.bolt.num",PROPLIST_ITEM(id++,"螺栓数量","主材角钢塔脚连接端的单肢连接数量"));
	AddPropItem("primaryAngleDown.bolt.rows",PROPLIST_ITEM(id++,"螺栓排数","主材角钢塔脚连接端的螺栓排数","0.单排|1.双排"));
	AddPropItem("primaryAngleDown.bolt.layout",PROPLIST_ITEM(id++,"布孔方式","沿主材角钢射线方向看螺栓的布置方式","0.左高右低|1.左低右高"));
	AddPropItem("primaryAngleDown.bolt.EndSpace",PROPLIST_ITEM(id++,"螺栓端距",""));
	AddPropItem("primaryAngleDown.bolt.BoltSpace",PROPLIST_ITEM(id++,"螺栓间距",""));
	//底板信息
	AddPropItem("basePlateInfo",PROPLIST_ITEM(id++,"底板参数","底板设计的参数"));
	AddPropItem("base.sPartNo",PROPLIST_ITEM(id++,"编号","底板编号"));
	AddPropItem("base.cMaterial",PROPLIST_ITEM(id++,"材质","底板的材质",MakeMaterialMarkSetString()));
	AddPropItem("base.shape",PROPLIST_ITEM(id++,"外形类别","底板的大致外形形状","0.方形|1.圆形"));
	AddPropItem("base.thick",PROPLIST_ITEM(id++,"底座厚度","底板厚度"));
	AddPropItem("base.nNormOffsetType",PROPLIST_ITEM(id++,"法向偏移类型","法向偏移类型","0.上平面|1.下平面|2.指定偏移"));
	AddPropItem("base.nNormOffset",PROPLIST_ITEM(id++,"法向偏移量","自装配定位原点向基准平面的法向偏移量"));
	AddPropItem("base.origin",PROPLIST_ITEM(id++,"定位原点","底板的装配坐标系的原点位置"));
	AddPropItem("base.origin.x",PROPLIST_ITEM(id++,"X坐标分量","底板的装配坐标系的原点位置X坐标"));
	AddPropItem("base.origin.y",PROPLIST_ITEM(id++,"Y坐标分量","底板的装配坐标系的原点位置Y坐标"));
	AddPropItem("base.origin.z",PROPLIST_ITEM(id++,"Z坐标分量","底板的装配坐标系的原点位置Z坐标"));
	AddPropItem("base.worknorm",PROPLIST_ITEM(id++,"底板法线方向","底板的装配坐标系的法线方向"));
	AddPropItem("base.worknorm.x",PROPLIST_ITEM(id++,"X坐标分量","底板的装配坐标系的法线方向X坐标"));
	AddPropItem("base.worknorm.y",PROPLIST_ITEM(id++,"Y坐标分量","底板的装配坐标系的法线方向Y坐标"));
	AddPropItem("base.worknorm.z",PROPLIST_ITEM(id++,"Z坐标分量","底板的装配坐标系的法线方向Z坐标"));
	AddPropItem("base.R",PROPLIST_ITEM(id++,"底板半径","底板半径"));
	AddPropItem("base.XA",PROPLIST_ITEM(id++,"X+向宽","底板水平正向尺寸"));
	AddPropItem("base.XB",PROPLIST_ITEM(id++,"X-向宽","底板水平负向尺寸"));
	AddPropItem("base.XC0",PROPLIST_ITEM(id++,"Ⅳ象限X向切角","第四象限水平切割长度"));
	AddPropItem("base.XC2",PROPLIST_ITEM(id++,"Ｉ象限X向切角","第一象限水平切割长度"));
	AddPropItem("base.XC1",PROPLIST_ITEM(id++,"Ⅲ象限X向切角","第三象限水平切割长度"));
	AddPropItem("base.XC3",PROPLIST_ITEM(id++,"Ⅱ象限X向切角","第二象限水平切割长度"));
	AddPropItem("base.YA",PROPLIST_ITEM(id++,"Y+向宽","底板竖直正向尺寸"));
	AddPropItem("base.YB",PROPLIST_ITEM(id++,"Y-向宽","底板竖直负向尺寸"));
	AddPropItem("base.YC0",PROPLIST_ITEM(id++,"Ⅳ象限Y向切角","第四象限竖直切割长度"));
	AddPropItem("base.YC2",PROPLIST_ITEM(id++,"Ｉ象限Y向切角","第一象限竖直切割长度"));
	AddPropItem("base.YC1",PROPLIST_ITEM(id++,"Ⅲ象限Y向切角","第三象限竖直切割长度"));
	AddPropItem("base.YC3",PROPLIST_ITEM(id++,"Ⅱ象限Y向切角","第二象限竖直切割长度"));
	//靴板信息
	AddPropItem("upLinkSP",PROPLIST_ITEM(id++,"上侧靴板参数","上侧靴板设计的参数"));
	AddPropItem("upLinkSP.A",PROPLIST_ITEM(id++,"外侧台阶高","靴板外侧台阶高度"));
	AddPropItem("upLinkSP.B",PROPLIST_ITEM(id++,"顶部外侧余量","靴板顶部外侧余量"));
	AddPropItem("upLinkSP.C",PROPLIST_ITEM(id++,"内侧台阶高","靴板内侧台阶高度"));
	AddPropItem("upLinkSP.D",PROPLIST_ITEM(id++,"顶部内侧余量","靴板顶部外侧余量"));
	AddPropItem("upLinkSP.E",PROPLIST_ITEM(id++,"斜材楞线侧余量","靴板在斜材楞线侧的延伸量"));
	AddPropItem("upLinkSP.F",PROPLIST_ITEM(id++,"斜材肢翼侧余量","靴板在斜材肢翼侧的延伸量"));
	AddPropItem("upLinkSP.M",PROPLIST_ITEM(id++,"靴板延伸余量","靴板延伸至底板边缘时的间隙值M==0表示延伸至底板边缘"));
	AddPropItem("upLinkSP.AL",PROPLIST_ITEM(id++,"靴板角钢方向延伸量","对焊靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算"));
	AddPropItem("upLinkSP.CM",PROPLIST_ITEM(id++,"靴板倒角模式","","0.切角|1.切弧"));
	AddPropItem("upLinkSP.style_bolt0_weld1",PROPLIST_ITEM(id++,"连接方式","靴板与主材角钢间的连接方式","0.螺栓连接|1.对焊|2.搭焊"));
	AddPropItem("upLinkSP.m_bBreakXueBan0C",PROPLIST_ITEM(id++,"打断X轴向上靴板","打断X轴向上靴板","0.贯通底板|1.中间分断"));
	AddPropItem("upLinkSP.m_bBreakXueBan90C",PROPLIST_ITEM(id++,"打断Y轴向上靴板","打断Y轴向上靴板","0.贯通底板|1.中间分断"));
	AddPropItem("downLinkSP",PROPLIST_ITEM(id++,"下侧靴板参数","下侧靴板设计的参数"));
	AddPropItem("downLinkSP.A",PROPLIST_ITEM(id++,"外侧台阶高",""));
	AddPropItem("downLinkSP.B",PROPLIST_ITEM(id++,"顶部外侧余量","顶部前侧余量"));
	AddPropItem("downLinkSP.C",PROPLIST_ITEM(id++,"内侧台阶高","靴板内侧台阶高度"));
	AddPropItem("downLinkSP.D",PROPLIST_ITEM(id++,"顶部内侧余量","靴板顶部外侧余量"));
	AddPropItem("downLinkSP.E",PROPLIST_ITEM(id++,"斜材楞线侧余量","靴板在斜材楞线侧的延伸量"));
	AddPropItem("downLinkSP.F",PROPLIST_ITEM(id++,"斜材肢翼侧余量","靴板在斜材肢翼侧的延伸量"));
	AddPropItem("downLinkSP.M",PROPLIST_ITEM(id++,"靴板延伸余量","靴板延伸至底板边缘时的间隙值M==0表示延伸至底板边缘"));
	AddPropItem("downLinkSP.AL",PROPLIST_ITEM(id++,"靴板角钢方向延伸量","对焊靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算"));
	AddPropItem("downLinkSP.CM",PROPLIST_ITEM(id++,"靴板倒角模式","","0.切角|1.切弧"));
	AddPropItem("downLinkSP.style_bolt0_weld1",PROPLIST_ITEM(id++,"连接方式","靴板与主材角钢间的连接方式","0.螺栓连接|1.对焊|2.搭焊"));
	AddPropItem("downLinkSP.m_bBreakXueBan0C",PROPLIST_ITEM(id++,"打断X轴向上靴板","打断X轴向上靴板","0.贯通底板|1.中间分断"));
	AddPropItem("downLinkSP.m_bBreakXueBan90C",PROPLIST_ITEM(id++,"打断Y轴向上靴板","打断Y轴向上靴板","0.贯通底板|1.中间分断"));
	//肋板信息
	AddPropItem("ribPlateInfo",PROPLIST_ITEM(id++,"肋板参数","肋板设计的参数"));
	AddPropItem("RibPlate.Serial",PROPLIST_ITEM(id++,"标识序号","当前模板肋板集合中肋板的标识序号"));
	AddPropItem("RibPlate.cMaterial",PROPLIST_ITEM(id++,"材质","肋板材质",MakeMaterialMarkSetString()));
	AddPropItem("RibPlate.PartNo",PROPLIST_ITEM(id++,"编号","肋板编号"));
	AddPropItem("RibPlate.origin",PROPLIST_ITEM(id++,"定位基点","肋板定位基点"));
	AddPropItem("RibPlate.OX",PROPLIST_ITEM(id++,"X","定位基点X坐标"));
	AddPropItem("RibPlate.OY",PROPLIST_ITEM(id++,"Y","定位基点Y坐标"));
	AddPropItem("RibPlate.axisXType",PROPLIST_ITEM(id++,"焊缝走向","肋板与底板焊缝的走向(即肋板X轴向)定位方式","0.指定角度|1.X+|2.X-|3.Y+|4.Y-|5.指定垂线角度"));
	AddPropItem("RibPlate.axisX_x",PROPLIST_ITEM(id++,"X坐标","X坐标"));
	AddPropItem("RibPlate.axisX_y",PROPLIST_ITEM(id++,"Y坐标","Y坐标"));
	AddPropItem("RibPlate.angle",PROPLIST_ITEM(id++,"射线角度","指定射线角度"));
	AddPropItem("RibPlate.fRayDist",PROPLIST_ITEM(id++,"射线距离","指定射线距离"));
	AddPropItem("RibPlate.thick",PROPLIST_ITEM(id++,"厚度","肋板厚度"));
	AddPropItem("RibPlate.upper",PROPLIST_ITEM(id++,"位于上侧","肋板位于底板的上侧或下侧","上侧|下侧"));
	AddPropItem("RibPlate.A",PROPLIST_ITEM(id++,"高度A","A参数，高度"));
	AddPropItem("RibPlate.B",PROPLIST_ITEM(id++,"宽度B","B参数，底边宽度"));
	AddPropItem("RibPlate.C",PROPLIST_ITEM(id++,"台阶高度C","C参数，台阶高度"));
	AddPropItem("RibPlate.D",PROPLIST_ITEM(id++,"顶部宽度D","D参数，顶部宽度"));
	AddPropItem("RibPlate.iCutMode",PROPLIST_ITEM(id++,"倒角模式","倒角模式","0.切角|1.切弧"));
	AddPropItem("RibPlate.fCutAngleWidth",PROPLIST_ITEM(id++,"切角宽度E","E参数，肋板内侧切角宽度"));
	AddPropItem("RibPlate.fGradientAngle",PROPLIST_ITEM(id++,"倾斜角度","肋板倾斜角度"));
	AddPropItem("RibPlate.MH",PROPLIST_ITEM(id++,"孔径H","工艺孔的孔径,mm"));
	AddPropItem("RibPlate.MF",PROPLIST_ITEM(id++,"孔径间隙F","工艺孔到底边的间隙,mm"));
	AddPropItem("RibPlate.MG",PROPLIST_ITEM(id++,"孔径间隙G","工艺孔到底边的间隙,mm"));
	AddPropItem("RibPlate.slotW",PROPLIST_ITEM(id++,"流水槽宽","流水槽宽"));
#endif
}

CXhChar100 PointToString(const f3dPoint &point,bool b3dPt=TRUE);
int CFootDesignTemplDlg::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)	//根据属性ID得到属性值字符串
{
	CXhChar100 sText;
	if(id==GetPropID("m_hBaseNode"))	//依赖节点
		sText.Printf("0X%X",foottempl.m_hBaseNode);
	else if(id==GetPropID("upLinkSP.A"))	//靴板台阶高
		sText.Printf("%d",foottempl.upLinkSP.A);
	else if(id==GetPropID("downLinkSP.A"))
		sText.Printf("%d",foottempl.downLinkSP.A);
	else if(id==GetPropID("upLinkSP.B"))	//靴板顶部余量
		sText.Printf("%d",foottempl.upLinkSP.B);
	else if(id==GetPropID("downLinkSP.B"))
		sText.Printf("%d",foottempl.downLinkSP.B);
	else if(id==GetPropID("upLinkSP.C"))
		sText.Printf("%d",foottempl.upLinkSP.C);
	else if(id==GetPropID("downLinkSP.C"))
		sText.Printf("%d",foottempl.downLinkSP.C);
	else if(id==GetPropID("upLinkSP.D"))
		sText.Printf("%d",foottempl.upLinkSP.D);
	else if(id==GetPropID("downLinkSP.D"))
		sText.Printf("%d",foottempl.downLinkSP.D);
	else if(id==GetPropID("upLinkSP.E"))
		sText.Printf("%d",foottempl.upLinkSP.E);
	else if(id==GetPropID("downLinkSP.E"))
		sText.Printf("%d",foottempl.downLinkSP.E);
	else if(id==GetPropID("upLinkSP.F"))
		sText.Printf("%d",foottempl.upLinkSP.F);
	else if(id==GetPropID("downLinkSP.F"))
		sText.Printf("%d",foottempl.downLinkSP.F);
	else if(id==GetPropID("upLinkSP.M"))	//靴板延伸至底板边缘时的间隙值M==0表示延伸至底板边缘
		sText.Printf("%d",foottempl.upLinkSP.M);
	else if(id==GetPropID("downLinkSP.M"))
		sText.Printf("%d",foottempl.downLinkSP.M);
	else if(id==GetPropID("upLinkSP.AL"))//靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算
		sText.Printf("%d",foottempl.upLinkSP.AL);
	else if(id==GetPropID("downLinkSP.AL"))
		sText.Printf("%d",foottempl.downLinkSP.AL);
	else if(id==GetPropID("C"))	//角钢端楞点到底板距离(碰撞间隙)
		sText.Printf("%d",foottempl.C);
	else if(id==GetPropID("upLinkSP.CM"))
	{
		if(foottempl.upLinkSP.CM==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Cut Angle");
		else
			sText.Copy("1.Cut Arc");
#else
			sText.Copy("0.切角");
		else
			sText.Copy("1.切弧");
#endif
	}
	else if(id==GetPropID("downLinkSP.CM"))
	{
		if(foottempl.downLinkSP.CM==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Cut Angle");
		else
			sText.Copy("1.Cut Arc");
#else
			sText.Copy("0.切角");
		else
			sText.Copy("1.切弧");
#endif
	}
	else if(id==GetPropID("m_bWeldDiZuo"))	//连接主材角钢焊接到底座
	{
		if(foottempl.m_bWeldDiZuo)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("Yes");
		else
			sText.Copy("No");
#else
			sText.Copy("是");
		else
			sText.Copy("否");
#endif
	}
	else if(id==GetPropID("primaryAngle"))	//连接主材角钢焊接到底座
	{
		if(foottempl.primaryAngle.pAngle)
			sText.Printf("0X%X",foottempl.primaryAngle.pAngle->handle);
		else
			sText.Copy("0X0");
	}
	else if(id==GetPropID("primaryAngle.oddment"))	//主材角钢连接端正负头
		sText.Printf("%g",foottempl.primaryAngle.oddment);
	else if(id==GetPropID("primaryAngle.bolt.d"))	//塔脚的上连接主材角钢
		sText.Printf("M%d",foottempl.primaryAngle.bolt.d);
	else if(id==GetPropID("primaryAngle.bolt.num"))	//螺栓数量
		sText.Printf("%d",foottempl.primaryAngle.bolt.num);
	else if(id==GetPropID("primaryAngle.bolt.EndSpace"))	//螺栓端距
		sText.Printf("%d",foottempl.primaryAngle.bolt.siEndSpace);
	else if(id==GetPropID("primaryAngle.bolt.BoltSpace"))	//螺栓间距
		sText.Printf("%d",foottempl.primaryAngle.bolt.siBoltSpace);
	else if(id==GetPropID("primaryAngle.bolt.rows"))	//螺栓排数
	{
		if(foottempl.primaryAngle.bolt.rows==2)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Double");
		else
			sText.Copy("0.Singular");
#else
			sText.Copy("1.双排");
		else
			sText.Copy("0.单排");
#endif
	}
	else if(id==GetPropID("primaryAngle.bolt.layout"))	//布孔方式
	{
		if(foottempl.primaryAngle.bolt.layout==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Left high Right low");
		else
			sText.Copy("1.Left low Right high");
#else
			sText.Copy("0.左高右低");
		else
			sText.Copy("1.左低右高");
#endif
	}
	else if(id==GetPropID("primaryAngle.ZhunJu"))
		sText.Printf("{g=%d,%d,%d}",foottempl.primaryAngle.g,foottempl.primaryAngle.g1,foottempl.primaryAngle.g2);
	else if(id==GetPropID("primaryAngle.g"))
		sText.Printf("%d",foottempl.primaryAngle.g);
	else if(id==GetPropID("primaryAngle.g1"))
		sText.Printf("%d",foottempl.primaryAngle.g1);
	else if(id==GetPropID("primaryAngle.g2"))
		sText.Printf("%d",foottempl.primaryAngle.g2);
	else if(id==GetPropID("primaryAngleDown"))	//连接主材角钢焊接到底座
	{
		if(foottempl.primaryAngleDown.pAngle)
			sText.Printf("0X%X",foottempl.primaryAngleDown.pAngle->handle);
		else
			sText.Copy("0X0");
	}
	else if(id==GetPropID("primaryAngleDown.oddment"))	//主材角钢连接端正负头
		sText.Printf("%g",foottempl.primaryAngleDown.oddment);
	else if(id==GetPropID("primaryAngleDown.bolt.d"))	//牛蹄板下侧的连接主材角钢
		sText.Printf("M%d",foottempl.primaryAngleDown.bolt.d);
	else if(id==GetPropID("primaryAngleDown.bolt.num"))	//螺栓数量
		sText.Printf("%d",foottempl.primaryAngleDown.bolt.num);
	else if(id==GetPropID("primaryAngleDown.bolt.EndSpace"))	//螺栓端距
		sText.Printf("%d",foottempl.primaryAngleDown.bolt.siEndSpace);
	else if(id==GetPropID("primaryAngleDown.bolt.BoltSpace"))	//螺栓间距
		sText.Printf("%d",foottempl.primaryAngleDown.bolt.siBoltSpace);
	else if(id==GetPropID("primaryAngleDown.bolt.rows"))//螺栓排数
	{
		if(foottempl.primaryAngleDown.bolt.rows==2)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Double");
		else
			sText.Copy("0.Singular");
#else
			sText.Copy("1.双排");
		else
			sText.Copy("0.单排");
#endif
	}
	else if(id==GetPropID("primaryAngleDown.bolt.layout"))	//布孔方式
	{
		if(foottempl.primaryAngleDown.bolt.layout==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Left high Right low");
		else
			sText.Copy("1.Left low Right high");
#else
			sText.Copy("0.左高右低");
		else
			sText.Copy("1.左低右高");
#endif
	}
	else if(id==GetPropID("primaryAngleDown.ZhunJu"))
		sText.Printf("{g=%d,%d,%d}",foottempl.primaryAngleDown.g,foottempl.primaryAngleDown.g1,foottempl.primaryAngleDown.g2);
	else if(id==GetPropID("primaryAngleDown.g"))
		sText.Printf("%d",foottempl.primaryAngleDown.g);
	else if(id==GetPropID("primaryAngleDown.g1"))
		sText.Printf("%d",foottempl.primaryAngleDown.g1);
	else if(id==GetPropID("primaryAngleDown.g2"))
		sText.Printf("%d",foottempl.primaryAngleDown.g2);
	else if(id==GetPropID("base.sPartNo"))	//底板编号
		sText.Copy(foottempl.base.sPartNo);
	else if(id==GetPropID("base.cMaterial"))//底板材质
	{
		STEELMAT *pSteelMat=QuerySteelMatByBriefMark(foottempl.base.cMaterial);
		if(pSteelMat)
			sText.Copy(pSteelMat->mark);
	}
	else if(id==GetPropID("base.shape"))//底板外形类别
	{
		if(foottempl.base.shape==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Circle");
		else
			sText.Copy("0.Rectangle");
#else
			sText.Copy("1.圆形");
		else
			sText.Copy("0.方形");
#endif
	}
	else if(id==GetPropID("base.thick"))//底板板厚
		sText.Printf("%d",foottempl.base.thick);
	else if(id==GetPropID("base.nNormOffsetType"))//基准平面类别
	{
		if(foottempl.base.cNormOffsetType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Upper plane");
		else if(foottempl.base.cNormOffsetType==1)
			sText.Copy("1.Under plane");
		else //if(foottempl.base.cNormOffsetType==1)
			sText.Copy("2.Specified offset");
#else
			sText.Copy("0.上平面");
		else if(foottempl.base.cNormOffsetType==1)
			sText.Copy("1.下平面");
		else //if(foottempl.base.cNormOffsetType==1)
			sText.Copy("2.指定偏移");
#endif
	}
	else if(id==GetPropID("base.nNormOffset"))//底板法向偏移
		sText.Printf("%d",foottempl.base.nNormOffset);
	else if(id==GetPropID("base.origin"))//底板定位原点
		sText.ConvertFromPoint(foottempl.origin.Position());
	/*else if(id==GetPropID("base.origin"))//底板材质
	{
		if(foottempl.origin.datum_pos_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Manual Input");
		else if(foottempl.origin.datum_pos_style==1)
			sText.Copy("1.Angle Ridge Vertex");
		else if(foottempl.origin.datum_pos_style==2)
			sText.Copy("2.Rod Node Position");
		else if(foottempl.origin.datum_pos_style==3)
			sText.Copy("3.Bolt line Intersection Point");
		else if(foottempl.origin.datum_pos_style==4)
			sText.Copy("4.Datum Line Intersection");
		else if(foottempl.origin.datum_pos_style==5)
			sText.Copy("5.Coord. Specified On Datum Line");
		else if(foottempl.origin.datum_pos_style==6)
			sText.Copy("6.Coord. Specified  On Datum Plane");
		else if(foottempl.origin.datum_pos_style==7)
			sText.Copy("7.angle ridge offset");
		else if(foottempl.origin.datum_pos_style==8)
			sText.Copy("8.Same Position With Node");
		else if(foottempl.origin.datum_pos_style==9)
			sText.Copy("9.Relative Coordinate On Datum Part");
#else 
			sText.Copy("0.手动输入");
		else if(foottempl.origin.datum_pos_style==1)
			sText.Copy("1.角钢楞点定位点");
		else if(foottempl.origin.datum_pos_style==2)
			sText.Copy("2.杆件节点定位点");
		else if(foottempl.origin.datum_pos_style==3)
			sText.Copy("3.角钢心线交点");
		else if(foottempl.origin.datum_pos_style==4)
			sText.Copy("4.基准线空间交点");
		else if(foottempl.origin.datum_pos_style==5)
			sText.Copy("5.基准线上指定坐标分量点");
		else if(foottempl.origin.datum_pos_style==6)
			sText.Copy("6.基准面上指定轴向点");
		else if(foottempl.origin.datum_pos_style==7)
			sText.Copy("7.沿肢法向偏移的包铁楞点");
		else if(foottempl.origin.datum_pos_style==8)
			sText.Copy("8.与节点同位");
		else if(foottempl.origin.datum_pos_style==9)
			sText.Copy("9.基准构件上的相对坐标");
#endif
	}*/
	else if(id==GetPropID("base.origin.x"))//底板定位原点X分量
		sText=CXhChar16(foottempl.origin.Position().x);
	else if(id==GetPropID("base.origin.y"))//底板定位原点Y分量
		sText=CXhChar16(foottempl.origin.Position().y);
	else if(id==GetPropID("base.origin.z"))//底板定位原点Z分量
		sText=CXhChar16(foottempl.origin.Position().z);
	else if(id==GetPropID("base.worknorm"))//底板定位原点X分量
		sText.Copy(PointToString(foottempl.axis_z));
	else if(id==GetPropID("base.worknorm.x"))//底板定位原点X分量
		sText=CXhChar16(foottempl.axis_z.x);
	else if(id==GetPropID("base.worknorm.y"))//底板定位原点Y分量
		sText=CXhChar16(foottempl.axis_z.y);
	else if(id==GetPropID("base.worknorm.z"))//底板定位原点Z分量
		sText=CXhChar16(foottempl.axis_z.z);
	else if(id==GetPropID("base.XA"))//X+向宽
		sText.Printf("%d",foottempl.base.XA);
	else if(id==GetPropID("base.XB"))//X-向宽
		sText.Printf("%d",foottempl.base.XB);
	else if(id==GetPropID("base.YA"))//Y+向宽
		sText.Printf("%d",foottempl.base.YA);
	else if(id==GetPropID("base.YB"))//Y-向宽
		sText.Printf("%d",foottempl.base.YB);
	else if(id==GetPropID("base.XC0"))//X+Y-侧X向倒角
		sText.Printf("%d",foottempl.base.XC[0]);
	else if(id==GetPropID("base.XC1"))//X-Y-侧X向倒角
		sText.Printf("%d",foottempl.base.XC[1]);
	else if(id==GetPropID("base.XC2"))//X+Y+侧X向倒角
		sText.Printf("%d",foottempl.base.XC[2]);
	else if(id==GetPropID("base.XC3"))//X-Y+侧X向倒角
		sText.Printf("%d",foottempl.base.XC[3]);
	else if(id==GetPropID("base.YC0"))//X+Y-侧Y向倒角
		sText.Printf("%d",foottempl.base.YC[0]);
	else if(id==GetPropID("base.YC1"))//X-Y-侧Y向倒角
		sText.Printf("%d",foottempl.base.YC[1]);
	else if(id==GetPropID("base.YC2"))//X+Y+侧Y向倒角
		sText.Printf("%d",foottempl.base.YC[2]);
	else if(id==GetPropID("base.YC3"))//X-Y+侧Y向倒角
		sText.Printf("%d",foottempl.base.YC[3]);
	else if(id==GetPropID("base.R"))
		sText.Printf("%g",foottempl.base.fR);
	else if(id==GetPropID("upLinkSP.style_bolt0_weld1"))//打断X轴向靴板
	{	//连接方式
		if(foottempl.upLinkSP.style_bolt0_weld1==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Bolt connect");
		else if(foottempl.upLinkSP.style_bolt0_weld1==1)
			sText.Copy("1.Butt Welding");
		else
			sText.Copy("2.Lap Welding");
#else
			sText.Copy("0.螺栓连接");
		else if(foottempl.upLinkSP.style_bolt0_weld1==1)
			sText.Copy("1.对焊");
		else
			sText.Copy("2.搭焊");
#endif
	}
	else if(id==GetPropID("downLinkSP.style_bolt0_weld1"))
	{
		if(foottempl.downLinkSP.style_bolt0_weld1==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Bolt connect");
		else if(foottempl.downLinkSP.style_bolt0_weld1==1)
			sText.Copy("1.Butt Welding");
		else
			sText.Copy("2.Lap Welding");
#else
			sText.Copy("0.螺栓连接");
		else if(foottempl.downLinkSP.style_bolt0_weld1==1)
			sText.Copy("1.对焊");
		else
			sText.Copy("2.搭焊");
#endif
	}
	else if(id==GetPropID("upLinkSP.m_bBreakXueBan0C"))//打断X轴向靴板
	{
		if(foottempl.upLinkSP.m_bBreakXueBan0C)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Break");
		else
			sText.Copy("0.Across");
#else
			sText.Copy("1.中间分断");
		else
			sText.Copy("0.贯通底板");
#endif
	}
	else if(id==GetPropID("downLinkSP.m_bBreakXueBan0C"))//打断X轴向靴板
	{
		if(foottempl.downLinkSP.m_bBreakXueBan0C)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Break");
		else
			sText.Copy("0.Across");
#else
			sText.Copy("1.中间分断");
		else
			sText.Copy("0.贯通底板");
#endif
	}
	else if(id==GetPropID("upLinkSP.m_bBreakXueBan90C"))//打断Y轴向靴板
	{
		if(foottempl.upLinkSP.m_bBreakXueBan90C)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Break");
		else
			sText.Copy("0.Across");
#else
			sText.Copy("1.中间分断");
		else
			sText.Copy("0.贯通底板");
#endif
	}
	else if(id==GetPropID("downLinkSP.m_bBreakXueBan90C"))//打断Y轴向靴板
	{
		if(foottempl.downLinkSP.m_bBreakXueBan90C)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Break");
		else
			sText.Copy("0.Across");
#else
			sText.Copy("1.中间分断");
		else
			sText.Copy("0.贯通底板");
#endif
	}
	else if(id==GetPropID("RibPlate.Serial"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->Serial());
	}
	else if(id==GetPropID("RibPlate.cMaterial"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		STEELMAT *pSteelMat=QuerySteelMatByBriefMark(ActiveRibPlate()->cMaterial);
		if(pSteelMat)
			sText.Copy(pSteelMat->mark);
	}
	else if(id==GetPropID("RibPlate.PartNo"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Copy(ActiveRibPlate()->sPartNo);
	}
	else if(id==GetPropID("RibPlate.OX"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%g",ActiveRibPlate()->pos_para.PARA1.OX);
	}
	else if(id==GetPropID("RibPlate.OY"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%g",ActiveRibPlate()->pos_para.PARA1.OY);
	}
	else if(id==GetPropID("RibPlate.axisXType"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(ActiveRibPlate()->axisXType==0)
			strcpy(sText,"0.Specified angle");
		else if(ActiveRibPlate()->axisXType==5)
			strcpy(sText,"5.Specified vertical line angle");
#else
		if(ActiveRibPlate()->axisXType==0)
			strcpy(sText,"0.指定角度");
		else if(ActiveRibPlate()->axisXType==5)
			strcpy(sText,"5.指定垂线角度");
#endif
		else if(ActiveRibPlate()->axisXType==1)
			strcpy(sText,"1.X+");
		else if(ActiveRibPlate()->axisXType==2)
			strcpy(sText,"2.X-");
		else if(ActiveRibPlate()->axisXType==3)
			strcpy(sText,"3.Y+");
		else if(ActiveRibPlate()->axisXType==4)
			strcpy(sText,"4.Y-");
	}
	else if(id==GetPropID("RibPlate.angle"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		if(ActiveRibPlate()->axisXType==0)
			sText.Printf("%f",ActiveRibPlate()->pos_para.PARA2.angle);
		else if(ActiveRibPlate()->axisXType==5)
			sText.Printf("%f",ActiveRibPlate()->pos_para.PARA3.angle);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("RibPlate.fRayDist"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%g",ActiveRibPlate()->pos_para.PARA3.fRayDist);
	}
	else if(id==GetPropID("RibPlate.axisX_x"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%f",ActiveRibPlate()->pos_para.PARA1.axisX_x);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("RibPlate.axisX_y"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%f",ActiveRibPlate()->pos_para.PARA1.axisX_y);
		SimplifiedNumString(sText);
	}
	else if(id==GetPropID("RibPlate.thick"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->thick);
	}
	else if(id==GetPropID("RibPlate.upper"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		if(ActiveRibPlate()->upper)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("upside");
		else
			sText.Copy("downside");
#else
			sText.Copy("上侧");
		else
			sText.Copy("下侧");
#endif
	}
	else if(id==GetPropID("RibPlate.A"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->A);
	}
	else if(id==GetPropID("RibPlate.B"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->B);
	}
	else if(id==GetPropID("RibPlate.C"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->C);
	}
	else if(id==GetPropID("RibPlate.D"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->D);
	}
	else if(id==GetPropID("RibPlate.slotW"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->water_slot_wide);
	}
	else if(id==GetPropID("RibPlate.iCutMode"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		if(ActiveRibPlate()->iCutMode==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Cut Angle");
		else
			sText.Copy("1.Cut Arc");
#else
			sText.Copy("0.切角");
		else
			sText.Copy("1.切弧");
#endif
	}
	else if(id==GetPropID("RibPlate.fCutAngleWidth"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%g",ActiveRibPlate()->fCutAngleWidth);
	}
	else if(id==GetPropID("RibPlate.fGradientAngle"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%g",ActiveRibPlate()->fGradientAngle);
	}
	else if(id==GetPropID("RibPlate.MH"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->MH);
	}
	else if(id==GetPropID("RibPlate.MF"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->MF);
	}
	else if(id==GetPropID("RibPlate.MG"))
	{
		if(ActiveRibPlate()==NULL)
			return 0;
		sText.Printf("%d",ActiveRibPlate()->MG);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
void UpdateLineAngleOddPropItem(CPropertyList *pPropList)
{
	CFootDesignTemplDlg* pTemplDlg=(CFootDesignTemplDlg*)pPropList->GetParent();
	CPropTreeItem* pFindItem=NULL;
	CXhChar50 sText;
	if(pTemplDlg->foottempl.primaryAngle.pAngle)
	{
		pTemplDlg->foottempl.UpdateLinkAngleOddment(pTemplDlg->foottempl.primaryAngle,true);
		pFindItem=pPropList->FindItemByPropId(CFootDesignTemplDlg::GetPropID("primaryAngle.oddment"),NULL);
		if(pFindItem)
		{
			pTemplDlg->GetPropValueStr(pFindItem->m_idProp,sText);
			pPropList->SetItemPropValue(pFindItem->m_idProp,sText);
			pFindItem->SetReadOnly(pTemplDlg->foottempl.m_bWeldDiZuo);
		}
	}
	if(pTemplDlg->foottempl.primaryAngleDown.pAngle)
	{
		pTemplDlg->foottempl.UpdateLinkAngleOddment(pTemplDlg->foottempl.primaryAngleDown,false);
		pFindItem=pPropList->FindItemByPropId(CFootDesignTemplDlg::GetPropID("primaryAngleDown.oddment"),NULL);
		if(pFindItem)
		{
			pTemplDlg->GetPropValueStr(pFindItem->m_idProp,sText);
			pPropList->SetItemPropValue(pFindItem->m_idProp,sText);
			pFindItem->SetReadOnly(pTemplDlg->foottempl.m_bWeldDiZuo);
		}
	}
}
BOOL ModifyDesignFootProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CFootDesignTemplDlg* pTemplDlg=(CFootDesignTemplDlg*)pPropList->GetParent();
	CPropertyListOper<CFootDesignTemplDlg> oper(pPropList,pTemplDlg);
	CXhChar100 tem_str,sText;
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	GEPOINT datumOrg=pTemplDlg->foottempl.origin.Position();
	if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("m_bWeldDiZuo"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("Yes")==0)
#else
		if(valueStr.Compare("是")==0)
#endif
			pTemplDlg->foottempl.m_bWeldDiZuo=TRUE;
		else
			pTemplDlg->foottempl.m_bWeldDiZuo=FALSE;
		pPropList->DeleteItemByPropId(CFootDesignTemplDlg::GetPropID("C"));
		if(!pTemplDlg->foottempl.m_bWeldDiZuo)
			oper.InsertEditPropItem(pItem,"C","","",-1,TRUE);
		//更新正负头
		UpdateLineAngleOddPropItem(pPropList);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("C"))
	{
		if(atoi(tem_str)<0)
		{
			AfxMessageBox("碰撞间隙不可为负值!");
			return FALSE;
		}
		pTemplDlg->foottempl.C=atoi(tem_str);
		//更新正负头
		UpdateLineAngleOddPropItem(pPropList);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle"))
	{
		long handle=HexStrToLong(tem_str);
		CLDSLineAngle* pAngle=(CLDSLineAngle*)console.FromPartHandle(handle,CLS_LINEANGLE);
		pTemplDlg->foottempl.primaryAngle.pAngle=pAngle;
		//更新子属性
		pPropList->DeleteAllSonItems(pItem);
		if(pTemplDlg->foottempl.primaryAngle.pAngle)
			pTemplDlg->UpdateLinkAngleProperty(pItem,TRUE);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.d"))
	{
		memmove(tem_str,tem_str+1,99);
		pTemplDlg->foottempl.primaryAngle.bolt.d=(BYTE)atol(tem_str);
		//更新螺栓端距、间距
		pTemplDlg->foottempl.UpdateBoltEndSpace(pTemplDlg->foottempl.primaryAngle);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.BoltSpace"),sText))	//间距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.BoltSpace"),sText);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.EndSpace"),sText))	//端距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.EndSpace"),sText);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.oddment"))
		pTemplDlg->foottempl.primaryAngle.oddment=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.num"))
	{
		int nBoltNum=atoi(tem_str);
		pTemplDlg->foottempl.primaryAngle.bolt.num=nBoltNum;
		if(nBoltNum>=5&&pTemplDlg->foottempl.primaryAngle.g1>0&&pTemplDlg->foottempl.primaryAngle.g2>0)
			pTemplDlg->foottempl.primaryAngle.bolt.rows=2;
		else
			pTemplDlg->foottempl.primaryAngle.bolt.rows=1;
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.rows"),tem_str))
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.rows"),tem_str);
		//更新螺栓端距、间距
		pTemplDlg->foottempl.UpdateBoltEndSpace(pTemplDlg->foottempl.primaryAngle);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.BoltSpace"),sText))	//间距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.BoltSpace"),sText);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.EndSpace"),sText))	//端距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.EndSpace"),sText);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.rows"))
	{
		pTemplDlg->foottempl.primaryAngle.bolt.rows=tem_str[0]-'0'+1;
		//更新螺栓端距、间距
		pTemplDlg->foottempl.UpdateBoltEndSpace(pTemplDlg->foottempl.primaryAngle);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.BoltSpace"),sText))	//间距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.BoltSpace"),sText);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.EndSpace"),sText))	//端距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.EndSpace"),sText);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.layout"))
		pTemplDlg->foottempl.primaryAngle.bolt.layout=tem_str[0]-'0';
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.EndSpace"))
		pTemplDlg->foottempl.primaryAngle.bolt.siEndSpace=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.bolt.BoltSpace"))
		pTemplDlg->foottempl.primaryAngle.bolt.siBoltSpace=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.g"))
		pTemplDlg->foottempl.primaryAngle.g=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.g1"))
		pTemplDlg->foottempl.primaryAngle.g1=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle.g2"))
		pTemplDlg->foottempl.primaryAngle.g2=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown"))
	{
		long handle=HexStrToLong(tem_str);
		CLDSLineAngle* pAngle=(CLDSLineAngle*)console.FromPartHandle(handle,CLS_LINEANGLE);
		pTemplDlg->foottempl.primaryAngleDown.pAngle=pAngle;
		//更新子属性
		pPropList->DeleteAllSonItems(pItem);
		if(pTemplDlg->foottempl.primaryAngleDown.pAngle)
		{
			pTemplDlg->m_bUpAndDownSide=TRUE;
			pTemplDlg->UpdateLinkAngleProperty(pItem,FALSE);
		}
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.oddment"))
		pTemplDlg->foottempl.primaryAngleDown.oddment=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.d"))
	{
		memmove(tem_str,tem_str+1,99);
		pTemplDlg->foottempl.primaryAngleDown.bolt.d=(BYTE)atol(tem_str);
		//更新螺栓端距、间距
		pTemplDlg->foottempl.UpdateBoltEndSpace(pTemplDlg->foottempl.primaryAngleDown);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.BoltSpace"),sText))	//间距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.BoltSpace"),sText);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.EndSpace"),sText))	//端距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.EndSpace"),sText);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.num"))
	{
		int nBoltNum=atoi(tem_str);
		pTemplDlg->foottempl.primaryAngleDown.bolt.num=nBoltNum;
		if(nBoltNum>=5&&pTemplDlg->foottempl.primaryAngleDown.g1>0&&pTemplDlg->foottempl.primaryAngleDown.g2>0)
			pTemplDlg->foottempl.primaryAngleDown.bolt.rows=2;
		else
			pTemplDlg->foottempl.primaryAngleDown.bolt.rows=1;
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.rows"),tem_str))
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.rows"),tem_str);
		//更新螺栓端距、间距
		pTemplDlg->foottempl.UpdateBoltEndSpace(pTemplDlg->foottempl.primaryAngleDown);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.BoltSpace"),sText))	//间距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.BoltSpace"),sText);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.EndSpace"),sText))	//端距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.EndSpace"),sText);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.rows"))
	{
		pTemplDlg->foottempl.primaryAngleDown.bolt.rows=tem_str[0]-'0'+1;
		//更新螺栓端距、间距
		pTemplDlg->foottempl.UpdateBoltEndSpace(pTemplDlg->foottempl.primaryAngleDown);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.BoltSpace"),sText))	//间距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.BoltSpace"),sText);
		if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.EndSpace"),sText))	//端距
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.EndSpace"),sText);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.layout"))
		pTemplDlg->foottempl.primaryAngleDown.bolt.layout=tem_str[0]-'0';
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.EndSpace"))
		pTemplDlg->foottempl.primaryAngleDown.bolt.siEndSpace=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.bolt.BoltSpace"))
		pTemplDlg->foottempl.primaryAngleDown.bolt.siBoltSpace=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.g"))
		pTemplDlg->foottempl.primaryAngleDown.g=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.g1"))
		pTemplDlg->foottempl.primaryAngleDown.g1=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown.g2"))
		pTemplDlg->foottempl.primaryAngleDown.g2=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.sPartNo"))
	{
		pTemplDlg->foottempl.base.sPartNo.Copy(tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(tem_str,BOMPART::PLATE);
		if(pPart)
		{
			pTemplDlg->foottempl.base.thick=ftoi(pPart->thick);
			pTemplDlg->foottempl.base.cMaterial=pPart->cMaterial;
			//
			CXhChar50 sValue;
			QuerySteelMatMark(pPart->cMaterial,sValue);
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.cMaterial"),(char*)sValue);
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.thick"),CXhChar50("%.0f",pPart->thick));
			if(pTemplDlg->foottempl.base.cNormOffsetType==0)	//上平面
			{
				pTemplDlg->foottempl.base.nNormOffset=-pTemplDlg->foottempl.base.thick;
				if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),tem_str))
					pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),tem_str);
				pPropList->SetItemReadOnly(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),TRUE);
			}
			//更新正负头
			UpdateLineAngleOddPropItem(pPropList);
		}
#endif
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.cMaterial"))
		pTemplDlg->foottempl.base.cMaterial=QuerySteelBriefMatMark(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.shape"))
	{
		int shape_type=tem_str[0]-'0';
		pTemplDlg->foottempl.base.shape=shape_type;
		pPropList->DeleteAllSonItems(pItem);
		if(shape_type==0)
		{
			oper.InsertEditPropItem(pItem,"base.XA","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"base.XB","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"base.YA","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"base.YB","","",-1,TRUE);
			CPropTreeItem* pSonItem=NULL;
			pSonItem=oper.InsertEditPropItem(pItem,"base.XC2","","",-1,TRUE);
			pSonItem->m_bHideChildren=TRUE;
			oper.InsertEditPropItem(pSonItem,"base.YC2","","",-1,TRUE);
			pSonItem=oper.InsertEditPropItem(pItem,"base.XC3","","",-1,TRUE);
			pSonItem->m_bHideChildren=TRUE;
			oper.InsertEditPropItem(pSonItem,"base.YC3","","",-1,TRUE);
			pSonItem=oper.InsertEditPropItem(pItem,"base.XC1");
			pSonItem->m_bHideChildren=TRUE;
			oper.InsertEditPropItem(pSonItem,"base.YC1","","",-1,TRUE);
			pSonItem=oper.InsertEditPropItem(pItem,"base.XC0","","",-1,TRUE);
			pSonItem->m_bHideChildren=TRUE;
			oper.InsertEditPropItem(pSonItem,"base.YC0","","",-1,TRUE);
			pTemplDlg->foottempl.listRibPlates.Empty();
		}
		else
		{
			oper.InsertEditPropItem(pItem,"base.R","","",-1,TRUE);
			pTemplDlg->foottempl.RetrieveCirularDibanRibPlates();
		}
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.thick"))
	{
		pTemplDlg->foottempl.base.thick=atoi(tem_str);
		if(pTemplDlg->foottempl.base.cNormOffsetType==0)	//上平面
		{
			pTemplDlg->foottempl.base.nNormOffset=-pTemplDlg->foottempl.base.thick;
			if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),tem_str))
				pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),tem_str);
			pPropList->SetItemReadOnly(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),TRUE);
		}
		//更新正负头
		UpdateLineAngleOddPropItem(pPropList);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.nNormOffsetType"))
	{
		BYTE cOffsetType=tem_str[0]-'0';
		pTemplDlg->foottempl.base.cNormOffsetType=tem_str[0]-'0';
		if(cOffsetType==0)	//上平面
		{
			pTemplDlg->foottempl.base.nNormOffset=-pTemplDlg->foottempl.base.thick;
			if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),tem_str))
				pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),tem_str);
			pPropList->SetItemReadOnly(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),TRUE);
		}
		else if(cOffsetType==1)	//下平面
		{
			pTemplDlg->foottempl.base.nNormOffset=0;
			if(pTemplDlg->GetPropValueStr(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),tem_str))
				pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),tem_str);
			pPropList->SetItemReadOnly(CFootDesignTemplDlg::GetPropID("base.nNormOffset"));
		}
		else	//指定
			pPropList->SetItemReadOnly(CFootDesignTemplDlg::GetPropID("base.nNormOffset"),FALSE);
		//更新正负头
		UpdateLineAngleOddPropItem(pPropList);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.nNormOffset"))
	{
		pTemplDlg->foottempl.base.nNormOffset=atoi(tem_str);
		//更新正负头
		UpdateLineAngleOddPropItem(pPropList);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.origin.x")||
			pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.origin.y")||
			pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.origin.z"))
	{
		if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.origin.x"))
			pTemplDlg->foottempl.origin.SetPositionX(atof(tem_str));
		else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.origin.y"))
			pTemplDlg->foottempl.origin.SetPositionY(atof(tem_str));
		else //if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.origin.z"))
			pTemplDlg->foottempl.origin.SetPositionZ(atof(tem_str));
		if(pTemplDlg->GetPropValueStr(pItem->m_pParent->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_pParent->m_idProp,sText);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm.x")||
			pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm.y")||
			pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm.z"))
	{
		if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm.x"))
			pTemplDlg->foottempl.axis_z.x=atof(tem_str);
		else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm.y"))
			pTemplDlg->foottempl.axis_z.y=atof(tem_str);
		else //if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm.z"))
			pTemplDlg->foottempl.axis_z.z=atof(tem_str);
		if(pTemplDlg->GetPropValueStr(pItem->m_pParent->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_pParent->m_idProp,sText);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.XA"))	//底板水平正向尺寸
	{
		pTemplDlg->foottempl.base.YA=pTemplDlg->foottempl.base.YB=
			pTemplDlg->foottempl.base.XB=pTemplDlg->foottempl.base.XA=atol(tem_str);
		pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.XB"),"%d",pTemplDlg->foottempl.base.XA);
		pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.YA"),"%d",pTemplDlg->foottempl.base.XA);
		pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.YB"),"%d",pTemplDlg->foottempl.base.XA);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.XB"))	//底板水平负向尺寸
		pTemplDlg->foottempl.base.XB=atol(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.XC0"))//第四象限水平切角长度
	{
		pTemplDlg->foottempl.base.YC[0]=pTemplDlg->foottempl.base.XC[0]=atol(tem_str);
		pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.YC0"),"%d",pTemplDlg->foottempl.base.XC[0]);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.XC1"))//第三象限水平切角长度
	{
		pTemplDlg->foottempl.base.YC[1]=pTemplDlg->foottempl.base.XC[1]=atol(tem_str);
		pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.YC1"),"%d",pTemplDlg->foottempl.base.XC[1]);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.XC2"))//第一象限水平切角长度
	{
		pTemplDlg->foottempl.base.YC[2]=pTemplDlg->foottempl.base.XC[2]=atol(tem_str);
		pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.YC2"),"%d",pTemplDlg->foottempl.base.XC[2]);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.XC3"))//第二象限水平切角长度
	{
		pTemplDlg->foottempl.base.YC[3]=pTemplDlg->foottempl.base.XC[3]=atol(tem_str);
		pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("base.YC3"),"%d",pTemplDlg->foottempl.base.XC[3]);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.YA"))	//底板竖向正向尺寸
		pTemplDlg->foottempl.base.YA=atol(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.YB"))	//底板竖向负向尺寸
		pTemplDlg->foottempl.base.YB=atol(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.YC0"))//第一象限竖向切角长度
		pTemplDlg->foottempl.base.YC[0]=atol(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.YC1"))//第二象限竖向切角长度
		pTemplDlg->foottempl.base.YC[1]=atol(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.YC2"))//第三象限竖向切角长度
		pTemplDlg->foottempl.base.YC[2]=atol(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.YC3"))//第四象限竖向切角长度
		pTemplDlg->foottempl.base.YC[3]=atol(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.R"))
		pTemplDlg->foottempl.base.fR=atof(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.A"))
		pTemplDlg->foottempl.upLinkSP.A=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.A"))
		pTemplDlg->foottempl.downLinkSP.A=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.B"))
		pTemplDlg->foottempl.upLinkSP.B=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.B"))
		pTemplDlg->foottempl.downLinkSP.B=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.C"))
		pTemplDlg->foottempl.upLinkSP.C=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.C"))
		pTemplDlg->foottempl.downLinkSP.C=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.D"))
		pTemplDlg->foottempl.upLinkSP.D=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.D"))
		pTemplDlg->foottempl.downLinkSP.D=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.E"))
		pTemplDlg->foottempl.upLinkSP.E=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.E"))
		pTemplDlg->foottempl.downLinkSP.E=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.F"))
		pTemplDlg->foottempl.upLinkSP.F=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.F"))
		pTemplDlg->foottempl.downLinkSP.F=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.M"))
		pTemplDlg->foottempl.upLinkSP.M=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.M"))
		pTemplDlg->foottempl.downLinkSP.M=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.AL"))
		pTemplDlg->foottempl.upLinkSP.AL=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.AL"))
		pTemplDlg->foottempl.downLinkSP.AL=atoi(tem_str);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.CM"))
		pTemplDlg->foottempl.upLinkSP.CM=tem_str[0]-'0';
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.CM"))
		pTemplDlg->foottempl.downLinkSP.CM=tem_str[0]-'0';
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.style_bolt0_weld1"))
	{
		pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1=tem_str[0]-'0';
		if(pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1==1)
		{	//焊接
			pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan0C=TRUE;
			pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan90C=TRUE;
		}
		else
			pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan0C=!pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan90C;
		//
		long id=CFootDesignTemplDlg::GetPropID("upLinkSP.m_bBreakXueBan0C");
		if(pTemplDlg->GetPropValueStr(id,tem_str))
			pPropList->SetItemPropValue(id,tem_str);
		pPropList->SetItemReadOnly(id,pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1==1);
		id=CFootDesignTemplDlg::GetPropID("upLinkSP.m_bBreakXueBan90C");
		if(pTemplDlg->GetPropValueStr(id,tem_str))
			pPropList->SetItemPropValue(id,tem_str);	
		pPropList->SetItemReadOnly(id,pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1==1);
		//
		pTemplDlg->foottempl.RetrieveShoePlates();
		pTemplDlg->RefreshShoePlateListCtrl();
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.style_bolt0_weld1"))
	{
		pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1=tem_str[0]-'0';
		if(pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1==1)
		{	//焊接
			pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan0C=TRUE;
			pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan90C=TRUE;
		}
		else
			pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan0C=!pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan90C;
		//
		long id=CFootDesignTemplDlg::GetPropID("downLinkSP.m_bBreakXueBan0C");
		if(pTemplDlg->GetPropValueStr(id,tem_str))
			pPropList->SetItemPropValue(id,tem_str);
		pPropList->SetItemReadOnly(id,pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1==1);
		id=CFootDesignTemplDlg::GetPropID("downLinkSP.m_bBreakXueBan90C");
		if(pTemplDlg->GetPropValueStr(id,tem_str))
			pPropList->SetItemPropValue(id,tem_str);	
		pPropList->SetItemReadOnly(id,pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1==1);
		//
		pTemplDlg->foottempl.RetrieveShoePlates();
		pTemplDlg->RefreshShoePlateListCtrl();
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.m_bBreakXueBan0C"))
	{	//打断X轴向上靴板
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("0.Across")==0)
#else
		if(valueStr.Compare("0.贯通底板")==0)
#endif
		{
			pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan0C=FALSE;
			if(pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1==0)
				pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan90C=TRUE;
		}
		else
		{	
			pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan0C=TRUE;
			if(pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1==0)
				pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan90C=FALSE;
		}
		long id=CFootDesignTemplDlg::GetPropID("upLinkSP.m_bBreakXueBan90C");
		if(pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1==0&&pTemplDlg->GetPropValueStr(id,tem_str))
			pPropList->SetItemPropValue(id,tem_str);
		//
		pTemplDlg->foottempl.RetrieveShoePlates();
		pTemplDlg->RefreshShoePlateListCtrl();
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.m_bBreakXueBan0C"))
	{	//打断X轴向下靴板
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("0.Across")==0)
#else
		if(valueStr.Compare("0.贯通底板")==0)
#endif
		{
			pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan0C=FALSE;
			if(pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1==0)
				pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan90C=TRUE;
		}
		else
		{	
			pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan0C=TRUE;
			if(pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1==0)
				pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan90C=FALSE;
		}
		long id=CFootDesignTemplDlg::GetPropID("downLinkSP.m_bBreakXueBan90C");
		if(pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1==0&&pTemplDlg->GetPropValueStr(id,tem_str))
			pPropList->SetItemPropValue(id,tem_str);
		//
		pTemplDlg->foottempl.RetrieveShoePlates();
		pTemplDlg->RefreshShoePlateListCtrl();
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("upLinkSP.m_bBreakXueBan90C"))
	{	//打断Y轴向上靴板
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("0.Across")==0)
#else
		if(valueStr.Compare("0.贯通底板")==0)
#endif
		{
			pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan90C=FALSE;
			if(pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1==0)
				pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan0C=TRUE;
		}
		else
		{	
			pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan90C=TRUE;
			if(pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1==0)
				pTemplDlg->foottempl.upLinkSP.m_bBreakXueBan0C=FALSE;
		}
		long id=CFootDesignTemplDlg::GetPropID("upLinkSP.m_bBreakXueBan0C");
		if(pTemplDlg->foottempl.upLinkSP.style_bolt0_weld1==0&&pTemplDlg->GetPropValueStr(id,tem_str))
			pPropList->SetItemPropValue(id,tem_str);
		//
		pTemplDlg->foottempl.RetrieveShoePlates();
		pTemplDlg->RefreshShoePlateListCtrl();
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("downLinkSP.m_bBreakXueBan90C"))
	{	//打断Y轴向下靴板
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("0.Across")==0)
#else
		if(valueStr.Compare("0.贯通底板")==0)
#endif
		{
			pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan90C=FALSE;
			if(pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1==0)
				pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan0C=TRUE;
		}
		else
		{	
			pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan90C=TRUE;
			if(pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1==0)
				pTemplDlg->foottempl.downLinkSP.m_bBreakXueBan0C=FALSE;
		}
		long id=CFootDesignTemplDlg::GetPropID("downLinkSP.m_bBreakXueBan0C");
		if(pTemplDlg->foottempl.downLinkSP.style_bolt0_weld1==0&&pTemplDlg->GetPropValueStr(id,tem_str))
			pPropList->SetItemPropValue(id,tem_str);
		//
		pTemplDlg->foottempl.RetrieveShoePlates();
		pTemplDlg->RefreshShoePlateListCtrl();
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.cMaterial"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->cMaterial=QuerySteelBriefMatMark(tem_str);
		pTemplDlg->RefreshLinkAngleListCtrl(NULL);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.PartNo"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->sPartNo.Copy(tem_str);
#if defined(__TMA_)||defined(__LMA_)
		BOMPART* pPart=NULL;
		if (::ValidateLocalizeFeature(FEATURE::MODEL_CHECK_BOMCOMPARE)!=0||
			VerifyValidFunction(LICFUNC::FUNC_IDENTITY_UBOM))
			pPart=globalDataCmpModel.GetDrawingBomPart(tem_str,BOMPART::PLATE);
		if(pPart)
		{
			pTemplDlg->ActiveRibPlate()->thick=ftoi(pPart->thick);
			pTemplDlg->ActiveRibPlate()->cMaterial=pPart->cMaterial;
			//
			CXhChar50 sValue("%.0f",pPart->thick);
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("RibPlate.thick"),(char*)sValue);
			QuerySteelMatMark(pPart->cMaterial,sValue);
			pPropList->SetItemPropValue(CFootDesignTemplDlg::GetPropID("RibPlate.cMaterial"),(char*)sValue);

		}
#endif
		pTemplDlg->RefreshLinkAngleListCtrl(NULL);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.OX"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->pos_para.PARA1.OX=atof(tem_str);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.OY"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->pos_para.PARA1.OY=atof(tem_str);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.axisXType"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->axisXType=tem_str[0]-'0';
		if(tem_str[0]=='1')				//X轴正向
		{
			pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_x=1;
			pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_y=0;
		}
		else if(tem_str[0]=='2')		//X轴负向
		{
			pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_x=-1;
			pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_y=0;
		}
		else if(tem_str[0]=='3')		//Y轴正向
		{
			pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_x=0;
			pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_y=1;
		}
		else if(tem_str[0]=='4')		//Y轴负向
		{
			pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_x=0;
			pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_y=-1;
		}
		CPropTreeItem* pSonItem=NULL,*pFindItem=NULL;
		pPropList->DeleteAllSonItems(pItem);
		pPropList->DeleteItemByPropId(CFootDesignTemplDlg::GetPropID("RibPlate.A"));
		pPropList->DeleteItemByPropId(CFootDesignTemplDlg::GetPropID("RibPlate.B"));
		pPropList->DeleteItemByPropId(CFootDesignTemplDlg::GetPropID("RibPlate.C"));
		pPropList->DeleteItemByPropId(CFootDesignTemplDlg::GetPropID("RibPlate.D"));
		pPropList->DeleteItemByPropId(CFootDesignTemplDlg::GetPropID("RibPlate.fGradientAngle"));
		if(tem_str[0]>='1'&&tem_str[0]<='4')
		{	
			pSonItem=oper.InsertEditPropItem(pItem,"RibPlate.axisX_x","","",0,TRUE);
			pSonItem->SetReadOnly();
			pSonItem=oper.InsertEditPropItem(pItem,"RibPlate.axisX_y","","",1,TRUE);
			pSonItem->SetReadOnly();
			//
			pFindItem=oper.InsertButtonPropItem(pItem->m_pParent,"RibPlate.origin","","",5,TRUE);
			pPropList->DeleteAllSonItems(pFindItem);
			if(tem_str[0]=='3'||tem_str[0]=='4')
				oper.InsertEditPropItem(pFindItem,"RibPlate.OX","","",-1,TRUE);
			if(tem_str[0]=='1'||tem_str[0]=='2')
				oper.InsertEditPropItem(pFindItem,"RibPlate.OY","","",-1,TRUE);
			oper.InsertEditPropItem(pItem->m_pParent,"RibPlate.A","","",6,TRUE);
			oper.InsertEditPropItem(pItem->m_pParent,"RibPlate.B","","",7,TRUE);
			oper.InsertEditPropItem(pItem->m_pParent,"RibPlate.C","","",8,TRUE);
			oper.InsertEditPropItem(pItem->m_pParent,"RibPlate.D","","",9,TRUE);
			oper.InsertEditPropItem(pItem->m_pParent,"RibPlate.fGradientAngle","","",-1,TRUE);
		}
		else 
		{
			pPropList->DeleteItemByPropId(CFootDesignTemplDlg::GetPropID("RibPlate.origin"));
			oper.InsertEditPropItem(pItem,"RibPlate.angle","","",-1,TRUE);
			if(tem_str[0]=='5')
			{
				oper.InsertEditPropItem(pItem,"RibPlate.fRayDist","","",-1,TRUE);
				oper.InsertEditPropItem(pItem->m_pParent,"RibPlate.A","","",5,TRUE);
			}
			else
			{
				oper.InsertEditPropItem(pItem->m_pParent,"RibPlate.C","","",6,TRUE);
				oper.InsertEditPropItem(pItem->m_pParent,"RibPlate.D","","",7,TRUE);
			}
		}
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.axisX_x"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_x=atof(tem_str);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.axisX_y"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->pos_para.PARA1.axisX_y=atof(tem_str);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.thick"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->thick=atoi(tem_str);
		pTemplDlg->RefreshLinkAngleListCtrl(NULL);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.upper"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		if(stricmp(tem_str,"downside")==0)
#else
		if(stricmp(tem_str,"下侧")==0)
#endif
			pTemplDlg->ActiveRibPlate()->upper=false;
		else //if(stricmp(tem_str,"上侧")==0)
			pTemplDlg->ActiveRibPlate()->upper=true;
		pTemplDlg->RefreshLinkAngleListCtrl(NULL);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.A"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->A=atoi(tem_str);
		if(pTemplDlg->foottempl.base.shape==1&&pTemplDlg->ActiveRibPlate()->axisXType==5)
		{	//批量修改所有类型为5的肋板参数
			for(CFootTempl::RIBPLATE* pRibPlate=pTemplDlg->foottempl.listRibPlates.GetFirst();pRibPlate;
				pRibPlate=pTemplDlg->foottempl.listRibPlates.GetNext())
			{
				if(pRibPlate->axisXType==5)
					pRibPlate->A=atoi(tem_str);
			}
		}
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.B"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->B=atoi(tem_str);
	}
	
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.C"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->C=atoi(tem_str);
		if(pTemplDlg->foottempl.base.shape==1&&pTemplDlg->ActiveRibPlate()->axisXType==0)
		{	//批量修改所有类型为0的肋板参数
			for(CFootTempl::RIBPLATE* pRibPlate=pTemplDlg->foottempl.listRibPlates.GetFirst();pRibPlate;
				pRibPlate=pTemplDlg->foottempl.listRibPlates.GetNext())
			{
				if(pRibPlate->axisXType==0)
					pRibPlate->C=atoi(tem_str);
			}
		}
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.D"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->D=atoi(tem_str);
		if(pTemplDlg->foottempl.base.shape==1&&pTemplDlg->ActiveRibPlate()->axisXType==0)
		{	//批量修改所有类型为0的肋板参数
			for(CFootTempl::RIBPLATE* pRibPlate=pTemplDlg->foottempl.listRibPlates.GetFirst();pRibPlate;
				pRibPlate=pTemplDlg->foottempl.listRibPlates.GetNext())
			{
				if(pRibPlate->axisXType==0)
					pRibPlate->D=atoi(tem_str);
			}
		}
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.slotW"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->water_slot_wide=atoi(tem_str);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.iCutMode"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->iCutMode=tem_str[0]-'0';
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.fCutAngleWidth"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->fCutAngleWidth=atof(tem_str);
		if(pTemplDlg->foottempl.base.shape==1&&
			(pTemplDlg->ActiveRibPlate()->axisXType==0||pTemplDlg->ActiveRibPlate()->axisXType==5))
		{	//批量修改所有类型为0、5的肋板参数
			for(CFootTempl::RIBPLATE* pRibPlate=pTemplDlg->foottempl.listRibPlates.GetFirst();pRibPlate;
				pRibPlate=pTemplDlg->foottempl.listRibPlates.GetNext())
			{
				if(pRibPlate->axisXType==pTemplDlg->ActiveRibPlate()->axisXType)
					pRibPlate->fCutAngleWidth=atof(tem_str);
			}
		}
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.fGradientAngle"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->fGradientAngle=atof(tem_str);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.angle"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		if(pTemplDlg->ActiveRibPlate()->axisXType==0)
			pTemplDlg->ActiveRibPlate()->pos_para.PARA2.angle=atof(tem_str);
		else 
			pTemplDlg->ActiveRibPlate()->pos_para.PARA3.angle=atof(tem_str);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.fRayDist"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->pos_para.PARA3.fRayDist=atof(tem_str);
		if(pTemplDlg->foottempl.base.shape==1&&pTemplDlg->ActiveRibPlate()->axisXType==5)
		{	//批量修改所有类型为5的肋板参数
			for(CFootTempl::RIBPLATE* pRibPlate=pTemplDlg->foottempl.listRibPlates.GetFirst();pRibPlate;
				pRibPlate=pTemplDlg->foottempl.listRibPlates.GetNext())
			{
				if(pRibPlate->axisXType==5)
					pRibPlate->pos_para.PARA3.fRayDist=atof(tem_str);
			}
			//修改射线距离以后更新右侧列表中的肋板射线距离
			for(int i=0;i<pTemplDlg->m_listCtrlLinkAngles.GetItemCount();i++)
			{
				CSuperGridCtrl::CTreeItem *pItem=pTemplDlg->m_listCtrlLinkAngles.GetTreeItem(i);
				TREEITEM_INFO *pItemInfo=(TREEITEM_INFO*)pItem->m_idProp;
				if(pItemInfo==NULL||pItemInfo->itemType!=CFootDesignTemplDlg::ITEM_TYPE_RIBPLATE)
					continue;
				CFootTempl::RIBPLATE *pRibPlate=pTemplDlg->foottempl.listRibPlates.GetValue(pItemInfo->dwRefData);
				if(pRibPlate&&pRibPlate->axisXType==5)
					pTemplDlg->m_listCtrlLinkAngles.SetSubItemText(pItem,5,CXhChar50("%g",pRibPlate->pos_para.PARA3.fRayDist));
			}
		}
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.MH"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->MH=atoi(tem_str);
		if(pTemplDlg->ActiveRibPlate()->MH>0)
		{
			oper.InsertEditPropItem(pItem,"RibPlate.MF","","",-1,TRUE);
			oper.InsertEditPropItem(pItem,"RibPlate.MG","","",-1,TRUE);
		}
		else
			pPropList->DeleteAllSonItems(pItem);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.MF"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->MF=atoi(tem_str);
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("RibPlate.MG"))
	{
		if(pTemplDlg->ActiveRibPlate()==NULL)
			return FALSE;
		pTemplDlg->ActiveRibPlate()->MG=atoi(tem_str);
	}
	if( pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm.x")||
		pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm.y")||
		pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm.z"))
	{	//当法线发生更改时应自动调整底板中心定位点坐标 wjh-2016.8.4
		UCS_STRU ucs;
		pTemplDlg->foottempl.GetUCS(ucs);
		pTemplDlg->UpdateBasePlateOriginProperty();
	}
	else if(fabs(pTemplDlg->foottempl.axis_z.x)+fabs(pTemplDlg->foottempl.axis_z.y)>0&&(
		pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.origin.x")||
		pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.origin.y")))
	{	//非标准斜塔脚，更改定位基点X,Y分量时其Z分量应根据原始平面重新计算
		GEPOINT org;
		Int3dlf(org,pTemplDlg->foottempl.origin.Position(),GEPOINT(0,0,1),datumOrg,pTemplDlg->foottempl.axis_z);
		pTemplDlg->foottempl.origin.SetPositionZ(org.z);
		pTemplDlg->UpdateBasePlateOriginProperty();
	}
	g_pDraw->BuildDisplayList(pTemplDlg);
	return TRUE;
}
BOOL DesignFootButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CFootDesignTemplDlg* pTemplDlg=(CFootDesignTemplDlg*)pPropList->GetParent();
	if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("m_hBaseNode"))
		pTemplDlg->SelectObject(CLS_NODE,pItem->m_idProp);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngle")||
		pItem->m_idProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown"))
		pTemplDlg->SelectObject(CLS_LINEANGLE,pItem->m_idProp);
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.origin"))	//定位原点（牛蹄板使用）
	{
		pTemplDlg->m_iCmdParaType=pItem->m_idProp;
		pTemplDlg->ExecuteCommand(pPropList,pItem->m_idProp,(WPARAM)&pTemplDlg->foottempl);
		return TRUE;
	}
	else if(pItem->m_idProp==CFootDesignTemplDlg::GetPropID("base.worknorm"))	//定位原点（牛蹄板使用）
	{
		pTemplDlg->m_iCmdParaType=pItem->m_idProp;
		pTemplDlg->ExecuteCommand(pPropList,pItem->m_idProp,(WPARAM)&pTemplDlg->foottempl);
		return TRUE;
	}
	return TRUE;
}
void CFootDesignTemplDlg::UpdateLinkAngleProperty(CPropTreeItem *pPareItem,BOOL bUpJg/*=TRUE*/)
{
	if(pPareItem==NULL)
		return;
	CPropertyListOper<CFootDesignTemplDlg> oper(&m_xPropList,this);
	CPropTreeItem *pLeafItem=NULL;
	pPareItem->m_bHideChildren=FALSE;
	//主材角钢连接端正负头(暂时用于直观显示)
	if(bUpJg)
		pLeafItem=oper.InsertEditPropItem(pPareItem,"primaryAngle.oddment");
	else
		pLeafItem=oper.InsertEditPropItem(pPareItem,"primaryAngleDown.oddment");
	pLeafItem->SetReadOnly(foottempl.m_bWeldDiZuo);
	//主材角钢塔脚连接端的螺栓数量
	if(bUpJg)
		oper.InsertEditPropItem(pPareItem,"primaryAngle.bolt.num");
	else
		oper.InsertEditPropItem(pPareItem,"primaryAngleDown.bolt.num");
	//主材角钢塔脚连接端的螺栓排数
	if(bUpJg)
		oper.InsertCmbListPropItem(pPareItem,"primaryAngle.bolt.rows");
	else
		oper.InsertCmbListPropItem(pPareItem,"primaryAngleDown.bolt.rows");
	//主材角钢塔脚连接端的螺栓直径
	if(bUpJg)
		oper.InsertCmbListPropItem(pPareItem,"primaryAngle.bolt.d");
	else
		oper.InsertCmbListPropItem(pPareItem,"primaryAngleDown.bolt.d");
	//螺栓端距
	if(bUpJg)
		oper.InsertEditPropItem(pPareItem,"primaryAngle.bolt.EndSpace");
	else
		oper.InsertEditPropItem(pPareItem,"primaryAngleDown.bolt.EndSpace");
	//螺栓间距
	if(bUpJg)
		oper.InsertEditPropItem(pPareItem,"primaryAngle.bolt.BoltSpace");
	else
		oper.InsertEditPropItem(pPareItem,"primaryAngleDown.bolt.BoltSpace");
	//主材角钢塔脚连接端的螺栓布置方式
	if(bUpJg)
		oper.InsertCmbListPropItem(pPareItem,"primaryAngle.bolt.layout");
	else
		oper.InsertCmbListPropItem(pPareItem,"primaryAngleDown.bolt.layout");
	//主材角钢上布孔准距
	if(bUpJg)
		pLeafItem=oper.InsertButtonPropItem(pPareItem,"primaryAngle.ZhunJu");
	else
		pLeafItem=oper.InsertButtonPropItem(pPareItem,"primaryAngleDown.ZhunJu");
	pLeafItem->m_bHideChildren=TRUE;
	//准距g
	if(bUpJg)
		oper.InsertEditPropItem(pLeafItem,"primaryAngle.g");
	else
		oper.InsertEditPropItem(pLeafItem,"primaryAngleDown.g");	
	//准距g1
	if(bUpJg)
		oper.InsertEditPropItem(pLeafItem,"primaryAngle.g1");
	else
		oper.InsertEditPropItem(pLeafItem,"primaryAngleDown.g1");
	//准距g2
	if(bUpJg)
		oper.InsertEditPropItem(pLeafItem,"primaryAngle.g2");
	else
		oper.InsertEditPropItem(pLeafItem,"primaryAngleDown.g2");
}
void CFootDesignTemplDlg::DisplayDesignFootProperty()
{
	m_xPropList.CleanTree();
	m_xPropList.SetModifyValueFunc(ModifyDesignFootProperty);
	m_xPropList.SetButtonClickFunc(DesignFootButtonClick);
	CPropertyListOper<CFootDesignTemplDlg> oper(&m_xPropList,this);
	const int GROUP_BASIC=1,GROUP_BASEPLATE=2,GROUP_SHOEPLATE=3,GROUP_RIBPLATE=4;
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pLeafItem=NULL,*pRootItem=m_xPropList.GetRootItem();
	//基本参数	
	pGroupItem=oper.InsertPropItem(pRootItem,"basicInfo");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_BASIC);
	oper.InsertBtnEditPropItem(pGroupItem,"m_hBaseNode");	//依赖节点
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"m_bWeldDiZuo");	//连接主材角钢焊接到底座
	//主材角钢直接焊接到底座时, 角钢端楞点碰撞间隙值C失效
	if(!foottempl.m_bWeldDiZuo)
		oper.InsertEditPropItem(pPropItem,"C");	//角钢端棱点到底板距离(负值表指定负头)
	oper.InsertEditPropItem(pGroupItem,"base.thick");	//底板板厚
	pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"primaryAngle");	//塔脚的连接主材角钢
	if(foottempl.primaryAngle.pAngle!=NULL)
		UpdateLinkAngleProperty(pPropItem,TRUE);
	//
	pPropItem=oper.InsertBtnEditPropItem(pGroupItem,"primaryAngleDown");	//牛蹄板的下侧主材角钢
	if(foottempl.primaryAngleDown.pAngle!=NULL)
		UpdateLinkAngleProperty(pPropItem,FALSE);
	//底板参数
	pGroupItem=oper.InsertPropItem(pRootItem,"basePlateInfo");
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_BASEPLATE);
	oper.InsertEditPropItem(pGroupItem,"base.sPartNo");	//底板编号
	oper.InsertCmbListPropItem(pGroupItem,"base.cMaterial");	//材质
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"base.nNormOffsetType");	//原点向基准平面的法向偏移
	pLeafItem=oper.InsertEditPropItem(pPropItem,"base.nNormOffset");	//自装配定位原点向基准平面的法向偏移量	
	pLeafItem->SetReadOnly(foottempl.base.cNormOffsetType!=2);
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"base.origin");	//底板的装配坐标系的原点位置
	pPropItem->m_bHideChildren=FALSE;
	oper.InsertEditPropItem(pPropItem,"base.origin.x");	//底板的装配坐标系的原点位置
	oper.InsertEditPropItem(pPropItem,"base.origin.y");	//底板的装配坐标系的原点位置
	oper.InsertEditPropItem(pPropItem,"base.origin.z");	//底板的装配坐标系的原点位置
	pPropItem=oper.InsertButtonPropItem(pGroupItem,"base.worknorm");	//底板的工作法线
	pPropItem->m_bHideChildren=TRUE;
	oper.InsertEditPropItem(pPropItem,"base.worknorm.x");
	oper.InsertEditPropItem(pPropItem,"base.worknorm.y");
	oper.InsertEditPropItem(pPropItem,"base.worknorm.z");
	pPropItem=oper.InsertCmbListPropItem(pGroupItem,"base.shape");	//外形类别
	if(foottempl.base.shape==0)
	{
		oper.InsertEditPropItem(pPropItem,"base.XA");	//原点至X+向宽
		oper.InsertEditPropItem(pPropItem,"base.XB");	//原点至X-向宽
		oper.InsertEditPropItem(pPropItem,"base.YA");	//原点至Y+向宽
		oper.InsertEditPropItem(pPropItem,"base.YB");	//原点至Y-向宽
		CPropTreeItem* pSonItem=NULL;
		pSonItem=oper.InsertEditPropItem(pPropItem,"base.XC2");	//第一象限切角宽度
		pSonItem->m_bHideChildren=TRUE;
		oper.InsertEditPropItem(pSonItem,"base.YC2");
		pSonItem=oper.InsertEditPropItem(pPropItem,"base.XC3");	//第二象限切角宽度
		pSonItem->m_bHideChildren=TRUE;
		oper.InsertEditPropItem(pSonItem,"base.YC3");
		pSonItem=oper.InsertEditPropItem(pPropItem,"base.XC1");	//第三象限切角宽度
		pSonItem->m_bHideChildren=TRUE;
		oper.InsertEditPropItem(pSonItem,"base.YC1");
		pSonItem=oper.InsertEditPropItem(pPropItem,"base.XC0");	//第四象限切角宽度
		pSonItem->m_bHideChildren=TRUE;
		oper.InsertEditPropItem(pSonItem,"base.YC0");
	}
	else
		oper.InsertEditPropItem(pPropItem,"base.R");	//原点至X+向宽
	//靴板参数
	pGroupItem = oper.InsertPropItem(pRootItem,"upLinkSP");
	pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_SHOEPLATE);
	UpdateShoePlateProperty(pGroupItem);
	if(m_bUpAndDownSide)
	{
		pGroupItem = oper.InsertPropItem(pRootItem,"downLinkSP");
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_SHOEPLATE);
		UpdateShoePlateProperty(pGroupItem,FALSE);
	}
	//肋板参数
	pGroupItem=oper.InsertPropItem(pRootItem,"ribPlateInfo");
	pGroupItem->m_dwPropGroup=GetSingleWord(GROUP_RIBPLATE);
	UpdateRibPlateProperty(pGroupItem);
	m_xPropList.Redraw();
}
void CFootDesignTemplDlg::UpdateShoePlateProperty(CPropTreeItem *pPareItem,BOOL bUpShoePlate/*=TRUE*/)
{
	if(pPareItem==NULL)
		return;
	CPropertyListOper<CFootDesignTemplDlg> oper(&m_xPropList,this);
	CPropTreeItem* pPropItem=NULL,*pSonItem=NULL;
	//靴板台阶高度
	if(bUpShoePlate)
		oper.InsertEditPropItem(pPareItem,"upLinkSP.A");
	else
		oper.InsertEditPropItem(pPareItem,"downLinkSP.A");
	//内侧台阶高度
	if(bUpShoePlate)
		oper.InsertEditPropItem(pPareItem,"upLinkSP.C");
	else
		oper.InsertEditPropItem(pPareItem,"downLinkSP.C");
	//靴板顶部前侧余量
	if(bUpShoePlate)
		oper.InsertEditPropItem(pPareItem,"upLinkSP.B");	
	else
		oper.InsertEditPropItem(pPareItem,"downLinkSP.B");
	//靴板顶部内侧余量
	if(bUpShoePlate)
		oper.InsertEditPropItem(pPareItem,"upLinkSP.D");	
	else
		oper.InsertEditPropItem(pPareItem,"downLinkSP.D");
	//靴板在斜材楞线侧的延伸量
	if(bUpShoePlate)
		oper.InsertEditPropItem(pPareItem,"upLinkSP.E");	
	else
		oper.InsertEditPropItem(pPareItem,"downLinkSP.E");
	//靴板在斜材肢翼侧的延伸量
	if(bUpShoePlate)
		oper.InsertEditPropItem(pPareItem,"upLinkSP.F");	
	else
		oper.InsertEditPropItem(pPareItem,"downLinkSP.F");
	//靴板延伸至底板边缘时的间隙值M==0表示延伸至底板边缘
	if(bUpShoePlate)
		oper.InsertEditPropItem(pPareItem,"upLinkSP.M");
	else
		oper.InsertEditPropItem(pPareItem,"downLinkSP.M");
	//靴板非连接侧角钢长度方向延伸量，默认根据角钢螺栓计算
	if(bUpShoePlate)
		oper.InsertEditPropItem(pPareItem,"upLinkSP.AL");
	else
		oper.InsertEditPropItem(pPareItem,"downLinkSP.AL");
	//靴板倒角模式
	if(bUpShoePlate)
		oper.InsertCmbListPropItem(pPareItem,"upLinkSP.CM");
	else
		oper.InsertCmbListPropItem(pPareItem,"downLinkSP.CM");
	//靴板与主材的连接方式
	if(bUpShoePlate)
		oper.InsertCmbListPropItem(pPareItem,"upLinkSP.style_bolt0_weld1");	
	else
		oper.InsertCmbListPropItem(pPareItem,"downLinkSP.style_bolt0_weld1");	
	//靴板是否打断
	if(bUpShoePlate)
	{
		pPropItem=oper.InsertCmbListPropItem(pPareItem,"upLinkSP.m_bBreakXueBan0C");
		pPropItem->SetReadOnly(foottempl.upLinkSP.style_bolt0_weld1==1);	
		pPropItem=oper.InsertCmbListPropItem(pPareItem,"upLinkSP.m_bBreakXueBan90C");
		pPropItem->SetReadOnly(foottempl.upLinkSP.style_bolt0_weld1==1);
	}
	else
	{
		pPropItem=oper.InsertCmbListPropItem(pPareItem,"downLinkSP.m_bBreakXueBan0C");
		pPropItem->SetReadOnly(foottempl.downLinkSP.style_bolt0_weld1==1);	
		pPropItem=oper.InsertCmbListPropItem(pPareItem,"downLinkSP.m_bBreakXueBan90C");
		pPropItem->SetReadOnly(foottempl.downLinkSP.style_bolt0_weld1==1);
	}
}
void CFootDesignTemplDlg::UpdateRibPlateProperty(CPropTreeItem *pParentItem,CFootTempl::RIBPLATE* pRibPlate/*=NULL*/)
{
	if(pParentItem==NULL)
		pParentItem = m_xPropList.FindItemByPropId(GetPropID("ribPlateInfo"),NULL);
	if(pRibPlate!=NULL)
		m_idActiveRibPlate=pRibPlate->Serial();
	else if(ActiveRibPlate()==NULL)
	{
		m_xPropList.DeleteAllSonItems(pParentItem);
		return;
	}
	CPropertyListOper<CFootDesignTemplDlg> oper(&m_xPropList,this);
	CPropTreeItem* pPropItem=NULL,*pSonItem=NULL;
	pPropItem=oper.InsertEditPropItem(pParentItem,"RibPlate.Serial");	//标识号
	pPropItem->SetReadOnly();
	oper.InsertCmbListPropItem(pParentItem,"RibPlate.cMaterial");	//材质
	oper.InsertEditPropItem(pParentItem,"RibPlate.PartNo");	//构件编号
	oper.InsertEditPropItem(pParentItem,"RibPlate.thick");
	if(foottempl.primaryAngleDown.pAngle!=NULL)	//牛蹄板应允许用户自由选择上下侧肋板
		oper.InsertCmbListPropItem(pParentItem,"RibPlate.upper");
	//定位参数
	BOOL bReadOnlyRibPara=foottempl.base.shape==1&&(pRibPlate->axisXType==0||pRibPlate->axisXType==5);
	int axisXType=ActiveRibPlate()->axisXType;
	pPropItem=oper.InsertCmbListPropItem(pParentItem,"RibPlate.axisXType");	//焊缝走向
	pPropItem->SetReadOnly(bReadOnlyRibPara);
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.A"));
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.B"));
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.C"));
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.D"));
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.OX"));
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.OY"));
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.fGradientAngle"));
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.MH"));
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.MF"));
	m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.MG"));
	if(axisXType==0)		//指定角度
	{
		pSonItem=oper.InsertEditPropItem(pPropItem,"RibPlate.angle");
		pSonItem->SetReadOnly(foottempl.base.shape==1);
		m_xPropList.DeleteItemByPropId(GetPropID("RibPlate.fRayDist"));
		oper.InsertEditPropItem(pParentItem,"RibPlate.C","","",5);		//台口高度
		oper.InsertEditPropItem(pParentItem,"RibPlate.D","","",6);		//顶部宽度
	}
	else if(axisXType==5)	//指定垂线角度
	{
		pSonItem=oper.InsertEditPropItem(pPropItem,"RibPlate.angle");
		pSonItem->SetReadOnly(bReadOnlyRibPara);
		oper.InsertEditPropItem(pPropItem,"RibPlate.fRayDist");
		//外形参数
		oper.InsertEditPropItem(pParentItem,"RibPlate.A","","",5);		//肋板高度
	}
	else
	{
		pSonItem=oper.InsertEditPropItem(pPropItem,"RibPlate.axisX_x");
		pSonItem->SetReadOnly();
		pSonItem=oper.InsertEditPropItem(pPropItem,"RibPlate.axisX_y");
		pSonItem->SetReadOnly();
		pPropItem=oper.InsertButtonPropItem(pParentItem,"RibPlate.origin");	//定位基点
		if(axisXType==3||axisXType==4)
			oper.InsertEditPropItem(pPropItem,"RibPlate.OX");
		if(axisXType==1||axisXType==2)
			oper.InsertEditPropItem(pPropItem,"RibPlate.OY");
		//外形参数
		oper.InsertEditPropItem(pParentItem,"RibPlate.A","","",6);		//肋板高度
		oper.InsertEditPropItem(pParentItem,"RibPlate.B","","",7);		//肋板长度
		oper.InsertEditPropItem(pParentItem,"RibPlate.C","","",8);		//台口高度
		oper.InsertEditPropItem(pParentItem,"RibPlate.D","","",9);		//顶部宽度
	}
	//外形参数
	oper.InsertCmbListPropItem(pParentItem,"RibPlate.iCutMode");	//倒角模式
	oper.InsertEditPropItem(pParentItem,"RibPlate.fCutAngleWidth");	//切角宽度
	if(axisXType!=5&&axisXType!=0)
		oper.InsertEditPropItem(pParentItem,"RibPlate.fGradientAngle");	//倾斜角度
	pPropItem=oper.InsertEditPropItem(pParentItem,"RibPlate.MH");	//肋板内侧通孔孔径
	if(ActiveRibPlate()->MH>0)
	{
		oper.InsertEditPropItem(pPropItem,"RibPlate.MF","","",-1,TRUE);	//孔间隙F
		oper.InsertEditPropItem(pPropItem,"RibPlate.MG","","",-1,TRUE);	//孔间隙G
	}
	m_xPropList.Redraw();
}

void CFootDesignTemplDlg::OnBnClickedBnAddRibPlate()
{
	CFootTempl::RIBPLATE* pRibPlate=foottempl.listRibPlates.Add(0);
	m_idActiveRibPlate=pRibPlate->Serial();
	foottempl.InitRibPlateParaInfo(pRibPlate);
	CPropTreeItem* pItem=m_xPropList.FindItemByPropId(GetPropID("ribPlateInfo"),NULL);
	UpdateRibPlateProperty(pItem);
	RefreshLinkAngleListCtrl(NULL,pRibPlate);
	g_pDraw->BuildDisplayList(this);
}
BOOL CFootDesignTemplDlg::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	if(idEventProp==CFootDesignTemplDlg::GetPropID("m_hBaseNode"))
	{
		CLDSNode* pNodo=console.FromNodeHandle(hPickObj);
		if(pNodo)
			foottempl.m_hBaseNode=hPickObj;
	}
	else if(idEventProp==CFootDesignTemplDlg::GetPropID("primaryAngle"))
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)console.FromPartHandle(hPickObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle)
		{
			CLDSNode* pBaseNode=console.FromNodeHandle(foottempl.m_hBaseNode);
			if(foottempl.primaryAngle.pAngle==NULL)
			{
				foottempl.primaryAngle.pAngle=pAngle;
				foottempl.primaryAngle.UpdateBoltInfo(pBaseNode);
			}
			else
				foottempl.primaryAngle.pAngle=pAngle;
		}
	}
	else if(idEventProp==CFootDesignTemplDlg::GetPropID("primaryAngleDown"))
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)console.FromPartHandle(hPickObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pAngle)
		{
			CLDSNode* pBaseNode=console.FromNodeHandle(foottempl.m_hBaseNode);
			if(foottempl.primaryAngleDown.pAngle==NULL)
			{
				foottempl.primaryAngleDown.pAngle=pAngle;
				foottempl.primaryAngleDown.UpdateBoltInfo(pBaseNode);
			}
			else	//之前已存在角钢时，就不需要再更新螺栓信息，否则操作极易冲掉之前正确的信息不方便 wjh-2015.9.19
				foottempl.primaryAngleDown.pAngle=pAngle;
		}
	}
	return TRUE;
}
//消息响应事件
void CFootDesignTemplDlg::OnTcnSelchangeTabGroup(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_iActiveTabPage=m_ctrlPropGroup.GetCurSel();
	if(m_iActiveTabPage==1)
		m_iDispType=DISP_BASE_PLATE;
	else if(m_iActiveTabPage==2)
		m_iDispType=DISP_SHOE_PLATE;
	else if(m_iActiveTabPage==3)
		m_iDispType=DISP_RIB_PLATE;
	else 
		m_iDispType=DISP_OTHER;
	UpdateSketchMap();
	if(m_iDispType==DISP_BASE_PLATE||m_iDispType==DISP_RIB_PLATE)
	{
		GetDlgItem(IDC_BN_ADD_BASE_HOLE)->ShowWindow(SW_SHOW);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText(":");
#else
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("底脚孔列表:");
#endif
	}
	else
	{
		GetDlgItem(IDC_BN_ADD_BASE_HOLE)->ShowWindow(SW_HIDE);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("Shoe Plate List:");
#else
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("靴板列表:");
#endif
	}
	if(m_iDispType==DISP_RIB_PLATE)
	{
		GetDlgItem(IDC_BN_ADD_RIB_PLATE)->ShowWindow(SW_SHOW);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("Rib Plate List:");
#else
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("肋板列表:");
#endif
	}
	else
	{
		GetDlgItem(IDC_BN_ADD_RIB_PLATE)->ShowWindow(SW_HIDE);
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("Connection Angle List:");
#else
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("连接角钢列表:");
#endif
	}

	RefreshShoePlateListCtrl();
	RefreshLinkAngleListCtrl();
	//
	m_xPropList.m_iPropGroup=m_iActiveTabPage;
	m_xPropList.Redraw();
	*pResult = 0;
}
void CFootDesignTemplDlg::DeleteItemInfo(TREEITEM_INFO *pItemInfo)
{
	for(TREEITEM_INFO *pInfo=itemInfoList.GetFirst();pInfo;pInfo=itemInfoList.GetNext())
	{
		if(pItemInfo==pInfo)
		{
			itemInfoList.DeleteCursor(TRUE);
			break;
		}
	}
}
void CFootDesignTemplDlg::RefreshLinkAngleListCtrl(CFootTempl::SHOEPLATE* pShoePlate/*=NULL*/,void *pSelectedItemData/*=NULL*/)
{
	CString itemText;
	CListCtrlItemInfo* lpInfo = NULL;
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	m_listCtrlLinkAngles.EmptyColumnHeader();
	if(m_iDispType==DISP_RIB_PLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("Rib plate list:");
		m_listCtrlLinkAngles.AddColumnHeader("Serial",55);
		m_listCtrlLinkAngles.AddColumnHeader("Material",60);
		m_listCtrlLinkAngles.AddColumnHeader("Label",60);
		m_listCtrlLinkAngles.AddColumnHeader("Thickness",60);
		if(foottempl.base.shape==1)	//圆底板时肋板为环绕布置,均为射线半径及射线角定位
		{
			m_listCtrlLinkAngles.AddColumnHeader("Ray line Angle°",80);
			m_listCtrlLinkAngles.AddColumnHeader("Ray Distance",80);
		}
	}
	else
	{
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("Angle list:");
		m_listCtrlLinkAngles.AddColumnHeader("Pos Label",105);
		m_listCtrlLinkAngles.AddColumnHeader("Handle",55);
		m_listCtrlLinkAngles.AddColumnHeader("Bolt Size",70);
		m_listCtrlLinkAngles.AddColumnHeader("Number",50);
		m_listCtrlLinkAngles.AddColumnHeader("Rows",60);
		m_listCtrlLinkAngles.AddColumnHeader("Layout",80);
	}
#else
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("肋板列表:");
		m_listCtrlLinkAngles.AddColumnHeader("序号",55);
		m_listCtrlLinkAngles.AddColumnHeader("材质",60);
		m_listCtrlLinkAngles.AddColumnHeader("编号",60);
		m_listCtrlLinkAngles.AddColumnHeader("厚度",60);
		if(foottempl.base.shape==1)	//圆底板时肋板为环绕布置,均为射线半径及射线角定位
		{
			m_listCtrlLinkAngles.AddColumnHeader("射线角°",80);
			m_listCtrlLinkAngles.AddColumnHeader("射线距离",80);
		}
	}
	else
	{
		GetDlgItem(IDC_S_TOP_LIST_TITLE2)->SetWindowText("连接角钢列表:");
		m_listCtrlLinkAngles.AddColumnHeader("位置标识",105);
		m_listCtrlLinkAngles.AddColumnHeader("句柄",55);
		m_listCtrlLinkAngles.AddColumnHeader("螺栓规格",70);
		m_listCtrlLinkAngles.AddColumnHeader("数量",50);
		m_listCtrlLinkAngles.AddColumnHeader("排数",60);
		m_listCtrlLinkAngles.AddColumnHeader("布孔方式",80);
	}
#endif
	m_listCtrlLinkAngles.InitListCtrl();
	m_listCtrlLinkAngles.DeleteAllItems();
	for(TREEITEM_INFO *pItemInfo=itemInfoList.GetFirst();pItemInfo;pItemInfo=itemInfoList.GetNext())
	{
		if(pItemInfo->itemType==ITEM_TYPE_RIBPLATE||pItemInfo->itemType==ITEM_TYPE_LINKANGLE)
			itemInfoList.DeleteCursor(FALSE);
	}
	itemInfoList.Clean();
	if(m_iDispType==DISP_BASE_PLATE || m_iDispType==DISP_OTHER)
	{
		for(CFootTempl::LINKANGLE* pLinkAngle=foottempl.base.linkAngles.GetFirst();pLinkAngle;pLinkAngle=foottempl.base.linkAngles.GetNext())
		{
			if(!pLinkAngle->cStateFlag.IsFromModel())
				continue;
			lpInfo = new CListCtrlItemInfo();
			//位置标识
			itemText.Format("%s",&pLinkAngle->PosLabel());
			lpInfo->SetSubItemText(0,itemText,TRUE);
			//角钢句柄
			itemText.Format("0X%X",pLinkAngle->pAngle->handle);
			lpInfo->SetSubItemText(1,itemText,FALSE);
			//螺栓直径
			itemText.Format("M%d",pLinkAngle->bolt.d);
			lpInfo->SetSubItemText(2,itemText,FALSE);
			lpInfo->SetControlType(2,GCT_CMB_EDIT);
			lpInfo->SetListItemsStr(2,"M12|M16|M20|M24");
			//螺栓数量
			itemText.Format("%d",pLinkAngle->bolt.num);
			lpInfo->SetSubItemText(3,itemText,FALSE);
			//螺栓排数
#ifdef AFX_TARG_ENU_ENGLISH
			if(pLinkAngle->bolt.rows==2)
				itemText="1.Double";
			else //if(pLinkAngle->bolt.rows==1)
				itemText="0.Singular";
#else
			if(pLinkAngle->bolt.rows==2)
				itemText="1.双排";
			else //if(pLinkAngle->bolt.rows==1)
				itemText="0.单排";
#endif
			lpInfo->SetSubItemText(4,itemText,FALSE);
			lpInfo->SetControlType(4,GCT_CMB_EDIT);
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->SetListItemsStr(4,"0.Singular|1.Double");
			//布置方式
			if(pLinkAngle->bolt.layout==0)
				itemText="0.Left High Right Low";
			else
				itemText="1.Left Low Right High";
			lpInfo->SetSubItemText(5,itemText,FALSE);
			lpInfo->SetControlType(5,GCT_CMB_EDIT);
			lpInfo->SetListItemsStr(5,"0.Left High Right Low|1.Left Low Right High");
#else
			lpInfo->SetListItemsStr(4,"0.单排|1.双排");
			//布置方式
			if(pLinkAngle->bolt.layout==0)
				itemText="0.左高右低";
			else
				itemText="1.左低右高";
			lpInfo->SetSubItemText(5,itemText,FALSE);
			lpInfo->SetControlType(5,GCT_CMB_EDIT);
			lpInfo->SetListItemsStr(5,"0.左高右低|1.左低右高");
#endif

			pItem = m_listCtrlLinkAngles.InsertRootItem(lpInfo);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(ITEM_TYPE_LINKANGLE,(DWORD)pLinkAngle));
			pItem->m_idProp=(long)pItemInfo;
		}
	}
	else if(m_iDispType==DISP_RIB_PLATE)
	{
		CSuperGridCtrl::CTreeItem *pSelectedItem=NULL;
		for(CFootTempl::RIBPLATE* pRibPlate=foottempl.listRibPlates.GetFirst();pRibPlate;pRibPlate=foottempl.listRibPlates.GetNext())
		{
			lpInfo = new CListCtrlItemInfo();
			//序号标识
			itemText.Format("%d",pRibPlate->Serial());
			lpInfo->SetSubItemText(0,itemText,TRUE);
			//材质
			STEELMAT *pSteelMat=QuerySteelMatByBriefMark(pRibPlate->cMaterial);
			itemText=pSteelMat->mark;
			lpInfo->SetSubItemText(1,itemText,FALSE);
			lpInfo->SetControlType(1,GCT_CMB_LIST);
			lpInfo->SetListItemsStr(1,MakeMaterialMarkSetString());
			//编号
			lpInfo->SetSubItemText(2,pRibPlate->sPartNo,FALSE);
			//板厚
			itemText.Format("%d",pRibPlate->thick);
			lpInfo->SetSubItemText(3,itemText,FALSE);
			if(foottempl.base.shape==1)	//圆底板时肋板为环绕布置,均为射线半径及射线角定位
			{	
				if(pRibPlate->axisXType==0)
				{	//射线角
					itemText.Format("%f",pRibPlate->pos_para.PARA2.angle);
					SimplifiedNumString(itemText);
					lpInfo->SetSubItemText(4,itemText,TRUE);
					//射线距离
					lpInfo->SetSubItemText(5,"-",TRUE);
				}
				else if(pRibPlate->axisXType==5)
				{	//射线角
					itemText.Format("%f",pRibPlate->pos_para.PARA3.angle);
					SimplifiedNumString(itemText);
					lpInfo->SetSubItemText(4,itemText,TRUE);
					//射线距离
					itemText.Format("%f",pRibPlate->pos_para.PARA3.fRayDist);
					SimplifiedNumString(itemText);
					lpInfo->SetSubItemText(5,itemText,TRUE);
				}
			}
			pItem = m_listCtrlLinkAngles.InsertRootItem(lpInfo);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(ITEM_TYPE_RIBPLATE,pRibPlate->Serial()));
			pItem->m_idProp=(long)pItemInfo;
			if(pSelectedItemData==pRibPlate)
				pSelectedItem=pItem;
		}
		if(pSelectedItem)
			m_listCtrlLinkAngles.SelectItem(pSelectedItem);
	}
	else if(m_iDispType==DISP_SHOE_PLATE && pShoePlate!=NULL)
	{
		for(CFootTempl::LINKANGLE* pLinkAngle=pShoePlate->linkAngles.GetFirst();pLinkAngle;pLinkAngle=pShoePlate->linkAngles.GetNext())
		{
			if(!pLinkAngle->cStateFlag.IsFromModel())
				continue;
			pLinkAngle->pLinkPlate=pShoePlate;
			lpInfo = new CListCtrlItemInfo();
			//位置标识
			itemText.Format("%s",(char*)pLinkAngle->PosLabel());
			lpInfo->SetSubItemText(0,itemText,TRUE);
			//角钢句柄
			itemText.Format("0X%X",pLinkAngle->pAngle->handle);
			lpInfo->SetSubItemText(1,itemText,FALSE);
			//螺栓直径
			itemText.Format("M%d",pLinkAngle->bolt.d);
			lpInfo->SetSubItemText(2,itemText,FALSE);
			lpInfo->SetControlType(2,GCT_CMB_EDIT);
			lpInfo->SetListItemsStr(2,"M12|M16|M20|M24");
			//螺栓数量
			itemText.Format("%d",pLinkAngle->bolt.num);
			lpInfo->SetSubItemText(3,itemText,FALSE);
			//螺栓排数
#ifdef AFX_TARG_ENU_ENGLISH
			if(pLinkAngle->bolt.rows==2)
				itemText="1.Double";
			else //if(pLinkAngle->bolt.rows==1)
				itemText="0.Singular";
			lpInfo->SetSubItemText(4,itemText,FALSE);
			lpInfo->SetControlType(4,GCT_CMB_LIST);
			lpInfo->SetListItemsStr(4,"0.Singular|1.Double");
			//布置方式
			if(pLinkAngle->bolt.layout==0)
				itemText="0.Left High Right Low";
			else
				itemText="1.Left Low Right High";
			lpInfo->SetSubItemText(5,itemText,FALSE);
			lpInfo->SetControlType(5,GCT_CMB_LIST);
			lpInfo->SetListItemsStr(5,"0.Left High Right Low|1.Left Low Right High");
#else
			if(pLinkAngle->bolt.rows==2)
				itemText="1.双排";
			else //if(pLinkAngle->bolt.rows==1)
				itemText="0.单排";
			lpInfo->SetSubItemText(4,itemText,FALSE);
			lpInfo->SetControlType(4,GCT_CMB_LIST);
			lpInfo->SetListItemsStr(4,"0.单排|1.双排");
			//布置方式
			if(pLinkAngle->bolt.layout==0)
				itemText="0.左高右低";
			else
				itemText="1.左低右高";
			lpInfo->SetSubItemText(5,itemText,FALSE);
			lpInfo->SetControlType(5,GCT_CMB_LIST);
			lpInfo->SetListItemsStr(5,"0.左高右低|1.左低右高");
#endif
			pItem = m_listCtrlLinkAngles.InsertRootItem(lpInfo);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(ITEM_TYPE_LINKANGLE,(DWORD)pLinkAngle));
			pItem->m_idProp=(long)pItemInfo;
		}
	}
}
void CFootDesignTemplDlg::RefreshShoePlateListCtrl()
{
	//1、更新列表标题
	m_listCtrlShoePlates.EmptyColumnHeader();
#ifdef AFX_TARG_ENU_ENGLISH
	if(m_iDispType==DISP_BASE_PLATE||m_iDispType==DISP_RIB_PLATE)
	{
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("Base Hole List:");
		m_listCtrlShoePlates.AddColumnHeader("Size",90);
		m_listCtrlShoePlates.AddColumnHeader("Hole",90);
		m_listCtrlShoePlates.AddColumnHeader("X",80);
		m_listCtrlShoePlates.AddColumnHeader("Y",80);
	}
	else
	{
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("Shoe Plate List:");
		m_listCtrlShoePlates.AddColumnHeader("Pos Label",110);
		m_listCtrlShoePlates.AddColumnHeader("Material",50);
		m_listCtrlShoePlates.AddColumnHeader("Part Label",60);
		m_listCtrlShoePlates.AddColumnHeader("Thick",45);
		m_listCtrlShoePlates.AddColumnHeader("Fillet",70);
	}
#else
	if(m_iDispType==DISP_BASE_PLATE||m_iDispType==DISP_RIB_PLATE)
	{
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("底脚孔列表:");
		m_listCtrlShoePlates.AddColumnHeader("规格",80);
		m_listCtrlShoePlates.AddColumnHeader("孔径",60);
		m_listCtrlShoePlates.AddColumnHeader("X",50);
		m_listCtrlShoePlates.AddColumnHeader("Y",50);
		m_listCtrlShoePlates.AddColumnHeader("",100);	//添加一列仅为便于选中当前行
	}
	else
	{
		GetDlgItem(IDC_S_TOP_LIST_TITLE)->SetWindowText("靴板列表:");
		m_listCtrlShoePlates.AddColumnHeader("位置标识",95);
		m_listCtrlShoePlates.AddColumnHeader("材质",45);
		m_listCtrlShoePlates.AddColumnHeader("编号",50);
		m_listCtrlShoePlates.AddColumnHeader("厚度",40);
		m_listCtrlShoePlates.AddColumnHeader("倒角值",53);
		m_listCtrlShoePlates.AddColumnHeader("法线偏移",65);
	}
#endif
	m_listCtrlShoePlates.InitListCtrl();
	//2、更新列表内容
	CString itemText;
	CListCtrlItemInfo* lpInfo = NULL;
	CSuperGridCtrl::CTreeItem* pItem=NULL;
	m_listCtrlShoePlates.DeleteAllItems();
	for(TREEITEM_INFO *pItemInfo=itemInfoList.GetFirst();pItemInfo;pItemInfo=itemInfoList.GetNext())
	{
		if(pItemInfo->itemType==ITEM_TYPE_HOLE||pItemInfo->itemType==ITEM_TYPE_SHOEPLATE)
			itemInfoList.DeleteCursor(FALSE);
	}
	itemInfoList.Clean();
	if(m_iDispType==DISP_OTHER||m_iDispType==DISP_SHOE_PLATE)	
	{	//提取靴板信息
		BOOL bReadOnly=FALSE;
		if(m_iDispType==DISP_OTHER)
			bReadOnly=TRUE;
		for(CFootTempl::SHOEPLATE* pShoePlate=foottempl.listShoePlates.GetFirst();pShoePlate;pShoePlate=foottempl.listShoePlates.GetNext())
		{
			if(!pShoePlate->cStateFlag.IsFromModel())
				continue;
			//位置标识
			lpInfo = new CListCtrlItemInfo();
			itemText.Format("%s",(char*)pShoePlate->PosLabel());
			lpInfo->SetSubItemText(0,itemText,TRUE);
			//材质
			STEELMAT *pSteelMat=QuerySteelMatByBriefMark(pShoePlate->cMaterial);
			itemText=pSteelMat->mark;
			lpInfo->SetSubItemText(1,itemText,bReadOnly);
			lpInfo->SetControlType(1,GCT_CMB_LIST);
			lpInfo->SetListItemsStr(1,MakeMaterialMarkSetString());
			//编号
			//itemText.Format("%s",pShoePlate->sPartNo);
			lpInfo->SetSubItemText(2,pShoePlate->sPartNo,bReadOnly);
			//板厚
			itemText.Format("%d",pShoePlate->thick);
			lpInfo->SetSubItemText(3,itemText,bReadOnly);
			//切角宽度
			itemText.Format("%f",pShoePlate->fCutAngleWidth);
			SimplifiedNumString(itemText);
			lpInfo->SetSubItemText(4,itemText,bReadOnly);
			//沿法线方向偏移距离(对焊的时候考虑此变量)
			BOOL bRO=bReadOnly;
			if(!bRO)
			{
				CXhChar16 label(pShoePlate->label.Label());
				if((label[0]=='U'&&foottempl.upLinkSP.style_bolt0_weld1!=1)||
					(label[1]=='D'&&foottempl.downLinkSP.style_bolt0_weld1!=1))	//非对焊
					bRO=TRUE;
				else if(strstr(label,"B"))	//连接肢处的靴板
					bRO=TRUE;
			}
			itemText.Format("%d",pShoePlate->norm_offset);
			lpInfo->SetSubItemText(5,itemText,bRO);
			//
			pItem = m_listCtrlShoePlates.InsertRootItem(lpInfo);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(ITEM_TYPE_SHOEPLATE,(DWORD)pShoePlate));
			pItem->m_idProp=(long)pItemInfo;
		}
	}
	else if(m_iDispType==DISP_BASE_PLATE||m_iDispType==DISP_RIB_PLATE)
	{	//提取底板孔信息
		BOOL bReadOnly=FALSE;
		//if(m_iDispType==DISP_RIB_PLATE)
		//	bReadOnly=TRUE;
		for(LIST_NODE<CFootTempl::BASEPLATE::HOLE> *pHole=foottempl.base.listHoles.EnumFirst();pHole;pHole=foottempl.base.listHoles.EnumNext())
		{
			//规格
			lpInfo = new CListCtrlItemInfo();
			itemText.Format("M%d",pHole->data.d);
			lpInfo->SetSubItemText(0,itemText,bReadOnly);
			lpInfo->SetControlType(0,GCT_CMB_EDIT);
			lpInfo->SetListItemsStr(0,"M12|M16|M20|M24|M27|M30|M36|M42|M45|M48|M52|M56|M60|M64");
			//孔径
			itemText.Format("%d",pHole->data.hole_d);
			lpInfo->SetSubItemText(1,itemText,bReadOnly);
			//坐标X
			itemText.Format("%d",pHole->data.x);
			SimplifiedNumString(itemText);
			lpInfo->SetSubItemText(2,itemText,bReadOnly);
			//坐标Y
			itemText.Format("%d",pHole->data.y);
			SimplifiedNumString(itemText);
			lpInfo->SetSubItemText(3,itemText,bReadOnly);
			pItem=m_listCtrlShoePlates.InsertRootItem(lpInfo);
			TREEITEM_INFO *pItemInfo=itemInfoList.append(TREEITEM_INFO(ITEM_TYPE_HOLE,(DWORD)pHole));
			pItem->m_idProp=(long)pItemInfo;
		}
	}
}
void CFootDesignTemplDlg::UpdateShoePlateInfo(CFootTempl::SHOEPLATE* pShoePlate)
{
	if(pShoePlate==NULL)
		return;
	for(CFootTempl::SHOEPLATE* pTmpPlate=foottempl.listShoePlates.GetFirst();pTmpPlate;pTmpPlate=foottempl.listShoePlates.GetNext())
	{
		if(pTmpPlate->label.CharAt(0)!=pShoePlate->label.CharAt(0))
			continue;
		if(pTmpPlate->label.IsEqual(pShoePlate->label))
			continue;
		pTmpPlate->cMaterial=pShoePlate->cMaterial;
		pTmpPlate->thick=pShoePlate->thick;
	}
}
void CFootDesignTemplDlg::OnAddHole()
{
	static CAddDiJiaoLsDlg addHoledlg;
	if(foottempl.base.shape==1)
	{	//圆塔脚默认从22.5°布螺栓孔
		addHoledlg.m_bEnableRayAngleMode=true;
		addHoledlg.m_fY=22.5;
		addHoledlg.m_nMirN=8;
	}
	else
	{
		addHoledlg.m_bEnableRayAngleMode=false;
		addHoledlg.m_fY=addHoledlg.m_fX;
		addHoledlg.m_nMirN=4;
	}
	if(addHoledlg.DoModal()!=IDOK)
		return;
	double len,angle;
	if(addHoledlg.m_bEnableRayAngleMode)
	{
		len=fabs(addHoledlg.m_fX);
		angle=22.5*RADTODEG_COEF;
	}
	else
	{
		f3dPoint center(addHoledlg.m_fX,addHoledlg.m_fY,0);
		len=center.mod();
		angle=ACOS(addHoledlg.m_fX/len);
		if(addHoledlg.m_fY<0)
			angle*=-1.0;	//Y坐标为负 起始角也应该取负
	}
	for(int i=0;i<addHoledlg.m_nMirN;i++)
	{
		f3dPoint pt=GetPtInPolar(f3dPoint(0,0,0),angle+2*i*Pi/addHoledlg.m_nMirN,len);
		CFootTempl::BASEPLATE::HOLE  ls_hole;
		ls_hole.d=atoi(addHoledlg.m_sLsGuiGe);
		ls_hole.hole_d=ftoi(addHoledlg.m_fHoleD);
		ls_hole.x=ftoi(pt.x);
		ls_hole.y=ftoi(pt.y);
		foottempl.base.listHoles.AttachNode(ls_hole);
	}
	//
	RefreshShoePlateListCtrl();
	g_pDraw->BuildDisplayList(this);
}
void CFootDesignTemplDlg::OnAddLinkAngle()
{

}
void CFootDesignTemplDlg::OnPaint()
{
	CPaintDC dc(this);
	g_pDraw->Draw();
}
void CFootDesignTemplDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//锁定光标在当前窗口
	CRect rect;
	CWnd *pWnd=GetDlgItem(IDC_S_MAP);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	if(!rect.PtInRect(point))
		return;
	pWnd->SetFocus();	//否则无法方便获取屏幕焦点，进而无法通过滚轮进行缩放
	SetCapture();
	BOOL bAltKeyDown=GetKeyState(VK_MENU)&0x8000;
	/*if(nFlags&MK_SHIFT)
	{
		m_xSelectObjects.Empty();
		g_pSolidDraw->ReleaseSnapStatus();
		UpdateProperty(DISPLAY_CONFIG,NULL);
	}*/
	g_pSolidOper->LMouseDown(point);
	CDialog::OnLButtonDown(nFlags, point);
}
void CFootDesignTemplDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	g_pOper->LMouseUp(point);
	if(GetCapture()==this)
		ReleaseCapture();
	CDialog::OnLButtonUp(nFlags, point);
}
void CFootDesignTemplDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	g_pOper->MouseMove(point,nFlags);
	CDialog::OnMouseMove(nFlags, point);
}
BOOL CFootDesignTemplDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	ZOOM_CENTER_STYLE zoom_style;
	g_pSet->GetZoomStyle(&zoom_style);
	g_pSet->SetZoomStyle(MOUSE_CENTER);
	if(zDelta>0)
		g_pOper->Scale(1.4);
	else
		g_pOper->Scale(1/1.4);
	g_pOper->RefreshScope();
	g_pSet->SetZoomStyle(zoom_style);
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}
void CFootDesignTemplDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	g_pOper->RMouseDown(point);
	g_pSet->SetOperType(OPER_OTHER);
	g_pDraw->ReleaseSnapStatus();
	//m_xSelectObjects.Empty();
}
void CFootDesignTemplDlg::OnBnPan()
{
	g_pSet->SetOperType(OPER_PAN);
}
void CFootDesignTemplDlg::OnBnOsnap()
{
	g_pSet->SetOperType(OPER_OSNAP);
}
void CFootDesignTemplDlg::OnBnZoom()
{
	g_pSet->SetOperType(OPER_ZOOM);
}
void CFootDesignTemplDlg::OnBnOther()
{
	g_pSet->SetOperType(OPER_OTHER);
}
#endif

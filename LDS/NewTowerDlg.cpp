//<LOCALE_TRANSLATE Confirm by hxr/>
// NewTowerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lds.h"
#include "GlobalFunc.h"
#include "NewTowerDlg.h"
#include "SelPanelLayoutDlg.h"
#include "StdFacePanel.h"
#include "comdef.h"
#include "Expression.h"
#include "LogFile.h"
#include "NewTowerDataOper.h"
#include "UI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
static CShankLeg *f_pActiveLeg;
void DrawTowerSketch(CDrawEngine* pDraw)
{
	CNewTowerDlg *pNewTowerDlg=(CNewTowerDlg*)pDraw->dwParam;
	int iViewType=pNewTowerDlg->m_iViewType;	//0.正面视图;1.侧面视图;2.透视图
	CShankDefine *pDefine=&pNewTowerDlg->m_xShankDef;
	char sText[100]="";
	CShankLeg *pLeg=NULL;
	CShankModule *pModule=NULL;
	CShankDefineObject *pDefObj=pNewTowerDlg->GetSelItemRelaObject();
	if(pDefObj!=NULL&&pDefObj->cObjType=='L')
		pLeg=(CShankLeg*)pDefObj;
	if(pLeg!=NULL)
	{
		pModule=pLeg->ParentModule();
		f_pActiveLeg=pLeg;
	}
	else
		pLeg=f_pActiveLeg;
	f3dLine line;
	CTower localTower;
	CShankDefineToTowerModel::m_iViewType=iViewType;
	CFGLEG::SetSchema(pNewTowerDlg->m_iMultiLegSchema+1);
	CStackVariant<BOOL> stack(&g_sysPara.m_bAnglePreferSimplePosMode, FALSE);
	CStackVariant<bool> stack1(&UI::blEnableIntermediateUpdateUI,false);
	CShankDefineToTowerModel modeler;
	if(!modeler.UpdateTowerModel(&localTower,pDefine))
		return;
	//根据视图类型设定塔显示坐标系
	UCS_STRU cs,std_cs;
	LoadDefaultUCS(&std_cs);
	if(iViewType==0||iViewType==2)	//正面视图或透视图
	{
		cs.axis_x.Set(1,0,0);
		cs.axis_y.Set(0,0,1);
		cs.axis_z.Set(0,-1,0);
	}
	else
	{
		cs.axis_x.Set(0,0,1);
		cs.axis_y.Set(-1,0,0);
		cs.axis_z.Set(0,-1,0);
	}
	pDraw->ClearAllCS();
	pDraw->SetObjectUcs(cs);
	pDraw->NewCS(std_cs,"X","Y","Z","",1);
	CFGWORD bodyword,legword;
	if(pModule)
		bodyword=CFGLEG((BYTE)pModule->m_iBodyNo,1).ToCfgWord();
	if(pLeg)
		legword=pLeg->legword.ToCfgWord();
	for(CLDSLinePart *pRod=localTower.Parts.GetFirstLinePart();pRod;pRod=localTower.Parts.GetNextLinePart())
	{
		if( (pLeg&&!legword.And(pRod->cfgword)&&pRod->IsLegObj())||
			(pModule&&!bodyword.And(pRod->cfgword)&&!pRod->IsLegObj()))
			continue;	//不属于当前呼高
		if(iViewType==0&&(pRod->layer(2)!='Q'&&pRod->layer(2)!='1'&&pRod->layer(2)!='2'))
			continue;	//不是正面杆件
		else if(iViewType==1&&(pRod->layer(2)!='Y'&&pRod->layer(2)!='1'&&pRod->layer(2)!='3'))
			continue;	//不是右侧面杆件
		line.startPt=pRod->pStart->Position(false);
		line.endPt=pRod->pEnd->Position(false);
		pDraw->NewLine(line);
	}
	SCOPE_STRU scope;
	localTower.GetTaSizeScope(scope);
	line.pen.crColor=RGB(0,0,255);	//标注线用蓝色
	for(SKETCH_SIZEDIM *pSizeDim=CShankDefineToTowerModel::dims.GetFirst();pSizeDim;pSizeDim=CShankDefineToTowerModel::dims.GetNext())
	{
		CFGWORD dimCfgWord = pSizeDim->cfgword;
		if( (pLeg&&!legword.And(dimCfgWord)&&pSizeDim->bLegDim)||
			(pModule&&!bodyword.And(dimCfgWord)&&!pSizeDim->bLegDim))
			continue;	//不属于当前呼高
		if(iViewType==0&&pSizeDim->cSizeType=='Y')
			continue;	//不是正面杆件
		else if(iViewType==1&&(pSizeDim->cSizeType=='X'))
			continue;	//不是右侧面杆件
		if(pSizeDim->cSizeType=='Z')
			pSizeDim->wAlignStyle=CDrawEngine::MiddleCenter;
		CXhChar16 dimText;
		dimText.Printf("%.0f",pSizeDim->Size());
		//绘制尺寸标注线
		double drawTextLen=0;
		pDraw->CalTextDrawSize(dimText,pNewTowerDlg->m_fTextHeight,&drawTextLen);
		cs.axis_x=pSizeDim->dimEnd-pSizeDim->dimStart;
		f3dPoint dim_align_vec=cs.axis_x;
		if(pSizeDim->cSizeType=='X')
		{
			if(cs.axis_x.x>0)
				cs.axis_x.Set(1,0,0);
			else
				cs.axis_x.Set(-1,0,0);
			cs.axis_y.Set(0,0,-1);
		}
		else if(pSizeDim->cSizeType=='Y')
		{
			if(cs.axis_x.y>0)
				cs.axis_x.Set(0,1,0);
			else
				cs.axis_x.Set(0,-1,0);
			cs.axis_y.Set(0,0,-1);
		}
		else if(pSizeDim->cSizeType=='Z')	//垂高标注需要根据归属分层确定标注点X坐标
		{
			if(cs.axis_x.z>0)
				cs.axis_x.Set(0,0,1);
			else
				cs.axis_x.Set(0,0,-1);
			if(iViewType==1)	//右侧面视图
			{
				cs.axis_y.Set(0,1,0);
				if(pSizeDim->cascade==1)		//第一分层，即分段垂高标注
					pSizeDim->pos.y=scope.fMaxY+pNewTowerDlg->m_fTextHeight*3;	//左移三行字高
				else if(pSizeDim->cascade==2)	//第二分层，坡段垂高标注
					pSizeDim->pos.y=scope.fMaxY+pNewTowerDlg->m_fTextHeight*5;	//左移五行字高
			}
			else
			{
				cs.axis_y.Set(-1,0,0);
				if(pSizeDim->cascade==1)		//第一分层，即分段垂高标注
					pSizeDim->pos.x=scope.fMinX-pNewTowerDlg->m_fTextHeight*3;	//左移三行字高
				else if(pSizeDim->cascade==2)	//第二分层，坡段垂高标注
					pSizeDim->pos.x=scope.fMinX-pNewTowerDlg->m_fTextHeight*5;	//左移五行字高
			}
		}
		else //if(pSizeDim->cSizeType=='L')
		{
			normalize(cs.axis_x);
			cs.axis_y=pSizeDim->pos-pSizeDim->dimStart;
			cs.axis_z=cs.axis_x^cs.axis_y;
			if(normalize(cs.axis_z))
				cs.axis_y=cs.axis_z^cs.axis_x;
			else
			{
				cs.axis_y=inters_vec(cs.axis_x);
				cs.axis_z=cs.axis_x^cs.axis_y;
				normalize(cs.axis_z);
				cs.axis_y=cs.axis_z^cs.axis_x;
			}
		}
		cs.origin=pSizeDim->pos;
		f3dPoint vert_axis=cs.axis_y;
		if((pSizeDim->pos-pSizeDim->dimStart)*cs.axis_y<0)
			vert_axis=-cs.axis_y;
		double dim_len=fabs(dim_align_vec*cs.axis_x);
		line.startPt=cs.origin-cs.axis_x*(dim_len*0.5)+vert_axis*pNewTowerDlg->m_fTextHeight*0.5;
		if(pSizeDim->cSizeType=='Z')
			line.endPt=line.startPt-cs.axis_y*(pNewTowerDlg->m_fTextHeight*2);
		else
			line.endPt=pSizeDim->dimStart;
		pDraw->NewLine(line);	//始端边界线
		line.startPt=cs.origin+cs.axis_x*(dim_len*0.5)+vert_axis*pNewTowerDlg->m_fTextHeight*0.5;
		if(pSizeDim->cSizeType=='Z')
			line.endPt=line.startPt-cs.axis_y*(pNewTowerDlg->m_fTextHeight*2);
		else
			line.endPt=pSizeDim->dimEnd;
		pDraw->NewLine(line);	//终端边界线
		if(pSizeDim->wAlignStyle==CDrawEngine::BottomCenter)
		{
			line.startPt=cs.origin-cs.axis_x*(dim_len*0.5)-0.2*pNewTowerDlg->m_fTextHeight*cs.axis_y;
			line.endPt=line.startPt+cs.axis_x*dim_len;
			pDraw->NewLine(line);	//始端侧基准线
		}
		else if(drawTextLen+pNewTowerDlg->m_fTextHeight*2<dim_len)
		{	//尺寸允许绘制标注基准线
			line.startPt=cs.origin-cs.axis_x*(dim_len*0.5);
			line.endPt=cs.origin-cs.axis_x*(drawTextLen*0.5+pNewTowerDlg->m_fTextHeight);
			pDraw->NewLine(line);	//始端侧基准线
			line.endPt=cs.origin+cs.axis_x*(dim_len*0.5);
			line.startPt=cs.origin+cs.axis_x*(drawTextLen*0.5+pNewTowerDlg->m_fTextHeight);
			pDraw->NewLine(line);	//始端侧基准线
		}
		//绘制文本
		pDraw->NewText(dimText,pSizeDim->pos,pNewTowerDlg->m_fTextHeight,pSizeDim->align_vec,pSizeDim->norm,pSizeDim->wAlignStyle);
	}
}
//坡段数据(K)可带有分段数据(D)或直接带有呼高分组数据(B)(之前不能存在分段数据) wht 10-07-23  
//分段段数据(D)只能带有呼高分组数据(B)
//呼高分组数据(B)可带有分段数据(D)以及塔腿数据(L)
static BOOL FireContextMenu(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,CPoint point)
{
	CNewTowerDlg *pNewTowerDlg=(CNewTowerDlg*)pListCtrl->GetParent();
	if(pNewTowerDlg==NULL)
		return FALSE;
	CMenu popMenu;
	popMenu.LoadMenu(IDR_ITEM_CMD_POPUP);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	pMenu->DeleteMenu(0,MF_BYPOSITION);
	BOOL bInSelItemRect=pSelItem?TRUE:FALSE;
	//if(pSelItem)
	//{
	//	CRect rectItem;
	//	pListCtrl->GetItemRect(pSelItem->GetIndex(),rectItem,LVIR_BOUNDS);
	//	if(rectItem.PtInRect(point))
	//		bInSelItemRect=TRUE;	//鼠标不在当前选中项上
	//}
	if(!bInSelItemRect)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		pMenu->AppendMenu(MF_STRING,ID_NEW_SLOPE,"new slope data");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"import tower body definition file");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"import tower body definition file");
#else
		pMenu->AppendMenu(MF_STRING,ID_NEW_SLOPE,"新建坡段数据");
		pMenu->AppendMenu(MF_STRING,ID_IMPORT_XML_FILE,"导入塔身定义文件");
		pMenu->AppendMenu(MF_STRING,ID_EXPORT_XML_FILE,"导出塔身定义文件");
#endif
	}
	else //if(bInSelItemRect)
	{
		if(pSelItem==NULL)
			return FALSE;
		CShankDefineObject* pObj=(CShankDefineObject*)pSelItem->m_idProp;
		if(pObj->cObjType=='K') //开口
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_NEW_SEG,"new segment data");
			//if(pListCtrl->NumChildren(pSelItem)==0)Let go of the slope section, directly add module group limitation wjh-2013.2.26
			pMenu->AppendMenu(MF_STRING,ID_NEW_HUGAO_MODULE,"new module group");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"delete");
#else
			pMenu->AppendMenu(MF_STRING,ID_NEW_SEG,"新建分段数据");
			//if(pListCtrl->NumChildren(pSelItem)==0)放开坡段下直接加呼高分组数量限制 wjh-2013.2.26
			pMenu->AppendMenu(MF_STRING,ID_NEW_HUGAO_MODULE,"新建呼高分组");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除");
#endif
		}
		else if(pObj->cObjType=='D')//分段
		{
			//当前分段所属的父节点中不存在呼高，可建呼高分组
			CShankSegment* pSegment=pObj->Segment();
			if(!pSegment->IsSegParentsHasModule())
#ifdef AFX_TARG_ENU_ENGLISH
				pMenu->AppendMenu(MF_STRING,ID_NEW_HUGAO_MODULE,"new module group");
			pMenu->AppendMenu(MF_STRING,ID_NEW_SECTION,"new inter nodes");
			pMenu->AppendMenu(MF_STRING,ID_NEW_SEG,"new segment data");
			//pMenu->AppendMenu(MF_STRING,ID_INSERT_SEG,"insert segment");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"delete");
#else
				pMenu->AppendMenu(MF_STRING,ID_NEW_HUGAO_MODULE,"新建呼高分组");
			pMenu->AppendMenu(MF_STRING,ID_NEW_SECTION,"新建节间");
			pMenu->AppendMenu(MF_STRING,ID_NEW_SEG,"新建分段数据");
			//pMenu->AppendMenu(MF_STRING,ID_INSERT_SEG,"插入分段");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除");
#endif
		}
		else if(pObj->cObjType=='B')//呼高分组
		{	
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_NEW_SEG,"new segment data");
			pMenu->AppendMenu(MF_STRING,ID_NEW_LEG,"new tower leg data");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"delete");
#else
			pMenu->AppendMenu(MF_STRING,ID_NEW_SEG,"新建分段数据");
			pMenu->AppendMenu(MF_STRING,ID_NEW_LEG,"新建塔腿数据");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除");
#endif
		}
		else if(pObj->cObjType=='J')
		{	//节间
#ifdef AFX_TARG_ENU_ENGLISH
			pMenu->AppendMenu(MF_STRING,ID_INSERT_SECTION,"insert inter nodes ");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"delete");
		}
		else if(pObj->cObjType=='L')//leg connecting
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"delete");
#else
			pMenu->AppendMenu(MF_STRING,ID_INSERT_SECTION,"插入节间");
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除");
		}
		else if(pObj->cObjType=='L')//接腿
			pMenu->AppendMenu(MF_STRING,ID_DELETE_ITEM,"删除");
#endif
		else
			return FALSE;
	}
	CPoint menu_pos=point;
	pListCtrl->ClientToScreen(&menu_pos);
    CMenu *pmenuPopup = popMenu.GetSubMenu(0);
    ASSERT(pmenuPopup != NULL);
	pmenuPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,menu_pos.x,menu_pos.y,pNewTowerDlg);
	pmenuPopup->DestroyMenu();
	return TRUE;
}
//
static DWORD TrackLiveComboMenu(CWnd* pParentWnd, CPoint point)
{
	CNewTowerDlg *pNewTowerDlg=(CNewTowerDlg*)pParentWnd;
	if(pNewTowerDlg==NULL)
		return FALSE;
	CSuperGridCtrl::CTreeItem *pSelItem=NULL;
	POSITION pos=pNewTowerDlg->m_listBodyOpening.GetFirstSelectedItemPosition();
	if(pos!=NULL)
	{
		int iCurSel=pNewTowerDlg->m_listBodyOpening.GetNextSelectedItem(pos);
		pSelItem=pNewTowerDlg->m_listBodyOpening.GetTreeItem(iCurSel);
	}
	pNewTowerDlg->m_listBodyOpening.ScreenToClient(&point);
	BOOL bValid=FireContextMenu(&pNewTowerDlg->m_listBodyOpening,pSelItem,point);
	return bValid;
}
//
static BOOL FireDeleteItem(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem)
{
	CNewTowerDlg *pNewTowerDlg=(CNewTowerDlg*)pListCtrl->GetParent();
	return pNewTowerDlg->DeleteItemAndAttachData(pSelItem,CNewTowerDlg::DELETE_ATTACHDATA);
}
static BOOL FireValueModify(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,
							int iSubItem,CString& sTextValue)
{
	CShankDefineObject* pObj=(CShankDefineObject*)pSelItem->m_idProp;
	if(pObj->cObjType=='K'||pObj->cObjType=='D'||pObj->cObjType=='L'||pObj->cObjType=='J'||pObj->cObjType=='B')
	{
		pSelItem->m_lpNodeInfo->SetSubItemText(iSubItem,sTextValue);	//
		pListCtrl->SetItemText(pSelItem->GetIndex(),iSubItem,sTextValue.GetBuffer(1));
		if(iSubItem==0)
		{
			if(pObj->cObjType=='K')
				pObj->Slope()->m_fTopCoordZ=atof(sTextValue);
			else if(pObj->cObjType=='D')
				pObj->Segment()->m_fHeight=atof(sTextValue);
			else if(pObj->cObjType=='L')
				pObj->Leg()->m_fHeight=atof(sTextValue);
			else if(pObj->cObjType=='J')
				pObj->Section()->btm_zstr.Printf("%s",sTextValue);
			else if(pObj->cObjType=='B')
				pObj->Module()->name.Printf("%s",sTextValue);
			CNewTowerDlg *pNewTowerDlg=(CNewTowerDlg*)pListCtrl->GetParent();
			if(pNewTowerDlg) //修改坡段开口后需要更新右侧预览图像
				pNewTowerDlg->ReDrawTowerSketchView();
		}
		else if(iSubItem==1)
		{
			if(pObj->cObjType=='D')
				pObj->Segment()->m_iSeg=SEGI(sTextValue.GetBuffer());
			else if(pObj->cObjType=='L')
				pObj->Leg()->m_iSeg=SEGI(sTextValue.GetBuffer());
		}
	}
	return TRUE;
}
//
static BOOL FireItemChanged(CSuperGridCtrl* pListCtrl,CSuperGridCtrl::CTreeItem* pSelItem,NM_LISTVIEW* pNMListView)
{
	CNewTowerDlg *pNewTowerDlg=(CNewTowerDlg*)pListCtrl->GetParent();
	if(!pNewTowerDlg->m_bFireListItemChange)
		return FALSE;	//未启用ItemChange
	if(pSelItem==NULL)
	{
		pNewTowerDlg->m_xMenuBtn.NotifyComboMenuClicked(ID_NEW_SLOPE,"新建坡段数据");
		return TRUE;
	}
	CShankDefineObject* pObj=(CShankDefineObject*)pSelItem->m_idProp;
	if(pObj==NULL)
		return FALSE;
	pNewTowerDlg->DisplayPropertyList(pSelItem);
	if(pObj->cObjType=='K')			//开口
		pNewTowerDlg->m_xMenuBtn.NotifyComboMenuClicked(ID_NEW_SEG,"新建分段数据");
	else if(pObj->cObjType=='D')	//分段
		pNewTowerDlg->m_xMenuBtn.NotifyComboMenuClicked(ID_NEW_SECTION,"新建节间");
	else if(pObj->cObjType=='B'	)	//呼高分组
		pNewTowerDlg->m_xMenuBtn.NotifyComboMenuClicked(ID_NEW_LEG,"新建塔腿数据");
	else if(pObj->cObjType=='J')	//节间
		pNewTowerDlg->m_xMenuBtn.NotifyComboMenuClicked(ID_INSERT_SECTION,"插入节间");
	else if(pObj&&pObj->cObjType=='L')
	{	//选中不同分组时需要更新右侧预览图像
		pNewTowerDlg->m_xMenuBtn.NotifyComboMenuClicked(ID_DELETE_ITEM,"删除");
		pNewTowerDlg->ReDrawTowerSketchView();
	}
	pNewTowerDlg->m_xMenuBtn.Invalidate(FALSE);
	return TRUE;
}
///////////////////////////////////////
//属性栏定义
static BOOL ModifyItemValue(CPropertyList *pPropList,CPropTreeItem *pItem, CString &valueStr)
{
	CNewTowerDlg *pNewTowerDlg=(CNewTowerDlg*)pPropList->GetParent();
	CShankDefineObject* pObj=(CShankDefineObject*)pPropList->m_nObjClassTypeId;
	if(pItem->m_idProp==CNewTowerDlg::GetPropID("front_clearance"))	//正面开口
	{
		if(pObj->cObjType=='K')
			pObj->Slope()->m_fTopFrontWidth=pObj->Slope()->m_fTopSideWidth=atof(valueStr);
		else if(pObj->cObjType=='L')
			pObj->Leg()->m_fFrontWidth=pObj->Leg()->m_fSideWidth=atof(valueStr);
		else
			return FALSE;
		pPropList->SetItemPropValue(CNewTowerDlg::GetPropID("side_clearance"),valueStr);
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("side_clearance"))	//正面开口
	{
		if(pObj->cObjType=='K')
			pObj->Slope()->m_fTopSideWidth=atof(valueStr);
		else if(pObj->cObjType=='L')
			pObj->Leg()->m_fSideWidth=atof(valueStr);
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("mainrod_type"))	//主材类型
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Angle")==0)
			pObj->Slope()->m_cMainRodType='L';
		else if(valueStr.CompareNoCase("Tube")==0)
#else
		if(valueStr.CompareNoCase("角钢")==0)
			pObj->Slope()->m_cMainRodType='L';
		else if(valueStr.CompareNoCase("钢管")==0)
#endif
			pObj->Slope()->m_cMainRodType='T';
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("divide_n"))		//等分数
	{
		if(pObj->cObjType=='J')
			pObj->Section()->divide_n=atoi(valueStr);
		else if(pObj->cObjType=='D')
			pObj->Segment()->divide_n=atoi(valueStr);
		else if(pObj->cObjType=='L')
			pObj->Leg()->divide_n=atoi(valueStr);
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("front_layout"))	//侧面布材
	{
		if(pObj->cObjType=='J')
		{
			StrCopy(pObj->Section()->front_layout,valueStr,51);
			StrCopy(pObj->Section()->side_layout,valueStr,51);
		}
		else if(pObj->cObjType=='D')
		{
			StrCopy(pObj->Segment()->front_layout,valueStr,51);
			StrCopy(pObj->Segment()->side_layout,valueStr,51);
		}
		else if(pObj->cObjType=='L')
		{
			StrCopy(pObj->Leg()->front_layout,valueStr,51);
			StrCopy(pObj->Leg()->side_layout,valueStr,51);
		}
		else
			return FALSE;
		pPropList->SetItemPropValue(CNewTowerDlg::GetPropID("side_layout"),valueStr);
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("side_layout"))	//腿部K型布材
	{
		if(pObj->cObjType=='J')
			StrCopy(pObj->Section()->side_layout,valueStr,51);
		else if(pObj->cObjType=='D')
			StrCopy(pObj->Segment()->side_layout,valueStr,51);
		else if(pObj->cObjType=='L')
			StrCopy(pObj->Leg()->side_layout,valueStr,51);
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("hsect_layout"))	//横隔面布材
	{
		if(pObj->cObjType=='J')
			StrCopy(pObj->Section()->hsect_layout,valueStr,51);
		else if(pObj->cObjType=='D')
			StrCopy(pObj->Segment()->hsect_layout,valueStr,51);
		else if(pObj->cObjType=='L')
			StrCopy(pObj->Leg()->hsect_layout,valueStr,51);
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("v_layout"))	//V面布材
	{
		if(pObj->cObjType=='J')
			StrCopy(pObj->Section()->v_layout,valueStr,51);
		else if(pObj->cObjType=='D')
			StrCopy(pObj->Segment()->v_layout,valueStr,51);
		else if(pObj->cObjType=='L')
			StrCopy(pObj->Leg()->v_layout,valueStr,51);
	}
	/*else if(pItem->m_idProp==CNewTowerDlg::GetPropID("btm_zstr"))	//底部垂高位置
	{
		if(pObj->cObjType=='J')
			pObj->Section()->btm_zstr.Copy(valueStr);
	}*/
	pNewTowerDlg->ReDrawTowerSketchView();
	return TRUE;
}
static BOOL PropHelpFunc(CPropertyList *pPropList,CPropTreeItem *pItem)
{
	CNewTowerDlg *pNewTowerDlg=(CNewTowerDlg*)pPropList->GetParent();
	if(pNewTowerDlg==NULL)
		return FALSE;
	CShankDefineObject* pObj=(CShankDefineObject*)pPropList->m_nObjClassTypeId;
	CString labelStdPanel=pItem->m_lpNodeInfo->m_strPropValue;
	int iPanelType=1;
	if(pItem->m_idProp==CNewTowerDlg::GetPropID("front_layout")||
		pItem->m_idProp==CNewTowerDlg::GetPropID("side_layout"))
	{
		if(pObj->cObjType=='L'||(pObj->cObjType=='J'&&pObj->Section()->m_pParent->cObjType=='L'))
			iPanelType=6;
		else
			iPanelType=1;
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("hsect_layout"))
		iPanelType=2;
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("v_layout"))
		iPanelType=5;
	else
		return FALSE;
	//更新界面标准面示意图
	pNewTowerDlg->UpdateStdPanelSketch(iPanelType,labelStdPanel);
	return TRUE;
}
static BOOL ButtonClickItemProp(CPropertyList *pPropList,CPropTreeItem *pItem)
{
	CNewTowerDlg *pNewTowerDlg=(CNewTowerDlg*)pPropList->GetParent();
	if(pNewTowerDlg==NULL)
		return FALSE;
	CSelPanelLayoutDlg layoutdlg;
	CShankDefineObject* pObj=(CShankDefineObject*)pPropList->m_nObjClassTypeId;
	if(pItem->m_idProp==CNewTowerDlg::GetPropID("front_layout")||
		pItem->m_idProp==CNewTowerDlg::GetPropID("side_layout"))
	{
		if(pObj->cObjType=='L'||(pObj->cObjType=='J'&&pObj->Section()->m_pParent->cObjType=='L'))
			layoutdlg.m_iPanelType=6;
		else
			layoutdlg.m_iPanelType=1;
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("hsect_layout"))
	{
		layoutdlg.m_iPanelType=2;
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("v_layout"))
	{
		layoutdlg.m_iPanelType=5;
	}
	layoutdlg.m_sFacePanel=pItem->m_lpNodeInfo->m_strPropValue;
	if(layoutdlg.DoModal()!=IDOK)
		return FALSE;
	pNewTowerDlg->UpdateStdPanelSketch(layoutdlg.m_iPanelType,layoutdlg.m_sFacePanel);
	pPropList->SetItemPropValue(pItem->m_idProp,layoutdlg.m_sFacePanel);
	if(pItem->m_idProp==CNewTowerDlg::GetPropID("front_layout"))	//侧面布材
	{
		if(pObj->cObjType=='J')
		{
			strncpy(pObj->Section()->front_layout,layoutdlg.m_sFacePanel,51);
			strncpy(pObj->Section()->side_layout,layoutdlg.m_sFacePanel,51);
		}
		else if(pObj->cObjType=='D')
		{
			strncpy(pObj->Segment()->front_layout,layoutdlg.m_sFacePanel,51);
			strncpy(pObj->Segment()->side_layout,layoutdlg.m_sFacePanel,51);
		}
		else if(pObj->cObjType=='L')
		{
			strncpy(pObj->Leg()->front_layout,layoutdlg.m_sFacePanel,51);
			strncpy(pObj->Leg()->side_layout,layoutdlg.m_sFacePanel,51);
		}
		else
			return FALSE;
		pPropList->SetItemPropValue(CNewTowerDlg::GetPropID("side_layout"),layoutdlg.m_sFacePanel);
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("side_layout"))	//腿部K型布材
	{
		if(pObj->cObjType=='J')
			strncpy(pObj->Section()->side_layout,layoutdlg.m_sFacePanel,51);
		else if(pObj->cObjType=='D')
			strncpy(pObj->Segment()->side_layout,layoutdlg.m_sFacePanel,51);
		else if(pObj->cObjType=='L')
			strncpy(pObj->Leg()->side_layout,layoutdlg.m_sFacePanel,51);
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("hsect_layout"))	//横隔面布材
	{
		if(pObj->cObjType=='J')
			strncpy(pObj->Section()->hsect_layout,layoutdlg.m_sFacePanel,51);
		else if(pObj->cObjType=='D')
			strncpy(pObj->Segment()->hsect_layout,layoutdlg.m_sFacePanel,51);
		else if(pObj->cObjType=='L')
			strncpy(pObj->Leg()->hsect_layout,layoutdlg.m_sFacePanel,51);
	}
	else if(pItem->m_idProp==CNewTowerDlg::GetPropID("v_layout"))	//V面布材
	{
		if(pObj->cObjType=='J')
			strncpy(pObj->Section()->v_layout,layoutdlg.m_sFacePanel,51);
		else if(pObj->cObjType=='D')
			strncpy(pObj->Segment()->v_layout,layoutdlg.m_sFacePanel,51);
		else if(pObj->cObjType=='L')
			strncpy(pObj->Leg()->v_layout,layoutdlg.m_sFacePanel,51);
	}
	pNewTowerDlg->ReDrawTowerSketchView();
	return TRUE;
}
const DWORD HASHTABLESIZE = 500;
CHashStrTable<long> CNewTowerDlg::propHashtable;
void CNewTowerDlg::InitItemPropHashtable()
{
	int id = 1;
	propHashtable.CreateHashTable(HASHTABLESIZE);
	propHashtable.SetValueAt("front_clearance",id++);
	propHashtable.SetValueAt("side_clearance",id++);
	propHashtable.SetValueAt("mainrod_type",id++);
	propHashtable.SetValueAt("divide_n",id++);
	propHashtable.SetValueAt("front_layout",id++);
	propHashtable.SetValueAt("side_layout",id++);
	propHashtable.SetValueAt("hsect_layout",id++);
	propHashtable.SetValueAt("v_layout",id++);
	propHashtable.SetValueAt("btm_zstr",id++);
	propHashtable.SetValueAt("m_bLegSection",id++);
}
long CNewTowerDlg::GetPropID(char* propStr)
{
	long id;
	if(propHashtable.GetValueAt(propStr,id))
		return id;
	else
	{
		return 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CNewTowerDlg dialog

CNewTowerDlg::CNewTowerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewTowerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewTowerDlg)
	m_iMainPartType = 1;
	m_iSideNum = 1;
	m_iMultiLegSchema=0;
	m_iUnit = 1;
	m_fTextHeight = 300.0;
	//}}AFX_DATA_INIT
	m_bAutoCalTextHeight=FALSE;
	m_iOldUnit=1;
	m_bFireListItemChange=true;
	m_iViewType=0;	//显示正面图
	m_pDraw=NULL;
	rcRightMargin = rcBtmMargin = 5;
#ifdef AFX_TARG_ENU_ENGLISH
	m_listBodyOpening.AddColumnHeader("vertical high position");
	m_listBodyOpening.AddColumnHeader("segment No.");
#else
	m_listBodyOpening.AddColumnHeader("垂高位置");
	m_listBodyOpening.AddColumnHeader("段号");
#endif
	m_listBodyOpening.AddColumnHeader("");
	m_listBodyOpening.m_bFireOnNoneItemSelect=true;
}


void CNewTowerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewTowerDlg)
	DDX_Control(pDX, IDC_LIST_BOX, m_propList);
	DDX_Control(pDX, IDC_LIST_BODY_OPENING, m_listBodyOpening);
	//DDX_CBIndex(pDX, IDC_CMB_PART_TYPE, m_iMainPartType);
	DDX_CBIndex(pDX, IDC_COMBO_TOWER_SIDE_NUM, m_iSideNum);
	DDX_CBIndex(pDX, IDC_CMB_UNIT, m_iUnit);
	DDX_Text(pDX, IDC_E_TEXT_HEIGHT, m_fTextHeight);
	DDV_MinMaxDouble(pDX, m_fTextHeight, 1., 100000.);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_CMB_LEG_SCHEMA, m_iMultiLegSchema);
	DDV_MinMaxInt(pDX, m_iMultiLegSchema, 0, 2);
	DDX_Control(pDX, IDC_MENU_BUTTON, m_xMenuBtn);
}


BEGIN_MESSAGE_MAP(CNewTowerDlg, CDialog)
	//{{AFX_MSG_MAP(CNewTowerDlg)
	ON_CBN_SELCHANGE(IDC_CMB_UNIT, OnSelchangeUnit)
	ON_CBN_SELCHANGE(IDC_COMBO_TOWER_SIDE_NUM, OnSelchangeTowerSideNum)
	ON_WM_PAINT()
	ON_COMMAND(ID_NEW_SLOPE, OnNewSlope)
	ON_COMMAND(ID_NEW_SEG, OnNewSeg)
	ON_COMMAND(ID_NEW_HUGAO_MODULE, OnNewModule)
	ON_COMMAND(ID_NEW_LEG, OnNewLeg)
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	ON_COMMAND(ID_IMPORT_XML_FILE, OnImportXMLFile)
	ON_COMMAND(ID_EXPORT_XML_FILE, OnExportXMLFile)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_FRONT_VIEW, OnBtnFrontView)
	ON_BN_CLICKED(IDC_BTN_PERSPECTIVE_VIEW, OnBtnPerspectiveView)
	ON_BN_CLICKED(IDC_BTN_SIDE_VIEW, OnBtnSideView)
	ON_COMMAND(ID_DRAW3D_ZOOM_ALL, OnDraw3dZoomAll)
	ON_COMMAND(ID_DRAW3D_ROTATE, OnDraw3dRotate)
	ON_COMMAND(ID_DRAW3D_ZOOM, OnDraw3dZoom)
	ON_COMMAND(ID_DRAW3D_RESET, OnDraw3dReset)
	ON_COMMAND(ID_DRAW3D_PAN, OnDraw3dPan)
	ON_COMMAND(ID_DRAW3D_ZOOM_WND, OnDraw3dZoomWnd)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_DRAW3D_IDLE, OnDraw3dIdle)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEWHEEL()
	ON_EN_CHANGE(IDC_E_TEXT_HEIGHT, OnChangeETextHeight)
	ON_CBN_SELCHANGE(IDC_CMB_LEG_SCHEMA, &CNewTowerDlg::OnSelchangeCmbLegSchema)
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_NEW_SECTION, OnNewSection)
	ON_COMMAND(ID_INSERT_SECTION, OnInsertSection)
	ON_COMMAND(ID_INSERT_SEG, OnInsertSeg)
	ON_BN_CLICKED(IDC_BN_SWITCH_TO_CLASSIC_UI, OnBtnSwitchToClassicUi)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewTowerDlg message handlers
//
BOOL CNewTowerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CSpinButtonCtrl* pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TEXT_HEIGHT);
	pSpinCtrl->SetBuddy((CEdit*)GetDlgItem(IDC_E_TEXT_HEIGHT));
	pSpinCtrl->SetRange32(1,1000);
	pSpinCtrl->SetPos(ftoi(m_fTextHeight/100));
	//pSpinCtrl->SetAccel(
	//m_listBodyOpening.SetExtendedStyle(LVS_EX_GRIDLINES);//
	InitItemPropHashtable();
	long col_wide_arr[4]={150,40,15};
	m_listBodyOpening.InitListCtrl(col_wide_arr);
	//设置修改回调函数
	m_listBodyOpening.SetModifyValueFunc(FireValueModify);
	//设置帮助回调函数
	m_listBodyOpening.SetItemChangedFunc(FireItemChanged);
	//设置右键菜单回调函数
	m_listBodyOpening.SetContextMenuFunc(FireContextMenu);
	//设置删除回调函数
	m_listBodyOpening.SetDeleteItemFunc(FireDeleteItem);
	//-1表示无图像链表
	m_listBodyOpening.InitImageList(IDR_IL_LIST);//-1 
	UpdateDialogSize(0,0);
	CRect rc;
	CPaintDC dc(this);
	CWnd *pWnd = GetDlgItem(IDC_PIC_CURRENT_PANEL);
	pWnd->GetWindowRect(&rc);
	bitmap.InitDC(dc.GetSafeHdc(),rc.Width(),rc.Height());
	//设置菜单按钮的关联菜单
	m_xMenuBtn.FireClickComboMenu=TrackLiveComboMenu;
	FireItemChanged(&this->m_listBodyOpening,NULL,NULL);	//默认按钮的显示菜单
#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
	GetDlgItem(IDC_BN_SWITCH_TO_CLASSIC_UI)->ShowWindow(SW_HIDE);
#else
	GetDlgItem(IDC_BN_SWITCH_TO_CLASSIC_UI)->ShowWindow(SW_NORMAL);
#endif
	/*CButton *pBtn;
	pBtn=(CButton*)GetDlgItem(IDC_BTN_FRONT_VIEW);
	pBtn->SetState(m_iViewType==0);
	pBtn=(CButton*)GetDlgItem(IDC_BTN_SIDE_VIEW);
	pBtn->SetState(m_iViewType==1);
	pBtn=(CButton*)GetDlgItem(IDC_BTN_PERSPECTIVE_VIEW);
	pBtn->SetState(m_iViewType==2);
	//初始化窗口控件大小,并移动对话框到左上角
	CSize screen,increExt;
	GetWindowRect(rc);
	screen.cx=GetSystemMetrics(SM_CXMAXIMIZED);
	screen.cy=GetSystemMetrics(SM_CYMAXIMIZED);
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
	rc.bottom+=increExt.cy;
	m_listBodyOpening.MoveWindow(&rc);
	//调整右侧塔身预览视图至最大
	pWnd = GetDlgItem(IDC_PIC_TOWER_MAP);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.right+=increExt.cx;
	rc.bottom+=increExt.cy;
	pWnd->MoveWindow(&rc);
	m_rectClient=rc;*/
	//调整中间标准面列表控件至最大
	/*pWnd = GetDlgItem(IDC_LIST_CTRL_STD_FACE);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.bottom+=increExt.cy;
	pWnd->MoveWindow(&rc);*/
	//初始化绘图引擎
	//以下三行为临时代码，将来完善Draw3d.dll对OnSize支持后再去掉　wjh-2017.6.2
	pWnd = GetDlgItem(IDC_PIC_TOWER_MAP);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_pDraw = CDrawEngine::CreateEngine();
	m_pDraw->Init(GetSafeHwnd(),rc);
	m_pDraw->SetBkColor(RGB(192,192,192));
	m_pDraw->dwParam=(DWORD)this;
	m_pDraw->SetDrawFunc(DrawTowerSketch);
	UCS_STRU object_cs,drawPlaneCS,std_cs;
	object_cs.axis_x.Set(1,0,0);
	object_cs.axis_y.Set(0,0,1);
	object_cs.axis_z.Set(0,-1,0);
	m_pDraw->SetObjectUcs(object_cs);
	LoadDefaultUCS(&drawPlaneCS);
	drawPlaneCS.axis_x.Set(1,0,0);
	drawPlaneCS.axis_y.Set(0,0,-1);
	POINT scr_org;
	scr_org.x=(rc.left+rc.right)/2;
	scr_org.y=rc.top+80;
	m_pDraw->FixView(scr_org,drawPlaneCS,1);
	LoadDefaultUCS(&std_cs);
	m_pDraw->NewCS(std_cs,"X","Y","Z","O",1);
	char fontfile[MAX_PATH];
	strcpy(fontfile,theApp.execute_path);
	strcat(fontfile,"sys\\simplex.shx");
	m_pDraw->SetShxFontFile(fontfile);
	strcpy(fontfile,theApp.execute_path);
	strcat(fontfile,"sys\\GBHZFS.shx");
	m_pDraw->SetBigFontFile(fontfile);
	m_pDraw->SetPen(RGB(255,0,0),0,1);
	//
	if (m_xShankDef.mode.blAppendMode)
	{
		m_xShankDef.Empty();
		CShankSlope* pSlope = m_xShankDef.AppendSlope();
		pSlope->m_fTopCoordZ = m_xShankDef.mode.dfTopCoordZ;
		pSlope->m_fTopFrontWidth = m_xShankDef.mode.dfTopFrontWidth;
		pSlope->m_fTopSideWidth = m_xShankDef.mode.dfTopSideWidth;
	}
	if(m_xShankDef.SlopeNum()>0)
	{	//有数据，更新ListCtrl并绘制草图
		m_bFireListItemChange=false;
		UpdateListOpenCtrlByShankDef(NULL,NULL);
		m_bFireListItemChange=true;
	}
	//m_pDraw->NewText("坐标轴：红色为X轴；绿色为Y轴；蓝色为Z轴",f3dPoint(0,0,0),20,f3dPoint(1,0,0),f3dPoint(0,1,0));
	ReDrawTowerSketchView();
	return TRUE; 
}
void CNewTowerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UpdateDialogSize(cx, cy, false);
}
void CNewTowerDlg::UpdateDialogSize(int cx, int cy,bool blReflectEventOnSize/*=true*/)
{
	CRect rc,rcPanel;
	CButton *pBtn;
	CWnd* pWnd;
	if (pBtn = (CButton*)GetDlgItem(IDC_BTN_FRONT_VIEW))
		pBtn->SetState(m_iViewType == 0);
	else
		return;
	if (pBtn=(CButton*)GetDlgItem(IDC_BTN_SIDE_VIEW))
		pBtn->SetState(m_iViewType==1);
	else
		return;
	if (pBtn=(CButton*)GetDlgItem(IDC_BTN_PERSPECTIVE_VIEW))
		pBtn->SetState(m_iViewType==2);
	else
		return;
	//初始化窗口控件大小,并移动对话框到左上角
	CSize screen,increExt;
	GetClientRect(rc); 
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
	if (blReflectEventOnSize)
	{
		rc.left = 0;
		rc.top = 0;
		rc.right = screen.cx;
		rc.bottom = screen.cy;
		MoveWindow(rc, TRUE);
	}
	//调整左侧主体开口列表框至最大
	m_listBodyOpening.GetWindowRect(&rcPanel);
	ScreenToClient(&rcPanel);
	//rcPanel.right = rc.right-rcRightMargin;
	rcPanel.bottom=rc.bottom-rcBtmMargin;
	//ClientToScreen(&rcPanel);
	m_listBodyOpening.MoveWindow(&rcPanel);
	//调整右侧塔身预览视图至最大
	if (pWnd = GetDlgItem(IDC_PIC_TOWER_MAP))
		pWnd->GetWindowRect(&rcPanel);
	else
		return;
	ScreenToClient(&rcPanel);
	rcPanel.right = rc.right-rcRightMargin;
	rcPanel.bottom = rc.bottom - rcBtmMargin;
	pWnd->MoveWindow(&rcPanel);
	m_rectClient= rcPanel;
	if (m_pDraw)
	{
		m_pDraw->UpdateViewport(&rcPanel);
		m_pDraw->BuildDisplayBuffer();
		InvalidateRect(&rcPanel, FALSE);
	}
}
int CNewTowerDlg::GetPropValueStr(CSuperGridCtrl::CTreeItem* pSelItem,int prop_id,char* valueStr)
{
	CXhChar100 sText;
	CShankDefineObject *pObj=(CShankDefineObject*)pSelItem->m_idProp;
	if(pObj->cObjType=='K')
	{	//变坡段
		if(GetPropID("front_clearance")==prop_id)
			sText.Printf("%f",pObj->Slope()->m_fTopFrontWidth);
		else if(GetPropID("side_clearance")==prop_id)
			sText.Printf("%f",pObj->Slope()->m_fTopSideWidth);
		else if(GetPropID("mainrod_type")==prop_id)
		{
			if(pObj->Slope()->m_cMainRodType=='T')
#ifdef AFX_TARG_ENU_ENGLISH
				sText.Copy("Tube");
			else
				sText.Copy("Angle");
#else
				sText.Copy("钢管");
			else
				sText.Copy("角钢");
#endif
		}
		SimplifiedNumString(sText);
	}
	else if(pObj->cObjType=='D')
	{	//塔身分段
		if(GetPropID("divide_n")==prop_id)
			sText.Printf("%d",pObj->Segment()->divide_n);
		else if(GetPropID("front_layout")==prop_id)
			sText.Copy(pObj->Segment()->front_layout);
		else if(GetPropID("side_layout")==prop_id)
			sText.Copy(pObj->Segment()->side_layout);
		else if(GetPropID("hsect_layout")==prop_id)
			sText.Copy(pObj->Segment()->hsect_layout);
		else if(GetPropID("v_layout")==prop_id)
			sText.Copy(pObj->Segment()->v_layout);
	}
	else if(pObj->cObjType=='L')
	{	//塔腿
		if(GetPropID("front_clearance")==prop_id)
			sText.Printf("%f",pObj->Leg()->m_fFrontWidth);
		else if(GetPropID("side_clearance")==prop_id)
			sText.Printf("%f",pObj->Leg()->m_fSideWidth);
		else if(GetPropID("front_layout")==prop_id)
			sText.Copy(pObj->Leg()->front_layout);
		else if(GetPropID("side_layout")==prop_id)
			sText.Copy(pObj->Leg()->side_layout);
		else if(GetPropID("hsect_layout")==prop_id)
			sText.Copy(pObj->Leg()->hsect_layout);
		else if(GetPropID("v_layout")==prop_id)
			sText.Copy(pObj->Leg()->v_layout);
		else if(GetPropID("divide_n")==prop_id)
			sText.Printf("%d",pObj->Leg()->divide_n);
		SimplifiedNumString(sText);
	}
	else if(pObj->cObjType=='J')
	{	//节间
		if(GetPropID("divide_n")==prop_id)
			sText.Printf("%d",pObj->Section()->divide_n);
		else if(GetPropID("front_layout")==prop_id)
			sText.Copy(pObj->Section()->front_layout);
		else if(GetPropID("side_layout")==prop_id)
			sText.Copy(pObj->Section()->side_layout);
		else if(GetPropID("hsect_layout")==prop_id)
			sText.Copy(pObj->Section()->hsect_layout);
		else if(GetPropID("v_layout")==prop_id)
			sText.Copy(pObj->Section()->v_layout);
		else if(GetPropID("btm_zstr")==prop_id)
			sText.Copy(pObj->Section()->btm_zstr);
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
void CNewTowerDlg::DisplayPropertyList(CSuperGridCtrl::CTreeItem* pSelItem)
{
	CXhChar100 valueStr;
	CItemInfo* lpInfo=NULL;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pSonPropItem=NULL,*pRootItem=m_propList.GetRootItem();

	m_propList.CleanTree();
	m_propList.m_nObjClassTypeId = pSelItem->m_idProp;
	CShankDefineObject* pObj=(CShankDefineObject*)pSelItem->m_idProp;
	if(pObj==NULL)
		return;
	//设置回调函数
	m_propList.SetModifyValueFunc(ModifyItemValue);
	m_propList.SetButtonClickFunc(ButtonClickItemProp);
	m_propList.SetPropHelpPromptFunc(PropHelpFunc);
	if(pObj->cObjType=='K')
	{	//变坡段
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="front open:";
		lpInfo->m_strPropHelp="front,that is, X-Z plane top open horizontal projection size.";
#else
		lpInfo->m_strPropName="正面开口:";
		lpInfo->m_strPropHelp="正面，即X-Z平面的上开口水平投影尺寸。";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPropItem->m_idProp = GetPropID("front_clearance");
		if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="side open:";
		lpInfo->m_strPropHelp="side,that is, Y-Z plane top open horizontal projection size.";
#else
		lpInfo->m_strPropName="侧面开口:";
		lpInfo->m_strPropHelp="侧面，即Y-Z平面的上开口水平投影尺寸。";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPropItem->m_idProp = GetPropID("side_clearance");
		if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Angle|Tube";
		lpInfo->m_strPropName="member type:";
		lpInfo->m_strPropHelp="leg member type";
#else
		lpInfo->m_cmbItems="角钢|钢管";
		lpInfo->m_strPropName="主材类型:";
		lpInfo->m_strPropHelp="塔身主材类型";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPropItem->m_idProp = GetPropID("mainrod_type");
		if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	else if(pObj->cObjType=='L')
	{	//塔腿
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="front open:";
		lpInfo->m_strPropHelp="front,that is, X-Z plane top open horizontal projection size.";
#else
		lpInfo->m_strPropName="正面开口:";
		lpInfo->m_strPropHelp="正面，即X-Z平面的上开口水平投影尺寸。";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPropItem->m_idProp = GetPropID("front_clearance");
		if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="side open:";
		lpInfo->m_strPropHelp="side,that is, Y-Z plane top open horizontal projection size.";
#else
		lpInfo->m_strPropName="侧面开口:";
		lpInfo->m_strPropHelp="侧面，即Y-Z平面的上开口水平投影尺寸。";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPropItem->m_idProp = GetPropID("side_clearance");
		if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType =BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName="top horizontal section layout mode:";
	lpInfo->m_strPropHelp="top horizontal section plane layout mode.";
#else
	lpInfo->m_strPropName="顶部横隔面布材型式:";
	lpInfo->m_strPropHelp="上横隔面布材型式。";
#endif
	pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
	pPropItem->m_idProp = GetPropID("hsect_layout");
	if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	if(pObj->cObjType=='J'||
		(pObj->cObjType=='D'&&pObj->Segment()->SectionNum()==0)||
		(pObj->cObjType=='L'&&pObj->Leg()->SectionNum()==0))
	{	//节间
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		//接腿不需要设定等分数
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Equal Fraction:";
		lpInfo->m_strPropHelp="Equal fraction.";
#else
		lpInfo->m_strPropName=pObj->cObjType!='L'?"等分数:":"节间数";
		lpInfo->m_strPropHelp=pObj->cObjType!='L'?"等分数:":"节间数";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPropItem->m_idProp = GetPropID("divide_n");
		if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;

		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType =BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="front layout rod mode:";
		lpInfo->m_strPropHelp="front layout plane rod mode.";
#else
		lpInfo->m_strPropName="正面布材型式:";
		lpInfo->m_strPropHelp="正面布材型式。";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPropItem->m_idProp = GetPropID("front_layout");
		if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		PropHelpFunc(&m_propList,pPropItem);	//默认显示正面布材型式
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType =BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="side layout rod mode:";
		lpInfo->m_strPropHelp="side layout plane rod mode.";
#else
		lpInfo->m_strPropName="侧面布材型式:";
		lpInfo->m_strPropHelp="侧面布材型式。";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPropItem->m_idProp = GetPropID("side_layout");
		if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType =BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="V-plane layout rod mode:";
		lpInfo->m_strPropHelp="V-plane layout rod mode.";
#else
		lpInfo->m_strPropName="V面布材型式:";
		lpInfo->m_strPropHelp="V面布材型式。";
#endif
		pPropItem = m_propList.InsertItem(pRootItem,lpInfo,-1);
		pPropItem->m_idProp = GetPropID("v_layout");
		if(GetPropValueStr(pSelItem,pPropItem->m_idProp,valueStr)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
	}
	m_propList.Redraw();
}

void CNewTowerDlg::OnSelchangeTowerSideNum() 
{
	UpdateData();
	POSITION pos = m_listBodyOpening.GetRootHeadPosition();
	int i=0;
	while(pos != NULL)
	{
		CSuperGridCtrl::CTreeItem *pParent=NULL,*pItem=NULL;
		pParent =(CSuperGridCtrl::CTreeItem*)m_listBodyOpening.GetNextRoot(pos); 
		pItem=pParent;
		CShankDefineObject* pObj=(CShankDefineObject*)pParent->m_idProp;
		if(pObj->cObjType!='K')
			continue;
		if(pParent->m_lpNodeInfo==NULL)
			continue;
		i++;	//记录当前坡段索引号
		BOOL bReadOnly=(m_iSideNum>1&&m_iSideNum<5);//五边//六边//八边只支持等宽多边形塔
		if(m_iSideNum==0&&i>1)
			bReadOnly=TRUE;	//三角塔只有第一个坡段的侧面开口有效，其余侧面开口应由计算所得
		pParent->m_lpNodeInfo->SetSubItemReadOnly(2,bReadOnly);	//设置坡段数据第三列位只读，随第二列变化
		
		//GetNext ....loop through all children 
		for(;;)
		{
			CSuperGridCtrl::CTreeItem *pCurItem = m_listBodyOpening.GetNext(pParent,pItem, TRUE, FALSE);	  
			if(!m_listBodyOpening.IsChildOf(pParent, pCurItem))
				break;
			else
			{
				if(pCurItem==pItem)
					break;
			}
			CShankDefineObject* pObj=(CShankDefineObject*)pCurItem->m_idProp;
			if(pObj->cObjType=='L')
			{
				if(pCurItem->m_lpNodeInfo)
					pCurItem->m_lpNodeInfo->SetSubItemReadOnly(2,bReadOnly);	//设置塔腿数据第三列位只读，随第二列变化
			}
			pItem=pCurItem;
		}
	}
}

void CNewTowerDlg::OnNewSlope()
{
	if (m_xShankDef.mode.blAppendMode)
		return;	//增量附加模式下，不允许手动添加呼高
	if(IsIncludeSpecTypeDataInItem('B',NULL))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("module group existed, no permission to newly create slope data!");
#else
		AfxMessageBox("已存在呼高分组,不允许新建坡段数据!");
#endif
			return;
	}
	CListCtrlItemInfo* lp = new CListCtrlItemInfo();
	lp->SetImage(DATA_SLOPE-1);
	//add item text
	lp->AddSubItemText(_T(""));
	//Create root item
	CSuperGridCtrl::CTreeItem * pRoot = m_listBodyOpening.InsertRootItem(lp);
	if( pRoot == NULL )
		return;	
	CShankSlope *pSlope=m_xShankDef.AppendSlope();
	pSlope->m_fTopCoordZ=0;
	pSlope->m_fTopFrontWidth=pSlope->m_fTopSideWidth=1000;
	pRoot->m_idProp=(long)pSlope;
	m_listBodyOpening.SetItemState(pRoot->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
	ReDrawTowerSketchView();	//重新绘制右侧预览区
}

void CNewTowerDlg::InsertSeg(CSuperGridCtrl::CTreeItem *pParentItem,int index/*=-1*/)
{
	CListCtrlItemInfo* lp = new CListCtrlItemInfo();
	lp->SetImage(DATA_SEG-1);
	//add item text
	if(m_iUnit==1)
		lp->SetSubItemText(0,_T("6000"));
	else
		lp->SetSubItemText(0,_T("6"));
	lp->AddSubItemText(_T(""));	//段号
	CSuperGridCtrl::CTreeItem * pSonItem = m_listBodyOpening.InsertItem(pParentItem,lp,index,true);
	if( pSonItem)
	{
		CShankDefineObject* pObj=(CShankDefineObject*)pParentItem->m_idProp;
		CShankSegment *pSeg=NULL;
		if(pObj->cObjType=='B')
		{
			if(index==-1)
				pSeg=pObj->Module()->AppendSegment();
			else 
			{
				CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(index);
				int curIndex=pObj->Module()->GetSegmentIndex((CShankSegment*)pCurItem->m_idProp);
				if(curIndex==-1)
					pSeg=pObj->Module()->AppendSegment();
				else 
					pSeg=pObj->Module()->InsertSegment(curIndex+1);
			}
		}
		else if(pObj->cObjType=='K')
		{	
			if(index==-1)
				pSeg=pObj->Slope()->AppendSegment();
			else 
			{
				CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(index);
				int curIndex=pObj->Slope()->GetSegmentIndex((CShankSegment*)pCurItem->m_idProp);
				if(curIndex==-1)
					pSeg=pObj->Slope()->AppendSegment();
				else 
					pSeg=pObj->Slope()->InsertSegment(curIndex+1);
			}
		}
		else if(pObj->cObjType=='D')	//连接子段（子段与子段间是并列互换关系非顺序衔接关系）
			pSeg=pObj->Segment()->AppendJunctionSegment();
		pSeg->m_fHeight=6000;
		pSonItem->m_idProp=(long)pSeg;
		m_listBodyOpening.SetItemState(pSonItem->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
	}
	ReDrawTowerSketchView();	//重新绘制右侧预览区
}

void CNewTowerDlg::OnInsertSeg()
{	//在当前选中分段之后插入新的分段
	int iCurSel=m_listBodyOpening.GetSelectedItem();
	if(iCurSel<0)
		return;
	CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(iCurSel);
	if(pCurItem==NULL)
		return;
	iCurSel=m_listBodyOpening.SelectItem(pCurItem);
	CSuperGridCtrl::CTreeItem *pParentItem=m_listBodyOpening.GetParentItem(pCurItem);
	if(pParentItem==NULL)
		return;
	InsertSeg(pParentItem,pCurItem->GetIndex());
}

void CNewTowerDlg::OnNewSeg()
{
	int iCurSel=m_listBodyOpening.GetSelectedItem();
	if(iCurSel<0)
		return;
	CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(iCurSel);
	if(pCurItem==NULL)
		return;
	CShankDefineObject* pObj=(CShankDefineObject*)pCurItem->m_idProp;
	if(pObj->cObjType=='B'&&IsIncludeSpecTypeDataInItem('L',pCurItem))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("tower leg data exits in the same module group, no permission to newly create segment data!");
#else
		AfxMessageBox("同一呼高分组下已存在塔腿数据,不允许新建分段数据!");
#endif
		return;
	}
	InsertSeg(pCurItem,-1);
}

void CNewTowerDlg::OnNewModule()
{
	int iCurSel=m_listBodyOpening.GetSelectedItem();
	if(iCurSel<0)
		return;
	CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(iCurSel);
	if(pCurItem==NULL)
		return;
	CShankDefineObject* pParentObj=(CShankDefineObject*)pCurItem->m_idProp;
	CListCtrlItemInfo* lp = new CListCtrlItemInfo();
	lp->SetImage(DATA_MODULE-1);
	//add item text
	lp->SetSubItemText(0,_T("30m"));
	for(int i=1;i<m_listBodyOpening.GetColumnCount();i++)	
		lp->SetSubItemReadOnly(i,TRUE); //除第一列其余设置为只读
	CSuperGridCtrl::CTreeItem * pSonItem = m_listBodyOpening.InsertItem(pCurItem,lp,-1,true);
	if( pSonItem)
	{
		CShankModule *pModule=NULL;
		if(pParentObj->cObjType=='D')
			pModule=pParentObj->Segment()->AppendModule();
		else if(pParentObj->cObjType=='K')
			pModule=pParentObj->Slope()->AppendModule();
		strcpy(pModule->name,"30m");
		pSonItem->m_idProp=(long)pModule;
		m_listBodyOpening.SetItemState(pSonItem->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
	}
	ReDrawTowerSketchView();	//重新绘制右侧预览区
}

void CNewTowerDlg::OnNewLeg()
{
	int iCurSel=m_listBodyOpening.GetSelectedItem();
	if(iCurSel<0)
		return;
	CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(iCurSel);
	if(pCurItem==NULL)
		return;
	CShankModule* pModule=(CShankModule*)pCurItem->m_idProp;
	CListCtrlItemInfo* lp = new CListCtrlItemInfo();
	lp->SetImage(DATA_LEG-1);
	//add item text
	CString sValue=_T("3000");
	if(m_iUnit==0)
		sValue=_T("3");
	lp->SetSubItemText(0,sValue);
	lp->AddSubItemText(_T(""));
	CSuperGridCtrl::CTreeItem * pSonItem = m_listBodyOpening.InsertItem(pCurItem,lp,-1,true);
	if( pSonItem)
	{
		CShankLeg *pLeg=pModule->AppendLeg();
		pLeg->m_fHeight=3000;
		pSonItem->m_idProp=(long)pLeg;
		m_listBodyOpening.SetItemState(pSonItem->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
	}
	ReDrawTowerSketchView();	//重新绘制右侧预览区
}
BOOL CNewTowerDlg::DeleteItemAndAttachData(CSuperGridCtrl::CTreeItem* pSelItem,DWORD dwFlag)
{
	if(pSelItem==NULL)
		return FALSE;
	CShankDefineObject* pObj=(CShankDefineObject*)pSelItem->m_idProp;
	int i;
	BOOL bRet=FALSE;
	if(pObj->cObjType=='K')
	{
		if (m_xShankDef.mode.blAppendMode)
		{
			AfxMessageBox("增量模式下，不允许删除从主体模型继承过来的塔身坡段");
			return FALSE;
		}
		for(i=0;i<m_xShankDef.SlopeNum();i++)
		{
			CShankSlope* pSlope=m_xShankDef.SlopeAtIndex(i);
			if(pSlope==pObj)
				bRet=m_xShankDef.DeleteSlopeAtIndex(i);
		}
	}
	else if(pObj->cObjType=='D')
	{
		CShankDefineObject *pParent=(CShankDefineObject*)pObj->Segment()->m_pParent;
		if(pParent&&pParent->cObjType=='K')
		{
			for(i=0;i<pParent->Slope()->SegmentNum();i++)
			{
				if(pParent->Slope()->SegmentAtIndex(i)==pObj)
					bRet=pParent->Slope()->DeleteSegmentAtIndex(i);
			}
		}
		else if(pParent&&pParent->cObjType=='B')
		{
			for(i=0;i<pParent->Module()->SegmentNum();i++)
			{
				if(pParent->Module()->SegmentAtIndex(i)==pObj)
					bRet=pParent->Module()->DeleteSegmentAtIndex(i);
			}
		}
		else if(pParent&&pParent->cObjType=='D')
		{
			for(i=0;i<pParent->Segment()->JunctionSegmentNum();i++)
			{
				if(pParent->Segment()->JunctionSegmentAtIndex(i)==pObj)
					bRet=pParent->Segment()->DeleteSegmentAtIndex(i);
			}
		}
	}
	else if(pObj->cObjType=='B')
	{
		CShankDefineObject* pParent=(CShankDefineObject*)pObj->Module()->m_pParent;
		if(pParent&&pParent->cObjType=='K')
		{
			for(i=0;i<pParent->Slope()->ModuleNum();i++)
			{
				if(pParent->Slope()->ModuleAtIndex(i)==pObj)
					bRet=pParent->Slope()->DeleteModuleAtIndex(i);
			}
		}
		else if(pParent&&pParent->cObjType=='D')
		{
			for(i=0;i<pParent->Segment()->ModuleNum();i++)
			{
				if(pParent->Segment()->ModuleAtIndex(i)==pObj)
					bRet=pParent->Segment()->DeleteModuleAtIndex(i);
			}
		}
		for(i=0;i<m_xShankDef.ModuleNum();i++)
		{
			CShankModule* pModule=m_xShankDef.ModuleAtIndex(i);
			if(bRet&&pModule==pObj)
				bRet=m_xShankDef.DeleteModuleAtIndex(i);
		}
	}
	else if(pObj->cObjType=='L')
	{
		CShankModule *pParentModule=(CShankModule*)pObj->Leg()->ParentModule();
		for(i=0;i<pParentModule->LegNum();i++)
		{
			if(pParentModule->LegAtIndex(i)==pObj)
				bRet=pParentModule->DeleteLegAtIndex(i);
		}
	}
	else if(pObj->cObjType=='J')
	{
		if(pObj->Section()->m_pParent&&pObj->Section()->m_pParent->cObjType=='D')
		{
			CShankSegment *pSegment=pObj->Section()->m_pParent->Segment();
			for(i=0;i<pSegment->SectionNum();i++)
			{
				if(pSegment->SectionAtIndex(i)==pObj)
					bRet=pSegment->DeleteSectionAtIndex(i);
			}
		}
		else if(pObj->Section()->m_pParent&&pObj->Section()->m_pParent->cObjType=='L')
		{
			CShankLeg *pLeg=pObj->Section()->m_pParent->Leg();
			for(i=0;i<pLeg->SectionNum();i++)
			{
				if(pLeg->SectionAtIndex(i)==pObj)
					bRet=pLeg->DeleteSectionAtIndex(i);
			}
		}
	}
	if(bRet)
	{
		//CSuperGridCtrl::DeleteItemEx不仅删除CListCtrl中的item（即CListCtrl::DeleteItem)，
		//还负责删除后台附加的CSuperGridCtrl::CTreeItem数据,如只调用DeleteItem可能会出现内存泄漏 wjh-2014.12.08
		if(dwFlag&DELETE_ITEM)
			m_listBodyOpening.DeleteItemEx(pSelItem,pSelItem->GetIndex());
		ReDrawTowerSketchView();
		return TRUE;
	}
	return FALSE;
}
void CNewTowerDlg::OnDeleteItem()
{
	int iCurSel=m_listBodyOpening.GetSelectedItem();
	if(iCurSel<0)
		return;
	DeleteItemAndAttachData(m_listBodyOpening.GetTreeItem(iCurSel),3);
}

//在指定项，指定索引后插入节间
void CNewTowerDlg::InsertSection(CSuperGridCtrl::CTreeItem *pParentItem,int index/*=-1*/)
{
	CShankDefineObject* pParentObj=(CShankDefineObject*)pParentItem->m_idProp;
	CListCtrlItemInfo* lp = new CListCtrlItemInfo();
	lp->SetImage(DATA_SECTION-1);
	//add item text
	lp->SetSubItemText(0,_T("1000"));
	for(int i=1;i<m_listBodyOpening.GetColumnCount();i++)	
		lp->SetSubItemReadOnly(i,TRUE); //除第一列其余设置为只读
	
	CSuperGridCtrl::CTreeItem * pSonItem = m_listBodyOpening.InsertItem(pParentItem,lp,index,true);
	if(pSonItem)
	{
		CShankSection *pSection=NULL;
		if(pParentObj->cObjType=='D')
		{
			if(index==-1)
				pSection=pParentObj->Segment()->AppendSection();
			else 
			{
				CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(index);
				int curIndex=pParentObj->Segment()->GetSectionIndex((CShankSection*)pCurItem->m_idProp);
				if(curIndex==-1)
					pSection=pParentObj->Segment()->AppendSection();
				else 
					pSection=pParentObj->Segment()->InsertSection(curIndex+1);
			}
		}
		else if(pParentObj->cObjType=='L')
		{
			if(index==-1)
				pSection=pParentObj->Leg()->AppendSection();
			else 
			{
				CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(index);
				int curIndex=pParentObj->Leg()->GetSectionIndex((CShankSection*)pCurItem->m_idProp);
				if(curIndex==-1)
					pSection=pParentObj->Leg()->AppendSection();
				else 
					pSection=pParentObj->Leg()->InsertSection(curIndex+1);
			}
		}
		pSection->btm_zstr.Copy("1000");
		pSonItem->m_idProp=(long)pSection;
		m_listBodyOpening.SetItemState(pSonItem->GetIndex(),LVNI_SELECTED,LVNI_SELECTED);
	}
	ReDrawTowerSketchView();	//重新绘制右侧预览区
}

void CNewTowerDlg::OnInsertSection()
{	//在当前选中节间之后插入新的节间
	int iCurSel=m_listBodyOpening.GetSelectedItem();
	if(iCurSel<0)
		return;
	CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(iCurSel);
	if(pCurItem==NULL)
		return;
	CSuperGridCtrl::CTreeItem *pParentItem=m_listBodyOpening.GetParentItem(pCurItem);
	if(pParentItem==NULL)
		return;
	InsertSection(pParentItem,pCurItem->GetIndex());
}
void CNewTowerDlg::OnNewSection() 
{	//插入节间
	int iCurSel=m_listBodyOpening.GetSelectedItem();
	if(iCurSel<0)
		return;
	CSuperGridCtrl::CTreeItem *pCurItem=m_listBodyOpening.GetTreeItem(iCurSel);
	if(pCurItem==NULL)
		return;
	int index=-1;
	POSITION pos = m_listBodyOpening.GetHeadPosition(pCurItem);
	while (pos)
	{
		CSuperGridCtrl::CTreeItem *pChild = m_listBodyOpening.GetNextChild(pCurItem, pos);
		if(pChild==NULL)
			continue;
		CShankDefineObject *pChildObj=(CShankDefineObject*)pChild->m_idProp;
		if(pChildObj&&pChildObj->cObjType!='J')
		{	//查找第一个非节间节点,并记录索引 wht 12-03-24
			index=pChild->GetIndex()-1;
			break;
		}
	}
	InsertSection(pCurItem,index);
}

void CNewTowerDlg::OnImportXMLFile()
{
	CString sFileName;
	char *sName="fll";
#ifdef AFX_TARG_ENU_ENGLISH
	char *sFileType="define tower(*.xml)|*.xml||";
#else
	char *sFileType="定义塔身(*.xml)|*.xml||";
#endif
	CFileDialog dlg(TRUE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	UpdateData();	//更新材质否则容易冲掉以前改过的材质
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		XMLDataSerialize(sFileName,FALSE);
		CLogErrorLife life;
		ReDrawTowerSketchView();	//重新绘制右侧预览区
		m_pDraw->ZoomAll(0.9);
		m_bAutoCalTextHeight=TRUE;
	}
}

void CNewTowerDlg::OnExportXMLFile()
{
	CString sFileName;
	char *sName="fll";
#ifdef AFX_TARG_ENU_ENGLISH
	char *sFileType="define tower(*.xml)|*.xml||";
#else
	char *sFileType="定义塔身(*.xml)|*.xml||";
#endif
	CFileDialog dlg(FALSE,sName,"Tower",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
	if(dlg.DoModal()==IDOK)
	{
		sFileName = dlg.GetPathName();
		XMLDataSerialize(sFileName,TRUE);
	}
}

/*CSuperGridCtrl::CTreeItem *CNewTowerDlg::ExprotListChildItemToXML(CMarkup &xml,LISTITEM_TYPE data_type,CSuperGridCtrl::CTreeItem *pParentItem)
{
	CSuperGridCtrl::CTreeItem *pSonItem=NULL;
	if(pParentItem==NULL&&data_type!=DATA_SLOPE)
		return NULL;
	POSITION pos=NULL;
	if(data_type==DATA_SLOPE)
		pos=m_listBodyOpening.GetRootHeadPosition();
	else 
		pos=m_listBodyOpening.GetHeadPosition(pParentItem);
	xml.IntoElem();
	while(pos)
	{
		if(DATA_SLOPE==data_type)
			pSonItem=(CSuperGridCtrl::CTreeItem*)m_listBodyOpening.GetNextRoot(pos); 
		else 
			pSonItem=(CSuperGridCtrl::CTreeItem*)m_listBodyOpening.GetNextChild(pParentItem,pos);
		if(pSonItem==NULL)
			continue;
		LISTITEM_TYPE nCurItemType=(LISTITEM_TYPE)pSonItem->m_idProp;
		if(nCurItemType!=data_type)
			continue;	//类型不匹配
		//
		CListCtrlItemInfo* lp = pSonItem->m_lpNodeInfo;
		if(lp==NULL)
			continue;
		int i=0;
		CString sTextValue=_T("");
		if(data_type==DATA_SLOPE)
		{	//坡段数据
			xml.AddElem("K");
			for(i=0;i<lp->GetItemCount();i++)
			{
				if(i==0)
					xml.AddAttrib("起始高",lp->GetSubItemText(i));
				else if(i==1)
					xml.AddAttrib("正面开口",lp->GetSubItemText(i));
				else if(i==2)
					xml.AddAttrib("侧面开口",lp->GetSubItemText(i));
				else 
					break;
			}
			//导出子项 DATA_MODULE
			ExprotListChildItemToXML(xml,DATA_MODULE,pSonItem);	//坡段数据下支持直接挂呼高分组 wht 10-07-24
			//导出子项 DATA_SEG
			ExprotListChildItemToXML(xml,DATA_SEG,pSonItem);
		}
		else if(data_type==DATA_SEG)
		{	//坡段内分段数据
			xml.AddElem("D");
			xml.AddAttrib("垂高",lp->GetSubItemText(0));
			xml.AddAttrib("段号",lp->GetSubItemText(3));
			//导出子项 DATA_MODULE
			ExprotListChildItemToXML(xml,DATA_MODULE,pSonItem);
		}
		else if(data_type==DATA_MODULE)
		{	//呼高分组数据
			xml.AddElem("B");
			xml.AddAttrib("名称",lp->GetSubItemText(0));
			//导出子项 DATA_SEG
			ExprotListChildItemToXML(xml,DATA_SEG,pSonItem);
			//导出子项 DATA_LEG
			ExprotListChildItemToXML(xml,DATA_LEG,pSonItem);
		}
		else if(data_type==DATA_LEG)
		{	//塔腿数据
			xml.AddElem("L");
			for(i=0;i<lp->GetItemCount();i++)
			{
				if(i==0)
					xml.AddAttrib("垂高",lp->GetSubItemText(i));
				else if(i==1)
					xml.AddAttrib("正面开口",lp->GetSubItemText(i));
				else if(i==2)
					xml.AddAttrib("侧面开口",lp->GetSubItemText(i));
				else if(i==3)
					xml.AddAttrib("段号",lp->GetSubItemText(i));
				else 
					break;
			}
		}
		else if(data_type==DATA_SECTION)
		{	//分段内节点数据
			xml.AddElem("J");
		}
	}
	xml.OutOfElem();
	return pSonItem;
}

CSuperGridCtrl::CTreeItem *CNewTowerDlg::ImprotXMLChildElemToList(CMarkup xml,LISTITEM_TYPE data_type,CSuperGridCtrl::CTreeItem *pParentItem)
{
	CString sElemName=_T("");
	if(data_type==DATA_SLOPE)
		sElemName="K";
	else if(data_type==DATA_SEG)
		sElemName="D";
	else if(data_type==DATA_MODULE)
		sElemName="B";
	else if(data_type==DATA_LEG)
		sElemName="L";
	else if(data_type==DATA_SECTION)
		sElemName="J";
	else
		return NULL;
	CSuperGridCtrl::CTreeItem *pNewItem=NULL;
	if(pParentItem==NULL&&data_type!=DATA_SLOPE)
		return NULL;
	CString sModuleName=_T(""),sVerHeight=_T(""),sFrontWidth=_T(""),sSideWidth=_T(""),sSeg=_T("");
	while (xml.FindChildElem(sElemName))   
	{ 
		xml.IntoElem();
		if(sElemName.CompareNoCase("K")==0)
		{
			sVerHeight=xml.GetAttrib("起始高");
			sFrontWidth=xml.GetAttrib("正面开口");
			sSideWidth=xml.GetAttrib("侧面开口");
		}
		else if(sElemName.CompareNoCase("D")==0)
		{
			sSeg=xml.GetAttrib("段号");
			sVerHeight=xml.GetAttrib("垂高");
		}
		else if(sElemName.CompareNoCase("B")==0)
			sModuleName=xml.GetAttrib("名称");
		else if(sElemName.CompareNoCase("L")==0)
		{
			sSeg=xml.GetAttrib("段号");
			sVerHeight=xml.GetAttrib("垂高");
			sFrontWidth=xml.GetAttrib("正面开口");
			sSideWidth=xml.GetAttrib("侧面开口");
		}
		else
			return NULL;
		//
		CListCtrlItemInfo* lp = new CListCtrlItemInfo();
		lp->SetImage(data_type-1);
		if(data_type==DATA_MODULE)
			lp->SetSubItemText(0,sModuleName);
		else
		{	
			lp->SetSubItemText(0,sVerHeight);
			lp->AddSubItemText(sFrontWidth);
			lp->AddSubItemText(sSideWidth);
			if(data_type==DATA_LEG||data_type==DATA_SEG)
				lp->AddSubItemText(sSeg);
			if(data_type==DATA_SEG||data_type==DATA_MODULE)
			{	 //第一列其余设置为只读
				for(int i=1;i<m_listBodyOpening.m_arrHeaderLabel.GetSize();i++)	
				{
					if(DATA_SEG==data_type&&i>2)
						continue;
					lp->SetSubItemReadOnly(i,TRUE);
				}
			}
		}
		if(sElemName.CompareNoCase("K")==0)
			pNewItem=m_listBodyOpening.InsertRootItem(lp);
		else
			pNewItem=m_listBodyOpening.InsertItem(pParentItem,lp,TRUE);
		if(pNewItem)
		{
			pNewItem->m_idProp=data_type;
			if(data_type==DATA_SLOPE)	//查找K的子节点D
			{	//依次查找K的子节点B,D
				ImprotXMLChildElemToList(xml,DATA_MODULE,pNewItem);	//坡段数据下支持直接挂呼高分组 wht 10-07-24
				ImprotXMLChildElemToList(xml,DATA_SEG,pNewItem);
			}
			else if(data_type==DATA_SEG)//查找D的子节点B
				ImprotXMLChildElemToList(xml,DATA_MODULE,pNewItem);
			else if(data_type==DATA_MODULE)
			{	//依次查找B的子节点D,L
				ImprotXMLChildElemToList(xml,DATA_SEG,pNewItem);
				ImprotXMLChildElemToList(xml,DATA_LEG,pNewItem);
			}
		}
		xml.OutOfElem();
	}
	return pNewItem;
}*/

//读写XML文件
BOOL CNewTowerDlg::XMLDataSerialize(CString sFileName, BOOL bWrite)
{
	CMarkup xml;
	if(bWrite)
	{	//导出XML文件
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
#ifdef AFX_TARG_ENU_ENGLISH
		xml.AddElem("tower");
		if(m_iUnit==0)
			xml.AddAttrib("unit","meter");
		else
			xml.AddAttrib("unit","millimeter");
		xml.AddAttrib("leg connecting mode",m_iMultiLegSchema+1);
#else
		xml.AddElem("铁塔");
		if(m_iUnit==0)
			xml.AddAttrib("单位","米");
		else
			xml.AddAttrib("单位","毫米");
		xml.AddAttrib("接腿模式",m_iMultiLegSchema+1);
#endif
		ExportShankDefToXML(xml,NULL);
		//ExprotListChildItemToXML(xml,DATA_SLOPE,NULL);
		xml.Save(sFileName);
	}
	else
	{	//导入XML文件
		if(!xml.Load(sFileName))
		{	//加载XML文件
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("fail to read file!");
#else
			AfxMessageBox("读取文件失败!");
#endif
			return FALSE;
		}
		xml.ResetMainPos();
#ifdef AFX_TARG_ENU_ENGLISH
		if(xml.FindElem("tower"))
		{
			CString sUint=xml.GetAttrib("unit");
			if(sUint.CompareNoCase("meter")==0)
#else
		if(xml.FindElem("铁塔"))
		{
			CString sUint=xml.GetAttrib("单位");
			if(sUint.CompareNoCase("米")==0)
#endif
				m_iOldUnit=m_iUnit=0;
			else 
				m_iOldUnit=m_iUnit=1;
#ifdef AFX_TARG_ENU_ENGLISH
			CString sSchema=xml.GetAttrib("leg connecting mode");
#else
			CString sSchema=xml.GetAttrib("接腿模式");
#endif
			if(sSchema.GetLength()>0)
				m_iMultiLegSchema=atoi(sSchema)-1;
			else
				m_iMultiLegSchema=0;
		}
		else
		{
			m_iOldUnit=m_iUnit=1;	//mm
			m_iMultiLegSchema=0;
		}
		CFGLEG::SetSchema(m_iMultiLegSchema+1);
		m_xShankDef.Empty();	//清空
		ImportShankDefFromXML(xml,DATA_SLOPE,NULL);
		if (m_xShankDef.mode.blAppendMode)
		{
			CShankSlope* pSlope = m_xShankDef.SlopeAtIndex(0);
			pSlope->m_fTopCoordZ = m_xShankDef.mode.dfTopCoordZ;
			pSlope->m_fTopFrontWidth = m_xShankDef.mode.dfTopFrontWidth;
			pSlope->m_fTopSideWidth = m_xShankDef.mode.dfTopSideWidth;
		}
		m_bFireListItemChange=false;
		UpdateListOpenCtrlByShankDef(NULL,NULL);
		m_bFireListItemChange=true;
		//ImprotXMLChildElemToList(xml,DATA_SLOPE,NULL);
		Invalidate();
	}
	UpdateData(FALSE);
	return TRUE;
}

bool CNewTowerDlg::UpdateListOpenCtrlByShankDef(CShankDefineObject *pObj,CSuperGridCtrl::CTreeItem *pParentItem)
{
	int i;
	CString sText;
	CListCtrlItemInfo* lpInfo=NULL;
	CSuperGridCtrl::CTreeItem *pItem=NULL;
	if(pObj==NULL||pParentItem==NULL)
	{
		m_listBodyOpening.DeleteAllItems();
		for(i=0;i<m_xShankDef.SlopeNum();i++)
		{
			CShankSlope *pSlope=m_xShankDef.SlopeAtIndex(i);
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_SLOPE-1);	//坡段
			if(pSlope->m_fTopCoordZ==0)
				sText="";
			else
				sText.Format("%f",pSlope->m_fTopCoordZ);
			SimplifiedNumString(sText);
			lpInfo->SetSubItemText(0, sText, m_xShankDef.mode.blAppendMode);//FALSE);
			pItem=m_listBodyOpening.InsertRootItem(lpInfo);
			pItem->m_idProp=(long)pSlope;
			UpdateListOpenCtrlByShankDef(pSlope,pItem);
		}
		m_listBodyOpening.Redraw();
	}
	else if(pObj->cObjType=='K')
	{
		CShankSlope *pSlope=pObj->Slope();
		for(i=0;i<pSlope->SegmentNum();i++)
		{
			CShankSegment *pSeg=pSlope->SegmentAtIndex(i);
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_SEG-1);	//呼高组
			if(pSeg->m_fHeight==0)
				sText="";
			else
				sText.Format("%f",pSeg->m_fHeight);
			SimplifiedNumString(sText);
			lpInfo->SetSubItemText(0,sText,FALSE);
			sText.Format("%s",(char*)pSeg->m_iSeg.ToString());
			lpInfo->SetSubItemText(1,sText,FALSE);
			pItem=m_listBodyOpening.InsertItem(pParentItem,lpInfo);
			pItem->m_idProp=(long)pSeg;
			UpdateListOpenCtrlByShankDef(pSeg,pItem);
		}
		for(i=0;i<pSlope->ModuleNum();i++)
		{
			CShankModule *pModule=pSlope->ModuleAtIndex(i);
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_MODULE-1);	//呼高组
			lpInfo->SetSubItemText(0,CString(pModule->name));
			pItem=m_listBodyOpening.InsertItem(pParentItem,lpInfo);
			pItem->m_idProp=(long)pModule;
			UpdateListOpenCtrlByShankDef(pModule,pItem);
		}

	}
	else if(pObj->cObjType=='D')
	{
		CShankSegment *pSeg=pObj->Segment();
		for(i=0;i<pSeg->JunctionSegmentNum();i++)
		{
			CShankSegment *pSegment=pSeg->JunctionSegmentAtIndex(i);
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_SEG-1);	//子分段
			if(pSegment->m_fHeight==0)
				sText="";
			else
				sText.Format("%f",pSegment->m_fHeight);
			SimplifiedNumString(sText);
			lpInfo->SetSubItemText(0,sText,FALSE);
			sText.Format("%s",(char*)pSegment->m_iSeg.ToString());
			lpInfo->SetSubItemText(1,sText,FALSE);
			pItem=m_listBodyOpening.InsertItem(pParentItem,lpInfo);
			pItem->m_idProp=(long)pSegment;
			UpdateListOpenCtrlByShankDef(pSegment,pItem);
		}
		for(i=0;i<pSeg->SectionNum();i++)
		{
			CShankSection *pSection=pSeg->SectionAtIndex(i);
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_SECTION-1);	//节间
			lpInfo->SetSubItemText(0,(char*)pSection->btm_zstr);
			pItem=m_listBodyOpening.InsertItem(pParentItem,lpInfo);
			pItem->m_idProp=(long)pSection;
			//UpdateListOpenCtrlByShankDef(pSeg,pItem);
		}
		for(i=0;i<pSeg->ModuleNum();i++)
		{
			CShankModule *pModule=pSeg->ModuleAtIndex(i);
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_MODULE-1);	//呼高组
			lpInfo->SetSubItemText(0,CString(pModule->name));
			pItem=m_listBodyOpening.InsertItem(pParentItem,lpInfo);
			pItem->m_idProp=(long)pModule;
			UpdateListOpenCtrlByShankDef(pModule,pItem);
		}
	}
	else if(pObj->cObjType=='B')
	{
		CShankModule *pModule=pObj->Module();
		for(i=0;i<pModule->SegmentNum();i++)
		{
			CShankSegment *pSeg=pModule->SegmentAtIndex(i);
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_SEG-1);	//呼高组
			if(pSeg->m_fHeight==0)
				sText="";
			else
				sText.Format("%f",pSeg->m_fHeight);
			SimplifiedNumString(sText);
			lpInfo->SetSubItemText(0,sText,FALSE);
			sText.Format("%s",(char*)pSeg->m_iSeg.ToString());
			lpInfo->SetSubItemText(1,sText,FALSE);
			pItem=m_listBodyOpening.InsertItem(pParentItem,lpInfo);
			pItem->m_idProp=(long)pSeg;
			UpdateListOpenCtrlByShankDef(pSeg,pItem);
		}
		for(i=0;i<pModule->LegNum();i++)
		{
			CShankLeg *pLeg=pModule->LegAtIndex(i);
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_LEG-1);	//呼高组
			if(pLeg->m_fHeight==0)
				sText="";
			else
				sText.Format("%f",pLeg->m_fHeight);
			SimplifiedNumString(sText);
			lpInfo->SetSubItemText(0,sText,FALSE);
			sText.Format("%s",(char*)pLeg->m_iSeg.ToString());
			lpInfo->SetSubItemText(1,sText,FALSE);
			pItem=m_listBodyOpening.InsertItem(pParentItem,lpInfo);
			pItem->m_idProp=(long)pLeg;
			UpdateListOpenCtrlByShankDef(pLeg,pItem);
		}
	}
	else if(pObj->cObjType=='L')
	{	//塔腿数据
		CShankLeg *pLeg=pObj->Leg();
		for(i=0;i<pLeg->SectionNum();i++)
		{
			CShankSection *pSection=pLeg->SectionAtIndex(i);
			lpInfo = new CListCtrlItemInfo();
			lpInfo->SetImage(DATA_SECTION-1);	//节间
			lpInfo->SetSubItemText(0,(char*)pSection->btm_zstr);
			pItem=m_listBodyOpening.InsertItem(pParentItem,lpInfo);
			pItem->m_idProp=(long)pSection;
		}
		/*xml.AddElem("L");
		sText.Format("%f",pParent->Leg()->m_fHeight);
		xml.AddAttrib("垂高",sText);
		sText.Format("%f",pParent->Leg()->m_fFrontWidth);
		xml.AddAttrib("正面开口",sText);
		sText.Format("%f",pParent->Leg()->m_fSideWidth);
		xml.AddAttrib("侧面开口",sText);
		sText.Format("%d",pParent->Leg()->m_iSeg);
		xml.AddAttrib("段号",sText);
		for(i=0;i<pParent->Leg()->SectionNum();i++)
		{	//导出子项 DATA_SECTION
			CShankSection *pSection=pParent->Leg()->SectionAtIndex(i);
			ExportShankDefToXML(xml,DATA_SECTION,pSection);
		}*/
	}
	else if(pObj->cObjType=='J')
	{	//分段内节点数据
		/*xml.AddElem("J");
		xml.AddAttrib("垂高",sText);
		xml.AddAttrib("底部位置",CString(pParent->Section()->btm_zstr));
		xml.AddAttrib("正面布材",CString(pParent->Section()->front_layout));
		xml.AddAttrib("侧面布材",CString(pParent->Section()->side_layout));
		xml.AddAttrib("上横隔布材",CString(pParent->Section()->hsect_layout));
		xml.AddAttrib("V布布材",CString(pParent->Section()->v_layout));
		sText.Format("%d",pParent->Section()->divide_n);
		xml.AddAttrib("等分数",sText);*/
	}
	return true;
}
bool CNewTowerDlg::ExportShankDefToXML(CMarkup& xml,CShankDefineObject *pParent)
{
	CSuperGridCtrl::CTreeItem *pSonItem=NULL;
	int i;
	CString sText;
	if(pParent==NULL)
	{
		for(i=0;i<m_xShankDef.SlopeNum();i++)
		{
			CShankSlope *pSlope=m_xShankDef.SlopeAtIndex(i);
			ExportShankDefToXML(xml,pSlope);
		}
		return true;
	}
	xml.IntoElem();
	if(pParent->cObjType=='K')
	{	//坡段数据
		xml.AddElem("K");
		sText.Format("%f",pParent->Slope()->m_fTopCoordZ);
		SimplifiedNumString(sText);
#ifdef AFX_TARG_ENU_ENGLISH
		xml.AddAttrib("start height",sText);
		sText.Format("%f",pParent->Slope()->m_fTopFrontWidth);
		SimplifiedNumString(sText);
		xml.AddAttrib("front open",sText);
		sText.Format("%f",pParent->Slope()->m_fTopSideWidth);
		SimplifiedNumString(sText);
		xml.AddAttrib("side open",sText);
		if(pParent->Slope()->m_cMainRodType=='T')
			xml.AddAttrib("Main Rod Type","T");
		else
			xml.AddAttrib("Main Rod Type","L");
#else
		xml.AddAttrib("起始高",sText);
		sText.Format("%f",pParent->Slope()->m_fTopFrontWidth);
		SimplifiedNumString(sText);
		xml.AddAttrib("正面开口",sText);
		sText.Format("%f",pParent->Slope()->m_fTopSideWidth);
		SimplifiedNumString(sText);
		xml.AddAttrib("侧面开口",sText);
		if(pParent->Slope()->m_cMainRodType=='T')
			xml.AddAttrib("主材类型","T");
		else
			xml.AddAttrib("主材类型","L");
#endif
		for(i=0;i<pParent->Slope()->ModuleNum();i++)
		{	//导出子项 DATA_MODULE
			CShankModule *pModule=pParent->Slope()->ModuleAtIndex(i);
			ExportShankDefToXML(xml,pModule);
		}
		for(i=0;i<pParent->Slope()->SegmentNum();i++)
		{	//导出子项 DATA_SEG
			CShankSegment *pSeg=pParent->Slope()->SegmentAtIndex(i);
			ExportShankDefToXML(xml,pSeg);
		}
	}
	else if(pParent->cObjType=='D')
	{	//坡段内分段数据
		xml.AddElem("D");
		sText.Format("%f",pParent->Segment()->m_fHeight);
		SimplifiedNumString(sText);
#ifdef AFX_TARG_ENU_ENGLISH
		xml.AddAttrib("vertical height",sText);
		sText.Format("%s",(char*)pParent->Segment()->m_iSeg.ToString());
		xml.AddAttrib("segment No.",sText);
#else
		xml.AddAttrib("垂高",sText);
		sText.Format("%s",(char*)pParent->Segment()->m_iSeg.ToString());
		xml.AddAttrib("段号",sText);
#endif
		if(pParent->Segment()->SectionNum()==0)
		{
			sText.Format("%d",pParent->Segment()->divide_n);
#ifdef AFX_TARG_ENU_ENGLISH
			xml.AddAttrib("Equal Fraction",sText);
		}
		if(strlen(pParent->Segment()->front_layout)>=1)
			xml.AddAttrib("front layout rod",CString(pParent->Segment()->front_layout));
		if(strlen(pParent->Segment()->side_layout)>=1)
			xml.AddAttrib("side layout rod",CString(pParent->Segment()->side_layout));
		if(strlen(pParent->Segment()->hsect_layout)>=1)
			xml.AddAttrib("top horizontal section layout rod",CString(pParent->Segment()->hsect_layout));
		if(strlen(pParent->Segment()->v_layout)>=1)
			xml.AddAttrib("V-plane layout rod",CString(pParent->Segment()->v_layout));
#else
			xml.AddAttrib("等分数",sText);
		}
		if(strlen(pParent->Segment()->front_layout)>=1)
			xml.AddAttrib("正面布材",CString(pParent->Segment()->front_layout));
		if(strlen(pParent->Segment()->side_layout)>=1)
			xml.AddAttrib("侧面布材",CString(pParent->Segment()->side_layout));
		if(strlen(pParent->Segment()->hsect_layout)>=1)
			xml.AddAttrib("上横隔布材",CString(pParent->Segment()->hsect_layout));
		if(strlen(pParent->Segment()->v_layout)>=1)
			xml.AddAttrib("V面布材",CString(pParent->Segment()->v_layout));
#endif
		for(i=0;i<pParent->Segment()->JunctionSegmentNum();i++)
		{	//导出子项 DATA_SEG
			CShankSegment *pSeg=pParent->Segment()->JunctionSegmentAtIndex(i);
			ExportShankDefToXML(xml,pSeg);
		}
		for(i=0;i<pParent->Segment()->SectionNum();i++)
		{	//导出子项 DATA_SECTION
			CShankSection *pSection=pParent->Segment()->SectionAtIndex(i);
			ExportShankDefToXML(xml,pSection);
		}
		for(i=0;i<pParent->Segment()->ModuleNum();i++)
		{	//导出子项 DATA_MODULE
			CShankModule *pModule=pParent->Segment()->ModuleAtIndex(i);
			ExportShankDefToXML(xml,pModule);
		}
	}
	else if(pParent->cObjType=='B')
	{	//呼高分组数据
		xml.AddElem("B");
#ifdef AFX_TARG_ENU_ENGLISH
		xml.AddAttrib("name",CString(pParent->Module()->name));
#else
		xml.AddAttrib("名称",CString(pParent->Module()->name));
#endif
		for(i=0;i<pParent->Module()->SegmentNum();i++)
		{	//导出子项 DATA_SEG
			CShankSegment *pSegment=pParent->Module()->SegmentAtIndex(i);
			ExportShankDefToXML(xml,pSegment);
		}
		for(i=0;i<pParent->Module()->LegNum();i++)
		{	//导出子项 DATA_LEG
			CShankLeg *pLeg=pParent->Module()->LegAtIndex(i);
			ExportShankDefToXML(xml,pLeg);
		}
	}
	else if(pParent->cObjType=='L')
	{	//塔腿数据
		xml.AddElem("L");
		sText.Format("%f",pParent->Leg()->m_fHeight);
		SimplifiedNumString(sText);
#ifdef AFX_TARG_ENU_ENGLISH
		xml.AddAttrib("vertical height",sText);
#else
		xml.AddAttrib("垂高",sText);
#endif
		if(pParent->Leg()->m_fFrontWidth>0)
		{
			sText.Format("%f",pParent->Leg()->m_fFrontWidth);
			SimplifiedNumString(sText);
#ifdef AFX_TARG_ENU_ENGLISH
			xml.AddAttrib("front open",sText);
#else
			xml.AddAttrib("正面开口",sText);
#endif
		}
		if(pParent->Leg()->m_fSideWidth>0)
		{
			sText.Format("%f",pParent->Leg()->m_fSideWidth);
			SimplifiedNumString(sText);
#ifdef AFX_TARG_ENU_ENGLISH
			xml.AddAttrib("side open",sText);
#else
			xml.AddAttrib("侧面开口",sText);
#endif
		}
		sText.Format("%s",(char*)pParent->Leg()->m_iSeg.ToString());
		SimplifiedNumString(sText);
#ifdef AFX_TARG_ENU_ENGLISH
		xml.AddAttrib("segment No.",sText);
		xml.AddAttrib("section_number",(char*)CXhChar16(pParent->Leg()->divide_n));
		if(strlen(pParent->Leg()->front_layout)>=1)
			xml.AddAttrib("front layout rod",CString(pParent->Leg()->front_layout));
		if(strlen(pParent->Leg()->side_layout)>=1)
			xml.AddAttrib("side layout rod",CString(pParent->Leg()->side_layout));
		if(strlen(pParent->Leg()->hsect_layout)>=1)
			xml.AddAttrib("top horizontal section layout rod",CString(pParent->Leg()->hsect_layout));
		if(strlen(pParent->Leg()->v_layout)>=1)
			xml.AddAttrib("V-plane layout rod",CString(pParent->Leg()->v_layout));
#else
		xml.AddAttrib("段号",sText);
		xml.AddAttrib("节间数",(char*)CXhChar16(pParent->Leg()->divide_n));
		if(strlen(pParent->Leg()->front_layout)>=1)
			xml.AddAttrib("正面布材",CString(pParent->Leg()->front_layout));
		if(strlen(pParent->Leg()->side_layout)>=1)
			xml.AddAttrib("侧面布材",CString(pParent->Leg()->side_layout));
		if(strlen(pParent->Leg()->hsect_layout)>=1)
			xml.AddAttrib("上横隔布材",CString(pParent->Leg()->hsect_layout));
		if(strlen(pParent->Leg()->v_layout)>=1)
			xml.AddAttrib("V面布材",CString(pParent->Leg()->v_layout));
#endif
		for(i=0;i<pParent->Leg()->SectionNum();i++)
		{	//导出子项 DATA_SECTION
			CShankSection *pSection=pParent->Leg()->SectionAtIndex(i);
			ExportShankDefToXML(xml,pSection);
		}
	}
	else if(pParent->cObjType=='J')
	{	//分段内节点数据
		xml.AddElem("J");
		/*CExpression expression;
		EXPRESSION_VAR *pVar=expression.varList.append();
		pVar->fValue=1;
		strcpy(pVar->variableStr,"D");
		double pos=expression.SolveExpression(pParent->Section()->btm_zstr);
		if(pos<=1)*/
		if(strlen(pParent->Section()->btm_zstr)>=1)
#ifdef AFX_TARG_ENU_ENGLISH
			xml.AddAttrib("vertical height",CString(pParent->Section()->btm_zstr));
		if(strlen(pParent->Section()->front_layout)>=1)
			xml.AddAttrib("front layout rod",CString(pParent->Section()->front_layout));
		if(strlen(pParent->Section()->side_layout)>=1)
			xml.AddAttrib("side layout rod",CString(pParent->Section()->side_layout));
		if(strlen(pParent->Section()->hsect_layout)>=1)
			xml.AddAttrib("top horizontal section layout rod",CString(pParent->Section()->hsect_layout));
		if(strlen(pParent->Section()->v_layout)>=1)
			xml.AddAttrib("V-plane layout rod",CString(pParent->Section()->v_layout));
		sText.Format("%d",pParent->Section()->divide_n);
		xml.AddAttrib("Equal Fraction",sText);
#else
			xml.AddAttrib("垂高",CString(pParent->Section()->btm_zstr));
		if(strlen(pParent->Section()->front_layout)>=1)
			xml.AddAttrib("正面布材",CString(pParent->Section()->front_layout));
		if(strlen(pParent->Section()->side_layout)>=1)
			xml.AddAttrib("侧面布材",CString(pParent->Section()->side_layout));
		if(strlen(pParent->Section()->hsect_layout)>=1)
			xml.AddAttrib("上横隔布材",CString(pParent->Section()->hsect_layout));
		if(strlen(pParent->Section()->v_layout)>=1)
			xml.AddAttrib("V面布材",CString(pParent->Section()->v_layout));
		sText.Format("%d",pParent->Section()->divide_n);
		xml.AddAttrib("等分数",sText);
#endif
	}
	xml.OutOfElem();
	return true;
}
bool CNewTowerDlg::ImportShankDefFromXML(CMarkup xml,LISTITEM_TYPE data_type,CShankDefineObject *pParent)
{
	CString sElemName=_T("");
	if(data_type==DATA_SLOPE)
		sElemName="K";
	else if(data_type==DATA_SEG)
		sElemName="D";
	else if(data_type==DATA_MODULE)
		sElemName="B";
	else if(data_type==DATA_LEG)
		sElemName="L";
	else if(data_type==DATA_SECTION)
		sElemName="J";
	else
		return NULL;
	if(pParent==NULL&&data_type!=DATA_SLOPE)
		return NULL;
	CString sModuleName=_T(""),sVerHeight=_T(""),sFrontWidth=_T(""),sSideWidth=_T(""),sSeg=_T(""),sText=_T("");
	while (xml.FindChildElem(sElemName))   
	{ 
		xml.IntoElem();
		if(sElemName.CompareNoCase("K")==0)
		{
			CString sMainRodType;
#ifdef AFX_TARG_ENU_ENGLISH
			sVerHeight=xml.GetAttrib("start height");
			sFrontWidth=xml.GetAttrib("front open");
			sSideWidth=xml.GetAttrib("side open");
			sMainRodType=xml.GetAttrib("Main Rod Type");
#else
			sVerHeight=xml.GetAttrib("起始高");
			sFrontWidth=xml.GetAttrib("正面开口");
			sSideWidth=xml.GetAttrib("侧面开口");
			sMainRodType=xml.GetAttrib("主材类型");
#endif
			CShankSlope *pSlope=m_xShankDef.AppendSlope();
			pSlope->m_fTopCoordZ=atof(sVerHeight);
			pSlope->m_fTopFrontWidth=atof(sFrontWidth);
			pSlope->m_fTopSideWidth=atof(sSideWidth);
			if(sMainRodType.GetLength()==0||sMainRodType[0]!='T')
				pSlope->m_cMainRodType='L';
			else
				pSlope->m_cMainRodType='T';
			CShankDefineObject *pObj=pSlope;
			pSlope=(CShankSlope*)pObj;
			ImportShankDefFromXML(xml,DATA_MODULE,pSlope);	//坡段数据下支持直接挂呼高分组 wht 10-07-24
			ImportShankDefFromXML(xml,DATA_SEG,pSlope);
		}
		else if(sElemName.CompareNoCase("D")==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sSeg=xml.GetAttrib("segment No.");
			sVerHeight=xml.GetAttrib("vertical height");
#else
			sSeg=xml.GetAttrib("段号");
			sVerHeight=xml.GetAttrib("垂高");
#endif
			CShankSegment *pSeg=NULL;
			if(pParent->cObjType=='K')
				pSeg=pParent->Slope()->AppendSegment();
			else if(pParent->cObjType=='B')
				pSeg=pParent->Module()->AppendSegment();
			else if(pParent->cObjType=='D')
				pSeg=pParent->Segment()->AppendJunctionSegment();
			if(pSeg!=NULL)
			{
				pSeg->m_fHeight=atof(sVerHeight);
				pSeg->m_iSeg=SEGI(sSeg.GetBuffer());
#ifdef AFX_TARG_ENU_ENGLISH
				sText=xml.GetAttrib("front layout rod");
				strcpy(pSeg->front_layout,sText);
				sText=xml.GetAttrib("side layout rod");
				strcpy(pSeg->side_layout,sText);
				sText=xml.GetAttrib("top horizontal section layout rod");
				strcpy(pSeg->hsect_layout,sText);
				sText=xml.GetAttrib("V-plane layout rod");
				strcpy(pSeg->v_layout,sText);
				sText=xml.GetAttrib("Equal Fraction");
#else
				sText=xml.GetAttrib("正面布材");
				StrCopy(pSeg->front_layout,sText,51);
				sText=xml.GetAttrib("侧面布材");
				StrCopy(pSeg->side_layout,sText,51);
				sText=xml.GetAttrib("上横隔布材");
				StrCopy(pSeg->hsect_layout,sText,51);
				sText=xml.GetAttrib("V面布材");
				StrCopy(pSeg->v_layout,sText,51);
				sText=xml.GetAttrib("等分数");
#endif
				if(sText.GetLength()==0)
					pSeg->divide_n=1;
				else
					pSeg->divide_n=atoi(sText);
				ImportShankDefFromXML(xml,DATA_SEG,pSeg);
				ImportShankDefFromXML(xml,DATA_MODULE,pSeg);
				ImportShankDefFromXML(xml,DATA_SECTION,pSeg);
			}
		}
		else if(sElemName.CompareNoCase("B")==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sModuleName=xml.GetAttrib("name");
#else
			sModuleName=xml.GetAttrib("名称");
#endif
			CShankModule *pModule=NULL;
			if(pParent->cObjType=='D')
				pModule=pParent->Segment()->AppendModule();
			else if(pParent->cObjType=='K')
				pModule=pParent->Slope()->AppendModule();
			if(pModule)
			{
				strcpy(pModule->name,sModuleName);
				ImportShankDefFromXML(xml,DATA_SEG,pModule);
				ImportShankDefFromXML(xml,DATA_LEG,pModule);
			}
		}
		else if(sElemName.CompareNoCase("L")==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sSeg=xml.GetAttrib("segment No.");
			sVerHeight=xml.GetAttrib("vertical height");
			sFrontWidth=xml.GetAttrib("front open");
			sSideWidth=xml.GetAttrib("side open");
#else
			sSeg=xml.GetAttrib("段号");
			sVerHeight=xml.GetAttrib("垂高");
			sFrontWidth=xml.GetAttrib("正面开口");
			sSideWidth=xml.GetAttrib("侧面开口");
#endif
			if(pParent->cObjType=='B')
			{
				CShankLeg *pLeg=pParent->Module()->AppendLeg();
				pLeg->m_iSeg=SEGI(sSeg.GetBuffer());
				pLeg->m_fHeight=atof(sVerHeight);
				pLeg->m_fFrontWidth=atof(sFrontWidth);
				pLeg->m_fSideWidth=atof(sSideWidth);
#ifdef AFX_TARG_ENU_ENGLISH
				sText=xml.GetAttrib("section_number");
				pLeg->divide_n=atoi(sText);
				sText=xml.GetAttrib("front layout rod");
				strcpy(pLeg->front_layout,sText);
				sText=xml.GetAttrib("side layout rod");
				strcpy(pLeg->side_layout,sText);
				sText=xml.GetAttrib("top horizontal section layout rod");
				strcpy(pLeg->hsect_layout,sText);
				sText=xml.GetAttrib("V-plane layout rod");
#else
				sText=xml.GetAttrib("节间数");
				if(sText.GetLength()>0)
					pLeg->divide_n=atoi(sText);
				sText=xml.GetAttrib("正面布材");
				StrCopy(pLeg->front_layout,sText,51);
				sText=xml.GetAttrib("侧面布材");
				StrCopy(pLeg->side_layout,sText,51);
				sText=xml.GetAttrib("上横隔布材");
				StrCopy(pLeg->hsect_layout,sText,51);
				sText=xml.GetAttrib("V面布材");
#endif
				StrCopy(pLeg->v_layout,sText,51);
				/*sText=xml.GetAttrib("等分数");
				if(sText.GetLength()==0)
					pLeg->divide_n=1;
				else
					pLeg->divide_n=atoi(sText);*/
				ImportShankDefFromXML(xml,DATA_SECTION,pLeg);
			}
		}
		else if(sElemName.CompareNoCase("J")==0)
		{
			CShankSection *pSection=NULL;
			if(pParent->cObjType=='D')
				pSection=pParent->Segment()->AppendSection();
			else if(pParent->cObjType=='L')
				pSection=pParent->Leg()->AppendSection();
			if(pSection)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sText=xml.GetAttrib("vertical height");
				pSection->btm_zstr.Copy(sText);
				sText=xml.GetAttrib("front layout rod");
				strcpy(pSection->front_layout,sText);
				sText=xml.GetAttrib("side layout rod");
				strcpy(pSection->side_layout,sText);
				sText=xml.GetAttrib("top horizontal section layout rod");
				strcpy(pSection->hsect_layout,sText);
				sText=xml.GetAttrib("V-plane layout rod");
				strcpy(pSection->v_layout,sText);
				sText=xml.GetAttrib("Equal Fraction");
#else
				sText=xml.GetAttrib("垂高");
				pSection->btm_zstr.Copy(sText);
				sText=xml.GetAttrib("正面布材");
				StrCopy(pSection->front_layout,sText,51);
				sText=xml.GetAttrib("侧面布材");
				StrCopy(pSection->side_layout,sText,51);
				sText=xml.GetAttrib("上横隔布材");
				StrCopy(pSection->hsect_layout,sText,51);
				sText=xml.GetAttrib("V面布材");
				StrCopy(pSection->v_layout,sText,51);
				sText=xml.GetAttrib("等分数");
#endif
				if(sText.GetLength()==0)
					pSection->divide_n=1;
				else
					pSection->divide_n=atoi(sText);
			}
		}
		xml.OutOfElem();
	}
	return true;
}
/*CSuperGridCtrl::CTreeItem *
CNewTowerDlg::AddDataFromListToShankDef(CShankDefine *pShankDef,CSuperGridCtrl::CTreeItem *pParentItem,
										LISTITEM_TYPE nItemType,void *pDataPtr,LISTITEM_TYPE nPtrType,int nScale)
{
	CSuperGridCtrl::CTreeItem *pSonItem=NULL;
	if((pDataPtr==NULL||pParentItem==NULL)&&nItemType!=DATA_SLOPE)
		return NULL;
	if(pShankDef==NULL)
		return NULL;
	POSITION pos=NULL;
	if(nItemType==DATA_SLOPE)
		pos=m_listBodyOpening.GetRootHeadPosition();
	else 
		pos=m_listBodyOpening.GetHeadPosition(pParentItem);
	while(pos)
	{
		if(DATA_SLOPE==nItemType)
			pSonItem=(CSuperGridCtrl::CTreeItem*)m_listBodyOpening.GetNextRoot(pos); 
		else 
			pSonItem=(CSuperGridCtrl::CTreeItem*)m_listBodyOpening.GetNextChild(pParentItem,pos);
		if(pSonItem==NULL)
			continue;
		LISTITEM_TYPE nCurItemType=(LISTITEM_TYPE)pSonItem->m_idProp;
		if(nCurItemType!=nItemType)
			continue;	//类型不匹配
		//
		CListCtrlItemInfo* lp = pSonItem->m_lpNodeInfo;
		if(lp==NULL)
			continue;
		int i=0;
		CXhChar100 sValue;
		if(nItemType==DATA_SLOPE)
		{	//坡段数据
			CShankSlope *pShankSlope=pShankDef->AppendSlope();
			for(i=0;i<lp->GetItemCount();i++)
			{
				sValue.Printf("%s",lp->GetSubItemText(i));
				if(i==0)
					pShankSlope->m_fTopCoordZ=atof(sValue)*nScale;
				else if(i==1)
					pShankSlope->m_fTopFrontWidth=atof(sValue)*nScale;
				else if(i==2)
					pShankSlope->m_fTopSideWidth=atof(sValue)*nScale;
				else 
					break;
			}
			//导出子项 DATA_MODULE //坡段数据下支持直接挂呼高分组 wht 10-07-24
			AddDataFromListToShankDef(pShankDef,pSonItem,DATA_MODULE,pShankSlope,DATA_SLOPE,nScale);
			//导出子项 DATA_SEG
			AddDataFromListToShankDef(pShankDef,pSonItem,DATA_SEG,pShankSlope,DATA_SLOPE,nScale);
		}
		else if(nItemType==DATA_SEG&&pDataPtr)
		{	//坡段内分段数据
			CShankSegment *pShankSeg=NULL;
			if(nPtrType==DATA_SLOPE)
				pShankSeg=((CShankSlope*)pDataPtr)->AppendSegment();
			else if(nPtrType==DATA_MODULE)
				pShankSeg=((CShankModule*)pDataPtr)->AppendSegment();
			if(pShankSeg==NULL)
				continue;
			sValue.Printf("%s",lp->GetSubItemText(0));
			pShankSeg->m_fHeight=atof(sValue)*nScale;
			sValue.Printf("%s",lp->GetSubItemText(3));
			pShankSeg->m_iSeg=atoi(sValue);
			//导出子项 DATA_MODULE
			AddDataFromListToShankDef(pShankDef,pSonItem,DATA_MODULE,pShankSeg,DATA_SEG,nScale);
		}
		else if(nItemType==DATA_MODULE)
		{	//呼高分组数据
			CShankModule *pShankModule=NULL;
			if(nPtrType==DATA_SEG)
				pShankModule=((CShankSegment*)pDataPtr)->AppendModule();
			else if(nPtrType==DATA_SLOPE)
				pShankModule=((CShankSlope*)pDataPtr)->AppendModule();
			if(pShankModule==NULL)
				continue;
			pShankModule->name.Printf("%s",lp->GetSubItemText(0));
			//导出子项 DATA_SEG
			AddDataFromListToShankDef(pShankDef,pSonItem,DATA_SEG,pShankModule,DATA_MODULE,nScale);
			//导出子项 DATA_LEG
			AddDataFromListToShankDef(pShankDef,pSonItem,DATA_LEG,pShankModule,DATA_MODULE,nScale);
		}
		else if(nItemType==DATA_LEG)
		{	//塔腿数据
			CShankLeg *pShankLeg=NULL;
			if(nPtrType==DATA_MODULE)
				pShankLeg=((CShankModule*)pDataPtr)->AppendLeg();
			if(pShankLeg==NULL)
				continue;
			for(i=0;i<lp->GetItemCount();i++)
			{
				sValue.Printf("%s",lp->GetSubItemText(i));
				if(i==0)	
					pShankLeg->m_fHeight=atof(sValue)*nScale;
				else if(i==1)
					pShankLeg->m_fFrontWidth=atof(sValue)*nScale;
				else if(i==2)
					pShankLeg->m_fSideWidth=atof(sValue)*nScale;
				else if(i==3) 
					pShankLeg->m_iSeg=atoi(sValue);
				else
					break;
			}
		}
		else if(nItemType==DATA_SECTION)
		{	//分段内节点数据
			;
		}
	}
	return pSonItem;
}*/

void CNewTowerDlg::OnOK() 
{
	CString templfile;
	theApp.GetWorkPathAuto();
	templfile.Format("%sbodytempl.xml",theApp.work_path);
#ifdef AFX_TARG_ENU_ENGLISH
	int rslt=MessageBox("tower template file is helpful to rapid modeling again, saved or not?","notes",MB_YESNOCANCEL);
#else
	int rslt=MessageBox("塔身模板文件有助于塔身再次快速建模，是否需要保存?","提示",MB_YESNOCANCEL);
#endif
	if(rslt==IDYES)
	{
		char *sName="body";
#ifdef AFX_TARG_ENU_ENGLISH
		char *sFileType="define tower(*.xml)|*.xml||";
#else
		char *sFileType="定义塔身(*.xml)|*.xml||";
#endif
		CFileDialog dlg(FALSE,sName,"Tower",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFileType);
		if(dlg.DoModal()==IDOK)
			templfile=dlg.GetPathName();
	}
	else if(rslt==IDCANCEL)
		return;
	XMLDataSerialize(templfile,TRUE);
	UpdateData();
	//输出数据时统一按毫米处理
	//int nScale=1;
	//if(m_iUnit==0)	//按米输入的数据，需转换为毫米
	//	nScale=1000;
	//AddDataFromListToShankDef(&m_xShankDef,NULL,DATA_SLOPE,NULL,LISTITEM_TYPE(0),nScale);
	//分配配材号
	m_xShankDef.AllocateCfgWord();
	if(!m_xShankDef.mode.blAppendMode&&
		m_xShankDef.SlopeNum()>0&&m_xShankDef.SlopeAtIndex(m_xShankDef.SlopeNum()-1)->GetDatumLeg()==NULL)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("No datum leg is specified, model cannot be drawn!");
#else
		AfxMessageBox("没有设置基准塔腿,无法绘制模型!");
#endif
		return;
	}
	CDialog::OnOK();
}

//单位
void CNewTowerDlg::OnSelchangeUnit()
{
	UpdateData();
	//0.米 1.毫米
	if(m_iOldUnit!=m_iUnit)
	{
		double fScale=1;
		if(m_iOldUnit==0)
			fScale=1000;	//米==>毫米
		else 
			fScale=0.001;	//毫米==>米
		POSITION pos = m_listBodyOpening.GetRootHeadPosition();
		while(pos != NULL)
		{
			CXhChar100 sValue;
			CSuperGridCtrl::CTreeItem *pParent=NULL,*pItem=NULL;
			pParent =(CSuperGridCtrl::CTreeItem*)m_listBodyOpening.GetNextRoot(pos); 
			pItem = pParent;
			LISTITEM_TYPE data_type=(LISTITEM_TYPE)pParent->m_idProp;
			CListCtrlItemInfo* lp = pParent->m_lpNodeInfo;
			if(data_type==DATA_SLOPE||data_type==DATA_SEG||data_type==DATA_LEG)
			{
				for(int nCol=0; nCol < lp->GetItemCount(); nCol++)
				{
					CString str = lp->GetSubItemText(nCol);
					if(str.GetLength()<=0)
						continue;
					else if((data_type==DATA_SLOPE||data_type==DATA_LEG)&&nCol>2)
						continue;
					else if(data_type==DATA_SEG&&nCol>0)
						continue;
					double fValue=atof((LPCTSTR)str)*fScale;
					sValue.Printf("%f",fValue);
					SimplifiedNumString(sValue);
					lp->SetSubItemText(nCol,CString(sValue));
					m_listBodyOpening.SetItemText(pParent->GetIndex(),nCol,sValue);
				}
			}
			//GetNext ....loop through all children 
			for(;;)
			{
				CSuperGridCtrl::CTreeItem *pCurItem = m_listBodyOpening.GetNext(pParent,pItem, TRUE, FALSE);	  
				if(!m_listBodyOpening.IsChildOf(pParent, pCurItem))
					break;
				else
				{
					if(pCurItem==pItem)
						break;
				}
				LISTITEM_TYPE data_type=(LISTITEM_TYPE)pCurItem->m_idProp;
				CListCtrlItemInfo* lp = pCurItem->m_lpNodeInfo;
				if(data_type==DATA_SLOPE||data_type==DATA_SEG||data_type==DATA_LEG)
				{
					for(int nCol=0; nCol < lp->GetItemCount(); nCol++)
					{
						CString str = lp->GetSubItemText(nCol);
						if(str.GetLength()<=0)
							continue;
						else if((data_type==DATA_SLOPE||data_type==DATA_LEG)&&nCol>2)
							continue;
						else if(data_type==DATA_SEG&&nCol>0)
							continue;
						double fValue=atof((LPCTSTR)str)*fScale;
						sValue.Printf("%f",fValue);
						SimplifiedNumString(sValue);
						lp->SetSubItemText(nCol,CString(sValue));
						m_listBodyOpening.SetItemText(pCurItem->GetIndex(),nCol,sValue);
					}
				}
				pItem=pCurItem;
			}
		}
	}
	m_iOldUnit=m_iUnit;	//记录前一次使用的单位索引
	UpdateData(FALSE);
}

//查找指定项下是否包括指定类型数据，pSpecItem==NULL时查找所有项
BOOL CNewTowerDlg::IsIncludeSpecTypeDataInItem(char cObjType,CSuperGridCtrl::CTreeItem *pSpecItem/*=NULL*/) 
{
	if(pSpecItem==NULL)
	{
		POSITION pos = m_listBodyOpening.GetRootHeadPosition();
		while(pos != NULL)
		{
			CXhChar100 sValue;
			CSuperGridCtrl::CTreeItem *pParent=NULL,*pItem=NULL;
			pParent =(CSuperGridCtrl::CTreeItem*)m_listBodyOpening.GetNextRoot(pos); 
			pItem = pParent;
			CShankDefineObject* pObj=(CShankDefineObject*)pParent->m_idProp;
			CListCtrlItemInfo* lp = pParent->m_lpNodeInfo;
			if(pObj->cObjType==cObjType)
				return TRUE;
			//查找子项
			if(IsIncludeSpecTypeDataInItem(cObjType,pItem))
				return TRUE;
		}
	}
	else
	{
		CSuperGridCtrl::CTreeItem *pItem=pSpecItem;
		for(;;)
		{
			CSuperGridCtrl::CTreeItem *pCurItem = m_listBodyOpening.GetNext(pSpecItem,pItem, TRUE, FALSE);	  
			if(!m_listBodyOpening.IsChildOf(pSpecItem, pCurItem))
				break;
			else
			{
				if(pCurItem==pItem)
					break;
			}
			CShankDefineObject* pObj=(CShankDefineObject*)pCurItem->m_idProp;
			if(pObj->cObjType==cObjType)
				return TRUE;
			pItem=pCurItem;
		}
	}
	return FALSE;
}
//标注构件尺寸 
//iOffsetType -1.X轴负方向 -2.Y轴负方向 1.X轴正方向 2.Y轴正方向
static void DimText(CPaintDC *pDc,f2dPoint start,f2dPoint end,double fLen,
					int iOffsetType,int nOffsetDist=0)
{
	CString sLen;
	sLen.Format("%f",fLen);
	SimplifiedNumString(sLen);
	CSize sizeText=pDc->GetTextExtent(sLen);
	int nSpaceDist=5;	//标注文字与轮廓边之间的间隙值
	f2dPoint dim_pos,dim_start,dim_end,mid_start,mid_end,ext_start,ext_end;
	dim_start=start; 
	dim_end=end;
	if(abs(iOffsetType)==1)
	{	//垂直标注
		if(nOffsetDist==0)	//标注位置相对于标注点的偏移距离
			nOffsetDist=16;//ftoi(sizeText.cx*0.5)+4;
		dim_start.x+=(iOffsetType*nOffsetDist);
		dim_end.x+=(iOffsetType*nOffsetDist);
		//
		ext_start=dim_start;
		ext_end=dim_end;
		ext_start.x+=(iOffsetType*sizeText.cy*0.25);
		ext_end.x+=(iOffsetType*sizeText.cy*0.25);
		dim_pos.Set(dim_start.x,0.5*(dim_start.y+dim_end.y));
		//增加两个标注点空出文字空间
		int nFlag=1;
		if(dim_start.y<dim_end.y)
			nFlag=-1;
		mid_start.Set(dim_start.x,dim_pos.y+0.6*sizeText.cy*nFlag);
		mid_end.Set(dim_start.x,dim_pos.y-0.6*sizeText.cy*nFlag);
	}
	else if(abs(iOffsetType)==2)
	{	//水平标注
		iOffsetType/=2;	//除以2
		if(nOffsetDist==0)	//标注位置相对于标注点的偏移距离
			nOffsetDist=ftoi(sizeText.cy*0.6);
		dim_start.y+=(iOffsetType*nOffsetDist);
		dim_end.y+=(iOffsetType*nOffsetDist);
		dim_pos.Set(0.5*(dim_start.x+dim_end.x),dim_start.y);
		//
		ext_start=dim_start;
		ext_end=dim_end;
		ext_start.y+=(iOffsetType*sizeText.cy*0.25);
		ext_end.y+=(iOffsetType*sizeText.cy*0.25);
		//增加两个标注点空出文字空间
		int nFlag=1;
		if(dim_start.x<dim_end.x)
			nFlag=-1;
		mid_start.Set(dim_pos.x+0.6*sizeText.cx*nFlag,dim_start.y);
		mid_end.Set(dim_pos.x-0.6*sizeText.cx*nFlag,dim_start.y);
		iOffsetType*=2;	//乘以2
	}
	//绘制标注线
	pDc->MoveTo(ftoi(start.x),ftoi(start.y));
	pDc->LineTo(ftoi(ext_start.x),ftoi(ext_start.y));
	pDc->MoveTo(ftoi(end.x),ftoi(end.y));
	pDc->LineTo(ftoi(ext_end.x),ftoi(ext_end.y));
	//
	int nDimSpace=(int)DISTANCE(f3dPoint(dim_start.x,dim_start.y,0),f3dPoint(dim_end.x,dim_end.y,0));
	if(abs(iOffsetType)==2&&nDimSpace<sizeText.cx)
	{
		pDc->MoveTo(ftoi(dim_start.x),ftoi(dim_start.y));
		pDc->LineTo(ftoi(dim_end.x),ftoi(dim_end.y));
		//绘制标注内容
		dim_pos.x+=sizeText.cx;
		pDc->TextOut(ftoi(dim_pos.x-0.5*sizeText.cx),ftoi(dim_pos.y-sizeText.cy*0.5),sLen);
	}
	else
	{
		pDc->MoveTo(ftoi(dim_start.x),ftoi(dim_start.y));
		pDc->LineTo(ftoi(mid_start.x),ftoi(mid_start.y));
		pDc->MoveTo(ftoi(mid_end.x),ftoi(mid_end.y));
		pDc->LineTo(ftoi(dim_end.x),ftoi(dim_end.y));
		//绘制标注内容
		pDc->TextOut(ftoi(dim_pos.x-0.5*sizeText.cx),ftoi(dim_pos.y-sizeText.cy*0.5),sLen);
	}
}
static void GetSegHeightArr(CShankSegment *pShankSeg,double *height_arr,int *arr_num,char *sModuleName);
//得到呼高分组垂高数组
static void GetModuleHeightArr(CShankModule *pModule,CShankModule *pDatumModule,double *height_arr,int *arr_num)
{	
	for(int j=0;j<pModule->SegmentNum();j++)
	{
		CShankSegment *pShankSeg=pModule->SegmentAtIndex(j);
		if(pShankSeg==NULL)
			continue;
		height_arr[*arr_num]=pShankSeg->m_fHeight;
		(*arr_num)++;
		//
		if(pDatumModule!=pModule)
			GetSegHeightArr(pShankSeg,height_arr,arr_num,pDatumModule->name);
	}
	if(pDatumModule==pModule)
	{
		for(int k=0;k<pModule->LegNum();k++)
		{
			CShankLeg *pShankLeg=pModule->LegAtIndex(k);
			if(pShankLeg==NULL)
				continue;
			height_arr[*arr_num]=pShankLeg->m_fHeight;
			(*arr_num)++;
		}
	}
}
//得到指定分段的垂高数组
static void GetSegHeightArr(CShankSegment *pShankSeg,double *height_arr,int *arr_num,char *sModuleName)
{
	int i;
	for(i=0;i<pShankSeg->ModuleNum();i++)
	{
		CShankModule *pModule=pShankSeg->ModuleAtIndex(i);
		if(pModule==NULL)
			continue;
		CShankModule *pDatumModule=pShankSeg->GetSpecModule(sModuleName);
		GetModuleHeightArr(pModule,pDatumModule,height_arr,arr_num);
	}
}

//得到指定坡段的垂高数组
static void GetSlopeHeightArr(CShankSlope *pShankSlope,double *height_arr,int *arr_num,char *sModuleName)
{
	int i=0;
	//
	CShankModule *pDatumModule=NULL;
	if(sModuleName==NULL)
		pDatumModule=pShankSlope->GetSpecModule(NULL);
	for(i=0;i<pShankSlope->ModuleNum();i++)
	{
		CShankModule *pModule=pShankSlope->ModuleAtIndex(i);
		if(pModule&&((sModuleName==NULL&&pModule==pDatumModule)		//基准呼高
			||(sModuleName&&strcmp(sModuleName,pModule->name)==0)))	//指定呼高
		{
			GetModuleHeightArr(pModule,pModule,height_arr,arr_num);
			return;
		}
	}
	for(i=0;i<pShankSlope->SegmentNum();i++)
	{
		CShankSegment *pShankSeg=pShankSlope->SegmentAtIndex(i);
		if(pShankSeg==NULL)
			continue;
		height_arr[*arr_num]=pShankSeg->m_fHeight;
		(*arr_num)++;
		CShankModule *pModule=pShankSeg->GetSpecModule(sModuleName);
		if(pModule)
		{
			GetSegHeightArr(pShankSeg,height_arr,arr_num,sModuleName);
			break;
		}
	}
}

void CNewTowerDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CWnd *pWnd=GetDlgItem(IDC_PIC_CURRENT_PANEL);
	CRect rc;
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	dc.BitBlt(rc.left,rc.top,rc.Width(),rc.Height(),bitmap.GetDC(),0,0,SRCCOPY);
	//绘制铁预览图
	if(m_pDraw)
		m_pDraw->Draw(&dc);
	if(m_bAutoCalTextHeight)
	{
		m_bAutoCalTextHeight=FALSE;
		double scale_user2scr=m_pDraw->GetScaleUserToScreen();
		m_fTextHeight=ftoi(0.1*scale_user2scr)*100;	//默认字高10个象素
		m_fTextHeight=max(1,m_fTextHeight);
		UpdateData(FALSE);
		OnChangeETextHeight();
	}
}

BOOL CNewTowerDlg::ReadCursorFacePanel(CFacePanel& panel,BOOL bPromptError)
{
	IStorage *pRootStg;
	DWORD grfMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
	char file_path[MAX_PATH];
	strcpy(file_path,theApp.execute_path);
	strcat(file_path,"facepanel.pns");
	HRESULT hr = ::StgOpenStorage(_bstr_t(file_path), NULL,grfMode,NULL, 0, &pRootStg);
	if(pRootStg==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Can't open defaulted standard plane file!");
#else
		AfxMessageBox("打不开默认标准面文件！");
#endif
		return FALSE;
	}
	IStream *pStream;
	IStorage *pStg;
	CString Part;
	if(panel.m_iFaceType==1)		//1表示塔身
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="Tower Body Standard Plane";
#else
		Part="塔身标准面";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	else if(panel.m_iFaceType==2)	//2:表示横隔面;
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="横隔标准面";
#else
		Part="横隔标准面";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	else if(panel.m_iFaceType==4)	//4:K面
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="K-Type Standard Plane";
#else
		Part="K型标准面";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	else if(panel.m_iFaceType==5)	//5:V面
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="V-Type Standard Plane";
#else
		Part="V型标准面";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}
	else if(panel.m_iFaceType==6)	//6:接腿正侧面
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Part="Leg Front Plane";
#else
		Part="接腿正侧面";
#endif
		pRootStg->OpenStorage(_bstr_t(Part),NULL,grfMode,NULL,0,&pStg);
		if(pStg==NULL)
			pRootStg->CreateStorage(_bstr_t(Part),grfMode|STGM_CREATE,0,0,&pStg);
	}

	pStg->OpenStream(_bstr_t(panel.sLabel),NULL,grfMode,0,&pStream);
	if(pStream)
	{
		panel.FromStream(pStream);
		pStream->Release();
	}
	pStg->Release();
	pRootStg->Release();
	if(pStream==NULL)
		return FALSE;
	return TRUE;
}
void CNewTowerDlg::UpdateFaceDetailGraph(CFacePanel &panel)
{
	try{
		UCS_STRU rgn_ucs;
		SCOPE_STRU scope;
		//标准塔面段
		for(CFacePanelNode *pNode=panel.node_list.GetFirst();pNode;pNode=panel.node_list.GetNext())
		{
			if(panel.m_iFaceType!=3&&pNode->m_bInternodeRepeat)
				continue;
			scope.VerifyVertex(f3dPoint(pNode->pos.x,pNode->pos.y));
		}
		double scale_x=(bitmap.Width()-60)/scope.wide();
		double scale_y=(bitmap.Height()-60)/scope.high();
		double scr_2_user=min(scale_x,scale_y);
		//确定绘图区相对对话框客户区的相对坐标系
		scope.centre(&rgn_ucs.origin.x,&rgn_ucs.origin.y,&rgn_ucs.origin.z);
		rgn_ucs.origin*=scr_2_user;
		rgn_ucs.origin.x-=bitmap.Width()/2;//+20-rect.left;	//考虑右侧有标注字符的延伸
		rgn_ucs.origin.y+=bitmap.Height()/2;//+rect.top;
		rgn_ucs.axis_x.Set(1,0,0);
		rgn_ucs.axis_y.Set(0,-1,0);
		rgn_ucs.axis_z.Set(0,0,-1);
		//绘制示意图
		bitmap.Clear();
		//ScreenToClient(&rectDrawRgn);
		//bitmap.GetDC()->SelectStockObject(BLACK_BRUSH); // TODO: Add your message handler code here
		//dc.Rectangle(rectDrawRgn);
		bitmap.GetDC()->SelectStockObject(HOLLOW_BRUSH);
		bitmap.GetDC()->SetBkMode(TRANSPARENT);
		bitmap.GetDC()->SetTextColor(RGB(255,0,0));
		CPen pen;
		pen.CreatePen(PS_SOLID,1,RGB(0,255,0));
		CPen *pOldPen=bitmap.GetDC()->SelectObject(&pen);
		/*for(pNode=panel.node_list.GetFirst();pNode;pNode=panel.node_list.GetNext())
		{
			if(panel.m_iFaceType!=3&&pNode->m_bInternodeRepeat)
				continue;
			f3dPoint pos(pNode->pos.x*scr_2_user,pNode->pos.y*scr_2_user);
			coord_trans(pos,rgn_ucs,FALSE);
			f3dPoint start(pos.x-6,pos.y);
			f3dPoint end(pos.x+6,pos.y);
			bitmap.GetDC()->MoveTo(ftoi(start.x),ftoi(start.y));
			bitmap.GetDC()->LineTo(ftoi(end.x),ftoi(end.y));
			start.Set(pos.x,pos.y+6);
			end.Set(pos.x,pos.y-6);
			bitmap.GetDC()->MoveTo(ftoi(start.x),ftoi(start.y));
			bitmap.GetDC()->LineTo(ftoi(end.x),ftoi(end.y));
			start.Set(pos.x-4,pos.y-4);
			end.Set(pos.x+4,pos.y+4);
			bitmap.GetDC()->MoveTo(ftoi(start.x),ftoi(start.y));
			bitmap.GetDC()->LineTo(ftoi(end.x),ftoi(end.y));
			start.Set(pos.x-4,pos.y+4);
			end.Set(pos.x+4,pos.y-4);
			bitmap.GetDC()->MoveTo(ftoi(start.x),ftoi(start.y));
			bitmap.GetDC()->LineTo(ftoi(end.x),ftoi(end.y));
			//bitmap.GetDC()->TextOut((int)pos.x,(int)pos.y,pNode->sTitle,strlen(pNode->sTitle));
		}*/
		for(CFacePanelPole *pPole=panel.pole_list.GetFirst();pPole;pPole=panel.pole_list.GetNext())
		{
			CFacePanelNode *pStart=&panel.node_list[pPole->start_i];
			CFacePanelNode *pEnd=&panel.node_list[pPole->end_i];
			f3dPoint start_pos=pStart->pos;
			f3dPoint end_pos=pEnd->pos;
			start_pos*=scr_2_user;
			end_pos  *=scr_2_user;
			coord_trans(start_pos,rgn_ucs,FALSE);
			coord_trans(end_pos,rgn_ucs,FALSE);
			bitmap.GetDC()->MoveTo(ftoi(start_pos.x),ftoi(start_pos.y));
			bitmap.GetDC()->LineTo(ftoi(end_pos.x),ftoi(end_pos.y));
		}
		bitmap.GetDC()->SelectObject(pOldPen);
		CWnd *pWnd=GetDlgItem(IDC_PIC_CURRENT_PANEL);
		RECT rc;
		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
		InvalidateRect(&rc,FALSE);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
}
void CNewTowerDlg::UpdateStdPanelSketch(int iPanelType,CString labelStdPanel)
{
	CFacePanel panel;
	_snprintf(panel.sLabel,23,"%s",labelStdPanel);
	panel.m_iFaceType=iPanelType;
	if(labelStdPanel.GetLength()>0)
	{
		if(!ReadCursorFacePanel(panel,TRUE))
		{	//读取失败，如果类型为塔身标准面或Ｋ型标准面，可轮换试一下．
			if(iPanelType==1)
				panel.m_iFaceType=4;
			else if(iPanelType==4)
				panel.m_iFaceType=1;
			ReadCursorFacePanel(panel,TRUE);
		}
	}
	CShankDefineObject* pObj=(CShankDefineObject*)m_propList.m_nObjClassTypeId;
	if(pObj!=NULL&&pObj->cObjType=='L'&&(panel.m_iFaceType==5||panel.m_iFaceType==6))
		panel.m_nInternodeNum=pObj->Leg()->divide_n;
	UpdateFaceDetailGraph(panel);
}
void CNewTowerDlg::ReDrawTowerSketchView()
{
	m_pDraw->BuildDisplayBuffer();
	m_pDraw->ZoomAll(0.9);	//全显
	RECT rc;
	CWnd *pWnd = GetDlgItem(IDC_PIC_TOWER_MAP);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	InvalidateRect(&rc,FALSE);
}

void CNewTowerDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if(m_pDraw)	//销毁绘图引擎
		CDrawEngine::Destroy(m_pDraw->GetEngineHandle());
}
CShankDefineObject* CNewTowerDlg::GetSelItemRelaObject() 
{
	POSITION pos=m_listBodyOpening.GetFirstSelectedItemPosition();
	CSuperGridCtrl::CTreeItem *pSelItem=NULL;
	if(pos!=NULL)
	{
		int iCurSel=m_listBodyOpening.GetNextSelectedItem(pos);
		pSelItem=m_listBodyOpening.GetTreeItem(iCurSel);
	}
	if(pSelItem==NULL)
		return NULL;
	else
		return (CShankDefineObject*)pSelItem->m_idProp;
}
void CNewTowerDlg::OnBtnFrontView() 
{
	m_iViewType=0;
	CButton *pBtn;
	pBtn=(CButton*)GetDlgItem(IDC_BTN_FRONT_VIEW);
	pBtn->SetState(m_iViewType==0);
	pBtn=(CButton*)GetDlgItem(IDC_BTN_SIDE_VIEW);
	pBtn->SetState(m_iViewType==1);
	pBtn=(CButton*)GetDlgItem(IDC_BTN_PERSPECTIVE_VIEW);
	pBtn->SetState(m_iViewType==2);
	ReDrawTowerSketchView();
}

void CNewTowerDlg::OnBtnSideView() 
{
	m_iViewType=1;
	CButton *pBtn;
	pBtn=(CButton*)GetDlgItem(IDC_BTN_FRONT_VIEW);
	pBtn->SetState(m_iViewType==0);
	pBtn=(CButton*)GetDlgItem(IDC_BTN_SIDE_VIEW);
	pBtn->SetState(m_iViewType==1);
	pBtn=(CButton*)GetDlgItem(IDC_BTN_PERSPECTIVE_VIEW);
	pBtn->SetState(m_iViewType==2);
	ReDrawTowerSketchView();
}

void CNewTowerDlg::OnBtnPerspectiveView() 
{
	m_iViewType=2;
	CButton *pBtn;
	pBtn=(CButton*)GetDlgItem(IDC_BTN_FRONT_VIEW);
	pBtn->SetState(m_iViewType==0);
	pBtn=(CButton*)GetDlgItem(IDC_BTN_SIDE_VIEW);
	pBtn->SetState(m_iViewType==1);
	pBtn=(CButton*)GetDlgItem(IDC_BTN_PERSPECTIVE_VIEW);
	pBtn->SetState(m_iViewType==2);
	ReDrawTowerSketchView();
}

void CNewTowerDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_rectClient.PtInRect(point))
		SetCapture();

	m_pDraw->LMouseDown(point);
	CDialog::OnLButtonDown(nFlags, point);
}

void CNewTowerDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_pDraw->LMouseUp(point);
	
	CDialog::OnLButtonUp(nFlags, point);
}
//<DEVELOP_PROCESS_MARK nameId="CNewTowerDlg::OnMouseMove">
void CNewTowerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_pDraw->MouseMove(point,nFlags);
	CRect rc;
	CWnd *pWnd = GetDlgItem(IDC_PIC_TOWER_MAP);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	//if(rc.PtInRect(point))
	//	pWnd->SetFocus();
	CDialog::OnMouseMove(nFlags, point);
}
//</DEVELOP_PROCESS_MARK>
void CNewTowerDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRect rc;
	CWnd *pWnd = GetDlgItem(IDC_PIC_TOWER_MAP);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	if(rc.PtInRect(point))
		ContextMenu(point);
	m_pDraw->RMouseDown(point);
	CDialog::OnRButtonDown(nFlags, point);
}

void CNewTowerDlg::ContextMenu(CPoint point)
{
	CPoint scr_point = point;
	ClientToScreen(&scr_point);
	CMenu popMenu;
	popMenu.LoadMenu(IDR_POPUP_DRAW3D_CTRL);
	CMenu *pMenu=popMenu.GetSubMenu(0);
	OPER_TYPE oper_type=m_pDraw->GetOperType();
	if(oper_type==OPER_PAN)
		pMenu->CheckMenuItem(ID_DRAW3D_PAN,MF_BYCOMMAND|MF_CHECKED);
	else if(oper_type==OPER_ZOOM)
		pMenu->CheckMenuItem(ID_DRAW3D_ZOOM,MF_BYCOMMAND|MF_CHECKED);
	else if(oper_type==OPER_ZOOMWND)
		pMenu->CheckMenuItem(ID_DRAW3D_ZOOM_WND,MF_BYCOMMAND|MF_CHECKED);
	else if(oper_type==OPER_ROTATE)
		pMenu->CheckMenuItem(ID_DRAW3D_ROTATE,MF_BYCOMMAND|MF_CHECKED);
	m_bStartContextMenu=TRUE;
	popMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,scr_point.x,scr_point.y,this);
	m_bStartContextMenu=FALSE;
}

void CNewTowerDlg::OnDraw3dZoomAll() 
{
	m_pDraw->ZoomAll(0.9);
}

void CNewTowerDlg::OnDraw3dRotate() 
{
	m_pDraw->SetOperType(OPER_ROTATE);
}

void CNewTowerDlg::OnDraw3dZoom() 
{
	m_pDraw->SetOperType(OPER_ZOOM);
}

void CNewTowerDlg::OnDraw3dReset() 
{
	m_pDraw->ReSetView();
}

void CNewTowerDlg::OnDraw3dPan() 
{
	m_pDraw->SetOperType(OPER_PAN);
	HCURSOR hCursor= AfxGetApp()->LoadCursor(IDC_ARROW_PAN_MOVE);
	SetCursor(hCursor);
}

void CNewTowerDlg::OnDraw3dZoomWnd() 
{
	m_pDraw->SetOperType(OPER_ZOOMWND);
}

void CNewTowerDlg::OnDraw3dIdle() 
{
	m_pDraw->SetOperType(OPER_OTHER);
}

BOOL CNewTowerDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_pDraw&&!m_bStartContextMenu)
	{
		CPoint point;
		GetCursorPos(&point);
		if(m_rectClient.PtInRect(point))
		{
			HCURSOR hCursor=m_pDraw->GetCursor();
			if(hCursor!=NULL)
				SetCursor(hCursor);
			return TRUE;
		}
	}
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CNewTowerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	ZOOM_CENTER_STYLE zoom_style=m_pDraw->GetZoomStyle();
	m_pDraw->SetZoomStyle(MOUSE_CENTER);
	if(zDelta>0)
		m_pDraw->Scale(1.4);
	else
		m_pDraw->Scale(1/1.4);
	m_pDraw->SetZoomStyle(zoom_style);
	
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CNewTowerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_TEXT_HEIGHT)
	{
        CString strValue;
        strValue.Format("%d", nPos*100);
       ((CSpinButtonCtrl*) pScrollBar)->GetBuddy()->SetWindowText(strValue);
    }
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CNewTowerDlg::OnChangeETextHeight() 
{
	double dfOldTextHeight = m_fTextHeight;
	UpdateData();
	CSpinButtonCtrl* pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TEXT_HEIGHT);
	pSpinCtrl->SetPos(ftoi(m_fTextHeight/100));
	if(dfOldTextHeight!=m_fTextHeight)
		ReDrawTowerSketchView();
}

void CNewTowerDlg::OnSelchangeCmbLegSchema()
{
	UpdateData();
}
//切换对话框
void CNewTowerDlg::OnBtnSwitchToClassicUi()
{
	if(m_xShankDef.SlopeNum()==0||
		(m_xShankDef.SlopeNum()>0&&AfxMessageBox("切换到旧界面时数据会丢失，是否进行切换?",MB_YESNO)==IDYES))
	{
		CNewTowerDataOper::m_bSwitchMode=TRUE;
		return CDialog::OnCancel();
	}
}

void CNewTowerDlg::OnCancel()
{
#ifdef AFX_TARG_ENU_ENGLISH
	if(m_xShankDef.SlopeNum()==0||MessageBox("Are you sure to quickly quit tower modelling environment?","notes",MB_OKCANCEL)==IDOK)
#else
	if(m_xShankDef.SlopeNum()==0||MessageBox("是否确认要继续退出快速塔身建模环境?","提示",MB_OKCANCEL)==IDOK)
#endif
		CDialog::OnCancel();
}
//#endif

//<LOCALE_TRANSLATE Confirm by hxr/>
// LDSView.cpp : implementation of the CLDSView class
//

#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "GUID.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "MainFrm.h"
#include "Tower.h"
#include "PromptDlg.h"
#include "CalInt3dllDlg.h"
#include "UserDefMsg.h"
#include "database.h"
#include "dlg.h"
#include "CoordDefDlg.h"
#include "AddDiJiaoLsDlg.h"
#include "3dPtDlg.h"
#include "DefProfileVertexDlg.h"
#include "BoltPropDlg.h"
#include "Query.h"
#include "LineFeatDlg.h"
#include "BendPlankStyleDlg.h"
#include "AddFillPlankDlg.h"
#include "InputAnValDlg.h"
#include "AddDianBanDlg.h"
#include "RollPlankEdgeDlg.h"
#include "NewParallelPlateDlg.h"
#include "NewGradientPlateDlg.h"
#include "DefFaceDlg.h"
#include "PlankVertexDlg.h"
#include "BoltPropDlg.h"
#include "ModifyVertexDlg.h"
#include "PartUcsDlg.h"
#include "CoordDefDlg.h"
#include "SnapTypeVerify.h"
#include "KeypointLifeObj.h"
#include "EmbedMirInfoPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void CLDSView::OnReCalHuoquPos() 
{
	m_curTask = TASK_CAL_PART_PLANK_HUOQU_POS;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_POINT);
	m_nObjectSnapedCounts=0;
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	m_snap_feat_atom_arr.RemoveAll();
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL CLDSView::FinishReCalHuoQuPos(fAtom *pAtom)
{
	CLDSPlate *pPlank;
	f3dPoint vertex;
	if(console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("bending and tangent point which do not deal with non-plate parts drawings!");
#else
		AfxMessageBox("不处理非钢板构件图的火曲和公切点!");
#endif
		return FALSE;
	}
	else
	{
		pPlank=(CLDSPlate*)console.m_pWorkPart;
		vertex = *((f3dPoint*)pAtom);
		int i,n =pPlank->vertex_list.GetNodeNum();
		for(i=0;i<n;i++)
		{
			f3dPoint vertice=pPlank->vertex_list[i].vertex;
			vertice.z=0;
			if(vertice==vertex)
			{
				f3dLine line;
				PROFILE_VERTEX *pPrevVertex = &pPlank->vertex_list[(n+i-1)%n];
				PROFILE_VERTEX *pVertex = &pPlank->vertex_list[i];
				PROFILE_VERTEX *pNextVertex = &pPlank->vertex_list[(i+1)%n];
				line.startPt = pPrevVertex->vertex;
				line.endPt = pNextVertex->vertex;
				vertex.feature = pVertex->vertex.feature;
				if(vertex.feature>10)
				{
					Int3dpl(line,pPlank->HuoQuLine[vertex.feature%10-2],vertex);
					pVertex->vertex.Set(vertex.x,vertex.y,vertex.z);
				}
				else if((pPrevVertex->type==1||pPrevVertex->type==2)||(pVertex->type==1||pVertex->type==2))
				{
					if(!CLDSPlate::UpdateVertexTangentPos(pPrevVertex,pVertex,pNextVertex))
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("select nodes, not arc tangent point!");
#else
						AfxMessageBox("选择节点不是圆弧公切顶点!");
#endif
						return FALSE;
					}
				}
				else
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("select nodes, not bending or tangent vertex!");
#else
					AfxMessageBox("选择节点不是火曲或公切顶点!");
#endif
					return FALSE;
				}
				pPlank->SetModified();
				return TRUE;
			}
		}
		return FALSE;
	}
}

BOOL CLDSView::FinishViewPartFeature(fAtom *pAtom)
{
	switch(pAtom->atom_type)
	{
	case AtomType::prPoint:
		return FinishViewVertex(pAtom);
	case AtomType::prLine:
	case AtomType::prArc:
		return FinishViewLine(pAtom);
	case AtomType::prCircle:
	case AtomType::prEllipse:
		return FinishViewCir(pAtom);
	default:
		return FALSE;
	}
}

BOOL CLDSView::FinishViewVertex(fAtom *pAtom)
{
	f3dPoint vertex = *((f3dPoint*)pAtom);
	f3dPoint old_vertex = vertex;
	coord_trans(vertex,console.m_curWCS,FALSE);
	CPlankVertexDlg dlg;
	if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		C3dPtDlg dlg;
		dlg.m_bCanModify=FALSE;
		dlg.m_fPtX = vertex.x;
		dlg.m_fPtY = vertex.y;
		dlg.DoModal();
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
	{
		CPlankVertexDlg dlg;
		dlg.m_fPosX = vertex.x;
		dlg.m_fPosY = vertex.y;
		CLDSPlate *pPlank=(CLDSPlate*)console.m_pWorkPart;
		PROFILE_VERTEX *pVertex;
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			if(pVertex->vertex.x==old_vertex.x&&pVertex->vertex.y==old_vertex.y)
			{
				if(pVertex->vertex.feature==2)
					dlg.m_iVertexType = 1;
				else if(pVertex->vertex.feature==12)
					dlg.m_iVertexType = 2;
				else if(pVertex->vertex.feature==3)
					dlg.m_iVertexType = 3;
				else if(pVertex->vertex.feature ==13)
					dlg.m_iVertexType = 4;
				else
					dlg.m_iVertexType = 0;
				break;
			}
		}
		if(dlg.DoModal()==IDOK)
		{
			vertex.x = dlg.m_fPosX;
			vertex.y = dlg.m_fPosY;
			coord_trans(vertex,console.m_curWCS,TRUE);
			if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE&&pVertex)
			{
				pVertex->vertex.x = vertex.x;
				pVertex->vertex.y = vertex.y;
				if(dlg.m_iVertexType == 1)
					pVertex->vertex.feature=2;
				else if(dlg.m_iVertexType == 2)
					pVertex->vertex.feature=12;
				else if(dlg.m_iVertexType == 3)
					pVertex->vertex.feature=3;
				else if(dlg.m_iVertexType == 4)
					pVertex->vertex.feature =13;
				else
					pVertex->vertex.feature = 1;
				pPlank->SetModified();
				g_pSolidDraw->BuildDisplayList();
			}
		}
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CPlankVertexDlg dlg;
		dlg.m_fPosX = vertex.x;
		dlg.m_fPosY = vertex.y;
		CLDSParamPlate *pPlank=(CLDSParamPlate*)console.m_pWorkPart;
		PROFILE_VERTEX *pVertex=NULL;
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			if(pVertex->vertex.x==old_vertex.x&&pVertex->vertex.y==old_vertex.y)
			{
				if(pVertex->vertex.feature==2)
					dlg.m_iVertexType = 1;
				else if(pVertex->vertex.feature==12)
					dlg.m_iVertexType = 2;
				else if(pVertex->vertex.feature==3)
					dlg.m_iVertexType = 3;
				else if(pVertex->vertex.feature ==13)
					dlg.m_iVertexType = 4;
				else
					dlg.m_iVertexType = 0;
				break;
			}
		}
		if(dlg.DoModal()==IDOK)
		{
			if(pVertex==NULL)
			{
				vertex.x = dlg.m_fPosX;
				vertex.y = dlg.m_fPosY;
				coord_trans(vertex,console.m_curWCS,TRUE);
				m_xAssistPointList.append(vertex);
				g_pSolidDraw->NewPoint(vertex);
				return FALSE;
			}
			if(fabs(dlg.m_fPosX-vertex.x)>EPS||fabs(dlg.m_fPosY-vertex.y)>EPS)
			{
				if(!pPlank->IsCanManualModify())
				{	//不支持直接修改
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox("no permission to directly modify parametric plate's top coordinate, change into common plate and then modify！");
#else
					MessageBox("目前仅支持修改靴板与底座板的顶点，其余参数化钢板应转换为普通钢板后再进行修改！");
#endif
				}
				else
				{	//
					vertex.x = dlg.m_fPosX;
					vertex.y = dlg.m_fPosY;
					coord_trans(vertex,console.m_curWCS,TRUE);
					pVertex->vertex.x = vertex.x;
					pVertex->vertex.y = vertex.y;
					if(dlg.m_iVertexType == 1)
						pVertex->vertex.feature=2;
					else if(dlg.m_iVertexType == 2)
						pVertex->vertex.feature=12;
					else if(dlg.m_iVertexType == 3)
						pVertex->vertex.feature=3;
					else if(dlg.m_iVertexType == 4)
						pVertex->vertex.feature =13;
					else
						pVertex->vertex.feature = 1;
					pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_NONE;
					pPlank->SetModified();
					g_pSolidDraw->BuildDisplayList();
				}
			}			
		}
	}
	g_pSolidDraw->ReleaseSnapStatus();
	return TRUE;
}

BOOL CLDSView::FinishViewLsCir(fAtom *pAtom)
{
	CBoltPropDlg dlg;
	CLDSBolt ls(console.GetActiveModel());
	CLsRef *pLsRef;
	CLDSLineAngle *pJg;
	f3dCircle *pCircle=(f3dCircle*)pAtom;
	f3dPoint ls_pos;
	dlg.m_pWorkPart=console.m_pWorkPart;
	dlg.work_ucs=console.m_curWCS;
	f3dPoint ls_init_pos;//记录螺栓初始位置
	if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		pJg=(CLDSLineAngle*)console.m_pWorkPart;
		pLsRef=pJg->FindLsByHandle(pCircle->ID);
		if(pLsRef==NULL)
			return FALSE;
		dlg.m_pLs=pLsRef->GetLsPtr();
		ls_init_pos=dlg.m_pLs->ucs.origin;	//记录螺栓初始位置
		dlg.m_iHoleFuncType=pLsRef->GetLsPtr()->FilterFuncType();
		if(dlg.m_iHoleFuncType<2&&pLsRef->GetLsPtr()->m_bVirtualPart)
			dlg.m_iHoleFuncType=2;	//默认旧版文件中的虚拟螺栓为挂线孔
		if(dlg.m_pLs->des_base_pos.datumPoint.datum_pos_style==0)
			dlg.m_bCanModifyPos=TRUE;	//基点定位方式为手动输入时才允许在构件编辑器中调整螺栓位置 wht 10-07-12
		else
			dlg.m_bCanModifyPos=FALSE;
		if(pLsRef->cFlag&CLsRef::DRILL_HOLE)	//螺栓孔加工工艺类型
			dlg.m_bDrillHole=TRUE;
		else
			dlg.m_bDrillHole=FALSE;
		dlg.m_bPunchHole=!dlg.m_bDrillHole;
		if(pLsRef->cFlag&CLsRef::ENLARGE_HOLE)
			dlg.m_bEnlargeHole=TRUE;
		else
			dlg.m_bEnlargeHole=FALSE;
		UCS_STRU ucs;
		pJg->getUCS(ucs);
		ls_pos = dlg.m_pLs->ucs.origin;
		double depth;
		double g = pJg->GetLsG(pLsRef,&depth);
		if(fabs(dlg.m_pLs->get_norm()*pJg->get_norm_x_wing())>
			fabs(dlg.m_pLs->get_norm()*pJg->get_norm_y_wing()))
		{	//X肢上的螺栓
			Int3dlf(ls_pos,ls_pos,dlg.m_pLs->get_norm(),pJg->Start(),pJg->get_norm_x_wing());
			coord_trans(ls_pos,ucs,FALSE);
			ls_pos.Set(ls_pos.z,-g,depth);
		}
		else
		{	//Y肢上的螺栓
			Int3dlf(ls_pos,ls_pos,dlg.m_pLs->get_norm(),pJg->Start(),pJg->get_norm_y_wing());
			coord_trans(ls_pos,ucs,FALSE);
			ls_pos.Set(ls_pos.z,g,depth);
		}
		//对不需要进行开合角的短角钢进行螺栓位置修正   wxc-2016.10.28
		if((pJg->pStart==NULL || pJg->pEnd==NULL)&&pJg->GetKaiHeWingX0Y1()<0)
			pJg->EmendShortJgLsPosY(dlg.m_pLs,ls_pos.y);
		coord_trans(ls_pos,console.m_curWCS,FALSE);
		dlg.m_fPosX = ls_pos.x;
		dlg.m_fPosY = ls_pos.y;
		dlg.m_fPosZ = ls_pos.z;
		dlg.m_fHoleD = dlg.m_pLs->get_d()+dlg.m_pLs->hole_d_increment;
		dlg.m_hLs.Format("0X%X",dlg.m_pLs->handle);
		dlg.m_sLsGuiGe.Format("%sM%dX%.0f",dlg.m_pLs->Grade(),dlg.m_pLs->get_d(),dlg.m_pLs->get_L());
		dlg.m_nWaistLen = pLsRef->waistLen;
		dlg.m_dwRayNo	= pLsRef->dwRayNo;
		dlg.m_fNormX=dlg.m_pLs->get_norm().x;
		dlg.m_fNormY=dlg.m_pLs->get_norm().y;
		dlg.m_fNormZ=dlg.m_pLs->get_norm().z;
		//腰圆孔方向
		dlg.waist_vec=dlg.m_pLs->ucs.axis_x;
		vector_trans(dlg.waist_vec,ucs,FALSE);
		f3dPoint waist_vec(dlg.waist_vec.z,dlg.waist_vec.x,dlg.waist_vec.y);
		dlg.waist_vec=waist_vec;
		vector_trans(dlg.waist_vec,console.m_curWCS,FALSE);
		if(dlg.DoModal()==IDOK)
		{
			(*pLsRef)->m_cFuncType=(BYTE)dlg.m_iHoleFuncType;
			if(dlg.m_iHoleFuncType>=2)
				(*pLsRef)->m_bVirtualPart=true;
			if(pLsRef->waistLen!=dlg.m_nWaistLen)
			{
				pLsRef->waistLen = dlg.m_nWaistLen;
				console.m_pWorkPart->SetModified();
			}
			pLsRef->cFlag=0;
			if(dlg.m_bDrillHole)
				pLsRef->cFlag=CLsRef::DRILL_HOLE;
			else
				pLsRef->cFlag=CLsRef::PUNCH_HOLE;
			if(dlg.m_bEnlargeHole)
				pLsRef->cFlag|=CLsRef::ENLARGE_HOLE;
			if(((int)pLsRef->waistLen)>0)
			{	//更新腰圆孔方向
				vector_trans(dlg.waist_vec,console.m_curWCS,TRUE);
				f3dPoint waist_vec(dlg.waist_vec.y,dlg.waist_vec.z,dlg.waist_vec.x);
				vector_trans(waist_vec,ucs,TRUE);
				dlg.m_pLs->ucs.axis_x=waist_vec;
			}
			(*pLsRef)->set_norm(f3dPoint(dlg.m_fNormX,dlg.m_fNormY,dlg.m_fNormZ));
			if((*pLsRef)->des_work_norm.norm_style==0)	//指定法向方向
				(*pLsRef)->des_work_norm.vector.Set(dlg.m_fNormX,dlg.m_fNormY,dlg.m_fNormZ);
			if(dlg.m_bCanModifyPos)
			{
				ls_pos.Set(dlg.m_fPosX,dlg.m_fPosY,dlg.m_fPosZ);
				coord_trans(ls_pos,console.m_curWCS,TRUE);
				if(fabs(dlg.m_pLs->get_norm()*pJg->get_norm_x_wing())>
					fabs(dlg.m_pLs->get_norm()*pJg->get_norm_y_wing()))
				{	//X肢上的螺栓
					pJg->SetWingXLsPos(pLsRef,ls_pos.x,-ls_pos.y,ls_pos.z);
				}
				else
				{	//Y肢上的螺栓
					pJg->SetWingYLsPos(pLsRef,ls_pos.x,ls_pos.y,ls_pos.z);
				}
			}
			restore_Ls_guige(dlg.m_sLsGuiGe,ls);
			dlg.m_pLs->SetGrade(ls.Grade());
			dlg.m_pLs->set_d(ls.get_d());
			dlg.m_pLs->set_L(ls.get_L());
			dlg.m_pLs->hole_d_increment = dlg.m_fHoleD-dlg.m_pLs->get_d();
			pLsRef->dwRayNo			= dlg.m_dwRayNo;
			pJg->SetModified();
			g_pSolidDraw->BuildDisplayList(this);
		}
		else
			return FALSE;
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE||console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CSuperSmartPtr<CLDSPart>pPlate=console.m_pWorkPart;
		if(pPlate->GetClassTypeId()==CLS_PLATE)
			pLsRef=pPlate.PlatePointer()->FindLsByHandle(pCircle->ID);
		else
			pLsRef=pPlate.ParamPlatePointer()->FindLsByHandle(pCircle->ID);
		if(pLsRef==NULL)
			return FALSE;
		dlg.m_pLs=pLsRef->GetLsPtr();
		ls_init_pos=dlg.m_pLs->ucs.origin;	//记录螺栓初始位置
		dlg.m_iHoleFuncType=pLsRef->GetLsPtr()->FuncType();
		if(dlg.m_iHoleFuncType<2&&pLsRef->GetLsPtr()->m_bVirtualPart)
			dlg.m_iHoleFuncType=2;	//默认旧版文件中的虚拟螺栓为挂线孔
		else if (!pLsRef->GetLsPtr()->m_bVirtualPart)
			dlg.m_iHoleFuncType=pLsRef->GetLsPtr()->IsFootNail()?1:0;	//连接螺栓
		if(Ta.GetLsRefCounts(dlg.m_pLs->handle)<=2&&dlg.m_pLs->des_base_pos.datumPoint.datum_pos_style==0)
			dlg.m_bCanModifyPos = TRUE;	//基点定位方式为手动输入时才允许在构件编辑器中调整螺栓位置 wht 10-07-12
		else
			dlg.m_bCanModifyPos = FALSE;
		if(pLsRef->cFlag==CLsRef::DRILL_HOLE)	//螺栓孔加工工艺类型
			dlg.m_bDrillHole=TRUE;
		else
			dlg.m_bDrillHole=FALSE;
		dlg.m_bPunchHole=!dlg.m_bDrillHole;
		if(pLsRef->cFlag&CLsRef::ENLARGE_HOLE)
			dlg.m_bEnlargeHole=TRUE;
		else
			dlg.m_bEnlargeHole=FALSE;
		if(!pLsRef->GetLsPtr()->IsLocalCoordLocation(pPlate))
			dlg.m_bCanModifyPos=FALSE;
		if(pLsRef->GetLsPtr()->IsLocalCoordLocation(pPlate))
		{
			ls_pos.x=pLsRef->GetLsPtr()->des_base_pos.len_offset_dist;
			ls_pos.y=pLsRef->GetLsPtr()->des_base_pos.wing_offset_dist;
		}
		else if(pPlate->GetClassTypeId()==CLS_PLATE)
			pPlate.PlatePointer()->SpreadLsPos(pLsRef,ls_pos);
		else
		{
			ls_pos=pLsRef->GetLsPtr()->ucs.origin;
			coord_trans(ls_pos,pPlate->ucs,FALSE);
		}
		coord_trans(ls_pos,console.m_curWCS,FALSE);
		dlg.m_nWaistLen = pLsRef->waistLen;
		dlg.m_fPosX = ls_pos.x;
		dlg.m_fPosY = ls_pos.y;
		dlg.m_fPosZ = ls_pos.z;
		dlg.m_fHoleD = dlg.m_pLs->get_d()+dlg.m_pLs->hole_d_increment;
		dlg.m_hLs.Format("0X%X",dlg.m_pLs->handle);
		dlg.m_sLsGuiGe.Format("%sM%dX%.0f",dlg.m_pLs->Grade(),dlg.m_pLs->get_d(),dlg.m_pLs->get_L());
		dlg.m_dwRayNo	= pLsRef->dwRayNo;
		dlg.m_fNormX=dlg.m_pLs->get_norm().x;
		dlg.m_fNormY=dlg.m_pLs->get_norm().y;
		dlg.m_fNormZ=dlg.m_pLs->get_norm().z;
		//腰圆孔方向
		dlg.waist_vec=dlg.m_pLs->ucs.axis_x;
		vector_trans(dlg.waist_vec,pPlate->ucs,FALSE);
		vector_trans(dlg.waist_vec,console.m_curWCS,FALSE);
		if(dlg.DoModal()==IDOK)
		{
			if(pLsRef->waistLen!=dlg.m_nWaistLen)
			{
				pLsRef->waistLen = dlg.m_nWaistLen;
				console.m_pWorkPart->SetModified();
			}
			if(((int)pLsRef->waistLen)>0)
			{	//更新腰圆孔方向
				vector_trans(dlg.waist_vec,console.m_curWCS,TRUE);
				vector_trans(dlg.waist_vec,pPlate->ucs,TRUE);
				dlg.m_pLs->ucs.axis_x=dlg.waist_vec;
			}
			if(dlg.m_bCanModifyPos)
			{
				ls_pos.Set(dlg.m_fPosX,dlg.m_fPosY,dlg.m_fPosZ);
				coord_trans(ls_pos,console.m_curWCS,TRUE);
				if(pLsRef->GetLsPtr()->IsLocalCoordLocation(pPlate))
				{
					(*pLsRef)->des_base_pos.len_offset_dist=ls_pos.x;
					(*pLsRef)->des_base_pos.wing_offset_dist=ls_pos.y;
					(*pLsRef)->correct_pos();
				}
				else
				{
					if(pPlate->GetClassTypeId()==CLS_PLATE)
					{
						int iFace=pPlate.PlatePointer()->GetNormFaceI((*pLsRef)->get_norm());
						ls_pos=pPlate.PlatePointer()->RestoreLsPos(ls_pos,iFace);
					}
					else
						coord_trans(ls_pos,pPlate->ucs,TRUE);
					dlg.m_pLs->ucs.origin=ls_pos;
				}
			}
			restore_Ls_guige(dlg.m_sLsGuiGe,ls);
			dlg.m_pLs->SetGrade(ls.Grade());
			dlg.m_pLs->set_d(ls.get_d());
			dlg.m_pLs->set_L(ls.get_L());
			dlg.m_pLs->hole_d_increment = dlg.m_fHoleD-dlg.m_pLs->get_d();
			pLsRef->dwRayNo			= dlg.m_dwRayNo;
			(*pLsRef)->m_cFuncType=(BYTE)dlg.m_iHoleFuncType;
			if(dlg.m_iHoleFuncType>=2)
				(*pLsRef)->m_bVirtualPart=true;
			if(dlg.m_bDrillHole)
				pLsRef->cFlag=CLsRef::DRILL_HOLE;
			else
				pLsRef->cFlag=CLsRef::PUNCH_HOLE;
			if(dlg.m_bEnlargeHole)
				pLsRef->cFlag|=CLsRef::ENLARGE_HOLE;
			pPlate->SetModified();
			if(dlg.m_pLs->des_work_norm.norm_style==0)
				(*pLsRef)->set_norm(f3dPoint(dlg.m_fNormX,dlg.m_fNormY,dlg.m_fNormZ));
			g_pSolidDraw->BuildDisplayList(this);
		}
		else
			return FALSE;
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINESLOT||console.m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT)
	{
		CLDSLineSlot *pSlot=(CLDSLineSlot*)console.m_pWorkPart;
		pLsRef=pSlot->FindLsByHandle(pCircle->ID);
		if(pLsRef==NULL)
			return FALSE;
		dlg.m_pLs=pLsRef->GetLsPtr();
		ls_init_pos=dlg.m_pLs->ucs.origin;	//记录螺栓初始位置
		dlg.m_iHoleFuncType=pLsRef->GetLsPtr()->FuncType();
		if(dlg.m_iHoleFuncType<2&&pLsRef->GetLsPtr()->m_bVirtualPart)
			dlg.m_iHoleFuncType=2;	//默认旧版文件中的虚拟螺栓为挂线孔
		if(dlg.m_pLs->des_base_pos.datumPoint.datum_pos_style==0)
			dlg.m_bCanModifyPos = TRUE;	//基点定位方式为手动输入时才允许在构件编辑器中调整螺栓位置 wht 10-07-12
		else
			dlg.m_bCanModifyPos=FALSE;
		UCS_STRU ucs=pSlot->BuildUCS();
		ls_pos = dlg.m_pLs->ucs.origin;
		coord_trans(ls_pos,ucs,FALSE);
		double depth=ls_pos.y;
		coord_trans(ls_pos,ucs,TRUE);
		Int3dlf(ls_pos,ls_pos,dlg.m_pLs->get_norm(),pSlot->Start(),pSlot->ucs.axis_y);
		coord_trans(ls_pos,ucs,FALSE);
		ls_pos.Set(ls_pos.z,ls_pos.x,depth);
		coord_trans(ls_pos,console.m_curWCS,FALSE);
		dlg.m_fPosX = ls_pos.x;
		dlg.m_fPosY = ls_pos.y;
		dlg.m_fPosZ = ls_pos.z;
		dlg.m_fHoleD = dlg.m_pLs->get_d()+dlg.m_pLs->hole_d_increment;
		dlg.m_hLs.Format("0X%X",dlg.m_pLs->handle);
		dlg.m_sLsGuiGe.Format("%sM%dX%.0f",dlg.m_pLs->Grade(),dlg.m_pLs->get_d(),dlg.m_pLs->get_L());
		dlg.m_nWaistLen = pLsRef->waistLen;
		dlg.m_dwRayNo	= pLsRef->dwRayNo;
		dlg.m_fNormX=dlg.m_pLs->get_norm().x;
		dlg.m_fNormY=dlg.m_pLs->get_norm().y;
		dlg.m_fNormZ=dlg.m_pLs->get_norm().z;
		//腰圆孔方向
		dlg.waist_vec=dlg.m_pLs->ucs.axis_x;
		vector_trans(dlg.waist_vec,ucs,FALSE);
		f3dPoint waist_vec(dlg.waist_vec.z,dlg.waist_vec.x,dlg.waist_vec.y);
		dlg.waist_vec=waist_vec;
		vector_trans(dlg.waist_vec,console.m_curWCS,FALSE);
		if(dlg.DoModal()==IDOK)
		{
			if(pLsRef->waistLen!=dlg.m_nWaistLen)
			{
				pLsRef->waistLen = dlg.m_nWaistLen;
				console.m_pWorkPart->SetModified();
			}
			if(((int)pLsRef->waistLen)>0)
			{	//更新腰圆孔方向
				vector_trans(dlg.waist_vec,console.m_curWCS,TRUE);
				f3dPoint waist_vec(dlg.waist_vec.y,dlg.waist_vec.z,dlg.waist_vec.x);
				vector_trans(waist_vec,ucs,TRUE);
				dlg.m_pLs->ucs.axis_x=waist_vec;
			}
			(*pLsRef)->set_norm(f3dPoint(dlg.m_fNormX,dlg.m_fNormY,dlg.m_fNormZ));
			if((*pLsRef)->des_work_norm.norm_style==0)	//指定法向方向
				(*pLsRef)->des_work_norm.vector.Set(dlg.m_fNormX,dlg.m_fNormY,dlg.m_fNormZ);
			if(dlg.m_bCanModifyPos)
			{
				ls_pos.Set(dlg.m_fPosX,dlg.m_fPosY,dlg.m_fPosZ);
				//coord_trans(ls_pos,console.m_curWCS,TRUE);
				dlg.m_pLs->ucs.origin.Set(ls_pos.y,ls_pos.z,ls_pos.x);
				coord_trans(dlg.m_pLs->ucs.origin,ucs,TRUE);
			}
			restore_Ls_guige(dlg.m_sLsGuiGe,ls);
			dlg.m_pLs->SetGrade(ls.Grade());
			dlg.m_pLs->set_d(ls.get_d());
			dlg.m_pLs->set_L(ls.get_L());
			dlg.m_pLs->hole_d_increment = dlg.m_fHoleD-dlg.m_pLs->get_d();
			(*pLsRef)->m_cFuncType=(BYTE)dlg.m_iHoleFuncType;
			if(dlg.m_iHoleFuncType>=2)
				(*pLsRef)->m_bVirtualPart=true;
			pLsRef->dwRayNo			= dlg.m_dwRayNo;
			pSlot->SetModified();
			g_pSolidDraw->BuildDisplayList(this);
		}
		else
			return FALSE;
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_ARCFLAT
			||console.m_pWorkPart->GetClassTypeId()==CLS_ARCANGLE
			||console.m_pWorkPart->GetClassTypeId()==CLS_ARCSLOT)
	{	
		CLDSArcPart *pArcPart=(CLDSArcPart*)console.m_pWorkPart;
		pLsRef=pArcPart->FindLsByHandle(pCircle->ID);
		if(pLsRef==NULL)
			return FALSE;
		dlg.m_pLs=pLsRef->GetLsPtr();
		ls_init_pos=dlg.m_pLs->ucs.origin;	//记录螺栓初始位置
		dlg.m_iHoleFuncType=pLsRef->GetLsPtr()->FuncType();
		if(dlg.m_iHoleFuncType<2&&pLsRef->GetLsPtr()->m_bVirtualPart)
			dlg.m_iHoleFuncType=2;	//默认旧版文件中的虚拟螺栓为挂线孔
		if(dlg.m_pLs->des_base_pos.datumPoint.datum_pos_style==0)
			dlg.m_bCanModifyPos = TRUE;	//基点定位方式为手动输入时才允许在构件编辑器中调整螺栓位置 wht 10-07-12
		else
			dlg.m_bCanModifyPos=FALSE;
		if(pLsRef->GetLsPtr()->des_base_pos.R<0)
			return FALSE;
		ls_pos.x = pLsRef->GetLsPtr()->des_base_pos.R*pLsRef->GetLsPtr()->des_base_pos.offset_angle;
		ls_pos.y = pLsRef->GetLsPtr()->des_base_pos.wing_offset_dist;
		if(pArcPart->GetClassTypeId()==CLS_ARCFLAT||pArcPart->GetClassTypeId()==CLS_ARCSLOT)
			ls_pos.y += pArcPart->offset_v;
		ls_pos.z = pLsRef->GetLsPtr()->des_base_pos.norm_offset.Thick(console.GetActiveModel());
		dlg.m_fPosX = ls_pos.x;
		dlg.m_fPosY = ls_pos.y;
		dlg.m_fPosZ = ls_pos.z;
		dlg.m_fHoleD = dlg.m_pLs->get_d()+dlg.m_pLs->hole_d_increment;
		dlg.m_hLs.Format("0X%X",dlg.m_pLs->handle);
		dlg.m_sLsGuiGe.Format("%sM%dX%.0f",dlg.m_pLs->Grade(),dlg.m_pLs->get_d(),dlg.m_pLs->get_L());
		dlg.m_nWaistLen = pLsRef->waistLen;
		dlg.m_dwRayNo	= pLsRef->dwRayNo;
		dlg.m_fNormX=dlg.m_pLs->get_norm().x;
		dlg.m_fNormY=dlg.m_pLs->get_norm().y;
		dlg.m_fNormZ=dlg.m_pLs->get_norm().z;
		//腰圆孔方向
		dlg.waist_vec=dlg.m_pLs->ucs.axis_x;
		vector_trans(dlg.waist_vec,pArcPart->ucs,FALSE);
		vector_trans(dlg.waist_vec,console.m_curWCS,FALSE);
		if(dlg.DoModal()==IDOK)
		{
			if(pLsRef->waistLen!=dlg.m_nWaistLen)
			{
				pLsRef->waistLen = dlg.m_nWaistLen;
				console.m_pWorkPart->SetModified();
			}
			if(((int)pLsRef->waistLen)>0)
			{	//更新腰圆孔方向
				vector_trans(dlg.waist_vec,console.m_curWCS,TRUE);
				vector_trans(dlg.waist_vec,pArcPart->ucs,TRUE);
				dlg.m_pLs->ucs.axis_x=dlg.waist_vec;
			}
			(*pLsRef)->set_norm(f3dPoint(dlg.m_fNormX,dlg.m_fNormY,dlg.m_fNormZ));
			if((*pLsRef)->des_work_norm.norm_style==0)	//指定法向方向
				(*pLsRef)->des_work_norm.vector.Set(dlg.m_fNormX,dlg.m_fNormY,dlg.m_fNormZ);
			if(dlg.m_bCanModifyPos)
			{
				dlg.m_pLs->des_base_pos.R = pArcPart->baseline.Radius();
				dlg.m_pLs->des_base_pos.offset_angle = dlg.m_fPosX/pArcPart->baseline.Radius();
				dlg.m_pLs->des_base_pos.wing_offset_dist = dlg.m_fPosY;
				if(pArcPart->GetClassTypeId()==CLS_ARCFLAT||pArcPart->GetClassTypeId()==CLS_ARCSLOT)
					dlg.m_pLs->des_base_pos.wing_offset_dist -= pArcPart->offset_v;
				dlg.m_pLs->des_base_pos.norm_offset.EmptyThick();
				dlg.m_pLs->des_base_pos.norm_offset.AddThick((int)dlg.m_fPosZ);
				ls_pos.Set(dlg.m_fPosX,dlg.m_fPosY,dlg.m_fPosZ);
				dlg.m_pLs->ucs.origin.Set(ls_pos.y,ls_pos.z,ls_pos.x);
			}
			restore_Ls_guige(dlg.m_sLsGuiGe,ls);
			dlg.m_pLs->SetGrade(ls.Grade());
			dlg.m_pLs->set_d(ls.get_d());
			dlg.m_pLs->set_L(ls.get_L());
			dlg.m_pLs->hole_d_increment = dlg.m_fHoleD-dlg.m_pLs->get_d();
			(*pLsRef)->m_cFuncType=(BYTE)dlg.m_iHoleFuncType;
			if(dlg.m_iHoleFuncType>=2)
				(*pLsRef)->m_bVirtualPart=true;
			pLsRef->dwRayNo = dlg.m_dwRayNo;
			dlg.m_pLs->correct_worknorm();
			dlg.m_pLs->correct_pos();
			pArcPart->SetModified();
			g_pSolidDraw->BuildDisplayList();
		}
		else
			return FALSE;
	}
	else
		return FALSE;
	if(!dlg.m_pLs->IsLocalCoordLocation(console.m_pWorkPart)&&
		!console.m_pWorkPart->IsArcPart()&&	//环形构件除外 wht 15-03-19
		dlg.m_pLs&&dlg.m_bCanModifyPos&&dlg.m_pLs->ucs.origin!=ls_init_pos)
	{	//螺栓位置发生变化,将螺栓基点定位方式改为直接指定 wht 10-04-14
		dlg.m_pLs->des_base_pos.datumPoint.datum_pos_style=0;
		dlg.m_pLs->des_base_pos.datumPoint.SetPosition(dlg.m_pLs->ucs.origin);
		dlg.m_pLs->des_base_pos.len_offset_dist=0;
		dlg.m_pLs->des_base_pos.wing_offset_dist=0;
	}
	return TRUE;
}

BOOL CLDSView::FinishViewCir(fAtom *pAtom)
{
	f3dCircle *pCircle;
	if(pAtom->atom_type==AtomType::prCircle)
	{
		pCircle=(f3dCircle*)pAtom;
		CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(pCircle->ID,CLS_BOLT);
		if(pBolt)
			return FinishViewLsCir(pAtom);
		else 
			return FinishViewLine(pAtom);
	}
	else
		return FinishViewLine(pAtom);
	return TRUE;
}

void ConvertPlateRollEdgeToHuoQu(CLDSPlate *pPlate);
BOOL CLDSView::FinishViewLine(fAtom *pAtom)
{
	CLineFeatDlg dlg;
	f3dLine *pLine = NULL;
	f3dArcLine arcLine;
	try{
		if(pAtom->atom_type==AtomType::prLine)
		{
			pLine = (f3dLine*)pAtom;
			arcLine.CreateMethod2(pLine->startPt,pLine->endPt,f3dPoint(0,0,1),0);
		}
		else
			arcLine = *((f3dArcLine*)pAtom);
		dlg.m_pArcLine=&arcLine;
		f3dPoint pos;
		BOOL bCir=FALSE;	//整(椭)圆
		PROFILE_VERTEX *pPrevVertex=NULL,*pStartVertex=NULL,*pEndVertex=NULL,*pNextVertex=NULL;
		int line_feature=0;
		if(pAtom->atom_type==AtomType::prCircle||pAtom->atom_type==AtomType::prEllipse)
		{
			if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate=(CLDSPlate*)console.m_pWorkPart;
				if(pPlate->m_fInnerRadius>0)
				{
					bCir=TRUE;	//整(椭)圆
					dlg.m_iEdgeType=1;
					dlg.m_fArcAngleOrR=pPlate->m_fInnerRadius;
					dlg.m_iSectorAngleStyle=1;
					dlg.m_fCenterX = pPlate->ucs.origin.x;
					dlg.m_fCenterY = pPlate->ucs.origin.y;
					dlg.m_fCenterZ = pPlate->ucs.origin.z;
					dlg.m_fColAxisX = pPlate->m_tInnerColumnNorm.x;
					dlg.m_fColAxisY = pPlate->m_tInnerColumnNorm.y;
					dlg.m_fColAxisZ = pPlate->m_tInnerColumnNorm.z;
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pPlate=(CLDSParamPlate*)console.m_pWorkPart;
				double F=0; 
				//跨越连板暂时不支持生成带内圆的钢板 wht 11-05-03
				//if(pPlate->m_iParamType==TYPE_TUBERIBPLATE)
				//	pPlate->GetDesignItemValue('F',&F);
				if(pPlate->m_iParamType==TYPE_FL||(F==1&&pPlate->m_iParamType==TYPE_TUBERIBPLATE))
				{
					f3dCircle *pCir=(f3dCircle*)pAtom;
					bCir=TRUE;	//整(椭)圆
					dlg.m_iEdgeType=1;
					dlg.m_fArcAngleOrR=pCir->radius;
					dlg.m_iSectorAngleStyle=1;
					dlg.m_fCenterX = pPlate->ucs.origin.x;
					dlg.m_fCenterY = pPlate->ucs.origin.y;
					dlg.m_fCenterZ = pPlate->ucs.origin.z;
					dlg.m_fColAxisX = pCir->norm.x;
					dlg.m_fColAxisY = pCir->norm.y;
					dlg.m_fColAxisZ = pCir->norm.z;
				}
			}
		}
		else
		{
			pos = arcLine.Start();
			coord_trans(pos,console.m_curWCS,FALSE);
			dlg.m_fStartX = pos.x;
			dlg.m_fStartY = pos.y;
			dlg.m_fStartZ = pos.z;

			pos = arcLine.End();
			coord_trans(pos,console.m_curWCS,FALSE);
			dlg.m_fEndX = pos.x;
			dlg.m_fEndY = pos.y;
			dlg.m_fEndZ = pos.z;
			if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlate=(CLDSPlate*)console.m_pWorkPart;
				f3dPoint ver;
				ver.feature=2;
				pPlate->GetDeformedVertex(ver);
				for(pStartVertex=pPlate->vertex_list.GetFirst();pStartVertex;pStartVertex=pPlate->vertex_list.GetNext())
				{
					GEPOINT vertice=GEPOINT(pStartVertex->vertex.x,pStartVertex->vertex.y);
					if(vertice.IsEqual(arcLine.Start(),0.1))
					{
						if(pPrevVertex==NULL)	//首顶点即是当前顶点,此时前一顶点是顶点表中的末顶点
						{
							pPrevVertex=pPlate->vertex_list.GetTail();
							pPlate->vertex_list.GetFirst();
						}
						pEndVertex=pPlate->vertex_list.GetNext();
						if(pEndVertex==NULL)	//末顶点是当前顶点,此时当前边的终顶点是顶点表中的首顶点
							pEndVertex=pPlate->vertex_list.GetFirst();
						pNextVertex=pPlate->vertex_list.GetNext();
						if(pNextVertex==NULL)	//末顶点是当前边终顶点,此时当前边下一顶点是顶点表中的首顶点
							pNextVertex=pPlate->vertex_list.GetFirst();
						break;
					}
					else
						pPrevVertex=pStartVertex;
				}
				//通过记录边线起始顶点序号，目前看意义不大，而且f3dPoint->GEPOINT时已无法记录额外属性，故注释掉　wjh-2013.11.05
				//if(arcLine.Start().ID>0&&arcLine.Start().ID<=pPlate->vertex_list.GetNodeNum())
				//	pStartVertex=&pPlate->vertex_list[arcLine.Start().ID-1];
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSParamPlate *pPlate=(CLDSParamPlate*)console.m_pWorkPart;
				for(pStartVertex=pPlate->vertex_list.GetFirst();pStartVertex;pStartVertex=pPlate->vertex_list.GetNext())
				{
					if(pStartVertex->vertex==arcLine.Start())
					{
						if(pPrevVertex==NULL)	//首顶点即是当前顶点,此时前一顶点是顶点表中的末顶点
						{
							pPrevVertex=pPlate->vertex_list.GetTail();
							pPlate->vertex_list.GetFirst();
						}
						pEndVertex=pPlate->vertex_list.GetNext();
						if(pEndVertex==NULL)	//末顶点是当前顶点,此时当前边的终顶点是顶点表中的首顶点
							pEndVertex=pPlate->vertex_list.GetFirst();
						pNextVertex=pPlate->vertex_list.GetNext();
						if(pNextVertex==NULL)	//末顶点是当前边终顶点,此时当前边下一顶点是顶点表中的首顶点
							pNextVertex=pPlate->vertex_list.GetFirst();
						break;
					}
					else
						pPrevVertex=pStartVertex;
				}
				//if(arcLine.Start().ID>0&&arcLine.Start().ID<=pPlate->vertex_list.GetNodeNum())
				//	pStartVertex=&pPlate->vertex_list[arcLine.Start().ID-1];
			}
			if(pLine!=NULL)
			{
				line_feature=pLine->feature;
				dlg.m_iEdgeType=0;
				dlg.m_fArcAngleOrR=0;
				dlg.m_fLength=DISTANCE(pLine->startPt,pLine->endPt);
			}
			else
			{
				line_feature=arcLine.feature;
				if(arcLine.WorkNorm().z>0)
					dlg.m_iRotateStyle=0;
				else
					dlg.m_iRotateStyle=1;
				if(pStartVertex)
				{
					dlg.m_iEdgeType=1;
					if(pStartVertex->type==1)		//指定扇形角
						dlg.m_fArcAngleOrR = 180*arcLine.SectorAngle()/Pi;
					else if(pStartVertex->type==2)	//指定R
						dlg.m_fArcAngleOrR = fabs(pStartVertex->radius);
					else if(pStartVertex->type==3)	//椭圆弧
						dlg.m_fArcAngleOrR=fabs(pStartVertex->radius);
					dlg.m_iSectorAngleStyle=pStartVertex->type-1;
					dlg.m_fCenterX = pStartVertex->center.x;
					dlg.m_fCenterY = pStartVertex->center.y;
					dlg.m_fCenterZ = pStartVertex->center.z;
					dlg.m_fColAxisX = pStartVertex->column_norm.x;
					dlg.m_fColAxisY = pStartVertex->column_norm.y;
					dlg.m_fColAxisZ = pStartVertex->column_norm.z;
				}
				/*else 
				{
				AfxMessageBox("暂时不支持在构件编辑器中调整钢板内圆环，可通过钢板外形属性进行调整！");
				return FALSE;
				}*/
				dlg.m_fLength = arcLine.Length();
			}
			if(pStartVertex&&line_feature!=2)
			{	//排除火曲线
				dlg.m_bWeldEdge=pStartVertex->m_bWeldEdge;
				if(dlg.m_bWeldEdge)
					dlg.m_nManuSpace=pStartVertex->manu_space;	//焊缝加工间隙
				if(pLine)
				{
					if(pLine->pen.style!=PS_SOLID)
					{	//已进行卷边处理的线，不再是实线
						dlg.m_bRollEdge=pStartVertex->m_bRollEdge;
						dlg.m_nRollEdgeOffsetDist=pStartVertex->roll_edge_offset_dist; //卷边外移量 wht 11-04-02
						dlg.m_nManuSpace=pStartVertex->manu_space;	//卷边高度
					}
					//对活点信息
					dlg.m_iLocalPointVec=pStartVertex->local_point_vec;
					dlg.m_fLocalPointY=pStartVertex->local_point_y;
				}
			}
			if(pLine&&pLine->feature==2)
			{	//火曲线
				dlg.line_type=pLine->feature;
				if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
				{	//钢板
					CLDSPlate* pPlate=(CLDSPlate*)console.m_pWorkPart;
					dlg.m_fHuoQuR=pPlate->GetHuoQuR(pLine->ID-1);
					dlg.m_fDeformCoef=pPlate->GetDeformCoef(pLine->ID-1);
					dlg.m_fThick=pPlate->GetThick();
					dlg.m_fHuoQuAngle=pPlate->GetHuoQuAngle(pLine->ID);
				}
				else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
				{	//参数化钢板火曲角度 wht 11-05-24
					CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.m_pWorkPart;
					dlg.m_fThick=console.m_pWorkPart->GetThick();
					double huoqu_cosa=pParamPlate->ucs.axis_z*pParamPlate->HuoquFaceNorm();
					if(fabs(huoqu_cosa)<EPS_COS)
						dlg.m_fHuoQuAngle=ACOS(huoqu_cosa);
				}
			}
			else
				dlg.line_type=0;
		}
		if(dlg.DoModal()==IDOK)
		{
			if(bCir)	//板中间的整(椭)圆 参数不能在此处修改
				return TRUE;	
			BOOL bValieEndPt=(pEndVertex&&pEndVertex->vertex.IsEqual(arcLine.End(),1))?TRUE:FALSE;
			if(pStartVertex&&bValieEndPt&&line_feature!=2)
			{	//排除火曲线(保证选中的直线为钢板实际存在的轮廓边)
				pStartVertex->m_bWeldEdge=(dlg.m_bWeldEdge!=0);
				pStartVertex->manu_space=dlg.m_nManuSpace;
				if(pLine)
				{
					pStartVertex->m_bRollEdge=(dlg.m_bRollEdge!=0);
					pStartVertex->roll_edge_offset_dist=dlg.m_nRollEdgeOffsetDist; //卷边外移量 wht 11-04-02
					if(console.m_pWorkPart&&console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
					{
						CLDSPlate *pPlate=(CLDSPlate*)console.m_pWorkPart;
						if(pStartVertex->m_bRollEdge)
							pPlate->designInfo.iProfileStyle0123=3;			//卷边之后，外形计算方式修改为仅更新坐标
					}
					//保存对活点信息
					pStartVertex->local_point_vec=dlg.m_iLocalPointVec;
					if(dlg.m_iLocalPointVec==0)
						pStartVertex->local_point_y=0;
					else
						pStartVertex->local_point_y=dlg.m_fLocalPointY;
				}
			}
			else if((pStartVertex==NULL||!bValieEndPt)&&dlg.m_bRollEdge&&console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			{	//处理多次卷边情况
				CLDSPlate *pPlate=(CLDSPlate*)console.m_pWorkPart;
				ConvertPlateRollEdgeToHuoQu(pPlate);
				for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
				{
					if(pVertex->vertex==arcLine.Start())
					{
						pVertex->m_bWeldEdge=(dlg.m_bWeldEdge!=0);
						pVertex->manu_space=dlg.m_nManuSpace;
						if(pLine)
						{
							pVertex->m_bRollEdge=(dlg.m_bRollEdge!=0);
							pVertex->roll_edge_offset_dist=dlg.m_nRollEdgeOffsetDist;
						}
						break;
					}
				}
			}
			if(pLine&&pLine->feature==2&&console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			{	//
				CLDSPlate *pPlank=(CLDSPlate*)console.m_pWorkPart;
				if(dlg.m_fHuoQuR!=pPlank->GetHuoQuR(pLine->ID-1)||dlg.m_fDeformCoef!=pPlank->GetDeformCoef(pLine->ID-1))
				{	//用户自定义参数
					pPlank->m_ciDeformType=1;
					pPlank->SetHuoQuR(dlg.m_fHuoQuR,pLine->ID-1);
					pPlank->SetDeformCoef(dlg.m_fDeformCoef,pLine->ID-1);
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate *pPlank=(CLDSPlate*)console.m_pWorkPart;
				if(pStartVertex)
				{
					if(dlg.m_iEdgeType==0)	//最终是直线
						pStartVertex->type = 0;
					else if(dlg.m_iEdgeType==1)	//最终是圆弧
					{
						pStartVertex->type = 1;
						if(dlg.m_iSectorAngleStyle==0)
						{	//指定扇形角
							pStartVertex->type=1;
							pStartVertex->sector_angle=fabs(dlg.m_fArcAngleOrR*RADTODEG_COEF);
							if(dlg.m_iRotateStyle==1)	//顺时针弧
								pStartVertex->sector_angle=-pStartVertex->sector_angle;
						}
						else if(dlg.m_iSectorAngleStyle==1)
						{	//指定R
							pStartVertex->type=2;
							pStartVertex->center.x = dlg.m_fCenterX;
							pStartVertex->center.y = dlg.m_fCenterY;
							pStartVertex->center.z = dlg.m_fCenterZ;
							pStartVertex->radius=fabs(dlg.m_fArcAngleOrR);
							if(dlg.m_iRotateStyle==1)	//顺时针弧
								pStartVertex->radius=-pStartVertex->radius;
						}
						else if(dlg.m_iSectorAngleStyle==2)
						{
							pStartVertex->type = 3;
							pStartVertex->center.x = dlg.m_fCenterX;
							pStartVertex->center.y = dlg.m_fCenterY;
							pStartVertex->center.z = dlg.m_fCenterZ;
							pStartVertex->radius = dlg.m_fArcAngleOrR;
							if(dlg.m_iRotateStyle==1)	//顺时针弧
								pStartVertex->radius =-dlg.m_fArcAngleOrR;
							pStartVertex->column_norm.x = dlg.m_fColAxisX;
							pStartVertex->column_norm.y = dlg.m_fColAxisY;
							pStartVertex->column_norm.z = dlg.m_fColAxisZ;
							if(pPrevVertex&&pStartVertex&&pEndVertex&&pNextVertex)
							{
								f3dLine prev_line(pPrevVertex->vertex,pStartVertex->vertex);
								f3dLine next_line(pEndVertex->vertex,pNextVertex->vertex);
								coord_trans(prev_line.startPt,pPlank->ucs,TRUE);
								coord_trans(prev_line.endPt,pPlank->ucs,TRUE);
								coord_trans(next_line.startPt,pPlank->ucs,TRUE);
								coord_trans(next_line.endPt,pPlank->ucs,TRUE);
								f3dPoint line_vec=prev_line.endPt-prev_line.startPt;
								f3dPoint start_vertex,end_vertex,inters1,inters2,vec1,vec2;
								if(pPrevVertex->type==0)
								{	//前一个轮廓边为直线时才可自动修正椭圆弧顶点 wht 10-06-23
									int n=Int3dlc(&inters1,&inters2,prev_line.startPt,line_vec,
										pStartVertex->center,pStartVertex->center+pStartVertex->column_norm*100,
										fabs(pStartVertex->radius),fabs(pStartVertex->radius));
									if(n==1)
										start_vertex=inters1;
									else if(n==2)
									{
										vec1=inters1-prev_line.endPt;
										vec2=inters2-prev_line.endPt;
										if(vec1*line_vec>vec2*line_vec)
											start_vertex = inters2;
										else
											start_vertex = inters1;
									}
									else
									{
#ifdef AFX_TARG_ENU_ENGLISH
										AfxMessageBox("error in input data，elliptical vertex automatica modification failed, please check data!");
#else
										AfxMessageBox("输入数据有误，椭圆顶点自动修正失败，请核实数据！");
#endif
										return FALSE;
									}
									coord_trans(start_vertex,pPlank->ucs,FALSE);
									pStartVertex->vertex.Set(start_vertex.x,start_vertex.y,start_vertex.z);
								}
								if(pEndVertex->type==0)
								{	//后一个轮廓边为直线时才可自动修正椭圆弧顶点
									line_vec=next_line.startPt-next_line.endPt;
									int n=Int3dlc(&inters1,&inters2,next_line.endPt,line_vec,
										pStartVertex->center,pStartVertex->center+pStartVertex->column_norm*100,
										fabs(pStartVertex->radius),fabs(pStartVertex->radius));
									if(n==1)
										end_vertex = inters1;
									else if(n==2)
									{
										vec1=inters1-next_line.startPt;
										vec2=inters2-next_line.startPt;
										if(vec1*line_vec>vec2*line_vec)
											end_vertex = inters2;
										else
											end_vertex = inters1;
									}
									else
									{
#ifdef AFX_TARG_ENU_ENGLISH
										AfxMessageBox("error in input data,elliptical vertex automatic modification failed, please check data!");
#else
										AfxMessageBox("输入数据有误，椭圆顶点自动修正失败，请核实数据！");
#endif
										return FALSE;
									}
									coord_trans(end_vertex,pPlank->ucs,FALSE);
									pEndVertex->vertex.Set(end_vertex.x,end_vertex.y,end_vertex.z);
								}
							}
						}
					}
					pPlank->SetModified();
				}
			}
			g_pSolidDraw->BuildDisplayList();
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	return TRUE;
}
void CLDSView::OnFeatureProp() 
{
	m_curTask = TASK_VIEW_PART_FEATURE;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	m_nObjectSnapedCounts=0;
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	m_snap_feat_atom_arr.RemoveAll();
}

CBoltPropDlg new_lsdlg;
void CLDSView::OnNewLs() 
{
	CLDSBolt *pLs=NULL,ls;
	CLsRef LsRef;
	CLDSLineAngle *pJg=NULL;
	if(m_curAppType==TA_ASSEMBLE_ENV||console.m_pWorkPart==NULL)
		return;
	if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE||console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		new_lsdlg.m_fNormX=console.m_pWorkPart->ucs.axis_z.x;
		new_lsdlg.m_fNormY=console.m_pWorkPart->ucs.axis_z.y;
		new_lsdlg.m_fNormZ=console.m_pWorkPart->ucs.axis_z.z;
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT||console.m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
	{	
		new_lsdlg.m_fNormX=console.m_pWorkPart->ucs.axis_y.x;
		new_lsdlg.m_fNormY=console.m_pWorkPart->ucs.axis_y.y;
		new_lsdlg.m_fNormZ=console.m_pWorkPart->ucs.axis_y.z;
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_ARCSLOT||console.m_pWorkPart->GetClassTypeId()==CLS_ARCANGLE||console.m_pWorkPart->GetClassTypeId()==CLS_ARCFLAT)
	{	
		CLDSArcPart *pArcPart=(CLDSArcPart*)console.m_pWorkPart;
		f3dPoint norm=console.m_pWorkPart->ucs.axis_x;
		if(fabs(pArcPart->start_rot_angle)>EPS)
			RotateVectorAroundVector(norm,pArcPart->start_rot_angle,pArcPart->ucs.axis_z);
		new_lsdlg.m_fNormX=norm.x;
		new_lsdlg.m_fNormY=norm.y;
		new_lsdlg.m_fNormZ=norm.z;
	}
	new_lsdlg.m_pWorkPart=console.m_pWorkPart;
	new_lsdlg.work_ucs=console.m_curWCS;
	if(new_lsdlg.DoModal()==IDOK)
	{
		Ta.BeginUndoListen();
		pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pLs->iSeg=console.m_pWorkPart->iSeg;
		pLs->m_cFuncType=(BYTE)new_lsdlg.m_iHoleFuncType;
		if(new_lsdlg.m_iHoleFuncType>=2)
			pLs->m_bVirtualPart=true;
		pLs->ucs.axis_x.Set();
		pLs->ucs.axis_y.Set();
		LsRef = pLs->GetLsRef();
		if(new_lsdlg.m_bDrillHole)
			LsRef.cFlag=CLsRef::DRILL_HOLE;
		else
			LsRef.cFlag=CLsRef::PUNCH_HOLE;
		if(new_lsdlg.m_bEnlargeHole)
			LsRef.cFlag|=CLsRef::ENLARGE_HOLE;
		LsRef.waistLen=new_lsdlg.m_nWaistLen;
		f3dPoint ls_pos;
		ls_pos.Set(new_lsdlg.m_fPosX,new_lsdlg.m_fPosY,new_lsdlg.m_fPosZ);
		coord_trans(ls_pos,console.m_curWCS,TRUE);
		if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			pJg=(CLDSLineAngle*)console.m_pWorkPart;
			if(theApp.m_bCooperativeWork&&(theApp.GetPermDword()&(pJg->dwPermission|pJg->dwStartPermission|pJg->dwEndPermission))==0)
			{
				console.DeletePart(pLs->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("no corresponding authority！");
#else
				AfxMessageBox("没有相应的权限！");
#endif
			}
			UCS_STRU ucs;
			pJg->getUCS(ucs);
			if(ls_pos.y>0)
			{	//Y肢上的螺栓
				pJg->SetWingYLsPos(&LsRef,ls_pos.x,ls_pos.y,ls_pos.z);
			}
			else
			{	//X肢上的螺栓
				pJg->SetWingXLsPos(&LsRef,ls_pos.x,-ls_pos.y,ls_pos.z);
			}
			if(!restore_Ls_guige(new_lsdlg.m_sLsGuiGe,ls))
			{
				console.DeletePart(pLs->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("bolts specifications input are illegal, bolt generation failed!");
#else
				AfxMessageBox("输入螺栓规格非法,螺栓生成失败!");
#endif
				return ;
			}
			pLs->set_d(ls.get_d());
			pLs->set_L(ls.get_L());
			pLs->hole_d_increment = new_lsdlg.m_fHoleD-ls.get_d();
			pLs->SetGrade(ls.Grade());
			pLs->cfgword=pJg->cfgword;	//调整螺栓配材号与基准构件配材号一致
			if(theApp.m_bCooperativeWork)
			{
				if(pJg->dwPermission&theApp.GetPermDword())
					pJg->AppendMidLsRef(LsRef);
				else if(pJg->dwStartPermission&theApp.GetPermDword())
					pJg->AppendStartLsRef(LsRef);
				else if(pJg->dwEndPermission&theApp.GetPermDword())
					pJg->AppendEndLsRef(LsRef);
				else
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("no corresponding authority！！");
#else
					AfxMessageBox("没有相应的权限！");
#endif
			}
			else
				pJg->AppendMidLsRef(LsRef);
			if(new_lsdlg.m_nWaistLen>0)
			{	//更新腰圆孔方向
				vector_trans(new_lsdlg.waist_vec,console.m_curWCS,TRUE);
				f3dPoint waist_vec(new_lsdlg.waist_vec.y,new_lsdlg.waist_vec.z,new_lsdlg.waist_vec.x);
				vector_trans(waist_vec,console.m_pWorkPart->ucs,TRUE);
				pLs->ucs.axis_x=waist_vec;
			}
		}
		else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE||console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE
			||console.m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT||console.m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
		{
			if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE||console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				new_lsdlg.bak_des_ls_pos.wing_offset_dist=pLs->des_base_pos.wing_offset_dist=ls_pos.y;
				new_lsdlg.bak_des_ls_pos.len_offset_dist=pLs->des_base_pos.len_offset_dist =ls_pos.x;
				new_lsdlg.bak_des_ls_pos.cLocationStyle=pLs->des_base_pos.cLocationStyle=DESIGN_LS_POS::LOCAL_COORD;
				new_lsdlg.bak_des_ls_norm.norm_style=3;
				new_lsdlg.bak_des_ls_norm.hVicePart=console.m_pWorkPart->handle;
				coord_trans(ls_pos,console.m_pWorkPart->ucs,TRUE);
				pLs->ucs.origin=ls_pos;
				if(new_lsdlg.m_nWaistLen>0)
				{	//更新腰圆孔方向
					vector_trans(new_lsdlg.waist_vec,console.m_curWCS,TRUE);
					vector_trans(new_lsdlg.waist_vec,console.m_pWorkPart->ucs,TRUE);
					pLs->ucs.axis_x=new_lsdlg.waist_vec;
				}
			}
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT||console.m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
			{
				f3dPoint startPt;
				UCS_STRU part_ucs;
				if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT)
				{
					startPt=((CLDSLineFlat*)console.m_pWorkPart)->Start();
					part_ucs=((CLDSLineFlat*)console.m_pWorkPart)->BuildUCS();
				}
				else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
				{
					startPt=((CLDSLineSlot*)console.m_pWorkPart)->Start();
					part_ucs=((CLDSLineSlot*)console.m_pWorkPart)->BuildUCS();
				}
				pLs->ucs.origin = startPt+part_ucs.axis_z*ls_pos.x+part_ucs.axis_x*ls_pos.y+ls_pos.z*part_ucs.axis_y;
				pLs->SetModified();
				if(new_lsdlg.m_nWaistLen>0)
				{	//更新腰圆孔方向
					vector_trans(new_lsdlg.waist_vec,console.m_curWCS,TRUE);
					f3dPoint waist_vec(new_lsdlg.waist_vec.y,new_lsdlg.waist_vec.z,new_lsdlg.waist_vec.x);
					vector_trans(waist_vec,console.m_pWorkPart->ucs,TRUE);
					pLs->ucs.axis_x=waist_vec;
				}
			}
			if(!restore_Ls_guige(new_lsdlg.m_sLsGuiGe,ls))
			{
				console.DeletePart(pLs->handle);
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("bolts specifications input are illegal, bolt generation failed!");
#else
				AfxMessageBox("输入螺栓规格非法,螺栓生成失败!");
#endif
				return ;
			}
			pLs->set_d(ls.get_d());
			pLs->set_L(ls.get_L());
			pLs->hole_d_increment = new_lsdlg.m_fHoleD-ls.get_d();
			pLs->SetGrade(ls.Grade());
			if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
				((CLDSPlate*)console.m_pWorkPart)->AppendLsRef(LsRef);
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
				((CLDSParamPlate*)console.m_pWorkPart)->AppendLsRef(LsRef);
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEFLAT)
				((CLDSLineFlat*)console.m_pWorkPart)->AppendMidLsRef(LsRef);
			else if(console.m_pWorkPart->GetClassTypeId()==CLS_LINESLOT)
				((CLDSLineSlot*)console.m_pWorkPart)->AppendMidLsRef(LsRef);
		}
		else if(console.m_pWorkPart->GetClassTypeId()==CLS_ARCSLOT||console.m_pWorkPart->GetClassTypeId()==CLS_ARCANGLE||console.m_pWorkPart->GetClassTypeId()==CLS_ARCFLAT)
		{
			CLDSArcPart *pArcPart=(CLDSArcPart*)console.m_pWorkPart;
			new_lsdlg.bak_des_ls_pos.hPart=pArcPart->handle;
			new_lsdlg.bak_des_ls_pos.R=pArcPart->baseline.Radius();
			new_lsdlg.bak_des_ls_pos.offset_angle=ls_pos.x/pArcPart->baseline.Radius();
			new_lsdlg.bak_des_ls_pos.wing_offset_dist=new_lsdlg.m_fPosY;
			if(pArcPart->GetClassTypeId()==CLS_ARCFLAT||pArcPart->GetClassTypeId()==CLS_ARCSLOT)
				new_lsdlg.bak_des_ls_pos.wing_offset_dist -= pArcPart->offset_v;
			new_lsdlg.bak_des_ls_pos.direction=0;
			pArcPart->AppendLsRef(LsRef);
			pLs->ucs.origin = pArcPart->PositionInAngle(new_lsdlg.bak_des_ls_pos.offset_angle);
			pLs->ucs.origin+= ls_pos.y*pArcPart->worknorm;
			if(new_lsdlg.m_nWaistLen>0)
			{	//更新腰圆孔方向
				vector_trans(new_lsdlg.waist_vec,console.m_curWCS,TRUE);
				f3dPoint waist_vec(new_lsdlg.waist_vec.y,new_lsdlg.waist_vec.z,new_lsdlg.waist_vec.x);
				vector_trans(waist_vec,console.m_pWorkPart->ucs,TRUE);
				pLs->ucs.axis_x=waist_vec;
			}
		}
		pLs->AddL0Thick(console.m_pWorkPart->handle,TRUE,TRUE);
		pLs->des_work_norm=new_lsdlg.bak_des_ls_norm;
		pLs->set_norm(f3dPoint(new_lsdlg.m_fNormX,new_lsdlg.m_fNormY,new_lsdlg.m_fNormZ));
		pLs->des_work_norm.vector=pLs->ucs.axis_z;
		pLs->des_base_pos=new_lsdlg.bak_des_ls_pos;
		if (console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
			pLs->des_base_pos.cLocationStyle=0;	//上一次在钢板局部坐标系中输入，当前如果是角钢是不存在局部坐标系录入坐标可能 wjh-2020.1.1
		pLs->des_base_pos.datumPoint.datum_pos_style=0;
		pLs->des_base_pos.datumPoint.SetPosition(pLs->ucs.origin);
		pLs->des_base_pos.hPart=console.m_pWorkPart->handle;
		pLs->correct_worknorm();
		pLs->correct_pos();
		pLs->CalGuigeAuto();
		g_pSolidDraw->BuildDisplayList();
		pLs->GetUCS();	//更新螺栓装配坐标系 wht 11-01-25
		if(pLs->m_bVirtualPart)	//构件上添加虚拟螺栓后设置显示构件上的螺栓孔 
			console.m_pWorkPart->m_bDisplayHole=TRUE;
		Ta.EndUndoListen();
	}
}

void CLDSView::CalDefaultAssembleCS(CLDSDbObject *pObj)
{
	if(pObj==NULL)
		return;
	CLDSLineAngle *pCommonJg;
	LoadDefaultUCS(&console.m_curWCS);
	if(pObj->GetClassTypeId()==CLS_LINEANGLE||pObj->GetClassTypeId()==CLS_LINEFLAT||
		pObj->GetClassTypeId()==CLS_LINESLOT)
	{
		UCS_STRU convert_ucs;	//使角钢/槽钢/扁钢以自左至右的形式显示的坐标系
		convert_ucs.axis_x.Set(0,0,1);
		convert_ucs.axis_y.Set(-1,0,0);
		convert_ucs.axis_z.Set(0,-1,0);
		if(pObj->GetClassTypeId()==CLS_LINEFLAT)
			console.m_curACS=((CLDSLineFlat*)pObj)->BuildUCS();
		else if(pObj->GetClassTypeId()==CLS_LINESLOT)
			console.m_curACS=((CLDSLineSlot*)pObj)->BuildUCS();
		else if(pObj->GetClassTypeId()==CLS_LINEANGLE)
			((CLDSLineAngle*)pObj)->getUCS(console.m_curACS);
		ReverseUCS(&console.m_curACS);
		vector_trans(console.m_curACS.axis_x,convert_ucs,FALSE);
		vector_trans(console.m_curACS.axis_y,convert_ucs,FALSE);
		vector_trans(console.m_curACS.axis_z,convert_ucs,FALSE);
		coord_trans(console.m_curACS.origin,convert_ucs,FALSE);
		console.m_curWCS.origin.Set(-((CLDSLinePart*)pObj)->startOdd());
	}
	else if(pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		UCS_STRU convert_ucs;	//使角钢以自左至右的形式显示的坐标系
		convert_ucs.axis_x.Set(0,0,1);
		convert_ucs.axis_y.Set(0,1,0);
		convert_ucs.axis_z.Set(-1,0,0);
		pCommonJg=(CLDSLineAngle*)console.m_pWorkPart;
		pCommonJg->getUCS(console.m_curACS,1);
		ReverseUCS(&console.m_curACS);
		vector_trans(console.m_curACS.axis_x,convert_ucs,FALSE);
		vector_trans(console.m_curACS.axis_y,convert_ucs,FALSE);
		vector_trans(console.m_curACS.axis_z,convert_ucs,FALSE);
		coord_trans(console.m_curACS.origin,convert_ucs,FALSE);
		console.m_curWCS.origin.Set(-((CLDSLinePart*)pObj)->startOdd());
	}
	else if(pObj->GetClassTypeId()==CLS_PLATE||pObj->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSPart *pPart = (CLDSPart*)pObj;
		console.m_curACS = pPart->ucs;
		ReverseUCS(&console.m_curACS);
	}
}

void CLDSView::OnAssembleCs() 
{
	//CPartUcsDlg dlg;
	CCoordDefDlg dlg;
	if(console.m_pWorkPart==NULL)
		return;
	dlg.ucs = console.m_pWorkPart->ucs;
	dlg.m_bCanModifyAxisX = TRUE;
	dlg.m_bCanModifyAxisY = TRUE;
	dlg.m_bCanModifyAxisZ = TRUE;
	if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle *pCommonJg;
		UCS_STRU convert_ucs;	//使角钢以自左至右的形式显示的坐标系
		convert_ucs.axis_x.Set(0,0,1);
		convert_ucs.axis_y.Set(-1,0,0);
		convert_ucs.axis_z.Set(0,-1,0);
		pCommonJg=(CLDSLineAngle*)console.m_pWorkPart;
		pCommonJg->getUCS(dlg.ucs);
	}
	if(dlg.DoModal()==IDOK&&console.m_pWorkPart->GetClassTypeId()==CLS_PLATE||console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		console.m_pWorkPart->ucs = dlg.ucs;
		((CLDSPlate*)console.m_pWorkPart)->SetModified();
	}
}

void CLDSView::OnWorkCs() 
{
	//CPartUcsDlg dlg;
	CCoordDefDlg dlg;
	dlg.ucs = console.m_curWCS;
	dlg.m_bCanModifyAxisZ = FALSE;
	if(dlg.DoModal()==IDOK)
	{
		console.m_curWCS = dlg.ucs;
		g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS);
		Invalidate(FALSE);
	}
}

void CLDSView::OnSpecWcsOrginByGlobalCs() 
{
	if(console.m_pWorkPart==NULL)
		return;
	CModifyVertexDlg orgDlg;
#ifdef AFX_TARG_ENU_ENGLISH
	orgDlg.m_sTitle="coordilates of Work coordinate system origin ";
#else
	orgDlg.m_sTitle="工作坐标系原点的全局坐标";
#endif
	f3dPoint origin = console.m_curWCS.origin;
	coord_trans(origin,console.m_pWorkPart->ucs,TRUE);
	orgDlg.m_fVertexPosX=origin.x;
	orgDlg.m_fVertexPosY=origin.y;
	orgDlg.m_fVertexPosZ=origin.z;
	if(orgDlg.DoModal()==IDOK)
	{
		origin.Set(orgDlg.m_fVertexPosX,orgDlg.m_fVertexPosY,orgDlg.m_fVertexPosZ);
		coord_trans(origin,console.m_pWorkPart->ucs,FALSE);
		console.m_curWCS.origin=origin;
		g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS);
		Invalidate(FALSE);
	}
}

BOOL CLDSView::FinishSpecWCS(fAtom *pAtom)
{
	GEPOINT spec_point;
	if(pAtom==NULL)
		return FALSE;
	if(pAtom->atom_type==AtomType::prPoint)
	{
		spec_point.x = ((f3dPoint*)pAtom)->x;
		spec_point.y = ((f3dPoint*)pAtom)->y;
		if(m_curAppType==TA_ASSEMBLE_ENV)
			spec_point.z = ((f3dPoint*)pAtom)->z;
		else
			spec_point.z = 0;

	}
	else if(pAtom->atom_type==AtomType::prCircle)
	{
		spec_point.x = ((f3dCircle*)pAtom)->centre.x;
		spec_point.y = ((f3dCircle*)pAtom)->centre.y;
		if(m_curAppType==TA_ASSEMBLE_ENV)
			spec_point.z = ((f3dCircle*)pAtom)->centre.z;
		else
			spec_point.z = 0;
	}
	else
		return FALSE;
	if(m_curTask==TASK_SPEC_WCS_ORIGIN)
		console.m_curWCS.origin = spec_point;
	else if(m_curTask==TASK_SPEC_WCS_AXIS_X)
	{
		console.m_curWCS.axis_x = spec_point-console.m_curWCS.origin;

		if(m_curAppType!=TA_ASSEMBLE_ENV)
			console.m_curWCS.axis_x.z=0;
		normalize(console.m_curWCS.axis_x);
		if(m_curAppType==TA_PART_EDITOR)
		{
			console.m_curWCS.axis_y = console.m_curWCS.axis_z^console.m_curWCS.axis_x;
			console.m_curWCS.axis_z.Set(0,0,1);
		}
		else if(console.m_pWorkPart&&m_curAppType==TA_ASSEMBLE_ENV)
		{	//建模环境中调整坐标系 wht 10-09-09
			if(console.m_pWorkPart->IsLinePart())
			{	//杆件
				if(console.m_curWCS.axis_z!=console.m_curWCS.axis_x)
				{	//Z轴与X轴不重合
					console.m_curWCS.axis_y = console.m_curWCS.axis_z^console.m_curWCS.axis_x;
					normalize(console.m_curWCS.axis_y);
					console.m_curWCS.axis_z = console.m_curWCS.axis_x^console.m_curWCS.axis_y;
					normalize(console.m_curWCS.axis_z);
				}
				else if(console.m_curWCS.axis_y!=console.m_curWCS.axis_x)
				{	//Z轴与Y轴不重合
					console.m_curWCS.axis_z = console.m_curWCS.axis_x^console.m_curWCS.axis_y;
					normalize(console.m_curWCS.axis_z);
					console.m_curWCS.axis_y = console.m_curWCS.axis_z^console.m_curWCS.axis_x;
					normalize(console.m_curWCS.axis_y);
				}
			}
			else 
				console.m_curWCS.axis_y = console.m_curWCS.axis_z^console.m_curWCS.axis_x;
		}
	}
	else if(m_curTask==TASK_SPEC_WCS_AXIS_Y)
	{
		console.m_curWCS.axis_y = spec_point-console.m_curWCS.origin;
		normalize(console.m_curWCS.axis_y);
		if(m_curAppType==TA_PART_EDITOR)
		{
			console.m_curWCS.axis_x = console.m_curWCS.axis_y^console.m_curWCS.axis_z;
			console.m_curWCS.axis_z.Set(0,0,1);
		}
		else if(console.m_pWorkPart&&m_curAppType==TA_ASSEMBLE_ENV)
		{	//建模环境中调整坐标系 wht 10-09-09
			if(console.m_pWorkPart->IsLinePart())
			{	//杆件
				if(console.m_curWCS.axis_z!=console.m_curWCS.axis_y)
				{	//Z轴与X轴不重合
					console.m_curWCS.axis_x = console.m_curWCS.axis_y^console.m_curWCS.axis_z;
					normalize(console.m_curWCS.axis_x);
					console.m_curWCS.axis_z = console.m_curWCS.axis_x^console.m_curWCS.axis_y;
					normalize(console.m_curWCS.axis_z);
				}
				else if(console.m_curWCS.axis_y!=console.m_curWCS.axis_x)
				{	//Z轴与Y轴不重合
					console.m_curWCS.axis_z = console.m_curWCS.axis_x^console.m_curWCS.axis_y;
					normalize(console.m_curWCS.axis_z);
					console.m_curWCS.axis_y = console.m_curWCS.axis_z^console.m_curWCS.axis_x;
					normalize(console.m_curWCS.axis_y);
				}
			}
			else 
				console.m_curWCS.axis_x = console.m_curWCS.axis_x^console.m_curWCS.axis_z;
		}
	}
	g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS);
	Invalidate(FALSE);
	if(m_curAppType==TA_ASSEMBLE_ENV&&console.m_pWorkPart)//更新钢板属性栏，主要目的更新顶点坐标  wht 10-09-02
		UpdatePropertyPage(console.m_pWorkPart);//DisplayPlateProperty(TRUE);
	else
	{
		g_pSolidDraw->ReleaseSnapStatus();
		UpdatePropertyPage();	
	}
	return TRUE;
}
void CLDSView::SpecWcsOrgin()
{
	//if(m_curAppType==TA_ASSEMBLE_ENV)
	//{
	//	g_pSolidSet->SetDisplayType(DISP_LINE);
	//	Invalidate(FALSE);
	//}
	m_curTask = TASK_SPEC_WCS_ORIGIN;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_POINT|SNAP_CIRCLE);
	m_nObjectSnapedCounts=0;
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	m_snap_feat_atom_arr.RemoveAll();
}
void CLDSView::SpecAxisXVertex()
{
	//if(m_curAppType==TA_ASSEMBLE_ENV)
	//{
	//	g_pSolidSet->SetDisplayType(DISP_LINE);
	//	Invalidate(FALSE);
	//}
	m_curTask = TASK_SPEC_WCS_AXIS_X;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_POINT|SNAP_CIRCLE);
	m_nObjectSnapedCounts=0;
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	m_snap_feat_atom_arr.RemoveAll();
}
void CLDSView::SpecAxisYVertex()
{
	if(m_curAppType==TA_ASSEMBLE_ENV)
	{
		g_pSolidSet->SetDisplayType(DISP_LINE);
		Invalidate(FALSE);
	}
	m_curTask = TASK_SPEC_WCS_AXIS_Y;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_POINT|SNAP_CIRCLE);
	m_nObjectSnapedCounts=0;
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	m_snap_feat_atom_arr.RemoveAll();
}

void CLDSView::OnSpecWcsOrgin(){SpecWcsOrgin();}

void CLDSView::OnSpecAxisXVertex() {SpecAxisXVertex();}

void CLDSView::OnDelPartFeature() 
{
	long *id_arr,id_num;
	id_num=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(id_num>0)
	{
		CSuperSmartPtr<CLDSPart>pWorkPart=console.m_pWorkPart;
		int nHits=0;
		for(int i=0;i<id_num;i++)
		{
			nHits++;
			BOOL blHoleRemoved=false;
			if(pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
				blHoleRemoved=pWorkPart.LineAnglePointer()->DeleteLsRef(id_arr[i]);
			else if(pWorkPart->GetClassTypeId()==CLS_PLATE)
				blHoleRemoved=pWorkPart.PlatePointer()->DeleteLsRef(id_arr[i]);
			else if(pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
				blHoleRemoved=pWorkPart.ParamPlatePointer()->DeleteLsRef(id_arr[i]);
			else if(pWorkPart->GetClassTypeId()==CLS_LINEFLAT)
				blHoleRemoved=pWorkPart.LineFlatPointer()->DeleteLsRef(id_arr[i]);
			else if(pWorkPart->GetClassTypeId()==CLS_LINESLOT)
				blHoleRemoved=pWorkPart.LineSlotPointer()->DeleteLsRef(id_arr[i]);
			else
				nHits--;
			if (!blHoleRemoved)
				continue;	//成功删除当前螺栓孔
			//虚拟螺栓一般仅存在一个构件上，故删除孔时应同时删除虚拟螺栓 wht 11-07-08
			CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
			if(pBolt&&pBolt->m_bVirtualPart)
				Ta.DelBoltAndRelativeBoltRef(pBolt->handle);
		}
		if(nHits>0)
		{
			pWorkPart->SetModified();
			g_pSolidDraw->BuildDisplayList();
		}
	}
	else
	{
		m_curTask = TASK_DEL_PART_FEATURE;
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_POINT|SNAP_CIRCLE|SNAP_LINE|SNAP_RECTANGLE);
		m_nObjectSnapedCounts=0;
		g_pSolidDraw->ReleaseSnapStatus();
		m_setSnapObjects.Empty();
		m_snap_feat_atom_arr.RemoveAll();
	}
}
BOOL CLDSView::FinishDelPartFeature(fAtom *pFeatAtom)
{
	CLDSPlate *pPlank;
	f3dPoint vertex;
	if(pFeatAtom->atom_type==AtomType::prPoint)
	{
		if(console.m_pWorkPart->IsLinePart())	//(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("can not delete rod's feature points!");
#else
			AfxMessageBox("杆件特征点不能删除!");
#endif
			return FALSE;
		}
		else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
		{
			pPlank=(CLDSPlate*)console.m_pWorkPart;
			vertex = *((f3dPoint*)pFeatAtom);
			for(int i=0;i<pPlank->designInfo.additional_vertex.GetNodeNum();i++)
			{	//删除自定义轮廓点时，从自定义轮廓点列表中删除相应顶点 否则重新设计时自定义点又会出现
				if(pPlank->designInfo.additional_vertex[i].x==vertex.x&&pPlank->designInfo.additional_vertex[i].y==vertex.y)
					pPlank->designInfo.additional_vertex.DeleteAt(i);
			}
			for(i=0;i<pPlank->vertex_list.GetNodeNum();i++)
			{
				if(pPlank->vertex_list[i].vertex.x==vertex.x&&pPlank->vertex_list[i].vertex.y==vertex.y)
				{
					if(pPlank->vertex_list[i].vertex.feature>10)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("can not delete bending points!");
#else
						AfxMessageBox("火曲点不能删除!");
#endif
						return FALSE;
					}
					pPlank->vertex_list.DeleteAt(i);
					pPlank->SetModified();
					return TRUE;
				}
			}
			if(i==pPlank->vertex_list.GetNodeNum())
			{	//删除因卷边生成的轮廓点时相当于删除卷边 wht 16-06-06
				PROFILE_VERTEX *pVertex=(PROFILE_VERTEX*)vertex.feature;
				if(pVertex&&pVertex->m_bRollEdge)
				{
					pVertex->m_bRollEdge=FALSE;
					pPlank->SetModified();
					return TRUE;
				}
			}
			return FALSE;
		}
		else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
		{	//参数化板轮廓点不允许删除
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*)console.m_pWorkPart;
			if(!pParamPlate->IsCanManualModify())
			{
				AfxMessageBox("此参数化板不支持删除特征点！");
				return FALSE;
			}
			vertex = *((f3dPoint*)pFeatAtom);
			for(int i=0;i<pParamPlate->vertex_list.GetNodeNum();i++)
			{
				if(pParamPlate->vertex_list[i].vertex==vertex)
				{
					if(pParamPlate->vertex_list[i].vertex.feature>10)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("can not delete bending points!");
#else
						AfxMessageBox("火曲点不能删除!");
#endif
						return FALSE;
					}
					pParamPlate->vertex_list.DeleteAt(i);
					pParamPlate->SetModified();
					return TRUE;
				}
			}
			return FALSE;
		}
		else
			return FALSE;
	}
	else if(pFeatAtom->atom_type==AtomType::prCircle)
	{
		BOOL bRetCode=FALSE;
		f3dCircle *pCircle=(f3dCircle*)pFeatAtom;
		console.m_pWorkPart->SetModified();
		bRetCode=console.m_pWorkPart->DeleteLsRef(pCircle->ID);
		//虚拟螺栓一般仅存在一个构件上，故删除孔时应同时删除虚拟螺栓 wht 11-07-08
		CLDSBolt *pBolt=(CLDSBolt*)console.FromPartHandle(pCircle->ID,CLS_BOLT);
		if(pBolt&&pBolt->m_bVirtualPart)
		{
			Ta.DelBoltAndRelativeBoltRef(pBolt->handle);
			//console.DeletePart(pBolt->handle);	//存在部分虚拟螺栓有两个以螺栓孔引用的情况，此时DeletePart就可能导致死机　wjh-2017.6.27
		}
		return bRetCode;
	}
	else if(pFeatAtom->atom_type==AtomType::prLine&&console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
	{
		f3dLine *pLine=(f3dLine*)pFeatAtom;
		if(pLine->feature<10&&pLine->feature!=2)	//不是火曲线
			return FALSE;
		pPlank=(CLDSPlate*)console.m_pWorkPart;
		pPlank->SetModified();
		int del_huoqu_face_i=0;
		for(PROFILE_VERTEX *pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			if(pVertex->vertex==pLine->startPt||pVertex->vertex==pLine->endPt)
			{
				if(pVertex->vertex.feature>10)
				{
					del_huoqu_face_i=__max(pVertex->vertex.feature/10,pVertex->vertex.feature%10);
					break;
				}
			}
		}
		if(del_huoqu_face_i==0&&pLine->feature>10)
		{	//删除因卷边生成的火曲线时相当于删除卷边 wht 16-06-06
			PROFILE_VERTEX *pVertex=(PROFILE_VERTEX*)pLine->feature;
			if(pVertex&&pVertex->m_bRollEdge)
			{
				pVertex->m_bRollEdge=FALSE;
				pPlank->SetModified();
				return TRUE;
			}
		}
		//1、删除火曲面后，更新火曲面上螺栓的法线
		f3dPoint huoquNorm=pPlank->huoqufacenorm[del_huoqu_face_i-2];
		f3dPoint plankNorm=pPlank->ucs.axis_z;
		ATOM_LIST<CLsRef*> lsList;
		for(CLsRef *pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			if(fabs(huoquNorm*pBolt->ucs.axis_z)>eps_cos)
				lsList.append(pLsRef);
		}
		for(CLsRef **ppLsRef=lsList.GetFirst();ppLsRef;ppLsRef=lsList.GetNext())
		{
			CLDSBolt* pBolt=(*ppLsRef)->GetLsPtr();
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==0)
			{
				pBolt->des_base_pos.cLocationStyle=1;//true;
				pBolt->des_base_pos.reside_plane=0;		//位于基准面上
				pBolt->des_base_pos.hPart=pPlank->handle;
				//pBolt->des_base_pos.len_offset_dist=ls_pos.x;
				//pBolt->des_base_pos.wing_offset_dist=ls_pos.y;
			}
			pBolt->set_norm(plankNorm);
			pBolt->des_work_norm.norm_style=3;	//构件工作法线
			pBolt->des_work_norm.hVicePart=pPlank->handle;
			pBolt->des_work_norm.reside_plane=0;
			pBolt->des_work_norm.vector=plankNorm;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
		}
		//2、删除火曲面后，调整板的外形
		pPlank->face_N--;
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			int face_i=__max(pVertex->vertex.feature/10,pVertex->vertex.feature%10);
			if(face_i==del_huoqu_face_i)
				pVertex->vertex.feature=1;
			else if(pVertex->vertex.feature>10&&face_i>1)	//另一火曲线
				pVertex->vertex.feature=12;
			else if(face_i>1)	//另一面板
				pVertex->vertex.feature=2;
		}
		if(del_huoqu_face_i==2)
		{
			pPlank->huoqufacenorm[0]=pPlank->huoqufacenorm[1];
			pPlank->huoqufacenorm[1].Set();
			pPlank->HuoQuLine[0]=pPlank->HuoQuLine[1];
		}
		g_pSolidDraw->BuildDisplayList();
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("succeeded in delete bending plane!");
#else
		MessageBox("成功删除火曲面!");
#endif
		return TRUE;
	}
	else
		return FALSE;
}

void CLDSView::OnMeasureVertexDist() 
{
	m_curTask = TASK_MEASURE_VERTEX_DIST;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_POINT|SNAP_CIRCLE);
	m_nObjectSnapedCounts=0;
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	m_snap_feat_atom_arr.RemoveAll();
}

BOOL CLDSView::FinishMeasureVertexDist(fAtom *pAtom1, fAtom *pAtom2)
{
	f3dPoint vertex1,vertex2;
	f3dCircle *pCircle;
	if(pAtom1->atom_type==AtomType::prCircle)
	{
		pCircle = (f3dCircle*)pAtom1;
		vertex1 = pCircle->centre;
	}
	else if(pAtom1->atom_type==AtomType::prPoint)
	{
		vertex1=*((f3dPoint*)pAtom1);
	}
	else
		return FALSE;
	if(pAtom2->atom_type==AtomType::prCircle)
	{
		pCircle = (f3dCircle*)pAtom2;
		vertex2 = pCircle->centre;
	}
	else if(pAtom2->atom_type==AtomType::prPoint)
	{
		vertex2=*((f3dPoint*)pAtom2);
	}
	else
		return FALSE;
	double length = DISTANCE(vertex1,vertex2);
	CString sText;
#ifdef AFX_TARG_ENU_ENGLISH
	sText.Format("distance between two points %.1fmm ",length);
#else
	sText.Format("两点间距离为%.1fmm ",length);
#endif
	MessageBox(sText);
	g_pSolidDraw->ReleaseSnapStatus();
	return TRUE;
}

void CLDSView::OnInVertex() 
{
	m_curTask = TASK_INSERT_PLANK_VERTEX;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_LINE|SNAP_CIRCLE);
	m_nObjectSnapedCounts=0;
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	m_snap_feat_atom_arr.RemoveAll();
}

BOOL CLDSView::FinishInsertPlankVertex(fAtom *pFeatAtom)
{
	static CDefProfileVertexDlg vertexdlg;
	f3dPoint insert_vertex;
	if(console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE&&console.m_pWorkPart->GetClassTypeId()!=CLS_PARAMPLATE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("only on editing plate can outline vertex be added!");
#else
		AfxMessageBox("只能在编辑板时添加轮廓顶点!");
#endif
		return FALSE;
	}
	if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{	//参数化板也支持在辅助线上添加点 wht 16-06-16
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.m_pWorkPart;
		PROFILE_VERTEX *pStartVertex=NULL,*pEndVertex=NULL;
		f3dLine *pLine=NULL;
		if(pFeatAtom->atom_type==AtomType::prLine)
			pLine = (f3dLine*)pFeatAtom;
		if(pLine)
		{
			for(PROFILE_VERTEX *pVertex=pParamPlate->vertex_list.GetFirst();pVertex;pVertex=pParamPlate->vertex_list.GetNext())
			{
				if(pVertex->vertex.x==pLine->startPt.x&&pVertex->vertex.y==pLine->startPt.y)
					pStartVertex=pVertex;
				else if(pVertex->vertex.x==pLine->endPt.x&&pVertex->vertex.y==pLine->endPt.y)
					pEndVertex=pVertex;
				if(pStartVertex&&pEndVertex)
					break;
			}
		}
		if(!pParamPlate->IsCanManualModify()&&pStartVertex&&pEndVertex)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("only after converting parametric plate into common plate, can outline vertex be added!");
#else
			AfxMessageBox("将参数化钢板转换为普通钢板后才可以添加轮廓点!");
#endif
			return FALSE;
		}
	}
	if(pFeatAtom->feature==2)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("straight line currently selected is bending line, cannot add outline vertex onto the bending line!");
#else
		AfxMessageBox("当前所选直线为火曲线,火曲线上不能添加轮廓点!");
#endif
		return FALSE;
	}
	
	f3dLine *pLine=NULL;
	f3dArcLine *pArcLine=NULL;
	f3dPoint lineStart,lineEnd;
	if(pFeatAtom->atom_type==AtomType::prLine)
	{
		pLine = (f3dLine*)pFeatAtom;
		//SetSectorAngle(0)必须在SetStart之前，否则如果datum_line为弧时会出错 wjh-2013.11.04
		vertexdlg.datum_line.SetSectorAngle(0);
		vertexdlg.datum_line.SetStart(pLine->startPt);
		vertexdlg.datum_line.SetEnd(pLine->endPt);
		lineStart = pLine->startPt;
		lineEnd = pLine->endPt;
	}
	else if(pFeatAtom->atom_type==AtomType::prArc)
	{
		pArcLine = (f3dArcLine*)pFeatAtom;
		vertexdlg.datum_line=*pArcLine;
		lineStart = pArcLine->Start();
		lineEnd = pArcLine->End();
	}
	f3dPoint start = vertexdlg.datum_line.Start();
	f3dPoint end = vertexdlg.datum_line.End();
	f3dPoint norm = vertexdlg.datum_line.WorkNorm();
	double angle = vertexdlg.datum_line.SectorAngle();
	coord_trans(start,console.m_curWCS,FALSE);
	coord_trans(end,console.m_curWCS,FALSE);
	vector_trans(norm,console.m_curWCS,FALSE);
	vertexdlg.datum_line.CreateMethod2(start,end,norm,angle);
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	f3dPoint port_pt(point.x,point.y);
	g_pSolidOper->ScreenToUser(&vertexdlg.pickPoint,port_pt);
	coord_trans(vertexdlg.pickPoint,console.m_curWCS,FALSE);
	if(vertexdlg.DoModal()==IDOK)
	{
		insert_vertex.Set(vertexdlg.m_fPosX,vertexdlg.m_fPosY);
		coord_trans(insert_vertex,console.m_curWCS,TRUE);
		insert_vertex.z=0;	//Z坐标归0
	}
	else
		return FALSE;
	if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlank=(CLDSPlate*)console.m_pWorkPart;
		int i=0;
		for(PROFILE_VERTEX *pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			if(pVertex->vertex.x==lineStart.x&&pVertex->vertex.y==lineStart.y)
			{
				lineStart.feature = pVertex->vertex.feature;
				break;
			}
		}
		
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			if(pPlank->IsPeakPointPlate()
				&&((pPlank->top_point.x==lineEnd.x&&pPlank->top_point.x==lineEnd.x)
				&&(pVertex->vertex.x==lineStart.x&&pVertex->vertex.y==lineStart.y)))
			{
				PROFILE_VERTEX *pTmVertex=pPlank->vertex_list.insert(PROFILE_VERTEX(insert_vertex.x,insert_vertex.y,pVertex->vertex.z),i+1);
				f3dPoint *pPt=pPlank->designInfo.additional_vertex.append(insert_vertex);	//将添加的轮廓点保存到固定顶点列表 
				pPt->feature=pVertex->vertex.feature;	//附加顶点所在面号
				if(pVertex->vertex.feature<10)
					pTmVertex->vertex.feature = pVertex->vertex.feature;
				else
					pTmVertex->vertex.feature = 1;
				break;
			}
			else
			{
				if(pArcLine&&fabs(pVertex->vertex.x-lineStart.x)+fabs(pVertex->vertex.y-lineStart.y)<EPS)
				{
					f3dArcLine arc = *pArcLine;
					arc.SetEnd(insert_vertex);
					if(arc.WorkNorm().z>0)
						pVertex->sector_angle=arc.SectorAngle();
					else
						pVertex->sector_angle=-arc.SectorAngle();
				}
				if(fabs(pVertex->vertex.x-lineEnd.x)+fabs(pVertex->vertex.y-lineEnd.y)<EPS)
				{
					PROFILE_VERTEX *pTmVertex=pPlank->vertex_list.insert(PROFILE_VERTEX(insert_vertex.x,insert_vertex.y,pVertex->vertex.z),i);
					f3dPoint *pPt=pPlank->designInfo.additional_vertex.append(insert_vertex);	//将添加的轮廓点保存到固定顶点列表 
					//附加顶点所在面号
					if(pVertex->vertex.feature<10)
						pPt->feature = pTmVertex->vertex.feature = pVertex->vertex.feature;
					else if(lineStart.feature<10)
						pPt->feature = pTmVertex->vertex.feature = lineStart.feature;
					else
						pPt->feature = pTmVertex->vertex.feature = 1;
					if(pArcLine)
					{
						f3dArcLine arc = *pArcLine;
						arc.SetStart(insert_vertex);
						double sector_angle=arc.SectorAngle();
						pTmVertex->type=1;
						//pPoint->layer[1]='0';
						if(arc.WorkNorm().z>0)
							pTmVertex->sector_angle=sector_angle;
						else
							pTmVertex->sector_angle=-sector_angle;
					}
					break;
				}
			}
			i++;
		}
		if(pVertex)
			pPlank->SetModified();
		else
		{
			m_xAssistPointList.append(insert_vertex);
			g_pSolidDraw->NewPoint(insert_vertex);
			return FALSE;
		}
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.m_pWorkPart;
		int i=0;
		for(PROFILE_VERTEX *pVertex=pParamPlate->vertex_list.GetFirst();pVertex;pVertex=pParamPlate->vertex_list.GetNext())
		{
			if(pVertex->vertex.x==lineStart.x&&pVertex->vertex.y==lineStart.y)
				lineStart.feature = pVertex->vertex.feature;
		}
		for(pVertex=pParamPlate->vertex_list.GetFirst();pVertex;pVertex=pParamPlate->vertex_list.GetNext())
		{
			if(pArcLine&&fabs(pVertex->vertex.x-lineStart.x)+fabs(pVertex->vertex.y-lineStart.y)<EPS)
			{
				f3dArcLine arc = *pArcLine;
				arc.SetEnd(insert_vertex);
				pVertex->sector_angle=arc.SectorAngle();
			}
			if(fabs(pVertex->vertex.x-lineEnd.x)+fabs(pVertex->vertex.y-lineEnd.y)<EPS)
			{
				PROFILE_VERTEX *pTmVertex=pParamPlate->vertex_list.insert(PROFILE_VERTEX(insert_vertex.x,insert_vertex.y,pVertex->vertex.z),i);
				if(pArcLine)
				{
					f3dArcLine arc = *pArcLine;
					arc.SetStart(insert_vertex);
					double sector_angle=arc.SectorAngle();
					pTmVertex->type=1;
					pTmVertex->sector_angle=sector_angle;
				}
				if(pVertex->vertex.feature<10)
					pTmVertex->vertex.feature = pVertex->vertex.feature;
				else if(lineStart.feature<10)
					pTmVertex->vertex.feature = lineStart.feature;
				else
					pTmVertex->vertex.feature = 1;
				break;
			}
			i++;
		}
		if(pVertex)
			pParamPlate->SetModified();
		else
		{
			m_xAssistPointList.append(insert_vertex);
			g_pSolidDraw->NewPoint(insert_vertex);
			return FALSE;
		}
	}
	g_pSolidDraw->BuildDisplayList();
	return TRUE;
}

void CLDSView::OnImportPlankProfile() 
{
	CFindPartDlg dlg;
	if(console.m_pWorkPart==NULL)
		return;
	if(console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE)
		return;
	CLDSPlate *m_pBasePlank=(CLDSPlate*)console.m_pWorkPart;
	if(dlg.DoModal()!=IDOK)
		return;
	CLDSPlate *pPlank=(CLDSPlate*)console.FromPartHandle(dlg.m_nPartHandle,CLS_PLATE);
	ATOM_LIST<PROFILE_VERTEX>bak_pt_list;
	if(pPlank&&pPlank->vertex_list.GetNodeNum()>3)
	{
		for(PROFILE_VERTEX *pVertex=m_pBasePlank->vertex_list.GetFirst();pVertex;pVertex=m_pBasePlank->vertex_list.GetNext())
			bak_pt_list.append(*pVertex);
		m_pBasePlank->vertex_list.Empty();
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
			m_pBasePlank->vertex_list.append(*pVertex);
		m_pBasePlank->HuoQuLine[0]=pPlank->HuoQuLine[0];
		m_pBasePlank->HuoQuLine[1]=pPlank->HuoQuLine[1];
		g_pSolidDraw->BuildDisplayList();
		g_pSolidOper->ZoomAll(0.95);
#ifdef AFX_TARG_ENU_ENGLISH
		if(MessageBox("are you sure to take the type of plate appearance?(Y/N)",NULL,MB_YESNO)!=IDYES)
#else
		if(MessageBox("你确认这种板外形吗?(Y/N)",NULL,MB_YESNO)!=IDYES)
#endif
		{
			m_pBasePlank->vertex_list.Empty();
			for(pVertex=bak_pt_list.GetFirst();pVertex;pVertex=bak_pt_list.GetNext())
				m_pBasePlank->vertex_list.append(*pVertex);
			g_pSolidDraw->BuildDisplayList();
		}
		else
			m_pBasePlank->jdb_style=pPlank->jdb_style;	//用于保证板类型一致
	}
	
}

void CLDSView::OnAutoCalPlankProfile() 
{
	if(console.m_pWorkPart==NULL)
		return;
	if(console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE)
		return;
	CUndoOperObject undo(&Ta);
	CLDSPlate *pPlank=(CLDSPlate*)console.m_pWorkPart;
#ifdef AFX_TARG_ENU_ENGLISH
	if(MessageBox("calculating automatically according to bolt's minimum envelop profile?(Y),or calculating according to rectangle's envelop profile(N)?","plate appearance",MB_YESNO)==IDYES)
#else
	if(MessageBox("自动按照螺栓最小包络外形计算(Y),还是按照矩形包络外形计算(N)?","钢板外形",MB_YESNO)==IDYES)
#endif
	{
		if(pPlank->CalWrapLsProfile(g_sysPara.m_nPlankEdgeAngleThreshold))
			g_pSolidDraw->BuildDisplayList();
	}
	else
	{
		if(pPlank->CalStdProfile())
			g_pSolidDraw->BuildDisplayList();
	}
}

void CLDSView::OnBendPlank() 
{
	if(console.m_pWorkPart==NULL)
		return;
	m_curTask = TASK_BEND_PLANK;
	g_pSolidSet->SetDrawType(DRAW_LINK);
	m_nObjectSnapedCounts=0;
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	m_snap_feat_atom_arr.RemoveAll();
}
void CLDSView::FinishBendPlank(fAtom *pAtomStart, fAtom *pAtomEnd)
{
	if(console.m_pWorkPart==NULL)
		return;
	if(console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE)
		return;
	if(pAtomStart->atom_type!=AtomType::prPoint||pAtomEnd->atom_type!=AtomType::prPoint)
		return;
	f3dPoint *pStart, *pEnd;
	pStart=(f3dPoint*)pAtomStart;
	pEnd  =(f3dPoint*)pAtomEnd;
	if(pStart->feature>10||pEnd->feature>10)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("the selected bending point is the end of  new bending line, plate bending failed!");
#else
		AfxMessageBox("选择了火曲点为新火曲线端点,钢板火曲失败!");
#endif
		return;
	}
	CLDSPlate *pPlank=(CLDSPlate*)console.m_pWorkPart;
	if(pPlank->face_N>=3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The system can not deal with above-three-plane bending plate, plate bending failed!");
#else
		AfxMessageBox("本系统现不能处理三面以上的火曲板,钢板火曲失败!");
#endif
		return;
	}
	static CBendPlankStyleDlg bend_style_dlg;
	f3dPoint bend_face_norm,rot_axis;
	rot_axis=*pEnd-*pStart;
	if(bend_style_dlg.m_iBendFaceStyle==2)	//相对法线
		vector_trans(pPlank->huoqufacenorm[pPlank->face_N-2],pPlank->ucs,FALSE);
	if(bend_style_dlg.m_iBendFaceStyle>0)	//输入法线值确定火曲面
	{
		bend_style_dlg.m_fNormX=pPlank->huoqufacenorm[pPlank->face_N-2].x;
		bend_style_dlg.m_fNormY=pPlank->huoqufacenorm[pPlank->face_N-2].y;
		bend_style_dlg.m_fNormZ=pPlank->huoqufacenorm[pPlank->face_N-2].z;
	}
	if(bend_style_dlg.DoModal()==IDOK)
	{
		if(bend_style_dlg.m_iBendFaceStyle==0)
		{
			bend_face_norm.Set(0,0,1);
			RotateVectorAroundVector(bend_face_norm,bend_style_dlg.m_fBendAngle*RADTODEG_COEF,rot_axis);
			vector_trans(bend_face_norm,pPlank->ucs,TRUE);
		}
		else
		{
			bend_face_norm.Set(bend_style_dlg.m_fNormX,bend_style_dlg.m_fNormY,bend_style_dlg.m_fNormZ);
			if(bend_style_dlg.m_iBendFaceStyle==2)	//相对法线转为绝对法线
				vector_trans(bend_face_norm,pPlank->ucs,TRUE);
		}
	}
	else
		return;
	SCOPE_STRU scope;//新生成的火曲面顶点组成的区域,用来判断螺栓是否在火曲面上 wht 09-09-04
	CUndoOperObject undo(&Ta);
	pPlank->face_N++;
	pPlank->HuoQuLine[pPlank->face_N-2].startPt=*pStart;
	pPlank->HuoQuLine[pPlank->face_N-2].endPt  =*pEnd;
	pPlank->HuoQuLine[pPlank->face_N-2].startPt.feature=10+pPlank->face_N;
	pPlank->HuoQuLine[pPlank->face_N-2].endPt.feature=10+pPlank->face_N;
	//如果选中的火曲点为附加顶点则将其添加到钢板轮廓点列表中 wht 11-01-15
	for(f3dPoint *pTempPt=pPlank->designInfo.additional_vertex.GetFirst();pTempPt;pTempPt=pPlank->designInfo.additional_vertex.GetNext())
	{
		if((fabs(pTempPt->x-pStart->x)<EPS2&&fabs(pTempPt->y-pStart->y)<EPS2)
			||(fabs(pTempPt->x-pEnd->x)<EPS2&&fabs(pTempPt->y-pEnd->y)<EPS2))
		{
			PROFILE_VERTEX *pVertex=NULL;
			for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
			{	//在钢板列表中超找是否已存在该顶点，避免重复添加 wht 11-02-22
				if(pVertex->vertex==*pTempPt)
					break;
			}
			if(pVertex)
				continue;
			pVertex=pPlank->vertex_list.append(PROFILE_VERTEX(*pTempPt));
			pVertex->vertex.feature=pTempPt->feature;
			pVertex->m_bCanCut=FALSE;	//设计外形时不移除手动添加的轮廓点
		}
	}
	/** 不知道之前为何要对顶点进行排序，目前发现排序后可能打乱原本顶点的合理顺序 wjh-2016.4.28
	//对顶点按逆时针进行排序
	int n=pPlank->vertex_list.GetNodeNum();
	PROFILE_VERTEX *pPntArr = new PROFILE_VERTEX[n];
	for(int i=0;i<n;i++)
	{
		pPntArr[i] = pPlank->vertex_list[i];
		pPntArr[i].vertex.z = 0;
	}
	pPlank->SortVertex(pPntArr,n);
	for(i=0;i<n;i++)
		pPlank->vertex_list[i] = pPntArr[i];
	delete []pPntArr;
	*/
	scope.VerifyVertex(*pStart); 
	scope.VerifyVertex(*pEnd);
	BOOL bStart=FALSE;
	PROFILE_VERTEX *pPt=pPlank->vertex_list.GetFirst();
	while(1)
	{
		if(bStart)	//开始火曲面
		{
			scope.VerifyVertex(pPt->vertex);//火曲面轮廓区
			if(pPt->vertex==*pEnd)
			{
				pPt->vertex.feature=pPlank->face_N+10;
				break;		//终止火曲面
			}
			if(pPt->vertex.feature==1)
				pPt->vertex.feature=pPlank->face_N;
		}
		else		//查找火曲面开始点
		{
			if(pPt->vertex==*pStart)
			{
				pPt->vertex.feature=pPlank->face_N+10;
				bStart=TRUE;
			}
		}
		pPt=pPlank->vertex_list.GetNext();
		if(pPt==NULL&&bStart)
			pPt=pPlank->vertex_list.GetFirst();
		else if(pPt==NULL)	//已经将轮廓点循环了一遍但未找到pStart,此处需要跳出否则会死循环 wht 11-01-15
			break;
	}
	normalize(bend_face_norm);
	pPlank->huoqufacenorm[pPlank->face_N-2]=bend_face_norm;
	pPlank->SetModified();
	if(pPlank->face_N==3)
		pPlank->jdb_style=1;//之前折叠板赋值为3是错的会与钢管夹板混淆，现重新修正为1，并增加IsFoldPlate()函数 wjh-2016.1.09
	//完成火曲钢板后，更新新生成的火曲面上螺栓的法线 wht 09-09-04
	CLsRef *pLsRef=NULL;
	for(pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		f3dPoint ls_pos=pBolt->ucs.origin;
		//将螺栓位置转换到钢板坐标系下
		coord_trans(ls_pos,pPlank->ucs,FALSE);
		ls_pos.z=0;	//Z坐标归零 否则不能正确判断螺栓是否位于火曲面上
		if(DistOf2dPtLine(*pStart,*pEnd,ls_pos)<0)
		{	//螺栓在火曲线右侧,说明螺栓为火曲面上的螺栓,须更新螺栓法线方向
			if(pBolt->des_base_pos.datumPoint.datum_pos_style==0)
			{
				pBolt->des_base_pos.cLocationStyle=true;
				pBolt->des_base_pos.reside_plane=(BYTE)pPlank->face_N-1;
				pBolt->des_base_pos.hPart=pPlank->handle;
				pBolt->des_base_pos.len_offset_dist=ls_pos.x;
				pBolt->des_base_pos.wing_offset_dist=ls_pos.y;
			}
			pBolt->set_norm(bend_face_norm);
			pBolt->des_work_norm.norm_style=3;	//构件工作法线
			pBolt->des_work_norm.hVicePart=pPlank->handle;
			pBolt->des_work_norm.reside_plane=(BYTE)pPlank->face_N-1;
			pBolt->des_work_norm.vector=bend_face_norm;
			pBolt->correct_worknorm();
			pBolt->correct_pos();
		}
	}
	g_pSolidDraw->BuildDisplayList();
}

//以下函数以前添加，现可能无用 WJH-2004/05/20
/*void CLDSView::OnModifyLsPos() 
{
	if(m_curAppType!=TA_PART_EDITOR)
		return;
	if(console.m_pWorkPart==NULL)
		return;
	if(console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE)
		return;
	CLDSPlate *pPlank=(CLDSPlate*)console.m_pWorkPart;
	COffsetPartDlg offset_dlg;
	offset_dlg.part_type=0;	//板
	if(offset_dlg.DoModal()!=IDOK)
		return;

	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	for(int i=0;i<n;i++)
	{
		CLDSBolt* pLs = Ta.Bolts.FromHandle(id_arr[i]);
		if(pLs)
		{
			f3dPoint ls_pos=pLs->ucs.origin;
			pPlank->SpreadLsPos(pLs,ls_pos);
			coord_trans(ls_pos,console.m_curWCS,FALSE);
			ls_pos.x+=offset_dlg.m_fBaseOffsetX;
			ls_pos.y+=offset_dlg.m_fBaseOffsetY;
			ls_pos.z+=offset_dlg.m_fBaseOffsetZ;
			coord_trans(ls_pos,console.m_curWCS,TRUE);
			coord_trans(ls_pos,pPlank->ucs,TRUE);
			pLs->ucs.origin=ls_pos);
		}
	}

	ReCreatePartSketch();
}*/

void CLDSView::OnAddFillPlank() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(console.m_pWorkPart==NULL)
		return;
	if(console.m_pWorkPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		static CAddFillPlankDlg fill_plank_dlg;
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)console.m_pWorkPart;
		if(fill_plank_dlg.m_fThick<=eps)
			fill_plank_dlg.m_fThick=pGroupJg->jg_space-g_sysPara.m_nGroupJgMachiningGap;
		if(fill_plank_dlg.DoModal()!=IDOK)
			return;
		CLDSPlate *pPlank=(CLDSPlate*)console.AppendPart(CLS_PLATE);
		pPlank->cfgword=pGroupJg->cfgword;	//调整钢板配材号与基准构件或基准节点配材号一致
		pPlank->SetPartNo(fill_plank_dlg.m_sPartNo);
		pPlank->Thick=ftoi(fill_plank_dlg.m_fThick);
		pPlank->cMaterial='S';
		pPlank->designInfo.m_hBasePart=pGroupJg->handle;
		f3dPoint vertice,origin;
		vertice.feature=1;
		vertice.Set(-fill_plank_dlg.m_fWidth*0.5,fill_plank_dlg.m_fHigh*0.5,0);
		pPlank->vertex_list.append(vertice);
		vertice.Set(-fill_plank_dlg.m_fWidth*0.5,-fill_plank_dlg.m_fHigh*0.5,0);
		pPlank->vertex_list.append(vertice);
		vertice.Set(fill_plank_dlg.m_fWidth*0.5,-fill_plank_dlg.m_fHigh*0.5,0);
		pPlank->vertex_list.append(vertice);
		vertice.Set(fill_plank_dlg.m_fWidth*0.5,fill_plank_dlg.m_fHigh*0.5,0);
		pPlank->vertex_list.append(vertice);

		origin.Set(fill_plank_dlg.m_fBaseX,fill_plank_dlg.m_fBaseY,fill_plank_dlg.m_fBaseZ);
		coord_trans(origin,console.m_curWCS,TRUE);
		coord_trans(origin,console.m_curACS,FALSE);
		pPlank->ucs.origin=origin;
		if(fill_plank_dlg.m_iPlankDirect==0)
		{	//平放
			pPlank->ucs.axis_x.Set(1,0,0);
			pPlank->ucs.axis_y.Set(0,1,0);
			pPlank->ucs.axis_z.Set(0,0,1);
		}
		else
		{	//竖放
			pPlank->ucs.axis_x.Set(1,0,0);
			pPlank->ucs.axis_y.Set(0,0,1);
			pPlank->ucs.axis_z.Set(0,-1,0);
		}
		vector_trans(pPlank->ucs.axis_x,console.m_curWCS,TRUE);
		vector_trans(pPlank->ucs.axis_y,console.m_curWCS,TRUE);
		vector_trans(pPlank->ucs.axis_z,console.m_curWCS,TRUE);
		vector_trans(pPlank->ucs.axis_x,console.m_curACS,FALSE);
		vector_trans(pPlank->ucs.axis_y,console.m_curACS,FALSE);
		vector_trans(pPlank->ucs.axis_z,console.m_curACS,FALSE);
		g_pSolidDraw->BuildDisplayList();
		Invalidate();
	}
#endif
}
//<DEVELOP_PROCESS_MARK nameId="FUNC_DESIGN_GUAXIAN_ARC">
#include "DesignWireHole.h"
//将挂线孔的设计函数封装到CDesignWireHole中，DesignWireHole.cpp  wxc-2018.5.21
//挂线孔处外形设计
void CLDSView::OnDesignGuaXianHole()
{
	logerr.ClearContents();
	if(console.m_pWorkPart==NULL ||console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE)
		return;
	//选择设计的挂线孔螺栓，确定挂线孔对应的圆弧半径
	CLDSPlate* pPlate=(CLDSPlate*)console.m_pWorkPart;
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	g_pSolidDraw->ReleaseSnapStatus();
	OnOperOther();	//清空以前任务状态
	g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_BOLT));
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("please select wire bolt's hole to be designed:","");
#else
	pCmdLine->FillCmdLine("请选择进行设计的挂线螺栓孔:","");
#endif
	CString cmdStr="";
	if(!pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);	//还原到之前的类型
		pCmdLine->CancelCmdLine();
		return;
	}
	long *id_arr;
	int n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n<=0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("no proper wire bolt's hole selected");
#else
		logerr.Log("没有选中合适的挂线螺栓孔");
#endif
		logerr.ShowToScreen();
		return;
	}
	CDesignWireHole designWireHole(pPlate);
	GUAXIANHOLE_BASE_INFO holeInfo;
	double R=40;
	BOLTSET boltSet;
	for(int i=0;i<n;i++)
	{
		CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
		if(pBolt==NULL)
			continue;
		boltSet.append(pBolt);
	}
	for(CLDSBolt *pBolt=boltSet.GetFirst();pBolt;pBolt=boltSet.GetNext())
	{
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine((char*)CXhChar50("please input arc radius corresponding with the bolts<%.0f>:",R),"");
#else
		pCmdLine->FillCmdLine((char*)CXhChar50("请输入该螺栓对应的圆弧半径<%.0f>:",R),"");
#endif
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			f3dPoint inters;
			if(strlen(cmdStr)>0)
				holeInfo.fRadius=R=atof(cmdStr);
			else
				holeInfo.fRadius=R;
			//计算螺栓位置
			pPlate->SpreadLsPos(pBolt,inters);
			inters.z=0;
			holeInfo.ls_pos=inters;
			designWireHole.AppendHoleInfo(holeInfo);
		}
	}
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
	CUndoOperObject undo(&Ta);
	//计算距挂线孔距离最近的轮廓边，根据挂线孔在轮廓边的投影距始端的距离进行排序
	if(!designWireHole.CalNearestEdgeToHole())
	{
		logerr.ShowToScreen();
		return;
	}
	//确定设计模式：外凸圆弧形 或 波浪圆弧形
	if(designWireHole.GetHoleNum()==1)
		designWireHole.m_bWaveShape=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
	else if(AfxMessageBox("plate appearance design:Y(Outer convex circular arc)/N(wave circular arc)?",MB_YESNO)==IDYES)
#else
	else if(AfxMessageBox("板外形设计:Y(外凸圆弧形)/N(波浪圆弧形)?",MB_YESNO)==IDYES)
#endif
		designWireHole.m_bWaveShape=FALSE;
	else
	{
		designWireHole.m_bWaveShape=TRUE;
		R=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine((char*)CXhChar50("please input arc radius corresponding with the bolts<%.0f>:",R),"");
#else
		pCmdLine->FillCmdLine((char*)CXhChar50("请输入该内凹处过渡圆弧半径<%.0f>:",R),"");
#endif
		if(pCmdLine->GetStrFromCmdLine(cmdStr))
		{
			if(strlen(cmdStr)>0)
				designWireHole.m_fInnerWaveArcRadius=R=atof(cmdStr);
			else
				designWireHole.m_fInnerWaveArcRadius=R;
		}
	}
	return designWireHole.DesignWirePlate();
}
//////////////////////////////////////////////////////////////////////////
/*挂板上的套管设计------wxc 2015.8.31
1、设计对话框----初始化套管信息
2、生成套管
3、生成附加环板
*/
//////////////////////////////////////////////////////////////////////////
#include "DesignBushTubeDlg.h"
void CLDSView::OnDesignBushTube()
{
	long *id_arr=NULL;
	f3dCircle* pLsCir=NULL;
	CLDSBolt* pBolt=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pBolt=(CLDSBolt*)console.FromPartHandle(id_arr[0],CLS_BOLT);
	if(pBolt==NULL||(pBolt&&pBolt->GetClassTypeId()!=CLS_BOLT))
	{	//捕捉螺栓
		g_pSolidDraw->ReleaseSnapStatus();
		CString cmdStr;
		CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("BushTube please select Bolt To Design Bush-Tube:");
#else
		cmdStr.Format("BushTube 请选择设计套管的螺栓孔:");
#endif
		DWORD dwhObj=0,dwExportFlag=0;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_BOLT)|GetSingleWord(SELECTINDEX_HOLEWALL));
		verifier.AddVerifyType(OBJPROVIDER::LINESPACE,AtomType::prCircle);
		pCmdLine->FillCmdLine(cmdStr,"");
		UINT idRawSolid = 0;
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return ;
			SELOBJ obj(dwhObj,dwExportFlag);
			idRawSolid = obj.hObj;
			if (obj.hObj < 0 && obj.iObjType == CLS_HOLEWALL)
			{
				obj.TranslateHoleWall(g_pSolidDraw);
				pBolt = (CLDSBolt*)Ta.Parts.FromHandle(obj.hObj, CLS_BOLT);
				//hHostPart = obj.hResidePart;
			}
			else
				pBolt=(CLDSBolt*)console.FromPartHandle(obj.hRelaObj,CLS_BOLT);
			if(pBolt)
			{
				pCmdLine->FinishCmdLine(CXhChar16("0x%X",pBolt->handle));
				g_pSolidDraw->SetEntSnapStatus(pBolt->handle);
				break;
			}
			else if(obj.provider==3&&obj.pAtom)
			{
				pLsCir=(f3dCircle*)obj.pAtom;
				break;
			}
			pCmdLine->FillCmdLine(cmdStr,"");
		}
		if(pBolt)
			pCmdLine->FinishCmdLine(CXhChar50("0X%X", pBolt->handle));
		g_pSolidDraw->SetEntSnapStatus(idRawSolid);
	}
	//
	GEPOINT location;
	CLDSGeneralPlate* pPlate=NULL;
	if(pBolt)
	{	//选中螺栓
		location=pBolt->ucs.origin;
		for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
		{
			if(pPart->FindLsByHandle(pBolt->handle)==NULL)
				continue;
			if(pPart->IsPlate())
			{
				pPlate=(CLDSGeneralPlate*)pPart;
				break;
			}
		}
	}
	else if(pLsCir&&console.m_pWorkPart&&console.m_pWorkPart->IsPlate())
	{	//选中板上的孔
		location=pLsCir->centre;
		pPlate=(CLDSGeneralPlate*)console.m_pWorkPart;
		coord_trans(location,pPlate->ucs,TRUE);
	}
	if(pPlate==NULL)
		return;
	CDesignBushTubeDlg designBushDlg;
	designBushDlg.m_xBush.hRelyBolt=pBolt?pBolt->handle:0;
	designBushDlg.m_xBush.hResidePlate=pPlate->handle;
	designBushDlg.m_xBush.cMaterial=pPlate->cMaterial;
	if(pBolt)
		designBushDlg.m_xBush.fOuterD=pBolt->get_d()+pBolt->hole_d_increment;
	else if(pLsCir)
		designBushDlg.m_xBush.fOuterD=pLsCir->radius*2;
	if(designBushDlg.DoModal()!=IDOK)
		return;
	//生成套管
	CLDSLineTube* pBushTube=(CLDSLineTube*)console.AppendPart(CLS_LINETUBE);
	pBushTube->m_cFuncType=CLDSLineTube::FUNC_WIREBUSH;
	pBushTube->m_hPartWeldParent=pPlate->handle;
	pBushTube->cfgword=pPlate->cfgword;
	pBushTube->iSeg=pPlate->iSeg;
	pBushTube->SetLayer(pPlate->layer());
	pBushTube->cMaterial=designBushDlg.m_xBush.cMaterial;
	pBushTube->SetPartNo(designBushDlg.m_xBush.sPartNo);
	pBushTube->size_wide=designBushDlg.m_xBush.fOuterD;	//套管直径
	pBushTube->size_thick=designBushDlg.m_xBush.fThick;	//套管厚度
	pBushTube->bush.height=(short)designBushDlg.m_xBush.fLength;
	pBushTube->bush.hBushRelyBolt=designBushDlg.m_xBush.hRelyBolt;
	pBushTube->xBushLocation.datum_pos_style=0;
	pBushTube->xBushLocation.SetPosition(location);
	pBushTube->bush.hBushResidePlate=designBushDlg.m_xBush.hResidePlate;
	pBushTube->CalPosition();
	pBushTube->BuildUCS();
	pBushTube->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pBushTube->GetSolidPartObject());
	//生成附加环板
	if(designBushDlg.m_xBush.bRingPlate)
	{	
		for(int i=0;i<2;i++)
		{
			CLDSPlate *pCirPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
			pCirPlate->m_hPartWeldParent = pBushTube->handle;
			pBushTube->cfgword=pPlate->cfgword;
			pCirPlate->iSeg=pPlate->iSeg;
			pCirPlate->SetPartNo(designBushDlg.m_xBush.ring_plate.sPartNo);
			pCirPlate->SetLayer(pPlate->layer());
			pCirPlate->cMaterial=designBushDlg.m_xBush.ring_plate.cMaterial;
			pCirPlate->Thick=designBushDlg.m_xBush.ring_plate.fThick;
			pCirPlate->m_fInnerRadius = designBushDlg.m_xBush.ring_plate.fInnerD*0.5;
			pCirPlate->m_fNormOffset = 0;
			pCirPlate->designInfo.origin.datum_pos_style=9;	//指定构件上的相对坐标
			pCirPlate->designInfo.origin.des_para.hPart=pBushTube->handle;
			pCirPlate->designInfo.norm.norm_style=5;	//基准杆件上的特征方向
			pCirPlate->designInfo.norm.hVicePart=pBushTube->handle;
			pCirPlate->designInfo.m_bEnableFlexibleDesign=TRUE;
			pCirPlate->designInfo.iProfileStyle0123=3;
			pCirPlate->designInfo.m_hBasePart=pBushTube->handle;	//避免重新设计时报警，实际上无用处 wjh-2016.3.22
			double odd=0.5*(pBushTube->bush.height-pPlate->GetThick());
			if(i==0)
			{
				pCirPlate->ucs.axis_z=pBushTube->End()-pBushTube->Start();
				normalize(pCirPlate->ucs.axis_z);
				pCirPlate->designInfo.origin.datum_pos.Set(0,0,pPlate->Thick*0.5);//pBushTube->Start()-pCirPlate->ucs.axis_z*odd;
				pCirPlate->designInfo.origin.UpdatePos(pBushTube->BelongModel());
				pCirPlate->ucs.origin=pCirPlate->designInfo.origin.Position();
				pCirPlate->designInfo.norm.direction=0;	//始->终
			}
			else
			{	
				pCirPlate->ucs.axis_z=pBushTube->Start()-pBushTube->End();
				normalize(pCirPlate->ucs.axis_z);
				pCirPlate->designInfo.origin.datum_pos.Set(0,0,-pPlate->Thick*0.5);//pBushTube->Start()-pCirPlate->ucs.axis_z*odd;
				pCirPlate->designInfo.origin.UpdatePos(pBushTube->BelongModel());
				pCirPlate->ucs.origin=pCirPlate->designInfo.origin.Position();
				pCirPlate->designInfo.norm.direction=1;	//终->始
			}
			pCirPlate->ucs.axis_x=pBushTube->ucs.axis_x;
			pCirPlate->ucs.axis_y=pCirPlate->ucs.axis_z^pCirPlate->ucs.axis_x;
			//计算顶点集合
			int piece_sector_n=360/90;
			double piece_sector_rad_angle=(2*Pi)/piece_sector_n;
			double outter_radius = designBushDlg.m_xBush.ring_plate.fOuterD*0.5;	//外径
			//pCirPlate->m_tInnerColumnNorm不能赋值，为零表示与钢板垂直，指定轴向值后当坐标转换后容易更新不及时 wjh-2016.3.22
			//pCirPlate->m_tInnerColumnNorm=pCirPlate->ucs.axis_z;
			f3dPoint vertex_outter;
			for(int i=0;i<piece_sector_n;i++)
			{
				if(i==0)
					vertex_outter=pCirPlate->ucs.origin+pCirPlate->ucs.axis_x*outter_radius;
				else
					rotate_point_around_axis(vertex_outter,piece_sector_rad_angle,pCirPlate->ucs.origin,pCirPlate->ucs.origin+pCirPlate->ucs.axis_z*100);
				vertex_outter.feature=1;
				vertex_outter.ID=2+i;
				PROFILE_VERTEX *pVertex=pCirPlate->vertex_list.append(PROFILE_VERTEX(vertex_outter,1));
				pVertex->sector_angle=piece_sector_rad_angle;
				pVertex->center=pCirPlate->ucs.origin;
			}
			for(PROFILE_VERTEX *pVertex=pCirPlate->vertex_list.GetFirst();pVertex;pVertex=pCirPlate->vertex_list.GetNext())
				coord_trans(pVertex->vertex,pCirPlate->ucs,FALSE);
			pCirPlate->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pCirPlate->GetSolidPartObject());
		}
	}
	g_pSolidDraw->Draw();
}

void CLDSView::OnSpecLsRayNo() 
{
	if(m_curAppType!=TA_PART_EDITOR)
		return;
	console.clipper_board.ClearBuffer();
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	CLsRefList lsref_list;
	DWORD dwRayNo=0;
	for(int i=0;i<n;i++)
	{
		CLDSBolt* pLs = (CLDSBolt*)console.FromPartHandle(id_arr[i],CLS_BOLT);
		if(pLs)
		{
			lsref_list.append(pLs->GetLsRef());
			dwRayNo=pLs->dwRayNo;
		}
	}
	CInputAnIntegerValDlg dlg;
	for(i=1;i<=32;i++)
	{
		if(dwRayNo&GetSingleWord(i))
		{
			dlg.m_nVal=i;
			break;
		}
	}
	if(lsref_list.GetNodeNum()>0)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		dlg.m_sValTitle="bolt's ray No.1~32";
#else
		dlg.m_sValTitle="螺栓射线号1~32";
#endif
		if(dlg.DoModal()!=IDOK)
			return;
	}
	dlg.m_nVal=min(32,dlg.m_nVal);
	for(CLsRef *pLsRef=lsref_list.GetFirst();pLsRef;pLsRef=lsref_list.GetNext())
	{
		if(pLsRef->GetLsPtr())
			(*pLsRef)->dwRayNo=GetSingleWord(dlg.m_nVal);
	}
}

void CLDSView::OnRollPlankEdge() 
{
	m_nPrevCommandID=ID_ROLL_PLANK_EDGE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Roll Edge";
#else
	m_sPrevCommandName="钢板卷边";
#endif
	RollPlankEdge();
}
int CLDSView::RollPlankEdge()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pCmdLine==NULL)
		return 0;
	CString cmdStr,errCmdStr,cmdValue;
	DWORD dwhObj=0,dwExportFlag=0;
	//选择基准钢板
	long *id_arr;
	CLDSPlate *pDatumPlate=NULL;
	if(g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pDatumPlate=(CLDSPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE);
	if(pDatumPlate==NULL||(pDatumPlate&&pDatumPlate->GetClassTypeId()!=CLS_PLATE))
	{
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("RollEdge please select to-be-roll_edge datum plate:");
		errCmdStr.Format("RollEdge select error,please select datum plate");
#else
		cmdStr.Format("RollEdge 请选择进行卷边的基准钢板:");
		errCmdStr.Format("RollEdge 没有选中正确的钢板，请重新选择基准钢板");
#endif
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE));
		pCmdLine->FillCmdLine(cmdStr,"");
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			pDatumPlate=(CLDSPlate*)console.FromPartHandle(obj.hRelaObj,CLS_PLATE);
			if(pDatumPlate&&pDatumPlate->vertex_list.GetNodeNum()>3)
				break;
			pCmdLine->FillCmdLine(errCmdStr,"");
		}	
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumPlate->handle));
		g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle);
	}
	//选择需要进行卷边的轮廓边（将钢板的显示模式调整到显示轮廓边模式）
	CUndoOperObject undo(&Ta);
	PROFILE_VERTEX* pSelVertex=NULL;
	CPlateDisplayLifeObject displayplate(pDatumPlate,1);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("RollEdge please select to-be-roll_edge datum edge-line:");
	errCmdStr.Format("RollEdge select error,please select edge-line");
#else
	cmdStr.Format("RollEdge 请选择进行卷边的轮廓边:");
	errCmdStr.Format("RollEdge 没有选中轮廓边或选择错误，请重新选择轮廓边");
#endif
	CSnapTypeVerify verifier(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbLine));
	pCmdLine->FillCmdLine(cmdStr,"");
	while(true)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
		IDbEntity* pEnt=pBuddyDrawing->GetDbEntity(obj.idEnt);
		if(pEnt && pEnt->GetDbEntType()==IDbEntity::DbLine&&pEnt->GetHiberId().masterId==pDatumPlate->handle)
		{
			DWORD dwObjectId=pEnt->GetHiberId().HiberDownId(1);
			for(PROFILE_VERTEX* pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate->vertex_list.GetNext())
			{
				if(pVertex->vertex.ID==dwObjectId)
				{
					pSelVertex=pVertex;
					break;
				}
			}
		}
		if(pSelVertex)
			break;
		pCmdLine->FillCmdLine(errCmdStr,"");
	}	
	//输入卷边高和卷边偏移距离
	int manu_space=50,roll_edge_offset_dist=0;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("RollEdge Please input roll height <%d>:",manu_space);
#else 
	cmdStr.Format("RollEdge 请输入卷边高度 <%d>:",manu_space);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue))
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		if(cmdValue.GetLength()>0)
			manu_space=atoi(cmdValue);
		if(abs(manu_space)==0)
			pCmdLine->FillCmdLine(cmdStr,"");
		else
			break;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("RollEdge Please input roll offset dist <%d>:",roll_edge_offset_dist);
#else 
	cmdStr.Format("RollEdge 请输入卷边卷边偏移距离 <%d>:",roll_edge_offset_dist);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue))
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		if(cmdValue.GetLength()>0)
			roll_edge_offset_dist=atoi(cmdValue);
		if(roll_edge_offset_dist<0)
			pCmdLine->FillCmdLine(cmdStr,"");
		else
			break;
	}
	pSelVertex->m_bRollEdge=TRUE;
	pSelVertex->manu_space=manu_space;
	pSelVertex->roll_edge_offset_dist=roll_edge_offset_dist;
	pDatumPlate->designInfo.iProfileStyle0123=3;	//卷边之后，外形计算方式修改为仅更新坐标
	//进行卷边处理并关闭显示轮廓边模式
	pDatumPlate->SetModified();
	pDatumPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pDatumPlate->GetSolidPartObject());
	g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle,false);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 1;
}
BOOL CLDSView::FinishRollPlankEdge(fAtom *pAtom)
{
	/*这部分代码已无用,将来核实无价值时应删除 wjh-2017.3.27
	if(m_curAppType!=TA_PART_EDITOR)
		return FALSE;
	if(console.m_pWorkPart==NULL||console.m_pWorkPart->GetClassTypeId()!=CLS_PLATE)
		return FALSE;
	CLDSPlate *pPlank=(CLDSPlate*)console.m_pWorkPart;
	if(pPlank->face_N>=3)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("the system can deal with three-plane bending connecting plate. the plate is three-planed, rolling property can be used to continue rolling！");
#else
		AfxMessageBox("系统只能处理三面火曲的连接板，此板已是三面板，可使用边属性中的卷边属性继续卷边！");
#endif
		OnOperOther();
		return FALSE;
	}
	if(pAtom->atom_type!=AtomType::prLine||pAtom->feature==2)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("the selected is not connecting plate's edge！");
#else
		AfxMessageBox("选择的不是连接板边！");
#endif
		return FALSE;
	}
	f3dLine *pLine=(f3dLine*)pAtom;
	int n=pPlank->vertex_list.GetNodeNum();
	f3dPoint Now,Next;
	for(int i=0;i<n;i++)
	{
		Now=pPlank->vertex_list[i].vertex;
		Next=pPlank->vertex_list[(i+1)%n].vertex;
		if(Now==pLine->startPt&&Next==pLine->endPt)
		{
			static CRollPlankEdgeDlg dlg;//输入卷边距离
			dlg.m_fRollLineOffsetDist=pPlank->GetThick();
			if(dlg.DoModal()!=IDOK)
				return FALSE;
			Ta.BeginUndoListen();
			pPlank->face_N++;
			if(pPlank->face_N==3)
				pPlank->jdb_style=1;//之前折叠板赋值为3是错的会与钢管夹板混淆，现重新修正为1，并增加IsFoldPlate()函数 wjh-2016.1.09
			f3dPoint axis_vec=Next-Now;
			normalize(axis_vec);
			f3dPoint norm=f3dPoint(0,0,1)^axis_vec;
			vector_trans(norm,pPlank->ucs,TRUE);
			pPlank->huoqufacenorm[pPlank->face_N-2]=norm;
			pPlank->designInfo.huoqufacenorm[pPlank->face_N-2].norm_style=0;
			pPlank->designInfo.huoqufacenorm[pPlank->face_N-2].vector=norm;
			//求向外的沿伸方向
			f3dPoint extend_vec;
			extend_vec.Set(Next.y-Now.y,Now.x-Next.x,0);
			normalize(extend_vec);
			//沿伸连接板边缘
			Now=Now+extend_vec*dlg.m_fRollLineOffsetDist;
			Next=Next+extend_vec*dlg.m_fRollLineOffsetDist;
			//如果当前边的一端为火曲点，需要调整火曲点位置 wht 09-08-14
			if(pPlank->HuoQuLine[0].startPt==pLine->startPt||pPlank->HuoQuLine[0].startPt==pLine->endPt)
				pPlank->HuoQuLine[0].startPt=Now;
			if(pPlank->HuoQuLine[0].endPt==pLine->startPt||pPlank->HuoQuLine[0].endPt==pLine->endPt)
				pPlank->HuoQuLine[0].endPt=Next;
			pPlank->HuoQuLine[pPlank->face_N-2].startPt=Now;
			pPlank->HuoQuLine[pPlank->face_N-2].endPt=Next;
			pPlank->vertex_list[i].vertex=Now;
			pPlank->vertex_list[(i+1)%n].vertex=Next;
			pPlank->vertex_list[i].vertex.feature=10+pPlank->face_N;
			pPlank->vertex_list[(i+1)%n].vertex.feature=10+pPlank->face_N;
			//确定卷边量
			f3dPoint inters=Now+extend_vec*dlg.m_fRollLineHeight;
			inters.feature=pPlank->face_N;
			if((i+1)%n==0)	//连接板实体外形判断有误，所以不能在表头添加火曲面点，只能在表后附加 WJH-2004.09.22
				pPlank->vertex_list.append(inters);
			else
				pPlank->vertex_list.insert(inters,(i+1)%n);
			inters=Next+extend_vec*dlg.m_fRollLineHeight;
			inters.feature=pPlank->face_N;
			if((i+2)%(n+1)==0)
				pPlank->vertex_list.append(inters);
			else
				pPlank->vertex_list.insert(inters,(i+2)%(n+1));
			pPlank->SetDocModifiedFlag();
			g_pSolidDraw->BuildDisplayList();
			Ta.EndUndoListen();
			return TRUE;
		}
	}*/
	return TRUE;
}
int CLDSView::FilletPlateCorner()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pCmdLine==NULL)
		return 0;
	CString cmdStr,errCmdStr,cmdValue;
	DWORD dwhObj=0,dwExportFlag=0;
	//选择基准钢板
	long *id_arr;
	CLDSGeneralPlate *pDatumPlate=NULL;
	if(selectObjs.GetNodeNum()>0)
		pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(selectObjs.GetFirst()->handle,CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPlate==NULL&&g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPlate==NULL)
	{
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("Chamfer please select cut angle datum plate:");
		errCmdStr.Format("Chamfer select error,please select datum plate");
#else
		cmdStr.Format("Fillet 请选择进行倒角的基准钢板:");
		errCmdStr.Format("Fillet 没有选中正确的钢板，请重新选择基准钢板");
#endif
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE));
		pCmdLine->FillCmdLine(cmdStr,"");
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(obj.hRelaObj,CLS_PLATE,CLS_PARAMPLATE);
			if(pDatumPlate&&pDatumPlate->vertex_list.GetNodeNum()>3)
				break;
			pCmdLine->FillCmdLine(errCmdStr,"");
		}	
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumPlate->handle));
		g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle);
	}
	//选择需要进行倒角的轮廓点（将钢板的显示模式调整到显示轮廓边模式）
	int num=pDatumPlate->vertex_list.GetNodeNum();
	if(pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE&&!((CLDSParamPlate*)pDatumPlate)->IsCanManualModify())
	{
		AfxMessageBox("此参数化钢板目前不支持倒角操作!");
		return 0;
	}
	CPlateDisplayLifeObject displayplate(pDatumPlate,1);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Fillet please select to-be-roll_edge datum edge-line:");
	errCmdStr.Format("Fillet select error,please select edge-line");
#else
	cmdStr.Format("Fillet 请选择进行倒角的轮廓点:");
	errCmdStr.Format("Fillet 没有选中轮廓点或选择错误，请重新选择轮廓点");
#endif
	int iSelVertex=-1;
	CSnapTypeVerify verifier(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbPoint));
	pCmdLine->FillCmdLine(cmdStr,"");
	while(true)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
		IDbEntity* pEnt=pBuddyDrawing->GetDbEntity(obj.idEnt);
		if(pEnt && pEnt->GetDbEntType()==IDbEntity::DbPoint&&pEnt->GetHiberId().masterId==pDatumPlate->handle)
		{
			DWORD dwObjectId=pEnt->GetHiberId().HiberDownId(1);
			for(PROFILE_VERTEX* pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate->vertex_list.GetNext())
			{
				if(pVertex->vertex.ID==dwObjectId)
				{
					iSelVertex=pDatumPlate->vertex_list.GetCurrentIndex();
					break;
				}
			}
		}
		if(iSelVertex>-1)
			break;
		pCmdLine->FillCmdLine(errCmdStr,"");
	}
	if(pDatumPlate->vertex_list[iSelVertex].vertex.feature>10)
	{
		AfxMessageBox("暂时不支持火曲点进行倒角操作!");
		return 0;
	}
	//输入倒角模式和倒角长度
	BYTE biCutMode=0;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Fillet Please input cut mode<0.angle|1.arc>[1]: ");
#else 
	cmdStr.Format("Fillet 请输入倒角模式<1.直角|2.圆弧> [1]: ");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue,CCmdLineDlg::KEYWORD_AS_RETURN,"1|2"))
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		if(cmdValue.GetLength()==0)
			biCutMode=1;
		else
			biCutMode=atoi(cmdValue);
		if(biCutMode==1||biCutMode==2)
			break;
		pCmdLine->FillCmdLine(cmdStr,"");
	}
	int nCutLen=30;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("Fillet Please input cut length <%d>:",nCutLen);
#else 
	cmdStr.Format("Fillet 请输入倒角长度 <%d>:",nCutLen);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	CEmbedMirInfoPanel xCmdMiniPanel;
	if(biCutMode==2)
	{	//切圆弧
		pCmdLine->AttachMiniCmdOptionPanel(&xCmdMiniPanel,200);
		xCmdMiniPanel.FillPanelTitle("切凸弧");
	}
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdValue))
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		if(cmdValue.GetLength()>0)
			nCutLen=atoi(cmdValue);
		if(nCutLen<=0)
			pCmdLine->FillCmdLine(cmdStr,"");
		else
			break;
	}
	BOOL bCutConVexArc=biCutMode==2?xCmdMiniPanel.m_bMirAll:FALSE;	//是否切凸弧
	pCmdLine->DetachMiniCmdOptionPanel();
	//
	CUndoOperObject undo(&Ta);
	f3dPoint prePt,cutPt,nextPt,offVec,preVec,nextVec;
	prePt=pDatumPlate->vertex_list[(iSelVertex-1+num)%num].vertex;
	cutPt=pDatumPlate->vertex_list[iSelVertex].vertex;
	nextPt=pDatumPlate->vertex_list[(iSelVertex+1)%num].vertex;
	preVec =(prePt-cutPt).normalized();
	nextVec=(nextPt-cutPt).normalized();
	offVec =(preVec+nextVec).normalized();
	if(DISTANCE(prePt,cutPt)>nCutLen && DISTANCE(nextPt,cutPt)>nCutLen)
	{
		f3dPoint cutPt1,cutPt2;
		//计算第一个倒角点
		if(pDatumPlate->vertex_list[(iSelVertex-1+num)%num].type==0)
			cutPt1=cutPt+preVec*nCutLen;
		else
		{
			f3dArcLine arcline;
			pDatumPlate->vertex_list[(iSelVertex-1+num)%num].RetrieveArcLine(arcline,cutPt,&pDatumPlate->ucs);
			double R=arcline.Radius(),fLen=SQRT(R*R-nCutLen*nCutLen*0.25);
			double fAngle=arcline.SectorAngle()-ACOS(fLen/R)*2;
			cutPt1=arcline.PositionInAngle(fAngle);
		}
		//计算第二个倒角点
		if(pDatumPlate->vertex_list[iSelVertex].type==0)
			cutPt2=cutPt+nextVec*nCutLen;
		else
		{
			f3dArcLine arcline;
			pDatumPlate->vertex_list[iSelVertex].RetrieveArcLine(arcline,nextPt,&pDatumPlate->ucs);
			double R=arcline.Radius(),fLen=SQRT(R*R-nCutLen*nCutLen*0.25);
			double fAngle=ACOS(fLen/R)*2;
			cutPt2=arcline.PositionInAngle(fAngle*2);
		}
		//更新选中轮廓点的坐标
		PROFILE_VERTEX cut_vertex=pDatumPlate->vertex_list[iSelVertex];
		bool bWeld=cut_vertex.m_bWeldEdge;
		cutPt1.feature=cutPt2.feature=cut_vertex.vertex.feature;
		pDatumPlate->vertex_list[iSelVertex].vertex=cutPt1;
		pDatumPlate->vertex_list[iSelVertex].m_bWeldEdge=false;
		if(biCutMode==2)
		{
			pDatumPlate->vertex_list[iSelVertex].type=2;
			pDatumPlate->vertex_list[iSelVertex].radius=bCutConVexArc?nCutLen:-nCutLen;
			if(bCutConVexArc)
				pDatumPlate->vertex_list[iSelVertex].center=cutPt+offVec*nCutLen*2;
			else
				pDatumPlate->vertex_list[iSelVertex].center=cutPt-offVec*nCutLen*2;
		}
		else
			pDatumPlate->vertex_list[iSelVertex].type=0;
		BYTE cPosCalType=pDatumPlate->vertex_list[iSelVertex].m_cPosDesignType;
		if(cPosCalType>=1&&cPosCalType<=4 && biCutMode==1)
		{
			pDatumPlate->vertex_list[iSelVertex].xOffset.ciOffMode=1;	//向前顶点偏移
			pDatumPlate->vertex_list[iSelVertex].xOffset.x=(float)nCutLen;
		}
		else 
			pDatumPlate->vertex_list[iSelVertex].m_cPosDesignType=0;	//用户自定义
		//插入一个新的轮廓点
		cPosCalType=pDatumPlate->vertex_list[iSelVertex].m_cPosDesignType;
		PROFILE_VERTEX* pVertex=pDatumPlate->vertex_list.insert(cut_vertex,(iSelVertex+1)%num);
		pVertex->vertex=cutPt2;
		pVertex->m_bWeldEdge=bWeld;
		pVertex->m_cPosDesignType=cPosCalType;	//顶点计算类型
		if(cPosCalType>=1&&cPosCalType<=4 && biCutMode==1)
		{
			pVertex->m_hRod=pDatumPlate->vertex_list[iSelVertex].m_hRod;
			pVertex->xOffset.ciOffMode=2;	//向后顶点偏移
			pVertex->xOffset.x=(float)nCutLen;
		}
		if(pDatumPlate->GetClassTypeId()==CLS_PLATE)
			((CLDSPlate*)pDatumPlate)->designInfo.iProfileStyle0123=3;
		//
		int i=0;
		for(PROFILE_VERTEX* pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate->vertex_list.GetNext())
			pVertex->vertex.ID=i++;
	}
	else
		AfxMessageBox("选中的轮廓点不适合倒角(可能倒角长度过长)!");
	//进行倒角处理并关闭显示轮廓边模式
	pDatumPlate->SetModified();
	pDatumPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pDatumPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 1;
}
//在钢板实体下插入自定义轮廓点
int CLDSView::InsertPlankVertex()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pCmdLine==NULL)
		return 0;
	DWORD dwhObj=0,dwExportFlag=0;
	//选择基准钢板
	long *id_arr;
	CLDSGeneralPlate *pDatumPlate=NULL;
	if(selectObjs.GetNodeNum()>0)
		pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(selectObjs.GetFirst()->handle,CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPlate==NULL&&g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPlate==NULL)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE));
		pCmdLine->FillCmdLine("InsertVertex 请选择待插入轮廓点的基准钢板:","");
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(obj.hRelaObj,CLS_PLATE,CLS_PARAMPLATE);
			if(pDatumPlate&&pDatumPlate->vertex_list.GetNodeNum()>3)
				break;
			pCmdLine->FillCmdLine("InsertVertex 请选择待插入轮廓点的基准钢板:","");
		}	
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumPlate->handle));
		g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle);
	}
	if(pDatumPlate->GetClassTypeId()==CLS_PARAMPLATE&&!((CLDSParamPlate*)pDatumPlate)->IsCanManualModify())
	{
		AfxMessageBox("此参数化钢板目前不支持添加轮廓点!");
		return FALSE;
	}
	//选择待插入顶点的轮廓边（将钢板的显示模式调整到显示轮廓边模式）
	int iSelIndex=0;
	PROFILE_VERTEX* pSelVertex=NULL,*pNextVertex=NULL;
	CSnapTypeVerify verifier(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbLine));
	pCmdLine->FillCmdLine("InsertVertex 请选择待插入轮廓点的轮廓边:","");
	while(true)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
		IDbEntity* pEnt=pBuddyDrawing->GetDbEntity(obj.idEnt);
		if(pEnt && pEnt->GetDbEntType()==IDbEntity::DbLine&&pEnt->GetHiberId().masterId==pDatumPlate->handle)
		{
			DWORD dwObjectId=pEnt->GetHiberId().HiberDownId(1);
			for(PROFILE_VERTEX* pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate->vertex_list.GetNext())
			{
				if(pVertex->vertex.ID==dwObjectId)
				{
					iSelIndex=pDatumPlate->vertex_list.GetCurrentIndex();
					pSelVertex=pVertex;
					pNextVertex=pDatumPlate->vertex_list.GetNext();
					if(pNextVertex==NULL)
						pNextVertex=pDatumPlate->vertex_list.GetFirst();
					break;
				}
			}
		}
		if(pSelVertex)
			break;
		pCmdLine->FillCmdLine("InsertVertex 请选择待插入轮廓点的轮廓边:","");
	}
	//
	CDefProfileVertexDlg vertexdlg;
	int sign=1;
	f3dPoint ptS,ptE,norm;
	ptS=pSelVertex->vertex;
	ptE=pNextVertex->vertex;
	if(pSelVertex->type==0)
	{
		vertexdlg.datum_line.SetSectorAngle(0);
		vertexdlg.datum_line.SetStart(ptS);
		vertexdlg.datum_line.SetEnd(ptE);
	}
	else if(pSelVertex->type==1)
	{
		if(pSelVertex->sector_angle<0)
			sign=-1;
		vertexdlg.datum_line.CreateMethod2(ptS,ptE,f3dPoint(0,0,sign),fabs(pSelVertex->sector_angle));
	}
	else if(pSelVertex->type==2)
		vertexdlg.datum_line.CreateMethod3(ptS,ptE,f3dPoint(0,0,1),pSelVertex->radius,pSelVertex->center);
	else if(pSelVertex->type==3)
	{
		f3dPoint center = pSelVertex->center;
		f3dPoint col_axis=pSelVertex->column_norm;
		coord_trans(center,pDatumPlate->ucs,FALSE);
		vector_trans(col_axis,pDatumPlate->ucs,FALSE);
		if(pSelVertex->radius<0)
			sign=-1;
		vertexdlg.datum_line.CreateEllipse(center,ptS,ptE,col_axis,f3dPoint(0,0,sign),sign*pSelVertex->radius);
	}
	ptS =vertexdlg.datum_line.Start();
	ptE =vertexdlg.datum_line.End();
	norm=vertexdlg.datum_line.WorkNorm();
	coord_trans(ptS,console.m_curWCS,FALSE);
	coord_trans(ptE,console.m_curWCS,FALSE);
	vector_trans(norm,console.m_curWCS,FALSE);
	vertexdlg.datum_line.CreateMethod2(ptS,ptE,norm,vertexdlg.datum_line.SectorAngle());
	if(vertexdlg.DoModal()!=IDOK)
		return 0;
	//插入轮廓点
	CUndoOperObject undo(&Ta);
	f3dPoint insert_vertex(vertexdlg.m_fPosX,vertexdlg.m_fPosY);
	coord_trans(insert_vertex,console.m_curWCS,TRUE);
	insert_vertex.z=0;
	int insert_index=(iSelIndex+1)%(pDatumPlate->vertex_list.GetNodeNum());
	PROFILE_VERTEX *pTmVertex=pDatumPlate->vertex_list.insert(PROFILE_VERTEX(insert_vertex),insert_index);
	pTmVertex->vertex.feature=pSelVertex->vertex.feature;	
	if(pDatumPlate->GetClassTypeId()==CLS_PLATE)
	{	//将添加的轮廓点保存到固定顶点列表 
		CLDSPlate* pPlate=(CLDSPlate*)pDatumPlate;
		pPlate->designInfo.iProfileStyle0123=3;		//仅更新坐标
		f3dPoint* pPt=pPlate->designInfo.additional_vertex.append(insert_vertex);
		pPt->feature=pTmVertex->vertex.feature;
	}
	//
	pDatumPlate->SetModified();
	pDatumPlate->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pDatumPlate->GetSolidPartObject());
	CLDSView::RedrawPlateProfileDrawing(pDatumPlate,NULL,true,0x03);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 1;
}
//在钢板实体下插入螺栓孔
int CLDSView::InsertPlankBolt()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pCmdLine==NULL)
		return 0;
	//选择基准钢板
	long *id_arr;
	CLDSGeneralPlate *pDatumPlate=NULL;
	if(selectObjs.GetNodeNum()>0)
		pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(selectObjs.GetFirst()->handle,CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPlate==NULL&&g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPlate==NULL)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		DWORD dwhObj=0,dwExportFlag=0;
		CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE));
		pCmdLine->FillCmdLine("InsertVertex 请选择待插入轮廓点的基准钢板:","");
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
				return 0;
			SELOBJ obj(dwhObj,dwExportFlag);
			pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(obj.hRelaObj,CLS_PLATE,CLS_PARAMPLATE);
			if(pDatumPlate&&pDatumPlate->vertex_list.GetNodeNum()>3)
				break;
			pCmdLine->FillCmdLine("InsertVertex 请选择待插入轮廓点的基准钢板:","");
		}	
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumPlate->handle));
		g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle);
	}
	new_lsdlg.m_fNormX=pDatumPlate->ucs.axis_z.x;
	new_lsdlg.m_fNormY=pDatumPlate->ucs.axis_z.y;
	new_lsdlg.m_fNormZ=pDatumPlate->ucs.axis_z.z;
	new_lsdlg.m_pWorkPart=pDatumPlate;
	new_lsdlg.work_ucs=console.m_curWCS;
	if(new_lsdlg.DoModal()!=IDOK)
		return 0;
	CLDSBolt ls;
	if(!restore_Ls_guige(new_lsdlg.m_sLsGuiGe,ls))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("bolts specifications input are illegal, bolt generation failed!");
#else
		AfxMessageBox("输入螺栓规格非法,螺栓生成失败!");
#endif
		return 0;
	}
	f3dPoint ls_pos(new_lsdlg.m_fPosX,new_lsdlg.m_fPosY,new_lsdlg.m_fPosZ);
	coord_trans(ls_pos,console.m_curWCS,TRUE);
	//
	CUndoOperObject undo(&Ta);
	CLDSBolt* pNewBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
	pNewBolt->iSeg=pDatumPlate->iSeg;
	pNewBolt->m_cFuncType=(BYTE)new_lsdlg.m_iHoleFuncType;
	pNewBolt->m_bVirtualPart=(new_lsdlg.m_iHoleFuncType>=2)?true:false;
	pNewBolt->set_d(ls.get_d());
	pNewBolt->set_L(ls.get_L());
	pNewBolt->SetGrade(ls.Grade());
	pNewBolt->hole_d_increment=new_lsdlg.m_fHoleD-ls.get_d();
	pNewBolt->AddL0Thick(pDatumPlate->handle,TRUE,TRUE);
	CLsRef LsRef=pNewBolt->GetLsRef();
	LsRef.cFlag=(new_lsdlg.m_bDrillHole)?CLsRef::DRILL_HOLE:CLsRef::PUNCH_HOLE;
	if(new_lsdlg.m_bEnlargeHole)
		LsRef.cFlag|=CLsRef::ENLARGE_HOLE;
	LsRef.waistLen=new_lsdlg.m_nWaistLen;
	if(new_lsdlg.m_nWaistLen>0)
	{	//更新腰圆孔方向
		vector_trans(new_lsdlg.waist_vec,console.m_curWCS,TRUE);
		vector_trans(new_lsdlg.waist_vec,pDatumPlate->ucs,TRUE);
		pNewBolt->ucs.axis_x=new_lsdlg.waist_vec;
	}
	pNewBolt->des_base_pos.datumPoint.datum_pos_style=0;
	pNewBolt->des_base_pos.cLocationStyle=DESIGN_LS_POS::LOCAL_COORD;
	pNewBolt->des_base_pos.hPart=pDatumPlate->handle;
	pNewBolt->des_base_pos.len_offset_dist=ls_pos.x;
	pNewBolt->des_base_pos.wing_offset_dist=ls_pos.y;
	pNewBolt->des_work_norm.norm_style=3;
	pNewBolt->des_work_norm.direction=0;
	pNewBolt->des_work_norm.hVicePart=pDatumPlate->handle;
	pNewBolt->correct_worknorm();
	pNewBolt->correct_pos();
	pNewBolt->CalGuigeAuto();
	pNewBolt->GetUCS();
	pNewBolt->SetModified();
	pNewBolt->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pNewBolt->GetSolidPartObject());
	//
	pDatumPlate->AppendLsRef(LsRef);
	pDatumPlate->m_bDisplayHole=pNewBolt->m_bVirtualPart?TRUE:FALSE;
	pDatumPlate->SetModified();
	pDatumPlate->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pDatumPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 1;
}
//删除钢板特征属性(轮廓点、螺栓孔)
int CLDSView::DeletePlankFeature()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pCmdLine==NULL)
		return 0;
	//选择基准钢板
	long *id_arr;
	CSnapTypeVerify verifier;
	DWORD dwhObj=0,dwExportFlag=0;
	CLDSGeneralPlate *pDatumPlate=NULL;
	if(selectObjs.GetNodeNum()>0)
		pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(selectObjs.GetFirst()->handle,CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPlate==NULL&&g_pSolidSnap->GetLastSelectEnts(id_arr)==1)
		pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(id_arr[0],CLS_PLATE,CLS_PARAMPLATE);
	if(pDatumPlate==NULL)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE));
		pCmdLine->FillCmdLine("InsertVertex 请选择待操作的基准钢板:","");
		while(true)
		{
			if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
			{
				pCmdLine->CancelCmdLine();
				return 0;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(obj.hRelaObj,CLS_PLATE,CLS_PARAMPLATE);
			if(pDatumPlate&&pDatumPlate->vertex_list.GetNodeNum()>3)
				break;
			pCmdLine->FillCmdLine("InsertVertex 请选择待操作的基准钢板:","");
		}	
		pCmdLine->FinishCmdLine(CXhChar16("0x%X",pDatumPlate->handle));
		g_pSolidDraw->SetEntSnapStatus(pDatumPlate->handle);
	}
	//选择待删除的轮廓点、螺栓
	verifier.ClearSnapFlag();
	verifier.SetVerifyFlag(OBJPROVIDER::DRAWINGSPACE,GetSingleWord(IDbEntity::DbPoint));
	verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_BOLT);
	pCmdLine->FillCmdLine("请选择待删除的顶点或螺栓:","");
	CLDSBolt* pPickBolt=NULL;
	PROFILE_VERTEX* pPickVertex=NULL;
	while(true)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return 0;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		if(obj.provider==OBJPROVIDER::SOLIDSPACE)
			pPickBolt=(CLDSBolt*)Ta.FromPartHandle(obj.hObj,CLS_BOLT);
		else if(obj.provider==OBJPROVIDER::DRAWINGSPACE)
		{
			IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
			IDbEntity* pEnt=pBuddyDrawing->GetDbEntity(obj.idEnt);
			if(pEnt==NULL || pEnt->GetDbEntType()!=IDbEntity::DbPoint)
				continue;	//非轮廓点
			if(pEnt->GetHiberId().masterId!=pDatumPlate->handle)
				continue;	//非当前钢板上的轮廓点
			DWORD dwObjectId=pEnt->GetHiberId().HiberDownId(1);
			for(PROFILE_VERTEX* pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate->vertex_list.GetNext())
			{
				if(pVertex->vertex.ID==dwObjectId)
				{
					pPickVertex=pVertex;
					break;
				}
			}
		}
		else
			continue;
		if(pPickBolt!=NULL||pPickVertex!=NULL)
			break;
	}
	//
	if(pPickVertex)
	{
		if(pDatumPlate->GetClassTypeId()==CLS_PLATE)
		{
			CLDSPlate* pPlank=(CLDSPlate*)pDatumPlate;
			int i=0;
			for(i=0;i<pPlank->designInfo.additional_vertex.GetNodeNum();i++)
			{	//删除自定义轮廓点时，从自定义轮廓点列表中删除相应顶点 否则重新设计时自定义点又会出现
				if(pPlank->designInfo.additional_vertex[i].IsEqual(pPickVertex->vertex))
					pPlank->designInfo.additional_vertex.DeleteAt(i);
			}
			for(i=0;i<pPlank->vertex_list.GetNodeNum();i++)
			{
				if(pPlank->vertex_list[i].vertex.IsEqual(pPickVertex->vertex))
				{
					if(pPlank->vertex_list[i].vertex.feature>10)
					{
						AfxMessageBox("火曲点不能删除!");
						return FALSE;
					}
					pPlank->vertex_list.DeleteAt(i);
				}
			}
			if(i==pPlank->vertex_list.GetNodeNum())
			{	//删除因卷边生成的轮廓点时相当于删除卷边 wht 16-06-06
				if(pPickVertex->m_bRollEdge)
					pPickVertex->m_bRollEdge=FALSE;
			}
		}
		else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
		{	//参数化板轮廓点不允许删除
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*)console.m_pWorkPart;
			if(!pParamPlate->IsCanManualModify())
			{
				AfxMessageBox("此参数化板不支持删除特征点！");
				return FALSE;
			}
			for(int i=0;i<pParamPlate->vertex_list.GetNodeNum();i++)
			{
				if(pParamPlate->vertex_list[i].vertex.IsEqual(pPickVertex->vertex))
				{
					if(pParamPlate->vertex_list[i].vertex.feature>10)
					{
						AfxMessageBox("火曲点不能删除!");
						return FALSE;
					}
					pParamPlate->vertex_list.DeleteAt(i);
				}
			}
		}
	}
	else if(pPickBolt)
	{
		pDatumPlate->DeleteLsRef(pPickBolt->handle);
		if(pPickBolt&&pPickBolt->m_bVirtualPart)
			Ta.DelBoltAndRelativeBoltRef(pPickBolt->handle);
	}
	//
	pDatumPlate->SetModified();
	pDatumPlate->Create3dSolidModel();
	CLDSView::RedrawPlateProfileDrawing(pDatumPlate);
	g_pSolidDraw->NewSolidPart(pDatumPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return 1;
}
void CLDSView::OnUpdateNewAttachVerticalPlate(CCmdUI* pCmdUI) 
{
	if(console.m_pWorkPart&&console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

//from lslist.cpp
BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
					  CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,
				   double epsilon,char *error_info,long *pH);
void CLDSView::OnPrevPart() 
{
	if(m_curAppType==TA_ASSEMBLE_ENV)
		return;	//非构件编辑器环境
	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();
	if(pFrame->prev_part.GetNodeCount()>0)
	{
		long h;
		if(console.m_pWorkPart)
			pFrame->next_part.push(console.m_pWorkPart->handle);
		pFrame->prev_part.pop(h);
		CLDSDbObject *pObj = (CLDSDbObject*)console.FromHandle(h,TRUE);
		if(pObj==NULL || pObj->GetClassTypeId()==CLS_NODE)
			return;
		CLDSPart *pNextPart=console.m_pWorkPart;	//记录前一个构件
		console.m_pWorkPart=(CLDSPart*)pObj;
		UpdatePropertyPage(console.m_pWorkPart);	//更新属性栏
		CalDefaultAssembleCS(console.m_pWorkPart);
		LoadDefaultUCS(&console.m_curWCS);
		console.m_curWCS.origin.Set();
		g_pSolidDraw->BuildDisplayList();
		g_pSolidOper->ReSetView();
		g_pSolidOper->ZoomAll(0.95);
		if(pNextPart&&pNextPart->GetClassTypeId()==console.m_pWorkPart->GetClassTypeId())
		{
			if(pNextPart->GetClassTypeId()==CLS_LINEANGLE)
			{	//比较角钢螺栓选中位置不同的螺栓 wht 11-01-15
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.m_pWorkPart;
				CLDSLineAngle *pViceLineAngle=(CLDSLineAngle*)pNextPart;
				UCS_STRU ucs,vice_ucs;
				pLineAngle->getUCS(ucs,2);
				pViceLineAngle->getUCS(vice_ucs,2);
				Sub_Pnt(ucs.origin,ucs.origin,ucs.axis_z*pLineAngle->startOdd());
				Sub_Pnt(vice_ucs.origin,vice_ucs.origin,vice_ucs.axis_z*pViceLineAngle->startOdd());
				long hBolt=0;
				if(!IsLsRefListEqual(pLineAngle->GetLsRefList(),ucs,
									 pViceLineAngle->GetLsRefList(),vice_ucs,0.5,NULL,&hBolt))
					g_pSolidDraw->SetEntSnapStatus(hBolt);
			}
		}
		Invalidate(FALSE);
	}
}
void CLDSView::OnNextPart() 
{
	if(m_curAppType==TA_ASSEMBLE_ENV)
		return;	//非构件编辑器环境
	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();
	if(pFrame->next_part.GetNodeCount()>0)
	{
		long h;
		if(console.m_pWorkPart)
			pFrame->prev_part.push(console.m_pWorkPart->handle);
		pFrame->next_part.pop(h);
		CLDSDbObject *pObj = (CLDSDbObject*)console.FromHandle(h,TRUE);
		if(pObj==NULL || pObj->GetClassTypeId()==CLS_NODE)
			return;
		CLDSPart *pPrePart=console.m_pWorkPart;	//记录前一个构件
		console.m_pWorkPart=(CLDSPart*)pObj;
		UpdatePropertyPage(console.m_pWorkPart);	//更新属性栏
		CalDefaultAssembleCS(console.m_pWorkPart);
		LoadDefaultUCS(&console.m_curWCS);
		console.m_curWCS.origin.Set();
		g_pSolidDraw->BuildDisplayList();
		g_pSolidOper->ReSetView();
		g_pSolidOper->ZoomAll(0.95);
		if(pPrePart&&pPrePart->GetClassTypeId()==console.m_pWorkPart->GetClassTypeId())
		{	
			if(pPrePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//比较角钢螺栓选中位置不同的螺栓 wht 11-01-15
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.m_pWorkPart;
				CLDSLineAngle *pViceLineAngle=(CLDSLineAngle*)pPrePart;
				UCS_STRU ucs,vice_ucs;
				pLineAngle->getUCS(ucs,2);
				pViceLineAngle->getUCS(vice_ucs,2);
				Sub_Pnt(ucs.origin,ucs.origin,ucs.axis_z*pLineAngle->startOdd());
				Sub_Pnt(vice_ucs.origin,vice_ucs.origin,vice_ucs.axis_z*pViceLineAngle->startOdd());
				long hBolt=0;
				if(!IsLsRefListEqual(pLineAngle->GetLsRefList(),ucs,
									 pViceLineAngle->GetLsRefList(),vice_ucs,0.5,NULL,&hBolt))
					g_pSolidDraw->SetEntSnapStatus(hBolt);
			}
		}
		Invalidate(FALSE);
	}
}

void CLDSView::OnUpdatePrevPart(CCmdUI* pCmdUI) 
{
	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();
	if(pFrame->prev_part.GetNodeCount()==0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CLDSView::OnUpdateNextPart(CCmdUI* pCmdUI) 
{
	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();
	if(pFrame->next_part.GetNodeCount()==0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CLDSView::OnImportExistLs() 
{
	CLDSBolt *pBolt;
	CLsRef *pLsRef;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("specify bolt's handle:","");
#else
	pCmdLine->FillCmdLine("指定螺栓句柄:","");
#endif
	while(1)
	{
		CString cmdStr;
		if(!pCmdLine->GetStrFromCmdLine(cmdStr))
			return ;
		long h=0;
		sscanf(cmdStr.GetBuffer(10),"%X",&h);
		pBolt = (CLDSBolt*)console.FromPartHandle(h,CLS_BOLT);
		if(pBolt==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("bolts you speicified handle not found","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("specify bolt's handle:","");
#else
			pCmdLine->FillCmdLine("未发现你所指定句柄的螺栓","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("指定螺栓句柄:","");
#endif
			continue;
		}
		break;
	}

	if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		CLDSLineAngle *pJg=(CLDSLineAngle*)console.m_pWorkPart;
		if(!pJg->FindLsByHandle(pBolt->handle))
		{
			if(theApp.m_bCooperativeWork)
			{
				if(pJg->dwPermission&theApp.GetPermDword())
					pLsRef = pJg->AppendMidLsRef(pBolt->GetLsRef());
				else if(pJg->dwStartPermission&theApp.GetPermDword())
					pLsRef = pJg->AppendStartLsRef(pBolt->GetLsRef());
				else if(pJg->dwEndPermission&theApp.GetPermDword())
					pLsRef = pJg->AppendEndLsRef(pBolt->GetLsRef());
				else
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("no corresponding authority！");
#else
					AfxMessageBox("没有相应的权限！");
#endif
			}
			else
				pLsRef=pJg->AppendMidLsRef(pBolt->GetLsRef());
		}
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)console.m_pWorkPart;
		if(!pPlate->FindLsByHandle(pBolt->handle))
			pLsRef = pPlate->AppendLsRef(pBolt->GetLsRef());
	}
	else if(console.m_pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)console.m_pWorkPart;
		if(!pParamPlate->FindLsByHandle(pBolt->handle))
			pLsRef = pParamPlate->AppendLsRef(pBolt->GetLsRef());
	}
	else //if(console.m_pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		if(!console.m_pWorkPart->FindLsByHandle(pBolt->handle))
			console.m_pWorkPart->AppendLsRef(pBolt->GetLsRef());
	}
	g_pSolidDraw->BuildDisplayList();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
}
#include "AssistLineDlg.h"
void CLDSView::OnAddAssistLine()
{
	static CAssistLineDlg assistline;
	if(assistline.DoModal()!=IDOK)
		return;
	GEPOINT lineStart(assistline.m_fLineStartX,assistline.m_fLineStartY);
	GEPOINT lineEnd(assistline.m_fLineEndX,assistline.m_fLineEndY);
	if(lineStart.IsEqual(lineEnd.x,lineEnd.y,lineEnd.z))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The end point is the same as the starting");
#else
		AfxMessageBox("始末端点重合");
#endif
		return;
	}
	coord_trans(lineStart,console.m_curWCS,TRUE);
	coord_trans(lineEnd,console.m_curWCS,TRUE);
	f3dLine line(lineStart,lineEnd);
	m_xAssistLineList.append(GELINE(lineStart,lineEnd));
	g_pSolidDraw->NewLine(line);
}
#else
void CLDSView::OnFeatureProp(){;}
void CLDSView::OnNewLs(){;}
void CLDSView::OnAssembleCs(){;}
void CLDSView::OnWorkCs(){;}
void CLDSView::OnSpecWcsOrginByGlobalCs(){;}
void CLDSView::OnSpecWcsOrgin(){;}
void CLDSView::OnSpecAxisXVertex(){;}
void CLDSView::OnDelPartFeature(){;}
void CLDSView::OnMeasureVertexDist(){;}
void CLDSView::OnInVertex(){;}
void CLDSView::OnImportPlankProfile(){;}
void CLDSView::OnAutoCalPlankProfile(){;}
void CLDSView::OnBendPlank(){;}
void CLDSView::OnAddFillPlank(){;} 
void CLDSView::OnDesignGuaXianHole(){;}
void CLDSView::OnDesignBushTube(){;}
void CLDSView::OnSpecLsRayNo(){;}
void CLDSView::OnRollPlankEdge(){;}
void CLDSView::OnUpdateNewAttachVerticalPlate(CCmdUI* pCmdUI){;}
void CLDSView::OnPrevPart(){;}
void CLDSView::OnNextPart(){;}
void CLDSView::OnUpdatePrevPart(CCmdUI* pCmdUI){;}
void CLDSView::OnUpdateNextPart(CCmdUI* pCmdUI){;}
void CLDSView::OnImportExistLs(){;}
void CLDSView::OnAddAssistLine(){;}
#endif

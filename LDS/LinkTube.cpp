#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "dlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "MirTaAtom.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "lds_part.h"
#include "LinkTubeDlg.h"
#include "database.h"
#include "LinkSlotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////
extern char CalPartPos(CLDSLinePart* pPart);
/////////////////////////////////////////////////////////
BOOL CLDSView::FinishLinkTube(CLDSNode *pStartNode, CLDSNode *pEndNode)
{
	static CLinkTubeDlg link_tube_dlg;
	CLDSLineTube* pNowTube=NULL, *pSideNowTube=NULL;
	f3dLine line;
	f3dPoint view_norm,side_view_norm;
	link_tube_dlg.m_pStartNode=pStartNode;
	link_tube_dlg.m_pEndNode=pEndNode;
	//设置默认段号
	if(pStartNode->iSeg.iSeg>0)
		link_tube_dlg.m_sSegNo = pStartNode->iSeg.ToString();
	else if(pEndNode->iSeg.iSeg>0)
		link_tube_dlg.m_sSegNo = pEndNode->iSeg.ToString();
	else 
		link_tube_dlg.m_sSegNo.Empty();
	if(link_tube_dlg.DoModal()!=IDOK)
		return FALSE;
	Ta.BeginUndoListen();
	try
	{
		pNowTube=(CLDSLineTube*)console.AppendPart(CLS_LINETUBE);
		if(pStartNode->hFatherPart<0x20)
		{
			pStartNode->hFatherPart = pNowTube->handle;
			pStartNode->SetModified();
		}
		if(pEndNode->hFatherPart<0x20)
		{
			pEndNode->hFatherPart = pNowTube->handle;
			pEndNode->SetModified();
		}
		pNowTube->cMaterial = CSteelMatLibrary::RecordAt(link_tube_dlg.m_iTubeMat).cBriefMark;
		pNowTube->SetPartNo(link_tube_dlg.m_sOriginPartNo.GetBuffer());
		pNowTube->iSeg=SEGI(link_tube_dlg.m_sSegNo.GetBuffer());
		pNowTube->pStart = pStartNode;
		pNowTube->pEnd   = pEndNode;
		
		//-------Wjh 未完成 --------------
		switch(link_tube_dlg.m_iPart1stChar)
		{
		case 0:
			pNowTube->layer(0) = 'T';
			break;
		case 1:
			pNowTube->layer(0) = 'S';
			break;
		case 2:
			pNowTube->layer(0) = 'L';
		default:
			break;
		}
		switch(link_tube_dlg.m_iPart2ndChar)
		{
		case 0:
			pNowTube->layer(1) = 'Z';
			break;
		case 1:
			pNowTube->layer(1) = 'X';
			break;
		case 2:
			pNowTube->layer(1) = 'H';
			break;
		case 3:
			pNowTube->layer(1) = 'F';
			break;
		default:
			break;
		}
		pNowTube->layer(2) = theApp.env.layer[2];
		//设置塔腿杆件配材号 wht 13-04-01
		if(pNowTube->Layer(0)=='L'&&pStartNode->Layer(0)=='L')
			pNowTube->cfgword&=pStartNode->UnifiedCfgword();
		if(pNowTube->Layer(0)=='L'&&pEndNode->Layer(0)=='L')
			pNowTube->cfgword&=pEndNode->UnifiedCfgword();
		double wing_wide,wing_thick;
		restore_JG_guige(link_tube_dlg.m_sTubeGuige.GetBuffer(),wing_wide,wing_thick);
		pNowTube->size_wide=wing_wide;
		pNowTube->size_thick=wing_thick;
		if(pNowTube->GetDiameter()>g_sysPara.fTubeWeldRoadMinD)
			pNowTube->m_bHasWeldRoad=TRUE;
		else
			pNowTube->m_bHasWeldRoad=FALSE;
		pNowTube->SetStart(pNowTube->pStart->ActivePosition());
		pNowTube->SetEnd(pNowTube->pEnd->ActivePosition());
		if(pNowTube->pStart&&pNowTube->pStart->hFatherPart!=pNowTube->handle)
			pNowTube->hWorkNormRefPart=pNowTube->pStart->hFatherPart;
		else if(pNowTube->pEnd&&pNowTube->pEnd->hFatherPart!=pNowTube->handle)
			pNowTube->hWorkNormRefPart=pNowTube->pEnd->hFatherPart;
		if(pNowTube->pStart&&pNowTube->pStart->hFatherPart!=pNowTube->handle)
		{
			pNowTube->desStartPos.endPosType=1;
			pNowTube->desStartPos.hDatumPart=pNowTube->pStart->hFatherPart;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)console.FromPartHandle(pNowTube->desStartPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pNowTube->pEnd->ActivePosition()-pNowTube->pStart->ActivePosition())^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
					pNowTube->desStartPos.datum_pos_style=1;
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
					pNowTube->desStartPos.datum_pos_style=2;
				else
					pNowTube->desStartPos.datum_pos_style=0;
			}
		}
		if(pNowTube->pEnd&&pNowTube->pEnd->hFatherPart!=pNowTube->handle)
		{
			pNowTube->desEndPos.endPosType=1;
			pNowTube->desEndPos.hDatumPart=pNowTube->pEnd->hFatherPart;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)console.FromPartHandle(pNowTube->desEndPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pNowTube->pEnd->ActivePosition()-pNowTube->pStart->ActivePosition())^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
					pNowTube->desEndPos.datum_pos_style=1;
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
					pNowTube->desEndPos.datum_pos_style=2;
				else
					pNowTube->desEndPos.datum_pos_style=0;
			}
		}
		pNowTube->nearWorkPlaneNorm=console.GetActiveView()->ucs.axis_z;
		pNowTube->CalWorkPlaneNorm();
		pNowTube->CalPosition();
		NewLinePart(pNowTube);
		g_pSolidDraw->SetEntSnapStatus(pNowTube->handle);
		if(!MirTaAtom(pNowTube, link_tube_dlg.mirmsg))
		{
			Ta.EndUndoListen();
			AfxMessageBox("此工况不能这样的对称，对称失败");
			return FALSE;
		}
		CSuperSmartPtr<CLDSPart>pMirPart;
		MIRMSG rotmirmsg = link_tube_dlg.mirmsg;
		rotmirmsg.axis_flag=8;
		if(link_tube_dlg.mirmsg.axis_flag&8)	//旋转对称
		{
			pMirPart=pNowTube->GetMirPart(rotmirmsg);
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_tube_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_tube_dlg.m_sOriginPartNo.GetBuffer(0));
			}
		}
		if(link_tube_dlg.mirmsg.axis_flag&1)	//X轴对称
		{
			pMirPart=pNowTube->GetMirPart(MIRMSG(1));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_tube_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_tube_dlg.m_sMirXPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&link_tube_dlg.mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_tube_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_tube_dlg.m_sMirXPartNo.GetBuffer(0));			
				}
			}
		}
		if(link_tube_dlg.mirmsg.axis_flag&2)	//Y轴对称
		{
			pMirPart=pNowTube->GetMirPart(MIRMSG(2));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_tube_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_tube_dlg.m_sMirYPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&link_tube_dlg.mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_tube_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_tube_dlg.m_sMirYPartNo.GetBuffer(0));
				}
			}
		}
		if(link_tube_dlg.mirmsg.axis_flag&4)	//Z轴对称
		{
			pMirPart=pNowTube->GetMirPart(MIRMSG(3));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_tube_dlg.m_sSegNo.GetBuffer());	
				pMirPart->SetPartNo(link_tube_dlg.m_sMirZPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&link_tube_dlg.mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_tube_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_tube_dlg.m_sMirZPartNo.GetBuffer(0));
				}
			}
		}
	}
	catch(char* sErrorMsg)
	{
		Ta.EndUndoListen();
		AfxMessageBox(sErrorMsg);
		RedrawAll();
		MessageBox("操作失败,程序回滚!");
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	Invalidate(FALSE);
	return TRUE;
}

BOOL CLDSView::FinishLinkSlot(CLDSNode *pStartNode, CLDSNode *pEndNode)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	static CLinkSlotDlg link_slot_dlg;
	CLDSLineSlot* pNowSlot=NULL, *pSideNowSlot=NULL;
	f3dLine line;
	link_slot_dlg.m_pStartNode = pStartNode;
	link_slot_dlg.m_pEndNode = pEndNode;
	//设置默认段号
	if(pStartNode->iSeg.iSeg>0)
		link_slot_dlg.m_sSegNo = pStartNode->iSeg.ToString();
	else if(pEndNode->iSeg.iSeg>0)
		link_slot_dlg.m_sSegNo = pEndNode->iSeg.ToString();
	else 
		link_slot_dlg.m_sSegNo.Empty();
	if(link_slot_dlg.DoModal()!=IDOK)
		return FALSE;
	Ta.BeginUndoListen();
	try
	{
		pNowSlot=(CLDSLineSlot*)console.AppendPart(CLS_LINESLOT);
		pNowSlot->SetPartNo(link_slot_dlg.m_sOriginPartNo.GetBuffer(0));
		pNowSlot->cMaterial = CSteelMatLibrary::RecordAt(link_slot_dlg.m_iSlotMat).cBriefMark;
		pNowSlot->iSeg=SEGI(link_slot_dlg.m_sSegNo.GetBuffer());
		if(pStartNode->hFatherPart<0x20)
		{
			pStartNode->hFatherPart = pNowSlot->handle;
			pStartNode->SetModified();
		}
		if(pEndNode->hFatherPart<0x20)
		{
			pEndNode->hFatherPart = pNowSlot->handle;
			pEndNode->SetModified();
		}
		pNowSlot->pStart = pStartNode;
		pNowSlot->pEnd   = pEndNode;
		//根据始终端节点预拱项属性设置槽钢预拱项		wxc-17.1.6 
		if(pStartNode->ArcLiftHandle()>0X20 && pStartNode->ArcLiftHandle()==pEndNode->ArcLiftHandle())
			pNowSlot->SetArcLift(pStartNode->ArcLiftHandle());
		//-------Wjh 未完成 --------------
		switch(link_slot_dlg.m_iPart1stChar)
		{
		case 0:
			pNowSlot->layer(0) = 'T';
			break;
		case 1:
			pNowSlot->layer(0) = 'S';
			break;
		case 2:
			pNowSlot->layer(0) = 'L';
		default:
			break;
		}
		switch(link_slot_dlg.m_iPart2ndChar)
		{
		case 0:
			pNowSlot->layer(1) = 'Z';
			break;
		case 1:
			pNowSlot->layer(1) = 'X';
			break;
		case 2:
			pNowSlot->layer(1) = 'F';
			break;
		case 3:
			pNowSlot->layer(1) = 'H';
			break;
		default:
			break;
		}
		pNowSlot->layer(2) = theApp.env.layer[2];
		//设置塔腿杆件配材号 wht 13-04-01
		if(pNowSlot->Layer(0)=='L'&&pStartNode->Layer(0)=='L')
			pNowSlot->cfgword&=pStartNode->UnifiedCfgword();
		if(pNowSlot->Layer(0)=='L'&&pEndNode->Layer(0)=='L')
			pNowSlot->cfgword&=pEndNode->UnifiedCfgword();
		//-------空间布置状态 Wjh 未完成 --------------
		SLOT_PARA_TYPE *pSlotPara=FindSlotType(link_slot_dlg.m_sSlotGuige.GetBuffer(16));
		if(pSlotPara==NULL)
			throw "无此规格槽钢!";
		_snprintf(pNowSlot->spec,MAX_CHAR_GUIGE_16,"%s",link_slot_dlg.m_sSlotGuige);
		pNowSlot->size_wide=pSlotPara->b;	//宽
		pNowSlot->size_height=pSlotPara->h;	//截面高
		pNowSlot->size_thick=pSlotPara->t;	//厚
		pNowSlot->SetStart(pNowSlot->pStart->ActivePosition());
		pNowSlot->SetEnd(pNowSlot->pEnd->ActivePosition());
		pNowSlot->CalPosition();
		int start0_end1=-1;
		if(pNowSlot->pStart&&pNowSlot->pStart->hFatherPart!=pNowSlot->handle)
		{
			pNowSlot->hWorkNormRefPart=pNowSlot->pStart->hFatherPart;
			start0_end1=0;
		}
		else if(pNowSlot->pEnd&&pNowSlot->pEnd->hFatherPart!=pNowSlot->handle)
		{
			pNowSlot->hWorkNormRefPart=pNowSlot->pEnd->hFatherPart;
			start0_end1=1;
		}
		if(pNowSlot->pStart&&pNowSlot->pStart->hFatherPart!=pNowSlot->handle)
		{
			pNowSlot->desStartPos.endPosType=1;
			pNowSlot->desStartPos.hDatumPart=pNowSlot->pStart->hFatherPart;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)console.FromPartHandle(pNowSlot->desStartPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pNowSlot->pEnd->ActivePosition()-pNowSlot->pStart->ActivePosition())^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
					pNowSlot->desStartPos.datum_pos_style=1;
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
					pNowSlot->desStartPos.datum_pos_style=2;
				else
					pNowSlot->desStartPos.datum_pos_style=0;
			}
		}
		if(pNowSlot->pEnd&&pNowSlot->pEnd->hFatherPart!=pNowSlot->handle)
		{
			pNowSlot->desEndPos.endPosType=1;
			pNowSlot->desEndPos.hDatumPart=pNowSlot->pEnd->hFatherPart;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)console.FromPartHandle(pNowSlot->desEndPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pNowSlot->pEnd->ActivePosition()-pNowSlot->pStart->ActivePosition())^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
					pNowSlot->desEndPos.datum_pos_style=1;
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
					pNowSlot->desEndPos.datum_pos_style=2;
				else
					pNowSlot->desEndPos.datum_pos_style=0;
			}
		}
		CSuperSmartPtr<CLDSPart>pWorkNormRefPart=console.FromPartHandle(pNowSlot->hWorkNormRefPart);
		if(pWorkNormRefPart.IsHasPtr()&&pWorkNormRefPart->IsArcPart()&&start0_end1!=-1)
		{
			CLDSNode *pDatumNode;
			if(start0_end1==0)
				pDatumNode=pNowSlot->pStart;
			else
				pDatumNode=pNowSlot->pEnd;
			if(link_slot_dlg.m_iSlotLayStyle==0)
				pNowSlot->nearWorkPlaneNorm=f3dPoint(pWorkNormRefPart.ArcPartPointer()->baseline.Center())
					-pWorkNormRefPart.ArcPartPointer()->GetDatumPos(pDatumNode->ActivePosition());
			else //if(link_slot_dlg.m_iSlotLayStyle==1)
				pNowSlot->nearWorkPlaneNorm=pWorkNormRefPart.ArcPartPointer()->GetDatumPos(pDatumNode->ActivePosition())
					-f3dPoint(pWorkNormRefPart.ArcPartPointer()->baseline.Center());
		}
		else
		{
			if(link_slot_dlg.m_iSlotLayStyle==0)
				pNowSlot->nearWorkPlaneNorm=-console.GetActiveView()->ucs.axis_z;
			else //if(link_slot_dlg.m_iSlotLayStyle==1)
				pNowSlot->nearWorkPlaneNorm=console.GetActiveView()->ucs.axis_z;
		}
		pNowSlot->CalWorkPlaneNorm();
		NewLinePart(pNowSlot);
		g_pSolidDraw->SetEntSnapStatus(pNowSlot->handle);
		MIRMSG mirmsg = link_slot_dlg.mirmsg;
		if(!MirTaAtom(pNowSlot, mirmsg))
		{
			Ta.EndUndoListen();
			AfxMessageBox("此工况不能这样的对称，对称失败");
			return FALSE;
		}
		CSuperSmartPtr<CLDSPart>pMirPart;
		MIRMSG rotmirmsg = mirmsg;
		rotmirmsg.axis_flag=8;
		if(mirmsg.axis_flag&8)	//旋转对称
		{
			pMirPart=pNowSlot->GetMirPart(rotmirmsg);
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_slot_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_slot_dlg.m_sOriginPartNo.GetBuffer(0));
			}
		}
		if(mirmsg.axis_flag&1)	//X轴对称
		{
			pMirPart=pNowSlot->GetMirPart(MIRMSG(1));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_slot_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_slot_dlg.m_sMirXPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_slot_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_slot_dlg.m_sMirXPartNo.GetBuffer(0));
				}
			}
		}
		if(mirmsg.axis_flag&2)	//Y轴对称
		{
			pMirPart=pNowSlot->GetMirPart(MIRMSG(2));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_slot_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_slot_dlg.m_sMirYPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_slot_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_slot_dlg.m_sMirYPartNo.GetBuffer(0));
				}
			}
		}
		if(mirmsg.axis_flag&4)	//Z轴对称
		{
			pMirPart=pNowSlot->GetMirPart(MIRMSG(3));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_slot_dlg.m_sSegNo.GetBuffer());	
				pMirPart->SetPartNo(link_slot_dlg.m_sMirZPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_slot_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_slot_dlg.m_sMirZPartNo.GetBuffer(0));
				}
			}
		}
	}
	catch(char* sErrorMsg)
	{
		Ta.EndUndoListen();
		AfxMessageBox(sErrorMsg);
		RedrawAll();
		MessageBox("操作失败,程序回滚!");
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	Invalidate(FALSE);
#endif
	return TRUE;
}

BOOL CLDSView::FinishLinkFlat(CLDSNode *pStartNode, CLDSNode *pEndNode)
{
	static CLinkTubeDlg link_flat_dlg;
	CLDSLineFlat* pNowFlat=NULL, *pSideNowFlat=NULL;
	f3dLine line;
	f3dPoint view_norm,side_view_norm;
	double wing_wide,wing_thick;
	link_flat_dlg.m_iPoleType=1; //扁钢
	link_flat_dlg.m_pStartNode=pStartNode;
	link_flat_dlg.m_pEndNode=pEndNode;
	//设置默认段号
	if(pStartNode->iSeg.iSeg>0)
		link_flat_dlg.m_sSegNo = pStartNode->iSeg.ToString();
	else if(pEndNode->iSeg.iSeg>0)
		link_flat_dlg.m_sSegNo = pEndNode->iSeg.ToString();
	else 
		link_flat_dlg.m_sSegNo.Empty();
	if(link_flat_dlg.DoModal()!=IDOK)
		return FALSE;
	Ta.BeginUndoListen();
	try
	{
		pNowFlat=(CLDSLineFlat*)console.AppendPart(CLS_LINEFLAT);
		pNowFlat->SetPartNo(link_flat_dlg.m_sOriginPartNo.GetBuffer(0));
		pNowFlat->cMaterial = CSteelMatLibrary::RecordAt(link_flat_dlg.m_iTubeMat).cBriefMark;
		pNowFlat->iSeg=SEGI(link_flat_dlg.m_sSegNo.GetBuffer());
		if(pStartNode->hFatherPart<0x20)
		{
			pStartNode->hFatherPart = pNowFlat->handle;
			pStartNode->SetModified();
		}
		if(pEndNode->hFatherPart<0x20)
		{
			pEndNode->hFatherPart = pNowFlat->handle;
			pEndNode->SetModified();
		}
		pNowFlat->pStart = pStartNode;
		pNowFlat->pEnd   = pEndNode;
		//根据始终端节点预拱项属性设置扁铁预拱项		wxc-17.1.6 
		if(pStartNode->ArcLiftHandle()>0X20 && pStartNode->ArcLiftHandle()==pEndNode->ArcLiftHandle())
			pNowFlat->SetArcLift(pStartNode->ArcLiftHandle());
		//-------Wjh 未完成 --------------
		switch(link_flat_dlg.m_iPart1stChar)
		{
		case 0:
			pNowFlat->layer(0) = 'T';
			break;
		case 1:
			pNowFlat->layer(0) = 'S';
			break;
		case 2:
			pNowFlat->layer(0) = 'L';
		default:
			break;
		}
		switch(link_flat_dlg.m_iPart2ndChar)
		{
		case 0:
			pNowFlat->layer(1) = 'Z';
			break;
		case 1:
			pNowFlat->layer(1) = 'X';
			break;
		case 2:
			pNowFlat->layer(1) = 'H';
			break;
		case 3:
			pNowFlat->layer(1) = 'F';
			break;
		default:
			break;
		}
		pNowFlat->layer(2) = theApp.env.layer[2];
		//设置塔腿杆件配材号 wht 13-04-01
		if(pNowFlat->Layer(0)=='L'&&pStartNode->Layer(0)=='L')
			pNowFlat->cfgword&=pStartNode->UnifiedCfgword();
		if(pNowFlat->Layer(0)=='L'&&pEndNode->Layer(0)=='L')
			pNowFlat->cfgword&=pEndNode->UnifiedCfgword();
		//-------空间布置状态 Wjh 未完成 --------------
		restore_JG_guige(link_flat_dlg.m_sTubeGuige.GetBuffer(),wing_wide,wing_thick);
		pNowFlat->size_wide=wing_wide;
		pNowFlat->size_thick=wing_thick;
		pNowFlat->SetStart(pNowFlat->pStart->ActivePosition());
		pNowFlat->SetEnd(pNowFlat->pEnd->ActivePosition());
		pNowFlat->CalPosition();
		if(pNowFlat->pStart&&pNowFlat->pStart->hFatherPart!=pNowFlat->handle)
			pNowFlat->hWorkNormRefPart=pNowFlat->pStart->hFatherPart;
		else if(pNowFlat->pEnd&&pNowFlat->pEnd->hFatherPart!=pNowFlat->handle)
			pNowFlat->hWorkNormRefPart=pNowFlat->pEnd->hFatherPart;
		if(pNowFlat->pStart&&pNowFlat->pStart->hFatherPart!=pNowFlat->handle)
		{
			pNowFlat->desStartPos.endPosType=1;
			pNowFlat->desStartPos.hDatumPart=pNowFlat->pStart->hFatherPart;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)console.FromPartHandle(pNowFlat->desStartPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pNowFlat->pEnd->ActivePosition()-pNowFlat->pStart->ActivePosition())^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
					pNowFlat->desStartPos.datum_pos_style=1;
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
					pNowFlat->desStartPos.datum_pos_style=2;
				else
					pNowFlat->desStartPos.datum_pos_style=0;
			}
		}
		if(pNowFlat->pEnd&&pNowFlat->pEnd->hFatherPart!=pNowFlat->handle)
		{
			pNowFlat->desEndPos.endPosType=1;
			pNowFlat->desEndPos.hDatumPart=pNowFlat->pEnd->hFatherPart;
			CLDSLineAngle *pDatumLineAngle=(CLDSLineAngle*)console.FromPartHandle(pNowFlat->desEndPos.hDatumPart,CLS_LINEANGLE);
			if(pDatumLineAngle)
			{
				f3dPoint verify_norm=(pNowFlat->pEnd->ActivePosition()-pNowFlat->pStart->ActivePosition())^(pDatumLineAngle->Start()-pDatumLineAngle->End());
				normalize(verify_norm);
				if(fabs(pDatumLineAngle->get_norm_x_wing()*verify_norm)>0.707)
					pNowFlat->desEndPos.datum_pos_style=1;
				else if(fabs(pDatumLineAngle->get_norm_y_wing()*verify_norm)>0.707)
					pNowFlat->desEndPos.datum_pos_style=2;
				else
					pNowFlat->desEndPos.datum_pos_style=0;
			}
		}
		pNowFlat->nearWorkPlaneNorm=console.GetActiveView()->ucs.axis_z;
		pNowFlat->CalWorkPlaneNorm();
		NewLinePart(pNowFlat);
		g_pSolidDraw->SetEntSnapStatus(pNowFlat->handle);
		if(!MirTaAtom(pNowFlat, link_flat_dlg.mirmsg))
		{
			Ta.EndUndoListen();
			AfxMessageBox("此工况不能这样的对称，对称失败");
			return FALSE;
		}
		CSuperSmartPtr<CLDSPart>pMirPart;
		MIRMSG rotmirmsg = link_flat_dlg.mirmsg;
		rotmirmsg.axis_flag=8;
		if(link_flat_dlg.mirmsg.axis_flag&8)	//旋转对称
		{
			pMirPart=pNowFlat->GetMirPart(rotmirmsg);
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_flat_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_flat_dlg.m_sOriginPartNo.GetBuffer(0));
			}
		}
		if(link_flat_dlg.mirmsg.axis_flag&1)	//X轴对称
		{
			pMirPart=pNowFlat->GetMirPart(MIRMSG(1));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_flat_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_flat_dlg.m_sMirXPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&link_flat_dlg.mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_flat_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_flat_dlg.m_sMirXPartNo.GetBuffer(0));
				}
			}
		}
		if(link_flat_dlg.mirmsg.axis_flag&2)	//Y轴对称
		{
			pMirPart=pNowFlat->GetMirPart(MIRMSG(2));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_flat_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_flat_dlg.m_sMirYPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&link_flat_dlg.mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_flat_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_flat_dlg.m_sMirYPartNo.GetBuffer(0));
				}
			}
		}
		if(link_flat_dlg.mirmsg.axis_flag&4)	//Z轴对称
		{
			pMirPart=pNowFlat->GetMirPart(MIRMSG(3));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_flat_dlg.m_sSegNo.GetBuffer());	
				pMirPart->SetPartNo(link_flat_dlg.m_sMirZPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&link_flat_dlg.mirmsg.axis_flag&8)	//旋转对称
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_flat_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_flat_dlg.m_sMirZPartNo.GetBuffer(0));
				}
			}
		}
	}
	catch(char* sErrorMsg)
	{
		Ta.EndUndoListen();
		AfxMessageBox(sErrorMsg);
		RedrawAll();
		MessageBox("操作失败,程序回滚!");
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	Invalidate(FALSE);
	return TRUE;
}
#endif
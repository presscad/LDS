#include "stdafx.h"
#include "..\lds.h"
#include "..\ShankDefCls.h"
#include "..\InsertStdTemplateOperation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __ALFA_TEST_VERSION_

#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
void Test_AdjustLinePartPosPara(LINEPARTSET &linePartList,const SLOPE_POSINFO& slopePosInfo)
{
	CSuperSmartPtr<CLDSLinePart> pStartLinePart=linePartList.GetFirst(),pEndLinePart=linePartList.GetTail();
	if(pStartLinePart.IsNULL()||pEndLinePart.IsNULL())
		return;
	int i=0;
	f3dPoint pos;
	f3dLine ber_line(pStartLinePart->Start(),pEndLinePart->End());
	if(pStartLinePart->GetClassTypeId()==CLS_LINEANGLE)
	{
		CSuperSmartPtr<CLDSLinePart> pLinePart;
		for(pLinePart=linePartList.GetFirst();pLinePart.IsHasPtr();pLinePart=linePartList.GetNext())
		{	
			if(pLinePart.IsNULL())
				continue;
#ifdef __SMART_DRAW_INC_//__LDS_
			//添加"!slopePosInfo.bBottomSlope"主要是用于中间坡段末端应设为向下一坡段起始角钢楞线投影
			if(pLinePart!=pEndLinePart||!slopePosInfo.bBottomSlope)
			{	//不是终端分段杆件或当前坡度下面还接有其它坡度
				pLinePart.LineAnglePointer()->pOldDesEndPos->jgber_cal_style=0;	//点向楞线投影
				pLinePart.LineAnglePointer()->pOldDesEndPos->datum_pos_style=1;
				pLinePart.LineAnglePointer()->pOldDesEndPos->offset_X_dist=0;
				pLinePart.LineAnglePointer()->pOldDesEndPos->offset_Y_dist=0;
				pLinePart.LineAnglePointer()->pOldDesEndPos->offset_X_dist_style=4;
				pLinePart.LineAnglePointer()->pOldDesEndPos->offset_Y_dist_style=4;
				SnapPerp(&pos,ber_line,pLinePart->pEnd->Position(true));
				pLinePart->SetEnd(pos);
			}
			else //终端分段杆件
			{
				pLinePart.LineAnglePointer()->pOldDesEndPos->datum_to_ber_style=1;	//平推
				pLinePart.LineAnglePointer()->pOldDesEndPos->cFaceOffsetDatumLine='X';
				if((pLinePart->pEnd->Position(true).x<0&&pLinePart->pEnd->Position(true).y>0)||
				   (pLinePart->pEnd->Position(true).x>0&&pLinePart->pEnd->Position(true).y<0))
					pLinePart.LineAnglePointer()->pOldDesEndPos->cFaceOffsetDatumLine='Y';
			}

			if(pLinePart!=pStartLinePart)
			{	//不是始分段杆件
				pLinePart.LineAnglePointer()->pOldDesStartPos->jgber_cal_style=1;	//点线投影
				pLinePart.LineAnglePointer()->pOldDesStartPos->start_jg_h=pStartLinePart->handle;
				pLinePart.LineAnglePointer()->pOldDesStartPos->end_jg_h=pEndLinePart->handle;
				pLinePart.LineAnglePointer()->pOldDesStartPos->iStartJgBerStyle=0;
				pLinePart.LineAnglePointer()->pOldDesStartPos->iEndJgBerStyle=1;
				pLinePart.LineAnglePointer()->pOldDesStartPos->fStartJgLengOffsetX=
					pLinePart.LineAnglePointer()->pOldDesStartPos->fStartJgLengOffsetY=
					pLinePart.LineAnglePointer()->pOldDesStartPos->fEndJgLengOffsetX=
					pLinePart.LineAnglePointer()->pOldDesStartPos->fEndJgLengOffsetY=0;
				if((pStartLinePart.LineAnglePointer()->pOldDesStartPos->jgber_cal_style==0&&
					pStartLinePart.LineAnglePointer()->pOldDesStartPos->datum_pos_style==0&&
					pStartLinePart.LineAnglePointer()->pOldDesStartPos->datum_to_ber_style==1)||
					pStartLinePart.LineAnglePointer()->pOldDesStartPos->jgber_cal_style==2) //平推
				{
					pLinePart.LineAnglePointer()->pOldDesStartPos->jgber_cal_style=2;	//点线平推
					pLinePart.LineAnglePointer()->pOldDesStartPos->datum_to_ber_style=1;	//平推
					pLinePart.LineAnglePointer()->pOldDesStartPos->face_offset_norm=
						pStartLinePart.LineAnglePointer()->pOldDesStartPos->face_offset_norm;
					pLinePart.LineAnglePointer()->pOldDesStartPos->bByOtherEndOffset=
						pStartLinePart.LineAnglePointer()->pOldDesStartPos->bByOtherEndOffset;
					pLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine=
						pStartLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine;
					pLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine='X';
					if((pLinePart->pEnd->Position(false).x<0&&pLinePart->pEnd->Position(false).y>0)||
					   (pLinePart->pEnd->Position(false).x>0&&pLinePart->pEnd->Position(false).y<0))
						pLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine='Y';
				}
				SnapPerp(&pos,ber_line,pLinePart->pStart->Position(false));
				pLinePart->SetStart(pos);
			}
			else
			{
				pLinePart.LineAnglePointer()->pOldDesStartPos->datum_to_ber_style=1;	//平推
				if(!slopePosInfo.bTopSlope)	//当前最高分段上面还接有其它坡度段，此时为保证变坡面必须以楞线为平推基准
					pLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine=0;
				else
				{
					pLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine='X';
					if((pLinePart->pEnd->Position(false).x<0&&pLinePart->pEnd->Position(false).y>0)||
						(pLinePart->pEnd->Position(false).x>0&&pLinePart->pEnd->Position(false).y<0))
						pLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine='Y';
				}
			}
			//else //始端分段杆件
			if(pLinePart!=pStartLinePart)
			{
				pLinePart.LineAnglePointer()->des_norm_x=pStartLinePart.LineAnglePointer()->des_norm_x;
				pLinePart.LineAnglePointer()->des_norm_y=pStartLinePart.LineAnglePointer()->des_norm_y;
				pLinePart.LineAnglePointer()->set_norm_x_wing(pStartLinePart.LineAnglePointer()->get_norm_x_wing());
				pLinePart.LineAnglePointer()->set_norm_y_wing(pStartLinePart.LineAnglePointer()->get_norm_y_wing());
			}
			pLinePart.LineAnglePointer()->des_norm_x.bSpecific=FALSE;
			pLinePart.LineAnglePointer()->des_norm_y.bSpecific=FALSE;
			pLinePart.LineAnglePointer()->des_norm_x.bByOtherWing=FALSE;
			pLinePart.LineAnglePointer()->des_norm_y.bByOtherWing=FALSE;
			//对于非末了坡段的终端节点父杆件有时是在下一坡段设定的，这样由于父杆件不对可能会导致位置计算错误异常
			//故在此处不计算位置而是在整塔建完后统一计算
			//pLinePart.LinePartPointer()->CalPosition();
			pLinePart.LineAnglePointer()->cal_x_wing_norm();
			pLinePart.LineAnglePointer()->cal_y_wing_norm();
#else
			//添加"!slopePosInfo.bBottomSlope"主要是用于中间坡段末端应设为向下一坡段起始角钢楞线投影
			if(pLinePart!=pEndLinePart||!slopePosInfo.bBottomSlope)
			{	//不是终端分段杆件或当前坡度下面还接有其它坡度
				SnapPerp(&pos,ber_line,pLinePart->pEnd->pos);
				pLinePart->SetEnd(pos);
			}

			if(pLinePart!=pStartLinePart)
			{	//不是始分段杆件
				SnapPerp(&pos,ber_line,pLinePart->pStart->pos);
				pLinePart->SetStart(pos);
			}
#endif
		}
	}
	else if(pStartLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		CSuperSmartPtr<CLDSLinePart> pLinePart;
		for(pLinePart=linePartList.GetFirst();pLinePart.IsHasPtr();pLinePart=linePartList.GetNext())
		{	
			if(pLinePart.IsNULL())
				continue;
#ifdef __SMART_DRAW_INC_//__LDS_
			//被打断的杆件起始/终止点定位方式都不是点线投影
			if(pLinePart!=pEndLinePart)
			{	//不是终分段杆件
				pLinePart.LineTubePointer()->desEndPos.endPosType=2;	//点线投影
				pLinePart.LineTubePointer()->desEndPos.bDatumStartStartEnd=TRUE;
				pLinePart.LineTubePointer()->desEndPos.bDatumEndStartEnd=FALSE;
				pLinePart.LineTubePointer()->desEndPos.hDatumStartPart=pStartLinePart->handle;
				pLinePart.LineTubePointer()->desEndPos.hDatumEndPart=pEndLinePart->handle;
				SnapPerp(&pos,ber_line,pLinePart->pEnd->Position(false));
				pLinePart->SetEnd(pos);
			}
			//else //终端分段杆件
			if(pLinePart!=pStartLinePart)
			{	//不是始分段杆件
				pLinePart.LineTubePointer()->desStartPos.endPosType=2;	//点线投影
				pLinePart.LineTubePointer()->desStartPos.bDatumStartStartEnd=TRUE;
				pLinePart.LineTubePointer()->desStartPos.bDatumEndStartEnd=FALSE;
				pLinePart.LineTubePointer()->desStartPos.hDatumStartPart=pStartLinePart->handle;
				pLinePart.LineTubePointer()->desStartPos.hDatumEndPart=pEndLinePart->handle;
				SnapPerp(&pos,ber_line,pLinePart->pStart->Position(false));
				pLinePart->SetStart(pos);
			}
			//else //始端分段杆件
			if(pLinePart!=pStartLinePart)
			{
				pLinePart.LineTubePointer()->nearWorkPlaneNorm=pStartLinePart.LineTubePointer()->nearWorkPlaneNorm;
				pLinePart.LineTubePointer()->hWorkNormRefPart=pStartLinePart.LineTubePointer()->hWorkNormRefPart;
			}
			pLinePart.LineTubePointer()->CalPosition();
			pLinePart.LineTubePointer()->CalWorkPlaneNorm();
#else
			//被打断的杆件起始/终止点定位方式都不是点线投影
			if(pLinePart!=pEndLinePart)
			{	//不是终分段杆件
				SnapPerp(&pos,ber_line,pLinePart->pEnd->pos);
				pLinePart->SetEnd(pos);
			}
			//else //终端分段杆件
			if(pLinePart!=pStartLinePart)
			{	//不是始分段杆件
				SnapPerp(&pos,ber_line,pLinePart->pStart->pos);
				pLinePart->SetStart(pos);
			}
#endif
		}
	}
}
#endif
#endif
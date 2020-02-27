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
			//���"!slopePosInfo.bBottomSlope"��Ҫ�������м��¶�ĩ��Ӧ��Ϊ����һ�¶���ʼ�Ǹ�����ͶӰ
			if(pLinePart!=pEndLinePart||!slopePosInfo.bBottomSlope)
			{	//�����ն˷ֶθ˼���ǰ�¶����滹���������¶�
				pLinePart.LineAnglePointer()->pOldDesEndPos->jgber_cal_style=0;	//��������ͶӰ
				pLinePart.LineAnglePointer()->pOldDesEndPos->datum_pos_style=1;
				pLinePart.LineAnglePointer()->pOldDesEndPos->offset_X_dist=0;
				pLinePart.LineAnglePointer()->pOldDesEndPos->offset_Y_dist=0;
				pLinePart.LineAnglePointer()->pOldDesEndPos->offset_X_dist_style=4;
				pLinePart.LineAnglePointer()->pOldDesEndPos->offset_Y_dist_style=4;
				SnapPerp(&pos,ber_line,pLinePart->pEnd->Position(true));
				pLinePart->SetEnd(pos);
			}
			else //�ն˷ֶθ˼�
			{
				pLinePart.LineAnglePointer()->pOldDesEndPos->datum_to_ber_style=1;	//ƽ��
				pLinePart.LineAnglePointer()->pOldDesEndPos->cFaceOffsetDatumLine='X';
				if((pLinePart->pEnd->Position(true).x<0&&pLinePart->pEnd->Position(true).y>0)||
				   (pLinePart->pEnd->Position(true).x>0&&pLinePart->pEnd->Position(true).y<0))
					pLinePart.LineAnglePointer()->pOldDesEndPos->cFaceOffsetDatumLine='Y';
			}

			if(pLinePart!=pStartLinePart)
			{	//����ʼ�ֶθ˼�
				pLinePart.LineAnglePointer()->pOldDesStartPos->jgber_cal_style=1;	//����ͶӰ
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
					pStartLinePart.LineAnglePointer()->pOldDesStartPos->jgber_cal_style==2) //ƽ��
				{
					pLinePart.LineAnglePointer()->pOldDesStartPos->jgber_cal_style=2;	//����ƽ��
					pLinePart.LineAnglePointer()->pOldDesStartPos->datum_to_ber_style=1;	//ƽ��
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
				pLinePart.LineAnglePointer()->pOldDesStartPos->datum_to_ber_style=1;	//ƽ��
				if(!slopePosInfo.bTopSlope)	//��ǰ��߷ֶ����滹���������¶ȶΣ���ʱΪ��֤���������������Ϊƽ�ƻ�׼
					pLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine=0;
				else
				{
					pLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine='X';
					if((pLinePart->pEnd->Position(false).x<0&&pLinePart->pEnd->Position(false).y>0)||
						(pLinePart->pEnd->Position(false).x>0&&pLinePart->pEnd->Position(false).y<0))
						pLinePart.LineAnglePointer()->pOldDesStartPos->cFaceOffsetDatumLine='Y';
				}
			}
			//else //ʼ�˷ֶθ˼�
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
			//���ڷ�ĩ���¶ε��ն˽ڵ㸸�˼���ʱ������һ�¶��趨�ģ��������ڸ��˼����Կ��ܻᵼ��λ�ü�������쳣
			//���ڴ˴�������λ�ö��������������ͳһ����
			//pLinePart.LinePartPointer()->CalPosition();
			pLinePart.LineAnglePointer()->cal_x_wing_norm();
			pLinePart.LineAnglePointer()->cal_y_wing_norm();
#else
			//���"!slopePosInfo.bBottomSlope"��Ҫ�������м��¶�ĩ��Ӧ��Ϊ����һ�¶���ʼ�Ǹ�����ͶӰ
			if(pLinePart!=pEndLinePart||!slopePosInfo.bBottomSlope)
			{	//�����ն˷ֶθ˼���ǰ�¶����滹���������¶�
				SnapPerp(&pos,ber_line,pLinePart->pEnd->pos);
				pLinePart->SetEnd(pos);
			}

			if(pLinePart!=pStartLinePart)
			{	//����ʼ�ֶθ˼�
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
			//����ϵĸ˼���ʼ/��ֹ�㶨λ��ʽ�����ǵ���ͶӰ
			if(pLinePart!=pEndLinePart)
			{	//�����շֶθ˼�
				pLinePart.LineTubePointer()->desEndPos.endPosType=2;	//����ͶӰ
				pLinePart.LineTubePointer()->desEndPos.bDatumStartStartEnd=TRUE;
				pLinePart.LineTubePointer()->desEndPos.bDatumEndStartEnd=FALSE;
				pLinePart.LineTubePointer()->desEndPos.hDatumStartPart=pStartLinePart->handle;
				pLinePart.LineTubePointer()->desEndPos.hDatumEndPart=pEndLinePart->handle;
				SnapPerp(&pos,ber_line,pLinePart->pEnd->Position(false));
				pLinePart->SetEnd(pos);
			}
			//else //�ն˷ֶθ˼�
			if(pLinePart!=pStartLinePart)
			{	//����ʼ�ֶθ˼�
				pLinePart.LineTubePointer()->desStartPos.endPosType=2;	//����ͶӰ
				pLinePart.LineTubePointer()->desStartPos.bDatumStartStartEnd=TRUE;
				pLinePart.LineTubePointer()->desStartPos.bDatumEndStartEnd=FALSE;
				pLinePart.LineTubePointer()->desStartPos.hDatumStartPart=pStartLinePart->handle;
				pLinePart.LineTubePointer()->desStartPos.hDatumEndPart=pEndLinePart->handle;
				SnapPerp(&pos,ber_line,pLinePart->pStart->Position(false));
				pLinePart->SetStart(pos);
			}
			//else //ʼ�˷ֶθ˼�
			if(pLinePart!=pStartLinePart)
			{
				pLinePart.LineTubePointer()->nearWorkPlaneNorm=pStartLinePart.LineTubePointer()->nearWorkPlaneNorm;
				pLinePart.LineTubePointer()->hWorkNormRefPart=pStartLinePart.LineTubePointer()->hWorkNormRefPart;
			}
			pLinePart.LineTubePointer()->CalPosition();
			pLinePart.LineTubePointer()->CalWorkPlaneNorm();
#else
			//����ϵĸ˼���ʼ/��ֹ�㶨λ��ʽ�����ǵ���ͶӰ
			if(pLinePart!=pEndLinePart)
			{	//�����շֶθ˼�
				SnapPerp(&pos,ber_line,pLinePart->pEnd->pos);
				pLinePart->SetEnd(pos);
			}
			//else //�ն˷ֶθ˼�
			if(pLinePart!=pStartLinePart)
			{	//����ʼ�ֶθ˼�
				SnapPerp(&pos,ber_line,pLinePart->pStart->pos);
				pLinePart->SetStart(pos);
			}
#endif
		}
	}
}
#endif
#endif
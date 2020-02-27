#include "stdafx.h"
#include "..\LDS.h"
#include "..\LDSDoc.h"
#include "..\LDSView.h"
#include "..\dlg.h"
#include "..\env_def.h"
#include "..\GlobalFunc.h"
#include "..\PromptDlg.h"
#include "..\MirTaAtom.h"
#include "..\GroupJgStyleDlg.h"
#include "Tower.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __ALFA_TEST_VERSION_

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
BOOL Test_InitLineAnglePosAndWingNorm(CLDSLineAngle* pCurJg,f3dPoint view_norm,BOOL bStartToBer/*=FALSE*/,BOOL bEndToBer/*=FALSE*/)
{
	CLDSPart *pPart;
	CString sErrorMsg;
	f3dPoint vec;
	if(pCurJg->pOldDesStartPos==NULL||pCurJg->pOldDesEndPos==NULL)
	{
		logto.Log("error");
		return FALSE;
	}
	try{
		//���㵱ǰ�Ǹֵ����췽����ʼ��ָ���ն�Ϊ��
		Sub_Pnt(vec,pCurJg->pEnd->Position(false),pCurJg->pStart->Position(true));
		if(vec.mod()<EPS)
			throw "��ǰ�Ǹ�ʼ�ն˵��غ�,���ݴ���!";
		normalize(vec);
		normalize(view_norm);
		if(fabs(vec*view_norm)>EPS_COS)
			throw "����ָ������!";
		switch(pCurJg->lstyle)
		{
		case 1:
		case 3:
			if(pCurJg->lstyle==1)	//����
				pCurJg->des_norm_x.near_norm =-view_norm;	//��֫���߽ӽ��Ļ�׼����
			else					//����
				pCurJg->des_norm_x.near_norm = view_norm;	//��֫���߽ӽ��Ļ�׼����
			pCurJg->des_norm_x.bByOtherWing = FALSE;
			pCurJg->des_norm_y.bByOtherWing = TRUE;		//Y֫������X֫����Ϊ��׼
			if(pCurJg->pStart->hFatherPart!=pCurJg->handle)
			{
				pCurJg->pOldDesStartPos->offset_X_dist_style = 4;	//�Ǹ�X֫�������Ǹ�
				pCurJg->pOldDesStartPos->offset_X_dist = 0;
				pCurJg->pOldDesStartPos->offset_Y_dist_style = 0;	//�Ǹ�Y֫ƫ��һ��׼��
				//pCurJg->pOldDesStartPos->offset_Y_dist = 0;
			}
			if(pCurJg->pEnd->hFatherPart!=pCurJg->handle)
			{
				pCurJg->pOldDesEndPos->offset_X_dist_style = 4;	//�Ǹ�X֫�������Ǹ�
				pCurJg->pOldDesEndPos->offset_X_dist = 0;
				pCurJg->pOldDesEndPos->offset_Y_dist_style = 0;	//�Ǹ�Y֫ƫ��һ��׼��
				//pCurJg->pOldDesEndPos->offset_Y_dist = 0;
			}
			break;
		case 2:
		case 4:
		default:
			if(pCurJg->lstyle==2)	//����
				pCurJg->des_norm_y.near_norm = view_norm;	//��֫���߽ӽ��Ļ�׼����
			else					//����
				pCurJg->des_norm_y.near_norm =-view_norm;	//��֫���߽ӽ��Ļ�׼����
			pCurJg->des_norm_y.bByOtherWing = FALSE;
			pCurJg->des_norm_x.bByOtherWing = TRUE;		//X֫������Y֫����Ϊ��׼
			if(pCurJg->pStart->hFatherPart!=pCurJg->handle)
			{
				pCurJg->pOldDesStartPos->offset_X_dist_style = 0;	//�Ǹ�X֫ƫ��һ��׼��
				//pCurJg->pOldDesStartPos->offset_X_dist = 0;
				pCurJg->pOldDesStartPos->offset_Y_dist_style = 4;	//�Ǹ�Y֫�������Ǹ�
				pCurJg->pOldDesStartPos->offset_Y_dist = 0;
			}
			if(pCurJg->pEnd->hFatherPart!=pCurJg->handle)
			{
				pCurJg->pOldDesEndPos->offset_X_dist_style = 0;	//�Ǹ�X֫ƫ��һ��׼��
				//pCurJg->pOldDesEndPos->offset_X_dist = 0;
				pCurJg->pOldDesEndPos->offset_Y_dist_style = 4;	//�Ǹ�Y֫�������Ǹ�
				pCurJg->pOldDesEndPos->offset_Y_dist = 0;
			}
			break;
		}
		
		
		CSuperSmartPtr<CLDSLinePart> pStartFatherPart,pEndFatherPart;
		pPart = Ta.Parts.FromHandle(pCurJg->pStart->hFatherPart);
		if(pPart&&pPart->IsLinePart())
			pStartFatherPart = (CLDSLinePart*)pPart;
		pPart = Ta.Parts.FromHandle(pCurJg->pEnd->hFatherPart);
		if(pPart&&pPart->IsLinePart())
			pEndFatherPart = (CLDSLinePart*)pPart;
		short start_attach_type=pCurJg->pStart->m_cPosCalType;//��ǰ�Ǹ�ʼ�˽ڵ�������ϵ
		short end_attach_type=pCurJg->pEnd->m_cPosCalType;	//��ǰ�Ǹ��ն˽ڵ�������ϵ
		//	****0.0���������Ҫ����ǰ�Ǹ�Ϊ����潻��ĵ����*****
		if(toupper(pCurJg->layer(2))=='G'&&pStartFatherPart.IsHasPtr())
		{
			f3dPoint cur_norm = CalFaceNorm(pCurJg->Start(),pCurJg->End(),pStartFatherPart->Start(),pStartFatherPart->End());
			if(cur_norm.mod()<EPS||fabs(cur_norm*view_norm)<EPS)
			{
				pStartFatherPart=NULL;
				pEndFatherPart=NULL;
				start_attach_type=end_attach_type=0; //�����ն˽ڵ�Ϊ��׼�ڵ��������д���
			}
		}
		//	****1.0 ��������Ϊ��ǰ�Ǹ���һ�˽ڵ�Ϊ�����ڵ�����****
		/*
		��ǰ�Ǹֶ˽ڵ�ĸ��Ǹ��Ѵ���(�ڵ�Ϊ�丸�Ǹֵ������ڵ�,�Ǹֵ�
		�����ڵ�һ��λ�ڽǸֵ��м䲻Ӧλ�ڽǸֵĶ˵�,������Ϊ�����ݴ���
		����ʱ�׳�������Ϣ),һ����˵,����(�����ڵ�)�ĸ��Ǹֲ�Ӧ�뵱ǰ��
		�ֹ���,�������,����Ϊ�����ݴ��󣬴�ʱ�׳�������Ϣ��
		*/
		if(start_attach_type>3)
		{	//ʼ�˵�Ϊ�����ڵ�
			if(!pStartFatherPart.IsHasPtr())
			{
				char sError[MAX_TEMP_CHAR_200+1]="";
				_snprintf(sError,MAX_TEMP_CHAR_200,"�ڵ�0X%X�ĵĸ��˼���ʧ,֫���߷������ʧ��!",pCurJg->pStart->handle);
				throw sError;
			}
			if(pCurJg->des_norm_y.bByOtherWing)	//Y֫������X֫����Ϊ�����׼
				pCurJg->des_norm_x.hViceJg = pStartFatherPart->handle;
			else								//X֫������Y֫����Ϊ�����׼
				pCurJg->des_norm_y.hViceJg = pStartFatherPart->handle;
		}
		else if(end_attach_type>3)
		{	//�ն˵�Ϊ�����ڵ�
			if(!pEndFatherPart.IsHasPtr())
			{
				char sError[MAX_TEMP_CHAR_200+1]="";
				_snprintf(sError,MAX_TEMP_CHAR_200,"�ڵ�0X%X�ĸ��˼���ʧ,֫���߷������ʧ��!",pCurJg->pEnd->handle);
				throw sError;
			}
			if(pCurJg->des_norm_y.bByOtherWing)	//Y֫������X֫����Ϊ�����׼
				pCurJg->des_norm_x.hViceJg = pEndFatherPart->handle;
			else								//X֫������Y֫����Ϊ�����׼
				pCurJg->des_norm_y.hViceJg = pEndFatherPart->handle;
		}
		//	****2.0 ����Ϊ��ǰ�ǸֵĶ˽ڵ��Ϊ�����ڵ�����****
		else
		{
			if( pStartFatherPart.IsHasPtr()
				&&pStartFatherPart->pStart!=pCurJg->pEnd
				&&pStartFatherPart->pEnd!=pCurJg->pEnd
				&&pStartFatherPart->cfgword.And(pCurJg->cfgword))//ʼ�˽ڵ㸸�Ǹֲ��ڵ�ǰģ�� wht 10-06-06
			{	//ʼ�˵㸸�Ǹ��뵱ǰ�Ǹֲ���ͬ
				if(pCurJg->des_norm_y.bByOtherWing)	//Y֫������X֫����Ϊ�����׼
					pCurJg->des_norm_x.hViceJg = pStartFatherPart->handle;
				else								//X֫������Y֫����Ϊ�����׼
					pCurJg->des_norm_y.hViceJg = pStartFatherPart->handle;
			}
			else if(pEndFatherPart.IsHasPtr()
				&&pEndFatherPart->pStart!=pCurJg->pStart
				&&pEndFatherPart->pEnd!=pCurJg->pStart
				&&pEndFatherPart->cfgword.And(pCurJg->cfgword))	//�ն˽ڵ㸸�Ǹֲ��ڵ�ǰģ�� wht 10-06-06
			{	//�ն˵㸸�Ǹ��뵱ǰ�Ǹֲ���ͬ
				if(pCurJg->des_norm_y.bByOtherWing)	//Y֫������X֫����Ϊ�����׼
					pCurJg->des_norm_x.hViceJg = pEndFatherPart->handle;
				else								//X֫������Y֫����Ϊ�����׼
					pCurJg->des_norm_y.hViceJg = pEndFatherPart->handle;
			}
			//�޷����ݸ��Ǹ�ȷ���ýǸֵ�֫���߷���ʱ���������ļ���֫��ʸ
			//�ָ�Ϊ����ͼ���߷�����ȷ��  WJH--01.06.25
			else
			{
				if(pCurJg->des_norm_x.bByOtherWing)
				{	//X֫������Y֫����Ϊ�����׼
					pCurJg->set_norm_y_wing(pCurJg->des_norm_y.near_norm,TRUE);
				}
				else
				{	//Y֫������X֫����Ϊ�����׼
					pCurJg->set_norm_x_wing(pCurJg->des_norm_x.near_norm,TRUE);
				}
			}
		}
		//����Ǹ���֫����
		pCurJg->cal_x_wing_norm();
		pCurJg->cal_y_wing_norm();
		if(!pCurJg->des_norm_x.bByOtherWing)
		{
			if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINEANGLE)	//���Ǹ�
			{
				if(pCurJg==pStartFatherPart)
					pCurJg->pOldDesStartPos->datum_pos_style = 0;
				else if(fabs(pCurJg->get_norm_x_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->pOldDesStartPos->datum_pos_style = 2;
				else
					pCurJg->pOldDesStartPos->datum_pos_style = 3;
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pStartFatherPart.LineAnglePointer();
				if(pCurJg==pStartFatherPart)
					pCurJg->pOldDesStartPos->datum_pos_style = 0;
				else if(pGroupJg->group_style==1)	//T����ϽǸ�
				{
					if(fabs(pCurJg->get_norm_x_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->pOldDesStartPos->datum_pos_style = 2;
					else
						pCurJg->pOldDesStartPos->datum_pos_style = 1;	//����Y֫��ʱʵ��Ӧ��������
				}
				else
					pCurJg->pOldDesStartPos->datum_pos_style=1;	//��������
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->pOldDesStartPos->datum_pos_style = 1;	//�ڵ�������ͶӰ
			else
				pCurJg->pOldDesStartPos->datum_pos_style=0;	//�ڵ㼴����
			if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(pCurJg==pEndFatherPart)
					pCurJg->pOldDesEndPos->datum_pos_style = 0;
				else if(fabs(pCurJg->get_norm_x_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->pOldDesEndPos->datum_pos_style = 2;
				else
					pCurJg->pOldDesEndPos->datum_pos_style = 3;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pEndFatherPart.LineAnglePointer();
				if(pCurJg==pEndFatherPart)
					pCurJg->pOldDesEndPos->datum_pos_style = 0;
				else if(pGroupJg->group_style==1)	//T����ϽǸ�
				{
					if(fabs(pCurJg->get_norm_x_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->pOldDesEndPos->datum_pos_style = 2;
					else
						pCurJg->pOldDesEndPos->datum_pos_style = 1;
				}
				else
					pCurJg->pOldDesEndPos->datum_pos_style=1;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->pOldDesEndPos->datum_pos_style=1;	//�ڵ�������ͶӰ
			else
				pCurJg->pOldDesEndPos->datum_pos_style=0;	//�ڵ㼴����
		}
		else //if(pCurJg->des_norm_y.bByOtherWing)
		{
			if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(pCurJg==pStartFatherPart)
					pCurJg->pOldDesStartPos->datum_pos_style = 0;
				else if(fabs(pCurJg->get_norm_y_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->pOldDesStartPos->datum_pos_style = 2;
				else
					pCurJg->pOldDesStartPos->datum_pos_style = 3;
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pStartFatherPart.LineAnglePointer();
				if(pCurJg==pStartFatherPart)
					pCurJg->pOldDesStartPos->datum_pos_style = 0;
				else if(pGroupJg->group_style==1)		//T����ϽǸ�
				{
					if(fabs(pCurJg->get_norm_y_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->pOldDesStartPos->datum_pos_style = 2;
					else
						pCurJg->pOldDesStartPos->datum_pos_style = 1;
				}
				else
					pCurJg->pOldDesStartPos->datum_pos_style=1;
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->pOldDesStartPos->datum_pos_style = 1;	//�ڵ�������ͶӰ
			else
				pCurJg->pOldDesStartPos->datum_pos_style=0;
			if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(pCurJg==pEndFatherPart)
					pCurJg->pOldDesEndPos->datum_pos_style = 0;
				else if(fabs(pCurJg->get_norm_y_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->pOldDesEndPos->datum_pos_style = 2;
				else
					pCurJg->pOldDesEndPos->datum_pos_style = 3;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pEndFatherPart.LineAnglePointer();
				if(pCurJg==pEndFatherPart)
					pCurJg->pOldDesEndPos->datum_pos_style = 0;
				else if(pGroupJg->group_style==1)	//T����ϽǸ�
				{
					if(fabs(pCurJg->get_norm_y_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->pOldDesEndPos->datum_pos_style = 2;
					else
						pCurJg->pOldDesEndPos->datum_pos_style = 1;
				}
				else
					pCurJg->pOldDesEndPos->datum_pos_style=1;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->pOldDesEndPos->datum_pos_style = 1;	//�ڵ�������ͶӰ
			else
				pCurJg->pOldDesEndPos->datum_pos_style=0;
		}
		if(bStartToBer&&pCurJg!=pStartFatherPart)
			pCurJg->pOldDesStartPos->datum_pos_style=1;
		if(bEndToBer&&pCurJg!=pEndFatherPart)
			pCurJg->pOldDesEndPos->datum_pos_style=1;
		pCurJg->CalPosition();
	}
	catch(char *sError)
	{
		CString ss;
		ss.Format("����Ǹ�֫���ߴ���%s",sError);
		AfxMessageBox(ss);
	}
	return TRUE;
}

#endif
#endif
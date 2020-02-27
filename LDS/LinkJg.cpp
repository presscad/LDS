//<LOCALE_TRANSLATE Confirm by hxr/>
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
#include "GroupJgStyleDlg.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
/////////////////////////////////////////////////////////
static CLinkJGDlg link_jg_dlg;
/////////////////////////////////////////////////////////
/*����Ϊ�����������Ǹֵ�֫���߷���
	1.��ǰ�Ǹ����˵�����һ���������ڵ�,��ʱ���ݴ˽ڵ����
	  ��ǰ�Ǹֵ�֫���߷���
	2.��ǰ�Ǹֵ����˵��Ϊ�����ڵ�,����һ�˵�ĸ��Ǹ��뵱ǰ�Ǹֲ���ͬ
	3.��ǰ�Ǹֵ����˵��Ϊ�����ڵ�,�Ҷ���û�и��ڵ�
	****�����ڵ�����������κ������Ľڵ��Z�����᲻��Ľڵ�****
*/
static BOOL InitLineAnglePosAndWingNorm(CLDSLineAngle* pCurJg,f3dPoint view_norm,BOOL bStartToBer/*=FALSE*/,BOOL bEndToBer/*=FALSE*/)
{
	CLDSPart *pPart;
	CString sErrorMsg;
	f3dPoint vec;

	try{
		//���㵱ǰ�Ǹֵ����췽����ʼ��ָ���ն�Ϊ��
		Sub_Pnt(vec,pCurJg->pEnd->Position(true),pCurJg->pStart->Position(true));
		if(vec.mod()<EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "Current angle's start and end point overlap,data error!";
#else
			throw "��ǰ�Ǹ�ʼ�ն˵��غ�,���ݴ���!";
#endif
		normalize(vec);
		normalize(view_norm);
		if(fabs(vec*view_norm)>EPS_COS)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "View vector specified wrong!";
#else
			throw "����ָ������!";
#endif
		switch(pCurJg->LayStyle)
		{
		case 1:	//X֫Ϊ��ǰ֫
		case 3:	//X֫Ϊ��ǰ֫
			if(pCurJg->LayStyle==1)	//����
				pCurJg->des_norm_x.near_norm =-view_norm;	//��֫���߽ӽ��Ļ�׼����
			else					//����
				pCurJg->des_norm_x.near_norm = view_norm;	//��֫���߽ӽ��Ļ�׼����
			pCurJg->des_norm_x.bByOtherWing = FALSE;
			pCurJg->des_norm_y.bByOtherWing = TRUE;		//Y֫������X֫����Ϊ��׼
			if(pCurJg->pStart->hFatherPart<0x20)
			{
				pCurJg->pStart->hFatherPart = pCurJg->handle;
				pCurJg->pStart->SetModified();
				if( pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_XY_SPECIFIED||
					pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||
					pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
				{
					pCurJg->desStartPos.spatialOperationStyle=0;	//ָ����׼��
					pCurJg->desStartPos.datumPoint.datum_pos_style=8;
					pCurJg->desStartPos.datumPoint.des_para.hNode=pCurJg->pStart->handle;
					pCurJg->desStartPos.wing_x_offset.gStyle=4;
					pCurJg->desStartPos.wing_x_offset.offsetDist=0;
				}
				else if(pCurJg->Layer(0)=='T')
				{	//ͷ���Ǹ�Ĭ��Ϊ��֫�������
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 15;
				}
				else
				{
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 18;
				}
			}
			else if(pCurJg->pStart->hFatherPart!=pCurJg->handle)
			{
				pCurJg->desStartPos.wing_x_offset.gStyle = 4;	//�Ǹ�X֫�������Ǹ�
				pCurJg->desStartPos.wing_x_offset.offsetDist = 0;
				pCurJg->desStartPos.wing_y_offset.gStyle = 0;	//�Ǹ�Y֫ƫ��һ��׼��
				//pCurJg->desStartPos.wing_y_offset.offsetDist = 0;
			}
			if(pCurJg->pEnd->hFatherPart<0x20)
			{
				pCurJg->pEnd->hFatherPart = pCurJg->handle;
				pCurJg->pEnd->SetModified();
				if( pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_XY_SPECIFIED||
					pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||
					pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
				{
					pCurJg->desEndPos.spatialOperationStyle=0;	//ָ����׼��
					pCurJg->desEndPos.datumPoint.datum_pos_style=8;
					pCurJg->desEndPos.datumPoint.des_para.hNode=pCurJg->pEnd->handle;
					pCurJg->desEndPos.wing_x_offset.gStyle=4;
					pCurJg->desEndPos.wing_x_offset.offsetDist=0;
				}
				else if(pCurJg->Layer(0)=='T')
				{	//ͷ���Ǹ�Ĭ��Ϊ��֫�������
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 15;
				}
				else
				{
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 18;
				}
			}
			else if(pCurJg->pEnd->hFatherPart!=pCurJg->handle)
			{
				pCurJg->desEndPos.wing_x_offset.gStyle = 4;	//�Ǹ�X֫�������Ǹ�
				pCurJg->desEndPos.wing_x_offset.offsetDist = 0;
				pCurJg->desEndPos.wing_y_offset.gStyle = 0;	//�Ǹ�Y֫ƫ��һ��׼��
				//pCurJg->desEndPos.wing_y_offset.offsetDist = 0;
			}
			break;
		case 2:	//Y֫Ϊ��ǰ֫
		case 4:	//Y֫Ϊ��ǰ֫
		default:
			if(pCurJg->LayStyle==2)	//����
				pCurJg->des_norm_y.near_norm = view_norm;	//��֫���߽ӽ��Ļ�׼����
			else					//����
				pCurJg->des_norm_y.near_norm =-view_norm;	//��֫���߽ӽ��Ļ�׼����
			pCurJg->des_norm_y.bByOtherWing = FALSE;
			pCurJg->des_norm_x.bByOtherWing = TRUE;		//X֫������Y֫����Ϊ��׼
			if(pCurJg->pStart->hFatherPart<0x20)
			{
				pCurJg->pStart->hFatherPart = pCurJg->handle;
				pCurJg->pStart->SetModified();
				if( pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_XY_SPECIFIED||
					pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||
					pCurJg->pStart->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
				{
					pCurJg->desStartPos.spatialOperationStyle=0;	//ָ����׼��
					pCurJg->desStartPos.datumPoint.datum_pos_style=8;
					pCurJg->desStartPos.datumPoint.des_para.hNode=pCurJg->pStart->handle;
					pCurJg->desStartPos.wing_y_offset.gStyle=4;
					pCurJg->desStartPos.wing_y_offset.offsetDist=0;
				}
				else if(pCurJg->Layer(0)=='T')
				{	//ͷ���Ǹ�Ĭ��Ϊ��֫�������
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 15;
				}
				else
				{
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 18;
				}
			}
			else if(pCurJg->pStart->hFatherPart!=pCurJg->handle)
			{
				pCurJg->desStartPos.wing_x_offset.gStyle = 0;	//�Ǹ�X֫ƫ��һ��׼��
				//pCurJg->desStartPos.wing_x_offset.offsetDist = 0;
				pCurJg->desStartPos.wing_y_offset.gStyle = 4;	//�Ǹ�Y֫�������Ǹ�
				pCurJg->desStartPos.wing_y_offset.offsetDist = 0;
			}
			if(pCurJg->pEnd->hFatherPart<0x20)
			{
				pCurJg->pEnd->hFatherPart = pCurJg->handle;
				pCurJg->pEnd->SetModified();
				if( pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_XY_SPECIFIED||
					pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_YZ_SPECIFIED||
					pCurJg->pEnd->m_cPosCalType==CLDSNode::PLANE_XZ_SPECIFIED)
				{
					pCurJg->desEndPos.spatialOperationStyle=0;	//ָ����׼��
					pCurJg->desEndPos.datumPoint.datum_pos_style=8;
					pCurJg->desEndPos.datumPoint.des_para.hNode=pCurJg->pEnd->handle;
					pCurJg->desEndPos.wing_y_offset.gStyle=4;
					pCurJg->desEndPos.wing_y_offset.offsetDist=0;
				}
				else if(pCurJg->Layer(0)=='T')
				{	//ͷ���Ǹ�Ĭ��Ϊ��֫�������
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 15;
				}
				else
				{
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 18;
				}
			}
			else if(pCurJg->pEnd->hFatherPart!=pCurJg->handle)
			{
				pCurJg->desEndPos.wing_x_offset.gStyle = 0;	//�Ǹ�X֫ƫ��һ��׼��
				//pCurJg->desEndPos.wing_x_offset.offsetDist = 0;
				pCurJg->desEndPos.wing_y_offset.gStyle = 4;	//�Ǹ�Y֫�������Ǹ�
				pCurJg->desEndPos.wing_y_offset.offsetDist = 0;
			}
			break;
		}
		
		
		CSuperSmartPtr<CLDSLinePart> pStartFatherPart,pEndFatherPart;
		if(pCurJg->desStartPos.spatialOperationStyle==0&&pCurJg->desStartPos.datumPoint.datum_pos_style==3)
			pPart=console.FromPartHandle(pCurJg->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum1);
		else
			pPart=console.FromPartHandle(pCurJg->pStart->hFatherPart);
		if(pPart&&pPart->IsLinePart())
			pStartFatherPart = (CLDSLinePart*)pPart;
		if(pCurJg->desEndPos.spatialOperationStyle==0&&pCurJg->desEndPos.datumPoint.datum_pos_style==3)
			pPart=console.FromPartHandle(pCurJg->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum1);
		else
			pPart=console.FromPartHandle(pCurJg->pEnd->hFatherPart);
		if(pPart&&pPart->IsLinePart())
			pEndFatherPart = (CLDSLinePart*)pPart;
		short start_attach_type=pCurJg->pStart->m_cPosCalType;//��ǰ�Ǹ�ʼ�˽ڵ�������ϵ
		short end_attach_type=pCurJg->pEnd->m_cPosCalType;	//��ǰ�Ǹ��ն˽ڵ�������ϵ
		/*  �ⲿ�ִ�������TMA V5�汾�ѶԺ����ǵ����Ľ����˴���(�������߽���)��
			��������ʵ�����壬�ҿ����и������� wjh-2016.5.19
			****0.0���������Ҫ����ǰ�Ǹ�Ϊ����潻��ĵ����*****
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
		*/
		CLDSLinePart* pWorkPlaneRod=NULL;
		//	****1.0 ��������Ϊ��ǰ�Ǹ���һ�˽ڵ�Ϊ�����ڵ�����****
		/*
		��ǰ�Ǹֶ˽ڵ�ĸ��Ǹ��Ѵ���(�ڵ�Ϊ�丸�Ǹֵ������ڵ�,�Ǹֵ�
		�����ڵ�һ��λ�ڽǸֵ��м䲻Ӧλ�ڽǸֵĶ˵�,������Ϊ�����ݴ���
		����ʱ�׳�������Ϣ),һ����˵,����(�����ڵ�)�ĸ��Ǹֲ�Ӧ�뵱ǰ��
		�ֹ���,�������,����Ϊ�����ݴ��󣬴�ʱ�׳�������Ϣ��
		*/
		if(!pStartFatherPart.IsHasPtr())
		{
			char sError[MAX_TEMP_CHAR_200+1]="";
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sError,MAX_TEMP_CHAR_200,"parent rod of node 0X%X lost, failure in calculating leg's normal direction!",pCurJg->pStart->handle);
#else
			_snprintf(sError,MAX_TEMP_CHAR_200,"�ڵ�0X%X�ĵĸ��˼���ʧ,֫���߷������ʧ��!",pCurJg->pStart->handle);
#endif
			throw sError;
		}
		else if(start_attach_type>3&&pStartFatherPart->handle!=pCurJg->handle)
		{	//ʼ�˵�Ϊ�����ڵ�
			if(pCurJg->des_norm_y.bByOtherWing)	//Y֫������X֫����Ϊ�����׼
				pCurJg->des_norm_x.hViceJg = pStartFatherPart->handle;
			else								//X֫������Y֫����Ϊ�����׼
				pCurJg->des_norm_y.hViceJg = pStartFatherPart->handle;
			pWorkPlaneRod=pStartFatherPart;
		}
		else if(!pEndFatherPart.IsHasPtr())
		{
			char sError[MAX_TEMP_CHAR_200+1]="";
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sError,MAX_TEMP_CHAR_200,"parent rod of node0X%X lost, failure in calculating leg's normal direction!",pCurJg->pEnd->handle);
#else
			_snprintf(sError,MAX_TEMP_CHAR_200,"�ڵ�0X%X�ĸ��˼���ʧ,֫���߷������ʧ��!",pCurJg->pEnd->handle);
#endif
			throw sError;
		}
		else if((start_attach_type<=3||pStartFatherPart.IsNULL())&&end_attach_type>3&&pEndFatherPart->handle!=pCurJg->handle)
		{	//�ն˵�Ϊ�����ڵ�
			if(pCurJg->des_norm_y.bByOtherWing)	//Y֫������X֫����Ϊ�����׼
				pCurJg->des_norm_x.hViceJg = pEndFatherPart->handle;
			else								//X֫������Y֫����Ϊ�����׼
				pCurJg->des_norm_y.hViceJg = pEndFatherPart->handle;
			pWorkPlaneRod=pEndFatherPart;
		}
		//	****2.0 ����Ϊ��ǰ�ǸֵĶ˽ڵ��Ϊ�����ڵ�����****
		else
		{
			if( pStartFatherPart.IsHasPtr()&&pStartFatherPart->handle!=pCurJg->handle
				&&pStartFatherPart->pStart!=pCurJg->pEnd	//����Ϊ�ڵ㸸�˼�ʱ������Ϊ֫���߲��ո˼� wjh-2014.11.28
				&&pStartFatherPart->pEnd!=pCurJg->pEnd
				&&pStartFatherPart->cfgword.And(pCurJg->cfgword))//ʼ�˽ڵ㸸�Ǹֲ��ڵ�ǰģ�� wht 10-06-06
			{	//ʼ�˵㸸�Ǹ��뵱ǰ�Ǹֲ���ͬ
				if(pCurJg->des_norm_y.bByOtherWing)	//Y֫������X֫����Ϊ�����׼
					pCurJg->des_norm_x.hViceJg = pStartFatherPart->handle;
				else								//X֫������Y֫����Ϊ�����׼
					pCurJg->des_norm_y.hViceJg = pStartFatherPart->handle;
				pWorkPlaneRod=pStartFatherPart;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->handle!=pCurJg->handle
				&&pEndFatherPart->pStart!=pCurJg->pStart	//����Ϊ�ڵ㸸�˼�ʱ������Ϊ֫���߲��ո˼� wjh-2014.11.28
				&&pEndFatherPart->pEnd!=pCurJg->pStart
				&&pEndFatherPart->cfgword.And(pCurJg->cfgword))	//�ն˽ڵ㸸�Ǹֲ��ڵ�ǰģ�� wht 10-06-06
			{	//�ն˵㸸�Ǹ��뵱ǰ�Ǹֲ���ͬ
				if(pCurJg->des_norm_y.bByOtherWing)	//Y֫������X֫����Ϊ�����׼
					pCurJg->des_norm_x.hViceJg = pEndFatherPart->handle;
				else								//X֫������Y֫����Ϊ�����׼
					pCurJg->des_norm_y.hViceJg = pEndFatherPart->handle;
				pWorkPlaneRod=pEndFatherPart;
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
		DESIGN_JGWING_NORM* pWorkWingNorm=!pCurJg->des_norm_x.bByOtherWing ? &pCurJg->des_norm_x : &pCurJg->des_norm_y;
		if(pWorkPlaneRod!=NULL&&!pWorkWingNorm->bByOtherWing&&pCurJg->pStart&&pCurJg->pEnd)
		{	//��Թ����淨���뵱ǰ��ͼ������������
			f3dPoint lenDependRodVec=pWorkPlaneRod->End()-pWorkPlaneRod->Start();
			f3dPoint lenRodVec=pCurJg->pEnd->Position(false)-pCurJg->pStart->Position(false);
			normalize(lenRodVec);
			normalize(lenDependRodVec);
			normalize(pWorkWingNorm->near_norm);
			f3dPoint vWorkNorm=lenDependRodVec^lenRodVec;
			normalize(vWorkNorm);
			if(fabs(vWorkNorm*pWorkWingNorm->near_norm)<0.707)
			{	//��ǰ�Ǹ��������˼������淨���뵱ǰ��ͼ���߼нǴ���45��
				pWorkWingNorm->near_norm=pWorkWingNorm->near_norm^lenRodVec;
				if(pCurJg->LayStyle==2||pCurJg->LayStyle==4)	//Y֫Ϊ��ǰ����֫
					pWorkWingNorm->near_norm*=-1.0;
				//��ǰ����֫X/Y����
				DESIGN_JGWING_NORM temp=pCurJg->des_norm_x;
				pCurJg->des_norm_x=pCurJg->des_norm_y;
				pCurJg->des_norm_y=temp;
			}
		}
		pCurJg->cal_x_wing_norm();
		pCurJg->cal_y_wing_norm();
		if(!pCurJg->des_norm_x.bByOtherWing)
		{
			if(pStartFatherPart.IsHasPtr()&&!pStartFatherPart.IsEqualPtr(pCurJg)&&pStartFatherPart->GetClassTypeId()==CLS_LINEANGLE)	//���Ǹ�
			{
				if(fabs(pCurJg->get_norm_x_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->desStartPos.spatialOperationStyle = 2;
				else
					pCurJg->desStartPos.spatialOperationStyle = 3;
				//��ᵣ����Ĭ����Ϊƽ�ƣ������Ǹ�������Ǹִ��������Ǹֽ����ϵ��´���
				//ʵ�ʴ˴�Ӧ�������趨����֫Ϊ��Ӷ�λ��׼֫��Ŀǰ��ͨ���򵥶�λ��������⡡wjh-2016.5.18
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED||	//ָ��X���������
				//	pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED||	//ָ��Y���������
				//	pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//ָ��Z���������
				//	pCurJg->desStartPos.spatialOperationStyle+=3;	//ͶӰ��ƽ��
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)	//ָ��X���������
				//	pCurJg->desStartPos.face_offset_norm.Set(1,0,0);
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)	//ָ��Y���������
				//	pCurJg->desStartPos.face_offset_norm.Set(0,1,0);
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//ָ��Z���������
				//	pCurJg->desStartPos.face_offset_norm.Set(0,0,1);
			}
			else if(pStartFatherPart.IsHasPtr()&&!pStartFatherPart.IsEqualPtr(pCurJg)&&pStartFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pStartFatherPart.LineAnglePointer();
				if(pCurJg==pStartFatherPart)
				{
					//pCurJg->desEndPos.datum_pos_style = 0;
					pCurJg->desStartPos.origin_pos_style = 0;
					pCurJg->desStartPos.spatialOperationStyle = 18;
				}
				else if(pGroupJg->group_style==1)	//T����ϽǸ�
				{
					if(fabs(pCurJg->get_norm_x_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->desStartPos.spatialOperationStyle = 2;
					else
						pCurJg->desStartPos.spatialOperationStyle = 1;	//����Y֫��ʱʵ��Ӧ��������
				}
				else
					pCurJg->desStartPos.spatialOperationStyle=1;	//��������
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->desStartPos.spatialOperationStyle = 1;	//�ڵ�������ͶӰ
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("untreated mode!");
#else
				logerr.Log("δ����ʽ!");
#endif
			if(pEndFatherPart.IsHasPtr()&&!pEndFatherPart.IsEqualPtr(pCurJg)&&pEndFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(fabs(pCurJg->get_norm_x_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->desEndPos.spatialOperationStyle = 2;
				else
					pCurJg->desEndPos.spatialOperationStyle = 3;
				//��ᵣ����Ĭ����Ϊƽ�ƣ������Ǹ�������Ǹִ��������Ǹֽ����ϵ��´���
				//ʵ�ʴ˴�Ӧ�������趨����֫Ϊ��Ӷ�λ��׼֫��Ŀǰ��ͨ���򵥶�λ��������⡡wjh-2016.5.18
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED||	//ָ��X���������
				//	pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED||	//ָ��Y���������
				//	pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//ָ��Z���������
				//	pCurJg->desEndPos.spatialOperationStyle+=3;	//ͶӰ��ƽ��
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)	//ָ��X���������
				//	pCurJg->desEndPos.face_offset_norm.Set(1,0,0);
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)	//ָ��Y���������
				//	pCurJg->desEndPos.face_offset_norm.Set(0,1,0);
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//ָ��Z���������
				//	pCurJg->desEndPos.face_offset_norm.Set(0,0,1);
			}
			else if(pEndFatherPart.IsHasPtr()&&!pEndFatherPart.IsEqualPtr(pCurJg)&&pEndFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pEndFatherPart.LineAnglePointer();
				if(pCurJg==pEndFatherPart)
				{
					//pCurJg->desEndPos.datum_pos_style = 0;
					pCurJg->desEndPos.origin_pos_style = 0;
					pCurJg->desEndPos.spatialOperationStyle = 18;
				}
				else if(pGroupJg->group_style==1)	//T����ϽǸ�
				{
					if(fabs(pCurJg->get_norm_x_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->desEndPos.spatialOperationStyle = 2;
					else
						pCurJg->desEndPos.spatialOperationStyle = 1;
				}
				else
					pCurJg->desEndPos.spatialOperationStyle=1;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->desEndPos.spatialOperationStyle=1;	//�ڵ�������ͶӰ
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("untreated mode!");
#else
				logerr.Log("δ����ʽ!");
#endif
		}
		else //if(pCurJg->des_norm_y.bByOtherWing)
		{
			if(pStartFatherPart.IsHasPtr()&&!pStartFatherPart.IsEqualPtr(pCurJg)&&pStartFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(fabs(pCurJg->get_norm_y_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->desStartPos.spatialOperationStyle = 2;
				else
					pCurJg->desStartPos.spatialOperationStyle = 3;
				//��ᵣ����Ĭ����Ϊƽ�ƣ������Ǹ�������Ǹִ��������Ǹֽ����ϵ��´���
				//ʵ�ʴ˴�Ӧ�������趨����֫Ϊ��Ӷ�λ��׼֫��Ŀǰ��ͨ���򵥶�λ��������⡡wjh-2016.5.18
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED||	//ָ��X���������
				//	pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED||	//ָ��Y���������
				//	pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//ָ��Z���������
				//	pCurJg->desStartPos.spatialOperationStyle+=3;	//ͶӰ��ƽ��
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)	//ָ��X���������
				//	pCurJg->desStartPos.face_offset_norm.Set(1,0,0);
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)	//ָ��Y���������
				//	pCurJg->desStartPos.face_offset_norm.Set(0,1,0);
				//if( pCurJg->pStart->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//ָ��Z���������
				//	pCurJg->desStartPos.face_offset_norm.Set(0,0,1);
			}
			else if(pStartFatherPart.IsHasPtr()&&!pStartFatherPart.IsEqualPtr(pCurJg)&&pStartFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pStartFatherPart.LineAnglePointer();
				if(pGroupJg->group_style==1)		//T����ϽǸ�
				{
					if(fabs(pCurJg->get_norm_y_wing()*pStartFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->desStartPos.spatialOperationStyle = 2;
					else
						pCurJg->desStartPos.spatialOperationStyle = 1;
				}
				else
					pCurJg->desStartPos.spatialOperationStyle=1;
			}
			else if(pStartFatherPart.IsHasPtr()&&pStartFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->desStartPos.spatialOperationStyle = 1;	//�ڵ�������ͶӰ
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("untreated mode!");
#else
				logerr.Log("δ����ʽ!");
#endif
			if(pEndFatherPart.IsHasPtr()&&!pEndFatherPart.IsEqualPtr(pCurJg)&&pEndFatherPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				if(fabs(pCurJg->get_norm_y_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
					pCurJg->desEndPos.spatialOperationStyle = 2;
				else
					pCurJg->desEndPos.spatialOperationStyle = 3;
				//��ᵣ����Ĭ����Ϊƽ�ƣ������Ǹ�������Ǹִ��������Ǹֽ����ϵ��´���
				//ʵ�ʴ˴�Ӧ�������趨����֫Ϊ��Ӷ�λ��׼֫��Ŀǰ��ͨ���򵥶�λ��������⡡wjh-2016.5.18
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED||	//ָ��X���������
				//	pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED||	//ָ��Y���������
				//	pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//ָ��Z���������
				//	pCurJg->desEndPos.spatialOperationStyle+=3;	//ͶӰ��ƽ��
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED)	//ָ��X���������
				//	pCurJg->desEndPos.face_offset_norm.Set(1,0,0);
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED)	//ָ��Y���������
				//	pCurJg->desEndPos.face_offset_norm.Set(0,1,0);
				//if( pCurJg->pEnd->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED)		//ָ��Z���������
				//	pCurJg->desEndPos.face_offset_norm.Set(0,0,1);
			}
			else if(pEndFatherPart.IsHasPtr()&&!pEndFatherPart.IsEqualPtr(pCurJg)&&pEndFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pEndFatherPart.LineAnglePointer();
				if(pGroupJg->group_style==1)	//T����ϽǸ�
				{
					if(fabs(pCurJg->get_norm_y_wing()*pEndFatherPart.LineAnglePointer()->get_norm_x_wing())>0.707)
						pCurJg->desEndPos.spatialOperationStyle = 2;
					else
						pCurJg->desEndPos.spatialOperationStyle = 1;
				}
				else
					pCurJg->desEndPos.spatialOperationStyle=1;
			}
			else if(pEndFatherPart.IsHasPtr()&&pEndFatherPart->GetClassTypeId()==CLS_LINETUBE)
				pCurJg->desEndPos.spatialOperationStyle = 1;	//�ڵ�������ͶӰ
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("untreated mode");
#else
				logerr.Log("δ����ʽ");
#endif
		}
		if(bStartToBer&&pCurJg!=pStartFatherPart)
			pCurJg->desStartPos.spatialOperationStyle=1;
		if(bEndToBer&&pCurJg!=pEndFatherPart)
			pCurJg->desEndPos.spatialOperationStyle=1;
		if(bStartToBer)
			pCurJg->desStartPos.jgber_cal_style=0;
		if(bEndToBer)
			pCurJg->desEndPos.jgber_cal_style=0;
		//ͨ���򵥶�λ����ʶ��Ǹֵ����λ�ò�����wjh-2017.12.14
		BYTE ciStartBerCalStyle =pCurJg->desStartPos.jgber_cal_style;
		BYTE ciEndBerCalStyle	=pCurJg->desEndPos.jgber_cal_style;
		if(pCurJg->desStartPos.spatialOperationStyle==15)
			pCurJg->desStartPos.jgber_cal_style=2;
		if(pCurJg->desEndPos.spatialOperationStyle==15)
			pCurJg->desEndPos.jgber_cal_style=2;
		pCurJg->CalPosition();
		pCurJg->desStartPos.jgber_cal_style=ciStartBerCalStyle;
		pCurJg->desEndPos.jgber_cal_style  =ciEndBerCalStyle;
	}
	catch(char *sError)
	{
		CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
		ss.Format("error in calculating angle leg normal, %s",sError);
#else
		ss.Format("����Ǹ�֫���ߴ���%s",sError);
#endif
		AfxMessageBox(ss);
	}
	return TRUE;
}

//���ݵ�ǰ�˼�ʼ/�ն˽ڵ㸸�˼������ж�ʼ/�ն��Ƿ���Ҫƽ��
static BOOL IsNeedHoriOffset(CLDSNode *pNode)
{
	CLDSLinePart *pLinePart = (CLDSLinePart*)console.FromPartHandle(pNode->hFatherPart,CLS_LINEPART);
	if(pLinePart == NULL)
		return FALSE;
	if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE&&pLinePart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
		return FALSE;
	if(toupper(pLinePart->layer(0))=='S'&&toupper((pLinePart->layer(1))=='Z'))
		return TRUE;
	return FALSE;
}

bool InitHorizonCrossAnglePosition(CLDSLineAngle* pAngle)
{
	if(pAngle->pStart==NULL||pAngle->pEnd==NULL)
		return false;
	if(fabs(pAngle->pStart->Position('Z')-pAngle->pEnd->Position('Z'))>EPS)
		return false;
	if(pAngle->pStart->Layer(0)!='L'&&pAngle->pStart->Layer(0)!='S')
		return false;	//��㲻ʼ����ڵ�
	if(pAngle->pEnd->Layer(0)!='L'&&pAngle->pEnd->Layer(0)!='S')
		return false;	//�յ㲻ʼ����ڵ�
	if(pAngle->Layer(0)!='L'&&pAngle->Layer(0)!='S')
		return false;	//�յ㲻ʼ����ڵ�
	char cQuadStart=pAngle->pStart->layer(2)-'0';
	char cQuadEnd=pAngle->pEnd->layer(2)-'0';
	GEPOINT lenVec=pAngle->pEnd->Position(false)-pAngle->pStart->Position(false);
	normalize(lenVec);
	if(fabs(lenVec.z)>EPS2||fabs(lenVec.x)<=0.25||fabs(lenVec.y)<=0.25||
		((cQuadStart<0||cQuadStart>4)&&(cQuadEnd<0||cQuadEnd>4)))
		return false;	//���ǶԽǽ���б��
	CLDSLineAngle* startLinkAnglePtrArr[2];
	CLDSLineAngle* endLinkAnglePtrArr[2];
	startLinkAnglePtrArr[0]=startLinkAnglePtrArr[1]=endLinkAnglePtrArr[0]=endLinkAnglePtrArr[1]=NULL;
	BOOL pushed=Ta.Parts.push_stack();
	for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pLineAngle;
		pLineAngle=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
	{
		if(pLineAngle->pStart==NULL||pLineAngle->pEnd==NULL||pLineAngle==pAngle)
			continue;
		if(pLineAngle->Layer(0)!='L'&&pLineAngle->Layer(0)!='S')
			continue;
		if(pLineAngle->pStart==pAngle->pStart||pLineAngle->pEnd==pAngle->pStart)
		{
			if(fabs(pLineAngle->pStart->Position('Z')-pLineAngle->pEnd->Position('Z'))<EPS)
			{
				if(fabs(pLineAngle->pStart->Position('X')-pLineAngle->pEnd->Position('X'))<EPS)
					startLinkAnglePtrArr[1]=pLineAngle;
				else if(fabs(pLineAngle->pStart->Position('Y')-pLineAngle->pEnd->Position('Y'))<EPS)
					startLinkAnglePtrArr[0]=pLineAngle;
			}
		}
		if(pLineAngle->pStart==pAngle->pEnd||pLineAngle->pEnd==pAngle->pEnd)
		{
			if(fabs(pLineAngle->pStart->Position('Z')-pLineAngle->pEnd->Position('Z'))<EPS)
			{
				if(fabs(pLineAngle->pStart->Position('X')-pLineAngle->pEnd->Position('X'))<EPS)
					endLinkAnglePtrArr[1]=pLineAngle;
				else if(fabs(pLineAngle->pStart->Position('Y')-pLineAngle->pEnd->Position('Y'))<EPS)
					endLinkAnglePtrArr[0]=pLineAngle;
			}
		}
		if( startLinkAnglePtrArr[0]!=NULL&&startLinkAnglePtrArr[1]!=NULL&&
			endLinkAnglePtrArr[0]!=NULL&&endLinkAnglePtrArr[1]!=NULL)
			break;
	}
	Ta.Parts.pop_stack(pushed);
	if( startLinkAnglePtrArr[0]!=NULL&&startLinkAnglePtrArr[1]!=NULL)
	{
		GEPOINT vHoriLenVec0=startLinkAnglePtrArr[0]->pEnd->Position(false)-startLinkAnglePtrArr[0]->pStart->Position(false);
		GEPOINT vHoriLenVec1=startLinkAnglePtrArr[1]->pEnd->Position(false)-startLinkAnglePtrArr[1]->pStart->Position(false);
		if(startLinkAnglePtrArr[0]->pEnd==pAngle->pStart)
			vHoriLenVec0*=-1;
		if(startLinkAnglePtrArr[1]->pEnd==pAngle->pStart)
			vHoriLenVec1*=-1;
		if(lenVec*vHoriLenVec0>EPS&&lenVec*vHoriLenVec1>EPS)
		{
			pAngle->desStartPos.spatialOperationStyle=0;
			pAngle->desStartPos.datumPoint.datum_pos_style=3;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum1=startLinkAnglePtrArr[0]->handle;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum2=startLinkAnglePtrArr[1]->handle;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.near_vector[0]=0;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.near_vector[1]=0;
			pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.near_vector[2]=-1.0;
			strcpy(pAngle->desStartPos.datumPoint.des_para.AXIS_INTERS.norm_offset,"");
		}
		else
			return false;
	}
	if(endLinkAnglePtrArr[0]!=NULL&&endLinkAnglePtrArr[1]!=NULL)
	{
		GEPOINT vHoriLenVec0=endLinkAnglePtrArr[0]->pEnd->Position(false)-endLinkAnglePtrArr[0]->pStart->Position(false);
		GEPOINT vHoriLenVec1=endLinkAnglePtrArr[1]->pEnd->Position(false)-endLinkAnglePtrArr[1]->pStart->Position(false);
		if(endLinkAnglePtrArr[0]->pEnd==pAngle->pEnd)
			vHoriLenVec0*=-1;
		if(endLinkAnglePtrArr[1]->pEnd==pAngle->pEnd)
			vHoriLenVec1*=-1;
		if(lenVec*vHoriLenVec0<-EPS&&lenVec*vHoriLenVec1<-EPS)
		{
			pAngle->desEndPos.spatialOperationStyle=0;
			pAngle->desEndPos.datumPoint.datum_pos_style=3;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum1=endLinkAnglePtrArr[0]->handle;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum2=endLinkAnglePtrArr[1]->handle;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.near_vector[0]=0;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.near_vector[1]=0;
			pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.near_vector[2]=-1.0;
			strcpy(pAngle->desEndPos.datumPoint.des_para.AXIS_INTERS.norm_offset,"");
		}
		else
			return false;
	}
	return true;
}
//����������бʱ�û���ò�Ҫ��͸��ͼ�½��У�
//��ʱϵͳ����ȱʡΪ3����ǰ�濴��
CLDSLineAngle* CLDSView::FinishLinkAngle(CLDSNode *pStartNode, CLDSNode *pEndNode)
{
	CLDSLineAngle* pNowJg=NULL;
	f3dPoint view_norm,side_view_norm;
	double wing_wide,wing_thick;
	link_jg_dlg.m_nViewFlag = m_eViewFlag;
	//link_jg_dlg.m_nJgMaterial=0; //������һ������Ĳ��� wht 11-04-25
	link_jg_dlg.m_pStartNode=pStartNode;
	link_jg_dlg.m_pEndNode=pEndNode;
	//����Ĭ�϶κ�
	//��Ĭ�϶κ�Ӧ�빹�����һ�£��ʲ��ٸ���ʼĩ�˽ڵ���жκŸ�ֵ wjh-2015.4.15
	//if(pStartNode->iSeg.iSeg>0)
	//	link_jg_dlg.m_sSegNo = pStartNode->iSeg.ToString();
	//else if(pEndNode->iSeg.iSeg>0)
	//	link_jg_dlg.m_sSegNo = pEndNode->iSeg.ToString();
	//else 
	//	link_jg_dlg.m_sSegNo.Empty();
	//�Ƿ���Ҫƽ���жϲ�׼ȷ��ʱ���Զ��ж�  wht 10-01-14
	//link_jg_dlg.m_bStartHoriOffset=IsNeedHoriOffset(pStartNode);	//ʼ��ƽ��
	//link_jg_dlg.m_bEndHoriOffset=IsNeedHoriOffset(pEndNode);		//�ն�ƽ��
	f3dLine line(pStartNode->Position(),pEndNode->Position());
	line.pen.style=PS_SOLID;
	line.pen.width=2;
	line.pen.crColor=RGB(255,0,0);
	line.ID=0x01;
	g_pSolidDraw->NewLine(line);	//��ʱ��ʾ�Ǹֵ��ߣ������жϸ˼�ʼ�ն˷��� wht 16-10-30
	int nRetCode=link_jg_dlg.DoModal();
	g_pSolidDraw->DelEnt(0x01);		//ɾ���Ǹֵ���
	if(nRetCode!=IDOK)
		return NULL;

	Ta.BeginUndoListen();
	try
	{
		if(link_jg_dlg.m_bGroupJg)
		{
			pNowJg=(CLDSGroupLineAngle*)console.AppendPart(CLS_GROUPLINEANGLE);
			((CLDSGroupLineAngle*)pNowJg)->jg_space=link_jg_dlg.m_fFillThick;
			//link_jg_dlg.group_style��ĸ����ѷ����仯���ֲ�ֳ�wSectStyle��ciMasterIndex�������ԡ�wjh-2017.1.10
			((CLDSGroupLineAngle*)pNowJg)->wSectStyle=link_jg_dlg.wSectStyle;
			((CLDSGroupLineAngle*)pNowJg)->ciMasterIndex=link_jg_dlg.ciMasterIndex;
			((CLDSGroupLineAngle*)pNowJg)->AppendSonJg();
			CLDSGroupLineAngle *pGroupJg = (CLDSGroupLineAngle*)pNowJg;
			for(int i=0;i<4;i++)
			{
				CLDSLineAngle *pComJg=(CLDSLineAngle*)console.FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
				if(pComJg)
					pComJg->SetPartNo(link_jg_dlg.sPartNoArr[0][i]);
			}
		}
		else
			pNowJg = (CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
		if(link_jg_dlg.m_sOriginPartNo.GetLength()>MAX_CHAR_GUIGE_16)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "part's number input is too long, input failure";
#else
			throw "����Ĺ������̫��,����ʧ��";
#endif
		pNowJg->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
		pNowJg->SetPartNo(link_jg_dlg.m_sOriginPartNo.GetBuffer(0));
		pNowJg->cMaterial = CSteelMatLibrary::RecordAt(link_jg_dlg.m_nJgMaterial).cBriefMark;
		pNowJg->pStart = pStartNode;
		pNowJg->pEnd   = pEndNode;
		//����ʼ�ն˽ڵ�Ԥ�����������ýǸ�Ԥ����		wxc-17.1.6 
		if(pStartNode->ArcLiftHandle()>0X20 && pStartNode->ArcLiftHandle()==pEndNode->ArcLiftHandle())
			pNowJg->SetArcLift(pStartNode->ArcLiftHandle());
		//-------Wjh δ��� --------------
		switch(link_jg_dlg.m_iAngleLocation)
		{
		case 0:
			pNowJg->layer(0) = 'T';
			break;
		case 1:
			pNowJg->layer(0) = 'S';
			break;
		case 2:
			pNowJg->layer(0) = 'L';
		default:
			break;
		}
		switch(link_jg_dlg.m_iAngleGradeType)
		{
		case 0:
			pNowJg->layer(1) = 'Z';
			break;
		case 1:
			pNowJg->layer(1) = 'X';
			break;
		case 2:
			pNowJg->layer(1) = 'H';
			break;
		case 3:
			pNowJg->layer(1) = 'F';
			break;
		default:
			break;
		}
		
		pNowJg->layer(2) = theApp.env.layer[2];
		//�������ȸ˼���ĺ� wht 13-04-01
		if(pNowJg->Layer(0)=='L')
		{
			if(pStartNode->Layer(0)=='L')
				pNowJg->cfgword&=pStartNode->UnifiedCfgword();
			if(pEndNode->Layer(0)=='L')
				pNowJg->cfgword&=pEndNode->UnifiedCfgword();
			if(toupper(pStartNode->Layer(0))!='L'&&toupper(pEndNode->Layer(0))!='L')
			{	//ʼ�ն˽ڵ��Ϊ����ڵ�ʱ���ݼ�����߼��������������ĺ� wht 16-06-18
				CLDSModule *pActiveModule=Ta.GetActiveModule();
				char cPos=CalPoleQuad(pStartNode->Position(),pEndNode->Position());
				if(pActiveModule)
				{
					if(cPos>='1'&&cPos<='4')
						pNowJg->cfgword=CFGLEG((BYTE)pActiveModule->GetBodyNo(),(BYTE)pActiveModule->m_arrActiveQuadLegNo[cPos-'1']).ToCfgWord();
					else
						pNowJg->cfgword.SetBodyLegs(pActiveModule->GetBodyNo(),pActiveModule->m_dwLegCfgWord.GetBodyLegs(1));
				}
			}
		}
		//-------�ռ䲼��״̬ Wjh δ��� --------------
		pNowJg->LayStyle = link_jg_dlg.m_iJgStyle+1;
		pNowJg->MirLayStyle=link_jg_dlg.m_iMirLayStyle+1;
		restore_JG_guige(link_jg_dlg.m_sJgGuiGe.GetBuffer(),wing_wide,wing_thick);
		pNowJg->set_wing_wide(wing_wide);
		pNowJg->set_wing_thick(wing_thick);
		//------vvvvvvvvv-------��˨���--------------------
		strcpy(pNowJg->connectStart.grade,link_jg_dlg.m_sBoltGrade);
		strcpy(pNowJg->connectEnd.grade,link_jg_dlg.m_sBoltGrade);
		if(link_jg_dlg.m_bConnect1Wing)
			pNowJg->connectStart.m_iConnectWing=pNowJg->connectEnd.m_iConnectWing=1;//��֫����
		else
			pNowJg->connectStart.m_iConnectWing=pNowJg->connectEnd.m_iConnectWing=0;//˫֫����
		pNowJg->connectStart.d = atoi(link_jg_dlg.m_sStartLsGuiGe);
		pNowJg->connectEnd.d   = atoi(link_jg_dlg.m_sEndLsGuiGe);
		pNowJg->connectStart.rows = 1;//ȱʡ������˨
		pNowJg->connectStart.wnConnBoltN = link_jg_dlg.m_nStartLsNum;
		pNowJg->connectEnd.d = atoi(link_jg_dlg.m_sEndLsGuiGe);
		pNowJg->connectEnd.rows = 1;//ȱʡ������˨
		pNowJg->connectEnd.wnConnBoltN = link_jg_dlg.m_nEndLsNum;
		//------^^^^^^^^^-------��˨���--------------------
		pNowJg->SetStart(pNowJg->pStart->ActivePosition());
		pNowJg->SetEnd(pNowJg->pEnd->ActivePosition());
		InitHorizonCrossAnglePosition(pNowJg);
		InitLineAnglePosAndWingNorm(pNowJg, console.GetActiveView()->ucs.axis_z,link_jg_dlg.m_bStartToBer,link_jg_dlg.m_bEndToBer);//����Ǹָ�����ʸ��
		InitHorizonCrossAnglePosition(pNowJg);
		if(link_jg_dlg.m_bStartHoriOffset)	//ƽ��
		{
			pNowJg->desStartPos.ChangeToDatumType(2);
			CLDSLineAngle *pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pNowJg->pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pFatherAngle&&pFatherAngle->desStartPos.IsFaceOffset())
				pNowJg->desStartPos.face_offset_norm=pFatherAngle->desStartPos.face_offset_norm;
			else if(pFatherAngle&&pFatherAngle->desEndPos.IsFaceOffset())
				pNowJg->desStartPos.face_offset_norm=pFatherAngle->desEndPos.face_offset_norm;
		}
		if(link_jg_dlg.m_bEndHoriOffset)	//ƽ��
		{
			pNowJg->desEndPos.ChangeToDatumType(2);
			CLDSLineAngle *pFatherAngle=(CLDSLineAngle*)console.FromPartHandle(pNowJg->pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pFatherAngle&&pFatherAngle->desStartPos.IsFaceOffset())
				pNowJg->desEndPos.face_offset_norm=pFatherAngle->desStartPos.face_offset_norm;
			else if(pFatherAngle&&pFatherAngle->desEndPos.IsFaceOffset())
				pNowJg->desEndPos.face_offset_norm=pFatherAngle->desEndPos.face_offset_norm;
		}
		//����׼��
		JGZJ jgzj;
		getjgzj(jgzj,pNowJg->GetWidth());
		pNowJg->xWingXZhunJu=pNowJg->xWingYZhunJu=jgzj;		//���½Ǹ�X��Y֫׼��,�Ǹֿ�ȷ����仯��gδ�����仯ʱ�޷���ʱ����g1,g2 wht 16-10-30
		if(jgzj.g!=link_jg_dlg.m_nWingG)
		{
			pNowJg->m_bEnableTeG=TRUE;
			pNowJg->xWingXZhunJu.g = link_jg_dlg.m_nWingG;
			pNowJg->xWingYZhunJu.g = link_jg_dlg.m_nWingG;
		}
		pNowJg->ClearFlag();
		pNowJg->CalPosition();
		f3dPoint len_vec=pNowJg->pEnd->Position()-pNowJg->pStart->Position();
		normalize(len_vec);
		if((pNowJg->layer(0)=='L'||pNowJg->layer(0)=='S')&&pNowJg->layer(1)=='H'&&
		   (fabs(len_vec*f3dPoint(1,0,0))>EPS_COS||fabs(len_vec*f3dPoint(0,1,0))>EPS_COS))	//��һ����֤�Ǹ�Ϊˮƽ��� wht 16-10-17
		{	//�Զ��趨�����ĵ�ˮƽ֫����Ϊˮƽ��
			//����֫���߲�Ϊ(0,0,1)��(0,0,-1)ʱ�����Զ�����ˮƽ֫����(���������ڲ����֫���߼������) wht 16-10-30
			if(pNowJg->des_norm_x.bByOtherWing&&fabs(pNowJg->vyWingNorm.z)<EPS_COS2)
				pNowJg->set_norm_x_wing(f3dPoint(0,0,pNowJg->vxWingNorm.z>0?1:-1));
			else if(pNowJg->des_norm_y.bByOtherWing&&fabs(pNowJg->vxWingNorm.z)<EPS_COS2)
				pNowJg->set_norm_y_wing(f3dPoint(0,0,pNowJg->vyWingNorm.z>0?1:-1));
		}
		NewLinePart(pNowJg);
		g_pSolidDraw->SetEntSnapStatus(pNowJg->handle);
		
		if(link_jg_dlg.m_bGroupJg)
		{
			((CLDSGroupLineAngle*)pNowJg)->UpdateSonJgBasicInfo();
			((CLDSGroupLineAngle*)pNowJg)->UpdateSonJgPos();
			((CLDSGroupLineAngle*)pNowJg)->UpdateSonJgWingNorm();
		}
		MIRMSG mirmsg=link_jg_dlg.mirmsg;
		if(!MirTaAtom(pNowJg, mirmsg))
		{
			Ta.EndUndoListen();
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("the kind of symmetry in the working condition not supported, symmetry failure");
#else
			AfxMessageBox("�˹������������ĶԳƣ��Գ�ʧ��");
#endif
			return FALSE;
		}
		CSuperSmartPtr<CLDSPart>pMirPart;
		MIRMSG rotmirmsg = mirmsg;
		rotmirmsg.axis_flag=8;
		if(mirmsg.axis_flag&8)	//��ת�Գ�
		{
			pMirPart=pNowJg->GetMirPart(rotmirmsg);
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_jg_dlg.m_sOriginPartNo.GetBuffer(0));
			}
		}
		if(mirmsg.axis_flag&1)	//X��Գ�
		{
			pMirPart=pNowJg->GetMirPart(MIRMSG(1));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_jg_dlg.m_sMirXOriginPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&mirmsg.axis_flag&8)	//��ת�Գ�
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_jg_dlg.m_sMirXOriginPartNo.GetBuffer(0));
			
				}
			}
		}
		if(mirmsg.axis_flag&2)	//Y��Գ�
		{
			pMirPart=pNowJg->GetMirPart(MIRMSG(2));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
				pMirPart->SetPartNo(link_jg_dlg.m_sMirYOriginPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&mirmsg.axis_flag&8)	//��ת�Գ�
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_jg_dlg.m_sMirYOriginPartNo.GetBuffer(0));
				}
			}
		}
		if(mirmsg.axis_flag&4)	//Z��Գ�
		{
			pMirPart=pNowJg->GetMirPart(MIRMSG(3));
			if(pMirPart.IsHasPtr())
			{
				pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());	
				pMirPart->SetPartNo(link_jg_dlg.m_sMirDiagOriginPartNo.GetBuffer(0));
			}
			if(pMirPart.IsHasPtr()&&mirmsg.axis_flag&8)	//��ת�Գ�
			{
				pMirPart=pMirPart.LinePartPointer()->GetMirPart(rotmirmsg);
				if(pMirPart.IsHasPtr())
				{
					pMirPart->iSeg = SEGI(link_jg_dlg.m_sSegNo.GetBuffer());
					pMirPart->SetPartNo(link_jg_dlg.m_sMirDiagOriginPartNo.GetBuffer(0));
				}
			}
		}
	}
	catch(char* sErrorMsg)
	{
		Ta.EndUndoListen();
		if(pNowJg)
			console.DeletePart(pNowJg->handle);
		AfxMessageBox(sErrorMsg);
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	Invalidate(FALSE);
	return pNowJg;
}
//���ڵ��Ǹ�ת��Ϊ���°ڷŷ�ʽ�ĶԽ���ϽǸ֣�����ӿ�������
static BOOL IsUseNewGroupStyle(int n,long* id_arr)
{
	if(n<4)
		return FALSE;
	CHashList<long> partHashList;
	for(int i=0;i<n;i++)
	{	
		CLDSPart* pPart = console.FromPartHandle(id_arr[i]);
		if(pPart==NULL)
			return FALSE;
		if(pPart->GetClassTypeId()!=CLS_LINEANGLE)
			return FALSE;
		if(pPart->Layer(0)=='T' || pPart->Layer(1)!='Z')
			return FALSE;
		if(pPart->Layer(2)<'1' || pPart->Layer(2)>'4')	
			return FALSE;
		if(pPart->relativeObjs.GetNodeNum()!=3)
			return FALSE;
		partHashList.SetValue(id_arr[i],id_arr[i]);
	}
	for(int i=0;i<n;i++)
	{
		CLDSPart* pPart = console.FromPartHandle(id_arr[i]);
		for(RELATIVE_OBJECT *pMirObj=pPart->relativeObjs.GetFirst();pMirObj;pMirObj=pPart->relativeObjs.GetNext())
		{
			long* phObj=partHashList.GetValue(pMirObj->hObj);
			if(phObj==NULL)
				return FALSE;
		}
	}
	return TRUE;
}
void CLDSView::OnConvertToGroupJg() 
{
	m_nPrevCommandID=ID_CONVERT_TO_GROUP_JG;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat converting into combined angle";
#else
	m_sPrevCommandName="�ظ�ת��Ϊ��ϽǸ�";
#endif 
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	PARTSET partSet;
	for(int i=0;i<n;i++)
	{
		CLDSLineAngle* pLineAngle = (CLDSLineAngle*)console.FromPartHandle(id_arr[i],CLS_GROUPLINEANGLE,CLS_LINEANGLE);
		if(pLineAngle&&pLineAngle->group_father_jg_h<=0)
			partSet.append(pLineAngle);
	}
	if(partSet.GetNodeNum()<=0)
	{
		AfxMessageBox("δѡ�д�ת���Ǹ֣�");
		return;
	}
	//������������˵����ԭ���ĶԻ����ֱ�۷��㡡wjh-2016.12.17
	CGroupJgStyleDlg group_style_dlg;
	if(group_style_dlg.DoModal()!=IDOK)
		return;
	SmartPartPtr pSmartAngle;
	//�ж��Ƿ�ѡ�нǸ�ԭ��������ϽǸ֣��ҽ�������δ�����仯
	bool hasmodified=false;
	for(pSmartAngle=partSet.GetFirst();pSmartAngle.IsHasPtr()&&!hasmodified;pSmartAngle=partSet.GetNext())
	{
		if(pSmartAngle->GetClassTypeId()==CLS_LINEANGLE)
			hasmodified=true;
		else if(pSmartAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			if( pSmartAngle.pGroupAngle->wSectStyle!=group_style_dlg.wSectStyle||
				pSmartAngle.pGroupAngle->ciMasterIndex!=group_style_dlg.ciMasterIndex||
				pSmartAngle.pGroupAngle->jg_space!=group_style_dlg.m_fFillThick)
				hasmodified=true;
		}
	}
	if(!hasmodified)
	{
		AfxMessageBox("ѡ����ϽǸֽ�������δ�����κα仯!");
	}
	//int group_style=0,layout_style=0,nSpace=12;
	int group_style=group_style_dlg.wSectStyle;
	int layout_style=group_style_dlg.ciMasterIndex;
	int nSpace=ftoi(group_style_dlg.m_fFillThick);
	//
	CUndoOperObject undo(&Ta);
	int nHits=0;
	for(pSmartAngle=partSet.GetFirst();pSmartAngle.IsHasPtr();pSmartAngle=partSet.GetNext())
	{
		CLDSGroupLineAngle *pGroupJg=NULL;
		CSuperSmartPtr<CLDSPart> pPart=pSmartAngle;
		BYTE iWorkWing=0;
		CMultiOffsetPos desStartPos,desEndPos;
		//ciShiftAngleLineAxisToBerX(Y)=trueʱ����������ϵ��ӽǸ־�Ӧ��ԭ���Ľ���ָ�����߸�Ϊ����������ϽǸֵ�����
		char ciShiftAngleLineAxis_BerX=(pPart->GetClassTypeId()==CLS_LINEANGLE);
		char ciShiftAngleLineAxis_BerY=(pPart->GetClassTypeId()==CLS_LINEANGLE);
		if(group_style_dlg.wSectStyle==1)	//T����ϽǸ�
		{
			if(pSmartAngle->GetClassTypeId()==CLS_GROUPLINEANGLE&&pSmartAngle.pGroupAngle->wSectStyle==1)
			{	//ԭ����ΪT����ϽǸ�
				if(pSmartAngle.pGroupAngle->ciMasterIndex==0)
				{
					ciShiftAngleLineAxis_BerX=1;
					ciShiftAngleLineAxis_BerY=2;
				}
				else
				{
					ciShiftAngleLineAxis_BerX=2;
					ciShiftAngleLineAxis_BerY=1;
				}
			}
			else if(group_style_dlg.ciMasterIndex==0)
				ciShiftAngleLineAxis_BerX=false;
			else if(group_style_dlg.ciMasterIndex==1)
				ciShiftAngleLineAxis_BerY=false;
		}
		if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			//����������ԭ��Ǹ�����ת��ʱ��������¾�� wjh-2012.9.17
			//1.Эͬʱ��Ҫ���·�����ϽǸֵĹ�������������Эͬ���ĳ���ͬ�����ͬ�������͵Ĵ���
			//2.Undo��������Ҫͨ�������ʶ��¼��ʷ�������ͬһ�������������Ǹֺ���ϽǸ�
			pGroupJg=(CLDSGroupLineAngle*)console.AppendPart(CLS_GROUPLINEANGLE);
			Ta.ReplaceReferenceHandle(CHandleReplacer(pPart->handle,pGroupJg->handle));
			pGroupJg->CopyProperty(pPart);
			pGroupJg->SetStartOdd(pPart.LineAnglePointer()->startOdd());
			pGroupJg->SetEndOdd(pPart.LineAnglePointer()->endOdd());
			pGroupJg->pStart=pPart.LineAnglePointer()->pStart;
			pGroupJg->pEnd=pPart.LineAnglePointer()->pEnd;
			pGroupJg->SetStart(pPart.LineAnglePointer()->pStart->Position(true));
			pGroupJg->SetEnd(pPart.LineAnglePointer()->pEnd->Position(true));
			pGroupJg->set_norm_x_wing(pPart.LineAnglePointer()->get_norm_x_wing(),FALSE);
			pGroupJg->set_norm_y_wing(pPart.LineAnglePointer()->get_norm_y_wing(),FALSE);
			pGroupJg->des_norm_x=pPart.LineAnglePointer()->des_norm_x;
			pGroupJg->des_norm_y=pPart.LineAnglePointer()->des_norm_y;
			desStartPos=pPart.LineAnglePointer()->desStartPos;
			desEndPos  =pPart.LineAnglePointer()->desEndPos;
			iWorkWing  =pPart.LineAnglePointer()->GetStartWorkWing();
			console.DispPartSet.DeleteNode(pPart.LineAnglePointer()->handle);
			console.DeletePart(pPart.LineAnglePointer()->handle);
			console.DispPartSet.append(pGroupJg);
			g_pSolidDraw->DelEnt(pPart->handle);	//����ʾ�������Ƴ�ԭ���Ǹ�
		}
		else if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			pGroupJg=pPart.pGroupAngle;
		else
			continue;
		pGroupJg->jg_space=nSpace;
		pGroupJg->group_style=group_style;
		if(pGroupJg->group_style==0)
		{
			pGroupJg->size_cSubClassType='D';
			if(layout_style==1)
			{	//ͨ���趨֫���������Ϣ�������Խ���ϽǸֵ�֫���߷��򣨲�����ֱ��ָ��֫���ߣ� wxc-2016.11.1
				f3dPoint x_wing_norm,y_wing_norm,wing_norm;
				x_wing_norm=pGroupJg->get_norm_x_wing();
				y_wing_norm=pGroupJg->get_norm_y_wing();
				DESIGN_JGWING_NORM tem_des_norm=pGroupJg->des_norm_y; 
				pGroupJg->des_norm_y=pGroupJg->des_norm_x;
				pGroupJg->des_norm_x=tem_des_norm;
				if(pGroupJg->Layer(2)=='2' || pGroupJg->Layer(2)=='3')
				{	//ֱ��ָ��֫���߸ı�spec_norm.vectֵ�������ٽ��з�ת����
					//pGroupJg->set_norm_x_wing(y_wing_norm);
					//pGroupJg->set_norm_y_wing(-x_wing_norm);
					pGroupJg->des_norm_y.near_norm*=-1.0;
					pGroupJg->des_norm_y.spec_norm.ReverseDirection();
				}
				else
				{	//ֱ��ָ��֫���߸ı�spec_norm.vectֵ�������ٽ��з�ת����
					//pGroupJg->set_norm_x_wing(-y_wing_norm);
					//pGroupJg->set_norm_y_wing( x_wing_norm);
					pGroupJg->des_norm_x.near_norm*=-1.0;
					pGroupJg->des_norm_x.spec_norm.ReverseDirection();
				}
			}
		}
		else if(pGroupJg->group_style==1)
		{
			pGroupJg->size_cSubClassType='T';
			if(layout_style==0)	//X֫����
				pGroupJg->ciMasterIndex=0;
			else				//Y֫����
				pGroupJg->ciMasterIndex=1;
		}
		else if(pGroupJg->group_style==2)
			pGroupJg->size_cSubClassType='X';
		pGroupJg->AppendSonJg();
		if(pGroupJg->group_style==1)
		{	//T����ϽǸ�Ӧ�̳�ԭ���Ǹֵ�λ�ô�ӹ�ϵ wjh-2015.6.29
			pGroupJg->desStartPos=desStartPos;
			pGroupJg->desEndPos=desEndPos;
		}
		//����㶨λ��ʽ����Ϊ��������λ���򵥶�λ�����ж϶�λ��Ϣ�������ⲿ�����趨
		pGroupJg->desStartPos.jgber_cal_style=0;
		pGroupJg->desEndPos.jgber_cal_style=0;
		if(pGroupJg->group_style!=1||(pGroupJg->group_style==1 && pGroupJg->ciMasterIndex!=iWorkWing))
		{	//��T����ϽǸֻ�T����ϽǸ�����֫�ǹ���֫ʱ�������֫����ƫ����Ϣ   wxc-2016.10.21
			pGroupJg->desStartPos.wing_x_offset.gStyle=pGroupJg->desStartPos.wing_y_offset.gStyle=4;
			pGroupJg->desStartPos.wing_x_offset.offsetDist=pGroupJg->desStartPos.wing_y_offset.offsetDist=0;
			pGroupJg->desEndPos.wing_x_offset.gStyle=pGroupJg->desEndPos.wing_y_offset.gStyle=4;
			pGroupJg->desEndPos.wing_x_offset.offsetDist=pGroupJg->desEndPos.wing_y_offset.offsetDist=0;
		}
		pGroupJg->CalPosition();
		//�����ӹ�����λ��Ϣ
		pGroupJg->UpdateSonJgBasicInfo();
		pGroupJg->UpdateSonJgOddment();
		pGroupJg->UpdateSonJgWingNorm();
		pGroupJg->UpdateSonJgPos();
		if(pGroupJg->pSolidBody)
			pGroupJg->pSolidBody->Empty();
		//������ʾ��������ϽǸּ��ӽǸֵ�ͼ����ʾ��Ϣ
		g_pSolidDraw->NewSolidPart(pGroupJg->GetSolidPartObject());
		for (int j = 0; j < 4; j++)
		{
			CLDSLineAngle* pSonAngle = NULL;
			if(pGroupJg->son_jg_h[j]>0)
				pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pGroupJg->son_jg_h[j], CLS_LINEANGLE);
			if (pSonAngle)
			{
				pSonAngle->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pSonAngle->GetSolidPartObject());
			}
		}
		pGroupJg->SetModified();
		//���´�������ϵ��������߽Ǹ�
		if(ciShiftAngleLineAxis_BerX+ciShiftAngleLineAxis_BerY>0)
		{
			SCOPE_STRU scope;
			scope.VerifyVertex(pGroupJg->pStart->xOriginalPos);
			scope.VerifyVertex(pGroupJg->pEnd->xOriginalPos);
			CLDSNode* pNode;
			CHashSet<CLDSNode*> hashResideNodes;
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(!pGroupJg->ModuleTogetherWith(pNode))
					continue;
				if(!scope.IsIncludePoint(pNode->xOriginalPos))
					continue;
				if(pNode->hFatherPart==pGroupJg->handle)
					hashResideNodes.SetValue(pNode->handle,pNode);
			}
			SmartRodPtr pSmartRayAngle;
			for(pSmartRayAngle=Ta.EnumRodFirst();pSmartRayAngle.IsHasPtr();pSmartRayAngle=Ta.EnumRodNext())
			{
				if(!pSmartRayAngle->IsAngle())
					continue;
				long hStartDatumAngle=pSmartRayAngle.pAngle->desStartPos.datum_jg_h;
				long hEndDatumAngle=pSmartRayAngle.pAngle->desEndPos.datum_jg_h;
				if(hStartDatumAngle==0&&pSmartRayAngle->pStart)
					hStartDatumAngle=pSmartRayAngle->pStart->hFatherPart;
				if(hEndDatumAngle==0&&pSmartRayAngle->pEnd)
					hEndDatumAngle=pSmartRayAngle->pEnd->hFatherPart;
				BYTE ciOldStartSpatialStyle=pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle;
				BYTE ciOldEndSpatialStyle=pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle;
				if( hStartDatumAngle==pGroupJg->handle)
				{
					if(ciShiftAngleLineAxis_BerX==1&&pSmartRayAngle.pAngle->desStartPos.IsInDatumJgWingX()&&pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle-=1;	//X֫���߱�Ϊ����
					else if(ciShiftAngleLineAxis_BerY==1&&pSmartRayAngle.pAngle->desStartPos.IsInDatumJgWingY()&&pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle-=2;	//Y֫���߱�Ϊ����
					else if(ciShiftAngleLineAxis_BerX==2&&pSmartRayAngle.pAngle->desStartPos.IsInDatumJgBer()&&pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle+=1;	//���߱�ΪX֫����
					else if(ciShiftAngleLineAxis_BerY==2&&pSmartRayAngle.pAngle->desStartPos.IsInDatumJgBer()&&pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle+=2;	//���߱�ΪY֫����
				}
				if(hEndDatumAngle==pGroupJg->handle)
				{
					if(ciShiftAngleLineAxis_BerX==1&&pSmartRayAngle.pAngle->desEndPos.IsInDatumJgWingX()&&pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle-=1;	//X֫���߱�Ϊ����
					else if(ciShiftAngleLineAxis_BerY==1&&pSmartRayAngle.pAngle->desEndPos.IsInDatumJgWingY()&&pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle-=2;	//Y֫���߱�Ϊ����
					else if(ciShiftAngleLineAxis_BerX==2&&pSmartRayAngle.pAngle->desEndPos.IsInDatumJgBer()&&pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle+=1;	//���߱�ΪX֫����
					else if(ciShiftAngleLineAxis_BerY==2&&pSmartRayAngle.pAngle->desEndPos.IsInDatumJgBer()&&pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle<13)
						pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle+=2;	//���߱�ΪY֫����
				}
				if( ciOldStartSpatialStyle!=pSmartRayAngle.pAngle->desStartPos.spatialOperationStyle||
					ciOldEndSpatialStyle!=pSmartRayAngle.pAngle->desEndPos.spatialOperationStyle)
					pSmartRayAngle->CalPosition(true);
			}
		}
		nHits++;
	}
	Invalidate();
	UpdatePropertyPage();	//����ˢ�����������������ת��Ϊ�ֹܺ�selectObjs�еĶ����ѱ����
	CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
	ss.Format("succeeded in converting %d angles into combined angle!",nHits);
#else
	ss.Format("�ѳɹ�ת��%d���Ǹ�Ϊ��ϽǸ�!",nHits);
#endif
	MessageBox(ss);
}

void CLDSView::OnConvertToSingleJg() 
{
	m_nPrevCommandID=ID_CONVERT_TO_SINGLE_JG;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat converting into single angle";
#else
	m_sPrevCommandName="�ظ�ת��Ϊ���Ǹ�";
#endif
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	int nHits=0,nTubes=0;
	CUndoOperObject undo(&Ta);
	for(int i=0;i<n;i++)
	{
		CLDSLineAngle *pJg=NULL;
		CLDSLineTube *pTube=NULL;
		CLDSGroupLineAngle* pGroupJg = (CLDSGroupLineAngle*)console.FromPartHandle(id_arr[i],CLS_GROUPLINEANGLE);
		if(pGroupJg)
		{
			//����������ԭ��Ǹ�����ת��ʱ��������¾�� wjh-2012.9.17
			//1.Эͬʱ��Ҫ���·�����ϽǸֵĹ�������������Эͬ���ĳ���ͬ�����ͬ�������͵Ĵ���
			//2.Undo��������Ҫͨ�������ʶ��¼��ʷ�������ͬһ�������������Ǹֺ���ϽǸ�
			pJg=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
			Ta.ReplaceReferenceHandle(CHandleReplacer(pGroupJg->handle,pJg->handle));
			pJg->CopyProperty((CLDSLineAngle*)pGroupJg);
			pJg->pStart=pGroupJg->pStart;
			pJg->pEnd=pGroupJg->pEnd;
			pJg->set_norm_x_wing(pGroupJg->get_norm_x_wing());
			pJg->set_norm_y_wing(pGroupJg->get_norm_y_wing());
			pJg->des_norm_x=pGroupJg->des_norm_x;
			pJg->des_norm_y=pGroupJg->des_norm_y;
			pJg->desStartPos=pGroupJg->desStartPos;
			pJg->desEndPos=pGroupJg->desEndPos;
			pJg->SetStart(pGroupJg->Start());
			pJg->SetEnd(pGroupJg->End());
			pJg->cal_x_wing_norm();
			pJg->cal_y_wing_norm();
			nHits++;
			for(int j=0;j<4;j++)
			{
				console.DispPartSet.DeleteNode(pGroupJg->son_jg_h[j]);
				console.DeletePart(pGroupJg->son_jg_h[j]);
			}
			//������ҵҲ��Ҫ���·���Ǹ־�� wht 09-10-13
			//if(theApp.m_bCooperativeWork)	
			{	//Эͬʱ��Ҫ���·���Ǹ־��
				g_pSolidDraw->DelEnt(pGroupJg->handle);
				console.DispPartSet.DeleteNode(pGroupJg->handle);
				console.DeletePart(pGroupJg->handle);
				console.DispPartSet.append(pJg);
				g_pSolidDraw->NewSolidPart(pJg->GetSolidPartObject());
			}
		}
		else if((pTube=(CLDSLineTube*)console.FromPartHandle(id_arr[i],CLS_LINETUBE))!=NULL)
		{
			if(ConvertToAngle(pTube)!=NULL)
				nTubes++;
		}
	}
	UpdatePropertyPage();	//����ˢ�����������������ת��Ϊ�ֹܺ�selectObjs�еĶ����ѱ����
	CString ss;
	if(nTubes==0)
#ifdef AFX_TARG_ENU_ENGLISH
		ss.Format("succeeded in converting %d combined angles into single angle!",nHits);
	else if(nHits==0)
		ss.Format("succeeded in converting %d tube into single angle!",nTubes);
	else
		ss.Format("succeeded in converting %d combined angles into single angle!",nHits,nTubes);
#else
		ss.Format("�ѳɹ�ת��%d����ϽǸ�Ϊ���Ǹ�!",nHits);
	else if(nHits==0)
		ss.Format("�ѳɹ�ת��%d���ֹ�Ϊ���Ǹ�!",nTubes);
	else
		ss.Format("�ѳɹ�ת��%d����ϽǸּ�%d���ֹ�Ϊ���Ǹ�!",nHits,nTubes);
#endif
	MessageBox(ss);
}

#else
void CLDSView::OnConvertToGroupJg(){;}
void CLDSView::OnConvertToSingleJg(){;}
#endif
void CLDSView::OnConvertToTube() 
{
#ifdef __COMPLEX_PART_INC_
	m_nPrevCommandID=ID_CONVERT_TO_TUBE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat converting into tube";
#else
	m_sPrevCommandName="�ظ�ת��Ϊ�ֹ�";
#endif
	long *id_arr=NULL;
	long n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	int nHits=0;
	CUndoOperObject undo(&Ta,false);
	for(int i=0;i<n;i++)
	{
		CLDSLineTube *pTube=NULL;
		CSuperSmartPtr<CLDSPart> pPart = console.FromPartHandle(id_arr[i]);
		if(pPart.IsNULL()||!pPart->IsLinePart())
			continue;
		if(pPart.LinePartPointer()->pStart==NULL||pPart.LinePartPointer()->pEnd==NULL)
			continue;
		if(pPart->GetClassTypeId()!=CLS_LINETUBE)
		{
			//����������ԭ��Ǹ�����ת��ʱ��������¾�� wjh-2012.9.17
			//1.Эͬʱ��Ҫ���·�����ϽǸֵĹ�������������Эͬ���ĳ���ͬ�����ͬ�������͵Ĵ���
			//2.Undo��������Ҫͨ�������ʶ��¼��ʷ�������ͬһ�������������Ǹֺ���ϽǸ�
			pTube=(CLDSLineTube*)console.AppendPart(CLS_LINETUBE);
			Ta.ReplaceReferenceHandle(CHandleReplacer(pPart->handle,pTube->handle));
			pTube->CopyProperty(pPart);
			pTube->pStart=pPart.LinePartPointer()->pStart;
			pTube->pEnd=pPart.LinePartPointer()->pEnd;
			pTube->SetStartOdd(pPart.LinePartPointer()->startOdd());
			pTube->SetEndOdd(pPart.LinePartPointer()->endOdd());
			pTube->SetStart(pTube->pStart->Position(true));
			pTube->SetEnd(pTube->pEnd->Position(true));
			if(pPart.LinePartPointer()->pStart->hFatherPart==pPart->handle)
				pTube->desStartPos.endPosType=0;
			else
				pTube->desStartPos.endPosType=1;
			if(pPart.LinePartPointer()->pEnd->hFatherPart==pPart->handle)
				pTube->desEndPos.endPosType=0;
			else
				pTube->desEndPos.endPosType=1;
			console.DispPartSet.DeleteNode(pPart->handle);
			console.DeletePart(pPart->handle);
			pTube->CalPosition();
			pTube->SetModified();
			nHits++;
		}
	}
	Invalidate();
	UpdatePropertyPage();	//����ˢ�����������������ת��Ϊ�ֹܺ�selectObjs�еĶ����ѱ����
	CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
	ss.Format("succeeded in converting %d rods into tube!",nHits);
#else
	ss.Format("�ѳɹ�ת��%d���˼�Ϊ�ֹ�!",nHits);
#endif
	MessageBox(ss);
#endif
}
bool CorrectSingleAngleLayout(CLDSLineAngle* pJg,bool updatePartSolid=true);	//defined in LDSView.cpp
CLDSLineAngle* CLDSView::ConvertToAngle(CLDSLineTube *pTube) 
{
#ifdef __COMPLEX_PART_INC_
	if(pTube==NULL||pTube->pStart==NULL||pTube->pEnd==NULL)
		return NULL;
	//����������ԭ��Ǹ�����ת��ʱ��������¾�� wjh-2012.9.17
	//1.Эͬʱ��Ҫ���·�����ϽǸֵĹ�������������Эͬ���ĳ���ͬ�����ͬ�������͵Ĵ���
	//2.Undo��������Ҫͨ�������ʶ��¼��ʷ�������ͬһ�������������Ǹֺ���ϽǸ�
	CLDSLineAngle* pAngle=(CLDSLineAngle*)console.AppendPart(CLS_LINEANGLE);
	Ta.ReplaceReferenceHandle(CHandleReplacer(pTube->handle,pAngle->handle));
	pAngle->CopyProperty(pTube);
	pAngle->pStart=pTube->pStart;
	pAngle->pEnd=pTube->pEnd;
	pAngle->SetStartOdd(pTube->startOdd());
	pAngle->SetEndOdd(pTube->endOdd());
	pAngle->SetStart(pAngle->pStart->Position(true));
	pAngle->SetEnd(pAngle->pEnd->Position(true));
	//
	pAngle->desStartPos.spatialOperationStyle=pAngle->desEndPos.spatialOperationStyle=18;
	CorrectSingleAngleLayout(pAngle);
	console.DispPartSet.DeleteNode(pTube->handle);
	console.DeletePart(pTube->handle);
	pAngle->CalPosition();
	UpdatePropertyPage();	//����ˢ�����������������ת��Ϊ�ֹܺ�selectObjs�еĶ����ѱ����
	return pAngle;
#else
	return NULL;
#endif
}

//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "dlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CLDSView::OnCal1jgNorm() 
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	m_nPrevCommandID=ID_CAL_1JG_NORM;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat counting angle's leg direction";
#else 
	m_sPrevCommandName="�ظ�����Ǹ�֫����";
#endif
	m_curTask = TASK_CAL_1JG_NORM;
	g_pSolidSet->SetOperType(OPER_OSNAP);
	g_pSolidSnap->SetSnapType(SNAP_LINE);
	//�л���������ʾ״̬
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
	m_nObjectSnapedCounts=0;
	m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
	g_pPromptMsg->SetMsg("Please choose to compute leg direction of angle firstly,then select another reference angle!");
#else 
	g_pPromptMsg->SetMsg("������ѡ��Ҫ����֫����ĽǸ�,Ȼ����ѡ��һ������������սǸ�!");
#endif
#endif
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
static CConfirmJgStyleDlg confirm_jgstyle_dlg;
BOOL CalLineAngleWingNorm(CLDSLineAngle* pCurJg,CLDSLinePart *pByLinePart,f3dPoint view_norm,BOOL bUpdateOtherWing,BOOL bSpecStyle/*=TRUE*/,BOOL bPromptError/*=TRUE*/)
{
	f3dPoint vec,aid_vec,norm,direct,view_vec,vertical_view;
	//���´���Ϊ��ǰ�ĽǸ�֫���߼�����룬һֱ���ȶ����ֻ�Ϊһ����CalFaceNorm���������� WJH-2004/07/01
	/*Sub_Pnt(vec,pCurJg->End(),pCurJg->Start());
	if(vec.mod()<EPS)
		throw "��ǰ�Ǹ�ʼ�ն˵��غ�,���ݴ���!";

	//Sub_Pnt(aid_vec,pByJg->pEnd->pos,pByJg->pStart->pos);
	//Ϊ�˴���Ʊ����������۽����и�Ϊ������� WJH--01.07.28
	Sub_Pnt(aid_vec,pByJg->End(),pByJg->Start());
	if(aid_vec.mod()<EPS)
		throw CString("��׼�Ǹ�ʼ�ն˽ڵ��غ�,���ݴ���!");
	//�������Ǹֹ�������֫�ĵķ�ʸ
	normalize(vec);
	normalize(aid_vec);
	norm = cross_prod(aid_vec,vec);
	normalize(view_norm);
	normalize(norm);
	if(norm.mod()<EPS)
	{
		aid_vec = pByJg->End()-pCurJg->End();
		normalize(aid_vec);
		norm = cross_prod(aid_vec,vec);
		normalize(view_norm);
		normalize(norm);
		if(norm.mod()<EPS)
			throw CString("��ǰ�Ǹ����׼�Ǹ��غ�,���ݴ���!");
	}*/
	try{
		norm=CalFaceNorm(pCurJg->Start(),pCurJg->End(),pByLinePart->Start(),pByLinePart->End());
		if(norm.mod()<EPS)
		{
			if(bPromptError)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The current angle and datum angle are coincidence,data is error!");
#else 
				AfxMessageBox("��ǰ�Ǹ����׼�Ǹ��غ�,���ݴ���!");
#endif
			else
				return FALSE;
		}
		
		if(bSpecStyle)
		{
			if(confirm_jgstyle_dlg.DoModal()!=IDOK)
				return FALSE;
		}
		else
			confirm_jgstyle_dlg.m_nJgStyle=pCurJg->LayStyle-1;
		pCurJg->ClearFlag();
		//����ǹ�����֫�ķ�ʸ
		switch(confirm_jgstyle_dlg.m_nJgStyle+1)
		{
		case 1:
		case 3:
			pCurJg->des_norm_x.hViceJg = pByLinePart->handle;
			if(confirm_jgstyle_dlg.m_nJgStyle+1==1)
			{	//����
				pCurJg->des_norm_x.near_norm = -view_norm;	//��֫���߽ӽ��Ļ�׼����
				if(norm*view_norm<0)
					pCurJg->set_norm_x_wing(norm,pCurJg->des_norm_x.bSpecific);
				else
					pCurJg->set_norm_x_wing(-norm,pCurJg->des_norm_x.bSpecific);
			}
			else
			{	//����
				pCurJg->des_norm_x.near_norm = view_norm;	//��֫���߽ӽ��Ļ�׼����
				if(norm*view_norm>0)
					pCurJg->set_norm_x_wing(norm,pCurJg->des_norm_x.bSpecific);
				else
					pCurJg->set_norm_x_wing(-norm,pCurJg->des_norm_x.bSpecific);
			}
			pCurJg->des_norm_x.bSpecific = FALSE;
			pCurJg->des_norm_x.bByOtherWing = FALSE;
			if(bUpdateOtherWing)	//��һ֫�뵱ǰ����֫һ�´���
			{
				pCurJg->des_norm_y.bByOtherWing = TRUE;
				pCurJg->des_norm_y.bSpecific = FALSE;
			}
			else
			{	//���㿪�Ͻ�ʱ�ǹ���֫���߷���������һ֫�仯 wht 09-08-28
				pCurJg->des_norm_y.bByOtherWing=FALSE;
				pCurJg->des_norm_y.bSpecific=TRUE;
			}
			if(pCurJg->des_norm_y.bByOtherWing)		//Y֫������X֫����Ϊ��׼
				pCurJg->cal_y_wing_norm();
			break;
		case 2:
		case 4:
		default:
			pCurJg->des_norm_y.hViceJg = pByLinePart->handle;
			if(confirm_jgstyle_dlg.m_nJgStyle+1==4)
			{	//����
				pCurJg->des_norm_y.near_norm = -view_norm;	//��֫���߽ӽ��Ļ�׼����
				if(norm*view_norm<0)
					pCurJg->set_norm_y_wing(norm);
				else
					pCurJg->set_norm_y_wing(-norm);
			}
			else
			{	//����
				pCurJg->des_norm_y.near_norm = view_norm;	//��֫���߽ӽ��Ļ�׼����
				if(norm*view_norm>0)
					pCurJg->set_norm_y_wing(norm);
				else
					pCurJg->set_norm_y_wing(-norm);
			}
			if(norm*view_norm>0)
				pCurJg->set_norm_y_wing(norm);
			else
				pCurJg->set_norm_y_wing(-norm);
			pCurJg->des_norm_y.bSpecific = FALSE;
			pCurJg->des_norm_y.bByOtherWing = FALSE;
			if(bUpdateOtherWing)	//��һ֫�뵱ǰ����֫һ�´���
			{
				pCurJg->des_norm_x.bByOtherWing = TRUE;
				pCurJg->des_norm_x.bSpecific = FALSE;
			}
			else
			{	//���㿪�Ͻ�ʱ�ǹ���֫���߷���������һ֫�仯	wht 09-08-28
				pCurJg->des_norm_x.bSpecific=TRUE;
				pCurJg->des_norm_x.bByOtherWing=FALSE;
			}
			if(pCurJg->des_norm_x.bByOtherWing)		//X֫������Y֫����Ϊ��׼
				pCurJg->cal_x_wing_norm();
			break;
		}
		pCurJg->LayStyle=confirm_jgstyle_dlg.m_nJgStyle+1;
	}
	catch(char* sError)
	{
		if(bPromptError)
			AfxMessageBox(sError);
		else
			return FALSE;
	}
	return TRUE;
}

//������б�ļ���ȱʡֵ������������������û�����
int CalJgPosStyle(CLDSNode *pStart,CLDSNode *pEnd,const char *layer,int lstyle)
{
	int iJgStyle=lstyle;
	//����б��һ��ΪƲ������
	if(layer[0] == 'S' &&layer[1]=='X')
	{
		switch(layer[2])
		{
		case 'Q':
			if( pEnd->Position(false).z>pStart->Position(false).z)
			{	//�Ǹ���������
				if(pEnd->Position(false).x<pStart->Position(false).x)
					iJgStyle=0;	//Ʋ�Ǹ�
				else
					iJgStyle=2;	//��Ǹ�
			}
			else// if( pEnd->pos.z<pStart->pos.z)
			{	//�Ǹ���������
				if(pEnd->Position(false).x>pStart->Position(false).x)
					iJgStyle=0;	//Ʋ�Ǹ�
				else
					iJgStyle=2;	//��Ǹ�
			}
			break;
		case 'H':
			if( pEnd->Position(false).z>pStart->Position(false).z)
			{	//�Ǹ���������
				if(pEnd->Position(true).x>pStart->Position(false).x)
					iJgStyle=0;	//Ʋ�Ǹ�
				else
					iJgStyle=2;	//��Ǹ�
			}
			else// if( pEnd->pos.z<pStart->pos.z)
			{	//�Ǹ���������
				if(pEnd->Position(false).x<pStart->Position(false).x)
					iJgStyle=0;	//Ʋ�Ǹ�
				else
					iJgStyle=2;	//��Ǹ�
			}
			break;
		case 'Z':
			if( pEnd->Position(false).z>pStart->Position(false).z)
			{	//�Ǹ���������
				if(pEnd->Position(false).y<pStart->Position(false).y)
					iJgStyle=0;	//Ʋ�Ǹ�
				else
					iJgStyle=2;	//��Ǹ�
			}
			else// if( pEnd->pos.z<pStart->pos.z)
			{	//�Ǹ���������
				if(pEnd->Position(false).y>pStart->Position(false).y)
					iJgStyle=0;	//Ʋ�Ǹ�
				else
					iJgStyle=2;	//��Ǹ�
			}
			break;
		case 'Y':
			if( pEnd->Position(false).z>pStart->Position(false).z)
			{	//�Ǹ���������
				if(pEnd->Position(false).y>pStart->Position(false).y)
					iJgStyle=0;	//Ʋ�Ǹ�
				else
					iJgStyle=2;	//��Ǹ�
			}
			else// if( pEnd->pos.z<pStart->pos.z)
			{	//�Ǹ���������
				if(pEnd->Position(false).y<pStart->Position(false).y)
					iJgStyle=0;	//Ʋ�Ǹ�
				else
					iJgStyle=2;	//��Ǹ�
			}
			break;
		case 'T':
			break;
		default:
			break;
		}
	}
	return iJgStyle;
}

BOOL CLDSView::FinishCal1JgNorm(CLDSDbObject *pAtom1,
						CLDSDbObject *pAtom2)
{
	CLDSLineAngle *pNowJg=NULL;
	CLDSLinePart *pByLinePart=NULL;
	if(pAtom1->GetClassTypeId()!=CLS_LINEANGLE&&pAtom1->GetClassTypeId()!=CLS_GROUPLINEANGLE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Choose the wrong part!");
#else 
		AfxMessageBox("ѡ���˴����������!");
#endif
		return FALSE;
	}
	if(pAtom2&&pAtom2->IsPart())
	{	//ȷ��pAtom2Ϊ�˼�
		CLDSPart *pPart=(CLDSPart*)pAtom2;
		if(!pPart->IsLinePart())
			return FALSE;
	}
	else
		return FALSE;
	f3dPoint view_norm;
	pNowJg = (CLDSLineAngle*)pAtom1;
	pByLinePart = (CLDSLinePart*)pAtom2;
	//------VVVVVV--------------�Ǹ�֫���°ڷ�-----------
	Ta.BeginUndoListen();
	try{
		BOOL bKaiHeAngle;
#ifdef AFX_TARG_ENU_ENGLISH
		if(MessageBox("Whether to need to process open and merge angle?","",MB_YESNO|MB_DEFBUTTON2)==IDYES)
#else 
		if(MessageBox("�Ƿ���Ҫ���ϽǴ���?","",MB_YESNO|MB_DEFBUTTON2)==IDYES)
#endif
			bKaiHeAngle=FALSE;
		else
			bKaiHeAngle=TRUE;
		CalLineAngleWingNorm(pNowJg,pByLinePart,console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
		
		if(pNowJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
			((CLDSGroupLineAngle*)pNowJg)->UpdateSonJgWingNorm();
		pNowJg->SetModified();
		//�л���ʵ����ʾ״̬
		g_pSolidSet->SetDisplayType(DISP_SOLID);
		Invalidate(FALSE);
		if(!bKaiHeAngle)
		{	//����ԳƽǸֵĿ��Ͻ�
			static CMirMsgDlg mir_dlg;
			if(mir_dlg.DoModal()==IDOK)
			{
				CLDSLineAngle *cur_jg[4]={NULL};
				CLDSLinePart *line_part[4]={NULL};
				cur_jg[0]=pNowJg;
				line_part[0]=pByLinePart;
				if(mir_dlg.mirmsg.axis_flag&1)
				{
					cur_jg[1]=(CLDSLineAngle*)pNowJg->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(1));
					line_part[1]=(CLDSLinePart*)pByLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(1));
					if(cur_jg[1]&&line_part[1])
					{
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(cur_jg[1]->handle,TRUE);
						g_pSolidDraw->SetEntSnapStatus(line_part[1]->handle,TRUE);
						CalLineAngleWingNorm(cur_jg[1],line_part[1],console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
						if(cur_jg[1]->GetClassTypeId()==CLS_GROUPLINEANGLE)
							((CLDSGroupLineAngle*)cur_jg[1])->UpdateSonJgWingNorm();
						cur_jg[1]->SetModified();
					}
				}
				if(mir_dlg.mirmsg.axis_flag&2)
				{
					cur_jg[2]=(CLDSLineAngle*)pNowJg->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(2));
					line_part[2]=(CLDSLinePart*)pByLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(2));
					if(cur_jg[2]&&line_part[2])
					{
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(cur_jg[2]->handle,TRUE);
						g_pSolidDraw->SetEntSnapStatus(line_part[2]->handle,TRUE);
						CalLineAngleWingNorm(cur_jg[2],line_part[2],console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
						if(cur_jg[2]->GetClassTypeId()==CLS_GROUPLINEANGLE)
							((CLDSGroupLineAngle*)cur_jg[2])->UpdateSonJgWingNorm();
						cur_jg[2]->SetModified();
					}
				}
				if(mir_dlg.mirmsg.axis_flag&4)
				{
					cur_jg[3]=(CLDSLineAngle*)pNowJg->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(3));
					line_part[3]=(CLDSLinePart*)pByLinePart->GetMirPart(mir_dlg.mirmsg.GetSubMirItem(3));
					if(cur_jg[3]&&line_part[3])
					{
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(cur_jg[3]->handle,TRUE);
						g_pSolidDraw->SetEntSnapStatus(line_part[3]->handle,TRUE);
						CalLineAngleWingNorm(cur_jg[3],line_part[3],console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
						if(cur_jg[3]->GetClassTypeId()==CLS_GROUPLINEANGLE)
							((CLDSGroupLineAngle*)cur_jg[3])->UpdateSonJgWingNorm();
						cur_jg[3]->SetModified();
					}
				}
				if(mir_dlg.mirmsg.axis_flag&8)
				{
					mir_dlg.mirmsg.axis_flag=8;
					for(int i=0;i<4;i++)
					{
						if(cur_jg[i]&&line_part[i])
						{
							CLDSLineAngle *pMirAngle=NULL;
							CLDSLinePart *pMirLinePart=NULL;
							for(int j=0;j<mir_dlg.mirmsg.array_num;j++)
							{
								if(j==0)
								{
									pMirAngle=(CLDSLineAngle*)cur_jg[i]->GetMirPart(mir_dlg.mirmsg);
									pMirLinePart=(CLDSLinePart*)line_part[i]->GetMirPart(mir_dlg.mirmsg);
								}
								else if(pMirAngle&&pMirLinePart)
								{
									pMirAngle=(CLDSLineAngle*)pMirAngle->GetMirPart(mir_dlg.mirmsg);
									pMirLinePart=(CLDSLinePart*)pMirLinePart->GetMirPart(mir_dlg.mirmsg);
								}
								if(pMirAngle&&pMirLinePart)
								{
									g_pSolidDraw->ReleaseSnapStatus();
									g_pSolidDraw->SetEntSnapStatus(pMirAngle->handle,TRUE);
									g_pSolidDraw->SetEntSnapStatus(pMirLinePart->handle,TRUE);
									CalLineAngleWingNorm(pMirAngle,pMirLinePart,console.GetActiveView()->ucs.axis_z,bKaiHeAngle);
									if(pMirAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
										((CLDSGroupLineAngle*)pMirAngle)->UpdateSonJgWingNorm();
									pMirAngle->SetModified();
								}
								else
									break;
							}
						}
					}
				}
			}
		}
		Ta.EndUndoListen();
	}
	catch(char* sError)
	{
		Ta.EndUndoListen();
		AfxMessageBox(sError);
		return FALSE;
	}
	
#ifdef AFX_TARG_ENU_ENGLISH
	MessageBox("Normal direction calculated successfully");
#else 
	MessageBox("���߷������ɹ�");
#endif
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	return TRUE;
}
#endif

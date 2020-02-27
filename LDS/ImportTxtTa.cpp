// ImportTxtTa.cpp: implementation of the CImportTxtTa class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LDS.h"
#include "stdio.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "env_def.h"
#include "Tower.h"
#include "ConfirmJgdlg.h"
#include "MirTaAtom.h"
#include "mainfrm.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "dlg.h"
#include "ImportTxtTa.h"
#include "PromptDlg.h"
#include "Query.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void SplitFilePath(char *pathname,char *path=NULL,char *filename=NULL, char *extname=NULL)
{
	char drive[4];
	char dir[MAX_PATH];
	char fname[MAX_PATH];
	char ext[MAX_PATH];

	_splitpath(pathname,drive,dir,fname,ext);
	if(path)
	{
		strcpy(path,drive);
		strcat(path,dir);
	}
	if(filename)
		strcpy(filename,fname);
	if(extname)
		strcpy(extname,ext);
}
static void CalAtomLayer(CLDSNode *pNode)
{
	if(pNode->pos.x>=0&&pNode->pos.y>=0)
		pNode->SetLayer("SP1");
	else if(pNode->pos.x<=0&&pNode->pos.y>=0)
		pNode->SetLayer("SP2");
	else if(pNode->pos.x>=0&&pNode->pos.y<=0)
		pNode->SetLayer("SP3");
	else 
		pNode->SetLayer("SP4");
}
static void CalAtomLayer(CLDSLineAngle *pJg,int jg_type)
{
	if( pJg->pStart->layer(2)=='1'&&
		pJg->pEnd->layer(2)=='1')
	{
		pJg->SetLayer("SZ1");
	}
	else if( pJg->pStart->layer(2)=='2'&&
		pJg->pEnd->layer(2)=='2')
	{
		pJg->SetLayer("SZ2");
	}
	else if( pJg->pStart->layer(2)=='3'&&
		pJg->pEnd->layer(2)=='3')
	{
		pJg->SetLayer("SZ3");
	}
	else if( pJg->pStart->layer(2)=='4'&&
		pJg->pEnd->layer(2)=='4')
	{
		pJg->SetLayer("SZ4");
	}
	else if( (pJg->pStart->layer(2)=='1'
		||pJg->pStart->layer(2)=='2')&&
		(pJg->pEnd->layer(2)=='1'
		||pJg->pEnd->layer(2)=='2'))
	{
		pJg->SetLayer("SZQ");
	}
	else if( (pJg->pStart->layer(2)=='3'
		||pJg->pStart->layer(2)=='4')&&
		(pJg->pEnd->layer(2)=='3'
		||pJg->pEnd->layer(2)=='4'))
	{
		pJg->SetLayer("SZH");
	}
	else if( (pJg->pStart->layer(2)=='2'
		||pJg->pStart->layer(2)=='4')&&
		(pJg->pEnd->layer(2)=='2'
		||pJg->pEnd->layer(2)=='4'))
	{
		pJg->SetLayer(pJg->pStart->layer());
		pJg->SetLayer("SZZ");
	}
	else if( (pJg->pStart->layer(2)=='1'
		||pJg->pStart->layer(2)=='3')&&
		(pJg->pEnd->layer(2)=='1'
		||pJg->pEnd->layer(2)=='3'))
	{
		pJg->SetLayer("SZY");
	}
	else
		pJg->SetLayer("SZT");
	if(pJg->pStart->layer(0)=='L'||pJg->pEnd->layer(0)=='L')
		pJg->layer(0)='L';
	switch(jg_type)
	{
	case -1:	//������������
	case 1:		//˫����������
		pJg->layer(1)='Z';
		break;
	case 2:		//�Ȳ�б��
		pJg->layer(0)='L';
		pJg->layer(1)='X';
		break;
	case 3:		//����б��
	case 4:		//����
	case 6:		//��б��
		pJg->layer(1)='X';
		break;
	case 5:		//������
		pJg->layer(1)='F';
		break;
	default:
		break;
	}
}
//From MirTaAtom.cpp
int FindMirNo(int origin_no,int iMirMsg);

CImportTxtTa::CImportTxtTa()
{
	BodyLegInfo=NULL;
	m_pTa=NULL;
	m_nBodyNum=m_nLegNum=0;
	m_pView = NULL;
}

void CImportTxtTa::Empty()
{
	if(BodyLegInfo)
		delete []BodyLegInfo;

	BodyLegInfo = NULL;
	m_nBodyNum=m_nLegNum=0;
}
CImportTxtTa::~CImportTxtTa()
{
	Empty();
}
CFGWORD CImportTxtTa::GetBodyWord(int iBody)
{
	CFGWORD dw=0;
	for(LEG_MAP_BODY_STRU *pLegMapBody=LegMapBody.GetFirst();pLegMapBody;pLegMapBody=LegMapBody.GetNext())
	{
		if(pLegMapBody->iBody==iBody)
			dw.AddSpecNoWord(pLegMapBody->iLeg);
	}
	return dw;
}
CFGWORD CImportTxtTa::CalNodeLegDWORD(CLDSNode *pNode)
{
	int i;
	NODE_STRU *pNodeStru;
	pNodeStru = FindNodeByNo(pNode->point_i);
	//pNodeStru->pt_type = 0;	//ȱʡΪ���ɽڵ�
	CFGWORD common_dw=0;
	for(i=0;i<m_nLegNum;i++)
		common_dw.AddSpecNoWord(i+1);
	if(pNode->point_i<=BodyLegInfo[0].Tm3)
	{
		return common_dw;	//���ñ���
	}

	for(i=1;i<m_nBodyNum;i++)//����νڵ�
	{
		if(pNode->point_i>BodyLegInfo[i-1].Tm3&&pNode->point_i<=BodyLegInfo[i].Tm3)
			return GetBodyWord(i+1);
	}
	for(i=m_nBodyNum;i<m_nBodyNum+m_nLegNum;i++)
	{						//���ȶνڵ�
		int iBody = BodyLegInfo[i].iBody%10;
		if(pNode->point_i>=BodyLegInfo[i].Tm1&&pNode->point_i<=BodyLegInfo[i].Tm3)
		{
			if(pNode->point_i>=BodyLegInfo[i].Tm2)
			{
				pNodeStru = FindNodeByNo(pNode->point_i);
//				pNode->pt_type = 1;	//Լ���ڵ�
			}
			pNode->layer(0)='L';
			return GetCfgWord(i-m_nBodyNum+1);
		}
	}
	//�����Ķ˵�
	//pNode->pt_type=2;
	if(pNode->point_i<BodyLegInfo[0].MAX)
		return common_dw;
	for(i=1;i<m_nBodyNum+m_nLegNum;i++)
	{
		if(i<m_nBodyNum)	//�����ϵĲ����Ķ˵�
		{
			if(pNode->point_i>BodyLegInfo[i-1].MAX
				&&pNode->point_i<=BodyLegInfo[i].MAX)
				return GetBodyWord(i+1);
		}
		//�����ϵĲ����Ķ˵�
		else if(pNode->point_i>BodyLegInfo[i-1].MAX
			&&pNode->point_i<=BodyLegInfo[i].MAX)
		{
			pNode->layer(0)='L';
			return GetCfgWord(i-m_nBodyNum+1);
		}
	}
	return 100;
}
static void ReviseFatherJg(CLDSLineAngle *pJg)
{
	CLDSNode *pNode=NULL;
	f3dLine line;
	line.startPt = pJg->pStart->pos;
	line.endPt   = pJg->pEnd->pos;
	for(pNode=Ta.Node.GetFirst();pNode!=NULL;pNode=Ta.Node.GetNext())
	{
		if(pNode==pJg->pStart||pNode==pJg->pEnd)
		{
			if(pNode->father_part_handle==pJg->handle)
				continue;
		}
		if(line.PtInLine(pNode->pos)==2)
			pNode->father_part_handle = pJg->handle;
		else if(line.PtInLine(pNode->pos)==1)
		{
			if(pNode->father_part_handle<0x20)
				pNode->father_part_handle=pJg->handle;
			else
			{
				CLDSLineAngle *pNowJg = (CLDSLineAngle*)Ta.Parts.FromHandle(pNode->father_part_handle,CLS_LINEANGLE);
				if(pNowJg==NULL)
					pNode->father_part_handle=pJg->handle;
				else
				{
					if(pNowJg->layer(1)=='X')
					{
						if(pJg->layer(1)=='Z')
							pNode->father_part_handle = pJg->handle;
					}
					else if(pNowJg->layer(1)=='F')
					{
						if(pJg->layer(1)!='B')
							pNode->father_part_handle = pJg->handle;
					}
				}
			}
		}
	}
}
static BOOL CalDefaultJgNorm(CLDSLineAngle *pJg)
{
	if(pJg==NULL)
		return FALSE;
	f3dPoint vec = pJg->End()-pJg->Start();
	normalize(vec);
	if(vec.mod()<EPS)
		return FALSE;
	f3dPoint wing_x_norm=inters_vec(vec);
	f3dPoint wing_y_norm=wing_x_norm^vec;
	wing_x_norm=vec^wing_y_norm;
	pJg->set_norm_x_wing(wing_x_norm);
	pJg->set_norm_y_wing(wing_y_norm);
	return TRUE;
}
BOOL CImportTxtTa::ImportTxtFile(CTower *pTa,char *pathname,CView *pView)
{
	int i,j;
	CLDSNode *pNode;
	CLDSLineAngle *pJg=NULL,*pMirJg=NULL;
	char sError[200]="";
	//CConfirmJgDlg dlg;
	CProcBarDlg *pProcDlg = NULL;
	char filepath[MAX_PATH];
	char path[MAX_PATH],filename[MAX_PATH],extname[MAX_PATH];

	if(pTa)
		m_pTa = pTa;
	else
		return FALSE;
	m_pView = (CLDSView*)pView;
	if(!m_pView->LockFunc())
		return FALSE;
	//���ԭת�����е����ı�����
	Empty();
	try
	{
		m_pView->BeginWaitCursor();
		//����Ǹֽڵ��ʼ����
		pProcDlg = new CProcBarDlg(m_pView);
		pProcDlg->Create();
		if(!ImportJgNode(pathname,pProcDlg))
			throw "�����ı������ļ�����ʧ��!";
		SplitFilePath(pathname,path,filename,extname);
		strcpy(filepath,path);
		strcat(filepath,filename);
		strcat(filepath,".ifo");
		//����Ǹ���Ϣ�����������˨��Ϣ��
		ImportJgInfo(filepath,pProcDlg);

		for(i=0;i<m_nBodyNum+m_nLegNum;i++)
		{
			BOOL bOrdered = TRUE;
			for(j=0;j<m_nBodyNum+m_nLegNum-i-1;j++)
			{
				if(BodyLegInfo[j].Tm3>BodyLegInfo[j+1].Tm3)
				{
					BODYLEGINFO_STRU tembody;
					tembody = BodyLegInfo[j];
					BodyLegInfo[j] = BodyLegInfo[j+1];
					BodyLegInfo[j+1] = tembody;
					bOrdered = FALSE;
				}
			}
			if(bOrdered)
				break;
		}
		pProcDlg->Refresh(21);
		//���ڵ㲹����Ϣ
		f3dPoint node_pt;
		CLDSNode *pAttachNode;
		for(i=0;i<node_arr.GetSize();i++)
		{
			pNode = Ta.Node.append();
			node_arr[i].handle = pNode->handle;
			pNode->pos=node_arr[i].pt;
			CalAtomLayer(pNode);
			pNode->point_i = node_arr[i].No;
			pNode->cfgword = CalNodeLegDWORD(pNode);
			//node_arr[i].pt_type = pNode->pt_type;
			node_pt = pNode->pos;
			coord_trans(node_pt,Ta.GetActiveView()->ucs,TRUE);
			node_pt.ID = pNode->handle;
			//������Ϣ
			pNode->attach_type = node_arr[i].attach_type;
			if(pNode->attach_type==5)	//�����ȷֵ�
				pNode->attach_scale = 1.0/node_arr[i].attach_para;
			pAttachNode = Ta.FromPointI(node_arr[i].attach_no1);
			if(pAttachNode)
				pNode->attach_pthandle_arr[0]=pAttachNode->handle;
			pAttachNode = Ta.FromPointI(node_arr[i].attach_no2);
			if(pAttachNode)
				pNode->attach_pthandle_arr[1]=pAttachNode->handle;
			g_pSolidDraw->NewPoint(node_pt);
			//pNode->mir_msg = node_arr[i].iMirMsg;
			//MirTaAtom(pNode,pNode->mir_msg);
			pProcDlg->Refresh(21+9*(i+1)/node_arr.GetSize());
		}
		g_pSolidOper->ZoomAll(0.95);
		NODE_STRU *pNodeStru;
		CLDSNode *pStartNode=NULL,*pEndNode=NULL,
			*pExtStartNode=NULL,*pExtEndNode=NULL;
		f3dLine line;	//�Ǹ���
		PEN_STRU red_pen,common_pen;
		red_pen.crColor = RGB(255,0,0);
		red_pen.style = PS_DASH;
		red_pen.width = 1;
		common_pen.crColor = RGB(0,0,0);
		common_pen.style = PS_SOLID;
		common_pen.width = 1;
		//m_pView->FrontFace();
		for(i=0;i<jg_arr.GetSize();i++)
		{
			if(!jg_arr[i].bFather)	//���Ǹ��Ǹ��Ե�
				continue;
			pJg = (CLDSLineAngle*)Ta.Parts.append(CLS_LINEANGLE);
			jg_arr[i].handle = pJg->handle;
			
			pStartNode = Ta.FromPointI(jg_arr[i].start_no);
			pEndNode = Ta.FromPointI(jg_arr[i].end_no);
			line.ID = pJg->handle;
			if(pStartNode==NULL)
			{
				_snprintf(sError,199,"�Ǹ�%d-->%d����ʼ�ڵ��Ҳ���,���ݴ���",jg_arr[i].start_no,jg_arr[i].end_no);
				throw sError;
			}
			if(pEndNode==NULL)
			{
				_snprintf(sError,199,"�Ǹ�%d-->%d����ֹ�ڵ��Ҳ���,���ݴ���",jg_arr[i].start_no,jg_arr[i].end_no);
				throw sError;
			}
			pJg->pStart=pStartNode;
			pJg->pEnd=pEndNode;
			//	//2003.07.06
			//dlg.m_sGuiGe.Format("%.0fX%.0f",jg_arr[i].wing_wide,jg_arr[i].wing_thick);
			pJg->set_wing_wide(jg_arr[i].wing_wide);
			pJg->set_wing_thick(jg_arr[i].wing_thick);
			pJg->cMaterial=jg_arr[i].cMaterial;
			pJg->cfgword = pJg->pStart->cfgword;
			pJg->cfgword.AndWord(pJg->pEnd->cfgword);
			//�жϽǸֵ�ͼ����
			CalAtomLayer(pJg,jg_arr[i].type);
			RButtonDown.ResetEvent();
			//�ʼ���������ͼ
			for(j=0;j<2;j++)
			{
				try
				{
					//��ȡ�Ǹ�������˨��Ϣ
					GetLsInfo(&pJg->start_ls,pStartNode->point_i,pEndNode->point_i);
					GetLsInfo(&pJg->end_ls,pStartNode->point_i,pEndNode->point_i);
					line.startPt = pStartNode->pos;
					line.endPt = pEndNode->pos;
					coord_trans(line.startPt,Ta.GetActiveView()->ucs,TRUE);
					coord_trans(line.endPt,Ta.GetActiveView()->ucs,TRUE);
					g_pSolidSet->SetPen(red_pen);
					g_pSolidDraw->NewLine(line);
					if(pStartNode->father_part_handle<0x20)
					{
						pStartNode->father_part_handle = GetFatherJgHandle(pStartNode);
						if(pStartNode->father_part_handle<0x20)
							pStartNode->father_part_handle = pJg->handle;
					}
					if(pEndNode->father_part_handle<0x20)
					{
						pEndNode->father_part_handle = GetFatherJgHandle(pEndNode);
						if(pEndNode->father_part_handle<0x20)
							pEndNode->father_part_handle = pJg->handle;
					}
					jg_arr[i].start_no = pStartNode->point_i;
					jg_arr[i].end_no   = pEndNode->point_i;
					
					pJg->pStart=pStartNode;
					pJg->pEnd=pEndNode;
					//pJg->mir_msg = jg_arr[i].iMirMsg;
					pJg->SetStart(pJg->pStart->pos);
					pJg->SetEnd(pJg->pEnd->pos);
					g_pSolidSet->SetPen(common_pen);
					if(!MirTaAtom(pJg,jg_arr[i].iMirMsg))
						throw "�Գ���Ϣ���󣬽ǸֶԳ�����ʧ�ܣ�";
					ReviseFatherJg(pJg);
					CalDefaultJgNorm(pJg);
					switch(jg_arr[i].iMirMsg)
					{
					case 0:
						break;
					case 1:
						pMirJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pJg->hMirPartY,CLS_LINEANGLE);
						pMirJg->SetStart(pMirJg->pStart->pos);
						pMirJg->SetEnd(pMirJg->pEnd->pos);
						ReviseFatherJg(pMirJg);
						CalDefaultJgNorm(pMirJg);
						break;
					case 2:
						pMirJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pJg->hMirPartX,CLS_LINEANGLE);
						pMirJg->SetStart(pMirJg->pStart->pos);
						pMirJg->SetEnd(pMirJg->pEnd->pos);
						ReviseFatherJg(pMirJg);
						CalDefaultJgNorm(pMirJg);
						break;
					case 3:
						pMirJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pJg->hMirPartZ,CLS_LINEANGLE);
						pMirJg->SetStart(pMirJg->pStart->pos);
						pMirJg->SetEnd(pMirJg->pEnd->pos);
						ReviseFatherJg(pMirJg);
						CalDefaultJgNorm(pMirJg);
						break;
					case 4:
						pMirJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pJg->hMirPartY,CLS_LINEANGLE);
						pMirJg->SetStart(pMirJg->pStart->pos);
						pMirJg->SetEnd(pMirJg->pEnd->pos);
						ReviseFatherJg(pMirJg);
						CalDefaultJgNorm(pMirJg);
						pMirJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pJg->hMirPartX,CLS_LINEANGLE);
						pMirJg->SetStart(pMirJg->pStart->pos);
						pMirJg->SetEnd(pMirJg->pEnd->pos);
						ReviseFatherJg(pMirJg);
						CalDefaultJgNorm(pMirJg);
						pMirJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pJg->hMirPartZ,CLS_LINEANGLE);
						pMirJg->SetStart(pMirJg->pStart->pos);
						pMirJg->SetEnd(pMirJg->pEnd->pos);
						ReviseFatherJg(pMirJg);
						CalDefaultJgNorm(pMirJg);
						break;
					default:
						break;
					}
					g_pSolidDraw->DelEnt(line.ID);
					g_pSolidDraw->NewLine(line);	//�����߻�Ϊ����
					break;
				}
				catch (int iError)
				{
					if(iError==0)
					{
						if(m_pView->m_eViewFlag == FRONT_VIEW)
							m_pView->m_eViewFlag=RIGHT_VIEW;
						else
							m_pView->m_eViewFlag=FRONT_VIEW;
					}
					else
					{
						if(iError==-1)
							_snprintf(sError,199,"����%d->%d�ǸֶԳ���Ϣ����,�޷��Գ�",pJg->pStart->point_i,pJg->pEnd->point_i);
						else
							_snprintf(sError,199,"%d->%d�ǸֵĶԳƽǸ�֫���߼������",pJg->pStart->point_i,pJg->pEnd->point_i);
						AfxMessageBox(sError);
						break;
					}
				}
			}
			pProcDlg->Refresh(30+60*(i+1)/jg_arr.GetSize());
		}
		//m_pView->FrontFace();
		g_pSolidSet->SetPen(common_pen);
		//����ڵ�Գ���Ϣ
		for(i=0;i<node_arr.GetSize();i++)
		{
			pNode = Ta.Node.FromHandle(node_arr[i].handle);
			//pNode->is_father_node = node_arr[i].bFather;
			NODE_STRU *pFatherNodeStru = FindNodeByNo(node_arr[i].father_no);
			if(pFatherNodeStru)
				pNode->father_node_handle = pFatherNodeStru->handle;
			if(node_arr[i].mir_x_no>0)
			{
				pNodeStru = FindNodeByNo(node_arr[i].mir_x_no);
				if(pNodeStru)
					pNode->mir_x_node_handle = pNodeStru->handle;
			}
			if(node_arr[i].mir_y_no>0)
			{
				pNodeStru = FindNodeByNo(node_arr[i].mir_y_no);
				if(pNodeStru)
					pNode->mir_y_node_handle = pNodeStru->handle;
			}
			if(node_arr[i].mir_z_no>0)
			{
				pNodeStru = FindNodeByNo(node_arr[i].mir_z_no);
				if(pNodeStru)
					pNode->mir_z_node_handle = pNodeStru->handle;
			}
			if(pNode->attach_type==5)	//�Ǹ��ϵı����ȷֵ�
				pNode->attach_jghandle_arr[0] = pNode->father_part_handle;
			if(node_arr[i].bFather)
			{
				CLDSNode *pMirNode;
				switch(node_arr[i].iMirMsg)
				{
				case 0:
					break;
				case 1:	//Y��Գ�
					pMirNode = Ta.Node.FromHandle(pNode->mir_y_node_handle);
					ConvertAttachHandle(pNode,pMirNode,1);
					break;
				case 2:	//X��Գ�
					pMirNode = Ta.Node.FromHandle(pNode->mir_x_node_handle);
					ConvertAttachHandle(pNode,pMirNode,2);
					break;
				case 3:	//Z��Գ�
					pMirNode = Ta.Node.FromHandle(pNode->mir_z_node_handle);
					ConvertAttachHandle(pNode,pMirNode,3);
					break;
				case 4:	//XYZ�Գ�
					pMirNode = Ta.Node.FromHandle(pNode->mir_y_node_handle);
					ConvertAttachHandle(pNode,pMirNode,1);
					pMirNode = Ta.Node.FromHandle(pNode->mir_x_node_handle);
					ConvertAttachHandle(pNode,pMirNode,2);
					pMirNode = Ta.Node.FromHandle(pNode->mir_z_node_handle);
					ConvertAttachHandle(pNode,pMirNode,3);
					break;
				}
			}
			pProcDlg->Refresh(90+10*(i+1)/node_arr.GetSize());
		}
	}
	catch(char *sError)
	{
		g_pSolidSet->SetPen(RGB(0,0,0),PS_SOLID,1);
		CString ss;
		if(pJg)
		{
			ss.Format("%s[�Ǹ�%d-->%d]",sError,pJg->pStart->point_i,pJg->pEnd->point_i);
			AfxMessageBox(ss);
		}
		else
			ss=sError;
		m_pView->ReleaseFunc();
		m_pView->EndWaitCursor();
		if(pProcDlg)
			delete pProcDlg;
		return FALSE;
	}

	if(pProcDlg)
		delete pProcDlg;
	m_pView->ReleaseFunc();
	m_pView->EndWaitCursor();
	return TRUE;
}
void CImportTxtTa::MirNode(NODE_STRU node,NODE_STRU mir_node_arr[4])
{
	char sError[200]="";
	int origin_no;
	origin_no = node.No;		//�ڵ��� 
	f3dPoint origin_pt = node.pt;
	node.bFather = TRUE;
	node.father_no = 0;
	switch(node.iMirMsg)
	{
	case 0:		//�޶Գ�
		mir_node_arr[0] = node;
		break;
	case 1:		//���ҶԳ�
		if(fabs(origin_pt.x)<EPS)
		{
			_snprintf(sError,199,"��X����Ϊ��Ľڵ�%d�������ҶԳ�,���ݴ���",origin_no);
			throw sError;
		}
		node.mir_y_no = FindMirNo(origin_no,1);
		mir_node_arr[0] = node;
		node.No = FindMirNo(origin_no,1);
		node.pt.Set(-1.0*origin_pt.x,origin_pt.y,origin_pt.z);
		node.mir_y_no = origin_no;
		node.bFather = FALSE;
		node.father_no = origin_no;
		node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,1);
		if(FindNodeByNo(node.attach_no1)==NULL)
			node.attach_no1 = mir_node_arr[0].attach_no1;
		node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,1);
		if(FindNodeByNo(node.attach_no2)==NULL)
			node.attach_no2 = mir_node_arr[0].attach_no2;
		mir_node_arr[1] = node;
		break;
	case 2:		//ǰ��Գ�
		if(fabs(origin_pt.y)<EPS)
		{
			_snprintf(sError,199,"��Y����Ϊ��Ľڵ�%d�������ҶԳ�,���ݴ���",origin_no);
			throw sError;
		}
		node.mir_x_no = FindMirNo(origin_no,2);
		mir_node_arr[0] = node;
		node.No = FindMirNo(origin_no,2);
		node.pt.Set(origin_pt.x,-1.0*origin_pt.y,origin_pt.z);
		node.mir_x_no = origin_no;
		node.bFather = FALSE;
		node.father_no = origin_no;
		node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,2);
		if(FindNodeByNo(node.attach_no1)==NULL)
			node.attach_no1 = mir_node_arr[0].attach_no1;
		node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,2);
		if(FindNodeByNo(node.attach_no2)==NULL)
			node.attach_no2 = mir_node_arr[0].attach_no2;
		mir_node_arr[1] = node;
		break;
	case 3:		//Z��Գ�
		if(fabs(origin_pt.x)<EPS&&fabs(origin_pt.y)<EPS)
		{
			_snprintf(sError,199,"��Z����Ϊ��Ľڵ�%d�������ҶԳ�,���ݴ���",origin_no);
			throw sError;
		}
		node.mir_x_no = FindMirNo(origin_no,3);
		mir_node_arr[0] = node;
		node.No = FindMirNo(origin_no,3);
		node.pt.Set(-1.0*origin_pt.x,-1.0*origin_pt.y,origin_pt.z);
		node.mir_x_no = origin_no;
		node.bFather = FALSE;
		node.father_no = origin_no;
		//������Ϣ
		node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,3);
		if(FindNodeByNo(node.attach_no1)==NULL)
		{
			if(fabs(mir_node_arr[0].pt.x)<EPS)
			{
				node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,2);
				if(FindNodeByNo(node.attach_no1)==NULL)
					node.attach_no1 = mir_node_arr[0].attach_no1;
			}
			else if(fabs(mir_node_arr[0].pt.y)<EPS)
			{
				node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,1);
				if(FindNodeByNo(node.attach_no1)==NULL)
					node.attach_no1 = mir_node_arr[0].attach_no1;
			}
			else
				node.attach_no1 = mir_node_arr[0].attach_no1;
		}
		node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,2);
		if(FindNodeByNo(node.attach_no2)==NULL)
		{
			if(fabs(mir_node_arr[0].pt.x)<EPS)
			{
				node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,2);
				if(FindNodeByNo(node.attach_no2)==NULL)
					node.attach_no2 = mir_node_arr[0].attach_no2;
			}
			else if(fabs(mir_node_arr[0].pt.y)<EPS)
			{
				node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,1);
				if(FindNodeByNo(node.attach_no2)==NULL)
					node.attach_no2 = mir_node_arr[0].attach_no2;
			}
			else
				node.attach_no2 = mir_node_arr[0].attach_no2;
		}
		mir_node_arr[1] = node;
		break;
	case 4:		//XYZ�Գ�
		node.mir_y_no = FindMirNo(origin_no,1);
		node.mir_x_no = FindMirNo(origin_no,2);
		node.mir_z_no = FindMirNo(origin_no,3);
		mir_node_arr[0] = node;

		node.bFather = FALSE;
		node.father_no = origin_no;
		//1.���ҶԳ�
		if(fabs(origin_pt.x)<EPS)
		{
			_snprintf(sError,199,"��X����Ϊ��Ľڵ�%d�������ҶԳ�,���ݴ���",origin_no);
			throw sError;
		}
		node.No = FindMirNo(origin_no,1);
		node.pt.Set(-1.0*origin_pt.x,origin_pt.y,origin_pt.z);
		node.mir_y_no = origin_no;
		node.mir_x_no = FindMirNo(origin_no,3);
		node.mir_z_no = FindMirNo(origin_no,2);
		node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,1);
		if(FindNodeByNo(node.attach_no1)==NULL)
			node.attach_no1 = mir_node_arr[0].attach_no1;
		node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,1);
		if(FindNodeByNo(node.attach_no2)==NULL)
			node.attach_no2 = mir_node_arr[0].attach_no2;
		mir_node_arr[1] = node;
		//2.ǰ��Գ�
		if(fabs(origin_pt.y)<EPS)
		{
			_snprintf(sError,199,"��Y����Ϊ��Ľڵ�%d�������ҶԳ�,���ݴ���",origin_no);
			throw sError;
		}
		node.No = FindMirNo(origin_no,2);
		node.pt.Set(origin_pt.x,-1.0*origin_pt.y,origin_pt.z);
		node.mir_y_no = FindMirNo(origin_no,3);
		node.mir_x_no = origin_no;
		node.mir_z_no = FindMirNo(origin_no,1);
		node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,2);
		if(FindNodeByNo(node.attach_no1)==NULL)
			node.attach_no1 = mir_node_arr[0].attach_no1;
		node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,2);
		if(FindNodeByNo(node.attach_no2)==NULL)
			node.attach_no2 = mir_node_arr[0].attach_no2;
		mir_node_arr[2] = node;
		//3.Z��Գ�
		if(fabs(origin_pt.x)<EPS&&fabs(origin_pt.y)<EPS)
		{
			_snprintf(sError,199,"��Z����Ϊ��Ľڵ�%d�������ҶԳ�,���ݴ���",origin_no);
			throw sError;
		}
		node.No = FindMirNo(origin_no,3);
		node.pt.Set(-1.0*origin_pt.x,-1.0*origin_pt.y,origin_pt.z);
		node.mir_y_no = FindMirNo(origin_no,2);
		node.mir_x_no = FindMirNo(origin_no,1);
		node.mir_z_no = origin_no;

		//������Ϣ
		node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,3);
		if(FindNodeByNo(node.attach_no1)==NULL)
		{
			if(fabs(mir_node_arr[0].pt.x)<EPS)
			{
				node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,2);
				if(FindNodeByNo(node.attach_no1)==NULL)
					node.attach_no1 = mir_node_arr[0].attach_no1;
			}
			else if(fabs(mir_node_arr[0].pt.y)<EPS)
			{
				node.attach_no1 = FindMirNo(mir_node_arr[0].attach_no1,1);
				if(FindNodeByNo(node.attach_no1)==NULL)
					node.attach_no1 = mir_node_arr[0].attach_no1;
			}
			else
				node.attach_no1 = mir_node_arr[0].attach_no1;
		}
		node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,2);
		if(FindNodeByNo(node.attach_no2)==NULL)
		{
			if(fabs(mir_node_arr[0].pt.x)<EPS)
			{
				node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,2);
				if(FindNodeByNo(node.attach_no2)==NULL)
					node.attach_no2 = mir_node_arr[0].attach_no2;
			}
			else if(fabs(mir_node_arr[0].pt.y)<EPS)
			{
				node.attach_no2 = FindMirNo(mir_node_arr[0].attach_no2,1);
				if(FindNodeByNo(node.attach_no2)==NULL)
					node.attach_no2 = mir_node_arr[0].attach_no2;
			}
			else
				node.attach_no2 = mir_node_arr[0].attach_no2;
		}
		mir_node_arr[3] = node;
		break;
	default:
		_snprintf(sError,199,"�ڵ�%d�ĶԳ���Ϣֵ(%d)����!",node.No,node.iMirMsg);
		throw sError;
	}
}
void CImportTxtTa::MirJg(JG_STRU jg,JG_STRU mir_jg_arr[4])
{
	char sError[200]="";
	int origin_no,origin_start_no,origin_end_no;
	NODE_STRU *pStartNodeStru,*pEndNodeStru,*pNodeStru=NULL;
	//��սǸ���Ϣ�������˨��Ϣ��
	jg.LsN = 0;

	origin_start_no=jg.start_no;
	origin_end_no  =jg.end_no;
	origin_no = jg.No;		//�Ǹֱ��
	pStartNodeStru = FindNodeByNo(jg.start_no);
	pEndNodeStru = FindNodeByNo(jg.end_no);
	if(pStartNodeStru==NULL||pEndNodeStru==NULL)
		throw CString("�Ǹ�ָ���˿սڵ�,���ݴ���");
	jg.bFather = TRUE;
	jg.father_no = 0;
	switch(jg.iMirMsg)
	{
	case 0:		//�޶Գ�
		mir_jg_arr[0] = jg;
		break;
	case 1:		//���ҶԳ�
		jg.mir_y_no = FindMirNo(origin_no,1);
		mir_jg_arr[0] = jg;
		jg.start_no = FindMirNo(jg.start_no,1);
		if(FindNodeByNo(jg.start_no)==NULL)
			jg.start_no = origin_start_no;
		jg.end_no = FindMirNo(jg.end_no,1);
		if(FindNodeByNo(jg.end_no)==NULL)
			jg.end_no = origin_end_no;
		jg.No = FindMirNo(origin_no,1);
		jg.mir_y_no = origin_no;
		jg.bFather = FALSE;
		jg.father_no = origin_no;
		mir_jg_arr[1] = jg;
		break;
	case 2:		//ǰ��Գ�
		jg.mir_x_no = FindMirNo(origin_no,2);
		mir_jg_arr[0] = jg;
		jg.start_no = FindMirNo(jg.start_no,2);
		if(FindNodeByNo(jg.start_no)==NULL)
			jg.start_no = origin_start_no;
		jg.end_no = FindMirNo(jg.end_no,2);
		if(FindNodeByNo(jg.end_no)==NULL)
			jg.end_no = origin_end_no;
		jg.No = FindMirNo(origin_no,2);
		jg.mir_x_no = origin_no;
		jg.bFather = FALSE;
		jg.father_no = origin_no;
		mir_jg_arr[1] = jg;
		break;
	case 3:		//Z��Գ�
		jg.mir_x_no = FindMirNo(origin_no,3);
		mir_jg_arr[0] = jg;
		jg.start_no = FindMirNo(origin_start_no,3);
		if(FindNodeByNo(jg.start_no)==NULL)
		{
			if(fabs(pStartNodeStru->pt.x)<EPS)
			{
				jg.start_no = FindMirNo(origin_start_no,2);
				if(FindNodeByNo(jg.start_no)==NULL)
					jg.start_no = origin_start_no;
			}
			else if(fabs(pStartNodeStru->pt.y)<EPS)
			{
				jg.start_no = FindMirNo(origin_start_no,1);
				if(FindNodeByNo(jg.start_no)==NULL)
					jg.start_no = origin_start_no;
			}
			else
				jg.start_no = origin_start_no;
		}
		jg.end_no = FindMirNo(jg.end_no,3);
		if(FindNodeByNo(jg.end_no)==NULL)
		{
			if(fabs(pEndNodeStru->pt.x)<EPS)
			{
				jg.end_no = FindMirNo(origin_end_no,2);
				if(FindNodeByNo(jg.end_no)==NULL)
					jg.end_no = origin_end_no;
			}
			else if(fabs(pEndNodeStru->pt.y)<EPS)
			{
				jg.end_no = FindMirNo(origin_end_no,1);
				if(FindNodeByNo(jg.end_no)==NULL)
					jg.end_no = origin_end_no;
			}
			else
				jg.end_no = origin_end_no;
		}
		jg.No = FindMirNo(origin_no,3);
		jg.mir_x_no = origin_no;
		jg.bFather = FALSE;
		jg.father_no = origin_no;
		mir_jg_arr[1] = jg;
		break;
	case 4:		//XYZ�Գ�
		jg.mir_y_no = FindMirNo(origin_no,1);
		jg.mir_x_no = FindMirNo(origin_no,2);
		jg.mir_z_no = FindMirNo(origin_no,3);
		mir_jg_arr[0] = jg;

		jg.bFather = FALSE;
		jg.father_no = origin_no;
		//1.���ҶԳ�
		jg.No = FindMirNo(origin_no,1);
		jg.start_no = FindMirNo(origin_start_no,1);
		if(FindNodeByNo(jg.start_no)==NULL)
			jg.start_no = origin_start_no;
		jg.end_no = FindMirNo(origin_end_no,1);
		if(FindNodeByNo(jg.end_no)==NULL)
			jg.end_no = origin_end_no;
		jg.mir_y_no = origin_no;
		jg.mir_x_no = FindMirNo(jg.No,2);
		jg.mir_z_no = FindMirNo(jg.No,3);
		mir_jg_arr[1] = jg;
		//2.ǰ��Գ�
		jg.No = FindMirNo(origin_no,2);
		jg.start_no = FindMirNo(origin_start_no,2);
		if(FindNodeByNo(jg.start_no)==NULL)
			jg.start_no = origin_start_no;
		jg.end_no = FindMirNo(origin_end_no,2);
		if(FindNodeByNo(jg.end_no)==NULL)
			jg.end_no = origin_end_no;
		jg.mir_y_no = FindMirNo(jg.No,1);
		jg.mir_x_no = origin_no;
		jg.mir_z_no = FindMirNo(jg.No,3);
		mir_jg_arr[2] = jg;
		//3.Z��Գ�
		jg.No = FindMirNo(origin_no,3);
		jg.start_no = FindMirNo(origin_start_no,3);
		if(FindNodeByNo(jg.start_no)==NULL)
		{
			if(fabs(pStartNodeStru->pt.x)<EPS)
			{
				jg.start_no = FindMirNo(origin_start_no,2);
				if(FindNodeByNo(jg.start_no)==NULL)
					jg.start_no = origin_start_no;
			}
			else if(fabs(pStartNodeStru->pt.y)<EPS)
			{
				jg.start_no = FindMirNo(origin_start_no,1);
				if(FindNodeByNo(jg.start_no)==NULL)
					jg.start_no = origin_start_no;
			}
			else
				jg.start_no = origin_start_no;
		}
		jg.end_no = FindMirNo(origin_end_no,3);
		if(FindNodeByNo(jg.end_no)==NULL)
		{
			if(fabs(pEndNodeStru->pt.x)<EPS)
			{
				jg.end_no = FindMirNo(origin_end_no,2);
				if(FindNodeByNo(jg.end_no)==NULL)
					jg.end_no = origin_end_no;
			}
			else if(fabs(pEndNodeStru->pt.y)<EPS)
			{
				jg.end_no = FindMirNo(origin_end_no,1);
				if(FindNodeByNo(jg.end_no)==NULL)
					jg.end_no = origin_end_no;
			}
			else
				jg.end_no = origin_end_no;
		}
		jg.mir_y_no = FindMirNo(jg.No,1);
		jg.mir_x_no = FindMirNo(jg.No,2);
		jg.mir_z_no = origin_no;
		mir_jg_arr[3] = jg;
		break;
	default:
		_snprintf(sError,199,"�Ǹ�%d-->%d�ĶԳ���Ϣֵ(%d)����!",jg.start_no,jg.end_no,jg.iMirMsg);
		throw sError;
	}
}

JG_STRU *CImportTxtTa::FindJgByStartEndNo(int start_no,int end_no)
{
	for(int i=0;i<jg_arr.GetSize();i++)
	{
		if((jg_arr[i].start_no==start_no&&jg_arr[i].end_no==end_no)
			||(jg_arr[i].start_no==end_no&&jg_arr[i].end_no==start_no))
		{
			return &jg_arr[i];
		}
	}
	return NULL;
}
JG_STRU *CImportTxtTa::FindJgByNo(int no)
{
	int nn;
	nn = jg_arr.GetSize();
	for(int i=0;i<nn;i++)
	{
		if(jg_arr[i].No==no)
			return &jg_arr[i];
	}
	return NULL;
}
NODE_STRU *CImportTxtTa::FindNodeByNo(int No)
{
	int nn;
	nn = node_arr.GetSize();
	for(int i=0;i<nn;i++)
	{
		if(node_arr[i].No==No)
			return &node_arr[i];
	}
	return NULL;
}

BOOL CImportTxtTa::ImportJgNode(char *filename,CProcBarDlg *pProcDlg)
{
	FILE *fp;
	NODE_STRU node,*pNode1,*pNode2;
	JG_STRU jg;
	char mark,line_txt[100];
	int i,iNodeIndex=0,iJgIndex=0;
	int Tmin,Tmax1,Tmax2,MAX,iBody,Var;
	int Kind,iMaterial,FirstNodeN,SecondNodeN,ForceJgN,ViceJgN,BodyLegN;
	char sError[200]="";

	if((fp=fopen(filename,"rt"))==NULL)
		throw CString("�򲻿�ָ���ļ�");
	try
	{
		//���������Ϣ
		fgets(line_txt,100,fp);
		sscanf(line_txt,"%d%c%d%c%d%c%d%c%d%c%d",&Kind,&mark,
			&FirstNodeN,&mark,&SecondNodeN,&mark,&ForceJgN,
			&mark,&ViceJgN,&mark,&BodyLegN);
		//������������,�������Ϊ��ͳһ��ʽ��
		fgets(line_txt,100,fp);
		m_nBodyNum = BodyLegN/100;
		m_nLegNum = BodyLegN%100;
		if(FirstNodeN<=0)
			throw CString("��һ��ڵ�����������,���ݴ���");
		else if(SecondNodeN<0)
			throw CString("�ڶ���ڵ�����Ϊ����,���ݴ���");
		if(ForceJgN<0)
			throw CString("����������Ϊ����,���ݴ���");
		else if(ViceJgN<0)
			throw CString("����������Ϊ����,���ݴ���");
		
		BodyLegInfo = new BODYLEGINFO_STRU[m_nBodyNum+m_nLegNum];
		for(i=0;i<m_nBodyNum+m_nLegNum;i++)
		{
			fgets(line_txt,100,fp);
			while(strcmp(line_txt,"\n")==0)
				fgets(line_txt,100,fp);
			sscanf(line_txt,"%d%c%d%c%d%c%d%c%d%c%d",&Tmin,&mark,&Tmax1,&mark,
			&Tmax2,&mark,&MAX,&mark,&iBody,&mark,&Var);
			if(i>=m_nBodyNum)
			{
				LEG_MAP_BODY_STRU *pLegMapBody = LegMapBody.append();
				pLegMapBody->iBody = iBody%10;	//�˽��ȵı����
				pLegMapBody->iLeg  = i-m_nBodyNum+1;
			}
			BodyLegInfo[i].Tm1 = Tmin;
			BodyLegInfo[i].Tm2 = Tmax1;
			BodyLegInfo[i].Tm3 = Tmax2;
			BodyLegInfo[i].MAX = MAX;
			BodyLegInfo[i].iBody = iBody;
			BodyLegInfo[i].Var = Var;
		}
		for(i=0;i<FirstNodeN;i++)
		{
			fgets(line_txt,100,fp);
			sscanf(line_txt,"%d%c%d%c%lf%c%lf%c%lf",&node.No,&mark,&node.iMirMsg,
			&mark,&node.pt.x,&mark,&node.pt.y,&mark,&node.pt.z);
			node.attach_type = 0;
			node.attach_no1 = 0;
			node.attach_no2 = 0;
			node.attach_para = 0;
			if(node.pt.y>10000&&node.pt.z>10000)
			{		//2.�Ǹ���X����ֵ�����
				node.attach_no1 = int(node.pt.y-10000);
				node.attach_no2 = int(node.pt.z-10000);
				pNode1 = FindNodeByNo(node.attach_no1);
				pNode2 = FindNodeByNo(node.attach_no2);
				if(pNode1==NULL||pNode2==NULL)
				{
					_snprintf(sError,199,"�ڵ�%d�������ݲ�����",node.No);
					throw sError;
				}
				node.pt.x *= 1000;	//��λת��(����ת��Ϊ����)
				node.pt.y = pNode1->pt.y+(pNode2->pt.y-pNode1->pt.y)*
					(node.pt.x-pNode1->pt.x)/(pNode2->pt.x-pNode1->pt.x);
				node.pt.z = pNode1->pt.z+(pNode2->pt.z-pNode1->pt.z)*
					(node.pt.x-pNode1->pt.x)/(pNode2->pt.x-pNode1->pt.x);
				node.attach_type = 1;
			}
			if(node.pt.x>10000&&node.pt.z>10000)
			{	//3.�Ǹ���Y����ֵ�����
				node.attach_no1 = int(node.pt.x-10000);
				node.attach_no2 = int(node.pt.z-10000);
				pNode1 = FindNodeByNo(node.attach_no1);
				pNode2 = FindNodeByNo(node.attach_no2);
				if(pNode1==NULL||pNode2==NULL)
				{
					_snprintf(sError,199,"�ڵ�%d�������ݲ�����",node.No);
					throw sError;
				}
				node.pt.y *= 1000;	//��λת��(����ת��Ϊ����)
				node.pt.x = pNode1->pt.x+(pNode2->pt.x-pNode1->pt.x)*
					(node.pt.y-pNode1->pt.y)/(pNode2->pt.y-pNode1->pt.y);
				node.pt.z = pNode1->pt.z+(pNode2->pt.z-pNode1->pt.z)*
					(node.pt.y-pNode1->pt.y)/(pNode2->pt.y-pNode1->pt.y);
				node.attach_type = 2;
			}
			if(node.pt.x>10000&&node.pt.y>10000)
			{	//4.�Ǹ���Z����ֵ�����
				node.attach_no1 = int(node.pt.x-10000);
				node.attach_no2 = int(node.pt.y-10000);
				pNode1 = FindNodeByNo(node.attach_no1);
				pNode2 = FindNodeByNo(node.attach_no2);
				if(pNode1==NULL||pNode2==NULL)
				{
					_snprintf(sError,199,"�ڵ�%d�������ݲ�����",node.No);
					throw sError;
				}
				node.pt.z *= 1000;	//��λת��(����ת��Ϊ����)
				node.pt.x = pNode1->pt.x+(pNode2->pt.x-pNode1->pt.x)*
					(node.pt.z-pNode1->pt.z)/(pNode2->pt.z-pNode1->pt.z);
				node.pt.y = pNode1->pt.y+(pNode2->pt.y-pNode1->pt.y)*
					(node.pt.z-pNode1->pt.z)/(pNode2->pt.z-pNode1->pt.z);
				node.attach_type = 3;
			}
			if(i<FirstNodeN-1&&feof(fp))
				throw CString("��һ��ڵ��������һ�п�����Ϣ����,���ݴ���");
			NODE_STRU mir_node_arr[4];
			if(node.attach_type==0)
				node.pt = node.pt*1000;		//��λת��(����ת��Ϊ����)
			MirNode(node,mir_node_arr);
			if(node.iMirMsg<=0)
				node_arr.Add(mir_node_arr[0]);
			else if(node.iMirMsg>0&&node.iMirMsg<4)
			{
				node_arr.Add(mir_node_arr[0]);
				node_arr.Add(mir_node_arr[1]);
			}
			else
			{
				for(int k=0;k<4;k++)
					node_arr.Add(mir_node_arr[k]);
			}
			iNodeIndex++;
			pProcDlg->Refresh(10*iNodeIndex/(FirstNodeN+SecondNodeN+ForceJgN+ViceJgN));
		}
		for(i=0;i<SecondNodeN;i++)
		{
			fgets(line_txt,100,fp);
			sscanf(line_txt,"%d%c%d%c%d%c%lf%c%d",&node.No,&mark,&node.attach_no1,
			&mark,&node.attach_no2,&mark,&node.attach_para,&mark,&node.iMirMsg);
			node.attach_type = 5;
			NODE_STRU *pNode1 = FindNodeByNo(node.attach_no1);
			NODE_STRU *pNode2 = FindNodeByNo(node.attach_no2);
			if(pNode1==NULL||pNode2==NULL)
			{
				_snprintf(sError,199,"�ڵ�%d�������ݲ�����",node.No);
				throw sError;
			}
			f3dPoint vec;
			Sub_Pnt(vec,pNode2->pt,pNode1->pt);
			Add_Pnt(node.pt,pNode1->pt,vec*(1/node.attach_para));
			if(node.attach_para<=0)
			{
				throw CString("�ڵ�ȷֲ���������,���ݴ���");
			}
			if(i<SecondNodeN-1&&feof(fp))
			{
				throw CString("�ڶ���ڵ��������һ�п�����Ϣ����,���ݴ���");
			}
			
			NODE_STRU mir_node_arr[4];
			MirNode(node,mir_node_arr);
			if(node.iMirMsg<=0)
				node_arr.Add(mir_node_arr[0]);
			else if(node.iMirMsg>0&&node.iMirMsg<4)
			{
				node_arr.Add(mir_node_arr[0]);
				node_arr.Add(mir_node_arr[1]);
			}
			else
			{
				for(int k=0;k<4;k++)
					node_arr.Add(mir_node_arr[k]);
			}
			iNodeIndex++;
			pProcDlg->Refresh(10*(FirstNodeN+iNodeIndex)/(FirstNodeN+SecondNodeN+ForceJgN+ViceJgN));
		}
		//��������������
		for(i=0;i<ForceJgN;i++)
		{
			fgets(line_txt,100,fp);
			sscanf(line_txt,"%d%c%d%c%d%c%d%c%d",&jg.start_no,&mark,
			&jg.end_no,&mark,&iMaterial,&mark,&jg.iMirMsg,&mark,&jg.type);
			if(i<ForceJgN-1&&feof(fp))
				throw CString("�������������һ�п�����Ϣ����,���ݴ���");
			jg.No = iJgIndex*10;
			JG_STRU mir_jg_arr[4];
			MirJg(jg,mir_jg_arr);
			if(jg.iMirMsg<=0)
				jg_arr.Add(mir_jg_arr[0]);
			else if(jg.iMirMsg>0&&jg.iMirMsg<4)
			{
				jg_arr.Add(mir_jg_arr[0]);
				jg_arr.Add(mir_jg_arr[1]);
			}
			else
			{
				for(int k=0;k<4;k++)
					jg_arr.Add(mir_jg_arr[k]);
			}
			iJgIndex++;
			pProcDlg->Refresh(10*(FirstNodeN+SecondNodeN+iNodeIndex)/(FirstNodeN+SecondNodeN+ForceJgN+ViceJgN));
		}
		//���벹��������
		for(i=0;i<ViceJgN;i++)
		{
			fgets(line_txt,100,fp);
			sscanf(line_txt,"%d%c%d%c%d",&jg.start_no,&mark,&jg.end_no,&mark,&jg.iMirMsg);
			jg.iMirMsg=jg.iMirMsg%10;
			if(jg.iMirMsg==1)
				jg.iMirMsg=0;
			else if(jg.iMirMsg==2)
			{
				NODE_STRU *pNode1 = FindNodeByNo(jg.start_no);
				NODE_STRU *pNode2 = FindNodeByNo(jg.end_no);
				if(pNode1==NULL||pNode2==NULL)
					jg.iMirMsg=0;
				else
				{
					//X-Zƽ���ϵ�����Y��Գ�;Y-Zƽ���ϵ�����X��Գ�
					//ȱʡΪZ��Գ�
					if(fabs((pNode1->pt.x-pNode2->pt.x))<EPS)
						jg.iMirMsg = 1;
					else if(fabs((pNode1->pt.y-pNode2->pt.y))<EPS)
						jg.iMirMsg = 2;
					else
						jg.iMirMsg = 3;
				}
			}
			else if(jg.iMirMsg==4)
				jg.iMirMsg=4;
			else
			{
				sprintf(sError,"������%d-->%dͳ����Ϣ����",jg.start_no,jg.end_no);
				throw sError;
			}
			if(i<ViceJgN-1&&feof(fp))
				throw CString("�������������һ�п�����Ϣ����,���ݴ���");
			jg.No = iJgIndex*10;
			JG_STRU mir_jg_arr[4];
			jg.type = 5;
			MirJg(jg,mir_jg_arr);
			if(jg.iMirMsg<=0)
				jg_arr.Add(mir_jg_arr[0]);
			else if(jg.iMirMsg>0&&jg.iMirMsg<4)
			{
				jg_arr.Add(mir_jg_arr[0]);
				jg_arr.Add(mir_jg_arr[1]);
			}
			else
			{
				for(int k=0;k<4;k++)
					jg_arr.Add(mir_jg_arr[k]);
			}
			iJgIndex++;
			pProcDlg->Refresh(10*(int)(1-(ViceJgN-iNodeIndex)/(double)(FirstNodeN+SecondNodeN+ForceJgN+ViceJgN)));
		}
	}
	catch (char* sError)
	{
		fclose(fp);
		AfxMessageBox(sError);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}
//����ֵ:-1��ʾҪ������ǰ����������Ϣ�ĵ���;0��ʾҪ����;1��ʾ����
static int JustStringType(char *sLineTxt)
{
	char *end_txt = "           TOWER       WEIGHT :  ( Kg )\n";
	char *nouse_title[] = {
	{"\n"},
	{"--------------------------------------------------------------------------------\n"},
	{"   MEMBER       SIZE     LENGTH        BOLT  \n"}};

	for(int i=0;i<3;i++)
	{
		if(strcmp(sLineTxt,nouse_title[i])==0)
			return 0;	//represent skip to next line
	}
	if(strcmp(sLineTxt,end_txt)==0)
		return -1;	//end cursor info-loop
	return 1;	//correct info-line
}
//����Ǹ���Ϣ�����������˨��Ϣ��
void CImportTxtTa::ImportJgInfo(char *filename,CProcBarDlg *pProcDlg)
{
	FILE *fp;
	char sError[200]="";
	JG_STRU jg,*pJgStru;
	//CLDSLineAngle *pJg;
	char mark,cMaterial,line_txt[200];
	char string[100];
	int i,JgN,last_process;
	int bolt_d,bolt_n;
	double wing_wide,wing_thick,LEN,bolt_grade;
	JgN=jg_arr.GetSize();
	if(JgN<=0)	//�Ǹ���Ϊ�㲻����Ǹ���Ϣ����
		return;

	if((fp=fopen(filename,"rt"))==NULL)
	{
		//sprintf(line_txt,"�򲻿�ָ���ļ�");
		//throw line_txt;
		AfxMessageBox("�򲻿�ָ���Ĺ�����Ϣ�ļ�(*.ifo)");
		return;
	}
	try
	{
		for(i=0;;i++)
		{
			fgets(line_txt,200,fp);
			if(feof(fp))	//now is the end of the info-file
				break;
			int iRetCode = JustStringType(line_txt);
			if(iRetCode==0)	//skip to next line
			{
				i--;
				continue;
			}
			else if(iRetCode==-1)	//end cursor info-line
				break;
			sscanf(line_txt,"%d%c%d%s%lf%c%lf%c%lf%lf%c%d%c%d",
				&jg.start_no,&mark,&jg.end_no,&string,&wing_wide,&mark,&wing_thick,
				&cMaterial,&LEN,&bolt_grade,&mark,&bolt_d,&mark,&bolt_n);
			pJgStru = FindJgByStartEndNo(jg.start_no,jg.end_no);
			if(pJgStru)
			{
				pJgStru->wing_wide=wing_wide;
				pJgStru->wing_thick=wing_thick;
				pJgStru->cMaterial=toupper(cMaterial);
				pJgStru->ls_grade = bolt_grade;
				pJgStru->ls_d = bolt_d;
				pJgStru->LsN = bolt_n;
			}
			else
			{
				_snprintf(sError,199,"�Ǹֽڵ������ļ���Ǹ���Ϣ�ļ������,�Ҳ����Ǹ�%d-->%d",
					jg.start_no,jg.end_no);
				throw sError;
			}
			pProcDlg->Refresh(10+10*(i+1)/JgN);
			last_process = 10+10*(i+1)/JgN;
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	fclose(fp);
}


CLDSNode* CImportTxtTa::GetCanStartExtendNode(double wing_wide,double wing_thick,CLDSNode *pStartNode, CLDSNode *pEndNode)
{
	CLDSNode *pExtNode=NULL,*pNode;
	JG_STRU *pJgStru=NULL;
	f3dPoint pick;
	f3dLine line;
	pick = pStartNode->pos;
	line.endPt = pEndNode->pos;
	for(pNode=m_pTa->Node.GetFirst();pNode!=NULL;pNode=m_pTa->Node.GetNext())
	{
		if(pNode==pStartNode||pNode==pEndNode)
			continue;
		line.startPt = pNode->pos;
		if(line.PtInLine(pick)>0)
		{
			pJgStru = FindJgByStartEndNo(
						pStartNode->point_i,pEndNode->point_i);
			if(pJgStru==NULL)
				continue;
			if(fabs(wing_wide-pJgStru->wing_wide)<EPS&&fabs(wing_thick-pJgStru->wing_thick))
				pExtNode = pNode;
			else
				continue;
		}
	}
	return pExtNode;
}

CLDSNode* CImportTxtTa::GetCanEndExtendNode(double wing_wide,double wing_thick,CLDSNode *pStartNode, CLDSNode *pEndNode)
{
	CLDSNode *pExtNode=NULL,*pNode;
	JG_STRU *pJgStru=NULL;
	f3dPoint pick;
	f3dLine line;
	pick = pEndNode->pos;
	line.startPt = pStartNode->pos;
	for(pNode=m_pTa->Node.GetFirst();pNode!=NULL;pNode=m_pTa->Node.GetNext())
	{
		if(pNode==pStartNode||pNode==pEndNode)
			continue;
		line.endPt = pNode->pos;
		if(line.PtInLine(pick)>0)
		{
			pJgStru = FindJgByStartEndNo(
						pStartNode->point_i,pEndNode->point_i);
			if(pJgStru==NULL)
				continue;
			if(fabs(wing_wide-pJgStru->wing_wide)<EPS&&fabs(wing_thick-pJgStru->wing_thick))
				pExtNode = pNode;
			else
				continue;
		}
	}
	return pExtNode;
}

void CImportTxtTa::GetLsInfo(CConnectInfo *ls_group, int start_no, int end_no)
{
	JG_STRU *pJgStru=NULL;
	pJgStru = FindJgByStartEndNo(start_no,end_no);
	if(pJgStru==NULL)
		return;
	sscanf(&pJgStru->LsGuiGe[4],"%d",&ls_group->d);
	ls_group->N = pJgStru->LsN;
}

long CImportTxtTa::GetFatherJgHandle(CLDSNode *pNode)
{
	char sError[200]="";
	CLDSLineAngle *pJg;
	f3dLine jg_line;
	if(pNode->attach_type<0)
	{
		_snprintf(sError,199,"%d�Žڵ������������[%d]�ͳ���",pNode->point_i,pNode->attach_type);
		throw sError;
	}
	if(pNode->attach_type==0)
		return pNode->father_part_handle;
	else	//��������ֵ����������ȷֵ�
	{
		if( pNode->attach_pthandle_arr[0]<0x20||
			pNode->attach_pthandle_arr[1]<0x20)
		{
			_snprintf(sError,199,"%d�Žڵ�������ڵ㲻����,���ݴ���",pNode->point_i);
			throw sError;
		}
		else
		{
			for(pJg=(CLDSLineAngle*)Ta.Parts.GetFirst(CLS_LINEANGLE);pJg!=NULL;pJg=(CLDSLineAngle*)Ta.Parts.GetNext(CLS_LINEANGLE))
			{
				jg_line.startPt = pJg->pStart->pos;
				jg_line.endPt = pJg->pEnd->pos;
				if(jg_line.PtInLine(pNode->pos)>0)
					return pJg->handle;
			}
			return 0;
		}
	}
}

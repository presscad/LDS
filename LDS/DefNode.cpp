//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "Tower.h"
#include "dlg.h"
#include "MirTaAtom.h"
#include "DefCtrlPtDlg.h"
#include "GlobalFunc.h"
#include "UserDefOper.h"
#include "AxisValFixNodeDlg.h"
#include "PromptDlg.h"
#include "env_def.h"
#include "CmdLineDlg.h"
#include "CmdLineDlg.h"
#include "DefScaleNodeDlg.h"
#include "DefOffsetNodeDlg.h"
#include "RelayoutScaleNodeDlg.h"
#include "SnapTypeVerify.h"
#include "SingleRayEndBoltLj.h"
#include "SortFunc.h"
#include "LinkNode.h"

class CCheckOverlappedNode {
public:
	CCheckOverlappedNode() {;}
	POS2KEY keygen;
	CHash64List<LIST_LDSNODEPTR> hashNodesByPos;
	LIST_LDSNODEPTR* AddLdsNode(CLDSNode* pNode)
	{
		__int64 ikey64 = keygen.ToKey64(pNode->xOriginalPos);
		LIST_LDSNODEPTR* pListNode = (LIST_LDSNODEPTR*)hashNodesByPos.GetValue(ikey64);
		if (pListNode == NULL)
		{
			pListNode = hashNodesByPos.Add(ikey64);
			pListNode->pLdsNode = pNode;
		}
		else
			pListNode = (LIST_LDSNODEPTR*)pListNode->AppendListNode(pNode);
		return pListNode;
	}
};
//���彻��ڵ�
static int NewIntersNode(CLDSLinePart *pLinePart1,CLDSLinePart *pLinePart2,CLDSNode *&pNode,
	CCheckOverlappedNode *pCheckOverlap=NULL)
{	
	f3dPoint start[2],end[2],pnt;
	start[0] = pLinePart1->pStart->ActivePosition();
	end[0]   = pLinePart1->pEnd->ActivePosition();
	start[1] = pLinePart2->pStart->ActivePosition();
	end[1]   = pLinePart2->pEnd->ActivePosition();
	if(!pLinePart1->cfgword.And(pLinePart2->cfgword))
		return 0;
	int nRetCode=Int3dll(start[0],end[0],start[1],end[1],pnt);
	if(nRetCode==1)
	{
		if (pCheckOverlap)
		{
			LIST_LDSNODEPTR* pListNode;
			__int64 ikey64 = pCheckOverlap->keygen.ToKey64(pnt);
			if ((pListNode = pCheckOverlap->hashNodesByPos.GetValue(ikey64)) != NULL)
			{
				while (pListNode != NULL)
				{
					CLDSNode* pExistNode = pListNode->pLdsNode;
					pListNode = (LIST_LDSNODEPTR*)pListNode->pNext;
					CLDSNode node;	//��ʱ�ڵ㣬��Ϊ��֤���ִ�ڵ��Ƿ񹲴���ͬһ���� wjh-2019.7.25
					node.SetLayer(pLinePart1->layer());
					node.cfgword = pLinePart1->cfgword&pLinePart2->cfgword;
					if (pExistNode->ModuleTogetherWith(&node) && pExistNode->xOriginalPos.IsEqual(pnt))
					{	//��λ���Ѵ��ڽڵ㣬�����ܲ��ǽ�������ͣ���ʱ��Ҫ�����е�����Ϊ����㣬��ɾ��ǰ�����ڵ�
						pExistNode->m_cPosCalType = 4;
						pExistNode->arrRelationNode[0] = pLinePart1->pStart->handle;
						pExistNode->arrRelationNode[1] = pLinePart1->pEnd->handle;
						pExistNode->arrRelationNode[2] = pLinePart2->pStart->handle;
						pExistNode->arrRelationNode[3] = pLinePart2->pEnd->handle;
						pExistNode->arrRelationPole[0] = pLinePart1->handle;
						pExistNode->arrRelationPole[1] = pLinePart2->handle;
						return 0;
					}
				}
			}
		}
		pNode = console.AppendNode();
		pNode->cfgword=pLinePart1->cfgword&pLinePart2->cfgword;
		pNode->SetPosition(pnt);
		NewNode(pNode);
		pNode->hFatherPart = pLinePart1->handle;
		pNode->SetArcLift(pLinePart1->ArcLiftHandle());
		pNode->Set();
		pNode->iSeg=pLinePart1->iSeg;
		strcpy(pNode->layer(),pLinePart1->layer());
		pNode->layer(1) = 'P';
		pNode->arrRelationNode[0] = pLinePart1->pStart->handle;
		pNode->arrRelationNode[1] = pLinePart1->pEnd->handle;
		pNode->arrRelationNode[2] = pLinePart2->pStart->handle;
		pNode->arrRelationNode[3] = pLinePart2->pEnd->handle;
		//pNode->arrRelationNode[0] = pLinePart1->pStart->handle;
		pNode->arrRelationPole[0] = pLinePart1->handle;
		pNode->arrRelationPole[1] = pLinePart2->handle;
		pNode->m_cPosCalType = 4;//3:���Ǹֽ��潻��
		pNode->Set();
#if defined(__LDS_)||defined(__TSA_)
		pLinePart1->CalLenCoef.hRefPole=pLinePart2->handle;
		pLinePart2->CalLenCoef.hRefPole=pLinePart1->handle;
#endif
	}
	return nRetCode;
}

static CMirMsgDlg mir_dlg;
static CDefScaleNodeDlg divide_dlg;
static CAxisValFixNodeDlg fix_axisval_dlg;
void CLDSView::OnDefCtrlPt() 
{	//������ƽڵ�
	m_nPrevCommandID=ID_DEF_CTRL_PT;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat definition control point";
#else
	m_sPrevCommandName="�ظ�������Ƶ�";
#endif
	Command("ControlNode");
}

int CLDSView::DefControlNode()
{	//������ƽڵ�
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());		//�Զ��л���Դ���
	//HINSTANCE hOldInstance = AfxGetResourceHandle();	//�õ���ǰ��Դ���
    //AfxSetResourceHandle(GetModuleHandle("LmaDlg"));//����ǰ��Դ���ָ��DLL�ļ�
	CDefCtrlPtDlg def_ctrlpt_dlg;
	long i;
	CLDSNode *pNode;
	f3dPoint node_pos;
	if(def_ctrlpt_dlg.DoModal()!=IDOK)
	{
		//AfxSetResourceHandle(hOldInstance);	//�ָ���Դ��� ������ʱ�ָ���Դ����ͻᱨ�����δ�ձ�ʹ�� wht 09-09-04
		return 0;
	}
	Ta.BeginUndoListen();
	try
	{
		for(i=0;i<def_ctrlpt_dlg.m_nNodeNum;i++)
		{
			CDefCtrlPtDlg::CTRL_PT_COORD& ctrl_coord = def_ctrlpt_dlg.DataList[i];
			pNode = console.AppendNode();
			pNode->SetLayer(theApp.env.layer);
			pNode->layer(1)='P';
			pNode->m_cPosCalType = 0;//���κ�����
			
			pNode->SetPosition(ctrl_coord.fPosX,ctrl_coord.fPosY,ctrl_coord.fPosZ);
			NewNode(pNode);
			MIRMSG msg;
			if(ctrl_coord.iMirMsg==1)
				msg.axis_flag=1;
			else if(ctrl_coord.iMirMsg==2)
				msg.axis_flag=2;
			else if(ctrl_coord.iMirMsg==3)
				msg.axis_flag=4;
			else if(ctrl_coord.iMirMsg==4)
				msg.axis_flag=7;
			MirTaAtom(pNode,msg);
		}
		if(m_pDoc)
			m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	Ta.EndUndoListen();
	return 0;
	//AfxSetResourceHandle(hOldInstance);	//�ָ���Դ��� ������ʱ�ָ���Դ����ͻᱨ�����δ�ձ�ʹ�� wht 09-09-04
}

void CLDSView::OnDefOffsetNode() 
{
	m_nPrevCommandID=ID_DEF_OFFSET_NODE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat definition offset point";
#else
	m_sPrevCommandName="�ظ�����ƫ�Ƶ�";
#endif
	Command("OffsetNode");
}

int CLDSView::DefOffsetNode()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	BOOL bRet=FinishDefOffsetNode();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("����:","");
#endif
	if(bRet)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		Command("OffsetNode");
	}
	return 0;
}
//<DEVELOP_PROCESS_MARK nameId="CLDSView.OnDefIntersNode">
struct INTERSNODE_DEFPARA{
	CLDSNode* pRod1Start,*pRod1End;
	CLDSNode* pRod2Start,*pRod2End;
	CLDSLinePart* pRod1,*pRod2;
	INTERSNODE_DEFPARA(){pRod1Start=pRod1End=pRod2Start=pRod2End=NULL;pRod1=pRod2=NULL;}
};
/*int inputSchemaType[4][4]={{SNAP_LINE,SNAP_LINE,0,0},
						   {SNAP_LINE,SNAP_POINT,SNAP_POINT,0},
						   {SNAP_POINT,SNAP_POINT,SNAP_LINE,0},
						   {SNAP_POINT,SNAP_POINT,SNAP_POINT,SNAP_POINT}};*/
const BYTE TWO_LINE			  = 0x01;
const BYTE ONE_LINE_TWO_POINT = 0x02;
const BYTE TWO_POINT_ONE_LINE = 0x04;
const BYTE FOUR_POINT         = 0x08;
BYTE schemaType;
int	nInputSchemaType;
//����������0.��ʾ����ʧ�� -1.�����߱�ִ����������������趨ϵͳ��������  ����.����ȷ������������
static int FinishSelectObjSnapType(DBOBJSET& setSnapObjects,CString& cmdStr)
{
	if(setSnapObjects.GetNodeNum()==0)
	{
		schemaType=0;
		nInputSchemaType=0;
		g_pSolidSnap->SetSnapType(SNAP_POINT|SNAP_LINE);
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("please select the first rod or starting node:");
#else
		cmdStr.Format("��ѡ���һ���˼�����ʼ�ڵ�:");
#endif
		return -1;
	}
	if(nInputSchemaType>0)
		return nInputSchemaType;
	if(setSnapObjects.GetNodeNum()==1)
	{	
		if(setSnapObjects[0] && ((CLDSPart*)setSnapObjects[0])->IsLinePart())
		{
			schemaType|=TWO_LINE;
			schemaType|=ONE_LINE_TWO_POINT;
			g_pSolidSnap->SetSnapType(SNAP_POINT|SNAP_LINE);
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please select the second rod or starting node:");
#else
			cmdStr.Format("��ѡ��ڶ����˼�����ʼ�ڵ�:");
#endif
			return -1;
		}
		else if(setSnapObjects[0]->GetClassTypeId()==CLS_NODE)
		{
			schemaType|=TWO_POINT_ONE_LINE;
			schemaType|=FOUR_POINT;
			g_pSolidSnap->SetSnapType(SNAP_POINT);
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please select the end node:");
#else
			cmdStr.Format("��ѡ����Ӧ����ֹ�ڵ�:");
#endif
			return -1;
		}
		else
			return 0;
	}
	if(setSnapObjects.GetNodeNum()==2)
	{
		if(setSnapObjects[1] && setSnapObjects[1]->GetClassTypeId()==CLS_NODE)
		{
			if(schemaType&TWO_LINE && schemaType&ONE_LINE_TWO_POINT)
			{
				nInputSchemaType=ONE_LINE_TWO_POINT;
				g_pSolidSnap->SetSnapType(SNAP_POINT);
#ifdef AFX_TARG_ENU_ENGLISH
				cmdStr.Format("please select the end node:");
#else
				cmdStr.Format("��ѡ����Ӧ����ֹ�ڵ�:");
#endif
				return -1;
			}
			else
			{
				g_pSolidSnap->SetSnapType(SNAP_POINT|SNAP_LINE);
#ifdef AFX_TARG_ENU_ENGLISH
				cmdStr.Format("please select the second rod or starting node:");
#else
				cmdStr.Format("��ѡ��ڶ����˼�����ʼ�ڵ�:");
#endif
				return -1;
			}
		}
		else if(((CLDSPart*)setSnapObjects[1])->IsLinePart() && schemaType&TWO_LINE)
			nInputSchemaType=TWO_LINE;
		else
			return 0;
	}
	if(setSnapObjects.GetNodeNum()==3)
	{
		if(schemaType&TWO_LINE || schemaType&ONE_LINE_TWO_POINT)
			return 0;
		if(setSnapObjects[2] && setSnapObjects[2]->GetClassTypeId()==CLS_NODE)
		{
			nInputSchemaType=FOUR_POINT;
			g_pSolidSnap->SetSnapType(SNAP_POINT);
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please select the end node:");
#else
			cmdStr.Format("��ѡ����Ӧ����ֹ�ڵ�:");
#endif
			return -1;
		}
		else if(((CLDSPart*)setSnapObjects[2])->IsLinePart() && schemaType&TWO_POINT_ONE_LINE)
			nInputSchemaType=TWO_POINT_ONE_LINE;
		else
			return 0;
	}
	return nInputSchemaType;
}
//�ж����ڵ��Ƿ���ͬһ�˼���
static BOOL IsInSameLinePart(CLDSNode* pNode1,CLDSNode* pNode2,THANDLE* pHandle=NULL)
{
	THANDLE hLinePart=0;
	if(pNode1->handle==pNode2->handle)
		return FALSE;
	if(pNode1->hFatherPart==pNode2->hFatherPart)
	{
		hLinePart=pNode1->hFatherPart;
		if(pHandle)
			*pHandle=hLinePart;
		return TRUE;
	}
	for(CLDSLinePart* pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
	{
		if(!pLinePart->cfgword.And(pNode1->cfgword) || !pLinePart->cfgword.And(pNode2->cfgword))
			break;
		if(pLinePart->pStart==pNode1)
		{
			if(pLinePart->pEnd==pNode2 || pNode2->hFatherPart==pLinePart->handle)
			{
				hLinePart=pLinePart->handle;
				break;
			}
		}	
		else if(pLinePart->pStart==pNode2)
		{
			if(pLinePart->pEnd==pNode1 || pNode1->hFatherPart==pLinePart->handle)
			{
				hLinePart=pLinePart->handle;
				break;
			}
		}
		else if(pLinePart->pEnd==pNode1)
		{
			if(pLinePart->pStart==pNode2 || pNode2->hFatherPart==pLinePart->handle)
			{
				hLinePart=pLinePart->handle;
				break;
			}
		}
		else if(pLinePart->pEnd==pNode2)
		{
			if(pLinePart->pStart==pNode1 || pNode1->hFatherPart==pLinePart->handle)
			{
				hLinePart=pLinePart->handle;
				break;
			}
		}
	}
	if(hLinePart>0x20)
	{
		if(pHandle)
			*pHandle=hLinePart;
		return TRUE;
	}
	return FALSE;
}
//��ʼ������ڵ�Ķ������
static int InitIntersNodeDefPara(DBOBJSET& setSnapObjects,INTERSNODE_DEFPARA& nodeDefPara)
{
	THANDLE hLinePart=0;
	if(nInputSchemaType==TWO_LINE)
	{
		nodeDefPara.pRod1=(CLDSLinePart*)setSnapObjects[0];
		nodeDefPara.pRod1Start=nodeDefPara.pRod1->pStart;
		nodeDefPara.pRod1End=nodeDefPara.pRod1->pEnd;
		nodeDefPara.pRod2=(CLDSLinePart*)setSnapObjects[1];
		nodeDefPara.pRod2Start=nodeDefPara.pRod2->pStart;
		nodeDefPara.pRod2End=nodeDefPara.pRod2->pEnd;
	}
	else if(nInputSchemaType==ONE_LINE_TWO_POINT)
	{
		nodeDefPara.pRod1=(CLDSLinePart*)setSnapObjects[0];
		nodeDefPara.pRod1Start=nodeDefPara.pRod1->pStart;
		nodeDefPara.pRod1End=nodeDefPara.pRod1->pEnd;
		nodeDefPara.pRod2=NULL;
		nodeDefPara.pRod2Start=(CLDSNode*)setSnapObjects[1];
		nodeDefPara.pRod2End=(CLDSNode*)setSnapObjects[2];
		if(nodeDefPara.pRod2Start==nodeDefPara.pRod2End)
			return -2;
		if(IsInSameLinePart(nodeDefPara.pRod2Start,nodeDefPara.pRod2End,&hLinePart))
			nodeDefPara.pRod2=(CLDSLinePart*)Ta.FromPartHandle(hLinePart,CLS_LINEPART);
	}
	else if(nInputSchemaType==TWO_POINT_ONE_LINE)
	{
		nodeDefPara.pRod1=NULL;
		nodeDefPara.pRod1Start=(CLDSNode*)setSnapObjects[0];
		nodeDefPara.pRod1End=(CLDSNode*)setSnapObjects[1];
		nodeDefPara.pRod2=(CLDSLinePart*)setSnapObjects[2];
		nodeDefPara.pRod2Start=nodeDefPara.pRod2->pStart;
		nodeDefPara.pRod2End=nodeDefPara.pRod2->pEnd;
		if(nodeDefPara.pRod1Start==nodeDefPara.pRod1End)
			return -2;
		if(IsInSameLinePart(nodeDefPara.pRod1Start,nodeDefPara.pRod1End,&hLinePart))
			nodeDefPara.pRod1=(CLDSLinePart*)Ta.FromPartHandle(hLinePart,CLS_LINEPART);
	}
	else if(nInputSchemaType==FOUR_POINT)
	{
		nodeDefPara.pRod1=nodeDefPara.pRod2=NULL;
		nodeDefPara.pRod1Start=(CLDSNode*)setSnapObjects[0];
		nodeDefPara.pRod1End=(CLDSNode*)setSnapObjects[1];
		nodeDefPara.pRod2Start=(CLDSNode*)setSnapObjects[2];
		nodeDefPara.pRod2End=(CLDSNode*)setSnapObjects[3];
		if(nodeDefPara.pRod1Start==nodeDefPara.pRod1End ||
			nodeDefPara.pRod2Start==nodeDefPara.pRod2End)
			return -2;
		if(IsInSameLinePart(nodeDefPara.pRod1Start,nodeDefPara.pRod1End,&hLinePart))
			nodeDefPara.pRod1=(CLDSLinePart*)Ta.FromPartHandle(hLinePart,CLS_LINEPART);
		if(IsInSameLinePart(nodeDefPara.pRod2Start,nodeDefPara.pRod2End,&hLinePart))
			nodeDefPara.pRod2=(CLDSLinePart*)Ta.FromPartHandle(hLinePart,CLS_LINEPART);
	}
	else
		return FALSE;
	if(nodeDefPara.pRod1==NULL && nodeDefPara.pRod2)
	{
		nodeDefPara.pRod1=nodeDefPara.pRod2;
		nodeDefPara.pRod2=NULL;
	}
	if(!nodeDefPara.pRod1Start->ModuleTogetherWith(nodeDefPara.pRod1End) ||
	   !nodeDefPara.pRod2Start->ModuleTogetherWith(nodeDefPara.pRod2End) ||
	   !nodeDefPara.pRod1Start->ModuleTogetherWith(nodeDefPara.pRod2Start))	//����ͬһģ����
	{
#ifdef AFX_TARG_ENU_ENGLISH
	   logerr.Log("Starting point and ending point does not exit in the same model");
#else
	   logerr.Log("�˼���ĩ�˽ڵ㲻������ͬһģ����");
#endif
	   return 0;
	}
	if(nodeDefPara.pRod1&&nodeDefPara.pRod2&&nodeDefPara.pRod1->IsAngle()&&nodeDefPara.pRod2->IsAngle())
	{	//��֤��һ���˼�(�����㸸�˼�)Ϊ����,������֤�������˨��λ��׼Ϊ����������������ƫ������ɵ���˨��λ��� wht 16-09-30
		int fatherAngleHandleArr[4]={nodeDefPara.pRod1->pStart?nodeDefPara.pRod1->pStart->hFatherPart:0,
									 nodeDefPara.pRod1->pEnd?nodeDefPara.pRod1->pEnd->hFatherPart:0,
									 nodeDefPara.pRod2->pStart?nodeDefPara.pRod2->pStart->hFatherPart:0,
									 nodeDefPara.pRod2->pEnd?nodeDefPara.pRod2->pEnd->hFatherPart:0};
		GEPOINT work_norm=(nodeDefPara.pRod1->End()-nodeDefPara.pRod1->Start())^(nodeDefPara.pRod2->End()-nodeDefPara.pRod2->Start());
		normalize(work_norm);
		//���ݽǸ�ͼ��ȷ���Ǹ����ڵ���ͼ���򣬵���work_norm��֤�ڵ㸸�˼�Ϊ����
		GEPOINT view_norm=work_norm;
		if(toupper(nodeDefPara.pRod1->layer(2))=='Q')
			view_norm.Set(0,1,0);
		else if(toupper(nodeDefPara.pRod1->layer(2))=='H')
			view_norm.Set(0,-1,0);
		else if(toupper(nodeDefPara.pRod1->layer(2))=='Z')
			view_norm.Set(-1,0,0);
		else if(toupper(nodeDefPara.pRod1->layer(2))=='Y')
			view_norm.Set(1,0,0);
		else if(toupper(nodeDefPara.pRod1->layer(2))=='G')
			view_norm.Set(0,0,1);
		work_norm*=(work_norm*view_norm>0)?1:-1;
		BOOL bReliableWorkNorm=FALSE;
		for(int i=0;i<4;i++)
		{
			CLDSLineAngle *pAngle=(CLDSLineAngle*)console.FromPartHandle(fatherAngleHandleArr[i],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pAngle==NULL)
				continue;
			double ddx=pAngle->vxWingNorm*work_norm,ddy=pAngle->vyWingNorm*work_norm;
			if((fabs(ddx)>fabs(ddy)&&ddx<0)||(fabs(ddy)>fabs(ddx)&&ddy<0))
			{
				if(bReliableWorkNorm)
				{	//��ǰһ����׼�Ǹ�֫������ͬ����������׼�˼�λ��
					bReliableWorkNorm=FALSE;
					break;
				}
				work_norm*=-1.0;
			}
			bReliableWorkNorm=TRUE;
		}
		if(bReliableWorkNorm)
		{	
			CLDSLineAngle *pAngle2=(CLDSLineAngle*)nodeDefPara.pRod2;
			double ddx=pAngle2->vxWingNorm*work_norm,ddy=pAngle2->vyWingNorm*work_norm;
			if((fabs(ddx)>fabs(ddy)&&ddx>0)||(fabs(ddy)>fabs(ddx)&&ddy>0))
			{	//�ڶ����Ǹ�Ϊ����
				nodeDefPara.pRod2=nodeDefPara.pRod1;
				nodeDefPara.pRod1=pAngle2;
				nodeDefPara.pRod1Start=nodeDefPara.pRod1->pStart;
				nodeDefPara.pRod1End=nodeDefPara.pRod1->pEnd;
				nodeDefPara.pRod2Start=nodeDefPara.pRod2->pStart;
				nodeDefPara.pRod2End=nodeDefPara.pRod2->pEnd;
			}
		}
	}
	return 1;
}
//���������߶εĿռ�λ�ù�ϵ
static int CalTwoLineRelation(INTERSNODE_DEFPARA& nodeDefPara)
{
	f3dPoint pt,pt1,pt2,pt3,pt4;
	pt1=nodeDefPara.pRod1Start->Position();
	pt2=nodeDefPara.pRod1End->Position();
	pt3=nodeDefPara.pRod2Start->Position();
	pt4=nodeDefPara.pRod2End->Position();
	f3dPoint vec,vec_1,vec_2,norm;
	vec_1=pt1-pt3;
	vec_2=pt2-pt3;
	norm=vec_1^vec_2;
	if(!normalize(norm))	//���˼�����
		return 0;
	vec=pt1-pt4;
	normalize(vec);
	if(fabs(vec*norm)>eps)	//����
		return 2;
	f3dLine line_1(pt1,pt2);
	f3dLine line_2(pt3,pt4);
	int iRet=Int3dll(line_1,line_2,pt);
	if(iRet==1)			//�ཻ�ҽ���Ϊ�ڵ�
		return 1;
	else if(iRet==0)
		return 2;		//����δ�ܼ���������΢����
	else				//����Ϊ��Ч��
		return 0; 
}
//������ͨ�����
BOOL FinishDefIntersNode(INTERSNODE_DEFPARA& nodeDefPara,int nCalType)
{
	if(mir_dlg.DoModal()!=IDOK)	//�Գ�
		return FALSE;
	Ta.BeginUndoListen();
	CLDSNode *pNode=console.AppendNode();
	pNode->cfgword=nodeDefPara.pRod1Start->cfgword;
	if(nodeDefPara.pRod1)
	{
		pNode->hFatherPart = nodeDefPara.pRod1->handle;
		pNode->SetArcLift(nodeDefPara.pRod1->ArcLiftHandle());
	}
	pNode->arrRelationNode[0] = nodeDefPara.pRod1Start->handle;
	pNode->arrRelationNode[1] = nodeDefPara.pRod1End->handle;
	pNode->arrRelationNode[2] = nodeDefPara.pRod2Start->handle;
	pNode->arrRelationNode[3] = nodeDefPara.pRod2End->handle;
	//
	if(nodeDefPara.pRod1)
		pNode->arrRelationPole[0] = nodeDefPara.pRod1->handle;
	if(nodeDefPara.pRod2)
		pNode->arrRelationPole[1] = nodeDefPara.pRod2->handle;
	pNode->m_cPosCalType = 4;			//���潻��
	pNode->m_cPosCalViceType=nCalType;	//ͶӰ����
	if(!pNode->Set())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("Intersection node 0x%x 's position calculate failed",pNode->handle);
#else
		logerr.Log("����ڵ�0x%x ��λ�ü���ʧ��",pNode->handle);
#endif
		return FALSE;
	}
	NewNode(pNode);
	pNode->iSeg=nodeDefPara.pRod1Start->iSeg;
	if(nodeDefPara.pRod1)
		strcpy(pNode->layer(),nodeDefPara.pRod1->layer());
	else
		strcpy(pNode->layer(),nodeDefPara.pRod1Start->layer());
	pNode->layer(1) = 'P';
#if defined(__LDS_)||defined(__TSA_)
	if(nodeDefPara.pRod1 && nodeDefPara.pRod2)
	{
		nodeDefPara.pRod1->CalLenCoef.hRefPole=nodeDefPara.pRod2->handle;
		nodeDefPara.pRod2->CalLenCoef.hRefPole=nodeDefPara.pRod1->handle;
	}
#endif
	MirTaAtom(pNode,mir_dlg.mirmsg);
	Ta.EndUndoListen();
	return TRUE;
}
//
void CLDSView::OnDefIntersNode() 
{
	m_nPrevCommandID=ID_DEF_INTERS_NODE;
	m_curTask = TASK_DEF_INTERS_NODE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to define cross-bracing point";
#else
	m_sPrevCommandName="�ظ����彻���";
#endif
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	DefIntersNode();
	//Command("IntersNode");
}
int CLDSView::DefIntersNode()
{
	//��������
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr;
	g_pSolidSet->SetOperType(OPER_OSNAP);	//�趨���Ϊ��׽״̬
	int retCode=FinishSelectObjSnapType(m_setSnapObjects,cmdStr);
	if(cmdStr.GetLength()>0)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		return -1;
	}
	g_pSolidSet->SetOperType(OPER_OTHER);	//�ָ������ʾ״̬
	//��ʼ������ڵ㶨�����
	INTERSNODE_DEFPARA nodeDefPara;
	retCode=InitIntersNodeDefPara(m_setSnapObjects,nodeDefPara);
	if(retCode>0) 
	{	//�ж������ȷ����ֱ�ߵĹ�ϵ
		int nProjectionType=0;
		int nRet=CalTwoLineRelation(nodeDefPara);	
		if(nRet<=0)
			retCode=nRet;
		else if(nRet==2)	//����
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("input projection type of different plane intersection:[0.cross-bracing plane,1.X-Z plane,2.Y-Z plane,3.X-Y plane]<1>:","");
#else
			pCmdLine->FillCmdLine("���������󽻵�ͶӰ����:[0.������,1.X-Zƽ��,2.Y-Zƽ��,3.X-Yƽ��]<1>��","");
#endif
			CString cmdStr;
			while(1)
			{
				if(!pCmdLine->GetStrFromCmdLine(cmdStr))
				{
					pCmdLine->CancelCmdLine();
					return 0;
				}
				if(strlen(cmdStr)==0)
					nProjectionType=1;
				else
					nProjectionType=atoi(cmdStr);
				if(nProjectionType<0 || nProjectionType>3)
				{
					pCmdLine->CancelCmdLine();
					return 0;
				}
				break;
			}
			pCmdLine->FinishCmdLine();
		}
		if(nRet>0)
		{	//��ɽ���ڵ㶨��
			retCode=FinishDefIntersNode(nodeDefPara,nProjectionType);
			m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The two selected cross-bracing line are collinear, unable to define any form of cross-bracing line!");
#else
			AfxMessageBox("ѡ�����������߹����غ�, �޷������κ���ʽ�Ľ����!");
#endif
			retCode=1;
		}
	}
	ReleaseFunc();
	//����չ������������״̬
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select the first rod or the rod node:","");
#else
	pCmdLine->FillCmdLine("��ѡ���һ���˼�����˽ڵ�:","");
#endif
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	m_setSnapObjects.Empty();
	schemaType=0;
	nInputSchemaType=0;
	return retCode;
}
//</DEVELOP_PROCESS_MARK>
void CLDSView::OnDefHorizonNode() 
{
	m_nPrevCommandID=ID_DEF_HORIZON_NODE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to define same height node";
#else
	m_sPrevCommandName="�ظ�����ȸߵ�";
#endif
	Command("HorizonNode");
}

int CLDSView::DefHorizonNode()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	BOOL bRet=FinishDefHorizonNode();
	pCmdLine->FinishCmdLine();
	if(bRet)
	{
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("command:","");
#else
		pCmdLine->FillCmdLine("����:","");
#endif
	}
	return 0;
}

void CLDSView::OnDefAxisValFixNode() 
{
	m_nPrevCommandID=ID_DEF_AXIS_VAL_FIX_NODE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to define the specified coordinate component point";
#else
	m_sPrevCommandName="�ظ�����ָ�����������";
#endif
	Command("FixCoordNode");
}

int CLDSView::DefFixCoordNode()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	BOOL bRet=FinishDefAxisValFixNode();
	pCmdLine->FinishCmdLine();
	if(bRet)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		Command("FixCoordNode");
	}
	return 0;
	//pCmdLine->FillCmdLine("����:","");
}

//����˼��ȷֵ�
BOOL CLDSView::FinishDefScaleNodeByLinePart(CLDSLinePart *pLinePart/*=NULL*/)
{
	if(pLinePart==NULL)
	{	
		CLDSPart *pPart=NULL;
		CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select a rod to be average separated:","");
#else
		pCmdLine->FillCmdLine("��ѡ��һ�����ȷָ˼�:","");
#endif
		while(1)
		{
			f3dLine line;
			int ret = g_pSolidSnap->SnapLine(line);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			else if(ret>0)
			{
				pPart=console.FromPartHandle(line.ID);
				if(pPart&&pPart->IsLinePart())
					pLinePart=(CLDSLinePart*)pPart;
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(line.ID);
				if(pLinePart)
					break;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("No proper rod is selected!","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("please select a rod to be average separated:","");
#else
			pCmdLine->FillCmdLine("û��ѡ�к��ʵĸ˼���","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("��ѡ��һ�����ȷָ˼�:","");
#endif
		}
	}
	int i;
	CLDSNode* pNowNode;
	f3dPoint start,end,vec,tem_pt;
	double jg_len,attach_scale;

	///////////////��׽����/////////////////////////
	if(divide_dlg.DoModal()!=IDOK)
		return FALSE;
	CUndoOperObject undo(&Ta,FALSE);
	try
	{
		start = pLinePart->pStart->ActivePosition();
		end   = pLinePart->pEnd->ActivePosition();
		Sub_Pnt(vec,end,start);
		jg_len = LENGTH(vec);
		normalize(vec);
		NODESET nodeset;
		for(i=1;i<divide_dlg.m_nDivideByN;i++)	//n�ȷ�ʱ��n-1���ȷֵ�
		{
			attach_scale = i/(double)divide_dlg.m_nDivideByN;
			Add_Pnt(tem_pt,start,vec*attach_scale*jg_len);
			pNowNode = console.AppendNode();
			pNowNode->hFatherPart = pLinePart->handle;
			pNowNode->cfgword = pLinePart->cfgword;
			pNowNode->SetPosition(tem_pt);
			NewNode(pNowNode);
			pNowNode->SetArcLift(pLinePart->ArcLiftHandle());
			pNowNode->m_cPosCalType = 5;  //�����ȷֵ�  WJH--00.07.20
			pNowNode->attach_scale = attach_scale;
			pNowNode->arrRelationNode[0] = pLinePart->pStart->handle;
			pNowNode->arrRelationNode[1] = pLinePart->pEnd->handle;
			pNowNode->arrRelationPole[0] = pLinePart->handle;
			pNowNode->iSeg=pLinePart->iSeg;
			strcpy(pNowNode->layer(), pLinePart->layer());
			pNowNode->layer(1) = 'P';
			nodeset.append(pNowNode);
		}
		//����ԭʼ�ڵ�������Ϻ�һ��Գƣ��������Է�ֹԭʼ�ڵ�����������Գƽڵ����� wjh-2012.2.19
		for(CLDSNode *pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
			MirTaAtom(pNode,divide_dlg.mirmsg);
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	return TRUE;
}

BOOL CLDSView::FinishDefAxisValFixNode()
{
	CLDSLinePart* pSelLinePart=NULL;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSArcPart* pSelArcPart=NULL;
#endif
	CLDSNode* pNowNode=NULL;
	CLDSPart *pPart = NULL;
	f3dPoint start,end,vec,tem_pt;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select a rod:","");
#else
	pCmdLine->FillCmdLine("��ѡ��һ���˼�:","");
#endif
	while(1)
	{
		f3dLine line;
		int ret = g_pSolidSnap->SnapLine(line);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		else if(ret>0)
		{
			pPart=console.FromPartHandle(line.ID);
			if(pPart)
			{
				if(pPart->IsLinePart())
					pSelLinePart=(CLDSLinePart*)pPart;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
				else if(pPart->IsArcPart())
					pSelArcPart=(CLDSArcPart*)pPart;
#endif
			}
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(line.ID);
			if(pSelLinePart)
				break;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			if(pSelArcPart)
				break;
#endif
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("No proper rod is selected!","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("Please select a rod:","");
#else
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĸ˼���","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("��ѡ��һ���˼�:","");
#endif
	}
	///////////////��׽����/////////////////////////
	if(pSelLinePart)
	{
		if(fabs(pSelLinePart->pStart->Position(false).x-pSelLinePart->pEnd->Position(false).x)<EPS)
			fix_axisval_dlg.m_bCanFixCoordX=FALSE;
		else
			fix_axisval_dlg.m_bCanFixCoordX=TRUE;
		if(fabs(pSelLinePart->pStart->Position(false).y-pSelLinePart->pEnd->Position(false).y)<EPS)
			fix_axisval_dlg.m_bCanFixCoordY=FALSE;
		else
			fix_axisval_dlg.m_bCanFixCoordY=TRUE;
		if(fabs(pSelLinePart->pStart->Position(false).z-pSelLinePart->pEnd->Position(false).z)<EPS)
			fix_axisval_dlg.m_bCanFixCoordZ=FALSE;
		else
			fix_axisval_dlg.m_bCanFixCoordZ=TRUE;
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pSelArcPart)
	{
		if(fabs(pSelArcPart->WorkNorm().x)>EPS_COS)
			fix_axisval_dlg.m_bCanFixCoordX=FALSE;
		else
			fix_axisval_dlg.m_bCanFixCoordX=TRUE;
		if(fabs(pSelArcPart->WorkNorm().y)>EPS_COS)
			fix_axisval_dlg.m_bCanFixCoordY=FALSE;
		else
			fix_axisval_dlg.m_bCanFixCoordY=TRUE;
		if(fabs(pSelArcPart->WorkNorm().z)>EPS_COS)
			fix_axisval_dlg.m_bCanFixCoordZ=FALSE;
		else
			fix_axisval_dlg.m_bCanFixCoordZ=TRUE;
	}
#endif
	if(fix_axisval_dlg.DoModal()!=IDOK)
		return FALSE;
	//�Գ�
	if(pSelLinePart&&mir_dlg.DoModal()!=IDOK)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("cancel operation");
#else
		MessageBox("����ȡ��");
#endif
		return FALSE;
	}
	CUndoOperObject undo(&Ta,true);
	try
	{
		if(pSelLinePart)
		{
			pNowNode = console.AppendNode();
			pNowNode->hFatherPart = pSelLinePart->handle;
			pNowNode->cfgword = pSelLinePart->cfgword;
			pNowNode->arrRelationNode[0]=pSelLinePart->pStart->handle;
			pNowNode->arrRelationNode[1]=pSelLinePart->pEnd->handle;
			pNowNode->arrRelationPole[0] = pSelLinePart->handle;
			if(fix_axisval_dlg.m_iFixAxis==0)	//X����ֵ�����
			{
				pNowNode->m_cPosCalType=1;
				pNowNode->SetPosition(fix_axisval_dlg.m_fFixValX,0,0);
			}
			else if(fix_axisval_dlg.m_iFixAxis==1)	//Y����ֵ�����
			{
				pNowNode->m_cPosCalType=2;
				pNowNode->SetPosition(0,fix_axisval_dlg.m_fFixValY,0);
			}
			else	//Z����ֵ�����
			{
				pNowNode->m_cPosCalType=3;
				pNowNode->SetPosition(0,0,fix_axisval_dlg.m_fFixValZ);
			}
			pNowNode->Set();
			NewNode(pNowNode);
			pNowNode->SetArcLift(pSelLinePart->ArcLiftHandle());
			pNowNode->iSeg=pSelLinePart->iSeg;
			strcpy(pNowNode->layer(), pSelLinePart->layer());
			pNowNode->layer(1) = 'P';
			MirTaAtom(pNowNode,mir_dlg.mirmsg);
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pSelArcPart)
		{
			//����Բ����ָ��ĳһ��������Ľڵ�λ��
			UCS_STRU arcucs;
			arcucs.origin=pSelArcPart->baseline.Center();
			arcucs.axis_z=pSelArcPart->baseline.WorkNorm();
			arcucs.axis_x=pSelArcPart->baseline.PositionInAngle(0)-arcucs.origin;
			normalize(arcucs.axis_x);
			arcucs.axis_y=arcucs.axis_z^arcucs.axis_x;
			f3dPoint chord_vec,chord_norm,inters;
			if(fix_axisval_dlg.m_iFixAxis==0)	//X����ֵ�����
			{
				chord_vec.Set(0,-arcucs.axis_z.z,arcucs.axis_z.y);
				chord_norm=arcucs.axis_z^chord_vec;
				Int3dlf(inters,arcucs.origin,chord_norm,f3dPoint(fix_axisval_dlg.m_fFixValX,0,0),f3dPoint(1,0,0));
			}
			else if(fix_axisval_dlg.m_iFixAxis==1)	//Y����ֵ�����
			{
				chord_vec.Set(arcucs.axis_z.z,0,-arcucs.axis_z.x);
				chord_norm=arcucs.axis_z^chord_vec;
				Int3dlf(inters,arcucs.origin,chord_norm,f3dPoint(0,fix_axisval_dlg.m_fFixValY,0),f3dPoint(0,1,0));
			}
			else	//Z����ֵ�����
			{
				chord_vec.Set(-arcucs.axis_z.y,arcucs.axis_z.x,0);
				chord_norm=arcucs.axis_z^chord_vec;
				Int3dlf(inters,arcucs.origin,chord_norm,f3dPoint(0,0,fix_axisval_dlg.m_fFixValZ),f3dPoint(0,0,1));
			}
			chord_norm=inters-arcucs.origin;
			double dist=chord_norm.mod();
			double radius=pSelArcPart->baseline.Radius();
			if(dist==radius)
			{
				pNowNode = console.AppendNode();
				pNowNode->SetPosition(inters);
			}
			else if(dist<radius)
			{
				normalize(chord_vec);
				f3dPoint offset_vec=chord_vec*sqrt(radius*radius-dist*dist);
				f3dPoint inters1=inters+offset_vec;
				f3dPoint inters2=inters-offset_vec;
				inters=inters1;
				coord_trans(inters,arcucs,FALSE);
				double angle=Cal2dLineAng(0,0,inters.x,inters.y);
				if(angle<pSelArcPart->baseline.SectorAngle())
				{
					pNowNode=console.AppendNode();
					pNowNode->SetPosition(inters1);
				}
				inters=inters2;
				coord_trans(inters,arcucs,FALSE);
				angle=Cal2dLineAng(0,0,inters.x,inters.y);
				if(angle<pSelArcPart->baseline.SectorAngle())
				{
					CLDSNode* pSecNode=console.AppendNode();
					pSecNode->SetPosition(inters2);
					if(pNowNode==NULL)
						pNowNode=pSecNode;
					else
					{
						pSecNode->hFatherPart = pSelArcPart->handle;
						pSecNode->cfgword = pSelArcPart->cfgword;
						pSecNode->m_cPosCalType=0;	//�������κ�������
						NewNode(pSecNode);
						pSecNode->SetArcLift(pSelArcPart->ArcLiftHandle());
						pSecNode->iSeg=pSelArcPart->iSeg;
						strcpy(pSecNode->layer(), pSelArcPart->layer());
						pSecNode->layer(1) = 'P';
					}
				}
			}
			if(pNowNode!=NULL)
			{
				pNowNode->hFatherPart = pSelArcPart->handle;
				pNowNode->cfgword = pSelArcPart->cfgword;
				pNowNode->m_cPosCalType=0;	//�������κ�������
				NewNode(pNowNode);
				pNowNode->SetArcLift(pSelArcPart->ArcLiftHandle());
				pNowNode->iSeg=pSelArcPart->iSeg;
				strcpy(pNowNode->layer(), pSelArcPart->layer());
				pNowNode->layer(1) = 'P';
			}
			else
				AfxMessageBox("ָ���������������ǰ���θ˼���Χ");
		}
#endif
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	return TRUE;
}

void CLDSView::OnDefDivideScaleNode() 
{	//��������ȷֽڵ�
	m_nPrevCommandID=ID_DEF_DIVIDE_SCALE_NODE;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to define average separate point";
#else
	m_sPrevCommandName="�ظ�����ȷֵ�";
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	Command("ScaleNode");
	//FinishDefDivideScaleNode();
}
int CLDSView::FinishDefDivideScaleNode()
{
	int retCode=0;
	CString cmdStr;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSDbObject *pFirstObj=NULL,*pSecondObj=NULL;
	if(m_setSnapObjects.GetNodeNum()==0)
	{	//�����߱�ִ����������������趨ϵͳ��������
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("ScaleNode please select the starting datum node or rod in average separate area:","");
#else
		pCmdLine->FillCmdLine("ScaleNode ��ѡ��ȷ������ʼ�˻�׼�ڵ��˼�:","");
#endif
		m_curTask=TASK_DEF_DIVIDE_NODE;
		g_pSolidSet->SetDisplayType(DISP_LINE);//�л���������ʾģʽ
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_ALL);
		g_pSolidDraw->ReleaseSnapStatus();
		m_setSnapObjects.Empty();
		return -1;
	}
	else
		pFirstObj=m_setSnapObjects.GetFirst();
	if(pFirstObj->GetClassTypeId()==CLS_NODE)
	{
		CLDSNode *pFirstNode=(CLDSNode*)pFirstObj;
		if(pFirstNode->m_cPosCalType==4)	//����ڵ�
			retCode= FinishDefScaleNodeByIntersNodeSection(pFirstNode);
		else if(m_setSnapObjects.GetNodeNum()==1)
		{
			pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("ScaleNode please select the ending datum node in average separate area:");
#else
			cmdStr.Format("ScaleNode ��ѡ��ȷ�������ն˻�׼�ڵ�:");
#endif
			pCmdLine->FillCmdLine(cmdStr,"");
			g_pSolidSnap->SetSnapType(SNAP_POINT);
			return -1;
		}
		else
		{
			pSecondObj=m_setSnapObjects.GetFirst();
			pSecondObj=m_setSnapObjects.GetNext();
			if(pSecondObj->GetClassTypeId()==CLS_NODE)
				retCode= FinishDefScaleNodeByNodeSection(pFirstNode,(CLDSNode*)pSecondObj);
			else
				retCode= -2;
		}
	}
	else if(pFirstObj->IsPart()&&((CLDSPart*)pFirstObj)->IsLinePart())	//�˼��ȷֽڵ�
		retCode= FinishDefScaleNodeByLinePart((CLDSLinePart*)pFirstObj);
	else
		retCode= -2;
	//���������ȷֽڵ������״̬
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("ScaleNode please select the starting datum node or rod in average separate area:");
#else
	cmdStr.Format("ScaleNode ��ѡ��ȷ������ʼ�˻�׼�ڵ��˼�:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	g_pSolidSet->SetDisplayType(DISP_LINE);//�л���������ʾģʽ
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	m_setSnapObjects.Empty();
	return retCode;
}
//����б�Ľ��洦�ȷֵ�
BOOL CLDSView::FinishDefScaleNodeByIntersNodeSection(CLDSNode *pSelNode/*=NULL*/) 
{
	CLDSNode *pNowNode=NULL;	 
	CLDSPart *pPart=NULL;
	CSuperSmartPtr<CLDSLinePart> pIntersLinePart[2];
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(pSelNode==NULL)
	{
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select a crossing node:","");
#else
		pCmdLine->FillCmdLine("��ѡ��һ������ڵ�:","");
#endif
		while(TRUE)
		{
			f3dPoint* pPoint;
			g_pSolidSnap->SetSnapType(SNAP_POINT);
			int ret=g_pSolidSnap->SnapPoint(pPoint);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			else if(ret>0)
			{
				pSelNode=console.FromNodeHandle(pPoint->ID);
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pPoint->ID);
				if(pSelNode&&pSelNode->m_cPosCalType==4)
					break;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Didn't select a right node!","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("Please select a crossing node:","");
#else
			pCmdLine->FillCmdLine("û��ѡ�к��ʵĽڵ㣡","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("��ѡ��һ������ڵ�:","");
#endif
		}
	}
	int k,M,i;
	f3dPoint start,end,vec,tem_pt;
	double jg_len,attach_scale;
	//long NODE_N = Ta.Node.GetNodeNum();
	//OperTypeStack.push(APPEND_NODE);
	//��һ������б��
	pPart = console.FromPartHandle(pSelNode->arrRelationPole[0]);
	if(pPart&&pPart->IsLinePart())
		pIntersLinePart[0] = (CLDSLinePart*)pPart;
	//�ڶ�������б��
	pPart = console.FromPartHandle(pSelNode->arrRelationPole[1]);
	if(pPart&&pPart->IsLinePart())
		pIntersLinePart[1] = (CLDSLinePart*)pPart;
	if(pIntersLinePart[0].IsNULL()||pIntersLinePart[1].IsNULL())
	{
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Rod didn't exist which node relative!","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("command:","");
#else
		pCmdLine->FillCmdLine("�ڵ�����˼������ڣ�","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("����:","");
#endif
		return FALSE;
	}

	BOOL bXieJgMir;//���������������˼��Ƿ�Ϊ�໥�Գ�
	if( pIntersLinePart[0]==pIntersLinePart[1]->GetMirPart(MIRMSG(2))||	//Y��Գ�
		pIntersLinePart[0]==pIntersLinePart[1]->GetMirPart(MIRMSG(1))||	//X��Գ�
		pIntersLinePart[0]==pIntersLinePart[1]->GetMirPart(MIRMSG(3)))	//Z��Գ�
		bXieJgMir = TRUE;	//�໥�Գ�
	else
		bXieJgMir = FALSE;	//�໥�Գ�
	Ta.BeginUndoListen();
	for(k=0;k<2;k++)
	{
		if(bXieJgMir&&k==1)	//�໥�Գ�ʱ�Ͳ���ԶԳ�б�˼��ȷ�
			break;
		for(M=0;M<2;M++)
		{
			start = pIntersLinePart[k]->pStart->ActivePosition();
			end	  = pIntersLinePart[k]->pEnd->ActivePosition();
			if(M==0)//�ȷֵ�ǰ�Ǹ���ʼ�˺ͽ����֮��ĽǸ�
				Sub_Pnt(vec, start, pSelNode->ActivePosition());
			else	//�ȷֵ�ǰ�Ǹ���ֹ�˺ͽ����֮��ĽǸ�
				Sub_Pnt(vec, end, pSelNode->ActivePosition());
			jg_len = LENGTH(vec);
			normalize(vec);
			f3dPoint mark_pos;
			Add_Pnt(mark_pos,pSelNode->ActivePosition(), vec*300.0);
			g_pSolidDraw->NewMark(mark_pos,5);
			g_pSolidDraw->SetEntSnapStatus(pIntersLinePart[k]->handle,TRUE);
			Invalidate(FALSE);
			
			if(divide_dlg.DoModal()!=IDOK)
			{
				g_pSolidDraw->SetEntSnapStatus(pIntersLinePart[k]->handle,FALSE);
				g_pSolidDraw->DestroyAllMark();
				Invalidate(FALSE);
				continue;
			}
			for(i=1;i<divide_dlg.m_nDivideByN;i++)
			{
				attach_scale = i/(double)divide_dlg.m_nDivideByN;
				Add_Pnt(tem_pt,pSelNode->ActivePosition(),vec*attach_scale*jg_len);
				pNowNode = console.AppendNode();
				pNowNode->hFatherPart = pIntersLinePart[k]->handle;
				pNowNode->cfgword = pIntersLinePart[k]->cfgword;
				pNowNode->SetPosition(tem_pt);
				pNowNode->m_cPosCalType = 5;  //�����ȷֵ�  WJH--00.07.20
				pNowNode->attach_scale = attach_scale;
				pNowNode->arrRelationNode[0] = pSelNode->handle;
				if(M==0)
					pNowNode->arrRelationNode[1] = pIntersLinePart[k]->pStart->handle;
				else
					pNowNode->arrRelationNode[1] = pIntersLinePart[k]->pEnd->handle;
				pNowNode->arrRelationPole[0] = pIntersLinePart[k]->handle;
				pNowNode->iSeg=pSelNode->iSeg;
				strcpy(pNowNode->layer(), pSelNode->layer());
				NewNode(pNowNode);
				pNowNode->SetArcLift(pIntersLinePart[k]->ArcLiftHandle());
				MirTaAtom(pNowNode,divide_dlg.mirmsg);
			}
			g_pSolidDraw->DestroyAllMark();
			Invalidate(FALSE);
		}
		g_pSolidDraw->SetEntSnapStatus(pIntersLinePart[k]->handle,FALSE);
		Invalidate(FALSE);
	}

	//long ll = Ta.Node.GetNodeNum()-NODE_N;
	//handle_stack.push(ll);
//////////////////////////////////////////////////////////////////////////	
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	Ta.EndUndoListen();
	return TRUE;
}
//������ͨ�ȷֵ�
BOOL CLDSView::FinishDefScaleNodeByNodeSection(CLDSNode *pSelNode1/*=NULL*/,CLDSNode *pSelNode2/*=NULL*/)
{
	if(pSelNode1==NULL||pSelNode2==NULL)
	{
		CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
		g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please select the start node of dividing area:","");
#else
		pCmdLine->FillCmdLine("��ѡ��ȷ������ʼ�˽ڵ�:","");
#endif
		while(TRUE)
		{
			f3dPoint* pPoint;
			int ret=g_pSolidSnap->SnapPoint(pPoint);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			else if(ret>0)
			{
				pSelNode1=console.FromNodeHandle(pPoint->ID);
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pPoint->ID);
				if(pSelNode1)
					break;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Didn't select a right node!","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("Please select the start node of dividing area:","");
		}
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("Please select the end node of dividing area:","");
#else
			pCmdLine->FillCmdLine("û��ѡ�к��ʵĽڵ㣡","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("��ѡ��ȷ������ʼ�˽ڵ�:","");
		}
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("��ѡ��ȷ�������ն˽ڵ�:","");
#endif
		while(TRUE)
		{
			f3dPoint* pPoint;
			int ret=g_pSolidSnap->SnapPoint(pPoint);
			if(ret<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			else if(ret>0)
			{
				pSelNode2=console.FromNodeHandle(pPoint->ID);
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(pPoint->ID);
				if(pSelNode2)
					break;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Didn't select a right node!","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("Please select the end node of dividing area:","");
#else
			pCmdLine->FillCmdLine("û��ѡ�к��ʵĽڵ㣡","");
			pCmdLine->FinishCmdLine();
			pCmdLine->FillCmdLine("��ѡ��ȷ�������ն˽ڵ�:","");
#endif
		}
	}
	int i;
	CLDSLinePart *pFatherPart=NULL;
	CLDSNode* pNowNode;
	f3dLine temLine;
	f3dPoint start,end,vec,tem_pt;
	double pt_dist,attach_scale;
	///////////////��׽����/////////////////////////

	if(divide_dlg.DoModal()!=IDOK)
		return FALSE;

	int NODE_N = Ta.Node.GetNodeNum();
	start = pSelNode1->ActivePosition();
	end   = pSelNode2->ActivePosition();
	Sub_Pnt(vec,end,start);
	pt_dist = LENGTH(vec);
	normalize(vec);

	//����ڵ�Ϊ�Ȳ��ڵ�ʱ�����ݽڵ���Ҹ��˼�ʱ��Ӧ�ڵ�ǰ���޼�������ڲ��� wht 13-04-02
	int iPos = -1;
	if(pSelNode1->Layer(0)=='L')
		iPos=CalPointQuadPos(pSelNode1->ActivePosition());
	else  if(pSelNode2->Layer(0)=='L')
		iPos=CalPointQuadPos(pSelNode2->ActivePosition());
	CLDSModule *pModule=console.GetActiveModule();
	CFGWORD dwActiveQuad1LegWord,dwActiveQuad2LegWord,dwActiveQuad3LegWord,dwActiveQuad4LegWord;
	if(pModule!=NULL)
	{
		dwActiveQuad1LegWord = CFGWORD(pModule->m_arrActiveQuadLegNo[0]);
		dwActiveQuad2LegWord = CFGWORD(pModule->m_arrActiveQuadLegNo[1]);
		dwActiveQuad3LegWord = CFGWORD(pModule->m_arrActiveQuadLegNo[2]);
		dwActiveQuad4LegWord = CFGWORD(pModule->m_arrActiveQuadLegNo[3]);
	}
	for(CLDSPart *pPart=Ta.Parts.GetFirst();pPart!=NULL;pPart=Ta.Parts.GetNext())
	{
		if(!pPart->IsLinePart())
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;
		if(iPos>=1&&iPos<=4&&pModule&& 
			!((iPos==1&&pLinePart->cfgword.And(dwActiveQuad1LegWord))
			||(iPos==2&&pLinePart->cfgword.And(dwActiveQuad2LegWord))
			||(iPos==3&&pLinePart->cfgword.And(dwActiveQuad3LegWord))
			||(iPos==4&&pLinePart->cfgword.And(dwActiveQuad4LegWord))))
			continue;
		
		temLine.startPt = pLinePart->pStart->ActivePosition();
		temLine.endPt  = pLinePart->pEnd->ActivePosition();
		if(temLine.PtInLine(pSelNode1->ActivePosition())>0&&temLine.PtInLine(pSelNode2->ActivePosition())>0)
		{
#ifndef __TSA_
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{	//��֤��ϽǸ��ϵȷֽڵ�ĸ��˼�Ϊ��ϽǸ� wht 10-10-28
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
				CLDSGroupLineAngle *pGroupJg=NULL;
				pGroupJg=(CLDSGroupLineAngle*)console.FromPartHandle(pLineAngle->group_father_jg_h,CLS_GROUPLINEANGLE);
				if(pGroupJg&&(pGroupJg->son_jg_h[0]==pLineAngle->handle||pGroupJg->son_jg_h[1]==pLineAngle->handle
					||pGroupJg->son_jg_h[2]==pLineAngle->handle||pGroupJg->son_jg_h[3]==pLineAngle->handle))
					pFatherPart=pGroupJg;
				else
					pFatherPart=pLineAngle;
			}
			else 
#endif
				pFatherPart=pLinePart;
			break;
		}
	}
	if(pFatherPart==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Didn't have rod to connect two node,can't divide!");
#else
		AfxMessageBox("���ڵ���޸˼�����,�޷����������ĵȷֽڵ����!");
#endif
		return FALSE;
	}
	Ta.BeginUndoListen();
	for(i=1;i<divide_dlg.m_nDivideByN;i++)
	{
		attach_scale = i/(double)divide_dlg.m_nDivideByN;
		Add_Pnt(tem_pt,start,vec*attach_scale*pt_dist);
		pNowNode = console.AppendNode();
		pNowNode->hFatherPart = pFatherPart->handle;
		pNowNode->cfgword = pSelNode1->cfgword;
		if(pSelNode2->Layer(0)=='L')
			pNowNode->cfgword = pSelNode2->cfgword;
		pNowNode->SetPosition(tem_pt);
		NewNode(pNowNode);
		pNowNode->SetArcLift(pFatherPart->ArcLiftHandle());
		pNowNode->m_cPosCalType = 5;  //�����ȷֵ�  WJH--00.07.20
		pNowNode->attach_scale = attach_scale;
		pNowNode->arrRelationNode[0] = pSelNode1->handle;
		pNowNode->arrRelationNode[1] = pSelNode2->handle;
		pNowNode->arrRelationPole[0] = pNowNode->hFatherPart;
		pNowNode->iSeg=pFatherPart->iSeg;
		strcpy(pNowNode->layer(), pFatherPart->layer());
		pNowNode->layer(1) = 'P';
		MirTaAtom(pNowNode,divide_dlg.mirmsg);
	}
	
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	Ta.EndUndoListen();
	return TRUE;
}

//����ƫ�Ƶ�
BOOL CLDSView::FinishDefOffsetNode()
{
	double up_offset,down_offset;
	CLDSNode *pSelNode = NULL, *pNode = NULL;
	CLDSPart *pPart = NULL;
	CLDSLinePart *pSelLinePart = NULL;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSArcPart *pSelArcPart = NULL;
#endif
	f3dPoint vec,pt;
	static CDefOffsetNodeDlg offset_dlg;
	f3dPoint* pPoint;
	
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select a offset datum node:","");
#else
	pCmdLine->FillCmdLine("��ѡ��һ��ƫ�ƻ�׼�ڵ�:","");
#endif
	while(TRUE)
	{
		int ret=g_pSolidSnap->SnapPoint(pPoint);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		else if(ret>0)
		{
			pSelNode=console.FromNodeHandle(pPoint->ID);
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pPoint->ID);
			if(pSelNode)
				break;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Didn't select a right node!","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("Please select a offset datum node:","");
	}
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("Please select a datum rod as offset direction:","");
#else
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĽڵ㣡","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("��ѡ��һ��ƫ�ƻ�׼�ڵ�:","");
	}
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("��ѡ��һ����׼�˼���Ϊƫ�Ʒ���:","");
#endif
	while(TRUE)
	{
		f3dLine line;
		int ret = g_pSolidSnap->SnapLine(line);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		else if(ret>0)
		{
			pPart=console.FromPartHandle(line.ID);
			if(pPart&&pPart->IsLinePart())
			{
				pSelLinePart=(CLDSLinePart*)pPart;
				if(line.PtInLine(*pPoint)!=0)
					break;
			}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			else if(pPart&&pPart->IsArcPart())
			{
				pSelArcPart=(CLDSArcPart*)pPart;
				//if(pSelArcPart->baseline.p
				break;
			}
#endif
			g_pSolidDraw->SetEntSnapStatus(line.ID);
			g_pSolidDraw->SetEntSnapStatus(line.ID);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Didn't select a right rod!","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("Please select a datum rod as offset direction:","");
#else
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĸ˼���","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("��ѡ��һ����׼�˼���Ϊƫ�Ʒ���:","");
#endif
	}
	if(pSelLinePart)
	{
		vec=pSelLinePart->pEnd->ActivePosition()-pSelLinePart->pStart->ActivePosition();
		if(pSelLinePart->pStart->handle == pSelNode->handle)
			offset_dlg.m_bUpOffset = FALSE;
		else
			offset_dlg.m_bUpOffset = TRUE;
		if(pSelLinePart->pEnd->handle == pSelNode->handle)
			offset_dlg.m_bDownOffset = FALSE;
		else
			offset_dlg.m_bDownOffset = TRUE;
		normalize(vec);
		f3dPoint mark_pos=pSelNode->ActivePosition() - vec*500;
		g_pSolidDraw->NewMark(mark_pos,15);
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pSelArcPart)
	{
		offset_dlg.m_bUpOffset=offset_dlg.m_bDownOffset=TRUE;
		g_pSolidDraw->NewMark(pSelArcPart->baseline.Start(),15);
	}
#endif
	Invalidate(FALSE);
	if(pSelLinePart)
		offset_dlg.InitOffsetType(pSelLinePart);
	else
		offset_dlg.InitOffsetType(pSelArcPart);
	CUndoOperObject undo(&Ta,TRUE);
	CLDSNode* pPrevOffsetNode=pSelNode;
	do{
		if(offset_dlg.DoModal()!=IDOK)
		{
			g_pSolidDraw->DestroyAllMark();
			Invalidate(FALSE);
			return FALSE;
		}
		pSelNode=pPrevOffsetNode;
		g_pSolidDraw->DestroyAllMark();
		up_offset = offset_dlg.m_fUpOffset;
		down_offset = offset_dlg.m_fDownOffset;
		if(pSelLinePart)
		{
			if(fabs(up_offset)>=EPS2)
			{
				pPrevOffsetNode=pNode = console.AppendNode();
				pNode->arrRelationNode[0] = pSelNode->handle;
				pNode->arrRelationNode[1] = pSelLinePart->pStart->handle;
				pNode->arrRelationNode[2] = pSelLinePart->pEnd->handle;
				pNode->arrRelationPole[0] = pSelLinePart->handle;
				pNode->attach_offset = -offset_dlg.m_fUpOffset;
				if(offset_dlg.m_iOffsetVec==1)
				{	//��X�᷽����ĳ�ڵ��ƫ�ƽڵ�
					if(fabs(vec.x)<EPS)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("The offset datum rod of offset node on X axis is horizontal rod,system modified to the offset node of spreading length direction!");
#else
						AfxMessageBox("X�᷽��ƫ�ƽڵ��ƫ��׼�˼�Ϊˮƽ�˼���ϵͳ�Զ�����Ϊ�ӳ��ȷ����ƫ�ƽڵ㣡");
#endif
						pNode->m_cPosCalType = 7;//��ʾ�ظ˼���ĳ�ڵ��ƫ�Ƶ�
						pNode->SetPosition(pSelNode->ActivePosition()-vec*up_offset);
					}
					else
					{
						pNode->m_cPosCalType = 7;//��ʾ��Z�᷽����ĳ�ڵ��ƫ�Ƶ�
						pNode->m_cPosCalViceType='X';
						pNode->SetPosition(pSelNode->ActivePosition()-vec*(up_offset*vec.mod()/fabs(vec.x)));
					}
				}
				else if(offset_dlg.m_iOffsetVec==2)
				{	//��Y�᷽����ĳ�ڵ��ƫ�ƽڵ�
					if(fabs(vec.y)<EPS)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("The offset datum rod of offset node on Y axis is horizontal rod,system modified to the offset node of spreading length direction!");
#else
						AfxMessageBox("Y�᷽��ƫ�ƽڵ��ƫ��׼�˼�Ϊˮƽ�˼���ϵͳ�Զ�����Ϊ�ӳ��ȷ����ƫ�ƽڵ㣡");
#endif
						pNode->m_cPosCalType = 7;//��ʾ�ظ˼���ĳ�ڵ��ƫ�Ƶ�
						pNode->SetPosition(pSelNode->ActivePosition()-vec*up_offset);
					}
					else
					{
						pNode->m_cPosCalType = 7;//��ʾ��Z�᷽����ĳ�ڵ��ƫ�Ƶ�
						pNode->m_cPosCalViceType='Y';
						pNode->SetPosition(pSelNode->ActivePosition()-vec*(up_offset*vec.mod()/fabs(vec.y)));
					}
				}
				else if(offset_dlg.m_iOffsetVec==3)
				{	//��Z�᷽����ĳ�ڵ��ƫ�ƽڵ�
					if(fabs(vec.z)<EPS)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("The offset datum rod of offset node on Z axis is horizontal rod,system modified to the offset node of spreading length direction!");
#else
						AfxMessageBox("Z�᷽��ƫ�ƽڵ��ƫ��׼�˼�Ϊˮƽ�˼���ϵͳ�Զ�����Ϊ�ӳ��ȷ����ƫ�ƽڵ㣡");
#endif
						pNode->m_cPosCalType = 7;//��ʾ�ظ˼���ĳ�ڵ��ƫ�Ƶ�
						pNode->SetPosition(pSelNode->ActivePosition()-vec*up_offset);
					}
					else
					{
						pNode->m_cPosCalType = 7;//��ʾ��Z�᷽����ĳ�ڵ��ƫ�Ƶ�
						pNode->m_cPosCalViceType='Z';
						pNode->SetPosition(pSelNode->ActivePosition()-vec*(up_offset*vec.mod()/fabs(vec.z)));
					}
				}
				else //if(offset_dlg.m_iOffsetVec==0)
				{	//�ظ˼�������ĳ�ڵ��ƫ�ƽڵ�
					pNode->m_cPosCalType = 7;
					pNode->SetPosition(pSelNode->ActivePosition()-vec*up_offset);
				}
				pNode->hFatherPart = pSelLinePart->handle;
				pNode->cfgword = pSelLinePart->cfgword;
				NewNode(pNode);
				pNode->SetArcLift(pSelLinePart->ArcLiftHandle());
				pNode->Set();
				pNode->iSeg=pSelLinePart->iSeg;
				pNode->SetLayer(pSelLinePart->layer());
				pNode->layer(1) = 'P';
				MirTaAtom(pNode,offset_dlg.mirmsg);
			}
			if(fabs(down_offset)>=EPS2)
			{
				pPrevOffsetNode=pNode = console.AppendNode();
				pNode->iSeg=pSelLinePart->iSeg;
				pNode->hFatherPart = pSelLinePart->handle;
				pNode->cfgword = pSelLinePart->cfgword;
				strcpy(pNode->layer(),pSelLinePart->layer());
				pNode->layer(1) = 'P';
				pNode->arrRelationNode[0] = pSelNode->handle;
				pNode->arrRelationNode[1] = pSelLinePart->pStart->handle;
				pNode->arrRelationNode[2] = pSelLinePart->pEnd->handle;
				pNode->arrRelationPole[0] = pSelLinePart->handle;
				if(offset_dlg.m_iOffsetVec==1)
				{	//��X�᷽����ĳ�ڵ��ƫ�ƽڵ�
					if(fabs(vec.x)<EPS)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("The offset datum rod of offset node on X axis is horizontal rod,system modified to the offset node of spreading length direction!");
#else
						AfxMessageBox("X�᷽��ƫ�ƽڵ��ƫ��׼�˼�Ϊˮƽ�˼���ϵͳ�Զ�����Ϊ�ӳ��ȷ����ƫ�ƽڵ㣡");
#endif
						pNode->m_cPosCalType = 7;//��ʾ�ظ˼���ĳ�ڵ��ƫ�Ƶ�
						pNode->SetPosition(pSelNode->ActivePosition()+vec*down_offset);
					}
					else
					{
						pNode->m_cPosCalType = 7;//��ʾ�ؽǸ���ĳ�ڵ��ƫ�Ƶ�
						pNode->m_cPosCalViceType='X';
						pNode->SetPosition(pSelNode->ActivePosition()+vec*(down_offset*vec.mod()/fabs(vec.x)));
					}
				}
				else if(offset_dlg.m_iOffsetVec==2)
				{	//��Y�᷽����ĳ�ڵ��ƫ�ƽڵ�
					if(fabs(vec.z)<EPS)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("The offset datum rod of offset node on Y axis is horizontal rod,system modified to the offset node of spreading length direction!");
#else
						AfxMessageBox("Y�᷽��ƫ�ƽڵ��ƫ��׼�˼�Ϊˮƽ�˼���ϵͳ�Զ�����Ϊ�ӳ��ȷ����ƫ�ƽڵ㣡");
#endif
						pNode->m_cPosCalType = 7;//��ʾ�ظ˼���ĳ�ڵ��ƫ�Ƶ�
						pNode->SetPosition(pSelNode->ActivePosition()+vec*down_offset);
					}
					else
					{
						pNode->m_cPosCalType = 7;//��ʾ�ؽǸ���ĳ�ڵ��ƫ�Ƶ�
						pNode->m_cPosCalViceType='Y';
						pNode->SetPosition(pSelNode->ActivePosition()+vec*(down_offset*vec.mod()/fabs(vec.y)));
					}
				}
				else if(offset_dlg.m_iOffsetVec==3)
				{	//��Z�᷽����ĳ�ڵ��ƫ�ƽڵ�
					if(fabs(vec.z)<EPS)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						AfxMessageBox("The offset datum rod of offset node on Z axis is horizontal rod,system modified to the offset node of spreading length direction!");
#else
						AfxMessageBox("Z�᷽��ƫ�ƽڵ��ƫ��׼�˼�Ϊˮƽ�˼���ϵͳ�Զ�����Ϊ�ӳ��ȷ����ƫ�ƽڵ㣡");
#endif
						pNode->m_cPosCalType = 7;//��ʾ�ظ˼���ĳ�ڵ��ƫ�Ƶ�
						pNode->SetPosition(pSelNode->ActivePosition()+vec*down_offset);
					}
					else
					{
						pNode->m_cPosCalType = 7;//��ʾ�ؽǸ���ĳ�ڵ��ƫ�Ƶ�
						pNode->m_cPosCalViceType='Z';
						pNode->SetPosition(pSelNode->ActivePosition()+vec*(down_offset*vec.mod()/fabs(vec.z)));
					}
				}
				else //if(offset_dlg.m_iOffsetVec==0)
				{	//�ظ˼�������ĳ�ڵ��ƫ�ƽڵ�
					pNode->m_cPosCalType = 7;
					pNode->SetPosition(pSelNode->ActivePosition()+vec*down_offset);
				}
				pNode->attach_offset = offset_dlg.m_fDownOffset;

				NewNode(pNode);
				pNode->SetArcLift(pSelLinePart->ArcLiftHandle());
				pNode->Set();
				MirTaAtom(pNode,offset_dlg.mirmsg);
			}
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pSelArcPart)
		{
			UCS_STRU ucs;
			ucs.origin=pSelArcPart->Center();
			ucs.axis_x=pSelArcPart->Start()-ucs.origin;
			ucs.axis_z=pSelArcPart->WorkNorm();
			normalize(ucs.axis_y);
			for(int i=0;i<2;i++)
			{
				double offset=0;
				if(i==0)
					offset=-up_offset;
				else
					offset=down_offset;
				if(fabs(offset)>=EPS2)
				{
					pNode = console.AppendNode();
					pNode->iSeg=pSelArcPart->iSeg;
					pNode->hFatherPart = pSelArcPart->handle;
					pNode->cfgword = pSelArcPart->cfgword;
					strcpy(pNode->layer(),pSelArcPart->layer());
					pNode->layer(1) = 'P';
					double rot_angle=offset/pSelArcPart->Radius();
					f3dPoint start_vec=pSelNode->xPreliftPos-ucs.origin;
					start_vec-=(start_vec*ucs.axis_z)*ucs.axis_z;
					if(fabs(pSelArcPart->WorkNorm()*pSelArcPart->baseline.ColumnNorm())>EPS_COS)
					{	//Բ��
						f3dPoint pos=ucs.origin+start_vec.normalized()*pSelArcPart->Radius();
						rotate_point_around_axis(pos,rot_angle,ucs.origin,ucs.origin+ucs.axis_z);
						pNode->SetPosition(pos);
					}
					else
					{	//��Բ��
						vector_trans(start_vec,ucs,FALSE);
						double angle=Cal2dLineAng(0,0,start_vec.x,start_vec.y)+rot_angle;
						pNode->SetPosition(pSelArcPart->baseline.PositionInAngle(angle));
					}
					pNode->SetArcLift(pSelArcPart->ArcLiftHandle());
					pNode->Set();
					NewNode(pNode);
				}
			}
		}
		if(up_offset!=0&&down_offset!=0)
			offset_dlg.m_bMultipleOffset=false;
#endif
	}while(offset_dlg.m_bMultipleOffset);
	//glOperate->SetMark(&_mark, FALSE);
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	g_pPromptMsg->Destroy();
	return TRUE;
}

//�ȸߵ�
BOOL CLDSView::FinishDefHorizonNode()
{
	double offset;
	CLDSNode *pSelNode = NULL, *pNode = NULL;
	CLDSPart *pPart = NULL;
	CLDSLinePart *pSelLinePart = NULL;
	f3dPoint vec, pt, *pPoint;
	//CHorizonNodeDlg dlg;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select a datum node:","");
#else
	pCmdLine->FillCmdLine("��ѡ��һ����׼�ڵ�:","");
#endif
	while(TRUE)
	{
		int ret = g_pSolidSnap->SnapPoint(pPoint);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		else if(ret>0)
		{
			pSelNode=console.FromNodeHandle(pPoint->ID);
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(pPoint->ID);
			if(pSelNode)
				break;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Did't select a right node!","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("Please select a datum node:","");
	}
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("Please select the rod that node with same height on it:","");
#else
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĽڵ㣡","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("��ѡ��һ����׼�ڵ�:","");
	}
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("��ѡ��ȸߵ����ڵĸ˼�:","");
#endif
	while(TRUE)
	{
		f3dLine line;
		int ret = g_pSolidSnap->SnapLine(line);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		else if(ret>0)
		{
			pPart=console.FromPartHandle(line.ID);
			if(pPart&&pPart->IsLinePart())
				pSelLinePart=(CLDSLinePart*)pPart;
			g_pSolidDraw->SetEntSnapStatus(line.ID);
			if(pSelLinePart&&(line.PtInLine(*pPoint)==0))
				break;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Didn't select a right rod!","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("Please select a dautm rod as offset direction:","");
#else
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĸ˼���","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("��ѡ��һ����׼�˼���Ϊƫ�Ʒ���:","");
#endif
	}
	if(mir_dlg.DoModal()!=IDOK)
		return FALSE;
	Ta.BeginUndoListen();
	try
	{
		Sub_Pnt(vec,pSelLinePart->pEnd->ActivePosition(), pSelLinePart->pStart->ActivePosition());
		offset = (pSelNode->ActivePosition().z-pSelLinePart->pStart->ActivePosition().z)
			*vec.mod()/vec.z;
		normalize(vec);
		if(fabs(offset)>=EPS2)
		{
			pNode = console.AppendNode();
			Add_Pnt(pt,pSelLinePart->pStart->ActivePosition(), vec*offset);
			pNode->SetPosition(pt);
			NewNode(pNode);
			pNode->SetArcLift(pSelLinePart->ArcLiftHandle());
			pNode->Set();
			pNode->hFatherPart = pSelLinePart->handle;
			pNode->cfgword = pSelLinePart->cfgword;
			pNode->iSeg=pSelLinePart->iSeg;
			pNode->SetLayer(pSelLinePart->layer());
			pNode->layer(1) = 'P';
			
			pNode->arrRelationNode[0] = pSelNode->handle;
			pNode->arrRelationNode[1] = pSelLinePart->pStart->handle;
			pNode->arrRelationNode[2] = pSelLinePart->pEnd->handle;
			pNode->arrRelationPole[0] = pSelLinePart->handle;
			pNode->m_cPosCalType = 6;//7:��ʾ�ؽǸ���ĳ�ڵ�ˮƽ�Ľڵ�
			pNode->m_cPosCalViceType='Z';
			MirTaAtom(pNode,mir_dlg.mirmsg);
			g_pPromptMsg->Destroy();
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	Ta.EndUndoListen();
	return TRUE;
}

static bool _LocalIsSupportRodTypeId(int idRodClsType)
{
	return (idRodClsType==CLS_LINEANGLE||idRodClsType==CLS_LINESLOT||idRodClsType==CLS_LINEFLAT);
}
//�������к���Ľ���ڵ�
void CLDSView::DefAllIntersNode()
{
	CUndoOperObject undo(&Ta,true);
	Ta.DisplayProcess(0,NULL);
	int iCurPos=0,nLinePartCount=0;
	__int64 nPercent = 0;
	//CHashList<long> classTypeIdHash;	//�˼����Ͳ�ͬҲ�����彻�������͹�ϣ�� wht 12-09-26
	//classTypeIdHash.SetValue(CLS_LINEANGLE,CLS_LINEANGLE);
	//classTypeIdHash.SetValue(CLS_LINESLOT,CLS_LINESLOT);
	//classTypeIdHash.SetValue(CLS_LINEFLAT,CLS_LINEFLAT);
	//�����ظ����彻��ڵ�
	CLDSNode *pNode=NULL;
	CHash64Set<CLDSNode*> hashIntersNodes;
	CMaxDouble maxZ;
	CMinDouble minZ;
	SCOPE_STRU scope;
	CCheckOverlappedNode overlap;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		maxZ.Update(pNode->xOriginalPos.z);
		minZ.Update(pNode->xOriginalPos.z);
		scope.VerifyVertex(pNode->xOriginalPos);
		if(pNode->m_cPosCalType!=4)
			continue;
		hashIntersNodes.SetValue(DUALKEY(pNode->arrRelationPole[0],pNode->arrRelationPole[1]),pNode);
		/** ��������ظ��������˼����Ƿ��Ѵ��ڽ��潻�� wjh-2019.7.24
		if(((pNode->arrRelationNode[0]==pFirstLinePart->pStart->handle&&pNode->arrRelationNode[1]==pFirstLinePart->pEnd->handle
			&&pNode->arrRelationNode[2]==pSecLinePart->pStart->handle&&pNode->arrRelationNode[3]==pSecLinePart->pEnd->handle)||
			(pNode->arrRelationNode[0]==pSecLinePart->pStart->handle&&pNode->arrRelationNode[1]==pSecLinePart->pEnd->handle
				&&pNode->arrRelationNode[2]==pFirstLinePart->pStart->handle&&pNode->arrRelationNode[3]==pFirstLinePart->pEnd->handle))&&
				((pNode->arrRelationPole[0]==pFirstLinePart->handle||pNode->arrRelationPole[1]==pSecLinePart->handle)||
			(pNode->arrRelationPole[1]==pFirstLinePart->handle||pNode->arrRelationPole[0]==pSecLinePart->handle)))
			break;	//�Ѵ�����ͬ������ͬ�Ľ���ڵ�
		*/
	}
	overlap.keygen.Init(&scope);
	for (pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
		overlap.AddLdsNode(pNode);
	CLDSLinePart *pRod,*pFirstLinePart=NULL,*pSecLinePart=NULL;
	for(pFirstLinePart=Ta.EnumRodFirst();pFirstLinePart;pFirstLinePart=Ta.EnumRodNext())
		nLinePartCount++;
	int nStepCount=max(1,nLinePartCount/2000);
	double dfSummHeight=maxZ.number-minZ.number;
	double dfStepH=dfSummHeight/nStepCount;
	g_pSolidDraw->ReleaseSnapStatus();
	CLDSLinePartPtr _xarrPool[10000];
	int nStepPercent=100/nStepCount;
	for(int step=0;step<nStepCount;step++)
	{
		double dfMinZOfSegment = minZ.number + step * dfStepH;
		double dfMaxZOfSegment = dfMinZOfSegment + dfStepH;
		if (step == nStepCount)
			dfMaxZOfSegment = maxZ.number;
		//���ܵ�ǰ����˼�����
		ARRAY_LIST<CLDSLinePartPtr> xarrSegRods(_xarrPool, 10000, 1000);
		for (pRod = Ta.Parts.GetFirstLinePart(); pRod; pRod = Ta.Parts.GetNextLinePart())
		{
			if (!_LocalIsSupportRodTypeId(pRod->GetClassTypeId()) ||
				pRod->pStart == NULL || pRod->pEnd == NULL)
				continue;
			if ((pRod->pStart->xOriginalPos.z > dfMinZOfSegment - 10 && pRod->pStart->xOriginalPos.z < dfMaxZOfSegment + 10) ||
				(pRod->pEnd->xOriginalPos.z > dfMinZOfSegment - 10 && pRod->pEnd->xOriginalPos.z < dfMaxZOfSegment + 10))
				xarrSegRods.append(pRod);
		}
		nPercent = 0;
		int nSumPercent=xarrSegRods.Count*(xarrSegRods.Count-1)/2;
		for (int i=0;i<xarrSegRods.Count;i++)
		{
			pRod = xarrSegRods[i];
			SCOPE_STRU first_scope;
			first_scope.VerifyVertex(pRod->pStart->xOriginalPos);
			first_scope.VerifyVertex(pRod->pEnd->xOriginalPos);
			for (int j=i+1;j<xarrSegRods.Count;j++,nPercent++)
			{
				int percent = (int)((nStepPercent * nPercent) / nSumPercent);
				Ta.DisplayProcess(percent+nStepPercent*step, NULL);
				CLDSLinePart* pNextRod = xarrSegRods[j];
				if (!pRod->ModuleTogetherWith(pNextRod))
					continue;	//���˼����Ͳ�ͬ�Ҳ���ָ���ķ�ͬ��˼��б��У�������ӽ���ڵ�

				SCOPE_STRU sec_scope;
				sec_scope.VerifyVertex(pNextRod->pStart->xOriginalPos);
				sec_scope.VerifyVertex(pNextRod->pEnd->xOriginalPos);
				if (!sec_scope.IsIntersWith(first_scope))
					continue;
				int nRetCode = 0;
				if ((pNode = hashIntersNodes.GetValue(DUALKEY(pRod->handle, pNextRod->handle))) == NULL)
					nRetCode = NewIntersNode(pRod, pNextRod, pNode,&overlap);
				if (nRetCode == 1)
				{
					g_pSolidDraw->SetEntSnapStatus(pRod->handle, TRUE);
					g_pSolidDraw->SetEntSnapStatus(pNextRod->handle, TRUE);
					if (pNode)
						g_pSolidDraw->SetEntSnapStatus(pNode->handle, TRUE);
					break;
				}
			}
		}
	}
	Ta.DisplayProcess(100,NULL);
}
//pReserveNode:�ϲ�����Ҫ�����Ľڵ�;pMergeNode:�ϲ�����Ҫɾ���Ľڵ� defined in OutputInfo.cpp
bool MergeOverlapNodes(CLDSNode *pReserveNode,CLDSNode *pMergeNode,COutputInfo *pOutputInfo=NULL);
//�ϲ��ڵ�
int CLDSView::MergeMultiNodes()
{
	long *id_arr,i,nCount=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CLDSNode* pNode=NULL,*pDestNode=NULL;
	if(nCount==0)
		return 0;
	NODESET nodeset;
	for(i=0;i<nCount;i++)
	{
		pNode=Ta.Node.FromHandle(id_arr[i]);
		if(pNode==NULL)
			continue;
		nodeset.append(pNode);
		if(pDestNode==NULL)
			pDestNode=pNode;
		else if(pDestNode->handle>pNode->handle)
			pDestNode=pNode;
	}
	if(nodeset.GetNodeNum()>=2)
	{
		nCount=0;
		for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
		{
			if(pNode!=pDestNode && MergeOverlapNodes(pDestNode,pNode))
				nCount++;
		}
		if(nCount>=1)
			g_pSolidDraw->ReleaseSnapStatus();
		return nCount;
	}
	else if(nodeset.GetNodeNum()==1)
	{
		m_setSnapObjects.Empty();
		m_setSnapObjects.append(nodeset.GetFirst());
		m_nObjectSnapedCounts=1;
	}
	return 0;
}
int CLDSView::MergeNodes()
{
	CString cmdStr,promptStr;
	int retCode=0;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSDbObject *pFirstObj=NULL,*pSecondObj=NULL;
	CLDSNode *pReserveNode=NULL,*pMergeNode=NULL;
	CUndoOperObject undo(&Ta);
	if(m_setSnapObjects.GetNodeNum()==0)
	{	//�����߱�ִ����������������趨ϵͳ��������
		g_pSolidDraw->ReleaseSnapStatus();
		m_setSnapObjects.Empty();
		m_nObjectSnapedCounts=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("Please specify the destination(remained) node which will merge to:","");
#else
		pCmdLine->FillCmdLine("��ѡ����ϲ�(Ӧɾ��)�ڵ�:","");
#endif
		return -1;
	}
	else
		pFirstObj=m_setSnapObjects.GetFirst();
	if(m_setSnapObjects.GetNodeNum()==1)
	{
		if(pCmdLine->m_bGettingStrFromCmdLine)
			pCmdLine->FinishCmdLine();
		if(pFirstObj->GetClassTypeId()==CLS_NODE)
		{
			pCmdLine->FinishCmdLine();
			g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("Please the node which will be merged(should delete):","");
#else
			pCmdLine->FillCmdLine("��ָ�����ϲ�����Ŀ��(����)�ڵ�:","");
#endif
			return -1;
		}
		else
			retCode=-3;
	}
	else
	{
		pSecondObj=m_setSnapObjects.GetFirst();
		pSecondObj=m_setSnapObjects.GetNext();
		if(pFirstObj->handle==pSecondObj->handle)
			retCode=-2;
		else if(pSecondObj&&pSecondObj->GetClassTypeId()==CLS_NODE)
		{
			//����ʵ��ʹ���������һ��ѡ�нڵ�Ϊ��ɾ���ڵ�����ϲ���ϰ�� wjh-2017.12.16
			if(MergeOverlapNodes((CLDSNode*)pSecondObj,(CLDSNode*)pFirstObj))
				retCode=TRUE;
		}
		else
			retCode=-3;
	}
	//������������״̬
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
	m_setSnapObjects.Empty();
	g_pSolidSet->SetDrawType(DRAW_LINK);	//ͨ�����лָ���������״̬
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please specify the destination(remained) node which will merge to:","");
#else
	pCmdLine->FillCmdLine("��ѡ����ϲ�(Ӧɾ��)�ڵ�:","");
#endif
	if(retCode==TRUE)
		g_pSolidDraw->BuildDisplayList();
	return retCode;
}
static int _LocalCompareNodes(const CLDSNodePtr& pNode1,const CLDSNodePtr& pNode2)
{
	if(pNode1->iSeg>pNode2->iSeg)
		return 1;
	else if(pNode1->iSeg<pNode2->iSeg)
		return -1;
	BYTEFLAG itemflag1=0,itemflag2=0;
	if(pNode1->Layer(0)=='T')
		itemflag1.SetBitState(4);
	if(pNode1->Layer(2)!='Q'&&pNode1->Layer(2)!='H'&&pNode1->Layer(2)!='Z'&&pNode1->Layer(2)!='Y')
		itemflag1.SetBitState(3);
	else if(pNode1->Layer(2)=='Z'&&pNode1->Layer(2)=='Y')
		itemflag1.SetBitState(2);
	if(pNode2->Layer(0)=='T')
		itemflag2.SetBitState(4);
	if(pNode2->Layer(2)!='Q'&&pNode2->Layer(2)!='H'&&pNode2->Layer(2)!='Z'&&pNode2->Layer(2)!='Y')
		itemflag2.SetBitState(3);
	else if(pNode2->Layer(2)=='Z'&&pNode2->Layer(2)=='Y')
		itemflag2.SetBitState(2);
	if((BYTE)itemflag1>(BYTE)itemflag2)
		return 1;
	else if((BYTE)itemflag1<(BYTE)itemflag2)
		return -1;
	GEPOINT pick1=pNode1->xPreliftPos;
	GEPOINT pick2=pNode2->xPreliftPos;
	if(pNode1->Layer(0)=='T')
	{	//�ᵣ���ɽ���Զ������б��ȡ��ֵ���Ḩ��ȡ��ֵ��
		if(fabs(pick1.x)+fabs(pick1.y)<fabs(pick2.x)+fabs(pick2.y)-1)
			return -1;
		else if(fabs(pick1.x)+fabs(pick1.y)>fabs(pick2.x)+fabs(pick2.y)+1)
			return  1;
		else if(pick1.y>pick2.y)
			return -1;
		else if(pick1.y<pick2.y)
			return 1;
		return 0;
	}
	else
	{
		if(pick1.z>pick2.z+1)
			return -1;
		else if(pick1.z<pick2.z-1)
			return 1;
		else if(fabs(pick1.y)<fabs(pick2.y)-1)
			return 1;
		else if(fabs(pick1.y)>fabs(pick2.y)+1)
			return -1;
		else if(fabs(pick1.y)-fabs(pick2.y)<1&&pick1.y>pick2.y)
			return -1;
		else if(fabs(pick1.y)-fabs(pick2.y)<1&&pick1.y<pick2.y)
			return 1;
		else if(pick1.x>pick2.x)
			return -1;
		else if(pick1.x<pick2.x)
			return 1;
		else
			return 0;
	}
	return 0;
}
#include "EmbedMirInfoPanel.h"
void SetRelativeObj(CLDSDbObject *pObj);
CLDSNode* CreateSingleMirNode(CLDSNode *pSrcNode, MIRMSG msg,BOOL bIncAttachInfo=TRUE,char *sErrorInfo=NULL);
static int _BranchNodes(CLDSView* pLdsView)
{
	//if(!PRODUCT_FUNC::IsHasInternalTest())
	//	return -1;
	char ciMirBranchNodeMode=0;
	long *id_arr=NULL;
	long i,n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CLDSNode* pNode;
	CLDSLinePart* pRod;
	CHashSet<CLDSNodePtr> hashNodes;
	LINEPARTSET rayRodSet;
	for(i=0;i<n;i++)
	{
		if((pNode=Ta.Node.FromHandle(id_arr[i]))!=NULL)
		{
			hashNodes.SetValue(pNode->handle,pNode);
			pNode->RetrieveConnectNodeInfo(NULL,false);
		}
		else if((pRod=Ta.FromRodHandle(id_arr[i]))!=NULL)
			rayRodSet.append(pRod);
	}
	CCmdLineDlg *pCmdLine=(CCmdLineDlg*)((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	if(hashNodes.GetNodeNum()==0&&rayRodSet.GetNodeNum()>0)
	{	//�ֶ�ƫ�Ʋ�����߸˼��Ķ˽ڵ�
		if(!pLdsView->OffsetRayRodEndNode(rayRodSet))
			return FALSE;
		return _BranchNodes(pLdsView);
	}
	else if(hashNodes.GetNodeNum()==0)
	{	//TODO��ϵͳ��ǰδѡ�нڵ�ʱ������ʾ�ü�ѡ�����ֵĽڵ� wjh-2017.12.16
		pCmdLine->FillCmdLine("BranchNodes ��ѡ�����ֵĶ˽ڵ��������߸˼�:","");
		CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_POINT|SNAP_LINE);
		verifier.AddVerifyType(OBJPROVIDER::SOLIDSPACE,CLS_LINEPART);
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		DWORD dwhObj=0,dwExportFlag=0;
		for(;;)
		{
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			dwhObj=obj.hRelaObj;
			if(dwhObj>0&&(pNode=console.FromNodeHandle(dwhObj))!=NULL)
			{
				hashNodes.SetValue(pNode->handle,pNode);
				pNode->RetrieveConnectNodeInfo(NULL,false);
				break;
			}
			else if(dwhObj>0&&(pRod=console.FromRodHandle(dwhObj))!=NULL)
			{
				rayRodSet.append(pRod);
				g_pSolidDraw->SetEntSnapStatus(pRod->handle);
				break;
			}
		}
		if(rayRodSet.GetNodeNum()>0)
		{
			if(!pLdsView->OffsetRayRodEndNode(rayRodSet))
				return FALSE;
			return _BranchNodes(pLdsView);
		}
	}
	else
	{
		DYN_ARRAY<CLDSNodePtr> sortnodes(hashNodes.GetNodeNum());
		for(pNode=hashNodes.GetFirst(),i=0;pNode;pNode=hashNodes.GetNext(),i++)
			sortnodes[i]=pNode;
		//�Դ���ֽڵ㰴���ϵ��£��������ҡ���ǰ�����˳�������Ա������û�����ͼϰ�� wjh-2017.12.16
		CQuickSort<CLDSNodePtr>::QuickSort(sortnodes,sortnodes.Size(),_LocalCompareNodes);
		hashNodes.Empty();
		for(i=0;i<(int)sortnodes.Size();i++)
			hashNodes.SetValue(sortnodes[i]->handle,sortnodes[i]);
	}
	CDisplayView *pCurView=console.GetActiveView();
	int branch_hits=0;
	{	//�˻�������������Ҫ����UNDO�����������⺯��Ƕ�׵��ô�����undo���� wjh-2017.12.16
		CUndoOperObject undo(&Ta,true);
		CEmbedMirInfoPanel xCmdMiniPanel;
		for(pNode=hashNodes.GetFirst();pNode;pNode=hashNodes.GetNext())
		{
			CONNECT_NODEINFO *pxConnNodeInfo=pNode->GetConnectNodeInfo();
			//CConnectPointInfoList *pListConnect=pNode->GetConnectPointInfoList();
			//for(CConnectPointInfo* pConnPoint=pListConnect->GetFirst();pConnPoint;pConnPoint=pListConnect->GetNext())
			for(i=0;i<4;i++)
			{
				CONNECT_PLANE* pConnPlane=pxConnNodeInfo->xCommNode.planes.GetAt(i);
				if (!pConnPlane->blValid||pConnPlane->designType.bIntersBolt)
					continue;	//	//��ƽ������˨
				if(fabs(pCurView->ucs.axis_z*pConnPlane->datumplane.vSolid)<0.9)//���������¶�̫�󣬵��������淨���뵱ǰ��ͼ���߼н����ҽ�СEPS_COS2)
					continue;
				CLDSNode *pBaseNode=Ta.Node.FromHandle(pxConnNodeInfo->xCommNode.hBaseNode);
				CSuperSmartPtr<CLDSLinePart>pDatumRod=(CLDSLinePart*)Ta.Parts.FromHandle(pxConnNodeInfo->xCommNode.hBaseRod);
				if(!pDatumRod->IsAngle()||pDatumRod->pStart==NULL||pDatumRod->pEnd==NULL)
					continue;
				if(pDatumRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
					continue;
				if(pConnPlane->xarrBasePlaneRods.Count<=1)
					continue;	//���Ǹֶ���˨���Ӳ���Ҫ�ֲ�ڵ�
				if(pConnPlane->xarrBasePlaneRods.Count>3)
					continue;

				CSingleRayEndBoltLj endbolt;
				CSingleRayEndBoltLj::BRANCH_NODE_TEST branchTest;
				CConnectPointInfo xConnPointInfo;
				pxConnNodeInfo->GetConnectPointInfo((short)i,&xConnPointInfo);
				if(endbolt.DesignConnectPoint(&xConnPointInfo,&branchTest)&&branchTest.pRods[0]!=NULL)
				{
					branch_hits++;
					//-------VV----------
					if(hashNodes.GetNodeNum()>1)
					{	//����ǰ�ڵ�λ�÷Ŵ���ʾ
						SCOPE_STRU scope;
						scope.VerifyVertex(pNode->xActivePos);
						scope.fMinX-=600;
						scope.fMaxX+=600;
						scope.fMinY-=600;
						scope.fMaxY+=600;
						scope.fMinZ-=600;
						scope.fMaxZ+=600;
						double fFocusZoomCoef=1.0/g_pSolidOper->GetScaleUserToScreen();
						g_pSolidOper->FocusTo(scope,fFocusZoomCoef);
						g_pSolidDraw->Draw();	//FocusTo�����������һ��Draw()������������FocusTo�ᵼ��OpenGL�ڲ�����,�������Ŵ��� wjh-2017.12.18
					}
					//-------^^----------
					short j,siLenOffset=branchTest.siLenOffset[0];
					short increment=0;	//ԭ������ƫ��ʱ�������һ������Ϊƫ��0����Ӧ����һ�����ƫ���Զ��ۼӵ��ڶ����� wjh-2017.12.18
					for(j=0;j<2;j++)
					{
						if(branchTest.pRods[j]==NULL)
							continue;
						if (branchTest.pDatumRods[j] != NULL)
							pDatumRod = branchTest.pDatumRods[j];
						GEPOINT lenStdOffsetVec = pDatumRod->pEnd->xPreliftPos - pDatumRod->pStart->xPreliftPos;
						normalize(lenStdOffsetVec);
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(xConnPointInfo.m_hBaseNode);
						g_pSolidDraw->SetEntSnapStatus(branchTest.pRods[j]->handle);
						if(j==1&&abs(branchTest.siLenOffset[j])>abs(siLenOffset))
						{	//ƫ����ȡ��ȡС�����Ҹ��߼������Ͻ��ʺ������߸˼�����'j==1'����Ϊ�˴���ɶ���.wjh-2019.10.14
							siLenOffset=branchTest.siLenOffset[j];
							if(siLenOffset>0)
								siLenOffset+=increment;
							else
								siLenOffset-=increment;
						}
						if(ciMirBranchNodeMode==0)
							pCmdLine->AttachMiniCmdOptionPanel(&xCmdMiniPanel,250);
						if(xCmdMiniPanel.m_bSpecOffset)	//����ָ��ֵʱ����Ĭ��Ϊ��һ��ָ��ƫ��ֵ
							siLenOffset=xCmdMiniPanel.m_nSpecOffset;
						pCmdLine->FillCmdLine((char*)CXhChar100("�˼��˽ڵ�0x%X��������ƫ��ֵ<%d>:",pNode->handle,abs(siLenOffset)),"");
						btc::SKETCH_PLANE sketch;
						GEPOINT vHoriVec=branchTest.pRods[j]->pEnd->xActivePos-branchTest.pRods[j]->pStart->xActivePos;
						if(branchTest.ciRodCurrEndS0E1[j]==1)
							vHoriVec*=-1;
						vHoriVec-=(vHoriVec*lenStdOffsetVec)*lenStdOffsetVec;
						normalize(vHoriVec);
						int sign=(branchTest.siLenOffset[j]<0)?-1:1;
						GEPOINT xSketchOrg=pBaseNode->xActivePos-vHoriVec*50+(abs(siLenOffset)*sign)*lenStdOffsetVec;
						sketch.CreateStdPlane(xSketchOrg,sign*lenStdOffsetVec,vHoriVec);
						g_pSolidDraw->NewWorkPlaneSketch(1,RGB(255,0,0),sketch.pVertexArr,sketch.VertexCount,sketch.normal,"",100,true,abs(siLenOffset));
						g_pSolidDraw->Draw();
						while(1)
						{
							CString cmdStr;
							if(!pCmdLine->GetStrFromCmdLine(cmdStr))
							{
								g_pSolidDraw->DelWorkPlaneSketch(1);
								return FALSE;
							}
							cmdStr.TrimLeft(" \t");
							if(cmdStr.GetLength()==0)
								break;
							siLenOffset= atoi(cmdStr);
							break;
						}
						xCmdMiniPanel.m_nSpecOffset=siLenOffset;
						if(ciMirBranchNodeMode==0)
							ciMirBranchNodeMode=xCmdMiniPanel.m_bMirAll?'Y':'N';
						pCmdLine->DetachMiniCmdOptionPanel();
						g_pSolidDraw->DelWorkPlaneSketch(1);
						if(siLenOffset!=0)
						{	//���ƫ�Ƶ�
							GEPOINT xCurrPos=pBaseNode->xPreliftPos+lenStdOffsetVec*(sign*abs(siLenOffset));
							CLDSNode* pNewNode = Ta.CheckOverlappedNode(xCurrPos,CFGINFO(pDatumRod->cfgword,pDatumRod->IsLegObj()));
							if(pNewNode==NULL)
							{
								pNewNode=console.AppendNode();
								pNewNode->m_cPosCalType = 7;
								pNewNode->arrRelationNode[0]=pBaseNode->handle;
								pNewNode->arrRelationNode[1]=pDatumRod->pStart->handle;
								pNewNode->arrRelationNode[2]=pDatumRod->pEnd->handle;
								pNewNode->arrRelationPole[0]=pDatumRod->handle;
								pNewNode->attach_offset=sign*abs(siLenOffset);
								pNewNode->SetPosition(xCurrPos);
								pNewNode->hFatherPart=pDatumRod->handle;
								pNewNode->cfgword=pDatumRod->cfgword;
								pNewNode->iSeg=pDatumRod->iSeg;
								pNewNode->SetLayer(pDatumRod->layer());
								pNewNode->layer(1)='P';
								NewNode(pNewNode);
							}
							branchTest.pOffsetNodes[j]=pNewNode;
							if(ciMirBranchNodeMode!='Y')
							{	//���ڲ���Ҫ�Գ�ʱ���������߽ǸֵĶ˽ڵ���Ϣ������Գ�ǰ���Ķ˽ڵ�ᵼ�²��ҶԳƸ˼�ʧ�� wjh-2017.12.17
								if(branchTest.ciRodCurrEndS0E1[j]==0)
									branchTest.pRods[j]->pStart=pNewNode;
								else
									branchTest.pRods[j]->pEnd=pNewNode;
								branchTest.pRods[j]->ClearFlag();
								branchTest.pRods[j]->CalPosition();
								branchTest.pRods[j]->Create3dSolidModel();
								g_pSolidDraw->NewSolidPart(branchTest.pRods[j]->GetSolidPartObject());
							}
						}
						else if(j==0&&branchTest.siLenOffset[j]!=0)
							increment=abs(branchTest.siLenOffset[j]);
						g_pSolidDraw->Draw();
					}
					if(ciMirBranchNodeMode=='Y')
					{
						for(RELATIVE_OBJECT* pRelaObj=pNode->relativeObjs.GetFirst();pRelaObj;pRelaObj=pNode->relativeObjs.GetNext())
						{
							CLDSNode* pMirNode=Ta.Node.FromHandle(pRelaObj->hObj);
							if(pMirNode==NULL)
								continue;
							for(j=0;j<2;j++)
							{
								if(branchTest.pRods[j]==NULL||branchTest.pOffsetNodes[j]==NULL)
									continue;
								CLDSLinePart* pMirRod=branchTest.pRods[j]->GetMirRod(pRelaObj->mirInfo);
								if(pMirRod==NULL)
									continue;
								CLDSNode* pMirOffsetNode=CreateSingleMirNode(branchTest.pOffsetNodes[j],pRelaObj->mirInfo,TRUE);
								if(pMirRod->pStart==pMirNode)
									pMirRod->pStart=pMirOffsetNode;
								else if(pMirRod->pEnd==pMirNode)
									pMirRod->pEnd=pMirOffsetNode;
								pMirRod->ClearFlag();
								pMirRod->CalPosition();
								pMirRod->Create3dSolidModel();
								g_pSolidDraw->NewSolidPart(pMirRod->GetSolidPartObject());
							}
							BOOL pushed=hashNodes.push_stack();
							hashNodes.DeleteNode(pMirNode->handle);
							if(pushed)
								hashNodes.pop_stack(pushed);
						}
						for(j=0;j<2;j++)
						{
							if(branchTest.pOffsetNodes[j]==NULL)
								continue;
							SetRelativeObj(branchTest.pOffsetNodes[j]);
							//�������߽ǸֵĶ˽ڵ���Ϣ
							if(branchTest.ciRodCurrEndS0E1[j]==0)
								branchTest.pRods[j]->pStart=branchTest.pOffsetNodes[j];
							else
								branchTest.pRods[j]->pEnd=branchTest.pOffsetNodes[j];
							branchTest.pRods[j]->ClearFlag();
							branchTest.pRods[j]->CalPosition();
							branchTest.pRods[j]->Create3dSolidModel();
							g_pSolidDraw->NewSolidPart(branchTest.pRods[j]->GetSolidPartObject());
						}
					}
				}
			}
		}
	}
	if(branch_hits==0)
		AfxMessageBox("û�з����Զ����������ѡ�нڵ㣡");
	g_pSolidDraw->ReleaseSnapStatus();
	if(hashNodes.GetNodeNum()>1)
		return TRUE;
	else
		return _BranchNodes(pLdsView);
}
void CLDSView::OnBranchNodes()
{
	Command("BranchNodes");
}
int CLDSView::BranchNodes()
{
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	return _BranchNodes(this);
}
void CLDSView::OnMergeNodes()
{
	m_nPrevCommandID=ID_MERGE_NODES;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat <merge nodes>";
#else
	m_sPrevCommandName="�ظ�<�ϲ��ڵ�>";
#endif
	CUndoOperObject undo(&Ta,true);
	int nCount=MergeMultiNodes();
	if(nCount>=1)
	{
		g_pSolidDraw->BuildDisplayList();
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox(CXhChar50("Already finished %d times node merging",nCount));
#else
		MessageBox(CXhChar50("�����%d�νڵ�ϲ�",nCount));
#endif
	}
	else
	{
		g_pSolidSet->SetDisplayType(DISP_LINE);//�л���������ʾģʽ
		m_curTask=TASK_MERGE_NODES;
		g_pSolidSet->SetDrawType(DRAW_LINK);
		//g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_POINT);
		MergeNodes();
	}
}
static int GetDenominator(double f,int precision,int maxDenominator=15)
{
	if(precision<2)
		precision=2;
	if(f<1)
	{
		double tolerance=pow(0.1,precision);
		if(1-f<tolerance)
			return 1;
		else if(fabs(0.5-f)<tolerance)
			return 2;	//val=1/2
		else
		{
			for(int divn=3;divn<=maxDenominator;divn++)
			{
				for(int j=1;j<divn;j++)
				{
					double div=j/(double)divn;
					if(fabs(div-f)<tolerance)
						return divn;	//val=j/divn;
				}
			}
		}
	}
	return 0;
}
void CLDSView::OnRelayoutScaleNodes()
{
	long *id_arr,i,nCount=g_pSolidSnap->GetLastSelectEnts(id_arr);
	CLDSNode* pNode;
	CUndoOperObject undo(&Ta,true);
	int maxfenmu=1;	//����ĸ
	NODESET nodeset;
	ARRAY_LIST<long>ids(0,100);
	for(i=0;i<nCount;i++)
	{
		pNode=Ta.Node.FromHandle(id_arr[i]);
		if(pNode==NULL||pNode->m_cPosCalType!=CLDSNode::COORD_SCALE)
			continue;
		CLDSLinePart* pFatherRod=Ta.FromRodHandle(pNode->hFatherPart);
		if(pFatherRod!=NULL&&(pFatherRod->layer(1)=='Z'||pFatherRod->layer(1)=='X'))
		{
			nodeset.append(pNode);
			ids.append(pNode->handle);
		}
	}
	for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{
		int fenmu=GetDenominator(pNode->attach_scale,3);
		if(fenmu>0&&fenmu%maxfenmu==0)
			maxfenmu=fenmu;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(ids.m_pData,ids.GetSize());
	CRelayoutScaleNodeDlg relayout;
	relayout.m_nSectCountNow=relayout.m_nSectCountDest=maxfenmu;
	if(relayout.DoModal()!=IDOK)
		return;
	double magnifycoef=relayout.m_nSectCountNow;
	magnifycoef/=relayout.m_nSectCountDest;
	for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{
		if(pNode->attach_scale<=relayout.m_fPivotScale)
			pNode->attach_scale*=magnifycoef;
		else
			pNode->attach_scale=1-(1-pNode->attach_scale)*magnifycoef;
		pNode->Unlock();
		pNode->Set();
	}
	g_pSolidDraw->BuildDisplayList(this);
	g_pSolidDraw->Draw();
}
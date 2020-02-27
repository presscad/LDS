//<LOCALE_TRANSLATE Confirm by hxr/>

//////////////////////////////////////////////////////////////////
//                  �����ź��������Ƽ����޹�˾
// �ļ���        :      ShankSectionModify.cpp
// �汾          :		V1.0
// Ŀ�ļ���Ҫ����:		��ӦLDS���󲿷ֹ����������˵�����
// ��������      :      2012.2.11
// �޸�����      :		2012.2.11
// ������        :
////////////////////////////////////////////////////////////////

// LDSView.cpp : implementation of the CLDSView class
//

#include "stdafx.h"
#include "LDS.h"
#include "Tower.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "GUID.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "MainFrm.h"
#include "PromptDlg.h"
#include "UserDefMsg.h"
#include "database.h"
#include "direct.h"
#include "DefinedView.h"
#include "image.h"
#include "lds_part.h"
#include "Query.h"
#include "LmaDlg.h"
#include "MirTaAtom.h"
#include "LibraryQuery.h"
#include "SortFunc.h"
#include "FuncLifeObject.h"
#include "LogFile.h"
#include "LifeObj.h"
#include "NewTowerDlg.h"
#include "ShankDefCls.h"
#include "InsertStdTemplateOperation.h"
#include "AddLegBraceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//���˵��
//���ܣ�ʵ����������ӶΣ��ڼ䣩��ɾ�Σ��ڼ䣩���޸Ľڼ䴹ֱ�߶�
//�������ӹ������в�׽һ�������ڵ㣨�ɲ��ղ����������룩���Ӷλ��޸Ľڼ䴹ֱ�߶�ʱӦ��ʾ����ڼ䴹ֱ�߶�
//����:
//	1.�ӶΣ��Ӷ�ˮƽλ�ô���һ���Ľڵ㣬�Ӷδ���
//  2.ɾ�Σ�ɾ���ڼ����ֹ���Ľڵ�
//  3.�ĶΣ��޸Ľڼ����ֹ���Ľڵ㼰�´�ֱ�߶�
//���������ڵ�ƽ�����µ����й�����λ�ü��ߴ�Ӧ��ɸ���

/////////////////////////////////////////////////////////////////////////////
// CLDSView

//����λ��
//������λ
//�жϽڵ��Ƿ�Ϊ��Ҫ�������Ŀ��ƽڵ�
BOOL IsMainShankNode(CLDSNode* pNode)
{
	//������
	//1.�����˼�Ϊ��������
	//2.�ڵ���������ӦΪָ��Z��������ڵ�
	if(pNode==NULL||(pNode->Layer(0)!='S'&&pNode->Layer(0)!='L')||isdigit(pNode->layer(2))==0)
		return FALSE;
	if(pNode->m_cPosCalType==0||pNode->m_cPosCalType==3||			//3.�˼���Z����ֵ�����
		(pNode->m_cPosCalType==7&&pNode->m_cPosCalViceType=='Z'))	//7.�ظ˼���ĳ�ڵ�Ĵ�ֱƫ�Ƶ�
	{
		CSuperSmartPtr<CLDSPart>pMainRod=Ta.Parts.FromHandle(pNode->hFatherPart);
		if(pMainRod.IsHasPtr()&&pMainRod->IsLinePart()&&
			(pMainRod->Layer(0)=='S'||pMainRod->Layer(0)=='L')&&
			pMainRod->layer(1)=='Z'&&isdigit(pMainRod->layer(2))!=0)
			return TRUE;
		else
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("the dependent line part of the selected quadrant angle node 0X%X is not tower leg member, operation between nodes such as adding and deleting can be executed.",pNode->handle);
	}
	else if(pNode->m_cPosCalType==5)
		logerr.Log("positioning type of the selected quadrant angle nodes 0X%X are scale nodes, operation between nodes such as adding and deleting can be executed.",pNode->handle);
#else
			logerr.Log("ѡ�����޽ǽڵ�0X%X�������˼����������ȣ����ģ����ܽ��нڼ����ɾ�Ĳ���",pNode->handle);
	}
	else if(pNode->m_cPosCalType==5)
		logerr.Log("ѡ�����޽ǽڵ�0X%X�Ķ�λ����Ϊ�����ȷֽڵ㣬���ܽ��нڼ����ɾ�Ĳ���",pNode->handle);
#endif
	return FALSE;
}

CLDSLinePart* FindDownDatumMainRod(CLDSNode* pUpNode)
{
	BOOL pushed=Ta.Parts.push_stack();
	for(CLDSLinePart* pRod=Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
	{
		if(pRod->pStart!=pUpNode&&pRod->pEnd!=pUpNode)
			continue;
		if((pRod->layer(0)=='S'||pRod->layer(0)=='L')&&pRod->cfgword.And(pUpNode->cfgword)&&
			pRod->layer(1)=='Z'&&pRod->layer(2)==pUpNode->layer(2))
		{
			if((pRod->pStart->Position(false).z+pRod->pEnd->Position(false).z)>pUpNode->Position(false).z*2)
				return pRod;
			else 
				continue;
		}
	}
	Ta.Parts.pop_stack(pushed);
	return NULL;
}

CLDSLinePart* FindUpDatumMainRod(CLDSNode* pBtmNode)
{
	BOOL pushed=Ta.Parts.push_stack();
	for(CLDSLinePart* pRod=Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
	{
		if(pRod->pStart!=pBtmNode&&pRod->pEnd!=pBtmNode)
			continue;
		if((pRod->layer(0)=='S'||pRod->layer(0)=='L')&&pRod->cfgword.And(pBtmNode->cfgword)&&
			pRod->layer(1)=='Z'&&pRod->layer(2)==pBtmNode->layer(2))
		{
			if((pRod->pStart->Position(false).z+pRod->pEnd->Position(false).z)<pBtmNode->Position(false).z*2)
				return pRod;
			else 
				continue;
		}
	}
	Ta.Parts.pop_stack(pushed);
	return NULL;
}
CHashList<double>hashNodeOffsetCoordZ;
static void InitNodeOffsetCoordZ(CLDSNode *pNode,double z_level,double z_offset)
{
	if(pNode==NULL||(pNode->Layer(0)!='S'&&pNode->Layer(0)!='L'))//||isdigit(pNode->layer(2))==0)
		return;	//��Ӧ�жϵ������ַ���������ַ�����ʱ����������ͷ���ڵ����������� wjh-2013.6.5
	if(pNode->m_cPosCalType==0||pNode->m_cPosCalType==3)
	{	//�������κ������ڵ㼰ָ��Z��������ڵ�ֱ���ۼ�Z����
		hashNodeOffsetCoordZ.SetValue(pNode->handle,z_offset);
	}
	else if(pNode->m_cPosCalType==7)
	{	//��Z��ƫ��ָ�����߽ڵ�����ݻ�׼�ڵ��Ƿ��ѽ��й�ƫ�����ж��Ƿ���Ҫ����ƫ����
		CLDSNode *pDatumNode=Ta.Node.FromHandle(pNode->arrRelationNode[0]);
		CLDSNode *pRelaNodeStart=Ta.Node.FromHandle(pNode->arrRelationNode[1]);
		CLDSNode *pRelaNodeEnd=Ta.Node.FromHandle(pNode->arrRelationNode[2]);
		if(pDatumNode&&pDatumNode->Position(false).z>z_level)
			return;	//����λ�ڲ�����׼ƽ��֮��,��ǰ����Զ������ƫ��
		else if(pDatumNode)
		{	//ֻƫ����ʼ��׼�ڵ�δ����ƫ������µĴ�ֱƫ�Ƶ��ƫ�Ʒ���
			if(pRelaNodeEnd&&pRelaNodeEnd->Position(false).z<pRelaNodeStart->Position(false).z)
				hashNodeOffsetCoordZ.SetValue(pNode->handle,-z_offset);
			else
				hashNodeOffsetCoordZ.SetValue(pNode->handle,+z_offset);
		}
	}
}
static void OffsetNodeCoordZ(CLDSNode *pNode)
{
	if(pNode==NULL||(pNode->Layer(0)!='S'&&pNode->Layer(0)!='L'))//||isdigit(pNode->layer(2))==0)
		return;	//��Ӧ�жϵ������ַ���������ַ�����ʱ����������ͷ���ڵ����������� wjh-2013.6.5
	if(pNode->m_cPosCalType!=0&&pNode->m_cPosCalType!=3&&pNode->m_cPosCalType!=7)
		return;
	double *pOffsetZ=hashNodeOffsetCoordZ.GetValue(pNode->handle);
	if(pOffsetZ==NULL)
		return;
	if(pNode->m_cPosCalType==0)
	{
		CSuperSmartPtr<CLDSPart> pRod=Ta.Parts.FromHandle(pNode->hFatherPart);
		if(pRod.IsHasPtr()&&pRod->IsLinePart()&&pRod.LinePartPointer()->pStart&&pRod.LinePartPointer()->pEnd)
		{	//�л�׼�˼�ʱ�������������ڵ���ػ�׼�˼���������ƫ��һ������
			f3dPoint line_vec=pRod.LinePartPointer()->pEnd->Position(false)-pRod.LinePartPointer()->pStart->Position(false);
			normalize(line_vec);
			if(line_vec.z*(*pOffsetZ)<0)
				line_vec*=-1.0;
			if(fabs(line_vec.z)>EPS)
			{
				f3dPoint pos=pNode->Position(false)+line_vec*((*pOffsetZ)/line_vec.z);
				pNode->SetPosition(pos);
			}
			else	//�����ˣ�һ�㲻������
				pNode->SetPositionZ(pNode->Position(false).z+*pOffsetZ);
		}
		else if(pNode->layer(0)!='T')	//��ͷ���ڵ�, ֱ�ӽ���Z����ƫ��
		{
			if(pRod.IsNULL())
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X nodes lack of parent rods.will influence its coordinate updating results!\n",pNode->handle);
			else if(pRod.LinePartPointer()->pStart==NULL)
				logerr.Log("0X%X rods lack of starting nodes.will influence the node's0X%X coordinate updating results!\n",pRod->handle,pNode->handle);
			else if(pRod.LinePartPointer()->pEnd==NULL)
				logerr.Log("0X%X rods lack of ending nodes.will influence the node's0X%X coordinate updating results!\n",pRod->handle,pNode->handle);
#else
				logerr.Log("0X%X�ڵ�ȱ�ٸ��˼�����Ӱ����������½����\n",pNode->handle);
			else if(pRod.LinePartPointer()->pStart==NULL)
				logerr.Log("0X%X�˼�ȱ��ʼ�˽ڵ㣬��Ӱ��ڵ�0X%X��������½����\n",pRod->handle,pNode->handle);
			else if(pRod.LinePartPointer()->pEnd==NULL)
				logerr.Log("0X%X�˼�ȱ���ն˽ڵ㣬��Ӱ��ڵ�0X%X��������½����\n",pRod->handle,pNode->handle);
#endif
			pNode->SetPositionZ(pNode->Position(false).z+*pOffsetZ);
		}
	}
	else if(pNode->m_cPosCalType==3)
	{	//�������κ������ڵ㼰ָ��Z��������ڵ�ֱ���ۼ�Z����
		pNode->SetPositionZ(pNode->Position(false).z+*pOffsetZ);
	}
	else if(pNode->m_cPosCalType==7)
	{	//��Z��ƫ��ָ�����߽ڵ�����ݻ�׼�ڵ��Ƿ��ѽ��й�ƫ�����ж��Ƿ���Ҫ����ƫ����
		pNode->attach_offset+=*pOffsetZ;
	}
	try
	{
		pNode->Set();
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
}
#ifdef __ANGLE_PART_INC_
//���ýǸ�ָ���˶�λ��ʽΪƽ��
static void SetAnglePosFaceOffset(CLDSLineAngle *pAngle,BOOL bStart,
								   long hStartAngle,long hEndAngle)
{
	if(pAngle==NULL)
		return;
	CMultiOffsetPos *pDesPos=&pAngle->desStartPos;
	if(!bStart)
		pDesPos=&pAngle->desEndPos;
	if(pDesPos==NULL)
		return;
	pDesPos->spatialOperationStyle=17;	//����ƽ��
	pDesPos->datumLineStart.SetRapidAngleBer(hStartAngle,0);
	pDesPos->datumLineEnd.SetRapidAngleBer(hEndAngle,1);
	/*pDesPos->start_jg_h=hStartAngle;
	pDesPos->end_jg_h=hEndAngle;
	pDesPos->iStartJgBerStyle=0;
	pDesPos->iEndJgBerStyle=1;
	pDesPos->fStartJgLengOffsetX=
		pDesPos->fStartJgLengOffsetY=
		pDesPos->fEndJgLengOffsetX=
		pDesPos->fEndJgLengOffsetY=0;*/
	pDesPos->face_offset_norm.Set(0,0,1);
	//pDesPos->bByOtherEndOffset=FALSE;
	pDesPos->cFaceOffsetDatumLine='X';
	if((pAngle->pEnd->Position(false).x<0&&pAngle->pEnd->Position(false).y>0)||
		(pAngle->pEnd->Position(false).x>0&&pAngle->pEnd->Position(false).y<0))
		pDesPos->cFaceOffsetDatumLine='Y';	
}
//���ýǸ�λ����Ʋ���Ϊ������ͶӰ
static void SetAnglePosProjectToBer(CLDSLineAngle *pAngle,BOOL bStart)
{
	if(pAngle==NULL)
		return;
	CMultiOffsetPos *pDesPos=&pAngle->desStartPos;
	if(!bStart)
		pDesPos=&pAngle->desEndPos;
	if(pDesPos==NULL)
		return;
	pDesPos->jgber_cal_style=0;		//��㶨λ��ʽ:������ƫ��
	pDesPos->origin_pos_style=0;
	pDesPos->spatialOperationStyle=1;		//���㶨λ��ʽ:������ͶӰ
	pDesPos->wing_x_offset.offsetDist=0;
	pDesPos->wing_y_offset.offsetDist=0;
	pDesPos->wing_x_offset.gStyle=4;
	pDesPos->wing_y_offset.gStyle=4;
}
#endif

//���øֹܶ�λ��ʽΪ����ͶӰ
#ifdef __COMPLEX_PART_INC_
static void SetTubePosProjectPoint(CLDSLineTube *pTube,BOOL bStart,long hStart,long hEnd)
{
	if(pTube==NULL)
		return;
	CTubeEndPosPara *pDesPos=&pTube->desStartPos;
	if(!bStart)
		pDesPos=&pTube->desEndPos;
	if(pDesPos==NULL)
		return;
	pDesPos->endPosType=2;	//����ͶӰ
	pDesPos->bDatumStartStartEnd=TRUE;
	pDesPos->bDatumEndStartEnd=FALSE;
	pDesPos->hDatumStartPart=hStart;
	pDesPos->hDatumEndPart=hEnd;
}
#endif
static void OperSectionDump(void* lpDumpInfo)
{
	g_pSolidSnap->SetSelectPartsType(g_sysPara.m_dwSelectPartsType);
	((CLDSView*)lpDumpInfo)->OperOther();
}
void ReplaceDependencyNodeRelaInfo(double fTopZ,long hOldNode,long hNewNode)
{
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->Position(false).z<=fTopZ)
			continue;	//ֻ����ֽ�ƽ��֮�µĽڵ�
		if(pNode->m_cPosCalType==0||pNode->m_cPosCalType==3||
			pNode->m_cPosCalType==7&&pNode->m_cPosCalViceType=='Z')
			continue;	//�Ѵ����
		for(int i=0;i<4;i++)
		{
			if(pNode->arrRelationNode[i]==hOldNode)
				pNode->arrRelationNode[i]=hNewNode;
		}
	}
}
static bool RetrieveTrunkDependsRodsAndNodes(CShankDefine::DEFINE_MODE* pDefMode,
	CLDSLinePart* pDatumRod, CLDSNode* pQuad1KeyNode, CLDSNode* pQuad1SlopePickNode)
{
	CLDSNode* pNode,*pAnchorNode=NULL;
	CLDSLineAngle* pDatumAngle = pDatumRod->IsAngle() ? (CLDSLineAngle*)pDatumRod : NULL;
	//��׼���߶���ê���ڵ�
	pDefMode->xarrTopNodesPtr[0] = pQuad1KeyNode;
	pDefMode->xarrTopNodesPtr[1] = pQuad1KeyNode->GetMirNode(MIRMSG(2));
	pDefMode->xarrTopNodesPtr[2] = pQuad1KeyNode->GetMirNode(MIRMSG(1));
	pDefMode->xarrTopNodesPtr[3] = pQuad1KeyNode->GetMirNode(MIRMSG(3));
	if (pQuad1SlopePickNode->m_cPosCalType == CLDSNode::COORD_Z_SPECIFIED)
	{
		pNode = Ta.Node.FromHandle(pQuad1SlopePickNode->arrRelationNode[0]);
		if (pNode!=NULL&&pNode->xOriginalPos.z > pQuad1SlopePickNode->xOriginalPos.z)
			pAnchorNode = pNode;
		else
		{
			pNode = Ta.Node.FromHandle(pQuad1SlopePickNode->arrRelationNode[1]);
			if (pNode != NULL && pNode->xOriginalPos.z > pQuad1SlopePickNode->xOriginalPos.z)
				pAnchorNode = pNode;
		}
	}
	if (pAnchorNode==NULL)
	{	//δͨ���򵥷�ʽ�ҵ��ײ�ê���ڵ�
		CPtInLineCheck check(pQuad1KeyNode->xOriginalPos, pQuad1SlopePickNode->xOriginalPos);
		CMaxDouble btmnode;
		for (pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
		{
			if (pNode->m_bVirtualObj|| !pQuad1KeyNode->ModuleTogetherWith(pNode) ||
				pNode->xOriginalPos.x<=0||pNode->xOriginalPos.y<=0)
				continue;
			bool blOnline = false;
			double fraction=check.CheckPointEx(pNode->xOriginalPos, &blOnline, 1);
			if (blOnline)
				btmnode.Update(fraction, pNode);
		}
		if (btmnode.IsInited())
			pAnchorNode = (CLDSNode*)btmnode.m_pRelaObj;
	}
	if (pAnchorNode == NULL)
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON, "δ�ҵ�����ײ��Ĺ���ê����׼�ڵ�");
		return false;
	}
	pDefMode->xarrBtmNodesPtr[0]=pAnchorNode;
	if ((pDefMode->xarrBtmNodesPtr[1] = pAnchorNode->GetMirNode(MIRMSG(2))) == NULL)
	{
		pDefMode->xarrBtmNodesPtr[1] = Ta.AppendNode();
		pDefMode->xarrBtmNodesPtr[1]->CopyProperty(pAnchorNode);
		pDefMode->xarrBtmNodesPtr[1]->layer(2) = '2';
		pDefMode->xarrBtmNodesPtr[1]->m_cPosCalType = 12;
		pDefMode->xarrBtmNodesPtr[1]->m_cPosCalViceType = 'Y';
		pDefMode->xarrBtmNodesPtr[1]->SetPosition(GEPOINT(-pAnchorNode->xOriginalPos.x, pAnchorNode->xOriginalPos.y, pAnchorNode->xOriginalPos.z));
		pDefMode->xarrBtmNodesPtr[1]->cfgword = pAnchorNode->cfgword;
	}
	if ((pDefMode->xarrBtmNodesPtr[2] = pAnchorNode->GetMirNode(MIRMSG(1))) == NULL)
	{
		pDefMode->xarrBtmNodesPtr[2] = Ta.AppendNode();
		pDefMode->xarrBtmNodesPtr[2]->CopyProperty(pAnchorNode);
		pDefMode->xarrBtmNodesPtr[2]->layer(2) = '3';
		pDefMode->xarrBtmNodesPtr[2]->m_cPosCalType = 12;
		pDefMode->xarrBtmNodesPtr[2]->m_cPosCalViceType = 'X';
		pDefMode->xarrBtmNodesPtr[2]->SetPosition(GEPOINT(pAnchorNode->xOriginalPos.x, -pAnchorNode->xOriginalPos.y, pAnchorNode->xOriginalPos.z));
		pDefMode->xarrBtmNodesPtr[2]->cfgword = pAnchorNode->cfgword;
	}
	if((pDefMode->xarrBtmNodesPtr[3] = pAnchorNode->GetMirNode(MIRMSG(3)))==NULL)
	{
		pDefMode->xarrBtmNodesPtr[3] = Ta.AppendNode();
		pDefMode->xarrBtmNodesPtr[3]->CopyProperty(pAnchorNode);
		pDefMode->xarrBtmNodesPtr[3]->layer(2) = '4';
		pDefMode->xarrBtmNodesPtr[3]->m_cPosCalType = 12;
		pDefMode->xarrBtmNodesPtr[3]->m_cPosCalViceType = 'Z';
		pDefMode->xarrBtmNodesPtr[3]->SetPosition(GEPOINT(-pAnchorNode->xOriginalPos.x, -pAnchorNode->xOriginalPos.y, pAnchorNode->xOriginalPos.z));
		pDefMode->xarrBtmNodesPtr[3]->cfgword = pAnchorNode->cfgword;
	}
	if (pDatumAngle == NULL)
	{
		memset(pDefMode->xarrBtmLegRodsPtr, 0, sizeof(CLDSLinePart*) * 4);
		return true;
	}
	if (pDatumAngle->desStartPos.spatialOperationStyle == 16 || pDatumAngle->desStartPos.spatialOperationStyle == 17)
	{
		CLDSLineAngle* pAngle = (CLDSLineAngle*)Ta.FromPartHandle(pDatumAngle->desStartPos.datumLineStart.des_para.hPart, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
		if (pAngle->xPosStart.z > pQuad1SlopePickNode->xOriginalPos.z || pAngle->xPosEnd.z > pQuad1SlopePickNode->xOriginalPos.z)
			pDefMode->xarrBtmLegRodsPtr[0] = pAngle;
		if (pDefMode->xarrBtmLegRodsPtr[0] == NULL)
		{
			pAngle = (CLDSLineAngle*)Ta.FromPartHandle(pDatumAngle->desStartPos.datumLineEnd.des_para.hPart, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
			if (pAngle->xPosStart.z > pQuad1SlopePickNode->xOriginalPos.z || pAngle->xPosEnd.z > pQuad1SlopePickNode->xOriginalPos.z)
				pDefMode->xarrBtmLegRodsPtr[0] = pAngle;
		}
	}
	else if (pDatumAngle->desEndPos.spatialOperationStyle == 16 || pDatumAngle->desEndPos.spatialOperationStyle == 17)
	{
		CLDSLineAngle* pAngle = (CLDSLineAngle*)Ta.FromPartHandle(pDatumAngle->desEndPos.datumLineStart.des_para.hPart, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
		if (pAngle->xPosStart.z > pQuad1SlopePickNode->xOriginalPos.z || pAngle->xPosEnd.z > pQuad1SlopePickNode->xOriginalPos.z)
			pDefMode->xarrBtmLegRodsPtr[0] = pAngle;
		if (pDefMode->xarrBtmLegRodsPtr[0] == NULL)
		{
			pAngle = (CLDSLineAngle*)Ta.FromPartHandle(pDatumAngle->desEndPos.datumLineEnd.des_para.hPart, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
			if (pAngle->xPosStart.z > pQuad1SlopePickNode->xOriginalPos.z || pAngle->xPosEnd.z > pQuad1SlopePickNode->xOriginalPos.z)
				pDefMode->xarrBtmLegRodsPtr[0] = pAngle;
		}
	}
	if (pDefMode->xarrBtmLegRodsPtr[0] == NULL)
	{	//����ʵ��˵�������ҵײ���λ�˼�
		f3dLine line(pDatumRod->xPosStart, pDatumRod->xPosEnd);
		if(pDatumRod->xPosStart.z>pDatumRod->xPosEnd.z)
			line=f3dLine(pDatumRod->xPosEnd, pDatumRod->xPosStart);
		CMaxDouble btmrod;
		CPtInLineCheck check(line.startPt, line.endPt);
		for (CLDSLinePart* pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
		{
			if (pRod->pStart == NULL || pRod->pEnd == NULL ||
				pRod->m_bVirtualPart || !pDatumRod->ModuleTogetherWith(pRod))
				continue;
			if (pRod->xPosStart.z < pQuad1KeyNode->xOriginalPos.z - 500)
				continue;	//�˼�ʼ�˵��ʰȡ�����
			if (pRod->xPosEnd.z < pQuad1KeyNode->xOriginalPos.z - 500)
				continue;	//�˼��ն˵��ʰȡ�����
			GEPOINT vLenStdVec= pRod->xPosEnd - pRod->xPosStart;
			normalize(vLenStdVec);
			double justify = vLenStdVec * check.LineStdVec();
			if (fabs(justify) < EPS_COS2)
				continue;	//������
			bool blOnline = false;
			GEPOINT xLowTipPoint = pRod->xPosStart.z > pRod->xPosEnd.z ? pRod->xPosStart : pRod->xPosEnd;
			double fraction = check.CheckPointEx(xLowTipPoint, &blOnline, 2);
			if (!blOnline)
				continue;
			btmrod.Update(fraction, pRod);
		}
		if (btmrod.IsInited())
			pDefMode->xarrBtmLegRodsPtr[0] = (CLDSLinePart*)btmrod.m_pRelaObj;
	}
	if (pDefMode->xarrBtmLegRodsPtr[0] == NULL)
		return false;
	pDefMode->xarrBtmLegRodsPtr[1] = pDefMode->xarrBtmLegRodsPtr[0]->GetMirRod(MIRMSG(2));
	pDefMode->xarrBtmLegRodsPtr[2] = pDefMode->xarrBtmLegRodsPtr[0]->GetMirRod(MIRMSG(1));
	pDefMode->xarrBtmLegRodsPtr[3] = pDefMode->xarrBtmLegRodsPtr[0]->GetMirRod(MIRMSG(3));
	if (pDefMode->xarrBtmLegRodsPtr[1] == NULL)
		logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON, "�ײ���׼�˼�ȱ����2���޶Գ�");
	if (pDefMode->xarrBtmLegRodsPtr[2] == NULL)
		logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON, "�ײ���׼�˼�ȱ����3���޶Գ�");
	if (pDefMode->xarrBtmLegRodsPtr[3] == NULL)
		logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON, "�ײ���׼�˼�ȱ����4���޶Գ�");
	return pDefMode->xarrBtmLegRodsPtr[1] != NULL && pDefMode->xarrBtmLegRodsPtr[2] != NULL && pDefMode->xarrBtmLegRodsPtr[3] != NULL;
}
static bool GetStdCornerLineVecOfQuad1(NODESET &keynodeset, CShankDefine::DEFINE_MODE* pDefMode)
{
	CLDSNode* pKeyNode, *pQuad1KeyNode = NULL, *pSlopePickNode=NULL;
	for (pKeyNode = keynodeset.GetFirst(); pKeyNode; pKeyNode = keynodeset.GetNext())
	{
		if (pKeyNode->xOriginalPos.x > 0 && pKeyNode->xOriginalPos.y > 0)
			break;
	}
	GEPOINT vStdLineVec;
	CLDSLinePart* pFatherRod = pKeyNode==NULL?NULL:(CLDSLinePart*)Ta.Parts.FromHandle(pKeyNode->hFatherPart, CLS_LINEPART);
	if (pFatherRod == NULL || pFatherRod->pStart == NULL || pFatherRod->pEnd == NULL)
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "δ�ҵ����ʵ���������1���޹ؼ��ڵ�");
		return false;
	}
	pQuad1KeyNode = pKeyNode;
	vStdLineVec = pFatherRod->pEnd->xOriginalPos - pFatherRod->pStart->xOriginalPos;
	CLDSLinePart* pUpLinkRod = NULL, *pDownLinkRod = NULL;
	if ((pFatherRod->pStart == pKeyNode && vStdLineVec.z > 0) || (pFatherRod->pEnd == pKeyNode && vStdLineVec.z < 0))
	{
		pDownLinkRod = pFatherRod;
		pSlopePickNode = vStdLineVec.z > 0 ? pDownLinkRod->pEnd : pDownLinkRod->pStart;
	}
	else if ((pFatherRod->pStart == pKeyNode && vStdLineVec.z < 0) || (pFatherRod->pEnd == pKeyNode && vStdLineVec.z > 0))
		pUpLinkRod = pFatherRod;
	else
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "��ǰѡ�еĹؼ��ڵ���������ĸ˼��˽ڵ�");
		return false;
	}
	if (pDownLinkRod==NULL)
	{	//������ڵ㸸�˼����Ӧ����һ����������
		normalize(vStdLineVec);
		if (vStdLineVec.z < 0)
			vStdLineVec *= -1.0;
		CFGWORD cfgword = pFatherRod->UnifiedCfgword();
		CMaxDouble bestrod;
		for (CLDSLinePart* pCoupleRod = Ta.EnumRodFirst(); pCoupleRod;pCoupleRod = Ta.EnumRodNext())
		{
			if (pCoupleRod == pFatherRod || pCoupleRod->GetClassTypeId() != pFatherRod->GetClassTypeId())
				continue;
			if (!pFatherRod->ModuleTogetherWith(pCoupleRod) || 
				pFatherRod->blGroupSonAngle||pFatherRod->m_bVirtualPart||
				pCoupleRod->pStart == NULL || pCoupleRod->pEnd == NULL)
				continue;
			GEPOINT vCurrLineVec;
			if (pCoupleRod->pStart == pKeyNode)
				vCurrLineVec = pCoupleRod->pEnd->xOriginalPos - pCoupleRod->pStart->xOriginalPos;
			else if (pCoupleRod->pEnd == pKeyNode)
				vCurrLineVec = pCoupleRod->pStart->xOriginalPos - pCoupleRod->pEnd->xOriginalPos;
			else
				continue;
			if (vCurrLineVec.IsZero())
				continue;
			normalize(vCurrLineVec);
			double dd = vCurrLineVec * vStdLineVec;
			if (dd < 0)
				continue;
			bestrod.Update(dd, pCoupleRod);	//���ǵ����´����˼���һ�����ߣ�����ѡ����ӽ����ߵ��²�˼� wjh-2019.5.22
			/*if (pUpLinkRod == NULL && dd < -EPS_COS)
				pUpLinkRod = pCoupleRod;
			else if (pDownLinkRod == NULL && dd > EPS_COS)
				pDownLinkRod = pCoupleRod;*/
			if (dd>EPS_COS2)
				break;	//�����ܴ��ڸ��ӽ��ĽǸ�
		}
		if (bestrod.IsInited())
			pDownLinkRod = (CLDSLinePart*)bestrod.m_pRelaObj;
	}
	if (pDownLinkRod == NULL)
		return false;
	else
		vStdLineVec = pDownLinkRod->pEnd->xOriginalPos - pDownLinkRod->pStart->xOriginalPos;
	pSlopePickNode = vStdLineVec.z > 0 ? pDownLinkRod->pEnd : pDownLinkRod->pStart;
	if (vStdLineVec.z < 0)
		vStdLineVec *= -1.0;
	normalize(vStdLineVec);
	pDefMode->dfGradientDetaX2H = vStdLineVec.x / vStdLineVec.z;
	pDefMode->dfGradientDetaY2H = vStdLineVec.y / vStdLineVec.z;
	return RetrieveTrunkDependsRodsAndNodes(pDefMode, pDownLinkRod, pQuad1KeyNode, pSlopePickNode);
}
bool CorrectSingleAngleLayout(CLDSLineAngle* pJg, bool updatePartSolid = true);
//��������ڼ�
static double InsertShankSection(NODESET &keynodeset,char cDefaultSubType)
{
	CFGWORD actCfgword;
	CLDSModule* pModule=NULL;
	int idBodyNo = 0;
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
		idBodyNo = max(idBodyNo, pModule->idBodyNo);
	if(cDefaultSubType=='B')
	{
		idBodyNo++;
		if (idBodyNo > CFGLEG::MaxBodys())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("beyond current maximum limit of module!");
#else
			AfxMessageBox("�ѳ�����ǰ������������!���ֶ��л�����-����ģʽ������Ӻ���");
#endif
			return 0;
		}
		//LDS�����Ŀ¼ʽ��ģʽ�������,TMA����л��ǰ��վ�ģʽ�������
#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
		CNewTowerDlg newheightdlg;
		newheightdlg.m_xShankDef.mode.blAppendMode = true;
		newheightdlg.m_xShankDef.mode.ciInitHeightSerial = idBodyNo;
		if (GetStdCornerLineVecOfQuad1(keynodeset, &newheightdlg.m_xShankDef.mode))
		{	//��Ŀ¼ʽ��ģʽ�������
			BYTE ciMultiLegSchema = CFGLEG::Schema();
			newheightdlg.m_iMultiLegSchema=ciMultiLegSchema-1;
			if (newheightdlg.DoModal() != IDOK)
				return 0;
			CFGLEG::SetSchema(ciMultiLegSchema);
			BYTE cnNewLegsPerHeight = (BYTE)newheightdlg.m_xShankDef.MaxLegsPerHeightGroup();
			if (cnNewLegsPerHeight > CFGLEG::MaxLegs())
			{
				if (cnNewLegsPerHeight <= 16)
					ciMultiLegSchema = 2;
				else if (cnNewLegsPerHeight <= 24)
					ciMultiLegSchema = 3;
				else
				{
					AfxMessageBox(CXhChar50("ϵͳ��ʱ��֧�ֵ�һ����������%d�����", cnNewLegsPerHeight));
					return 0;
				}
			}
			CFGLEG::SetSchema(ciMultiLegSchema);
			//�������׿����½�����ʱ��ʱ�ݲ����ü򵥶�λģʽ
			CStackVariant<BOOL> stackmode(&g_sysPara.m_bAnglePreferSimplePosMode, FALSE);
			CShankDefineToTowerModel modeler;
			{
				Ta.Parts.GetTail();
				CLdsPartListStack stack(&Ta.Parts);
				if (!modeler.UpdateTowerModel(&Ta, &newheightdlg.m_xShankDef, 4))
					return 0;
			}
			SCOPE_STRU scope;
			//Ӧ�Զ�ִ�нǸְڷ�λ������
			for (SmartRodPtr pRod = Ta.EnumRodNext(); pRod.blValid; pRod = Ta.EnumRodNext())
			{
				scope.VerifyVertex(pRod->pStart->xOriginalPos);
				scope.VerifyVertex(pRod->pEnd->xOriginalPos);
				if (pRod->layer(1) == 'Z' || !pRod->IsAngle())
					continue;	//������Ȳ����Ĳ�����
				CorrectSingleAngleLayout(pRod.pAngle);
			}
			//���²����������������ͷ���ֵ���ĺ� wjh-2019.5.23
			CLDSNode* pPickKeyNode = keynodeset.GetFirst();
			double zInsertPlane = newheightdlg.m_xShankDef.mode.dfTopCoordZ;
			for (CLDSNode* pNode = Ta.EnumNodeFirst(); pNode; pNode = Ta.EnumNodeNext()) 
			{
				if (!pNode->ModuleTogetherWith(pPickKeyNode))
					continue;	//��������ؼ��ڵ㲻����
				if (pNode->cfgword.IsHasBodyNo(idBodyNo) || pNode->IsLegObj())
					continue;	//�����ڵ����
				if (pNode->xOriginalPos.z > zInsertPlane + EPS2)
					continue;	//�ڵ��ڲ����������
				pNode->cfgword.AddBodyLegs(idBodyNo);
			}
			for (SmartPartPtr pSmartPart = Ta.EnumPartFirst(); pSmartPart.blValid; pSmartPart = Ta.EnumPartNext())
			{
				if (!pSmartPart->ModuleTogetherWith(pPickKeyNode))
					continue;	//��������ؼ��ڵ㲻����
				if (pSmartPart->cfgword.IsHasBodyNo(idBodyNo) || pSmartPart->IsLegObj())
					continue;	//������������
				if (pSmartPart->IsLinePart())
				{
					if (pSmartPart.pRod->pStart&&pSmartPart.pRod->pStart->xOriginalPos.z > zInsertPlane + EPS2)
						continue;	//ʼ���ڲ����������
					if (pSmartPart.pRod->pEnd&&pSmartPart.pRod->pEnd->xOriginalPos.z > zInsertPlane + EPS2)
						continue;	//ĩ���ڲ����������
					if (pSmartPart.pRod->pStart==NULL&&pSmartPart.pRod->xPosStart.z > zInsertPlane + EPS2)
						continue;	//ʼ�˵��ڲ����������
					if (pSmartPart.pRod->pEnd == NULL &&pSmartPart.pRod->xPosEnd.z > zInsertPlane + EPS2)
						continue;	//ĩ�˵��ڲ����������
					pSmartPart->cfgword.AddBodyLegs(idBodyNo);
					for (CLsRef* pLsRef = pSmartPart.pRod->GetFirstLsRef(); pLsRef; pLsRef = pSmartPart.pRod->GetNextLsRef())
						pLsRef->pBolt->cfgword.AddBodyLegs(idBodyNo);
				}
				else if (pSmartPart->IsPlate())
				{
					CLDSNode* pBaseNode = pSmartPart.pBasicPlate->GetBaseNode();
					if (pBaseNode != NULL && !pBaseNode->cfgword.IsHasBodyNo(idBodyNo))
						continue;
					if (pSmartPart->ucs.origin.z > zInsertPlane + 100)
						continue;
					pSmartPart->cfgword.AddBodyLegs(idBodyNo);
					for (CLsRef* pLsRef = pSmartPart.pBasicPlate->GetFirstLsRef(); pLsRef; pLsRef = pSmartPart.pBasicPlate->GetNextLsRef())
						pLsRef->pBolt->cfgword.AddBodyLegs(idBodyNo);
				}
			}
			//ģ��������������ĺ��߽ڵ�
			CTowerTreeDlg* pTowerStruTree = ((CMainFrame*)AfxGetMainWnd())->GetTowerTreePage();
			pTowerStruTree->AddOrUpdateTreeItemByObj(Ta.Module.GetTail(), true, true);
			return max(0,scope.fMaxZ - scope.fMinZ);
		}
#endif
	}
	double section_z=6000;
#ifdef AFX_TARG_ENU_ENGLISH
	CXhChar50 szVarTitle = "Attach height : ";
#else
	CXhChar50 szVarTitle = (cDefaultSubType == 'B') ? "����߶�:" : "���ȸ߶�";
#endif
	if (Input(szVarTitle, &section_z, 'f') <= 0)
		return false;
	THANDLE _hOldActiveHeight = Ta.m_hActiveModule;
	if (cDefaultSubType == 'B')
	{
		actCfgword.SetBodyLegs(idBodyNo);
		pModule = Ta.Module.append();
		pModule->m_dwLegCfgWord.Clear();
		pModule->m_dwLegCfgWord.SetBodyLegs(idBodyNo, 1);
		pModule->SetBodyNo(idBodyNo);
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(pModule->description, "newly-added module");
#else
		strcpy(pModule->description, "��������");
#endif
		for (int i = 0; i < 4; i++)
			pModule->m_arrActiveQuadLegNo[i] = idBodyNo * CFGLEG::MaxLegs();
		Ta.m_hActiveModule = pModule->handle;
	}
	else if(cDefaultSubType=='L')
	{
		int iLegNo=0,nMaxLegs=CFGLEG::MaxLegs();
		pModule=Ta.GetActiveModule();
		for(iLegNo=1;iLegNo<nMaxLegs;iLegNo++)
		{
			actCfgword=CFGWORD(pModule->GetBodyNo(),iLegNo);
			if(!pModule->m_dwLegCfgWord.And(actCfgword))
				break;
		}
		if(iLegNo==nMaxLegs)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("beyond current maximum limit of connecting legs!");
#else
			AfxMessageBox("�ѳ�����ǰ������������!");
#endif
			return false;
		}
		pModule->m_dwLegCfgWord.AddSpecWord(actCfgword);
		for(int i=0;i<4;i++)
			pModule->m_arrActiveQuadLegNo[i]=(pModule->GetBodyNo()-1)*CFGLEG::MaxLegs()+iLegNo;
	}
	for(CLDSNode* pKeyNode=keynodeset.GetFirst();pKeyNode;pKeyNode=keynodeset.GetNext())
	{
		CLDSLinePart* pFatherRod=(CLDSLinePart*)Ta.Parts.FromHandle(pKeyNode->hFatherPart,CLS_LINEPART);
		if(pFatherRod==NULL||pFatherRod->pStart==NULL||pFatherRod->pEnd==NULL)
			continue;
		f3dPoint line_vec=pFatherRod->pEnd->Position(false)-pFatherRod->pStart->Position(false);
		normalize(line_vec);
		CLDSLinePart* pUpLinkRod=NULL,*pDownLinkRod=NULL;
		if((pFatherRod->pStart==pKeyNode&&line_vec.z>0)||(pFatherRod->pEnd==pKeyNode&&line_vec.z<0))
			pDownLinkRod=pFatherRod;
		else if((pFatherRod->pStart==pKeyNode&&line_vec.z<0)||(pFatherRod->pEnd==pKeyNode&&line_vec.z>0))
			pUpLinkRod=pFatherRod;
		
		if(pFatherRod->pStart==pKeyNode||pFatherRod->pEnd==pKeyNode)
		{//������ڵ㸸�˼����Ӧ����һ����������
			if(line_vec.z<0)
				line_vec*=-1.0;
			CFGWORD cfgword=pFatherRod->UnifiedCfgword();
			for(CLDSLinePart* pCoupleRod=(CLDSLinePart*)Ta.Parts.GetFirstLinePart();pCoupleRod;
			pCoupleRod=(CLDSLinePart*)Ta.Parts.GetNextLinePart())
			{
				if(pCoupleRod==pFatherRod||pCoupleRod->GetClassTypeId()!=pFatherRod->GetClassTypeId())
					continue;
				if(pCoupleRod->pStart==NULL||pCoupleRod->pEnd==NULL||!cfgword.And(pCoupleRod->UnifiedCfgword()))
					continue;
				f3dPoint vec;
				if(pCoupleRod->pStart==pKeyNode)
					vec=pCoupleRod->pEnd->Position(false)-pCoupleRod->pStart->Position(false);
				else if(pCoupleRod->pEnd==pKeyNode)
					vec=pCoupleRod->pStart->Position(false)-pCoupleRod->pEnd->Position(false);
				if(vec.IsZero())
					continue;
				normalize(vec);
				double dd=vec*line_vec;
				if(pUpLinkRod==NULL&&dd<-EPS_COS)
					pUpLinkRod=pCoupleRod;
				else if(pDownLinkRod==NULL&&dd>EPS_COS)
					pDownLinkRod=pCoupleRod;
				if(pUpLinkRod&&pDownLinkRod)
					break;
			}
		}
		bool blJustifyCond=(pKeyNode->m_cPosCalType==0&&(pDownLinkRod!=NULL||pUpLinkRod!=NULL))	
			||(pUpLinkRod&&pDownLinkRod);
		if (!blJustifyCond)
		{
			//pInsertNode->hFatherPart=pFatherRod->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("leg member angle insertion failed!");
#else
			AfxMessageBox("���ĽǸֲ���ʧ��!");
#endif
			if (cDefaultSubType == 'B'&&pModule->iNo == 0)
				Ta.Module.DeleteCursor();	//�Ƴ���������, ���ָ�ԭ�м������
			Ta.m_hActiveModule = _hOldActiveHeight;
			return false;
		}

		f3dPoint joint_pos;	//ɾ���ڼ��Ľڼ䶥����ؼ������Ӧ���νӵ�
		joint_pos.z=pKeyNode->xOriginalPos.z+section_z;
		joint_pos.x=pKeyNode->xOriginalPos.x+section_z*(line_vec.x/line_vec.z);
		joint_pos.y=pKeyNode->xOriginalPos.y+section_z*(line_vec.y/line_vec.z);
		CLDSNode *pInsertNode=console.AppendNode();
		pInsertNode->CopyProperty(pKeyNode);
		pInsertNode->SetPosition(joint_pos);
		if(cDefaultSubType=='S')
			ReplaceDependencyNodeRelaInfo(pKeyNode->xOriginalPos.z,pKeyNode->handle,pInsertNode->handle);
		if(pInsertNode->m_cPosCalType==3)	//�˼���ָ��Z���������
		{
			pInsertNode->arrRelationNode[0]=pKeyNode->arrRelationNode[0];
			pInsertNode->arrRelationNode[1]=pKeyNode->arrRelationNode[1];
		}
		else if(pInsertNode->m_cPosCalType==7&&pInsertNode->m_cPosCalViceType=='Z')
		{
			pInsertNode->arrRelationNode[0]=pKeyNode->arrRelationNode[0];
			pInsertNode->arrRelationNode[1]=pKeyNode->arrRelationNode[1];
			pInsertNode->arrRelationNode[2]=pKeyNode->arrRelationNode[2];
			pInsertNode->attach_offset+=section_z;
		}
		if(pKeyNode->m_cPosCalType==0&&pUpLinkRod&&pDownLinkRod)
			pUpLinkRod=NULL;	//���´�---�ؼ��ڵ�Ϊ���ƽڵ������¸˼�������ʱ���¶����Ӹ˼�Ϊ��׼
		CLDSLinePart *pInsertRod=NULL;
		//if((pKeyNode->m_cPosCalType==0&&(pDownLinkRod!=NULL||pUpLinkRod!=NULL))	
		//	||(pUpLinkRod&&pDownLinkRod))
		{
			pInsertRod=(CLDSLinePart*)console.AppendPart(pFatherRod->GetClassTypeId());
			pInsertRod->CopyProperty(pFatherRod);
			pInsertRod->pStart=pKeyNode;
			pInsertRod->pEnd=pInsertNode;
			if(pInsertRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				((CLDSGroupLineAngle*)pInsertRod)->AppendSonJg();
			if(pDownLinkRod)
				pInsertNode->hFatherPart=pDownLinkRod->handle;
			else 
				pInsertNode->hFatherPart=pUpLinkRod->handle;
			if(pInsertRod->IsAngle())
			{
#ifdef __ANGLE_PART_INC_
				CLDSLineAngle *pInsertAngle=(CLDSLineAngle*)pInsertRod;
				CLDSLineAngle *pUpAngle=(CLDSLineAngle*)pUpLinkRod;
				CLDSLineAngle *pDownAngle=(CLDSLineAngle*)pDownLinkRod;
				pInsertAngle->m_cPosCtrlDatumLineType=((CLDSLineAngle*)pFatherRod)->m_cPosCtrlDatumLineType;
				pInsertAngle->m_cPosCtrlType=((CLDSLineAngle*)pFatherRod)->m_cPosCtrlType;
				//�Ǹ�ʼ�ն�λ����Ʋ���
				if(pUpLinkRod==NULL||pDownLinkRod==NULL)
				{
					CLDSLineAngle *pRefAngle=NULL;
					if(pUpLinkRod)
					{
						pRefAngle=pUpAngle;
						if(pRefAngle->pStart==pKeyNode)
						{
							pInsertAngle->desStartPos=pRefAngle->desEndPos;
							pInsertAngle->desEndPos=pRefAngle->desStartPos;
						}
						else if(pRefAngle->pEnd==pKeyNode)
						{
							pInsertAngle->desStartPos=pRefAngle->desStartPos;
							pInsertAngle->desEndPos=pRefAngle->desEndPos;	
						}
					}
					else 
					{
						pRefAngle=pDownAngle;
						if(pRefAngle->pStart==pKeyNode)
						{
							pInsertAngle->desStartPos=pRefAngle->desStartPos;
							pInsertAngle->desEndPos=pRefAngle->desEndPos;
						}
						else if(pRefAngle->pEnd==pKeyNode)
						{
							pInsertAngle->desStartPos=pRefAngle->desEndPos;
							pInsertAngle->desEndPos=pRefAngle->desStartPos;	
						}
					}
					//�����˼���λ�������ؼ�������
					//���ո˼�ʼ�ն˾�Ϊ���ƽڵ�
					if(pRefAngle->pStart->m_cPosCalType==0&&pRefAngle->pEnd->m_cPosCalType==0)
					{	//���ո˼�ʼ�ն˾�Ϊ���ƽڵ�
						if(pDownLinkRod)
						{	//���ò���˼��ն˶�λ��ʽ
							SetAnglePosProjectToBer(pRefAngle,FALSE);
							//���òθ˼�Z�����С�˶�λ��ʽ
							SetAnglePosFaceOffset(pInsertAngle,pRefAngle->pStart==pKeyNode,pInsertAngle->handle,pRefAngle->handle);
						}
						else if(pUpLinkRod) 
						{	//���ò���˼�ʼ�˶�λ��ʽ
							SetAnglePosFaceOffset(pInsertAngle,TRUE,pRefAngle->handle,pInsertAngle->handle);
							//���òθ˼�Z�����С�˶�λ��ʽ
							SetAnglePosProjectToBer(pRefAngle,pRefAngle->pEnd!=pKeyNode);
						}
					}
					//�ؼ��ڵ�Ϊ�¶ο�ʼ�ڵ�
					else if(pDownLinkRod)
					{	//�������ø˼���ؼ��������˶�λ��ʽ(Z�����С��һ��)
						if(pRefAngle->pStart==pKeyNode)
						{
							CLDSLinePart *pNextRod=FindDownDatumMainRod(pRefAngle->pEnd);
							if(pNextRod&&pNextRod->IsAngle())
							{
								if(pNextRod->pStart==pRefAngle->pEnd)
									pRefAngle->desStartPos=((CLDSLineAngle*)pNextRod)->desStartPos;
								else 
									pRefAngle->desStartPos=((CLDSLineAngle*)pNextRod)->desEndPos;
							}
						}
						else 
						{
							CLDSLinePart *pNextRod=FindDownDatumMainRod(pRefAngle->pStart);
							if(pNextRod&&pNextRod->IsAngle())
							{
								if(pNextRod->pStart==pRefAngle->pStart)
									pRefAngle->desEndPos=((CLDSLineAngle*)pNextRod)->desStartPos;
								else 
									pRefAngle->desEndPos=((CLDSLineAngle*)pNextRod)->desEndPos;
							}			
						}
					}
					//�ؼ��ڵ�Ϊ�¶ν����ڵ�
					else if(pUpLinkRod)	//�������ø˼���ؼ��������˶�λ��ʽ(Z����ϴ��һ��)
						SetAnglePosProjectToBer(pRefAngle,pRefAngle->pEnd!=pKeyNode);
				}
				else 
				{
					if(pUpLinkRod->pStart==pKeyNode)
						pInsertAngle->desEndPos=pUpAngle->desStartPos;
					else if(pUpLinkRod->pEnd==pKeyNode)
						pInsertAngle->desEndPos=pUpAngle->desEndPos;
					if(pDownLinkRod->pStart==pKeyNode)
						pInsertAngle->desStartPos=pDownAngle->desStartPos;
					else if(pDownLinkRod->pEnd==pKeyNode)
						pInsertAngle->desStartPos=pDownAngle->desEndPos;
				}
				//�Ǹַ�����Ʋ���
				if(pUpAngle)
				{
					pInsertAngle->des_norm_x=pUpAngle->des_norm_x;
					pInsertAngle->des_norm_y=pUpAngle->des_norm_y;
					pInsertAngle->set_norm_x_wing(pUpAngle->vxWingNorm,FALSE);
					pInsertAngle->set_norm_y_wing(pUpAngle->vyWingNorm,FALSE);
				}
				else if(pDownAngle)
				{
					pInsertAngle->des_norm_x=pDownAngle->des_norm_x;
					pInsertAngle->des_norm_y=pDownAngle->des_norm_y;	
					pInsertAngle->set_norm_x_wing(pDownAngle->vxWingNorm,FALSE);
					pInsertAngle->set_norm_y_wing(pDownAngle->vyWingNorm,FALSE);
				}
				pInsertAngle->CalPosition();
#endif
			}
			else if(pInsertRod->GetClassTypeId()==CLS_LINETUBE)
			{
#ifdef __COMPLEX_PART_INC_
				CLDSLineTube *pInsertTube=(CLDSLineTube*)pInsertRod;
				CLDSLineTube *pUpTube=(CLDSLineTube*)pUpLinkRod;
				CLDSLineTube *pDownTube=(CLDSLineTube*)pDownLinkRod;
				//�ֹ�ʼ�ն�λ����Ʋ���
				if(pUpLinkRod==NULL||pDownLinkRod==NULL)
				{
					CLDSLineTube *pRefTube=NULL;
					if(pUpLinkRod)
					{
						pRefTube=pUpTube;
						if(pRefTube->pStart==pKeyNode)
						{
							pInsertTube->desStartPos=pRefTube->desEndPos;
							pInsertTube->desEndPos=pRefTube->desStartPos;
						}
						else if(pRefTube->pEnd==pKeyNode)
						{
							pInsertTube->desStartPos=pRefTube->desStartPos;
							pInsertTube->desEndPos=pRefTube->desEndPos;
						}
					}
					else 
					{
						pRefTube=pDownTube;
						if(pRefTube->pStart==pKeyNode)
						{
							pInsertTube->desStartPos=pRefTube->desStartPos;
							pInsertTube->desEndPos=pRefTube->desEndPos;
						}
						else if(pRefTube->pEnd==pKeyNode)
						{
							pInsertTube->desStartPos=pRefTube->desEndPos;
							pInsertTube->desEndPos=pRefTube->desStartPos;	
						}
					}
					//�����˼���λ�������ؼ�������
					//���ո˼�ʼ�ն˾�Ϊ���ƽڵ�
					if(pRefTube->pStart->m_cPosCalType==0&&pRefTube->pEnd->m_cPosCalType==0)
					{	
						if(pDownLinkRod)
						{	//���ò���˼��ն˶�λ��ʽΪ����ͶӰ
							SetTubePosProjectPoint(pRefTube,FALSE,pInsertRod->handle,pRefTube->handle);
							//���òθ˼�Z�����С�˶�λ��ʽΪ����ͶӰ
							SetTubePosProjectPoint(pInsertTube,pRefTube->pStart==pKeyNode,pInsertTube->handle,pRefTube->handle);
						}
						else if(pUpLinkRod) 
						{	//���ò���˼�ʼ�˶�λ��ʽΪ����ͶӰ
							SetTubePosProjectPoint(pInsertTube,TRUE,pRefTube->handle,pInsertTube->handle);
							//���ò��ո˼�Z�����С��Ϊ����ͶӰ
							SetTubePosProjectPoint(pInsertTube,pRefTube->pEnd!=pKeyNode,pInsertTube->handle,pRefTube->handle);
						}
					}
					//�ؼ��ڵ�Ϊ�¶ο�ʼ�ڵ������㣬�ҵ�ǰ�¶������������ڼ�
					else 
					{	
						if(pKeyNode==pRefTube->pStart)		//�����ն�����ʼ��λ�ò���
							SetTubePosProjectPoint(pRefTube,TRUE,pRefTube->desEndPos.hDatumStartPart,pRefTube->desEndPos.hDatumEndPart);
						else if(pKeyNode==pRefTube->pEnd)	//����ʼ������ʼ��λ�ò���
							SetTubePosProjectPoint(pRefTube,FALSE,pRefTube->desStartPos.hDatumStartPart,pRefTube->desStartPos.hDatumEndPart);
					}
				}
				else 
				{	//�ֹ�ʼ�ն�λ����Ʋ���
					if(pUpLinkRod->pStart==pKeyNode)
						pInsertTube->desEndPos=pUpTube->desStartPos;
					else if(pUpLinkRod->pEnd==pKeyNode)
						pInsertTube->desEndPos=pUpTube->desEndPos;
					if(pDownLinkRod->pStart==pKeyNode)
						pInsertTube->desStartPos=pDownTube->desStartPos;
					else if(pDownLinkRod->pEnd==pKeyNode)
						pInsertTube->desStartPos=pDownTube->desEndPos;
				}
				//�ֹܹ����淨�߼������߲���
				if(pUpTube)
				{
					pInsertTube->nearWeldRoadVec=pUpTube->nearWeldRoadVec;
					pInsertTube->nearWorkPlaneNorm=pUpTube->nearWorkPlaneNorm;
					pInsertTube->hWorkNormRefPart=pUpTube->hWorkNormRefPart;
					pInsertTube->m_bHasWeldRoad=pUpTube->m_bHasWeldRoad;
				}
				else if(pDownTube)
				{
					pInsertTube->nearWeldRoadVec=pDownTube->nearWeldRoadVec;
					pInsertTube->nearWorkPlaneNorm=pDownTube->nearWorkPlaneNorm;
					pInsertTube->hWorkNormRefPart=pDownTube->hWorkNormRefPart;
					pInsertTube->m_bHasWeldRoad=pDownTube->m_bHasWeldRoad;
				}
				pInsertTube->CalPosition();
#endif
			}
			NewLinePart(pInsertRod);
			
			if(cDefaultSubType!='B'&&cDefaultSubType!='L')
			{	//������������ڼ�ʱ����Ҫ���¹ؼ��ڵ㸸�˼� wjh-2015.4.9
				pInsertRod->CalPosition();//���ʱ�����㣬���ڿ��ܻ���ΪĬ��ֵ����������ƽ�Ƽ����쳣 wjh-2015.4.9
				pKeyNode->hFatherPart=pInsertRod->handle;
			}
			//���ò���ڵ㶨λ��ʽ
			pInsertNode->m_cPosCalType=3;	//�˼���ָ��Z�����
			if(pUpLinkRod==NULL||pDownLinkRod==NULL)
			{
				CLDSLinePart *pRefRod=pUpLinkRod;
				if(pRefRod==NULL)
					pRefRod=pDownLinkRod;
				if(pRefRod->pStart->m_cPosCalType==0&&pRefRod->pEnd->m_cPosCalType==0)
				{	//���ո˼�ʼ�ն˾�Ϊ���ƽڵ�
					pInsertNode->arrRelationNode[0]=pRefRod->pStart->handle;
					pInsertNode->arrRelationNode[1]=pRefRod->pEnd->handle;
				}
				else if(pDownLinkRod)
				{	//���¶ζ�������ڼ�
					CLDSNode *pRefNode=NULL;
					if(pDownLinkRod->pStart!=pKeyNode)
						pRefNode=pDownLinkRod->pStart;
					else if(pDownLinkRod->pEnd!=pKeyNode)
						pRefNode=pDownLinkRod->pEnd;
					if(pRefNode&&pRefNode->m_cPosCalType==3)
					{	//ȡ��ؼ��ڵ��Ӧ�Ľڵ�������ڵ�
						pInsertNode->arrRelationNode[0]=pRefNode->arrRelationNode[0];
						pInsertNode->arrRelationNode[1]=pRefNode->arrRelationNode[1];
					}
				}
				else //if(pUpLinkRod)
				{	//���¶εײ�����ڵ㣬���ò���ڵ�Ϊ���ƽڵ㣻�ؼ��ڵ�Ϊָ��Z����ڵ�
					CLDSNode *pRefNode=NULL;
					if(pUpLinkRod->pStart!=pKeyNode)
						pRefNode=pUpLinkRod->pStart;
					else if(pUpLinkRod->pEnd!=pKeyNode)
						pRefNode=pUpLinkRod->pEnd;
					if(pRefNode&&pRefNode->m_cPosCalType==3)
					{	//���ùؼ��ڵ�Ϊָ��Z����ڵ�
						pKeyNode->m_cPosCalType=3;
						pKeyNode->arrRelationNode[0]=pRefNode->arrRelationNode[0];
						pKeyNode->arrRelationNode[1]=pRefNode->arrRelationNode[1];
					}
					//���ò���ڵ�Ϊ���ƽڵ�
					pInsertNode->m_cPosCalType=0;
					//���¶�ĩβ����½ڵ����Ҫ���½ڵ�����
					for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
						pNode->ReplaceReferenceHandle(CHandleReplacer(pKeyNode->handle,pInsertNode->handle),CLS_NODE);
				}
			}
			else 
			{	//ֱ��ʹ�ùؼ��ڵ�������ڵ�
				pInsertNode->arrRelationNode[0]=pKeyNode->arrRelationNode[0];
				pInsertNode->arrRelationNode[1]=pKeyNode->arrRelationNode[1];
			}
		}
		/*else //ǰ�� wjh-2019.8.1
		{
			pInsertNode->hFatherPart=pFatherRod->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("leg member angle insertion failed!");
#else
			AfxMessageBox("���ĽǸֲ���ʧ��!");
#endif
			return false;
		}*/
		pInsertNode->Set();
		if(cDefaultSubType=='S')
			pInsertNode->layer(0)=pInsertRod->layer(0)='S';
		else if(cDefaultSubType=='B')
		{
			pInsertNode->layer(0)=pInsertRod->layer(0)='S';
			pInsertNode->cfgword=pInsertRod->cfgword=actCfgword;
		}
		else if(cDefaultSubType=='L')
		{
			pInsertNode->layer(0)=pInsertRod->layer(0)='L';
			pInsertNode->cfgword=pInsertRod->cfgword=actCfgword;
		}
		NewNode(pInsertNode);
		if(cDefaultSubType=='S')
		{
			for(CLDSLinePart* pRod=Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
			{
				if(pRod!=pInsertRod)
				{	//�²���˼�����Ҫ����
					double min_z=pKeyNode->Position(false).z;
					if(pRod->pStart&&pRod->pEnd&&(pRod->pStart->Position(false).z>min_z||pRod->pEnd->Position(false).z>min_z))
						pRod->ReplaceReferenceHandle(CHandleReplacer(pKeyNode->handle,pInsertNode->handle));
				}
				//�ؼ��ڵ�Ϊ���Ƶ�ʱ�������º���Ҫ�������þ��
				if(pKeyNode->m_cPosCalType==0&&(pDownLinkRod&&pUpLinkRod==NULL)
					||(pDownLinkRod==NULL&&pUpLinkRod))
				{
					long hOld=0;
					if(pDownLinkRod)
						hOld=pDownLinkRod->handle;
					else if(pUpLinkRod)
						hOld=pUpLinkRod->handle;
					else 
						continue;
					if(pRod->IsAngle())
					{
#ifdef __ANGLE_PART_INC_
						((CLDSLineAngle*)pRod)->desStartPos.ReplaceReferenceHandle(CHandleReplacer(hOld,pInsertRod->handle),NULL);
						((CLDSLineAngle*)pRod)->desEndPos.ReplaceReferenceHandle(CHandleReplacer(hOld,pInsertRod->handle),NULL);
#endif
					}
					else if(pRod->IsTube())
					{
#ifdef __COMPLEX_PART_INC_
						((CLDSLineTube*)pRod)->desStartPos.ReplaceReferenceHandle(CHandleReplacer(hOld,pInsertRod->handle),NULL);
						((CLDSLineTube*)pRod)->desEndPos.ReplaceReferenceHandle(CHandleReplacer(hOld,pInsertRod->handle),NULL);
#endif
					}
				}
			}
		}
	}
	return true;
}
void PreReplaceSelfDependNode(CLDSNode* pNode,CLDSNode* pDependentNode)
{
	if(pNode->m_cPosCalType==3||pNode->m_cPosCalType==5)	//3.�˼���Z����ֵ�����;5.�˼��ϵı����ȷֵ�
	{
		CLDSNode* pNewDatumNode=NULL;
		if(pNode->arrRelationNode[0]==pDependentNode->handle||pNode->arrRelationNode[1]==pDependentNode->handle)
		{
			if(pDependentNode->m_cPosCalType==3||pDependentNode->m_cPosCalType==5||pDependentNode->m_cPosCalType==7)
			{
				int start=0;
				if(pDependentNode->m_cPosCalType==7)
					start=1;
				CLDSNode* pRelaNode1=Ta.Node.FromHandle(pDependentNode->arrRelationNode[start]);
				CLDSNode* pRelaNode2=Ta.Node.FromHandle(pDependentNode->arrRelationNode[start+1]);
				if(pRelaNode1->Position(false).z>pRelaNode1->Position(false).z)
					pNewDatumNode=pRelaNode1;
				else if(pRelaNode2->Position(false).z>pRelaNode2->Position(false).z)
					pNewDatumNode=pRelaNode2;
				else if(pRelaNode1->Position(false).z<pNode->Position(false).z)
					pNewDatumNode=pRelaNode1;
				else if(pRelaNode2->Position(false).z<pNode->Position(false).z)
					pNewDatumNode=pRelaNode2;
				if(pNewDatumNode&&pNode->arrRelationNode[0]==pDependentNode->handle)
					pNode->arrRelationNode[0]=pNewDatumNode->handle;
				else if(pNewDatumNode&&pNode->arrRelationNode[1]==pDependentNode->handle)
					pNode->arrRelationNode[1]=pNewDatumNode->handle;
			}
		}
	}
	else if(pNode->m_cPosCalType==7&&pNode->m_cPosCalViceType=='Z')	//�ظ˼���ĳ�ڵ��ƫ�Ƶ�
	{
		CLDSNode* pNewDatumNode=NULL;
		if(pNode->arrRelationNode[1]==pDependentNode->handle||pNode->arrRelationNode[2]==pDependentNode->handle)
		{
			if(pDependentNode->m_cPosCalType==3||pDependentNode->m_cPosCalType==5||pDependentNode->m_cPosCalType==7)
			{
				int start=0;
				if(pDependentNode->m_cPosCalType==7)
					start=1;
				CLDSNode* pRelaNode1=Ta.Node.FromHandle(pDependentNode->arrRelationNode[start]);
				CLDSNode* pRelaNode2=Ta.Node.FromHandle(pDependentNode->arrRelationNode[start+1]);
				if(pRelaNode1->Position(false).z>pNode->Position(false).z)
					pNewDatumNode=pRelaNode1;
				else if(pRelaNode2->Position(false).z>pNode->Position(false).z)
					pNewDatumNode=pRelaNode2;
				else if(pRelaNode1->Position(false).z<pNode->Position(false).z)
					pNewDatumNode=pRelaNode1;
				else if(pRelaNode2->Position(false).z<pNode->Position(false).z)
					pNewDatumNode=pRelaNode2;
				if(pNewDatumNode&&pNode->arrRelationNode[1]==pDependentNode->handle)
					pNode->arrRelationNode[1]=pNewDatumNode->handle;
				else if(pNewDatumNode&&pNode->arrRelationNode[2]==pDependentNode->handle)
					pNode->arrRelationNode[2]=pNewDatumNode->handle;
			}
		}
	}
}
//ɾ������ڼ�
static void DeleteShankSection(NODESET &keynodeset,double min_z,double max_z)
{
	TAPER_SCOPE taper;
	CHashList<CLDSLinePart*>hashDatumRods;
	//�Ƿ���Ҫ��ɾ���˼����������ڸ˼���λ����(ɾ���ڵ���ߵ����͵�ڵ�Ϊ���Ƶ�)
	CHashList<BOOL>hashUpdateRodDesPara;
	//��¼��׼�ڵ���ؼ��ڵ��Ӧ�� keyΪ�ؼ��ڵ�handle,valueΪ��׼�ڵ�handle
	CHashList<long>datumNodeHashKeyNode;
	for(CLDSNode* pKeyNode=keynodeset.GetFirst();pKeyNode;pKeyNode=keynodeset.GetNext())
	{
		taper.VerifyVertex(pKeyNode->Position(false));
		CLDSLinePart* pFatherRod=(CLDSLinePart*)Ta.Parts.FromHandle(pKeyNode->hFatherPart,CLS_LINEPART);
		if(pFatherRod==NULL||pFatherRod->pStart==NULL||pFatherRod->pEnd==NULL)
			continue;
		f3dPoint line_vec=pFatherRod->pEnd->Position(false)-pFatherRod->pStart->Position(false);
		if(pKeyNode->m_cPosCalType==0)
		{	//�ؼ��ڵ�Ϊ���Ƶ�ʱ��Ҫ���ҹؼ��ڵ��Ϸ��˼���������˼����췽��(���´�)
			CLDSLinePart *pUpDatumRod=FindUpDatumMainRod(pKeyNode);
			if(pUpDatumRod)
				line_vec=pUpDatumRod->pEnd->Position(false)-pUpDatumRod->pStart->Position(false);
		}
		normalize(line_vec);
		f3dPoint joint_pos;	//ɾ���ڼ��Ľڼ䶥����ؼ������Ӧ���νӵ�
		joint_pos.z=min_z;
		joint_pos.x=pFatherRod->pStart->Position(false).x+(min_z-pFatherRod->pStart->Position(false).z)*(line_vec.x/line_vec.z);
		joint_pos.y=pFatherRod->pStart->Position(false).y+(min_z-pFatherRod->pStart->Position(false).z)*(line_vec.y/line_vec.z);
		CLDSNode* pDatumNode=NULL;
		for(pDatumNode=Ta.Node.GetFirst();pDatumNode;pDatumNode=Ta.Node.GetNext())
		{
			if(isdigit(pDatumNode->layer(2))&&pDatumNode->Position(false)==joint_pos&&pDatumNode->cfgword.And(pKeyNode->cfgword))
			{
				taper.VerifyVertex(pDatumNode->Position(false));
				CLDSLinePart* pDatumRod=pFatherRod;
				//��׼�ڵ�Ϊ�ؼ��ڵ�ʱ����ǰ�ڼ�Ϊ�¶���߽ڼ䣻��ɾ����˼������Ļ�׼�˼�Ϊ�ؼ��㸸�˼�
				//��׼�ڵ�Ϊ�ǹؼ��ڵ�ʱ��������ɾ���˼��������϶˸˼�
				if(pDatumNode->m_cPosCalType!=0)
					pDatumRod=FindUpDatumMainRod(pDatumNode);
				if(pDatumRod)
				{
					hashDatumRods.SetValue(pDatumNode->layer(2)-'0',pDatumRod);
					//��¼��Ӧ���������Ƿ���Ҫ����ʼ�˻��ն���Ʋ���
					if(pKeyNode->m_cPosCalType==0)		//�ؼ���Ϊ���Ƶ�
					{
						hashUpdateRodDesPara.SetValue(pDatumNode->layer(2)-'0',TRUE);
						//�ؼ��ڵ�Ϊ���Ƶ�ʱ����Ҫ����׼�ڵ�����Ϊ���Ƶ�(�ؼ��ڵ㽫��ɾ��)
						pDatumNode->m_cPosCalType=0;	
					}
					else if(pDatumRod==pFatherRod)	//��ؼ��ڵ��Ӧ����һ�˽ڵ�Ϊ���Ƶ�
						hashUpdateRodDesPara.SetValue(pDatumNode->layer(2)-'0',TRUE);
					else								
						hashUpdateRodDesPara.SetValue(pDatumNode->layer(2)-'0',FALSE);
				}
				//��¼�ؼ��ڵ����Ӧ�ڵ�Ķ��չ�ϵ,�����Ժ�����滻ʱʹ��
				datumNodeHashKeyNode.SetValue(pKeyNode->handle,pDatumNode->handle);
				if( pDatumNode->arrRelationNode[0]==pKeyNode->handle||pDatumNode->arrRelationNode[1]==pKeyNode->handle||
					pDatumNode->arrRelationNode[2]==pKeyNode->handle||pDatumNode->arrRelationNode[3]==pKeyNode->handle)
					PreReplaceSelfDependNode(pDatumNode,pKeyNode);
				//��׼�ڵ㸸�˼�����Ϊ�ؼ��ڵ㸸�˼�
				pDatumNode->hFatherPart=pFatherRod->handle;
				break;
			}
		}
		console.DispNodeSet.DeleteNode(pKeyNode->handle);
		console.DeleteNode(pKeyNode->handle);
	}
	for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->Position(false).z<=min_z||pNode->Position(false).z>max_z)
			continue;
		if(taper.IsIncludePoint(pNode->Position(false)))
		{
			Ta.RemoveNodeRelativeObjs(pNode);
			console.DispNodeSet.DeleteNode(pNode->handle);
			console.DeleteNode(pNode->handle);
		}
	}
	for(CLDSLinePart *pRod=Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if( pRod->pStart->Position(false).z>max_z||pRod->pStart->Position(false).z<min_z||
			pRod->pEnd->Position(false).z>max_z||pRod->pEnd->Position(false).z<min_z)
			continue;	//��ȫ������ɾ���ڼ�Z���䷶Χ��
		if(taper.IsIncludePoint(pRod->pStart->Position(false))&&taper.IsIncludePoint(pRod->pEnd->Position(false)))
		{
			int push=Ta.Node.push_stack();
			CLDSLinePart **ppDatumRod=NULL;
			if((pRod->layer(0)=='S'||pRod->layer(0)=='L')&&
				pRod->layer(1)=='Z'&&isdigit(pRod->layer(2)))
				ppDatumRod=hashDatumRods.GetValue(pRod->layer(2)-'0');
			if(ppDatumRod&&(datumNodeHashKeyNode.GetValue(pRod->pStart->handle)
				||datumNodeHashKeyNode.GetValue(pRod->pEnd->handle)))
			{	//pRod��ؼ��ڵ����ʱ����Ҫ�������²���
				Ta.ReplaceReferenceHandle(CHandleReplacer(pRod->handle,(*ppDatumRod)->handle),(*ppDatumRod)->GetClassTypeId());
				BOOL *pUpdateDesPara=hashUpdateRodDesPara.GetValue(pRod->layer(2)-'0');
				if(pUpdateDesPara&&(*pUpdateDesPara)==TRUE&&*ppDatumRod!=pRod)
				{	//Ҫɾ���Ľڼ���ߵ����͵�Ϊ���Ƶ�ʱ��Ҫ���»�׼�˼�λ����Ʋ���
					if((*ppDatumRod)->IsAngle())
					{
#ifdef __ANGLE_PART_INC_
						CLDSLineAngle *pDatumAngle=(CLDSLineAngle*)(*ppDatumRod);
						CLDSLineAngle *pAngle=(CLDSLineAngle*)pRod;
						if(pDatumAngle->pStart==pAngle->pEnd)
							pDatumAngle->desStartPos=pAngle->desStartPos;
						else if(pDatumAngle->pStart==pAngle->pStart)
							pDatumAngle->desStartPos=pAngle->desEndPos;
						else if(pDatumAngle->pEnd==pAngle->pEnd)
							pDatumAngle->desEndPos=pAngle->desStartPos;
						else if(pDatumAngle->pEnd==pAngle->pStart)
							pDatumAngle->desEndPos=pAngle->desEndPos;
#endif
					}
					else if((*ppDatumRod)->IsTube())
					{
#ifdef __COMPLEX_PART_INC_
						CLDSLineTube *pDatumTube=(CLDSLineTube*)(*ppDatumRod);
						CLDSLineTube *pTube=(CLDSLineTube*)pRod;
						if(pDatumTube->pStart==pTube->pEnd)
							pDatumTube->desStartPos=pTube->desStartPos;
						else if(pDatumTube->pStart==pTube->pStart)
							pDatumTube->desStartPos=pTube->desEndPos;
						else if(pDatumTube->pEnd==pTube->pEnd)
							pDatumTube->desEndPos=pTube->desStartPos;
						else if(pDatumTube->pEnd==pTube->pStart)
							pDatumTube->desEndPos=pTube->desEndPos;
#endif
					}
				}
			}
			console.DispPartSet.DeleteNode(pRod->handle);
			console.DeletePart(pRod->handle);
			Ta.Parts.pop_stack(push);
		}
	}
	//�����ø˼�λ�ò���ʱ����Ҫʹ��ʼ�ն˾�����бȽϣ��ʽڵ��滻�����������
	for(long *pH=datumNodeHashKeyNode.GetFirst();pH;pH=datumNodeHashKeyNode.GetNext())
	{	//
		long key=datumNodeHashKeyNode.GetCursorKey();
		Ta.ReplaceReferenceHandle(CHandleReplacer(key,*pH),CLS_NODE);
	}
}
int CLDSView::ModifyShankSection()
{
	CFuncLifeObject func_life(OperSectionDump,this);
	CString cmdStr;
	int retCode=0;
	static char cDefaultType, cDefaultSubType;
	CLogErrorLife life;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)theApp.m_pMainWnd)->GetCmdLinePage();
	CUndoOperObject undo(&Ta,true);
	hashNodeOffsetCoordZ.Empty();
	if(cDefaultType==0)
		cDefaultType='I';
	if(cDefaultSubType==0)
		cDefaultSubType= 'S';	//'B' or 'L'
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("SECTION operation type[newly insert(I)/vertical height updating(M)/delete(D)]<%C>:",cDefaultType);
#else
	cmdStr.Format("SECTION ��������[�²���(I)/���߱��(M)/ɾ��(D)]<%C>:",cDefaultType);
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	while(1)
	{
		if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"I|M|D"))
		{
			pCmdLine->m_bGettingStrFromCmdLine=FALSE;
			return 0;
		}
		pCmdLine->m_bGettingStrFromCmdLine=FALSE;
		if(cmdStr.CompareNoCase("I")==0)
			cDefaultType='I';
		else if(cmdStr.CompareNoCase("M")==0)
			cDefaultType='M';
		else if(cmdStr.CompareNoCase("D")==0)
			cDefaultType='D';
		else if(cmdStr.GetLength()>0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("option key words needed","");
			pCmdLine->FinishCmdLine();
			cmdStr.Format("SECTION operation type[newly insert(I)/vertical height updating(M)/delete(D)]<%C>:",cDefaultType);
#else
			pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
			pCmdLine->FinishCmdLine();
			cmdStr.Format("SECTION ��������[�²���(I)/���߱��(M)/ɾ��(D)]<%C>:",cDefaultType);
#endif
			pCmdLine->FillCmdLine(cmdStr,"");
			continue;
		}
		break;
	}
	long *id_arr=NULL;
	int i,n = g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n==0)
	{
		if(cDefaultType=='I')	//�����½ڼ�
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("please select leg member's key nodes of the insertion location, then press enter to confirm:","");
#else
			pCmdLine->FillCmdLine("��ѡ�������ϲ���λ�õ��������Ĺؼ��ڵ㣬Ȼ��س���ȷ��:","");
#endif
			g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_NODE));
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("please select tower nodes or V titled material which penetrate current node's vertical range, then press enter to confirm:","");
#else
			pCmdLine->FillCmdLine("��ѡ��ᴩ��ǰ�����ڼ䴹�߷�Χ������ڵ��б�ģ�Ȼ��س���ȷ��:","");
#endif
			g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_NODE)|SELECT_LINEPART);
		}
		OpenWndSel();	//����ѡ��
	}
	if(n>0||pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		CLDSNode *pNode=NULL;
		n = g_pSolidSnap->GetLastSelectEnts(id_arr);
		NODESET keynodeset;
		CHashList<NODE_PTR> hashNodesById;
		//�����������Ĺؼ��ڵ�
		for(i=0;i<n;i++)
		{
			pNode=Ta.Node.FromHandle(id_arr[i]);
			if(IsMainShankNode(pNode))
				hashNodesById.SetValue(pNode->handle,pNode);
			CSuperSmartPtr<CLDSPart>pPart;
			if(pNode==NULL)
				pPart=Ta.Parts.FromHandle(id_arr[i]);
			if(pPart.IsHasPtr()&&pPart->IsLinePart())
			{
				if(IsMainShankNode(pPart.LinePartPointer()->pStart))
					hashNodesById.SetValue(pPart.LinePartPointer()->pStart->handle,pPart.LinePartPointer()->pStart);
				if(IsMainShankNode(pPart.LinePartPointer()->pEnd))
					hashNodesById.SetValue(pPart.LinePartPointer()->pEnd->handle,pPart.LinePartPointer()->pEnd);
			}
		}
		if(hashNodesById.GetNodeNum()==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->PromptStrInCmdLine("No any key nodes selected, please check whether the layer name of the chose nodes is true or false!");
#else
			pCmdLine->PromptStrInCmdLine("δѡ���κ�����ؼ��ڵ㣬��鿴ѡ�нڵ��ͼ�����Ƿ�����");
#endif
			return 0;
		}
		//����ڼ�Z�᷽�����䷶Χ
		double min_z,max_z;
		bool bInitCoordZ=false;
		NODE_PTR* ppNode;
		for(ppNode=hashNodesById.GetFirst();ppNode;ppNode=hashNodesById.GetNext())
		{
			if(!bInitCoordZ)
			{
				min_z=max_z=(*ppNode)->Position(false).z;
				bInitCoordZ=true;
			}
			else if((*ppNode)->Position(false).z>max_z)
				max_z=(*ppNode)->Position(false).z;
			else if((*ppNode)->Position(false).z<min_z)
				min_z=(*ppNode)->Position(false).z;
		}
		//if(cDefaultType=='I'&&max_z==min_z)
		//��ȡ����ڼ䷶Χ����ؼ��ǽڵ㣨λ���������ļ�������ײ�ƽ���ϵĽڵ㣩
		for(ppNode=hashNodesById.GetFirst();ppNode;ppNode=hashNodesById.GetNext())
		{
			if((*ppNode)->Position(false).z==max_z)
				keynodeset.append(*ppNode);
		}
		//����ԳƵ�δѡ�еĹؼ��ǽڵ�
		NODESET addnodeset;
		for(pNode=keynodeset.GetFirst();pNode;pNode=keynodeset.GetNext())
		{
			CLDSNode *pMirXNode=pNode->GetMirNode(MIRMSG(1));
			CLDSNode *pMirYNode=pNode->GetMirNode(MIRMSG(2));
			CLDSNode *pMirZNode=pNode->GetMirNode(MIRMSG(3));
			if(pMirXNode&&hashNodesById.GetValue(pMirXNode->handle)==NULL)
			{
				hashNodesById.SetValue(pMirXNode->handle,pMirXNode);
				addnodeset.append(pMirXNode);
			}
			if(pMirYNode&&hashNodesById.GetValue(pMirYNode->handle)==NULL)
			{
				hashNodesById.SetValue(pMirYNode->handle,pMirYNode);
				addnodeset.append(pMirYNode);
			}
			if(pMirZNode&&hashNodesById.GetValue(pMirZNode->handle)==NULL)
			{
				hashNodesById.SetValue(pMirZNode->handle,pMirZNode);
				addnodeset.append(pMirZNode);
			}
		}
		for(pNode=addnodeset.GetFirst();pNode;pNode=addnodeset.GetNext())
			keynodeset.append(pNode);
		//���нڼ����
		double section_z=3000;
		if(cDefaultType=='I')	//�����½ڼ�
		{
			if(max_z>min_z)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->PromptStrInCmdLine("when inserting new operation, the nodes selected must be in the same horizontal plane!");
#else
				pCmdLine->PromptStrInCmdLine("�����½ڼ����ʱ��ѡ�еĽڵ����λ��ͬһˮƽ���ڣ�");
#endif
				return 0;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("SECTION section insertion operation type[tower body(S)/module entity(B)/connecting legs(L)]<%C>:", cInsertSubType);
#else
			cmdStr.Format("SECTION ��������ڼ�����[����(S)/����(B)/����(L)]<%C>:", cDefaultSubType);
#endif
			pCmdLine->FillCmdLine(cmdStr, "");
			while (1)
			{
				if (!pCmdLine->GetStrFromCmdLine(cmdStr, CCmdLineDlg::KEYWORD_AS_RETURN, "S|B|L"))
				{
					pCmdLine->m_bGettingStrFromCmdLine = FALSE;
					return 0;
				}
				pCmdLine->m_bGettingStrFromCmdLine = FALSE;
				if (cmdStr.CompareNoCase("S") == 0)
					cDefaultSubType = 'S';
				else if (cmdStr.CompareNoCase("B") == 0)
					cDefaultSubType = 'B';
				else if (cmdStr.CompareNoCase("L") == 0)
					cDefaultSubType = 'L';
				else if (cmdStr.GetLength() > 0)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					pCmdLine->FillCmdLine("option key words needed", "");
					pCmdLine->FinishCmdLine();
					cmdStr.Format("SECTION section insertion operation type[tower body(S)/module entity(B)/connecting legs(L)]<%C>:", cDefaultSubType);
#else
					pCmdLine->FillCmdLine("��Ҫѡ��ؼ���", "");
					pCmdLine->FinishCmdLine();
					cmdStr.Format("SECTION ��������ڼ�����[����(S)/����(B)/����(L)]<%C>:", cDefaultSubType);
#endif
					pCmdLine->FillCmdLine(cmdStr, "");
					continue;
				}
				break;
			}
		}
		else if(cDefaultType=='M')
		{
			if(max_z==min_z)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->PromptStrInCmdLine("please select tower nodes or V titled material which penetrate current node's vertical range!");
#else
				pCmdLine->PromptStrInCmdLine("ѡ��ᴩ��ǰ�����ڼ䴹�߷�Χ������ڵ��б�ģ�");
#endif
				return 0;
			}
			section_z=max_z-min_z;
		}
		else //if(cDefaultType=='D')
		{
			if(max_z==min_z)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->PromptStrInCmdLine("please select tower nodes or V titled material which penetrate current node's vertical range��");
#else
				pCmdLine->PromptStrInCmdLine("ѡ��ᴩ��ǰ�����ڼ䴹�߷�Χ������ڵ��б�ģ�");
#endif
				return 0;
			}
			section_z=-max_z+min_z;
		}
		if(cDefaultType=='M')
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(Input("section height:",&section_z,'f')>0)
#else
			if(Input("�ڼ�߶�:",&section_z,'f')>0)
#endif
				section_z-=(max_z-min_z);
			else 
				return 0;
		}
		CFGWORD shareword;
		shareword.Clear();	//�����ʱӦֻ�������漰����
		bool hasLegNode=false;
		for(pNode=keynodeset.GetFirst();pNode;pNode=keynodeset.GetNext())
		{
			if(pNode->layer(0)=='L')
				hasLegNode=true;
			if(shareword.IsNull())
				shareword=pNode->UnifiedCfgword();
			else
				shareword&=pNode->UnifiedCfgword();
		}
		if(!hasLegNode)
		{
			for(i=1;i<=24;i++)
			{
				if(shareword.IsHasBodyNo(i))
					shareword.SetBodyLegs(i);
			}
		}
		logerr.ClearContents();
		if (cDefaultType == 'I')		//��������ڼ�
		{
			if ((section_z=InsertShankSection(keynodeset, cDefaultSubType))<=0)
				return 0;
		}
		else if (cDefaultType == 'D')	//ɾ������ڼ�
			DeleteShankSection(keynodeset, min_z, max_z);
		//
		if(cDefaultType!='I'||cDefaultSubType=='S')
		{
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{	//���ȣ�ƫ���������κ������ڵ㼰ָ��Z��������ڵ�
				if(pNode->m_cPosCalType==7)
					continue;
				if(pNode->Position(false).z>min_z&&shareword.And(pNode->cfgword))
					InitNodeOffsetCoordZ(pNode,min_z,section_z);
			}
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{	//��Σ�������ʼ��׼�ڵ�ƫ����Ҫƫ�ƵĴ�ֱƫ�ƽڵ�
				if(pNode->m_cPosCalType!=7)
					continue;
				if(pNode->Position(false).z>min_z&&shareword.And(pNode->cfgword))
					InitNodeOffsetCoordZ(pNode,min_z,section_z);
			}
		}
		//
		if(cDefaultType!='I'||cDefaultSubType=='S')
		{	//���\ɾ���ڼ�߶ȡ���������ڼ�����������Ӧ����ǰ����ƽ������ȫ��ͬ�¶���λ wjh-2019.5.22
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{	//���ȣ�ƫ���������κ������ڵ㼰ָ��Z��������ڵ�
				if(pNode->m_cPosCalType==7)
					continue;
				if(pNode->Position(false).z>min_z&&shareword.And(pNode->cfgword))
					OffsetNodeCoordZ(pNode);
			}
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{	//��Σ�������ʼ��׼�ڵ�ƫ����Ҫƫ�ƵĴ�ֱƫ�ƽڵ�
				if(pNode->m_cPosCalType!=7)
					continue;
				if(pNode->Position(false).z>min_z&&shareword.And(pNode->cfgword))
					OffsetNodeCoordZ(pNode);
			}
		}
		g_pSolidDraw->BuildDisplayList(this);
		AutoDaoWei();
	}
	else
		pCmdLine->CancelCmdLine();
	if(logerr.IsHasContents())
		logerr.ShowToScreen();
	return 0;
}
void CLDSView::OnOperShankSection()
{
	ModifyShankSection();
}
//�������Ȳ���
UINT InitHeightLegs(const char* szDefinitions, CXhSimpleList<HEIGHT_LEGS>* pListHeights)
{
	int nMaxLeg = 0;
	HEIGHT_LEGS* pHeightLeg = NULL;
	char* sKey = NULL;
	CXhChar100 arrLineTextPool[128];
	ARRAY_LIST<CXhChar100> xarrLineTexts(arrLineTextPool,128,24);
	//UINT nLines = DELIMITER_STR::ParseWordsFromStr(szDefinitions, "\n", xarrLineTexts.m_pData, xarrLineTexts.GetSize());
	CXhChar100 itemstr;
	const char* pszIter,*pszWordStart=szDefinitions;
	for (pszIter = szDefinitions; true; pszIter++)
	{
		if (*pszIter!='\n' && *pszIter != 0)
			continue;
		itemstr.Empty();
		itemstr.NCopy(pszWordStart, pszIter - pszWordStart);
		pszWordStart = pszIter + 1;
		if (itemstr.GetLength() == 0 && (*pszIter != ' ' || *pszIter != '\t'))
			continue;	//	���ʼ�������ֶ�������ո��tab��
		xarrLineTexts.append(itemstr);
		if (*pszIter == 0)
			break;
	}
	int nLines = xarrLineTexts.GetSize();
	for (int i = 0; i < nLines; i++)
	{
		CXhChar50 lineText = xarrLineTexts[i];
		if (lineText.GetLength() == 0)
			continue;
		lineText.Replace("\t", "");
		lineText.Replace("\n", "");
		lineText.Replace(" ", "");
		CXhChar16 sLegTypeName = strtok(lineText, "#");
		if(strstr(sLegTypeName,"Height"))
		{	//Height#1: level=1.5m
			sKey = strtok(NULL, ":");
			int iBodyNo= atoi(sKey);
			CXhChar100 sLineTexts = strtok(NULL, " ");
			sKey=strtok(sLineTexts,"=");
			if (sKey = strtok(NULL, "m"))
			{
				pHeightLeg = pListHeights->Append();
				pHeightLeg->idBodyNo = iBodyNo;
				pHeightLeg->wnLevelHeightDiff = ftoi(atof(sKey) * 1000);
			}
		}
		else if (strstr(sLegTypeName,"Leg"))
		{	//Leg#1: front=L1, vplane=v2, number=6,spec=L70x5      
			sKey = strtok(NULL, ":");
			int iLegNo = atoi(sKey);
			CXhChar16 sFront, sVplane, sDivideNum,sSpec;
			if (sKey = strtok(NULL, ","))
			{
				sFront.Copy(sKey);
				sFront.Replace("front=", "");
			}
			if (sKey = strtok(NULL, ","))
			{
				sVplane.Copy(sKey);
				sVplane.Replace("vplane=", "");
			}
			if (sKey = strtok(NULL, ","))
			{
				sDivideNum.Copy(sKey);
				sDivideNum.Replace("number=","");
			}
			if (sKey = strtok(NULL, ","))
			{
				sSpec.Copy(sKey);
				sSpec.Replace("spec=", "");
			}
			if (pHeightLeg)
			{
				pHeightLeg->xPresetLegs[iLegNo-1].szFrontLegLayout = sFront;
				pHeightLeg->xPresetLegs[iLegNo-1].szVPlaneLegLayout = sVplane;
				pHeightLeg->xPresetLegs[iLegNo-1].wiSectionNum = atoi(sDivideNum);
				pHeightLeg->xPresetLegs[iLegNo - 1].szSpec = sSpec;
				pHeightLeg->xPresetLegs[iLegNo-1].wnHeightDiff = (iLegNo-1)*pHeightLeg->wnLevelHeightDiff;
				pHeightLeg->cnPresetLegCount++;
				nMaxLeg = max(nMaxLeg,iLegNo);
			}
		}
	}
	return nMaxLeg;
}

CLDSLinePart* AppendLegFrontOrSidePanel(CLDSNode* pNodeS,CLDSNode* pNodeE, CLDSLinePart* pPostRod,char ciPanelSymbol, char* spec,BOOL bLeftLeg)
{
	if (pNodeS == NULL || pNodeE == NULL)
		return NULL;
	CLdsPartListStack stackparts(&Ta);
	CPtInLineCheck check(pNodeS->xOriginalPos, pNodeE->xOriginalPos);
	CLDSLinePart* pLegXieRod = NULL;
	for (pLegXieRod = Ta.EnumRodFirst(); pLegXieRod; pLegXieRod = Ta.EnumRodNext())
	{
		if (pLegXieRod->pStart == NULL || pLegXieRod->pEnd == NULL || pLegXieRod->m_bVirtualPart ||
			!pLegXieRod->ModuleTogetherWith(pNodeE))
			continue;
		bool blColinear = false;
		double fS = check.CheckPointEx(pLegXieRod->pStart->xOriginalPos, &blColinear);
		if (!blColinear || fS < -EPS || fS>1 + EPS)
			continue;
		double fE = check.CheckPointEx(pLegXieRod->pEnd->xOriginalPos, &blColinear);
		if (!blColinear || fE < -EPS || fE>1 + EPS)
			continue;
		if (fabs(fE - fS) > 0.5)
			break;
	}
	if (pLegXieRod)
		return pLegXieRod;
	//���б��
	GEPOINT vPanelNormal;
	if (ciPanelSymbol == 'Q')
		vPanelNormal.Set(0, 1, 0);
	else if (ciPanelSymbol == 'H')
		vPanelNormal.Set(0, -1, 0);
	else if (ciPanelSymbol == 'Z')
		vPanelNormal.Set(-1, 0, 0);
	else if (ciPanelSymbol == 'Y')
		vPanelNormal.Set(1, 0, 0);
	int iClassId = (pPostRod->IsAngle()) ? CLS_LINEANGLE : pPostRod->GetClassTypeId();
	pLegXieRod = (CLDSLinePart*)Ta.AppendPart(iClassId);
	pLegXieRod->pStart = pNodeS;
	pLegXieRod->pEnd = pNodeE;
	pLegXieRod->chLayer1st = 'L';
	pLegXieRod->chLayer2nd = 'X';
	pLegXieRod->chLayer3rd = ciPanelSymbol;
	pLegXieRod->cfgword = pNodeS->UnifiedCfgword()&pNodeE->UnifiedCfgword();
	if (strlen(spec)>0 && pLegXieRod->IsAngle())
	{	//�������
		CXhChar50 sGuige(spec);
		sGuige.Replace("L", "");
		double wing_wide=0,wing_thick=0;
		restore_JG_guige(sGuige, wing_wide, wing_thick);
		pLegXieRod->size_wide = wing_wide;
		pLegXieRod->size_thick = wing_thick;
	}
	if (pLegXieRod->IsAngle() && bLeftLeg)
	{	//���б��
		CLDSLineAngle* pLegXieJg = (CLDSLineAngle*)pLegXieRod;
		pLegXieJg->des_norm_x.bSpecific = false;
		pLegXieJg->des_norm_x.bByOtherWing = false;
		pLegXieJg->des_norm_x.hViceJg = pPostRod->handle;
		pLegXieJg->des_norm_x.near_norm = vPanelNormal;
		pLegXieJg->des_norm_y.bByOtherWing = true;
		pLegXieJg->desStartPos.jgber_cal_style = 2;
		pLegXieJg->desEndPos.jgber_cal_style = 2;
		//
		pLegXieJg->cal_x_wing_norm();
		pLegXieJg->cal_y_wing_norm();
	}
	else if(pLegXieRod->IsAngle() && !bLeftLeg)
	{	//�Ҳ�б��
		CLDSLineAngle* pLegXieJg = (CLDSLineAngle*)pLegXieRod;
		pLegXieJg->des_norm_x.bByOtherWing = true;
		pLegXieJg->des_norm_y.bSpecific = false;
		pLegXieJg->des_norm_y.bByOtherWing = false;
		pLegXieJg->des_norm_y.hViceJg = pPostRod->handle;
		pLegXieJg->des_norm_y.near_norm = vPanelNormal;
		pLegXieJg->desStartPos.jgber_cal_style = 2;
		pLegXieJg->desEndPos.jgber_cal_style = 2;
		//
		pLegXieJg->cal_x_wing_norm();
		pLegXieJg->cal_y_wing_norm();
	}
	pLegXieRod->CalPosition();
	NewLinePart(pLegXieRod);
	//
	return pLegXieRod;
}
int InsertLegSection(CLDSModule* pModule,NODESET& keyNodeSet,double fLegHeight,char* sGuige)
{
	if(pModule==NULL)
		pModule = Ta.GetActiveModule();
	if (pModule == NULL)
		return 0;
	CFGWORD actCfgword;
	int iLegNo = 0, nMaxLegs = CFGLEG::MaxLegs();
	for (iLegNo = 1; iLegNo < nMaxLegs; iLegNo++)
	{
		actCfgword = CFGWORD(pModule->GetBodyNo(), iLegNo);
		if (!pModule->m_dwLegCfgWord.And(actCfgword))
			break;
	}
	if (iLegNo == nMaxLegs)
	{
		AfxMessageBox("�ѳ�����ǰ������������!");
		return 0;
	}
	pModule->m_dwLegCfgWord.AddSpecWord(actCfgword);
	//�����Ȳ��ڵ�͸˼�
	CLDSLinePart* pNewPostRod = NULL;
	for (CLDSNode* pKeyNode = keyNodeSet.GetFirst(); pKeyNode; pKeyNode = keyNodeSet.GetNext())
	{
		CLDSLinePart* pFatherRod = (CLDSLinePart*)Ta.Parts.FromHandle(pKeyNode->hFatherPart, CLS_LINEPART);
		if (pFatherRod == NULL || pFatherRod->pStart == NULL || pFatherRod->pEnd == NULL)
			continue;
		f3dPoint line_vec = pFatherRod->pEnd->Position(false) - pFatherRod->pStart->Position(false);
		normalize(line_vec);
		CLDSLinePart* pUpLinkRod = NULL, *pDownLinkRod = NULL;
		if ((pFatherRod->pStart == pKeyNode && line_vec.z > 0) || (pFatherRod->pEnd == pKeyNode && line_vec.z < 0))
			pDownLinkRod = pFatherRod;
		else if ((pFatherRod->pStart == pKeyNode && line_vec.z < 0) || (pFatherRod->pEnd == pKeyNode && line_vec.z > 0))
			pUpLinkRod = pFatherRod;
		//������ڵ㸸�˼����Ӧ����һ����������
		if (pFatherRod->pStart == pKeyNode || pFatherRod->pEnd == pKeyNode)
		{	
			if (line_vec.z < 0)
				line_vec *= -1.0;
			CFGWORD cfgword = pFatherRod->UnifiedCfgword();
			for (CLDSLinePart* pCoupleRod=console.EnumRodFirst(); pCoupleRod;pCoupleRod = console.EnumRodNext())
			{
				if (pCoupleRod == pFatherRod || pCoupleRod->GetClassTypeId() != pFatherRod->GetClassTypeId())
					continue;
				if (pCoupleRod->pStart == NULL || pCoupleRod->pEnd == NULL || !cfgword.And(pCoupleRod->UnifiedCfgword()))
					continue;
				f3dPoint vec;
				if (pCoupleRod->pStart == pKeyNode)
					vec = pCoupleRod->pEnd->Position(false) - pCoupleRod->pStart->Position(false);
				else if (pCoupleRod->pEnd == pKeyNode)
					vec = pCoupleRod->pStart->Position(false) - pCoupleRod->pEnd->Position(false);
				if (vec.IsZero())
					continue;
				normalize(vec);
				double dd = vec * line_vec;
				if (pUpLinkRod == NULL && dd < -EPS_COS)
					pUpLinkRod = pCoupleRod;
				else if (pDownLinkRod == NULL && dd > EPS_COS)
					pDownLinkRod = pCoupleRod;
				if (pUpLinkRod&&pDownLinkRod)
					break;
			}
		}
		//����ڵ�
		f3dPoint joint_pos;	
		joint_pos.z = pKeyNode->Position(false).z + fLegHeight;
		joint_pos.x = pKeyNode->Position(false).x + fLegHeight * (line_vec.x / line_vec.z);
		joint_pos.y = pKeyNode->Position(false).y + fLegHeight * (line_vec.y / line_vec.z);
		CLDSNode *pInsertNode = console.AppendNode();
		pInsertNode->CopyProperty(pKeyNode);
		pInsertNode->SetPosition(joint_pos);
		if (pInsertNode->m_cPosCalType == 3)	//�˼���ָ��Z���������
		{
			pInsertNode->arrRelationNode[0] = pKeyNode->arrRelationNode[0];
			pInsertNode->arrRelationNode[1] = pKeyNode->arrRelationNode[1];
		}
		else if (pInsertNode->m_cPosCalType == 7 && pInsertNode->m_cPosCalViceType == 'Z')
		{
			pInsertNode->arrRelationNode[0] = pKeyNode->arrRelationNode[0];
			pInsertNode->arrRelationNode[1] = pKeyNode->arrRelationNode[1];
			pInsertNode->arrRelationNode[2] = pKeyNode->arrRelationNode[2];
			pInsertNode->attach_offset += fLegHeight;
		}
		//����˼�
		CLDSLinePart *pInsertRod = (CLDSLinePart*)console.AppendPart(pFatherRod->GetClassTypeId());
		pInsertRod->CopyProperty(pFatherRod);
		pInsertRod->pStart = pKeyNode;
		pInsertRod->pEnd = pInsertNode;
		if (pInsertRod->GetClassTypeId() == CLS_GROUPLINEANGLE)
			((CLDSGroupLineAngle*)pInsertRod)->AppendSonJg();
		if (pDownLinkRod)
			pInsertNode->hFatherPart = pDownLinkRod->handle;
		else
			pInsertNode->hFatherPart = pUpLinkRod->handle;
		if (pInsertRod->IsAngle())
		{
#ifdef __ANGLE_PART_INC_
			CLDSLineAngle *pInsertAngle = (CLDSLineAngle*)pInsertRod;
			CLDSLineAngle *pUpAngle = (CLDSLineAngle*)pUpLinkRod;
			CLDSLineAngle *pDownAngle = (CLDSLineAngle*)pDownLinkRod;
			pInsertAngle->m_cPosCtrlDatumLineType = ((CLDSLineAngle*)pFatherRod)->m_cPosCtrlDatumLineType;
			pInsertAngle->m_cPosCtrlType = ((CLDSLineAngle*)pFatherRod)->m_cPosCtrlType;
			//�Ǹ�ʼ�ն�λ����Ʋ���
			if (pUpLinkRod == NULL || pDownLinkRod == NULL)
			{
				CLDSLineAngle *pRefAngle = NULL;
				if (pUpLinkRod)
				{
					pRefAngle = pUpAngle;
					if (pRefAngle->pStart == pKeyNode)
					{
						pInsertAngle->desStartPos = pRefAngle->desEndPos;
						pInsertAngle->desEndPos = pRefAngle->desStartPos;
					}
					else if (pRefAngle->pEnd == pKeyNode)
					{
						pInsertAngle->desStartPos = pRefAngle->desStartPos;
						pInsertAngle->desEndPos = pRefAngle->desEndPos;
					}
				}
				else
				{
					pRefAngle = pDownAngle;
					if (pRefAngle->pStart == pKeyNode)
					{
						pInsertAngle->desStartPos = pRefAngle->desStartPos;
						pInsertAngle->desEndPos = pRefAngle->desEndPos;
					}
					else if (pRefAngle->pEnd == pKeyNode)
					{
						pInsertAngle->desStartPos = pRefAngle->desEndPos;
						pInsertAngle->desEndPos = pRefAngle->desStartPos;
					}
				}
				//�����˼���λ�������ؼ�������
				//���ո˼�ʼ�ն˾�Ϊ���ƽڵ�
				if (pRefAngle->pStart->m_cPosCalType == 0 && pRefAngle->pEnd->m_cPosCalType == 0)
				{	//���ո˼�ʼ�ն˾�Ϊ���ƽڵ�
					if (pDownLinkRod)
					{	//���ò���˼��ն˶�λ��ʽ
						SetAnglePosProjectToBer(pRefAngle, FALSE);
						//���òθ˼�Z�����С�˶�λ��ʽ
						SetAnglePosFaceOffset(pInsertAngle, pRefAngle->pStart == pKeyNode, pInsertAngle->handle, pRefAngle->handle);
					}
					else if (pUpLinkRod)
					{	//���ò���˼�ʼ�˶�λ��ʽ
						SetAnglePosFaceOffset(pInsertAngle, TRUE, pRefAngle->handle, pInsertAngle->handle);
						//���òθ˼�Z�����С�˶�λ��ʽ
						SetAnglePosProjectToBer(pRefAngle, pRefAngle->pEnd != pKeyNode);
					}
				}
				//�ؼ��ڵ�Ϊ�¶ο�ʼ�ڵ�
				else if (pDownLinkRod)
				{	//�������ø˼���ؼ��������˶�λ��ʽ(Z�����С��һ��)
					if (pRefAngle->pStart == pKeyNode)
					{
						CLDSLinePart *pNextRod = FindDownDatumMainRod(pRefAngle->pEnd);
						if (pNextRod&&pNextRod->IsAngle())
						{
							if (pNextRod->pStart == pRefAngle->pEnd)
								pRefAngle->desStartPos = ((CLDSLineAngle*)pNextRod)->desStartPos;
							else
								pRefAngle->desStartPos = ((CLDSLineAngle*)pNextRod)->desEndPos;
						}
					}
					else
					{
						CLDSLinePart *pNextRod = FindDownDatumMainRod(pRefAngle->pStart);
						if (pNextRod&&pNextRod->IsAngle())
						{
							if (pNextRod->pStart == pRefAngle->pStart)
								pRefAngle->desEndPos = ((CLDSLineAngle*)pNextRod)->desStartPos;
							else
								pRefAngle->desEndPos = ((CLDSLineAngle*)pNextRod)->desEndPos;
						}
					}
				}
				//�ؼ��ڵ�Ϊ�¶ν����ڵ�
				else if (pUpLinkRod)	//�������ø˼���ؼ��������˶�λ��ʽ(Z����ϴ��һ��)
					SetAnglePosProjectToBer(pRefAngle, pRefAngle->pEnd != pKeyNode);
			}
			else
			{
				if (pUpLinkRod->pStart == pKeyNode)
					pInsertAngle->desEndPos = pUpAngle->desStartPos;
				else if (pUpLinkRod->pEnd == pKeyNode)
					pInsertAngle->desEndPos = pUpAngle->desEndPos;
				if (pDownLinkRod->pStart == pKeyNode)
					pInsertAngle->desStartPos = pDownAngle->desStartPos;
				else if (pDownLinkRod->pEnd == pKeyNode)
					pInsertAngle->desStartPos = pDownAngle->desEndPos;
			}
			//�Ǹַ�����Ʋ���
			if (pUpAngle)
			{
				pInsertAngle->des_norm_x = pUpAngle->des_norm_x;
				pInsertAngle->des_norm_y = pUpAngle->des_norm_y;
				pInsertAngle->set_norm_x_wing(pUpAngle->vxWingNorm, FALSE);
				pInsertAngle->set_norm_y_wing(pUpAngle->vyWingNorm, FALSE);
			}
			else if (pDownAngle)
			{
				pInsertAngle->des_norm_x = pDownAngle->des_norm_x;
				pInsertAngle->des_norm_y = pDownAngle->des_norm_y;
				pInsertAngle->set_norm_x_wing(pDownAngle->vxWingNorm, FALSE);
				pInsertAngle->set_norm_y_wing(pDownAngle->vyWingNorm, FALSE);
			}
			pInsertAngle->CalPosition();
#endif
		}
		else if (pInsertRod->GetClassTypeId() == CLS_LINETUBE)
		{
#ifdef __COMPLEX_PART_INC_
			CLDSLineTube *pInsertTube = (CLDSLineTube*)pInsertRod;
			CLDSLineTube *pUpTube = (CLDSLineTube*)pUpLinkRod;
			CLDSLineTube *pDownTube = (CLDSLineTube*)pDownLinkRod;
			//�ֹ�ʼ�ն�λ����Ʋ���
			if (pUpLinkRod == NULL || pDownLinkRod == NULL)
			{
				CLDSLineTube *pRefTube = NULL;
				if (pUpLinkRod)
				{
					pRefTube = pUpTube;
					if (pRefTube->pStart == pKeyNode)
					{
						pInsertTube->desStartPos = pRefTube->desEndPos;
						pInsertTube->desEndPos = pRefTube->desStartPos;
					}
					else if (pRefTube->pEnd == pKeyNode)
					{
						pInsertTube->desStartPos = pRefTube->desStartPos;
						pInsertTube->desEndPos = pRefTube->desEndPos;
					}
				}
				else
				{
					pRefTube = pDownTube;
					if (pRefTube->pStart == pKeyNode)
					{
						pInsertTube->desStartPos = pRefTube->desStartPos;
						pInsertTube->desEndPos = pRefTube->desEndPos;
					}
					else if (pRefTube->pEnd == pKeyNode)
					{
						pInsertTube->desStartPos = pRefTube->desEndPos;
						pInsertTube->desEndPos = pRefTube->desStartPos;
					}
				}
				//�����˼���λ�������ؼ�������
				//���ո˼�ʼ�ն˾�Ϊ���ƽڵ�
				if (pRefTube->pStart->m_cPosCalType == 0 && pRefTube->pEnd->m_cPosCalType == 0)
				{
					if (pDownLinkRod)
					{	//���ò���˼��ն˶�λ��ʽΪ����ͶӰ
						SetTubePosProjectPoint(pRefTube, FALSE, pInsertRod->handle, pRefTube->handle);
						//���òθ˼�Z�����С�˶�λ��ʽΪ����ͶӰ
						SetTubePosProjectPoint(pInsertTube, pRefTube->pStart == pKeyNode, pInsertTube->handle, pRefTube->handle);
					}
					else if (pUpLinkRod)
					{	//���ò���˼�ʼ�˶�λ��ʽΪ����ͶӰ
						SetTubePosProjectPoint(pInsertTube, TRUE, pRefTube->handle, pInsertTube->handle);
						//���ò��ո˼�Z�����С��Ϊ����ͶӰ
						SetTubePosProjectPoint(pInsertTube, pRefTube->pEnd != pKeyNode, pInsertTube->handle, pRefTube->handle);
					}
				}
				//�ؼ��ڵ�Ϊ�¶ο�ʼ�ڵ������㣬�ҵ�ǰ�¶������������ڼ�
				else
				{
					if (pKeyNode == pRefTube->pStart)		//�����ն�����ʼ��λ�ò���
						SetTubePosProjectPoint(pRefTube, TRUE, pRefTube->desEndPos.hDatumStartPart, pRefTube->desEndPos.hDatumEndPart);
					else if (pKeyNode == pRefTube->pEnd)	//����ʼ������ʼ��λ�ò���
						SetTubePosProjectPoint(pRefTube, FALSE, pRefTube->desStartPos.hDatumStartPart, pRefTube->desStartPos.hDatumEndPart);
				}
			}
			else
			{	//�ֹ�ʼ�ն�λ����Ʋ���
				if (pUpLinkRod->pStart == pKeyNode)
					pInsertTube->desEndPos = pUpTube->desStartPos;
				else if (pUpLinkRod->pEnd == pKeyNode)
					pInsertTube->desEndPos = pUpTube->desEndPos;
				if (pDownLinkRod->pStart == pKeyNode)
					pInsertTube->desStartPos = pDownTube->desStartPos;
				else if (pDownLinkRod->pEnd == pKeyNode)
					pInsertTube->desStartPos = pDownTube->desEndPos;
			}
			//�ֹܹ����淨�߼������߲���
			if (pUpTube)
			{
				pInsertTube->nearWeldRoadVec = pUpTube->nearWeldRoadVec;
				pInsertTube->nearWorkPlaneNorm = pUpTube->nearWorkPlaneNorm;
				pInsertTube->hWorkNormRefPart = pUpTube->hWorkNormRefPart;
				pInsertTube->m_bHasWeldRoad = pUpTube->m_bHasWeldRoad;
			}
			else if (pDownTube)
			{
				pInsertTube->nearWeldRoadVec = pDownTube->nearWeldRoadVec;
				pInsertTube->nearWorkPlaneNorm = pDownTube->nearWorkPlaneNorm;
				pInsertTube->hWorkNormRefPart = pDownTube->hWorkNormRefPart;
				pInsertTube->m_bHasWeldRoad = pDownTube->m_bHasWeldRoad;
			}
			pInsertTube->CalPosition();
#endif
		}
		NewLinePart(pInsertRod);
		//���ò���ڵ㶨λ��ʽ
		pInsertNode->m_cPosCalType = 3;	//�˼���ָ��Z�����
		if (pUpLinkRod == NULL || pDownLinkRod == NULL)
		{
			CLDSLinePart *pRefRod = pUpLinkRod;
			if (pRefRod == NULL)
				pRefRod = pDownLinkRod;
			if (pRefRod->pStart->m_cPosCalType == 0 && pRefRod->pEnd->m_cPosCalType == 0)
			{	//���ո˼�ʼ�ն˾�Ϊ���ƽڵ�
				pInsertNode->arrRelationNode[0] = pRefRod->pStart->handle;
				pInsertNode->arrRelationNode[1] = pRefRod->pEnd->handle;
			}
			else if (pDownLinkRod)
			{	//���¶ζ�������ڼ�
				CLDSNode *pRefNode = NULL;
				if (pDownLinkRod->pStart != pKeyNode)
					pRefNode = pDownLinkRod->pStart;
				else if (pDownLinkRod->pEnd != pKeyNode)
					pRefNode = pDownLinkRod->pEnd;
				if (pRefNode&&pRefNode->m_cPosCalType == 3)
				{	//ȡ��ؼ��ڵ��Ӧ�Ľڵ�������ڵ�
					pInsertNode->arrRelationNode[0] = pRefNode->arrRelationNode[0];
					pInsertNode->arrRelationNode[1] = pRefNode->arrRelationNode[1];
				}
			}
			else //if(pUpLinkRod)
			{	//���¶εײ�����ڵ㣬���ò���ڵ�Ϊ���ƽڵ㣻�ؼ��ڵ�Ϊָ��Z����ڵ�
				CLDSNode *pRefNode = NULL;
				if (pUpLinkRod->pStart != pKeyNode)
					pRefNode = pUpLinkRod->pStart;
				else if (pUpLinkRod->pEnd != pKeyNode)
					pRefNode = pUpLinkRod->pEnd;
				if (pRefNode&&pRefNode->m_cPosCalType == 3)
				{	//���ùؼ��ڵ�Ϊָ��Z����ڵ�
					pKeyNode->m_cPosCalType = 3;
					pKeyNode->arrRelationNode[0] = pRefNode->arrRelationNode[0];
					pKeyNode->arrRelationNode[1] = pRefNode->arrRelationNode[1];
				}
				//���ò���ڵ�Ϊ���ƽڵ�
				pInsertNode->m_cPosCalType = 0;
				//���¶�ĩβ����½ڵ����Ҫ���½ڵ�����
				for (CLDSNode *pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
					pNode->ReplaceReferenceHandle(CHandleReplacer(pKeyNode->handle, pInsertNode->handle), CLS_NODE);
			}
		}
		else
		{	//ֱ��ʹ�ùؼ��ڵ�������ڵ�
			pInsertNode->arrRelationNode[0] = pKeyNode->arrRelationNode[0];
			pInsertNode->arrRelationNode[1] = pKeyNode->arrRelationNode[1];
		}
		pInsertNode->Set();
		pInsertNode->layer(0) = pInsertRod->layer(0) = 'L';
		pInsertNode->cfgword = pInsertRod->cfgword = actCfgword;
		NewNode(pInsertNode);
		//����
		if (pInsertRod->pStart->xOriginalPos.x > 0 || pInsertRod->pEnd->xOriginalPos.x > 0 ||
			pInsertRod->pStart->xOriginalPos.y < 0 || pInsertRod->pEnd->xOriginalPos.y < 0)
			continue;	//�˼����ڵڶ�����
		pNewPostRod = pInsertRod;
	}
	//�����Ȳ�б��
	CSuperSmartPtr<CLDSLinePart> linePartArr[4];
	if (pNewPostRod)
	{
		linePartArr[0] = pNewPostRod;
		linePartArr[1] = pNewPostRod->GetMirRod(MIRMSG(2));
		linePartArr[2] = pNewPostRod->GetMirRod(MIRMSG(3));
		linePartArr[3] = pNewPostRod->GetMirRod(MIRMSG(1));
	}
	if (linePartArr[1] == NULL || linePartArr[2] == NULL || linePartArr[3] == NULL)
		return iLegNo;
	CLDSNodePtr keyNodeTopArr[4] = { 0 }, keyNodeBtmArr[4] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		keyNodeTopArr[i] = linePartArr[i]->pStart;
		keyNodeBtmArr[i] = linePartArr[i]->pEnd;
	}
	CShankDefineToTowerModel::SetTowerModel(&Ta);
	char quad_layer[5] = "QYHZ";;
	CLDSNode* pNodeS = NULL, *pNodeE = NULL;
	CLDSLinePart *pInsertRod = NULL;
	for (int i = 0; i < 4; i++)
	{
		//���б��
		pNodeS = CShankDefineToTowerModel::AddDivScaleNode(keyNodeTopArr[i], keyNodeTopArr[(i+1)%4], 0.5);
		pNodeE = keyNodeBtmArr[i];
		pInsertRod=AppendLegFrontOrSidePanel(pNodeS,pNodeE,linePartArr[i],quad_layer[i], sGuige,TRUE);
		if (pInsertRod)
			pInsertRod->cfgword = actCfgword;
		//�Ҳ�б��
		pNodeE = keyNodeBtmArr[(i+1)%4];
		pInsertRod = AppendLegFrontOrSidePanel(pNodeS,pNodeE,linePartArr[i],quad_layer[i], sGuige,FALSE);
		if (pInsertRod)
			pInsertRod->cfgword = actCfgword;
	}
	return iLegNo;
}
CLDSLinePart* FindLegPostRod(CFGWORD& legCfgWord)
{
	CLDSLinePart* pRod = NULL;
	for (pRod = Ta.EnumRodFirst(); pRod; pRod = Ta.EnumRodNext())
	{
		if (!pRod->IsLegObj() || pRod->pStart == NULL || pRod->pEnd == NULL || pRod->m_bVirtualPart)
			continue;
		if (pRod->pStart->xOriginalPos.x > 0 || pRod->pEnd->xOriginalPos.x > 0 ||
			pRod->pStart->xOriginalPos.y < 0 || pRod->pEnd->xOriginalPos.y < 0)
			continue;	//�˼����ڵڶ�����
		SmartRodPtr pSmartRod = pRod;
		if (pSmartRod->IsCompositeSonAngle())
			continue;
		if (pRod->cfgword.And(legCfgWord))
			break;
	}
	return pRod;
}
extern bool SortModuleByHeight();
int  CLDSView::AddLegAndBraceRods()
{
	CLogErrorLife logErrLife;
	CCmdLockObject cmdlock(this);
	if (!cmdlock.LockSuccessed())
		return FALSE;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//��ȡ���ķ�������ʼ�������Ϣ
	CAddLegBraceDlg legBraceDlg;
	if (legBraceDlg.DoModal() != IDOK)
	{
		pCmdLine->CancelCmdLine();
		return FALSE;
	}
	CXhSimpleList<HEIGHT_LEGS> listHeightLegs;
	int nMaxLeg=InitHeightLegs(legBraceDlg.m_sLegScript, &listHeightLegs);
	if (listHeightLegs.Count <= 0 || Ta.Parts.GetNodeNum() <= 0)
		return FALSE;
	CWaitCursor waitCursor;
	//���������Ϣ����ӽ���
	if (nMaxLeg <= 8 && CFGLEG::MaxLegs() < 8)
		CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX08);
	else if (nMaxLeg > 8 && nMaxLeg <= 16 && CFGLEG::MaxLegs() < 16)
		CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX16);
	else if (nMaxLeg > 16 && nMaxLeg <= 24 && CFGLEG::MaxLegs() < 24)
		CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX24);
	long hActiveModule = Ta.m_hActiveModule;
	for (HEIGHT_LEGS* pItem=listHeightLegs.EnumObjectFirst();pItem;pItem=listHeightLegs.EnumObjectNext())
	{
		CLDSModule* pModule = NULL;
		for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
		{
			if(pModule->idBodyNo==pItem->idBodyNo)
				break;
		}
		if(pModule==NULL)
			continue;
		pItem->pModule = pModule;
		//ͳ�Ƶ�ǰ���ߵĽ�����Ϣ,��ʼ��ȱ��
		PRESET_ARRAY32<MODULE_LEG> validLegs;
		UINT nCopyCount = pModule->UpdateValidLegs(validLegs.pPresetObjs, 32);
		validLegs.ReSize(nCopyCount);
		for (int i = 0; i < pItem->cnPresetLegCount; i++)
		{
			for (WORD iLeg = 0; iLeg < validLegs.Count; iLeg++)
			{
				long nHeightDiff = ftoi(validLegs[0].m_fLowestZ - validLegs[iLeg].m_fLowestZ);
				if (pItem->xPresetLegs[i].wnHeightDiff == nHeightDiff)
				{
					pItem->xPresetLegs[i].wiMatchLeg = iLeg+1;
					break;
				}
			}
		}
		//���в���
		NODESET keyNodeSet;
		SmartRodPtr pSmartRod = validLegs[0].pLegPostRod;
		if (!pSmartRod.IsHasPtr())
			continue;
		CLDSNode* pBaseNode = pSmartRod->pStart;
		if (pSmartRod->pStart->xOriginalPos.z > pSmartRod->pEnd->xOriginalPos.z)
			pBaseNode = pSmartRod->pEnd;
		if (!IsMainShankNode(pBaseNode))
			continue;
		keyNodeSet.append(pBaseNode);
		CLDSNode *pMirXNode = pBaseNode->GetMirNode(MIRMSG(1));
		if(pMirXNode)
			keyNodeSet.append(pMirXNode);
		CLDSNode *pMirYNode = pBaseNode->GetMirNode(MIRMSG(2));
		if(pMirYNode)
			keyNodeSet.append(pMirYNode);
		CLDSNode *pMirZNode = pBaseNode->GetMirNode(MIRMSG(3));
		if(pMirZNode)
			keyNodeSet.append(pMirZNode);
		if (keyNodeSet.GetNodeNum() != 4)
		{
			logerr.Log("�Ҳ����Ȳ��ؼ��ڵ�");
			continue;
		}
		for (int i = 0; i < pItem->cnPresetLegCount; i++)
		{
			if(pItem->xPresetLegs[i].wiMatchLeg>0)
				continue;
			double fLegLowestZ = validLegs[0].m_fLowestZ - pItem->xPresetLegs[i].wnHeightDiff;
			double fLegH = fLegLowestZ-pBaseNode->xOriginalPos.z;
			CXhChar16 spec = pItem->xPresetLegs[i].szSpec;
			pItem->xPresetLegs[i].wiMatchLeg=InsertLegSection(pModule, keyNodeSet,fLegH,spec);
		}
	}
	//���������Ϣ���Խ��Ƚ��в���
	CShankDefineToTowerModel::SetTowerModel(&Ta);
	for (HEIGHT_LEGS* pItem = listHeightLegs.EnumObjectFirst(); pItem; pItem = listHeightLegs.EnumObjectNext())
	{
		if (pItem->pModule == NULL)
		{
			logerr.Log("�Ҳ������Ϊ%d�ĺ���",pItem->idBodyNo);
			continue;
		}
		//���������Ϣ���Խ��Ƚ���������|V�沼��
		CLDSNodePtr keyNodeArr[4], keyNodeTopSectArr[4]={0}, keyNodeBtmSectArr[4]={0};
		CSuperSmartPtr<CLDSLinePart> linePartArr[4];
		CInsertStdTemplateOperation operation;
		CXhChar100 lib_file;
		lib_file.Copy(theApp.execute_path);
		lib_file.Append("facepanel.pns");
		Ta.m_hActiveModule = 0;
		for (int i = 0; i < pItem->cnPresetLegCount; i++)
		{
			if (pItem->xPresetLegs[i].wiMatchLeg <= 0)
			{
				logerr.Log("%s�����Ҳ���ƥ��Ľ���(Ҫ����Ⱥ�%d)!", pItem->pModule->description, i+1);
				continue;
			}
			CFGWORD legword(pItem->pModule->GetBodyNo(), pItem->xPresetLegs[i].wiMatchLeg);
			CFGWORD bodyword = legword.ToBodyWord();
			CLDSLinePart* pLegPostRod = FindLegPostRod(legword);
			if(pLegPostRod)
			{
				linePartArr[0] = pLegPostRod;
				linePartArr[1] = pLegPostRod->GetMirRod(MIRMSG(2));
				linePartArr[2] = pLegPostRod->GetMirRod(MIRMSG(3));
				linePartArr[3] = pLegPostRod->GetMirRod(MIRMSG(1));
			}
			if(linePartArr[1]==NULL || linePartArr[2] ==NULL|| linePartArr[3] ==NULL)
				continue;
			for (int j = 0; j < 4; j++)
			{
				keyNodeTopSectArr[j] = linePartArr[j]->pStart;
				keyNodeBtmSectArr[j] = linePartArr[j]->pEnd;
			}
			//����������
			CFacePanel facepanel;
			facepanel.m_iFaceType = 6;	
			for (int j = 0; j < 4; j++)
			{
				keyNodeArr[0] = keyNodeTopSectArr[j];
				keyNodeArr[1] = keyNodeBtmSectArr[j];
				keyNodeArr[2] = keyNodeTopSectArr[(j+1)%4];
				keyNodeArr[3] = NULL;
				char specLayer[4] = "L??",quad_layer[5] = "QYHZ";;
				specLayer[2] = quad_layer[j];
				STDPANEL_INFO panelinfo(specLayer, NULL, 1);
				if (facepanel.ReadFromLibFile(lib_file, pItem->xPresetLegs[i].szFrontLegLayout))
				{
					facepanel.m_nInternodeNum = pItem->xPresetLegs[i].wiSectionNum;
					//ԭʼģ��һ���Ե�һ��������Ϊ��׼���壬����������֫����ת	wjh-2019.4.24
					panelinfo.blFlipHorizon = true;	
					CLDSNode* pTopHoriMidNode = CShankDefineToTowerModel::AddDivScaleNode(keyNodeArr[0], keyNodeArr[2], 0.5);
					keyNodeArr[2] = pTopHoriMidNode;
					operation.CreateStdTemplate(&facepanel, keyNodeArr, 0, 0, &panelinfo, &bodyword, &legword);
					//ԭʼģ��һ���Ե�һ��������Ϊ��׼���壬���Ҳ������֫����ת	wjh-2019.4.24
					keyNodeArr[0] = keyNodeTopSectArr[(j+1)%4];
					keyNodeArr[1] = keyNodeBtmSectArr[(j+1)%4];
					panelinfo.blFlipHorizon = false;	
					operation.CreateStdTemplate(&facepanel, keyNodeArr, 0, 0, &panelinfo, &bodyword, &legword);
				}
			}
			//V�ͱ�׼��
			facepanel.m_iFaceType = 5;
			for (int j = 0; j < 4; j++)
			{
				int jn = (j+1)%4,jnn = (j+2)%4;
				keyNodeArr[0] = CShankDefineToTowerModel::AddDivScaleNode(keyNodeTopSectArr[jn], keyNodeTopSectArr[jnn], 0.5);
				keyNodeArr[1] = CShankDefineToTowerModel::AddDivScaleNode(keyNodeTopSectArr[j], keyNodeTopSectArr[jn], 0.5);
				keyNodeArr[2] = keyNodeBtmSectArr[jn];
				keyNodeArr[3] = NULL;
				STDPANEL_INFO panelinfo("L?G", GEPOINT(0, 0, 1));
				if (facepanel.ReadFromLibFile(lib_file, pItem->xPresetLegs[i].szVPlaneLegLayout))
				{	//�趨��V�ͱ�׼��
					facepanel.m_nInternodeNum = pItem->xPresetLegs[i].wiSectionNum;
					operation.CreateStdTemplate(&facepanel, keyNodeArr, 0, 0, &panelinfo, NULL, &legword);
				}
			}
		}
	}
	//�������ͺ��߼����������¼��㼶��
	Ta.m_hActiveModule = hActiveModule;
	Ta.m_hActiveModule = Ta.Module.GetFirst()->handle;
	SortModuleByHeight();
	for(CLDSModule* pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
		pModule->UpdateValidLegs();
	g_pSolidDraw->Draw();
	//
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:", "");
#else
	pCmdLine->FillCmdLine("����:", "");
#endif
	return TRUE;
}

void CLDSView::OnAddLegAndBraceRods()
{
	AddLegAndBraceRods();
}

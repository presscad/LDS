//<LOCALE_TRANSLATE Confirm by hxr/>

//////////////////////////////////////////////////////////////////
//                  北京信狐天诚软件科技有限公司
// 文件名        :      ShankSectionModify.cpp
// 版本          :		V1.0
// 目的及主要功能:		响应LDS绝大部分工作区操作菜单命令
// 创建日期      :      2012.2.11
// 修改日期      :		2012.2.11
// 评审人        :
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
//设计说明
//功能：实现塔身主体加段（节间）、删段（节间）或修改节间垂直高度
//交互：从工作区中捕捉一到两个节点（可参照测量两点间距离），加段或修改节间垂直高度时应提示输入节间垂直高度
//输入:
//	1.加段：加段水平位置处任一主材节点，加段垂高
//  2.删段：删除节间的起止主材节点
//  3.改段：修改节间的起止主材节点及新垂直高度
//输出：变更节点平面以下的所有构件的位置及尺寸应完成更新

/////////////////////////////////////////////////////////////////////////////
// CLDSView

//构件位移
//构件移位
//判断节点是否为主要塔身主材控制节点
BOOL IsMainShankNode(CLDSNode* pNode)
{
	//条件：
	//1.从属杆件为塔身主材
	//2.节点依附类型应为指定Z坐标分量节点
	if(pNode==NULL||(pNode->Layer(0)!='S'&&pNode->Layer(0)!='L')||isdigit(pNode->layer(2))==0)
		return FALSE;
	if(pNode->m_cPosCalType==0||pNode->m_cPosCalType==3||			//3.杆件上Z坐标值不变点
		(pNode->m_cPosCalType==7&&pNode->m_cPosCalViceType=='Z'))	//7.沿杆件自某节点的垂直偏移点
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
			logerr.Log("选中象限角节点0X%X的依赖杆件不是塔身（腿）主材，不能进行节间的增删改操作",pNode->handle);
	}
	else if(pNode->m_cPosCalType==5)
		logerr.Log("选中象限角节点0X%X的定位类型为比例等分节点，不能进行节间的增删改操作",pNode->handle);
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
		return;	//不应判断第三个字符，否则该字符错误时将不处理，仅头部节点跳过不处理 wjh-2013.6.5
	if(pNode->m_cPosCalType==0||pNode->m_cPosCalType==3)
	{	//坐标无任何依赖节点及指定Z坐标分量节点直接累加Z坐标
		hashNodeOffsetCoordZ.SetValue(pNode->handle,z_offset);
	}
	else if(pNode->m_cPosCalType==7)
	{	//沿Z轴偏移指定垂高节点须根据基准节点是否已进行过偏移来判断是否需要增加偏移量
		CLDSNode *pDatumNode=Ta.Node.FromHandle(pNode->arrRelationNode[0]);
		CLDSNode *pRelaNodeStart=Ta.Node.FromHandle(pNode->arrRelationNode[1]);
		CLDSNode *pRelaNodeEnd=Ta.Node.FromHandle(pNode->arrRelationNode[2]);
		if(pDatumNode&&pDatumNode->Position(false).z>z_level)
			return;	//基点位于操作基准平面之下,当前点会自动随基点偏移
		else if(pDatumNode)
		{	//只偏移起始基准节点未进行偏移情况下的垂直偏移点的偏移分量
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
		return;	//不应判断第三个字符，否则该字符错误时将不处理，仅头部节点跳过不处理 wjh-2013.6.5
	if(pNode->m_cPosCalType!=0&&pNode->m_cPosCalType!=3&&pNode->m_cPosCalType!=7)
		return;
	double *pOffsetZ=hashNodeOffsetCoordZ.GetValue(pNode->handle);
	if(pOffsetZ==NULL)
		return;
	if(pNode->m_cPosCalType==0)
	{
		CSuperSmartPtr<CLDSPart> pRod=Ta.Parts.FromHandle(pNode->hFatherPart);
		if(pRod.IsHasPtr()&&pRod->IsLinePart()&&pRod.LinePartPointer()->pStart&&pRod.LinePartPointer()->pEnd)
		{	//有基准杆件时，无坐标依赖节点可沿基准杆件方向向下偏移一个垂高
			f3dPoint line_vec=pRod.LinePartPointer()->pEnd->Position(false)-pRod.LinePartPointer()->pStart->Position(false);
			normalize(line_vec);
			if(line_vec.z*(*pOffsetZ)<0)
				line_vec*=-1.0;
			if(fabs(line_vec.z)>EPS)
			{
				f3dPoint pos=pNode->Position(false)+line_vec*((*pOffsetZ)/line_vec.z);
				pNode->SetPosition(pos);
			}
			else	//出错了，一般不会这样
				pNode->SetPositionZ(pNode->Position(false).z+*pOffsetZ);
		}
		else if(pNode->layer(0)!='T')	//非头部节点, 直接进行Z坐标偏移
		{
			if(pRod.IsNULL())
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X nodes lack of parent rods.will influence its coordinate updating results!\n",pNode->handle);
			else if(pRod.LinePartPointer()->pStart==NULL)
				logerr.Log("0X%X rods lack of starting nodes.will influence the node's0X%X coordinate updating results!\n",pRod->handle,pNode->handle);
			else if(pRod.LinePartPointer()->pEnd==NULL)
				logerr.Log("0X%X rods lack of ending nodes.will influence the node's0X%X coordinate updating results!\n",pRod->handle,pNode->handle);
#else
				logerr.Log("0X%X节点缺少父杆件，会影响其坐标更新结果！\n",pNode->handle);
			else if(pRod.LinePartPointer()->pStart==NULL)
				logerr.Log("0X%X杆件缺少始端节点，会影响节点0X%X的坐标更新结果！\n",pRod->handle,pNode->handle);
			else if(pRod.LinePartPointer()->pEnd==NULL)
				logerr.Log("0X%X杆件缺少终端节点，会影响节点0X%X的坐标更新结果！\n",pRod->handle,pNode->handle);
#endif
			pNode->SetPositionZ(pNode->Position(false).z+*pOffsetZ);
		}
	}
	else if(pNode->m_cPosCalType==3)
	{	//坐标无任何依赖节点及指定Z坐标分量节点直接累加Z坐标
		pNode->SetPositionZ(pNode->Position(false).z+*pOffsetZ);
	}
	else if(pNode->m_cPosCalType==7)
	{	//沿Z轴偏移指定垂高节点须根据基准节点是否已进行过偏移来判断是否需要增加偏移量
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
//设置角钢指定端定位方式为平推
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
	pDesPos->spatialOperationStyle=17;	//点线平推
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
//设置角钢位置设计参数为向楞线投影
static void SetAnglePosProjectToBer(CLDSLineAngle *pAngle,BOOL bStart)
{
	if(pAngle==NULL)
		return;
	CMultiOffsetPos *pDesPos=&pAngle->desStartPos;
	if(!bStart)
		pDesPos=&pAngle->desEndPos;
	if(pDesPos==NULL)
		return;
	pDesPos->jgber_cal_style=0;		//楞点定位方式:分两步偏移
	pDesPos->origin_pos_style=0;
	pDesPos->spatialOperationStyle=1;		//基点定位方式:向楞向投影
	pDesPos->wing_x_offset.offsetDist=0;
	pDesPos->wing_y_offset.offsetDist=0;
	pDesPos->wing_x_offset.gStyle=4;
	pDesPos->wing_y_offset.gStyle=4;
}
#endif

//设置钢管定位方式为点线投影
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
	pDesPos->endPosType=2;	//点线投影
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
			continue;	//只处理分界平面之下的节点
		if(pNode->m_cPosCalType==0||pNode->m_cPosCalType==3||
			pNode->m_cPosCalType==7&&pNode->m_cPosCalViceType=='Z')
			continue;	//已处理过
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
	//基准单线顶部锚定节点
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
	{	//未通过简单方式找到底部锚定节点
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
		logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON, "未找到塔身底部的共线锚定基准节点");
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
	{	//根据实体端点坐标查找底部定位杆件
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
				continue;	//杆件始端点比拾取点过高
			if (pRod->xPosEnd.z < pQuad1KeyNode->xOriginalPos.z - 500)
				continue;	//杆件终端点比拾取点过高
			GEPOINT vLenStdVec= pRod->xPosEnd - pRod->xPosStart;
			normalize(vLenStdVec);
			double justify = vLenStdVec * check.LineStdVec();
			if (fabs(justify) < EPS_COS2)
				continue;	//不共线
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
		logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON, "底部基准杆件缺乏第2象限对称");
	if (pDefMode->xarrBtmLegRodsPtr[2] == NULL)
		logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON, "底部基准杆件缺乏第3象限对称");
	if (pDefMode->xarrBtmLegRodsPtr[3] == NULL)
		logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON, "底部基准杆件缺乏第4象限对称");
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
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "未找到合适的塔身主材1象限关键节点");
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
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "当前选中的关键节点非塔身主材杆件端节点");
		return false;
	}
	if (pDownLinkRod==NULL)
	{	//查找与节点父杆件相对应的另一根塔身主材
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
			bestrod.Update(dd, pCoupleRod);	//考虑到变坡处两杆件不一定共线，所以选择最接近共线的下侧杆件 wjh-2019.5.22
			/*if (pUpLinkRod == NULL && dd < -EPS_COS)
				pUpLinkRod = pCoupleRod;
			else if (pDownLinkRod == NULL && dd > EPS_COS)
				pDownLinkRod = pCoupleRod;*/
			if (dd>EPS_COS2)
				break;	//不可能存在更接近的角钢
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
//插入塔身节间
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
			AfxMessageBox("已超出当前最大呼高数限制!须手动切换呼高-接腿模式后再添加呼高");
#endif
			return 0;
		}
		//LDS软件按目录式新模式插入呼高,TMA软件中还是按照旧模式插入呼高
#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
		CNewTowerDlg newheightdlg;
		newheightdlg.m_xShankDef.mode.blAppendMode = true;
		newheightdlg.m_xShankDef.mode.ciInitHeightSerial = idBodyNo;
		if (GetStdCornerLineVecOfQuad1(keynodeset, &newheightdlg.m_xShankDef.mode))
		{	//按目录式新模式插入呼高
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
					AfxMessageBox(CXhChar50("系统暂时不支持单一呼高中容纳%d组减腿", cnNewLegsPerHeight));
					return 0;
				}
			}
			CFGLEG::SetSchema(ciMultiLegSchema);
			//出于稳妥考虑新建塔身时暂时暂不启用简单定位模式
			CStackVariant<BOOL> stackmode(&g_sysPara.m_bAnglePreferSimplePosMode, FALSE);
			CShankDefineToTowerModel modeler;
			{
				Ta.Parts.GetTail();
				CLdsPartListStack stack(&Ta.Parts);
				if (!modeler.UpdateTowerModel(&Ta, &newheightdlg.m_xShankDef, 4))
					return 0;
			}
			SCOPE_STRU scope;
			//应自动执行角钢摆放位置修正
			for (SmartRodPtr pRod = Ta.EnumRodNext(); pRod.blValid; pRod = Ta.EnumRodNext())
			{
				scope.VerifyVertex(pRod->pStart->xOriginalPos);
				scope.VerifyVertex(pRod->pEnd->xOriginalPos);
				if (pRod->layer(1) == 'Z' || !pRod->IsAngle())
					continue;	//塔身或腿部主材不修正
				CorrectSingleAngleLayout(pRod.pAngle);
			}
			//更新插入截面以上塔身及塔头部分的配材号 wjh-2019.5.23
			CLDSNode* pPickKeyNode = keynodeset.GetFirst();
			double zInsertPlane = newheightdlg.m_xShankDef.mode.dfTopCoordZ;
			for (CLDSNode* pNode = Ta.EnumNodeFirst(); pNode; pNode = Ta.EnumNodeNext()) 
			{
				if (!pNode->ModuleTogetherWith(pPickKeyNode))
					continue;	//与插入截面关键节点不共存
				if (pNode->cfgword.IsHasBodyNo(idBodyNo) || pNode->IsLegObj())
					continue;	//新增节点对象
				if (pNode->xOriginalPos.z > zInsertPlane + EPS2)
					continue;	//节点在插入截面以下
				pNode->cfgword.AddBodyLegs(idBodyNo);
			}
			for (SmartPartPtr pSmartPart = Ta.EnumPartFirst(); pSmartPart.blValid; pSmartPart = Ta.EnumPartNext())
			{
				if (!pSmartPart->ModuleTogetherWith(pPickKeyNode))
					continue;	//与插入截面关键节点不共存
				if (pSmartPart->cfgword.IsHasBodyNo(idBodyNo) || pSmartPart->IsLegObj())
					continue;	//新增构件对象
				if (pSmartPart->IsLinePart())
				{
					if (pSmartPart.pRod->pStart&&pSmartPart.pRod->pStart->xOriginalPos.z > zInsertPlane + EPS2)
						continue;	//始端在插入截面以下
					if (pSmartPart.pRod->pEnd&&pSmartPart.pRod->pEnd->xOriginalPos.z > zInsertPlane + EPS2)
						continue;	//末端在插入截面以下
					if (pSmartPart.pRod->pStart==NULL&&pSmartPart.pRod->xPosStart.z > zInsertPlane + EPS2)
						continue;	//始端点在插入截面以下
					if (pSmartPart.pRod->pEnd == NULL &&pSmartPart.pRod->xPosEnd.z > zInsertPlane + EPS2)
						continue;	//末端点在插入截面以下
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
			//模型树上添加新增的呼高节点
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
	CXhChar50 szVarTitle = (cDefaultSubType == 'B') ? "接身高度:" : "接腿高度";
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
		strcpy(pModule->description, "新增呼高");
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
			AfxMessageBox("已超出当前最大接腿数限制!");
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
		{//查找与节点父杆件相对应的另一根塔身主材
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
			AfxMessageBox("主材角钢插入失败!");
#endif
			if (cDefaultSubType == 'B'&&pModule->iNo == 0)
				Ta.Module.DeleteCursor();	//移除新增呼高, 并恢复原有激活呼高
			Ta.m_hActiveModule = _hOldActiveHeight;
			return false;
		}

		f3dPoint joint_pos;	//删除节间后的节间顶部与关键点相对应的衔接点
		joint_pos.z=pKeyNode->xOriginalPos.z+section_z;
		joint_pos.x=pKeyNode->xOriginalPos.x+section_z*(line_vec.x/line_vec.z);
		joint_pos.y=pKeyNode->xOriginalPos.y+section_z*(line_vec.y/line_vec.z);
		CLDSNode *pInsertNode=console.AppendNode();
		pInsertNode->CopyProperty(pKeyNode);
		pInsertNode->SetPosition(joint_pos);
		if(cDefaultSubType=='S')
			ReplaceDependencyNodeRelaInfo(pKeyNode->xOriginalPos.z,pKeyNode->handle,pInsertNode->handle);
		if(pInsertNode->m_cPosCalType==3)	//杆件上指定Z坐标分量点
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
			pUpLinkRod=NULL;	//变坡处---关键节点为控制节点且上下杆件均存在时以下端连接杆件为基准
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
				//角钢始终端位置设计参数
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
					//调整杆件定位参数及关键点类型
					//参照杆件始终端均为控制节点
					if(pRefAngle->pStart->m_cPosCalType==0&&pRefAngle->pEnd->m_cPosCalType==0)
					{	//参照杆件始终端均为控制节点
						if(pDownLinkRod)
						{	//设置插入杆件终端定位方式
							SetAnglePosProjectToBer(pRefAngle,FALSE);
							//设置参杆件Z坐标较小端定位方式
							SetAnglePosFaceOffset(pInsertAngle,pRefAngle->pStart==pKeyNode,pInsertAngle->handle,pRefAngle->handle);
						}
						else if(pUpLinkRod) 
						{	//设置插入杆件始端定位方式
							SetAnglePosFaceOffset(pInsertAngle,TRUE,pRefAngle->handle,pInsertAngle->handle);
							//设置参杆件Z坐标较小端定位方式
							SetAnglePosProjectToBer(pRefAngle,pRefAngle->pEnd!=pKeyNode);
						}
					}
					//关键节点为坡段开始节点
					else if(pDownLinkRod)
					{	//修正引用杆件与关键点相连端定位方式(Z坐标较小的一端)
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
					//关键节点为坡段结束节点
					else if(pUpLinkRod)	//修正引用杆件与关键点相连端定位方式(Z坐标较大的一端)
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
				//角钢法线设计参数
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
				//钢管始终端位置设计参数
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
					//调整杆件定位参数及关键点类型
					//参照杆件始终端均为控制节点
					if(pRefTube->pStart->m_cPosCalType==0&&pRefTube->pEnd->m_cPosCalType==0)
					{	
						if(pDownLinkRod)
						{	//设置插入杆件终端定位方式为点线投影
							SetTubePosProjectPoint(pRefTube,FALSE,pInsertRod->handle,pRefTube->handle);
							//设置参杆件Z坐标较小端定位方式为点线投影
							SetTubePosProjectPoint(pInsertTube,pRefTube->pStart==pKeyNode,pInsertTube->handle,pRefTube->handle);
						}
						else if(pUpLinkRod) 
						{	//设置插入杆件始端定位方式为点线投影
							SetTubePosProjectPoint(pInsertTube,TRUE,pRefTube->handle,pInsertTube->handle);
							//设置参照杆件Z坐标较小端为点线投影
							SetTubePosProjectPoint(pInsertTube,pRefTube->pEnd!=pKeyNode,pInsertTube->handle,pRefTube->handle);
						}
					}
					//关键节点为坡段开始节点或结束点，且当前坡段至少有两个节间
					else 
					{	
						if(pKeyNode==pRefTube->pStart)		//参照终端设置始端位置参数
							SetTubePosProjectPoint(pRefTube,TRUE,pRefTube->desEndPos.hDatumStartPart,pRefTube->desEndPos.hDatumEndPart);
						else if(pKeyNode==pRefTube->pEnd)	//参照始端设置始端位置参数
							SetTubePosProjectPoint(pRefTube,FALSE,pRefTube->desStartPos.hDatumStartPart,pRefTube->desStartPos.hDatumEndPart);
					}
				}
				else 
				{	//钢管始终端位置设计参数
					if(pUpLinkRod->pStart==pKeyNode)
						pInsertTube->desEndPos=pUpTube->desStartPos;
					else if(pUpLinkRod->pEnd==pKeyNode)
						pInsertTube->desEndPos=pUpTube->desEndPos;
					if(pDownLinkRod->pStart==pKeyNode)
						pInsertTube->desStartPos=pDownTube->desStartPos;
					else if(pDownLinkRod->pEnd==pKeyNode)
						pInsertTube->desStartPos=pDownTube->desEndPos;
				}
				//钢管工作面法线及焊道线参数
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
			{	//仅在增加塔身节间时才需要更新关键节点父杆件 wjh-2015.4.9
				pInsertRod->CalPosition();//如此时不计算，后期可能会因为默认值错误引起搭接平推计算异常 wjh-2015.4.9
				pKeyNode->hFatherPart=pInsertRod->handle;
			}
			//设置插入节点定位方式
			pInsertNode->m_cPosCalType=3;	//杆件上指定Z坐标点
			if(pUpLinkRod==NULL||pDownLinkRod==NULL)
			{
				CLDSLinePart *pRefRod=pUpLinkRod;
				if(pRefRod==NULL)
					pRefRod=pDownLinkRod;
				if(pRefRod->pStart->m_cPosCalType==0&&pRefRod->pEnd->m_cPosCalType==0)
				{	//参照杆件始终端均为控制节点
					pInsertNode->arrRelationNode[0]=pRefRod->pStart->handle;
					pInsertNode->arrRelationNode[1]=pRefRod->pEnd->handle;
				}
				else if(pDownLinkRod)
				{	//在坡段顶部插入节间
					CLDSNode *pRefNode=NULL;
					if(pDownLinkRod->pStart!=pKeyNode)
						pRefNode=pDownLinkRod->pStart;
					else if(pDownLinkRod->pEnd!=pKeyNode)
						pRefNode=pDownLinkRod->pEnd;
					if(pRefNode&&pRefNode->m_cPosCalType==3)
					{	//取与关键节点对应的节点的依附节点
						pInsertNode->arrRelationNode[0]=pRefNode->arrRelationNode[0];
						pInsertNode->arrRelationNode[1]=pRefNode->arrRelationNode[1];
					}
				}
				else //if(pUpLinkRod)
				{	//在坡段底部插入节点，设置插入节点为控制节点；关键节点为指定Z坐标节点
					CLDSNode *pRefNode=NULL;
					if(pUpLinkRod->pStart!=pKeyNode)
						pRefNode=pUpLinkRod->pStart;
					else if(pUpLinkRod->pEnd!=pKeyNode)
						pRefNode=pUpLinkRod->pEnd;
					if(pRefNode&&pRefNode->m_cPosCalType==3)
					{	//设置关键节点为指定Z坐标节点
						pKeyNode->m_cPosCalType=3;
						pKeyNode->arrRelationNode[0]=pRefNode->arrRelationNode[0];
						pKeyNode->arrRelationNode[1]=pRefNode->arrRelationNode[1];
					}
					//设置插入节点为控制节点
					pInsertNode->m_cPosCalType=0;
					//在坡段末尾添加新节点后需要更新节点引用
					for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
						pNode->ReplaceReferenceHandle(CHandleReplacer(pKeyNode->handle,pInsertNode->handle),CLS_NODE);
				}
			}
			else 
			{	//直接使用关键节点的依附节点
				pInsertNode->arrRelationNode[0]=pKeyNode->arrRelationNode[0];
				pInsertNode->arrRelationNode[1]=pKeyNode->arrRelationNode[1];
			}
		}
		/*else //前移 wjh-2019.8.1
		{
			pInsertNode->hFatherPart=pFatherRod->handle;
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("leg member angle insertion failed!");
#else
			AfxMessageBox("主材角钢插入失败!");
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
				{	//新插入杆件不需要更新
					double min_z=pKeyNode->Position(false).z;
					if(pRod->pStart&&pRod->pEnd&&(pRod->pStart->Position(false).z>min_z||pRod->pEnd->Position(false).z>min_z))
						pRod->ReplaceReferenceHandle(CHandleReplacer(pKeyNode->handle,pInsertNode->handle));
				}
				//关键节点为控制点时，插入新后需要更新引用句柄
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
	if(pNode->m_cPosCalType==3||pNode->m_cPosCalType==5)	//3.杆件上Z坐标值不变点;5.杆件上的比例等分点
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
	else if(pNode->m_cPosCalType==7&&pNode->m_cPosCalViceType=='Z')	//沿杆件自某节点的偏移点
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
//删除塔身节间
static void DeleteShankSection(NODESET &keynodeset,double min_z,double max_z)
{
	TAPER_SCOPE taper;
	CHashList<CLDSLinePart*>hashDatumRods;
	//是否需要用删除杆件来更新相邻杆件定位参数(删除节点最高点或最低点节点为控制点)
	CHashList<BOOL>hashUpdateRodDesPara;
	//记录基准节点与关键节点对应表 key为关键节点handle,value为基准节点handle
	CHashList<long>datumNodeHashKeyNode;
	for(CLDSNode* pKeyNode=keynodeset.GetFirst();pKeyNode;pKeyNode=keynodeset.GetNext())
	{
		taper.VerifyVertex(pKeyNode->Position(false));
		CLDSLinePart* pFatherRod=(CLDSLinePart*)Ta.Parts.FromHandle(pKeyNode->hFatherPart,CLS_LINEPART);
		if(pFatherRod==NULL||pFatherRod->pStart==NULL||pFatherRod->pEnd==NULL)
			continue;
		f3dPoint line_vec=pFatherRod->pEnd->Position(false)-pFatherRod->pStart->Position(false);
		if(pKeyNode->m_cPosCalType==0)
		{	//关键节点为控制点时需要查找关键节点上方杆件用来计算杆件延伸方向(变坡处)
			CLDSLinePart *pUpDatumRod=FindUpDatumMainRod(pKeyNode);
			if(pUpDatumRod)
				line_vec=pUpDatumRod->pEnd->Position(false)-pUpDatumRod->pStart->Position(false);
		}
		normalize(line_vec);
		f3dPoint joint_pos;	//删除节间后的节间顶部与关键点相对应的衔接点
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
				//基准节点为关键节点时，当前节间为坡段最高节间；与删除后杆件相连的基准杆件为关键点父杆件
				//基准节点为非关键节点时，查找与删除杆件相连的上端杆件
				if(pDatumNode->m_cPosCalType!=0)
					pDatumRod=FindUpDatumMainRod(pDatumNode);
				if(pDatumRod)
				{
					hashDatumRods.SetValue(pDatumNode->layer(2)-'0',pDatumRod);
					//记录对应象限主材是否需要更新始端或终端设计参数
					if(pKeyNode->m_cPosCalType==0)		//关键点为控制点
					{
						hashUpdateRodDesPara.SetValue(pDatumNode->layer(2)-'0',TRUE);
						//关键节点为控制点时，需要将基准节点设置为控制点(关键节点将被删除)
						pDatumNode->m_cPosCalType=0;	
					}
					else if(pDatumRod==pFatherRod)	//与关键节点对应的另一端节点为控制点
						hashUpdateRodDesPara.SetValue(pDatumNode->layer(2)-'0',TRUE);
					else								
						hashUpdateRodDesPara.SetValue(pDatumNode->layer(2)-'0',FALSE);
				}
				//记录关键节点与对应节点的对照关系,方便以后进行替换时使用
				datumNodeHashKeyNode.SetValue(pKeyNode->handle,pDatumNode->handle);
				if( pDatumNode->arrRelationNode[0]==pKeyNode->handle||pDatumNode->arrRelationNode[1]==pKeyNode->handle||
					pDatumNode->arrRelationNode[2]==pKeyNode->handle||pDatumNode->arrRelationNode[3]==pKeyNode->handle)
					PreReplaceSelfDependNode(pDatumNode,pKeyNode);
				//基准节点父杆件设置为关键节点父杆件
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
			continue;	//不全包括在删除节间Z区间范围内
		if(taper.IsIncludePoint(pRod->pStart->Position(false))&&taper.IsIncludePoint(pRod->pEnd->Position(false)))
		{
			int push=Ta.Node.push_stack();
			CLDSLinePart **ppDatumRod=NULL;
			if((pRod->layer(0)=='S'||pRod->layer(0)=='L')&&
				pRod->layer(1)=='Z'&&isdigit(pRod->layer(2)))
				ppDatumRod=hashDatumRods.GetValue(pRod->layer(2)-'0');
			if(ppDatumRod&&(datumNodeHashKeyNode.GetValue(pRod->pStart->handle)
				||datumNodeHashKeyNode.GetValue(pRod->pEnd->handle)))
			{	//pRod与关键节点关联时才需要进行以下操作
				Ta.ReplaceReferenceHandle(CHandleReplacer(pRod->handle,(*ppDatumRod)->handle),(*ppDatumRod)->GetClassTypeId());
				BOOL *pUpdateDesPara=hashUpdateRodDesPara.GetValue(pRod->layer(2)-'0');
				if(pUpdateDesPara&&(*pUpdateDesPara)==TRUE&&*ppDatumRod!=pRod)
				{	//要删除的节间最高点或最低点为控制点时需要更新基准杆件位置设计参数
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
	//在设置杆件位置参数时还需要使用始终端句柄进行比较，故节点替换操作放在最后
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
	cmdStr.Format("SECTION 操作类型[新插入(I)/垂高变更(M)/删除(D)]<%C>:",cDefaultType);
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
			pCmdLine->FillCmdLine("需要选项关键字","");
			pCmdLine->FinishCmdLine();
			cmdStr.Format("SECTION 操作类型[新插入(I)/垂高变更(M)/删除(D)]<%C>:",cDefaultType);
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
		if(cDefaultType=='I')	//插入新节间
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("please select leg member's key nodes of the insertion location, then press enter to confirm:","");
#else
			pCmdLine->FillCmdLine("请选择塔身上插入位置的塔身主材关键节点，然后回车键确认:","");
#endif
			g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_NODE));
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("please select tower nodes or V titled material which penetrate current node's vertical range, then press enter to confirm:","");
#else
			pCmdLine->FillCmdLine("请选择贯穿当前操作节间垂高范围的塔身节点或斜材，然后回车键确认:","");
#endif
			g_pSolidSnap->SetSelectPartsType(GetSingleWord(SELECTINDEX_NODE)|SELECT_LINEPART);
		}
		OpenWndSel();	//开窗选择
	}
	if(n>0||pCmdLine->GetStrFromCmdLine(cmdStr))
	{
		CLDSNode *pNode=NULL;
		n = g_pSolidSnap->GetLastSelectEnts(id_arr);
		NODESET keynodeset;
		CHashList<NODE_PTR> hashNodesById;
		//过滤塔身主材关键节点
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
			pCmdLine->PromptStrInCmdLine("未选中任何塔身关键节点，请查看选中节点的图层名是否有误！");
#endif
			return 0;
		}
		//计算节间Z轴方向区间范围
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
		//提取塔身节间范围定义关键角节点（位于塔身主材及顶部或底部平面上的节点）
		for(ppNode=hashNodesById.GetFirst();ppNode;ppNode=hashNodesById.GetNext())
		{
			if((*ppNode)->Position(false).z==max_z)
				keynodeset.append(*ppNode);
		}
		//补充对称但未选中的关键角节点
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
		//进行节间操作
		double section_z=3000;
		if(cDefaultType=='I')	//插入新节间
		{
			if(max_z>min_z)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->PromptStrInCmdLine("when inserting new operation, the nodes selected must be in the same horizontal plane!");
#else
				pCmdLine->PromptStrInCmdLine("插入新节间操作时，选中的节点必须位于同一水平面内！");
#endif
				return 0;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("SECTION section insertion operation type[tower body(S)/module entity(B)/connecting legs(L)]<%C>:", cInsertSubType);
#else
			cmdStr.Format("SECTION 操作插入节间类型[塔身(S)/呼高(B)/接腿(L)]<%C>:", cDefaultSubType);
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
					pCmdLine->FillCmdLine("需要选项关键字", "");
					pCmdLine->FinishCmdLine();
					cmdStr.Format("SECTION 操作插入节间类型[塔身(S)/呼高(B)/接腿(L)]<%C>:", cDefaultSubType);
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
				pCmdLine->PromptStrInCmdLine("选择贯穿当前操作节间垂高范围的塔身节点或斜材！");
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
				pCmdLine->PromptStrInCmdLine("please select tower nodes or V titled material which penetrate current node's vertical range！");
#else
				pCmdLine->PromptStrInCmdLine("选择贯穿当前操作节间垂高范围的塔身节点或斜材！");
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
			if(Input("节间高度:",&section_z,'f')>0)
#endif
				section_z-=(max_z-min_z);
			else 
				return 0;
		}
		CFGWORD shareword;
		shareword.Clear();	//多接腿时应只处理所涉及呼高
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
		if (cDefaultType == 'I')		//插入塔身节间
		{
			if ((section_z=InsertShankSection(keynodeset, cDefaultSubType))<=0)
				return 0;
		}
		else if (cDefaultType == 'D')	//删除塔身节间
			DeleteShankSection(keynodeset, min_z, max_z);
		//
		if(cDefaultType!='I'||cDefaultSubType=='S')
		{
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{	//首先，偏移坐标无任何依赖节点及指定Z坐标分量节点
				if(pNode->m_cPosCalType==7)
					continue;
				if(pNode->Position(false).z>min_z&&shareword.And(pNode->cfgword))
					InitNodeOffsetCoordZ(pNode,min_z,section_z);
			}
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{	//其次，根据起始基准节点偏移需要偏移的垂直偏移节点
				if(pNode->m_cPosCalType!=7)
					continue;
				if(pNode->Position(false).z>min_z&&shareword.And(pNode->cfgword))
					InitNodeOffsetCoordZ(pNode,min_z,section_z);
			}
		}
		//
		if(cDefaultType!='I'||cDefaultSubType=='S')
		{	//变更\删除节间高度、插入塔身节间等三种情况下应将当前操作平面以下全部同坡度移位 wjh-2019.5.22
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{	//首先，偏移坐标无任何依赖节点及指定Z坐标分量节点
				if(pNode->m_cPosCalType==7)
					continue;
				if(pNode->Position(false).z>min_z&&shareword.And(pNode->cfgword))
					OffsetNodeCoordZ(pNode);
			}
			for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{	//其次，根据起始基准节点偏移需要偏移的垂直偏移节点
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
//完善塔腿布材
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
			continue;	//	单词间允许出现多个连续空格或tab键
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
	//添加斜材
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
	{	//解析规格
		CXhChar50 sGuige(spec);
		sGuige.Replace("L", "");
		double wing_wide=0,wing_thick=0;
		restore_JG_guige(sGuige, wing_wide, wing_thick);
		pLegXieRod->size_wide = wing_wide;
		pLegXieRod->size_thick = wing_thick;
	}
	if (pLegXieRod->IsAngle() && bLeftLeg)
	{	//左侧斜材
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
	{	//右侧斜材
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
		AfxMessageBox("已超出当前最大接腿数限制!");
		return 0;
	}
	pModule->m_dwLegCfgWord.AddSpecWord(actCfgword);
	//插入腿部节点和杆件
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
		//查找与节点父杆件相对应的另一根塔身主材
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
		//插入节点
		f3dPoint joint_pos;	
		joint_pos.z = pKeyNode->Position(false).z + fLegHeight;
		joint_pos.x = pKeyNode->Position(false).x + fLegHeight * (line_vec.x / line_vec.z);
		joint_pos.y = pKeyNode->Position(false).y + fLegHeight * (line_vec.y / line_vec.z);
		CLDSNode *pInsertNode = console.AppendNode();
		pInsertNode->CopyProperty(pKeyNode);
		pInsertNode->SetPosition(joint_pos);
		if (pInsertNode->m_cPosCalType == 3)	//杆件上指定Z坐标分量点
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
		//插入杆件
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
			//角钢始终端位置设计参数
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
				//调整杆件定位参数及关键点类型
				//参照杆件始终端均为控制节点
				if (pRefAngle->pStart->m_cPosCalType == 0 && pRefAngle->pEnd->m_cPosCalType == 0)
				{	//参照杆件始终端均为控制节点
					if (pDownLinkRod)
					{	//设置插入杆件终端定位方式
						SetAnglePosProjectToBer(pRefAngle, FALSE);
						//设置参杆件Z坐标较小端定位方式
						SetAnglePosFaceOffset(pInsertAngle, pRefAngle->pStart == pKeyNode, pInsertAngle->handle, pRefAngle->handle);
					}
					else if (pUpLinkRod)
					{	//设置插入杆件始端定位方式
						SetAnglePosFaceOffset(pInsertAngle, TRUE, pRefAngle->handle, pInsertAngle->handle);
						//设置参杆件Z坐标较小端定位方式
						SetAnglePosProjectToBer(pRefAngle, pRefAngle->pEnd != pKeyNode);
					}
				}
				//关键节点为坡段开始节点
				else if (pDownLinkRod)
				{	//修正引用杆件与关键点相连端定位方式(Z坐标较小的一端)
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
				//关键节点为坡段结束节点
				else if (pUpLinkRod)	//修正引用杆件与关键点相连端定位方式(Z坐标较大的一端)
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
			//角钢法线设计参数
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
			//钢管始终端位置设计参数
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
				//调整杆件定位参数及关键点类型
				//参照杆件始终端均为控制节点
				if (pRefTube->pStart->m_cPosCalType == 0 && pRefTube->pEnd->m_cPosCalType == 0)
				{
					if (pDownLinkRod)
					{	//设置插入杆件终端定位方式为点线投影
						SetTubePosProjectPoint(pRefTube, FALSE, pInsertRod->handle, pRefTube->handle);
						//设置参杆件Z坐标较小端定位方式为点线投影
						SetTubePosProjectPoint(pInsertTube, pRefTube->pStart == pKeyNode, pInsertTube->handle, pRefTube->handle);
					}
					else if (pUpLinkRod)
					{	//设置插入杆件始端定位方式为点线投影
						SetTubePosProjectPoint(pInsertTube, TRUE, pRefTube->handle, pInsertTube->handle);
						//设置参照杆件Z坐标较小端为点线投影
						SetTubePosProjectPoint(pInsertTube, pRefTube->pEnd != pKeyNode, pInsertTube->handle, pRefTube->handle);
					}
				}
				//关键节点为坡段开始节点或结束点，且当前坡段至少有两个节间
				else
				{
					if (pKeyNode == pRefTube->pStart)		//参照终端设置始端位置参数
						SetTubePosProjectPoint(pRefTube, TRUE, pRefTube->desEndPos.hDatumStartPart, pRefTube->desEndPos.hDatumEndPart);
					else if (pKeyNode == pRefTube->pEnd)	//参照始端设置始端位置参数
						SetTubePosProjectPoint(pRefTube, FALSE, pRefTube->desStartPos.hDatumStartPart, pRefTube->desStartPos.hDatumEndPart);
				}
			}
			else
			{	//钢管始终端位置设计参数
				if (pUpLinkRod->pStart == pKeyNode)
					pInsertTube->desEndPos = pUpTube->desStartPos;
				else if (pUpLinkRod->pEnd == pKeyNode)
					pInsertTube->desEndPos = pUpTube->desEndPos;
				if (pDownLinkRod->pStart == pKeyNode)
					pInsertTube->desStartPos = pDownTube->desStartPos;
				else if (pDownLinkRod->pEnd == pKeyNode)
					pInsertTube->desStartPos = pDownTube->desEndPos;
			}
			//钢管工作面法线及焊道线参数
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
		//设置插入节点定位方式
		pInsertNode->m_cPosCalType = 3;	//杆件上指定Z坐标点
		if (pUpLinkRod == NULL || pDownLinkRod == NULL)
		{
			CLDSLinePart *pRefRod = pUpLinkRod;
			if (pRefRod == NULL)
				pRefRod = pDownLinkRod;
			if (pRefRod->pStart->m_cPosCalType == 0 && pRefRod->pEnd->m_cPosCalType == 0)
			{	//参照杆件始终端均为控制节点
				pInsertNode->arrRelationNode[0] = pRefRod->pStart->handle;
				pInsertNode->arrRelationNode[1] = pRefRod->pEnd->handle;
			}
			else if (pDownLinkRod)
			{	//在坡段顶部插入节间
				CLDSNode *pRefNode = NULL;
				if (pDownLinkRod->pStart != pKeyNode)
					pRefNode = pDownLinkRod->pStart;
				else if (pDownLinkRod->pEnd != pKeyNode)
					pRefNode = pDownLinkRod->pEnd;
				if (pRefNode&&pRefNode->m_cPosCalType == 3)
				{	//取与关键节点对应的节点的依附节点
					pInsertNode->arrRelationNode[0] = pRefNode->arrRelationNode[0];
					pInsertNode->arrRelationNode[1] = pRefNode->arrRelationNode[1];
				}
			}
			else //if(pUpLinkRod)
			{	//在坡段底部插入节点，设置插入节点为控制节点；关键节点为指定Z坐标节点
				CLDSNode *pRefNode = NULL;
				if (pUpLinkRod->pStart != pKeyNode)
					pRefNode = pUpLinkRod->pStart;
				else if (pUpLinkRod->pEnd != pKeyNode)
					pRefNode = pUpLinkRod->pEnd;
				if (pRefNode&&pRefNode->m_cPosCalType == 3)
				{	//设置关键节点为指定Z坐标节点
					pKeyNode->m_cPosCalType = 3;
					pKeyNode->arrRelationNode[0] = pRefNode->arrRelationNode[0];
					pKeyNode->arrRelationNode[1] = pRefNode->arrRelationNode[1];
				}
				//设置插入节点为控制节点
				pInsertNode->m_cPosCalType = 0;
				//在坡段末尾添加新节点后需要更新节点引用
				for (CLDSNode *pNode = Ta.Node.GetFirst(); pNode; pNode = Ta.Node.GetNext())
					pNode->ReplaceReferenceHandle(CHandleReplacer(pKeyNode->handle, pInsertNode->handle), CLS_NODE);
			}
		}
		else
		{	//直接使用关键节点的依附节点
			pInsertNode->arrRelationNode[0] = pKeyNode->arrRelationNode[0];
			pInsertNode->arrRelationNode[1] = pKeyNode->arrRelationNode[1];
		}
		pInsertNode->Set();
		pInsertNode->layer(0) = pInsertRod->layer(0) = 'L';
		pInsertNode->cfgword = pInsertRod->cfgword = actCfgword;
		NewNode(pInsertNode);
		//查找
		if (pInsertRod->pStart->xOriginalPos.x > 0 || pInsertRod->pEnd->xOriginalPos.x > 0 ||
			pInsertRod->pStart->xOriginalPos.y < 0 || pInsertRod->pEnd->xOriginalPos.y < 0)
			continue;	//杆件不在第二象限
		pNewPostRod = pInsertRod;
	}
	//生成腿部斜材
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
		//左侧斜材
		pNodeS = CShankDefineToTowerModel::AddDivScaleNode(keyNodeTopArr[i], keyNodeTopArr[(i+1)%4], 0.5);
		pNodeE = keyNodeBtmArr[i];
		pInsertRod=AppendLegFrontOrSidePanel(pNodeS,pNodeE,linePartArr[i],quad_layer[i], sGuige,TRUE);
		if (pInsertRod)
			pInsertRod->cfgword = actCfgword;
		//右侧斜材
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
			continue;	//杆件不在第二象限
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
	//读取布材方案，初始化设计信息
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
	//根据设计信息，添加接腿
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
		//统计当前呼高的接腿信息,初始化缺腿
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
		//进行补腿
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
			logerr.Log("找不到腿部关键节点");
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
	//根据设计信息，对接腿进行布材
	CShankDefineToTowerModel::SetTowerModel(&Ta);
	for (HEIGHT_LEGS* pItem = listHeightLegs.EnumObjectFirst(); pItem; pItem = listHeightLegs.EnumObjectNext())
	{
		if (pItem->pModule == NULL)
		{
			logerr.Log("找不到序号为%d的呼高",pItem->idBodyNo);
			continue;
		}
		//根据设计信息，对接腿进行正侧面|V面布材
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
				logerr.Log("%s呼高找不到匹配的接腿(要求接腿号%d)!", pItem->pModule->description, i+1);
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
			//接腿正侧面
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
					//原始模板一般以第一象限正面为基准定义，故左侧需进行肢朝向翻转	wjh-2019.4.24
					panelinfo.blFlipHorizon = true;	
					CLDSNode* pTopHoriMidNode = CShankDefineToTowerModel::AddDivScaleNode(keyNodeArr[0], keyNodeArr[2], 0.5);
					keyNodeArr[2] = pTopHoriMidNode;
					operation.CreateStdTemplate(&facepanel, keyNodeArr, 0, 0, &panelinfo, &bodyword, &legword);
					//原始模板一般以第一象限正面为基准定义，故右侧需进行肢朝向翻转	wjh-2019.4.24
					keyNodeArr[0] = keyNodeTopSectArr[(j+1)%4];
					keyNodeArr[1] = keyNodeBtmSectArr[(j+1)%4];
					panelinfo.blFlipHorizon = false;	
					operation.CreateStdTemplate(&facepanel, keyNodeArr, 0, 0, &panelinfo, &bodyword, &legword);
				}
			}
			//V型标准面
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
				{	//设定有V型标准面
					facepanel.m_nInternodeNum = pItem->xPresetLegs[i].wiSectionNum;
					operation.CreateStdTemplate(&facepanel, keyNodeArr, 0, 0, &panelinfo, NULL, &legword);
				}
			}
		}
	}
	//更新塔型呼高及构件，重新计算级差
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
	pCmdLine->FillCmdLine("命令:", "");
#endif
	return TRUE;
}

void CLDSView::OnAddLegAndBraceRods()
{
	AddLegAndBraceRods();
}

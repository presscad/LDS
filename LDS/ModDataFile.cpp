// ModDataFile.cpp: implementation of the CModDataFile class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "ModDataFile.h"
#include "GlobalFunc.h"
#include "XhCharString.h"
#include "SortFunc.h"
#include "ElectricTower.h"
#include "direct.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __PART_DESIGN_INC_
typedef CLDSModule* ModulePtr;
static int comparefun1(const NODE_PTR& item1,const NODE_PTR& item2)
{
	return compare_long(item1->pointI,item2->pointI);
}
static int comparefun2(const ModulePtr& item1,const ModulePtr& item2)
{
	return compare_long(item1->iNo,item2->iNo);
}
CXhChar100 CreateGuidStr()
{
	CXhChar100 szGUID;
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		szGUID.Printf("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"
			, guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2],
			guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
	return szGUID;
}
//////////////////////////////////////////////////////////////////////////
//SUB_LEG_INFO
bool SUB_LEG_INFO::ValidateRodLiveScope(CLDSLinePart* pRod,CBasicSegment* pParentSegment)
{
	if(pParentSegment!=NULL&&!pParentSegment->IsLegType())
		return false;
	//TODO:
	return true;
}
//////////////////////////////////////////////////////////////////////////
//CLegGroupItem
CLegGroupItem::CLegGroupItem()
{
	m_pTopTagNode=NULL;
	m_fExclusiveBodyH=0;
	m_fMaxLegH=0;
}
CLegGroupItem::~CLegGroupItem()
{

}
void CLegGroupItem::InitLegItemByModule(CLDSModule* pModule,CHashSet<CLDSNode*>& hashLinkNode)
{
	if(pModule==NULL)
		return;
	m_fMaxLegH=pModule->LowestZ();
	m_idHeight=pModule->GetBodyNo();
	NODESET allLegNodeSet;
	for(CLDSNode* pNode=hashLinkNode.GetFirst();pNode;pNode=hashLinkNode.GetNext())
	{
		double pos_z=pNode->Position().z;
		if(pNode->IsLegObj() && pNode->cfgword.IsHasBodyNo(m_idHeight))
			allLegNodeSet.append(pNode);
		else if(pNode->cfgword.IsHasBodyNoOnly(m_idHeight))
		{
			segmentNodeSet.append(pNode);
			if(pos_z>m_fExclusiveBodyH)
			{
				m_fExclusiveBodyH=pos_z;
				m_xLowestLegPt=pNode->xOriginalPos;
			}	
		}
	}
	LINEPARTSET allLegRodSet;
	CMinDouble min_leg_node;
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		if(pRod->IsLegObj() && pRod->cfgword.IsHasBodyNo(m_idHeight))
			allLegRodSet.append(pRod);
		else if(pRod->cfgword.IsHasBodyNoOnly(m_idHeight))
			segmentRodSet.append(pRod);
		else
			continue;
		//��ʼ���ú��ߵ��Ͻӱ����ǩ�ڵ�
		GEPOINT ptS = pRod->pStart->xOriginalPos;
		GEPOINT ptE = pRod->pEnd->xOriginalPos;
		if ((ptS.x <= 0 && ptS.y >= 0) && (ptE.x <= 0 && ptE.y >= 0))
		{	//�ڶ����޸˼�����Ϊ����������һ�£�����һ��ȡ��2����wjh-2020.2.19��
			CLDSNode* pNode = (ptS.z < ptE.z) ? pRod->pStart : pRod->pEnd;
			min_leg_node.Update(pNode->xActivePos.z, pNode);
		}
	}
	if (min_leg_node.IsInited())
		m_pTopTagNode = (CLDSNode*)min_leg_node.m_pRelaObj;
	if(allLegRodSet.GetNodeNum()>1)
	{	//������������
		//��ȡ�Ȳ���Ϣ
		int nMaxLegs=CFGLEG::MaxLegOfBody(pModule->idBodyNo);//MaxLegs();
		WORD activeQuadLegNo[4];
		memcpy(activeQuadLegNo,pModule->m_arrActiveQuadLegNo,8);
		for(int i=0;i<nMaxLegs;i++)
		{
			if(!pModule->m_dwLegCfgWord.And(CFGWORD(m_idHeight,i+1)))
				break;
			char cLegMark='A'+i;
			BYTE ciStartBitIndex=CFGLEG::BitAddrIndexOf(pModule->idBodyNo);
			pModule->m_arrActiveQuadLegNo[0]=ciStartBitIndex/*(m_idHeight-1)*nMaxLegs*/+cLegMark-'@';
			pModule->m_arrActiveQuadLegNo[1]=ciStartBitIndex/*(m_idHeight-1)*nMaxLegs*/+cLegMark-'@';
			pModule->m_arrActiveQuadLegNo[2]=ciStartBitIndex/*(m_idHeight-1)*nMaxLegs*/+cLegMark-'@';
			pModule->m_arrActiveQuadLegNo[3]=ciStartBitIndex/*(m_idHeight-1)*nMaxLegs*/+cLegMark-'@';
			SUB_LEG_INFO* pLegInfo=subLegInfoList.append();
			for(CLDSNode* pNode=allLegNodeSet.GetFirst();pNode;pNode=allLegNodeSet.GetNext())
			{
				if(!pModule->IsSonPart(pNode))
					continue;
				pLegInfo->legNodeSet.append(pNode);
				if(pNode->Position().z>pLegInfo->m_fLegH)
					pLegInfo->m_fLegH=pNode->xOriginalPos.z;
				//��¼�ú��ߵ���ȵĵ�����
				if(pNode->xOriginalPos.z>m_xLowestLegPt.z)
					m_xLowestLegPt=pNode->xOriginalPos;
			}
			pLegInfo->m_fLegH-=m_fMaxLegH;
			//�Ȳ��˼�
			for(CLDSLinePart* pRod=allLegRodSet.GetFirst();pRod;pRod=allLegRodSet.GetNext())
			{
				if(!pModule->IsSonPart(pRod))
					continue;
				//����Ȳ��˼����Ƿ���ڿ������(ʼ�˽ڵ���ն˽ڵ㶼���Ȳ��ڵ㣬������ͬһ������)
				BOOL bValidS=TRUE,bValidE=TRUE;
				if(pRod->pStart->IsLegObj())
					bValidS=pModule->IsSonPart(pRod->pStart);
				if(pRod->pEnd->IsLegObj())
					bValidE=pModule->IsSonPart(pRod->pEnd);
				if(bValidS && bValidE)
					pLegInfo->legRodSet.append(pRod);
				else
					logerr.Log("�˼�(0X%X)��ʼ�˽ڵ���ն˽ڵ㲻��ͬһ����!)");
				//��¼�ú��ߵ���ȵĵ�����
				if(pRod->pStart->xOriginalPos.z>m_xLowestLegPt.z)
					m_xLowestLegPt=pRod->pStart->xOriginalPos;
				if(pRod->pEnd->xOriginalPos.z>m_xLowestLegPt.z)
					m_xLowestLegPt=pRod->pEnd->xOriginalPos;
			}
		}
		memcpy(pModule->m_arrActiveQuadLegNo,activeQuadLegNo,8);
	}
	else if(allLegRodSet.GetNodeNum()==1)
	{	//�ֹܸ�һ������ֻ��һ���ȣ�û��SubLeg����
		for(CLDSNode* pNode=allLegNodeSet.GetFirst();pNode;pNode=allLegNodeSet.GetNext())
		{
			segmentNodeSet.append(pNode);
			double pos_z=pNode->Position().z;
			if(pos_z>m_fExclusiveBodyH)
			{
				m_fExclusiveBodyH=pos_z;
				m_xLowestLegPt=pNode->xOriginalPos;
			}	
		}
		for(CLDSLinePart* pRod=allLegRodSet.GetFirst();pRod;pRod=allLegRodSet.GetNext())
			segmentRodSet.append(pRod);
	}
}
CLDSNode* CLegGroupItem::FindMidNode(CLDSLinePart* pRod)
{
	CPtInLineCheck check(pRod->pStart->xOriginalPos,pRod->pEnd->xOriginalPos);
	CLDSNode* pFindNode=NULL;
	for(pFindNode=segmentNodeSet.GetFirst();pFindNode;pFindNode=segmentNodeSet.GetNext())
	{
		bool bOnLine=false;
		double scale=check.CheckPointEx(pFindNode->xOriginalPos,&bOnLine);
		if(bOnLine&&scale>0.1&&scale<0.9&&pFindNode->hFatherPart==pRod->handle)
			break;
	}
	return pFindNode;
}
//////////////////////////////////////////////////////////////////////////
//
CBodyItem::CBodyItem()
{
	m_pBtmTagNode=NULL;
	m_fBodyH=0;
	m_iLegS=0;
}
CBodyItem::~CBodyItem()
{

}
bool CBodyItem::IsIncludeLegItem(CLegGroupItem* pLegItem)
{
	return pLegItem->m_pTopTagNode==m_pBtmTagNode;	//����ж��������Ͻ���������������Ҫ��д
}
void CBodyItem::InitBodyItem(CLDSNode* &pTopTagNode,NODESET& selNodeSet,LINEPARTSET& selRodSet)
{
	for(CLDSNode* pNode=selNodeSet.GetFirst();pNode;pNode=selNodeSet.GetNext())
		bodyNodeSet.append(pNode);
	//
	double dfLowestZ=pTopTagNode?pTopTagNode->xOriginalPos.z:-1e50;
	for(CLDSLinePart* pRod=selRodSet.GetFirst();pRod;pRod=selRodSet.GetNext())
	{
		if(pRod->pStart==NULL&&pRod->pEnd!=NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�˼�ʼ�˽ڵ㶪ʧ",pRod->handle);
		else if(pRod->pStart!=NULL&&pRod->pEnd==NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�˼��ն˽ڵ㶪ʧ",pRod->handle);
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		bodyRodSet.append(pRod);
		GEPOINT ptS=pRod->pStart->Position();
		GEPOINT ptE=pRod->pEnd->Position();
		if((ptS.x<=0 &&ptS.y>=0) && (ptE.x<=0 &&ptE.y>=0))
		{	//�ڶ����޸˼�
			CLDSNode* pNode=(ptS.z>ptE.z)?pRod->pStart:pRod->pEnd;
			if(pNode->xOriginalPos.z>dfLowestZ)
			{
				dfLowestZ=pNode->xOriginalPos.z;
				m_pBtmTagNode=pNode;
			}
		}
	}
	if (pTopTagNode)
	{
		m_fBodyH=dfLowestZ-pTopTagNode->xOriginalPos.z;
		pTopTagNode=m_pBtmTagNode;
	}
}
//////////////////////////////////////////////////////////////////////////
//CModDataFile
CModDataFile::CModDataFile()
{
	m_fTowerHeight=0;
	m_bTurnLeft = FALSE;
}
CModDataFile::~CModDataFile()
{
	m_xAssistNode.Empty();
}
GECS CModDataFile::BuildUcsByModCS()
{
	GECS ucs;
	ucs.origin.Set(0,0,m_fTowerHeight);
	ucs.axis_x.Set(1,0,0);
	ucs.axis_y.Set(0,-1,0);
	ucs.axis_z.Set(0,0,-1);
	return ucs;
}
//����ģ��Ҫ����ת������ģ��ˮƽ��ת180�㣨����ģ������ϵ�е�X���Y��*-1��
GECS CModDataFile::BuildRotateCS()
{
	GECS cs;
	cs.origin.Set(0, 0, 0);
	cs.axis_x.Set(-1, 0, 0);
	cs.axis_y.Set(0, -1, 0);
	cs.axis_z.Set(0, 0, 1);
	return cs;
}
BOOL CModDataFile::RetrieveBodyInfo(HEIGHT_JOINT xarrHeightJoints[24],
		NODESET& validNodeSet,NODESET& selNodeSet,LINEPARTSET& validRodSet,LINEPARTSET& selRodSet)
{
	int i,n,least=0;
	for(i=0;i<24;i++)
	{ 
		if (xarrHeightJoints[i].idHeightSerial==0)
		{
			n=i;
			break;
		}
		else if (!xarrHeightJoints[i].blJointed)
			least++;
	}
	if (least < 2)
		return FALSE;	//ֻʣһ�����ߣ��������崦��
	//��ȡ����ڵ�
	for(CLDSNode* pNode=validNodeSet.GetFirst();pNode;pNode=validNodeSet.GetNext())
	{
		BOOL blAllShared=TRUE;
		for (i = 0; i < n; i++)
		{
			if (xarrHeightJoints[i].blJointed)
				continue;
			if (!pNode->cfgword.IsHasBodyNo(xarrHeightJoints[i].idHeightSerial))
			{
				blAllShared = FALSE;
				break;
			}
		}
		if(blAllShared)
			selNodeSet.append(pNode);
	}
	//��ȡ����˼�
	for(CLDSLinePart* pRod=validRodSet.GetFirst();pRod;pRod=validRodSet.GetNext())
	{
		BOOL blAllShared=TRUE;
		for (i = 0; i < n; i++)
		{
			if (xarrHeightJoints[i].blJointed)
				continue;
			if(!pRod->cfgword.IsHasBodyNo(xarrHeightJoints[i].idHeightSerial))
			{
				blAllShared=FALSE;
				break;
			}
		}
		if(blAllShared)
			selRodSet.append(pRod);
	}
	return (selNodeSet.GetNodeNum()>0&&selRodSet.GetNodeNum()>0);
}
CLegGroupItem* CModDataFile::FindLegGroupItem(int idHeightSerial)
{
	CLegGroupItem* pLegGroupItem=NULL;
	for(pLegGroupItem=m_listHeightItem.GetFirst();pLegGroupItem;pLegGroupItem=m_listHeightItem.GetNext())
	{
		if(pLegGroupItem->m_idHeight==idHeightSerial)
			break;
	}
	return pLegGroupItem;
}
BOOL CModDataFile::IsHasValidPointI()
{
	CHashList<BOOL> hashByPointI;
	for(CLDSNode* pNode=m_hashLinkNode.GetFirst();pNode;pNode=m_hashLinkNode.GetNext())
	{
		if(pNode->point_i<=0)
			return FALSE;
		if(hashByPointI.GetValue(pNode->point_i))
			return FALSE;
		hashByPointI.SetValue(pNode->point_i,TRUE);
	}
	return TRUE;
}
extern bool SortModuleByHeight();
CLDSNode* FindLowestHorizArmControlNode()
{
	CMapList<CLDSLineAngle*> horizAngleMapList;
	LINEPARTSET horizArmRodSet;
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL||!pRod->IsAngle())
			continue;
		if(toupper(pRod->Layer(0))=='T')
		{
			if(toupper(pRod->Layer(1))=='Z')
				horizArmRodSet.append(pRod);
			continue;
		}
		CLDSLineAngle *pAngle=(CLDSLineAngle*)pRod;
		CLDSLinePart *pStartRod=console.FromRodHandle(pAngle->pStart->hFatherPart);
		CLDSLinePart *pEndRod=console.FromRodHandle(pAngle->pEnd->hFatherPart);
		if( pStartRod==NULL||pEndRod==NULL||pStartRod==pAngle||pEndRod==pAngle||
			toupper(pStartRod->Layer(1))!='Z'||toupper(pEndRod->Layer(1))!='Z')
			continue;
		f3dPoint len_vec=(pRod->End()-pRod->Start()).normalized();
		double ddx=len_vec*f3dPoint(1,0,0),ddy=len_vec*f3dPoint(0,1,0);
		if(fabs(ddx)<EPS_COS2&&fabs(ddy)<EPS_COS2)
			continue;
		horizAngleMapList.SetValue(pAngle->pStart->handle,pAngle->pEnd->handle,pAngle);
	}
	if(horizArmRodSet.GetNodeNum()<=0)
		return NULL;
	CLDSNode* pMaxZNode=NULL;
	CMaxDouble lowestHorizNodeZ;
	for(CLDSNode* pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext())
	{	//�Եڶ����޵Ľڵ�Ϊ��׼�ڵ㣬���ݶԳƵ�ȷ�����������
		if(pNode->Position().x>0 || pNode->Position().y<0)	
			continue;
		int iPush=Ta.PushNodeStack();
		CLDSNode* pMirX = pNode->GetMirNode(MIRMSG(1));
		CLDSNode* pMirY = pNode->GetMirNode(MIRMSG(2));
		CLDSNode* pMirZ = pNode->GetMirNode(MIRMSG(3));
		Ta.PopNodeStack(iPush);
		if(pMirX==NULL || pMirY==NULL || pMirZ==NULL)		//�������ԳƵĽڵ�
			continue;
		if( (horizAngleMapList.GetValue(pNode->handle,pMirX->handle)==NULL&&horizAngleMapList.GetValue(pMirX->handle,pNode->handle)==NULL)||
			(horizAngleMapList.GetValue(pNode->handle,pMirY->handle)==NULL&&horizAngleMapList.GetValue(pMirY->handle,pNode->handle)==NULL)||
			(horizAngleMapList.GetValue(pMirZ->handle,pMirX->handle)==NULL&&horizAngleMapList.GetValue(pMirX->handle,pMirZ->handle)==NULL)||
			(horizAngleMapList.GetValue(pMirZ->handle,pMirY->handle)==NULL&&horizAngleMapList.GetValue(pMirY->handle,pMirZ->handle)==NULL))
			continue;	//�Գƽڵ�������Ӹ˼�
		//�ж��Ƿ�Ϊ�ᵣ���ĺ�����
		CLDSLinePart* pHorizArmRod=NULL;
		for(pHorizArmRod=horizArmRodSet.GetFirst();pHorizArmRod;pHorizArmRod=horizArmRodSet.GetNext())
		{
			if(pHorizArmRod->pStart!=pNode && pHorizArmRod->pEnd!=pNode)
				continue;
			f3dPoint ptS=pHorizArmRod->pStart->xOriginalPos;
			f3dPoint ptE=pHorizArmRod->pEnd->xOriginalPos;
			if((fabs(ptS.z-ptE.z))<10)
				break;
		}
		if(pHorizArmRod)
		{
			if(lowestHorizNodeZ.Update(pNode->xOriginalPos.z)==pNode->xOriginalPos.z)
				pMaxZNode=pNode;
		}
	}
	return pMaxZNode;
}

BOOL CModDataFile::PrepareParaData()
{
	//������Ч�����ӽڵ�
	m_hashLinkNode.Empty();
	ARRAY_LIST<NODE_PTR> nodeArr;
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		CLDSNode* pNodeS=pRod->pStart;
		if(pNodeS && m_hashLinkNode.GetValue(pNodeS->handle)==NULL)
		{
			m_hashLinkNode.SetValue(pNodeS->handle,pNodeS);
			nodeArr.append(pNodeS);
		}
		//
		CLDSNode* pNodeE=pRod->pEnd;
		if(pNodeE && m_hashLinkNode.GetValue(pNodeE->handle)==NULL)
		{
			m_hashLinkNode.SetValue(pNodeE->handle,pNodeE);
			nodeArr.append(pNodeE);
		}
	}
	//���ݽڵ��������������¸�ֵ��m_hashLinkNode
	BOOL bHasPtI=IsHasValidPointI();
	if(bHasPtI)
	{
		CQuickSort<NODE_PTR>::QuickSort(nodeArr.m_pData,nodeArr.GetSize(),comparefun1);
		m_hashLinkNode.Empty();
		for(int i=0;i<nodeArr.GetSize();i++)
			m_hashLinkNode.SetValue(nodeArr[i]->handle,nodeArr[i]);
	}
	//��ʼ���ڵ���
	int index=0,iMaxIndex=0;
	NODESET validBodyNodeSet;
	LINEPARTSET validBodyRodSet;
	for(CLDSNode* pNode=m_hashLinkNode.GetFirst();pNode;pNode=m_hashLinkNode.GetNext())
	{
		if(bHasPtI)
			index=pNode->point_i;
		else
			index+=1;
		m_hashNodeData.SetValue(pNode->handle,NODE_DATA(index));
		if(index>iMaxIndex)
			iMaxIndex=index;
		if(!pNode->IsLegObj())
			validBodyNodeSet.append(pNode);
	}
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(!pRod->IsLegObj())
			validBodyRodSet.append(pRod);
	}
	//�����ߴӵ͵��߽�������
	SortModuleByHeight();
	ARRAY_LIST<ModulePtr> moduleArr;
	for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		moduleArr.append(pModule);
	CQuickSort<ModulePtr>::QuickSort(moduleArr.m_pData,moduleArr.GetSize(),comparefun2);
	HEIGHT_JOINT xarrHeightJoints[24];
	int i,nNum=moduleArr.GetSize();
	for(i=0;i<moduleArr.GetSize();i++)
	{
		double lowest_module_z=0;
		CLDSModule *pModule=moduleArr[i];
		pModule->GetModuleScopeZ(NULL,&lowest_module_z);
		if(lowest_module_z>m_fTowerHeight)
			m_fTowerHeight=lowest_module_z;
		//��ȡ�������ߵĶ��зֶκͽ�����Ϣ
		CLegGroupItem* pLegItem=m_listHeightItem.append();
		pLegItem->InitLegItemByModule(pModule,m_hashLinkNode);
		xarrHeightJoints[i].idHeightSerial=(BYTE)pLegItem->m_idHeight;
	}
	//��ȡ������Ϣ
	CLDSNode* pBodyBtmTagNode=NULL;	//ȡ������߽ڵ�Ϊ�����ǩ�ڵ��ʼֵ����ֵ��ʾ����ײ���ǩ�ڵ� wjh-2020.2.19
	for(pNode=validBodyNodeSet.GetFirst();pNode;pNode=validBodyNodeSet.GetNext())
	{
		if(pBodyBtmTagNode==NULL||pBodyBtmTagNode->xOriginalPos.z>pNode->xOriginalPos.z)
			pBodyBtmTagNode=pNode;
	}
	if(nNum==1)
	{	//ֻ��һ������
		CBodyItem* pBodyItem=m_listBodyItem.append();
		pBodyItem->InitBodyItem(pBodyBtmTagNode,validBodyNodeSet,validBodyRodSet);
		CLegGroupItem* pLegItem=m_listHeightItem.GetFirst();
		pLegItem->m_pTopTagNode=pBodyBtmTagNode;
		if(pLegItem->subLegInfoList.GetNodeNum()>0)
		{	//�����߶���ȣ�LEG���ֿ�Ϊ��
			pLegItem->segmentNodeSet.Empty();
			pLegItem->segmentRodSet.Empty();
			pLegItem->m_fExclusiveBodyH=0;
		}
		else
		{	//�����ߵ�����,ֻ�����Ȳ���Ϣ
			double fTagZ=pBodyBtmTagNode?pBodyBtmTagNode->xOriginalPos.z:0;
			pLegItem->m_fExclusiveBodyH=pLegItem->m_fMaxLegH-fTagZ;
			pLegItem->m_fMaxLegH=0;
			for(CLDSNode* pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
			{
				if(pNode->IsBodyObj())
					pLegItem->segmentNodeSet.DeleteCursor();
			}
			pLegItem->segmentNodeSet.Clean();
			//
			for(CLDSLinePart* pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				if(pRod->IsBodyObj())
					pLegItem->segmentRodSet.DeleteCursor();
			}
			pLegItem->segmentRodSet.Clean();
		}
	}
	else
	{	//�����
		int niLeastHeights=nNum;
		do
		{	//��ȡ����
			NODESET selNodeSet;
			LINEPARTSET selRodSet;
			if (RetrieveBodyInfo(xarrHeightJoints, validBodyNodeSet, selNodeSet, validBodyRodSet, selRodSet))
			{
				CBodyItem* pBodyItem = m_listBodyItem.append();
				pBodyItem->InitBodyItem(pBodyBtmTagNode, selNodeSet, selRodSet);
				if (pBodyBtmTagNode == NULL)
				{
					logerr.Log("������������(%d-%d),�Ҳ��������ڵ�!", xarrHeightJoints[index].idHeightSerial, xarrHeightJoints[nNum - 1].idHeightSerial);
					return FALSE;
				}
				for (CLDSNode* pNode = selNodeSet.GetFirst(); pNode; pNode = selNodeSet.GetNext())
					validBodyNodeSet.DeleteNode(pNode->handle);
				for (CLDSLinePart* pRod = selRodSet.GetFirst(); pRod; pRod = selRodSet.GetNext())
					validBodyRodSet.DeleteNode(pRod->handle);
				//��ʼ���ϽӸñ����Leg
				CLegGroupItem* pLegItem = NULL;
				int index=0;
				for (pLegItem = m_listHeightItem.GetFirst(); pLegItem; pLegItem = m_listHeightItem.GetNext(),index++)
				{

					if(xarrHeightJoints[index].blJointed)
						continue;
					if (pLegItem->m_pTopTagNode == pBodyBtmTagNode)
					{
						xarrHeightJoints[index].blJointed= true;
						niLeastHeights--;
						if (pLegItem->m_fExclusiveBodyH > 0)
						{
							pLegItem->m_fMaxLegH -= pLegItem->m_fExclusiveBodyH;
							pLegItem->m_fExclusiveBodyH -= pBodyBtmTagNode->xOriginalPos.z;
						}
						else
							pLegItem->m_fMaxLegH -= pBodyBtmTagNode->xOriginalPos.z;
					}
				}
			}
		} while (niLeastHeights>1);
		if (niLeastHeights==1)//hashState.GetNodeNum()!= bodyNoArr.GetSize())
		{
			logerr.Log("�����������½�Legƥ��ʧ��!");
			return FALSE;
		}
	}
	//��ȡ�ҵ���Ϣ����������
	bool initWireNodePosZ=false;
	GEPOINT maxWireNodePos;
	CLDSNode* pMaxZHorizArmNode=FindLowestHorizArmControlNode();
	if(pMaxZHorizArmNode)
	{
		initWireNodePosZ=true;
		maxWireNodePos=pMaxZHorizArmNode->xOriginalPos;
	}
	for(CXhWirePoint* pWirePt=gxWirePointModel.m_hashWirePoints.GetFirst();pWirePt;pWirePt=gxWirePointModel.m_hashWirePoints.GetNext())
	{
		HANG_NODE_INFO* pGuaInfo=m_listGuaNode.append();
		pGuaInfo->m_xWirePlace=pWirePt->m_xWirePlace;
		pGuaInfo->pos=pWirePt->xPosition;
		if(pWirePt->m_xWirePlace.ciWireType=='C')			//����
			pGuaInfo->m_ciType='C';
		else if(pWirePt->m_xWirePlace.ciWireType=='E')	//����
			pGuaInfo->m_ciType='G';
		else //if(pWirePt->m_xWirePlace.ciWireType=='J')	//����
			pGuaInfo->m_ciType='T';
		if(strlen(pWirePt->m_sGimName)>0)
			strcpy(pGuaInfo->m_sName,pWirePt->m_sGimName);
		if(initWireNodePosZ)
			continue;
		if(maxWireNodePos.z<pWirePt->xPosition.z)
			maxWireNodePos=pWirePt->xPosition;
	}
	CQuickSort<HANG_NODE_INFO>::QuickSortClassic(m_listGuaNode.m_pData,m_listGuaNode.GetSize());
	//��ȡ���߷�����Ϣ
	//�뱣֤ԭ������������ڱ���ԽӺ��ߺŲ������������Body1�Խ�1,3���ߣ�Body2�Խ�2,4���ߣ�
	//����m_listHeightItemΪ���ѭ����m_listBodyItemΪ�ڴ�ѭ�� wjh-2020.2.19
	int body_index=1,leg_index=1;
	for (CLegGroupItem* pLegItem=m_listHeightItem.GetFirst();pLegItem;pLegItem=m_listHeightItem.GetNext(),leg_index++)
	{
		CBodyItem* pBodyItem=NULL;
		for (pBodyItem=m_listBodyItem.GetFirst(),body_index=1;pBodyItem;pBodyItem=m_listBodyItem.GetNext(),body_index++)
		{
			if (pBodyItem->IsIncludeLegItem(pLegItem))//pLegItem->m_hTagNode!=pBodyItem->m_hTagNode)
				break;
		}
		if (pBodyItem==NULL)
		{
			logerr.Log("error");
			continue;
		}
		MODULE_ITEM* pModuleItem=m_listModuleItem.append();
		pModuleItem->m_iBody=body_index;
		pModuleItem->m_iLeg=leg_index;
		bool blCalSuccessed=false;
		double dfNamedHeight=Ta._IntelliCalNamedHeightZeroZ(&blCalSuccessed);
		blCalSuccessed = true;	//��ʱ��ӣ����������������ж��󱨵���ָ�����߻���ֵ��Ч������ wjh-2019.5.7
		if (blCalSuccessed)//Ta.NamedHeightZeroZ>0)
			pModuleItem->m_fNamedHeight=pLegItem->m_xLowestLegPt.z-Ta.NamedHeightZeroZ;
		else
			pModuleItem->m_fNamedHeight=pLegItem->m_xLowestLegPt.z-maxWireNodePos.z;
		for (i=1;i<=body_index;i++)
			pModuleItem->m_arrBody.Append(i);
	}
	//���ݵ���Ĺ淶Ҫ��,��鲢�޸�subleg���Ǵ��ڿ�εĸ˼�
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		for(CLDSNode* pNode=pBodyItem->bodyNodeSet.GetFirst();pNode;pNode=pBodyItem->bodyNodeSet.GetNext())
		{
			NODE_DATA* pNodeItem=m_hashNodeData.GetValue(pNode->handle);
			pNodeItem->ciLevel=0;
		}
		for(CLDSLinePart* pRod=pBodyItem->bodyRodSet.GetFirst();pRod;pRod=pBodyItem->bodyRodSet.GetNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				continue;
			NODE_DATA* pNodeItemS=m_hashNodeData.GetValue(pRod->pStart->handle);
			NODE_DATA* pNodeItemE=m_hashNodeData.GetValue(pRod->pEnd->handle);
			ROD_DATA* pRodItem=m_hashRodData.Add(pRod->handle);
			pRodItem->iPointS=pNodeItemS->iPointI;
			pRodItem->iPointE=pNodeItemE->iPointI;
		}
		//Leg
		for(CLegGroupItem* pLegItem=m_listHeightItem.GetFirst();pLegItem;pLegItem=m_listHeightItem.GetNext())
		{		
			if(!pBodyItem->IsIncludeLegItem(pLegItem))//pLegItem->m_hTagNode!=pBodyItem->m_hTagNode)
				continue;
			for(CLDSNode* pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
			{
				NODE_DATA* pPoint=m_hashNodeData.GetValue(pNode->handle);
				pPoint->ciLevel=1;
			}
			for(CLDSLinePart* pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
					continue;
				NODE_DATA* pNodeItemS=m_hashNodeData.GetValue(pRod->pStart->handle);
				NODE_DATA* pNodeItemE=m_hashNodeData.GetValue(pRod->pEnd->handle);
				ROD_DATA* pRodItem=m_hashRodData.Add(pRod->handle);
				pRodItem->iPointS=pNodeItemS->iPointI;
				pRodItem->iPointE=pNodeItemE->iPointI;
			}
			//SubLeg
			CHashList<NODE_DATA> hashNewAddPoint;
			for(SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.GetFirst();pSubLeg;pSubLeg=pLegItem->subLegInfoList.GetNext())
			{
				for(CLDSNode* pNode=pSubLeg->legNodeSet.GetFirst();pNode;pNode=pSubLeg->legNodeSet.GetNext())
				{
					NODE_DATA* pPoint=m_hashNodeData.GetValue(pNode->handle);
					pPoint->ciLevel=2;
				}
				//���ҵ��³��ֿ�θ˼��Ķ˵�,������п����滻
				for(pRod=pSubLeg->legRodSet.GetFirst();pRod;pRod=pSubLeg->legRodSet.GetNext())
				{
					if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
						continue;
					//ʼ��
					NODE_DATA* pNodeItemS=m_hashNodeData.GetValue(pRod->pStart->handle);
					if(pNodeItemS->ciLevel==0)
					{
						NODE_DATA* pNewPoint=hashNewAddPoint.GetValue(pRod->pStart->handle);
						if(pNewPoint==NULL)
						{	//����µĽڵ㣬���浽Leg
							iMaxIndex+=1;
							CLDSNode* pNode=m_xAssistNode.append();
							pNode->handle=Ta.GetNewHandle();
							pNode->point_i=iMaxIndex;
							pNode->SetPosition(pRod->pStart->xOriginalPos);
							pLegItem->segmentNodeSet.append(pNode);
							m_hashNodeData.SetValue(pNode->handle,NODE_DATA(iMaxIndex));
							//
							pNewPoint=hashNewAddPoint.Add(pRod->pStart->handle);
							pNewPoint->ciLevel=1;
							pNewPoint->iPointI=pNode->point_i;
						}
						pNodeItemS=pNewPoint;
					}
					//�ն�
					NODE_DATA* pNodeItemE=m_hashNodeData.GetValue(pRod->pEnd->handle);
					if(pNodeItemE->ciLevel==0)
					{
						NODE_DATA* pNewPoint=hashNewAddPoint.GetValue(pRod->pEnd->handle);
						if(pNewPoint==NULL)
						{	//��ӽڵ�
							iMaxIndex+=1;
							CLDSNode* pNode=m_xAssistNode.append();
							pNode->handle=Ta.GetNewHandle();
							pNode->point_i=iMaxIndex;
							pNode->SetPosition(pRod->pEnd->xOriginalPos);
							pLegItem->segmentNodeSet.append(pNode);
							m_hashNodeData.SetValue(pNode->handle,NODE_DATA(iMaxIndex));
							//
							pNewPoint=hashNewAddPoint.Add(pRod->pEnd->handle);
							pNewPoint->ciLevel=1;
							pNewPoint->iPointI=pNode->point_i;
						}
						pNodeItemE=pNewPoint;
					}
					//
					ROD_DATA* pRodItem=m_hashRodData.Add(pRod->handle);
					pRodItem->iPointS=pNodeItemS->iPointI;
					pRodItem->iPointE=pNodeItemE->iPointI;
				}
			}
		}
	}
	return TRUE;
}
BOOL CModDataFile::AmendData()
{
	//��ʼ���ڵ����꣬�����ĺ�
	GECS ucs=BuildUcsByModCS();
	for(CLDSNode* pNode=Ta.EnumNodeFirst();pNode;pNode=Ta.EnumNodeNext())
	{
		f3dPoint pos=pNode->Position();
		pos=ucs.TransPToCS(pos);
		pNode->SetPosition(pos);
		pNode->cfgword.Clear();
	}
	//��ʼ���˼��ռ�λ�ã������ĺ�
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		CLDSNode* pNodeS=pRod->pStart;
		CLDSNode* pNodeE=pRod->pEnd;
		pRod->SetStart(pNodeS->xOriginalPos);
		pRod->SetEnd(pNodeE->xOriginalPos);
		if(pRod->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
			JG_NORM_INFO* pJgNorm=m_hashJgNorm.GetValue(pJg->handle);
			if(pJgNorm)
			{
				f3dPoint vNormX=pJgNorm->vNormX;
				f3dPoint vNormY=pJgNorm->vNormY;
				vNormX=ucs.TransVToCS(vNormX);
				vNormY=ucs.TransVToCS(vNormY);
				pJg->set_norm_x_wing(vNormX);
				pJg->set_norm_y_wing(vNormY);
			}
			pJg->ClearFlag();
			pJg->desStartPos.jgber_cal_style=pJg->desEndPos.jgber_cal_style=1;
			pJg->CalPosition();
		}
		//
		pRod->cfgword.Clear();
	}
	//��Ӻ���
	CObjNoGroup *pGroup=Ta.NoManager.FromGroupID(MODEL_GROUP);
	pGroup->EmptyNo();
	Ta.Module.Empty();
	Ta.m_hActiveModule=0;
	CLDSModule *pModule=NULL;
	CHashSet<CLDSModule*> hashHeightModules;
	for(MODULE_ITEM* pModuleItem=m_listModuleItem.GetFirst();pModuleItem;pModuleItem=m_listModuleItem.GetNext())
	{
		pModule=Ta.Module.append();
		pModule->SetBodyNo(pModuleItem->m_iLeg);
		if(pModuleItem->m_fNamedHeight>0)
		{
			double height=fto_halfi(pModuleItem->m_fNamedHeight*0.001);
			CXhChar50 name(height);
			name.Append('m');
			StrCopy(pModule->description,name,51);
		}
		else
			sprintf(pModule->description,"����%d",pModuleItem->m_iLeg);
		pModule->iNo=pGroup->EnumIdleNo();
		pGroup->SetNoState(pModule->iNo);
		if(Ta.m_hActiveModule<=0X20)
			Ta.m_hActiveModule=pModule->handle;
		hashHeightModules.SetValue(pModuleItem->m_iLeg,pModule);
	}
	int nMaxLegsPerBody=0;
	for(CLegGroupItem* pLegItem=m_listHeightItem.GetFirst();pLegItem;pLegItem=m_listHeightItem.GetNext())
	{
		int nLeg=pLegItem->subLegInfoList.GetNodeNum();
		if(nMaxLegsPerBody<nLeg)
			nMaxLegsPerBody=nLeg;
	}
	if(nMaxLegsPerBody<=8&&CFGLEG::MaxLegs()!=8)
		CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX08);
	else if(nMaxLegsPerBody> 8&&nMaxLegsPerBody<=16&&CFGLEG::MaxLegs()!=16)
		CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX16);
	else if(nMaxLegsPerBody>16&&nMaxLegsPerBody<=24&&CFGLEG::MaxLegs()!=24)
		CFGLEG::SetSchema(CFGLEG::MULTILEG_MAX24);
	const DWORD flagConstArr[24]={
		0x000001,0x000002,0x000004,0x000008,0x000010,0x000020,0x000040,0x000080,
		0x000100,0x000200,0x000400,0x000800,0x001000,0x002000,0x004000,0x008000,
		0x010000,0x020000,0x040000,0x080000,0x100000,0x200000,0x400000,0x800000	};
	DWORD flag=0;
	for(CLegGroupItem* pLegItem=m_listHeightItem.GetFirst();pLegItem;pLegItem=m_listHeightItem.GetNext())
	{
		pModule=hashHeightModules.GetValue(pLegItem->m_idHeight);
		if(pModule==NULL)
			continue;
		if(pLegItem->subLegInfoList.GetNodeNum()>0)
		{	//��������
			UINT legword=flag=0;
			for(SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.GetFirst();pSubLeg;pSubLeg=pLegItem->subLegInfoList.GetNext())
			{
				for(CLDSNode* pNode=pSubLeg->legNodeSet.GetFirst();pNode;pNode=pSubLeg->legNodeSet.GetNext())
				{
					pNode->cfgword.AddBodyLegs(pModule->GetBodyNo(),flagConstArr[legword]);
					pNode->layer(0)='L';
				}
				for(CLDSLinePart* pRod=pSubLeg->legRodSet.GetFirst();pRod;pRod=pSubLeg->legRodSet.GetNext())
				{
					pRod->cfgword.AddBodyLegs(pModule->GetBodyNo(),flagConstArr[legword]);
					pRod->layer(0)='L';
				}
				flag|=flagConstArr[legword];
				legword++;
			}
			pModule->m_dwLegCfgWord.Clear();
			pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),flag);
			pModule->m_arrActiveQuadLegNo[0]=pModule->m_arrActiveQuadLegNo[1]=
				pModule->m_arrActiveQuadLegNo[2]=pModule->m_arrActiveQuadLegNo[3]=CFGLEG::MaxLegs()*(pModule->GetBodyNo()-1)+1;
			//���߶�����
			for(CLDSNode* pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
			{
				pNode->cfgword.SetBodyLegs(pModule->GetBodyNo());
				pNode->layer(0)='S';
			}
			for(CLDSLinePart* pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				pRod->cfgword.SetBodyLegs(pModule->GetBodyNo());
				pRod->layer(0)='S';
			}
		}
		else
		{	//ƽ�����
			pModule->m_dwLegCfgWord.Clear();
			pModule->m_dwLegCfgWord.AddBodyLegs(pModule->GetBodyNo(),flagConstArr[0]);
			pModule->m_arrActiveQuadLegNo[0]=pModule->m_arrActiveQuadLegNo[1]=
				pModule->m_arrActiveQuadLegNo[2]=pModule->m_arrActiveQuadLegNo[3]=CFGLEG::MaxLegs()*(pModule->GetBodyNo()-1)+1;
			for(CLDSNode* pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
			{
				pNode->cfgword.AddBodyLegs(pModule->GetBodyNo(),flagConstArr[0]);
				pNode->layer(0)='L';
			}
			for(CLDSLinePart* pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				pRod->cfgword.AddBodyLegs(pModule->GetBodyNo(),flagConstArr[0]);
				pRod->layer(0)='L';
			}
		}
	}
	//����(���߹����)
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		CFGWORD cfgword;
		for(int iNo=pBodyItem->m_iLegS;iNo<=m_listHeightItem.GetNodeNum();iNo++)
			cfgword.AddBodyLegs(iNo);
		for(CLDSNode* pNode=pBodyItem->bodyNodeSet.GetFirst();pNode;pNode=pBodyItem->bodyNodeSet.GetNext())
		{
			pNode->cfgword=cfgword;
			pNode->layer(0)='S';
		}
		for(CLDSLinePart* pRod=pBodyItem->bodyRodSet.GetFirst();pRod;pRod=pBodyItem->bodyRodSet.GetNext())
		{
			pRod->cfgword=cfgword;
			pRod->layer(0)='S';
		}
	}
	//��������MOD��ʽ�ĸ˼����˼����ļ�ĩβ��
	CBodyItem* pFirBody=m_listBodyItem.GetFirst();
	if(pFirBody && pFirBody->bodyRodSet.GetNodeNum()<=0)
	{
		for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			CLDSNode* pNodeS=pRod->pStart;
			CLDSNode* pNodeE=pRod->pEnd;
			if(pNodeE->layer(0)==pNodeS->layer(0)&&pNodeS->layer(0)=='S')
			{
				pRod->cfgword=pNodeS->cfgword&pNodeE->cfgword;
				pRod->layer(0)='S';	
			}
			else if(pRod->pStart->layer(0)=='L')
			{
				pRod->cfgword=pNodeS->cfgword;
				pRod->layer(0)=pNodeS->layer(0);
			}
			else
			{	
				pRod->cfgword=pNodeE->cfgword;
				pRod->layer(0)=pNodeE->layer(0);
			}
			pRod->SetStart(pNodeS->xOriginalPos);
			pRod->SetEnd(pNodeE->xOriginalPos);
			if(pRod->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
				JG_NORM_INFO* pJgNorm=m_hashJgNorm.GetValue(pJg->handle);
				if(pJgNorm)
				{
					f3dPoint vNormX=pJgNorm->vNormX;
					f3dPoint vNormY=pJgNorm->vNormY;
					vNormX=ucs.TransVToCS(vNormX);
					vNormY=ucs.TransVToCS(vNormY);
					pJg->set_norm_x_wing(vNormX);
					pJg->set_norm_y_wing(vNormY);
				}
				pJg->ClearFlag();
				pJg->desStartPos.jgber_cal_style=pJg->desEndPos.jgber_cal_style=1;
				pJg->CalPosition();
			}
		}
	}
	//��ʼ���ҵ���Ϣ
	gxWirePointModel.m_bGimWireModelInherited=true;
	gxWirePointModel.m_hashConnPoints.Empty();
	gxWirePointModel.m_hashWirePoints.Empty();
	gxWirePointModel.m_listWireCircuits.Empty();
	for(HANG_NODE_INFO* pGuaInfo=m_listGuaNode.GetFirst();pGuaInfo;pGuaInfo=m_listGuaNode.GetNext())
	{
		CLDSNode xTmpNode;
		GEPOINT pos=pGuaInfo->pos;
		xTmpNode.SetPosition(ucs.TransPToCS(pos));
		CLDSNode* pNode=NULL;
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			GEPOINT dv=pNode->xOriginalPos-pos;
			if(fabs(dv.x)>1||fabs(dv.y)>1||fabs(dv.z)>1)
				continue;
			break;
		}
		if(pNode==NULL)
			pNode=&xTmpNode;
		/*if(pNode==NULL||pNode->m_cHangWireType=='C'||pNode->m_cHangWireType=='E'||pNode->m_cHangWireType=='J')
		{	//���������Ҳ����ڵ㣬���߽ڵ��Ѽ�¼�˹�����Ϣ����Ҫ�½�һ���ظ��Ľڵ�������¼�ù�����Ϣ
			pNode=Ta.Node.append();
			if(pGuaInfo->pos.y>0)
				pNode->SetLayer("TPH");
			else
				pNode->SetLayer("TPQ");
			pNode->SetPosition(pos);
			pNode->cfgword.Clear();
			for(pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
				pNode->cfgword.AddBodyLegs(pModule->GetBodyNo());
		}*/
		if(pNode)
		{
			if(pGuaInfo->m_ciType=='C')
				pNode->wireplace.ciWireType=pNode->m_cHangWireType='C';
			else if(pGuaInfo->m_ciType=='G')
				pNode->wireplace.ciWireType=pNode->m_cHangWireType='E';
			else if(pGuaInfo->m_ciType=='T')
				pNode->wireplace.ciWireType=pNode->m_cHangWireType='J';
			strcpy(pNode->m_sHangWireDesc,pGuaInfo->m_sName);
		}
		//�����ҵ���Ϣ
		CXhChar50 sGimName(pGuaInfo->m_sName);
		char* pszWireType=strstr(sGimName,"��");
		if(pszWireType==NULL)
			pszWireType=strstr(sGimName,"��");
		if(pszWireType==NULL)
			pszWireType=strstr(sGimName,"��");
		if(pszWireType!=NULL)
		{
			pszWireType+=2;
			char* pchSpliter=strchr(pszWireType,'_');
			if(pchSpliter==NULL)
				pchSpliter=strchr(pszWireType,'-');
			if(pchSpliter)
				*pchSpliter=0;
			int iPhaseSerial=atoi(pszWireType);
			if(pNode->wireplace.ciWireType!='E'&&iPhaseSerial>0)
			{
				iPhaseSerial-=1;
				pNode->wireplace.ciCircuitSerial=1+(iPhaseSerial/3); 
				pNode->wireplace.ciPhaseSerial=1+iPhaseSerial%3;
			}
			else
				pNode->wireplace.ciPhaseSerial=iPhaseSerial;
			pNode->wireplace.ciHangingStyle=0;
			pNode->wireplace.ciPostCode=pGuaInfo->m_xWirePlace.ciPostCode;
			if(pchSpliter && pGuaInfo->m_ciType!='T')
			{	//��ȡ�Ҵ�����
				pchSpliter++;
				if(*pchSpliter=='S'||*pchSpliter=='s')
					pNode->wireplace.ciHangingStyle='S';
				else if(*pchSpliter=='V'||*pchSpliter=='v')
					pNode->wireplace.ciHangingStyle='V';
			}
			else if(pchSpliter && pGuaInfo->m_ciType=='T')
			{	//��ȡ���߸�����
				pchSpliter++;
				pNode->wireplace.ciPostCode=atoi(pchSpliter);
			}
			if(pNode->wireplace.ciHangingStyle!=0)
			{	//��ȡ��|���߸�����
				pchSpliter++;
				pNode->wireplace.ciPostCode=atoi(pchSpliter);
			}
		}
		//���¹ҵ�ģ��
		CXhWirePoint* pWirePoint=gxWirePointModel.m_hashWirePoints.Append();
		pWirePoint->m_sGimName=pGuaInfo->m_sName;
		pWirePoint->xPosition=pNode->xOriginalPos;
		pWirePoint->m_xWirePlace=pNode->wireplace;
		pWirePoint->m_hRelaNode=pNode->handle;
	}
	//������ͼ
	theApp.GetLDSDoc()->UpdateAllViews(NULL);
	return TRUE;
}
void CModDataFile::WriteToFileByUtf8(FILE* fp)
{
	if(fp==NULL)
		return;
	GECS ucs=BuildUcsByModCS();
	GECS rot_cs = BuildRotateCS();
	CLDSNode* pNode=NULL;
	CLDSLinePart* pRod=NULL;
	//���������Ϣ
	fwprintf(fp,L"HNum,%d\n",m_listModuleItem.GetSize());
	for(MODULE_ITEM* pModuleItem=m_listModuleItem.GetFirst();pModuleItem;pModuleItem=m_listModuleItem.GetNext())
		fwprintf(fp,L"H,%.0f,Body%d,Leg%d\n",pModuleItem->m_fNamedHeight,pModuleItem->m_iBody,pModuleItem->m_iLeg);
	//����౾��������Ϣ
	int body_index=1,leg_index=1;
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		//������Ϣ
		fwprintf(fp,L"Body%d\n",body_index);
		fwprintf(fp,L"HBody%d,%.0f\n",body_index,pBodyItem->m_fBodyH);
		for(pNode=pBodyItem->bodyNodeSet.GetFirst();pNode;pNode=pBodyItem->bodyNodeSet.GetNext())
		{
			NODE_DATA* pNodeItem=m_hashNodeData.GetValue(pNode->handle);
			f3dPoint pos=pNode->Position();
			if (m_bTurnLeft)
				pos = rot_cs.TransPToCS(pos);
			pos=ucs.TransPToCS(pos);
			fwprintf(fp,L"P,%d,%f,%f,%f\n",pNodeItem->iPointI,pos.x,pos.y,pos.z);
		}
		for(pRod=pBodyItem->bodyRodSet.GetFirst();pRod;pRod=pBodyItem->bodyRodSet.GetNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				continue;
			ROD_DATA* pRodItem=m_hashRodData.GetValue(pRod->handle);
			if(pRod->IsAngle())
			{
				CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
				wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
				CXhChar16 sMat,sSpec;
				QuerySteelMatMark(pJg->cMaterial,sMat);
				sprintf(sSpec,"L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
				//pJg->GetSpecification(sSpec,FALSE);	//����ָ���ֻ��'X',����'x'
				ANSIToUnicode(sMat,sWMat);
				ANSIToUnicode(sSpec,sWSpec);
				f3dPoint xWingVec=pJg->GetWingVecX();
				f3dPoint ywingVec=pJg->GetWingVecY();
				if (m_bTurnLeft)
					xWingVec = rot_cs.TransVToCS(xWingVec);
				xWingVec = ucs.TransVToCS(xWingVec);
				if (m_bTurnLeft)
					ywingVec = rot_cs.TransVToCS(ywingVec);
				ywingVec=ucs.TransVToCS(ywingVec);
				fwprintf(fp,L"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",pRodItem->iPointS,pRodItem->iPointE,sWSpec,sWMat,
					xWingVec.x,xWingVec.y,xWingVec.z,ywingVec.x,ywingVec.y,ywingVec.z);
			}
			else if(pRod->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube* pTube=(CLDSLineTube*)pRod;
				wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
				CXhChar16 sMat,sSpec;
				QuerySteelMatMark(pTube->cMaterial,sMat);
				//pTube->GetSpecification(sSpec,FALSE);
				sprintf(sSpec,"��%.0fX%.0f",pTube->GetDiameter(),pTube->GetThick());
				ANSIToUnicode(sMat,sWMat);
				ANSIToUnicode(sSpec,sWSpec);
				fwprintf(fp,L"R,%d,%d,%s,%s\n",pRodItem->iPointS,pRodItem->iPointE,sWSpec,sWMat);
			}
#ifdef __GGG_
			else if(pRod->GetClassTypeId()==CLS_CONETUBE)
			{
				CLDSConeTube* pConeTube=(CLDSConeTube*)pRod;
				wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
				CXhChar16 sMat,sSpec;
				QuerySteelMatMark(pConeTube->cMaterial,sMat);
				ANSIToUnicode(sMat,sWMat);
				ANSIToUnicode(sSpec,sWSpec);
				double fDiaS=pConeTube->size_wide*2;
				double pDiaE=pConeTube->size_wideUpSide*2;
				fwprintf(fp,L"R,%d,%d,%s,%s,%.0f,%.0f,%.0f,%d\n",pRodItem->iPointS,pRodItem->iPointE,sWSpec,sWMat,fDiaS,pDiaE,
					pConeTube->size_thick,pConeTube->faceN);
			}
#endif
			else
				fwprintf(fp,L"R,%d,%d\n",pRodItem->iPointS,pRodItem->iPointE);
		}
		//�ñ����µĽ�����Ϣ
		for(CLegGroupItem* pLegItem=m_listHeightItem.GetFirst();pLegItem;pLegItem=m_listHeightItem.GetNext())
		{
			if(!pBodyItem->IsIncludeLegItem(pLegItem))//pLegItem->m_hTagNode!=pBodyItem->m_hTagNode)
				continue;
			fwprintf(fp,L"Leg%d\n",leg_index);
			fwprintf(fp,L"HLeg%d,%.0f,%.0f\n",leg_index,pLegItem->m_fExclusiveBodyH,pLegItem->m_fMaxLegH);
			for(pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
			{
				NODE_DATA* pNodeItem=m_hashNodeData.GetValue(pNode->handle);
				f3dPoint pos=pNode->Position();
				if (m_bTurnLeft)
					pos = rot_cs.TransPToCS(pos);
				pos=ucs.TransPToCS(pos);
				fwprintf(fp,L"P,%d,%f,%f,%f\n",pNodeItem->iPointI,pos.x,pos.y,pos.z);
			}
			for(CLegGroupItem::NODEREF_OBJ* pRedundnatNode=pLegItem->listRedundantNodes.EnumObjectFirst();pRedundnatNode;pRedundnatNode=pLegItem->listRedundantNodes.EnumObjectNext())
			{
				f3dPoint pos=pRedundnatNode->pos;
				if (m_bTurnLeft)
					pos = rot_cs.TransPToCS(pos);
				pos = ucs.TransPToCS(pos);
				fwprintf(fp,L"P,%d,%f,%f,%f\n",pRedundnatNode->iPointI,pos.x,pos.y,pos.z);
			}
			for(pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
					continue;
				ROD_DATA* pRodItem=m_hashRodData.GetValue(pRod->handle);
				int iPointS=pRodItem->iPointS;
				int iPointE=pRodItem->iPointE;
				NODE_DATA* pNodeItemS=m_hashNodeData.GetValue(pRod->pStart->handle);
				NODE_DATA* pNodeItemE=m_hashNodeData.GetValue(pRod->pEnd->handle);
				if(pNodeItemS->ciLevel==0&&pNodeItemE->ciLevel==0)
				{	//�˼���ʼ�ն˵㶼��body�����¸ø˼�����������ظ����֣�������д�ϴ���
					CLDSNode* pMidNode=pLegItem->FindMidNode(pRod);
					if(pMidNode)
					{
						NODE_DATA* pNodeItemM=m_hashNodeData.GetValue(pMidNode->handle);
						iPointE=pNodeItemM->iPointI;
						if(pRod->IsAngle())
						{
							CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
							wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
							CXhChar16 sMat,sSpec;
							QuerySteelMatMark(pJg->cMaterial,sMat);
							sprintf(sSpec,"L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
							ANSIToUnicode(sMat,sWMat);
							ANSIToUnicode(sSpec,sWSpec);
							f3dPoint xWingVec=pJg->GetWingVecX();
							f3dPoint ywingVec=pJg->GetWingVecY();
							if (m_bTurnLeft)
								xWingVec = rot_cs.TransVToCS(xWingVec);
							xWingVec = ucs.TransVToCS(xWingVec);
							if (m_bTurnLeft)
								ywingVec = rot_cs.TransVToCS(ywingVec);
							ywingVec=ucs.TransVToCS(ywingVec);
							fwprintf(fp,L"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",iPointS,iPointE,sWSpec,sWMat,
								xWingVec.x,xWingVec.y,xWingVec.z,ywingVec.x,ywingVec.y,ywingVec.z);
						}
						else if(pRod->GetClassTypeId()==CLS_LINETUBE)
						{
							CLDSLineTube* pTube=(CLDSLineTube*)pRod;
							wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
							CXhChar16 sMat,sSpec;
							QuerySteelMatMark(pTube->cMaterial,sMat);
							//pTube->GetSpecification(sSpec,FALSE);
							sprintf(sSpec,"��%.0fX%.0f",pTube->GetDiameter(),pTube->GetThick());
							ANSIToUnicode(sMat,sWMat);
							ANSIToUnicode(sSpec,sWSpec);
							fwprintf(fp,L"R,%d,%d,%s,%s\n",iPointS,iPointE,sWSpec,sWMat);
						}
#ifdef __GGG_
						else if(pRod->GetClassTypeId()==CLS_CONETUBE)
						{
							CLDSConeTube* pConeTube=(CLDSConeTube*)pRod;
							wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
							CXhChar16 sMat,sSpec;
							QuerySteelMatMark(pConeTube->cMaterial,sMat);
							ANSIToUnicode(sMat,sWMat);
							ANSIToUnicode(sSpec,sWSpec);
							double fDiaS=pConeTube->size_wide*2;
							double pDiaE=pConeTube->size_wideUpSide*2;
							fwprintf(fp,L"R,%d,%d,%s,%s,%.0f,%.0f,%.0f,%d\n",pRodItem->iPointS,pRodItem->iPointE,sWSpec,sWMat,fDiaS,pDiaE,
								pConeTube->size_thick,pConeTube->faceN);
						}
#endif
						else
							fwprintf(fp,L"R,%d,%d\n",iPointS,iPointE);
						//
						iPointS=pRodItem->iPointE;
					}
				}
				if(pRod->IsAngle())
				{
					CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
					wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
					CXhChar16 sMat,sSpec;
					QuerySteelMatMark(pJg->cMaterial,sMat);
					sprintf(sSpec,"L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
					ANSIToUnicode(sMat,sWMat);
					ANSIToUnicode(sSpec,sWSpec);
					f3dPoint xWingVec=pJg->GetWingVecX();
					f3dPoint ywingVec=pJg->GetWingVecY();
					if (m_bTurnLeft)
						xWingVec = rot_cs.TransVToCS(xWingVec);
					xWingVec = ucs.TransVToCS(xWingVec);
					if(m_bTurnLeft)
						ywingVec = rot_cs.TransVToCS(ywingVec);
					ywingVec=ucs.TransVToCS(ywingVec);
					fwprintf(fp,L"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",iPointS,iPointE,sWSpec,sWMat,
						xWingVec.x,xWingVec.y,xWingVec.z,ywingVec.x,ywingVec.y,ywingVec.z);
				}
				else if(pRod->GetClassTypeId()==CLS_LINETUBE)
				{
					CLDSLineTube* pTube=(CLDSLineTube*)pRod;
					wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
					CXhChar16 sMat,sSpec;
					QuerySteelMatMark(pTube->cMaterial,sMat);
					//pTube->GetSpecification(sSpec,FALSE);
					sprintf(sSpec,"��%.0fX%.0f",pTube->GetDiameter(),pTube->GetThick());
					ANSIToUnicode(sMat,sWMat);
					ANSIToUnicode(sSpec,sWSpec);
					fwprintf(fp,L"R,%d,%d,%s,%s\n",iPointS,iPointE,sWSpec,sWMat);
				}
#ifdef __GGG_
				else if(pRod->GetClassTypeId()==CLS_CONETUBE)
				{
					CLDSConeTube* pConeTube=(CLDSConeTube*)pRod;
					wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
					CXhChar16 sMat,sSpec;
					QuerySteelMatMark(pConeTube->cMaterial,sMat);
					ANSIToUnicode(sMat,sWMat);
					ANSIToUnicode(sSpec,sWSpec);
					double fDiaS=pConeTube->size_wide*2;
					double pDiaE=pConeTube->size_wideUpSide*2;
					fwprintf(fp,L"R,%d,%d,%s,%s,%.0f,%.0f,%.0f,%d\n",pRodItem->iPointS,pRodItem->iPointE,sWSpec,sWMat,fDiaS,pDiaE,
						pConeTube->size_thick,pConeTube->faceN);
				}
#endif
				else
					fwprintf(fp,L"R,%d,%d\n",iPointS,iPointE);
			}
			//��������Ϣ
			int sub_leg_index=1;
			for(SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.GetFirst();pSubLeg;pSubLeg=pLegItem->subLegInfoList.GetNext())
			{
				fwprintf(fp,L"SubLeg%d\n",sub_leg_index);
				fwprintf(fp,L"HSubLeg%d,%.0f\n",sub_leg_index,pSubLeg->m_fLegH);
				for(pNode=pSubLeg->legNodeSet.GetFirst();pNode;pNode=pSubLeg->legNodeSet.GetNext())
				{
					NODE_DATA* pNodeItem=m_hashNodeData.GetValue(pNode->handle);
					f3dPoint pos=pNode->Position();
					if (m_bTurnLeft)
						pos = rot_cs.TransPToCS(pos);
					pos=ucs.TransPToCS(pos);
					fwprintf(fp,L"P,%d,%f,%f,%f\n",pNodeItem->iPointI,pos.x,pos.y,pos.z);
				}
				for(pRod=pSubLeg->legRodSet.GetFirst();pRod;pRod=pSubLeg->legRodSet.GetNext())
				{
					if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
						continue;
					ROD_DATA* pRodItem=m_hashRodData.GetValue(pRod->handle);
					if(pRod->IsAngle())
					{
						CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
						wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
						CXhChar16 sMat,sSpec;
						QuerySteelMatMark(pJg->cMaterial,sMat);
						sprintf(sSpec,"L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
						//pJg->GetSpecification(sSpec,FALSE);
						ANSIToUnicode(sMat,sWMat);
						ANSIToUnicode(sSpec,sWSpec);
						f3dPoint xWingVec=pJg->GetWingVecX();
						f3dPoint ywingVec=pJg->GetWingVecY();
						if(m_bTurnLeft)
							xWingVec = rot_cs.TransVToCS(xWingVec);
						xWingVec=ucs.TransVToCS(xWingVec);
						if(m_bTurnLeft)
							ywingVec = rot_cs.TransVToCS(ywingVec);
						ywingVec=ucs.TransVToCS(ywingVec);
						fwprintf(fp,L"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",pRodItem->iPointS,pRodItem->iPointE,sWSpec,sWMat,
							xWingVec.x,xWingVec.y,xWingVec.z,ywingVec.x,ywingVec.y,ywingVec.z);
					}
					else if(pRod->GetClassTypeId()==CLS_LINETUBE)
					{
						CLDSLineTube* pTube=(CLDSLineTube*)pRod;
						wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
						CXhChar16 sMat,sSpec;
						QuerySteelMatMark(pTube->cMaterial,sMat);
						//pTube->GetSpecification(sSpec,FALSE);
						sprintf(sSpec,"��%.0fX%.0f",pTube->GetDiameter(),pTube->GetThick());
						ANSIToUnicode(sMat,sWMat);
						ANSIToUnicode(sSpec,sWSpec);
						fwprintf(fp,L"R,%d,%d,%s,%s\n",pRodItem->iPointS,pRodItem->iPointE,sWSpec,sWMat);
					}
#ifdef __GGG_
					else if(pRod->GetClassTypeId()==CLS_CONETUBE)
					{
						CLDSConeTube* pConeTube=(CLDSConeTube*)pRod;
						wchar_t sWMat[MAX_PATH],sWSpec[MAX_PATH];
						CXhChar16 sMat,sSpec;
						QuerySteelMatMark(pConeTube->cMaterial,sMat);
						ANSIToUnicode(sMat,sWMat);
						ANSIToUnicode(sSpec,sWSpec);
						double fDiaS=pConeTube->size_wide*2;
						double pDiaE=pConeTube->size_wideUpSide*2;
						fwprintf(fp,L"R,%d,%d,%s,%s,%.0f,%.0f,%.0f,%d\n",pRodItem->iPointS,pRodItem->iPointE,sWSpec,sWMat,fDiaS,pDiaE,
							pConeTube->size_thick,pConeTube->faceN);
					}
#endif
					else
						fwprintf(fp,L"R,%d,%d\n",pRodItem->iPointS,pRodItem->iPointE);
				}
				sub_leg_index++;
			}
			leg_index++;
		}
		body_index++;
	}
	//����ҵ���Ϣ
	for(HANG_NODE_INFO* pGuaInfo=m_listGuaNode.GetFirst();pGuaInfo;pGuaInfo=m_listGuaNode.GetNext())
	{
		f3dPoint pos=pGuaInfo->pos;
		if(m_bTurnLeft)
			pos = rot_cs.TransPToCS(pos);
		pos=ucs.TransPToCS(pos);
		wchar_t sWText[MAX_PATH];
		ANSIToUnicode(pGuaInfo->m_sName,sWText);
		fwprintf(fp,L"G,%c,%s,%f,%f,%f\n",pGuaInfo->m_ciType,sWText,pos.x,pos.y,pos.z);
	}
	fclose(fp);
}
void CModDataFile::WriteToFileByAnsi(FILE* fp)
{
	if(fp==NULL)
		return;
	GECS ucs=BuildUcsByModCS();
	GECS rot_cs = BuildRotateCS();
	CLDSNode* pNode=NULL;
	CLDSLinePart* pRod=NULL;
	//���������Ϣ
	fprintf(fp,"HNum,%d\n",m_listModuleItem.GetSize());
	for(MODULE_ITEM* pModuleItem=m_listModuleItem.GetFirst();pModuleItem;pModuleItem=m_listModuleItem.GetNext())
		fprintf(fp,"H,%.2f,Body%d,Leg%d\n",pModuleItem->m_fNamedHeight,pModuleItem->m_iBody,pModuleItem->m_iLeg);
	//����౾��������Ϣ
	int body_index=1,leg_index=1;
	for(CBodyItem* pBodyItem=m_listBodyItem.GetFirst();pBodyItem;pBodyItem=m_listBodyItem.GetNext())
	{
		//������Ϣ
		fprintf(fp,"Body%d\n",body_index);
		fprintf(fp,"HBody%d,%s\n",body_index,(char*)CXhChar16(pBodyItem->m_fBodyH));
		for(pNode=pBodyItem->bodyNodeSet.GetFirst();pNode;pNode=pBodyItem->bodyNodeSet.GetNext())
		{
			NODE_DATA* pNodeItem=m_hashNodeData.GetValue(pNode->handle);
			f3dPoint pos=pNode->Position();
			if(m_bTurnLeft)
				pos = ucs.TransPToCS(pos);
			pos=ucs.TransPToCS(pos);
			fprintf(fp,"P,%d,%f,%f,%f\n",pNodeItem->iPointI,pos.x,pos.y,pos.z);
		}
		for(pRod=pBodyItem->bodyRodSet.GetFirst();pRod;pRod=pBodyItem->bodyRodSet.GetNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				continue;
			ROD_DATA* pRodItem=m_hashRodData.GetValue(pRod->handle);
			if(pRod->IsAngle())
			{
				CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
				CXhChar16 sMat,sSpec;
				QuerySteelMatMark(pJg->cMaterial,sMat);
				sprintf(sSpec,"L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
				f3dPoint xWingVec=pJg->GetWingVecX();
				f3dPoint ywingVec=pJg->GetWingVecY();
				if (m_bTurnLeft)
					xWingVec = rot_cs.TransVToCS(xWingVec);
				xWingVec=ucs.TransVToCS(xWingVec);
				if(m_bTurnLeft)
					ywingVec = rot_cs.TransVToCS(ywingVec);
				ywingVec=ucs.TransVToCS(ywingVec);
				fprintf(fp,"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",pRodItem->iPointS,pRodItem->iPointE,(char*)sSpec,(char*)sMat,
					xWingVec.x,xWingVec.y,xWingVec.z,ywingVec.x,ywingVec.y,ywingVec.z);
			}
			else if(pRod->GetClassTypeId()==CLS_LINETUBE)
			{
				CLDSLineTube* pTube=(CLDSLineTube*)pRod;
				CXhChar16 sMat,sSpec;
				QuerySteelMatMark(pTube->cMaterial,sMat);
				sprintf(sSpec,"��%.0fX%.0f",pTube->GetDiameter(),pTube->GetThick());
				fprintf(fp,"R,%d,%d,%s,%s\n",pRodItem->iPointS,pRodItem->iPointE,(char*)sSpec,(char*)sMat);
			}
#ifdef __GGG_
			else if(pRod->GetClassTypeId()==CLS_CONETUBE)
			{
				CLDSConeTube* pConeTube=(CLDSConeTube*)pRod;
				CXhChar16 sMat;
				QuerySteelMatMark(pConeTube->cMaterial,sMat);
				double fDiaS=pConeTube->size_wide*2;
				double pDiaE=pConeTube->size_wideUpSide*2;
				fprintf(fp,"R,%d,%d," ",%s,%.0f,%.0f,%.0f,%d\n",pRodItem->iPointS,pRodItem->iPointE,sMat,fDiaS,pDiaE,
					pConeTube->size_thick,pConeTube->faceN);
			}
#endif
			else
				fprintf(fp,"R,%d,%d\n",pRodItem->iPointS,pRodItem->iPointE);
		}
		//�ñ����µĽ�����Ϣ
		for(CLegGroupItem* pLegItem=m_listHeightItem.GetFirst();pLegItem;pLegItem=m_listHeightItem.GetNext())
		{
			if(!pBodyItem->IsIncludeLegItem(pLegItem))//pLegItem->m_hTagNode!=pBodyItem->m_hTagNode)
				continue;
			fprintf(fp,"Leg%d\n",leg_index);
			fprintf(fp,"HLeg%d,%s,%s\n",leg_index,(char*)CXhChar16(pLegItem->m_fExclusiveBodyH),(char*)CXhChar16(pLegItem->m_fMaxLegH));
			for(pNode=pLegItem->segmentNodeSet.GetFirst();pNode;pNode=pLegItem->segmentNodeSet.GetNext())
			{
				NODE_DATA* pNodeItem=m_hashNodeData.GetValue(pNode->handle);
				f3dPoint pos=pNode->Position();
				if (m_bTurnLeft)
					pos = rot_cs.TransPToCS(pos);
				pos=ucs.TransPToCS(pos);
				fprintf(fp,"P,%d,%f,%f,%f\n",pNodeItem->iPointI,pos.x,pos.y,pos.z);
			}
			for(CLegGroupItem::NODEREF_OBJ* pRedundnatNode=pLegItem->listRedundantNodes.EnumObjectFirst();pRedundnatNode;pRedundnatNode=pLegItem->listRedundantNodes.EnumObjectNext())
			{
				f3dPoint pos=pRedundnatNode->pos;
				if (m_bTurnLeft)
					pos = rot_cs.TransPToCS(pos);
				pos = ucs.TransPToCS(pos);
				fprintf(fp,"P,%d,%f,%f,%f\n",pRedundnatNode->iPointI,pos.x,pos.y,pos.z);
			}
			for(pRod=pLegItem->segmentRodSet.GetFirst();pRod;pRod=pLegItem->segmentRodSet.GetNext())
			{
				if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
					continue;
				ROD_DATA* pRodItem=m_hashRodData.GetValue(pRod->handle);
				int iPointS=pRodItem->iPointS;
				int iPointE=pRodItem->iPointE;
				NODE_DATA* pNodeItemS=m_hashNodeData.GetValue(pRod->pStart->handle);
				NODE_DATA* pNodeItemE=m_hashNodeData.GetValue(pRod->pEnd->handle);
				if(pNodeItemS->ciLevel==0&&pNodeItemE->ciLevel==0)
				{	//�˼���ʼ�ն˵㶼��body�����¸ø˼�����������ظ����֣�������д�ϴ���
					CLDSNode* pMidNode=pLegItem->FindMidNode(pRod);
					if(pMidNode)
					{
						NODE_DATA* pNodeItemM=m_hashNodeData.GetValue(pMidNode->handle);
						iPointE=pNodeItemM->iPointI;
						if(pRod->IsAngle())
						{
							CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
							CXhChar16 sMat,sSpec;
							QuerySteelMatMark(pJg->cMaterial,sMat);
							sprintf(sSpec,"L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
							//pJg->GetSpecification(sSpec,FALSE);
							f3dPoint xWingVec=pJg->GetWingVecX();
							f3dPoint ywingVec=pJg->GetWingVecY();
							if(m_bTurnLeft)
								xWingVec = rot_cs.TransVToCS(xWingVec);
							xWingVec=ucs.TransVToCS(xWingVec);
							if(m_bTurnLeft)
								ywingVec = rot_cs.TransVToCS(ywingVec);
							ywingVec=ucs.TransVToCS(ywingVec);
							fprintf(fp,"R,%d,%d,%s,%s,%3f,%f,%f,%f,%f,%f\n",iPointS,iPointE,(char*)sSpec,(char*)sMat,
								xWingVec.x,xWingVec.y,xWingVec.z,ywingVec.x,ywingVec.y,ywingVec.z);
						}
						else if(pRod->GetClassTypeId()==CLS_LINETUBE)
						{
							CLDSLineTube* pTube=(CLDSLineTube*)pRod;
							CXhChar16 sMat,sSpec;
							QuerySteelMatMark(pTube->cMaterial,sMat);
							sprintf(sSpec,"��%.0fX%.0f",pTube->GetDiameter(),pTube->GetThick());
							fprintf(fp,"R,%d,%d,%s,%s\n",iPointS,iPointE,(char*)sSpec,(char*)sMat);
						}
#ifdef __GGG_
						else if(pRod->GetClassTypeId()==CLS_CONETUBE)
						{
							CLDSConeTube* pConeTube=(CLDSConeTube*)pRod;
							CXhChar16 sMat;
							QuerySteelMatMark(pConeTube->cMaterial,sMat);
							double fDiaS=pConeTube->size_wide*2;
							double pDiaE=pConeTube->size_wideUpSide*2;
							fprintf(fp,"R,%d,%d," ",%s,%.0f,%.0f,%.0f,%d\n",pRodItem->iPointS,pRodItem->iPointE,sMat,fDiaS,pDiaE,
								pConeTube->size_thick,pConeTube->faceN);
						}
#endif
						else
							fprintf(fp,"R,%d,%d\n",iPointS,iPointE);
						//
						iPointS=pRodItem->iPointE;
					}
				}
				if(pRod->IsAngle())
				{
					CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
					CXhChar16 sMat,sSpec;
					QuerySteelMatMark(pJg->cMaterial,sMat);
					sprintf(sSpec,"L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
					//pJg->GetSpecification(sSpec,FALSE);
					f3dPoint xWingVec=pJg->GetWingVecX();
					f3dPoint ywingVec=pJg->GetWingVecY();
					if(m_bTurnLeft)
						xWingVec = rot_cs.TransVToCS(xWingVec);
					xWingVec=ucs.TransVToCS(xWingVec);
					if(m_bTurnLeft)
						ywingVec = rot_cs.TransVToCS(ywingVec);
					ywingVec=ucs.TransVToCS(ywingVec);
					fprintf(fp,"R,%d,%d,%s,%s,%3f,%f,%f,%f,%f,%f\n",iPointS,iPointE,(char*)sSpec,(char*)sMat,
						xWingVec.x,xWingVec.y,xWingVec.z,ywingVec.x,ywingVec.y,ywingVec.z);
				}
				else if(pRod->GetClassTypeId()==CLS_LINETUBE)
				{
					CLDSLineTube* pTube=(CLDSLineTube*)pRod;
					CXhChar16 sMat,sSpec;
					QuerySteelMatMark(pTube->cMaterial,sMat);
					sprintf(sSpec,"��%.0fX%.0f",pTube->GetDiameter(),pTube->GetThick());
					fprintf(fp,"R,%d,%d,%s,%s\n",iPointS,iPointE,(char*)sSpec,(char*)sMat);
				}
#ifdef __GGG_
				else if(pRod->GetClassTypeId()==CLS_CONETUBE)
				{
					CLDSConeTube* pConeTube=(CLDSConeTube*)pRod;
					CXhChar16 sMat;
					QuerySteelMatMark(pConeTube->cMaterial,sMat);
					double fDiaS=pConeTube->size_wide*2;
					double pDiaE=pConeTube->size_wideUpSide*2;
					fprintf(fp,"R,%d,%d," ",%s,%.0f,%.0f,%.0f,%d\n",pRodItem->iPointS,pRodItem->iPointE,sMat,fDiaS,pDiaE,
						pConeTube->size_thick,pConeTube->faceN);
				}
#endif
				else
					fprintf(fp,"R,%d,%d\n",iPointS,iPointE);
			}
			//��������Ϣ
			int sub_leg_index=1;
			for(SUB_LEG_INFO* pSubLeg=pLegItem->subLegInfoList.GetFirst();pSubLeg;pSubLeg=pLegItem->subLegInfoList.GetNext())
			{
				fprintf(fp,"SubLeg%d\n",sub_leg_index);
				fprintf(fp,"HSubLeg%d,%s\n",sub_leg_index,(char*)CXhChar16(pSubLeg->m_fLegH));
				for(pNode=pSubLeg->legNodeSet.GetFirst();pNode;pNode=pSubLeg->legNodeSet.GetNext())
				{
					NODE_DATA* pNodeItem=m_hashNodeData.GetValue(pNode->handle);
					f3dPoint pos=pNode->Position();
					if (m_bTurnLeft)
						pos = rot_cs.TransPToCS(pos);
					pos=ucs.TransPToCS(pos);
					fprintf(fp,"P,%d,%f,%f,%f\n",pNodeItem->iPointI,pos.x,pos.y,pos.z);
				}
				for(pRod=pSubLeg->legRodSet.GetFirst();pRod;pRod=pSubLeg->legRodSet.GetNext())
				{
					if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
						continue;
					ROD_DATA* pRodItem=m_hashRodData.GetValue(pRod->handle);
					if(pRod->IsAngle())
					{
						CLDSLineAngle* pJg=(CLDSLineAngle*)pRod;
						CXhChar16 sMat,sSpec;
						QuerySteelMatMark(pJg->cMaterial,sMat);
						sprintf(sSpec,"L%.0fX%.0f",pJg->GetWidth(),pJg->GetThick());
						//pJg->GetSpecification(sSpec,FALSE);
						f3dPoint xWingVec=pJg->GetWingVecX();
						f3dPoint ywingVec=pJg->GetWingVecY();
						if(m_bTurnLeft)
							xWingVec = rot_cs.TransVToCS(xWingVec);
						xWingVec=ucs.TransVToCS(xWingVec);
						if(m_bTurnLeft)
							ywingVec = rot_cs.TransVToCS(ywingVec);
						ywingVec=ucs.TransVToCS(ywingVec);
						fprintf(fp,"R,%d,%d,%s,%s,%f,%f,%f,%f,%f,%f\n",pRodItem->iPointS,pRodItem->iPointE,(char*)sSpec,(char*)sMat,
							xWingVec.x,xWingVec.y,xWingVec.z,ywingVec.x,ywingVec.y,ywingVec.z);
					}
					else if(pRod->GetClassTypeId()==CLS_LINETUBE)
					{
						CLDSLineTube* pTube=(CLDSLineTube*)pRod;
						CXhChar16 sMat,sSpec;
						QuerySteelMatMark(pTube->cMaterial,sMat);
						sprintf(sSpec,"��%.0fX%.0f",pTube->GetDiameter(),pTube->GetThick());
						fprintf(fp,"R,%d,%d,%s,%s\n",pRodItem->iPointS,pRodItem->iPointE,(char*)sSpec,(char*)sMat);
					}
#ifdef __GGG_
					else if(pRod->GetClassTypeId()==CLS_CONETUBE)
					{
						CLDSConeTube* pConeTube=(CLDSConeTube*)pRod;
						CXhChar16 sMat;
						QuerySteelMatMark(pConeTube->cMaterial,sMat);
						double fDiaS=pConeTube->size_wide*2;
						double pDiaE=pConeTube->size_wideUpSide*2;
						fprintf(fp,"R,%d,%d," ",%s,%.0f,%.0f,%.0f,%d\n",pRodItem->iPointS,pRodItem->iPointE,sMat,fDiaS,pDiaE,
							pConeTube->size_thick,pConeTube->faceN);
					}
#endif
					else
						fprintf(fp,"R,%d,%d\n",pRodItem->iPointS,pRodItem->iPointE);
				}
				sub_leg_index++;
			}
			leg_index++;
		}
		body_index++;
	}
	//����ҵ���Ϣ
	for(HANG_NODE_INFO* pGuaInfo=m_listGuaNode.GetFirst();pGuaInfo;pGuaInfo=m_listGuaNode.GetNext())
	{
		f3dPoint pos=pGuaInfo->pos;
		if(m_bTurnLeft)
			pos = rot_cs.TransPToCS(pos);
		pos=ucs.TransPToCS(pos);
		fprintf(fp,"G,%c,%s,%f,%f,%f\n",pGuaInfo->m_ciType,pGuaInfo->m_sName,pos.x,pos.y,pos.z);
	}
	fclose(fp);
}
void CModDataFile::ReadModFile(FILE* fp,BOOL bUtf8/*=FALSE*/)
{
	if(fp==NULL)
		return;
	CLogErrorLife logErrLife;
	Ta.Empty();
	Ta.InitTower();
	console.InitTowerModel();
	console.DispNodeSet.Empty();
	console.DispPartSet.Empty();
	console.DispBlockRefSet.Empty();
	console.DispFoundationSet.Empty();
	g_pSolidDraw->BuildDisplayList(NULL);
	m_fTowerHeight=0;
	int iLegNo=0;
	CHashList<long> hashNodeByIndex;
	BYTE ciReadType=0;	//0:����|1.����|2.����
	CBodyItem* pBodyItem=NULL;
	CLegGroupItem* pLegItem=NULL;
	SUB_LEG_INFO* pSubLeg=NULL;
	CXhChar100 line_txt,sText,key_word;
	char sLine1[MAX_PATH]="",sLine2[MAX_PATH]="";
	CHashStrList<HEIGHT_HEADER> hashHeightNames;
	int lineindex=0;
	while(!feof(fp))
	{
		lineindex++;
		if(fgets(sLine1,MAX_PATH,fp)==NULL)
			break;
		if(bUtf8)
			UTF8ToANSI(sLine1,sLine2);
		else
			strcpy(sLine2,sLine1);
		line_txt.Copy(sLine2);
		line_txt.Replace('\t',' ');
		line_txt.Remove(' ');
		strcpy(sText,line_txt);
		char *skey=strtok(sText,",");
		strncpy(key_word,skey,100);
		if(strstr(key_word,"HBody")||strstr(key_word,"HLeg")||strstr(key_word,"HSubLeg")||strstr(key_word,"HNum"))
			continue;
		HEIGHT_HEADER* pNamedHeight;
		if(stricmp(key_word,"H")==0)
		{
			skey=strtok(NULL,",");
			double height=skey!=NULL?atof(skey):0;
			UINT idBody=0;
			if(skey!=NULL)	//�����Խӱ���
				skey=strtok(NULL,",");
			if(skey!=NULL)	//�����Խӽ��ȣ������飩
			{
				CXhChar50 itemstr;
				itemstr.NCopy(&skey,4,true);
				if(itemstr.EqualNoCase("Body"))
					idBody=atoi(skey);
				skey=strtok(NULL,",");
				if(skey!=NULL)
				{
					pNamedHeight=hashHeightNames.Add(skey);
					pNamedHeight->fNamedHeight=(float)height;
					pNamedHeight->idJointShareBody=idBody;
				}
			}
			continue;
		}
		else if(strstr(key_word,"Body"))
		{	//����
			ciReadType=0;
			pBodyItem=m_listBodyItem.append();
		}
		else if(strstr(key_word,"Leg")&&key_word[0]=='L')
		{	//����
			ciReadType=1;
			iLegNo+=1;
			//MODULE_ITEMʵ������MOD�ļ��е�LEG_ITEMһһ��Ӧ wjh-2018.6.11
			MODULE_ITEM* pModuleItem=m_listModuleItem.append();
			pLegItem=m_listHeightItem.append();
			pModuleItem->m_iLeg=pLegItem->m_idHeight=iLegNo;
			if((pNamedHeight=hashHeightNames.GetValue(key_word))!=NULL)
			{
				pModuleItem->m_fNamedHeight=pNamedHeight->fNamedHeight;
				pModuleItem->m_iBody=pNamedHeight->idJointShareBody;
			}
			if(pBodyItem->m_iLegS==0)
				pBodyItem->m_iLegS=iLegNo;
		}
		else if(strstr(key_word,"SubLeg"))
		{	//����
			ciReadType=2;
			if(pLegItem!=NULL)
				pSubLeg=pLegItem->subLegInfoList.append();
			else
				AfxMessageBox(CXhChar50("Line%d#�ӽ���SubLeg��������Leg֮��",lineindex));
		}
		else
		{
			if(stricmp(key_word,"G")!=0)
				line_txt.Replace(',',' ');
			key_word.Remove(' ');
			if(stricmp(key_word,"P")==0)
			{	//�ڵ�
				f3dPoint pos;
				sscanf(line_txt,"%s%d%lf%lf%lf",(char*)key_word,&pos.feature,&pos.x,&pos.y,&pos.z);
				if(pos.z>m_fTowerHeight)
					m_fTowerHeight=pos.z;
				CLDSNode* pNode=Ta.AppendNode();
				pNode->point_i=pos.feature;
				pNode->SetPosition(pos);
				hashNodeByIndex.SetValue(pNode->point_i,pNode->handle);
				if(ciReadType==0 && pBodyItem)
					pBodyItem->bodyNodeSet.append(pNode);
				else if(ciReadType==1 && pLegItem)
					pLegItem->segmentNodeSet.append(pNode);
				else if(ciReadType==2 && pSubLeg)
					pSubLeg->legNodeSet.append(pNode);
			}
			else if(stricmp(key_word,"R")==0)
			{	//�˼�
				int indexS=0,indexE=0;
				double fWidth=0,fThick=0;
				char sMat[16]="",sSpec[16]="";
				CXhChar100 sTempLine(line_txt);
				sscanf(sTempLine,"%s%d%d%s%s",(char*)key_word,&indexS,&indexE,sSpec,sMat);
				long* hStart=hashNodeByIndex.GetValue(indexS);
				long* hEnd=hashNodeByIndex.GetValue(indexE);
				if(hStart==NULL || hEnd==NULL)
				{
					logerr.Log("Mod�˼���������(ʼ�˵��%d,�ն˵��%d)!",indexS,indexE);
					continue;
				}
				CLDSNode* pNodeS=Ta.FromNodeHandle(*hStart);
				CLDSNode* pNodeE=Ta.FromNodeHandle(*hEnd);
				if(pNodeS==NULL || pNodeE==NULL)
				{
					logerr.Log("Mod�˼���������(ʼ�˵��%d,�ն˵��%d)!",indexS,indexE);
					continue;
				}
				if(strstr(sSpec,"L"))
				{	//�Ǹ�
					f3dPoint ptX,ptY;
					sscanf(line_txt,"%s%d%d%s%s%lf%lf%lf%lf%lf%lf",(char*)key_word,&indexS,&indexE,sSpec,sMat,&ptX.x,&ptX.y,&ptX.z,&ptY.x,&ptY.y,&ptY.z);
					char mark,symbol;
					sscanf(sSpec,"%c%lf%c%lf",&symbol,&fWidth,&mark,&fThick);
					CLDSLineAngle* pJg=(CLDSLineAngle*)Ta.AppendPart(CLS_LINEANGLE);
					pJg->layer(1)='X';
					pJg->pStart=pNodeS;
					pJg->pEnd=pNodeE;
					pJg->SetStart(pNodeS->xOriginalPos);
					pJg->SetEnd(pNodeE->xOriginalPos);
					pJg->cMaterial=QuerySteelBriefMatMark(sMat);
					pJg->set_wing_wide(fWidth);
					pJg->set_wing_thick(fThick);
					JG_NORM_INFO jg_norm;
					GEPOINT axis_z=pNodeE->xOriginalPos-pNodeS->xOriginalPos;
					normalize(axis_z);
					RotateVectorAroundVector(ptX,-1,0,axis_z);
					RotateVectorAroundVector(ptY, 1,0,axis_z);
					jg_norm.vNormX=ptX;
					jg_norm.vNormY=ptY;
					m_hashJgNorm.SetValue(pJg->handle,jg_norm);
					//
					if(ciReadType==0 && pBodyItem)
						pBodyItem->bodyRodSet.append(pJg);
					else if(ciReadType==1 && pLegItem)
						pLegItem->segmentRodSet.append(pJg);
					else if(ciReadType==2 && pSubLeg)
						pSubLeg->legRodSet.append(pJg);
				}
				else if(strstr(sSpec,"��")||strstr(sSpec,"��"))
				{	//�ֹ�
					sscanf(line_txt,"%s%d%d%s",(char*)key_word,&indexS,&indexE,sSpec);
					char mark,symbol;
					CXhChar16 ss(sSpec);
					ss.Replace("��","G");
					sscanf(ss,"%c%lf%c%lf",&symbol,&fWidth,&mark,&fThick);
					CLDSLineTube* pTube=(CLDSLineTube*)Ta.AppendPart(CLS_LINETUBE);
					pTube->layer(1)='X';
					pTube->cMaterial=QuerySteelBriefMatMark(sMat);
					pTube->pStart=pNodeS;
					pTube->pEnd=pNodeE;
					pTube->size_wide=fWidth;
					pTube->size_thick=fThick;
					//
					if(ciReadType==0 && pBodyItem)
						pBodyItem->bodyRodSet.append(pTube);
					else if(ciReadType==1 && pLegItem)
						pLegItem->segmentRodSet.append(pTube);
					else if(ciReadType==2 && pSubLeg)
						pSubLeg->legRodSet.append(pTube);
				}
#ifdef __GGG_
				else if(strlen(sMat)>0)
				{	//׵�ܣ����Ϊ�գ����ʲ�Ϊ��
					int nDiameterS=0,nDiameterE,nThick=0,nFaceNum=0;
					sscanf(line_txt,"%s%d%d%s%d%d%d%d",(char*)key_word,&indexS,&indexE,sMat,&nDiameterS,&nDiameterE,&nThick,&nFaceNum);
					CLDSConeTube* pConeTube=(CLDSConeTube*)Ta.AppendPart(CLS_CONETUBE);
					pConeTube->layer(1)='X';
					pConeTube->cMaterial=QuerySteelBriefMatMark(sMat);
					pConeTube->pStart=pNodeS;
					pConeTube->pEnd=pNodeE;
					pConeTube->size_wide=nDiameterS*0.5;
					pConeTube->size_wideUpSide=nDiameterE*0.5;
					pConeTube->size_thick=nThick;
					pConeTube->faceN=nFaceNum;
					//
					if(ciReadType==0 && pBodyItem)
						pBodyItem->bodyRodSet.append(pConeTube);
					else if(ciReadType==1 && pLegItem)
						pLegItem->segmentRodSet.append(pConeTube);
					else if(ciReadType==2 && pSubLeg)
						pSubLeg->legRodSet.append(pConeTube);
				}
#endif
				else
				{
					CLDSLineAngle* pJg=(CLDSLineAngle*)Ta.AppendPart(CLS_LINEANGLE);
					pJg->layer(1)='X';
					pJg->pStart=pNodeS;
					pJg->pEnd=pNodeE;
					//
					if(ciReadType==0 && pBodyItem)
						pBodyItem->bodyRodSet.append(pJg);
					else if(ciReadType==1 && pLegItem)
						pLegItem->segmentRodSet.append(pJg);
					else if(ciReadType==2 && pSubLeg)
						pSubLeg->legRodSet.append(pJg);
				}
			}
			else if(stricmp(key_word,"G")==0)
			{	//�ҵ�
				CXhChar16 sType;
				HANG_NODE_INFO* pGuaInfo=m_listGuaNode.append();
				CXhChar200 szBackupText=line_txt;
				char* pszKey=strtok(line_txt,", ");
				int indexOfPrevKeyAddr=0;
				for(int i=0;pszKey!=NULL&&i<6;i++)
				{
					int indexOfCurrKey=pszKey-(char*)line_txt;
					for(int j=indexOfPrevKeyAddr;j<indexOfCurrKey-1;j++)
					{
						if(szBackupText[j]==',')
							i++;
					}
					indexOfPrevKeyAddr=indexOfCurrKey;
					if(i==1)	//��������
						pGuaInfo->m_ciType=*pszKey;
					else if(i==2)	//�ҵ�����
					{
						StrCopy(pGuaInfo->m_sName,pszKey,50);
						char* pchSpliter=strchr(pGuaInfo->m_sName,'_');
						if(pchSpliter==NULL)
							pchSpliter=strchr(pGuaInfo->m_sName,'-');
						while(pchSpliter)
						{
							if(*pchSpliter<'0'||*pchSpliter>'9')
							{
								pchSpliter++;
								continue;
							}
							pGuaInfo->m_xWirePlace.ciPostCode=atoi(pchSpliter);
							break;
						}
					}
					else if(i==3)
						pGuaInfo->pos.x=atof(pszKey);
					else if(i==4)
						pGuaInfo->pos.y=atof(pszKey);
					else if(i==5)
						pGuaInfo->pos.z=atof(pszKey);
					pszKey=strtok(NULL,", ");
				}
			}
		}
	}
	fclose(fp);
	//
	AmendData();
}
void CModDataFile::ExportModFile(const char* szTowerModFile)
{
	CLogErrorLife loglife(&logerr);
	CWaitCursor xWaitCursor;
	if(!PrepareParaData())
		return;
	FILE *fp=NULL;
	if(g_sysPara.m_iModFileCodeType==0)
		fp=fopen(szTowerModFile,"wt,ccs=UTF-8");
	else
		fp=fopen(szTowerModFile,"wt");
	if(fp==NULL)
	{
		logerr.Log("MOD�ļ�����ʧ��@%s", szTowerModFile);
		return;
	}
	if(g_sysPara.m_iModFileCodeType==0)
		WriteToFileByUtf8(fp);
	else
		WriteToFileByAnsi(fp);
}
//////////////////////////////////////////////////////////////////////////
//UTF-8�ַ��������:
//1�ֽڣ�0xxxxxxx
//2�ֽڣ�110xxxxx 10xxxxxx
//3�ֽڣ�1110xxxx 10xxxxxx 10xxxxxx
//4�ֽڣ�11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//5�ֽڣ�111110xx	10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//6�ֽڣ�1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//////////////////////////////////////////////////////////////////////////
BOOL CModDataFile::IsUTF8File(const char* mod_file)
{
	FILE *fp = fopen(mod_file,"rb");
	if(fp==NULL)
		return FALSE;
	fseek(fp, 0, SEEK_END);
	long lSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	CBuffer buffer;
	buffer.Write(NULL,lSize);
	fread(buffer.GetBufferPtr(),1,buffer.GetLength(),fp);
	fclose(fp);
	//
	unsigned char* start = (unsigned char*)buffer.GetBufferPtr();
	unsigned char* end = (unsigned char*)start + lSize;
	BOOL bIsUTF8 = TRUE;
	while(start<end)
	{
		if(*start<0x80)
		{	//1�ֽ�(0xxxxxxx): ֵС��0x80��ΪASCII�ַ�  	
			start++; 
		}
		else if(*start<(0xC0)) 
		{	//ֵ����0x80��0xC0֮���Ϊ��ЧUTF-8�ַ� 
			bIsUTF8=FALSE;  
			break;
		}
		else if(*start<(0xE0)) 
		{	//2�ֽ�(110xxxxx 10xxxxxx)
			if(start>=end-1)
				break;
			if((start[1]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=2;
		}
		else if(*start<(0xF0)) 
		{	//3�ֽ�(1110xxxx 10xxxxxx 10xxxxx)
			if(start>=end-2)
				break;
			if((start[1]&(0xC0))!=0x80||(start[2]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=3;
		}
		else if(*start<(0xF8))
		{	//4�ֽ�(11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
			if(start>=end-3)
				break;
			if((start[1]&(0xC0))!=0x80||(start[2]&(0xC0))!=0x80||(start[3]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=4;
		}
		else if(*start<(0xFC))
		{	//5�ֽ�(111110xx	10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx)
			if(start>=end-4)
				break;
			if((start[1]&(0xC0))!=0x80||(start[2]&(0xC0))!=0x80||(start[3]&(0xC0))!=0x80||
				(start[4]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=5;
		}
		else if(*start<(0xFE))
		{	//6�ֽ�(1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx)
			if(start>=end-5)
				break;
			if((start[1]&(0xC0))!=0x80||(start[2]&(0xC0))!=0x80||(start[3]&(0xC0))!=0x80||
				(start[4]&(0xC0))!=0x80||(start[5]&(0xC0))!=0x80)
			{
				bIsUTF8=FALSE;
				break;
			}
			start+=6;
		}
		else
		{
			bIsUTF8 = FALSE;
			break;
		}
	}
	if(!bIsUTF8)
	{	//�����ַ�ƥ������ٴν����ж�
		int goodbytes = 0, asciibytes = 0;
		int rawtextlen = lSize;
		char* rawtext=buffer.GetBufferPtr();
		for(int i = 0; i < rawtextlen; i++) 
		{
			if ((rawtext[i] &  0x7F) == rawtext[i]) 
			{	//���λ��0��ASCII�ַ�
				//һλ��������
				asciibytes++;
			} 
			else if (-64 <= rawtext[i] && rawtext[i] <= -33
				//��λ��������,��һλ11000000--11011111
				//��һλ��10000000--10111111
				&&i + 1 < rawtextlen 
				&& -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65) 
			{
				goodbytes += 2;
				i++;
			} 
			else if (-32 <= rawtext[i]&& rawtext[i] <= -17
				//��λ��������,��һλ11100000--11101111
				//����λ��10000000--10111111
				&&i + 2 < rawtextlen 
				&& -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65 
				&& -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65) 
			{
				goodbytes += 3;
				i += 2;
			}
			else if(-16 <= rawtext[i]&& rawtext[i] <= -9
				//��λ��������,��һλ11110000--11110111
				//����λ��10000000--10111111
				&&i + 3 < rawtextlen 
				&&  -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65 
				&& -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65
				&& -128 <= rawtext[i + 3] && rawtext[i + 3] <= -65)

			{
				goodbytes += 4;
				i += 3;
			}
			else if(-8 <= rawtext[i]&& rawtext[i] <= -5
				//��λ��������,��һλ11111000--11111011
				//����λ��10000000--10111111
				&&i + 4 < rawtextlen 
				&&  -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65 
				&& -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65
				&& -128 <= rawtext[i + 3] && rawtext[i + 3] <= -65
				&& -128 <= rawtext[i + 4] && rawtext[i + 4] <= -65)
			{
				goodbytes += 5;
				i += 4;
			}
			else if(-4 <= rawtext[i]&& rawtext[i] <= -3
				//��λ��������,��һλ11111100--11111101
				//����λ��10000000--10111111
				&&i + 5 < rawtextlen 
				&&  -128 <= rawtext[i + 1] && rawtext[i + 1] <= -65 
				&& -128 <= rawtext[i + 2] && rawtext[i + 2] <= -65
				&& -128 <= rawtext[i + 3] && rawtext[i + 3] <= -65
				&& -128 <= rawtext[i + 4] && rawtext[i + 4] <= -65
				&& -128 <= rawtext[i + 5] && rawtext[i + 5] <= -65)
			{
				goodbytes += 6;
				i += 5;
			}
		}
		if (asciibytes == rawtextlen) 
			return true;
		int score=100*goodbytes/(rawtextlen - asciibytes);
		if (score>98) //���ƥ���ʴﵽ98%����,��ɹ�
			return true;
		else if(score>95&&goodbytes>30) 
			return true;
		else 
			return false;
	}
	return true;
}
void CModDataFile::ImportModFile(const char* szTowerModFile)
{
	BOOL bUtf8=IsUTF8File(szTowerModFile);
	FILE *fp = fopen(szTowerModFile,"rt");
	if(fp==NULL)
		return;
	ReadModFile(fp,bUtf8);
}
//////////////////////////////////////////////////////////////////////////
//CGimDataFile
CGimDataFile::CGimDataFile()
{
	m_ciCodingType=UTF8_CODING;
}
CGimDataFile::~CGimDataFile()
{

}
BOOL CGimDataFile::Run7zCmd(char* sCmd)
{
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(NULL,sCmd,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL, NULL, &si,&pi))
	{
		logerr.Log("��������ʧ��");
		return FALSE;
	}
	//�ȴ����̽���
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return TRUE;
}
//#define TEMP_HENAN_LOG2
void CGimDataFile::CreateGuidFile()
{
	//�������·��
	m_sOutPath.Printf("%s%s_GIM", (char*)m_sFolder, (char*)m_sName);
	_mkdir(m_sOutPath);
#ifdef TEMP_HENAN_LOG2
	logerr.Log("Ŀ¼����%s", (char*)m_sOutPath);
#endif
	//�����Ŀ¼������MOD�ļ�
	CXhChar500 sFilePath= m_sOutPath;
	sFilePath.Append("\\MOD");
	_mkdir(sFilePath);
	CXhChar500 sModFilePath("%s\\%s.mod",(char*)sFilePath,(char*)m_sName);
#ifdef TEMP_HENAN_LOG2
	logerr.Log("�ļ�·�����ɣ�%s", (char*)sModFilePath);
#endif
	WriteModFile(sModFilePath);
#ifdef TEMP_HENAN_LOG2
	logerr.Log("mod�ļ�����");
#endif
	//��������MOD�ļ���PHM�ļ�
	sFilePath= m_sOutPath;
	sFilePath.Append("\\PHM");
	_mkdir(sFilePath);
	m_sPhmGuid=CreateGuidStr();
#ifdef TEMP_HENAN_LOG2
	logerr.Log("GUID ���ɣ�%s", (char*)m_sPhmGuid);
#endif
	CXhChar500 sPhmFilePath("%s\\%s.phm",(char*)sFilePath,(char*)m_sPhmGuid);
	WritePhmFile(sPhmFilePath);
	//��������PHM�ļ���DEV�ļ�����Ӧ�����ļ�
#ifdef TEMP_HENAN_LOG2
	logerr.Log("PHM ���ɣ�%s", (char*)sPhmFilePath);
#endif
	sFilePath= m_sOutPath;
	sFilePath.Append("\\DEV");
	_mkdir(sFilePath);
	m_sFamGuid=m_sDevGuid=CreateGuidStr();
#ifdef TEMP_HENAN_LOG2
	logerr.Log("m_sFamGuid ���ɣ�%s", (char*)m_sDevGuid);
#endif
	CXhChar500 sFamFilePath("%s\\%s.fam",(char*)sFilePath,(char*)m_sFamGuid);
	WriteFamFile(sFamFilePath);
#ifdef TEMP_HENAN_LOG2
	logerr.Log("fam ���ɣ�%s", (char*)sFamFilePath);
#endif
	CXhChar500 sDevFilePath("%s\\%s.dev",(char*)sFilePath,(char*)m_sDevGuid);
	WriteDevFile(sDevFilePath);
#ifdef TEMP_HENAN_LOG2
	logerr.Log("DEV ���ɣ�%s", (char*)sDevFilePath);
#endif
	//��������DEV�ļ����豸�����ļ�CBM
	sFilePath= m_sOutPath;
	sFilePath.Append("\\CBM");
	_mkdir(sFilePath);
	m_sCbmGuid=CreateGuidStr();
	CXhChar500 sCbmFilePath("%s\\%s.cbm",(char*)sFilePath,(char*)m_sCbmGuid);
#ifdef TEMP_HENAN_LOG2
	logerr.Log("sCbmFilePath ���ɣ�%s", (char*)sCbmFilePath);
#endif
	WriteCbmFile(sCbmFilePath);
#ifdef TEMP_HENAN_LOG2
	logerr.Log("sCbmFilePath ����2��%s", (char*)sCbmFilePath);
#endif
}
void CGimDataFile::DeleteGuidFile()
{
	CXhChar500 sFilePath=m_sOutPath;
	sFilePath.Append("\\MOD");
	DeleteDirectory(sFilePath);
	sFilePath= m_sOutPath;
	sFilePath.Append("\\PHM");
	DeleteDirectory(sFilePath);
	sFilePath= m_sOutPath;
	sFilePath.Append("\\DEV");
	DeleteDirectory(sFilePath);
	sFilePath= m_sOutPath;
	sFilePath.Append("\\CBM");
	DeleteDirectory(sFilePath);
	DeleteDirectory(m_sOutPath);
}
void CGimDataFile::WriteModFile(const char* sModFilePath)
{
	CXhChar500 mod_file("%s%s.mod",(char*)m_sFolder,(char*)m_sName);
	FILE *fp = fopen(mod_file,"rb");
	if(fp==NULL)
	{
		logerr.Log(CXhChar500("%s��ȡʧ��!",(char*)mod_file));
		return;
	}
	fseek(fp,0,SEEK_END);
	long buf_size=ftell(fp);
	fseek(fp,0,SEEK_SET);
	CBuffer buffer;
	buffer.Write(NULL,buf_size);
	fread(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
	//
	fp = fopen(sModFilePath,"wb");
	if(fp==NULL)
		return;
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
}
void CGimDataFile::WritePhmFile(const char* sPhmFile)
{
	FILE *fp=NULL;
	if(m_ciCodingType==ANSI_CODING)
		fp=fopen(sPhmFile,"wt");
	else
		fp=fopen(sPhmFile,"wt,ccs=UTF-8");
	if(fp==NULL)
		return;
	if(m_ciCodingType==ANSI_CODING)
	{	//ANSI
		fprintf(fp,"SOLIDMODELS.NUM=1\n");
		fprintf(fp,"SOLIDMODEL0=%s.mod\n",(char*)m_sName);
		fprintf(fp,"TRANSFORMMATRIX0=1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000\n");
		fprintf(fp,"COLOR0=255,255,255\n");
	}
	else
	{	//UTF-8
		wchar_t sWValue[MAX_PATH];
		ANSIToUnicode(m_sName,sWValue);
		fwprintf(fp,L"SOLIDMODELS.NUM=1\n");
		fwprintf(fp,L"SOLIDMODEL0=%s.mod\n",sWValue);
		fwprintf(fp,L"TRANSFORMMATRIX0=1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000\n");
		fwprintf(fp,L"COLOR0=255,255,255\n");
	}
	fclose(fp);
}
void CGimDataFile::WriteDevFile(const char* sDevFile)
{
	FILE *fp=NULL;
	if(m_ciCodingType==ANSI_CODING)
		fp=fopen(sDevFile,"wt");
	else
		fp=fopen(sDevFile,"wt,ccs=UTF-8");
	if(fp==NULL)
		return;
	if(m_ciCodingType==ANSI_CODING)
	{	//ANSI
		fprintf(fp,"DEVICETYPE = TOWER\n");
		fprintf(fp,"SYMBOLNAME = TOWER\n");
		fprintf(fp,"BASEFAMILYPOINTER=%s.fam\n",(char*)m_sFamGuid);
		fprintf(fp,"SOLIDMODELS.NUM = 1\n");
		fprintf(fp,"SOLIDMODEL0 = %s.phm\n",(char*)m_sPhmGuid);
		fprintf(fp,"TRANSFORMMATRIX0=1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000\n");
	}
	else
	{	//UTF-8
		wchar_t sWValue[MAX_PATH];
		fwprintf(fp,L"DEVICETYPE = TOWER\n");
		fwprintf(fp,L"SYMBOLNAME = TOWER\n");
		ANSIToUnicode(m_sFamGuid,sWValue);
		fwprintf(fp,L"BASEFAMILYPOINTER=%s.fam\n",sWValue);
		fwprintf(fp,L"SOLIDMODELS.NUM = 1\n");
		ANSIToUnicode(m_sPhmGuid,sWValue);
		fwprintf(fp,L"SOLIDMODEL0 = %s.phm\n",sWValue);
		fwprintf(fp,L"TRANSFORMMATRIX0=1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000\n");
	}
	fclose(fp);
}
void CGimDataFile::WriteFamFile(const char* sFamFile)
{
	FILE *fp=NULL;
	if(m_ciCodingType==ANSI_CODING)
		fp=fopen(sFamFile,"wt");
	else
		fp=fopen(sFamFile,"wt,ccs=UTF-8");
	if(fp==NULL)
		return;
	CXhChar50 ss;
	if(m_ciCodingType==ANSI_CODING)
	{
		//��ѹ�ȼ�
		fprintf(fp,"VOLTAGE = ��ѹ�ȼ� = %s\n",(char*)m_xTowerProp.m_sVoltGrade);
		//����
		fprintf(fp,"TYPE = ���� = %s\n",(char*)m_xTowerProp.m_sType);
		//��������
		fprintf(fp,"TEXTURE = �������� = %s\n",(char*)m_xTowerProp.m_sTexture);
		//�̶���ʽ
		fprintf(fp,"FIXEDTYPE = �̶���ʽ = %s\n",(char*)m_xTowerProp.m_sFixedType);
		//��������
		fprintf(fp,"TOWERTYPE = �������� = %s\n",(char*)m_xTowerProp.m_sTaType);
		//��·��
		fprintf(fp,"CIRCUIT = ��·�� = %d\n",m_xTowerProp.m_nCircuit);
		//�����ͺ�
		fprintf(fp,"CONDUCTOR = �����ͺ� = %s\n",(char*)m_xTowerProp.m_sCWireSpec);
		//�����ͺ�
		fprintf(fp,"GROUNDWIRE = �����ͺ� = %s\n",(char*)m_xTowerProp.m_sEWireSpec);
		//��������
		ss.Printf("%f",m_xTowerProp.m_fWindSpeed);
		SimplifiedNumString(ss);
		fprintf(fp,"REFERENCEWINDSPEED = ��ƻ������� = %s\n",(char*)ss);
		//�������
		ss.Printf("%f",m_xTowerProp.m_fNiceThick);
		SimplifiedNumString(ss);
		fprintf(fp,"DESIGNICETHICKNESS = ��Ƹ������ = %s\n",(char*)ss);
		//ˮƽ����
		fprintf(fp,"DESIGNWINDSPAN = ���ˮƽ���� = %s\n",(char*)m_xTowerProp.m_sWindSpan);
		//��ֱ����
		fprintf(fp,"DESIGNWEIGHTSPAN = ��ƴ�ֱ���� = %s\n",(char*)m_xTowerProp.m_sWeightSpan);
		//ǰ�������
		ss.Printf("%f",m_xTowerProp.m_fFrontRulingSpan);
		SimplifiedNumString(ss);
		fprintf(fp,"DESIGNFRONTRULINGSPAN = ���ǰ������� = %s\n",(char*)ss);
		//��������
		ss.Printf("%f",m_xTowerProp.m_fBackRulingSpan);
		SimplifiedNumString(ss);
		fprintf(fp,"DESIGNBACKRULINGSPAN=��ƺ�������=%s\n",(char*)ss);
		//��󵵾�
		ss.Printf("%f",m_xTowerProp.m_fMaxSpan);
		SimplifiedNumString(ss);
		fprintf(fp,"DESIGNMAXSPAN = �����󵵾� =%s\n",(char*)ss);
		//ת�Ƿ�Χ
		fprintf(fp,"ANGLERANGE = ���ת�Ƿ�Χ =%s\n",(char*)m_xTowerProp.m_sAngleRange);
		//Kv
		ss.Printf("%f",m_xTowerProp.m_fDesignKV);
		SimplifiedNumString(ss);
		fprintf(fp,"DESIGNKV = ���Kvֵ = %s\n",(char*)ss);
		//�������
		fprintf(fp,"RATEDNOMINALHEIGHT = ������� = %s\n",(char*)m_xTowerProp.m_sRatedHeight);
		//���߷�Χ
		fprintf(fp,"NOMINALHEIGHTRANGE = ���߷�Χ = %s\n",(char*)m_xTowerProp.m_sHeightRange);
		//����
		fprintf(fp,"TOWERWEIGHT = ���� = %s\n",(char*)m_xTowerProp.m_sTowerWeight);
		//��ƹ�Ƶҡ�ڽǶ�
		ss.Printf("%f",m_xTowerProp.m_fFrequencyRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp,"ROCKANGLEOFDESIGNPOWERFREQUENCY = ��ƹ�Ƶҡ�ڽǶ� = %s\n",(char*)ss);
		//����׵�ҡ�ڽǶ�
		ss.Printf("%f",m_xTowerProp.m_fLightningRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp,"ROCKANGLEOFDESIGNLIGHTNING = ����׵�ҡ�ڽǶ� = %s\n",(char*)ss);
		//��Ʋ���ҡ�ڽǶ�
		ss.Printf("%f",m_xTowerProp.m_fSwitchingRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp,"ROCKANGLEOFDESIGNSWITCHING = ��Ʋ���ҡ�ڽǶ� = %s\n",(char*)ss);
		//��ƴ�����ҵҡ�ڽǶ�
		ss.Printf("%f",m_xTowerProp.m_fWorkingRockAngle);
		SimplifiedNumString(ss);
		fprintf(fp,"ROCKANGLEOFDESIGNLIVEWORKING = ��ƴ�����ҵҡ�ڽǶ� = %s\n",(char*)ss);
		//��������
		fprintf(fp,"MANUFACTURER = �������� = %s\n",(char*)m_xTowerProp.m_sManuFacturer);
		//���ʱ���
		fprintf(fp,"MATERIALCODE = ���ʱ��� = %s\n",(char*)m_xTowerProp.m_sMaterialCode);
		//װ��ģ�ͱ��
		fprintf(fp,"PROCESSMODELCODE = װ��ģ�ͱ�� = %s\n",(char*)m_xTowerProp.m_sProModelCode);
	}
	else
	{
		CXhChar500 sValue;
		wchar_t sWValue[MAX_PATH];
		//��ѹ�ȼ�
		sValue.Printf("VOLTAGE = ��ѹ�ȼ� = %s",(char*)m_xTowerProp.m_sVoltGrade);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//����
		sValue.Printf("TYPE = ���� = %s",(char*)m_xTowerProp.m_sType);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��������
		sValue.Printf("TEXTURE = �������� = %s",(char*)m_xTowerProp.m_sTexture);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//�̶���ʽ
		sValue.Printf("FIXEDTYPE = �̶���ʽ = %s",(char*)m_xTowerProp.m_sFixedType);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��������
		sValue.Printf("TOWERTYPE = �������� = %s",(char*)m_xTowerProp.m_sTaType);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��·��
		sValue.Printf("CIRCUIT = ��·�� = %d",m_xTowerProp.m_nCircuit);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//�����ͺ�
		sValue.Printf("CONDUCTOR = �����ͺ� = %s",(char*)m_xTowerProp.m_sCWireSpec);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//�����ͺ�
		sValue.Printf("GROUNDWIRE = �����ͺ� = %s",(char*)m_xTowerProp.m_sEWireSpec);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��ƻ�������
		ss.Printf("%f",m_xTowerProp.m_fWindSpeed);
		SimplifiedNumString(ss);
		sValue.Printf("REFERENCEWINDSPEED = ��ƻ������� = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��Ƹ������
		ss.Printf("%f",m_xTowerProp.m_fNiceThick);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNICETHICKNESS = ��Ƹ������ = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//���ˮƽ����
		sValue.Printf("DESIGNWINDSPAN = ���ˮƽ���� = %s",(char*)m_xTowerProp.m_sWindSpan);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��ƴ�ֱ����
		sValue.Printf("DESIGNWEIGHTSPAN = ��ƴ�ֱ���� = %s",(char*)m_xTowerProp.m_sWeightSpan);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//���ǰ�������
		ss.Printf("%f",m_xTowerProp.m_fFrontRulingSpan);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNFRONTRULINGSPAN = ���ǰ������� = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��ƺ�������
		ss.Printf("%f",m_xTowerProp.m_fBackRulingSpan);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNBACKRULINGSPAN = ��ƺ������� = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//�����󵵾�
		ss.Printf("%f",m_xTowerProp.m_fMaxSpan);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNMAXSPAN = �����󵵾� = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//���ת�Ƿ�Χ
		sValue.Printf("ANGLERANGE = ���ת�Ƿ�Χ = %s",(char*)m_xTowerProp.m_sAngleRange);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//Kv
		ss.Printf("%f",m_xTowerProp.m_fDesignKV);
		SimplifiedNumString(ss);
		sValue.Printf("DESIGNKV = ���Kvֵ = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//�������
		sValue.Printf("RATEDNOMINALHEIGHT = ������� = %s",(char*)m_xTowerProp.m_sRatedHeight);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//���߷�Χ
		sValue.Printf("NOMINALHEIGHTRANGE = ���߷�Χ = %s",(char*)m_xTowerProp.m_sHeightRange);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//����
		sValue.Printf("TOWERWEIGHT = ���� = %s",(char*)m_xTowerProp.m_sTowerWeight);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��ƹ�Ƶҡ�ڽǶ�
		ss.Printf("%f",m_xTowerProp.m_fFrequencyRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNPOWERFREQUENCY = ��ƹ�Ƶҡ�ڽǶ� = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//����׵�ҡ�ڽǶ�
		ss.Printf("%f",m_xTowerProp.m_fLightningRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNLIGHTNING = ����׵�ҡ�ڽǶ� = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��Ʋ���ҡ�ڽǶ�
		ss.Printf("%f",m_xTowerProp.m_fSwitchingRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNSWITCHING = ��Ʋ���ҡ�ڽǶ� = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��ƴ�����ҵҡ�ڽǶ�
		ss.Printf("%f",m_xTowerProp.m_fWorkingRockAngle);
		SimplifiedNumString(ss);
		sValue.Printf("ROCKANGLEOFDESIGNLIVEWORKING = ��ƴ�����ҵҡ�ڽǶ� = %s",(char*)ss);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//��������
		sValue.Printf("MANUFACTURER = �������� = %s",(char*)m_xTowerProp.m_sManuFacturer);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//���ʱ���
		sValue.Printf("MATERIALCODE = ���ʱ��� = %s",(char*)m_xTowerProp.m_sMaterialCode);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
		//װ��ģ�ͱ��
		sValue.Printf("PROCESSMODELCODE = װ��ģ�ͱ�� = %s",(char*)m_xTowerProp.m_sProModelCode);
		ANSIToUnicode(sValue,sWValue);
		fwprintf(fp,L"%s\n",sWValue);
	}
	fclose(fp);
}
BOOL CGimDataFile::ReadFamFile(const char* sFamFile)
{
	FILE *fp=fopen(sFamFile,"rt");
	if(fp==NULL)
	{
		logerr.Log("%s��ʧ��!",sFamFile);
		return FALSE;
	}
	char sLine1[MAX_PATH]="",sLine2[MAX_PATH]="";
	while(!feof(fp))
	{
		if(fgets(sLine1,MAX_PATH,fp)==NULL)
			continue;
		if(m_ciCodingType==UTF8_CODING)
			UTF8ToANSI(sLine1,sLine2);
		else
			strcpy(sLine2,sLine1);
		CXhChar100 sText(sLine2);
		sText.Replace('\t',' ');
		sText.Replace('\n',' ');
		sText.Remove(' ');
		char *skey1=strtok(sText,"=");
		char *skey2=strtok(NULL,"=");
		char *skey=strtok(NULL,"=");
		if(skey==NULL||strlen(skey)<=0||skey[0]==10)
			continue;
		if(strstr(skey1,"VOLTAGE")||stricmp(skey2,"��ѹ�ȼ�")==0)
			m_xTowerProp.m_sVoltGrade.Copy(skey);
		else if(stricmp(skey1,"TYPE")==0||stricmp(skey2,"����")==0)
			m_xTowerProp.m_sType.Copy(skey);
		else if(stricmp(skey1,"TEXTURE")==0||stricmp(skey2,"��������")==0)
			m_xTowerProp.m_sTexture.Copy(skey);
		else if(stricmp(skey1,"FIXEDTYPE")==0||stricmp(skey2,"�̶���ʽ")==0)
			m_xTowerProp.m_sFixedType.Copy(skey);
		else if(stricmp(skey1,"TOWERTYPE")==0||stricmp(skey2,"��������")==0)
			m_xTowerProp.m_sTaType.Copy(skey);
		else if(stricmp(skey1,"CIRCUIT")==0||stricmp(skey2,"��·��")==0)
			m_xTowerProp.m_nCircuit=atoi(skey);
		else if(stricmp(skey1,"CONDUCTOR")==0||stricmp(skey2,"�����ͺ�")==0)
			m_xTowerProp.m_sCWireSpec.Copy(skey);
		else if(stricmp(skey1,"GROUNDWIRE")==0||stricmp(skey2,"�����ͺ�")==0)
			m_xTowerProp.m_sEWireSpec.Copy(skey);
		else if(stricmp(skey1,"REFERENCEWINDSPEED")==0||stricmp(skey2,"��ƻ�������")==0)
			m_xTowerProp.m_fWindSpeed=atof(skey);
		else if(stricmp(skey1,"DESIGNICETHICKNESS")==0||stricmp(skey2,"��Ƹ������")==0)
			m_xTowerProp.m_fNiceThick=atof(skey);
		else if(stricmp(skey1,"DESIGNWINDSPAN")==0||stricmp(skey2,"���ˮƽ����")==0)
			m_xTowerProp.m_sWindSpan.Copy(skey);
		else if(stricmp(skey1,"DESIGNWEIGHTSPAN")==0||stricmp(skey2,"��ƴ�ֱ����")==0)
			m_xTowerProp.m_sWeightSpan.Copy(skey);
		else if(stricmp(skey1,"DESIGNFRONTRULINGSPAN")==0||stricmp(skey2,"���ǰ�������")==0)
			m_xTowerProp.m_fFrontRulingSpan=atof(skey);
		else if(stricmp(skey1,"DESIGNBACKRULINGSPAN")==0||stricmp(skey2,"��ƺ�������")==0)
			m_xTowerProp.m_fBackRulingSpan=atof(skey);
		else if(stricmp(skey1,"DESIGNMAXSPAN")==0||stricmp(skey2,"�����󵵾�")==0)
			m_xTowerProp.m_fMaxSpan=atof(skey);
		else if(stricmp(skey1,"ANGLERANGE")==0||stricmp(skey2,"���ת�Ƿ�Χ")==0)
			m_xTowerProp.m_sAngleRange.Copy(skey);
		else if(stricmp(skey1,"DESIGNKV")==0||stricmp(skey2,"���Kvֵ")==0)
			m_xTowerProp.m_fDesignKV=atof(skey);
		else if(stricmp(skey1,"RATEDNOMINALHEIGHT")==0||stricmp(skey2,"�������")==0)
			m_xTowerProp.m_sRatedHeight.Copy(skey);
		else if(stricmp(skey1,"NOMINALHEIGHTRANGE")==0||stricmp(skey2,"���߷�Χ")==0)
			m_xTowerProp.m_sHeightRange.Copy(skey);
		else if(stricmp(skey1,"TOWERWEIGHT")==0||stricmp(skey2,"����")==0)
			m_xTowerProp.m_sTowerWeight.Copy(skey);
		else if(stricmp(skey1,"ROCKANGLEOFDESIGNPOWERFREQUENCY")==0||stricmp(skey2,"��ƹ�Ƶҡ�ڽǶ�")==0)
			m_xTowerProp.m_fFrequencyRockAngle=atof(skey);
		else if(stricmp(skey1,"ROCKANGLEOFDESIGNLIGHTNING")==0||stricmp(skey2,"����׵�ҡ�ڽǶ�")==0)
			m_xTowerProp.m_fLightningRockAngle=atof(skey);
		else if(stricmp(skey1,"ROCKANGLEOFDESIGNSWITCHING")==0||stricmp(skey2,"��Ʋ���ҡ�ڽǶ�")==0)
			m_xTowerProp.m_fSwitchingRockAngle=atof(skey);
		else if(stricmp(skey1,"ROCKANGLEOFDESIGNLIVEWORKING")==0||stricmp(skey2,"��ƴ�����ҵҡ�ڽǶ�")==0)
			m_xTowerProp.m_fWorkingRockAngle=atof(skey);
		else if(stricmp(skey1,"MANUFACTURER")==0||stricmp(skey2,"��������")==0)
			m_xTowerProp.m_sManuFacturer.Copy(skey);
		else if(stricmp(skey1,"MATERIALCODE")==0||stricmp(skey2,"���ʱ���")==0)
			m_xTowerProp.m_sMaterialCode.Copy(skey);
		else if(stricmp(skey1,"PROCESSMODELCODE")==0||stricmp(skey2,"װ��ģ�ͱ��")==0)
			m_xTowerProp.m_sProModelCode.Copy(skey);
	}
	fclose(fp);
	return TRUE;
}
void CGimDataFile::WriteCbmFile(const char* sCbmFile)
{
	FILE *fp=NULL;
	if(m_ciCodingType==ANSI_CODING)
		fp=fopen(sCbmFile,"wt");
	else
		fp=fopen(sCbmFile,"wt,ccs=UTF-8");
	if(fp==NULL)
		return;
	if(m_ciCodingType==ANSI_CODING)
	{
		fprintf(fp,"ENTITYNAME = Device\n");
		fprintf(fp,"OBJECTMODELPOINTER = %s.dev\n",(char*)m_sDevGuid);
		fprintf(fp,"BASEFAMILY = \n");
		fprintf(fp,"TRANSFORMMATRIX = 0.000000,0.000000,-0.000000,0.000000,-0.000000,0.000000,-0.000000,0.000000,-0.000000,0.000000,1.000000,0.000000,-0.000000,0.000000,-0.000000,1.000000\n");
	}
	else
	{
		wchar_t sWValue[MAX_PATH];
		ANSIToUnicode(m_sDevGuid,sWValue);
		fwprintf(fp,L"ENTITYNAME = Device\n");
		fwprintf(fp,L"OBJECTMODELPOINTER = %s.dev\n",sWValue);
		fwprintf(fp,L"BASEFAMILY = \n");
		fwprintf(fp,L"TRANSFORMMATRIX = 0.000000,0.000000,-0.000000,0.000000,-0.000000,0.000000,-0.000000,0.000000,-0.000000,0.000000,1.000000,0.000000,-0.000000,0.000000,-0.000000,1.000000\n");
	}
	fclose(fp);
}
BOOL CGimDataFile::ReadDevFile(const char* sDevFile)
{
	FILE *fp = fopen(sDevFile, "rt");
	if (fp == NULL)
	{
		logerr.Log("%s��ʧ��!", sDevFile);
		return FALSE;
	}
	CXhChar50 sPhmName,sFamName;
	char sLine1[MAX_PATH] = "", sLine2[MAX_PATH] = "";
	while (!feof(fp))
	{
		if (fgets(sLine1, MAX_PATH, fp) == NULL)
			continue;
		if (m_ciCodingType == UTF8_CODING)
			UTF8ToANSI(sLine1, sLine2);
		else
			strcpy(sLine2, sLine1);
		CXhChar100 sText(sLine2);
		sText.Replace('\t', ' ');
		sText.Remove(' ');
		char *skey = strtok(sText, "=");
		if (stricmp(skey, "SOLIDMODEL0") == 0)
		{
			skey = strtok(NULL, "=");
			sPhmName.Copy(skey);
		}
		if (stricmp(skey, "BASEFAMILYPOINTER") == 0)
		{
			skey = strtok(NULL, "=");
			sFamName.Copy(skey);
		}
	}
	fclose(fp);
	//��ȡFAM&PHM�ļ�
	_splitpath(sDevFile, NULL, NULL, m_sDevGuid, NULL);
	if(sFamName.GetLength()>0)
		_splitpath(sFamName, NULL, NULL, m_sFamGuid, NULL);
	else
		m_sFamGuid = m_sDevGuid;
	if (sPhmName.GetLength() > 0)
		_splitpath(sPhmName, NULL, NULL, m_sPhmGuid, NULL);
	return TRUE;
}
BOOL CGimDataFile::ReadPhmFile(const char* sPhmFile)
{
	FILE *fp = fopen(sPhmFile, "rt");
	if (fp == NULL)
	{
		logerr.Log("%s��ʧ��!", sPhmFile);
		return FALSE;
	}
	CXhChar50 sModName;
	char sLine1[MAX_PATH] = "", sLine2[MAX_PATH] = "";
	while (!feof(fp))
	{
		if (fgets(sLine1, MAX_PATH, fp) == NULL)
			continue;
		if (m_ciCodingType == UTF8_CODING)
			UTF8ToANSI(sLine1, sLine2);
		else
			strcpy(sLine2, sLine1);
		CXhChar100 sText(sLine2);
		sText.Replace('\t', ' ');
		sText.Remove(' ');
		char *skey = strtok(sText, "=");
		if (stricmp(skey, "SOLIDMODEL0") == 0)
		{
			skey = strtok(NULL, "=");
			sModName.Copy(skey);
		}
	}
	fclose(fp);
	//��ȡMOD�ļ�
	if (sModName.GetLength() > 0)
		_splitpath(sModName, NULL, NULL, m_sName, NULL);
	return TRUE;
}
BOOL CGimDataFile::ReadCbmFile(const char* sCbmFile)
{
	FILE *fp = fopen(sCbmFile, "rt");
	if (fp == NULL)
	{
		logerr.Log("%s��ʧ��!", sCbmFile);
		return FALSE;
	}

	fclose(fp);
	return TRUE;
}
void CGimDataFile::ToBuffer(CBuffer &buffer,CBuffer &zipBuffer)
{
	//��ȡ��ǰʱ�䣬��ת��Ϊ������
	if(strlen(m_xGimHeadProp.m_sTime)<=0)
	{
		time_t tt = time(0);
		char sTime[32]={0};
		strftime(sTime, sizeof(sTime),"%Y-%m-%d %H:%M", localtime(&tt));
		memcpy(m_xGimHeadProp.m_sTime,sTime,16);
	}
	CXhString szBuffSize(m_xGimHeadProp.m_sBufSize,8);
	szBuffSize.Printf("%d",zipBuffer.GetLength());
	//д��ָ��ͷ����
	buffer.Write(m_xGimHeadProp.m_sFileTag,16);			//�ļ���ʶ
	buffer.Write(m_xGimHeadProp.m_sFileName,256);		//�ļ�����
	buffer.Write(m_xGimHeadProp.m_sDesigner,64);		//�����
	buffer.Write(m_xGimHeadProp.m_sUnit,256);			//��֯��λ
	buffer.Write(m_xGimHeadProp.m_sSoftName,128);		//�������
	buffer.Write(m_xGimHeadProp.m_sTime,16);			//����ʱ��		
	buffer.Write(m_xGimHeadProp.m_sSoftMajorVer,8);		//������汾��
	buffer.Write(m_xGimHeadProp.m_sSoftMinorVer,8);		//����ΰ汾��
	buffer.Write(m_xGimHeadProp.m_sMajorVersion,8);		//��׼���汾��
	buffer.Write(m_xGimHeadProp.m_sMinorVersion,8);		//��׼�ΰ汾��
	buffer.Write(m_xGimHeadProp.m_sBufSize,8);			//�洢�����С
	//д��7Z����
	buffer.Write(zipBuffer.GetBufferPtr(),zipBuffer.GetLength());
}
void CGimDataFile::FromBuffer(CBuffer &buffer)
{
	buffer.SeekToBegin();
	//��ȡͷ����
	buffer.Read(m_xGimHeadProp.m_sFileTag,16);		//�ļ���ʶ
	buffer.Read(m_xGimHeadProp.m_sFileName,256);	//�ļ�����
	buffer.Read(m_xGimHeadProp.m_sDesigner,64);		//�����
	buffer.Read(m_xGimHeadProp.m_sUnit,256);		//��֯��λ
	buffer.Read(m_xGimHeadProp.m_sSoftName,128);	//�������
	buffer.Read(m_xGimHeadProp.m_sTime,16);			//����ʱ��		
	buffer.Read(m_xGimHeadProp.m_sSoftMajorVer,8);	//������汾��
	buffer.Read(m_xGimHeadProp.m_sSoftMinorVer,8);	//����ΰ汾��
	buffer.Read(m_xGimHeadProp.m_sMajorVersion,8);	//��׼���汾��
	buffer.Read(m_xGimHeadProp.m_sMinorVersion,8);	//��׼�ΰ汾��
	buffer.Read(m_xGimHeadProp.m_sBufSize,8);		//�洢�����С
}
//����ѹ���������
BOOL CGimDataFile::PackGimFile()
{
	//Ĭ��ѹ��Ϊ7Z��ʽ��
	CXhChar500 gim_path("%s%s.gim",(char*)m_sFolder,(char*)m_sName);
	CXhChar500 cmd_str("7z.exe a %s %s\\*",(char*)gim_path,(char*)m_sOutPath);
	if(!CGimDataFile::Run7zCmd(cmd_str))
	{
		logerr.Log(CXhChar100("gim�ļ�<%s>����ʧ��!",(char*)m_sOutPath));
		return FALSE;
	}
	//ɾ����ʱ���ɵ��ļ���
	DeleteGuidFile();
	return TRUE;
}
BOOL CGimDataFile::UpdateGimFile()
{
	//��ȡ7zѹ����Ĭ�ϵ��ֽ�
	CXhChar500 gim_path("%s%s.gim",(char*)m_sFolder,(char*)m_sName);
	FILE* fp=fopen(gim_path,"rb");
	if(fp==NULL)
	{
		logerr.Log(CXhChar100("%s��������ʧ��(��)!",(char*)gim_path));
		return FALSE;
	}
	fseek(fp,0,SEEK_END);
	long buf_size=ftell(fp);
	fseek(fp,0,SEEK_SET);
	CBuffer zipBuf;
	zipBuf.Write(NULL,buf_size);
	fread(zipBuf.GetBufferPtr(),zipBuf.GetLength(),1,fp);
	fclose(fp);
	//��GIM�ļ�ͷ������ض�����
	CBuffer gimBuf(10000000);
	ToBuffer(gimBuf,zipBuf);
	fp=fopen(gim_path,"wb");
	if(fp==NULL)
	{
		logerr.Log(CXhChar100("%s��������ʧ��(д)!",(char*)gim_path));
		return FALSE;
	}
	fwrite(gimBuf.GetBufferPtr(),gimBuf.GetLength(),1,fp);
	fclose(fp);
	return TRUE;
}
void CGimDataFile::SearchFile(ATOM_LIST<CString>& xFileList, CXhChar200 sFilePath, BYTE ciFileType/*=0*/)
{
	CFileFind file_find;
	BOOL bFind = file_find.FindFile(sFilePath);
	while (bFind)
	{
		bFind = file_find.FindNextFile();
		if (file_find.IsDots() || file_find.IsHidden() || file_find.IsReadOnly() ||
			file_find.IsSystem() || file_find.IsTemporary())
			continue;
		CString file_path = file_find.GetFilePath();
		if (file_find.IsDirectory())
		{
			CString file_name = file_find.GetFileName();
			if (ciFileType == 0 ||
				(ciFileType == CBM_FILE && file_name == "CBM") ||
				(ciFileType == DEV_FILE && file_name == "DEV") ||
				(ciFileType == FAM_FILE && file_name == "DEV") ||
				(ciFileType == PHM_FILE && file_name == "PHM") ||
				(ciFileType == MOD_FILE && file_name == "MOD"))
				SearchFile(xFileList, CXhChar200("%s\\*", file_path));
		}
		else
			xFileList.append(file_path);
	}
	file_find.Close();
}
BOOL CGimDataFile::IsTowerDevFile(const char* sDevFile)
{
	FILE *fp = fopen(sDevFile, "rt");
	if (fp == NULL)
		return FALSE;
	BOOL bTowerDevFile = FALSE;
	char sLine1[MAX_PATH] = "", sLine2[MAX_PATH] = "";
	while (!feof(fp))
	{
		if (fgets(sLine1, MAX_PATH, fp) == NULL)
			continue;
		UTF8ToANSI(sLine1, sLine2);
		CXhChar100 sText(sLine2);
		sText.Replace('\t', ' ');
		sText.Remove(' ');
		char *skey = strtok(sText, "=");
		skey = strtok(NULL, "=");
		if (skey == NULL)
			continue;
		if (strstr(skey, "TOWER"))
		{
			bTowerDevFile = TRUE;
			break;
		}
	}
	fclose(fp);
	return bTowerDevFile;
}
BOOL CGimDataFile::UnpackGimFile(const char* gim_file)
{
	char drive[4];
	char dir[MAX_PATH], fname[MAX_PATH], folder[MAX_PATH];
	_splitpath(gim_file, drive, dir, fname, NULL);
	strcpy(folder, drive);
	strcat(folder, dir);
	m_sOutPath.Printf("%s%s_GIM", folder, fname);
	//
	CString sCmd;
	sCmd.Format("%s7zA\\7z.exe x %s -o%s -aoa", theApp.execute_path, gim_file, (char*)m_sOutPath);
	if (!CGimDataFile::Run7zCmd(sCmd.GetBuffer()))
		return FALSE;
	return TRUE;
}
BOOL CGimDataFile::ParseGimFileEx(const char* gim_file)
{
	if (!UnpackGimFile(gim_file))
		return FALSE;
	//��ȡͷ����Ϣ
	FILE* fp = fopen(gim_file, "rb");
	if (fp == NULL)
	{
		AfxMessageBox("GIM�ļ���ȡʧ��!");
		return FALSE;
	}
	fseek(fp, 0, SEEK_END);
	long buf_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	CBuffer gimBuf;
	gimBuf.Write(NULL, buf_size);
	fread(gimBuf.GetBufferPtr(), gimBuf.GetLength(), 1, fp);
	fclose(fp);
	FromBuffer(gimBuf);
	//
	ATOM_LIST<CString> xDevFileList;
	CXhChar200 sFilePath("%s\\*", (char*)m_sOutPath);
	SearchFile(xDevFileList, sFilePath);
	int nFileNum = xDevFileList.GetNodeNum();
	for (int index = 0; index < nFileNum; index++)
	{
		CString fdPath = xDevFileList[index];
		CString str_ext = fdPath.Right(4);	//ȡ��׺��
		str_ext.MakeLower();
		if (str_ext.CompareNoCase(".dev") != 0)
			continue;
		if (!IsTowerDevFile(fdPath))
			continue;
		if (ReadDevFile(fdPath))
			break;
	}
	CXhChar200 sFamFile("%s\\DEV\\%s.fam", (char*)m_sOutPath, (char*)m_sFamGuid);
	if (!ReadFamFile(sFamFile))
		return FALSE;
	CXhChar200 sPhmFile("%s\\PHM\\%s.phm", (char*)m_sOutPath, (char*)m_sPhmGuid);
	if (!ReadPhmFile(sPhmFile))
		return FALSE;
	m_sModFile.Printf("%s\\MOD\\%s.mod", (char*)m_sOutPath, (char*)m_sName);
	return TRUE;
}
BOOL CGimDataFile::ParseGimFile(const char* gim_file)
{
	//��ȡͷ����Ϣ����ʱ����Ҫ
	FILE* fp = fopen(gim_file, "rb");
	if (fp == NULL)
	{
		AfxMessageBox("GIM�ļ���ȡʧ��!");
		return FALSE;
	}
	fseek(fp, 0, SEEK_END);
	long buf_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	CBuffer gimBuf;
	gimBuf.Write(NULL, buf_size);
	fread(gimBuf.GetBufferPtr(), gimBuf.GetLength(), 1, fp);
	fclose(fp);
	FromBuffer(gimBuf);
	//�������·��
	char drive[4];
	char dir[MAX_PATH], fname[MAX_PATH], folder[MAX_PATH];
	_splitpath(gim_file, drive, dir, fname, NULL);
	strcpy(folder, drive);
	strcat(folder, dir);
	m_sOutPath.Printf("%s%s_GIM", folder, fname);
	//��ѹ������DEV
	CString sCmd;
	sCmd.Format("7z.exe x %s DEV\\*.dev -o%s -aoa", gim_file, (char*)m_sOutPath);
	if (!CGimDataFile::Run7zCmd(sCmd.GetBuffer()))
		return FALSE;
	ATOM_LIST<CString> xDevFileList;
	CXhChar200 sFilePath("%s\\*", (char*)m_sOutPath);
	SearchFile(xDevFileList, sFilePath);
	int nFileNum = xDevFileList.GetNodeNum();
	for (int index = 0; index < nFileNum; index++)
	{
		CString fdPath = xDevFileList[index];
		CString str_ext = fdPath.Right(4);	//ȡ��׺��
		str_ext.MakeLower();
		if (str_ext.CompareNoCase(".dev") != 0)
			continue;
		if (!IsTowerDevFile(fdPath))
			continue;
		if(ReadDevFile(fdPath))
			break;
	}
	//��ѹ������FAM�ļ�
	CString file_str;
	file_str.Append(CXhChar50("DEV\\%s.fam ", (char*)m_sFamGuid));
	sCmd.Format("7z.exe x %s %s -o%s -aoa", gim_file, file_str.GetBuffer(), (char*)m_sOutPath);
	if (!CGimDataFile::Run7zCmd(sCmd.GetBuffer()))
		return FALSE;
	CXhChar200 sFamFile("%s\\DEV\\%s.fam",(char*)m_sOutPath,(char*)m_sFamGuid);
	ReadFamFile(sFamFile);
	//��ѹ������PHM�ļ�
	file_str.Empty();
	file_str.Append(CXhChar50("PHM\\%s.phm ", (char*)m_sPhmGuid));
	sCmd.Format("7z.exe x %s %s -o%s -aoa", gim_file, file_str.GetBuffer(), (char*)m_sOutPath);
	if (!CGimDataFile::Run7zCmd(sCmd.GetBuffer()))
		return FALSE;
	CXhChar200 sPhmFile("%s\\PHM\\%s.phm",(char*)m_sOutPath,(char*)m_sPhmGuid);
	ReadPhmFile(sPhmFile);
	//��ѹ������MOD�ļ�
	file_str.Empty();
	file_str.Append(CXhChar50("MOD\\%s.mod ", (char*)m_sName));
	sCmd.Format("7z.exe x %s %s -o%s -aoa", gim_file, file_str.GetBuffer(), (char*)m_sOutPath);
	if (!CGimDataFile::Run7zCmd(sCmd.GetBuffer()))
		return FALSE;
	m_sModFile.Printf("%s\\MOD\\%s.mod", (char*)m_sOutPath, (char*)m_sName);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void Test_WriteMod()
{
	CString filename=theApp.GetLDSDoc()->GetTitle();
	char* separator=SearchChar(filename.GetBuffer(),'.',true);
	if(separator)
	{
		*separator=0;
		filename.ReleaseBuffer();
	}
	CFileDialog file_dlg(false,"mod",filename,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"����ģ���ļ�(*.mod)|*.mod|�����ļ�(*.*)|*.*||");
	if(file_dlg.DoModal()!=IDOK)
		return;
	CModDataFile xModDataFile;
	xModDataFile.ExportModFile(file_dlg.GetPathName());
}
void Test_ReadMod()
{
	CFileDialog file_dlg(true,"mod","Tower",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"����ģ���ļ�(*.mod)|*.mod|�����ļ�(*.*)|*.*||");
	if(file_dlg.DoModal()!=IDOK)
		return;
	CModDataFile xModDataFile;
	xModDataFile.ImportModFile(file_dlg.GetPathName());
}
#endif

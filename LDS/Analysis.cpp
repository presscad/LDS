#include "stdafx.h"
#include "stdio.h"
#include "math.h"
#include "LDS.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "SortFunc.h"
#if defined(__LDS_)||defined(__TSA_)
#include "AnalyzeProcessor.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__LDS_)||defined(__TSA_)
struct PTPOS_INLINE 
{
	CLDSNode *pNode;
	double posLen;
};
static int compareNodeOrderFunc(const PTPOS_INLINE& item1,const PTPOS_INLINE& item2)
{
	if(item1.posLen>item2.posLen)
		return 1;
	else if(item1.posLen<item2.posLen)
		return -1;
	else
		return 0;
}
BOOL SelectNodeInPole(CLDSLinePart *pLinePart,NODESET& node_set)
{
	CLDSNode *pNode;
	PTPOS_INLINE nodePos;
	CArray<PTPOS_INLINE,PTPOS_INLINE&>nodeArr;
	CPtInLineCheck line3d(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false));
	BOOL bPush=Ta.Node.push_stack();
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode==pLinePart->pStart)
			nodePos.posLen=0;
		else if(pNode==pLinePart->pEnd)
			nodePos.posLen=1;
		// TODO: ��Ҫ�Ľ� wjh 2012.6.19
		else if(!pLinePart->cfgword.And(pNode->cfgword)||(pNode->hFatherPart!=pLinePart->handle&&
			pNode->arrRelationPole[0]!=pLinePart->handle&&pNode->arrRelationPole[1]!=pLinePart->handle))
			continue;	//�ڽڵ㸸�˼�Ӧ�������ڸ˼�,����ĺ�һ��.���д���������ʼ�ն˽ڵ��ж�֮��,����ᶪ���˽ڵ㸸�˼��Ǹ˼����
		else	//���ڵ���TTA����ʱ���ֵ�������ܲ���ȷ���²�һ���ϸ��ߣ��ʴ˴�ֻ����CheckPointEx,��������CheckPoint wjh-2017.3.7
			nodePos.posLen=line3d.CheckPointEx(pNode->Position(false));
		nodePos.pNode=pNode;
		if(nodePos.posLen>=-EPS&&nodePos.posLen<=1+EPS)
			nodeArr.Add(nodePos);
	}
	if(bPush)
		Ta.Node.pop_stack();
	CQuickSort<PTPOS_INLINE>::QuickSort(nodeArr.GetData(),nodeArr.GetSize(),compareNodeOrderFunc);
	node_set.Empty();
	for(int i=0;i<nodeArr.GetSize();i++)
		node_set.append(nodeArr[i].pNode);
	return TRUE;
}
BOOL SelectNodeInPole(f3dPoint lineStart,f3dPoint lineEnd,NODESET& node_set,NODESET *pSrNodeSet=NULL)
{
	CLDSNode *pNode;
	PTPOS_INLINE nodePos;
	CArray<PTPOS_INLINE,PTPOS_INLINE&>nodeArr;
	CPtInLineCheck line3d(lineStart,lineEnd);
	if(pSrNodeSet==NULL)
	{
		BOOL bPush=Ta.Node.push_stack();
		for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			nodePos.posLen=line3d.CheckPoint(pNode->Position(false));
			nodePos.pNode=pNode;
			if(nodePos.posLen>=-EPS&&nodePos.posLen<=1+EPS)
				nodeArr.Add(nodePos);
		}
		if(bPush)
			Ta.Node.pop_stack();
	}
	else
	{ 
		for(pNode=pSrNodeSet->GetFirst();pNode;pNode=pSrNodeSet->GetNext())
		{
			nodePos.posLen=line3d.CheckPoint(pNode->Position(false));
			nodePos.pNode=pNode;
			if(nodePos.posLen>=-EPS&&nodePos.posLen<=1+EPS)
				nodeArr.Add(nodePos);
		}
	}
	CQuickSort<PTPOS_INLINE>::QuickSort(nodeArr.GetData(),nodeArr.GetSize(),compareNodeOrderFunc);
	node_set.Empty();
	for(int i=0;i<nodeArr.GetSize();i++)
		node_set.append(nodeArr[i].pNode);
	return TRUE;
}
#endif
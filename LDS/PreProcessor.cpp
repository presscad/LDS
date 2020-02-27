// PreProcessor.cpp: implementation of the CPreProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LDS.h"
#include "PreProcessor.h"
#include "Query.h"
#include "WindLoad.h"
#include "env_def.h"
#include "SortFunc.h"
#include "GlobalFunc.h"
#include "EarthQuake.h"
#include "Expression.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if defined(__LDS_)||defined(__TSA_)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPreProcessor preprocessor;
CTsaTurboBuffer::CTsaTurboBuffer(long key) : CAttachBuffer(key)
{
	;
}

WINDSEGLOADCAL_EXPORT::WINDSEGLOADCAL_EXPORT()
{
	windSegI=0;	//风压段号
	cType=0;		//风压段类型0:八点塔身1:多点轮廓2:三角塔身
	cCalStyle=0;	//0.塔身方式计算1.X方向水平横担2.Y方向水平横担
	cMirType=0;	//对称信息0.无对称1.左右对称
	high=0;
	Wo=0;	//kN/m2
	A_a=A_b=Af_a=Af_b=0;	//m2
	fai_a=fai_b=0;
	b_a_a=b_a_b=yita_a=yita_b=1;
	muZ=muSa=muSb=betaZ=K=Cqi_a=Cqi_b=0;
	Wsa=Wsb=Wsc=0;	//kN
	Xcoef_Wsa=Xcoef_Wsb=Xcoef_Wsc=0;
	Ycoef_Wsa=Ycoef_Wsb=Ycoef_Wsc=0;
	Wx=Wy=0;
}

CWindSegElemPicker::CWindSegElemPicker()
{
	bHasTubeRod=false;
	m_fPrjAreaCoef=1.0;
	m_fWrapArea=m_fNetAreaOfAngle=m_fNetAreaOfTube=minZ=maxZ=0.0;
}

bool CWindSegElemPicker::InitElemPicker(NODESET &nodeset,f3dPoint prjOrg)
{
	double Af=0.0;	//净截面积
	CLDSNode *pNode=NULL, *pStart=NULL, *pEnd=NULL;
	f3dPoint pos,jg_vec;
	f3dLine line;
	int i;
	f3dPoint origin;
	if(nodeset.GetNodeNum()<3)
		return false;
	ARRAY_LIST<GEPOINT> legacyVertices;
	vertexList.SetSize(nodeset.GetNodeNum());
	legacyVertices.SetSize(nodeset.GetNodeNum());
	cfgword = nodeset.GetFirst()->cfgword;
	for(pNode=nodeset.GetFirst(),i=0;pNode;pNode=nodeset.GetNext(),i++)
	{
		cfgword&=pNode->UnifiedCfgword();//pNode->cfgword;
		legacyVertices[i] = vertexList[i] = pNode->Position(false);
		if(i==0)
			ucs.origin=vertexList[i];
		else if(ucs.axis_x.IsZero())
		{
			ucs.axis_x=vertexList[i]-vertexList[i-1];
			normalize(ucs.axis_x);
		}
		else if(ucs.axis_y.IsZero())
		{
			ucs.axis_y=vertexList[i]-vertexList[i-1];
			normalize(ucs.axis_y);
			if(fabs(ucs.axis_y*ucs.axis_x)>EPS_COS)	//与X轴共线为无效Y轴方向,清空继续寻找
				ucs.axis_y.Set();
			else
			{	//找到合理坐标系
				ucs.axis_z = ucs.axis_x^ucs.axis_y;
				ucs.axis_y = ucs.axis_z^ucs.axis_x;
				normalize(ucs.axis_y);
				normalize(ucs.axis_z);
			}
		}
	}
	//计算风压投影面
	UCS_STRU prjucs;	//风压段轮廓顶点投影面相对坐标系，用于计算构件单元的投影挡风面积
	prjucs.origin=ucs.origin;
	if(fabs(ucs.axis_z.x)>fabs(ucs.axis_z.y))
	{
		prjucs.axis_z.Set(1,0,0);
		prjucs.axis_x.Set(0,ucs.axis_x.y,ucs.axis_x.z);
	}
	else
	{
		prjucs.axis_z.Set(0,1,0);
		prjucs.axis_x.Set(ucs.axis_x.x,0,ucs.axis_x.z);
	}
	normalize(prjucs.axis_x);
	prjucs.axis_y=prjucs.axis_z^prjucs.axis_x;
	SCOPE_STRU scope;
	minZ=maxZ=0;
	for(i=0;i<nodeset.GetNodeNum();i++)
	{
		pos=vertexList[i];
		coord_trans(pos,prjucs,FALSE);
		scope.VerifyVertex(pos);
		coord_trans(vertexList[i],ucs,FALSE);
		if(minZ>vertexList[i].z)
			minZ=vertexList[i].z;
		if(maxZ<vertexList[i].z)
			maxZ=vertexList[i].z;
		vertexList[i].z=0;
	}
	//计算多边形轮廓总面积
	//削除多边形凹角
	int n=vertexList.GetSize();
	for(i=0;i<n;i++)
	{
		double result=DistOf2dPtLine(vertexList[(i+2)%n],vertexList[i],vertexList[(i+1)%n]);
		if(result<0)	// 后点在线右侧，有凹角出现
		{
			vertexList.RemoveAt((i+1)%n);
			n--;
		}
	}
	m_fWrapArea=0;
	geomCenter.Set();
	double weight_deitax=0,cosaPrjZ=fabs(ucs.axis_z*prjucs.axis_z);
	for(i=1;i<vertexList.GetSize()-1;i++)
	{
		double area=CalTriArea(vertexList[0].x,vertexList[0].y,vertexList[i].x,vertexList[i].y,vertexList[i+1].x,vertexList[i+1].y);
		m_fWrapArea+=area;
		geomCenter+=(vertexList[0]+vertexList[i]+vertexList[i+1])*(area/3.0);
		double highz=min(legacyVertices[0].z,legacyVertices[1].z);
		highz=min(highz,legacyVertices[2].z);
		double lowz=max(legacyVertices[0].z,legacyVertices[1].z);
		lowz=max(lowz,legacyVertices[2].z);
		if((lowz-highz)>0)	//*cosaPrjZ:用于考虑挡风面积在Ｚ轴向的投影
			weight_deitax+=area*cosaPrjZ*area/(lowz-highz);
	}
	if(m_fWrapArea<EPS)
		return false;
	geomCenter/=m_fWrapArea;
	coord_trans(geomCenter,ucs,TRUE,TRUE);
	
	m_fPrjAreaCoef=fabs(prjucs.axis_z*ucs.axis_z)/1000000;	//由mm2->m2
	double a=weight_deitax/m_fWrapArea;
	m_fWrapArea*=m_fPrjAreaCoef;
	
	//计算b_a
	//经仔细研判《建筑结构荷载规范》中约定的宽度，实际指每榀迎风面长宽中较窄的尺度，并非指水平宽度　wjh-2017.3.1
	double h=m_fWrapArea*1e6/a;
	if(a>EPS&&h<a)
		a=h;
	else if(a<EPS)
	{
		if(scope.wide()>scope.high())
			a=m_fWrapArea/scope.wide()*1e6;
		else
			a=m_fWrapArea/scope.high()*1e6;
	}
	coord_trans(prjOrg,prjucs,FALSE);
	double b=fabs(2*prjOrg.z-(scope.fMaxZ+scope.fMinZ));
	b_a=b/a;
	return true;
}
void CWindSegElemPicker::StartElemPicker(ATOM_LIST<CElement> &allElemSet,CLDSModule *pModule/*=NULL*/)
{
	int n=vertexList.GetSize();
	cfgword.Clear();
	elemSet.Empty();
	CElement *pFemElem;
	for(pFemElem=allElemSet.GetFirst();pFemElem;pFemElem=allElemSet.GetNext())
	{
		if(pModule&&!pModule->IsSonPart(pFemElem->pOrgPole))
			continue;
		f3dPoint start=pFemElem->pStart->Position(false);
		f3dPoint end=pFemElem->pEnd->Position(false);
		coord_trans(start,ucs,FALSE,TRUE);
		coord_trans(end,ucs,FALSE,TRUE);
		bool bInternal=false;
		if(start.z>=minZ-EPS&&start.z<=maxZ+EPS&&end.z>=minZ-EPS&&end.z<=maxZ+EPS)
		{
			for(int i=0;i<n;i++)
			{
				f3dPoint v1=vertexList[(i+1)%n]-vertexList[i];
				f3dPoint v2=start-vertexList[i];
				f3dPoint v3=end-vertexList[i];
				double L1=v1.mod();
				double L2=v2.mod();
				double L3=v3.mod();
				if(L2>EPS&&v1.x*v2.y-v1.y*v2.x<-EPS*L1*L2) 
					break;	//起点在某条边的右侧故不是凸多边形的内点
				if(L3>EPS&&v1.x*v3.y-v1.y*v3.x<-EPS*L1*L3) 
					break;	//终点在某条边的右侧故不是凸多边形的内点
			}
			if(i==n)
				bInternal=true;
		}
		if(bInternal)
		{
			elemSet.append(pFemElem);
			if(cfgword.IsNull())
				cfgword=pFemElem->cfgword;
			else
				cfgword&=pFemElem->cfgword;
		}
	}
	bHasTubeRod=false;
	m_fNetAreaOfAngle=m_fNetAreaOfTube=0;
	for(pFemElem=elemSet.GetFirst();pFemElem;pFemElem=elemSet.GetNext())
	{
		if(pFemElem->pOrgPole->size_idClassType==CLS_LINETUBE)
			bHasTubeRod=true;	//有钢管时构件体型系数需要根据风压单独计算
		double A=pFemElem->Length()*pFemElem->pOrgPole->size_wide;;
		if(pFemElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE&&	//对角对称双角钢组合
			(pFemElem->pOrgPole->size_cSubClassType=='D'||pFemElem->pOrgPole->size_cSubClassType=='X'))//十字对称四角钢组合
			A*=2;//*pFemElem->Length()*pFemElem->pOrgPole->size_wide;
		if(pFemElem->pOrgPole->size_idClassType==CLS_LINETUBE)
			m_fNetAreaOfTube+=A;
		else
			m_fNetAreaOfAngle+=A;
	}
}
double CWindSegElemPicker::CalWindSegAf(double W,int iLoadExecuteCode,double *weighted_coef,WINDSEGLOADCAL_EXPORT *pExport/*=NULL*/)
{
	double Af=m_fNetAreaOfAngle+m_fNetAreaOfTube;
	if(weighted_coef==NULL)
		return Af;
	double weightedK=0;
	if(bHasTubeRod)
	{	//避免大多数情况下的多余遍历消耗时间
		for(CElement *pFemElem=elemSet.GetFirst();pFemElem;pFemElem=elemSet.GetNext())
		{
			if(pFemElem->pOrgPole->size_idClassType==CLS_LINETUBE)
			{
				double A=pFemElem->Length()*pFemElem->pOrgPole->size_wide;;
				double d=pFemElem->pOrgPole->size_wide;
				double W0d2=W*d*d*1e-6;
				double coef1,coef2,critical1=0.002,critical2=0.015;
				if(iLoadExecuteCode==1)
				{
					critical1=0.003;
					critical2=0.02;
				}
				if(iLoadExecuteCode==0)
				{
					coef1=0.7/1.3;
					coef2=1.2/1.3;
				}
				else //if(iLoadExecuteCode==1||iLoadExecuteCode==2)
				{
					coef1=0.6;
					coef2=0.8;
				}
				double coef;
				if(W0d2<=critical1)
					coef=coef2;
				else if(W0d2>=critical2)
					coef=coef1;
				else
					coef=(coef1+(0.015-W0d2)*(coef2-coef1)/0.013);
				weightedK+=A*coef;
				if(pExport)
				{
					int startPointI=min(pFemElem->pStart->point_i,pFemElem->pEnd->point_i);
					int endPointI  =max(pFemElem->pStart->point_i,pFemElem->pEnd->point_i);
					WINDSEGLOADCAL_EXPORT::ELEMREC* pRec=pExport->elemrecs.GetValue(startPointI,endPointI);
					if(pRec)
					{
						pRec->muS=coef;
						pRec->efficArea=pRec->area*pRec->muS;
					}
				}
			}
		}
	}
	weightedK+=1.3*m_fNetAreaOfAngle;
	if(fabs(Af)<EPS)
		*weighted_coef=1.3;
	else
		*weighted_coef=weightedK/Af;
	return Af;
}

IMemberDesignCode* CPreProcessor::ActiveDesignCode=IMemberDesignCode::FromCodeSerial(0);
//from Tower.cpp
DWORD Hash2Key(DWORD key1,DWORD key2,DWORD nHashSize);
CPreProcessor::CPreProcessor()
{
	m_pTower=NULL;
	m_pAnalysisProcessor=NULL;
	result.bHasSwingNode=true;
	result.hashElemsMap.SetHashFunc(Hash2Key);
	m_bIncEiffelFactorX=m_bIncEiffelFactorY=true;
	pTurboBuffer=NULL;
	LOWEST_ROD_POSZ_INITED=false;
	LOWEST_ROD_POSZ=0.0;
}

CPreProcessor::~CPreProcessor()
{

}
void CPreProcessor::VerifyEiffelFactor(CTower* pTower)
{
	jointx_z_arr.Empty();
	jointy_z_arr.Empty();
	if(pTower->m_cStructureType==1)	//变电构架不需要考虑埃菲尔效应
		m_bIncEiffelFactorX=m_bIncEiffelFactorY=false;
	if(!m_bIncEiffelFactorX&&!m_bIncEiffelFactorY)
		return;
	ATOM_LIST<f3dLine>slopeset;
	ARRAY_LIST<GEPOINT2D>joint_z_arr;
	int i,n=pTower->GetSlopeList(joint_z_arr,&slopeset);
	jointx_z_arr.SetSize(joint_z_arr.GetSize());
	jointy_z_arr.SetSize(joint_z_arr.GetSize());
	for(i=0;i<joint_z_arr.GetSize();i++)
	{
		jointx_z_arr[i]=joint_z_arr[i].x;
		jointy_z_arr[i]=joint_z_arr[i].y;
	}
	if(n<2)	//单坡度塔身
		m_bIncEiffelFactorX=m_bIncEiffelFactorY=false;
	else if(n==2)
	{
		f3dLine *pTopSlope=slopeset.GetFirst();
		if(joint_z_arr[0].x<pTopSlope->startPt.z)
			m_bIncEiffelFactorX=false;
		if(joint_z_arr[0].y<pTopSlope->startPt.z)
			m_bIncEiffelFactorY=false;
	}
}
//from Analysis.cpp
BOOL SelectNodeInPole(CLDSLinePart *pLinePart,NODESET& node_set);
CProcessBodyLoad CPreProcessor::GetStartBodyLoad(CElement *pElem,CLDSModule *pActiveModule/*=NULL*/)
{
	CProcessBodyLoad start_load,end_load,tem_load;
	if(result.hashElemStartBodyLoad.GetValueAt(pElem->id,start_load))
	{
		if(pActiveModule==NULL||pActiveModule->IsSonPart(pElem->pOrgPole,&start_load.cfgword))
			return start_load;
	}
	//CPtInLineCheck lineCheck(pElem->pStart->pos,pElem->pEnd->pos);
	start_load = end_load = pElem->BodyLoad()*0.5;
	BOOL bPush=result.allnodeset.push_stack();
	//此处必须是result.allnodeset，否则会丢掉辅材端点上的重力荷载
	for(CLDSNode* pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
	{	//不能用Nodeset遍历因为其中不包含补助材端点
		if(pActiveModule!=NULL&&!pActiveModule->IsSonPart(pNode))
			continue;	//不是当前模型中当前接腿塔上的节点
		if(pNode==pElem->pStart||pNode==pElem->pEnd)
			continue;
		else if(!pElem->cfgword.And(pNode->cfgword))
			continue;	//不在同一模型中出现
		//if(pNode->RootFatherPartHandle()!=pElem->pOrgPole->handle)
		//	continue;	//不能通过父杆件判断，因为存在组合角钢及交叉材等特殊情况存在
		double proportion=pElem->CheckPoint(pNode->Position(false));
		if(proportion<EPS||proportion>1-EPS)	//点不在线段内,或为端节点
			continue;
		BOOL bPush2=result.allnodeset.push_stack();
		tem_load = GetBodyLoad(pNode,pActiveModule);
		if(bPush2)
			result.allnodeset.pop_stack();
		//double len = pElem->Length();
		//double fraction = DISTANCE(pNode->pos,pElem->pEnd->pos);
		if(pNode->m_cPosCalType==4&&pNode->arrRelationPole[0]>0x20&&pNode->arrRelationPole[1]>0x20)
		{	//交叉点属于共用点，所以分配质量应减半
			start_load += tem_load*(0.5-0.5*proportion);//(0.5*fraction/len);//
			end_load += tem_load*(0.5*proportion);
		}
		else
		{
			start_load += tem_load*(1-proportion);//(fraction/len);
			end_load += tem_load*proportion;//(1-fraction/len);
		}
	}
	if(bPush)
		result.allnodeset.pop_stack();
	if(pActiveModule==NULL&&!start_load.cfgword.IsNull())
	{
		result.hashElemStartBodyLoad.SetValueAt(pElem->id,start_load);
		result.hashElemEndBodyLoad.SetValueAt(pElem->id,end_load);
	}
	return start_load;
}
CProcessBodyLoad CPreProcessor::GetEndBodyLoad(CElement *pElem,CLDSModule *pActiveModule/*=NULL*/)
{
	CProcessBodyLoad start_load,end_load,tem_load;
	if(result.hashElemEndBodyLoad.GetValueAt(pElem->id,end_load))
	{
		if(pActiveModule==NULL||pActiveModule->IsSonPart(pElem->pOrgPole,&end_load.cfgword))
			return end_load;
	}
	//CPtInLineCheck lineCheck(pElem->pStart->pos,pElem->pEnd->pos);
	start_load = end_load = pElem->BodyLoad()*0.5;
	BOOL bPush=result.allnodeset.push_stack();
	//此处必须是result.allnodeset，否则会丢掉辅材端点上的重力荷载
	for(CLDSNode* pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
	{	//不能用Nodeset遍历因为其中不包含补助材端点
		if(pActiveModule!=NULL&&!pActiveModule->IsSonPart(pNode))
			continue;	//不是当前模型中当前接腿塔上的节点
		if(pNode==pElem->pStart||pNode==pElem->pEnd)
			continue;
		else if(!pElem->cfgword.And(pNode->cfgword))
			continue;
		//if(pNode->RootFatherPartHandle()!=pElem->pOrgPole->handle)
		//	continue;	//不能通过父杆件判断，因为存在组合角钢及交叉材等特殊情况存在
		double proportion=pElem->CheckPoint(pNode->Position(false));
		if(proportion<EPS||proportion>1-EPS)	//点不在线段内,或为端节点
			continue;
		BOOL bPush2=result.allnodeset.push_stack();
		tem_load = GetBodyLoad(pNode,pActiveModule);
		if(bPush2)
			result.allnodeset.pop_stack();
		//double len = pElem->Length();
		//double fraction = DISTANCE(pNode->pos,pElem->pStart->pos);
		if(pNode->m_cPosCalType==4&&pNode->arrRelationPole[0]>0x20&&pNode->arrRelationPole[1]>0x20)
		{	//交叉点属于共用点，所以分配质量应减半
			start_load += tem_load*(0.5-0.5*proportion);
			end_load += tem_load*(0.5*proportion);
		}
		else
		{
			start_load += tem_load*(1-proportion);//(1-fraction/len);
			end_load += tem_load*proportion;//(fraction/len);
		}
	}
	if(bPush)
		result.allnodeset.pop_stack();
	if(pActiveModule==NULL&&!end_load.cfgword.IsNull())
	{
		result.hashElemStartBodyLoad.SetValueAt(pElem->id,start_load);
		result.hashElemEndBodyLoad.SetValueAt(pElem->id,end_load);
	}
	return end_load;
}
CProcessBodyLoad CPreProcessor::GetBodyLoad(CLDSNode *pNode,CLDSModule *pActiveModule/*=NULL*/)
{
	CProcessBodyLoad selfload;
	if(result.hashNodeBodyLoad.GetValueAt(pNode->handle,selfload))
	{
		if(pActiveModule==NULL||pActiveModule->IsSonPart(pNode,&selfload.cfgword))
			return selfload;
	}
	selfload.cfgword=pNode->cfgword;
	CNodeBodyLoad* pBodyLoad=result.hashNodeBodyLoads.GetValue(pNode->handle);
	if(pBodyLoad!=NULL)
	{
		for(ELEM_BODYLOAD_ALLOC* pAlloc=pBodyLoad->hashElemAllocs.GetFirst();pAlloc;pAlloc=pBodyLoad->hashElemAllocs.GetNext())
		{
			if(pActiveModule&&!pActiveModule->IsSonPart(pAlloc->pElem->pOrgPole))
				continue;
			selfload+=pAlloc->pElem->BodyLoad()*pAlloc->fAllocCoef;
			selfload.cfgword&=pAlloc->pElem->cfgword;
			//指定当前计算模型时，偶而在多接腿与塔身交接处可能会存在配材号不匹配情况（因身腿不一致），
			//此时即然已通过了IsSonPart的模型校验，继续计算依然是合理的，但由于不具有共用性，因此不
			//进行哈希表存储 wjh-2013-2-08
			if(pActiveModule==NULL&&selfload.cfgword.IsNull())
				break;	//计算过程中没有出现共有的配材号，无法完成汇总，只能在分析时即时分析体荷载
		}
	}
	/*else 不是任何单元的端节点,如交叉点
	{
		CNodeLinkElems *pNodeLink=result.hashNodeLinks.GetValue(pNode->handle);
		if(pNodeLink==NULL)
			return selfload;
		for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{
			if(pActiveModule&&!pActiveModule->IsSonPart(pElem->pOrgPole))
				continue;
			BOOL bPush=result.elemList.push_stack();
			if(pElem->pStart==pNode)
				selfload += GetStartBodyLoad(pElem,pActiveModule);
			else if(pElem->pEnd==pNode)
				selfload += GetEndBodyLoad(pElem,pActiveModule);
			if(bPush)
				result.elemList.pop_stack();
			//指定当前计算模型时，偶而在多接腿与塔身交接处可能会存在配材号不匹配情况（因身腿不一致），
			//此时即然已通过了IsSonPart的模型校验，继续计算依然是合理的，但由于不具有共用性，因此不
			//进行哈希表存储 wjh-2013-2-08
			if(pActiveModule==NULL&&selfload.cfgword.IsNull())
				break;	//计算过程中没有出现共有的配材号，无法完成汇总，只能在分析时即时分析体荷载
		}
	}
	// **以下为新算法正确性的测试代码
	if(pBodyLoad!=NULL&&!pBodyLoad->bBraceNode)
	{
		if(!selfload2.cfgword.IsEqual(selfload.cfgword))
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"Internal error #3! @%d node",pNode->pointI);
		else if(selfload2.cfgword.IsNull()&&selfload.cfgword.IsNull())
			return selfload;	//空配材号（可能是腿身换接处）没有必要存储到哈希表中
		if(fabs(selfload2.selfWeightLoad-selfload.selfWeightLoad)>0.001)
		{
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"Internal error #4! @%d node %.3f!=%.3f braceNode=%d",pNode->pointI,selfload2.selfWeightLoad,selfload.selfWeightLoad,pBodyLoad->bBraceNode);
			for(ELEM_BODYLOAD_ALLOC* pAlloc=pBodyLoad->hashElemAllocs.GetFirst();pAlloc;pAlloc=pBodyLoad->hashElemAllocs.GetNext())
			{
				if(pActiveModule&&!pActiveModule->IsSonPart(pAlloc->pElem->pOrgPole))
					continue;
				logerr.Log("%d-%d weight=%.2f alloc %.2f%%",pAlloc->pElem->pStart->pointI,pAlloc->pElem->pEnd->pointI,pAlloc->pElem->BodyLoad().selfWeightLoad,pAlloc->fAllocCoef);
			}
		}
		if(selfload2.iceWeightLoadCoef!=selfload.iceWeightLoadCoef||selfload2.iceWeightLoadCoef2!=selfload.iceWeightLoadCoef2)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"Internal error #5! @%d node",pNode->pointI);
	}*/
	if(pActiveModule==NULL&&!selfload.cfgword.IsNull())
		result.hashNodeBodyLoad.SetValueAt(pNode->handle,selfload);
	return selfload;
}
int compareNodeOrderByPointI(const CLDSNodePtr& pNode1,const CLDSNodePtr& pNode2){return pNode1->point_i-pNode2->point_i;}
bool IsMirLine(f3dPoint start1,f3dPoint end1,f3dPoint start2,f3dPoint end2,int mir_x1_y2_z3)
{
	if( mir_x1_y2_z3==1&&	//X轴对称
	  ((fabs(start1.x-start2.x)<EPS&&fabs(end1.x-end2.x)<EPS&&
		fabs(start1.y+start2.y)<EPS&&fabs(end1.y+end2.y)<EPS&&
		fabs(start1.z-start2.z)<EPS&&fabs(end1.z-end2.z)<EPS)||
	   (fabs(start1.x-end2.x)<EPS&&fabs(end1.x-start2.x)<EPS&&
		fabs(start1.y+end2.y)<EPS&&fabs(end1.y+start2.y)<EPS&&
		fabs(start1.z-end2.z)<EPS&&fabs(end1.z-start2.z)<EPS)))
		return true;
	if( mir_x1_y2_z3==2&&	//Y轴对称
	  ((fabs(start1.x+start2.x)<EPS&&fabs(end1.x+end2.x)<EPS&&
		fabs(start1.y-start2.y)<EPS&&fabs(end1.y-end2.y)<EPS&&
		fabs(start1.z-start2.z)<EPS&&fabs(end1.z-end2.z)<EPS)||
	   (fabs(start1.x+end2.x)<EPS&&fabs(end1.x+start2.x)<EPS&&
		fabs(start1.y-end2.y)<EPS&&fabs(end1.y-start2.y)<EPS&&
		fabs(start1.z-end2.z)<EPS&&fabs(end1.z-start2.z)<EPS)))
		return true;
	if( mir_x1_y2_z3==3&&	//Z轴对称
	  ((fabs(start1.x+start2.x)<EPS&&fabs(end1.x+end2.x)<EPS&&
		fabs(start1.y+start2.y)<EPS&&fabs(end1.y+end2.y)<EPS&&
		fabs(start1.z-start2.z)<EPS&&fabs(end1.z-end2.z)<EPS)||
	   (fabs(start1.x+end2.x)<EPS&&fabs(end1.x+start2.x)<EPS&&
		fabs(start1.y+end2.y)<EPS&&fabs(end1.y+start2.y)<EPS&&
		fabs(start1.z-end2.z)<EPS&&fabs(end1.z-start2.z)<EPS)))
		return true;
	return false;
}

//避免乘法带来的效率低下
bool IsAntiSign(double f1,double f2)
{
	if((f1<0&&f2>0) || (f1>0&&f2<0))
		return true;
	else
		return false;
}
struct RODMIR_INFO{
	CLDSLinePart *pRod;	//主杆件
	CLDSLinePart *pRodMirX,*pRodMirY,*pRodMirZ;
	RODMIR_INFO(){memset(this,0,sizeof(RODMIR_INFO));}
};
//根据对称原则提取出输入单元所对应的父单元
CElement* CPreProcessor::FindFatherElem(CElement* pElem)
{
	CElement* pFatherElem=result.hashFatherElemPtrs.GetValue(pElem->id);
	if(pFatherElem!=NULL)
		return pFatherElem;
	if(pElem->relaElem.idElemMirX>0&&(pFatherElem=result.hashFatherElemPtrs.GetValue(pElem->relaElem.idElemMirX)))
		return pFatherElem;
	if(pElem->relaElem.idElemMirY>0&&(pFatherElem=result.hashFatherElemPtrs.GetValue(pElem->relaElem.idElemMirY)))
		return pFatherElem;
	if(pElem->relaElem.idElemMirZ>0&&(pFatherElem=result.hashFatherElemPtrs.GetValue(pElem->relaElem.idElemMirZ)))
		return pFatherElem;
	return NULL;
}
void CPreProcessor::RetrieveFatherElemHashSet(){
	result.hashFatherElemPtrs.Empty();
	CElement* pElem,*pFatherElem;
	for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		pFatherElem=result.hashFatherElemPtrs.GetValue(pElem->id);
		if(pFatherElem==NULL)
		{
			if((pElem->relaElem.idElemMirX>0&&(pFatherElem=result.hashFatherElemPtrs.GetValue(pElem->relaElem.idElemMirX)))||
			   (pElem->relaElem.idElemMirY>0&&(pFatherElem=result.hashFatherElemPtrs.GetValue(pElem->relaElem.idElemMirY)))||
			   (pElem->relaElem.idElemMirZ>0&&(pFatherElem=result.hashFatherElemPtrs.GetValue(pElem->relaElem.idElemMirZ))))
			{
				if(pFatherElem->CompareElemPointI(pElem)>0)
				{
					result.hashFatherElemPtrs.DeleteNode(pFatherElem->id);
					pFatherElem=result.hashFatherElemPtrs.SetValue(pElem->id,pElem);
				}
			}
		}
		if(pFatherElem==NULL)
			pFatherElem=result.hashFatherElemPtrs.SetValue(pElem->id,pElem);
	}
}
//计算验证(父)单元是否有足够支撑
void CPreProcessor::ValidateElemSupportStiff()
{
	for(CElement* pElem=result.hashFatherElemPtrs.GetFirst();pElem;pElem=result.hashFatherElemPtrs.GetNext())
	{
		if(pElem->BearTens1Comp2==1||pElem->BearTens1Comp2==2)	//零杆
			continue;
		//计算是否可提供足够的转动约束刚度
		if( pElem->pOrgPole->IsAuxPole()||	!pElem->pOrgPole->IsPostRod())//辅材或非主材压杆单元
		{	//计算受压杆是否有合法的刚性约束
			f3dPoint base_vec;
			if(pElem->start_joint_type==JOINT_RIGID)
				pElem->m_bValidStartRestStiff=TRUE;
			else if(pElem->start_joint_type==JOINT_HINGE)
				pElem->m_bValidStartRestStiff=FALSE;
			else if(pElem->start_joint_type==JOINT_AUTOJUSTIFY&&(pElem->pStart!=pElem->pOrgPole->pStart||pElem->pOrgPole->connectStart.wnConnBoltN>=2))
			{	//此杆件需要其它非共线父角钢单元提供约束
				CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pElem->pStart->handle);
				double support_stiff=0;
				double rest_stiff=pElem->GetWorkI()/pElem->Length();
				if(pNodeLinks)
				{
					CLDSLinePart *pFatherRod=(CLDSLinePart*)Ta.Parts.FromHandle(pElem->pStart->hFatherPart);
					if(pFatherRod&&pFatherRod->IsLinePart())
						base_vec=pFatherRod->pEnd->Position(false)-pFatherRod->pStart->Position(false);
					if(pFatherRod&&pFatherRod->IsLinePart())
					{
						normalize(base_vec);
						f3dPoint work_norm=pElem->LenStdVec()^base_vec;
						normalize(work_norm);
						for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
						{
							if(pElemStiff->pElem->id==pElem->id)
								continue;
							else if(pElem->cfgword.And(pElemStiff->pElem->cfgword))
							{	//当前计算模型内的单元
								if(pElemStiff->pElem->pOrgPole==pFatherRod)
								{
									if(pElemStiff->pElem->pStart==pElem->pStart||pElemStiff->pElem->pEnd==pElem->pStart)
										support_stiff+=pElemStiff->stiff;	//判断此单元是否为当前模型中单元
									else
										support_stiff+=pElemStiff->stiff*4;	//节点在单元中间支撑刚度乘4 2*I/0.5L=4*I/L
								}
								else if(fabs(pElemStiff->pElem->LenStdVec()*base_vec)>EPS_COS)	//提供约束的单元
									support_stiff+=pElemStiff->stiff;
								else if(fabs(pElemStiff->pElem->LenStdVec()*work_norm)<EPS)	//工作面内被约束的单元
									rest_stiff+=pElemStiff->stiff;
							}
						}
					}
				}
				if(support_stiff>=rest_stiff)
					pElem->m_bValidStartRestStiff=TRUE;
				else
					pElem->m_bValidStartRestStiff=FALSE;
			}
			else
				pElem->m_bValidStartRestStiff=FALSE;
			if(pElem->end_joint_type==JOINT_RIGID)
				pElem->m_bValidEndRestStiff=TRUE;
			else if(pElem->end_joint_type==JOINT_HINGE)
				pElem->m_bValidEndRestStiff=FALSE;
			else if(pElem->end_joint_type==JOINT_AUTOJUSTIFY&&(pElem->pEnd!=pElem->pOrgPole->pEnd||pElem->pOrgPole->connectEnd.wnConnBoltN>=2))
			{	//此杆件需要其它非共线父角钢单元提供约束
				CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pElem->pEnd->handle);
				double support_stiff=0;
				double rest_stiff=0;
				if(pNodeLinks)
				{
					CLDSLinePart *pFatherRod=(CLDSLinePart*)Ta.Parts.FromHandle(pElem->pEnd->hFatherPart);
					if(pFatherRod&&pFatherRod->IsLinePart())
						base_vec=pFatherRod->pEnd->Position(false)-pFatherRod->pStart->Position(false);
					if(pFatherRod&&pFatherRod->IsLinePart())
					{
						normalize(base_vec);
						f3dPoint work_norm=pElem->LenStdVec()^base_vec;
						normalize(work_norm);
						for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
						{
							if(pElemStiff->pElem->id==pElem->id)
								continue;
							else if(pElem->cfgword.And(pElemStiff->pElem->cfgword))
							{	//当前计算模型内的单元
								if(pElemStiff->pElem->pOrgPole==pFatherRod)
								{
									if(pElemStiff->pElem->pStart==pElem->pEnd||pElemStiff->pElem->pEnd==pElem->pEnd)
										support_stiff+=pElemStiff->stiff;	//判断此单元是否为当前模型中单元
									else
										support_stiff+=pElemStiff->stiff*4;	//节点在单元中间支撑刚度乘4 2*I/0.5L=4*I/L
								}
								else if(fabs(pElemStiff->pElem->LenStdVec()*base_vec)>EPS_COS)	//提供约束的单元
									support_stiff+=pElemStiff->stiff;
								else if(fabs(pElemStiff->pElem->LenStdVec()*work_norm)<EPS)	//工作面内被约束的单元
									rest_stiff+=pElemStiff->stiff;
							}
						}
					}
				}
				if(support_stiff>=rest_stiff)
					pElem->m_bValidEndRestStiff=TRUE;
				else
					pElem->m_bValidEndRestStiff=FALSE;
			}
			else
				pElem->m_bValidEndRestStiff=FALSE;
		}
	}
}
//自动将集中力加载点及风压轮廓加载点判断为传力节点，即进行CLDSNode::feature=1;临时赋值
void CPreProcessor::SmartJustifyNodeForceType(CTower *pTower,NODESET& nodeset)
{
	CLDSNode* pNode;
	NODESET filter;
	bool hasSwingNode=false;
	for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{	//建立目前为自动判断传力类型的节点集合
		if(pNode->feature==0)
			filter.append(pNode);
		if(pNode->force_type==SWING_NODE)
			hasSwingNode=true;	//摆动节点
	}
	for(pNode=filter.GetFirst();pNode;pNode=filter.GetNext())
	{
		if(pNode->feature!=0)
			continue;	//目前已不是自动判断传力类型节点，可能已处理过
		for(CWorkStatus* pCase=pTower->WorkStatus.GetFirst();pCase;pCase=pTower->WorkStatus.GetNext())
		{
			CExternalNodeLoad *pNodeLoad=pCase->hashNodeLoad.GetValue(pNode->handle);
			if(pNodeLoad!=NULL&&(!pNodeLoad->Fxyz.IsZero()||pNodeLoad->permanentFz!=0))
			{
				pNode->feature=1;	//集中荷载加载点设为传力节点
				break;
			}
		}

		for(CWindSegment* pSeg=pTower->WindLoadSeg.GetFirst();pSeg;pSeg=pTower->WindLoadSeg.GetNext())
		{
			for(int i=0;i<pSeg->GetMaxNodeCount();i++)
			{
				if(pNode->handle==pSeg->nodeArr[i])
				{
					pNode->feature=1;	//风压段轮廓加载点设为传力节点
					break;
				}
			}
			if(pNode->feature==1)	//风压段轮廓加载点设为传力节点
				break;
		}
		for(CAttachWindLoad* pAttachWindLoad=pTower->AttachWindLoad.GetFirst();pAttachWindLoad;pAttachWindLoad=pTower->AttachWindLoad.GetNext())
		{
			for(long *pNodeH=pAttachWindLoad->GetNodeList()->GetFirst();pNodeH;pNodeH=pAttachWindLoad->GetNodeList()->GetNext())
			{
				if(pNode->handle==*pNodeH)
				{
					pNode->feature=1;	//附加风压段轮廓加载点设为传力节点
					break;
				}
			}
			if(pNode->feature==1)	//风压段轮廓加载点设为传力节点
				break;
		}
	}
	if(!hasSwingNode)
		return;	//无摆动节点
	for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{
		if(pNode->force_type!=SWING_NODE)
			continue;	//非摆动节点
		CLDSLinePart *pRod=NULL,*pRod1=NULL,*pRod2=NULL;
		for(pRod=pTower->EnumRodFirst();pRod;pRod=pTower->EnumRodNext())
		{//result.allrodset中不包含摆动节点的关联杆件,故只能使用m_pTower->Parts遍历 wjh-2017.3.26
			CLDSLinePart* pConnRod=NULL;
			if(pRod->pStart==pNode||pRod->pEnd==pNode)
				pConnRod=pRod;
			if(pConnRod&&pRod1==NULL)
				pRod1=pRod;
			else if(pConnRod&&pRod2==NULL)
				pRod2=pRod;
			else if(pRod1!=NULL&&pRod2!=NULL&&pConnRod)
			{
				logerr.Log("0X%X节点摆动节点关联杆件数大于2, 请先核查后再进行力学分析!",pNode->handle);
				break;
			}
		}
		if(pRod1!=NULL)
			(pRod1->pStart==pNode) ? pRod1->pEnd->feature=1 : pRod1->pStart->feature=1;
		if(pRod2!=NULL)
			(pRod2->pStart==pNode) ? pRod2->pEnd->feature=1 : pRod2->pStart->feature=1;
	}
}
//返回考虑埃菲尔效应时的坡段数
int CPreProcessor::GetEiffelSlopes(bool bOnlyValid/*=true*/,CWorkStatus* pVerifyStatus/*=NULL*/)
{
	char cAxis=0;
	if(pVerifyStatus)
	{
		if(pVerifyStatus->m_cMainLoadType==0&&pVerifyStatus->WindLoad.azimuth%180==0)
			cAxis='Y';	//Y轴向风
		else if(pVerifyStatus->m_cMainLoadType==0&&pVerifyStatus->WindLoad.azimuth%90==0)
			cAxis='X';	//X轴向风
		else
			return 0;	//无效风向角或非大风工况
	}
	if( bOnlyValid&&(
		(cAxis==0&&!m_bIncEiffelFactorX&&!m_bIncEiffelFactorY)||
		(cAxis=='X'&&!m_bIncEiffelFactorX)||
		(cAxis=='Y'&&!m_bIncEiffelFactorY)))
		return 0;
	else
		return (int)jointx_z_arr.GetSize();
}
//TODO:(2)[加载计算模型的加速缓存]在PrepareTower中快速继承过往分析获得模型中稳固不常变的结构信息
bool CPreProcessor::InheritStableModelInfo(CBuffer& buffer)
{
	return false;
	//1.更新单元对称信息
		//CHashList<RODMIR_INFO>hashRodMirInfo;
		//更新单元的解析及对称信息
		//RetrieveFatherElemHashSet();	//提取父单元，以便简化计算单元是否有足够支撑
	//2.改进GetBodyLoad
	//3.改进InitElemLamdaInfo(loadSrcDesignElemInfo);
	//4.自动将集中力加载点及风压轮廓加载点判断为传力节点
		//SmartJustifyNodeForceType
	//5.增加呼高-工况荷载计算施加缓存
	return true;
}
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::PrepareTower">
bool CPreProcessor::PrepareTower(CTower *pTower,const CFGWORD& cfgwordBody,const CFGWORD& cfgwordLeg,bool bApplyLoad/*=true*/,bool loadSrcDesignElemInfo/*=true*/)
{
	LINEPARTSET poleset;
	CElement *pElem;
	CFGWORD bodyCfgWord;
	CLDSNode *pNode;
	CLDSLinePart *pLinePart;
	CRodIncIntervals *pRodIntervals;
	m_pTower=pTower;
	result.cfgwordBody=cfgwordBody;
	result.cfgwordLeg=cfgwordLeg;
	//校验是否需要考虑塔身斜材的埃菲尔效应
	m_bIncEiffelFactorX=m_bIncEiffelFactorY=ActiveDesignCode->IsIncEiffelEffect();
	VerifyEiffelFactor(pTower);
	pTurboBuffer=Ta.AttachBuffer.GetValue(CTower::TSA_TURBO_MODEL);
	//清空原有数据
	Empty();
	if(InheritStableModelInfo(*pTurboBuffer))
		return true;
	hashTowerNodeI.CreateHashTable(pTower->Node.GetNodeNum());
	//一、分析汇总传力节点集合
	for(pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
	{
		pNode->UnifiedCfgword();
		if(pNode->layer(0)=='L'&&cfgwordLeg.And(pNode->cfgword))
			result.allnodeset.append(pNode);
		else if(cfgwordBody.And(pNode->cfgword))
			result.allnodeset.append(pNode);
		hashTowerNodeI.SetValueAt(pNode->point_i,pNode);
		if(pNode->force_type==AUTO_NODE)
			pNode->feature=0;	//置为非传力节点（非受力材端点）
		else if(pNode->force_type==FORCE_NODE)
			pNode->feature=1;	//手动设定的传力节点
		else //if(pNode->force_type==SWING_NODE)
			pNode->feature=0;	//摆动节点上的荷载被等效到其余节点上后将被忽略
	}
	for(pLinePart=pTower->Parts.GetFirstLinePart();pLinePart;pLinePart=pTower->Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
			continue;
		if(pLinePart->pStart->force_type==SWING_NODE||pLinePart->pEnd->force_type==SWING_NODE)
			continue;	//与摆动节点相连的杆件为摆动杆件,将被忽略
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//组合角钢中的子角钢，由组合角钢在力学分析时代理所有行为不应单独重复考虑
#endif
		pLinePart->UnifiedCfgword();
		if(pLinePart->layer(0)=='L'&&cfgwordLeg.And(pLinePart->cfgword))
			result.allrodset.append(pLinePart);
		else if(cfgwordBody.And(pLinePart->cfgword))
			result.allrodset.append(pLinePart);
		if(pLinePart->cMaterial==0)
			throw "存在杆件缺少材质信息,请进行数据完整性校验!";
		if(pLinePart->IsAuxPole())
			continue;
		if(pLinePart->pStart) 
			pLinePart->pStart->feature=1;
		if(pLinePart->pEnd) 
			pLinePart->pEnd->feature=1;
	}
	SmartJustifyNodeForceType(pTower,result.allnodeset);//有集中力或风压轮廓加载点即为传力节点
	int i;
	//
	CLDSNodePtr* nodeArr=new CLDSNodePtr[result.allnodeset.GetNodeNum()];
	for(pNode=result.allnodeset.GetFirst(),i=0;pNode;pNode=result.allnodeset.GetNext(),i++)
		nodeArr[i]=pNode;
	CQuickSort<CLDSNode*>::QuickSort(nodeArr,result.allnodeset.GetNodeNum(),compareNodeOrderByPointI);
	CHashList<CLDSNodePtr>hashPointI;
	bool passNodeICheck=true;
	for(i=0;i<result.allnodeset.GetNodeNum();i++)
	{
		if(nodeArr[i]->feature!=0)
		{
			if(nodeArr[i]->point_i<=0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Node 0X%X not numbered legally(%d)",nodeArr[i]->handle,nodeArr[i]->point_i);
#else
				logerr.Log("节点0X%X还未进行合法编号(%d)",nodeArr[i]->handle,nodeArr[i]->point_i);
#endif
				passNodeICheck=false;
			}
			else if(hashPointI.GetValue(nodeArr[i]->point_i)!=NULL)
			{
				pNode=hashPointI[nodeArr[i]->point_i];
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("node 0X%X and node 0X%X share the same number(%d),serial number is not reasonable!",pNode->handle,nodeArr[i]->handle,pNode->point_i);
#else
				logerr.Log("节点0X%X与节点0X%X有相同的编号(%d)，编号不合理！",pNode->handle,nodeArr[i]->handle,pNode->point_i);
#endif
				passNodeICheck=false;
			}
			else
				hashPointI.SetValue(nodeArr[i]->point_i,nodeArr[i]);
			result.nodeSet.append(nodeArr[i]);
		}
	}
	delete []nodeArr;
	if(!passNodeICheck)
		return false;
	//分配节点体荷载哈希表存储空间
	result.hashNodeBodyLoad.CreateHashTable(result.nodeSet.GetNodeNum());
	//二、分析拆分杆件为单元集合
	//对铁塔中较长杆件需要中间分段的情况进行处理,规则如下：
	//1.对一长杆件上任两个受力节点间的一段算作一个应力分析单元段
	//2.对补助材不进行分析略过不算作应力分析单元
	for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
	{
		//if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
		//	continue;
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
		//if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&pLinePart->hGr
		if(pLinePart->IsAuxPole())	//补助材
		{
			pElem=result.elemList.append();
			pElem->cfgword=pLinePart->cfgword;
			if(pLinePart->size_idClassType==CLS_CABLE)
			{	//拉线单元
				pElem->elem_type =1;
				pElem->T0=pLinePart->m_fPreTension;
				if(pLinePart->pEnd->IsHasRestrict(X_RESTRICT|Y_RESTRICT|Z_RESTRICT))
					pElem->bAnchorEnd=true;
				else if(pLinePart->pStart->IsHasRestrict(X_RESTRICT|Y_RESTRICT|Z_RESTRICT))
					pElem->bAnchorEnd=false;
				else if(pLinePart->pStart->Position(false).z>pLinePart->pEnd->Position(false).z)
					pElem->bAnchorEnd=false;
				else //if(pLinePart->pEnd->pos.z>pLinePart->pStart->pos.z)
					pElem->bAnchorEnd=true;
			}
			pElem->pOrgPole=pLinePart;
			pElem->pStart=pLinePart->pStart;
			pElem->pEnd=pLinePart->pEnd;
			pElem->start_force_type=pLinePart->start_force_type;
			pElem->end_force_type=pLinePart->end_force_type;
			pElem->start_joint_type=pLinePart->start_joint_type;
			pElem->end_joint_type=pLinePart->end_joint_type;
		}
		else								//受力材
		{
			NODESET nodeset;
			SelectNodeInPole(pLinePart,nodeset);
			int n = nodeset.GetNodeNum();
			if(n==2||pLinePart->size_idClassType==CLS_CABLE)
			{
				pElem = result.elemList.append();
				pElem->cfgword=pLinePart->cfgword;
				pElem->pStart=pLinePart->pStart;
				pElem->pEnd=pLinePart->pEnd;
				pElem->pOrgPole=pLinePart;
				pElem->start_force_type=pLinePart->start_force_type;
				pElem->end_force_type=pLinePart->end_force_type;
				pElem->start_joint_type=pLinePart->start_joint_type;
				pElem->end_joint_type=pLinePart->end_joint_type;
				if(pLinePart->size_idClassType==CLS_CABLE)
				{	//拉线
					pElem->elem_type =1;
					pElem->T0=pLinePart->m_fPreTension;
					if(pLinePart->pEnd->IsHasRestrict(X_RESTRICT|Y_RESTRICT|Z_RESTRICT))
						pElem->bAnchorEnd=true;
					else if(pLinePart->pStart->IsHasRestrict(X_RESTRICT|Y_RESTRICT|Z_RESTRICT))
						pElem->bAnchorEnd=false;
					else if(pLinePart->pStart->Position(false).z>pLinePart->pEnd->Position(false).z)
						pElem->bAnchorEnd=false;
					else //if(pLinePart->pEnd->pos.z>pLinePart->pStart->pos.z)
						pElem->bAnchorEnd=true;
				}
			}
			else if(n>2)
			{
				CLDSNode *pNode=NULL, *pCurNode=NULL;
				CFGWORD basicCfgWord=pLinePart->pStart->UnifiedCfgword();//cfgword;
				basicCfgWord&=pLinePart->UnifiedCfgword();//cfgword;
				basicCfgWord&=pLinePart->pEnd->UnifiedCfgword();//cfgword;
				CFGWORD lineword;
				if(pLinePart->Layer(0)=='L')
					lineword=cfgwordLeg;
				else
					lineword=cfgwordBody;
				basicCfgWord&=lineword;
				lineword&=pLinePart->cfgword;
				if(!basicCfgWord.IsEqual(lineword))
				{
					CXhChar50 strStart,strEnd,strRod;
					if(pLinePart->Layer(0)=='L')	//接腿构件
						pLinePart->cfgword.GetLegScopeStr(strRod,50,true);
					else
						pLinePart->cfgword.GetBodyScopeStr(strRod);
					if(pLinePart->pStart->Layer(0)=='L')	//接腿构件
						pLinePart->pStart->cfgword.GetLegScopeStr(strStart,50,true);
					else
						pLinePart->pStart->cfgword.GetBodyScopeStr(strStart);
					if(pLinePart->pEnd->Layer(0)=='L')	//接腿构件
						pLinePart->pEnd->cfgword.GetLegScopeStr(strEnd,50,true);
					else
						pLinePart->pEnd->cfgword.GetBodyScopeStr(strEnd);
					throw (char*)CXhChar50("0X%X杆件端节点配材号('%s' -> '%s')与杆件自身配材号(%s)不一致！",pLinePart->handle,(char*)strStart,(char*)strEnd,(char*)strRod);
				}
				for(pNode=nodeset.GetFirst();pNode!=NULL&&pNode!=pLinePart->pEnd;pNode=nodeset.GetNext())
				{
					if(pNode->feature==0)
						continue;	//不是传力节点
					CFGWORD processedCfgWord,elemAllCfgWord=basicCfgWord;
					elemAllCfgWord&=pNode->cfgword;
					pElem=result.elemList.append();
					pElem->pStart=pNode;
					BOOL bPush=nodeset.push_stack();
					for(pNode=nodeset.GetNext();pNode;pNode=nodeset.GetNext())
					{
						if(pNode->feature==0)
							continue;
						if(!elemAllCfgWord.And(pNode->cfgword))
							continue;
						pElem->pEnd=pNode;
						pElem->cfgword=elemAllCfgWord;
						pElem->cfgword&=pNode->cfgword;
						pElem->pOrgPole=pLinePart;
						if(pLinePart->size_idClassType==CLS_CABLE)
							pElem->elem_type =1;
						//判断杆单元两端的连接类型
						if(pElem->pStart==pLinePart->pStart)
							pElem->start_joint_type=pLinePart->start_joint_type;
						else
							pElem->start_joint_type=JOINT_RIGID;
						if(pElem->pEnd==pLinePart->pEnd)
							pElem->end_joint_type=pLinePart->end_joint_type;
						else
							pElem->end_joint_type=JOINT_RIGID;
						//判断杆单元两端的受力类型
						if(pElem->pStart==pLinePart->pStart)
							pElem->start_force_type=pLinePart->start_force_type;
						else
							pElem->start_force_type=CENTRIC_FORCE;
						if(pElem->pEnd==pLinePart->pEnd)
							pElem->end_force_type=pLinePart->end_force_type;
						else
							pElem->end_force_type=CENTRIC_FORCE;
						
						processedCfgWord.AddSpecWord(pElem->cfgword);
						if(processedCfgWord.IsEqual(elemAllCfgWord))
							break;
						if(pElem->pStart->Position(false)==pElem->pEnd->Position(false))
							throw (char*)CXhChar50("单元%d-%d始末端节点位置重合，数据有误！",pElem->pStart->point_i,pElem->pEnd->point_i);
					}
					if(pElem->pEnd==NULL)
						result.elemList.DeleteCursor(TRUE);
					if(bPush)
						nodeset.pop_stack();
				}
			}
			else	//不合法的杆件(一根杆件至少应有两个端节点在杆件上)
				throw (char*)CXhChar50("0X%X杆件至少有一个端节点不在当前模型上",pLinePart->handle);
		}
	}
	UpdateElemLengthInfo();	//更新单元长度信息
	for(pElem=result.elemList.GetFirst(),i=1;pElem;pElem=result.elemList.GetNext(),i++)
		pElem->id=i;
	result.InitNodeBodyLoads();	//初始化新版体荷载算法, 必须置于CElement::id赋值以后
	result.hashElemStartBodyLoad.CreateHashTable(result.elemList.GetNodeNum());
	result.hashElemEndBodyLoad.CreateHashTable(result.elemList.GetNodeNum());
	if(bApplyLoad)
	{
		result.hashElemsMap.Empty();
		result.hashElem.CreateHashTable(result.elemList.GetNodeNum());
		//赋值单元标识及更新节点连接单元信息
		ELEMLATERAL_STIFF lateralStiff;
		for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{
			result.hashElem.SetValueAt(pElem->id,pElem);
			result.hashElemsMap.SetValue(pElem->pStart->handle,pElem->pEnd->handle,pElem);
			lateralStiff.pElem=pElem;
			lateralStiff.stiff=pElem->GetWorkI()/pElem->Length();
			//更新始端节点连接单元信息
			CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pElem->pStart->handle);
			if(pNodeLinks==NULL)
			{
				pNodeLinks=result.hashNodeLinks.Add(pElem->pStart->handle);
				pNodeLinks->pNode=pElem->pStart;
				pNodeLinks->cfgword=pElem->pStart->cfgword;
			}
			pNodeLinks->linkElems.append(lateralStiff);
			pNodeLinks->cfgword&=pElem->cfgword;
			if(!pNodeLinks->cfgword.IsNull()&&pNodeLinks->supportType<3)
			{	//存在共用配材号，且目前尚不是三维立体支撑
				if(pNodeLinks->supportType||pNodeLinks->supportDirect.IsZero())
				{
					pNodeLinks->supportType=1;
					pNodeLinks->supportDirect=pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==1&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//由轴向支撑升级为平面支撑
					pNodeLinks->supportType=2;
					pNodeLinks->supportDirect=pNodeLinks->supportDirect^pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==2&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//由平面支撑升级为三维立体支撑
					pNodeLinks->supportType=3;
				}
			}
			//更新终端节点连接单元信息
			pNodeLinks=result.hashNodeLinks.GetValue(pElem->pEnd->handle);
			if(pNodeLinks==NULL)
			{
				pNodeLinks=result.hashNodeLinks.Add(pElem->pEnd->handle);
				pNodeLinks->pNode=pElem->pEnd;
				pNodeLinks->cfgword=pElem->pEnd->cfgword;
			}
			pNodeLinks->linkElems.append(lateralStiff);
			pNodeLinks->cfgword&=pElem->cfgword;
			if(!pNodeLinks->cfgword.IsNull()&&pNodeLinks->supportType<3)
			{	//存在共用配材号，且目前尚不是三维立体支撑
				if(pNodeLinks->supportType||pNodeLinks->supportDirect.IsZero())
				{
					pNodeLinks->supportType=1;
					pNodeLinks->supportDirect=pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==1&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//由轴向支撑升级为平面支撑
					pNodeLinks->supportType=2;
					pNodeLinks->supportDirect=pNodeLinks->supportDirect^pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==2&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//由平面支撑升级为三维立体支撑
					pNodeLinks->supportType=3;
				}
			}
		}
		//更新辅助材端节点位于受力材单元中间的节点连接情况
		for(pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
		{
			if(pNode->feature==1)	//受力材节点
				continue;
			for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
			{
				if(pElem->pOrgPole->handle!=pNode->RootFatherPartHandle())
					continue;
				f3dPoint vec_start=pNode->Position(false)-pElem->pStart->Position(false);
				f3dPoint vec_end=pNode->Position(false)-pElem->pEnd->Position(false);
				if(vec_start*vec_end<0)
				{
					lateralStiff.pElem=pElem;
					lateralStiff.stiff=pElem->GetWorkI()/pElem->Length();
					CNodeLinkElems *pNodeLinks=result.hashNodeLinks.Add(pNode->handle);
					pNodeLinks->linkElems.append(lateralStiff);
					break;
				}
			}
		}
		//更新单元对称信息
		CHashList<RODMIR_INFO>hashRodMirInfo;
		//TODO:(3)[更新单元对称信息]极大影响计算效率　wjh-2016.2.25
//#ifdef dkslkd
		for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
		{
#ifndef __TSA_
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
				continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
			RODMIR_INFO *pMirInfo=NULL,*pMirXInfo=NULL,*pMirYInfo=NULL,*pMirZInfo=NULL;
			pMirInfo=hashRodMirInfo.GetValue(pLinePart->handle);
			if(pMirInfo!=NULL)
				continue;
			CLdsPartListStack stack(pTower);
			CLDSLinePart *pMirXRod=pLinePart->GetMirRod(MIRMSG(1));
			/*if(pMirXRod==pLinePart)
				pMirXRod=NULL;	//横隔面X轴向中线杆件X轴对称是自己，此处不能赋空，否则会导致单元对称信息部分缺失 wjh-2013.5.1
			else */
			if(pMirXRod!=NULL)
			{
				pMirInfo=hashRodMirInfo.Add(pLinePart->handle);
				pMirInfo->pRod=pLinePart;
				pMirInfo->pRodMirX=pMirXRod;
				pMirXInfo=hashRodMirInfo.Add(pMirXRod->handle);
				pMirXInfo->pRod=pMirXRod;
				pMirXInfo->pRodMirX=pLinePart;
			}
			CLDSLinePart *pMirYRod=pLinePart->GetMirRod(MIRMSG(2));
			/*if(pMirYRod==pLinePart)
				pMirYRod=NULL;	//横隔面Y轴向中线杆件X轴对称是自己
			else */
			if(pMirYRod==pMirXRod)
				pMirYRod=NULL;	//橫隔面交叉材即是X对称又是Y对称
			else if(pMirYRod!=NULL)
			{
				pMirInfo=hashRodMirInfo.Add(pLinePart->handle);
				pMirInfo->pRod=pLinePart;
				pMirInfo->pRodMirY=pMirYRod;
				pMirYInfo=hashRodMirInfo.Add(pMirYRod->handle);
				pMirYInfo->pRod=pMirYRod;
				pMirYInfo->pRodMirY=pLinePart;
			}
			CLDSLinePart *pMirZRod=pLinePart->GetMirRod(MIRMSG(3));
			/*if(pMirZRod==pLinePart)
				pMirZRod=NULL;	//横隔面对角线杆件X轴对称是自己
			else */
			if(pMirZRod!=NULL)
			{
				pMirInfo=hashRodMirInfo.Add(pLinePart->handle);
				pMirInfo->pRod=pLinePart;
				pMirInfo->pRodMirZ=pMirZRod;
				pMirZInfo=hashRodMirInfo.Add(pMirZRod->handle);
				pMirZInfo->pRod=pMirZRod;
				pMirZInfo->pRodMirZ=pLinePart;
			}
			if(pMirXInfo)
			{
				pMirXInfo->pRodMirY=pMirInfo->pRodMirZ;
				pMirXInfo->pRodMirZ=pMirInfo->pRodMirY;
			}
			if(pMirYInfo)
			{
				pMirYInfo->pRodMirX=pMirInfo->pRodMirZ;
				pMirYInfo->pRodMirZ=pMirInfo->pRodMirX;
			}
			if(pMirZInfo)
			{
				pMirZInfo->pRodMirX=pMirInfo->pRodMirY;
				pMirZInfo->pRodMirY=pMirInfo->pRodMirX;
			}
		}
//#endif
		ARRAY_LIST<ELEM_PTR>elem_arr,elem_arr_x,elem_arr_y,elem_arr_z;
		for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
			pLinePart->feature%=10;	//清空处理标记
		for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{
			pRodIntervals=result.hashRodIntervals.Add(pElem->pOrgPole->handle);
			pRodIntervals->listElems.append(pElem);
		}
		for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
		{
			//if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			//	continue;
#ifndef __TSA_
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
				continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
			if(pLinePart->feature/10!=0)
				continue;	//已处理

			ELEM_PTR* ppElem;
			RODMIR_INFO *pMirInfo=hashRodMirInfo.GetValue(pLinePart->handle);
			if(pMirInfo==NULL)
			{	//非自身对称的杆件
				pRodIntervals=result.hashRodIntervals.Add(pLinePart->handle);
				for(ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
					(*ppElem)->relaElem.bFatherElem=true;
				continue;	//无对称信息
			}
			elem_arr.SetSize(0,10);
			elem_arr_x.SetSize(0,10);
			elem_arr_y.SetSize(0,10);
			elem_arr_z.SetSize(0,10);
			pRodIntervals=result.hashRodIntervals.Add(pLinePart->handle);
			for(ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
				elem_arr.append(*ppElem);

			CLDSLinePart *pMirXRod=pMirInfo->pRodMirX;//(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(1));
			if(pMirXRod!=NULL)
			{
				pMirXRod->feature+=10;
				pRodIntervals=result.hashRodIntervals.Add(pMirXRod->handle);
				for(ELEM_PTR* ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
					elem_arr_x.append(*ppElem);
			}
			CLDSLinePart *pMirYRod=pMirInfo->pRodMirY;//(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(2));
			if(pMirYRod==pMirXRod)
				pMirYRod=NULL;	//橫隔面交叉材即是X对称又是Y对称
			else if(pMirYRod!=NULL)
			{
				pMirYRod->feature+=10;
				pRodIntervals=result.hashRodIntervals.Add(pMirYRod->handle);
				for(ELEM_PTR* ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
					elem_arr_y.append(*ppElem);
			}
			CLDSLinePart *pMirZRod=pMirInfo->pRodMirZ;//(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(3));
			if(pMirZRod!=NULL)
			{
				pMirZRod->feature+=10;
				pRodIntervals=result.hashRodIntervals.Add(pMirZRod->handle);
				for(ELEM_PTR* ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
					elem_arr_z.append(*ppElem);
			}
			for(i=0;i<elem_arr.GetSize();i++)
			{
				int j;
				CElement *pFatherElem=NULL,*pMirXElem=NULL,*pMirYElem=NULL,*pMirZElem=NULL;
				for(j=0;j<elem_arr_x.GetSize();j++)
				{
					if(elem_arr[i]->id==elem_arr_x[j]->id)
						continue;	//单元自对称
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_x[j]->pStart->Position(false),elem_arr_x[j]->pEnd->Position(false),1))
					{	//X轴对称
						elem_arr[i]->relaElem.idElemMirX=elem_arr_x[j]->id;
						pMirXElem=elem_arr_x[j];
						pMirXElem->relaElem.idElemMirX=elem_arr[i]->id;
						if(elem_arr[i]->CompareElemPointI(elem_arr_x[j])<0)
						{
							pFatherElem=elem_arr[i];
						}
						else
						{
							pFatherElem=pMirXElem;
						}
						break;
					}
				}
				for(j=0;j<elem_arr_y.GetSize();j++)
				{
					if(elem_arr[i]->id==elem_arr_y[j]->id)
						continue;	//单元自对称
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_y[j]->pStart->Position(false),elem_arr_y[j]->pEnd->Position(false),2))
					{	//Y轴对称
						elem_arr[i]->relaElem.idElemMirY=elem_arr_y[j]->id;
						pMirYElem=elem_arr_y[j];
						pMirYElem->relaElem.idElemMirY=elem_arr[i]->id;
						if(pFatherElem==NULL&&elem_arr[i]->CompareElemPointI(elem_arr_y[j])<0)
						{
							pFatherElem=elem_arr[i];
						}
						else if(pFatherElem==NULL||pFatherElem->CompareElemPointI(elem_arr_y[j])>0)
						{
							pFatherElem=pMirYElem;
						}
						break;
					}
				}
				for(j=0;j<elem_arr_z.GetSize();j++)
				{
					if(elem_arr[i]->id==elem_arr_z[j]->id)
						continue;	//单元自对称
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_z[j]->pStart->Position(false),elem_arr_z[j]->pEnd->Position(false),3))
					/*	fabs(elem_arr[i]->pStart->pos.x+elem_arr_z[j]->pStart->pos.x)<EPS&&
						fabs(elem_arr[i]->pStart->pos.y+elem_arr_z[j]->pStart->pos.y)<EPS&&
						fabs(elem_arr[i]->pStart->pos.z-elem_arr_z[j]->pStart->pos.z)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.x+elem_arr_z[j]->pEnd->pos.x)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.y+elem_arr_z[j]->pEnd->pos.y)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.z-elem_arr_z[j]->pEnd->pos.z)<EPS)*/
					{	//Z轴对称
						if(fabs(elem_arr[i]->pStart->Position(false).x)<EPS&&fabs(elem_arr[i]->pEnd->Position(false).x)<EPS)
						{
							elem_arr[i]->relaElem.idElemMirX=elem_arr_z[j]->id;
							pMirXElem=elem_arr_z[j];
							pMirXElem->relaElem.idElemMirX=elem_arr[i]->id;
							if(pFatherElem==NULL&&elem_arr[i]->CompareElemPointI(elem_arr_z[j])<0)
							{
								pFatherElem=elem_arr[i];
							}
							else if(pFatherElem==NULL||pFatherElem->CompareElemPointI(elem_arr_z[j])>0)
							{
								pFatherElem=pMirXElem;
							}
						}
						else if(fabs(elem_arr[i]->pStart->Position(false).y)<EPS&&fabs(elem_arr[i]->pEnd->Position(false).y)<EPS)
						{
							elem_arr[i]->relaElem.idElemMirY=elem_arr_z[j]->id;
							pMirYElem=elem_arr_z[j];
							pMirYElem->relaElem.idElemMirY=elem_arr[i]->id;
							if(pFatherElem==NULL&&elem_arr[i]->CompareElemPointI(elem_arr_z[j])<0)
							{
								pFatherElem=elem_arr[i];
							}
							else if(pFatherElem==NULL||pFatherElem->CompareElemPointI(elem_arr_z[j])>0)
							{
								pFatherElem=pMirYElem;
							}
						}
						else
						{
							elem_arr[i]->relaElem.idElemMirZ=elem_arr_z[j]->id;
							pMirZElem=elem_arr_z[j];
							pMirZElem->relaElem.idElemMirZ=elem_arr[i]->id;
							if(pFatherElem==NULL&&elem_arr[i]->CompareElemPointI(elem_arr_z[j])<0)
							{
								pFatherElem=elem_arr[i];
							}
							else if(pFatherElem==NULL||pFatherElem->CompareElemPointI(elem_arr_z[j])>0)
							{
								pFatherElem=pMirZElem;
							}
						}
						break;
					}
				}
				if(elem_arr[i]->relaElem.idElemMirZ>0)
				{
					if( elem_arr[i]->relaElem.idElemMirZ==elem_arr[i]->relaElem.idElemMirX||
						elem_arr[i]->relaElem.idElemMirZ==elem_arr[i]->relaElem.idElemMirY)
					{
						elem_arr[i]->relaElem.idElemMirZ=0;//即是Z轴对称又是X或Y轴对称时以X或Y轴对称优先显示对称，且不得重复对称
						if(pMirZElem)
							pMirZElem->relaElem.idElemMirZ=0;
					}
					elem_arr[i]->relaElem.bFatherElem=false;
				}
				if(pMirXElem&&elem_arr[i]->relaElem.idElemMirX>0)
				{
					pMirXElem->relaElem.idElemMirY=elem_arr[i]->relaElem.idElemMirZ;
					pMirXElem->relaElem.idElemMirZ=elem_arr[i]->relaElem.idElemMirY;
					if( pMirXElem->relaElem.idElemMirZ==pMirXElem->relaElem.idElemMirX||
						pMirXElem->relaElem.idElemMirZ==pMirXElem->relaElem.idElemMirY)
						pMirXElem->relaElem.idElemMirZ=0;//即是Z轴对称又是X或Y轴对称时以X或Y轴对称优先显示对称，且不得重复对称
					pMirXElem->relaElem.bFatherElem=false;
				}
				if(pMirYElem&&elem_arr[i]->relaElem.idElemMirY>0)
				{
					pMirYElem->relaElem.idElemMirX=elem_arr[i]->relaElem.idElemMirZ;
					pMirYElem->relaElem.idElemMirZ=elem_arr[i]->relaElem.idElemMirX;
					if( pMirYElem->relaElem.idElemMirZ==pMirYElem->relaElem.idElemMirX||
						pMirYElem->relaElem.idElemMirZ==pMirYElem->relaElem.idElemMirY)
						pMirYElem->relaElem.idElemMirZ=0;//即是Z轴对称又是X或Y轴对称时以X或Y轴对称优先显示对称，且不得重复对称
					pMirYElem->relaElem.bFatherElem=false;
				}
				if(pMirZElem&&elem_arr[i]->relaElem.idElemMirZ>0)
				{
					pMirZElem->relaElem.idElemMirX=elem_arr[i]->relaElem.idElemMirY;
					pMirZElem->relaElem.idElemMirY=elem_arr[i]->relaElem.idElemMirX;
					if( pMirZElem->relaElem.idElemMirZ==pMirZElem->relaElem.idElemMirX||
						pMirZElem->relaElem.idElemMirZ==pMirZElem->relaElem.idElemMirY)
						pMirZElem->relaElem.idElemMirZ=0;//即是Z轴对称又是X或Y轴对称时以X或Y轴对称优先显示对称，且不得重复对称
					pMirZElem->relaElem.bFatherElem=false;
				}
				if(pFatherElem)
					pFatherElem->relaElem.bFatherElem=true;
				else	//孤立单元(如横隔面内自X/Y轴对称的中间横材)
					elem_arr[i]->relaElem.bFatherElem=true;
			}
		}
		InitElemLamdaInfo(loadSrcDesignElemInfo);
		RetrieveFatherElemHashSet();	//提取父单元，以便简化计算单元是否有足够支撑
		CalAuxElemBraceForcePreInfo();	//计算辅材单元的支撑力系数
		if(ActiveDesignCode->GetBraceCoefOfPrimaryToPost()>0)
			CalPrimaryElemBraceForcePreInfo();	//计算斜材单元的支撑力系数
		//三、初始化荷载工况哈希表以便后面查询
		CWorkStatus *pStatus;
		CHashTable<CWorkStatus*>statusHashTbl;
		statusHashTbl.CreateHashTable(Ta.WorkStatus.GetNodeNum());
		for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			if(pStatus->feature==0)	//处于睡眠状态现不处理此计算工况
				continue;
			double W0=pStatus->WindLoad.velocity*pStatus->WindLoad.velocity/1.6;
			statusHashTbl.SetValueAt(pStatus->handle,pStatus);
			double sina=sin(RADTODEG_COEF*pStatus->WindLoad.azimuth);
			double cosa=cos(RADTODEG_COEF*pStatus->WindLoad.azimuth);
			for(CWorkStatus::WINDLOAD_REFPARAM *pRefParam=pStatus->AttachWindLoad.GetFirst();pRefParam;pRefParam=pStatus->AttachWindLoad.GetNext())
			{
				CAttachWindLoad *pWindLoad=pTower->AttachWindLoad.FromHandle(pStatus->AttachWindLoad.GetCursorKey());
				if(pWindLoad==NULL)	//以防数据库中此荷载已删除，而工况中未能及时更新2005/01/17 WJH
					continue;
				double node_wind_area=pWindLoad->m_fShieldArea/pWindLoad->GetNodeList()->GetNodeNum();
				pRefParam->attachMass=pWindLoad->m_fAttachWeight/pWindLoad->GetNodeList()->GetNodeNum();
				double Wk=pRefParam->shieldCoef*pWindLoad->m_fProfileCoef*pWindLoad->m_fBetaZ*node_wind_area*pWindLoad->m_fShieldCoef*W0;
				pRefParam->basicAttachWindLoadF.x=Wk*sina;
				pRefParam->basicAttachWindLoadF.y=Wk*cosa;
			}
		}
		//四、更新长细比关联构件同时受压时长细比计算过程所需的共用部分关联单元信息
		for(CLDSLinePart *pRod=result.allrodset.GetFirst();pRod;pRod=result.allrodset.GetNext())
		{
#ifndef __TSA_
			if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0x20)
				continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
			CRodIncIntervals *pRodInfo=NULL,*pRefRodInfo=NULL;
			if(pRod->CalLenCoef.hRefPole>0x20)
				pRefRodInfo=result.hashRodIntervals.GetValue(pRod->CalLenCoef.hRefPole);
			if(pRefRodInfo)
				pRodInfo=result.hashRodIntervals.GetValue(pRod->handle);
			if(pRodInfo&&pRefRodInfo&&pRodInfo->listElems.GetNodeNum()==1&&pRefRodInfo->listElems.GetNodeNum()==1)
			{	//只有一个单元时表明杆件与单元是重合的，即杆件不用拆分即是计算单元
				pElem=*pRodInfo->listElems.GetFirst();
				CElement *pRefElem=*pRefRodInfo->listElems.GetFirst();
				if(pElem->cfgword.And(pRefElem->cfgword))
				{	//必须有同时出现的可能存在
					pElem->relaElem.idRefElem=pRefElem->id;
					pRefElem->relaElem.idRefElem=pElem->id;
				}
			}
		}
	}
	return true;
}
//</DEVELOP_PROCESS_MARK>
	//初次调用或计算模型(含接腿)发生变化时调用更新有限元分析模型信息
void CPreProcessor::RetrieveFemAnalInfo(CLDSModule *pModule)
{
	int i=0;
	CHashTable<FEM_NODE*> femNodeHashTbl;
	femNodeHashTbl.CreateHashTable(result.nodeSet.GetNodeNum());
	for(CLDSNode *pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
		pNode->feature=0;
	m_pAnalysisProcessor->nodeset.Empty();
	m_pAnalysisProcessor->hashElems.Empty();
	m_pAnalysisProcessor->femNodeList.Empty();
	m_pAnalysisProcessor->femElemList.Empty();
	m_pAnalysisProcessor->hashFemNodes.Empty();
	for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
 		if(pModule->IsSonPart(pElem->pOrgPole)&&pModule->IsSonPart(pElem->pStart)&&pModule->IsSonPart(pElem->pEnd))
		{
			pElem=m_pAnalysisProcessor->hashElems.SetValue(pElem->id,*pElem);	//后处理用单元信息填充
			if(!pElem->pOrgPole->IsAuxPole())	//受力材端节点置1标识
				pElem->pStart->feature=pElem->pEnd->feature=1;
		}
	}
	for(pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
	{
		if(pModule->IsSonPart(pNode)&&pNode->feature==1)
		{	//
			m_pAnalysisProcessor->nodeset.append(pNode);
			FEM_NODE *pFemNode=m_pAnalysisProcessor->femNodeList.append();
			m_pAnalysisProcessor->hashFemNodes.SetValue(pNode->point_i,pFemNode);
			pFemNode->index=m_pAnalysisProcessor->femNodeList.GetNodeNum()-1;
			pFemNode->point_i=pNode->point_i;
			femNodeHashTbl.SetValueAt(pNode->handle,pFemNode);
			pFemNode->pos=pNode->Position(false);
			pFemNode->wRestrict=pNode->GetRestrictWord(pModule->handle);
			pFemNode->dimension=0;
			//计算赋值节点上除连接单元外的附加质量，仅用于动力特性分析计算
			pFemNode->attachMass=pNode->attachMass+pNode->PermanentLoad.z/9.8;	//考虑附加质量，将来还应考虑辅材质量问题　wjh-2011.9.14
			pFemNode->pTag=NULL;
// 			pFemNode->Fxyz=?	//由于此处缺少工况信息，应在后续流程代码中赋值
		}
	}
	for(pElem=m_pAnalysisProcessor->hashElems.GetFirst();pElem;pElem=m_pAnalysisProcessor->hashElems.GetNext())
	{
		//有限元分析模块专用单元信息填充
		if(pElem->pStartBraceForceElem!=NULL)	//由前处理器中的单元更新为始端支撑受力材单元为分析处理器中的单元
			pElem->pStartBraceForceElem=m_pAnalysisProcessor->hashElems.GetValue(pElem->pStartBraceForceElem->id);
		if(pElem->pEndBraceForceElem!=NULL)		//由前处理器中的单元更新为终端支撑受力材单元为分析处理器中的单元
			pElem->pEndBraceForceElem=m_pAnalysisProcessor->hashElems.GetValue(pElem->pEndBraceForceElem->id);
		if(pElem->pOrgPole->IsAuxPole())
			continue;	//辅助材，只用于统计重量用
		if(pElem->BearTens1Comp2==1||pElem->BearTens1Comp2==2)
			continue;						//失效的拉压杆
		FEM_ELEMENT *pFemElem=m_pAnalysisProcessor->femElemList.append();
		pFemElem->pTag=NULL;
		pFemElem->idElem=pElem->id;
		pElem->pFemElem=pFemElem;
		if(!femNodeHashTbl.GetValueAt(pElem->pStart->handle,pFemElem->pStart))
		{
			char sError[100]="";
			sprintf(sError,"0X%X(%d)-0X%X(%d)单元始端节点与单元配材号不一致，数据不完整！",
				pElem->pStart->handle,pElem->pStart->point_i,pElem->pEnd->handle,pElem->pEnd->point_i);
			throw sError;
		}
		if(!femNodeHashTbl.GetValueAt(pElem->pEnd->handle,pFemElem->pEnd))
		{
			char sError[100]="";
			sprintf(sError,"0X%X(%d)-0X%X(%d)单元终端节点与单元配材号不一致，数据不完整！",
				pElem->pStart->handle,pElem->pStart->point_i,pElem->pEnd->handle,pElem->pEnd->point_i);
			throw sError;
		}
		pFemElem->start_i=pFemElem->pStart->index;
		pFemElem->end_i=pFemElem->pEnd->index;
		pFemElem->density=7.85e-6*pElem->pOrgPole->m_fWeightRaiseCoef;	//钢材密度7.85e-6kg/mm3
		pFemElem->elem_type=pElem->elem_type;
		pFemElem->A=pElem->pOrgPole->GetArea();
		pFemElem->E=pElem->pOrgPole->GetElasticityModulus();
		pFemElem->G=pFemElem->E/2.6;
		pFemElem->Iz=pFemElem->Ip=pElem->pOrgPole->GetWorkI()*g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef;	//以平行轴半径进行计算
		if(pElem->elem_type==1)
		{
			pFemElem->T0=pElem->T0;	//拉线单元预张力
			pFemElem->A=pElem->pOrgPole->GetArea();
			pFemElem->bAnchorEnd=pElem->bAnchorEnd;
			pFemElem->E=pElem->pOrgPole->GetWireElasticCoef();
			pFemElem->density=pElem->pOrgPole->GetWireUnitWeight()/(pElem->pOrgPole->GetWireArea()*1e6);
		}
		else
			pFemElem->T0=0;
	}
}
//抗震验算时的地震荷载计算
bool CPreProcessor::CalEarthQuakeLoad(CTower *pTower, CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode)
{
	CLDSNode *pNode;
	double tower_height=-1e10;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(!pModule->IsSonPart(pNode))
			continue;
		if(pNode->Position(false).z>tower_height)
			tower_height=pNode->Position(false).z;
		pNode->m_offsetAmplitude.Set();
		pNode->Feq.Set();
	}
	char file_path[MAX_PATH];
	theApp.GetFileName(file_path);
	strcat(file_path,".frq");
	//_snprintf(file_path,MAX_PATH-1,"%sfreqs.frq",theApp.work_path);
	FILE* fp=fopen(file_path,"rb");
	//计算地震荷载
	int iMode=FindAdaptNo(pStatus->vibration_mode_str);
	long i,nNodeNum=Ta.ReadVibraNodeMass(fp,NULL);
	if(nNodeNum==0)	//fp==NULL时nNodeNum=0
		return false;
	DYNAMIC_ANAL_NODE *node_arr=new DYNAMIC_ANAL_NODE[nNodeNum];
	double *m_arr=new double[nNodeNum];
	double *x_arr=new double[nNodeNum];
	double *f_arr=new double[nNodeNum];
	Ta.ReadVibraNodeMass(fp,node_arr);
	for(i=0;i<nNodeNum;i++)
	{
		node_arr[i].id=(DWORD)Ta.Node.FromHandle(node_arr[i].id);	//由节点句柄转换为节点指针
		m_arr[i]=node_arr[i].mass;
	}
	//读取首个振型数据
	bool bRead=Ta.ReadVibraMode(fp,iMode-1,true);

	double alfaMaxV=0;
	double hori_cosa=0,hori_sina=0,hori_angle=RADTODEG_COEF*pStatus->m_cHoriEarthQuakeAxis;
	hori_cosa=cos(hori_angle);
	hori_sina=sin(hori_angle);

	for(i=0;i<Ta.EarthQuake.nFreqsRank;i++)
	{
		double freqs=Ta.EarthQuake.modes_arr[i].freqs;
		double T=1/freqs;
		if(Ta.EarthQuake.alfa[i]<=0)
			Ta.EarthQuake.alfa[i]=CalAlfaOfEQ(T,Ta.EarthQuake.Tg,Ta.EarthQuake.alfa_max,Ta.EarthQuake.zeta,Ta.m_iLoadExecuteCode);
		if(Ta.EarthQuake.alfa[i]>alfaMaxV)
			alfaMaxV=Ta.EarthQuake.alfa[i];
	}
	if(pStatus->m_cMainLoadType!=8)	//需要考虑水平方向地震
	{
		while(bRead)
		{
			memset(f_arr,0,8*nNodeNum);	//清零
			for(i=0;i<nNodeNum;i++)
			{
				pNode=(CLDSNode*)node_arr[i].id;
				if(pStatus->m_cHoriEarthQuakeAxis==0)
					x_arr[i]=pNode->m_offsetAmplitude.x;
				else if(pStatus->m_cHoriEarthQuakeAxis==90)
					x_arr[i]=pNode->m_offsetAmplitude.y;
				else
					x_arr[i]=pNode->m_offsetAmplitude.x*hori_cosa+pNode->m_offsetAmplitude.y*hori_sina;
			}
			//计算各阶振型的振型参预系数
			double gama=0;
			if(pStatus->m_cHoriEarthQuakeAxis==0)//&&Ta.EarthQuake.modes_arr[iMode-1].gama_x<=0)
				gama=Ta.EarthQuake.modes_arr[iMode-1].gama_x;//=CalVibrationModeInvolvedCoef(m_arr,x_arr,n);
			else if(pStatus->m_cHoriEarthQuakeAxis==90)//&&Ta.EarthQuake.modes_arr[iMode-1].gama_y<=0)
				gama=Ta.EarthQuake.modes_arr[iMode-1].gama_y;//=CalVibrationModeInvolvedCoef(m_arr,x_arr,n);
			else
				gama=CalVibrationModeInvolvedCoef(m_arr,x_arr,nNodeNum);

			CalHoriEarthQuakeEffect(gama,Ta.EarthQuake.alfa[iMode-1],m_arr,x_arr,f_arr,nNodeNum);
			for(i=0;i<nNodeNum;i++)
			{
				pNode=(CLDSNode*)node_arr[i].id;
				if(pStatus->m_cHoriEarthQuakeAxis==0)
					pNode->Feq.x+=f_arr[i]*f_arr[i];
				else //if(pStatus->m_cHoriEarthQuakeAxis==90)
					pNode->Feq.y+=f_arr[i]*f_arr[i];
			}
			//读取下一阶振型数据
			iMode=FindAdaptNo(NULL);
			bRead=Ta.ReadVibraMode(fp,iMode-1,false);
		}
		for(i=0;i<nNodeNum;i++)
		{
			pNode=(CLDSNode*)node_arr[i].id;
			if(pStatus->m_cHoriEarthQuakeAxis==0&&pNode->Feq.x>0)
				pNode->Feq.x=sqrt(pNode->Feq.x);
			else if(pStatus->m_cHoriEarthQuakeAxis==90&&pNode->Feq.y>0)
				pNode->Feq.y=sqrt(pNode->Feq.y);
			else if(pNode->Feq.y>0)
			{
				pNode->Feq.x=sqrt(pNode->Feq.y)*hori_cosa;
				pNode->Feq.y=sqrt(pNode->Feq.y)*hori_sina;
			}
		}
	}
	fclose(fp);
	if(pStatus->m_cMainLoadType!=7)	//需要考虑竖直方向地震
	{
		memset(f_arr,0,8*nNodeNum);	//清零
		for(i=0;i<nNodeNum;i++)
			x_arr[i]=tower_height-((CLDSNode*)node_arr[i].id)->Position(false).z;
		CalVertEarthQuakeEffect(alfaMaxV,m_arr,x_arr,f_arr,nNodeNum);
		for(i=0;i<nNodeNum;i++)
			((CLDSNode*)node_arr[i].id)->Feq.z=f_arr[i]*0.75;
	}
	delete []node_arr;
	delete []m_arr;
	delete []x_arr;
	delete []f_arr;
	return true;
}
//iCase须<=(slope_n-1)*2
double CalEiffelFactor(double* joint_z_arr,double slope_n,double z,UINT iCase)
{
	if(iCase==0)
		return 1.0;	//无风压折减工况
	int slope_i=(iCase-1)/2+1;
	if(z<joint_z_arr[slope_i])
	{	//风压计算点在坡段交点之上
		if(slope_i%2==1)	//上大下小,下侧折减
			return 1.0;
		else				//上小下大,上侧折减
			return CTower::W0_EIFFEL_FACTOR;
	}
	else
	{	//风压计算点在坡段交点之下
		if(slope_i%2==1)	//上大下小,下侧折减
			return CTower::W0_EIFFEL_FACTOR;
		else				//上小下大,上侧折减
			return 1.0;
	}
}
//计算风压段的承受风压荷载
void CPreProcessor::CalWindAndAttachLoad(CTower *pTower,CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode,
								bool bForFoundation/*=false*/,double magnifyCoef/*=1.0*/,WINDSEGEXPORT_LIST* pExportList/*=NULL*/)
{
	double v=pStatus->WindLoad.velocity;
	double W0=v*v/1.6;	// N/m2
	double lowest_z=pModule->LowestZ();	//基础Z坐标
	//for(CLDSNode *pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
	for(CLDSNode *pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
	{
		pNode->WindLoad.Set();
		pNode->attachMass=0;
	}
	for(CWindSegment *pWindSeg=pTower->WindLoadSeg.GetFirst();pWindSeg;pWindSeg=pTower->WindLoadSeg.GetNext())
	{
		double mu_sa=1.3,mu_sb=1.3;
		int iSegNo=pWindSeg->cType;
		int iStyle=pWindSeg->cCalStyle;	//0.塔身方式计算1.X方向水平横担2.Y方向水平横担
		int iMirMsg=pWindSeg->cMirType;	//对称信息0.无对称1.左右对称
		double Wsy=0,Wsx=0,Wx=0,Wy=0;
		double high_pos=0;//pModule->land_high;
		double coef_x=1.0,coef_y=1.0;
		BOOL bValidSeg=TRUE;
		int i,nValidNode=pWindSeg->ValidNodeCount();
		for(i=0;i<nValidNode;i++)
		{
			if(!pModule->IsSonPart(pWindSeg->nodePtrArr[i]))
			{
				bValidSeg=FALSE;
				break;
			}
		}
		if(!bValidSeg)	//非本模型基本高的风压段
			continue;
		//斜材艾菲尔效应
		CWindSegElemPicker *pSegElemPicker=NULL;
		if(pWindSeg->pFrontElemPicker)
			pSegElemPicker=(CWindSegElemPicker*)pWindSeg->pFrontElemPicker;
		else
			pSegElemPicker=(CWindSegElemPicker*)pWindSeg->pSideElemPicker;
		int iEiffelCase=pStatus->GetEiffelCaseNo();
		if(	iEiffelCase>0&&pSegElemPicker!=NULL)
		{
			double eiffel_factor=1.0;
			//无论风压上大下小(iEiffelCase%2==1)还是上小下大(iEiffelCase%2==0),均取风压段下侧Z
			if(m_bIncEiffelFactorX&&pStatus->WindLoad.azimuth%180==0)	//90°X轴大风工况joint_z_arr)
			{
				eiffel_factor=CalEiffelFactor(jointx_z_arr.m_pData,jointx_z_arr.GetSize(),pSegElemPicker->MaxZ(),iEiffelCase);
				W0*=eiffel_factor;	//计入埃菲尔效应，风压折减系数
			}
			else if(m_bIncEiffelFactorY&&pStatus->WindLoad.azimuth%90==0)	//0°Y轴大风工况joint_z_arr)
			{
				eiffel_factor=CalEiffelFactor(jointy_z_arr.m_pData,jointy_z_arr.GetSize(),pSegElemPicker->MaxZ(),iEiffelCase);
				W0*=eiffel_factor;	//计入埃菲尔效应，风压折减系数
			}
		}
		WINDSEGLOADCAL_EXPORT *pExport=pExportList!=NULL?pExportList->append():NULL;
		for(i=0;i<2;i++)
		{
			CWindSegElemPicker *pSegElemPicker=(CWindSegElemPicker*)pWindSeg->pFrontElemPicker;
			if(i==1)
				pSegElemPicker=(CWindSegElemPicker*)pWindSeg->pSideElemPicker;
			if(pSegElemPicker)
			{
				if(!pSegElemPicker->cfgword.IsHasBodyNo(pModule->GetBodyNo())||pSegElemPicker->elemSet.GetNodeNum()==0)
					pSegElemPicker->StartElemPicker(result.elemList,pModule);
				if(pExport)
				{
					for(CElement* pElem=pSegElemPicker->elemSet.GetFirst();pElem;pElem=pSegElemPicker->elemSet.GetNext())
					{
						int startPointI=min(pElem->pStart->point_i,pElem->pEnd->point_i);
						int endPointI  =max(pElem->pStart->point_i,pElem->pEnd->point_i);
						WINDSEGLOADCAL_EXPORT::ELEMREC* pRec=pExport->elemrecs.Add(startPointI,endPointI);
						pRec->startPointI=startPointI;
						pRec->endPointI=endPointI;
						pRec->area=pElem->Length()*pElem->GetWidth();
						pRec->size=pElem->pOrgPole->GetSizeSpec();
						pRec->length=pElem->Length();
						pRec->cMaterial=pElem->pOrgPole->cMaterial;
						pRec->muS=pWindSeg->m_fProfileModeCoef;
						pRec->efficArea=pRec->area*pRec->muS;
						if(i==0)
							pRec->frontwind=true;
						else
							pRec->sidewind=true;
					}
				}
				if(i==0)
				{
					high_pos=(lowest_z-pSegElemPicker->geomCenter.z)/1000+pModule->land_high;	//距离基准基础平均高
					if(pWindSeg->m_bAutoCalProfileCoef)
						pWindSeg->AfFront=pSegElemPicker->m_fPrjAreaCoef*pSegElemPicker->CalWindSegAf(W0,iLoadExecuteCode,&pWindSeg->m_fProfileModeCoef);
					else
						pWindSeg->AfFront=pSegElemPicker->m_fPrjAreaCoef*pSegElemPicker->CalWindSegAf(W0,iLoadExecuteCode,NULL);
					if(pSegElemPicker->m_fWrapArea<EPS)
						throw "风压段挡风面积为零，风压段信息有误";
					pWindSeg->FaiFront=pWindSeg->CqiFront*pWindSeg->AfFront/pSegElemPicker->m_fWrapArea;
					if(!pWindSeg->m_bSpecB_A)
					{
						if(pWindSeg->cType==0)
							pWindSeg->B_AFront=fabs(pWindSeg->nodePtrArr[0]->Position(false).y/pWindSeg->nodePtrArr[0]->Position(false).x);
						else
							pWindSeg->B_AFront=pSegElemPicker->b_a;
					}
				}
				else
				{
					if(pWindSeg->m_bAutoCalProfileCoef)
						pWindSeg->AfSide=pSegElemPicker->m_fPrjAreaCoef*pSegElemPicker->CalWindSegAf(W0,iLoadExecuteCode,&pWindSeg->m_fProfileModeCoef);
					else
						pWindSeg->AfSide=pSegElemPicker->m_fPrjAreaCoef*pSegElemPicker->CalWindSegAf(W0,iLoadExecuteCode,NULL);
					if(pSegElemPicker->m_fWrapArea<EPS)
						throw "风压段挡风面积为零，风压段信息有误";
					pWindSeg->FaiSide=pWindSeg->CqiSide*pWindSeg->AfSide/pSegElemPicker->m_fWrapArea;
					if(!pWindSeg->m_bSpecB_A)
					{
						if(pWindSeg->cType==0)
							pWindSeg->B_ASide=fabs(pWindSeg->nodePtrArr[0]->Position(false).x/pWindSeg->nodePtrArr[0]->Position(false).y);
						else
							pWindSeg->B_ASide=pSegElemPicker->b_a;
					}
					else
						pWindSeg->B_ASide=1.0/pWindSeg->B_AFront;
				}
			}
		}
		double mu_z=Find_MuZ(high_pos,pTower->m_cLandscape);	//查找风压高度变化系数
		double Xcoef_x,Xcoef_y,Ycoef_x,Ycoef_y;
		double betaZ=pWindSeg->BetaZ;
		if(bForFoundation)
			betaZ=pWindSeg->BetaZForFoundation;
		/*else */if(magnifyCoef>0)	//以前有else，但现在认为基础时，也应考虑风荷载覆冰增大系数
			betaZ*=magnifyCoef;

		if(pWindSeg->cType!=2)
		{
			mu_sb= pWindSeg->m_fProfileModeCoef*(1+Find_YiTa(pWindSeg->FaiFront,pWindSeg->B_AFront));
			Wsy=W0*mu_z*pWindSeg->K*mu_sb*betaZ*pWindSeg->AfFront*pWindSeg->CqiFront;//*pWindSeg->case_coef;
			mu_sa= pWindSeg->m_fProfileModeCoef*(1+Find_YiTa(pWindSeg->FaiSide,pWindSeg->B_ASide));
			Wsx=W0*mu_z*pWindSeg->K*mu_sa*betaZ*pWindSeg->AfSide*pWindSeg->CqiSide;//*pWindSeg->case_coef;
			long iCalStyle=pWindSeg->cCalStyle;
			if(iCalStyle==0&&pWindSeg->AfSide<EPS)
				iCalStyle=1;
			GetWindLoadAllotCoef(&Xcoef_x,&Xcoef_y,&Ycoef_x,&Ycoef_y,pStatus->WindLoad.azimuth,iCalStyle);
		}
		else if(pWindSeg->cType==2)
		{	//三角形塔架
			mu_sb = Find_MuS(pWindSeg->FaiFront,0);
			Wsx=Wsy=W0*mu_z*pWindSeg->K*mu_sb*betaZ*pWindSeg->AfFront*pWindSeg->CqiFront;//*pWindSeg->case_coef;
			Xcoef_x=sin(pStatus->WindLoad.azimuth*RADTODEG_COEF);
			Ycoef_x=cos(pStatus->WindLoad.azimuth*RADTODEG_COEF);
			Xcoef_y=Ycoef_y=0;
		}
		if(pWindSeg->cCalStyle!=0||pWindSeg->AfSide<EPS)	//横担段或侧面积无法计算时
		{
			Wx=Xcoef_x*Wsx+Xcoef_y*Wsy;
			Wy=Ycoef_x*Wsx+Ycoef_y*Wsy;
		}
		else
		{	//塔身斜向风荷载
			Wx=Xcoef_x*Wsx+Xcoef_y*Wsy;
			Wy=Ycoef_x*Wsx+Ycoef_y*Wsy;
		}
		if(pExportList)
		{
			pExport->windSegI=pWindSeg->iNo;
			pExport->cType=pWindSeg->cType;
			pExport->cCalStyle=pWindSeg->cCalStyle;
			pExport->cMirType=pWindSeg->cMirType;
			pExport->Wo=W0/1000;	//kN/m2
			pExport->fai_a=pWindSeg->FaiSide;
			pExport->fai_b=pWindSeg->FaiFront;
			pExport->b_a_a=pWindSeg->B_ASide;
			pExport->b_a_b=pWindSeg->B_AFront;
			if(pWindSeg->cType!=2)
			{
				pExport->yita_a=Find_YiTa(pWindSeg->FaiSide,pWindSeg->B_ASide);
				pExport->yita_b=Find_YiTa(pWindSeg->FaiFront,pWindSeg->B_AFront);
			}
			pExport->high=high_pos;
			CWindSegElemPicker *pFrontElemPicker=(CWindSegElemPicker*)pWindSeg->pFrontElemPicker;
			CWindSegElemPicker *pSideElemPicker=(CWindSegElemPicker*)pWindSeg->pSideElemPicker;
			if(pFrontElemPicker)
			{
				pExport->A_b=pFrontElemPicker->m_fWrapArea;
				pExport->Af_b=pWindSeg->AfFront;
			}
			if(pSideElemPicker)
			{
				pExport->A_a=pSideElemPicker->m_fWrapArea;
				pExport->Af_a=pWindSeg->AfSide;
			}
			pExport->muSa=mu_sa;
			pExport->muSb=mu_sb;
			pExport->muZ=mu_z;
			pExport->betaZ=betaZ;
			pExport->K=pWindSeg->K;
			pExport->Cqi_b=pWindSeg->CqiFront;
			pExport->Cqi_a=pWindSeg->CqiSide;
			pExport->Wsa=Wsx/1000;	//侧面垂直风吹风荷载
			pExport->Wsb=Wsy/1000;	//正面垂直风吹风荷载
			if(pWindSeg->cCalStyle==1)
			{
				pExport->Wsc=Wsy/1000;	//X向横担垂直风吹风荷载
				pExport->Xcoef_Wsc=Xcoef_y;
				pExport->Ycoef_Wsc=Ycoef_y;
			}
			else if(pWindSeg->cCalStyle==2)
			{
				pExport->Wsc=Wsx/1000;	//Y向横担垂直风吹风荷载
				pExport->Xcoef_Wsc=Xcoef_x;
				pExport->Ycoef_Wsc=Ycoef_x;
			}
			pExport->Xcoef_Wsa=Xcoef_x;
			pExport->Xcoef_Wsb=Xcoef_y;
			pExport->Ycoef_Wsa=Ycoef_x;
			pExport->Ycoef_Wsb=Ycoef_y;
			pExport->Wx=Wx/1000;
			pExport->Wy=Wy/1000;
		}
		WINDLOADNODE *pLoadNode=NULL;
		if(pWindSeg->cType==0&&nValidNode>=2)	//八点分配塔身段
		{
			CLDSNode *mir_node;
			if(pWindSeg->nodePtrArr[0]&&pWindSeg->nodePtrArr[1])
			{
				pWindSeg->nodePtrArr[0]->WindLoad.x+=Wx/8;
				pWindSeg->nodePtrArr[0]->WindLoad.y+=Wy/8;
				pWindSeg->nodePtrArr[1]->WindLoad.x+=Wx/8;
				pWindSeg->nodePtrArr[1]->WindLoad.y+=Wy/8;
				if(pExport)
				{
					pLoadNode=pExport->loadNodeList.Add(pWindSeg->nodePtrArr[0]->point_i);
					pLoadNode->nodeI=pWindSeg->nodePtrArr[0]->point_i;
					pLoadNode=pExport->loadNodeList.Add(pWindSeg->nodePtrArr[1]->point_i);
					pLoadNode->nodeI=pWindSeg->nodePtrArr[1]->point_i;
				}
				for(i=1;i<4;i++)
				{
					mir_node=pWindSeg->nodePtrArr[0]->GetMirNode(MIRMSG(i));
					if(mir_node)
					{
						mir_node->WindLoad.x+=Wx/8;
						mir_node->WindLoad.y+=Wy/8;
						if(pExport)
						{
							pLoadNode=pExport->loadNodeList.Add(mir_node->point_i);
							pLoadNode->nodeI=mir_node->point_i;
						}
					}
					mir_node=pWindSeg->nodePtrArr[1]->GetMirNode(MIRMSG(i));
					if(mir_node)
					{
						mir_node->WindLoad.x+=Wx/8;
						mir_node->WindLoad.y+=Wy/8;
						if(pExport)
						{
							pLoadNode=pExport->loadNodeList.Add(mir_node->point_i);
							pLoadNode->nodeI=mir_node->point_i;
						}
					}
				}
				if(pExport)
				{
					for(pLoadNode=pExport->loadNodeList.GetFirst();pLoadNode;pLoadNode=pExport->loadNodeList.GetNext())
					{
						pLoadNode->Fwx=Wx/8000;
						pLoadNode->Fwy=Wy/8000;
					}
				}
			}
		}
		else if(pWindSeg->cType==1&&nValidNode>=3)	//自定义轮廓
		{
			CLDSNode *mir_node;
			int nSumAllocLoadNode=2*nValidNode;
			for(i=0;i<nValidNode;i++)
			{
				pWindSeg->nodePtrArr[i]->WindLoad.x+=Wx/nSumAllocLoadNode;
				pWindSeg->nodePtrArr[i]->WindLoad.y+=Wy/nSumAllocLoadNode;
				if(pExport)
				{
					pLoadNode=pExport->loadNodeList.Add(pWindSeg->nodePtrArr[i]->point_i);
					pLoadNode->nodeI=pWindSeg->nodePtrArr[i]->point_i;
					pLoadNode->Fwx=Wx/nSumAllocLoadNode/1000;
					pLoadNode->Fwy=Wy/nSumAllocLoadNode/1000;
				}
				MIRMSG mirmsg;
				if(pWindSeg->cCalStyle==1)	//X轴向横担
					mirmsg.axis_flag=0x01;
				else if(pWindSeg->cCalStyle==2)
					mirmsg.axis_flag=0x02;
				mirmsg.origin=pWindSeg->project_origin;
				mir_node=pWindSeg->nodePtrArr[i]->GetMirNode(mirmsg);
				if(mir_node)
				{
					mir_node->WindLoad.x+=Wx/nSumAllocLoadNode;
					mir_node->WindLoad.y+=Wy/nSumAllocLoadNode;
					if(pExport)
					{
						pLoadNode=pExport->loadNodeList.Add(mir_node->point_i);
						pLoadNode->nodeI=mir_node->point_i;
						pLoadNode->Fwx=Wx/nSumAllocLoadNode/1000;
						pLoadNode->Fwy=Wy/nSumAllocLoadNode/1000;
					}
				}
				if(pWindSeg->cMirType==1||pWindSeg->cMirType==2)
				{
					if(pWindSeg->cMirType==1)
						mirmsg.axis_flag=0x02;	//Y轴对称风压段
					else if(pWindSeg->cMirType==2)
						mirmsg.axis_flag=0x01;	//X轴对称风压段
					mirmsg.origin.Set();
					CLDSNode *pMirNode=pWindSeg->nodePtrArr[i]->GetMirNode(mirmsg);
					if(pMirNode)
					{
						pMirNode->WindLoad.x+=Wx/nSumAllocLoadNode;
						pMirNode->WindLoad.y+=Wy/nSumAllocLoadNode;
						if(pExport)
						{
							pLoadNode=pExport->loadNodeList.Add(pMirNode->point_i);
							pLoadNode->nodeI=pMirNode->point_i;
							pLoadNode->Fwx=Wx/nSumAllocLoadNode/1000;
							pLoadNode->Fwy=Wy/nSumAllocLoadNode/1000;
						}
					}
					if(mir_node)
					{
						pMirNode=mir_node->GetMirNode(mirmsg);
						if(pMirNode)
						{
							pMirNode->WindLoad.x+=Wx/nSumAllocLoadNode;
							pMirNode->WindLoad.y+=Wy/nSumAllocLoadNode;
							if(pExport)
							{
								pLoadNode=pExport->loadNodeList.Add(pMirNode->point_i);
								pLoadNode->nodeI=pMirNode->point_i;
								pLoadNode->Fwx=Wx/nSumAllocLoadNode/1000;
								pLoadNode->Fwy=Wy/nSumAllocLoadNode/1000;
							}
						}
					}
				}
			}
		}
		else if(pWindSeg->cType==2&&nValidNode==4)	//六点分配三角塔身段
		{
			if(pWindSeg->nodePtrArr[0]&&pWindSeg->nodePtrArr[1]&&pWindSeg->nodePtrArr[2]&&pWindSeg->nodePtrArr[3])
			{
				pWindSeg->nodePtrArr[0]->WindLoad.x+=Wx/6;
				pWindSeg->nodePtrArr[0]->WindLoad.y+=Wy/6;
				pWindSeg->nodePtrArr[1]->WindLoad.x+=Wx/6;
				pWindSeg->nodePtrArr[1]->WindLoad.y+=Wy/6;
				pWindSeg->nodePtrArr[2]->WindLoad.x+=Wx/6;
				pWindSeg->nodePtrArr[2]->WindLoad.y+=Wy/6;
				pWindSeg->nodePtrArr[3]->WindLoad.x+=Wx/6;
				pWindSeg->nodePtrArr[3]->WindLoad.y+=Wy/6;
				f3dPoint v_vec(0,0,1),h_vec,prj_norm;
				h_vec=pWindSeg->nodePtrArr[3]->Position(false)-pWindSeg->nodePtrArr[1]->Position(false);
				prj_norm=v_vec^h_vec;
				prj_norm.z=0;
				normalize(prj_norm);
				CLDSNode *pUpBackNode=NULL,*pDownBackNode=NULL;
				double dist,distUpBackNode=0,distDownBackNode=0;
				for(pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
				{
					if(fabs(pNode->Position(false).z-pWindSeg->nodePtrArr[0]->Position(false).z)<EPS&&
						pNode!=pWindSeg->nodePtrArr[0]&&pNode!=pWindSeg->nodePtrArr[2])
					{
						dist=DistOf3dPtLine(pNode->Position(false),pWindSeg->nodePtrArr[0]->Position(false),pWindSeg->nodePtrArr[2]->Position(false));
						if(dist>distUpBackNode)
						{
							distUpBackNode=dist;
							pUpBackNode=pNode;
						}
					}
					if(fabs(pNode->Position(false).z-pWindSeg->nodePtrArr[1]->Position(false).z)<EPS&&
						pNode!=pWindSeg->nodePtrArr[1]&&pNode!=pWindSeg->nodePtrArr[3])
					{
						dist=DistOf3dPtLine(pNode->Position(false),pWindSeg->nodePtrArr[1]->Position(false),pWindSeg->nodePtrArr[3]->Position(false));
						if(dist>distDownBackNode)
						{
							distDownBackNode=dist;
							pDownBackNode=pNode;
						}
					}
				}
				if(pExport)
				{
					for(int i=0;i<4;i++)
					{
						pLoadNode=pExport->loadNodeList.Add(pWindSeg->nodePtrArr[i]->point_i);
						pLoadNode->nodeI=pWindSeg->nodePtrArr[i]->point_i;
						pLoadNode->Fwx=Wx/6000;
						pLoadNode->Fwy=Wy/6000;
					}
				}
				if(pUpBackNode)
				{
					pUpBackNode->WindLoad.x+=Wx/6;
					pUpBackNode->WindLoad.y+=Wy/6;
					if(pExport)
					{
						pLoadNode=pExport->loadNodeList.Add(pUpBackNode->point_i);
						pLoadNode->nodeI=pUpBackNode->point_i;
						pLoadNode->Fwx=Wx/6000;
						pLoadNode->Fwy=Wy/6000;
					}
				}
				if(pDownBackNode)
				{
					pDownBackNode->WindLoad.x+=Wx/6;
					pDownBackNode->WindLoad.y+=Wy/6;
					if(pExport)
					{
						pLoadNode=pExport->loadNodeList.Add(pDownBackNode->point_i);
						pLoadNode->nodeI=pDownBackNode->point_i;
						pLoadNode->Fwx=Wx/6000;
						pLoadNode->Fwy=Wy/6000;
					}
				}
			}
		}
		for(i=0;i<8;i++)
		{
			if(pWindSeg->nodePtrArr[i]&&fabs(pWindSeg->nodePtrArr[i]->WindLoad.x)>10000000)
				logerr.Log("%d号节点在进行%d#号风压段风荷载计算时，出现X向风载{%.0fkN}异常，请核实!",pWindSeg->nodePtrArr[i]->point_i,pWindSeg->nodePtrArr[i]->WindLoad.x*0.001);
			if(pWindSeg->nodePtrArr[i]&&fabs(pWindSeg->nodePtrArr[i]->WindLoad.y)>10000000)
				logerr.Log("%d号节点在进行%d#号风压段风荷载计算时，出现Y向风载{%.0fkN}异常，请核实!",pWindSeg->nodePtrArr[i]->point_i,pWindSeg->nodePtrArr[i]->WindLoad.y*0.001);
		}
	}
	for(CWorkStatus::WINDLOAD_REFPARAM *pRefParam=pStatus->AttachWindLoad.GetFirst();pRefParam;
	pRefParam=pStatus->AttachWindLoad.GetNext())
	{
		CAttachWindLoad *pWindLoad=pTower->AttachWindLoad.FromHandle(pStatus->AttachWindLoad.GetCursorKey());
		f3dPoint load=pRefParam->basicAttachWindLoadF;
		if(bForFoundation)
			load*=pWindLoad->m_fBetaZForFoundation/pWindLoad->m_fBetaZ;
		for(long *pH=pWindLoad->GetNodeList()->GetFirst();pH;pH=pWindLoad->GetNodeList()->GetNext())
		{
			pNode=pTower->Node.FromHandle(*pH);
			if(pNode&&pNode->handle==*pH)
			{
				double high_pos=(lowest_z-pNode->Position(false).z)/1000+pModule->land_high;	//1000 for mm->m
				double mu_z=Find_MuZ(high_pos,pTower->m_cLandscape);	//查找风压高度变化系数
				pNode->WindLoad.x+=load.x*mu_z;
				pNode->WindLoad.y+=load.y*mu_z;
				pNode->attachMass+=pRefParam->attachMass;//*9.8;
			}
		}
	}
}
bool CPreProcessor::ApplyNodeLoad(ATOM_LIST<FEM_NODE> &nodeList,CLDSModule *pModule,CWorkStatus *pStatus,bool bApplyStdLoad)
{
	CLDSNode *pNode;
	double ice_thick_2=pStatus->ice_thick*pStatus->ice_thick;
	for(FEM_NODE *pFemNode=nodeList.GetFirst();pFemNode;pFemNode=nodeList.GetNext())
	{
		if(!hashTowerNodeI.GetValueAt(pFemNode->point_i,pNode))
			continue;
		//一、获得各项节点等效荷载
			//1.与工况相关的集中外荷载(含可变荷载及Ｚ向持久重力荷载)
		CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
			//2.计算自重体荷载（含塔身覆冰荷载）
		double selfIceWeightLoad=0;								//杆塔本身覆冰荷载(N)
		f3dPoint selfWeightLoad(0,0,pNode->bodyLoad.selfWeightLoad);	//杆塔本身自重荷载
		if(pStatus->ice_thick>0)
		{	//重覆冰规范中12.0.10节规定，杆塔覆冰重力荷载采取原重力荷载增大系数修正 wjh-2013.12.30
			if(CPreProcessor::ActiveDesignCode->CodeSerial()==CODES::GB50135_2006)
				selfIceWeightLoad=pNode->bodyLoad.iceWeightLoadCoef*pStatus->ice_thick+pNode->bodyLoad.iceWeightLoadCoef2*ice_thick_2;
			else
				selfIceWeightLoad=pNode->bodyLoad.selfWeightLoad*(g_sysPara.IceLoadCoefInterpByThick(pStatus->ice_thick)-1);
		}
		//二、向有限元节点施加荷载
		f3dPoint wireLoad;
		//1.分类汇总永久荷载
		f3dPoint permanentLoad=selfWeightLoad+pNode->PermanentLoad;//直接施加于节点上的集中持久外荷载
		permanentLoad.z+=pNode->attachMass*9.8;		//累加附属件(平台、馈线等)重力荷载
		if(pStatus->m_bLoadIncFactor)
		{	//直调应用已包含各项系数的外加导地线集中荷载值
			//2.分类汇总可变荷载
			if(pNodeLoad)
			{
				if(bApplyStdLoad)
					wireLoad=pNodeLoad->stdLoad;
				else
					wireLoad=pNodeLoad->designLoad;
			}
			wireLoad+=pNode->vSwingLoadFxyz;	//累加由摆动节点等效过来的临时导地线荷载（同wireLoad一样包含了各项系数）
			//3.杆塔风荷载：pNode->WindLoad
			f3dPoint iceLoad(0,0,selfIceWeightLoad);	//可变荷载中的杆塔覆冰重力荷载
			if(bApplyStdLoad)
				pFemNode->Fxyz=wireLoad+permanentLoad+iceLoad+pNode->WindLoad;
			else
			{
				double safetyCoef=1.0;
				if(pStatus->m_cMainLoadType<4)
					safetyCoef=Ta.SafetyCoef();	//安装及抗震验算工况不需要考虑结构重要性系数
				pFemNode->Fxyz= wireLoad+safetyCoef*(Ta.m_fGamaGForDesign*permanentLoad+
					pStatus->Kesai_I()*pStatus->GamaQ_I()*iceLoad+
					pStatus->Kesai_W()*pStatus->GamaQ_W()*pNode->WindLoad);
			}
		}
		else
		{
			if(pNodeLoad)
				permanentLoad.z+=pNodeLoad->permanentFz;	//累加导地线上传来的自重荷载
			//2.分类汇总可变荷载
			f3dPoint iceLoad(0,0,selfIceWeightLoad);	//可变荷载中的杆塔覆冰重力荷载
			if(pNodeLoad)
			{
				wireLoad=pNodeLoad->Fxyz;				//导地线等外力施加可变荷载
				pNodeLoad->designLoad=Ta.m_fGamaGForDesign*permanentLoad+pStatus->Kesai()*pStatus->GamaQ()*wireLoad;
				if(pStatus->m_cMainLoadType<4)
					pNodeLoad->designLoad*=Ta.SafetyCoef();	//安装及抗震验算工况不需要考虑结构重要性系数
			}
			if(result.bHasSwingNode)
				wireLoad+=pNode->vSwingLoadFxyz;			//累加由摆动节点等效过来的临时导地线荷载
			//3.杆塔风荷载：pNode->WindLoad
			if(bApplyStdLoad)
				pFemNode->Fxyz=permanentLoad+wireLoad+iceLoad+pNode->WindLoad;
			else
			{
				pFemNode->Fxyz= Ta.m_fGamaGForDesign*permanentLoad+
					pStatus->Kesai()*pStatus->GamaQ()*wireLoad+
					pStatus->Kesai_I()*pStatus->GamaQ_I()*iceLoad+
					pStatus->Kesai_W()*pStatus->GamaQ_W()*pNode->WindLoad;
				if(pStatus->m_cMainLoadType<4)
					pFemNode->Fxyz*=Ta.SafetyCoef();	//安装及抗震验算工况不需要考虑结构重要性系数
			}
		}
		if(pStatus->m_cMainLoadType>=5)
		{	//考虑地震验算荷载
			pFemNode->Fxyz.x+=pNode->Feq.x*pStatus->GamaQ_Eh();
			pFemNode->Fxyz.y+=pNode->Feq.y*pStatus->GamaQ_Eh();
			pFemNode->Fxyz.z+=pNode->Feq.z*pStatus->GamaQ_Ev();
		}
	}
	return true;
}
long CPreProcessor::GetFatherElemId(CElement* pElem)
{
	CElement *pMirElem=NULL;
	if(pElem->relaElem.bFatherElem)
		return pElem->id;//elem_id;
	if(result.hashElem.GetValueAt(pElem->relaElem.idElemMirX,pMirElem)&&pMirElem->relaElem.bFatherElem)
		return pMirElem->id;
	if(result.hashElem.GetValueAt(pElem->relaElem.idElemMirY,pMirElem)&&pMirElem->relaElem.bFatherElem)
		return pMirElem->id;
	if(result.hashElem.GetValueAt(pElem->relaElem.idElemMirZ,pMirElem)&&pMirElem->relaElem.bFatherElem)
		return pMirElem->id;
	return 0;
}
bool RetrieveSideProfileParams(NODESET& nodeset,const double* prjorg,CLDSNodePtr upNode[2],CLDSNodePtr downNode[2],double* b_a)
{
	CLDSNode* pNode,*pFirstNode=nodeset.GetFirst();
	if(pFirstNode==NULL)
		return false;
	CMaxDouble lowestZ=pFirstNode->Position('Z');
	CMinDouble highestZ=lowestZ;
	CLDSNode* pPrevNode=nodeset.GetTail();
	CLDSNode* pTipNode=pPrevNode;
	for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{
		double z=pNode->Position('Z');
		lowestZ.Update(z);
		highestZ.Update(z);
	}
	double height=lowestZ.number-highestZ.number;
	double side_h=0;
	CLDSNodePtr sidenodes[2];
	double grossArea=0;
	double weight_deitax=0;
	for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{
		double h=fabs(pNode->Position('Z')-pPrevNode->Position('Z'));
		if(h>side_h)
		{
			sidenodes[0]=pNode;
			sidenodes[1]=pPrevNode;
			side_h=h;
		}
		GEPOINT axis_vec1,axis_vec2,axis_z;
		double cosaPrjZ=0;
		GEPOINT vertexs[3]={pTipNode->Position(false),pPrevNode->Position(false),pNode->Position(false)};
		if(pTipNode!=pPrevNode&&pTipNode!=pNode)
		{
			double area=CalTriArea(vertexs[0].x,vertexs[0].z,vertexs[1].x,vertexs[1].z,vertexs[2].x,vertexs[2].z);
			if(axis_z.IsZero())
			{
				axis_vec1=vertexs[1]-vertexs[0];
				axis_vec2=vertexs[2]-vertexs[0];
				axis_z=axis_vec1^axis_vec2;
				normalize(axis_z);
				cosaPrjZ=sqrt(axis_z.x*axis_z.x+axis_z.y*axis_z.y);
			}
			double highz=min(vertexs[0].z,vertexs[1].z);
			highz=min(highz,vertexs[2].z);
			double lowz=max(vertexs[0].z,vertexs[1].z);
			lowz=max(lowz,vertexs[2].z);
			if((lowz-highz)>0)	//*cosaPrjZ:用于考虑挡风面积在Ｚ轴向的投影
				weight_deitax+=area*cosaPrjZ*area/(lowz-highz);
			grossArea+=area;
		}

		pPrevNode=pNode;
	}

	if(side_h<height*0.8)	//找不到足够包容风压段整高80%的轮廓边
		return false;
	upNode[0]=sidenodes[0];
	GEPOINT backpos(upNode[0]->Position('X',false),prjorg[1]*2-upNode[0]->Position('Y',false),upNode[0]->Position('Z',false));
	BOOL pushed=Ta.Node.push_stack();
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(!pNode->ModuleTogetherWith(upNode[0]))
			continue;
		GEPOINT pos=pNode->Position(false);
		if(!backpos.IsEqual(pos.x,pos.y,pos.z,0.01))
			continue;
		upNode[1]=pNode;
		break;
	}
	downNode[0]=sidenodes[1];
	backpos=GEPOINT(downNode[0]->Position('X',false),prjorg[1]*2-downNode[0]->Position('Y',false),downNode[0]->Position('Z',false));
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(!pNode->ModuleTogetherWith(downNode[0]))
			continue;
		GEPOINT pos=pNode->Position(false);
		if(!backpos.IsEqual(pos.x,pos.y,pos.z,0.01))
			continue;
		downNode[1]=pNode;
		break;
	}
	Ta.Node.pop_stack(pushed);
	if(upNode[1]==NULL||downNode[1]==NULL)
		return false;
	if(b_a)
	{
		double a1=fabs(upNode[1]->Position('Y',false)-upNode[0]->Position('Y',false));
		double a2=fabs(downNode[1]->Position('Y',false)-downNode[0]->Position('Y',false));
		double a=0.5*(a1+a2);	//侧面的迎风面宽度
		double b=weight_deitax/grossArea;	//加权平均进深（X轴向）
		*b_a=b/a;
	}
	return true;
}

void CPreProcessor::InitWindSegElemPicker(CTower *pTower)
{
	for(CWindSegment *pWindSeg=pTower->WindLoadSeg.GetFirst();pWindSeg;pWindSeg=pTower->WindLoadSeg.GetNext())
	{
		pWindSeg->FaiFront=pWindSeg->FaiSide=pWindSeg->B_AFront=pWindSeg->B_ASide=0.0;
		CWindSegElemPicker *pSegPicker;
		pWindSeg->pFrontElemPicker=pWindSeg->pSideElemPicker=NULL;
		memset(pWindSeg->nodePtrArr,0,sizeof(pWindSeg->nodePtrArr));
		int i,nValidNode=pWindSeg->StatValidNodeCount(pTower);
		if(pWindSeg->cType==0)	//八点分配塔身段
		{
			NODESET nodeset;
			if(pWindSeg->nodePtrArr[0]==NULL||pWindSeg->nodePtrArr[1]==NULL)
				pWindSeg->AfFront=pWindSeg->AfSide=0;	//条件不满足
			else
			{
				nodeset.append(pWindSeg->nodePtrArr[0]);
				nodeset.append(pWindSeg->nodePtrArr[1]);
				//Y轴对称
				MIRMSG mir_y(2);
				mir_y.origin=pWindSeg->project_origin;
				pWindSeg->nodePtrArr[2]=pWindSeg->nodePtrArr[1]->GetMirNode(mir_y);
				pWindSeg->nodePtrArr[3]=pWindSeg->nodePtrArr[0]->GetMirNode(mir_y);
				if(pWindSeg->nodePtrArr[2]==NULL||pWindSeg->nodePtrArr[3]==NULL)
					pWindSeg->AfFront=pWindSeg->AfSide=0;	//条件不满足
				else	//满足条件
				{
					nodeset.append(pWindSeg->nodePtrArr[2]);
					nodeset.append(pWindSeg->nodePtrArr[3]);
					pWindSeg->pFrontElemPicker=pSegPicker=windSegPickerList.append();
					pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
					pSegPicker->StartElemPicker(result.elemList,NULL);
				}
				CLDSNode *baknode[2];
				//X轴对称
				MIRMSG mir_x(1);
				mir_x.origin=pWindSeg->project_origin;
				baknode[0]=pWindSeg->nodePtrArr[1]->GetMirNode(mir_x);
				baknode[1]=pWindSeg->nodePtrArr[0]->GetMirNode(mir_x);
				if(nodeset.GetNodeNum()>=4)
				{
					nodeset.DeleteNode(pWindSeg->nodePtrArr[3]->handle);
					nodeset.DeleteNode(pWindSeg->nodePtrArr[2]->handle);
				}
				if(baknode[0]&&baknode[1])	//满足条件
				{
					nodeset.append(baknode[0]);
					nodeset.append(baknode[1]);
					pWindSeg->pSideElemPicker=pSegPicker=windSegPickerList.append();
					pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
					pSegPicker->StartElemPicker(result.elemList,NULL);
				}
				else	//条件不满足
					pWindSeg->AfSide=0;
				if(pWindSeg->cCalStyle==1)	//X向水平横担
					pWindSeg->AfSide=0;
				else if(pWindSeg->cCalStyle==2)	//Y向水平横担
					pWindSeg->AfFront=0;
			}
		}
		else if(pWindSeg->cType==1)	//自定义多节点轮廓
		{
			NODESET nodeset;
			pWindSeg->FaiFront=pWindSeg->FaiSide=pWindSeg->B_AFront=pWindSeg->B_ASide=0;
			if(nValidNode<3)
				pWindSeg->AfFront=pWindSeg->AfSide=0;	//条件不满足
			else
			{
				for(i=0;i<nValidNode;i++)
					nodeset.append(pWindSeg->nodePtrArr[i]);
				if(pWindSeg->cCalStyle==0||pWindSeg->cCalStyle==1)	//X轴向横担
				{
					pWindSeg->pFrontElemPicker=pSegPicker=windSegPickerList.append();
					pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
					pSegPicker->StartElemPicker(result.elemList,NULL);
					if(pWindSeg->cCalStyle==1||nValidNode!=4)
						pWindSeg->AfSide =0.0;
					else	//曲臂等塔头风压段，也需要按塔身方式计算侧面挡风面积
					{
						CLDSNodePtr node_arr[4]={NULL};
						for(i=0;i<4;i++)
						{
							for(int j=0;j<4;j++)
							{
								if(node_arr[j]==NULL)
								{
									node_arr[j]=pWindSeg->nodePtrArr[i];
									break;
								}
								else if(pWindSeg->nodePtrArr[i]->Position(false).z<node_arr[j]->Position(false).z)
								{
									memmove(&node_arr[j+1],&node_arr[j],4*(3-j));
									node_arr[j]=pWindSeg->nodePtrArr[i];
									break;
								}
							}
						}
						nodeset.Empty();
						CLDSNode* pUpBkNode,*pDownBkNode;
						if(node_arr[0]->Position(false).x>node_arr[1]->Position(false).x)
							pUpBkNode=nodeset.append(node_arr[0]);	//
						else
							pUpBkNode=nodeset.append(node_arr[1]);
						if(node_arr[2]->Position(false).x>node_arr[3]->Position(false).x)
							pDownBkNode=nodeset.append(node_arr[2]);	//
						else
							pDownBkNode=nodeset.append(node_arr[3]);
						MIRMSG mir_x(1);
						mir_x.origin=pWindSeg->project_origin;
						pUpBkNode=pUpBkNode->GetMirNode(mir_x);
						pDownBkNode=pDownBkNode->GetMirNode(mir_x);
						if(pUpBkNode&&pDownBkNode)
						{
							nodeset.append(pDownBkNode);
							nodeset.append(pUpBkNode);
							pWindSeg->pSideElemPicker=pSegPicker=windSegPickerList.append();
							pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
							pSegPicker->StartElemPicker(result.elemList,NULL);
						}
					}
					double b_a=1;
					CLDSNodePtr upNodes[2],downNodes[2];
					if( pWindSeg->cCalStyle==0&&
						RetrieveSideProfileParams(nodeset,pWindSeg->project_origin,upNodes,downNodes,&b_a))
					{	//多点轮廓点按照塔身计算时也需要添加挡风侧面
						pWindSeg->pSideElemPicker=pSegPicker=windSegPickerList.append();
						NODESET sidenodes;
						sidenodes.append(upNodes[0]);
						sidenodes.append(downNodes[0]);
						sidenodes.append(downNodes[1]);
						sidenodes.append(upNodes[1]);
						pSegPicker->InitElemPicker(sidenodes,pWindSeg->project_origin);
						pSegPicker->StartElemPicker(result.elemList,NULL);
						pSegPicker->b_a=b_a;
					}
				}
				else// if(pWindSeg->cCalStyle==2)//Y轴向横担
				{	
					pWindSeg->pSideElemPicker=pSegPicker=windSegPickerList.append();
					pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
					pSegPicker->StartElemPicker(result.elemList,NULL);
					pWindSeg->AfFront=0.0;
				}
			}
		}
		else if(pWindSeg->cType==2)	//六点分配三角塔身
		{
			NODESET nodeset;
			if(pWindSeg->nodePtrArr[0]==NULL||pWindSeg->nodePtrArr[1]==NULL||pWindSeg->nodePtrArr[2]==NULL||pWindSeg->nodePtrArr[3]==NULL)
				pWindSeg->AfFront=pWindSeg->AfSide=0;	//条件不满足
			else
			{
				nodeset.append(pWindSeg->nodePtrArr[0]);
				nodeset.append(pWindSeg->nodePtrArr[1]);
				nodeset.append(pWindSeg->nodePtrArr[2]);
				nodeset.append(pWindSeg->nodePtrArr[3]);
				pWindSeg->pFrontElemPicker=pSegPicker=windSegPickerList.append();
				pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
				pSegPicker->StartElemPicker(result.elemList,NULL);
			}
		}
	}
}
//<DEVELOP_PROCESS_MARK nameId="Global.CalLinePartCoef">
static void CalLinePartCoef(AUXELEM_CONST elemConstArr[],int nNum)
{
	double coef_arr[4] = {0};
	int i = 0;
	//A*Δx+B*Δy=N
	//C*Δx+(D+K)*Δy=0
	//由于工程中假定K>>A,B,C,D,所以Δy很小，可以忽略为0
	for(i=0;i<nNum;i++)
	{
		coef_arr[0] += elemConstArr[i].k_xcoef*elemConstArr[i].A;
		//coef_arr[1] += elemConstArr[i].k*elemConstArr[i].sina*elemConstArr[i].cosa;
		//coef_arr[2] += elemConstArr[i].k*elemConstArr[i].cosa*elemConstArr[i].sina;
		//coef_arr[3] += elemConstArr[i].k*elemConstArr[i].sina*elemConstArr[i].sina;	
	}
	//double dd=(coef_arr[0]*coef_arr[3]-coef_arr[1]*coef_arr[2]);
	//double deita_x = coef_arr[3]/dd;
	//double deita_y =-coef_arr[2]/dd;
	for(i=0;i<nNum;i++)
		elemConstArr[i].brace_coef = elemConstArr[i].k_lcoef*elemConstArr[i].A/coef_arr[0];
		//elemConstArr[i].brace_coef = (deita_x*elemConstArr[i].cosa+deita_y*elemConstArr[i].sina)*elemConstArr[i].k;
}
//</DEVELOP_PROCESS_MARK>
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::CalAuxElemBraceForcePreInfo">
void CPreProcessor::CalAuxElemBraceForcePreInfo()
{
	int i;
	CLDSNode *pNode;
	CElement *pElem;
	ARRAY_LIST<CElement*>auxElemPtrArr;
	//一、建立辅材单元集合
	auxElemPtrArr.SetSize(0,3);
	for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		if( pElem->pOrgPole->IsAuxPole()&&
			pElem->pOrgPole->GetArea()>0&&pElem->pOrgPole->GetElasticityModulus()>0)
			auxElemPtrArr.append(pElem);
	}
	//二、建立非传力辅助材端节点集合
	//现在本算法还有问题未考虑到平面受力时垂直面方向支撑辅材受压验算的考虑 wjh-2010-11-3
	CHashList<CLDSNode*>auxNodePtrList;
	CHashList<CElement*>hashBraceForceElem;
	for(i=0;i<auxElemPtrArr.GetSize();i++)
	{
		auxElemPtrArr[i]->feature=0;	//清空处理标记
		if(// auxElemPtrArr[i]->pStart->feature==0&&	//pNode->feature==0	//表示非受力材端点
			!auxNodePtrList.GetValue(auxElemPtrArr[i]->pStart->handle))
			auxNodePtrList.SetValue(auxElemPtrArr[i]->pStart->handle,auxElemPtrArr[i]->pStart);
		if(// auxElemPtrArr[i]->pEnd->feature==0&&
			!auxNodePtrList.GetValue(auxElemPtrArr[i]->pEnd->handle))
			auxNodePtrList.SetValue(auxElemPtrArr[i]->pEnd->handle,auxElemPtrArr[i]->pEnd);
	}
	//三、搜索辅助材端节点所对应的支撑受力材单元
	for(CLDSNode** ppAuxNode=auxNodePtrList.GetFirst();ppAuxNode;ppAuxNode=auxNodePtrList.GetNext())
	{
		pNode=*ppAuxNode;
		//for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pNode->handle);
		if(pNodeLinks==NULL)
			continue;
		for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
		{
			pElem=pElemStiff->pElem;
			if(pElem->pOrgPole->IsAuxPole())	//辅材单元不受力材单元
				continue;
			else if(pNode->RootFatherPartHandle()!=pElem->pOrgPole->handle)
				continue;	//不是节点父杆件分拆出来的单元
			//if(!pElem->cfgword.And(pNode->cfgword))	//因节点父杆件与单元父杆件一致，同一模型检查已无必要
			double justify=pElem->CheckPoint(pNode->Position(false));
			if(justify<-EPS||justify>1+EPS)	//点不在线段内,包括辅材端点是受力材端点情况（存在辅材支撑面内无受力材情况）
				continue;
			else	//节点在此受力材单元中间，且父杆件一致，为合理支撑受力材单元
				break;
		}
		if(pElemStiff)	//找到辅助材端节点所对应的支撑受力材单元
			hashBraceForceElem.SetValue(pNode->handle,pElem);
	}
	for(i=0;i<auxElemPtrArr.GetSize();i++)
	{
		CElement**ppElem;
		ppElem=hashBraceForceElem.GetValue(auxElemPtrArr[i]->pStart->handle);
		if(ppElem)
			auxElemPtrArr[i]->pStartBraceForceElem=*ppElem;
		ppElem=hashBraceForceElem.GetValue(auxElemPtrArr[i]->pEnd->handle);
		if(ppElem)
			auxElemPtrArr[i]->pEndBraceForceElem=*ppElem;
	}
	ARRAY_LIST<ELEM_PTR>forceElemPtrTmArr;
	for(CLDSNode** ppNode=auxNodePtrList.GetFirst();ppNode;ppNode=auxNodePtrList.GetNext())
	{
		pNode=*ppNode;
		CLDSPart *pPart=pNode->RootFatherPart();
		if(pPart==NULL||!pPart->IsLinePart())
			continue;
		UCS_STRU ucs;
		ucs.axis_y=((CLDSLinePart*)pPart)->pEnd->Position(false)-((CLDSLinePart*)pPart)->pStart->Position(false);
		normalize(ucs.axis_y);
		//找出节点上同时关联的受力材，以便处理排除辅材与受力材共工作面情况(此时受力材支承力系数设为0)
		forceElemPtrTmArr.SetSize(0,8);
		CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pNode->handle);
		for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
		{
			if(pElemStiff->pElem->pOrgPole->handle!=pNode->RootFatherPartHandle()&&!pElemStiff->pElem->pOrgPole->IsAuxPole())
				forceElemPtrTmArr.append(pElemStiff->pElem);
		}
		//具体算法，找出所有与辅材端节点连接的辅材单元，并计算出其支撑原始弹性系数，最后汇总求解出实际支撑系数
		bool bFinished;
		do{	//do循环的目的是为了搜索该节点所有工作面上的辅材连接
			bFinished=true;
			ucs.axis_x.Set();	//清空支撑面坐标系Ｘ轴方向
			ARRAY_LIST<ELEM_PTR>auxElemPtrTmArr;
			ARRAY_LIST<f3dPoint>elemLenVecTmArr;
			auxElemPtrTmArr.SetSize(0,2);
			elemLenVecTmArr.SetSize(0,2);
			for(i=0;i<auxElemPtrArr.GetSize();i++)
			{
				if(auxElemPtrArr[i]->pStart==pNode&&!(auxElemPtrArr[i]->feature&0x01))
				{
					if(ucs.axis_x.IsZero())
					{
						ucs.axis_x=auxElemPtrArr[i]->LenStdVec();
						ucs.axis_z=ucs.axis_x^ucs.axis_y;
						if(normalize(ucs.axis_z))
						{
							bFinished=false;
							ucs.axis_x=ucs.axis_y^ucs.axis_z;
						}
						auxElemPtrTmArr.append(auxElemPtrArr[i]);
						elemLenVecTmArr.append(auxElemPtrArr[i]->LenStdVec());
					}
					else if(auxElemPtrArr[i]->cfgword.And(auxElemPtrArr[0]->cfgword)&&
						fabs(auxElemPtrArr[i]->LenStdVec()*ucs.axis_z)<EPS)
					{
						auxElemPtrTmArr.append(auxElemPtrArr[i]);
						elemLenVecTmArr.append(auxElemPtrArr[i]->LenStdVec());
					}
					else	//不是当前支撑面辅助材单元
						continue;
				}
				else if(auxElemPtrArr[i]->pEnd==pNode&&!(auxElemPtrArr[i]->feature&0x02))
				{
					if(ucs.axis_x.IsZero())
					{
						ucs.axis_x=-auxElemPtrArr[i]->LenStdVec();
						ucs.axis_z=ucs.axis_x^ucs.axis_y;
						if(normalize(ucs.axis_z))
						{
							bFinished=false;
							ucs.axis_x=ucs.axis_y^ucs.axis_z;
						}
						auxElemPtrTmArr.append(auxElemPtrArr[i]);
						elemLenVecTmArr.append(-auxElemPtrArr[i]->LenStdVec());
					}
					else if(auxElemPtrArr[i]->cfgword.And(auxElemPtrArr[0]->cfgword)&&
						fabs(auxElemPtrArr[i]->LenStdVec()*ucs.axis_z)<EPS)
					{
						auxElemPtrTmArr.append(auxElemPtrArr[i]);
						elemLenVecTmArr.append(-auxElemPtrArr[i]->LenStdVec());
					}
					else	//不是当前支撑面辅助材单元
						continue;
				}
			}
			if(!bFinished)
			{
				for(i=0;i<auxElemPtrTmArr.GetSize();i++)
				{
					if(auxElemPtrTmArr[i]->pStart==pNode)
						auxElemPtrTmArr[i]->feature|=0x01;	//始端已处理
					else if(auxElemPtrTmArr[i]->pEnd==pNode)
						auxElemPtrTmArr[i]->feature|=0x02;	//终端已处理
				}
				bool bHasForceSupport=false;
				for(i=0;i<forceElemPtrTmArr.GetSize();i++)
				{
					if(fabs(ucs.axis_y*forceElemPtrTmArr[i]->LenStdVec())<EPS_COS&&fabs(ucs.axis_z*forceElemPtrTmArr[i]->LenStdVec())<EPS)
					{	//找到共面但与被支撑材轴线不共线的受力材单元
						bHasForceSupport=true;
						break;
					}
				}
				CNodeBraceAuxElems *pBrace=NULL;
				if(!bHasForceSupport)
				{
					pBrace=result.listBraceLinks.append();
					pBrace->pNode=pNode;
					pBrace->cfgword=pNode->cfgword;
					pBrace->linkElemsConst.SetSize(0,2);
					bool hasSingleSecondaryBraceElem=false;
					for(i=0;i<auxElemPtrTmArr.GetSize();i++)
					{
						f3dPoint vec=elemLenVecTmArr[i];
						vector_trans(vec,ucs,FALSE,TRUE);
						AUXELEM_CONST *pElemConst=pBrace->linkElemsConst.append();
						pElemConst->pElem=auxElemPtrTmArr[i];
						/*CElemInfo* pElemInfo=Ta.Elems.GetValue(pElemConst->pElem->pStart->handle,pElemConst->pElem->pEnd->handle);
						if(pElemInfo)
							g_pSolidDraw->SetEntSnapStatus(pElemInfo->Id|0x80000000);*/
						if(pElemConst->pElem->pStartBraceForceElem&&pElemConst->pElem->pEndBraceForceElem&&
							!pElemConst->pElem->pStartBraceForceElem->pOrgPole->IsPostRod()&&
							!pElemConst->pElem->pEndBraceForceElem->pOrgPole->IsPostRod())
						{
							if(pBrace->SecondaryElemStiff()==NULL)
							{
								pBrace->m_iSecondaryElemStiff=pBrace->linkElemsConst.GetSize()-1;//pElemConst;
								hasSingleSecondaryBraceElem=true;
							}
							else	//有多个二类辅材支撑时，一般不会出现，暂不支持
							{
								if(hasSingleSecondaryBraceElem)
									logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%d号节点处存在两根以上的二类辅材支撑%d-%d;%d-%d;",pNode->point_i,
									pBrace->SecondaryElemStiff()->pElem->pStart->point_i,
									pBrace->SecondaryElemStiff()->pElem->pEnd->point_i,
									pElemConst->pElem->pStart->point_i,pElemConst->pElem->pEnd->point_i);
								else
									logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%d-%d;",pElemConst->pElem->pStart->point_i,pElemConst->pElem->pEnd->point_i);
								hasSingleSecondaryBraceElem=false;
							}
						}
						pElemConst->sina=vec.y;
						pElemConst->cosa=vec.x;
						pElemConst->k_lcoef=auxElemPtrTmArr[i]->pOrgPole->GetElasticityModulus()*pElemConst->cosa;
						pElemConst->k_xcoef=pElemConst->k_lcoef*pElemConst->cosa;
						pBrace->cfgword&=auxElemPtrTmArr[i]->pOrgPole->cfgword;
					}
					if(!hasSingleSecondaryBraceElem&&pBrace->SecondaryElemStiff())
						pBrace->m_iSecondaryElemStiff=-1;
				}
				//else	//有受力材支撑，辅材支撑系数为0,不需计算
			}
		}while(!bFinished);
	}
}
//</DEVELOP_PROCESS_MARK>
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::CalPrimaryElemBraceForcePreInfo">
//是否为主材上可能的中间支撑节点类型
static bool IsSecondaryNode(BYTE cPosCalType)
{	
	if( cPosCalType==1||cPosCalType==2||cPosCalType==3||	//指定X/Y/Z坐标分量点
		cPosCalType==5||				//比例等分点
		cPosCalType==6||cPosCalType==7)	//偏移点
		return true;
	else
		return false;
}
void CPreProcessor::CalPrimaryElemBraceForcePreInfo()
{
	int i;
	CLDSNode *pNode;
	CElement *pElem;
	ARRAY_LIST<CElement*>primaryElemPtrArr;
	//一、建立辅材单元集合
	primaryElemPtrArr.SetSize(0,3);
	for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		if( pElem->pOrgPole->GetArea()>0&&pElem->pOrgPole->GetElasticityModulus()>0&&pElem->pOrgPole->IsPrimaryRod(true))
			primaryElemPtrArr.append(pElem);
	}
	//二、建立非传力辅助材端节点集合
	//现在本算法还有问题未考虑到平面受力时垂直面方向支撑辅材受压验算的考虑 wjh-2010-11-3
	CHashList<CLDSNode*>primaryNodePtrList;
	CHashList<CElement*>hashBraceForceElem;
	for(i=0;i<primaryElemPtrArr.GetSize();i++)
	{
		primaryElemPtrArr[i]->feature=0;	//清空处理标记
		if(!primaryNodePtrList.GetValue(primaryElemPtrArr[i]->pStart->handle)&&
			IsSecondaryNode(primaryElemPtrArr[i]->pStart->m_cPosCalType)&&
			!primaryElemPtrArr[i]->pStart->IsHasRestrict(ALL_RESTRICT))	//基础节点不考虑支撑作用
			primaryNodePtrList.SetValue(primaryElemPtrArr[i]->pStart->handle,primaryElemPtrArr[i]->pStart);
		if(!primaryNodePtrList.GetValue(primaryElemPtrArr[i]->pEnd->handle)&&
			IsSecondaryNode(primaryElemPtrArr[i]->pEnd->m_cPosCalType)&&
			!primaryElemPtrArr[i]->pEnd->IsHasRestrict(ALL_RESTRICT))	//基础节点不考虑支撑作用
			primaryNodePtrList.SetValue(primaryElemPtrArr[i]->pEnd->handle,primaryElemPtrArr[i]->pEnd);
	}
	//三、搜索斜材端节点所对应的支撑主材单元
	for(CLDSNode** ppPrimaryNode=primaryNodePtrList.GetFirst();ppPrimaryNode;ppPrimaryNode=primaryNodePtrList.GetNext())
	{
		pNode=*ppPrimaryNode;
		//for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pNode->handle);
		if(pNodeLinks==NULL)
			continue;
		for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
		{
			pElem=pElemStiff->pElem;
			if(!pElem->pOrgPole->IsPostRod())	//非主材单元
				continue;
			else if(pNode->RootFatherPartHandle()!=pElem->pOrgPole->handle)
				continue;	//不是节点父杆件分拆出来的单元
			//if(!pElem->cfgword.And(pNode->cfgword))	//因节点父杆件与单元父杆件一致，同一模型检查已无必要
			double justify=pElem->CheckPoint(pNode->Position(false));
			if(justify<-EPS||justify>1+EPS)	//点不在线段内,包括辅材端点是受力材端点情况（存在辅材支撑面内无受力材情况）
				continue;
			else	//节点在此受力材单元中间，且父杆件一致，为合理支撑受力材单元
				break;
		}
		if(pElemStiff)	//找到辅助材端节点所对应的支撑受力材单元
			hashBraceForceElem.SetValue(pNode->handle,pElem);
	}
	for(i=0;i<primaryElemPtrArr.GetSize();i++)
	{
		CElement**ppElem;
		ppElem=hashBraceForceElem.GetValue(primaryElemPtrArr[i]->pStart->handle);
		if(ppElem)
			primaryElemPtrArr[i]->pStartBraceForceElem=*ppElem;
		ppElem=hashBraceForceElem.GetValue(primaryElemPtrArr[i]->pEnd->handle);
		if(ppElem)
			primaryElemPtrArr[i]->pEndBraceForceElem=*ppElem;
	}
	ARRAY_LIST<ELEM_PTR>postElemPtrTmArr;
	for(CLDSNode** ppNode=primaryNodePtrList.GetFirst();ppNode;ppNode=primaryNodePtrList.GetNext())
	{
		pNode=*ppNode;
		CLDSPart *pPart=pNode->RootFatherPart();
		if(pPart==NULL||!pPart->IsLinePart())
			continue;
		UCS_STRU ucs;
		ucs.axis_y=((CLDSLinePart*)pPart)->pEnd->Position(false)-((CLDSLinePart*)pPart)->pStart->Position(false);
		normalize(ucs.axis_y);
		//找出节点上同时关联的受力主材，以便处理排除辅材与受力材共工作面情况(此时受力材支承力系数设为0)
		postElemPtrTmArr.SetSize(0,8);
		CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pNode->handle);
		for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
		{
			if(pElemStiff->pElem->pOrgPole->handle!=pNode->RootFatherPartHandle()&&pElemStiff->pElem->pOrgPole->IsPostRod())
				postElemPtrTmArr.append(pElemStiff->pElem);
		}
		//具体算法，找出所有与辅材端节点连接的辅材单元，并计算出其支撑原始弹性系数，最后汇总求解出实际支撑系数
		bool finished;
		do{	//do循环的目的是为了搜索该节点所有工作面上的辅材连接
			finished=true;
			ucs.axis_x.Set();	//清空支撑面坐标系Ｘ轴方向
			ARRAY_LIST<ELEM_PTR>primaryElemPtrTmArr;
			ARRAY_LIST<f3dPoint>elemLenVecTmArr;
			primaryElemPtrTmArr.SetSize(0,2);
			elemLenVecTmArr.SetSize(0,2);
			for(i=0;i<primaryElemPtrArr.GetSize();i++)
			{
				if(primaryElemPtrArr[i]->pStart==pNode&&!(primaryElemPtrArr[i]->feature&0x01))
				{
					if(ucs.axis_x.IsZero())
					{
						ucs.axis_x=primaryElemPtrArr[i]->LenStdVec();
						ucs.axis_z=ucs.axis_x^ucs.axis_y;
						if(normalize(ucs.axis_z))
						{
							finished=false;
							ucs.axis_x=ucs.axis_y^ucs.axis_z;
						}
						primaryElemPtrTmArr.append(primaryElemPtrArr[i]);
						elemLenVecTmArr.append(primaryElemPtrArr[i]->LenStdVec());
					}
					else if(primaryElemPtrArr[i]->cfgword.And(primaryElemPtrArr[0]->cfgword)&&
						fabs(primaryElemPtrArr[i]->LenStdVec()*ucs.axis_z)<EPS)
					{
						primaryElemPtrTmArr.append(primaryElemPtrArr[i]);
						elemLenVecTmArr.append(primaryElemPtrArr[i]->LenStdVec());
					}
					else	//不是当前支撑面辅助材单元
						continue;
				}
				else if(primaryElemPtrArr[i]->pEnd==pNode&&!(primaryElemPtrArr[i]->feature&0x02))
				{
					if(ucs.axis_x.IsZero())
					{
						ucs.axis_x=-primaryElemPtrArr[i]->LenStdVec();
						ucs.axis_z=ucs.axis_x^ucs.axis_y;
						if(normalize(ucs.axis_z))
						{
							finished=false;
							ucs.axis_x=ucs.axis_y^ucs.axis_z;
						}
						primaryElemPtrTmArr.append(primaryElemPtrArr[i]);
						elemLenVecTmArr.append(-primaryElemPtrArr[i]->LenStdVec());
					}
					else if(primaryElemPtrArr[i]->cfgword.And(primaryElemPtrArr[0]->cfgword)&&
						fabs(primaryElemPtrArr[i]->LenStdVec()*ucs.axis_z)<EPS)
					{
						primaryElemPtrTmArr.append(primaryElemPtrArr[i]);
						elemLenVecTmArr.append(-primaryElemPtrArr[i]->LenStdVec());
					}
					else	//不是当前支撑面辅助材单元
						continue;
				}
			}
			if(!finished)
			{
				for(i=0;i<primaryElemPtrTmArr.GetSize();i++)
				{
					if(primaryElemPtrTmArr[i]->pStart==pNode)
						primaryElemPtrTmArr[i]->feature|=0x01;	//始端已处理
					else if(primaryElemPtrTmArr[i]->pEnd==pNode)
						primaryElemPtrTmArr[i]->feature|=0x02;	//终端已处理
				}
				bool bHasPostSupport=false;
				for(i=0;i<postElemPtrTmArr.GetSize();i++)
				{
					if(fabs(ucs.axis_y*postElemPtrTmArr[i]->LenStdVec())<EPS_COS&&fabs(ucs.axis_z*postElemPtrTmArr[i]->LenStdVec())<EPS)
					{	//找到共面但与被支撑材轴线不共线的主杆受力材单元
						bHasPostSupport=true;
						break;
					}
				}
				CNodeBraceAuxElems *pBrace=NULL;
				if(!bHasPostSupport)
				{
					pBrace=result.listBraceLinks.append();
					pBrace->pNode=pNode;
					pBrace->cfgword=pNode->cfgword;
					pBrace->linkElemsConst.SetSize(0,2);
					//pBrace->pSecondaryElemStiff=NULL;
					for(i=0;i<primaryElemPtrTmArr.GetSize();i++)
					{
						f3dPoint vec=elemLenVecTmArr[i];
						vector_trans(vec,ucs,FALSE,TRUE);
						AUXELEM_CONST *pElemConst=pBrace->linkElemsConst.append();
						pElemConst->pElem=primaryElemPtrTmArr[i];
						/*CElemInfo* pElemInfo=Ta.Elems.GetValue(pElemConst->pElem->pStart->handle,pElemConst->pElem->pEnd->handle);
						if(pElemInfo)
							g_pSolidDraw->SetEntSnapStatus(pElemInfo->Id|0x80000000);*/
						pElemConst->sina=vec.y;
						pElemConst->cosa=vec.x;
						pElemConst->k_lcoef=primaryElemPtrTmArr[i]->pOrgPole->GetElasticityModulus()*pElemConst->cosa;
						pElemConst->k_xcoef=pElemConst->k_lcoef*pElemConst->cosa;
						pBrace->cfgword&=primaryElemPtrTmArr[i]->pOrgPole->cfgword;
					}
				}
			}
		}while(!finished);
	}
}
//</DEVELOP_PROCESS_MARK>
void CPreProcessor::Empty()
{
	result.bHasSwingNode=true;
	windSegPickerList.Empty();
	//为防止数据不一致，执行完windSegPickerList.Empty();后应即时清空m_pTower->WindLoadSeg关联数据 wjh-2015.3.22
	for(CWindSegment *pWindSeg=m_pTower->WindLoadSeg.GetFirst();pWindSeg;pWindSeg=m_pTower->WindLoadSeg.GetNext())
		pWindSeg->pFrontElemPicker=pWindSeg->pSideElemPicker=NULL;
	result.allnodeset.Empty();
	result.allrodset.Empty();
	result.nodeSet.Empty();
	result.elemList.Empty();
	result.hashElem.Empty();
	result.hashElemsMap.Empty();
	result.hashNodeLinks.Empty();
	result.hashElemStartBodyLoad.Empty();
	result.hashElemEndBodyLoad.Empty();
	result.hashNodeBodyLoad.Empty();
	result.listBraceLinks.Empty();
	result.hashRodIntervals.Empty();
	result.hashModuleFrames.Empty();
	LOWEST_ROD_POSZ_INITED=false;
	LOWEST_ROD_POSZ=0.0;
	memset(Ta.EarthQuake.alfa,0,Ta.EarthQuake.nFreqsRank*8);
	for(int i=0;i<Ta.EarthQuake.nFreqsRank;i++)
		Ta.EarthQuake.modes_arr[i].gama_x=Ta.EarthQuake.modes_arr[i].gama_y=0;
}

static int compareNodeOrderFunc(const INTERVAL_NODE& item1,const INTERVAL_NODE& item2)
{
	if(item1.posLen>item2.posLen)
		return 1;
	else if(item1.posLen<item2.posLen)
		return -1;
	else
		return 0;
}
//下面的函数现在已停用，被InitElemLamdaInfo代替，以后确定确实无用时可删除
double GetMaxIntervalInElement(CElement *pElem)
{
	ATOM_LIST<INTERVAL_NODE>list;
	f3dLine line;
	line.startPt=pElem->pStart->Position(false);
	line.endPt=pElem->pEnd->Position(false);
	for(CLDSNode *pNode=pElem->pOrgPole->BelongModel()->EnumNodeFirst();pNode;pNode=pElem->pOrgPole->BelongModel()->EnumNodeNext())
	{
		if(!pElem->pOrgPole->cfgword.And(pNode->cfgword))
			continue;
		if(pNode==pElem->pStart||pNode==pElem->pEnd)
			continue;
		if(line.PtInLine(pNode->Position(false))==2)	//节点在直线上
		{
			double dd=pElem->Length();
			INTERVAL_NODE atom;
			atom.posLen=dd;
			for(INTERVAL_NODE *pIN=list.GetFirst();pIN;pIN=list.GetNext())
			{
				if(dd<=pIN->posLen)
				{
					list.insert(atom);	//需要测试
					break;
				}
			}
			if(pIN==NULL)
				list.append(atom);
		}
	}
	double max_interval=0,last_dist=0;
	for(INTERVAL_NODE *pIN=list.GetFirst();pIN;pIN=list.GetNext())
	{
		max_interval=max(max_interval,pIN->posLen-last_dist);
		last_dist=pIN->posLen;
	}
	max_interval=max(max_interval,pElem->Length()-last_dist);
	return max_interval;
}

//下面的函数现在已停用，被InitElemLamdaInfo代替，以后确定确实无用时可删除
//返回支撑方向数<=2表示平面支撑,3表示三维空间立体支撑
int GetNodeSupportDirection(INTERVAL_NODE *pIntervalNode,CLDSLinePart *pAxisPart,CTower *pTower)
{
	f3dPoint axis_vec=pAxisPart->pEnd->Position(false)-pAxisPart->pStart->Position(false);
	pIntervalNode->supportDirect.Set();
	CLdsPartListStack ps(&pTower->Parts);
	for(CLDSLinePart *pLinePart=pTower->Parts.GetFirstLinePart();pLinePart;pLinePart=pTower->Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
			continue;	//非合理的杆件分析单元、不参预预力学分析的虚构杆件
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
		if(pLinePart==pAxisPart)
			continue;	//轴向基准杆件
		if(pLinePart->pStart==pIntervalNode->pNode||pLinePart->pEnd==pIntervalNode->pNode||
			(pIntervalNode->pNode->m_cPosCalType==4&&
			(pIntervalNode->pNode->arrRelationPole[0]==pLinePart->handle||pIntervalNode->pNode->arrRelationPole[1]==pLinePart->handle)
			))	//杆件端节点或交叉节点依附杆件
		{
			f3dPoint supportDirect=pLinePart->pEnd->Position(false)-pLinePart->pStart->Position(false);
			normalize(supportDirect);
			if(pIntervalNode->supportDirect.IsZero())
			{
				pIntervalNode->supportDirect=supportDirect^axis_vec;
				normalize(pIntervalNode->supportDirect);
			}
			else if(fabs(supportDirect*pIntervalNode->supportDirect)<EPS2)	//同支撑面支撑
				continue;
			else	//发现不同面支撑，即立体空间支撑
			{
				pIntervalNode->bSupport3D=true;
				break;
			}
		}
	}
	if(pIntervalNode->bSupport3D)
		return 3;	//立体空间支撑
	else if(pIntervalNode->supportDirect.IsZero())
		return 1;	//轴向支撑
	else
		return 2;	//平面支撑
}
typedef struct tagSUPPORT_SECTION{
	int start_i,end_i;
	f3dPoint supportDirect;	//为空时表示是立体空间支撑
	tagSUPPORT_SECTION(){start_i=end_i=-1;}
}SUPPORT_SECTION;
static SUPPORT_SECTION FindNextSupportSection(ARRAY_LIST<INTERVAL_NODE> &nodeArr,
									   const f3dPoint &supportDirect,int start_i)
{
	SUPPORT_SECTION section;
	section.start_i=start_i;
	if(nodeArr[start_i].bSupport3D)
		section.supportDirect.Set();
	else if(supportDirect.IsZero())
		section.supportDirect=nodeArr[start_i].supportDirect;
	else
		section.supportDirect=supportDirect;
	for(int i=start_i+1;i<nodeArr.GetSize();i++)
	{
		if(nodeArr[i].bSupport3D)
			break;	//遇到三维立体支撑，结束支撑节间
		if(section.supportDirect.IsZero())	//前无支撑面法线时遇到第一个平面支撑，设定为当前支撑面法线
			section.supportDirect=nodeArr[i].supportDirect;
		//else if(fabs(nodeArr[i].supportDirect*nodeArr[start_i].supportDirect)<EPS_COS2)		//感觉该行代码是以前写错了，故改为下面代码 wjh-2015.5.25
		else if(fabs(nodeArr[i].supportDirect*section.supportDirect)<EPS_COS2)//nodeArr[start_i].supportDirect)<EPS_COS2)
			break;	//遇到非同向支撑，结束支撑节间
	}
	if(i<nodeArr.GetSize())
		section.end_i=i;
	else
		section.end_i=nodeArr.GetSize()-1;
	return section;
}
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::SmartRetrieveCalLenInfo">
//根据已初始化的长细比原始设计信息智能提取各单元的最大支撑区间及支撑类型
void CPreProcessor::SmartRetrieveCalLenInfo()
{
	INTERVAL_NODE *pIntervalNode;
	CRodIncIntervals *pRodIntervals;
	CElement *pElem;
	CXhChar100 error_info;
	ARRAY_LIST<INTERVAL_NODE>nodeArr;
	for(CLDSLinePart *pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
	{
		if(pLinePart->size_idClassType==CLS_CABLE)
			continue;	//拉线单元
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
			continue;	//非合理的杆件分析单元、不参预预力学分析或虚构杆件
#ifdef __ANGLE_PART_INC_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
		pRodIntervals=result.hashRodIntervals.GetValue(pLinePart->handle);
		if(pRodIntervals==NULL)
			continue;
		//1.处理特殊类型长细比计算
		if(pLinePart->CalLenCoef.iTypeNo==1&&pRodIntervals->listElems.GetNodeNum()==1)
		{	//V斜材（考虑同时受压，对应规定中的序号6）
			//A. 同端点邻杆同时受压
			pElem=*pRodIntervals->listElems.GetFirst();
			pElem->smcinfo.smcItems[0].LoRc.coef=0.65;
			if(isdigit(pElem->pStart->layer(2))&&isdigit(pElem->pEnd->layer(2))==0)
				pElem->smcinfo.smcItems[0].cSMCType=SMC_ITEM::SMC_REFER_START;
			else	//默认为考虑终端节点邻杆同时受压
				pElem->smcinfo.smcItems[0].cSMCType=SMC_ITEM::SMC_REFER_END;
			pElem->smcinfo.smcItems[0].LoRc.bCalByElemLen=true;	//考虑同时受压
			//pElem->lamdaRc.R=Rx;	通过cSMCType为正值，默认平行轴为基准轴
			pElem->smcinfo.smcItems[1].cSMCType=pElem->smcinfo.smcItems[2].cSMCType=SMC_ITEM::SMC_NONE;

			//B. 同端点邻杆一拉一压(通过)
		}
		//else if(pLinePart->CalLenCoef.iTypeNo==2)
		//	V斜材（V面连接）（不考虑同时受压，对应规定中的序号7），可完全根据智能判断计算
		else if((pLinePart->CalLenCoef.iTypeNo==3||pLinePart->CalLenCoef.iTypeNo==4)&&
			pRodIntervals->listElems.GetNodeNum()==1)
		{	//V斜材3或4节间带横连杆（对应规定中的序号8、9）
			CSuperSmartPtr<CLDSPart> pHorizPole=pElem->pOrgPole->BelongModel()->FromPartHandle(pElem->pOrgPole->CalLenCoef.hHorizPole);
			pElem=*pRodIntervals->listElems.GetFirst();
			//更新单元原始信息中的参照横连杆单元
			CElemInfo* pElemInfo=Ta.Elems.GetValue(pElem->pStart->handle,pElem->pEnd->handle);
			if(pElemInfo&&pHorizPole.IsHasPtr()&&pHorizPole->IsLinePart()&&
				pHorizPole.LinePartPointer()->pStart&&pHorizPole.LinePartPointer()->pEnd)
			{
				pElemInfo->horiElem.hStart=pHorizPole.LinePartPointer()->pStart->handle;
				pElemInfo->horiElem.hEnd=pHorizPole.LinePartPointer()->pEnd->handle;
			}
			//同时受压信息与杆件规格相关需在UpdateInfoAtElemSizeChange()->CalElemCompStableInfo()中更新计算
			//一拉一压时可完全根据智能判断计算
		}
		else if(pLinePart->CalLenCoef.iTypeNo==5||pLinePart->CalLenCoef.iTypeNo==6)
		{
			pRodIntervals=result.hashRodIntervals.Add(pLinePart->handle);
			for(ELEM_PTR *ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
			{
				pElem=*ppElem;
				pElem->lamdaRyo.pStart=Ta.Node.FromHandle(pLinePart->CalLenCoef.minR.hStartNode);
				pElem->lamdaRyo.pEnd=Ta.Node.FromHandle(pLinePart->CalLenCoef.minR.hEndNode);
				pElem->lamdaRyo.muCoefBasic=pLinePart->CalLenCoef.minR.coef;
				if(pElem->lamdaRyo.muCoefBasic>0&&pElem->lamdaRyo.pStart==NULL)
					pElem->lamdaRyo.pStart=pLinePart->pStart;
				if(pElem->lamdaRyo.muCoefBasic>0&&pElem->lamdaRyo.pEnd==NULL)
					pElem->lamdaRyo.pEnd=pLinePart->pEnd;

				pElem->lamdaRx.pStart=Ta.Node.FromHandle(pLinePart->CalLenCoef.paraR.hStartNode);
				pElem->lamdaRx.pEnd=Ta.Node.FromHandle(pLinePart->CalLenCoef.paraR.hEndNode);
				pElem->lamdaRx.muCoefBasic=pLinePart->CalLenCoef.paraR.coef;
				if(pElem->lamdaRx.muCoefBasic>0&&pElem->lamdaRx.pStart==NULL)
					pElem->lamdaRx.pStart=pLinePart->pStart;
				if(pElem->lamdaRx.muCoefBasic>0&&pElem->lamdaRx.pEnd==NULL)
					pElem->lamdaRx.pEnd=pLinePart->pEnd;
			}
			if(pLinePart->CalLenCoef.iTypeNo==5)
				continue;	//用户指定不需要智能判断
		}

		//2.智能判断计算
		CPtInLineCheck line3d(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false));
		//统计当前杆件上的节点集合
		nodeArr.SetSize(0,10);
		//for(pIntervalNode=hashIntervalNode.GetFirst();pIntervalNode;pIntervalNode=hashIntervalNode.GetNext())
		pRodIntervals=result.hashRodIntervals.Add(pLinePart->handle);
		if(pRodIntervals==NULL)
			continue;
		for(INTERVAL_NODEPTR *pIntervalPtr=pRodIntervals->hashNodes.GetFirst();pIntervalPtr;pIntervalPtr=pRodIntervals->hashNodes.GetNext())
		{
			pIntervalNode=*pIntervalPtr;
			if(pIntervalNode->pNode==pLinePart->pStart)
			{
				pIntervalNode->posLen=0;
				nodeArr.append(*pIntervalNode);
			}
			else if(pIntervalNode->pNode==pLinePart->pEnd)
			{
				pIntervalNode->posLen=1;
				nodeArr.append(*pIntervalNode);
			}
			else if(pIntervalNode->pNode->m_cPosCalType==4||pIntervalNode->pNode->RootFatherPartHandle()==pLinePart->handle)
			{
				pIntervalNode->posLen=line3d.CheckPoint(pIntervalNode->pNode->Position(false));
				if(pIntervalNode->posLen>=-EPS&&pIntervalNode->posLen<=1+EPS)
					nodeArr.append(*pIntervalNode);
			}
		}
		CQuickSort<INTERVAL_NODE>::QuickSort(nodeArr.m_pData,nodeArr.GetSize(),compareNodeOrderFunc);
		int i,start_i=0,end_i=0;
		//逐个单元进行长细比计算类型智能匹配判断
		for(ELEM_PTR *ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
		{
			pElem=*ppElem;
			start_i=end_i=-1;
			for(i=0;i<nodeArr.GetSize();i++)
			{
				if(nodeArr[i].pNode==pElem->pStart)
					start_i=i;
				else if(nodeArr[i].pNode==pElem->pEnd)
					end_i=i;
				if(start_i>=0&&end_i>=0)
					break;
			}
			if(start_i<0)
				throw CXhChar100("%d号节点的父杆件应该是杆件0X%X，而目前不是导致无法正确计算长细比信息,请确认是否未进行节点编号!",pElem->pStart->pointI,pElem->pOrgPole->handle);
			if(end_i<0)
				throw CXhChar100("%d号节点的父杆件应该是杆件0X%X，而目前不是导致无法正确计算长细比信息,请确认是否未进行节点编号!",pElem->pEnd->pointI,pElem->pOrgPole->handle);
			/*
			原希望：调换首末端节点号，统一始端节点号为小号终端为大号，以保持与上面的节点集合排序一致
			现感觉：不用调换，因为单元始末端节点本来就与杆件始末端是一致的，调换了反而错了wjh-2011.11.21
			if(start_i>end_i)
			{
			int temp=start_i;
			start_i=end_i;
			end_i=start_i;
			}*/

			f3dPoint inters_pos;
			CSuperSmartPtr<CLDSLinePart> pRefLinePart;
			bool bHasIntersPt=false;
			if( pElem->pOrgPole->layer(1)!='Z'&&
				pElem->pOrgPole->pStart==pElem->pStart&&pElem->pOrgPole->pEnd==pElem->pEnd)
			{	//判断是否是交叉材同时受压的情况
				if(pElem->pOrgPole->CalLenCoef.hRefPole>0x20)
				{
					CLdsPartListStack ps(&m_pTower->Parts);
					pRefLinePart=(CLDSLinePart*)m_pTower->Parts.FromHandle(pElem->pOrgPole->CalLenCoef.hRefPole);
					if(!bHasIntersPt&&pRefLinePart.IsHasPtr()&&
						Int3dll(pElem->pOrgPole->pStart->Position(false),pElem->pOrgPole->pEnd->Position(false),
						pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)==1)	//有内交点
					{
						bHasIntersPt=true;
						pElem->pOrgPole->CalLenCoef.hRefPole=pRefLinePart->handle;
					}
					else
						throw CXhChar100("在计算交叉材0X%X的长细比时，未能根据指定句柄找到同时受压参照杆件!",pElem->pOrgPole->handle);
				}
				if(!bHasIntersPt)
					pRefLinePart=NULL;
				if(bHasIntersPt)
				{	//有内部交点，为合理的考虑同时受压的交叉材
					//pElem->lamdaRc.pStart=pElem->pOrgPole->pStart;
					//pElem->lamdaRc.pEnd=pElem->pOrgPole->pEnd;
					//pElem->lamdaRc.coef=0.8;
					if(ActiveDesignCode->GetBraceCoefOfPrimaryToPost()>0)
					{
						double L2a=f3dPoint(inters_pos-pElem->pStart->Position(false))*pElem->LenStdVec();
						if(L2a>=0.5*pElem->Length()&&L2a<pElem->Length())
							pElem->smcinfo.smcItems[0].ratioL2toL3=L2a/pElem->Length();
						else if(L2a>=0&&L2a<pElem->Length())	//0<L2a/ELEM_LEN<0.5
							pElem->smcinfo.smcItems[0].ratioL2toL3=1-L2a/pElem->Length();
					}
					pElem->smcinfo.smcItems[0].LoRc.coef=0.8;
					pElem->smcinfo.smcItems[0].cSMCType=SMC_ITEM::SMC_REFER_UDF;
					pElem->smcinfo.smcItems[0].LoRc.hStartNode=pElem->pOrgPole->pStart->handle;
					pElem->smcinfo.smcItems[0].LoRc.hEndNode=pElem->pOrgPole->pEnd->handle;
					pElem->smcinfo.smcItems[0].relaElem.hStart=pRefLinePart->pStart->handle;
					pElem->smcinfo.smcItems[0].relaElem.hEnd=pRefLinePart->pEnd->handle;
					pElem->smcinfo.smcItems[0].RetrieveSmcType(pElem->pStart->handle,pElem->pEnd->handle,m_pTower);
					if(pLinePart->CalLenCoef.iTypeNo==6)
						break;	//指定长细比时的交叉斜材(主要是为了兼容TTA导入数据,缺少塔身辅材情况)wjh-2015.6.03
					CLDSNode *pIntersNode=NULL;
					for(i=0;i<nodeArr.GetSize();i++)
					{
						if(nodeArr[i].pNode->m_cPosCalType==4&&nodeArr[i].pNode->Position(false)==inters_pos)
						{
							pIntersNode=nodeArr[i].pNode;
							break;
						}
					}
					if(pIntersNode)
					{
						double len,lenRy0=0,lenRxa=0;
						if(i==start_i+1)
						{	//此节间无辅材支撑，有可能最小轴危险
							pElem->lamdaRyo.pStart=nodeArr[start_i].pNode;
							pElem->lamdaRyo.pEnd=pIntersNode;
							lenRy0=DISTANCE(nodeArr[start_i].pNode->Position(false),pIntersNode->Position(false));
						}
						else
						{	//此节间有辅材支撑
							pElem->lamdaRx.pStart=nodeArr[start_i].pNode;
							pElem->lamdaRx.pEnd=pIntersNode;
							pElem->lamdaRx.muCoefBasic=1.1;
							lenRxa=DISTANCE(nodeArr[start_i].pNode->Position(false),pIntersNode->Position(false));
						}
						if(i==end_i-1)
						{	//此节间无辅材支撑，有可能最小轴危险
							if(pElem->lamdaRx.pStart==NULL||
								nodeArr[end_i].posLen-nodeArr[i].posLen>nodeArr[i].posLen-nodeArr[start_i].posLen)
							{
								len=DISTANCE(pIntersNode->Position(false),nodeArr[end_i].pNode->Position(false));
								if(len>lenRy0)
								{
									pElem->lamdaRyo.pStart=pIntersNode;
									pElem->lamdaRyo.pEnd=nodeArr[end_i].pNode;
								}
							}
						}
						else
						{	//此节间有辅材支撑
							len=DISTANCE(pIntersNode->Position(false),nodeArr[end_i].pNode->Position(false));
							if(len>lenRxa)
							{
								pElem->lamdaRx.pStart=pIntersNode;
								pElem->lamdaRx.pEnd=nodeArr[end_i].pNode;
								pElem->lamdaRx.muCoefBasic=1.1;
							}
						}
						continue;
					}
					throw CXhChar100("在计算交叉材0X%X的长细比时，未找到交叉节点,请确认定义交叉节点!",pElem->pOrgPole->handle);
				}
				else if( nodeArr.GetSize()<=2||start_i<0||end_i<0)
				{	//典型最小轴计算，一般不会出现start_i<0或end_i<0情况，上面的相关判断出于防错处理考虑
					pElem->lamdaRyo.pStart=pElem->pStart;
					pElem->lamdaRyo.pEnd=pElem->pEnd;
					pElem->lamdaRyo.bCalByElemLen=true;
				}
			}
			else
				pElem->pOrgPole->CalLenCoef.hRefPole=0;

			SUPPORT_SECTION section,sectionRx,sectionRyo;
			if(!nodeArr[start_i].bSupport3D&&start_i>0)
			{	//起始点为平面支撑或无侧向支撑，需要向前延伸追溯
				section.supportDirect=nodeArr[start_i].supportDirect;
				do
				{	//同向支撑继续，不同向支撑终止并设为节间起始
					start_i--;
					if(start_i>0&&section.supportDirect.IsZero())
						section.supportDirect=nodeArr[start_i].supportDirect;
					//未遇到立体支撑，且新节点为同向支撑
				}while(start_i>0&&!nodeArr[start_i].bSupport3D&&
					fabs(section.supportDirect*nodeArr[start_i].supportDirect)>EPS_COS2);
			}
			section.start_i=section.end_i=start_i;	//起始点为立体支撑时不需向前延伸追溯
			int start_search_i=start_i;
			do{
				section=FindNextSupportSection(nodeArr,section.supportDirect,start_search_i);	//??
				if(section.end_i==section.start_i+1)
				{	//最小轴
					if(sectionRyo.start_i<0)
						sectionRyo=section;
					else if(nodeArr[section.end_i].posLen-nodeArr[section.start_i].posLen>
						nodeArr[sectionRyo.end_i].posLen-nodeArr[sectionRyo.start_i].posLen)
						sectionRyo=section;
					else if(fabs(nodeArr[section.end_i].posLen-nodeArr[section.start_i].posLen-
						nodeArr[sectionRyo.end_i].posLen+nodeArr[sectionRyo.start_i].posLen)<EPS2)
					{
						int endpoint_n=0,endpoint_nd=0;
						if(nodeArr[section.start_i].pNode==pElem->pOrgPole->pStart)
							endpoint_n++;
						if(nodeArr[section.end_i].pNode==pElem->pOrgPole->pEnd)
							endpoint_n++;
						if(nodeArr[sectionRyo.start_i].pNode==pElem->pOrgPole->pStart)
							endpoint_nd++;
						if(nodeArr[sectionRyo.end_i].pNode==pElem->pOrgPole->pEnd)
							endpoint_nd++;
						if(endpoint_n>endpoint_nd)
							sectionRyo=section;
					}
				}
				else if(section.start_i<end_i)
				{
					if(sectionRx.start_i<0)
						sectionRx=section;
					else if(nodeArr[section.end_i].posLen-nodeArr[section.start_i].posLen>
						nodeArr[sectionRx.end_i].posLen-nodeArr[sectionRx.start_i].posLen)
						sectionRx=section;
					else if(fabs(nodeArr[section.end_i].posLen-nodeArr[section.start_i].posLen-
						nodeArr[sectionRx.end_i].posLen+nodeArr[sectionRx.start_i].posLen)<EPS2)
					{
						int endpoint_n=0,endpoint_nd=0;
						if(nodeArr[section.start_i].pNode==pElem->pOrgPole->pStart)
							endpoint_n++;
						if(nodeArr[section.end_i].pNode==pElem->pOrgPole->pEnd)
							endpoint_n++;
						if(nodeArr[sectionRx.start_i].pNode==pElem->pOrgPole->pStart)
							endpoint_nd++;
						if(nodeArr[sectionRx.end_i].pNode==pElem->pOrgPole->pEnd)
							endpoint_nd++;
						if(endpoint_n>endpoint_nd)
							sectionRx=section;
					}
				}
				start_search_i=section.end_i;
				if(start_search_i>=end_i)	//节间超出当前单元范围，终止搜索
				{
					if(sectionRyo.start_i>=0&&sectionRyo.end_i>=0)
					{
						pElem->lamdaRyo.pStart=nodeArr[sectionRyo.start_i].pNode;
						pElem->lamdaRyo.pEnd=nodeArr[sectionRyo.end_i].pNode;
					}
					if(sectionRx.start_i>=0&&sectionRx.end_i>=0)
					{
						pElem->lamdaRx.pStart=nodeArr[sectionRx.start_i].pNode;
						pElem->lamdaRx.pEnd=nodeArr[sectionRx.end_i].pNode;
						if(!nodeArr[sectionRx.start_i].bSupport3D||!nodeArr[sectionRx.end_i].bSupport3D)
							pElem->lamdaRx.muCoefBasic=1.2;
					}
					break;
				}
				section.supportDirect.Set();
			}while(true);
			if(sectionRyo.start_i>=0)
			{
				pElem->lamdaRyo.pStart=nodeArr[sectionRyo.start_i].pNode;
				pElem->lamdaRyo.pEnd=nodeArr[sectionRyo.end_i].pNode;
			}
			else
				pElem->lamdaRyo.bCalByElemLen=false;
			if(sectionRx.start_i>=0)
			{
				pElem->lamdaRx.pStart=nodeArr[sectionRx.start_i].pNode;
				pElem->lamdaRx.pEnd=nodeArr[sectionRx.end_i].pNode;
				if(!nodeArr[sectionRx.start_i].bSupport3D||!nodeArr[sectionRx.end_i].bSupport3D)
					pElem->lamdaRx.muCoefBasic=1.2;
				else
					pElem->lamdaRx.muCoefBasic=1.0;
			}
			else
				pElem->lamdaRx.bCalByElemLen=false;
		}
	}
}
//</DEVELOP_PROCESS_MARK>
//初始化各单元的长细比计算信息
void CPreProcessor::InitElemLamdaInfo(bool loadSrcDesignElemInfo/*=true*/)
{
	CLDSNode *pNode;
	INTERVAL_NODE *pIntervalNode;
	CRodIncIntervals *pRodIntervals;
	CElement *pElem;
	INTERVAL_NODE nodePos;
	CXhChar100 error_info;
	CHashList<INTERVAL_NODE>hashIntervalNode;
	nodePos.bSupport3D=false;
	nodePos.posLen=0;
	//1.初始化长细比智能计算原始信息哈希表（支撑节间端点，杆件内支撑节间表）
	for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		if(pElem->elem_type!=0)
			continue;
		INTERVAL_NODE *pStartInterval=hashIntervalNode.Add(pElem->pStart->point_i);
		INTERVAL_NODE *pEndInterval=hashIntervalNode.Add(pElem->pEnd->point_i);
		pRodIntervals=result.hashRodIntervals.Add(pElem->pOrgPole->handle);
		//处理单元始端,将节点添加到单元杆件的内点表中
		pStartInterval->pNode=pElem->pStart;
		pRodIntervals->hashNodes.SetValue(pStartInterval->pNode->point_i,pStartInterval);
		//处理单元终端,将节点添加到单元杆件的内点表中
		pEndInterval->pNode=pElem->pEnd;
		pRodIntervals->hashNodes.SetValue(pEndInterval->pNode->point_i,pEndInterval);

		if(!pStartInterval->bSupport3D)
		{
			if(pStartInterval->supportDirect.IsZero())
				pStartInterval->supportDirect=pElem->LenStdVec();
			else if(pStartInterval->posLen<=1)	//仅轴向支撑
			{
				if(fabs(pStartInterval->supportDirect*pElem->LenStdVec())<EPS_COS)	//出现平面支撑
				{
					pStartInterval->supportDirect=pStartInterval->supportDirect^pElem->LenStdVec();
					normalize(pStartInterval->supportDirect);
					pStartInterval->posLen=2;	//平面支撑
				}
				//else	//依旧是轴向支撑
			}
			else	//当前已是平面支撑
			{
				if(fabs(pStartInterval->supportDirect*pElem->LenStdVec())>EPS2)	//出现立体支撑(与原支撑面法向不垂直)
					pStartInterval->bSupport3D=true;
			}
		}
		if(!pEndInterval->bSupport3D)
		{
			if(pEndInterval->supportDirect.IsZero())
				pEndInterval->supportDirect=pElem->LenStdVec();
			else if(pEndInterval->posLen<=1)	//仅轴向支撑
			{
				if(fabs(pEndInterval->supportDirect*pElem->LenStdVec())<EPS_COS)	//出现平面支撑
				{
					pEndInterval->supportDirect=pEndInterval->supportDirect^pElem->LenStdVec();
					normalize(pEndInterval->supportDirect);
					pEndInterval->posLen=2;	//平面支撑
				}
				//else	//依旧是轴向支撑
			}
			else	//当前已是平面支撑
			{
				if(fabs(pEndInterval->supportDirect*pElem->LenStdVec())>EPS2)	//出现立体支撑(与原支撑面法向不垂直)
					pEndInterval->bSupport3D=true;
			}
		}
	}
	//2.补充非任何单元端节点的交叉节点到支撑节点表
	for(pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
	{
		if(pNode->m_cPosCalType==4)
		{	//
			pIntervalNode=hashIntervalNode.Add(pNode->point_i);
			pIntervalNode->pNode=pNode;
			//将交叉点添加到交叉杆件的内点表中
			if(pNode->arrRelationPole[0]>0x20)
			{
				pRodIntervals=result.hashRodIntervals.Add(pNode->arrRelationPole[0]);
				pRodIntervals->hashNodes.SetValue(pIntervalNode->pNode->point_i,pIntervalNode);
			}
			if(pNode->arrRelationPole[1]>0x20)
			{
				pRodIntervals=result.hashRodIntervals.Add(pNode->arrRelationPole[1]);
				pRodIntervals->hashNodes.SetValue(pIntervalNode->pNode->point_i,pIntervalNode);
			}
		}
	}
	//3.补充单元对内节点的支撑考虑（一类是交叉杆件对交叉点的平面支撑，一类是单元对内节点的轴向支撑），同时补充杆内内支撑节间分隔节点表信息
	for(pIntervalNode=hashIntervalNode.GetFirst();pIntervalNode;pIntervalNode=hashIntervalNode.GetNext())
	{	//3.1 补充交叉面对非三维立体支撑的交叉节点的支撑
		if(!pIntervalNode->bSupport3D&&pIntervalNode->pNode->m_cPosCalType==4)
		{
			CSuperSmartPtr<CLDSPart>pCrossPart1=m_pTower->Parts.FromHandle(pIntervalNode->pNode->arrRelationPole[0]);
			CSuperSmartPtr<CLDSPart>pCrossPart2=m_pTower->Parts.FromHandle(pIntervalNode->pNode->arrRelationPole[1]);
			if(pCrossPart1.IsHasPtr()&&pCrossPart2.IsHasPtr()&&pCrossPart1->IsLinePart()&&pCrossPart2->IsLinePart())
			{
				f3dPoint line_vec1=pCrossPart1.LinePartPointer()->pEnd->Position(false)-pCrossPart1.LinePartPointer()->pStart->Position(false);
				f3dPoint line_vec2=pCrossPart2.LinePartPointer()->pEnd->Position(false)-pCrossPart2.LinePartPointer()->pStart->Position(false);
				f3dPoint cross_norm=line_vec1^line_vec2;
				normalize(cross_norm);
				if(pIntervalNode->supportDirect.IsZero())
				{
					pIntervalNode->supportDirect=cross_norm;
					pIntervalNode->posLen=2;
				}
				else if(pIntervalNode->posLen<=1)	//轴向支撑
				{
					if(pIntervalNode->supportDirect*cross_norm>EPS2)	//出现立体支撑(与交叉支撑面法向不垂直)
						pIntervalNode->bSupport3D=true;
					else
					{
						pIntervalNode->supportDirect=cross_norm;
						pIntervalNode->posLen=2;	//交叉面支撑
					}
				}
				else	//平面支撑
				{
					if(pIntervalNode->supportDirect*cross_norm<EPS_COS2)	//两个非共面支撑构成三维立体支撑
						pIntervalNode->bSupport3D=true;
				}
			}
		}
		//3.2 将单元对内节点的轴向支撑,用以处理辅材端节点不是受力材端节点,故受力材中无辅材端节点的情况
		CSuperSmartPtr<CLDSPart>pFatherPart=pIntervalNode->pNode->RootFatherPart();
		if( pFatherPart.IsHasPtr()&&pFatherPart->IsLinePart()&&
			pFatherPart.LinePartPointer()->pStart&&pFatherPart.LinePartPointer()->pEnd)
		{
			pRodIntervals=result.hashRodIntervals.Add(pFatherPart->handle);
			pRodIntervals->hashNodes.SetValue(pIntervalNode->pNode->point_i,pIntervalNode);
			if(!pIntervalNode->bSupport3D&&
				pIntervalNode->pNode!=pFatherPart.LinePartPointer()->pStart&&
				pIntervalNode->pNode!=pFatherPart.LinePartPointer()->pEnd)
			{	//不是三维立体支撑时，需要补充来自主材的支撑
				f3dPoint lineVec=pFatherPart.LinePartPointer()->pEnd->Position(false)-pFatherPart.LinePartPointer()->pStart->Position(false);
				normalize(lineVec);
				if(pIntervalNode->supportDirect.IsZero())
					pIntervalNode->supportDirect=lineVec;
				else if(pIntervalNode->posLen<=1)
				{
					if(fabs(pIntervalNode->supportDirect*lineVec)<EPS_COS)
					{	//非同向支撑，轴向支撑升级为平面支撑
						pIntervalNode->supportDirect=pIntervalNode->supportDirect^lineVec;
						normalize(pIntervalNode->supportDirect);
						pIntervalNode->posLen=2;	//平面支撑
					}
				}
				else// if(pIntervalNode->posLen==2)
				{	//原为平面支撑
					if(fabs(pIntervalNode->supportDirect*lineVec)>EPS2)
					{	//非同向支撑，平面支撑升级为立体支撑
						pIntervalNode->bSupport3D=true;	//平面支撑
					}
				}
			}
		}
		if(!pIntervalNode->bSupport3D&&pIntervalNode->posLen<=1)
			pIntervalNode->supportDirect.Set();	//轴向支撑视为无支撑，否则会与平面支撑时supportDirect含义相冲突
	}
	//4.智能判断单元的最长支撑区间及支撑类型
	SmartRetrieveCalLenInfo();
	//5.根据需要加载用户自己配置的单元长细比信息
	if(!loadSrcDesignElemInfo)
		return;	//不需加载单元原始设计信息
	//从CTower::Elems中加载预定的单元原始设计信息
	for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		CElemInfo *pElemInfo=m_pTower->Elems.GetValue(pElem->pStart->handle,pElem->pEnd->handle);
		if(pElemInfo==NULL)
			continue;
		if(pElemInfo->IsSpecLoInfo())
		{	//由单元原始设计信息向当前计算单元中同步
			if(pElemInfo->LoRyo.bCalByElemLen)
			{
				pElem->lamdaRyo.pStart=m_pTower->Node.FromHandle(pElemInfo->m_hStartNode);
				pElem->lamdaRyo.pEnd=m_pTower->Node.FromHandle(pElemInfo->m_hEndNode);
				pElem->lamdaRyo.bCalByElemLen=true;
			}
			else
			{
				if(pElemInfo->LoRyo.hStartNode>0)
					pElem->lamdaRyo.pStart=m_pTower->Node.FromHandle(pElemInfo->LoRyo.hStartNode);
				else
					pElem->lamdaRyo.pStart=NULL;
				if(pElemInfo->LoRyo.hEndNode>0)
					pElem->lamdaRyo.pEnd=m_pTower->Node.FromHandle(pElemInfo->LoRyo.hEndNode);
				else
					pElem->lamdaRyo.pEnd=NULL;
			}
			pElem->lamdaRyo.muCoefBasic=pElemInfo->LoRyo.coef;

			if(pElemInfo->LoRx.bCalByElemLen)
			{
				pElem->lamdaRx.pStart=m_pTower->Node.FromHandle(pElemInfo->m_hStartNode);
				pElem->lamdaRx.pEnd=m_pTower->Node.FromHandle(pElemInfo->m_hEndNode);
				pElem->lamdaRx.bCalByElemLen=true;
			}
			else
			{
				if(pElemInfo->LoRx.hStartNode>0)
					pElem->lamdaRx.pStart=m_pTower->Node.FromHandle(pElemInfo->LoRx.hStartNode);
				else
					pElem->lamdaRx.pStart=NULL;
				if(pElemInfo->LoRx.hEndNode>0)
					pElem->lamdaRx.pEnd=m_pTower->Node.FromHandle(pElemInfo->LoRx.hEndNode);
				else
					pElem->lamdaRx.pEnd=NULL;
			}
			pElem->lamdaRx.muCoefBasic=pElemInfo->LoRx.coef;
			//拷贝同时受压信息
			memcpy(pElem->smcinfo.smcItems,pElemInfo->smcItems,sizeof(SMC_ITEM)*3);
		}
		else	//向单元原始设计信息中同步
			pElem->CopyBasicInfoTo(pElemInfo);
	}
}
//<DEVELOP_PROCESS_MARK nameId="Global.CalKlamda">
static double CalKlamdaCore(double lamda,bool bPrimaryRod,int start_joint_type,int end_joint_type,int start_force_type,int end_force_type)
{
	double Klamda=lamda;
	if(!CPreProcessor::ActiveDesignCode->IsIncLamdaK())
		return lamda;
	int joint_type[2],force_type[2];
	joint_type[0]=start_joint_type;
	joint_type[1]=end_joint_type;
	force_type[0]=start_force_type;
	force_type[1]=end_force_type;
	char cRodType='B';	//辅助材
	if(bPrimaryRod)
		cRodType='P';	//受力材
	if(!bPrimaryRod)	//辅助材（不受力材）
	{
		if(lamda<120)
			Klamda=lamda;				//两端偏心受压
		else if(lamda<=250&&start_joint_type==JOINT_HINGE&&end_joint_type==JOINT_HINGE)
			Klamda=lamda;				//两端无约束
		else if(lamda<=330&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
			Klamda=0.615*lamda+46.2;	//两端有约束
		else if(lamda<=290)//一端有约束
			Klamda=0.762*lamda+28.6;
		else
			Klamda=lamda;
	}
	else //if(!bIncActualCompForce||pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//普通受压材0.0001为人为设定零因子
	{
		if(lamda<120)
		{
			if(start_force_type==CENTRIC_FORCE&&end_force_type==CENTRIC_FORCE)
				Klamda=lamda;			//两端中心受力
			//根据设计技术规定 DL/T 5154-2002,	交叉斜材一拉一压时，中间部分认为是中心受力
			else if(start_force_type!=end_force_type)
				Klamda=0.75*lamda+30;	//一端中心一端偏心受压
			else// if(pElem->start_force_type==ECCENTRIC_FORCE&&pElem->end_force_type==ECCENTRIC_FORCE)
				Klamda=0.5*lamda+60;	//两端偏心受压
		}
		else //if(lamda>=120)
		{
			if(CPreProcessor::ActiveDesignCode->CodeSerial()==IMemberDesignCode::CODE_DLT_2002()->CodeSerial())	//中国规范DL/T
			{
				if(lamda<=231&&start_joint_type!=end_joint_type)	//一端有约束
					Klamda=0.90*lamda+11.89;
				else if(lamda<242&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
					Klamda=0.82*lamda+21.64;
				else// if(lamda<
					Klamda=lamda;
			}
			else //if(iDesignCode==1)	//美国ASCE规范
			{
				if(lamda<=225&&start_joint_type!=end_joint_type)	//一端有约束
					Klamda=0.762*lamda+28.6;
				else if(lamda<250&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
					Klamda=0.615*lamda+46.2;
				else// if(lamda<
					Klamda=lamda;
			}
		}
	}
	return Klamda;
}
//lamdaType=0:ladmaRyo;1:lamdaRx;2:lamdaRc
double CalKlamda(CElement *pElem,double lamda,CLDSNode* pLoStart,CLDSNode* pLoEnd,bool bIncActualCompForce=true)
{
	double Klamda=lamda;
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
		return lamda;
	int start_joint_type= pElem->start_joint_type;
	int end_joint_type  = pElem->end_joint_type;
	if(pElem->start_joint_type!=JOINT_RIGID&&pElem->pStart==pElem->pOrgPole->pStart)
	{
		if(pElem->pOrgPole->connectStart.wnConnBoltN<2)
			pElem->m_bValidStartRestStiff=FALSE;
	}
	if(pElem->end_joint_type!=JOINT_RIGID&&pElem->pEnd==pElem->pOrgPole->pEnd)
	{
		if(pElem->pOrgPole->connectEnd.wnConnBoltN<2)
			pElem->m_bValidEndRestStiff=FALSE;
	}
	if(!pElem->m_bValidStartRestStiff)
		start_joint_type=JOINT_HINGE;
	else
		start_joint_type=JOINT_RIGID;
	if(!pElem->m_bValidEndRestStiff)
		end_joint_type=JOINT_HINGE;
	else
		end_joint_type=JOINT_RIGID;
	//交叉斜材一拉一压时，中间交叉点只有一个螺栓，认为无转动约束，而交叉点一般又靠近始端
	int start_force_type=pElem->start_force_type;
	int end_force_type=pElem->end_force_type;
	if(pLoStart==pElem->pStart&&pLoEnd!=pElem->pEnd&&pLoEnd!=NULL)
	{	//计算长度为始端点与交叉点
		if(pLoEnd->m_cPosCalType==4)
			end_joint_type=JOINT_HINGE;
		else
			end_joint_type=JOINT_RIGID;
		end_force_type=CENTRIC_FORCE;
	}
	else if(pLoStart!=pElem->pStart&&pLoEnd==pElem->pEnd&&pLoStart!=NULL)
	{	//计算长度为交叉点与终端点
		if(pLoStart->m_cPosCalType==4)
			start_joint_type=JOINT_HINGE;
		else
			start_joint_type=JOINT_RIGID;
		start_force_type=CENTRIC_FORCE;
	}
	if(start_joint_type==JOINT_RIGID)
		pElem->design_info.bStartRotRest=TRUE;
	else
		pElem->design_info.bStartRotRest=FALSE;
	if(end_joint_type==JOINT_RIGID)
		pElem->design_info.bEndRotRest=TRUE;
	else
		pElem->design_info.bEndRotRest=FALSE;
	if(pElem->pOrgPole->IsAuxPole())	//辅助材（不受力材）
		Klamda=CPreProcessor::ActiveDesignCode->CalRodKLamda(lamda,'B',start_joint_type,end_joint_type,start_force_type,end_force_type);
		//Klamda=CalKlamdaCore(lamda,false,start_joint_type,end_joint_type,start_force_type,end_force_type);
	else if(!bIncActualCompForce||pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//普通受压材0.0001为人为设定零因子
		Klamda=CPreProcessor::ActiveDesignCode->CalRodKLamda(lamda,'P',start_joint_type,end_joint_type,start_force_type,end_force_type);
		//Klamda=CalKlamdaCore(lamda,true,start_joint_type,end_joint_type,start_force_type,end_force_type);
	return Klamda;
}
//</DEVELOP_PROCESS_MARK>
#ifdef __ALFA_TEST_VERSION_
//<DEVELOP_PROCESS_MARK_TEST nameId="Global.CalKlamda">
#include "LogFile.h"
void Test_CalKlamdaCore()
{
	logerr.ClearContents();
	int joint_type[2],force_type[2];
	for(int lamda=1;lamda<400;lamda++)
	{
		for(BYTE flag=0;flag<16;flag++)
		{
			if(flag&0x08)
				joint_type[0]=1;
			else
				joint_type[0]=0;
			if(flag&0x04)
				joint_type[1]=1;
			else
				joint_type[1]=0;
			if(flag&0x02)
				force_type[0]=1;
			else
				force_type[0]=0;
			if(flag&0x01)
				force_type[1]=1;
			else
				force_type[1]=0;
			double klamda1=CalKlamdaCore(lamda,false,joint_type[0],joint_type[1],force_type[0],force_type[1]);
			double klamda2=CPreProcessor::ActiveDesignCode->CalRodKLamda(lamda,'B',joint_type[0],joint_type[1],force_type[0],force_type[1]);
			if(fabs(klamda1-klamda2)>EPS)
				logerr.Log("lamda=%d @B%d-%d-%d-%d error=%f\n",lamda,joint_type[0],joint_type[1],force_type[0],force_type[1],fabs(klamda1-klamda2));
			klamda1=CalKlamdaCore(lamda,true,joint_type[0],joint_type[1],force_type[0],force_type[1]);
			klamda2=CPreProcessor::ActiveDesignCode->CalRodKLamda(lamda,'P',joint_type[0],joint_type[1],force_type[0],force_type[1]);
			if(fabs(klamda1-klamda2)>EPS)
				logerr.Log("lamda=%d @P%d-%d-%d-%d error=%f\n",lamda,joint_type[0],joint_type[1],force_type[0],force_type[1],fabs(klamda1-klamda2));
		}
	}
	if(logerr.IsHasContents())
		logerr.ShowToScreen();
}
//</DEVELOP_PROCESS_MARK>
#endif
/*
static double CalKlamda(CElement *pElem,double lamda,BYTE lamdaType,bool bTensAndCompState=false)
{
	double Klamda=lamda;
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
		return lamda;
	//以下十四行用于考虑是否刚度上满足结构上的刚性约束
	int start_joint_type= pElem->start_joint_type;
	int end_joint_type  = pElem->end_joint_type;
	if(pElem->start_joint_type!=JOINT_RIGID&&pElem->pStart==pElem->pOrgPole->pStart)
	{
		if(pElem->pOrgPole->connectStart.N<2)
			pElem->m_bValidStartRestStiff=FALSE;
	}
	if(pElem->end_joint_type!=JOINT_RIGID&&pElem->pEnd==pElem->pOrgPole->pEnd)
	{
		if(pElem->pOrgPole->connectEnd.N<2)
			pElem->m_bValidEndRestStiff=FALSE;
	}
	if(!pElem->m_bValidStartRestStiff)
		start_joint_type=JOINT_HINGE;
	else
		start_joint_type=JOINT_RIGID;
	if(!pElem->m_bValidEndRestStiff)
		end_joint_type=JOINT_HINGE;
	else
		end_joint_type=JOINT_RIGID;
	//交叉斜材一拉一压时，中间交叉点只有一个螺栓，认为无转动约束，而交叉点一般又靠近始端
	int start_force_type=pElem->start_force_type;
	int end_force_type=pElem->end_force_type;
	if(lamdaType==0)
	{
		if(pElem->lamdaRyo.pStart==pElem->pStart&&pElem->lamdaRyo.pEnd!=pElem->pEnd)
		{	//计算长度为始端点与交叉点
			if(pElem->lamdaRyo.pEnd->m_cPosCalType==4)
				end_joint_type=JOINT_HINGE;
			else
				end_joint_type=JOINT_RIGID;
			end_force_type=CENTRIC_FORCE;
		}
		else if(pElem->lamdaRyo.pStart!=pElem->pStart&&pElem->lamdaRyo.pEnd==pElem->pEnd)
		{	//计算长度为交叉点与终端点
			if(pElem->lamdaRyo.pStart->m_cPosCalType==4)
				start_joint_type=JOINT_HINGE;
			else
				start_joint_type=JOINT_RIGID;
			start_force_type=CENTRIC_FORCE;
		}
	}
	else if(lamdaType==1)
	{
		if(pElem->lamdaRx.pStart==pElem->pStart&&pElem->lamdaRx.pEnd!=pElem->pEnd)
		{	//计算长度为始端点与交叉点
			if(pElem->lamdaRx.pEnd->m_cPosCalType==4)
				end_joint_type=JOINT_HINGE;
			else
				end_joint_type=JOINT_RIGID;
			end_force_type=CENTRIC_FORCE;
		}
		else if(pElem->lamdaRx.pStart!=pElem->pStart&&pElem->lamdaRx.pEnd==pElem->pEnd)
		{	//计算长度为交叉点与终端点
			if(pElem->lamdaRx.pStart->m_cPosCalType==4)
				start_joint_type=JOINT_HINGE;
			else
				start_joint_type=JOINT_RIGID;
			start_force_type=CENTRIC_FORCE;
		}
	}
	else if(lamdaType==2)
	{
		if(pElem->lamdaRc.pStart==pElem->pStart&&pElem->lamdaRc.pEnd!=pElem->pEnd)
		{	//计算长度为始端点与交叉点
			if(pElem->lamdaRc.pEnd->m_cPosCalType==4)
				end_joint_type=JOINT_HINGE;
			else
				end_joint_type=JOINT_RIGID;
			end_force_type=CENTRIC_FORCE;
		}
		else if(pElem->lamdaRc.pStart!=pElem->pStart&&pElem->lamdaRc.pEnd==pElem->pEnd)
		{	//计算长度为交叉点与终端点
			if(pElem->lamdaRc.pStart->m_cPosCalType==4)
				start_joint_type=JOINT_HINGE;
			else
				start_joint_type=JOINT_RIGID;
			start_force_type=CENTRIC_FORCE;
		}
	}

	if(start_joint_type==JOINT_RIGID)
		pElem->design_info.bStartRotRest=TRUE;
	else
		pElem->design_info.bStartRotRest=FALSE;
	if(end_joint_type==JOINT_RIGID)
		pElem->design_info.bEndRotRest=TRUE;
	else
		pElem->design_info.bEndRotRest=FALSE;
	if(pElem->pOrgPole->IsAuxPole())	//辅助材（不受力材）
	{
		if(lamda<120)
			Klamda=lamda;				//两端偏心受压
		else if(lamda<=250&&start_joint_type==JOINT_HINGE&&end_joint_type==JOINT_HINGE)
			Klamda=lamda;				//两端无约束
		else if(lamda<=330&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
			Klamda=0.615*lamda+46.2;	//两端有约束
		else if(lamda<=290)//一端有约束
			Klamda=0.762*lamda+28.6;
		else
			Klamda=lamda;
	}
	else if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//普通受压材0.0001为人为设定零因子
	{
		if(lamda<120)
		{
			if(start_force_type==CENTRIC_FORCE&&end_force_type==CENTRIC_FORCE)
				Klamda=lamda;			//两端中心受力
			//根据设计技术规定 DL/T 5154-2002,	交叉斜材一拉一压时，中间部分认为是中心受力
			else if(start_force_type!=end_force_type)
				Klamda=0.75*lamda+30;	//一端中心一端偏心受压
			else// if(pElem->start_force_type==ECCENTRIC_FORCE&&pElem->end_force_type==ECCENTRIC_FORCE)
				Klamda=0.5*lamda+60;	//两端偏心受压
		}
		else //if(lamda>=120)
		{
			if(Ta.m_iMemberSizeCheckCode==0)	//中国规范DL/T
			{
				if(lamda<=231&&start_joint_type!=end_joint_type)	//一端有约束
					Klamda=0.90*lamda+11.89;
				else if(lamda<242&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
					Klamda=0.82*lamda+21.64;
				else// if(lamda<
					Klamda=lamda;
			}
			else //if(iDesignCode==1)	//美国ASCE规范
			{
				if(lamda<=225&&start_joint_type!=end_joint_type)	//一端有约束
					Klamda=0.762*lamda+28.6;
				else if(lamda<250&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
					Klamda=0.615*lamda+46.2;
				else// if(lamda<
					Klamda=lamda;
			}
		}
	}
	return Klamda;
}
*/
//from database.cpp
//中国规范
double CalStableCoef1(double lamda,char sectionType,char cMaterial);
//美国导则
double CalStableCoef2(double lamda,double E,double fy);
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::CalElemCompStableInfo">
void CPreProcessor::CalElemCompStableInfo(CElement *pElem,IMemberDesignCode* pDesignCode,bool bIncActualCompForce/*=true*/)
{	//计算当前单元的长细比
	double lamda=0;		//  λ：长细比
	double Klamda=0;	//K*λ：修正后的长细比
	double raiseCoef=1.0;
	double Rx,Ry0,Ix,t,area;	//单位均为mm级
	char sError[100]="";
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pParaType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pParaType==NULL)
			pElem->pParaType=pParaType=FindTubeType(pElem->GetWidth(),pElem->GetThick());
		if(pParaType==NULL)
		{
			sprintf(sError,"规格库中找不到0X%X杆件的规格φ%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
			throw sError;
		}
		Rx=pParaType->r*10;
		Ry0=Rx;
		Ix=pParaType->I*10000;
		t=pParaType->thick;
		area=pParaType->area*100;
	}
	else
	{
		JG_PARA_TYPE *pParaType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pParaType==NULL)
			pElem->pParaType=pParaType=FindJgType(pElem->GetWidth(),pElem->GetThick(),pElem->pOrgPole->size_cSubClassType);
		if(pParaType==NULL)
		{
			if(pElem->pOrgPole->size_cSubClassType=='L')
				sprintf(sError,"规格库中找不到0X%X杆件的规格L%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
			else
				sprintf(sError,"规格库中找不到0X%X杆件的规格%CL%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_cSubClassType,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
			g_pSolidDraw->SetEntSnapStatus(pElem->pOrgPole->handle);
			throw sError;
		}
		Rx=pParaType->Rx*10;
		Ry0=pParaType->Ry0*10;
		Ix=pParaType->Ix*10000;
		t=pParaType->wing_thick;
		area=pParaType->area*100;
	}
	if(pElem->pOrgPole->m_bLegVRod)//layer(0)=='L'&&pElem->pOrgPole->layer(1)=='X')
		raiseCoef=1.2;
	char sectionType='B';
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE&&pElem->pOrgPole->size_wide<g_sysPara.fTubeWeldRoadMinD)
		sectionType='A';
	STEELMAT *pMatType=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
	if(pMatType==NULL)
		throw "材质错误!";
	if(pElem->CalLenType()==0)	//pOrgPole->CalLenCoef.iTypeNo==0
	{	//自动智能计算含（主材最小轴或无辅材交叉斜材、主材平行轴或有辅材交叉斜材、非主材最小轴、非主材平行轴）
		if(pElem->lamdaRyo.pStart)
		{
			pElem->lamdaRyo.R=Ry0;
			pElem->lamdaRyo.L=raiseCoef*DISTANCE(pElem->lamdaRyo.pStart->Position(false),pElem->lamdaRyo.pEnd->Position(false));
			lamda=pElem->lamdaRyo.L/Ry0;
			if(pDesignCode->IsIncLamdaK())//iDesignCode!=1)
				pElem->lamdaRyo.Lo_r=CalKlamda(pElem,lamda,pElem->lamdaRyo.pStart,pElem->lamdaRyo.pEnd,bIncActualCompForce);//0,pElem->pOrgPole->CalLenCoef.hRefPole>0x20);
			else
				pElem->lamdaRyo.Lo_r=lamda;
			if((pElem->lamdaRyo.pStart==pElem->pStart&&pElem->lamdaRyo.pEnd==pElem->pEnd)||
			   (pElem->lamdaRyo.pStart==pElem->pEnd&&pElem->lamdaRyo.pEnd==pElem->pStart))
			   pElem->lamdaRyo.braceStr.Copy(" ELEM_LEN");
			else if(pElem->lamdaRyo.pStart->point_i<pElem->lamdaRyo.pEnd->point_i)
				pElem->lamdaRyo.braceStr.Printf("%4d~%4d",pElem->lamdaRyo.pStart->point_i,pElem->lamdaRyo.pEnd->point_i);
			else
				pElem->lamdaRyo.braceStr.Printf("%4d~%4d",pElem->lamdaRyo.pEnd->point_i,pElem->lamdaRyo.pStart->point_i);
		}
		if(pElem->lamdaRx.pStart)
		{
			pElem->lamdaRx.R=Rx;
			pElem->lamdaRx.L=pElem->lamdaRx.muCoefBasic*raiseCoef*
				DISTANCE(pElem->lamdaRx.pStart->Position(false),pElem->lamdaRx.pEnd->Position(false));
			lamda=pElem->lamdaRx.L/Rx;
			if(pDesignCode->IsIncLamdaK())//iDesignCode!=1)
				pElem->lamdaRx.Lo_r=CalKlamda(pElem,lamda,pElem->lamdaRx.pStart,pElem->lamdaRx.pEnd,bIncActualCompForce);//1,pElem->pOrgPole->CalLenCoef.hRefPole>0x20);
			else
				pElem->lamdaRx.Lo_r=lamda;
			if((pElem->lamdaRx.pStart==pElem->pStart&&pElem->lamdaRx.pEnd==pElem->pEnd)||
			   (pElem->lamdaRx.pStart==pElem->pEnd&&pElem->lamdaRx.pEnd==pElem->pStart))
				pElem->lamdaRx.braceStr.Copy(" ELEM_LEN");
			else if(pElem->lamdaRx.pStart->point_i<pElem->lamdaRx.pEnd->point_i)
				pElem->lamdaRx.braceStr.Printf("%4d~%4d",pElem->lamdaRx.pStart->point_i,pElem->lamdaRx.pEnd->point_i);
			else
				pElem->lamdaRx.braceStr.Printf("%4d~%4d",pElem->lamdaRx.pEnd->point_i,pElem->lamdaRx.pStart->point_i);
		}
		if(pElem->pOrgPole->CalLenCoef.hRefPole<=0x20)
		{	//非交叉材
			lamda=0;
			if(pElem->lamdaRyo.Lo_r>lamda)
			{
				lamda=pElem->lamdaRyo.Lo_r;
				pElem->MaxCompStable.lamdatype_tenscomp=0;
			}
			if(pElem->lamdaRx.Lo_r>lamda)
			{
				lamda=pElem->lamdaRx.Lo_r;
				pElem->MaxCompStable.lamdatype_tenscomp=1;
			}
			/*
			同时受压时一般是压稳设计应力控制，此时应在CAnalyzeProcessor::UpdateInfoAtStatusChange中处理 wjh-2013.5.29
			if(pElem->lamdaRc.Lo_r>lamda)
			{
				lamda=pElem->lamdaRc.Lo_r;
				pElem->MaxCompStable.lamdatype_tenscomp=2;
			}
			*/
			pElem->MaxCompStable.fai_tenscomp=pDesignCode->CompStableCoef(sectionType,lamda,pMatType,t);
			/*if(iDesignCode!=2)	//	国内规范
				pElem->MaxCompStable.fai_tenscomp=CalStableCoef1(lamda,sectionType,pElem->pOrgPole->cMaterial);
			else	//美国ASCE 10-97导则
				pElem->MaxCompStable.fai_tenscomp=CalStableCoef2(lamda,pMatType->E,pMatType->fy(t));*/
			pElem->MaxCompStable.Afai_tenscomp=pElem->MaxCompStable.fai_tenscomp*area;
		}
		else
		{
			//计算一拉一压
			if(pElem->lamdaRyo.Lo_r>pElem->lamdaRx.Lo_r)
			{
				lamda=pElem->lamdaRyo.Lo_r;
				pElem->MaxCompStable.lamdatype_tenscomp=0;
			}
			else
			{
				lamda=pElem->lamdaRx.Lo_r;
				pElem->MaxCompStable.lamdatype_tenscomp=1;
			}
			pElem->MaxCompStable.fai_tenscomp=pDesignCode->CompStableCoef(sectionType,lamda,pMatType,t);
			/*if(iDesignCode!=2)	//	国内规范
				pElem->MaxCompStable.fai_tenscomp=CalStableCoef1(lamda,sectionType,pElem->pOrgPole->cMaterial);
			else	//美国ASCE 10-97导则
				pElem->MaxCompStable.fai_tenscomp=CalStableCoef2(lamda,pMatType->E,pMatType->fy(t));*/
			pElem->MaxCompStable.Afai_tenscomp=pElem->MaxCompStable.fai_tenscomp*area;
			/*计算同时受压
			同时受压时一般是压稳设计应力控制，此时应在CAnalyzeProcessor::UpdateInfoAtStatusChange中处理 wjh-2013.5.29
			lamda=pElem->lamdaRc.Lo_r;
			pElem->MaxCompStable.lamdatype_bothcomp=2;
			if(iDesignCode!=2)	//	国内规范
				pElem->MaxCompStable.fai_bothcomp=CalStableCoef1(lamda,sectionType,pElem->pOrgPole->cMaterial);
			else	//美国ASCE 10-97导则
				pElem->MaxCompStable.fai_bothcomp=CalStableCoef2(lamda,pMatType->E,pMatType->fy(t));
			pElem->MaxCompStable.Afai_bothcomp=pElem->MaxCompStable.fai_bothcomp*area;
			*/
		}
		if(pElem->MaxCompStable.bothComp)
		{
			lamda=pElem->MaxCompStable.Lo_r_bothcomp;//pElem->MaxCompStable.L_bothcomp/Rx;
			if(pDesignCode->IsIncLamdaK())//iDesignCode!=1)
				pElem->MaxCompStable.Lo_r_bothcomp=CalKlamda(pElem,lamda,pElem->pStart,pElem->pEnd,bIncActualCompForce);//0,pElem->pOrgPole->CalLenCoef.hRefPole>0x20);
			else
				pElem->MaxCompStable.Lo_r_bothcomp=lamda;
			//极端情况会出现交点到一端距离过长导致最小轴长细比反而比平行轴更长，同时受压时更危险，故应取长细比最大者 wjh-2017.2.4
			//if(pElem->lamdaRyo.Lo_r>pElem->MaxCompStable.Lo_r_bothcomp)
			//	pElem->MaxCompStable.Lo_r_bothcomp=pElem->lamdaRyo.Lo_r;
		}
	}
	else if(pElem->CalLenType()==1)//pOrgPole->CalLenCoef.iTypeNo==1)
	{	//V斜材（考虑同时受压，对应规定中的序号6）
		/*同端点邻杆同时受压
		同时受压时一般是压稳设计应力控制，此时应在CAnalyzeProcessor::UpdateInfoAtStatusChange中处理 wjh-2013.5.29
		pElem->lamdaRc.coef=0.65;
		pElem->lamdaRc.L=pElem->lamdaRc.coef*raiseCoef*pElem->Length();
		pElem->lamdaRc.R=Rx;
		pElem->lamdaRc.Lo_r=CalKlamda(pElem,pElem->lamdaRc.L/Rx,pElem->lamdaRc.pStart,pElem->lamdaRc.pEnd);//2,false);
		pElem->MaxCompStable.lamdatype_bothcomp=2;
		pElem->MaxCompStable.fai_bothcomp=CalStableCoef1(pElem->lamdaRc.Lo_r,sectionType,pElem->pOrgPole->cMaterial);
		pElem->MaxCompStable.Afai_bothcomp=area*pElem->MaxCompStable.fai_tenscomp;
		pElem->lamdaRc.braceStr.Copy(" ELEM_LEN");
		*/
		//同端点邻杆一拉一压
		if(pElem->lamdaRyo.pStart&&pElem->lamdaRyo.pEnd)
			pElem->lamdaRyo.L=DISTANCE(pElem->lamdaRyo.pStart->Position(false),pElem->lamdaRyo.pEnd->Position(false));
		else if(pElem->lamdaRyo.bCalByElemLen)
		{
			if(pElem->Length()<EPS2)
				pElem->CalLength();
			pElem->lamdaRyo.L=pElem->Length();
			if(pElem->lamdaRyo.L<EPS2)
			{	//过往存在过数据不合理情况，故遇此情况输出警示信息 wjh-2017.3.10
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,
					"%d-%d单元最小计算长度过短(L(%d-%d)=%.3fmm)，请核实",pElem->pStart->pointI,pElem->pEnd->pointI,
					pElem->lamdaRyo.pStart->pointI,pElem->lamdaRyo.pEnd->pointI,pElem->lamdaRyo.L);
			}
		}
		pElem->lamdaRyo.L*=pElem->lamdaRyo.muCoefBasic*raiseCoef;
		pElem->lamdaRyo.R=Ry0;
		pElem->lamdaRyo.Lo_r=CalKlamda(pElem,pElem->lamdaRyo.L/Ry0,pElem->lamdaRyo.pStart,pElem->lamdaRyo.pEnd,bIncActualCompForce);//0);
		pElem->MaxCompStable.lamdatype_tenscomp=0;
		if(pElem->lamdaRyo.pStart)
			pElem->lamdaRyo.braceStr.Printf("%4d~%4d",pElem->lamdaRyo.pStart->point_i,pElem->lamdaRyo.pEnd->point_i);
		else if(pElem->lamdaRyo.bCalByElemLen)
			pElem->lamdaRyo.braceStr.Printf(" ELEM_LEN");
		else
			pElem->lamdaRyo.braceStr.Printf("         ");
		pElem->MaxCompStable.fai_tenscomp=CalStableCoef1(pElem->lamdaRyo.Lo_r,sectionType,pElem->pOrgPole->cMaterial);
		pElem->MaxCompStable.Afai_tenscomp=area*pElem->MaxCompStable.fai_tenscomp;
		//平行轴验算
		if(pElem->lamdaRx.pStart&&pElem->lamdaRx.pEnd)
		{
			pElem->lamdaRx.L=DISTANCE(pElem->lamdaRx.pStart->Position(false),pElem->lamdaRx.pEnd->Position(false));
			pElem->lamdaRx.L*=pElem->lamdaRx.muCoefBasic*raiseCoef;
			pElem->lamdaRx.R=Ry0;
			pElem->lamdaRx.Lo_r=CalKlamda(pElem,pElem->lamdaRx.L/Rx,pElem->lamdaRx.pStart,pElem->lamdaRx.pEnd,bIncActualCompForce);//0);
			pElem->MaxCompStable.lamdatype_tenscomp=0;
			pElem->lamdaRx.braceStr.Printf("%4d~%4d",pElem->lamdaRx.pStart->point_i,pElem->lamdaRx.pEnd->point_i);
			double fai_tenscomp=CalStableCoef1(pElem->lamdaRx.Lo_r,sectionType,pElem->pOrgPole->cMaterial);
			double Afai_tenscomp=area*pElem->MaxCompStable.fai_tenscomp;
			pElem->MaxCompStable.fai_tenscomp=min(fai_tenscomp,CalStableCoef1(pElem->lamdaRx.Lo_r,sectionType,pElem->pOrgPole->cMaterial));
			pElem->MaxCompStable.Afai_tenscomp=min(Afai_tenscomp,area*pElem->MaxCompStable.fai_tenscomp);
		}
	}
	else if(pElem->CalLenType()==2)//pOrgPole->CalLenCoef.iTypeNo==2)
	{	//V斜材（V面连接）（不考虑同时受压，对应规定中的序号7）
		if(pElem->lamdaRyo.pStart)
			pElem->lamdaRyo.L=DISTANCE(pElem->lamdaRyo.pStart->Position(false),pElem->lamdaRyo.pEnd->Position(false));
		else if(pElem->lamdaRyo.bCalByElemLen)
			pElem->lamdaRyo.L=pElem->Length();
		else
			pElem->lamdaRyo.L=0;
		pElem->lamdaRyo.L*=pElem->lamdaRyo.muCoefBasic*raiseCoef;
		pElem->lamdaRyo.R=Ry0;
		pElem->lamdaRyo.Lo_r=CalKlamda(pElem,pElem->lamdaRyo.L/Ry0,pElem->lamdaRyo.pStart,pElem->lamdaRyo.pEnd,bIncActualCompForce);//0);
		pElem->MaxCompStable.lamdatype_tenscomp=0;
		if(pElem->lamdaRyo.pStart)
			pElem->lamdaRyo.braceStr.Printf("%4d~%4d",pElem->lamdaRyo.pStart->point_i,pElem->lamdaRyo.pEnd->point_i);
		else
			pElem->lamdaRyo.braceStr.Printf(" ELEM_LEN");
		pElem->MaxCompStable.fai_tenscomp=CalStableCoef1(pElem->lamdaRyo.Lo_r,sectionType,pElem->pOrgPole->cMaterial);
		pElem->MaxCompStable.Afai_tenscomp=area*pElem->MaxCompStable.fai_tenscomp;
		//平行轴验算
		if(pElem->lamdaRx.pStart&&pElem->lamdaRx.pEnd)
		{
			pElem->lamdaRx.L=DISTANCE(pElem->lamdaRx.pStart->Position(false),pElem->lamdaRx.pEnd->Position(false));
			pElem->lamdaRx.L*=pElem->lamdaRx.muCoefBasic*raiseCoef;
			pElem->lamdaRx.R=Ry0;
			pElem->lamdaRx.Lo_r=CalKlamda(pElem,pElem->lamdaRx.L/Rx,pElem->lamdaRx.pStart,pElem->lamdaRx.pEnd,bIncActualCompForce);//0);
			pElem->MaxCompStable.lamdatype_tenscomp=0;
			pElem->lamdaRx.braceStr.Printf("%4d~%4d",pElem->lamdaRx.pStart->point_i,pElem->lamdaRx.pEnd->point_i);
			double fai_tenscomp=CalStableCoef1(pElem->lamdaRx.Lo_r,sectionType,pElem->pOrgPole->cMaterial);
			double Afai_tenscomp=area*pElem->MaxCompStable.fai_tenscomp;
			pElem->MaxCompStable.fai_tenscomp=min(fai_tenscomp,CalStableCoef1(pElem->lamdaRx.Lo_r,sectionType,pElem->pOrgPole->cMaterial));
			pElem->MaxCompStable.Afai_tenscomp=min(Afai_tenscomp,area*pElem->MaxCompStable.fai_tenscomp);
		}
	}
	else if(pElem->CalLenType()==3||pElem->CalLenType()==4)//pOrgPole->CalLenCoef.iTypeNo==3||pElem->pOrgPole->CalLenCoef.iTypeNo==4)
	{	//V斜材3或4节间带横连杆（对应规定中的序号8、9）
		CSuperSmartPtr<CLDSPart> pHorizPole=pElem->pOrgPole->BelongModel()->FromPartHandle(pElem->pOrgPole->CalLenCoef.hHorizPole);
		double lamda1=0,lamda2=0;
		//同端点邻杆同时受压
		double I4=0;
		if(pHorizPole.IsNULL())
			I4=0;
		else if(pHorizPole.LinePartPointer()->size_idClassType==CLS_LINEANGLE
			||pHorizPole.LinePartPointer()->size_idClassType==CLS_GROUPLINEANGLE)
		{
			JG_PARA_TYPE *pTmJgType=FindJgType(pHorizPole->GetWidth(),pHorizPole->GetThick(),pHorizPole.LinePartPointer()->size_cSubClassType);
			I4=pTmJgType->Ix;
		}
		else if(pHorizPole.LinePartPointer()->size_idClassType==CLS_LINETUBE)
		{
			TUBE_PARA_TYPE *pTmTubeType=FindTubeType(pHorizPole->GetWidth(),pHorizPole->GetThick());
			I4=pTmTubeType->I;
		}
		else
			I4=0;
		pElem->smcinfo.smcItems[1].cSMCType=pElem->smcinfo.smcItems[2].cSMCType=0;
		//此处给默认值仅是用于出错时显示的错误数据非任意值，导致后面代码可能出现的字符串溢出异常
		double alfa,L3=10000,L4=10000;
		if(I4>0)
		{
			L3=DISTANCE(pElem->pOrgPole->pStart->Position(false),pElem->pOrgPole->pEnd->Position(false));//GetLength();
			L4=DISTANCE(pHorizPole.LinePartPointer()->pStart->Position(false),pHorizPole.LinePartPointer()->pEnd->Position(false));//GetLength();
			alfa=I4*pow(L3,3)/(Ix*pow(L4,3));
		}
		else
			alfa=0;
		pElem->smcinfo.smcItems[0].cSMCType=SMC_ITEM::SMC_REFER_END;
		//默认计算长度区间为杆件整长
		pElem->smcinfo.smcItems[0].LoRc.hStartNode=pElem->pOrgPole->pStart->handle;
		pElem->smcinfo.smcItems[0].LoRc.hEndNode=pElem->pOrgPole->pEnd->handle;
		pElem->smcinfo.lamdaItems[0].pStart=pElem->pOrgPole->pStart;
		pElem->smcinfo.lamdaItems[0].pEnd=pElem->pOrgPole->pEnd;
		if(pElem->CalLenType()==3)//pOrgPole->CalLenCoef.iTypeNo==3)
		{	//3节间
			if(alfa<0.25)	//按无此横连杆计算
			{
				pElem->smcinfo.lamdaItems[0].L=0.65*L3;
				pElem->smcinfo.smcItems[0].LoRc.coef=0.65;
			}
			else
			{
				pElem->smcinfo.lamdaItems[0].L=0.55*L3;
				pElem->smcinfo.smcItems[0].LoRc.coef=0.55;
			}
			/*pElem->lamdaRc.R=Rx;
			pElem->lamdaRc.L*=pElem->lamdaRc.coef*raiseCoef;
			pElem->lamdaRc.Lo_r=CalKlamda(pElem,pElem->lamdaRc.L/Rx,pElem->lamdaRc.pStart,pElem->lamdaRc.pEnd);//2);
			pElem->lamdaRc.braceStr.Copy(" ELEM_LEN");*/
		}
		else
		{	//4节间
			if(alfa<0.4)	//按无此横连杆计算
			{
				//pElem->lamdaRc.L=0.65*L3;
				pElem->smcinfo.smcItems[0].LoRc.coef=0.65;
				//pElem->lamdaRc.braceStr.Copy(" ELEM_LEN");
			}
			else if(alfa>=0.4&&alfa<1.0)
			{
				//pElem->lamdaRc.L=0.55*L3;
				pElem->smcinfo.smcItems[0].LoRc.coef=0.55;
				//pElem->lamdaRc.braceStr.Copy(" ELEM_LEN");
			}
			else// if(alfa>=1.0)
			{
				f3dPoint inters;
				f3dLine line,mirline;
				line.startPt=pElem->pStart->Position(false);
				line.endPt=pElem->pEnd->Position(false);
				mirline.startPt=pHorizPole.LinePartPointer()->pStart->Position(false);
				mirline.endPt=pHorizPole.LinePartPointer()->pEnd->Position(false);
				pElem->smcinfo.smcItems[0].LoRc.coef=0.65;
				if(Int3dll(line,mirline,inters)>0)	//交于端点或内点
				{
					pElem->smcinfo.smcItems[0].LoRc.bCalByElemLen=false;
					double L2 =DISTANCE(inters,line.endPt);
					double L2i=DISTANCE(inters,line.startPt);
					if(L2<L2i)
					{
						pElem->smcinfo.lamdaItems[0].L=0.65*L2i;
						pElem->smcinfo.lamdaItems[0].braceStr.Printf("交点~%4d",pElem->pStart->point_i);
						pElem->smcinfo.smcItems[0].LoRc.hEndNode=0;
						pElem->smcinfo.lamdaItems[0].pEnd=NULL;
					}
					else
					{
						pElem->smcinfo.lamdaItems[0].L=0.65*L2;
						pElem->smcinfo.lamdaItems[0].braceStr.Printf("交点~%4d",pElem->pEnd->point_i);
						pElem->smcinfo.smcItems[0].LoRc.hStartNode=0;
						pElem->smcinfo.lamdaItems[0].pStart=NULL;
					}
					//交于端点或内点时计算长度区间交点信息不完整，后续计算无法正常进行，所以必须在此更新长细比信息
					pElem->smcinfo.lamdaItems[0].R=Rx;
					pElem->smcinfo.lamdaItems[0].L*=raiseCoef;
					pElem->smcinfo.lamdaItems[0].Lo_r=CalKlamda(pElem,pElem->smcinfo.lamdaItems[0].L/Rx,
						pElem->smcinfo.lamdaItems[0].pStart,pElem->smcinfo.lamdaItems[0].pEnd,bIncActualCompForce);//2);
				}
				/*else	//不合法参照杆时，按杆件全长为计算长度
				{
					pElem->lamdaRc.L=0.65*L3;
					pElem->lamdaRc.braceStr.Copy(" ELEM_LEN");
				}*/
			}
		}
		pElem->MaxCompStable.lamdatype_bothcomp=2;
		pElem->MaxCompStable.fai_bothcomp=CalStableCoef1(pElem->smcinfo.lamdaItems[0].Lo_r,sectionType,pElem->pOrgPole->cMaterial);
		pElem->MaxCompStable.Afai_bothcomp=area*pElem->MaxCompStable.fai_tenscomp;
		//同端节点邻杆一拉一压
		if(pElem->lamdaRyo.pStart)
			pElem->lamdaRyo.L=DISTANCE(pElem->lamdaRyo.pStart->Position(false),pElem->lamdaRyo.pEnd->Position(false));
		else if(pElem->lamdaRyo.bCalByElemLen)
			pElem->lamdaRyo.L=pElem->Length();
		else
			pElem->lamdaRyo.L=0;
		pElem->lamdaRyo.R=Ry0;
		pElem->lamdaRyo.L*=pElem->lamdaRyo.muCoefBasic*raiseCoef;
		pElem->lamdaRyo.Lo_r=CalKlamda(pElem,pElem->lamdaRyo.L/Ry0,pElem->lamdaRyo.pStart,pElem->lamdaRyo.pEnd,bIncActualCompForce);//0);
		pElem->MaxCompStable.lamdatype_tenscomp=0;
		pElem->MaxCompStable.fai_tenscomp=CalStableCoef1(pElem->lamdaRyo.Lo_r,sectionType,pElem->pOrgPole->cMaterial);
		pElem->MaxCompStable.Afai_tenscomp=area*pElem->MaxCompStable.fai_tenscomp;
	}
	else //if(pElem->CalLenType()==5||pElem->CalLenType()==6)
	{	//V斜材指定杆件计算长度
		raiseCoef=1.0;	//即然是指定杆件计算长度，放大系数就失去意义故赋值为1.0
		pElem->lamdaRyo.L=pElem->pOrgPole->CalSpecLamdaLenMinR();
		pElem->lamdaRyo.muCoefBasic=pElem->pOrgPole->CalLenCoef.minR.coef;
		pElem->lamdaRyo.R=Ry0;
		pElem->lamdaRyo.Lo_r=CalKlamda(pElem,pElem->lamdaRyo.L/Ry0,pElem->lamdaRyo.pStart,pElem->lamdaRyo.pEnd,bIncActualCompForce);//0);
		if((pElem->lamdaRyo.pStart==pElem->pStart&&pElem->lamdaRyo.pEnd==pElem->pEnd)||
			(pElem->lamdaRyo.pStart==pElem->pEnd&&pElem->lamdaRyo.pEnd==pElem->pStart))
			pElem->lamdaRyo.braceStr.Copy(" ELEM_LEN");
		else if(pElem->lamdaRyo.pStart&&pElem->lamdaRyo.pEnd)
			pElem->lamdaRyo.braceStr.Printf("%4d~%4d",pElem->lamdaRyo.pStart->point_i,pElem->lamdaRyo.pEnd->point_i);
		else if(pElem->lamdaRyo.muCoefBasic!=0)
			pElem->lamdaRyo.braceStr.Copy(" ELEM_LEN");
		//else
		//	logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d-%d单元指定杆件最小轴计算长度区间信息不完整!",pElem->pStart->pointI,pElem->pEnd->pointI);

		pElem->lamdaRx.L=pElem->pOrgPole->CalSpecLamdaLenParaR();
		pElem->lamdaRx.muCoefBasic=pElem->pOrgPole->CalLenCoef.paraR.coef;
		pElem->lamdaRx.R=Rx;
		pElem->lamdaRx.Lo_r=CalKlamda(pElem,pElem->lamdaRx.L/Rx,pElem->lamdaRx.pStart,pElem->lamdaRx.pEnd,bIncActualCompForce);//1);
		if((pElem->lamdaRx.pStart==pElem->pStart&&pElem->lamdaRx.pEnd==pElem->pEnd)||
			(pElem->lamdaRx.pStart==pElem->pEnd&&pElem->lamdaRx.pEnd==pElem->pStart))
			pElem->lamdaRx.braceStr.Copy(" ELEM_LEN");
		else if(pElem->lamdaRx.pStart&&pElem->lamdaRx.pEnd)
			pElem->lamdaRx.braceStr.Printf("%4d~%4d",pElem->lamdaRx.pStart->point_i,pElem->lamdaRx.pEnd->point_i);
		else if(pElem->lamdaRx.muCoefBasic!=0)
			pElem->lamdaRx.braceStr.Copy(" ELEM_LEN");

		double lamda=max(pElem->lamdaRyo.Lo_r,pElem->lamdaRx.Lo_r);
		if(pElem->lamdaRyo.Lo_r>pElem->lamdaRx.Lo_r)
			pElem->MaxCompStable.lamdatype_tenscomp=0;
		else
			pElem->MaxCompStable.lamdatype_tenscomp=1;
		pElem->MaxCompStable.fai_tenscomp=CalStableCoef1(lamda,sectionType,pElem->pOrgPole->cMaterial);
		pElem->MaxCompStable.Afai_tenscomp=area*pElem->MaxCompStable.fai_tenscomp;
	}
	if(bIncActualCompForce&&pElem->MaxCompression.N>CElement::COMPRESSION_ZERO_FORCE)
		return;	//拉杆不进行同时受压检测计算
	for(int i=0;i<3;i++)
	{
		if(pElem->smcinfo.smcItems[i].cSMCType==SMC_ITEM::SMC_NONE)
			continue;
		if(pElem->smcinfo.smcItems[i].cSMCType>0)	//平行轴
			pElem->smcinfo.lamdaItems[i].R=Rx;
		else
			pElem->smcinfo.lamdaItems[i].R=Ry0;
		if(pElem->smcinfo.smcItems[i].LoRc.bCalByElemLen||(pElem->smcinfo.smcItems[i].LoRc.coef>0&&pElem->smcinfo.smcItems[i].LoRc.hStartNode==0&&pElem->smcinfo.smcItems[i].LoRc.hEndNode==0))
		{
			pElem->smcinfo.lamdaItems[i].pStart=pElem->pStart;
			pElem->smcinfo.lamdaItems[i].pEnd=pElem->pEnd;
			pElem->smcinfo.lamdaItems[i].L=pElem->Length();
		}
		else if(pElem->smcinfo.smcItems[i].LoRc.coef>0&&pElem->smcinfo.smcItems[i].LoRc.hStartNode>0&&pElem->smcinfo.smcItems[i].LoRc.hEndNode>0)
		{
			pElem->smcinfo.lamdaItems[i].pStart=Ta.Node.FromHandle(pElem->smcinfo.smcItems[i].LoRc.hStartNode);
			pElem->smcinfo.lamdaItems[i].pEnd=Ta.Node.FromHandle(pElem->smcinfo.smcItems[i].LoRc.hEndNode);
			pElem->smcinfo.lamdaItems[i].L=DISTANCE(pElem->smcinfo.lamdaItems[i].pStart->Position(false),pElem->smcinfo.lamdaItems[i].pEnd->Position(false));
		}
		else
			continue;
		pElem->smcinfo.lamdaItems[i].muCoefBasic=raiseCoef*pElem->smcinfo.smcItems[i].LoRc.coef;
		pElem->smcinfo.lamdaItems[i].L*=pElem->smcinfo.lamdaItems[i].muCoefBasic;
		double Lo_r=pElem->smcinfo.lamdaItems[i].L/pElem->smcinfo.lamdaItems[i].R;
		pElem->smcinfo.lamdaItems[i].Lo_r=CalKlamda(pElem,Lo_r,pElem->smcinfo.lamdaItems[i].pStart,pElem->smcinfo.lamdaItems[i].pEnd,bIncActualCompForce);//0);
		//*极端情况会出现交点到一端距离过长导致最小轴长细比反而比平行轴更长，同时受压时更危险，故应取长细比最大者 wjh-2017.2.4
		//if(pElem->smcinfo.lamdaItems[i].Lo_r<pElem->lamdaRyo.Lo_r)
		//	pElem->smcinfo.lamdaItems[i]=pElem->lamdaRyo;
		pElem->smcinfo.lamdaItems[i].coefFai=pDesignCode->CompStableCoef(sectionType,pElem->smcinfo.lamdaItems[i].Lo_r,pMatType,pElem->GetThick());
		/*if(iDesignCode!=2)
			pElem->smcinfo.lamdaItems[i].coefFai=CalStableCoef1(pElem->smcinfo.lamdaItems[i].Lo_r,sectionType,pElem->pOrgPole->cMaterial);
		else
			pElem->smcinfo.lamdaItems[i].coefFai=CalStableCoef2(pElem->smcinfo.lamdaItems[i].Lo_r,pMatType->E,pMatType->fy(pElem->GetThick()));*/
		pElem->smcinfo.lamdaItems[i].coefAreaFai=area*pElem->smcinfo.lamdaItems[i].coefFai;
		if((pElem->smcinfo.lamdaItems[i].pStart==pElem->pStart&&pElem->smcinfo.lamdaItems[i].pEnd==pElem->pEnd)||
			(pElem->smcinfo.lamdaItems[i].pStart==pElem->pEnd&&pElem->smcinfo.lamdaItems[i].pEnd==pElem->pStart))
			pElem->smcinfo.lamdaItems[i].braceStr.Copy(" ELEM_LEN");
		else if(pElem->smcinfo.lamdaItems[i].pStart&&pElem->smcinfo.lamdaItems[i].pEnd)
		{
			double L=DISTANCE(pElem->smcinfo.lamdaItems[i].pStart->Position(false),pElem->smcinfo.lamdaItems[i].pEnd->Position(false));
			double coef=L/pElem->Length();
			CXhChar16 expr_str;
			CExpression::ToExpression(coef,3,expr_str,17);
			if(strchr(expr_str,'.')!=NULL)	//与单元长度比值为不规则小数
			{
				int min_i=min(pElem->smcinfo.lamdaItems[i].pStart->point_i,pElem->smcinfo.lamdaItems[i].pEnd->point_i);
				int max_i=max(pElem->smcinfo.lamdaItems[i].pStart->point_i,pElem->smcinfo.lamdaItems[i].pEnd->point_i);
				pElem->smcinfo.lamdaItems[i].braceStr.Printf("%4d~%4d",min_i,max_i);
			}
			else
			{
				pElem->smcinfo.lamdaItems[i].braceStr.Printf("ELEM*%s",(char*)expr_str);
				pElem->smcinfo.lamdaItems[i].braceStr.ResizeLength(9,true);
			}
		}
	}
}
//</DEVELOP_PROCESS_MARK>
	//更新单元长度信息，在PreparaTower中调用
void CPreProcessor::UpdateElemLengthInfo()
{
	for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		pElem->CalLength();	//计算单元长度
}
	//更新单元体荷载(自重及覆冰荷载)信息，选材发生变化时调用
void CPreProcessor::UpdateElemBodyLoadInfo()
{
	if(!LOWEST_ROD_POSZ_INITED)
	{
		SCOPE_STRU scope;
		Ta.GetTaSizeScope(scope);
		LOWEST_ROD_POSZ=scope.fMaxZ/1000.0;
		LOWEST_ROD_POSZ_INITED=true;
	}
	for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		pElem->CalBodyLoad(LOWEST_ROD_POSZ);	//计算单元长度
}
//设定分析处理器对象
void CPreProcessor::SetAnalysisProcessor(CAnalyzeProcessor *pProcessor)
{
	m_pAnalysisProcessor=pProcessor;
	m_pAnalysisProcessor->SetPreProcessor(this);
}
//新体荷载计算算法
bool CPreProcessor::PREPROCESS_RESULT::InitNodeBodyLoads()
{
	hashNodeBodyLoads.Empty();
	CElement* pElem;
	CNodeBodyLoad* pNodeLoad,*pStartRelyNodeLoad,*pEndRelyNodeLoad;
	ELEM_BODYLOAD_ALLOC *pAlloc,*pAlloc2;
	for(pElem=elemList.GetFirst();pElem;pElem=elemList.GetNext())
	{	//均分全部单元(含辅助材在内)的体荷载到单元两端节点上
		//始端
		pNodeLoad=hashNodeBodyLoads.Add(pElem->pStart->handle);
		pNodeLoad->pNode=pElem->pStart;
		pAlloc=pNodeLoad->hashElemAllocs.Add(pElem->id);
		pAlloc->pElem=pElem;
		pAlloc->fAllocCoef=0.5;
		//终端
		pNodeLoad=hashNodeBodyLoads.Add(pElem->pEnd->handle);
		pNodeLoad->pNode=pElem->pEnd;
		pAlloc=pNodeLoad->hashElemAllocs.Add(pElem->id);
		pAlloc->pElem=pElem;
		pAlloc->fAllocCoef=0.5;
	}
	for(CLDSNode* pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{	//设定模型传力节点标记
		if((pNodeLoad=hashNodeBodyLoads.GetValue(pNode->handle))!=NULL)
			pNodeLoad->bBraceNode=false;
		else if(pNode->force_type!=SWING_NODE)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"Internal error #1!");
	}
	bool hasBraceNode=false;
	do{
		hasBraceNode=false;
		for(pNodeLoad=hashNodeBodyLoads.GetFirst();pNodeLoad;pNodeLoad=hashNodeBodyLoads.GetNext())
		{
			if(!pNodeLoad->bBraceNode||pNodeLoad->hashElemAllocs.GetNodeNum()==0)
				continue;	//传力节点或已处理节点(共体荷载已在之前代码中分配到父级搭接节点上)
			//搜索辅材节点在其它单元中间的情况
			ELEM_PTR pResideElemArr[2]={NULL};	//pResideElem2仅用于交叉材
			double proportion[2]={1.0,1.0};
			for(pElem=elemList.GetFirst();pElem;pElem=elemList.GetNext())
			{
				if(!pElem->pOrgPole->ModuleTogetherWith(pNodeLoad->pNode))
					continue;
				double fraction=pElem->CheckPoint(pNodeLoad->pNode->Position(false));
				if(fraction>eps&&fraction<1-eps)
				{
					if( pNodeLoad->pNode->m_cPosCalType==CLDSNode::COORD_INTERSECTION&&
						pNodeLoad->pNode->arrRelationPole[0]>0x20&&pNodeLoad->pNode->arrRelationPole[1]>0x20)
					{
						if(pResideElemArr[0]==NULL)
						{
							pResideElemArr[0]=pElem;
							proportion[0]=fraction;
						}
						else
						{
							pResideElemArr[1]=pElem;
							proportion[1]=fraction;
						}
						if(pResideElemArr[1]!=NULL)
							break;
					}
					else
					{
						pResideElemArr[0]=pElem;
						proportion[0]=fraction;
						break;
					}
				}
			}
			for(int k=0;k<2;k++)
			{
				CElement* pResideElem=pResideElemArr[k];
				if(pResideElem==NULL)
					continue;	//辅材一端连接到传力节点上情况
				double fraction=proportion[k];
				double fAllocCoefOnStart=1-fraction,fAllocCoefOnEnd=fraction;
				if(pResideElemArr[1]!=NULL)
				{	//交叉点属于共用点，所以分配质量应减半
					fAllocCoefOnStart*=0.5;
					fAllocCoefOnEnd*=0.5;
				}
				pStartRelyNodeLoad=hashNodeBodyLoads.GetValue(pResideElem->pStart->handle);
				pEndRelyNodeLoad=hashNodeBodyLoads.GetValue(pResideElem->pEnd->handle);
				bool allocToBraceNode=false;
				for(pAlloc=pNodeLoad->hashElemAllocs.GetFirst();pAlloc;pAlloc=pNodeLoad->hashElemAllocs.GetNext())
				{
					pAlloc2=pStartRelyNodeLoad->hashElemAllocs.Add(pAlloc->idElem);
					pAlloc2->pElem=pAlloc->pElem;
					pAlloc2->fAllocCoef+=pAlloc->fAllocCoef*fAllocCoefOnStart;
					pAlloc2=pEndRelyNodeLoad->hashElemAllocs.Add(pAlloc->idElem);
					pAlloc2->pElem=pAlloc->pElem;
					pAlloc2->fAllocCoef+=pAlloc->fAllocCoef*fAllocCoefOnEnd;
					allocToBraceNode|=(pStartRelyNodeLoad->bBraceNode||pEndRelyNodeLoad->bBraceNode);
				}
				hasBraceNode|=allocToBraceNode;
			}
			pNodeLoad->hashElemAllocs.Empty();	//该辅材节点的体荷载已分配到父级搭接杆件端节点上
		}
	}while(hasBraceNode);
	for(pNodeLoad=hashNodeBodyLoads.GetFirst();pNodeLoad;pNodeLoad=hashNodeBodyLoads.GetNext())
	{
		pNodeLoad->hasLegElemLink=false;
		for(pAlloc=pNodeLoad->hashElemAllocs.GetFirst();pAlloc;pAlloc=pNodeLoad->hashElemAllocs.GetNext())
		{
			if(!pAlloc->pElem->pOrgPole->IsLegObj())
				continue;
			pNodeLoad->hasLegElemLink=true;
			break;
		}
	}
	return false;
}
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::UpdateInfoAtElemSizeChange">
//初次调用或在单元材料规格发生变化时调用更新相关计算信息
void CPreProcessor::UpdateInfoAtElemSizeChange()
{
	//具有一致配材号的标准风压段挡风面积计算
	windSegPickerList.Empty();
	InitWindSegElemPicker(m_pTower);
	//更新单元体荷载(自重及覆冰荷载)信息，选材发生变化时调用
	UpdateElemBodyLoadInfo();
	//更新节点连接单元的支撑刚度
	CHashTable<double>hashElemStiff;
	hashElemStiff.CreateHashTable(result.elemList.GetNodeNum());
	ValidateElemSupportStiff();
	for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		double stiff=pElem->GetWorkI()/pElem->Length();
		CElement* pFatherElem=FindFatherElem(pElem);
		if(pFatherElem&&pElem!=pFatherElem)
		{
			pElem->m_bValidStartRestStiff=pFatherElem->m_bValidStartRestStiff;
			pElem->m_bValidEndRestStiff=pFatherElem->m_bValidEndRestStiff;
		}
		hashElemStiff.SetValueAt(pElem->id,stiff);
		//按照选材前杆件规格预计算每个单元的压稳信息，以便于遴选出最危险的压稳工况
		//TODO:(4)[实时更新压稳计算]极大影响计算效率　wjh-2016.2.25
		CalElemCompStableInfo(pElem,ActiveDesignCode,false);//Ta.m_iMemberSizeCheckCode
	}
	for(CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetFirst();pNodeLinks;pNodeLinks=result.hashNodeLinks.GetNext())
	{
		for(ELEMLATERAL_STIFF* pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
			hashElemStiff.GetValueAt(pElemStiff->pElem->id,pElemStiff->stiff);
	}
	//计算折算到节点上的杆塔自重荷载及覆冰荷载及各工况下荷载节点的等效集中荷载值
	result.hashNodeBodyLoad.EmptyKeys();	//由于规格发生变化，需要清空原塔架等效节点自重荷载
	result.hashElemStartBodyLoad.EmptyKeys();
	result.hashElemEndBodyLoad.EmptyKeys();
	//[规格变化时更新单元体荷载]
	for(CLDSNode* pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
	{
		BOOL pushed=result.nodeSet.push_stack();
		pNode->bodyLoad=GetBodyLoad(pNode);
		result.nodeSet.pop_stack(pushed);
	}
	//更新辅材单元的单元截面积，以便计算轴向支撑刚度系数及部分共用支撑的辅材支撑力分配系数
	for(CNodeBraceAuxElems *pBrace=result.listBraceLinks.GetFirst();pBrace;pBrace=result.listBraceLinks.GetNext())
	{
		for(int i=0;i<pBrace->linkElemsConst.GetSize();i++)
			pBrace->linkElemsConst[i].A=pBrace->linkElemsConst[i].pElem->pOrgPole->GetArea();
		if(!pBrace->cfgword.IsNull())
		{
			CalLinePartCoef(pBrace->linkElemsConst.m_pData,pBrace->linkElemsConst.GetSize());
			for(i=0;i<pBrace->linkElemsConst.GetSize();i++)
			{
				if(pBrace->linkElemsConst[i].pElem->pStart==pBrace->pNode)
					pBrace->linkElemsConst[i].pElem->m_fStartBraceNCoef=pBrace->linkElemsConst[i].brace_coef;
				else if(pBrace->linkElemsConst[i].pElem->pEnd==pBrace->pNode)
					pBrace->linkElemsConst[i].pElem->m_fEndBraceNCoef=pBrace->linkElemsConst[i].brace_coef;
			}
		}
	}
}
//</DEVELOP_PROCESS_MARK>
//初次调用或计算呼高组发生变化时调用更新呼高信息
void CPreProcessor::UpdateInfoAtModuleChange(CLDSModule *pModule)
{
	//计算基础Ｚ坐标
	pModule->GetModuleScopeZ(NULL,NULL);
}
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::UpdateInfoAtLegChange">
//初次调用或计算模型中接腿发生变化时调用更新构件验算相关信息
void CPreProcessor::UpdateInfoAtLegChange(CLDSModule *pModule,CBlockResolver* pResolver/*=NULL*/)
{
	//更新节点上分配的等效节点体荷载
	//[接腿轮换时更新单元体荷载]
	for(CLDSNode* pNode=m_pAnalysisProcessor->nodeset.GetFirst();pNode;pNode=m_pAnalysisProcessor->nodeset.GetNext())
		pNode->bodyLoad=GetBodyLoad(pNode,pModule);
	//计算剩余非共用部分支撑的辅材支撑力分配系数
	for(CNodeBraceAuxElems *pBrace=result.listBraceLinks.GetFirst();pBrace;pBrace=result.listBraceLinks.GetNext())
	{
		if(!pModule->IsSonPart(pBrace->pNode,&pBrace->cfgword))
		{
			ARRAY_LIST<AUXELEM_CONST>auxElemConst;
			auxElemConst.SetSize(0,2);
			for(int i=0;i<pBrace->linkElemsConst.GetSize();i++)
			{
				if(pModule->IsSonPart(pBrace->linkElemsConst[i].pElem->pOrgPole))
					auxElemConst.append(pBrace->linkElemsConst[i]);
			}
			CalLinePartCoef(auxElemConst.m_pData,auxElemConst.GetSize());
			for(i=0;i<auxElemConst.GetSize();i++)
			{
				if(auxElemConst[i].pElem->pStart==pBrace->pNode)
					auxElemConst[i].pElem->m_fStartBraceNCoef=auxElemConst[i].brace_coef;
				else if(auxElemConst[i].pElem->pEnd==pBrace->pNode)
					auxElemConst[i].pElem->m_fEndBraceNCoef=auxElemConst[i].brace_coef;
			}
		}
	}
	m_pAnalysisProcessor->AnalyzeElemDegree(pResolver);	//有待优化
	for(CElement* pElem=m_pAnalysisProcessor->hashElems.GetFirst();pElem;pElem=m_pAnalysisProcessor->hashElems.GetNext())
	{
		CElemInfo* pElemInfo = this->m_pTower->Elems.GetValue(pElem->pStart->handle,pElem->pEnd->handle);
		if(pElemInfo)
		{
			pElemInfo->ciFreedomStart=pElem->ciFreedomStart;
			pElemInfo->ciFreedomEnd=pElem->ciFreedomEnd;
		}
	}
}
//</DEVELOP_PROCESS_MARK>
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::CalRodForceByBanlance">
//inputV为输入的力包含了力的方向和大小信息，v1与v2输入时仅代表另外两根杆件的射线方向，输出时表示输出的力(含大小与方向信息）
bool CalRodForceByBanlance(GEPOINT inputV,double* v1,double* v2)
{
	GECS wcs;
	GEPOINT rv1(v1),rv2(v2);
	if(inputV.IsZero())
	{	//输入力为零，输出也为零
		v1[0]=v1[1]=v1[2]=v2[0]=v2[1]=v2[2]=0.0;
		return true;
	}
	wcs.axis_x=inputV;
	wcs.axis_z=cross_prod(wcs.axis_x,rv1);
	if(wcs.axis_z.IsZero())
		return false;//logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"节点当前")
	normalize(wcs.axis_x);
	normalize(wcs.axis_z);
	wcs.axis_y=cross_prod(wcs.axis_z,wcs.axis_x);
	normalize(rv1);
	normalize(rv2);
	if(fabs(wcs.axis_z*rv2)>EPS)
		return false;	//不共面
	double input_fx=inputV.mod();
	GEPOINT lrv1=wcs.TransVToCS(rv1);	//v1在局部坐标系下的单位化射线方向（承力轴向）
	GEPOINT lrv2=wcs.TransVToCS(rv2);	//v2在局部坐标系下的单位化射线方向（承力轴向）
	//解方程: lrv1.x*P1+lrv2.x*P2=-input_fx;
	//        lrv1.y*P1+lrv2.y*P2=0;
	double baseF=(lrv1.x*lrv2.y-lrv2.x*lrv1.y);
	if(fabs(lrv1.y)<EPS||fabs(lrv2.y)<EPS||fabs(baseF)<EPS)
		return false;
	double P1=-lrv2.y*input_fx/baseF;
	double P2= lrv1.y*input_fx/baseF;
	lrv1*=P1;
	lrv2*=P2;
	rv1=wcs.TransVFromCS(lrv1);
	rv2=wcs.TransVFromCS(lrv2);
	v1[0]=rv1.x;
	v1[1]=rv1.y;
	v1[2]=rv1.z;
	v2[0]=rv2.x;
	v2[1]=rv2.y;
	v2[2]=rv2.z;
	return true;
}
//</DEVELOP_PROCESS_MARK>
//将摆动挂线节点上对应工况下(如V串挂线)的外荷载等效分配到非挂线点上
void CPreProcessor::DispatchSwingNodeLoad(CLDSModule *pModule,CWorkStatus *pStatus,bool bApplyStdLoad)
{
	char sError[100];
	bool hasSwingNode=false;
	CLDSNode *pNode;
	//注意此处遍历不能与下面的遍历合在一起，否则会清掉前面摆动节点分派到当前节点上的分力 wjh-2012.6.1
	for(pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
	{
		pNode->vSwingLoadFxyz.Set();	//清零以往摆动节点等效荷载临时记录
		if(pNode->force_type==SWING_NODE)
			hasSwingNode=true;	//摆动节点
	}
	result.bHasSwingNode=hasSwingNode;
	if(!hasSwingNode)
		return;	//无摆动节点
	for(pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
	{
		if(pNode->force_type!=SWING_NODE)
			continue;	//非摆动节点
		LINEPARTSET rodset;
		CLDSLinePart *pRod1=NULL,*pRod2=NULL;
		BOOL bErrorNum=FALSE;
		//result.allrodset中不包含摆动节点的关联杆件,故只能使用m_pTower->Parts遍历 wjh-2017.3.26
		//for(CLDSLinePart *pRod=result.allrodset.GetFirst();pRod;pRod=result.allrodset.GetNext())
		for(CLDSLinePart *pRod=m_pTower->EnumRodFirst();pRod;pRod=m_pTower->EnumRodNext())
		{
			CLDSLinePart* pConnRod=NULL;
			if(pRod->pStart==pNode||pRod->pEnd==pNode)
				pConnRod=pRod;
			if(pConnRod&&pRod1==NULL)
				pRod1=pRod;
			else if(pConnRod&&pRod2==NULL)
				pRod2=pRod;
			else if(pRod1!=NULL&&pRod2!=NULL&&pConnRod)
			{
				bErrorNum=TRUE;
				break;
			}
		}
		if(bErrorNum||pRod1==NULL||pRod2==NULL)
		{
			sprintf(sError,"0X%X节点摆动节点关联杆件数不是2,请先核查后再进行力学分析!",pNode->handle);
			throw sError;
		}
		f3dLine axis_line;
		CLDSNode *pHangNode1,*pHangNode2;
		if(pRod1->pStart==pNode)
			pHangNode1=pRod1->pEnd;
		else
			pHangNode1=pRod1->pStart;
		if(pRod2->pStart==pNode)
			pHangNode2=pRod2->pEnd;
		else
			pHangNode2=pRod2->pStart;
		axis_line.startPt=pHangNode1->Position(false);
		axis_line.endPt=pHangNode2->Position(false);
		f3dPoint init_v_norm,work_norm,work_pos,exter_load;
			//1.与工况相关的集中外荷载(含可变荷载及Ｚ向持久重力荷载)
		CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
		if(pNodeLoad)
		{
			if(pStatus->m_bLoadIncFactor)
			{
				if(bApplyStdLoad)
					exter_load=pNodeLoad->stdLoad+f3dPoint(0,0,pNodeLoad->permanentFz);
				else
					exter_load=pNodeLoad->designLoad+f3dPoint(0,0,pNodeLoad->permanentFz);
			}
			else
				exter_load=pNodeLoad->designLoad=Ta.m_fGamaGForDesign*pNodeLoad->permanentFz+pStatus->Kesai()*pStatus->GamaQ()*pNodeLoad->Fxyz;
			if(exter_load.IsZero())
				continue;	//当前工况节点无实际荷载力，跳过该节点
			GEPOINT axis_vec=axis_line.endPt-axis_line.startPt;
			GEPOINT v1=pNode->Position(false)-axis_line.startPt;
			GEPOINT v2=pNode->Position(false)-axis_line.endPt;
			GECS cs;
			cs.axis_z=axis_vec;
			normalize(cs.axis_z);
			cs.axis_x=v1;
			cs.axis_x-=(v1*cs.axis_z)*cs.axis_z;
			normalize(cs.axis_x);
			cs.axis_y=cs.axis_z^cs.axis_x;
			GEPOINT loadvec=cs.TransVToCS(exter_load);
			if(fabs(loadvec.x)+fabs(loadvec.y)>EPS)
			{	//旋转摆动杆件(模拟绝缘子串)至实际受力平面上
				double rot_ang=Cal2dLineAng(0,0,loadvec.x,loadvec.y);
				RotateVectorAroundVector(v1,rot_ang,cs.axis_z);
				RotateVectorAroundVector(v2,rot_ang,cs.axis_z);
			}
			//计算方法1:根据节点平衡法
			GEPOINT vv1=-v1,vv2=-v2;
			if(!CalRodForceByBanlance(exter_load,v1,v2))
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d号摆动节点在%d号工况下的V串力分解失败，请核实荷载力与V串方向是否合理!",pNode->point_i,pStatus->iNo);
			else
			{	//摆动节点将持久荷载也计入可变荷载
				pHangNode1->vSwingLoadFxyz-=v1;
				pHangNode2->vSwingLoadFxyz-=v2;
				if(v1*vv1<0)
					logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%4d号-%4d绝缘子串在%d号工况下出现受压情况,压力：%.1fkN",pNode->pointI,pHangNode1->pointI,pStatus->iNo,v1.mod()*0.001);
				if(v2*vv2<0)
					logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%4d号-%4d绝缘子串在%d号工况下出现受压情况,压力：%.1fkN",pNode->pointI,pHangNode2->pointI,pStatus->iNo,v2.mod()*0.001);
			}
			//计算方法2:根据延长线计算和杠杆原理分解(缺点是未考虑两挂点间的相向力)
			/*f3dPoint inters_pt;	//力作用线与V串上连接线的交点
			int ret=Int3dll(axis_line.startPt,axis_line.endPt,work_pos,work_pos+exter_load,inters_pt);
			if(ret==0||ret==1)
			{	//交点无效时荷载均分
				pHangNode1->Fxyz=pHangNode2->Fxyz=0.5*pNodeLoad->Fxyz;
				pHangNode1->Fz2=pHangNode2->Fz2=0.5*pNodeLoad->permanentFz;
			}
			else
			{
				normalize(axis_vec);
				f3dPoint vec1=inters_pt-axis_line.startPt;
				f3dPoint vec2=inters_pt-axis_line.endPt;
				double len1=vec1*axis_vec;
				double len2=-(vec2*axis_vec);
				double coef1=len2/(len1+len2);
				double coef2=len1/(len1+len2);
				pHangNode1->Fxyz=coef1*pNodeLoad->Fxyz;
				pHangNode1->Fz2=coef1*pNodeLoad->permanentFz;
				pHangNode2->Fxyz=coef2*pNodeLoad->Fxyz;
				pHangNode2->Fz2=coef2*pNodeLoad->permanentFz;
			}*/
		}
	}
}
//初次调用或工况发生变化时调用更新相关计算信息
void CPreProcessor::UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus,bool bVerifyFoundation,bool bApplyStdLoad,bool bEqCheck,int fLoadStepCoef/*=1.0*/)
{
	/*if(bVerifyFoundation)	//验算基础
		CalWindAndAttachLoad(m_pTower,pModule,pStatus,g_sysPara.m_iLoadExecuteCode,g_sysPara.SelMatPara.m_fWindBetaZForFoundation,1.0);
	//为什么这里没有else？否则上一行计算基础时不是白算了？怀疑是以前的一个笔误。 wjh-2011-5-29
	else
		CalWindAndAttachLoad(m_pTower,pModule,pStatus,g_sysPara.m_iLoadExecuteCode,0.0,pStatus->Bw_I());*/
	//为支持计算基础时,风振系数可分段取值,现在统一改为如下形式 wjh 2011.9.22
	CalWindAndAttachLoad(m_pTower,pModule,pStatus,Ta.m_iLoadExecuteCode,bVerifyFoundation,pStatus->Bw_I());
	if(bEqCheck&&pStatus->m_cMainLoadType>=5&&pStatus->m_cMainLoadType<=8)	//抗震验算荷载
	{
		if(!CalEarthQuakeLoad(m_pTower,pModule,pStatus,Ta.m_iLoadExecuteCode))
			throw "找不到结构的振型文件*.frq，请先进行结构动力特性分析！";
	}
	if(result.bHasSwingNode)	//能节省因每次遍历带来的时间开销
		DispatchSwingNodeLoad(pModule,pStatus,bApplyStdLoad);
	ApplyNodeLoad(m_pAnalysisProcessor->femNodeList,pModule,pStatus,bApplyStdLoad);
}
int CPreProcessor::GetForceFaceNum(CLDSNode *pNode,f3dPoint axis_vec,f3dPoint *force_face_norm/*=FALSE*/)
{
	f3dPoint force_face_vec,face_norm[2];
	if(!axis_vec.IsZero())
		normalize(axis_vec);
	int num_face=0;
	for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		if(pElem->pOrgPole->IsAuxPole())
			continue;	//辅材单元
		if(pElem->pStart!=pNode&&pElem->pEnd!=pNode)	//非以此节点为端节点的杆件单元
			continue;
		if(axis_vec.IsZero())
		{
			axis_vec=pElem->pEnd->Position(false)-pElem->pStart->Position(false);
			normalize(axis_vec);
			continue;
		}
		force_face_vec=pElem->pEnd->Position(false)-pElem->pStart->Position(false);
		normalize(force_face_vec);
		if(fabs(axis_vec*force_face_vec)>EPS_COS2)	//共线同向杆件单元
			continue;
		if(num_face==0)
		{
			face_norm[0]=axis_vec^force_face_vec;
			normalize(face_norm[0]);
			num_face=1;
		}
		else if(fabs(force_face_vec*face_norm[0])>EPS2)
		{	//出现第二受力面杆件
			num_face=2;
			break;
		}
	}
	if(force_face_norm)
		*force_face_norm=face_norm[0];
	if(pNode->IsHasRestrict(X_RESTRICT)&&fabs(face_norm[0].x)>EPS2)
		num_face++;
	if(pNode->IsHasRestrict(Y_RESTRICT)&&fabs(face_norm[0].y)>EPS2)
		num_face++;
	if(pNode->IsHasRestrict(Z_RESTRICT)&&fabs(face_norm[0].z)>EPS2)
		num_face++;
	num_face=min(num_face,2);
	return num_face;
}
//分析各承力单元端节点的自由度及连接类型
void CPreProcessor::AnalyzeElemDegree()
{
	CString sError;
	CHashTable<int>forceSupportTbl;
	int startForceSupport,endForceSupport;
	forceSupportTbl.CreateHashTable(result.nodeSet.GetNodeNum());
	//m_bHasCableElem=false;
	for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		if(pElem->pOrgPole->IsAuxPole())
			continue;	//辅材
		//else if(pElem->elem_type==1)
		//	m_bHasCableElem=true;	//结构体中有拉线单元
		BOOL pushed=result.elemList.push_stack();
		if(!forceSupportTbl.GetValueAt(pElem->pStart->point_i,startForceSupport))
		{
			startForceSupport=GetForceFaceNum(pElem->pStart,f3dPoint(0,0,0),NULL);
			forceSupportTbl.SetValueAt(pElem->pStart->point_i,startForceSupport);
		}
		if(!forceSupportTbl.GetValueAt(pElem->pEnd->point_i,endForceSupport))
		{
			endForceSupport=GetForceFaceNum(pElem->pEnd,f3dPoint(0,0,0),NULL);
			forceSupportTbl.SetValueAt(pElem->pEnd->point_i,endForceSupport);
		}
		if(pElem->pOrgPole->m_iElemType==0)
		{	//自动判断
			if(startForceSupport>=2)
				pElem->real_start_joint_type=JOINT_HINGE;
			else if(pElem->pStart!=pElem->pOrgPole->pStart)
				pElem->real_start_joint_type=JOINT_RIGID;
			else
				pElem->real_start_joint_type=JOINT_HINGE;
			if(endForceSupport>=2)
				pElem->real_end_joint_type=JOINT_HINGE;
			else if(pElem->pEnd!=pElem->pOrgPole->pEnd)
				pElem->real_end_joint_type=JOINT_RIGID;
			else
				pElem->real_end_joint_type=JOINT_HINGE;
		}
		else if(pElem->pOrgPole->m_iElemType==1)
		{	//二力杆桁架单元
			pElem->real_start_joint_type=JOINT_HINGE;
			pElem->real_end_joint_type=JOINT_HINGE;
		}
		else if(pElem->pOrgPole->m_iElemType==2)
		{	//经典梁单元
			if(pElem->pStart!=pElem->pOrgPole->pStart)
				pElem->real_start_joint_type=JOINT_RIGID;
			else if(startForceSupport<2||pElem->pOrgPole->start_joint_type==JOINT_RIGID)
				pElem->real_start_joint_type=JOINT_RIGID;
			else
				pElem->real_start_joint_type=JOINT_HINGE;
			if(pElem->pEnd!=pElem->pOrgPole->pEnd)
				pElem->real_end_joint_type=JOINT_RIGID;
			else if(endForceSupport<2||pElem->pOrgPole->end_joint_type==JOINT_RIGID)
				pElem->real_end_joint_type=JOINT_RIGID;
			else
				pElem->real_end_joint_type=JOINT_HINGE;
		}
		result.elemList.pop_stack(pushed);
		//判定各节点的自由度
		if(pElem->real_start_joint_type==JOINT_RIGID&&pElem->GetWorkI()<=0)
		{
			sError.Format("系统把0X%X杆件内单元始端作为梁处理，但此杆件不具有抗弯能力，请检查规格表!",pElem->pOrgPole->handle);
			throw sError;
			//pElem->pStart->dimension=6;
		}
		/*else if(pElem->pFemElem->pStart->dimension!=6)
			pElem->pFemElem->pStart->dimension=3;
		if(pElem->pFemElem->end_joint_type==JOINT_RIGID)
		{
			if(pElem->GetWorkI()<=0)
			{
				sError.Format("系统把0X%X杆件内单元终端作为梁处理，但此杆件不具有抗弯能力，请检查规格表!",pElem->pOrgPole->handle);
				throw sError;
			}
			pElem->pFemElem->pEnd->dimension=6;
		}
		else if(pElem->pFemElem->pEnd->dimension!=6)
			pElem->pFemElem->pEnd->dimension=3;*/
	}
	for(CLDSNode* pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
	{
		int supportDirections=0;
		forceSupportTbl.GetValueAt(pNode->point_i,supportDirections);
		if(supportDirections>=2)
			continue;
		CHashList<GEPOINT>hashVectors;
		//对于轴向支撑或平面支撑杆件，应寻找穿越节点的共线单元对 wjh-2015.6.3
		for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{
			if(pElem->pFemElem==NULL)
				continue;	//辅材
			if(pElem->pStart!=pNode&&pElem->pEnd!=pNode)
				continue;
			double prev_justify=0;
			CElement* pJointElem=NULL,*pPerfectJointElem=NULL;
			GEPOINT lenStdVec=pElem->LenStdVec();
			for(GEPOINT *pGeVec=hashVectors.GetFirst();pGeVec;pGeVec=hashVectors.GetNext())
			{
				double justify=fabs(lenStdVec*(*pGeVec));
				if(justify>EPS_COS2)	//共线
				{
					result.hashElem.GetValueAt(hashVectors.GetCursorKey(),pJointElem);
					if(justify>prev_justify)
					{
						pPerfectJointElem=pJointElem;
						prev_justify=justify;
						break;
					}
				}
			}
			pJointElem=pPerfectJointElem;
			if(pGeVec==NULL)
				hashVectors.SetValue(pElem->id,lenStdVec);
			else if(pJointElem)
			{
				if(pElem->pStart==pNode&&pElem->start_joint_type!=JOINT_HINGE)
					pElem->real_start_joint_type=JOINT_RIGID;
				else if(pElem->pEnd==pNode&&pElem->end_joint_type!=JOINT_HINGE)
					pElem->real_end_joint_type=JOINT_RIGID;
				if(pJointElem->pStart==pNode&&pJointElem->start_joint_type!=JOINT_HINGE)
					pJointElem->real_start_joint_type=JOINT_RIGID;
				else if(pJointElem->pEnd==pNode&&pJointElem->end_joint_type!=JOINT_HINGE)
					pJointElem->real_end_joint_type=JOINT_RIGID;
				break;
			}
		}
	}
}
bool CPreProcessor::PrepareTowerSubBlock(CBlockResolver* pResolver,CTower *pTower,const CFGWORD& cfgwordBody,const CFGWORD& cfgwordLeg)
{
	LINEPARTSET poleset;
	CElement *pElem;
	CFGWORD bodyCfgWord;
	CLDSNode *pNode;
	CLDSLinePart *pLinePart;
	CRodIncIntervals *pRodIntervals;
	m_pTower=pTower;
	result.cfgwordBody=cfgwordBody;
	result.cfgwordLeg=cfgwordLeg;
	//校验是否需要考虑塔身斜材的埃菲尔效应
	m_bIncEiffelFactorX=m_bIncEiffelFactorY=ActiveDesignCode->IsIncEiffelEffect();
	VerifyEiffelFactor(pTower);
	//清空原有数据
	Empty();
	if(InheritStableModelInfo(*pTurboBuffer))
		return true;
	hashTowerNodeI.CreateHashTable(pTower->Node.GetNodeNum());
	//一、分析汇总传力节点及杆件集合
	for(pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
	{
		pNode->UnifiedCfgword();
		if(pNode->layer(0)=='L'&&cfgwordLeg.And(pNode->cfgword))
			result.allnodeset.append(pNode);
		else if(cfgwordBody.And(pNode->cfgword))
			result.allnodeset.append(pNode);
		hashTowerNodeI.SetValueAt(pNode->point_i,pNode);
		if(pNode->force_type==AUTO_NODE)
			pNode->feature=0;	//置为非传力节点（非受力材端点）
		else if(pNode->force_type==FORCE_NODE)
			pNode->feature=1;	//手动设定的传力节点
		else //if(pNode->force_type==SWING_NODE)
			pNode->feature=0;	//摆动节点上的荷载被等效到其余节点上后将被忽略
	}
	for(pLinePart=pTower->Parts.GetFirstLinePart();pLinePart;pLinePart=pTower->Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
			continue;
		if(pLinePart->pStart->force_type==SWING_NODE||pLinePart->pEnd->force_type==SWING_NODE)
			continue;	//与摆动节点相连的杆件为摆动杆件,将被忽略
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//组合角钢中的子角钢，由组合角钢在力学分析时代理所有行为不应单独重复考虑
#endif
		pLinePart->UnifiedCfgword();
		if(pLinePart->layer(0)=='L'&&cfgwordLeg.And(pLinePart->cfgword))
			result.allrodset.append(pLinePart);
		else if(cfgwordBody.And(pLinePart->cfgword))
			result.allrodset.append(pLinePart);
		if(pLinePart->cMaterial==0)
			throw "存在杆件缺少材质信息,请进行数据完整性校验!";
		if(pLinePart->IsAuxPole())
			continue;
		if(pLinePart->pStart) 
			pLinePart->pStart->feature=1;
		if(pLinePart->pEnd) 
			pLinePart->pEnd->feature=1;
	}
	int i;
	//
	CLDSNodePtr* nodeArr=new CLDSNodePtr[result.allnodeset.GetNodeNum()];
	for(pNode=result.allnodeset.GetFirst(),i=0;pNode;pNode=result.allnodeset.GetNext(),i++)
		nodeArr[i]=pNode;
	CQuickSort<CLDSNode*>::QuickSort(nodeArr,result.allnodeset.GetNodeNum(),compareNodeOrderByPointI);
	CHashList<CLDSNodePtr>hashPointI;
	bool passNodeICheck=true;
	for(i=0;i<result.allnodeset.GetNodeNum();i++)
	{
		if(nodeArr[i]->feature!=0)
		{
			if(nodeArr[i]->point_i<=0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Node 0X%X not numbered legally(%d)",nodeArr[i]->handle,nodeArr[i]->point_i);
#else
				logerr.Log("节点0X%X还未进行合法编号(%d)",nodeArr[i]->handle,nodeArr[i]->point_i);
#endif
				passNodeICheck=false;
			}
			else if(hashPointI.GetValue(nodeArr[i]->point_i)!=NULL)
			{
				pNode=hashPointI[nodeArr[i]->point_i];
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("node 0X%X and node 0X%X share the same number(%d),serial number is not reasonable!",pNode->handle,nodeArr[i]->handle,pNode->point_i);
#else
				logerr.Log("节点0X%X与节点0X%X有相同的编号(%d)，编号不合理！",pNode->handle,nodeArr[i]->handle,pNode->point_i);
#endif
				passNodeICheck=false;
			}
			else
				hashPointI.SetValue(nodeArr[i]->point_i,nodeArr[i]);
			result.nodeSet.append(nodeArr[i]);
		}
	}
	delete []nodeArr;
	if(!passNodeICheck)
		return false;
	//分配节点体荷载哈希表存储空间
	result.hashNodeBodyLoad.CreateHashTable(result.nodeSet.GetNodeNum());
	//二、分析拆分杆件为单元集合
	//对铁塔中较长杆件需要中间分段的情况进行处理,规则如下：
	//1.对一长杆件上任两个受力节点间的一段算作一个应力分析单元段
	//2.对补助材不进行分析略过不算作应力分析单元
	for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
	{
		//if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
		//	continue;
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
		//if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&pLinePart->hGr
		if(pLinePart->IsAuxPole())	//补助材
		{
			pElem=result.elemList.append();
			pElem->cfgword=pLinePart->cfgword;
			if(pLinePart->size_idClassType==CLS_CABLE)
			{	//拉线单元
				pElem->elem_type =1;
				pElem->T0=pLinePart->m_fPreTension;
				if(pLinePart->pEnd->IsHasRestrict(X_RESTRICT|Y_RESTRICT|Z_RESTRICT))
					pElem->bAnchorEnd=true;
				else if(pLinePart->pStart->IsHasRestrict(X_RESTRICT|Y_RESTRICT|Z_RESTRICT))
					pElem->bAnchorEnd=false;
				else if(pLinePart->pStart->Position(false).z>pLinePart->pEnd->Position(false).z)
					pElem->bAnchorEnd=false;
				else //if(pLinePart->pEnd->pos.z>pLinePart->pStart->pos.z)
					pElem->bAnchorEnd=true;
			}
			pElem->pOrgPole=pLinePart;
			pElem->pStart=pLinePart->pStart;
			pElem->pEnd=pLinePart->pEnd;
			pElem->start_force_type=pLinePart->start_force_type;
			pElem->end_force_type=pLinePart->end_force_type;
			pElem->start_joint_type=pLinePart->start_joint_type;
			pElem->end_joint_type=pLinePart->end_joint_type;
		}
		else								//受力材
		{
			NODESET nodeset;
			SelectNodeInPole(pLinePart,nodeset);
			int n = nodeset.GetNodeNum();
			if(n==2||pLinePart->size_idClassType==CLS_CABLE)
			{
				pElem = result.elemList.append();
				pElem->cfgword=pLinePart->cfgword;
				pElem->pStart=pLinePart->pStart;
				pElem->pEnd=pLinePart->pEnd;
				pElem->pOrgPole=pLinePart;
				pElem->start_force_type=pLinePart->start_force_type;
				pElem->end_force_type=pLinePart->end_force_type;
				pElem->start_joint_type=pLinePart->start_joint_type;
				pElem->end_joint_type=pLinePart->end_joint_type;
				if(pLinePart->size_idClassType==CLS_CABLE)
				{	//拉线
					pElem->elem_type =1;
					pElem->T0=pLinePart->m_fPreTension;
					if(pLinePart->pEnd->IsHasRestrict(X_RESTRICT|Y_RESTRICT|Z_RESTRICT))
						pElem->bAnchorEnd=true;
					else if(pLinePart->pStart->IsHasRestrict(X_RESTRICT|Y_RESTRICT|Z_RESTRICT))
						pElem->bAnchorEnd=false;
					else if(pLinePart->pStart->Position(false).z>pLinePart->pEnd->Position(false).z)
						pElem->bAnchorEnd=false;
					else //if(pLinePart->pEnd->pos.z>pLinePart->pStart->pos.z)
						pElem->bAnchorEnd=true;
				}
			}
			else if(n>2)
			{
				CLDSNode *pNode=NULL, *pCurNode=NULL;
				CFGWORD basicCfgWord=pLinePart->pStart->cfgword;
				basicCfgWord&=pLinePart->cfgword;
				basicCfgWord&=pLinePart->pEnd->cfgword;
				CFGWORD lineword;
				if(toupper(pLinePart->layer(0))=='L')
					lineword=cfgwordLeg;
				else
					lineword=cfgwordBody;
				basicCfgWord&=lineword;
				lineword&=pLinePart->cfgword;
				if(!basicCfgWord.IsEqual(lineword))
					throw (char*)CXhChar50("0X%X杆件端节点与杆件自身配材号不一致！",pLinePart->handle);
				for(pNode=nodeset.GetFirst();pNode!=NULL&&pNode!=pLinePart->pEnd;pNode=nodeset.GetNext())
				{
					if(pNode->feature==0)
						continue;	//不是传力节点
					CFGWORD processedCfgWord,elemAllCfgWord=basicCfgWord;
					elemAllCfgWord&=pNode->cfgword;
					pElem=result.elemList.append();
					pElem->pStart=pNode;
					BOOL bPush=nodeset.push_stack();
					for(pNode=nodeset.GetNext();pNode;pNode=nodeset.GetNext())
					{
						if(pNode->feature==0)
							continue;
						if(!elemAllCfgWord.And(pNode->cfgword))
							continue;
						pElem->pEnd=pNode;
						pElem->cfgword=elemAllCfgWord;
						pElem->cfgword&=pNode->cfgword;
						pElem->pOrgPole=pLinePart;
						if(pLinePart->size_idClassType==CLS_CABLE)
							pElem->elem_type =1;
						//判断杆单元两端的连接类型
						if(pElem->pStart==pLinePart->pStart)
							pElem->start_joint_type=pLinePart->start_joint_type;
						else
							pElem->start_joint_type=JOINT_RIGID;
						if(pElem->pEnd==pLinePart->pEnd)
							pElem->end_joint_type=pLinePart->end_joint_type;
						else
							pElem->end_joint_type=JOINT_RIGID;
						//判断杆单元两端的受力类型
						if(pElem->pStart==pLinePart->pStart)
							pElem->start_force_type=pLinePart->start_force_type;
						else
							pElem->start_force_type=CENTRIC_FORCE;
						if(pElem->pEnd==pLinePart->pEnd)
							pElem->end_force_type=pLinePart->end_force_type;
						else
							pElem->end_force_type=CENTRIC_FORCE;
						
						processedCfgWord.AddSpecWord(pElem->cfgword);
						if(processedCfgWord.IsEqual(elemAllCfgWord))
							break;
						if(pElem->pStart->Position(false)==pElem->pEnd->Position(false))
							throw "有一单元始末端节点重合，数据有误！";
					}
					if(pElem->pEnd==NULL)
						result.elemList.DeleteCursor(TRUE);
					if(bPush)
						nodeset.pop_stack();
				}
			}
			else	//不合法的杆件(一根杆件至少应有两个端节点在杆件上)
			{
				char error[100]="";
				sprintf(error,"0X%X杆件至少有一个端节点不在当前模型上",pLinePart->handle);
				throw error;
			}
		}
	}
	UpdateElemLengthInfo();	//更新单元长度信息
	for(pElem=result.elemList.GetFirst(),i=1;pElem;pElem=result.elemList.GetNext(),i++)
		pElem->id=i;
	result.InitNodeBodyLoads();	//初始化新版体荷载算法, 必须置于CElement::id赋值以后
	AnalyzeElemDegree();
	for(CLDSModule* pModule=pTower->Module.GetFirst();pModule;pModule=pTower->Module.GetNext())
	{
		if(pModule->feature==0)
			continue;
		ISubBlock* pBlock=pResolver->AddSubBlock(NULL);
		CModuleFrame* pFrame = result.hashModuleFrames.Add(pModule->iNo);
		pFrame->pModule=pModule;
		SUBBLOCKREF* pBlkRef=pFrame->blkrefs.append();
		pBlkRef->serial=pBlock->GetSerial();
		pBlkRef->acs.InitStdCS();
		
		CHashList<CLDSNodePtr> boundary;
		for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{	//子结构中不含接腿材
			if(pElem->pOrgPole->IsAuxPole())
				continue;
			else if(pElem->pOrgPole->Layer(0)!='L'&&pElem->cfgword.IsHasBodyNo(pModule->GetBodyNo()))
			{
				FEM_ELEMENT femElem;
				FEM_NODE femStartNode,femEndNode;
				femElem.idElem=pElem->id;
				femStartNode.point_i=pElem->pStart->point_i;
				femStartNode.pos=pElem->pStart->Position(false);
				femStartNode.wRestrict=pElem->pStart->GetRestrictWord(pModule->handle);
				femEndNode.point_i=pElem->pEnd->point_i;
				femEndNode.pos=pElem->pEnd->Position(false);
				femEndNode.wRestrict=pElem->pEnd->GetRestrictWord(pModule->handle);
				//计算赋值节点上除连接单元外的附加质量，仅用于动力特性分析计算
				femStartNode.attachMass=pElem->pStart->attachMass+pElem->pStart->PermanentLoad.z/9.8;	//考虑附加质量，将来还应考虑辅材质量问题　wjh-2011.9.14
				femEndNode.attachMass=pElem->pEnd->attachMass+pElem->pEnd->PermanentLoad.z/9.8;	//考虑附加质量，将来还应考虑辅材质量问题　wjh-2011.9.14
				femStartNode.pTag=femEndNode.pTag=NULL;
				femStartNode.dimension=femEndNode.dimension=0;

				femElem.pStart=&femStartNode;
				femElem.pEnd=&femEndNode;
				femElem.elem_type=pElem->elem_type;
				femElem.start_joint_type=pElem->real_start_joint_type;
				femElem.end_joint_type=pElem->real_end_joint_type;
				femElem.density=7.85e-6*pElem->pOrgPole->m_fWeightRaiseCoef;	//钢材密度7.85e-6kg/mm3
				femElem.A=pElem->pOrgPole->GetArea();
				femElem.E=pElem->pOrgPole->GetElasticityModulus();
				femElem.G=femElem.E/2.6;
				femElem.Iz=femElem.Ip=pElem->pOrgPole->GetWorkI()*g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef;	//以平行轴半径进行计算
				if(pElem->elem_type==1)
				{
					femElem.T0=pElem->T0;	//拉线单元预张力
					femElem.A=pElem->pOrgPole->GetArea();
					femElem.bAnchorEnd=pElem->bAnchorEnd;
					femElem.E=pElem->pOrgPole->GetWireElasticCoef();
					femElem.density=pElem->pOrgPole->GetWireUnitWeight()/(pElem->pOrgPole->GetWireArea()*1e6);
				}
				else
					femElem.T0=0;
				pBlock->AddElem(femElem);
			}
			else if(pElem->pOrgPole->Layer(0)=='L'&&pElem->cfgword.And(pModule->m_dwLegCfgWord))
			{	//腿身连接处传力节点即为子结构边界节点
				if(pElem->pStart->Layer(0)!='L'&&pElem->pStart->feature!=0)
					boundary.SetValue(pElem->pStart->point_i,pElem->pStart);
				if(pElem->pEnd->Layer(0)!='L'&&pElem->pEnd->feature!=0)
					boundary.SetValue(pElem->pEnd->point_i,pElem->pEnd);
			}
		}
		//SetBoundaryNode必须放在最后，否则求解器中无此节点信息时就设定为边界节点会遭遇失败 wjh-2013-4-28
		for(CLDSNodePtr* ppNode=boundary.GetFirst();ppNode;ppNode=boundary.GetNext())
			pBlock->SetBoundaryNode(boundary.GetCursorKey());
	}
	
	result.hashElemStartBodyLoad.CreateHashTable(result.elemList.GetNodeNum());
	result.hashElemEndBodyLoad.CreateHashTable(result.elemList.GetNodeNum());
	//if(bApplyLoad)
	{
		result.hashElemsMap.Empty();
		result.hashElem.CreateHashTable(result.elemList.GetNodeNum());
		//赋值单元标识及更新节点连接单元信息
		ELEMLATERAL_STIFF lateralStiff;
		for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{
			result.hashElem.SetValueAt(pElem->id,pElem);
			result.hashElemsMap.SetValue(pElem->pStart->handle,pElem->pEnd->handle,pElem);
			lateralStiff.pElem=pElem;
			lateralStiff.stiff=pElem->GetWorkI()/pElem->Length();
			//更新始端节点连接单元信息
			CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pElem->pStart->handle);
			if(pNodeLinks==NULL)
			{
				pNodeLinks=result.hashNodeLinks.Add(pElem->pStart->handle);
				pNodeLinks->pNode=pElem->pStart;
				pNodeLinks->cfgword=pElem->pStart->cfgword;
			}
			pNodeLinks->linkElems.append(lateralStiff);
			pNodeLinks->cfgword&=pElem->cfgword;
			if(!pNodeLinks->cfgword.IsNull()&&pNodeLinks->supportType<3)
			{	//存在共用配材号，且目前尚不是三维立体支撑
				if(pNodeLinks->supportType||pNodeLinks->supportDirect.IsZero())
				{
					pNodeLinks->supportType=1;
					pNodeLinks->supportDirect=pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==1&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//由轴向支撑升级为平面支撑
					pNodeLinks->supportType=2;
					pNodeLinks->supportDirect=pNodeLinks->supportDirect^pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==2&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//由平面支撑升级为三维立体支撑
					pNodeLinks->supportType=3;
				}
			}
			//更新终端节点连接单元信息
			pNodeLinks=result.hashNodeLinks.GetValue(pElem->pEnd->handle);
			if(pNodeLinks==NULL)
			{
				pNodeLinks=result.hashNodeLinks.Add(pElem->pEnd->handle);
				pNodeLinks->pNode=pElem->pEnd;
				pNodeLinks->cfgword=pElem->pEnd->cfgword;
			}
			pNodeLinks->linkElems.append(lateralStiff);
			pNodeLinks->cfgword&=pElem->cfgword;
			if(!pNodeLinks->cfgword.IsNull()&&pNodeLinks->supportType<3)
			{	//存在共用配材号，且目前尚不是三维立体支撑
				if(pNodeLinks->supportType||pNodeLinks->supportDirect.IsZero())
				{
					pNodeLinks->supportType=1;
					pNodeLinks->supportDirect=pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==1&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//由轴向支撑升级为平面支撑
					pNodeLinks->supportType=2;
					pNodeLinks->supportDirect=pNodeLinks->supportDirect^pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==2&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//由平面支撑升级为三维立体支撑
					pNodeLinks->supportType=3;
				}
			}
		}
		//更新辅助材端节点位于受力材单元中间的节点连接情况
		for(pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
		{
			if(pNode->feature==1)	//受力材节点
				continue;
			for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
			{
				if(pElem->pOrgPole->handle!=pNode->RootFatherPartHandle())
					continue;
				f3dPoint vec_start=pNode->Position(false)-pElem->pStart->Position(false);
				f3dPoint vec_end=pNode->Position(false)-pElem->pEnd->Position(false);
				if(vec_start*vec_end<0)
				{
					lateralStiff.pElem=pElem;
					lateralStiff.stiff=pElem->GetWorkI()/pElem->Length();
					CNodeLinkElems *pNodeLinks=result.hashNodeLinks.Add(pNode->handle);
					pNodeLinks->linkElems.append(lateralStiff);
					break;
				}
			}
		}
		//更新单元对称信息
		CHashList<RODMIR_INFO>hashRodMirInfo;
		for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
		{
#ifndef __TSA_
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
				continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
			RODMIR_INFO *pMirInfo=NULL,*pMirXInfo=NULL,*pMirYInfo=NULL,*pMirZInfo=NULL;
			pMirInfo=hashRodMirInfo.GetValue(pLinePart->handle);
			if(pMirInfo!=NULL)
				continue;
			BOOL pushed=pTower->Parts.push_stack();
			CLDSLinePart *pMirXRod=pLinePart->GetMirRod(MIRMSG(1));
			if(pMirXRod!=NULL)
			{
				pMirInfo=hashRodMirInfo.Add(pLinePart->handle);
				pMirInfo->pRod=pLinePart;
				pMirInfo->pRodMirX=pMirXRod;
				pMirXInfo=hashRodMirInfo.Add(pMirXRod->handle);
				pMirXInfo->pRod=pMirXRod;
				pMirXInfo->pRodMirX=pLinePart;
			}
			CLDSLinePart *pMirYRod=pLinePart->GetMirRod(MIRMSG(2));
			if(pMirYRod==pMirXRod)
				pMirYRod=NULL;	//橫隔面交叉材即是X对称又是Y对称
			else if(pMirYRod!=NULL)
			{
				pMirInfo=hashRodMirInfo.Add(pLinePart->handle);
				pMirInfo->pRod=pLinePart;
				pMirInfo->pRodMirY=pMirYRod;
				pMirYInfo=hashRodMirInfo.Add(pMirYRod->handle);
				pMirYInfo->pRod=pMirYRod;
				pMirYInfo->pRodMirY=pLinePart;
			}
			CLDSLinePart *pMirZRod=pLinePart->GetMirRod(MIRMSG(3));
			if(pMirZRod!=NULL)
			{
				pMirInfo=hashRodMirInfo.Add(pLinePart->handle);
				pMirInfo->pRod=pLinePart;
				pMirInfo->pRodMirZ=pMirZRod;
				pMirZInfo=hashRodMirInfo.Add(pMirZRod->handle);
				pMirZInfo->pRod=pMirZRod;
				pMirZInfo->pRodMirZ=pLinePart;
			}
			if(pMirXInfo)
			{
				pMirXInfo->pRodMirY=pMirInfo->pRodMirZ;
				pMirXInfo->pRodMirZ=pMirInfo->pRodMirY;
			}
			if(pMirYInfo)
			{
				pMirYInfo->pRodMirX=pMirInfo->pRodMirZ;
				pMirYInfo->pRodMirZ=pMirInfo->pRodMirX;
			}
			if(pMirZInfo)
			{
				pMirZInfo->pRodMirX=pMirInfo->pRodMirY;
				pMirZInfo->pRodMirY=pMirInfo->pRodMirX;
			}
			pTower->Parts.pop_stack(pushed);
		}

		ARRAY_LIST<ELEM_PTR>elem_arr,elem_arr_x,elem_arr_y,elem_arr_z;
		for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
			pLinePart->feature%=10;	//清空处理标记
		for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{
			pRodIntervals=result.hashRodIntervals.Add(pElem->pOrgPole->handle);
			pRodIntervals->listElems.append(pElem);
		}
		for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
		{
			//if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			//	continue;
#ifndef __TSA_
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
				continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
			if(pLinePart->feature/10!=0)
				continue;	//已处理
			RODMIR_INFO *pMirInfo=hashRodMirInfo.GetValue(pLinePart->handle);
			if(pMirInfo==NULL)
				continue;	//无对称信息
			elem_arr.SetSize(0,10);
			elem_arr_x.SetSize(0,10);
			elem_arr_y.SetSize(0,10);
			elem_arr_z.SetSize(0,10);
			pRodIntervals=result.hashRodIntervals.Add(pLinePart->handle);
			for(ELEM_PTR* ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
				elem_arr.append(*ppElem);

			CLDSLinePart *pMirXRod=pMirInfo->pRodMirX;//(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(1));
			if(pMirXRod!=NULL)
			{
				pMirXRod->feature+=10;
				pRodIntervals=result.hashRodIntervals.Add(pMirXRod->handle);
				for(ELEM_PTR* ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
					elem_arr_x.append(*ppElem);
			}
			CLDSLinePart *pMirYRod=pMirInfo->pRodMirY;//(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(2));
			if(pMirYRod==pMirXRod)
				pMirYRod=NULL;	//橫隔面交叉材即是X对称又是Y对称
			else if(pMirYRod!=NULL)
			{
				pMirYRod->feature+=10;
				pRodIntervals=result.hashRodIntervals.Add(pMirYRod->handle);
				for(ELEM_PTR* ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
					elem_arr_y.append(*ppElem);
			}
			CLDSLinePart *pMirZRod=pMirInfo->pRodMirZ;//(CLDSLinePart*)pLinePart->GetMirPart(MIRMSG(3));
			if(pMirZRod!=NULL)
			{
				pMirZRod->feature+=10;
				pRodIntervals=result.hashRodIntervals.Add(pMirZRod->handle);
				for(ELEM_PTR* ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
					elem_arr_z.append(*ppElem);
			}
			for(i=0;i<elem_arr.GetSize();i++)
			{
				int j;
				CElement *pFatherElem=NULL,*pMirXElem=NULL,*pMirYElem=NULL,*pMirZElem=NULL;
				for(j=0;j<elem_arr_x.GetSize();j++)
				{
					if(elem_arr[i]->id==elem_arr_x[j]->id)
						continue;	//单元自对称
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_x[j]->pStart->Position(false),elem_arr_x[j]->pEnd->Position(false),1))
					{	//X轴对称
						elem_arr[i]->relaElem.idElemMirX=elem_arr_x[j]->id;
						pMirXElem=elem_arr_x[j];
						pMirXElem->relaElem.idElemMirX=elem_arr[i]->id;
						if(elem_arr[i]->CompareElemPointI(elem_arr_x[j])<0)
						{
							pFatherElem=elem_arr[i];
						}
						else
						{
							pFatherElem=pMirXElem;
						}
						break;
					}
				}
				for(j=0;j<elem_arr_y.GetSize();j++)
				{
					if(elem_arr[i]->id==elem_arr_y[j]->id)
						continue;	//单元自对称
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_y[j]->pStart->Position(false),elem_arr_y[j]->pEnd->Position(false),2))
					{	//Y轴对称
						elem_arr[i]->relaElem.idElemMirY=elem_arr_y[j]->id;
						pMirYElem=elem_arr_y[j];
						pMirYElem->relaElem.idElemMirY=elem_arr[i]->id;
						if(pFatherElem==NULL&&elem_arr[i]->CompareElemPointI(elem_arr_y[j])<0)
						{
							pFatherElem=elem_arr[i];
						}
						else if(pFatherElem==NULL||pFatherElem->CompareElemPointI(elem_arr_y[j])>0)
						{
							pFatherElem=pMirYElem;
						}
						break;
					}
				}
				for(j=0;j<elem_arr_z.GetSize();j++)
				{
					if(elem_arr[i]->id==elem_arr_z[j]->id)
						continue;	//单元自对称
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_z[j]->pStart->Position(false),elem_arr_z[j]->pEnd->Position(false),3))
					/*	fabs(elem_arr[i]->pStart->pos.x+elem_arr_z[j]->pStart->pos.x)<EPS&&
						fabs(elem_arr[i]->pStart->pos.y+elem_arr_z[j]->pStart->pos.y)<EPS&&
						fabs(elem_arr[i]->pStart->pos.z-elem_arr_z[j]->pStart->pos.z)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.x+elem_arr_z[j]->pEnd->pos.x)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.y+elem_arr_z[j]->pEnd->pos.y)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.z-elem_arr_z[j]->pEnd->pos.z)<EPS)*/
					{	//Z轴对称
						if(fabs(elem_arr[i]->pStart->Position(false).x)<EPS&&fabs(elem_arr[i]->pEnd->Position(false).x)<EPS)
						{
							elem_arr[i]->relaElem.idElemMirX=elem_arr_z[j]->id;
							pMirXElem=elem_arr_z[j];
							pMirXElem->relaElem.idElemMirX=elem_arr[i]->id;
							if(pFatherElem==NULL&&elem_arr[i]->CompareElemPointI(elem_arr_z[j])<0)
							{
								pFatherElem=elem_arr[i];
							}
							else if(pFatherElem==NULL||pFatherElem->CompareElemPointI(elem_arr_z[j])>0)
							{
								pFatherElem=pMirXElem;
							}
						}
						else if(fabs(elem_arr[i]->pStart->Position(false).y)<EPS&&fabs(elem_arr[i]->pEnd->Position(false).y)<EPS)
						{
							elem_arr[i]->relaElem.idElemMirY=elem_arr_z[j]->id;
							pMirYElem=elem_arr_z[j];
							pMirYElem->relaElem.idElemMirY=elem_arr[i]->id;
							if(pFatherElem==NULL&&elem_arr[i]->CompareElemPointI(elem_arr_z[j])<0)
							{
								pFatherElem=elem_arr[i];
							}
							else if(pFatherElem==NULL||pFatherElem->CompareElemPointI(elem_arr_z[j])>0)
							{
								pFatherElem=pMirYElem;
							}
						}
						else
						{
							elem_arr[i]->relaElem.idElemMirZ=elem_arr_z[j]->id;
							pMirZElem=elem_arr_z[j];
							pMirZElem->relaElem.idElemMirZ=elem_arr[i]->id;
							if(pFatherElem==NULL&&elem_arr[i]->CompareElemPointI(elem_arr_z[j])<0)
							{
								pFatherElem=elem_arr[i];
							}
							else if(pFatherElem==NULL||pFatherElem->CompareElemPointI(elem_arr_z[j])>0)
							{
								pFatherElem=pMirZElem;
							}
						}
						break;
					}
				}
				if(elem_arr[i]->relaElem.idElemMirZ>0)
				{
					if( elem_arr[i]->relaElem.idElemMirZ==elem_arr[i]->relaElem.idElemMirX||
						elem_arr[i]->relaElem.idElemMirZ==elem_arr[i]->relaElem.idElemMirY)
					{
						elem_arr[i]->relaElem.idElemMirZ=0;//即是Z轴对称又是X或Y轴对称时以X或Y轴对称优先显示对称，且不得重复对称
						if(pMirZElem)
							pMirZElem->relaElem.idElemMirZ=0;
					}
					elem_arr[i]->relaElem.bFatherElem=false;
				}
				if(pMirXElem&&elem_arr[i]->relaElem.idElemMirX>0)
				{
					pMirXElem->relaElem.idElemMirY=elem_arr[i]->relaElem.idElemMirZ;
					pMirXElem->relaElem.idElemMirZ=elem_arr[i]->relaElem.idElemMirY;
					if( pMirXElem->relaElem.idElemMirZ==pMirXElem->relaElem.idElemMirX||
						pMirXElem->relaElem.idElemMirZ==pMirXElem->relaElem.idElemMirY)
						pMirXElem->relaElem.idElemMirZ=0;//即是Z轴对称又是X或Y轴对称时以X或Y轴对称优先显示对称，且不得重复对称
					pMirXElem->relaElem.bFatherElem=false;
				}
				if(pMirYElem&&elem_arr[i]->relaElem.idElemMirY>0)
				{
					pMirYElem->relaElem.idElemMirX=elem_arr[i]->relaElem.idElemMirZ;
					pMirYElem->relaElem.idElemMirZ=elem_arr[i]->relaElem.idElemMirX;
					if( pMirYElem->relaElem.idElemMirZ==pMirYElem->relaElem.idElemMirX||
						pMirYElem->relaElem.idElemMirZ==pMirYElem->relaElem.idElemMirY)
						pMirYElem->relaElem.idElemMirZ=0;//即是Z轴对称又是X或Y轴对称时以X或Y轴对称优先显示对称，且不得重复对称
					pMirYElem->relaElem.bFatherElem=false;
				}
				if(pMirZElem&&elem_arr[i]->relaElem.idElemMirZ>0)
				{
					pMirZElem->relaElem.idElemMirX=elem_arr[i]->relaElem.idElemMirY;
					pMirZElem->relaElem.idElemMirY=elem_arr[i]->relaElem.idElemMirX;
					if( pMirZElem->relaElem.idElemMirZ==pMirZElem->relaElem.idElemMirX||
						pMirZElem->relaElem.idElemMirZ==pMirZElem->relaElem.idElemMirY)
						pMirZElem->relaElem.idElemMirZ=0;//即是Z轴对称又是X或Y轴对称时以X或Y轴对称优先显示对称，且不得重复对称
					pMirZElem->relaElem.bFatherElem=false;
				}
				if(pFatherElem)
					pFatherElem->relaElem.bFatherElem=true;
				else
					elem_arr[i]->relaElem.bFatherElem=true;
			}
		}
		InitElemLamdaInfo(true);
		RetrieveFatherElemHashSet();	//提取父单元，以便简化计算单元是否有足够支撑
		CalAuxElemBraceForcePreInfo();	//计算辅材单元的支撑力系数
		if(ActiveDesignCode->GetBraceCoefOfPrimaryToPost()>0)
			CalPrimaryElemBraceForcePreInfo();	//计算斜材单元的支撑力系数
		//三、初始化荷载工况哈希表以便后面查询
		CWorkStatus *pStatus;
		CHashTable<CWorkStatus*>statusHashTbl;
		statusHashTbl.CreateHashTable(Ta.WorkStatus.GetNodeNum());
		for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			if(pStatus->feature==0)	//处于睡眠状态现不处理此计算工况
				continue;
			double W0=pStatus->WindLoad.velocity*pStatus->WindLoad.velocity/1.6;
			statusHashTbl.SetValueAt(pStatus->handle,pStatus);
			double sina=sin(RADTODEG_COEF*pStatus->WindLoad.azimuth);
			double cosa=cos(RADTODEG_COEF*pStatus->WindLoad.azimuth);
			for(CWorkStatus::WINDLOAD_REFPARAM *pRefParam=pStatus->AttachWindLoad.GetFirst();pRefParam;pRefParam=pStatus->AttachWindLoad.GetNext())
			{
				CAttachWindLoad *pWindLoad=pTower->AttachWindLoad.FromHandle(pStatus->AttachWindLoad.GetCursorKey());
				if(pWindLoad==NULL)	//以防数据库中此荷载已删除，而工况中未能及时更新2005/01/17 WJH
					continue;
				double node_wind_area=pWindLoad->m_fShieldArea/pWindLoad->GetNodeList()->GetNodeNum();
				pRefParam->attachMass=pWindLoad->m_fAttachWeight/pWindLoad->GetNodeList()->GetNodeNum();
				double Wk=pRefParam->shieldCoef*pWindLoad->m_fProfileCoef*pWindLoad->m_fBetaZ*node_wind_area*pWindLoad->m_fShieldCoef*W0;
				pRefParam->basicAttachWindLoadF.x=Wk*sina;
				pRefParam->basicAttachWindLoadF.y=Wk*cosa;
			}
		}
		//四、更新长细比关联构件同时受压时长细比计算过程所需的共用部分关联单元信息
		for(CLDSLinePart *pRod=result.allrodset.GetFirst();pRod;pRod=result.allrodset.GetNext())
		{
#ifndef __TSA_
			if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0x20)
				continue;	//组合角钢中的子角钢，其真实意义已由组合角钢代替，故要跳过，否则就重复了
#endif
			CRodIncIntervals *pRodInfo=NULL,*pRefRodInfo=NULL;
			if(pRod->CalLenCoef.hRefPole>0x20)
				pRefRodInfo=result.hashRodIntervals.GetValue(pRod->CalLenCoef.hRefPole);
			if(pRefRodInfo)
				pRodInfo=result.hashRodIntervals.GetValue(pRod->handle);
			if(pRodInfo&&pRefRodInfo&&pRodInfo->listElems.GetNodeNum()==1&&pRefRodInfo->listElems.GetNodeNum()==1)
			{	//只有一个单元时表明杆件与单元是重合的，即杆件不用拆分即是计算单元
				pElem=*pRodInfo->listElems.GetFirst();
				CElement *pRefElem=*pRefRodInfo->listElems.GetFirst();
				if(pElem->cfgword.And(pRefElem->cfgword))
				{	//必须有同时出现的可能存在
					pElem->relaElem.idRefElem=pRefElem->id;
					pRefElem->relaElem.idRefElem=pElem->id;
				}
			}
		}
	}
	return true;
}
#endif

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
	windSegI=0;	//��ѹ�κ�
	cType=0;		//��ѹ������0:�˵�����1:�������2:��������
	cCalStyle=0;	//0.����ʽ����1.X����ˮƽ�ᵣ2.Y����ˮƽ�ᵣ
	cMirType=0;	//�Գ���Ϣ0.�޶Գ�1.���ҶԳ�
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
	double Af=0.0;	//�������
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
			if(fabs(ucs.axis_y*ucs.axis_x)>EPS_COS)	//��X�Ṳ��Ϊ��ЧY�᷽��,��ռ���Ѱ��
				ucs.axis_y.Set();
			else
			{	//�ҵ���������ϵ
				ucs.axis_z = ucs.axis_x^ucs.axis_y;
				ucs.axis_y = ucs.axis_z^ucs.axis_x;
				normalize(ucs.axis_y);
				normalize(ucs.axis_z);
			}
		}
	}
	//�����ѹͶӰ��
	UCS_STRU prjucs;	//��ѹ����������ͶӰ���������ϵ�����ڼ��㹹����Ԫ��ͶӰ�������
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
	//�����������������
	//��������ΰ���
	int n=vertexList.GetSize();
	for(i=0;i<n;i++)
	{
		double result=DistOf2dPtLine(vertexList[(i+2)%n],vertexList[i],vertexList[(i+1)%n]);
		if(result<0)	// ��������Ҳ࣬�а��ǳ���
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
		if((lowz-highz)>0)	//*cosaPrjZ:���ڿ��ǵ�������ڣ������ͶӰ
			weight_deitax+=area*cosaPrjZ*area/(lowz-highz);
	}
	if(m_fWrapArea<EPS)
		return false;
	geomCenter/=m_fWrapArea;
	coord_trans(geomCenter,ucs,TRUE,TRUE);
	
	m_fPrjAreaCoef=fabs(prjucs.axis_z*ucs.axis_z)/1000000;	//��mm2->m2
	double a=weight_deitax/m_fWrapArea;
	m_fWrapArea*=m_fPrjAreaCoef;
	
	//����b_a
	//����ϸ���С������ṹ���ع淶����Լ���Ŀ�ȣ�ʵ��ָÿ�ӭ���泤���н�խ�ĳ߶ȣ�����ָˮƽ��ȡ�wjh-2017.3.1
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
					break;	//�����ĳ���ߵ��Ҳ�ʲ���͹����ε��ڵ�
				if(L3>EPS&&v1.x*v3.y-v1.y*v3.x<-EPS*L1*L3) 
					break;	//�յ���ĳ���ߵ��Ҳ�ʲ���͹����ε��ڵ�
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
			bHasTubeRod=true;	//�иֹ�ʱ��������ϵ����Ҫ���ݷ�ѹ��������
		double A=pFemElem->Length()*pFemElem->pOrgPole->size_wide;;
		if(pFemElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE&&	//�ԽǶԳ�˫�Ǹ����
			(pFemElem->pOrgPole->size_cSubClassType=='D'||pFemElem->pOrgPole->size_cSubClassType=='X'))//ʮ�ֶԳ��ĽǸ����
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
	{	//������������µĶ����������ʱ��
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
	if(pTower->m_cStructureType==1)	//��繹�ܲ���Ҫ���ǰ��ƶ�ЧӦ
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
	if(n<2)	//���¶�����
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
	//�˴�������result.allnodeset������ᶪ�����Ķ˵��ϵ���������
	for(CLDSNode* pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
	{	//������Nodeset������Ϊ���в����������Ķ˵�
		if(pActiveModule!=NULL&&!pActiveModule->IsSonPart(pNode))
			continue;	//���ǵ�ǰģ���е�ǰ�������ϵĽڵ�
		if(pNode==pElem->pStart||pNode==pElem->pEnd)
			continue;
		else if(!pElem->cfgword.And(pNode->cfgword))
			continue;	//����ͬһģ���г���
		//if(pNode->RootFatherPartHandle()!=pElem->pOrgPole->handle)
		//	continue;	//����ͨ�����˼��жϣ���Ϊ������ϽǸּ�����ĵ������������
		double proportion=pElem->CheckPoint(pNode->Position(false));
		if(proportion<EPS||proportion>1-EPS)	//�㲻���߶���,��Ϊ�˽ڵ�
			continue;
		BOOL bPush2=result.allnodeset.push_stack();
		tem_load = GetBodyLoad(pNode,pActiveModule);
		if(bPush2)
			result.allnodeset.pop_stack();
		//double len = pElem->Length();
		//double fraction = DISTANCE(pNode->pos,pElem->pEnd->pos);
		if(pNode->m_cPosCalType==4&&pNode->arrRelationPole[0]>0x20&&pNode->arrRelationPole[1]>0x20)
		{	//��������ڹ��õ㣬���Է�������Ӧ����
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
	//�˴�������result.allnodeset������ᶪ�����Ķ˵��ϵ���������
	for(CLDSNode* pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
	{	//������Nodeset������Ϊ���в����������Ķ˵�
		if(pActiveModule!=NULL&&!pActiveModule->IsSonPart(pNode))
			continue;	//���ǵ�ǰģ���е�ǰ�������ϵĽڵ�
		if(pNode==pElem->pStart||pNode==pElem->pEnd)
			continue;
		else if(!pElem->cfgword.And(pNode->cfgword))
			continue;
		//if(pNode->RootFatherPartHandle()!=pElem->pOrgPole->handle)
		//	continue;	//����ͨ�����˼��жϣ���Ϊ������ϽǸּ�����ĵ������������
		double proportion=pElem->CheckPoint(pNode->Position(false));
		if(proportion<EPS||proportion>1-EPS)	//�㲻���߶���,��Ϊ�˽ڵ�
			continue;
		BOOL bPush2=result.allnodeset.push_stack();
		tem_load = GetBodyLoad(pNode,pActiveModule);
		if(bPush2)
			result.allnodeset.pop_stack();
		//double len = pElem->Length();
		//double fraction = DISTANCE(pNode->pos,pElem->pStart->pos);
		if(pNode->m_cPosCalType==4&&pNode->arrRelationPole[0]>0x20&&pNode->arrRelationPole[1]>0x20)
		{	//��������ڹ��õ㣬���Է�������Ӧ����
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
			//ָ����ǰ����ģ��ʱ��ż���ڶ�����������Ӵ����ܻ������ĺŲ�ƥ������������Ȳ�һ�£���
			//��ʱ��Ȼ��ͨ����IsSonPart��ģ��У�飬����������Ȼ�Ǻ���ģ������ڲ����й����ԣ���˲�
			//���й�ϣ��洢 wjh-2013-2-08
			if(pActiveModule==NULL&&selfload.cfgword.IsNull())
				break;	//���������û�г��ֹ��е���ĺţ��޷���ɻ��ܣ�ֻ���ڷ���ʱ��ʱ���������
		}
	}
	/*else �����κε�Ԫ�Ķ˽ڵ�,�罻���
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
			//ָ����ǰ����ģ��ʱ��ż���ڶ�����������Ӵ����ܻ������ĺŲ�ƥ������������Ȳ�һ�£���
			//��ʱ��Ȼ��ͨ����IsSonPart��ģ��У�飬����������Ȼ�Ǻ���ģ������ڲ����й����ԣ���˲�
			//���й�ϣ��洢 wjh-2013-2-08
			if(pActiveModule==NULL&&selfload.cfgword.IsNull())
				break;	//���������û�г��ֹ��е���ĺţ��޷���ɻ��ܣ�ֻ���ڷ���ʱ��ʱ���������
		}
	}
	// **����Ϊ���㷨��ȷ�ԵĲ��Դ���
	if(pBodyLoad!=NULL&&!pBodyLoad->bBraceNode)
	{
		if(!selfload2.cfgword.IsEqual(selfload.cfgword))
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"Internal error #3! @%d node",pNode->pointI);
		else if(selfload2.cfgword.IsNull()&&selfload.cfgword.IsNull())
			return selfload;	//����ĺţ������������Ӵ���û�б�Ҫ�洢����ϣ����
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
	if( mir_x1_y2_z3==1&&	//X��Գ�
	  ((fabs(start1.x-start2.x)<EPS&&fabs(end1.x-end2.x)<EPS&&
		fabs(start1.y+start2.y)<EPS&&fabs(end1.y+end2.y)<EPS&&
		fabs(start1.z-start2.z)<EPS&&fabs(end1.z-end2.z)<EPS)||
	   (fabs(start1.x-end2.x)<EPS&&fabs(end1.x-start2.x)<EPS&&
		fabs(start1.y+end2.y)<EPS&&fabs(end1.y+start2.y)<EPS&&
		fabs(start1.z-end2.z)<EPS&&fabs(end1.z-start2.z)<EPS)))
		return true;
	if( mir_x1_y2_z3==2&&	//Y��Գ�
	  ((fabs(start1.x+start2.x)<EPS&&fabs(end1.x+end2.x)<EPS&&
		fabs(start1.y-start2.y)<EPS&&fabs(end1.y-end2.y)<EPS&&
		fabs(start1.z-start2.z)<EPS&&fabs(end1.z-end2.z)<EPS)||
	   (fabs(start1.x+end2.x)<EPS&&fabs(end1.x+start2.x)<EPS&&
		fabs(start1.y-end2.y)<EPS&&fabs(end1.y-start2.y)<EPS&&
		fabs(start1.z-end2.z)<EPS&&fabs(end1.z-start2.z)<EPS)))
		return true;
	if( mir_x1_y2_z3==3&&	//Z��Գ�
	  ((fabs(start1.x+start2.x)<EPS&&fabs(end1.x+end2.x)<EPS&&
		fabs(start1.y+start2.y)<EPS&&fabs(end1.y+end2.y)<EPS&&
		fabs(start1.z-start2.z)<EPS&&fabs(end1.z-end2.z)<EPS)||
	   (fabs(start1.x+end2.x)<EPS&&fabs(end1.x+start2.x)<EPS&&
		fabs(start1.y+end2.y)<EPS&&fabs(end1.y+start2.y)<EPS&&
		fabs(start1.z-end2.z)<EPS&&fabs(end1.z-start2.z)<EPS)))
		return true;
	return false;
}

//����˷�������Ч�ʵ���
bool IsAntiSign(double f1,double f2)
{
	if((f1<0&&f2>0) || (f1>0&&f2<0))
		return true;
	else
		return false;
}
struct RODMIR_INFO{
	CLDSLinePart *pRod;	//���˼�
	CLDSLinePart *pRodMirX,*pRodMirY,*pRodMirZ;
	RODMIR_INFO(){memset(this,0,sizeof(RODMIR_INFO));}
};
//���ݶԳ�ԭ����ȡ�����뵥Ԫ����Ӧ�ĸ���Ԫ
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
//������֤(��)��Ԫ�Ƿ����㹻֧��
void CPreProcessor::ValidateElemSupportStiff()
{
	for(CElement* pElem=result.hashFatherElemPtrs.GetFirst();pElem;pElem=result.hashFatherElemPtrs.GetNext())
	{
		if(pElem->BearTens1Comp2==1||pElem->BearTens1Comp2==2)	//���
			continue;
		//�����Ƿ���ṩ�㹻��ת��Լ���ն�
		if( pElem->pOrgPole->IsAuxPole()||	!pElem->pOrgPole->IsPostRod())//���Ļ������ѹ�˵�Ԫ
		{	//������ѹ���Ƿ��кϷ��ĸ���Լ��
			f3dPoint base_vec;
			if(pElem->start_joint_type==JOINT_RIGID)
				pElem->m_bValidStartRestStiff=TRUE;
			else if(pElem->start_joint_type==JOINT_HINGE)
				pElem->m_bValidStartRestStiff=FALSE;
			else if(pElem->start_joint_type==JOINT_AUTOJUSTIFY&&(pElem->pStart!=pElem->pOrgPole->pStart||pElem->pOrgPole->connectStart.wnConnBoltN>=2))
			{	//�˸˼���Ҫ�����ǹ��߸��Ǹֵ�Ԫ�ṩԼ��
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
							{	//��ǰ����ģ���ڵĵ�Ԫ
								if(pElemStiff->pElem->pOrgPole==pFatherRod)
								{
									if(pElemStiff->pElem->pStart==pElem->pStart||pElemStiff->pElem->pEnd==pElem->pStart)
										support_stiff+=pElemStiff->stiff;	//�жϴ˵�Ԫ�Ƿ�Ϊ��ǰģ���е�Ԫ
									else
										support_stiff+=pElemStiff->stiff*4;	//�ڵ��ڵ�Ԫ�м�֧�Ÿնȳ�4 2*I/0.5L=4*I/L
								}
								else if(fabs(pElemStiff->pElem->LenStdVec()*base_vec)>EPS_COS)	//�ṩԼ���ĵ�Ԫ
									support_stiff+=pElemStiff->stiff;
								else if(fabs(pElemStiff->pElem->LenStdVec()*work_norm)<EPS)	//�������ڱ�Լ���ĵ�Ԫ
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
			{	//�˸˼���Ҫ�����ǹ��߸��Ǹֵ�Ԫ�ṩԼ��
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
							{	//��ǰ����ģ���ڵĵ�Ԫ
								if(pElemStiff->pElem->pOrgPole==pFatherRod)
								{
									if(pElemStiff->pElem->pStart==pElem->pEnd||pElemStiff->pElem->pEnd==pElem->pEnd)
										support_stiff+=pElemStiff->stiff;	//�жϴ˵�Ԫ�Ƿ�Ϊ��ǰģ���е�Ԫ
									else
										support_stiff+=pElemStiff->stiff*4;	//�ڵ��ڵ�Ԫ�м�֧�Ÿնȳ�4 2*I/0.5L=4*I/L
								}
								else if(fabs(pElemStiff->pElem->LenStdVec()*base_vec)>EPS_COS)	//�ṩԼ���ĵ�Ԫ
									support_stiff+=pElemStiff->stiff;
								else if(fabs(pElemStiff->pElem->LenStdVec()*work_norm)<EPS)	//�������ڱ�Լ���ĵ�Ԫ
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
//�Զ������������ص㼰��ѹ�������ص��ж�Ϊ�����ڵ㣬������CLDSNode::feature=1;��ʱ��ֵ
void CPreProcessor::SmartJustifyNodeForceType(CTower *pTower,NODESET& nodeset)
{
	CLDSNode* pNode;
	NODESET filter;
	bool hasSwingNode=false;
	for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{	//����ĿǰΪ�Զ��жϴ������͵Ľڵ㼯��
		if(pNode->feature==0)
			filter.append(pNode);
		if(pNode->force_type==SWING_NODE)
			hasSwingNode=true;	//�ڶ��ڵ�
	}
	for(pNode=filter.GetFirst();pNode;pNode=filter.GetNext())
	{
		if(pNode->feature!=0)
			continue;	//Ŀǰ�Ѳ����Զ��жϴ������ͽڵ㣬�����Ѵ����
		for(CWorkStatus* pCase=pTower->WorkStatus.GetFirst();pCase;pCase=pTower->WorkStatus.GetNext())
		{
			CExternalNodeLoad *pNodeLoad=pCase->hashNodeLoad.GetValue(pNode->handle);
			if(pNodeLoad!=NULL&&(!pNodeLoad->Fxyz.IsZero()||pNodeLoad->permanentFz!=0))
			{
				pNode->feature=1;	//���к��ؼ��ص���Ϊ�����ڵ�
				break;
			}
		}

		for(CWindSegment* pSeg=pTower->WindLoadSeg.GetFirst();pSeg;pSeg=pTower->WindLoadSeg.GetNext())
		{
			for(int i=0;i<pSeg->GetMaxNodeCount();i++)
			{
				if(pNode->handle==pSeg->nodeArr[i])
				{
					pNode->feature=1;	//��ѹ���������ص���Ϊ�����ڵ�
					break;
				}
			}
			if(pNode->feature==1)	//��ѹ���������ص���Ϊ�����ڵ�
				break;
		}
		for(CAttachWindLoad* pAttachWindLoad=pTower->AttachWindLoad.GetFirst();pAttachWindLoad;pAttachWindLoad=pTower->AttachWindLoad.GetNext())
		{
			for(long *pNodeH=pAttachWindLoad->GetNodeList()->GetFirst();pNodeH;pNodeH=pAttachWindLoad->GetNodeList()->GetNext())
			{
				if(pNode->handle==*pNodeH)
				{
					pNode->feature=1;	//���ӷ�ѹ���������ص���Ϊ�����ڵ�
					break;
				}
			}
			if(pNode->feature==1)	//��ѹ���������ص���Ϊ�����ڵ�
				break;
		}
	}
	if(!hasSwingNode)
		return;	//�ްڶ��ڵ�
	for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
	{
		if(pNode->force_type!=SWING_NODE)
			continue;	//�ǰڶ��ڵ�
		CLDSLinePart *pRod=NULL,*pRod1=NULL,*pRod2=NULL;
		for(pRod=pTower->EnumRodFirst();pRod;pRod=pTower->EnumRodNext())
		{//result.allrodset�в������ڶ��ڵ�Ĺ����˼�,��ֻ��ʹ��m_pTower->Parts���� wjh-2017.3.26
			CLDSLinePart* pConnRod=NULL;
			if(pRod->pStart==pNode||pRod->pEnd==pNode)
				pConnRod=pRod;
			if(pConnRod&&pRod1==NULL)
				pRod1=pRod;
			else if(pConnRod&&pRod2==NULL)
				pRod2=pRod;
			else if(pRod1!=NULL&&pRod2!=NULL&&pConnRod)
			{
				logerr.Log("0X%X�ڵ�ڶ��ڵ�����˼�������2, ���Ⱥ˲���ٽ�����ѧ����!",pNode->handle);
				break;
			}
		}
		if(pRod1!=NULL)
			(pRod1->pStart==pNode) ? pRod1->pEnd->feature=1 : pRod1->pStart->feature=1;
		if(pRod2!=NULL)
			(pRod2->pStart==pNode) ? pRod2->pEnd->feature=1 : pRod2->pStart->feature=1;
	}
}
//���ؿ��ǰ��ƶ�ЧӦʱ���¶���
int CPreProcessor::GetEiffelSlopes(bool bOnlyValid/*=true*/,CWorkStatus* pVerifyStatus/*=NULL*/)
{
	char cAxis=0;
	if(pVerifyStatus)
	{
		if(pVerifyStatus->m_cMainLoadType==0&&pVerifyStatus->WindLoad.azimuth%180==0)
			cAxis='Y';	//Y�����
		else if(pVerifyStatus->m_cMainLoadType==0&&pVerifyStatus->WindLoad.azimuth%90==0)
			cAxis='X';	//X�����
		else
			return 0;	//��Ч����ǻ�Ǵ�繤��
	}
	if( bOnlyValid&&(
		(cAxis==0&&!m_bIncEiffelFactorX&&!m_bIncEiffelFactorY)||
		(cAxis=='X'&&!m_bIncEiffelFactorX)||
		(cAxis=='Y'&&!m_bIncEiffelFactorY)))
		return 0;
	else
		return (int)jointx_z_arr.GetSize();
}
//TODO:(2)[���ؼ���ģ�͵ļ��ٻ���]��PrepareTower�п��ټ̳й����������ģ�����ȹ̲�����Ľṹ��Ϣ
bool CPreProcessor::InheritStableModelInfo(CBuffer& buffer)
{
	return false;
	//1.���µ�Ԫ�Գ���Ϣ
		//CHashList<RODMIR_INFO>hashRodMirInfo;
		//���µ�Ԫ�Ľ������Գ���Ϣ
		//RetrieveFatherElemHashSet();	//��ȡ����Ԫ���Ա�򻯼��㵥Ԫ�Ƿ����㹻֧��
	//2.�Ľ�GetBodyLoad
	//3.�Ľ�InitElemLamdaInfo(loadSrcDesignElemInfo);
	//4.�Զ������������ص㼰��ѹ�������ص��ж�Ϊ�����ڵ�
		//SmartJustifyNodeForceType
	//5.���Ӻ���-�������ؼ���ʩ�ӻ���
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
	//У���Ƿ���Ҫ��������б�ĵİ��ƶ�ЧӦ
	m_bIncEiffelFactorX=m_bIncEiffelFactorY=ActiveDesignCode->IsIncEiffelEffect();
	VerifyEiffelFactor(pTower);
	pTurboBuffer=Ta.AttachBuffer.GetValue(CTower::TSA_TURBO_MODEL);
	//���ԭ������
	Empty();
	if(InheritStableModelInfo(*pTurboBuffer))
		return true;
	hashTowerNodeI.CreateHashTable(pTower->Node.GetNodeNum());
	//һ���������ܴ����ڵ㼯��
	for(pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
	{
		pNode->UnifiedCfgword();
		if(pNode->layer(0)=='L'&&cfgwordLeg.And(pNode->cfgword))
			result.allnodeset.append(pNode);
		else if(cfgwordBody.And(pNode->cfgword))
			result.allnodeset.append(pNode);
		hashTowerNodeI.SetValueAt(pNode->point_i,pNode);
		if(pNode->force_type==AUTO_NODE)
			pNode->feature=0;	//��Ϊ�Ǵ����ڵ㣨�������Ķ˵㣩
		else if(pNode->force_type==FORCE_NODE)
			pNode->feature=1;	//�ֶ��趨�Ĵ����ڵ�
		else //if(pNode->force_type==SWING_NODE)
			pNode->feature=0;	//�ڶ��ڵ��ϵĺ��ر���Ч������ڵ��Ϻ󽫱�����
	}
	for(pLinePart=pTower->Parts.GetFirstLinePart();pLinePart;pLinePart=pTower->Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
			continue;
		if(pLinePart->pStart->force_type==SWING_NODE||pLinePart->pEnd->force_type==SWING_NODE)
			continue;	//��ڶ��ڵ������ĸ˼�Ϊ�ڶ��˼�,��������
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//��ϽǸ��е��ӽǸ֣�����ϽǸ�����ѧ����ʱ����������Ϊ��Ӧ�����ظ�����
#endif
		pLinePart->UnifiedCfgword();
		if(pLinePart->layer(0)=='L'&&cfgwordLeg.And(pLinePart->cfgword))
			result.allrodset.append(pLinePart);
		else if(cfgwordBody.And(pLinePart->cfgword))
			result.allrodset.append(pLinePart);
		if(pLinePart->cMaterial==0)
			throw "���ڸ˼�ȱ�ٲ�����Ϣ,���������������У��!";
		if(pLinePart->IsAuxPole())
			continue;
		if(pLinePart->pStart) 
			pLinePart->pStart->feature=1;
		if(pLinePart->pEnd) 
			pLinePart->pEnd->feature=1;
	}
	SmartJustifyNodeForceType(pTower,result.allnodeset);//�м��������ѹ�������ص㼴Ϊ�����ڵ�
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
				logerr.Log("�ڵ�0X%X��δ���кϷ����(%d)",nodeArr[i]->handle,nodeArr[i]->point_i);
#endif
				passNodeICheck=false;
			}
			else if(hashPointI.GetValue(nodeArr[i]->point_i)!=NULL)
			{
				pNode=hashPointI[nodeArr[i]->point_i];
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("node 0X%X and node 0X%X share the same number(%d),serial number is not reasonable!",pNode->handle,nodeArr[i]->handle,pNode->point_i);
#else
				logerr.Log("�ڵ�0X%X��ڵ�0X%X����ͬ�ı��(%d)����Ų�����",pNode->handle,nodeArr[i]->handle,pNode->point_i);
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
	//����ڵ�����ع�ϣ��洢�ռ�
	result.hashNodeBodyLoad.CreateHashTable(result.nodeSet.GetNodeNum());
	//����������ָ˼�Ϊ��Ԫ����
	//�������нϳ��˼���Ҫ�м�ֶε�������д���,�������£�
	//1.��һ���˼��������������ڵ���һ������һ��Ӧ��������Ԫ��
	//2.�Բ����Ĳ����з����Թ�������Ӧ��������Ԫ
	for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
	{
		//if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
		//	continue;
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
#endif
		//if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&pLinePart->hGr
		if(pLinePart->IsAuxPole())	//������
		{
			pElem=result.elemList.append();
			pElem->cfgword=pLinePart->cfgword;
			if(pLinePart->size_idClassType==CLS_CABLE)
			{	//���ߵ�Ԫ
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
		else								//������
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
				{	//����
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
					if(pLinePart->Layer(0)=='L')	//���ȹ���
						pLinePart->cfgword.GetLegScopeStr(strRod,50,true);
					else
						pLinePart->cfgword.GetBodyScopeStr(strRod);
					if(pLinePart->pStart->Layer(0)=='L')	//���ȹ���
						pLinePart->pStart->cfgword.GetLegScopeStr(strStart,50,true);
					else
						pLinePart->pStart->cfgword.GetBodyScopeStr(strStart);
					if(pLinePart->pEnd->Layer(0)=='L')	//���ȹ���
						pLinePart->pEnd->cfgword.GetLegScopeStr(strEnd,50,true);
					else
						pLinePart->pEnd->cfgword.GetBodyScopeStr(strEnd);
					throw (char*)CXhChar50("0X%X�˼��˽ڵ���ĺ�('%s' -> '%s')��˼�������ĺ�(%s)��һ�£�",pLinePart->handle,(char*)strStart,(char*)strEnd,(char*)strRod);
				}
				for(pNode=nodeset.GetFirst();pNode!=NULL&&pNode!=pLinePart->pEnd;pNode=nodeset.GetNext())
				{
					if(pNode->feature==0)
						continue;	//���Ǵ����ڵ�
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
						//�жϸ˵�Ԫ���˵���������
						if(pElem->pStart==pLinePart->pStart)
							pElem->start_joint_type=pLinePart->start_joint_type;
						else
							pElem->start_joint_type=JOINT_RIGID;
						if(pElem->pEnd==pLinePart->pEnd)
							pElem->end_joint_type=pLinePart->end_joint_type;
						else
							pElem->end_joint_type=JOINT_RIGID;
						//�жϸ˵�Ԫ���˵���������
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
							throw (char*)CXhChar50("��Ԫ%d-%dʼĩ�˽ڵ�λ���غϣ���������",pElem->pStart->point_i,pElem->pEnd->point_i);
					}
					if(pElem->pEnd==NULL)
						result.elemList.DeleteCursor(TRUE);
					if(bPush)
						nodeset.pop_stack();
				}
			}
			else	//���Ϸ��ĸ˼�(һ���˼�����Ӧ�������˽ڵ��ڸ˼���)
				throw (char*)CXhChar50("0X%X�˼�������һ���˽ڵ㲻�ڵ�ǰģ����",pLinePart->handle);
		}
	}
	UpdateElemLengthInfo();	//���µ�Ԫ������Ϣ
	for(pElem=result.elemList.GetFirst(),i=1;pElem;pElem=result.elemList.GetNext(),i++)
		pElem->id=i;
	result.InitNodeBodyLoads();	//��ʼ���°�������㷨, ��������CElement::id��ֵ�Ժ�
	result.hashElemStartBodyLoad.CreateHashTable(result.elemList.GetNodeNum());
	result.hashElemEndBodyLoad.CreateHashTable(result.elemList.GetNodeNum());
	if(bApplyLoad)
	{
		result.hashElemsMap.Empty();
		result.hashElem.CreateHashTable(result.elemList.GetNodeNum());
		//��ֵ��Ԫ��ʶ�����½ڵ����ӵ�Ԫ��Ϣ
		ELEMLATERAL_STIFF lateralStiff;
		for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{
			result.hashElem.SetValueAt(pElem->id,pElem);
			result.hashElemsMap.SetValue(pElem->pStart->handle,pElem->pEnd->handle,pElem);
			lateralStiff.pElem=pElem;
			lateralStiff.stiff=pElem->GetWorkI()/pElem->Length();
			//����ʼ�˽ڵ����ӵ�Ԫ��Ϣ
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
			{	//���ڹ�����ĺţ���Ŀǰ�в�����ά����֧��
				if(pNodeLinks->supportType||pNodeLinks->supportDirect.IsZero())
				{
					pNodeLinks->supportType=1;
					pNodeLinks->supportDirect=pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==1&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//������֧������Ϊƽ��֧��
					pNodeLinks->supportType=2;
					pNodeLinks->supportDirect=pNodeLinks->supportDirect^pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==2&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//��ƽ��֧������Ϊ��ά����֧��
					pNodeLinks->supportType=3;
				}
			}
			//�����ն˽ڵ����ӵ�Ԫ��Ϣ
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
			{	//���ڹ�����ĺţ���Ŀǰ�в�����ά����֧��
				if(pNodeLinks->supportType||pNodeLinks->supportDirect.IsZero())
				{
					pNodeLinks->supportType=1;
					pNodeLinks->supportDirect=pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==1&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//������֧������Ϊƽ��֧��
					pNodeLinks->supportType=2;
					pNodeLinks->supportDirect=pNodeLinks->supportDirect^pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==2&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//��ƽ��֧������Ϊ��ά����֧��
					pNodeLinks->supportType=3;
				}
			}
		}
		//���¸����Ķ˽ڵ�λ�������ĵ�Ԫ�м�Ľڵ��������
		for(pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
		{
			if(pNode->feature==1)	//�����Ľڵ�
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
		//���µ�Ԫ�Գ���Ϣ
		CHashList<RODMIR_INFO>hashRodMirInfo;
		//TODO:(3)[���µ�Ԫ�Գ���Ϣ]����Ӱ�����Ч�ʡ�wjh-2016.2.25
//#ifdef dkslkd
		for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
		{
#ifndef __TSA_
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
				continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
#endif
			RODMIR_INFO *pMirInfo=NULL,*pMirXInfo=NULL,*pMirYInfo=NULL,*pMirZInfo=NULL;
			pMirInfo=hashRodMirInfo.GetValue(pLinePart->handle);
			if(pMirInfo!=NULL)
				continue;
			CLdsPartListStack stack(pTower);
			CLDSLinePart *pMirXRod=pLinePart->GetMirRod(MIRMSG(1));
			/*if(pMirXRod==pLinePart)
				pMirXRod=NULL;	//�����X�������߸˼�X��Գ����Լ����˴����ܸ��գ�����ᵼ�µ�Ԫ�Գ���Ϣ����ȱʧ wjh-2013.5.1
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
				pMirYRod=NULL;	//�����Y�������߸˼�X��Գ����Լ�
			else */
			if(pMirYRod==pMirXRod)
				pMirYRod=NULL;	//�M���潻��ļ���X�Գ�����Y�Գ�
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
				pMirZRod=NULL;	//�����Խ��߸˼�X��Գ����Լ�
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
			pLinePart->feature%=10;	//��մ�����
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
				continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
#endif
			if(pLinePart->feature/10!=0)
				continue;	//�Ѵ���

			ELEM_PTR* ppElem;
			RODMIR_INFO *pMirInfo=hashRodMirInfo.GetValue(pLinePart->handle);
			if(pMirInfo==NULL)
			{	//������ԳƵĸ˼�
				pRodIntervals=result.hashRodIntervals.Add(pLinePart->handle);
				for(ppElem=pRodIntervals->listElems.GetFirst();ppElem;ppElem=pRodIntervals->listElems.GetNext())
					(*ppElem)->relaElem.bFatherElem=true;
				continue;	//�޶Գ���Ϣ
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
				pMirYRod=NULL;	//�M���潻��ļ���X�Գ�����Y�Գ�
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
						continue;	//��Ԫ�ԶԳ�
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_x[j]->pStart->Position(false),elem_arr_x[j]->pEnd->Position(false),1))
					{	//X��Գ�
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
						continue;	//��Ԫ�ԶԳ�
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_y[j]->pStart->Position(false),elem_arr_y[j]->pEnd->Position(false),2))
					{	//Y��Գ�
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
						continue;	//��Ԫ�ԶԳ�
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_z[j]->pStart->Position(false),elem_arr_z[j]->pEnd->Position(false),3))
					/*	fabs(elem_arr[i]->pStart->pos.x+elem_arr_z[j]->pStart->pos.x)<EPS&&
						fabs(elem_arr[i]->pStart->pos.y+elem_arr_z[j]->pStart->pos.y)<EPS&&
						fabs(elem_arr[i]->pStart->pos.z-elem_arr_z[j]->pStart->pos.z)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.x+elem_arr_z[j]->pEnd->pos.x)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.y+elem_arr_z[j]->pEnd->pos.y)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.z-elem_arr_z[j]->pEnd->pos.z)<EPS)*/
					{	//Z��Գ�
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
						elem_arr[i]->relaElem.idElemMirZ=0;//����Z��Գ�����X��Y��Գ�ʱ��X��Y��Գ�������ʾ�Գƣ��Ҳ����ظ��Գ�
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
						pMirXElem->relaElem.idElemMirZ=0;//����Z��Գ�����X��Y��Գ�ʱ��X��Y��Գ�������ʾ�Գƣ��Ҳ����ظ��Գ�
					pMirXElem->relaElem.bFatherElem=false;
				}
				if(pMirYElem&&elem_arr[i]->relaElem.idElemMirY>0)
				{
					pMirYElem->relaElem.idElemMirX=elem_arr[i]->relaElem.idElemMirZ;
					pMirYElem->relaElem.idElemMirZ=elem_arr[i]->relaElem.idElemMirX;
					if( pMirYElem->relaElem.idElemMirZ==pMirYElem->relaElem.idElemMirX||
						pMirYElem->relaElem.idElemMirZ==pMirYElem->relaElem.idElemMirY)
						pMirYElem->relaElem.idElemMirZ=0;//����Z��Գ�����X��Y��Գ�ʱ��X��Y��Գ�������ʾ�Գƣ��Ҳ����ظ��Գ�
					pMirYElem->relaElem.bFatherElem=false;
				}
				if(pMirZElem&&elem_arr[i]->relaElem.idElemMirZ>0)
				{
					pMirZElem->relaElem.idElemMirX=elem_arr[i]->relaElem.idElemMirY;
					pMirZElem->relaElem.idElemMirY=elem_arr[i]->relaElem.idElemMirX;
					if( pMirZElem->relaElem.idElemMirZ==pMirZElem->relaElem.idElemMirX||
						pMirZElem->relaElem.idElemMirZ==pMirZElem->relaElem.idElemMirY)
						pMirZElem->relaElem.idElemMirZ=0;//����Z��Գ�����X��Y��Գ�ʱ��X��Y��Գ�������ʾ�Գƣ��Ҳ����ظ��Գ�
					pMirZElem->relaElem.bFatherElem=false;
				}
				if(pFatherElem)
					pFatherElem->relaElem.bFatherElem=true;
				else	//������Ԫ(����������X/Y��ԳƵ��м���)
					elem_arr[i]->relaElem.bFatherElem=true;
			}
		}
		InitElemLamdaInfo(loadSrcDesignElemInfo);
		RetrieveFatherElemHashSet();	//��ȡ����Ԫ���Ա�򻯼��㵥Ԫ�Ƿ����㹻֧��
		CalAuxElemBraceForcePreInfo();	//���㸨�ĵ�Ԫ��֧����ϵ��
		if(ActiveDesignCode->GetBraceCoefOfPrimaryToPost()>0)
			CalPrimaryElemBraceForcePreInfo();	//����б�ĵ�Ԫ��֧����ϵ��
		//������ʼ�����ع�����ϣ���Ա�����ѯ
		CWorkStatus *pStatus;
		CHashTable<CWorkStatus*>statusHashTbl;
		statusHashTbl.CreateHashTable(Ta.WorkStatus.GetNodeNum());
		for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			if(pStatus->feature==0)	//����˯��״̬�ֲ�����˼��㹤��
				continue;
			double W0=pStatus->WindLoad.velocity*pStatus->WindLoad.velocity/1.6;
			statusHashTbl.SetValueAt(pStatus->handle,pStatus);
			double sina=sin(RADTODEG_COEF*pStatus->WindLoad.azimuth);
			double cosa=cos(RADTODEG_COEF*pStatus->WindLoad.azimuth);
			for(CWorkStatus::WINDLOAD_REFPARAM *pRefParam=pStatus->AttachWindLoad.GetFirst();pRefParam;pRefParam=pStatus->AttachWindLoad.GetNext())
			{
				CAttachWindLoad *pWindLoad=pTower->AttachWindLoad.FromHandle(pStatus->AttachWindLoad.GetCursorKey());
				if(pWindLoad==NULL)	//�Է����ݿ��д˺�����ɾ������������δ�ܼ�ʱ����2005/01/17 WJH
					continue;
				double node_wind_area=pWindLoad->m_fShieldArea/pWindLoad->GetNodeList()->GetNodeNum();
				pRefParam->attachMass=pWindLoad->m_fAttachWeight/pWindLoad->GetNodeList()->GetNodeNum();
				double Wk=pRefParam->shieldCoef*pWindLoad->m_fProfileCoef*pWindLoad->m_fBetaZ*node_wind_area*pWindLoad->m_fShieldCoef*W0;
				pRefParam->basicAttachWindLoadF.x=Wk*sina;
				pRefParam->basicAttachWindLoadF.y=Wk*cosa;
			}
		}
		//�ġ����³�ϸ�ȹ�������ͬʱ��ѹʱ��ϸ�ȼ����������Ĺ��ò��ֹ�����Ԫ��Ϣ
		for(CLDSLinePart *pRod=result.allrodset.GetFirst();pRod;pRod=result.allrodset.GetNext())
		{
#ifndef __TSA_
			if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0x20)
				continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
#endif
			CRodIncIntervals *pRodInfo=NULL,*pRefRodInfo=NULL;
			if(pRod->CalLenCoef.hRefPole>0x20)
				pRefRodInfo=result.hashRodIntervals.GetValue(pRod->CalLenCoef.hRefPole);
			if(pRefRodInfo)
				pRodInfo=result.hashRodIntervals.GetValue(pRod->handle);
			if(pRodInfo&&pRefRodInfo&&pRodInfo->listElems.GetNodeNum()==1&&pRefRodInfo->listElems.GetNodeNum()==1)
			{	//ֻ��һ����Ԫʱ�����˼��뵥Ԫ���غϵģ����˼����ò�ּ��Ǽ��㵥Ԫ
				pElem=*pRodInfo->listElems.GetFirst();
				CElement *pRefElem=*pRefRodInfo->listElems.GetFirst();
				if(pElem->cfgword.And(pRefElem->cfgword))
				{	//������ͬʱ���ֵĿ��ܴ���
					pElem->relaElem.idRefElem=pRefElem->id;
					pRefElem->relaElem.idRefElem=pElem->id;
				}
			}
		}
	}
	return true;
}
//</DEVELOP_PROCESS_MARK>
	//���ε��û����ģ��(������)�����仯ʱ���ø�������Ԫ����ģ����Ϣ
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
			pElem=m_pAnalysisProcessor->hashElems.SetValue(pElem->id,*pElem);	//�����õ�Ԫ��Ϣ���
			if(!pElem->pOrgPole->IsAuxPole())	//�����Ķ˽ڵ���1��ʶ
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
			//���㸳ֵ�ڵ��ϳ����ӵ�Ԫ��ĸ��������������ڶ������Է�������
			pFemNode->attachMass=pNode->attachMass+pNode->PermanentLoad.z/9.8;	//���Ǹ���������������Ӧ���Ǹ����������⡡wjh-2011.9.14
			pFemNode->pTag=NULL;
// 			pFemNode->Fxyz=?	//���ڴ˴�ȱ�ٹ�����Ϣ��Ӧ�ں������̴����и�ֵ
		}
	}
	for(pElem=m_pAnalysisProcessor->hashElems.GetFirst();pElem;pElem=m_pAnalysisProcessor->hashElems.GetNext())
	{
		//����Ԫ����ģ��ר�õ�Ԫ��Ϣ���
		if(pElem->pStartBraceForceElem!=NULL)	//��ǰ�������еĵ�Ԫ����Ϊʼ��֧�������ĵ�ԪΪ�����������еĵ�Ԫ
			pElem->pStartBraceForceElem=m_pAnalysisProcessor->hashElems.GetValue(pElem->pStartBraceForceElem->id);
		if(pElem->pEndBraceForceElem!=NULL)		//��ǰ�������еĵ�Ԫ����Ϊ�ն�֧�������ĵ�ԪΪ�����������еĵ�Ԫ
			pElem->pEndBraceForceElem=m_pAnalysisProcessor->hashElems.GetValue(pElem->pEndBraceForceElem->id);
		if(pElem->pOrgPole->IsAuxPole())
			continue;	//�����ģ�ֻ����ͳ��������
		if(pElem->BearTens1Comp2==1||pElem->BearTens1Comp2==2)
			continue;						//ʧЧ����ѹ��
		FEM_ELEMENT *pFemElem=m_pAnalysisProcessor->femElemList.append();
		pFemElem->pTag=NULL;
		pFemElem->idElem=pElem->id;
		pElem->pFemElem=pFemElem;
		if(!femNodeHashTbl.GetValueAt(pElem->pStart->handle,pFemElem->pStart))
		{
			char sError[100]="";
			sprintf(sError,"0X%X(%d)-0X%X(%d)��Ԫʼ�˽ڵ��뵥Ԫ��ĺŲ�һ�£����ݲ�������",
				pElem->pStart->handle,pElem->pStart->point_i,pElem->pEnd->handle,pElem->pEnd->point_i);
			throw sError;
		}
		if(!femNodeHashTbl.GetValueAt(pElem->pEnd->handle,pFemElem->pEnd))
		{
			char sError[100]="";
			sprintf(sError,"0X%X(%d)-0X%X(%d)��Ԫ�ն˽ڵ��뵥Ԫ��ĺŲ�һ�£����ݲ�������",
				pElem->pStart->handle,pElem->pStart->point_i,pElem->pEnd->handle,pElem->pEnd->point_i);
			throw sError;
		}
		pFemElem->start_i=pFemElem->pStart->index;
		pFemElem->end_i=pFemElem->pEnd->index;
		pFemElem->density=7.85e-6*pElem->pOrgPole->m_fWeightRaiseCoef;	//�ֲ��ܶ�7.85e-6kg/mm3
		pFemElem->elem_type=pElem->elem_type;
		pFemElem->A=pElem->pOrgPole->GetArea();
		pFemElem->E=pElem->pOrgPole->GetElasticityModulus();
		pFemElem->G=pFemElem->E/2.6;
		pFemElem->Iz=pFemElem->Ip=pElem->pOrgPole->GetWorkI()*g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef;	//��ƽ����뾶���м���
		if(pElem->elem_type==1)
		{
			pFemElem->T0=pElem->T0;	//���ߵ�ԪԤ����
			pFemElem->A=pElem->pOrgPole->GetArea();
			pFemElem->bAnchorEnd=pElem->bAnchorEnd;
			pFemElem->E=pElem->pOrgPole->GetWireElasticCoef();
			pFemElem->density=pElem->pOrgPole->GetWireUnitWeight()/(pElem->pOrgPole->GetWireArea()*1e6);
		}
		else
			pFemElem->T0=0;
	}
}
//��������ʱ�ĵ�����ؼ���
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
	//����������
	int iMode=FindAdaptNo(pStatus->vibration_mode_str);
	long i,nNodeNum=Ta.ReadVibraNodeMass(fp,NULL);
	if(nNodeNum==0)	//fp==NULLʱnNodeNum=0
		return false;
	DYNAMIC_ANAL_NODE *node_arr=new DYNAMIC_ANAL_NODE[nNodeNum];
	double *m_arr=new double[nNodeNum];
	double *x_arr=new double[nNodeNum];
	double *f_arr=new double[nNodeNum];
	Ta.ReadVibraNodeMass(fp,node_arr);
	for(i=0;i<nNodeNum;i++)
	{
		node_arr[i].id=(DWORD)Ta.Node.FromHandle(node_arr[i].id);	//�ɽڵ���ת��Ϊ�ڵ�ָ��
		m_arr[i]=node_arr[i].mass;
	}
	//��ȡ�׸���������
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
	if(pStatus->m_cMainLoadType!=8)	//��Ҫ����ˮƽ�������
	{
		while(bRead)
		{
			memset(f_arr,0,8*nNodeNum);	//����
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
			//����������͵����Ͳ�Ԥϵ��
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
			//��ȡ��һ����������
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
	if(pStatus->m_cMainLoadType!=7)	//��Ҫ������ֱ�������
	{
		memset(f_arr,0,8*nNodeNum);	//����
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
//iCase��<=(slope_n-1)*2
double CalEiffelFactor(double* joint_z_arr,double slope_n,double z,UINT iCase)
{
	if(iCase==0)
		return 1.0;	//�޷�ѹ�ۼ�����
	int slope_i=(iCase-1)/2+1;
	if(z<joint_z_arr[slope_i])
	{	//��ѹ��������¶ν���֮��
		if(slope_i%2==1)	//�ϴ���С,�²��ۼ�
			return 1.0;
		else				//��С�´�,�ϲ��ۼ�
			return CTower::W0_EIFFEL_FACTOR;
	}
	else
	{	//��ѹ��������¶ν���֮��
		if(slope_i%2==1)	//�ϴ���С,�²��ۼ�
			return CTower::W0_EIFFEL_FACTOR;
		else				//��С�´�,�ϲ��ۼ�
			return 1.0;
	}
}
//�����ѹ�εĳ��ܷ�ѹ����
void CPreProcessor::CalWindAndAttachLoad(CTower *pTower,CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode,
								bool bForFoundation/*=false*/,double magnifyCoef/*=1.0*/,WINDSEGEXPORT_LIST* pExportList/*=NULL*/)
{
	double v=pStatus->WindLoad.velocity;
	double W0=v*v/1.6;	// N/m2
	double lowest_z=pModule->LowestZ();	//����Z����
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
		int iStyle=pWindSeg->cCalStyle;	//0.����ʽ����1.X����ˮƽ�ᵣ2.Y����ˮƽ�ᵣ
		int iMirMsg=pWindSeg->cMirType;	//�Գ���Ϣ0.�޶Գ�1.���ҶԳ�
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
		if(!bValidSeg)	//�Ǳ�ģ�ͻ����ߵķ�ѹ��
			continue;
		//б�İ��ƶ�ЧӦ
		CWindSegElemPicker *pSegElemPicker=NULL;
		if(pWindSeg->pFrontElemPicker)
			pSegElemPicker=(CWindSegElemPicker*)pWindSeg->pFrontElemPicker;
		else
			pSegElemPicker=(CWindSegElemPicker*)pWindSeg->pSideElemPicker;
		int iEiffelCase=pStatus->GetEiffelCaseNo();
		if(	iEiffelCase>0&&pSegElemPicker!=NULL)
		{
			double eiffel_factor=1.0;
			//���۷�ѹ�ϴ���С(iEiffelCase%2==1)������С�´�(iEiffelCase%2==0),��ȡ��ѹ���²�Z
			if(m_bIncEiffelFactorX&&pStatus->WindLoad.azimuth%180==0)	//90��X���繤��joint_z_arr)
			{
				eiffel_factor=CalEiffelFactor(jointx_z_arr.m_pData,jointx_z_arr.GetSize(),pSegElemPicker->MaxZ(),iEiffelCase);
				W0*=eiffel_factor;	//���밣�ƶ�ЧӦ����ѹ�ۼ�ϵ��
			}
			else if(m_bIncEiffelFactorY&&pStatus->WindLoad.azimuth%90==0)	//0��Y���繤��joint_z_arr)
			{
				eiffel_factor=CalEiffelFactor(jointy_z_arr.m_pData,jointy_z_arr.GetSize(),pSegElemPicker->MaxZ(),iEiffelCase);
				W0*=eiffel_factor;	//���밣�ƶ�ЧӦ����ѹ�ۼ�ϵ��
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
					high_pos=(lowest_z-pSegElemPicker->geomCenter.z)/1000+pModule->land_high;	//�����׼����ƽ����
					if(pWindSeg->m_bAutoCalProfileCoef)
						pWindSeg->AfFront=pSegElemPicker->m_fPrjAreaCoef*pSegElemPicker->CalWindSegAf(W0,iLoadExecuteCode,&pWindSeg->m_fProfileModeCoef);
					else
						pWindSeg->AfFront=pSegElemPicker->m_fPrjAreaCoef*pSegElemPicker->CalWindSegAf(W0,iLoadExecuteCode,NULL);
					if(pSegElemPicker->m_fWrapArea<EPS)
						throw "��ѹ�ε������Ϊ�㣬��ѹ����Ϣ����";
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
						throw "��ѹ�ε������Ϊ�㣬��ѹ����Ϣ����";
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
		double mu_z=Find_MuZ(high_pos,pTower->m_cLandscape);	//���ҷ�ѹ�߶ȱ仯ϵ��
		double Xcoef_x,Xcoef_y,Ycoef_x,Ycoef_y;
		double betaZ=pWindSeg->BetaZ;
		if(bForFoundation)
			betaZ=pWindSeg->BetaZForFoundation;
		/*else */if(magnifyCoef>0)	//��ǰ��else����������Ϊ����ʱ��ҲӦ���Ƿ���ظ�������ϵ��
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
		{	//����������
			mu_sb = Find_MuS(pWindSeg->FaiFront,0);
			Wsx=Wsy=W0*mu_z*pWindSeg->K*mu_sb*betaZ*pWindSeg->AfFront*pWindSeg->CqiFront;//*pWindSeg->case_coef;
			Xcoef_x=sin(pStatus->WindLoad.azimuth*RADTODEG_COEF);
			Ycoef_x=cos(pStatus->WindLoad.azimuth*RADTODEG_COEF);
			Xcoef_y=Ycoef_y=0;
		}
		if(pWindSeg->cCalStyle!=0||pWindSeg->AfSide<EPS)	//�ᵣ�λ������޷�����ʱ
		{
			Wx=Xcoef_x*Wsx+Xcoef_y*Wsy;
			Wy=Ycoef_x*Wsx+Ycoef_y*Wsy;
		}
		else
		{	//����б������
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
			pExport->Wsa=Wsx/1000;	//���洹ֱ�紵�����
			pExport->Wsb=Wsy/1000;	//���洹ֱ�紵�����
			if(pWindSeg->cCalStyle==1)
			{
				pExport->Wsc=Wsy/1000;	//X��ᵣ��ֱ�紵�����
				pExport->Xcoef_Wsc=Xcoef_y;
				pExport->Ycoef_Wsc=Ycoef_y;
			}
			else if(pWindSeg->cCalStyle==2)
			{
				pExport->Wsc=Wsx/1000;	//Y��ᵣ��ֱ�紵�����
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
		if(pWindSeg->cType==0&&nValidNode>=2)	//�˵���������
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
		else if(pWindSeg->cType==1&&nValidNode>=3)	//�Զ�������
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
				if(pWindSeg->cCalStyle==1)	//X����ᵣ
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
						mirmsg.axis_flag=0x02;	//Y��ԳƷ�ѹ��
					else if(pWindSeg->cMirType==2)
						mirmsg.axis_flag=0x01;	//X��ԳƷ�ѹ��
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
		else if(pWindSeg->cType==2&&nValidNode==4)	//����������������
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
				logerr.Log("%d�Žڵ��ڽ���%d#�ŷ�ѹ�η���ؼ���ʱ������X�����{%.0fkN}�쳣�����ʵ!",pWindSeg->nodePtrArr[i]->point_i,pWindSeg->nodePtrArr[i]->WindLoad.x*0.001);
			if(pWindSeg->nodePtrArr[i]&&fabs(pWindSeg->nodePtrArr[i]->WindLoad.y)>10000000)
				logerr.Log("%d�Žڵ��ڽ���%d#�ŷ�ѹ�η���ؼ���ʱ������Y�����{%.0fkN}�쳣�����ʵ!",pWindSeg->nodePtrArr[i]->point_i,pWindSeg->nodePtrArr[i]->WindLoad.y*0.001);
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
				double mu_z=Find_MuZ(high_pos,pTower->m_cLandscape);	//���ҷ�ѹ�߶ȱ仯ϵ��
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
		//һ����ø���ڵ��Ч����
			//1.�빤����صļ��������(���ɱ���ؼ�����־���������)
		CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
			//2.������������أ������������أ�
		double selfIceWeightLoad=0;								//��������������(N)
		f3dPoint selfWeightLoad(0,0,pNode->bodyLoad.selfWeightLoad);	//�����������غ���
		if(pStatus->ice_thick>0)
		{	//�ظ����淶��12.0.10�ڹ涨�����������������ز�ȡԭ������������ϵ������ wjh-2013.12.30
			if(CPreProcessor::ActiveDesignCode->CodeSerial()==CODES::GB50135_2006)
				selfIceWeightLoad=pNode->bodyLoad.iceWeightLoadCoef*pStatus->ice_thick+pNode->bodyLoad.iceWeightLoadCoef2*ice_thick_2;
			else
				selfIceWeightLoad=pNode->bodyLoad.selfWeightLoad*(g_sysPara.IceLoadCoefInterpByThick(pStatus->ice_thick)-1);
		}
		//����������Ԫ�ڵ�ʩ�Ӻ���
		f3dPoint wireLoad;
		//1.����������ú���
		f3dPoint permanentLoad=selfWeightLoad+pNode->PermanentLoad;//ֱ��ʩ���ڽڵ��ϵļ��г־������
		permanentLoad.z+=pNode->attachMass*9.8;		//�ۼӸ�����(ƽ̨�����ߵ�)��������
		if(pStatus->m_bLoadIncFactor)
		{	//ֱ��Ӧ���Ѱ�������ϵ������ӵ����߼��к���ֵ
			//2.������ܿɱ����
			if(pNodeLoad)
			{
				if(bApplyStdLoad)
					wireLoad=pNodeLoad->stdLoad;
				else
					wireLoad=pNodeLoad->designLoad;
			}
			wireLoad+=pNode->vSwingLoadFxyz;	//�ۼ��ɰڶ��ڵ��Ч��������ʱ�����ߺ��أ�ͬwireLoadһ�������˸���ϵ����
			//3.��������أ�pNode->WindLoad
			f3dPoint iceLoad(0,0,selfIceWeightLoad);	//�ɱ�����еĸ���������������
			if(bApplyStdLoad)
				pFemNode->Fxyz=wireLoad+permanentLoad+iceLoad+pNode->WindLoad;
			else
			{
				double safetyCoef=1.0;
				if(pStatus->m_cMainLoadType<4)
					safetyCoef=Ta.SafetyCoef();	//��װ���������㹤������Ҫ���ǽṹ��Ҫ��ϵ��
				pFemNode->Fxyz= wireLoad+safetyCoef*(Ta.m_fGamaGForDesign*permanentLoad+
					pStatus->Kesai_I()*pStatus->GamaQ_I()*iceLoad+
					pStatus->Kesai_W()*pStatus->GamaQ_W()*pNode->WindLoad);
			}
		}
		else
		{
			if(pNodeLoad)
				permanentLoad.z+=pNodeLoad->permanentFz;	//�ۼӵ������ϴ��������غ���
			//2.������ܿɱ����
			f3dPoint iceLoad(0,0,selfIceWeightLoad);	//�ɱ�����еĸ���������������
			if(pNodeLoad)
			{
				wireLoad=pNodeLoad->Fxyz;				//�����ߵ�����ʩ�ӿɱ����
				pNodeLoad->designLoad=Ta.m_fGamaGForDesign*permanentLoad+pStatus->Kesai()*pStatus->GamaQ()*wireLoad;
				if(pStatus->m_cMainLoadType<4)
					pNodeLoad->designLoad*=Ta.SafetyCoef();	//��װ���������㹤������Ҫ���ǽṹ��Ҫ��ϵ��
			}
			if(result.bHasSwingNode)
				wireLoad+=pNode->vSwingLoadFxyz;			//�ۼ��ɰڶ��ڵ��Ч��������ʱ�����ߺ���
			//3.��������أ�pNode->WindLoad
			if(bApplyStdLoad)
				pFemNode->Fxyz=permanentLoad+wireLoad+iceLoad+pNode->WindLoad;
			else
			{
				pFemNode->Fxyz= Ta.m_fGamaGForDesign*permanentLoad+
					pStatus->Kesai()*pStatus->GamaQ()*wireLoad+
					pStatus->Kesai_I()*pStatus->GamaQ_I()*iceLoad+
					pStatus->Kesai_W()*pStatus->GamaQ_W()*pNode->WindLoad;
				if(pStatus->m_cMainLoadType<4)
					pFemNode->Fxyz*=Ta.SafetyCoef();	//��װ���������㹤������Ҫ���ǽṹ��Ҫ��ϵ��
			}
		}
		if(pStatus->m_cMainLoadType>=5)
		{	//���ǵ����������
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
			if((lowz-highz)>0)	//*cosaPrjZ:���ڿ��ǵ�������ڣ������ͶӰ
				weight_deitax+=area*cosaPrjZ*area/(lowz-highz);
			grossArea+=area;
		}

		pPrevNode=pNode;
	}

	if(side_h<height*0.8)	//�Ҳ����㹻���ݷ�ѹ������80%��������
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
		double a=0.5*(a1+a2);	//�����ӭ������
		double b=weight_deitax/grossArea;	//��Ȩƽ�����X����
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
		if(pWindSeg->cType==0)	//�˵���������
		{
			NODESET nodeset;
			if(pWindSeg->nodePtrArr[0]==NULL||pWindSeg->nodePtrArr[1]==NULL)
				pWindSeg->AfFront=pWindSeg->AfSide=0;	//����������
			else
			{
				nodeset.append(pWindSeg->nodePtrArr[0]);
				nodeset.append(pWindSeg->nodePtrArr[1]);
				//Y��Գ�
				MIRMSG mir_y(2);
				mir_y.origin=pWindSeg->project_origin;
				pWindSeg->nodePtrArr[2]=pWindSeg->nodePtrArr[1]->GetMirNode(mir_y);
				pWindSeg->nodePtrArr[3]=pWindSeg->nodePtrArr[0]->GetMirNode(mir_y);
				if(pWindSeg->nodePtrArr[2]==NULL||pWindSeg->nodePtrArr[3]==NULL)
					pWindSeg->AfFront=pWindSeg->AfSide=0;	//����������
				else	//��������
				{
					nodeset.append(pWindSeg->nodePtrArr[2]);
					nodeset.append(pWindSeg->nodePtrArr[3]);
					pWindSeg->pFrontElemPicker=pSegPicker=windSegPickerList.append();
					pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
					pSegPicker->StartElemPicker(result.elemList,NULL);
				}
				CLDSNode *baknode[2];
				//X��Գ�
				MIRMSG mir_x(1);
				mir_x.origin=pWindSeg->project_origin;
				baknode[0]=pWindSeg->nodePtrArr[1]->GetMirNode(mir_x);
				baknode[1]=pWindSeg->nodePtrArr[0]->GetMirNode(mir_x);
				if(nodeset.GetNodeNum()>=4)
				{
					nodeset.DeleteNode(pWindSeg->nodePtrArr[3]->handle);
					nodeset.DeleteNode(pWindSeg->nodePtrArr[2]->handle);
				}
				if(baknode[0]&&baknode[1])	//��������
				{
					nodeset.append(baknode[0]);
					nodeset.append(baknode[1]);
					pWindSeg->pSideElemPicker=pSegPicker=windSegPickerList.append();
					pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
					pSegPicker->StartElemPicker(result.elemList,NULL);
				}
				else	//����������
					pWindSeg->AfSide=0;
				if(pWindSeg->cCalStyle==1)	//X��ˮƽ�ᵣ
					pWindSeg->AfSide=0;
				else if(pWindSeg->cCalStyle==2)	//Y��ˮƽ�ᵣ
					pWindSeg->AfFront=0;
			}
		}
		else if(pWindSeg->cType==1)	//�Զ����ڵ�����
		{
			NODESET nodeset;
			pWindSeg->FaiFront=pWindSeg->FaiSide=pWindSeg->B_AFront=pWindSeg->B_ASide=0;
			if(nValidNode<3)
				pWindSeg->AfFront=pWindSeg->AfSide=0;	//����������
			else
			{
				for(i=0;i<nValidNode;i++)
					nodeset.append(pWindSeg->nodePtrArr[i]);
				if(pWindSeg->cCalStyle==0||pWindSeg->cCalStyle==1)	//X����ᵣ
				{
					pWindSeg->pFrontElemPicker=pSegPicker=windSegPickerList.append();
					pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
					pSegPicker->StartElemPicker(result.elemList,NULL);
					if(pWindSeg->cCalStyle==1||nValidNode!=4)
						pWindSeg->AfSide =0.0;
					else	//���۵���ͷ��ѹ�Σ�Ҳ��Ҫ������ʽ������浲�����
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
					{	//��������㰴���������ʱҲ��Ҫ��ӵ������
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
				else// if(pWindSeg->cCalStyle==2)//Y����ᵣ
				{	
					pWindSeg->pSideElemPicker=pSegPicker=windSegPickerList.append();
					pSegPicker->InitElemPicker(nodeset,pWindSeg->project_origin);
					pSegPicker->StartElemPicker(result.elemList,NULL);
					pWindSeg->AfFront=0.0;
				}
			}
		}
		else if(pWindSeg->cType==2)	//���������������
		{
			NODESET nodeset;
			if(pWindSeg->nodePtrArr[0]==NULL||pWindSeg->nodePtrArr[1]==NULL||pWindSeg->nodePtrArr[2]==NULL||pWindSeg->nodePtrArr[3]==NULL)
				pWindSeg->AfFront=pWindSeg->AfSide=0;	//����������
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
	//A*��x+B*��y=N
	//C*��x+(D+K)*��y=0
	//���ڹ����мٶ�K>>A,B,C,D,���Ԧ�y��С�����Ժ���Ϊ0
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
	//һ���������ĵ�Ԫ����
	auxElemPtrArr.SetSize(0,3);
	for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		if( pElem->pOrgPole->IsAuxPole()&&
			pElem->pOrgPole->GetArea()>0&&pElem->pOrgPole->GetElasticityModulus()>0)
			auxElemPtrArr.append(pElem);
	}
	//���������Ǵ��������Ķ˽ڵ㼯��
	//���ڱ��㷨��������δ���ǵ�ƽ������ʱ��ֱ�淽��֧�Ÿ�����ѹ����Ŀ��� wjh-2010-11-3
	CHashList<CLDSNode*>auxNodePtrList;
	CHashList<CElement*>hashBraceForceElem;
	for(i=0;i<auxElemPtrArr.GetSize();i++)
	{
		auxElemPtrArr[i]->feature=0;	//��մ�����
		if(// auxElemPtrArr[i]->pStart->feature==0&&	//pNode->feature==0	//��ʾ�������Ķ˵�
			!auxNodePtrList.GetValue(auxElemPtrArr[i]->pStart->handle))
			auxNodePtrList.SetValue(auxElemPtrArr[i]->pStart->handle,auxElemPtrArr[i]->pStart);
		if(// auxElemPtrArr[i]->pEnd->feature==0&&
			!auxNodePtrList.GetValue(auxElemPtrArr[i]->pEnd->handle))
			auxNodePtrList.SetValue(auxElemPtrArr[i]->pEnd->handle,auxElemPtrArr[i]->pEnd);
	}
	//�������������Ķ˽ڵ�����Ӧ��֧�������ĵ�Ԫ
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
			if(pElem->pOrgPole->IsAuxPole())	//���ĵ�Ԫ�������ĵ�Ԫ
				continue;
			else if(pNode->RootFatherPartHandle()!=pElem->pOrgPole->handle)
				continue;	//���ǽڵ㸸�˼��ֲ�����ĵ�Ԫ
			//if(!pElem->cfgword.And(pNode->cfgword))	//��ڵ㸸�˼��뵥Ԫ���˼�һ�£�ͬһģ�ͼ�����ޱ�Ҫ
			double justify=pElem->CheckPoint(pNode->Position(false));
			if(justify<-EPS||justify>1+EPS)	//�㲻���߶���,�������Ķ˵��������Ķ˵���������ڸ���֧�������������������
				continue;
			else	//�ڵ��ڴ������ĵ�Ԫ�м䣬�Ҹ��˼�һ�£�Ϊ����֧�������ĵ�Ԫ
				break;
		}
		if(pElemStiff)	//�ҵ������Ķ˽ڵ�����Ӧ��֧�������ĵ�Ԫ
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
		//�ҳ��ڵ���ͬʱ�����������ģ��Ա㴦���ų������������Ĺ����������(��ʱ������֧����ϵ����Ϊ0)
		forceElemPtrTmArr.SetSize(0,8);
		CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pNode->handle);
		for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
		{
			if(pElemStiff->pElem->pOrgPole->handle!=pNode->RootFatherPartHandle()&&!pElemStiff->pElem->pOrgPole->IsAuxPole())
				forceElemPtrTmArr.append(pElemStiff->pElem);
		}
		//�����㷨���ҳ������븨�Ķ˽ڵ����ӵĸ��ĵ�Ԫ�����������֧��ԭʼ����ϵ��������������ʵ��֧��ϵ��
		bool bFinished;
		do{	//doѭ����Ŀ����Ϊ�������ýڵ����й������ϵĸ�������
			bFinished=true;
			ucs.axis_x.Set();	//���֧��������ϵ���᷽��
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
					else	//���ǵ�ǰ֧���渨���ĵ�Ԫ
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
					else	//���ǵ�ǰ֧���渨���ĵ�Ԫ
						continue;
				}
			}
			if(!bFinished)
			{
				for(i=0;i<auxElemPtrTmArr.GetSize();i++)
				{
					if(auxElemPtrTmArr[i]->pStart==pNode)
						auxElemPtrTmArr[i]->feature|=0x01;	//ʼ���Ѵ���
					else if(auxElemPtrTmArr[i]->pEnd==pNode)
						auxElemPtrTmArr[i]->feature|=0x02;	//�ն��Ѵ���
				}
				bool bHasForceSupport=false;
				for(i=0;i<forceElemPtrTmArr.GetSize();i++)
				{
					if(fabs(ucs.axis_y*forceElemPtrTmArr[i]->LenStdVec())<EPS_COS&&fabs(ucs.axis_z*forceElemPtrTmArr[i]->LenStdVec())<EPS)
					{	//�ҵ����浫�뱻֧�Ų����߲����ߵ������ĵ�Ԫ
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
							else	//�ж�����ศ��֧��ʱ��һ�㲻����֣��ݲ�֧��
							{
								if(hasSingleSecondaryBraceElem)
									logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%d�Žڵ㴦�����������ϵĶ��ศ��֧��%d-%d;%d-%d;",pNode->point_i,
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
				//else	//��������֧�ţ�����֧��ϵ��Ϊ0,�������
			}
		}while(!bFinished);
	}
}
//</DEVELOP_PROCESS_MARK>
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::CalPrimaryElemBraceForcePreInfo">
//�Ƿ�Ϊ�����Ͽ��ܵ��м�֧�Žڵ�����
static bool IsSecondaryNode(BYTE cPosCalType)
{	
	if( cPosCalType==1||cPosCalType==2||cPosCalType==3||	//ָ��X/Y/Z���������
		cPosCalType==5||				//�����ȷֵ�
		cPosCalType==6||cPosCalType==7)	//ƫ�Ƶ�
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
	//һ���������ĵ�Ԫ����
	primaryElemPtrArr.SetSize(0,3);
	for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		if( pElem->pOrgPole->GetArea()>0&&pElem->pOrgPole->GetElasticityModulus()>0&&pElem->pOrgPole->IsPrimaryRod(true))
			primaryElemPtrArr.append(pElem);
	}
	//���������Ǵ��������Ķ˽ڵ㼯��
	//���ڱ��㷨��������δ���ǵ�ƽ������ʱ��ֱ�淽��֧�Ÿ�����ѹ����Ŀ��� wjh-2010-11-3
	CHashList<CLDSNode*>primaryNodePtrList;
	CHashList<CElement*>hashBraceForceElem;
	for(i=0;i<primaryElemPtrArr.GetSize();i++)
	{
		primaryElemPtrArr[i]->feature=0;	//��մ�����
		if(!primaryNodePtrList.GetValue(primaryElemPtrArr[i]->pStart->handle)&&
			IsSecondaryNode(primaryElemPtrArr[i]->pStart->m_cPosCalType)&&
			!primaryElemPtrArr[i]->pStart->IsHasRestrict(ALL_RESTRICT))	//�����ڵ㲻����֧������
			primaryNodePtrList.SetValue(primaryElemPtrArr[i]->pStart->handle,primaryElemPtrArr[i]->pStart);
		if(!primaryNodePtrList.GetValue(primaryElemPtrArr[i]->pEnd->handle)&&
			IsSecondaryNode(primaryElemPtrArr[i]->pEnd->m_cPosCalType)&&
			!primaryElemPtrArr[i]->pEnd->IsHasRestrict(ALL_RESTRICT))	//�����ڵ㲻����֧������
			primaryNodePtrList.SetValue(primaryElemPtrArr[i]->pEnd->handle,primaryElemPtrArr[i]->pEnd);
	}
	//��������б�Ķ˽ڵ�����Ӧ��֧�����ĵ�Ԫ
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
			if(!pElem->pOrgPole->IsPostRod())	//�����ĵ�Ԫ
				continue;
			else if(pNode->RootFatherPartHandle()!=pElem->pOrgPole->handle)
				continue;	//���ǽڵ㸸�˼��ֲ�����ĵ�Ԫ
			//if(!pElem->cfgword.And(pNode->cfgword))	//��ڵ㸸�˼��뵥Ԫ���˼�һ�£�ͬһģ�ͼ�����ޱ�Ҫ
			double justify=pElem->CheckPoint(pNode->Position(false));
			if(justify<-EPS||justify>1+EPS)	//�㲻���߶���,�������Ķ˵��������Ķ˵���������ڸ���֧�������������������
				continue;
			else	//�ڵ��ڴ������ĵ�Ԫ�м䣬�Ҹ��˼�һ�£�Ϊ����֧�������ĵ�Ԫ
				break;
		}
		if(pElemStiff)	//�ҵ������Ķ˽ڵ�����Ӧ��֧�������ĵ�Ԫ
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
		//�ҳ��ڵ���ͬʱ�������������ģ��Ա㴦���ų������������Ĺ����������(��ʱ������֧����ϵ����Ϊ0)
		postElemPtrTmArr.SetSize(0,8);
		CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetValue(pNode->handle);
		for(ELEMLATERAL_STIFF *pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
		{
			if(pElemStiff->pElem->pOrgPole->handle!=pNode->RootFatherPartHandle()&&pElemStiff->pElem->pOrgPole->IsPostRod())
				postElemPtrTmArr.append(pElemStiff->pElem);
		}
		//�����㷨���ҳ������븨�Ķ˽ڵ����ӵĸ��ĵ�Ԫ�����������֧��ԭʼ����ϵ��������������ʵ��֧��ϵ��
		bool finished;
		do{	//doѭ����Ŀ����Ϊ�������ýڵ����й������ϵĸ�������
			finished=true;
			ucs.axis_x.Set();	//���֧��������ϵ���᷽��
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
					else	//���ǵ�ǰ֧���渨���ĵ�Ԫ
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
					else	//���ǵ�ǰ֧���渨���ĵ�Ԫ
						continue;
				}
			}
			if(!finished)
			{
				for(i=0;i<primaryElemPtrTmArr.GetSize();i++)
				{
					if(primaryElemPtrTmArr[i]->pStart==pNode)
						primaryElemPtrTmArr[i]->feature|=0x01;	//ʼ���Ѵ���
					else if(primaryElemPtrTmArr[i]->pEnd==pNode)
						primaryElemPtrTmArr[i]->feature|=0x02;	//�ն��Ѵ���
				}
				bool bHasPostSupport=false;
				for(i=0;i<postElemPtrTmArr.GetSize();i++)
				{
					if(fabs(ucs.axis_y*postElemPtrTmArr[i]->LenStdVec())<EPS_COS&&fabs(ucs.axis_z*postElemPtrTmArr[i]->LenStdVec())<EPS)
					{	//�ҵ����浫�뱻֧�Ų����߲����ߵ����������ĵ�Ԫ
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
	//Ϊ��ֹ���ݲ�һ�£�ִ����windSegPickerList.Empty();��Ӧ��ʱ���m_pTower->WindLoadSeg�������� wjh-2015.3.22
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
//����ĺ���������ͣ�ã���InitElemLamdaInfo���棬�Ժ�ȷ��ȷʵ����ʱ��ɾ��
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
		if(line.PtInLine(pNode->Position(false))==2)	//�ڵ���ֱ����
		{
			double dd=pElem->Length();
			INTERVAL_NODE atom;
			atom.posLen=dd;
			for(INTERVAL_NODE *pIN=list.GetFirst();pIN;pIN=list.GetNext())
			{
				if(dd<=pIN->posLen)
				{
					list.insert(atom);	//��Ҫ����
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

//����ĺ���������ͣ�ã���InitElemLamdaInfo���棬�Ժ�ȷ��ȷʵ����ʱ��ɾ��
//����֧�ŷ�����<=2��ʾƽ��֧��,3��ʾ��ά�ռ�����֧��
int GetNodeSupportDirection(INTERVAL_NODE *pIntervalNode,CLDSLinePart *pAxisPart,CTower *pTower)
{
	f3dPoint axis_vec=pAxisPart->pEnd->Position(false)-pAxisPart->pStart->Position(false);
	pIntervalNode->supportDirect.Set();
	CLdsPartListStack ps(&pTower->Parts);
	for(CLDSLinePart *pLinePart=pTower->Parts.GetFirstLinePart();pLinePart;pLinePart=pTower->Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
			continue;	//�Ǻ���ĸ˼�������Ԫ������ԤԤ��ѧ�������鹹�˼�
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
#endif
		if(pLinePart==pAxisPart)
			continue;	//�����׼�˼�
		if(pLinePart->pStart==pIntervalNode->pNode||pLinePart->pEnd==pIntervalNode->pNode||
			(pIntervalNode->pNode->m_cPosCalType==4&&
			(pIntervalNode->pNode->arrRelationPole[0]==pLinePart->handle||pIntervalNode->pNode->arrRelationPole[1]==pLinePart->handle)
			))	//�˼��˽ڵ�򽻲�ڵ������˼�
		{
			f3dPoint supportDirect=pLinePart->pEnd->Position(false)-pLinePart->pStart->Position(false);
			normalize(supportDirect);
			if(pIntervalNode->supportDirect.IsZero())
			{
				pIntervalNode->supportDirect=supportDirect^axis_vec;
				normalize(pIntervalNode->supportDirect);
			}
			else if(fabs(supportDirect*pIntervalNode->supportDirect)<EPS2)	//֧ͬ����֧��
				continue;
			else	//���ֲ�ͬ��֧�ţ�������ռ�֧��
			{
				pIntervalNode->bSupport3D=true;
				break;
			}
		}
	}
	if(pIntervalNode->bSupport3D)
		return 3;	//����ռ�֧��
	else if(pIntervalNode->supportDirect.IsZero())
		return 1;	//����֧��
	else
		return 2;	//ƽ��֧��
}
typedef struct tagSUPPORT_SECTION{
	int start_i,end_i;
	f3dPoint supportDirect;	//Ϊ��ʱ��ʾ������ռ�֧��
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
			break;	//������ά����֧�ţ�����֧�Žڼ�
		if(section.supportDirect.IsZero())	//ǰ��֧���淨��ʱ������һ��ƽ��֧�ţ��趨Ϊ��ǰ֧���淨��
			section.supportDirect=nodeArr[i].supportDirect;
		//else if(fabs(nodeArr[i].supportDirect*nodeArr[start_i].supportDirect)<EPS_COS2)		//�о����д�������ǰд���ˣ��ʸ�Ϊ������� wjh-2015.5.25
		else if(fabs(nodeArr[i].supportDirect*section.supportDirect)<EPS_COS2)//nodeArr[start_i].supportDirect)<EPS_COS2)
			break;	//������ͬ��֧�ţ�����֧�Žڼ�
	}
	if(i<nodeArr.GetSize())
		section.end_i=i;
	else
		section.end_i=nodeArr.GetSize()-1;
	return section;
}
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::SmartRetrieveCalLenInfo">
//�����ѳ�ʼ���ĳ�ϸ��ԭʼ�����Ϣ������ȡ����Ԫ�����֧�����估֧������
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
			continue;	//���ߵ�Ԫ
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
			continue;	//�Ǻ���ĸ˼�������Ԫ������ԤԤ��ѧ�������鹹�˼�
#ifdef __ANGLE_PART_INC_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
#endif
		pRodIntervals=result.hashRodIntervals.GetValue(pLinePart->handle);
		if(pRodIntervals==NULL)
			continue;
		//1.�����������ͳ�ϸ�ȼ���
		if(pLinePart->CalLenCoef.iTypeNo==1&&pRodIntervals->listElems.GetNodeNum()==1)
		{	//Vб�ģ�����ͬʱ��ѹ����Ӧ�涨�е����6��
			//A. ͬ�˵��ڸ�ͬʱ��ѹ
			pElem=*pRodIntervals->listElems.GetFirst();
			pElem->smcinfo.smcItems[0].LoRc.coef=0.65;
			if(isdigit(pElem->pStart->layer(2))&&isdigit(pElem->pEnd->layer(2))==0)
				pElem->smcinfo.smcItems[0].cSMCType=SMC_ITEM::SMC_REFER_START;
			else	//Ĭ��Ϊ�����ն˽ڵ��ڸ�ͬʱ��ѹ
				pElem->smcinfo.smcItems[0].cSMCType=SMC_ITEM::SMC_REFER_END;
			pElem->smcinfo.smcItems[0].LoRc.bCalByElemLen=true;	//����ͬʱ��ѹ
			//pElem->lamdaRc.R=Rx;	ͨ��cSMCTypeΪ��ֵ��Ĭ��ƽ����Ϊ��׼��
			pElem->smcinfo.smcItems[1].cSMCType=pElem->smcinfo.smcItems[2].cSMCType=SMC_ITEM::SMC_NONE;

			//B. ͬ�˵��ڸ�һ��һѹ(ͨ��)
		}
		//else if(pLinePart->CalLenCoef.iTypeNo==2)
		//	Vб�ģ�V�����ӣ���������ͬʱ��ѹ����Ӧ�涨�е����7��������ȫ���������жϼ���
		else if((pLinePart->CalLenCoef.iTypeNo==3||pLinePart->CalLenCoef.iTypeNo==4)&&
			pRodIntervals->listElems.GetNodeNum()==1)
		{	//Vб��3��4�ڼ�������ˣ���Ӧ�涨�е����8��9��
			CSuperSmartPtr<CLDSPart> pHorizPole=pElem->pOrgPole->BelongModel()->FromPartHandle(pElem->pOrgPole->CalLenCoef.hHorizPole);
			pElem=*pRodIntervals->listElems.GetFirst();
			//���µ�Ԫԭʼ��Ϣ�еĲ��պ����˵�Ԫ
			CElemInfo* pElemInfo=Ta.Elems.GetValue(pElem->pStart->handle,pElem->pEnd->handle);
			if(pElemInfo&&pHorizPole.IsHasPtr()&&pHorizPole->IsLinePart()&&
				pHorizPole.LinePartPointer()->pStart&&pHorizPole.LinePartPointer()->pEnd)
			{
				pElemInfo->horiElem.hStart=pHorizPole.LinePartPointer()->pStart->handle;
				pElemInfo->horiElem.hEnd=pHorizPole.LinePartPointer()->pEnd->handle;
			}
			//ͬʱ��ѹ��Ϣ��˼�����������UpdateInfoAtElemSizeChange()->CalElemCompStableInfo()�и��¼���
			//һ��һѹʱ����ȫ���������жϼ���
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
				continue;	//�û�ָ������Ҫ�����ж�
		}

		//2.�����жϼ���
		CPtInLineCheck line3d(pLinePart->pStart->Position(false),pLinePart->pEnd->Position(false));
		//ͳ�Ƶ�ǰ�˼��ϵĽڵ㼯��
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
		//�����Ԫ���г�ϸ�ȼ�����������ƥ���ж�
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
				throw CXhChar100("%d�Žڵ�ĸ��˼�Ӧ���Ǹ˼�0X%X����Ŀǰ���ǵ����޷���ȷ���㳤ϸ����Ϣ,��ȷ���Ƿ�δ���нڵ���!",pElem->pStart->pointI,pElem->pOrgPole->handle);
			if(end_i<0)
				throw CXhChar100("%d�Žڵ�ĸ��˼�Ӧ���Ǹ˼�0X%X����Ŀǰ���ǵ����޷���ȷ���㳤ϸ����Ϣ,��ȷ���Ƿ�δ���нڵ���!",pElem->pEnd->pointI,pElem->pOrgPole->handle);
			/*
			ԭϣ����������ĩ�˽ڵ�ţ�ͳһʼ�˽ڵ��ΪС���ն�Ϊ��ţ��Ա���������Ľڵ㼯������һ��
			�ָо������õ�������Ϊ��Ԫʼĩ�˽ڵ㱾������˼�ʼĩ����һ�µģ������˷�������wjh-2011.11.21
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
			{	//�ж��Ƿ��ǽ����ͬʱ��ѹ�����
				if(pElem->pOrgPole->CalLenCoef.hRefPole>0x20)
				{
					CLdsPartListStack ps(&m_pTower->Parts);
					pRefLinePart=(CLDSLinePart*)m_pTower->Parts.FromHandle(pElem->pOrgPole->CalLenCoef.hRefPole);
					if(!bHasIntersPt&&pRefLinePart.IsHasPtr()&&
						Int3dll(pElem->pOrgPole->pStart->Position(false),pElem->pOrgPole->pEnd->Position(false),
						pRefLinePart->pStart->Position(false),pRefLinePart->pEnd->Position(false),inters_pos)==1)	//���ڽ���
					{
						bHasIntersPt=true;
						pElem->pOrgPole->CalLenCoef.hRefPole=pRefLinePart->handle;
					}
					else
						throw CXhChar100("�ڼ��㽻���0X%X�ĳ�ϸ��ʱ��δ�ܸ���ָ������ҵ�ͬʱ��ѹ���ո˼�!",pElem->pOrgPole->handle);
				}
				if(!bHasIntersPt)
					pRefLinePart=NULL;
				if(bHasIntersPt)
				{	//���ڲ����㣬Ϊ����Ŀ���ͬʱ��ѹ�Ľ����
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
						break;	//ָ����ϸ��ʱ�Ľ���б��(��Ҫ��Ϊ�˼���TTA��������,ȱ�����������)wjh-2015.6.03
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
						{	//�˽ڼ��޸���֧�ţ��п�����С��Σ��
							pElem->lamdaRyo.pStart=nodeArr[start_i].pNode;
							pElem->lamdaRyo.pEnd=pIntersNode;
							lenRy0=DISTANCE(nodeArr[start_i].pNode->Position(false),pIntersNode->Position(false));
						}
						else
						{	//�˽ڼ��и���֧��
							pElem->lamdaRx.pStart=nodeArr[start_i].pNode;
							pElem->lamdaRx.pEnd=pIntersNode;
							pElem->lamdaRx.muCoefBasic=1.1;
							lenRxa=DISTANCE(nodeArr[start_i].pNode->Position(false),pIntersNode->Position(false));
						}
						if(i==end_i-1)
						{	//�˽ڼ��޸���֧�ţ��п�����С��Σ��
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
						{	//�˽ڼ��и���֧��
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
					throw CXhChar100("�ڼ��㽻���0X%X�ĳ�ϸ��ʱ��δ�ҵ�����ڵ�,��ȷ�϶��彻��ڵ�!",pElem->pOrgPole->handle);
				}
				else if( nodeArr.GetSize()<=2||start_i<0||end_i<0)
				{	//������С����㣬һ�㲻�����start_i<0��end_i<0��������������жϳ��ڷ�������
					pElem->lamdaRyo.pStart=pElem->pStart;
					pElem->lamdaRyo.pEnd=pElem->pEnd;
					pElem->lamdaRyo.bCalByElemLen=true;
				}
			}
			else
				pElem->pOrgPole->CalLenCoef.hRefPole=0;

			SUPPORT_SECTION section,sectionRx,sectionRyo;
			if(!nodeArr[start_i].bSupport3D&&start_i>0)
			{	//��ʼ��Ϊƽ��֧�Ż��޲���֧�ţ���Ҫ��ǰ����׷��
				section.supportDirect=nodeArr[start_i].supportDirect;
				do
				{	//ͬ��֧�ż�������ͬ��֧����ֹ����Ϊ�ڼ���ʼ
					start_i--;
					if(start_i>0&&section.supportDirect.IsZero())
						section.supportDirect=nodeArr[start_i].supportDirect;
					//δ��������֧�ţ����½ڵ�Ϊͬ��֧��
				}while(start_i>0&&!nodeArr[start_i].bSupport3D&&
					fabs(section.supportDirect*nodeArr[start_i].supportDirect)>EPS_COS2);
			}
			section.start_i=section.end_i=start_i;	//��ʼ��Ϊ����֧��ʱ������ǰ����׷��
			int start_search_i=start_i;
			do{
				section=FindNextSupportSection(nodeArr,section.supportDirect,start_search_i);	//??
				if(section.end_i==section.start_i+1)
				{	//��С��
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
				if(start_search_i>=end_i)	//�ڼ䳬����ǰ��Ԫ��Χ����ֹ����
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
//��ʼ������Ԫ�ĳ�ϸ�ȼ�����Ϣ
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
	//1.��ʼ����ϸ�����ܼ���ԭʼ��Ϣ��ϣ��֧�Žڼ�˵㣬�˼���֧�Žڼ��
	for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		if(pElem->elem_type!=0)
			continue;
		INTERVAL_NODE *pStartInterval=hashIntervalNode.Add(pElem->pStart->point_i);
		INTERVAL_NODE *pEndInterval=hashIntervalNode.Add(pElem->pEnd->point_i);
		pRodIntervals=result.hashRodIntervals.Add(pElem->pOrgPole->handle);
		//����Ԫʼ��,���ڵ���ӵ���Ԫ�˼����ڵ����
		pStartInterval->pNode=pElem->pStart;
		pRodIntervals->hashNodes.SetValue(pStartInterval->pNode->point_i,pStartInterval);
		//����Ԫ�ն�,���ڵ���ӵ���Ԫ�˼����ڵ����
		pEndInterval->pNode=pElem->pEnd;
		pRodIntervals->hashNodes.SetValue(pEndInterval->pNode->point_i,pEndInterval);

		if(!pStartInterval->bSupport3D)
		{
			if(pStartInterval->supportDirect.IsZero())
				pStartInterval->supportDirect=pElem->LenStdVec();
			else if(pStartInterval->posLen<=1)	//������֧��
			{
				if(fabs(pStartInterval->supportDirect*pElem->LenStdVec())<EPS_COS)	//����ƽ��֧��
				{
					pStartInterval->supportDirect=pStartInterval->supportDirect^pElem->LenStdVec();
					normalize(pStartInterval->supportDirect);
					pStartInterval->posLen=2;	//ƽ��֧��
				}
				//else	//����������֧��
			}
			else	//��ǰ����ƽ��֧��
			{
				if(fabs(pStartInterval->supportDirect*pElem->LenStdVec())>EPS2)	//��������֧��(��ԭ֧���淨�򲻴�ֱ)
					pStartInterval->bSupport3D=true;
			}
		}
		if(!pEndInterval->bSupport3D)
		{
			if(pEndInterval->supportDirect.IsZero())
				pEndInterval->supportDirect=pElem->LenStdVec();
			else if(pEndInterval->posLen<=1)	//������֧��
			{
				if(fabs(pEndInterval->supportDirect*pElem->LenStdVec())<EPS_COS)	//����ƽ��֧��
				{
					pEndInterval->supportDirect=pEndInterval->supportDirect^pElem->LenStdVec();
					normalize(pEndInterval->supportDirect);
					pEndInterval->posLen=2;	//ƽ��֧��
				}
				//else	//����������֧��
			}
			else	//��ǰ����ƽ��֧��
			{
				if(fabs(pEndInterval->supportDirect*pElem->LenStdVec())>EPS2)	//��������֧��(��ԭ֧���淨�򲻴�ֱ)
					pEndInterval->bSupport3D=true;
			}
		}
	}
	//2.������κε�Ԫ�˽ڵ�Ľ���ڵ㵽֧�Žڵ��
	for(pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
	{
		if(pNode->m_cPosCalType==4)
		{	//
			pIntervalNode=hashIntervalNode.Add(pNode->point_i);
			pIntervalNode->pNode=pNode;
			//���������ӵ�����˼����ڵ����
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
	//3.���䵥Ԫ���ڽڵ��֧�ſ��ǣ�һ���ǽ���˼��Խ�����ƽ��֧�ţ�һ���ǵ�Ԫ���ڽڵ������֧�ţ���ͬʱ���������֧�Žڼ�ָ��ڵ����Ϣ
	for(pIntervalNode=hashIntervalNode.GetFirst();pIntervalNode;pIntervalNode=hashIntervalNode.GetNext())
	{	//3.1 ���佻����Է���ά����֧�ŵĽ���ڵ��֧��
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
				else if(pIntervalNode->posLen<=1)	//����֧��
				{
					if(pIntervalNode->supportDirect*cross_norm>EPS2)	//��������֧��(�뽻��֧���淨�򲻴�ֱ)
						pIntervalNode->bSupport3D=true;
					else
					{
						pIntervalNode->supportDirect=cross_norm;
						pIntervalNode->posLen=2;	//������֧��
					}
				}
				else	//ƽ��֧��
				{
					if(pIntervalNode->supportDirect*cross_norm<EPS_COS2)	//�����ǹ���֧�Ź�����ά����֧��
						pIntervalNode->bSupport3D=true;
				}
			}
		}
		//3.2 ����Ԫ���ڽڵ������֧��,���Դ����Ķ˽ڵ㲻�������Ķ˽ڵ�,�����������޸��Ķ˽ڵ�����
		CSuperSmartPtr<CLDSPart>pFatherPart=pIntervalNode->pNode->RootFatherPart();
		if( pFatherPart.IsHasPtr()&&pFatherPart->IsLinePart()&&
			pFatherPart.LinePartPointer()->pStart&&pFatherPart.LinePartPointer()->pEnd)
		{
			pRodIntervals=result.hashRodIntervals.Add(pFatherPart->handle);
			pRodIntervals->hashNodes.SetValue(pIntervalNode->pNode->point_i,pIntervalNode);
			if(!pIntervalNode->bSupport3D&&
				pIntervalNode->pNode!=pFatherPart.LinePartPointer()->pStart&&
				pIntervalNode->pNode!=pFatherPart.LinePartPointer()->pEnd)
			{	//������ά����֧��ʱ����Ҫ�����������ĵ�֧��
				f3dPoint lineVec=pFatherPart.LinePartPointer()->pEnd->Position(false)-pFatherPart.LinePartPointer()->pStart->Position(false);
				normalize(lineVec);
				if(pIntervalNode->supportDirect.IsZero())
					pIntervalNode->supportDirect=lineVec;
				else if(pIntervalNode->posLen<=1)
				{
					if(fabs(pIntervalNode->supportDirect*lineVec)<EPS_COS)
					{	//��ͬ��֧�ţ�����֧������Ϊƽ��֧��
						pIntervalNode->supportDirect=pIntervalNode->supportDirect^lineVec;
						normalize(pIntervalNode->supportDirect);
						pIntervalNode->posLen=2;	//ƽ��֧��
					}
				}
				else// if(pIntervalNode->posLen==2)
				{	//ԭΪƽ��֧��
					if(fabs(pIntervalNode->supportDirect*lineVec)>EPS2)
					{	//��ͬ��֧�ţ�ƽ��֧������Ϊ����֧��
						pIntervalNode->bSupport3D=true;	//ƽ��֧��
					}
				}
			}
		}
		if(!pIntervalNode->bSupport3D&&pIntervalNode->posLen<=1)
			pIntervalNode->supportDirect.Set();	//����֧����Ϊ��֧�ţ��������ƽ��֧��ʱsupportDirect�������ͻ
	}
	//4.�����жϵ�Ԫ���֧�����估֧������
	SmartRetrieveCalLenInfo();
	//5.������Ҫ�����û��Լ����õĵ�Ԫ��ϸ����Ϣ
	if(!loadSrcDesignElemInfo)
		return;	//������ص�Ԫԭʼ�����Ϣ
	//��CTower::Elems�м���Ԥ���ĵ�Ԫԭʼ�����Ϣ
	for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
	{
		CElemInfo *pElemInfo=m_pTower->Elems.GetValue(pElem->pStart->handle,pElem->pEnd->handle);
		if(pElemInfo==NULL)
			continue;
		if(pElemInfo->IsSpecLoInfo())
		{	//�ɵ�Ԫԭʼ�����Ϣ��ǰ���㵥Ԫ��ͬ��
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
			//����ͬʱ��ѹ��Ϣ
			memcpy(pElem->smcinfo.smcItems,pElemInfo->smcItems,sizeof(SMC_ITEM)*3);
		}
		else	//��Ԫԭʼ�����Ϣ��ͬ��
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
	char cRodType='B';	//������
	if(bPrimaryRod)
		cRodType='P';	//������
	if(!bPrimaryRod)	//�����ģ��������ģ�
	{
		if(lamda<120)
			Klamda=lamda;				//����ƫ����ѹ
		else if(lamda<=250&&start_joint_type==JOINT_HINGE&&end_joint_type==JOINT_HINGE)
			Klamda=lamda;				//������Լ��
		else if(lamda<=330&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
			Klamda=0.615*lamda+46.2;	//������Լ��
		else if(lamda<=290)//һ����Լ��
			Klamda=0.762*lamda+28.6;
		else
			Klamda=lamda;
	}
	else //if(!bIncActualCompForce||pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//��ͨ��ѹ��0.0001Ϊ��Ϊ�趨������
	{
		if(lamda<120)
		{
			if(start_force_type==CENTRIC_FORCE&&end_force_type==CENTRIC_FORCE)
				Klamda=lamda;			//������������
			//������Ƽ����涨 DL/T 5154-2002,	����б��һ��һѹʱ���м䲿����Ϊ����������
			else if(start_force_type!=end_force_type)
				Klamda=0.75*lamda+30;	//һ������һ��ƫ����ѹ
			else// if(pElem->start_force_type==ECCENTRIC_FORCE&&pElem->end_force_type==ECCENTRIC_FORCE)
				Klamda=0.5*lamda+60;	//����ƫ����ѹ
		}
		else //if(lamda>=120)
		{
			if(CPreProcessor::ActiveDesignCode->CodeSerial()==IMemberDesignCode::CODE_DLT_2002()->CodeSerial())	//�й��淶DL/T
			{
				if(lamda<=231&&start_joint_type!=end_joint_type)	//һ����Լ��
					Klamda=0.90*lamda+11.89;
				else if(lamda<242&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
					Klamda=0.82*lamda+21.64;
				else// if(lamda<
					Klamda=lamda;
			}
			else //if(iDesignCode==1)	//����ASCE�淶
			{
				if(lamda<=225&&start_joint_type!=end_joint_type)	//һ����Լ��
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
	//����б��һ��һѹʱ���м佻���ֻ��һ����˨����Ϊ��ת��Լ�����������һ���ֿ���ʼ��
	int start_force_type=pElem->start_force_type;
	int end_force_type=pElem->end_force_type;
	if(pLoStart==pElem->pStart&&pLoEnd!=pElem->pEnd&&pLoEnd!=NULL)
	{	//���㳤��Ϊʼ�˵��뽻���
		if(pLoEnd->m_cPosCalType==4)
			end_joint_type=JOINT_HINGE;
		else
			end_joint_type=JOINT_RIGID;
		end_force_type=CENTRIC_FORCE;
	}
	else if(pLoStart!=pElem->pStart&&pLoEnd==pElem->pEnd&&pLoStart!=NULL)
	{	//���㳤��Ϊ��������ն˵�
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
	if(pElem->pOrgPole->IsAuxPole())	//�����ģ��������ģ�
		Klamda=CPreProcessor::ActiveDesignCode->CalRodKLamda(lamda,'B',start_joint_type,end_joint_type,start_force_type,end_force_type);
		//Klamda=CalKlamdaCore(lamda,false,start_joint_type,end_joint_type,start_force_type,end_force_type);
	else if(!bIncActualCompForce||pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//��ͨ��ѹ��0.0001Ϊ��Ϊ�趨������
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
	//����ʮ�������ڿ����Ƿ�ն�������ṹ�ϵĸ���Լ��
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
	//����б��һ��һѹʱ���м佻���ֻ��һ����˨����Ϊ��ת��Լ�����������һ���ֿ���ʼ��
	int start_force_type=pElem->start_force_type;
	int end_force_type=pElem->end_force_type;
	if(lamdaType==0)
	{
		if(pElem->lamdaRyo.pStart==pElem->pStart&&pElem->lamdaRyo.pEnd!=pElem->pEnd)
		{	//���㳤��Ϊʼ�˵��뽻���
			if(pElem->lamdaRyo.pEnd->m_cPosCalType==4)
				end_joint_type=JOINT_HINGE;
			else
				end_joint_type=JOINT_RIGID;
			end_force_type=CENTRIC_FORCE;
		}
		else if(pElem->lamdaRyo.pStart!=pElem->pStart&&pElem->lamdaRyo.pEnd==pElem->pEnd)
		{	//���㳤��Ϊ��������ն˵�
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
		{	//���㳤��Ϊʼ�˵��뽻���
			if(pElem->lamdaRx.pEnd->m_cPosCalType==4)
				end_joint_type=JOINT_HINGE;
			else
				end_joint_type=JOINT_RIGID;
			end_force_type=CENTRIC_FORCE;
		}
		else if(pElem->lamdaRx.pStart!=pElem->pStart&&pElem->lamdaRx.pEnd==pElem->pEnd)
		{	//���㳤��Ϊ��������ն˵�
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
		{	//���㳤��Ϊʼ�˵��뽻���
			if(pElem->lamdaRc.pEnd->m_cPosCalType==4)
				end_joint_type=JOINT_HINGE;
			else
				end_joint_type=JOINT_RIGID;
			end_force_type=CENTRIC_FORCE;
		}
		else if(pElem->lamdaRc.pStart!=pElem->pStart&&pElem->lamdaRc.pEnd==pElem->pEnd)
		{	//���㳤��Ϊ��������ն˵�
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
	if(pElem->pOrgPole->IsAuxPole())	//�����ģ��������ģ�
	{
		if(lamda<120)
			Klamda=lamda;				//����ƫ����ѹ
		else if(lamda<=250&&start_joint_type==JOINT_HINGE&&end_joint_type==JOINT_HINGE)
			Klamda=lamda;				//������Լ��
		else if(lamda<=330&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
			Klamda=0.615*lamda+46.2;	//������Լ��
		else if(lamda<=290)//һ����Լ��
			Klamda=0.762*lamda+28.6;
		else
			Klamda=lamda;
	}
	else if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//��ͨ��ѹ��0.0001Ϊ��Ϊ�趨������
	{
		if(lamda<120)
		{
			if(start_force_type==CENTRIC_FORCE&&end_force_type==CENTRIC_FORCE)
				Klamda=lamda;			//������������
			//������Ƽ����涨 DL/T 5154-2002,	����б��һ��һѹʱ���м䲿����Ϊ����������
			else if(start_force_type!=end_force_type)
				Klamda=0.75*lamda+30;	//һ������һ��ƫ����ѹ
			else// if(pElem->start_force_type==ECCENTRIC_FORCE&&pElem->end_force_type==ECCENTRIC_FORCE)
				Klamda=0.5*lamda+60;	//����ƫ����ѹ
		}
		else //if(lamda>=120)
		{
			if(Ta.m_iMemberSizeCheckCode==0)	//�й��淶DL/T
			{
				if(lamda<=231&&start_joint_type!=end_joint_type)	//һ����Լ��
					Klamda=0.90*lamda+11.89;
				else if(lamda<242&&start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
					Klamda=0.82*lamda+21.64;
				else// if(lamda<
					Klamda=lamda;
			}
			else //if(iDesignCode==1)	//����ASCE�淶
			{
				if(lamda<=225&&start_joint_type!=end_joint_type)	//һ����Լ��
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
//�й��淶
double CalStableCoef1(double lamda,char sectionType,char cMaterial);
//��������
double CalStableCoef2(double lamda,double E,double fy);
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::CalElemCompStableInfo">
void CPreProcessor::CalElemCompStableInfo(CElement *pElem,IMemberDesignCode* pDesignCode,bool bIncActualCompForce/*=true*/)
{	//���㵱ǰ��Ԫ�ĳ�ϸ��
	double lamda=0;		//  �ˣ���ϸ��
	double Klamda=0;	//K*�ˣ�������ĳ�ϸ��
	double raiseCoef=1.0;
	double Rx,Ry0,Ix,t,area;	//��λ��Ϊmm��
	char sError[100]="";
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pParaType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pParaType==NULL)
			pElem->pParaType=pParaType=FindTubeType(pElem->GetWidth(),pElem->GetThick());
		if(pParaType==NULL)
		{
			sprintf(sError,"�������Ҳ���0X%X�˼��Ĺ���%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
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
				sprintf(sError,"�������Ҳ���0X%X�˼��Ĺ��L%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
			else
				sprintf(sError,"�������Ҳ���0X%X�˼��Ĺ��%CL%.fX%.f!",pElem->pOrgPole->handle,pElem->pOrgPole->size_cSubClassType,pElem->pOrgPole->size_wide,pElem->pOrgPole->size_thick);
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
		throw "���ʴ���!";
	if(pElem->CalLenType()==0)	//pOrgPole->CalLenCoef.iTypeNo==0
	{	//�Զ����ܼ��㺬��������С����޸��Ľ���б�ġ�����ƽ������и��Ľ���б�ġ���������С�ᡢ������ƽ���ᣩ
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
		{	//�ǽ����
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
			ͬʱ��ѹʱһ����ѹ�����Ӧ�����ƣ���ʱӦ��CAnalyzeProcessor::UpdateInfoAtStatusChange�д��� wjh-2013.5.29
			if(pElem->lamdaRc.Lo_r>lamda)
			{
				lamda=pElem->lamdaRc.Lo_r;
				pElem->MaxCompStable.lamdatype_tenscomp=2;
			}
			*/
			pElem->MaxCompStable.fai_tenscomp=pDesignCode->CompStableCoef(sectionType,lamda,pMatType,t);
			/*if(iDesignCode!=2)	//	���ڹ淶
				pElem->MaxCompStable.fai_tenscomp=CalStableCoef1(lamda,sectionType,pElem->pOrgPole->cMaterial);
			else	//����ASCE 10-97����
				pElem->MaxCompStable.fai_tenscomp=CalStableCoef2(lamda,pMatType->E,pMatType->fy(t));*/
			pElem->MaxCompStable.Afai_tenscomp=pElem->MaxCompStable.fai_tenscomp*area;
		}
		else
		{
			//����һ��һѹ
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
			/*if(iDesignCode!=2)	//	���ڹ淶
				pElem->MaxCompStable.fai_tenscomp=CalStableCoef1(lamda,sectionType,pElem->pOrgPole->cMaterial);
			else	//����ASCE 10-97����
				pElem->MaxCompStable.fai_tenscomp=CalStableCoef2(lamda,pMatType->E,pMatType->fy(t));*/
			pElem->MaxCompStable.Afai_tenscomp=pElem->MaxCompStable.fai_tenscomp*area;
			/*����ͬʱ��ѹ
			ͬʱ��ѹʱһ����ѹ�����Ӧ�����ƣ���ʱӦ��CAnalyzeProcessor::UpdateInfoAtStatusChange�д��� wjh-2013.5.29
			lamda=pElem->lamdaRc.Lo_r;
			pElem->MaxCompStable.lamdatype_bothcomp=2;
			if(iDesignCode!=2)	//	���ڹ淶
				pElem->MaxCompStable.fai_bothcomp=CalStableCoef1(lamda,sectionType,pElem->pOrgPole->cMaterial);
			else	//����ASCE 10-97����
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
			//�����������ֽ��㵽һ�˾������������С�᳤ϸ�ȷ�����ƽ���������ͬʱ��ѹʱ��Σ�գ���Ӧȡ��ϸ������� wjh-2017.2.4
			//if(pElem->lamdaRyo.Lo_r>pElem->MaxCompStable.Lo_r_bothcomp)
			//	pElem->MaxCompStable.Lo_r_bothcomp=pElem->lamdaRyo.Lo_r;
		}
	}
	else if(pElem->CalLenType()==1)//pOrgPole->CalLenCoef.iTypeNo==1)
	{	//Vб�ģ�����ͬʱ��ѹ����Ӧ�涨�е����6��
		/*ͬ�˵��ڸ�ͬʱ��ѹ
		ͬʱ��ѹʱһ����ѹ�����Ӧ�����ƣ���ʱӦ��CAnalyzeProcessor::UpdateInfoAtStatusChange�д��� wjh-2013.5.29
		pElem->lamdaRc.coef=0.65;
		pElem->lamdaRc.L=pElem->lamdaRc.coef*raiseCoef*pElem->Length();
		pElem->lamdaRc.R=Rx;
		pElem->lamdaRc.Lo_r=CalKlamda(pElem,pElem->lamdaRc.L/Rx,pElem->lamdaRc.pStart,pElem->lamdaRc.pEnd);//2,false);
		pElem->MaxCompStable.lamdatype_bothcomp=2;
		pElem->MaxCompStable.fai_bothcomp=CalStableCoef1(pElem->lamdaRc.Lo_r,sectionType,pElem->pOrgPole->cMaterial);
		pElem->MaxCompStable.Afai_bothcomp=area*pElem->MaxCompStable.fai_tenscomp;
		pElem->lamdaRc.braceStr.Copy(" ELEM_LEN");
		*/
		//ͬ�˵��ڸ�һ��һѹ
		if(pElem->lamdaRyo.pStart&&pElem->lamdaRyo.pEnd)
			pElem->lamdaRyo.L=DISTANCE(pElem->lamdaRyo.pStart->Position(false),pElem->lamdaRyo.pEnd->Position(false));
		else if(pElem->lamdaRyo.bCalByElemLen)
		{
			if(pElem->Length()<EPS2)
				pElem->CalLength();
			pElem->lamdaRyo.L=pElem->Length();
			if(pElem->lamdaRyo.L<EPS2)
			{	//�������ڹ����ݲ������������������������ʾ��Ϣ wjh-2017.3.10
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,
					"%d-%d��Ԫ��С���㳤�ȹ���(L(%d-%d)=%.3fmm)�����ʵ",pElem->pStart->pointI,pElem->pEnd->pointI,
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
		//ƽ��������
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
	{	//Vб�ģ�V�����ӣ���������ͬʱ��ѹ����Ӧ�涨�е����7��
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
		//ƽ��������
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
	{	//Vб��3��4�ڼ�������ˣ���Ӧ�涨�е����8��9��
		CSuperSmartPtr<CLDSPart> pHorizPole=pElem->pOrgPole->BelongModel()->FromPartHandle(pElem->pOrgPole->CalLenCoef.hHorizPole);
		double lamda1=0,lamda2=0;
		//ͬ�˵��ڸ�ͬʱ��ѹ
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
		//�˴���Ĭ��ֵ�������ڳ���ʱ��ʾ�Ĵ������ݷ�����ֵ�����º��������ܳ��ֵ��ַ�������쳣
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
		//Ĭ�ϼ��㳤������Ϊ�˼�����
		pElem->smcinfo.smcItems[0].LoRc.hStartNode=pElem->pOrgPole->pStart->handle;
		pElem->smcinfo.smcItems[0].LoRc.hEndNode=pElem->pOrgPole->pEnd->handle;
		pElem->smcinfo.lamdaItems[0].pStart=pElem->pOrgPole->pStart;
		pElem->smcinfo.lamdaItems[0].pEnd=pElem->pOrgPole->pEnd;
		if(pElem->CalLenType()==3)//pOrgPole->CalLenCoef.iTypeNo==3)
		{	//3�ڼ�
			if(alfa<0.25)	//���޴˺����˼���
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
		{	//4�ڼ�
			if(alfa<0.4)	//���޴˺����˼���
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
				if(Int3dll(line,mirline,inters)>0)	//���ڶ˵���ڵ�
				{
					pElem->smcinfo.smcItems[0].LoRc.bCalByElemLen=false;
					double L2 =DISTANCE(inters,line.endPt);
					double L2i=DISTANCE(inters,line.startPt);
					if(L2<L2i)
					{
						pElem->smcinfo.lamdaItems[0].L=0.65*L2i;
						pElem->smcinfo.lamdaItems[0].braceStr.Printf("����~%4d",pElem->pStart->point_i);
						pElem->smcinfo.smcItems[0].LoRc.hEndNode=0;
						pElem->smcinfo.lamdaItems[0].pEnd=NULL;
					}
					else
					{
						pElem->smcinfo.lamdaItems[0].L=0.65*L2;
						pElem->smcinfo.lamdaItems[0].braceStr.Printf("����~%4d",pElem->pEnd->point_i);
						pElem->smcinfo.smcItems[0].LoRc.hStartNode=0;
						pElem->smcinfo.lamdaItems[0].pStart=NULL;
					}
					//���ڶ˵���ڵ�ʱ���㳤�����佻����Ϣ�����������������޷��������У����Ա����ڴ˸��³�ϸ����Ϣ
					pElem->smcinfo.lamdaItems[0].R=Rx;
					pElem->smcinfo.lamdaItems[0].L*=raiseCoef;
					pElem->smcinfo.lamdaItems[0].Lo_r=CalKlamda(pElem,pElem->smcinfo.lamdaItems[0].L/Rx,
						pElem->smcinfo.lamdaItems[0].pStart,pElem->smcinfo.lamdaItems[0].pEnd,bIncActualCompForce);//2);
				}
				/*else	//���Ϸ����ո�ʱ�����˼�ȫ��Ϊ���㳤��
				{
					pElem->lamdaRc.L=0.65*L3;
					pElem->lamdaRc.braceStr.Copy(" ELEM_LEN");
				}*/
			}
		}
		pElem->MaxCompStable.lamdatype_bothcomp=2;
		pElem->MaxCompStable.fai_bothcomp=CalStableCoef1(pElem->smcinfo.lamdaItems[0].Lo_r,sectionType,pElem->pOrgPole->cMaterial);
		pElem->MaxCompStable.Afai_bothcomp=area*pElem->MaxCompStable.fai_tenscomp;
		//ͬ�˽ڵ��ڸ�һ��һѹ
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
	{	//Vб��ָ���˼����㳤��
		raiseCoef=1.0;	//��Ȼ��ָ���˼����㳤�ȣ��Ŵ�ϵ����ʧȥ����ʸ�ֵΪ1.0
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
		//	logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d-%d��Ԫָ���˼���С����㳤��������Ϣ������!",pElem->pStart->pointI,pElem->pEnd->pointI);

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
		return;	//���˲�����ͬʱ��ѹ������
	for(int i=0;i<3;i++)
	{
		if(pElem->smcinfo.smcItems[i].cSMCType==SMC_ITEM::SMC_NONE)
			continue;
		if(pElem->smcinfo.smcItems[i].cSMCType>0)	//ƽ����
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
		//*�����������ֽ��㵽һ�˾������������С�᳤ϸ�ȷ�����ƽ���������ͬʱ��ѹʱ��Σ�գ���Ӧȡ��ϸ������� wjh-2017.2.4
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
			if(strchr(expr_str,'.')!=NULL)	//�뵥Ԫ���ȱ�ֵΪ������С��
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
	//���µ�Ԫ������Ϣ����PreparaTower�е���
void CPreProcessor::UpdateElemLengthInfo()
{
	for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		pElem->CalLength();	//���㵥Ԫ����
}
	//���µ�Ԫ�����(���ؼ���������)��Ϣ��ѡ�ķ����仯ʱ����
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
		pElem->CalBodyLoad(LOWEST_ROD_POSZ);	//���㵥Ԫ����
}
//�趨��������������
void CPreProcessor::SetAnalysisProcessor(CAnalyzeProcessor *pProcessor)
{
	m_pAnalysisProcessor=pProcessor;
	m_pAnalysisProcessor->SetPreProcessor(this);
}
//������ؼ����㷨
bool CPreProcessor::PREPROCESS_RESULT::InitNodeBodyLoads()
{
	hashNodeBodyLoads.Empty();
	CElement* pElem;
	CNodeBodyLoad* pNodeLoad,*pStartRelyNodeLoad,*pEndRelyNodeLoad;
	ELEM_BODYLOAD_ALLOC *pAlloc,*pAlloc2;
	for(pElem=elemList.GetFirst();pElem;pElem=elemList.GetNext())
	{	//����ȫ����Ԫ(������������)������ص���Ԫ���˽ڵ���
		//ʼ��
		pNodeLoad=hashNodeBodyLoads.Add(pElem->pStart->handle);
		pNodeLoad->pNode=pElem->pStart;
		pAlloc=pNodeLoad->hashElemAllocs.Add(pElem->id);
		pAlloc->pElem=pElem;
		pAlloc->fAllocCoef=0.5;
		//�ն�
		pNodeLoad=hashNodeBodyLoads.Add(pElem->pEnd->handle);
		pNodeLoad->pNode=pElem->pEnd;
		pAlloc=pNodeLoad->hashElemAllocs.Add(pElem->id);
		pAlloc->pElem=pElem;
		pAlloc->fAllocCoef=0.5;
	}
	for(CLDSNode* pNode=nodeSet.GetFirst();pNode;pNode=nodeSet.GetNext())
	{	//�趨ģ�ʹ����ڵ���
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
				continue;	//�����ڵ���Ѵ���ڵ�(�����������֮ǰ�����з��䵽������ӽڵ���)
			//�������Ľڵ���������Ԫ�м�����
			ELEM_PTR pResideElemArr[2]={NULL};	//pResideElem2�����ڽ����
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
					continue;	//����һ�����ӵ������ڵ������
				double fraction=proportion[k];
				double fAllocCoefOnStart=1-fraction,fAllocCoefOnEnd=fraction;
				if(pResideElemArr[1]!=NULL)
				{	//��������ڹ��õ㣬���Է�������Ӧ����
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
			pNodeLoad->hashElemAllocs.Empty();	//�ø��Ľڵ��������ѷ��䵽������Ӹ˼��˽ڵ���
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
//���ε��û��ڵ�Ԫ���Ϲ�����仯ʱ���ø�����ؼ�����Ϣ
void CPreProcessor::UpdateInfoAtElemSizeChange()
{
	//����һ����ĺŵı�׼��ѹ�ε����������
	windSegPickerList.Empty();
	InitWindSegElemPicker(m_pTower);
	//���µ�Ԫ�����(���ؼ���������)��Ϣ��ѡ�ķ����仯ʱ����
	UpdateElemBodyLoadInfo();
	//���½ڵ����ӵ�Ԫ��֧�Ÿն�
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
		//����ѡ��ǰ�˼����Ԥ����ÿ����Ԫ��ѹ����Ϣ���Ա�����ѡ����Σ�յ�ѹ�ȹ���
		//TODO:(4)[ʵʱ����ѹ�ȼ���]����Ӱ�����Ч�ʡ�wjh-2016.2.25
		CalElemCompStableInfo(pElem,ActiveDesignCode,false);//Ta.m_iMemberSizeCheckCode
	}
	for(CNodeLinkElems *pNodeLinks=result.hashNodeLinks.GetFirst();pNodeLinks;pNodeLinks=result.hashNodeLinks.GetNext())
	{
		for(ELEMLATERAL_STIFF* pElemStiff=pNodeLinks->linkElems.GetFirst();pElemStiff;pElemStiff=pNodeLinks->linkElems.GetNext())
			hashElemStiff.GetValueAt(pElemStiff->pElem->id,pElemStiff->stiff);
	}
	//�������㵽�ڵ��ϵĸ������غ��ؼ��������ؼ��������º��ؽڵ�ĵ�Ч���к���ֵ
	result.hashNodeBodyLoad.EmptyKeys();	//���ڹ�����仯����Ҫ���ԭ���ܵ�Ч�ڵ����غ���
	result.hashElemStartBodyLoad.EmptyKeys();
	result.hashElemEndBodyLoad.EmptyKeys();
	//[���仯ʱ���µ�Ԫ�����]
	for(CLDSNode* pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
	{
		BOOL pushed=result.nodeSet.push_stack();
		pNode->bodyLoad=GetBodyLoad(pNode);
		result.nodeSet.pop_stack(pushed);
	}
	//���¸��ĵ�Ԫ�ĵ�Ԫ��������Ա��������֧�Ÿն�ϵ�������ֹ���֧�ŵĸ���֧��������ϵ��
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
//���ε��û��������鷢���仯ʱ���ø��º�����Ϣ
void CPreProcessor::UpdateInfoAtModuleChange(CLDSModule *pModule)
{
	//�������������
	pModule->GetModuleScopeZ(NULL,NULL);
}
//<DEVELOP_PROCESS_MARK nameId="CPreProcessor::UpdateInfoAtLegChange">
//���ε��û����ģ���н��ȷ����仯ʱ���ø��¹������������Ϣ
void CPreProcessor::UpdateInfoAtLegChange(CLDSModule *pModule,CBlockResolver* pResolver/*=NULL*/)
{
	//���½ڵ��Ϸ���ĵ�Ч�ڵ������
	//[�����ֻ�ʱ���µ�Ԫ�����]
	for(CLDSNode* pNode=m_pAnalysisProcessor->nodeset.GetFirst();pNode;pNode=m_pAnalysisProcessor->nodeset.GetNext())
		pNode->bodyLoad=GetBodyLoad(pNode,pModule);
	//����ʣ��ǹ��ò���֧�ŵĸ���֧��������ϵ��
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
	m_pAnalysisProcessor->AnalyzeElemDegree(pResolver);	//�д��Ż�
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
//inputVΪ����������������ķ���ʹ�С��Ϣ��v1��v2����ʱ���������������˼������߷������ʱ��ʾ�������(����С�뷽����Ϣ��
bool CalRodForceByBanlance(GEPOINT inputV,double* v1,double* v2)
{
	GECS wcs;
	GEPOINT rv1(v1),rv2(v2);
	if(inputV.IsZero())
	{	//������Ϊ�㣬���ҲΪ��
		v1[0]=v1[1]=v1[2]=v2[0]=v2[1]=v2[2]=0.0;
		return true;
	}
	wcs.axis_x=inputV;
	wcs.axis_z=cross_prod(wcs.axis_x,rv1);
	if(wcs.axis_z.IsZero())
		return false;//logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"�ڵ㵱ǰ")
	normalize(wcs.axis_x);
	normalize(wcs.axis_z);
	wcs.axis_y=cross_prod(wcs.axis_z,wcs.axis_x);
	normalize(rv1);
	normalize(rv2);
	if(fabs(wcs.axis_z*rv2)>EPS)
		return false;	//������
	double input_fx=inputV.mod();
	GEPOINT lrv1=wcs.TransVToCS(rv1);	//v1�ھֲ�����ϵ�µĵ�λ�����߷��򣨳�������
	GEPOINT lrv2=wcs.TransVToCS(rv2);	//v2�ھֲ�����ϵ�µĵ�λ�����߷��򣨳�������
	//�ⷽ��: lrv1.x*P1+lrv2.x*P2=-input_fx;
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
//���ڶ����߽ڵ��϶�Ӧ������(��V������)������ص�Ч���䵽�ǹ��ߵ���
void CPreProcessor::DispatchSwingNodeLoad(CLDSModule *pModule,CWorkStatus *pStatus,bool bApplyStdLoad)
{
	char sError[100];
	bool hasSwingNode=false;
	CLDSNode *pNode;
	//ע��˴���������������ı�������һ�𣬷�������ǰ��ڶ��ڵ���ɵ���ǰ�ڵ��ϵķ��� wjh-2012.6.1
	for(pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
	{
		pNode->vSwingLoadFxyz.Set();	//���������ڶ��ڵ��Ч������ʱ��¼
		if(pNode->force_type==SWING_NODE)
			hasSwingNode=true;	//�ڶ��ڵ�
	}
	result.bHasSwingNode=hasSwingNode;
	if(!hasSwingNode)
		return;	//�ްڶ��ڵ�
	for(pNode=result.nodeSet.GetFirst();pNode;pNode=result.nodeSet.GetNext())
	{
		if(pNode->force_type!=SWING_NODE)
			continue;	//�ǰڶ��ڵ�
		LINEPARTSET rodset;
		CLDSLinePart *pRod1=NULL,*pRod2=NULL;
		BOOL bErrorNum=FALSE;
		//result.allrodset�в������ڶ��ڵ�Ĺ����˼�,��ֻ��ʹ��m_pTower->Parts���� wjh-2017.3.26
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
			sprintf(sError,"0X%X�ڵ�ڶ��ڵ�����˼�������2,���Ⱥ˲���ٽ�����ѧ����!",pNode->handle);
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
			//1.�빤����صļ��������(���ɱ���ؼ�����־���������)
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
				continue;	//��ǰ�����ڵ���ʵ�ʺ������������ýڵ�
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
			{	//��ת�ڶ��˼�(ģ���Ե�Ӵ�)��ʵ������ƽ����
				double rot_ang=Cal2dLineAng(0,0,loadvec.x,loadvec.y);
				RotateVectorAroundVector(v1,rot_ang,cs.axis_z);
				RotateVectorAroundVector(v2,rot_ang,cs.axis_z);
			}
			//���㷽��1:���ݽڵ�ƽ�ⷨ
			GEPOINT vv1=-v1,vv2=-v2;
			if(!CalRodForceByBanlance(exter_load,v1,v2))
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%d�Űڶ��ڵ���%d�Ź����µ�V�����ֽ�ʧ�ܣ����ʵ��������V�������Ƿ����!",pNode->point_i,pStatus->iNo);
			else
			{	//�ڶ��ڵ㽫�־ú���Ҳ����ɱ����
				pHangNode1->vSwingLoadFxyz-=v1;
				pHangNode2->vSwingLoadFxyz-=v2;
				if(v1*vv1<0)
					logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%4d��-%4d��Ե�Ӵ���%d�Ź����³�����ѹ���,ѹ����%.1fkN",pNode->pointI,pHangNode1->pointI,pStatus->iNo,v1.mod()*0.001);
				if(v2*vv2<0)
					logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%4d��-%4d��Ե�Ӵ���%d�Ź����³�����ѹ���,ѹ����%.1fkN",pNode->pointI,pHangNode2->pointI,pStatus->iNo,v2.mod()*0.001);
			}
			//���㷽��2:�����ӳ��߼���͸ܸ�ԭ��ֽ�(ȱ����δ�������ҵ���������)
			/*f3dPoint inters_pt;	//����������V���������ߵĽ���
			int ret=Int3dll(axis_line.startPt,axis_line.endPt,work_pos,work_pos+exter_load,inters_pt);
			if(ret==0||ret==1)
			{	//������Чʱ���ؾ���
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
//���ε��û򹤿������仯ʱ���ø�����ؼ�����Ϣ
void CPreProcessor::UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus,bool bVerifyFoundation,bool bApplyStdLoad,bool bEqCheck,int fLoadStepCoef/*=1.0*/)
{
	/*if(bVerifyFoundation)	//�������
		CalWindAndAttachLoad(m_pTower,pModule,pStatus,g_sysPara.m_iLoadExecuteCode,g_sysPara.SelMatPara.m_fWindBetaZForFoundation,1.0);
	//Ϊʲô����û��else��������һ�м������ʱ���ǰ����ˣ���������ǰ��һ������ wjh-2011-5-29
	else
		CalWindAndAttachLoad(m_pTower,pModule,pStatus,g_sysPara.m_iLoadExecuteCode,0.0,pStatus->Bw_I());*/
	//Ϊ֧�ּ������ʱ,����ϵ���ɷֶ�ȡֵ,����ͳһ��Ϊ������ʽ wjh 2011.9.22
	CalWindAndAttachLoad(m_pTower,pModule,pStatus,Ta.m_iLoadExecuteCode,bVerifyFoundation,pStatus->Bw_I());
	if(bEqCheck&&pStatus->m_cMainLoadType>=5&&pStatus->m_cMainLoadType<=8)	//�����������
	{
		if(!CalEarthQuakeLoad(m_pTower,pModule,pStatus,Ta.m_iLoadExecuteCode))
			throw "�Ҳ����ṹ�������ļ�*.frq�����Ƚ��нṹ�������Է�����";
	}
	if(result.bHasSwingNode)	//�ܽ�ʡ��ÿ�α���������ʱ�俪��
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
			continue;	//���ĵ�Ԫ
		if(pElem->pStart!=pNode&&pElem->pEnd!=pNode)	//���Դ˽ڵ�Ϊ�˽ڵ�ĸ˼���Ԫ
			continue;
		if(axis_vec.IsZero())
		{
			axis_vec=pElem->pEnd->Position(false)-pElem->pStart->Position(false);
			normalize(axis_vec);
			continue;
		}
		force_face_vec=pElem->pEnd->Position(false)-pElem->pStart->Position(false);
		normalize(force_face_vec);
		if(fabs(axis_vec*force_face_vec)>EPS_COS2)	//����ͬ��˼���Ԫ
			continue;
		if(num_face==0)
		{
			face_norm[0]=axis_vec^force_face_vec;
			normalize(face_norm[0]);
			num_face=1;
		}
		else if(fabs(force_face_vec*face_norm[0])>EPS2)
		{	//���ֵڶ�������˼�
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
//������������Ԫ�˽ڵ�����ɶȼ���������
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
			continue;	//����
		//else if(pElem->elem_type==1)
		//	m_bHasCableElem=true;	//�ṹ���������ߵ�Ԫ
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
		{	//�Զ��ж�
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
		{	//��������ܵ�Ԫ
			pElem->real_start_joint_type=JOINT_HINGE;
			pElem->real_end_joint_type=JOINT_HINGE;
		}
		else if(pElem->pOrgPole->m_iElemType==2)
		{	//��������Ԫ
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
		//�ж����ڵ�����ɶ�
		if(pElem->real_start_joint_type==JOINT_RIGID&&pElem->GetWorkI()<=0)
		{
			sError.Format("ϵͳ��0X%X�˼��ڵ�Ԫʼ����Ϊ���������˸˼������п����������������!",pElem->pOrgPole->handle);
			throw sError;
			//pElem->pStart->dimension=6;
		}
		/*else if(pElem->pFemElem->pStart->dimension!=6)
			pElem->pFemElem->pStart->dimension=3;
		if(pElem->pFemElem->end_joint_type==JOINT_RIGID)
		{
			if(pElem->GetWorkI()<=0)
			{
				sError.Format("ϵͳ��0X%X�˼��ڵ�Ԫ�ն���Ϊ���������˸˼������п����������������!",pElem->pOrgPole->handle);
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
		//��������֧�Ż�ƽ��֧�Ÿ˼���ӦѰ�Ҵ�Խ�ڵ�Ĺ��ߵ�Ԫ�� wjh-2015.6.3
		for(CElement *pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{
			if(pElem->pFemElem==NULL)
				continue;	//����
			if(pElem->pStart!=pNode&&pElem->pEnd!=pNode)
				continue;
			double prev_justify=0;
			CElement* pJointElem=NULL,*pPerfectJointElem=NULL;
			GEPOINT lenStdVec=pElem->LenStdVec();
			for(GEPOINT *pGeVec=hashVectors.GetFirst();pGeVec;pGeVec=hashVectors.GetNext())
			{
				double justify=fabs(lenStdVec*(*pGeVec));
				if(justify>EPS_COS2)	//����
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
	//У���Ƿ���Ҫ��������б�ĵİ��ƶ�ЧӦ
	m_bIncEiffelFactorX=m_bIncEiffelFactorY=ActiveDesignCode->IsIncEiffelEffect();
	VerifyEiffelFactor(pTower);
	//���ԭ������
	Empty();
	if(InheritStableModelInfo(*pTurboBuffer))
		return true;
	hashTowerNodeI.CreateHashTable(pTower->Node.GetNodeNum());
	//һ���������ܴ����ڵ㼰�˼�����
	for(pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
	{
		pNode->UnifiedCfgword();
		if(pNode->layer(0)=='L'&&cfgwordLeg.And(pNode->cfgword))
			result.allnodeset.append(pNode);
		else if(cfgwordBody.And(pNode->cfgword))
			result.allnodeset.append(pNode);
		hashTowerNodeI.SetValueAt(pNode->point_i,pNode);
		if(pNode->force_type==AUTO_NODE)
			pNode->feature=0;	//��Ϊ�Ǵ����ڵ㣨�������Ķ˵㣩
		else if(pNode->force_type==FORCE_NODE)
			pNode->feature=1;	//�ֶ��趨�Ĵ����ڵ�
		else //if(pNode->force_type==SWING_NODE)
			pNode->feature=0;	//�ڶ��ڵ��ϵĺ��ر���Ч������ڵ��Ϻ󽫱�����
	}
	for(pLinePart=pTower->Parts.GetFirstLinePart();pLinePart;pLinePart=pTower->Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
			continue;
		if(pLinePart->pStart->force_type==SWING_NODE||pLinePart->pEnd->force_type==SWING_NODE)
			continue;	//��ڶ��ڵ������ĸ˼�Ϊ�ڶ��˼�,��������
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//��ϽǸ��е��ӽǸ֣�����ϽǸ�����ѧ����ʱ����������Ϊ��Ӧ�����ظ�����
#endif
		pLinePart->UnifiedCfgword();
		if(pLinePart->layer(0)=='L'&&cfgwordLeg.And(pLinePart->cfgword))
			result.allrodset.append(pLinePart);
		else if(cfgwordBody.And(pLinePart->cfgword))
			result.allrodset.append(pLinePart);
		if(pLinePart->cMaterial==0)
			throw "���ڸ˼�ȱ�ٲ�����Ϣ,���������������У��!";
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
				logerr.Log("�ڵ�0X%X��δ���кϷ����(%d)",nodeArr[i]->handle,nodeArr[i]->point_i);
#endif
				passNodeICheck=false;
			}
			else if(hashPointI.GetValue(nodeArr[i]->point_i)!=NULL)
			{
				pNode=hashPointI[nodeArr[i]->point_i];
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("node 0X%X and node 0X%X share the same number(%d),serial number is not reasonable!",pNode->handle,nodeArr[i]->handle,pNode->point_i);
#else
				logerr.Log("�ڵ�0X%X��ڵ�0X%X����ͬ�ı��(%d)����Ų�����",pNode->handle,nodeArr[i]->handle,pNode->point_i);
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
	//����ڵ�����ع�ϣ��洢�ռ�
	result.hashNodeBodyLoad.CreateHashTable(result.nodeSet.GetNodeNum());
	//����������ָ˼�Ϊ��Ԫ����
	//�������нϳ��˼���Ҫ�м�ֶε�������д���,�������£�
	//1.��һ���˼��������������ڵ���һ������һ��Ӧ��������Ԫ��
	//2.�Բ����Ĳ����з����Թ�������Ӧ��������Ԫ
	for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
	{
		//if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
		//	continue;
#ifndef __TSA_
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
#endif
		//if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&pLinePart->hGr
		if(pLinePart->IsAuxPole())	//������
		{
			pElem=result.elemList.append();
			pElem->cfgword=pLinePart->cfgword;
			if(pLinePart->size_idClassType==CLS_CABLE)
			{	//���ߵ�Ԫ
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
		else								//������
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
				{	//����
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
					throw (char*)CXhChar50("0X%X�˼��˽ڵ���˼�������ĺŲ�һ�£�",pLinePart->handle);
				for(pNode=nodeset.GetFirst();pNode!=NULL&&pNode!=pLinePart->pEnd;pNode=nodeset.GetNext())
				{
					if(pNode->feature==0)
						continue;	//���Ǵ����ڵ�
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
						//�жϸ˵�Ԫ���˵���������
						if(pElem->pStart==pLinePart->pStart)
							pElem->start_joint_type=pLinePart->start_joint_type;
						else
							pElem->start_joint_type=JOINT_RIGID;
						if(pElem->pEnd==pLinePart->pEnd)
							pElem->end_joint_type=pLinePart->end_joint_type;
						else
							pElem->end_joint_type=JOINT_RIGID;
						//�жϸ˵�Ԫ���˵���������
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
							throw "��һ��Ԫʼĩ�˽ڵ��غϣ���������";
					}
					if(pElem->pEnd==NULL)
						result.elemList.DeleteCursor(TRUE);
					if(bPush)
						nodeset.pop_stack();
				}
			}
			else	//���Ϸ��ĸ˼�(һ���˼�����Ӧ�������˽ڵ��ڸ˼���)
			{
				char error[100]="";
				sprintf(error,"0X%X�˼�������һ���˽ڵ㲻�ڵ�ǰģ����",pLinePart->handle);
				throw error;
			}
		}
	}
	UpdateElemLengthInfo();	//���µ�Ԫ������Ϣ
	for(pElem=result.elemList.GetFirst(),i=1;pElem;pElem=result.elemList.GetNext(),i++)
		pElem->id=i;
	result.InitNodeBodyLoads();	//��ʼ���°�������㷨, ��������CElement::id��ֵ�Ժ�
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
		{	//�ӽṹ�в������Ȳ�
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
				//���㸳ֵ�ڵ��ϳ����ӵ�Ԫ��ĸ��������������ڶ������Է�������
				femStartNode.attachMass=pElem->pStart->attachMass+pElem->pStart->PermanentLoad.z/9.8;	//���Ǹ���������������Ӧ���Ǹ����������⡡wjh-2011.9.14
				femEndNode.attachMass=pElem->pEnd->attachMass+pElem->pEnd->PermanentLoad.z/9.8;	//���Ǹ���������������Ӧ���Ǹ����������⡡wjh-2011.9.14
				femStartNode.pTag=femEndNode.pTag=NULL;
				femStartNode.dimension=femEndNode.dimension=0;

				femElem.pStart=&femStartNode;
				femElem.pEnd=&femEndNode;
				femElem.elem_type=pElem->elem_type;
				femElem.start_joint_type=pElem->real_start_joint_type;
				femElem.end_joint_type=pElem->real_end_joint_type;
				femElem.density=7.85e-6*pElem->pOrgPole->m_fWeightRaiseCoef;	//�ֲ��ܶ�7.85e-6kg/mm3
				femElem.A=pElem->pOrgPole->GetArea();
				femElem.E=pElem->pOrgPole->GetElasticityModulus();
				femElem.G=femElem.E/2.6;
				femElem.Iz=femElem.Ip=pElem->pOrgPole->GetWorkI()*g_sysPara.AnalysisPara.m_fBeamLateralStifReductCoef;	//��ƽ����뾶���м���
				if(pElem->elem_type==1)
				{
					femElem.T0=pElem->T0;	//���ߵ�ԪԤ����
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
			{	//�������Ӵ������ڵ㼴Ϊ�ӽṹ�߽�ڵ�
				if(pElem->pStart->Layer(0)!='L'&&pElem->pStart->feature!=0)
					boundary.SetValue(pElem->pStart->point_i,pElem->pStart);
				if(pElem->pEnd->Layer(0)!='L'&&pElem->pEnd->feature!=0)
					boundary.SetValue(pElem->pEnd->point_i,pElem->pEnd);
			}
		}
		//SetBoundaryNode���������󣬷�����������޴˽ڵ���Ϣʱ���趨Ϊ�߽�ڵ������ʧ�� wjh-2013-4-28
		for(CLDSNodePtr* ppNode=boundary.GetFirst();ppNode;ppNode=boundary.GetNext())
			pBlock->SetBoundaryNode(boundary.GetCursorKey());
	}
	
	result.hashElemStartBodyLoad.CreateHashTable(result.elemList.GetNodeNum());
	result.hashElemEndBodyLoad.CreateHashTable(result.elemList.GetNodeNum());
	//if(bApplyLoad)
	{
		result.hashElemsMap.Empty();
		result.hashElem.CreateHashTable(result.elemList.GetNodeNum());
		//��ֵ��Ԫ��ʶ�����½ڵ����ӵ�Ԫ��Ϣ
		ELEMLATERAL_STIFF lateralStiff;
		for(pElem=result.elemList.GetFirst();pElem;pElem=result.elemList.GetNext())
		{
			result.hashElem.SetValueAt(pElem->id,pElem);
			result.hashElemsMap.SetValue(pElem->pStart->handle,pElem->pEnd->handle,pElem);
			lateralStiff.pElem=pElem;
			lateralStiff.stiff=pElem->GetWorkI()/pElem->Length();
			//����ʼ�˽ڵ����ӵ�Ԫ��Ϣ
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
			{	//���ڹ�����ĺţ���Ŀǰ�в�����ά����֧��
				if(pNodeLinks->supportType||pNodeLinks->supportDirect.IsZero())
				{
					pNodeLinks->supportType=1;
					pNodeLinks->supportDirect=pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==1&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//������֧������Ϊƽ��֧��
					pNodeLinks->supportType=2;
					pNodeLinks->supportDirect=pNodeLinks->supportDirect^pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==2&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//��ƽ��֧������Ϊ��ά����֧��
					pNodeLinks->supportType=3;
				}
			}
			//�����ն˽ڵ����ӵ�Ԫ��Ϣ
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
			{	//���ڹ�����ĺţ���Ŀǰ�в�����ά����֧��
				if(pNodeLinks->supportType||pNodeLinks->supportDirect.IsZero())
				{
					pNodeLinks->supportType=1;
					pNodeLinks->supportDirect=pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==1&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//������֧������Ϊƽ��֧��
					pNodeLinks->supportType=2;
					pNodeLinks->supportDirect=pNodeLinks->supportDirect^pElem->LenStdVec();
				}
				else if(pNodeLinks->supportType==2&&fabs(pNodeLinks->supportDirect*pElem->LenStdVec())<EPS_COS2)
				{	//��ƽ��֧������Ϊ��ά����֧��
					pNodeLinks->supportType=3;
				}
			}
		}
		//���¸����Ķ˽ڵ�λ�������ĵ�Ԫ�м�Ľڵ��������
		for(pNode=result.allnodeset.GetFirst();pNode;pNode=result.allnodeset.GetNext())
		{
			if(pNode->feature==1)	//�����Ľڵ�
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
		//���µ�Ԫ�Գ���Ϣ
		CHashList<RODMIR_INFO>hashRodMirInfo;
		for(pLinePart=result.allrodset.GetFirst();pLinePart;pLinePart=result.allrodset.GetNext())
		{
#ifndef __TSA_
			if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
				continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
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
				pMirYRod=NULL;	//�M���潻��ļ���X�Գ�����Y�Գ�
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
			pLinePart->feature%=10;	//��մ�����
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
				continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
#endif
			if(pLinePart->feature/10!=0)
				continue;	//�Ѵ���
			RODMIR_INFO *pMirInfo=hashRodMirInfo.GetValue(pLinePart->handle);
			if(pMirInfo==NULL)
				continue;	//�޶Գ���Ϣ
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
				pMirYRod=NULL;	//�M���潻��ļ���X�Գ�����Y�Գ�
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
						continue;	//��Ԫ�ԶԳ�
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_x[j]->pStart->Position(false),elem_arr_x[j]->pEnd->Position(false),1))
					{	//X��Գ�
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
						continue;	//��Ԫ�ԶԳ�
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_y[j]->pStart->Position(false),elem_arr_y[j]->pEnd->Position(false),2))
					{	//Y��Գ�
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
						continue;	//��Ԫ�ԶԳ�
					if(IsMirLine(elem_arr[i]->pStart->Position(false),elem_arr[i]->pEnd->Position(false),
						elem_arr_z[j]->pStart->Position(false),elem_arr_z[j]->pEnd->Position(false),3))
					/*	fabs(elem_arr[i]->pStart->pos.x+elem_arr_z[j]->pStart->pos.x)<EPS&&
						fabs(elem_arr[i]->pStart->pos.y+elem_arr_z[j]->pStart->pos.y)<EPS&&
						fabs(elem_arr[i]->pStart->pos.z-elem_arr_z[j]->pStart->pos.z)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.x+elem_arr_z[j]->pEnd->pos.x)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.y+elem_arr_z[j]->pEnd->pos.y)<EPS&&
						fabs(elem_arr[i]->pEnd->pos.z-elem_arr_z[j]->pEnd->pos.z)<EPS)*/
					{	//Z��Գ�
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
						elem_arr[i]->relaElem.idElemMirZ=0;//����Z��Գ�����X��Y��Գ�ʱ��X��Y��Գ�������ʾ�Գƣ��Ҳ����ظ��Գ�
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
						pMirXElem->relaElem.idElemMirZ=0;//����Z��Գ�����X��Y��Գ�ʱ��X��Y��Գ�������ʾ�Գƣ��Ҳ����ظ��Գ�
					pMirXElem->relaElem.bFatherElem=false;
				}
				if(pMirYElem&&elem_arr[i]->relaElem.idElemMirY>0)
				{
					pMirYElem->relaElem.idElemMirX=elem_arr[i]->relaElem.idElemMirZ;
					pMirYElem->relaElem.idElemMirZ=elem_arr[i]->relaElem.idElemMirX;
					if( pMirYElem->relaElem.idElemMirZ==pMirYElem->relaElem.idElemMirX||
						pMirYElem->relaElem.idElemMirZ==pMirYElem->relaElem.idElemMirY)
						pMirYElem->relaElem.idElemMirZ=0;//����Z��Գ�����X��Y��Գ�ʱ��X��Y��Գ�������ʾ�Գƣ��Ҳ����ظ��Գ�
					pMirYElem->relaElem.bFatherElem=false;
				}
				if(pMirZElem&&elem_arr[i]->relaElem.idElemMirZ>0)
				{
					pMirZElem->relaElem.idElemMirX=elem_arr[i]->relaElem.idElemMirY;
					pMirZElem->relaElem.idElemMirY=elem_arr[i]->relaElem.idElemMirX;
					if( pMirZElem->relaElem.idElemMirZ==pMirZElem->relaElem.idElemMirX||
						pMirZElem->relaElem.idElemMirZ==pMirZElem->relaElem.idElemMirY)
						pMirZElem->relaElem.idElemMirZ=0;//����Z��Գ�����X��Y��Գ�ʱ��X��Y��Գ�������ʾ�Գƣ��Ҳ����ظ��Գ�
					pMirZElem->relaElem.bFatherElem=false;
				}
				if(pFatherElem)
					pFatherElem->relaElem.bFatherElem=true;
				else
					elem_arr[i]->relaElem.bFatherElem=true;
			}
		}
		InitElemLamdaInfo(true);
		RetrieveFatherElemHashSet();	//��ȡ����Ԫ���Ա�򻯼��㵥Ԫ�Ƿ����㹻֧��
		CalAuxElemBraceForcePreInfo();	//���㸨�ĵ�Ԫ��֧����ϵ��
		if(ActiveDesignCode->GetBraceCoefOfPrimaryToPost()>0)
			CalPrimaryElemBraceForcePreInfo();	//����б�ĵ�Ԫ��֧����ϵ��
		//������ʼ�����ع�����ϣ���Ա�����ѯ
		CWorkStatus *pStatus;
		CHashTable<CWorkStatus*>statusHashTbl;
		statusHashTbl.CreateHashTable(Ta.WorkStatus.GetNodeNum());
		for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			if(pStatus->feature==0)	//����˯��״̬�ֲ�����˼��㹤��
				continue;
			double W0=pStatus->WindLoad.velocity*pStatus->WindLoad.velocity/1.6;
			statusHashTbl.SetValueAt(pStatus->handle,pStatus);
			double sina=sin(RADTODEG_COEF*pStatus->WindLoad.azimuth);
			double cosa=cos(RADTODEG_COEF*pStatus->WindLoad.azimuth);
			for(CWorkStatus::WINDLOAD_REFPARAM *pRefParam=pStatus->AttachWindLoad.GetFirst();pRefParam;pRefParam=pStatus->AttachWindLoad.GetNext())
			{
				CAttachWindLoad *pWindLoad=pTower->AttachWindLoad.FromHandle(pStatus->AttachWindLoad.GetCursorKey());
				if(pWindLoad==NULL)	//�Է����ݿ��д˺�����ɾ������������δ�ܼ�ʱ����2005/01/17 WJH
					continue;
				double node_wind_area=pWindLoad->m_fShieldArea/pWindLoad->GetNodeList()->GetNodeNum();
				pRefParam->attachMass=pWindLoad->m_fAttachWeight/pWindLoad->GetNodeList()->GetNodeNum();
				double Wk=pRefParam->shieldCoef*pWindLoad->m_fProfileCoef*pWindLoad->m_fBetaZ*node_wind_area*pWindLoad->m_fShieldCoef*W0;
				pRefParam->basicAttachWindLoadF.x=Wk*sina;
				pRefParam->basicAttachWindLoadF.y=Wk*cosa;
			}
		}
		//�ġ����³�ϸ�ȹ�������ͬʱ��ѹʱ��ϸ�ȼ����������Ĺ��ò��ֹ�����Ԫ��Ϣ
		for(CLDSLinePart *pRod=result.allrodset.GetFirst();pRod;pRod=result.allrodset.GetNext())
		{
#ifndef __TSA_
			if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0x20)
				continue;	//��ϽǸ��е��ӽǸ֣�����ʵ����������ϽǸִ��棬��Ҫ������������ظ���
#endif
			CRodIncIntervals *pRodInfo=NULL,*pRefRodInfo=NULL;
			if(pRod->CalLenCoef.hRefPole>0x20)
				pRefRodInfo=result.hashRodIntervals.GetValue(pRod->CalLenCoef.hRefPole);
			if(pRefRodInfo)
				pRodInfo=result.hashRodIntervals.GetValue(pRod->handle);
			if(pRodInfo&&pRefRodInfo&&pRodInfo->listElems.GetNodeNum()==1&&pRefRodInfo->listElems.GetNodeNum()==1)
			{	//ֻ��һ����Ԫʱ�����˼��뵥Ԫ���غϵģ����˼����ò�ּ��Ǽ��㵥Ԫ
				pElem=*pRodInfo->listElems.GetFirst();
				CElement *pRefElem=*pRefRodInfo->listElems.GetFirst();
				if(pElem->cfgword.And(pRefElem->cfgword))
				{	//������ͬʱ���ֵĿ��ܴ���
					pElem->relaElem.idRefElem=pRefElem->id;
					pRefElem->relaElem.idRefElem=pElem->id;
				}
			}
		}
	}
	return true;
}
#endif

#include "stdafx.h"
#include "XhLdsLm.h"
#include "lds_part.h"
#include "ParseAdaptNo.h"
#include "SortFunc.h"
#include "CommandDrawingModel.h"

CCmdDrawNode::CCmdDrawNode()
{
	handle=pointI=ciMatchLevel=0;
	memset(arrRelationNode,0,16);
}
void CCmdDrawNode::RetrieveCdfInfo(CLDSNode* pNode)
{
	cfgword=pNode->cfgword;
	pos=pNode->xPreliftPos;	//�ڵ�����
	pointI=pNode->pointI;
	m_cPosCalType=pNode->m_cPosCalType;// �ڵ������������ͣ�
	m_cPosCalViceType=pNode->m_cPosCalViceType;
	attach_scale=pNode->attach_scale;			// �������������(�ԽǸ���ʼ�����ȫ�Ǹֳ�)
	attach_offset=pNode->attach_offset;			// ƫ�ƾ���
	memcpy(arrRelationNode,pNode->arrRelationNode,16);	// �����ڵ�������ڵ�
}
void CCmdDrawRod::RetrieveCdfInfo(CLDSLinePart* pRod)
{
	cfgword=pRod->cfgword;
	idClsType=pRod->size_idClassType;
	pStart=m_pModel->hashNodes.GetValue(pRod->hStartNode);
	pEnd=m_pModel->hashNodes.GetValue(pRod->hEndNode);
	ciStartJointType=(BYTE)pRod->start_joint_type;	//JOINT_HINGE(0),JOINT_RIGID(1),JOINT_AUTOJUSTIFY(2);
	ciEndJointType	=(BYTE)pRod->end_joint_type;	//JOINT_HINGE(0),JOINT_RIGID(1),JOINT_AUTOJUSTIFY(2);
	ciStartForceType=(BYTE)pRod->start_force_type;	//CENTRIC_FORCE(0);ECCENTRIC_FORCE(1)
	ciEndForceType	=(BYTE)pRod->end_force_type;	//CENTRIC_FORCE(0);ECCENTRIC_FORCE(1)
	this->ciSectType=(BYTE)pRod->size_idClassType;
	this->cSubSectType=pRod->size_cSubClassType;
	this->wWide=(WORD)pRod->size_wide;
	this->wThick=(WORD)pRod->size_thick;
	this->ciMaterial=pRod->cMaterial;
	// --------- ʼ��������˨��Ϣ -----------
	startBolt.d=(WORD)pRod->connectStart.d;   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	startBolt.N=(WORD)pRod->connectStart.wnSummBoltN;   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	StrCopy(startBolt.grade,pRod->connectStart.grade,5);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	// --------- �ն�������˨��Ϣ -----------
	endBolt.d=(WORD)pRod->connectEnd.d;   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	endBolt.N=(WORD)pRod->connectEnd.wnSummBoltN;   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	StrCopy(endBolt.grade,pRod->connectEnd.grade,5);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	//������Ϣ
	/*int iConnType=0;
	buffer.ReadInteger(&iConnType);
	connectStart.m_iConnectType=(BYTE)iConnType;
	buffer.ReadString(connectStart.flNo);
	buffer.ReadInteger(&iConnType);
	connectEnd.m_iConnectType=(BYTE)iConnType;
	buffer.ReadString(connectEnd.flNo);*/
}
LINEAR_VECTOR::LINEAR_VECTOR(CCmdDrawNode* pStart,CCmdDrawNode* pEnd){Init(pStart,pEnd);}
bool LINEAR_VECTOR::Init(CCmdDrawNode* pStart,CCmdDrawNode* pEnd)
{
	xStartPoint=pStart->pos;
	vStdLenVec=pEnd->pos-pStart->pos;
	length=vStdLenVec.mod();
	if(length<EPS)
		return false;
	vStdLenVec/=length;
	return true;
}
bool LINEAR_VECTOR::Init(CCmdDrawRod* pRod)
{
	xStartPoint=pRod->pStart->pos;
	vStdLenVec=pRod->pEnd->pos-pRod->pStart->pos;
	length=vStdLenVec.mod();
	if(length<EPS)
		return false;
	vStdLenVec/=length;
	return true;
}
CCommandDrawingModel::CCommandDrawingModel(void)
{
}


CCommandDrawingModel::~CCommandDrawingModel(void)
{
}
static int _LocalCompareCmdDrawRodPtr(const CCmdDrawRodPtr& pRod1,const CCmdDrawRodPtr& pRod2)
{
	if(pRod1->matchrod.pRod->iSeg.iSeg>pRod2->matchrod.pRod->iSeg.iSeg)
		return 1;
	else if(pRod1->matchrod.pRod->iSeg.iSeg<pRod2->matchrod.pRod->iSeg.iSeg)
		return -1;
	else
		return 0;
}
void CCommandDrawingModel::CheckModel(IModel* pTower,CHashList<long>* pHashPointIById/*=NULL*/,const char* exportMatchNodeFileName2/*=NULL*/,
	const char* segIncStr/*=NULL*/,CHashList<long>* pHashIdByPointI/*=NULL*/)
{
	CLogErrorLife life(&logerr);
	CLDSNode* pLdsNode=NULL;
	CLDSLinePart* pLdsRod=NULL;
	//У������
	CHashSet<CCmdDrawNode*>hashMaptoCmdDrawNodes;	//��ֵΪCLDSNode->handle
	for(pLdsNode=pTower->EnumNodeFirst();pLdsNode;pLdsNode=pTower->EnumNodeNext())
		pLdsNode->feature=0;
	for(pLdsRod=pTower->EnumRodFirst();pLdsRod;pLdsRod=pTower->EnumRodNext())
	{	//ɸѡ�˼��˽ڵ�
		if(pLdsRod->pStart)
			pLdsRod->pStart->feature=1;
		if(pLdsRod->pEnd)
			pLdsRod->pEnd->feature=1;
	}
	CHashList<SEGI> hashIncSegs;
	if(segIncStr!=NULL&&strchr(segIncStr,'*')==NULL)
	{	//�ֽ�κ��ַ���
		GetSegNoHashTblBySegStr(segIncStr,hashIncSegs);
	}
	//1.���ݽڵ���Ϣ����Ҫ�����꣩ƥ��
	CCmdDrawNode *pMatchCmdNode=NULL,*pCmdDrawNode=NULL;
	CCmdDrawRod  *pMatchCmdRod =NULL,*pCmdDrawRod =NULL;
	for(pCmdDrawNode=hashNodes.GetFirst();pCmdDrawNode;pCmdDrawNode=hashNodes.GetNext())
		pCmdDrawNode->cbMatchFlag=0;
	for(pCmdDrawRod=hashRods.GetFirst();pCmdDrawRod;pCmdDrawRod=hashRods.GetNext())
	{
		if(pCmdDrawRod->pStart)
			pCmdDrawRod->pStart->cbMatchFlag|=0x01;
		if(pCmdDrawRod->pEnd)
			pCmdDrawRod->pEnd->cbMatchFlag|=0x01;
	}
	int rowI=0;
	CHashSet<CLDSNodePtr> hashUnmatchedNodes;
	for(pLdsNode=pTower->EnumNodeFirst();pLdsNode;pLdsNode=pTower->EnumNodeNext())
	{
		if(pLdsNode->feature==0)
			continue;
		long *pMatchPointI=pHashPointIById?pHashPointIById->GetValue(pLdsNode->handle):NULL;
		CLDSNode* pDatumNode=NULL;
		if(pLdsNode&&pLdsNode->m_cPosCalType==CLDSNode::COORD_OFFSET&&pLdsNode->m_cPosCalViceType==0)
		{
			BOOL pushed=pTower->PushNodeStack();
			pDatumNode=pTower->FromNodeHandle(pLdsNode->arrRelationNode[0]);
			pTower->PopNodeStack(pushed);
		}
		CCmdDrawNode* pMatchCmdNode=NULL;
		for(pCmdDrawNode=hashNodes.GetFirst();pCmdDrawNode&&pMatchCmdNode==NULL;pCmdDrawNode=hashNodes.GetNext())
		{
			if(pMatchPointI&&*pMatchPointI==pCmdDrawNode->pointI)
			{
				pMatchCmdNode=hashMaptoCmdDrawNodes.SetValue(pLdsNode->handle,pCmdDrawNode);
				pMatchCmdNode->ciMatchLevel=1;
				double dist=DISTANCE(pLdsNode->xPreliftPos,pMatchCmdNode->pos);
				if(dist>10)	//����
					logerr.Log("%3d#0x%X�ڵ�����ƥ���˾��ͼ�ڵ�{%d}λ��ƫ�ƾ����Զ��=%.0f",++rowI,pLdsNode->handle,pCmdDrawNode->pointI,dist);
				break;
			}
			else if(!pLdsNode->cfgword.And(pCmdDrawNode->cfgword))
				continue;	//��ĺŲ�ͬ
			if(pLdsNode->xPreliftPos.IsEqual(pCmdDrawNode->pos,0.8))
			{
				pMatchCmdNode=hashMaptoCmdDrawNodes.SetValue(pLdsNode->handle,pCmdDrawNode);
				pMatchCmdNode->ciMatchLevel=0;
			}
			else if(pDatumNode&&pDatumNode->xPreliftPos.IsEqual(pCmdDrawNode->pos,0.8))
			{
				pMatchCmdNode=hashMaptoCmdDrawNodes.SetValue(pLdsNode->handle,pCmdDrawNode);
				if(fabs(pLdsNode->attach_offset)>120)	//����
					logerr.Log("%3d#0x%X{%d}�ڵ�����ƥ��ĳ�ʼ�ڵ�ƫ�ƾ����Զ��=%.0f",++rowI,pLdsNode->handle,pCmdDrawNode->pointI,fabs(pLdsNode->attach_offset));
			}
			else	//��ƥ��
				continue;
		}
		//����һЩʵ���޷�ƥ��Ľڵ㣬��ͼͨ���ڵ�����ͬ��ƥ��
		if(pMatchCmdNode==NULL&&(hashIncSegs.GetNodeNum()==0||hashIncSegs.GetValue(pLdsNode->iSeg.iSeg)!=NULL))
		{
			CCmdDrawNode* pCmdDrawNode=hashNodes.GetValue(pLdsNode->handle);
			double dist=pCmdDrawNode==NULL?1000:DISTANCE(pCmdDrawNode->pos,pLdsNode->xPreliftPos);
			if(dist>300)
				hashUnmatchedNodes.SetValue(pLdsNode->handle,pLdsNode);
			else
			{
				pMatchCmdNode=hashMaptoCmdDrawNodes.SetValue(pLdsNode->handle,pCmdDrawNode);
				pMatchCmdNode->ciMatchLevel=2;
				pMatchCmdNode->cbMatchFlag|=0x02;	//�ҵ�˾��ͼ��ƥ��ڵ�
				logerr.Log("%3d#0x%X{%d}�ڵ�����ƥ��ĳ�ʼ�ڵ�����Զ��=%.0f",++rowI,pLdsNode->handle,pCmdDrawNode->pointI,dist);
			}
		}
		else if(pMatchCmdNode->cbMatchFlag>0)
			pMatchCmdNode->cbMatchFlag|=0x02;	//�ҵ�˾��ͼ��ƥ��ڵ�
	}
	for(pLdsNode=hashUnmatchedNodes.GetFirst();pLdsNode;pLdsNode=hashUnmatchedNodes.GetNext())
	{
		if(pLdsNode->m_cPosCalType==0)
			continue;

		CCmdDrawNode* pMatchCmdNode=NULL;
		CCmdDrawNode* parrMatchCmdNodes[4]={NULL};
		for(int i=0;i<4;i++)
			parrMatchCmdNodes[i]=(pLdsNode->arrRelationNode[i]<=0x20)?NULL:hashMaptoCmdDrawNodes.GetValue(pLdsNode->arrRelationNode[i]);
		bool matchstate=false;
		for(pCmdDrawNode=hashNodes.GetFirst();pCmdDrawNode;pCmdDrawNode=hashNodes.GetNext())
		{
			if(!pLdsNode->cfgword.And(pCmdDrawNode->cfgword))
				continue;	//��ĺŲ�ͬ
			if(!pLdsNode->xPreliftPos.IsEqual(pCmdDrawNode->pos,500))
				continue;	//�����Զ��Ϊ�϶���ƥ��
			if(pCmdDrawNode->m_cPosCalType!=pLdsNode->m_cPosCalType)
				continue;
			if(pCmdDrawNode->m_cPosCalType==CLDSNode::COORD_SCALE&&fabs(pCmdDrawNode->attach_scale-pLdsNode->attach_scale)>0.01)
				continue;	//������ȷֱ�����ͬ
			if(pCmdDrawNode->m_cPosCalType==CLDSNode::COORD_X_SPECIFIED&&fabs(pCmdDrawNode->pos.x-pLdsNode->xPreliftPos.x)>0.01)
				continue;	//ָ��x�������������ֵ��ͬ
			if(pCmdDrawNode->m_cPosCalType==CLDSNode::COORD_Y_SPECIFIED&&fabs(pCmdDrawNode->pos.y-pLdsNode->xPreliftPos.y)>0.01)
				continue;	//ָ��x�������������ֵ��ͬ
			if(pCmdDrawNode->m_cPosCalType==CLDSNode::COORD_Z_SPECIFIED&&fabs(pCmdDrawNode->pos.z-pLdsNode->xPreliftPos.z)>0.01)
				continue;	//ָ��x�������������ֵ��ͬ
			int k;
			for(k=0;k<4;k++)
			{
				long Id=parrMatchCmdNodes[k]==NULL?0:parrMatchCmdNodes[k]->Id;
				if(pCmdDrawNode->arrRelationNode[k]!=Id)
					break;
			}
			if(k<4)
			{
				double dist=pCmdDrawNode==NULL?1000:DISTANCE(pCmdDrawNode->pos,pLdsNode->xPreliftPos);
				if(dist<=300)
				{
					pMatchCmdNode=hashMaptoCmdDrawNodes.SetValue(pLdsNode->handle,pCmdDrawNode);
					pMatchCmdNode->ciMatchLevel=2;
					pMatchCmdNode->cbMatchFlag|=0x02;	//�ҵ�˾��ͼ��ƥ��ڵ�
					logerr.Log("%3d#0x%X{%d}�ڵ�����ƥ��ĳ�ʼ�ڵ�����Զ��=%.0f",++rowI,pLdsNode->handle,pCmdDrawNode->pointI,dist);
					matchstate=true;
					break;
				}
			}
		}
		if(!matchstate)
			logerr.Log("%3d#0x%X�ڵ�δ��˾��ͼ���ҵ���Ӧ��ƥ��ڵ�",++rowI,pLdsNode->handle);
	}
	if(exportMatchNodeFileName2!=NULL)
	{
		FILE* fp=NULL;
		if((fp=fopen(exportMatchNodeFileName2,"wt"))!=NULL)
		{
			for(pCmdDrawNode=hashMaptoCmdDrawNodes.GetFirst();pCmdDrawNode;pCmdDrawNode=hashMaptoCmdDrawNodes.GetNext())
			{
				CXhChar16 labelId("0x%X",hashMaptoCmdDrawNodes.GetCursorKey());
				labelId.ResizeLength(8);//,' ',true)
				fprintf(fp,"%s= %4d\n",(char*)labelId,pCmdDrawNode->pointI);
			}
			fclose(fp);
		}
	}
	for(pCmdDrawNode=hashNodes.GetFirst();pCmdDrawNode;pCmdDrawNode=hashNodes.GetNext())
	{
		if(  pCmdDrawNode->cbMatchFlag==0x01&&
			(pHashIdByPointI==NULL||pHashIdByPointI->GetValue(pCmdDrawNode->pointI)==NULL))	//ԭͼ�еĵ�Ԫ�˽ڵ�����άģ����δ�ҵ���Ӧ��ƥ��ڵ�
			logerr.Log("%3d#˾��ͼ�ж˽ڵ�{%d}δ����άģ�����ҵ���Ӧ��ƥ��ڵ�",++rowI,pCmdDrawNode->pointI);
	}
	//2.�˼�ʼĩ�˽ڵ�ƥ��
	CHashListEx<LINEAR_VECTOR> hashRodVectors;	//˾��ͼģ����ÿ���˼���Ԫ�ĳ��ȼ���λ�����췽��
	for(pLdsRod=pTower->EnumRodFirst();pLdsRod;pLdsRod=pTower->EnumRodNext())
	{
		if(pLdsRod->pStart==NULL||pLdsRod->pEnd==NULL)
			continue;
		else if(hashIncSegs.GetNodeNum()!=0&&hashIncSegs.GetValue(pLdsRod->iSeg.iSeg)==NULL)
			continue;	//���ڵ�ǰУ��κŷ�Χ��
		LINEAR_VECTOR vector;
		CCmdDrawNode* pCmdDrawStartNode =hashMaptoCmdDrawNodes.GetValue(pLdsRod->pStart->handle);
		CCmdDrawNode* pCmdDrawEndNode	=hashMaptoCmdDrawNodes.GetValue(pLdsRod->pEnd->handle);
		for(pCmdDrawRod=hashRods.GetFirst();pCmdDrawRod;pCmdDrawRod=hashRods.GetNext())
		{
			//if(pCmdDrawStartNode==NULL)
			//	logerr.Log("%3d#0x%X�˼�ʼ�˽ڵ�δ�ҵ�˾��ͼ��ƥ��ڵ�",++rowI,pLdsRod->handle);
			//if(pCmdDrawEndNode==NULL)
			//	logerr.Log("%3d#0x%X�˼��ն˽ڵ�δ�ҵ�˾��ͼ��ƥ��ڵ�",++rowI,pLdsRod->handle);
			if(pCmdDrawStartNode==NULL||pCmdDrawEndNode==NULL)
				continue;
			if(pLdsRod->size_idClassType!=pCmdDrawRod->idClsType)
				continue;
			else if(pCmdDrawRod->matchrod.pRod!=NULL)
				continue;
			else if(pCmdDrawRod->pStart==pCmdDrawStartNode&&pCmdDrawRod->pEnd==pCmdDrawEndNode)
			{	//��������ƥ��
				pCmdDrawRod->matchrod.pRod=pLdsRod;
				pCmdDrawRod->matchrod.cbMatchFlag=0x03;
			}
			else if(pCmdDrawRod->pEnd==pCmdDrawStartNode&&pCmdDrawRod->pStart==pCmdDrawEndNode)
			{	//��������ƥ��
				pCmdDrawRod->matchrod.pRod=pLdsRod;
				pCmdDrawRod->matchrod.cbMatchFlag=0x83;
			}
			else
			{
				if(!vector.IsInited())
					vector.Init(pCmdDrawStartNode,pCmdDrawEndNode);
				LINEAR_VECTOR* pCmdDrawVector=hashRodVectors.GetValue(pCmdDrawRod->handle);
				if(pCmdDrawVector==NULL)
					pCmdDrawVector=hashRodVectors.SetValue(pCmdDrawRod->handle,LINEAR_VECTOR(pCmdDrawRod));
				double justify=vector.vStdLenVec*pCmdDrawVector->vStdLenVec;
				if(fabs(justify)<EPS_COS)
					continue;	//�����߿϶��ǲ�ƥ��
				GEPOINT prjvec=pCmdDrawVector->xStartPoint-vector.xStartPoint;
				double lenoffset=prjvec*vector.vStdLenVec;
				prjvec-=lenoffset*vector.vStdLenVec;	//��������ʸ���ĺ���ͶӰ
				if(fabs(prjvec.x)+fabs(prjvec.y)+fabs(prjvec.z)>1)
					continue;	//ƽ�е�������
				pCmdDrawRod->matchrod.cbMatchFlag=0;
				if(justify>0)
				{	//����һ��
					if(fabs(lenoffset)<EPS2)
						pCmdDrawRod->matchrod.cbMatchFlag =0x01;//ʼ�˽ڵ�ƥ��
					if(fabs(lenoffset+pCmdDrawVector->length-vector.length)<EPS2)
						pCmdDrawRod->matchrod.cbMatchFlag|=0x02;//�ն˽ڵ�ƥ��
					if(lenoffset>-EPS2&&(lenoffset+pCmdDrawVector->length-vector.length)<EPS2)
						pCmdDrawRod->matchrod.pRod=pLdsRod;	//�м�ڼ�ƥ��
				}
				else //if(justify<0)
				{	//�����෴
					if(fabs(lenoffset+pCmdDrawVector->length)<EPS2)
						pCmdDrawRod->matchrod.cbMatchFlag =0x01;//��matchrod.pRodʼ�˽ڵ�ƥ��
					if(fabs(lenoffset-vector.length)<EPS2)
						pCmdDrawRod->matchrod.cbMatchFlag|=0x02;//��matchrod.pRod�ն˽ڵ�ƥ��
					if((lenoffset-pCmdDrawVector->length)>-EPS2&&(lenoffset-vector.length)<EPS2)
					{
						pCmdDrawRod->matchrod.pRod=pLdsRod;	//�м�ڼ�ƥ��
						pCmdDrawRod->matchrod.cbMatchFlag|=0x80;//����άģ���е�ƥ��˼�����
					}
				}
			}
		}
	}
	for(pCmdDrawRod=hashRods.GetFirst();pCmdDrawRod;pCmdDrawRod=hashRods.GetNext())
	{
		if(pCmdDrawRod->matchrod.pRod==NULL)
			logerr.Log("%3d#%d-%d�˼�δ����άģ�����ҵ�ƥ��ĸ˼�",++rowI,pCmdDrawRod->pStart->pointI,pCmdDrawRod->pEnd->pointI);
	}
	//4.�˼������˨��Ϣ
	ARRAY_LIST<CCmdDrawRod*>arrCmdDrawRodPtrs;
	for(pCmdDrawRod=hashRods.GetFirst();pCmdDrawRod;pCmdDrawRod=hashRods.GetNext())
	{
		if(pCmdDrawRod->matchrod.pRod==NULL)
			continue;
		arrCmdDrawRodPtrs.append(pCmdDrawRod);
	}
	CQuickSort<CCmdDrawRod*>::QuickSort(arrCmdDrawRodPtrs.m_pData,arrCmdDrawRodPtrs.GetSize(),_LocalCompareCmdDrawRodPtr);
	//for(pCmdDrawRod=hashRods.GetFirst();pCmdDrawRod;pCmdDrawRod=hashRods.GetNext())
	for(int i=0;i<arrCmdDrawRodPtrs.GetSize();i++)
	{
		pCmdDrawRod=arrCmdDrawRodPtrs[i];
		if(hashIncSegs.GetNodeNum()!=0&&hashIncSegs.GetValue(pCmdDrawRod->matchrod.pRod->iSeg.iSeg)==NULL)
			continue;	//���ڵ�ǰУ��κŷ�Χ��
		if( pCmdDrawRod->ciMaterial!=pCmdDrawRod->matchrod.pRod->cMaterial||pCmdDrawRod->idClsType!=pCmdDrawRod->matchrod.pRod->size_idClassType||
			pCmdDrawRod->wWide!=pCmdDrawRod->matchrod.pRod->size_wide||pCmdDrawRod->wThick!=pCmdDrawRod->matchrod.pRod->size_thick)
		{
			CXhChar16 sizestr,sizestr_cmddraw,steelmark;
			pCmdDrawRod->matchrod.pRod->GetSpecification(sizestr);
			if(pCmdDrawRod->ciMaterial!=CSteelMatLibrary::Q235BriefMark())
				QuerySteelMatMark(pCmdDrawRod->ciMaterial,steelmark);
			if(pCmdDrawRod->idClsType==CLS_LINEANGLE)
				sizestr_cmddraw.Printf("%sL%dx%d",(char*)steelmark,pCmdDrawRod->wWide,pCmdDrawRod->wThick);
			else if(pCmdDrawRod->idClsType==CLS_GROUPLINEANGLE)
				sizestr_cmddraw.Printf("%s%CL%dx%d",(char*)steelmark,pCmdDrawRod->cSubSectType,pCmdDrawRod->wWide,pCmdDrawRod->wThick);
			else
				sizestr_cmddraw.Printf("%s%C%dx%d",(char*)steelmark,pCmdDrawRod->ciSectType,pCmdDrawRod->wWide,pCmdDrawRod->wThick);
			CXhChar16 label=pCmdDrawRod->matchrod.pRod->Label;
			logerr.Log("%3d#0x%X�Ǹ�{%s}�Ĺ����Ϣ{%s}��˾��ͼӳ��˼�%d-%d{%s}��һ��",++rowI,pCmdDrawRod->matchrod.pRod->handle,(char*)label,(char*)sizestr,pCmdDrawRod->pStart->pointI,pCmdDrawRod->pEnd->pointI,(char*)sizestr_cmddraw);
		}
		if(pCmdDrawRod->matchrod.cbMatchFlag&0x01)
		{	//У��ʼ����˨��Ϣ(��˨ֱ������˨��)
			CLDSLinePart::ENDBOLTS endbolts;
			if(!pCmdDrawRod->matchrod.pRod->GetEffectiveEndBolts(&endbolts,true))
			{
				if(pCmdDrawRod->matchrod.pRod->IsAngle())
				{
					CXhChar16 label=pCmdDrawRod->matchrod.pRod->Label;
					if(label.Length==0)
						label.Printf("%s��",(char*)pCmdDrawRod->matchrod.pRod->iSeg.ToString());
					logerr.Log("%3d#0x%X�Ǹ�{%s}��ʼ����Ч������˨��Ϣ��ȡʧ��",++rowI,pCmdDrawRod->matchrod.pRod->handle,(char*)label);
				}
				continue;
			}
			CCmdDrawRod::BOLT* pCurrConnBolt=&pCmdDrawRod->startBolt;
			if(pCmdDrawRod->matchrod.cbMatchFlag&0x08)	//˾��ͼ��Ԫʼĩ��������άģ������ӳ��ƥ��ĸ˼��෴
				pCurrConnBolt=&pCmdDrawRod->endBolt;
			CLDSLinePart::ENDBOLTS cdfbolts(pCurrConnBolt->d,pCurrConnBolt->N,pCurrConnBolt->grade);
			double cdf_Avf=Pi*pCurrConnBolt->d*pCurrConnBolt->d*pCurrConnBolt->N*0.25;	//���������ǿ��ֵ
			double cdf_Ahf=pCurrConnBolt->d*pCmdDrawRod->wThick*pCurrConnBolt->N;	//�ױڳ�ѹǿ��ֵ
			//��Ϊ��˨����ǿ�����㲻������˨����ǿ�Ȼ��ܱ����ӹ������Ͽױڳ�ѹǿ������, �ʳ�4.8����
			//һ���ͳһΪ6.8��,ֻ����˨ֱ��������������˨ǿ�ȵȼ����� wjh-2017.8.24
			if(strcmp(pCurrConnBolt->grade,"4.8")==0)
			{
				cdf_Avf*=170;
				cdf_Ahf*=420;
			}
			else //if(strcmp(pCurrConnBolt->grade,"6.8")==0)
			{
				cdf_Avf*=240;
				cdf_Ahf*=600;
			}
			/*else if(strcmp(pCurrConnBolt->grade,"8.8")==0)
			{
				cdf_Avf*=300;
				cdf_Ahf*=800;
			}*/
			double Af_model	 =min(endbolts.Ahf,endbolts.Avf);
			double Af_cmddraw=min(cdf_Avf,cdf_Ahf);
			double safety_coef=Af_model/Af_cmddraw;
			if(safety_coef<0.99)
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%3d#0x%X�˼�{%s}ʼ����˨{%s}ǿ�Ȳ�����{%s},safety=%.2f",++rowI,
				pCmdDrawRod->matchrod.pRod->handle,pCmdDrawRod->matchrod.pRod->Label,(char*)endbolts.szBoltStr,(char*)cdfbolts.szBoltStr,safety_coef);
			else if(safety_coef>1.6)
				logerr.Log("%3d#0x%X�˼�{%s}ʼ����˨{%s}������{%s},safety=%.2f",++rowI,
				pCmdDrawRod->matchrod.pRod->handle,pCmdDrawRod->matchrod.pRod->Label,(char*)endbolts.szBoltStr,(char*)cdfbolts.szBoltStr,safety_coef);
		}
		if(pCmdDrawRod->matchrod.cbMatchFlag&0x02)
		{	//У���ն���˨��Ϣ(��˨ֱ������˨��)
			CLDSLinePart::ENDBOLTS endbolts;
			if(!pCmdDrawRod->matchrod.pRod->GetEffectiveEndBolts(&endbolts,false))
			{
				if(pCmdDrawRod->matchrod.pRod->IsAngle())
				{
					CXhChar16 label=pCmdDrawRod->matchrod.pRod->Label;
					if(label.Length==0)
						label.Printf("%s��",(char*)pCmdDrawRod->matchrod.pRod->iSeg.ToString());
					logerr.Log("%3d#0x%X�Ǹ�{%s}���ն���Ч������˨��Ϣ��ȡʧ��",++rowI,pCmdDrawRod->matchrod.pRod->handle,(char*)label);
				}
				continue;
			}
			CCmdDrawRod::BOLT* pCurrConnBolt=&pCmdDrawRod->endBolt;
			if(pCmdDrawRod->matchrod.cbMatchFlag&0x08)	//˾��ͼ��Ԫʼĩ��������άģ������ӳ��ƥ��ĸ˼��෴
				pCurrConnBolt=&pCmdDrawRod->startBolt;
			CLDSLinePart::ENDBOLTS cdfbolts(pCurrConnBolt->d,pCurrConnBolt->N,pCurrConnBolt->grade);
			double cdf_Avf=Pi*pCurrConnBolt->d*pCurrConnBolt->d*pCurrConnBolt->N*0.25;	//���������ǿ��ֵ
			double cdf_Ahf=pCurrConnBolt->d*pCmdDrawRod->wThick*pCurrConnBolt->N;	//�ױڳ�ѹǿ��ֵ
			//��Ϊ��˨����ǿ�����㲻������˨����ǿ�Ȼ��ܱ����ӹ������Ͽױڳ�ѹǿ������, �ʳ�4.8����
			//һ���ͳһΪ6.8��,ֻ����˨ֱ��������������˨ǿ�ȵȼ����� wjh-2017.8.24
			if(strcmp(pCurrConnBolt->grade,"4.8")==0)
			{
				cdf_Avf*=170;
				cdf_Ahf*=420;
			}
			else //if(strcmp(pCurrConnBolt->grade,"6.8")==0)
			{
				cdf_Avf*=240;
				cdf_Ahf*=600;
			}
			/*else if(strcmp(pCurrConnBolt->grade,"8.8")==0)
			{
				cdf_Avf*=300;
				cdf_Ahf*=800;
			}*/
			double Af_model	 =min(endbolts.Ahf,endbolts.Avf);
			double Af_cmddraw=min(cdf_Avf,cdf_Ahf);
			double safety_coef=Af_model/Af_cmddraw;
			if(safety_coef<0.99)
				logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"%3d#0x%X�˼�{%s}�ն���˨{%s}ǿ�Ȳ�����{%s},safety=%.2f",++rowI,
				pCmdDrawRod->matchrod.pRod->handle,pCmdDrawRod->matchrod.pRod->Label,(char*)endbolts.szBoltStr,(char*)cdfbolts.szBoltStr,safety_coef);
			else if(safety_coef>1.6)
				logerr.Log("%3d#0x%X�˼�{%s}�ն���˨{%s}������{%s},safety=%.2f",++rowI,
				pCmdDrawRod->matchrod.pRod->handle,pCmdDrawRod->matchrod.pRod->Label,(char*)endbolts.szBoltStr,(char*)cdfbolts.szBoltStr,safety_coef);
		}
	}
}
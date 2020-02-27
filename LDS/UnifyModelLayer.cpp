#include "stdafx.h"
#include "UnifyModelLayer.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "CommonLjSmartDesign.h"
#include "DesignWireHole.h"
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__LDS_CONTEXT_)
#include ".\StdComTempl\StdComModel.h"
#include ".\IntelliAgent\IntelliAgent.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
CUnifyModelLayer::CUnifyModelLayer(void)
{
	ciSectPolyN=4;
	//m_fDistTolerance=EPS;
	m_bKeepMechanicalLayer=m_bCorrectNodeFatherRod=true;
}

CUnifyModelLayer::~CUnifyModelLayer(void)
{
}
CUnifyModelLayer::WORKPLANE::WORKPLANE()
{
	cType=0;	//0.��������;1.��ᵣ����ƽ��;2.������ͷ����ƽ��
	id=cThirdLayer=0;
}
bool CUnifyModelLayer::WORKPLANE::IsPtOnPlane(const double* poscoords)
{
	GEPOINT lenVec(poscoords[0]-org.x,poscoords[1]-org.y,poscoords[2]-org.z);
	return fabs(lenVec*norm)<=1e-3;//EPS;
}
CUnifyModelLayer::PLATE* CUnifyModelLayer::RegisterPlate(long hBaseNode, long hBasePart, char cWorkWing,
	short siBodyOrLegNo/*=0*/)
{
	CXhChar50 key = PLATE::BuildKey(hBaseNode, hBasePart, cWorkWing, siBodyOrLegNo);
	PLATE* pRegisterPlate = hashPlates.GetValue(key);
	if (pRegisterPlate)
		return pRegisterPlate;
	else
		return hashPlates.Add(key);
}
CUnifyModelLayer::PLATE::PLATE()
{
	memset(this,0,sizeof(PLATE));
}
//siBodyOrLegNo>0��ʾ���Ⱥ�;<0��ʾ�����
CUnifyModelLayer::PLATE::PLATE(long hNode,long hDatumAngle,char cAngleWorkWing, short _siBodyOrLegNo/* = 0*/)
{
	memset(this,0,sizeof(PLATE));
	hAttachNode=hNode;
	hLocationAngle=hDatumAngle;
	cWorkWing=cAngleWorkWing;
	this->siBodyOrLegNo = _siBodyOrLegNo;
}
//siBodyOrLegNo>0��ʾ���Ⱥ�;<0��ʾ�����
CXhChar50 CUnifyModelLayer::PLATE::BuildKey(long hNode, long hDatumAngle, char cAngleWorkWing, short _siBodyOrLegNo/* = 0*/)
{
	if(cAngleWorkWing==0)
		cAngleWorkWing='X';
	else if(cAngleWorkWing==1)
		cAngleWorkWing='Y';
	if(_siBodyOrLegNo==0)
		return CXhChar50("%X-%X-%C", hDatumAngle, hNode, cAngleWorkWing);
	else if(_siBodyOrLegNo>0)
		return CXhChar50("%X-%X-%C-L%d", hDatumAngle, hNode, cAngleWorkWing, _siBodyOrLegNo);
	else
		return CXhChar50("%X-%X-%C-B%d", hDatumAngle, hNode, cAngleWorkWing,-_siBodyOrLegNo);
}
CXhChar50 CUnifyModelLayer::PLATE::StrKey()
{
	if (siBodyOrLegNo == 0)
		return CXhChar50("%X-%X-%C", hLocationAngle, hAttachNode, cWorkWing);
	else if (siBodyOrLegNo > 0)
		return CXhChar50("%X-%X-%C-L%d", hLocationAngle, hAttachNode, cWorkWing, siBodyOrLegNo);
	else
		return CXhChar50("%X-%X-%C-B%d", hLocationAngle, hAttachNode, cWorkWing, -siBodyOrLegNo);
}

CUnifyModelLayer::SLOPE::SLOPE()
{
	length=0;
	bTopOnStart=true;
	bBtmOnStart=false;
	pTopRod=pBtmRod=NULL;
	pDatumLine=NULL;
}
CUnifyModelLayer::SLOPE::SLOPE(GEPOINT toppt,GEPOINT btmpt)
{
	top=toppt;
	bottom=btmpt;
	lenStdVec=top-bottom;
	length=lenStdVec.mod();
	if(length>EPS)
		lenStdVec/=length;
	bTopOnStart=true;
	bBtmOnStart=false;
	pTopRod=pBtmRod=NULL;
	pDatumLine=NULL;
}
CUnifyModelLayer::WORKPLANE* CUnifyModelLayer::AppendUniqueWorkPlane(WORKPLANE plane,WORKPLANE** ppMatchPlane/*=NULL*/)
{
	if(plane.norm.IsZero())
	{
		if(ppMatchPlane)
			*ppMatchPlane=NULL;
		return NULL;
	}
	normalize(plane.norm);
	WORKPLANE* pPlane=NULL;
	for(pPlane=hashPlanes.GetFirst();pPlane;pPlane=hashPlanes.GetNext())
	{
		GEPOINT lenVec=plane.org-pPlane->org;
		if(fabs(pPlane->norm*plane.norm)<EPS_COS)
			continue;	//���߲�ͬ
		if(fabs(pPlane->norm*lenVec)>EPS)
			continue;	//����λ�ò�ͬ
		if(ppMatchPlane)
			*ppMatchPlane=pPlane;
		return NULL;	//�ҵ��ص��Ĺ���ƽ��
	}
	pPlane=hashPlanes.SetValue(0,plane,true);
	if(ppMatchPlane)
		*ppMatchPlane=pPlane;
	return pPlane;
}
bool CUnifyModelLayer::SLOPE::TestInSlopeLine(const double* point,double* dist2top/*=NULL*/,double tolerance/*=EPS*/)
{
	GEPOINT distvec=GEPOINT(point)-top;
	double distance=distvec*lenStdVec;
	GEPOINT prjvec=distance*lenStdVec;
	GEPOINT vertvec=distvec-prjvec;
	bool online=(vertvec.mod2()<=tolerance*tolerance);//vertvec.IsZero();
	if(dist2top)
		*dist2top=distance;
	return online;
}

double CUnifyModelLayer::GetRodGradient2(CLDSLinePart *pLinePart)
{
	if(pLinePart==NULL||pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
		return 0;
	GEPOINT vec=pLinePart->pEnd->xPreliftPos-pLinePart->pStart->xPreliftPos;
	double h2=vec.x*vec.x+vec.y*vec.y;
	if(fabs(h2)<EPS)
		return 1000000;	//��ֱ�˼�ֱ�ӷ���һ������1000000
	else
		return (vec.z*vec.z)/h2;
}

CLDSLinePart* CUnifyModelLayer::FindSlopeRodByBaseNode(CLDSNode *pBaseNode)
{
	//����������ڵ������ҽǶ����ĸ˼���Ϊ����
	CLDSLinePart *pSlopePart=NULL;
	double fTanAngle1 ;
	for(CLDSLinePart *pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
	{ 
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||(pLinePart->pStart!=pBaseNode&&pLinePart->pEnd!=pBaseNode))
			continue;
		if(pLinePart->pStart->xOriginalPos.z>pBaseNode->xOriginalPos.z||pLinePart->pEnd->xOriginalPos.z>pBaseNode->xOriginalPos.z)
			continue;
		if(pLinePart->pStart->xOriginalPos.x<EPS||pLinePart->pStart->xOriginalPos.y<EPS||pLinePart->pEnd->xOriginalPos.x<EPS||pLinePart->pEnd->xOriginalPos.y<EPS)
			continue;
		if(pSlopePart==NULL)
		{
			fTanAngle1 = GetRodGradient2(pLinePart);
			if(fTanAngle1<0.25)
				continue;	//��ֹ���������е��ᵣ�� wjh-2015.8.3
			pSlopePart=pLinePart;
			continue;
		}
		CLDSNode *pStartNode=pLinePart->pStart;
		CLDSNode *pEndNode=pLinePart->pEnd;
		double fTanAngle2 = GetRodGradient2(pLinePart);
		if(fTanAngle2<0.25)
			continue;	//��ֹ���������е��ᵣ�� wjh-2015.8.3
		if(fTanAngle2>fTanAngle1)
		{
			fTanAngle1 = fTanAngle2;
			pSlopePart = pLinePart;
		}
	}
	return pSlopePart;
}

bool CUnifyModelLayer::FindQuad1LowestBaseNode(CLDSNode** ppLowestBaseNode, CLDSLinePart** ppLowestLegRod)
{
	//1.���1������ͽڵ�����Ϊ1��˵����ǰ���ͷǵ����Ľ���������false
	CLDSNode *pBaseNode=NULL;
	//1.1 �ҵ���һ������͵�
	int lowestNodeCount=0;
	CLDSLinePart* pPrimaryRod=NULL;
	double gradient2=0;	//pPrimaryRod��Ӧ��б��ƽ��
	for(CLDSLinePart *pLinePart=Ta.Parts.GetFirstLinePart();pLinePart;pLinePart=Ta.Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL||pLinePart->m_bVirtualPart)
			continue;
		if( pLinePart->pStart->Position('X',false)<=0||pLinePart->pStart->Position('Y',false)<=0||
			pLinePart->pEnd->Position('X',false)<=0||pLinePart->pEnd->Position('Y',false)<=0)
			continue;	//ֻ������ȫ�ڵ�1�����ڵĸ˼�
		if(pBaseNode==NULL)
		{	//��ʼ��
			pBaseNode=pLinePart->pStart;
			lowestNodeCount=1;
			pPrimaryRod=pLinePart;
			gradient2=GetRodGradient2(pLinePart);
		}
		if(pLinePart->pStart->Position('Z',false)>pBaseNode->Position('Z',false))
		{
			pBaseNode=pLinePart->pStart;
			lowestNodeCount=1;
			pPrimaryRod=pLinePart;
			gradient2=GetRodGradient2(pLinePart);
		}
		else if(pLinePart->pStart!=pBaseNode&&
			fabs(pLinePart->pStart->Position('Z',false)-pBaseNode->Position('Z',false))<EPS)
			lowestNodeCount++;

		if(pLinePart->pEnd->Position('Z',false)>pBaseNode->Position('Z',false))
		{
			pBaseNode=pLinePart->pEnd;
			lowestNodeCount=1;
			pPrimaryRod=pLinePart;
			gradient2=GetRodGradient2(pLinePart);
		}
		else if(pLinePart->pEnd!=pBaseNode&&
			fabs(pLinePart->pEnd->Position('Z',false)-pBaseNode->Position('Z',false))<EPS)
			lowestNodeCount++;
		if(pLinePart->pStart==pBaseNode || pLinePart->pEnd==pBaseNode)
		{
			double grad2=GetRodGradient2(pLinePart);
			if(grad2>gradient2)
			{
				gradient2=grad2;
				pPrimaryRod=pLinePart;
				lowestNodeCount=1;
			}
		}
	}
	if(lowestNodeCount!=1)
		return false;
	else
		*ppLowestBaseNode = pBaseNode;
	if(ppLowestLegRod!=NULL)
		*ppLowestLegRod=pPrimaryRod;
	return true;
}

void CUnifyModelLayer::GetMirRotateHashTable(CHashSet<CLDSLinePart*>& hashRods)
{
	MIRMSG mirmsg;
	mirmsg.axis_flag=0x08;
	mirmsg.rotate_angle=90;
	mirmsg.array_num=1;
	for (CLDSLinePart *pLegXiePart =hashRods.GetFirst();pLegXiePart;pLegXiePart=hashRods.GetNext())
	{
		hashRods.push_stack();
		CLDSLinePart *pMirRotatePart = pLegXiePart->GetMirRod(mirmsg);
		if(pMirRotatePart==NULL||hashRods.GetValue(pMirRotatePart->handle))
			continue;
		hashRods.SetValue(pMirRotatePart->handle,pMirRotatePart);
		hashRods.pop_stack();
	}
}

void CUnifyModelLayer::GetMirHashTable(CHashSet<CLDSLinePart*>& hashRods)
{
	for (CLDSLinePart *pLegXiePart =hashRods.GetFirst();pLegXiePart;pLegXiePart=hashRods.GetNext())
	{
		hashRods.push_stack();
		if(pLegXiePart==NULL)
			continue;
		for (int j=1;j<4;j++)
		{
			CLDSLinePart *pTempPart = pLegXiePart->GetMirRod(MIRMSG(j));
			if(pTempPart==NULL||hashRods.GetValue(pTempPart->handle))
				continue;
			hashRods.SetValue(pTempPart->handle,pTempPart);
		}
		hashRods.pop_stack();
	}
}

void CUnifyModelLayer::GetTopNodeByDirection(CHashSet<CLDSLinePart*>&hashSlopePart,f3dPoint stdXieLenVec,
	CLDSNode* pSlopeBtmQuadNode,CLDSNode**ppTopNode,CLDSLinePart** ppTopRod/*=NULL*/,COLINEAR_TOLERANCE* pTolerance/*=NULL*/)
{
	COLINEAR_TOLERANCE xPreferTolerance;
	if(pTolerance==NULL)
		pTolerance=&xPreferTolerance;
	CLDSNode *pDatumNode = *ppTopNode;
	CPtInLineCheck check(pSlopeBtmQuadNode->xOriginalPos,pDatumNode->xOriginalPos);
	bool reloop=true;
	for (CLDSLinePart* pPart = Ta.Parts.GetFirstLinePart();pPart;
		reloop?pPart=Ta.Parts.GetFirstLinePart():pPart=Ta.Parts.GetNextLinePart())
	{
		reloop=false;
		if(pPart->pStart==NULL||pPart->pEnd==NULL||!(pPart->pEnd==pDatumNode||pPart->pStart==pDatumNode))
			continue;
		if( pPart->pEnd->Position('Z',false)>pDatumNode->Position('Z',false)||
			pPart->pStart->Position('Z',false)>pDatumNode->Position('Z',false)||
			fabs(pPart->pEnd->Position('Z',false)-pPart->pStart->Position('Z',false))<EPS)
			continue;
		if(!pSlopeBtmQuadNode->ModuleTogetherWith(pPart))
			continue;
		if(hashSlopePart.GetValue(pPart->handle))
			continue;
		bool bOnLine=false;
		check.CheckPointEx(pPart->pStart->xOriginalPos,&bOnLine,pTolerance->distance,pTolerance->tangent);
		if(!bOnLine)	//������
			continue;
		check.CheckPointEx(pPart->pEnd->xOriginalPos,&bOnLine,pTolerance->distance,pTolerance->tangent);
		if(!bOnLine)	//������
			continue;
		/*f3dPoint vectPt;
		if(pDatumNode==pPart->pStart)
			vectPt =pPart->pEnd->Position(false)-pPart->pStart->Position(false);
		else if(pDatumNode==pPart->pEnd)
			vectPt =pPart->pStart->Position(false)-pPart->pEnd->Position(false);
		else
			continue;
		normalize(vectPt);
		if(fabs(stdXieLenVec*vectPt)<EPS_COS)
			continue;	//������*/
		hashSlopePart.SetValue(pPart->handle,pPart);
		if(pDatumNode==pPart->pEnd)
		{
			*ppTopNode=pDatumNode=pPart->pStart;
			if(ppTopRod)
				*ppTopRod=pPart;
			check.SetLine(pSlopeBtmQuadNode->xOriginalPos,pDatumNode->xOriginalPos);
			reloop=true;
		}
		else
		{
			*ppTopNode=pDatumNode=pPart->pEnd;
			if(ppTopRod)
				*ppTopRod=pPart;
			check.SetLine(pSlopeBtmQuadNode->xOriginalPos,pDatumNode->xOriginalPos);
			reloop=true;
		}
	}
}

CLDSNode* CUnifyModelLayer::FindLegTopNodeByLegTopZ(double fLegTopZ,CHashSet<CLDSLinePart*>&hashLegPostRods,
	CLDSNode* pSlopeBtmQuadNode,CLDSLinePart* pSlopeBtmQuadRod,CLDSLinePart** ppTopRod/*=NULL*/)
{
	//1.�õ��Ȳ����ķ���
	if(pSlopeBtmQuadRod==NULL)
		return NULL;
	hashLegPostRods.SetValue(pSlopeBtmQuadRod->handle,pSlopeBtmQuadRod);
	f3dPoint vect = pSlopeBtmQuadRod->pStart->Position(false) - pSlopeBtmQuadRod->pEnd->Position(false);
	if(vect.z>0)
		vect=-vect;
	normalize(vect);
	//2.�����Ȳ����ķ����fLegTopZȷ�������Ȳ�����
	CLDSNode *pLegTopNode = pSlopeBtmQuadNode;
	bool reloop=true;
	for (CLDSLinePart* pPart = Ta.Parts.GetFirstLinePart();pPart;
		reloop?pPart=Ta.Parts.GetFirstLinePart():pPart=Ta.Parts.GetNextLinePart())
	{
		reloop=false;
		if(pPart->pStart==NULL||pPart->pEnd==NULL||!pSlopeBtmQuadNode->ModuleTogetherWith(pPart))
			continue;
		f3dPoint vectPt;
		if(pPart->pEnd==pLegTopNode)
			vectPt=pPart->pStart->Position(false)-pPart->pEnd->Position(false);
		else if(pPart->pStart==pLegTopNode)
			vectPt=pPart->pEnd->Position(false)-pPart->pStart->Position(false);
		else
			continue;
		if(vectPt.z>-EPS)
			continue;
		normalize(vectPt);
		if(fabs(vect*vectPt)<EPS_COS)
			continue;
		double fMaxRodZ = max(pPart->pEnd->Position('Z',false),pPart->pStart->Position('Z',false));
		if(fMaxRodZ>fLegTopZ+EPS)
		{	//�Ѵ�
			if(pPart->pStart==pLegTopNode)
				pLegTopNode = pPart->pEnd;
			else
				pLegTopNode = pPart->pStart;
			if(ppTopRod)
				*ppTopRod=pPart;
			hashLegPostRods.SetValue(pPart->handle,pPart);
			reloop=true;
		}
	}
	//3.�õ��Ȳ����ĶԳ�����
	GetMirHashTable(hashLegPostRods);
	return pLegTopNode;
}

double CUnifyModelLayer::FindLegTopZ(CHashSet<CLDSLinePart*>&hashLegXiePart,CLDSNode* pSlopeBtmQuadNode,CLDSLinePart* pSlopeBtmQuadRod)
{
	CLDSNode *pDatumNode=NULL;
	CLDSLinePart *pDatumPart=NULL;
	//1.�õ�һ��б��
	GEPOINT post_stdvec=pSlopeBtmQuadRod->End()-pSlopeBtmQuadRod->Start();
	normalize(post_stdvec);
	if(post_stdvec.z>0)
		post_stdvec*=-1;
	for (CLDSLinePart* pPart = Ta.Parts.GetFirstLinePart();pPart;pPart=Ta.Parts.GetNextLinePart())
	{
		if(pPart==NULL||pPart->pStart==NULL||pPart->pEnd==NULL||pPart==pSlopeBtmQuadRod||
			fabs(pPart->pEnd->Position('Z',false)-pPart->pStart->Position('Z',false))<EPS)
			continue;
		long hGroupAngle=0,hGroupAngle2=0;
		if(pSlopeBtmQuadRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
			hGroupAngle=pSlopeBtmQuadRod->handle;
		else if(pSlopeBtmQuadRod->GetClassTypeId()==CLS_LINEANGLE)
			hGroupAngle=((CLDSLineAngle*)pSlopeBtmQuadRod)->group_father_jg_h;
		if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			hGroupAngle2=pPart->handle;
		else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			hGroupAngle2=((CLDSLineAngle*)pPart)->group_father_jg_h;
		if(hGroupAngle==hGroupAngle2&&hGroupAngle>0)
			continue;	//������ͬһ����ϽǸ�

		if(!pSlopeBtmQuadNode->ModuleTogetherWith(pPart))
			continue;
		GEPOINT stdvec;
		if(pPart->pEnd==pSlopeBtmQuadNode)
		{
			pDatumNode = pPart->pStart;
			pDatumPart = pPart;
			stdvec=pPart->pStart->Position(false)-pPart->pEnd->Position(false);
		}
		else if(pPart->pStart==pSlopeBtmQuadNode)
		{
			pDatumNode = pPart->pEnd;
			pDatumPart = pPart;
			stdvec=pPart->pEnd->Position(false)-pPart->pStart->Position(false);
		}
		if(!stdvec.IsZero())
		{
			normalize(stdvec);
			double justify=post_stdvec*stdvec;
			if(justify>EPS&&justify<EPS_COS)
				break;	//����ҵ�һ�����ʵ��Ȳ�б��
		}
	}
	if(pDatumPart==NULL)
		return 0;
	hashLegXiePart.SetValue(pDatumPart->handle,pDatumPart);
	//2.�õ�б����ߵ�
	f3dPoint vect ;
	CLDSNode *pTopNode;
	if(pDatumPart->pEnd->Position('Z',false)>pDatumPart->pStart->Position('Z',false))
	{
		vect = pDatumPart->pStart->Position(false) - pDatumPart->pEnd->Position(false);
		pTopNode = pDatumPart->pStart;
	}
	else //if(pDatumPart->pEnd->Position('Z',false)<pDatumPart->pStart->Position('Z',false))
	{
		vect = pDatumPart->pEnd->Position(false) - pDatumPart->pStart->Position(false);
		pTopNode = pDatumPart->pEnd;
	}
	vect=vect.normalized();
	GetTopNodeByDirection(hashLegXiePart,vect,pSlopeBtmQuadNode,&pTopNode);
	return pTopNode->Position('Z',false);
}

void CUnifyModelLayer::UpdateSlopeRodMirLayer(CLDSLinePart* pSlopeRod)
{
	for (int i=1;i<4;i++)
	{
		CLDSLinePart* pPart =(CLDSLinePart*)pSlopeRod->GetMirPart(MIRMSG(i));
		if(pPart)
			pSlopeRod->SyncMirPropTo(pPart,MIRMSG(i),"layer");
	}
}

void CUnifyModelLayer::GetSlopeTopNode(CHashSet<CLDSLinePart*>&hashSlopePart,
	CLDSNode *pSlopeBtmQuadNode,CLDSLinePart* pSlopeBtmQuadRod,CLDSNode **ppSlopeTopQuadNode,CLDSLinePart** ppTopRod/*=NULL*/,
	COLINEAR_TOLERANCE* pTolerance/*=NULL*/)
{
	f3dPoint slopeQuadLineVec;
	if(pSlopeBtmQuadRod->pEnd==pSlopeBtmQuadNode)
	{
		*ppSlopeTopQuadNode = pSlopeBtmQuadRod->pStart;
		if(ppTopRod)
			*ppTopRod=pSlopeBtmQuadRod;
		slopeQuadLineVec = pSlopeBtmQuadRod->pStart->Position(false)-pSlopeBtmQuadRod->pEnd->Position(false);
	}
	else if(pSlopeBtmQuadRod->pStart==pSlopeBtmQuadNode)
	{
		*ppSlopeTopQuadNode = pSlopeBtmQuadRod->pEnd;
		if(ppTopRod)
			*ppTopRod=pSlopeBtmQuadRod;
		slopeQuadLineVec = pSlopeBtmQuadRod->pEnd->Position(false)-pSlopeBtmQuadRod->pStart->Position(false);
	}
	normalize(slopeQuadLineVec);
	GetTopNodeByDirection(hashSlopePart,slopeQuadLineVec,pSlopeBtmQuadNode,ppSlopeTopQuadNode,ppTopRod,pTolerance);
}
/*������������ж�ԭ�򲻺����ҿɶ���̫�wjh-2015.3.28
char GetLayerCharacter(CXhPtrSet<CLDSNode> &nodeList,CLDSLinePart *pPart)
{
	char cName=pPart->layer(2);
	f3dPoint pt1,pt2,pt3,pt4,pt5,pt6;
	f3dPoint centerPt = (pPart->pStart->Position(false) + pPart->pEnd->Position(false))/2;
	f3dPoint centerPt2 = centerPt;
	for(CLDSNode *pNode = nodeList.GetFirst();pNode;pNode=nodeList.GetNext())
	{
		if(pNode->Position('Y',false)>0&&pNode->Position('X',false)>0)
		{
			if(pt1.x==0)
				pt1=pNode->Position(false);
			else 
				pt2=pNode->Position(false);
		}
		else if(pNode->Position('Y',false)<0&&pNode->Position('X',false)<0)
		{
			if(pt3.x==0)
				pt3=pNode->Position(false);
			else 
				pt4=pNode->Position(false);
		}
		else if(pt5.x==0&&pNode->Position('Y',false)>0&&pNode->Position('X',false)<0)
			pt5 = pNode->Position(false);
		else if(pt6.x==0&&pNode->Position('Y',false)<0&&pNode->Position('X',false)>0)
			pt6 = pNode->Position(false);
	}
	f3dPoint vect1 = pt1-pt2;
	f3dPoint vect2 = pt2-pt5;
	f3dPoint vect3 = pt3-pt4;
	f3dPoint vect4 = pt4-pt6;
	f3dPoint vect5 = pt1-pt6;
	f3dPoint vect6 = pt3-pt5;
	project_point(centerPt, pt1, vect1^vect2);
	double fDistanceQ =DISTANCE(centerPt,centerPt2);

	centerPt = centerPt2;
	project_point(centerPt, pt3, vect3^vect4);
	double fDistanceH =DISTANCE(centerPt,centerPt2);

	centerPt = centerPt2;
	project_point(centerPt,pt1,vect1^vect5);
	double fDistanceY =DISTANCE(centerPt,centerPt2);

	centerPt = centerPt2;
	project_point(centerPt,pt3,vect3^vect6);
	double fDistanceZ =DISTANCE(centerPt,centerPt2);

	if(fDistanceQ<fDistanceZ&&fDistanceQ<fDistanceH&&fDistanceQ<fDistanceY)
		cName = 'Q';
	else if(fDistanceH<fDistanceZ&&fDistanceH<fDistanceQ&&fDistanceH<fDistanceY)
		cName = 'H';
	else if(fDistanceZ<fDistanceQ&&fDistanceZ<fDistanceH&&fDistanceZ<fDistanceY)
		cName = 'Z';
	else if(fDistanceY<fDistanceZ&&fDistanceY<fDistanceH&&fDistanceY<fDistanceQ)
		cName = 'Y';
	return cName;
}
*/
void CUnifyModelLayer::InitHashSlopeNode(CHashSet<CLDSNode*>&hashSlopeNode,CLDSNode * pTopNode,CLDSNode *pDownNode)
{
	f3dLine lineArr[4];
	lineArr[0].startPt = pDownNode->Position(false);
	lineArr[0].endPt = pTopNode->Position(false);
	for (int i =1 ;i<4;i++)
	{
		CLDSNode *pTempTopNode =pTopNode->GetMirNode(i);
		CLDSNode *pTempDownNode =pDownNode->GetMirNode(i);
		if(pTempDownNode==NULL||pTempTopNode==NULL)
			continue;
		lineArr[i].startPt = pTempTopNode->Position(false);
		lineArr[i].endPt = pTempDownNode->Position(false);
	}
	for (CLDSNode *pNode = Ta.Node.GetFirst();pNode;pNode = Ta.Node.GetNext())
	{
		f3dPoint nodePt = pNode->Position(false);
		if(!pDownNode->ModuleTogetherWith(pNode))
			continue;
		if(nodePt.z>pDownNode->Position('Z',false)||nodePt.z<pTopNode->Position('Z',false))
			continue;
		if((lineArr[0].PtInLine(nodePt)>0||lineArr[1].PtInLine(nodePt)>0||lineArr[2].PtInLine(nodePt)>0||lineArr[3].PtInLine(nodePt)>0)&&hashSlopeNode.GetValue(pNode->handle)==NULL)
			hashSlopeNode.SetValue(pNode->handle,pNode);
	}
}
bool CUnifyModelLayer::SLOPE::MergeWithUpperSlope(CUnifyModelLayer::SLOPE* pUpperSlope)
{
	top=pUpperSlope->top;
	lenStdVec=bottom-top;
	length=lenStdVec.mod();
	lenStdVec/=length;
	pTopRod=pUpperSlope->pTopRod;
	bTopOnStart=pUpperSlope->bTopOnStart;
	taper.VerifyVertex(top);
	taper.VerifyVertex(GEPOINT(-top.x,-top.y,top.z));
	this->InitTaperSlopePlanes();
	return true;
}
void CUnifyModelLayer::SLOPE::InitTaperSlopePlanes()
{
	planes[0].org.Set(taper.TopMaxX(),taper.TopMaxY(),taper.TopZ());
	GEPOINT len_vec=GEPOINT(taper.BtmMaxX(),taper.BtmMaxY(),taper.BtmZ())-planes[0].org;
	planes[0].norm=GEPOINT(1,0,0)^len_vec;
	planes[0].cThirdLayer='Q';

	planes[3].org=planes[0].org;
	planes[3].norm=GEPOINT(0,1,0)^len_vec;
	planes[3].cThirdLayer='Y';	//��'Y'
	
	planes[1].org.Set(taper.TopMinX(),taper.TopMinY(),taper.TopZ());
	len_vec=GEPOINT(taper.BtmMinX(),taper.BtmMinY(),taper.BtmZ())-planes[1].org;
	planes[1].norm=GEPOINT(1,0,0)^len_vec;
	planes[1].cThirdLayer='H';	//��'H'

	planes[2].org=planes[1].org;
	planes[2].norm=GEPOINT(0,1,0)^len_vec;
	planes[2].cThirdLayer='Z';	//��'Z'
	for(int i=0;i<4;i++)
		normalize(planes[i].norm);
}
char CUnifyModelLayer::SLOPE::IdentifyPointTaperSlopeLayer(const GEPOINT& point,double tolerance/*=EPS*/)
{
	char arrLayerPos[4]={'Q','H','Z','Y'};
	bool inplanes[4]={false};
	for(int i=0;i<4;i++)
	{
		GEPOINT prjVec=point-planes[i].org;
		if(fabs(prjVec*planes[i].norm)<tolerance)
			inplanes[i]=true;
	}
	if(inplanes[0]&&inplanes[2])
		return '2';	//ͬʱ��'Q'��'Z'��ʱ��ʾ��2�����޽�����
	else if(inplanes[0]&&inplanes[3])
		return '1';	//ͬʱ��'Q'��'Y'��ʱ��ʾ��2�����޽�����
	else if(inplanes[1]&&inplanes[2])
		return '4';	//ͬʱ��'H'��'Z'��ʱ��ʾ��4�����޽�����
	else if(inplanes[1]&&inplanes[3])
		return '3';	//ͬʱ��'H'��'Y'��ʱ��ʾ��3�����޽�����
	else
	{
		for(int j=0;j<4;j++){
			if(inplanes[j])
				return arrLayerPos[j];
		}
		return 'G';
	}
}
char CUnifyModelLayer::SLOPE::IdentifyLineTaperSlopeLayer(CLDSLinePart* pRod,double tolerance/*=EPS*/)
{
	GEPOINT start=pRod->pStart->Position(false);
	GEPOINT end=pRod->pEnd->Position(false);
	//char arrLayerPos[4]={'Q','H','Z','Y'};
	bool inplanes[4]={false};
	for(int i=0;i<4;i++)
	{
		GEPOINT prjVecStart=start-planes[i].org;
		GEPOINT prjVecEnd=end-planes[i].org;
		if(fabs(prjVecStart*planes[i].norm)<tolerance&&fabs(prjVecEnd*planes[i].norm)<tolerance)
			inplanes[i]=true;
	}
	if(inplanes[0]&&inplanes[2])
		return '2';	//ͬʱ��'Q'��'Z'��ʱ��ʾ��2�����޽�����
	else if(inplanes[0]&&inplanes[3])
		return '1';	//ͬʱ��'Q'��'Y'��ʱ��ʾ��2�����޽�����
	else if(inplanes[1]&&inplanes[2])
		return '4';	//ͬʱ��'H'��'Z'��ʱ��ʾ��4�����޽�����
	else if(inplanes[1]&&inplanes[3])
		return '3';	//ͬʱ��'H'��'Y'��ʱ��ʾ��3�����޽�����
	else
	{
		for(int j=0;j<4;j++){
			if(inplanes[j])
				return planes[j].cThirdLayer;
		}
		return 'G';
	}
}
bool CUnifyModelLayer::SetRodRevision(long hRod,DWORD dwFlagType)
{
	BITFLAG* pRevFlag=hashObjRevision.Add(hRod);
	if(pRevFlag==NULL)
		return false;
	pRevFlag->SetFlagDword(dwFlagType);
	return true;
}
bool CUnifyModelLayer::IsRodRevision(long hRod,DWORD dwFlagType)
{
	BITFLAG* pRevFlag=hashObjRevision.GetValue(hRod);
	if(pRevFlag==NULL)
		return false;
	return pRevFlag->IsHasFlag(dwFlagType)==TRUE;
}
bool CUnifyModelLayer::SetNodeRevision(long hNode,DWORD dwFlagType)
{
	BITFLAG* pRevFlag=hashObjRevision.Add(hNode);
	if(pRevFlag==NULL)
		return false;
	pRevFlag->SetFlagDword(dwFlagType);
	return true;
}
bool CUnifyModelLayer::IsNodeRevision(long hNode,DWORD dwFlagType)
{
	BITFLAG* pRevFlag=hashObjRevision.GetValue(hNode);
	if(pRevFlag==NULL)
		return false;
	return pRevFlag->IsHasFlag(dwFlagType)==TRUE;
}
bool CUnifyModelLayer::UnifySlopeObjLayer(CLDSNode* pSlopeBtmQuadNode,CLDSLinePart* pSlopeBtmQuadRod,double* pBodyTopZ,bool bCorrectObjLayer/*=true*/)
{
	double fBodyTopZ;
	CLDSNode *pNode,*pSlopeTopQuadNode,*pTopNode=NULL;
	CLDSLinePart* pRod,*pTopRod=NULL;
	bool bToTowerHead=false;
	if(pSlopeBtmQuadNode==NULL)
	{
		if(!FindQuad1LowestBaseNode(&pSlopeBtmQuadNode,&pSlopeBtmQuadRod))
			return false;
		if(bCorrectObjLayer)
			pSlopeBtmQuadNode->layer(0)=pSlopeBtmQuadRod->layer(0)='L';
	}
	else
	{
		if(pSlopeBtmQuadRod==NULL)
			pSlopeBtmQuadRod =FindSlopeRodByBaseNode(pSlopeBtmQuadNode);
		if(pSlopeBtmQuadRod!=NULL&&pSlopeBtmQuadRod->pStart!=NULL&&pSlopeBtmQuadRod->pEnd!=NULL)
		{
			GEPOINT upward_vec=pSlopeBtmQuadRod->pEnd->Position(false)-pSlopeBtmQuadRod->pStart->Position(false);
			LINEPARTSET rodset;
			bool bStartTipIsStartNode,bEndTipIsStartNode;
			pTopNode=upward_vec.z>0?pSlopeBtmQuadRod->pStart:pSlopeBtmQuadRod->pEnd;
			if(Ta.SearchSortedColinearRods(pSlopeBtmQuadRod,rodset,&bStartTipIsStartNode,&bEndTipIsStartNode,xTolerance.distance,xTolerance.tangent))
			{
				if(upward_vec.z>0)
				{	//pSlopeBtmQuadRodʼ->���������£�ʼ��Ϊ����
					pTopRod=rodset.GetFirst();
					pTopNode=bStartTipIsStartNode?pTopRod->pStart:pTopRod->pEnd;
				}
				else
				{	//pSlopeBtmQuadRodʼ->���������ϣ��ն�Ϊ����
					pTopRod=rodset.GetTail();
					pTopNode=bEndTipIsStartNode?pTopRod->pStart:pTopRod->pEnd;
				}
			}
			if(pTopNode->m_cPosCalType!=0&&pTopNode->m_cPosCalType!=CLDSNode::COORD_Z_SPECIFIED)
			{
				pTopNode=NULL;
				pTopRod=NULL;
				bToTowerHead=true;	//���´��ڵ�����һ��������������ʽ��
			}
			else
			{
				if(upward_vec.z>0)	//��upward_vecΪ��ǰ���½ڵ��������������� wjh-2019.7.21
					upward_vec*=-1.0;
				//���տڱ��ſ�(���¶ȴ���k0)ʱ�Ĺ�����Ϊƿ��,֮��Ϊͷ��,���ڸ�������γ��ڽṹ����ԭ���ڱ�֤��Ƥ��ֱ����£�
				//�����¶ȿ��ܴ�����΢���ڣ����¶���Ҫ�趨һ�ݲ���ֵ(k0=0.15) wjh-2015.3.21
				//���������������¶Ƚϴ��������ʱӦ�������ı������������죨����X>0,��Z<0,��X/��Z<-0.15) wjh-2019.8.8
				//if(fabs(upward_vec.z)>0&&fabs(upward_vec.x/upward_vec.z)>0.15&&fabs(upward_vec.y/upward_vec.z)>0.15)
				//���һ���˼���һ�����������������࣬ʶ��Ϊ�ᵣ����ͷ�˼�
				//��������˼���X��Y�������죬ͬʱ��������������������ʶ��Ϊ�ᵣ����ͷ�˼� wjh-2019.8.8
				double dfGradX2Z=upward_vec.x/upward_vec.z;
				double dfGradY2Z=upward_vec.y/upward_vec.z;
				if( fabs(upward_vec.z)>0&&(
					dfGradX2Z<-0.15||dfGradY2Z<-0.15||
					(dfGradX2Z<=0&&dfGradY2Z<=0&&fabs(dfGradX2Z-dfGradY2Z)>0.15)))
					bToTowerHead=true;
			}
		}
		if(!bToTowerHead)
		{
			if(m_bKeepMechanicalLayer&&bCorrectObjLayer)
				pSlopeBtmQuadNode->chLayer1st='S';
			if(pSlopeBtmQuadRod!=NULL)
			{
				if(m_bKeepMechanicalLayer&&bCorrectObjLayer)
					pSlopeBtmQuadRod->chLayer1st='S';
			}
			else	//���������������
				return true;
		}
		else
		{	//�ҵ���һ��ͷ��
			double separtor_z=pSlopeBtmQuadNode->Position('Z',false);
			for (pNode = Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				if(pNode->IsLegObj())
					continue;
				if(pNode->Position('Z',false)<separtor_z)
				{
					if(m_bKeepMechanicalLayer&&bCorrectObjLayer)
						pNode->chLayer1st='T';
				}
			}
			for (pRod = Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
			{
				if(pRod==NULL||pRod->pEnd==NULL||pRod->pStart==NULL||pRod->IsLegObj())
					continue;
				if(pRod->pStart->Position('Z',false)<separtor_z||pRod->pEnd->Position('Z',false)<separtor_z)
				{
					if(m_bKeepMechanicalLayer&&bCorrectObjLayer)
						pRod->chLayer1st = 'T';
				}
			}
			return true;
		}
	}
	if(bCorrectObjLayer)
	{
		if(m_bKeepMechanicalLayer)
			pSlopeBtmQuadRod->chLayer1st=pSlopeBtmQuadNode->chLayer1st;
		if(m_bKeepMechanicalLayer)
			pSlopeBtmQuadRod->chLayer2nd='Z';
		if(m_bKeepMechanicalLayer)
			pSlopeBtmQuadRod->chLayer3rd='1';
		SetRodRevision(pSlopeBtmQuadRod->handle,REVISION::LAYER);
	}
	UpdateSlopeRodMirLayer(pSlopeBtmQuadRod);
	SLOPE* pSlopeLine=listSlopes.GetTail();
	//�����ж���������ԭ��pSlopeBtmQuadNodeΪ�Ȳ��ڵ㣬���Ȳ�б�Ĺ��������ԭ����������ߵ㼴Ϊ������ƽ�档֮��ȫ���Ľڵ�˼���Ϊ��
	if(pSlopeBtmQuadNode->layer(0)=='L')
	{	//��ǰ�¶�Ϊ�����¶�
		CHashSet<CLDSLinePart*> hashLegXiePart;
		CHashSet<CLDSLinePart*> hashLegPostRods;
		//TODO:δ���Ƕ���߶������� wjh-2015.8.03
		fBodyTopZ=FindLegTopZ(hashLegXiePart,pSlopeBtmQuadNode, pSlopeBtmQuadRod);
		if(hashLegXiePart.GetNodeNum()==0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Gradient primary rod link to the lowest base node not found, layer revision failed!");
#else
			logerr.Log("δ�ҵ���������ͻ����ڵ��ϵ�����б�ģ�ͼ��������ʧ��!");
#endif
			return false;
		}
		if(pBodyTopZ)
			*pBodyTopZ=fBodyTopZ;
		double dist2top=0;
		pTopRod=NULL;
		CLDSNode *pLegTopNode=FindLegTopNodeByLegTopZ(fBodyTopZ,hashLegPostRods,pSlopeBtmQuadNode,pSlopeBtmQuadRod,&pTopRod);
		if(pSlopeLine!=NULL&&pSlopeLine->TestInSlopeLine(pLegTopNode->Position(false),&dist2top,xTolerance.distance))
		{
			if(dist2top>0)
			{
				pSlopeLine->top=pLegTopNode->Position(false);
				pSlopeLine->pTopRod=pTopRod;
				if(pTopRod->pEnd==pLegTopNode)
					pSlopeLine->bTopOnStart=false;
				else
					pSlopeLine->bTopOnStart=true;
			}
		}
		else
		{
			pSlopeLine=listSlopes.append();
			*pSlopeLine=SLOPE(pLegTopNode->Position(false),pSlopeBtmQuadNode->Position(false));
			pSlopeLine->pBtmRod=pSlopeBtmQuadRod;
			if(pSlopeBtmQuadRod->pEnd->Position('Z')>pSlopeBtmQuadRod->pStart->Position('Z'))
				pSlopeLine->bBtmOnStart=false;
			else
				pSlopeLine->bBtmOnStart=true;
			pSlopeLine->pTopRod=pTopRod;
			if(pTopRod->pEnd==pLegTopNode)
				pSlopeLine->bTopOnStart=false;
			else
				pSlopeLine->bTopOnStart=true;
		}
		if(m_bKeepMechanicalLayer)
			pLegTopNode->layer(0)='S';//���ȶ����ڵ��Ϊ����ڵ�
		GetMirHashTable(hashLegXiePart);
		GetMirRotateHashTable(hashLegXiePart);
		for(pRod=hashLegPostRods.GetFirst();pRod;pRod=hashLegPostRods.GetNext())
		{
			if(m_bKeepMechanicalLayer&&bCorrectObjLayer)
				pRod->layer(0)='L';
			if(m_bKeepMechanicalLayer&&bCorrectObjLayer)
				pRod->layer(1)='Z';
			char cQuad=CalPartPos(pRod);
			if(isdigit(cQuad)&&m_bKeepMechanicalLayer)
				pRod->ciLegQuad=cQuad;
				//pRod->layer(2)=cQuad;
		}
		f3dPoint btmpos=pSlopeBtmQuadNode->Position(false);
		f3dPoint toppos=pLegTopNode->Position(false);
		pSlopeLine->taper.VerifyVertex(btmpos);
		pSlopeLine->taper.VerifyVertex(toppos);
		btmpos.x*=-1;
		btmpos.y*=-1;
		toppos.x*=-1;
		toppos.y*=-1;
		pSlopeLine->taper.VerifyVertex(btmpos);
		pSlopeLine->taper.VerifyVertex(toppos);
		pSlopeLine->InitTaperSlopePlanes();
		for(pRod=hashLegXiePart.GetFirst();pRod;pRod=hashLegXiePart.GetNext())
		{
			if(m_bKeepMechanicalLayer&&bCorrectObjLayer)
				pRod->layer(0)='L';
			if(m_bKeepMechanicalLayer&&bCorrectObjLayer)
				pRod->layer(1)='X';
			if(isdigit(pRod->layer(2)))
			{
				char cQuad=CalPartPos(pRod);
				if(isdigit(cQuad)&&m_bKeepMechanicalLayer)
					pRod->ciLegQuad=cQuad;
					//pRod->layer(2)=cQuad;
			}
			else if(m_bKeepMechanicalLayer)
				pRod->ciLegQuad=pSlopeLine->IdentifyLineTaperSlopeLayer(pRod,xTolerance.distance);
				//pRod->layer(2)=pSlopeLine->IdentifyLineTaperSlopeLayer(pRod);
		}
		for (pNode = Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(!pSlopeBtmQuadNode->ModuleTogetherWith(pNode))
				continue;
			if( pNode->Position('Z',false)>fBodyTopZ+EPS&&bCorrectObjLayer)
			{
				if(m_bKeepMechanicalLayer)
					pNode->layer(0) = 'L';
				pNode->layer(1) = 'P';
				if(m_bKeepMechanicalLayer&&isdigit(pNode->layer(2)))
				{
					GEPOINT pos=pNode->Position(false);
					if(pos.x>0&&pos.y>0)
						pNode->ciLegQuad='1';
					else if(pos.x<0&&pos.y>0)
						pNode->ciLegQuad='2';
					else if(pos.x>0&&pos.y<0)
						pNode->ciLegQuad='3';
					else if(pos.x<0&&pos.y<0)
						pNode->ciLegQuad='4';
					else if(pos.x>0)
						pNode->chLayer3rd='Y';
					else if(pos.x<0)
						pNode->chLayer3rd='Z';
					else if(pos.y>0)
						pNode->chLayer3rd='Q';
					else if(pos.y<0)
						pNode->chLayer3rd='H';
					else
						pNode->chLayer3rd='G';
				}
				else if(m_bKeepMechanicalLayer)
					pNode->chLayer3rd=pSlopeLine->IdentifyPointTaperSlopeLayer(pNode->Position(false),xTolerance.distance);
			}
		}
		for (pRod = Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
		{
			if(pRod==NULL||pRod->pEnd==NULL||pRod->pStart==NULL)
				continue;
			if(!pSlopeBtmQuadNode->ModuleTogetherWith(pRod)||
				hashLegPostRods.GetValue(pRod->handle)||hashLegXiePart.GetValue(pRod->handle))
				continue;
			if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0x20)
			{
				long hGroupAngle=((CLDSLineAngle*)pRod)->group_father_jg_h;
				CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)hashLegPostRods.GetValue(hGroupAngle);
				if(pGroupAngle!=NULL)
				{
					pRod->SetLayer(pGroupAngle->layer());
					continue;
				}
			}
			if((pRod->pEnd->Position('Z',false)>fBodyTopZ+EPS||pRod->pStart->Position('Z',false)>fBodyTopZ+EPS)&&bCorrectObjLayer)
			{
				if(m_bKeepMechanicalLayer)
					pRod->layer(0) = 'L';
				if(m_bKeepMechanicalLayer)
					pRod->layer(1) = 'F';
				if(m_bKeepMechanicalLayer&&isdigit(pRod->layer(2)))
				{
					char cQuad=CalPartPos(pRod);
					if(isdigit(cQuad))
						pRod->layer(2)=cQuad;
				}
				else if(m_bKeepMechanicalLayer)
					pRod->layer(2)=pSlopeLine->IdentifyLineTaperSlopeLayer(pRod,xTolerance.distance);
			}
		}
		//if(fBodyTopZ>=pSlopeBtmQuadRod->pEnd->Position('Z',false)||fBodyTopZ>=pSlopeBtmQuadRod->pStart->Position('Z',false))
		//{
		//	if(pSlopeBtmQuadRod->pEnd==pSlopeBtmQuadNode)
		//		return UnifySlopeObjLayer(pSlopeBtmQuadRod->pStart,NULL,pBodyTopZ);
		//	else if(pSlopeBtmQuadRod->pStart==pSlopeBtmQuadNode)
		//		return UnifySlopeObjLayer(pSlopeBtmQuadRod->pEnd,NULL,pBodyTopZ);
		//}
		//else
			return UnifySlopeObjLayer(pLegTopNode,NULL,pBodyTopZ);
	}
	else
	{
		//����pSlopeBtmQuadNode��pSlopeBtmQuadRod����slopeQuadLineVec(�¶�1���޽��ߵķ�λ��
		//���ݸ˼��ڵ���slopeQuadLineVec���ߵ�ԭ�򣬲��ҵ�ǰ�¶�1���޽����ϵ����и˼��ڵ㣬����Ϊ����1����"S*1"��Ҫ��pSlopeBtmQuadNode->ModuleTogetherWith��pObj��==true)
		CHashSet<CLDSNode*>hashSlopeNode;
		CHashSet<CLDSLinePart*>hashSlopePostRods;
		hashSlopePostRods.SetValue(pSlopeBtmQuadRod->handle,pSlopeBtmQuadRod);
		if(pTopRod==NULL||pTopNode==NULL)
			GetSlopeTopNode(hashSlopePostRods,pSlopeBtmQuadNode,pSlopeBtmQuadRod,&pSlopeTopQuadNode,&pTopRod,&xTolerance);
		else
		{
			pSlopeTopQuadNode=pTopNode;
			//��ʼ����ǰ�¶ν����ϵ��Ҹ˼���
			CPtInLineCheck check(pTopNode->xOriginalPos,pSlopeBtmQuadNode->xOriginalPos);
			for (pRod = Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->pStart==NULL||pRod->pEnd==NULL)
					continue;
				if( pRod->pStart->xOriginalPos.z<pTopNode->xOriginalPos.z-100||pRod->pStart->xOriginalPos.z>pSlopeBtmQuadNode->xOriginalPos.z+100||
					pRod->pEnd->xOriginalPos.z  <pTopNode->xOriginalPos.z-100||pRod->pEnd->xOriginalPos.z  >pSlopeBtmQuadNode->xOriginalPos.z+100)
					continue;
				if(hashSlopePostRods.GetValue(pRod->handle))
					continue;
				bool bOnLine=false;
				check.CheckPointEx(pRod->pStart->xOriginalPos,&bOnLine,xTolerance.distance,xTolerance.tangent);
				if(!bOnLine)	//������
					continue;
				check.CheckPointEx(pRod->pEnd->xOriginalPos,&bOnLine,xTolerance.distance,xTolerance.tangent);
				if(!bOnLine)	//������
					continue;
				hashSlopePostRods.SetValue(pRod->handle,pRod);
			}
		}
		if(pSlopeBtmQuadNode==NULL||pSlopeTopQuadNode==NULL)
			return false;
		double dist2top=0;
		if(pSlopeLine!=NULL&&pSlopeLine->TestInSlopeLine(pSlopeTopQuadNode->Position(false),&dist2top,xTolerance.distance))
		{
			if(dist2top>0)
			{
				pSlopeLine->top=pSlopeTopQuadNode->Position(false);
				if(pTopRod==NULL)
					logerr.Log("�����¶ν����Ҳ��������ĸ˼����!");
				else
					pSlopeLine->pTopRod=pTopRod;
				if(pTopRod->pEnd==pSlopeTopQuadNode)
					pSlopeLine->bTopOnStart=false;
				else
					pSlopeLine->bTopOnStart=true;
			}
		}
		else
		{
			pSlopeLine=listSlopes.append();
			*pSlopeLine=SLOPE(pSlopeTopQuadNode->Position(false),pSlopeBtmQuadNode->Position(false));
			if(pTopRod==NULL)
				logerr.Log("�����¶ν����Ҳ��������ĸ˼����!");
			else
				pSlopeLine->pTopRod=pTopRod;
			if(pTopRod->pEnd==pSlopeTopQuadNode)
				pSlopeLine->bTopOnStart=false;
			else
				pSlopeLine->bTopOnStart=true;
			pSlopeLine->pBtmRod=pSlopeBtmQuadRod;
			if(pSlopeBtmQuadRod->pEnd->Position('Z')>pSlopeBtmQuadRod->pStart->Position('Z'))
				pSlopeLine->bBtmOnStart=false;
			else
				pSlopeLine->bBtmOnStart=true;
		}
		GetMirHashTable(hashSlopePostRods);
		//���ݶԳƿɻ�ȡ��ǰ�¶ε�����̨�Ķ������
		f3dPoint btmpos=pSlopeBtmQuadNode->Position(false);
		f3dPoint toppos=pSlopeTopQuadNode->Position(false);
		pSlopeLine->taper.VerifyVertex(btmpos);
		pSlopeLine->taper.VerifyVertex(toppos);
		btmpos.x*=-1;
		btmpos.y*=-1;
		toppos.x*=-1;
		toppos.y*=-1;
		pSlopeLine->taper.VerifyVertex(btmpos);
		pSlopeLine->taper.VerifyVertex(toppos);
		pSlopeLine->InitTaperSlopePlanes();
		//��Zֵλ������̨֮�䣬ͬʱ��λ������̨�ĸ���֮��Ķ�����Ϊͷ������
		//����λ������̨�ĸ�����֮�ϵĸ˼��ڵ㣨������֮�⣩���ֱ���Ϊ'Q,H,Z,Y'(���ȳ��⣩
		double fBodyDownZ =pSlopeBtmQuadNode->Position('Z',false);
		fBodyTopZ = pSlopeTopQuadNode->Position('Z',false);
		if(pBodyTopZ)
			*pBodyTopZ=fBodyTopZ;
		InitHashSlopeNode(hashSlopeNode,pSlopeTopQuadNode,pSlopeBtmQuadNode);
		if(bCorrectObjLayer)
		{
			for (pNode = Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
			{
				double posZ=pNode->Position('Z',false);
				if( posZ<fBodyTopZ||posZ>fBodyDownZ)		//�ն˽ڵ�Z���겻������̨֮��
					continue;
				if(pNode->IsLegObj())
					continue;	//���ȴ��ڵ������Ƕ�ȡ�����ļ��ж����ùʲ���Ҫ�ٵ����϶��Ƿ�Ϊͷ����
								//������п�����Ϊģ���нڵ����ݲ���ȷ������������ڵ�Ϊ��ͷ wjh-2018.7.7
				//λ������̨�����ͷ��
				if(!pSlopeLine->taper.IsIncludePoint(pNode->Position(false),xTolerance.distance))
					pNode->layer(0)='T';
				else if(pNode->Layer(0)!='L'||!m_bKeepMechanicalLayer)
					pNode->layer(0)='S';
			}
		}
		for (pRod = Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->pEnd==NULL||pRod->pStart==NULL)
				continue;
			double fEndZ=pRod->pEnd->Position('Z',false),fStartZ=pRod->pStart->Position('Z',false);
			if( fEndZ<fBodyTopZ||fEndZ>fBodyDownZ||			//ʼ�˽ڵ�Z���겻������̨֮��
				fStartZ<fBodyTopZ||fStartZ>fBodyDownZ)		//�ն˽ڵ�Z���겻������̨֮��
				continue;
			if(!bCorrectObjLayer)
				continue;
			//���ȴ��˼������Ƕ�ȡ�����ļ��ж����ùʲ���Ҫ�ٵ����϶��Ƿ�Ϊͷ����
			//������п�����Ϊģ���нڵ����ݲ���ȷ������������ڵ�Ϊ��ͷ wjh-2018.7.7
			if(pRod->IsLegObj())
				continue;
			//λ������̨�����ͷ��
			if(!pSlopeLine->taper.IsIncludePoint(pRod->pEnd->Position(false),xTolerance.distance)||!pSlopeLine->taper.IsIncludePoint(pRod->pStart->Position(false),xTolerance.distance))
				pRod->layer(0)='T';
			else if(pRod->Layer(0)!='L'||!m_bKeepMechanicalLayer)
			{
				pRod->layer(0)='S';
				long hGroupAngle=0;
				if(pRod->GetClassTypeId()==CLS_LINEANGLE)
					hGroupAngle=((CLDSLineAngle*)pRod)->group_father_jg_h;
				if(hashSlopePostRods.GetValue(pRod->handle)||(hGroupAngle>0x20&&hashSlopePostRods.GetValue(hGroupAngle)))
				{
					pRod->layer(1)='Z';
					continue;
				}
				else if(hashSlopeNode.GetValue(pRod->pStart->handle)&&hashSlopeNode.GetValue(pRod->pEnd->handle))
				{
					CLDSNode *pMirNode =(CLDSNode*)pRod->pStart->GetMirNode(3);
					if(fabs(pRod->pStart->xOriginalPos.z-pRod->pEnd->xOriginalPos.z)>EPS||pMirNode==pRod->pEnd)
						pRod->layer(1)='X';
					else
						pRod->layer(1)='H';
				}
			}
		}

		return UnifySlopeObjLayer(pSlopeTopQuadNode,NULL,pBodyTopZ);
	}
	return true;
}
void CUnifyModelLayer::UnifyTrunkObjPosLayer()
{
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->Layer(0)!='S'||pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;	//����������˼�Ĭ��ͼ����Ϊ'G'
		pRod->layer(2)='G';
	}
	//��������ڵ�λ��ͼ���ַ�
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->Layer(0)=='T')
			continue;	//�����������Ȳ����޽ǽڵ�
		GEPOINT pos=pNode->Position(false);
		GEPOINT posQuad(fabs(pos.x),fabs(pos.y),pos.z);
		for(SLOPE* pSlope=listSlopes.GetFirst();pSlope;pSlope=listSlopes.GetNext())
		{
			if(pos.z<pSlope->top.z||pos.z>pSlope->bottom.z)
				continue;	//�ǵ�ǰ�¶�
			pNode->chLayer3rd=pSlope->IdentifyPointTaperSlopeLayer(pos,xTolerance.distance);
		}
	}
	for (CLDSLinePart* pRod = Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pEnd==NULL||pRod->pStart==NULL||pRod->Layer(0)=='T')
			continue;
		double fEndZ=pRod->pEnd->Position('Z',false),fStartZ=pRod->pStart->Position('Z',false);
		for(SLOPE* pSlope=listSlopes.GetFirst();pSlope;pSlope=listSlopes.GetNext())
		{
			double fBodyTopZ=pSlope->top.z;
			double fBodyDownZ=pSlope->bottom.z;
			if( fEndZ<fBodyTopZ||fEndZ>fBodyDownZ||fStartZ<fBodyTopZ||fStartZ>fBodyDownZ)
				continue;	//ʼ���ն˽ڵ�Z���겻������̨֮��
			if(pRod->pStart->Layer(2)==pRod->pEnd->Layer(2))
				pRod->layer(2)=pRod->pStart->Layer(2);
			else if(pRod->pStart->Layer(2)=='G'||pRod->pEnd->Layer(2)=='G')
				pRod->layer(2)='G';
			else
			{
				char ciPosQuadLayer=pRod->Layer(2);
				pRod->layer(2) = pSlope->IdentifyLineTaperSlopeLayer(pRod,xTolerance.distance);
				if(pRod->Layer(0)=='L'&&(ciPosQuadLayer>'0'&&ciPosQuadLayer<='6'))
					pRod->ciLegQuad=ciPosQuadLayer;
			}
		}
	}
}
//������ͷ&�ᵣ�˼���λ
bool CUnifyModelLayer::CorrectHeadArmObjPosLayerByWorkPlane()
{
	CUnifyModelLayer::WORKPLANE *pPlane;
	bool hasObjsCorrect=false;
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->Layer(0)!='T'||pNode->Layer(2)!='G')
			continue;
		for(pPlane=hashPlanes.GetFirst();pPlane;pPlane=hashPlanes.GetNext())
		{
			if(pPlane->IsPtOnPlane(pNode->Position(false)))
			{
				if((pNode->Layer(2)=='Q'&&pPlane->cThirdLayer=='Y')||(pNode->Layer(2)=='Y'&&pPlane->cThirdLayer=='Q'))
					pNode->chLayer3rd='1';
				else if((pNode->Layer(2)=='Q'&&pPlane->cThirdLayer=='Z')||(pNode->Layer(2)=='Z'&&pPlane->cThirdLayer=='Q'))
					pNode->chLayer3rd='2';
				else if((pNode->Layer(2)=='H'&&pPlane->cThirdLayer=='Y')||(pNode->Layer(2)=='Y'&&pPlane->cThirdLayer=='H'))
					pNode->chLayer3rd='3';
				else if((pNode->Layer(2)=='H'&&pPlane->cThirdLayer=='Z')||(pNode->Layer(2)=='Z'&&pPlane->cThirdLayer=='H'))
					pNode->chLayer3rd='4';
				else
					pNode->chLayer3rd=pPlane->cThirdLayer;
				pNode->chLayer3rd=pPlane->cThirdLayer;
				hasObjsCorrect=true;
			}
		}
	}
	for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->Layer(0)!='T'||pRod->pStart==NULL||pRod->pEnd==NULL||pRod->Layer(2)!='G')
			continue;
		else if(pRod->pStart->Layer(0)!='T'&&pRod->pEnd->Layer(0)!='T')
			continue;
		for(pPlane=hashPlanes.GetFirst();pPlane;pPlane=hashPlanes.GetNext())
		{
			if(pPlane->IsPtOnPlane(pRod->pStart->Position(false))&&pPlane->IsPtOnPlane(pRod->pEnd->Position(false)))
			{
				if((pRod->Layer(2)=='Q'&&pPlane->cThirdLayer=='Y')||(pRod->Layer(2)=='Y'&&pPlane->cThirdLayer=='Q'))
					pRod->layer(2)='1';
				else if((pRod->Layer(2)=='Q'&&pPlane->cThirdLayer=='Z')||(pRod->Layer(2)=='Z'&&pPlane->cThirdLayer=='Q'))
					pRod->layer(2)='2';
				else if((pRod->Layer(2)=='H'&&pPlane->cThirdLayer=='Y')||(pRod->Layer(2)=='Y'&&pPlane->cThirdLayer=='H'))
					pRod->layer(2)='3';
				else if((pRod->Layer(2)=='H'&&pPlane->cThirdLayer=='Z')||(pRod->Layer(2)=='Z'&&pPlane->cThirdLayer=='H'))
					pRod->layer(2)='4';
				else
					pRod->layer(2)=pPlane->cThirdLayer;
				break;
			}
		}
		if(pPlane)
			continue;
		if( pRod->pStart->Layer(2)==pRod->pEnd->Layer(2))
		{
			pRod->layer(2)=pRod->pStart->Layer(2);
			hasObjsCorrect=true;
		}
		else
		{
			char cOldPosLayer=pRod->Layer(2);
			if(pRod->pStart->Layer(0)!='T'&&pRod->pEnd->Layer(0)=='T')
				pRod->layer(2)=pRod->pEnd->Layer(2);
			else if(pRod->pStart->Layer(0)=='T'&&pRod->pEnd->Layer(0)!='T')
				pRod->layer(2)=pRod->pStart->Layer(2);
			if(pRod->Layer(2)=='Q'&&(pRod->pStart->Position('Y',false)<=0||pRod->pEnd->Position('Y',false)<=0))
				pRod->layer(2)='G';
			else if(pRod->Layer(2)=='H'&&(pRod->pStart->Position('Y',false)>=0||pRod->pEnd->Position('Y',false)>=0))
				pRod->layer(2)='G';
			else if(pRod->Layer(2)=='Y'&&(pRod->pStart->Position('X',false)<=0||pRod->pEnd->Position('X',false)<=0))
				pRod->layer(2)='G';
			else if(pRod->Layer(2)=='Z'&&(pRod->pStart->Position('X',false)>=0||pRod->pEnd->Position('X',false)>=0))
				pRod->layer(2)='G';
			if(cOldPosLayer!=pRod->Layer(2))
				hasObjsCorrect=true;
		}
	}
	return hasObjsCorrect;
}
void CUnifyModelLayer::UnifyHeadArmObjPosLayer()
{
	//һ�����ӵ����������Ͻڵ㣬�����˼�ΪX���Y��Գƣ����ȷ��һ������
	CLDSNode* pNode;
	CLDSLinePart* pRod;
	CUnifyModelLayer::SLOPE* pSlope;
	CUnifyModelLayer::SLOPE* pTopSlope=listSlopes.GetTail();
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->Layer(0)=='T')
			pNode->layer(2)='G';
	}
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->Layer(0)=='T')
			pRod->layer(2)='G';
		if(pRod->pStart&&pRod->pStart->Layer(0)=='T')
			pRod->pStart->layer(2)='G';
		if(pRod->pEnd&&pRod->pEnd->Layer(0)=='T')
			pRod->pEnd->layer(2)='G';
	}
	for(pSlope=listSlopes.GetFirst();pSlope;pSlope=listSlopes.GetNext())
	{
		GEPOINT lenStdVecQuad2(-pSlope->lenStdVec.x, pSlope->lenStdVec.y,pSlope->lenStdVec.z);
		GEPOINT lenStdVecQuad3( pSlope->lenStdVec.x,-pSlope->lenStdVec.y,pSlope->lenStdVec.z);
		GEPOINT lenStdVecQuad4(-pSlope->lenStdVec.x,-pSlope->lenStdVec.y,pSlope->lenStdVec.z);
		for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->Layer(0)!='T'||pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			GEPOINT start=pRod->pStart->Position(false);
			GEPOINT end  =pRod->pEnd->Position(false);
			int head1_sidearm2=0;
			if(start.z>=pSlope->top.z&&start.z<=pSlope->bottom.z&&end.z>=pSlope->top.z&&end.z<=pSlope->bottom.z)
			{	//ͷ���˼�λ�������¶�ˮƽ����,���Ա����Ϊ����ᵣ
				head1_sidearm2=2;
			}
			else if(pSlope==pTopSlope&&start.z<=pSlope->top.z&&end.z<=pSlope->top.z)
			{	//ͷ���˼�λ���������¶εĶ�������,���Ա����Ϊèͷ�Ʊ�������ͷ������
				head1_sidearm2=1;
			}
			else
				continue;
			CLDSNode* pPostNode=NULL,*pExtendNode=NULL;
			start.x=fabs(start.x);
			start.y=fabs(start.y);
			end.x=fabs(end.x);
			end.y=fabs(end.y);
			if(pSlope->TestInSlopeLine(start,NULL,xTolerance.distance))
				pPostNode=pRod->pStart;
			else
				pExtendNode=pRod->pStart;
			if(pSlope->TestInSlopeLine(end,NULL,xTolerance.distance))
				pPostNode=pRod->pEnd;
			else
				pExtendNode=pRod->pEnd;
			if(pPostNode==NULL||pExtendNode==NULL)
				continue;	//������������ֱ�����������ĸ˼�
			GEPOINT extend_vec=pExtendNode->Position(false)-pPostNode->Position(false);
			CUnifyModelLayer::WORKPLANE workplane;
			workplane.org=pPostNode->Position(false);
			if(head1_sidearm2==1)
			{
				bool frontrod=false;
				if(pPostNode->Position('Y',false)>0&&pExtendNode->Position('Y',false)>0)
				{	//�˼���ǰ��
					workplane.cThirdLayer='Q';
					workplane.norm.Set(0,-extend_vec.z,extend_vec.y);//GEPOINT(1,0,0)^extend_vec;
				}
				else if(pPostNode->Position('Y',false)<0&&pExtendNode->Position('Y',false)<0)
				{	//�˼��ں��
					workplane.cThirdLayer='H';
					workplane.norm.Set(0,extend_vec.z,-extend_vec.y);//GEPOINT(1,0,0)^extend_vec;
				}
				if(!workplane.norm.IsZero())	//���ǰ������(����)
				{
					frontrod=true;
					pRod->layer(2)=workplane.cThirdLayer;
					AppendUniqueWorkPlane(workplane);
				}
				workplane.norm.Set();
				if(pPostNode->Position('X',false)>0&&pExtendNode->Position('X',false)>0)
				{	//�˼����Ҳ�
					workplane.cThirdLayer='Y';
					workplane.norm.Set(-extend_vec.z,0,extend_vec.x);//GEPOINT(0,-1,0)^extend_vec;
				}
				else if(pPostNode->Position('X',false)<0&&pExtendNode->Position('X',false)<0)
				{	//�˼������
					workplane.cThirdLayer='Z';
					workplane.norm.Set(extend_vec.z,0,-extend_vec.x);//GEPOINT(0,1,0)^extend_vec;
				}
				if(!workplane.norm.IsZero())	//������ҹ�����(����)
				{
					if(frontrod)
					{
						if(pRod->Layer(2)=='Q'&&workplane.cThirdLayer=='Y')
							pRod->layer(2)='1';
						else if(pRod->Layer(2)=='Q'&&workplane.cThirdLayer=='Z')
							pRod->layer(2)='2';
						else if(pRod->Layer(2)=='H'&&workplane.cThirdLayer=='Y')
							pRod->layer(2)='3';
						else if(pRod->Layer(2)=='H'&&workplane.cThirdLayer=='Z')
							pRod->layer(2)='4';
						else
							pRod->layer(2)=workplane.cThirdLayer;
					}
					else
						pRod->layer(2)=workplane.cThirdLayer;
					AppendUniqueWorkPlane(workplane);
				}
			}
			else //if(head1_sidearm2==2)
			{
				workplane.norm.Set();
				if(fabs(extend_vec.x)>=fabs(extend_vec.y))	//X��ᵣ
				{
					if(pPostNode->Position('Y',false)>0&&pExtendNode->Position('Y',false)>0)
					{
						workplane.cThirdLayer='Q';
						if(extend_vec.x>0&&pPostNode->Position('X',false)>0)
							workplane.norm=extend_vec^pSlope->lenStdVec;	//ǰ���Ҳ�
						else if(extend_vec.x<0&&pPostNode->Position('X',false)<0)
							workplane.norm=lenStdVecQuad2^extend_vec;	//ǰ�����
					}
					else if(pPostNode->Position('Y',false)<0&&pExtendNode->Position('Y',false)<0)
					{
						workplane.cThirdLayer='H';
						if(extend_vec.x>0&&pPostNode->Position('X',false)>0)
							workplane.norm=lenStdVecQuad3^extend_vec;	//�����Ҳ�
						else if(extend_vec.x<0&&pPostNode->Position('X',false)<0)
							workplane.norm=extend_vec^lenStdVecQuad4;	//�������
					}
					if(!workplane.norm.IsZero())	//������ҹ�����(����)
					{
						pRod->layer(2)=workplane.cThirdLayer;
						AppendUniqueWorkPlane(workplane);
					}
				}
				else if(fabs(extend_vec.x)<fabs(extend_vec.y))	//Y��ᵣ
				{
					if(pPostNode->Position('X',false)>0&&pExtendNode->Position('X',false)>0)
					{
						workplane.cThirdLayer='Y';
						if(extend_vec.y>0&&pPostNode->Position('Y',false)>0)
							workplane.norm=pSlope->lenStdVec^extend_vec;//ǰ���Ҳ�
						else if(extend_vec.y<0&&pPostNode->Position('Y',false)<0)
							workplane.norm=extend_vec^lenStdVecQuad3;	//�����Ҳ�
					}
					else if(pPostNode->Position('X',false)<0&&pExtendNode->Position('X',false)<0)
					{
						workplane.cThirdLayer='Z';
						if(extend_vec.y>0&&pPostNode->Position('Y',false)>0)
							workplane.norm=extend_vec^lenStdVecQuad2;	//ǰ�����
						else if(extend_vec.y<0&&pPostNode->Position('Y',false)<0)
							workplane.norm=lenStdVecQuad4^extend_vec;	//�������
					}
					if(!workplane.norm.IsZero())	//������ҹ�����(����)
					{
						pRod->layer(2)=workplane.cThirdLayer;
						AppendUniqueWorkPlane(workplane);
					}
				}
			}
		}
	}
	for(int j=0;j<2;j++)
	{	//��Ӵ��������棨�ԽǸ�Ϊ������׼�Ĺ���ƽ�棩
		if(!CorrectHeadArmObjPosLayerByWorkPlane())
			break;
		for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->Layer(0)!='T' || pRod->Layer(1)!='Z' || pRod->pStart==NULL || pRod->pEnd==NULL)
				continue;
			long hStartDependRod=pRod->pStart->hFatherPart;
			long hEndDependRod=pRod->pEnd->hFatherPart;
			if(pRod->IsAngle())
			{
				CLDSGroupLineAngle* pGroupAngle=NULL;
				if(((CLDSLineAngle*)pRod)->group_father_jg_h>0x20)
					pGroupAngle=(CLDSGroupLineAngle*)Ta.Parts.FromHandle(((CLDSLineAngle*)pRod)->group_father_jg_h,CLS_GROUPLINEANGLE);	//���������Ǹ��ӽǸ�
				if(pGroupAngle)
					pRod=pGroupAngle;
				if(((CLDSLineAngle*)pRod)->desStartPos.datum_jg_h>0x20)
					hStartDependRod=((CLDSLineAngle*)pRod)->desStartPos.datum_jg_h;
				if(((CLDSLineAngle*)pRod)->desEndPos.datum_jg_h>0x20)
					hEndDependRod=((CLDSLineAngle*)pRod)->desEndPos.datum_jg_h;
			}
			WORKPLANE workplane;
			GEPOINT rodvec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
			CLDSLinePart* pParentRod=Ta.FromRodHandle(hStartDependRod);
			if(pParentRod&&(pParentRod->Layer(2)=='Q'||pParentRod->Layer(2)=='H')&&pParentRod->pStart&&pParentRod->pEnd&&!isdigit(pRod->pStart->Layer(2)))
			{
				GEPOINT dependvec=pParentRod->pEnd->Position(false)-pParentRod->pStart->Position(false);
				workplane.org=pRod->pStart->Position(false);
				workplane.norm=rodvec^dependvec;
				if(normalize(workplane.norm))
				{
					if(fabs(workplane.norm.x)<fabs(workplane.norm.y))
					{	//ǰ���湤����
						if(pRod->pStart->Position('Y',false)>0&&pRod->pEnd->Position('Y',false)>0)
							workplane.cThirdLayer='Q';
						else if(pRod->pStart->Position('Y',false)<0&&pRod->pEnd->Position('Y',false)<0)
							workplane.cThirdLayer='H';
						else
							workplane.cThirdLayer='G';
					}
					else
					{	//�����湤����
						if(pRod->pStart->Position('X',false)>0&&pRod->pEnd->Position('X',false)>0)
							workplane.cThirdLayer='Y';
						else if(pRod->pStart->Position('X',false)<0&&pRod->pEnd->Position('X',false)<0)
							workplane.cThirdLayer='Z';
						else
							workplane.cThirdLayer='G';
					}
					AppendUniqueWorkPlane(workplane);	//���ͷ������չ��������(����)
				}
			}
			pParentRod=Ta.FromRodHandle(hEndDependRod);
			if(pParentRod&&(pParentRod->Layer(2)=='Q'||pParentRod->Layer(2)=='H')&&pParentRod->pStart&&pParentRod->pEnd&&!isdigit(pRod->pEnd->Layer(2)))
			{
				GEPOINT dependvec=pParentRod->pEnd->Position(false)-pParentRod->pStart->Position(false);
				workplane.org=pRod->pEnd->Position(false);
				workplane.norm=rodvec^dependvec;
				if(normalize(workplane.norm))
				{
					if(fabs(workplane.norm.x)<fabs(workplane.norm.y))
					{	//ǰ���湤����
						if(pRod->pStart->Position('Y',false)>0&&pRod->pEnd->Position('Y',false)>0)
							workplane.cThirdLayer='Q';
						else if(pRod->pStart->Position('Y',false)<0&&pRod->pEnd->Position('Y',false)<0)
							workplane.cThirdLayer='H';
						else
							workplane.cThirdLayer='G';
					}
					else
					{	//�����湤����
						if(pRod->pStart->Position('X',false)>0&&pRod->pEnd->Position('X',false)>0)
							workplane.cThirdLayer='Y';
						else if(pRod->pStart->Position('X',false)<0&&pRod->pEnd->Position('X',false)<0)
							workplane.cThirdLayer='Z';
						else
							workplane.cThirdLayer='G';
					}
					AppendUniqueWorkPlane(workplane);	//���ͷ������չ��������(����)
				}
			}
		}
	}
}
//pReserveNode:�ϲ�����Ҫ�����Ľڵ�;pMergeNode:�ϲ�����Ҫɾ���Ľڵ� defined in OutputInfo.cpp
class COutputInfo;
bool MergeOverlapNodes(CLDSNode *pReserveNode,CLDSNode *pMergeNode,COutputInfo *pOutputInfo=NULL,bool updateRelaRodSolid=true);
UINT CUnifyModelLayer::MergeModelOverlapNodes()
{
	UINT i=0,hits=0,nCount=Ta.Node.GetNodeNum();
#ifdef __LDS_CONTEXT_
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		CAtomListStack<CLDSNode> stack(&Ta.Node);
		for(CLDSNode* pNextNode=Ta.Node.GetNext();pNextNode;pNextNode=Ta.Node.GetNext())
		{
			if(!pNode->ModuleTogetherWith(pNextNode))
				continue;
			if(pNode->xOriginalPos.IsEqual(pNextNode->xOriginalPos,0.1))
			{
				CAtomListStack<CLDSNode> stack2(&Ta.Node);
				if(MergeOverlapNodes(pNode,pNextNode,NULL,false))
					hits++;
			}
		}
	}
#endif
	return hits;
}
UINT CUnifyModelLayer::CorrectDependRodsOfIntersNode()
{
	CLDSNode* pNode;
	NODESET intersNodeSet;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->m_cPosCalType==4)	//����ڵ�
			intersNodeSet.append(pNode);
	}
	UINT i=0,hits=0,n=intersNodeSet.GetNodeNum();
	for(pNode=intersNodeSet.GetFirst();pNode;pNode=intersNodeSet.GetNext(),i++)
	{	//�����н���㣬����ʧ���ո˼������
		CLDSNode* pLine1Start=Ta.Node.FromHandle(pNode->arrRelationNode[0]);
		CLDSNode* pLine1End=Ta.Node.FromHandle(pNode->arrRelationNode[1]);
		CLDSNode* pLine2Start=Ta.Node.FromHandle(pNode->arrRelationNode[2]);
		CLDSNode* pLine2End=Ta.Node.FromHandle(pNode->arrRelationNode[3]);
		Ta.DisplayProcess(i*100/n,"���佻��ڵ������Ϣ");
		if(pLine1Start==NULL||pLine1End==NULL||pLine2Start==NULL||pLine2End==NULL)
			continue;
		CPtInLineCheck checkline1(pLine1Start->xOriginalPos,pLine1End->xOriginalPos);
		CPtInLineCheck checkline2(pLine2Start->xOriginalPos,pLine2End->xOriginalPos);
		CLDSLinePart* pRod1=pNode->arrRelationPole[0]>0x20?Ta.FromRodHandle(pNode->arrRelationPole[0]):NULL;
		CLDSLinePart* pRod2=pNode->arrRelationPole[1]>0x20?Ta.FromRodHandle(pNode->arrRelationPole[1]):NULL;
		bool bOnline=!(pRod1==NULL||pRod1->pStart==NULL||pRod1->pEnd==NULL);
		if(bOnline)
			checkline1.CheckPointEx(pRod1->pStart->xOriginalPos,&bOnline);
		if(bOnline)
			checkline1.CheckPointEx(pRod1->pEnd->xOriginalPos,&bOnline);
		pRod1=NULL;
		SCOPE_STRU scope;
		scope.VerifyVertex(pLine1Start->xOriginalPos);
		scope.VerifyVertex(pLine1End->xOriginalPos);
		scope.VerifyVertex(pLine2Start->xOriginalPos);
		scope.VerifyVertex(pLine2End->xOriginalPos);
		if(!bOnline)
		{
			for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->pStart==NULL||pRod->pEnd==NULL||!pNode->ModuleTogetherWith(pRod))
					continue;
				SCOPE_STRU scope2;
				scope2.VerifyVertex(pRod->pStart->xOriginalPos);
				scope2.VerifyVertex(pRod->pEnd->xOriginalPos);
				if(!scope.IsIntersWith(scope2))
					continue;
				checkline1.CheckPointEx(pRod->pStart->xOriginalPos,&bOnline);
				if(bOnline)
					checkline1.CheckPointEx(pRod->pEnd->xOriginalPos,&bOnline);
				if(!bOnline)	//�˼����ڵ�һ����������
					continue;
				f3dLine line(pRod->pStart->Position(false),pRod->pEnd->Position(false));
				if(line.PtInLine(pNode->xOriginalPos)>0)	//�����Ϊֱ�Ӷ˵���ڵ�
				{
					pNode->arrRelationPole[0]=pRod->handle;
					pRod1=pRod;
					break;
				}
			}
		}
		bOnline=!(pRod2==NULL||pRod2->pStart==NULL||pRod2->pEnd==NULL);
		if(bOnline)
			checkline2.CheckPointEx(pRod2->pStart->xOriginalPos,&bOnline);
		if(bOnline)
			checkline2.CheckPointEx(pRod2->pEnd->xOriginalPos,&bOnline);
		pRod2=NULL;
		if(!bOnline)
		{
			for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
			{
				if(pRod->pStart==NULL||pRod->pEnd==NULL||!pNode->ModuleTogetherWith(pRod))
					continue;
				SCOPE_STRU scope2;
				scope2.VerifyVertex(pRod->pStart->xOriginalPos);
				scope2.VerifyVertex(pRod->pEnd->xOriginalPos);
				if(!scope.IsIntersWith(scope2))
					continue;
				checkline2.CheckPointEx(pRod->pStart->xOriginalPos,&bOnline);
				if(bOnline)
					checkline2.CheckPointEx(pRod->pEnd->xOriginalPos,&bOnline);
				if(!bOnline)	//�˼����ڵڶ�����������
					continue;
				f3dLine line(pRod->pStart->xOriginalPos,pRod->pEnd->xOriginalPos);
				if( line.PtInLine(pNode->xOriginalPos))	//�����Ϊֱ�Ӷ˵���ڵ�
				{
					pNode->arrRelationPole[1]=pRod->handle;
					pRod2=pRod;
					break;
				}
			}
		}
		if(pRod1!=NULL&&pRod2!=NULL)
			hits++;
	}
	Ta.DisplayProcess(100,"���佻��ڵ������Ϣ");
	return hits;
}
bool CUnifyModelLayer::UnifyObjsLayer(COLINEAR_TOLERANCE tolerance/*=COLINEAR_TOLERANCE(1.0)*/)
{
	listSlopes.Empty();
	hashPlanes.Empty();
	hashPlates.Empty();
	xTolerance=tolerance;
	if(!UnifySlopeObjLayer(NULL,NULL,NULL))
		return false;
	UnifyTrunkObjPosLayer();
	SLOPE* pTopSlope=listSlopes.GetTail();
	if(pTopSlope)
	{
		for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		{
			if(pNode->xOriginalPos.z>pTopSlope->bottom.z||pNode->IsLegObj())
				continue;
			if(pNode->xOriginalPos.z<=pTopSlope->top.z-1000)
				pNode->layer(0)='T';
			else if(!pTopSlope->taper.IsIncludePoint(f3dPoint(pNode->xOriginalPos),tolerance.distance))
				pNode->layer(0)='T';
			else if(!pNode->IsLegObj())
				pNode->layer(0)='S';
		}
		for(CLDSLinePart* pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->IsLegObj())
				continue;
			if(pRod->pStart->Layer(0)=='T'||pRod->pEnd->Layer(0)=='T')
				pRod->layer(0)='T';
		}
	}
	UnifyHeadArmObjPosLayer();
	return true;
}
bool CUnifyModelLayer::RecognizeSlopes(double distTolerance/* = 5.0*/, double tangentTolerance/* = 0.001*/)
{
	xTolerance=COLINEAR_TOLERANCE(distTolerance, tangentTolerance);
	listSlopes.Empty();
	if (!UnifySlopeObjLayer(NULL, NULL, NULL, FALSE))
		return false;
	return true;
}
bool CUnifyModelLayer::RecognizeSlopes(COLINEAR_TOLERANCE* pTolerance/* = NULL*/)
{
	listSlopes.Empty();
	if (pTolerance == NULL)
		xTolerance = *pTolerance;
	if(!UnifySlopeObjLayer(NULL,NULL,NULL,FALSE))
		return false;
	return true;
}

#ifdef __ANGLE_PART_INC_
CIntelliModelDesigner::CIntelliModelDesigner(CIntelliAgent *pIntelliAgent/*=NULL*/)
{
	m_pIntelliAgent=pIntelliAgent;
	m_bEnablePromptUI=true;
}
#endif
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__LDS_CONTEXT_)
BOOL CIntelliModelDesigner::DesignPlate(PLATE& plate)
{
	CConnectPointInfo conn;
	conn.m_hBaseNode=plate.hAttachNode;
	conn.m_hBasePart=plate.hLocationAngle;
	conn.m_hBendlineAngle=plate.hBendlineAngle;
	CLDSLineAngle* pLocationAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(conn.m_hBasePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(pLocationAngle==NULL)
		return FALSE;
	if(plate.cWorkWing=='X')
	{
		conn.m_iConnectWing=0;
		conn.datumplane.vSolid =pLocationAngle->get_norm_x_wing();
	}
	else if(plate.cWorkWing=='Y')
	{
		conn.m_iConnectWing=1;
		conn.datumplane.vSolid=pLocationAngle->get_norm_y_wing();
	}
	else
		return FALSE;
	GEPOINT datumvec=pLocationAngle->pEnd->Position(false)-pLocationAngle->pStart->Position(false);
	normalize(datumvec);
	//��ȡ�ְ��������Ӹ˼�����
	CLdsPartListStack statck(&Ta.Parts);
	CLDSLinePart* pRod;
	conn.m_nMaxRayLsNum=0;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->handle==conn.m_hBasePart)
			continue;
		if(pRod->pStart->handle!=conn.m_hBaseNode&&pRod->pEnd->handle!=conn.m_hBaseNode)
			continue;
		GEPOINT rayvec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
		GEPOINT verifynorm=datumvec^rayvec;
		if(!normalize(verifynorm))
			continue;	//����
		double dd1=0;
		if(!conn.datumplane.vLine.IsZero())
			dd1=fabs(verifynorm*conn.datumplane.vLine);
		else
			dd1=fabs(verifynorm*conn.datumplane.vSolid);
		double dd2=fabs(verifynorm*plate.bendPlaneNorm);
		if(dd1>dd2&&dd1>0.99)	//cos(8��)=0.99
		{
			if(conn.datumplane.vLine.IsZero())
				conn.datumplane.vLine=verifynorm;
			conn.linkPartSet.append(pRod);
		}
		else if(dd1<dd2&&dd2>0.99)
			conn.bendPlaneRodSet.append(pRod);
		else
			continue;
		if(pRod->pStart->handle==conn.m_hBaseNode)
			conn.m_nMaxRayLsNum=max(conn.m_nMaxRayLsNum,pRod->connectStart.wnConnBoltN);
		else if(pRod->pEnd->handle==conn.m_hBaseNode)
			conn.m_nMaxRayLsNum=max(conn.m_nMaxRayLsNum,pRod->connectEnd.wnConnBoltN);
	}
	plate.hPlate=DesignConnectionPoint(&conn);
	return plate.hPlate>0;
}
long CIntelliModelDesigner::DesignConnectionPoint(CConnectPointInfo* pConn)
{
	CSuperSmartPtr<CLDSLinePart> pRod=Ta.FromRodHandle(pConn->m_hBasePart);
	char cWorkWing=0;
	if(pRod->IsAngle()&&pConn->m_iConnectWing==0)
		cWorkWing='X';
	else if(pRod->IsAngle()&&pConn->m_iConnectWing==1)
		cWorkWing='Y';
	PLATE plate(pConn->m_hBaseNode,pConn->m_hBasePart,cWorkWing),*pDesignPlate=NULL;
	if(pRod->IsAngle())
	{
		pDesignPlate=hashPlates.GetValue(plate.StrKey());
		if(pDesignPlate)
			return pDesignPlate->hPlate;
		if(pConn->m_hBendlineAngle==0&&pConn->bendPlaneRodSet.GetNodeNum()>0)
			pConn->m_hBendlineAngle=pConn->m_hBasePart;
	}
	if(pRod.IsHasPtr()&&pRod->IsAngle()&&pRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CGroupAngleLjDesign design;
		plate.hPlate=design.DesignConnectPoint(pConn,1);
	}
	else
	{
		CCommonLjSmartDesign design;
		plate.hPlate=design.DesignConnectPoint(pConn,0,TRUE,TRUE,1,false);
	}
	if(plate.hPlate>0&&pRod->IsAngle())
		hashPlates.SetValue(plate.StrKey(),plate);
	return plate.hPlate;
}
#endif
#if defined(__RAPID_ROUGH_3DMODEL_)&&defined(__LDS_CONTEXT_)
//#ifndef __TMP_LOG_PROC_
//#define __TMP_LOG_PROC_
//#endif
void CIntelliModelDesigner::DesignAllNodeConnections(bool bRetrieveConnPoints/*=true*/)
{
	int i=0,percent=0;
	CXhChar50 szProcessTitle = "�ڵ������������";
	Ta.DisplayProcess(0, szProcessTitle);
#ifndef __STATION_
	for(STDTEMPL* pLiveArm=listArms.EnumObjectFirst();pLiveArm;pLiveArm=listArms.EnumObjectNext())
	{
		//���ݺᵣģ��������Ӱ壨�����߰壩
		for(STDTEMPL::PLATE* pArmPlate=pLiveArm->listParaPlates.EnumObjectFirst();pArmPlate;pArmPlate=pLiveArm->listParaPlates.EnumObjectNext())
			DesignWirePlatePoint(pArmPlate,pLiveArm);
		//���ݺᵣģ����ƹ��߽Ǹ�
		//for(STDTEMPL::WIREANGLE* pWireAngle=pLiveArm->hashWireAngles.GetFirst();pWireAngle;pWireAngle=pLiveArm->hashWireAngles.EnumObjectNext())
			;//DesignWirePlatePoint(pArmPlate,pLiveArm);
		//���ݺᵣģ����ƽǸּӾ��߰�
		//for(STDTEMPL::ANGLEPLATE* pAnglePlate=pLiveArm->listAnglePlates.EnumObjectFirst();pAnglePlate;pAnglePlate=pLiveArm->listAnglePlates.EnumObjectNext())
		//	DesignWirePlatePoint(pAnglePlate,pLiveArm);
	}
#endif
	CDisplayView *pCurView=console.GetActiveView();
	CStackVariant<bool> stackvar1(&CStdComModel::m_bEnablePromptUI, m_bEnablePromptUI);
	CStackVariant<bool> stackvar1i(&CStdComModel::m_bEnableMirCreate, false);
	CStackVariant<BOOL> stackvar2(&g_sysPara.intelligentConnect.m_bOnlyDesignCurView, FALSE);
	long nNodeCount = Ta.Node.GetNodeNum();
#ifdef __TMP_LOG_PROC_
	logerr.Log("����ƽڵ�����%d",nNodeCount);
#endif
	CLDSModule *pWorkHeight=Ta.GetActiveModule();
	CLDSNodePtr  pNode,_xarrNodePool[20000];
	//�����Զ���ƹ����л��Զ������½ڵ㣬���Ա�����ǰ��ԭ�ڵ�ŵ������� wjh-2019.7.30
	ARRAY_LIST<CLDSNode*> xarrNodes(_xarrNodePool, 20000, 2000);
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext(),i++)
		xarrNodes.append(pNode);
	for(i=0;i<xarrNodes.Count;i++)
	{	//����ʶ��ǰ���ӵ���Ϣ
		pNode = xarrNodes[i];
		if(pNode->cfgword.IsNull()||pNode->m_bVirtualObj)
			continue;	//����ռ�ڵ�
#ifdef __TMP_LOG_PROC_
		logerr.Log("%5d#�ڵ�0x%X�����",i,pNode->handle);
		DWORD tickstart=GetTickCount();
		DWORD tick1=0,tick2=0,tick3=0;
#endif
		if(bRetrieveConnPoints)
		{
			DWORD xarrConstDwords[24] = {
				0x00000001,0x00000002,0x00000004,0x00000008,0x00000010,0x00000020,0x00000040,0x00000080,
				0x00000100,0x00000200,0x00000400,0x00000800,0x00001000,0x00002000,0x00004000,0x00008000,
				0x00001000,0x00002000,0x00004000,0x00008000,0x00010000,0x00020000,0x00040000,0x00080000 };
			WORD wiQuadLegNo = 0;
			if(pWorkHeight==NULL||!pWorkHeight->IsSonPart(pNode))
			{
				for(pWorkHeight=Ta.Module.GetFirst();pWorkHeight;pWorkHeight=Ta.Module.GetNext())
				{
					if (!pNode->cfgword.IsHasBodyNo(pWorkHeight->idBodyNo))
						continue;
					if (pNode->IsLegObj())
					{	//���Ƚڵ���Ҫ�ֻ�����ǰ�����Ľ��Ⱥ���ȡ���ӵ���Ϣ
						DWORD legword = pNode->cfgword.GetBodyLegs(pWorkHeight->idBodyNo);
						for (short j = 1; j <= 24; j++)
						{
							if (legword&xarrConstDwords[j - 1])
							{
								wiQuadLegNo = (pWorkHeight->idBodyNo - 1)*CFGLEG::MaxLegs() + j;
								break;
							}
						}
					}
					break;
				}
			}
			WORD xarrQuadLegNo[4];
			memcpy(xarrQuadLegNo,pWorkHeight->m_arrActiveQuadLegNo,sizeof(WORD)*4);
			if(wiQuadLegNo>0)
				pWorkHeight->m_arrActiveQuadLegNo[0]= pWorkHeight->m_arrActiveQuadLegNo[1]= 
				pWorkHeight->m_arrActiveQuadLegNo[2]= pWorkHeight->m_arrActiveQuadLegNo[3]=wiQuadLegNo;
			pNode->RetrieveConnectNodeInfo(NULL,g_sysPara.intelligentConnect.m_bDesignBendConnect,pWorkHeight);
			if (wiQuadLegNo > 0)
				memcpy(pWorkHeight->m_arrActiveQuadLegNo, xarrQuadLegNo,sizeof(WORD)*4);
		}
#ifdef __TMP_LOG_PROC_
		DWORD tick=GetTickCount();
		tick1=tick-tickstart;
#endif
		CONNECT_NODEINFO* pxConnNodeInfo=pNode->GetConnectNodeInfo();
		//�����ͨ����
		BOOL pushed=Ta.Node.push_stack();
		//�����趨Ҫ��ɾ���ǵ�ǰ���������Ƶ�
		SLOPE_TRANSITION transit;
		if(pxConnNodeInfo->ciTransitConnType==1)
		{	//���´����
			if(pxConnNodeInfo->transit.pDownRod->GetClassTypeId()==CLS_LINEANGLE)
				CCommonLjSmartDesign::DesignSlopeTransition(&pxConnNodeInfo->transit,this);
			else if(pxConnNodeInfo->transit.pDownRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
				CCommonLjSmartDesign::DesignSlopOrFootPoint(&pxConnNodeInfo->transit);
			continue;
		}
		else if(pxConnNodeInfo->ciTransitConnType==2)
		{	//���Ű����
			//���Ű��ѵ����ڶ�������ʱ���ɣ��ڴ˲����ظ����� wjh-2019.6.25
			//CCommonLjSmartDesign::DesignSlopOrFootPoint(&pxConnNodeInfo->transit);
			continue;
		}
		//CConnectPointInfo* pConnectPoint=NULL;
		//for(pConnectPoint=pConnectList->GetFirst();pConnectPoint;pConnectPoint=pConnectList->GetNext())
		CConnectPointInfo xConnPointInfo;
		for(UINT j=0;j<pxConnNodeInfo->xCommNode.planes.Count;j++)
		{
			CONNECT_PLANE* pConnPlane=&pxConnNodeInfo->xCommNode.planes[j];
			if (!pConnPlane->blValid||pConnPlane->designType.bIntersBolt)
				continue;
			//DWORD ticktmpstart=GetTickCount();
			if(pxConnNodeInfo->GetConnectPointInfo(j,&xConnPointInfo))
				DesignConnectPointStructure(&xConnPointInfo,this); //��Ƹ����ӵ�����
			else
				logerr.Log("error");
			//DWORD ticktmpend=GetTickCount();
			//logerr.Log("%.3f,%.3f,%.3f#%dms",pConnectPoint->datumplane.vLine.x,pConnectPoint->datumplane.vLine.y,pConnectPoint->datumplane.vLine.z,
			//	ticktmpend-ticktmpstart);
		}
		Ta.Node.pop_stack(pushed);
#ifdef __TMP_LOG_PROC_
		DWORD tickprev=tick;
		tick=GetTickCount();
		tick2=tick-tickprev;
#endif
		//��ƽ������˨���Ǹֽ�ͷ���ֹܷ����ȣ����ڵ�Ȧ��������϶��ԭ��������ƣ�
		for(UINT j=0;j<pxConnNodeInfo->xCommNode.planes.Count;j++)
		{
			CONNECT_PLANE* pConnPlane=&pxConnNodeInfo->xCommNode.planes[j];
			if (!pConnPlane->blValid||!pConnPlane->designType.bIntersBolt)//||pConnectPoint->designType.bAngleJoint||pConnectPoint->designType.bFlJoint)
				continue;	//�����������
			pxConnNodeInfo->GetConnectPointInfo(j,&xConnPointInfo);
			DesignConnectPointStructure(&xConnPointInfo,this); //��Ƹ����ӵ�����
		}
		//��ƽǸֽ�ͷ��ֹܷ���
		if(pxConnNodeInfo->GetConnectPointInfo(CONNECT_NODEINFO::CONNPLANE_JOINT,&xConnPointInfo))
			DesignConnectPointStructure(&xConnPointInfo,this); //��Ƹ����ӵ�Ǹֽ�ͷ

#ifdef __TMP_LOG_PROC_
		DWORD tickend=GetTickCount();
		tick3=tickend-tick;
		if(tickend-tickstart>200)
			logerr.Log("0x%x�ڵ����ʱ�������%d ms(tick1=%d,tick2=%d,tick3=%d",pNode->handle,
			tickend-tickstart,tick1,tick2,tick3);
#endif
		percent=100*i/nNodeCount;
		Ta.DisplayProcess(percent, szProcessTitle);
	}
	Ta.DisplayProcess(100, szProcessTitle);
}

//һ���Դ���ᵣ�����Ҹ����������ļ������λ��
bool CorrectSingleAngleLayout(CLDSLineAngle* pJg,bool updatePartSolid=true);	//defined 
void CIntelliModelDesigner::CorrectGeneralArmRodPosition()
{
	CSuperSmartPtr<CLDSLinePart>pRod;
	CLDSLineAngle* pAngle;
	//�������ĺᵣ�����������Ļ����ְ�
	for(pRod=Ta.EnumRodFirst();pRod.IsHasPtr();pRod=Ta.EnumRodNext())
	{
		if(pRod->Layer(0)!='T'||!pRod->IsAngle()||pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;	//ֻ��Ҫ���ýǸֹ�����λ��
		if (pRod->blGroupSonAngle)
			continue;	//��ϽǸ��ӽǸ�����ϽǸֿ��ƣ����������� wjh-2019.10.23
		pAngle=pRod.LineAnglePointer();
		if(!IsRodRevision(pAngle->handle,REVISION::LAYOUT))
		{
			CorrectSingleAngleLayout(pAngle,false);	//������ǰ��������洦���/��ˮ�жϻ᲻׼ȷ wjh-2018.6.10
			if(pAngle->Layer(1)=='Z')
			{	//����һ��Ĭ��Ϊ����
				char ciAnchorWing=pAngle->ciStartAnchorWing;
				if(ciAnchorWing==0||ciAnchorWing>2)
					ciAnchorWing=pAngle->ciEndAnchorWing;
				else if(ciAnchorWing!=pAngle->ciEndAnchorWing)
					ciAnchorWing=0;
				if(ciAnchorWing==1&&pAngle->vxWingNorm.y<0)
					pAngle->TurnLineAngle(ciAnchorWing-1,0);	//Ŀǰ������ǰ���淭ת
				else if(ciAnchorWing==2&&pAngle->vyWingNorm.y<0)
					pAngle->TurnLineAngle(ciAnchorWing-1,0);	//Ŀǰ������ǰ���淭ת
			}
			else
			{	//ǰ���������趨����ͷ���Ĳ������ر���Ϊ��ˮ�Ĺ涨 wjh-2018.6.2
				char ciAnchorWing=pAngle->ciStartAnchorWing;
				if(ciAnchorWing==0||ciAnchorWing>2)
					ciAnchorWing=pAngle->ciEndAnchorWing;
				else if(ciAnchorWing!=pAngle->ciEndAnchorWing)
					ciAnchorWing=0;
				//����ȷ�Ķ�λ����֫ʱӦ��֤��һ֫��֫���߾������ϣ��������ýǸ���ˮ��
				if(ciAnchorWing==1&&pAngle->vyWingNorm.z>0.1)
					pAngle->TurnLineAngle(ciAnchorWing-1,1);	//Ŀǰ��ˮ���������淭ת
				else if(ciAnchorWing==2&&pAngle->vxWingNorm.z>0.1)
					pAngle->TurnLineAngle(ciAnchorWing-1,1);	//Ŀǰ��ˮ���������淭ת
			}
		}
		long hStartDependRod=pAngle->pStart->hFatherPart;
		long hEndDependRod=pAngle->pEnd->hFatherPart;
		if(pAngle->desStartPos.datum_jg_h>0x20)
			hStartDependRod=pAngle->desStartPos.datum_jg_h;
		if(pAngle->desEndPos.datum_jg_h>0x20)
			hEndDependRod=pAngle->desEndPos.datum_jg_h;
		CLDSLineAngle* pStartDependAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(hStartDependRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		CLDSLineAngle* pEndDependAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(hEndDependRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		LDSNORMAL datumvec,rayvec,bendPlaneNorm;
		rayvec.vSolid=pAngle->End()-pAngle->Start();
		rayvec.vLine=pAngle->pEnd->Position(false)-pAngle->pStart->Position(false);
		if(pStartDependAngle&&pStartDependAngle->Layer(0)=='S'&&pStartDependAngle->Layer(1)=='Z')
		{
			datumvec.vSolid=pStartDependAngle->End()-pStartDependAngle->Start();
			datumvec.vLine=pStartDependAngle->pEnd->Position(false)-pStartDependAngle->pStart->Position(false);
			bendPlaneNorm.vSolid=datumvec.vSolid^rayvec.vSolid;
			bendPlaneNorm.vLine=datumvec.vLine^rayvec.vLine;
			normalize(bendPlaneNorm.vSolid);
			normalize(bendPlaneNorm.vLine);
			double ddx=fabs(bendPlaneNorm.vSolid*pStartDependAngle->get_norm_x_wing());
			double ddy=fabs(bendPlaneNorm.vSolid*pStartDependAngle->get_norm_y_wing());
			if( !pAngle->desStartPos.IsInHuoQuLine()||
				(pAngle->desStartPos.huoqu_jg_h!=0&&pAngle->desStartPos.huoqu_jg_h!=hStartDependRod))
			{
				if(ddx<EPS_COS2&&ddy<EPS_COS2)
					continue;	//�Ӽ��οռ����жϣ�δ���ӵ��κ�һ����׼�Ǹ�֫�ϣ�������Ϊ����б����
			}
			if(!pAngle->desStartPos.IsInDatumJgWingX()&&!pAngle->desStartPos.IsInDatumJgWingY())
			{
				if(ddx>ddy)	//��X֫����ͶӰ
					pAngle->desStartPos.spatialOperationStyle=2;
				else		//��Y֫����ͶӰ
					pAngle->desStartPos.spatialOperationStyle=3;
				ddx=fabs(bendPlaneNorm.vSolid*pStartDependAngle->get_norm_x_wing());	//cos(30��)=0.816
				ddy=fabs(bendPlaneNorm.vSolid*pStartDependAngle->get_norm_y_wing());
				if(ddx>ddy)
				{
					pAngle->desStartPos.wing_x_offset.gStyle=4;
					pAngle->desStartPos.wing_x_offset.offsetDist=0;
					pAngle->desStartPos.wing_y_offset.gStyle=0;
					//pAngle->desStartPos.wing_y_offset.offsetDist=0;
				}
				else
				{
					pAngle->desStartPos.wing_x_offset.gStyle=0;
					//pAngle->desStartPos.wing_x_offset.offsetDist=0;
					pAngle->desStartPos.wing_y_offset.gStyle=4;
					pAngle->desStartPos.wing_y_offset.offsetDist=0;
				}
			}
		}
		else if(pEndDependAngle&&pEndDependAngle->Layer(0)=='S'&&pEndDependAngle->Layer(1)=='Z')
		{
			datumvec.vSolid=pEndDependAngle->End()-pEndDependAngle->Start();
			datumvec.vLine=pEndDependAngle->pEnd->Position(false)-pEndDependAngle->pStart->Position(false);
			bendPlaneNorm.vSolid=datumvec.vSolid^rayvec.vSolid;
			bendPlaneNorm.vLine=datumvec.vLine^rayvec.vLine;
			normalize(bendPlaneNorm.vSolid);
			normalize(bendPlaneNorm.vLine);
			double ddx=fabs(bendPlaneNorm.vSolid*pEndDependAngle->get_norm_x_wing());
			double ddy=fabs(bendPlaneNorm.vSolid*pEndDependAngle->get_norm_y_wing());
			if( !pAngle->desEndPos.IsInHuoQuLine()||
				(pAngle->desEndPos.huoqu_jg_h!=0&&pAngle->desEndPos.huoqu_jg_h!=hEndDependRod))
			{
				if(ddx<EPS_COS2&&ddy<EPS_COS2)
					continue;	//�Ӽ��οռ����жϣ�δ���ӵ��κ�һ����׼�Ǹ�֫�ϣ�������Ϊ����б����
			}
			if(!pAngle->desEndPos.IsInDatumJgWingX()&&!pAngle->desEndPos.IsInDatumJgWingY())
			{
				if(ddx>ddy)	//��X֫����ͶӰ
					pAngle->desEndPos.spatialOperationStyle=2;
				else		//��Y֫����ͶӰ
					pAngle->desEndPos.spatialOperationStyle=3;
				ddx=fabs(bendPlaneNorm.vSolid*pEndDependAngle->get_norm_x_wing());	//cos(30��)=0.816
				ddy=fabs(bendPlaneNorm.vSolid*pEndDependAngle->get_norm_y_wing());
				if(ddx>ddy)
				{
					pAngle->desEndPos.wing_x_offset.gStyle=4;
					pAngle->desEndPos.wing_x_offset.offsetDist=0;
					pAngle->desEndPos.wing_y_offset.gStyle=0;
					//pAngle->desEndPos.wing_y_offset.offsetDist=0;
				}
				else
				{
					pAngle->desEndPos.wing_x_offset.gStyle=0;
					//pAngle->desEndPos.wing_x_offset.offsetDist=0;
					pAngle->desEndPos.wing_y_offset.gStyle=4;
					pAngle->desEndPos.wing_y_offset.offsetDist=0;
				}
			}
		}
	}
}
void CIntelliModelDesigner::CorrectHeadArmRodPosition()
{
	if(m_pIntelliAgent==NULL||listSlopes.GetNodeNum()<=0)
		return;
	//�ᵣһ�㶼������¶Σ������Ȳ��ҵ�����¶�
	SLOPE *pUpperSlope=listSlopes.GetFirst();
	if(pUpperSlope==NULL)
		return;
	for(CUnifyModelLayer::SLOPE* pSlope=listSlopes.GetNext();pSlope;pSlope=listSlopes.GetNext())
	{
		if(pSlope->top.z<pUpperSlope->top.z)
			pUpperSlope=pSlope;
	}
	if(m_pIntelliAgent->GetRodCount()==0)
		m_pIntelliAgent->SyncModel(&Ta);
	listArms.DeleteList();
	m_pIntelliAgent->RecognizeAllStdTempl(pUpperSlope->top,pUpperSlope->bottom,&listArms,&this->xTolerance);
	PRESET_ARRAY8<long> arrArmTempPlanes;
	for(STDTEMPL* pLiveArm=listArms.EnumObjectFirst();pLiveArm;pLiveArm=listArms.EnumObjectNext())
	{
		for(STDTEMPL::DATUM_PLANE* pDatumPlane=pLiveArm->hashParaPlanes.GetFirst();pDatumPlane;pDatumPlane=pLiveArm->hashParaPlanes.GetNext())
		{
			CLDSPlane* pPlane=Ta.Plane.append();
			bool bValidParam=true;
			for(int i=0;bValidParam&&i<3;i++)
			{
				if(pDatumPlane->points[i].ciType==0)
				{	//�ڵ�
					STDTEMPL::MATCHNODE* pMatchNode=pLiveArm->hashMatchNodes.GetValue(pDatumPlane->points[i].widNodeOrRod);
					if(pMatchNode && pMatchNode->hRelaModelNode>0X20)
					{
						pPlane->hPointArr[i].handle=pMatchNode->hRelaModelNode;
						pPlane->hPointArr[i].bSimpleNodePoint=true;
					}
					else
						bValidParam=false;
				}
				else if(pDatumPlane->points[i].ciType=='S'||pDatumPlane->points[i].ciType=='E')
				{	//�˼���ʼ�˵�
					STDTEMPL::MATCHROD* pMatchRod=pLiveArm->hashMatchRods.GetValue(pDatumPlane->points[i].widNodeOrRod);
					if(pMatchRod && pMatchRod->relaRods[0].pIntelliRod!=NULL&&
						pMatchRod->relaRods[0].pIntelliRod->handle>0X20)
					{
						STDTEMPL::EDGEROD* pEdgeRod=&pMatchRod->relaRods[0];
						if(pDatumPlane->points[i].ciType=='E'&&pMatchRod->relaRods.Count>1)
							pEdgeRod=&pMatchRod->relaRods[pMatchRod->relaRods.Count-1];
						pPlane->hPointArr[i].handle=pMatchRod->relaRods[0].pIntelliRod->handle;
						if((pDatumPlane->points[i].ciType=='S'&&!pEdgeRod->bReverseDirection)||(pDatumPlane->points[i].ciType=='E'&&pEdgeRod->bReverseDirection))
							pPlane->hPointArr[i].bRodTipStart=true;
						else
							pPlane->hPointArr[i].bRodTipEnd=true;
					}
					else
						bValidParam=false;
				}
				else
					bValidParam=false;
			}
			if(!bValidParam)
				Ta.Plane.DeleteCursor();
			else
				arrArmTempPlanes.Set(pDatumPlane->widPlane-1,pPlane->handle);
		}
		long *parrPlanes=arrArmTempPlanes.pPresetObjs;
		DYN_ARRAY<long> arrPlanes;
		if(arrArmTempPlanes.Count>arrArmTempPlanes.CountOfPreset)
		{
			arrPlanes.Resize(arrArmTempPlanes.Count);
			for(WORD i=0;i<arrArmTempPlanes.Count;i++)
				arrPlanes[i]=arrArmTempPlanes.At(i);
			parrPlanes=arrPlanes;
		}
		//����ģ���������ýڵ㸸�˼����˼���λ
		m_pIntelliAgent->ApplyLiveArmTempl(pLiveArm,parrPlanes,arrArmTempPlanes.Count,&hashObjRevision);
	}
	CorrectGeneralArmRodPosition();
}
CLDSPlate* CIntelliModelDesigner::InitWirePlateBaseInfo(CLDSNode* pBaseNode,CLDSLineAngle* pBaseJg,BYTE ciWorkWing,LINEPARTSET& rayRodSet)
{
	if(pBaseJg==NULL || pBaseJg->GetClassTypeId()!=CLS_LINEANGLE ||rayRodSet.GetNodeNum()<=0)
		return NULL;
	int thick=ftoi(pBaseJg->GetThick());
	if(thick>5&&thick%2>0)	//���ȡż��
		thick+=3;
	else
		thick+=2;	//���Ӧ�����߲������ֵ��һ���ȼ�
	UCS_STRU ucs;
	double g=(ciWorkWing==0)?pBaseJg->GetNodeWingXG(pBaseNode):pBaseJg->GetNodeWingYG(pBaseNode);
	JGZJ jgzj=(ciWorkWing==0)?pBaseJg->GetZhunJu('X'):pBaseJg->GetZhunJu('Y');
	f3dPoint work_norm=(ciWorkWing==0)?pBaseJg->vxWingNorm:pBaseJg->vyWingNorm;
	ucs.origin=(ciWorkWing==0)?pBaseJg->GetDatumPosWingX(pBaseNode):pBaseJg->GetDatumPosWingY(pBaseNode);
	ucs.axis_x=(pBaseJg->End()-pBaseJg->Start()).normalized();
	//���������߸˼���������
	ARRAY_LIST<RAYROD_INFO> arrRayRodItem;
	arrRayRodItem.SetSize(0,rayRodSet.getCount());
	for(CLDSLinePart* pRayRod=rayRodSet.GetFirst();pRayRod;pRayRod=rayRodSet.GetNext())
	{
		f3dPoint ray_vec;
		if(pRayRod->pStart==pBaseNode)
			ray_vec=(pRayRod->End()-pRayRod->Start()).normalized();
		else
			ray_vec=(pRayRod->Start()-pRayRod->End()).normalized();
		if(ucs.axis_z.IsZero())
		{
			ucs.axis_z=ray_vec^ucs.axis_x;
			normalize(ucs.axis_z);
			if(ucs.axis_z*work_norm<0)
				ucs.axis_z*=-1;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
		}
		//
		vector_trans(ray_vec,ucs,FALSE);
		double alfa=Cal2dLineAng(0,0,ray_vec.x,ray_vec.y);
		RAYROD_INFO* pItem=arrRayRodItem.append();
		pItem->rod.pRod=pRayRod;
		pItem->sina=sin(alfa);
		pItem->cosa=cos(alfa);
		pItem->alfa=alfa*DEGTORAD_COEF;	//ת��Ϊ�Ƕ���
		if(pRayRod->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLDSLineAngle* pRayJg=(CLDSLineAngle*)pRayRod;
			pItem->bInsideAngle=IsInsideJg(pRayJg,ucs.axis_z,&pItem->work_wing);
		}
	}
	CQuickSort<RAYROD_INFO>::QuickSort(arrRayRodItem.m_pData,arrRayRodItem.GetSize(),RAYROD_INFO::CompareRayRod);
	//
	CLDSPlate* pPlate=(CLDSPlate*)pBaseJg->BelongModel()->AppendPart(CLS_PLATE);
	pPlate->Thick=thick;
	pPlate->cMaterial=pBaseJg->cMaterial;
	pPlate->iSeg=pBaseNode->iSeg;
	pPlate->SetLayer(pBaseJg->layer());
	pPlate->cfgword=pBaseJg->cfgword;
	pPlate->ucs=ucs;
	pPlate->designInfo.m_hBaseNode=pBaseNode->handle;
	pPlate->designInfo.m_hBasePart=pBaseJg->handle;
	pPlate->designInfo.iFaceType=1;
	pPlate->designInfo.iProfileStyle0123=1;
	pPlate->designInfo.m_bEnableFlexibleDesign=TRUE;
	pPlate->designInfo.norm.norm_style=1;
	pPlate->designInfo.norm.hVicePart=pBaseJg->handle;
	pPlate->designInfo.norm.direction=0;
	pPlate->designInfo.norm.norm_wing=ciWorkWing;
	pPlate->designInfo.norm.UpdateVector(pBaseJg->BelongModel());
	pPlate->designInfo.origin.datum_pos_style=2;
	pPlate->designInfo.origin.des_para.RODNODE.hRod=pBaseJg->handle;
	pPlate->designInfo.origin.des_para.RODNODE.hNode=pBaseNode->handle;
	pPlate->designInfo.origin.des_para.RODNODE.offset_wing=ciWorkWing;
	pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=4;
	pPlate->designInfo.origin.des_para.RODNODE.wing_offset_dist=g;
	if(g==jgzj.g)
		pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=0;
	else if(g==jgzj.g1)
		pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=1;
	else if(g==jgzj.g2)
		pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=2;
	else if(g==jgzj.g3)
		pPlate->designInfo.origin.des_para.RODNODE.wing_offset_style=3;
	//��������˼����ӱ��л�׼�ĵ������Ϣ
	CDesignLjPartPara *pLjPartPara=pPlate->designInfo.partList.Add(pBaseJg->handle);
	pLjPartPara->iFaceNo=1;
	pLjPartPara->hPart=pBaseJg->handle;
	pLjPartPara->m_nClassTypeId=CLS_LINEANGLE;
	pLjPartPara->angle.bTwoEdge=1;
	pLjPartPara->angle.cur_wing_x0_y1=ciWorkWing;
	pLjPartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOBER);
	pLjPartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pLjPartPara->angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
	for(int i=0;i<arrRayRodItem.GetSize();i++)
	{
		if(arrRayRodItem[i].rod.pRod->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		CLDSLineAngle* pRayJg=(CLDSLineAngle*)arrRayRodItem[i].rod.pRod;
		pLjPartPara=pPlate->designInfo.partList.Add(pRayJg->handle);
		pLjPartPara->iFaceNo=1;
		pLjPartPara->hPart=pRayJg->handle;
		pLjPartPara->m_nClassTypeId=CLS_LINEANGLE;
		pLjPartPara->start0_end1=(pRayJg->pStart==pBaseNode)?0:1;
		pLjPartPara->angle.cur_wing_x0_y1=arrRayRodItem[i].work_wing;
		pLjPartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
		pLjPartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
		pLjPartPara->angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_BOLTEND);
		//�������߽Ǹֹ���֫λ��
		CMultiOffsetPos* pDesPos=(pRayJg->pStart==pBaseNode)?(&pRayJg->desStartPos):(&pRayJg->desEndPos);
		if(arrRayRodItem[i].bInsideAngle)
		{	//����
			if(arrRayRodItem[i].work_wing==0)	//X֫����
			{
				pDesPos->wing_x_offset.gStyle=4;
				pDesPos->wing_x_offset.offsetDist=0;
				pDesPos->wing_y_offset.gStyle=0;
			}
			else
			{
				pDesPos->wing_x_offset.gStyle=0;
				pDesPos->wing_y_offset.gStyle=4;
				pDesPos->wing_y_offset.offsetDist=0;
			}
		}
		else
		{	//����
			if(arrRayRodItem[i].work_wing==0)	//X֫����
			{
				pDesPos->wing_x_offset.gStyle=4;
				pDesPos->wing_x_offset.offsetDist=-pPlate->GetThick();
				pDesPos->wing_y_offset.gStyle=0;
			}
			else
			{
				pDesPos->wing_x_offset.gStyle=0;
				pDesPos->wing_y_offset.gStyle=4;
				pDesPos->wing_y_offset.offsetDist=-pPlate->GetThick();
			}
		}
		pRayJg->ClearFlag();
		pRayJg->CalPosition();
	}
	return pPlate;
}
//���ݹ��߿���Ϣ�����ڵ���޸�Ϊ���߰�
void CIntelliModelDesigner::CreateWirePlate(CLDSPlate* pWirePlate,STDTEMPL* pLiveArm,STDTEMPL::PLATE* pWireHolePara)
{
	if(pWirePlate==NULL || pWireHolePara==NULL)
		return;
	pWirePlate->DesignPlate();
	if(pWirePlate->vertex_list.GetNodeNum()<=3)
		return;
	CLDSLineAngle* pBaseJg=(CLDSLineAngle*)console.FromPartHandle(pWirePlate->designInfo.m_hBasePart,CLS_LINEANGLE);
	//��ӹ��߿�
	BOOL ciInBer0_Wing1=0;
	PROFILE_VERTEX* pPreVertex=pWirePlate->vertex_list.GetTail(),*pCurVertex=NULL;
	for(pCurVertex=pWirePlate->vertex_list.GetFirst();pCurVertex;pCurVertex=pWirePlate->vertex_list.GetNext())
	{
		if(pPreVertex->m_hRod!=pBaseJg->handle || pCurVertex->m_hRod!=pBaseJg->handle)
			continue;
		if(pPreVertex->m_cPosDesignType>0&&abs(pPreVertex->m_cPosDesignType-pCurVertex->m_cPosDesignType)==2)
		{	//��ͬ�����������
			if(pPreVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER||pPreVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2)
				ciInBer0_Wing1=0;
			else
				ciInBer0_Wing1=1;
			break;
		}
		pPreVertex=pCurVertex;
	}
	JGZJ jgzj=(pWireHolePara->ciDatumAngleWing=='X')?pBaseJg->GetZhunJu('X'):pBaseJg->GetZhunJu('Y');
	f3dPoint ls_pos,wing_vec=(pWireHolePara->ciDatumAngleWing=='X')?pBaseJg->GetWingVecX():pBaseJg->GetWingVecY();
	double fCos=wing_vec*pWirePlate->ucs.axis_x;
	BYTE cFlag=1;
	if((ciInBer0_Wing1==0&&fCos>0)||(ciInBer0_Wing1==1&&fCos<0))
		cFlag=-1;
	CDesignWireHole designWireHole(pWirePlate);
	for(UINT i=0;i<pWireHolePara->holes.Count;i++)
	{
		STDTEMPL::PLATE::HOLE wire_hole=pWireHolePara->holes[i];
		ls_pos.x=(wire_hole.wiDistance+jgzj.g)*cFlag;
		ls_pos.y=wire_hole.siUpperOffset;
		ls_pos.z=0;
		coord_trans(ls_pos,pWirePlate->ucs,TRUE);
		CLDSBolt *pLs=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pLs->iSeg=pWirePlate->iSeg;
		pLs->m_cFuncType=CLDSBolt::FUNC_WIREHOLE;
		pLs->m_bVirtualPart=true;
		pLs->set_d(wire_hole.ciHoleDiameter);
		pLs->ucs.origin=ls_pos;
		pLs->des_work_norm.norm_style=0;
		pLs->des_work_norm.vector=pWirePlate->ucs.axis_z;
		pLs->des_base_pos.datumPoint.datum_pos_style=0;
		pLs->des_base_pos.datumPoint.SetPosition(pLs->ucs.origin);
		pLs->des_base_pos.hPart=pWirePlate->handle;
		pLs->AddL0Thick(pWirePlate->handle,TRUE,TRUE);
		pLs->correct_worknorm();
		pLs->correct_pos();
		pLs->CalGuigeAuto();
		pWirePlate->AppendLsRef(pLs->GetLsRef());
		//
		GUAXIANHOLE_BASE_INFO holeInfo;
		pWirePlate->SpreadLsPos(pLs,holeInfo.ls_pos);
		holeInfo.ls_pos.z=0;
		holeInfo.fRadius=pWireHolePara->ciEdgeRadius;
		designWireHole.AppendHoleInfo(holeInfo);
	}
	pWirePlate->m_bDisplayHole=TRUE;
	//��ƹ��߰�����
	designWireHole.m_bWaveShape=(pWireHolePara->ciCurveEdgeMode==1)?TRUE:FALSE;
	if(designWireHole.m_bWaveShape)
		designWireHole.m_fInnerWaveArcRadius=pWireHolePara->ciFilletRadius;
	designWireHole.DesignWirePlate();
	//����ְ�
	STDTEMPL::MATCHROD* pMatchRod=pLiveArm->hashMatchRods.GetValue(pWireHolePara->widBendEdgeAngle1);
	CLDSLineAngle* pBendBaseJg=(pMatchRod)?(CLDSLineAngle*)pMatchRod->relaRods[0].pIntelliRod->MappingRod():NULL;
	if(pBendBaseJg==NULL||pWireHolePara->fBendDegAngle1==0)
		return;
	double fHuoquAngle=pWireHolePara->fBendDegAngle1*RADTODEG_COEF;
	BYTE ciBendIn_B0_W1=0;
	if(pWireHolePara->ciBendEdgeWing1=='x'||pWireHolePara->ciBendEdgeWing1=='y')
		ciBendIn_B0_W1=1;	
	BendPlateByLineAngle(pWirePlate,pBendBaseJg,fHuoquAngle,ciBendIn_B0_W1);
}
bool CIntelliModelDesigner::DesignWirePlatePoint(STDTEMPL::PLATE* pArmPlate,STDTEMPL* pLiveArm)
{
	if(pLiveArm==NULL)
		return false;
	STDTEMPL::MATCHNODE* pMatchNode=pLiveArm->hashMatchNodes.GetValue(pArmPlate->widBaseNode);
	CLDSNode* pBaseNode=(pMatchNode)?(CLDSNode*)pMatchNode->pIntelliNode->MappingNode():NULL;
	if(pBaseNode==NULL)
		return false;
	STDTEMPL::MATCHROD* pMatchRod=pLiveArm->hashMatchRods.GetValue(pArmPlate->widBaseRod);
	CLDSLineAngle* pBaseJg=(pMatchRod)?(CLDSLineAngle*)pMatchRod->relaRods[0].pIntelliRod->MappingRod():NULL;
	if(pBaseJg==NULL)
		return false;
	CConnectPointInfo connpoint;
	connpoint.m_hBaseNode=pBaseNode->handle;
	connpoint.m_hBasePart=pBaseJg->handle;
	connpoint.m_iConnectWing=(pArmPlate->ciDatumAngleWing=='X')?0:1;
	//���ģ���е����Ӹ˼�
	//��ģ������ȡ��ǰ��������һЩ����Ҫ�ĸ˼�
	//�ӱ�׼�ᵣģ���в��Ҹ���Ƶ�Ĺ��߰���Ϣ
	//��ʼ�����߰�Ļ�����Ϣ
	LINEPARTSET rayJgSet;
	for(CLDSLinePart* pRod=connpoint.linkPartSet.GetFirst();pRod;pRod=connpoint.linkPartSet.GetNext())
	{
		if(pRod->handle!=pBaseJg->handle)
			rayJgSet.append(pRod);
	}
	CLDSPlate* pWirePlate=InitWirePlateBaseInfo(pBaseNode,pBaseJg,connpoint.m_iConnectWing,rayJgSet);
	//���û�׼�Ǹ���˨
	ATOM_LIST<CDesignLsPara> ls_para_list;
	int bolt_num=(int)ceil(connpoint.m_nMaxRayLsNum*1.5);
	if(pBaseJg->pStart==pBaseNode&&bolt_num<pBaseJg->connectStart.wnConnBoltN)
		bolt_num=pBaseJg->connectStart.wnConnBoltN;
	else if(pBaseJg->pEnd==pBaseNode&&bolt_num<pBaseJg->connectEnd.wnConnBoltN)
		bolt_num=pBaseJg->connectEnd.wnConnBoltN;
	SmartInitBaseJgLsParas(pBaseNode,pBaseJg,connpoint.m_iConnectWing,bolt_num,&ls_para_list);
	for(CDesignLsPara *pLsPara=ls_para_list.GetFirst();pLsPara;pLsPara=ls_para_list.GetNext())
	{
		CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
		pBolt->des_base_pos=pLsPara->des_base_pos;
		pBolt->des_work_norm=pLsPara->des_work_norm;
		if(pBolt->des_work_norm.direction==0)
			_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",pBaseJg->handle);
		else
			_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",pWirePlate->handle);
		pBolt->set_d(pLsPara->d);
		pBolt->iSeg = pLsPara->iSeg;
		pBolt->SetGrade(pLsPara->grade);
		pBolt->CopyModuleInstanceInfo(pBaseJg);
		pBolt->AddL0Thick(pBaseJg->handle,TRUE);
		pBolt->AddL0Thick(pWirePlate->handle,TRUE);
		pBolt->CalGuigeAuto();
		pBolt->correct_worknorm();
		pBolt->correct_pos();
		pBolt->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		//
		if(pBaseNode==pBaseJg->pStart)
			pBaseJg->AppendStartLsRef(pBolt->GetLsRef());
		else if(pBaseNode==pBaseJg->pEnd)
			pBaseJg->AppendEndLsRef(pBolt->GetLsRef());
		else
			pBaseJg->AppendMidLsRef(pBolt->GetLsRef());
		pWirePlate->AppendLsRef(pBolt->GetLsRef());
	}
	//��׼�˼�ʼ�ն��������Ӱ�ʱӦ�ȸ�������ͷ�Ա�֤���߲���ײ���,���������ĺ��ٻָ�
	if(pBaseJg->pStart==pBaseNode)
		pBaseJg->SetStartOdd(500);
	else if(pBaseJg->pEnd==pBaseNode)
		pBaseJg->SetEndOdd(500);
	//�������߽Ǹ���˨
	for(CLDSLinePart* pRayRod=rayJgSet.GetFirst();pRayRod;pRayRod=rayJgSet.GetNext())
	{	
		if(pRayRod->GetClassTypeId()!=CLS_LINEANGLE)
			continue;
		CLDSLineAngle* pRayJg=(CLDSLineAngle*)pRayRod;
		if(pRayJg->pStart->handle==pBaseNode->handle)
		{
			pRayJg->desStartOdd.m_iOddCalStyle = 0;
			pRayJg->desStartOdd.m_hRefPart1=pBaseJg->handle;
			pRayJg->desStartOdd.m_fCollideDist=5;
			pRayJg->CalStartOddment();
		}
		else
		{
			pRayJg->desEndOdd.m_iOddCalStyle=0;
			pRayJg->desEndOdd.m_hRefPart1=pBaseJg->handle;
			pRayJg->desEndOdd.m_fCollideDist=5;
			pRayJg->CalEndOddment();
		}
		SmartInitRayJgLsParas(pBaseNode,pRayJg,pBaseJg,&ls_para_list);
		for(CDesignLsPara *pLsPara=ls_para_list.GetFirst();pLsPara;pLsPara=ls_para_list.GetNext())
		{
			CLDSBolt *pBolt=(CLDSBolt*)console.AppendPart(CLS_BOLT);
			pBolt->des_base_pos=pLsPara->des_base_pos;
			pBolt->des_work_norm=pLsPara->des_work_norm;
			if(pBolt->des_work_norm.direction==0)
				_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",pRayJg->handle);
			else
				_snprintf(pBolt->des_base_pos.norm_offset.key_str,MAX_TEMP_CHAR_50,"-0X%X",pWirePlate->handle);
			pBolt->set_d(pLsPara->d);
			pBolt->iSeg = pLsPara->iSeg;
			pBolt->SetGrade(pLsPara->grade);
			pBolt->CopyModuleInstanceInfo(pRayJg);	//������˨��ĺ����׼������ĺ�һ��
			pBolt->AddL0Thick(pRayJg->handle,TRUE);
			pBolt->AddL0Thick(pWirePlate->handle,TRUE);
			pBolt->CalGuigeAuto();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			//
			if(pBaseNode->handle==pRayJg->pStart->handle)
				pRayJg->AppendStartLsRef(pBolt->GetLsRef());
			else if(pBaseNode->handle==pRayJg->pEnd->handle)
				pRayJg->AppendEndLsRef(pBolt->GetLsRef());
			else
				pRayJg->AppendMidLsRef(pBolt->GetLsRef());
			pWirePlate->AppendLsRef(pBolt->GetLsRef());
		}
		pRayJg->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pRayJg->GetSolidPartObject());
	}
	//���ݹҿ���Ϣ��ƹ��߰�
	CreateWirePlate(pWirePlate,pLiveArm,pArmPlate);
	pWirePlate->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pWirePlate->GetSolidPartObject());
	//��˨λ�ø��º󣬸�����˨���¼����׼�Ǹ�����ͷ
	if(pBaseJg->pStart==pBaseNode)
	{
		pBaseJg->desStartOdd.m_iOddCalStyle=1;
		pBaseJg->CalStartOddment();
	}
	else if(pBaseJg->pEnd==pBaseNode)
	{
		pBaseJg->desEndOdd.m_iOddCalStyle=1;
		pBaseJg->CalEndOddment();
	}
	pBaseJg->Create3dSolidModel();
	g_pSolidDraw->NewSolidPart(pBaseJg->GetSolidPartObject());
	return true;
}
void DisplaySegmentNode(CModelSegmentNode* pSegment,int ilevel=0)
{
	int i;
	//CModelSlopeNode* pSlopeNode;
	CModelSegmentNode* pSubSegment;
	CModelHeightNode* pHeightNode;
	logerr.Log("  |-�ֶ�%3d#0x%X Tz=%g,H=%g",pSegment->iSeg,pSegment->Id,pSegment->fTopZ,pSegment->fHeight);
	for(pHeightNode=pSegment->hashHeightSet.GetFirst();pHeightNode;pHeightNode=pSegment->hashHeightSet.GetNext())
	{
		double zTransitHoriPlane=pSegment->pTreeModel!=NULL?pSegment->pTreeModel->m_fNamedHeightZeroZ:0;
		CXhChar50 zstr;
		if(zTransitHoriPlane>=0)
			zstr.Printf("%gm=%g-%g m",(pHeightNode->zTransitHoriPlane-zTransitHoriPlane)*0.001,pHeightNode->zTransitHoriPlane*0.001,zTransitHoriPlane*0.001);
		else
			zstr.Printf("%gm=%g+%g m",(pHeightNode->zTransitHoriPlane-zTransitHoriPlane)*0.001,pHeightNode->zTransitHoriPlane*0.001,-zTransitHoriPlane*0.001);
		CXhChar50 szText("    |-%s����,�����:%s",(char*)pHeightNode->name,(char*)zstr);
		for(i=0;i<ilevel;i++)
			szText.InsertBefore("  ",0);
		logerr.Log(szText);
		for(pSubSegment=pHeightNode->hashSegments.GetFirst();pSubSegment;pSubSegment=pHeightNode->hashSegments.GetNext())
		{
			szText.Printf("      |-�ֶ�%3d#0x%X Tz=%g,H=%g",pSubSegment->iSeg,pSubSegment->Id,pSubSegment->fTopZ,pSubSegment->fHeight);
			for(i=0;i<ilevel;i++)
				szText.InsertBefore("  ",0);
			logerr.Log(szText);
		}
		WORD wLegCount=pHeightNode->wLegCount;
		for(WORD ileg=0;ileg<wLegCount;ileg++)
		{
			szText.Printf("      |+%gm����",fto_halfi(pHeightNode->xarrSubLegs[ileg].fLegHeight*0.001));
			for(i=0;i<ilevel;i++)
				szText.InsertBefore("  ",0);
			logerr.Log(szText);
		}
		for(pSubSegment=pSegment->hashMutexSubSegments.GetFirst();pSubSegment;pSubSegment=pSegment->hashMutexSubSegments.GetNext())
			DisplaySegmentNode(pSubSegment,ilevel+1);
	}
}
extern bool SortModuleByHeight();
//extern CLDSNode* FindNamedHeightZeroZNode();
void CUnifyModelLayer::RetrievedTowerModeTree(CTowerModelTree &modeltree)
{
	modeltree.Empty();
	//1.��ȡ�趨�������Ķ�λ��׼�ľ�
	//1.1��ȡ�������ɽṹĿ¼��
	SLOPE* pSlope,*pTopSlope,*pBtmSlope;	//�/�ײ������¶�
	CLDSLinePart* pRod;
	SmartRodPtr pSmartRod;
	LINEPARTSET postRodSet;
	for(pRod=Ta.EnumRodFirst();pRod;pRod=Ta.EnumRodNext())
	{
		if( pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart||	//(�������ݴ���ʱ�û����ص���������Ϊ����˼������)
			pRod->Layer(0)=='T'||pRod->Layer(1)!='Z' || pRod->Layer(2)!='1')
			continue;	//�̽Ǹֻ��1������������
		pSmartRod=pRod;
		if(pSmartRod->IsAngle()&&pSmartRod.pAngle->group_father_jg_h>0x20)
			continue;	//��ϽǸ�ֻ����������ϽǸֲ������ӽǸ�
		pRod->feature=0;	//���㴦����
		postRodSet.append(pRod);
	}
	UINT i=0;
	//��ͳ�Ƴ���1���޵����Ľ�������
	DYN_ARRAY<CTowerModelTree::SORTSEG> segrods(postRodSet.GetNodeNum());
	for(pRod=postRodSet.GetFirst();pRod;pRod=postRodSet.GetNext(),i++)
		segrods[i]=pRod;
	CQuickSort<CTowerModelTree::SORTSEG>::QuickSortClassic(segrods,segrods.Size());
	postRodSet.Empty();
	for(i=0;i<segrods.Size();i++)
		postRodSet.append(segrods[i].pRelaRod);

	CModelSlopeNode* pSlopeNode;
	CModelSegmentNode* pSegment;
	CModelHeightNode* pHeightNode;
	CModelSubLegNode* pSubLeg;
	SortModuleByHeight();	//�������ɵ�������򣬺����ڵĽ����ɸ��������
	CLDSModule* pHeight,*pHighestModule=Ta.Module.GetTail();
	//�������ֵ����ʼƽ���׼Z����
	//CLDSNode* pHeightDatumPlaneNode=FindNamedHeightZeroZNode();
	modeltree.m_fNamedHeightZeroZ=Ta.NamedHeightZeroZ;//pHeightDatumPlaneNode!=NULL?pHeightDatumPlaneNode->xOriginalPos.z:0;

	CModelHeightNode* pHighestHeightNode=modeltree.AppendHeightNode(pHighestModule->idBodyNo);
	for(pHeight=Ta.Module.GetFirst();pHeight;pHeight=Ta.Module.GetNext())
	{
		pHeightNode= modeltree.AppendHeightNode(pHeight->idBodyNo);
		pHeightNode->zLongestLeg=pHeightNode->zExclusiveBodyTop=modeltree.m_fNamedHeightZeroZ;//0;
		pHeightNode->name=pHeight->description;
		//pHeightNode->wLegCount=0;
		for(pRod=postRodSet.GetFirst();pRod;pRod=postRodSet.GetNext())
		{
			if (pRod->pStart == NULL || pRod->pEnd == NULL)
				continue;
			if(pHeight!=pHighestModule&&!pRod->IsLegObj()&&pRod->cfgword.IsHasBodyNoOnly(pHeight->idBodyNo))
			{
				double dfMinTipPosZ=min(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
				if (pHeightNode->zExclusiveBodyTop<dfMinTipPosZ)
					pHeightNode->zExclusiveBodyTop=dfMinTipPosZ;
				pHeightNode->AppendSegmentRod(pRod);
			}
			else if(pRod->IsLegObj()&&pRod->cfgword.IsHasBodyNo(pHeight->idBodyNo))
			{	//������ǰ���ߵĽ���
				WORD iLegSerial=1;
				for(iLegSerial=1;iLegSerial<=CFGLEG::MaxLegs();iLegSerial++)
				{
					if(pRod->cfgword.And(CFGWORD(pHeight->idBodyNo,iLegSerial)))
					{
						if((pSubLeg=pHeightNode->AppendSubLeg(CModelSubLegNode(iLegSerial)))!=NULL)
							pSubLeg->AppendLegPostRod(pRod);
						else
							logerr.Log("error");
					}
					if (pHeightNode->zLongestLeg < pRod->pStart->xOriginalPos.z)
						pHeightNode->zLongestLeg = pRod->pStart->xOriginalPos.z;
					if (pHeightNode->zLongestLeg < pRod->pEnd->xOriginalPos.z)
						pHeightNode->zLongestLeg = pRod->pEnd->xOriginalPos.z;
				}
			}
		}
	}
	//��ȡ����ֶθ˼�
	pBtmSlope=listSlopes.GetFirst();//��ײ������¶Σ�һ�㺬�к��߽��ȣ�
	pTopSlope=listSlopes.GetTail();	//��������¶�
	CModelSegmentNode* pLowestSegment=NULL;
	for(pSlope=pTopSlope;pSlope;pSlope=listSlopes.GetPrev())
	{
		pSlopeNode=modeltree.AppendSlopeNode();
		if(pSlopeNode->pMainHeight==NULL)
			pSlopeNode->pMainHeight=modeltree.hashHeights.GetValue(pHighestModule->idBodyNo);
		if (pSlope==pBtmSlope)
		{	//ͳ�ƴ����²��¶��Ͽ��ڴ�ֱ�����ӵķǻ�׼����ߣ�����
			for (pHeight=Ta.Module.GetFirst();pHeight;pHeight=Ta.Module.GetNext())
			{
				if (pHeight==pHighestModule)
					continue;	//������߻�׼���ߺ�������г��洦��
				pHeightNode= modeltree.hashHeights.GetValue(pHeight->idBodyNo);
				if (fabs(pHeightNode->zExclusiveBodyTop-pSlope->top.z)>1)
					continue;	//�Ǳ��¿�ֱ�Ӻ���
				pSlopeNode->hashRootHeights.SetValue(pHeight->idBodyNo,pHeightNode);
			}
		}
		for(pRod=postRodSet.GetFirst();pRod;pRod=postRodSet.GetNext())
		{
			if(pRod->feature==1)
				continue;	//�Ѵ���˼�
			if(pRod->pStart->xOriginalPos.z-pSlope->top.z<-100||pRod->pStart->xOriginalPos.z-pSlope->bottom.z>100)
				continue;	//ʼ�˽ڵ㲻�ڵ�ǰ�¶�
			if(pRod->pEnd->xOriginalPos.z-pSlope->top.z<-100||pRod->pEnd->xOriginalPos.z-pSlope->bottom.z>100)
				continue;	//�ն˽ڵ㲻�ڵ�ǰ�¶�
			pSegment=NULL;
			if(pSlope!=pBtmSlope)
				pSegment=pSlopeNode->AppendSegmentRod(pRod);
			//��ײ����¶Σ�һ������������öμ���ͬ���ߵ�����
			else if(pRod->cfgword.IsHasBodyNo(pHighestModule->idBodyNo)&&!pRod->IsLegObj())
				pSegment=pSlopeNode->AppendSegmentRod(pRod);
			if(pSegment)
			{
				pLowestSegment=pSegment;
				BOOL pushed=postRodSet.push_stack();
				pSegment->AppendSubTreeNode(postRodSet,pHighestModule->idBodyNo);
				postRodSet.pop_stack(pushed);
			}
		}
	}
	if(pLowestSegment)
		pLowestSegment->hashHeightSet.SetValue(pHighestModule->idBodyNo,pHighestHeightNode);
}
void CIntelliModelDesigner::ResetSpecialAngleG(bool displayUI/*=false*/)
{
	//1.��ȡ�趨�������Ķ�λ��׼�ľ�
	//1.1��ȡ�������ɽṹĿ¼��
	CTowerModelTree modeltree(&Ta);
	//gxModelTree.SetBelongTower(&Ta);
	RetrievedTowerModeTree(modeltree);
	//CLDSLinePart* pRod;
	//SmartRodPtr pSmartRod;

	CModelSlopeNode* pSlopeNode;
	CModelSegmentNode* pSegment;
	if(displayUI)
	{
		int index=0;
		for(pSlopeNode=modeltree.listSlopes.EnumObjectFirst();pSlopeNode;pSlopeNode=modeltree.listSlopes.EnumObjectNext(),index++)
		{
			logerr.Log("�¶�%d FW=%g, SW=%g, Tz=%g",index+1,pSlopeNode->fFrontWidth,pSlopeNode->fSideWidth,pSlopeNode->fTopZ);
			for(pSegment=pSlopeNode->hashSegments.GetFirst();pSegment;pSegment=pSlopeNode->hashSegments.GetNext())
				DisplaySegmentNode(pSegment);
		}
	}
	//��ȡ�趨����������������ľ�
	//��ȡ�趨����ֶ��Ҹ����Ķ�λ��׼�ľ�
	//��ȡ�趨�����Ǹֶ�λ��׼�ľ�
}
bool AddIfNotExist(ARRAY_LIST<long>& xarrInts,long id)
{
	for(int i=0;i<xarrInts.Count;i++)
	{
		if(xarrInts[i]==id)
			return false;
	}
	xarrInts.append(id);
	return true;
}
struct CONNANGLE {
	CDesignLjPartPara* pLjRod;
	CLDSLineAngle* pAngle;
	GEPOINT lenStdVec;
	CONNANGLE(CDesignLjPartPara* pConnRod = NULL) {
		pLjRod = pConnRod;
		pAngle = NULL;
	}
};
int CUnifyModelLayer::CheckColinearJointAngles(CLDSPlate* pPlate, CUnifyModelLayer::JOINTINFO* xarrJoints, int nMaxCount)
{
	CONNANGLE _xarrConnRods[8];
	ARRAY_LIST<CONNANGLE> xarrConnRods(_xarrConnRods, 8,2);
	CDesignLjPartPara* pConnRod;
	for (pConnRod = pPlate->designInfo.partList.GetFirst(); pConnRod; pConnRod = pPlate->designInfo.partList.GetNext())
	{
		CONNANGLE connangle(pConnRod);
		if((connangle.pAngle=(CLDSLineAngle*)pPlate->BelongModel()->FromPartHandle(pConnRod->hPart, CLS_LINEANGLE))!=NULL)
			xarrConnRods.append(connangle);
	}
	int i,j,hits=0;
	for (i = 0; i < xarrConnRods.Count; i++)
	{
		if (xarrConnRods[i].lenStdVec.IsZero())
		{
			xarrConnRods[i].lenStdVec = xarrConnRods[i].pAngle->xPosEnd - xarrConnRods[i].pAngle->xPosStart;
			if (xarrConnRods[i].pLjRod->start0_end1 == 1)
				xarrConnRods[i].lenStdVec *= -1;
			normalize(xarrConnRods[i].lenStdVec);
		}
		for (j = i + 1; j < xarrConnRods.Count; j++)
		{
			if(xarrConnRods[j].lenStdVec.IsZero())
			{
				xarrConnRods[j].lenStdVec = xarrConnRods[j].pAngle->xPosEnd - xarrConnRods[j].pAngle->xPosStart;
				if (xarrConnRods[j].pLjRod->start0_end1 == 1)
					xarrConnRods[j].lenStdVec *= -1;
				normalize(xarrConnRods[j].lenStdVec);
			}
			if (xarrConnRods[i].lenStdVec*xarrConnRods[j].lenStdVec < -0.99)	//-EPS_COS2)
			{
				xarrJoints[hits].hMainAngle = xarrConnRods[i].pAngle->handle;
				xarrJoints[hits].hCoupleAngle= xarrConnRods[j].pAngle->handle;
				hits++;
				if (hits >= nMaxCount)
					break;
			}
		}
	}
	return hits;
}
void CUnifyModelLayer::RetrieveModelLegacyConnections(IModel* pModel)
{
	hashPlates.Empty();
	BYTE* pcbEndBolts;
	CSuperSmartPtr<CLDSGeneralPlate> pBasicPlate;
	CHashList<BYTE> hashAngleEndBolts;
	for(pBasicPlate = (CLDSGeneralPlate*)pModel->EnumPartFirst(CLS_PLATE, CLS_PARAMPLATE); pBasicPlate.blValid;
		pBasicPlate = (CLDSGeneralPlate*)pModel->EnumPartNext(CLS_PLATE, CLS_PARAMPLATE))
	{
		char cWorkWing = 'X';
		long hAttachNode= pBasicPlate->m_hBaseNode;	//�ְ��������Ľڵ�
		long hLocationAngle= pBasicPlate->m_hBaseRod;//�ְ嶨λ��׼�Ǹ�
		SmartPartPtr pBaseAngle = pModel->FromPartHandle(hLocationAngle, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
		if (!pBaseAngle.blValid)
			continue;
		if (fabs(pBasicPlate->ucs.axis_z*pBaseAngle.pAngle->vxWingNorm) < fabs(pBasicPlate->ucs.axis_z*pBaseAngle.pAngle->vyWingNorm))
			cWorkWing = 'Y';		//�ְ嶨λ��׼�ǸֵĹ���֫������֫��
		PLATE plate(hAttachNode, hLocationAngle, cWorkWing);
		plate.hPlate = pBasicPlate->handle;
		if(hashPlates.GetValue(plate.szKey)==NULL)
			hashPlates.SetValue(plate.szKey, plate);
		if (pBasicPlate->GetClassTypeId() != CLS_PLATE)
			continue;
		for (CDesignLjPartPara* pLjRod = pBasicPlate.pPlate->designInfo.partList.GetFirst(); pLjRod;
			pLjRod = pBasicPlate.pPlate->designInfo.partList.GetNext())
		{	//ͳ�ƶ�ͷ��ͨ���ְ����ӵĸ˼�
			if(pLjRod->start0_end1!=0&&pLjRod->start0_end1!=1)
				continue;	//�м�����
			if((pcbEndBolts=hashAngleEndBolts.GetValue(pLjRod->hPart))==NULL)
				pcbEndBolts = hashAngleEndBolts.SetValue(pLjRod->hPart,0);
			*pcbEndBolts|=pLjRod->start0_end1==0?0x01:0x02;
		}
		//ʶ����ͨ�����иְ����Ӷ��ް����Ĺ��߽Ǹ�����������֧��ñ�ӣ� wjh-2019.7.21
		JOINTINFO xarrJoints[4];
		int i,count = CheckColinearJointAngles(pBasicPlate.pPlate, xarrJoints, 4);
		for(i=0;i<count;i++)
			this->hashJoints.Add(DUALKEY(xarrJoints[i].hMainAngle, xarrJoints[i].hCoupleAngle, true));
	}
	CLDSLineAngle* pAngle;
	const double SCOPE_LEN_TOLERANCE = 80;
	for (pAngle = (CLDSLineAngle*)pModel->EnumPartFirst(CLS_LINEANGLE); pAngle != NULL;
		pAngle = (CLDSLineAngle*)pModel->EnumPartNext(CLS_LINEANGLE))
	{
		if(pAngle->pStart==NULL||pAngle->pEnd==NULL||pAngle->m_bVirtualPart)
			continue;
		bool bHasStartConn=false, bHasEndConn=false;
		BYTE *pcbEndBolts=hashAngleEndBolts.GetValue(pAngle->handle);
		if(pcbEndBolts)
		{
			bHasStartConn = ((*pcbEndBolts) & 0x01) > 0;
			bHasEndConn = ((*pcbEndBolts) & 0x02) > 0;
		}
		if(bHasStartConn&&bHasEndConn)
			continue;	//�ýǸ����˵�ǰ�Ѵ��ڸְ��������
		if( pAngle->desStartPos.spatialOperationStyle<1||pAngle->desStartPos.spatialOperationStyle>6||
			pAngle->desEndPos.spatialOperationStyle < 1||pAngle->desEndPos.spatialOperationStyle > 6)
			continue;	//�Ǹֶ�ͷ��˨�ް�ֱ����������������ڶ˵���ͶӰ�����
		GEPOINT inters;
		GEPOINT lenStdVec=pAngle->xPosEnd-pAngle->xPosStart;
		double length=lenStdVec.mod();
		if(length<EPS)
			continue;	//�˵��غ�
		lenStdVec/=length;
		for (CLsRef* pLsRef = pAngle->GetFirstLsRef(); pLsRef; pLsRef = pAngle->GetNextLsRef())
		{
			int wingx0_y1 = 0;
			if (!pAngle->GetBoltIntersPos(pLsRef->pBolt, inters,&wingx0_y1))
				continue;
			double distOfLen=lenStdVec*(inters-pAngle->xPosStart);
			if (!bHasStartConn&&distOfLen <= SCOPE_LEN_TOLERANCE)
			{
				PLATE plate(pAngle->pStart->handle, pAngle->handle, wingx0_y1 == 0 ? 'X' : 'Y');
				plate.hBolt = pLsRef->pBolt->handle;
				if (hashPlates.GetValue(plate.szKey) == NULL)
					hashPlates.SetValue(plate.szKey, plate);
				bHasStartConn = true;
				SmartRodPtr pDependRod;
				if(pAngle->pStart->hFatherPart!=pAngle->handle)
					pDependRod =pModel->FromRodHandle(pAngle->pStart->hFatherPart);
				if(pDependRod.blValid&&pDependRod->IsAngle())
				{
					char ciWorkWing = 'Y';
					if (fabs(pDependRod.pAngle->vxWingNorm*pLsRef->pBolt->ucs.axis_z) >
						fabs(pDependRod.pAngle->vyWingNorm*pLsRef->pBolt->ucs.axis_z))
						ciWorkWing = 'X';
					PLATE plate2(pAngle->pStart->handle, pDependRod->handle, ciWorkWing);
					plate2.hBolt = pLsRef->pBolt->handle;
					if (hashPlates.GetValue(plate2.szKey) == NULL)
						hashPlates.SetValue(plate2.szKey, plate2);
				}
			}
			if (!bHasEndConn&&distOfLen >= length-SCOPE_LEN_TOLERANCE)
			{
				PLATE plate(pAngle->pEnd->handle, pAngle->handle, wingx0_y1 == 0 ? 'X' : 'Y');
				if (hashPlates.GetValue(plate.szKey) == NULL)
					hashPlates.SetValue(plate.szKey, plate);
				bHasEndConn = true;
				SmartRodPtr pDependRod;
				if(pAngle->pEnd->hFatherPart!=pAngle->handle)
					pDependRod =pModel->FromRodHandle(pAngle->pEnd->hFatherPart);
				if(pDependRod.blValid&&pDependRod->IsAngle())
				{
					char ciWorkWing = 'Y';
					if (fabs(pDependRod.pAngle->vxWingNorm*pLsRef->pBolt->ucs.axis_z) >
						fabs(pDependRod.pAngle->vyWingNorm*pLsRef->pBolt->ucs.axis_z))
						ciWorkWing = 'X';
					PLATE plate2(pAngle->pEnd->handle, pDependRod->handle, ciWorkWing);
					plate2.hBolt = pLsRef->pBolt->handle;
					if (hashPlates.GetValue(plate2.szKey) == NULL)
						hashPlates.SetValue(plate2.szKey, plate2);
				}
			}
		}
	}
	CLDSLineAngle* _xarrAngles[4];
	long _hRelaPartArr[16];
	for(pAngle = (CLDSLineAngle*)pModel->EnumPartFirst(CLS_LINEANGLE); pAngle!=NULL;
		pAngle = (CLDSLineAngle*)pModel->EnumPartNext(CLS_LINEANGLE))
	{
		if(pAngle->pStart!=NULL||pAngle->pEnd!=NULL)
			continue;	//�����϶���ʼĩ�˽ڵ�
		GEPOINT vLenStdVec=pAngle->xPosEnd-pAngle->xPosStart;
		normalize(vLenStdVec);
		CLdsPartListStack stackparts(pModel);
		ARRAY_LIST<CLDSLineAngle*>xarrAngles(_xarrAngles,4);
		ARRAY_LIST<long>xarrRelaPartId(_hRelaPartArr,16);
		for(CLsRef* pLsRef=pAngle->GetFirstLsRef();pLsRef;pLsRef=pAngle->GetNextLsRef())
		{
			for(THICK_ITEM item=pLsRef->pBolt->GetFirstL0ThickItem();!item.IsFinalKey();
				item=pLsRef->pBolt->GetNextL0ThickItem())
			{
				if(item.IsHandle()&&item.val!=pAngle->handle)
					AddIfNotExist(xarrRelaPartId,abs(item.val));
			}
		}
		for(int i=0;i<xarrRelaPartId.Count;i++)
		{
			CLDSLineAngle* pJointAngle=(CLDSLineAngle*)pModel->FromPartHandle(xarrRelaPartId[i],CLS_LINEANGLE);
			if(pJointAngle==NULL||pJointAngle->pStart==NULL||pJointAngle->pEnd==NULL)
				continue;
			GEPOINT vLenStdVec2=pJointAngle->xPosEnd-pJointAngle->xPosStart;
			normalize(vLenStdVec2);
			if(fabs(vLenStdVec*vLenStdVec2)<EPS_COS2)
				continue;	//������
			xarrAngles.append(pJointAngle);
		}
		if(xarrAngles.Count!=2)
			continue;
		this->hashJoints.Add(DUALKEY(xarrAngles[0]->handle,xarrAngles[1]->handle,true));
	}
}
#endif
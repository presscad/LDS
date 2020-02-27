#include "stdafx.h"
#include "IntelliAgent.h"

#ifndef __PUBLIC_RELEASE_COMPILE_
#include "IntelliCore\IntelliCore.h"
#else
#include "IntelliCore\IntelliCoreObject.h"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifdef __RAPID_ROUGH_3DMODEL_
CIntelliAgent::CIntelliAgent(IModel* pModel/*=NULL*/,UINT uiMinKeyLength/*=300*/)
{
	m_pModel=pModel;
	SyncModel(pModel);
	m_uiMinKeyLength=uiMinKeyLength;
}


CIntelliAgent::~CIntelliAgent(void)
{
}
void CIntelliAgent::ClearObjects(){
	hashModelNodes.Empty();
	hashModelRods.Empty();
}
CIntelliAgent::CModelNode::CModelNode(CLDSNode* pNode/*=NULL*/)
{
	_bLayerModified=false;
	if((pMappingNode=pNode)!=NULL)
	{
		handle=pNode->handle;
		hParentRod=pNode->hFatherPart;
		memcpy(layer,pNode->layer(),4);
		xPosition=pNode->xPreliftPos;
	}
	else
	{
		handle=hParentRod=0;
		layer[0]=layer[1]=layer[2]=layer[3]=0;
	}
}
bool CIntelliAgent::CModelNode::SetLayerRevState(bool modified/*=true*/)
{
	return _bLayerModified=modified;
}
bool CIntelliAgent::CModelRod::SetLayerRevState(bool modified/*=true*/)
{
	return m_bLayerModified=modified;
}
CIntelliAgent::CModelRod::CModelRod(CLDSLinePart* pRod/*=NULL*/)
{
	m_bLayerModified=false;
	if((pMappingRod=pRod)!=NULL)
	{
		handle=pRod->handle;
		hStartNode=pRod->pStart->handle;
		hEndNode=pRod->pEnd->handle;
		memcpy(layer,pRod->layer(),4);
		xStartPoint=pRod->pStart->xPreliftPos;
		xEndPoint=pRod->pEnd->xPreliftPos;
	}
	else
	{
		handle=hStartNode=hEndNode=0;
		layer[0]=layer[1]=layer[2]=layer[3]=0;
	}
	pStart=pEnd=NULL;
}
IModelNode* CIntelliAgent::AppendNode(CLDSNode* pNode)
{
	return hashModelNodes.SetValue(pNode->handle,CModelNode(pNode));
}
IModelRod* CIntelliAgent::AppendRod(CLDSLinePart* pRod)
{
	return hashModelRods.SetValue(pRod->handle,CModelRod(pRod));
}
IModelNode* CIntelliAgent::FromNodeHandle(long hNode)
{
	return hashModelNodes.GetValue(hNode);
}
IModelRod* CIntelliAgent::FromRodHandle(long hRod)
{
	return hashModelRods.GetValue(hRod);
}
IModelNode* CIntelliAgent::EnumNodeFirst()
{
	return hashModelNodes.GetFirst();
}
IModelNode* CIntelliAgent::EnumNodeNext()
{
	return hashModelNodes.GetNext();
}
IModelRod* CIntelliAgent::EnumRodFirst()
{
	return hashModelRods.GetFirst();
}
IModelRod* CIntelliAgent::EnumRodNext()
{
	return hashModelRods.GetNext();
}
void CIntelliAgent::SyncModel(IModel* pModel)
{
	m_pModel=pModel;
	hashModelNodes.Empty();
	hashModelRods.Empty();
	if(pModel==NULL)
		return;
	for(CLDSNode* pNode=pModel->EnumNodeFirst();pNode;pNode=pModel->EnumNodeNext())
	{
		if(!pNode->m_bVirtualObj)
			AppendNode(pNode);
	}
	for(CLDSLinePart* pRod=pModel->EnumRodFirst();pRod;pRod=pModel->EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart)
			continue;
		IModelRod* pModelRod=AppendRod(pRod);
		pModelRod->pStart=FromNodeHandle(pModelRod->hStartNode);
		pModelRod->pEnd=FromNodeHandle(pModelRod->hEndNode);
	}
}
////////////////////////////////////////////////////////////////////////////////
//功能函数
bool CIntelliAgent::RecognizeConnPointSchema(CLDSNode* pBaseNode,STDTEMPL::CONNPOINT* pConnPoint,GECS* pDCS/*=NULL*/)
{
	PRESET_ARRAY8<STDTEMPL::CONNPOINT::MATCHROD> rodarr;
	for(CModelRod* pModelRod=hashModelRods.GetFirst();pModelRod;pModelRod=hashModelRods.GetNext())
	{
		STDTEMPL::CONNPOINT::MATCHROD rod;
		rod.pModelRod=pModelRod;
		if(pModelRod->pMappingRod->size_idClassType==CLS_LINEANGLE)
			rod.ciRodSectType='L';
		else if(pModelRod->pMappingRod->size_idClassType==CLS_GROUPLINEANGLE)
			rod.ciRodSectType=pModelRod->pMappingRod->size_cSubClassType;
		else if(pModelRod->pMappingRod->size_idClassType==CLS_LINETUBE)
			rod.ciRodSectType='O';
		else
			continue;
		GEPOINT xStartPoint=pDCS?pDCS->TransPToCS(pModelRod->xStartPoint):pModelRod->xStartPoint;
		GEPOINT xEndPoint  =pDCS?pDCS->TransPToCS(pModelRod->xEndPoint):pModelRod->xEndPoint;
		if(pModelRod->pMappingRod->pStart==pBaseNode)
		{
			rod.ciRodType='R';
			rod.siActualRayAngle=(short)(0.5+Cal2dLineAng(xStartPoint.x,xStartPoint.y,xEndPoint.x,xEndPoint.y)*DEGTORAD_COEF);
		}
		else if(pModelRod->pMappingRod->pEnd==pBaseNode)
		{
			rod.ciRodType='R';
			rod.siActualRayAngle=(short)(0.5+Cal2dLineAng(xEndPoint.x,xEndPoint.y,xStartPoint.x,xStartPoint.y)*DEGTORAD_COEF);
		}
		else
		{
			if(pModelRod->pMappingRod->pStart==NULL||pModelRod->pMappingRod->pEnd==NULL)
				continue;
			CPtInLineCheck check(xStartPoint,xEndPoint);
			bool online=false;
			double ratio=check.CheckPointEx(pBaseNode->xOriginalPos,&online,1);
			if(!online||ratio<EPS||ratio>1-EPS)
				return false;
			rod.ciRodType='A';
			rod.siActualRayAngle=(short)(0.5+Cal2dLineAng(xStartPoint.x,xStartPoint.y,xEndPoint.x,xEndPoint.y)*DEGTORAD_COEF);
			if(rod.siActualRayAngle>=90&&rod.siActualRayAngle<180)
				rod.siActualRayAngle+=180;
			else if(rod.siActualRayAngle>=180&&rod.siActualRayAngle<270)
				rod.siActualRayAngle-=180;
		}
		rodarr.Append(rod);
	}
#ifdef __PUBLIC_RELEASE_COMPILE_
	CIntelliCore intelli;
	memset(pConnPoint->matchrod_indexes,0,sizeof(WORD)*8);
	if(rodarr.Count>=8)
		return false;
	memcpy(pConnPoint->rodarr,rodarr.Data(),sizeof(STDTEMPL::CONNPOINT::MATCHROD)*rodarr.Count);
	return intelli.RecognizeConnPointSchema(pConnPoint);
#else
	return false;
#endif
}
#include "../BasicOutput.h"
int CIntelliAgent::RecognizeAllStdTempl(const double* vxSlopeTop3d,const double* vxSlopeBtm3d,IXhList<STDTEMPL>* pListArms,
		COLINEAR_TOLERANCE* pxTolerance/*=NULL*/)
{
	COLINEAR_TOLERANCE xPreferColinearTolerance;
	if(pxTolerance==NULL)
		pxTolerance=&xPreferColinearTolerance;
#ifndef __PUBLIC_RELEASE_COMPILE_
	int recog_count=::RecognizeAllStdTempl(this,vxSlopeTop3d,vxSlopeBtm3d,pListArms,&logerr);
#else
	CIntelliCore core;
	CLogFile recoglog("recog.log");
	CTaskLogErrorLife loglife(&recoglog);
	int recog_count=core.RecognizeAllStdTempl(this,vxSlopeTop3d,vxSlopeBtm3d,pListArms,&recoglog,m_uiMinKeyLength,pxTolerance);
#endif
	//更新图层名
	CLDSNode* pNode;
	for(pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
	{
		if(pNode->Layer(0)=='T'&&pNode->Layer(2)=='Q')
			pNode->feature=1;	//前侧节点
		else
			pNode->feature=0;	//非前侧节点
	}
	for(IModelNode* pIntelliNode=EnumNodeFirst();pIntelliNode;pIntelliNode=EnumNodeNext())
	{
		CModelNode* pModelNode=(CModelNode*)pIntelliNode;
		if(pModelNode->m_bLayerModified)
		{
			pModelNode->pMappingNode->SetLayer(pModelNode->layer);
			pModelNode->pMappingNode->feature+=2;	//0->2,1->3;
		}
	}
	for(pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
	{
		if(pNode->feature==0||pNode->feature==3)	//节点前侧属性状态未生变化0.表示修订前后都不是前侧，1表示修订前后都是前侧
			continue;
		pNode->SyncMirProp("layer");	//节点是否属于前面，状态发生了前化
	}
	CLDSLinePart* pRod;
	for(pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
	{
		if(pRod->Layer(0)=='T'&&pRod->Layer(2)=='Q')
			pRod->feature=1;	//前侧杆件
		else
			pRod->feature=0;	//非前侧杆件
	}
	for(IModelRod* pIntelliRod=EnumRodFirst();pIntelliRod;pIntelliRod=EnumRodNext())
	{
		CModelRod* pModelRod=(CModelRod*)pIntelliRod;
		if(pModelRod->m_bLayerModified)
		{
			pModelRod->pMappingRod->SetLayer(pModelRod->layer);
			pModelRod->pMappingRod->feature+=2;	//0->2,1->3;
		}
	}
	for(pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
	{
		if(pRod->feature==0||pRod->feature==3)	//节点前侧属性状态未生变化0.表示修订前后都不是前侧，1表示修订前后都是前侧
			continue;
		pRod->SyncMirProp("layer");	//节点是否属于前面，状态发生了前化
	}
	return recog_count;
}
//是否为长杆件隶属度
static double CalLongRodFactor(double length)
{
	if(length>4000)
		return 1;	//长杆件，可认为是主框架杆件
	else if(length<2000)	//4000000=2000*2000
		return 0;	//短杆件，不被认为是框架主杆件
	else
	{
		double MID_LENGTH=3000;	//3000=(2000+4000)/2;
		double LENGTH_SPAN=2000;//4000-2000;
		double HALF_LENGTH_SPAN=LENGTH_SPAN*0.5;
		if(length<=MID_LENGTH)
		{
			double len_dist=length-2000;
			double factor=len_dist/2000;
			return 2*factor*factor;
		}
		else //if(length>MID_LENGTH)
		{
			double len_dist=length-MID_LENGTH;
			double factor=len_dist/HALF_LENGTH_SPAN;
			double root=sqrt(factor);
			return 0.5+root*0.5;
		}
	}
}
//是否为正交杆件的隶属度
static double CalRodOrthoFactor(double dx,double dz)
{
	if(fabs(dz)<1)	//水平杆件
		return 1.0;
	else if(fabs(dx)<1)
		return 1.0;	//竖直杆件
	if(fabs(dx)>fabs(dz))
		return 1.0-fabs(dz/dx);
	else //if(fabs(dx)<fabs(dz))
		return 1.0-fabs(dx/dz);
}
bool CIntelliAgent::ApplyLiveArmTempl(STDTEMPL* pLiveArm,long* arrayPlanes/*=NULL*/,int nPlaneCount/*=0*/,CHashList<BITFLAG> *pHashObjRevision/*=NULL*/)
{
	int i;
	STDTEMPL::PROFILE_NODE* pParaNode;
	STDTEMPL::PROFILE_ROD* pParaRod;
	STDTEMPL::MATCHNODE* pMatchNode;
	STDTEMPL::MATCHROD* pMatchRod;
	//0）该横担是未学习过的新类型横担或塔头，根据设计习惯进行一般规则性处理
	/*if(pLiveArm->hashParaNodes.GetNodeNum()==0&&pLiveArm->hashMatchNodes.GetNodeNum()>0)
	{
		STDTEMPL::MATCHNODE *pMatchNode=NULL,*pNextNode=NULL;
		STDTEMPL::MATCHNODE *pPrevNode=pLiveArm->hashMatchNodes.GetFirst();
		for(pMatchNode=pLiveArm->hashMatchNodes.GetNext();pMatchNode!=NULL;)
		{
			BITFLAG *pRevFlag=pHashObjRevision?pHashObjRevision->Add(pMatchNode->hRelaModelNode):NULL;
			//const DWORD PARENT_ROD	=0x00000002;	//节点父杆件
			if(pRevFlag&&pRevFlag->IsHasFlag(0x02))
				continue;
			pNextNode=pLiveArm->hashMatchNodes.GetNext();
			if(pPrevNode&&pNextNode)
			{
				GEPOINT vxPrevEdgeVec=pPrevNode->pIntelliNode->xPosition-pMatchNode->pIntelliNode->xPosition;
				GEPOINT vxNextEdgeVec=pNextNode->pIntelliNode->xPosition-pMatchNode->pIntelliNode->xPosition;
				double prev_longrod_factor=CalLongRodFactor (vxPrevEdgeVec.mod());
				double prev_ortho_factor  =CalRodOrthoFactor(vxPrevEdgeVec.x,vxPrevEdgeVec.z);
				double next_longrod_factor=CalLongRodFactor (vxNextEdgeVec.mod());
				double next_ortho_factor  =CalRodOrthoFactor(vxNextEdgeVec.x,vxNextEdgeVec.z);
				int rev_parentrod_prev1_next2=0;
				if(prev_longrod_factor-next_longrod_factor>0.6)	//取长不取短
					rev_parentrod_prev1_next2=1;	//取长不取短,前侧杆件长
				else if(prev_longrod_factor-next_longrod_factor<-0.6)
					rev_parentrod_prev1_next2=2;	//取长不取短,后侧杆件长
				else if(fabs(prev_longrod_factor-next_longrod_factor)<0.3)
				{	//两杆件长度差不多，优先取正交
					if(prev_ortho_factor-next_ortho_factor>0.6)
						rev_parentrod_prev1_next2=1;
					else if(prev_ortho_factor-next_ortho_factor<-0.6)
						rev_parentrod_prev1_next2=2;
				}
				if(rev_parentrod_prev1_next2==0)
					continue;
				GEPOINT vxStdRayEdgeVec=rev_parentrod_prev1_next2==1?vxPrevEdgeVec:vxNextEdgeVec;
				normalize(vxStdRayEdgeVec);
				for(IModelRod* pRod=EnumRodFirst();pRod;pRod=EnumRodNext())
				{
					GEPOINT vxRayVec=pRod->xEndPoint-pRod->xStartPoint;
					if(pRod->pEnd==pMatchNode->pIntelliNode)
						vxRayVec*=-1.0;
					else if(pRod->pStart!=pMatchNode->pIntelliNode)
						continue;
					normalize(vxRayVec);
					if(vxRayVec*vxStdRayEdgeVec>0.99)
					{
						((CLDSNode*)pMatchNode->pIntelliNode->MappingNode())->hFatherPart=pRod->handle;
						if(pRevFlag)
							pRevFlag->SetFlagDword(0x02);
						break;
					}
				}
			}
			pMatchNode=pNextNode;
		}
	}*/

	STDTEMPL::EDGEROD* pEdgeRod;
	//1）重置节点父杆件
	for(pParaNode=pLiveArm->hashParaNodes.GetFirst();pParaNode;pParaNode=pLiveArm->hashParaNodes.GetNext())
	{
		if(pParaNode->widMasterRod==0)
			continue;
		if((pMatchNode=pLiveArm->hashMatchNodes.GetValue(pParaNode->widNode))==NULL)
			continue;
		if((pMatchRod=pLiveArm->hashMatchRods.GetValue(pParaNode->widMasterRod))==NULL)
			continue;
		CLDSLinePart* pRod=NULL;
		for(UINT i=0;i<pMatchRod->relaRods.Count;i++)
		{
			if((pEdgeRod=pMatchRod->relaRods.GetAt(i))==NULL)
				continue;
			pRod=(CLDSLinePart*)pEdgeRod->pIntelliRod->MappingRod();
			if(pMatchRod->lenModelStdVec.IsZero())
			{
				pMatchRod->lenModelStdVec=pRod->pEnd->xPreliftPos-pRod->pStart->xPreliftPos;
				pMatchRod->lenModelStdVec.y=0;
				normalize(pMatchRod->lenModelStdVec);
			}
			double endLenPos=pMatchRod->lenModelStdVec*(pRod->pEnd->xPreliftPos-pRod->pStart->xPreliftPos);
			double nodeLenPos=pMatchRod->lenModelStdVec*(pMatchNode->pIntelliNode->xPosition-pRod->pStart->xPreliftPos);
			if(endLenPos>0&&nodeLenPos>-EPS&&nodeLenPos<endLenPos+EPS)
				break;
			else if(endLenPos<0&&nodeLenPos<EPS&&nodeLenPos>=endLenPos-EPS)
				break;
		}
		if(pRod)
		{
			CLDSNode* pRelaModelNode=(CLDSNode*)pMatchNode->pIntelliNode->MappingNode();
			if(pRelaModelNode->hFatherPart!=pRod->handle)
			{
				CPtInLineCheck check(pRod->pStart->xOriginalPos,pRod->pEnd->xOriginalPos);
				bool bOnLine=false;
				check.CheckPointEx(pRelaModelNode->xOriginalPos,&bOnLine,0.001);
				if(bOnLine)
				{	//防止因为识别错误，如存在Y向横担时可能将Y向横担上的节点识别为主横担上的节点 wjh-2018.10.21
					pRelaModelNode->hFatherPart=pRod->handle;
					pRelaModelNode->SyncMirProp("hFatherPart",CLDSPart::MIR_X);
				}
			}
			if(pHashObjRevision)
			{
				BITFLAG *pRevFlag=pHashObjRevision->Add(pMatchNode->hRelaModelNode);
				//const DWORD PARENT_ROD	=0x00000002;	//节点父杆件
				pRevFlag->SetFlagDword(0x02);
				for(RELATIVE_OBJECT* pRelaObj=pRelaModelNode->relativeObjs.GetFirst();pRelaObj;pRelaObj=pRelaModelNode->relativeObjs.GetNext())
				{
					if(pRelaObj->mirInfo.axis_flag==0x01)
					{
						pRevFlag=pHashObjRevision->Add(pRelaObj->hObj);
						pRevFlag->SetFlagDword(0x02);//LAYOUT	=0x02;	//节点父杆件
					}
				}
			}
		}
	}
	for(pParaNode=pLiveArm->hashParaNodes.GetFirst();pParaNode;pParaNode=pLiveArm->hashParaNodes.GetNext())
	{
		if(pParaNode->wirecode.ciWireType==0)
			continue;
		if((pMatchNode=pLiveArm->hashMatchNodes.GetValue(pParaNode->widNode))==NULL)
			continue;
		CLDSNode* pNode=(CLDSNode*)pMatchNode->pIntelliNode->MappingNode();
		if(pNode)
		{
			pNode->wireplace.ciWireType=pParaNode->wirecode.ciWireType;
			pNode->m_ciWireFractions=pParaNode->ciFractions;
			pNode->SyncMirProp("hangingInfo");
		}
	}
	//2）重置角钢摆放方式
	for(pParaRod=pLiveArm->hashParaRods.GetFirst();pParaRod;pParaRod=pLiveArm->hashParaRods.GetNext())
	{
		UINT i;
		if((pMatchRod=pLiveArm->hashMatchRods.GetValue(pParaRod->widRod))==NULL)
			continue;
		for(i=0;i<pMatchRod->relaRods.Count;i++)
		{
			if((pEdgeRod=pMatchRod->relaRods.GetAt(i))==NULL||!pEdgeRod->pIntelliRod->IsAngle())
				continue;
			CLDSLineAngle* pMatchAngle=(CLDSLineAngle*)pEdgeRod->pIntelliRod->MappingRod();
			pMatchAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
		}
		if(pParaRod->ciAngleLayout==0)
			continue;
		if(pParaRod->ciSectStyle==2||pParaRod->ciSectStyle==3)
		{	//'T' or 'D'
			for(i=0;i<pMatchRod->relaRods.Count;i++)
			{
				if((pEdgeRod=pMatchRod->relaRods.GetAt(i))==NULL||!pEdgeRod->pIntelliRod->IsAngle())
					continue;
				CLDSLineAngle* pMatchAngle=(CLDSLineAngle*)pEdgeRod->pIntelliRod->MappingRod();
				CLDSGroupLineAngle* pGroupAngle=pMatchAngle->GetClassTypeId()!=CLS_GROUPLINEANGLE?NULL:(CLDSGroupLineAngle*)pMatchAngle;
				if(pGroupAngle&&pMatchAngle->size_cSubClassType=='L')
					pMatchAngle->size_cSubClassType='D';	//TODO:不知道为什么导入计算模型会出这样错 wjh-2018.7.3
				if(pMatchAngle->size_cSubClassType!='T'&&pMatchAngle->size_cSubClassType!='D'&&pMatchAngle)
					continue;
				char cNewSectStyle=pParaRod->ciSectStyle==2?'T':'D';
				if(pMatchAngle->size_cSubClassType!=cNewSectStyle)
					pMatchAngle->size_cSubClassType=cNewSectStyle;
			}
		}
		
		for(UINT i=0;i<pMatchRod->relaRods.Count;i++)
		{
			if((pEdgeRod=pMatchRod->relaRods.GetAt(i))==NULL||!pEdgeRod->pIntelliRod->IsAngle())
				continue;
			CLDSLineAngle* pMatchAngle=(CLDSLineAngle*)pEdgeRod->pIntelliRod->MappingRod();
			CLDSGroupLineAngle* pGroupAngle=NULL;
			if(pMatchAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
				pGroupAngle=(CLDSGroupLineAngle*)pMatchAngle;
			STDTEMPL::MATCHNODE* pMatchRodStart=pLiveArm->hashMatchNodes.GetValue(pParaRod->widStartNode);
			STDTEMPL::MATCHNODE* pMatchRodEnd=pLiveArm->hashMatchNodes.GetValue(pParaRod->widEndNode);
			DESIGN_JGWING_NORM* pDesAngleWingNormX=&pMatchAngle->des_norm_x;
			DESIGN_JGWING_NORM* pDesAngleWingNormY=&pMatchAngle->des_norm_y;
			bool reverseWingXnY=false;
			if(pMatchRodStart&&pMatchRodEnd)
			{
				GEPOINT vxRefLenVec=pMatchRodEnd->pIntelliNode->xPosition-pMatchRodStart->pIntelliNode->xPosition;
				GEPOINT vxLenVec=pMatchAngle->pEnd->xOriginalPos-pMatchAngle->pStart->xOriginalPos;
				if(vxRefLenVec*vxLenVec<0)
				{
					pDesAngleWingNormX=&pMatchAngle->des_norm_y;
					pDesAngleWingNormY=&pMatchAngle->des_norm_x;
					reverseWingXnY=true;
				}
			}
			if(pParaRod->ciSectStyle>1)
			{
				if(pGroupAngle&&pMatchAngle->size_cSubClassType=='T')
					pGroupAngle->ciMasterIndex=!reverseWingXnY?pParaRod->ciMasterIndex:1-pParaRod->ciMasterIndex>0;
			}
			BYTE ciAngleLayout=pParaRod->ciAngleLayout;
			if(pLiveArm->ciOnRightSide=='L'&&ciAngleLayout>=1&&ciAngleLayout<=4)
			{
				ciAngleLayout=5-ciAngleLayout;	//由于模板全部按右横担定义，对称左横担时需要调换角钢摆方方式
				if(pGroupAngle&&pMatchAngle->size_cSubClassType=='T')
					pGroupAngle->ciMasterIndex=1-pGroupAngle->ciMasterIndex;
			}
			GEPOINT vViewNorm(0,1,0);
			if(pParaRod->ciNearNormAxis==1)
				vViewNorm.Set(1,0,0);
			else if(pParaRod->ciNearNormAxis==2)
				vViewNorm.Set(-1,0,0);
			else if(pParaRod->ciNearNormAxis==3)
				vViewNorm.Set(0, 1,0);
			else if(pParaRod->ciNearNormAxis==4)
				vViewNorm.Set(0,-1,0);
			else if(pParaRod->ciNearNormAxis==5)
				vViewNorm.Set(0,0, 1);
			else if(pParaRod->ciNearNormAxis==6)
				vViewNorm.Set(0,0,-1);
			if(pGroupAngle!=NULL)
			{
				pGroupAngle->desStartPos.wing_x_offset.SetOffset(0,4);
				pGroupAngle->desStartPos.wing_y_offset.SetOffset(0,4);
				pGroupAngle->desEndPos.wing_x_offset.SetOffset(0,4);
				pGroupAngle->desEndPos.wing_y_offset.SetOffset(0,4);
			}
			if(ciAngleLayout==1||ciAngleLayout==3)
			{
				pDesAngleWingNormX->bSpecific=pDesAngleWingNormX->bByOtherWing=false;
				if(ciAngleLayout==1)
					pDesAngleWingNormX->near_norm=-vViewNorm;//.Set(0,-1,0);
				else //if(ciAngleLayout==3)
					pDesAngleWingNormX->near_norm= vViewNorm;//.Set(0, 1,0);
				STDTEMPL::MATCHROD* pRefMatchRod=pLiveArm->hashMatchRods.GetValue(pParaRod->widRefRodOfWorkWing);
				if(pRefMatchRod&&pRefMatchRod->relaRods.Count>0)
					pDesAngleWingNormX->hViceJg=pRefMatchRod->relaRods[0].pIntelliRod->handle;
				else if(pMatchAngle->pStart->hFatherPart>0x20&&pMatchAngle->pStart->hFatherPart!=pMatchAngle->handle)
					pDesAngleWingNormX->hViceJg=pMatchAngle->pStart->hFatherPart;
				else if(pMatchAngle->pEnd->hFatherPart>0x20&&pMatchAngle->pEnd->hFatherPart!=pMatchAngle->handle)
					pDesAngleWingNormX->hViceJg=pMatchAngle->pEnd->hFatherPart;
				if(!pParaRod->bHasSectView)
				{
					pDesAngleWingNormY->bSpecific=false;
					pDesAngleWingNormY->bByOtherWing=true;
				}
				else
				{
					CLDSLinePart* pMirXRod=pMatchAngle->GetMirRod(MIRMSG(1));
					if(pMirXRod)
					{
						pDesAngleWingNormY->bSpecific=false;
						pDesAngleWingNormY->bByOtherWing=true;
						if(reverseWingXnY)
							pMatchAngle->cal_x_wing_norm(true);	//先按照不开合角预估一下Y肢法线
						else
							pMatchAngle->cal_y_wing_norm(true);	//先按照不开合角预估一下Y肢法线
						pDesAngleWingNormY->hViceJg=pMirXRod->handle;
						pDesAngleWingNormY->bSpecific=pDesAngleWingNormY->bByOtherWing=false;
						pDesAngleWingNormY->near_norm=reverseWingXnY?pMatchAngle->vxWingNorm:pMatchAngle->vyWingNorm;
					}
					else
					{
						pDesAngleWingNormY->bSpecific=false;
						pDesAngleWingNormY->bByOtherWing=true;
					}
				}
			}
			else if(ciAngleLayout==2||ciAngleLayout==4)
			{
				pDesAngleWingNormY->bSpecific=pDesAngleWingNormY->bByOtherWing=false;
				if(ciAngleLayout==2)
					pDesAngleWingNormY->near_norm= vViewNorm;//.Set(0, 1,0);
				else //if(ciAngleLayout==4)
					pDesAngleWingNormY->near_norm=-vViewNorm;//.Set(0,-1,0);
				STDTEMPL::MATCHROD* pRefMatchRod=pLiveArm->hashMatchRods.GetValue(pParaRod->widRefRodOfWorkWing);
				if(pRefMatchRod&&pRefMatchRod->relaRods.Count>0)
					pDesAngleWingNormY->hViceJg=pRefMatchRod->relaRods[0].pIntelliRod->handle;
				else if(pMatchAngle->pStart->hFatherPart>0x20&&pMatchAngle->pStart->hFatherPart!=pMatchAngle->handle)
					pDesAngleWingNormY->hViceJg=pMatchAngle->pStart->hFatherPart;
				else if(pMatchAngle->pEnd->hFatherPart>0x20&&pMatchAngle->pEnd->hFatherPart!=pMatchAngle->handle)
					pDesAngleWingNormY->hViceJg=pMatchAngle->pEnd->hFatherPart;
				if(!pParaRod->bHasSectView)
				{
					pDesAngleWingNormX->bSpecific=false;
					pDesAngleWingNormX->bByOtherWing=true;
				}
				else
				{
					CLDSLinePart* pMirXRod=pMatchAngle->GetMirRod(MIRMSG(1));
					if(pMirXRod)
					{
						pDesAngleWingNormX->bSpecific=false;
						pDesAngleWingNormX->bByOtherWing=true;
						if(reverseWingXnY)
							pMatchAngle->cal_y_wing_norm(true);	//先按照不开合角预估一下Y肢法线
						else
							pMatchAngle->cal_x_wing_norm(true);	//先按照不开合角预估一下Y肢法线
						pDesAngleWingNormX->hViceJg=pMirXRod->handle;
						pDesAngleWingNormX->bSpecific=pDesAngleWingNormX->bByOtherWing=false;
						pDesAngleWingNormX->near_norm=reverseWingXnY?pMatchAngle->vyWingNorm:pMatchAngle->vxWingNorm;
					}
					else
					{
						pDesAngleWingNormX->bSpecific=false;
						pDesAngleWingNormX->bByOtherWing=true;
					}
				}

			}
			else
				continue;
			pMatchAngle->cal_x_wing_norm();
			pMatchAngle->cal_y_wing_norm();
			if(pGroupAngle)
				pGroupAngle->UpdateSonJgWingNorm();
			pMatchAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
			pMatchAngle->SyncMirProp("wingNormInfo",CLDSPart::MIR_X);
			pMatchAngle->SyncMirProp("Position",CLDSPart::MIR_X);
			if(pHashObjRevision)
			{
				BITFLAG *pRevFlag=pHashObjRevision->Add(pMatchAngle->handle);
				pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
				for(RELATIVE_OBJECT* pRelaObj=pMatchAngle->relativeObjs.GetFirst();pRelaObj;pRelaObj=pMatchAngle->relativeObjs.GetNext())
				{
					if(pRelaObj->mirInfo.axis_flag==0x01)
					{
						pRevFlag=pHashObjRevision->Add(pRelaObj->hObj);
						pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
					}
				}
			}
		}
	}
	//3）处理主材四边形
	for(STDTEMPL::PROFILE_QUADSECT* pQuadSect=pLiveArm->listQuadSects.EnumObjectFirst();pQuadSect;pQuadSect=pLiveArm->listQuadSects.EnumObjectNext())
	{
		MAIN_ANGLE angleArr[4];
		SmartRodPtr pSmartRod1,pSmartRod2;
		STDTEMPL::MATCHROD* pMatchRod1=pLiveArm->hashMatchRods.GetValue(abs(pQuadSect->arrRods[0]));
		angleArr[0].disabled=pQuadSect->arrRods[0]<0;
		if(pMatchRod1==NULL||pMatchRod1->relaRods.Count==0)
			continue;	//个别缺少边界杆件可能导致pMatchRod1->relaRods[0]=NULL
		pSmartRod1=(CLDSLinePart*)pMatchRod1->relaRods[0].pIntelliRod->MappingRod();
		STDTEMPL::MATCHROD* pMatchRod2=pLiveArm->hashMatchRods.GetValue(abs(pQuadSect->arrRods[1]));
		angleArr[1].disabled=pQuadSect->arrRods[1]<0;
		if(pMatchRod2==NULL||pMatchRod2->relaRods.Count==0)
			continue;
		pSmartRod2=(CLDSLinePart*)pMatchRod2->relaRods[0].pIntelliRod->MappingRod();
		angleArr[0].pAngle=(CLDSLineAngle*)pSmartRod1.pRod;
		angleArr[1].pAngle=(CLDSLineAngle*)pSmartRod2.pRod;
		angleArr[2].pAngle=(CLDSLineAngle*)pSmartRod2->GetMirRod(MIRMSG(1));
		angleArr[3].pAngle=(CLDSLineAngle*)pSmartRod1->GetMirRod(MIRMSG(1));
		angleArr[2].disabled=angleArr[1].disabled;
		angleArr[3].disabled=angleArr[0].disabled;
		GEPOINT vxLenStdVec1=pSmartRod1->pEnd->xPreliftPos-pSmartRod1->pStart->xPreliftPos;
		GEPOINT vxLenStdVec2=pSmartRod2->pEnd->xPreliftPos-pSmartRod2->pStart->xPreliftPos;
		normalize(vxLenStdVec1);
		normalize(vxLenStdVec2);
		if(vxLenStdVec1*vxLenStdVec2<0)
			vxLenStdVec2*=-1.0;
		GECS sectcs;
		sectcs.axis_z=vxLenStdVec1+vxLenStdVec2;
		sectcs.axis_z.y=0;
		normalize(sectcs.axis_z);
		sectcs.axis_x.Set(0,1,0);
		sectcs.axis_y=sectcs.axis_z^sectcs.axis_x;
		GEPOINT xSectPlanePick=(angleArr[0].pAngle->pStart->xOriginalPos+angleArr[0].pAngle->pEnd->xOriginalPos)*0.5;
		bool integrityCheck=true;
		for(i=0;i<4;i++)
		{
			if(angleArr[i].pAngle==NULL)
			{
				integrityCheck=false;
				break;
			}
			Int3dlf(angleArr[i].vertex,f3dLine(angleArr[i].pAngle->pStart->xPreliftPos,angleArr[i].pAngle->pEnd->xPreliftPos),
				xSectPlanePick,sectcs.axis_z);	//angleArr[0].pAngle杆件端点有可能与angleArr[1]重合，不宜做截面拾取点
			sectcs.origin+=angleArr[i].vertex*0.25;
		}
		if(!integrityCheck)
		{
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"主材四边形修订时，缺失相应主材件件");
			continue;
		}
		for(i=0;i<4;i++)
		{
			GEPOINT vertex=sectcs.TransPToCS(angleArr[i].vertex);
			angleArr[i].fAngle=Cal2dLineAng(0,0,vertex.x,vertex.y);
		}
		CQuickSort<MAIN_ANGLE>::QuickSortClassic(angleArr,4);
		for(i=0;i<4;i++)
		{
			if(angleArr[i].pAngle->GetClassTypeId()!=CLS_LINEANGLE&&angleArr[i].pAngle->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				continue;	
			if(angleArr[i].disabled)
				continue;
			DESIGN_JGWING_NORM* pNameWingNormX=&angleArr[i].pAngle->des_norm_x;
			DESIGN_JGWING_NORM* pNameWingNormY=&angleArr[i].pAngle->des_norm_y;
			if((angleArr[i].pAngle->pStart->xPreliftPos-angleArr[i].pAngle->pEnd->xPreliftPos)*sectcs.axis_z<0)
			{
				pNameWingNormX=&angleArr[i].pAngle->des_norm_y;
				pNameWingNormY=&angleArr[i].pAngle->des_norm_x;
			}
			pNameWingNormY->hViceJg=angleArr[(i+1)%4].pAngle->handle;
			pNameWingNormX->hViceJg=angleArr[(i+3)%4].pAngle->handle;
			pNameWingNormX->bSpecific=pNameWingNormX->bByOtherWing=false;
			pNameWingNormY->bSpecific=pNameWingNormY->bByOtherWing=false;
			if(i==0)
			{
				pNameWingNormY->near_norm=sectcs.axis_y;
				pNameWingNormX->near_norm=sectcs.axis_x;
			}
			else if(i==1)
			{
				pNameWingNormX->near_norm= sectcs.axis_y;
				pNameWingNormY->near_norm=-sectcs.axis_x;
			}
			else if(i==2)
			{
				pNameWingNormX->near_norm=-sectcs.axis_x;
				pNameWingNormY->near_norm=-sectcs.axis_y;
			}
			else if(i==3)
			{
				pNameWingNormX->near_norm=-sectcs.axis_y;
				pNameWingNormY->near_norm= sectcs.axis_x;
			}
			//4.重新计算角钢肢法线方向
			angleArr[i].pAngle->cal_x_wing_norm();
			angleArr[i].pAngle->cal_y_wing_norm();
			if(angleArr[i].pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
				((CLDSGroupLineAngle*)angleArr[i].pAngle)->UpdateSonJgWingNorm();
			if(pHashObjRevision)
			{
				BITFLAG *pRevFlag=pHashObjRevision->Add(angleArr[i].pAngle->handle);
				pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
				for(RELATIVE_OBJECT* pRelaObj=angleArr[i].pAngle->relativeObjs.GetFirst();pRelaObj;pRelaObj=angleArr[i].pAngle->relativeObjs.GetNext())
				{
					if(pRelaObj->mirInfo.axis_flag==0x01)
					{
						pRevFlag=pHashObjRevision->Add(pRelaObj->hObj);
						pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
					}
				}
			}
			//angleArr[i].pAngle->Create3dSolidModel();
			//g_pSolidDraw->NewSolidPart(angleArr[i].pAngle->GetSolidPartObject());
			//RedrawAngleFrameDrawing(angleArr[i].pAngle);
		}
		STDTEMPL::EDGEROD *pEdgeRod,*pDatumEdgeRod=&pMatchRod1->relaRods[0];
		if(pHashObjRevision)
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod();
			BITFLAG *pRevFlag=pHashObjRevision->Add(pAngle->handle);
			pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
			for(RELATIVE_OBJECT* pRelaObj=pAngle->relativeObjs.GetFirst();pRelaObj;pRelaObj=pAngle->relativeObjs.GetNext())
			{
				if(pRelaObj->mirInfo.axis_flag==0x01)
				{
					pRevFlag=pHashObjRevision->Add(pRelaObj->hObj);
					pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
				}
			}
		}
		for(i=1;pDatumEdgeRod->pIntelliRod->IsAngle()&&i<(int)pMatchRod1->relaRods.Count;i++)
		{
			pEdgeRod=pMatchRod1->relaRods.GetAt(i);
			if(pEdgeRod==NULL||pEdgeRod->pIntelliRod->GetClassTypeId()!=pDatumEdgeRod->pIntelliRod->GetClassTypeId())
				continue;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pEdgeRod->pIntelliRod->MappingRod();
			if(pEdgeRod->bReverseDirection==pDatumEdgeRod->bReverseDirection)
			{
				pAngle->des_norm_x=((CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod())->des_norm_x;
				pAngle->des_norm_y=((CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod())->des_norm_y;
			}
			else
			{
				pAngle->des_norm_x=((CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod())->des_norm_y;
				pAngle->des_norm_y=((CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod())->des_norm_x;
			}
			pAngle->cal_x_wing_norm(true);
			pAngle->cal_y_wing_norm(true);
			pAngle->CalPosition(true);
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
			pAngle->SyncMirProp("wingNormInfo",CLDSPart::MIR_X);
			pAngle->SyncMirProp("Position",CLDSPart::MIR_X);
			if(pHashObjRevision)
			{
				BITFLAG *pRevFlag=pHashObjRevision->Add(pAngle->handle);
				pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
				for(RELATIVE_OBJECT* pRelaObj=pAngle->relativeObjs.GetFirst();pRelaObj;pRelaObj=pAngle->relativeObjs.GetNext())
				{
					if(pRelaObj->mirInfo.axis_flag==0x01)
					{
						pRevFlag=pHashObjRevision->Add(pRelaObj->hObj);
						pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
					}
				}
			}
		}
		pDatumEdgeRod=&pMatchRod2->relaRods[0];
		if(pHashObjRevision)
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod();
			BITFLAG *pRevFlag=pHashObjRevision->Add(pAngle->handle);
			pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
			for(RELATIVE_OBJECT* pRelaObj=pAngle->relativeObjs.GetFirst();pRelaObj;pRelaObj=pAngle->relativeObjs.GetNext())
			{
				if(pRelaObj->mirInfo.axis_flag==0x01)
				{
					pRevFlag=pHashObjRevision->Add(pRelaObj->hObj);
					pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
				}
			}
		}
		for(i=1;pDatumEdgeRod->pIntelliRod->IsAngle()&&i<(int)pMatchRod2->relaRods.Count;i++)
		{
			pEdgeRod=pMatchRod2->relaRods.GetAt(i);
			if(pEdgeRod==NULL||pEdgeRod->pIntelliRod->GetClassTypeId()!=pDatumEdgeRod->pIntelliRod->GetClassTypeId())
				continue;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pEdgeRod->pIntelliRod->MappingRod();
			if(pEdgeRod->bReverseDirection==pDatumEdgeRod->bReverseDirection)
			{
				pAngle->des_norm_x=((CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod())->des_norm_x;
				pAngle->des_norm_y=((CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod())->des_norm_y;
			}
			else
			{
				pAngle->des_norm_x=((CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod())->des_norm_y;
				pAngle->des_norm_y=((CLDSLineAngle*)pDatumEdgeRod->pIntelliRod->MappingRod())->des_norm_x;
			}
			pAngle->cal_x_wing_norm(true);
			pAngle->cal_y_wing_norm(true);
			pAngle->CalPosition(true);
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
			pAngle->SyncMirProp("wingNormInfo",CLDSPart::MIR_X);
			pAngle->SyncMirProp("Position",CLDSPart::MIR_X);
			if(pHashObjRevision)
			{
				BITFLAG *pRevFlag=pHashObjRevision->Add(pAngle->handle);
				pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
				for(RELATIVE_OBJECT* pRelaObj=pAngle->relativeObjs.GetFirst();pRelaObj;pRelaObj=pAngle->relativeObjs.GetNext())
				{
					if(pRelaObj->mirInfo.axis_flag==0x01)
					{
						pRevFlag=pHashObjRevision->Add(pRelaObj->hObj);
						pRevFlag->SetFlagDword(0x00000008);//LAYOUT	=0x08;	//角钢肢朝向
					}
				}
			}
		}
	}
	//3)根据共面信息重置杆件位置
	for(pParaRod=pLiveArm->hashParaRods.GetFirst();pParaRod;pParaRod=pLiveArm->hashParaRods.GetNext())
	{
		if(!pParaRod->desPosStart.blPresetParams && !pParaRod->desPosEnd.blPresetParams)
			continue;
		if((pMatchRod=pLiveArm->hashMatchRods.GetValue(pParaRod->widRod))==NULL)
			continue;
		for(UINT i=0;i<pMatchRod->relaRods.Count;i++)
		{
			if((pEdgeRod=pMatchRod->relaRods.GetAt(i))==NULL||!pEdgeRod->pIntelliRod->IsAngle())
				continue;
			CLDSLineAngle* pMatchAngle=(CLDSLineAngle*)pEdgeRod->pIntelliRod->MappingRod();
			if(pParaRod->desPosStart.cTouchLineType==4)
			{	//共面点
				CLDSNode* pTipNode=pEdgeRod->bReverseDirection?pMatchAngle->pEnd:pMatchAngle->pStart;
				CMultiOffsetPos* pCurrDesPos=pEdgeRod->bReverseDirection?&pMatchAngle->desEndPos:&pMatchAngle->desStartPos;
				WING_OFFSET_STYLE* pDesWingXOffset=pEdgeRod->bReverseDirection?&pCurrDesPos->wing_y_offset:&pCurrDesPos->wing_x_offset;
				WING_OFFSET_STYLE* pDesWingYOffset=pEdgeRod->bReverseDirection?&pCurrDesPos->wing_x_offset:&pCurrDesPos->wing_y_offset;
				if(pTipNode&&pCurrDesPos->spatialOperationStyle!=0)
				{
					pCurrDesPos->spatialOperationStyle=0;	//基准点
					if(pParaRod->desPosStart.datumpoint.ciCoordType==0)
					{	//指定YZ坐标分量
						pCurrDesPos->datumPoint.datum_pos.y=pTipNode->xOriginalPos.y;
						pCurrDesPos->datumPoint.datum_pos.z=pTipNode->xOriginalPos.z;
					}
					else if(pParaRod->desPosStart.datumpoint.ciCoordType==1)
					{	//指定XZ坐标分量
						pCurrDesPos->datumPoint.datum_pos.x=pTipNode->xOriginalPos.x;
						pCurrDesPos->datumPoint.datum_pos.z=pTipNode->xOriginalPos.z;
					}
					else if(pParaRod->desPosStart.datumpoint.ciCoordType==2)
					{	//指定XY坐标分量
						pCurrDesPos->datumPoint.datum_pos.x=pTipNode->xOriginalPos.x;
						pCurrDesPos->datumPoint.datum_pos.y=pTipNode->xOriginalPos.y;
					}
				}
				pCurrDesPos->datumPoint.datum_pos_style=6;	//基准面上轴向坐标不变点
				pCurrDesPos->datumPoint.des_para.DATUMPLANE.hDatumPlane=arrayPlanes[pParaRod->desPosStart.datumpoint.widPlane-1];
				if(pParaRod->desPosStart.datumpoint.ciCoordType==0)//指定YZ坐标分量
					pCurrDesPos->datumPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=0;
				else if(pParaRod->desPosStart.datumpoint.ciCoordType==1)//指定XZ坐标分量
					pCurrDesPos->datumPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=1;
				else if(pParaRod->desPosStart.datumpoint.ciCoordType==2)		//指定XY坐标分量
					pCurrDesPos->datumPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=2;
				//
				if(pParaRod->desPosStart.szXorCtrlWingOffset[0]=='g')
				{
					if(pParaRod->desPosStart.szXorCtrlWingOffset[1]>='1'&&pParaRod->desPosStart.szXorCtrlWingOffset[1]<='3')
						pDesWingXOffset->gStyle=pParaRod->desPosStart.szXorCtrlWingOffset[1]-'0';
					else
						pDesWingXOffset->gStyle=0;
				}
				else
				{
					pDesWingXOffset->gStyle=4;
					pDesWingXOffset->offsetDist=atoi(pParaRod->desPosStart.szXorCtrlWingOffset);
				}
				if(pParaRod->desPosStart.szYorOthrWingOffset[0]=='g')
				{
					if(pParaRod->desPosStart.szYorOthrWingOffset[1]>='1'&&pParaRod->desPosStart.szYorOthrWingOffset[1]<='3')
						pDesWingYOffset->gStyle=pParaRod->desPosStart.szYorOthrWingOffset[1]-'0';
					else
						pDesWingYOffset->gStyle=0;
				}
				else
				{
					pDesWingYOffset->gStyle=4;
					pDesWingYOffset->offsetDist=atoi(pParaRod->desPosStart.szYorOthrWingOffset);
				}
			}
			if(pParaRod->desPosEnd.cTouchLineType==4)
			{	//共面点
				CLDSNode* pTipNode=pEdgeRod->bReverseDirection?pMatchAngle->pStart:pMatchAngle->pEnd;
				CMultiOffsetPos* pCurrDesPos=pEdgeRod->bReverseDirection?&pMatchAngle->desStartPos:&pMatchAngle->desEndPos;
				WING_OFFSET_STYLE* pDesWingXOffset=pEdgeRod->bReverseDirection?&pCurrDesPos->wing_y_offset:&pCurrDesPos->wing_x_offset;
				WING_OFFSET_STYLE* pDesWingYOffset=pEdgeRod->bReverseDirection?&pCurrDesPos->wing_x_offset:&pCurrDesPos->wing_y_offset;

				if(pTipNode&&pCurrDesPos->spatialOperationStyle!=0)
				{
					pCurrDesPos->spatialOperationStyle=0;	//基准点
					if(pParaRod->desPosStart.datumpoint.ciCoordType==0)
					{	//指定YZ坐标分量
						pCurrDesPos->datumPoint.datum_pos.y=pTipNode->xOriginalPos.y;
						pCurrDesPos->datumPoint.datum_pos.z=pTipNode->xOriginalPos.z;
					}
					else if(pParaRod->desPosStart.datumpoint.ciCoordType==1)
					{	//指定XZ坐标分量
						pCurrDesPos->datumPoint.datum_pos.x=pTipNode->xOriginalPos.x;
						pCurrDesPos->datumPoint.datum_pos.z=pTipNode->xOriginalPos.z;
					}
					else if(pParaRod->desPosStart.datumpoint.ciCoordType==2)
					{	//指定XY坐标分量
						pCurrDesPos->datumPoint.datum_pos.x=pTipNode->xOriginalPos.x;
						pCurrDesPos->datumPoint.datum_pos.y=pTipNode->xOriginalPos.y;
					}
				}
				pCurrDesPos->spatialOperationStyle=0;	//基准点
				pCurrDesPos->datumPoint.datum_pos_style=6;	//基准面上轴向坐标不变点
				pCurrDesPos->datumPoint.des_para.DATUMPLANE.hDatumPlane=arrayPlanes[pParaRod->desPosEnd.datumpoint.widPlane-1];
				if(pParaRod->desPosEnd.datumpoint.ciCoordType==0)		//指定XY坐标分量
					pCurrDesPos->datumPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=2;
				else if(pParaRod->desPosEnd.datumpoint.ciCoordType==1)//指定YZ坐标分量
					pCurrDesPos->datumPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=0;
				else if(pParaRod->desPosEnd.datumpoint.ciCoordType==2)//指定XZ坐标分量
					pCurrDesPos->datumPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=1;
				//
				if(pParaRod->desPosEnd.szXorCtrlWingOffset[0]=='g')
				{
					if(pParaRod->desPosEnd.szXorCtrlWingOffset[1]>='1'&&pParaRod->desPosEnd.szXorCtrlWingOffset[1]<='3')
						pDesWingXOffset->gStyle=pParaRod->desPosEnd.szXorCtrlWingOffset[1]-'0';
					else
						pDesWingXOffset->gStyle=0;
				}
				else
				{
					pDesWingXOffset->gStyle=4;
					pDesWingXOffset->offsetDist=atoi(pParaRod->desPosEnd.szXorCtrlWingOffset);
				}
				if(pParaRod->desPosEnd.szYorOthrWingOffset[0]=='g')
				{
					if(pParaRod->desPosEnd.szYorOthrWingOffset[1]>='1'&&pParaRod->desPosEnd.szYorOthrWingOffset[1]<='3')
						pDesWingYOffset->gStyle=pParaRod->desPosEnd.szYorOthrWingOffset[1]-'0';
					else
						pDesWingYOffset->gStyle=0;
				}
				else
				{
					pDesWingYOffset->gStyle=4;
					pDesWingYOffset->offsetDist=atoi(pParaRod->desPosEnd.szYorOthrWingOffset);
				}
			}
			pMatchAngle->CalPosition(true);
			pMatchAngle->SyncMirProp("Position",CLDSPart::MIR_X);
		}
	}
	return true;
}
#endif
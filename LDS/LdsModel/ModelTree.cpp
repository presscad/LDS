// Bolt.cpp: implementation of the C3dLs class.
//wbt translation
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tower.h"
#include "ModelTree.h"
#include "SortFunc.h"
#include "XhLdsLm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CModelSegmentNode::CModelSegmentNode()
{
	iSeg=0;
	pPostRod=NULL;
	m_pBelongHeight=NULL;
	wFrontAngleG=wSideAngleG=0;	//1象限主材角钢正侧面肢定位基准心距（组合角钢及钢管为0）
}
double CModelSegmentNode::get_fTopZ()
{
	if(pPostRod==NULL)
		return 0;
	if(pPostRod->pStart->xOriginalPos.z<pPostRod->pEnd->xOriginalPos.z)
		return pPostRod->pStart->xOriginalPos.z;
	else
		return pPostRod->pEnd->xOriginalPos.z;
}
double CModelSegmentNode::get_fSegHeight()
{
	if(pPostRod==NULL)
		return 0;
	return fabs(pPostRod->pEnd->xOriginalPos.z-pPostRod->pStart->xOriginalPos.z);
}
WORD CModelSegmentNode::get_wActualFrontAngleG()
{
	if(this->syncflag.IsSpcecifyAngleG())
		return this->wFrontAngleG;
	else if(pPostRod!=NULL)
	{
		char ciFrontWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'X':'Y';
		if(pPostRod->GetClassTypeId()!=CLS_LINEANGLE)
			return 0;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
		JGZJ jgzj=pAngle->GetZhunJu(ciFrontWing);
		return (WORD)jgzj.g;
	}
	else
		return this->wPreferAngleG;
}
WORD CModelSegmentNode::get_wActualSideAngleG()
{
	if(this->syncflag.IsSpcecifyAngleG())
		return this->wSideAngleG;
	else if(pPostRod!=NULL)
	{
		char ciSideWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'Y':'X';
		if(pPostRod->GetClassTypeId()!=CLS_LINEANGLE)
			return 0;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
		JGZJ jgzj=pAngle->GetZhunJu(ciSideWing);
		return (WORD)jgzj.g;
	}
	else
		return this->wPreferAngleG;
}
WORD CModelSegmentNode::get_wPreferAngleG()
{
	if(pPostRod==NULL||pPostRod->GetClassTypeId()!=CLS_LINEANGLE)
		return 0;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	JGZJ jgzj=CLDSLineAngle::GetAngleZJ(pAngle->size_wide);
	if(pAngle->size_wide<140)
		return (WORD)jgzj.g;
	else
		return (WORD)jgzj.g1;
}
double CModelSegmentNode::get_fTopSizeX()
{	//分段正面上开口
	if(pPostRod==NULL)
		return 0;
	CLDSNode* pBaseNode=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?pPostRod->pStart:pPostRod->pEnd;
	if(!pPostRod->IsAngle()||pPostRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		return fabs(pBaseNode->xOriginalPos.x);
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	char ciCurrWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'X':'Y';
	GEPOINT vCurrWingVec=ciCurrWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	JGZJ jgzj;
	if(syncflag.IsSpcecifyAngleG())
		jgzj.g=wFrontAngleG;
	else
		jgzj=pAngle->GetZhunJu(ciCurrWing);
	GEPOINT inters;
	f3dLine axisline(pPostRod->xPosStart+vCurrWingVec*jgzj.g,pPostRod->xPosEnd+vCurrWingVec*jgzj.g);
	Int3dlf(inters,axisline,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
	return fabs(inters.x)*2;
}
double CModelSegmentNode::get_fTopSizeY()
{	//分段侧面上开口
	if(pPostRod==NULL)
		return 0;
	CLDSNode* pBaseNode=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?pPostRod->pStart:pPostRod->pEnd;
	if(!pPostRod->IsAngle()||pPostRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		return fabs(pBaseNode->xOriginalPos.x);
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	char ciCurrWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'Y':'X';
	GEPOINT vCurrWingVec=ciCurrWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	JGZJ jgzj;
	if(syncflag.IsSpcecifyAngleG())
		jgzj.g=wSideAngleG;
	else
		jgzj=pAngle->GetZhunJu(ciCurrWing);
	GEPOINT inters;
	f3dLine axisline(pPostRod->xPosStart+vCurrWingVec*jgzj.g,pPostRod->xPosEnd+vCurrWingVec*jgzj.g);
	Int3dlf(inters,axisline,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
	return fabs(inters.y)*2;
}
double CModelSegmentNode::get_fBtmSizeX()
{	//分段正面下开口
	if(pPostRod==NULL)
		return 0;
	CLDSNode* pBaseNode=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?pPostRod->pEnd:pPostRod->pStart;
	if(!pPostRod->IsAngle()||pPostRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		return fabs(pBaseNode->xOriginalPos.x);
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	char ciCurrWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'X':'Y';
	GEPOINT vCurrWingVec=ciCurrWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	JGZJ jgzj;
	if(syncflag.IsSpcecifyAngleG())
		jgzj.g=wFrontAngleG;
	else
		jgzj=pAngle->GetZhunJu(ciCurrWing);
	GEPOINT inters;
	f3dLine axisline(pPostRod->xPosStart+vCurrWingVec*jgzj.g,pPostRod->xPosEnd+vCurrWingVec*jgzj.g);
	Int3dlf(inters,axisline,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
	return fabs(inters.x)*2;
}
double CModelSegmentNode::get_fBtmSizeY()
{	//分段侧面下开口
	if(pPostRod==NULL)
		return 0;
	CLDSNode* pBaseNode=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?pPostRod->pEnd:pPostRod->pStart;
	if(!pPostRod->IsAngle()||pPostRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		return fabs(pBaseNode->xOriginalPos.x);
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	char ciCurrWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'Y':'X';
	GEPOINT vCurrWingVec=ciCurrWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	JGZJ jgzj;
	if(syncflag.IsSpcecifyAngleG())
		jgzj.g=wSideAngleG;
	else
		jgzj=pAngle->GetZhunJu(ciCurrWing);
	GEPOINT inters;
	f3dLine axisline(pPostRod->xPosStart+vCurrWingVec*jgzj.g,pPostRod->xPosEnd+vCurrWingVec*jgzj.g);
	Int3dlf(inters,axisline,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
	return fabs(inters.y)*2;
}
double CModelSubLegNode::get_fTopSizeX()
{	//分段正面上开口
	if(pPostRod==NULL)
		return 0;
	CLDSNode* pBaseNode=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?pPostRod->pStart:pPostRod->pEnd;
	if(!pPostRod->IsAngle()||pPostRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		return fabs(pBaseNode->xOriginalPos.x);
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	char ciCurrWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'X':'Y';
	GEPOINT vCurrWingVec=ciCurrWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	JGZJ jgzj;
	if(syncflag.IsSpcecifyAngleG())
		jgzj.g=wFrontAngleG;
	else
		jgzj=pAngle->GetZhunJu(ciCurrWing);
	GEPOINT inters;
	f3dLine axisline(pPostRod->xPosStart+vCurrWingVec*jgzj.g,pPostRod->xPosEnd+vCurrWingVec*jgzj.g);
	Int3dlf(inters,axisline,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
	return fabs(inters.x)*2;
}
double CModelSubLegNode::get_fTopSizeY()
{	//分段侧面上开口
	if(pPostRod==NULL)
		return 0;
	CLDSNode* pBaseNode=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?pPostRod->pStart:pPostRod->pEnd;
	if(!pPostRod->IsAngle()||pPostRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		return fabs(pBaseNode->xOriginalPos.x);
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	char ciCurrWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'Y':'X';
	GEPOINT vCurrWingVec=ciCurrWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	JGZJ jgzj;
	if(syncflag.IsSpcecifyAngleG())
		jgzj.g=wSideAngleG;
	else
		jgzj=pAngle->GetZhunJu(ciCurrWing);
	GEPOINT inters;
	f3dLine axisline(pPostRod->xPosStart+vCurrWingVec*jgzj.g,pPostRod->xPosEnd+vCurrWingVec*jgzj.g);
	Int3dlf(inters,axisline,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
	return fabs(inters.y)*2;
}
double CModelSubLegNode::get_fBtmSizeX()
{	//分段正面下开口
	return this->fHalfFrontWidth*2;
}
double CModelSubLegNode::get_fBtmSizeY()
{	//分段侧面下开口
	return this->fHalfSideWidth*2;
}
double CModelSubLegNode::get_fBaseSizeX()
{
	if(!syncflag.IsSpcecifyBasePos())
		UpdateFoundationPosition();
	return fHalfFrontBaseWidth*2;
}
double CModelSubLegNode::get_fBaseSizeY()
{
	if(!syncflag.IsSpcecifyBasePos())
		UpdateFoundationPosition();
	return this->fHalfSideBaseWidth*2;
}
WORD CModelSubLegNode::get_wActualFrontAngleG()
{
	if(this->syncflag.IsSpcecifyAngleG())
		return this->wFrontAngleG;
	else if(pPostRod!=NULL)
	{
		char ciFrontWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'X':'Y';
		if(pPostRod->GetClassTypeId()!=CLS_LINEANGLE)
			return 0;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
		JGZJ jgzj=pAngle->GetZhunJu(ciFrontWing);
		return (WORD)jgzj.g;
	}
	else
		return this->wPreferAngleG;
}
WORD CModelSubLegNode::get_wActualSideAngleG()
{
	if(this->syncflag.IsSpcecifyAngleG())
		return this->wSideAngleG;
	else if(pPostRod!=NULL)
	{
		char ciSideWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'Y':'X';
		if(pPostRod->GetClassTypeId()!=CLS_LINEANGLE)
			return 0;
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
		JGZJ jgzj=pAngle->GetZhunJu(ciSideWing);
		return (WORD)jgzj.g;
	}
	else
		return wPreferAngleG;
}
WORD CModelSubLegNode::get_wPreferAngleG()
{
	if(pPostRod==NULL||pPostRod->GetClassTypeId()!=CLS_LINEANGLE)
		return 0;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	JGZJ jgzj=CLDSLineAngle::GetAngleZJ(pAngle->size_wide);
	if(pAngle->size_wide<140)
		return (WORD)jgzj.g;
	else
		return (WORD)jgzj.g1;
}
bool CModelSegmentNode::AppendSubTreeNode(LINEPARTSET& rodset,UINT idSiblingBodyNo)
{
	CFGWORD d;
	double topz_upper=min(pPostRod->pStart->xOriginalPos.z,pPostRod->pEnd->xOriginalPos.z);
	double btmz_upper=max(pPostRod->pStart->xOriginalPos.z,pPostRod->pEnd->xOriginalPos.z);
	BOOL pushed=rodset.push_stack();
	for(CLDSLinePart* pRod=rodset.GetCursor();pRod;pRod=rodset.GetNext())
	{
		if((pRod->cfgword.IsHasBodyNo(idSiblingBodyNo)||!pPostRod->ModuleTogetherWith(pRod)))
			continue;	//属于同级分段杆件或不共用于任何呼高
		double topz_curr=min(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
		//double btmz_curr=max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
		if(fabs(btmz_upper-topz_curr)>1)	//没有上下连接关系
			continue;
		BYTE bodyIds[24]={0};
		int i,count=pRod->cfgword.GetExistBodyNos(bodyIds,24);
		if(!pRod->IsLegObj())
		{
			CModelSegmentNode* pSegment=this->m_pTreeModel->AppendSegmentRod(pRod);
			if(pSegment->pBelongHeight!=NULL)
				hashHeightSet.SetValue(pSegment->pBelongHeight->Id,pSegment->pBelongHeight);
			else
			{
				hashMutexSubSegments.SetValue(pSegment->Id,pSegment);
				if(count>0)
					pSegment->AppendSubTreeNode(rodset,bodyIds[count-1]);
			}
		}
		else
		{
			for(i=0;i<count;i++)
			{
				CModelHeightNode* pHeight=m_pTreeModel->hashHeights.GetValue(bodyIds[i]);
				if(pHeight!=NULL)
					hashHeightSet.SetValue(bodyIds[i],pHeight);
			}
		}
	}
	if(pushed)
		rodset.pop_stack(pushed);
	return true;
}
CModelSubLegNode::CModelSubLegNode(UINT iLegSerial)
{
	idLegSerial=iLegSerial;
	iSeg=0;
	m_hRelaFoundation=0;
	wFrontAngleG=wSideAngleG=0;
	minz=maxz=fLegHeight=zFoundationTopPlane=0;
	m_arrPostRods[0]=m_arrPostRods[1]=m_arrPostRods[2]=m_arrPostRods[3]=NULL;
	fHalfFrontBaseWidth=fHalfSideBaseWidth=0;
}
UINT CModelSubLegNode::get_idModelLegSerial()	//接腿在全局模型中的接腿号
{
	BYTE nMaxLegs = CFGLEG::MaxLegs();
	if (m_pBelongHeight != NULL)
		return (m_pBelongHeight->Id - 1)*nMaxLegs + idLegSerial;
	logerr.Log("error");
	return idLegSerial;
}
WORD CModelSubLegNode::get_wRodCount(){
	WORD count=0;
	while(count<4&&m_arrPostRods[count]!=NULL)
		count++;
	return count;
};
bool Test_IdentifyPantsLeg(CTower* pTower,CLDSLinePart *pPostRod,CModelSubLegNode::PANTSLEG* pPantsLeg);
bool CModelSubLegNode::IdentifyPantsLeg(bool blClearLegacyState/*=true*/)
{	//识别当前接腿是否为大裤衩结构的换接腿
	if (!blClearLegacyState)
		return pants.pVLeftTopNode!=NULL&&pants.pVRightTopNode!=NULL&&pants.pLeftHoriAngle!=NULL&&pants.pRightHoriAngle!=NULL;
	CTower* pTower=NULL;
	if (m_pBelongHeight!=NULL&&m_pBelongHeight->pTreeModel!=NULL)
		pTower=m_pBelongHeight->pTreeModel->pTower;
	CLDSLinePart* pLivePostRod=pPostRod;	//此行作用是为了避免重复查找角主材
	return Test_IdentifyPantsLeg(pTower,pPostRod,&pants);
}
static bool LocalTestRodNodeColinear(CLDSLinePart* pRod,CLDSNode* pNode)
{
	if (pRod->pStart==NULL||pRod->pEnd==NULL||pNode==NULL)
		return false;
	if (pRod->pStart==pNode||pRod->pEnd==pNode)
		return true;
	GEPOINT nv=pRod->pEnd->xOriginalPos-pRod->pStart->xOriginalPos;
	GEPOINT dv=pNode->xOriginalPos-pRod->pStart->xOriginalPos;
	if (nv.IsZero()||dv.IsZero())
		return true;
	normalize(nv);
	dv-=(dv*nv)*nv;
	return dv.IsZero();
}
bool Test_IdentifyPantsLeg(CTower* pTower,CLDSLinePart *pLivePostRod,CModelSubLegNode::PANTSLEG* pPantsLeg)
{
	CLDSNode* pBaseNode=pLivePostRod->xPosEnd.z>pLivePostRod->xPosStart.z?pLivePostRod->pEnd:pLivePostRod->pStart;
	GEPOINT vLeftPlane,vRightPlane;
	GEPOINT vCornerRodVec=pLivePostRod->pEnd->xOriginalPos-pLivePostRod->pStart->xOriginalPos;
	normalize(vCornerRodVec);
	if (vCornerRodVec.z>0)	//保证主材脊线方向朝上
		vCornerRodVec*=-1.0;
	if (pBaseNode->xOriginalPos.x>0&&pBaseNode->xOriginalPos.y>0)
	{	//1象限 vCornerRodVec(-0.01,-0.01,-1),L(0.00,1,-0.01),R(1,0.00,-0.01)
		vLeftPlane.Set(0,-vCornerRodVec.z,vCornerRodVec.y);
		vRightPlane.Set(-vCornerRodVec.z,0,vCornerRodVec.x);
	}
	else if (pBaseNode->xOriginalPos.x<0&&pBaseNode->xOriginalPos.y>0)
	{	//2象限 vCornerRodVec( 0.01,-0.01,-1),L(-1,0.00,-0.01),R(0.00,1,-0.01)
		vLeftPlane.Set(vCornerRodVec.z,0,-vCornerRodVec.x);
		vRightPlane.Set(0,-vCornerRodVec.z,vCornerRodVec.y);
	}
	else if (pBaseNode->xOriginalPos.x>0&&pBaseNode->xOriginalPos.y<0)
	{	//3象限 vCornerRodVec(-0.01, 0.01,-1),L(1,0.00,-0.01),R(0.00,-1,-0.01)
		vLeftPlane.Set(-vCornerRodVec.z,0, vCornerRodVec.x);
		vRightPlane.Set(0,vCornerRodVec.z,-vCornerRodVec.y);
	}
	else if (pBaseNode->xOriginalPos.x<0&&pBaseNode->xOriginalPos.y<0)
	{	//4象限 vCornerRodVec( 0.01, 0.01,-1),L(0.00,-1,-0.01),R(-1,0.00,-0.01)
		vLeftPlane.Set(0,vCornerRodVec.z,-vCornerRodVec.y);
		vRightPlane.Set(vCornerRodVec.z,0,-vCornerRodVec.x);
	}
	normalize(vLeftPlane);
	normalize(vRightPlane);
	SmartRodPtr pLeftPrimaryRod,pRightPrimaryRod;
	CLDSLinePart* pRod;
	CMinDouble minLegZ;
	for(pRod=pTower->EnumRodFirst();pRod;pRod=pTower->EnumRodNext())
	{
		if( pRod==pLivePostRod||pRod->pStart==NULL||pRod->pEnd==NULL||
			pRod->m_bVirtualPart||!pRod->IsLegObj()||!pRod->IsPrimaryRod())
			continue;
		if(!pLivePostRod->ModuleTogetherWith(pRod))
			continue;
		minLegZ.Update(pRod->pStart->Position('Z'));
		minLegZ.Update(pRod->pEnd->Position('Z'));
		if (!pLeftPrimaryRod.blValid||!pRightPrimaryRod.blValid)
		{
			char ciTipStart0End1=-1;
			if(pRod->pStart==pBaseNode)
				ciTipStart0End1=0;
			else if(pRod->pEnd==pBaseNode)
				ciTipStart0End1=1;
			else
				continue;
			GEPOINT lenRayVec=ciTipStart0End1==0?pRod->pEnd->xOriginalPos-pRod->pStart->xOriginalPos:	\
				pRod->pStart->xOriginalPos-pRod->pEnd->xOriginalPos;
			normalize(lenRayVec);
			if (fabs(lenRayVec*vCornerRodVec)>0.999)
				continue;	//角主材
			else if (!pLeftPrimaryRod.blValid&&fabs(lenRayVec*vLeftPlane)<0.001)	//左面射线杆件
				pLeftPrimaryRod=pRod;
			else if (!pRightPrimaryRod.blValid&&fabs(lenRayVec*vRightPlane)<0.001)	//左面射线杆件
				pRightPrimaryRod=pRod;
			else
				continue;
		}
	}
	if (!pLeftPrimaryRod.blValid||!pRightPrimaryRod.blValid)
		return false;
	//提取原则:腿部基础节点相连的两V斜材延伸至横隔面的节点
	CLDSNode *pNode,*pVLeftTopNode=NULL,*pVRightTopNode=NULL,*pLegPostTopNode=NULL;
	/*for(pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
	{
		//1、获取腿部基准节点
		GEPOINT pos=pNode->xOriginalPos;
		if(pNode->IsBodyObj()&&pos.z<minLegZ.number-EPS)
			continue;
		else if(!pLivePostRod->ModuleTogetherWith(pNode))
			continue;	//不在当前呼高的激活接腿中
		//else if(pNode->ciLegQuad!=pLivePostRod->ciLegQuad)
		//	continue;	//不在当前象限
		GEPOINT vv=pos-pBaseNode->xOriginalPos;
		if( (fabs(pos.x)<EPS&&(pLivePostRod->ciLegQuad==1||pLivePostRod->ciLegQuad==4))||
			(fabs(pos.y)<EPS&&(pLivePostRod->ciLegQuad==2||pLivePostRod->ciLegQuad==3)))
		{
			if (fabs(vv*vLeftPlane)>1)
				continue;	//不在外坡面上
			if(pVLeftTopNode==NULL)
				pVLeftTopNode=pNode;
			else if(pos.z<pVLeftTopNode->xOriginalPos.z)
				pVLeftTopNode=pNode;
		}else
		if( (fabs(pos.y)<EPS&&(pLivePostRod->ciLegQuad==1||pLivePostRod->ciLegQuad==4))||
			(fabs(pos.x)<EPS&&(pLivePostRod->ciLegQuad==2||pLivePostRod->ciLegQuad==3)))
		{
			if (fabs(vv*vRightPlane)>1)
				continue;	//不在外坡面上
			if(pVRightTopNode==NULL)
				pVRightTopNode=pNode;
		}
		if (pVLeftTopNode!=NULL&&pVRightTopNode!=NULL)
			break;
	}*/
	//大裤衩接腿需要根据斜材查找V面节点 wht 19-01-25
	if(pVLeftTopNode==NULL)
	{
		if(pLeftPrimaryRod->pStart->xOriginalPos.z<pLeftPrimaryRod->pEnd->xOriginalPos.z)
			pVLeftTopNode=pLeftPrimaryRod->pStart;
		else
			pVLeftTopNode=pLeftPrimaryRod->pEnd;
	}
	if(pVRightTopNode==NULL)
	{
		if(pRightPrimaryRod->pStart->xOriginalPos.z<pRightPrimaryRod->pEnd->xOriginalPos.z)
			pVRightTopNode=pRightPrimaryRod->pStart;
		else
			pVRightTopNode=pRightPrimaryRod->pEnd;
	}
	CPtInLineCheck vleftline(pBaseNode->xOriginalPos,pVLeftTopNode->xOriginalPos);
	CPtInLineCheck vrightline(pBaseNode->xOriginalPos,pVRightTopNode->xOriginalPos);
	double dfMaxScaleOfLeft=1.0,dfMaxScaleOfRight=1.0;
	for (pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
	{
		//1、获取腿部基准节点
		f3dPoint pos=pNode->Position(false);
		if (fabs(pos.z-minLegZ.number)>EPS)
			continue;	//不在腿身过渡平面上
		else if (pNode->IsLegObj()&&pNode->ciLegQuad!=pLivePostRod->ciLegQuad)
			continue;	//不在当前象限
		else if (!pBaseNode->ModuleTogetherWith(pNode))
			continue;	//不在当前呼高的激活接腿中
		if (pNode==pVLeftTopNode||pNode==pVRightTopNode)
			continue;
		bool online=false;
		double scale=vleftline.CheckPointEx(pos,&online);
		if (online&&scale>dfMaxScaleOfLeft)
		{	//Ｖ斜材左侧面顶部节点
			dfMaxScaleOfLeft=scale;
			pVLeftTopNode=pNode;
			continue;
		}
		scale=vrightline.CheckPointEx(pos,&online);
		if (online&&scale>dfMaxScaleOfRight)
		{	//Ｖ斜材右侧面顶部节点
			dfMaxScaleOfRight=scale;
			pVRightTopNode=pNode;
		}
	}
	CPtInLineCheck vpostline(pLivePostRod->pStart->xOriginalPos,pLivePostRod->pEnd->xOriginalPos);
	for (pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
	{	//获取接腿主材顶部节点
		if (fabs(pNode->xOriginalPos.z-minLegZ.number)>EPS)
			continue;	//不在腿身过渡平面上
		else if (pNode->IsLegObj()&&pNode->ciLegQuad!=pLivePostRod->ciLegQuad)
			continue;	//不在当前象限
		else if (!pBaseNode->ModuleTogetherWith(pNode))
			continue;	//不在当前呼高的激活接腿中
		bool online=false;
		vpostline.CheckPointEx(pNode->xOriginalPos,&online);
		if (online)	//塔腿主材顶部节点
		{
			pLegPostTopNode=pNode;
			break;
		}
	}
	//查找塔腿上左右正侧面的小横材（设为定位基准杆件）
	CLDSLinePart *pLeftHoriAngle=NULL,*pRightHoriAngle=NULL;
	for (pRod=pTower->EnumRodFirst();pRod;pRod=pTower->EnumRodNext())
	{
		if (pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart)
			continue;
		if (!pLivePostRod->ModuleTogetherWith(pRod))
			continue;
		GEPOINT vrod=pRod->pStart->xOriginalPos-pRod->pEnd->xOriginalPos;
		if (fabs(vrod.z)>EPS)
			continue;
		if (pBaseNode->ciLegQuad==1||pBaseNode->ciLegQuad==4)
		{
			if (pRod->pStart==pVLeftTopNode&&fabs(vrod.y)<EPS&&
				fabs(pRod->pEnd->xOriginalPos.x)-fabs(pRod->pStart->xOriginalPos.x)>EPS)
				pLeftHoriAngle=pRod;
			else if (pRod->pEnd==pVLeftTopNode&&fabs(vrod.y)<EPS&&
				fabs(pRod->pStart->xOriginalPos.x)-fabs(pRod->pEnd->xOriginalPos.x)>EPS)
				pLeftHoriAngle=pRod;
			if (pRod->pStart==pVRightTopNode&&fabs(vrod.x)<EPS&&
				fabs(pRod->pEnd->xOriginalPos.y)-fabs(pRod->pStart->xOriginalPos.y)>EPS)
				pRightHoriAngle=pRod;
			else if (pRod->pEnd==pVRightTopNode&&fabs(vrod.x)<EPS&&
				fabs(pRod->pStart->xOriginalPos.y)-fabs(pRod->pEnd->xOriginalPos.y)>EPS)
				pRightHoriAngle=pRod;
		}
		else if (pBaseNode->ciLegQuad==2||pBaseNode->ciLegQuad==3)
		{
			if (pRod->pStart==pVLeftTopNode&&fabs(vrod.x)<EPS&&
				fabs(pRod->pEnd->xOriginalPos.y)-fabs(pRod->pStart->xOriginalPos.y)>EPS)
				pLeftHoriAngle=pRod;
			else if (pRod->pEnd==pVLeftTopNode&&fabs(vrod.x)<EPS&&
				fabs(pRod->pStart->xOriginalPos.y)-fabs(pRod->pEnd->xOriginalPos.y)>EPS)
				pLeftHoriAngle=pRod;
			if (pRod->pStart==pVRightTopNode&&fabs(vrod.y)<EPS&&
				fabs(pRod->pEnd->xOriginalPos.x)-fabs(pRod->pStart->xOriginalPos.x)>EPS)
				pRightHoriAngle=pRod;
			else if (pRod->pEnd==pVRightTopNode&&fabs(vrod.y)<EPS&&
				fabs(pRod->pStart->xOriginalPos.x)-fabs(pRod->pEnd->xOriginalPos.x)>EPS)
				pRightHoriAngle=pRod;
		}
		if (pLeftHoriAngle!=NULL&&pRightHoriAngle!=NULL)
			break;
	}
	if (pLeftHoriAngle==NULL||pRightHoriAngle==NULL)
		return false;
	if (!pLeftHoriAngle->IsAngle()||!pRightHoriAngle->IsAngle())
		return false;
	pPantsLeg->pLeftHoriAngle=(CLDSLineAngle*)pLeftHoriAngle;	//大裤衩结构接腿顶部的中间截断小横材(左)
	pPantsLeg->pRightHoriAngle=(CLDSLineAngle*)pRightHoriAngle;	//大裤衩结构接腿顶部的中间截断小横材(右)
	pPantsLeg->pVLeftTopNode=pVLeftTopNode;		//大裤衩结构接腿八字斜材顶部节点(左)
	pPantsLeg->pVRightTopNode=pVRightTopNode;	//大裤衩结构接腿八字斜材顶部节点(右)
	//填充非必须信息，但一般情况也肯定是有的。
	for (pRod=pTower->EnumRodFirst();pRod;pRod=pTower->EnumRodNext())
	{
		if (pRod->pStart==NULL||pRod->pEnd==NULL||pRod->m_bVirtualPart||!pRod->IsAngle())
			continue;
		if (!pLivePostRod->ModuleTogetherWith(pRod))
			continue;
		GEPOINT vrod=pRod->pStart->xOriginalPos-pRod->pEnd->xOriginalPos;
		if (pRod->pStart==pVLeftTopNode||pRod->pEnd==pVLeftTopNode)
		{
			if (pRod->pStart==pVLeftTopNode)
				vrod*=-1;
			if (fabs(vrod*vLeftPlane)>2)
				continue;	//非左外立面杆件
			if (vrod.z<0&&pRod->pStart==pVLeftTopNode&&(
				fabs(pRod->pEnd->xOriginalPos.x)<EPS||fabs(pRod->pEnd->xOriginalPos.y)<EPS))
				pPantsLeg->pLeftUpLegAngle=(CLDSLineAngle*)pRod;
			if (vrod.z<0&&pRod->pEnd==pVLeftTopNode&&(
				fabs(pRod->pStart->xOriginalPos.x)<EPS||fabs(pRod->pStart->xOriginalPos.y)<EPS))
				pPantsLeg->pLeftUpLegAngle=(CLDSLineAngle*)pRod;
			if (vrod.z>0&&LocalTestRodNodeColinear(pRod,pBaseNode))
				pPantsLeg->pLeftDwLegAngle=(CLDSLineAngle*)pRod;
		}
		if (pRod->pStart==pVRightTopNode||pRod->pEnd==pVRightTopNode)
		{
			if (pRod->pStart==pVRightTopNode)
				vrod*=-1;
			if (fabs(vrod*vRightPlane)>2)
				continue;	//非右外立面杆件
			if (vrod.z<0&&pRod->pStart==pVRightTopNode&&(
				fabs(pRod->pEnd->xOriginalPos.x)<EPS||fabs(pRod->pEnd->xOriginalPos.y)<EPS))
				pPantsLeg->pRightUpLegAngle=(CLDSLineAngle*)pRod;
			if (vrod.z<0&&pRod->pEnd==pVRightTopNode&&(
				fabs(pRod->pStart->xOriginalPos.x)<EPS||fabs(pRod->pStart->xOriginalPos.y)<EPS))
				pPantsLeg->pRightUpLegAngle=(CLDSLineAngle*)pRod;
			if (vrod.z>0&&LocalTestRodNodeColinear(pRod,pBaseNode))
				pPantsLeg->pRightDwLegAngle=(CLDSLineAngle*)pRod;
		}
	}
	return true;
}
bool CModelSubLegNode::CorrectPantsKneeJointPosition()
{
	if (!IdentifyPantsLeg(true))
		return false;	//非大裤衩型腿身复合接腿
	//处理大裤衩复合腿身过渡小横材的端点位置
	if (pants.pLeftHoriAngle->IsAngle())
	{
		CLDSNode* pTopNode=pants.pVLeftTopNode;
		CLDSLineAngle *pHoriAngle=(CLDSLineAngle*)pants.pLeftHoriAngle;
		CLDSLineAngle *pHoriMirYAngle=(CLDSLineAngle*)pHoriAngle->GetMirPart(MIRMSG(2),pTopNode->xOriginalPos);
		CLDSPoint xDatumStart,xDatumEnd;
		CLDSNode* pMirTopNode=pTopNode->GetMirNode(MIRMSG(2));
		if (pHoriMirYAngle)
		{
			xDatumStart.des_para.RODEND.hRod=pHoriMirYAngle->handle;
			xDatumEnd.des_para.RODEND.hRod=pHoriAngle->handle;
			xDatumStart.des_para.RODEND.direction=pHoriMirYAngle->pStart==pMirTopNode?1:0;
			xDatumStart.des_para.RODEND.SetWingOffset();
			xDatumEnd.des_para.RODEND.direction=pHoriAngle->pStart==pTopNode?1:0;
			xDatumEnd.des_para.RODEND.SetWingOffset();
			if (pHoriAngle->pStart==pTopNode)
			{
				xDatumEnd.des_para.RODEND.direction=1;
				pHoriAngle->desStartPos.jgber_cal_style=0;
				pHoriAngle->desStartPos.spatialOperationStyle=16;
				pHoriAngle->desStartPos.wing_x_offset.SetOffset();
				pHoriAngle->desStartPos.wing_y_offset.SetOffset();
				pHoriAngle->desStartPos.datumLineStart.datum_pos_style=pHoriAngle->desStartPos.datumLineEnd.datum_pos_style=1;
				pHoriAngle->desStartPos.datumLineStart.des_para=xDatumStart.des_para;
				pHoriAngle->desStartPos.datumLineEnd.des_para=xDatumEnd.des_para;
			}
			else
			{
				xDatumEnd.des_para.RODEND.direction=0;
				pHoriAngle->desEndPos.jgber_cal_style=0;
				pHoriAngle->desEndPos.spatialOperationStyle=16;
				pHoriAngle->desEndPos.wing_x_offset.SetOffset();
				pHoriAngle->desEndPos.wing_y_offset.SetOffset();
				pHoriAngle->desEndPos.datumLineStart.datum_pos_style=pHoriAngle->desEndPos.datumLineEnd.datum_pos_style=1;
				pHoriAngle->desEndPos.datumLineStart.des_para=xDatumStart.des_para;
				pHoriAngle->desEndPos.datumLineEnd.des_para=xDatumEnd.des_para;
			}
			pHoriAngle->SyncMirProp("Position");
		}
		//更新相关射线角钢的肢朝向
		//1.更新水平产射线分隔横材肢朝向
		GEPOINT vLeftPlaneNormal;
		if (pHoriAngle->pStart==pTopNode)
		{
			pHoriAngle->des_norm_x.hViceJg=pPostRod->handle;
			pHoriAngle->des_norm_x.bSpecific=pHoriAngle->des_norm_x.bByOtherWing=FALSE;
			GEPOINT lenRayVec=pHoriAngle->pEnd->xOriginalPos-pHoriAngle->pStart->xOriginalPos;
			vLeftPlaneNormal.Set(-lenRayVec.y,lenRayVec.x);
			normalize(vLeftPlaneNormal);
			pHoriAngle->des_norm_x.near_norm=vLeftPlaneNormal;
			pHoriAngle->des_norm_y.bSpecific=true;
			pHoriAngle->des_norm_y.spec_norm.vector.Set(0,0,-1);
		}
		else
		{
			pHoriAngle->des_norm_y.hViceJg=pPostRod->handle;
			pHoriAngle->des_norm_y.bSpecific=pHoriAngle->des_norm_y.bByOtherWing=FALSE;
			GEPOINT lenRayVec=pHoriAngle->pStart->xOriginalPos-pHoriAngle->pEnd->xOriginalPos;
			vLeftPlaneNormal.Set(-lenRayVec.y,lenRayVec.x);
			normalize(vLeftPlaneNormal);
			pHoriAngle->des_norm_y.near_norm=vLeftPlaneNormal;
			pHoriAngle->des_norm_x.bSpecific=true;
			pHoriAngle->des_norm_x.spec_norm.vector.Set(0,0,-1);
		}
		pHoriAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
		pHoriAngle->SyncMirProp("wingNormInfo");
		if(pants.pLeftUpLegAngle!=NULL)
		{
			CLDSLineAngle* pAngle=pants.pLeftUpLegAngle;
			if (pAngle->pStart==pTopNode)
			{
				pAngle->des_norm_x.hViceJg=pHoriAngle->handle;
				pAngle->des_norm_x.bSpecific=pAngle->des_norm_x.bByOtherWing=FALSE;
				pAngle->des_norm_x.near_norm=vLeftPlaneNormal;
				pAngle->des_norm_y.bByOtherWing=true;
			}
			else
			{
				pAngle->des_norm_y.hViceJg=pHoriAngle->handle;
				pAngle->des_norm_y.bSpecific=pAngle->des_norm_y.bByOtherWing=FALSE;
				pAngle->des_norm_y.near_norm=vLeftPlaneNormal;
				pAngle->des_norm_x.bByOtherWing=true;
			}
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
			pAngle->SyncMirProp("wingNormInfo");
		}
		if(pants.pLeftDwLegAngle!=NULL)
		{
			CLDSLineAngle* pAngle=pants.pLeftDwLegAngle;
			if (pAngle->pStart==pTopNode)
			{
				pAngle->des_norm_y.hViceJg=pHoriAngle->handle;
				pAngle->des_norm_y.bSpecific=pAngle->des_norm_y.bByOtherWing=FALSE;
				pAngle->des_norm_y.near_norm=vLeftPlaneNormal;
				pAngle->des_norm_x.bByOtherWing=true;
			}
			else
			{
				pAngle->des_norm_x.hViceJg=pHoriAngle->handle;
				pAngle->des_norm_x.bSpecific=pAngle->des_norm_x.bByOtherWing=FALSE;
				pAngle->des_norm_x.near_norm=vLeftPlaneNormal;
				pAngle->des_norm_y.bByOtherWing=true;
			}
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
			pAngle->SyncMirProp("wingNormInfo");
		}
	}
	if (pants.pRightHoriAngle->IsAngle())
	{
		CLDSNode* pTopNode=pants.pVRightTopNode;
		CLDSLineAngle *pHoriAngle=(CLDSLineAngle*)pants.pRightHoriAngle;
		CLDSLineAngle *pHoriMirXAngle=(CLDSLineAngle*)pHoriAngle->GetMirPart(MIRMSG(1),pTopNode->xOriginalPos);
		CLDSPoint xDatumStart,xDatumEnd;
		xDatumStart.datum_pos_style=xDatumEnd.datum_pos_style=1;
		CLDSNode* pMirTopNode=pTopNode->GetMirNode(MIRMSG(1));
		if (pHoriMirXAngle)
		{
			xDatumStart.des_para.RODEND.hRod=pHoriMirXAngle->handle;
			xDatumEnd.des_para.RODEND.hRod=pHoriAngle->handle;
			xDatumStart.des_para.RODEND.direction=pHoriMirXAngle->pStart==pMirTopNode?1:0;
			xDatumStart.des_para.RODEND.SetWingOffset();
			xDatumEnd.des_para.RODEND.direction=pHoriAngle->pStart==pTopNode?1:0;
			xDatumEnd.des_para.RODEND.SetWingOffset();
			if (pHoriAngle->pStart==pTopNode)
			{
				xDatumEnd.des_para.RODEND.direction=1;
				pHoriAngle->desStartPos.jgber_cal_style=0;
				pHoriAngle->desStartPos.spatialOperationStyle=16;
				pHoriAngle->desStartPos.wing_x_offset.SetOffset();
				pHoriAngle->desStartPos.wing_y_offset.SetOffset();
				pHoriAngle->desStartPos.datumLineStart.datum_pos_style=pHoriAngle->desStartPos.datumLineEnd.datum_pos_style=1;
				pHoriAngle->desStartPos.datumLineStart.des_para=xDatumStart.des_para;
				pHoriAngle->desStartPos.datumLineEnd.des_para=xDatumEnd.des_para;
			}
			else
			{
				xDatumEnd.des_para.RODEND.direction=0;
				pHoriAngle->desEndPos.jgber_cal_style=0;
				pHoriAngle->desEndPos.spatialOperationStyle=16;
				pHoriAngle->desEndPos.wing_x_offset.SetOffset();
				pHoriAngle->desEndPos.wing_y_offset.SetOffset();
				pHoriAngle->desEndPos.datumLineStart.datum_pos_style=pHoriAngle->desEndPos.datumLineEnd.datum_pos_style=1;
				pHoriAngle->desEndPos.datumLineStart.des_para=xDatumStart.des_para;
				pHoriAngle->desEndPos.datumLineEnd.des_para=xDatumEnd.des_para;
			}
			pHoriAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
			pHoriAngle->SyncMirProp("Position");
		}
		//更新相关射线角钢的肢朝向
		//1.更新水平产射线分隔横材肢朝向
		GEPOINT vRightPlaneNormal;
		if (pHoriAngle->pStart==pTopNode)
		{
			pHoriAngle->des_norm_y.hViceJg=pPostRod->handle;
			pHoriAngle->des_norm_y.bSpecific=pHoriAngle->des_norm_y.bByOtherWing=FALSE;
			GEPOINT lenRayVec=pHoriAngle->pEnd->xOriginalPos-pHoriAngle->pStart->xOriginalPos;
			vRightPlaneNormal.Set(lenRayVec.y,-lenRayVec.x);
			pHoriAngle->des_norm_y.near_norm=vRightPlaneNormal;
			pHoriAngle->des_norm_x.bSpecific=true;
			pHoriAngle->des_norm_x.spec_norm.vector.Set(0,0,-1);
		}
		else
		{
			pHoriAngle->des_norm_x.hViceJg=pPostRod->handle;
			pHoriAngle->des_norm_x.bSpecific=pHoriAngle->des_norm_x.bByOtherWing=FALSE;
			GEPOINT lenRayVec=pHoriAngle->pStart->xOriginalPos-pHoriAngle->pEnd->xOriginalPos;
			vRightPlaneNormal.Set(lenRayVec.y,-lenRayVec.x);
			pHoriAngle->des_norm_x.near_norm=vRightPlaneNormal;
			pHoriAngle->des_norm_y.bSpecific=true;
			pHoriAngle->des_norm_y.spec_norm.vector.Set(0,0,-1);
		}
		pHoriAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
		pHoriAngle->SyncMirProp("wingNormInfo");
		if(pants.pRightUpLegAngle!=NULL)
		{
			CLDSLineAngle* pAngle=pants.pRightUpLegAngle;
			if (pAngle->pStart==pTopNode)
			{
				pAngle->des_norm_y.hViceJg=pHoriAngle->handle;
				pAngle->des_norm_y.bSpecific=pAngle->des_norm_y.bByOtherWing=FALSE;
				pAngle->des_norm_y.near_norm=vRightPlaneNormal;
				pAngle->des_norm_x.bByOtherWing=true;
			}
			else
			{
				pAngle->des_norm_x.hViceJg=pHoriAngle->handle;
				pAngle->des_norm_x.bSpecific=pAngle->des_norm_x.bByOtherWing=FALSE;
				pAngle->des_norm_x.near_norm=vRightPlaneNormal;
				pAngle->des_norm_y.bByOtherWing=true;
			}
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
			pAngle->SyncMirProp("wingNormInfo");
		}
		if(pants.pRightDwLegAngle!=NULL)
		{
			CLDSLineAngle* pAngle=pants.pRightDwLegAngle;
			if (pAngle->pStart==pTopNode)
			{
				pAngle->des_norm_x.hViceJg=pHoriAngle->handle;
				pAngle->des_norm_x.bSpecific=pAngle->des_norm_x.bByOtherWing=FALSE;
				pAngle->des_norm_x.near_norm=vRightPlaneNormal;
				pAngle->des_norm_y.bByOtherWing=true;
			}
			else
			{
				pAngle->des_norm_y.hViceJg=pHoriAngle->handle;
				pAngle->des_norm_y.bSpecific=pAngle->des_norm_y.bByOtherWing=FALSE;
				pAngle->des_norm_y.near_norm=vRightPlaneNormal;
				pAngle->des_norm_x.bByOtherWing=true;
			}
			pAngle->MirLayStyle=CLDSLineAngle::MIRXorY_SAMEDIRECT;
			pAngle->SyncMirProp("wingNormInfo");
		}
	}
	return true;
}
bool CModelSubLegNode::UpdateFoundationPosition()	//更新接腿的基础根开默认值
{
	if(pPostRod==NULL||syncflag.IsSpcecifyBasePos())
		return false;
	CLDSNode* pBaseNode=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?pPostRod->pEnd:pPostRod->pStart;
	if(!pPostRod->IsAngle()||pPostRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		fHalfFrontBaseWidth=fabs(pBaseNode->xOriginalPos.x);
		fHalfSideBaseWidth =fabs(pBaseNode->xOriginalPos.y);

		if(pTreeModel->pTower->m_xFoundation.biBasePlateType==FOUNDATION_PARA::TYPE_TOP)	//交于底板上平面
			zFoundationTopPlane=pBaseNode->xOriginalPos.z+pTreeModel->pTower->m_xFoundation.wiBasePlateThick;
		else if(pTreeModel->pTower->m_xFoundation.biBasePlateType==FOUNDATION_PARA::TYPE_BTM)	//交于底板下平面
			zFoundationTopPlane=pBaseNode->xOriginalPos.z;
		else //if(pTreeModel->pTower->m_xFoundation.biBasePlateType==FOUNDATION_PARA::TYPE_MID)	//交于底板中间
			zFoundationTopPlane=pBaseNode->xOriginalPos.z+pTreeModel->pTower->m_xFoundation.wiBasePlateThick*0.5;
		return true;
	}
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	GEPOINT vDiagonal=-(pAngle->vxWingNorm+pAngle->vyWingNorm);
	if(vDiagonal.x>0)
		vDiagonal.x*=-1.0;
	if(vDiagonal.y>0)
		vDiagonal.y*=-1.0;
	normalize(vDiagonal);
	double offsetlen=pAngle->size_wide*0.25*1.414;
	f3dLine line(pAngle->xPosStart+vDiagonal*offsetlen,pAngle->xPosEnd+vDiagonal*offsetlen);
	GEPOINT xWeightCenter;
	Int3dlf(xWeightCenter,line,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
	fHalfFrontBaseWidth=fabs(xWeightCenter.x);
	fHalfSideBaseWidth =fabs(xWeightCenter.y);
	if(pTreeModel->pTower->m_xFoundation.biBasePlateType==FOUNDATION_PARA::TYPE_TOP)	//交于底板上平面
		zFoundationTopPlane=pBaseNode->xOriginalPos.z+pTreeModel->pTower->m_xFoundation.wiBasePlateThick;
	else if(pTreeModel->pTower->m_xFoundation.biBasePlateType==FOUNDATION_PARA::TYPE_MID)	//交于底板中间
		zFoundationTopPlane=pBaseNode->xOriginalPos.z+pTreeModel->pTower->m_xFoundation.wiBasePlateThick*0.5;
	else //if(pTreeModel->m_xFoundation.biBasePlateType==FOUNDATION_PARA::TYPE_BTM)	//交于底板下平面
		zFoundationTopPlane=pBaseNode->xOriginalPos.z;
	return true;
}
double CModelSubLegNode::get_fHalfFrontWidth()
{
	if(pPostRod==NULL)
		return 0;
	CLDSNode* pBaseNode=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?pPostRod->pEnd:pPostRod->pStart;
	if(!pPostRod->IsAngle()||pPostRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		return fabs(pBaseNode->xOriginalPos.x);
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	char ciCurrWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'X':'Y';
	GEPOINT vCurrWingVec=ciCurrWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	double g=this->wActualFrontAngleG;
	f3dLine axisline(pPostRod->xPosStart+vCurrWingVec*g,pPostRod->xPosEnd+vCurrWingVec*g);
	GEPOINT inters;
	Int3dlf(inters,axisline,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
	return fabs(inters.x);
}
double CModelSubLegNode::get_fHalfSideWidth()
{
	if(pPostRod==NULL)
		return 0;
	CLDSNode* pBaseNode=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?pPostRod->pEnd:pPostRod->pStart;
	if(!pPostRod->IsAngle()||pPostRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		return fabs(pBaseNode->xOriginalPos.x);
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPostRod;
	char ciCurrWing=pPostRod->xPosEnd.z>pPostRod->xPosStart.z?'Y':'X';
	GEPOINT vCurrWingVec=ciCurrWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	double g=this->wActualSideAngleG;
	f3dLine axisline(pPostRod->xPosStart+vCurrWingVec*g,pPostRod->xPosEnd+vCurrWingVec*g);
	GEPOINT inters;
	Int3dlf(inters,axisline,pBaseNode->xOriginalPos,GEPOINT(0,0,1));
	return fabs(inters.y);
}
CTowerModelTree* CModelSubLegNode::get_pTreeModel(){
	return m_pBelongHeight!=NULL?m_pBelongHeight->pTreeModel:NULL;
}
CLDSLinePart* CModelSubLegNode::get_pPostRod()
{
	CLDSLinePart* pDownRod=NULL;
	double maxz=0;
	for(int i=0;i<4;i++)
	{
		if(pDownRod==NULL&&m_arrPostRods[i]!=NULL)
		{
			pDownRod=m_arrPostRods[i];
			maxz=max(pDownRod->pStart->xOriginalPos.z,pDownRod->pEnd->xOriginalPos.z);
		}
		else if(m_arrPostRods[i]!=NULL)
		{
			double currz=max(m_arrPostRods[i]->pStart->xOriginalPos.z,m_arrPostRods[i]->pEnd->xOriginalPos.z);
			if(currz>maxz)
			{
				pDownRod=m_arrPostRods[i];
				maxz=currz;
			}
		}
	}
	return pDownRod;
}
int CModelSubLegNode::AppendLegPostRod(CLDSLinePart* pRod)
{
	int idleI=-1;
	for(int i=0;i<4;i++)
	{
		if(idleI<0&&m_arrPostRods[i]==NULL)
			idleI=i;
		if(m_arrPostRods[i]==pRod)
			return i;
	}
	if(this->iSeg==0)
		iSeg=pRod->iSeg;
	BYTE start0_end1_other=-1;
	if(idleI==0)
	{
		minz=min(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
		maxz=max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z);
		if(pRod->pStart->xOriginalPos.z>pRod->pEnd->xOriginalPos.z)
			start0_end1_other=0;
		else
			start0_end1_other=1;
	}
	else
	{
		if(pRod->pStart->xOriginalPos.z<minz)
			minz=pRod->pStart->xOriginalPos.z;
		if(pRod->pStart->xOriginalPos.z>maxz)
		{
			maxz=pRod->pStart->xOriginalPos.z;
			start0_end1_other=0;
		}
		if(pRod->pEnd->xOriginalPos.z<minz)
			minz=pRod->pEnd->xOriginalPos.z;
		if(pRod->pEnd->xOriginalPos.z>maxz)
		{
			maxz=pRod->pEnd->xOriginalPos.z;
			start0_end1_other=1;
		}
	}
	if(pRod->IsAngle()&&start0_end1_other>=0)
	{
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pRod;
		if(start0_end1_other==0)
		{
			fHalfFrontBaseWidth=pRod->pStart->xOriginalPos.x;
			fHalfSideBaseWidth=pRod->pStart->xOriginalPos.y;
			f3dPoint xWingPos=pLineAngle->GetDatumPosWingX(pRod->pStart);
			f3dPoint yWingPos=pLineAngle->GetDatumPosWingY(pRod->pStart);
			//foundationPara.datumPos=pRod->pStart->xOriginalPos;
		}
		else
		{
			fHalfFrontBaseWidth=pRod->pEnd->xOriginalPos.x;
			fHalfSideBaseWidth=pRod->pEnd->xOriginalPos.y;
			f3dPoint xWingPos=pLineAngle->GetDatumPosWingX(pRod->pEnd);
			f3dPoint yWingPos=pLineAngle->GetDatumPosWingY(pRod->pEnd);
			//foundationPara.datumPos=pRod->pEnd->xOriginalPos;
		}
	}
	fLegHeight=maxz-minz;
	m_arrPostRods[idleI]=pRod;
	pRod->feature=1;
	return idleI;
}
CModelHeightNode::CModelHeightNode(){
	m_pTreeModel=NULL;
	zLongestLeg=zExclusiveBodyTop=0;
	for(UINT i=0;i<xarrSubLegs.CountOfPreset;i++)
		xarrSubLegs[i].SetBelongHeight(this);
	xarrSubLegs.SetSize(0);	//新版PRESET_ARRAY[i]调用时会自动更新数量 wjh-2019.6.3
}
double CModelHeightNode::get_zTransitHoriPlane()
{
	WORD count=0;
	for(WORD i=0;i<xarrSubLegs.Count;i++)
	{
		if(xarrSubLegs[i].Id!=0)
			return xarrSubLegs[i].minz;
	}
	return 0;
}
double CModelHeightNode::get_fNamedHeight()
{
	double fNamedHeightZeroZ = pTreeModel != NULL ? pTreeModel->m_fNamedHeightZeroZ : 0;
	double fNamedHeight = zLongestLeg - fNamedHeightZeroZ;
	return fNamedHeight;
}
double CModelHeightNode::get_fLinkBodyHeight()
{
	double fNamedHeightZeroZ = pTreeModel != NULL ? pTreeModel->m_fNamedHeightZeroZ : 0;
	double fBodyHeight = zTransitHoriPlane - fNamedHeightZeroZ;
	return fBodyHeight;
}
WORD CModelHeightNode::get_wLegCount(){
	return (WORD)xarrSubLegs.Count;
};
CModelSubLegNode* CModelHeightNode::AppendSubLeg(CModelSubLegNode &subleg)
{
	UINT i;
	for(i=0;i<xarrSubLegs.Count;i++)
	{
		if(xarrSubLegs[i].Id==subleg.Id)
		{
			xarrSubLegs[i]=subleg;
			xarrSubLegs[i].SetBelongHeight(this);
			return &xarrSubLegs[i];
		}
	}
	CModelSubLegNode* pCurrSubLeg=xarrSubLegs.Append(subleg);
	//新增加的子接腿
	pCurrSubLeg->SetKey(subleg.Id);
	pCurrSubLeg->SetBelongHeight(this);
	for(int j=0;j<4;j++)
	{
		if(subleg.m_arrPostRods[j]!=NULL)
			pCurrSubLeg->AppendLegPostRod(subleg.m_arrPostRods[j]);
	}
	return pCurrSubLeg;
};
CModelSegmentNode* CModelHeightNode::AppendSegmentRod(CLDSLinePart* pRod)
{
	CModelSegmentNode* pSegment=this->m_pTreeModel->AppendSegmentRod(pRod);
	pSegment->SetBelongHeight(this);
	pRod->feature=1;
	return hashSegments.SetValue(pRod->handle,pSegment);
}
CModelSlopeNode::CModelSlopeNode(){
	pMainHeight=NULL;
	fTopZ=0;
	fFrontWidth=fSideWidth=0;
	fBtmZ=0;
	fBtmFrontWidth=fBtmSideWidth=0;
}
CModelSegmentNode* CModelSlopeNode::AppendSegmentRod(CLDSLinePart* pRod)
{
	pRod->feature=1;
	//顶部开口|Z值
	bool firstseg=(hashSegments.Count==0);
	if(firstseg||pRod->pStart->xOriginalPos.z<fTopZ)
	{
		fTopZ=pRod->pStart->xOriginalPos.z;
		fFrontWidth=fabs(pRod->pStart->xOriginalPos.x)*2;
		fSideWidth =fabs(pRod->pStart->xOriginalPos.y)*2;
	}
	else if(pRod->pEnd->xOriginalPos.z<fTopZ)
	{
		fTopZ=pRod->pEnd->xOriginalPos.z;
		fFrontWidth=fabs(pRod->pEnd->xOriginalPos.x)*2;
		fSideWidth =fabs(pRod->pEnd->xOriginalPos.y)*2;
	}
	//底部开口|Z值
	if (pRod->pStart->xOriginalPos.z > fBtmZ)
	{
		fBtmZ = pRod->pStart->xOriginalPos.z;
		fBtmFrontWidth = fabs(pRod->pStart->xOriginalPos.x) * 2;
		fBtmSideWidth = fabs(pRod->pStart->xOriginalPos.y) * 2;
	}
	else if (pRod->pEnd->xOriginalPos.z > fBtmZ)
	{
		fBtmZ = pRod->pEnd->xOriginalPos.z;
		fBtmFrontWidth = fabs(pRod->pEnd->xOriginalPos.x) * 2;
		fBtmSideWidth = fabs(pRod->pEnd->xOriginalPos.y) * 2;
	}
	CModelSegmentNode* pSegment=m_pTreeModel->AppendSegmentRod(pRod);
	this->hashSegments.SetValue(pSegment->Id,pSegment);
	return pSegment;
}
void CModelSlopeNode::SortSegments()
{
}
double CModelSlopeNode::get_fSlopeRatio()
{
	double fDiffZ = fBtmZ - fTopZ;
	double fDiffX = (fBtmFrontWidth - fFrontWidth) * 0.5;
	double fSlopeRatio = (fDiffZ > 0) ? (fDiffX / fDiffZ) : 0;
	return fSlopeRatio;
}
CTowerModelTree::SORTSEG::SORTSEG(CLDSLinePart* pRod/*=NULL*/)
{
	InitByRod(pRod);
	pRelaTreeNode=NULL;
}
void CTowerModelTree::SORTSEG::InitByRod(CLDSLinePart* pRod)
{
	this->pRelaRod=pRod;
	if(pRod)
	{
		minz=ftoi(min(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z));
		maxz=ftoi(max(pRod->pStart->xOriginalPos.z,pRod->pEnd->xOriginalPos.z));
	}
}
int CTowerModelTree::SORTSEG::Compare(const ICompareClass *pCompareObj)const
{
	SORTSEG* pSortSeg2=(SORTSEG*)pCompareObj;
	if(this->minz<pSortSeg2->minz)
		return -1;
	else if(this->minz>pSortSeg2->minz)
		return 1;
	else if(maxz<pSortSeg2->maxz)
		return -1;
	else if(this->maxz>pSortSeg2->maxz)
		return 1;
	else
		return 0;
}
CTowerModelTree gxModelTree;
CTowerModelTree::CTowerModelTree(CTower* pTower)
{
	m_pTower=pTower;
	m_fNamedHeightZeroZ=0;
}
bool CTowerModelTree::AddOrUpdateFoundations()
{	//更新或生成铁塔基础
	if (m_pTower == NULL)
		return false;
	for (CModelHeightNode *pHeight = hashHeights.GetFirst(); pHeight; pHeight = hashHeights.GetNext())
	{
		for (int i = 0; i < 24; i++)
		{
			if (pHeight->xarrSubLegs[i].Id <= 0)
				continue;
			for (int iLegQuad = 1; iLegQuad <= 4; iLegQuad++)
			{
				BYTE ciHeightSerial = pHeight->Id;
				BYTE ciLegSerial = pHeight->xarrSubLegs[i].Id;
				BYTE ciLegQuad = iLegQuad;
				//KEY4C k(ciHeightSerial,ciLegSerial,ciLegQuad);
				CLDSFoundation * pFoundation = m_pTower->AddFoundation(ciHeightSerial, ciLegSerial, ciLegQuad);
				pHeight->xarrSubLegs[i].m_hRelaFoundation = pFoundation->handle;
				pHeight->xarrSubLegs[i].UpdateFoundationPosition();	//更新接腿的基础根开默认值
				pFoundation->location.Set(pHeight->xarrSubLegs[i].fHalfFrontBaseWidth, pHeight->xarrSubLegs[i].fHalfSideBaseWidth, pHeight->xarrSubLegs[i].zFoundationTopPlane);
				if (iLegQuad == 2 || iLegQuad == 4)
					pFoundation->location.x *= -1.0;
				if (iLegQuad == 3 || iLegQuad == 4)
					pFoundation->location.y *= -1.0;
				pFoundation->ciAnchorBoltD = (BYTE)m_pTower->m_xFoundation.wiBoltD;
				pFoundation->LayoutAnchorBolts(m_pTower->m_xFoundation.biLayoutType, m_pTower->m_xFoundation.wiBoltSapce);
				pFoundation->AddOrUpdateAnchorBolts(m_pTower->m_xFoundation.wiBasePlateThick);
				pFoundation->SetModified();
				pFoundation->wiWidth = m_pTower->m_xFoundation.wiFoundationWidth;
				//LiveDisplayActiveFoundation(pFoundation);
			}
		}
	}
	return true;
}
GEPOINT CTowerModelTree::GetFoundationPosition(CModelSubLegNode* pSubLeg,CModelHeightNode* pBlongHeight/*=NULL*/)
{
	//GEPOINT location;
	return GEPOINT(pSubLeg->fBaseSizeX*0.5,pSubLeg->fBaseSizeY*0.5,pSubLeg->zFoundationTopPlane);
}
CModelSlopeNode* CTowerModelTree::AppendSlopeNode()
{
	CModelSlopeNode* pSlopeNode=this->listSlopes.Append();
	pSlopeNode->SetTreeModel(this);
	return pSlopeNode;
}
CModelHeightNode* CTowerModelTree::AppendHeightNode(UINT idHeightNo)
{
	CModelHeightNode* pHeightNode=this->hashHeights.Add(idHeightNo);
	pHeightNode->SetTreeModel(this);
	return pHeightNode;
}
CModelSegmentNode* CTowerModelTree::AppendSegmentRod(CLDSLinePart* pRod)
{
	CModelSegmentNode* pSegment=hashSegments.Append(pRod->handle);
	pSegment->SetTreeModel(this);
	pSegment->pPostRod=pRod;
	pSegment->iSeg=pRod->iSeg;
	SmartRodPtr pSmartRod=pRod;
	if(!pSmartRod->IsAngle()||pSmartRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pSegment->wFrontAngleG=pSegment->wSideAngleG=0;
	else if(pSmartRod->xPosEnd.z>pSmartRod->xPosStart.z)
	{
		pSegment->wFrontAngleG=pSmartRod.pAngle->GetWingXG(0);
		pSegment->wSideAngleG =pSmartRod.pAngle->GetWingYG(0);
	}
	else
	{
		pSegment->wFrontAngleG=pSmartRod.pAngle->GetWingYG(0);
		pSegment->wSideAngleG =pSmartRod.pAngle->GetWingXG(0);
	}
	return pSegment;
}
long IncrementValidBodySegNumber(long liCurrBodySegNumber,int* xarrExcludeSegNumber=NULL,UINT uiExcludeNumberCount=0)
{
	if(xarrExcludeSegNumber==NULL||uiExcludeNumberCount==0)
		return liCurrBodySegNumber+1;
	liCurrBodySegNumber+=1;
	bool blHasRepeat=true;
	do
	{
		blHasRepeat=false;
		for(UINT i=0;i<uiExcludeNumberCount;i++)
		{
			if(liCurrBodySegNumber==xarrExcludeSegNumber[i])
			{
				liCurrBodySegNumber++;
				blHasRepeat=true;
				break;
			}
		}
	}while(blHasRepeat);
	return liCurrBodySegNumber;
}
int _LocalCompareSegmentCoordZ(const CModelSegmentNodePtr& seg1,const CModelSegmentNodePtr& seg2)
{
	double dfSegPosZ1=min(seg1->pPostRod->pStart->xOriginalPos.z,seg1->pPostRod->pEnd->xOriginalPos.z);
	double dfSegPosZ2=min(seg2->pPostRod->pStart->xOriginalPos.z,seg2->pPostRod->pEnd->xOriginalPos.z);
	if(fabs(dfSegPosZ1-dfSegPosZ2)<0.1)
		return 0;
	else if(dfSegPosZ1>dfSegPosZ2)
		return 1;
	else //if(dfSegPosZ1<dfSegPosZ2)
		return -1;
}
long CTowerModelTree::AllocBodySegmentNumber(int iInitBodySegNumber,int* xarrExcludeSegNumber/*=NULL*/,UINT uiExcludeNumberCount/*=0*/)
{	//自动分配塔身及接腿的分段号
	//自动塔材分段规则（由西南院李美峰提供）：
	//1)自上至下，先横担后塔身;
	//2)塔身先依自上至下顺序连续编号至最高呼高，然后按低呼高->高呼高顺序编其余身段;
	//3)所有塔身段编排完毕后，再从最低呼高的短腿－＞长腿编号，然后再根据低－＞高呼高依此顺序编其它接腿
	//1.获取最高的呼高,并遍历所有塔身段进行段号编排
	CLsRef* pLsRef;
	CLDSNode* pNode;
	CLDSLinePart* pRod;
	CModelSegmentNode* pSegment;
	CModelHeightNode* pHeightNode,*pMasterHeight=hashHeights.GetFirst();
	CModelSubLegNode* pSubLeg,*pShortestLeg=NULL;
	CModelSubLegNode* pHighestLeg=&pMasterHeight->xarrSubLegs[0];
	UINT i,count=pMasterHeight->xarrSubLegs.Count;
	//由于hashSegments并非完全按自上至下顺序存储，故需重新排序
	ARRAY_LIST<CModelSegmentNodePtr> xarrSegments(0,hashSegments.GetNodeNum());
	for(pSegment=hashSegments.GetFirst();pSegment!=NULL;pSegment=hashSegments.GetNext())
	{
		xarrSegments.append(pSegment);
		pSegment->pPostRod->iSeg=pSegment->iSeg=SEGI(0L);	//清零塔身分段号
	}
	for(pHeightNode=hashHeights.GetFirst();pHeightNode!=NULL;pHeightNode=hashHeights.GetNext())
	{
		for(i=0;i<pHeightNode->xarrSubLegs.Count;i++)
		{	//清零各呼高接腿分段号
			CModelSubLegNode* pSubLeg=&pHeightNode->xarrSubLegs[i];
			if(pSubLeg->m_arrPostRods[0]!=NULL)
				pSubLeg->m_arrPostRods[0]->iSeg=pSubLeg->iSeg=SEGI(0L);
		}
	}
	CQuickSort<CModelSegmentNodePtr>::QuickSort(xarrSegments.m_pData,xarrSegments.GetSize(),_LocalCompareSegmentCoordZ);
	//1.1查找最高呼高的最低接腿
	for(i=0;i<pMasterHeight->xarrSubLegs.Count;i++)
	{
		pShortestLeg=&pMasterHeight->xarrSubLegs[count-i-1];
		if(pShortestLeg->Id>0)
			break;
	}
	if(pShortestLeg==NULL)
	{
		logerr.Log("error");
		return 0;
	}
	long liCurrBodySegNumber=iInitBodySegNumber;
	//1.2从最高呼高开始自上至下依次对塔身分段进行分段编号
	for(i=0;i<(UINT)xarrSegments.GetSize();i++)
	{
		pSegment=xarrSegments[i];
		if(!pSegment->pPostRod->ModuleTogetherWith(pShortestLeg->m_arrPostRods[0]))
			continue;	//与当前呼高不共存，说明当前塔身分段不属于当前呼高
		pSegment->iSeg=liCurrBodySegNumber;
		liCurrBodySegNumber=IncrementValidBodySegNumber(liCurrBodySegNumber,xarrExcludeSegNumber,uiExcludeNumberCount);
	}
	//1.3低呼高->高呼高顺序编其余身段(hashHeights按高->低顺序存储)
	for(pHeightNode=hashHeights.GetTail();pHeightNode!=NULL;pHeightNode=hashHeights.GetPrev())
	{
		if(pHeightNode==pMasterHeight)
			continue;
		pSubLeg=&pHeightNode->xarrSubLegs[0];
		for(i=0;i<(UINT)xarrSegments.GetSize();i++)
		{
			pSegment=xarrSegments[i];
			if(pSegment->iSeg!=0)
				continue;	//已编过段号
			if(!pSegment->pPostRod->ModuleTogetherWith(pSubLeg->m_arrPostRods[0]))
				continue;	//与当前呼高不共存，说明当前塔身分段不属于当前呼高
			pSegment->iSeg=liCurrBodySegNumber;
			liCurrBodySegNumber=IncrementValidBodySegNumber(liCurrBodySegNumber,xarrExcludeSegNumber,uiExcludeNumberCount);
		}
	}
	//2.从最低呼高的短腿－＞长腿编号，然后再根据低－＞高呼高依此顺序编其它接腿
	for(pHeightNode=hashHeights.GetTail();pHeightNode!=NULL;pHeightNode=hashHeights.GetPrev())
	{
		count=pHeightNode->xarrSubLegs.Count;
		for(i=0;i<pHeightNode->xarrSubLegs.Count;i++)
		{	//由低->高的顺序依次对接腿进行分段编号
			pSubLeg=&pHeightNode->xarrSubLegs[i];
			pSubLeg->iSeg=liCurrBodySegNumber;
			liCurrBodySegNumber=IncrementValidBodySegNumber(liCurrBodySegNumber,xarrExcludeSegNumber,uiExcludeNumberCount);
		}
	}
	//3.根据之前的段号分配方案，对全塔构件进行段号赋值
	for(pSegment=this->hashSegments.GetFirst();pSegment;pSegment=hashSegments.GetNext())
	{
		TAPER_SCOPE taper;
		taper.VerifyVertex(pSegment->pPostRod->pStart->xOriginalPos);
		taper.VerifyVertex(pSegment->pPostRod->pEnd->xOriginalPos);
		for(pNode=m_pTower->EnumNodeFirst();pNode;pNode=m_pTower->EnumNodeNext())
		{
			if(!pSegment->pPostRod->ModuleTogetherWith(pNode))
				continue;
			if(!taper.IsIncludePoint(pNode->xOriginalPos,0.001))
				continue;
			pNode->iSeg=pSegment->iSeg;
		}
		for(pRod=m_pTower->EnumRodFirst();pRod;pRod=m_pTower->EnumRodNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			if(!pSegment->pPostRod->ModuleTogetherWith(pRod))
				continue;
			if( !taper.IsIncludePoint(pRod->pStart->xOriginalPos,0.001)||
				!taper.IsIncludePoint(pRod->pEnd->xOriginalPos,0.001))
				continue;
			pRod->iSeg=pSegment->iSeg;
		}
	}
	for(pHeightNode=hashHeights.GetFirst();pHeightNode!=NULL;pHeightNode=hashHeights.GetNext())
	{
		count=pHeightNode->xarrSubLegs.Count;
		for(i=0;i<pHeightNode->xarrSubLegs.Count;i++)
		{
			pSubLeg=&pHeightNode->xarrSubLegs[i];
			CLDSLinePart* pPickRod=pSubLeg->m_arrPostRods[0];
			TAPER_SCOPE taper;
			taper.VerifyVertex(pPickRod->pStart->xOriginalPos);
			taper.VerifyVertex(pPickRod->pEnd->xOriginalPos);
			for(pNode=m_pTower->EnumNodeFirst();pNode;pNode=m_pTower->EnumNodeNext())
			{
				if(!pPickRod->ModuleTogetherWith(pNode))
					continue;
				if(!taper.IsIncludePoint(pNode->xOriginalPos,0.001))
					continue;
				pNode->iSeg=pSubLeg->iSeg;
			}
			for(pRod=m_pTower->EnumRodFirst();pRod;pRod=m_pTower->EnumRodNext())
			{
				if(pRod->pStart==NULL||pRod->pEnd==NULL)
					continue;
				if(!pPickRod->ModuleTogetherWith(pRod))
					continue;
				if( !taper.IsIncludePoint(pRod->pStart->xOriginalPos,0.001)||
					!taper.IsIncludePoint(pRod->pEnd->xOriginalPos,0.001))
					continue;
				pRod->iSeg=pSubLeg->iSeg;
			}
		}
	}
	for(pRod=m_pTower->EnumRodFirst();pRod;pRod=m_pTower->EnumRodNext())
	{
		for(pLsRef=pRod->GetFirstLsRef();pLsRef;pLsRef=pRod->GetNextLsRef())
		{
			if(pLsRef->pBolt->des_base_pos.hPart==pRod->handle)
				pLsRef->pBolt->iSeg=pRod->iSeg;
		}
	}
	return liCurrBodySegNumber;
}
void CTowerModelTree::Empty()
{
	listSlopes.Empty();
	hashHeights.Empty();
	hashSegments.Empty();
}

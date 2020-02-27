#include "stdafx.h"
#include "KeypointLifeObj.h"
#include "LDSView.h"

//////////////////////////////////////////////////////////////////////////
//CAngleDisplayLifeObject
CAngleDisplayLifeObject::CAngleDisplayLifeObject()
{
	m_cOldDisplayMode=0;
	m_pAngle=NULL;
}
CAngleDisplayLifeObject::~CAngleDisplayLifeObject()
{
	RestoreDisplayMode();
}
bool CAngleDisplayLifeObject::AttachDisplayState(CLDSPart* pPart,BYTE cbMode)
{
	if(!pPart->IsAngle())
		return false;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pPart;
	if(m_pAngle)
		RestoreDisplayMode();
	m_pAngle=pAngle;
	m_cOldDisplayMode=cbMode;
	return true;
}
void CAngleDisplayLifeObject::DetachDisplayState()
{
	m_pAngle=NULL;
}
void CAngleDisplayLifeObject::RestoreDisplayMode()
{
	if(m_pAngle==NULL)
		return;
	CLDSView::RedrawAngleFrameDrawing(m_pAngle,this->m_cOldDisplayMode,NULL,true);
	g_pSolidDraw->Draw();
}
//////////////////////////////////////////////////////////////////////////
//CPlateDisplayLifeObject
CPlateDisplayLifeObject::CPlateDisplayLifeObject(CLDSGeneralPlate* pPlate,char cCurrDisplayMode/*=-1*/)
{
	if((m_pPlate=pPlate)==NULL)
		return;
	m_cOldDisplayMode=pPlate->m_cPickedDisplayMode;
	if(cCurrDisplayMode>=0)
	{
		pPlate->m_cPickedDisplayMode=cCurrDisplayMode;
		CLDSView::RedrawPlateProfileDrawing(pPlate,NULL,false,0x03);
		g_pSolidDraw->Draw();
	}
}
CPlateDisplayLifeObject::~CPlateDisplayLifeObject()
{
	RestoreDisplayMode();
}
bool CPlateDisplayLifeObject::AttachDisplayState(CLDSPart* pPart,BYTE cbMode)
{
	if(pPart==NULL||!pPart->IsPlate())
		return false;
	m_pPlate=(CLDSGeneralPlate*)pPart;
	m_cOldDisplayMode=m_pPlate->m_cPickedDisplayMode;
	if(cbMode>=0)
	{
		m_pPlate->m_cPickedDisplayMode=cbMode;
		CLDSView::RedrawPlateProfileDrawing(m_pPlate,NULL,false,0x03);
		g_pSolidDraw->Draw();
	}
	return true;
}
void CPlateDisplayLifeObject::DetachDisplayState()
{
	m_pPlate=NULL;
}
void CPlateDisplayLifeObject::Redraw(char cDisplayMode,bool removeOldEntities/*=true*/,BYTE cbMode/*=0*/)
{
	m_pPlate->m_cPickedDisplayMode=cDisplayMode;
	CLDSView::RedrawPlateProfileDrawing(m_pPlate,NULL,removeOldEntities,cbMode);
	g_pSolidDraw->Draw();
}
void CPlateDisplayLifeObject::RestoreDisplayMode()
{
	if(m_pPlate==NULL||m_cOldDisplayMode<0)
		return;
	m_pPlate->m_cPickedDisplayMode=m_cOldDisplayMode;
	CLDSView::RedrawPlateProfileDrawing(m_pPlate,NULL,true);
	m_cOldDisplayMode=-1;
	g_pSolidDraw->Draw();
}

//////////////////////////////////////////////////////
// 实体下显示节点类CDisplayNodeAtFrontLife
CDisplayNodeAtFrontLife::CDisplayNodeAtFrontLife()
{
	m_bOriginalEnabled=g_pSolidSet->LineModelDepthTestState();
};
CDisplayNodeAtFrontLife::~CDisplayNodeAtFrontLife()
{
	if(m_bOriginalEnabled!=g_pSolidSet->LineModelDepthTestState())
		m_bOriginalEnabled?g_pSolidSet->EnableLineModelDepthTest():g_pSolidSet->DisableLineModelDepthTest();
}
BOOL CDisplayNodeAtFrontLife::DisplayNodeAtFront()
{
	DISPLAY_TYPE mode;
	g_pSolidSet->GetDisplayType(&mode);
	if(g_pSolidSet->LineModelDepthTestState()&&mode==DISP_SOLID)
	{
		g_pSolidSet->DisableLineModelDepthTest();
		g_pSolidDraw->Draw();
		return TRUE;
	}
	return FALSE;
}
BOOL CDisplayNodeAtFrontLife::HideNodeAtFront()
{
	if(!g_pSolidSet->LineModelDepthTestState())
	{
		g_pSolidSet->EnableLineModelDepthTest();
		g_pSolidDraw->Draw();
		return TRUE;
	}
	return FALSE;
};
//////////////////////////////////////////////////////////////////////////
//CWorkPlaneSketchLifeObject
CWorkPlaneSketchLifeObject::CWorkPlaneSketchLifeObject()
{
	nWorkPlaneSketch=0;
}
CWorkPlaneSketchLifeObject::~CWorkPlaneSketchLifeObject()
{
	Clear();
	g_pSolidDraw->Draw();
}
void CWorkPlaneSketchLifeObject::Clear()
{
	for(int i=1;i<=nWorkPlaneSketch;i++)
		g_pSolidDraw->DelWorkPlaneSketch(i);
	nWorkPlaneSketch=0;
}
void CWorkPlaneSketchLifeObject::WorkPlaneSketch(GEPOINT base_pos,GEPOINT norm,const char* sName,UINT uiArrowScrLen/*=200*/)
{
	nWorkPlaneSketch++;
	g_pSolidDraw->NewWorkPlaneSketch(nWorkPlaneSketch,RGB(152,152,152),&base_pos,1,norm,sName,uiArrowScrLen);
	g_pSolidDraw->Draw();
}
void CWorkPlaneSketchLifeObject::WorkPlaneSketch(btc::SKETCH_PLANE &sketch,GEPOINT norm,const char* sName,UINT uiArrowScrLen/*=200*/)
{
	nWorkPlaneSketch++;
	g_pSolidDraw->NewWorkPlaneSketch(nWorkPlaneSketch,RGB(152,152,152),sketch.pVertexArr,sketch.VertexCount,norm,sName,uiArrowScrLen);
	g_pSolidDraw->Draw();
}
//////////////////////////////////////////////////////////////////////////
//CModifiedPartsOperObject
bool CModifiedPartsOperObject::OBJECT_INSTANCE_RUNNING=false;
CModifiedPartsOperObject::CModifiedPartsOperObject(bool bClearRecordBuff/*=true*/,bool bTemporyDisable/*=false*/)
{
	if(OBJECT_INSTANCE_RUNNING)
		m_bObjectOvered=true;
	else
	{
		m_bObjectOvered=false;
		m_cbOldAngleMovePartsUpdateFlag=CLDSLineAngle::GetMovePartsUpdateFlag();
		m_cbAngleMovePartsUpdateFlag =bClearRecordBuff?0x01:0;
		m_cbAngleMovePartsUpdateFlag |=bTemporyDisable?0x02:0;
		CLDSLineAngle::SetMovePartsUpdateFlag(m_cbAngleMovePartsUpdateFlag);
		CLDSLineAngle::ClearModifiedPartsBuff();
	}
}
CModifiedPartsOperObject::~CModifiedPartsOperObject(){Terminate();}
bool CModifiedPartsOperObject::Terminate(bool bRedrawRelaPartsSolid/*=true*/)
{
#ifdef __ANGLE_PART_INC_
	if(!m_bObjectOvered)
	{
		CLDSLinePart::SetMovePartsUpdateFlag(m_cbOldAngleMovePartsUpdateFlag);
		OBJECT_INSTANCE_RUNNING=false;
	}
	//AgentDogType()==3无锁授权,暂试开放根据角钢随动螺栓及板的功能 wjh-2017.11.30
	//if(!CLDSPart::IsLaboratoryFuncEnabled())
	//	return false;
	if(!m_bObjectOvered)
	{	//重绘杆件(角钢或钢管)实体及因最近角钢移位导致的移位螺栓及连板的实体
		for(CLDSLineAngle* pAngle=CLDSLineAngle::EnumFirstModifiedAngles();pAngle;pAngle=CLDSLineAngle::EnumNextModifiedAngles())
		{
			if(!pAngle->is_visible)
				continue;	//只更新当前处于显示状态的构件
			pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole);
			g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
		}
		for(CLDSLineTube* pTube=CLDSLineTube::EnumFirstModifiedTube();pTube;pTube=CLDSLineTube::EnumNextModifiedTube())
		{
			if(!pTube->is_visible)
				continue;	//只更新当前处于显示状态的构件
			pTube->Create3dSolidModel(g_sysPara.bDisplayAllHole);
			g_pSolidDraw->NewSolidPart(pTube->GetSolidPartObject());
		}
		//for(int i=0;i<2;i++)
		{	
			for(CLDSBolt* pBolt=CLDSLinePart::EnumFirstRelaMoveBolt();pBolt;pBolt=CLDSLinePart::EnumNextRelaMoveBolt())
			{
				if(!pBolt->is_visible)
					continue;	//只更新当前处于显示状态的构件
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
			//重绘连板信息
			for(CLDSGeneralPlate* pPlate=CLDSLinePart::EnumFirstRelaMovePlate();pPlate;pPlate=CLDSLinePart::EnumNextRelaMovePlate())
			{
				if(!pPlate->is_visible)
					continue;	//只更新当前处于显示状态的构件
				pPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
		}
	}
#endif
	return m_bObjectOvered=true;
}
//////////////////////////////////////////////////////////////////////////
//CAngleMoveOperObject
bool CAngleMoveOperObject::OBJECT_INSTANCE_RUNNING=false;
CAngleMoveOperObject::CAngleMoveOperObject(CLDSLinePart* pAngleOrTube/*=NULL*/,bool bClearRecordBuff/*=true*/,bool bTemporyDisable/*=false*/)
{
	if(OBJECT_INSTANCE_RUNNING)
		m_bObjectOvered=true;
	else
	{
		m_pOperRod=pAngleOrTube;
		m_bObjectOvered=false;
		m_cbOldAngleMovePartsUpdateFlag=CLDSLineAngle::GetMovePartsUpdateFlag();
		m_cbAngleMovePartsUpdateFlag =bClearRecordBuff?0x01:0;
		m_cbAngleMovePartsUpdateFlag |=bTemporyDisable?0x02:0;
		CLDSLineAngle::SetMovePartsUpdateFlag(m_cbAngleMovePartsUpdateFlag);
		CLDSLineAngle::ClearUpdateMovePartsBuff();
	}
}
CAngleMoveOperObject::~CAngleMoveOperObject(){Terminate();}
bool CAngleMoveOperObject::Enable(bool bEnableAgain/*=true*/)
{
	if(bEnableAgain&&m_bObjectOvered)
	{
		m_bObjectOvered=false;
		CLDSLineAngle::SetMovePartsUpdateFlag(m_cbAngleMovePartsUpdateFlag);
		CLDSLineAngle::ClearUpdateMovePartsBuff();
	}
	if(!bEnableAgain)
		m_bObjectOvered=true;
	return bEnableAgain;
}
bool CAngleMoveOperObject::Terminate(bool bRedrawRelaPartsSolid/*=true*/,CLDSLinePart* pAngleOrTube/*=NULL*/)
{
#ifdef __ANGLE_PART_INC_
	if(!m_bObjectOvered)
	{
		CLDSLinePart::SetMovePartsUpdateFlag(m_cbOldAngleMovePartsUpdateFlag);
		OBJECT_INSTANCE_RUNNING=false;
	}
	CLDSLinePart* pRedrawedRod=NULL;
	if(pAngleOrTube==NULL&&m_pOperRod)
		pAngleOrTube=m_pOperRod;
	if(pAngleOrTube&&bRedrawRelaPartsSolid)
	{
		pRedrawedRod=pAngleOrTube;
		pAngleOrTube->Create3dSolidModel(g_sysPara.bDisplayAllHole);
		g_pSolidDraw->NewSolidPart(pAngleOrTube->GetSolidPartObject());
	}
	//AgentDogType()==3无锁授权,暂试开放根据角钢随动螺栓及板的功能 wjh-2017.11.30
	//if(!CLDSPart::IsLaboratoryFuncEnabled())
	//	return false;
	if(!m_bObjectOvered)
	{	//重绘杆件(角钢或钢管)实体及因最近角钢移位导致的移位螺栓及连板的实体
		for(CLDSLineAngle* pAngle=CLDSLineAngle::EnumFirstModifiedAngles();pAngle;pAngle=CLDSLineAngle::EnumNextModifiedAngles())
		{
			if(pAngle==pRedrawedRod)
				continue;	//避免重复刷新实体
			pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole);
			g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
		}
		for(CLDSLineTube* pTube=CLDSLineTube::EnumFirstModifiedTube();pTube;pTube=CLDSLineTube::EnumNextModifiedTube())
		{
			if(pTube==pRedrawedRod)
				continue;	//避免重复刷新实体
			pTube->Create3dSolidModel(g_sysPara.bDisplayAllHole);
			g_pSolidDraw->NewSolidPart(pTube->GetSolidPartObject());
		}
		for(int i=0;i<2;i++)
		{	
			for(CLDSBolt* pBolt=CLDSLinePart::EnumFirstRelaMoveBolt();pBolt;pBolt=CLDSLinePart::EnumNextRelaMoveBolt())
			{
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				pBolt->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
			}
			//重绘连板信息
			for(CLDSGeneralPlate* pPlate=CLDSLinePart::EnumFirstRelaMovePlate();pPlate;pPlate=CLDSLinePart::EnumNextRelaMovePlate())
			{
				if(pPlate->GetClassTypeId()==CLS_PLATE)
					((CLDSPlate*)pPlate)->DesignPlate();
				else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				{
					CLDSParamPlate* pParamPlate=(CLDSParamPlate*)pPlate;
					if(pParamPlate->m_iParamType==TYPE_FL||pParamPlate->m_iParamType==TYPE_FLD
						||pParamPlate->m_iParamType==TYPE_FLP||pParamPlate->m_iParamType==TYPE_ROLLEND
						||pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_XEND)
					{	
						CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(pParamPlate->m_hPartWeldParent,CLS_LINETUBE);
						if(pLineTube)
						{	//重新设计参数化板
							if(pLineTube->m_tJointEnd.hLinkObj==pParamPlate->handle)
								pLineTube->DesignEndJointUCS(pParamPlate);
							else if(pLineTube->m_tJointStart.hLinkObj==pParamPlate->handle)
								pLineTube->DesignStartJointUCS(pParamPlate);
						}
					}
					pParamPlate->DesignPlate();
				}
				pPlate->Create3dSolidModel();
				g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
			}
		}
	}
#endif
	return m_bObjectOvered=true;
}
//////////////////////////////////////////////////////////////////////////
//CNodeMoveOperObject
bool CNodeMoveOperObject::OBJECT_INSTANCE_RUNNING=false;
CNodeMoveOperObject::CNodeMoveOperObject(bool bClearRecordBuff/*=true*/,bool bTemporyDisable/*=false*/)
{
	if(OBJECT_INSTANCE_RUNNING)
		m_bObjectOvered=true;
	else
	{
		CLDSNode::INTERNAL_TEST_VERSION=(PRODUCT_FUNC::IsHasInternalTest()||AgentDogType()==3);
		OBJECT_INSTANCE_RUNNING=true;	
		m_bObjectOvered=false;
		m_cbOldNodeMovePartsUpdateFlag=CLDSNode::GetMovePartsUpdateFlag();
		m_cbNodeMovePartsUpdateFlag =bClearRecordBuff?0x01:0;
		m_cbNodeMovePartsUpdateFlag |=bTemporyDisable?0x02:0;
		CLDSNode::SetMovePartsUpdateFlag(m_cbNodeMovePartsUpdateFlag);
		CLDSNode::ClearUpdateMovePartsBuff();
		Ta.InitializeNodeRelations();
	}
}
CNodeMoveOperObject::~CNodeMoveOperObject(){Terminate();}

//struct NODEMOVECONTEXT{};
void _LocalTaskNodeMoveVisitFunc(NODERELAINFO* pRelaNode,void* pContext)
{
	GEPOINT xOldPosition=pRelaNode->pNode->xActivePos;
	pRelaNode->pNode->Set();
	GEPOINT xMoveOffset=pRelaNode->pNode->xActivePos-xOldPosition;
	if(xOldPosition.IsEqual(xMoveOffset,0.1))
		return;
	g_pSolidDraw->ModifyDbPoint(pRelaNode->pNode->handle,pRelaNode->pNode->xActivePos);
	UINT i;
	for(i=0;i<pRelaNode->relationRods.Count;i++)
	{
		CLDSLinePart* pRod=pRelaNode->relationRods.At(i);
		pRod->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pRod->GetSolidPartObject());
	}
	for(i=0;i<pRelaNode->relationNodes.Count;i++)
	{
		NODERELAINFO* pLeafRela=pRelaNode->relationNodes.At(i);
		if(pLeafRela->uiVisits==0)
			pLeafRela->Visit(_LocalTaskNodeMoveVisitFunc,pContext);
	}
}
bool CNodeMoveOperObject::Enable(bool bEnableAgain/*=true*/)
{
	if(bEnableAgain&&m_bObjectOvered)
	{
		m_bObjectOvered=false;
		CLDSNode::SetMovePartsUpdateFlag(m_cbNodeMovePartsUpdateFlag);
		CLDSNode::ClearUpdateMovePartsBuff();
	}
	if(!bEnableAgain)
		m_bObjectOvered=true;
	return bEnableAgain;
}
bool CNodeMoveOperObject::Terminate(bool bRedrawRelaPartsSolid/*=true*/)
{
	if(!m_bObjectOvered)
	{
		CLDSNode::SetMovePartsUpdateFlag(m_cbOldNodeMovePartsUpdateFlag);
		for(CLDSNode* pNode=CLDSNode::EnumFirstRelaNode();pNode;pNode=CLDSNode::EnumNextRelaNode())
		{
			pNode->CalPosition();
			NODERELAINFO* pNodeRela=Ta.GetNodeRelaInfo(pNode->handle);
			pNodeRela->Visit(_LocalTaskNodeMoveVisitFunc);
		}
		OBJECT_INSTANCE_RUNNING=false;
	}
	if(!m_bObjectOvered)
	{	
		for(CLDSLinePart* pRod=CLDSNode::EnumFirstRelaRod();pRod;pRod=CLDSNode::EnumNextRelaRod())
		{
			pRod->CalPosition();
			if(bRedrawRelaPartsSolid)
			{
				if (pRod->is_visible)
				{
					pRod->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pRod->GetSolidPartObject());
				}
			}
		}
		g_pSolidDraw->Draw();
	}
	return m_bObjectOvered=true;
}
//////////////////////////////////////////////////////////////////////////
//CBoltMoveOperObject
bool CBoltMoveOperObject::OBJECT_INSTANCE_RUNNING=false;
CBoltMoveOperObject::CBoltMoveOperObject(CLDSBolt* pBolt,bool bClearRecordBuff/*=true*/,bool bTemporyDisable/*=false*/)
{
	if(OBJECT_INSTANCE_RUNNING)
		m_bObjectOvered=true;
	else
	{
		OBJECT_INSTANCE_RUNNING=true;	
		m_bObjectOvered=false;
		m_pOperBolt=pBolt;
		m_cbOldBoltMovePartsUpdateFlag=CLDSBolt::GetMovePartsUpdateFlag();
		m_cbBoltMovePartsUpdateFlag =bClearRecordBuff?0x01:0;
		m_cbBoltMovePartsUpdateFlag |=bTemporyDisable?0x02:0;
		CLDSBolt::SetMovePartsUpdateFlag(m_cbBoltMovePartsUpdateFlag);
		CLDSBolt::ClearUpdateMovePartsBuff();
	}
}
CBoltMoveOperObject::~CBoltMoveOperObject(){Terminate();}
bool CBoltMoveOperObject::Enable(bool bEnableAgain/*=true*/)
{
	if(bEnableAgain&&m_bObjectOvered)
	{
		m_bObjectOvered=false;
		CLDSBolt::SetMovePartsUpdateFlag(m_cbBoltMovePartsUpdateFlag);
		CLDSBolt::ClearUpdateMovePartsBuff();
	}
	if(!bEnableAgain)
		m_bObjectOvered=true;
	return bEnableAgain;
}
bool CBoltMoveOperObject::Terminate(bool bRedrawRelaPartsSolid/*=true*/)
{
	if(!m_bObjectOvered)
	{
		CLDSBolt::SetMovePartsUpdateFlag(m_cbOldBoltMovePartsUpdateFlag);
		OBJECT_INSTANCE_RUNNING=false;
	}
	if(!m_bObjectOvered)
	{	
		//for(CLDSLinePart* pRod=CLDSNode::EnumFirstRelaRod();pRod;pRod=CLDSNode::EnumNextRelaRod())
		//{
		//	pRod->CalPosition();
		//	pRod->Create3dSolidModel();
		//	g_pSolidDraw->NewSolidPart(pRod->GetSolidPartObject());
		//}
	}
	return m_bObjectOvered=true;
}
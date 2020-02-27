#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "objptr_list.h"
#include "LogFile.h"
#include "IntelliCoreObject.h"
#ifdef __PUBLIC_RELEASE_COMPILE_
#include "../../env_def.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __RAPID_ROUGH_3DMODEL_
bool CIntelliCore::AddHeadType_HD1(STDTEMPL *pStdTempl)
{
	STDTEMPL::PROFILE_NODE* pParaNode;
	STDTEMPL::PROFILE_ROD* pRod;
	STDTEMPL::PROFILE_QUADSECT* pQuadSect;
	memcpy(pStdTempl->name,"HD1",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=5;	//17
	pParaNode->siMaxDegAngle=40;

	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=290;	//308
	pParaNode->siMaxDegAngle=330;

	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=195;	//215
	pParaNode->siMaxDegAngle=235;

	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=20;	//41
	pParaNode->siMaxDegAngle=60;

	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=95;	//113
	pParaNode->siMaxDegAngle=135;

	pParaNode=pStdTempl->hashParaNodes.Add(7);

	pRod=pStdTempl->hashParaRods.Add(1);
	pRod->widStartNode= 6;
	pRod->widEndNode  = 7;
	pRod=pStdTempl->hashParaRods.Add(2);
	pRod->widStartNode= 1;
	pRod->widEndNode  = 2;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode= 3;
	pRod->widEndNode  = 2;
	pRod=pStdTempl->hashParaRods.Add(4);
	pRod->widStartNode= 4;
	pRod->widEndNode  = 3;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode= 4;
	pRod->widEndNode  = 5;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=1;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode= 6;
	pRod->widEndNode  = 5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=1;
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode= 7;
	pRod->widEndNode  = 3;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode= 3;
	pRod->widEndNode  = 1;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode= 2;
	pRod->widEndNode  =-2;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode= 5;
	pRod->widEndNode  =-5;
	pRod->widRefRodOfWorkWing=5;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//定义主材四边形组
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=4;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=2;
	pQuadSect->arrRods[1]=3;
	return true;
}
#endif

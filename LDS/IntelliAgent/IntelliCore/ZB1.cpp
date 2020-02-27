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
bool CIntelliCore::AddHeadType_ZB1(STDTEMPL *pStdTempl)
{
	STDTEMPL::PROFILE_NODE* pParaNode;
	STDTEMPL::PROFILE_ROD* pRod;
	STDTEMPL::PROFILE_QUADSECT* pQuadSect;
	pStdTempl->ciClassicType='B';	//酒杯塔
	pStdTempl->ciOnRightSide='U';	//塔头类型
	memcpy(pStdTempl->name,"ZB1",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=20;	//酒杯塔上下曲臂交界点右侧
	pParaNode->widLinkPrevRod=1;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=160;
	pParaNode->siMaxDegAngle=185;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->ciPosCalType=22;	//酒杯中上曲臂一与横担X+外侧连接节点
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=18;
	pParaNode->siMinDegAngle=240;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=3;
	pParaNode->siMinDegAngle=15;
	pParaNode->siMaxDegAngle=40;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle=270;
	pParaNode->siMaxDegAngle=320;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=6;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=270;	//230°
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=6;
	pParaNode->siMinDegAngle=25;
	pParaNode->siMaxDegAngle=60;
	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=55;
	pParaNode->siMaxDegAngle=130;
	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=9;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=260;
	//中轴分界线
	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=9;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=260;
	pParaNode=pStdTempl->hashParaNodes.Add(11);
	pParaNode->widLinkPrevRod=10;
	pParaNode->widMasterRod=10;
	pParaNode->siMinDegAngle=55;
	pParaNode->siMaxDegAngle=130;
	pParaNode=pStdTempl->hashParaNodes.Add(12);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=11;
	pParaNode->widMasterRod=12;
	pParaNode->siMinDegAngle=25;
	pParaNode->siMaxDegAngle=60;
	pParaNode=pStdTempl->hashParaNodes.Add(13);
	pParaNode->widLinkPrevRod=12;
	pParaNode->widMasterRod=12;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=270;	//230
	pParaNode=pStdTempl->hashParaNodes.Add(14);
	pParaNode->widLinkPrevRod=13;
	pParaNode->widMasterRod=14;
	pParaNode->siMinDegAngle=270;
	pParaNode->siMaxDegAngle=320;
	pParaNode=pStdTempl->hashParaNodes.Add(15);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=14;
	pParaNode->widMasterRod=15;
	pParaNode->siMinDegAngle=15;
	pParaNode->siMaxDegAngle=40;
	pParaNode=pStdTempl->hashParaNodes.Add(16);
	pParaNode->ciPosCalType=-22;	//酒杯中上曲臂一与横担X-外侧连接节点
	pParaNode->widLinkPrevRod=19;
	pParaNode->widMasterRod=19;
	pParaNode->siMinDegAngle=240;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(17);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=-20;	//酒杯塔上下曲臂交界点右侧
	pParaNode->widLinkPrevRod=16;
	pParaNode->widMasterRod=17;
	pParaNode->siMinDegAngle=160;
	pParaNode->siMaxDegAngle=185;
	pParaNode=pStdTempl->hashParaNodes.Add(18);
	//非轮廓点
	pParaNode=pStdTempl->hashParaNodes.Add(19);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=5;
	pParaNode->raynode.widOrgNode=9;
	pParaNode->raynode.widEdgeStart=3;
	pParaNode->raynode.widEdgeEnd=4;
	pParaNode->raynode.siMinRayAngle=260;
	pParaNode->raynode.siMaxRayAngle=280;
	pParaNode=pStdTempl->hashParaNodes.Add(20);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=5;
	pParaNode->raynode.widOrgNode=10;
	pParaNode->raynode.widEdgeStart=16;
	pParaNode->raynode.widEdgeEnd=15;
	pParaNode->raynode.siMinRayAngle=260;
	pParaNode->raynode.siMaxRayAngle=280;
	pParaNode=pStdTempl->hashParaNodes.Add(21);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 21;
	pParaNode=pStdTempl->hashParaNodes.Add(22);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-21;
	pParaNode->widMasterRod=25;
	pParaNode=pStdTempl->hashParaNodes.Add(23);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 23;
	pParaNode->widMasterRod= 26;
	pParaNode=pStdTempl->hashParaNodes.Add(24);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-23;
	pParaNode->widMasterRod= 27;

	pRod=pStdTempl->hashParaRods.Add(1);
	pRod->widStartNode=2;
	pRod->widEndNode  =1;
	pRod=pStdTempl->hashParaRods.Add(2);	//下弦杆下半部
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(18);
	pRod->widStartNode=3;
	pRod->widEndNode  =19;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode=19;
	pRod->widEndNode  =4;
	//pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(4);
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode=6;
	pRod->widEndNode  =5;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode=6;
	pRod->widEndNode  =7;
	pRod->widRefRodOfWorkWing=8;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode=8;
	pRod->widEndNode  =7;
	pRod->widRefRodOfWorkWing=8;
	pRod->ciAngleLayout=3;
 	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode=8;
	pRod->widEndNode  =9;
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode=10;
	pRod->widEndNode  =9;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode=11;
	pRod->widEndNode  =10;
	pRod=pStdTempl->hashParaRods.Add(11);
	pRod->widStartNode=11;
	pRod->widEndNode  =12;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(12);
	pRod->widStartNode=13;
	pRod->widEndNode  =12;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(13);
	pRod->widStartNode=13;
	pRod->widEndNode  =14;
	pRod=pStdTempl->hashParaRods.Add(14);
	pRod->widStartNode=14;
	pRod->widEndNode  =15;
	pRod=pStdTempl->hashParaRods.Add(15);
	pRod->widStartNode=20;
	pRod->widEndNode  =15;
	pRod=pStdTempl->hashParaRods.Add(19);
	pRod->widStartNode=16;
	pRod->widEndNode  =20;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(16);
	pRod->widStartNode=16;
	pRod->widEndNode  =17;
	pRod=pStdTempl->hashParaRods.Add(17);
	pRod->widStartNode=17;
	pRod->widEndNode  =18;
	//pRod->widRefRodOfWorkWing=2;
	//pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(20);
	pRod->widStartNode= 2;
	pRod->widEndNode  =18;
	pRod=pStdTempl->hashParaRods.Add(21);
	pRod->widStartNode=17;
	pRod->widEndNode  = 1;

	pRod=pStdTempl->hashParaRods.Add(22);
	pRod->widStartNode=23;
	pRod->widEndNode  = 2;
	pRod=pStdTempl->hashParaRods.Add(23);
	pRod->widStartNode=24;
	pRod->widEndNode  =17;
	pRod=pStdTempl->hashParaRods.Add(24);
	pRod->widStartNode=22;
	pRod->widEndNode  =23;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(25);
	pRod->widStartNode=22;
	pRod->widEndNode  =24;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(26);
	pRod->widStartNode=23;
	pRod->widEndNode  = 3;
	pRod=pStdTempl->hashParaRods.Add(27);
	pRod->widStartNode=24;
	pRod->widEndNode  =16;
	pRod=pStdTempl->hashParaRods.Add(28);	//右侧地线支架T型组合水平挂线角钢
	pRod->widStartNode= 7;
	pRod->widEndNode  =-7;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;

	pRod=pStdTempl->hashParaRods.Add(29);	//左侧地线支架T型组合水平挂线角钢
	pRod->widStartNode= 12;
	pRod->widEndNode  =-12;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=12;
	pRod->ciSectStyle=2;
	pRod=pStdTempl->hashParaRods.Add(30);	//右侧导线支架T型组合水平挂线角钢
	pRod->widStartNode= 4;
	pRod->widEndNode  =-4;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=3;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;

	pRod=pStdTempl->hashParaRods.Add(31);	//左侧导线支架T型组合水平挂线角钢
	pRod->widStartNode= 15;
	pRod->widEndNode  =-15;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=15;
	pRod->ciSectStyle=2;
	//定义主材四边形组
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=20;
	pQuadSect->arrRods[1]= 1;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=21;
	pQuadSect->arrRods[1]=17;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 2;
	pQuadSect->arrRods[1]=16;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=22;
	pQuadSect->arrRods[1]= 2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=23;
	pQuadSect->arrRods[1]=16;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=4;
	pQuadSect->arrRods[1]=3;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=14;
	pQuadSect->arrRods[1]=15;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=8;
	pQuadSect->arrRods[1]=5;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=10;
	pQuadSect->arrRods[1]=13;
	return true;
}
#endif
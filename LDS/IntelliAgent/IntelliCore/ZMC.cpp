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
bool CIntelliCore::AddHeadType_ZMC(STDTEMPL *pStdTempl)
{
	STDTEMPL::PROFILE_NODE* pParaNode;
	STDTEMPL::PROFILE_ROD* pRod;
	STDTEMPL::PROFILE_QUADSECT* pQuadSect;
	pStdTempl->ciClassicType='M';	//猫头塔
	pStdTempl->ciOnRightSide='U';	//塔头类型
	memcpy(pStdTempl->name,"ZMC",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=1;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=210;	//230
	pParaNode->siMaxDegAngle=250;

	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=12;	//19
	pParaNode->siMaxDegAngle=40;

	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->ciPosCalType=25;	//猫头中胡须处下曲臂外拐角处顶部节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=215;	//233
	pParaNode->siMaxDegAngle=255;

	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=20;	//酒杯(猫头)塔上下曲臂交界点右侧
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle=160;	//180
	pParaNode->siMaxDegAngle=185;

	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType= 22;	//酒杯(猫头)中上曲臂一与横担X+外侧连接节点
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=160;	//180
	pParaNode->siMaxDegAngle=185;

	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=255;	//276
	pParaNode->siMaxDegAngle=295;

	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=15;	//35
	pParaNode->siMaxDegAngle=55;

	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=9;
	pParaNode->siMinDegAngle=85;	//104
	pParaNode->siMaxDegAngle=125;

	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=10;
	pParaNode->siMinDegAngle=210;	//227
	pParaNode->siMaxDegAngle=250;

	pParaNode=pStdTempl->hashParaNodes.Add(11);
	pParaNode->widLinkPrevRod=10;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=145;	//166
	pParaNode->siMaxDegAngle=185;

	pParaNode=pStdTempl->hashParaNodes.Add(12);
	pParaNode->widLinkPrevRod=11;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=210;	//227
	pParaNode->siMaxDegAngle=250;

	pParaNode=pStdTempl->hashParaNodes.Add(13);
	pParaNode->widLinkPrevRod=12;
	pParaNode->widMasterRod=12;
	pParaNode->siMinDegAngle=85;	//104
	pParaNode->siMaxDegAngle=125;

	pParaNode=pStdTempl->hashParaNodes.Add(14);
	pParaNode->widLinkPrevRod=13;
	pParaNode->widMasterRod=14;
	pParaNode->siMinDegAngle=15;	//35
	pParaNode->siMaxDegAngle=55;

	pParaNode=pStdTempl->hashParaNodes.Add(15);
	pParaNode->widLinkPrevRod=14;
	pParaNode->widMasterRod=14;
	pParaNode->siMinDegAngle=255;	//276
	pParaNode->siMaxDegAngle=295;

	pParaNode=pStdTempl->hashParaNodes.Add(16);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=-22;	//酒杯(猫头)中上曲臂一与横担X+外侧连接节点
	pParaNode->widLinkPrevRod=15;
	pParaNode->widMasterRod=16;
	pParaNode->siMinDegAngle=160;	//180
	pParaNode->siMaxDegAngle=185;

	pParaNode=pStdTempl->hashParaNodes.Add(17);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=-20;	//酒杯(猫头)塔上下曲臂交界点右侧
	pParaNode->widLinkPrevRod=16;
	pParaNode->widMasterRod=17;
	pParaNode->siMinDegAngle=160;	//180
	pParaNode->siMaxDegAngle=185;

	pParaNode=pStdTempl->hashParaNodes.Add(18);
	pParaNode->ciPosCalType=-25;	//猫头中胡须处下曲臂外拐角处顶部节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=17;
	pParaNode->widMasterRod=20;
	pParaNode->siMinDegAngle=215;	//233
	pParaNode->siMaxDegAngle=255;

	pParaNode=pStdTempl->hashParaNodes.Add(19);
	pParaNode->widLinkPrevRod=18;
	pParaNode->widMasterRod=19;
	pParaNode->siMinDegAngle=5;	//19
	pParaNode->siMaxDegAngle=40;

	pParaNode=pStdTempl->hashParaNodes.Add(20);
	pParaNode->widLinkPrevRod=19;
	pParaNode->widMasterRod=20;
	pParaNode->siMinDegAngle=210;	//230
	pParaNode->siMaxDegAngle=250;

	pParaNode=pStdTempl->hashParaNodes.Add(21);
	pParaNode=pStdTempl->hashParaNodes.Add(22);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 28;	//猫头塔自下至上中轴线上的交汇点
	pParaNode->widMasterRod=22;
	pParaNode=pStdTempl->hashParaNodes.Add(23);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 26;	//猫头中胡须处下曲臂内右侧拐角处节点
	pParaNode->widMasterRod=23;
	pParaNode=pStdTempl->hashParaNodes.Add(24);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-26;	//猫头中胡须处下曲臂内右侧拐角处节点
	pParaNode->widMasterRod=24;
	pParaNode=pStdTempl->hashParaNodes.Add(25);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 23;	//酒杯(猫头)中上曲臂一与横担内侧连接节点
	pParaNode->widMasterRod=29;
	pParaNode=pStdTempl->hashParaNodes.Add(26);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-23;	//酒杯(猫头)中上曲臂一与横担内侧连接节点
	pParaNode->widMasterRod=30;
	pParaNode=pStdTempl->hashParaNodes.Add(27);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-21;	//酒杯(猫头)中横担中轴线截面上下节点（正为上侧，负为下侧）
	pParaNode->widMasterRod=30;


	pRod=pStdTempl->hashParaRods.Add(1);
	pRod->widStartNode= 1;
	pRod->widEndNode  = 2;
	pRod=pStdTempl->hashParaRods.Add(2);
	pRod->widStartNode= 2;
	pRod->widEndNode  = 3;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode= 3;
	pRod->widEndNode  = 4;
	pRod=pStdTempl->hashParaRods.Add(4);
	pRod->widStartNode= 5;
	pRod->widEndNode  = 4;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode= 6;
	pRod->widEndNode  = 5;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode= 7;
	pRod->widEndNode  = 6;
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode= 7;
	pRod->widEndNode  = 8;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode= 9;
	pRod->widEndNode  = 8;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode= 9;
	pRod->widEndNode  =10;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode=11;
	pRod->widEndNode  =10;
	pRod=pStdTempl->hashParaRods.Add(11);
	pRod->widStartNode=11;
	pRod->widEndNode  =12;
	pRod=pStdTempl->hashParaRods.Add(12);
	pRod->widStartNode=13;
	pRod->widEndNode  =12;
	pRod=pStdTempl->hashParaRods.Add(13);
	pRod->widStartNode=13;
	pRod->widEndNode  =14;
	pRod->widRefRodOfWorkWing=12;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(14);
	pRod->widStartNode=15;
	pRod->widEndNode  =14;
	pRod->widRefRodOfWorkWing=12;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(15);
	pRod->widStartNode=15;
	pRod->widEndNode  =16;
	pRod=pStdTempl->hashParaRods.Add(16);
	pRod->widStartNode=16;
	pRod->widEndNode  =17;
	pRod=pStdTempl->hashParaRods.Add(17);
	pRod->widStartNode=17;
	pRod->widEndNode  =18;
	pRod=pStdTempl->hashParaRods.Add(18);
	pRod->widStartNode=18;
	pRod->widEndNode  =19;
	pRod=pStdTempl->hashParaRods.Add(19);
	pRod->widStartNode=19;
	pRod->widEndNode  =20;
	pRod=pStdTempl->hashParaRods.Add(20);
	pRod->widStartNode=20;
	pRod->widEndNode  =21;
	pRod=pStdTempl->hashParaRods.Add(21);
	pRod->widStartNode=22;
	pRod->widEndNode  = 1;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(22);
	pRod->widStartNode=22;
	pRod->widEndNode  =21;
	pRod->widRefRodOfWorkWing=20;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(23);
	pRod->widStartNode=23;
	pRod->widEndNode  =22;
	pRod=pStdTempl->hashParaRods.Add(24);
	pRod->widStartNode=24;
	pRod->widEndNode  =22;
	pRod=pStdTempl->hashParaRods.Add(25);
	pRod->widStartNode= 5;
	pRod->widEndNode  =23;
	pRod=pStdTempl->hashParaRods.Add(26);
	pRod->widStartNode=17;
	pRod->widEndNode  =24;
	pRod=pStdTempl->hashParaRods.Add(27);
	pRod->widStartNode=25;
	pRod->widEndNode  = 5;
	pRod=pStdTempl->hashParaRods.Add(28);
	pRod->widStartNode=26;
	pRod->widEndNode  =17;
	pRod=pStdTempl->hashParaRods.Add(29);
	pRod->widStartNode=27;
	pRod->widEndNode  =25;
	pRod=pStdTempl->hashParaRods.Add(30);
	pRod->widStartNode=27;
	pRod->widEndNode  =26;
	pRod=pStdTempl->hashParaRods.Add(31);
	pRod->widStartNode=23;
	pRod->widEndNode  = 4;
	pRod=pStdTempl->hashParaRods.Add(32);
	pRod->widStartNode=24;
	pRod->widEndNode  =18;
	pRod=pStdTempl->hashParaRods.Add(33);
	pRod->widStartNode=10;
	pRod->widEndNode  =25;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(34);
	pRod->widStartNode=12;
	pRod->widEndNode  =26;
	pRod->widRefRodOfWorkWing=11;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	/*pRod=pStdTempl->hashParaRods.Add(35);
	pRod->widStartNode=27;
	pRod->widEndNode  = 4;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(36);
	pRod->widStartNode=28;
	pRod->widEndNode  =22;
	pRod->widRefRodOfWorkWing=24;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(37);
	pRod->widStartNode=12;
	pRod->widEndNode  =29;
	pRod->widRefRodOfWorkWing=37;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(38);
	pRod->widStartNode=14;
	pRod->widEndNode  =30;
	pRod->widRefRodOfWorkWing=38;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;*/
	pRod=pStdTempl->hashParaRods.Add(39);
	pRod->widStartNode= 3;
	pRod->widEndNode  =-3;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(40);
	pRod->widStartNode= 19;
	pRod->widEndNode  =-19;
	pRod->widRefRodOfWorkWing=19;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(41);
	pRod->widStartNode= 8;
	pRod->widEndNode  =-8;
	pRod->widRefRodOfWorkWing=7;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(42);
	pRod->widStartNode= 14;
	pRod->widEndNode  =-14;
	pRod->widRefRodOfWorkWing=15;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//定义主材四边形组
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=23;
	pQuadSect->arrRods[1]= 1;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=24;
	pQuadSect->arrRods[1]=20;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=25;
	pQuadSect->arrRods[1]= 4;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=26;
	pQuadSect->arrRods[1]=17;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=27;
	pQuadSect->arrRods[1]= 5;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=28;
	pQuadSect->arrRods[1]=16;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=3;
	pQuadSect->arrRods[1]=2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=18;
	pQuadSect->arrRods[1]=19;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=10;
	pQuadSect->arrRods[1]=29;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=11;
	pQuadSect->arrRods[1]=30;
	return true;
}
#endif

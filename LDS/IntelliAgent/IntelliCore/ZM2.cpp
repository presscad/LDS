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
bool CIntelliCore::AddHeadType_ZM2(STDTEMPL *pStdTempl)
{
	STDTEMPL::PROFILE_NODE* pParaNode;
	STDTEMPL::PROFILE_ROD* pRod;
	STDTEMPL::PROFILE_QUADSECT* pQuadSect;
	pStdTempl->ciClassicType='M';	//酒杯塔
	pStdTempl->ciOnRightSide='U';	//塔头类型
	memcpy(pStdTempl->name,"ZM2",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=1;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=205;	//226
	pParaNode->siMaxDegAngle=246;

	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=15;	//32
	pParaNode->siMaxDegAngle=52;

	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->ciPosCalType=25;	//猫头中胡须处下曲臂外拐角处顶部节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=210;	//231
	pParaNode->siMaxDegAngle=251;

	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=20;	//酒杯(猫头)塔上下曲臂交界点右侧
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=2;
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
	pParaNode->ciPosCalType=24;	//猫头中横担上梁两侧端节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=6;
	pParaNode->siMinDegAngle=175;	//193
	pParaNode->siMaxDegAngle=213;

	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=245;	//264
	pParaNode->siMaxDegAngle=284;

	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=20;	//39
	pParaNode->siMaxDegAngle=59;

	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=10;
	pParaNode->siMinDegAngle=85;	//103
	pParaNode->siMaxDegAngle=123;

	pParaNode=pStdTempl->hashParaNodes.Add(11);
	pParaNode->widLinkPrevRod=10;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=135;	//153
	pParaNode->siMaxDegAngle=173;

	pParaNode=pStdTempl->hashParaNodes.Add(12);
	pParaNode->ciPosCalType=27;	//±27:猫头中横担上梁与两侧地线支架内拐点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=11;
	pParaNode->widMasterRod=12;
	pParaNode->siMinDegAngle=245;	//262
	pParaNode->siMaxDegAngle=282;

	pParaNode=pStdTempl->hashParaNodes.Add(13);
	pParaNode->ciPosCalType=21;	//±21:酒杯(猫头)中横担中轴线截面上下节点（正为上侧，负为下侧）
	pParaNode->widLinkPrevRod=12;
	pParaNode->widMasterRod=12;
	pParaNode->siMinDegAngle=130;	//149
	pParaNode->siMaxDegAngle=169;

	pParaNode=pStdTempl->hashParaNodes.Add(14);
	pParaNode->ciPosCalType=-27;	//±27:猫头中横担上梁与两侧地线支架内拐点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=13;
	pParaNode->widMasterRod=13;
	pParaNode->siMinDegAngle=245;	//262
	pParaNode->siMaxDegAngle=282;

	pParaNode=pStdTempl->hashParaNodes.Add(15);
	pParaNode->widLinkPrevRod=14;
	pParaNode->widMasterRod=14;
	pParaNode->siMinDegAngle=135;	//153
	pParaNode->siMaxDegAngle=173;

	pParaNode=pStdTempl->hashParaNodes.Add(16);
	pParaNode->widLinkPrevRod=15;
	pParaNode->widMasterRod=15;
	pParaNode->siMinDegAngle=85;	//103
	pParaNode->siMaxDegAngle=123;

	pParaNode=pStdTempl->hashParaNodes.Add(17);
	pParaNode->widLinkPrevRod=16;
	pParaNode->widMasterRod=17;
	pParaNode->siMinDegAngle=20;	//39
	pParaNode->siMaxDegAngle=59;

	pParaNode=pStdTempl->hashParaNodes.Add(18);
	pParaNode->widLinkPrevRod=17;
	pParaNode->widMasterRod=17;
	pParaNode->siMinDegAngle=245;	//264
	pParaNode->siMaxDegAngle=284;

	pParaNode=pStdTempl->hashParaNodes.Add(19);
	pParaNode->ciPosCalType=-24;	//猫头中横担上梁两侧端节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=18;
	pParaNode->widMasterRod=19;
	pParaNode->siMinDegAngle=175;	//193
	pParaNode->siMaxDegAngle=213;

	pParaNode=pStdTempl->hashParaNodes.Add(20);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=-22;	//酒杯(猫头)中上曲臂一与横担X+外侧连接节点
	pParaNode->widLinkPrevRod=19;
	pParaNode->widMasterRod=20;
	pParaNode->siMinDegAngle=160;	//180
	pParaNode->siMaxDegAngle=185;

	pParaNode=pStdTempl->hashParaNodes.Add(21);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=-20;	//酒杯(猫头)塔上下曲臂交界点右侧
	pParaNode->widLinkPrevRod=20;
	pParaNode->widMasterRod=21;
	pParaNode->siMinDegAngle=160;	//180
	pParaNode->siMaxDegAngle=185;

	pParaNode=pStdTempl->hashParaNodes.Add(22);
	pParaNode->ciPosCalType=-25;	//猫头中胡须处下曲臂外拐角处顶部节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=21;
	pParaNode->widMasterRod=24;
	pParaNode->siMinDegAngle=210;	//231
	pParaNode->siMaxDegAngle=251;

	pParaNode=pStdTempl->hashParaNodes.Add(23);
	pParaNode->widLinkPrevRod=22;
	pParaNode->widMasterRod=23;
	pParaNode->siMinDegAngle=15;	//32
	pParaNode->siMaxDegAngle=52;

	pParaNode=pStdTempl->hashParaNodes.Add(24);
	pParaNode->widLinkPrevRod=23;
	pParaNode->widMasterRod=24;
	pParaNode->siMinDegAngle=205;	//226
	pParaNode->siMaxDegAngle=246;
	pParaNode=pStdTempl->hashParaNodes.Add(25);	//塔头与身的左侧连接点
	pParaNode=pStdTempl->hashParaNodes.Add(26);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 28;
	pParaNode->widMasterRod=26;
	pParaNode=pStdTempl->hashParaNodes.Add(27);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 26;	//猫头中胡须处下曲臂内右侧拐角处节点
	pParaNode->widMasterRod=27;
	pParaNode=pStdTempl->hashParaNodes.Add(28);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-26;	//猫头中胡须处下曲臂内右侧拐角处节点
	pParaNode->widMasterRod=28;
	pParaNode=pStdTempl->hashParaNodes.Add(29);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 23;	//酒杯(猫头)中上曲臂一与横担内侧连接节点
	pParaNode->widMasterRod=35;
	pParaNode=pStdTempl->hashParaNodes.Add(30);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-23;	//酒杯(猫头)中上曲臂一与横担内侧连接节点
	pParaNode->widMasterRod=36;
	pParaNode=pStdTempl->hashParaNodes.Add(31);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-21;	//酒杯(猫头)中横担中轴线截面上下节点（正为上侧，负为下侧）
	pParaNode->widMasterRod=33;


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
	pRod->widStartNode= 8;
	pRod->widEndNode  = 7;
	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode= 8;
	pRod->widEndNode  = 9;
	pRod->widRefRodOfWorkWing=11;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode=10;
	pRod->widEndNode  = 9;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode=10;
	pRod->widEndNode  =11;
	pRod->widRefRodOfWorkWing=8;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(11);
	pRod->widStartNode=11;
	pRod->widEndNode  =12;
	pRod=pStdTempl->hashParaRods.Add(12);
	pRod->widStartNode=12;
	pRod->widEndNode  =13;
	pRod=pStdTempl->hashParaRods.Add(13);
	pRod->widStartNode=13;
	pRod->widEndNode  =14;
	pRod=pStdTempl->hashParaRods.Add(14);
	pRod->widStartNode=14;
	pRod->widEndNode  =15;
	pRod=pStdTempl->hashParaRods.Add(15);
	pRod->widStartNode=16;
	pRod->widEndNode  =15;
	pRod->widRefRodOfWorkWing=17;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(16);
	pRod->widStartNode=16;
	pRod->widEndNode  =17;
	pRod->widRefRodOfWorkWing=15;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(17);
	pRod->widStartNode=18;
	pRod->widEndNode  =17;
	pRod->widRefRodOfWorkWing=14;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
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
	pRod->widStartNode=21;
	pRod->widEndNode  =22;
	pRod=pStdTempl->hashParaRods.Add(22);
	pRod->widStartNode=22;
	pRod->widEndNode  =23;
	pRod=pStdTempl->hashParaRods.Add(23);
	pRod->widStartNode=23;
	pRod->widEndNode  =24;
	pRod=pStdTempl->hashParaRods.Add(24);
	pRod->widStartNode=24;
	pRod->widEndNode  =25;
	pRod=pStdTempl->hashParaRods.Add(25);
	pRod->widStartNode=26;
	pRod->widEndNode  = 1;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(26);
	pRod->widStartNode=26;
	pRod->widEndNode  =25;
	pRod->widRefRodOfWorkWing=24;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(27);
	pRod->widStartNode=27;
	pRod->widEndNode  =26;
	pRod=pStdTempl->hashParaRods.Add(28);
	pRod->widStartNode=28;
	pRod->widEndNode  =26;
	pRod=pStdTempl->hashParaRods.Add(29);
	pRod->widStartNode= 5;
	pRod->widEndNode  =27;
	pRod=pStdTempl->hashParaRods.Add(30);
	pRod->widStartNode=21;
	pRod->widEndNode  =28;
	pRod=pStdTempl->hashParaRods.Add(31);
	pRod->widStartNode=29;
	pRod->widEndNode  = 5;
	pRod=pStdTempl->hashParaRods.Add(32);
	pRod->widStartNode=30;
	pRod->widEndNode  =21;
	pRod=pStdTempl->hashParaRods.Add(33);
	pRod->widStartNode=31;
	pRod->widEndNode  =29;
	pRod=pStdTempl->hashParaRods.Add(34);
	pRod->widStartNode=31;
	pRod->widEndNode  =30;
	pRod=pStdTempl->hashParaRods.Add(35);
	pRod->widStartNode=29;
	pRod->widEndNode  = 6;
	pRod=pStdTempl->hashParaRods.Add(36);
	pRod->widStartNode=30;
	pRod->widEndNode  =20;
	pRod=pStdTempl->hashParaRods.Add(37);
	pRod->widStartNode=12;
	pRod->widEndNode  = 7;
	pRod->widRefRodOfWorkWing=35;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(38);
	pRod->widStartNode=14;
	pRod->widEndNode  =19;
	pRod->widRefRodOfWorkWing=36;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(39);
	pRod->widStartNode=27;
	pRod->widEndNode  = 4;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(40);
	pRod->widStartNode=28;
	pRod->widEndNode  =22;
	pRod->widRefRodOfWorkWing=24;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(41);
	pRod->widStartNode=12;
	pRod->widEndNode  =29;
	pRod->widRefRodOfWorkWing=37;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(42);
	pRod->widStartNode=14;
	pRod->widEndNode  =30;
	pRod->widRefRodOfWorkWing=38;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(43);
	pRod->widStartNode= 3;
	pRod->widEndNode  =-3;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(44);
	pRod->widStartNode= 23;
	pRod->widEndNode  =-23;
	pRod->widRefRodOfWorkWing=23;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(45);
	pRod->widStartNode= 9;
	pRod->widEndNode  =-9;
	pRod->widRefRodOfWorkWing=8;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(46);
	pRod->widStartNode= 17;
	pRod->widEndNode  =-17;
	pRod->widRefRodOfWorkWing=17;
	pRod->ciAngleLayout=1;
	pRod->ciNearNormAxis=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//定义主材四边形组
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=27;
	pQuadSect->arrRods[1]= 1;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=28;
	pQuadSect->arrRods[1]=24;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 3;
	pQuadSect->arrRods[1]= 2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=22;
	pQuadSect->arrRods[1]=23;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=29;
	pQuadSect->arrRods[1]= 4;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=21;
	pQuadSect->arrRods[1]=30;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 5;
	pQuadSect->arrRods[1]=31;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=20;
	pQuadSect->arrRods[1]=32;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=12;
	pQuadSect->arrRods[1]=33;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=13;
	pQuadSect->arrRods[1]=34;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=11;
	pQuadSect->arrRods[1]= 7;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=14;
	pQuadSect->arrRods[1]=18;
	return true;
}
#endif

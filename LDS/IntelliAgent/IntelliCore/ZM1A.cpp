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
bool CIntelliCore::AddHeadType_ZM1A(STDTEMPL *pStdTempl,char ciSerial/*=0*/)
{
	STDTEMPL::PROFILE_NODE* pParaNode;
	STDTEMPL::PROFILE_ROD* pRod;
	STDTEMPL::PROFILE_QUADSECT* pQuadSect;
	pStdTempl->ciClassicType='M';	//猫头塔
	pStdTempl->ciOnRightSide='U';	//塔头类型
	if(ciSerial==0)
		memcpy(pStdTempl->name,"ZM1A",5);
	else
		memcpy(pStdTempl->name,"ZM1A2",6);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=1;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=210;
	pParaNode->siMaxDegAngle=245;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=75;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->ciPosCalType=25;	//猫头中胡须处下曲臂外拐角处顶部节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=37;
	pParaNode->siMinDegAngle=180;
	pParaNode->siMaxDegAngle=230;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=20;	//酒杯(猫头)塔上下曲臂交界点右侧
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle=160;
	pParaNode->siMaxDegAngle=185;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	if(ciSerial>0)
		pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=22;	//酒杯(猫头)中上曲臂一与横担X+外侧连接节点
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=31;
	pParaNode->siMinDegAngle=175;
	pParaNode->siMaxDegAngle=200;
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->ciPosCalType=24;	//猫头中横担上梁两侧端节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=33;
	pParaNode->siMinDegAngle=185;
	pParaNode->siMaxDegAngle=220;
	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=240;
	pParaNode->siMaxDegAngle=275;
	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=70;
	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=10;
	pParaNode->siMinDegAngle=60;
	pParaNode->siMaxDegAngle=110;
	pParaNode=pStdTempl->hashParaNodes.Add(11);
	pParaNode->ciPosCalType=27;	//猫头中横担上梁与两侧地线支架右侧内拐点
	pParaNode->widLinkPrevRod=10;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=210;
	pParaNode->siMaxDegAngle=265;
	//中轴分界线
	pParaNode=pStdTempl->hashParaNodes.Add(12);
	pParaNode->ciProfileNode=11;
	pParaNode->ciPosCalType=21;	//酒杯(猫头)中横担中轴线截面上下节点（正为上侧，负为下侧）
	pParaNode->siMinDegAngle=175;
	pParaNode->siMaxDegAngle=195;
	//左侧
	pParaNode=pStdTempl->hashParaNodes.Add(13);
	pParaNode->ciPosCalType=-27;//猫头中横担上梁与两侧地线支架左侧内拐点
	pParaNode->widLinkPrevRod=12;
	pParaNode->widMasterRod=12;
	pParaNode->siMinDegAngle=210;
	pParaNode->siMaxDegAngle=265;
	pParaNode=pStdTempl->hashParaNodes.Add(14);
	pParaNode->widLinkPrevRod=13;
	pParaNode->widMasterRod=13;
	pParaNode->siMinDegAngle=60;
	pParaNode->siMaxDegAngle=110;
	pParaNode=pStdTempl->hashParaNodes.Add(15);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=14;
	pParaNode->widMasterRod=15;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=70;
	pParaNode=pStdTempl->hashParaNodes.Add(16);
	pParaNode->widLinkPrevRod=15;
	pParaNode->widMasterRod=15;
	pParaNode->siMinDegAngle=240;
	pParaNode->siMaxDegAngle=275;
	pParaNode=pStdTempl->hashParaNodes.Add(17);
	pParaNode->ciPosCalType=-24;	//猫头中横担上梁两侧端节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=16;
	pParaNode->widMasterRod=34;
	pParaNode->siMinDegAngle=185;
	pParaNode->siMaxDegAngle=220;
	pParaNode=pStdTempl->hashParaNodes.Add(18);
	if(ciSerial>0)
		pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=-22;	//酒杯(猫头)中上曲臂一与横担X+外侧连接节点
	pParaNode->widLinkPrevRod=17;
	pParaNode->widMasterRod=32;
	pParaNode->siMinDegAngle=175;
	pParaNode->siMaxDegAngle=200;
	pParaNode=pStdTempl->hashParaNodes.Add(19);
	pParaNode->ciProfileNode=2;	//非必须轮廓点
	pParaNode->ciPosCalType=-20;	//酒杯(猫头)塔上下曲臂交界点右侧
	pParaNode->widLinkPrevRod=18;
	pParaNode->widMasterRod=19;
	pParaNode->siMinDegAngle=160;
	pParaNode->siMaxDegAngle=185;
	pParaNode=pStdTempl->hashParaNodes.Add(20);
	pParaNode->ciPosCalType=-25;	//猫头中胡须处下曲臂外拐角处顶部节点（正为右侧，负为左侧）
	pParaNode->widLinkPrevRod=19;
	pParaNode->widMasterRod=38;
	pParaNode->siMinDegAngle=180;
	pParaNode->siMaxDegAngle=230;
	pParaNode=pStdTempl->hashParaNodes.Add(21);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=20;
	pParaNode->widMasterRod=21;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=75;

	pParaNode=pStdTempl->hashParaNodes.Add(22);
	pParaNode->widLinkPrevRod=21;
	pParaNode->widMasterRod=22;
	pParaNode->siMinDegAngle=210;
	pParaNode->siMaxDegAngle=245;
	pParaNode=pStdTempl->hashParaNodes.Add(23);
	//非轮廓点
	pParaNode=pStdTempl->hashParaNodes.Add(24);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 28;
	pParaNode->widMasterRod=24;
	pParaNode=pStdTempl->hashParaNodes.Add(25);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 26;	//猫头中胡须处下曲臂内右侧拐角处节点
	pParaNode->widMasterRod=25;
	pParaNode=pStdTempl->hashParaNodes.Add(26);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-26;	//猫头中胡须处下曲臂内左侧拐角处节点
	pParaNode->widMasterRod=26;
	pParaNode=pStdTempl->hashParaNodes.Add(27);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 23;	//酒杯(猫头)中上曲臂一与横担内侧连接节点
	pParaNode->widMasterRod=31;
	pParaNode=pStdTempl->hashParaNodes.Add(28);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-23;	//酒杯(猫头)中上曲臂一与横担内侧连接节点
	pParaNode->widMasterRod=32;
	pParaNode=pStdTempl->hashParaNodes.Add(29);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-21;	//酒杯(猫头)中横担中轴线截面上下节点（正为上侧，负为下侧）
	pParaNode->widMasterRod=36;

	pRod=pStdTempl->hashParaRods.Add(1);
	pRod->widStartNode=2;
	pRod->widEndNode  =1;
	pRod=pStdTempl->hashParaRods.Add(2);	//下弦杆下半部
	pRod->widStartNode=2;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	//pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(4);
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode=6;
	pRod->widEndNode  =5;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode=7;
	pRod->widEndNode  =6;
	pRod->widRefRodOfWorkWing=33;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode=8;
	pRod->widEndNode  =7;
 	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode=8;
	pRod->widEndNode  =9;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode=10;
	pRod->widEndNode  =9;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode=10;
	pRod->widEndNode  =11;
	pRod=pStdTempl->hashParaRods.Add(11);
	pRod->widStartNode=12;
	pRod->widEndNode  =11;
	pRod=pStdTempl->hashParaRods.Add(12);
	pRod->widStartNode=13;
	pRod->widEndNode  =12;
	pRod=pStdTempl->hashParaRods.Add(13);
	pRod->widStartNode=14;
	pRod->widEndNode  =13;
	pRod->widRefRodOfWorkWing=12;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(14);
	pRod->widStartNode=14;
	pRod->widEndNode  =15;
	pRod->widRefRodOfWorkWing=13;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(15);
	pRod->widStartNode=16;
	pRod->widEndNode  =15;
	pRod->widRefRodOfWorkWing=13;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(16);
	pRod->widStartNode=16;
	pRod->widEndNode  =17;
	pRod->widRefRodOfWorkWing=36;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(17);
	pRod->widStartNode=17;
	pRod->widEndNode  =18;
	pRod->widRefRodOfWorkWing=34;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(18);
	pRod->widStartNode=18;
	pRod->widEndNode  =19;
	pRod=pStdTempl->hashParaRods.Add(19);//横梁下平面左侧主梁
	pRod->widStartNode=19;
	pRod->widEndNode  =20;
	pRod=pStdTempl->hashParaRods.Add(20);
	pRod->widStartNode=20;
	pRod->widEndNode  =21;
	pRod=pStdTempl->hashParaRods.Add(21);
	pRod->widStartNode=22;
	pRod->widEndNode  =21;
	pRod=pStdTempl->hashParaRods.Add(22);
	pRod->widStartNode=22;
	pRod->widEndNode  =23;
	//非轮廓边界的关键杆件
	pRod=pStdTempl->hashParaRods.Add(23);
	pRod->widStartNode=24;
	pRod->widEndNode  = 1;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(24);
	pRod->widStartNode=24;
	pRod->widEndNode  =23;
	pRod->widRefRodOfWorkWing=22;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(25);
	pRod->widStartNode=25;
	pRod->widEndNode  =24;
	pRod=pStdTempl->hashParaRods.Add(26);
	pRod->widStartNode=26;
	pRod->widEndNode  =24;
	pRod=pStdTempl->hashParaRods.Add(27);
	pRod->widStartNode= 5;
	pRod->widEndNode  =25;
	pRod=pStdTempl->hashParaRods.Add(28);
	pRod->widStartNode=19;
	pRod->widEndNode  =26;
	pRod=pStdTempl->hashParaRods.Add(29);
	pRod->widStartNode=27;
	pRod->widEndNode  = 5;
	pRod=pStdTempl->hashParaRods.Add(30);
	pRod->widStartNode=28;
	pRod->widEndNode  =19;
	pRod=pStdTempl->hashParaRods.Add(31);
	pRod->widStartNode=27;
	pRod->widEndNode  = 6;
	pRod->widRefRodOfWorkWing=29;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(32);
	pRod->widStartNode=28;
	pRod->widEndNode  =18;
	pRod->widRefRodOfWorkWing=30;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(33);
	pRod->widStartNode=11;
	pRod->widEndNode  = 7;
	pRod->widRefRodOfWorkWing=35;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(34);
	pRod->widStartNode=13;
	pRod->widEndNode  =17;
	pRod->widRefRodOfWorkWing=36;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(35);
	pRod->widStartNode=29;
	pRod->widEndNode  =27;
	pRod=pStdTempl->hashParaRods.Add(36);
	pRod->widStartNode=29;
	pRod->widEndNode  =28;
	pRod=pStdTempl->hashParaRods.Add(37);
	pRod->widStartNode= 4;
	pRod->widEndNode  = 2;
	pRod=pStdTempl->hashParaRods.Add(38);
	pRod->widStartNode=20;
	pRod->widEndNode  =22;
	pRod=pStdTempl->hashParaRods.Add(39);	//右侧地线支架T型组合水平挂线角钢
	pRod->widStartNode= 9;
	pRod->widEndNode  =-9;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=8;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(40);	//左侧地线支架T型组合水平挂线角钢
	pRod->widStartNode= 15;
	pRod->widEndNode  =-15;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=14;
	pRod->ciSectStyle=2;
	//定义主材四边形组
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=25;
	pQuadSect->arrRods[1]= 1;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=26;
	pQuadSect->arrRods[1]=22;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 3;
	pQuadSect->arrRods[1]= 2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=20;
	pQuadSect->arrRods[1]=21;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=27;
	pQuadSect->arrRods[1]= 4;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=28;
	pQuadSect->arrRods[1]=19;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=29;
	pQuadSect->arrRods[1]= 5;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=30;
	pQuadSect->arrRods[1]=18;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=33;
	pQuadSect->arrRods[1]=31;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=34;
	pQuadSect->arrRods[1]=32;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=11;
	pQuadSect->arrRods[1]=35;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=12;
	pQuadSect->arrRods[1]=36;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=10;
	pQuadSect->arrRods[1]= 7;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=13;
	pQuadSect->arrRods[1]=16;
	return true;
}
#endif

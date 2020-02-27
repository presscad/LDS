#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD4���ӵ�ģ�����
void CStdComTemplConnPoint::InitConnPointTempl_DX3()
{
	name="XH_DX3";
	szSketchImgFile = "DX3.bmp";
	description="����֧�����Ҹ������ཻ�ڵ�";
	widBaseRod=1;	//����֧�ܵ����Ǹ�
	widParentRod=1;	//ͬ��
	this->ciBaseRodConnState=1;	//1.�м������
	this->xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=20;
	xView.siMaxBaseRodAngle=60;
	this->m_bEnableIntelliConnPointDesign=true;
	rodarr.ZeroPresetMemory();
	//1.�ڲ��Ҹ�
	InitSchemaRod(rodarr[0],1,-1,1,1,'y','A',true);
	//1.ˮƽ������׼�Ǹ�
	InitSchemaRod(rodarr[1],2,95,160,1,'y','R');
	rodarr[1].xFlag.bFilterInsideAngle=true;
	rodarr[1].xFlag.bFilterHoriPrimaryRod=true;
	//3.�²�֧�Ÿ�
	InitSchemaRod(rodarr[2],3,30,75,1,'Y','R');
	rodarr.ReSize(3);

	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DX3);
	pMatchPlate->InitDefaultMatchRods(3);
}
void CStdComTemplConnPoint::InitConnPointTempl_DX3A()
{
	name="XH_DX3A";
	szSketchImgFile = "DX3A.bmp";
	description="����֧�����Ҹ�(���߽ض�)�����ཻ�ڵ�";
	widBaseRod=1;	//����֧�ܵ����Ǹ�
	widParentRod=1;	//ͬ��
	//this->ciBaseRodConnState=0;	//4.���߽ضϵ�����
	this->xView.ciBaseAxisXMode=1;	//��׼�����߷���Ϊ
	this->xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=0;
	xView.siMaxBaseRodAngle=5;
	this->m_bEnableIntelliConnPointDesign=true;
	rodarr.ZeroPresetMemory();
	//1.ˮƽ������׼�Ǹ�
	InitSchemaRod(rodarr[0],1,0,1,1,'x','R',true);
	rodarr[0].xFlag.bFilterHoriPrimaryRod=true;
	//2.�ڲ��Ҹˣ��ϲ�)
	InitSchemaRod(rodarr[1],2,30, 50, 1, 'y', 'R');
	//3.�ڲ��Ҹˣ��²�)
	InitSchemaRod(rodarr[2],3,210,230,1,'y','R');
	//4.�²�֧�Ÿ�
	InitSchemaRod(rodarr[3],4,240,280,1,'X','R');
	rodarr.ReSize(4);

	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_XH_DX3A);
	pMatchPlate->InitDefaultMatchRods(4);
}
//DX3���Ӱ�ģ�����
void CStdComTempl::InitNodePlateTemp_DX3()
{
	szSketchImgFile = "DX3.bmp";
	this->m_bEnableIntelliConnPointDesign=true;
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	pParaAngle->xFlag.bFilterWorkWing=true;		//Ҫ�������жϹ���֫����
	pParaAngle->xFlag.bFilterInsideAngle=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->ciAngleWorkWing='X';
	//pParaAngle->xFlag.bInheritWorkWing=true;	//�̳й���֫������Ĭ��ֵ
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=1;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T|]=-R1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	CStdTemplParaPlate* pParaPlate=this->listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode=1;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.ciProfileStyle=1;	//��������
	pParaPlate->design.ciDatumLenAxis='Y';
	pParaPlate->design.ciDatumAngleWing='Y';
	pParaPlate->design.bend.widAngleBendBy=0;
	pParaPlate->connrods.Clear();
	//1.����֧���ڲ��Ҹ�
	pParaPlate->connrods[0].sidRod=1;
	pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=-1;
	pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=1;
	pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[0].ciCurrTipSideS0E1=2;
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].cnMinBoltN=3;
	pParaPlate->connrods[0].dualSideEdge=true;
	pParaPlate->connrods[0].ciConnectWing=0;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=1;
	pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts[0].idDirectConnectToAngle1=3;	//3����֧�ŲĹ���ֱ������˨
	pParaPlate->connrods[0].layoutbolts[1].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[1].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[1].cnCount=1;
	pParaPlate->connrods[0].layoutbolts[1].ciRows=1;
	pParaPlate->connrods[0].layoutbolts[1].ciLayoutStyle=0;
	pParaPlate->connrods[0].layoutbolts[1].ciDatumWing=0;//'Y';
	StrCopy(pParaPlate->connrods[0].layoutbolts[1].szLenOffsetOf1stBolt,"LS",8);
	pParaPlate->connrods[0].layoutbolts[1].ciFlowDirection=BOLT_LAYOUT::FLOW_EtoS;
	//TODO:Ŀǰ��˨���û��Ƚ����Դ����������˨���������ֻ����ʱ wjh-2019.10.30
	pParaPlate->connrods[0].layoutbolts[2].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[2].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[2].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[2].ciRows=1;
	pParaPlate->connrods[0].layoutbolts[2].ciLayoutStyle=0;
	pParaPlate->connrods[0].layoutbolts[2].ciDatumWing=0;//'Y';
	StrCopy(pParaPlate->connrods[0].layoutbolts[2].szLenOffsetOf1stBolt,"LS+30",8);	//'LS+30'Ϊ��ʱֵ���Ժ�Ӧϸ��
	pParaPlate->connrods[0].layoutbolts[2].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(3);
	//2.ˮƽ���
	pParaPlate->connrods[1].sidRod=2;
	pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=95;	//120
	pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=160;
	pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[1].InitTipInfo(0,0,1,0);
	pParaPlate->connrods[1].ciPlaneNo=1;
	pParaPlate->connrods[1].cnMinBoltN=2;
	pParaPlate->connrods[1].dualSideEdge=false;
	pParaPlate->connrods[1].ciConnectWing='Y';
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.�²ศ��
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=30;
	pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=75;
	pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[2].InitTipInfo(0,1);
	pParaPlate->connrods[2].ciPlaneNo=1;
	pParaPlate->connrods[2].cnMinBoltN=1;
	pParaPlate->connrods[2].dualSideEdge=false;
	pParaPlate->connrods[2].ciConnectWing=0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=BOLT_ORIGIN::LOCA_AXISINTERS;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseAngle=3;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleG=0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widCrossAngle=1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciStyleCrossG=7;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(3);
}
void CStdComTempl::InitNodePlateTemp_DX3A()
{
	szSketchImgFile = "DX3A.bmp";
	this->m_bEnableIntelliConnPointDesign=true;
	m_cbDisableConnPlaneFlag=0x80;	//��Ƴɹ�ͬ��ֹͬһ�ڵ㴦�Ľ�ͷ��� wjh-2019.11.4
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8

	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	pParaNode->keyLabelFatherRod = 1;	//�趨�ڵ�ĸ��˼�
	CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='y';
	pParaAngle->m_bNecessaryRod=true;
	//pParaAngle->desStart.szXorCtrlWingOffset="g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	
	pParaAngle->xFlag.bFilterWorkWing=true;		//Ҫ�������жϹ���֫����
	pParaAngle->xFlag.bFilterInsideAngle=true;
	//2.�ڲ��Ҹˣ��ϲ�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->desEnd.cTouchLineType = 0;
	pParaAngle->desEnd.cTouchLine = 1;//'Y';
	pParaAngle->desEnd.uidTouchLineRod = 1;
	pParaAngle->desEnd.szXorCtrlWingOffset = "g";
	pParaAngle->desEnd.szYorOthrWingOffset = "[=-P1#T";	
	pParaAngle->bUpdateEndPos = true;
	//3.�ڲ��Ҹˣ��²�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->desStart.cTouchLineType = 0;
	pParaAngle->desStart.cTouchLine = 1;//'Y';
	pParaAngle->desStart.uidTouchLineRod = 1;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";
	pParaAngle->bUpdateStartPos = true;
	//4.���²�֧�Ÿ�
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=1;//
	pParaAngle->desStart.uidTouchLineRod = 1;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";	
	pParaAngle->bUpdateStartPos=true;
	CStdTemplParaPlate* pParaPlate=this->listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode=1;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.ciProfileStyle=1;	//��������
	pParaPlate->design.ciDatumLenAxis='Y';
	pParaPlate->design.ciDatumAngleWing='Y';
	pParaPlate->design.bend.widAngleBendBy=0;
	pParaPlate->connrods.Clear();
	//1.ˮƽ���
	pParaPlate->connrods[0].InitBasicInfo(1, 1, 1, 5,true);
	pParaPlate->connrods[0].InitTipInfo(0, 1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].cnMaxSingularLineBolts=5;
	pParaPlate->connrods[0].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.����֧���ڲ��Ҹˣ��ϲ�)
	pParaPlate->connrods[1].InitBasicInfo(2, 1, 1, 2, true);
	pParaPlate->connrods[1].InitTipInfo(1, 0, 1);
	pParaPlate->connrods[1].dynaRod.siRayMinDegAngle = 30;
	pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle = 50;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseAngle = 0;
	pParaPlate->connrods[1].layoutbolts[0].cnCount = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows = 1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.����֧���ڲ��Ҹˣ��²�)
	pParaPlate->connrods[2].InitBasicInfo(3, 1, 1, 2, true);
	pParaPlate->connrods[2].InitTipInfo(0,0,1);
	pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=210;
	pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle=230;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseAngle=0;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.�²ศ��
	pParaPlate->connrods[3].InitBasicInfo(4, 1, 1, 2,true);
	pParaPlate->connrods[3].InitTipInfo(0,0,1,3);
	pParaPlate->connrods[3].dynaRod.siRayMinDegAngle=240;
	pParaPlate->connrods[3].dynaRod.siRayMaxDegAngle=280;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows=1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing = 'X';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(4);
}

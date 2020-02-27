#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD1���ӵ�ģ�����
void CStdComTemplConnPoint::InitConnPointTempl_BP1or2(long idStdComTmpl)
{	//0x02030001
	name="BP1_2";//ID  7
	description="������������µ����";
	widBaseRod=1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->xView.ciViewPlaneNormal='H';
	this->xView.siMinBaseRodAngle=-90;
	this->xView.siMaxBaseRodAngle=-80;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;
	//1.�²���������
	InitSchemaRod(rodarr[0],1,175,185,1,'X','R');
	rodarr[0].xFlag.bFilterWorkWing=true;
	//2.���ˮƽ���
	InitSchemaRod(rodarr[1],2,80,90,1,'Y','R');
	rodarr[1].xFlag.bFilterWorkWing=true;
	//3.��������
	InitSchemaRod(rodarr[2],3,-30,5,1,'Y','R');
	rodarr[2].ciPlaneNo=2;
	rodarr[2].xFlag.bFilterWorkWing=true;
	//4.����б����
	InitSchemaRod(rodarr[3],4,20,70,0,'Y','R');
	rodarr[3].ciPlaneNo=2;
	//5.����б����
	InitSchemaRod(rodarr[4],5,100,160,0,'X','R');
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_BP1);
	pMatchPlate->InitDefaultMatchRods(5);
	pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_BP2);
	pMatchPlate->InitDefaultMatchRods(5);
}
void CStdComTemplConnPoint::InitConnPointTempl_BP3()
{	//0x02030003
	name="BP3";//ID  7
	szSketchImgFile = "BP3.bmp";
	description="�����ͷ����λ�ô�";
	widBaseRod=1;	//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->xView.ciViewPlaneNormal = 'Y';
	this->xView.siMinBaseRodAngle = -90;
	this->xView.siMaxBaseRodAngle = -70;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//1.�²���������
	InitSchemaRod(rodarr[0],1,179,181,1,2,'R');
	//2.���ˮƽ��
	InitSchemaRod(rodarr[1],2,80,90,1,'Y','R');
	rodarr[1].xFlag.bFilterWorkWing=true;
	//3.�������ϲ�����������
	InitSchemaRod(rodarr[2],3,-70,-16,1,'Y','R');
	rodarr[2].ciPlaneNo=2;
	rodarr[2].xFlag.bFilterWorkWing=true;
	//4.����б��
	InitSchemaRod(rodarr[3],4,20,70,0,'Y','R');
	rodarr[3].ciPlaneNo=2;
	//5.����б����
	InitSchemaRod(rodarr[4],5,100,160,0,'X','R');
	//6.�����ϲ��м���б(��)�ĸ�
	InitSchemaRod(rodarr[5],6,-15,15,2,'Y','R');
	rodarr.ReSize(6);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_BP3);
	pMatchPlate->InitDefaultMatchRods(6);
}

//HD1���Ӱ�ģ�����
void CStdComTempl::InitNodePlateTemp_BP1or2(long idStdComTempl)
{
	if (idStdComTempl == STDTEMPL_NODEPLATE_BP1)
		name.Copy("BP1");//0x03034201,������Ϊһ�鵹K��
	else
		name.Copy("BP2");//0x03034202,������Ϊһ�齻���
	m_bEnableIntelliConnPointDesign=true;	//��ͨ�������֤�ı�׼�ڵ��
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam=this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//
	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->xFlag.bFilterWorkWing=true;
	pParaAngle->desStart.cTouchLineType=3;
	pParaAngle->desStart.coordCtrlType=4;
	pParaAngle->desStart.cFaceOffsetDatumLine=0;
	pParaAngle->desStart.face_offset_norm.Set(0,0,1);
	pParaAngle->bUpdateStartPos=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLine=2;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.szXorCtrlWingOffset.Copy("0");
	pParaAngle->desStart.szYorOthrWingOffset.Copy("0");
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->ciAngleWorkWing='Y';	//����б��
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod=false;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine = 0xff;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->ciAngleWorkWing='X';	//����б��
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod= false;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine= 0xff;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����

	CStdTemplParaPlate* pParaPlate=this->listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode=1;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.ciProfileStyle=1;	//��������
	pParaPlate->design.ciDatumLenAxis='Y';
	pParaPlate->design.ciDatumAngleWing='X';
	pParaPlate->design.bend.widAngleBendBy=2;
	pParaPlate->design.bend.ciBendMode=2;
	pParaPlate->design.bend.normal.idBendPlaneByAngle=3;
	pParaPlate->design.bend.normal.cDatumWingXorY='Y';
	pParaPlate->connrods.Clear();
	//1.��������
	pParaPlate->connrods[0].sidRod=1;
	//pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=175;
	//pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=185;
	pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[0].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].ciBoltNCalMode=-1;	//˫����˨��
	pParaPlate->connrods[0].cnMinBoltN=6;
	pParaPlate->connrods[0].dualSideEdge=false;
	pParaPlate->connrods[0].ciConnectWing='X';
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);

	//2.���ˮƽ���
	//rodarr[1].xFlag.bFilterWorkWing=true;
	pParaPlate->connrods[1].sidRod=2;
	//pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=80;
	//pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=90;
	pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
	if (idStdComTempl == STDTEMPL_NODEPLATE_BP1)
		pParaPlate->connrods[1].InitTipInfo(0,0,1);	//5#��Ϊ���Ĳģ��������ٵ�ǰ2#��ĸ�ͷ
	else //if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_BP2)
		pParaPlate->connrods[1].InitTipInfo(0,0,1,5);//5#��Ϊ����б�ģ��������ٸ�ͷ
	pParaPlate->connrods[1].ciPlaneNo=1;
	pParaPlate->connrods[1].cnMinBoltN = 2;
	pParaPlate->connrods[1].cnMaxSingularLineBolts = 3;
	pParaPlate->connrods[1].dualSideEdge=false;
	pParaPlate->connrods[1].ciConnectWing='Y';
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows=0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.��������
	//InitSchemaRod(rodarr[2],3,-30,5,1,'Y','R');
	//rodarr[2].ciPlaneNo=2;
	//rodarr[2].xFlag.bFilterWorkWing=true;
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=-30;
	pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle= 5;
	pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[2].InitTipInfo(0,0,1);
	pParaPlate->connrods[2].ciPlaneNo=2;
	pParaPlate->connrods[2].ciBoltNCalMode=-1;	//˫����˨��
	pParaPlate->connrods[2].cnMinBoltN=6;
	pParaPlate->connrods[2].dualSideEdge=false;
	pParaPlate->connrods[2].ciConnectWing='Y';
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows= 0;
	pParaPlate->connrods[2].layoutbolts[0].blPreferBerSide=false;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.����б����
	//InitSchemaRod(rodarr[3],4,20,70,0,'Y','R');
	//rodarr[3].ciPlaneNo=2;
	pParaPlate->connrods[3].sidRod=4;
	pParaPlate->connrods[3].dynaRod.siRayMinDegAngle=20;
	pParaPlate->connrods[3].dynaRod.siRayMaxDegAngle=70;
	pParaPlate->connrods[3].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[3].InitTipInfo(0,0,3,2);;
	pParaPlate->connrods[3].ciPlaneNo=2;
	pParaPlate->connrods[3].cnMinBoltN=4;
	pParaPlate->connrods[3].dualSideEdge=false;
	pParaPlate->connrods[3].ciConnectWing='Y';
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows= 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.����б����
	//InitSchemaRod(rodarr[4],5,100,160,0,'X','R');
	pParaPlate->connrods[4].sidRod=5;
	//pParaPlate->connrods[4].dynaRod.siRayMinDegAngle=100;
	//pParaPlate->connrods[4].dynaRod.siRayMaxDegAngle=160;
	pParaPlate->connrods[4].ciNecessity=2;	//���п���
	pParaPlate->connrods[4].ciPlaneNo = 1;
	if (idStdComTempl == STDTEMPL_NODEPLATE_BP1)
	{
		pParaPlate->connrods[4].InitTipInfo(0,0,1,2);//5#��Ϊ���Ĳģ��������ٵ�ǰ2#��ĸ�ͷ
		pParaPlate->connrods[4].cnMinBoltN = 1;
	}
	else //if (pTemplSource->idStdComTempl == STDTEMPL_NODEPLATE_BP2)
	{
		pParaPlate->connrods[4].InitTipInfo(0,0,1);	//5#��Ϊ����б�ģ��������ٸ�ͷ
		pParaPlate->connrods[4].cnMinBoltN = 3;
	}
	pParaPlate->connrods[4].cnMaxSingularLineBolts = 3;
	pParaPlate->connrods[4].dualSideEdge=false;
	pParaPlate->connrods[4].ciConnectWing='X';
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows= 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}
void CStdComTempl::InitNodePlateTemp_BP3()
{
	name.Copy("BP3");//0x03034202,������Ϊһ�齻���
	szSketchImgFile="BP3.bmp";
	m_bEnableIntelliConnPointDesign=true;	//��ͨ�������֤�ı�׼�ڵ��
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam;
	pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//
	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->xFlag.bFilterWorkWing=true;
	pParaAngle->desStart.cTouchLineType=3;
	pParaAngle->desStart.coordCtrlType=4;
	pParaAngle->desStart.cFaceOffsetDatumLine=0;
	pParaAngle->desStart.face_offset_norm.Set(0,0,1);
	pParaAngle->bUpdateStartPos=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->ciAngleWorkWing='Y';	//����б��
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine = 0xff;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->ciAngleWorkWing='X';	//����б��
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod= false;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine= 0xff;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(6,CLS_LINEANGLE);
	pParaAngle->ciAngleWorkWing='Y';	//�����ϲ��м���б(��)�ĸ�
	pParaAngle->idStartNode=1;
	pParaAngle->m_bNecessaryRod= true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine= 0xff;//'Y';
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����

	CStdTemplParaPlate* pParaPlate=this->listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;	//��ʾ��ͨ���Ӱ�
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode=1;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.ciProfileStyle=1;	//��������
	pParaPlate->design.ciDatumLenAxis='Y';
	pParaPlate->design.ciDatumAngleWing='X';
	pParaPlate->design.bend.widAngleBendBy=2;
	pParaPlate->design.bend.ciBendMode=2;
	pParaPlate->design.bend.normal.idBendPlaneByAngle=3;
	pParaPlate->design.bend.normal.cDatumWingXorY='Y';
	pParaPlate->connrods.Clear();
	//1.��������
	pParaPlate->connrods[0].sidRod=1;
	//pParaPlate->connrods[0].dynaRod.siRayMinDegAngle=175;
	//pParaPlate->connrods[0].dynaRod.siRayMaxDegAngle=185;
	pParaPlate->connrods[0].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[0].ciCurrTipSideS0E1=0;
	pParaPlate->connrods[0].ciPlaneNo=1;
	pParaPlate->connrods[0].ciBoltNCalMode=-1;	//˫����˨��
	pParaPlate->connrods[0].cnMinBoltN=6;
	pParaPlate->connrods[0].dualSideEdge=false;
	pParaPlate->connrods[0].ciConnectWing='X';
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[0].layoutbolts[0].ciRows=2;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);

	//2.���ˮƽ���
	//rodarr[1].xFlag.bFilterWorkWing=true;
	pParaPlate->connrods[1].sidRod=2;
	//pParaPlate->connrods[1].dynaRod.siRayMinDegAngle=80;
	//pParaPlate->connrods[1].dynaRod.siRayMaxDegAngle=90;
	pParaPlate->connrods[1].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[1].InitTipInfo(0,0,1);	//5#��Ϊ���Ĳģ��������ٵ�ǰ2#��ĸ�ͷ
	pParaPlate->connrods[1].ciPlaneNo=1;
	pParaPlate->connrods[1].cnMinBoltN = 2;
	pParaPlate->connrods[1].cnMaxSingularLineBolts = 3;
	pParaPlate->connrods[1].dualSideEdge=false;
	pParaPlate->connrods[1].ciConnectWing='Y';
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[1].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[1].layoutbolts[0].ciRows=0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.��������
	//InitSchemaRod(rodarr[2],3,-30,5,1,'Y','R');
	//rodarr[2].ciPlaneNo=2;
	//rodarr[2].xFlag.bFilterWorkWing=true;
	pParaPlate->connrods[2].sidRod=3;
	pParaPlate->connrods[2].dynaRod.siRayMinDegAngle=-30;
	pParaPlate->connrods[2].dynaRod.siRayMaxDegAngle= 5;
	pParaPlate->connrods[2].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[2].InitTipInfo(0,0,1);
	pParaPlate->connrods[2].ciPlaneNo=2;
	pParaPlate->connrods[2].ciBoltNCalMode=-1;	//˫����˨��
	pParaPlate->connrods[2].cnMinBoltN=6;
	pParaPlate->connrods[2].dualSideEdge=false;
	pParaPlate->connrods[2].ciConnectWing='Y';
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[2].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[2].layoutbolts[0].ciRows= 0;
	pParaPlate->connrods[2].layoutbolts[0].blPreferBerSide=false;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.����б����
	//InitSchemaRod(rodarr[3],4,20,70,0,'Y','R');
	//rodarr[3].ciPlaneNo=2;
	pParaPlate->connrods[3].sidRod=4;
	pParaPlate->connrods[3].dynaRod.siRayMinDegAngle=20;
	pParaPlate->connrods[3].dynaRod.siRayMaxDegAngle=70;
	pParaPlate->connrods[3].ciNecessity=1;	//��Ҫ�˼�
	pParaPlate->connrods[3].InitTipInfo(0,0,3,2);
	pParaPlate->connrods[3].ciPlaneNo=2;
	pParaPlate->connrods[3].cnMinBoltN=4;
	pParaPlate->connrods[3].dualSideEdge=false;
	pParaPlate->connrods[3].ciConnectWing='Y';
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[3].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[3].layoutbolts[0].ciRows= 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.����б����
	//InitSchemaRod(rodarr[4],5,100,160,0,'X','R');
	pParaPlate->connrods[4].sidRod=5;
	//pParaPlate->connrods[4].dynaRod.siRayMinDegAngle=100;
	//pParaPlate->connrods[4].dynaRod.siRayMaxDegAngle=160;
	pParaPlate->connrods[4].ciNecessity=2;	//���п���
	pParaPlate->connrods[4].ciPlaneNo = 1;
	pParaPlate->connrods[4].InitTipInfo(0,0,1,2);//5#��Ϊ���Ĳģ��������ٵ�ǰ2#��ĸ�ͷ
	pParaPlate->connrods[4].cnMinBoltN = 1;
	pParaPlate->connrods[4].cnMaxSingularLineBolts = 3;
	pParaPlate->connrods[4].dualSideEdge=false;
	pParaPlate->connrods[4].ciConnectWing='X';
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[4].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[4].layoutbolts[0].ciRows= 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6.�����ϲ��м���б(��)�ĸ�
	//InitSchemaRod(rodarr[4],5,100,160,0,'X','R');
	pParaPlate->connrods[5].sidRod=6;
	pParaPlate->connrods[5].ciNecessity=1;
	pParaPlate->connrods[5].ciPlaneNo = 2;
	pParaPlate->connrods[5].InitTipInfo(0,0,1,3);//5#��Ϊ���Ĳģ��������ٵ�ǰ2#��ĸ�ͷ
	pParaPlate->connrods[5].cnMinBoltN = 2;
	pParaPlate->connrods[5].cnMaxSingularLineBolts = 3;
	pParaPlate->connrods[5].dualSideEdge=false;
	pParaPlate->connrods[5].ciConnectWing='X';
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[5].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[5].layoutbolts[0].cnCount=0;
	pParaPlate->connrods[5].layoutbolts[0].ciRows= 0;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[5].layoutbolts[0].ciDatumWing=0;//'Y';
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(6);
}
#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD4���ӵ�ģ�����
void CStdComTemplConnPoint::InitConnPointTempl_HD4()
{
	name = "HD-04";
	szSketchImgFile = "HD4.bmp";
	description="èͷ�����ڹյ㴦";
	widBaseRod=1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->ciBaseRodConnState=0;	//�����ƣ�һ����2.�νӶ�ĩ������
	xView.ciBaseAxisXMode=0;	//ʼ->�շ���
	xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=200;
	xView.siMaxBaseRodAngle=225;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//1.����б��
	InitSchemaRod(rodarr[0],1,0,10,1,'Y','R');
	//2.��б��
	InitSchemaRod(rodarr[1],2,50,85,1,'X','R');
	//3.���¸���
	InitSchemaRod(rodarr[2],3,85,125,2,'X','R');
	//3.ˮƽ���
	InitSchemaRod(rodarr[3],4,140,165,1,'X','R');
	//4.����б��
	InitSchemaRod(rodarr[4],5,230,250,1,'Y','R');
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD4);
	pMatchPlate->InitDefaultMatchRods(5);
	pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD4_2);
	pMatchPlate->InitDefaultMatchRods(5);
}

//HD4���Ӱ�ģ�����
void CStdComTempl::InitNodePlateTemp_HD4(bool blShrinkRodTrue2False1/*=true*/)
{
	short idBaseOddRefRod = blShrinkRodTrue2False1 ? 1 : 2;
	if (blShrinkRodTrue2False1)
		szSketchImgFile = "PHD4.bmp";
	else
		szSketchImgFile = "PHD4-2.bmp";
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��׼�ڵ�
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//1.����б��
	CStdTemplParaAngle* pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//2.������
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//3.����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//4.ˮƽ���
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//5.����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Add(1);
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.����б��
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,3,false);
	if(blShrinkRodTrue2False1)
		pParaPlate->connrods[0].InitTipInfo(0,2);
	else
		pParaPlate->connrods[0].InitTipInfo(0,0,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.������
	pParaPlate->connrods[1].InitBasicInfo(2,true,1,3,false);
	if (blShrinkRodTrue2False1)
		pParaPlate->connrods[1].InitTipInfo(0,0,1);
	else
		pParaPlate->connrods[1].InitTipInfo(0,2);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.����б��
	pParaPlate->connrods[2].InitBasicInfo(3,false,1,1,false);
	pParaPlate->connrods[2].InitTipInfo(0,0,2,4);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.ˮƽ���
	pParaPlate->connrods[3].InitBasicInfo(4,true,1,3,false);
	pParaPlate->connrods[3].InitTipInfo(0,0,idBaseOddRefRod);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.����б��
	pParaPlate->connrods[4].InitBasicInfo(5,true,1,3,false);
	pParaPlate->connrods[4].InitTipInfo(1,0,idBaseOddRefRod,4);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}
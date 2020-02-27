#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD6���ӵ�ģ�����
void CStdComTemplConnPoint::InitConnPointTempl_HD6()
{
	name = "HD6";
	szSketchImgFile = "HD6.bmp";
	description = "èͷ������յ㴦(��)";
	widBaseRod=1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->xView.ciViewPlaneNormal='Y';
	xView.siMinBaseRodAngle=30;
	xView.siMaxBaseRodAngle=50;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//1-�ᴩ����
	InitSchemaRod(rodarr[0],1,0,1,1,'X','A');
	rodarr[0].xFlag.bFilterWorkWing=true;
	//2-��֧��б��
	InitSchemaRod(rodarr[1],2,30,75,1,'X','R');
	rodarr[1].xFlag.bFilterWorkWing=true;
	//3-���ϸ��ĸ�
	InitSchemaRod(rodarr[2],3,80,125,1,'Y','R');
	//4-���ˮƽ��
	InitSchemaRod(rodarr[3],4,126,150,2,'Y','R');
	//5-�Ҳ�ˮƽ��
	InitSchemaRod(rodarr[4],5,280,335,1,'Y','R');
	rodarr[4].xFlag.bFilterWorkWing=true;
	rodarr[4].ciPlaneNo=2;

	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD6);
	pMatchPlate->InitDefaultMatchRods(5);
}

//HD6���Ӱ�ģ�����
void CStdComTempl::InitNodePlateTemp_HD6()
{
	szSketchImgFile = "PHD6.bmp";
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
	//1-���Ϲᴩ����
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idMidNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	//2-��֧��б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//3-���ϸ��ĸ�
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//4-���ˮƽ��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//5-�Ҳ�ˮƽ��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 1;
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 5;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 1;
	pParaPlate->connrods.Clear();
	//���Ϲᴩ����
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,6,true);
	pParaPlate->connrods[0].InitTipInfo(2,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//����б��1
	pParaPlate->connrods[1].InitBasicInfo(2,true,1,1,false);
	pParaPlate->connrods[1].InitTipInfo(1,0,1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//����б��2
	pParaPlate->connrods[2].InitBasicInfo(3,true,1,1,false);
	pParaPlate->connrods[2].InitTipInfo(0,0,1,2);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//���ˮƽ��
	pParaPlate->connrods[3].InitBasicInfo(4,false,1,3,false);
	pParaPlate->connrods[3].InitTipInfo(0,0,1);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//�Ҳ�ˮƽ��
	pParaPlate->connrods[4].InitBasicInfo(5,true,2,3,false);
	pParaPlate->connrods[4].InitTipInfo(1,0,1);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}


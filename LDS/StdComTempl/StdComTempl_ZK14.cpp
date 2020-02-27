#include <stdafx.h>
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"

//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);

//ZK14���ӵ�ģ��-�Ʊ�èͷ��������K�ڵ㴦�����������ģ�
void CStdComTemplConnPoint::InitConnPointTempl_ZK14_K1()
{
	name = "ZK14_K1";
	szSketchImgFile = "ZK14_K1.bmp";
	description = "�Ʊ�èͷ��������K�ڵ㴦(����)";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->xView.ciViewPlaneNormal = 'Y';
	xView.ciBaseAxisXMode=2;	//���߷�����
	xView.siMinBaseRodAngle = 60;
	xView.siMaxBaseRodAngle = 90;
	this->ciBaseRodConnState=4;	//��׼�˼�Ϊ���߽ضϵ�
	this->m_bEnableIntelliConnPointDesign=true;
	this->xSideConnPoint.idSideStdConnPoint=STDTEMPL_CONNPOINT_ZK14_K1_SIDE;
	this->xSideConnPoint.idSideStdNodePlate=STDTEMPL_NODEPLATE_ZK14_K1_SIDE;
	this->xSideConnPoint.xarrShareRods.Append(MAPPING_ROD(1,1));
	this->xSideConnPoint.xarrShareRods.Append(MAPPING_ROD(4,5));
	this->xSideConnPoint.xarrShareRods.Append(MAPPING_ROD(5,6));
	rodarr.ZeroPresetMemory();
	//1.����������
	InitSchemaRod(rodarr[0], 1, 175, 185, 1, 3, 'R');
	//2.����б��
	InitSchemaRod(rodarr[1], 2, 135, 165, 1, 2, 'R');
	//3.����б��
	InitSchemaRod(rodarr[2], 3,  15,  45, 1, 3, 'R');
	//4.����������
	InitSchemaRod(rodarr[3], 4, -5, 5, 1, 2, 'R');
	rodarr.ReSize(4);
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK14_K1);
	pMatchPlate->xarrReflectMapRods.Append(MAPPING_ROD(5,5));
	pMatchPlate->InitDefaultMatchRods(4);
}
void CStdComTemplConnPoint::InitConnPointTempl_ZK14_K1_SIDE()
{
	name = "ZK14_K1_SIDE";
	szSketchImgFile = "ZK14_K1_SIDE.bmp";
	description = "�Ʊ���������K�ڵ㴦(����)";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->xView.ciViewPlaneNormal = 'X';
	xView.ciBaseAxisXMode=1;	//���߷�����
	xView.siMinBaseRodAngle = 250;
	xView.siMaxBaseRodAngle = 270;
	this->ciBaseRodConnState=4;	//��׼�˼�Ϊ���߽ضϵ�
	this->m_bEnableIntelliConnPointDesign=false;
	rodarr.ZeroPresetMemory();
	//1.����������
	InitSchemaRod(rodarr[0], 1, -1, 1, 1, 3, 'R');
	//2.����б��(������Ҫ�Ǳ���˼�)
	InitSchemaRod(rodarr[1], 2, 20, 85, 2, 2, 'R');
	//3.ˮƽ���
	InitSchemaRod(rodarr[2], 3, 90,120, 1, 3, 'R');
	//4.����б��(������Ҫ�Ǳ���˼�)
	InitSchemaRod(rodarr[3], 4, 125, 170, 2, 2, 'R');
	//5.����������
	InitSchemaRod(rodarr[4], 5, 175, 185, 1, 2, 'R');
	//6.���������������������(������Ҫ�Ǳ���˼�)
	InitSchemaRod(rodarr[5], 6,135,165, 2, 3, 'R');
	rodarr.ReSize(6);
	CMatchStdPlate* pMatchPlate;
	pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK14_K1_SIDE);
	pMatchPlate->InitDefaultMatchRods(6);
	pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK14_K3_SIDE);
	pMatchPlate->InitDefaultMatchRods(5);
}
//ZK14_K1���ӵ�ģ��-�Ʊ�èͷ��������K�ڵ㴦
void CStdComTemplConnPoint::InitConnPointTempl_ZK14_K2()
{
	name = "ZK14_K2";
	szSketchImgFile = "ZK14_K2.bmp";
	description = "�Ʊ�èͷ��������K�ڵ㴦(���ض�)";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 60;
	xView.siMaxBaseRodAngle = 90;
	rodarr.ZeroPresetMemory();
	//1.�ᴩ����
	InitSchemaRod(rodarr[0], 1, 0, 1, 1, 2, 'A');
	//2.����б��
	InitSchemaRod(rodarr[1], 2, 15, 45, 1, 3, 'R');
	//3.����б��
	InitSchemaRod(rodarr[2], 3, 135, 165, 1, 2, 'R');
	rodarr.ReSize(3);
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK14_K2);
	pMatchPlate->InitDefaultMatchRods(3);
}
void CStdComTemplConnPoint::InitConnPointTempl_ZK14_K3()
{
	name = "ZK14_K3";
	szSketchImgFile = "ZK14_K3.bmp";
	description = "�Ʊ���������K�ڵ㴦(����)";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;		//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->xView.ciViewPlaneNormal = 'Y';
	xView.ciBaseAxisXMode=2;	//���߷�����
	xView.siMinBaseRodAngle = 60;
	xView.siMaxBaseRodAngle = 90;
	this->ciBaseRodConnState=3;	//�νӶ�ĩ������
	this->m_bEnableIntelliConnPointDesign=true;
	this->xSideConnPoint.idSideStdConnPoint=STDTEMPL_CONNPOINT_ZK14_K3_SIDE;
	this->xSideConnPoint.idSideStdNodePlate=STDTEMPL_NODEPLATE_ZK14_K3_SIDE;
	this->xSideConnPoint.xarrShareRods.Append(MAPPING_ROD(1,1));
	this->xSideConnPoint.xarrShareRods.Append(MAPPING_ROD(4,5));
	rodarr.ZeroPresetMemory();
	//1.����������
	InitSchemaRod(rodarr[0], 1, 175, 185, 1, 3, 'R');
	//2.����б��
	InitSchemaRod(rodarr[1], 2, 135, 165, 1, 2, 'R');
	//3.����б��
	InitSchemaRod(rodarr[2], 3,  15,  45, 1, 3, 'R');
	//4.����������
	InitSchemaRod(rodarr[3], 4, 2, 15, 1, 2, 'R');
	rodarr.ReSize(4);
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK14_K3);
	pMatchPlate->InitDefaultMatchRods(4);
}
void CStdComTemplConnPoint::InitConnPointTempl_ZK14_K3_SIDE()
{
	name = "ZK14_K3_SIDE";
	szSketchImgFile = "ZK14_K3_SIDE.bmp";
	description = "�Ʊ���������K�ڵ㴦(����)";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->xView.ciViewPlaneNormal = 'X';
	this->xView.ciBaseAxisXMode=1;	//���߷�����
	this->xView.siMinBaseRodAngle = 250;
	this->xView.siMaxBaseRodAngle = 270;
	this->ciBaseRodConnState=3;	//�νӶ�ĩ������
	this->m_bEnableIntelliConnPointDesign=true;
	rodarr.ZeroPresetMemory();
	//1.����������
	InitSchemaRod(rodarr[0], 1, -1, 1, 1, 3, 'R');
	//2.����б��(������Ҫ�Ǳ���˼�)
	InitSchemaRod(rodarr[1], 2, 20, 85, 2, 2, 'R');
	//3.ˮƽ���
	InitSchemaRod(rodarr[2], 3, 90,120, 1, 3, 'R');
	//4.����б��(������Ҫ�Ǳ���˼�)
	InitSchemaRod(rodarr[3], 4, 125, 170, 2, 2, 'R');
	//5.����������
	InitSchemaRod(rodarr[4], 5, 175, 185, 1, 2, 'R');
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_ZK14_K3_SIDE);
	pMatchPlate->InitDefaultMatchRods(5);
}
//���ð��Ǹ�����
void CStdComTempl::InitNodePlateTemp_ZK14_K1()
{
	szSketchImgFile = "ZK14_K1.bmp";
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	m_cbDisableConnPlaneFlag=0x80;	//��Ƴɹ�ͬ��ֹͬһ�ڵ㴦�Ľ�ͷ��� wjh-2019.10.6
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��׼�ڵ�
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//������
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'D';
	pParaAngle->m_bNecessaryRod = true;
	//����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//������
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'D';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//�����Ǹ�
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5, CLS_LINEANGLE);
	pParaAngle->attachangle.idBaseAngle = 1;
	pParaAngle->attachangle.idBaseNode = 1;
	pParaAngle->attachangle.idEndTipBaseAngle=4;
	pParaAngle->attachangle.ciDatumAngleWing = 'O';	//�����
	pParaAngle->attachangle.ciNormalOffset = 0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	pParaAngle->attachangle.ciAngleLayout = 4;	//���
	pParaAngle->attachangle.ciLenDirection = 'Z';	//ʼ->����'X'�����
	pParaAngle->attachangle.siLenStartTipPos=pParaAngle->attachangle.siLenEndTipPos=100;
	pParaAngle->m_sGuige.Copy("L300x24");
	pParaAngle->m_cbNeedUpdatePosition = 0;
	pParaAngle->m_bNeedUpdateNormal = 0;
	pParaAngle->blShortAngle = true;
	pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
	pParaAngle->ciAngleWorkWing = 'D';
	//���ӵ�ṹ���
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_VIRTUAL_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.������
	pParaPlate->connrods[0].ciBoltNCalMode=0;//blCalBoltNByRayRodsOnBaseRod=false;
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 4, false);
	pParaPlate->connrods[0].InitTipInfo(0, 0, 2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].idDirectConnectToAngle1=5;	//��˨ͬʱ��������5#�����
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciDatumWing='X';	//���Ȳ���֫
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	/*pParaPlate->connrods[0].layoutbolts[1].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[1].idDirectConnectToAngle1=5;	//��˨ͬʱ��������5#�����
	pParaPlate->connrods[0].layoutbolts[1].ciLayoutStyle = 0;
	pParaPlate->connrods[0].layoutbolts[1].ciDatumWing='Y';
	StrCopy(pParaPlate->connrods[0].layoutbolts[1].szLenOffsetOf1stBolt,"LE+LS/2",8);
	pParaPlate->connrods[0].layoutbolts[1].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;*/
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.����б��
	pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 2, false);
	pParaPlate->connrods[1].InitTipInfo(0, 3, 5);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].idDirectConnectToAngle1=5;	//��˨ͬʱ��������5#�����
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.����б��
	pParaPlate->connrods[2].InitBasicInfo(3, true, 1, 2, false);
	pParaPlate->connrods[2].InitTipInfo(1, 0, 2, 5);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].idDirectConnectToAngle1=5;	//��˨ͬʱ��������5#�����
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.������
	pParaPlate->connrods[3].InitBasicInfo(4, true, 1, 4, false);
	pParaPlate->connrods[3].InitTipInfo(1, 0, 3);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].idDirectConnectToAngle1=5;	//��˨ͬʱ��������5#�����
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 2;
	pParaPlate->connrods[3].layoutbolts[0].blPreferBerSide=false;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(4);
}
//���ð��Ǹ�����
void CStdComTempl::InitNodePlateTemp_ZK14_K1_SIDE()
{
	szSketchImgFile = "ZK14_K1_SIDE.bmp";
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	m_cbDisableConnPlaneFlag=0x80;	//��Ƴɹ�ͬ��ֹͬһ�ڵ㴦�Ľ�ͷ��� wjh-2019.10.6
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��׼�ڵ�
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//1.������
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	//2.����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->desStart.siDatumLenOffset=80;	//�����һֵ���Ժ���Խ����Ż�����
	pParaAngle->desStart.szXorCtrlWingOffset="[=-R6#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";
	pParaAngle->bUpdateStartPos = true;
	//3.ˮƽ���
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-R6#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";
	pParaAngle->bUpdateStartPos = true;
	//4.����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->desStart.uidTouchLineRod=5;		//������������Ϊ��������˼�
	pParaAngle->desStart.siDatumLenOffset=-80;	//�����һֵ���Ժ���Խ����Ż�����
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-R6#T";
	pParaAngle->bUpdateStartPos = true;
	//5.����������
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'D';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = false;
	//���������������������
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(6, CLS_LINEANGLE);
	pParaAngle->blShortAngle = true;
	pParaAngle->ciAngleWorkWing = 'D';
	//���ӵ�ṹ���
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_VIRTUAL_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.������
	pParaPlate->connrods[0].ciBoltNCalMode=0;//blCalBoltNByRayRodsOnBaseRod=false;
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 4, false);
	pParaPlate->connrods[0].InitTipInfo(0, -1);
	//pParaPlate->connrods[0].layoutbolts.ReSize(0);	//�������������в���˨���˴����ٵ�������˨
	//2.����б��
	pParaPlate->connrods[1].InitBasicInfo(2, false, 1, 1, false);
	pParaPlate->connrods[1].InitTipInfo(0, 1, 6);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_AXISINTERS;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widCrossAngle=1;
	pParaPlate->connrods[1].layoutbolts[0].idDirectConnectToAngle1=6;	//��˨ͬʱ��������6#�����
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.ˮƽ���
	pParaPlate->connrods[2].InitBasicInfo(3, true, 1, 1, false);
	pParaPlate->connrods[2].InitTipInfo(0, 1, 6);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_AXISINTERS;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widCrossAngle=1;
	pParaPlate->connrods[2].layoutbolts[0].idDirectConnectToAngle1=6;	//��˨ͬʱ��������6#�����
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.����б��
	pParaPlate->connrods[3].InitBasicInfo(4, false, 1, 1, false);
	pParaPlate->connrods[3].InitTipInfo(0, 1, 6);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_AXISINTERS;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widCrossAngle=2;
	pParaPlate->connrods[3].layoutbolts[0].idDirectConnectToAngle1=6;	//��˨ͬʱ��������6#�����
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.������
	pParaPlate->connrods[4].InitBasicInfo(5, true, 1, 0, false);
	pParaPlate->connrods[4].InitTipInfo(1, -1);
	pParaPlate->connrods[4].layoutbolts.ReSize(0);	//�������������в���˨���˴����ٵ�������˨
	pParaPlate->connrods.ReSize(5);
}
//ZK14_K2���Ӱ�ģ�����
void CStdComTempl::InitNodePlateTemp_ZK14_K2()
{
	szSketchImgFile = "ZK14_K1.bmp";
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��׼�ڵ�
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//�ᴩ����
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idMidNode = 1;
	pParaAngle->m_bNecessaryRod = true;
	//����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
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
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.�ᴩ����
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 8, true);
	pParaPlate->connrods[0].InitTipInfo(2, 2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.��������
	pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 2, false);
	pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.����б��
	pParaPlate->connrods[2].InitBasicInfo(3, true, 1, 2, false);
	pParaPlate->connrods[2].InitTipInfo(1, 0, 1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(3);
}

//ZK14���Ӱ�ģ�����(���²��øְ������滥��)
void CStdComTempl::InitNodePlateTemp_ZK14_K3()
{
	szSketchImgFile = "ZK14_K3.bmp";
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��׼�ڵ�
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//������
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	//����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=0;
	pParaAngle->desStart.uidTouchLineRod=4;
	pParaAngle->bUpdateStartPos = true;
	//������
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->desEnd.cTouchLineType=0;
	pParaAngle->desEnd.cTouchLine=0;
	pParaAngle->desEnd.uidTouchLineRod=1;
	pParaAngle->desEnd.szXorCtrlWingOffset="0";
	pParaAngle->desEnd.szYorOthrWingOffset="g";
	pParaAngle->bUpdateEndPos=true;
	//��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//��������
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 0;
	pParaPlate->connrods.Clear();
	//1.������
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 5, true);
	pParaPlate->connrods[0].InitTipInfo(0, 2);
	pParaPlate->connrods[0].ciBoltNCalMode=0;//blCalBoltNByRayRodsOnBaseRod=false;
	pParaPlate->connrods[0].cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.����б��
	pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 2, false);
	pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.����б��
	pParaPlate->connrods[2].InitBasicInfo(3, true, 1, 2, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 4);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.������
	pParaPlate->connrods[3].InitBasicInfo(4, true, 1, 5, true);
	pParaPlate->connrods[3].InitTipInfo(1, 0, 1);
	pParaPlate->connrods[3].cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(4);
}
void CStdComTempl::InitNodePlateTemp_ZK14_K3_SIDE()
{
	szSketchImgFile = "ZK14_K3_SIDE.bmp";
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��׼�ڵ�
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//1.������
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	//2.����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->desStart.siDatumLenOffset=80;	//�����һֵ���Ժ���Խ����Ż�����
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";
	pParaAngle->bUpdateStartPos = true;
	//3.ˮƽ���
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="-10";
	pParaAngle->bUpdateStartPos = true;
	//4.����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->desEnd.uidTouchLineRod=5;		//������������Ϊ��������˼�
	pParaAngle->desEnd.siDatumLenOffset=-80;	//�����һֵ���Ժ���Խ����Ż�����
	pParaAngle->desEnd.szXorCtrlWingOffset="g";
	pParaAngle->desEnd.szYorOthrWingOffset="[=-P1#T";
	pParaAngle->bUpdateEndPos = true;
	//5.����������
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = false;
	//���ӵ�ṹ���
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'Y';
	pParaPlate->design.bend.widAngleBendBy = 5;	//�˴�Ϊ0�ᵼ��BuildModel()ʱ������������� wjh-2019.10.31
	pParaPlate->design.bend.biSelfCalBendline = TRUE;	//������ͨ��������ƽ���󽻻�ȡ
	pParaPlate->design.bend.ciBendMode=2;	//ָ���Ǹ�֫������Ϊ�����淨��
	pParaPlate->design.bend.normal.idBendPlaneByAngle=5;
	pParaPlate->design.bend.normal.cDatumWingXorY = 'Y';
	pParaPlate->connrods.Clear();
	//1.������
	pParaPlate->connrods[0].ciBoltNCalMode=0;//blCalBoltNByRayRodsOnBaseRod=false;
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 5, false);
	pParaPlate->connrods[0].InitTipInfo(0, -1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].blPreferBerSide=false;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.����б��
	pParaPlate->connrods[1].InitBasicInfo(2, false, 1, 2, false);
	pParaPlate->connrods[1].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].basepoint.widBaseNodeOrBolt =1;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.ˮƽ���
	pParaPlate->connrods[2].InitBasicInfo(3, true, 1, 3, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1, 5);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].basepoint.widBaseNodeOrBolt =1;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.����б��
	pParaPlate->connrods[3].InitBasicInfo(4, false, 2, 2, false);
	pParaPlate->connrods[3].InitTipInfo(1, 0, 5);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].basepoint.widBaseNodeOrBolt =1;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.������
	pParaPlate->connrods[4].InitBasicInfo(5, true, 2, 5,false);
	pParaPlate->connrods[4].InitTipInfo(1, -1);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}

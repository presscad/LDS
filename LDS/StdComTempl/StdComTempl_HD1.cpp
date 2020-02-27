#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
#include "lds_part.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD1���ӵ�ģ�����
void CStdComTemplConnPoint::InitConnPointTempl_HD1()
{	//02010002
	name="HD-01";	//"HD1"
	szSketchImgFile = "HD1.bmp";
	description="ˮƽ�ᵣ�����֧���ڲ����ӵ�";	//"�Ʊ�èͷ�ᵣ�����֧���ڲ����Ӵ�";
	widBaseRod=1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod=1;		//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 0;
	xView.siMaxBaseRodAngle = 20;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//1.ˮƽ���
	InitSchemaRod(rodarr[0],1,0,1,1,3,'A');
	//2.����б��
	InitSchemaRod(rodarr[1],2,30,65,1,3,'R');
	//3.����б��
	InitSchemaRod(rodarr[2],3,210,245,1,1,'R');
	//4.������
	InitSchemaRod(rodarr[3],4,265,275,2,2,'R');
	//5.����б��
	InitSchemaRod(rodarr[4],5,290,330,1,3,'R');
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD1);
	pMatchPlate->InitDefaultMatchRods(5);
}
void CStdComTemplConnPoint::InitConnPointTempl_HD1A()
{	//02010002
	name = "HD-01A";	//"HD1"
	szSketchImgFile = "HD1A.bmp";
	description = "ˮƽ�ᵣ(���)�����֧���ڲ����ӵ�";	//"�Ʊ�èͷ�ᵣ�����֧���ڲ����Ӵ�";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;		//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 0;
	xView.siMaxBaseRodAngle = 20;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//1.��ˮƽ���
	InitSchemaRod(rodarr[0], 1, 0, 1, 1, 'y', 'R');
	//2.����б��
	InitSchemaRod(rodarr[1], 2, 30, 65, 1, 'X', 'R');
	//3.������
	InitSchemaRod(rodarr[2], 3, 85, 95, 2, 'y', 'R');
	//4.����б��
	InitSchemaRod(rodarr[3], 4, 110, 150, 1, 'x', 'R');
	//5.��ˮƽ���
	InitSchemaRod(rodarr[4], 5, 180, 185, 1, 'y', 'R');
	//6.����б��
	InitSchemaRod(rodarr[5], 6, 210, 245, 1, 'y', 'R');
	rodarr.ReSize(6);
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD1A);
	pMatchPlate->InitDefaultMatchRods(6);
}
//HD1���Ӱ�ģ�����
void CStdComTempl::InitNodePlateTemp_HD1()
{
	szSketchImgFile = "PHD1.bmp";
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��׼�ڵ�
	CStdTemplParaNode* pParaNode=this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//1-ˮƽ����
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->ciAngleWorkWing='Y';
	pParaAngle->m_bNecessaryRod=true;
	//2-����б��
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	//3-����б��
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//4.XIA����
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=false;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//5.����б��
	pParaAngle=(CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idStartNode=1;
	pParaAngle->ciAngleWorkWing='X';
	pParaAngle->m_bNecessaryRod=true;
	pParaAngle->bUpdateStartPos=true;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset="g";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//��ʾ��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate=this->listParaPlates.Append();
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode=1;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.ciProfileStyle=1;	//��������
	pParaPlate->design.ciDatumAngleWing='X';
	pParaPlate->design.bend.widAngleBendBy = 1;	//�����������˼�
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 2;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 1;
	pParaPlate->connrods.Clear();
	//1.ˮƽ���
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,8,true);
	pParaPlate->connrods[0].InitTipInfo(2,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=1;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.����б��
	pParaPlate->connrods[1].InitBasicInfo(2,true,2,3,false);
	pParaPlate->connrods[1].InitTipInfo(0,0,1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.����б��
	pParaPlate->connrods[2].InitBasicInfo(3,true,1,3,false);
	pParaPlate->connrods[2].InitTipInfo(0,0,1,4);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.XIA����
	pParaPlate->connrods[3].InitBasicInfo(4,false,1,2,false);
	pParaPlate->connrods[3].InitTipInfo(0,0,1);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.����б��
	pParaPlate->connrods[4].InitBasicInfo(5,true,1,3,false);
	pParaPlate->connrods[4].InitTipInfo(0,0,1,4);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle=0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}
void CStdComTempl::InitNodePlateTemp_HD1A()
{
	szSketchImgFile = "PHD1A.bmp";
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	m_cbDisableConnPlaneFlag = 0x80;	//��Ƴɹ�ͬ��ֹͬһ�ڵ㴦�Ľ�ͷ��� wjh-2019.10.6
	//�ڵ㹹���׼װ������ϵ����
	STDTEMPL_PARAM_ITEM* pParam = this->listParams.Add(KEY4C("acs"));
	pParam->value.ciValType = EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType = EXPRESSION_VALUE::OBJTYPE_ACS3D;
	pParam->value.objtype.wiPropValue = 0;
	strcpy(pParam->exprstr, "CrossRodAcs(1,1,2)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	//��׼�ڵ�
	CStdTemplParaNode* pParaNode = this->listParaNodes.Add(1);
	pParaNode->m_bNecessaryNode = true;
	//1-��ˮƽ����
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->m_bNecessaryRod = true;
	//2-����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType=0;
	pParaAngle->desStart.cTouchLine=1;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//3-XIA����
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->desStart.uidTouchLineRod = 1;
	pParaAngle->bUpdateStartPos = true;
	//4.����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4, CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'x';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->desStart.uidTouchLineRod = 1;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "[=-P1#T";
	pParaAngle->desStart.szYorOthrWingOffset = "g";
	//5.��ˮƽ���
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->desEnd.uidTouchLineRod = 1;
	pParaAngle->bUpdateEndPos = false;
	//����б��
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(6, CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'y';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->desEnd.uidTouchLineRod = 1;
	pParaAngle->bUpdateEndPos = true;
	//�����Ǹ�
	//pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(7, CLS_LINEANGLE);
	//pParaAngle->attachangle.idBaseAngle = 1;
	//pParaAngle->attachangle.idBaseNode = 1;
	//pParaAngle->attachangle.idEndTipBaseAngle = 5;
	//pParaAngle->attachangle.ciDatumAngleWing = 'I';	//�����
	//pParaAngle->attachangle.ciNormalOffset = 0;	//��ʾ�Զ��жϣ�ʵ����Ҫ����ƫ��һ�����Ӱ��
	//pParaAngle->attachangle.ciLenDirection = 'X';	//ʼ->����'X'�����
	//pParaAngle->attachangle.ciAssistWing = 'Y';
	//pParaAngle->attachangle.siLenStartTipPos = pParaAngle->attachangle.siLenEndTipPos = 100;
	//pParaAngle->m_sGuige.Copy("L110x10");
	//pParaAngle->m_cbNeedUpdatePosition = 1;
	//pParaAngle->m_bNeedUpdateNormal = 0;
	//pParaAngle->blShortAngle = true;
	//pParaAngle->wiStartOddCalStyle = pParaAngle->wiEndOddCalStyle = 1;	//������˨λ�ü���
	//pParaAngle->ciAngleWorkWing = 'y';
	//��ʾ��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;	//��������
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 1;	//�����������˼�
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 6;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 1;
	pParaPlate->connrods.Clear();
	//1.ˮƽ���
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 4, false);
	pParaPlate->connrods[0].InitTipSpecOddment(0,-5);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 0;
	pParaPlate->connrods[0].layoutbolts[0].idDirectConnectToAngle1 = 7;	//��˨ͬʱ��������7#�ڰ���
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 2;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.����б��
	pParaPlate->connrods[1].InitBasicInfo(2, true, 1, 3, false);
	pParaPlate->connrods[1].InitTipInfo(0, 0, 1, 3);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.XIA����
	pParaPlate->connrods[2].InitBasicInfo(3, false, 1, 2, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.����б��
	pParaPlate->connrods[3].InitBasicInfo(4, true, 1, 3, false);
	pParaPlate->connrods[3].InitTipInfo(0, 0, 3, 5);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.��ˮƽ���
	pParaPlate->connrods[4].InitBasicInfo(5, true, 1, 4, false);
	pParaPlate->connrods[4].InitTipSpecOddment(1,-5);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[4].layoutbolts[0].idDirectConnectToAngle1 = 7;	//��˨ͬʱ��������7#�ڰ���
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 2;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6.����б��
	pParaPlate->connrods[5].InitBasicInfo(6, true, 2, 3, false);
	pParaPlate->connrods[5].InitTipInfo(1, 0, 1, 5);
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(6);
}
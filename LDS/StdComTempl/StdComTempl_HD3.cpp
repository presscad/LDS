#include "stdafx.h"
#include "..\LDS.h"
#include "IStdComTempl.h"
#include "NodeStdComTempl.h"
//
void InitSchemaRod(SCHEMA_ROD &rod, WORD idRod, short siMinDegAng, short siMaxDegAng, char ciNecessity, char ciAngleLayout, char ciRodType, bool dualSideEdge = false);
//HD3���ӵ�ģ�����
void CStdComTemplConnPoint::InitConnPointTempl_HD3()
{
	name = "HD-03";
	szSketchImgFile = "HD3.bmp";
	description="�Ʊ��������������ᵣ�����ӵ�";	//"�Ʊ�èͷ��������ᵣ���ӵ�";
	widBaseRod=1;	//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->cnConnectPlane=3;
	//this->ciBaseRodConnState=4;	//3.�νӶνضϵ�����;4.���߽ضϵ�����,������������� ���ݲ���˹������� wjh-2019.9.15
	this->ciBaseRodSearchMode=1;//�Զ����ݹ���ƽ����ͼ������λ��׼�˼�
	xView.ciBaseAxisXMode=1;	//���߷���
	xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 175;
	xView.siMaxBaseRodAngle = 185;
	rodarr.ZeroPresetMemory();
	m_bEnableIntelliConnPointDesign = true;			//��ͨ�������֤�ı�׼�ڵ��
	//1-���ˮƽ��
	InitSchemaRod(rodarr[0],1,0,1,1,'x','R');
	//2-ˮƽ���·����������ĸ�
	InitSchemaRod(rodarr[1],2,45,88,1,3,'R');
	//3-��������б���ĸ�
	InitSchemaRod(rodarr[2],3,20,40,2,'X','R');
	//4-�Ҳ�ˮƽ��
	InitSchemaRod(rodarr[3],4,175,185,1,0,'R');
	//5-����б��
	InitSchemaRod(rodarr[4],5,200,250,2,3,'R');
	//6-������
	InitSchemaRod(rodarr[5],6,260,280,1,2,'R');
	//7-����б��
	InitSchemaRod(rodarr[6],7,300,330,2,2,'R');
	rodarr.ReSize(7);
	CMatchStdPlate* pMatchPlate=this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD3);
	pMatchPlate->InitDefaultMatchRods(7);
	//pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD3_2);
	//pMatchPlate->InitDefaultMatchRods(6);
}

void CStdComTemplConnPoint::InitConnPointTempl_HD3_G5()
{
	name = "HD-03-02";
	szSketchImgFile = "HD3_G5.bmp";
	description = "�Ʊ��������������ᵣ�����ӵ�(ˮƽ�˼��Ƕ�)";
	widBaseRod = 1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	widParentRod = 1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	this->ciBaseRodConnState=1;	//��׼�˼��м�����
	xView.ciBaseAxisXMode=0;	//ʼ->�շ���
	xView.ciViewPlaneNormal = 'Y';
	xView.siMinBaseRodAngle = 0;
	xView.siMaxBaseRodAngle = 20;
	rodarr.ZeroPresetMemory();
	//1-ˮƽ�ᴩ����
	InitSchemaRod(rodarr[0], 1, 0, 1, 1, 2, 'A');
	//2-���������
	InitSchemaRod(rodarr[1], 2, 40, 88, 1, 3, 'R');
	//3-����б��
	InitSchemaRod(rodarr[2], 3, 200, 250, 2, 3, 'R');
	//4-������
	InitSchemaRod(rodarr[3], 4, 260, 280, 1, 2, 'R');
	//5-����б��
	InitSchemaRod(rodarr[4], 5, 300, 330, 2, 2, 'R');
	rodarr.ReSize(5);
	CMatchStdPlate* pMatchPlate = this->hashMaybeTmplPlates.Add(STDTEMPL_NODEPLATE_HD3_G5);
	pMatchPlate->InitDefaultMatchRods(5);
}

//HD3���Ӱ�ģ�����
void CStdComTempl::InitNodePlateTemp_HD3()
{
	szSketchImgFile = "PHD3.bmp";
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
	//1-���ˮƽ��(ʼ��)
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//2-���������(ʼ��)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType=1;
	pParaAngle->desStart.cTouchLine=0;
	pParaAngle->desStart.uidBendByRod=1;
	//3-��������б���ĸ�
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType=1;
	pParaAngle->desStart.cTouchLine=0;
	pParaAngle->desStart.uidBendByRod=1;
	pParaAngle->desStart.szXorCtrlWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	pParaAngle->desStart.szYorOthrWingOffset="g";
	//4-�Ҳ�ˮƽ��(�ն�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	pParaAngle->desStart.cTouchLineType=1;
	pParaAngle->desStart.cTouchLine=0;
	pParaAngle->desStart.uidBendByRod=6;
	//5-����б��(ʼ��)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.cTouchLineType=1;
	pParaAngle->desStart.cTouchLine=0;
	pParaAngle->desStart.uidBendByRod=6;
	pParaAngle->desStart.szXorCtrlWingOffset="g";
	pParaAngle->desStart.szYorOthrWingOffset="[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//6-������(�ն�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(6,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//7-����б��(�ն�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(7,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateEndPos = true;
	//��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 1;
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 2;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 1;
	pParaPlate->design.bend2.widAngleBendBy=6;
	pParaPlate->design.bend2.ciBendMode = 3;
	pParaPlate->design.bend2.normal.widBendPlaneRefRod1 = 4;
	pParaPlate->design.bend2.normal.widBendPlaneRefRod2 = 5;
	pParaPlate->connrods.Clear();
	//1-���ˮƽ��
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,6,false);
	pParaPlate->connrods[0].InitTipInfo(0,1);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = BOLT_ORIGIN::LOCA_RODNODE;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt=1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle=BOLT_LAYOUT::LAYOUT_ONESIDE;
	pParaPlate->connrods[0].layoutbolts[0].szLenOffsetOf1stBolt[0]='0';
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.���������
	pParaPlate->connrods[1].InitBasicInfo(2,true,2,6,true);
	pParaPlate->connrods[1].InitTipInfo(0,0,1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[1].cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.��������б���ĸ�
	pParaPlate->connrods[2].InitBasicInfo(3,true,2,1,false);
	pParaPlate->connrods[2].InitTipInfo(0,0,1,2);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.�Ҳ�ˮƽ��
	pParaPlate->connrods[3].InitBasicInfo(4,true,3,6,true);
	pParaPlate->connrods[3].InitTipInfo(1,0,1,6);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[3].cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[3].cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_TOEDGE);
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.����б��
	pParaPlate->connrods[4].InitBasicInfo(5,false,3,2,false);
	pParaPlate->connrods[4].InitTipInfo(0,0,4,6);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6.������
	pParaPlate->connrods[5].InitBasicInfo(6,true,1,7,false);
	pParaPlate->connrods[5].InitTipInfo(1,0,1);
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	//7.����б��
	pParaPlate->connrods[6].InitBasicInfo(7,false,1,2,false);
	pParaPlate->connrods[6].InitTipInfo(1,0,1,6);
	pParaPlate->connrods[6].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[6].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[6].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[6].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(7);
}

void CStdComTempl::InitNodePlateTemp_HD3_2()
{	//TODO:�¼� wjh-2019.8.16
	szSketchImgFile = "PHD3_2.bmp";
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
	//1-���ˮƽ��(ʼ��)
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//2-���������(ʼ��)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//3-�Ҳ�ˮƽ��(�ն�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//4-����б��(ʼ��)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	//5-������(�ն�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//6-����б��(�ն�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(6,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateEndPos = true;
	//��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 5;
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 5;
	pParaPlate->design.bend2.widAngleBendBy = 1;
	pParaPlate->design.bend2.ciBendMode = 3;
	pParaPlate->design.bend2.normal.widBendPlaneRefRod1 = 1;
	pParaPlate->design.bend2.normal.widBendPlaneRefRod2 = 2;
	pParaPlate->connrods.Clear();
	//1-���ˮƽ��
	pParaPlate->connrods[0].InitBasicInfo(1,true,1,4,false);
	pParaPlate->connrods[0].InitTipInfo(0,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.���������
	pParaPlate->connrods[1].InitBasicInfo(2,true,3,6,false);
	pParaPlate->connrods[1].InitTipInfo(0,0,1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.�Ҳ�ˮƽ��
	pParaPlate->connrods[2].InitBasicInfo(3,true,2,6,false);
	pParaPlate->connrods[2].InitTipInfo(1,0,1,5);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.����б��
	pParaPlate->connrods[3].InitBasicInfo(4,false,2,2,false);
	pParaPlate->connrods[3].InitTipInfo(0,0,3,5);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.������
	pParaPlate->connrods[4].InitBasicInfo(5,true,1,6,false);
	pParaPlate->connrods[4].InitTipInfo(1,0,1);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	//6.����б��
	pParaPlate->connrods[5].InitBasicInfo(6,false,1,2,false);
	pParaPlate->connrods[5].InitTipInfo(1,0,1,5);
	pParaPlate->connrods[5].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[5].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[5].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(6);
}

void CStdComTempl::InitNodePlateTemp_HD3_G5()
{	//TODO:�¼� wjh-2019.8.16
	szSketchImgFile = "PHD3_G5.bmp";
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
	//1-ˮƽ�ᴩ����
	CStdTemplParaAngle* pParaAngle = NULL;
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(1,CLS_LINEANGLE);
	pParaAngle->idMidNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	//2-���������(ʼ��)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(2,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateStartPos = true;
	//3-����б��(ʼ��)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(3,CLS_LINEANGLE);
	pParaAngle->idStartNode = 1;
	pParaAngle->ciAngleWorkWing = 'Y';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateStartPos = true;
	pParaAngle->desStart.szXorCtrlWingOffset = "g";
	pParaAngle->desStart.szYorOthrWingOffset = "[=-P1#T";	//'['��ʾ����;']'��ʾ��������ָ����ʾ����
	//4-������(�ն�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(4,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = true;
	pParaAngle->bUpdateEndPos = true;
	//5-����б��(�ն�)
	pParaAngle = (CStdTemplParaAngle*)this->listParaRods.Add(5,CLS_LINEANGLE);
	pParaAngle->idEndNode = 1;
	pParaAngle->ciAngleWorkWing = 'X';
	pParaAngle->m_bNecessaryRod = false;
	pParaAngle->bUpdateEndPos = true;
	//��ͨ���Ӱ�
	CStdTemplParaPlate* pParaPlate = this->listParaPlates.Append();
	pParaPlate->wiType = CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->cbMirCreateFlag=0x01;
	pParaPlate->design.idBaseNode = 1;
	pParaPlate->design.idBaseRod = 1;
	pParaPlate->design.ciProfileStyle = 1;
	pParaPlate->design.ciDatumAngleWing = 'X';
	pParaPlate->design.bend.widAngleBendBy = 1;
	pParaPlate->design.bend.ciBendMode = 3;
	pParaPlate->design.bend.normal.widBendPlaneRefRod1 = 2;
	pParaPlate->design.bend.normal.widBendPlaneRefRod2 = 1;
	pParaPlate->connrods.Clear();
	//1-���ˮƽ��
	pParaPlate->connrods[0].InitBasicInfo(1, true, 1, 8, true);
	pParaPlate->connrods[0].InitTipInfo(2,2);
	pParaPlate->connrods[0].layoutbolts[0].basepoint.ciPosCalStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].basepoint.widBaseNodeOrBolt = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciLayoutStyle = 1;
	pParaPlate->connrods[0].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[0].layoutbolts.ReSize(1);
	//2.���������
	pParaPlate->connrods[1].InitBasicInfo(2,true,2,6,false);
	pParaPlate->connrods[1].InitTipInfo(0,0,1);
	pParaPlate->connrods[1].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[1].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[1].layoutbolts.ReSize(1);
	//3.����б��
	pParaPlate->connrods[2].InitBasicInfo(3, false, 1, 2, false);
	pParaPlate->connrods[2].InitTipInfo(0, 0, 1, 4);
	pParaPlate->connrods[2].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[2].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_StoE;
	pParaPlate->connrods[2].layoutbolts.ReSize(1);
	//4.������
	pParaPlate->connrods[3].InitBasicInfo(4, true, 1, 6, false);
	pParaPlate->connrods[3].InitTipInfo(1, 0, 1);
	pParaPlate->connrods[3].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[3].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[3].layoutbolts.ReSize(1);
	//5.����б��
	pParaPlate->connrods[4].InitBasicInfo(5, false, 1, 2, false);
	pParaPlate->connrods[4].InitTipInfo(1, 0, 1, 5);
	pParaPlate->connrods[4].layoutbolts[0].basepoint.ciPosCalStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciLayoutStyle = 0;
	pParaPlate->connrods[4].layoutbolts[0].ciFlowDirection = BOLT_LAYOUT::FLOW_EtoS;
	pParaPlate->connrods[4].layoutbolts.ReSize(1);
	pParaPlate->connrods.ReSize(5);
}
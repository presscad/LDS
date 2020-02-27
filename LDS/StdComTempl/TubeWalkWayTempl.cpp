#include "stdafx.h"
#include "Tower.h"
#include "TubeWalkWayTempl.h"

//////////////////////////////////////////////////////////////////////////
//CTubeWalkWayStdComTempl
CTubeWalkWayStdComTempl::CTubeWalkWayStdComTempl()
{
}
CTubeWalkWayStdComTempl::~CTubeWalkWayStdComTempl()
{

}
void CTubeWalkWayStdComTempl::InitComponentTempl(TEMPL_SOURCE* pTemplSource/*=NULL*/)
{
	szSketchImgFile = "WalkWay.bmp";
	//��ʼ����Ʋ���
	InitTemplParams();
	//���ģ���еĸ˼�������
	CStdTemplParaRod* pParaRod=listParaRods.Add(1,CLS_LINETUBE);	//�����ֹ�
	pParaRod->m_bNecessaryRod=true;
	pParaRod->m_bNeedUpdatePosition=false;
	pParaRod->m_bNeedUpdateNormal=false;
	//���ģ���нڵ㶨����
	CStdTemplParaNode* pParaNode=listParaNodes.Add(1);	//�ߵ���ƻ�׼�ڵ�
	pParaNode->m_bNecessaryNode=true;
	pParaNode=listParaNodes.Add(KEY4C("S"));
	pParaNode->m_bNecessaryNode=true;
	pParaNode=listParaNodes.Add(KEY4C("E"));
	pParaNode->m_bNecessaryNode=true;
	//
	InitWalkWayTempl();								//�Ǹ��ߵ�
	IStdComTempl::TEMPL_SOURCE source(0,1,1);
	if(pTemplSource==NULL)
		pTemplSource=&source;
	InitHolderTempl(pTemplSource->dwTemplParam);	//�ߵ�֧��				
}
void CTubeWalkWayStdComTempl::InitTemplParams()
{
	//�ߵ���Ʋ���
	STDTEMPL_PARAM_ITEM* pParam=NULL;
	pParam=listParams.Add(KEY4C("W"));	//�ߵ����
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=this->m_fWidth;
	pParam=listParams.Add(KEY4C("L"));	//�ߵ�����
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=this->m_fLength;
	pParam=listParams.Add(KEY4C("YA"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fYA;	//����ƫ��
	pParam=listParams.Add(KEY4C("ZH"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fZH;	//����߶�ƫ��
	pParam=listParams.Add(KEY4C("Ls"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=(m_fZjOffDist-m_fZdOffDist);	//�ߵ�ʼ������ƫ�ƾ���
	//֧����Ʋ���
	pParam=listParams.Add(KEY4C("Zh"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fZh;	//б���²�Z��ƫ��
	pParam=listParams.Add(KEY4C("D1"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fD1;	//б���²����ƫ��
	pParam=listParams.Add(KEY4C("D2"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fD2;	//б���ϲ�˵���ˮƽ�����ڵĺ���ƫ��
	pParam=listParams.Add(KEY4C("YB"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fYB;	//֧��λ������ƫ�ƾ���
	pParam=listParams.Add(KEY4C("X0"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=m_fZjOffDist;	//֧��λ������ƫ�ƾ���
	//
	pParam=listParams.Add(KEY4C("Md"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
	//pParam->value.liValue=this->m_nBoltMd;	//�ֹ������Ӱ��ϲ����ֱ��
	pParam=listParams.Add(KEY4C("BN"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=this->m_nBoltNum;	//�ֹ������Ӱ��ϲ����˨��
	pParam=listParams.Add(KEY4C("BS"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=this->m_fBoltSpace;//�ֹ������Ӱ��ϲ����˨���
	//�ߵ�����Ʋ���
	pParam=listParams.Add(KEY4C("PA"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=para.A;	//�ߵ����Ӱ����μн�
	pParam=listParams.Add(KEY4C("PB"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=para.B;	//��˨���Ӱ����μн�
	pParam=listParams.Add(KEY4C("PW"));
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
	//pParam->value.fValue=para.W;	//
	//װ������ϵ
	pParam=listParams.Add(KEY4C("acsO"));	//��׼���װ������ϵԭ��
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	//strcpy(pParam->exprstr,"Node_Pos(1)");
	strcpy(pParam->exprstr,"RodNode_Pos(1,1)");	//��Ϊ�˼��Ͻڵ㶨λ����Ч�����ͷΪ���Ͳ��ʱ��ƫ�������� wjh-2018.10.8
	pParam=listParams.Add(KEY4C("acsX"));	//��׼���װ������ϵX�᷽��δָ���յ㷽��ʱ��������ʼ��׼���������������죩
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"Rod_LenV(1)");
	pParam=listParams.Add(KEY4C("acsY"));	//��׼���װ������ϵY�᷽��
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"auto");	//�Զ�����X�ἰ��׼�ֹܵ�λ�ü���
	pParam=listParams.Add(KEY4C("acsZ"));	//��׼���װ������ϵZ�᷽��
	pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;	//��ά���ʸ������EXPRESSION_VALUE::VALUETYPE_OBJECT
	pParam->value.objtype.ciObjType=EXPRESSION_VALUE::OBJTYPE_COORD3D;
	pParam->value.objtype.wiPropValue=0;
	strcpy(pParam->exprstr,"auto");	//�Զ�����X�����(��֤���ϼ���ģ������ϵ��-Z����һ��)
}
void CTubeWalkWayStdComTempl::InitWalkWayTempl()
{
	//�ߵ��Ŀ��ƽڵ�
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode=listParaNodes.Add(KEY4C("SI"));	//�ߵ��Ҹ���ʼ���ڲ�ڵ�
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_INNER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"Ls");
	strcpy(pParaNode->posy_expression,"YA-W");
	strcpy(pParaNode->posz_expression,"ZH");

	pParaNode=listParaNodes.Add(KEY4C("EI"));	//�ߵ��Ҹ����ն��ڲ�ڵ�
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_INNER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"Ls+L");
	strcpy(pParaNode->posy_expression,"YA-W");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("SI");
	pParaNode->attachNodeArr[1]=KEY4C("S");
	pParaNode->attachNodeArr[2]=KEY4C("E");
	strcpy(pParaNode->offset_expression,"L");

	pParaNode=listParaNodes.Add(KEY4C("DI0"));	//�ߵ��ڲ��Ҹ˻�׼���
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_INNER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"0");
	strcpy(pParaNode->posy_expression,"YA-W");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("SI");
	pParaNode->attachNodeArr[1]=KEY4C("SI");
	pParaNode->attachNodeArr[2]=KEY4C("EI");
	strcpy(pParaNode->offset_expression,"-Ls");

	pParaNode=listParaNodes.Add(KEY4C("SO"));	//�ߵ��Ҹ���ʼ�����ڵ�
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_OUTER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"Ls");
	strcpy(pParaNode->posy_expression,"YA");
	strcpy(pParaNode->posz_expression,"ZH");
	
	pParaNode=listParaNodes.Add(KEY4C("EO"));	//�ߵ��Ҹ���ʼ�����ڵ�
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_OUTER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"Ls+L");
	strcpy(pParaNode->posy_expression,"YA");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("SO");
	pParaNode->attachNodeArr[1]=KEY4C("S");
	pParaNode->attachNodeArr[2]=KEY4C("E");
	strcpy(pParaNode->offset_expression,"L");

	pParaNode=listParaNodes.Add(KEY4C("DO0"));	//�ߵ�����Ҹ˻�׼���
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod= WAY_OUTER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"0");
	strcpy(pParaNode->posy_expression,"YA");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("SO");
	pParaNode->attachNodeArr[1]=KEY4C("SO");
	pParaNode->attachNodeArr[2]=KEY4C("EO");
	strcpy(pParaNode->offset_expression,"-Ls");
	//�Ǹ�
	CStdTemplParaAngle* pParaJg1=(CStdTemplParaAngle*)listParaRods.Add(WAY_INNER_ANGLE_ID,CLS_LINEANGLE);
	pParaJg1->m_idPartClsType=CLS_LINEANGLE;
	pParaJg1->m_bNecessaryRod= true;
	pParaJg1->keyStartNode=KEY4C("SI");
	pParaJg1->keyEndNode=KEY4C("EI");
	pParaJg1->normal.uidRefRodOfWorkWing=WAY_OUTER_ANGLE_ID;
	pParaJg1->normal.ciNormalDirection='Z';
	pParaJg1->ciAngleLayout=1;
	pParaJg1->desStart.cTouchLineType=pParaJg1->desEnd.cTouchLineType=3;//���Ƶ�
	pParaJg1->desStart.coordCtrlType=pParaJg1->desEnd.coordCtrlType=3;	//���Ƶ�ͶӰ
	pParaJg1->desStart.szXorCtrlWingOffset="-$4#g";
	pParaJg1->desStart.szYorOthrWingOffset="g";
	pParaJg1->desEnd.szXorCtrlWingOffset="-$4#g";
	pParaJg1->desEnd.szYorOthrWingOffset="g";
	pParaJg1->m_bNeedUpdatePosition=pParaJg1->m_bNeedUpdateNormal=false;
	//
	CStdTemplParaAngle* pParaJg2=(CStdTemplParaAngle*)listParaRods.Add(WAY_OUTER_ANGLE_ID,CLS_LINEANGLE);
	pParaJg2->m_idPartClsType=CLS_LINEANGLE;
	pParaJg2->m_bNecessaryRod= true;
	pParaJg2->keyStartNode=KEY4C("SO");
	pParaJg2->keyEndNode=KEY4C("EO");
	pParaJg2->normal.uidRefRodOfWorkWing=WAY_INNER_ANGLE_ID;	//��ǰ�Ǹֹ���֫��2�Ÿ˼�����
	pParaJg2->normal.ciNormalDirection='Z';	//��װ������ϵZ�ῴ�Ǹֵ�֫����Ϊ4�Ų�����ʽ
	pParaJg2->ciAngleLayout=4;
	pParaJg2->desStart.cTouchLineType=pParaJg2->desEnd.cTouchLineType=3;//���Ƶ�
	pParaJg2->desStart.coordCtrlType=pParaJg2->desEnd.coordCtrlType=3;	//���Ƶ�ͶӰ
	pParaJg2->desStart.szXorCtrlWingOffset="g";		//X֫����ƫ�������׼�ľ�
	pParaJg2->desStart.szYorOthrWingOffset="-$4#g";	//Y֫����ƫ�Ƹ���4�ŽǸֵı�׼�ľ�
	pParaJg2->desEnd.szXorCtrlWingOffset="g";
	pParaJg2->desEnd.szYorOthrWingOffset="-$4#g";
	pParaJg2->m_bNeedUpdatePosition=pParaJg1->m_bNeedUpdateNormal=false;
}
void CTubeWalkWayStdComTempl::InitHolderTempl(int iHolerType)
{
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode=listParaNodes.Add(KEY4C("BN"));
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=1;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=1;
	pParaNode->attachNodeArr[1]=KEY4C("S");
	pParaNode->attachNodeArr[2]=KEY4C("E");
	strcpy(pParaNode->offset_expression,"X0");
	//
	pParaNode=listParaNodes.Add(KEY4C("HI"));
	pParaNode->m_bNecessaryNode=false;
	pParaNode->keyLabelFatherRod=WAY_INNER_ANGLE_ID;
	strcpy(pParaNode->posx_expression,"X0");
	strcpy(pParaNode->posy_expression,"YA-W");
	strcpy(pParaNode->posz_expression,"ZH");
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("DI0");
	pParaNode->attachNodeArr[1]=KEY4C("SI");
	pParaNode->attachNodeArr[2]=KEY4C("EI");
	strcpy(pParaNode->offset_expression,"X0");
	if(iHolerType>0)
	{	//�Ϻ�ŽǸ�ʼ�ն�
		pParaNode=listParaNodes.Add(KEY4C("HS"));
		pParaNode->m_bNecessaryNode=false;
		pParaNode->keyLabelFatherRod=WAY_OUTER_ANGLE_ID;
		strcpy(pParaNode->posx_expression,"X0");
		strcpy(pParaNode->posy_expression,"YA");
		strcpy(pParaNode->posz_expression,"ZH");
		pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
		pParaNode->attachNodeArr[0]=KEY4C("DO0");
		pParaNode->attachNodeArr[1]=KEY4C("SO");
		pParaNode->attachNodeArr[2]=KEY4C("EO");
		strcpy(pParaNode->offset_expression,"X0");

		pParaNode=listParaNodes.Add(KEY4C("HE"));
		pParaNode->m_bNecessaryNode=false;
		strcpy(pParaNode->posx_expression,"X0");
		strcpy(pParaNode->posy_expression,"0");
		strcpy(pParaNode->posz_expression,"ZH");
		pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
		pParaNode->attachNodeArr[0]=KEY4C("HI");
		pParaNode->attachNodeArr[1]=KEY4C("HS");
		pParaNode->attachNodeArr[2]=KEY4C("HI");
		strcpy(pParaNode->offset_expression,"YA-W");
	}
	//
	if(iHolerType==0)
		InitPlateHolderTempl();		//�ְ�֧��
	else if(iHolerType==1)
		InitTriangleHolderTempl();	//����֧��
	else if(iHolerType==2)
		InitPlatformHolderTempl();	//̨ʽ֧��
}
void CTubeWalkWayStdComTempl::InitPlateHolderTempl()
{
	//�Ǹ����Ӱ�
	CStdTemplParaPlate* pParaPlate=listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_CONNECT_PLATE;
	pParaPlate->design.idBaseNode=KEY4C("HI");
	pParaPlate->design.idBaseRod=WAY_INNER_ANGLE_ID;
	pParaPlate->design.ciProfileStyle=1;	//��������
	pParaPlate->design.ciDatumLenAxis='Y';
	pParaPlate->design.ciDatumAngleWing='Y';
	CONNECT_ROD* pConnRod=pParaPlate->connrods.Append(CONNECT_ROD(2));
	pConnRod->cnMinBoltN=2;
	pConnRod->ciConnectWing='Y';
	pConnRod->ciCurrTipSideS0E1=1;
	pConnRod->dualSideEdge=true;
	BOLT_LAYOUT* pLayout=pConnRod->layoutbolts.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HI");
	pLayout->cnCount=2;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing=pConnRod->ciConnectWing;
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;
	//�ߵ�����Ӿ���
	pParaPlate=listParaPlates.Add(2);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_TUBECIRPLATE;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBasePlate=1;
}
//����ʽ�Ǹ�֧��ģ��
void CTubeWalkWayStdComTempl::InitTriangleHolderTempl()
{	//�ڵ�
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode=listParaNodes.Add(KEY4C("XS"));
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->keyLabelFatherRod=4;
	pParaNode->attachNodeArr[0]=KEY4C("HS");
	pParaNode->attachNodeArr[1]=KEY4C("HS");
	pParaNode->attachNodeArr[2]=KEY4C("HE");
	strcpy(pParaNode->offset_expression,"D2");

	pParaNode=listParaNodes.Add(KEY4C("XE"));
	pParaNode->m_bNecessaryNode=false;
	strcpy(pParaNode->posx_expression,"X0");
	strcpy(pParaNode->posy_expression,"D1");
	strcpy(pParaNode->posz_expression,"-Zh");
	//��ŽǸ�
	CStdTemplParaAngle* pParaJg1=(CStdTemplParaAngle*)listParaRods.Add(BRACE_UPPER_HORI_ANGLE_ID,CLS_LINEANGLE);
	pParaJg1->m_idPartClsType=CLS_LINEANGLE;
	pParaJg1->m_bNecessaryRod= true;
	pParaJg1->keyStartNode=KEY4C("HS");
	pParaJg1->keyEndNode=KEY4C("HE");
	pParaJg1->normal.uidRefRodOfWorkWing=2;
	pParaJg1->normal.ciNormalDirection='Z';
	pParaJg1->ciAngleLayout=3;
	pParaJg1->desStart.cTouchLineType=0;
	pParaJg1->desStart.cTouchLine=1;	//�����ߵ��������Ǹ�Y֫����
	pParaJg1->desStart.uidTouchLineRod = 3;
	pParaJg1->desStart.szXorCtrlWingOffset[0]='0';
	pParaJg1->desStart.szYorOthrWingOffset[0]='g';
	pParaJg1->desEnd.cTouchLineType = 3;//���Ƶ�
	pParaJg1->desEnd.coordCtrlType = 3;
	pParaJg1->desEnd.szXorCtrlWingOffset[0]='g';
	pParaJg1->desEnd.szYorOthrWingOffset[0]='g';
	pParaJg1->wiStartOddCalStyle=pParaJg1->wiEndOddCalStyle=1;	//������˨��������ͷ
	//ʼ����˨���ò���
	BOLT_LAYOUT* pLayout=NULL;
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HS");
	pLayout->basepoint.widBaseAngle=WAY_OUTER_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;	//ʼ����
	//�м���˨����
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HI");
	pLayout->basepoint.widBaseAngle=WAY_INNER_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;	//ʼ����
	//�ն���˨���ò���
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HE");
	pLayout->cnCount=0;		//�Ӳ����л�ȡ
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='Y';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_EtoS;	//�յ�ʼ
	//б�ŽǸ�
	CStdTemplParaAngle* pParaJg2=(CStdTemplParaAngle*)listParaRods.Add(BRACE_GRADIENT_ANGLE_ID,CLS_LINEANGLE);
	pParaJg2->m_idPartClsType=CLS_LINEANGLE;
	pParaJg2->m_bNecessaryRod= true;
	pParaJg2->keyStartNode=KEY4C("XS");
	pParaJg2->keyEndNode=KEY4C("XE");
	pParaJg2->normal.uidRefRodOfWorkWing=BRACE_UPPER_HORI_ANGLE_ID;
	pParaJg2->normal.ciNormalDirection='x';
	pParaJg2->ciAngleLayout=1;
	pParaJg2->desStart.cTouchLineType=0;
	pParaJg2->desStart.cTouchLine=1;
	pParaJg2->desStart.uidTouchLineRod = 4;
	pParaJg2->desStart.coordCtrlType=3;
	pParaJg2->desStart.szXorCtrlWingOffset="-R4#T";//"g";
	pParaJg2->desStart.szYorOthrWingOffset="g";//"-g,-P2#T";
	pParaJg2->desEnd.cTouchLineType=3;	//���Ƶ�
	pParaJg2->desEnd.coordCtrlType=3;	//���Ƶ�ͶӰ
	pParaJg2->desEnd.szXorCtrlWingOffset="g";
	pParaJg2->desEnd.szYorOthrWingOffset="g";//"-g,-P2#T";
	pParaJg2->wiStartOddCalStyle=pParaJg2->wiEndOddCalStyle=1;	//������˨��������ͷ
	//ʼ����˨���ò���
	pLayout=pParaJg2->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("XS");
	pLayout->basepoint.widBaseAngle=4;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_StoE;	//ʼ����
	//�ն���˨���ò���
	pLayout=pParaJg2->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("XE");
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=BOLT_LAYOUT::FLOW_EtoS;	//ʼ����
	//�ϲ��ߵ����Ӱ�
	CStdTemplParaPlate* pParaPlate=listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_CIRBOLTPLATE;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBasePlate=-4;
	//�²��ߵ����Ӱ�
	pParaPlate=listParaPlates.Add(2);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_CIRBOLTPLATE;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBasePlate=-5;
	pParaPlate->design.idAssistPart=4;
}
void CTubeWalkWayStdComTempl::InitPlatformHolderTempl()
{
	//�ڵ�
	CStdTemplParaNode* pParaNode=NULL;
	pParaNode=listParaNodes.Add(KEY4C("hS"));
	pParaNode->m_bNecessaryNode=false;
	strcpy(pParaNode->posx_expression,"X0");
	strcpy(pParaNode->posy_expression,"YB+D1");
	strcpy(pParaNode->posz_expression,"-Zh");

	pParaNode=listParaNodes.Add(KEY4C("hE"));
	pParaNode->m_bNecessaryNode=false;
	strcpy(pParaNode->posx_expression,"X0");
	strcpy(pParaNode->posy_expression,"0");
	strcpy(pParaNode->posz_expression,"-Zh");

	pParaNode=listParaNodes.Add(KEY4C("XS"));
	pParaNode->keyLabelFatherRod=4;
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("HS");
	pParaNode->attachNodeArr[1]=KEY4C("HS");
	pParaNode->attachNodeArr[2]=KEY4C("HE");
	strcpy(pParaNode->offset_expression,"D2");

	pParaNode=listParaNodes.Add(KEY4C("VS"));
	pParaNode->keyLabelFatherRod=4;
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("HE");
	pParaNode->attachNodeArr[1]=KEY4C("HE");
	pParaNode->attachNodeArr[2]=KEY4C("HS");
	strcpy(pParaNode->offset_expression,"YB");

	pParaNode=listParaNodes.Add(KEY4C("VE"));
	pParaNode->keyLabelFatherRod=6;
	pParaNode->m_bNecessaryNode=false;
	pParaNode->ciPosCalType=CStdTemplParaNode::OFFSET;
	pParaNode->attachNodeArr[0]=KEY4C("hE");
	pParaNode->attachNodeArr[1]=KEY4C("hE");
	pParaNode->attachNodeArr[2]=KEY4C("hS");
	strcpy(pParaNode->offset_expression,"YB");
	//�Ϻ�ŽǸ�
	CStdTemplParaAngle* pParaJg1=(CStdTemplParaAngle*)listParaRods.Add(BRACE_UPPER_HORI_ANGLE_ID,CLS_LINEANGLE);
	pParaJg1->m_idPartClsType=CLS_LINEANGLE;
	pParaJg1->m_bNecessaryRod= true;
	pParaJg1->keyStartNode=KEY4C("HS");
	pParaJg1->keyEndNode=KEY4C("HE");
	pParaJg1->normal.uidRefRodOfWorkWing=2;
	pParaJg1->normal.ciNormalDirection='Z';
	pParaJg1->ciAngleLayout=3;
	pParaJg1->desStart.cTouchLineType = 0;
	pParaJg1->desStart.cTouchLine = 1;	//�����ߵ��������Ǹ�Y֫����
	pParaJg1->desStart.szXorCtrlWingOffset[0] = '0';
	pParaJg1->desStart.szYorOthrWingOffset[0] = 'g';
	pParaJg1->desEnd.cTouchLineType = 3;//���Ƶ�
	pParaJg1->desEnd.coordCtrlType = 3;
	pParaJg1->desEnd.szXorCtrlWingOffset[0] = 'g';
	pParaJg1->desEnd.szYorOthrWingOffset[0] = 'g';
	pParaJg1->wiStartOddCalStyle=pParaJg1->wiEndOddCalStyle=1;	//������˨��������ͷ
	//ʼ����˨���ò���
	BOLT_LAYOUT* pLayout=NULL;
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HS");
	pLayout->basepoint.widBaseAngle=WAY_OUTER_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//ʼ����
	//�м���˨����
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HI");
	pLayout->basepoint.widBaseAngle=WAY_INNER_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//ʼ����
	//�ն���˨���ò���
	pLayout=pParaJg1->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("HE");
	pLayout->cnCount=0;		//�Ӳ����л�ȡ
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='Y';
	pLayout->ciFlowDirection=1;	//�յ�ʼ
	//б�ŽǸ�
	CStdTemplParaAngle* pParaJg2=(CStdTemplParaAngle*)listParaRods.Add(BRACE_GRADIENT_ANGLE_ID,CLS_LINEANGLE);
	pParaJg2->m_idPartClsType=CLS_LINEANGLE;
	pParaJg2->m_bNecessaryRod= true;
	pParaJg2->keyStartNode=KEY4C("XS");
	pParaJg2->keyEndNode=KEY4C("hS");
	pParaJg2->normal.uidRefRodOfWorkWing=4;
	pParaJg2->normal.ciNormalDirection='x';
	pParaJg2->ciAngleLayout=1;
	pParaJg2->desStart.cTouchLineType=3;
	pParaJg2->desStart.coordCtrlType=3;
	pParaJg2->desStart.szXorCtrlWingOffset="g,-$4#T";;
	pParaJg2->desStart.szYorOthrWingOffset='g';
	pParaJg2->desEnd.cTouchLineType=3;	//���Ƶ�
	pParaJg2->desEnd.coordCtrlType=3;	//���Ƶ�ͶӰ
	pParaJg2->desEnd.szXorCtrlWingOffset='g';
	pParaJg2->desEnd.szYorOthrWingOffset='g';
	pParaJg2->wiStartOddCalStyle=1;	//������˨��������ͷ
	//ʼ����˨���ò���
	pLayout=pParaJg2->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("XS");
	pLayout->basepoint.widBaseAngle=4;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//ʼ����
	//�º�ŽǸ�
	CStdTemplParaAngle* pParaJg3=(CStdTemplParaAngle*)listParaRods.Add(BRACE_DOWN_HORI_ANGLE_ID,CLS_LINEANGLE);
	pParaJg3->m_idPartClsType=CLS_LINEANGLE;
	pParaJg3->m_bNecessaryRod= true;
	pParaJg3->keyStartNode=KEY4C("hS");
	pParaJg3->keyEndNode=KEY4C("hE");
	pParaJg3->normal.uidRefRodOfWorkWing=2;
	pParaJg3->normal.ciNormalDirection='Z';
	pParaJg3->ciAngleLayout=4;
	pParaJg3->desStart.cTouchLineType=pParaJg3->desEnd.cTouchLineType=3;//���Ƶ�
	pParaJg3->desStart.coordCtrlType=pParaJg3->desEnd.coordCtrlType=3;	//���Ƶ�ͶӰ
	pParaJg3->desStart.szXorCtrlWingOffset[0]='g';
	pParaJg3->desStart.szYorOthrWingOffset[0]='g';
	pParaJg3->desEnd.szXorCtrlWingOffset[0] = 'g';
	pParaJg3->desEnd.szYorOthrWingOffset[0] = 'g';
	pParaJg3->wiEndOddCalStyle=1;	//������˨��������ͷ
	//
	pLayout=pParaJg3->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("hE");
	pLayout->cnCount=0;		//�Ӳ����л�ȡ
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=1;	//�յ�ʼ
	//����Ǹ�
	CStdTemplParaAngle* pParaJg4=(CStdTemplParaAngle*)listParaRods.Add(7,CLS_LINEANGLE);
	pParaJg4->m_idPartClsType=CLS_LINEANGLE;
	pParaJg4->m_bNecessaryRod= true;
	pParaJg4->keyStartNode=KEY4C("VS");
	pParaJg4->keyEndNode=KEY4C("VE");
	pParaJg4->normal.uidRefRodOfWorkWing=4;
	pParaJg4->normal.ciNormalDirection='X';
	pParaJg4->ciAngleLayout=3;
	pParaJg4->desStart.cTouchLineType=0;
	pParaJg4->desStart.cTouchLine=1;
	pParaJg4->desStart.uidTouchLineRod = 4;
	pParaJg4->desStart.szXorCtrlWingOffset="-$4#T";
	pParaJg4->desStart.szYorOthrWingOffset="g";
	pParaJg4->desEnd.cTouchLineType=0;
	pParaJg4->desEnd.cTouchLine=0;
	pParaJg4->desStart.uidTouchLineRod = 6;
	pParaJg4->desEnd.szXorCtrlWingOffset="-$6#T";
	pParaJg4->desEnd.szYorOthrWingOffset="g";
	pParaJg4->wiStartOddCalStyle=pParaJg4->wiEndOddCalStyle=1;	//������˨��������ͷ
	//ʼ����˨
	pLayout=pParaJg4->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("VS");
	pLayout->basepoint.widBaseAngle=BRACE_UPPER_HORI_ANGLE_ID;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//ʼ����
	//�ն���˨
	pLayout=pParaJg4->m_layoutBoltArr.Append(BOLT_LAYOUT());
	pLayout->basepoint.ciPosCalStyle=1;
	pLayout->basepoint.widBaseNodeOrBolt=KEY4C("hS");
	pLayout->basepoint.widBaseAngle=6;
	pLayout->cnCount=1;
	pLayout->ciLayoutStyle=1;
	pLayout->ciDatumWing='X';
	pLayout->ciFlowDirection=0;	//ʼ����
	//�ϲ��ߵ����Ӱ�
	CStdTemplParaPlate* pParaPlate=listParaPlates.Add(1);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_CIRBOLTPLATE;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBasePlate=-4;	//��:����ʼ��|��:�����ն�
	//�²��ߵ����Ӱ�
	pParaPlate=listParaPlates.Add(2);
	pParaPlate->wiType=CStdTemplParaPlate::TYPE_W_CIRBOLTPLATE;
	pParaPlate->design.idBaseRod=1;
	pParaPlate->design.idBaseNode=KEY4C("BN");
	pParaPlate->design.idBasePlate=-6;	//��:����ʼ��|��:�����ն�
}
bool CTubeWalkWayStdComTempl::IsMeetConditions()
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
//
CTubeWalkWayStdComModel::CTubeWalkWayStdComModel(IModel* pModel):CStdComModel(pModel)
{
}
CTubeWalkWayStdComModel::~CTubeWalkWayStdComModel(void)
{

}
bool CTubeWalkWayStdComModel::BuildAcsOfAssmComponent()
{
	STDTEMPL_PARAM_ITEM* pParam;
	CStdComTemplExpression expr(m_pStdTempl);
	m_xNearAxisZ.Set(0,0,-1);
	//��׼���װ������ϵԭ��
	pParam=m_pStdTempl->listParams.GetValue(KEY4C("acsO"));
	int ciOrgFuncType=0;
	if(strstr(pParam->exprstr,"RodNode_Pos")!=NULL)
		ciOrgFuncType=2;
	else if(strstr(pParam->exprstr,"Node_Pos")!=NULL)
		ciOrgFuncType=1;
	else
		return false;
	char* pszParamStart=strchr(pParam->exprstr,'(');
	if(pszParamStart!=NULL)
		pszParamStart++;
	char* pszParamEnd=strchr(pParam->exprstr,')');
	if(pszParamEnd!=NULL)
	{
		CXhChar16 szParam;
		szParam.NCopy(pszParamStart,pszParamEnd-pszParamStart);
		EXPRESSION_FUNC* pExprFunc=ciOrgFuncType==1?expr.FromFunctionName("Node_Pos"):expr.FromFunctionName("RodNode_Pos");
		pExprFunc->Calc(&pParam->value,szParam,&expr);
		acs.origin.Set(pParam->value.coord3d[0],pParam->value.coord3d[1],pParam->value.coord3d[2]);
	}
	pParam=m_pStdTempl->listParams.GetValue(KEY4C("acsX"));
	if(strstr(pParam->exprstr,"Rod_LenV")==NULL)
		return false;
	pszParamStart=strchr(pParam->exprstr,'(');
	if(pszParamStart!=NULL)
		pszParamStart++;
	pszParamEnd=strchr(pParam->exprstr,')');
	if(pszParamEnd!=NULL)
	{
		CXhChar16 szParam;
		szParam.NCopy(pszParamStart,pszParamEnd-pszParamStart);
		EXPRESSION_FUNC* pExprFunc=expr.FromFunctionName("Rod_LenV");
		pExprFunc->Calc(&pParam->value,szParam,&expr);
		acs.axis_x.Set(pParam->value.coord3d[0],pParam->value.coord3d[1],pParam->value.coord3d[2]);
	}
	//���ݻ�׼�˼�����λ�õ�������ϵ
	GEPOINT nearVecX(1,0,0),nearVecY(0,1,0);
	CLDSLinePart* pBaseRod=GetLdsRodFromId(1);
	if(pBaseRod)
	{	
		GEPOINT ptS=pBaseRod->xPosStart,ptE=pBaseRod->xPosEnd;
		if(fabs(acs.axis_x.x)>fabs(acs.axis_x.y))
		{	//X�����ߵ�
			if(ptS.x>0&&ptE.x>0)		//�Ҳ�ᵣ
				nearVecX.Set(1,0,0);
			else if(ptS.x<0&&ptE.x<0)	//���ᵣ
				nearVecX.Set(-1,0,0);
			else if(ptS.x*ptE.x<0)		//��������
				nearVecX=(ptS.y>0)?GEPOINT(1,0,0):GEPOINT(-1,0,0);
			if(ptS.y>0&&ptE.y>0)
				nearVecY.Set(0,-1,0);
			else
				nearVecY.Set(0,1,0);
		}
		else
		{	//Y�����ߵ�
			if(ptS.y>0&&ptE.y>0)		//ǰ��ᵣ
				nearVecX.Set(0,1,0);
			else if(ptS.y<0&&ptE.y<0)	//���ᵣ
				nearVecX.Set(0,-1,0);
			else if(ptS.y*ptE.y<0)		//��������
				nearVecX=(ptS.x>0)?GEPOINT(0,-1,0):GEPOINT(0,1,0);
			if(ptS.x>0&&ptE.x>0)
				nearVecY.Set(-1,0,0);
			else
				nearVecY.Set(1,0,0);
		}
	}
	if(acs.axis_x*nearVecX<0)
		acs.axis_x*=-1;
	acs.axis_y=acs.axis_x^GEPOINT(0,0,1);
	normalize(acs.axis_x);
	normalize(acs.axis_y);
	if(acs.axis_y*nearVecY<0)
		acs.axis_y*=-1;
	normalize(acs.axis_y);
	acs.axis_z=acs.axis_x^acs.axis_y;
	if (acs.axis_z.z>0)
	{	//�ߵ�ƽ������ϵZ��ʼ�ճ��ϣ���ģ������ϵZ�Ḻ�� wjh-2019.9.22
		acs.axis_z*=-1.0;
		acs.axis_y*=-1.0;
	}
	normalize(acs.axis_z);
	return true;
}
void CTubeWalkWayStdComModel::UpdateNodeAndRodBaseInfo()
{
	CTubeWalkWayStdComTempl* pTempl=(CTubeWalkWayStdComTempl*)m_pStdTempl;
	CLDSLinePart* pBaseTube=GetLdsRodFromId(1);
	if(pBaseTube==NULL||pBaseTube->GetClassTypeId()!=CLS_LINETUBE)
		return;
	for(CStdTemplParaNode* pParaNode=m_pStdTempl->listParaNodes.GetFirst();pParaNode;pParaNode=m_pStdTempl->listParaNodes.GetNext())
	{
		if(pParaNode->m_bNecessaryNode)
			continue;
		CLDSNode* pNode=GetLdsNodeFromId(pParaNode->Id);
		if(pNode==NULL)
		{
			logerr.Log("%d��ʧ�ڵ���Ϣ",pParaNode->Id);
			continue;
		}
		pNode->SetLayer("TPG");
		pNode->iSeg=pBaseTube->iSeg;
		pNode->cfgword=pBaseTube->cfgword;
		if(pNode->m_cPosCalType==CLDSNode::COORD_OFFSET)
		{
			pNode->SetArcLift(pBaseTube->ArcLiftHandle());
			pNode->CalPosition(true);
		}
	}
	for(CStdTemplParaRod* pParaRod=m_pStdTempl->listParaRods.GetFirst();pParaRod;pParaRod=m_pStdTempl->listParaRods.GetNext())
	{
		CLDSLinePart* pRod=GetLdsRodFromId(pParaRod->Id);
		if(pRod==NULL)
		{
			logerr.Log("%d��ʧ�˼���Ϣ",pParaRod->Id);
			continue;
		}
		if(pRod->GetClassTypeId()==CLS_LINETUBE)
			continue;
		pRod->SetLayer("TFG");
		pRod->iSeg=pBaseTube->iSeg;
		pRod->cfgword=pBaseTube->cfgword;
		pRod->SetArcLift(pBaseTube->ArcLiftHandle());
		pRod->CalPosition(true);
		//������˨������Ϣ
		for(CLsRef* pLsRef=pRod->GetFirstLsRef();pLsRef;pLsRef=pRod->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			pBolt->iSeg=pBaseTube->iSeg;
			pBolt->cfgword=pBaseTube->cfgword;
		}
	}
	for(CStdTemplParaPlate* pParaPlate=m_pStdTempl->listParaPlates.GetFirst();pParaPlate;pParaPlate=m_pStdTempl->listParaPlates.GetNext())
	{
		CLDSGeneralPlate* pPlate=GetLdsPlateFromId(pParaPlate->Id);
		if(pPlate==NULL)
		{
			logerr.Log("%d��ʧ�ְ���Ϣ",pParaPlate->Id);
			continue;
		}
		pPlate->iSeg=pBaseTube->iSeg;
		pPlate->cfgword=pBaseTube->cfgword;
	}
}
bool CTubeWalkWayStdComModel::BuildModel(CStdComTempl* pStdTempl)
{
	if(CStdComModel::BuildModel(pStdTempl))
		UpdateNodeAndRodBaseInfo();
	return true;
}

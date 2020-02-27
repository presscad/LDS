// DesignWireHole.cpp: implementation of the CDesignWireHole class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LDS.h"
#include "GlobalFunc.h"
#include "tower.h"
#include "env_def.h"
#include "SortFunc.h"
#include "LogFile.h"
#include "GeneralNodeDesign.h"
#include "CommonLjSmartDesign.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void GENERAL_NODEDESIGN_RULE::InitDefaultParams()
{
	wiSerial=0;
	blHasPlate=true;		//�Ƿ���Ҫʵ�������
	cnBoltDirectConn=2;		//�����(ֱ��)������˨����
	cnBaseAngleBoltRows=2;	//��׼����������õ���˨����
	blPriorMiddleEndLj=true;//�������Ӧ�ñ��滻��
}

INTELLI_MODEL_GUIDE gxModelGuide;
IMPLEMENT_PROP_FUNC(INTELLI_MODEL_GUIDE);
void INTELLI_MODEL_GUIDE::InitPropHashtable()
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(50);//CreateHashTable(HASHTABLESIZE);
	//ͨ��������ƹ���
	AddPropItem("ciProcessLegacyConnections", PROPLIST_ITEM(id++, "����������ʽ", "", "0.�������|1.���������"));
	AddPropItem("ciPlateProfileStyle", PROPLIST_ITEM(id++, "��������", "", "0.��׼����|1.��������"));
	//��ͷ�ᵣ�������
	AddPropItem("blIncArmHeadConnPlates", PROPLIST_ITEM(id++, "���Ӱ����", "������ͷ�ᵣ���Ӱ����","��|��"));
	AddPropItem("blAddWireAngle", PROPLIST_ITEM(id++, "���߽Ǹ����", "��Ҫʱ����������߽Ǹ�", "��|��"));
	AddPropItem("blAddWirePoint", PROPLIST_ITEM(id++, "�ҵ��������", "��ӹҵ��������", "��|��"));
	//����������������
	AddPropItem("blIncSlopeTransitPlates", PROPLIST_ITEM(id++, "���°�", "�������°�,��ͷƿ�ڰ�","��|��"));
	AddPropItem("blIncArmConnPlates", PROPLIST_ITEM(id++, "����ᵣ���Ӱ�", "��������ᵣ���Ӱ�", "��|��"));
	AddPropItem("blSectionCornerPlates", PROPLIST_ITEM(id++, "������������", "����ˮƽ�������ӽǰ�", "��|��"));
	//��������
	AddPropItem("VPlate.ciProfileSchema", PROPLIST_ITEM(id++, "V����������", "V����������", "0.��׼����|1.��������|2.���λ�"));
	AddPropItem("VPlate.blVPlateInside", PROPLIST_ITEM(id++, "V�����λ��", "V�����ԽǸ�֫��λ��", "������Ƥ|������Ƥ"));
	AddPropItem("VPlate.siSpecThick", PROPLIST_ITEM(id++, "V�����", "0.��ʾ�Զ�������;>0.��ʾͳһָ�����"));
	AddPropItem("VPlate.siBendLineSpace", PROPLIST_ITEM(id++, "V��������϶", "V��������϶ֵ"));
	AddPropItem("VPlate.ciTouchLineType", PROPLIST_ITEM(id++, "���߲Ľ��ӷ�ʽ", "V���б�Ľ��ӷ�ʽ", "0.������|1.ƫ��׼��"));
	AddPropItem("VPlate.siBendLineG", PROPLIST_ITEM(id++, "ƫ��׼��"));
	AddPropItem("VPlate.cnMinBaseAngleBolts", PROPLIST_ITEM(id++, "��׼��������˨��", "V����׼���Ӳ��ϵ���С��˨��"));
}
int INTELLI_MODEL_GUIDE::GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen/* = 100*/)
{	//��������ID�õ�����ֵ
	CXhChar100 szText;
	if (GetPropID("ciProcessLegacyConnections") == id)
	{
		if (this->ciProcessLegacyConnections == 0)
			szText.Copy("0.�������");
		else
			szText.Copy("1.���������");
	}
	else if (GetPropID("ciPlateProfileStyle") == id)
	{
		if (this->ciPlateProfileStyle == 0)
			szText.Copy("0.��׼����");
		else
			szText.Copy("1.��������");
	}
	//��ͷ�ᵣ�������
	else if (GetPropID("blIncArmHeadConnPlates") == id)
	{	//������ͷ�ᵣ���Ӱ����
		szText = (this->blIncArmHeadConnPlates) ? "��" : "��";
	}
	else if (GetPropID("blAddWireAngle") == id)
	{	//��Ҫʱ����������߽Ǹ�
		szText = (this->blAddWireAngle) ? "��" : "��";
	}
	else if (GetPropID("blAddWirePoint") == id)
	{	//��ӹҵ��������
		szText = (this->blAddWirePoint) ? "��" : "��";
	}
	//����������������
	else if (GetPropID("blIncSlopeTransitPlates") == id)
	{	//�������°�,��ͷƿ�ڰ�
		szText = (this->blIncSlopeTransitPlates) ? "��" : "��";
	}
	else if (GetPropID("blIncArmConnPlates") == id)
	{	//��������ᵣ���Ӱ�
		szText = (this->blIncArmConnPlates) ? "��" : "��";
	}
	else if (GetPropID("blSectionCornerPlates") == id)
	{	//����ˮƽ�������ӽǰ�
		szText = (this->blSectionCornerPlates) ? "��" : "��";
	}
	//��������
	else if (GetPropID("VPlate.blVPlateInside") == id)
	{	//V�����ԽǸ�֫��λ��
		szText = (multileg.blVPlateInside) ? "������Ƥ" : "������Ƥ";
	}
	else if (GetPropID("VPlate.cnMinBaseAngleBolts") == id)
	{	//V����׼���Ӳ��ϵ���С��˨��
		szText.Printf("%d", multileg.snMinBaseAngleBolts);
	}
	else if (GetPropID("VPlate.siBendLineSpace") == id)
	{	//V��������϶
		szText.Printf("%d", multileg.siBendLineSpace);
	}
	else if (GetPropID("VPlate.siBendLineG") == id)
	{	//V���ƫ��׼��
		szText.Printf("%d", multileg.siBendLineG);
	}
	else if (GetPropID("VPlate.ciProfileSchema") == id)
	{
		if (multileg.ciProfileSchema == 0)
			szText.Copy("0.��׼����");
		else if (multileg.ciProfileSchema == 1)
			szText.Copy("1.��������");
		else if (multileg.ciProfileSchema == 2)
			szText.Copy("2.���λ�");
	}
	else if (GetPropID("VPlate.siSpecThick") == id)
	{
		szText.Printf("%d", multileg.siSpecThick);
	}
	else if (GetPropID("VPlate.ciTouchLineType") == id)
	{
		if (multileg.ciTouchLineType == 0)
			szText.Copy("0.������");
		else if (multileg.ciTouchLineType == 1)
			szText.Copy("1.ƫ��׼��");
	}
	if (valueStr)
		StrCopy(valueStr, szText, nMaxStrBufLen);
	return szText.Length;
}
INTELLI_MODEL_GUIDE::INTELLI_MODEL_GUIDE()
{
	m_nMaxNodeCountOfManualDesign=5;
	ciProcessLegacyConnections = 1;
	ciPlateProfileStyle = 1;
	wiInitBranchNodeSpace=60;			//�����ӽڵ��ֳ�ʼ���
	wiBranchNodeSpaceIncrementStep=20;	//�����ӽڵ��ּ����������
	blIncArmHeadConnPlates = blAddWireAngle = blAddWirePoint = true;
	blIncSlopeTransitPlates = blIncArmConnPlates = blSectionCornerPlates = true;
	multileg.snMinBaseAngleBolts = 2;
	multileg.blVPlateInside = true;
	multileg.ciProfileSchema = 1;
	multileg.ciTouchLineType = 0;
	multileg.siBendLineSpace = 10;
	multileg.siBendLineG = 0;
	multileg.siSpecThick = 0;
}
void INTELLI_MODEL_GUIDE::InitNodeDesignRule()
{
	GENERAL_NODEDESIGN_RULE xarrRules1[5]={
		{1,"L_1LB1",false,1,1,true},
		{2,"L_1LB2",false,1,1,true},
		{3,"L_1LE1",false,1,1,true},
		{4,"L_1LE2",false,1,1,true},
		{5,"L_1LO1",false,1,1,true},
	};
	GENERAL_NODEDESIGN_RULE xarrRules2[9]={
		{1,"L_2LB1",false,1,1,true},
		{2,"L_2LB2",false,1,1,true},
		{3,"L_2LB3",false,1,1,true},
		{4,"L_2LB4",false,1,1,true},
		{5,"L_2LE1",false,1,1,true},
		{6,"L_2LE2",false,1,1,true},
		{7,"L_2LC1",false,1,1,true},
		{8,"L_2LC2",false,1,1,true},
		{9,"L_2LO1",false,1,1,true},
	};
	GENERAL_NODEDESIGN_RULE xarrRules3[6]={
		{1,"L_3LB1",false,1,1,true},
		{2,"L_3LB2",false,1,1,true},
		{3,"L_3LC1",false,1,1,true},
		{4,"L_3LC2",false,1,1,true},
		{5,"L_3LC3",false,1,1,true},
		{6,"L_3LO1",false,1,1,true},
	};
}
void INTELLI_MODEL_GUIDE::CopyFrom(INTELLI_MODEL_GUIDE* pModelGuide)
{
	ciProcessLegacyConnections = pModelGuide->ciProcessLegacyConnections;
	m_nMaxNodeCountOfManualDesign = pModelGuide->m_nMaxNodeCountOfManualDesign;
	ciPlateProfileStyle = pModelGuide->ciPlateProfileStyle;
	blIncArmHeadConnPlates = pModelGuide->blIncArmHeadConnPlates;
	blAddWireAngle = pModelGuide->blAddWireAngle;
	blAddWirePoint = pModelGuide ->blAddWirePoint;
	blIncSlopeTransitPlates = pModelGuide->blIncSlopeTransitPlates;
	blIncArmConnPlates = pModelGuide->blIncArmConnPlates;
	blSectionCornerPlates = pModelGuide ->blSectionCornerPlates;
	multileg.snMinBaseAngleBolts = pModelGuide ->multileg.snMinBaseAngleBolts;
	multileg.blVPlateInside = pModelGuide ->multileg.blVPlateInside;
	multileg.ciProfileSchema = pModelGuide->multileg.ciProfileSchema;
	multileg.ciTouchLineType = pModelGuide->multileg.ciTouchLineType;
	multileg.siBendLineSpace = pModelGuide->multileg.siBendLineSpace;
	multileg.siBendLineG = pModelGuide->multileg.siBendLineG;
	multileg.siSpecThick = pModelGuide->multileg.siSpecThick;
}
void INTELLI_MODEL_GUIDE::CopyTo(INTELLI_MODEL_GUIDE* pModelGuide)
{
	pModelGuide->ciProcessLegacyConnections = ciProcessLegacyConnections;
	pModelGuide->m_nMaxNodeCountOfManualDesign = m_nMaxNodeCountOfManualDesign;
	pModelGuide->ciPlateProfileStyle = ciPlateProfileStyle;
	pModelGuide->blIncArmHeadConnPlates = blIncArmHeadConnPlates;
	pModelGuide->blAddWireAngle = blAddWireAngle;
	pModelGuide->blAddWirePoint = blAddWirePoint;
	pModelGuide->blIncSlopeTransitPlates = blIncSlopeTransitPlates;
	pModelGuide->blIncArmConnPlates = blIncArmConnPlates;
	pModelGuide->blSectionCornerPlates = blSectionCornerPlates;
	pModelGuide->multileg.snMinBaseAngleBolts = multileg.snMinBaseAngleBolts;
	pModelGuide->multileg.blVPlateInside = multileg.blVPlateInside;
	pModelGuide->multileg.ciProfileSchema = multileg.ciProfileSchema;
	pModelGuide->multileg.ciTouchLineType = multileg.ciTouchLineType;
	pModelGuide->multileg.siBendLineSpace = multileg.siBendLineSpace;
	pModelGuide->multileg.siBendLineG = multileg.siBendLineG;
	pModelGuide->multileg.siSpecThick = multileg.siSpecThick;
}

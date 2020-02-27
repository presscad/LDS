#include "stdafx.h"
#include "f_ent.h"
#include "XhCharString.h"

#ifdef __RAPID_ROUGH_3DMODEL_
#include "IntelliModel.h"
////////////////////////////////////////////////////////////////////////////////
//�ṹ����
STDTEMPL::DYNA_LINKROD::DYNA_LINKROD()
{
	widRod=widStartNode=widEndNode=0;	//��ʼ�ڵ㣬��ֹ�ڵ�
	ciNormalDirection=0;			//���ƹ������ߣ�����֫������ӽ��������ᣩX��Y��Z������Сд��ʾ���᷽��
	ciAngleLayout=0;				//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫����
	widRefRodOfWorkWing=0;		//����֫���ո˼�
}
STDTEMPL::CONNECT_ROD::CONNECT_ROD()
{
	widRod=0;
	dynaRod.widStartNode=dynaRod.widEndNode=0;
	dynaRod.siRayMinDegAngle=dynaRod.siRayMaxDegAngle=0;
	ciConnectWing=0;
	ciPlaneNo=1;
	dualSideEdge=false;
	cnMinBoltN=2;
	cnMaxSingularLineBolts=4;
	widCrossBoltAngle=0;
}
STDTEMPL::PLATE::PLATE()
{
	widBaseRod=0;			//�ְ�Ķ�λ��׼�Ǹ�
	widBaseNode=0;			//�ְ嶨λ�ڵ�
	ciDatumAngleWing='X';	//�ְ�Ķ�λ��׼�ǸֵĹ���֫��'X','Y'��
	widBendEdgeAngle1=0;	//��һ��������������߶�λ��׼�Ǹ֣���ְ巨���ڲ���Ϊ���������Ϊ������
	ciBendEdgeWing1='X';	//'X','x','Y','y':��д��ʾ�����߲࣬Сд��֫���
	fBendDegAngle1=0;		//����ǣ��������߻�׼�Ǹ�ʼ->��Ϊ����ʱ����תΪ��(�Ƕ��ƣ�0��ʾ�Զ�����)
	widBendEdgeAngle2=0;	//��һ��������������߶�λ��׼�Ǹ֣���ְ巨���ڲ���Ϊ���������Ϊ������
	ciBendEdgeWing2='X';	//'X','x','Y','y':��д��ʾ�����߲࣬Сд��֫���
	fBendDegAngle2=0;		//����ǣ��������߻�׼�Ǹ�ʼ->��Ϊ����ʱ����תΪ��(�Ƕ��ƣ�0��ʾ�Զ�����)

	ciCurveEdgeMode=0;	//0.��͹ʽ;1.����ʽ
	ciEdgeRadius=40;	//�ҿױ߾��Բ���뾶
	ciFilletRadius=10;//�ҿױ��ڰ��Ǵ�������Բ���뾶
}
void STDTEMPL::PLATE::CloneFrom(PLATE* pFrom)
{
	widBaseRod=pFrom->widBaseRod;		//�ְ�Ķ�λ��׼�Ǹ�
	widBaseNode=pFrom->widBaseNode;		//�ְ嶨λ�ڵ�
	ciDatumAngleWing=pFrom->ciDatumAngleWing;	//�ְ�Ķ�λ��׼�ǸֵĹ���֫��'X','Y'��
	widBendEdgeAngle1=pFrom->widBendEdgeAngle1;	//��һ��������������߶�λ��׼�Ǹ֣���ְ巨���ڲ���Ϊ���������Ϊ������
	ciBendEdgeWing1=pFrom->ciBendEdgeWing1;	//'X','x','Y','y':��д��ʾ�����߲࣬Сд��֫���
	fBendDegAngle1=pFrom->fBendDegAngle1;	//����ǣ��������߻�׼�Ǹ�ʼ->��Ϊ����ʱ����תΪ��(�Ƕ��ƣ�0��ʾ�Զ�����)
	widBendEdgeAngle2=pFrom->widBendEdgeAngle2;	//��һ��������������߶�λ��׼�Ǹ֣���ְ巨���ڲ���Ϊ���������Ϊ������
	ciBendEdgeWing2=pFrom->ciBendEdgeWing2;	//'X','x','Y','y':��д��ʾ�����߲࣬Сд��֫���
	fBendDegAngle2=pFrom->fBendDegAngle2;	//����ǣ��������߻�׼�Ǹ�ʼ->��Ϊ����ʱ����תΪ��(�Ƕ��ƣ�0��ʾ�Զ�����)
	//�����ְ����Ӹ˼�
	connrods.Clear();
	for(UINT i=0;i<pFrom->connrods.Count;i++)
		connrods.Set(i,pFrom->connrods.At(i),true);
	//���߰������Ϣ
	holes.Clear();
	for(UINT i=0;i<pFrom->holes.Count;i++)
		holes.Set(i,pFrom->holes.At(i),true);
	ciCurveEdgeMode=pFrom->ciCurveEdgeMode;	//0.��͹ʽ;1.����ʽ
	ciEdgeRadius=pFrom->ciEdgeRadius;	//�ҿױ߾��Բ���뾶
	ciFilletRadius=pFrom->ciFilletRadius;//�ҿױ��ڰ��Ǵ�������Բ���뾶
}
STDTEMPL::WIREANGLE::WIREANGLE()
{
	widBaseRod=0;			//�ְ�Ķ�λ��׼�Ǹ�
	widBaseNode=0;			//�ְ嶨λ�ڵ�
	ciLayer=0;				//����λ�ò㼶��0.��ʾ�붨λ�Ǹֽ�����Խ���ʾԽ���
	ciDatumAngleWing='X';	//�ְ�Ķ�λ��׼�ǸֵĹ���֫��'X','Y'��
	ciNormalOffset=0;		//0.��ʾ�Զ��ж�
	ciAngleLayout=0;		//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫�������������֫���߿�
	ciLenDirection='X';		//�̽Ǹ�ʼ->�ս��Ʒ�����ӽ��������ᣩX��Y��Z������Сд��ʾ���᷽��
	width=width2=140;
	thick=10;				//����֫����һ֫��֫��
}
void STDTEMPL::WIREANGLE::CloneFrom(WIREANGLE* pFrom)
{
	widAngle=pFrom->widAngle;
	widBaseRod=pFrom->widBaseRod;		//�ְ�Ķ�λ��׼�Ǹ�
	widBaseNode=pFrom->widBaseNode;		//�ְ嶨λ�ڵ�
	ciLayer=pFrom->ciLayer;	//����λ�ò㼶��0.��ʾ�붨λ�Ǹֽ�����Խ���ʾԽ���
	ciDatumAngleWing=pFrom->ciDatumAngleWing;	//�ְ�Ķ�λ��׼�ǸֵĹ���֫��'X','Y'��
	ciNormalOffset=pFrom->ciNormalOffset;	//0.��ʾ�Զ��ж�
	ciAngleLayout=pFrom->ciAngleLayout;	//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫�������������֫���߿�
	ciLenDirection=pFrom->ciLenDirection;//�̽Ǹ�ʼ->�ս��Ʒ�����ӽ��������ᣩX��Y��Z������Сд��ʾ���᷽��
	width=pFrom->width;
	width2=pFrom->width2;
	thick=pFrom->thick;	//����֫����һ֫��֫��
	//�������߿�
	holes.Clear();
	for(UINT i=0;i<pFrom->holes.Count;i++)
		holes.Set(i,pFrom->holes.At(i),true);
}
STDTEMPL::ANGLEPLATE::ANGLEPLATE()
{
	ciRibType=0;		//�Ǹּ�ǿ�߰�����
	ciLocationMode=0;	//0.�̽Ǹ��е�ʼ->�շ���ƫ��;'S':��ʼ���濪ʼ���ն�ƫ��;'E'���ն��濪ʼ��ʼ��ƫ��
	siLenOffset=0;
	thick=8;
}
////////////////////////////////////////////////////////////////////////////////
STDTEMPL::PROFILE_ROD::PROFILE_ROD()
{
	widRod=widStartNode=widEndNode=0;
	bHasSectView=false;	//�Ƿ����չ����
	ciRodType=ciSectFlag=0;
	ciAngleLayout=0;	//�ǸֵĲ��÷�ʽ1,2,3,4;<=0��ʾ��ָ��֫����
	widRefRodOfWorkWing=0;
	siMinRayDegAngle=siMaxRayDegAngle=0;
}
STDTEMPL::PROFILE_NODE::PROFILE_NODE()
{
	ciPosCalType=0;					//0.������;1.ָ��X ;2.ָ��Y;3.ָ��Z;4.��׼��ָ��XZ
	widNode=0;
	widLinkPrevRod=0;				//���ӵ�ǰһ�˼�Id;0.��ʾǰ���޸˼�����
	widMasterRod=0;					//�ڵ㸸�˼�
	siMinDegAngle=siMaxDegAngle=0;	//�ö��㴦ǰ�������߼�нǷ�Χ
	ciProfileNode=true;
	ciFractions=1;
}
STDTEMPL::STDTEMPL()
{
	m_wnProfileNodeCount=0;
	memset(name,0,51);
	ciOnRightSide='R';
	ciClassicType=0;
	//DYN_LINKROD
	//CONNECT_ROD
	//PLATE
	//WIREANGLE
	//ANGLEPLATE
	/*PROFILE_NODE* pParaNode;
	PROFILE_ROD* pRod;
	pParaNode=hashParaNodes.Add(1);
	pParaNode=hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 80;
	pParaNode->siMaxDegAngle=100;
	pParaNode=hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle= 90;
	pParaNode->siMaxDegAngle=120;
	pParaNode=hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=1;
	//pParaNode->widMasterRod=0;
	//pParaNode->siMinDegAngle= 90;
	//pParaNode->siMaxDegAngle=120;
	pRod=hashParaRods.Add(1);	//��������
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=hashParaRods.Add(2);	//�ײ�����
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod=hashParaRods.Add(3);	//��˲�����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->bHasSectView=true;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	PROFILE_QUADSECT* pQuadSect=listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect=listQuadSects.AttachObject();
	pQuadSect->arrRods[1]=2;*/
}
WORD STDTEMPL::get_ProfileNodeCount()
{
	if(m_wnProfileNodeCount<=0)
	{
		m_wnProfileNodeCount=0;
		for(PROFILE_NODE* pTmplNode=hashParaNodes.GetFirst();pTmplNode;pTmplNode=hashParaNodes.GetNext())
		{
			if(pTmplNode->ciProfileNode>0)
				m_wnProfileNodeCount++;
		}
	}
	return m_wnProfileNodeCount;
}
void STDTEMPL::Empty(){
	hashParaRods.Empty();
	hashParaNodes.Empty();
	hashParaPlanes.Empty();
	listQuadSects.DeleteList();
}
void STDTEMPL::CopyStdTemplFrom(STDTEMPL* pStdTempl)
{
	StrCopy(name,pStdTempl->name,50);
	ciClassicType=pStdTempl->ciClassicType;
	ciOnRightSide=pStdTempl->ciOnRightSide;
	Empty();
	for(PROFILE_ROD* pParaRod=pStdTempl->hashParaRods.GetFirst();pParaRod;pParaRod=pStdTempl->hashParaRods.GetNext())
		hashParaRods.SetValue(pParaRod->widRod,*pParaRod);
	for(PROFILE_NODE* pParaNode=pStdTempl->hashParaNodes.GetFirst();pParaNode;pParaNode=pStdTempl->hashParaNodes.GetNext())
		hashParaNodes.SetValue(pParaNode->widNode,*pParaNode);
	for(DATUM_PLANE* pDatumPlane=pStdTempl->hashParaPlanes.GetFirst();pDatumPlane;pDatumPlane=pStdTempl->hashParaPlanes.GetNext())
		hashParaPlanes.SetValue(pDatumPlane->widPlane,*pDatumPlane);
	for(PROFILE_QUADSECT* pQuadSect=pStdTempl->listQuadSects.EnumObjectFirst();pQuadSect;pQuadSect=pStdTempl->listQuadSects.EnumObjectNext())
		listQuadSects.Append(*pQuadSect);
	for(DYNA_LINKROD* pDynaRod=pStdTempl->hashDynaLinkRods.GetFirst();pDynaRod;pDynaRod=pStdTempl->hashDynaLinkRods.GetNext())
		hashDynaLinkRods.Append(*pDynaRod);
	for(WIREANGLE* pWireAngle=pStdTempl->hashWireAngles.GetFirst();pWireAngle;pWireAngle=pStdTempl->hashWireAngles.GetNext())
	{
		WIREANGLE* pNewWireAngle=hashWireAngles.Append(pWireAngle->widAngle);
		pNewWireAngle->CloneFrom(pWireAngle);
	}
	for(PLATE* pConnPlate=pStdTempl->listParaPlates.EnumObjectFirst();pConnPlate;pConnPlate=pStdTempl->listParaPlates.EnumObjectNext())
	{
		PLATE* pNewPlate=listParaPlates.Append();
		pNewPlate->CloneFrom(pConnPlate);
	}
	for(ANGLEPLATE* pAnglePlate=pStdTempl->listAnglePlates.EnumObjectFirst();pAnglePlate;pAnglePlate=pStdTempl->listAnglePlates.EnumObjectNext())
		listAnglePlates.Append(*pAnglePlate);
}

#endif
#include "stdafx.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "objptr_list.h"
#include "LogFile.h"
#include "IntelliCoreObject.h"
#ifdef __PUBLIC_RELEASE_COMPILE_
#include "../../env_def.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef __RAPID_ROUGH_3DMODEL_

class CMaxDouble{
	bool inited;
public:
	double number;
	void* m_pRelaObj;
	CMaxDouble(){inited=false;m_pRelaObj=NULL;}
	CMaxDouble(double init_val,void* pRelaObj=NULL){inited=true;number=init_val;m_pRelaObj=pRelaObj;}
	operator double(){return number;}
	bool IsInited() const{return inited;}
	double Update(double val,void* pRelaObj=NULL,double tolerance=0)
	{
		if(!inited)
		{
			number=val;
			inited=true;
			m_pRelaObj=pRelaObj;
		}
		else if(number<val-tolerance)
		{
			number=val;
			m_pRelaObj=pRelaObj;
		}
		return number;
	}
};
////////////////////////////////////////////////////////////////////////////////
int CIntelliCore::PREDEF_ARM_COUNT;//=20;
int CIntelliCore::InitClassicArmTemplArr()
{
	STDTEMPL::PROFILE_NODE* pParaNode;
	STDTEMPL::PROFILE_ROD* pRod;
	STDTEMPL* pStdTempl;
	int i;
	PREDEF_ARM_COUNT=27;
	ARM_ARR.SetSize(0,PREDEF_ARM_COUNT+5);
	//for(i=0;i<PREDEF_ARM_COUNT;i++)
	//	ARM_ARR[i].Empty();
	ARM_ARR.Empty();
	for(i=0;i<PREDEF_ARM_PART_COUNT;i++)
		ARM_PART_ARR[i].Empty();
	STDTEMPL stdtempl;
	//"HX1"
	pStdTempl=ARM_ARR.append();//[0];
	pStdTempl->Empty();
	memcpy(pStdTempl->name,"HX1",3);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->wirecode.ciWireType='E';	//���߹ҵ�
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=10;	//�г��ּн�Ϊ17��������� wjh-2018.5.23
	pParaNode->siMaxDegAngle=45;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=1;
	//pParaNode->widMasterRod=0;
	pRod=pStdTempl->hashParaRods.Add(1);	//����ˮƽ����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->ciRodType='H';
	pRod=pStdTempl->hashParaRods.Add(2);	//б����
	pRod->widStartNode=2;
	pRod->widEndNode  =1;
	pRod=pStdTempl->hashParaRods.Add(3);	//�ҵ���֧��T����ϽǸ�
	pRod->widStartNode= 2;
	pRod->widEndNode  =-2;
	pRod->ciNearNormAxis=6;
	pRod->ciAngleLayout=1;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	STDTEMPL::PROFILE_QUADSECT* pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"HX1A"
	pStdTempl=ARM_ARR.append();
	pStdTempl->Empty();
	memcpy(pStdTempl->name,"HX1A",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=245;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->wirecode.ciWireType='J';	//���߹ҵ�
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=80;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->wirecode.ciWireType='E';	//���߹ҵ�
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=1;
	//pParaNode->widMasterRod=0;
	pRod=pStdTempl->hashParaRods.Add(1);	//����ˮƽ����
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod->ciRodType='H';
	pRod=pStdTempl->hashParaRods.Add(2);	//б����
	pRod->widStartNode=2;
	pRod->widEndNode  =1;
	pRod=pStdTempl->hashParaRods.Add(3);	//б���Ҹ�
	pRod->widStartNode=2;
	pRod->widEndNode  =3;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=2;
	pRod=pStdTempl->hashParaRods.Add(4);	//���߼������ֱ�Ҹ�
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(5);	//б�����ϰ벿
	pRod->widStartNode=4;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(6);	//�����߼�T����ϽǸ�
	pRod->widStartNode= 3;
	pRod->widEndNode  =-3;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=4;
	pRod->ciSectStyle=2;
	/* ���߼�ͼֽ����ʱ��ͨ�������Ұ壬����ҪT����ϽǸ� wjh-2018.7.6
	pRod=pStdTempl->hashParaRods.Add(6);	//�ҵ���֧��T����ϽǸ�
	pRod->widStartNode= 4;
	pRod->widEndNode  =-4;
	pRod->ciNearNormAxis=6;
	pRod->ciAngleLayout=1;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;*/
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"HX2"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"HX2",3);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->wirecode.ciWireType='C';	//���߹ҵ�
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=10;	//�г��ּн�Ϊ17��������� wjh-2018.5.23
	pParaNode->siMaxDegAngle=45;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=1;
	//pParaNode->widMasterRod=0;
	pRod=pStdTempl->hashParaRods.Add(1);	//б����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(2);	//�ײ�ˮƽ����
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod->ciRodType='H';
	pRod=pStdTempl->hashParaRods.Add(3);	//�ҵ���֧��T����ϽǸ�
	pRod->widStartNode= 2;
	pRod->widEndNode  =-2;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"PART-HX2"
	pStdTempl=&ARM_PART_ARR[0];
	memcpy(pStdTempl->name,"PART-HX2",8);
	pStdTempl->ciOnRightSide='P';	//��ͷ��ᵣ�ľֲ����͹���
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->wirecode.ciWireType='C';	//���߹ҵ�
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=10;	//�г��ּн�Ϊ17��������� wjh-2018.5.23
	pParaNode->siMaxDegAngle=45;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=1;
	//pParaNode->widMasterRod=0;
	pRod=pStdTempl->hashParaRods.Add(1);	//б����
	pParaNode->siMinDegAngle=330;	//3-2�˼�������X����С��ʼ�н�
	pParaNode->siMaxDegAngle=350;	//3-2�˼�������X�������ʼ�н�
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(2);	//�ײ�ˮƽ����
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod->siMinRayDegAngle=0;	//1-2�˼�������X����С��ʼ�н�
	pRod->siMaxRayDegAngle=5;	//1-2�˼�������X�������ʼ�н�
	pRod->ciRodType='H';
	pRod=pStdTempl->hashParaRods.Add(3);	//�ҵ���֧��T����ϽǸ�
	pRod->widStartNode= 2;
	pRod->widEndNode  =-2;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"HX3"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"HX3",3);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 85;
	pParaNode->siMaxDegAngle=95;
	pParaNode->wirecode.ciWireType='C';
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle= 95;
	pParaNode->siMaxDegAngle=120;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=1;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=5;
	pParaNode->raynode.widOrgNode=3;
	pParaNode->raynode.widEdgeStart=1;
	pParaNode->raynode.widEdgeEnd=2;
	pParaNode->raynode.siMinRayAngle=220;
	pParaNode->raynode.siMaxRayAngle=260;

	pRod=pStdTempl->hashParaRods.Add(1);	//��������
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(2);	//�ײ�����
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(3);	//��˲�����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->bHasSectView=true;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"PART-HX3"
	pStdTempl=&ARM_PART_ARR[1];
	memcpy(pStdTempl->name,"PART-HX3",8);
	pStdTempl->ciOnRightSide='P';	//��ͷ��ᵣ�ľֲ����͹���
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 85;
	pParaNode->siMaxDegAngle=95;
	pParaNode->wirecode.ciWireType='C';
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle= 85;
	pParaNode->siMaxDegAngle=120;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=1;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=5;
	pParaNode->raynode.widOrgNode=3;
	pParaNode->raynode.widEdgeStart=1;
	pParaNode->raynode.widEdgeEnd=2;
	pParaNode->raynode.siMinRayAngle=220;
	pParaNode->raynode.siMaxRayAngle=260;
	pRod=pStdTempl->hashParaRods.Add(1);	//��������
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(2);	//�ײ�����
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod->siMinRayDegAngle=0;
	pRod->siMaxRayDegAngle=5;
	pRod=pStdTempl->hashParaRods.Add(3);	//��˲�����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->bHasSectView=true;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;	//
	//"HX3A"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"HX3A",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 25;
	pParaNode->siMaxDegAngle=50;
	pParaNode->wirecode.ciWireType='J';
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle= 120;
	pParaNode->siMaxDegAngle=170;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=1;
	//pParaNode->widMasterRod=0;
	//pParaNode->siMinDegAngle= 90;
	//pParaNode->siMaxDegAngle=120;
	pRod=pStdTempl->hashParaRods.Add(1);	//��������
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(2);	//�ײ�����
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(3);	//��˲�����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->bHasSectView=true;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"PART-HX3A"
	pStdTempl=&ARM_PART_ARR[2];
	memcpy(pStdTempl->name,"PART-HX3A",9);
	pStdTempl->ciOnRightSide='P';	//��ͷ��ᵣ�ľֲ����͹���
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 25;
	pParaNode->siMaxDegAngle=50;
	pParaNode->wirecode.ciWireType='J';
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle= 120;
	pParaNode->siMaxDegAngle=170;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=1;
	//pParaNode->widMasterRod=0;
	//pParaNode->siMinDegAngle= 90;
	//pParaNode->siMaxDegAngle=120;
	pRod=pStdTempl->hashParaRods.Add(1);	//��������
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(2);	//�ײ�����
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod->siMinRayDegAngle=0;
	pRod->siMaxRayDegAngle=5;
	pRod=pStdTempl->hashParaRods.Add(3);	//��˲�����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->bHasSectView=true;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"HX4"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"HX4",3);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 95;
	pParaNode->siMaxDegAngle=120;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle= 85;
	pParaNode->siMaxDegAngle=95;
	pParaNode->wirecode.ciWireType='E';
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=1;
	//pParaNode->widMasterRod=0;
	//pParaNode->siMinDegAngle= 90;
	//pParaNode->siMaxDegAngle=120;
	pRod=pStdTempl->hashParaRods.Add(1);	//��������
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(2);	//�ײ�����
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(3);	//��˲�����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->bHasSectView=true;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"NJ3"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"NJ3",3);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle= 85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=220;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=220;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=220;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=50;
	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=9;
	pParaNode->siMinDegAngle=80;
	pParaNode->siMaxDegAngle=100;
	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=10;
	pParaNode->siMinDegAngle=150;
	pParaNode->siMaxDegAngle=180;
	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->widLinkPrevRod=10;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=235;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(11);
	pParaNode=pStdTempl->hashParaNodes.Add(12);
	pParaNode->ciProfileNode=false;	//��������������
	pParaNode->ciPosCalType=5;	//���߽����
	pParaNode->raynode.widOrgNode=5;
	pParaNode->raynode.widEdgeStart=1;
	pParaNode->raynode.widEdgeEnd=2;
	pParaNode->raynode.siMinRayAngle=260;
	pParaNode->raynode.siMaxRayAngle=280;

	pRod=pStdTempl->hashParaRods.Add(1);	//��������
	pRod->widStartNode= 1;
	pRod->widEndNode  =12;
	pRod=pStdTempl->hashParaRods.Add(2);	//��������
	pRod->widStartNode=12;
	pRod->widEndNode  = 2;
	pRod=pStdTempl->hashParaRods.Add(3);	//��˲����߼�����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(4);	//���߼�ƽ��
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(5);	//�ºᵣ����
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(6);	//���߼�����֧�Ÿ�
	pRod->widStartNode=6;
	pRod->widEndNode  =5;
	pRod=pStdTempl->hashParaRods.Add(7);	//���߼ܼ��ƽ��
	pRod->widStartNode=6;
	pRod->widEndNode  =7;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(8);	//���߼ܼ���Ҳ��
	pRod->widStartNode=8;
	pRod->widEndNode  =7;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=7;
	pRod=pStdTempl->hashParaRods.Add(9);	//���߼ܼ������
	pRod->widStartNode=8;
	pRod->widEndNode  =9;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=7;
	//Ԥ��˼�λ����Ϣ
	pRod->desPosStart.blPresetParams=true;
	pRod->desPosStart.cTouchLineType=4;	//�����ָ����׼��
	pRod->desPosStart.datumpoint.widPlane=1;	//�����1�Ż�׼��
	pRod->desPosStart.datumpoint.ciCoordType=1;	//0.ָ��Y-Z����;1.ָ��X-Z����;2.ָ��X-Y����
	pRod->desPosStart.szXorCtrlWingOffset[0]='g';	//һ��ȡֵ��"g","g1","g2","40","-30"
	pRod->desPosStart.szYorOthrWingOffset[0]='0';
	pRod=pStdTempl->hashParaRods.Add(10);	//���߼�����֧�Ÿ�
	pRod->widStartNode=9;
	pRod->widEndNode  =10;
	//Ԥ��˼�λ����Ϣ
	pRod->desPosStart.blPresetParams=true;
	pRod->desPosStart.cTouchLineType=4;	//�����ָ����׼��
	pRod->desPosStart.datumpoint.widPlane=1;	//�����1�Ż�׼��
	pRod->desPosStart.datumpoint.ciCoordType=1;	//0.ָ��Y-Z����;1.ָ��X-Z����;2.ָ��X-Y����
	pRod->desPosStart.szXorCtrlWingOffset[0]='g';	//һ��ȡֵ��"g","g1","g2","40","-30"
	pRod->desPosStart.szYorOthrWingOffset[0]='0';

	pRod=pStdTempl->hashParaRods.Add(11);	//���߼�����֧�Ÿ�
	pRod->widStartNode=11;
	pRod->widEndNode  =10;
	//Ԥ��˼�λ����Ϣ
	pRod->desPosStart.blPresetParams=true;
	pRod->desPosStart.cTouchLineType=4;	//�����ָ����׼��
	pRod->desPosStart.datumpoint.widPlane=1;	//�����1�Ż�׼��
	pRod->desPosStart.datumpoint.ciCoordType=1;	//0.ָ��Y-Z����;1.ָ��X-Z����;2.ָ��X-Y����
	pRod->desPosStart.szXorCtrlWingOffset[0]='0';	//һ��ȡֵ��"g","g1","g2","40","-30"
	pRod->desPosStart.szYorOthrWingOffset[0]='g';

	pRod=pStdTempl->hashParaRods.Add(12);	//
	pRod->widStartNode=5;
	pRod->widEndNode  =12;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(13);	//����֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 7;
	pRod->widEndNode  =-7;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=7;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	STDTEMPL::DATUM_PLANE* pSolidPlane=pStdTempl->hashParaPlanes.Add(1);
	pSolidPlane->points[0].ciType='S';
	pSolidPlane->points[0].widNodeOrRod=1;
	pSolidPlane->points[1].ciType='E';
	pSolidPlane->points[1].widNodeOrRod=2;
	pSolidPlane->points[2].ciType='E';
	pSolidPlane->points[2].widNodeOrRod=7;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=11;
	pQuadSect->arrRods[1]=1;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=5;
	pQuadSect->arrRods[1]=2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=10;
	pQuadSect->arrRods[1]=6;
	//"NJ4"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"NJ4",3);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=1;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=220;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle= 90;
	pParaNode->siMaxDegAngle=110;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=10;
	pParaNode->siMinDegAngle=230;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=6;
	pParaNode->siMinDegAngle=230;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=6;
	pParaNode->siMinDegAngle=25;
	pParaNode->siMaxDegAngle=50;
	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=80;
	pParaNode->siMaxDegAngle=120;
	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=9;
	pParaNode->siMinDegAngle=150;
	pParaNode->siMaxDegAngle=180;
	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=10;
	pParaNode->siMinDegAngle=235;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(11);
	//���Ӹ˼�
	pRod=pStdTempl->hashParaRods.Add(1);	//�ײ�ˮƽ�Ҹ�
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(2);	//�ײ�����ᵣ�²��Ҹ�
	pRod->widStartNode=2;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(4);	//����ᵣ�ϲ��Ҹ�
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(5);	//���߼����֧���Ҹ�
	pRod->widStartNode=6;
	pRod->widEndNode  =5;
	pRod=pStdTempl->hashParaRods.Add(6);	//���߼�ˮƽ�Ҹ�
	pRod->widStartNode=6;
	pRod->widEndNode  =7;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(7);	//��˲�����
	pRod->widStartNode=8;
	pRod->widEndNode  =7;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->widRefRodOfWorkWing=6;
	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode=8;
	pRod->widEndNode  =9;
	pRod->widRefRodOfWorkWing=6;
	pRod->ciAngleLayout=2;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode=9;
	pRod->widEndNode  =10;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode=11;
	pRod->widEndNode  =10;
	pRod=pStdTempl->hashParaRods.Add(11);	//����֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 7;
	pRod->widEndNode  =-7;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=10;
	pQuadSect->arrRods[1]=1;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=4;
	pQuadSect->arrRods[1]=2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=9;
	pQuadSect->arrRods[1]=5;
	//"HX4A"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"HX4A",5);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 95;
	pParaNode->siMaxDegAngle=160;
	pParaNode->wirecode.ciWireType='C';
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle= 10;
	pParaNode->siMaxDegAngle=80;
	pParaNode->wirecode.ciWireType='E';
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=1;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=5;
	pParaNode->raynode.widOrgNode=2;
	pParaNode->raynode.widEdgeStart=4;
	pParaNode->raynode.widEdgeEnd=3;
	pParaNode->raynode.siMinRayAngle=80;
	pParaNode->raynode.siMaxRayAngle=100;

	pRod=pStdTempl->hashParaRods.Add(1);	//��������
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(2);	//�ײ�����
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(3);	//��˲���
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(4);	//���߹ҵ�����
	pRod->widStartNode=5;
	pRod->widEndNode  =2;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(5);	//���߹��߽Ǹֵ�����
	pRod->widStartNode= 3;
	pRod->widEndNode  =-3;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=4;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(6);	//���߹��߽Ǹֵ�����
	pRod->widStartNode= 2;
	pRod->widEndNode  =-2;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"HX7"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"HX7",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=260;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=1;

	pRod=pStdTempl->hashParaRods.Add(1);	//�ᵣ���Ҹ�
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(2);	//���Ҹ�
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod->ciRodType='H';
	pRod=pStdTempl->hashParaRods.Add(3);	//���ߺᵣ�Ҳ�����
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->bHasSectView=true;
	pRod->widRefRodOfWorkWing=4;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(4);	//С���ߺᵣ���Ҹ�
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod->ciRodType='H';

	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 4;
	pQuadSect->arrRods[1]=-2;	//�����գ�������
	//"HYJ1"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"HYJ1",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=235;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=3;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=95;	//����ᵣ4-3��Ϊ��ֱ�ˣ���ʱ��3=90��
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=220;	//����ᵣ4-3��Ϊ��ֱ�ˣ���ʱ180��<��4<270��
	pParaNode->siMaxDegAngle=320;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=6;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=235;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=6;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=55;
	pParaNode->wirecode.ciWireType='E';
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=80;
	pParaNode->siMaxDegAngle=130;
	pParaNode=pStdTempl->hashParaNodes.Add(8);

	pRod=pStdTempl->hashParaRods.Add(1);	//��Ǻᵣ���Ҹ�
	pRod->widStartNode=7;
	pRod->widEndNode  =8;
	pRod=pStdTempl->hashParaRods.Add(2);	//���Ҹ��°벿
	pRod->widStartNode=2;
	pRod->widEndNode  =1;
	pRod=pStdTempl->hashParaRods.Add(3);	//С���ߺᵣ���Ҹ�
	pRod->widStartNode=2;
	pRod->widEndNode  =3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(4);	//С���ߺᵣ���Ҹ�
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode=5;
	pRod->widEndNode  =6;
	pRod->bHasSectView=true;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciAngleLayout=2;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode=7;
	pRod->widEndNode  =6;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(8);	//����֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 6;
	pRod->widEndNode  =-6;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(9);	//���߼�T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 3;
	pRod->widEndNode  =-3;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=3;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=3;
	pQuadSect->arrRods[1]=4;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=-1;	//�����գ�������
	pQuadSect->arrRods[1]=5;
	//"PART-HYJ1"
	pStdTempl=&ARM_PART_ARR[3];
	memcpy(pStdTempl->name,"PART-YJ1",8);
	pStdTempl->ciOnRightSide='P';	//��ͷ��ᵣ�ľֲ����͹���
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=3;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=235;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=3;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=55;
	pParaNode->wirecode.ciWireType='E';
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=80;
	pParaNode->siMaxDegAngle=130;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	//�߽�˼�
	pRod=pStdTempl->hashParaRods.Add(1);	//��Ǻᵣ���߼����Ҹ�
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod->siMinRayDegAngle=20;
	pRod->siMaxRayDegAngle=60;
	pRod=pStdTempl->hashParaRods.Add(2);	//���Ҹ�
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod->siMinRayDegAngle=25;
	pRod->siMaxRayDegAngle=65;
	pRod=pStdTempl->hashParaRods.Add(3);	//���ߺᵣˮƽ��
	pRod->widStartNode=2;
	pRod->widEndNode  =3;
	pRod->siMinRayDegAngle=0;
	pRod->siMaxRayDegAngle=5;
	pRod->bHasSectView=true;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(4);	//���߼�б����
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(5);	//����֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 3;
	pRod->widEndNode  =-3;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=3;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"PART-PDX",ƽ�����߼�
	pStdTempl=&ARM_PART_ARR[4];
	memcpy(pStdTempl->name,"PART-PDX",8);
	pStdTempl->ciOnRightSide='P';	//��ͷ��ᵣ�ľֲ����͹���
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=20;
	pParaNode->siMaxDegAngle=50;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=125;
	pParaNode->siMaxDegAngle=155;
	pParaNode=pStdTempl->hashParaNodes.Add(4);

	pRod=pStdTempl->hashParaRods.Add(1);	//ƽ�����߼ܣ������ھƱ�����
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod->siMinRayDegAngle=25;
	pRod->siMaxRayDegAngle=65;
	pRod=pStdTempl->hashParaRods.Add(2);	//���Ҹ�
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod->siMinRayDegAngle=20;
	pRod->siMaxRayDegAngle=60;
	pRod=pStdTempl->hashParaRods.Add(3);	//���ߺᵣˮƽ��
	pRod->widStartNode=2;
	pRod->widEndNode  =3;
	pRod->ciRodType='H';
	pRod->siMinRayDegAngle=0;
	pRod->siMaxRayDegAngle=5;
	pRod->bHasSectView=true;
	pRod->widRefRodOfWorkWing=-3;
	pRod->ciAngleLayout=1;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	//"HD1"
	pStdTempl=ARM_ARR.append();
	AddHeadType_HD1(pStdTempl);
	//"5D1"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"5D1",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=280;
	pParaNode->siMaxDegAngle=340;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=25;
	pParaNode->siMaxDegAngle=60;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=110;
	pParaNode->siMaxDegAngle=160;
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=250;
	pParaNode=pStdTempl->hashParaNodes.Add(8);

	pRod=pStdTempl->hashParaRods.Add(1);	//�ᵣ���Ҹ�
	pRod->widStartNode=8;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(2);	//���Ҹ��°벿
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(4);	//С���ߺᵣ���Ҹ�
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode=6;
	pRod->widEndNode  =5;
	pRod->bHasSectView=true;
	pRod->widRefRodOfWorkWing=-6;
	pRod->ciAngleLayout=1;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode=6;
	pRod->widEndNode  =7;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 4;
	pQuadSect->arrRods[1]=-2;	//�����գ�������
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=7;
	pQuadSect->arrRods[1]=5;
	//"YJ1"
	pStdTempl=ARM_ARR.append();
	pStdTempl->ciClassicType='Y';
	pStdTempl->ciOnRightSide='U';	//��ͷ����
	memcpy(pStdTempl->name,"YJ1",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=1;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=250;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=20;
	pParaNode->siMaxDegAngle=70;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=3;
	//pParaNode->widMasterRod=0;
	pParaNode->siMinDegAngle=260;
	pParaNode->siMaxDegAngle=300;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=240;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=20;
	pParaNode->siMaxDegAngle=60;
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=130;
	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=250;
	//����ֽ���
	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=9;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=250;
	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=9;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=130;
	pParaNode=pStdTempl->hashParaNodes.Add(11);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=10;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=20;
	pParaNode->siMaxDegAngle=60;
	pParaNode=pStdTempl->hashParaNodes.Add(12);
	pParaNode->widLinkPrevRod=11;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=240;
	pParaNode=pStdTempl->hashParaNodes.Add(13);
	pParaNode->widLinkPrevRod=12;
	//pParaNode->widMasterRod=12;
	pParaNode->siMinDegAngle=260;
	pParaNode->siMaxDegAngle=300;
	pParaNode=pStdTempl->hashParaNodes.Add(14);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=13;
	pParaNode->widMasterRod=14;
	pParaNode->siMinDegAngle=20;
	pParaNode->siMaxDegAngle=70;
	pParaNode=pStdTempl->hashParaNodes.Add(15);
	pParaNode->widLinkPrevRod=15;
	pParaNode->widMasterRod=15;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=250;
	pParaNode=pStdTempl->hashParaNodes.Add(16);

	pRod=pStdTempl->hashParaRods.Add(1);
	pRod->widStartNode=2;
	pRod->widEndNode  =1;
	pRod=pStdTempl->hashParaRods.Add(2);	//���Ҹ��°벿
	pRod->widStartNode=2;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	//pRod->ciAngleLayout=3;
	//pRod->widRefRodOfWorkWing=2;
	//pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(4);	//С���ߺᵣ���Ҹ�
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode=5;
	pRod->widEndNode  =6;
	pRod->widRefRodOfWorkWing=7;
	pRod->ciAngleLayout=2;	//ָ���Ǹ�֫����
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode=7;
	pRod->widEndNode  =6;
	//pRod->bHasSectView=true;
	pRod->widRefRodOfWorkWing=7;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode=7;
	pRod->widEndNode  =8;
	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode=9;
	pRod->widEndNode  =8;
	pRod->widRefRodOfWorkWing=7;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode=10;
	pRod->widEndNode  =9;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode=10;
	pRod->widEndNode  =11;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=2;	//ָ���Ǹ�֫����
	pRod=pStdTempl->hashParaRods.Add(11);
	pRod->widStartNode=12;
	pRod->widEndNode  =11;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(12);
	pRod->widStartNode=12;
	pRod->widEndNode  =13;
	pRod=pStdTempl->hashParaRods.Add(13);
	pRod->widStartNode=13;
	pRod->widEndNode  =14;
	pRod=pStdTempl->hashParaRods.Add(14);
	pRod->widStartNode=15;
	pRod->widEndNode  =14;
	pRod=pStdTempl->hashParaRods.Add(15);
	pRod->widStartNode=15;
	pRod->widEndNode  =16;
	pRod=pStdTempl->hashParaRods.Add(16);	//�ҵ���֧��T����ϽǸ�
	pRod->widStartNode= 6;
	pRod->widEndNode  =-6;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=5;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(17);	//�����֧��T����ϽǸ�
	pRod->widStartNode=11;
	pRod->widEndNode  =-11;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=11;
	pRod->ciSectStyle=2;
	pRod=pStdTempl->hashParaRods.Add(18);	//�ҵ���֧��T����ϽǸ�
	pRod->widStartNode= 3;
	pRod->widEndNode  =-3;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=2;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(19);	//����֧��T����ϽǸ�
	pRod->widStartNode=11;
	pRod->widEndNode  =-11;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=14;
	pRod->ciSectStyle=2;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=15;
	pQuadSect->arrRods[1]= 2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 4;
	pQuadSect->arrRods[1]= 7;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 9;
	pQuadSect->arrRods[1]=12;	//�����գ�������
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=3;
	pQuadSect->arrRods[1]=2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=13;
	pQuadSect->arrRods[1]=14;
	//"NJ1"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"NJ1",3);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 95;
	pParaNode->siMaxDegAngle=140;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle= 200;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle=20;
	pParaNode->siMaxDegAngle=60;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=16;
	pParaNode->siMinDegAngle=80;
	pParaNode->siMaxDegAngle=120;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=200;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(7);

	pRod=pStdTempl->hashParaRods.Add(1);	//��������
	pRod->widStartNode= 7;
	pRod->widEndNode  = 6;
	pRod=pStdTempl->hashParaRods.Add(2);	//��������
	pRod->widStartNode= 1;
	pRod->widEndNode  = 2;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(4);
	pRod->widStartNode=3;
	pRod->widEndNode  =4;
	pRod->bHasSectView=true;
	pRod->ciAngleLayout=2;	//ָ���Ǹ�֫����
	pRod->widRefRodOfWorkWing=6;
	pRod=pStdTempl->hashParaRods.Add(5);	//�ºᵣ����
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod->bHasSectView=true;
	pRod->ciAngleLayout=3;	//ָ���Ǹ�֫����
	pRod->widRefRodOfWorkWing=6;
	pRod=pStdTempl->hashParaRods.Add(6);	//���߼�����֧�Ÿ�
	pRod->widStartNode=5;
	pRod->widEndNode  =6;
	pRod=pStdTempl->hashParaRods.Add(7);	//����֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 4;
	pRod->widEndNode  =-4;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=4;
	pRod->widRefRodOfWorkWing=4;
	pRod->ciSectStyle=2;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=6;
	pQuadSect->arrRods[1]=3;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=10;
	pQuadSect->arrRods[1]=6;
	//"NJ2"
	pStdTempl=ARM_ARR.append();
	memcpy(pStdTempl->name,"NJ2",3);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=250;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle= 10;
	pParaNode->siMaxDegAngle=40;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=110;
	pParaNode->siMaxDegAngle=160;
	pParaNode=pStdTempl->hashParaNodes.Add(5);

	pRod=pStdTempl->hashParaRods.Add(1);	//��������
	pRod->widStartNode= 5;
	pRod->widEndNode  = 4;
	pRod=pStdTempl->hashParaRods.Add(2);	//��������
	pRod->widStartNode= 1;
	pRod->widEndNode  = 2;
	pRod=pStdTempl->hashParaRods.Add(3);	//���Ҽ������
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=pStdTempl->hashParaRods.Add(4);	//���Ҽ������
	pRod->widStartNode=2;
	pRod->widEndNode  =3;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect->arrRods[1]=2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=3;
	pQuadSect->arrRods[1]=4;
	//"ZB1"
	pStdTempl=ARM_ARR.append();
	AddHeadType_ZB1(pStdTempl);
	//"ZB-ZJC"
	pStdTempl=ARM_ARR.append();
	AddHeadType_ZBnZJC(pStdTempl);
	//"ZB2"
	pStdTempl=ARM_ARR.append();
	AddHeadType_ZB2(pStdTempl);
	//"ZB2A"
	pStdTempl=ARM_ARR.append();
	AddHeadType_ZB2A(pStdTempl);
	//"ZB3"
	pStdTempl=ARM_ARR.append();
	pStdTempl->ciClassicType='B';	//�Ʊ���
	pStdTempl->ciOnRightSide='U';	//��ͷ����
	memcpy(pStdTempl->name,"ZB3",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->ciProfileNode=2;	//�Ǳ���������
	pParaNode->ciPosCalType=20;	//�Ʊ����������۽�����Ҳ�
	pParaNode->widLinkPrevRod=1;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=160;
	pParaNode->siMaxDegAngle=185;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->ciPosCalType=22;	//�Ʊ���������һ��ᵣX+������ӽڵ�
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=20;
	pParaNode->siMinDegAngle=240;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=3;
	pParaNode->siMinDegAngle=15;
	pParaNode->siMaxDegAngle=40;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=22;
	pParaNode->siMinDegAngle=240;
	pParaNode->siMaxDegAngle=320;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=55;
	pParaNode->siMaxDegAngle=85;	//230��
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=110;
	pParaNode->siMaxDegAngle=150;
	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=260;
	//����ֽ���
	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=260;
	pParaNode=pStdTempl->hashParaNodes.Add(11);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=9;
	pParaNode->siMinDegAngle=110;
	pParaNode->siMaxDegAngle=150;
	pParaNode=pStdTempl->hashParaNodes.Add(12);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=10;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=55;
	pParaNode->siMaxDegAngle=85;
	pParaNode=pStdTempl->hashParaNodes.Add(13);
	pParaNode->widLinkPrevRod=11;
	pParaNode->widMasterRod=23;
	pParaNode->siMinDegAngle=240;
	pParaNode->siMaxDegAngle=320;
	pParaNode=pStdTempl->hashParaNodes.Add(14);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=12;
	pParaNode->widMasterRod=13;
	pParaNode->siMinDegAngle=15;
	pParaNode->siMaxDegAngle=40;
	pParaNode=pStdTempl->hashParaNodes.Add(15);
	pParaNode->ciPosCalType=-22;	//�Ʊ���������һ��ᵣX-������ӽڵ�
	pParaNode->widLinkPrevRod=13;
	pParaNode->widMasterRod=21;
	pParaNode->siMinDegAngle=240;
	pParaNode->siMaxDegAngle=270;
	pParaNode=pStdTempl->hashParaNodes.Add(16);
	pParaNode->ciProfileNode=2;	//�Ǳ���������
	pParaNode->ciPosCalType=-20;	//�Ʊ����������۽�����Ҳ�
	pParaNode->widLinkPrevRod=14;
	pParaNode->widMasterRod=15;
	pParaNode->siMinDegAngle=160;
	pParaNode->siMaxDegAngle=185;
	pParaNode=pStdTempl->hashParaNodes.Add(17);
	//��������
	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 21;
	pParaNode=pStdTempl->hashParaNodes.Add(18);
	pParaNode->widMasterRod=20;
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 23;
	pParaNode=pStdTempl->hashParaNodes.Add(19);
	pParaNode->widMasterRod=21;
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-23;

	pRod=pStdTempl->hashParaRods.Add(1);
	pRod->widStartNode=2;
	pRod->widEndNode  =1;
	pRod=pStdTempl->hashParaRods.Add(2);	//���Ҹ��°벿
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode=3;
	pRod->widEndNode  =4;
	//pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(4);
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode=6;
	pRod->widEndNode  =5;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode=7;
	pRod->widEndNode  =6;
	pRod->widRefRodOfWorkWing=-6;
	pRod->ciAngleLayout=1;
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode=7;
	pRod->widEndNode  =8;
 	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode=10;
	pRod->widEndNode  = 8;
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode=11;
	pRod->widEndNode  =10;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode=11;
	pRod->widEndNode  =12;
	pRod->widRefRodOfWorkWing=-10;
	pRod->ciAngleLayout=4;
	pRod=pStdTempl->hashParaRods.Add(11);
	pRod->widStartNode=12;
	pRod->widEndNode  =13;
	pRod=pStdTempl->hashParaRods.Add(12);
	pRod->widStartNode=13;
	pRod->widEndNode  =14;
	pRod=pStdTempl->hashParaRods.Add(13);
	pRod->widStartNode=15;
	pRod->widEndNode  =14;
	pRod=pStdTempl->hashParaRods.Add(14);
	pRod->widStartNode=15;
	pRod->widEndNode  =16;
	pRod=pStdTempl->hashParaRods.Add(15);
	pRod->widStartNode=16;
	pRod->widEndNode  =17;
	//����߽������Ĺؼ��˼�
	pRod=pStdTempl->hashParaRods.Add(16);	//�Ҳ��������ڲ�����
	pRod->widStartNode=2;
	pRod->widEndNode  =17;
	pRod=pStdTempl->hashParaRods.Add(17);	//����������ڲ�����
	pRod->widStartNode=16;
	pRod->widEndNode  = 1;
	pRod=pStdTempl->hashParaRods.Add(18);
	pRod->widStartNode=18;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(19);
	pRod->widStartNode=19;
	pRod->widEndNode  =16;
	pRod=pStdTempl->hashParaRods.Add(20);
	pRod->widStartNode=18;
	pRod->widEndNode  = 3;
	pRod=pStdTempl->hashParaRods.Add(21);
	pRod->widStartNode=19;
	pRod->widEndNode  =15;
	pRod=pStdTempl->hashParaRods.Add(22);
	pRod->widStartNode=8;
	pRod->widEndNode  =5;
	pRod=pStdTempl->hashParaRods.Add(23);
	pRod->widStartNode=10;
	pRod->widEndNode  =13;
	pRod=pStdTempl->hashParaRods.Add(24);//������ƽ������
	pRod->widStartNode=19;
	pRod->widEndNode  =18;
	pRod=pStdTempl->hashParaRods.Add(25);	//�����Ҳ�߽�����
	pRod->widStartNode= 5;
	pRod->widEndNode  = 3;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=22;
	pRod=pStdTempl->hashParaRods.Add(26);	//�������߽�����
	pRod->widStartNode= 13;
	pRod->widEndNode  = 15;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=23;
	pRod=pStdTempl->hashParaRods.Add(27);	//�����Ҳ�������
	pRod->widStartNode= 8;
	pRod->widEndNode  =18;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=22;
	pRod=pStdTempl->hashParaRods.Add(28);	//�������������
	pRod->widStartNode= 10;
	pRod->widEndNode  = 19;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=23;
	pRod=pStdTempl->hashParaRods.Add(29);	//�Ҳർ��֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 4;
	pRod->widEndNode  =-4;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=3;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(30);	//��ർ��֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 14;
	pRod->widEndNode  =-14;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=13;
	pRod->ciSectStyle=2;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 16;
	pQuadSect->arrRods[1]=  1;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=17;
	pQuadSect->arrRods[1]=15;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=18;
	pQuadSect->arrRods[1]= 2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=19;
	pQuadSect->arrRods[1]=14;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=4;
	pQuadSect->arrRods[1]=3;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 12;
	pQuadSect->arrRods[1]= 13;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 8;
	pQuadSect->arrRods[1]=24;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=7;
	pQuadSect->arrRods[1]=5;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 9;
	pQuadSect->arrRods[1]=11;
	//"ZM1"
	pStdTempl=ARM_ARR.append();
	pStdTempl->ciClassicType='M';	//èͷ��
	pStdTempl->ciOnRightSide='U';	//��ͷ����
	memcpy(pStdTempl->name,"ZM1",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=1;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle=220;
	pParaNode->siMaxDegAngle=245;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=40;
	pParaNode->siMaxDegAngle=75;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->ciPosCalType=25;	//èͷ�к��봦��������սǴ������ڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=29;
	pParaNode->siMinDegAngle=185;
	pParaNode->siMaxDegAngle=220;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->ciProfileNode=2;	//�Ǳ���������
	pParaNode->ciPosCalType=20;	//�Ʊ�(èͷ)���������۽�����Ҳ�
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=4;
	pParaNode->siMinDegAngle=160;
	pParaNode->siMaxDegAngle=185;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->ciPosCalType=22;	//�Ʊ�(èͷ)��������һ��ᵣX+������ӽڵ�
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=33;
	pParaNode->siMinDegAngle=180;
	pParaNode->siMaxDegAngle=200;
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->ciProfileNode=2;	//�Ǳ���������
	pParaNode->ciPosCalType=24;	//èͷ�кᵣ��������˽ڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=35;
	pParaNode->siMinDegAngle=180;
	pParaNode->siMaxDegAngle=200;
	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=250;
	pParaNode->siMaxDegAngle=275;
	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=8;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=70;
	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=10;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=100;
	pParaNode=pStdTempl->hashParaNodes.Add(11);
	pParaNode->ciPosCalType=27;	//èͷ�кᵣ�������������֧���Ҳ��ڹյ�
	pParaNode->widLinkPrevRod=10;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=220;
	pParaNode->siMaxDegAngle=265;
	//����ֽ���
	pParaNode=pStdTempl->hashParaNodes.Add(12);
	pParaNode->ciPosCalType=-27;//èͷ�кᵣ�������������֧������ڹյ�
	pParaNode->widLinkPrevRod=11;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=220;
	pParaNode->siMaxDegAngle=265;
	pParaNode=pStdTempl->hashParaNodes.Add(13);
	pParaNode->widLinkPrevRod=12;
	pParaNode->widMasterRod=12;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=100;
	pParaNode=pStdTempl->hashParaNodes.Add(14);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=13;
	pParaNode->widMasterRod=14;
	pParaNode->siMinDegAngle=30;
	pParaNode->siMaxDegAngle=70;
	pParaNode=pStdTempl->hashParaNodes.Add(15);
	pParaNode->widLinkPrevRod=14;
	pParaNode->widMasterRod=14;
	pParaNode->siMinDegAngle=250;
	pParaNode->siMaxDegAngle=275;
	pParaNode=pStdTempl->hashParaNodes.Add(16);
	pParaNode->ciProfileNode=2;	//�Ǳ���������
	pParaNode->ciPosCalType=-24;	//èͷ�кᵣ��������˽ڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
	pParaNode->widLinkPrevRod=15;
	pParaNode->widMasterRod=36;
	pParaNode->siMinDegAngle=180;
	pParaNode->siMaxDegAngle=200;
	pParaNode=pStdTempl->hashParaNodes.Add(17);
	pParaNode->ciPosCalType=-22;	//�Ʊ�(èͷ)��������һ��ᵣX+������ӽڵ�
	pParaNode->widLinkPrevRod=16;
	pParaNode->widMasterRod=34;
	pParaNode->siMinDegAngle=180;
	pParaNode->siMaxDegAngle=200;
	pParaNode=pStdTempl->hashParaNodes.Add(18);
	pParaNode->ciProfileNode=2;	//�Ǳ���������
	pParaNode->ciPosCalType=-20;	//�Ʊ�(èͷ)���������۽�����Ҳ�
	pParaNode->widLinkPrevRod=17;
	pParaNode->widMasterRod=18;
	pParaNode->siMinDegAngle=160;
	pParaNode->siMaxDegAngle=185;
	pParaNode=pStdTempl->hashParaNodes.Add(19);
	pParaNode->ciPosCalType=-25;	//èͷ�к��봦��������սǴ������ڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
	pParaNode->widLinkPrevRod=18;
	pParaNode->widMasterRod=30;
	pParaNode->siMinDegAngle=185;
	pParaNode->siMaxDegAngle=220;
	pParaNode=pStdTempl->hashParaNodes.Add(20);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=19;
	pParaNode->widMasterRod=20;
	pParaNode->siMinDegAngle=40;
	pParaNode->siMaxDegAngle=75;

	pParaNode=pStdTempl->hashParaNodes.Add(21);
	pParaNode->widLinkPrevRod=20;
	pParaNode->widMasterRod=21;
	pParaNode->siMinDegAngle=220;
	pParaNode->siMaxDegAngle=245;
	pParaNode=pStdTempl->hashParaNodes.Add(22);
	//��������
	pParaNode=pStdTempl->hashParaNodes.Add(23);
	pParaNode->widMasterRod=22;
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 26;	//èͷ�к��봦���������Ҳ�սǴ��ڵ�
	pParaNode=pStdTempl->hashParaNodes.Add(24);
	pParaNode->widMasterRod=23;
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-26;	//èͷ�к��봦�����������սǴ��ڵ�
	pParaNode=pStdTempl->hashParaNodes.Add(25);
	pParaNode->widMasterRod=28;
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType= 23;	//�Ʊ�(èͷ)��������һ��ᵣ�ڲ����ӽڵ�
	pParaNode=pStdTempl->hashParaNodes.Add(26);
	pParaNode->widMasterRod=28;
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=-23;	//�Ʊ�(èͷ)��������һ��ᵣ�ڲ����ӽڵ�

	pRod=pStdTempl->hashParaRods.Add(1);
	pRod->widStartNode=2;
	pRod->widEndNode  =1;
	pRod=pStdTempl->hashParaRods.Add(2);	//���Ҹ��°벿
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	//pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(4);
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode=6;
	pRod->widEndNode  =5;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode=7;
	pRod->widEndNode  =6;
	pRod->widRefRodOfWorkWing=35;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode=8;
	pRod->widEndNode  =7;
 	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode=8;
	pRod->widEndNode  =9;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode=10;
	pRod->widEndNode  =9;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode=11;
	pRod->widEndNode  =10;
	pRod=pStdTempl->hashParaRods.Add(11);
	pRod->widStartNode=12;
	pRod->widEndNode  =11;
	pRod=pStdTempl->hashParaRods.Add(12);
	pRod->widStartNode=13;
	pRod->widEndNode  =12;
	pRod=pStdTempl->hashParaRods.Add(13);
	pRod->widStartNode=13;
	pRod->widEndNode  =14;
	pRod->widRefRodOfWorkWing=12;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(14);
	pRod->widStartNode=15;
	pRod->widEndNode  =14;
	pRod->widRefRodOfWorkWing=12;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(15);
	pRod->widStartNode=15;
	pRod->widEndNode  =16;
	pRod=pStdTempl->hashParaRods.Add(16);
	pRod->widStartNode=16;
	pRod->widEndNode  =17;
	pRod->widRefRodOfWorkWing=36;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(17);
	pRod->widStartNode=17;
	pRod->widEndNode  =18;
	pRod=pStdTempl->hashParaRods.Add(18);
	pRod->widStartNode=18;
	pRod->widEndNode  =19;
	pRod=pStdTempl->hashParaRods.Add(19);//������ƽ���������
	pRod->widStartNode=19;
	pRod->widEndNode  =20;
	pRod=pStdTempl->hashParaRods.Add(20);
	pRod->widStartNode=21;
	pRod->widEndNode  =20;
	pRod=pStdTempl->hashParaRods.Add(21);
	pRod->widStartNode=21;
	pRod->widEndNode  =22;
	//�������߽�Ĺؼ��˼�
	pRod=pStdTempl->hashParaRods.Add(22);
	pRod->widStartNode=23;
	pRod->widEndNode  =22;
	pRod=pStdTempl->hashParaRods.Add(23);
	pRod->widStartNode=24;
	pRod->widEndNode  = 1;
	pRod=pStdTempl->hashParaRods.Add(24);
	pRod->widStartNode= 5;
	pRod->widEndNode  =23;
	pRod=pStdTempl->hashParaRods.Add(25);
	pRod->widStartNode=18;
	pRod->widEndNode  =24;
	pRod=pStdTempl->hashParaRods.Add(26);
	pRod->widStartNode=25;
	pRod->widEndNode  = 5;
	pRod=pStdTempl->hashParaRods.Add(27);
	pRod->widStartNode=26;
	pRod->widEndNode  =18;
	pRod=pStdTempl->hashParaRods.Add(28);
	pRod->widStartNode=26;
	pRod->widEndNode  =25;
	pRod=pStdTempl->hashParaRods.Add(29);
	pRod->widStartNode= 4;
	pRod->widEndNode  = 2;
	pRod=pStdTempl->hashParaRods.Add(30);
	pRod->widStartNode=19;
	pRod->widEndNode  =21;
	pRod=pStdTempl->hashParaRods.Add(31);
	pRod->widStartNode=23;
	pRod->widEndNode  = 4;
	pRod->widRefRodOfWorkWing=29;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(32);
	pRod->widStartNode=24;
	pRod->widEndNode  =19;
	pRod->widRefRodOfWorkWing=30;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(33);
	pRod->widStartNode=25;
	pRod->widEndNode  = 6;
	pRod->widRefRodOfWorkWing=35;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(34);
	pRod->widStartNode=26;
	pRod->widEndNode  =17;
	pRod->widRefRodOfWorkWing=36;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(35);
	pRod->widStartNode=11;
	pRod->widEndNode  = 7;
	pRod=pStdTempl->hashParaRods.Add(36);
	pRod->widStartNode=12;
	pRod->widEndNode  =16;
	pRod=pStdTempl->hashParaRods.Add(37);	//�Ҳ����֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 9;
	pRod->widEndNode  =-9;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=8;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(38);	//������֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 14;
	pRod->widEndNode  =-14;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=14;
	pRod->ciSectStyle=2;
	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=22;
	pQuadSect->arrRods[1]= 1;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=23;
	pQuadSect->arrRods[1]=21;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 3;
	pQuadSect->arrRods[1]= 2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=19;
	pQuadSect->arrRods[1]=20;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=-22;
	pQuadSect->arrRods[1]= 29;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=-23;
	pQuadSect->arrRods[1]= 30;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=24;
	pQuadSect->arrRods[1]= 4;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=25;
	pQuadSect->arrRods[1]=18;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=26;
	pQuadSect->arrRods[1]= 5;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=27;
	pQuadSect->arrRods[1]=17;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=10;
	pQuadSect->arrRods[1]= 7;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=12;
	pQuadSect->arrRods[1]=15;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=11;
	pQuadSect->arrRods[1]=28;
	//"ZM1A"
	pStdTempl=ARM_ARR.append();
	AddHeadType_ZM1A(pStdTempl);
	//"ZM1A2"
	pStdTempl=ARM_ARR.append();
	AddHeadType_ZM1A(pStdTempl,1);
	//"ZM2"
	pStdTempl=ARM_ARR.append();
	AddHeadType_ZM2(pStdTempl);
	//"ZMC"
	pStdTempl=ARM_ARR.append();
	AddHeadType_ZMC(pStdTempl);
	//"ZL"
	pStdTempl=ARM_ARR.append();
	pStdTempl->ciClassicType=1;		//ֱ��
	pStdTempl->ciOnRightSide='U';	//��ͷ����
	memcpy(pStdTempl->name,"ZL",4);
	pParaNode=pStdTempl->hashParaNodes.Add(1);
	pParaNode=pStdTempl->hashParaNodes.Add(2);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=3;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=4;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=185;
	pParaNode->siMaxDegAngle=220;
	pParaNode=pStdTempl->hashParaNodes.Add(5);
	pParaNode->widLinkPrevRod=5;
	pParaNode->widMasterRod=5;
	pParaNode->siMinDegAngle=260;
	pParaNode->siMaxDegAngle=300;
	pParaNode=pStdTempl->hashParaNodes.Add(6);
	pParaNode->widLinkPrevRod=6;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=210;
	pParaNode->siMaxDegAngle=270;	//230��
	pParaNode=pStdTempl->hashParaNodes.Add(7);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=7;
	pParaNode->widMasterRod=7;
	pParaNode->siMinDegAngle=25;
	pParaNode->siMaxDegAngle=60;
	pParaNode=pStdTempl->hashParaNodes.Add(8);
	pParaNode->widLinkPrevRod=8;
	pParaNode->widMasterRod=9;
	pParaNode->siMinDegAngle=75;
	pParaNode->siMaxDegAngle=130;
	pParaNode=pStdTempl->hashParaNodes.Add(9);
	pParaNode->widLinkPrevRod=9;
	pParaNode->widMasterRod=21;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=230;
	//����ֽ���
	pParaNode=pStdTempl->hashParaNodes.Add(10);
	pParaNode->ciProfileNode=2;	//�Ǳ���������
	pParaNode->ciPosCalType=28;	//800kvֱ�����������Ͻ����
	pParaNode->widLinkPrevRod=10;
	pParaNode->widMasterRod=11;
	pParaNode->siMinDegAngle=175;
	pParaNode->siMaxDegAngle=200;
	//������
	pParaNode=pStdTempl->hashParaNodes.Add(11);
	pParaNode->widLinkPrevRod=11;
	pParaNode->widMasterRod=22;
	pParaNode->siMinDegAngle=190;
	pParaNode->siMaxDegAngle=230;
	pParaNode=pStdTempl->hashParaNodes.Add(12);
	pParaNode->widLinkPrevRod=12;
	pParaNode->widMasterRod=12;
	pParaNode->siMinDegAngle=75;
	pParaNode->siMaxDegAngle=130;
	pParaNode=pStdTempl->hashParaNodes.Add(13);
	pParaNode->wirecode.ciWireType='E';
	pParaNode->widLinkPrevRod=13;
	pParaNode->widMasterRod=14;
	pParaNode->siMinDegAngle=25;
	pParaNode->siMaxDegAngle=60;
	pParaNode=pStdTempl->hashParaNodes.Add(14);
	pParaNode->widLinkPrevRod=14;
	pParaNode->widMasterRod=14;
	pParaNode->siMinDegAngle=210;
	pParaNode->siMaxDegAngle=270;	//230��
	pParaNode=pStdTempl->hashParaNodes.Add(15);
	pParaNode->widLinkPrevRod=15;
	pParaNode->widMasterRod=16;
	pParaNode->siMinDegAngle=260;
	pParaNode->siMaxDegAngle=300;
	pParaNode=pStdTempl->hashParaNodes.Add(16);
	pParaNode->widLinkPrevRod=16;
	pParaNode->widMasterRod=16;
	pParaNode->siMinDegAngle=185;
	pParaNode->siMaxDegAngle=220;
	pParaNode=pStdTempl->hashParaNodes.Add(17);
	pParaNode->widLinkPrevRod=17;
	pParaNode->widMasterRod=17;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(18);
	pParaNode->wirecode.ciWireType='C';
	pParaNode->widLinkPrevRod=18;
	pParaNode->widMasterRod=19;
	pParaNode->siMinDegAngle=85;
	pParaNode->siMaxDegAngle=95;
	pParaNode=pStdTempl->hashParaNodes.Add(19);
	//��������
	pParaNode=pStdTempl->hashParaNodes.Add(20);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=5;
	pParaNode->raynode.widOrgNode=9;
	pParaNode->raynode.widEdgeStart=1;
	pParaNode->raynode.widEdgeEnd  =2;
	pParaNode->raynode.siMinRayAngle=260;
	pParaNode->raynode.siMinRayAngle=280;
	pParaNode=pStdTempl->hashParaNodes.Add(21);
	pParaNode->ciProfileNode=false;
	pParaNode->ciPosCalType=5;
	pParaNode->raynode.widOrgNode=11;
	pParaNode->raynode.widEdgeStart=18;
	pParaNode->raynode.widEdgeEnd  =19;
	pParaNode->raynode.siMinRayAngle=260;
	pParaNode->raynode.siMinRayAngle=280;

	pRod=pStdTempl->hashParaRods.Add(1);
	pRod->widStartNode=1;
	pRod->widEndNode  =20;
	pRod=pStdTempl->hashParaRods.Add(2);	//���Ҹ��°벿
	pRod->widStartNode=20;
	pRod->widEndNode  =2;
	pRod=pStdTempl->hashParaRods.Add(3);
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->widRefRodOfWorkWing=4;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(4);
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod->widRefRodOfWorkWing=5;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(5);
	pRod->widStartNode=5;
	pRod->widEndNode  =4;
	pRod=pStdTempl->hashParaRods.Add(6);
	pRod->widStartNode=6;
	pRod->widEndNode  =5;
	pRod->widRefRodOfWorkWing=8;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(7);
	pRod->widStartNode=6;
	pRod->widEndNode  =7;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=2;
 	pRod=pStdTempl->hashParaRods.Add(8);
	pRod->widStartNode=8;
	pRod->widEndNode  =7;
	pRod->widRefRodOfWorkWing=9;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(9);
	pRod->widStartNode=8;
	pRod->widEndNode  =9;
	pRod=pStdTempl->hashParaRods.Add(10);
	pRod->widStartNode=10;
	pRod->widEndNode  = 9;
	pRod->widRefRodOfWorkWing=21;
	pRod->ciAngleLayout=3;
	pRod=pStdTempl->hashParaRods.Add(11);
	pRod->widStartNode=10;
	pRod->widEndNode  =11;
	pRod->widRefRodOfWorkWing=22;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(12);
	pRod->widStartNode=12;
	pRod->widEndNode  =11;
	pRod=pStdTempl->hashParaRods.Add(13);
	pRod->widStartNode=12;
	pRod->widEndNode  =13;
	pRod->widRefRodOfWorkWing=12;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(14);
	pRod->widStartNode=14;
	pRod->widEndNode  =13;
	pRod->widRefRodOfWorkWing=12;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(15);
	pRod->widStartNode=14;
	pRod->widEndNode  =15;
	pRod=pStdTempl->hashParaRods.Add(16);
	pRod->widStartNode=15;
	pRod->widEndNode  =16;
	pRod=pStdTempl->hashParaRods.Add(17);
	pRod->widStartNode=16;
	pRod->widEndNode  =17;
	pRod->widRefRodOfWorkWing=16;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(18);//������ƽ���Ҳ�����
	pRod->widStartNode=17;
	pRod->widEndNode  =18;
	pRod->widRefRodOfWorkWing=17;
	pRod->ciAngleLayout=2;
	pRod=pStdTempl->hashParaRods.Add(19);//������ƽ���������
	pRod->widStartNode=18;
	pRod->widEndNode  =21;
	pRod=pStdTempl->hashParaRods.Add(20);
	pRod->widStartNode=21;
	pRod->widEndNode  =19;
	pRod=pStdTempl->hashParaRods.Add(21);
	pRod->widStartNode= 9;
	pRod->widEndNode  = 1;
	pRod->widRefRodOfWorkWing=1;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(22);
	pRod->widStartNode=11;
	pRod->widEndNode  =19;
	pRod->widRefRodOfWorkWing=20;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(23);
	pRod->widStartNode=10;
	pRod->widEndNode  = 1;
	pRod->widRefRodOfWorkWing=10;
	pRod->ciAngleLayout=3;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(24);
	pRod->widStartNode=10;
	pRod->widEndNode  =19;
	pRod->widRefRodOfWorkWing=11;
	pRod->ciAngleLayout=2;
	pRod->bHasSectView=true;
	pRod=pStdTempl->hashParaRods.Add(25);	//�Ҳ����֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 7;
	pRod->widEndNode  =-7;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=3;
	pRod->widRefRodOfWorkWing=6;
	pRod->ciSectStyle=2;
	pRod->ciMasterIndex=1;
	pRod=pStdTempl->hashParaRods.Add(26);	//������֧��T�����ˮƽ���߽Ǹ�
	pRod->widStartNode= 13;
	pRod->widEndNode  =-13;
	pRod->ciNearNormAxis=5;
	pRod->ciAngleLayout=2;
	pRod->widRefRodOfWorkWing=14;
	pRod->ciSectStyle=2;

	//���������ı�����
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 5;
	pQuadSect->arrRods[1]= 2;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=16;
	pQuadSect->arrRods[1]=19;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]= 9;
	pQuadSect->arrRods[1]= 6;
	pQuadSect=pStdTempl->listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=12;
	pQuadSect->arrRods[1]=15;
	PREDEF_ARM_COUNT=ARM_ARR.Size();//Count;
	return PREDEF_ARM_COUNT;
}
////////////////////////////////////////////////////////////////////////////////
#include "SortFunc.h"
static int _LocalCompareConnArmRod(const CONNECT_ARMROD& armrod1,const CONNECT_ARMROD& armrod2)
{	//�����������򣬼�Zֵ�ɴ���С����
	if(armrod1.zJointCoord<armrod2.zJointCoord)
		return  1;
	else if(armrod1.zJointCoord>armrod2.zJointCoord)
		return -1;
	else
		return 0;
}
void CIntelliCore::ExtendArmBandScopeZ(ARM_BAND* pArmBand)
{
	//CLdsPartListStack stack(m_pModel);
	IModelRod* pRod;
	bool finalloop=true;
	do{
		finalloop=true;
		for(pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			GEPOINT start=pRod->pStart->xPosition;
			GEPOINT end  =pRod->pEnd->xPosition;
			if(pArmBand->ciOnRightSide=='L')
			{	//�����ᵣ����ͳһת��Ϊ�Ҳ�ᵣ�����
				start.x*=-1;
				end.x*=-1;
			}
			double minx=min(start.x,end.x);
			double maxx=max(start.x,end.x);
			double minz=min(start.z,end.z);
			double maxz=max(start.z,end.z);
			if(minz>=pArmBand->scopeMinZ-EPS&&maxz<=pArmBand->scopeMaxZ+EPS)
				continue;	//�������ڵ�ǰ�ᵣ�������
			if(minx<0)
				continue;	//���ǵ�ǰ�ࣨ����ң��ᵣ�˼�
			if(minz>pArmBand->scopeMaxZ+EPS||maxz<=pArmBand->scopeMinZ-EPS)
				continue;	//�ø˼��������ε���ѭ���е�ǰ�ᵣ�������Zֵ��Χ
			short justifyStart=SideOfPtLine(GEPOINT(start.x,start.z),pArmBand->boundaryLineTopImgXY,pArmBand->boundaryLineBtmImgXY,5);
			short justifyEnd  =SideOfPtLine(GEPOINT(end.x  ,end.z  ),pArmBand->boundaryLineTopImgXY,pArmBand->boundaryLineBtmImgXY,5);
			if(justifyStart>=0||justifyEnd>=0)
				continue;	//�˼�����������(�ֽ���)��ࣨע���ʱͼ������ϵZ��ָ����Ļ�󷽣�
			if(minz<pArmBand->scopeMinZ)
				pArmBand->scopeMinZ=minz;
			if(maxz>pArmBand->scopeMaxZ)
				pArmBand->scopeMaxZ=maxz;
			finalloop=false;
		}
	}while(!finalloop);
}
int CIntelliCore::RecognizeAllStdTempl(ITowerModel* pModel,const double* vxSlopeTop3d,const double* vxSlopeBtm3d,IXhList<STDTEMPL>* pListLiveArms,
	ILog2File* pLogErrFile/*=NULL*/,UINT uiMinKeyLength/*=300*/,COLINEAR_TOLERANCE* pxTolerance/*=NULL*/)
{
	m_pLogErrFile=pLogErrFile!=NULL?pLogErrFile:&logerr;
	if(pxTolerance!=NULL)
		xTolerance=*pxTolerance;
	else
		xTolerance=COLINEAR_TOLERANCE();
	m_pModel=pModel;
	CLogErrorLife life((CLogFile*)m_pLogErrFile);
	CONNECT_ARMROD armrod,*pArmRod=NULL;
	STDTEMPL templ;
	IModelRod* pRod;
	CHashList<CONNECT_ARMROD>hashRightArmRod,hashLeftArmRod;
	//�ж��Ƿ�����ͷ���֣���λ����������¶�֮�ϵ���ͷ���֣���èͷ����ǡ��Ʊ�����ͷ wjh-2018.5.6
	bool hasTowerHead=false;
	GEPOINT vxSlopeTopPoint(vxSlopeTop3d),vxSlopeBtmPoint(vxSlopeBtm3d);
	for(pRod=m_pModel->EnumRodFirst();!hasTowerHead&&pRod;pRod=m_pModel->EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->layer[0]=='L')
			continue;
		GEPOINT start=pRod->pStart->xPosition;
		GEPOINT end  =pRod->pEnd->xPosition;
		GEPOINT vecstart=start-vxSlopeTopPoint;
		GEPOINT vecend=end-vxSlopeTopPoint;
		if(vecstart.IsZero()&&end.z<vxSlopeTopPoint.z-500&&end.x<start.x-100)
			hasTowerHead=true;
		else if(vecend.IsZero()&&start.z<vxSlopeTopPoint.z-500&&start.x<end.x-100)
			hasTowerHead=true;
	}
	for(pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod->layer[0]=='L')
			continue;
		GEPOINT start=pRod->pStart->xPosition;
		GEPOINT end  =pRod->pEnd->xPosition;
		if(hasTowerHead)
		{
			GEPOINT xStartToQuad1(fabs(start.x),fabs(start.y),start.z),xEndToQuad1(fabs(end.x),fabs(end.y),end.z);
			GEPOINT vecstart=xStartToQuad1-vxSlopeTopPoint;
			GEPOINT vecend=xEndToQuad1-vxSlopeTopPoint;
			if( (vecstart.IsZero()&&end.z<vxSlopeTopPoint.z-500)||
				(vecend.IsZero()&&start.z<vxSlopeTopPoint.z-500))
				continue;	//�ж�Ϊ��ͷ���ֵ����۸˼�
		}
		bool bStartOnLine=false,bEndOnLine=false,bArmRodMatched=false;
		GEPOINT postLineStart(vxSlopeTopPoint.x,vxSlopeTopPoint.z),postLineEnd(vxSlopeBtmPoint.x,vxSlopeBtmPoint.z);
		bStartOnLine=SideOfPtLine(GEPOINT(start.x,start.z),postLineStart,postLineEnd,5)==0;
		bEndOnLine  =SideOfPtLine(GEPOINT(end.x,end.z),postLineStart,postLineEnd,5)==0;
		if(bStartOnLine||bEndOnLine)
		{	//
			if(bStartOnLine&&bEndOnLine)
				continue;	//�ø˼��������¶ν�����
			else if(bStartOnLine&&end.x<=start.x+0.1)
				continue;	//�ø˼�ʼ�����������ӣ��ն����¶�����̨��
			else if(bEndOnLine&&start.x<=end.x+0.1)
				continue;	//�ø˼��ն����������ӣ�ʼ�����¶�����̨��
			if(start.z>vxSlopeBtmPoint.z+0.1||end.z>vxSlopeBtmPoint.z+0.1)
				continue;	//
			//�ø˼��������¶���ֱ������
			armrod.pConnRod=pRod;
			armrod.ciOnRightSide=true;
			armrod.scopeMinZ=min(start.z,end.z);
			armrod.scopeMaxZ=max(start.z,end.z);
			armrod.zJointCoord=ftoi(bStartOnLine?start.z-vxSlopeTopPoint.z+1:end.z-vxSlopeTopPoint.z+1);	//-pSlope->top.z+1��Ϊ�˱���zJointCoord����<=0���
			CONNECT_ARMROD* pExistArmRod=hashRightArmRod.GetValue(armrod.zJointCoord);
			if(pExistArmRod&&armrod.scopeMinZ<pExistArmRod->scopeMinZ)
				pExistArmRod->scopeMinZ=armrod.scopeMinZ;
			else if(pExistArmRod&&armrod.scopeMaxZ>pExistArmRod->scopeMaxZ)
				pExistArmRod->scopeMaxZ=armrod.scopeMaxZ;
			else if(pExistArmRod==NULL)
				hashRightArmRod.SetValue(armrod.zJointCoord,armrod);
			SideOfPtLine(GEPOINT(end.x,end.z),postLineStart,postLineEnd,5);
			bArmRodMatched=true;
		}
		if(!bArmRodMatched)
		{
			bStartOnLine=SideOfPtLine(GEPOINT(-start.x,start.z),postLineStart,postLineEnd,5)==0;
			bEndOnLine  =SideOfPtLine(GEPOINT(-end.x,end.z),postLineStart,postLineEnd,5)==0;
		}
		if( !bArmRodMatched&&(bStartOnLine||bEndOnLine))
		{
			if(bStartOnLine&&bEndOnLine)
				continue;	//�ø˼��������¶ν�����
			else if(bStartOnLine&&end.x>=start.x-0.1)
				continue;	//�ø˼�ʼ�����������ӣ��ն����¶�����̨��
			else if(bEndOnLine&&start.x>=end.x-0.1)
				continue;	//�ø˼��ն����������ӣ�ʼ�����¶�����̨��
			if(start.z>vxSlopeBtmPoint.z+0.1||end.z>vxSlopeBtmPoint.z+0.1)
				continue;	//
			//�ø˼��������¶���ֱ������
			armrod.pConnRod=pRod;
			armrod.ciOnRightSide=false;
			armrod.scopeMinZ=min(start.z,end.z);
			armrod.scopeMaxZ=max(start.z,end.z);
			armrod.zJointCoord=ftoi(bStartOnLine?start.z-vxSlopeTopPoint.z+1:end.z-vxSlopeTopPoint.z+1);	//-pSlope->top.z+1��Ϊ�˱���zJointCoord����<=0���
			CONNECT_ARMROD* pExistArmRod=hashLeftArmRod.GetValue(armrod.zJointCoord);
			if(pExistArmRod&&armrod.scopeMinZ<pExistArmRod->scopeMinZ)
				pExistArmRod->scopeMinZ=armrod.scopeMinZ;
			else if(pExistArmRod&&armrod.scopeMaxZ>pExistArmRod->scopeMaxZ)
				pExistArmRod->scopeMaxZ=armrod.scopeMaxZ;
			else if(pExistArmRod==NULL)
				hashLeftArmRod.SetValue(armrod.zJointCoord,armrod);
			bArmRodMatched=true;
		}
	}
	DYN_ARRAY<CONNECT_ARMROD> arrLeftArmRods(hashLeftArmRod.GetNodeNum());
	DYN_ARRAY<CONNECT_ARMROD> arrRightArmRods(hashRightArmRod.GetNodeNum());
	int i=0;
	for(pArmRod=hashLeftArmRod.GetFirst(),i=0;pArmRod;pArmRod=hashLeftArmRod.GetNext(),i++)
		arrLeftArmRods[i]=*pArmRod;
	for(pArmRod=hashRightArmRod.GetFirst(),i=0;pArmRod;pArmRod=hashRightArmRod.GetNext(),i++)
		arrRightArmRods[i]=*pArmRod;
	CQuickSort<CONNECT_ARMROD>::QuickSort(arrLeftArmRods,arrLeftArmRods.Size(),_LocalCompareConnArmRod);
	CQuickSort<CONNECT_ARMROD>::QuickSort(arrRightArmRods,arrLeftArmRods.Size(),_LocalCompareConnArmRod);
	double MAX_ARMBAND_HEIGHT=6000;

	ARM_BAND armband,*pArmBand=NULL;
	armband.boundaryLineTopImgXY[0]=vxSlopeTopPoint.x;
	armband.boundaryLineTopImgXY[1]=vxSlopeTopPoint.z;
	armband.boundaryLineBtmImgXY[0]=vxSlopeBtmPoint.x;
	armband.boundaryLineBtmImgXY[1]=vxSlopeBtmPoint.z;
	CXhSimpleList<ARM_BAND>listRightArmBands,listLeftArmBands;
	int armrodcount=arrRightArmRods.Size();	//����ת��int���͵�armrodcount������arrRightArmRods.Size()�Ǻܴ�� wjh-2018.5.6
	for(i=0;i<(int)arrRightArmRods.Size()-1;i++)
	{
		CONNECT_ARMROD* pDownArmRod=&arrRightArmRods[i];
		CONNECT_ARMROD* pUpArmRod=&arrRightArmRods[i+1];
		if(pDownArmRod->zJointCoord-pUpArmRod->zJointCoord<=MAX_ARMBAND_HEIGHT)
		{
			armband.ciOnRightSide='R';
			armband.scopeMaxZ=max(pUpArmRod->scopeMaxZ,pDownArmRod->scopeMaxZ);
			armband.scopeMinZ=min(pUpArmRod->scopeMinZ,pDownArmRod->scopeMinZ);
			listRightArmBands.Append(armband);
			i++;
		}
		else
		{
			pLogErr->Log("0x%X�˼��ںᵣʶ������б���Ϊ���쳣�ĺᵣ���Ӹ˼�",pDownArmRod->pConnRod->handle);
			i++;
		}
	}
	for(i=0;i<(int)arrLeftArmRods.Size()-1;i++)
	{
		CONNECT_ARMROD* pDownArmRod=&arrLeftArmRods[i];
		CONNECT_ARMROD* pUpArmRod=&arrLeftArmRods[i+1];
		if(pDownArmRod->zJointCoord-pUpArmRod->zJointCoord<=MAX_ARMBAND_HEIGHT)
		{
			armband.ciOnRightSide='L';
			armband.scopeMaxZ=max(pUpArmRod->scopeMaxZ,pDownArmRod->scopeMaxZ);
			armband.scopeMinZ=min(pUpArmRod->scopeMinZ,pDownArmRod->scopeMinZ);
			listLeftArmBands.Append(armband);
			i++;
		}
		else
		{
			pLogErr->Log("0x%X�˼��ںᵣʶ������б���Ϊ���쳣�ĺᵣ���Ӹ˼�",pDownArmRod->pConnRod->handle);
			i++;
		}
	}
	int recog_count=0;
	//���Ǹ��������ߴ��С���´���ʶ�����Ӱ��
	//uiMinKeyLength*SCALE_OF_IMAGE2MODEL>8
	if(uiMinKeyLength>0)
		SCALE_OF_IMAGE2MODEL=10.0/uiMinKeyLength;
	SCALE_OF_IMAGE2MODEL=max(0.015,SCALE_OF_IMAGE2MODEL);
	if(hasTowerHead)
	{
		ARM_BAND head;
		STDTEMPL* pLiveArm=pListLiveArms->Append(0);
		head.ciOnRightSide=pLiveArm->ciOnRightSide='U';
		head.scopeMaxZ=vxSlopeTopPoint.z;
		head.boundaryLineTopImgXY[0]=vxSlopeTopPoint.x;
		head.boundaryLineTopImgXY[1]=vxSlopeTopPoint.z;
		if(RecogStdArmTemplPortal(pLiveArm,&head))
			recog_count++;
	}
	for(pArmBand=listRightArmBands.EnumObjectFirst();pArmBand;pArmBand=listRightArmBands.EnumObjectNext())
	{
		ExtendArmBandScopeZ(pArmBand);
		STDTEMPL* pLiveArm=pListLiveArms->Append(0);
		pLiveArm->ciOnRightSide='R';
		if(RecogStdArmTemplPortal(pLiveArm,pArmBand))
			recog_count++;
	}
	for(pArmBand=listLeftArmBands.EnumObjectFirst();pArmBand;pArmBand=listLeftArmBands.EnumObjectNext())
	{
		ExtendArmBandScopeZ(pArmBand);
		STDTEMPL* pLiveArm=pListLiveArms->Append(0);
		pLiveArm->ciOnRightSide='L';
		if(RecogStdArmTemplPortal(pLiveArm,pArmBand))
			recog_count++;
	}
	return recog_count;
}
bool CIntelliCore::RecognizeConnPointSchema(STDTEMPL::CONNPOINT *pConnPoint)
{
	STDTEMPL::CONNPOINT_SCHEMA connschema_arr[1];
	connschema_arr[0].name="�Ʊ�èͷ��������ᵣ���ӵ�";
	connschema_arr[0].widBaseRod=1;		//�Ե�һ���˼�Ϊ�ڵ㸸�˼�
	connschema_arr[0].widParentRod=1;	//�Ե�һ���˼�Ϊ���Ӷ�λ�����˼�
	//1.ˮƽ���
	connschema_arr[0].rodarr[0].widRod=1;
	connschema_arr[0].rodarr[0].siMinRayAngle=0;
	connschema_arr[0].rodarr[0].siMaxRayAngle=1;
	connschema_arr[0].rodarr[0].ciNecessity=1;	//��Ҫ�˼�
	connschema_arr[0].rodarr[0].ciAngleLayout=2;//
	connschema_arr[0].rodarr[0].ciRodType='R';
	//2.������
	connschema_arr[0].rodarr[1].widRod=2;
	connschema_arr[0].rodarr[1].siMinRayAngle=89;
	connschema_arr[0].rodarr[1].siMaxRayAngle=91;
	connschema_arr[0].rodarr[1].ciNecessity=1;	//��Ҫ�˼�
	connschema_arr[0].rodarr[1].ciAngleLayout=3;//
	connschema_arr[0].rodarr[1].ciRodType='R';
	//3.����������ĺᵣ���Ҹ�
	connschema_arr[0].rodarr[2].widRod=3;
	connschema_arr[0].rodarr[2].siMinRayAngle=135;
	connschema_arr[0].rodarr[2].siMaxRayAngle=180;
	connschema_arr[0].rodarr[2].ciNecessity=1;	//��Ҫ�˼�
	connschema_arr[0].rodarr[2].ciAngleLayout=3;//
	connschema_arr[0].rodarr[2].ciRodType='R';
	//4.���»�����������������Ҹ�
	connschema_arr[0].rodarr[3].widRod=4;
	connschema_arr[0].rodarr[3].siMinRayAngle=270;
	connschema_arr[0].rodarr[3].siMaxRayAngle=215;
	connschema_arr[0].rodarr[3].ciNecessity=1;	//��Ҫ�˼�
	connschema_arr[0].rodarr[3].ciAngleLayout=2;//
	connschema_arr[0].rodarr[3].ciRodType='R';
	for(int i=0;i<1;i++)
	{
		if(MatchWith(&connschema_arr[i],pConnPoint))
			return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////
CIntelliCore::CIntelliCore()
{
	m_pModel=NULL;
	m_pLogErrFile=NULL;
	SCALE_OF_IMAGE2MODEL=0.015;
	InitClassicArmTemplArr();
}
ILog2File* CIntelliCore::LogErrFile()
{
	if(m_pLogErrFile!=NULL)
		return m_pLogErrFile;
	else
		return &logerr;
}
struct RODLINE{
	long hRod;
	GELINE briefline;
	RODLINE(){hRod=0;}
	RODLINE(GELINE line,long _hRod=0)
	{
		briefline=line;
		hRod=_hRod;
	}
	RODLINE(const double* lineStart,const double* lineEnd,long _hRod=0)
	{
		briefline.start=lineStart;
		briefline.end=lineEnd;
		hRod=_hRod;
	}
};

void CIntelliCore::CalImgPoint(short* psiImgPointX,short* psiImgPointY,const double* modelPos,GECS& imgcs,double scaleImage2Model/*=0.01*/)
{
	GEPOINT local=imgcs.TransPToCS(modelPos)*scaleImage2Model;
	if(psiImgPointX)
	{
		double complement=local.x>0?0.5:-0.5;
		*psiImgPointX=(short)(local.x+complement);
	}
	if(psiImgPointY)
	{
		double complement=local.y>0?0.5:-0.5;
		*psiImgPointY=(short)(local.y+complement);
	}
}
CIntelliCore::PIXEL SCANPIXELS_R1[8]={{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
CIntelliCore::PIXEL SCANPIXELS_R2[16]={
	 2, 0,//# 1 
	 2, 1,//# 2 
	 2, 2,//# 3 //1,1��SCANPIXELS_R1�ص�,�ʸ�Ϊ2,2
	 1, 2,//# 4 
	 0, 2,//# 5 
	-1, 2,//# 6 
	-2, 2,//# 7 //ȡ��-1, 1
	-2, 1,//# 8 
	-2, 0,//# 9 
	-2,-1,//#10 
	-2,-2,//#11 //ȡ��-1,-1
	-1,-2,//#12 
	 0,-2,//#13 
	 1,-2,//#14 
	 2,-2,//#15 //ȡ��1,-1
	 2,-1,//#16 
};
CIntelliCore::PIXEL SCANPIXELS_R3[24]={
	 3, 0,//# 1 
	 3, 1,//# 2 
	 3, 2,//# 3 
	 2, 2,//# 4 
	 2, 3,//# 5 
	 1, 3,//# 6 
	 0, 3,//# 7 
	-1, 3,//# 8 
	-2, 3,//# 9 
	-2, 2,//#10 
	-3, 2,//#11 
	-3, 1,//#12 
	-3, 0,//#13 
	-3,-1,//#14 
	-3,-2,//#15 
	-2,-2,//#16 
	-2,-3,//#17 
	-1,-3,//#18 
	 0,-3,//#19 
	 1,-3,//#20 
	 2,-3,//#21 
	 2,-2,//#22 
	 3,-2,//#23 
	 3,-1,//#24 
};
CIntelliCore::PIXEL SCANPIXELS_R4[32]={
	 4, 0,//# 1
	 4, 1,//# 2
	 4, 2,//# 3
	 3, 2,//# 4
	 3, 3,//# 5
	 2, 3,//# 6
	 2, 4,//# 7
	 1, 4,//# 8
	 0, 4,//# 9
	-1, 4,//#10
	-2, 4,//#11
	-2, 3,//#12
	-3, 3,//#13
	-3, 2,//#14
	-4, 2,//#15
	-4, 1,//#16
	-4, 0,//#17
	-4,-1,//#18
	-4,-2,//#19
	-3,-2,//#20
	-3,-3,//#21
	-2,-3,//#22
	-2,-4,//#23
	-1,-4,//#24
	 0,-4,//#25
	 1,-4,//#26
	 2,-4,//#27
	 2,-3,//#28
	 3,-3,//#29
	 3,-2,//#30
	 4,-2,//#31
	 4,-1,//#32
};
CIntelliCore::PIXEL SCANPIXELS_R5[40]={
	 5, 0,//# 1
	 5, 1,//# 2
	 5, 2,//# 3
	 4, 2,//# 4
	 4, 3,//# 5
	 4, 4,//# 6
	 3, 4,//# 7
	 2, 4,//# 8
	 2, 5,//# 9
	 1, 5,//#10
	 0, 5,//#11
	-1, 5,//#12
	-2, 5,//#13
	-2, 4,//#14
	-3, 4,//#15
	-4, 4,//#16
	-4, 3,//#17
	-4, 2,//#18
	-5, 2,//#19
	-5, 1,//#20
	-5, 0,//#21
	-5,-1,//#22
	-5,-2,//#23
	-4,-2,//#24
	-4,-3,//#25
	-4,-4,//#26
	-3,-4,//#27
	-2,-4,//#28
	-2,-5,//#29
	-1,-5,//#30
	 0,-5,//#31
	 1,-5,//#32
	 2,-5,//#33
	 2,-4,//#34
	 3,-4,//#35
	 4,-4,//#36
	 4,-3,//#37
	 4,-2,//#38
	 5,-2,//#39
	 5,-1,//#40
};
CIntelliCore::PIXEL SCANPIXELS_R6[48]={
	 6, 0,//# 1
	 6, 1,//# 2
	 6, 2,//# 3
	 5, 2,//# 4
	 5, 3,//# 5
	 5, 4,//# 6
	 4, 4,//# 7
	 4, 5,//# 8
	 3, 5,//# 9
	 2, 5,//#10
	 2, 6,//#11
	 1, 6,//#12
	 0, 6,//#13
	-1, 6,//#14
	-2, 6,//#15
	-2, 5,//#16
	-3, 5,//#17
	-4, 5,//#18
	-4, 4,//#19
	-5, 4,//#20
	-5, 3,//#21
	-5, 2,//#22
	-6, 2,//#23
	-6, 1,//#24
	-6, 0,//#25
	-6,-1,//#26
	-6,-2,//#27
	-5,-2,//#28
	-5,-3,//#29
	-5,-4,//#30
	-4,-4,//#31
	-4,-5,//#32
	-3,-5,//#33
	-2,-5,//#34
	-2,-6,//#35
	-1,-6,//#36
	 0,-6,//#37
	 1,-6,//#38
	 2,-6,//#39
	 2,-5,//#40
	 3,-5,//#41
	 4,-5,//#42
	 4,-4,//#43
	 5,-4,//#44
	 5,-3,//#45
	 5,-2,//#46
	 6,-2,//#47
	 6,-1,//#48
};
CIntelliCore::PIXEL SCANPIXELS_R7[56]={
	 7, 0,// 1# 
	 7, 1,// 2# 
	 7, 2,// 3# 
	 7, 3,// 4# 
	 6, 3,// 5# 
	 6, 4,// 6# 
	 5, 4,// 7# 
	 5, 5,// 8# 
	 4, 5,// 9# 
	 4, 6,//10# 
	 3, 6,//11# 
	 3, 7,//12# 
	 2, 7,//13# 
	 1, 7,//14# 
	 0, 7,//15# 
	-1, 7,//16# 
	-2, 7,//17# 
	-3, 7,//18# 
	-3, 6,//19# 
	-4, 6,//20# 
	-4, 5,//21# 
	-5, 5,//22# 
	-5, 4,//23# 
	-6, 4,//24# 
	-6, 3,//25# 
	-7, 3,//26# 
	-7, 2,//27# 
	-7, 1,//28# 
	-7, 0,//29# 
	-7,-1,//30# 
	-7,-2,//31# 
	-7,-3,//32# 
	-6,-3,//33# 
	-6,-4,//34# 
	-5,-4,//35# 
	-5,-5,//36# 
	-4,-5,//37# 
	-4,-6,//38# 
	-3,-6,//39# 
	-3,-7,//40# 
	-2,-7,//41# 
	-1,-7,//42# 
	 0,-7,//43# 
	 1,-7,//44# 
	 2,-7,//45# 
	 3,-7,//46# 
	 3,-6,//47# 
	 4,-6,//48# 
	 4,-5,//49# 
	 5,-5,//50# 
	 5,-4,//51# 
	 6,-4,//52# 
	 6,-3,//53# 
	 7,-3,//54# 
	 7,-2,//55# 
	 7,-1,//56# 
};
BYTE CIntelliCore::CIRCLE_SCAN::SummaryScanPixels()
{	//ѡ��ɨ��Բ�뾶����Ӧ��ȫ��������
	if(ciRadius==1)
		return 8;
	else if(ciRadius==2)
		return 16;
	else if(ciRadius==3)
		return 24;
	else if(ciRadius==4)
		return 32;
	else if(ciRadius==5)
		return 40;
	else if(ciRadius==6)
		return 48;
	else if(ciRadius==7)
		return 56;
	else
		return 0;
}
bool CIntelliCore::CIRCLE_SCAN::Init(const double* lenStdVec2d,BYTE radius/*=4*/)
{
	CIntelliCore::PIXEL* parrPixels=NULL;
	if(radius==1)
		parrPixels=SCANPIXELS_R1;
	else if(radius==2)
		parrPixels=SCANPIXELS_R2;
	else if(radius==3)
		parrPixels=SCANPIXELS_R3;
	else if(radius==4)
		parrPixels=SCANPIXELS_R4;
	else if(radius==5)
		parrPixels=SCANPIXELS_R5;
	else if(radius==6)
		parrPixels=SCANPIXELS_R6;
	else if(radius=7)
		parrPixels=SCANPIXELS_R7;
	else
		return false;
	ciRadius=radius;
	GEPOINT vxRayVec(-lenStdVec2d[0]*radius,-lenStdVec2d[1]*radius);
	//siBaseEdgeAngle=Cal2dLineAng(0,0,vxRayVec.x,vxRayVec.y);
	BYTE count=SummaryScanPixels();
	ciBaseEdgeIndex=-1;
	double minimal_deita2=0,skip_deita2=0;
	for(BYTE i=0;i<count;i++)
	{
		double dx=parrPixels[i].X-vxRayVec.x;
		double dy=parrPixels[i].Y-vxRayVec.y;
		double deita2=dx*dx+dy*dy;
		if(ciBaseEdgeIndex==-1)
		{
			ciBaseEdgeIndex=i;
			minimal_deita2=deita2;
		}
		else if(deita2<minimal_deita2)
		{
			ciBaseEdgeIndex=i;
			minimal_deita2=deita2;
		}
	}
	if(vxRayVec.x*parrPixels[ciBaseEdgeIndex].Y-vxRayVec.y*parrPixels[ciBaseEdgeIndex].X<0)
		ciBaseEdgeIndex=(ciBaseEdgeIndex+1)%count;	//���뱣֤ciBaseEdgeIndex��ʼ�ߴ��ڵ���vxRayVec�н�
	return true;
}
short CIntelliCore::CIRCLE_SCAN::ScanSectorPixels(int xOrgI,int yOrgJ,int xCurrI,int yCurrJ,BYTE radius/*=2*/,bool righthandcs/*=true*/,PIXEL* scanpixels/*=NULL*/,short maxpixels/*=0*/)
{
	double vx=xCurrI-xOrgI,vy=yCurrJ-yOrgJ;
	double len=SQRT(vx*vx+vy*vy);	//����
	vx/=len;
	vy/=len;
	double vnx=righthandcs?-vy: vy;
	double vny=righthandcs? vx:-vx;

	//SCANPIXELS_R2
	//PIXELEX* pPixel;
	PRESET_ARRAY32<PIXEL> pixels;
	CIntelliCore::PIXEL* parrPixels;
	short i,j,count;
	if(radius==1)
	{
		parrPixels=SCANPIXELS_R1;
		count=8;
	}
	else if(radius==2)
	{
		parrPixels=SCANPIXELS_R2;
		count=16;
	}
	else if(radius==3)
	{
		parrPixels=SCANPIXELS_R3;
		count=24;
	}
	else if(radius==4)
	{
		parrPixels=SCANPIXELS_R4;
		count=32;
	}
	CMaxDouble maxy;
	for(i=0;i<count;i++)
	{
		double x=vx*parrPixels[i].X+vy*parrPixels[i].Y;
		double y=vnx*parrPixels[i].X+vny*parrPixels[i].Y;
		if(x<0&&y<0)
			maxy.Update(y,(void*)i);
	}
	if(!maxy.IsInited())
		return 0;
	for(i=0;i<count;i++)
	{
		int index=0;
		int startIndex=(int)maxy.m_pRelaObj;
		if(righthandcs)
			index=(startIndex+i)%count;
		else
			index=(startIndex+count-i)%count;
		pixels.Append(parrPixels[index]);
	}
	//pPixel->sortnum=ftoi(y*100+x*10);
	//CQuickSort<PIXELEX>::QuickSort(pixels,pixels.Count,_LocalComparePixelEx);
	count=min((short)pixels.Count,maxpixels);
	for(j=0;j<count;j++)
		scanpixels[j]=pixels[j];
	return count;
}
short CIntelliCore::CIRCLE_SCAN::ScanPixels(double fromDegAngle/*=15*/,double toDegAngle/*=345*/,bool righthandcs/*=true*/,
			PIXEL* scanpixels/*=NULL*/,short maxpixels/*=0*/)
{
	CIntelliCore::PIXEL* parrPixels=NULL;
	double slicesPerDegree=0;
	short count=SummaryScanPixels();
	if(ciRadius==1)
	{
		parrPixels=SCANPIXELS_R1;
		slicesPerDegree=0.0266667;
	}
	else if(ciRadius==2)
	{
		parrPixels=SCANPIXELS_R2;
		slicesPerDegree=0.044444;
	}
	else if(ciRadius==3)
	{
		parrPixels=SCANPIXELS_R3;
		slicesPerDegree=0.066667;
	}
	else if(ciRadius==4)
	{
		parrPixels=SCANPIXELS_R4;
		slicesPerDegree=0.088889;
	}
	else if(ciRadius==5)
	{
		parrPixels=SCANPIXELS_R5;
		slicesPerDegree=0.111111;
	}
	else if(ciRadius==6)
	{
		parrPixels=SCANPIXELS_R6;
		slicesPerDegree=0.133333;
	}
	else if(ciRadius==7)
	{
		parrPixels=SCANPIXELS_R7;
		slicesPerDegree=0.155555;
	}
	else
		return 0;
	short scanStartIndexFromBaseIndex=(short)ceil(fromDegAngle*slicesPerDegree);
	short scanEndIndexFromBaseIndex  =(short)ftoi(toDegAngle*slicesPerDegree);	//�������(radiusС�нǷ�Χ�󣩣�floor�ᶪʧ����ɨ���
	short scanpixel_count=scanEndIndexFromBaseIndex-scanStartIndexFromBaseIndex+1;
	if(maxpixels==0)
		maxpixels=SummaryScanPixels();
	short actual_count=min(scanpixel_count,maxpixels);
	for(short i=0;i<actual_count;i++)
	{
		int localI;
		if(righthandcs)
			localI=ciBaseEdgeIndex+scanStartIndexFromBaseIndex+i;
		else
			localI=ciBaseEdgeIndex-scanStartIndexFromBaseIndex-i;
		if(localI<0)
			localI+=count;
		else if(localI>=count)
			localI%=count;
		scanpixels[i]=parrPixels[localI];
	}
	return scanpixel_count;
}
bool CIntelliCore::SearchNextPofileEdgeRod(SCAN_CONTEXT& context,SEARCH_RESULT* pRslt,BYTE scan_radius/*=5*/)
{
	CIRCLE_SCAN scanner;
	scanner.Init(context.vCurrLenStdVec,scan_radius);
	PIXEL scanpixels[48]={0,0};
	short scanpixel_count=scanner.ScanPixels(context.siStartScanAngle,355,false,scanpixels,48);
	int width =context.pMonoImage->GetImageWidth();
	int height=context.pMonoImage->GetImageHeight();
	PIXEL_RELAOBJ* pPixelRelaObj=NULL;
	GEPOINT vMajorSearchStdVec;
	bool bFindFirstHitPixel=false;
	short siLeastScanPixels=scanpixel_count;
	int xOriginalI=context.siCurrScanPointX;
	int yOriginalJ=context.siCurrScanPointY;
	int xInitI=xOriginalI;
	int yInitJ=yOriginalJ;
	for(int i=0;i<scanpixel_count&&siLeastScanPixels>0;i++)
	{
		//int xInitI=context.siCurrScanPointX;
		//int yInitJ=context.siCurrScanPointY;
		int xI=xInitI+scanpixels[i].X;
		int yJ=yInitJ+scanpixels[i].Y;
		if(bFindFirstHitPixel)
			siLeastScanPixels--;
		if(xI<0||xI>=width||yJ<0||yJ>=height)
			continue;
		vMajorSearchStdVec.Set(scanpixels[i].X,scanpixels[i].Y);
		if((pPixelRelaObj=context.pMonoImage->GetPixelRelaObjEx(xI,yJ,0,&xInitI,&yInitJ))!=NULL)
		{
			if( pPixelRelaObj->hRod==context.hCurrRod&&
				context.vCurrLenStdVec.x*scanpixels[i].X+context.vCurrLenStdVec.y*scanpixels[i].Y<0)
			{
				pPixelRelaObj=NULL;
				continue;	//�������µ�����Ϊ��ǰ�˼�
			}
			//if(context.hashRecogRods.GetValue(pPixelRelaObj->hRod)!=NULL)
			//	continue;	//������ж���Ϊ�˷�ֹ������ʶ��ı߽�˼������ĸ���
			pRslt->hRod=pPixelRelaObj->hRod;
			pRslt->xHitI=xI;
			pRslt->yHitJ=yJ;
			if(!bFindFirstHitPixel)
			{
				bFindFirstHitPixel=true;
				siLeastScanPixels=3;	//�ҵ���һ��������֮�������ɨ��3�����أ��������ɨ�赽������Χ�Ĵ���˼�
			}
			SEARCH_RESULT rslt;
			CIRCLE_SCAN scanner;
			PRESET_ARRAY32<PIXEL> subpixels;
			PIXEL_RELAOBJ* pTestPixelRelaObj=NULL,*pTestPixelRelaObj2=NULL;
			BYTE shortradius=scan_radius>2?2:1;
			short j,count=scanner.ScanSectorPixels(context.siCurrScanPointX,context.siCurrScanPointY,xI,yJ,shortradius,false,subpixels,16);
			int xTestI,yTestJ,xTestI2,yTestJ2;
			for(j=0;j<count;j++)
			{
				xTestI=xI+subpixels[j].X;
				yTestJ=yJ+subpixels[j].Y;
				if((pTestPixelRelaObj=context.pMonoImage->GetPixelRelaObjEx(xTestI,yTestJ,0))!=NULL)
					break;
			}
			if(pTestPixelRelaObj&&pTestPixelRelaObj->hRod==pPixelRelaObj->hRod)
			{
				rslt.hRod=pTestPixelRelaObj->hRod;
				rslt.xHitI=xTestI;
				rslt.yHitJ=yTestJ;
			}
			else
			{
				rslt.hRod=0;
				//����������߲Ķ˵�����γ������������ص��ǰ������̽��������ټ��2������
				count=scanner.ScanSectorPixels(context.siCurrScanPointX,context.siCurrScanPointY,xI,yJ,4,false,subpixels,32);
				for(j=0;j<count;j++)
				{
					xTestI2=xI+subpixels[j].X;
					yTestJ2=yJ+subpixels[j].Y;
					if((pTestPixelRelaObj2=context.pMonoImage->GetPixelRelaObjEx(xTestI2,yTestJ2,0))!=NULL)
						break;
				}
				if(pTestPixelRelaObj2&&pTestPixelRelaObj2->hRod==pPixelRelaObj->hRod)
				{
					rslt.hRod=pTestPixelRelaObj2->hRod;
					rslt.xHitI=xTestI2;
					rslt.yHitJ=yTestJ2;
				}
				if(pTestPixelRelaObj&&pTestPixelRelaObj2&&pTestPixelRelaObj2->hRod==pTestPixelRelaObj->hRod)
				{
					rslt.hRod=pTestPixelRelaObj2->hRod;
					pPixelRelaObj=pTestPixelRelaObj2;
					rslt.xHitI=xTestI;
					rslt.yHitJ=yTestJ;
				}
			}

			if(rslt.hRod>0)
			{
				pRslt->xHitI=rslt.xHitI;
				pRslt->yHitJ=rslt.yHitJ;
				break;
			}
			else
			{
				bool shrink=true;
				if(fabs(context.vCurrLenStdVec.x)>fabs(context.vCurrLenStdVec.y))
				{
					if(abs(xInitI-xOriginalI)<2)
					{
						xInitI-=context.vCurrLenStdVec.x>0?1:-1;
						double detax=xInitI-xOriginalI;
						yInitJ-=ftoi(detax*context.vCurrLenStdVec.y/context.vCurrLenStdVec.x);
					}
					else
						shrink=false;
				}
				else
				{
					if(abs(yInitJ-yOriginalJ)<2)
					{
						yInitJ-=context.vCurrLenStdVec.y>0?1:-1;
						double detay=yInitJ-yOriginalJ;
						xInitI-=ftoi(detay*context.vCurrLenStdVec.x/context.vCurrLenStdVec.y);
					}
					else
						shrink=false;
				}
				if(shrink)
				{
					i=-1;
					bFindFirstHitPixel=false;
				}
			}
		}
	}
	GELINE briefline;
	if(pPixelRelaObj)
	{
		IModelRod* pRod=m_pModel->FromRodHandle(pPixelRelaObj->hRod);
		briefline.start=context.ocs.TransPToCS(pRod->pStart->xPosition);
		briefline.end  =context.ocs.TransPToCS(pRod->pEnd->xPosition);
		GEPOINT lenStdVec(briefline.end.x-briefline.start.x,briefline.end.y-briefline.start.y);
		normalize(lenStdVec);
		pRslt->pRod=pRod;
		pRslt->hRod=pPixelRelaObj->hRod;	//�������¸�ֵ����Ϊ��������ʰȡ��ʱ���ܻ�����ʼ��ֵ��pRslt->hRod wjh-2018.6.24
		pRslt->lenStdVec=lenStdVec;
		pRslt->lineStart=briefline.start;
		pRslt->lineEnd=briefline.end;
		pRslt->majorStartI=5;
		pRslt->step.siStepMajor=5;
		pRslt->step.siStepMinor=1;

		GEPOINT inters,vVerifyLenVec;
		if(pPixelRelaObj->hRod==context.hCurrRod)
			vVerifyLenVec=context.vCurrLenStdVec;
		else
		{
			double cosa=lenStdVec*context.vCurrLenStdVec;
			if(fabs(cosa)>0.985)	//cos(10��)=0.985
				vVerifyLenVec=context.vCurrLenStdVec;
			else
			{
				GEPOINT xCurrLineStart(xOriginalI,yOriginalJ);
				GEPOINT xCurrLineEnd=xCurrLineStart+context.vCurrLenStdVec;
				briefline.start*=SCALE_OF_IMAGE2MODEL;
				briefline.end*=SCALE_OF_IMAGE2MODEL;
				int retcode=Int2dpl(f2dLine(xCurrLineStart,xCurrLineEnd),f2dLine(briefline.start,briefline.end),inters.x,inters.y);
				if(retcode<=0)
					vVerifyLenVec=context.vCurrLenStdVec;
				else
					vVerifyLenVec.Set(pRslt->xHitI-inters.x,pRslt->yHitJ-inters.y);
			}
		}
		if(lenStdVec*vVerifyLenVec>0)
		{
			pRslt->step.major.vfStdScanX=lenStdVec.x;
			pRslt->step.major.vfStdScanY=lenStdVec.y;
		}
		else
		{
			pRslt->step.major.vfStdScanX=-lenStdVec.x;
			pRslt->step.major.vfStdScanY=-lenStdVec.y;
		}
		pRslt->step.UpdateMinorDirection();
		return true;
	}
	else //if(pPixelRelaObj==NULL)
	{	//����ȷʵ�Ҳ�����Ҳ�����Ǹ����˼���ϵ���ӣ��޷��ҵ�ȷ������������߽�˼�
		//if(pVerifyPixel==NULL)
		//	logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"error");
		return false;
	}
}
bool CIntelliCore::MatchWith(STDTEMPL* pStdArmTempl,STDTEMPL* pLiveArmTempl)
{
	STDTEMPL::PROFILE_NODE* pTmplNode;
	bool bHasOptionProfileNode=false,matched=true;;
	int nMinmalProfileNodes=0;
	for(pTmplNode=pStdArmTempl->hashParaNodes.GetFirst();pTmplNode;pTmplNode=pStdArmTempl->hashParaNodes.GetNext())
	{
		if(pTmplNode->ciProfileNode==1)
			nMinmalProfileNodes++;
		else if(pTmplNode->ciProfileNode==2)
			bHasOptionProfileNode=true;
	}
	if( pLiveArmTempl->hashMatchNodes.GetNodeNum()!=pStdArmTempl->ProfileNodeCount&&
		pLiveArmTempl->hashMatchNodes.GetNodeNum()!=nMinmalProfileNodes)
		return false;
	bool isHead1=pLiveArmTempl->ciOnRightSide=='U';
	bool isHead2=pStdArmTempl->ciOnRightSide=='U';
	if(pStdArmTempl->ciClassicType=='Y'&&pLiveArmTempl->bMaybeHeadZBorZM)
		return false;	//��ǰ��ͷ����Ϊ�Ʊ���èͷ������ǰģ��Ϊ���
	else if((pStdArmTempl->ciClassicType=='B'||pStdArmTempl->ciClassicType=='M')&&
		!pLiveArmTempl->bMaybeHeadZBorZM)
		return false;	//��ǰ��ͷ�������ǾƱ���èͷ����ģ��Ϊ�Ʊ���èͷ
	if(isHead1!=isHead2)
		return false;

	PRESET_ARRAY128<STDTEMPL::MATCHNODE>matchNodeArr;
	pTmplNode=pStdArmTempl->hashParaNodes.GetFirst();
	STDTEMPL::MATCHNODE* pLiveNode=pLiveArmTempl->hashMatchNodes.GetFirst();
	STDTEMPL::MATCHNODE* pMatchZB1_Node3=NULL,*pMatchZB1_Node5=NULL,*pMatchZB1_Node9=NULL;
	bool bHeadTypeIsZB1=stricmp(pStdArmTempl->name,"ZB1")==0;
	bool bHeadTypeIsZB2=stricmp(pStdArmTempl->name,"ZB2")==0;
	bool bHeadTypeIsZB2A=stricmp(pStdArmTempl->name,"ZB2A")==0;
	bool bHeadTypeIsZB =bHeadTypeIsZB1||bHeadTypeIsZB2||bHeadTypeIsZB2A;
	do
	{
		//MATCHNODE* pMatchNode=pLiveArmTempl->hashMatchNodes.GetValue(pLiveNode->widNode);
		if( pTmplNode==NULL||pLiveNode==NULL)
		{
			matched=false;
			break;
		}
		if( pTmplNode->siMaxDegAngle-pTmplNode->siMinDegAngle>0&&(
			pLiveNode->siActualDegAngle<pTmplNode->siMinDegAngle||pLiveNode->siActualDegAngle>pTmplNode->siMaxDegAngle))
		{
			matched=false;	//�нǲ��ڱ�׼�ᵣģ�巶Χ��
			break;
		}
		STDTEMPL::MATCHNODE matchnode=*pLiveNode;
		matchnode.widNode=pTmplNode->widNode;
		matchNodeArr.Set(pTmplNode->widNode,matchnode);
		if(bHeadTypeIsZB&&pTmplNode->widNode==3)
			pMatchZB1_Node3=pLiveNode;
		else if(bHeadTypeIsZB&&pTmplNode->widNode==5)
			pMatchZB1_Node5=pLiveNode;
		else if(bHeadTypeIsZB&&pTmplNode->widNode==9)
			pMatchZB1_Node9=pLiveNode;
		if(pMatchZB1_Node3&&pMatchZB1_Node5&&pMatchZB1_Node9)
		{
			if(bHeadTypeIsZB1&&pMatchZB1_Node9->pIntelliNode->xPosition.x-pMatchZB1_Node3->pIntelliNode->xPosition.x<100)
				return false;
			if(bHeadTypeIsZB2&&fabs(pMatchZB1_Node5->pIntelliNode->xPosition.x-pMatchZB1_Node3->pIntelliNode->xPosition.x)>100)
				return false;
			if(bHeadTypeIsZB2A&&fabs(pMatchZB1_Node9->pIntelliNode->xPosition.x-pMatchZB1_Node3->pIntelliNode->xPosition.x)>100)
				return false;
		}
		do{
			if((pTmplNode=pStdArmTempl->hashParaNodes.GetNext())!=NULL&&pTmplNode->ciProfileNode>0)
				break;
			//else if(!pTmplNode->bProfileNode)
			//	continue;//���������������������ڵ�
		}while(pTmplNode!=NULL);
		pLiveNode=pLiveArmTempl->hashMatchNodes.GetNext();
	}while(pTmplNode!=NULL&&pLiveNode!=NULL);
	if(!matched&&bHasOptionProfileNode)
	{
		matched=true;
		matchNodeArr.Clear();
		memset(matchNodeArr,0,matchNodeArr.CountOfPreset*sizeof(STDTEMPL::MATCHNODE));
		pTmplNode=pStdArmTempl->hashParaNodes.GetFirst();
		pLiveNode=pLiveArmTempl->hashMatchNodes.GetFirst();
		pMatchZB1_Node3=pMatchZB1_Node9=NULL;
		do
		{
			if( pTmplNode==NULL||pLiveNode==NULL)
				return false;
			if( pTmplNode->siMaxDegAngle-pTmplNode->siMinDegAngle>0&&(
				pLiveNode->siActualDegAngle<pTmplNode->siMinDegAngle||pLiveNode->siActualDegAngle>pTmplNode->siMaxDegAngle))
				return false;	//�нǲ��ڱ�׼�ᵣģ�巶Χ��
			STDTEMPL::MATCHNODE matchnode=*pLiveNode;
			matchnode.widNode=pTmplNode->widNode;
			matchNodeArr.Set(pTmplNode->widNode,matchnode);
			if(bHeadTypeIsZB&&pTmplNode->widNode==3)
				pMatchZB1_Node3=pLiveNode;
			else if(bHeadTypeIsZB&&pTmplNode->widNode==9)
				pMatchZB1_Node9=pLiveNode;
			if(pMatchZB1_Node3&&pMatchZB1_Node9)
			{
				if(bHeadTypeIsZB1&&pMatchZB1_Node9->pIntelliNode->xPosition.x-pMatchZB1_Node3->pIntelliNode->xPosition.x<100)
					return false;
				if(bHeadTypeIsZB2&&pMatchZB1_Node9->pIntelliNode->xPosition.x-pMatchZB1_Node3->pIntelliNode->xPosition.x>100)
					return false;
			}
			do{
				if((pTmplNode=pStdArmTempl->hashParaNodes.GetNext())!=NULL&&pTmplNode->ciProfileNode==1)
					break;
			}while(pTmplNode!=NULL);
			pLiveNode=pLiveArmTempl->hashMatchNodes.GetNext();
		}while(pTmplNode!=NULL&&pLiveNode!=NULL);
	}
	if(!matched)
		return false;
	pLiveArmTempl->hashMatchNodes.Empty();
	for(UINT i=0;i<matchNodeArr.Count;i++)
		pLiveArmTempl->hashMatchNodes.SetValue(matchNodeArr[i].widNode,matchNodeArr[i]);
	/*
	STDTEMPL::MATCHNODE* pArmPostJointNodeRight=NULL,*pArmPostJointNodeLeft=NULL;
	for(UINT i=0;i<matchNodeArr.Count;i++)
	{
		pLiveNode=pLiveArmTempl->hashMatchNodes.SetValue(matchNodeArr[i].widNode,matchNodeArr[i]);
		pTmplNode=pStdArmTempl->hashParaNodes.GetValue(matchNodeArr[i].widNode);
		if(pTmplNode&&pTmplNode->ciPosCalType==20)
			pArmPostJointNodeRight=pLiveNode;
		else if(pTmplNode&&pTmplNode->ciPosCalType==-20)
			pArmPostJointNodeLeft=pLiveNode;
	}
	if(pLiveArmTempl->ciClassicType=='B'||pLiveArmTempl->ciClassicType=='M')
	{
		if(pArmPostJointNodeRight==NULL)
		{
			//m_pModel->
		}
		//if(pArmPostJointNodeLeft==NULL&&pArmPostJointNodeRight!=NULL)
		//	pArmPostJointNodeLeft=pArmPostJointNodeRight->
	}*/
	for(STDTEMPL::PROFILE_ROD* pEdgeRod=pStdArmTempl->hashParaRods.GetFirst();pEdgeRod;pEdgeRod=pStdArmTempl->hashParaRods.GetNext())
	{
		if(pEdgeRod->ciRodType!='H'&&pEdgeRod->ciRodType!='V')
			continue;
		STDTEMPL::MATCHNODE* pLiveStartNode=pLiveArmTempl->hashMatchNodes.GetValue(pEdgeRod->widStartNode);
		STDTEMPL::MATCHNODE* pLiveEndNode  =pLiveArmTempl->hashMatchNodes.GetValue(pEdgeRod->widEndNode);
		if(pLiveStartNode==NULL||pLiveEndNode==NULL)
			return false;
		if(pEdgeRod->ciRodType=='H'&&fabs(pLiveStartNode->pIntelliNode->xPosition.z-pLiveEndNode->pIntelliNode->xPosition.z)>1)
			return false;	//��ظ˼�����ˮƽ��
		else if(pEdgeRod->ciRodType=='V'&&fabs(pLiveStartNode->pIntelliNode->xPosition.x-pLiveEndNode->pIntelliNode->xPosition.x)>1)
			return false;	//��ظ˼�������ֱ��
	}
	return true;
}
bool CIntelliCore::MatchWith(STDTEMPL::CONNPOINT_SCHEMA* pConnPointSchema,STDTEMPL::CONNPOINT* pConnPoint)
{
	double TOLERANCE=0.1;
	WORD matchindexes[8]={0};	//�洢���ӵ�ģʽ���뵱ǰ�����˼���ƥ�������ģ���и˼���������
	int i,j;
	for(i=0;i<8;i++)
	{
		bool matched=false;
		for(j=0;j<8;j++)
		{
			if(pConnPointSchema->rodarr[j].widRod==0)
				break;
			if(pConnPointSchema->rodarr[j].siMinRayAngle>=0)
			{
				if( pConnPoint->rodarr[i].siActualRayAngle>pConnPointSchema->rodarr[j].siMaxRayAngle+TOLERANCE||
					pConnPoint->rodarr[i].siActualRayAngle<pConnPointSchema->rodarr[j].siMinRayAngle-TOLERANCE)
					continue;
			}
			else if(pConnPointSchema->rodarr[j].siMinRayAngle<0)
			{
				double minangle=pConnPointSchema->rodarr[j].siMinRayAngle+360;
				if( pConnPoint->rodarr[i].siActualRayAngle>=pConnPointSchema->rodarr[j].siMaxRayAngle+TOLERANCE&&
					pConnPoint->rodarr[i].siActualRayAngle<=minangle-TOLERANCE)
					continue;
			}
			matched=true;
			matchindexes[pConnPointSchema->rodarr[j].widRod-1]=i;
			break;
		}
		if(!matched&&pConnPointSchema->m_bStrictReview)
			return false;	//�ϸ����ʱ����������޷�ƥ��ĸ˼�
	}
	for(i=0;i<8;i++)
	{
		if(pConnPointSchema->rodarr[i].widRod==0)
			break;
		if(pConnPointSchema->rodarr[i].ciNecessity==1&&matchindexes[i]==0)
			return false;	//����ģ����ƥ�䵽ģʽ�еĸ����Ҫ�˼�
	}
	memcpy(pConnPoint->matchrod_indexes,matchindexes,sizeof(WORD)*8);
	pConnPoint->schema=*pConnPointSchema;
	return true;
}
//#include "ParseAdaptNo.h"
static bool CreateHeadTemplCode(STDTEMPL* pLiveArm,const char* szExcludeNodeSerials=NULL,const char* szExcludeRodSerials=NULL)
{
	CXhChar50 tmplname("d:/tmpl#%s.cpp",pLiveArm->name);
	FILE* fp=fopen(tmplname,"wt");
	fprintf(fp,"\tSTDTEMPL::PROFILE_NODE* pParaNode;\n");
	fprintf(fp,"\tSTDTEMPL::PROFILE_ROD* pRod;\n");
	fprintf(fp,"\tSTDTEMPL::PROFILE_QUADSECT* pQuadSect;\n");
	fprintf(fp,"\tpStdTempl->ciClassicType='B';	//�Ʊ���\n");
	fprintf(fp,"\tpStdTempl->ciOnRightSide='U';	//��ͷ����\n");
	fprintf(fp,"\tmemcpy(pStdTempl->name,\"ZB1\",4);\n");
	CHashList<long>hashExcludeNodes,hashExcludeRods;
	PRESET_ARRAY128<bool>xExcludeNodesArr,xExcludeRodsArr;
	memset((bool*)xExcludeNodesArr,0,128);
	memset((bool*)xExcludeRodsArr,0,128);
	/*if(szExcludeNodeSerials!=NULL)
	{
		long iterSerial=FindAdaptNo(szExcludeNodeSerials,",","-");
		do{
			xExcludeNodesArr.At(iterSerial)=true;
			iterSerial=FindAdaptNo(NULL,",","-");
		}while(iterSerial>0);
	}
	if(szExcludeRodSerials!=NULL)
	{
		long iterSerial=FindAdaptNo(szExcludeRodSerials,",","-");
		do{
			xExcludeRodsArr.At(iterSerial)=true;
			iterSerial=FindAdaptNo(NULL,",","-");
		}while(iterSerial>0);
	}*/
	STDTEMPL::MATCHNODE* pMatchNode,*pPrevNode=NULL;
	UINT serial=1,count=pLiveArm->hashMatchNodes.GetNodeNum();
	CXhChar100 slText;
	CHashList<long>hashNodeSerials;
	for(pMatchNode=pLiveArm->hashMatchNodes.GetFirst();pMatchNode;pMatchNode=pLiveArm->hashMatchNodes.GetNext(),serial++)
	{
		while(xExcludeNodesArr.At(serial))
			serial++;
		slText.Printf("\tpParaNode=pStdTempl->hashParaNodes.Add(%d);\n",serial);
		hashNodeSerials.SetValue(pMatchNode->widNode,serial);
		fprintf(fp,slText);
		if(serial==1)
			continue;
		else if(serial==count)
			break;	//���һ���ڵ�
		//pParaNode->ciProfileNode=2;	//�Ǳ���������
		//pParaNode->ciPosCalType=20;	//�Ʊ����������۽�����Ҳ�
		fprintf(fp,"\tpParaNode->widLinkPrevRod=1;\n");
		fprintf(fp,"\tpParaNode->widMasterRod=1;\n");
		int siMinDegAngle=max(  5,pMatchNode->siActualDegAngle-20);
		int odd=siMinDegAngle%5;
		if(odd<2)
			siMinDegAngle-=odd;
		else
			siMinDegAngle+=5-odd;
		int siMaxDegAngle=min(350,pMatchNode->siActualDegAngle+20);
		odd=siMaxDegAngle%5;
		if(odd<2)
			siMaxDegAngle-=odd;
		else
			siMaxDegAngle+=5-odd;
		slText.Printf("\tpParaNode->siMinDegAngle=%d;	//%d\n",siMinDegAngle,pMatchNode->siActualDegAngle);
		fprintf(fp,(char*)slText);
		fprintf(fp,"\tpParaNode->siMaxDegAngle=%d;\n\n",siMaxDegAngle);
	}
	//
	fprintf(fp,"\n");
	serial=1;
	pPrevNode=pLiveArm->hashMatchNodes.GetFirst();
	for(pMatchNode=pLiveArm->hashMatchNodes.GetNext();pMatchNode;pMatchNode=pLiveArm->hashMatchNodes.GetNext(),serial++)
	{
		while(xExcludeRodsArr.At(serial))
			serial++;

		long *phStartSerial=hashNodeSerials.GetValue(pPrevNode->widNode);
		long *phEndSerial=hashNodeSerials.GetValue(pMatchNode->widNode);
		slText.Printf("\tpRod=pStdTempl->hashParaRods.Add(%d);\n",serial);
		fprintf(fp,slText);
		if(phStartSerial!=NULL)
			slText.Printf("\tpRod->widStartNode=%2d;\n",*phStartSerial);
		else
			slText.Printf("\tpRod->widStartNode=  ;\n");
		fprintf(fp,slText);
		if(phEndSerial!=NULL)
			slText.Printf("\tpRod->widEndNode  =%2d;\n",*phEndSerial);
		else
			slText.Printf("\tpRod->widEndNode  =  ;\n");
		fprintf(fp,slText);
		pPrevNode=pMatchNode;
	}
	fprintf(fp,"\t//���������ı�����\n");
	fprintf(fp,"\tpQuadSect=pStdTempl->listQuadSects.AttachObject();\n");
	fprintf(fp,"\tpQuadSect->arrRods[0]=20;\n");
	fprintf(fp,"\tpQuadSect->arrRods[1]= 1;\n");
	fclose(fp);
	return true;
}
bool CIntelliCore::RecogZBorZMInnerKeyNodes(STDTEMPL* pLiveArm,CMonoImage* pMonoImage,GECS& ocs,KEY_NODE* keynodes)
{
	int width=pMonoImage->Width;
	int xStartI=width/2;
	int xI,yJ,whitehits=0;
	bool findblackpixel=false;
	for(yJ=pMonoImage->Height-1;yJ>0;yJ--)
	{
		if(!pMonoImage->IsBlackPixel(xStartI,yJ))
		{
			if(findblackpixel)
				whitehits++;
			if(whitehits>10)
				break;
			continue;
		}
		else
			findblackpixel=true;
	}
	PRESET_ARRAY16<IModelRod*> edgeRodArr;
	PRESET_ARRAY16<IModelRod*> edgeUpperRodArr;
	PRESET_ARRAY16<IModelNode*> cornerNodeArr;
	PRESET_ARRAY16<GEPOINT> edgeStdVecArr;
	edgeRodArr.ZeroPresetMemory();
	edgeUpperRodArr.ZeroPresetMemory();
	cornerNodeArr.ZeroPresetMemory();
	//memset(edgeRodArr,0,4*edgeRodArr.CountOfPreset);
	//memset(edgeUpperRodArr,0,4*edgeUpperRodArr.CountOfPreset);
	//memset(cornerNodeArr,0,4*cornerNodeArr.CountOfPreset);
	GEPOINT vPrevLenStdVec;
	IModelRod* pModelRod, *pCurrScaningRod=NULL,*pPrevRod=NULL,*pRod=NULL;
	PIXEL_RELAOBJ* pRelaObj=NULL,*pPrevRelaObj=NULL;
	//int yBtmJ=yJ;	//��ʱyBtmJ����èͷ�������ڲ��м佻��
	int hits=0,indexOfRod=0;
	long hCurrScaningRod=0,hPrevEdgeRod=0;
	for(yJ=yJ;yJ>0;hits<10?yJ--:yJ-=5)
	{
		for(xI=xStartI;xI<width;xI++)
		{
			if(!pMonoImage->IsBlackPixel(xI,yJ))
				continue;
			pRelaObj=pMonoImage->GetPixelRelaObj(xI,yJ);
			break;
		}
		if(xI<xStartI+20&&edgeRodArr.Count>0)
		{
			if(hits>=10&&hCurrScaningRod!=hPrevEdgeRod)	//��Ҫ�����ڲ�˼���Ч��������һ�������10��
			{
				if((pModelRod=m_pModel->FromRodHandle(hCurrScaningRod))!=NULL)
				{
					GEPOINT vLenStdVec=pModelRod->xEndPoint-pModelRod->xStartPoint;
					if(vLenStdVec.z>0)
						vLenStdVec*=-1.0;
					normalize(vLenStdVec);
					if(fabs(vLenStdVec*vPrevLenStdVec)>EPS_COS)	//���߸˼�
					{
						hPrevEdgeRod=hCurrScaningRod;
						edgeUpperRodArr.Set(indexOfRod,pCurrScaningRod);
					}
					else
					{
						vPrevLenStdVec=vLenStdVec;
						edgeRodArr.Append(pModelRod);
						indexOfRod=edgeRodArr.Count-1;
						hPrevEdgeRod=hCurrScaningRod;
						edgeStdVecArr.Set(indexOfRod,vLenStdVec);
					}
				}
			}
			break;	//��ɨ�赽�ᵣ�����Ҹ�
		}
		if(hCurrScaningRod==0)
		{
			hCurrScaningRod=pRelaObj->hRod;
			hits=1;
		}
		else if(pRelaObj->hRod==hCurrScaningRod)
			hits++;
		else if(pRelaObj->hRod!=hCurrScaningRod)
		{
			if(hits>=10&&hCurrScaningRod!=hPrevEdgeRod)	//��Ҫ�����ڲ�˼���Ч��������һ�������10��
			{
				if((pModelRod=m_pModel->FromRodHandle(hCurrScaningRod))!=NULL)
				{
					GEPOINT vLenStdVec=pModelRod->xEndPoint-pModelRod->xStartPoint;
					if(vLenStdVec.z>0)
						vLenStdVec*=-1.0;
					normalize(vLenStdVec);
					if(fabs(vLenStdVec*vPrevLenStdVec)>EPS_COS)	//���߸˼�
					{
						hPrevEdgeRod=hCurrScaningRod;
						edgeUpperRodArr.Set(indexOfRod,pCurrScaningRod);
					}
					else
					{
						vPrevLenStdVec=vLenStdVec;
						edgeRodArr.Append(pModelRod);
						indexOfRod=edgeRodArr.Count-1;
						hPrevEdgeRod=hCurrScaningRod;
						edgeStdVecArr.Set(indexOfRod,vLenStdVec);
					}
				}
			}
			hCurrScaningRod=pRelaObj->hRod;
			pCurrScaningRod=m_pModel->FromRodHandle(pRelaObj->hRod);
			hits=(hCurrScaningRod!=hPrevEdgeRod)?1:11;
		}
	}
	IModelNode* pNode,*pFrontNode=NULL;
	for(WORD i=1;i<edgeRodArr.Count;i++)
	{
		pPrevRod=edgeRodArr.At(i-1);
		if((pRod=edgeUpperRodArr.At(i-1))!=NULL)
			pPrevRod=pRod;
		pRod=edgeRodArr.At(i);
		IModelNode* pUpperNode=pPrevRod->xEndPoint.z<pPrevRod->xStartPoint.z?pPrevRod->pEnd:pPrevRod->pStart;
		if(pRod->pStart==pUpperNode||pRod->pEnd==pUpperNode)
			cornerNodeArr.Append(pUpperNode);
		else
		{
			GEPOINT inters;
			f2dLine L1(f2dPoint(pPrevRod->xStartPoint.x,pPrevRod->xStartPoint.z),f2dPoint(pPrevRod->xEndPoint.x,pPrevRod->xEndPoint.z));
			f2dLine L2(f2dPoint(pRod->xStartPoint.x,pRod->xStartPoint.z),f2dPoint(pRod->xEndPoint.x,pRod->xEndPoint.z));
			Int2dpl(L1,L2,inters.x,inters.y);
			pFrontNode=NULL;
			for(pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
			{
				if(fabs(pNode->xPosition.x-inters.x)+fabs(pNode->xPosition.z-inters.y)>1)
					continue;
				if(pFrontNode==NULL)
					pFrontNode=pNode;
				else if(pFrontNode->xPosition.y<pNode->xPosition.y)
					pFrontNode=pNode;
			}
			if(pFrontNode==NULL)
			{
				if(pRod->xStartPoint.y<pRod->xEndPoint.y)
					pFrontNode=pRod->pStart;
				else
					pFrontNode=pRod->pEnd;
			}
			cornerNodeArr.Append(pFrontNode);
		}
	}
	IModelRod* pTopRod=edgeRodArr.At(edgeRodArr.Count-1);
	if((pRod=edgeUpperRodArr.At(edgeRodArr.Count-1))!=NULL)
		pTopRod=pRod;
	if(fabs(pTopRod->xEndPoint.z-pTopRod->xStartPoint.z)>fabs(pTopRod->xEndPoint.x-pTopRod->xStartPoint.x))
	{
		if(pTopRod->xStartPoint.z<pTopRod->xEndPoint.z)
			cornerNodeArr.Append(pTopRod->pStart);
		else
			cornerNodeArr.Append(pTopRod->pEnd);
	}
	if(pLiveArm->ciClassicType=='B')
	{
		keynodes->zb.pArmPostJointNodeRight=cornerNodeArr[0];
		keynodes->zb.pArmBeamJointNodeInnerRight=cornerNodeArr[1];
		//�ǾݶԳƹ�ϵȷ�����ؼ��ڽǽڵ�
		for(pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
		{
			if( fabs(pNode->xPosition.x+keynodes->zb.pArmPostJointNodeRight->xPosition.x)+
				fabs(pNode->xPosition.z-keynodes->zb.pArmPostJointNodeRight->xPosition.z)<EPS2)
			{
				if( keynodes->zb.pArmPostJointNodeLeft==NULL)
					keynodes->zb.pArmPostJointNodeLeft=pNode;
				else if(pNode->xPosition.y>keynodes->zb.pArmPostJointNodeLeft->xPosition.y)
					keynodes->zb.pArmPostJointNodeLeft=pNode;
			}
			else if(fabs(pNode->xPosition.x+keynodes->zb.pArmBeamJointNodeInnerRight->xPosition.x)+
					fabs(pNode->xPosition.z-keynodes->zb.pArmBeamJointNodeInnerRight->xPosition.z)<EPS2)
			{
				if( keynodes->zb.pArmBeamJointNodeInnerLeft==NULL)
					keynodes->zb.pArmBeamJointNodeInnerLeft=pNode;
				else if(pNode->xPosition.y>keynodes->zb.pArmBeamJointNodeInnerLeft->xPosition.y)
					keynodes->zb.pArmBeamJointNodeInnerLeft=pNode;
			}
		}
	}
	else if(pLiveArm->ciClassicType=='M')
	{
		keynodes->zm.pArmBeardJointNodeInnerRight=cornerNodeArr[0];
		keynodes->zm.pArmPostJointNodeRight=cornerNodeArr[1];
		if((keynodes->zm.pArmBeamJointNodeInnerRight=cornerNodeArr[2])!=NULL)
		{
			short siImgOrgPointX,siImgOrgPointY;
			CalImgPoint(&siImgOrgPointX,&siImgOrgPointY,cornerNodeArr[2]->xPosition,ocs,SCALE_OF_IMAGE2MODEL);
			siImgOrgPointX+=5;
			IModelRod* pBtmBeamOuterRod=NULL;
			for(yJ=siImgOrgPointY;yJ>6;yJ++)
			{
				if((pRelaObj=pMonoImage->GetPixelRelaObj(siImgOrgPointX,yJ))!=NULL)
				{
					pBtmBeamOuterRod=m_pModel->FromRodHandle(pRelaObj->hRod);
					break;
				}
			}
			if(pBtmBeamOuterRod!=NULL)
			{
				if(pBtmBeamOuterRod->xEndPoint.x>pBtmBeamOuterRod->xStartPoint.x)
					keynodes->zm.pArmBeamJointNodeOutRight=pBtmBeamOuterRod->pEnd;
				else
					keynodes->zm.pArmBeamJointNodeOutRight=pBtmBeamOuterRod->pStart;
			}
		}

		//�ǾݶԳƹ�ϵȷ�����ؼ��ڽǽڵ�
		for(pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
		{
			if( fabs(pNode->xPosition.x+keynodes->zm.pArmBeardJointNodeInnerRight->xPosition.x)+
				fabs(pNode->xPosition.z-keynodes->zm.pArmBeardJointNodeInnerRight->xPosition.z)<EPS2)
			{
				if( keynodes->zm.pArmBeardJointNodeInnerLeft==NULL)
					keynodes->zm.pArmBeardJointNodeInnerLeft=pNode;
				else if(pNode->xPosition.y>keynodes->zm.pArmBeardJointNodeInnerLeft->xPosition.y)
					keynodes->zm.pArmBeardJointNodeInnerLeft=pNode;
			}
			else if(fabs(pNode->xPosition.x+keynodes->zm.pArmPostJointNodeRight->xPosition.x)+
					fabs(pNode->xPosition.z-keynodes->zm.pArmPostJointNodeRight->xPosition.z)<EPS2)
			{
				if( keynodes->zm.pArmPostJointNodeLeft==NULL)
					keynodes->zm.pArmPostJointNodeLeft=pNode;
				else if(pNode->xPosition.y>keynodes->zm.pArmPostJointNodeLeft->xPosition.y)
					keynodes->zm.pArmPostJointNodeLeft=pNode;
			}
			else if(fabs(pNode->xPosition.x+keynodes->zm.pArmBeamJointNodeOutRight->xPosition.x)+
					fabs(pNode->xPosition.z-keynodes->zm.pArmBeamJointNodeOutRight->xPosition.z)<EPS2)
			{
				if( keynodes->zm.pArmBeamJointNodeOutLeft==NULL)
					keynodes->zm.pArmBeamJointNodeOutLeft=pNode;
				else if(pNode->xPosition.y>keynodes->zm.pArmBeamJointNodeOutLeft->xPosition.y)
					keynodes->zm.pArmBeamJointNodeOutLeft=pNode;
			}
			else if(fabs(pNode->xPosition.x+keynodes->zm.pArmBeamJointNodeInnerRight->xPosition.x)+
					fabs(pNode->xPosition.z-keynodes->zm.pArmBeamJointNodeInnerRight->xPosition.z)<EPS2)
			{
				if( keynodes->zm.pArmBeamJointNodeInnerLeft==NULL)
					keynodes->zm.pArmBeamJointNodeInnerLeft=pNode;
				else if(pNode->xPosition.y>keynodes->zm.pArmBeamJointNodeInnerLeft->xPosition.y)
					keynodes->zm.pArmBeamJointNodeInnerLeft=pNode;
			}
		}
	}
	return true;
}
bool CIntelliCore::RecogStdArmTemplByCoreImage(STDTEMPL* pLiveArm,CMonoImage* pMonoImage,
	char ciArmsideOrHead,GECS& ocs,double scaleImage2Model/*=0.01*/)
{
	int xI,yJ,width=pMonoImage->GetImageWidth(),height=pMonoImage->GetImageHeight();
	//�Һᵣ�������м�����������ɨ�裬�����ݶ�5���ҵ������Ҹ˼��ϣ����γ�
	short STEP_X=5,STEP_Y=5;
	PIXEL_RELAOBJ *pPixelRelaObj,*pTmpRelaObj=NULL,*pUpperRelaObj=NULL,*pDownRelaObj=NULL,*pRightRelaObj=NULL;
	int yTopJ,yBtmJ;
	CXhPtrSet<IModelRod> rodset,upperPostRodSet,downPostRodSet;
	IModelRod* pPrevUpperRod=NULL,*pPrevDownRod=NULL,*pPrevRod=NULL,*pRod=NULL;
	if(ciArmsideOrHead!='L'&&ciArmsideOrHead!='R'&&ciArmsideOrHead!='U')
		return false;	//�쳣��ͷ��ᵣ�����ַ�

	GECS lcs;
	lcs.axis_z.Set(0,0,1);
	pLiveArm->ClearMatchInfo();
	WORD widSeed=1;
	STDTEMPL::MATCHNODE* pMatchNode=NULL;
	STDTEMPL::MATCHROD*  pPrevMatchRod =NULL,*pMatchRod =NULL;

	SCAN_CONTEXT context;
	short siCurrScanPointX=0,siCurrScanPointY=0;
	short siMidPointX=pMonoImage->Width/2;
	short siLeftTrunkPointX=siMidPointX,siRightTrunkPointX=siMidPointX;
	context.pMonoImage=pMonoImage;
	//1.Ѱ��������ɨ�����ʼ�������
	if(ciArmsideOrHead=='U')
	{	//1.1Ѱ����ͷ��������ʼ�ڵ�(�ײ����������ӵ��Ҳ��νӵ�)
		pPixelRelaObj=NULL;
		int xPickLineStartI,yPickLineStartJ;
		int xRightI=width-1;
		int yUpperJ=height-STEP_Y*2;
		pPixelRelaObj=NULL;
		for(xI=width-1;xI>siMidPointX;xI--)
		{
			for(yJ=height-1;yJ>=yUpperJ;yJ--)
			{
				if((pTmpRelaObj=pMonoImage->GetPixelRelaObj(xI,yJ))==NULL)
					continue;
				yUpperJ=max(yUpperJ,yJ-1);
				if(pPixelRelaObj==NULL||(pPixelRelaObj->hRod!=pTmpRelaObj->hRod))
				{
					pPixelRelaObj=pTmpRelaObj;
					xPickLineStartI=xI;
					yPickLineStartJ=yJ;
				}
				else if(pPixelRelaObj->hRod==pTmpRelaObj->hRod)
				{
					if(abs(xI-xPickLineStartI)+abs(yJ-yPickLineStartJ)>=5)
					{	//����abs(dx)+abs(dy)���������д���Ϊ�ж���������Ҫ���ڲ��־Ʊ��������۱Ƚ��¶Ƚϴ���ָ����Y������detax������>=5 wjh-2018.7.2
						xRightI=xI;
						pRightRelaObj=pPixelRelaObj;
					}
				}
				break;
			}
		}
		if(pRightRelaObj==NULL||(pRod=m_pModel->FromRodHandle(pRightRelaObj->hRod))==NULL)
			return false;
	}
	else
	{	//1.2Ѱ��180��������ɨ��ĺᵣ���²��������
		for(xI=0;xI<width;xI+=STEP_X)
		{
			pUpperRelaObj=pDownRelaObj=NULL;
			bool bFindProfilePixel=false;
			for(yJ=0;yJ<height;yJ++)
			{
				if((pPixelRelaObj=pMonoImage->GetPixelRelaObj(xI,yJ))==NULL)
				{	//bFindProfilePixel�ж���Ϊ�˱���һЩб���������������˼�ĸ��ţ�һ������нǻ���ֿհ׵�)
					if(bFindProfilePixel&&yJ>yTopJ+3)
						break;	//���ҵ����Ҹˣ���3��������δ�ҵ������ʵ����Ҹˣ����˳�����ѭ��
					continue;
				}
				bFindProfilePixel=true;
				if(pUpperRelaObj==NULL)
				{
					pUpperRelaObj=pPixelRelaObj;
					yTopJ=yJ;
				}
				else if(pUpperRelaObj->hRod==pPixelRelaObj->hRod)
					pUpperRelaObj=pPixelRelaObj;
				else if(yJ<yTopJ+3&&pPixelRelaObj->zDepth>pUpperRelaObj->zDepth)
					pUpperRelaObj=pPixelRelaObj;
				else if(yJ>=yTopJ+3)
					break;
			}
			bFindProfilePixel=false;
			for(yJ=height-1;yJ>=0;yJ--)
			{
				if((pPixelRelaObj=pMonoImage->GetPixelRelaObj(xI,yJ))==NULL)
				{
					if(bFindProfilePixel&&yJ<yBtmJ-3)
						break;	//���ҵ����Ҹˣ���3��������δ�ҵ������ʵ����Ҹˣ����˳�����ѭ��
					continue;
				}
				bFindProfilePixel=true;
				if(pDownRelaObj==NULL)
				{
					pDownRelaObj=pPixelRelaObj;
					yBtmJ=yJ;
				}
				else if(pDownRelaObj->hRod==pPixelRelaObj->hRod)
					pDownRelaObj=pPixelRelaObj;
				else if(yJ>yBtmJ-3&&pPixelRelaObj->hRod!=pDownRelaObj->hRod)
				{	//��ǰ��һ����������߲Ľ�����
					xI+=STEP_X;
					yJ=height;
					pDownRelaObj=NULL;
				}
				else if(yJ<=yBtmJ-3)
					break;
			}
			if(pDownRelaObj!=NULL&&abs(yBtmJ-yTopJ)<STEP_Y)
				pDownRelaObj=NULL;	//���±߽����غϣ����ǵ�����������խ��Ϊ�����Ҹ� wjh-2018.5.4
			siCurrScanPointX=xI;
			siCurrScanPointY=max(0,yBtmJ);
			if(pDownRelaObj!=NULL)
			{
				context.vCurrLenStdVec.Set(1,0,0);
				context.siCurrScanPointX=xI;
				context.siCurrScanPointY=yBtmJ;
				SEARCH_RESULT search;
				VERIFY_PIXEL verify(pDownRelaObj->hRod);
				if(SearchNextPofileEdgeRod(context,&search,2))
					break;	//�ҵ��ᵣ���²�������ֱ���ӵ��������ĸ˼�
			}
		}
		if(pDownRelaObj==NULL||(pRod=m_pModel->FromRodHandle(pDownRelaObj->hRod))==NULL)
			return false;
	}
	rodset.append(pRod);	//�����ʱ���һ���������˼���������ֱ�����������ģ�
	context.hashRecogRods.SetValue(pRod->handle,pRod);
	pPrevRod=pRod;
	GEPOINT lenStdVec,vPrevLenStdVec;		//��Զ���Ƕ�Ӧ�˼���ʼ->�շ���
	GEPOINT lenStdVecEx,vPrevLenStdVecEx;	//�ظ˼�����������ʱ�뷽��
	GELINE prevline,briefline;
	prevline.start=ocs.TransPToCS(pRod->pStart->xPosition);
	prevline.end  =ocs.TransPToCS(pRod->pEnd->xPosition);
	vPrevLenStdVec.Set(prevline.end.x-prevline.start.x,prevline.end.y-prevline.start.y);
	normalize(vPrevLenStdVec);
	vPrevLenStdVecEx=vPrevLenStdVec;

	//����׸������ڵ㣨�����Ҹ�������ֱ���ڵ㣩
	briefline=prevline;
	pMatchNode=pLiveArm->hashMatchNodes.Add(widSeed);
	widSeed++;
	if(ciArmsideOrHead=='U')
	{
		if(briefline.start.y>briefline.end.y+EPS)
			pMatchNode->pIntelliNode=pRod->pStart;
		else if(briefline.start.y<briefline.end.y-EPS)
		{
			pMatchNode->pIntelliNode=pRod->pEnd;
			vPrevLenStdVecEx=-vPrevLenStdVec;
		}
		else if(briefline.start.x<briefline.end.x-EPS)
			pMatchNode->pIntelliNode=pRod->pStart;
		else //if(briefline.start.x>briefline.end.x+EPS)
		{
			pMatchNode->pIntelliNode=pRod->pEnd;
			vPrevLenStdVecEx=-vPrevLenStdVec;
		}
	}
	else
	{
		if(briefline.start.x<briefline.end.x)
			pMatchNode->pIntelliNode=pRod->pStart;
		else
		{
			pMatchNode->pIntelliNode=pRod->pEnd;
			vPrevLenStdVecEx=-vPrevLenStdVec;
		}
	}
	pMatchNode->hRelaModelNode=pMatchNode->pIntelliNode->handle;
	CalImgPoint(&siCurrScanPointX,&siCurrScanPointY,pMatchNode->pIntelliNode->xPosition,ocs,scaleImage2Model);
	if(ciArmsideOrHead=='U')	//���������ʵ��X����
		siLeftTrunkPointX=siMidPointX-(siCurrScanPointX-siMidPointX);
	//2.�����Ҹ˿�ʼ����ɨ��
	double TOLERANCE_COSA=0.999999;//1/SQRT(1+0.001*0.001)
	bool bScanByX=true;
	short siStepMajor=STEP_X;	//ɨ���������ɨ�貽��
	short siStepMinor=-1;			//��ֱɨ��������Ĳ�����һ��ȡ1��-1
	SEARCH_RESULT search;
	search.step.major.vfStdScanX= 1;
	search.step.major.vfStdScanY= 0;
	search.step.minor.vfStdScanX= 0;
	search.step.minor.vfStdScanY=-1;
	search.step.siStepMajor= 5;
	search.step.siStepMinor=-1;
	search.majorStartI=0;
	search.majorEndI=width-1;
	search.minorStartJ=-5;
	search.minorEndJ=height-1;
	search.xHitI=siCurrScanPointX;
	search.yHitJ=siCurrScanPointY;
	bool bFindFarProfileNode=false;
	context.vCurrLenStdVec=vPrevLenStdVecEx;
	context.hCurrRod=pRod->handle;
	context.ocs=ocs;
	UINT xiPrevKeyPointI=siCurrScanPointX,yjPrevKeyPointJ=siCurrScanPointY;
	do{
		IModelNode* pMatchModelNode=NULL;
		int scanhits=0;
		long hHitRod=NULL;
		SEARCH_RESULT rslt;
		context.siCurrScanPointX=search.xHitI;
		context.siCurrScanPointY=search.yHitJ;
		bool finded=false;
		long hits=0;
		int vCurrOffsetX=0,vCurrOffsetY=0,xPrevPointX=search.xHitI,yPrevPointY=search.yHitJ,error_iter=0;;
		do{
			int deltax=context.siCurrScanPointX-xiPrevKeyPointI;
			int deltay=context.siCurrScanPointY-yjPrevKeyPointJ;
			if(abs(deltax)+abs(deltay)>10)
				context.siStartScanAngle=15;
			if(finded=SearchNextPofileEdgeRod(context,&rslt))
			{
				bool turnright=
					context.vCurrLenStdVec.x*(rslt.yHitJ-context.siCurrScanPointY)-
					context.vCurrLenStdVec.y*(rslt.xHitI-context.siCurrScanPointX)>0.01;
				if(!turnright&&rslt.hRod!=pPrevRod->handle)
				{
					SEARCH_RESULT rslt2;
					context.siCurrScanPointX+=(short)ftoi(context.vCurrLenStdVec.x*8);
					context.siCurrScanPointY+=(short)ftoi(context.vCurrLenStdVec.y*8);
					if( context.siCurrScanPointX<pMonoImage->GetImageWidth()&&
						context.siCurrScanPointY>0&&context.siCurrScanPointY<pMonoImage->GetImageHeight()&&
						SearchNextPofileEdgeRod(context,&rslt2)&&rslt2.hRod==pPrevRod->handle)
						rslt=rslt2;	//���ڲ�б���Ķ˸�������ļ���ձ߽�˼���
				}
				if(vCurrOffsetX==0&&vCurrOffsetY==0)
				{
					vCurrOffsetX=rslt.xHitI-xPrevPointX;
					vCurrOffsetY=rslt.yHitJ-yPrevPointY;
				}
				else
				{
					int vStepX=rslt.xHitI-xPrevPointX;
					int vStepY=rslt.yHitJ-yPrevPointY;
					if(rslt.hRod==pPrevRod->handle&&(vStepX*vCurrOffsetX+vStepY*vCurrOffsetY)<0)
						int b=3;//return false;//���ַ�������ɨ�裬�������
				}
				xPrevPointX=context.siCurrScanPointX=rslt.xHitI;
				yPrevPointY=context.siCurrScanPointY=rslt.yHitJ;
				if(pLiveArm->ciOnRightSide!='U'&&bFindFarProfileNode&&rslt.xHitI<5)
					finded=false;	//���ҵ��ᵣ�������ϲ����ӵ㣬���ڼ�������
			}
			error_iter++;
			if(error_iter>10000)
				return false;
		}while(finded&&rslt.hRod==pPrevRod->handle);
		if(finded)
		{
			pRod=m_pModel->FromRodHandle(rslt.hRod);
			//�������˼������ֻ�����ʱӦ�ж��Ƿ���ڷǹ��ߵ������ǽڵ����
			briefline.start=ocs.TransPToCS(pRod->pStart->xPosition);
			briefline.end  =ocs.TransPToCS(pRod->pEnd->xPosition);
			lenStdVec.Set(briefline.end.x-briefline.start.x,briefline.end.y-briefline.start.y);
			normalize(lenStdVec);
			double justify=vPrevLenStdVecEx*lenStdVec;
			if(fabs(justify)>TOLERANCE_COSA)	//
			{
				pPrevRod=pRod;
				prevline=briefline;
				vPrevLenStdVec= lenStdVec;
				if(justify<0)
					lenStdVecEx=-lenStdVec;
				else
					lenStdVecEx=lenStdVec;
				vPrevLenStdVecEx= lenStdVecEx;
				search.majorStartI=5;
				search.majorEndI=width*2;
				search.lenStdVec= lenStdVec;
				search.step.major.vfStdScanX=lenStdVecEx.x;
				search.step.major.vfStdScanY=lenStdVecEx.y;
				search.step.UpdateMinorDirection();
				search.minorStartJ=-5;
				search.minorEndJ=5;
				search.xHitI=rslt.xHitI;
				search.yHitJ=rslt.yHitJ;
				pMatchModelNode=justify>0?pRod->pStart:pRod->pEnd;
				//CalImgPoint(&siCurrScanPointX,&siCurrScanPointY,pMatchModelNode->xPosition,ocs,scaleImage2Model);
				if(context.hashRecogRods.GetValue(pRod->handle)==NULL)
				{
					rodset.append(pRod);
					context.hashRecogRods.SetValue(pRod->handle,pRod);
				}
				continue;	//����
			}
			if(pPrevRod->pStart==pRod->pStart||pPrevRod->pEnd==pRod->pStart)
				pMatchModelNode=pRod->pStart;
			else if(pPrevRod->pStart==pRod->pEnd||pPrevRod->pEnd==pRod->pEnd)
			{
				pMatchModelNode=pRod->pEnd;
				lenStdVecEx=-lenStdVec;
			}
			if(pMatchModelNode==NULL)
			{	//���˼�û�й�ͬ�Ķ˽ڵ�,�жϵ�ǰ�˼��Ƿ����ӵ�ǰһ�˼��м�ڵ���
				short judgeStart=SideOfPtLine(briefline.start,prevline.start,prevline.end,1);
				short judgeEnd  =SideOfPtLine(briefline.end  ,prevline.start,prevline.end,1);
				double fraction;
				GEPOINT lenvec =prevline.end-prevline.start;
				if(judgeStart==0)
				{
					GEPOINT distvec=briefline.start-prevline.start;
					if(fabs(lenvec.x)>=fabs(lenvec.y)&&fabs(lenvec.x)>=fabs(lenvec.z))
						fraction=distvec.x/lenvec.x;
					else if(fabs(lenvec.y)>=fabs(lenvec.x)&&fabs(lenvec.y)>=fabs(lenvec.z))
						fraction=distvec.y/lenvec.y;
					else //if(fabs(lenvec.z)>=fabs(lenvec.x)&&fabs(lenvec.z)>=fabs(lenvec.y))
						fraction=distvec.z/lenvec.z;
					if(fraction>-EPS&&fraction<1+EPS)
						pMatchModelNode=pRod->pStart;
				}
				else if(judgeEnd==0)
				{	//�ն�Ϊ��ǰ�����ǽڵ㣬��ǰ���췽��Ӧ��Ϊ��->ʼ
					GEPOINT distvec=briefline.end-prevline.start;
					if(fabs(lenvec.x)>=fabs(lenvec.y)&&fabs(lenvec.x)>=fabs(lenvec.z))
						fraction=distvec.x/lenvec.x;
					else if(fabs(lenvec.y)>=fabs(lenvec.x)&&fabs(lenvec.y)>=fabs(lenvec.z))
						fraction=distvec.y/lenvec.y;
					else //if(fabs(lenvec.z)>=fabs(lenvec.x)&&fabs(lenvec.z)>=fabs(lenvec.y))
						fraction=distvec.z/lenvec.z;
					if(fraction>-EPS&&fraction<1+EPS)
					{
						pMatchModelNode=pRod->pEnd;
						lenStdVecEx*=-1.0;
					}
				}
			}
			if(pMatchModelNode==NULL)
			{	//���˼�û�й�ͬ�Ķ˽ڵ�,�ж�ǰһ�˼��Ƿ����ӵ���ǰ�˼��м�ڵ���
				short judgeStart=SideOfPtLine(prevline.start,briefline.start,briefline.end,1);
				short judgeEnd  =SideOfPtLine(prevline.end  ,briefline.start,briefline.end,1);
				double fraction;
				GEPOINT lenvec =briefline.end-briefline.start;
				if(judgeStart==0)
				{
					GEPOINT distvec=prevline.start-briefline.start;
					if(fabs(lenvec.x)>=fabs(lenvec.y)&&fabs(lenvec.x)>=fabs(lenvec.z))
						fraction=distvec.x/lenvec.x;
					else if(fabs(lenvec.y)>=fabs(lenvec.x)&&fabs(lenvec.y)>=fabs(lenvec.z))
						fraction=distvec.y/lenvec.y;
					else //if(fabs(lenvec.z)>=fabs(lenvec.x)&&fabs(lenvec.z)>=fabs(lenvec.y))
						fraction=distvec.z/lenvec.z;
					if(fraction>-EPS&&fraction<1+EPS)
						pMatchModelNode=pPrevRod->pStart;
				}
				else if(judgeEnd==0)
				{
					GEPOINT distvec=prevline.end-briefline.start;
					if(fabs(lenvec.x)>=fabs(lenvec.y)&&fabs(lenvec.x)>=fabs(lenvec.z))
						fraction=distvec.x/lenvec.x;
					else if(fabs(lenvec.y)>=fabs(lenvec.x)&&fabs(lenvec.y)>=fabs(lenvec.z))
						fraction=distvec.y/lenvec.y;
					else //if(fabs(lenvec.z)>=fabs(lenvec.x)&&fabs(lenvec.z)>=fabs(lenvec.y))
						fraction=distvec.z/lenvec.z;
					if(fraction>-EPS&&fraction<1+EPS)
						pMatchModelNode=pPrevRod->pEnd;
				}
				if(pMatchModelNode&&(vPrevLenStdVecEx.x*lenStdVec.y-vPrevLenStdVecEx.y*lenStdVec.x<0))
					lenStdVecEx=-lenStdVec;	//ͨ��ʸ��������ж�lenStdVec�Ƿ�ָ��vPrevLenStdVec�Ҳ�
			}
			if(pMatchModelNode==NULL)
			{	//cal inters point
				f3dPoint interspoint;
				if(Int3dpl(f3dLine(prevline.start,prevline.end),f3dLine(briefline.start,briefline.end),interspoint,1)>0)
				{
					GEPOINT vps=GEPOINT(interspoint)-prevline.start;
					GEPOINT vpe=GEPOINT(interspoint)-prevline.end;
					GEPOINT vcs=GEPOINT(interspoint)-briefline.start;
					GEPOINT vce=GEPOINT(interspoint)-briefline.end;
					if((vps*vpe<=EPS)&&(vcs*vce<=EPS))
					{	//�����ڸ˼��ڵ�
						interspoint=ocs.TransPFromCS(interspoint);
						for(IModelNode* pModelNode=m_pModel->EnumNodeFirst();pModelNode;pModelNode=m_pModel->EnumNodeNext())
						{
							if(pModelNode->xPosition.IsEqual(interspoint,1))
							{
								pMatchModelNode=pModelNode;
								break;
							}
						}
					}
				}
			}
		}
		if(pMatchModelNode==NULL)
		{
			if(search.step.major.vfStdScanX*vPrevLenStdVec.x+search.step.major.vfStdScanY*vPrevLenStdVec.y>0)
				pMatchModelNode=pPrevRod->pEnd;
			else
				pMatchModelNode=pPrevRod->pStart;
		}
		pMatchNode=pLiveArm->hashMatchNodes.Add(widSeed);
		widSeed++;
		pMatchNode->pIntelliNode=pMatchModelNode;
		pMatchNode->hRelaModelNode=pMatchModelNode->handle;
		
		//ɨ���ת���������ڵ㴦
		CalImgPoint(&siCurrScanPointX,&siCurrScanPointY,pMatchNode->pIntelliNode->xPosition,ocs,scaleImage2Model);
		if(ciArmsideOrHead=='U')
		{
			if(bFindFarProfileNode&&siCurrScanPointY>=height-5&&siCurrScanPointX>=siLeftTrunkPointX-5)
				break;	//ɨ�����
			else if(siCurrScanPointY<height-30)
				bFindFarProfileNode=true;	//��ͷ�߶Ȳ�����С��30/0.015=2000mm����ʱ�������ҵ���һ������ƿ��ˮƽ��һ������ĸߵ� wjh-2018.5.26
		}
		else
		{
			if(bFindFarProfileNode&&siCurrScanPointX<=5)
				break;	//ɨ�����
			else if(siCurrScanPointX>5)
				bFindFarProfileNode=true;
		}
		//����ԭɨ�跽���������ж�,���Һ��ʵ����������Ӹ˼�
		context.siCurrScanPointX=siCurrScanPointX;
		context.siCurrScanPointY=siCurrScanPointY;//��ǰ��������ʼ��
		context.vCurrLenStdVec=vPrevLenStdVecEx;//��ǰɨ������᷽��
		context.hCurrRod=pPrevRod->handle;
		context.ocs=ocs;
		GEPOINT xCurrPoint=context.ocs.TransPToCS(pMatchNode->pIntelliNode->xPosition);
		CPtInLineCheck check;
		bool online=false;
		long hSearchNextRod=0;
		BYTE PREFER_SCANRADIUS=5;
		int deltax=context.siCurrScanPointX-xiPrevKeyPointI;
		int deltay=context.siCurrScanPointY-yjPrevKeyPointJ;
		xiPrevKeyPointI=siCurrScanPointX;
		yjPrevKeyPointJ=siCurrScanPointY;
		if(abs(deltax)+abs(deltay)>10)
			context.siStartScanAngle=15;
		do{
			//SCAN_CONTEXT tmp(&context);
			if(!SearchNextPofileEdgeRod(context,&search,PREFER_SCANRADIUS))
			{
				PREFER_SCANRADIUS-=1;
				continue;
			}
			if(search.hRod==pPrevRod->handle)
				online=false;
			else
			{
				if(hSearchNextRod==0||hSearchNextRod!=search.hRod)
				{
					hSearchNextRod=search.hRod;
					check.SetLine(search.lineStart,search.lineEnd);
				}
				double scale=check.CheckPointEx(xCurrPoint,&online,xTolerance.distance,xTolerance.tangent);
				if(scale<-EPS||scale>1+EPS)
					online=false;
			}
			if(!online)
				PREFER_SCANRADIUS-=1;
		}while(PREFER_SCANRADIUS>1&&!online);
		if(search.pRod==NULL)
			return false;
		//���㵱ǰ�����㴦�������ڼн�
		lcs.axis_x=vPrevLenStdVecEx;
		lcs.axis_y.Set(lcs.axis_x.y,-lcs.axis_x.x);
		GEPOINT vxNextRodVec=lcs.TransVToCS(search.lenStdVec);
		if(search.lenStdVec.x*search.step.major.vfStdScanX+search.lenStdVec.y*search.step.major.vfStdScanY<0)
			vxNextRodVec*=-1;
		pMatchNode->siActualDegAngle=180-ftoi(Cal2dLineAng(0,0,vxNextRodVec.x,vxNextRodVec.y)*DEGTORAD_COEF);
		if(pMatchNode->siActualDegAngle<0)
			pMatchNode->siActualDegAngle+=360;

		pPrevRod=search.pRod;
		rodset.append(search.pRod);
		context.hashRecogRods.SetValue(search.pRod->handle,search.pRod);
		prevline.start=search.lineStart;
		prevline.end=search.lineEnd;
		vPrevLenStdVec=search.lenStdVec;
		if(search.lenStdVec.x*search.step.major.vfStdScanX+search.lenStdVec.y*search.step.major.vfStdScanY>0)
			vPrevLenStdVecEx=vPrevLenStdVec;
		else
			vPrevLenStdVecEx=-vPrevLenStdVec;
		context.vCurrLenStdVec=vPrevLenStdVecEx;
		context.hCurrRod=search.pRod->handle;
		if(pMatchNode->siActualDegAngle>180)
			context.siStartScanAngle=180-(360-pMatchNode->siActualDegAngle)+5;
		else
			context.siStartScanAngle=15;
	}while(widSeed<50);
#if defined(__PUBLIC_RELEASE_COMPILE_)&&defined(_DEBUG)
	if(PRODUCT_FUNC::IsHasInternalTest())
	{
	for(pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
		pLogErr->Log("0x%x�Ҹ˼���ʶ��",pRod->handle);
	bool firstNode=true;
	for(pMatchNode=pLiveArm->hashMatchNodes.GetFirst();pMatchNode;pMatchNode=pLiveArm->hashMatchNodes.GetNext())
	{
		CXhChar16 anglestr;
		if(pMatchNode->siActualDegAngle>0)
			anglestr.Printf("�н�=%d",pMatchNode->siActualDegAngle);
		else if(firstNode)
			anglestr.Copy("��ʿ�ʼ");
		else
			anglestr.Copy("̧�ʽ���");
		firstNode=false;
		pLogErr->SetCurrTempRelation(0,pMatchNode->hRelaModelNode);
		pLogErr->Log("%2d{0x%X}#�����ڵ���ʶ��, %s",pMatchNode->widNode,pMatchNode->hRelaModelNode,(char*)anglestr);
		pLogErr->SetCurrTempRelation(0,0);
	}
	}
#endif
	//�жϵ�ǰ��ͷ���޿���Ϊèͷ��Ʊ�
	if(pLiveArm->ciOnRightSide!='U')
		pLiveArm->bMaybeHeadZBorZM=false;
	else
	{
		int yStartJ=pMonoImage->GetImageHeight()/10;
		int yEndJ=pMonoImage->GetImageHeight()-yStartJ;
		pLiveArm->bMaybeHeadZBorZM=false;
		for(int yJ=yStartJ;yJ<yEndJ;yJ++)
		{
			bool bBandActive=false;
			int band_count=0,bandStart=0,bandEnd=0,maxband_width=0;
			for(int xI=0;xI<pMonoImage->GetImageWidth();xI++)
			{
				if(pMonoImage->IsBlackPixel(xI,yJ))
				{
					if(!bBandActive)
					{
						bBandActive=true;
						bandStart=bandEnd=xI;
					}
					else
						bandEnd=xI;
				}
				else if(bBandActive)
				{
					band_count++;
					maxband_width=max(maxband_width,bandEnd-bandStart+1);
					bBandActive=false;
					if(band_count>2)
						break;
				}
			}
			if(band_count==2&&maxband_width<3)
			{	//�ҵ��������۽��ӵ�
				pLiveArm->bMaybeHeadZBorZM=true;
				break;
			}
		}
	}
	for(int i=0;i<PREDEF_ARM_COUNT;i++)
	{
		if(MatchWith(&ARM_ARR[i],pLiveArm))
		{
			pLiveArm->CopyStdTemplFrom(&ARM_ARR[i]);
			KEY_NODE keynodes;
			if(pLiveArm->ciClassicType=='M')
				RecogZBorZMInnerKeyNodes(pLiveArm,pMonoImage,ocs,&keynodes);
			RetrieveLiveStdArmTemplRods(pLiveArm,ocs,keynodes);
			pLiveArm->ciOnRightSide=ciArmsideOrHead;
#if defined(__PUBLIC_RELEASE_COMPILE_)&&defined(_DEBUG)
			if(PRODUCT_FUNC::IsHasInternalTest())
			{
			CXhChar16 sidestr=ciArmsideOrHead=='R'?"�Ҳ�ᵣ":"���ᵣ";
			if(ciArmsideOrHead=='U')
				sidestr="��ͷ";
			pLogErr->Log("%s����%s��ʶ�����а����Ҹ�:",(char*)pLiveArm->name,(char*)sidestr);
			//for(pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
			//	logerr.Log("0x%x�Ҹ˼���ʶ��",pRod->handle);
			for(pMatchRod=pLiveArm->hashMatchRods.GetFirst();pMatchRod;pMatchRod=pLiveArm->hashMatchRods.GetNext())
			{
				CXhChar50 idstr;
				long hRelaRodArr[2]={0};
				for(UINT j=0;j<pMatchRod->relaRods.Count;j++)
				{
					long hRod=pMatchRod->relaRods.At(j).pIntelliRod->handle;
					CXhChar16 subidstr("0x%X",hRod);
					idstr.Append(subidstr,';');
					if(j<2)
						hRelaRodArr[j]=hRod;
				}
				pLogErr->SetCurrTempRelation(0,hRelaRodArr[0],hRelaRodArr[1]);
				pLogErr->Log("rod id=#%d��ʶ��:%s",pMatchRod->widRod,(char*)idstr);
				pLogErr->SetCurrTempRelation(0,0);
			}
			}
#endif
			return true;
		}
	}
#if defined(__PUBLIC_RELEASE_COMPILE_)&&defined(_DEBUG)
	if(PRODUCT_FUNC::IsHasInternalTest())
	{/*
		char* excludeNodeStr="5,6,16,17";
		char* excludeRodStr ="5,6,16,17";
		CreateHeadTemplCode(pLiveArm,excludeNodeStr,excludeRodStr);*/
	}
#endif
	return false;
}
//��ȡ��ģ��ƥ����������˼�����
int _LocalCompareEdgeRodMember(const STDTEMPL::EDGEROD& rodmember1,const STDTEMPL::EDGEROD& rodmember2)
{
	if(rodmember1.lenpos>rodmember2.lenpos)
		return 1;
	else if(rodmember1.lenpos<rodmember2.lenpos)
		return -1;
	else
		return 0;
}
bool CIntelliCore::RetrieveLiveEdgeRods(STDTEMPL::MATCHNODE *pMatchStart,STDTEMPL::MATCHNODE* pMatchEnd,GECS ocs,CXhSimpleList<STDTEMPL::EDGEROD>* pListEdgeRods)
{
	IModelRod* pRod;
	IModelNode* pStartNode=pMatchStart->pIntelliNode;
	IModelNode* pEndNode  =pMatchEnd->pIntelliNode;
	if(pStartNode==NULL||pEndNode==NULL)
		return false;
	GEPOINT axis_x=pEndNode->xPosition-pStartNode->xPosition;
	ocs.origin=pStartNode->xPosition;
	bool bAxisYRod=false;
	if(fabs(axis_x.x)+fabs(axis_x.z)<EPS)
	{
		axis_x.y=axis_x.y>0?1:-1;
		axis_x.x=axis_x.z=0;
		ocs.axis_x=axis_x;
		ocs.axis_y.Set(0,0,-1);
		ocs.axis_z=ocs.axis_x^ocs.axis_y;
		bAxisYRod=true;
	}
	else
	{
		axis_x=ocs.TransVToCS(axis_x);
		axis_x.z=0;
		axis_x=ocs.TransVFromCS(axis_x);
		normalize(axis_x);
		ocs.axis_x=axis_x;
		ocs.axis_y=ocs.axis_z^ocs.axis_x;
	}
	GEPOINT xEndTip=ocs.TransPToCS(pEndNode->xPosition);
	if(xEndTip.x<1)
		return false;
	double scaleimg2model=min(0.01,1000/xEndTip.x);
	long i,MAX_I=0;
	PIXEL_RELAOBJ pixelline[1001];
	for(pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		GEPOINT lenVecStart=pRod->pStart->xPosition-ocs.origin;
		GEPOINT lenVecEnd  =pRod->pEnd->xPosition-ocs.origin;
		lenVecStart=ocs.TransVToCS(lenVecStart);
		double TEMP_EPS=1;	//��ʱ�����Ժ�����Ľڵ㾫�Ȳ������ʴ˴�ȡ�������ֵΪ0.1����Eps wj-2018.7.9
		if(!bAxisYRod&&fabs(lenVecStart.y)>TEMP_EPS)	//+fabs(lenVecStart.z)
			continue;	//�˼�δ��ͶӰ�ڵ�ǰ��������
		else if( bAxisYRod&&fabs(lenVecStart.y)+fabs(lenVecStart.z)>TEMP_EPS)	//+fabs(lenVecStart.z)
			continue;	//�˼�δ��ͶӰ�ڵ�ǰ��������
		lenVecEnd=ocs.TransVToCS(lenVecEnd);
		if(!bAxisYRod&&fabs(lenVecEnd.y)>TEMP_EPS)//EPS)	//+fabs(lenVecStart.z)
			continue;	//�˼�δ��ͶӰ�ڵ�ǰ��������
		else if( bAxisYRod&&fabs(lenVecEnd.y)+fabs(lenVecEnd.z)>TEMP_EPS)	//+fabs(lenVecStart.z)
			continue;	//�˼�δ��ͶӰ�ڵ�ǰ��������
		int min_x=ftoi(min(lenVecStart.x,lenVecEnd.x));
		int max_x=ftoi(max(lenVecStart.x,lenVecEnd.x));
		if(max_x<1||min_x>xEndTip.x-1)
			continue;//���ڵ�ǰ�������߶η�Χ��
		if(bAxisYRod)
		{
			pListEdgeRods->Append(STDTEMPL::EDGEROD(pRod,0));
			return true;
		}
		bool reversed=lenVecStart.x>lenVecEnd.x;//�˼�ʼ��ʼ->�շ����������߶��巽���෴
		long xStartI=max(  0,ftoi(min_x*scaleimg2model));
		long xEndI  =min(1000,ftoi(max_x*scaleimg2model));
		MAX_I=max(MAX_I,xEndI);
		double scale_of_z2x=(lenVecEnd.z-lenVecStart.z)/(lenVecEnd.x-lenVecStart.x);
		PIXEL_RELAOBJ pixel(pRod->handle,0,false);
		pixel.bReverseDirection=reversed;
		for(long xI=xStartI;xI<=xEndI;xI++)
		{
			pixel.bTipPixel=(xI==xStartI||xI==xEndI-1);
			double x=xI/scaleimg2model;
			pixel.zDepth=(float)(lenVecStart.z+(x-lenVecStart.x)*scale_of_z2x);
			if(pixelline[xI].hRod!=0)
			{
				if(pixelline[xI].zDepth>pixel.zDepth)
					continue;	//�õ��ѻ����˸���ǰ�ĸ˼�
				else if(fabs(pixelline[xI].zDepth-pixel.zDepth)<0.1&&(!pixelline[xI].bTipPixel||pixel.bTipPixel))
					continue;
			}
			pixelline[xI]=pixel;
		}
	}
	CHashList<PIXEL_RELAOBJ> hashRelaRods;
	pListEdgeRods->DeleteList();
	for(i=0;i<=MAX_I-3;i++)	//998=1001-3
	{
		if(pixelline[i].hRod<=0||hashRelaRods.GetValue(pixelline[i].hRod)!=NULL)
			continue;
		if(pixelline[i].hRod==pixelline[i+1].hRod&&pixelline[i].hRod==pixelline[i+2].hRod)
		{
			if((pRod=m_pModel->FromRodHandle(pixelline[i].hRod))==NULL)
				continue;
			pListEdgeRods->Append(STDTEMPL::EDGEROD(pRod,(i+1)/(float)scaleimg2model,pixelline[i].bReverseDirection));
			hashRelaRods.SetValue(pixelline[i].hRod,pixelline[i]);
		}
	}
	return true;
}
bool CIntelliCore::RetrieveLiveStdArmTemplRods(STDTEMPL* pLiveArm,GECS ocs,KEY_NODE keynodes) 
		//IModelNode* pArmPostJointNodeRight/*=NULL*/,IModelNode* pArmPostJointNodeLeft/*=NULL*/)
{
	STDTEMPL::PROFILE_NODE *pTmplNode;
	STDTEMPL::MATCHNODE *pLiveNode,*pMatchStart,*pMatchEnd;
	//KEY_NODE keynodes;
	//��ȡ�����Ľڵ㣬��ͨ��������߽��߽��㶨��Ľڵ�
	for(pTmplNode=pLiveArm->hashParaNodes.GetFirst();pTmplNode!=NULL;pTmplNode=pLiveArm->hashParaNodes.GetNext())
	{
		if(pTmplNode->ciProfileNode==1||pTmplNode->ciPosCalType<5)
			continue;
		if(abs(pTmplNode->ciPosCalType)==20)
		{	//�Ʊ��������۽����(��Ϊ�Ҳ࣬��Ϊ���)
		}
		else if(abs(pTmplNode->ciPosCalType)==21)
		{	//�Ʊ��кᵣ�����߽������½ڵ㣨��Ϊ�ϲ࣬��Ϊ�²ࣩ
		}
		else if(pTmplNode->ciPosCalType!=5)
			continue;
		if((pLiveNode=pLiveArm->hashMatchNodes.GetValue(pTmplNode->raynode.widOrgNode))==NULL)
			continue;
		//��ȡ����ԭ�㼰���ߵ����˵�
		IModelNode* pRayOrgNode,*pEdgeStartNode,*pEdgeEndNode;
		pRayOrgNode=pLiveNode->pIntelliNode;
		if((pMatchStart=pLiveArm->hashMatchNodes.GetValue(pTmplNode->raynode.widEdgeStart))==NULL)
			continue;
		pEdgeStartNode=pMatchStart->pIntelliNode;
		if((pMatchEnd=pLiveArm->hashMatchNodes.GetValue(pTmplNode->raynode.widEdgeEnd))==NULL)
			continue;
		pEdgeEndNode=pMatchEnd->pIntelliNode;
		//��ȡָ������������ȡ�ĸ˼����ϣ������ɶ�Ӧ��CPtInLineCheck���󼯺�
		CXhSimpleList<STDTEMPL::EDGEROD> listEdgeRods;//��ȡpTmplNode->raynode.widEdgeStart->pTmplNode->raynode.widEdgeEnd���ǰ̨�˼�����
		RetrieveLiveEdgeRods(pMatchStart,pMatchEnd,ocs,&listEdgeRods);
		CXhSimpleList<CPtInLineCheck> lineSegments;
		for(STDTEMPL::EDGEROD* pEdgeRod=listEdgeRods.EnumObjectFirst();pEdgeRod;pEdgeRod=listEdgeRods.EnumObjectNext())
		{
			if(pEdgeRod->bReverseDirection)
				lineSegments.Append(CPtInLineCheck(pEdgeRod->pIntelliRod->pEnd->xPosition,pEdgeRod->pIntelliRod->pStart->xPosition));
			else
				lineSegments.Append(CPtInLineCheck(pEdgeRod->pIntelliRod->pStart->xPosition,pEdgeRod->pIntelliRod->pEnd->xPosition));
		}
		//��ȡ�ڵ�Ӧ�ô��ڵĺ���Χ
		SCOPE_STRU scope;	//ԭʼģ������ϵ������
		scope.VerifyVertex(pEdgeStartNode->xPosition);
		scope.VerifyVertex(pEdgeEndNode->xPosition);
		GEPOINT xRayOrg=ocs.TransPToCS(pRayOrgNode->xPosition);
		GEPOINT lineStart=ocs.TransPToCS(pEdgeStartNode->xPosition);
		GEPOINT lineEnd=ocs.TransPToCS(pEdgeEndNode->xPosition);
		double radStartAngle=(360-pTmplNode->raynode.siMinRayAngle)*RADTODEG_COEF;
		double radEndAngle  =(360-pTmplNode->raynode.siMaxRayAngle)*RADTODEG_COEF;
		GEPOINT vRayVecStart(cos(radStartAngle),sin(radStartAngle));
		GEPOINT vRayVecEnd(cos(radEndAngle),sin(radEndAngle));
		GEPOINT inters1,inters2;
		int retcode1=Int2dpl(f2dLine((double*)lineStart,(double*)lineEnd),f2dLine((double*)xRayOrg,(double*)(xRayOrg+vRayVecStart)),inters1.x,inters1.y);
		int retcode2=Int2dpl(f2dLine((double*)lineStart,(double*)lineEnd),f2dLine((double*)xRayOrg,(double*)(xRayOrg+vRayVecEnd)),inters2.x,inters2.y);
		GEPOINT lenEdgeStdVec(lineEnd.x-lineStart.x,lineEnd.y-lineStart.y);
		normalize(lenEdgeStdVec);
		double lenScalar1=retcode1>0?lenEdgeStdVec*(inters1-lineStart):0;
		double lenScalar2=retcode2>0?lenEdgeStdVec*(inters2-lineStart):1;
		double minLenScalar=min(lenScalar1,lenScalar2);
		double maxLenScalar=max(lenScalar1,lenScalar2);

		GEPOINT axis_x=pEdgeEndNode->xPosition-pEdgeStartNode->xPosition;
		ocs.origin=pEdgeStartNode->xPosition;
		axis_x=ocs.TransVToCS(axis_x);
		axis_x.z=0;
		axis_x=ocs.TransVFromCS(axis_x);
		normalize(axis_x);
		ocs.axis_x=axis_x;
		ocs.axis_y=ocs.axis_z^ocs.axis_x;
		GEPOINT xEndTip=ocs.TransPToCS(pEdgeEndNode->xPosition);
		//��������ǰ̨�˼������ϵ�λ�ڼнǷ�Χ�ڵĽڵ㼯��
		IModelNode* pNode,*pAdaptNode=NULL;
		for(pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
		{
			if(!scope.IsIncludePoint(pNode->xPosition))
				continue;	//���нڵ㲻�������ڣ�����
			GEPOINT local=ocs.TransPToCS(pNode->xPosition);
			if(fabs(local.y)>0.1)
				continue;	//ת����ͶӰ������ϵ�󣬵㲻�ڶ�ά������
			if(local.x<minLenScalar||local.x>maxLenScalar)
				continue;	//�㲻��ָ���ļнǷ�Χ��
			for(CPtInLineCheck* pLnCheck=lineSegments.EnumObjectFirst();pLnCheck&&pAdaptNode==NULL;pLnCheck=lineSegments.EnumObjectNext())
			{
				bool bOnline=false;
				double scalar=pLnCheck->CheckPointEx(pNode->xPosition,&bOnline,0.1,0.001);
				if(bOnline&&scalar>-EPS&&scalar<1+EPS)
					pAdaptNode=pNode;	//�ҵ�һ���ʵĽڵ㣨�ڵ���ǰ�������߸˼��ϣ�
			}
			if(pAdaptNode!=NULL)
				break;
		}
		if(pNode!=NULL)
		{
			pLiveNode=pLiveArm->hashMatchNodes.Add(pTmplNode->widNode);
			pLiveNode->hRelaModelNode=pNode->handle;
			pLiveNode->pIntelliNode=pNode;
		}
	}
	for(pTmplNode=pLiveArm->hashParaNodes.GetFirst();pTmplNode!=NULL;pTmplNode=pLiveArm->hashParaNodes.GetNext())
	{
		if(pLiveArm->ciClassicType==1)
		{
			if(pTmplNode->ciPosCalType!=28)
				continue;
			if((pLiveNode=pLiveArm->hashMatchNodes.GetValue(pTmplNode->widNode))==NULL)
				continue;
			keynodes.zl.pMidBeamNode=pLiveNode->pIntelliNode;
		}
		if(pLiveArm->ciClassicType=='B')
		{
			BYTE biPosCalType=abs(pTmplNode->ciPosCalType);
			if(biPosCalType!=20&&biPosCalType!=22)
				continue;
			if((pLiveNode=pLiveArm->hashMatchNodes.GetValue(pTmplNode->widNode))==NULL)
				continue;
			if(pTmplNode->ciPosCalType==20)
				keynodes.zb.pArmPostJointNodeRight=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==-20)
				keynodes.zb.pArmPostJointNodeLeft=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==22)
				keynodes.zb.pArmBeamJointNodeOutRight=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==-22)
				keynodes.zb.pArmBeamJointNodeOutLeft=pLiveNode->pIntelliNode;
		}
		if(pLiveArm->ciClassicType=='M')
		{
			BYTE biPosCalType=abs(pTmplNode->ciPosCalType);
			if(biPosCalType<20)
				continue;
			if((pLiveNode=pLiveArm->hashMatchNodes.GetValue(pTmplNode->widNode))==NULL)
				continue;
			if(pTmplNode->ciPosCalType==20)
				keynodes.zm.pArmPostJointNodeRight=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==-20)
				keynodes.zm.pArmPostJointNodeLeft=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==22)
				keynodes.zm.pArmBeamJointNodeOutRight=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==-22)
				keynodes.zm.pArmBeamJointNodeOutLeft=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==24)
				keynodes.zm.pArmBeamUpperTipNodeRight =pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==-24)
				keynodes.zm.pArmBeamUpperTipNodeLeft=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==27)
				keynodes.zm.pArmBeamUpperInnerNodeRight =pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==-27)
				keynodes.zm.pArmBeamUpperInnerNodeLeft=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==25)
				keynodes.zm.pArmBeardJointNodeOutRight=pLiveNode->pIntelliNode;
			else if(pTmplNode->ciPosCalType==-25)
				keynodes.zm.pArmBeardJointNodeOutLeft=pLiveNode->pIntelliNode;
		}
	}

	if(pLiveArm->ciClassicType=='B')
	{
		//�Ʊ��������۽����(��Ϊ�Ҳ࣬��Ϊ���)
		//1.0���ҾƱ��кᵣ�����߽������½ڵ㣨��Ϊ�ϲ࣬��Ϊ�²ࣩ
		double minZ,maxY;
		double maxZ,maxZY;	//�жϺᵣ�����߽������ǰ��(Y+)�ڵ�����Z���꼰���Z���괦�����Y����ֵ
		bool initialized=false;
		IModelNode* pModelNode;
		for(pModelNode=this->m_pModel->EnumNodeFirst();pModelNode;pModelNode=this->m_pModel->EnumNodeNext())
		{
			if(pModelNode->xPosition.x<-1||pModelNode->xPosition.x>1)
				continue;
			if(keynodes.zb.pArmBeamJointNodeOutRight&&pModelNode->xPosition.z-keynodes.zb.pArmBeamJointNodeOutRight->xPosition.z>100)
				continue;	//�ᵣ�����߽���ؼ����Z���겻���ܵ�����������ᵣ���ӽڵ� wjh-2018.6.29
			if(!initialized)
			{
				initialized=true;
				minZ=maxZ =pModelNode->xPosition.z;
				maxY=maxZY=pModelNode->xPosition.y;
				keynodes.zb.pMidBeamTopNode=pModelNode;
				if(keynodes.zb.pArmBeamJointNodeOutRight)
					keynodes.zb.pMidBeamBtmNode=pModelNode;
			}
			else if(pModelNode->xPosition.z<minZ-0.1)
			{
				minZ=pModelNode->xPosition.z;
				maxY=pModelNode->xPosition.y;
				keynodes.zb.pMidBeamTopNode=pModelNode;
			}
			else if(pModelNode->xPosition.z<minZ+0.1)
			{
				if(pModelNode->xPosition.y>maxY)
				{
					maxY=pModelNode->xPosition.y;
					keynodes.zb.pMidBeamTopNode=pModelNode;
				}
			}

			if(keynodes.zb.pArmBeamJointNodeOutRight!=NULL)
			{
				if(pModelNode->xPosition.z>maxZ+0.1)
				{
					maxZ=pModelNode->xPosition.z;
					maxZY=pModelNode->xPosition.y;
					keynodes.zb.pMidBeamBtmNode=pModelNode;
				}
				else if(pModelNode->xPosition.z>maxZ-0.1)
				{	//����ͬһƽ����
					if(pModelNode->xPosition.y>maxZY)
					{
						maxZY=pModelNode->xPosition.y;
						keynodes.zb.pMidBeamBtmNode=pModelNode;
					}
				}
			}
		}
		if(keynodes.zb.pMidBeamTopNode!=NULL&&keynodes.zb.pMidBeamBtmNode==NULL)
		{	//1.1���ԭʼ��������δ�ҵ���������ᵣ��ཻ�ӵ�����£����ݺᵣ���߶ȼ��ᵣ�����߽������ؼ���
			initialized=false;
			for(pModelNode=m_pModel->EnumNodeFirst();pModelNode;pModelNode=m_pModel->EnumNodeNext())
			{
				if(pModelNode->xPosition.x<-1||pModelNode->xPosition.x>1)
					continue;
				if(pModelNode->xPosition.z-keynodes.zb.pMidBeamTopNode->xPosition.z>5000)
					continue;	//Ĭ�ϾƱ����ᵣ�߶Ȳ�����5m wjh-2018.6.29
				if(!initialized)
				{
					initialized=true;
					maxZ =pModelNode->xPosition.z;
					maxZY=pModelNode->xPosition.y;
					keynodes.zb.pMidBeamTopNode=pModelNode;
					keynodes.zb.pMidBeamBtmNode=pModelNode;
				}
				if(pModelNode->xPosition.z>maxZ+0.1)
				{
					maxZ=pModelNode->xPosition.z;
					maxZY=pModelNode->xPosition.y;
					keynodes.zb.pMidBeamBtmNode=pModelNode;
				}
				else if(pModelNode->xPosition.z>maxZ-0.1)
				{	//����ͬһƽ����
					if(pModelNode->xPosition.y>maxZY)
					{
						maxZY=pModelNode->xPosition.y;
						keynodes.zb.pMidBeamBtmNode=pModelNode;
					}
				}
			}
		}
		//2.�����������ڲ���ᵣ���ӵ�
		IModelRod *pModelRod,*pInnerArmRodR=NULL,*pInnerArmRodL=NULL;
		double zBeamPlane=0,yMaxNearPointR=0,yMaxNearPointL=0;
		double xMinNearPointR=1e5,xMinNearPointL=-1e5;
		if(keynodes.zb.pArmBeamJointNodeOutRight!=NULL)
			zBeamPlane=keynodes.zb.pArmBeamJointNodeOutRight->xPosition.z;
		for(pModelRod=m_pModel->EnumRodFirst();pModelRod;pModelRod=m_pModel->EnumRodNext())
		{
			char ciSide=0;
			if(pModelRod->xStartPoint.x>0&&pModelRod->xEndPoint.x>0)
				ciSide='R';
			else if(pModelRod->xStartPoint.x<0&&pModelRod->xEndPoint.x<0)
				ciSide='L';
			else
				continue;
			double minz=min(pModelRod->xStartPoint.z,pModelRod->xEndPoint.z);
			double maxz=max(pModelRod->xStartPoint.z,pModelRod->xEndPoint.z);
			if(fabs(minz-zBeamPlane)>100||maxz-zBeamPlane<1000)
				continue;
			double gradientX2Z=(pModelRod->xEndPoint.x-pModelRod->xStartPoint.x)/(pModelRod->xEndPoint.z-pModelRod->xStartPoint.z);
			double gradientY2Z=(pModelRod->xEndPoint.y-pModelRod->xStartPoint.y)/(pModelRod->xEndPoint.z-pModelRod->xStartPoint.z);
			double xNearPoint=pModelRod->xStartPoint.x+gradientX2Z*(zBeamPlane+100-pModelRod->xStartPoint.z);
			double yNearPoint=pModelRod->xStartPoint.y+gradientY2Z*(zBeamPlane+100-pModelRod->xStartPoint.z);
			if(ciSide=='R')
			{
				if(xNearPoint<xMinNearPointR-1||fabs(xNearPoint-xMinNearPointR)<1&&yNearPoint>yMaxNearPointR)
				{
					xMinNearPointR=xNearPoint;
					yMaxNearPointR=yNearPoint;
					pInnerArmRodR=pModelRod;
				}
			}
			else if(ciSide=='L')
			{
				if(xNearPoint>xMinNearPointL+1||fabs(xNearPoint-xMinNearPointL)<1&&yNearPoint>yMaxNearPointL)
				{
					xMinNearPointL=xNearPoint;
					yMaxNearPointL=yNearPoint;
					pInnerArmRodL=pModelRod;
				}
			}
		}
		if(pInnerArmRodR!=NULL)
		{
			if(pInnerArmRodR->xStartPoint.z<pInnerArmRodR->xEndPoint.z)
				keynodes.zb.pArmBeamJointNodeInnerRight=pInnerArmRodR->pStart;
			else
				keynodes.zb.pArmBeamJointNodeInnerRight=pInnerArmRodR->pEnd;
		}
		if(pInnerArmRodL!=NULL)
		{
			if(pInnerArmRodL->xStartPoint.z<pInnerArmRodL->xEndPoint.z)
				keynodes.zb.pArmBeamJointNodeInnerLeft=pInnerArmRodL->pStart;
			else
				keynodes.zb.pArmBeamJointNodeInnerLeft=pInnerArmRodL->pEnd;
		}
		//2.�����������۽����
		if(keynodes.zb.pArmPostJointNodeRight==NULL&&pInnerArmRodR!=NULL)
		{
			CPtInLineCheck check(pInnerArmRodR->xStartPoint,pInnerArmRodR->xEndPoint);
			IModelNode* pCurrNode=pInnerArmRodR->pEnd;
			if(pInnerArmRodR->xEndPoint.z<pInnerArmRodR->xStartPoint.z)
			{
				check.SetLine(pInnerArmRodR->xEndPoint,pInnerArmRodR->xStartPoint);
				pCurrNode=pInnerArmRodR->pStart;
			}
			IModelRod* pNextRod=NULL,*pLastColineaRod=NULL;
			do{
				pNextRod=NULL;
				for(pModelRod=m_pModel->EnumRodFirst();pModelRod;pModelRod=m_pModel->EnumRodNext())
				{
					bool online=false;
					double scale;
					if(pModelRod->pStart==pCurrNode)
						scale=check.CheckPointEx(pModelRod->xEndPoint,&online);
					else if(pModelRod->pEnd==pCurrNode)
						scale=check.CheckPointEx(pModelRod->xStartPoint,&online);
					else
						continue;
					if(online&&scale>1.01)
					{
						pLastColineaRod=pNextRod=pModelRod;
						if(pCurrNode==pModelRod->pStart)
						{
							pCurrNode=pModelRod->pEnd;
							check.SetLine(pModelRod->xStartPoint,pModelRod->xEndPoint);
						}
						else
						{
							pCurrNode=pModelRod->pStart;
							check.SetLine(pModelRod->xEndPoint,pModelRod->xStartPoint);
						}
						break;
					}
				}
			}while(pNextRod!=NULL);
			keynodes.zb.pArmPostJointNodeRight=pCurrNode;
		}
		if(keynodes.zb.pArmPostJointNodeLeft==NULL&&pInnerArmRodL!=NULL)
		{
			CPtInLineCheck check(pInnerArmRodL->xStartPoint,pInnerArmRodL->xEndPoint);
			IModelNode* pCurrNode=pInnerArmRodL->pEnd;
			if(pInnerArmRodL->xEndPoint.z<pInnerArmRodL->xStartPoint.z)
			{
				check.SetLine(pInnerArmRodL->xEndPoint,pInnerArmRodL->xStartPoint);
				pCurrNode=pInnerArmRodL->pStart;
			}
			IModelRod* pNextRod=NULL,*pLastColineaRod=NULL;
			do{
				pNextRod=NULL;
				for(pModelRod=m_pModel->EnumRodFirst();pModelRod;pModelRod=m_pModel->EnumRodNext())
				{
					bool online=false;
					double scale;
					if(pModelRod->pStart==pCurrNode)
						scale=check.CheckPointEx(pModelRod->xEndPoint,&online);
					else if(pModelRod->pEnd==pCurrNode)
						scale=check.CheckPointEx(pModelRod->xStartPoint,&online);
					else
						continue;
					if(online&&scale>1.01)
					{
						pLastColineaRod=pNextRod=pModelRod;
						if(pCurrNode==pModelRod->pStart)
						{
							pCurrNode=pModelRod->pEnd;
							check.SetLine(pModelRod->xStartPoint,pModelRod->xEndPoint);
						}
						else
						{
							pCurrNode=pModelRod->pStart;
							check.SetLine(pModelRod->xEndPoint,pModelRod->xStartPoint);
						}
						break;
					}
				}
			}while(pNextRod!=NULL);
			keynodes.zb.pArmPostJointNodeLeft=pCurrNode;
		}
		//����Ʊ����еĹؼ��ڵ�
		for(pTmplNode=pLiveArm->hashParaNodes.GetFirst();pTmplNode!=NULL;pTmplNode=pLiveArm->hashParaNodes.GetNext())
		{
			pLiveNode=pLiveArm->hashMatchNodes.Add(pTmplNode->widNode);
			if(pTmplNode->ciPosCalType==20)
			{	//�Ʊ��������۽����(��Ϊ�Ҳ࣬��Ϊ���)
				pLiveNode->pIntelliNode=keynodes.zb.pArmPostJointNodeRight;
				pLiveNode->hRelaModelNode=keynodes.zb.pArmPostJointNodeRight->handle;
			}
			else if(pTmplNode->ciPosCalType==-20)
			{	//�Ʊ��������۽����(��Ϊ�Ҳ࣬��Ϊ���)
				pLiveNode->pIntelliNode=keynodes.zb.pArmPostJointNodeLeft;
				pLiveNode->hRelaModelNode=keynodes.zb.pArmPostJointNodeLeft->handle;
			}
			else if(pTmplNode->ciPosCalType==21)
			{	//�Ʊ��кᵣ�����߽������½ڵ�(��Ϊ�ϲ࣬��Ϊ�²�)
				pLiveNode->pIntelliNode=keynodes.zb.pMidBeamTopNode;
				pLiveNode->hRelaModelNode=keynodes.zb.pMidBeamTopNode->handle;
			}
			else if(pTmplNode->ciPosCalType==-21)
			{	//�Ʊ��кᵣ�����߽������½ڵ㣨��Ϊ�ϲ࣬��Ϊ�²ࣩ
				pLiveNode->pIntelliNode=keynodes.zb.pMidBeamBtmNode;
				pLiveNode->hRelaModelNode=keynodes.zb.pMidBeamBtmNode->handle;
			}
			else if(pTmplNode->ciPosCalType==22)
			{	//�Ʊ���������ᵣ��ཻ���(��Ϊ�Ҳ࣬��Ϊ���)
				pLiveNode->pIntelliNode=keynodes.zb.pArmBeamJointNodeOutRight;
				pLiveNode->hRelaModelNode=keynodes.zb.pArmBeamJointNodeOutRight->handle;
			}
			else if(pTmplNode->ciPosCalType==-22)
			{	//�Ʊ���������ᵣ��ཻ���(��Ϊ�Ҳ࣬��Ϊ���)
				pLiveNode->pIntelliNode=keynodes.zb.pArmBeamJointNodeOutLeft;
				pLiveNode->hRelaModelNode=keynodes.zb.pArmBeamJointNodeOutLeft->handle;
			}
			else if(pTmplNode->ciPosCalType==23)
			{	//�Ʊ���������ᵣ�ڲཻ���(��Ϊ�Ҳ࣬��Ϊ���)
				pLiveNode->pIntelliNode=keynodes.zb.pArmBeamJointNodeInnerRight;
				pLiveNode->hRelaModelNode=keynodes.zb.pArmBeamJointNodeInnerRight->handle;
			}
			else if(pTmplNode->ciPosCalType==-23)
			{	//�Ʊ���������ᵣ�ڲཻ���(��Ϊ�Ҳ࣬��Ϊ���)
				pLiveNode->pIntelliNode=keynodes.zb.pArmBeamJointNodeInnerLeft;
				pLiveNode->hRelaModelNode=keynodes.zb.pArmBeamJointNodeInnerLeft->handle;
			}
			else if(pTmplNode->ciPosCalType!=5)
				continue;
		}
	}
	else if(pLiveArm->ciClassicType=='M')
	{
		//èͷ�������۽����(��Ϊ�Ҳ࣬��Ϊ���)
		//1.0����èͷ�кᵣ�����߽������½ڵ㣨��Ϊ�ϲ࣬��Ϊ�²ࣩ
		IModelNode* pModelNode;
		double minZ,maxY;
		double maxZ,maxZY;	//�жϺᵣ�����߽������ǰ��(Y+)�ڵ�����Z���꼰���Z���괦�����Y����ֵ
		bool initialized=false;
		for(pModelNode=this->m_pModel->EnumNodeFirst();pModelNode;pModelNode=this->m_pModel->EnumNodeNext())
		{
			if(pModelNode->xPosition.x<-1||pModelNode->xPosition.x>1)
				continue;
			if(keynodes.zm.pArmBeamJointNodeOutRight&&pModelNode->xPosition.z-keynodes.zm.pArmBeamJointNodeOutRight->xPosition.z>100)
				continue;	//�ᵣ�����߽���ؼ����Z���겻���ܵ�����������ᵣ���ӽڵ� wjh-2018.6.29
			if(!initialized)
			{
				initialized=true;
				minZ=maxZ =pModelNode->xPosition.z;
				maxY=maxZY=pModelNode->xPosition.y;
				keynodes.zm.pMidBeamTopNode=pModelNode;
				if(keynodes.zm.pArmBeamJointNodeOutRight)
					keynodes.zm.pMidBeamBtmNode=pModelNode;
			}
			if(pModelNode->xPosition.z<minZ-0.1)
			{
				minZ=pModelNode->xPosition.z;
				maxY=pModelNode->xPosition.y;
				keynodes.zm.pMidBeamTopNode=pModelNode;
			}
			else if(pModelNode->xPosition.z<minZ+0.1)
			{
				if(pModelNode->xPosition.y>maxY)
				{
					maxY=pModelNode->xPosition.y;
					keynodes.zm.pMidBeamTopNode=pModelNode;
				}
			}

			if(keynodes.zm.pArmBeamJointNodeOutRight!=NULL)
			{
				if(pModelNode->xPosition.z>maxZ+0.1)
				{
					maxZ=pModelNode->xPosition.z;
					maxZY=pModelNode->xPosition.y;
					keynodes.zm.pMidBeamBtmNode=pModelNode;
				}
				else if(pModelNode->xPosition.z>maxZ-0.1)
				{	//����ͬһƽ����
					if(pModelNode->xPosition.y>maxZY)
					{
						maxZY=pModelNode->xPosition.y;
						keynodes.zm.pMidBeamBtmNode=pModelNode;
					}
				}
			}
		}
		double maxBtmZ,maxBtmZY;
		initialized=false;
		double MAX_HEAD_NODE_Z=pLiveArm->hashMatchNodes.GetValue(1)->pIntelliNode->xPosition.z;
		for(pModelNode=this->m_pModel->EnumNodeFirst();pModelNode;pModelNode=this->m_pModel->EnumNodeNext())
		{
			if(pModelNode->xPosition.x<-1||pModelNode->xPosition.x>1)
				continue;
			if(keynodes.zm.pArmBeamJointNodeOutRight&&pModelNode->xPosition.z-MAX_HEAD_NODE_Z>-10)
				continue;
			if(!initialized)
			{
				initialized=true;
				maxBtmZ =pModelNode->xPosition.z;
				maxBtmZY=pModelNode->xPosition.y;
				keynodes.zm.pBtmArmMidNode=pModelNode;
			}
			else if(pModelNode->xPosition.z>maxBtmZ+0.1)
			{
				maxBtmZ=pModelNode->xPosition.z;
				maxBtmZY=pModelNode->xPosition.y;
				keynodes.zm.pBtmArmMidNode=pModelNode;
			}
			else if(pModelNode->xPosition.z>maxBtmZ-0.1)
			{
				if(pModelNode->xPosition.y>maxBtmZY)
				{
					maxBtmZY=pModelNode->xPosition.y;
					keynodes.zm.pBtmArmMidNode=pModelNode;
				}
			}
		}
		if(keynodes.zm.pMidBeamTopNode!=NULL&&keynodes.zm.pMidBeamBtmNode==NULL)
		{	//1.1���ԭʼ��������δ�ҵ���������ᵣ��ཻ�ӵ�����£����ݺᵣ���߶ȼ��ᵣ�����߽������ؼ���
			initialized=false;
			for(pModelNode=m_pModel->EnumNodeFirst();pModelNode;pModelNode=m_pModel->EnumNodeNext())
			{
				if(pModelNode->xPosition.x<-1||pModelNode->xPosition.x>1)
					continue;
				if(pModelNode->xPosition.z-keynodes.zm.pMidBeamTopNode->xPosition.z>5000)
					continue;	//Ĭ�ϾƱ����ᵣ�߶Ȳ�����5m wjh-2018.6.29
				if(!initialized)
				{
					initialized=true;
					maxZ =pModelNode->xPosition.z;
					maxZY=pModelNode->xPosition.y;
					keynodes.zm.pMidBeamTopNode=pModelNode;
					keynodes.zm.pMidBeamBtmNode=pModelNode;
				}
				if(pModelNode->xPosition.z>maxZ+0.1)
				{
					maxZ=pModelNode->xPosition.z;
					maxZY=pModelNode->xPosition.y;
					keynodes.zm.pMidBeamBtmNode=pModelNode;
				}
				else if(pModelNode->xPosition.z>maxZ-0.1)
				{	//����ͬһƽ����
					if(pModelNode->xPosition.y>maxZY)
					{
						maxZY=pModelNode->xPosition.y;
						keynodes.zm.pMidBeamBtmNode=pModelNode;
					}
				}
			}
		}
		//2.�����������ڲ���ᵣ���ӵ�
		IModelRod *pModelRod,*pInnerArmRodR=NULL,*pInnerArmRodL=NULL;
		double zBeamPlane=0,yMaxNearPointR=0,yMaxNearPointL=0;
		double xMinNearPointR=1e5,xMinNearPointL=-1e5;
		if(keynodes.zm.pArmBeamJointNodeInnerRight==NULL&&keynodes.zm.pArmBeamJointNodeOutRight!=NULL)
		{
			zBeamPlane=keynodes.zm.pArmBeamJointNodeOutRight->xPosition.z;
			for(pModelRod=m_pModel->EnumRodFirst();pModelRod;pModelRod=m_pModel->EnumRodNext())
			{
				char ciSide=0;
				if(pModelRod->xStartPoint.x>0&&pModelRod->xEndPoint.x>0)
					ciSide='R';
				else if(pModelRod->xStartPoint.x<0&&pModelRod->xEndPoint.x<0)
					ciSide='L';
				else
					continue;
				double minz=min(pModelRod->xStartPoint.z,pModelRod->xEndPoint.z);
				double maxz=max(pModelRod->xStartPoint.z,pModelRod->xEndPoint.z);
				if(fabs(minz-zBeamPlane)>100||maxz-zBeamPlane<1000)
					continue;
				double gradientX2Z=(pModelRod->xEndPoint.x-pModelRod->xStartPoint.x)/(pModelRod->xEndPoint.z-pModelRod->xStartPoint.z);
				double gradientY2Z=(pModelRod->xEndPoint.y-pModelRod->xStartPoint.y)/(pModelRod->xEndPoint.z-pModelRod->xStartPoint.z);
				double xNearPoint=pModelRod->xStartPoint.x+gradientX2Z*(zBeamPlane+100-pModelRod->xStartPoint.z);
				double yNearPoint=pModelRod->xStartPoint.y+gradientY2Z*(zBeamPlane+100-pModelRod->xStartPoint.z);
				if(ciSide=='R')
				{
					if(xNearPoint<xMinNearPointR-1||fabs(xNearPoint-xMinNearPointR)<1&&yNearPoint>yMaxNearPointR)
					{
						xMinNearPointR=xNearPoint;
						yMaxNearPointR=yNearPoint;
						pInnerArmRodR=pModelRod;
					}
				}
				else if(ciSide=='L')
				{
					if(xNearPoint>xMinNearPointL+1||fabs(xNearPoint-xMinNearPointL)<1&&yNearPoint>yMaxNearPointL)
					{
						xMinNearPointL=xNearPoint;
						yMaxNearPointL=yNearPoint;
						pInnerArmRodL=pModelRod;
					}
				}
			}
			if(pInnerArmRodR!=NULL)
			{
				if(pInnerArmRodR->xStartPoint.z<pInnerArmRodR->xEndPoint.z)
					keynodes.zm.pArmBeamJointNodeInnerRight=pInnerArmRodR->pStart;
				else
					keynodes.zm.pArmBeamJointNodeInnerRight=pInnerArmRodR->pEnd;
			}
			if(pInnerArmRodL!=NULL)
			{
				if(pInnerArmRodL->xStartPoint.z<pInnerArmRodL->xEndPoint.z)
					keynodes.zm.pArmBeamJointNodeInnerLeft=pInnerArmRodL->pStart;
				else
					keynodes.zm.pArmBeamJointNodeInnerLeft=pInnerArmRodL->pEnd;
			}
		}
		//2.�����Ϻ������������Ӵ��ڹյ�
		if(keynodes.zm.pArmBeamJointNodeInnerRight==NULL)
		{
			IModelRod *pDownInnerArmRodR=NULL;
			IModelRod *pDownInnerArmRodL=NULL;
			zBeamPlane=0,yMaxNearPointR=0,yMaxNearPointL=0;
			xMinNearPointR=1e5,xMinNearPointL=-1e5;
			if(keynodes.zm.pArmBeardJointNodeOutRight!=NULL)
				zBeamPlane=keynodes.zm.pArmBeardJointNodeOutRight->xPosition.z;
			for(pModelRod=m_pModel->EnumRodFirst();pModelRod;pModelRod=m_pModel->EnumRodNext())
			{
				char ciSide=0;
				if(fabs(pModelRod->xStartPoint.x)>fabs(pModelRod->xEndPoint.x))
				{
					if(pModelRod->xStartPoint.x>0)
						ciSide='R';
					else if(pModelRod->xStartPoint.x<0)
						ciSide='L';
					else
						continue;
				}
				else
				{
					if(pModelRod->xEndPoint.x>0)
						ciSide='R';
					else if(pModelRod->xEndPoint.x<0)
						ciSide='L';
					else
						continue;
				}
				double minz=min(pModelRod->xStartPoint.z,pModelRod->xEndPoint.z);
				double maxz=max(pModelRod->xStartPoint.z,pModelRod->xEndPoint.z);
				if(fabs(minz-zBeamPlane)>100||maxz-zBeamPlane<1000)
					continue;
				double gradientX2Z=(pModelRod->xEndPoint.x-pModelRod->xStartPoint.x)/(pModelRod->xEndPoint.z-pModelRod->xStartPoint.z);
				double gradientY2Z=(pModelRod->xEndPoint.y-pModelRod->xStartPoint.y)/(pModelRod->xEndPoint.z-pModelRod->xStartPoint.z);
				double xNearPoint=pModelRod->xStartPoint.x+gradientX2Z*(zBeamPlane+100-pModelRod->xStartPoint.z);
				double yNearPoint=pModelRod->xStartPoint.y+gradientY2Z*(zBeamPlane+100-pModelRod->xStartPoint.z);
				if(ciSide=='R')
				{
					if(xNearPoint<xMinNearPointR-1||fabs(xNearPoint-xMinNearPointR)<1&&yNearPoint>yMaxNearPointR)
					{
						xMinNearPointR=xNearPoint;
						yMaxNearPointR=yNearPoint;
						pDownInnerArmRodR=pModelRod;
					}
				}
				else if(ciSide=='L')
				{
					if(xNearPoint>xMinNearPointL+1||fabs(xNearPoint-xMinNearPointL)<1&&yNearPoint>yMaxNearPointL)
					{
						xMinNearPointL=xNearPoint;
						yMaxNearPointL=yNearPoint;
						pDownInnerArmRodL=pModelRod;
					}
				}
			}
			if(pDownInnerArmRodR!=NULL)
			{
				if(pDownInnerArmRodR->xStartPoint.z<pDownInnerArmRodR->xEndPoint.z)
					keynodes.zm.pArmBeardJointNodeInnerRight=pDownInnerArmRodR->pStart;
				else
					keynodes.zm.pArmBeardJointNodeInnerRight=pDownInnerArmRodR->pEnd;
			}
			if(pDownInnerArmRodL!=NULL)
			{
				if(pDownInnerArmRodL->xStartPoint.z<pDownInnerArmRodL->xEndPoint.z)
					keynodes.zm.pArmBeardJointNodeInnerLeft=pDownInnerArmRodL->pStart;
				else
					keynodes.zm.pArmBeardJointNodeInnerLeft=pDownInnerArmRodL->pEnd;
			}
		}
		//3.�����������۽����
		if(keynodes.zm.pArmPostJointNodeRight==NULL&&pInnerArmRodR!=NULL)
		{
			CPtInLineCheck check(pInnerArmRodR->xStartPoint,pInnerArmRodR->xEndPoint);
			IModelNode* pCurrNode=pInnerArmRodR->pEnd;
			if(pInnerArmRodR->xEndPoint.z<pInnerArmRodR->xStartPoint.z)
			{
				check.SetLine(pInnerArmRodR->xEndPoint,pInnerArmRodR->xStartPoint);
				pCurrNode=pInnerArmRodR->pStart;
			}
			IModelRod* pNextRod=NULL,*pLastColineaRod=NULL;
			do{
				pNextRod=NULL;
				for(pModelRod=m_pModel->EnumRodFirst();pModelRod;pModelRod=m_pModel->EnumRodNext())
				{
					bool online=false;
					double scale;
					if(pModelRod->pStart==pCurrNode)
						scale=check.CheckPointEx(pModelRod->xEndPoint,&online);
					else if(pModelRod->pEnd==pCurrNode)
						scale=check.CheckPointEx(pModelRod->xStartPoint,&online);
					else
						continue;
					if(online&&scale>1.01)
					{
						pLastColineaRod=pNextRod=pModelRod;
						if(pCurrNode==pModelRod->pStart)
						{
							pCurrNode=pModelRod->pEnd;
							check.SetLine(pModelRod->xStartPoint,pModelRod->xEndPoint);
						}
						else
						{
							pCurrNode=pModelRod->pStart;
							check.SetLine(pModelRod->xEndPoint,pModelRod->xStartPoint);
						}
						break;
					}
				}
			}while(pNextRod!=NULL);
			keynodes.zm.pArmPostJointNodeRight=pCurrNode;
		}
		if(keynodes.zm.pArmPostJointNodeLeft==NULL&&pInnerArmRodL!=NULL)
		{
			CPtInLineCheck check(pInnerArmRodL->xStartPoint,pInnerArmRodL->xEndPoint);
			IModelNode* pCurrNode=pInnerArmRodL->pEnd;
			if(pInnerArmRodL->xEndPoint.z<pInnerArmRodL->xStartPoint.z)
			{
				check.SetLine(pInnerArmRodL->xEndPoint,pInnerArmRodL->xStartPoint);
				pCurrNode=pInnerArmRodL->pStart;
			}
			IModelRod* pNextRod=NULL,*pLastColineaRod=NULL;
			do{
				pNextRod=NULL;
				for(pModelRod=m_pModel->EnumRodFirst();pModelRod;pModelRod=m_pModel->EnumRodNext())
				{
					bool online=false;
					double scale;
					if(pModelRod->pStart==pCurrNode)
						scale=check.CheckPointEx(pModelRod->xEndPoint,&online);
					else if(pModelRod->pEnd==pCurrNode)
						scale=check.CheckPointEx(pModelRod->xStartPoint,&online);
					else
						continue;
					if(online&&scale>1.01)
					{
						pLastColineaRod=pNextRod=pModelRod;
						if(pCurrNode==pModelRod->pStart)
						{
							pCurrNode=pModelRod->pEnd;
							check.SetLine(pModelRod->xStartPoint,pModelRod->xEndPoint);
						}
						else
						{
							pCurrNode=pModelRod->pStart;
							check.SetLine(pModelRod->xEndPoint,pModelRod->xStartPoint);
						}
						break;
					}
				}
			}while(pNextRod!=NULL);
			keynodes.zm.pArmPostJointNodeLeft=pCurrNode;
		}
		//4.���Һᵣ��������Ķ˽ڵ�
		if((keynodes.zm.pArmBeamUpperTipNodeRight==NULL||keynodes.zm.pArmBeamUpperTipNodeLeft==NULL)&&
			keynodes.zm.pArmBeamUpperInnerNodeLeft!=NULL&&keynodes.zm.pArmBeamUpperInnerNodeRight!=NULL)
		{
			CPtInLineCheck check(keynodes.zm.pArmBeamUpperInnerNodeLeft->xPosition,keynodes.zm.pArmBeamUpperInnerNodeRight->xPosition);
			IModelNode* pCurrNodeRight=keynodes.zm.pArmBeamUpperInnerNodeRight;
			IModelNode* pCurrNodeLeft =keynodes.zm.pArmBeamUpperInnerNodeLeft;
			double right_scale=1,left_scale=0;
			for(pModelNode=m_pModel->EnumNodeFirst();pModelNode;pModelNode=m_pModel->EnumNodeNext())
			{
				if(pModelNode->xPosition.x>=pCurrNodeLeft->xPosition.x&&pModelNode->xPosition.x<=pCurrNodeRight->xPosition.x)
					continue;
				bool online=false;
				double scale=check.CheckPointEx(pModelNode->xPosition,&online);
				if(!online)
					continue;
				if(scale>right_scale+0.01)
				{
					pCurrNodeRight=pModelNode;
					right_scale=scale;
				}
				else if(scale<left_scale-0.01)
				{
					pCurrNodeLeft=pModelNode;
					left_scale=scale;
				}
			}
			keynodes.zm.pArmBeamUpperTipNodeRight=pCurrNodeRight;
			keynodes.zm.pArmBeamUpperTipNodeLeft =pCurrNodeLeft;
		}
		//����èͷ���еĹؼ��ڵ�
		for(pTmplNode=pLiveArm->hashParaNodes.GetFirst();pTmplNode!=NULL;pTmplNode=pLiveArm->hashParaNodes.GetNext())
		{
			pLiveNode=pLiveArm->hashMatchNodes.Add(pTmplNode->widNode);
			if(pTmplNode->ciPosCalType==20)
			{	//�Ʊ��������۽����(��Ϊ�Ҳ࣬��Ϊ���)
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmPostJointNodeRight)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmPostJointNodeRight->handle;
			}
			else if(pTmplNode->ciPosCalType==-20)
			{	//�Ʊ��������۽����(��Ϊ�Ҳ࣬��Ϊ���)
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmPostJointNodeLeft)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmPostJointNodeLeft->handle;
			}
			else if(pTmplNode->ciPosCalType==21)
			{	//�Ʊ��кᵣ�����߽������½ڵ�(��Ϊ�ϲ࣬��Ϊ�²�)
				if((pLiveNode->pIntelliNode=keynodes.zm.pMidBeamTopNode)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pMidBeamTopNode->handle;
			}
			else if(pTmplNode->ciPosCalType==-21)
			{	//�Ʊ��кᵣ�����߽������½ڵ㣨��Ϊ�ϲ࣬��Ϊ�²ࣩ
				if((pLiveNode->pIntelliNode=keynodes.zm.pMidBeamBtmNode)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pMidBeamBtmNode->handle;
			}
			else if(pTmplNode->ciPosCalType==22)
			{	//�Ʊ���������ᵣ��ཻ���(��Ϊ�Ҳ࣬��Ϊ���)
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmBeamJointNodeOutRight)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmBeamJointNodeOutRight->handle;
			}
			else if(pTmplNode->ciPosCalType==-22)
			{	//�Ʊ���������ᵣ��ཻ���(��Ϊ�Ҳ࣬��Ϊ���)
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmBeamJointNodeOutLeft)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmBeamJointNodeOutLeft->handle;
			}
			else if(pTmplNode->ciPosCalType==23)
			{	//èͷ��������ᵣ�ڲཻ���(��Ϊ�Ҳ࣬��Ϊ���)
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmBeamJointNodeInnerRight)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmBeamJointNodeInnerRight->handle;
			}
			else if(pTmplNode->ciPosCalType==-23)
			{	//èͷ��������ᵣ�ڲཻ���(��Ϊ�Ҳ࣬��Ϊ���)
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmBeamJointNodeInnerLeft)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmBeamJointNodeInnerLeft->handle;
			}
			else if(pTmplNode->ciPosCalType==24)
			{	//èͷ�Ϻᵣ����˽ڵ�(��Ϊ�Ҳ࣬��Ϊ���)
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmBeamUpperTipNodeRight)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmBeamUpperTipNodeRight->handle;
			}
			else if(pTmplNode->ciPosCalType==-24)
			{	//èͷ�Ϻᵣ����˽ڵ�(��Ϊ�Ҳ࣬��Ϊ���)
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmBeamUpperTipNodeLeft)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmBeamUpperTipNodeLeft->handle;
			}
			else if(pTmplNode->ciPosCalType==26)
			{	//èͷ�к��봦�������ڹսǴ��ڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmBeardJointNodeInnerRight)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmBeardJointNodeInnerRight->handle;
			}
			else if(pTmplNode->ciPosCalType==-26)
			{	//èͷ�к��봦�������ڹսǴ��ڵ㣨��Ϊ�Ҳ࣬��Ϊ��ࣩ
				if((pLiveNode->pIntelliNode=keynodes.zm.pArmBeardJointNodeInnerLeft)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pArmBeardJointNodeInnerLeft->handle;
			}
			else if(pTmplNode->ciPosCalType==28)
			{	//èͷ�����������������ϵĽ����
				if((pLiveNode->pIntelliNode=keynodes.zm.pBtmArmMidNode)!=NULL)
					pLiveNode->hRelaModelNode=keynodes.zm.pBtmArmMidNode->handle;
			}
			else if(pTmplNode->ciPosCalType!=5)
				continue;
		}
	}
	else if(pLiveArm->ciClassicType==1&&keynodes.zl.pMidBeamNode==NULL)	//"ZL"
	{	//800kvֱ�����������Ͻ����
		double minZ,maxY;
		bool initialized=false;
		IModelNode* pModelNode;
		for(pModelNode=this->m_pModel->EnumNodeFirst();pModelNode;pModelNode=this->m_pModel->EnumNodeNext())
		{
			if(pModelNode->xPosition.x<-1||pModelNode->xPosition.x>1)
				continue;
			if(!initialized)
			{
				initialized=true;
				minZ=pModelNode->xPosition.z;
				maxY=pModelNode->xPosition.y;
				keynodes.zl.pMidBeamNode=pModelNode;
			}
			else if(pModelNode->xPosition.z<minZ-0.1)
			{
				minZ=pModelNode->xPosition.z;
				maxY=pModelNode->xPosition.y;
				keynodes.zl.pMidBeamNode=pModelNode;
			}
			else if(pModelNode->xPosition.z<minZ+0.1)
			{
				if(pModelNode->xPosition.y>maxY)
				{
					maxY=pModelNode->xPosition.y;
					keynodes.zl.pMidBeamNode=pModelNode;
				}
			}
		}
		//����ֱ�����еĹؼ��ڵ�
		for(pTmplNode=pLiveArm->hashParaNodes.GetFirst();pTmplNode!=NULL;pTmplNode=pLiveArm->hashParaNodes.GetNext())
		{
			pLiveNode=pLiveArm->hashMatchNodes.Add(pTmplNode->widNode);
			if(pTmplNode->ciPosCalType==28)//800kvֱ�����������Ͻ����
			{
				pLiveNode->pIntelliNode=keynodes.zl.pMidBeamNode;
				break;
			}
		}
	}
	//��ȡ�����߽�˼�
	for(STDTEMPL::PROFILE_ROD* pEdgeRod=pLiveArm->hashParaRods.GetFirst();pEdgeRod;pEdgeRod=pLiveArm->hashParaRods.GetNext())
	{
		short siStartNode=pEdgeRod->widStartNode,siEndNode=pEdgeRod->widEndNode;
		pMatchStart=pLiveArm->hashMatchNodes.GetValue(abs(siStartNode));
		if(siStartNode<0&&pMatchStart!=NULL)
		{	//X��Գƽڵ�
			pMatchStart=pLiveArm->hashMatchNodes.SetValue(pEdgeRod->widStartNode,*pMatchStart);
			GEPOINT mirpoint=pMatchStart->pIntelliNode->xPosition;
			mirpoint.y*=-1;
			for(IModelNode* pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
			{
				if(pNode->xPosition.IsEqual(mirpoint))
				{
					pMatchStart->pIntelliNode=pNode;
					pMatchStart->hRelaModelNode=pNode->handle;
					break;
				}
			}
		}
		pMatchEnd=pLiveArm->hashMatchNodes.GetValue(abs(siEndNode));
		if(siEndNode<0&&pMatchEnd!=NULL)
		{
			pMatchEnd=pLiveArm->hashMatchNodes.SetValue(pEdgeRod->widEndNode,*pMatchEnd);
			GEPOINT mirpoint=pMatchEnd->pIntelliNode->xPosition;
			mirpoint.y*=-1;
			for(IModelNode* pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
			{
				if(pNode->xPosition.IsEqual(mirpoint))
				{
					pMatchEnd->pIntelliNode=pNode;
					pMatchEnd->hRelaModelNode=pNode->handle;
					break;
				}
			}
		}
#ifdef _DEBUG
		if(pMatchStart==NULL||pMatchEnd==NULL)
		{
			pLogErr->Log("%d{����δʶ��˼�}",pEdgeRod->widRod);
			continue;
		}
#endif
		CXhSimpleList<STDTEMPL::EDGEROD> listEdgeRods;
		RetrieveLiveEdgeRods(pMatchStart,pMatchEnd,ocs,&listEdgeRods);
		STDTEMPL::MATCHROD* pMatchRod=pLiveArm->hashMatchRods.Add(pEdgeRod->widRod);
		if(pMatchRod!=NULL)
		{
			pMatchRod->relaRods.Clear();
			for(STDTEMPL::EDGEROD* pEdgeRod=listEdgeRods.EnumObjectFirst();pEdgeRod;pEdgeRod=listEdgeRods.EnumObjectNext())
				pMatchRod->relaRods.Append(*pEdgeRod);
		}
	}
	return true;
}
bool CIntelliCore::RecogStdArmTemplPortal(STDTEMPL* pLiveMatchArmTempl,ARM_BAND *pArmBand)
{
	float TOLERANCE=0.1f;
	IModelRod* pRod=NULL;
	CXhPtrSet<IModelRod> rodset;
	GELINE postline(GEPOINT(pArmBand->boundaryLineTopImgXY[0],pArmBand->boundaryLineTopImgXY[1],0),
					GEPOINT(pArmBand->boundaryLineBtmImgXY[0],pArmBand->boundaryLineBtmImgXY[1],0));
	//1.��ȡ��ǰ�ᵣ������ڵ���ɸ˼�����
	for(pRod=m_pModel->EnumRodFirst();pRod;pRod=m_pModel->EnumRodNext())
	{
		if(pRod->pStart==NULL||pRod->pEnd==NULL)
			continue;
		GELINE briefline(pRod->pStart->xPosition,pRod->pEnd->xPosition);
		if(pArmBand->ciOnRightSide=='U')
		{	//�˼�������ͷ���б�������Zֵ����������ƽ���λ��������ƽ��ͬʱX����λ������֮�� wjh-2018.7.1
			if(  briefline.start.z<pArmBand->scopeMaxZ+TOLERANCE&&briefline.end.z<pArmBand->scopeMaxZ+TOLERANCE&&	//������ˮƽ�����¸˼�
				(briefline.start.z<=pArmBand->scopeMaxZ-TOLERANCE||briefline.end.z<=pArmBand->scopeMaxZ-TOLERANCE||
				fabs(briefline.start.x)>pArmBand->boundaryLineTopImgXY[0]||fabs(briefline.end.x)>pArmBand->boundaryLineTopImgXY[0]))	//ˮƽ���
				rodset.append(pRod);//���ٴ���һ����ȷλ������ֽ����Ҳ�(ǰ��ͼ��,���ݷֽ���ʼ->�տ������)
			continue;
		}
		else if(pArmBand->ciOnRightSide=='L'||pArmBand->ciOnRightSide=='R')
		{
			if(briefline.start.z<pArmBand->scopeMinZ-TOLERANCE||briefline.start.z>pArmBand->scopeMaxZ+TOLERANCE)
				continue;	//ʼ�˵㲻�ڵ�ǰ�ᵣ������
			if(briefline.end.z<pArmBand->scopeMinZ-TOLERANCE||briefline.end.z>pArmBand->scopeMaxZ+TOLERANCE)
				continue;	//�ն˵㲻�ڵ�ǰ�ᵣ������
			if(briefline.start.y<0&&briefline.end.y<0)
				continue;	//���˼�
		}
		else
			return false;
		briefline.start.y=briefline.start.z;
		briefline.end.y  =briefline.end.z;
		if(pArmBand->ciOnRightSide=='L')
		{	//���ᵣͨ��Y��ԳƷ�ת��ͨ��1�����¶ν����ж�
			briefline.start.x*=-1.0;
			briefline.end.x*=-1.0;
		}
		double justify_start=DistOf2dPtLine(briefline.start,postline.start,postline.end);
		if(justify_start>5.0)
			continue;	//�˼�ʼ�˵�������ֽ������(ǰ��ͼ��,���ݷֽ���ʼ->�տ����Ҳ�)
		double justify_end=DistOf2dPtLine(briefline.end,postline.start,postline.end);
		if(justify_end>5.0)
			continue;	//�˼��ն˵�������ֽ������(ǰ��ͼ��,���ݷֽ���ʼ->�տ����Ҳ�)
		if(justify_start<-5||justify_end<-5)
			rodset.append(pRod);//���ٴ���һ����ȷλ������ֽ����Ҳ�(ǰ��ͼ��,���ݷֽ���ʼ->�տ������)
	}
	//2.���ɶ�ά�ᵣ�˼����߿�ͼ��
	GECS ocs;
	if(pArmBand->ciOnRightSide=='L')
		ocs.axis_x.Set(-1,0,0);
	else
		ocs.axis_x.Set( 1,0,0);
	ocs.axis_y.Set(0, 0,1);
	ocs.axis_z.Set(0, 1,0);
	SCOPE_STRU scope;
	CXhSimpleList<RODLINE> listBriefLines;
	CXhChar50 name;
	for(pRod=rodset.GetFirst();pRod!=NULL;pRod=rodset.GetNext())
	{
		if(name.Length==0)
			name.Printf("0x%X",pRod->handle);
		f3dPoint lineStart=pRod->pStart->xPosition;
		f3dPoint lineEnd=pRod->pEnd->xPosition;
		lineStart=ocs.TransPToCS(lineStart);
		lineEnd  =ocs.TransPToCS(lineEnd);
		scope.VerifyVertex(lineStart);
		scope.VerifyVertex(lineEnd);
		listBriefLines.Append(RODLINE(lineStart,lineEnd,pRod->handle));
	}
	CByteMonoImage image(NULL,ftoi(scope.wide()*SCALE_OF_IMAGE2MODEL)+1,ftoi(scope.high()*SCALE_OF_IMAGE2MODEL)+1);
	GEPOINT xOrgOffset(scope.fMinX,scope.fMinY);
	if(pArmBand->ciOnRightSide=='L')
		ocs.origin.Set(-scope.fMinX,0,scope.fMinY);
	else
		ocs.origin.Set( scope.fMinX,0,scope.fMinY);
	DYN_ARRAY<PIXEL_RELAOBJ> arrRelaObjs(image.GetImageWidth()*image.GetImageHeight());
	image.SetRelaObjMap(arrRelaObjs);
	for(RODLINE* pLine=listBriefLines.EnumObjectFirst();pLine;pLine=listBriefLines.EnumObjectNext())
		image.DrawLine((pLine->briefline.start-xOrgOffset)*SCALE_OF_IMAGE2MODEL,(pLine->briefline.end-xOrgOffset)*SCALE_OF_IMAGE2MODEL,pLine->hRod);
	CHashList<long>hashRodHits;
	CHashList<IModelNode*>hashFrontNodes;
	//����ͼ����
	for(UINT k=0;k<arrRelaObjs.Size();k++)
	{
		if(arrRelaObjs[k].hRod==0)
			continue;
		long* pHits=hashRodHits.GetValue(arrRelaObjs[k].hRod);
		if(pHits==NULL)
			pHits=hashRodHits.SetValue(arrRelaObjs[k].hRod,1);
		else
			(*pHits)++;
		if((*pHits)==4)
		{	//�˼����߳���4�����ص㼴����Ϊ����
			pRod=m_pModel->FromRodHandle(arrRelaObjs[k].hRod);
			if(pRod->layer[0]!='L')
				pRod->layer[0]='T';
			pRod->layer[2]='Q';
			pRod->SetLayerRevState();
			if(hashFrontNodes.GetValue(pRod->pStart->handle)==NULL)
			{
				short justify=0;
				if(pArmBand->ciOnRightSide=='U'&&pRod->pStart->xPosition.z<pArmBand->scopeMaxZ-1)
					justify=-1;
				else
				{
					GEPOINT point(pRod->pStart->xPosition.x,pRod->pStart->xPosition.z);
					if(pArmBand->ciOnRightSide=='L')
						point.x*=-1;
					justify=SideOfPtLine(point,postline.start,postline.end,1);
				}
				if(justify<0)	//�Ҳ�ᵣ�ڵ�
				{
					if(pRod->pStart->layer[0]!='L')
						pRod->pStart->layer[0]='T';	//�ų�ԭ���Ƚڵ���Ϊ�˷�����һ�����½��ȶ�����һ�� wjh-2018.7.7
					pRod->pStart->layer[2]='Q';
					pRod->pStart->SetLayerRevState();
					hashFrontNodes.SetValue(pRod->pStart->handle,pRod->pStart);
				}
			}
			if(hashFrontNodes.GetValue(pRod->pEnd->handle)==NULL)
			{
				short justify=0;
				if(pArmBand->ciOnRightSide=='U'&&pRod->pEnd->xPosition.z<pArmBand->scopeMaxZ-1)
					justify=-1;
				else
				{
					GEPOINT point(pRod->pEnd->xPosition.x,pRod->pEnd->xPosition.z);
					if(pArmBand->ciOnRightSide=='L')
						point.x*=-1;
					justify=SideOfPtLine(point,postline.start,postline.end,1);
				}
				if(justify<0)	//�Ҳ�ᵣ�ڵ�
				{
					if(pRod->pEnd->layer[0]!='L')
						pRod->pEnd->layer[0]='T';	//�ų�ԭ���Ƚڵ���Ϊ�˷�����һ�����½��ȶ�����һ�� wjh-2018.7.7
					pRod->pEnd->layer[2]='Q';
					pRod->pEnd->SetLayerRevState();
					hashFrontNodes.SetValue(pRod->pEnd->handle,pRod->pEnd);
				}
			}
		}
	}
	int width =image.GetImageWidth();
	int height=image.GetImageHeight();
	int nEffWidth=(width+7)/8;

	const BYTE BIT2BYTE[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	BYTE_ARRAY imagebits(nEffWidth*height,true);
	BYTE *prow=imagebits, *rowdata=image.GetImageMap();
	for(int j=0;j<height;j++)
	{
		for(int i=0;i<width;i++)
		{
			int ibyte=i/8,ibit=i%8;
			//�������ᵣ���˴���Y�᾵��תΪ�Ҳ�ᵣ���Ա����ͳһ���� wjh-2018.5.7
			//BYTE cbImgPixelByte=pArmBand->ciOnRightSide=='L'?rowdata[width-1-i]:rowdata[i];
			if(rowdata[i]==0)	//�׵�
				*(prow+ibyte)|=BIT2BYTE[7-ibit];
		}
		rowdata+=width;
		prow   +=nEffWidth;
	}
#if defined(__PUBLIC_RELEASE_COMPILE_)&&defined(_DEBUG)
//#ifdef _DEBUG
	image.WriteMonoBmpFile(CXhChar100("c:/arm#%s.bmp",(char*)name),image.GetImageWidth(),nEffWidth,image.GetImageHeight(),imagebits);
//#endif
	if(PRODUCT_FUNC::IsHasInternalTest())
	{
		pLogErr->Log("\n***********************************");
		pLogErr->Log("ARM-%s recognizing",(char*)name);
		StrCopy(pLiveMatchArmTempl->name,name,50);
	}
#endif
	//3.���ݺᵣ���߿�ͼ��ʶ��ƥ������кᵣģ��
	return RecogStdArmTemplByCoreImage(pLiveMatchArmTempl,&image,pArmBand->ciOnRightSide,ocs,SCALE_OF_IMAGE2MODEL);
}
#endif
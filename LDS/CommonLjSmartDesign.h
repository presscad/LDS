// CommonLjSmartDesign.h: interface for the CMultiRayEndLj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMONLJSMARTDESIGN_H__49DD48F0_EB40_4F48_92DC_A080FD9A4BFC__INCLUDED_)
#define AFX_COMMONLJSMARTDESIGN_H__49DD48F0_EB40_4F48_92DC_A080FD9A4BFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ArrayList.h"
#include "ConnBoltsStrength.h"
#ifdef __ANGLE_PART_INC_
struct RAYROD_INFO
{
protected:
	bool _blViewAsSingularAngle;
public:
	f3dPoint rayVec;			//�Ǹ����߷����ѵ�λ����
	f3dPoint wing_vec,wing_norm;//�Ǹֹ�������֫���򼰷��߷���
	int work_wing;				//�Ǹֵ�ǰ��������֫0:wing x;1:wing y
	BYTE ciPlaneNo;		//���ڵ��������,1(����),2,3
	BOOL bInsideAngle;			//�Ƿ�Ϊ����
	CConnectInfo* pConn;		//���߽Ǹ����Ӷ˵���˨������Ϣ
	int endLjBoltNum;			//���߽Ǹֶ�������˨��0:���ɽ��ж�����;1:�ɽ���һ����˨������;2:�ɽ���������˨������
	JGZJ jgzj;					//�Ǹ�׼��
	LSSPACE_STRU LsSpace;		//���߲���˨���ü���
	BOOL bOnSideBer;			//TRUE:���߲�λ�ڻ�׼�����߲�;FALSE:���߲�λ�ڻ�׼��֫���
	double alfa;				//��λ�㣬���߽Ǹ������ӹ�����������ڻ�׼�ĵ�ƽ����ת��(��׼��ʼ->��ΪX��,�����淨��ΪZ��)
	double cosa,sina;			//���׼�ļнǵ�������ֵ
	union ENDPOS_PARA{			//���߸˼����Ӷ˵�λ����Ʋ���
		CMultiOffsetPos* pAnglePos;
		CTubeEndPosPara* pTubePos;
	}pos;
	union ROD{					//���߸˼�ָ��
		CLDSLinePart* pRod;
		CLDSLineAngle* pAngle;
		CLDSGroupLineAngle* pGroupAngle;
		CLDSLineTube* pTube;
	}rod;
	bool get_blViewAsSingularAngle() const;
	bool set_blViewAsSingularAngle(bool blValue);
	__declspec(property(get = get_blViewAsSingularAngle,put=set_blViewAsSingularAngle)) bool blViewAsSingularAngle;
	CMultiOffsetPos* AnglePos(){return pos.pAnglePos;}
	CTubeEndPosPara* TubePos(){return pos.pTubePos;}
	CLDSLinePart* Rod(){return rod.pRod;}
	CLDSLineAngle* Angle(){return rod.pAngle;}
	__declspec(property(get = Rod)) CLDSLinePart* pRod;
	__declspec(property(get = Angle)) CLDSLineAngle* pAngle;
	CLDSLineTube* Tube(){return rod.pTube;}
	static int CompareRayRod(const RAYROD_INFO& rodInfo1,const RAYROD_INFO& rodInfo2);
	RAYROD_INFO() { memset(this, 0, sizeof(RAYROD_INFO)); ciPlaneNo = 1; }
};
struct COVER_REGION{
	double lenOffsetStart;	//�Խڵ��ڻ�׼������(g,g1,g2)ͶӰΪ���㶨λ���ػ�׼��������ʼ���շ������ʼƫ����
	double lenOffsetEnd;	//�Խڵ��ڻ�׼������(g,g1,g2)ͶӰΪ���㶨λ���ػ�׼��������ʼ���շ������ֹƫ����
	bool bDatumSonAngle;
	int iRayOrder;			//����(�Ǹ�)��������
};
class CIntelliModelDesigner;
class CCommonLjSmartDesign
{
	BOOL m_ciRayEndFaceOffset;	//���߲Ķ�ͷƽ�ƴ�����ʱ��׼����˨Ĭ��ҲӦΪƽ��
	CLDSBolt *m_pFirstEndLjBolt,*m_pSecondEndLjBolt;
	int wing_intersline_style;	//�����˨������ʱ�����߲Ľ����������߷�ʽ����������˨�������ߣ�
protected:
	CFGWORD cfgword;
	UCS_STRU ucs;
	long m_nBaseStartG3,m_nBaseEndG3;	//��׼��ʼ�ղ��������˨��ѭ������׼��
	JGZJ m_jgzjBase;		//��׼�Ǹֵĵ�ǰ֫׼��
	CConnectPointInfo *m_pConnectPoint;
	CLDSNode* m_pBaseNode;
	CSuperSmartPtr<CLDSLinePart>m_pBaseRod;
	int m_iBaseAngleWorkWing;		//0:wing x;1:wing y
	int m_iFirstEndLjAngle,m_iSecEndLjAngle;
	f3dPoint base_wing_vec,base_wing_norm;
	CTmaPtrList<CLDSBolt,CLDSBolt*> boltset,endlj_boltset;	//��ʱ��¼��ǰ�����а�������˨
	PARTSET padSet;
	ARRAY_LIST<RAYROD_INFO>m_arrRayRod;
	ARRAY_LIST<COVER_REGION>m_arrG1Cover,m_arrG2Cover;	//�����ڻ�׼��g1(��g)��g2�����ϵ��ڸ�����
	CLDSBolt *m_pShareBolt;	//������ʱ�Ĺ�����˨
	CLDSPlate *m_pPlate;	//�а�����ʱ�����Ӱ�
	CONNBOLTS xBaseAngleBolts;
	CLDSLineAngle* BaseAngle(); 
	CLDSGroupLineAngle* BaseGroupAngle(); 
	CLDSLineTube* BaseTube(); 
	void SetPlateBasicInfo();
	BOOL Prepare(int endljNum,BOOL bHasPlate,BOOL canMultiRow,BOOL priorMiddleEndLj=TRUE);	//�������ǰ��׼������
	virtual void LayoutBaseAngleBolts();
	virtual bool DesignBraceConnParams(short* psiStyleG,double* pdfOffsetDistG);	//��Ƹ������ӵ���ز���
	void LayoutAngleBolts(int index);
	BOOL DesignBendPlane(CConnectPointInfo *pConnectPoint);
	//��ȡiRayRodOrder���߽Ǹֵ����ฺͷ������ո˼� wjh-2019.12.13
	BOOL GetOddCalNearRods(int iRayRodOrder,long* phRefRod1=NULL,long* phRefRod2=NULL);
public:
	CCommonLjSmartDesign();
	virtual ~CCommonLjSmartDesign();
	static bool DesignSlopeTransition(SLOPE_TRANSITION* pTransit,CIntelliModelDesigner* pIntelliDesigner=NULL);
	static bool DesignSlopOrFootPoint(SLOPE_TRANSITION* pTransit);
	long DesignConnectPoint(CConnectPointInfo *pConnectPoint,int endljNum,BOOL bHasPlate,BOOL canMultiRow=TRUE,BOOL priorMiddleEndLj=TRUE,bool bCreate3dSolid=true);
};
struct ENDLJ_ANGLEPTR{
	int iAngleOrder;	//���սǶȵ�����������
	RAYROD_INFO *pRod;
};
class CGroupAngleLjDesign:public CCommonLjSmartDesign
{
	CLDSLineAngle *m_pDatumSonAngle;	//��ϽǸ��е�1�������Ǹ�
	CLDSLineAngle *m_pCoupleSonAngle;	//��ϽǸ�����1�������Ǹ�����Ե��ӽǸ�
	CLDSLineAngle *m_pCoupleSonAngle2;	//��ϽǸ�����1�������Ǹ�����Ե��ӽǸ�1 3
	CLDSLineAngle *m_pCoupleSonAngle4;	//��ϽǸ�����1�������Ǹ�����Ե��ӽǸ�2 4
	ARRAY_LIST<ENDLJ_ANGLEPTR>m_arrRayRodPtr;
protected:
	static int CalFillPlateBoltNum(int wide);
	//axis_g_n=1,ֻ����g���߸�������axis_g_n=2,ֻ����g1,g2���߸�������axis_g_n=3,����g,g1,g2���߸�������
	virtual void CalRayCoverRegion(int iRayOrder,COVER_REGION cover_arr[],int axis_g_n=3);
	virtual BOOL Prepare(BOOL bEndLjPrefer=TRUE);	//�������ǰ��׼������
	virtual void LayoutBaseAngleBolts();
	void LayoutAngleBolts(int index,CLDSBolt *pDatumEndLjBolt=NULL);
public:
	static int FILLPLATE_BOLT2_WIDEMAX;
	static int FILLPLATE_BOLT3_WIDEMAX;
	static int FILLPLATE_BOLT4_WIDEMAX;
	static int FILLPLATE_BOLT5_WIDEMAX;
	static int FILLPLATE_BOLT6_WIDEMAX;
	CGroupAngleLjDesign();
	virtual ~CGroupAngleLjDesign();
	long DesignConnectPoint(CConnectPointInfo *pConnectPoint,BOOL bEndLjPrefer=TRUE);
};

#endif
#endif // !defined(AFX_COMMONLJSMARTDESIGN_H__49DD48F0_EB40_4F48_92DC_A080FD9A4BFC__INCLUDED_)

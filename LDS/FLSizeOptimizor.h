// FLSizeOptimizor.h: interface for the CFLSizeOptimizor class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_FLSIZEOPTIMIZOR_H__D080CAF2_07E5_4A8C_B8D0_50B39DE35328__INCLUDED_)
#define AFX_FLSIZEOPTIMIZOR_H__D080CAF2_07E5_4A8C_B8D0_50B39DE35328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__LDS_)
#include "AnalyzeProcessor.h"

class CJointCouple{
public:
	CLDSNode *pNode;
	CLDSLinePart *pRod;
	CLDSLinePart *pLinkRod;
	union{
		struct FL_INFO{
			CConnectFL *pFLInfo,*pCoupleFlInfo;
			CLDSPart *pFL,*pCoupleFl;
		}FL;			//���������Ϣ
		ANGLE_JOINT* pAngleJoint;	//��ͷ�����Ϣ
	}info;
	double N,M;	//N ������(N),M���(N*m)��һ��ΪpRod�˵�ԪΪ��׼
	char label[16],labelCouple[16];
	CJointCouple(){memset(this,0,sizeof(CJointCouple));}
	BOOL IsRelativeMirWith(CJointCouple *pCouple);
	static void StatJointLinkPart(ATOM_LIST<CJointCouple>* pListCouple, int idClassType=0);	//�Ż�ѡFLʱ��Է��������˼�
};
struct BOLT_SIZE_INFO
{
	double gradeF;		//������ʽ��������˨����
	int d;				//��˨ֱ��,mm
	double Nt;			//�������������,N
	int inner_space;	//��ܱ���С��϶
	int outer_space;	//��ܱ���С��϶
};
//FLDESIGN_PROC_INFO�࣬���ڼ�¼������ƹ����е�һЩϸ����Ϣ,�Ա�������У��
struct FLDESIGN_PROC_INFO
{
	int bolt_n;		//��˨����
	double fl_dia;	//����ֱ��,mm
	double bolt_dia;//��˨Բֱ��,mm
	int rib_height;	//�߰�߶�,mm
	int rib_height_couple;	//����߰�߶�,mm
	int rib_thick;	//�߰���,mm
	double Ntbmax;	//������˨�����������,N
	double Lx,Ly;	//�Ӿ�����Ƭ����,��,mm
	double q;		//���Ͼ�������MPa
	double alfa;	//�Ӿ��巴����ϵ��
	double beta;	//���ϵ��
	double Mox;		//�������N*m
	double tao_rib;	//�����Ӧ��MPa
	double sigm_rib;//������Ӧ��MPa
	double f_fl;	//�����ֲ�ǿ�ȼ�����ǿ�����ֵMPa
	double fw_rib,fvw_rib;	//�Ӿ���ֲĺ���ǿ�ȼ�����ǿ�����ֵMPa
	double m;		//�޾�����������˨��������ϵ��,һ��ȡ0.65
	double Rf;		//�޾�������Ķ���,N
	double S;		//��˨���
	double tao_none;//�޾��������Ӧ��,MPa
	double sigm_none;//�޾���������Ӧ��,MPa
	//0.��Ƴɹ���1.��˨ǿ�Ȳ���;2.�߰���С��������ǿ��Ҫ��;3.�߰�߶Ȳ�����ǿ��Ҫ��;4.��Է����߰�߶Ȳ�����ǿ��Ҫ��;
	int retCode;//5.ָ����˨Բ�뾶��С;6.ָ�������⾶̫С
	
};
struct BOLTSIZE_ITEM{
	double gradeF;	//��˨����
	int d;		//��˨ֱ��
};
class CFLSizeOptimizor  
{
	CObjNoGroup flNoGroup;
	CHashList<CConnectFL> hashFLStep;
	CHashList<CConnectFL>hashConnectFL;
private:
	static long BOLT_TYPE_N;
	static BOLT_SIZE_INFO BOLTSIZE_TBL[24];
	//0Ϊ��˨��Ż���
	double CalYi(int i,int n,double bolt_radius,double axisCoordX);
	double CalY1(int n,double bolt_radius,double axisCoordX);
	double CalNtbmaxCoef(int n,double bolt_radius,double axisCoordX);
	double QueryBetaCoef(double Ly_Lx);
	double QueryAlfaCoef(double Ly_Lx);
	void GenStep1FLSet();
public:
	static void InitBoltSizeTbl(BOLTSIZE_ITEM *boltSizeArr,int size_n=0);
	//��Ʒ���ʱ��ѭ����ƹ淶��
	//0.���ܿ��͵���·�ֹ�����Ƽ����涨��	DL/T XXXX-2009���ܿ������·�ֹ���������ƹ涨�� Q/GDW 391-2009
	//1.���ֽṹ�ֹ�ͨ����������̡�		CECS 236:2008
	//2.���ܿ��͵���·�����ṹ��Ƽ����涨��DL/T 5154-2002
	int m_iDesignCode;	
	MASKFLAG designStyleFlag;
	static DWORD eSpecBoltNum;		//0x00 0001 00:00000000 0000000000000001 00000000
	static DWORD eSpecFlDiameter;	//0x00 0002 00:00000000 0000000000000010 00000000
	static DWORD eSpecBoltDiameter;	//0x00 0004 00:00000000 0000000000000100 00000000
	static DWORD eSpecRibHeight;	//0x00 0008 00:00000000 0000000000001000 00000000
	static DWORD eSpecRibThick;		//0x00 0010 00:00000000 0000000000010000 00000000
	ATOM_LIST<CJointCouple>listCouple;
	CFLSizeOptimizor();
	virtual ~CFLSizeOptimizor();
	void Clear();	//�����ǰ������ѡ��������
	//M:���(N*m);N����(N,m����Ϊ��);inner_radius:�ֹܰ뾶;cMaterial:�ֹܲ���;�Զ��������������Ƿ�Ҫ�Ӿ�����
	CConnectFL SelFL(double M,double N,double inner_radius_arr[2],char cMaterial);
	//M:���;N����(����Ϊ��);inner_radius:�ֹܰ뾶;t:�ֹܱں�;cMaterial:�ֹܲ���;
	CConnectFL SelFLHasRib(FLDESIGN_PROC_INFO* design, double M,double N,double inner_radius_arr[2],char cMaterial);
	//M:���;N����(����Ϊ��);inner_radius:�ֹܰ뾶;t:�ֹܱں�;cMaterial:�ֹܲ���;
	CConnectFL SelFLNoneRib(FLDESIGN_PROC_INFO* design, double M,double N,double inner_radius,char cMaterial);
	void GenFLSet();						    //�Ż�FL������Ϊ�����������
};
class CAngleJointDesigner
{
public:	//���ýǸֽ�ͷ��ƹ��ߺ���
	//�������ָ����ͷ, ANGLE_JOINT::hSmallRod,hBigRod,smallAngle,bigAngleΪ�������
	static bool SelectAngleJoint(ANGLE_JOINT* pAngleJoint,char cJointType=-1);
	//���������ListCouple��������ɽǸֽ�ͷ���������hashConnectAngleJoints��
	static void GenAngleJointSet(ATOM_LIST<CJointCouple> *pListCouple,CHashList<ANGLE_JOINT>* pHashAngleJoints);
};
#endif
#endif // !defined(AFX_FLSIZEOPTIMIZOR_H__D080CAF2_07E5_4A8C_B8D0_50B39DE35328__INCLUDED_)

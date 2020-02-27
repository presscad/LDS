#pragma once
#include "PartLib.h"

struct IPartLibrary{
	virtual double FindAngleInnerR(double wide)=0;	//��ѯ�Ǹ���Բ���뾶
	virtual JGZJ   GetAngleZJ(double wide)=0;		//��ѯ�Ǹ�׼��
	virtual double GetPartArea(double size_para1,double size_para2,double size_para3,int idPartClsType,char cSubType='L')=0;
	virtual double GetWeightOf1Meter(double size_para1,double size_para2,double size_para3,int idPartClsType,char cSubType='L')=0;
	virtual double GetWorkI(double size_para1,double size_para2,double size_para3,int axis,int idPartClsType,char cSubType='L')=0;
	virtual double GetWorkW(double size_para1,double size_para2,double size_para3,int axis,int idPartClsType,char cSubType='L')=0;
	virtual double GetWorkR(double size_para1,double size_para2,double size_para3,int axis,int idPartClsType,char cSubType='L')=0;
	virtual double GetElasticityModulus(char cMaterial)=0;
	virtual STEELMAT* QuerySteelMatByBriefMark(char briefmark)=0;
	virtual double GetW(double size_para1,double size_para2,double size_para3,int axis,int idPartClsType,char cSubType='L')=0;
	virtual void FindLsBoltDH(int d, double *D,double *H,double *H2)=0;
#if defined(__TSA_)||defined(__LDS_)
	//����ר�ò�ѯ����
	virtual double GetWireArea(char* wireType,char* wireSpec)=0;			//���½����,mm2
	virtual double GetWireUnitWeightPerKM(char* wireType,char* wireSpec)=0;	//��λ������,kg/km
	virtual double GetWireForcePermit(char* wireType,char* wireSpec)=0;		//�������,N
	virtual double GetWireElasticCoef(char* wireType,char* wireSpec)=0;		//����ϵ��(ģ��),MPa
	virtual double GetWireExpansionCoef(char* wireType,char* wireSpec)=0;	//�¶�������ϵ��,
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//���ϵͳ����Ӧ����ɢ�� ��ʱ���ѯ������ĺ�������һ���Ժ��ٽ�IPartLibrary������չ wht 10-02-23 
	virtual double GetStressDiffuseAngle()=0;
	virtual UINT GetVertexDist()=0;
	virtual UINT GetHuoQuClearance()=0;	//����ϵͳ����������϶
	virtual double GetJgKaiHeJiaoThreshold()=0;	//��ȡ�Ǹֿ��Ͻ���ֵ wht 11-05-06	
	virtual double GetJgKaiHeJiaoMinDeita()=0;
	virtual double GetMaxKaiHeJiaoClearance()=0;
	virtual double GetMinCutAngleLen()=0;
	virtual double GetMaxIgnoreCutAngleLen()=0;
	//��ȡ�нǼ�϶��Բ���뾶ռ��ϵ��
	virtual double GetCutAngleCoefR()=0;
	//��ȡ�ְ����������㷨
	virtual int GetProfileAlgorithm()=0;
	//�õ���׼����ģ��
	virtual bool GetStdPartModel(char *sPartNo,int std_part_type, void *pStdPartModel,double t=0)=0;
	//���ݸֹ�ֱ������ȡ�����Ͷ˰����͵Ȳ�ѯ��׼��
	virtual bool GetStdPartModel(double d,double t,char cTubeMat,BYTE level,int std_part_type, void *pStdPartModel)=0;
	//�õ���Ȧ��� wht 10-12-24
	//ciGetFirst0Max1=0,���ص�һ�������Ȧ���ֵ��
	//ciGetFirst0Max1=1,�������������Ȧ���ֵ��=2,������С��������ֵ wjh-2019.7.24
	virtual int GetBoltPadThick(int bolt_d,char ciGetFirst0Max1Min2=0)=0;
	virtual int GetWasherThicks(int niMd,BYTE* parrThicks,int nMaxThickCount=4)=0;
	virtual short GetBestMatchWasherThick(int niMd,double distance)=0;	//������ǡ���ĵ�Ȧ���
	//����ƥ�����˨����ͺ�
	virtual LS_XING_HAO *FindLsGuiGe(int d, double L0,long hFamily=0)=0;
	//������˨ֱ������Ч������ƥ����˨�ĵ�������,kg
	virtual double GetLsWeight(int d, double L,long hFamily=0)=0;
#endif
	virtual COLORREF GetColor(int idClassType,long itemType,BYTE cMaterial)=0;
};

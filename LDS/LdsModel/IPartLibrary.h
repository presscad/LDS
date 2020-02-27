#pragma once
#include "PartLib.h"

struct IPartLibrary{
	virtual double FindAngleInnerR(double wide)=0;	//查询角钢内圆弧半径
	virtual JGZJ   GetAngleZJ(double wide)=0;		//查询角钢准距
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
	//线缆专用查询函数
	virtual double GetWireArea(char* wireType,char* wireSpec)=0;			//线缆截面积,mm2
	virtual double GetWireUnitWeightPerKM(char* wireType,char* wireSpec)=0;	//单位长重量,kg/km
	virtual double GetWireForcePermit(char* wireType,char* wireSpec)=0;		//额定抗拉力,N
	virtual double GetWireElasticCoef(char* wireType,char* wireSpec)=0;		//弹性系数(模量),MPa
	virtual double GetWireExpansionCoef(char* wireType,char* wireSpec)=0;	//温度线膨胀系数,
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//获得系统参数应力扩散角 暂时与查询构件库的函数放在一起，以后再将IPartLibrary进行扩展 wht 10-02-23 
	virtual double GetStressDiffuseAngle()=0;
	virtual UINT GetVertexDist()=0;
	virtual UINT GetHuoQuClearance()=0;	//返回系统参数火曲间隙
	virtual double GetJgKaiHeJiaoThreshold()=0;	//获取角钢开合角阈值 wht 11-05-06	
	virtual double GetJgKaiHeJiaoMinDeita()=0;
	virtual double GetMaxKaiHeJiaoClearance()=0;
	virtual double GetMinCutAngleLen()=0;
	virtual double GetMaxIgnoreCutAngleLen()=0;
	//获取切角间隙内圆弧半径占比系数
	virtual double GetCutAngleCoefR()=0;
	//获取钢板外形生成算法
	virtual int GetProfileAlgorithm()=0;
	//得到标准构件模型
	virtual bool GetStdPartModel(char *sPartNo,int std_part_type, void *pStdPartModel,double t=0)=0;
	//根据钢管直径、厚度、级别和端板类型等查询标准件
	virtual bool GetStdPartModel(double d,double t,char cTubeMat,BYTE level,int std_part_type, void *pStdPartModel)=0;
	//得到垫圈厚度 wht 10-12-24
	//ciGetFirst0Max1=0,返回第一条适配垫圈厚度值；
	//ciGetFirst0Max1=1,返回最大的适配垫圈厚度值；=2,返回最小的适配厚度值 wjh-2019.7.24
	virtual int GetBoltPadThick(int bolt_d,char ciGetFirst0Max1Min2=0)=0;
	virtual int GetWasherThicks(int niMd,BYTE* parrThicks,int nMaxThickCount=4)=0;
	virtual short GetBestMatchWasherThick(int niMd,double distance)=0;	//返回最恰当的垫圈厚度
	//查找匹配的螺栓规格型号
	virtual LS_XING_HAO *FindLsGuiGe(int d, double L0,long hFamily=0)=0;
	//根据螺栓直径及有效长查找匹配螺栓的单颗重量,kg
	virtual double GetLsWeight(int d, double L,long hFamily=0)=0;
#endif
	virtual COLORREF GetColor(int idClassType,long itemType,BYTE cMaterial)=0;
};

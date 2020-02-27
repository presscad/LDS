#pragma once
// 下列 ifdef 块是创建使从 DLL 导出更简单的
//宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CODES_EXPORTS
// 符号编译的。在使用此 DLL 的
//任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将 
// CODES_API 函数视为是从此 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef CODES_EXPORTS
#define CODES_API __declspec(dllexport)
#else
#define CODES_API __declspec(dllimport)
#endif
#include <math.h>
#include "LogFile.h"

struct STEELMAT;
struct CODE_PROPITEM{
	static const BYTE DTVALTYPE_BOOL	= 0;
	static const BYTE DTVALTYPE_LONG	= 1;
	static const BYTE DTVALTYPE_DOUBLE	= 2;
	static const BYTE DTVALTYPE_STR100	= 3;
public:
	long id;
	char idKeyStr[64];	//标识字符串
	char name[96];
	char ciValType;	//0.bool;1.long;2.double;3.短字符串
	union{
		bool bVal;
		long liVal;
		double dfVal;
		char szStrBuff[101];
	};
};
union LOAD_CODES{
	//低位短整数表示规范系列号, 高位短整数表示规范版本号, 0值默认表示《技术规定》
	static const int GB50135_2006	= 0x000001;	//高耸结构设计规范2006版
	static const int ASCE10_97 = 0x000002;	//美国ASCE10-97
	static const int DLT_2002 = 0x000003;	//国内电力杆塔结构设计技术规定 2002版
	static const int DLT_2012 = 0x010003;	//国内电力杆塔结构设计技术规定 2012版
	static const int DLT_2018 = 0x010004;	//国内架空输电线路荷载规范DLT5551-2018. 2018版
public:
	struct{
		short family;	//规范系列
		short version;	//规范版本号
	}details;
	int serial;
	operator long(){return serial;}
	LOAD_CODES(int codeSerial=0){serial=codeSerial;}
	LOAD_CODES(short family,short version=0){
		details.family=family;
		details.version=version;
	}
};
struct CODES_API ILoadCalCode{
	long m_idPostRodKlamdaLimit;
	long m_idPrimaryRodKlamdaLimit;
	long m_idBraceRodKlamdaLimit;
	long m_idTensionElemLamdaLimit;
	long m_idBraceCoefOfRedundantToPost;
	long m_idBraceCoefOfRedundantToPrimary;
	long m_idBraceCoefOfPrimaryToPost;
protected:
	CODE_PROPITEM* AddLocaleItemLong(const char* keyidstr,long liPropVal,const char* name = NULL);
	CODE_PROPITEM* AddLocaleItemDouble(const char* keyidstr,double dfPropVal,const char* name = NULL);
	CODE_PROPITEM* AddLocaleItemBool(const char* keyidstr,bool blPropVal,const char* name=NULL);
	CODE_PROPITEM* AddLocaleItemStr100(const char* keyidstr, const char* pszPropVal, const char* name = NULL);
	int InitPropHashtable(int iLoadCalCode);
public:
	static const BYTE XUAN_CHUI = 0;//悬垂挂线;
	static const BYTE NAI_ZHANG = 1;//耐张挂线
	//static ILoadCalCode* CODE_DLT_2002();
	static ILoadCalCode* CODE_DLT_2012();
	static ILoadCalCode* CODE_DLT_2018();
	static ILoadCalCode* CODE_ASCE10_97();
	static ILoadCalCode* FromCodeSerial(int codeSerial);
	static bool GetSupportCodesStr(char* codesStr,int maxStrBufLength);
	long get_liCodeSerial() { return CodeSerial(); }
	__declspec(property(get=get_liCodeSerial)) long liCodeSerial;
	CODE_PROPITEM* GetCodeItem(const char* keyidstr);
	//根据属性ID得到属性值字符串
	virtual int GetItemValueStr(long idItemProp,char* valueStr);
	//virtual int InitDefaultPropValue()=0;
	virtual long GetPropId(const char* itemKeyStr);
	//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
	virtual double GetItemValue(long idItemProp)=0;
	virtual double GetItemValue(const char* szItemKeyStr);
	virtual double GetSetupAttachWeightLoad(long volt_grade=500,char ciWireType='E',bool bNaiZhang=true);
	virtual double FindPercentInHeavyIceArea(long volt_grade=500,bool bNaiZhang=true);
	//导地线在不均匀覆冰情况下的不平衡张力取值系数
	virtual double FindUnbalanceIceCoef(double ice_thick,char cWireType='E',long volt_grade=500,bool bNaiZhang=true);
	virtual double FindBreakWireCoefLoadY(
		long wVoltGrade=500,long wFenLieNum=1,	//电压等级,分裂数
		char cWireFuncType='C',	//'E':地线;'C':导线;'O':跳线
		double ice_thick=10,	//覆冰厚度,mm
		char ciHangWireStyle=1,	//挂线方式
		char ciLineRoadType=0);	//线路类型,平地线路、丘陵线路
	virtual double Find_MuZ(double high,char cLandEnv);
	virtual bool GetWindLoadAllocCoef(double *Xcoef_inWx,double *Xcoef_inWy,double *Ycoef_inWx,double *Ycoef_inWy,long azimuth,long iStyle);
	virtual bool GetWindLoadBodyAllocCoef(double *Xcoef_inWx,double *Xcoef_inWy,double *Ycoef_inWx,double *Ycoef_inWy,long azimuth);
	virtual bool GetWindLoadArmAllocCoef(double *Xcoef_inWsc,double *Ycoef_inWsc,long azimuth,long iStyle);
	virtual double CalMusOfTube(double dfMuzW0D_2);	//dfMuzW0D_2=μz*Wo*d^2
public:
	//返回规范的标识系列号
	virtual int CodeSerial()=0;
	//获取规范名称
	virtual char* CodeName()=0;
};
class LOADCOAD_DL_T_2012 : public ILoadCalCode{
	int InitPropHashtable();
public:
	LOADCOAD_DL_T_2012();
	//返回规范的标识系列号
	virtual int CodeSerial(){return LOAD_CODES::DLT_2012;}
	//获取规范名称
	virtual char* CodeName();
	//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
	virtual double GetItemValue(long idItemProp);
};
class LOADCOAD_ASCE10_97 : public ILoadCalCode{
	int InitPropHashtable();
public:
	LOADCOAD_ASCE10_97();
	//返回规范的标识系列号
	virtual int CodeSerial(){return LOAD_CODES::ASCE10_97;}
	//获取规范名称
	virtual char* CodeName();
	//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
	virtual double GetItemValue(long idItemProp);
};
class LOADCOAD_DL_T_2018 : public ILoadCalCode {
	int InitPropHashtable();
public:
	LOADCOAD_DL_T_2018();
	//返回规范的标识系列号
	virtual int CodeSerial() { return LOAD_CODES::DLT_2018; }
	//获取规范名称
	virtual char* CodeName();
	//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
	virtual double GetItemValue(long idItemProp);
	//查询风压高度变化系数
	virtual double Find_MuZ(double high,char cLandEnv);
};

union CODES{
	//低位短整数表示规范系列号, 高位短整数表示规范版本号, 0值默认表示《技术规定》
	static const int GB50135_2006	= 0x000001;	//高耸结构设计规范2006版
	static const int ASCE10_97		= 0x000002;	//美国ASCE10-97
	static const int DLT_2002		= 0x000003;	//国内电力杆塔结构设计技术规定 2002版
	static const int DLT_2012		= 0x010003;	//国内电力杆塔结构设计技术规定 2012版
	//梁元受弯构件设计技术规定
	static const int GB50017_2003	= 0x000004;	//钢结构规范　2003版
	static const int DLT5254_2010	= 0x000005;	//国内钢管塔设计技术规定 2010版
	static const int DLT5130_2001	= 0x000006;	//钢管杆设计技术规定2001版
	static const int CECS236_2008	= 0x000007;	//钢结构单管通信塔技术规程　2008版
public:
	struct{
		short family;	//规范系列
		short version;	//规范版本号
	}details;
	int serial;
	operator long(){return serial;}
	CODES(int codeSerial=0){serial=codeSerial;}
	CODES(short family,short version=0){
		details.family=family;
		details.version=version;
	}
};

struct CODES_API BEAM_SECTION{
	static const BYTE SECT_ANGLE = 1;	//角钢
	static const BYTE SECT_TUBE  = 2;	//圆管
	static const BYTE SECT_POLY  = 3;	//多边形管
	static const BYTE SECT_SLOT  = 4;	//槽钢
public:
	double M[3],P[3];	//梁杆的纵向为Ｚ轴,[0]x分量;[1]y分量;[2]z分量;单位N*mm,N
	double I,W;		//x-x及y-y惯性矩(mm4)和抗弯模量(mm3),暂认为Ix=Iy,Wx=Wy
	double Q_It;	//静矩/I*t,确定最大弯曲剪应力参数,1/mm2
	double C_J;		//半径/极惯性矩,确定最大扭转剪应力参数,1/mm3
	double f,fy;	//材料设计强度、屈服强度,MPa
	double lamda;	//杆件的整体长细比
	double Ag,An;	//毛截面积，净截面积mm2
	BYTE cSectionType;
	union{
		struct{
			double wide,thick;	//角钢肢宽，肢厚(mm)
		}angle;
		struct{
			double D,thick;		//圆管外径及壁厚(mm)
		}tube;
		struct{
			UINT slices;		//多边形管时的边数
			double BR;			//有效弯曲半径,mm
			double D0,thick;	//边到边距离及壁厚(mm)
		}poly;
		struct{
			double wide,height,thick;	//槽钢肢宽、背高及壁厚(mm)
		}slot;
	}para;
public:	//截面验算结果输出信息
	double fsb;		//受弯局部稳定控制强度设计值MPa,
	double fsc;		//受压局部稳定控制强度设计值MPa,
	double fv;		//受剪强度允许值MPa,
	double sigemaNt;	//轴向拉力正应力MPa(即N/mm2)
	double sigemaNc;	//轴向压力正应力MPa(即N/mm2)
	double sigemaBend;	//弯曲强度MPa
	double taoShear;	//剪切应力M=集中力剪应力+扭转剪应力，MPa
	double sigemaComplex;	//复合应力
public:
	static const BYTE COND_TENSION = 1;	//抗拉验算
	static const BYTE COND_COMPBEND= 2;	//压弯局部稳定强度设计
	static const BYTE COND_BENDING = 3;	//弯曲强度
	static const BYTE COND_SHEAR   = 4;	//剪切应力
	static const BYTE COND_COMPLEX = 5;	//复合应力强度设计
	BEAM_SECTION();
	double Check(BYTE* cCtrlCond,int exec_code);	//返回控制条件下的功效系数,cCtrlCond返回控制条件或最危险条件
};

struct CODES_API IMemberDesignCode{
	long m_idPostRodKlamdaLimit;
	long m_idPrimaryRodKlamdaLimit;
	long m_idBraceRodKlamdaLimit;
	long m_idTensionElemLamdaLimit;
	long m_idBraceCoefOfRedundantToPost;
	long m_idBraceCoefOfRedundantToPrimary;
	long m_idBraceCoefOfPrimaryToPost;
	static ILog2File* ERR_LOGGER;
protected:
	int InitPropHashtable(int iDesignCode);
	ILog2File* Log2File();
	__declspec(property(get=Log2File)) ILog2File* pLogErr;
public:
	static double modulusE;		//杨氏弹性模量,MPa
	static ILog2File* SetHostErrorLogger(ILog2File* pErrLogger);
	static IMemberDesignCode* CODE_DLT_2002();
	static IMemberDesignCode* GB50135_2006();
	static IMemberDesignCode* CODE_DLT_2012();
	static IMemberDesignCode* CODE_ASCE10_97();
	static IMemberDesignCode* FromCodeSerial(int codeSerial);
	static bool GetSupportCodesStr(char* codesStr,int maxStrBufLength);
	//根据属性ID得到属性值字符串
	virtual int GetItemValueStr(long idItemProp,char* valueStr);
	long GetPostRodKlamdaLimit(bool bTube=false);
	long GetPrimaryRodKlamdaLimit(bool bTube=false);
	long GetBraceRodKlamdaLimit(bool bTube=false);
	long GetTensionRodKlamdaLimit(bool bTube=false);
	double GetBraceCoefOfRedundantToPost();		//计算辅材对主材的支撑力占比系数
	double GetBraceCoefOfRedundantToPrimary();	//计算辅材对斜材的支撑力占比系数
	double GetBraceCoefOfPrimaryToPost();		//计算斜材对主材的支撑力占比系数
public:
	struct BevelRodLamdaPara{
		char cType;	//'C'同时受压，'c'表示一拉一压(但N0<0.2N)但按同时受压考虑, 其余值按一拉一压考虑
		double K;	//长细比修正系数
		BevelRodLamdaPara(char type=0,double k=1.0){cType=type;K=k;}
	};
public:
	//返回规范的标识系列号
	virtual int CodeSerial()=0;
	//获取规范名称
	virtual char* CodeName()=0;
	//是否考虑埃菲尔效应对塔身斜材的影响
	virtual bool IsIncEiffelEffect(){return false;}
	//ASCE10-97规范中设计强度取屈服强度的90%
	virtual double GetSteelDesignStrenth(double fy,double f){return f;};
	//virtual int InitDefaultPropValue()=0;
	virtual long GetPropId(char* itemKeyStr);
	//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
	virtual double GetItemValue(long idItemProp)=0;
	//计算角钢单元的螺栓减孔数需要输入信息：肢厚t、螺栓直径d0,内排心距g1,X肢心距差gx,Y肢心距差gy,错孔距离S,布置方式(1、2、3)
	virtual double CalElemHoleOutNum(int style,double t,double d0,double d1,double gx=0,double gy=0,double S=0);
	//计算受压材长细比时是否考虑长细比修正系数
	virtual bool IsIncLamdaK(){return true;}
	//普通受力材及辅材杆件修正生长细比,cRodType,'P'：受力材(Primary)；'B':辅助材：辅助材(Brace);endType：杆端状况，具体见DL/T 5154
	virtual double CalRodKLamda(double lamda,char cRodType,
		int start_joint_type,int end_joint_type,int start_force_type,int end_force_type)=0;
	//压杆稳定强度折减系数需输入信息：b:肢翼自由外伸宽度,t：肢厚;f:设计或屈服强度,bending:是否有弯矩
	virtual double CompReduceCoef(double b,double t,double f,double fy=0,double Klamda=0,bool isTube=false,bool bending=false)=0;
	//K为轴心受压杆件长细比修正系数，lamda为长细比，fy材料屈服强度
	virtual double CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t=10)=0;
	//根据交叉斜材内力及布材型式，返回斜材长细比计算参数,N0为绝对值较小内力的代数值,style参照DL/T-2012
	virtual BevelRodLamdaPara IntersBevelRodLamdaCal(double N,double N0,double ratioL2toL3=0);
	//块剪验算
	virtual bool IsSupportBlockShearCheck(){return false;}
	virtual double BlockShearCheck(double N,double endSpace,double midSpace,double c,double t,double f,double fv);
	//受弯构件计算，返回值为功效系数；f为杆件材料的强度设计值,Do>0&&t>0时表示为钢管构件的外径和壁厚
	virtual double BendingRodCheck(double Mx,double Wx,double My,double Wy,double f,double Do=0,double t=0,bool isTube=false);
	//压弯构件计算，返回值为功效系数，参数单位：N(N),A(mm^2),M(N*m),W(cm^3),f(MPa)；
	virtual double BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f)=0;
	//拉弯构件计算，返回值为功效系数；
	virtual double BendingTensionRodCheck(double m,double N,double An,double M,double W,double f);
};

class CODES_API DL_T_2002 : public IMemberDesignCode{
	int InitPropHashtable();
public:
	DL_T_2002(void);
	//返回规范的标识系列号
	virtual int CodeSerial(){return CODES::DLT_2002;}
	//获取规范名称
	virtual char* CodeName();
	//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
	virtual double GetItemValue(long idItemProp);
	//普通受力材及辅材杆件修正生长细比,cRodType,'P'：受力材(Primary)；'B':辅助材：辅助材(Brace);endType：杆端状况，具体见DL/T 5154
	double CalRodKLamda(double lamda,char cRodType,
		int start_joint_type,int end_joint_type,int start_force_type,int end_force_type);
	//压杆稳定强度折减系数需输入信息：b:肢翼自由外伸宽度,t：肢厚;f:设计强度,bending:是否有弯矩
	double CompReduceCoef(double b,double t,double f,double fy=0,double Klamda=0,bool isTube=false,bool bending=false);
	//K为轴心受压杆件长细比修正系数，lamda为长细比
	double CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t=10);
	//根据交叉斜材内力及布材型式，返回斜材长细比计算参数,N0为绝对值较小内力的代数值
	BevelRodLamdaPara IntersBevelRodLamdaCal(double N,double N0,double ratioL2toL3=0);
	//块剪验算(2002版技术规定不考虑块剪，因此功效系数返回0)
	virtual double BlockShearCheck(double N,double endSpace,double midSpace,double c,double t,double f,double fv){return 0;}
	//受弯构件计算，返回值为功效系数；f为杆件材料的强度设计值,Do>0&&t>0时表示为钢管构件的外径和壁厚
	double BendingRodCheck(double Mx,double Wx,double My,double Wy,double f,double Do=0,double t=0);
	//压弯构件计算，返回值为功效系数；
	double BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f);
};
class CODES_API GB50135_2006 : public DL_T_2002
{
	int InitPropHashtable();
public:
	GB50135_2006();
	//返回规范的标识系列号
	virtual int CodeSerial(){return CODES::GB50135_2006;}
	//获取规范名称
	virtual char* CodeName();
	//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
	virtual double GetItemValue(long idItemProp);
	//计算受压材长细比时是否考虑长细比修正系数
	virtual bool IsIncLamdaK(){return false;}
};
class CODES_API DL_T_2012 : public IMemberDesignCode{
	int InitPropHashtable();
public:
	DL_T_2012(void);
	//返回规范的标识系列号
	virtual int CodeSerial(){return CODES::DLT_2012;}
	//获取规范名称
	virtual char* CodeName();
	//是否考虑埃菲尔效应对塔身斜材的影响
	virtual bool IsIncEiffelEffect(){return true;}
	//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
	virtual double GetItemValue(long idItemProp);
	//普通受力材及辅材杆件修正生长细比,cRodType,'P'：受力材(Primary)；'B':辅助材：辅助材(Brace);endType：杆端状况，具体见DL/T 5154
	double CalRodKLamda(double lamda,char cRodType,
		int start_joint_type,int end_joint_type,int start_force_type,int end_force_type);
	//压杆稳定强度折减系数需输入信息：b:肢翼自由外伸宽度,t：肢厚;f:设计强度,bending:是否有弯矩
	double CompReduceCoef(double b,double t,double f,double fy=0,double Klamda=0,bool isTube=false,bool bending=false);
	//K为轴心受压杆件长细比修正系数，lamda为长细比
	double CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t=10);
	//是否支持块剪验算
	bool IsSupportBlockShearCheck(){return true;}
	//受弯构件计算，返回值为功效系数；f为杆件材料的强度设计值,Do>0&&t>0时表示为钢管构件的外径和壁厚
	double BendingRodCheck(double Mx,double Wx,double My,double Wy,double f,double Do=0,double t=0);
	//压弯构件计算，返回值为功效系数；
	double BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f);
};

class CODES_API ASCE10_97 : public IMemberDesignCode{
	int InitPropHashtable();
public:
	ASCE10_97(void);
	//返回规范的标识系列号
	virtual int CodeSerial(){return CODES::ASCE10_97;}
	//获取规范名称
	virtual char* CodeName();
	//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
	virtual double GetItemValue(long idItemProp);
	//ASCE10-97规范中设计强度取屈服强度的90%
	double GetSteelDesignStrenth(double fy,double f){return 0.9*fy;};
	//普通受力材及辅材杆件修正生长细比,cRodType,'P'：受力材(Primary)；'B':辅助材：辅助材(Brace);endType：杆端状况，具体见DL/T 5154
	double CalRodKLamda(double lamda,char cRodType,
		int start_joint_type,int end_joint_type,int start_force_type,int end_force_type);
	//压杆稳定强度折减系数需输入信息：b:肢翼自由外伸宽度,t：肢厚;f:设计强度,bending:是否有弯矩
	double CompReduceCoef(double b,double t,double f,double fy=0,double Klamda=0,bool isTube=false,bool bending=false);
	//K为轴心受压杆件长细比修正系数，lamda为长细比
	double CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t=10);
	//是否支持块剪验算
	bool IsSupportBlockShearCheck(){return true;}
	//受弯构件计算，返回值为功效系数；f为杆件材料的强度设计值,Do>0&&t>0时表示为钢管构件的外径和壁厚
	double BendingRodCheck(double Mx,double Wx,double My,double Wy,double f,double Do=0,double t=0);
	//压弯构件计算，返回值为功效系数；
	double BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f);
};

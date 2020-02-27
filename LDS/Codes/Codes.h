#pragma once
// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
//��ı�׼�������� DLL �е������ļ��������������϶���� CODES_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
//�κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ 
// CODES_API ������Ϊ�ǴӴ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
	char idKeyStr[64];	//��ʶ�ַ���
	char name[96];
	char ciValType;	//0.bool;1.long;2.double;3.���ַ���
	union{
		bool bVal;
		long liVal;
		double dfVal;
		char szStrBuff[101];
	};
};
union LOAD_CODES{
	//��λ��������ʾ�淶ϵ�к�, ��λ��������ʾ�淶�汾��, 0ֵĬ�ϱ�ʾ�������涨��
	static const int GB50135_2006	= 0x000001;	//���ʽṹ��ƹ淶2006��
	static const int ASCE10_97 = 0x000002;	//����ASCE10-97
	static const int DLT_2002 = 0x000003;	//���ڵ��������ṹ��Ƽ����涨 2002��
	static const int DLT_2012 = 0x010003;	//���ڵ��������ṹ��Ƽ����涨 2012��
	static const int DLT_2018 = 0x010004;	//���ڼܿ������·���ع淶DLT5551-2018. 2018��
public:
	struct{
		short family;	//�淶ϵ��
		short version;	//�淶�汾��
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
	static const BYTE XUAN_CHUI = 0;//��������;
	static const BYTE NAI_ZHANG = 1;//���Ź���
	//static ILoadCalCode* CODE_DLT_2002();
	static ILoadCalCode* CODE_DLT_2012();
	static ILoadCalCode* CODE_DLT_2018();
	static ILoadCalCode* CODE_ASCE10_97();
	static ILoadCalCode* FromCodeSerial(int codeSerial);
	static bool GetSupportCodesStr(char* codesStr,int maxStrBufLength);
	long get_liCodeSerial() { return CodeSerial(); }
	__declspec(property(get=get_liCodeSerial)) long liCodeSerial;
	CODE_PROPITEM* GetCodeItem(const char* keyidstr);
	//��������ID�õ�����ֵ�ַ���
	virtual int GetItemValueStr(long idItemProp,char* valueStr);
	//virtual int InitDefaultPropValue()=0;
	virtual long GetPropId(const char* itemKeyStr);
	//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
	virtual double GetItemValue(long idItemProp)=0;
	virtual double GetItemValue(const char* szItemKeyStr);
	virtual double GetSetupAttachWeightLoad(long volt_grade=500,char ciWireType='E',bool bNaiZhang=true);
	virtual double FindPercentInHeavyIceArea(long volt_grade=500,bool bNaiZhang=true);
	//�������ڲ����ȸ�������µĲ�ƽ������ȡֵϵ��
	virtual double FindUnbalanceIceCoef(double ice_thick,char cWireType='E',long volt_grade=500,bool bNaiZhang=true);
	virtual double FindBreakWireCoefLoadY(
		long wVoltGrade=500,long wFenLieNum=1,	//��ѹ�ȼ�,������
		char cWireFuncType='C',	//'E':����;'C':����;'O':����
		double ice_thick=10,	//�������,mm
		char ciHangWireStyle=1,	//���߷�ʽ
		char ciLineRoadType=0);	//��·����,ƽ����·��������·
	virtual double Find_MuZ(double high,char cLandEnv);
	virtual bool GetWindLoadAllocCoef(double *Xcoef_inWx,double *Xcoef_inWy,double *Ycoef_inWx,double *Ycoef_inWy,long azimuth,long iStyle);
	virtual bool GetWindLoadBodyAllocCoef(double *Xcoef_inWx,double *Xcoef_inWy,double *Ycoef_inWx,double *Ycoef_inWy,long azimuth);
	virtual bool GetWindLoadArmAllocCoef(double *Xcoef_inWsc,double *Ycoef_inWsc,long azimuth,long iStyle);
	virtual double CalMusOfTube(double dfMuzW0D_2);	//dfMuzW0D_2=��z*Wo*d^2
public:
	//���ع淶�ı�ʶϵ�к�
	virtual int CodeSerial()=0;
	//��ȡ�淶����
	virtual char* CodeName()=0;
};
class LOADCOAD_DL_T_2012 : public ILoadCalCode{
	int InitPropHashtable();
public:
	LOADCOAD_DL_T_2012();
	//���ع淶�ı�ʶϵ�к�
	virtual int CodeSerial(){return LOAD_CODES::DLT_2012;}
	//��ȡ�淶����
	virtual char* CodeName();
	//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
	virtual double GetItemValue(long idItemProp);
};
class LOADCOAD_ASCE10_97 : public ILoadCalCode{
	int InitPropHashtable();
public:
	LOADCOAD_ASCE10_97();
	//���ع淶�ı�ʶϵ�к�
	virtual int CodeSerial(){return LOAD_CODES::ASCE10_97;}
	//��ȡ�淶����
	virtual char* CodeName();
	//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
	virtual double GetItemValue(long idItemProp);
};
class LOADCOAD_DL_T_2018 : public ILoadCalCode {
	int InitPropHashtable();
public:
	LOADCOAD_DL_T_2018();
	//���ع淶�ı�ʶϵ�к�
	virtual int CodeSerial() { return LOAD_CODES::DLT_2018; }
	//��ȡ�淶����
	virtual char* CodeName();
	//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
	virtual double GetItemValue(long idItemProp);
	//��ѯ��ѹ�߶ȱ仯ϵ��
	virtual double Find_MuZ(double high,char cLandEnv);
};

union CODES{
	//��λ��������ʾ�淶ϵ�к�, ��λ��������ʾ�淶�汾��, 0ֵĬ�ϱ�ʾ�������涨��
	static const int GB50135_2006	= 0x000001;	//���ʽṹ��ƹ淶2006��
	static const int ASCE10_97		= 0x000002;	//����ASCE10-97
	static const int DLT_2002		= 0x000003;	//���ڵ��������ṹ��Ƽ����涨 2002��
	static const int DLT_2012		= 0x010003;	//���ڵ��������ṹ��Ƽ����涨 2012��
	//��Ԫ���乹����Ƽ����涨
	static const int GB50017_2003	= 0x000004;	//�ֽṹ�淶��2003��
	static const int DLT5254_2010	= 0x000005;	//���ڸֹ�����Ƽ����涨 2010��
	static const int DLT5130_2001	= 0x000006;	//�ֹܸ���Ƽ����涨2001��
	static const int CECS236_2008	= 0x000007;	//�ֽṹ����ͨ����������̡�2008��
public:
	struct{
		short family;	//�淶ϵ��
		short version;	//�淶�汾��
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
	static const BYTE SECT_ANGLE = 1;	//�Ǹ�
	static const BYTE SECT_TUBE  = 2;	//Բ��
	static const BYTE SECT_POLY  = 3;	//����ι�
	static const BYTE SECT_SLOT  = 4;	//�۸�
public:
	double M[3],P[3];	//���˵�����Ϊ����,[0]x����;[1]y����;[2]z����;��λN*mm,N
	double I,W;		//x-x��y-y���Ծ�(mm4)�Ϳ���ģ��(mm3),����ΪIx=Iy,Wx=Wy
	double Q_It;	//����/I*t,ȷ�����������Ӧ������,1/mm2
	double C_J;		//�뾶/�����Ծ�,ȷ�����Ťת��Ӧ������,1/mm3
	double f,fy;	//�������ǿ�ȡ�����ǿ��,MPa
	double lamda;	//�˼������峤ϸ��
	double Ag,An;	//ë��������������mm2
	BYTE cSectionType;
	union{
		struct{
			double wide,thick;	//�Ǹ�֫��֫��(mm)
		}angle;
		struct{
			double D,thick;		//Բ���⾶���ں�(mm)
		}tube;
		struct{
			UINT slices;		//����ι�ʱ�ı���
			double BR;			//��Ч�����뾶,mm
			double D0,thick;	//�ߵ��߾��뼰�ں�(mm)
		}poly;
		struct{
			double wide,height,thick;	//�۸�֫�����߼��ں�(mm)
		}slot;
	}para;
public:	//���������������Ϣ
	double fsb;		//����ֲ��ȶ�����ǿ�����ֵMPa,
	double fsc;		//��ѹ�ֲ��ȶ�����ǿ�����ֵMPa,
	double fv;		//�ܼ�ǿ������ֵMPa,
	double sigemaNt;	//����������Ӧ��MPa(��N/mm2)
	double sigemaNc;	//����ѹ����Ӧ��MPa(��N/mm2)
	double sigemaBend;	//����ǿ��MPa
	double taoShear;	//����Ӧ��M=��������Ӧ��+Ťת��Ӧ����MPa
	double sigemaComplex;	//����Ӧ��
public:
	static const BYTE COND_TENSION = 1;	//��������
	static const BYTE COND_COMPBEND= 2;	//ѹ��ֲ��ȶ�ǿ�����
	static const BYTE COND_BENDING = 3;	//����ǿ��
	static const BYTE COND_SHEAR   = 4;	//����Ӧ��
	static const BYTE COND_COMPLEX = 5;	//����Ӧ��ǿ�����
	BEAM_SECTION();
	double Check(BYTE* cCtrlCond,int exec_code);	//���ؿ��������µĹ�Чϵ��,cCtrlCond���ؿ�����������Σ������
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
	static double modulusE;		//���ϵ���ģ��,MPa
	static ILog2File* SetHostErrorLogger(ILog2File* pErrLogger);
	static IMemberDesignCode* CODE_DLT_2002();
	static IMemberDesignCode* GB50135_2006();
	static IMemberDesignCode* CODE_DLT_2012();
	static IMemberDesignCode* CODE_ASCE10_97();
	static IMemberDesignCode* FromCodeSerial(int codeSerial);
	static bool GetSupportCodesStr(char* codesStr,int maxStrBufLength);
	//��������ID�õ�����ֵ�ַ���
	virtual int GetItemValueStr(long idItemProp,char* valueStr);
	long GetPostRodKlamdaLimit(bool bTube=false);
	long GetPrimaryRodKlamdaLimit(bool bTube=false);
	long GetBraceRodKlamdaLimit(bool bTube=false);
	long GetTensionRodKlamdaLimit(bool bTube=false);
	double GetBraceCoefOfRedundantToPost();		//���㸨�Ķ����ĵ�֧����ռ��ϵ��
	double GetBraceCoefOfRedundantToPrimary();	//���㸨�Ķ�б�ĵ�֧����ռ��ϵ��
	double GetBraceCoefOfPrimaryToPost();		//����б�Ķ����ĵ�֧����ռ��ϵ��
public:
	struct BevelRodLamdaPara{
		char cType;	//'C'ͬʱ��ѹ��'c'��ʾһ��һѹ(��N0<0.2N)����ͬʱ��ѹ����, ����ֵ��һ��һѹ����
		double K;	//��ϸ������ϵ��
		BevelRodLamdaPara(char type=0,double k=1.0){cType=type;K=k;}
	};
public:
	//���ع淶�ı�ʶϵ�к�
	virtual int CodeSerial()=0;
	//��ȡ�淶����
	virtual char* CodeName()=0;
	//�Ƿ��ǰ��ƶ�ЧӦ������б�ĵ�Ӱ��
	virtual bool IsIncEiffelEffect(){return false;}
	//ASCE10-97�淶�����ǿ��ȡ����ǿ�ȵ�90%
	virtual double GetSteelDesignStrenth(double fy,double f){return f;};
	//virtual int InitDefaultPropValue()=0;
	virtual long GetPropId(char* itemKeyStr);
	//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
	virtual double GetItemValue(long idItemProp)=0;
	//����Ǹֵ�Ԫ����˨��������Ҫ������Ϣ��֫��t����˨ֱ��d0,�����ľ�g1,X֫�ľ��gx,Y֫�ľ��gy,��׾���S,���÷�ʽ(1��2��3)
	virtual double CalElemHoleOutNum(int style,double t,double d0,double d1,double gx=0,double gy=0,double S=0);
	//������ѹ�ĳ�ϸ��ʱ�Ƿ��ǳ�ϸ������ϵ��
	virtual bool IsIncLamdaK(){return true;}
	//��ͨ�����ļ����ĸ˼���������ϸ��,cRodType,'P'��������(Primary)��'B':�����ģ�������(Brace);endType���˶�״���������DL/T 5154
	virtual double CalRodKLamda(double lamda,char cRodType,
		int start_joint_type,int end_joint_type,int start_force_type,int end_force_type)=0;
	//ѹ���ȶ�ǿ���ۼ�ϵ����������Ϣ��b:֫������������,t��֫��;f:��ƻ�����ǿ��,bending:�Ƿ������
	virtual double CompReduceCoef(double b,double t,double f,double fy=0,double Klamda=0,bool isTube=false,bool bending=false)=0;
	//KΪ������ѹ�˼���ϸ������ϵ����lamdaΪ��ϸ�ȣ�fy��������ǿ��
	virtual double CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t=10)=0;
	//���ݽ���б��������������ʽ������б�ĳ�ϸ�ȼ������,N0Ϊ����ֵ��С�����Ĵ���ֵ,style����DL/T-2012
	virtual BevelRodLamdaPara IntersBevelRodLamdaCal(double N,double N0,double ratioL2toL3=0);
	//�������
	virtual bool IsSupportBlockShearCheck(){return false;}
	virtual double BlockShearCheck(double N,double endSpace,double midSpace,double c,double t,double f,double fv);
	//���乹�����㣬����ֵΪ��Чϵ����fΪ�˼����ϵ�ǿ�����ֵ,Do>0&&t>0ʱ��ʾΪ�ֹܹ������⾶�ͱں�
	virtual double BendingRodCheck(double Mx,double Wx,double My,double Wy,double f,double Do=0,double t=0,bool isTube=false);
	//ѹ�乹�����㣬����ֵΪ��Чϵ����������λ��N(N),A(mm^2),M(N*m),W(cm^3),f(MPa)��
	virtual double BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f)=0;
	//���乹�����㣬����ֵΪ��Чϵ����
	virtual double BendingTensionRodCheck(double m,double N,double An,double M,double W,double f);
};

class CODES_API DL_T_2002 : public IMemberDesignCode{
	int InitPropHashtable();
public:
	DL_T_2002(void);
	//���ع淶�ı�ʶϵ�к�
	virtual int CodeSerial(){return CODES::DLT_2002;}
	//��ȡ�淶����
	virtual char* CodeName();
	//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
	virtual double GetItemValue(long idItemProp);
	//��ͨ�����ļ����ĸ˼���������ϸ��,cRodType,'P'��������(Primary)��'B':�����ģ�������(Brace);endType���˶�״���������DL/T 5154
	double CalRodKLamda(double lamda,char cRodType,
		int start_joint_type,int end_joint_type,int start_force_type,int end_force_type);
	//ѹ���ȶ�ǿ���ۼ�ϵ����������Ϣ��b:֫������������,t��֫��;f:���ǿ��,bending:�Ƿ������
	double CompReduceCoef(double b,double t,double f,double fy=0,double Klamda=0,bool isTube=false,bool bending=false);
	//KΪ������ѹ�˼���ϸ������ϵ����lamdaΪ��ϸ��
	double CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t=10);
	//���ݽ���б��������������ʽ������б�ĳ�ϸ�ȼ������,N0Ϊ����ֵ��С�����Ĵ���ֵ
	BevelRodLamdaPara IntersBevelRodLamdaCal(double N,double N0,double ratioL2toL3=0);
	//�������(2002�漼���涨�����ǿ������˹�Чϵ������0)
	virtual double BlockShearCheck(double N,double endSpace,double midSpace,double c,double t,double f,double fv){return 0;}
	//���乹�����㣬����ֵΪ��Чϵ����fΪ�˼����ϵ�ǿ�����ֵ,Do>0&&t>0ʱ��ʾΪ�ֹܹ������⾶�ͱں�
	double BendingRodCheck(double Mx,double Wx,double My,double Wy,double f,double Do=0,double t=0);
	//ѹ�乹�����㣬����ֵΪ��Чϵ����
	double BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f);
};
class CODES_API GB50135_2006 : public DL_T_2002
{
	int InitPropHashtable();
public:
	GB50135_2006();
	//���ع淶�ı�ʶϵ�к�
	virtual int CodeSerial(){return CODES::GB50135_2006;}
	//��ȡ�淶����
	virtual char* CodeName();
	//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
	virtual double GetItemValue(long idItemProp);
	//������ѹ�ĳ�ϸ��ʱ�Ƿ��ǳ�ϸ������ϵ��
	virtual bool IsIncLamdaK(){return false;}
};
class CODES_API DL_T_2012 : public IMemberDesignCode{
	int InitPropHashtable();
public:
	DL_T_2012(void);
	//���ع淶�ı�ʶϵ�к�
	virtual int CodeSerial(){return CODES::DLT_2012;}
	//��ȡ�淶����
	virtual char* CodeName();
	//�Ƿ��ǰ��ƶ�ЧӦ������б�ĵ�Ӱ��
	virtual bool IsIncEiffelEffect(){return true;}
	//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
	virtual double GetItemValue(long idItemProp);
	//��ͨ�����ļ����ĸ˼���������ϸ��,cRodType,'P'��������(Primary)��'B':�����ģ�������(Brace);endType���˶�״���������DL/T 5154
	double CalRodKLamda(double lamda,char cRodType,
		int start_joint_type,int end_joint_type,int start_force_type,int end_force_type);
	//ѹ���ȶ�ǿ���ۼ�ϵ����������Ϣ��b:֫������������,t��֫��;f:���ǿ��,bending:�Ƿ������
	double CompReduceCoef(double b,double t,double f,double fy=0,double Klamda=0,bool isTube=false,bool bending=false);
	//KΪ������ѹ�˼���ϸ������ϵ����lamdaΪ��ϸ��
	double CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t=10);
	//�Ƿ�֧�ֿ������
	bool IsSupportBlockShearCheck(){return true;}
	//���乹�����㣬����ֵΪ��Чϵ����fΪ�˼����ϵ�ǿ�����ֵ,Do>0&&t>0ʱ��ʾΪ�ֹܹ������⾶�ͱں�
	double BendingRodCheck(double Mx,double Wx,double My,double Wy,double f,double Do=0,double t=0);
	//ѹ�乹�����㣬����ֵΪ��Чϵ����
	double BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f);
};

class CODES_API ASCE10_97 : public IMemberDesignCode{
	int InitPropHashtable();
public:
	ASCE10_97(void);
	//���ع淶�ı�ʶϵ�к�
	virtual int CodeSerial(){return CODES::ASCE10_97;}
	//��ȡ�淶����
	virtual char* CodeName();
	//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
	virtual double GetItemValue(long idItemProp);
	//ASCE10-97�淶�����ǿ��ȡ����ǿ�ȵ�90%
	double GetSteelDesignStrenth(double fy,double f){return 0.9*fy;};
	//��ͨ�����ļ����ĸ˼���������ϸ��,cRodType,'P'��������(Primary)��'B':�����ģ�������(Brace);endType���˶�״���������DL/T 5154
	double CalRodKLamda(double lamda,char cRodType,
		int start_joint_type,int end_joint_type,int start_force_type,int end_force_type);
	//ѹ���ȶ�ǿ���ۼ�ϵ����������Ϣ��b:֫������������,t��֫��;f:���ǿ��,bending:�Ƿ������
	double CompReduceCoef(double b,double t,double f,double fy=0,double Klamda=0,bool isTube=false,bool bending=false);
	//KΪ������ѹ�˼���ϸ������ϵ����lamdaΪ��ϸ��
	double CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t=10);
	//�Ƿ�֧�ֿ������
	bool IsSupportBlockShearCheck(){return true;}
	//���乹�����㣬����ֵΪ��Чϵ����fΪ�˼����ϵ�ǿ�����ֵ,Do>0&&t>0ʱ��ʾΪ�ֹܹ������⾶�ͱں�
	double BendingRodCheck(double Mx,double Wx,double My,double Wy,double f,double Do=0,double t=0);
	//ѹ�乹�����㣬����ֵΪ��Чϵ����
	double BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f);
};

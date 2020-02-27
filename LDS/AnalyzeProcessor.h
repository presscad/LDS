// AnalyzeProcessor.h: interface for the CAnalyzeProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYZEPROCESSOR_H__03EC360F_FE92_404A_848E_6AC3EFCC5269__INCLUDED_)
#define AFX_ANALYZEPROCESSOR_H__03EC360F_FE92_404A_848E_6AC3EFCC5269__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Tower.h"
#include "XhCharString.h"

#if defined(__LDS_)||defined(__TSA_)
#include "Resolver.h"
struct ELEMFORCE_ENV{
	bool bEffectN;
	double N;	//����������ʾ����ʱ��λ,N;��ʾ���ʱ��λ,N*mm
	bool bothComp;	// true:�����ͬʱ��ѹ����;false:�ǽ���Ļ򽻲��һ��һѹ��������
	//THANDLE hWorkStatus;//�������
	SUBID hWorkStatus;
	THANDLE hModule;	//ģ�;��
	long leg_no[4];		//�����޽��Ⱥ�
	ELEMFORCE_ENV(){bEffectN=bothComp=false;N=0;}
};
struct ELEMCOMPSTABLE_ENV{
	bool bEffectN;
	double N;		// (N)
	bool bothComp;	// true:�����ͬʱ��ѹ����;false:�ǽ���Ļ򽻲��һ��һѹ��������
	double sigema;	// N/(A*��),(MPa)
	//���в�����ͬʱ��ѹ�ĸ˼���Ԫ����Ϣ��һ��һѹ�洢
	double Afai_bothcomp,Afai_tenscomp;//A*��
	double fai_bothcomp,fai_tenscomp;	//ͬʱ��ѹ��һ��һѹʱ��ѹ��ϵ��
	double L_bothcomp,Lo_r_bothcomp;	//ͬʱ��ѹ���ƹ����µļ��㳤�ȣ������㳤������ϵ��������ϸ�ȣ�����ϸ������ϵ����
	double muCoefL_bothcomp;			//ͬʱ��ѹ���ƹ����µļ��㳤������ϵ��
	BYTE lamdatype_bothcomp,lamdatype_tenscomp;	// 0:lamdaRyo����;1:lamdaRx����;2:lamdaRx2����
	SUBID hWorkStatus;//�������
	THANDLE hModule;	//ģ�;��
	long leg_no[4];		//�����޽��Ⱥ�
	ELEMCOMPSTABLE_ENV(){bEffectN=false;}
};
struct LAMDA_STABLE_INFO{
	CLDSNode *pStart,*pEnd;	//���㳤�ȵ���ʼ����ֹ�ڵ�
	double L,R,Lo_r;		//���㳤��,��ת�뾶,ʵ�ʳ�ϸ��
	//���㳤�Ȼ�׼����ϵ������ֵ̬��L���Ѽ����ϵ��ֵ���ѿ����Ȳ�б�ĳ�ϸ�ȷŴ�ϵ��������б��֮�������ϵ��ֵ)
	double muCoefBasic;		//��ϵ����Ϊ��̬����braceStrʱ�ã�L��Lo_r���Ѽ����ϵ����,��ͬ��SMC_ITEM.Lo_CALINFO.coef
	double muCoefLive;		//����б�İ�ͬʱ��ѹ����ʱ�ļ��㳤�ȶ�̬(ʵʱ)����ϵ��(L��Lo_r��δ�����ϵ��Ӱ�죬�붯̬���룩
	bool bCalByElemLen;		//pStart=NULL,pEnd=NULLʱ���Ƿ��ʾ����Ԫ�������㳤�Ȼ��ǲ����Ǹü��㳤��
	bool bothCompLive;		//True:��ǰ��������Ϊͬʱ��ѹ;False:��ǰ��������Ϊ��ͬʱ��ѹ�������
	double coefFai;			//ѹ��ϵ�������ڼ���ѹ��״̬�µ����Ӧ��
	double coefAreaFai;		//ѹ��ϵ��������˻������ڼ���ѹ��״̬�µ����Ӧ��
	CXhChar50 braceStr;		//���㳤��ȡֵ�����ַ���
	double MuCoef();		//coefMuBasic*coefMuLive;}
	void ClearZero();
	DECLARE_READONLY_PROPERTY(bool,Validate);
	LAMDA_STABLE_INFO();
};
struct ELEMDESIGN_INFO{
	double hole_out;	//������
	double hole_d;		//�׾�
	double w_t;
	double fCoefTensStrengthDecrease,fCoefCompStrengthDecrease;
	double fCoefStablity;	//������ѹ�����ȶ�ϵ��
	double fCoefEffic;		//���ƹ��������ָ��ռָ�������޵ı���ϵ�����޿��ƹ���������Ϊ��
	double fCoefOfBearLoad;	//���ƹ����³��ܺ���ռ���ؼ��޵ı���ϵ�����޿��ƹ���������Ϊ��
	int    iDangerousRule;	//Σ�տ��ƹ���
	int iDangerousLoadCase;	//��Σ�յĳ���ָ��
	bool bBlockShearActivated;	//��������ʱ������㱻����
	double mN;		//ѹ���ȶ�ǿ���ۼ�ϵ��
	double workSigema;	//�����㹤��Ӧ��
	CXhChar50 LoExpression,lamdaExpression;		//��ϸ�ȼ�����ʽ
	BOOL bStartRotRest,bEndRotRest;
	BYTE lamdaSelType;	// 0:lamdaRyo����;1:lamdaRx����;2:lamdaRx2����
	char cLamdaTypeR;	//0:Rx;1:Ry0
	double lamdaL,lamdaR,Lo_r;		//���㳤��,��ת�뾶,ʵ�ʳ�ϸ��
	double UltimateLo_r;	//�������ϸ��
};
#include "MemberProperty.h"
class CElement
{
	f3dPoint lenVec,lenStdVec;			//��λ���ĵ�Ԫ���ȷ���ʸ��
	double length;				//��Ԫ����mm����ʱ��¼Ϊ�˽����Ż�
	CProcessBodyLoad bodyLoad;	//�������������ڵĵ�Ԫ�����
public:
	static double COMPRESSION_ZERO_FORCE;	//��Сѹ��(N)�����ڴ�ֵ����Ϊѹ��
	static double DIFFERENCE_ZERO_FORCE;	//��С������(N)�����ڴ�ֵ����Ϊ�����в���
	static double DIFFERENCE_ZERO_STRESS;	//��СӦ����(MPa)�����ڴ�ֵ����Ϊ�����в���
	CFGWORD cfgword;
	CLDSNode *pStart,*pEnd;
	BYTE ciFreedomStart,ciFreedomEnd;	//��ʱ�洢ʼĩ��ʵ������Ԫ����ʱ�����ɶȡ�wjh-2017.7.14
	CLDSLinePart *pOrgPole;
	struct RELATIVE_ELEM
	{
		long idRefElem;		//����б�Ŀ���ͬʱ��ѹ���յ�Ԫ
		long idRefElem2;	//����ͬ�˽ڵ�ͬʱ��ѹ���յ�Ԫ
		bool bFatherElem;	//�˵�Ԫ�Ƿ�Ϊ�Գƹ�����Ԫ�еĸ���Ԫ�����ڵ���ۺϽ�С�ĵ�Ԫ
		long idElemMirX,idElemMirY,idElemMirZ;
	}relaElem;
	int    BearTens1Comp2;	//1:�����˳�ѹ;2:��ѹ�˳���;����Ϊ�����ˡ�1��2Ϊ��ˣ��������Ϊ���
private:
	struct SIZE_PARA_TYPE{
		bool m_bOutdated;	//�Ƿ��ѹ���
		union PARA_TYPE{
			JG_PARA_TYPE angle;
			TUBE_PARA_TYPE tube;
			BIAN_PARA_TYPE flat;
			SLOT_PARA_TYPE slot;
		}para;
	}m_xParaType;
public:
	int m_iSizeIndex;		//���ڰ��չ����ʱ����ȽϹ���С
	PROPERTY(void*,pParaType); 
	virtual GET(pParaType){
		if(m_xParaType.m_bOutdated)
			return NULL;	//����
		else
			return &m_xParaType.para;
	}
	virtual SET(pParaType){
		if(value!=NULL)
		{
			m_xParaType.m_bOutdated=false;
			if(pOrgPole->size_idClassType==CLS_LINEANGLE||pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
				memcpy(&m_xParaType.para,value,sizeof(JG_PARA_TYPE));
			else if(pOrgPole->size_idClassType==CLS_LINETUBE)
				memcpy(&m_xParaType.para,value,sizeof(TUBE_PARA_TYPE));
			else if(pOrgPole->size_idClassType==CLS_LINEFLAT)
				memcpy(&m_xParaType.para,value,sizeof(BIAN_PARA_TYPE));
			else if(pOrgPole->size_idClassType==CLS_LINESLOT)
				memcpy(&m_xParaType.para,value,sizeof(SLOT_PARA_TYPE));
			else
				m_xParaType.m_bOutdated=true;
		}
		else
			m_xParaType.m_bOutdated=true;
	}
	//void* pParaType;
	bool bEffectN;			//NֵΪб(��)�Ŀ��ǵ�֧������ʱ���� wjh-2016.12.14
	double N;				//����(N)
	double T0;				//ê�̶�Ԥ����(N)
	double W0,H0,V0;		//����Ԥ�Ž�����������(N)�����ɶ���Ԥ���������²���������ˮƽ��������ֱ����
	bool bAnchorEnd;		//true:�����ն˶Ե�ê��;false:����ʼ�˶Ե�ê��
	GEPOINT moment,VFS,VFE;	//��Ԫ������(N*mm),����ʼĩ�˽������
	short elem_type;	//0:��ʾ�����˻򾭵���;1:��ʾ������Ԫ
	LAMDA_STABLE_INFO lamdaRyo;//��С�᳤ϸ�ȼ�����Ϣ
	LAMDA_STABLE_INFO lamdaRx;	//��ͨ�Ļ򽻲�б��һ��һѹʱƽ���᳤ϸ�ȼ�����Ϣ
	LAMDA_STABLE_INFO _lamdaRc;	//����б��ͬʱ��ѹʱƽ���᳤ϸ�ȼ�����Ϣ(��ʱ�ķ������õ�) wjh 2013.9.22
	LAMDA_STABLE_INFO LamdaRc();	//����б��ͬʱ��ѹʱƽ���᳤ϸ�ȼ�����Ϣ
	SMC_ITEM ActiveSmcItem();
	struct SMCITEM_GROUP{	//��࿼������ͬʱ��ѹ���ո˼���Ԫ��0.������б�ļ�1-2.ͬ�˽ڵ�ģ�
		bool hasBothComp;	//True:ʵ�ʹ�������ͬʱ��ѹ�������;False:ʵ���������ͬʱ��ѹ�������
		char iActiveItem;//���ռ����ͬʱ��ѹ��������lamdaRcͬ������
		SMC_ITEM smcItems[3];
		LAMDA_STABLE_INFO lamdaItems[3];
		bool ValidateActiveItem();
		SMCITEM_GROUP();
	}smcinfo;
	double m_fStartBraceNCoef;	//������ʼ�˶������ĵ�֧����ϵ��
	double m_fEndBraceNCoef;	//�������ն˶������ĵ�֧����ϵ��
	double m_fWeightRaiseCoef;	//������������ϵ��
	int m_iCtrlRule;	//1.��ϸ��2.����3.��ѹ4.ѹ��5.ͳ��6.��˨
	//long m_idPartClsType;
	//int iSizeIndex;
	BOOL m_bValidStartRestStiff,m_bValidEndRestStiff;	//�Ƿ��г��ܲ���ת��Լ��������
	//BOOL m_bTensAndCompElem,m_bBothCompElem;	//ǰ�߱�ʾ�Գ�һ��һѹ�����߱�ʾ�Գ�ͬʱ��ѹ
	//xx_joint_type��������ԣ���real_xx_joint_type�����ʵ�ʼ���ʱ��֧������
	//0����ȫ�½�JOINT_HINGE��1�����ָս�JOINT_RIGID�����Ƴ�Ťת���ת��Լ����,2���Զ��ж�JOINT_AUTOJUSTIFY
	int start_joint_type,end_joint_type;
	int real_start_joint_type,real_end_joint_type;	//0����ȫ�½�JOINT_HINGE1�����ָս�JOINT_RIGID�����Ƴ�Ťת���ת��Լ����
	int start_force_type,end_force_type;			//�Ǹ���������״��CENTRIC_FORCE,ECCENTRIC_FORCE
	ELEMDESIGN_INFO design_info;
	ELEMFORCE_ENV MaxTension,MaxCompression,MaxMoment;	//MaxMoment����ص�λΪ(N*mm),
	ELEMCOMPSTABLE_ENV MaxCompStable;			//ѹ�ȿ���ʱ�����ѹ�ȼ���Ӧ��
	//���ڸ����ĵ�Ԫ��ʾʼĩ����֧�ŵ������ĵ�Ԫ�����ڼ��㸨������ѹ��
	CElement *pStartBraceForceElem,*pEndBraceForceElem;
	FEM_ELEMENT *pFemElem;
	long id;	//��Ԫ��ʶ����ǰ�����������ɣ�һֱ�ᴩʼ��
	long feature;	//����ĳЩ�㷨����ʱ���ñ������洢��ֻ���ض��㷨���к���
	//double* elemStiffMat;	//double [dimension][dimension]
public:
	CElement();
	CElement& operator=(const CElement& element);
	double Length() const{return length;}
	__declspec(property(get=Length)) double dfLength;	//��Ԫ����
	f3dPoint LenStdVec() const{return lenStdVec;}
	//<-2.0ֱ�߶�������;<0.0||>1.0:�㲻�����ϻ�ֱ����;0.0:����ֱ��ʼ���غ�;<1.0����ֱ����;1.0����ֱ���ն��غ�
	double CheckPoint(const f3dPoint& pick);
	void CalLength();
	double GetThick();
	double GetWidth();
	bool IsExportBothCompLamda(IMemberDesignCode* pDesignCode=NULL);	//�Ƿ��ڱ����������ͬʱ��ѹʱ�ĳ�ϸ��(ֻ��ͬʱ��ѹʱ��ϸ�ȹ���ϵ��<����ϵ��ʱ���)
	double LamdaByType(int lamdaType);
	//pbSpecBothCompStateΪ������������ָ���Ƿ���ͬʱ��ѹͬʱ��ѹ״̬ wjh-2017.2.4
	double CompStableLamda(bool* pbSpecBothCompState = NULL);
	void UpdateLamdaInfo();
	char CalLenType(); 
	char LamdaRType(){return design_info.cLamdaTypeR;}
	double LamdaL(){return design_info.lamdaL;}	//���㳤��
	double LamdaR(){return design_info.lamdaR;}	//��ת�뾶
	double Lamda(){return design_info.Lo_r;}//ʵ�ʳ�ϸ��
	//���Ƴ�CElemInfo::relaElem����ʶ֮���ԭʼ�����Ϣ��ԭʼ��Ƶ�Ԫ��
	void CopyBasicInfoTo(CElemInfo* pElemInfo,bool copyCalLenInfo=true);
	bool IsHasMirInfo();
	//�Ƿ��ǶԳƵ�Ԫ��ʼĩ�ڵ����С�ĵ�Ԫ
	BOOL IsFatherElem(){return pOrgPole->IsFatherObject();}
	//�Ƿ���pElem�жԳƹ���
	BOOL IsMirWithElem(CElement *pElem);
	//�Ƚ�(�Գ�)��Ԫ��ʼĩ�ڵ��
	int CompareElemPointI(CElement *pElem);
	double GetWorkI(int para=0);	//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ���Ծ�(mm4)
	double GetWorkW(int para=0);	//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ����ģ��(mm3)
	double GetWorkR(int para=0);	//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ��ת�뾶(mm)
	double SelfWeight(){return bodyLoad.SelfWeight();}
	CProcessBodyLoad CalBodyLoad(double lowest_z,double ice_density=900);	//
	CProcessBodyLoad BodyLoad() const{return bodyLoad;}	//
};
typedef CElement* ELEM_PTR;

//Ӧ������ʱ�����˴�����ȫ����Ԫ�ڵ㣨һ���������ڵ���ʱ���ϣ�
class CPreProcessor;
class CPostProcessor;
class CAnalyzeProcessor  
{
	CPreProcessor *m_pPreProcessor;
	CPostProcessor *m_pPostProcessor;
	//���㵥Ԫ��̬(ʵʱ)ͬʱ��ѹѹ����Ϣ����Ҫ���ڽ���б�Ķ�̬��ϸ�ȼ�ѹ��ϵ��ʵʱ���¼���
	void CalElemCompStableLiveSMCInfo(CElement *pElem);
public:
	NODESET nodeset;//һ���������ڵ�ȫ������Ԫ��ѧ�����ڵ㼯��,��femNodeList�е�FEM_NODEһһ��Ӧ
	ATOM_LIST<FEM_NODE>femNodeList;		//��nodeset�е�CLDSNodeһһ��Ӧ
	ATOM_LIST<FEM_ELEMENT>femElemList;	//�����������ڵ�һ���������ڵ�ȫ������Ԫ��ѧ������Ԫ����
	CHashList<CElement>hashElems;		//�����ĵ�Ԫ���ڵ�һ���������ڵ�ȫ����Ԫ����
	CHashList<FEM_NODE_PTR>hashFemNodes;
	bool m_bHasCableElem;				//��ǰ�ṹ�����Ƿ������ߵ�Ԫ�������ߵ�Ԫ���ڷ����Է���
public:
	CAnalyzeProcessor();
	virtual ~CAnalyzeProcessor();
	//һ������ģ�͵����й���������̷�����Ϻ����
	void UpdateInfoAtModuleChange(CLDSModule *pModule);
	//һ������ģ�͵ĵ�ǰ����������̷�����Ϻ����
	void UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus);
	//���м�����Ϻ󣬸��µ�Ԫ�����˼���ͷ�����������������
	void UpdateElemRelaRodDesignForce();
	void ApplyBraceForceToElem();
	void SetPreProcessor(CPreProcessor *pPreProcessor){m_pPreProcessor=pPreProcessor;}
	void SetPostProcessor(CPostProcessor *pPostProcessor){m_pPostProcessor=pPostProcessor;}
	int GetForceFaceNum(CLDSNode *pNode,f3dPoint axis_vec,f3dPoint *force_face_norm=FALSE);
	void AnalyzeElemDegree();
	void AnalyzeElemDegree(CBlockResolver* pResolver);
	BOOL CreateAnalyzeFile(char *file_name);
};
extern CAnalyzeProcessor analysis; 
#endif
#endif // !defined(AFX_ANALYZEPROCESSOR_H__03EC360F_FE92_404A_848E_6AC3EFCC5269__INCLUDED_)

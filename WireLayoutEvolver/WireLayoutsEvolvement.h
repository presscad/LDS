#pragma once
#include "Hashtable.h"
#include "XhCharString.h"
#include "Buffer.h"
//#include "Resolver.h"

class CWorkEnvLoad
{
public:
	char name[41];					//��Ӧ��������
	double m_fLoadX;				//�����ṩ������������ܺ�(N)
	double m_fLoadY;				//�����ṩ��������ܺ�(N)
	double m_fLoadZ;				//�����ṩ��ֱ�����ܺ�(N)
	double m_fMinVertSpaceLoadX;	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
	double m_fMinVertSpaceLoadY;	//�����ṩ�������(��С��ֱ����)�ܺ�(N)
	double m_fMinVertSpaceLoadZ;	//�����ṩ��ֱ����(��С��ֱ����)�ܺ�(N)
	double m_fWireLoadX;			//���ߵķ����(N)
	double m_fWireLoadZ;			//���ߵĴ�ֱ����(N)
	double m_fMinVertSpaceWireLoadZ;//��С��ֱ����ʱ���ߵĴ�ֱ����(N)
	double m_fAttachmentLoadX;		//�����������(������Ӱ��)(N)
	double m_fAttachmentLoadZ;		//��������ֱ����(������Ӱ��)(N)
	double m_fSetupGa;				//��װʱ����ĸ��Ӵ�ֱ����(N)
	double huchui;					//m,����
	CWorkEnvLoad();
	void SumCal();	//�����ַֺ��ػ��ܳ��ܺ���
};					//�����ṩ��ԭʼ��������
enum WORKENVLOAD_STATUS{
	WORKENVLOAD_WIND=1,WORKENVLOAD_ICE,WORKENVLOAD_LOWTEMP,WORKENVLOAD_ICEDIFF,
	WORKENVLOAD_ICELESS,WORKENVLOAD_ICEMORE,WORKENVLOAD_DUANXIANRUNNING,WORKENVLOAD_DUANXIANBREAK,
	WORKENVLOAD_SETUPSUSPEND,WORKENVLOAD_SETUPSUSPENDING,WORKENVLOAD_SETUPJINXIAN_OTHER_FINISH,WORKENVLOAD_SETUPJINXIAN_OTHER_EMPTY,
	WORKENVLOAD_ANCHORFINISHED,WORKENVLOAD_SETUPANCHORING,WORKENVLOAD_EARTHQUAKE};

struct XIANLU_STRU
{
	double horizon_space;	//ˮƽ����
	double vertical_space;	//��ֱ����
	double min_vert_space;	//��С��ֱ����
	double horizon_space_allotcoef;	//ˮƽ�������ϵ��
	double vertical_space_allotcoef;//��ֱ�������ϵ��
	double represent_space;	//������
	double HtoL;			//�߲�ϵ��
	double xianlu_angle;	//��λ��(��)
};
struct DIANXIAN_STRU
{
	double m_fDiameter;		// �����⾶,mm
	double m_fArea;			// ���߼�������,mm2
	double m_fUnitWeight;	// ���ߵ�λ������,kg/km
	double m_fForcePermit;	// ���߶������,N
	double m_fSafeCoef;		// ���߰�ȫϵ��
	double m_fMaxUseTension;// �������ʹ������,N
	double m_fElasticCoef;	// ���ߵ���ϵ��,MPa
	double m_fExpansionCoef;// �����¶�������ϵ��,1/1/��C
	char specWireType[50];	// ���ߵ����ʹ���
	char specWire[50];		// ���ߵĹ���ͺ�����
};
class CWireInfo
{
public:
	DIANXIAN_STRU wire;
	XIANLU_STRU xianlu;
	BYTE m_cXianType;	//'E'����Earth Wire,'C'����Conducting Wire,'O'Ϊ���ߵȸ����߹ҵ㣬����Ϊ�ǹ��ߵ�
	BYTE m_cFormula;    //�����������㹫ʽ0��������1б������ 
	BYTE m_nDaoXianFengLieNum;	//���߷�����
	bool m_bNaiZhang;	//true���Ź��ߵ� ,false����ֱ�߹��ߵ�
	int  m_nVoltGrade;	//��ѹ�ȼ�(kv)
	double land_high;	//(m)
	char specAttachment[50];			//��߼���Ե�ӵȸ�������ϸ
	double m_fAttachmentWeight;			//��߼���Ե�ӵȸ���������(kg)
	double m_fAttachmentDiameter;		//��Ե�ӵȸ�������Ȼ��⾶(mm)
	double m_fAttachmentLength;			//��Ե�ӵȸ���������(mm)
	double m_fAchorLineAngle;			//ê�߶Եؼн�
	double m_fTractionLineAngle;		//ǣ���߶Եؼн�
	CWireInfo();
};

struct WEATHER{
	//����Ϊ������¡�ƽ�����¡���硢�������¡�������¡���װ����
	long m_nHighestT,m_nLowestT,m_nAverageT,m_nMaxWindT,m_nMaxIceT,m_nSetupT;
	double m_fMaxIceThick;		//��󸲱����(mm)
	double m_fMaxIceThickCheck;	//���㸲�����(mm)
	double m_fMaxIceWindV;
	double m_fMaxWindV;			//����ٶ�(m/s)
	double m_fMaxWindVCheck;	//�������(m/s)
	double m_fSetupWindV;
	void CopyWeather(WEATHER* pWeather)
	{
		m_fMaxIceThick=pWeather->m_fMaxIceThick;		//��󸲱����(mm)
		m_fMaxIceThickCheck=pWeather->m_fMaxIceThick;	//���㸲�����(mm)
		m_fMaxIceWindV=pWeather->m_fMaxIceWindV;
		m_fMaxWindV=pWeather->m_fMaxWindV;		//����ٶ�(m/s)
		m_fMaxWindVCheck=pWeather->m_fMaxWindV;	//�������(m/s)
		m_fSetupWindV=pWeather->m_fSetupWindV;
		m_nAverageT=pWeather->m_nAverageT;
		m_nHighestT=pWeather->m_nHighestT;
		m_nLowestT=pWeather->m_nLowestT;
		m_nMaxIceT=pWeather->m_nMaxIceT;
		m_nMaxWindT=pWeather->m_nMaxWindT;
		m_nSetupT=pWeather->m_nSetupT;
	}
	void FromBuffer(CBuffer &buffer,long version=NULL);
	void ToBuffer(CBuffer &buffer);	
	//void ToXmlFile(FILE* fp,DWORD schema);
};
class CWireLoadModel
{
	//CXuanLianResolver* m_pResolver;
public:
	WEATHER weather;
	char name[51];	//�������ؼ���ģ������
	long iNo;		//��ʶ���
	CWireInfo wireInfo;
	CHashList<CWorkEnvLoad>hashLoad;	//�����ṩ����
	CWireLoadModel(){memset(name,0,51);iNo=0;}
	//void InitWeather(CTower *pTower);	//Ϊʹ����ͷ�ļ�������������CTower����
	void ResetWorkEnvLoadSet();
	void CopyWireLoadModel(CWireLoadModel *pWireLoadModel);
	void FromBuffer(CBuffer &buffer,int nVersion=0,long doc_type=3);
	void ToBuffer(CBuffer &buffer);
	//void ToXmlFile(FILE *fp,DWORD schema);
	//�жϵ����ߵĿ���������������ʼ������״̬����
	void InitDianXianEquation(
		WEATHER &weather,double gama_g,double gama_ice_w,double gama_max_w,double gama_ice);
	//weather����������
	//cal_style=0��ʹ�������߷��̼���cal_style=1:ʹ��б�����߷��̼���
	void CalWorkEnvLoad(char cLandscape,char cLineRoadType);
	/*
	//���Թ���
	static CStringKeyHashTable<long> propHashtable;
	static CHashTable<DWORD> propStatusHashtable;
	int GetPropValueStr(long id,char *valueStr);				//�õ�����ֵ�ַ���
	static void InitModelPropHashtable();
	static int GetPropStr(long id,char *propStr);		//�õ������ַ���
	static long GetPropID(char* propStr);	//�õ�����ID
	static BOOL IsPropEqual(OBJSET &selectObjs, long idProp);	//�������ֵ�Ƿ����
	static int GetPropStatus(long id);					//�õ�����״̬
	static BOOL SetPropStatus(long id,BOOL bHide);		//��������״̬
	*/
};
//ԭʼ���,ת����Tower.h��.cpp���� wjh-2014.4.28
class WIRENODE_STRU
{
public:
	long hNode;	//��ýڵ��������ʵ��ģ���нڵ�ľ��ֵ
	DWORD idNodeI;	//�ڵ���ģ���еı�ʶ���
	DWORD idPlace;	//�ڵ��ں��ع�������ģ���еĶ�Ӧ�Ĺ�λ��
	double fLoadAllocCoefX,fLoadAllocCoefY,fLoadAllocCoefZ;
	CHashList<long>hashWorkEnv;	//��ֵ��ԭʼ������ʶ�ţ�Ԫ��ֵ����Ӧ�����¶�Ӧ��ԭʼ��������CRawWireLoad��WorkEnv�ı�ʶ��
	WIRENODE_STRU();
};
//����ؽṹ
struct WIND_LOAD
{
	//X/Y/Z����ķ���ػ���ϵ�����Զ���������ʱ�ֱ��ʾ���١���ѹ����ϵ���ͷ����
	double velocity;	//����
	long  azimuth;		//����λ�ǣ��㣩������·����Ϊ׼����Y������
};
//�ڵ㼯�к���,��ת��lds_part.h wjh-2014.4.27
class CExternalNodeLoad
{
public:
	double permanentFz;	//���������׼���������ֵ(N)
	f3dPoint Fxyz;		//���������᷽��ļ��пɱ������ֵ(N)
	CExternalNodeLoad(){permanentFz=0.0;}
};
//ԭʼ����,ת����Tower.h��.cpp���� wjh-2014.4.28
class RAWWORKSTATUS_STRU
{
public:
	CXhChar100 name;
	CXhChar50 lim_str;
	BYTE m_cHoriEarthQuakeAxis;	//ˮƽ��������ʱ���񶯷���ǡ㣬'X'=0��'Y'=90
	CXhChar50 vibration_mode_str;	//���ͷֽⷴӦ�׷�����ʱ��Ҫ���ǵ������б�
	long m_hWorkStatus;		//��Ӧ��CWorkStatus�ľ��ֵ
	BYTE m_cMainLoadType;	//0:���1:����2:����3:����4:��װ5:�����ȸ���6:ˮƽ����7:�������8:��ˮƽ����9:���������10:����
	bool m_bCheckWorkCond;	//���㹤��
	double ice_thick;	//�����������(mm)
	bool m_bMinVertSpace;	//��С��ֱ���๤��
	WIND_LOAD WindLoad;	//�����
	RAWWORKSTATUS_STRU(){memset(this,0,sizeof(RAWWORKSTATUS_STRU));}
};
//���λ,ת����Tower.h��.cpp���� wjh-2014.4.28
class LOADNODEROOM_STRU
{
public:
	char helpString[101];
	long m_iRawWireLoadNo;		//ԭʼ�������ر�ʶ��
	CHashList<WIRENODE_STRU>hashNode;
	CHashList<long>hashWorkEnv;	//��ֵ���ǹ�����ʶ�ţ�Ԫ��ֵ����Ӧ�����¶�Ӧ��ԭʼ��������CRawWireLoad��WorkEnv�ı�ʶ��
	LOADNODEROOM_STRU(){m_iRawWireLoadNo=0;memset(helpString,0,100);}
};
//�������������Ϣ,ת����Tower.h��.cpp���� wjh-2014.4.28
class CWorkStatusGroupInfo
{
public:
	double m_fSafetyCoef;	//�ṹ��Ҫ��ϵ��
	double m_fGamaG;		//����ط���ϵ��
	double m_fGamaQ;		//�ɱ���ط���ϵ��
	double m_fLiveLoadFai;	//��������ϵ��
	double m_fCheckLoadFai;	//����������ϵ��
	double m_fLiveLoadZ;	//�������(N)
	CWorkStatusGroupInfo();
	CHashList<LOADNODEROOM_STRU>hashNodeRoom;
	CHashList<RAWWORKSTATUS_STRU>hashWorkStatus;
	int EnumIdleRawStatusNo();
};
//Դ�ṹ����(������߷����µĽṹ����)������Ӧȡ��RAWWORKSTATUS_STRU
class SRCCASE_STRU
{
public:
	long id;
	//long m_idWorkCase;		//��Ӧ��CWorkCase�ı�ʶֵ
	char name[101];
	//char lim_str[51];
	BYTE m_cHoriEarthQuakeAxis;	//ˮƽ��������ʱ���񶯷���ǡ㣬'X'=0��'Y'=90
	char vibration_mode_str[51];	//���ͷֽⷴӦ�׷�����ʱ��Ҫ���ǵ������б�
	BYTE m_cMainLoadType;	//0:���1:����2:����3:����4:��װ5:�����ȸ���6:ˮƽ����7:�������8:��ˮƽ����9:���������10:����
	bool m_bCheckWorkCond;	//���㹤��
	double m_fIceThick;		//�����������(mm)
	bool m_bMinVertSpace;	//��С��ֱ���๤��
	WIND_LOAD WindLoad;		//�����
	SRCCASE_STRU(){memset(this,0,sizeof(SRCCASE_STRU));}
	double Kesai();		//��������ϵ��
	double GamaQ();		//��������ϵ��
	void FromBuffer(CBuffer &buffer,long version=0);
	void ToBuffer(CBuffer &buffer);
};
//����λ��(��λ��,��λ��),����ȡ��LOADNODEROOM_STRU
class WIREPLACE_STRU
{
public:
	long idPlace;
	long m_iRawWireLoadNo;		//ԭʼ�������ر�ʶ��
	char helpString[101];
	CHashList<long>hashWorkEnv;	//��ֵ���ǹ�����ʶ�ţ�Ԫ��ֵ����Ӧ�����¶�Ӧ��ԭʼ��������CRawWireLoad��WorkEnv�ı�ʶ��
	WIREPLACE_STRU(){idPlace=m_iRawWireLoadNo=0;memset(helpString,0,100);}
	void FromBuffer(CBuffer &buffer,long version=0);
	void ToBuffer(CBuffer &buffer);
};
//���߷���,ת���ԡ�Tower.h(.cpp)�� wjh-2014.4.28
class CWireLayout //: public ILog
{
public:
	int iNo;		//�������
	char name[50];	//��������

	CXhChar100 m_sRelaTemplateName;
	double m_fSafetyCoef;	//�ṹ��Ҫ��ϵ��
	double m_fGamaG;		//����ط���ϵ��
	double m_fGamaQ;		//�ɱ���ط���ϵ��
	double m_fLiveLoadFai;	//��������ϵ��
	double m_fCheckLoadFai;	//����������ϵ��
	double m_fLiveLoadZ;	//�������(N)
	CHashList<SRCCASE_STRU>hashSrcCase;	//hashSrcCaseͬgroupWorkStatus.hashWorkStatus
	CHashList<WIREPLACE_STRU>hashWirePlace;	//hashWirePlaceͬgroupWorkStatus.hashNodeRoom
	CHashList<WIRENODE_STRU> hashWireNode;		//ʵ�ʹ��߽ڵ�,��ֵΪ�ҵ��ʶ
	CMapList<CExternalNodeLoad> hashNodeLoad;	//��ǰ���߷�����ָ����λ��ԭʼ�����µĺ���ֵ����ֵΪ:��λ��,ԭʼ������
	//TODO: �����ᱻ������볹��ȡ��
	CWorkStatusGroupInfo groupWorkStatus;
	CHashList<CWireLoadModel>hashWireLoadModel;
	CWireLayout();
	void EvolveToNewVersion();	//����ȡ��groupWorkStatus���°汾ת��
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void Empty();
};
class CTower;
class CWireLayoutsEvolvement
{
public:
	WEATHER weather;
	ATOM_LIST<CWireLayout> WireLayouts;
	CWireLayoutsEvolvement(void);
	~CWireLayoutsEvolvement(void);
	bool EvolveToWireLoadBuffer();//CTower* pTower);
	void FromBuffer(CBuffer &buffer,long version=0,long doc_type=0);
	void ToBuffer(CBuffer &buffer,long version=0,long doc_type=0);
};
extern CWireLayoutsEvolvement evolve;

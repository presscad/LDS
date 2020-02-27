// PreProcessor.h: interface for the CPreProcessTower class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREPROCESSOR_H__BA395871_CAF0_4D53_9CB8_F7C8AFD49FBE__INCLUDED_)
#define AFX_PREPROCESSOR_H__BA395871_CAF0_4D53_9CB8_F7C8AFD49FBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HashTable.h"
//#include "ToolKits.h"
#include "Tower.h"
#include "ArrayList.h"
#include "ldsptr_list.h"
#if defined(__LDS_)||defined(__TSA_)
#include "AnalyzeProcessor.h"
#include "Codes.h"
#include "Resolver.h"

class WINDSEGLOADCAL_EXPORT;
class CWindSegElemPicker
{
	double minZ,maxZ;		//��ѹ�����������ڷ�ѹ����������ϵ�µ�Z���귶Χ
	bool bHasTubeRod;
	double m_fNetAreaOfAngle;	//��ѹ���ڽǸּ�ʵ�ʵ������,mm2
	double m_fNetAreaOfTube;	//��ѹ���ڸֹܼ�ʵ�ʵ������,mm2
	long m_idWindSeg;			//��ѹ�εı�ʶ���
	bool m_bShiftToSidePlane;	//������������������ڵ㣬�л��²�����㣨��Ʊ������ۼ�����浲�����ʱ��
public:
	GECS ucs;			//��ѹ���������㹤�����������ϵ������ʰȡ�жϵ�Ԫ�Ƿ��ڷ�ѹ����
	GEPOINT geomCenter;	//��������
	double m_fPrjAreaCoef;	//��ѹ�ι�������ķ�ѹͶӰϵ��
	struct WINDSEG_VERTEX {
		short siNodeSerial;	//�������ڷ�ѹ�����е�������ţ��м�ɾ���������Ų���
		GEPOINT xModelPos;	//������ģ������
		GEPOINT xPlanePos;	//������ӭ��������
		GEPOINT xProjPos;	//������ͶӰ������
		WINDSEG_VERTEX() {
			siNodeSerial = 0;
		}
	};
	ARRAY_LIST<WINDSEG_VERTEX>xarrVertices;	//͹����ζ��㹤�����������ϵ���������
	CFGWORD cfgword;	//����ĵ�Ԫ���ϵ�������ĺŷ�Χ��ֻ�������
	CTmaPtrList<CElement,CElement*>elemSet;
	double a, b, b_a;	//��������ӭ�����뱳����֮�����/����ӭ������
	double m_fWrapArea;	//����������ͶӰ�����m^2
	CWindSegElemPicker();
	bool get_blShiftToSidePlane() {return m_bShiftToSidePlane;}
	__declspec(property(get = get_blShiftToSidePlane)) bool blShiftToSidePlane;
	bool InitElemPicker(NODESET &nodeset,f3dPoint prjOrg,UINT idWindSeg,bool bShiftToSidePlane=false);
	void StartElemPicker(ATOM_LIST<CElement> &allElemSet,CLDSModule *pModule=NULL, 
		CHashList<long> *pHashElemBelongWindSeg=NULL, bool blCalByBodySeg = true);
	bool IsIncludeElem(CElement* pElem,CLDSModule *pModule=NULL);
	double CalWindSegAf(double W,double *weighted_coef,WINDSEGLOADCAL_EXPORT *pExport=NULL);
	double MinZ()const{return minZ;}
	double MaxZ()const{return maxZ;}
	double get_dfNetAreaOfAngles() { return m_fNetAreaOfAngle; }
	double get_dfNetAreaOfTubes() { return m_fNetAreaOfTube; }
	__declspec(property(get = get_dfNetAreaOfAngles)) double dfNetAreaOfAngles;
	__declspec(property(get = get_dfNetAreaOfTubes))  double dfNetAreaOfTubes;
#ifdef __ALFA_TEST_VERSION_
public:
	double Test_CalWindSegAf(double W,int iLoadExecuteCode,double *weighted_coef);
#endif
};
struct ELEMLATERAL_STIFF
{
	CElement *pElem;
	double stiff;
};
class CNodeLinkElems
{
public:
	CLDSNode *pNode;
	CFGWORD cfgword;		//�����bSupport3D��supportDirect������ģ�ͷ�Χ
	int supportType;		//<=0:��֧��;1:����֧��;2:ƽ��֧��;>=3:��ά����֧��
	f3dPoint supportDirect;	//ƽ��֧��
	ATOM_LIST<ELEMLATERAL_STIFF>linkElems;
	CNodeLinkElems(){pNode=NULL;supportType=0;}
};
struct AUXELEM_CONST
{
	CElement *pElem;	//�����ĸ��ĵ�Ԫָ��
	double sina;		//��Ԫ���߷�����֧��������ϵ����ļн�����
	double cosa;		//��Ԫ���߷�����֧��������ϵ����ļн�����
	double A;			//��Ԫ�Ľ�������˴�A�����ٲ���Ҫ����Ϊ���ս��brace_coef�������ٵ�
	double k_lcoef;		//��Ԫ������ն�ϵ��k=k_lcoef*A
	double k_xcoef;		//��Ԫ���ھֲ�֧������ϵX����ĸն�ϵ��k_x=k_xcoef*A
	double brace_coef;	//��������ĵ�Ԫ�������ĵ�֧��������ϵ��
};
class CNodeBraceAuxElems
{
public:
	CLDSNode *pNode;
	CFGWORD cfgword;	//��ֵʱ��ʾlinkElems�й�ͬ����ĺţ�����cfgword��Χ�ڲ����ٴ��жϸ��ĵ�Ԫ�Ƿ�Ե�ǰģ����Ч
	int m_iSecondaryElemStiff;	//���κ�һ�����������ĵĸ���,��ͬ�˵㸨��ȴ��������
	ARRAY_LIST<AUXELEM_CONST>linkElemsConst;	//�ýڵ����ӵ����и���
	CNodeBraceAuxElems(){pNode=NULL;m_iSecondaryElemStiff=-1;}
	AUXELEM_CONST* SecondaryElemStiff(){
		if(m_iSecondaryElemStiff<0||m_iSecondaryElemStiff>=linkElemsConst.GetSize())
			return NULL;
		else
			return &linkElemsConst[m_iSecondaryElemStiff];
	}
};
struct WINDLOADNODE{
	int nodeI;
	double Fwx,Fwy;	//kN
	void SetKey(DWORD key){nodeI=key;}
};
class WINDSEGLOADCAL_EXPORT
{
public:
	struct ELEMREC{
		int startPointI,endPointI;
		char cMaterial;
		SIZESPEC size;
		double length,area,muS,efficArea;
		double mass;			//ԭʼ������kg
		double massIncRaiseCoef;//������ϵ����������kg
		bool frontwind,sidewind;
		ELEMREC()
		{
			frontwind=sidewind=false;
			memset(this,0,sizeof(ELEMREC));
			muS=1.3;	//Ĭ�ϽǸ�
		}
		void InitFromFemElement(CElement* pFemElem,bool blCalByBodySeg=true);
	};
	int windSegI;
	BYTE cType;		//��ѹ������0:�˵�����1:�������2:��������
	BYTE cCalStyle;	//0.����ʽ����1.X����ˮƽ�ᵣ2.Y����ˮƽ�ᵣ
	BYTE cMirType;	//�Գ���Ϣ0.�޶Գ�1.���ҶԳ�
	double dfNamedPosH;	//��ѹ�μ���߶�
	double Wo;	//kN/m2
	double A_a,A_b,Af_a,Af_b;	//m2
	double areaOfFrontAngles, areaOfFrontTubes, areaOfSideAngles, areaOfSideTubes;
	struct B_A {
		int a, b;	//ӭ�������Ĵ���ȣ��߶Ƚ�խ�ķ��򣩣���ѹ�ε�ǰ�������ȣ�mm
		B_A() { a = b = 0; }
	}b2aFR, b2aSD;	//b2aFR:����(FRont)b/a������b2aSD:���棨SiDe)b/a����
	double fai_a,fai_b,b_a_a,b_a_b,yita_a,yita_b;
	double muZ,muSa,muSb,betaZ,K,Cqi_a,Cqi_b;
	double Wsa,Wsb,Wsc;	//kN
	double Xcoef_Wsa,Xcoef_Wsb,Xcoef_Wsc;
	double Ycoef_Wsa,Ycoef_Wsb,Ycoef_Wsc;
	double Wx,Wy;
	WINDSEGLOADCAL_EXPORT();
	CHashListEx<WINDLOADNODE>loadNodeList;
	CMapList<ELEMREC> elemrecs;
};
typedef ATOM_LIST<WINDSEGLOADCAL_EXPORT> WINDSEGEXPORT_LIST;

struct INTERVAL_NODE
{
	double posLen;
	CLDSNode *pNode;
	bool bSupport3D;		//��ά����֧��
	f3dPoint supportDirect;	//ƽ��֧��
	INTERVAL_NODE(){memset(this,0,sizeof(INTERVAL_NODE));}
};
typedef INTERVAL_NODE* INTERVAL_NODEPTR;
struct CRodIncIntervals
{
	CHashList<INTERVAL_NODEPTR>hashNodes;	//��ֵ�ǽڵ��
	ATOM_LIST<ELEM_PTR>listElems;
};
struct SUBBLOCK
{
	SUBBLOCK(long key){serial=key;}
	GECS lcs;
	DWORD serial;
	//CFGWORD cfgword;				//���õĺ���ģ�ͣ���Ӧģ�����CLDSModule::iNo)
	ARRAY_LIST<long>elems;			//�ӽṹ��ɵ�Ԫ
	CHashList<CLDSNodePtr> boundary;//�����������ᣩ�ڵ�
};
struct SUBBLOCKREF
{
	DWORD serial;
	GECS acs;
};
class CModuleFrame
{
	long m_iNo;
public:
	CModuleFrame(long key=0){m_iNo=key;}
	ATOM_LIST<SUBBLOCKREF> blkrefs;
	ARRAY_LIST<long> elems;	//�ӽṹ֮��ļܹ���Ԫ��һ��Ϊ�Ȳ������ĵ�Ԫ�����������ġ�б�ģ�
	CLDSModule* pModule;
};
class CTsaTurboBuffer : public CAttachBuffer
{
public:
	CTsaTurboBuffer(long key);
};
class CWindSegDispatcher;
class CPreProcessor
{
	CTower *m_pTower;
	CAnalyzeProcessor *m_pAnalysisProcessor;
	CHashTable<CLDSNode*>hashTowerNodeI;	//ע���ֵΪ�ڵ�Ŷ����Ǿ��
	ATOM_LIST<CWindSegElemPicker>windSegPickerList;
	bool m_bIncEiffelFactorX,m_bIncEiffelFactorY;	//�Ƿ�����б�İ��ƶ�ЧӦ����
	bool LOWEST_ROD_POSZ_INITED;
	double LOWEST_ROD_POSZ;	//������͵�Z����ֵ(m)����Ҫ���ڼ��������������ʱ������ظߣ�ȡ���к��������Ϊ��׼) wjh-2017.3.10
	ARRAY_LIST<double>jointx_z_arr,jointy_z_arr;
	CAttachBuffer* pTurboBuffer;	//
private:
	void VerifyEiffelFactor(CTower* pTower);
	CProcessBodyLoad GetBodyLoad(CLDSNode *pNode,CLDSModule *pActiveModule=NULL);
	CProcessBodyLoad GetStartBodyLoad(CElement *pElem,CLDSModule *pActiveModule=NULL);
	CProcessBodyLoad GetEndBodyLoad(CElement *pElem,CLDSModule *pActiveModule=NULL);
	//���ݶԳ�ԭ����ȡ�˵�Ž�С�ĵ�Ԫ�γɸ��˼�����
	void RetrieveFatherElemHashSet();
	//���ݶԳ�ԭ����ȡ�����뵥Ԫ����Ӧ�ĸ���Ԫ
	CElement* FindFatherElem(CElement* pElem);
	//������֤(��)��Ԫ�Ƿ����㹻֧��
	void ValidateElemSupportStiff();
	//����һ������ڵ���������ڵ����и˼�(�����߽�˼�)��ͶӰ��ѹ�������(m2)
	double SnapNodeFace(NODESET &nodeset,f3dPoint prjOrg,double W,int iLoadExecuteCode,
		double *weighted_coef,double *net_area,double *BtoH);
	//�����ѳ�ʼ���ĳ�ϸ��ԭʼ�����Ϣ������ȡ����Ԫ�����֧�����估֧������
	void SmartRetrieveCalLenInfo();
	//��ʼ������Ԫ�ĳ�ϸ�ȼ�����Ϣ
	void InitElemLamdaInfo(bool loadSrcDesignElemInfo=true);
	//���µ�Ԫ������Ϣ����PreparaTower�е���
	void UpdateElemLengthInfo();
	//���µ�Ԫ�����(���ؼ���������)��Ϣ��ѡ�ķ����仯ʱ����
	void UpdateElemBodyLoadInfo();
	//���㸨��֧��������Ԥ����Ϣ
	void CalAuxElemBraceForcePreInfo();
	//����б��֧��������Ԥ����Ϣ
	void CalPrimaryElemBraceForcePreInfo();
	//������������Ԫ�˽ڵ�����ɶȼ���������
	int GetForceFaceNum(CLDSNode *pNode,f3dPoint axis_vec,f3dPoint *force_face_norm=FALSE);
	void AnalyzeElemDegree();
	//�����ǰ�Ļ�������
	void Empty();
	//���ڶ����߽ڵ��϶�Ӧ������(��V������)������ص�Ч���䵽�ǹ��ߵ���
	void DispatchSwingNodeLoad(CLDSModule *pModule,CWorkStatus *pStatus,bool bApplyStdLoad);
	//ʩ�ӽڵ㼯�к���
	bool ApplyNodeLoad(ATOM_LIST<FEM_NODE> &nodeList,CLDSModule *pModule,CWorkStatus *pStatus,bool bApplyStdLoad);
	//��������ʱ�ĵ�����ؼ���
	bool CalEarthQuakeLoad(CTower *pTower, CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode);
public:
	//�Զ������������ص㼰��ѹ�������ص��ж�Ϊ�����ڵ㣬������CLDSNode::feature=1;��ʱ��ֵ
	static void SmartJustifyNodeForceType(CTower *pTower,NODESET& nodeset);
public:
	struct ELEM_BODYLOAD_ALLOC{
		DWORD idElem;	//��Ӧ
		CElement* pElem;	//��Ӧ��Ԫ
		double fAllocCoef;	//����ϵ��
		ELEM_BODYLOAD_ALLOC(){idElem=0;pElem=NULL;fAllocCoef=0.0;}
		void SetKey(DWORD key){idElem=key;}
	};
	class CNodeBodyLoad{
		DWORD idNode;
	public:
		bool bBraceNode;
		bool hasLegElemLink;
		CLDSNode* pNode;
		CHashListEx<ELEM_BODYLOAD_ALLOC> hashElemAllocs;
		CNodeBodyLoad(){idNode=0;pNode=0;bBraceNode=true;hasLegElemLink=false;}
		void SetKey(DWORD key){idNode=key;}
	};
	struct PREPROCESS_RESULT{
		bool bHasSwingNode;
		CFGWORD cfgwordBody;						//����������CPostProcessReport
		CFGWORD cfgwordLeg;							//����������CPostProcessReport
		NODESET allnodeset;							//��ǰ����ģ�ͷ�Χ�����нڵ㼯��
		LINEPARTSET allrodset;						//��ǰ����ģ�ͷ�Χ�����и˼����ϣ������޶˽ڵ�˼���
		NODESET nodeSet;							//����ģ�ʹ����ڵ㼯��
		ATOM_LIST<CElement>elemList;				//�����������ڵ����е�Ԫ����
		CHashListEx<CNodeBodyLoad> hashNodeBodyLoads;	//����Ԫ������ڽڵ��ϵķ���
		CHashTable<ELEM_PTR>hashElem;
		CHashSet<ELEM_PTR>hashFatherElemPtrs;		//�Գƺϲ���ڵ�Ž�С�ĵ�ԪΪ����Ԫ�����常��ԭ��ͬCPostProcessor
		CMapList<ELEM_PTR>hashElemsMap;				//��ֵΪʼĩ�˽ڵ���
		CHashTable<CProcessBodyLoad>hashElemStartBodyLoad;	//���䵽��Ԫʼ�˵ĵ�Ч����أ���ԪidΪ��ֵ
		CHashTable<CProcessBodyLoad>hashElemEndBodyLoad;	//���䵽��Ԫ�ն˵ĵ�Ч����أ���ԪidΪ��ֵ
		CHashTable<CProcessBodyLoad>hashNodeBodyLoad;	//��Ч���䵽�ڵ��ϵĵ�Ч����أ��ڵ���Ϊ��ֵ
		CHashList<CRodIncIntervals>hashRodIntervals;	//��¼�ɸ˼���ֳ����ĵĵ�Ԫ����
		CHashList<CNodeLinkElems>hashNodeLinks;			//�ڵ����ӵ�Ԫ������ֵΪ�ڵ���
		ATOM_LIST<CNodeBraceAuxElems>listBraceLinks;	//�ڵ��ϸ���֧������
		CHashPtrList<SUBBLOCK>blocks;
		CHashPtrList<CModuleFrame> hashModuleFrames;
		bool InitNodeBodyLoads();
	}result;

	CPreProcessor();
	virtual ~CPreProcessor();
	static IMemberDesignCode* ActiveDesignCode;
	static ILoadCalCode* ActiveLoadCode;
	static void CalElemCompStableInfo(CElement *pElem,IMemberDesignCode* pDesignCode,bool bIncActualCompForce=true);
/*
	�����������
	1���������ܴ����ڵ�
	2��������ָ˼�Ϊ��Ԫ
	3������������º��ؽڵ�ĵ�Ч�����ֵ
	4������������º��ؽڵ�ĵ�Ч�����ֵ
	5������һ����ĺŵı�׼��ѹ�ε����������
	ciApplyLoad=0(false)��ʾ��ͨ���ؼ����龰(����ʩ�Ӻ���)��
	ciApplyLoad=1(true)��ʾ��ͨ�������������龰(��ʩ�Ӻ���)��
	ciApplyLoad=2��ʾģ̬�����龰������ʩ�Ӻ��أ��������ṹ�������������Ķ˽ڵ���Ϣ��
*/
	bool PrepareTower(CTower *pTower, const CFGWORD& cfgwordBody, const CFGWORD& cfgwordLeg, char ciApplyLoad = true, bool loadSrcDesignElemInfo = true);
	bool PreCalFreqNodeAllocAddedStruMass(CLDSModule* pModule);
	bool PrepareTowerSubBlock(CBlockResolver* pResolver,CTower *pTower,const CFGWORD& cfgwordBody,const CFGWORD& cfgwordLeg);
	//��PrepareTower�п��ټ̳й����������ģ�����ȹ̲�����Ľṹ��Ϣ
	bool InheritStableModelInfo(CBuffer& buffer);
	bool InitSourceDesignInfo(CTower *pTower);
	//���ؿ��ǰ��ƶ�ЧӦʱ���¶���
	int GetEiffelSlopes(bool bOnlyValid=true,CWorkStatus* pVerifyStatus=NULL);
	//�趨��������������
	void SetAnalysisProcessor(CAnalyzeProcessor *pProcessor);
	//���ε��û��ڵ�Ԫ���Ϲ�����仯ʱ���ø�����ؼ�����Ϣ
	void UpdateInfoAtElemSizeChange();
	//���ε��û��������鷢���仯ʱ���ø��º�����Ϣ
	void UpdateInfoAtModuleChange(CLDSModule *pModule);
	//���ε��û����ģ��(������)�����仯ʱ���ø�������Ԫ����ģ����Ϣ
	void RetrieveFemAnalInfo(CLDSModule *pModule);
	//���ε��û����ģ��(������)�����仯ʱ���ø��¹������������Ϣ
	void UpdateInfoAtLegChange(CLDSModule *pModule,CBlockResolver* pResolver=NULL);
	//���ε��û򹤿������仯ʱ���ø�����ؼ�����Ϣ,fLoadStepCoef:�������صļ���ϵ������0.7��1.0��1.1
	void UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus,bool bVerifyFoundation,bool bApplyStdLoad,bool bEqCheck=false,int fLoadStepCoef=1.0);
	//��ʼ����ѹ�εĳ��ܷ�ѹ��ͶӰ�����
	void InitWindSegElemPicker(CTower *pTower);
	//�����ѹ�εĳ��ܷ�ѹ���ء�iLoadExecuteCode=0:������Ƽ����涨1:���ʹ淶2:�����ṹ���ع淶
	void CalWindAndAttachLoad(CTower *pTower, CLDSModule *pModule, CWorkStatus *pStatus, //CWindSegDispatcher* pDispatcher,
		bool bForFoundation = false, double magnifyCoef = 1.0, WINDSEGEXPORT_LIST* pExportList = NULL);
	long GetFatherElemId(CElement* pElem);
	//����Ϊ�ɺ����������´���ԱȲ���
/*
#ifdef __ALFA_TEST_VERSION_
private:
	CProcessBodyLoad Test_GetBodyLoad(CLDSNode *pNode,CLDSModule *pActiveModule=NULL);
	CProcessBodyLoad Test_GetStartBodyLoad(CElement *pElem,CLDSModule *pActiveModule=NULL);
	CProcessBodyLoad Test_GetEndBodyLoad(CElement *pElem,CLDSModule *pActiveModule=NULL);
	void Test_InitWindSegElemPicker(CTower *pTower);
	//��ʼ������Ԫ�ĳ�ϸ�ȼ�����Ϣ
	void Test_InitElemLamdaInfo();
	//���㸨��֧��������Ԥ����Ϣ
	void Test_CalAuxElemBraceForcePreInfo();
	//��������ʱ�ĵ�����ؼ���
	bool Test_CalEarthQuakeLoad(CTower *pTower, CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode);
public:
	int Test_EqualResult(CPreProcessor& test_processor,FILE* fp);
	//���ε��û��ڵ�Ԫ���Ϲ�����仯ʱ���ø�����ؼ�����Ϣ
	void Test_UpdateInfoAtElemSizeChange();
	void Test_RetrieveFemAnalInfo(CLDSModule *pModule);
	//���ε��û����ģ���н��ȷ����仯ʱ���ø�����ؼ�����Ϣ
	//void Test_UpdateInfoAtLegChange(CLDSModule *pModule);
	//���ε��û򹤿������仯ʱ���ø�����ؼ�����Ϣ,fLoadStepCoef:�������صļ���ϵ������0.7��1.0��1.1
	void Test_UpdateInfoAtStatusChange(CLDSModule *pModule,CWorkStatus *pStatus,bool bVerifyFoundation,int fLoadStepCoef=1.0);

	bool Test_PrepareTower(CTower *pTower,const CFGWORD& cfgwordBody,const CFGWORD& cfgwordLeg,bool bApplyLoad=true);
	//�����ѹ�εĳ��ܷ�ѹ����
	void Test_CalWindAndAttachLoad(CTower *pTower,CLDSModule *pModule,CWorkStatus *pStatus,int iLoadExecuteCode,
		bool bForFoundation=false,double magnifyCoef=1.0,WINDSEGEXPORT_LIST* pExportList=NULL) ;
#endif
*/
//---------------����ϵ�����㺯��---------------
private:
	//��PrepareTower�е�����ȡ��������ԭ�������޵�Ԫģ�ͣ��Ա����������������ۼ������ѹ�η���ϵ�� wjh-2019.8.30
	bool DiscretizeFemElemModel(CLDSModule *pModule);
	//ͳ�Ƽ������ϵ��ʱ����ķ�ѹ�ε�ӭ���浲�����������ϵ������ѹ�߶ȱ仯ϵ����
	bool CalTowerWindSegBetaZ_PrepareData(CLDSModule* pModule, double dfWindV, CWindSegDispatcher* pDispatcher = NULL);
	bool CalTowerWindSegBetaZ_AllocMass(CLDSModule* pModule, CWindSegDispatcher* pDispatcher = NULL);
	bool CalTowerWindSegBetaZ_Final(CLDSModule *pModule, double dfMaxWindV, double f1, CWindSegDispatcher* pDispatcher);
public:
	bool CalTowerWindSegBetaZ(CTower *pTower, CLDSModule* pModule, double dfMaxWindV, double dfVibraFreq, CWindSegDispatcher* pDispatcher = NULL);
};
extern CPreProcessor preprocessor;
#endif
#endif // !defined(AFX_PREPROCESSOR_H__BA395871_CAF0_4D53_9CB8_F7C8AFD49FBE__INCLUDED_)

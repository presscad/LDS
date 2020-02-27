// ShankDefCls.h: interface for the CShankSlope class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHANKDEFCLS_H__7900E6F9_E966_41BF_8503_BCF774170A85__INCLUDED_)
#define AFX_SHANKDEFCLS_H__7900E6F9_E966_41BF_8503_BCF774170A85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ArrayList.h"
#include "ArrayPtrList.h"
#include "XhCharString.h"
#include "HashTable.h"
#include "ArrayList.h"

//#if defined(__LDS_)||defined(__TSA_)||defined(__TDA_)
class CShankDefine;
class CShankSlope;
class CShankSegment;
class CShankModule;
class CShankLeg;
class CShankSection;
class CShankDefineObject
{
public:
	char cObjType;	//K:CShankSlope;D:CShankSegment;B:CShankModule;L:CShankLeg;J:CShankSection
	CShankDefineObject(){cObjType=0;}
	virtual ~CShankDefineObject(){;}
	CShankSlope* Slope(){return (CShankSlope*)this;}
	CShankSegment* Segment(){return (CShankSegment*)this;}
	CShankModule* Module(){return (CShankModule*)this;}
	CShankLeg* Leg(){return (CShankLeg*)this;}
	CShankSection* Section(){return (CShankSection*)this;}
};
class CShankSection : public CShankDefineObject
{
public:
	CShankSection();
	virtual ~CShankSection(){;}
	void CloneShankSection(CShankSection* pSrcSection);
	CShankDefineObject* m_pParent;	//���ֶΣ�����������ֶ�Ҳ�����ǽ��ȣ�
	int divide_n;		//������ڼ��ڲ��ȷ��ӽڼ���
	CXhChar16 btm_zstr;	//�ײ�Z����λ���ַ���,��ֵ>1��ʾ����,С��1��ʾ�ײ���ռ��ǰ�ֶεĴ��߱���
	char front_layout[51],side_layout[51],hsect_layout[51],v_layout[51];//���沼�ġ����沼�ġ���������沼�ļ�V�沼��
};
class CShankLeg : public CShankDefineObject 
{
	ARRAYPTR_LIST<CShankSection> sectionList;
	friend class CShankModule;
	CShankModule *m_pParentModule;
public:
	CShankLeg();
	virtual ~CShankLeg();
	CFGLEG legword;		//���Ⱥ�(��ĺ�)
	SEGI m_iSeg;				//�ֶκ�
	BOOL m_bDatumLeg;		//�¶ȶ����׼��
	double m_fHeight;		//���ȴ���(mm)
	double m_fFrontWidth,m_fSideWidth;	//���������(mm)
	int divide_n;			//�����漰V��ĵȷ��ӽڼ���
	char front_layout[51],side_layout[51],hsect_layout[51],v_layout[51];//���沼�ġ����沼�ġ���������沼�ļ�V�沼��
	int SectionNum(){return sectionList.GetSize();}
	CShankSection* AppendSection();
	CShankSection* InsertSection(int index=-1);
	int GetSectionIndex(CShankSection* pSection);
	CShankSection* SectionAtIndex(int index);
	bool DeleteSectionAtIndex(int index);
	CShankModule* ParentModule(){return m_pParentModule;}
	//
	void CloneShankLeg(CShankLeg* pSrcLeg);
};
class CShankSegment : public CShankDefineObject 
{
	friend class CShankModule;
	friend class CShankSlope;
	ARRAY_LIST<CShankModule*> modulePtrList;
	ARRAYPTR_LIST<CShankSection> sectionList;
	ARRAYPTR_LIST<CShankSegment> segmentList;	//�����ӶΣ��Ӷ����Ӷμ��ǲ��л�����ϵ��˳���νӹ�ϵ��
	CShankDefine *m_pShankDef;
	double _fHeight;		//�ֶδ���(mm)
public:
	CShankSegment();
	virtual ~CShankSegment();
	int ModuleNum(){return modulePtrList.GetSize();}
	CShankModule* AppendModule();
	CShankModule* ModuleAtIndex(int index);
	bool DeleteModuleAtIndex(int index);
	int SectionNum(){return sectionList.GetSize();}
	CShankSection* AppendSection();
	CShankSection* InsertSection(int index=-1);
	int GetSectionIndex(CShankSection* pSection);
	CShankSection* SectionAtIndex(int index);
	bool DeleteSectionAtIndex(int index);
	//�����Ӷ�
	CShankSegment* AppendJunctionSegment();
	CShankSegment* InsertJunctionSegment(int index=-1);
	int JunctionSegmentNum(){return segmentList.GetSize();}
	CShankSegment* JunctionSegmentAtIndex(int index);
	bool DeleteSegmentAtIndex(int index);
	//
	void CloneShankSegment(CShankSegment* pSrcSegment);
public:
	CShankDefineObject* m_pParent;	//���ֶΣ�����������ֶ�Ҳ�����ǽ��ȣ�
	CFGLEG bodyword;	//�����
	SEGI m_iSeg;				//�ֶκ�
	DECLARE_PROPERTY(double,m_fHeight);		//�ֶδ���(mm)
	int divide_n;		//������ڼ��ڲ��ȷ��ӽڼ���
	char front_layout[51],side_layout[51],hsect_layout[51],v_layout[51];//���沼�ġ����沼�ġ���������沼�ļ�V�沼��
	double GetDatumLegHeight();		//��ȡ�÷ֶ��µĻ�׼���ȣ��ҵ��ĵ�һ�������������ֵ�Ľ��ȣ�����ǰ�����ϿڵĴ���
	double GetLowestLegHeight();	//��ȡ�÷ֶ��½ӵ�����ȣ�����ݻ�����Z�����жϣ�����ǰ�����ϿڵĴ���
	CShankLeg* GetDatumLeg();
	CShankLeg* GetLowestLeg(double* pdfLowestZ=NULL);
	CShankModule *GetSpecModule(char *sModuleName);		//�õ�ָ�����߷���
	double GetSpecModuleLegHeight(char *sModuleName);	//�õ�ָ�����߷���߶�
	double GetSpecLegHeight(CShankLeg *pLeg);	//����ָ�����ȸ߶ȣ�Ҫ��pLeg����ָ��ǰ�¶��е�ĳһ���ȶ���
	int AllocateBodyNo(int iInitLegNo);	//������Ϳɷ�����Ⱥ�,���ط������Ϳ�����߽��Ⱥ�
	CFGLEG AllocateBodyWord();
	bool IsSegParentsHasModule();		//�жϷֶ������ĸ��ڵ����Ƿ���ں���
};
//'B'
class CShankModule : public CShankDefineObject 
{
	friend class CShankSlope;
	friend class CShankSegment;
	CShankDefine *m_pShankDef;
	ARRAYPTR_LIST<CShankSegment> segList;
	ARRAYPTR_LIST<CShankLeg> legList;
public:
	CShankModule();
	virtual ~CShankModule();
	bool IsFirstModule();
	int SegmentNum(){return segList.GetSize();}
	int LegNum(){return legList.GetSize();}
	CShankSegment* AppendSegment();
	CShankSegment* InsertSegment(int index=-1);
	bool DeleteSegmentAtIndex(int index);
	CShankLeg* AppendLeg();
	bool DeleteLegAtIndex(int index);
	CShankSegment* SegmentAtIndex(int index);
	int GetSegmentIndex(CShankSegment* pSeg);
	CShankLeg* LegAtIndex(int index);
	CShankLeg* GetLowestLeg(double* pdfLowestZ=NULL);	//��ȡ�÷ֶ��½ӵ�����ȣ�����ݻ�����Z�����жϣ����¶��Ͽڴ���
	void CloneShankModule(CShankModule* pSrcModule);
public:
	CShankDefineObject* m_pParent;	//���ֶ�(�������¶�Ҳ�����Ƿֶ�)
	CXhChar50 name;
	WORD m_iBodyNo;					//���岿�ֶ�Ӧ�ı����1~24
	CFGLEG legword;					//������ĺ�
	WORD m_arrActiveQuadLegNo[4];	//��ģ�͵�ǰ��һ�������޵ĵ�ǰ���Ⱥ�
	int AllocateBodyNo(int iInitLegNo);	//������Ϳɷ�����Ⱥ�,���ط������Ϳ�����߽��Ⱥ�
	bool AllocateLegWord();
};

class CShankSlope : public CShankDefineObject 
{
	friend class CShankDefine;
	CShankDefine *m_pShankDef;
	ARRAYPTR_LIST<CShankSegment> segList;
	ARRAYPTR_LIST<CShankModule*> modulePtrList;
public:
	CShankSlope();
	virtual ~CShankSlope();
	CShankSegment *AppendSegment();
	CShankSegment* InsertSegment(int index=-1);
	CShankSegment *SegmentAtIndex(int index);
	int GetSegmentIndex(CShankSegment* pSeg);
	bool DeleteSegmentAtIndex(int index);
	int SegmentNum(){return segList.GetNodeNum();}
	//
	int ModuleNum(){return modulePtrList.GetSize();}
	CShankModule* AppendModule();
	CShankModule* ModuleAtIndex(int index);
	bool DeleteModuleAtIndex(int index);
	bool GetDatumLegHeight(double* pdfDatumLegZ);	//��ȡ�÷ֶ��µĻ�׼���ȣ��ҵ��ĵ�һ�������������ֵ�Ľ��ȣ����¶��Ͽڴ���
	bool GetLowestLegHeight(double* pdfLowestLegZ);	//��ȡ�÷ֶ��½ӵ�����ȣ�����ݻ�����Z�����жϣ����¶��Ͽڴ���
	bool GetDatumLegFrontWidth(double* pdfWidth);	//��׼��������������
	bool GetDatumLegSideWidth(double* pdfWidth);	//��׼�Ȳ�����������
	double GetSpecLegHeight(CShankLeg *pLeg);	//����ָ�����ȸ߶ȣ�Ҫ��pLeg����ָ��ǰ�¶��е�ĳһ���ȶ���
	CShankLeg* GetDatumLeg();
	CShankLeg* GetLowestLeg(double* pdfLowestZ=NULL);	//��ȡ�÷ֶ��½ӵ�����ȣ�����ݻ�����Z�����жϣ����¶��Ͽڴ���
	CShankModule *GetSpecModule(char *sModuleName);		//�õ�ָ�����߷���
	double GetSpecModuleLegHeight(char *sModuleName);	//�õ�ָ�����߷���߶�
	//
	void CloneShankSlope(CShankSlope* pSrcSlope);
public:
	char m_cMainRodType;	//'L'�Ǹ�;'T'�ֹ�;
	double m_fTopCoordZ;	//��λ��mm
	double m_fTopFrontWidth,m_fTopSideWidth;	//��λ��mm
	double SlopeBodyHeight();	//���ش��¶α��岿���ܸ߶�(mm)
};

typedef struct tagSECTION_NODEARR
{
	CLDSNodePtr nodePtrArr[8];			//�¶���ʼ�ڵ��µĽڵ�����
	CLDSLinePartPtr linePartPtrArr[8];	//�¶���ʼ�ڵ��µĸ˼�����
	tagSECTION_NODEARR(){memset(this,0,sizeof(tagSECTION_NODEARR));}
}SECTION_NODEARR;
struct SLOPE_POSINFO{
	double coordTopZ,coordBottomZ;
	bool bTopSlope,bBottomSlope;
	CShankLeg *pDatumLeg;				//���һ���¶��еĻ�׼����
	CLDSLinePartPtr legRodPtrArr[8];	//���һ���¶��еĻ�׼���ȵ����ĸ˼�
};
class CShankDefine 
{
	friend class CShankSlope;
	friend class CShankSegment;
	friend class CShankModule;
	ARRAYPTR_LIST<CShankModule> Modules;
	ARRAYPTR_LIST<CShankSlope>Slopes;
public:
	struct DEFINE_MODE {
		BYTE ciInitHeightSerial;
		bool blAppendMode;
		//����ģʽ������ģ����
		double dfDefaultHeightDiff;	//�߲�
		double dfGradientDetaX2H, dfGradientDetaY2H;
		CLDSNodePtr xarrTopNodesPtr[4],xarrBtmNodesPtr[4];			//�¶���ʼ�ڵ��µĽڵ�����
		CLDSLinePartPtr xarrBtmLegRodsPtr[4];	//�¶���ʼ�ڵ��µĸ˼�����
	public:
		//�����������ȶε��¶��Ͽ�λ�ü��ߴ�
		double get_dfTopCoordZ();
		double get_dfTopFrontWidth();
		double get_dfTopSideWidth();
		__declspec(property(get = get_dfTopCoordZ)) double dfTopCoordZ;
		__declspec(property(get = get_dfTopFrontWidth)) double dfTopFrontWidth;
		__declspec(property(get = get_dfTopSideWidth))  double dfTopSideWidth;
		DEFINE_MODE();
	}mode;
	CShankDefine();
	virtual ~CShankDefine();
	CShankSlope *AppendSlope();
	void Empty(){Slopes.Empty();Modules.Empty();}
	CShankSlope *SlopeAtIndex(int index);
	int SlopeNum(){return Slopes.GetSize();}
	bool DeleteSlopeAtIndex(int inedx);
	//
	CShankModule *ModuleAtIndex(int index);
	int ModuleNum(){return Modules.GetSize();}
	bool DeleteModuleAtIndex(int index);
	int MaxLegsPerHeightGroup();	//ÿ�������е������������������ж����ʵĺ���-����ģʽ wjh-2019.8.1
	//
	void CloneShankDef(CShankDefine* pSrcShankDef);
public:
	bool FromString(char* strDef);
	double SlopeTopFrontWidth(int iSlope,CShankLeg *pLeg=NULL);
	double SlopeTopSideWidth(int iSlope,CShankLeg *pLeg=NULL);
	double SlopeTopCoordZ(int iSlope);		//iSlopeΪ0Ϊ�����ı��¶�������
	CFGWORD SlopeTopCfgword(int iSlope);	//iSlopeΪ0Ϊ�����ı��¶�������
	SEGI SlopeTopSegI(int iSlope);	//�¶ζ����ڵ�Ķκ� wht 10-11-16
	//���CShankSlope::SlopeHeight,�˺������Ȳ��¶η��ظ߶�ֵΪ�Ͽ�����׼�����ŵĸ߶�
	//sModuleName==NULL ����Ͽ�����׼�����ŵĸ߶� sModuleName�ǿ�ʱ����Ͽ�����ָ�����߷������ŵĸ߶� wht 10-06-08
	double SlopeHeight(int iSlope,CShankLeg *pLeg=NULL);
	double SlopeBottomFrontWidth(UINT iSlope,CShankLeg *pLeg=NULL);
	double SlopeBottomSideWidth(UINT iSlope,CShankLeg *pLeg=NULL);
	double SlopeBottomCoordZ(int iSlope,CShankLeg *pLeg=NULL);
	bool AllocateCfgWord(BYTE ciInitHeightNumber=1);
	int ValidCheck();
};
class CFrameDefine
{
public:
	struct OPEN_SIZE{
		double m_fVH;	//��ֱ�߶�
		double m_fFW;	//���濪��
		double m_fSW;	//���濪��
		OPEN_SIZE(){m_fVH=m_fFW=m_fSW=0;}
	};
	ARRAY_LIST<OPEN_SIZE> m_xOpenArr;
	int	m_iMainPartType;		//��������0.�ֹ�|1.�Ǹ�
public:
	CFrameDefine(){m_iMainPartType=1;}
	void Empty(){m_xOpenArr.Empty();}
	int GetSizeNum(){return m_xOpenArr.GetSize();}
	void CloneFrameDef(CFrameDefine* pSrcDef)
	{
		Empty();
		m_iMainPartType=pSrcDef->m_iMainPartType;
		m_xOpenArr.SetSize(0,pSrcDef->GetSizeNum());
		for(int i=0;i<pSrcDef->GetSizeNum();i++)
			m_xOpenArr.append(pSrcDef->m_xOpenArr[i]);
	}
}; 
//��XML��ʽ������ģ��ת��Ϊʵ�ʵ�����ģ��
typedef struct tagSKETCH_SIZEDIM
{
	f3dPoint dimStart,dimEnd;
	BYTE cSizeType;	//'X','Y',Z',"L'
	WORD wAlignStyle;
	double fTextHeight;
	int cascade;	//�ֲ��עʱ��ʾ�ֲ��
	f3dPoint align_vec,norm,pos;
	BOOL bLegDim;	//�Ƿ�Ϊ�����ϵı�ע
	CFGWORD cfgword;//ͬLDS��������ĺţ����ڿ��ƶ����ʱ��ע����ʾ
	tagSKETCH_SIZEDIM();
	double Size();
}SKETCH_SIZEDIM;
//
struct HEIGHT_LEGS{
	BYTE idBodyNo;
	CLDSModule* pModule;
	BYTE cnPresetLegCount;
	WORD wnLevelHeightDiff;	//�ڽ��ȸ߶ȼ���,mm
	struct LEG{
		WORD wnHeightDiff;	//����ȼ���,mm
		WORD wiMatchLeg;	//ģ����ƥ��Ľ���
		//���Ȳ�����Ϣ
		WORD wiSectionNum;			//���Ƚڼ���
		CXhChar16 szSpec;			//Vб�Ĺ��
		CXhChar16 szFrontLegLayout;	//��������Ȳ�����ʽ
		CXhChar16 szVPlaneLegLayout;//V�沼����ʽ
		LEG() {
			wiSectionNum = 0; wnHeightDiff = 0; wiMatchLeg = 0;
		}
	}xPresetLegs[24];
	HEIGHT_LEGS();
};
class CShankDefineToTowerModel
{
	static CTower* m_pTower;
	static CShankDefine *m_pDefine;
	static bool AddSlopeToTowerModel(SECTION_NODEARR &slopeStartNodeArr,int nPoly,int iSlope,int nSlopeNum,int idClassType);
	static void AddShankModule(CShankModule *pModuleDef,SECTION_NODEARR startNodeArr,SLOPE_POSINFO& slopePosInfo,
					 SECTION_NODEARR& slopeStartNodeArr,SECTION_NODEARR slopeEndNodeArr,int nPoly,int idLinePartClsType);
	static void AddShankSegment(CShankSegment *pSegment,SECTION_NODEARR& startNodeArr,SLOPE_POSINFO& slopePosInfo,
					 SECTION_NODEARR& slopeStartNodeArr,SECTION_NODEARR slopeEndNodeArr,int nPoly,int idLinePartClsType);
	static void AddShankLeg(CShankLeg *pLeg,SECTION_NODEARR &startNodeArr,
				SECTION_NODEARR& slopeStartNodeArr,SECTION_NODEARR slopeEndNodeArr,int nPoly,int idLinePartClsType);
	static CLDSNode *AddFixCoordZNode(CLDSNode *pAttachStartNode,CLDSNode *pAttachEndNode,double coordZ,BOOL bPriorFromSlopeNode=true,char layer_first_char=0);
public:
	static CLDSNode* AddDivScaleNode(CLDSNode *pAttachStartNode,CLDSNode *pAttachEndNode,double scale);
	static CLDSLinePart* AddLinePart(CLDSNode *pStart,CLDSNode *pEnd,int idLinePartClsType);
	static void SetTowerModel(CTower* pTower) { m_pTower = pTower; }
public:
	static int m_iViewType;
	static ARRAY_LIST<SKETCH_SIZEDIM>dims;
	//Ĭ�ϸ˼�����
	bool UpdateTowerModel(CTower* pTower,CShankDefine *pDefine,int nPoly=4,long idClassType=CLS_LINEANGLE);
	bool BuildLegacyFrameTowerModel(CTower* pTower,CFrameDefine *pDefine,int nPoly=4);
};
//#endif
#endif // !defined(AFX_SHANKDEFCLS_H__7900E6F9_E966_41BF_8503_BCF774170A85__INCLUDED_)

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
	CShankDefineObject* m_pParent;	//父分段（可能是塔身分段也可能是接腿）
	int divide_n;		//正侧面节间内部等分子节间数
	CXhChar16 btm_zstr;	//底部Z坐标位置字符串,其值>1表示垂高,小于1表示底部所占当前分段的垂高比例
	char front_layout[51],side_layout[51],hsect_layout[51],v_layout[51];//正面布材、侧面布材、顶部横隔面布材及V面布材
};
class CShankLeg : public CShankDefineObject 
{
	ARRAYPTR_LIST<CShankSection> sectionList;
	friend class CShankModule;
	CShankModule *m_pParentModule;
public:
	CShankLeg();
	virtual ~CShankLeg();
	CFGLEG legword;		//接腿号(配材号)
	SEGI m_iSeg;				//分段号
	BOOL m_bDatumLeg;		//坡度定义基准腿
	double m_fHeight;		//接腿垂高(mm)
	double m_fFrontWidth,m_fSideWidth;	//正侧面根开(mm)
	int divide_n;			//正侧面及V面的等分子节间数
	char front_layout[51],side_layout[51],hsect_layout[51],v_layout[51];//正面布材、侧面布材、顶部横隔面布材及V面布材
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
	ARRAYPTR_LIST<CShankSegment> segmentList;	//连接子段（子段与子段间是并列互换关系非顺序衔接关系）
	CShankDefine *m_pShankDef;
	double _fHeight;		//分段垂高(mm)
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
	//连接子段
	CShankSegment* AppendJunctionSegment();
	CShankSegment* InsertJunctionSegment(int index=-1);
	int JunctionSegmentNum(){return segmentList.GetSize();}
	CShankSegment* JunctionSegmentAtIndex(int index);
	bool DeleteSegmentAtIndex(int index);
	//
	void CloneShankSegment(CShankSegment* pSrcSegment);
public:
	CShankDefineObject* m_pParent;	//父分段（可能是塔身分段也可能是接腿）
	CFGLEG bodyword;	//本体号
	SEGI m_iSeg;				//分段号
	DECLARE_PROPERTY(double,m_fHeight);		//分段垂高(mm)
	int divide_n;		//正侧面节间内部等分子节间数
	char front_layout[51],side_layout[51],hsect_layout[51],v_layout[51];//正面布材、侧面布材、顶部横隔面布材及V面布材
	double GetDatumLegHeight();		//获取该分段下的基准接腿（找到的第一组有正侧面根开值的接腿）到当前变坡上口的垂高
	double GetLowestLegHeight();	//获取该分段下接的最低腿（令根据基础点Z坐标判断）到当前变坡上口的垂高
	CShankLeg* GetDatumLeg();
	CShankLeg* GetLowestLeg(double* pdfLowestZ=NULL);
	CShankModule *GetSpecModule(char *sModuleName);		//得到指定呼高分组
	double GetSpecModuleLegHeight(char *sModuleName);	//得到指定呼高分组高度
	double GetSpecLegHeight(CShankLeg *pLeg);	//返回指定接腿高度，要求pLeg必须指向当前坡段中的某一接腿对象
	int AllocateBodyNo(int iInitLegNo);	//输入最低可分配接腿号,返回分配后最低可用最高接腿号
	CFGLEG AllocateBodyWord();
	bool IsSegParentsHasModule();		//判断分段所属的父节点中是否存在呼高
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
	CShankLeg* GetLowestLeg(double* pdfLowestZ=NULL);	//获取该分段下接的最低腿（令根据基础点Z坐标判断）到坡段上口垂高
	void CloneShankModule(CShankModule* pSrcModule);
public:
	CShankDefineObject* m_pParent;	//父分段(可能是坡段也可能是分段)
	CXhChar50 name;
	WORD m_iBodyNo;					//本体部分对应的本体号1~24
	CFGLEG legword;					//接腿配材号
	WORD m_arrActiveQuadLegNo[4];	//本模型当前第一～四象限的当前接腿号
	int AllocateBodyNo(int iInitLegNo);	//输入最低可分配接腿号,返回分配后最低可用最高接腿号
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
	bool GetDatumLegHeight(double* pdfDatumLegZ);	//获取该分段下的基准接腿（找到的第一组有正侧面根开值的接腿）到坡段上口垂高
	bool GetLowestLegHeight(double* pdfLowestLegZ);	//获取该分段下接的最低腿（令根据基础点Z坐标判断）到坡段上口垂高
	bool GetDatumLegFrontWidth(double* pdfWidth);	//基准腿正面铁塔根开
	bool GetDatumLegSideWidth(double* pdfWidth);	//基准腿侧面铁塔根开
	double GetSpecLegHeight(CShankLeg *pLeg);	//返回指定接腿高度，要求pLeg必须指向当前坡段中的某一接腿对象
	CShankLeg* GetDatumLeg();
	CShankLeg* GetLowestLeg(double* pdfLowestZ=NULL);	//获取该分段下接的最低腿（令根据基础点Z坐标判断）到坡段上口垂高
	CShankModule *GetSpecModule(char *sModuleName);		//得到指定呼高分组
	double GetSpecModuleLegHeight(char *sModuleName);	//得到指定呼高分组高度
	//
	void CloneShankSlope(CShankSlope* pSrcSlope);
public:
	char m_cMainRodType;	//'L'角钢;'T'钢管;
	double m_fTopCoordZ;	//单位：mm
	double m_fTopFrontWidth,m_fTopSideWidth;	//单位：mm
	double SlopeBodyHeight();	//返回此坡段本体部分总高度(mm)
};

typedef struct tagSECTION_NODEARR
{
	CLDSNodePtr nodePtrArr[8];			//坡段起始节点下的节点数据
	CLDSLinePartPtr linePartPtrArr[8];	//坡段起始节点下的杆件数据
	tagSECTION_NODEARR(){memset(this,0,sizeof(tagSECTION_NODEARR));}
}SECTION_NODEARR;
struct SLOPE_POSINFO{
	double coordTopZ,coordBottomZ;
	bool bTopSlope,bBottomSlope;
	CShankLeg *pDatumLeg;				//最后一个坡段中的基准接腿
	CLDSLinePartPtr legRodPtrArr[8];	//最后一个坡段中的基准接腿的主材杆件
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
		//增量模式下塔身建模参数
		double dfDefaultHeightDiff;	//高差
		double dfGradientDetaX2H, dfGradientDetaY2H;
		CLDSNodePtr xarrTopNodesPtr[4],xarrBtmNodesPtr[4];			//坡段起始节点下的节点数据
		CLDSLinePartPtr xarrBtmLegRodsPtr[4];	//坡段起始节点下的杆件数据
	public:
		//待添加塔身接腿段的坡段上口位置及尺寸
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
	int MaxLegsPerHeightGroup();	//每个呼高中的最多减腿组数，用于判定合适的呼高-减腿模式 wjh-2019.8.1
	//
	void CloneShankDef(CShankDefine* pSrcShankDef);
public:
	bool FromString(char* strDef);
	double SlopeTopFrontWidth(int iSlope,CShankLeg *pLeg=NULL);
	double SlopeTopSideWidth(int iSlope,CShankLeg *pLeg=NULL);
	double SlopeTopCoordZ(int iSlope);		//iSlope为0为基数的变坡段索引号
	CFGWORD SlopeTopCfgword(int iSlope);	//iSlope为0为基数的变坡段索引号
	SEGI SlopeTopSegI(int iSlope);	//坡段顶部节点的段号 wht 10-11-16
	//相比CShankSlope::SlopeHeight,此函数对腿部坡段返回高度值为上口至基准腿塔脚的高度
	//sModuleName==NULL 获得上口至基准腿塔脚的高度 sModuleName非空时获得上开口至指定呼高分组塔脚的高度 wht 10-06-08
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
		double m_fVH;	//垂直高度
		double m_fFW;	//正面开口
		double m_fSW;	//侧面开口
		OPEN_SIZE(){m_fVH=m_fFW=m_fSW=0;}
	};
	ARRAY_LIST<OPEN_SIZE> m_xOpenArr;
	int	m_iMainPartType;		//主材类型0.钢管|1.角钢
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
//由XML型式的数据模型转化为实际的铁塔模型
typedef struct tagSKETCH_SIZEDIM
{
	f3dPoint dimStart,dimEnd;
	BYTE cSizeType;	//'X','Y',Z',"L'
	WORD wAlignStyle;
	double fTextHeight;
	int cascade;	//分层标注时表示分层号
	f3dPoint align_vec,norm,pos;
	BOOL bLegDim;	//是否为接腿上的标注
	CFGWORD cfgword;//同LDS构件的配材号，用于控制多接腿时标注的显示
	tagSKETCH_SIZEDIM();
	double Size();
}SKETCH_SIZEDIM;
//
struct HEIGHT_LEGS{
	BYTE idBodyNo;
	CLDSModule* pModule;
	BYTE cnPresetLegCount;
	WORD wnLevelHeightDiff;	//邻近腿高度级差,mm
	struct LEG{
		WORD wnHeightDiff;	//与最长腿级差,mm
		WORD wiMatchLeg;	//模型中匹配的接腿
		//接腿布材信息
		WORD wiSectionNum;			//接腿节间数
		CXhChar16 szSpec;			//V斜材规格
		CXhChar16 szFrontLegLayout;	//正侧面接腿布材型式
		CXhChar16 szVPlaneLegLayout;//V面布材型式
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
	//默认杆件类型
	bool UpdateTowerModel(CTower* pTower,CShankDefine *pDefine,int nPoly=4,long idClassType=CLS_LINEANGLE);
	bool BuildLegacyFrameTowerModel(CTower* pTower,CFrameDefine *pDefine,int nPoly=4);
};
//#endif
#endif // !defined(AFX_SHANKDEFCLS_H__7900E6F9_E966_41BF_8503_BCF774170A85__INCLUDED_)

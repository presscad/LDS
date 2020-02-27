#ifndef __PART_GROUP_H_
#define __PART_GROUP_H_

#include "lds_part.h"

struct JOINTRAYTUBE	: public ICompareClass 
{
	THANDLE hRayTube;
	double fDist;
	virtual int Compare(const ICompareClass *pCompareObj)const{
		JOINTRAYTUBE* pCopareTube=(JOINTRAYTUBE*)pCompareObj;
		return this->fDist>pCopareTube->fDist;
	}
};
//
struct SECTION_PART{
	CLDSPart *pPart;
	f3dLine prjLine;
	BOOL m_bAddToView;
	SECTION_PART(){pPart=NULL;m_bAddToView=FALSE;}
};
//节点节间
struct SECTION_SEG{
	const static int SECTION_SPACE_THRESHOLD	= 500;	//节点区间间距阈值
	//
	const static int SECTION_TYPE_NODEPLATE		= 0x0;	//0x00.节点板视图 
	const static int SECTION_TYPE_TUBERIBPLATE	= 0x01;	//0x01.跨越连板视图 
	const static int SECTION_TYPE_CIRCOVERPLATE	= 0x02;	//0x02.环向封板视图 
	const static int SECTION_TYPE_CIRRIBPLATE	= 0x04;	//0x04.环向肋板视图 
	const static int SECTION_TYPE_CIRPLATE		= 0x08;	//0x08.环向板端面视图
	//
	BYTE m_cSectionType;	
	SECTION_PART *m_pStartPart;
	SECTION_PART *m_pEndPart;
	SECTION_PART *m_pDatumPart;
	//ATOM_LIST<SECTION_PART> sectionPartList;
	CHashList<SECTION_PART> sectionPartList;
public:
	SECTION_SEG();
	~SECTION_SEG(){;}
	f3dPoint GetSectionDatumPos(){return f3dPoint();}
	BOOL IsIncludeSectionPart(SECTION_PART *pPart);
	int StatSectionPartCount(BOOL bOnlyNoneToView=TRUE);
};
//工作平面
class CPartGroupWorkPlane{
public:
	long iNo,iMatchView;
	CXhChar50 sName;
	CLDSPart *pDatumPart;
	f3dPoint workPlaneNorm,workPlaneOrg;
	CParaCS::PARAMETRIC_INFO csParaInfo;
	int nRayTubeNum,nPartNum;	//工作面优先级的判断依据
	bool bJointTubePlane;		//该工作面为两钢管对接焊接（可能是角分面也可能是指定平面）
	GEPOINT vTempAxisY;			//临时记录的展开面延展方向
	SECTION_PART* pTempSectionParts;
	CDrawingAssistant assistant,assistant2;	//assistant2为除基本近似共面工作面外的，另一近似共面（或垂直）的工作面
public:
	CPartGroupWorkPlane();
	~CPartGroupWorkPlane(){;}
	void SetKey(DWORD dwKey){iNo=(long)dwKey;}
	f3dPoint CalYAxisVec(CLDSLineTube* pRootTube=NULL);
};
struct DIFF_PLANE_ERROR_LOG
{
	long iPlaneNo;		//对应的工作平面
	double diff;		//角度差异
	CXhChar100 error;	//错误信息
};
//
struct CIRPLATE_ITEM{
	f3dPoint m_xPrjPt;
	double m_fPrjLen;
	CLDSPart *m_pPart;
	POLYGONEX m_xRegion;
	CIRPLATE_ITEM(){m_pPart=NULL;m_fPrjLen=0;}
};
//分层单元
class CLayerUnit{
	f3dPoint prj_pt;	//
public:
	BOOL m_bNeed;		//
	ATOM_LIST<CIRPLATE_ITEM> m_xPartSet;
public:
	CLayerUnit();
	~CLayerUnit(){}
	void SetProjectionPt(f3dPoint perp){prj_pt=perp;}
	bool IsInSameLayer(f3dPoint perp){return prj_pt.IsEqual(perp,0.5);}
	BOOL IsOverLapped(CLayerUnit* pUnit,double DECISION=1);
	BOOL IsCirCoverPlateLayer();
};
//分层组
class CLayerGroup{
public:
	BOOL m_bFinish;
	ATOM_LIST<CLayerUnit*> m_xLayerUnitPtrSet;
public:
	CLayerGroup();
	~CLayerGroup(){}
	BOOL IsOverlapped(CLayerUnit* pOtherUnit);
	BOOL IsCirCoverPlateGroup();
	BOOL GetGroupName(CXhChar50& sName);
};
//
struct WELD_PLATES{
	static const BYTE WITH_NONEHOLE_PLATE	= 1;//单纯焊接肋板
	static const BYTE WITH_HOLE_PLATE		= 2;//焊接立板上也含螺栓孔（可能为单面板，也可能为火曲板)
	static const BYTE WITH_BUDDY_BENDPLATE	= 3;//同为火曲板（都含螺栓孔），且火曲线共面
	static const BYTE WITH_TRIPPLE_PLATES	= 4;//三视图组焊板
	char cType;
public:
	CLDSPlate* pRootPlate;
	struct{
		CLDSGeneralPlate* pBuddyPlate;	//与主视图等位钢板
		CLDSGeneralPlate* pSectPlate;	//与其余两块钢板火曲线截面上的焊接钢板
	};
	WELD_PLATES(){memset(this,0,sizeof(WELD_PLATES));}
};

//
class CTubeBushPlateView : public CSectionView
{	//用于安装的衬托板视图(以两根对焊钢管交叉面为视图平面)
public:
	//long m_hRootTube;			//基准钢管（长短差距大时以较长者为基准钢管）
	//long m_hBushPlate;		//衬托板
	CTubeBushPlateView(int id=0);
};
#endif
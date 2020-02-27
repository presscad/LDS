// CommonLjSmartDesign.h: interface for the CMultiRayEndLj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMONLJSMARTDESIGN_H__49DD48F0_EB40_4F48_92DC_A080FD9A4BFC__INCLUDED_)
#define AFX_COMMONLJSMARTDESIGN_H__49DD48F0_EB40_4F48_92DC_A080FD9A4BFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ArrayList.h"
#include "ConnBoltsStrength.h"
#ifdef __ANGLE_PART_INC_
struct RAYROD_INFO
{
protected:
	bool _blViewAsSingularAngle;
public:
	f3dPoint rayVec;			//角钢射线方向（已单位化）
	f3dPoint wing_vec,wing_norm;//角钢工作连接肢方向及法线方向
	int work_wing;				//角钢当前工作连接肢0:wing x;1:wing y
	BYTE ciPlaneNo;		//所在的连接面号,1(基面),2,3
	BOOL bInsideAngle;			//是否为里铁
	CConnectInfo* pConn;		//射线角钢连接端的螺栓连接信息
	int endLjBoltNum;			//射线角钢端连接螺栓数0:不可进行端连接;1:可进行一颗螺栓端连接;2:可进行两颗螺栓端连接
	JGZJ jgzj;					//角钢准距
	LSSPACE_STRU LsSpace;		//射线材螺栓布置间距表
	BOOL bOnSideBer;			//TRUE:射线材位于基准材楞线侧;FALSE:射线材位于基准材肢翼侧
	double alfa;				//单位°，射线角钢在连接工作面内相对于基准材的平面旋转角(基准材始->终为X轴,工作面法线为Z轴)
	double cosa,sina;			//与基准材夹角的正余弦值
	union ENDPOS_PARA{			//射线杆件连接端的位置设计参数
		CMultiOffsetPos* pAnglePos;
		CTubeEndPosPara* pTubePos;
	}pos;
	union ROD{					//射线杆件指针
		CLDSLinePart* pRod;
		CLDSLineAngle* pAngle;
		CLDSGroupLineAngle* pGroupAngle;
		CLDSLineTube* pTube;
	}rod;
	bool get_blViewAsSingularAngle() const;
	bool set_blViewAsSingularAngle(bool blValue);
	__declspec(property(get = get_blViewAsSingularAngle,put=set_blViewAsSingularAngle)) bool blViewAsSingularAngle;
	CMultiOffsetPos* AnglePos(){return pos.pAnglePos;}
	CTubeEndPosPara* TubePos(){return pos.pTubePos;}
	CLDSLinePart* Rod(){return rod.pRod;}
	CLDSLineAngle* Angle(){return rod.pAngle;}
	__declspec(property(get = Rod)) CLDSLinePart* pRod;
	__declspec(property(get = Angle)) CLDSLineAngle* pAngle;
	CLDSLineTube* Tube(){return rod.pTube;}
	static int CompareRayRod(const RAYROD_INFO& rodInfo1,const RAYROD_INFO& rodInfo2);
	RAYROD_INFO() { memset(this, 0, sizeof(RAYROD_INFO)); ciPlaneNo = 1; }
};
struct COVER_REGION{
	double lenOffsetStart;	//以节点在基准材心线(g,g1,g2)投影为基点定位，沿基准材心线自始至终方向的起始偏移量
	double lenOffsetEnd;	//以节点在基准材心线(g,g1,g2)投影为基点定位，沿基准材心线自始至终方向的终止偏移量
	bool bDatumSonAngle;
	int iRayOrder;			//射线(角钢)的索引号
};
class CIntelliModelDesigner;
class CCommonLjSmartDesign
{
	BOOL m_ciRayEndFaceOffset;	//射线材端头平推处理，此时基准材螺栓默认也应为平推
	CLDSBolt *m_pFirstEndLjBolt,*m_pSecondEndLjBolt;
	int wing_intersline_style;	//多个螺栓端连接时，射线材交点所在心线方式（即内排螺栓所在心线）
protected:
	CFGWORD cfgword;
	UCS_STRU ucs;
	long m_nBaseStartG3,m_nBaseEndG3;	//基准材始终侧端连接螺栓遵循的特殊准距
	JGZJ m_jgzjBase;		//基准角钢的当前肢准距
	CConnectPointInfo *m_pConnectPoint;
	CLDSNode* m_pBaseNode;
	CSuperSmartPtr<CLDSLinePart>m_pBaseRod;
	int m_iBaseAngleWorkWing;		//0:wing x;1:wing y
	int m_iFirstEndLjAngle,m_iSecEndLjAngle;
	f3dPoint base_wing_vec,base_wing_norm;
	CTmaPtrList<CLDSBolt,CLDSBolt*> boltset,endlj_boltset;	//临时记录当前连接中包含的螺栓
	PARTSET padSet;
	ARRAY_LIST<RAYROD_INFO>m_arrRayRod;
	ARRAY_LIST<COVER_REGION>m_arrG1Cover,m_arrG2Cover;	//射线在基准材g1(或g)、g2心线上的遮盖区域
	CLDSBolt *m_pShareBolt;	//端连接时的共用螺栓
	CLDSPlate *m_pPlate;	//有板连接时的连接板
	CONNBOLTS xBaseAngleBolts;
	CLDSLineAngle* BaseAngle(); 
	CLDSGroupLineAngle* BaseGroupAngle(); 
	CLDSLineTube* BaseTube(); 
	void SetPlateBasicInfo();
	BOOL Prepare(int endljNum,BOOL bHasPlate,BOOL canMultiRow,BOOL priorMiddleEndLj=TRUE);	//连接设计前的准备处理
	virtual void LayoutBaseAngleBolts();
	virtual bool DesignBraceConnParams(short* psiStyleG,double* pdfOffsetDistG);	//设计辅材连接点相关参数
	void LayoutAngleBolts(int index);
	BOOL DesignBendPlane(CConnectPointInfo *pConnectPoint);
	//获取iRayRodOrder射线角钢的两侧负头计算参照杆件 wjh-2019.12.13
	BOOL GetOddCalNearRods(int iRayRodOrder,long* phRefRod1=NULL,long* phRefRod2=NULL);
public:
	CCommonLjSmartDesign();
	virtual ~CCommonLjSmartDesign();
	static bool DesignSlopeTransition(SLOPE_TRANSITION* pTransit,CIntelliModelDesigner* pIntelliDesigner=NULL);
	static bool DesignSlopOrFootPoint(SLOPE_TRANSITION* pTransit);
	long DesignConnectPoint(CConnectPointInfo *pConnectPoint,int endljNum,BOOL bHasPlate,BOOL canMultiRow=TRUE,BOOL priorMiddleEndLj=TRUE,bool bCreate3dSolid=true);
};
struct ENDLJ_ANGLEPTR{
	int iAngleOrder;	//按照角度的排序索引号
	RAYROD_INFO *pRod;
};
class CGroupAngleLjDesign:public CCommonLjSmartDesign
{
	CLDSLineAngle *m_pDatumSonAngle;	//组合角钢中的1号特征角钢
	CLDSLineAngle *m_pCoupleSonAngle;	//组合角钢中与1号特征角钢相配对的子角钢
	CLDSLineAngle *m_pCoupleSonAngle2;	//组合角钢中与1号特征角钢相配对的子角钢1 3
	CLDSLineAngle *m_pCoupleSonAngle4;	//组合角钢中与1号特征角钢相配对的子角钢2 4
	ARRAY_LIST<ENDLJ_ANGLEPTR>m_arrRayRodPtr;
protected:
	static int CalFillPlateBoltNum(int wide);
	//axis_g_n=1,只计算g心线覆盖区域，axis_g_n=2,只计算g1,g2心线覆盖区域，axis_g_n=3,计算g,g1,g2心线覆盖区域
	virtual void CalRayCoverRegion(int iRayOrder,COVER_REGION cover_arr[],int axis_g_n=3);
	virtual BOOL Prepare(BOOL bEndLjPrefer=TRUE);	//连接设计前的准备处理
	virtual void LayoutBaseAngleBolts();
	void LayoutAngleBolts(int index,CLDSBolt *pDatumEndLjBolt=NULL);
public:
	static int FILLPLATE_BOLT2_WIDEMAX;
	static int FILLPLATE_BOLT3_WIDEMAX;
	static int FILLPLATE_BOLT4_WIDEMAX;
	static int FILLPLATE_BOLT5_WIDEMAX;
	static int FILLPLATE_BOLT6_WIDEMAX;
	CGroupAngleLjDesign();
	virtual ~CGroupAngleLjDesign();
	long DesignConnectPoint(CConnectPointInfo *pConnectPoint,BOOL bEndLjPrefer=TRUE);
};

#endif
#endif // !defined(AFX_COMMONLJSMARTDESIGN_H__49DD48F0_EB40_4F48_92DC_A080FD9A4BFC__INCLUDED_)

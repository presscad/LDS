#pragma once
#include <WinDef.h>
#include "SolidBody.h"

//获取圆弧简化轮廓点
void GetAngleArcVertex(double dfChordL,double r,BOOL isOn,f3dPoint vertex_arr[],int slices);
struct METASOLID{
public:
	struct PROFVERTEX{
		bool blSkipEdge;	//跳过不生成此顶点为起点的轮廓边
		char ciEdgeType;	//0:普通直边1:指定圆弧夹角2:指定圆弧半径3:椭圆弧
		short snArcSlices;	//>0:表示圆弧离散成多折边数目，<0:表示指定离散精度
		GEPOINT pos;		//顶点的位置坐标
		double cx,cy;		//圆心点坐标
		union{
			double dfArcRadius;	//椭圆弧所需要的参数
			double dfSectAngle;	//标准圆弧所需要的参数
		};
		GEPOINT vColNormal;	//椭圆弧所需要的参数
		PROFVERTEX(){memset(this,0,sizeof(PROFVERTEX));}
	};
public:
	//parrVertexes由于sizeof(f3dPoint)=2*sizeof(COORD3D)=2*sizeof(GEPOINT)，故此处parrVertexes不允许定义为f3dPoint[] wjh-2019.12.27
	static UINT CirclePolygonize(double radius, COORD3D* parrVertexes, UINT uiMaxCount,bool blIntelliCalSlices=true);
	//向一组圆点插入指定数量特征点
	static int AppendSamplePointsToCirclePolyVertex(GEPOINT* parrSamplePoints, int nMaxSamplePointsCount, GEPOINT* parrCirPolyVertexes, int nCurrCirPolyVertexesCount, double TOLERANCE_ALFA);
	//向pBody添加实心圆柱元实体
	static int AppendDiskSolid(fBody* pBody,double d,double H,GECS& acs,int slices,COLORREF crLs);
	//向pBody添加空心圆管/垫圈/套筒等元实体,其中D为外径
	static int AppendTubeSolid(fBody* pBody,double D,double t,double H,GECS& acs,int slices,COLORREF crColor);
	//向pBody添加圆环实体,R圆环外径，r圆环内径，fAngle环角度(0~2Pi)
	static int AppendRingSolid(fBody* pBody, double R, double r, double fArcAngle, GECS& acs, int slices, COLORREF crColor);
	//向pBody添加含端面倒弧的六角螺母元实体
	static int AppendTaperNutSolid(fBody* pBody,double De,double Ds,double H,GECS& acs,int slices,COLORREF crLs);
	//向pBody添加简化六角螺母（不含过端面倒弧）元实体
	static const BYTE NUT_INC_TOP  = 0x01;
	static const BYTE NUT_INC_BTM  = 0x02;
	static const BYTE NUT_INC_SIDE = 0x04;
	static int AppendHexPolygonNutSolid(fBody* pBody,double De,double H,GECS& acs,COLORREF crLs,DWORD dwFaceFlag=0);
	//生成由指定多边形（含圆弧边）拉伸形成的标准实体
	static int AppendPolygonExtrudeSolid(fBody* pBody,GECS& acs,double dfExtrudeLength,PROFVERTEX* xarrProfVertexes,int niCount,COLORREF crColor);
	//生成指定角钢肢开合区域的开合变形区实体
	struct KAIHEINFO {
		char   ciOperLeg;		//当前开合操作的角钢肢,取值 0/'X',1/'Y'
		double dfOpenRadAngle;	//>0表示开角，<0合角度数，弧度值
		double dfDeformedLenOfStart,dfDeformedLenOfEnd;	//开合区域起止两侧的变形区长度
		double dfZlenStart,dfZlenEnd;	//开合角长度方向起止Z值
		double dfLegWidth,dfLegThick;	//开合肢的宽度与厚度
		double dfReservedWidth;			//开合角肢向保留值（肢向保留不开合区域的宽度，一般为肢厚+2R/3,取值>=肢厚）
		KAIHEINFO() { memset(this,0,sizeof(KAIHEINFO)); }
		bool CalMidFlatVertexes(COORD3D xarrVertexes[14]) const;
		bool CalKaiheVertexes(COORD3D xarrVertexes[12]) const;
	};
	static int AppendMidFlatAngleWing(fBody* pBody,GECS& acs,const KAIHEINFO* pDefKaiheInfo,COLORREF crLegPlane=0,COLORREF crTipEdge=0);
	static int AppendKaiheAngleWing(fBody* pBody,GECS& acs,const KAIHEINFO* pDefKaiheInfo,COLORREF crLegPlane=0,COLORREF crTipEdge=0);
	static bool InitializeArcEdgeline(f3dAtomLine* pAtomLine,PROFVERTEX* pStart,PROFVERTEX* pEnd,bool bByStartVertex);
	//生成特定孔
	static BOOL AppendPolyHole(GECS &hole_ucs, PROFVERTEX* xarrVertex, int nVertexCount,
		f3dPolyFace *pUpFace, f3dPolyFace *pDownFace, fBody *pBody, COLORREF crLsHole);
};
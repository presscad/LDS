#include "StdAfx.h"
#include "f_alg_fun.h"
#include "CreateFace.h"
#include "StdMetaSolid.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#include "StdMetaSolid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//获取圆弧简化轮廓点
void GetAngleArcVertex(double dfChordL,double r,BOOL isOn,f3dPoint vertex_arr[], int slices)
{
	double startradius=0;
	double sector_angle=2*asin(dfChordL/(2*r));
	if (isOn)
		startradius=(Pi-sector_angle)/2;
	else
		startradius=(Pi-sector_angle)/2+Pi;
	double slice_angle = sector_angle/(slices+1);
	double sina = sin(slice_angle);	//扇片角正弦
	double cosa = cos(slice_angle);	//扇片角余弦
	vertex_arr[0].Set(r*cos(startradius+slice_angle),r*sin(startradius+slice_angle));
	for(int i=1;i<slices;i++)
	{
		vertex_arr[i].x=vertex_arr[i-1].x*cosa-vertex_arr[i-1].y*sina;
		vertex_arr[i].y=vertex_arr[i-1].y*cosa+vertex_arr[i-1].x*sina;
		vertex_arr[i].z=0;
	}
}
UINT METASOLID::CirclePolygonize(double radius, COORD3D* parrVertexes, UINT uiMaxCount,bool blIntelliCalSlices/*=true*/)
{
	UINT slices=uiMaxCount;
	if(blIntelliCalSlices)
	{
		const double SAMPLE_LEN=2.0;
		double length=(Pi+Pi)*radius;
		slices = max(6,(int)(length/SAMPLE_LEN+0.5));
		slices=min(slices,uiMaxCount);//int max_n=144;//ftoi(72*sector_angle/Pi);
	}
	double sina,cosa;
	if(slices==6)
	{
		sina=0.86602540378443864676372317075294;
		cosa=0.5;
	}
	else if(slices==12)
	{
		sina=0.5;
		cosa=0.86602540378443864676372317075294;
	}
	else if(slices==16)
	{
		sina=0.3826834323650897717284599840304;
		cosa=0.92387953251128675612818318939679;
	}
	else if(slices==24)
	{
		sina=0.25881904510252076234889883762405;
		cosa=0.9659258262890682867497431997289;
	}
	else if(slices==36)
	{
		sina=0.17364817766693034885171662676931;
		cosa=0.98480775301220805936674302458952;
	}
	else
	{
		double dfSliceAngle= 2*Pi/slices;
		sina = sin(dfSliceAngle);	//扇片角正弦
		cosa = cos(dfSliceAngle);	//扇片角余弦
	}
	parrVertexes[0].Set(radius);
	for(UINT i=1;i<slices;i++)
	{
		parrVertexes[i].x=parrVertexes[i-1].x*cosa-parrVertexes[i-1].y*sina;
		parrVertexes[i].y=parrVertexes[i-1].y*cosa+parrVertexes[i-1].x*sina;
		parrVertexes[i].z=0;
	}
	return slices;
}
int METASOLID::AppendDiskSolid(fBody* pBody,double d,double H,GECS& acs,int slices,COLORREF crLs)
{
	GEPOINT xarrCirclePolyVertexes[32];
	GEPOINT xarrDiskSolidVertexes[64];
	CirclePolygonize(d*0.5,xarrCirclePolyVertexes,32,false);
	int i;
	slices=32;
	for(i=0;i<32;i++)
	{
		xarrDiskSolidVertexes[i]=xarrCirclePolyVertexes[i];
		xarrDiskSolidVertexes[i+slices]=xarrCirclePolyVertexes[i];
		xarrDiskSolidVertexes[i+slices].z=H;
	}
	long liInitIndex=pBody->vertex.GetNodeNum();
	for(i=0;i<64;i++)
		pBody->vertex.append(xarrDiskSolidVertexes[i]);
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4]={0,0,0,0};
	//圆柱体顶面
	f3dPolyFace *pFace=pBody->faceList.append();
	pFace->poly_norm=acs.TransVFromCS(GEPOINT(0,0,1));
	pFace->material=crLs;
	for (i=0;i<slices;i++)
		createFace.NewOutterEdgeLine(pFace,(i+1)%slices+slices+liInitIndex,i+slices+liInitIndex);
	//圆柱体底面
	pFace=pBody->faceList.append();
	pFace->poly_norm=acs.TransVFromCS(GEPOINT(0,0,-1));
	pFace->material=crLs;
	for (i=slices-1;i>=0;i--)
		createFace.NewOutterEdgeLine(pFace,(slices+i-1)%slices+liInitIndex,i+liInitIndex);
	//圆柱体侧面
	for (i=0;i<slices;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material=crLs;
		createFace.NewOutterEdgeLine(pFace,(i+1)%slices+liInitIndex,i+liInitIndex);
		createFace.NewOutterEdgeLine(pFace,(i+1)%slices+liInitIndex+slices);
		createFace.NewOutterEdgeLine(pFace,i+liInitIndex+slices);
		createFace.NewOutterEdgeLine(pFace,i+liInitIndex);
	}
	i=0;
	for(f3dPoint* pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext(),i++)
	{
		if(i<liInitIndex)
			continue;	//原有顶点
		GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
		pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
	}
	return pBody->vertex.GetNodeNum();
}
//向pBody添加空心圆管/垫圈/套筒等元实体,其中D为外径
int METASOLID::AppendTubeSolid(fBody* pBody,double D,double t,double H,GECS& acs,int slices,COLORREF crColor)
{
	GEPOINT xarrBtmVertexes[100],xarrBtmInnerVertexes[100];
	slices = min(slices, 100);
	int i, countOfOuter, countOfInner;
	long liStartIndex=pBody->vertex.GetNodeNum();
	countOfOuter=CirclePolygonize(D*0.5,xarrBtmVertexes,slices);
	countOfInner=CirclePolygonize((D*0.5-t),xarrBtmInnerVertexes,slices);
	//外圆
	for(i=0;i<countOfOuter;i++)
		pBody->vertex.append(xarrBtmVertexes[i].x,xarrBtmVertexes[i].y,0);
	for(i=0;i<countOfOuter;i++)
		pBody->vertex.append(xarrBtmVertexes[i].x,xarrBtmVertexes[i].y,H);
	for(i=0;i<countOfInner;i++)
		pBody->vertex.append(xarrBtmInnerVertexes[i].x,xarrBtmInnerVertexes[i].y,0);
	for(i=0;i<countOfInner;i++)
		pBody->vertex.append(xarrBtmInnerVertexes[i].x,xarrBtmInnerVertexes[i].y,H);
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	//绘制螺栓顶面
	f3dPolyFace *pUpFace=NULL,*pDownFace=NULL;
	//顶面生成
	f3dPolyFace* pFace=pBody->faceList.append();
	
	//pFace->poly_norm.Set(1,0,0);
	pFace->material=crColor;
	for(i=0;i<countOfOuter;i++)
		createFace.NewOutterEdgeLine(pFace,(i+1)%countOfOuter+countOfOuter+liStartIndex,i+countOfOuter+liStartIndex);
	pUpFace=pFace;
	//添加顶面内环
	fLoop* pInnerLoop=pUpFace->inner_loop.append();
	int iInitIndex=countOfOuter*2+countOfInner+liStartIndex;
	for(i=countOfInner-1;i>=0;i--)
		pInnerLoop->append(createFace.VertexPtrAt(i+iInitIndex), createFace.VertexPtrAt((i+countOfInner-1)%countOfInner+iInitIndex));
	//底面生成
	pFace=pBody->faceList.append();
	//pFace->poly_norm.Set(-1,0,0);
	pFace->material=crColor;
	for (i=countOfOuter-1;i>=0;i--)
		createFace.NewOutterEdgeLine(pFace,(countOfOuter+i-1)%countOfOuter+liStartIndex,i+liStartIndex);
	pDownFace=pFace;
	//添加底面内环
	iInitIndex=countOfOuter*2+liStartIndex;
	pInnerLoop=pDownFace->inner_loop.append();
	for(i=0;i<countOfInner;i++)
		pInnerLoop->append(createFace.VertexPtrAt(i + iInitIndex), createFace.VertexPtrAt((i+1)%countOfInner + iInitIndex));
	//绘制管壁外侧
	for(i=0;i<countOfOuter;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material=crColor;
		createFace.NewOutterEdgeLine(pFace,(i+1)%countOfOuter+liStartIndex,i+liStartIndex);
		createFace.NewOutterEdgeLine(pFace,(i+1)%countOfOuter+countOfOuter+liStartIndex);
		createFace.NewOutterEdgeLine(pFace,i+countOfOuter+liStartIndex);
		createFace.NewOutterEdgeLine(pFace,i+liStartIndex);
	}
	//绘制管壁内侧
	iInitIndex=countOfOuter*2+liStartIndex;
	for(i=0;i<countOfInner;i++)
	{
		pFace=pBody->faceList.append();
		pFace->material=crColor;
		createFace.NewOutterEdgeLine(pFace,i+iInitIndex,(i+1)%countOfInner+iInitIndex);
		createFace.NewOutterEdgeLine(pFace,i+countOfInner+iInitIndex);
		createFace.NewOutterEdgeLine(pFace,(i+1)%countOfInner+countOfInner+iInitIndex);
		createFace.NewOutterEdgeLine(pFace,i+iInitIndex);
	}
	i=0;
	for(f3dPoint* pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext(),i++)
	{
		if(i<liStartIndex)
			continue;	//原有顶点
		GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
		pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
	}
	return pBody->vertex.GetNodeNum();
}
//向pBody添加圆环实体
int METASOLID::AppendRingSolid(fBody* pBody, double R, double r, double fArcAngle, GECS& acs, int slices, COLORREF crColor)
{
	//获取圆环的圆截面轮廓点
	GEPOINT vertexs[200];
	double fSectionCirD = R - r;
	int nCount = METASOLID::CirclePolygonize(fSectionCirD*0.5, vertexs, 128);
	DYN_ARRAY<GEPOINT> section_vertex_arr(nCount);
	for (int i = 0; i < nCount; i++)
		section_vertex_arr[i] = vertexs[i];
	//简化环形路径，添加轮廓点
	long liStartIndex = pBody->vertex.GetNodeNum();
	double rot_ang = fArcAngle / slices;
	GEPOINT ring_path_pt;
	for (int i = 0; i <= slices; i++)
	{
		if (i == 0)
			ring_path_pt.Set(r + fSectionCirD * 0.5, 0, 0);
		else
			rotate_point_around_axis(ring_path_pt, rot_ang, GEPOINT(0,0,0), GEPOINT(0, 0, 1));
		GECS section_cir_cs;
		section_cir_cs.origin = ring_path_pt;
		section_cir_cs.axis_x = GEPOINT(0, 0, 0)-ring_path_pt;
		normalize(section_cir_cs.axis_x);
		section_cir_cs.axis_y.Set(0, 0, 1);
		section_cir_cs.axis_z = section_cir_cs.axis_x^section_cir_cs.axis_y;
		normalize(section_cir_cs.axis_z);
		for (DWORD j = 0; j < section_vertex_arr.Size(); j++)
		{	//进行平面点的转换
			f3dPoint vertex = section_vertex_arr[j];
			coord_trans(vertex, section_cir_cs, TRUE);
			pBody->vertex.append(vertex);
		}
	}
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	f3dPolyFace *pFace = pBody->faceList.append();
	for (int i = 0; i < slices; i++)
	{
		int iStart = liStartIndex + i * nCount;
		for (int j = 0; j < nCount; j++)
		{
			pFace = pBody->faceList.append();
			pFace->material = crColor;
			createFace.NewOutterEdgeLine(pFace, (j + 1) % nCount + iStart, j + iStart);
			createFace.NewOutterEdgeLine(pFace, (j + 1) % nCount + nCount + iStart);
			createFace.NewOutterEdgeLine(pFace, j + nCount + iStart);
			createFace.NewOutterEdgeLine(pFace, j + iStart);
		}
	}
	//转换到坐标系下
	int index = 0;
	for (f3dPoint* pVertice = pBody->vertex.GetFirst(); pVertice; pVertice = pBody->vertex.GetNext(), index++)
	{
		if (index < liStartIndex)
			continue;	//原有顶点
		GEPOINT xActualPoint = acs.TransPFromCS(*pVertice);
		pVertice->Set(xActualPoint.x, xActualPoint.y, xActualPoint.z);
	}
	return pBody->vertex.GetNodeNum();
}
int METASOLID::AppendTaperNutSolid(fBody* pBody,double De,double Ds,double H,GECS& acs,int slices,COLORREF crLs)
{
	long i=0,j=0,liInitIndex=pBody->vertex.GetNodeNum();
	//Ds=De*0.866;
	const int BOLT_EDGE_COUNT=6;//螺母的边数
	double dfHalfD=De/2;
	GEPOINT xarrBoltPolyVertexes[12];
	double cosa_arr[6]={1.0,0.5,-0.5,-1.0,-0.5,0.5};
	double sina_arr[6]={0.0,0.8660254037844,0.8660254037844,0.0,-0.8660254037844,-0.8660254037844};
	double w=(De-Ds)/2;
	double dfInnerD=min(dfHalfD*cos(2*Pi/(BOLT_EDGE_COUNT*2)),Ds/2);
	double m_fNutTaper=0.35;
	double h=w*cos(m_fNutTaper);//坡度的高
	//生成侧面点
	for(i=0;i<BOLT_EDGE_COUNT;i++)
	{
		xarrBoltPolyVertexes[i].Set(cosa_arr[i]*dfHalfD,sina_arr[i]*dfHalfD,h);		//螺帽贴合面
		xarrBoltPolyVertexes[BOLT_EDGE_COUNT+i]=xarrBoltPolyVertexes[i];
		xarrBoltPolyVertexes[BOLT_EDGE_COUNT+i].z+=H-2*h;//螺帽顶面
	}
	//生成顶面点
	GEPOINT btm_vertex_arr[200],b_vertex_arr[200];
	CirclePolygonize(dfInnerD,b_vertex_arr,slices,false);
	for(i=0;i<slices;i++)
	{
		btm_vertex_arr[i].Set(b_vertex_arr[i].x,b_vertex_arr[i].y);		//螺帽贴合面
		btm_vertex_arr[slices+i]=btm_vertex_arr[i];
		btm_vertex_arr[slices+i].z+=H;//螺帽顶面
	}
	CStack<void*> stack;
	int pushed=pBody->vertex.PushStack();
	//将点添加入Body，按照 下侧六角顶点+下圆端面点+上侧六角顶点+上圆端面点
	for (i=0;i<2;i++)
	{
		for(j=BOLT_EDGE_COUNT*i;j<BOLT_EDGE_COUNT*(i+1);j++)
			pBody->vertex.append(xarrBoltPolyVertexes[j]);
		for(j=i*slices;j<slices*(i+1);j++)
			pBody->vertex.append(btm_vertex_arr[j]);
	}
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4]={0,0,0,0};
	//螺母顶面
	f3dPolyFace *pFace=pBody->faceList.append();
	pFace->poly_norm=acs.TransVFromCS(GEPOINT(0,0,1));
	pFace->material=crLs;
	int liTmpInitIndex=liInitIndex+slices+BOLT_EDGE_COUNT*2;
	for (i=0;i<slices;i++)
		createFace.NewOutterEdgeLine(pFace,(i+1)%slices+liTmpInitIndex,i+liTmpInitIndex);
	//螺母底面
	pFace=pBody->faceList.append();
	pFace->poly_norm=acs.TransVFromCS(GEPOINT(0,0,-1));
	pFace->material=crLs;
	liTmpInitIndex=liInitIndex+BOLT_EDGE_COUNT;
	for (i=slices-1;i>=0;i--)
		createFace.NewOutterEdgeLine(pFace,(slices+i-1)%slices+liTmpInitIndex,i+liTmpInitIndex);
	//螺母的下坡面
	for(i=BOLT_EDGE_COUNT;i<slices+BOLT_EDGE_COUNT;i++)
	{
		j=0;
		int k=(i-BOLT_EDGE_COUNT+slices/(BOLT_EDGE_COUNT*2))%slices;
		j=k/(slices/BOLT_EDGE_COUNT);
		vertexIndexArr[0]=i+liInitIndex;
		vertexIndexArr[1]=(i-BOLT_EDGE_COUNT+1)%slices+BOLT_EDGE_COUNT+liInitIndex;
		vertexIndexArr[2]=j+liInitIndex;
		//
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutsideFace(pFace, vertexIndexArr, 3);
	}
	//螺母的上坡面
	for(i=slices+BOLT_EDGE_COUNT*2;i<(slices+BOLT_EDGE_COUNT)*2;i++)
	{
		j=0;
		int k=(i-BOLT_EDGE_COUNT*2-slices+slices/(BOLT_EDGE_COUNT*2))%slices;
		j=k/(slices/BOLT_EDGE_COUNT)+BOLT_EDGE_COUNT+slices;
		vertexIndexArr[2]=i+liInitIndex;
		vertexIndexArr[1]=(i-BOLT_EDGE_COUNT*2+1)%slices+slices+BOLT_EDGE_COUNT*2+liInitIndex;
		vertexIndexArr[0]=j+liInitIndex;
		//
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutsideFace(pFace, vertexIndexArr, 3);
	}
	int iDifference=slices/(BOLT_EDGE_COUNT*2)+BOLT_EDGE_COUNT;//侧面点与顶面和侧面的切点的索引差
	//螺母的侧面
	for(i=0;i<BOLT_EDGE_COUNT;i++)
	{
		vertexIndexArr[0]=i+liInitIndex;
		vertexIndexArr[1]=(i+1)%BOLT_EDGE_COUNT +liInitIndex;
		vertexIndexArr[2]=(i+1)%BOLT_EDGE_COUNT +liInitIndex+slices+BOLT_EDGE_COUNT;
		vertexIndexArr[3]= i					+liInitIndex+slices+BOLT_EDGE_COUNT;
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutsideFace(pFace, vertexIndexArr, 4);
		//螺母侧面与顶面切点处生成的小面
		vertexIndexArr[2]=i+liInitIndex;
		vertexIndexArr[1]=(i+1)%BOLT_EDGE_COUNT+liInitIndex;
		vertexIndexArr[0]=i*slices/BOLT_EDGE_COUNT+iDifference+liInitIndex;
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutsideFace(pFace, vertexIndexArr, 3);
		//
		vertexIndexArr[0]=i+slices+BOLT_EDGE_COUNT+liInitIndex;
		vertexIndexArr[1]=(i+1)%BOLT_EDGE_COUNT+slices+BOLT_EDGE_COUNT+liInitIndex;
		vertexIndexArr[2]=i*slices/BOLT_EDGE_COUNT+slices+BOLT_EDGE_COUNT+iDifference+liInitIndex;
		pFace = pBody->faceList.append();
		pFace->material = crLs;
		createFace.NewOutsideFace(pFace, vertexIndexArr, 3);
	}
	if(pBody->vertex.PopStack(pushed))
	{
		for(f3dPoint* pVertice=pBody->vertex.GetNext();pVertice;pVertice=pBody->vertex.GetNext())
		{
			GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
			pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
		}
	}
	else
	{
		i=0;
		for(f3dPoint* pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext(),i++)
		{
			if(i<liInitIndex)
				continue;	//原有顶点
			GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
			pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
		}
	}
	return pBody->vertex.GetNodeNum();
}
//向pBody添加简化六角螺母（不含过端面倒弧）元实体
int METASOLID::AppendHexPolygonNutSolid(fBody* pBody,double De,double H,GECS& acs,COLORREF crLs,DWORD dwFaceFlag/*=0*/)
{
	if(dwFaceFlag==0)
		dwFaceFlag=0xff;
	GEPOINT xarrHexPolyVertexes[6];
	GEPOINT xarrNutVertexes[12];

	long i=0,j=0;
	const int BOLT_EDGE_COUNT=6;//螺母的边数
	double dfHalfD=De/2;
	double cosa_arr[6]={1.0,0.5,-0.5,-1.0,-0.5,0.5};
	double sina_arr[6]={0.0,0.8660254037844,0.8660254037844,0.0,-0.8660254037844,-0.8660254037844};
	//生成侧面点
	for(i=0;i<BOLT_EDGE_COUNT;i++)
	{
		xarrNutVertexes[i].Set(cosa_arr[i]*dfHalfD,sina_arr[i]*dfHalfD,0);		//螺母底面六角顶点
		xarrNutVertexes[i+6].Set(xarrNutVertexes[i].x,xarrNutVertexes[i].y,H);	//螺母顶面六角顶点
	}
	int pushed=pBody->vertex.PushStack();
	long liInitIndex=pBody->vertex.GetNodeNum();
	for(i=0;i<12;i++)
		pBody->vertex.append(xarrNutVertexes[i]);
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4]={0,0,0,0};
	//螺母顶面
	f3dPolyFace *pTopFace=NULL,*pBtmFace=NULL,*pFace=NULL;
	if(dwFaceFlag&NUT_INC_TOP)
	{
		pTopFace=pBody->faceList.append();
		pTopFace->poly_norm=acs.TransVFromCS(GEPOINT(0,0,1));
		pTopFace->material=crLs;
	}
	//螺母底面
	if(dwFaceFlag&NUT_INC_BTM)
	{
		pBtmFace=pBody->faceList.append();
		pBtmFace->poly_norm=acs.TransVFromCS(GEPOINT(0,0,-1));
		pBtmFace->material=crLs;
	}
	for (i=0;i<BOLT_EDGE_COUNT;i++)
	{
		//螺母顶面
		if(pTopFace)
			createFace.NewOutterEdgeLine(pTopFace,(i+1)%6+BOLT_EDGE_COUNT+liInitIndex,i+BOLT_EDGE_COUNT+liInitIndex);
		if(dwFaceFlag&NUT_INC_SIDE)
		{	//螺母侧面
			f3dPolyFace *pFace=pBody->faceList.append();
			pFace->material=crLs;
			createFace.NewOutterEdgeLine(pFace,(i+1)%6+				 0+liInitIndex,i+liInitIndex);
			createFace.NewOutterEdgeLine(pFace,(i+1)%6+BOLT_EDGE_COUNT+liInitIndex);
			createFace.NewOutterEdgeLine(pFace, i     +BOLT_EDGE_COUNT+liInitIndex);
			createFace.NewOutterEdgeLine(pFace, i     +				 0+liInitIndex);
		}
		if(pBtmFace)
		{	//螺母底面
			int ki=(5-i);	//(0~5)
			int kn=(6+ki-1)%6;
			createFace.NewOutterEdgeLine(pBtmFace,kn+liInitIndex,ki+liInitIndex);
		}
	}
	if(pBody->vertex.PopStack(pushed))
	{
		for(f3dPoint* pVertice=pBody->vertex.GetNext();pVertice;pVertice=pBody->vertex.GetNext())
		{
			GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
			pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
		}
	}
	else
	{
		i=0;
		for(f3dPoint* pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext(),i++)
		{
			if(i<liInitIndex)
				continue;	//原有顶点
			GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
			pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
		}
	}
	return pBody->vertex.GetNodeNum();
}
//向一组圆点插入指定数量特征点
struct INSERTPOS {
	//0.将[index]移位至插入样点；1.将[index+1]移位至插入样点；2.在[index]之后 插入样点；其余不在当前扇片区
	char ciProcessType;
	short index;
};
int METASOLID::AppendSamplePointsToCirclePolyVertex(GEPOINT* parrSamplePoints, int nMaxSamplePointsCount,
	GEPOINT* parrCirPolyVertexes, int nCurrCirPolyVertexesCount, double TOLERANCE_ALFA)
{
	double dfSquareR = parrCirPolyVertexes[0].x*parrCirPolyVertexes[0].x + parrCirPolyVertexes[0].y*parrCirPolyVertexes[0].y;
	double dfSquareTolerDelta=dfSquareR*TOLERANCE_ALFA*TOLERANCE_ALFA;
	short i, j;
	for(i=0;i<nMaxSamplePointsCount;i++)
	{
		GEPOINT xSamPoint = parrSamplePoints[i];
		INSERTPOS insertpos;
		insertpos.index = -1;
		GEPOINT vertex, nextVertex;
		for(j=0;j<nCurrCirPolyVertexesCount;j++)
		{
			//TODO:可根据坐标简易判断是xSamPoint是否在parrCirPolyVertexes[j]与parrCirPolyVertexes[(j+1)%nCurrCirPolyVertexesCount]区间
			//TODO:赋值insertpos
			vertex = parrCirPolyVertexes[j];
			nextVertex = parrCirPolyVertexes[(j + 1) % nCurrCirPolyVertexesCount];
			if (xSamPoint.IsEqual(vertex))
				insertpos.index = j;
			else if (nextVertex.x > vertex.x)
			{	//顺时针的上半圆和逆时针的下半圆
				if (xSamPoint.x > vertex.x && xSamPoint.x < nextVertex.x)
					insertpos.index = j;
				else if (vertex.y > 0 && nextVertex.y < 0)
				{	//逆时针左半圆或顺时针右半圆
					if (vertex.x < 0 && nextVertex.x < 0 && xSamPoint.x <= vertex.x)
						insertpos.index = j;
					else if (vertex.x > 0 && nextVertex.x > 0 && xSamPoint.x >= nextVertex.x)
						insertpos.index = j;
				}
				else if (vertex.y < 0 && nextVertex.y > 0)
				{	//逆时针右半圆或顺时针左半圆
					if (vertex.x < 0 && nextVertex.x < 0 && xSamPoint.x <= vertex.x)
						insertpos.index = j;
					else if (vertex.x > 0 && nextVertex.x > 0 && xSamPoint.x >= nextVertex.x)
						insertpos.index = j;
				}
			}
			else  if (nextVertex.x < vertex.x)
			{	//顺时针的下半圆和逆时针的上半圆
				if (xSamPoint.x > nextVertex.x  && xSamPoint.x < vertex.x)
					insertpos.index = j;
				else if (vertex.y > 0 && nextVertex.y < 0)
				{	//逆时针左半圆或顺时针右半圆
					if (vertex.x < 0 && nextVertex.x < 0 && xSamPoint.x <= nextVertex.x)
						insertpos.index = j;
					else if (vertex.x > 0 && nextVertex.x > 0 && xSamPoint.x >= vertex.x)
						insertpos.index = j;
				}
				else if (vertex.y < 0 && nextVertex.y > 0)
				{	//逆时针右半圆或顺时针左半圆
					if (vertex.x < 0 && nextVertex.x < 0 && xSamPoint.x <= nextVertex.x)
						insertpos.index = j;
					else if (vertex.x > 0 && nextVertex.x > 0 && xSamPoint.x >= vertex.x)
						insertpos.index = j;
				}
			}
			if(insertpos.index>-1)
				break;
		}
		//判断公差确认方式
		double dfSquareDeltaLast = (xSamPoint.x - vertex.x)*(xSamPoint.x - vertex.x) + (xSamPoint.y - vertex.y)*(xSamPoint.y - vertex.y);
		double dfSquareDeltaNext = (xSamPoint.x - nextVertex.x)*(xSamPoint.x - nextVertex.x) + (xSamPoint.y - nextVertex.y)*(xSamPoint.y - nextVertex.y);
		if (dfSquareDeltaLast <= dfSquareTolerDelta)//
			insertpos.ciProcessType = 0;
		else if (dfSquareDeltaNext <= dfSquareTolerDelta)
			insertpos.ciProcessType = 1;
		else
			insertpos.ciProcessType = 2;
		//插入数据
		if (insertpos.ciProcessType==0)
			parrCirPolyVertexes[insertpos.index] = xSamPoint;
		else if(insertpos.ciProcessType == 1)
			parrCirPolyVertexes[(insertpos.index + 1) % nCurrCirPolyVertexesCount] = xSamPoint;
		else if(insertpos.ciProcessType == 2)
		{
			//TODO:判断插入点的索引，进行移位和插入
			short nInsertPosIndex = insertpos.index + 1;

			for (j = nCurrCirPolyVertexesCount; j > nInsertPosIndex; j--)
				parrCirPolyVertexes[j] = parrCirPolyVertexes[j - 1];
			parrCirPolyVertexes[nInsertPosIndex] = xSamPoint;
			nCurrCirPolyVertexesCount += 1;
		}
	}
	return nCurrCirPolyVertexesCount;
}
bool METASOLID::KAIHEINFO::CalMidFlatVertexes(COORD3D xarrVertexes[14]) const
{
	char ciUniKaiheWing=(this->ciOperLeg==0||this->ciOperLeg=='X'||this->ciOperLeg=='x')?'X':'Y';
	//局部开合实体每侧7个顶点
	//[0]楞线侧内壁(里皮)顶点;[1]开合肢翼尖里皮顶点;[2]开合肢翼尖外皮顶点;[3]开合角肢向起始外皮顶点1([2]沿肢法向偏移肢厚)
	//[4]开合肢向起始外皮顶点(楞点沿肢向偏移保留宽度);[5]开合角变形区起始点内皮点;[6]开合角变形区起始点外皮点
	double dfRootOffset=max(this->dfReservedWidth,dfLegThick);
	double dfWingExtendOffset=this->dfLegWidth-dfRootOffset;;
	GEPOINT vWingExtend,vWingNormal;
	if (ciUniKaiheWing=='X')
	{
		xarrVertexes[0].Set(dfRootOffset,this->dfLegThick,this->dfZlenStart);
		xarrVertexes[4].Set(dfRootOffset,0,this->dfZlenStart);
		xarrVertexes[5].Set(this->dfLegWidth,dfLegThick,this->dfZlenStart-this->dfDeformedLenOfStart);
		xarrVertexes[6].Set(this->dfLegWidth,		  0,this->dfZlenStart-this->dfDeformedLenOfStart);
		vWingExtend.x= cos(this->dfOpenRadAngle);
		vWingExtend.y=-sin(this->dfOpenRadAngle);
		vWingNormal.Set(vWingExtend.y,-vWingExtend.x);
	}
	else
	{
		xarrVertexes[0].Set(this->dfLegThick,dfRootOffset,this->dfZlenStart);
		xarrVertexes[4].Set(0,dfRootOffset,this->dfZlenStart);
		xarrVertexes[5].Set(this->dfLegThick,this->dfLegWidth,this->dfZlenStart-this->dfDeformedLenOfStart);
		xarrVertexes[6].Set(0				,this->dfLegWidth,this->dfZlenStart-this->dfDeformedLenOfStart);
		vWingExtend.y= cos(this->dfOpenRadAngle);
		vWingExtend.x=-sin(this->dfOpenRadAngle);
		vWingNormal.Set(-vWingExtend.y,vWingExtend.x);
	}
	xarrVertexes[1]=xarrVertexes[0]+vWingExtend*dfWingExtendOffset;
	xarrVertexes[2]=xarrVertexes[1]+vWingNormal*this->dfLegThick;
	xarrVertexes[3]=xarrVertexes[2]-vWingExtend*dfWingExtendOffset;
	//计算开合区末端关键点
	for (int i=0;i<5;i++)
		xarrVertexes[i+7].Set(xarrVertexes[i].x,xarrVertexes[i].y,this->dfZlenEnd);
	xarrVertexes[12]=xarrVertexes[5];
	xarrVertexes[13]=xarrVertexes[6];
	xarrVertexes[12].z+=this->dfZlenEnd-this->dfZlenStart+this->dfDeformedLenOfStart+this->dfDeformedLenOfEnd;
	xarrVertexes[13].z =xarrVertexes[12].z;
	return true;
}
bool METASOLID::KAIHEINFO::CalKaiheVertexes(COORD3D xarrVertexes[12]) const
{
	char ciUniKaiheWing=(this->ciOperLeg==0||this->ciOperLeg=='X'||this->ciOperLeg=='x')?'X':'Y';
	//局部开合实体每侧6个顶点
	//[0]楞线侧内壁(里皮)顶点;[1]开合肢翼尖里皮顶点;[2]开合肢翼尖外皮顶点;[3]开合角肢向起始外皮顶点(楞点沿肢向偏移保留宽度)
	//[4]开合角变形区起始点内皮点;[5];开合角变形区起始点外皮点
	double dfRootOffset=max(this->dfReservedWidth,dfLegThick);
	double dfOutrOffset=max(this->dfReservedWidth,dfLegThick);
	double dfWingExtendOffset=this->dfLegWidth-dfRootOffset;;
	double dfHalfOpenRadAngle=fabs(this->dfOpenRadAngle)*0.5;
	double dfIncreOffset=dfLegThick*tan(dfHalfOpenRadAngle);
	if (this->dfOpenRadAngle>0)	//开角时里皮侧保留区更长
		dfRootOffset+=dfIncreOffset;
	else	//合角时外皮侧保留区更长
		dfOutrOffset+=dfIncreOffset;
	GEPOINT vWingExtend,vWingNormal;
	if (ciUniKaiheWing=='X')
	{
		xarrVertexes[0].Set(dfRootOffset,this->dfLegThick,this->dfZlenStart);
		xarrVertexes[3].Set(dfOutrOffset,0,this->dfZlenStart);
		xarrVertexes[4].Set(this->dfLegWidth,dfLegThick,this->dfZlenStart-this->dfDeformedLenOfStart);
		xarrVertexes[5].Set(this->dfLegWidth,		  0,this->dfZlenStart-this->dfDeformedLenOfStart);
		vWingExtend.x= cos(this->dfOpenRadAngle);
		vWingExtend.y=-sin(this->dfOpenRadAngle);
		vWingNormal.Set(vWingExtend.y,-vWingExtend.x);
	}
	else
	{
		xarrVertexes[0].Set(this->dfLegThick,dfRootOffset,this->dfZlenStart);
		xarrVertexes[3].Set(0,dfOutrOffset,this->dfZlenStart);
		xarrVertexes[4].Set(this->dfLegThick,this->dfLegWidth,this->dfZlenStart-this->dfDeformedLenOfStart);
		xarrVertexes[5].Set(0				,this->dfLegWidth,this->dfZlenStart-this->dfDeformedLenOfStart);
		vWingExtend.y= cos(this->dfOpenRadAngle);
		vWingExtend.x=-sin(this->dfOpenRadAngle);
		vWingNormal.Set(-vWingExtend.y,vWingExtend.x);
	}
	if (this->dfOpenRadAngle>0)	//开角时里皮侧保留区更长
		xarrVertexes[1]=xarrVertexes[0]+vWingExtend*(dfWingExtendOffset+dfIncreOffset);
	else
		xarrVertexes[1]=xarrVertexes[0]+vWingExtend*dfWingExtendOffset;
	xarrVertexes[2]=xarrVertexes[1]+vWingNormal*this->dfLegThick;
	//计算开合区末端关键点
	for (int i=0;i<4;i++)
		xarrVertexes[i+6].Set(xarrVertexes[i].x,xarrVertexes[i].y,this->dfZlenEnd);
	xarrVertexes[10]=xarrVertexes[4];
	xarrVertexes[11]=xarrVertexes[5];
	xarrVertexes[10].z+=this->dfZlenEnd-this->dfZlenStart+this->dfDeformedLenOfStart+this->dfDeformedLenOfEnd;
	xarrVertexes[11].z =xarrVertexes[10].z;
	return true;
}
int METASOLID::AppendMidFlatAngleWing(fBody* pBody,GECS& acs,const KAIHEINFO* pDefKaiheInfo,
	COLORREF crLegPlane/*=0*/,COLORREF crTipEdge/*=0*/)
{
	long i,liInitIndex=pBody->vertex.GetNodeNum();
	//局部开合实体每侧7个顶点
	//[0]楞线侧内壁(里皮)顶点;[1]开合肢翼尖里皮顶点;[2]开合肢翼尖外皮顶点;[3]开合角肢向起始外皮顶点1([2]沿肢法向偏移肢厚)
	//[4]开合肢向起始外皮顶点(楞点沿肢向偏移肢厚);[5]开合角变形区起始点内皮点;[6]开合角变形区起始点外皮点
	GEPOINT xarrVertexes[14];
	if (!pDefKaiheInfo->CalKaiheVertexes(xarrVertexes))
		return liInitIndex;
	for(i=0;i<14;i++)
		pBody->vertex.append(xarrVertexes[i]);
	//初始化链表
	CCreateFace creator;
	creator.InitVertexList(pBody);
	f3dPolyFace *pFace;
	char ciUniKaiheWing=(pDefKaiheInfo->ciOperLeg==0||pDefKaiheInfo->ciOperLeg=='X'||pDefKaiheInfo->ciOperLeg=='x')?'X':'Y';
	if (ciUniKaiheWing=='X')
	{
		//开合区域外皮平面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+2,liInitIndex+3);
		creator.NewOutterEdgeLine(pFace,liInitIndex+9);
		creator.NewOutterEdgeLine(pFace,liInitIndex+10);
		creator.NewOutterEdgeLine(pFace,liInitIndex+3);
		//开合区域肢边平面
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+8,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+9);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		//开合区域楞线侧过渡制弯斜面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+3,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+10);
		creator.NewOutterEdgeLine(pFace,liInitIndex+11);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		//---------------------------起始端变形区域---------------------------
		//开合变形区制弯三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+6,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+3);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		//开合变形区外皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+3,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+6);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		//开合变形区里皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+1,liInitIndex+0);
		creator.NewOutterEdgeLine(pFace,liInitIndex+5);
		creator.NewOutterEdgeLine(pFace,liInitIndex+0);
		//开合变形区肢边三角平面对
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+5,liInitIndex+6);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+6);
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+1,liInitIndex+6);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+6);
	}
	else
	{
		//开合区域外皮平面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+3,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+10);
		creator.NewOutterEdgeLine(pFace,liInitIndex+9);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		//开合区域肢边平面
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+2,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+9);
		creator.NewOutterEdgeLine(pFace,liInitIndex+8);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		//开合区域楞线侧过渡制弯斜面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+4,liInitIndex+3);
		creator.NewOutterEdgeLine(pFace,liInitIndex+11);
		creator.NewOutterEdgeLine(pFace,liInitIndex+10);
		creator.NewOutterEdgeLine(pFace,liInitIndex+3);
		//---------------------------起始端变形区域---------------------------
		//开合变形区制弯三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+3,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+6);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		//开合变形区外皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+2,liInitIndex+3);
		creator.NewOutterEdgeLine(pFace,liInitIndex+6);
		creator.NewOutterEdgeLine(pFace,liInitIndex+3);
		//开合变形区里皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+0,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+5);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		//开合变形区肢边三角平面对
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+6,liInitIndex+5);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+5);
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+2,liInitIndex+6);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+6);
	}
	
	i=0;
	for(f3dPoint* pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext(),i++)
	{
		if(i<liInitIndex)
			continue;	//原有顶点
		GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
		pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
	}
	return pBody->vertex.GetNodeNum();
}
int METASOLID::AppendKaiheAngleWing(fBody* pBody,GECS& acs,const KAIHEINFO* pDefKaiheInfo,
	COLORREF crLegPlane/*=0*/,COLORREF crTipEdge/*=0*/)
{
	long i,liInitIndex=pBody->vertex.GetNodeNum();
	//局部开合实体每侧7个顶点
	//[0]楞线侧内壁(里皮)顶点;[1]开合肢翼尖里皮顶点;[2]开合肢翼尖外皮顶点;
	//[3]开合角肢向起始外皮顶点(楞点沿肢向偏移保留宽度);[4]开合角变形区起始点内皮点;[5]开合角变形区起始点外皮点
	GEPOINT xarrVertexes[12];
	if (!pDefKaiheInfo->CalKaiheVertexes(xarrVertexes))
		return liInitIndex;
	for(i=0;i<12;i++)
		pBody->vertex.append(xarrVertexes[i]);
	//初始化链表
	CCreateFace creator;
	creator.InitVertexList(pBody);
	f3dPolyFace *pFace;
	char ciUniKaiheWing=(pDefKaiheInfo->ciOperLeg==0||pDefKaiheInfo->ciOperLeg=='X'||pDefKaiheInfo->ciOperLeg=='x')?'X':'Y';
	if (ciUniKaiheWing=='X')
	{
		//开合区域外皮平面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+2,liInitIndex+3);
		creator.NewOutterEdgeLine(pFace,liInitIndex+8);
		creator.NewOutterEdgeLine(pFace,liInitIndex+9);
		creator.NewOutterEdgeLine(pFace,liInitIndex+3);
		//开合区域里皮平面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+0,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+6);
		creator.NewOutterEdgeLine(pFace,liInitIndex+7);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		//开合区域肢边平面
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+7,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+8);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		//---------------------------起始端变形区域---------------------------
		//开合变形区外皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+5,liInitIndex+3);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+3);
		//开合变形区里皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+1,liInitIndex+0);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+0);
		//开合变形区肢边三角平面对
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+2,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+5,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		//---------------------------终止端变形区域---------------------------
		//开合变形区外皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+5,liInitIndex+3);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+3);
		//开合变形区里皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+1,liInitIndex+0);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+0);
		//开合变形区肢边三角平面对
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+2,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+5,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
	}
	else
	{
		//开合区域外皮平面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+3,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+9);
		creator.NewOutterEdgeLine(pFace,liInitIndex+8);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		//开合区域里皮平面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+1,liInitIndex+0);
		creator.NewOutterEdgeLine(pFace,liInitIndex+7);
		creator.NewOutterEdgeLine(pFace,liInitIndex+6);
		creator.NewOutterEdgeLine(pFace,liInitIndex+0);
		//开合区域肢边平面
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+2,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+8);
		creator.NewOutterEdgeLine(pFace,liInitIndex+7);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		//---------------------------起始端变形区域---------------------------
		//开合变形区外皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+2,liInitIndex+3);
		creator.NewOutterEdgeLine(pFace,liInitIndex+5);
		creator.NewOutterEdgeLine(pFace,liInitIndex+3);
		//开合变形区里皮斜三角面
		pFace=pBody->faceList.append();
		pFace->material=crLegPlane;
		creator.NewOutterEdgeLine(pFace,liInitIndex+4,liInitIndex+0);
		creator.NewOutterEdgeLine(pFace,liInitIndex+1);
		creator.NewOutterEdgeLine(pFace,liInitIndex+0);
		//开合变形区肢边三角平面对
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+1,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+4);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
		pFace=pBody->faceList.append();
		pFace->material=crTipEdge;
		creator.NewOutterEdgeLine(pFace,liInitIndex+4,liInitIndex+2);
		creator.NewOutterEdgeLine(pFace,liInitIndex+5);
		creator.NewOutterEdgeLine(pFace,liInitIndex+2);
	}
	i=0;
	for(f3dPoint* pVertice=pBody->vertex.GetFirst();pVertice;pVertice=pBody->vertex.GetNext(),i++)
	{
		if(i<liInitIndex)
			continue;	//原有顶点
		GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
		pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
	}
	return pBody->vertex.GetNodeNum();
}
bool METASOLID::InitializeArcEdgeline(f3dAtomLine* pAtomLine,PROFVERTEX* pStart,PROFVERTEX* pEnd,bool bByStartVertex)
{
	PROFVERTEX* pDatumVertex=bByStartVertex?pStart:pEnd;
	GEPOINT worknorm(0,0,bByStartVertex?1:-1);
	if(pDatumVertex->ciEdgeType==0||pStart->pos.IsEqual(pEnd->pos))//防止因两点重合生成圆弧抛出异常
		return false;	//普通直边处理重合点
	else if(pDatumVertex->ciEdgeType==1)	//指定圆弧夹角
	{
		if(pDatumVertex->dfSectAngle>0)
			pAtomLine->CreateArcLine(worknorm, pDatumVertex->dfSectAngle);
		else
			pAtomLine->CreateArcLine(worknorm,-pDatumVertex->dfSectAngle);
	}
	else if(pDatumVertex->ciEdgeType==2)	//指定圆弧半径
	{
		int sign=1;
		if(pDatumVertex->dfArcRadius<0)
			sign=-1;
		pAtomLine->CreateArcLine2(worknorm*sign,sign*pDatumVertex->dfArcRadius,f3dPoint(pDatumVertex->cx,pDatumVertex->cy));
		//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
		//arcline.CreateMethod2(vertex,endPoint,f3dPoint(0,0,sign),sector_angle);
	}
	/*else if(pDatumVertex->ciEdgeType==3)	//椭圆弧
	{
		int sign=1;
		if(radius<0)
			sign=-1;
		f3dPoint columnNorm=column_norm;
		f3dPoint centerTemp=center;
		coord_trans(centerTemp,*ucs,FALSE);
		vector_trans(columnNorm,*ucs,FALSE);
		arcline.CreateEllipse(centerTemp,vertex,endPoint,columnNorm,f3dPoint(0,0,sign),sign*radius);
	}*/
	return true;
}
//生成由指定多边形（含圆弧边）拉伸形成的标准实体
int METASOLID::AppendPolygonExtrudeSolid(fBody* pBody,GECS& acs,double dfExtrudeLength,
		PROFVERTEX* xarrProfVertexes,int niCount,COLORREF crColor)
{
	int i,pushed=pBody->vertex.PushStack();
	long liInitVertexIndex=pBody->vertex.GetNodeNum();
	long liInitFaceIndex=pBody->faceList.GetNodeNum();
	f3dPoint* _arrVertexPointers[200];
	ARRAY_LIST<f3dPoint*> xarrVertexPointers(_arrVertexPointers,200,1);
	xarrVertexPointers.SetSize(niCount*2);
	for(i=0;i<niCount;i++)
		xarrVertexPointers[i]=pBody->vertex.append(xarrProfVertexes[i].pos);
	for(i=0;i<niCount;i++)
		xarrVertexPointers[i+niCount]=pBody->vertex.append(xarrProfVertexes[i].pos.x,xarrProfVertexes[i].pos.y,xarrProfVertexes[i].pos.z+dfExtrudeLength);

	f3dAtomLine* pEdgeLine;
	CXhSimpleList<f3dAtomLine*> listArcEdges;
	f3dPolyFace* pFace,*pTopFace,*pBtmFace;
	//生成顶面
	pFace=pTopFace=pBody->faceList.append();
	pFace->material=crColor;
	//pFace->poly_norm=acs.axis_z;
	for(i=0;i<niCount;i++)
	{
		pEdgeLine=pFace->outer_edge.append(xarrVertexPointers[i+niCount],xarrVertexPointers[(i+1)%niCount+niCount]);
		if(xarrProfVertexes[i].ciEdgeType>0)
		{
			listArcEdges.Append(pEdgeLine);
			InitializeArcEdgeline(pEdgeLine,&xarrProfVertexes[i],&xarrProfVertexes[(i+1)%niCount],true);
		}
	}
	//生成底面
	pFace=pBtmFace=pBody->faceList.append();
	pFace->material=crColor;
	//pFace->poly_norm=-acs.axis_z;
	for(i=niCount-1;i>=0;i--)
	{
		pEdgeLine=pFace->outer_edge.append(xarrVertexPointers[i],xarrVertexPointers[(i+niCount-1)%niCount]);
		if (xarrProfVertexes[(i + niCount - 1) % niCount].ciEdgeType > 0)
		{
			listArcEdges.Append(pEdgeLine);
			InitializeArcEdgeline(pEdgeLine, &xarrProfVertexes[i], &xarrProfVertexes[(i + niCount - 1) % niCount], false);
		}
	}
	//生成侧面
	for(i=0;i<niCount;i++)
	{
		if(xarrProfVertexes[i].blSkipEdge)
			continue;
		pFace=pBody->faceList.append();
		pFace->material=crColor;
		pEdgeLine=pFace->outer_edge.append(xarrVertexPointers[i],xarrVertexPointers[(i+1)%niCount]);
		if(xarrProfVertexes[i].ciEdgeType>0)
			InitializeArcEdgeline(pEdgeLine,&xarrProfVertexes[i],&xarrProfVertexes[(i+1)%niCount],true);
		pEdgeLine=pFace->outer_edge.append(xarrVertexPointers[(i+1)%niCount],xarrVertexPointers[(i+1)%niCount+niCount]);
		pEdgeLine=pFace->outer_edge.append(xarrVertexPointers[(i+1)%niCount+niCount],xarrVertexPointers[i+niCount]);
		if (xarrProfVertexes[(i + 1) % niCount].ciEdgeType > 0)
		{
			listArcEdges.Append(pEdgeLine);
			InitializeArcEdgeline(pEdgeLine, &xarrProfVertexes[i], &xarrProfVertexes[(i + 1) % niCount], false);
		}
		pEdgeLine=pFace->outer_edge.append(xarrVertexPointers[i+niCount],xarrVertexPointers[i]);
	}
	f3dPoint* pVertice;
	for(f3dAtomLine** ppArcEdge=listArcEdges.EnumObjectFirst();ppArcEdge;ppArcEdge=listArcEdges.EnumObjectNext())
		(*ppArcEdge)->CoordTrans(UCS_STRU(acs), TRUE, false);
	if(pBody->vertex.PopStack(pushed))
	{
		for(pVertice=pBody->vertex.GetNext();pVertice;pVertice=pBody->vertex.GetNext())
		{
			GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
			pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
		}
	}
	else
	{
		for(pVertice=pBody->vertex.GetFirst(),i=0;pVertice;pVertice=pBody->vertex.GetNext(),i++)
		{
			if(i<liInitVertexIndex)
				continue;	//原有顶点
			GEPOINT xActualPoint=acs.TransPFromCS(*pVertice);
			pVertice->Set(xActualPoint.x,xActualPoint.y,xActualPoint.z);
		}
	}
	return pBody->vertex.GetNodeNum();
}
BOOL METASOLID::AppendPolyHole(GECS &hole_ucs, PROFVERTEX* xarrVertex, int nVertexCount,
	f3dPolyFace *pUpFace, f3dPolyFace *pDownFace, fBody *pBody, COLORREF crLsHole)
{
	if (!normalize(hole_ucs.axis_z))	//非法的孔方向
		return FALSE;
	if (pUpFace == NULL || pDownFace == NULL || pBody == NULL)
		return FALSE;
	UCS_STRU upface_ucs, downface_ucs;
	f3dPoint inters_up, inters_down;
	typedef f3dPoint* POINT_PTR;
	int final_slices = nVertexCount;

	f3dPoint* scan_point_arr = new f3dPoint[final_slices];
	POINT_PTR *up_cir_vertex = new POINT_PTR[final_slices];
	POINT_PTR *down_cir_vertex = new POINT_PTR[final_slices];
	pUpFace->GetPolyUCS(upface_ucs);
	pDownFace->GetPolyUCS(downface_ucs);
	Int3dlf(inters_up, hole_ucs.origin, hole_ucs.axis_z, upface_ucs.origin, upface_ucs.axis_z);
	Int3dlf(inters_down, hole_ucs.origin, hole_ucs.axis_z, downface_ucs.origin, downface_ucs.axis_z);
	//生成孔的相对坐标系
	if (hole_ucs.axis_x.IsZero() || fabs(hole_ucs.axis_x*hole_ucs.axis_z) > EPS_COS)
		hole_ucs.axis_x = inters_vec(hole_ucs.axis_z);
	hole_ucs.axis_x -= (hole_ucs.axis_x*hole_ucs.axis_z)*hole_ucs.axis_z;
	normalize(hole_ucs.axis_x);
	hole_ucs.axis_y = hole_ucs.axis_z^hole_ucs.axis_x;
	normalize(hole_ucs.axis_y);
	f3dPoint scan_point;	//投影圆周扫描点
	BOOL bFlipHoleDirection = FALSE;
	if (hole_ucs.axis_z*upface_ucs.axis_z < 0)
		bFlipHoleDirection = TRUE;	//孔方向与面的上下方向相反，应颠倒孔方向
	int i;
	for (i = 0; i < nVertexCount; i++)
	{
		PROFVERTEX simpleVertex = xarrVertex[i];
		scan_point_arr[i]=simpleVertex.pos;
	}
	for (i = 0; i < final_slices; i++)
	{
		coord_trans(scan_point_arr[i], hole_ucs, TRUE);	//转为实际坐标系下的投影圆周扫描点
		f3dPoint *pUpPt = pBody->vertex.append(0, 0, 0);
		f3dPoint *pDownPt = pBody->vertex.append(0, 0, 0);
		Int3dlf(*pUpPt, scan_point_arr[i], hole_ucs.axis_z, upface_ucs.origin, upface_ucs.axis_z);
		Int3dlf(*pDownPt, scan_point_arr[i], hole_ucs.axis_z, downface_ucs.origin, downface_ucs.axis_z);
		up_cir_vertex[i] = pUpPt;
		down_cir_vertex[i] = pDownPt;
	}
	fLoop *pUpLoop = pUpFace->inner_loop.append();
	fLoop *pDownLoop = pDownFace->inner_loop.append();
	for (i = 0; i < final_slices; i++)
	{
		f3dPolyFace *pInnerFace = pBody->faceList.append();
		pInnerFace->material = crLsHole;
		if (bFlipHoleDirection)
		{
			pUpLoop->append(up_cir_vertex[i], up_cir_vertex[(i + 1) % final_slices]);
			if (i < final_slices - 1)
				pDownLoop->append(down_cir_vertex[final_slices - 1 - i], down_cir_vertex[final_slices - 2 - i]);
			else
				pDownLoop->append(down_cir_vertex[final_slices - 1 - i], down_cir_vertex[final_slices - 1]);

			pInnerFace->outer_edge.append(up_cir_vertex[(i + 1) % final_slices], up_cir_vertex[i]);
			pInnerFace->outer_edge.append(up_cir_vertex[i], down_cir_vertex[i]);
			pInnerFace->outer_edge.append(down_cir_vertex[i], down_cir_vertex[(i + 1) % final_slices]);
			pInnerFace->outer_edge.append(down_cir_vertex[(i + 1) % final_slices], up_cir_vertex[(i + 1) % final_slices]);
		}
		else
		{
			pDownLoop->append(down_cir_vertex[i], down_cir_vertex[(i + 1) % final_slices]);
			if (i < final_slices - 1)
				pUpLoop->append(up_cir_vertex[final_slices - 1 - i], up_cir_vertex[final_slices - 2 - i]);
			else
				pUpLoop->append(up_cir_vertex[final_slices - 1 - i], up_cir_vertex[final_slices - 1]);
			pInnerFace->outer_edge.append(up_cir_vertex[i], up_cir_vertex[(i + 1) % final_slices]);
			pInnerFace->outer_edge.append(up_cir_vertex[(i + 1) % final_slices], down_cir_vertex[(i + 1) % final_slices]);
			pInnerFace->outer_edge.append(down_cir_vertex[(i + 1) % final_slices], down_cir_vertex[i]);
			pInnerFace->outer_edge.append(down_cir_vertex[i], up_cir_vertex[i]);
		}
	}
	delete[]scan_point_arr;
	delete[]up_cir_vertex;
	delete[]down_cir_vertex;
	return TRUE;
}

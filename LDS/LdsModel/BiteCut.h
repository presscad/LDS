// BiteCut.h: interface for the CFlatBiteCut class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//通过一个不封闭的折线（TopFace法线方向由外向内看，逆时针遍历时左侧为待切割侧），
//在一个指定平面的侧边上咬切掉一个缺口
struct CFlatBiteCut
{
	struct INTERS_POINT{
		int index;	//该顶点对应的索引序号（可以是整个solid的顶点表索引，也可以是局部顶点数组的索引）
		GEPOINT position;
		INTERS_POINT(const double* pos_coords=NULL){
			index=-1;
			position=GEPOINT(pos_coords);
		}
	};
public:
	static bool Cut(GEPOINT* xarrCutPolyVertexes,int count,f3dPolyFace *pTopFace, f3dPolyFace *pBtmFace,fBody *pBody);
};

#include "stdafx.h"
#include "acedads.h"
#include "adscodes.h"
#include "list.h"
#include "f_alg_fun.h"
#include "ArrayList.h"
#include "SortFunc.h"
#include "SearchBlankPosition.h"


CSearchBlankPosition::SEARCH_CELL::SEARCH_CELL(int x/*=0*/,int y/*=0*/)
{
	xi=x;yj=y;
	weight=CalPriorWeight(x,y);
}
double CSearchBlankPosition::SEARCH_CELL::CalPriorWeight(int x,int y)
{
	double dx=x*fHoriPriorCoef;
	double dy=y*fVertPriorCoef;
	return (dx*dx+dy*dy);
}
double CSearchBlankPosition::SEARCH_CELL::fHoriPriorCoef=0.5;
double CSearchBlankPosition::SEARCH_CELL::fVertPriorCoef=0.5;
static int _LocalCompareSearchCell(const CSearchBlankPosition::SEARCH_CELL& cell1,const CSearchBlankPosition::SEARCH_CELL& cell2)
{
	if(cell1.weight<cell2.weight)
		return -1;
	else if(cell1.weight>cell2.weight)
		return 1;
	else if(cell1.xi>cell2.xi)
		return -1;	//权重相同时，优先X轴正向
	else if(cell1.xi<cell2.xi)
		return 1;
	else if(cell1.yj>cell2.yj)
		return -1;	//权重及X偏移相同时，优先Y轴正向
	else if(cell1.yj<cell2.yj)
		return 1;
	return 0;
}
CSearchBlankPosition::CSearchBlankPosition(void)
{
	m_pxUpdatePoint=NULL;
}


CSearchBlankPosition::~CSearchBlankPosition(void)
{
}
static resbuf verts4[4];
int CSearchBlankPosition::IsHasIntersObj(f3dPoint rgn_vert[4],const struct resbuf *filter/*=NULL*/)
{
	long i,ll;
	ads_name ss_name;
	//struct resbuf *plist;
	for(i=0;i<4;i++)
	{
		verts4[i].restype=5002;
		verts4[i].resval.rpoint[X] = rgn_vert[i].x;
		verts4[i].resval.rpoint[Y] = rgn_vert[i].y;
		verts4[i].resval.rpoint[Z] = rgn_vert[i].z;
		if(i<3)
			verts4[i].rbnext=&verts4[i+1];
		else
			verts4[3].rbnext=NULL;
	}
	//plist=acutBuildList(RTPOINT,ads_vert[0],RTPOINT,ads_vert[1],RTPOINT,ads_vert[2],RTPOINT,ads_vert[3],RTNONE);
	
#if defined(_ARX_2007)&&!defined(_ZRX_2012)
	if (acedSSGet(L"cp",&verts4[0],NULL,filter,ss_name)==RTNORM)
#else
	if (acedSSGet("cp",&verts4[0],NULL,filter,ss_name)==RTNORM)
#endif
	{
		acedSSLength(ss_name,&ll);
		acedSSFree(ss_name);
		return ll;
	}
	else
	{
		acedSSFree(ss_name);
		return 0;
	}
}
bool CSearchBlankPosition::SearchNoneIntersPos(f3dPoint rgnSrcVert[4],const double* vcSearchFlowDirection,
	double maxHoriSearchDist/*=5.0*/,double maxVertSearchDist/*=5*/,
	double fHoriPriorCoef/*=0.5*/,double fVertPriorCoef/*=0.5*/,char cbDualDirectMode/*=0x0f*/,void *pExcusiveSelfDrawObj/*=NULL*/)
{
	const BYTE DIRECT_X			=0x01;
	const BYTE DIRECT_X_INVERSE =0x02;
	const BYTE DIRECT_Y			=0x04;
	const BYTE DIRECT_Y_INVERSE =0x08;
	int i,j;
	const struct resbuf *filter=NULL;
	//ll=JustifyInters(vert,filter);
	SEARCH_CELL::fHoriPriorCoef=fHoriPriorCoef>EPS?1/fHoriPriorCoef:1e10;
	SEARCH_CELL::fVertPriorCoef=fVertPriorCoef>EPS?1/fVertPriorCoef:1e10;
	CXhSimpleList<SEARCH_CELL> listCells;
	for(i=0;i<=maxHoriSearchDist;i++)
	{
		for(j=0;j<=maxVertSearchDist;j++)
		{
			if(i==0&&j==0)
				continue;
			if(i==0||(i>=0&&(cbDualDirectMode&DIRECT_X)))
			{
				if(j==0||(j>=0&&(cbDualDirectMode&DIRECT_Y)))
					listCells.AttachObject(SEARCH_CELL(i,j));
				if(j>0&&(cbDualDirectMode&DIRECT_Y_INVERSE))
					listCells.AttachObject(SEARCH_CELL(i,-j));
			}
			if(i>0&&(cbDualDirectMode&DIRECT_X_INVERSE))
			{
				if(j==0||(j>=0&&(cbDualDirectMode&DIRECT_Y)))
					listCells.AttachObject(SEARCH_CELL(-i,j));
				if(j>0&&(cbDualDirectMode&DIRECT_Y_INVERSE))
					listCells.AttachObject(SEARCH_CELL(-i,-j));
			}
		}
	}
	i=0;
	DYN_ARRAY<SEARCH_CELL> cells(listCells.Count);
	for(SEARCH_CELL* pCell=listCells.EnumObjectFirst();pCell;pCell=listCells.EnumObjectNext(),i++)
		cells[i]=*pCell;
	CQuickSort<SEARCH_CELL>::QuickSort(cells,cells.Size(),_LocalCompareSearchCell);
	f3dPoint origin,rgnVert[4];
	GEPOINT horiSearchVec(vcSearchFlowDirection[0],vcSearchFlowDirection[1]);	//横向搜索方向
	normalize(horiSearchVec);
	GEPOINT vertSearchVec(-horiSearchVec.y,horiSearchVec.x);;
	for(i=0;i<(int)cells.Size();i++)
	{
		origin=cells[i].xi*horiSearchVec+cells[i].yj*vertSearchVec;
		rgnVert[0]=rgnSrcVert[0]+origin;
		rgnVert[1]=rgnSrcVert[1]+origin;
		rgnVert[2]=rgnSrcVert[2]+origin;
		rgnVert[3]=rgnSrcVert[3]+origin;
		
		if(IsHasIntersObj(rgnVert,filter))
			continue;
		else
		{
			if(this->m_pxUpdatePoint)
			{
				GEPOINT xOffset=(rgnVert[0]+rgnVert[2]-rgnSrcVert[0]-rgnSrcVert[2])*0.5;
				m_pxUpdatePoint[0]+=xOffset.x;
				m_pxUpdatePoint[1]+=xOffset.y;
				m_pxUpdatePoint[2]+=xOffset.z;
			}
			rgnSrcVert[0]=rgnVert[0];
			rgnSrcVert[1]=rgnVert[1];
			rgnSrcVert[2]=rgnVert[2];
			rgnSrcVert[3]=rgnVert[3];
			return true;
		}
	}
	return false;
}

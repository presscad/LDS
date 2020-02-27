#include "stdafx.h"
#include "PreviewObject.h"
#include "f_alg_fun.h"

//////////////////////////////////////////////////////////////////////////////
int CPreviewObject::PIXEL::Compare(const ICompareClass *pCompareObj)const
{
	PIXEL* pComparePixel=(PIXEL*)pCompareObj;
	if(square<pComparePixel->square)
		return  1;	//距离近的排序到搜索队列后面
	else if(square>pComparePixel->square)
		return -1;	//距离远的排序到搜索队列前面
	else
	{	//暂还不需要在等径位置时按逆时针排序
		return 0;
	}
}
CPreviewObject::SEARCH_CELL::SEARCH_CELL(int x/*=0*/,int y/*=0*/)
{
	xi=x;yj=y;
	weight=CalPriorWeight(x,y);
}
double CPreviewObject::SEARCH_CELL::CalPriorWeight(int x,int y)
{
	double dx=x*fHoriPriorCoef;
	double dy=y*fVertPriorCoef;
	return (dx*dx+dy*dy);
}
double CPreviewObject::SEARCH_CELL::fHoriPriorCoef=0.5;
double CPreviewObject::SEARCH_CELL::fVertPriorCoef=0.5;
static int _LocalCompareSearchCell(const CPreviewObject::SEARCH_CELL& cell1,const CPreviewObject::SEARCH_CELL& cell2)
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
//////////////////////////////////////////////////////////////////////////////
CPreviewObject::CPreviewObject(long serial)
{
	m_idSerial=serial;
	m_fImgOrgX=m_fImgOrgY=0;
}


CPreviewObject::~CPreviewObject(void)
{
}

void CPreviewObject::SetOCS(GECS& _ocs,double scaleOfView2Model/*=0.01*/)
{
	ocs=_ocs;
	m_fScaleOfView2Model=scaleOfView2Model;
}
void CPreviewObject::SetViewport(int vox,int voy,int _width,int _height)
{
	m_fImgOrgX=vox;
	m_fImgOrgY=voy;
	int width =_width;
	int height=_height;
}
void CPreviewObject::_InternalDrawLine(const double* start,const double* end,long hRelaRod)
{	//start,end为ocs下局部坐标
	//int pixel_sx=start[0]-m_fImgOrgX;
	image.DrawLine(start,end,hRelaRod);
}
void CPreviewObject::_InternalDrawPoint(const double* point,long hRelaNode)
{	//point为ocs下局部坐标
	image.DrawPoint(point,hRelaNode);
}
void CPreviewObject::DrawLine(const double* start,const double* end,long hRelaRod/*=0*/)
{
	BUFF_LINE line;
	line.start=ocs.TransPToCS(start);
	line.end=ocs.TransPToCS(end);
	line.start*=m_fScaleOfView2Model;
	line.end*=m_fScaleOfView2Model;
	GEPOINT imgstart(line.start.x-m_fImgOrgX,line.start.y-m_fImgOrgY);
	GEPOINT imgend(line.end.x-m_fImgOrgX,line.end.y-m_fImgOrgY);
	_InternalDrawLine(imgstart,imgend,hRelaRod);
}
void CPreviewObject::DrawPoint(const double* point,long hRelaNode/*=0*/)
{
	GEPOINT localPoint=ocs.TransPToCS(point);
	localPoint*=m_fScaleOfView2Model;
	GEPOINT imgpoint(localPoint.x-m_fImgOrgX,localPoint.y-m_fImgOrgY);
	_InternalDrawPoint(imgpoint,hRelaNode);
}
void CPreviewObject::DrawPlacePolygon(GEPOINT* lpPoints,int count)	//绘制占位符
{
}
GEPOINT CPreviewObject::TransPToOcs(const double* modelpoint)
{
	GEPOINT point(modelpoint[0]*m_fScaleOfView2Model,modelpoint[1]*m_fScaleOfView2Model,modelpoint[2]*m_fScaleOfView2Model);
	return ocs.TransPToCS(point);
}
GEPOINT CPreviewObject::TransPFromOcs(double xiView,double yiView,double ziView/*=0*/)
{
	GEPOINT point(xiView/m_fScaleOfView2Model,yiView/m_fScaleOfView2Model,ziView/m_fScaleOfView2Model);
	return ocs.TransPFromCS(point);
}
void CPreviewObject::AppendLine(const double* start,const double* end,long hRelaRod/*=0*/)
{
	BUFF_LINE* pLine=listBuffLines.Append();
	pLine->start=ocs.TransPToCS(start);
	pLine->end=ocs.TransPToCS(end);
	pLine->start*=m_fScaleOfView2Model;
	pLine->end*=m_fScaleOfView2Model;
	pLine->hRelaRod=hRelaRod;
}
void CPreviewObject::AppendPoint(const double* point,long hRelaNode/*=0*/)
{
	BUFF_POINT* pPoint=listBuffPoints.Append();
	pPoint->point=ocs.TransPToCS(point);
	pPoint->point*=m_fScaleOfView2Model;
	pPoint->hRelaNode=hRelaNode;
}
void CPreviewObject::FlushPreview(short siMarginTop/*=20*/,short siMarginLeft/*=20*/,short siMarginRight/*=20*/,short siMarginBtm/*=20*/)
{
	BUFF_POINT* pPoint;
	BUFF_LINE* pLine;
	if(image.Width==0)
	{	//初始化图像绘制范围
		SCOPE_STRU scope;
		for(pLine=listBuffLines.EnumObjectFirst();pLine;pLine=listBuffLines.EnumObjectNext())
		{
			scope.VerifyVertex(pLine->start);
			scope.VerifyVertex(pLine->end);
		}
		for(pPoint=listBuffPoints.EnumObjectFirst();pPoint;pPoint=listBuffPoints.EnumObjectNext())
			scope.VerifyVertex(pPoint->point);
		int width=1,height=1;
		if(scope.wide()>0)
		{
			m_fImgOrgX=scope.fMinX-siMarginLeft;
			m_fImgOrgY=scope.fMinY-siMarginBtm;
			width =ftoi(scope.fMaxX-scope.fMinX)+1+siMarginLeft+siMarginRight;
			height=ftoi(scope.fMaxY-scope.fMinY)+1+siMarginTop+siMarginBtm;
		}
		image.InitBitImage(NULL,width,height);
		arrRelaObjs.Resize(image.GetImageWidth()*image.GetImageHeight());
		image.SetRelaObjMap(arrRelaObjs);
	}
	for(pLine=listBuffLines.EnumObjectFirst();pLine;pLine=listBuffLines.EnumObjectNext())
	{
		GEPOINT imgstart(pLine->start.x-m_fImgOrgX,pLine->start.y-m_fImgOrgY);
		GEPOINT imgend(pLine->end.x-m_fImgOrgX,pLine->end.y-m_fImgOrgY);
		_InternalDrawLine(imgstart,imgend,pLine->hRelaRod);
	}
	for(pPoint=listBuffPoints.EnumObjectFirst();pPoint;pPoint=listBuffPoints.EnumObjectNext())
	{
		GEPOINT imgpoint(pPoint->point.x-m_fImgOrgX,pPoint->point.y-m_fImgOrgY);
		_InternalDrawPoint(imgpoint,pPoint->hRelaNode);
	}
	listBuffLines.DeleteList();
	listBuffPoints.DeleteList();
}
bool CPreviewObject::IsHasBlackPixel(ISearchRgn *pSearchRgn,int offset_x,int offset_y,PIXEL* pixels/*=NULL*/,UINT count/*=0*/)
{
	ARRAY_LIST<PIXEL> searchpixels;
	int i,j;
	if(pSearchRgn->ciRgnType==1)	//RGN_CIR
	{
		RGN_CIR* pCir=(RGN_CIR*)pSearchRgn;
		searchpixels.SetSize(0,pCir->r*pCir->r*4);
		if(count==0)
		{
			int r_2=pCir->r*pCir->r;
			int minr_2=pCir->wiRgnSearchMode>pCir->r?0:(pCir->r-pCir->wiRgnSearchMode)*(pCir->r-pCir->wiRgnSearchMode);
			for(i=pCir->r;i>=0;i--)
			{
				for(j=pCir->r;j>=0;j--)
				{
					int square=i*i+j*j;
					if(square>r_2||square<minr_2)
						continue;
					searchpixels.append(PIXEL(i,j));
					if(i!=0)
						searchpixels.append(PIXEL(-i, j));
					if(j!=0)
						searchpixels.append(PIXEL( i,-j));
					if(i!=0&&j!=0)
						searchpixels.append(PIXEL(-i,-j));
				}
			}
			count=searchpixels.Size();
			pixels=searchpixels.m_pData;
			CQuickSort<PIXEL>::QuickSortClassic(pixels,count);
			for(i=0;i<(int)count;i++)
			{
				pixels[i].X+=pCir->cx+offset_x;
				pixels[i].Y+=pCir->cy+offset_y;
			}
		}
	}
	else if(pSearchRgn->ciRgnType==2)	//RGN_RECT
	{
		RGN_RECT* pRc=(RGN_RECT*)pSearchRgn;
		if(count==0)
		{
			count=(pRc->w+1)*(pRc->h+1);
			searchpixels.SetSize(0,count);
			short cx=pRc->w/2,cy=pRc->h/2;
			for(i=0;i<=pRc->w;i++)
			{
				for(j=0;j<=pRc->h;j++)
					searchpixels.append(PIXEL(i-cx,j-cy));
			}
			pixels=searchpixels.m_pData;
			CQuickSort<PIXEL>::QuickSortClassic(pixels,count);
			for(i=0;i<(int)count;i++)
			{
				pixels[i].X+=cx+pRc->ox+offset_x;
				pixels[i].Y+=cy+pRc->oy+offset_y;
			}
		}
	}
	else
		return false;
	for(UINT index=0;index<count;index++)
	{
		if(image.IsBlackPixel(pixels[index].X,pixels[index].Y))
			return true;
	}
	return false;
}
bool CPreviewObject::SearchBlankRegion(ISearchRgn* pRgnPlace,SEARCH* pSearchRule/*=NULL*/)
{
	SEARCH search;
	if(pSearchRule!=NULL)
		search=*pSearchRule;
	int i,j;
	SEARCH_CELL::fHoriPriorCoef=search.fHoriPriorCoef>EPS?1/search.fHoriPriorCoef:1e10;
	SEARCH_CELL::fVertPriorCoef=search.fVertPriorCoef>EPS?1/search.fVertPriorCoef:1e10;
	CXhSimpleList<SEARCH_CELL> listCells;
	for(i=0;i<=search.maxHoriSearchDist;i++)
	{
		for(j=0;j<=search.maxVertSearchDist;j++)
		{
			if(i==0&&j==0)
				continue;
			if(i==0||(i>=0&&(search.cbDualDirectMode&SEARCH::DIRECT_X)))
			{
				if(j==0||(j>=0&&(search.cbDualDirectMode&SEARCH::DIRECT_Y)))
					listCells.AttachObject(SEARCH_CELL(i,j));
				if(j>0&&(search.cbDualDirectMode&SEARCH::DIRECT_Y_INVERSE))
					listCells.AttachObject(SEARCH_CELL(i,-j));
			}
			if(i>0&&(search.cbDualDirectMode&SEARCH::DIRECT_X_INVERSE))
			{
				if(j==0||(j>=0&&(search.cbDualDirectMode&SEARCH::DIRECT_Y)))
					listCells.AttachObject(SEARCH_CELL(-i,j));
				if(j>0&&(search.cbDualDirectMode&SEARCH::DIRECT_Y_INVERSE))
					listCells.AttachObject(SEARCH_CELL(-i,-j));
			}
		}
	}
	i=0;
	DYN_ARRAY<SEARCH_CELL> cells(listCells.Count);
	for(SEARCH_CELL* pCell=listCells.EnumObjectFirst();pCell;pCell=listCells.EnumObjectNext(),i++)
		cells[i]=*pCell;
	CHeapSort<SEARCH_CELL>::HeapSort(cells,cells.Size(),_LocalCompareSearchCell);
	f3dPoint origin,rgnVert[4];
	GEPOINT horiSearchVec(search.vhSearchFlowDirection.x,search.vhSearchFlowDirection.y);	//横向搜索方向
	normalize(horiSearchVec);
	GEPOINT vertSearchVec(-horiSearchVec.y,horiSearchVec.x);;
	for(i=0;i<(int)cells.Size();i++)
	{
		origin=cells[i].xi*horiSearchVec+cells[i].yj*vertSearchVec;
		origin.x-=this->m_fImgOrgX;
		origin.y-=this->m_fImgOrgY;
		if(IsHasBlackPixel(pRgnPlace,ftoi(origin.x),ftoi(origin.y)))
			continue;
		else
		{
			pRgnPlace->Offset(ftoi(origin.x+m_fImgOrgX),ftoi(origin.y+m_fImgOrgY));
			return true;
		}
	}
	return false;
}

CPreviewObject::PIXEL_XY SCANPIXELS_R2[16]={
	{ 2, 0},
	{ 2, 1}, 
	{ 1, 1}, 
	{ 1, 2}, 
	{ 0, 2}, 
	{-1, 2}, 
	{-1, 1}, 
	{-2, 1}, 
	{-2, 0}, 
	{-2,-1}, 
	{-1,-1}, 
	{-1,-2}, 
	{ 0,-2}, 
	{ 1,-2}, 
	{ 1,-1}, 
	{ 2,-1} 
};
bool IsColinear(double vx,double vy,double vx2,double vy2)
{
	double vbase=fabs(vx)+fabs(vy),vbase2=fabs(vx2)+fabs(vy2);
	if(vbase==0||vbase2==0)
		return true;
	if(fabs(vx)>=fabs(vy)&&fabs(vx2)>EPS&&fabs(vy/vx-vy2/vx2)<EPS)
		return true;
	else if(fabs(vx)<fabs(vy)&&fabs(vy2)>EPS&&fabs(vx/vy-vx2/vy2)<EPS)
		return true;
	else
		return false;
}
int CPreviewObject::GetRotScanPixels(int cx,int cy, int toX,int toY,PIXEL* scanpixels,int count/*=0*/)
{
	int x=toX-cx,y=toY-cy;
	double len=abs(x)+abs(y);
	if(x==0&&y==0)
		return 0;
	double vx=-x,vy=-y;	//由边界点指向孤岛中心点
	short i,index=0,siBaseEdgeIndex = 0;
	if(vx>0&&vy==0)
		siBaseEdgeIndex=15;
	else if(vx<0&&vy==0)
		siBaseEdgeIndex=7;
	else
	{
		for(i=1;i<16;i++)
		{
			if(IsColinear(vx,vy,SCANPIXELS_R2[i].X,SCANPIXELS_R2[i].Y))
			{
				if(vx*SCANPIXELS_R2[i].X+vy*SCANPIXELS_R2[i].Y>0)
				{	//同向共线，夹角相同
					siBaseEdgeIndex=i-1;
					break;
				}
				else if(vy>0)
					continue;	//反向共线，当前线位于Y轴正向，表明夹角小于指定方向(vx,vy)180度
				//else if(vy<=0)
				//	logerr.Log("不会出现");
			}
			else
			{

				if(vy<0&&SCANPIXELS_R2[i].Y>0)
					continue;	//(vx,vy)在3,4象限夹角小于SCANPIXELS_R2[i]（在1,2象限)
				else if(vy>0&&SCANPIXELS_R2[i].Y<0)
					siBaseEdgeIndex=i-1;
				else if(vx*SCANPIXELS_R2[i].Y-vy*SCANPIXELS_R2[i].X>0)
					siBaseEdgeIndex=i-1;	//(vx,vy)夹角小于(SCANPIXELS_R2[i].X,SCANPIXELS_R2[i].Y)
				else //if(vx*SCANPIXELS_R2[i].Y-vy*SCANPIXELS_R2[i].X<0)
					continue;	//(vx,vy)夹角大于(SCANPIXELS_R2[i].X,SCANPIXELS_R2[i].Y)
				break;
			}
		}
		if(i==16)
			siBaseEdgeIndex=15;
	}

	vx=x;
	vy=y;
	double stepx,stepy;
	if(fabs(vx)>fabs(vy))
	{
		stepx=vx>0?1:-1;
		stepy=(vy/vx)*stepx;
	}
	else //if(fabs(vx)<=fabs(vy))
	{
		stepy=vy>0?1:-1;
		stepx=(vx/vy)*stepy;
	}
	GEPOINT2D vn(-y,x);
	double dx=-4*stepx,dy=-4*stepy;
	PRESET_ARRAY32<PIXEL> pixels;
	for(i=0,index=0;i<4;i++){
		int dxI=ftoi(dx),dyJ=ftoi(dy);
		dx+=stepx;
		dy+=stepy;
		if((toX+dxI-cx)*x+(toY+dyJ-cy)*y<0)
			continue;	//延射线回退也不能超过中心点
		double vx=(dxI*x+dyJ*y)/len;	//搜索半径为2（以防出现斜线像素中的钜齿效应）
		double vy=(dxI*vn.x+dyJ*vn.y)/len;
		if(vy<0||(vy<EPS&&vx>0))	//扫描角0<=alfa<180
			continue;
		pixels.Append(PIXEL(dxI,dyJ));
		index++;
	}
	short count2=min(16,count-index);
	for(i=0;i<count2;i++)
	{
		short indicator=(siBaseEdgeIndex-i+16)%16;
		PIXEL px=SCANPIXELS_R2[indicator];
		double vx=(px.X*x+px.Y*y)/len;	//搜索半径为2（以防出现斜线像素中的钜齿效应）
		double vy=(px.X*vn.x+px.Y*vn.y)/len;
		if(vy<EPS)
		{
			if(vx<0)	//扫描角0<=alfa<180
				continue;
			else if(vx>0)
				break;
		}
		pixels.Append(px);
	}
	int validcount=min(pixels.Count,pixels.CountOfPreset);
	if(count>validcount)
		count=validcount;
	//memcpy(scanpixels,pixels,sizeof(PIXEL)*count);
	return count;
}
struct PIXELQUEUE{
private:
	short indicator;
	CPreviewObject::PIXEL scanpixels[8];
public:
	PIXELQUEUE(){indicator=0;}
	short AddQueue(int xI,int yJ)
	{
		if(scanpixels[indicator].X!=0||scanpixels[indicator].Y!=0)
			indicator=(indicator+1)%8;
		scanpixels[indicator]=CPreviewObject::PIXEL(xI,yJ);
		return indicator;
	}
	bool CheckQueue(int xI,int yJ)
	{
		for(int i=0;i<8;i++)
		{
			int index=indicator-i;
			if(index<0)
				index+=8;
			if(scanpixels[index].X==xI&&scanpixels[index].Y==yJ)
				return true;//已处理过的像素，直接跳过
			if(scanpixels[index].X==0&&scanpixels[index].Y==0)
				break;	//空元素
		}
		return false;
	}
};
int CPreviewObject::SearchIslands(CXhSimpleList<ISLAND>* pListIslands/*=NULL*/,WORD wiMinIslandPixels/*=0*/)
{
	CXhSimpleList<CByteMonoImage::ISLAND> listIslands;
	int i,j,count=image.DetectIslands(&listIslands);
	for(CByteMonoImage::ISLAND* pIsland=listIslands.EnumObjectFirst();pIsland;pIsland=listIslands.EnumObjectNext())
	{
		if(pIsland->count<wiMinIslandPixels)
			continue;
		int cx=ftoi(pIsland->x/pIsland->count);
		int cy=ftoi(pIsland->y/pIsland->count);
		ISLAND island;
		PIXEL_RELAOBJ* pRelaObj=NULL;
		for(i=cx;i<image.Width;i++)
		{
			if((pRelaObj=image.GetPixelRelaObj(i,cy))!=NULL)
				break;
		}
		if(pRelaObj==NULL)
			continue;
		PIXEL scanpixels[16];
		int currRodI=0,xCurrI=i,yCurrJ=cy,pixelcount=0,hits=1,iter_count=0;
		int MIN_RODLINE_PIXEL_HITS=3;	//每根杆件简化线最少的有效扫描像素数
		PIXELQUEUE queue;	//已处理像素队列，用以防止就近像素的反复扫描循环
		queue.AddQueue(xCurrI,yCurrJ);
		long hCurrScaningRod=pRelaObj->hRod;
		do{
			int xPrevRayI=xCurrI-cx,yPrevRayJ=yCurrJ-cy;
			pixelcount=GetRotScanPixels(cx,cy,xCurrI,yCurrJ,scanpixels,16);
			for(j=0,pRelaObj=NULL;j<pixelcount&&pRelaObj==NULL;j++)
			{
				if(scanpixels[j].X==0&&scanpixels[j].Y==0)
					continue;	//
				int xRayI=xCurrI+scanpixels[j].X-cx,yRayJ=yCurrJ+scanpixels[j].Y-cy;
				if( fabs(xPrevRayI/(double)(xPrevRayI+yPrevRayJ)-xRayI/(double)(xRayI+yRayJ))>EPS&&	//非共线
					(xPrevRayI*yRayJ-yPrevRayJ*xRayI)<0)
					continue;	//只能逆时针旋转扫描不允许反向扫描
				if(queue.CheckQueue(xCurrI+scanpixels[j].X,yCurrJ+scanpixels[j].Y))
				{
					j=max(j,3);	//一旦发现该像素已扫描过，就跳过前4个射线像素
					continue;//该像素最近已扫描过
				}
				if((pRelaObj=image.GetPixelRelaObj(xCurrI+scanpixels[j].X,yCurrJ+scanpixels[j].Y))!=NULL)
					break;
			}
			if(pRelaObj==NULL)
			{
				island.arrayEdgeRods[0]=island.arrayEdgeRods[1]=island.arrayEdgeRods[2]=0;
				break;	//确实找不到，很罕见需要具体分析
			}
			else
			{
				xCurrI+=scanpixels[j].X;
				yCurrJ+=scanpixels[j].Y;
			}
			queue.AddQueue(xCurrI,yCurrJ);
			if(pRelaObj->hRod==hCurrScaningRod)
				hits++;
			else if(pRelaObj->hRod!=hCurrScaningRod)
			{
				if(hits>=MIN_RODLINE_PIXEL_HITS)	//有效命中边界杆件
				{
					iter_count=0;
					if(island.arrayEdgeRods[currRodI]!=0&&hCurrScaningRod!=island.arrayEdgeRods[currRodI])
						currRodI++;
					island.arrayEdgeRods[currRodI]=hCurrScaningRod;
				}
				hits=1;
				hCurrScaningRod=pRelaObj->hRod;
				if(hCurrScaningRod==island.arrayEdgeRods[0]||currRodI==7)
				{
					if(currRodI<2||currRodI>=7)	//确实找不到或超出数量异常，很罕见需要具体分析
						island.arrayEdgeRods[0]=island.arrayEdgeRods[1]=island.arrayEdgeRods[2]=0;
					break;
				}
				iter_count++;	//iter_count<10，防止陷入死锁状态
			}
		}while(iter_count<10);

		island.count=pIsland->count;
		island.cx=(short)ftoi(pIsland->x/pIsland->count+m_fImgOrgX);
		island.cy=(short)ftoi(pIsland->y/pIsland->count+m_fImgOrgY);
		island.miny=(WORD)(pIsland->miny+m_fImgOrgY);
		island.maxy=(WORD)(pIsland->maxy+m_fImgOrgY);
		pListIslands->Append(island);
	}
	return count;
}

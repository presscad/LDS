#pragma once
#include "list.h"
#include "IPreview.h"
#include "MonoImage.h"
#include "SortFunc.h"

class CPreviewObject : public IPreview
{
protected:
	long m_idSerial;
	GECS ocs;
	double m_fImgOrgX,m_fImgOrgY;
	double m_fScaleOfView2Model;
	CByteMonoImage image;
	struct BUFF_LINE{
		long hRelaRod;
		GEPOINT start,end;
	};
	DYN_ARRAY<PIXEL_RELAOBJ> arrRelaObjs;
	struct BUFF_POINT{long hRelaNode;GEPOINT point;};
	CXhSimpleList<BUFF_LINE>  listBuffLines;
	CXhSimpleList<BUFF_POINT>listBuffPoints;
public:
	struct PIXEL_XY{
		short X,Y;
	};
	struct PIXEL : public ICompareClass{
		short X,Y;
		int square;	//X*X+Y*Y,用于判断距中心点距离，排序时用
		PIXEL(short _x=0,short _y=0){X=_x;Y=_y;square=X*X+Y*Y;}
		PIXEL(PIXEL_XY pixel){X=pixel.X;Y=pixel.Y;square=X*X+Y*Y;}
		virtual int Compare(const ICompareClass *pCompareObj)const;
	};
	struct SEARCH_CELL{
		int xi,yj;
		double weight;//搜索优先权重，越小越优先
		static double fHoriPriorCoef;
		static double fVertPriorCoef;
	public:
		SEARCH_CELL(int x=0,int y=0);
		double CalPriorWeight(int x,int y);
	};
protected:
	bool IsHasBlackPixel(ISearchRgn *pSearchRgn,int offset_x,int offset_y,PIXEL* pixels=NULL,UINT count=0);
	void _InternalDrawLine(const double* start,const double* end,long hRelaRod=0);//start,end为ocs下局部坐标
	void _InternalDrawPoint(const double* point,long hRelaNode=0);//point为ocs下局部坐标
public:
	virtual long Serial(){return m_idSerial;}
	virtual void SetOCS(GECS& ocs,double scaleOfView2Model=0.01);
	virtual void SetViewport(int vox,int voy,int width,int height);//vox,voy为ocs坐标系下缩放后图形显示视口的原点位置
	virtual void DrawLine(const double* start,const double* end,long hRelaRod=0);
	virtual void DrawPoint(const double* point,long hRelaNode=0);
	virtual void DrawPlacePolygon(GEPOINT* lpPoints,int count);	//绘制占位符
	virtual GEPOINT TransPToOcs(const double* modelpoint);
	virtual GEPOINT TransPFromOcs(double xiView,double yiView,double ziView=0);
	virtual double ScaleOfView2Model(){return m_fScaleOfView2Model;}
	virtual int WriteMonoBmpFile(const char* fileName){return image.WriteMonoBmpFile(fileName);}
public:
	virtual void AppendLine(const double* start,const double* end,long hRelaRod=0);
	virtual void AppendPoint(const double* point,long hRelaNode=0);
	virtual void FlushPreview(short siMarginTop=20,short siMarginLeft=20,short siMarginRight=20,short siMarginBtm=20);
	virtual UINT GetImageWidth(){return image.GetImageWidth();}
	virtual UINT GetImageHeight(){return image.GetImageHeight();}
public:
	virtual bool SearchBlankRegion(ISearchRgn* pRgnPlace,SEARCH* pSearchRule=NULL);
public:
	int GetRotScanPixels(int cx,int cy, int toX,int toY,PIXEL* scanpixels,int count=0);
	virtual int SearchIslands(CXhSimpleList<ISLAND>* pListIslands=NULL,WORD wiMinIslandPixels=0);
public:
	CPreviewObject(long serial);
	~CPreviewObject();
};


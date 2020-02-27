#pragma once
#include "f_ent.h"
#include "list.h"

struct IPreview
{
	static IPreview* CreatePreview();
	static IPreview* PreviewFromSerial(long serial);
	static BOOL Destroy(long h);
public:
	virtual long Serial()=0;
	virtual void SetOCS(GECS& ocs,double scaleOfView2Model=0.01)=0;
	virtual void SetViewport(int vox,int voy,int width,int height)=0;//vox,voy为ocs坐标系下缩放后图形显示视口的原点位置
	virtual void DrawLine(const double* start,const double* end,long hRelaRod=0)=0;
	virtual void DrawPoint(const double* point,long hRelaNode=0)=0;
	virtual void DrawPlacePolygon(GEPOINT* lpPoints,int count)=0;	//绘制占位符
	virtual GEPOINT TransPToOcs(const double* modelpoint)=0;
	virtual GEPOINT TransPFromOcs(double xiView,double yiView,double ziView=0)=0;
	virtual double ScaleOfView2Model()=0;
	virtual int WriteMonoBmpFile(const char* fileName)=0;
public:
	virtual void AppendLine(const double* start,const double* end,long hRelaRod=0)=0;
	virtual void AppendPoint(const double* point,long hRelaNode=0)=0;
	virtual void FlushPreview(short siMarginTop=20,short siMarginLeft=20,short siMarginRight=20,short siMarginBtm=20)=0;
	virtual UINT GetImageWidth()=0;
	virtual UINT GetImageHeight()=0;
public:
	struct ISearchRgn{
	protected:
		WORD wiSearchRgnFrameWidth;
	public:
		ISearchRgn(){wiSearchRgnFrameWidth=1;}
		virtual char get_ciRgnType()=0;
		_declspec(property(get=get_ciRgnType)) char ciRgnType;	//搜索对象的区域类型，只读
		//0xffff表示区域全覆盖的填充模式，否则表示边界线宽
		virtual WORD set_RgnSearchMode(WORD wiFrameWidth=1){return wiSearchRgnFrameWidth=wiFrameWidth;}
		//0xffff表示区域全覆盖的填充模式，否则表示边界线宽
		virtual WORD get_RgnSearchMode(){return wiSearchRgnFrameWidth;};
		_declspec(property(put=set_RgnSearchMode,get=get_RgnSearchMode)) WORD wiRgnSearchMode;	//搜索对象的区域类型，只读
		virtual void Offset(int offsetX,int offsetY)=0;
	};
	struct RGN_CIR : public ISearchRgn{
		short cx,cy;//Preview坐标系下（即OCS坐标系下）
		short r;	//Preview显示比例下（即Preview中实际显示的缩放后圆半径）
		virtual char get_ciRgnType(){return 1;}
		virtual void Offset(int offsetX,int offsetY)
		{
			cx+=(short)offsetX;
			cy+=(short)offsetY;
		}
	};
	struct RGN_RECT : public ISearchRgn{
		short ox,oy;	//矩形原点（x,y坐标值最小的角点坐标）
		short w,h;		//矩形的宽，高
		double flowWX,flowWY;	//矩形横向宽度的走向，即宽度的延伸方向
		virtual char get_ciRgnType(){return 2;}
		virtual void Offset(int offsetX,int offsetY)
		{
			ox+=(short)offsetX;
			oy+=(short)offsetY;
		}
	};
	struct SEARCH{
		char cbDualDirectMode;//=0x0f
		double maxHoriSearchDist;//=5.0;
		double maxVertSearchDist;//=5.0;
		double fHoriPriorCoef;//=0.5;
		double fVertPriorCoef;//=0.5;
		GEPOINT2D vhSearchFlowDirection;	//横向搜索方向
		static const BYTE DIRECT_X			=0x01;
		static const BYTE DIRECT_X_INVERSE  =0x02;
		static const BYTE DIRECT_Y			=0x04;
		static const BYTE DIRECT_Y_INVERSE  =0x08;
		static const BYTE DIRECT_ALL		=0x0f;
		SEARCH( const double* vhcSearchFlowDirection=NULL,char _cbDualDirectMode=DIRECT_ALL,double _maxHoriSearchDist=5.0,
				double _maxVertSearchDist=5,double _fHoriPriorCoef=0.5,double _fVertPriorCoef=0.5)
		{
			cbDualDirectMode=_cbDualDirectMode;
			maxHoriSearchDist=_maxHoriSearchDist;
			maxVertSearchDist=_maxVertSearchDist;
			fHoriPriorCoef=_fHoriPriorCoef;
			fVertPriorCoef=_fVertPriorCoef;
			if(vhcSearchFlowDirection==NULL)
			{
				vhSearchFlowDirection.x=1;
				vhSearchFlowDirection.y=0;
			}
			else
			{
				vhSearchFlowDirection.x=vhcSearchFlowDirection[0];
				vhSearchFlowDirection.y=vhcSearchFlowDirection[1];
			}
		}
	};
	virtual bool SearchBlankRegion(ISearchRgn* pRgnPlace,SEARCH* pSearchRule=NULL)=0;
public:
	struct ISLAND{
		WORD maxy,miny;
		double cx,cy,count;
		ISLAND(){Clear();}
		long arrayEdgeRods[8];	//最多存储8根边界杆件，逆时针旋转
		void Clear(){
			cx=cy=count=maxy=miny=0;
			arrayEdgeRods[0]=arrayEdgeRods[1]=arrayEdgeRods[2]=arrayEdgeRods[3]=0;
			arrayEdgeRods[4]=arrayEdgeRods[5]=arrayEdgeRods[6]=arrayEdgeRods[7]=0;
		}
	};	//内部空白孤岛特征区域
	virtual int SearchIslands(CXhSimpleList<ISLAND>* pListIslands=NULL,WORD wiMinIslandPixels=0)=0;
};

struct PREVIEW{
private:
	IPreview* m_pPreview;
public:
	PREVIEW()
	{
		m_pPreview=IPreview::CreatePreview();
	}
	~PREVIEW()
	{
		IPreview::Destroy(m_pPreview->Serial());
	}
	IPreview* operator->()const{return m_pPreview;}
};

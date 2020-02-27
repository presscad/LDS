#pragma once
#include "ArrayList.h"
#include "HashTable.h"

struct LAYOUT_COORD2D{
	double x,y;
	LAYOUT_COORD2D(double x=0,double y=0){this->x=x;this->y=y;}
	void Set(double x=0,double y=0){this->x=x;this->y=y;}
	operator double*(){return &x;};			//返回一个双精度指针
};

struct SIMPLE_SCOPE{
public:
	double fMinX, fMaxX;
	double fMinY, fMaxY;
	double fMinZ, fMaxZ;
public:
	SIMPLE_SCOPE(){ClearScope();}
	double wide(){return max(0,fMaxX-fMinX);}
	double high(){return max(0,fMaxY-fMinY);}
	double depth(){return max(0,fMaxZ-fMinZ);}
	void ClearScope(){
		fMinX = fMinY = fMinZ =  1.7976931348623158E308;
		fMaxX = fMaxY = fMaxZ = -fMinX;
	}
	void VerifyVertex(const double* pt,int len){
		if(len>=1)
		{
			if(pt[0]<fMinX)
				fMinX = pt[0];
			if(pt[0]>fMaxX)
				fMaxX = pt[0];
		}
		if(len>=2)
		{
			if(pt[1]<fMinY)
				fMinY = pt[1];
			if(pt[1]>fMaxY)
				fMaxY = pt[1];
		}
		if(len>=3)
		{
			if(pt[2]<fMinZ)
				fMinZ = pt[2];
			if(pt[2]>fMaxZ)
				fMaxZ = pt[2];
		}
	}
};

struct DRAWRECT
{
	friend class CDrawingRectLayout;
	//friend class CDrawingRectArray;
	BOOL m_bLayout;					//内部优化布局时用于标识是否已经布置位置
	LAYOUT_COORD2D m_PrevTopLeft;	//内部优化布局时用于存储前一布局方案的左上角位置,以便于回滚布局
public:
	int m_iPageIndex;			//0为起始页码索引
	LAYOUT_COORD2D topLeft;		//包容矩形重新布局后的左上角坐标位置
	LAYOUT_COORD2D initTopLeft;	//在重新布局前视图包容矩形的初始左上角坐标，用于与重新布局后topLeft最终值一起确定包容矩形的偏移量
	LAYOUT_COORD2D LocatePoint(){return LAYOUT_COORD2D(topLeft.x-initTopLeft.x,topLeft.y-initTopLeft.y);}
	double width,height;		//草图绘制矩形的宽度和高度
	//指向关联的CSectionViewDrawing对象
	//无关联对象时表示该矩形为固定矩形
	void *m_pDrawing;	
	DRAWRECT(){m_pDrawing=NULL;m_iPageIndex=0;m_bLayout=FALSE;}
};
//struct SEGREC{SEGI iSeg;int nPageCount;};
class CRectLayout
{
protected:
	long m_idType;
	double m_fFrameWidth;	//边框宽度
	double m_fFrameHeight;	//边框高度
	double m_fBlkZoomcoef;	//原始图元尺寸/绘制图元尺寸
	bool IsDefiniteNotEnoughSpace(double width,double height,bool canExtendByX=false,bool canExtendByY=false);
	bool GetBlankRectTopLeft(double width,double height,LAYOUT_COORD2D *topLeft,double marginSpace,bool bSearchHori=true);
	CArray<double,double&>m_arrRowHeight;	//
public:
	static const long TYPE_BASIC = 1;
	PRESET_ARRAY4<LAYOUT_COORD2D> arrOrgOfSubpages;
	bool m_bAutoZoomFixRect;//随动比例缩放固定位置矩形
	bool m_bPlaceFixRect;	//固定区域宽度高度仅用来占位，分页时便于调整主视图长度一致 wht 18-08-03
	double m_fFixRectWidth;	//固定位置矩形宽
	double m_fFixRectHeight;//固定位置矩形高
	LAYOUT_COORD2D m_FixRectTopLeft;
	static const int FIX_ALIGN_TOP_LEFT		= 1;	//左上角
	static const int FIX_ALIGN_TOP_RIGHT	= 2;	//右上角
	static const int FIX_ALIGN_BTM_LEFT		= 3;	//左下角
	static const int FIX_ALIGN_BTM_RIGHT	= 4;	//右下角
	int fixRectAlign;	//固定位置矩形的定位方式DIMALIGN_TYPE
	CRectLayout();
	virtual ~CRectLayout(void);
	long get_idType(){return m_idType;}
	__declspec(property(get=get_idType)) long idType;
	ARRAY_LIST<DRAWRECT> drawRectArr;
	virtual bool Relayout(double h_to_w,double marginSpace,double limitWidth=0,double maxHeight=0);
	double GetFrameWidth(){return m_fFrameWidth;}
	double GetFrameHeight(){return m_fFrameHeight;}
	double GetRealDrawingHeight();
	//根据实际绘图区域大小重新设置绘图矩形区域的位置
	void ResetDrawingRectPosition();
};

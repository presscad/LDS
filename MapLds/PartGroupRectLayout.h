#pragma once
#include "RectLayout.h"
#include "PartGroupDrawing.h"

//////////////////////////////////////////////////////////////////////////
//
class CPartGroupRectLayout : public CRectLayout
{
protected:
	double draw_rc_width,draw_rc_height;
	CPartGroupDrawing *m_pPartGroupDrawing;
public:
	static const long TYPE_DEFAULT		= 2;	//组焊件默认布局类
	static const long TYPE_3VIEW		= 3;	//塔脚板、十字板典型三视图布局
	static const long TYPE_COMMON_PLATE = 4;	//钢板通用布局
	static const long TYPE_COMMON_TUBE	= 5;	//钢管组焊图
	static const long TYPE_SIMPLE_TUBE	= 6;	//单个主视图组焊件布局
	static const long TYPE_PAGEING_TUBE	= 7;	//钢管分页组焊图
public:
	CPartGroupRectLayout();
	CLDSPartGroup *GetPartGroup();
	CLDSPart *GetRootPart();
	void SetPartGroupDrawing(CPartGroupDrawing *pPartGroupDrawing);
	bool InitDrawingRectLayout(int iPageIndex=-1);
	virtual bool Relayout(double h_to_w,double marginSpace,double limitWidth=0,double maxHeight=0);
	virtual int Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr);

};
//////////////////////////////////////////////////////////////////////////
//
class C3ViewsDrawingRectLayout : public CPartGroupRectLayout
{
public:
	C3ViewsDrawingRectLayout();
	virtual bool Relayout(double h_to_w,double marginSpace,double limitWidth=0,double maxHeight=0);
};

//////////////////////////////////////////////////////////////////////////
class CCommonPlateDrawingRectLayout : public CPartGroupRectLayout
{
public:
	CCommonPlateDrawingRectLayout();
	virtual bool Relayout(double h_to_w,double marginSpace,double limitWidth=0,double maxHeight=0);
};

//////////////////////////////////////////////////////////////////////////
struct SLAVE_DRAWING_RECT
{
	double fDistX;
	DRAWRECT* pSlaveRect;
	SLAVE_DRAWING_RECT(){fDistX=0.0;pSlaveRect=NULL;}
};
class CCommonTubeDrawingRectLayout : public CPartGroupRectLayout
{
private:
	bool m_bCompulsoryHoriLayout;
	double m_fSubRectMaxHigh;
	double m_fSubRectMaxUpHigh;
	double m_fSubRectMaxDownHigh;
	double m_fMainViewRectW;
	CArray<DRAWRECT,DRAWRECT&> drawMainRectArr;
	CArray<DRAWRECT,DRAWRECT&> drawSubRectArr;
	CSectionViewDrawing *m_pMainViewDrawing;
	CArray<SLAVE_DRAWING_RECT,SLAVE_DRAWING_RECT&> slaveDrawRectArr;
	bool Init();
	void CalFrameSize(double h_to_w,double marginSpace,double limitWidth=0,double maxHeight=0,
					  bool bZoomRelayout=false,double *pfPageCount=NULL);
protected:
	int m_iDatumPage;
	double m_fDatumZoomCoef;	//分页时的基准缩放比例，用来保证所有页内的主视图长度一致 wht 18-08-03
	bool m_bZoomRelayout;
public:
	CCommonTubeDrawingRectLayout(CPartGroupDrawing *pPartGroupDrawing=NULL);
	bool IsSlaveViewException();
	bool InternalRelayout(double &zoomdraw_coef,int iPageIndex=-1);
	double CalPageCount(double h_to_w,double marginSpace,double limitWidth=0);
	virtual bool Relayout(double h_to_w,double marginSpace,double limitWidth=0,double maxHeight=0);
	virtual int Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr);
};

class CSimpleTubeDrawingRectLayout : public CCommonTubeDrawingRectLayout
{
public:
	CSimpleTubeDrawingRectLayout();
	virtual bool Relayout(double h_to_w,double marginSpace,double limitWidth=0,double maxHeight=0);
};

class CPagingTubeDrawingRectLayout : public CCommonTubeDrawingRectLayout
{
	int Prepaging();
public:
	CPagingTubeDrawingRectLayout();
	virtual int Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr);
};

//////////////////////////////////////////////////////////////////////////
class CPartGroupDrawingLayoutOper
{
private:
	CPartGroupRectLayout *m_pRectLayout;
	CPartGroupDrawing *m_pPartGroupDrawing;
	//布局参数
	//static const UINT PAPER_A3_WIDTH  = 420;
	//static const UINT PAPER_A3_HEIGHT = 297;
	//static const UINT PAPER_A4_WIDTH  = 297;
	//static const UINT PAPER_A4_HEIGHT = 210;
	//static const double MIN_TUBE_DIA_DRAWLENGTH = 10;
	//分析、创建、销毁布局器
	int AnalysisCommonTubeDrawingLayoutType();
	int AnalysisPartGroupDrawingLayoutType();
	CPartGroupRectLayout* CreateRectLayoutInstance(int idLayoutType);
	bool DestroyRectLayoutInstance();
public:
	static const int MARGIN_SPACE=10;
public:
	CPartGroupDrawingLayoutOper(CPartGroupDrawing *pPartGroupDrawing);
	~CPartGroupDrawingLayoutOper();
	int Relayout(ARRAY_LIST<double> &zoomdraw_coef_arr);
};
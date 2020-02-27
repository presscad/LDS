#include "stdafx.h"
#include "SysDrawingGraph.h"
#include "RxTools.h"
#include "DrawingToolKits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __BP_ONLY_
//////////////////////////////////////////////////////////////////////////
void DrawFunc(void* pContext)
{
	if(pContext==NULL)
		return;
	CSysDrawingGraph* pManager=(CSysDrawingGraph*)pContext;
	pManager->Draw();
	g_p2dDraw->RenderDrawing();
	g_pSolidOper->ZoomAll(0.95);
}
//////////////////////////////////////////////////////////////////////////
//
ISolidDraw *g_pSolidDraw;
ISolidSet *g_pSolidSet;
ISolidOper *g_pSolidOper;
I2dDrawing *g_p2dDraw;
CSysDrawingGraph::CSysDrawingGraph()
{
	m_pDrawSolid=NULL;
	InitPartPara();
}
CSysDrawingGraph::~CSysDrawingGraph()
{
	if(m_pDrawSolid)
	{
		CDrawSolidFactory::Destroy(m_pDrawSolid->GetSerial());
		m_pDrawSolid=NULL;
		g_pSolidDraw=NULL;
		g_pSolidOper=NULL;
		g_pSolidSet=NULL;
		g_p2dDraw=NULL;
	}
}
void CSysDrawingGraph::InitPartPara()
{
	//初始化角钢信息
	m_xAngle.m_fThick=10;
	m_xAngle.m_fWidth=100;
	m_xAngle.m_wLength=1000;
	//初始化钢板信息

}
void CSysDrawingGraph::InitDrawingPara(DRAWING_PARA& draw_para)
{
	m_xDrawingPara.m_iDrawType=draw_para.m_iDrawType;
	m_xDrawingPara.m_iPartType=draw_para.m_iPartType;
}
void CSysDrawingGraph::InitDrawingEnv(HWND hWnd,HWND hParentWnd)
{
	//创建绘图接口
	m_pDrawSolid=CDrawSolidFactory::CreateDrawEngine();
	g_pSolidSet=m_pDrawSolid->SolidSet();
	g_pSolidOper=m_pDrawSolid->SolidOper();
	g_pSolidDraw=m_pDrawSolid->SolidDraw();
	g_p2dDraw=m_pDrawSolid->Drawing2d();
	//设置绘图参数
	UCS_STRU ucs;								//设置坐标系
	LoadDefaultUCS(&ucs);
	g_pSolidSet->Init(hWnd,hParentWnd);
	g_pSolidSet->SetObjectUcs(ucs);
	g_pSolidSet->SetBkColor(RGB(192,192,192));	//设置背景颜色
	g_pSolidSet->SetPen(RGB(200,0,200),PS_SOLID,2);
	g_pSolidSet->SetDisplayType(DISP_DRAWING);		//设置显示类型
	g_pSolidSet->SetDisplayFunc(DrawFunc);
	g_pSolidSet->SetSolidAndLineStatus(FALSE);
	g_pSolidSet->SetArcSamplingLength(5);
	g_pSolidSet->SetSmoothness(36);
	g_pSolidSet->SetDisplayLineVecMark(FALSE);
	g_pSolidSet->SetDispObjectUcs(TRUE);
	g_pSolidSet->SetRotOrg(f3dPoint(0,0,0));
	g_pSolidSet->SetZoomStyle(ROT_CENTER);
	g_pSolidDraw->InitialUpdate();
	//初始化二维绘图环境
	char sFontFile[MAX_PATH],sBigFontFile[MAX_PATH],sAppPath[MAX_PATH];
	GetSysPath(sAppPath);
	sprintf(sFontFile,"%s\\sys\\simplex.shx",sAppPath);
	bool bRetCode=g_pSolidSet->SetShxFontFile(sFontFile);
	sprintf(sBigFontFile,"%s\\sys\\GBHZFS.shx",sAppPath);
	bRetCode=g_pSolidSet->SetBigFontFile(sBigFontFile);
	IDrawingAssembly* pDrawingAssembly=g_p2dDraw->InitDrawingAssembly();
	IDrawing *pDrawing=pDrawingAssembly->AppendDrawing(1,"示意图");
	pDrawing->Database()->AddTextStyle("standard",sFontFile,sBigFontFile);
	g_p2dDraw->SetActiveDrawing(pDrawing->GetId());
}
void CSysDrawingGraph::Draw()
{
	IDrawing *pDrawing=g_p2dDraw->GetActiveDrawing();
	if(pDrawing==NULL)
		return;
	pDrawing->EmptyDbEntities();
	if(m_xDrawingPara.m_iDrawType==DRAWING_PARA::LINE_MAP)
		DrawLineMap();
	else if(m_xDrawingPara.m_iDrawType==DRAWING_PARA::FORCE_MAP)
		DrawForceMap();
	else if(m_xDrawingPara.m_iDrawType==DRAWING_PARA::STRU_MAP)
		DrawStruMap();
	else if(m_xDrawingPara.m_iDrawType==DRAWING_PARA::PART_MAP)
	{
		if(m_xDrawingPara.m_iPartType==DRAWING_PARA::DRAW_LINE_ANGLE)
			DrawAngle();
		else if(m_xDrawingPara.m_iPartType==DRAWING_PARA::DRAW_PLATE)
			DrawPlate();
		else if(m_xDrawingPara.m_iPartType==DRAWING_PARA::DRAW_LINE_TUBE)
			DrawTube();
	}
	else if(m_xDrawingPara.m_iDrawType==DRAWING_PARA::WELD_MAP)
		DrawWeldMap();
}
//绘制总图示意图
void CSysDrawingGraph::DrawLineMap()
{
	IDrawing *pDrawing=g_p2dDraw->GetActiveDrawing();
	if(pDrawing==NULL)
		return;
}
//绘制司令图示意图
void CSysDrawingGraph::DrawForceMap()
{
	IDrawing *pDrawing=g_p2dDraw->GetActiveDrawing();
	if(pDrawing==NULL)
		return;
}
//绘制结构图示意图
void CSysDrawingGraph::DrawStruMap()
{
	IDrawing *pDrawing=g_p2dDraw->GetActiveDrawing();
	if(pDrawing==NULL)
		return;
}
//绘制角钢示意图
void CSysDrawingGraph::DrawAngle()
{
	IDrawing *pDrawing=g_p2dDraw->GetActiveDrawing();
	if(pDrawing==NULL)
		return;
	double len=m_xAngle.m_wLength;
	//轮廓点
	AppendDbPoint(pDrawing,GEPOINT(0,0,0),0,PS_SOLID);
	AppendDbPoint(pDrawing,GEPOINT(0,m_xAngle.m_fThick,0),0,PS_SOLID);
	AppendDbPoint(pDrawing,GEPOINT(0,m_xAngle.m_fWidth,0),0,PS_SOLID);
	AppendDbPoint(pDrawing,GEPOINT(0,-m_xAngle.m_fThick,0),0,PS_SOLID);
	AppendDbPoint(pDrawing,GEPOINT(0,-m_xAngle.m_fWidth,0),0,PS_SOLID);
	AppendDbPoint(pDrawing,GEPOINT(len,0,0),0,PS_SOLID);
	AppendDbPoint(pDrawing,GEPOINT(len,m_xAngle.m_fThick,0),0,PS_SOLID);
	AppendDbPoint(pDrawing,GEPOINT(len,m_xAngle.m_fWidth,0),0,PS_SOLID);
	AppendDbPoint(pDrawing,GEPOINT(len,-m_xAngle.m_fThick,0),0,PS_SOLID);
	AppendDbPoint(pDrawing,GEPOINT(len,-m_xAngle.m_fWidth,0),0,PS_SOLID);
	//轮廓线
	AppendDbLine(pDrawing,GEPOINT(0,-m_xAngle.m_fWidth),GEPOINT(0,m_xAngle.m_fWidth),0,PS_SOLID);
	AppendDbLine(pDrawing,GEPOINT(0,-m_xAngle.m_fWidth),GEPOINT(len,-m_xAngle.m_fWidth),0,PS_SOLID);
	AppendDbLine(pDrawing,GEPOINT(0,m_xAngle.m_fWidth),GEPOINT(len,m_xAngle.m_fWidth),0,PS_SOLID);
	AppendDbLine(pDrawing,GEPOINT(len,-m_xAngle.m_fWidth),GEPOINT(len,m_xAngle.m_fWidth),0,PS_SOLID);
	AppendDbLine(pDrawing,GEPOINT(0,0),GEPOINT(len,0),0,PS_SOLID);
	AppendDbLine(pDrawing,GEPOINT(0,-m_xAngle.m_fThick),GEPOINT(len,-m_xAngle.m_fThick),0,PS_DASH);
	AppendDbLine(pDrawing,GEPOINT(0,m_xAngle.m_fThick),GEPOINT(len,m_xAngle.m_fThick),0,PS_DASH);
	//螺栓孔
	for(BOLT_INFO *pBolt=m_xAngle.m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xAngle.m_xBoltInfoList.GetNext())
	{
		f3dPoint ls_pos(pBolt->posX,pBolt->posY);
		AppendDbCircle(pDrawing,ls_pos,f3dPoint(0,0,1),pBolt->bolt_d*0.5,0,PS_SOLID);
	}
	//尺寸标注

	//螺栓标注
}
//绘制钢板示意图
void CSysDrawingGraph::DrawPlate()
{
	IDrawing *pDrawing=g_p2dDraw->GetActiveDrawing();
	if(pDrawing==NULL)
		return;
}
//绘制钢管示意图
void CSysDrawingGraph::DrawTube()
{
	IDrawing *pDrawing=g_p2dDraw->GetActiveDrawing();
	if(pDrawing==NULL)
		return;
}
//绘制组焊图示意图
void CSysDrawingGraph::DrawWeldMap()
{
	IDrawing *pDrawing=g_p2dDraw->GetActiveDrawing();
	if(pDrawing==NULL)
		return;
}
#endif
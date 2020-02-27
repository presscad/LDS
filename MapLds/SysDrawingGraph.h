#ifndef __SYSDRAWINGGRAPH_H_
#define __SYSDRAWINGGRAPH_H_

#include "I_DrawSolid.h"
#include "ProcessPart.h"
//////////////////////////////////////////////////////////////////////////
//
struct DRAWING_PARA
{
	static const int LINE_MAP	=1;	//单线图
	static const int FORCE_MAP	=2;	//司令图
	static const int STRU_MAP	=3;	//结构图
	static const int PART_MAP	=4;	//构件图
	static const int WELD_MAP	=5;	//组焊图
	int m_iDrawType;	//绘制图形类型
	static const int DRAW_LINE_ANGLE	=1;	//角钢
	static const int DRAW_PLATE			=2;	//钢板
	static const int DRAW_LINE_TUBE		=3;	//钢管
	int m_iPartType;	//绘制构件类型
public:
	DRAWING_PARA(){m_iDrawType=0;m_iPartType=0;}
	DRAWING_PARA(int iDrawType,int iPartType){m_iDrawType=iDrawType;m_iPartType=iPartType;}
};
class CSysDrawingGraph
{
	IDrawSolid *m_pDrawSolid;
	DRAWING_PARA m_xDrawingPara;
	CProcessAngle m_xAngle;
	CProcessPlate m_xPlate;
private:
	void InitPartPara();
	void DrawLineMap();
	void DrawForceMap();
	void DrawStruMap();
	void DrawAngle();
	void DrawPlate();
	void DrawTube();
	void DrawWeldMap();
public:
	CSysDrawingGraph();
	~CSysDrawingGraph();
	void InitDrawingEnv(HWND hWnd,HWND hParentWnd);
	void InitDrawingPara(DRAWING_PARA& draw_para);
	void Draw();
};
extern ISolidDraw *g_pSolidDraw;
extern ISolidSet *g_pSolidSet;
extern ISolidOper *g_pSolidOper;
extern I2dDrawing *g_p2dDraw;
#endif
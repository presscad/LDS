#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Tower.h"
#include "DrawUnit.h"
#include "PartGroupDrawing.h"	//IDbEntsContext在这个头文件中声明

class CTubeDrawing{
	bool CreateSectSlotDrawing(CLDSLineTube *pTube,CLDSGeneralPlate* pInsertPlate,CDrawCoordTrans& dcs,IDbEntsContext* pContext);		//绘制含开口槽的钢管截面图
public:
	struct TUBE_DRAWING_STYLE{
		bool bDrawExterCircle;	//与法兰一起绘制时外圆不绘制，应设为false关掉绘制
		bool bDimSectSlotAngle;	//标注截面图中交叉开口槽非标角度（用于非垂直的十字插板开槽）
		bool bDrawWeldline;		//有焊缝时是否需要绘制焊缝
		long hSectSlotPlate;	//截面图中绘制开口槽关联的钢板,0表示不绘制开口槽
		char ciDrawModeH0V1;
		bool bMandatoryDimLabel;//强制性标注编号
		TUBE_DRAWING_STYLE();
	}style;
	CTubeDrawing();
	void CreateUprightLayingDrawing(CLDSLineTube *pTube,CDrawCoordTrans& dcs,IDbEntsContext* pContext);	//绘制含钢管截面图
	void CreateFlatLayingDrawing(CLDSLineTube *pTube,CDrawCoordTrans& dcs,IDbEntsContext* pContext);	//绘制含钢管平放图
	BOOL CreateTubeDrawing(CLDSLineTube *pTube,CDrawCoordTrans& dcs,IDbEntsContext* pContext,AcDbObjectId *pOutEntId=NULL);
};

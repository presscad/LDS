#include "XhCharString.h"
enum DIM_ITEM_TYPE{
	DIM_JG_SPART_NO=0,		//角钢编号
	DIM_BOLT_DIA=1,			//螺栓直径
	DIM_BOLT_HOR_DIST,		//螺栓水平边距
	DIM_BOLT_VER_DIST,		//螺栓竖向边距
	DIM_BOLT_HIGH,			//螺栓定位高度
	DIM_RIB_SPART_NO1,		//肋板编号1
	DIM_RIB_SPART_NO2,		//肋板编号2
	DIM_RIB_H_DIST,			//肋板台口宽度
	DIM_RIB_V_DIST,			//肋板外侧高度
	DIM_RIB_HIGH,			//肋板高度
	DIM_RIB_EDGE_DIST,		//肋板边距
	DIM_FL_THICK,			//法兰厚度
	DIM_WELD_THICK,			//焊接厚度
	DIM_FL_SPART_NO=1,		//法兰编号
	DIM_TUBE_DIA,			//钢管直径
	DIM_VIEW_SYMBOL,		//剖视图幅
	DIM_VIEW_NAME,			//视图名称
};

typedef struct tagTJ_DIM_INFO
{
	CXhChar16 sPartNo;			//角钢件号
	CXhChar16 sBoltDim;			//螺栓直径
	CXhChar16 sBoltLocHigh;		//螺栓定位高度
	CXhChar16 sBoltHorDist;		//螺栓水平偏移距
	CXhChar16 sBoltVerDist;		//螺栓竖向偏移距
	CXhChar16 sRibHorDist;		//肋板台口宽度			
	CXhChar16 sRibVerDist;		//肋板外侧高度	
	CXhChar16 sTjThick;			//塔脚板厚度
	CXhChar16 sWeldThick;		//焊接厚度
	CXhChar16 sTubeDim;			//钢管直径
	CXhChar16 sViewSymbol;		//剖视图幅
	CXhChar16 sViewName;		//视图名称
}TJ_DIM_INFO;

typedef struct tagFL_DIM_INFO
{
	CXhChar16 sFlPartNo;		//法兰编号
	CXhChar16 sRibHorDist;		//肋板台口宽度
	CXhChar16 sRibVerDist;		//肋板外侧高度
	CXhChar16 sRibPartNo1;		//肋板标注1
	CXhChar16 sRibPartNo2;		//肋板标注2
	CXhChar16 sViewSymbol;		//剖视图幅
	CXhChar16 sViewName;		//视图名称
}FL_DIM_INFO;
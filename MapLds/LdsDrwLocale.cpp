#include "StdAfx.h"
#include "ClientId.h"
#include "lds_part.h"
#include ".\LdsDrwLocale.h"
#include ".\PlankDrawing.h"
#include ".\SysPara.h"

#if defined(_DEBUG)&&!defined(_DISABLE_DEBUG_NEW_)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////////
LDSDRW_LOCALE gxDrwLocalizer;
LDSDRW_LOCALE::LDSDRW_LOCALE()
{
	bSplitAngleSketchDrawing=false;
}
void LDSDRW_LOCALE::InitCustomerSerial(UINT uiCustomizeSerial)
{
	XHLOCALE::InitCustomerSerial(uiCustomizeSerial);
	CLDSPart::CustomerSerial=uiCustomizeSerial;	//客户定制标识序号
	CPlankDrawing::PREFER_STYLEFLAG.SetMaskBitCount(0);
	if(uiCustomizeSerial==XHCID::CID_QingDao_DongFang)	//XHCID::CID_QingDao_DongFang=5
	{	//青岛东方
		this->AddLocaleItemStr100("ProcessCard.HasSymbol","○");
		this->AddLocaleItemStr100("WeldLabelPrefix", "Z");	//十字插板及底脚组焊中主视图绘制剖视图符
		this->AddLocaleItemBool("CardCodeLabelAddSegI",true);
		this->AddLocaleItemBool("WeldMasterMap.DrawSectSymbol",true);	//十字插板及底脚组焊中主视图绘制剖视图符
		this->AddLocaleItemBool("StdXPlates.blDimVertBoltSpace",true);	//标注十字插板竖向螺栓间距
		//设定组焊明细表样式
		sys.part_group_map.part_table.bDispPartGroupNo = true;	//组焊编号
		sys.part_group_map.part_table.bDispPartNo = true;		//组焊子构件编号
		sys.part_group_map.part_table.bDispPartGuiGe = true;	//组焊子构件规格
		sys.part_group_map.part_table.bDispPartLen = false;		//组焊子构件长度
		sys.part_group_map.part_table.bDispPartNum = true;		//组焊子构件数量
		sys.part_group_map.part_table.bDispPartWeight = false;	//组焊子构件单重
		sys.part_group_map.part_table.bDispNotes = false;
		sys.part_group_map.part_table.bDispPartGroupNum = true;	//组焊件数量
		sys.part_group_map.part_table.bDispInternal = false;	//大吉属性
		sys.part_group_map.part_table.bDispAppearance = false;	//大吉属性
		sys.general_set.iMatCharPosType = 0;	//青岛东方工艺卡编号不需要添加材质符
		this->AddLocaleItemStr100("WeldBOM.AddPartStampLabel", \
			"DF-%TowerType%-%PartLabel%-%material%-%PartNum%");	//添加子构件钢印号
		sys.welding.bZoomAdaptiveWithScale=sys.part_group_map.part_table.bZoomInProcessCard=false;
		//设定螺栓标注样式
		this->AddLocaleItemBool("DimRayLsSpace", true);			//标注螺栓间距
		this->AddLocaleItemBool("DimLsRayAngle", true);			//标注螺栓射线角度
		//this->AddLocaleItemBool("DimSpaceToWeldEdge", true);	//标注螺栓到焊接边的最小垂距
		this->AddLocaleItemBool("DimSpaceToEdgeLine", false);	//标注轮廓特征螺栓到轮廓边的距离
		this->AddLocaleItemBool("DimFeatueLsSpace", false);		//标注轮廓特征螺栓间距
	}
	else if( uiCustomizeSerial==XHCID::CID_YunNan_DongDian||
		uiCustomizeSerial==XHCID::CID_YunNan_ZhanYe||
		uiCustomizeSerial==XHCID::CID_YunNan_JianYuan)
	{
		this->AddLocaleItemBool("WeldSubPartDrawWeldingSymbol",true);
	}
	else if( uiCustomizeSerial==XHCID::CID_ZheJiangShengDa)
	{	//浙江盛达冯建刚要求TMA输出组焊板时添加螺栓间距标注 wjh-2020.1.8
		this->AddLocaleItemBool("DimRayLsSpace", true);			//标注螺栓间距
	}
	else if(uiCustomizeSerial==2)
	{	//江苏南通华灿，定制项如下：
		//角钢工艺卡中角钢草图分成正视图及俯视图(因不常用，不做风格项设定，直接通过权限控制)
		bSplitAngleSketchDrawing=true;
		//分开显示主管组焊图的正视图及俯视图(暂不做风格项设定，直接通过权限控制）
		//螺栓孔到焊接边的最小垂直距离标注
		CPlankDrawing::PREFER_STYLEFLAG.SetFlagDword(CPlankDrawing::DimBoltSpaceToWeldEdge);
		//各螺栓孔径的孔径标注（无论是否图符标注）
		CPlankDrawing::PREFER_STYLEFLAG.SetFlagDword(CPlankDrawing::DimHoleDiameterAlways);
	}
	else if(uiCustomizeSerial==3)
	{	//武晓高爱美说需要在输出分段构件明细表时将法兰及十字插板上的附加子助板独立显示输出 wjh-2017.5.26
		sys.stru_map.bMergeAttachPlatesToBasePlateInBOM=FALSE;
	}
}

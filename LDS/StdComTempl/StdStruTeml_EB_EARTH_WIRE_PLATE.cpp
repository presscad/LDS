#include "stdafx.h"
#include "..\LDS.h"
#include "NodeStdComTempl.h"
#include "WireStruComTempl.h"

//初始化模板参数-IStdComTempl::STDTMPL_STRU_EB_EARTH_WIRE_PLATE
void CStdComTemplWireStructure::InitComTempl_EB_EARTH_WIRE_PLATE()
{
	ClearTmplParams();
	name.Copy("EB型耐张塔地线挂点顶面板");
	szSketchImgFile.Copy("EB3.bmp");
}

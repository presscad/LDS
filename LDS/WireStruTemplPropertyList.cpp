#include "stdafx.h"
#include "LDS.h"
#include "I_DrawSolid.h"
#include "btc.h"
#include "WireStruTemplPropertyList.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "CallBackDialog.h"
#include "Fitting.h"

#ifdef __COMMON_PART_INC_
///////////////////////////////////////////////////////////////////////////
CWireStruComPropertyList::CWireStruComPropertyList(void)
{
	params.m_pSetupNode10=params.m_pSetupNode20=params.m_pSetupNode30=params.m_pSetupNode40=NULL;
	m_hSetupNode10=m_hSetupNode20=0;
}

CWireStruComPropertyList::~CWireStruComPropertyList(void)
{
}
CLDSNode* CWireStruComPropertyList::get_pSetupNode10() { return params.m_pSetupNode10; }
CLDSNode* CWireStruComPropertyList::set_pSetupNode10(CLDSNode* _pSetupNode10)
{
	CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.Add(KEY4C(1));	//N10节点
	long hSetupNode=_pSetupNode10!=NULL?_pSetupNode10->handle:0;
	pStdTempl->SetMappingNode(1,hSetupNode);
	CLDSNode* pMirNodeX=_pSetupNode10->GetMirNode(MIRMSG(1));
	CStdTemplParaNode* pMirParaNode=pStdTempl->listParaNodes.Add(TMPLOBJID(1,'X'));	//N10节点
	if (pMirNodeX!=NULL&&pMirParaNode!=NULL)
	{
		pStdTempl->SetMappingNode(TMPLOBJID(1,'X'),pMirNodeX->handle);
		pMirParaNode->pRelaNode=pMirNodeX;
	}
	return params.m_pSetupNode10=pParaNode->pRelaNode=_pSetupNode10;
};
CLDSNode* CWireStruComPropertyList::get_pSetupNode20() { return params.m_pSetupNode20; }
CLDSNode* CWireStruComPropertyList::set_pSetupNode20(CLDSNode* _pSetupNode20)
{
	CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.Add(KEY4C(2));	//N20节点
	long hSetupNode=_pSetupNode20!=NULL?_pSetupNode20->handle:0;
	pStdTempl->SetMappingNode(2,hSetupNode);
	CLDSNode* pMirNodeX=_pSetupNode20->GetMirNode(MIRMSG(1));
	CStdTemplParaNode* pMirParaNode=pStdTempl->listParaNodes.Add(TMPLOBJID(2,'X'));	//N10节点
	if (pMirNodeX!=NULL&&pMirParaNode!=NULL)
	{
		pStdTempl->SetMappingNode(TMPLOBJID(2,'X'),pMirNodeX->handle);
		pMirParaNode->pRelaNode=pMirNodeX;
	}
	return params.m_pSetupNode20=pParaNode->pRelaNode=_pSetupNode20;
};
CLDSNode* CWireStruComPropertyList::get_pSetupNode30() { return params.m_pSetupNode30; }
CLDSNode* CWireStruComPropertyList::set_pSetupNode30(CLDSNode* _pSetupNode30)
{
	CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.Add(KEY4C(3));	//N30节点
	long hSetupNode=_pSetupNode30!=NULL?_pSetupNode30->handle:0;
	pStdTempl->SetMappingNode(3,hSetupNode);
	CLDSNode* pMirNodeX=_pSetupNode30->GetMirNode(MIRMSG(1));
	CStdTemplParaNode* pMirParaNode=pStdTempl->listParaNodes.Add(TMPLOBJID(3,'X'));	//N10节点
	if (pMirNodeX!=NULL&&pMirParaNode!=NULL)
	{
		pStdTempl->SetMappingNode(TMPLOBJID(3,'X'),pMirNodeX->handle);
		pMirParaNode->pRelaNode=pMirNodeX;
	}
	return params.m_pSetupNode30=pParaNode->pRelaNode=_pSetupNode30;
};
CLDSNode* CWireStruComPropertyList::get_pSetupNode40() {
	return params.m_pSetupNode40; 
}
CLDSNode* CWireStruComPropertyList::set_pSetupNode40(CLDSNode* _pSetupNode40)
{
	CStdTemplParaNode* pParaNode=pStdTempl->listParaNodes.Add(KEY4C(4));	//N40节点
	long hSetupNode=_pSetupNode40!=NULL?_pSetupNode40->handle:0;
	pStdTempl->SetMappingNode(4,hSetupNode);
	CLDSNode* pMirNodeX=_pSetupNode40->GetMirNode(MIRMSG(1));
	CStdTemplParaNode* pMirParaNode=pStdTempl->listParaNodes.Add(TMPLOBJID(4,'X'));	//N40节点
	if (pMirNodeX!=NULL&&pMirParaNode!=NULL)
	{
		pStdTempl->SetMappingNode(TMPLOBJID(4,'X'),pMirNodeX->handle);
		pMirParaNode->pRelaNode=pMirNodeX;
	}
	return params.m_pSetupNode40=pParaNode->pRelaNode=_pSetupNode40;
};
const DWORD HASHTABLESIZE = 500;
long CWireStruComPropertyList::InitPropHashtable(long initId/*=1*/,long initHashSize/*=50*/,long initStatusHashSize/*=50*/)	//返回下一空闲Id
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(initStatusHashSize);
	//模板信息
	AddPropItem("BasicInfo",PROPLIST_ITEM(id++,"基本信息"));
	AddPropItem("EB_WIDTH",PROPLIST_ITEM(id++,"金具预留宽度",""));
	AddPropItem("SetupInfo",PROPLIST_ITEM(id++,"装配位置"));
	AddPropItem("WireHoleCount",PROPLIST_ITEM(id++,"挂线孔数",NULL,"1|3|5"));
	AddPropItem("WireHoleSpace#1-",PROPLIST_ITEM(id++,"1#孔间距"));
	AddPropItem("WireHole#0",PROPLIST_ITEM(id++,"基准孔"));
	AddPropItem("WireHole#1+",PROPLIST_ITEM(id++,"挂线孔+1"));
	AddPropItem("WireHole#1-",PROPLIST_ITEM(id++,"挂线孔-1"));
	AddPropItem("WireHoleSpace#2-",PROPLIST_ITEM(id++,"2#孔间距"));
	AddPropItem("WireHole#2+",PROPLIST_ITEM(id++,"挂线孔+2"));
	AddPropItem("WireHole#2-",PROPLIST_ITEM(id++,"挂线孔-2"));
	AddPropItem("WIRE_HOLES_STR",PROPLIST_ITEM(id++,"EB安装孔径",""));	//未来应去掉，现在保留是为了避免死机 wjh-2019.8.11
	AddPropItem("WIREANGLE_SIZE",PROPLIST_ITEM(id++,"挂线角钢规格"));
	AddPropItem("BASE_WIREANGLE_SIZE",PROPLIST_ITEM(id++,"底座角钢规格", "挂线底座角钢规格"));
	AddPropItem("FIX_CONN_BOLTS_N",PROPLIST_ITEM(id++,"固接螺栓数","","2|4"));	//未来应去掉，现在保留是为了避免死机 wjh-2019.8.11
	//第1组挂孔设计参数
	AddPropItem("WirePointGroup1",PROPLIST_ITEM(id++,"1#挂孔设计参数"));
	AddPropItem("GP1.szFittingType",PROPLIST_ITEM(id++,"主金具类型","","0.EB挂板|1.GD挂板"));
	AddPropItem("GP1.szFittingSizeCode",PROPLIST_ITEM(id++,"主金具型号",""));
	AddPropItem("GP1.FITTING_RESERVE_WIDTH",PROPLIST_ITEM(id++,"预留金具宽度",""));
	AddPropItem("GP1.WIRE_HOLE_D",PROPLIST_ITEM(id++,"预留安装孔径",""));
	AddPropItem("GP1.WIRE_HOLE_COUNT",PROPLIST_ITEM(id++,"挂孔数量",""));
	AddPropItem("GP1.LOCATE_SPACE",PROPLIST_ITEM(id++,"开档距离L",""));
	AddPropItem("GP1.SwingAngle",PROPLIST_ITEM(id++,"主金具摆动角","与杆塔连接金具绕耳轴的转动角度，单位°"));
	AddPropItem("GP1.URingSizeLabel",PROPLIST_ITEM(id++,"挂接U型环型号","配套U型环型号"));
	AddPropItem("GP1.URingSwingAngle",PROPLIST_ITEM(id++,"U型环摆动角","挂接U型环绕挂孔的摆动角，单位°"));
	//第2组挂孔设计参数
	AddPropItem("WirePointGroup2",PROPLIST_ITEM(id++,"2#挂孔设计参数"));
	AddPropItem("GP2.szFittingType",PROPLIST_ITEM(id++,"主金具类型","","0.EB挂板|1.GD挂板"));
	AddPropItem("GP2.szFittingSizeCode",PROPLIST_ITEM(id++,"主金具型号",""));
	AddPropItem("GP2.FITTING_RESERVE_WIDTH",PROPLIST_ITEM(id++,"预留金具宽度",""));
	AddPropItem("GP2.WIRE_HOLE_D",PROPLIST_ITEM(id++,"预留安装孔径",""));
	AddPropItem("GP2.WIRE_HOLE_COUNT",PROPLIST_ITEM(id++,"挂孔数量",""));
	AddPropItem("GP2.LOCATE_SPACE",PROPLIST_ITEM(id++,"开档距离L",""));
	AddPropItem("GP2.SwingAngle",PROPLIST_ITEM(id++,"主金具摆动角","与杆塔连接金具绕耳轴的转动角度，单位°"));
	AddPropItem("GP2.URingSizeLabel",PROPLIST_ITEM(id++,"挂接U型环型号","配套U型环型号"));
	AddPropItem("GP2.URingSwingAngle",PROPLIST_ITEM(id++,"U型环摆动角","挂接U型环绕挂孔的摆动角，单位°"));
	//关联节点及杆件
	AddPropItem("Node_N10",PROPLIST_ITEM(id++,"特征节点N10",""));
	AddPropItem("Node_N20",PROPLIST_ITEM(id++,"特征节点N20",""));
	AddPropItem("Node_N30",PROPLIST_ITEM(id++,"特征节点N30",""));
	AddPropItem("Node_N40",PROPLIST_ITEM(id++,"特征节点N40",""));
	AddPropItem("ParaRod#1", PROPLIST_ITEM(id++, "特征杆件1", ""));
	AddPropItem("ParaRod#2", PROPLIST_ITEM(id++, "特征杆件2", ""));
	AddPropItem("ParaRod#3", PROPLIST_ITEM(id++, "特征杆件3", ""));
	AddPropItem("ParaRod#4", PROPLIST_ITEM(id++, "特征杆件4", ""));
	AddPropItem("ParaRod#5", PROPLIST_ITEM(id++, "特征杆件5", ""));
	AddPropItem("DimensionParamLevel1",PROPLIST_ITEM(id++,"一级尺寸设计参数"));
	if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_FRAMELINE_HYJ1)
	{
		AddPropItem("X1",PROPLIST_ITEM(id++,"地线挂点位置X1","地线挂点到塔中心轴线的X轴投影距离"));
		AddPropItem("X2",PROPLIST_ITEM(id++,"导线挂点位置X2","导线挂点到塔中心轴线的X轴投影距离"));
		AddPropItem("Y1",PROPLIST_ITEM(id++,"地线架宽度Y1","地线挂点Y向宽度"));
		AddPropItem("Y2",PROPLIST_ITEM(id++,"导线架宽度Y2","导线挂点Y向宽度"));
		AddPropItem("Z1",PROPLIST_ITEM(id++,"层间距Z1","导地线挂点间Z向垂直间距"));
		AddPropItem("Z2",PROPLIST_ITEM(id++,"导线垂高Z2","导线挂点到塔身顶面垂高位置(允许负值)"));
		AddPropItem("DimensionParamLevel2",PROPLIST_ITEM(id++,"二级尺寸设计参数"));
		AddPropItem("X3",PROPLIST_ITEM(id++,"地线架伸出量X3",""));
		AddPropItem("X4",PROPLIST_ITEM(id++,"导线架位置X4",""));
		AddPropItem("Z3",PROPLIST_ITEM(id++,"地线架高度Z3",""));
		AddPropItem("Z4",PROPLIST_ITEM(id++,"导线架高度Z4",""));
		//工作面填充参数
		AddPropItem("PanelInfo",PROPLIST_ITEM(id++,"工作面腹材"));
	}
	else if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_FRAMELINE_HX3)
	{
		AddPropItem("X",PROPLIST_ITEM(id++,"导线挂点位置X","导线挂点到塔中心轴线的X轴投影距离"));
		AddPropItem("Y",PROPLIST_ITEM(id++,"导线架宽度Y","导线挂点Y向宽度"));
		AddPropItem("DimensionParamLevel2",PROPLIST_ITEM(id++,"二级尺寸设计参数"));
		AddPropItem("Z1",PROPLIST_ITEM(id++,"横担高度Z1","导线横担根部Z向垂直高度"));
		AddPropItem("Z2",PROPLIST_ITEM(id++,"挂点端面高Z2","导线挂点端面的垂高位置"));
	}
	return id;
}
long CWireStruComPropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	STDTEMPL_PARAM_ITEM* pParam;
	if (GetPropID("Node_N10")==id)
	{	//上装配节点N10
		long hNode=pStdTempl->MappingLdsNodeH(1);
		sText.Printf("0x%X",hNode);
	}
	else if (GetPropID("Node_N20")==id)
	{	//下装配节点N20
		long hNode=pStdTempl->MappingLdsNodeH(2);
		sText.Printf("0x%X",hNode);
	}
	else if (GetPropID("Node_N30")==id)
	{	//下装配节点N30
		long hNode=pStdTempl->MappingLdsNodeH(3);
		sText.Printf("0x%X",hNode);
	}
	else if (GetPropID("Node_N40")==id)
	{	//下装配节点N40
		long hNode=pStdTempl->MappingLdsNodeH(4);
		sText.Printf("0x%X",hNode);
	}
	else if (GetPropID("ParaRod#1")==id)
	{	//杆件
		long *phRod=pStdTempl->hashMatchRodHid.GetValue(1);
		if (phRod)
			sText.Printf("0x%X",*phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#2") == id)
	{	//杆件
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(2);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#2") == id)
	{	//杆件
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(2);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#3") == id)
	{	//杆件
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(3);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#4") == id)
	{	//杆件
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(4);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("ParaRod#5") == id)
	{	//杆件
		long *phRod = pStdTempl->hashMatchRodHid.GetValue(5);
		if (phRod)
			sText.Printf("0x%X", *phRod);
		else
			sText.Copy("0x0");
	}
	else if (GetPropID("EB_WIDTH")==id)
	{	//EB金具宽度
		if ((pParam=pStdTempl->listParams.GetValue(2))==NULL)
		{	//"EBW"：EB挂点金具宽度+间隙，mm
			pParam=pStdTempl->listParams.Add(2);
			pParam->value.fValue=84;
		}
		sText.Printf("%g",pParam->value.fValue);
	}
	else if (GetPropID("WIRE_HOLES_STR")==id)
	{	//挂孔清单"D2.15@0#50,D26@200#50"
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C('D',1)))!=NULL)
			sText=pParam->value.szStrBuf;
	}
	else if (GetPropID("WIREANGLE_SIZE")==id)
	{	//挂线T型组合角钢规格
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("WAS")))!=NULL)
			sText=pParam->value.szStrBuf;
	}
	else if (GetPropID("BASE_WIREANGLE_SIZE")==id)
	{	//挂线底座角钢规格
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("BAS")))!=NULL)
			sText=pParam->value.szStrBuf;
	}
	else if (GetPropID("FIX_CONN_BOLTS_N")==id)
	{	//挂线短角钢的固接螺栓数
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("FBN")))!=NULL)
			sText.Printf("%d",pParam->value.liSafeVal);
	}
	else if (GetPropID("X")==id)
	{	//导线挂点位置X
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="10400";
	}
	else if (GetPropID("X1")==id)
	{	//地线挂点位置X1
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X1")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="10400";
	}
	else if (GetPropID("X2")==id)
	{	//导线挂点位置X2
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X2")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="7700";
	}
	else if (GetPropID("Y")==id)
	{	//导线架宽度Y
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="600";
	}
	else if (GetPropID("Y1")==id)
	{	//地线架宽度Y1
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y1")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="10400";
	}
	else if (GetPropID("Y2")==id)
	{	//导线架宽度Y2
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y2")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="7700";
	}
	else if (GetPropID("Z1")==id)
	{	//导地线层间距Z1
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z1")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="2300";
	}
	else if (GetPropID("Z2")==id)
	{	//导线垂高Z2，导线挂点到塔身顶面垂高位置(允许负值)
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z2")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="600";
	}
	else if (GetPropID("X3")==id)
	{	//地线架伸出量X3
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X3")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="800";
	}
	//AddPropItem("X4",PROPLIST_ITEM(id++,"导线架位置X4",""));
	else if (GetPropID("Z3")==id)
	{	//地线架高度Z3
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z3")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="600";
	}
	else if (GetPropID("Z4")==id)
	{	//导线架高度Z4
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z4")))!=NULL)
			sText.Printf("%g",pParam->value.lfSafeVal);
		else
			sText="600";
	}
	//金具属性
	else if (GetPropID("GP1.szFittingType")==id||GetPropID("GP2.szFittingType")==id)
	{	//金具类型
		int group=(id==GetPropID("GP2.szFittingType"))?1:0;
		if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_EB)
			sText.Copy("0.EB挂板");
		else if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_GD)
			sText.Copy("1.GD挂板");
		else if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_UR)
			sText.Copy("2.U型挂环");
		else if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_US)
			sText.Copy("3.U型螺丝");
		else if (this->pWireStruTmpl->xarrWireGroups[group].ciFittingType==CLDSFitting::TYPE_UB)
			sText.Copy("4.U型挂板");
	}
	else if (GetPropID("GP1.szFittingSizeCode")==id||GetPropID("GP2.szFittingSizeCode")==id)
	{	//主金具型号
		int group=(id==GetPropID("GP2.szFittingSizeCode"))?1:0;
		sText.Copy(pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode);
	}
	else if (GetPropID("GP1.WIRE_HOLE_COUNT")==id||GetPropID("GP2.WIRE_HOLE_COUNT")==id)
	{	//挂线角钢的挂孔数量（即主金具数量）
		int group=(id==GetPropID("GP2.WIRE_HOLE_COUNT"))?1:0;
		sText.Printf("%d",pWireStruTmpl->xarrWireGroups[group].cnWirePointCount);
	}
	else if (GetPropID("GP1.WIRE_HOLE_D")==id||GetPropID("GP2.WIRE_HOLE_D")==id)
	{	//预留金具安装孔径
		int group=(id==GetPropID("GP2.WIRE_HOLE_D"))?1:0;
		sText.Printf("%d",pWireStruTmpl->xarrWireGroups[group].cnHoleD);
	}
	else if (GetPropID("GP1.LOCATE_SPACE")==id||GetPropID("GP2.LOCATE_SPACE")==id)
	{	//开档距离
		int group=(id==GetPropID("GP2.LOCATE_SPACE"))?1:0;
		sText.Printf("%d",pWireStruTmpl->xarrWireGroups[group].wiSpaceL);
	}
	else if (GetPropID("GP1.FITTING_RESERVE_WIDTH")==id||GetPropID("GP2.FITTING_RESERVE_WIDTH")==id)
	{	//预留金具宽度
		int group=(id==GetPropID("GP2.FITTING_RESERVE_WIDTH"))?1:0;
		sText.Printf("%d",pWireStruTmpl->xarrWireGroups[group].wnReservedWidth);
	}
	else if (GetPropID("GP1.SwingAngle")==id||GetPropID("GP2.SwingAngle")==id)
	{	//主金具摆动角，单位°
		int group=(id==GetPropID("GP2.SwingAngle"))?1:0;
		sText.Printf("%g",pWireStruTmpl->xarrWireGroups[group].sfMasterSwingAngle);
	}
	else if (GetPropID("GP1.URingSizeLabel")==id||GetPropID("GP2.URingSizeLabel")==id)
	{	//挂接U型环型号
		int group=(id==GetPropID("GP2.URingSizeLabel"))?1:0;
		sText.Copy(pWireStruTmpl->xarrWireGroups[group].szUringSizeCode);
	}
	else if (GetPropID("GP1.URingSwingAngle")==id||GetPropID("GP2.URingSwingAngle")==id)
	{	//挂接U形环摆动角，单位°
		int group=(id==GetPropID("GP2.URingSwingAngle"))?1:0;
		sText.Printf("%g",pWireStruTmpl->xarrWireGroups[group].sfSlaveUrSwingAngle);
	}
	if (valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
BOOL CWireStruComPropertyList::HelpPromptProperty(CPropTreeItem* pItem)
{
	g_pSolidDraw->ReleaseSnapStatus();
	if (pItem->m_idProp==GetPropID("Node_N10"))
	{
		if (params.m_pSetupNode10)
			g_pSolidDraw->SetEntSnapStatus(params.m_pSetupNode10->handle);
	}
	else if (pItem->m_idProp==GetPropID("Node_N20"))
	{
		if (params.m_pSetupNode20)
			g_pSolidDraw->SetEntSnapStatus(params.m_pSetupNode20->handle);
	}
	return true;
}
#include "PropertyListOper.h"
bool CWireStruComPropertyList::ModifyValueFunc(CPropTreeItem* pItem,CString &valueStr,BOOL *lpRslt)
{
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	STDTEMPL_PARAM_ITEM* pParam;
	long hNode=HexStrToLong(tem_str);
	CNativePropListManager oper(this);//(pPropList,pDlg);
	if (pItem->m_idProp==GetPropID("Node_N10"))		//上装配节点N10
	{
		if (params.m_pSetupNode10=this->m_pBelongModel->FromNodeHandle(hNode))
			pStdTempl->SetMappingNode(1,params.m_pSetupNode10);
		this->SetItemPropValue(GetPropID("Node_N20"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(2)));
		this->SetItemPropValue(GetPropID("Node_N30"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(3)));
		this->SetItemPropValue(GetPropID("Node_N40"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(4)));
	}
	else if (pItem->m_idProp==GetPropID("Node_N20"))	//下装配节点N20
	{
		if (params.m_pSetupNode20=this->m_pBelongModel->FromNodeHandle(atoi(tem_str)))
			pStdTempl->SetMappingNode(2,atoi(tem_str));
		this->SetItemPropValue(GetPropID("Node_N30"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(3)));
		this->SetItemPropValue(GetPropID("Node_N40"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(4)));
	}
	else if (pItem->m_idProp==GetPropID("Node_N30"))	//下装配节点N20
	{
		params.m_pSetupNode30=this->m_pBelongModel->FromNodeHandle(atoi(tem_str));
		this->SetItemPropValue(GetPropID("Node_N40"),CXhChar16("0x%X",pStdTempl->MappingLdsNodeH(4)));
	}
	else if (pItem->m_idProp==GetPropID("Node_N40"))	//下装配节点N20
	{
		//params.m_pSetupNode40=this->m_pBelongModel->FromNodeHandle(atoi(tem_str));
	}
	else if (pItem->m_idProp==GetPropID("EB_WIDTH"))	//下装配节点N20
	{	//EB金具宽度
		pParam=pStdTempl->listParams.Add(2);	//"EBW"：EB挂点金具宽度+间隙，mm
		pParam->value.fValue=atof(valueStr);
	}
	else if (pItem->m_idProp==GetPropID("WIRE_HOLES_STR"))
	{	//EB安装孔径
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C('D',1)))!=NULL)
		{
			CXhString xhstr(pParam->value.szStrBuf,96);
			xhstr.Copy(valueStr);
		}
	}
	else if (pItem->m_idProp==GetPropID("WIREANGLE_SIZE"))
	{	//挂线T型组合角钢规格
		pParam=pStdTempl->listParams.Add(KEY4C("WAS"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_STRING;
		StrCopy(pParam->value.szStrBuf,valueStr,96);
	}
	else if (pItem->m_idProp==GetPropID("BASE_WIREANGLE_SIZE"))
	{	//挂线底座角钢规格
		pParam=pStdTempl->listParams.Add(KEY4C("BAS"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_STRING;
		StrCopy(pParam->value.szStrBuf,valueStr,96);
	}
	else if (pItem->m_idProp==GetPropID("FIX_CONN_BOLTS_N"))
	{	//挂线短角钢的固接螺栓数
		pParam=pStdTempl->listParams.Add(KEY4C("FBN"));
		pParam->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
		pParam->value.liValue=atoi(valueStr);
	}
	//}
	else if (GetPropID("X")==pItem->m_idProp)
	{	//导线挂点位置X
		params.X1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X")))!=NULL)
			pParam->value.fValue=params.X1;
	}
	else if (GetPropID("X1")==pItem->m_idProp)
	{	//地线挂点位置X1
		params.X1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X1")))!=NULL)
			pParam->value.fValue=params.X1;
	}
	else if (GetPropID("X2")==pItem->m_idProp)
	{	//导线挂点位置X2
		params.X2=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X2")))!=NULL)
			pParam->value.fValue=params.X2;
	}
	else if (GetPropID("Y")==pItem->m_idProp)
	{	//导线挂点位置X
		params.Y1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y")))!=NULL)
			pParam->value.fValue=params.Y1;
	}
	else if (GetPropID("Y1")==pItem->m_idProp)
	{	//地线架宽度Y1
		params.Y1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y1")))!=NULL)
			pParam->value.fValue=params.Y1;
	}
	else if (GetPropID("Y2")==pItem->m_idProp)
	{	//导线架宽度Y2
		params.Y2=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Y2")))!=NULL)
			pParam->value.fValue=params.Y2;
	}
	else if (GetPropID("Z1")==pItem->m_idProp)
	{	//导地线层间距Z1
		params.Z1=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z1")))!=NULL)
			pParam->value.fValue=params.Z1;
	}
	else if (GetPropID("Z2")==pItem->m_idProp)
	{	//导线垂高Z2，导线挂点到塔身顶面垂高位置(允许负值)
		params.Z2=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z2")))!=NULL)
			pParam->value.fValue=params.Z2;
	}
	else if (GetPropID("X3")==pItem->m_idProp)
	{	//地线架伸出量X3
		params.X3=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("X3")))!=NULL)
			pParam->value.fValue=params.X3;
	}
	//AddPropItem("X4",PROPLIST_ITEM(id++,"导线架位置X4",""));
	else if (GetPropID("Z3")==pItem->m_idProp)
	{	//地线架高度Z3
		params.Z3=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z3")))!=NULL)
			pParam->value.fValue=params.Z3;
	}
	else if (GetPropID("Z4")==pItem->m_idProp)
	{	//导线架高度Z4
		params.Z4=atof(tem_str);
		if ((pParam=pStdTempl->listParams.GetValue(KEY4C("Z4")))!=NULL)
			pParam->value.fValue=params.Z4;
	}
	//金具属性
	else if (GetPropID("GP1.szFittingType")==pItem->m_idProp||GetPropID("GP2.szFittingType")==pItem->m_idProp)
	{	//金具类型:0.EB挂板;1.GD挂板;2.U形螺栓
		int group=(pItem->m_idProp==GetPropID("GP2.szFittingType"))?1:0;
		BYTE ciOldType = pWireStruTmpl->xarrWireGroups[group].ciFittingType;
		pWireStruTmpl->xarrWireGroups[group].ciFittingType=tem_str[0]-'0';
		if (ciOldType != pWireStruTmpl->xarrWireGroups[group].ciFittingType)
		{
			//匹配更新金具信息
			if (pWireStruTmpl->xarrWireGroups[group].ciFittingType == 0)
			{
				FITTING_EB *pEBInfo = MatchFitting<FITTING_EB>(pWireStruTmpl->xarrWireGroups[group].cnHoleD);
				if (pEBInfo == NULL)
				{
					IFittingLibrary<FITTING_EB>* pEBLib = FittingLibrary<FITTING_EB>();
					pEBInfo = pEBLib->GetAt(0);
				}
				StrCopy(pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode, pEBInfo->szSizeCode, 17);
			}
			else if (pWireStruTmpl->xarrWireGroups[group].ciFittingType == 1)
			{
				FITTING_GD *pGDInfo = MatchFitting<FITTING_GD>(pWireStruTmpl->xarrWireGroups[group].cnHoleD);
				if (pGDInfo == NULL)
				{
					IFittingLibrary<FITTING_GD>* pGDLib = FittingLibrary<FITTING_GD>();
					pGDInfo = pGDLib->GetAt(0);
				}
				StrCopy(pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode, pGDInfo->szSizeCode, 17);
			}
			char* pszSizeLabel = (group == 0) ? "GP1.szFittingSizeCode" : "GP2.szFittingSizeCode";
			oper.UpdatePropItemValue(pszSizeLabel);
			CPropTreeItem* pFindItem = FindItemByPropId(GetPropID(pszSizeLabel), NULL);
			if (pFindItem)
			{
				if (pWireStruTmpl->xarrWireGroups[group].ciFittingType == 0)
					pFindItem->m_lpNodeInfo->m_cmbItems = MakeFittingSetString(CLDSFitting::TYPE_EB);
				else if (pWireStruTmpl->xarrWireGroups[group].ciFittingType == 1)
					pFindItem->m_lpNodeInfo->m_cmbItems = MakeFittingSetString(CLDSFitting::TYPE_GD);
				else
					pFindItem->m_lpNodeInfo->m_cmbItems = MakeFittingSetString(CLDSFitting::TYPE_UR);
			}
			//匹配更新U形环型号
			CLDSFitting fitting(pWireStruTmpl->xarrWireGroups[group].ciFittingType,
				pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode);
			fitting.MatchUring();
			StrCopy(pWireStruTmpl->xarrWireGroups[group].szUringSizeCode, fitting.sizespecUR, 17);
			char* pszUringSizeLabel = (group == 0) ? "GP1.URingSizeLabel" : "GP2.URingSizeLabel";
			oper.UpdatePropItemValue(pszUringSizeLabel);
			//匹配更新挂孔直径
			pWireStruTmpl->xarrWireGroups[group].cnShaftD = (BYTE)ftoi(fitting.basic.dfTrunnionD);
			pWireStruTmpl->xarrWireGroups[group].cnHoleD = pWireStruTmpl->xarrWireGroups[group].cnShaftD + 2;
			char* pszWireHoleD = (group == 0) ? "GP1.WIRE_HOLE_D" : "GP2.WIRE_HOLE_D";
			oper.UpdatePropItemValue(pszWireHoleD);
			//更新预留金具安装宽度,同参数"EBW"：EB挂点金具宽度+间隙，mm
			pWireStruTmpl->xarrWireGroups[group].wnFittingWidth = ftoi(fitting.basic.dfBoardL);
			pWireStruTmpl->xarrWireGroups[group].wnReservedWidth = pWireStruTmpl->xarrWireGroups[group].wnFittingWidth + 4;
			pParam = pWireStruTmpl->listParams.Add(2);
			pParam->value.fValue = pWireStruTmpl->xarrWireGroups[group].wnReservedWidth;
			oper.UpdatePropItemValue("EB_WIDTH");
		}
	}
	else if (GetPropID("GP1.szFittingSizeCode")==pItem->m_idProp||GetPropID("GP2.szFittingSizeCode")==pItem->m_idProp)
	{	//主金具型号
		int group=(pItem->m_idProp==GetPropID("GP2.szFittingSizeCode"))?1:0;
		StrCopy(pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode,tem_str,17);
		CLDSFitting fitting(pWireStruTmpl->xarrWireGroups[group].ciFittingType,
							pWireStruTmpl->xarrWireGroups[group].szFittingSizeCode);
		fitting.MatchUring();
		StrCopy(pWireStruTmpl->xarrWireGroups[group].szUringSizeCode,fitting.sizespecUR,17);
		//匹配更新U形环型号
		char* pszUringSizeLabel=(group==0)?"GP1.URingSizeLabel":"GP2.URingSizeLabel";
		oper.UpdatePropItemValue(pszUringSizeLabel);
		//匹配更新挂孔直径
		pWireStruTmpl->xarrWireGroups[group].cnShaftD=(BYTE)ftoi(fitting.basic.dfTrunnionD);
		pWireStruTmpl->xarrWireGroups[group].cnHoleD=(BYTE)ftoi(fitting.basic.dfTrunnionD)+2;
		char* pszWireHoleD=(group==0)?"GP1.WIRE_HOLE_D":"GP2.WIRE_HOLE_D";
		oper.UpdatePropItemValue(pszWireHoleD);
		//更新预留金具安装宽度,同参数"EBW"：EB挂点金具宽度+间隙，mm
		pWireStruTmpl->xarrWireGroups[group].wnFittingWidth=ftoi(fitting.basic.dfBoardL);
		pWireStruTmpl->xarrWireGroups[group].wnReservedWidth=ftoi(fitting.basic.dfBoardL)+4;
		char* pszFittingSpace=(group==0)?"GP1.FITTING_RESERVE_WIDTH":"GP2.FITTING_RESERVE_WIDTH";
		oper.UpdatePropItemValue(pszFittingSpace);
		pParam=pWireStruTmpl->listParams.Add(2);
		pParam->value.fValue=pWireStruTmpl->xarrWireGroups[group].wnReservedWidth;
		oper.UpdatePropItemValue("EB_WIDTH");
	}
	else if (GetPropID("GP1.WIRE_HOLE_COUNT")==pItem->m_idProp||GetPropID("GP2.WIRE_HOLE_COUNT")==pItem->m_idProp)
	{	//挂线角钢的挂孔数量（即主金具数量）
		int group=(pItem->m_idProp==GetPropID("GP2.WIRE_HOLE_COUNT"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].cnWirePointCount=atoi(tem_str);
	}
	else if (GetPropID("GP1.WIRE_HOLE_D")==pItem->m_idProp||GetPropID("GP2.WIRE_HOLE_D")==pItem->m_idProp)
	{	//预留金具安装孔径
		int group=(pItem->m_idProp==GetPropID("GP2.WIRE_HOLE_D"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].cnHoleD=atoi(tem_str);
	}
	else if (GetPropID("GP1.LOCATE_SPACE")==pItem->m_idProp||GetPropID("GP2.LOCATE_SPACE")==pItem->m_idProp)
	{	//开档距离
		int group=(pItem->m_idProp==GetPropID("GP2.LOCATE_SPACE"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].wiSpaceL=atoi(tem_str);
	}
	else if (GetPropID("GP1.FITTING_RESERVE_WIDTH")==pItem->m_idProp||GetPropID("GP2.FITTING_RESERVE_WIDTH")==pItem->m_idProp)
	{	//预留金具宽度
		int group=(pItem->m_idProp==GetPropID("GP2.FITTING_RESERVE_WIDTH"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].wnReservedWidth=atoi(tem_str);
	}
	else if (GetPropID("GP1.SwingAngle")==pItem->m_idProp||GetPropID("GP2.SwingAngle")==pItem->m_idProp)
	{	//主金具摆动角，单位°
		int group=(pItem->m_idProp==GetPropID("GP2.SwingAngle"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].sfMasterSwingAngle=(float)atof(tem_str);
	}
	else if (GetPropID("GP1.URingSizeLabel")==pItem->m_idProp||GetPropID("GP2.URingSizeLabel")==pItem->m_idProp)
	{	//挂接U型环型号
		int group=(pItem->m_idProp==GetPropID("GP2.URingSizeLabel"))?1:0;
		StrCopy(pWireStruTmpl->xarrWireGroups[group].szUringSizeCode,tem_str,17);
	}
	else if (GetPropID("GP1.URingSwingAngle")==pItem->m_idProp||GetPropID("GP2.URingSwingAngle")==pItem->m_idProp)
	{	//挂接U形环摆动角，单位°
		int group=(pItem->m_idProp==GetPropID("GP2.URingSwingAngle"))?1:0;
		pWireStruTmpl->xarrWireGroups[group].sfSlaveUrSwingAngle=(float)atof(tem_str);
	}
	return TRUE;
}
bool CWireStruComPropertyList::ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt)
{
	CCallBackDialog* pCallBackDlg=(CCallBackDialog*)GetParent();
	if (pItem->m_idProp==GetPropID("Node_N10"))
		pCallBackDlg->SelectObject(CLS_NODE,pItem->m_idProp);
	else if (pItem->m_idProp==GetPropID("Node_N20"))
		pCallBackDlg->SelectObject(CLS_NODE,pItem->m_idProp);
	else if(pItem->m_idProp == GetPropID("Node_N30"))
		pCallBackDlg->SelectObject(CLS_NODE, pItem->m_idProp);
	else if(pItem->m_idProp == GetPropID("Node_N40"))
		pCallBackDlg->SelectObject(CLS_NODE, pItem->m_idProp);
	return TRUE;
}
bool CWireStruComPropertyList::CreateComTemplInstance()
{
	if (!this->pWireStruTmpl->TranslateTmplInstance())
		return false;
	CStdComModel modeler(&Ta);
	modeler.BuildModel(this->pStdTempl);
	return TRUE;
}
bool CWireStruComPropertyList::CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt)
{
	return true;
}
BOOL CWireStruComPropertyList::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	CCallBackDialog* pCallBackDlg=(CCallBackDialog*)GetParent();
	CLDSNode* pNode=NULL;
	if (idEventProp==GetPropID("Node_N10"))
	{
		if ((pNode=m_pBelongModel->FromNodeHandle(hPickObj))!=NULL)
		{
			this->pSetupNode10=pNode;
			this->pStdTempl->SetMappingNode(1,pNode);
			if (pStdTempl->Id==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_GD||
				pStdTempl->Id==IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1)
			{
				MAP_RODnNODE keynode(1,pSetupNode10);
				((CStdComTemplWireStructure*)pStdTempl)->SetRecogKeyNodesAndRods(&keynode);			
			}
		}
	}
	else if (idEventProp==GetPropID("Node_N20"))
	{
		if ((pNode=m_pBelongModel->FromNodeHandle(hPickObj))!=NULL)
		{
			this->pSetupNode20=pNode;
			this->pStdTempl->SetMappingNode(2,pNode);
			if (pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_NZ_FRT_2_GD||
				pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD||
				pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2)
			{
				if (pSetupNode10!=NULL&&pSetupNode20!=NULL)
				{
					MAP_RODnNODE keynodes[2];
					keynodes[0].Set(1,pSetupNode10);
					keynodes[1].Set(2,pSetupNode20);
					((CStdComTemplWireStructure*)pStdTempl)->SetRecogKeyNodesAndRods(keynodes,2);
				}
			}
		}
	}
	else if (idEventProp == GetPropID("Node_N30"))
	{
		if ((pNode = m_pBelongModel->FromNodeHandle(hPickObj)) != NULL)
		{
			this->pSetupNode30 = pNode;
			this->pStdTempl->SetMappingNode(3, pNode);
			if (pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD)
			{
				if (pSetupNode10 != NULL && pSetupNode20 != NULL && this->pSetupNode30!=NULL)
				{
					MAP_RODnNODE keynodes[3];
					keynodes[0].Set(1, pSetupNode10);
					keynodes[1].Set(2, pSetupNode20);
					keynodes[2].Set(3, pSetupNode30);
					((CStdComTemplWireStructure*)pStdTempl)->SetRecogKeyNodesAndRods(keynodes, 3);
				}
			}
		}
	}
	else if (idEventProp == GetPropID("Node_N40"))
	{
		if ((pNode = m_pBelongModel->FromNodeHandle(hPickObj)) != NULL)
		{
			this->pSetupNode40 = pNode;
			this->pStdTempl->SetMappingNode(4, pNode);
		}
	}
	else
		return false;
	return true;
}
void CWireStruComPropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	CleanTree();
	CNativePropListManager oper(this);
	CPropTreeItem *pBasicItem=NULL,*pGroupItem=NULL,*pPropItem=NULL,*pRoot=GetRootItem();
	if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1 )
	{	//EB挂点
		//基本信息
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		//装配信息
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20");
		oper.InsertButtonPropItem(pGroupItem,"Node_N30");
		oper.InsertButtonPropItem(pGroupItem,"Node_N40");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#3");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#4");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#5");
#endif
		//第一组挂孔参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
		//第二组挂孔参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup2", "2#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP2.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP2.LOCATE_SPACE");		//开档距离
		oper.InsertEditPropItem(pGroupItem, "GP2.WIRE_HOLE_D");
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP2.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP2.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP2.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP2.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP2.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2)
	{	//EB挂点
		//基本信息
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		//装配信息
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20");
		oper.InsertButtonPropItem(pGroupItem,"Node_N30");
		oper.InsertButtonPropItem(pGroupItem,"Node_N40");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#3");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#4");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#5");
#endif
		//第一组挂孔参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		oper.InsertEditPropItem(pGroupItem, "GP1.LOCATE_SPACE");		//开档距离
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3)
	{
		//基本信息
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		//装配信息
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "特征节点N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "特征节点N20");
		oper.InsertButtonPropItem(pGroupItem, "Node_N30", "特征节点N30");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#4","T型横材角钢");
#endif
		//第一组挂孔参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.LOCATE_SPACE");		//开档距离
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1 )
	{	//EB挂点
		//基本信息
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		//装配信息
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","特征节点N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20","节点N20");
		oper.InsertButtonPropItem(pGroupItem,"Node_N30","节点N30");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
#endif
		//第一组挂孔参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2)
	{
		//基本信息
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//装配信息
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "特征节点N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "特征节点N20");

#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#3");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#4");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#5");
#endif
		//挂孔设计参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		this->pWireStruTmpl->xarrWireGroups[0].ciFittingType = CLDSFitting::TYPE_EB;	//提前将主金具设定为EB类型
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//金具类型
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3)
	{
		//基本信息
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		//装配信息
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "特征节点N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "特征节点N20");
		oper.InsertButtonPropItem(pGroupItem, "Node_N30", "特征节点N30");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#4","T型横材角钢");
#endif
		//第一组挂孔参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.LOCATE_SPACE");		//开档距离
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB||
		pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB)
	{	//EB挂点
		//基本信息
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//装配信息
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","特征节点N10");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#1","T型横材角钢");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2", "横担主材角钢");
		//挂孔设计参数
		pGroupItem=oper.InsertPropItem(pRoot,"WirePointGroup1","1#挂孔设计参数");
		pPropItem=oper.InsertEditPropItem(pGroupItem,"GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		if (pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB)
			oper.InsertEditPropItem(pGroupItem, "GP1.LOCATE_SPACE");		//开档距离
		oper.InsertEditPropItem(pGroupItem,"GP1.WIRE_HOLE_D");		//预留金具安装孔径
		this->pWireStruTmpl->xarrWireGroups[0].ciFittingType=CLDSFitting::TYPE_EB;	//提前将主金具设定为EB类型
		pPropItem=oper.InsertCmbListPropItem(pGroupItem,"GP1.szFittingType");		//金具类型
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem,"GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_EB));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem,"GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD||
		pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_GD)
	{	//GD挂点
		//基本信息
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertCmbListPropItem(pBasicItem, "BASE_WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//装配信息
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","特征节点N10");
		oper.InsertButtonPropItem(pGroupItem,"ParaRod#1","横担正面主材角钢");
		//挂孔设计参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//金具类型
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_GD));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_STRU_NZ_FRT_2_GD)
	{	//GD挂点
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertCmbListPropItem(pBasicItem, "BASE_WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","特征节点N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20","特征节点N20");
		oper.InsertButtonPropItem(pGroupItem,"Node_N30","节点N30");
		oper.InsertButtonPropItem(pGroupItem,"Node_N40","节点N40");
#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#3");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#4");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#5");
#endif
		//挂孔设计参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//金具类型
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_GD));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD)
	{
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertCmbListPropItem(pBasicItem, "BASE_WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "特征节点N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "特征节点N20");
	#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#2");
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#3");
	#endif
		//挂孔设计参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//金具类型
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_GD));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (pStdTempl->GetTmplId() == IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD)
	{	//GD挂点
		pBasicItem = oper.InsertPropItem(pRoot, "BasicInfo");
		oper.InsertCmbListPropItem(pBasicItem, "WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertCmbListPropItem(pBasicItem, "BASE_WIREANGLE_SIZE", MakeAngleSetString());
		oper.InsertEditPropItem(pBasicItem, "EB_WIDTH");	//EB金具宽度
		oper.InsertCmbEditPropItem(pBasicItem, "FIX_CONN_BOLTS_N");
		//
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem, "Node_N10", "特征节点N10");
		oper.InsertButtonPropItem(pGroupItem, "Node_N20", "特征节点N20");
		oper.InsertButtonPropItem(pGroupItem, "Node_N30", "特征节点N30");
	#ifdef _DEBUG
		oper.InsertButtonPropItem(pGroupItem, "ParaRod#1");
	#endif
		//挂孔设计参数
		pGroupItem = oper.InsertPropItem(pRoot, "WirePointGroup1", "1#挂孔设计参数");
		pPropItem = oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_COUNT");	//挂孔数量
		pPropItem->SetReadOnly();
		oper.InsertEditPropItem(pGroupItem, "GP1.WIRE_HOLE_D");		//预留金具安装孔径
		pPropItem = oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingType");		//金具类型
		pPropItem->SetReadOnly();
		oper.InsertCmbListPropItem(pGroupItem, "GP1.szFittingSizeCode", MakeFittingSetString(CLDSFitting::TYPE_GD));	//EB金具型号
		oper.InsertEditPropItem(pGroupItem, "GP1.SwingAngle");
		oper.InsertCmbListPropItem(pGroupItem, "GP1.URingSizeLabel", MakeFittingSetString(CLDSFitting::TYPE_UR));	//挂接U型环型号
		oper.InsertEditPropItem(pGroupItem, "GP1.URingSwingAngle");
	}
	else if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_FRAMELINE_HYJ1)
	{
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","上装配节点N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20","下装配节点N20");
		//一级尺寸设计参数	
		pGroupItem=oper.InsertPropItem(pRoot,"DimensionParamLevel1");
		oper.InsertEditPropItem(pGroupItem,"X1");	//地线挂点横向位置X1
		oper.InsertEditPropItem(pGroupItem,"X2");	//导线挂点横向位置X2
		oper.InsertEditPropItem(pGroupItem,"Y1");	//地线架宽度Y1
		oper.InsertEditPropItem(pGroupItem,"Y2");	//导线架宽度Y2
		oper.InsertEditPropItem(pGroupItem,"Z1");	//层间距Z1
		oper.InsertEditPropItem(pGroupItem,"Z2");	//导线垂高Z2
		//二级尺寸设计参数	
		pGroupItem=oper.InsertPropItem(pRoot,"DimensionParamLevel2");
		oper.InsertEditPropItem(pGroupItem,"X3");	//地线支架伸出量X3
		//oper.InsertEditPropItem(pGroupItem,"X4");	//导线架根部横向位置X4
		oper.InsertEditPropItem(pGroupItem,"Z3");	//地线架高度Z3
		oper.InsertEditPropItem(pGroupItem,"Z4");	//导线架高度Z4
		//工作面腹材填充参数
		pGroupItem=oper.InsertPropItem(pRoot,"PanelInfo");
	}
	else if (this->pStdTempl->GetTmplId()==IStdComTempl::STDTMPL_FRAMELINE_HX3)
	{
		pGroupItem = oper.InsertPropItem(pRoot, "SetupInfo");
		oper.InsertButtonPropItem(pGroupItem,"Node_N10","上装配节点N10");
		oper.InsertButtonPropItem(pGroupItem,"Node_N20","下装配节点N20");
		pGroupItem=oper.InsertPropItem(pRoot,"DimensionParamLevel1");
		oper.InsertEditPropItem(pGroupItem,"X");	//地线挂点横向位置X1
		oper.InsertEditPropItem(pGroupItem,"Y");	//地线架宽度Y1
		//二级尺寸设计参数	
		pGroupItem=oper.InsertPropItem(pRoot,"DimensionParamLevel2");
		oper.InsertEditPropItem(pGroupItem,"Z1");	//层间距Z1
		oper.InsertEditPropItem(pGroupItem,"Z2");	//导线垂高Z2
	}
	//
	Redraw();
}
bool CWireStruComPropertyList::DisplaySlavePropertyList(CPropTreeItem* pItem,DWORD dwRelaInfo/*=0*/)
{
	if (this->m_pRelaSubPropList==NULL)
		return false;
	CNativePropListManager oper(m_pRelaSubPropList);
	CPropTreeItem *pGroupItem=NULL,*pPropItem=NULL,*pRoot=m_pRelaSubPropList->GetRootItem();
	m_pRelaSubPropList->CleanTree();
	//m_pRelaSubPropList->SetModifyValueFunc(ModifyProperty);
	//
	//pGroupItem=oper.InsertPropItem(pRoot,"PlateInfo");
	//oper.InsertEditPropItem(pGroupItem,"PlateThick");
	//oper.InsertCmbListPropItem(pGroupItem,"PlateMat",MakeMaterialMarkSetString());
	//if(params.m_ciWalkWayType==0)
	//	oper.InsertEditPropItem(pGroupItem,"para.A");
	//else
	//{
	//	oper.InsertEditPropItem(pGroupItem,"para.B");
	//	oper.InsertEditPropItem(pGroupItem,"para.W");
	//	//oper.InsertEditPropItem(pGroupItem,"para.H");
	//	//
	//	oper.InsertCmbEditPropItem(pGroupItem,"BoltMd","12|16|20|24","","",-1,TRUE);
	//	oper.InsertEditPropItem(pGroupItem,"BoltNum","","",-1,TRUE);
	//	oper.InsertEditPropItem(pGroupItem,"BoltSpace","","",-1,TRUE);
	//}
	////
	//m_pRelaSubPropList->Redraw();
	return true;
}
void CWireStruComPropertyList::UpdateSketchMap()
{
	if (m_pSketchBitmap==NULL)
		return;
	if (pStdTempl->sketchimg.bmWidth==0)
		pStdTempl->LoadSketchImage();
	m_pSketchBitmap->Detach();
	m_pSketchBitmap->CreateBitmapIndirect(&this->pStdTempl->sketchimg);
	HBITMAP hBitmap=(HBITMAP)m_pSketchBitmap->GetSafeHandle();
	m_pSketchPanel->SetBitmap(hBitmap);
}
void CWireStruComPropertyList::DelWorkPlaneSketch()
{
	g_pSolidDraw->DelWorkPlaneSketch(1);
	g_pSolidDraw->DelWorkPlaneSketch(2);
	g_pSolidDraw->Draw();
}
void CWireStruComPropertyList::WorkPlaneSketch()
{

}
bool CWireStruComPropertyList::InitializeUI(bool bInternalStart/*=false*/,DWORD idStdComTmpl/*=0*/)
{
	if (!bInternalStart)
	{
		CStdComTempl* pWireStruTmpl=globalStdComTmplLibrary.LoadStdComTempl(idStdComTmpl);
		if (pWireStruTmpl == NULL)
			return false;
		xWireStruTmpl.Id = idStdComTmpl;
		xWireStruTmpl.CloneStdTmplFrom(pWireStruTmpl);
		//
		InitPropHashtable();
	}
	UpdateSketchMap();
	WorkPlaneSketch();
	return true;
}
bool CWireStruComPropertyList::ConfirmOK()
{
	return true;
}
bool CWireStruComPropertyList::CancelUI()
{
	DelWorkPlaneSketch();
	return true;
}
#endif
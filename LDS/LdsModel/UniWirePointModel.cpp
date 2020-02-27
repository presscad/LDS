#include "stdafx.h"
#include "UniWirePointModel.h"
#include "CreateFace.h"
#include "SolidBody.h"
#include "Markup.h"
#include "I_DrawSolid.h"

#if defined(_DEBUG)&&!defined(_DISABLE_DEBUG_NEW_)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//CUniWirePoint
#if !defined(DISABLE_SOLID_BODY)&&!defined(_HANDOVER_TO_CLIENT_)
#include "f_ent_list.h"

#ifdef __RAPID_ROUGH_3DMODEL_
#include "..\StdPartSolid\StdMetaSolid.h"
bool CUniWirePoint::Create3dSolidModel(UINT slices/*=6*/)
{
	if(pSolidBody!=NULL&& !IsSolidModified())
		return true;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	SetModified(false);
	fBody body;
	//acs.axis_x方向为挂接金具耳轴方向，acs.axis_z为挂线方向
	//挂接金具模拟正方体，边长5mm
	int i;
	GEPOINT vertex_arr[20];
	//模拟金具正方体顶点
	vertex_arr[0].Set(-2.5,-2.5);
	vertex_arr[1].Set( 2.5,-2.5);
	vertex_arr[2].Set( 2.5, 2.5);
	vertex_arr[3].Set(-2.5, 2.5);
	for(i=4;i<8;i++)
	{
		vertex_arr[i]=vertex_arr[i-4];
		vertex_arr[i].z=5;
	}
	COLORREF crWireArrow=RGB(0,255,255);//g_sysPara.crTsaMode.crLoadArrow;
	CCreateFace creator;
	creator.InitVertexList(&body);
	//金具模拟底座立方体底面
	f3dPolyFace* pFace=body.faceList.append();
	pFace->material=crWireArrow;
	pFace->poly_norm.Set(0,0,-1);
	creator.NewOutterEdgeLine(pFace,3,0);
	creator.NewOutterEdgeLine(pFace,2);
	creator.NewOutterEdgeLine(pFace,1);
	creator.NewOutterEdgeLine(pFace,0);
	//金具模拟底座立方体顶面
	pFace=body.faceList.append();
	pFace->material=crWireArrow;
	pFace->poly_norm.Set(0,0,1);
	creator.NewOutterEdgeLine(pFace,5,4);
	creator.NewOutterEdgeLine(pFace,6);
	creator.NewOutterEdgeLine(pFace,7);
	creator.NewOutterEdgeLine(pFace,4);
	//金具模拟底座立方体侧面
	for(i=0;i<4;i++)
	{
		pFace=body.faceList.append();
		pFace->material=crWireArrow;
		creator.NewOutterEdgeLine(pFace,(i+1)%4,i);
		creator.NewOutterEdgeLine(pFace,(i+1)%4+4);
		creator.NewOutterEdgeLine(pFace, i     +4);
		creator.NewOutterEdgeLine(pFace, i);
	}
	//模拟挂线方向箭头六棱柱
	GECS stdcs;
	stdcs.InitStdCS();
	stdcs.origin.z=5;
	METASOLID::AppendDiskSolid(&body,3,600,stdcs,6,crWireArrow);
	//装配到模型实际位置
	UCS_STRU ucs(acs);
	for(f3dPoint* pVertice=body.vertex.GetFirst();pVertice;pVertice=body.vertex.GetNext())
		*pVertice=acs.TransPFromCS(*pVertice);
	pSolidBody->ConvertFrom(&body);
	return true;
}
#else
bool CUniWirePoint::Create3dSolidModel(UINT slices/*=6*/){return false;}
#endif
CSolidPart CUniWirePoint::GetSolidPartObject(double dfArrowLength/*=2000*/)
{
	CSolidPart solidpart;
	solidpart.m_idClassType=CLS_WIREARROW;
	//solidpart.m_hPart=gxWirePointModel.m_hashWirePoints.GetCursorKey();//pWirePoint->m_hLdsObj;
	solidpart.hiberid.masterId=this->Id;
	solidpart.hiberid.SetHiberarchy(2,0);	//2表示挂点示意箭头
	solidpart.m_idClassType = CLS_WIREARROW;
	solidpart.acs = acs;
	solidpart.pBody = pSolidBody;

	solidpart.m_bDisplaySingle=true;
	solidpart.m_iLineType = 3;	//箭头
	solidpart.dfSphereRadius=0;
	solidpart.line.startPt=this->xPosition;
	solidpart.line.endPt=this->xPosition+this->vWirePull*dfArrowLength;
	return solidpart;
}
#endif
//////////////////////////////////////////////////////////////////////////
//CXhWirePoint
CXhWirePoint::CXhWirePoint()
{
	m_ciLevel=0;
	m_hRelaPart = 0;
	m_hRelaNode = 0;
	m_bRelyAngle=true;
	m_fSpaceWidth=m_fHoleD=0;
	holes[0].hBolt=holes[0].hResidePart=0;
	holes[1].hBolt=holes[1].hResidePart=0;
	szLayer[0]=szLayer[1]=szLayer[2]=szLayer[3]=0;
	pSolidBody=NULL;
}
CXhWirePoint::~CXhWirePoint()
{
	if(pSolidBody)
		delete pSolidBody;
}
char* CXhWirePoint::get_szLayer()
{
	_szLayer[3]=0;
	return _szLayer;
}
char* CXhWirePoint::set_szLayer(const char* layer)
{
	StrCopy(_szLayer,layer,4);
	return _szLayer;
}
UINT GetCirclePolyVertex(double radius, GEPOINT* parrVertexes, UINT uiMaxCount,bool blIntelliCalSlices/*=true*/);
bool CXhWirePoint::Create3dSolidModel(UINT slices/*=6*/)
{
	if(pSolidBody!=NULL)
		return true;
	else
		pSolidBody=new CSolidBody();
	double d=m_fHoleD-2;
	PRESET_ARRAY16<GEPOINT> xarrBtmVertexes;
	UINT i;
	slices=GetCirclePolyVertex(d*0.5,xarrBtmVertexes.pPresetObjs,16,false);
	GECS acs;
	acs.origin=this->m_xBasicPoint;
	acs.axis_z=this->m_vStdBasicAxisVec;
	acs.axis_x=inters_vec(f3dPoint(acs.axis_z));
	acs.axis_y=acs.axis_z^acs.axis_x;
	normalize(acs.axis_y);
	acs.axis_x=acs.axis_y^acs.axis_z;
	//
	fBody body;
	for(i=0;i<slices;i++)
	{
		body.vertex.append(acs.TransPFromCS(xarrBtmVertexes[i]));	//底面外圆弧点
		xarrBtmVertexes[i].z=this->m_fSpaceWidth;
		body.vertex.append(acs.TransPFromCS(xarrBtmVertexes[i]));	//顶面外圆弧点
	}
	body.vertex.append(this->m_xBasicPoint);
	GEPOINT xTopCenter=this->m_xBasicPoint+this->m_vStdBasicAxisVec*this->m_fSpaceWidth;
	body.vertex.append(xTopCenter);
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(&body);

	f3dPoint axis_pick_start,axis_pick_end;
	//钢管底面创建
	f3dPolyFace *pFace;
	COLORREF crShaft=CLDSPart::GetColor(CLS_POLYJOINT,-1,' ');
	for(i=0;i<slices;i++)
	{
		//由于不共面，所以整个底面拆分成若干三角面
		pFace=body.faceList.append();
		pFace->material = crShaft;
		createFace.NewOutterEdgeLine(pFace,slices*2+0,i*2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2);
		createFace.NewOutterEdgeLine(pFace,i*2);
	}
	//钢管顶面创建
	for(i=0;i<slices;i++)
	{
		//由于不共面，所以整个底面拆分成若干三角面
		pFace=body.faceList.append();
		pFace->material = crShaft;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2+1,i*2+1);
		createFace.NewOutterEdgeLine(pFace,slices*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2+1);
	}
	//轴外侧面创建
	for(i=0;i<slices;i++)
	{
		//钢管外侧面创建
		pFace = body.faceList.append();
		pFace->material = crShaft;
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2+0,i*2);
		createFace.NewOutterEdgeLine(pFace,((i+1)%slices)*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2+1);
		createFace.NewOutterEdgeLine(pFace,i*2);
	}
	pSolidBody->ConvertFrom(&body);
	return true;
}
void CXhWirePoint::CloneProp(CXhWirePoint* pSrcWriePoint)
{
	m_sGimName = pSrcWriePoint->m_sGimName;
	m_szStyleName = pSrcWriePoint->m_szStyleName;
	m_xWirePlace = pSrcWriePoint->m_xWirePlace;
	m_hRelaNode = pSrcWriePoint->m_hRelaNode;
	m_hRelaPart = pSrcWriePoint->m_hRelaPart;
	m_fHoleD = pSrcWriePoint->m_fHoleD;
	m_fHalfConeDegAngle = pSrcWriePoint->m_fHalfConeDegAngle;
	m_fSpaceWidth = pSrcWriePoint->m_fSpaceWidth;
	holes[0].hBolt = pSrcWriePoint->holes[0].hBolt;
	holes[0].hResidePart = pSrcWriePoint->holes[0].hResidePart;
	holes[1].hBolt = pSrcWriePoint->holes[1].hBolt;
	holes[1].hResidePart = pSrcWriePoint->holes[1].hResidePart;
}

IMPLEMENT_PROP_FUNC(CXhWirePoint);
void CXhWirePoint::InitPropHashtable()
{
	int id = 1;
	CXhWirePoint::propHashtable.SetHashTableGrowSize(500);
	CXhWirePoint::propStatusHashtable.CreateHashTable(50);
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("BasicInfo", PROPLIST_ITEM(id++, "BasicInfo"));
	AddPropItem("GimName", PROPLIST_ITEM(id++, "GimName"));
	AddPropItem("WireType", PROPLIST_ITEM(id++, "WireType", "", "D|E|J"));
	AddPropItem("PhaseSerial", PROPLIST_ITEM(id++, "PhaseSerial"));
	AddPropItem("CircuitSerial", PROPLIST_ITEM(id++, "CircuitSerial"));
	AddPropItem("CircuitDC", PROPLIST_ITEM(id++, "CircuitDC", "", "BC|DC"));
	AddPropItem("TensionType", PROPLIST_ITEM(id++, "TensionType", "", ""));
	AddPropItem("LocationInfo", PROPLIST_ITEM(id++, "LocationInfo"));
	AddPropItem("Level", PROPLIST_ITEM(id++, "Level", "", "0.MOD|1.Solid"));
	AddPropItem("RelaNode", PROPLIST_ITEM(id++, "RelaNode"));
	AddPropItem("HoleD", PROPLIST_ITEM(id++, "HoleD"));
	AddPropItem("HOLE1", PROPLIST_ITEM(id++, "HOLE1"));
	AddPropItem("HOLE1.hBolt", PROPLIST_ITEM(id++, "hBolt"));
	AddPropItem("HOLE1.hResidePart", PROPLIST_ITEM(id++, "hResidePart"));
	AddPropItem("HOLE2", PROPLIST_ITEM(id++, "HOLE2"));
	AddPropItem("HOLE2.hBolt", PROPLIST_ITEM(id++, "hBolt"));
	AddPropItem("HOLE2.hResidePart", PROPLIST_ITEM(id++, "hResidePart"));
	AddPropItem("SpaceWidth", PROPLIST_ITEM(id++, "SpaceWidth"));
	AddPropItem("Location", PROPLIST_ITEM(id++, "定位信息"));
	AddPropItem("ucs.orgion", PROPLIST_ITEM(id++, "orgion"));
	AddPropItem("ucs.orgion.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ucs.orgion.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ucs.orgion.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("ucs.axis_x", PROPLIST_ITEM(id++, "axis_x"));
	AddPropItem("ucs.axis_x.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ucs.axis_x.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ucs.axis_x.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("ucs.axis_y", PROPLIST_ITEM(id++, "axis_y"));
	AddPropItem("ucs.axis_y.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ucs.axis_y.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ucs.axis_y.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("ucs.axis_z", PROPLIST_ITEM(id++, "axis_z"));
	AddPropItem("ucs.axis_z.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ucs.axis_z.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ucs.axis_z.z", PROPLIST_ITEM(id++, "Z"));
#else
	AddPropItem("BasicInfo", PROPLIST_ITEM(id++, "挂点信息", "", "添加|更新"));
	AddPropItem("GimName", PROPLIST_ITEM(id++, "挂点名称"));
	AddPropItem("WireType", PROPLIST_ITEM(id++, "线缆类型", "", "导线|地线|跳线"));
	AddPropItem("PhaseSerial", PROPLIST_ITEM(id++, "相序号", "", "1|2|3"));
	AddPropItem("CircuitSerial", PROPLIST_ITEM(id++, "回路号", "", "1|2|3"));
	AddPropItem("CircuitDC", PROPLIST_ITEM(id++, "回路类型", "", "交流|直流"));
	AddPropItem("TensionType", PROPLIST_ITEM(id++, "受力类型", "", "悬垂|耐张"));
	AddPropItem("WireDirec", PROPLIST_ITEM(id++, "挂点方位", "", "前侧|后侧"));
	AddPropItem("HangingStyle", PROPLIST_ITEM(id++, "装配对象", "该挂点所归属串形", "单串|双串|V串"));
	AddPropItem("PostCode", PROPLIST_ITEM(id++, "附加码", "", "1|2|3"));
	AddPropItem("Level", PROPLIST_ITEM(id++, "定义级别", "", "0.MOD级别|1.Solid级别"));
	AddPropItem("RelaNode", PROPLIST_ITEM(id++, "关联节点"));
	AddPropItem("JinjuInfo", PROPLIST_ITEM(id++, "金具信息"));
	AddPropItem("Fittings", PROPLIST_ITEM(id++, "挂接金具"));
	AddPropItem("HalfDegAngle", PROPLIST_ITEM(id++, "活动半角"));
	AddPropItem("StyleName", PROPLIST_ITEM(id++, "挂串名称"));
	AddPropItem("HoleD", PROPLIST_ITEM(id++, "挂孔大小"));
	AddPropItem("HOLE1", PROPLIST_ITEM(id++, "实体挂孔1"));
	AddPropItem("HOLE1.hBolt", PROPLIST_ITEM(id++, "关联螺栓"));
	AddPropItem("HOLE1.hResidePart", PROPLIST_ITEM(id++, "所在构件"));
	AddPropItem("HOLE2", PROPLIST_ITEM(id++, "实体挂孔2"));
	AddPropItem("HOLE2.hBolt", PROPLIST_ITEM(id++, "关联螺栓"));
	AddPropItem("HOLE2.hResidePart", PROPLIST_ITEM(id++, "所在构件"));
	AddPropItem("SpaceWidth", PROPLIST_ITEM(id++, "挂孔间距"));
	AddPropItem("Location", PROPLIST_ITEM(id++, "定位信息"));
	AddPropItem("ucs.orgion", PROPLIST_ITEM(id++, "原点"));
	AddPropItem("ucs.orgion.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ucs.orgion.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ucs.orgion.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("ucs.axis_x", PROPLIST_ITEM(id++, "X轴"));
	AddPropItem("ucs.axis_x.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ucs.axis_x.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ucs.axis_x.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("ucs.axis_y", PROPLIST_ITEM(id++, "Y轴"));
	AddPropItem("ucs.axis_y.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ucs.axis_y.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ucs.axis_y.z", PROPLIST_ITEM(id++, "Z"));
	AddPropItem("ucs.axis_z", PROPLIST_ITEM(id++, "Z轴"));
	AddPropItem("ucs.axis_z.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("ucs.axis_z.y", PROPLIST_ITEM(id++, "Y"));
	AddPropItem("ucs.axis_z.z", PROPLIST_ITEM(id++, "Z"));

#endif
}
int CXhWirePoint::GetPropValueStr(long id, char *valueStr, UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if (GetPropID("GimName") == id)
		sText.Copy(m_sGimName);
	else if (GetPropID("WireType") == id)
	{
		if (m_xWirePlace.ciWireType == 'C')
			sText.Copy("导线");
		else if (m_xWirePlace.ciWireType == 'E')
			sText.Copy("地线");
		else if (m_xWirePlace.ciWireType == 'J')
			sText.Copy("跳线");
	}
	else if (GetPropID("PhaseSerial") == id)
		sText.Printf("%d", m_xWirePlace.ciPhaseSerial);
	else if (GetPropID("CircuitSerial") == id)
		sText.Printf("%d", m_xWirePlace.ciCircuitSerial);
	else if (GetPropID("CircuitDC") == id)
	{
		if (m_xWirePlace.blCircuitDC)
			sText.Copy("直流");
		else
			sText.Copy("交流");
	}
	else if (GetPropID("TensionType") == id)
	{
		if (m_xWirePlace.ciTensionType == 1)
			sText.Copy("悬垂");
		else if (m_xWirePlace.ciTensionType == 2)
			sText.Copy("耐张");
		else if (m_xWirePlace.ciTensionType == 3)
			sText.Copy("终端");
		else
			sText.Copy("未设定");
	}
	else if (GetPropID("HangingStyle") == id)
	{
		if (m_xWirePlace.ciHangingStyle == 'S')
			sText.Copy("双串");
		else if (m_xWirePlace.ciHangingStyle == 'V')
			sText.Copy("V串");
		else
			sText.Copy("单串");
	}
	else if (GetPropID("PostCode") == id)
		sText.Printf("%d", m_xWirePlace.ciPostCode);
	else if (GetPropID("WireDirec") == id)
	{
		if (ciWirePosSymbol == 'Q')
			sText.Copy("前侧");
		else if (ciWirePosSymbol == 'H')
			sText.Copy("后侧");
		else
			sText.Copy("无");
	}
	else if (GetPropID("Level") == id)
	{
		if (m_ciLevel == 0)
			sText.Copy("0.MOD级别");
		else if (m_ciLevel == 1)
			sText.Copy("1.Solid级别");
	}
	else if (GetPropID("RelaNode") == id)
		sText.Printf("0X%X", m_hRelaNode);
	else if (GetPropID("Fittings") == id)
		sText.Printf("0X%X", m_hRelaPart);
	else if (GetPropID("HalfDegAngle") == id)
	{
		sText.Printf("%f", m_fHalfConeDegAngle);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("StyleName") == id)
		sText.Copy(m_szStyleName);
	else if (GetPropID("HoleD") == id)
	{
		sText.Printf("%f", m_fHoleD);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("HOLE1.hBolt") == id)
		sText.Printf("0X%X", holes[0].hBolt);
	else if (GetPropID("HOLE1.hResidePart") == id)
		sText.Printf("0X%X", holes[0].hResidePart);
	else if (GetPropID("HOLE2.hBolt") == id)
		sText.Printf("0X%X", holes[1].hBolt);
	else if (GetPropID("HOLE2.hResidePart") == id)
		sText.Printf("0X%X", holes[1].hResidePart);
	else if (GetPropID("SpaceWidth") == id)
	{
		sText.Printf("%f", m_fSpaceWidth);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.orgion.x"))
	{
		sprintf(sText, "%f", acs.origin.x);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.orgion.y"))
	{
		sprintf(sText, "%f", acs.origin.y);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.orgion.z"))
	{
		sprintf(sText, "%f", acs.origin.z);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.axis_x.x"))
	{
		sprintf(sText, "%f", acs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.axis_x.y"))
	{
		sprintf(sText, "%f", acs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.axis_x.z"))
	{
		sprintf(sText, "%f", acs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.axis_y.x"))
	{
		sprintf(sText, "%f", acs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.axis_y.y"))
	{
		sprintf(sText, "%f", acs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.axis_y.z"))
	{
		sprintf(sText, "%f", acs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.axis_z.x"))
	{
		sprintf(sText, "%f", acs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.axis_z.y"))
	{
		sprintf(sText, "%f", acs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if (id == GetPropID("ucs.axis_z.z"))
	{
		sprintf(sText, "%f", acs.axis_z.z);
		SimplifiedNumString(sText);
	}
	//
	if (valueStr)
		StrCopy(valueStr, sText, nMaxStrBufLen);
	return strlen(sText);
}
//////////////////////////////////////////////////////////////////////
//CInsulatorBranch
CInsulatorBranch::CInsulatorBranch()
{
	m_uiInsulatorStyle=1;
	m_idConnPoint=0;
	m_uiBranchSerial=0;
	m_uiActiveGroupId=1;
}
CInsulatorBranch::~CInsulatorBranch()
{
}
CWirePointGroup* CInsulatorBranch::GetActiveWirePointGroup()
{
	CWirePointGroup* pWirePtGroup=NULL;
	for(pWirePtGroup=m_xWirePtGroupArr.GetFirst();pWirePtGroup;pWirePtGroup=m_xWirePtGroupArr.GetNext())
	{
		if(pWirePtGroup->m_uiGroupId==m_uiActiveGroupId)
			break;
	}
	return pWirePtGroup;
}
//////////////////////////////////////////////////////////////////////
//CWireConnPoint
CWireConnPoint::CWireConnPoint()
{
	m_uiSerial=0;
	m_uiCircuitId=0;
	m_uiPhaseSerial=0;
}
CWireConnPoint::~CWireConnPoint()
{

}
GEPOINT CWireConnPoint::CalAssemblyPos()
{
	GEPOINT pos;

	return pos;
}
//////////////////////////////////////////////////////////////////////////
//CWireCircuit
CWireCircuit::CWireCircuit()
{
	m_uiSerilaId=0;
	m_ciCurrent=0;
	m_ciForceType=0;
	m_wiVoltage=220;
}
//
UINT CWireCircuit::GetHangWireCount()
{
	UINT nWire=0;
	if(m_uiSerilaId==0)
		nWire=2;	//地线回路有两个地线
	else if(m_ciCurrent==0)
		nWire=2;	//直流导线回路有两相导线
	else //if(m_ciCurrent==1)
		nWire=3;	//交流导线回路有三相导线
	return nWire;
}
//
UINT CWireCircuit::GetConnPointCount()
{
	UINT nWire=GetHangWireCount();
	if(m_ciForceType==0)
		return nWire*1;		//悬垂受力，一跟线对应一个连接点
	else
		return nWire*2;		//耐张受力，一根线对应两个连接点(前+后)
}
//////////////////////////////////////////////////////////////////////
//CUniWireModel
CUniWireModel::CUniWireModel()
{
	m_bGimWireModelInherited=false;
	m_cnEarthWireCount=2;//默认2根地线
}
CXhChar50 CUniWireModel::GetWirePtGimName(WIREPLACE_CODE wireplace, GEPOINT pos)
{
	BYTE ciPosSymbol = 0;
	if (wireplace.ciWireType != 'J' && wireplace.ciTensionType >= 2)
	{	//wireplace.ciTensionType==3 表示终端塔，终端塔是一种特殊的耐张塔 wjh-2019.10.2
		if (wireplace.ciWireDirection == 'Y')
			ciPosSymbol = (pos.y < -EPS2) ? 'Q' : 'H';
		else //if(m_xWirePlace.ciWireDirection=='X')
			ciPosSymbol = (pos.x < -EPS2) ? 'Q' : 'H';
	}
	CXhChar50 sDesc;
	if (wireplace.ciWireType == 'E')
	{
		if (ciPosSymbol == 'Q')
			sDesc.Printf("前地%d", wireplace.iSerial);
		else if (ciPosSymbol == 'H')
			sDesc.Printf("后地%d", wireplace.iSerial);
		else
			sDesc.Printf("地%d", wireplace.iSerial);
	}
	else if (wireplace.ciWireType == 'C')
	{
		if (ciPosSymbol == 'Q')
			sDesc.Printf("前导%d", wireplace.iSerial);
		else if (ciPosSymbol == 'H')
			sDesc.Printf("后导%d", wireplace.iSerial);
		else
			sDesc.Printf("导%d", wireplace.iSerial);
		//根据正式GIM移交规范，挂点编号与后缀码之间用'_'分割而非'-'分割 wjh-2019.12.31
		if (wireplace.ciHangingStyle == 'S' && wireplace.ciPostCode > 0)
			sDesc.Append(CXhChar16("_S%d", wireplace.ciPostCode));
		else if (wireplace.ciHangingStyle == 'V'&& wireplace.ciPostCode > 0)
			sDesc.Append(CXhChar16("_V%d", wireplace.ciPostCode));
	}
	else if (wireplace.ciWireType == 'J')
	{
		sDesc.Printf("跳%d", wireplace.iSerial);
		if (wireplace.ciPostCode > 0)
			sDesc.Append(CXhChar16("_%d", wireplace.ciPostCode));
	}
	return sDesc;
}
bool CUniWireModel::UpdateWirePointGimName(CUniWirePoint* pWirePoint)
{
	pWirePoint->m_sGimName=GetWirePtGimName(pWirePoint->m_xWirePlace, pWirePoint->xPosition);
	return pWirePoint->m_sGimName.GetLength()>0;
}
//导出挂点的XML格式文件
#ifdef __LDS_CONTEXT_
bool CUniWireModel::ExportUniWireXmlData(const char* sFile)
{
	int nCircuit=m_listWireCircuits.GetNodeNum();
	if(nCircuit<1)
		return false;
	CString sText;
	CMarkup xml;
	//XML文件头
	xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	//根节点
	xml.AddElem("XML");
	xml.AddAttrib("Category","Tower");
	xml.AddAttrib("Type","");
	xml.AddAttrib("Circuit",nCircuit-1);
	//挂点节间
	xml.IntoElem();
	xml.AddElem("WirePointSet");	//添加节间
	for(CXhWirePoint* pWirePt=m_hashWirePoints.GetFirst();pWirePt;pWirePt=m_hashWirePoints.GetNext())
	{
		xml.IntoElem();
		xml.AddElem("WirePoint");
		xml.AddAttrib("Id",pWirePt->Id);
		xml.AddAttrib("HoleD",(char*)CXhChar16("%.f",pWirePt->m_fHoleD));
		sText=(pWirePt->m_ciLevel==0)?"mod":"solid";
		xml.AddAttrib("Code",sText);
		//挂点位置
		xml.IntoElem();
		xml.AddElem("Position");
		xml.AddAttrib("Px",(char*)CXhChar16("%.6f",pWirePt->xPosition.x));
		xml.AddAttrib("Py",(char*)CXhChar16("%.6f",pWirePt->xPosition.y));
		xml.AddAttrib("Pz",(char*)CXhChar16("%.6f",pWirePt->xPosition.z));
		xml.OutOfElem();
		//挂孔方向
		xml.IntoElem();
		xml.AddElem("HoleAxis");
		xml.AddAttrib("Px",(char*)CXhChar16("%.6f",pWirePt->m_vStdBasicAxisVec.x));
		xml.AddAttrib("Py",(char*)CXhChar16("%.6f",pWirePt->m_vStdBasicAxisVec.y));
		xml.AddAttrib("Pz",(char*)CXhChar16("%.6f",pWirePt->m_vStdBasicAxisVec.z));
		xml.OutOfElem();
		//受力方向约束
		xml.IntoElem();
		xml.AddElem("ConeInfo");
		xml.AddAttrib("Px",(char*)CXhChar16("%.6f",pWirePt->vWirePull.x));
		xml.AddAttrib("Py",(char*)CXhChar16("%.6f",pWirePt->vWirePull.y));
		xml.AddAttrib("Pz",(char*)CXhChar16("%.6f",pWirePt->vWirePull.z));
		xml.AddAttrib("HanlAngle",(char*)CXhChar16(".0f",pWirePt->m_fHalfConeDegAngle));
		xml.OutOfElem();
		//
		xml.OutOfElem();
	}
	xml.OutOfElem();
	//回路节间
	xml.IntoElem();
	xml.AddElem("CircuitSet");	//添加节间
	for(CWireCircuit* pCircuit=m_listWireCircuits.GetFirst();pCircuit;pCircuit=m_listWireCircuits.GetNext())
	{
		xml.IntoElem();
		xml.AddElem("Circuit");
		xml.AddAttrib("Id",pCircuit->m_uiSerilaId);
		xml.AddAttrib("Name",(char*)pCircuit->m_sName);
		if(pCircuit->m_uiSerilaId!=0)
		{	//导线回路
			xml.AddAttrib("Voltage",(char*)CXhChar16("%dkv",pCircuit->m_wiVoltage));
			sText=(pCircuit->m_ciForceType==0)?"悬垂":"耐张";
			xml.AddAttrib("Type",sText);
			sText=(pCircuit->m_ciCurrent==0)?"DC":"AC";
			xml.AddAttrib("Current",sText);
		}
		for(int i=0;i<pCircuit->m_xPhaseWireArr.GetSize();i++)
		{
			CPhaseWire* pPhase=pCircuit->m_xPhaseWireArr.GetByIndex(i);
			xml.IntoElem();
			xml.AddElem("PhaseWire");
			xml.AddAttrib("PhaseId",pPhase->m_ciPhaseSerial);
			xml.AddAttrib("PhaseName",(char*)CXhChar16("相线%d",pPhase->m_ciPhaseSerial));
			xml.OutOfElem();
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();
	//装配点节间
	xml.IntoElem();
	xml.AddElem("AssembyPointSet");
	for(CWireConnPoint* pAssemPt=m_hashConnPoints.GetFirst();pAssemPt;pAssemPt=m_hashConnPoints.GetNext())
	{
		xml.IntoElem();
		xml.AddElem("AssembyPoint");
		xml.AddAttrib("Id",pAssemPt->Id);
		xml.AddAttrib("HangType",(char*)pAssemPt->m_sDes);
		xml.AddAttrib("Circuit",pAssemPt->m_uiCircuitId);
		xml.AddAttrib("Phase",pAssemPt->m_uiPhaseSerial);
		//装配点位置
		xml.IntoElem();
		xml.AddElem("Positon");
		xml.AddAttrib("Px",(char*)CXhChar16("%.6f",pAssemPt->m_xPos.x));
		xml.AddAttrib("Py",(char*)CXhChar16("%.6f",pAssemPt->m_xPos.y));
		xml.AddAttrib("Pz",(char*)CXhChar16("%.6f",pAssemPt->m_xPos.z));
		xml.OutOfElem();
		CInsulatorBranch* pBranch=pAssemPt->m_xInsulatorBranchArr.GetFirst();
		if(pBranch && pBranch->m_xWirePtGroupArr.GetSize()>0)
		{
			//挂串分支
			xml.IntoElem();
			xml.AddElem("InsulatorBranch");
			xml.AddAttrib("BranchId","1");
			xml.AddAttrib("InsulatorType",(char*)pBranch->m_sInsulatorName);
			xml.AddAttrib("number",(char*)CXhChar16("%d",pBranch->m_uiActiveGroupId));
			//挂点引用
			CWirePointGroup* pWirePtGroup=pBranch->m_xWirePtGroupArr.GetByIndex(pBranch->m_uiActiveGroupId);
			for(UINT i=0;i<pWirePtGroup->m_xWirePointArr.Count;i++)
			{
				xml.IntoElem();
				xml.AddElem("WirePointRef");
				xml.AddAttrib("Id",(char*)CXhChar16("%d",pWirePtGroup->m_xWirePointArr[i]));
				xml.OutOfElem();
			}
			xml.OutOfElem();
		}
		//挂点组合
		xml.IntoElem();
		xml.AddElem("AssembyPoint");
		xml.OutOfElem();
		//
		xml.OutOfElem();
	}
	xml.OutOfElem();
	return xml.Save(sFile);
}
//导入挂点信息文件
bool CUniWireModel::ImPortUniWireXmlData(const char* sFile)
{
	CMarkup xml;
	if(!xml.Load(sFile))
		return FALSE;
	return TRUE;
}
#endif
void CUniWireModel::Empty()
{
	m_hashWirePoints.Empty();
	m_hashConnPoints.Empty();
	m_listWireCircuits.Empty();
	m_bGimWireModelInherited=false;
}
void CUniWireModel::FromBuffer(CBuffer& buffer)
{
	m_hashWirePoints.Empty();
	buffer.SeekToBegin();
	m_bGimWireModelInherited=false;
	int version=1;
	buffer.ReadInteger(&version);	//version
	buffer.ReadBooleanThin(&m_bGimWireModelInherited);
	int nWirePt=0;
	buffer.ReadInteger(&nWirePt);
	for(int i=0;i<nWirePt;i++)
	{
		CXhWirePoint* pWirePointObj=m_hashWirePoints.Add(0);
		buffer.ReadInteger(&pWirePointObj->m_hRelaPart);
		buffer.ReadDword(&pWirePointObj->m_xWirePlace.data.dwDataCode);
		buffer.ReadString(pWirePointObj->m_sGimName);
		buffer.ReadBoolean(&pWirePointObj->m_bRelyAngle);
		buffer.ReadInteger(&pWirePointObj->m_hRelaNode);
		buffer.ReadInteger(&pWirePointObj->holes[0].hBolt);
		buffer.ReadInteger(&pWirePointObj->holes[0].hResidePart);
		buffer.ReadInteger(&pWirePointObj->holes[1].hBolt);
		buffer.ReadInteger(&pWirePointObj->holes[1].hResidePart);
		buffer.ReadFloat(&pWirePointObj->m_fHoleD);
		buffer.ReadFloat(&pWirePointObj->m_fSpaceWidth);
		if (version >= 1)
			buffer.ReadPoint(pWirePointObj->m_vSketchArrow);
		buffer.ReadPoint(pWirePointObj->m_vStdBasicAxisVec);
		buffer.ReadPoint(pWirePointObj->m_xBasicPoint);
		buffer.ReadPoint(pWirePointObj->acs.origin);
		buffer.ReadPoint(pWirePointObj->acs.axis_x);
		buffer.ReadPoint(pWirePointObj->acs.axis_y);
		buffer.ReadPoint(pWirePointObj->acs.axis_z);
	}
}
void CUniWireModel::ToBuffer(CBuffer& buffer)
{
	long version = 1;
	buffer.WriteInteger(version);
	buffer.WriteBooleanThin(m_bGimWireModelInherited);
	int nWirePt=m_hashWirePoints.GetNodeNum();
	buffer.WriteInteger(nWirePt);
	for(CXhWirePoint* pWirePoint=m_hashWirePoints.GetFirst();pWirePoint;pWirePoint=m_hashWirePoints.GetNext())
	{
		buffer.WriteInteger(pWirePoint->m_hRelaPart);
		buffer.WriteDword(pWirePoint->m_xWirePlace.data.dwDataCode);
		buffer.WriteString(pWirePoint->m_sGimName);
		buffer.WriteBoolean(pWirePoint->m_bRelyAngle);
		buffer.WriteInteger(pWirePoint->m_hRelaNode);
		buffer.WriteInteger(pWirePoint->holes[0].hBolt);
		buffer.WriteInteger(pWirePoint->holes[0].hResidePart);
		buffer.WriteInteger(pWirePoint->holes[1].hBolt);
		buffer.WriteInteger(pWirePoint->holes[1].hResidePart);
		buffer.WriteFloat(pWirePoint->m_fHoleD);
		buffer.WriteFloat(pWirePoint->m_fSpaceWidth);
		if (version >= 1)
			buffer.WritePoint(pWirePoint->m_vSketchArrow);
		buffer.WritePoint(pWirePoint->m_vStdBasicAxisVec);
		buffer.WritePoint(pWirePoint->m_xBasicPoint);
		buffer.WritePoint(pWirePoint->acs.origin);
		buffer.WritePoint(pWirePoint->acs.axis_x);
		buffer.WritePoint(pWirePoint->acs.axis_y);
		buffer.WritePoint(pWirePoint->acs.axis_z);
	}
}

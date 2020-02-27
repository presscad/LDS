#include "stdafx.h"
#include "f_ent.h"
#include "XhCharString.h"

#ifdef __RAPID_ROUGH_3DMODEL_
#include "IntelliModel.h"
////////////////////////////////////////////////////////////////////////////////
//结构连接
STDTEMPL::DYNA_LINKROD::DYNA_LINKROD()
{
	widRod=widStartNode=widEndNode=0;	//起始节点，终止节点
	ciNormalDirection=0;			//近似工作法线（工作肢法线最接近的坐标轴）X、Y、Z，（或小写表示负轴方向）
	ciAngleLayout=0;				//角钢的布置方式1,2,3,4;<=0表示不指定肢朝向
	widRefRodOfWorkWing=0;		//工作肢参照杆件
}
STDTEMPL::CONNECT_ROD::CONNECT_ROD()
{
	widRod=0;
	dynaRod.widStartNode=dynaRod.widEndNode=0;
	dynaRod.siRayMinDegAngle=dynaRod.siRayMaxDegAngle=0;
	ciConnectWing=0;
	ciPlaneNo=1;
	dualSideEdge=false;
	cnMinBoltN=2;
	cnMaxSingularLineBolts=4;
	widCrossBoltAngle=0;
}
STDTEMPL::PLATE::PLATE()
{
	widBaseRod=0;			//钢板的定位基准角钢
	widBaseNode=0;			//钢板定位节点
	ciDatumAngleWing='X';	//钢板的定位基准角钢的工作肢，'X','Y'。
	widBendEdgeAngle1=0;	//第一个制弯面的制弯线定位基准角钢，向钢板法线内侧曲为正，外侧曲为负，度
	ciBendEdgeWing1='X';	//'X','x','Y','y':大写表示在楞线侧，小写在肢翼侧
	fBendDegAngle1=0;		//制弯角，沿制弯线基准角钢始->终为轴逆时针旋转为正(角度制，0表示自动计算)
	widBendEdgeAngle2=0;	//第一个制弯面的制弯线定位基准角钢，向钢板法线内侧曲为正，外侧曲为负，度
	ciBendEdgeWing2='X';	//'X','x','Y','y':大写表示在楞线侧，小写在肢翼侧
	fBendDegAngle2=0;		//制弯角，沿制弯线基准角钢始->终为轴逆时针旋转为正(角度制，0表示自动计算)

	ciCurveEdgeMode=0;	//0.外凸式;1.波浪式
	ciEdgeRadius=40;	//挂孔边距的圆弧半径
	ciFilletRadius=10;//挂孔边内凹角处倒角内圆弧半径
}
void STDTEMPL::PLATE::CloneFrom(PLATE* pFrom)
{
	widBaseRod=pFrom->widBaseRod;		//钢板的定位基准角钢
	widBaseNode=pFrom->widBaseNode;		//钢板定位节点
	ciDatumAngleWing=pFrom->ciDatumAngleWing;	//钢板的定位基准角钢的工作肢，'X','Y'。
	widBendEdgeAngle1=pFrom->widBendEdgeAngle1;	//第一个制弯面的制弯线定位基准角钢，向钢板法线内侧曲为正，外侧曲为负，度
	ciBendEdgeWing1=pFrom->ciBendEdgeWing1;	//'X','x','Y','y':大写表示在楞线侧，小写在肢翼侧
	fBendDegAngle1=pFrom->fBendDegAngle1;	//制弯角，沿制弯线基准角钢始->终为轴逆时针旋转为正(角度制，0表示自动计算)
	widBendEdgeAngle2=pFrom->widBendEdgeAngle2;	//第一个制弯面的制弯线定位基准角钢，向钢板法线内侧曲为正，外侧曲为负，度
	ciBendEdgeWing2=pFrom->ciBendEdgeWing2;	//'X','x','Y','y':大写表示在楞线侧，小写在肢翼侧
	fBendDegAngle2=pFrom->fBendDegAngle2;	//制弯角，沿制弯线基准角钢始->终为轴逆时针旋转为正(角度制，0表示自动计算)
	//拷贝钢板连接杆件
	connrods.Clear();
	for(UINT i=0;i<pFrom->connrods.Count;i++)
		connrods.Set(i,pFrom->connrods.At(i),true);
	//挂线板设计信息
	holes.Clear();
	for(UINT i=0;i<pFrom->holes.Count;i++)
		holes.Set(i,pFrom->holes.At(i),true);
	ciCurveEdgeMode=pFrom->ciCurveEdgeMode;	//0.外凸式;1.波浪式
	ciEdgeRadius=pFrom->ciEdgeRadius;	//挂孔边距的圆弧半径
	ciFilletRadius=pFrom->ciFilletRadius;//挂孔边内凹角处倒角内圆弧半径
}
STDTEMPL::WIREANGLE::WIREANGLE()
{
	widBaseRod=0;			//钢板的定位基准角钢
	widBaseNode=0;			//钢板定位节点
	ciLayer=0;				//贴合位置层级，0.表示与定位角钢紧贴，越大表示越外层
	ciDatumAngleWing='X';	//钢板的定位基准角钢的工作肢，'X','Y'。
	ciNormalOffset=0;		//0.表示自动判断
	ciAngleLayout=0;		//角钢的布置方式1,2,3,4;<=0表示不指定肢朝向，相对于贴合肢法线看
	ciLenDirection='X';		//短角钢始->终近似方向（最接近的坐标轴）X、Y、Z，（或小写表示负轴方向）
	width=width2=140;
	thick=10;				//贴合肢宽，另一肢宽，肢厚
}
void STDTEMPL::WIREANGLE::CloneFrom(WIREANGLE* pFrom)
{
	widAngle=pFrom->widAngle;
	widBaseRod=pFrom->widBaseRod;		//钢板的定位基准角钢
	widBaseNode=pFrom->widBaseNode;		//钢板定位节点
	ciLayer=pFrom->ciLayer;	//贴合位置层级，0.表示与定位角钢紧贴，越大表示越外层
	ciDatumAngleWing=pFrom->ciDatumAngleWing;	//钢板的定位基准角钢的工作肢，'X','Y'。
	ciNormalOffset=pFrom->ciNormalOffset;	//0.表示自动判断
	ciAngleLayout=pFrom->ciAngleLayout;	//角钢的布置方式1,2,3,4;<=0表示不指定肢朝向，相对于贴合肢法线看
	ciLenDirection=pFrom->ciLenDirection;//短角钢始->终近似方向（最接近的坐标轴）X、Y、Z，（或小写表示负轴方向）
	width=pFrom->width;
	width2=pFrom->width2;
	thick=pFrom->thick;	//贴合肢宽，另一肢宽，肢厚
	//拷贝挂线孔
	holes.Clear();
	for(UINT i=0;i<pFrom->holes.Count;i++)
		holes.Set(i,pFrom->holes.At(i),true);
}
STDTEMPL::ANGLEPLATE::ANGLEPLATE()
{
	ciRibType=0;		//角钢加强肋板类型
	ciLocationMode=0;	//0.短角钢中点始->终方向偏移;'S':从始端面开始向终端偏移;'E'从终端面开始向始端偏移
	siLenOffset=0;
	thick=8;
}
////////////////////////////////////////////////////////////////////////////////
STDTEMPL::PROFILE_ROD::PROFILE_ROD()
{
	widRod=widStartNode=widEndNode=0;
	bHasSectView=false;	//是否具有展开面
	ciRodType=ciSectFlag=0;
	ciAngleLayout=0;	//角钢的布置方式1,2,3,4;<=0表示不指定肢朝向
	widRefRodOfWorkWing=0;
	siMinRayDegAngle=siMaxRayDegAngle=0;
}
STDTEMPL::PROFILE_NODE::PROFILE_NODE()
{
	ciPosCalType=0;					//0.孤立点;1.指定X ;2.指定Y;3.指定Z;4.基准面指定XZ
	widNode=0;
	widLinkPrevRod=0;				//连接的前一杆件Id;0.表示前面无杆件连接
	widMasterRod=0;					//节点父杆件
	siMinDegAngle=siMaxDegAngle=0;	//该顶点处前后轮廓边间夹角范围
	ciProfileNode=true;
	ciFractions=1;
}
STDTEMPL::STDTEMPL()
{
	m_wnProfileNodeCount=0;
	memset(name,0,51);
	ciOnRightSide='R';
	ciClassicType=0;
	//DYN_LINKROD
	//CONNECT_ROD
	//PLATE
	//WIREANGLE
	//ANGLEPLATE
	/*PROFILE_NODE* pParaNode;
	PROFILE_ROD* pRod;
	pParaNode=hashParaNodes.Add(1);
	pParaNode=hashParaNodes.Add(2);
	pParaNode->widLinkPrevRod=2;
	pParaNode->widMasterRod=2;
	pParaNode->siMinDegAngle= 80;
	pParaNode->siMaxDegAngle=100;
	pParaNode=hashParaNodes.Add(3);
	pParaNode->widLinkPrevRod=3;
	pParaNode->widMasterRod=1;
	pParaNode->siMinDegAngle= 90;
	pParaNode->siMaxDegAngle=120;
	pParaNode=hashParaNodes.Add(4);
	pParaNode->widLinkPrevRod=1;
	//pParaNode->widMasterRod=0;
	//pParaNode->siMinDegAngle= 90;
	//pParaNode->siMaxDegAngle=120;
	pRod=hashParaRods.Add(1);	//顶部横梁
	pRod->widStartNode=4;
	pRod->widEndNode  =3;
	pRod=hashParaRods.Add(2);	//底部横梁
	pRod->widStartNode=1;
	pRod->widEndNode  =2;
	pRod=hashParaRods.Add(3);	//外端部立杆
	pRod->widStartNode=3;
	pRod->widEndNode  =2;
	pRod->ciAngleLayout=3;	//指定角钢肢朝向
	pRod->bHasSectView=true;
	//CHashListEx<DATUM_PLANE> hashParaPlanes;
	//定义主材四边形组
	PROFILE_QUADSECT* pQuadSect=listQuadSects.AttachObject();
	pQuadSect->arrRods[0]=1;
	pQuadSect=listQuadSects.AttachObject();
	pQuadSect->arrRods[1]=2;*/
}
WORD STDTEMPL::get_ProfileNodeCount()
{
	if(m_wnProfileNodeCount<=0)
	{
		m_wnProfileNodeCount=0;
		for(PROFILE_NODE* pTmplNode=hashParaNodes.GetFirst();pTmplNode;pTmplNode=hashParaNodes.GetNext())
		{
			if(pTmplNode->ciProfileNode>0)
				m_wnProfileNodeCount++;
		}
	}
	return m_wnProfileNodeCount;
}
void STDTEMPL::Empty(){
	hashParaRods.Empty();
	hashParaNodes.Empty();
	hashParaPlanes.Empty();
	listQuadSects.DeleteList();
}
void STDTEMPL::CopyStdTemplFrom(STDTEMPL* pStdTempl)
{
	StrCopy(name,pStdTempl->name,50);
	ciClassicType=pStdTempl->ciClassicType;
	ciOnRightSide=pStdTempl->ciOnRightSide;
	Empty();
	for(PROFILE_ROD* pParaRod=pStdTempl->hashParaRods.GetFirst();pParaRod;pParaRod=pStdTempl->hashParaRods.GetNext())
		hashParaRods.SetValue(pParaRod->widRod,*pParaRod);
	for(PROFILE_NODE* pParaNode=pStdTempl->hashParaNodes.GetFirst();pParaNode;pParaNode=pStdTempl->hashParaNodes.GetNext())
		hashParaNodes.SetValue(pParaNode->widNode,*pParaNode);
	for(DATUM_PLANE* pDatumPlane=pStdTempl->hashParaPlanes.GetFirst();pDatumPlane;pDatumPlane=pStdTempl->hashParaPlanes.GetNext())
		hashParaPlanes.SetValue(pDatumPlane->widPlane,*pDatumPlane);
	for(PROFILE_QUADSECT* pQuadSect=pStdTempl->listQuadSects.EnumObjectFirst();pQuadSect;pQuadSect=pStdTempl->listQuadSects.EnumObjectNext())
		listQuadSects.Append(*pQuadSect);
	for(DYNA_LINKROD* pDynaRod=pStdTempl->hashDynaLinkRods.GetFirst();pDynaRod;pDynaRod=pStdTempl->hashDynaLinkRods.GetNext())
		hashDynaLinkRods.Append(*pDynaRod);
	for(WIREANGLE* pWireAngle=pStdTempl->hashWireAngles.GetFirst();pWireAngle;pWireAngle=pStdTempl->hashWireAngles.GetNext())
	{
		WIREANGLE* pNewWireAngle=hashWireAngles.Append(pWireAngle->widAngle);
		pNewWireAngle->CloneFrom(pWireAngle);
	}
	for(PLATE* pConnPlate=pStdTempl->listParaPlates.EnumObjectFirst();pConnPlate;pConnPlate=pStdTempl->listParaPlates.EnumObjectNext())
	{
		PLATE* pNewPlate=listParaPlates.Append();
		pNewPlate->CloneFrom(pConnPlate);
	}
	for(ANGLEPLATE* pAnglePlate=pStdTempl->listAnglePlates.EnumObjectFirst();pAnglePlate;pAnglePlate=pStdTempl->listAnglePlates.EnumObjectNext())
		listAnglePlates.Append(*pAnglePlate);
}

#endif
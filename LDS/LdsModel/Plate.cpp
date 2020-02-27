// Plate.cpp: implementation of the C3dPlank class.
//<LOCALE_TRANSLATE BY wbt />
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tower.h"
#include "btc.h"
#include "SortFunc.h"
#include "ConnBoltsStrength.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
WELDLINE::WELDLINE()
{
	cStateFlag=cCalModeFlag=0;//默认智能判断坡口，自动根据母构件自行计算；正反面均满焊
	hWeldMotherPart=0;	//焊接母构件（焊缝所在的母构件）
	height=length=0;	//焊缝高度,焊缝长度,mm
	lenpos=0;		//焊缝起点位置(自边始端点的偏移量), mm
}
//用于设定含有排它性选项的标识字,排它选项掩码标识位在低位，排它选项标识位自高位排列
char WELDLINE::SetStateFlagDword(char cFlag,bool bSet/*=true*/)
{
	if(bSet)
	{	//设定
		cStateFlag|=cFlag;
	}
	else
	{	//移除
		char flag=0XFF^cFlag;	//通过异或操作得出dwFlag的补码
		cStateFlag&=flag;	//通过位与操作清除dwFlag中的标识位
	}
	return cStateFlag;
}
char WELDLINE::SetCalModeFlagDword(char cFlag,bool bSet/*=true*/)
{
	if(bSet)
	{	//设定
		cCalModeFlag|=cFlag;
	}
	else
	{	//移除
		char flag=0XFF^cFlag;	//通过异或操作得出dwFlag的补码
		cCalModeFlag&=flag;	//通过位与操作清除dwFlag中的标识位
	}
	return cCalModeFlag;
}
//是否具有某种标识设定
bool WELDLINE::IsHasStateFlag(char cFlag)
{
	if(cStateFlag&cFlag)
		return true;
	else
		return false;
}
bool WELDLINE::IsHasCalModeFlag(char cFlag)
{
	if(cCalModeFlag&cFlag)
		return true;
	else
		return false;
}
void WELDLINE::FromBuffer(CBuffer &buffer)
{
	buffer.ReadByte(&cCalModeFlag);
	buffer.ReadByte(&cStateFlag);
	buffer.ReadWord(&height);
	buffer.ReadWord(&length);
	buffer.ReadWord(&lenpos);
	buffer.ReadInteger(&hWeldMotherPart);
}
void WELDLINE::ToBuffer(CBuffer &buffer)
{
	buffer.WriteByte(cCalModeFlag);
	buffer.WriteByte(cStateFlag);
	buffer.WriteWord(height);
	buffer.WriteWord(length);
	buffer.WriteWord(lenpos);
	buffer.WriteInteger(hWeldMotherPart);
}

//顶点
PROFILE_VERTEX::PROFILE_VERTEX()
{
	manu_space=type=0;
	sector_angle=radius=0;
	m_bWeldEdge=m_bRollEdge=false;
	local_point_y=local_point_vec=0;
	m_bCanCut = true;
	rayVecX = 0;
	rayVecY = 0;
	roll_edge_offset_dist=0;
	m_cPosDesignType=DT_NONE; 
	m_hRod=0;	
	m_pBelongPlate=NULL;
	m_pTransCS=NULL;
	xTag.uiTagValue=0;
}
PROFILE_VERTEX::PROFILE_VERTEX(f3dPoint vertice,BYTE arctype/*=0*/)
{
	Initialize(vertice.x,vertice.y,vertice.z,arctype);
	vertex.feature=vertice.feature<1?1:vertice.feature;
}
PROFILE_VERTEX::PROFILE_VERTEX(COORD3D vertice,BYTE arctype/*=0*/)
{
	Initialize(vertice.x,vertice.y,vertice.z,arctype);
	vertex.feature=1;	//默认面号为1
}
PROFILE_VERTEX::PROFILE_VERTEX(double x,double y,double z,BYTE arctype/*=0*/)
{
	Initialize(x,y,z,arctype);
	vertex.feature=1;	//默认面号为1
}
void PROFILE_VERTEX::Initialize(double x,double y,double z,BYTE arctype/*=0*/)
{
	vertex=f3dPoint(x,y,z);
	manu_space=0;
	sector_angle=radius=0;
	m_bWeldEdge=m_bRollEdge=false;
	local_point_y=local_point_vec=0;
	type=arctype;
	m_bCanCut = true;
	rayVecX = 0;
	rayVecY = 0;
	roll_edge_offset_dist=0;
	m_cPosDesignType=DT_NONE; 
	m_hRod=0;	
	m_pBelongPlate=NULL;
	m_pTransCS=NULL;
	xTag.uiTagValue=0;
}
BYTE PROFILE_VERTEX::set_ciPlaneNo(BYTE _ciPlaneNo)
{
	if (_ciPlaneNo==0)
		_ciPlaneNo=1;
	return vertex.feature=_ciPlaneNo;
}
//对接点相对于初始零点沿当前焊接边的偏移量（以钢管始->终方向为偏移量正值方向） wjh-2019.11.02
long PROFILE_VERTEX::get_liLocationAlongOffset() const
{
	return ftoi(this->local_point_y);
}
long PROFILE_VERTEX::set_liLocationAlongOffset(long liAlongOffset)
{
	local_point_y=liAlongOffset;
	return liAlongOffset;
}
//连接板与基准钢管间的对接走向标识模式,等同local_point_vec，
//表示焊缝延基准钢管始->终的对接走向模式,0.表示无对活点;1.对接走向标识为始->终;2.对接走向标识为终->始
char PROFILE_VERTEX::get_ciWeldFlowDirection() const
{
	return (char)this->local_point_vec;
}
char PROFILE_VERTEX::set_ciWeldFlowDirection(char ciFlowDirection)
{
	local_point_vec=ciFlowDirection;
	return ciFlowDirection;
}
IMPLEMENT_GET(PROFILE_VERTEX,fLengCutWidth)
{
	if(m_pBelongPlate==NULL||m_pBelongPlate->wiType!=CLDSPlate::WASHER_PLATE)
		return xOffset.x;
	BOOL pushed=m_pBelongPlate->vertex_list.push_stack();
	PROFILE_VERTEX *pPrevVertex=m_pBelongPlate->vertex_list.GetTail(),*pVertex=NULL,*pNextVertex=NULL;
	for(pVertex=m_pBelongPlate->vertex_list.GetFirst();pVertex;pVertex=m_pBelongPlate->vertex_list.GetNext())
	{
		if(pVertex==this)
			break;
		pPrevVertex=pVertex;
	}
	pNextVertex=m_pBelongPlate->vertex_list.GetNext();
	if(pVertex!=NULL&&pNextVertex==NULL)
		pNextVertex=m_pBelongPlate->vertex_list.GetFirst();
	if(pushed)
		m_pBelongPlate->vertex_list.pop_stack(pushed);
	if(pVertex==NULL)
		return xOffset.x;
	GEPOINT vWingCutVec=pPrevVertex->vertex-pVertex->vertex;
	if(m_pBelongPlate->cDatumAxis=='Y')
	{	//Y轴为长度方向
		if(pPrevVertex->m_cPosDesignType!=pVertex->m_cPosDesignType&&fabs(vWingCutVec.x)<EPS)
		{	//通过前一点参照切角方向
			if(vWingCutVec.y>0)
				return xOffset.y;
			else if(vWingCutVec.y<0)
				return -xOffset.y;
		}
		else if(pVertex->m_cPosDesignType!=pNextVertex->m_cPosDesignType)
		{
			vWingCutVec=pNextVertex->vertex-pVertex->vertex;
			if(vWingCutVec.y>0)
				return xOffset.y;
			else if(vWingCutVec.y<0)
				return -xOffset.y;
		}
	}
	else //if(m_pBelongPlate->cDatumAxis=='X')
	{	//X轴为长度方向
		if(pPrevVertex->m_cPosDesignType!=pVertex->m_cPosDesignType&&fabs(vWingCutVec.y)<EPS)
		{	//通过前一点参照切角方向
			if(vWingCutVec.x>0)
				return xOffset.x;
			else if(vWingCutVec.x<0)
				return -xOffset.x;
		}
		else if(pVertex->m_cPosDesignType!=pNextVertex->m_cPosDesignType)
		{
			vWingCutVec=pNextVertex->vertex-pVertex->vertex;
			if(vWingCutVec.x>0)
				return xOffset.x;
			else if(vWingCutVec.x<0)
				return -xOffset.x;
		}
	}
	return 0;
}
IMPLEMENT_SET(PROFILE_VERTEX,fLengCutWidth)
{
	if(m_pBelongPlate==NULL||m_pBelongPlate->wiType!=CLDSPlate::WASHER_PLATE)
	{
		xOffset.x=(float)value;
		return;
	}
	BOOL pushed=m_pBelongPlate->vertex_list.push_stack();
	PROFILE_VERTEX *pPrevVertex=m_pBelongPlate->vertex_list.GetTail(),*pVertex=NULL,*pNextVertex=NULL;
	for(pVertex=m_pBelongPlate->vertex_list.GetFirst();pVertex;pVertex=m_pBelongPlate->vertex_list.GetNext())
	{
		if(pVertex==this)
			break;
		pPrevVertex=pVertex;
	}
	pNextVertex=m_pBelongPlate->vertex_list.GetNext();
	if(pVertex!=NULL&&pNextVertex==NULL)
		pNextVertex=m_pBelongPlate->vertex_list.GetFirst();
	if(pushed)
		m_pBelongPlate->vertex_list.pop_stack(pushed);
	if(pVertex==NULL)
	{
		xOffset.x=(float)value;
		return;
	}
	GEPOINT vWingCutVec=pPrevVertex->vertex-pVertex->vertex;
	if(m_pBelongPlate->cDatumAxis=='Y')
	{	//Y轴为长度方向
		if(pPrevVertex->m_cPosDesignType!=pVertex->m_cPosDesignType&&fabs(vWingCutVec.x)<EPS)
		{	//通过前一点参照切角方向
			if(vWingCutVec.y>0)
				xOffset.y= (float)value;
			else if(vWingCutVec.y<0)
				xOffset.y=-(float)value;
		}
		else if(pVertex->m_cPosDesignType!=pNextVertex->m_cPosDesignType)
		{
			vWingCutVec=pNextVertex->vertex-pVertex->vertex;
			if(vWingCutVec.y>0)
				xOffset.y= (float)value;
			else if(vWingCutVec.y<0)
				xOffset.y=-(float)value;
		}
	}
	else //if(m_pBelongPlate->cDatumAxis=='X')
	{	//X轴为长度方向
		if(pPrevVertex->m_cPosDesignType!=pVertex->m_cPosDesignType&&fabs(vWingCutVec.y)<EPS)
		{	//通过前一点参照切角方向
			if(vWingCutVec.x>0)
				xOffset.x= (float)value;
			else if(vWingCutVec.x<0)
				xOffset.x=-(float)value;
		}
		else if(pVertex->m_cPosDesignType!=pNextVertex->m_cPosDesignType)
		{
			vWingCutVec=pNextVertex->vertex-pVertex->vertex;
			if(vWingCutVec.x>0)
				xOffset.x= (float)value;
			else if(vWingCutVec.x<0)
				xOffset.x=-(float)value;
		}
	}
}
IMPLEMENT_GET(PROFILE_VERTEX,fWingCutWidth)
{
	if(m_pBelongPlate==NULL||m_pBelongPlate->wiType!=CLDSPlate::WASHER_PLATE)
		return xOffset.y;
	BOOL pushed=m_pBelongPlate->vertex_list.push_stack();
	PROFILE_VERTEX *pPrevVertex=m_pBelongPlate->vertex_list.GetTail(),*pVertex=NULL,*pNextVertex=NULL;
	for(pVertex=m_pBelongPlate->vertex_list.GetFirst();pVertex;pVertex=m_pBelongPlate->vertex_list.GetNext())
	{
		if(pVertex==this)
			break;
		pPrevVertex=pVertex;
	}
	pNextVertex=m_pBelongPlate->vertex_list.GetNext();
	if(pVertex!=NULL&&pNextVertex==NULL)
		pNextVertex=m_pBelongPlate->vertex_list.GetFirst();
	if(pushed)
		m_pBelongPlate->vertex_list.pop_stack(pushed);
	if(pVertex==NULL)
		return xOffset.y;
	GEPOINT vWingCutVec=pPrevVertex->vertex-pVertex->vertex;
	if(m_pBelongPlate->cDatumAxis=='Y')
	{	//Y轴为长度方向
		if(pPrevVertex->m_cPosDesignType!=pVertex->m_cPosDesignType&&fabs(vWingCutVec.y)<EPS)
		{	//通过前一点参照切角方向
			if(vWingCutVec.x>0)
				return xOffset.x;
			else if(vWingCutVec.x<0)
				return -xOffset.x;
		}
		else if(pVertex->m_cPosDesignType!=pNextVertex->m_cPosDesignType)
		{
			vWingCutVec=pNextVertex->vertex-pVertex->vertex;
			if(vWingCutVec.x>0)
				return xOffset.x;
			else if(vWingCutVec.x<0)
				return -xOffset.x;
		}
	}
	else //if(m_pBelongPlate->cDatumAxis=='X')
	{	//X轴为长度方向
		if(pPrevVertex->m_cPosDesignType!=pVertex->m_cPosDesignType&&fabs(vWingCutVec.x)<EPS)
		{	//通过前一点参照切角方向
			if(vWingCutVec.y>0)
				return xOffset.y;
			else if(vWingCutVec.y<0)
				return -xOffset.y;
		}
		else if(pVertex->m_cPosDesignType!=pNextVertex->m_cPosDesignType)
		{
			vWingCutVec=pNextVertex->vertex-pVertex->vertex;
			if(vWingCutVec.y>0)
				return xOffset.y;
			else if(vWingCutVec.y<0)
				return -xOffset.y;
		}
	}
	return 0;
}
IMPLEMENT_SET(PROFILE_VERTEX,fWingCutWidth)
{
	if(m_pBelongPlate==NULL||m_pBelongPlate->wiType!=CLDSPlate::WASHER_PLATE)
	{
		xOffset.y=(float)value;
		return;
	}
	BOOL pushed=m_pBelongPlate->vertex_list.push_stack();
	PROFILE_VERTEX *pPrevVertex=m_pBelongPlate->vertex_list.GetTail(),*pVertex=NULL,*pNextVertex=NULL;
	for(pVertex=m_pBelongPlate->vertex_list.GetFirst();pVertex;pVertex=m_pBelongPlate->vertex_list.GetNext())
	{
		if(pVertex==this)
			break;
		pPrevVertex=pVertex;
	}
	pNextVertex=m_pBelongPlate->vertex_list.GetNext();
	if(pVertex!=NULL&&pNextVertex==NULL)
		pNextVertex=m_pBelongPlate->vertex_list.GetFirst();
	if(pushed)
		m_pBelongPlate->vertex_list.pop_stack(pushed);
	if(pVertex==NULL)
	{
		xOffset.y=(float)value;
		return;
	}
	GEPOINT vWingCutVec=pPrevVertex->vertex-pVertex->vertex;
	if(m_pBelongPlate->cDatumAxis=='Y')
	{	//Y轴为长度方向
		if(pPrevVertex->m_cPosDesignType!=pVertex->m_cPosDesignType&&fabs(vWingCutVec.y)<EPS)
		{	//通过前一点参照切角方向
			if(vWingCutVec.x>0)
				xOffset.x= (float)value;
			else if(vWingCutVec.x<0)
				xOffset.x=-(float)value;
		}
		else if(pVertex->m_cPosDesignType!=pNextVertex->m_cPosDesignType)
		{
			vWingCutVec=pNextVertex->vertex-pVertex->vertex;
			if(vWingCutVec.x>0)
				xOffset.x= (float)value;
			else if(vWingCutVec.x<0)
				xOffset.x=-(float)value;
		}
	}
	else //if(m_pBelongPlate->cDatumAxis=='X')
	{	//X轴为长度方向
		if(pPrevVertex->m_cPosDesignType!=pVertex->m_cPosDesignType&&fabs(vWingCutVec.x)<EPS)
		{	//通过前一点参照切角方向
			if(vWingCutVec.y>0)
				xOffset.y= (float)value;
			else if(vWingCutVec.y<0)
				xOffset.y=-(float)value;
		}
		else if(pVertex->m_cPosDesignType!=pNextVertex->m_cPosDesignType)
		{
			vWingCutVec=pNextVertex->vertex-pVertex->vertex;
			if(vWingCutVec.y>0)
				xOffset.y= (float)value;
			else if(vWingCutVec.y<0)
				xOffset.y=-(float)value;
		}
	}
}
void PROFILE_VERTEX::CopyEdgeInfoTo(PROFILE_EDGEINFO* pEdgeInfo)
{
	pEdgeInfo->m_bWeldEdge=m_bWeldEdge;
	pEdgeInfo->m_bRollEdge=m_bRollEdge;
	pEdgeInfo->weldline=weldline;
	pEdgeInfo->manu_space=manu_space;
	pEdgeInfo->roll_edge_offset_dist=roll_edge_offset_dist;
	pEdgeInfo->radius=radius;
	pEdgeInfo->sector_angle=sector_angle;
	pEdgeInfo->center=center;
	pEdgeInfo->column_norm=column_norm;
	pEdgeInfo->local_point_y=local_point_y;
	pEdgeInfo->local_point_vec=local_point_vec;
}
void PROFILE_VERTEX::CopyEdgeInfoFrom(const PROFILE_EDGEINFO& edgeinfo)
{
	m_bWeldEdge=edgeinfo.m_bWeldEdge;
	m_bRollEdge=edgeinfo.m_bRollEdge;
	weldline=edgeinfo.weldline;
	manu_space=edgeinfo.manu_space;
	roll_edge_offset_dist=edgeinfo.roll_edge_offset_dist;
	radius=edgeinfo.radius;
	sector_angle=edgeinfo.sector_angle;
	center=edgeinfo.center;
	column_norm=edgeinfo.column_norm;
	local_point_y=edgeinfo.local_point_y;
	local_point_vec=edgeinfo.local_point_vec;
}
void PROFILE_VERTEX::RetrieveArcLine(f3dArcLine &arcline,f3dPoint endPoint,UCS_STRU *ucs)
{
	if(vertex.IsEqual(endPoint))//防止因两点重合生成圆弧抛出异常
		arcline=f3dArcLine(f3dLine(vertex,endPoint));
	else if(type==0)	//普通直边
		arcline.CreateMethod2(vertex,endPoint,f3dPoint(0,0,1),0);
	else if(type==1)	//指定圆弧夹角
	{
		if(sector_angle>0)
			arcline.CreateMethod2(vertex,endPoint,f3dPoint(0,0,1),sector_angle);
		else
			arcline.CreateMethod2(vertex,endPoint,f3dPoint(0,0,-1),-sector_angle);
	}
	else if(type==2)	//指定圆弧半径
	{
		int sign=1;
		if(radius<0)
			sign=-1;
		arcline.CreateMethod3(vertex,endPoint,f3dPoint(0,0,sign),sign*radius,center);
		//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
		//arcline.CreateMethod2(vertex,endPoint,f3dPoint(0,0,sign),sector_angle);
	}
	else if(type==3)	//椭圆弧
	{
		int sign=1;
		if(radius<0)
			sign=-1;
		f3dPoint columnNorm=column_norm;
		f3dPoint centerTemp=center;
		if (ucs != NULL)
		{
			coord_trans(centerTemp, *ucs, FALSE);
			vector_trans(columnNorm, *ucs, FALSE);
		}
		arcline.CreateEllipse(centerTemp,vertex,endPoint,columnNorm,f3dPoint(0,0,sign),sign*radius);
	}
}
//计算钢板组焊时的定位标识点位置（在焊缝线上）所以位置及方向矢量均应转换到同一相对坐标系下方可进行
GEPOINT PROFILE_VERTEX::RetrieveWeldLocation(double* endPoint,double* tube_line_vec,double* datum_org/*=NULL*/)
{
	f3dPoint perp,datumorg(datum_org);	//datum_org为locate_point_y的基准位置
	SnapPerp(&perp,vertex,f3dPoint(endPoint),datumorg);
	if(liLocationAlongOffset!=0)	//this->local_point_y
	{	//有纵向偏移
		GEPOINT axis_y(endPoint[0]-vertex.x,endPoint[1]-vertex.y);
		normalize(axis_y);
		//判定当前焊接边走向与基准钢管始->终走向是否一致（或反向） wjh-2019.11.02
		int sign=(axis_y.x*tube_line_vec[0]+axis_y.y*tube_line_vec[1]<0)?-1:1;	//axis_y*tube_line_vec<0
		perp+=axis_y*(sign*liLocationAlongOffset);
	}
	return perp;
}
//
void PROFILE_VERTEX::AmendWeldLocationY(double* endPoint,double* tube_line_vec,double* datum_org/*=NULL*/)
{
	if(this->ciWeldFlowDirection==0)//local_point_vec==0)
		return;	//无对活点
	f3dPoint next_vertex(endPoint);
	//计算焊接边正侧走向vRegWeldVec（与基准钢管始->终一致） wjh-2019.11.02
	GEPOINT vRegWeldVec(next_vertex.x-vertex.x,next_vertex.y-vertex.y);
	normalize(vRegWeldVec);
	if(vRegWeldVec.x*tube_line_vec[0]+vRegWeldVec.y*tube_line_vec[1]<0)
		vRegWeldVec*=-1.0;	//正则化
	f3dLine weld_line(vertex,next_vertex);
	f3dPoint perp,loca_pt,loca_pt_l,loca_pt_r;
	SnapPerp(&perp,weld_line,f3dPoint(datum_org));
	//临时代码,修复安徽宏源变电构架上的钢板对活点
	if(local_point_y>eps||local_point_y<-eps)
	{	//TODO:????? wjh-2019.11.02
		f3dPoint mid_pt=0.5*(weld_line.startPt+weld_line.endPt);
		f3dPoint loca_pt=perp+vRegWeldVec*local_point_y;
		if(DISTANCE(mid_pt,loca_pt)<eps2)
			local_point_y=0;	//对活点在焊接边中点
	}
	loca_pt=perp+vRegWeldVec*local_point_y;
	//loca_pt_l=loca_pt-weld_vec*100;
	//loca_pt_r=loca_pt+weld_vec*100;
	//if(weld_line.PtInLine(loca_pt_l)<=0||weld_line.PtInLine(loca_pt_r)<=0)
	if(weld_line.PtInLine(loca_pt)<=0)
	{	//组焊对活标记点在组焊线外，进行调整(不需要考虑左右偏移点)
		GEPOINT weld_mid_pt=0.5*(weld_line.startPt+weld_line.endPt);
		this->liLocationAlongOffset=ftoi((weld_mid_pt-perp)*vRegWeldVec);
		/*f3dPoint off_vec=weld_mid_pt-perp;
		local_point_y=off_vec.mod();
		normalize(off_vec);
		if(off_vec.x*tube_line_vec[0]+off_vec.y*tube_line_vec[1]<0)
			local_point_y*=-1;*/
	}
}
//pNextVertex用于检测圆弧一致性
bool PROFILE_VERTEX::IsEqualWith(PROFILE_VERTEX* pCompVertex,PROFILE_VERTEX* pNextVertex,PROFILE_VERTEX* pCompNextVertex)
{
	if(!vertex.IsEqual(pCompVertex->vertex))
		return false;
	if(type==0&&pCompVertex->type==0||(pNextVertex==NULL&&pCompNextVertex==NULL))
		return true;
	else if(pNextVertex==NULL||pCompNextVertex==NULL)
		return false;	//非直线且缺少后续定义点
	if(type==3&&pCompVertex->type==3&&pNextVertex->vertex.IsEqual(pCompNextVertex->vertex)&&radius==pNextVertex->radius)
		return true;	//暂还未检测柱面轴向及圆心点影响，目前不急需，以后需要增加　wjh-2013.11.11
	if(type>0&&pCompVertex->type>0&&type<3&&pCompVertex->type<3)
	{	//圆弧
		f3dArcLine arc1,arc2;
		RetrieveArcLine(arc1,pNextVertex->vertex,NULL);
		pCompVertex->RetrieveArcLine(arc2,pCompNextVertex->vertex,NULL);
		if(fabs(arc1.SectorAngle()-arc2.SectorAngle())>EPS)
			return false;
		else if(arc1.WorkNorm()!=arc2.WorkNorm())
			return false;
		else
			return true;
	}
	else
		return false;
}
//板的外形顶点属性设置
IMPLEMENT_PROP_FUNC(PROFILE_VERTEX)
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
void PROFILE_VERTEX::InitProfileVertexPropHashtable()
{
	return InitPropHashtable();
}
void PROFILE_VERTEX::InitPropHashtable()
{
	int id=1;
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CXhChar500 sCmbItem,sHelpStr;
#ifdef AFX_TARG_ENU_ENGLISH
	PROFILE_VERTEX::AddPropItem("subjectPlateInfo",PROPLIST_ITEM(id++,"Subject Plate Information"));
	PROFILE_VERTEX::AddPropItem("m_hSubjectPlate",PROPLIST_ITEM(id++,"Subject Plate Handle"));
	PROFILE_VERTEX::AddPropItem("m_sSubjectPlateNo",PROPLIST_ITEM(id++,"Subject Plate Label"));
	PROFILE_VERTEX::AddPropItem("m_cSubjectPlateMaterial",PROPLIST_ITEM(id++,"Subject Plate Material"));
	PROFILE_VERTEX::AddPropItem("m_fSubjectPlateThick",PROPLIST_ITEM(id++,"Subject Plate Thick"));
	PROFILE_VERTEX::AddPropItem("m_relaPlate",PROPLIST_ITEM(id++,"Rela Plate"));
	PROFILE_VERTEX::AddPropItem("m_cPickedDisplayMode",PROPLIST_ITEM(id++,"Profile Visible","","None|Actual Profile|Spread Profile"));
	//
	PROFILE_VERTEX::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Outline Vertex Infomation"));
	PROFILE_VERTEX::AddPropItem("cVertexType",PROPLIST_ITEM(id++,"Vertex Type","","0.Datum Plane Vertex|1.2nd Bending Plane Point|2.1-2 Bending Point|3.3rd Bending Plane Point|4.1-3 Bending Point"));
	PROFILE_VERTEX::AddPropItem("vertex",PROPLIST_ITEM(id++,"Vertex Coordinate"));
	PROFILE_VERTEX::AddPropItem("vertex.x",PROPLIST_ITEM(id++,"X"));
	PROFILE_VERTEX::AddPropItem("vertex.y",PROPLIST_ITEM(id++,"Y"));
	PROFILE_VERTEX::AddPropItem("vertex.z",PROPLIST_ITEM(id++,"Z"));
	//
	PROFILE_VERTEX::AddPropItem("vertexDesignInfo", PROPLIST_ITEM(id++,"vertexDesignInfo"));
	PROFILE_VERTEX::AddPropItem("hRod",PROPLIST_ITEM(id++,"Dependent Rod Handle"));
	PROFILE_VERTEX::AddPropItem("start0_end1", PROPLIST_ITEM(id++,"Connecting End","","starting|ending|middle"));
	PROFILE_VERTEX::AddPropItem("wing_x0_y1", PROPLIST_ITEM(id++,"Connecting Wing","","X Leg|Y Leg"));
	sCmbItem.Copy("0.User Specify|1.Top Ridge Line Side Vertex|2.Top Leg Side Vertex|3.Bottom Ridge Line Side Vertex|4.Down Leg Side Vertex|5.Common Tangent Point|6.Straight Line Edge Bending Point|11.Param Point");
	PROFILE_VERTEX::AddPropItem("cPosDesignType",PROPLIST_ITEM(id++,"Vertex Calculate Type","",sCmbItem));
	PROFILE_VERTEX::AddPropItem("wing_space_style", PROPLIST_ITEM(id++,"Wing Side Space Type","","0.Specified|1.Standard Margin|2.Extend To Margin|3.Extend To Margin-thickness"));
	PROFILE_VERTEX::AddPropItem("wing_space", PROPLIST_ITEM(id++,"Wing Side Space"));
	PROFILE_VERTEX::AddPropItem("ber_space_style", PROPLIST_ITEM(id++,"Ridge Side Space Style","","0.Specified|1.Standard Margin|2.Extend To Margin|3.Extend To Margin-thickness"));
	PROFILE_VERTEX::AddPropItem("ber_space", PROPLIST_ITEM(id++,"Ridge Side Space"));
	PROFILE_VERTEX::AddPropItem("end_space_style", PROPLIST_ITEM(id++,"Wing Side Space Type","","0.Specified|1.Standard Margin"));
	PROFILE_VERTEX::AddPropItem("end_space", PROPLIST_ITEM(id++,"End Distance"));
	PROFILE_VERTEX::AddPropItem("xOffset.x",PROPLIST_ITEM(id++,"X vector off"));
	PROFILE_VERTEX::AddPropItem("xOffset.y",PROPLIST_ITEM(id++,"Y vector off"));
	//
	PROFILE_VERTEX::AddPropItem("ProfileEdgeInfo", PROPLIST_ITEM(id++,"Profile Edge Base Info"));
	PROFILE_VERTEX::AddPropItem("lineLength", PROPLIST_ITEM(id++,"line Length"));
	PROFILE_VERTEX::AddPropItem("startVertex", PROPLIST_ITEM(id++,"Start Vertex"));
	PROFILE_VERTEX::AddPropItem("startVertex.x", PROPLIST_ITEM(id++,"X"));
	PROFILE_VERTEX::AddPropItem("startVertex.y", PROPLIST_ITEM(id++,"Y"));
	PROFILE_VERTEX::AddPropItem("startVertex.z", PROPLIST_ITEM(id++,"Z"));
	PROFILE_VERTEX::AddPropItem("endVertex", PROPLIST_ITEM(id++,"End Vertex"));
	PROFILE_VERTEX::AddPropItem("endVertex.x", PROPLIST_ITEM(id++,"X"));
	PROFILE_VERTEX::AddPropItem("endVertex.y", PROPLIST_ITEM(id++,"Y"));
	PROFILE_VERTEX::AddPropItem("endVertex.z", PROPLIST_ITEM(id++,"Z"));
	//线型
	PROFILE_VERTEX::AddPropItem("HuoQuR", PROPLIST_ITEM(id++,"Bend R"));
	PROFILE_VERTEX::AddPropItem("NeutralLayer", PROPLIST_ITEM(id++,"Neutral Layer Coefficient"));
	PROFILE_VERTEX::AddPropItem("m_bRollEdge", PROPLIST_ITEM(id++,"rolled edge","rolled edge","Yes|No"));
	PROFILE_VERTEX::AddPropItem("manu_space1", PROPLIST_ITEM(id++,"height of rolled edge"));
	PROFILE_VERTEX::AddPropItem("roll_edge_offset_dist", PROPLIST_ITEM(id++,"outside offset value of rolled edge"));
	PROFILE_VERTEX::AddPropItem("m_bWeldEdge", PROPLIST_ITEM(id++,"Weld Edge Line","Weld Edge Line","Yes|No"));
	PROFILE_VERTEX::AddPropItem("manu_space", PROPLIST_ITEM(id++,"Work Clearance"));
	PROFILE_VERTEX::AddPropItem("local_point_vec", PROPLIST_ITEM(id++,"Weld Point Direction","Weld Point Direction","0.none|1.start->end|2.end->start"));
	PROFILE_VERTEX::AddPropItem("local_point_y", PROPLIST_ITEM(id++,"Weld Point Y"));
	PROFILE_VERTEX::AddPropItem("type", PROPLIST_ITEM(id++,"Edge Line Type","Edge Line Type","0.line|1.Specify Sector Angle|2.Specify R|3.Column Arc"));
	PROFILE_VERTEX::AddPropItem("radius", PROPLIST_ITEM(id++,"Circle R"));
	PROFILE_VERTEX::AddPropItem("center", PROPLIST_ITEM(id++,"Circle Center Pick Point Position"));
	PROFILE_VERTEX::AddPropItem("center.x", PROPLIST_ITEM(id++,"X"));
	PROFILE_VERTEX::AddPropItem("center.y", PROPLIST_ITEM(id++,"Y"));
	PROFILE_VERTEX::AddPropItem("center.z", PROPLIST_ITEM(id++,"Z"));
	PROFILE_VERTEX::AddPropItem("column_norm", PROPLIST_ITEM(id++,"Column Plane Axis Direction"));
	PROFILE_VERTEX::AddPropItem("column_norm.x", PROPLIST_ITEM(id++,"X"));
	PROFILE_VERTEX::AddPropItem("column_norm.y", PROPLIST_ITEM(id++,"Y"));
	PROFILE_VERTEX::AddPropItem("column_norm.z", PROPLIST_ITEM(id++,"Z"));
#else
	PROFILE_VERTEX::AddPropItem("subjectPlateInfo",PROPLIST_ITEM(id++,"隶属钢板信息"));
	PROFILE_VERTEX::AddPropItem("m_hSubjectPlate",PROPLIST_ITEM(id++,"句柄","隶属钢板句柄"));
	PROFILE_VERTEX::AddPropItem("m_sSubjectPlatePartNo",PROPLIST_ITEM(id++,"件号","隶属钢板编号"));
    PROFILE_VERTEX::AddPropItem("m_cSubjectPlateMaterial",PROPLIST_ITEM(id++,"材质","隶属钢板材质"));
	PROFILE_VERTEX::AddPropItem("m_fSubjectPlateThick",PROPLIST_ITEM(id++,"厚度","隶属钢板厚度"));
	PROFILE_VERTEX::AddPropItem("m_relaPlate",PROPLIST_ITEM(id++,"关联构件","隶属钢板的关联构件"));
	PROFILE_VERTEX::AddPropItem("m_cPickedDisplayMode",PROPLIST_ITEM(id++,"显示轮廓边","实体模式下钢板以是否以轮廓边","不显示|实际轮廓边|展开轮廓边"));
	//
	PROFILE_VERTEX::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"外形顶点基本信息"));
	PROFILE_VERTEX::AddPropItem("cVertexType",PROPLIST_ITEM(id++,"顶点类型","顶点所在的面号","0.基准面顶点|1.第2火曲面点|2.1-2面火曲点|3.第3火曲面点|4.1-3面火曲点"));
	PROFILE_VERTEX::AddPropItem("vertex",PROPLIST_ITEM(id++,"顶点坐标"));
	PROFILE_VERTEX::AddPropItem("vertex.x",PROPLIST_ITEM(id++,"X坐标"));
	PROFILE_VERTEX::AddPropItem("vertex.y",PROPLIST_ITEM(id++,"Y坐标"));
	PROFILE_VERTEX::AddPropItem("vertex.z",PROPLIST_ITEM(id++,"Z坐标"));
	//
	PROFILE_VERTEX::AddPropItem("vertexDesignInfo", PROPLIST_ITEM(id++,"外形顶点设计信息"));
	PROFILE_VERTEX::AddPropItem("hRod",PROPLIST_ITEM(id++,"依赖杆件句柄"));
	PROFILE_VERTEX::AddPropItem("start0_end1", PROPLIST_ITEM(id++,"连接端","","始端|终端|中间"));
	PROFILE_VERTEX::AddPropItem("wing_x0_y1", PROPLIST_ITEM(id++,"连接肢","","X肢|Y肢"));
	sCmbItem.Copy("0.用户自定义|1.顶部楞线侧顶点|2.顶部肢翼侧顶点|3.底部楞线侧顶点|4.底部肢翼侧顶点|5.公切点|6.直线边内制弯点|11.参数化板顶点");
	PROFILE_VERTEX::AddPropItem("cPosDesignType",PROPLIST_ITEM(id++,"顶点计算类型","",sCmbItem));
	PROFILE_VERTEX::AddPropItem("wing_space_style", PROPLIST_ITEM(id++,"肢翼侧距方式","肢翼侧螺栓距板边的距离确定方式","0.指定边距|1.标准边距|2.延伸至边|3.延伸至边-厚"));
	PROFILE_VERTEX::AddPropItem("wing_space", PROPLIST_ITEM(id++,"肢翼侧距"));
	PROFILE_VERTEX::AddPropItem("wing_space_syncmode", PROPLIST_ITEM(id++,"两端取值","","0.相同|1.不同"));
	PROFILE_VERTEX::AddPropItem("ber_space_style", PROPLIST_ITEM(id++,"楞线侧距方式","楞线侧螺栓距板边的距离确定方式","0.指定边距|1.标准边距|2.延伸至边|3.延伸至边-厚"));
	PROFILE_VERTEX::AddPropItem("ber_space", PROPLIST_ITEM(id++,"楞线侧距"));
	PROFILE_VERTEX::AddPropItem("ber_space_syncmode", PROPLIST_ITEM(id++,"两端取值","","0.相同|1.不同"));
	PROFILE_VERTEX::AddPropItem("end_space_style", PROPLIST_ITEM(id++,"端距方式","","0.指定边距|1.标准边距"));
	PROFILE_VERTEX::AddPropItem("end_space", PROPLIST_ITEM(id++,"端距"));
	PROFILE_VERTEX::AddPropItem("end_space_syncmode", PROPLIST_ITEM(id++,"两端取值","","0.相同|1.不同"));
	PROFILE_VERTEX::AddPropItem("xOffset.x",PROPLIST_ITEM(id++,"切角值"));
	PROFILE_VERTEX::AddPropItem("xOffset.y",PROPLIST_ITEM(id++,"切角值"));
	PROFILE_VERTEX::AddPropItem("fLengCutWidth",PROPLIST_ITEM(id++,"纵向切角值"));
	PROFILE_VERTEX::AddPropItem("fWingCutWidth",PROPLIST_ITEM(id++,"横向切角值"));
	//
	PROFILE_VERTEX::AddPropItem("ProfileEdgeInfo", PROPLIST_ITEM(id++,"轮廓边特征信息"));
	PROFILE_VERTEX::AddPropItem("lineLength", PROPLIST_ITEM(id++,"边长"));
	PROFILE_VERTEX::AddPropItem("startVertex", PROPLIST_ITEM(id++,"始点"));
	PROFILE_VERTEX::AddPropItem("startVertex.x", PROPLIST_ITEM(id++,"X"));
	PROFILE_VERTEX::AddPropItem("startVertex.y", PROPLIST_ITEM(id++,"Y"));
	PROFILE_VERTEX::AddPropItem("startVertex.z", PROPLIST_ITEM(id++,"Z"));
	PROFILE_VERTEX::AddPropItem("endVertex", PROPLIST_ITEM(id++,"终点"));
	PROFILE_VERTEX::AddPropItem("endVertex.x", PROPLIST_ITEM(id++,"X"));
	PROFILE_VERTEX::AddPropItem("endVertex.y", PROPLIST_ITEM(id++,"Y"));
	PROFILE_VERTEX::AddPropItem("endVertex.z", PROPLIST_ITEM(id++,"Z"));
	//线型
	PROFILE_VERTEX::AddPropItem("HuoQuR", PROPLIST_ITEM(id++,"火曲半径"));
	PROFILE_VERTEX::AddPropItem("NeutralLayer", PROPLIST_ITEM(id++,"中性层系数"));

	PROFILE_VERTEX::AddPropItem("m_bRollEdge", PROPLIST_ITEM(id++,"卷边","卷边","是|否"));
	PROFILE_VERTEX::AddPropItem("manu_space1", PROPLIST_ITEM(id++,"卷边高度"));
	PROFILE_VERTEX::AddPropItem("roll_edge_offset_dist", PROPLIST_ITEM(id++,"卷边外移量"));

	PROFILE_VERTEX::AddPropItem("m_bWeldEdge", PROPLIST_ITEM(id++,"焊缝线","焊缝线","是|否"));
	PROFILE_VERTEX::AddPropItem("manu_space", PROPLIST_ITEM(id++,"焊缝间隙"));
	PROFILE_VERTEX::AddPropItem("local_point_vec", PROPLIST_ITEM(id++,"对活点方向","对活点方向","0.无|1.始->终|2.终->始"));
	PROFILE_VERTEX::AddPropItem("local_point_y", PROPLIST_ITEM(id++,"对活点Y坐标"));
	PROFILE_VERTEX::AddPropItem("type", PROPLIST_ITEM(id++,"边线类型","边线类型","0.直线|1.指定扇形角|2.指定R|3.椭圆弧"));
	PROFILE_VERTEX::AddPropItem("radius", PROPLIST_ITEM(id++,"圆弧R"));
	PROFILE_VERTEX::AddPropItem("center", PROPLIST_ITEM(id++,"圆心拾取点"));
	PROFILE_VERTEX::AddPropItem("center.x", PROPLIST_ITEM(id++,"X"));
	PROFILE_VERTEX::AddPropItem("center.y", PROPLIST_ITEM(id++,"Y"));
	PROFILE_VERTEX::AddPropItem("center.z", PROPLIST_ITEM(id++,"Z"));
	PROFILE_VERTEX::AddPropItem("column_norm", PROPLIST_ITEM(id++,"圆柱面轴线方向"));
	PROFILE_VERTEX::AddPropItem("column_norm.x", PROPLIST_ITEM(id++,"X"));
	PROFILE_VERTEX::AddPropItem("column_norm.y", PROPLIST_ITEM(id++,"Y"));
	PROFILE_VERTEX::AddPropItem("column_norm.z", PROPLIST_ITEM(id++,"Z"));

#endif
}
static void GetLinePropValueStr(long id,char* valueStr,PROFILE_VERTEX* pVertex ,CLDSGeneralPlate* pBelongPlate/*=NULL*/)
{
	char sText[100]="";
	if(!pBelongPlate||!pVertex)
		return ;
	PROFILE_VERTEX* pNextVertex=NULL;
	for (PROFILE_VERTEX* pCurVertex=pBelongPlate->vertex_list.GetFirst();pCurVertex;pCurVertex=pBelongPlate->vertex_list.GetNext())
	{
		if(pVertex==pCurVertex)
		{
			pNextVertex=pBelongPlate->vertex_list.GetNext();
			break;
		}
	}
	if(pNextVertex==NULL)
		pNextVertex=pBelongPlate->vertex_list.GetFirst();
	if(pNextVertex==NULL)
		return;
	if(PROFILE_VERTEX::GetPropID("lineLength")==id)
	{
		strcpy(sText,"0");
		if(pBelongPlate!=NULL)
		{
			double dd=DISTANCE(pVertex->vertex,pCurVertex->vertex);
			sprintf(sText,"%f",dd);
			SimplifiedNumString(sText);
		}
	}
	else if(PROFILE_VERTEX::GetPropID("startVertex")==id)
		sprintf(sText,"%s",(char*)CXhChar200(pVertex->vertex));
	else if(PROFILE_VERTEX::GetPropID("startVertex.x")==id)
	{
		sprintf(sText,"%f",pVertex->vertex.x);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("startVertex.y")==id)
	{
		sprintf(sText,"%f",pVertex->vertex.y);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("startVertex.z")==id)
	{
		sprintf(sText,"%f",pVertex->vertex.z);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("endVertex")==id)
		sprintf(sText,"%s",(char*)CXhChar200(pNextVertex->vertex));
	else if(PROFILE_VERTEX::GetPropID("endVertex.x")==id)
	{
		strcpy(sText,"0");
		sprintf(sText,"%f",pNextVertex->vertex.x);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("endVertex.y")==id)
	{
		strcpy(sText,"0");
		sprintf(sText,"%f",pNextVertex->vertex.y);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("endVertex.z")==id)
	{
		strcpy(sText,"0");
		sprintf(sText,"%f",pNextVertex->vertex.z);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("HuoQuR")==id)
	{
	}
	else if(PROFILE_VERTEX::GetPropID("NeutralLayer")==id)		//中性层系数
	{
		
	}
	else if(PROFILE_VERTEX::GetPropID("m_bRollEdge")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pVertex->m_bRollEdge)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(pVertex->m_bRollEdge)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(PROFILE_VERTEX::GetPropID("manu_space")==id)		
	{
		if(pVertex->m_bWeldEdge)
		{
			sprintf(sText,"%hd",pVertex->manu_space);
			SimplifiedNumString(sText);
		}
		else
			sprintf(sText,"%s","0");
	}
	else if(PROFILE_VERTEX::GetPropID("roll_edge_offset_dist")==id)		
	{
		sprintf(sText,"%hd",pVertex->roll_edge_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("m_bWeldEdge")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pVertex->m_bWeldEdge)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else
		if(pVertex->m_bWeldEdge)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(PROFILE_VERTEX::GetPropID("manu_space1")==id)		
	{
		if (pVertex->m_bRollEdge)
		{
			sprintf(sText,"%hd",pVertex->manu_space);
			SimplifiedNumString(sText);
		}
		else
			sprintf(sText,"%s","0");
	}
	else if(PROFILE_VERTEX::GetPropID("local_point_y")==id)
	{
		sprintf(sText,"%f",pVertex->local_point_y);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("local_point_vec")==id)		
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		if(pVertex->local_point_vec==0)
			strcpy(sText,"0.none");
		else if(pVertex->local_point_vec==1)
			strcpy(sText,"1.start->end");
		else if(pVertex->local_point_vec==1)
			strcpy(sText,"2.end->start");
#else 
		if(pVertex->local_point_vec==0)
			strcpy(sText,"0.无");
		else if(pVertex->local_point_vec==1)
			strcpy(sText,"1.始->终");
		else if(pVertex->local_point_vec==1)
			strcpy(sText,"2.终->始");
#endif
	}
	else if(PROFILE_VERTEX::GetPropID("type")==id)		
	{
#ifdef	AFX_TARG_ENU_ENGLISH
		if(pVertex->type==0)
			strcpy(sText,"0.直线");
		else if(pVertex->type==1)
			strcpy(sText,"1.Specify Sector Angle");
		else if(pVertex->type==2)
			strcpy(sText,"2.Specify R");
		else if(pVertex->type==3)
			strcpy(sText,"3.Column Arc");
#else
		if(pVertex->type==0)
			strcpy(sText,"0.直线");
		else if(pVertex->type==1)
			strcpy(sText,"1.指定扇形角");
		else if(pVertex->type==2)
			strcpy(sText,"2.指定R");
		else if(pVertex->type==3)
			strcpy(sText,"3.椭圆弧");
#endif
	}
	else if(PROFILE_VERTEX::GetPropID("radius")==id)		
	{
		if(pVertex->type==1)		//指定扇形角
			sprintf(sText,"%f",pVertex->radius);
		else if(pVertex->type==2)	//指定R
			sprintf(sText,"%f",fabs(pVertex->radius));
		else if(pVertex->type==3)	//椭圆弧
			sprintf(sText,"%f",fabs(pVertex->radius));
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("center")==id)
		sprintf(sText,"%s",(char*)CXhChar200(pVertex->center));
	else if(PROFILE_VERTEX::GetPropID("center.x")==id)
	{
		sprintf(sText,"%f",pVertex->center.x);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("center.y")==id)
	{
		sprintf(sText,"%f",pVertex->center.y);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("center.z")==id)
	{
		sprintf(sText,"%f",pVertex->center.z);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("column_norm")==id)
		sprintf(sText,"%s",(char*)CXhChar200(pVertex->column_norm));
	else if(PROFILE_VERTEX::GetPropID("column_norm.x")==id)
	{
		sprintf(sText,"%f",pVertex->column_norm.x);
		SimplifiedNumString(sText);	
	}
	else if(PROFILE_VERTEX::GetPropID("column_norm.y")==id)
	{
		sprintf(sText,"%f",pVertex->column_norm.y);
		SimplifiedNumString(sText);
	}
	else if(PROFILE_VERTEX::GetPropID("column_norm.z")==id)
	{
		sprintf(sText,"%f",pVertex->column_norm.z);
		SimplifiedNumString(sText);
	}
	if(sText)
		strcpy(valueStr,sText);
}
int PROFILE_VERTEX::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[200]="";
	CXhString szText(sText,200);
	f3dPoint vertice=vertex;
	if(m_pTransCS)
		coord_trans(vertice,*m_pTransCS,FALSE);
	CLDSGeneralPlate* pBelongPlate=m_pBelongPlate;
	CDesignLjPartPara *pLjPartPara=NULL;
	CLDSGeneralPlate::GASKETEDGESPACE *pBoltPadSpace=NULL;
	if(pBelongPlate&&pBelongPlate->GetClassTypeId()==CLS_PLATE)
	{
		pLjPartPara=((CLDSPlate*)pBelongPlate)->GetLjRodPara(m_hRod);
		if(pBelongPlate->wiType==CLDSGeneralPlate::WASHER_PLATE)
			pBoltPadSpace=&pBelongPlate->gasketEdgeSpace;
	}
	//
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"");
	else if(pBelongPlate&&GetPropID("m_hSubjectPlate")==id)
		sprintf(sText,"0X%X",pBelongPlate->handle);
	else  if (pBelongPlate&&GetPropID("m_sSubjectPlatePartNo")==id)
		sprintf(sText,"%s",(char*)pBelongPlate->GetPartNo());
	else  if (pBelongPlate&&GetPropID("m_cSubjectPlateMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(pBelongPlate->cMaterial,matStr))
			sprintf(sText,"%s",matStr);
	}
	else if (pBelongPlate&&GetPropID("m_fSubjectPlateThick")==id)
		sprintf(sText,"%s",(char*)CXhChar100(pBelongPlate->GetThick()));
	else if (pBelongPlate&&GetPropID("m_relaPlate")==id)
	{
		for(RELATIVE_OBJECT *pRelaObj=pBelongPlate->relativeObjs.GetFirst();pRelaObj;pRelaObj=pBelongPlate->relativeObjs.GetNext())
			szText.Append(CXhChar50("0X%X",pRelaObj->hObj),',');
	}
	else if(pBelongPlate&&GetPropID("m_cPickedDisplayMode")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pBelongPlate->m_cPickedDisplayMode==0)
			strcpy(sText,"None");
		else if(pBelongPlate->m_cPickedDisplayMode==1)
			strcpy(sText,"Actual Profile");
		else if(pBelongPlate->m_cPickedDisplayMode==2)
			strcpy(sText,"Spread Profile");
#else
		if(pBelongPlate->m_cPickedDisplayMode==0)
			strcpy(sText,"不显示");
		else if(pBelongPlate->m_cPickedDisplayMode==1)
			strcpy(sText,"实际轮廓边");
		else if(pBelongPlate->m_cPickedDisplayMode==2)
			strcpy(sText,"展开轮廓边");
#endif
	}
	else if(GetPropID("cVertexType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(vertex.feature==2)
			sprintf(sText,"1.2nd Bending Plane Point");
		else if(vertex.feature==12)
			sprintf(sText,"2.1-2 Bending Point");
		else if(vertex.feature==3)
			sprintf(sText,"3.3rd Bending Plane Point");
		else if(vertex.feature ==13)
			sprintf(sText,"4.1-3 Bending Point");
		else //if(vertex.feature==1)
			sprintf(sText,"0.Datum Plane Vertex");
#else 
		if(vertex.feature==2)
			sprintf(sText,"1.第2火曲面点");
		else if(vertex.feature==12)
			sprintf(sText,"2.1-2面火曲点");
		else if(vertex.feature==3)
			sprintf(sText,"3.第3火曲面点");
		else if(vertex.feature ==13)
			sprintf(sText,"4.1-3面火曲点");
		else //if(vertex.feature==1)
			sprintf(sText,"0.基准面顶点");
#endif
	}
	else if(GetPropID("vertex.x")==id)
	{
		sprintf(sText,"%f",vertice.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("vertex.y")==id)
	{
		sprintf(sText,"%f",vertice.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("vertex.z")==id)
	{
		sprintf(sText,"%f",vertice.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cPosDesignType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_cPosDesignType==DT_NONE)
			sprintf(sText,"0.User Specify");
		else if(m_cPosDesignType==DT_BER)
			sprintf(sText,"1.Top Ridge Line Side Vertex");
		else if(m_cPosDesignType==DT_WING)
			sprintf(sText,"2.Top Leg Side Vertex");
		else if(m_cPosDesignType==DT_BER2)
			sprintf(sText,"3.Bottom Ridge Line Side Vertex");
		else if(m_cPosDesignType==DT_WING2)
			sprintf(sText,"4.Down Leg Side Vertex");
		else if(m_cPosDesignType==DT_ARCTANGENT)
			sprintf(sText,"5.Common Tangent Point");
		else if(m_cPosDesignType==DT_INNERBEND)
			sprintf(sText,"6.Straight Line Edge Bending Point");
		else if(m_cPosDesignType==DT_WELDSTART)//焊缝边起始顶点（基准杆件始端侧）
			sprintf(sText,"7.Weld Line Edge Start Vertex");
		else if(m_cPosDesignType==DT_WELDEND)		//焊缝边终止顶点（基准杆件始端侧）
			sprintf(sText,"8.Weld Line Edge End Vertex");
		else if(m_cPosDesignType==DT_WELDSTART_TIP)	//焊缝边起始顶点（基准杆件始端侧）
			sprintf(sText,"9.Weld Line Edge Start Extend Vertex");
		else if(m_cPosDesignType==DT_WELDEND_TIP)	//焊缝边终止顶点（基准杆件始端侧）
			sprintf(sText,"10.Weld Line Edge End Extend Vertex");
		else if(m_cPosDesignType==DT_PARAMETRIC)
			sprintf(sText,"11.PARAMETRIC Vertex");
#else 
		if(m_cPosDesignType==DT_NONE)
			sprintf(sText,"0.用户自定义");
		else if(m_cPosDesignType==DT_BER)
			sprintf(sText,"1.顶部楞线侧顶点");
		else if(m_cPosDesignType==DT_WING)
			sprintf(sText,"2.顶部肢翼侧顶点");
		else if(m_cPosDesignType==DT_BER2)
			sprintf(sText,"3.底部楞线侧顶点");
		else if(m_cPosDesignType==DT_WING2)
			sprintf(sText,"4.底部肢翼侧顶点");
		else if(m_cPosDesignType==DT_ARCTANGENT)
			sprintf(sText,"5.公切点");
		else if(m_cPosDesignType==DT_INNERBEND)
			sprintf(sText,"6.直线边内的制弯点");
		else if(m_cPosDesignType==DT_WELDSTART)//焊缝边起始顶点（基准杆件始端侧）
			sprintf(sText,"7.焊缝边起始端顶点");
		else if(m_cPosDesignType==DT_WELDEND)		//焊缝边终止顶点（基准杆件始端侧）
			sprintf(sText,"8.焊缝边终止端顶点");
		else if(m_cPosDesignType==DT_WELDSTART_TIP)	//焊缝边起始顶点（基准杆件始端侧）
			sprintf(sText,"9.焊缝边起始端延展顶点");
		else if(m_cPosDesignType==DT_WELDEND_TIP)	//焊缝边终止顶点（基准杆件始端侧）*/
			sprintf(sText,"10.焊缝边终止端延展顶点");
		else if(m_cPosDesignType==DT_PARAMETRIC)
			sprintf(sText,"11.参数化板顶点");
#endif
	}
	else if(GetPropID("hRod")==id)
		sprintf(sText,"0X%X",m_hRod);
	else if (pLjPartPara&&GetPropID("start0_end1")==id)
	{
		if(pLjPartPara->start0_end1==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Start");
		else if(pLjPartPara->start0_end1==1)
			strcpy(sText,"End");
		else
			strcpy(sText,"Middle");
#else 
			strcpy(sText,"始端");
		else if(pLjPartPara->start0_end1==1)
			strcpy(sText,"终端");
		else
			strcpy(sText,"中间");
#endif
	}
	else if (pLjPartPara&&GetPropID("wing_x0_y1")==id)
	{
		if(pLjPartPara->angle.cur_wing_x0_y1==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg");
		else
			strcpy(sText,"Y Leg");
#else 
			strcpy(sText,"X肢");
		else
			strcpy(sText,"Y肢");
#endif
	}
	else if(pLjPartPara&&GetPropID("wing_space_style")==id)
	{
		if(pLjPartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Specify Space");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.Standard Space");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			strcpy(sText,"2.Extend To Edge");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			strcpy(sText,"3.Extend To Edge-Thick");
#else 
			strcpy(sText,"0.指定边距");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.标准边距");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			strcpy(sText,"2.延伸至边");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			strcpy(sText,"3.延伸至边-厚");
#endif
	}
	else if(GetPropID("wing_space")==id)
	{
		if(pLjPartPara)
		{
			if(m_cPosDesignType==PROFILE_VERTEX::DT_BER2||m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
				sprintf(sText,"%.1f",pLjPartPara->startSideWingSpace);
			else if(m_cPosDesignType==PROFILE_VERTEX::DT_BER||m_cPosDesignType==PROFILE_VERTEX::DT_WING)
				sprintf(sText,"%.1f",pLjPartPara->endSideWingSpace);
			else
				sprintf(sText,"%.1f",pLjPartPara->wing_space);
			SimplifiedNumString(sText);
		}
		else if(pBoltPadSpace)
			sprintf(sText,"%d",pBoltPadSpace->wingSpace);
	}
	else if(pLjPartPara && GetPropID("ber_space_style")==id)
	{
		
		if(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Specify Space");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.Standard Space");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			strcpy(sText,"2.Extend To Edge");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			strcpy(sText,"3.Extend To Edge-Thick");
#else 
			strcpy(sText,"0.指定边距");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.标准边距");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			strcpy(sText,"2.延伸至边");
		else if(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			strcpy(sText,"3.延伸至边-厚");
#endif
	}
	else if(GetPropID("ber_space")==id)
	{
		if(pLjPartPara)
		{
			if(m_cPosDesignType==PROFILE_VERTEX::DT_BER2||m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
				sprintf(sText,"%.1f",pLjPartPara->startSideBerSpace);
			else if(m_cPosDesignType==PROFILE_VERTEX::DT_BER||m_cPosDesignType==PROFILE_VERTEX::DT_WING)
				sprintf(sText,"%.1f",pLjPartPara->endSideBerSpace);
			else
				sprintf(sText,"%.1f",pLjPartPara->ber_space);
			SimplifiedNumString(sText);
		}
		else if(pBoltPadSpace)
			sprintf(sText,"%d",pBoltPadSpace->berSpace);
	}
	else if(GetPropID("end_space_style")==id)
	{
		if(pLjPartPara!=NULL&&pLjPartPara->angle.cbSpaceFlag.StyleOfEndSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Specify Space");
		else //if(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.Standard Space");
#else 
			strcpy(sText,"0.指定边距");
		else //if(pLjPartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			strcpy(sText,"1.标准边距");
#endif
	}
	else if(GetPropID("end_space")==id)
	{
		if(pLjPartPara)
		{
			if(m_cPosDesignType==DT_WING2||m_cPosDesignType==DT_BER2)
				sprintf(sText,"%g",pLjPartPara->startSideEndSpace);
			else
				sprintf(sText,"%g",pLjPartPara->endSideEndSpace);
		}
		else if(pBoltPadSpace)
		{
			if(m_cPosDesignType==DT_WING2||m_cPosDesignType==DT_BER2)
				sprintf(sText,"%d",pBoltPadSpace->endSpaceS);
			else
				sprintf(sText,"%d",pBoltPadSpace->endSpaceE);
		}
	}
	else if(GetPropID("end_space_syncmode")==id)
	{
		if(pLjPartPara==NULL||!pLjPartPara->angle.bTwoEdge||pLjPartPara->ciAsynDualEdgeEndSpace==0)
			strcpy(sText,"0.相同");
		else if(pLjPartPara->ciAsynDualEdgeEndSpace==1)
			strcpy(sText,"1.不同");
	}
	else if(GetPropID("ber_space_syncmode")==id)
	{
		if(pLjPartPara==NULL||!pLjPartPara->angle.bTwoEdge||pLjPartPara->ciAsynDualEdgeBerSpace==0)
			strcpy(sText,"0.相同");
		else if(pLjPartPara->ciAsynDualEdgeBerSpace==1)
			strcpy(sText,"1.不同");
	}
	else if(GetPropID("wing_space_syncmode")==id)
	{
		if(pLjPartPara==NULL||!pLjPartPara->angle.bTwoEdge||pLjPartPara->ciAsynDualEdgeWingSpace==0)
			strcpy(sText,"0.相同");
		else if(pLjPartPara->ciAsynDualEdgeWingSpace==1)
			strcpy(sText,"1.不同");
	}
	else if(GetPropID("fLengCutWidth")==id)
		sprintf(sText,"%g",fLengCutWidth);
	else if(GetPropID("fWingCutWidth")==id)
		sprintf(sText,"%g",fWingCutWidth);
	else if(GetPropID("xOffset.x")==id)
	{
		if((xOffset.ciOffMode==0&&pBelongPlate->wiType==CLDSPlate::WASHER_PLATE)||xOffset.ciOffMode>0)
			sprintf(sText,"%g",fabs(xOffset.x));
		else
			sprintf(sText,"%g",xOffset.x);
	}
	else if(GetPropID("xOffset.y")==id)
	{
		if((xOffset.ciOffMode==0&&pBelongPlate->wiType==CLDSPlate::WASHER_PLATE)||xOffset.ciOffMode>0)
			sprintf(sText,"%g",fabs(xOffset.y));
		else
			sprintf(sText,"%g",xOffset.y);
	}
	else
		GetLinePropValueStr(id,sText,this,pBelongPlate);
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
BOOL PROFILE_VERTEX::IsPropEqual(SUBOBJSET &selectObjs, long idProp)
{
	SUB_OBJ* pSelObj=selectObjs.GetFirst();
	PROFILE_VERTEX *pVertex,*pFirstVertex=(PROFILE_VERTEX*)pSelObj->pSubObj;
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	pFirstVertex->GetPropValueStr(idProp,valueStr);
	selectObjs.push_stack();
	for(pSelObj=selectObjs.GetNext();pSelObj;pSelObj=selectObjs.GetNext())
	{
		pVertex=(PROFILE_VERTEX*)pSelObj->pSubObj;
		char valueStr2[500]="";
		pVertex->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}
//
IMPLEMENT_SET(CLDSGeneralPlate,cDatumAxis)
{
	if(toupper(value)=='X'||toupper(value)=='Y')
		m_cDatumAxis=toupper(value);
}
IMPLEMENT_GET(CLDSGeneralPlate,cDatumAxis)
{
	return m_cDatumAxis;
}
BYTE CLDSGeneralPlate::m_ciDisplayVertexType=1;
CLDSGeneralPlate::CLDSGeneralPlate()
{
	pLsRefList=new CLsRefList;
	pLsRefList->SetModifiedFlagAddress(m_pModified);
	style.iLegacyStyle=0;
	m_cDatumAxis='Y';	//以Z轴及Y轴为优先基准定义钢板装配坐标系
	m_cRollProcessType=0xFF;	//默认取整塔的默认值
	m_ciRollOffsetType=0xFF;	//默认取整塔的默认值
	m_ciDeformType=0XFF;		//默认取整塔的默认值
	m_fDeformCoef=0.35;
	m_fHuoquR=0;
	m_cPickedDisplayMode=0;
	m_bClockwiseCalProfile=false;
	m_uiInnerLoopBuffSize=0;
	m_pxInnerLoopBuff=NULL;
}
CLDSGeneralPlate::~CLDSGeneralPlate()
{
	if(pLsRefList!=NULL)
	{
		delete pLsRefList;
		pLsRefList=NULL;
	}
}
bool CLDSGeneralPlate::IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr)
{
	if(pObj==NULL||propKeyStr==NULL||pObj->GetClassTypeId()!=m_nClassTypeId)
		return false;
	CLDSGeneralPlate* pNode=(CLDSGeneralPlate*)pObj;
	MEMORY_BLOCK cl=mpcl;
	if(cl.IsZero())
		cl=MPCL;
	MEMORY_BLOCK mir_cl=pNode->mpcl;
	if(mir_cl.IsZero())
		mir_cl=MPCL;
	PROPLIST_ITEM* pItem=propMirSync.GetPropItem(propKeyStr);
	if(pItem!=NULL)
		return cl.GetBitState((WORD)pItem->id-1)&&mir_cl.GetBitState((WORD)pItem->id-1);
	else
	{
		Log2File()->Log("%s同步对称属性项缺失",propKeyStr);
		return false;
	}
}
//pMirCs!=NULL时保留钢板pMirPlate->ucs不变，仅更新轮廓点坐标
static bool SyncPlateVertexs(CLDSPlate *pSrcPlate,CLDSPlate* pMirPlate,MIRMSG msg,UCS_STRU* pMirCS=NULL)
{	//1.计算对称坐标系
	UCS_STRU ucs;
	ucs.origin=GetMirPos(pSrcPlate->ucs.origin,msg);
	ucs.axis_x=GetMirVector(pSrcPlate->ucs.axis_x,msg);
	ucs.axis_y=GetMirVector(pSrcPlate->ucs.axis_y,msg);
	ucs.axis_z=GetMirVector(pSrcPlate->ucs.axis_z,msg);
	BOOL bReverseOrder=(msg.axis_flag&4)==0&&(msg.axis_flag&8)==0&&pSrcPlate->jdb_style!=5;
	if(bReverseOrder)
	{
		if(pSrcPlate->cDatumAxis=='Y')
			ucs.axis_x*=-1.0;	//不是Z轴对称和旋转对称或附加板
		else
			ucs.axis_y*=-1.0;	//不是Z轴对称和旋转对称或附加板
	}
	if(pMirCS)
		*pMirCS=ucs;
	else
		pMirPlate->ucs=ucs;
	//2.对称轮廓点信息
	int nLjPartErrorCount=0;
	pMirPlate->vertex_list.Empty();
	PROFILE_VERTEX *pVertex, *pPrevVertex=pSrcPlate->vertex_list.GetTail();
	for(pVertex=pSrcPlate->vertex_list.GetFirst();pVertex;pVertex=pSrcPlate->vertex_list.GetNext())
	{	//2.1 对称轮廓顶点
		PROFILE_VERTEX* pTmVertex=NULL,vertex;
		if(!bReverseOrder)
		{	//2.1.1 直接把顶点列表复制过去即可(Z轴对称、旋转对称、相对于基准构件位置不动的钢板)
			vertex=*pVertex;
			if(vertex.type==3)	//椭圆弧
			{
				vertex.column_norm=GetMirVector(vertex.column_norm,msg);
				vertex.center=GetMirPos(vertex.center,msg);
			}
			pTmVertex=pMirPlate->vertex_list.append(vertex);
			MirPartHandle(pSrcPlate->BelongModel(),&pTmVertex->m_hRod,vertex.m_hRod,msg);
		}
		else
		{	//2.1.2 逆序插入轮廓点 
			vertex=*pPrevVertex;
			vertex.vertex.Set(-pVertex->vertex.x,pVertex->vertex.y,pVertex->vertex.z);
			vertex.vertex.feature=pVertex->vertex.feature;
			//由于Ｘ轴、Ｙ轴对称时圆心拾取点坐标也应进行相应变换
			if(vertex.type==2)	//指定圆弧半径及拾取点
				vertex.center.x*=-1;
			else if(vertex.type==3)	//椭圆弧
			{
				vertex.column_norm=GetMirVector(pPrevVertex->column_norm,msg);
				vertex.center=GetMirPos(pPrevVertex->center,msg);
			}
			vertex.m_cPosDesignType=pVertex->m_cPosDesignType;
			vertex.m_hRod=pVertex->m_hRod;
			pTmVertex=pMirPlate->vertex_list.insert(vertex,0);
			MirPartHandle(pSrcPlate->BelongModel(),&pTmVertex->m_hRod,vertex.m_hRod,msg);
			pPrevVertex=pVertex;
		}
		//2.2 更新轮廓顶点的计算类型(外形计算方式为仅更新坐标时使用顶点计算方式)
		if(vertex.m_cPosDesignType>0&&vertex.m_cPosDesignType<5)
		{	
			CDesignLjPartPara *pLjPart=pSrcPlate->GetLjRodPara(vertex.m_hRod);
			CDesignLjPartPara *pMirLjPart=pMirPlate->GetLjRodPara(pTmVertex->m_hRod);
			if(pLjPart==NULL||pMirLjPart==NULL)
			{
				nLjPartErrorCount++;
				continue;
			}
			pLjPart->pCurLinePart=(CLDSLinePart*)pSrcPlate->BelongModel()->FromPartHandle(pLjPart->hPart,CLS_LINEPART);
			pMirLjPart->pCurLinePart=(CLDSLinePart*)pSrcPlate->BelongModel()->FromPartHandle(pMirLjPart->hPart,CLS_LINEPART);
			if(pLjPart->pCurLinePart==NULL||pMirLjPart->pCurLinePart==NULL)
			{
				nLjPartErrorCount++;
				continue;
			}
			if(pLjPart->pCurLinePart->IsAngle()&&pMirLjPart->pCurLinePart->IsAngle())
			{
				CLDSLineAngle *pSrcAngle=(CLDSLineAngle*)pLjPart->pCurLinePart;
				CLDSLineAngle* pMirAngle=(CLDSLineAngle*)pMirLjPart->pCurLinePart;
				//2.2.1 肢翼侧与楞线侧需要更新
				GEPOINT src_work_wing_vec=pLjPart->angle.cur_wing_x0_y1==0?pSrcAngle->GetWingVecX():pSrcAngle->GetWingVecY();
				GEPOINT mir_work_wing_vec=pMirLjPart->angle.cur_wing_x0_y1==0?pMirAngle->GetWingVecX():pMirAngle->GetWingVecY();
				GEPOINT tmp_work_wing_vec=GetMirVector(src_work_wing_vec,msg);
				if(mir_work_wing_vec*tmp_work_wing_vec<0)
				{	
					if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER)
						pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING;
					else if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2)
						pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING2;
					else if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING)
						pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER;
					else if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
						pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER2;
				}
				//2.2.2 启用两端轮廓时两端轮廓点类型需要更新
				if(pLjPart->angle.bTwoEdge)
				{
					f3dPoint line_vec=pLjPart->pCurLinePart->End()-pLjPart->pCurLinePart->Start();
					f3dPoint mir_line_vec=GetMirVector(line_vec,msg);
					f3dPoint tm_line_vec=pMirAngle->End()-pMirAngle->Start();
					if(mir_line_vec*tm_line_vec<0)	//杆件朝向相反
					{	//两端轮廓点需要对调
						if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER)
							pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER2;
						else if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2)
							pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER;
						else if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING)
							pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING2;
						else if(pTmVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2)
							pTmVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING;
					}
				}
			}
			else if(pLjPart->pCurLinePart->GetClassTypeId()!=pMirLjPart->pCurLinePart->GetClassTypeId())
				nLjPartErrorCount++;
		}
		//2.4 不更新对称钢板坐标系时，需要将轮廓点坐标转换到目标钢板坐标系中
		if(pMirCS)
		{
			coord_trans(pTmVertex->vertex,ucs,TRUE);
			coord_trans(pTmVertex->vertex,pMirPlate->ucs,FALSE);
			pTmVertex->vertex.z=0;
		}
	}
	//3.反序插入轮廓点时，需更新轮廓点上的轮廓边附属信息
	if(bReverseOrder)
	{
		PROFILE_EDGEINFO edgeinfo,tailedgeinfo;
		PROFILE_VERTEX* pTailVertex=pPrevVertex=pMirPlate->vertex_list.GetTail();
		pPrevVertex->CopyEdgeInfoTo(&tailedgeinfo);
		for(pVertex=pMirPlate->vertex_list.GetFirst();pVertex;pVertex=pMirPlate->vertex_list.GetNext())
		{
			if(pVertex==pTailVertex)
				edgeinfo=tailedgeinfo;
			else
				pVertex->CopyEdgeInfoTo(&edgeinfo);
			pPrevVertex->CopyEdgeInfoFrom(edgeinfo);
		}
	}
	return nLjPartErrorCount==0;
}
struct BOLT_EX{
	CLDSBolt *pBolt;
	GEPOINT pos;	//相对位置
	BOLT_EX(){pBolt=NULL;}
	BOLT_EX(CLDSBolt *pBolt,GEPOINT bolt_pos){this->pBolt=pBolt;pos=bolt_pos;}
};
struct ROD_BOLTS{
	DWORD dwKey;
	CLDSLinePart *pRod,*pMirRod;
	ARRAY_LIST<BOLT_EX> boltArr;
	ROD_BOLTS(){pRod=NULL;pMirRod=NULL;}
	void SetKey(DWORD key){dwKey=key;}
};
struct PEER_BOLTPTR{
	CLDSBolt *m_pBolt;
	CLDSBolt *m_pMirBolt;
	PEER_BOLTPTR(CLDSBolt *pBolt=NULL,CLDSBolt *pMirBolt=NULL){m_pBolt=pBolt; m_pMirBolt=pMirBolt;}
};
static int CompareBoltEx(const BOLT_EX& boltEx1,const BOLT_EX& boltEx2)
{
	if(boltEx1.pos.z-boltEx2.pos.z>eps2)
		return 1;
	else if(boltEx2.pos.z-boltEx1.pos.z>eps2)
		return -1;
	else
	{
		if(boltEx1.pos.x-boltEx2.pos.x>eps2)
			return 1;
		else
			return -1;
	}
}
//TODO:1.局部类统一定成一个xxxLocal.h
//2.有时间时把钢板同步写成一个CSynPlateObject
bool SyncPlateProfile(CLDSPlate *pSrcPlate,CLDSPlate *pDestPlate,MIRMSG msg)
{	//1.根据原始钢板及对称关系计算对称钢板轮廓点坐标
	UCS_STRU mir_ucs;
	bool bRetCode=SyncPlateVertexs(pSrcPlate,pDestPlate,msg,&mir_ucs);
	//2.更新钢板连接杆件设计参数
	int nSrcUnmatched=0,nDestUnmatched=0;
	CHashSet<CDesignLjPartPara*> hashMirLjParaBySrcHandle,hashSrcLjParaByMirHandle;
	for(CDesignLjPartPara *pLjPara=pSrcPlate->GetFirstLjPara();pLjPara;pLjPara=pSrcPlate->GetNextLjPara())
	{
		long hMirRod=0;
		MirPartHandle(pSrcPlate->BelongModel(),&hMirRod,pLjPara->hPart,msg);
		CLDSLinePart *pMirRod=(CLDSLinePart*)pSrcPlate->BelongModel()->FromPartHandle(hMirRod,CLS_LINEPART);
		CDesignLjPartPara *pMirLjPara=pDestPlate->designInfo.FromHandle(hMirRod);
		if(pMirLjPara==NULL&&pMirRod&&pMirRod->IsAngle())
		{	//添加新的射线杆件
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pMirRod;
			pMirLjPara=pDestPlate->designInfo.partList.SetValue(pMirRod->handle,*pLjPara);
			GEPOINT work_norm=pMirLjPara->iFaceNo>1?pDestPlate->ucs.axis_z:pDestPlate->huoqufacenorm[pMirLjPara->iFaceNo-2];
			if(fabs(pAngle->vxWingNorm*work_norm)>fabs(pAngle->vyWingNorm*work_norm))
				pMirLjPara->angle.cur_wing_x0_y1=0;
			else
				pMirLjPara->angle.cur_wing_x0_y1=1;
		}
		if(pMirLjPara)
		{
			hashMirLjParaBySrcHandle.SetValue(pLjPara->hPart,pMirLjPara);
			hashSrcLjParaByMirHandle.SetValue(pMirLjPara->hPart,pLjPara);
			//
			pMirLjPara->angle.cbSpaceFlag=pLjPara->angle.cbSpaceFlag;
			pMirLjPara->angle.bTwoEdge=pLjPara->angle.bTwoEdge;
			pMirLjPara->angle.bEndLjJg=pLjPara->angle.bEndLjJg;
			pMirLjPara->iFaceNo=pLjPara->iFaceNo;
			pMirLjPara->end_space=pLjPara->end_space;
			pMirLjPara->ber_space=pLjPara->ber_space;
			pMirLjPara->wing_space=pLjPara->wing_space;
			if(pLjPara->start0_end1==0||pLjPara->start0_end1==1)
			{
				CLDSLinePart *pRod=(CLDSLinePart*)pSrcPlate->BelongModel()->FromPartHandle(pLjPara->hPart,CLS_LINEPART);
				if(pRod&&pMirRod)
				{
					pRod->BuildUCS();
					pMirRod->BuildUCS();
					GEPOINT mir_axis_z=GetMirVector(pRod->ucs.axis_z,msg);
					if(mir_axis_z*pMirRod->ucs.axis_z<0)
						pMirLjPara->start0_end1=1-pLjPara->start0_end1;
					else
						pMirLjPara->start0_end1=pLjPara->start0_end1;
				}
				//else 保留原有start0_end1数值
			}
			else
				pMirLjPara->start0_end1=pLjPara->start0_end1;
		}
		else
			nSrcUnmatched++;
	}
	//目标钢板上多余的射线杆件
	for(CDesignLjPartPara *pLjPara=pDestPlate->GetFirstLjPara();pLjPara;pLjPara=pDestPlate->GetNextLjPara())
	{
		if(hashSrcLjParaByMirHandle.GetValue(pLjPara->hPart)==NULL)
			nDestUnmatched++;
	}
	//3.更新关联螺栓参数
	const DWORD RODBOLTS_NULL_KEY=0xFFFFFFFF;
	if(hashMirLjParaBySrcHandle.GetNodeNum()==0||nSrcUnmatched>0||nDestUnmatched>0)
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The profile of plate(0x%X and 0x%X) synchornize failed(Connecting rods do not match)！",pSrcPlate->handle,pDestPlate->handle);
#else
		logerr.Log("钢板0x%X与0x%X外形同步失败(连接杆件信息不匹配)！",pSrcPlate->handle,pDestPlate->handle);
#endif
	if(hashMirLjParaBySrcHandle.GetNodeNum()>0)
	{	//3.1 按定位基准构件将螺栓分组,同时记录螺栓在定位基准构件中的相对坐标
		CHashListEx<ROD_BOLTS> hashSrcRodBoltsByHandle,hashMirRodBoltsByHandle;
		for(CLsRef *pLsRef=pSrcPlate->GetFirstLsRef();pLsRef;pLsRef=pSrcPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			CLDSLinePart *pRod=(CLDSLinePart*)pSrcPlate->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart,CLS_LINEPART);
			DWORD dwKey=(pRod==NULL)?RODBOLTS_NULL_KEY:pRod->handle;	//螺栓定位构件不是杆件时以0xFFFFFFFF作为key值
			ROD_BOLTS *pRodBolts=hashSrcRodBoltsByHandle.GetValue(dwKey);
			if(pRodBolts==NULL)
			{
				pRodBolts=hashSrcRodBoltsByHandle.Add(dwKey);
				pRodBolts->pRod=pRod;
				if(pRodBolts->pRod)
					pRodBolts->pRod->BuildUCS();
				CDesignLjPartPara *pMirLjPara=hashMirLjParaBySrcHandle.GetValue(pBolt->des_base_pos.hPart);
				if(pMirLjPara)
					pRodBolts->pMirRod=(CLDSLinePart*)pDestPlate->BelongModel()->FromPartHandle(pMirLjPara->hPart,CLS_LINEPART);
			}
			GEPOINT ls_pos=pBolt->ucs.origin;
			if(pRodBolts->pRod)	//转换到杆件坐标系下
			{
				pRodBolts->pRod->GetBoltIntersPos(pBolt,ls_pos);
				coord_trans(ls_pos,pRodBolts->pRod->ucs,FALSE);
			}
			else				//转换到对称坐标系下
			{
				pSrcPlate->GetBoltIntersPos(pBolt,ls_pos);
				ls_pos=msg.MirPoint(ls_pos);
				coord_trans(ls_pos,mir_ucs,FALSE,TRUE);
			}
			pBolt->feature=0;
			pRodBolts->boltArr.append(BOLT_EX(pBolt,ls_pos));
		}
		for(CLsRef *pLsRef=pDestPlate->GetFirstLsRef();pLsRef;pLsRef=pDestPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			CLDSLinePart *pRod=(CLDSLinePart*)pDestPlate->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart,CLS_LINEPART);
			DWORD dwKey=(pRod==NULL)?RODBOLTS_NULL_KEY:pRod->handle;	//螺栓定位构件不是杆件时以0xFFFFFFFF作为key值
			ROD_BOLTS *pRodBolts=hashMirRodBoltsByHandle.GetValue(dwKey);
			if(pRodBolts==NULL)
			{
				pRodBolts=hashMirRodBoltsByHandle.Add(dwKey);
				pRodBolts->pRod=pRod;
				if(pRodBolts->pRod)
					pRodBolts->pRod->BuildUCS();
			}
			GEPOINT ls_pos=pBolt->ucs.origin;
			if(pRodBolts->pRod)	//转换到杆件坐标系下
			{
				pRodBolts->pRod->GetBoltIntersPos(pBolt,ls_pos);
				coord_trans(ls_pos,pRodBolts->pRod->ucs,FALSE,TRUE);
			}
			else				//转换到对称坐标系下
			{
				pDestPlate->GetBoltIntersPos(pBolt,ls_pos);
				coord_trans(ls_pos,mir_ucs,FALSE,TRUE);
			}
			pBolt->feature=0;
			pRodBolts->boltArr.append(BOLT_EX(pBolt,ls_pos));
		}
		//3.2 将每一组螺栓进行排序
		for(ROD_BOLTS *pRodBolts=hashSrcRodBoltsByHandle.GetFirst();pRodBolts;pRodBolts=hashSrcRodBoltsByHandle.GetNext())
		{
			bool bAscending=true;
			if(pRodBolts->pRod)
			{
				f3dPoint mir_axis_z,axis_z=pRodBolts->pRod->ucs.axis_z;
				if(pRodBolts->pMirRod)
					mir_axis_z=GetMirVector(pRodBolts->pMirRod->ucs.axis_z,msg);
				if(axis_z*mir_axis_z<0)
					bAscending=false;
			}
			CBubbleSort<BOLT_EX>::BubSort(pRodBolts->boltArr.m_pData,pRodBolts->boltArr.GetSize(),CompareBoltEx,bAscending);
		}
		for(ROD_BOLTS *pRodBolts=hashMirRodBoltsByHandle.GetFirst();pRodBolts;pRodBolts=hashMirRodBoltsByHandle.GetNext())
			CBubbleSort<BOLT_EX>::BubSort(pRodBolts->boltArr.m_pData,pRodBolts->boltArr.GetSize(),CompareBoltEx);
		//3.3 同步螺栓属性
		int nErrorCount=0;
		for(ROD_BOLTS *pRodBolts=hashSrcRodBoltsByHandle.GetFirst();pRodBolts;pRodBolts=hashSrcRodBoltsByHandle.GetNext())
		{
			ROD_BOLTS *pMirRodBolts=NULL;
			if(pRodBolts->dwKey==RODBOLTS_NULL_KEY)
			{
				pMirRodBolts=hashMirRodBoltsByHandle.GetValue(RODBOLTS_NULL_KEY);
				if(pMirRodBolts==NULL)
				{
					nErrorCount++;
					continue;
				}
			}
			else
			{
				if(pRodBolts->pRod==NULL)
				{
					nErrorCount++;
					continue;
				}
				CDesignLjPartPara *pMirLjPara=hashMirLjParaBySrcHandle.GetValue(pRodBolts->pRod->handle);
				if(pMirLjPara==NULL)
				{
					nErrorCount++;
					continue;
				}
				pMirRodBolts=hashMirRodBoltsByHandle.GetValue(pMirLjPara->hPart);
				if(pMirRodBolts==NULL)
				{
					nErrorCount++;
					continue;
				}
			}
			ARRAY_LIST<PEER_BOLTPTR> peerBoltList;
			if(pRodBolts->dwKey==RODBOLTS_NULL_KEY)
			{	//根据螺栓坐标进行匹配，单纯根据螺栓坐标匹配不准确
				const int OFFSET_EPS=5;
				for(int i=0;i<pRodBolts->boltArr.GetSize();i++)
				{
					for(int j=0;j<pMirRodBolts->boltArr.GetSize();j++)
					{
						if(1==pMirRodBolts->boltArr[j].pBolt->feature)
							continue;
						if(DISTANCE(pRodBolts->boltArr[i].pos,pMirRodBolts->boltArr[j].pos)<OFFSET_EPS)
						{
							pRodBolts->boltArr[i].pBolt->feature=pMirRodBolts->boltArr[j].pBolt->feature=1;
							peerBoltList.append(PEER_BOLTPTR(pRodBolts->boltArr[i].pBolt,pMirRodBolts->boltArr[j].pBolt));
							break;
						}
					}
				}
				if(pRodBolts->boltArr.GetSize()!=pMirRodBolts->boltArr.GetSize())
					logerr.Log("0x%x钢板上的孔数与对称钢板0x%x不同",pSrcPlate->handle,pDestPlate->handle);
				else
				{
					BOLT_EX *pBoltHole=NULL;
					for(int i=0;i<pRodBolts->boltArr.GetSize();i++)
					{
						if(pRodBolts->boltArr[i].pBolt->feature==0)
							pBoltHole=&pRodBolts->boltArr[i];
					}
					if(pBoltHole)
						logerr.Log("0x%x钢板上的0x%x孔未在0x%x钢板上找到匹配的对称孔进行同步更新",pSrcPlate->handle,pBoltHole->pBolt->handle,pDestPlate->handle);
				}
			}
			else
			{
				for(int i=0;i<pRodBolts->boltArr.GetSize()&&i<pMirRodBolts->boltArr.GetSize();i++)
					peerBoltList.append(PEER_BOLTPTR(pRodBolts->boltArr[i].pBolt,pMirRodBolts->boltArr[i].pBolt));
				if(pRodBolts->boltArr.GetSize()!=pMirRodBolts->boltArr.GetSize())
					logerr.Log("0x%x杆件上的当前连接螺栓数与对称杆件0x%x杆件不同",
								pRodBolts->pRod?pRodBolts->pRod->handle:0,pMirRodBolts->pMirRod?pMirRodBolts->pMirRod->handle:0);
			}
			for(PEER_BOLTPTR *pPeerBolt=peerBoltList.GetFirst();pPeerBolt;pPeerBolt=peerBoltList.GetNext())
			{	//同步螺栓属性
				CLDSBolt *pBolt=pPeerBolt->m_pBolt;
				CLDSBolt *pMirBolt=pPeerBolt->m_pMirBolt;
				if(pBolt==NULL||pMirBolt==NULL)
					continue;
				pBolt->SyncMirPropTo(pMirBolt,msg);
				GEPOINT location,location2;
				pSrcPlate->GetBoltIntersPos(pBolt,location);
				location=msg.MirPoint(location);
				pDestPlate->GetBoltIntersPos(pMirBolt,location2);
				double dist=DISTANCE(location,location2);
				if(dist>0.5)
					logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"0x%x螺栓与对称的0x%x螺栓同步后位置发生%.1fmm偏移",pBolt->handle,pMirBolt->handle,dist);
				else if(dist>0.1)
					logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%x螺栓与对称的0x%x螺栓同步后位置发生%.1fmm偏移",pBolt->handle,pMirBolt->handle,dist);
			}
		}
		if(nErrorCount>0||hashSrcRodBoltsByHandle.GetValue(RODBOLTS_NULL_KEY))
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("The profile of plate(0x%X and 0x%X) synchornize failed(Bolts do not match)！",pSrcPlate->handle,pDestPlate->handle);
#else
			logerr.Log("钢板0x%X与0x%X外形同步失败(螺栓匹配失败)！",pSrcPlate->handle,pDestPlate->handle);
#endif
	}
	//4.同步钢板制弯信息
	for(int i=0;i<pDestPlate->face_N-1;i++)
	{
		pSrcPlate->designInfo.huoquline_start[i].MirToPoint(pDestPlate->BelongModel(),pDestPlate->designInfo.huoquline_start[i],msg);
		pSrcPlate->designInfo.huoquline_end[i].MirToPoint(pDestPlate->BelongModel(),pDestPlate->designInfo.huoquline_end[i],msg);
		pSrcPlate->designInfo.huoqufacenorm[i].MirToVector(pDestPlate->BelongModel(),pDestPlate->designInfo.huoqufacenorm[i],msg);
		pDestPlate->designInfo.huoqufacenorm[i].UpdateVector(pDestPlate->BelongModel());
		//基准角钢里外铁样式不一致时，对称钢板Z轴与(基准钢板Z轴)对称方向相反,制弯面方向也应该反向
		//CreateSingleMirPlate()函数中也有相关代码,将来应统一处理 wht 16-09-19
		double dd1=pSrcPlate->huoqufacenorm[i]*pSrcPlate->ucs.axis_z;
		double dd2=pDestPlate->designInfo.huoqufacenorm[i].vector*pDestPlate->designInfo.norm.vector;
		if(dd1*dd2<0)
			pDestPlate->designInfo.huoqufacenorm[i].ReverseDirection();
	}
	//5.重新设计钢板外形
	PROFILE_VERTEX *pVertex;
	ARRAY_LIST<PROFILE_VERTEX> vertices(0,pDestPlate->vertex_list.GetNodeNum());
	for(pVertex=pDestPlate->vertex_list.GetFirst();pVertex;pVertex=pDestPlate->vertex_list.GetNext())
		vertices.append(*pVertex);
	pDestPlate->designInfo.m_bEnableFlexibleDesign=pSrcPlate->designInfo.m_bEnableFlexibleDesign;
	pDestPlate->designInfo.iProfileStyle0123=pSrcPlate->designInfo.iProfileStyle0123;
	pDestPlate->designInfo.m_bSelfCorrectBendline=pSrcPlate->designInfo.m_bSelfCorrectBendline;
	pDestPlate->DesignPlate();
	pDestPlate->SetModified();
	if(pDestPlate->vertex_list.GetNodeNum()!=vertices.GetSize())
	{
		logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X钢板同步时参数化设计前后钢板顶点数不同",pDestPlate->handle);
		return FALSE;
	}
	for(pVertex=pDestPlate->vertex_list.GetFirst();pVertex;pVertex=pDestPlate->vertex_list.GetNext())
	{
		CMinDouble dist;
		for(int i=0;i<vertices.GetSize();i++)	//DesignPlate过程中顶点顺序可能会发生变化，故必须按位置重新搜索
			dist.Update(DISTANCE(vertices[i].vertex,pVertex->vertex),(void*)i,0.1);
		int matchI=(int)dist.m_pRelaObj;
		if(dist.number>0.5)
		{
			logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"0x%X钢板同步时顶点(%.1f,%.1f)，参数化设计前后发生%.1fmm移位",pDestPlate->handle,dist.number);
		}
		else if(dist.number>0.1)
		{
			logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,
				"0x%X钢板同步时顶点(%.1f,%.1f)，参数化设计前后发生%.1fmm移位",
				pDestPlate->handle,pVertex->vertex.x,pVertex->vertex.y,dist.number);
		}
		if( pVertex->m_bWeldEdge!=vertices[matchI].m_bWeldEdge||pVertex->m_bRollEdge!=vertices[matchI].m_bRollEdge||
			pVertex->manu_space!=vertices[matchI].manu_space)
		{
			logerr.LevelLog(CLogFile::WARNING_LEVEL2_COMMON,
				"0x%X钢板同步时顶点(%.1f,%.1f)为起始的板边，工艺信息同步失败",
				pDestPlate->handle,pVertex->vertex.x,pVertex->vertex.y);
		}
	}
	return TRUE;
}

bool CLDSGeneralPlate::SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL||pDestObj->GetClassTypeId()!=m_nClassTypeId)
		return false;
	CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pDestObj;
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//分段号
		pPlate->iSeg=iSeg;
		pPlate->SetModified(TRUE,FALSE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cMaterial")==0)&&IsSyncMirPropWith(pDestObj,"cMaterial"))
	{	//分段号
		pPlate->cMaterial=cMaterial;
		pPlate->cbVirginQualityLevel=cbVirginQualityLevel;
		pPlate->SetModified(TRUE,FALSE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//分段号
		pPlate->Thick=Thick;
		pPlate->SetModified(TRUE,TRUE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//配材号
		pPlate->cfgword=cfgword;
		pPlate->SetModified(TRUE,FALSE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"thick")==0)&&IsSyncMirPropWith(pDestObj,"thick"))
	{	//厚度
		pPlate->Thick=Thick;
		pPlate->SetModified(TRUE,FALSE);
	}
	return true;
}
bool CLDSGeneralPlate::SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL||pDestObj->GetClassTypeId()!=m_nClassTypeId)
		return false;
	CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pDestObj;
	if((propKeyStr==NULL||strcmp(propKeyStr,"layer")==0)&&IsSyncMirPropWith(pDestObj,"layer"))
	{	//图层名同步对称
		MirObjLayer(layer(),pPlate->layer(),mirmsg);
		pPlate->SetModified(TRUE,FALSE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//分段号
		pPlate->iSeg=iSeg;
		pPlate->SetModified(TRUE,FALSE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cMaterial")==0)&&IsSyncMirPropWith(pDestObj,"cMaterial"))
	{	//分段号
		pPlate->cMaterial=cMaterial;
		pPlate->cbVirginQualityLevel=cbVirginQualityLevel;
		pPlate->SetModified(TRUE,FALSE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//分段号
		pPlate->Thick=Thick;
		pPlate->SetModified(TRUE,TRUE);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//配材号
		pPlate->cfgword=cfgword;
		pPlate->SetModified(TRUE,FALSE);
	}
	return true;
}
bool CLDSGeneralPlate::SyncMirProp(const char* propKeyStr/*=NULL*/,BYTE ciMirAll0X1Y2Z3/*=0*/,DWORD dwPropFilterFlag/*=0xffffffff*/)//propKeyStr表示同步对称全部属性
{
	//CRelaObjGroup* pRelaGroup=BelongModel()->FindRelaObjGroup(handle);
	//if(pRelaGroup==NULL)
	//	return false;
	//CRelaObjGroup::RELA_OBJECT* pTriggerRela=pRelaGroup->relaObjs.GetValue(handle);
	//if(pTriggerRela==NULL)
	//	return false;
	//for(CRelaObjGroup::RELA_OBJECT* pRela=pRelaGroup->relaObjs.GetFirst();pRela;pRela=pRelaGroup->relaObjs.GetNext())
	MIRMSG xSpecMirInfo(ciMirAll0X1Y2Z3);
	for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		//if(pRela==pTriggerRela)
		//	continue;
		//CRelaObjGroup::MIRINFO mirinfo=CRelaObjGroup::TransMirInfo(pTriggerRela->hMirAcs,pTriggerRela->wRotAngle,pRela->hMirAcs,pRela->wRotAngle);
		if(ciMirAll0X1Y2Z3!=0&&!pRelaObj->mirInfo.IsSameMirMsg(xSpecMirInfo))
			continue;
		BOOL pushed=relativeObjs.push_stack();
		CLDSPart* pPart=BelongModel()->FromPartHandle(pRelaObj->hObj);
		SyncMirPropTo(pPart,pRelaObj->mirInfo,propKeyStr);
		relativeObjs.pop_stack(pushed);
	}
	return true;
}
// 板
int CLDSPlate::nPlankEdgeAngleThreshold=80;
IMPLEMENT_GET(CLDSPlate,Thick){
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->Thick;
	else
		return _thick;
}
IMPLEMENT_SET(CLDSPlate,Thick){
	if(value>=0)
	{
		CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
		double fOldThick=_thick;
		_thick = ftoi(value);
		SetModified();
		if(pMotherObj!=NULL&&pMotherObj!=this)
			pMotherObj->Thick=value;
		if(designInfo.origin.m_hDatumRod>0x20)
		{
			CSuperSmartPtr<CLDSPart> pDatumRod=BelongModel()->FromPartHandle(designInfo.origin.m_hDatumRod,CLS_GROUPLINEANGLE);
			if(pDatumRod.IsHasPtr()&&pDatumRod.pGroupAngle->size_cSubClassType=='T')
			{
				int offsetwing=0;
				if(designInfo.origin.datum_pos_style==1)
					offsetwing=designInfo.origin.des_para.RODEND.offset_wing;
				else if(designInfo.origin.datum_pos_style==2||designInfo.origin.datum_pos_style==7)
					offsetwing=designInfo.origin.des_para.RODNODE.offset_wing;
				if(pDatumRod.pGroupAngle->ciMasterIndex!=offsetwing)
					pDatumRod=NULL;	//钢板贴合在T型组合角钢的顶部大平面上,视同单角钢 wjh-2016.11.8
			}
			if(pDatumRod.IsHasPtr())	//以组合角钢为定位基准时,应随动更改法向偏移量
			{
				if(designInfo.origin.datum_pos_style==1)
				{
					CParaThick thickpara(designInfo.origin.des_para.RODEND.norm_offset);
					thickpara.AddThick(ftoi((fOldThick-_thick)*0.5));
					StrCopy(designInfo.origin.des_para.RODEND.norm_offset,(char*)thickpara.ToThickStr(),51);
				}
				else if(designInfo.origin.datum_pos_style==2||designInfo.origin.datum_pos_style==7)
				{
					CParaThick thickpara(designInfo.origin.des_para.RODNODE.norm_offset);
					thickpara.AddThick(ftoi((fOldThick-_thick)*0.5));
					StrCopy(designInfo.origin.des_para.RODNODE.norm_offset,(char*)thickpara.ToThickStr(),51);
				}
			}
		}
	}
}
CLDSPlate::CLDSPlate(void)
{
	m_nClassTypeId=CLS_PLATE;
	//strcpy(m_sClassName,"CLDSPlate");
	_thick = 8;
	is_data_modified = TRUE;
	_material = CSteelMatLibrary::Q235BriefMark();//'S';
	m_bReplPart = FALSE;		//代用构件
	m_fInnerRadius = m_fNormOffset = 0.0;
	org_material = CSteelMatLibrary::Q235BriefMark();//'S';
	pSolidBody = NULL;
	strcpy(sPartNo,"");
	face_N = 1;
	m_iLsRayNo=0;
	m_bDisplayHole = false;
	m_bWeldPart = FALSE;
	m_bNeedFillet = FALSE;
	//base_node_handle = base_part_handle=0;
	jdb_style = 0;	//缺省普通连接板
	_cbAlignment=0;
	m_uStatMatCoef = 1;
	//iSeg = 0;
	dwPermission = 0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	neutrosphere = 2;
	LoadDefaultUCS(&ucsView);
	m_iLsArrayStyle = 0;	//钢板上螺栓的阵列方式	0-矩形阵列 非0-环形阵列	wht08-07-31
	*((double*)&HuoQuLine[0].layer[0])=0;
	*((double*)&HuoQuLine[1].layer[0])=0;
	*((double*)&HuoQuLine[0].layer[8])=0.35;
	*((double*)&HuoQuLine[1].layer[8])=0.35;
	crMaterial=GetColor(CLS_PLATE,-1,'S');
	m_bAttachPartByWeldRoad=FALSE;
	_peakVertexOffsetY=0;		//制弯线共线三面板，火曲线交点Y坐标偏移量
	_extendVertexOffsetLen=200;	//制弯线共线三面板，切割位置X长度方向沿神量
	_peakPointMargin=1;			//两制弯点默认间隙制
}

int CLDSPlate::GetEdgeFaceNo(int start_feature,int end_feature)
{
	if(start_feature>=10&&end_feature>=10)
		return 1;
	else if(start_feature<10&&end_feature<10&&start_feature!=end_feature)
		return 0;
	else
	{
		int face_no=min(start_feature,end_feature);
		if(face_no>face_N)
			return 0;
		return face_no;
	}
}

CLDSPlate::~CLDSPlate()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody = NULL;
	}
}

BOOL CLDSPlate::IsInsideAngle(CLDSLineAngle* pAngle, short* x_wing0_y_wing1)
{
	//TODO: 对于火曲板的火曲面法线对于影射构件时如何处理还未考虑太清楚　wjh 2013-1-17
	double ddx,ddy;
	int iFaceI = 1;
	for(CDesignLjPartPara *pLjPart=designInfo.partList.GetFirst();pLjPart;pLjPart=designInfo.partList.GetNext())
	{
		if(pAngle->handle==pLjPart->hPart||pAngle->group_father_jg_h==pLjPart->hPart)
		{
			iFaceI = pLjPart->iFaceNo;
			break;
		}
	}
	f3dPoint view_norm;
	if(iFaceI==1)
		view_norm = ucs.axis_z;
	else if(iFaceI==2)
		view_norm = huoqufacenorm[0];
	else if(iFaceI==3)
		view_norm = huoqufacenorm[1];
	else
		view_norm = ucs.axis_z;
	ddx = pAngle->get_norm_x_wing()*view_norm;
	ddy = pAngle->get_norm_y_wing()*view_norm;
	if(fabs(ddx)>fabs(ddy))
	{
		if(x_wing0_y_wing1)
			*x_wing0_y_wing1 = 0;
		if(ddx>0)	//里铁
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if(x_wing0_y_wing1)
			*x_wing0_y_wing1 = 1;
		if(ddy>0)	//里铁
			return TRUE;
		else
			return FALSE;
	}
}
BOOL CLDSPlate::IsInsideAngle(CLDSLineAngle* pAngle, BYTE* x_wing0_y_wing1)
{
	short siWingStyle=0;
	if(!IsInsideAngle(pAngle,&siWingStyle))
		return FALSE;
	if(x_wing0_y_wing1)
		*x_wing0_y_wing1=(BYTE)siWingStyle;
	return TRUE;
}
CLDSGeneralPlate* CLDSGeneralPlate::CloneCopy(double offset,double* vUdfDirection)
{
	return NULL;
}
CLDSGeneralPlate* CLDSGeneralPlate::ShadowCopy(double offset,double* vUdfDirection)
{
	return NULL;
}
CLDSGeneralPlate* CLDSGeneralPlate::CloneCopy(GEPOINT vOffset,bool localAcsCoord/*=true*/)
{
	return NULL;
}
CLDSGeneralPlate* CLDSGeneralPlate::ShadowCopy(double offset,bool localAcsCoord/*=true*/)
{
	return NULL;
}
bool CLDSPlate::IsBelongTo(int iClassTypeId)const
{
	if(iClassTypeId==this->m_nClassTypeId||iClassTypeId==CLS_PART||iClassTypeId==CLS_DBOBJECT)//||iClassTypeId==CLS_GENERALPLATE)
		return true;
	else
		return false;
}
CLDSPart* CLDSPlate::NewCopy(bool bByShadowMode/*=false*/)
{
	if(bByShadowMode)
		return CLDSPart::NewCopy(true);
	else
	{
		char pool[2048]={0};
		CBuffer buf(pool,2048);
		ToBuffer(buf);
		CLDSPlate* pPlate=(CLDSPlate*)BelongModel()->AppendPart(CLS_PLATE);
		buf.SeekToBegin();
		pPlate->FromBuffer(buf);
		pPlate->relativeObjs.Empty();	//新复制的备份没必要保留关联对象
		return pPlate;
	}
}
CLDSGeneralPlate* CLDSPlate::CloneCopy(double offset,double* vUdfDirection)
{
	CLDSPlate* pPlate=(CLDSPlate*)NewCopy();
	GEPOINT vOffsetDirection=(vUdfDirection==NULL) ? pPlate->ucs.axis_z : GEPOINT(vUdfDirection);
	vOffsetDirection.IsEqual(pPlate->ucs.axis_z.x,pPlate->ucs.axis_z.y,pPlate->ucs.axis_z.z);
	pPlate->ucs.origin+=vOffsetDirection*offset;
	if(!pPlate->designInfo.origin.OffsetPosParam(offset,pPlate->ucs.axis_z))
	{
		pPlate->designInfo.origin.datum_pos_style=0;
		pPlate->designInfo.origin.SetPosition(pPlate->ucs.origin);
	}
	return pPlate;
}
CLDSGeneralPlate* CLDSPlate::ShadowCopy(double offset,double* vUdfDirection)
{
	return NULL;
}
CLDSGeneralPlate* CLDSPlate::CloneCopy(GEPOINT vOffset,bool localAcsCoord/*=true*/)
{
	CLDSPlate* pPlate=(CLDSPlate*)NewCopy();
	return pPlate;
}
CLDSGeneralPlate* CLDSPlate::ShadowCopy(double offset,bool localAcsCoord/*=true*/)
{
	return NULL;
}
bool CLDSPlate::ShadowDesign()
{
	if(!IsShadowObject())	//影像构件
		return false;

	CLDSPlate *pMotherPlate=(CLDSPlate*)MotherObject(false);
	_thick=(int)pMotherPlate->GetThick();
	cMaterial=pMotherPlate->cMaterial;
	m_cQualityLevel=pMotherPlate->m_cQualityLevel;
	
	if(m_iShadowObjType==SHADOW_ISOLATED)
	{
		CParaCS *pAcs=m_pModel->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
		if(pAcs&&pAcs->GetDefStyle()==CParaCS::CS_MIRTRANS)
		{
			CParaCS::PARAMETRIC_INFO param = pAcs->GetParamInfo();
			if(param.m_xMirInfo.axis_flag&0x13)
				m_fNormOffset=-_thick-pMotherPlate->m_fNormOffset;
		}
		else
			m_fNormOffset=pMotherPlate->m_fNormOffset;
	}
	else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
		m_fNormOffset=pMotherPlate->m_fNormOffset;
	//外形信息
	vertex_list.Empty();
	for(PROFILE_VERTEX *pVertex=pMotherPlate->vertex_list.GetFirst();pVertex;pVertex=pMotherPlate->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	jdb_style=pMotherPlate->jdb_style;
	strcpy(sNotes,pMotherPlate->sNotes);
	face_N=pMotherPlate->face_N;
	for(int i=1;i<face_N;i++)
	{
		HuoQuLine[i-1]=pMotherPlate->HuoQuLine[i-1];
		huoqufacenorm[i-1]=pMotherPlate->huoqufacenorm[i-1];
		vector_trans(huoqufacenorm[i-1],pMotherPlate->ucs,FALSE,TRUE);
		vector_trans(huoqufacenorm[i-1],ucs,TRUE,TRUE);
	}
	SetModified();
	return CLDSPart::ShadowDesign();
}

BOOL CLDSPlate::Create3dSolidRollEdge(fBody* pBody,f3dAtomLine *pAtomLine,PROFILE_VERTEX *pFeaVertex,int iFaceNo,COLORREF plank_color)
{
	BOOL bInverseRoll;
	if(pAtomLine->pStart->feature>10&&pAtomLine->pEnd->feature>10)
		return FALSE;
	//顶面正向卷边
	if(pFeaVertex->m_bRollEdge&&pFeaVertex->manu_space>0)
		bInverseRoll=FALSE;	//向外卷边
	else if(pFeaVertex->m_bRollEdge&&pFeaVertex->manu_space<0)
		bInverseRoll=TRUE;	//向内卷边
	else
		return FALSE;
	f3dPoint line_start,line_end;
	f3dPoint face_norm,offset_norm,line_vec=*pAtomLine->pEnd-*pAtomLine->pStart;
	normalize(line_vec);
	if(iFaceNo==0)		//基本面
		face_norm.Set(0,0,1);
	else if(iFaceNo==1)	//第二制弯面
		face_norm=huoqufacenorm[0];
	else if(iFaceNo==2)
		face_norm=huoqufacenorm[1];
	else
		return FALSE;
	if(iFaceNo==1||iFaceNo==2)
		vector_trans(face_norm,ucs,FALSE);
	if(bInverseRoll)
		face_norm*=-1.0;
	offset_norm=face_norm^line_vec;
	normalize(offset_norm);
	f3dPoint *cube_vert_arr[8];
	PROFILE_VERTEX *pEndVertex=NULL,*pPrevVertex=NULL,*pNextVertex=NULL;
	if(pFeaVertex->roll_edge_offset_dist>0||1==m_cRollProcessType)//face_N==1&&
	{
		int i=0,nVertex=vertex_list.GetNodeNum();
		for(i=0;i<nVertex;i++)
		{
			if(pFeaVertex->type==(&vertex_list[i])->type
				&&pFeaVertex->vertex==(&vertex_list[i])->vertex)
			{
				pPrevVertex=&vertex_list[(i-1+nVertex)%nVertex];
				pEndVertex=&vertex_list[(i+1)%nVertex];
				pNextVertex=&vertex_list[(i+2)%nVertex];
				break;
			}
		}
	}
	double start_odd=0,end_odd=0;		//最少刀数绘制实体时卷边实体顶部轮廓点始\终回退量(正值有效)
	f3dLine edge_line,roll_edge_line;
	if(pPrevVertex&&pEndVertex&&pNextVertex)
	{	//生成因卷边偏移量而发生的钢板延伸 wht 11-04-09
		f3dPoint prev_vec,next_vec;
		if(bInverseRoll)
		{	//特征点为pAtomLine始端
			prev_vec=pFeaVertex->vertex-pPrevVertex->vertex;
			next_vec=pEndVertex->vertex-pNextVertex->vertex;
		}
		else 
		{	//特征点为pAtomLine终端
			prev_vec=pEndVertex->vertex-pNextVertex->vertex;
			next_vec=pFeaVertex->vertex-pPrevVertex->vertex;
		}
		normalize(prev_vec);
		normalize(next_vec);
		double cosa=fabs(line_vec*prev_vec);//判断当前边与前一边是否共线
		double cosb=fabs(line_vec*next_vec);//判断当前边与下一边是否共线
		//计算发生偏移后(卷边)边的初始位置
		roll_edge_line.startPt=*(pAtomLine->pStart)+offset_norm*pFeaVertex->roll_edge_offset_dist;
		roll_edge_line.endPt=*(pAtomLine->pEnd)+offset_norm*pFeaVertex->roll_edge_offset_dist;
		if(cosa<EPS_COS2)//存在当前卷边基准线与前后边接近共线的情况，此时如果允许误差太小，会导致偏差过大失真 wjh-2019.11.1
		{	//前一边与当前边求交计算(卷边)边的始端坐标
			edge_line.startPt=*pAtomLine->pStart;
			edge_line.endPt=*pAtomLine->pStart+prev_vec*pFeaVertex->roll_edge_offset_dist*2;
			Int3dll(roll_edge_line,edge_line,roll_edge_line.startPt);
		}
		if(cosb<EPS_COS2)
		{	//后一边与当前边求交计算(卷边)边的终端坐标
			edge_line.startPt=*pAtomLine->pEnd;
			edge_line.endPt=*pAtomLine->pEnd+next_vec*pFeaVertex->roll_edge_offset_dist*2;
			Int3dll(roll_edge_line,edge_line,roll_edge_line.endPt);
		}
		//
		f3dPoint roll_edge_vec=roll_edge_line.endPt-roll_edge_line.startPt;
		if(roll_edge_vec*line_vec<0)
		{	//前一边与后一边相交，直接沿offset_norm向外偏移一个卷边外移量
			roll_edge_line.startPt=*(pAtomLine->pStart)+offset_norm*pFeaVertex->roll_edge_offset_dist;
			roll_edge_line.endPt=*(pAtomLine->pEnd)+offset_norm*pFeaVertex->roll_edge_offset_dist;
		}
		//生成卷边延伸实体
		cube_vert_arr[0]=pAtomLine->pStart;
		cube_vert_arr[1]=pAtomLine->pEnd;
		cube_vert_arr[2]=pBody->vertex.append(roll_edge_line.endPt);
		cube_vert_arr[3]=pBody->vertex.append(roll_edge_line.startPt);
		int ii;
		f3dPolyFace *pRollEdgeTopFace=pBody->faceList.append();
		f3dPolyFace *pRollEdgeBtmFace=pBody->faceList.append();
		for(ii=0;ii<4;ii++)
		{
			line_start=*cube_vert_arr[ii]+face_norm*_thick;
			cube_vert_arr[4+ii]=pBody->vertex.append(line_start);
		}
		pRollEdgeTopFace->material = plank_color;
		pRollEdgeBtmFace->material = plank_color;
		for(ii=0;ii<4;ii++)
		{
			pRollEdgeTopFace->outer_edge.append(cube_vert_arr[ii+4],cube_vert_arr[(ii+1)%4+4]);
			pRollEdgeBtmFace->outer_edge.append(cube_vert_arr[(4-ii)%4],cube_vert_arr[4-ii-1]);
			f3dPolyFace *pRollEdgeSideFace=pBody->faceList.append();
			pRollEdgeSideFace->material = plank_color;
			pRollEdgeSideFace->outer_edge.append(cube_vert_arr[ii],cube_vert_arr[(ii+1)%4]);
			pRollEdgeSideFace->outer_edge.append(cube_vert_arr[(ii+1)%4],cube_vert_arr[(ii+1)%4+4]);
			pRollEdgeSideFace->outer_edge.append(cube_vert_arr[(ii+1)%4+4],cube_vert_arr[ii+4]);
			pRollEdgeSideFace->outer_edge.append(cube_vert_arr[ii+4],cube_vert_arr[ii]);
		}
		//计算最少刀数显示实体的顶部轮廓点回缩量
		if(1==m_cRollProcessType)
		{	//最少刀数
			f3dLine roll_edge_top_line;
			roll_edge_top_line.startPt=roll_edge_line.startPt+offset_norm*abs(pFeaVertex->manu_space);
			roll_edge_top_line.endPt=roll_edge_line.endPt+offset_norm*abs(pFeaVertex->manu_space);
			if(cosa<EPS_COS2)
			{	//前一边与当前边求交计算(卷边)边的始端坐标
				edge_line.startPt=roll_edge_line.startPt;
				edge_line.endPt=edge_line.startPt+prev_vec*abs(pFeaVertex->manu_space)*2;
				Int3dll(roll_edge_top_line,edge_line,line_start);
			}
			if(cosb<EPS_COS2)
			{	//后一边与当前边求交计算(卷边)边的终端坐标
				edge_line.startPt=roll_edge_line.endPt;
				edge_line.endPt=edge_line.startPt+next_vec*abs(pFeaVertex->manu_space)*2;
				Int3dll(roll_edge_top_line,edge_line,line_end);
			}
			//
			roll_edge_vec=line_end-line_start;
			if(roll_edge_vec*line_vec>0)
			{	//计算回退量
				start_odd=DISTANCE(roll_edge_top_line.startPt,line_start);
				end_odd=DISTANCE(roll_edge_top_line.endPt,line_end);
			}
		}
	}
	else
	{
		roll_edge_line.startPt=*(pAtomLine->pStart);
		roll_edge_line.endPt=*(pAtomLine->pEnd);
	}
	if(1==m_cRollProcessType&&(start_odd>0||end_odd>0))
	{	//最少刀数
		f3dPoint *trape_vert_arr[12];
		trape_vert_arr[0]=pBody->vertex.append(roll_edge_line.endPt);
		trape_vert_arr[1]=pBody->vertex.append(roll_edge_line.startPt);
		trape_vert_arr[2]=pBody->vertex.append(roll_edge_line.startPt+face_norm*_thick);
		line_start=roll_edge_line.startPt+face_norm*(abs(pFeaVertex->manu_space)+_thick)+line_vec*start_odd;
		line_end=roll_edge_line.endPt+face_norm*(abs(pFeaVertex->manu_space)+_thick)-line_vec*end_odd;
		trape_vert_arr[3]=pBody->vertex.append(line_start);
		trape_vert_arr[4]=pBody->vertex.append(line_end);
		trape_vert_arr[5]=pBody->vertex.append(roll_edge_line.endPt+face_norm*_thick);
		int ii;
		f3dPolyFace *pRollEdgeTopFace=pBody->faceList.append();
		f3dPolyFace *pRollEdgeBtmFace=pBody->faceList.append();
		for(ii=0;ii<6;ii++)
		{
			line_start=*trape_vert_arr[ii]+offset_norm*_thick;
			trape_vert_arr[6+ii]=pBody->vertex.append(line_start);
		}
		pRollEdgeTopFace->material = pRollEdgeBtmFace->material = plank_color;
		for(ii=0;ii<6;ii++)
		{
			pRollEdgeTopFace->outer_edge.append(trape_vert_arr[ii+6],trape_vert_arr[(ii+1)%6+6]);
			pRollEdgeBtmFace->outer_edge.append(trape_vert_arr[(6-ii)%6],trape_vert_arr[6-ii-1]);
			f3dPolyFace *pRollEdgeSideFace=pRollEdgeSideFace=pBody->faceList.append();
			pRollEdgeSideFace->material = plank_color;
			pRollEdgeSideFace->outer_edge.append(trape_vert_arr[ii],trape_vert_arr[(ii+1)%6]);
			pRollEdgeSideFace->outer_edge.append(trape_vert_arr[(ii+1)%6],trape_vert_arr[(ii+1)%6+6]);
			pRollEdgeSideFace->outer_edge.append(trape_vert_arr[(ii+1)%6+6],trape_vert_arr[ii+6]);
			pRollEdgeSideFace->outer_edge.append(trape_vert_arr[ii+6],trape_vert_arr[ii]);
		}
	}
	else
	{	//最佳受力
		cube_vert_arr[0]=pBody->vertex.append(roll_edge_line.startPt);
		cube_vert_arr[1]=pBody->vertex.append(roll_edge_line.endPt);
		line_start=roll_edge_line.startPt+offset_norm*_thick;
		line_end=roll_edge_line.endPt+offset_norm*_thick;
		cube_vert_arr[2]=pBody->vertex.append(line_end);
		cube_vert_arr[3]=pBody->vertex.append(line_start);
		int ii;
		f3dPolyFace *pRollEdgeTopFace=pBody->faceList.append();
		f3dPolyFace *pRollEdgeBtmFace=pBody->faceList.append();
		for(ii=0;ii<4;ii++)
		{
			line_start=*cube_vert_arr[ii]+face_norm*(abs(pFeaVertex->manu_space)+_thick);
			cube_vert_arr[4+ii]=pBody->vertex.append(line_start);
		}
		pRollEdgeTopFace->material = plank_color;
		pRollEdgeBtmFace->material = plank_color;
		for(ii=0;ii<4;ii++)
		{
			pRollEdgeTopFace->outer_edge.append(cube_vert_arr[ii+4],cube_vert_arr[(ii+1)%4+4]);
			pRollEdgeBtmFace->outer_edge.append(cube_vert_arr[(4-ii)%4],cube_vert_arr[4-ii-1]);
			f3dPolyFace *pRollEdgeSideFace=pRollEdgeSideFace=pBody->faceList.append();
			pRollEdgeSideFace->material = plank_color;
			pRollEdgeSideFace->outer_edge.append(cube_vert_arr[ii],cube_vert_arr[(ii+1)%4]);
			pRollEdgeSideFace->outer_edge.append(cube_vert_arr[(ii+1)%4],cube_vert_arr[(ii+1)%4+4]);
			pRollEdgeSideFace->outer_edge.append(cube_vert_arr[(ii+1)%4+4],cube_vert_arr[ii+4]);
			pRollEdgeSideFace->outer_edge.append(cube_vert_arr[ii+4],cube_vert_arr[ii]);
		}
	}
	return TRUE;
}

GEPOINT CalLocalCS_X_AXIS(const double* axis_z_coords);
BOOL CLDSPlate::Create3dSolidModel(BOOL bDisplayHole,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;
	else
	{
		fBody body;
		fBody* pBody=&body;
		if(pSolidBody!=NULL&& !IsSolidModified())
			return TRUE;
		else
		{
			if(pSolidBody == NULL)
				pSolidBody = new CSolidBody;
			else
				pSolidBody->Empty();
		}
		SetModified(FALSE);
		if(vertex_list.GetNodeNum()<=1)
			return FALSE;
		COLORREF plank_color=CLDSPart::GetColor(CLS_PLATE,-1,cMaterial);	//默认板颜色
		if(m_bUserSpecColor)	//用户指定构件颜色
			plank_color=crMaterial;
		//huoqu_face_separ_i[0],huoqu_face_separ_i[1]仅用在三面板中表示被剪面(第四面或多余面)
		//的起终顶点索引号,qu_vertex_i表示两火曲线与连接板边界线
		//的交点索引号
		int i,n,huoqu_face_separ_i[2],qu_vertex_i[2];
		PROFILE_VERTEX *pVertex;
		f3dPoint pt,tempt,main_norm,norm[2],mid_norm[2],face_pick[3];
		ATOM_LIST<BOLTHOLE_INFO> face_ls[3];
		CHashTable<PROFILE_VERTEX>vertex_tbl;
		vertex_tbl.CreateHashTable(vertex_list.GetNodeNum());
		i=0;
		int prev_face_no=0,now_face_no=0,next_face_no=0;
		bool bNeedInverseHuoQuFace=false;	//三面板是否需要颠倒两火曲面（便于逆时针生成实体面）
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			i++;
			pVertex->vertex.ID = i;
			vertex_tbl.SetValueAt(i,*pVertex);
			if(!bNeedInverseHuoQuFace&&IsPeakPointPlate())
			{
				if(prev_face_no==0)
					prev_face_no=pVertex->vertex.feature;
				else if(now_face_no==0)
					now_face_no=pVertex->vertex.feature;
				else if(next_face_no==0)
					next_face_no=pVertex->vertex.feature;
				else
				{
					prev_face_no=now_face_no;
					now_face_no=next_face_no;
					next_face_no=pVertex->vertex.feature;
				}
				if((prev_face_no==3&&next_face_no==1)||(prev_face_no==1&&next_face_no==2))
				{
					bNeedInverseHuoQuFace=true;
					break;
				}
			}
		}
		main_norm.Set(0,0,1);
		if(face_N<1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("The plane of connection plate(0X%X) is zero or negative,so the data is error,connection plate solid model creates failed!",handle);
#else 
			Log2File()->Log("连接板(0X%X)面数为零或负数,数据错误,连接板实体生成失败!",handle);
#endif
		}
		double angle1=0,angle2=0;
		if(face_N==1)
		{	//单面板时直接将连接板顶点表加进连接板实体模型中
			for(pVertex=vertex_list.GetFirst();
			pVertex!=NULL;pVertex=vertex_list.GetNext())
				pBody->vertex.append(pVertex->vertex);
		}
		else if(face_N<4)	//多面板(双面板或三面板)
		{	//多面板时在将连接板顶点加进连接板实体模型前
			//需要对火曲点处进行变形处理
			//angle表示两个面间的夹角(可能是钝角)而angle1和angle2为锐角
			double angle,offset_dist[2];	
			f3dPoint rotate_axis,vec,perp;
			offset_dist[0] = offset_dist[1] = 0.5;	//变形区偏移半个毫米
			//在连接板顶点表中查找各面的一个拾取点（该面内的任一顶点即可）
			if(IsFoldPlate())	//折叠板
			{
				for(i=0;i<vertex_list.GetNodeNum();i++)
					face_pick[0]+=vertex_list[i].vertex;
				face_pick[0]*=1.0/vertex_list.GetNodeNum();
			}
			else
			{
				for(i=0;i<vertex_list.GetNodeNum();i++)
				{
					if(vertex_list[i].vertex.feature==1)
					{
						face_pick[0]=vertex_list[i].vertex;
						break;
					}
				}
				if(i==vertex_list.GetNodeNum())
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("The first plane's pick point of connection plate(0X%X) is not found,the solid model of connection plate creats failed!",handle);
#else 
					Log2File()->Log("未找到连接板(0X%X)第一面的拾取点,连接板实体创建失败!",handle);
#endif
				}
			}
			for(i=0;i<vertex_list.GetNodeNum();i++)
			{
				if(vertex_list[i].vertex.feature==2)
				{
					face_pick[1]=vertex_list[i].vertex;
					break;
				}
			}
			if(i==vertex_list.GetNodeNum())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("The second plane's pick point of connection plate(0X%X) is not found,the solid model of connection plate creats failed!",handle);
#else 
				Log2File()->Log("未找到连接板(0X%X)第二面的拾取点,连接板实体创建失败!",handle);
#endif
			}
			if (face_N==3)
			{
				for(i=0;i<vertex_list.GetNodeNum();i++)
				{
					if(vertex_list[i].vertex.feature==3)
					{
						face_pick[2]=vertex_list[i].vertex;
						break;
					}
				}
				if(i==vertex_list.GetNodeNum())
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("The third plane's pick point of connection plate(0X%X) is not found,the solid model of connection plate creats failed!",handle);
#else 
					Log2File()->Log("未找到连接板(0X%X)第三面的拾取点,连接板实体创建失败!",handle);
#endif
				}
			}
			//计算连接板其余面与基准面(第一面)间的旋转角度
			for(i=0;i<face_N-1;i++)
			{
				rotate_axis = cross_prod(ucs.axis_z,huoqufacenorm[i]);
				if(i==0)
					angle1 = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
				else
					angle2 = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
				Sub_Pnt(vec,HuoQuLine[i].endPt,
					HuoQuLine[i].startPt);
				vector_trans(rotate_axis,ucs,FALSE);
				if(dot_prod(vec,rotate_axis)<0)
				{
					tempt = HuoQuLine[i].startPt;
					HuoQuLine[i].startPt = 
						HuoQuLine[i].endPt;
					HuoQuLine[i].endPt = tempt;
				}
			}
			//根据火曲线处理旋转轴线
			f3dPoint face_vec[2];
			double dist;
			{	//处理第一根火曲线
				//计算出一根第一面拾取点在第一根火曲线上的投影线
				SnapPerp(&perp,HuoQuLine[0].startPt,
					HuoQuLine[0].endPt,face_pick[0],&dist);
				Sub_Pnt(face_vec[0],face_pick[0],perp);
				//计算第二面上的拾取点的实际坐标（需要将相对坐标旋转）
				rotate_point_around_axis(face_pick[1],angle1,
					HuoQuLine[0].startPt,HuoQuLine[0].endPt);
				//计算出一根第二面拾取点在第一根火曲线上的投影线
				SnapPerp(&perp,HuoQuLine[0].startPt,
					HuoQuLine[0].endPt,face_pick[1],&dist);
				Sub_Pnt(face_vec[1],face_pick[1],perp);
				angle = cal_angle_of_2vec(face_vec[0],face_vec[1]);
				//angle角钢现在无用，以后可能有用所以暂保留WJH--01.10.09
				//if(angle>Pi/2.0)
				//	offset_dist[0]=GetThick()*tan(Pi/2-angle/2)+0.5;
				//replaced the pre-2 line by next line wjh--01.10.09
				if(face_pick[1].z>0)	//向工作面法向内凹,此时需要重新考虑火曲变形区
					offset_dist[0]=GetThick()*tan(fabs(angle1)/2.0)+0.5;
			}
			if(face_N==3)	//三面板的第二根火曲线(旋转轴线)
			{
				//计算出一根第一面拾取点在第二根火曲线上的投影线
				SnapPerp(&perp,HuoQuLine[1].startPt,
					HuoQuLine[1].endPt,face_pick[0],&dist);
				Sub_Pnt(face_vec[0],face_pick[0],perp);
				//计算第三面上的拾取点的实际坐标（需要将相对坐标旋转）
				rotate_point_around_axis(face_pick[2],angle2,
					HuoQuLine[1].startPt,HuoQuLine[1].endPt);
				//计算出一根第三面拾取点在第二根火曲线上的投影线
				SnapPerp(&perp,HuoQuLine[1].startPt,
					HuoQuLine[1].endPt,face_pick[2],&dist);
				Sub_Pnt(face_vec[1],face_pick[2],perp);
				angle = cal_angle_of_2vec(face_vec[0],face_vec[1]);
				//angle角钢现在无用，以后可能有用所以暂保留WJH--01.10.09
				//if(angle>Pi/2.0)
				//	offset_dist[1]=GetThick()*tan(Pi/2-angle/2)+0.5;
				//replaced the pre-2 line by next line wjh--01.10.09
				if(face_pick[2].z>0)	//向工作面法向内凹,此时需要重新考虑火曲变形区
					offset_dist[1]=GetThick()*tan(fabs(angle2)/2.0)+0.5;
			}
			//将多面板其余面上的连接板顶点旋转到实际位置
			for(pVertex=vertex_list.GetFirst();
			pVertex!=NULL;pVertex=vertex_list.GetNext())
			{
				pt = pVertex->vertex;
				if(pt.feature<1)	//必须大于0(不包括0)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("The wrong plane numbers are found in connection plate(0X%X)'s vertices,the solid model of connection plate creats failed!",handle);
#else 
					Log2File()->Log("连接板(0X%X)顶点中发现了错误的面号标识,连接板实体创建失败!",handle);
#endif
				}
				else if(pt.feature==1)	//第一面上的顶点直接插入到实体模型中
					pBody->vertex.append(pt);
				else if(pt.feature==2)
				{					//第二面上的顶点要旋转后才可插入
					rotate_point_around_axis(pt,angle1,HuoQuLine[0]);
					pt.ID=pVertex->vertex.ID;
					//pt.feature = pVertex->vertex.feature;
					//memcpy(pt.layer,pVertex->layer,sizeof(pPoint->layer));
					pBody->vertex.append(pt);
				}
				else if(pt.feature==3)
				{					//第三面上的顶点要旋转后才可插入
					rotate_point_around_axis(pt,angle2,HuoQuLine[1]);
					pt.ID=pVertex->vertex.ID;
					//memcpy(pt.layer,pPoint->layer,sizeof(pPoint->layer));
					pBody->vertex.append(pt);
				}
				//feature大于10的表示火曲线上的点,
				//十位和个位分别表示其所在的两个面
				else if(pt.feature>10)
				{
					int huoqu_i;
					double dd;	//偏移方向判子
					f3dPoint huoqu_vec,direct,pick;
					huoqu_i = __max(pt.feature/10,pt.feature%10)-2;
					if(huoqu_i<0||huoqu_i>1)	//只能是0或1
					{
#ifdef AFX_TARG_ENU_ENGLISH
						Log2File()->Log("The wrong plane numbers are found in connection plate(0X%X)'s vertices,the solid model of connection plate creats failed!",handle);
#else 
						Log2File()->Log("连接板(0X%X)顶点中发现了错误的面号标识,连接板实体创建失败!",handle);
#endif
					}
					Sub_Pnt(huoqu_vec,HuoQuLine[huoqu_i].endPt,
						HuoQuLine[huoqu_i].startPt);
					direct = cross_prod(main_norm,huoqu_vec);
					normalize(direct);
					//前移一位获取前一顶点
					pVertex = vertex_list.GetPrev();
					if(pVertex==NULL)	//第一个顶点就在火曲线上
					{
						pVertex = vertex_list.GetTail();
						pick = pVertex->vertex;
						vertex_list.GetFirst();
					}
					else
					{
						pick = pVertex->vertex;
						vertex_list.GetNext();//	返回原位
					}
					pt.ID*=-1;	//当前节点ID设为负值 wht 2009-08-14
					pick.ID=abs(pt.ID);	//设置插入点的ID
					f3dPoint pt_vec=pt-pick;
					normalize(pt_vec);
					dd = DistOf2dPtLine(pick,HuoQuLine[huoqu_i].startPt,
						HuoQuLine[huoqu_i].endPt);
					if(dd<0)//前一点位于火曲线右侧	偏移板厚模拟真实变形
						Sub_Pnt(pick,pt,offset_dist[huoqu_i]*direct);
					else 
						Add_Pnt(pick,pt,offset_dist[huoqu_i]*direct);
					f3dPoint pick_vec=pick-pt;
					normalize(pick_vec);
					if(pick_vec*pt_vec<0)
						pick.ID*=-1;
					if(pVertex->vertex.feature==2)	//第二面
					{
						rotate_point_around_axis(pick,angle1,HuoQuLine[0]);
					}
					else if(pVertex->vertex.feature==3)	//第三面
					{
						rotate_point_around_axis(pick,angle2,HuoQuLine[1]);
					}
					if(pVertex->vertex.feature>10)
						pick.feature = 1;
					else
						pick.feature = pVertex->vertex.feature;
					pBody->vertex.append(pick);
					pBody->vertex.append(pt);
					//后移一位获取后一顶点
					pVertex = vertex_list.GetNext();
					if(pVertex==NULL)
					{
						pVertex = vertex_list.GetFirst();
						pick = pVertex->vertex;
						vertex_list.GetTail();
					}
					else
					{
						pick = pVertex->vertex;
						vertex_list.GetPrev();//	返回原位
					}
					pick.ID=abs(pt.ID);	//设置插入点的ID
					pt_vec=pick-pt;
					normalize(pt_vec);
					dd = DistOf2dPtLine(pick,HuoQuLine[huoqu_i].startPt,
						HuoQuLine[huoqu_i].endPt);
					if(dd<0)//后一点位于火曲线右侧	偏移0.5毫米模拟真实变形
						Sub_Pnt(pick,pt,offset_dist[huoqu_i]*direct);
					else
						Add_Pnt(pick,pt,offset_dist[huoqu_i]*direct);
					pick_vec=pick-pt;
					normalize(pick_vec);
					if(pick_vec*pt_vec<0)
						pick.ID*=-1;
					if(pVertex->vertex.feature==2)	//第二面
					{
						rotate_point_around_axis(pick,angle1,HuoQuLine[0]);
					}
					else if(pVertex->vertex.feature==3)	//第三面
					{
						rotate_point_around_axis(pick,angle2,HuoQuLine[1]);
					}
					if(pVertex->vertex.feature>10)
						pick.feature = 1;
					else
						pick.feature = pVertex->vertex.feature;
					pBody->vertex.append(pick);
				}
				else
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("The wrong plane numbers are found in connection plate(0X%X)'s vertices,the solid model of connection plate creats failed!",handle);
#else 
					Log2File()->Log("连接板(0X%X)顶点中发现了错误的面号标识,连接板实体创建失败!",handle);
#endif
				}
			}
			f3dPoint axis1,axis2;
			norm[0] = main_norm;
			norm[1] = main_norm;
			Sub_Pnt(axis1,HuoQuLine[0].endPt,HuoQuLine[0].startPt);
			Sub_Pnt(axis2,HuoQuLine[1].endPt,HuoQuLine[1].startPt);
			RotateVectorAroundVector(norm[0],angle1,axis1);
			if(face_N==3)
				RotateVectorAroundVector(norm[1],angle2,axis2);
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("System couldn't creat connection plate which have more than three planes,the solid model of connection plate(0X%X) creats failed!",handle);
#else 
			Log2File()->Log("系统不能生成多于三面的连接板,连接板(0X%X)实体生成失败!",handle);
#endif
		}
		//建立连接板板厚的另一面顶点表
		i=0;
		n = pBody->vertex.GetNodeNum();
		f3dPoint *pnt_arr = new f3dPoint[n];
		for(f3dPoint *pPoint=pBody->vertex.GetFirst();
		pPoint!=NULL;pPoint=pBody->vertex.GetNext())
		{
			//将塔坐标系下的其余面的法线方向转到节点板相对坐标系下
			if(pPoint->feature==1)
				Add_Pnt(pnt_arr[i],*pPoint,main_norm*GetThick());
			else if(pPoint->feature==2)
				Add_Pnt(pnt_arr[i],*pPoint,norm[0]*GetThick());
			else if(pPoint->feature==3)
				Add_Pnt(pnt_arr[i],*pPoint,norm[1]*GetThick());
			else	//feature>10此时点在火曲线上同属于两节点面
			{
				f3dPoint sum_norm;
				int face1,face2;
				face1 = pPoint->feature/10;
				face2 = pPoint->feature%10;
				if(face1>3||face1<1||face2>3||face2<1)
				{
					delete []pnt_arr;
					if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
						Log2File()->Log("Vertex plate(0X%X) has unreasonable plane,data error!",handle);
#else 
						Log2File()->Log("节点板(0X%X)中包含不合理的面,数据错误,已修正到基面上!",handle);
#endif
					pPoint->feature=1;
					return FALSE;
				}
				if(face1==1)
					mid_norm[0] = main_norm;
				else
					mid_norm[0] = norm[face1-2];
				if(face2==1)
					mid_norm[1] = main_norm;
				else
					mid_norm[1] = norm[face2-2];
				//---------VVVVVVV------较难理解,以后要加好注释------
				normalize(mid_norm[0]);
				normalize(mid_norm[1]);
				Add_Pnt(sum_norm,mid_norm[0],mid_norm[1]);
				double angle = cal_angle_of_2vec(sum_norm,mid_norm[0]);
				angle = cal_angle_of_2vec(sum_norm,mid_norm[1]);
				normalize(sum_norm);
				sum_norm = sum_norm*(1.0/cos(angle));
				//---------^^^^^^^------本段较难理解,以后要加好注释------
				
				Add_Pnt(pnt_arr[i],*pPoint,sum_norm*GetThick());
			}
			pnt_arr[i].feature = pPoint->feature;
			pnt_arr[i].ID = pPoint->ID;
			//memcpy(pnt_arr[i].layer,pPoint->layer,sizeof(pPoint->layer));
			i++;
		}
		for(i=0;i<n;i++)
			pBody->vertex.append(pnt_arr[i]);
		delete []pnt_arr;
		//三面除边界点外还存在一个各面的交汇点,此外还存在一个多余面(被剪面)
		if(IsPeakPointPlate())	//非折叠板情况
		{
			//下面便是查找多余面的起终顶点号的过程:其中huoqu_face_separ_i[0]是
			//第二面上的边界点而huoqu_face_separ_i[1]则是第三面的边界点
			huoqu_face_separ_i[0]=huoqu_face_separ_i[1]=0;
			for(i=0;i<n-1;i++)
			{
				if(bNeedInverseHuoQuFace)
				{
					if(pBody->vertex[i].feature==2
						&&pBody->vertex[i+1].feature==3)
					{
						huoqu_face_separ_i[0]=i;
						huoqu_face_separ_i[1]=i+1;
						break;
					}
				}
				else
				{
					if(pBody->vertex[i].feature==3
						&&pBody->vertex[i+1].feature==2)
					{
						huoqu_face_separ_i[0]=i+1;
						huoqu_face_separ_i[1]=i;
						break;
					}
				}
			}
			if(huoqu_face_separ_i[0]==0&&huoqu_face_separ_i[1]==0)
			{
				if(bNeedInverseHuoQuFace)
				{
					if( pBody->vertex[n-1].feature==2
						&&pBody->vertex[0].feature==3)
					{
						huoqu_face_separ_i[0] = n-1;
						huoqu_face_separ_i[1] = 0;
					}
				}
				else
				{
					if( pBody->vertex[n-1].feature==3
						&&pBody->vertex[0].feature==2)
					{
						huoqu_face_separ_i[0] = 0;
						huoqu_face_separ_i[1] = n-1;
					}
				}
			}
			if(huoqu_face_separ_i[0]==0&&huoqu_face_separ_i[1]==0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("Can't find the cut plane of 3 planar plate,the solid model of connection plate(0X%X) creats failed!",handle);
#else 
				Log2File()->Log("没有找到三面连接板的被剪面,连接板(0X%X)实体创建失败!",handle);
#endif
			}
			//计算三面板的各面交汇点
			//-----VVVVVVV------本段更难理解,以后一定要加好注释------
			f3dPoint top_pnt,pick[3],nnorm[3];
			Add_Pnt(pick[0],top_point,main_norm*GetThick());
			Add_Pnt(pick[1],top_point,norm[0]*GetThick());
			Add_Pnt(pick[2],top_point,norm[1]*GetThick());
			int nRetCode=Int3d3f(&top_pnt,pick[0],main_norm,pick[1],norm[0],pick[2],norm[1]);
			if(nRetCode!=1)
				top_pnt=pick[0];
			//-----^^^^^^^------本段更难理解,以后一定要加好注释------
			top_point.z=0;
			pBody->vertex.append(top_point);
			pBody->vertex.append(top_pnt);
		}
		//连接板板厚底面创建
		f3dPolyFace *pFace;
		qu_vertex_i[0] = qu_vertex_i[1] = -1;
		for(i=0;i<n;i++)	//查找火曲顶点索引号
		{
			if(pBody->vertex[i].feature<10)	//不是火曲顶点
				continue;
			int qu_max_i = __max(pBody->vertex[i].feature/10,
				pBody->vertex[i].feature%10);
			if(qu_max_i==2)
				qu_vertex_i[0] = i;
			else if(qu_max_i==3)
				qu_vertex_i[1] = i;
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("The connection plate(0X%X)'s vertices meet wrong plane number!Program error!",handle);
#else 
				Log2File()->Log("连接板(0X%X)顶点中遭遇了非法的面号!程序出错!",handle);
#endif
			}
		}
		if(face_N>1)	//连接板火曲顶点检查
		{
			if(qu_vertex_i[0]<=0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("Can't find the bending vertex of the fist bending line in multiple plane(0X%X),the solid model creats failed!",handle);
#else 
				Log2File()->Log("没有找到多面板(0X%X)第一根火曲线的火曲顶点,实体创建失败!",handle);
#endif
			}
			if(face_N==3&&qu_vertex_i[1]<=0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("Can't find the bending vertex of the second bending line in multiple plane(0X%X),the solid model creats failed!",handle);
#else 
				Log2File()->Log("没有找到多面板(0X%X)第二根火曲线的火曲顶点,实体创建失败!",handle);
#endif
			}
		}
		f3dPolyFace *pUpFace[3],*pDownFace[3];
		if(face_N<3)	//单面板及双面板
		{
			//连接板板厚底面创建
			f3dPolyFace *pFace;
			f3dPoint *pStart,*pEnd;
			int k;
			for(k=0;k<face_N;k++)
			{
				BOOL bStartEdge3Face = FALSE;
				pDownFace[k] = pFace = pBody->faceList.append();
				pFace->material = plank_color;
				//由于对180°半圆形环向板无法根据顶点计算出poly_norm,
				//故在此处设置第一面底面法线方向(生成螺栓孔需要相对方向)
				//在最后需要统一将其转换为绝对方向 wht 11-07-08
				if(k==0)
					pFace->poly_norm=-main_norm;
				for(i=n-1;i>=0;i--)
				{
					f3dAtomLine line;
					line.is_positive_line = FALSE;
					pStart = &pBody->vertex[i];
					if( pStart->feature!=k+1
						&&(pStart->feature%10)!=k+1
						&&(pStart->feature/10)!=k+1)
						continue;
					while(1)
					{
						if(i>0)
						{
							line.is_positive_line = FALSE;
							pEnd = &pBody->vertex[i-1];
							if( pEnd->feature!=k+1
								&&(pEnd->feature%10)!=k+1
								&&(pEnd->feature/10)!=k+1)
							{	//不是当前面顶点,跳过
								i--;
								continue;
							}
							break;
						}
						else
						{
							line.is_positive_line = TRUE;
							if(face_N==2 && pFace->outer_edge.GetNodeNum()>0)	//双面板
							{
								pEnd = pFace->outer_edge[0].pStart;
								break;
							}
							else
							{
								pEnd = &pBody->vertex[n-1];
								break;
							}
						}
					}
					line.pStart = pStart;
					line.pEnd = pEnd;
					pFace->outer_edge.append(line);
					//底面向外卷边
					PROFILE_VERTEX feaVertex;
					if(vertex_tbl.GetValueAt(line.pEnd->ID,feaVertex)&&feaVertex.m_bRollEdge&&feaVertex.manu_space>0)
						Create3dSolidRollEdge(pBody,&line,&feaVertex,k,plank_color);
				}
			}
			//连接板板厚顶面创建
			for(k=0;k<face_N;k++)
			{
				pUpFace[k] = pFace = pBody->faceList.append();
				pFace->material = plank_color;
				//由于对180°半圆形环向板无法根据顶点计算出poly_norm,
				//故在此处设置第一面顶面法线方向(生成螺栓孔需要相对方向)
				//在最后需要统一将其转换为绝对方向 wht 11-07-08
				if(k==0)
					pFace->poly_norm=main_norm;
				BOOL bStartEdge3Face = FALSE;
				for(i=n;i<2*n;i++)
				{
					f3dAtomLine line;
					pStart = &pBody->vertex[i];
					if( pStart->feature!=k+1
						&&(pStart->feature%10)!=k+1
						&&(pStart->feature/10)!=k+1)
						continue;//不是当前面,跳过
					while(1)
					{
						if(i<2*n-1)
						{
							line.is_positive_line = TRUE;
							pEnd = &pBody->vertex[i+1];
							if( pEnd->feature!=k+1
								&&(pEnd->feature%10)!=k+1
								&&(pEnd->feature/10)!=k+1)
							{		//不是当前面,跳过
								i++;
								continue;
							}
							break;
						}
						else
						{
							line.is_positive_line = FALSE;
							if(face_N==2 && pFace->outer_edge.GetNodeNum()>0)	//双面板
							{
								pEnd = pFace->outer_edge[0].pStart;
								break;
							}
							else
							{
								pEnd = &pBody->vertex[n];
								break;
							}
						}
					}
					line.pStart = pStart;
					line.pEnd	 = pEnd;
					pFace->outer_edge.append(line);
					//顶面向内卷边
					PROFILE_VERTEX feaVertex;
					if(vertex_tbl.GetValueAt(line.pStart->ID,feaVertex)&&feaVertex.m_bRollEdge&&feaVertex.manu_space<0)
						Create3dSolidRollEdge(pBody,&line,&feaVertex,k,plank_color);
				}
			}
			//连接板板厚侧面创建
			for(i=0;i<n;i++)
			{
				int k = i+1;
				f3dAtomLine line;
				pFace = pBody->faceList.append();
				pFace->material = plank_color;
				line.pStart = &pBody->vertex[i];
				if(k==n)
				{
					k=0;
					line.is_positive_line = FALSE;
					line.pEnd = &pBody->vertex[k];
				}
				else
				{
					line.is_positive_line = TRUE;
					line.pEnd = &pBody->vertex[i+1];
				}
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k];
				line.pEnd = &pBody->vertex[k+n];
				line.is_positive_line = TRUE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k+n];
				line.pEnd = &pBody->vertex[i+n];
				if(i+n>k+n)
					line.is_positive_line = TRUE;
				else
					line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[i+n];
				line.pEnd   = &pBody->vertex[i];
				line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
			}
		}
		else if(IsPeakPointPlate()) //普通三面板
		{
			//底面的第一面
			pDownFace[0] = pFace = pBody->faceList.append();
			pFace->material = plank_color;
			int huoqu_face_i=1;
			if(bNeedInverseHuoQuFace)	//颠倒两个火曲面生成顺序
				huoqu_face_i=0;
			pBody->NewEdgeLine(pFace,qu_vertex_i[huoqu_face_i]%n,2*n);
			int jj=0;	//累计判子,以防下面的语句陷入死循环
			for(i=qu_vertex_i[huoqu_face_i]-1;i>=qu_vertex_i[huoqu_face_i]-n;i--)
			{
				int ii = i;
				if(i<0)
					i+=n;
				f3dAtomLine* pAtomLine=pBody->NewEdgeLine(pFace,i%n);
				//底面向外卷边
				PROFILE_VERTEX feaVertex;
				if(vertex_tbl.GetValueAt(pAtomLine->pEnd->ID,feaVertex)&&feaVertex.m_bRollEdge&&feaVertex.manu_space>0)
					Create3dSolidRollEdge(pBody,pAtomLine,&feaVertex,0,plank_color);
				jj++;
				if(i==qu_vertex_i[1-huoqu_face_i])
					break;
				if(jj>n)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("Can't find the bending vertex of the fist bending line in all connection plates(0X%X),the solid model creats failed!",handle);
#else 
					Log2File()->Log("在所有连接板(0X%X)顶点中没有找到第一根火曲线的顶点,实体创建失败!",handle);
#endif
					break;
				}
			}
			pBody->NewEdgeLine(pFace,2*n);
			//底面的第二面
			pDownFace[1] = pFace = pBody->faceList.append();
			pFace->material = plank_color;
			if(bNeedInverseHuoQuFace)	//颠倒两个火曲面生成顺序
				huoqu_face_i=1;
			else
				huoqu_face_i=0;
			pBody->NewEdgeLine(pFace,qu_vertex_i[huoqu_face_i]%n,2*n);
			jj=0;
			for(i=qu_vertex_i[huoqu_face_i]-1;i>qu_vertex_i[huoqu_face_i]-n;i--)
			{
				int ii = i;
				if(ii<0)
					ii+=n;
				f3dAtomLine* pAtomLine=pBody->NewEdgeLine(pFace,ii);
				//底面向外卷边
				PROFILE_VERTEX feaVertex;
				if(vertex_tbl.GetValueAt(pAtomLine->pEnd->ID,feaVertex)&&feaVertex.m_bRollEdge&&feaVertex.manu_space>0)
					Create3dSolidRollEdge(pBody,pAtomLine,&feaVertex,1,plank_color);
				jj++;
				if(ii==huoqu_face_separ_i[huoqu_face_i])
					break;
				if(jj>n)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("Can't find the start vertex of cutting plane in all connection plates(0X%X),the solid model creats failed",handle);
#else 
					Log2File()->Log("在所有连接板(0X%X)顶点中没有找到被剪面的起始顶点,实体创建失败",handle);
#endif
				}
			}
			pBody->NewEdgeLine(pFace,2*n);
			//底面的第三面
			pDownFace[2] = pFace = pBody->faceList.append();
			pFace->material = plank_color;
			if(bNeedInverseHuoQuFace)	//颠倒两个火曲面生成顺序
				huoqu_face_i=0;
			else
				huoqu_face_i=1;
			pBody->NewEdgeLine(pFace,huoqu_face_separ_i[huoqu_face_i],2*n);
			jj=0;
			for(i=huoqu_face_separ_i[huoqu_face_i]-1;i>huoqu_face_separ_i[huoqu_face_i]-n;i--)
			{
				int ii = i;
				if(ii<0)
					ii+=n;
				f3dAtomLine* pAtomLine=pBody->NewEdgeLine(pFace,ii);
				//底面向外卷边
				PROFILE_VERTEX feaVertex;
				if(vertex_tbl.GetValueAt(pAtomLine->pEnd->ID,feaVertex)&&feaVertex.m_bRollEdge&&feaVertex.manu_space>0)
					Create3dSolidRollEdge(pBody,pAtomLine,&feaVertex,2,plank_color);
				jj++;
				if(ii==qu_vertex_i[huoqu_face_i])
					break;
				if(jj>n)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("Can't find the bending vertex of the second bending line in all connection plates(0X%X),the solid model creats failed!",handle);
#else 
					Log2File()->Log("在所有连接板(0X%X)顶点中没有找到第二根火曲线的顶点,实体创建失败!",handle);
#endif
				}
			}
			pBody->NewEdgeLine(pFace,2*n);
			//连接板板厚顶面创建
			qu_vertex_i[0] +=n;
			qu_vertex_i[1] +=n;
			//顶面的第一面
			pUpFace[0] = pFace = pBody->faceList.append();
			pFace->material = plank_color;
			if(bNeedInverseHuoQuFace)	//颠倒两个火曲面生成顺序
				huoqu_face_i=1;
			else
				huoqu_face_i=0;
			pBody->NewEdgeLine(pFace,qu_vertex_i[huoqu_face_i],2*n+1);
			jj=0;
			for(i=qu_vertex_i[huoqu_face_i]+1;i<=qu_vertex_i[huoqu_face_i]+n;i++)
			{
				int ii = i;
				if(ii>=2*n)
					ii -=n;
				f3dAtomLine* pAtomLine=pBody->NewEdgeLine(pFace,ii);
				//顶面向内卷边
				PROFILE_VERTEX feaVertex;
				if(vertex_tbl.GetValueAt(pAtomLine->pStart->ID,feaVertex)&&feaVertex.m_bRollEdge&&feaVertex.manu_space<0)
					Create3dSolidRollEdge(pBody,pAtomLine,&feaVertex,0,plank_color);
				jj++;
				if(ii==qu_vertex_i[1-huoqu_face_i])
					break;
				if(jj>n)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("Can't find the bending vertex of the second bending line in all connection plates(0X%X),the solid model creats failed!",handle);
#else 
					Log2File()->Log("在所有连接板(0X%X)顶点中没有找到第二根火曲线的顶点,实体创建失败!",handle);
#endif
				}
			}
			pBody->NewEdgeLine(pFace,2*n+1);
			//顶面的第二面
			pUpFace[1] = pFace = pBody->faceList.append();
			pFace->material = plank_color;
			if(bNeedInverseHuoQuFace)	//颠倒两个火曲面生成顺序
				huoqu_face_i=1;
			else
				huoqu_face_i=0;
			pBody->NewEdgeLine(pFace,huoqu_face_separ_i[huoqu_face_i]+n,2*n+1);
			jj=0;
			for(i=huoqu_face_separ_i[huoqu_face_i]+n+1;i<huoqu_face_separ_i[huoqu_face_i]+2*n;i++)
			{
				int ii = i;
				if(ii>=2*n)
					ii-=n;
				f3dAtomLine* pAtomLine=pBody->NewEdgeLine(pFace,ii);
				//顶面向内卷边
				PROFILE_VERTEX feaVertex;
				if(vertex_tbl.GetValueAt(pAtomLine->pStart->ID,feaVertex)&&feaVertex.m_bRollEdge&&feaVertex.manu_space<0)
					Create3dSolidRollEdge(pBody,pAtomLine,&feaVertex,1,plank_color);
				jj++;
				if(ii==qu_vertex_i[huoqu_face_i])
					break;
				if(jj>n)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("Can't find the bending vertex of the first bending line in all connection plates(0X%X),the solid model creats failed!",handle);
#else 
					Log2File()->Log("在所有连接板(0X%X)顶点中没有找到第一根火曲线的顶点,实体创建失败!",handle);
#endif
				}
			}
			pBody->NewEdgeLine(pFace,2*n+1);
			//顶面的第三面
			pUpFace[2] = pFace = pBody->faceList.append();
			pFace->material = plank_color;
			if(bNeedInverseHuoQuFace)	//颠倒两个火曲面生成顺序
				huoqu_face_i=0;
			else
				huoqu_face_i=1;
			pBody->NewEdgeLine(pFace,qu_vertex_i[huoqu_face_i],2*n+1);
			jj=0;
			for(i=qu_vertex_i[huoqu_face_i]+1;i<qu_vertex_i[huoqu_face_i]+n;i++)
			{
				int ii = i;
				if(ii>=2*n)
					ii =i-n;
				f3dAtomLine* pAtomLine=pBody->NewEdgeLine(pFace,ii);
				//顶面向内卷边
				PROFILE_VERTEX feaVertex;
				if(vertex_tbl.GetValueAt(pAtomLine->pStart->ID,feaVertex)&&feaVertex.m_bRollEdge&&feaVertex.manu_space<0)
					Create3dSolidRollEdge(pBody,pAtomLine,&feaVertex,2,plank_color);
				jj++;
				if(ii==huoqu_face_separ_i[huoqu_face_i]+n)
					break;
				if(jj>n)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("Can't find the end vertex of cutting plane in all connection plates(0X%X,the solid model creats failed!",handle);
#else 
					Log2File()->Log("在所有连接板(0X%X)顶点中没有找到被剪面的终止顶点,实体创建失败!",handle);
#endif
				}
			}
			pBody->NewEdgeLine(pFace,2*n+1);
			
			qu_vertex_i[0]-=n;
			qu_vertex_i[1]-=n;
			if(bNeedInverseHuoQuFace)	//颠倒两个火曲面生成顺序
				huoqu_face_i=1;
			else
				huoqu_face_i=0;
			//连接板板厚侧面创建
			for(i=0;i<n;i++)
			{
				int k = i+1;
				f3dAtomLine line;
				line.pStart = &pBody->vertex[i];
				if(k==n)
				{
					k=0;
					line.is_positive_line = FALSE;
					line.pEnd   = &pBody->vertex[k];
				}
				else
				{
					line.is_positive_line = TRUE;
					line.pEnd   = &pBody->vertex[i+1];
				}
				if(i==huoqu_face_separ_i[1-huoqu_face_i]&&k==huoqu_face_separ_i[huoqu_face_i])	//此面被剪掉了
					continue;
				pFace = pBody->faceList.append();
				pFace->material = plank_color;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k];
				line.pEnd   = &pBody->vertex[k+n];
				line.is_positive_line = TRUE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k+n];
				line.pEnd	 = &pBody->vertex[i+n];
				if(i+n>k+n)
					line.is_positive_line = TRUE;
				else
					line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[i+n];
				line.pEnd   = &pBody->vertex[i];
				line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
			}
			pFace = pBody->faceList.append();
			pFace->material = plank_color;
			pBody->NewEdgeLine(pFace,2*n+1,huoqu_face_separ_i[huoqu_face_i]);
			pBody->NewEdgeLine(pFace,2*n);
			pBody->NewEdgeLine(pFace,huoqu_face_separ_i[huoqu_face_i]);
			pFace = pBody->faceList.append();
			pFace->material = plank_color;
			pBody->NewEdgeLine(pFace,huoqu_face_separ_i[huoqu_face_i]+n,huoqu_face_separ_i[huoqu_face_i]);
			pBody->NewEdgeLine(pFace,2*n+1);
			pBody->NewEdgeLine(pFace,huoqu_face_separ_i[huoqu_face_i]);
			pFace = pBody->faceList.append();
			pFace->material = plank_color;
			pBody->NewEdgeLine(pFace,2*n,huoqu_face_separ_i[1-huoqu_face_i]);
			pBody->NewEdgeLine(pFace,2*n+1);
			pBody->NewEdgeLine(pFace,huoqu_face_separ_i[1-huoqu_face_i]);
			pFace = pBody->faceList.append();
			pFace->material = plank_color;
			pBody->NewEdgeLine(pFace,2*n+1,huoqu_face_separ_i[1-huoqu_face_i]);
			pBody->NewEdgeLine(pFace,huoqu_face_separ_i[1-huoqu_face_i]+n);
			pBody->NewEdgeLine(pFace,huoqu_face_separ_i[1-huoqu_face_i]);
		}
		else	//折叠三面板
		{
			//TODO:未处理折叠三面板的卷边问题　wjh-2016.11.28
			int huoqu_face_separ_i[2],base_start_i;
			for(int k=0;k<3;k++)
			{
				//底面
				int min_face_i,max_face_i,face_i;
				pDownFace[k] = pFace = pBody->faceList.append();
				pFace->material = plank_color;
				huoqu_face_separ_i[0] = huoqu_face_separ_i[1] = base_start_i = -1;
				for(i=n-1;i>=0;i--)
				{
					face_i = pBody->vertex[i].feature;
					if(face_i<10)
						max_face_i = min_face_i = face_i;
					else
					{
						min_face_i = min(face_i/10,face_i%10);
						max_face_i = max(face_i/10,face_i%10);
					}
					if(min_face_i>1&&k==0)	//不是第一面上的顶点
						continue;
					else if(max_face_i!=k+1&&k>0)
						continue;
					else if(huoqu_face_separ_i[0]==-1)
						huoqu_face_separ_i[0] = i;
					else//
					{
						huoqu_face_separ_i[1] = i;
						if(huoqu_face_separ_i[1]>huoqu_face_separ_i[0])
							pBody->NewEdgeLine(pFace,huoqu_face_separ_i[1],huoqu_face_separ_i[0],TRUE);
						else
							pBody->NewEdgeLine(pFace,huoqu_face_separ_i[1],huoqu_face_separ_i[0],FALSE);
						huoqu_face_separ_i[0] = huoqu_face_separ_i[1];
						huoqu_face_separ_i[1] = -1;
					}
					if(base_start_i==-1)
						base_start_i = i;
				}
				pBody->NewEdgeLine(pFace,base_start_i,-1,FALSE);
				
				pUpFace[k] = pFace = pBody->faceList.append();
				pFace->material = plank_color;
				huoqu_face_separ_i[0] = huoqu_face_separ_i[1] = base_start_i = -1;
				for(i=n;i<2*n;i++)
				{
					face_i = pBody->vertex[i].feature;
					if(face_i<10)
						max_face_i = min_face_i = face_i;
					else
					{
						min_face_i = min(face_i/10,face_i%10);
						max_face_i = max(face_i/10,face_i%10);
					}
					if(min_face_i>1&&k==0)
						continue;
					else if(max_face_i!=k+1&&k>0)
						continue;
					else if(huoqu_face_separ_i[0]==-1)
						huoqu_face_separ_i[0] = i;
					else //if(huoqu_face_separ_i[1]==-1)
					{
						huoqu_face_separ_i[1] = i;
						if(huoqu_face_separ_i[1]>huoqu_face_separ_i[0])
							pBody->NewEdgeLine(pFace,huoqu_face_separ_i[1],huoqu_face_separ_i[0],TRUE);
						else
							pBody->NewEdgeLine(pFace,huoqu_face_separ_i[1],huoqu_face_separ_i[0],FALSE);
						huoqu_face_separ_i[0] = huoqu_face_separ_i[1];
						huoqu_face_separ_i[1] = -1;
					}
					if(base_start_i==-1)
						base_start_i = i;
				}
				pBody->NewEdgeLine(pFace,base_start_i,-1,FALSE);
			}
			
			//连接板板厚侧面创建
			for(i=0;i<n;i++)
			{
				int k = i+1;
				f3dAtomLine line;
				line.pStart = &pBody->vertex[i];
				if(k==n)
				{
					k=0;
					line.is_positive_line = FALSE;
					line.pEnd   = &pBody->vertex[k];
				}
				else
				{
					line.is_positive_line = TRUE;
					line.pEnd   = &pBody->vertex[i+1];
				}
				if(i==huoqu_face_separ_i[1]&&k==huoqu_face_separ_i[0])	//此面被剪掉了
					continue;
				pFace = pBody->faceList.append();
				pFace->material = plank_color;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k];
				line.pEnd   = &pBody->vertex[k+n];
				line.is_positive_line = TRUE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[k+n];
				line.pEnd	 = &pBody->vertex[i+n];
				if(i+n>k+n)
					line.is_positive_line = TRUE;
				else
					line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
				line.pStart = &pBody->vertex[i+n];
				line.pEnd   = &pBody->vertex[i];
				line.is_positive_line = FALSE;
				pFace->outer_edge.append(line);
			}
		}
		//记录第一火曲面的展开特征方向
		f3dPoint vSpreadVec=(HuoQuLine[0].endPt-HuoQuLine[0].startPt).normalized();
		vSpreadVec.Set(-vSpreadVec.y,vSpreadVec.x);
		CMaxDouble maxdist;
		int sign=1;
		for(PROFILE_VERTEX* pVerter=vertex_list.GetFirst();pVerter;pVerter=vertex_list.GetNext())
		{
			if(pVerter->vertex.feature!=2)
				continue;
			double fDist=(pVerter->vertex-HuoQuLine[0].startPt)*vSpreadVec;
			if(maxdist.Update(fabs(fDist),pVerter)==fabs(fDist))
				sign=(fDist>0)?1:-1;
		}
		vSpreadVec*=sign;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(m_bDisplayHole || bDisplayHole ||pLsRef->GetLsPtr()->m_bVirtualPart)
			{	//系统要求显示螺栓孔或当前螺栓为虚拟螺栓
				BOLTHOLE_INFO boltHole;
				int face_i = 1;
				boltHole.bolt_d=(*pLsRef)->get_d();	//记录原始匹配螺栓直径
				boltHole.norm = (*pLsRef)->get_norm();
				boltHole.axis_x=(*pLsRef)->ucs.axis_x;
				boltHole.centre = (*pLsRef)->ucs.origin;
				//螺栓孔需要沿钢板法向进行偏移(-m_fNormOffset),避免出现火曲面上的螺栓孔与螺栓实体错位情况 wxc-2019.7.12
				if (fabs(m_fNormOffset) > 0)
					boltHole.centre -= m_fNormOffset * ucs.axis_z;
				f3dPoint ls_pos;
				SpreadLsPos(pLsRef, ls_pos);
				int nFaceNo=GetFaceNoByPos(ls_pos);
				if (nFaceNo > 0)
					face_i = nFaceNo;
				/*if(face_N==2)
				{	//火曲度为0时，根据法向判断螺栓所在面不准确
					f3dPoint ls_pos=boltHole.centre;
					coord_trans(ls_pos,ucs,FALSE);
					ls_pos.z=0;
					if((ls_pos-HuoQuLine[0].startPt)*vSpreadVec>0)
						face_i = 2;
				}
				else if(face_N==3)
				{
					double dd1,dd2,dd3;
					dd1 = fabs(boltHole.norm*ucs.axis_z.normalized());
					dd2 = fabs(boltHole.norm*huoqufacenorm[0].normalized());
					dd3 = fabs(boltHole.norm*huoqufacenorm[1].normalized());
					if( dd1>=dd2&&dd1>=dd3)
						face_i = 1;
					else if(dd2>=dd1&&dd2>=dd3)
						face_i = 2;
					else
						face_i = 3;
					if(fabs(dd2-dd3)<EPS)	//两火曲面法线近似时根据螺栓位置获取面号 wht 17-02-28
						face_i=GetNormFaceI(pLsRef->GetLsPtr())+1;
				}*/
				coord_trans(boltHole.centre,ucs,FALSE);
				vector_trans(boltHole.axis_x,ucs,FALSE);
				vector_trans(boltHole.norm,ucs,FALSE);
				boltHole.hole_d = (*pLsRef)->get_d()+(*pLsRef)->hole_d_increment;
				boltHole.waistLen = pLsRef->waistLen;	//腰圆长度
				if(face_i==1)
					face_ls[0].append(boltHole);
				else if(face_i==2)
					face_ls[1].append(boltHole);
				else
					face_ls[2].append(boltHole);
			}
		}
			
		CHoleMaker hole_maker;
		for(int k=0;k<face_N;k++)
		{
			UCS_STRU hole_ucs;
			int face_i=k;
			if(bNeedInverseHuoQuFace)	//颠倒两个火曲面生成顺序
			{
				if(k==1)
					face_i=2;
				else if(k==2)
					face_i=1;
			}
			for(BOLTHOLE_INFO *pLsCir=face_ls[k].GetFirst();pLsCir;pLsCir=face_ls[k].GetNext())
			{
				hole_ucs.axis_z = pLsCir->norm;
				hole_ucs.axis_x = pLsCir->axis_x;
				hole_ucs.origin = pLsCir->centre;
				if(hole_ucs.axis_x.IsZero())
					hole_ucs.axis_x=btc::CalLocalCS_X_AXIS(hole_ucs.axis_z);
				COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');	//根据螺栓指定得到螺栓孔颜色
				hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace[face_i],pDownFace[face_i],pBody,crLsHole);
			}
		}
		if(m_fInnerRadius>0)
		{
			CHoleMaker hole_maker;
			int slices=CalArcResolution(m_fInnerRadius,2*Pi,scale_of_user2scr,sample_len,smoothness);
			UCS_STRU hole_ucs;
			if(m_tInnerColumnNorm.IsZero())
				hole_ucs.axis_z.Set(0,0,1);
			else
			{
				hole_ucs.axis_z=m_tInnerColumnNorm;
				vector_trans(hole_ucs.axis_z,ucs,FALSE);
			}
			hole_maker.Create(hole_ucs,m_fInnerRadius*2,0,pUpFace[0],pDownFace[0],pBody,plank_color,slices);
		}
		/*if(this->m_pxInnerLoopBuff!=NULL&&this->m_uiInnerLoopBuffSize>0)
		{	//有自定义内环存在
			UINT j,uiLoopN=0,uiLoopVertexesN=0;
			CBuffer buff(m_pxInnerLoopBuff,m_uiInnerLoopBuffSize);
			buff.SeekToBegin();
			buff.ReadInteger(&uiLoopN);
			for(j=0;j<uiLoopN;j++)
			{
				//TODO:交杨志新完善
			}
		}*/
		for(f3dPoint *pPnt=pBody->vertex.GetFirst();pPnt;pPnt=pBody->vertex.GetNext())
		{
			if(!coord_trans(*pPnt,ucs,TRUE))
				Log2File()->Log("0X%X钢板的装配坐标系不是合法的右手螺旋坐标系,实体位置装配失败!",handle);
			*pPnt+=m_fNormOffset*ucs.axis_z;
		}
		PROFILE_VERTEX curVertex,nextVertex;
		for(pFace=pBody->faceList.GetFirst();pFace;pFace=pBody->faceList.GetNext())
		{
			if(!pFace->poly_norm.IsZero())	//在此处将面法线方向转换为绝对方向(目前仅手动设置了第一面的法线方向) wht 11-07-08
				vector_trans(pFace->poly_norm,ucs,TRUE);
			for(f3dAtomLine* pLine=pFace->outer_edge.GetFirst();pLine;pLine=pFace->outer_edge.GetNext())
			{
				f3dPoint *pFeatureNode,*pNextNode;
				int sub_id=abs(pLine->pEnd->ID)-abs(pLine->pStart->ID);
				if(sub_id==0)
					continue;
				int sign=1;
				if(sub_id==1||sub_id==1-vertex_list.GetNodeNum())
				{
					pFeatureNode=pLine->pStart;
					pNextNode=pLine->pEnd;
				}
				else
				{
					pFeatureNode=pLine->pEnd;
					pNextNode=pLine->pStart;
					sign = -1;
				}
				if(!vertex_tbl.GetValueAt(abs(pFeatureNode->ID),curVertex) || !vertex_tbl.GetValueAt(abs(pNextNode->ID),nextVertex))
					continue;
				if (curVertex.vertex.feature>= 10 && nextVertex.vertex.feature>= 10)
					continue;
				if(curVertex.type>=1)
				{
					int iFace=0;
					if(curVertex.vertex.feature<10)
						iFace=curVertex.vertex.feature-1;
					else if (curVertex.vertex.feature>10 && curVertex.vertex.feature<20
						&& nextVertex.vertex.feature == 2)
						iFace=curVertex.vertex.feature%10-1;
					if(curVertex.radius<0)
						sign=-sign;
					double sec_angle=0;
					if(curVertex.type==1)//指定圆弧夹角(自动计算圆弧夹角的情况现在已自动计算出精确值)
					{
						sec_angle=curVertex.sector_angle;
						if(sec_angle<0)
						{
							sign=-sign;
							sec_angle=-sec_angle;
						}
					}
					f3dPoint worknorm;
					if(iFace==0)
						worknorm=sign*ucs.axis_z;
					else if(iFace==1)
						worknorm=sign*huoqufacenorm[0];
					else //if(iFace==2)
						worknorm=sign*huoqufacenorm[1];
					if(curVertex.type<2)
						pLine->CreateArcLine(worknorm,sec_angle);
					else if(curVertex.type==2)
					{	//板相对坐标系下的圆弧拾取点，应该先转换到相应的火曲面上，再转换的绝对坐标下 wxc-2017.3.24
						f3dPoint center_pick=curVertex.center;
						if(iFace==1)
							rotate_point_around_axis(center_pick,angle1,HuoQuLine[0]);
						else if(iFace==2)
							rotate_point_around_axis(center_pick,angle2,HuoQuLine[1]);
						coord_trans(center_pick,ucs,TRUE);
						pLine->CreateArcLine2(worknorm,fabs(curVertex.radius),center_pick);
					}
					else //椭圆弧
					{
						pLine->CreateEllipseArcLine(curVertex.center,curVertex.column_norm,worknorm,fabs(curVertex.radius));
						//椭圆时有可能需要调整始终端点位置
						f3dPoint start=pLine->PositionInAngle(0);
						f3dPoint end = pLine->PositionInAngle(pLine->SectorAngle());
						pLine->pStart->Set(start.x,start.y,start.z);
						pLine->pEnd->Set(end.x,end.y,end.z);
					}
				}
			}
		}
		pSolidBody->ConvertFrom(pBody);
	}
	/*if(face_N==3&&complement_plate_handle>0x20)
	{
		//三面板重新设计完成后同时重新设计三面板补焊钢板 wht 10-01-12
		CLDSPlate *pComplementPlate=NULL;
		pComplementPlate=(CLDSPlate*)BelongModel()->FromPartHandle(complement_plate_handle,CLS_PLATE);
		//if(pComplementPlate)
		//	pComplementPlate->Create3dSolidModel();
	}*/	
	return TRUE;
}

int CLDSPlate::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[200]="";
	CXhString szText(sText,200);
	if(GetPropID("basicInfo")==0)
		strcpy(valueStr,"");
	else if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("designInfo.hNode")==id)
		sprintf(sText,"0X%X",designInfo.m_hBaseNode);
	else if(GetPropID("designInfo.hRod")==id)
		sprintf(sText,"0X%X",designInfo.m_hBasePart);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("cMaterial")==id)	
	{
		char matStr[20];
		if(QuerySteelMatMark(MotherPart()->cMaterial,matStr))
			sprintf(sText,"%s",matStr);
	}
	else if(GetPropID("m_cQualityLevel")==id)
	{
		if(!this->blDefaultQualityLevel&&isalpha(cQualityLevel))
			sprintf(sText,"%C",cQualityLevel);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
#else
			strcpy(sText,"默认");
#endif
	}
	else if(GetPropID("thick")==id)
		sprintf(sText,"%.0f", GetThick()); 
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		sprintf(sText,"%s",(char*)MotherPart()->GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//接腿构件
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bDisplayHole)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bDisplayHole)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_cPickedDisplayMode")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_cPickedDisplayMode==0)
			strcpy(sText,"None");
		else if(m_cPickedDisplayMode==1)
			strcpy(sText,"Actual Profile");
		else if(m_cPickedDisplayMode==2)
			strcpy(sText,"Spread Profile");
#else 
		if(m_cPickedDisplayMode==0)
			strcpy(sText,"0.不显示");
		else if(m_cPickedDisplayMode==1)
			strcpy(sText,"1.轮廓边");
		else if(m_cPickedDisplayMode==2)
			strcpy(sText,"2.轮廓边(展开)");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bVirtualPart)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bVirtualPart)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bAttachPartByWeldRoad")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bAttachPartByWeldRoad)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bAttachPartByWeldRoad)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else 
		if(m_bUserSpecColor)
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	else if(GetPropID("m_fNormOffset")==id)
	{
		sprintf(sText,"%f",m_fNormOffset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.hLinePart")==id)			//基准杆件句柄
		sprintf(sText,"0X%0X",designInfo.origin.des_para.SCALE_NODE.hLinePart);
	else if(GetPropID("des_para.SCALE_NODE.hStartNode")==id)		//始端节点句柄
		sprintf(sText,"0X%0X",designInfo.origin.des_para.SCALE_NODE.hStartNode);
	else if(GetPropID("des_para.SCALE_NODE.hEndNode")==id)			//终端节点句柄
		sprintf(sText,"0X%0X",designInfo.origin.des_para.SCALE_NODE.hEndNode);
	else if(GetPropID("des_para.SCALE_NODE.start_offset_dist")==id)	//始端偏移量
	{
		sprintf(sText,"%f",designInfo.origin.des_para.SCALE_NODE.start_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.end_offset_dist")==id)	//终端偏移量
	{
		sprintf(sText,"%f",designInfo.origin.des_para.SCALE_NODE.end_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.offset_scale")==id)		//偏移比例
	{
		sprintf(sText,"%f",designInfo.origin.des_para.SCALE_NODE.offset_scale);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.offset_dist")==id)	//在等分基础上的偏移值
	{
		sprintf(sText,"%f",designInfo.origin.des_para.SCALE_NODE.offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.x")==id)
	{
		sprintf(sText,"%f",ucs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.y")==id)
	{
		sprintf(sText,"%f",ucs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.z")==id)
	{
		sprintf(sText,"%f",ucs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("plateProfile")==id)
	{
		if(this->IsSplayFoldPlate())
			strcpy(sText,"构架顶梁八字板");
		else if(this->IsTubeFoldPlate())
			strcpy(sText,"构架曲梁折叠板");
		else if(this->wiType==WASHER_PLATE)
			strcpy(sText,"螺栓垫板");
		else if(this->wiType==WRAPPER_PLATE)
			strcpy(sText,"接头包钢板");
		else if(this->wiType==RIB_PLATE)
			strcpy(sText,"加强肋板");
		else //if(this->wiType==CONNECT_PLATE)
			strcpy(sText,"普通连接板");
	}
	else if(GetPropID("m_fInnerRadius")==id)
	{
		sprintf(sText,"%f",m_fInnerRadius);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_tInnerColumnNorm.x")==id)
	{
		sprintf(sText,"%f",m_tInnerColumnNorm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_tInnerColumnNorm.y")==id)
	{
		sprintf(sText,"%f",m_tInnerColumnNorm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_tInnerColumnNorm.z")==id)
	{
		sprintf(sText,"%f",m_tInnerColumnNorm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_tInnerOrigin.x")==id)
	{
		sprintf(sText,"%f",m_tInnerOrigin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_tInnerOrigin.y")==id)
	{
		sprintf(sText,"%f",m_tInnerOrigin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_tInnerOrigin.z")==id)
	{
		sprintf(sText,"%f",m_tInnerOrigin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("iProfileStyle0123")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(designInfo.iProfileStyle0123==0)
			strcpy(sText,"0.Standard Profile");
		else if(designInfo.iProfileStyle0123==1)
			strcpy(sText,"1.Customized Profile");
		else if(designInfo.iProfileStyle0123==2)
			strcpy(sText,"2.Envelope Profile");
		else// if(designInfo.iProfileStyle0123==3)
			strcpy(sText,"3.Just Update Coordinate"); 
#else 
		if(designInfo.iProfileStyle0123==0)
			strcpy(sText,"0.标准外形");
		else if(designInfo.iProfileStyle0123==1)
			strcpy(sText,"1.定制外形");
		else if(designInfo.iProfileStyle0123==2)
			strcpy(sText,"2.包络外形");
		else// if(designInfo.iProfileStyle0123==3)
			strcpy(sText,"3.仅更新坐标"); 
#endif
	}
	else if(GetPropID("m_cRollProcessType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(_cRollProcessType==1||_cRollProcessType==2)
			strcpy(sText,"1.Processing Priority");
		else if(_cRollProcessType==0)
			strcpy(sText,"0.Mechanical Priority");
		else //if (_cRollProcessType == 0xFF)
			strcpy(sText,"2.Default");
#else
		if(_cRollProcessType==1||_cRollProcessType==2)
			strcpy(sText,"1.最少刀数");
		else if(_cRollProcessType==0)
			strcpy(sText,"0.最佳受力");
		else //if (_cRollProcessType == 0xFF)
			strcpy(sText,"2.默认值");
#endif
	}
	else if(GetPropID("m_cRollOffsetType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(_ciRollOffsetType==0)
			strcpy(sText,"0.Outside");
		else if(_ciRollOffsetType==1)
			strcpy(sText,"1.Inside");
		else
			strcpy(sText,"2.Default");
#else
		if(_ciRollOffsetType==0)
			strcpy(sText,"0.外侧");
		else if(_ciRollOffsetType==1)
			strcpy(sText,"1.内侧");
		else
			strcpy(sText,"2.默认值");
#endif
	}
	else if(GetPropID("m_cDeformType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(_ciDeformType==0)
			strcpy(sText,"0.Assembly Plane");
		else if(_ciDeformType==1)
			strcpy(sText,"1.Middle Plane");
		else if(_ciDeformType==2)
			strcpy(sText,"2.Raised Plane");
		else if(_ciDeformType==3)
			strcpy(sText,"3.Sunk Plane");
		else
			strcpy(sText,"4.Default");
#else
		if(_ciDeformType==0)
			strcpy(sText,"0.装配面");
		else if(_ciDeformType==1)
			strcpy(sText,"1.中性层");
		else if(_ciDeformType==2)
			strcpy(sText,"2.凸面");
		else if(_ciDeformType==3)
			strcpy(sText,"3.凹面");
		else
			strcpy(sText,"4.默认值");
#endif
	}
	else if(GetPropID("m_fDeformCoef")==id)
	{
		sprintf(sText,"%f",m_fDeformCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fDeformCoef2")==id)
	{
		sprintf(sText,"%f",m_fDeformCoef2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_siProfileAlgor")==id)
	{
#ifndef AFX_TARG_ENU_ENGLISH
		if(m_siProfileAlgor==1)
			strcpy(sText,"1.改进算法");
		else //if(m_siProfileAlgor==0)
			strcpy(sText,"0.传统算法");
#endif
	}
	else if(GetPropID("face_N")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(face_N==1)
		{
			if(wiType==WASHER_PLATE)
				strcpy(sText,"4. Bolt Gasket");
			else 
				strcpy(sText,"0. Planar Plate");
		}
		else if(face_N==2)
			strcpy(sText,"1. 2 Planar Plate");
		else if(jdb_style==0)
			strcpy(sText,"2. 3 Planar Plate"); 
		else
			strcpy(sText,"3. Folded Plate");
#else 
		if(face_N==1)
		{
			if(wiType==WASHER_PLATE)
				strcpy(sText,"4.螺栓垫板");
			else 
				strcpy(sText,"0.单面板");
		}
		else if(face_N==2)
			strcpy(sText,"1.双面板");
		else if(jdb_style==0)
			strcpy(sText,"2.三面板"); 
		else
			strcpy(sText,"3.折叠板");
#endif
	}
	else if(GetPropID("m_bSelfCorrectBendline")==id)
	{
		if(!designInfo.m_bSelfCorrectBendline)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Line specified");
		else
			strcpy(sText,"1.Intersection");
#else
			strcpy(sText,"0.指定火曲线");
		else
			strcpy(sText,"1.面面求交线");
#endif
	}
	else if(GetPropID("huoqufacenorm[0].norm_style")==id)
		strcpy(sText,designInfo.huoqufacenorm[0].GetNormStyleDes());
	else if(GetPropID("huoqufacenorm[0].vector.x")==id)
	{
		sprintf(sText,"%f",huoqufacenorm[0].x); //designInfo.huoqufacenorm[0].vector.x);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("huoqufacenorm[0].angle")==id)
		sprintf(sText,"%s",(char*)CXhChar50(GetHuoQuAngle(1)*DEGTORAD_COEF));
	else if(GetPropID("huoqufacenorm[1].angle")==id)
		sprintf(sText,"%s",(char*)CXhChar50(GetHuoQuAngle(2)*DEGTORAD_COEF));
	else if(GetPropID("huoqufacenorm[0].vector.y")==id)
	{
		sprintf(sText,"%f",huoqufacenorm[0].y);//designInfo.huoqufacenorm[0].vector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("huoqufacenorm[0].vector.z")==id)
	{
		sprintf(sText,"%f",huoqufacenorm[0].z);//designInfo.huoqufacenorm[0].vector.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("huoqufacenorm[0].hVicePart")==id)
		sprintf(sText,"0X%X",designInfo.huoqufacenorm[0].hVicePart);
	else if(GetPropID("huoqufacenorm[0].norm_wing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(designInfo.huoqufacenorm[0].norm_wing==0)
			strcpy(sText,"X leg normal");
		else if(designInfo.huoqufacenorm[0].norm_wing==1)
			strcpy(sText,"Y leg normal");  
#else 
		if(designInfo.huoqufacenorm[0].norm_wing==0)
			strcpy(sText,"X肢法线");
		else if(designInfo.huoqufacenorm[0].norm_wing==1)
			strcpy(sText,"Y肢法线");  
#endif
	}
	else if(GetPropID("huoqufacenorm[0].direction")==id)
		sprintf(sText,"%s",designInfo.huoqufacenorm[0].GetDirectionDes());
	else if(GetPropID("huoqufacenorm[0].hCrossPart")==id)
		sprintf(sText,"0X%X",designInfo.huoqufacenorm[0].hCrossPart);
	else if(GetPropID("huoqufacenorm[1].norm_style")==id)
		strcpy(sText,designInfo.huoqufacenorm[1].GetNormStyleDes());
	else if(GetPropID("huoqufacenorm[1].vector.x")==id)
	{
		sprintf(sText,"%f",huoqufacenorm[1].x);//designInfo.huoqufacenorm[1].vector.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("huoqufacenorm[1].vector.y")==id)
	{
		sprintf(sText,"%f",huoqufacenorm[1].y);//designInfo.huoqufacenorm[1].vector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("huoqufacenorm[1].vector.z")==id)
	{
		sprintf(sText,"%f",huoqufacenorm[1].z);//designInfo.huoqufacenorm[1].vector.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("huoqufacenorm[1].hVicePart")==id)
		sprintf(sText,"0X%X",designInfo.huoqufacenorm[1].hVicePart);
	else if(GetPropID("huoqufacenorm[1].norm_wing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(designInfo.huoqufacenorm[1].norm_wing==0)
			strcpy(sText,"X leg normal");
		else if(designInfo.huoqufacenorm[1].norm_wing==1)
			strcpy(sText,"Y leg normal");  
#else 
		if(designInfo.huoqufacenorm[1].norm_wing==0)
			strcpy(sText,"X肢法线");
		else if(designInfo.huoqufacenorm[1].norm_wing==1)
			strcpy(sText,"Y肢法线");  
#endif
	}
	else if(GetPropID("huoqufacenorm[1].direction")==id)
		sprintf(sText,"%s",designInfo.huoqufacenorm[1].GetDirectionDes());
	else if(GetPropID("huoqufacenorm[1].hCrossPart")==id)
		sprintf(sText,"0X%X",designInfo.huoqufacenorm[1].hCrossPart);
	else if(GetPropID("m_bWeldPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bWeldPart)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bWeldPart)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("iWeldLineStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(designInfo.iWeldLineStyle==0)
			strcpy(sText,"0.Calculate freely");
		else if(designInfo.iWeldLineStyle==1)
			strcpy(sText,"1.Specified Weld Line");
		else
			strcpy(sText,"2.Extention Aligned");
#else 
		if(designInfo.iWeldLineStyle==0)
			strcpy(sText,"0.自由计算");
		else if(designInfo.iWeldLineStyle==1)
			strcpy(sText,"1.指定焊缝");
		else
			strcpy(sText,"2.延伸对齐");
#endif
	}
	else if(GetPropID("m_bNeedFillet")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bNeedFillet)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bNeedFillet)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("designInfo.m_bEnableFlexibleDesign")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(((CLDSPlate*)MotherPart())->designInfo.m_bEnableFlexibleDesign)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(((CLDSPlate*)MotherPart())->designInfo.m_bEnableFlexibleDesign)
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("designInfo.weld_start_high")==id)
	{
		sprintf(sText,"%f",designInfo.weld_start_high);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("designInfo.weld_end_high")==id)
	{
		sprintf(sText,"%f",designInfo.weld_end_high);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("designInfo.weld_width")==id)
	{
		sprintf(sText,"%f",designInfo.weld_width);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("relativeObjs")==id)
	{
		for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
			szText.Append(CXhChar50("0X%X",pRelaObj->hObj),',');
	}
	else if(GetPropID("shadowInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("m_hMotherObj")==id)
	{
		CLDSPlate *pMotherPlate=(CLDSPlate*)MotherObject(false);
		if(pMotherPlate!=this)
			sprintf(sText,"0X%X",pMotherPlate->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_bSyncShadowPartNo")==id)
	{
		if(m_bSyncShadowPartNo)
			strcpy(sText,"是");
		else 
			strcpy(sText,"否");
	}
	else if(GetPropID("m_hBlockRef")==id)
	{
		CBlockParameter blockPara;
		if (GetBlockParameter(blockPara))
			sprintf(sText,"0X%X",blockPara.m_pBlockRef->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_hBlock")==id)
		strcpy(sText,GetBlockIdName());
	else if(GetPropID("wcs.origin")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("(%.1f,%.1f)",pEditState->m_fLocalWcsOrgX,pEditState->m_fLocalWcsOrgY);
	}
	else if(GetPropID("wcs.origin.x")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("%g",pEditState->m_fLocalWcsOrgX);
	}
	else if(GetPropID("wcs.origin.y")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("%g",pEditState->m_fLocalWcsOrgY);
	}
	else if(GetPropID("wcs.axis_x")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("(%.1f,%.1f)",pEditState->m_fLocalWcsAxisX_x,pEditState->m_fLocalWcsAxisX_y);
	}
	else if(GetPropID("wcs.axis_x.a")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		double alfa=Cal2dLineAng(0,0,pEditState->m_fLocalWcsAxisX_x,pEditState->m_fLocalWcsAxisX_y)*DEGTORAD_COEF;
		szText.Printf("%.2f",alfa);
		SimplifiedNumString(szText);
	}
	else if(GetPropID("wcs.axis_y")==id)
	{
		PLATEEDIT_STATE* pEditState=m_hashDisplayPlateStates.Add(handle);
		szText.Printf("(%.1f,%.1f)",pEditState->m_fLocalWcsAxisY_x,pEditState->m_fLocalWcsAxisY_y);
	}
	else if(GetPropID("peakVertexOffsetY")==id)
	{
		sprintf(sText,"%f",peakVertexOffsetY);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("extendVertexOffsetLen")==id)
	{
		sprintf(sText,"%f",extendVertexOffsetLen);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("peakPointMargin")==id)
	{
		sprintf(sText,"%f",peakPointMargin);
		SimplifiedNumString(sText);
	}
	else 
	{	//获取钢板顶点在当前工作坐标系下的相对坐标 
		int i=0;
		BOOL bPosX=TRUE;
		char sPosX[50]="",sPosY[50]="";
		for(i=0;i<26;i++)
		{
			sprintf(sPosX,"%c.x",'A'+i);
			sprintf(sPosY,"%c.y",'A'+i);
			if(GetPropID(sPosX)==id)
			{
				bPosX=TRUE;
				break;
			}
			else if(GetPropID(sPosY)==id)
			{
				bPosX=FALSE;
				break;
			}
		}
		if(i<26&&i<vertex_list.GetNodeNum())
		{
			UCS_STRU object_ucs,wcs;
			wcs=_console->WCS();
			object_ucs.origin.Set();
			object_ucs.axis_x.Set(1,0,0);
			object_ucs.axis_y.Set(0,0,1);
			object_ucs.axis_z.Set(0,-1,0);
			coord_trans(wcs.origin,object_ucs,FALSE);
			vector_trans(wcs.axis_x,object_ucs,FALSE);
			vector_trans(wcs.axis_y,object_ucs,FALSE);
			vector_trans(wcs.axis_z,object_ucs,FALSE);
			int iVertex=0;
			PROFILE_VERTEX *pVertex=NULL;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(iVertex==i)
					break;
				iVertex++;
			}
			if(pVertex)
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,ucs,TRUE);	
				coord_trans(vertex,wcs,FALSE);
				if(bPosX)
					sprintf(sText,"%f",vertex.x);
				else 
					sprintf(sText,"%f",vertex.y);
				SimplifiedNumString(sText);
			}
		}
	}
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("0X%X Plate's property##d string value out of length",handle,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("钢板0X%X属性#%d字符串超出长度限制",handle,id);
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}

BOOL CLDSPlate::GetObjPropReadOnlyDefaultState(long id)
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}
BOOL CLDSPlate::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)
{
	if (shadow_mode==CLDSObject::SHADOWMODE_COMMON)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("cMaterial")==id)
			return FALSE;
		else if(GetPropID("m_cQualityLevel")==id)
			return FALSE;
		else if(GetPropID("sPartNo")==id)
			return FALSE;
		else if(GetPropID("iSeg")==id)
			return FALSE;
		else if(GetPropID("cfgword")==id)
			return FALSE;
		else if(GetPropID("m_uStatMatCoef")==id)
			return FALSE;
		else if(GetPropID("sNotes")==id)
			return FALSE;
		else if(GetPropID("relativeObjs")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else
			return TRUE;
	}
	else if(shadow_mode==CLDSObject::SHADOWMODE_BLOCK)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(GetPropID("handle")==id)
			return TRUE;
		else if(GetPropID("wcs.origin")==id)
			return TRUE;
		else if(GetPropID("wcs.axis_x")==id)
			return TRUE;
		else if(GetPropID("wcs.axis_y")==id)
			return TRUE;
		else
			return FALSE;
	}
}
BOOL CLDSPlate::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	CLDSPlate *pPlate=NULL, *pFirstPlate = (CLDSPlate*)pFirstObj;
	pFirstPlate->GetPropValueStr(idProp,valueStr);
	for(pPlate=(CLDSPlate*)selectObjs.GetNext();pPlate;pPlate=(CLDSPlate*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pPlate->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}
BOOL CLDSPlate::CalProfileVertex(CDesignLjPartPara *pLinePartPara,BOOL bFirstRayPart/*=FALSE*/,f3dPoint *pWeldLineStart/*=NULL*/,f3dPoint *pWeldLineEnd/*=NULL*/)
{
	f3dLine ber_line;
	PROFILE_VERTEX *pVertex=NULL;
	f3dPoint vertex_up,vertex_down,bolt_up,bolt_down;
	f3dPoint base_pos,ls_pos,ls_offset_vec,vice_vec,wing_vec;
	CLDSPart *pPart=BelongModel()->FromPartHandle(pLinePartPara->hPart);
	if(designInfo.iProfileStyle0123==3)
		return FALSE;	//不设计轮廓外形
	CLDSLineAngle agentAngle;
	if(pPart!=NULL&&pPart->IsArcPart())
		pPart=agentAngle.Fabricate(pPart,ucs.origin);
	if(pPart==NULL||!pPart->IsLinePart())
	{
		char ss[200];
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(ss,"Can't find the link line part 0X%X in flexible design parameters of the plate 0X%X ！Program error！",pLinePartPara->hPart,handle);
#else 
		sprintf(ss,"0X%X板中的柔性设计参数中找不到0X%X连接杆件！程序出错！",handle,pLinePartPara->hPart);
#endif
		return FALSE;
	}
	CSuperSmartPtr<CLDSLinePart> pCurLinePart = pLinePartPara->pCurLinePart=(CLDSLinePart*)pPart;
	if(pLinePartPara->start0_end1!=1)	//始端连接或中间连接
		vice_vec=pCurLinePart->End()-pCurLinePart->Start();
	else								//终端连接
		vice_vec=pCurLinePart->Start()-pCurLinePart->End();
	normalize(vice_vec);
	LSSPACE_STRU LsSpace;
	int head_bolt_d=20,end_bolt_d=20;
	if(pCurLinePart->IsAngle())	//角钢构件一般有连接螺栓
	{	//IsAngle()代替GetClassTypeId()==CLS_LINEANGLE用于支持射线材为组合角钢情况 wjh-2019.11.26
		if(pCurLinePart->GetClassTypeId()==CLS_LINEANGLE&&pCurLinePart->GetLsCount()==0)
			return FALSE;	//射线杆件上无螺栓(组合角钢的虚拟角钢上可能无螺栓）
		ber_line.startPt=pCurLinePart->Start();
		ber_line.endPt=pCurLinePart->End();
		if(pLinePartPara->start0_end1!=1)	//终端节点不是当前设计节点
			base_pos=pLinePartPara->pCurLinePart->Start();
		else
			base_pos=pLinePartPara->pCurLinePart->End();
		double max_ber_dist=0,min_ber_dist=pCurLinePart.LineAnglePointer()->GetWidth();
		double head_len=0,tail_len=0;	//头->尾方向与射线方向一致
		if(pLinePartPara->start0_end1!=1)	//终端节点不是当前设计节点
			head_len=tail_len=-pCurLinePart->startOdd();
		else
			head_len=tail_len=-pCurLinePart->endOdd();
		BOOL bFirstLs=TRUE;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt&&pCurLinePart->FindLsByHandle(pBolt->handle))
			{
				if( pBolt->des_base_pos.datumPoint.datum_pos_style==3&&(
					pCurLinePart->handle!=designInfo.m_hBasePart&&pCurLinePart.LineAnglePointer()->group_father_jg_h!=designInfo.m_hBasePart))
				{	//心线交点时往往是端连接螺栓，只需在基准角钢里考虑钢板包络，其余射线角钢里不需要二次考虑 wjh-2015.11.12
					long hIntersRefAngle = 0;
					if (pCurLinePart->handle == pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1)
						hIntersRefAngle = pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
					else if(pCurLinePart->handle == pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2)
						hIntersRefAngle = pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
					//hIntersRefAngle不一定就是基准角钢，还存在V板上两根射线材相交于基准准线的情况 wjh-2019.6.23
					CLDSLineAngle* pBaseAngle = NULL;	//非基准材射线角钢
					if(hIntersRefAngle==designInfo.m_hBasePart)
						pBaseAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(hIntersRefAngle,CLS_LINEANGLE);
					if(pBaseAngle&&pBaseAngle->IsLsInWing(pBolt))
						continue;	//直接通过几何位置判断是否为直连接螺栓比句柄更准确(尤其考虑到用户误操作至外螺栓包到了基准角钢里情况） wjh-2019.1.2
					//if(pBaseAngle!=NULL&&pBaseAngle->FindLsByHandle(pBolt->handle)!=NULL&&
					//	(pBaseAngle->handle==designInfo.m_hBasePart||pBaseAngle->group_father_jg_h==designInfo.m_hBasePart))
					//	continue;
				}
				//角钢正负头修改后螺栓位置和法线都有可能修改，所以需要再次更新
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				ls_pos=pBolt->ucs.origin;
				if(pLinePartPara->angle.cur_wing_x0_y1==0)
					project_point(ls_pos,base_pos,pCurLinePart.LineAnglePointer()->get_norm_x_wing());
				else
					project_point(ls_pos,base_pos,pCurLinePart.LineAnglePointer()->get_norm_y_wing());
				double dist=0;
				SnapPerp(&ls_pos,ber_line,ls_pos,&dist);
				if(bFirstLs)
					max_ber_dist=min_ber_dist=dist;
				else if(dist>max_ber_dist)
					max_ber_dist=dist;
				else if(dist<min_ber_dist)
					min_ber_dist=dist;
				ls_offset_vec = ls_pos-base_pos;
				if(ls_offset_vec*vice_vec>0)
					dist = ls_offset_vec.mod();//+xDesJgResult.oddment;
				else
					dist = -ls_offset_vec.mod();//+xDesJgResult.oddment;
				if(bFirstLs)
				{
					head_len=tail_len=dist;
					head_bolt_d=end_bolt_d=pBolt->get_d();
				}
				else if(dist>tail_len)
				{
					tail_len=dist;
					end_bolt_d=pBolt->get_d();
				}
				else if(dist<head_len)
				{
					head_len=dist;
					head_bolt_d=pBolt->get_d();
				}
				bFirstLs=FALSE;
			}
		}
		if(!pLinePartPara->angle.bTwoEdge)
			GetLsSpace(LsSpace,end_bolt_d);
		else
			GetLsSpace(LsSpace,max(head_bolt_d,end_bolt_d));
		if(pLinePartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			pLinePartPara->ber_space=LsSpace.EndSpace;
		else if(pLinePartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			pLinePartPara->ber_space=min_ber_dist;
		else if(pLinePartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			pLinePartPara->ber_space=min_ber_dist-pCurLinePart->GetThick();
		//else (pLinePartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)

		if(pLinePartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			pLinePartPara->wing_space=LsSpace.EndSpace;
		else if(pLinePartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			pLinePartPara->wing_space=pCurLinePart->GetWidth()-max_ber_dist;
		else if(pLinePartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			pLinePartPara->wing_space=pCurLinePart->GetWidth()-max_ber_dist-pCurLinePart->GetThick();
		//else (pLinePartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_UDF)

		if(pLinePartPara->angle.cbSpaceFlag.StyleOfEndSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			pLinePartPara->end_space=LsSpace.EndSpace;

		if(bFirstLs)	//射线杆件上无螺栓
			return FALSE;
		if(pLinePartPara->angle.cur_wing_x0_y1==0)
			wing_vec=pCurLinePart.LineAnglePointer()->GetWingVecX();
		else
			wing_vec=pCurLinePart.LineAnglePointer()->GetWingVecY();
		if (designInfo.iProfileStyle0123==0 || designInfo.iProfileStyle0123==1)
		{
			GetLsSpace(LsSpace,end_bolt_d);
			f3dPoint vertice,vv,vertice1;
			if(designInfo.iProfileStyle0123==0)		//定制(标准)连板外形
			{
				vv=base_pos+vice_vec*(tail_len+LsSpace.PlateEdgeSpace);
				vertice=vv+wing_vec*(min_ber_dist-LsSpace.PlateEdgeSpace);
				vertice1=vv+wing_vec*(max_ber_dist+LsSpace.PlateEdgeSpace);
			}
			else 
			{
				if(pLinePartPara->end_space!=0)
					vv=base_pos+vice_vec*(tail_len+pLinePartPara->end_space);
				else
					vv=base_pos+vice_vec*(tail_len+LsSpace.EndSpace);
				vertice=vv+wing_vec*(min_ber_dist-pLinePartPara->ber_space);
				vertice1=vv+wing_vec*(max_ber_dist+pLinePartPara->wing_space);
			}
			coord_trans(vertice,ucs,FALSE);
			vertice=GetSpreadVertex(vertice,pLinePartPara->iFaceNo);
			coord_trans(vertice1,ucs,FALSE);
			vertice1=GetSpreadVertex(vertice1,pLinePartPara->iFaceNo);
			vertice.feature=vertice1.feature=pLinePartPara->iFaceNo;
			vertice.z=vertice1.z=0;
			vertex_up=vertice;
			vertex_down=vertice1;//用于计算焊接线位置
			PROFILE_VERTEX *pTailVertexUp=vertex_list.append(PROFILE_VERTEX(vertice));
			PROFILE_VERTEX *pTailVertexDown=vertex_list.append(PROFILE_VERTEX(vertice1));
			//记录顶点设计信息,用于更新顶点坐标
			pTailVertexUp->m_hRod=pCurLinePart->handle;
			pTailVertexUp->m_cPosDesignType=PROFILE_VERTEX::DT_BER;
			pTailVertexDown->m_hRod=pCurLinePart->handle;
			pTailVertexDown->m_cPosDesignType=PROFILE_VERTEX::DT_WING;
			f3dPoint lineVec=vice_vec;
			if ((ucs.axis_z^lineVec)*wing_vec < 0 )//方向一致
			{
				pTailVertexUp->m_bUpVertex = true;
				pTailVertexDown->m_bUpVertex = false;
			}
			else
			{
				pTailVertexUp->m_bUpVertex = false;
				pTailVertexDown->m_bUpVertex = true;
			}
			vector_trans(lineVec,ucs,FALSE);
			lineVec = GetSpreadVertex(lineVec,pLinePartPara->iFaceNo,false);
			//设计共用板时使用射线方向排序会舍去其中一根基准角钢的合法轮廓点，
			//rayVecX，rayVecY暂不赋值，按默认方式(形心到轮廓点的方向)排序 wht 10-05-17
			if(jdb_style==6||pCurLinePart.LineAnglePointer()->group_father_jg_h>0x20)	
			{	//共用板或组合角钢子角钢 
				pTailVertexUp->rayVecX = pTailVertexDown->rayVecX = 0;
				pTailVertexUp->rayVecY = pTailVertexDown->rayVecY = 0;
			}
			else
			{	//普通节点板 wht 10-06-29
				pTailVertexUp->rayVecX = pTailVertexDown->rayVecX = lineVec.x;
				pTailVertexUp->rayVecY = pTailVertexDown->rayVecY = lineVec.y;
			}
			if(pLinePartPara->angle.bTwoEdge)		
			{	//另一端两轮廓顶点也需要
				GetLsSpace(LsSpace,head_bolt_d);
				if (designInfo.iProfileStyle0123==0)
				{
					vv=base_pos+vice_vec*(head_len-LsSpace.PlateEdgeSpace);
					vertice=vv+wing_vec*(min_ber_dist-LsSpace.PlateEdgeSpace);
					vertice1=vv+wing_vec*(max_ber_dist+LsSpace.PlateEdgeSpace);
				}
				else
				{
					if(pLinePartPara->startSideEndSpace!=0)
						vv=base_pos+vice_vec*(head_len-pLinePartPara->startSideEndSpace);
					else
						vv=base_pos+vice_vec*(head_len-LsSpace.EndSpace);
					vertice=vv+wing_vec*(min_ber_dist-pLinePartPara->startSideBerSpace);//ber_space);
					vertice1=vv+wing_vec*(max_ber_dist+pLinePartPara->startSideWingSpace);//wing_space);
				}
				coord_trans(vertice,ucs,FALSE);
				vertice=GetSpreadVertex(vertice,pLinePartPara->iFaceNo);
				coord_trans(vertice1,ucs,FALSE);
				vertice1=GetSpreadVertex(vertice1,pLinePartPara->iFaceNo);
				vertice.feature	= vertice1.feature = pLinePartPara->iFaceNo;
				vertice.z=vertice1.z = 0;
				PROFILE_VERTEX *pHeadVertexUp=vertex_list.append(PROFILE_VERTEX(vertice));
				PROFILE_VERTEX *pHeadVertexDown=vertex_list.append(PROFILE_VERTEX(vertice1));
				//记录顶点设计信息,用于更新顶点坐标
				pHeadVertexUp->m_cPosDesignType=PROFILE_VERTEX::DT_BER2;
				pHeadVertexUp->m_hRod=pCurLinePart->handle;
				pHeadVertexDown->m_hRod=pCurLinePart->handle;
				pHeadVertexDown->m_cPosDesignType=PROFILE_VERTEX::DT_WING2;
				//两端轮廓 头部两轮廓点与尾部两轮廓点的m_bUpVertex属性相反 
				pHeadVertexUp->m_bUpVertex=pTailVertexDown->m_bUpVertex;
				pHeadVertexDown->m_bUpVertex=pTailVertexUp->m_bUpVertex;
				if(jdb_style==6||pCurLinePart.LineAnglePointer()->group_father_jg_h>0x20
					||pCurLinePart.LineAnglePointer()->handle!=designInfo.m_hBasePart)	//射线角钢两端轮廓应使用默认排序方式 wht 10-07-12	
				{	//共用板或组合角钢子角钢 
					pHeadVertexUp->rayVecX = pHeadVertexDown->rayVecX = 0;	//-lineVec.x;
					pHeadVertexUp->rayVecY = pHeadVertexDown->rayVecY = 0;	//-lineVec.y;
				}
				else
				{	//普通节点板 wht 10-06-29
					pHeadVertexUp->rayVecX = pHeadVertexDown->rayVecX = -lineVec.x;
					pHeadVertexUp->rayVecY = pHeadVertexDown->rayVecY = -lineVec.y;
				}
			}
		}
		bolt_up=base_pos+vice_vec*tail_len+wing_vec*min_ber_dist;
		bolt_down=base_pos+vice_vec*tail_len+wing_vec*max_ber_dist;
		coord_trans(vertex_up,ucs,TRUE);
		coord_trans(vertex_down,ucs,TRUE);
		//根据螺栓计算正负头
		if(pLinePartPara->start0_end1==0&&pCurLinePart->desStartOdd.m_iOddCalStyle==1)
			pCurLinePart->CalStartOddment();
		else if(pLinePartPara->start0_end1==1&&pCurLinePart->desEndOdd.m_iOddCalStyle==1)
			pCurLinePart->CalEndOddment();
	}
	else if(pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		if(pCurLinePart->handle==designInfo.m_hBasePart)
			return FALSE;	//基准杆件不进行设计
		CSuperSmartPtr<CLDSPart> pPart;
		if(pLinePartPara->start0_end1==0)	//始端连接
			pPart=BelongModel()->FromPartHandle(pCurLinePart.LineTubePointer()->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		else if(pLinePartPara->start0_end1==1)	//终端连接
			pPart=BelongModel()->FromPartHandle(pCurLinePart.LineTubePointer()->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		double L,H;
		if(pPart.IsNULL())
			return FALSE;	//无端板不进行设计
		else if(pPart.ParamPlatePointer()->m_iParamType!=TYPE_UEND&&
			pPart.ParamPlatePointer()->m_iParamType!=TYPE_ROLLEND&&
			pPart.ParamPlatePointer()->m_iParamType!=TYPE_XEND)
			return FALSE;	//不是U型插板、槽型插板或十字插板不进行设计
		else
		{
			if(pPart.ParamPlatePointer()->m_bStdPart)
			{	//标准构件
				CLDSStdPartModel stdPartModel;
				if(!pPart.ParamPlatePointer()->GetStdPartModel(&stdPartModel))
					return 0;
				L=stdPartModel.param.insert_plate_param.L;
				H=stdPartModel.param.insert_plate_param.H;
			}
			else
			{	//非标准构件
				pPart.ParamPlatePointer()->GetDesignItemValue('L',&L);
				pPart.ParamPlatePointer()->GetDesignItemValue('H',&H);
			}
		}
		double minx=1000000,maxx=0,miny=0.5*H,maxy=-0.5*H;
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt&&pPart.ParamPlatePointer()->FindLsByHandle(pBolt->handle))
			{	//钢管正负头修改后螺栓位置和法线方向都有可能变化
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				f3dPoint pos = (*pLsRef)->ucs.origin;
				coord_trans(pos,pPart->ucs,FALSE);
				if(pos.x<minx)
				{
					head_bolt_d=pBolt->get_d();
					minx=pos.x;
				}
				if(pos.x>maxx)
				{
					end_bolt_d=pBolt->get_d();
					maxx=pos.x;
				}
				if(pos.y<miny)
					miny=pos.y;
				if(pos.y>maxy)
					maxy=pos.y;
			}
		}
		if(minx>maxx)
			minx=maxx;
		GetLsSpace(LsSpace,head_bolt_d);
		vertex_up.x=vertex_down.x=minx-LsSpace.EndSpace;
		if(designInfo.iProfileStyle0123==1)		//定制外形
		{
			vertex_up.y=0.5*H;
			//ber_space中存储插板边距值 wht 17-02-22
			if(pLinePartPara->ber_space>0)
				vertex_up.y+=pLinePartPara->ber_space;
		}
		else 
			vertex_up.y=0.5*H;
		vertex_down.y=-vertex_up.y;
		bolt_up.x=bolt_down.x=minx;
		bolt_up.y=maxy;
		bolt_down.y=miny;
		coord_trans(vertex_up,pPart->ucs,TRUE);
		coord_trans(vertex_down,pPart->ucs,TRUE);
		coord_trans(bolt_up,pPart->ucs,TRUE);
		coord_trans(bolt_down,pPart->ucs,TRUE);
		f3dPoint lineVec; 
		if(pLinePartPara->start0_end1==0)
			lineVec = pCurLinePart->End()-pCurLinePart->Start();
		else if (pLinePartPara->start0_end1==1)
			lineVec = pCurLinePart->Start()-pCurLinePart->End();
		//端板上的顶点
		f3dPoint v_u=vertex_up;
		f3dPoint v_d=vertex_down;
		coord_trans(v_u,ucs,FALSE);
		v_u=GetSpreadVertex(v_u,pLinePartPara->iFaceNo);
		coord_trans(v_d,ucs,FALSE);
		v_d=GetSpreadVertex(v_d,pLinePartPara->iFaceNo);
		v_u.z = v_d.z = 0;
		v_u.feature = v_d.feature = pLinePartPara->iFaceNo;
		PROFILE_VERTEX *pUpVertex=vertex_list.append(PROFILE_VERTEX(v_u));
		PROFILE_VERTEX *pDownVertex=vertex_list.append(PROFILE_VERTEX(v_d));
		//记录顶点设计信息,用于更新顶点坐标
		pUpVertex->m_hRod=pCurLinePart->handle;
		pUpVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER;
		pDownVertex->m_hRod=pCurLinePart->handle;
		pDownVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING;
		if ((ucs.axis_z^lineVec)*pPart->ucs.axis_y > 0)
		{	//方向一致
			pUpVertex->m_bUpVertex = true;
			pDownVertex->m_bUpVertex = false;
		}
		else
		{
			pUpVertex->m_bUpVertex = false;
			pDownVertex->m_bUpVertex = true;
		}
		pUpVertex->m_bCanCut = pDownVertex->m_bCanCut = false;
		vector_trans(lineVec,ucs,FALSE);
		pUpVertex->rayVecX = pDownVertex->rayVecX = lineVec.x;
		pUpVertex->rayVecY = pDownVertex->rayVecY = lineVec.y;
	}
	else if(pCurLinePart->GetClassTypeId()==CLS_LINEFLAT||pCurLinePart->GetClassTypeId()==CLS_LINESLOT)
	{
		if(pCurLinePart->GetLsCount()==0)
			return FALSE;	//射线杆件上无螺栓
		wing_vec=pCurLinePart->GetClassTypeId()==CLS_LINEFLAT?pCurLinePart->ucs.axis_y:pCurLinePart->ucs.axis_x;
		ber_line.startPt=pCurLinePart->Start();
		ber_line.endPt=pCurLinePart->End();
		if(pLinePartPara->start0_end1!=1)	//终端节点不是当前设计节点
			base_pos=pCurLinePart->Start();
		else
			base_pos=pCurLinePart->End();
		double ber_dist=pCurLinePart->GetWidth();
		double head_len=0,tail_len=0;	//头->尾方向与射线方向一致
		if(pLinePartPara->start0_end1!=1)	//终端节点不是当前设计节点
			head_len=tail_len=-pCurLinePart->startOdd();
		else
			head_len=tail_len=-pCurLinePart->endOdd();
		BOOL bFirstLs=TRUE;
		double max_ber_dist=0,min_ber_dist=pCurLinePart->GetWidth();
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt&&pCurLinePart->FindLsByHandle(pBolt->handle))
			{
				//角钢正负头修改后螺栓位置和法线都有可能修改，所以需要再次更新
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				ls_pos=pBolt->ucs.origin;
				project_point(ls_pos,base_pos,pCurLinePart->WorkPlaneNorm());
				double dist=0;
				SnapPerp(&ls_pos,ber_line,ls_pos,&ber_dist);
				ls_offset_vec = ls_pos-base_pos;
				if(ls_offset_vec*vice_vec>0)
					dist = ls_offset_vec.mod();//+xDesJgResult.oddment;
				else
					dist = -ls_offset_vec.mod();//+xDesJgResult.oddment;
				if(bFirstLs)
					max_ber_dist=min_ber_dist=dist;
				else if(dist>max_ber_dist)
					max_ber_dist=dist;
				else if(dist<min_ber_dist)
					min_ber_dist=dist;
				if(bFirstLs)
				{
					head_len=tail_len=dist;
					head_bolt_d=end_bolt_d=pBolt->get_d();
				}
				else if(dist>tail_len)
				{
					tail_len=dist;
					end_bolt_d=pBolt->get_d();
				}
				else if(dist<head_len)
				{
					head_len=dist;
					head_bolt_d=pBolt->get_d();
				}
				bFirstLs=FALSE;
			}
		}
		if(bFirstLs)	//射线杆件上无螺栓
			return FALSE;
		if (designInfo.iProfileStyle0123==0 || designInfo.iProfileStyle0123==1)
		{
			GetLsSpace(LsSpace,end_bolt_d);
			if(pLinePartPara->ber_space==0)
				pLinePartPara->ber_space=LsSpace.EndSpace;
			if(pLinePartPara->wing_space==0)
				pLinePartPara->wing_space=LsSpace.EndSpace;
			f3dPoint vertice,vv,vertice1;
			if(designInfo.iProfileStyle0123==0)		//定制(标准)连板外形
			{	//Y轴方向表示扁铁的展开方向(wing_vec)，楞线侧顶点用Y-表示，肢翼侧顶点用Y+表示
				vv=base_pos+vice_vec*(tail_len+LsSpace.PlateEdgeSpace);
				vertice=vv-wing_vec*(ber_dist+LsSpace.PlateEdgeSpace);	//Y-
				vertice1=vv+wing_vec*(ber_dist+LsSpace.PlateEdgeSpace);	//Y+
			}
			else 
			{
				if(pLinePartPara->end_space!=0)
					vv=base_pos+vice_vec*(tail_len+pLinePartPara->end_space);
				else
					vv=base_pos+vice_vec*(tail_len+LsSpace.EndSpace);
				vertice=vv-wing_vec*(ber_dist+pLinePartPara->ber_space);	//Y-
				vertice1=vv+wing_vec*(ber_dist+pLinePartPara->ber_space);	//Y+
			}
			coord_trans(vertice,ucs,FALSE);
			vertice=GetSpreadVertex(vertice,pLinePartPara->iFaceNo);
			coord_trans(vertice1,ucs,FALSE);
			vertice1=GetSpreadVertex(vertice1,pLinePartPara->iFaceNo);
			vertice.feature=vertice1.feature=pLinePartPara->iFaceNo;
			vertice.z=vertice1.z=0;
			vertex_up=vertice;
			vertex_down=vertice1;//用于计算焊接线位置
			PROFILE_VERTEX *pTailVertexUp=vertex_list.append(PROFILE_VERTEX(vertice));
			PROFILE_VERTEX *pTailVertexDown=vertex_list.append(PROFILE_VERTEX(vertice1));
			//记录顶点设计信息,用于更新顶点坐标
			pTailVertexUp->m_hRod=pCurLinePart->handle;
			pTailVertexUp->m_cPosDesignType=PROFILE_VERTEX::DT_BER;
			pTailVertexDown->m_hRod=pCurLinePart->handle;
			pTailVertexDown->m_cPosDesignType=PROFILE_VERTEX::DT_WING;
			f3dPoint lineVec=vice_vec;
			if ((ucs.axis_z^lineVec)*wing_vec > 0 )//方向一致
			{
				pTailVertexUp->m_bUpVertex = true;
				pTailVertexDown->m_bUpVertex = false;
			}
			else
			{
				pTailVertexUp->m_bUpVertex = false;
				pTailVertexDown->m_bUpVertex = true;
			}
			vector_trans(lineVec,ucs,FALSE);
			lineVec = GetSpreadVertex(lineVec,pLinePartPara->iFaceNo,false);
			pTailVertexUp->rayVecX = pTailVertexDown->rayVecX = lineVec.x;
			pTailVertexUp->rayVecY = pTailVertexDown->rayVecY = lineVec.y;
		}
		bolt_up=base_pos+vice_vec*tail_len+wing_vec*ber_dist;
		bolt_down=base_pos+vice_vec*tail_len-wing_vec*ber_dist;
		coord_trans(vertex_up,ucs,TRUE);
		coord_trans(vertex_down,ucs,TRUE);
		//根据螺栓计算正负头
		if(pLinePartPara->start0_end1==0&&pCurLinePart->desStartOdd.m_iOddCalStyle==1)
			pCurLinePart->CalStartOddment();
		else if(pLinePartPara->start0_end1==1&&pCurLinePart->desEndOdd.m_iOddCalStyle==1)
			pCurLinePart->CalEndOddment();
	}
	if(pWeldLineStart&&pWeldLineEnd&&designInfo.iWeldLineStyle!=1)
	{	//钢管为安装定位基准杆件时需要焊接	//由系统自动计算焊接区域外形顶点
		CalWeldLineVertex(pLinePartPara,vertex_up,vertex_down,	bolt_up,bolt_down,*pWeldLineStart,*pWeldLineEnd,bFirstRayPart);
	}
	return TRUE;
}

//根据杆件按照定制外形计算钢板外形顶点坐标
BOOL CLDSPlate::CalProfileVertexPos(CDesignLjPartPara *pLinePartPara,ARRAY_LIST<f3dPoint>& vertexArr,f3dPoint *pBlotUp/* =NULL */,f3dPoint *pBoltDown/* =NULL */)
{
	CLDSLinePart *pLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pLinePartPara->hPart);
	if(pLinePart==NULL || !pLinePart->IsLinePart())
		return FALSE;
	CSuperSmartPtr<CLDSLinePart> pCurLinePart=pLinePart;
	f3dPoint base_pos,vice_vec;
	if(pLinePartPara->start0_end1!=1)	//始端连接或中间连接
	{
		base_pos=pCurLinePart->Start();
		vice_vec=pCurLinePart->End()-pCurLinePart->Start();
	}
	else								//终端连接
	{	
		base_pos=pCurLinePart->End();
		vice_vec=pCurLinePart->Start()-pCurLinePart->End();
	}
	normalize(vice_vec);
	//依赖杆件是角钢或扁铁
	if( pCurLinePart->GetClassTypeId()==CLS_LINEANGLE || pCurLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE || 
		pCurLinePart->GetClassTypeId()==CLS_LINEFLAT || pCurLinePart->GetClassTypeId()==CLS_LINESLOT)
	{
		if(pCurLinePart->GetLsCount()==0)
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("0X%X plate's 0X%X connect rod lacks bolt hole,can't calculate vertex coodinate of relative outline.",handle,pCurLinePart->handle);
#else 
				Log2File()->Log("0X%X钢板的0X%X连接杆件上缺少螺栓孔,无法计算相关轮廓顶点坐标",handle,pCurLinePart->handle);
#endif
			return FALSE;	//射线杆件上无螺栓
		}
		f3dPoint wing_vec,norm,ls_pos,ls_offset_vec;
		f3dLine ber_line;
		if(pCurLinePart->IsAngle())
		{
			ber_line.startPt=pCurLinePart->Start();
			ber_line.endPt=pCurLinePart->End();
			if(pLinePartPara->angle.cur_wing_x0_y1==0)
			{
				wing_vec=pCurLinePart.LineAnglePointer()->GetWingVecX();
				norm=pCurLinePart.LineAnglePointer()->get_norm_x_wing();
			}
			else
			{
				wing_vec=pCurLinePart.LineAnglePointer()->GetWingVecY();
				norm=pCurLinePart.LineAnglePointer()->get_norm_y_wing();
			}
		}
		else
		{
			wing_vec=pCurLinePart->ucs.axis_y;
			norm=pCurLinePart->WorkPlaneNorm();
			base_pos-=wing_vec*0.5*pCurLinePart->GetWidth();
			ber_line.startPt-=wing_vec*0.5*pCurLinePart->GetWidth();
			ber_line.endPt-=wing_vec*0.5*pCurLinePart->GetWidth();
		}
		double max_ber_dist=0,min_ber_dist=pCurLinePart->GetWidth();
		double head_len=0,tail_len=0;
		if(pLinePartPara->start0_end1!=1)	//终端节点不是当前设计节点
			head_len=tail_len=-pCurLinePart->startOdd();
		else
			head_len=tail_len=-pCurLinePart->endOdd();
		long head_bolt_d=20,end_bolt_d=20;
		BOOL bFirstLs=TRUE;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(pBolt&&pCurLinePart->FindLsByHandle(pBolt->handle))
			{	//角钢正负头修改后螺栓位置和法线都有可能修改，所以需要再次更新
				double dist=0;
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				ls_pos=pBolt->ucs.origin;
				project_point(ls_pos,base_pos,norm);
				SnapPerp(&ls_pos,ber_line,ls_pos,&dist);
				if(bFirstLs)
					max_ber_dist=min_ber_dist=dist;
				else if(dist>max_ber_dist)
					max_ber_dist=dist;
				else if(dist<min_ber_dist)
					min_ber_dist=dist;
				ls_offset_vec = ls_pos-base_pos;
				if(ls_offset_vec*vice_vec>0)
					dist = ls_offset_vec.mod();
				else
					dist = -ls_offset_vec.mod();
				if(bFirstLs)
				{
					head_len=tail_len=dist;
					head_bolt_d=end_bolt_d=pBolt->get_d();
				}
				else if(dist>tail_len)
				{
					tail_len=dist;
					end_bolt_d=pBolt->get_d();
				}
				else if(dist<head_len)
				{
					head_len=dist;
					head_bolt_d=pBolt->get_d();
				}
				bFirstLs=FALSE;
			}
		}
		LSSPACE_STRU LsSpace;
		if(!pLinePartPara->angle.bTwoEdge)
			GetLsSpace(LsSpace,end_bolt_d);
		else
			GetLsSpace(LsSpace,max(head_bolt_d,end_bolt_d));
		if(pLinePartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			pLinePartPara->ber_space=LsSpace.EndSpace;
		else if(pLinePartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			pLinePartPara->ber_space=min_ber_dist;
		else if(pLinePartPara->angle.cbSpaceFlag.StyleOfBerSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			pLinePartPara->ber_space=min_ber_dist-pCurLinePart->GetThick();
		if(pLinePartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			pLinePartPara->wing_space=LsSpace.EndSpace;
		else if(pLinePartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOEDGE)
			pLinePartPara->wing_space=pCurLinePart->GetWidth()-max_ber_dist;
		else if(pLinePartPara->angle.cbSpaceFlag.StyleOfWingSpace()==ANGLE_SPACE_FLAG::SPACE_TOBER)
			pLinePartPara->wing_space=pCurLinePart->GetWidth()-max_ber_dist-pCurLinePart->GetThick();
		if(pLinePartPara->angle.cbSpaceFlag.StyleOfEndSpace()==ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			pLinePartPara->end_space=LsSpace.EndSpace;
		if(bFirstLs)	//射线杆件上无螺栓
			return FALSE;
		double wing_dist=0,ber_dist=0,tail_edge_dist=0,head_edge_dist=0;
		//更新外形顶点坐标统一按照定制外形进行计算
		double startSideEndSpace=(pLinePartPara->startSideEndSpace!=0)?pLinePartPara->startSideEndSpace:LsSpace.EndSpace;	//pLinePartPara->end_space==0时表示标准端距 wht 16-09-30
		double endSideEndSpace=(pLinePartPara->endSideEndSpace!=0)?pLinePartPara->endSideEndSpace:LsSpace.EndSpace;	//pLinePartPara->end_space==0时表示标准端距 wht 16-09-30
		head_edge_dist=head_len-startSideEndSpace;//end_space;
		tail_edge_dist=tail_len+  endSideEndSpace;//end_space;
		ber_dist=min_ber_dist-pLinePartPara->ber_space;
		wing_dist=max_ber_dist+pLinePartPara->wing_space;
		//计算顶点坐标
		f3dPoint vv,ber_vertex,wing_vertex;
		vv=base_pos+vice_vec*tail_edge_dist;
		ber_vertex=vv+wing_vec*ber_dist;
		coord_trans(ber_vertex,ucs,FALSE);
		ber_vertex=GetSpreadVertex(ber_vertex,pLinePartPara->iFaceNo);
		ber_vertex.z=0;
		vertexArr.append(ber_vertex);
		wing_vertex=vv+wing_vec*wing_dist;
		coord_trans(wing_vertex,ucs,FALSE);
		wing_vertex=GetSpreadVertex(wing_vertex,pLinePartPara->iFaceNo);
		wing_vertex.z=0;
		vertexArr.append(wing_vertex);
		if(pLinePartPara->angle.bTwoEdge)
		{	//两边轮廓时，启用始端边距属性变量
			ber_dist=min_ber_dist-pLinePartPara->startSideBerSpace;
			wing_dist=max_ber_dist+pLinePartPara->startSideWingSpace;
			//
			vv=base_pos+vice_vec*head_edge_dist;
			ber_vertex=vv+wing_vec*ber_dist;
			coord_trans(ber_vertex,ucs,FALSE);
			ber_vertex=GetSpreadVertex(ber_vertex,pLinePartPara->iFaceNo);
			ber_vertex.z=0;
			vertexArr.append(ber_vertex);
			wing_vertex=vv+wing_vec*wing_dist;
			coord_trans(wing_vertex,ucs,FALSE);
			wing_vertex=GetSpreadVertex(wing_vertex,pLinePartPara->iFaceNo);
			wing_vertex.z=0;
			vertexArr.append(wing_vertex);
		}
		//计算螺栓
		if(pBlotUp&&pBoltDown)
		{
			f3dPoint bolt_up,bolt_down;
			bolt_up=base_pos+vice_vec*tail_len+wing_vec*min_ber_dist;
			bolt_down=base_pos+vice_vec*tail_len+wing_vec*max_ber_dist;
			coord_trans(bolt_up,ucs,TRUE);
			coord_trans(bolt_down,ucs,TRUE);
			pBlotUp=&bolt_up;
			pBoltDown=&bolt_down;
		}
	}
	else if(pCurLinePart->GetClassTypeId()==CLS_LINETUBE)	//依赖杆件是钢管
	{
		if(pCurLinePart->handle==designInfo.m_hBasePart)
			return FALSE;	//基准杆件不进行设计
		CSuperSmartPtr<CLDSPart> pPart;
		if(pLinePartPara->start0_end1==0)	//始端连接
			pPart=BelongModel()->FromPartHandle(pCurLinePart.LineTubePointer()->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		else if(pLinePartPara->start0_end1==1)	//终端连接
			pPart=BelongModel()->FromPartHandle(pCurLinePart.LineTubePointer()->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		double L,H;
		if(pPart.IsNULL())
			return FALSE;	//无端板不进行设计
		if(pPart.ParamPlatePointer()->m_iParamType!=TYPE_UEND&&
			pPart.ParamPlatePointer()->m_iParamType!=TYPE_ROLLEND&&
			pPart.ParamPlatePointer()->m_iParamType!=TYPE_XEND)
			return FALSE;	//不是U型插板、槽型插板或十字插板不进行设计
		if(pPart.ParamPlatePointer()->m_bStdPart)
		{	//标准构件
			CLDSStdPartModel stdPartModel;
			if(!pPart.ParamPlatePointer()->GetStdPartModel(&stdPartModel))
				return 0;
			L=stdPartModel.param.insert_plate_param.L;
			H=stdPartModel.param.insert_plate_param.H;
		}
		else
		{	//非标准构件
			pPart.ParamPlatePointer()->GetDesignItemValue('L',&L);
			pPart.ParamPlatePointer()->GetDesignItemValue('H',&H);
		}
		double minx=1000000,maxx=0,miny=0.5*H,maxy=-0.5*H;
		long bolt_d=20;
		if(pLsRefList->GetLsCount()==0)
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("0X%X plate's 0X%X plug board lacks bolt hole,can't calculate vertex coodinate of relative outline.",handle,pCurLinePart->handle);
#else 
				Log2File()->Log("0X%X钢板的0X%X连接钢管端头插板上缺少螺栓孔,无法计算相关轮廓顶点坐标",handle,pCurLinePart->handle);
#endif
			return FALSE;	//射线杆件上无螺栓
		}
		for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			bolt_d=pBolt->get_d();
			//钢管插板不同于角钢不需要判断螺栓是否一定在在当前钢板上,否则容错性太差(如插板重叠,螺栓孔重叠时) wjh-2014.12.26
			//if(pBolt&&pPart.ParamPlatePointer()->FindLsByHandle(pBolt->handle))
			{	//钢管正负头修改后螺栓位置和法线方向都有可能变化
				pBolt->correct_worknorm();
				pBolt->correct_pos();
				f3dPoint pos = pBolt->ucs.origin;
				coord_trans(pos,pPart->ucs,FALSE);
				if(pos.x<minx)
					minx=pos.x;
				if(pos.x>maxx)
					maxx=pos.x;
				if(pos.y<miny)
					miny=pos.y;
				if(pos.y>maxy)
					maxy=pos.y;
			}
		}
		if(minx>maxx)
			minx=maxx;
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,bolt_d);
		f3dPoint vertex_up,vertex_down;
		vertex_up.x=vertex_down.x=minx-LsSpace.EndSpace;
		//按照定制外形计算顶点坐标
		vertex_up.y=0.5*H;
		if(pLinePartPara->ber_space>0)
			vertex_up.y+=pLinePartPara->ber_space;
		vertex_down.y=-vertex_up.y;
		coord_trans(vertex_up,pPart->ucs,TRUE);
		coord_trans(vertex_down,pPart->ucs,TRUE);
		//端板上的顶点
		coord_trans(vertex_up,ucs,FALSE);
		vertex_up=GetSpreadVertex(vertex_up,pLinePartPara->iFaceNo);
		vertex_up.z=0;
		vertexArr.append(vertex_up);
		coord_trans(vertex_down,ucs,FALSE);
		vertex_down=GetSpreadVertex(vertex_down,pLinePartPara->iFaceNo);
		vertex_down.z = 0;
		vertexArr.append(vertex_down);
		//计算螺栓
		if(pBlotUp&&pBoltDown)
		{
			f3dPoint bolt_up,bolt_down;
			bolt_up.x=bolt_down.x=minx;
			bolt_up.y=maxy;
			bolt_down.y=miny;
			coord_trans(bolt_up,pPart->ucs,TRUE);
			coord_trans(bolt_down,pPart->ucs,TRUE);
			pBlotUp=&bolt_up;
			pBoltDown=&bolt_down;
		}
	}
	return TRUE;
}
void CLDSPlate::CalWeldLineVertex(CDesignLjPartPara *pLinePartPara,f3dPoint vertex_up,f3dPoint vertex_down,f3dPoint bolt_up,
						  f3dPoint bolt_down,f3dPoint &weld_vertex_start,f3dPoint &weld_vertex_end,BOOL bFirstRayPart)
{
	BOOL bCheckTowEnd=FALSE;	//TRUE表示检测焊缝线两端顶点
	if(designInfo.partList.GetNodeNum()<2) //板上除基准杆件外只连有一根杆件的情况
		bCheckTowEnd=TRUE;
	CLDSLinePart *pBaseLinePart=(CLDSLinePart*)m_pModel->FromPartHandle(designInfo.m_hBasePart,CLS_LINEPART);
	if(pBaseLinePart==NULL)
		return;
	CLDSLinePart* pCurLinePart=(CLDSLinePart*)m_pModel->FromPartHandle(pLinePartPara->hPart,CLS_LINEPART);
	if(pCurLinePart==NULL)
		return;
	
	f3dLine prj_line;	//投影基准线
	double stress_diffuse_angle=CLDSPart::library->GetStressDiffuseAngle();
	f3dPoint vice_vec,offset_vec,baseLineVec;
	baseLineVec = pBaseLinePart->End() - pBaseLinePart->Start();
	normalize(baseLineVec);
	if(pLinePartPara->start0_end1!=1)	//始端连接或中间连接
		vice_vec=pCurLinePart->End()-pCurLinePart->Start();
	else								//终端连接
		vice_vec=pCurLinePart->Start()-pCurLinePart->End();
	normalize(vice_vec);

	if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
	{	//基准杆件为钢管
		int nFlag = 1;
		TUBEJOINT *pCurJoint=NULL;
		CLDSLineTube* pBaseTube = (CLDSLineTube*)pBaseLinePart;
		f3dPoint otherBaseLineVec,cutPlaneNorm,cutPlanePos;
		if(pBaseTube->pStart->handle==designInfo.m_hBaseNode)
		{
			pCurJoint=&pBaseTube->m_tJointStart;
			cutPlanePos = pBaseTube->Start();
		}
		else if(pBaseTube->pEnd->handle==designInfo.m_hBaseNode)
		{
			pCurJoint=&pBaseTube->m_tJointEnd;
			cutPlanePos = pBaseTube->End();
			nFlag = -1;
		}
		CLDSLineTube* pOtherBaseTube=NULL;
		//TODO:为什么添加designInfo.FromHandle(pCurJoint->hLinkObj
		if(pCurJoint&&pCurJoint->type==1&&pCurJoint->hViceLinkObj<0&&designInfo.FromHandle(pCurJoint->hLinkObj))
			pOtherBaseTube=(CLDSLineTube*)m_pModel->FromPartHandle(pCurJoint->hLinkObj,CLS_LINETUBE);
		if(pOtherBaseTube)
		{	//计算法线
			otherBaseLineVec = pOtherBaseTube->End() - pOtherBaseTube->Start();
			if(otherBaseLineVec*baseLineVec > 0)
				otherBaseLineVec *= -1;
			normalize(otherBaseLineVec);
			f3dPoint midVec = otherBaseLineVec + baseLineVec;
			if (midVec*ucs.axis_x < 0)
				midVec *= -1;
			normalize(midVec);
			//f3dPoint cutPlaneNorm1 = otherBaseLineVec - midVec*(otherBaseLineVec*midVec);
			cutPlaneNorm = pCurJoint->cutPlaneNorm;
		}
		else if(pOtherBaseTube==NULL&&designInfo.partList.GetNodeNum()==2)
		{
			BOOL bPush=designInfo.partList.push_stack();
			CDesignLjPartPara *pLjPart=designInfo.partList.GetFirst();
			CDesignLjPartPara *pNextLjPart=designInfo.partList.GetNext();
			if(pLjPart->hPart==designInfo.m_hBasePart||pNextLjPart->hPart==designInfo.m_hBasePart)
				bCheckTowEnd=TRUE;
			if(bPush)
				designInfo.partList.pop_stack();
		}
		//获取投影基准线
		offset_vec=pBaseTube->GetPlateVertSpreadVec(this);
		if(offset_vec.IsZero())
			offset_vec=0.5*pBaseTube->GetDiameter()*ucs.axis_x;
		else
			offset_vec*=0.5*pBaseTube->GetDiameter();
		//觉得以下代码有问题，尤其是连接板命令设计钢管连接板时，会出错 wjh-2015.6.18
		//if(vice_vec*ucs.axis_x<0)
		//	offset_vec*=-1.0;
		prj_line.startPt=pBaseTube->Start()+offset_vec;
		prj_line.endPt=pBaseTube->End()+offset_vec;
		if (pOtherBaseTube)
		{
			f3dPoint lineStart;
			Int3dlf(lineStart,prj_line,cutPlanePos,cutPlaneNorm);
			f3dLine otherPrjLine(lineStart,lineStart+otherBaseLineVec*100);
			if (vice_vec*baseLineVec*nFlag<0)
				prj_line = otherPrjLine;
			//添加特殊凹点
			coord_trans(lineStart,ucs,FALSE);
			lineStart.feature = pLinePartPara->iFaceNo;
			//避免添加重复的凹点 wht 10-06-07
			PROFILE_VERTEX *pTempVertex=NULL;
			for(pTempVertex=vertex_list.GetFirst();pTempVertex;pTempVertex=vertex_list.GetNext())
			{
				if(pTempVertex->vertex==lineStart)
					break;
			}
			if(pTempVertex==NULL)
			{
				PROFILE_VERTEX *pCutPlanePt=vertex_list.append(PROFILE_VERTEX(lineStart));
				//pCutPlanePt->rayVecX = -1;
				pCutPlanePt->rayVecX=pCutPlanePt->rayVecY=0;
				pCutPlanePt->m_bCanCut = FALSE;
			}
		}
	}
	else if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE&&designInfo.m_bWeldToDatumJg)
	{	//基准杆件为钢管且为焊件连接	
		CLDSLineAngle *pBaseLineAngle=(CLDSLineAngle*)pBaseLinePart;
		if(designInfo.partList.GetNodeNum()==2)
		{
			BOOL bPush=designInfo.partList.push_stack();
			CDesignLjPartPara *pLjPart=designInfo.partList.GetFirst();
			CDesignLjPartPara *pNextLjPart=designInfo.partList.GetNext();
			if(pLjPart->hPart==designInfo.m_hBasePart||pNextLjPart->hPart==designInfo.m_hBasePart)
				bCheckTowEnd=TRUE;
			if(bPush)
				designInfo.partList.pop_stack();
		}
		//获取投影基准线
		CDesignLjPartPara *pLjPara=designInfo.FromHandle(pBaseLinePart->handle);
		int wing_x0_y1=0;
		if(pLjPara)
			wing_x0_y1=pLjPara->angle.cur_wing_x0_y1;
		else
		{
			if(fabs(ucs.axis_z*pBaseLineAngle->get_norm_y_wing())>fabs(ucs.axis_z*pBaseLineAngle->get_norm_x_wing()))
				wing_x0_y1=1;	//Y肢为当前肢
		}
		f3dPoint cur_wing_vec=pBaseLineAngle->GetWingVecX();
		f3dPoint cur_wing_norm_vec=pBaseLineAngle->get_norm_x_wing();
		if(wing_x0_y1==1)
		{
			cur_wing_vec=pBaseLineAngle->GetWingVecY();
			cur_wing_norm_vec=pBaseLineAngle->cal_y_wing_norm();
		}
		offset_vec=pBaseLineAngle->GetWidth()*cur_wing_vec;
		prj_line.startPt=pBaseLineAngle->Start()+offset_vec;
		prj_line.endPt=pBaseLineAngle->End()+offset_vec;
	}
	else
		return;
	f3dPoint seed_vertex[4],diffused_up_vec,diffused_down_vec;
	SnapPerp(&seed_vertex[0],prj_line,vertex_up);
	SnapPerp(&seed_vertex[1],prj_line,vertex_down);
	diffused_up_vec = diffused_down_vec = -vice_vec;
	//根据国内外经验应力扩散角均取30°,即每侧扩散30°，总计60°
	double diffuseAngle=stress_diffuse_angle*RADTODEG_COEF;
	//计算上下两条扩散线方向的时候应该绕螺栓所在构件的Z轴旋转
	f3dPoint axis_z;
	if(pCurLinePart->GetClassTypeId()==CLS_LINEANGLE)
		axis_z=ucs.axis_z;////pCurLinePart->ucs.axis_z;
	else if(pCurLinePart->GetClassTypeId()==CLS_LINETUBE)
	{	
		CLDSLineTube *pTube = (CLDSLineTube *)pCurLinePart;
		CSuperSmartPtr<CLDSPart> pPart;
		if(pLinePartPara->start0_end1==0)	//始端连接
			pPart=BelongModel()->FromPartHandle(
			pTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		else if(pLinePartPara->start0_end1==1)	//终端连接
			pPart=BelongModel()->FromPartHandle(
			pTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pPart.IsNULL())
			return;	//无端板不进行设计
		else if(pPart.ParamPlatePointer()->m_iParamType!=TYPE_UEND&&
			pPart.ParamPlatePointer()->m_iParamType!=TYPE_ROLLEND&&
			pPart.ParamPlatePointer()->m_iParamType!=TYPE_XEND)
			return;	//不是U型插板、槽型插板或十字插板不进行设计
		axis_z=pPart->ucs.axis_z;
	}
	RotateVectorAroundVector(diffused_up_vec,diffuseAngle,axis_z);
	RotateVectorAroundVector(diffused_down_vec,-diffuseAngle,axis_z);
	f3dPoint diffused_upplane_norm = diffused_up_vec^ucs.axis_z;
	f3dPoint diffused_downplane_norm = diffused_down_vec^ucs.axis_z;
	Int3dlf(seed_vertex[2],prj_line,bolt_up,diffused_upplane_norm);
	Int3dlf(seed_vertex[3],prj_line,bolt_down,diffused_downplane_norm);
	//用扩散角中线纠正无效的扩散线所带来的板外形畸形
	f3dPoint midVertex,diffused_midplane_norm = vice_vec^ucs.axis_z;
	Int3dlf(midVertex,prj_line,0.5*(bolt_up+bolt_down),diffused_midplane_norm);
	double dd2=DISTANCE(seed_vertex[2],midVertex);
	double dd3=DISTANCE(seed_vertex[3],midVertex);
	offset_vec=seed_vertex[3]-seed_vertex[2];
	normalize(offset_vec);
	f3dPoint vec2=seed_vertex[2]-midVertex;
	normalize(vec2);
	f3dPoint vec3=seed_vertex[3]-midVertex;
	normalize(vec3);
	if(vec2*vec3<0)
	{	//扩散生成的两点在扩散线两侧，较远的种子顶点肯定是位于中点另一侧种子顶点,因此一律取较短者
		if(!bCheckTowEnd)
		{	//仅有一根射线杆件时不应该以短代长，当有多根射线杆件时板外形才会因较远的种子而发生异常
			if(dd2<dd3)		
				seed_vertex[3]=midVertex+offset_vec*dd2;
			else if(dd2>dd3)
				seed_vertex[2]=midVertex-offset_vec*dd3;
		}
		else
		{	//由于塔脚处八字腿部斜材与主材夹角小时，会导致该板外形很难看，故应考虑环向封板或底板因素　wjh-2015.1.02
			if((dd2+dd2+dd2)<dd3)
				seed_vertex[3]=midVertex+offset_vec*(dd2+dd2+dd2);
			else if(dd2>(dd3+dd3+dd3))
				seed_vertex[2]=midVertex-offset_vec*(dd3+dd3+dd3);
		}
	}
	else if(vec2*vec3>0)
	{	//扩散生成的两点在扩散线同一侧，较远的种子点肯定是另一侧种子顶点发生异变导致，故应取较短者并反号
		if(dd2<dd3)
			seed_vertex[3]=midVertex-vec3*dd2;
		else if(dd2>dd3)
			seed_vertex[2]=midVertex-vec2*dd3;
	}
	/*BOOL bStartSide=TRUE;	//FALSE;
	//感觉下面这段代码有问题可能时当时添加小夹角射线材判断时忘了去掉
	if(minCosa*maxCosa>0&&minCosa!=maxCosa)
	{	//有多根射线杆件但仅分布在始端侧或终端侧一侧
		if(minCosa>0)
		{	//射线杆件均分布在始端侧 用角度最大的杆件去控制终端焊缝端点
			bStartSide=TRUE;
			if(vice_vec*base_part_line_vec==minCosa)
			bStartSide=FALSE;
			}
		else if(maxCosa<0)
		{	//射线杆件均分布在终端侧 用角度最小的杆件去控制始端焊缝端点
			bStartSide=FALSE;
			if(vice_vec*base_part_line_vec==maxCosa)
			bStartSide=TRUE;
		}
	}
	else
	{	//仅有一根杆件或者始端侧和终端侧均有射线杆件时
		if(vice_vec*base_part_line_vec<0)	//ucs.axis_y
		bStartSide=TRUE;
	}*/
	for(int i=0;i<4;i++)
	{
		if(bFirstRayPart&&i==0)
		{
			weld_vertex_start=weld_vertex_end=seed_vertex[0];
			f3dPoint start_vec=seed_vertex[1]-seed_vertex[0];
			if(start_vec*baseLineVec<0)	//始端侧
				weld_vertex_start=seed_vertex[1];
			else
				weld_vertex_end=seed_vertex[1];
			bFirstRayPart=FALSE;
			i++;
		}
		else
		{
			f3dPoint extend_vec;
			if(bCheckTowEnd)
			{	//只有一根射线杆件时一根杆件同时决定始终端两侧的焊缝起止点
				//始端侧
				extend_vec = seed_vertex[i]-weld_vertex_start;
				if(extend_vec*baseLineVec<0)//*ucs.axis_y<0)
					weld_vertex_start=seed_vertex[i];
				//终端侧
				extend_vec=seed_vertex[i]-weld_vertex_end;
				if(extend_vec*baseLineVec>0)//*ucs.axis_y>0)
					weld_vertex_end=seed_vertex[i];
			}
			else
			{	//多根射线材时始端侧射线只影响始端侧焊缝端点，终端侧射线只影响终端侧焊缝顶点
				f3dPoint verify_vec = seed_vertex[i]-midVertex;//ucs.origin;
				BOOL bVerifyStartVertex=FALSE;
				if(verify_vec*baseLineVec<0)	//ucs.axis_y<0
					bVerifyStartVertex=TRUE;
				if(bVerifyStartVertex)
				{
					extend_vec = seed_vertex[i]-weld_vertex_start;
					if(extend_vec*baseLineVec<0)//*ucs.axis_y<0)
						weld_vertex_start=seed_vertex[i];
				}
				else if(!bVerifyStartVertex)//!bStartSide&& 连有两根杆件 且bStartSide均为TRUE的情况
				{
					extend_vec=seed_vertex[i]-weld_vertex_end;
					if(extend_vec*baseLineVec>0)//*ucs.axis_y>0)
						weld_vertex_end=seed_vertex[i];
				}
			}
		}	
	}
}

//获取钢板在指定延伸方向上的最高点
f3dPoint GetPlateHighestVertex(CLDSPlate *pPlate,f3dPoint spread_vec)
{
	UCS_STRU temp_ucs=pPlate->ucs;
	temp_ucs.axis_x=spread_vec;
	temp_ucs.axis_y=temp_ucs.axis_z^temp_ucs.axis_x;
	normalize(temp_ucs.axis_y);
	CMaxDouble maxValue;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		f3dPoint vertex=pVertex->vertex;
		coord_trans(vertex,pPlate->ucs,TRUE);
		coord_trans(vertex,temp_ucs,FALSE);
		maxValue.Update(vertex.x,pVertex);
	}
	pVertex=(PROFILE_VERTEX*)maxValue.m_pRelaObj;
	if(pVertex==NULL)
		return f3dPoint(0,0,0);
	else
	{
		f3dPoint vertex=pVertex->vertex;
		coord_trans(vertex,pPlate->ucs,TRUE);
		vertex+=pPlate->ucs.axis_z*(pPlate->GetNormOffset()+0.5*pPlate->Thick);
		return vertex;
	}
}

typedef PROFILE_VERTEX* VERTEX_PTR;
//获取底边对应的始终端轮廓点
BOOL GetPlateBottomEdgeProfileVertex(CLDSPlate *pPlate,f3dPoint spread_vec,f3dPoint tube_len_vec,
									 VERTEX_PTR *ppStartVertex,VERTEX_PTR *ppEndVertex)
{
	UCS_STRU temp_ucs=pPlate->ucs;
	temp_ucs.axis_x=spread_vec;
	temp_ucs.axis_y=temp_ucs.axis_z^temp_ucs.axis_x;
	normalize(temp_ucs.axis_y);
	//
	BOOL bRetCode=FALSE;
	double minVertexX=100000;
	vector_trans(tube_len_vec,pPlate->ucs,FALSE);
	PROFILE_VERTEX *pPrevVertex=pPlate->vertex_list.GetTail();
	for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		f3dPoint edge_vec=pVertex->vertex-pPrevVertex->vertex;
		normalize(edge_vec);
		double dd=tube_len_vec*edge_vec;
		if(fabs(dd)>EPS_COS2)
		{
			f3dPoint vertex=(pVertex->vertex+pPrevVertex->vertex)*0.5;
			coord_trans(vertex,pPlate->ucs,TRUE);
			coord_trans(vertex,temp_ucs,FALSE);
			if(vertex.x<minVertexX)
			{
				minVertexX=vertex.x;
				if(ppStartVertex&&ppEndVertex)
				{
					*ppStartVertex=pPrevVertex;
					*ppEndVertex=pVertex;
				}
			}
			bRetCode=TRUE;
		}
		pPrevVertex=pVertex;
	}
	return bRetCode;
}
//获取钢板指定延伸方向上的底边
//ppStartVertex:底边方向与tube_len_vec相同时为始端轮廓点，相反为终端轮廓点
//pbReverse:底边方向与tube_len_vec相同时为FALSe,相反为TRUE
BOOL GetPlateBottomEdge(CLDSPlate *pPlate,f3dPoint spread_vec,f3dPoint tube_len_vec,
						f3dLine &btm_line,VERTEX_PTR *ppStartVertex,BOOL *pbReverse)
{
	PROFILE_VERTEX *pStartVertex=NULL,*pEndVertex=NULL;
	BOOL bRetCode=GetPlateBottomEdgeProfileVertex(pPlate,spread_vec,tube_len_vec,&pStartVertex,&pEndVertex);
	if(!bRetCode||pStartVertex==NULL||pEndVertex==NULL)
		return FALSE;
	vector_trans(tube_len_vec,pPlate->ucs,FALSE);
	f3dPoint edge_vec=pEndVertex->vertex-pStartVertex->vertex;
	normalize(edge_vec);
	double dd=tube_len_vec*edge_vec;
	if(ppStartVertex)
		*ppStartVertex=(dd>0)?pStartVertex:pEndVertex;
	if(pbReverse)
		*pbReverse=(dd<0);
	btm_line.startPt=pStartVertex->vertex;
	btm_line.endPt=pEndVertex->vertex;
	coord_trans(btm_line.startPt,pPlate->ucs,TRUE);
	coord_trans(btm_line.endPt,pPlate->ucs,TRUE);
	btm_line.startPt+=pPlate->ucs.axis_z*(pPlate->GetNormOffset()+0.5*pPlate->Thick);
	btm_line.endPt+=pPlate->ucs.axis_z*(pPlate->GetNormOffset()+0.5*pPlate->Thick);
	return bRetCode;
}
//合并钢板轮廓标至vertex_list
f3dLine AddPlateVertexsToList(ATOM_LIST<PROFILE_VERTEX> &vertex_list,f3dPoint vertex_arr[4],UCS_STRU &plate_ucs,int iFaceNo,
							  CLDSPlate *pPlate,PROFILE_VERTEX *pStartVertex,BOOL bReserveOrder)
{	
	//0.计算火曲线
	f3dLine axis_line;
	if(iFaceNo==2)
	{	
		axis_line.startPt=vertex_arr[1];
		axis_line.endPt=vertex_arr[2];
	}
	else //if(iFaceNo==3)
	{
		axis_line.startPt=vertex_arr[3];
		axis_line.endPt=vertex_arr[0];
	}
	//1.获取钢板顶点链表,该链表第一个点为底边第一个点,旋转方向为底边向外延伸方向
	//  iFaceNo==2时为逆时针方向 iFaceNo==3时为顺时针方向,故iFaceNo==3时需要翻转链表顺序保证逆时针方向添加 wht 16-09-30
	BOOL bAddToTempSet=TRUE;
	CXhPtrSet<PROFILE_VERTEX> vertexSet,tempVertexSet;
	if(bReserveOrder)
	{
		for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetTail();pVertex;pVertex=pPlate->vertex_list.GetPrev())
		{
			if(pVertex==pStartVertex)
				bAddToTempSet=FALSE;
			if(bAddToTempSet)
				tempVertexSet.append(pVertex);
			else
				vertexSet.append(pVertex);
		}
	}
	else
	{
		for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex==pStartVertex)
				bAddToTempSet=FALSE;
			if(bAddToTempSet)
				tempVertexSet.append(pVertex);
			else
				vertexSet.append(pVertex);
		}
	}
	if(tempVertexSet.GetNodeNum()>0)
		vertexSet.append(tempVertexSet);
	if(iFaceNo==3)
	{	//翻转链表顺序保证链表顺序为逆时针方向
		tempVertexSet.Empty();
		tempVertexSet.append(vertexSet);
		vertexSet.Empty();
		for(PROFILE_VERTEX *pVertex=tempVertexSet.GetTail();pVertex;pVertex=tempVertexSet.GetPrev())
			vertexSet.append(pVertex);
	}
	//2.计算火曲角度
	UCS_STRU huoquucs;
	huoquucs.axis_z=axis_line.endPt-axis_line.startPt;
	normalize(huoquucs.axis_z);
	huoquucs.axis_x.Set(0,0,1);
	huoquucs.axis_y=huoquucs.axis_z^huoquucs.axis_x;
	f3dPoint huoqu_norm=pPlate->ucs.axis_z;
	//TODO:当火曲角度超过90度时存在隐患
	if(huoqu_norm*plate_ucs.axis_z<0)
		huoqu_norm*=-1;
	vector_trans(huoqu_norm,plate_ucs,FALSE);
	//if(fabs(huoqu_norm*huoquucs.axis_z)>0.001)//EPS)//部分挂线板火曲面与火曲线误差偏大一点,如果太严格总报警示信息 wjh-2016.3.28
	//Log2File()->Log("0x%X钢板的第%d(火曲)面法线方向与火曲线不垂直，火曲角计算有误!");//,handle,iFace-1);
	vector_trans(huoqu_norm,huoquucs,FALSE);
	double huoquangle=Cal2dLineAng(0,0,huoqu_norm.x,huoqu_norm.y);
	if(huoquangle>Pi)
		huoquangle-=2*Pi;
	//3.将顶点链表添加至vertex_list 
	int i=0;
	double fMaxPosY=-10000;
	GEPOINT topEdgeStart,topEdgeEnd;
	BOOL bFirstPtIsHuoquPt=FALSE;
	if(iFaceNo==3)
	{	//axis_line.startPt为顶部火曲点
		topEdgeStart=axis_line.startPt;
		bFirstPtIsHuoquPt=TRUE;
		i=1;
	}
	//钢板偏移量均为半个板厚,若钢板偏移量不为半个板厚时需要进行偏移 wht 16-10-20
	double fNormOffset=pPlate->m_fNormOffset+(0.5*pPlate->GetThick());
	PROFILE_VERTEX *pPrevVertex=vertexSet.GetTail();
	for(PROFILE_VERTEX *pVertex=vertexSet.GetFirst();pVertex;pVertex=vertexSet.GetNext())
	{
		f3dPoint vertex=pVertex->vertex;
		if(fabs(fNormOffset)>0)
			vertex.z-=fNormOffset;
		coord_trans(vertex,pPlate->ucs,TRUE);
		coord_trans(vertex,plate_ucs,FALSE);
		//将实际坐标转换为展开坐标
		rotate_point_around_axis(vertex,-huoquangle,axis_line.startPt,axis_line.endPt);
		if(fabs(vertex.z)>EPS)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			CLDSObject::Log2File()->Log("Vertex pos is error when create fold plate!");
#else 
			CLDSObject::Log2File()->Log("生成折叠板时火曲面轮廓点转至基准面时出现异常!");
#endif
		}
		vertex.feature=iFaceNo;
		if(DistOf2dPtLine(vertex,axis_line.startPt,axis_line.endPt)<0)	//在火曲线右侧
		{
			if(i==0)
			{	//TODO:这里根据Y值判断是否存在构架梁顶部板与底板Y轴反向的隐患?
				if(bFirstPtIsHuoquPt&&pPrevVertex)
				{	//第一个轮廓点为火曲点时，如果前一个轮廓点Y坐标较大时直接取前一轮廓点为始端点 wht 16-10-10
					if(pPrevVertex->vertex.y>topEdgeStart.y)
						topEdgeStart=pPrevVertex->vertex;
				}
				else
					topEdgeStart=vertex;
			}
			else if(i==1)
			{
				topEdgeEnd=vertex;	
				fMaxPosY=(topEdgeStart.y+topEdgeEnd.y)*0.5;
			}
			else if(i>1)
			{
				double dist=(vertex.y+topEdgeEnd.y)*0.5;
				//终端点在直线(始:直线始端,终:下一个轮廓点)左侧还是右侧,dd>0在左侧为凹点，dd<0在右侧为凸点
				double dd=DistOf2dPtLine(topEdgeEnd,topEdgeStart,vertex);	
				if((bFirstPtIsHuoquPt&&dd>0)||	//顶部直线起始点为火曲点且终端点为凹点时取下一轮廓边为顶部直线
				   (dist>fMaxPosY&&dd<0))		//Y坐标大于最大Y坐标且终端点为凸点时取下一轮廓边为顶部直线	wht 16-10-10
				{
					topEdgeStart=topEdgeEnd;
					topEdgeEnd=vertex;
					fMaxPosY=dist;
					if(bFirstPtIsHuoquPt)
						bFirstPtIsHuoquPt=FALSE;
				}
			}
			i++;
			vertex_list.append(PROFILE_VERTEX(vertex));
		}
		//else 跳过火曲线左侧无效顶点
		pPrevVertex=pVertex;
	}
	if(iFaceNo!=3&&i==1)			//未找到顶部直线末端点，以顶部火曲点为末端点
		topEdgeEnd=axis_line.endPt;	//axis_line.endPt为顶部火曲点
	return f3dLine(topEdgeStart,topEdgeEnd);
}
//设计钢管折叠板
bool CLDSPlate::DesignTubeFoldPlate()
{
	if(designInfo.iFaceType!=6||face_N!=3)
		return false;
	//1.获取基准钢管
	CLDSNode *pBaseNode=BelongModel()->FromNodeHandle(designInfo.m_hBaseNode);
	CLDSLineTube *pTube=(CLDSLineTube*)BelongModel()->FromPartHandle(designInfo.m_hBasePart,CLS_LINETUBE),*pOtherTube=NULL;
	if(pTube==NULL||pBaseNode==NULL)
		return false;
	TUBEJOINT *pCurJoint=NULL;
	if(pTube->pStart==pBaseNode)
		pCurJoint=&pTube->m_tJointStart;
	else if(pTube->pEnd==pBaseNode)
		pCurJoint=&pTube->m_tJointEnd;
	if(pCurJoint&&pCurJoint->type==1)
		pOtherTube=(CLDSLineTube*)BelongModel()->FromPartHandle(pCurJoint->hLinkObj,CLS_LINETUBE);
	typedef CLDSLineTube* TUBE_PTR;
	typedef CLDSPlate* PLATE_PTR;
	TUBE_PTR baseTubePtrArr[2]={pTube,pTube};	//0.第3面钢板对应的基准钢管 1.第2面钢板对应的基准钢管
	if(pOtherTube!=NULL)
	{
		CDesignLjPartPara *pLjPara=GetLjRodPara(pOtherTube->handle);
		if(pLjPara&&pLjPara->iFaceNo==3)
			baseTubePtrArr[0]=pOtherTube;
		else if(pLjPara&&pLjPara->iFaceNo==2)
			baseTubePtrArr[1]=pOtherTube;
	}
	//变电构件横梁，两根十字交叉斜材需要调整始终端偏移量避免碰撞，调整偏移量后钢板相应面应随之旋转一定角度，尽量保证角钢与钢板紧贴
	//根据角钢偏移量及射线角钢正负头，计算钢板相应面偏移角度，获取面法线方向 wht 18-05-03
	ROD_PTR anglePtrArr[2]={NULL};
	double normOffsetArr[2]={0,0};
	double offsetAngleArr[2]={0,0};
	GEPOINT rotateAxisArr[2];
	double fHalfThick=Thick*0.5;
	double fTubeR=pTube->GetDiameter()*0.5;
	for(CDesignLjPartPara *pLjPara=GetFirstLjPara();pLjPara;pLjPara=GetNextLjPara())
	{
		if(pLjPara->iFaceNo!=2&&pLjPara->iFaceNo!=3)
			continue;
		pLjPara->pCurLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pLjPara->hPart,CLS_LINEPART);
		if(pLjPara->pCurLinePart==pOtherTube||pLjPara->pCurLinePart==pTube||!pLjPara->pCurLinePart->IsAngle())
			continue;	//不添加对接钢管和基准构件
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLjPara->pCurLinePart;
		int index=(pLjPara->iFaceNo==2)?0:1;
		anglePtrArr[index]=pLjPara->pCurLinePart;
		if(pLjPara->start0_end1==0&&pLjPara->angle.cur_wing_x0_y1==0&&pLineAngle->desStartPos.wing_x_offset.gStyle==4)
			normOffsetArr[index]=-fHalfThick-pLineAngle->desStartPos.wing_x_offset.offsetDist;
		else if(pLjPara->start0_end1==0&&pLjPara->angle.cur_wing_x0_y1==1&&pLineAngle->desStartPos.wing_y_offset.gStyle==4)
			normOffsetArr[index]=-fHalfThick-pLineAngle->desStartPos.wing_y_offset.offsetDist;
		else if(pLjPara->start0_end1==1&&pLjPara->angle.cur_wing_x0_y1==0&&pLineAngle->desEndPos.wing_x_offset.gStyle==4)
			normOffsetArr[index]=-fHalfThick-pLineAngle->desEndPos.wing_x_offset.offsetDist;
		if(pLjPara->start0_end1==1&&pLjPara->angle.cur_wing_x0_y1==1&&pLineAngle->desEndPos.wing_y_offset.gStyle==4)
			normOffsetArr[index]=-fHalfThick-pLineAngle->desEndPos.wing_y_offset.offsetDist;
		if(normOffsetArr[index]>0)
		{
			GEPOINT wing_norm=pLjPara->angle.cur_wing_x0_y1==0?pLineAngle->vxWingNorm:pLineAngle->vyWingNorm;
			GEPOINT len_vec=pLineAngle->End()-pLineAngle->Start();
			if(pLjPara->start0_end1==1)
				len_vec*=-1.0;
			normalize(len_vec);
			GEPOINT verify_vec=wing_norm^len_vec;
			normalize(verify_vec);
			rotateAxisArr[index]=pTube->ucs.axis_z;
			if(verify_vec*rotateAxisArr[index]<0)
				rotateAxisArr[index]*=-1.0;
			double ray_angle=cal_angle_of_2vec(len_vec,pTube->ucs.axis_z);
			double fOddLen=fabs((pLjPara->start0_end1==0)?pLineAngle->startOdd():pLineAngle->endOdd())+30;
			fOddLen*=sin(ray_angle);
			offsetAngleArr[index]=atan(normOffsetArr[0]/fOddLen);
		}
		if(anglePtrArr[0]&&anglePtrArr[1])
			break;
	}
	//2.更新火曲面法线
	designInfo.huoqufacenorm[0].UpdateVector(BelongModel());
	huoqufacenorm[0]=designInfo.huoqufacenorm[0].vector;
	designInfo.huoqufacenorm[1].UpdateVector(BelongModel());
	huoqufacenorm[1]=designInfo.huoqufacenorm[1].vector;
	//
	CLDSVector temp_norm[2]={designInfo.huoqufacenorm[0],designInfo.huoqufacenorm[1]};
	for(int i=0;i<2;i++)
	{
		if(anglePtrArr[i]==NULL||fabs(offsetAngleArr[i])<EPS)
			continue;
		temp_norm[i].norm_style=0;
		RotateVectorAroundVector(huoqufacenorm[i],offsetAngleArr[i],rotateAxisArr[i]);
		temp_norm[i].vector=temp_norm[i].nearVector=huoqufacenorm[i];
	}
	//2.构造两块临时钢板计算轮廓点链表
	CLDSPlate plateArr[2];
	PLATE_PTR platePtrArr[2]={&plateArr[0],&plateArr[1]};
	for(int i=0;i<2;i++)
	{
		int index=2-(i+1);	//第2面对应第2块钢板，第3面对应第1块钢板 wht 16-10-14
		platePtrArr[index]->SetBelongModel(BelongModel());
		platePtrArr[index]->CopyProperty(this);
		platePtrArr[index]->face_N=1;
		platePtrArr[index]->jdb_style=0;
		platePtrArr[index]->designInfo.iFaceType=1;
		platePtrArr[index]->designInfo.iProfileStyle0123=designInfo.iProfileStyle0123;
		platePtrArr[index]->designInfo.m_bEnableFlexibleDesign=TRUE;
		platePtrArr[index]->designInfo.m_hBaseNode=designInfo.m_hBaseNode;
		platePtrArr[index]->designInfo.m_hBasePart=baseTubePtrArr[index]->handle;
		platePtrArr[index]->designInfo.norm=temp_norm[i];
		
		platePtrArr[index]->designInfo.origin.datum_pos_style=2;
		platePtrArr[index]->designInfo.origin.des_para.RODNODE.hRod=baseTubePtrArr[index]->handle;
		platePtrArr[index]->designInfo.origin.des_para.RODNODE.hNode=pBaseNode->handle;
		platePtrArr[index]->designInfo.origin.des_para.RODNODE.bFaceOffset=FALSE;
		platePtrArr[index]->designInfo.origin.UpdatePos(BelongModel());
		platePtrArr[index]->ucs.origin=designInfo.origin.Position();
		platePtrArr[index]->ucs.axis_y.Set();
		//
		platePtrArr[index]->EmptyLsRef();
		platePtrArr[index]->designInfo.partList.Empty();
		for(CDesignLjPartPara *pLjPara=GetFirstLjPara();pLjPara;pLjPara=GetNextLjPara())
		{
			if(pLjPara->iFaceNo==(i+2))
			{
				pLjPara->pCurLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pLjPara->hPart,CLS_LINEPART);
				if(pLjPara->pCurLinePart==pOtherTube||pLjPara->pCurLinePart==pTube)
					continue;	//不添加对接钢管和基准构件
				CDesignLjPartPara *pNewLjPara=platePtrArr[index]->designInfo.partList.SetValue(pLjPara->hPart,*pLjPara);
				pNewLjPara->iFaceNo=1;
				//将杆件关联螺栓添加至钢板
				if(pNewLjPara->pCurLinePart)
				{
					for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
					{
						if(pNewLjPara->pCurLinePart->FindLsByHandle((*pLsRef)->handle))
							platePtrArr[index]->AppendLsRef(*pLsRef);
					}
				}
			}
		}
		platePtrArr[index]->DesignPlate();
	}
	if( (platePtrArr[0]->designInfo.partList.GetNodeNum()==1&&platePtrArr[1]->designInfo.partList.GetNodeNum()==2)||
		(platePtrArr[1]->designInfo.partList.GetNodeNum()==1&&platePtrArr[0]->designInfo.partList.GetNodeNum()==2))
	{	//三根射线角钢，其中一面有一根射线角钢且为待偏移量的外铁
		int index=(platePtrArr[0]->designInfo.partList.GetNodeNum()==1)?0:1;
		CDesignLjPartPara *pLjPart=platePtrArr[index]->designInfo.partList.GetFirst();
		CLDSLineAngle *pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pLjPart->hPart,CLS_LINEANGLE);
		const double MIN_NORM_OFFSET = -10;
		WING_OFFSET_STYLE wingOffsetArr[2][2]={ {pAngle->desStartPos.wing_x_offset,pAngle->desStartPos.wing_y_offset},
												{pAngle->desEndPos.wing_x_offset,pAngle->desEndPos.wing_y_offset}};
		int cur_wing=pLjPart->angle.cur_wing_x0_y1;
		if( pAngle&&(cur_wing==0||cur_wing==1)&&
			wingOffsetArr[0][cur_wing].gStyle==4&&wingOffsetArr[1][cur_wing].gStyle==4&&
			wingOffsetArr[0][cur_wing].offsetDist<MIN_NORM_OFFSET&&
			fabs(wingOffsetArr[0][cur_wing].offsetDist-wingOffsetArr[1][cur_wing].offsetDist)<EPS2)
		/* fi未来有时间审核确认代码后应用ciAnchorWing取代上述代码 wjh-2018.2.28
		BYTE ciAnchorWing=(pAngle->pEnd!=NULL&&pAngle->pEnd->handle==designInfo.m_hBaseNode)?pAngle->ciEndAnchorWing:pAngle->ciStartAnchorWing;
		if(ciAnchorWing==1||ciAnchorWing==2)*/
		{	
			GEPOINT work_norm=(pLjPart->angle.cur_wing_x0_y1==0)?pAngle->vxWingNorm:pAngle->vyWingNorm;
			//GEPOINT work_norm=(ciAnchorWing==1)?pAngle->vxWingNorm:pAngle->vyWingNorm;
			if(work_norm*platePtrArr[index]->ucs.axis_z<0)
			{
				platePtrArr[index]->designInfo.origin.datum_pos_style=1;
				platePtrArr[index]->designInfo.origin.des_para.RODEND.hRod=pAngle->handle;
				if(pBaseNode==pAngle->pStart)
					platePtrArr[index]->designInfo.origin.des_para.RODEND.direction=0;
				else
					platePtrArr[index]->designInfo.origin.des_para.RODEND.direction=1;
				platePtrArr[index]->m_fNormOffset=-platePtrArr[index]->Thick;
				platePtrArr[index]->DesignPlate();
			}
		}
	}
	//3.重新计算火曲线位置、基准面法线及基准面轮廓点
	GEPOINT cut_plane_norm,cut_plane_pick=pBaseNode->Position();
	baseTubePtrArr[0]->BuildUCS();
	if(baseTubePtrArr[1]!=baseTubePtrArr[0])
		baseTubePtrArr[1]->BuildUCS();
	GEPOINT tube_len_vec[2]={baseTubePtrArr[0]->ucs.axis_z,baseTubePtrArr[1]->ucs.axis_z};
	//3.1 计算切割面及基准钢管向外延伸方向
	if(baseTubePtrArr[0]!=baseTubePtrArr[1]&&fabs(baseTubePtrArr[0]->ucs.axis_z*baseTubePtrArr[1]->ucs.axis_z)<EPS_COS)
	{	//两钢板基准构件不同时一般为端连接情况，根据连接端判断连接方向
		for(int i=0;i<2;i++)
		{
			tube_len_vec[i]=baseTubePtrArr[i]->ucs.axis_z;
			if(baseTubePtrArr[i]->pEnd==pBaseNode||DISTANCE(baseTubePtrArr[i]->Start(),platePtrArr[i]->ucs.origin)>DISTANCE(baseTubePtrArr[i]->End(),platePtrArr[i]->ucs.origin))
				tube_len_vec[i]=-baseTubePtrArr[i]->ucs.axis_z;
		}
		//计算切割面法线
		f3dPoint axis_z=tube_len_vec[0]^-tube_len_vec[1];
		normalize(axis_z);
		cut_plane_norm=tube_len_vec[0]+tube_len_vec[1];
		normalize(cut_plane_norm);
		RotateVectorAroundVector(cut_plane_norm,1,0,axis_z);
	}
	else
	{
		GEPOINT lsPosArr[2]={platePtrArr[0]->ucs.origin,platePtrArr[1]->ucs.origin};
		for(int i=0;i<2;i++)
		{
			CLsRef *pFirstLsRef=platePtrArr[i]->GetFirstLsRef();
			if(pFirstLsRef)
			{
				if(!platePtrArr[i]->GetBoltIntersPos(pFirstLsRef->GetLsPtr(),lsPosArr[i]))
#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,CXhChar100("Bolt 0x%X and Plate 0x%X calculate intersection point failed！",pFirstLsRef->GetLsPtr()->handle,handle));
#else 
					Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,CXhChar100("螺栓0x%X与钢板0x%X计算交点失败！",pFirstLsRef->GetLsPtr()->handle,handle));
#endif
			}
		}
		GEPOINT verfiy_vec=lsPosArr[1]-lsPosArr[0];
		normalize(verfiy_vec);
		if(tube_len_vec[0]*verfiy_vec>0)
			tube_len_vec[0]*=-1;
		else //if(tube_len_vec[1]*verfiy_vec<0)
			tube_len_vec[1]*=-1;
		//切割面法线
		cut_plane_norm=baseTubePtrArr[0]->ucs.axis_z;
	}
	//3.2 与截面求交计算交点，回退一个板厚获取两个轮廓点
	f3dLine btm_line_arr[2];
	f3dPoint datum_vertex[2];
	f3dPoint vertex_arr[4];
	VERTEX_PTR keyVertexPtr[2]={NULL,NULL};
	BOOL reserveOrderArr[2]={FALSE,FALSE};
	f3dPoint spreadVecArr[2]={baseTubePtrArr[0]->GetPlateVertSpreadVec(platePtrArr[0]),
							  baseTubePtrArr[1]->GetPlateVertSpreadVec(platePtrArr[1])};
	for(int i=0;i<2;i++)
	{
		if(!GetPlateBottomEdge(platePtrArr[i],spreadVecArr[i],tube_len_vec[i],btm_line_arr[i],&keyVertexPtr[i],&reserveOrderArr[i]))
			return FALSE;
		int nRetCode=Int3dlf(datum_vertex[i],btm_line_arr[i],cut_plane_pick,cut_plane_norm);
		vertex_arr[i]=datum_vertex[i]+tube_len_vec[i]*platePtrArr[i]->Thick;	//默认偏移一个板厚
	}
	//3.3 在1、2号钢板上分别求取一条最外侧射线角钢,计算基准面法线方向
	GEPOINT btm_edge_vec=vertex_arr[1]-vertex_arr[0];
	normalize(btm_edge_vec);
	typedef CDesignLjPartPara* LJPART_PTR;
	GEPOINT wing_norm[2],wing_vec[2],ray_vec[2];
	LJPART_PTR ljPartPtrArr[2]={NULL,NULL};
	for(int i=0;i<2;i++)
	{
		CMaxDouble maxAngle;
		CDesignLjPartPara *pLjPara=NULL,*pNewLjPara=NULL;
		for(pLjPara=platePtrArr[i]->GetFirstLjPara();pLjPara;pLjPara=platePtrArr[i]->GetNextLjPara())
		{
			CLDSLinePart *pRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pLjPara->hPart,CLS_LINEANGLE);
			if(pRod==NULL||pRod->handle==platePtrArr[i]->designInfo.m_hBasePart)
				continue;
			pLjPara->pCurLinePart=pRod;
			f3dPoint ray_vec=(pLjPara->start0_end1==0)?(pRod->End()-pRod->Start()):(pRod->Start()-pRod->End());
			normalize(ray_vec);
			double angle=cal_angle_of_2vec(tube_len_vec[i],ray_vec);
			maxAngle.Update(angle,pLjPara);
		}
		pLjPara=(CDesignLjPartPara*)maxAngle.m_pRelaObj;
		if(pLjPara&&pLjPara->pCurLinePart&&pLjPara->pCurLinePart->IsAngle())
		{
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pLjPara->pCurLinePart;
			wing_norm[i]=(pLjPara->angle.cur_wing_x0_y1==0)?pAngle->vxWingNorm:pAngle->vyWingNorm;
			wing_vec[i]=(pLjPara->angle.cur_wing_x0_y1==0)?pAngle->GetWingVecX():pAngle->GetWingVecY();
			ray_vec[i]=(pLjPara->start0_end1==0)?(pAngle->End()-pAngle->Start()):(pAngle->Start()-pAngle->End());
			ljPartPtrArr[i]=pLjPara;
		}
		else
			return FALSE;
	}
	//3.4 修正基准面第一、二个轮廓点
	//获取基准角钢索引，根据基准角钢计算基准线
	int datum_index=0,other_index=0;
	if(wing_norm[0]*wing_norm[1]>0)
	{	//同里同外，根据初始火曲拾取点走向获取凹角火曲贴合角钢
		//使用vertex_arr不够准确(vertex_arr[1]-vertex_arr[0]接近垂直wing_norm时)可能误判,现改用datum_vertex wht 16-10-14
		GEPOINT verfiy_vec=datum_vertex[1]-datum_vertex[0];	
		normalize(verfiy_vec);
		datum_index=(verfiy_vec*wing_norm[0]>0)?1:0;
	}
	else
	{	//一里一外,默认取负头较小的角钢为基准角钢
		/*CLDSLinePart *pRod1=ljPartPtrArr[0]->pCurLinePart,*pRod2=ljPartPtrArr[1]->pCurLinePart;
		double odd1=DISTANCE(pRod1->Start(),vertex_arr[0])>DISTANCE(pRod1->End(),vertex_arr[0])?pRod1->endOdd():pRod1->startOdd();
		double odd2=DISTANCE(pRod2->Start(),vertex_arr[1])>DISTANCE(pRod2->End(),vertex_arr[1])?pRod2->endOdd():pRod2->startOdd();
		datum_index=odd1>odd2?0:1;	//odd1,odd2一般为负值*/
		//取与基准钢管接近垂直的角钢作为制弯基准角钢 wht 16-10-19
		if(fabs(ray_vec[0]*tube_len_vec[0])<fabs(ray_vec[1]*tube_len_vec[1]))
			datum_index=0;
		else
			datum_index=1;
	}
	other_index=1-datum_index;
	//
	const double HUOQU_SPACE=library->GetHuoQuClearance();;
	f3dLine datum_line(ljPartPtrArr[datum_index]->pCurLinePart->Start(),ljPartPtrArr[datum_index]->pCurLinePart->End());
	if(wing_vec[datum_index]*tube_len_vec[datum_index]<0)
	{
		datum_line.startPt+=wing_vec[datum_index]*(ljPartPtrArr[datum_index]->pCurLinePart->GetWidth()+HUOQU_SPACE);
		datum_line.endPt+=wing_vec[datum_index]*(ljPartPtrArr[datum_index]->pCurLinePart->GetWidth()+HUOQU_SPACE);
	}
	else
	{
		datum_line.startPt-=wing_vec[datum_index]*HUOQU_SPACE;
		datum_line.endPt-=wing_vec[datum_index]*HUOQU_SPACE;
	}
	//基准线与底边求交计算基准轮廓点
	f3dPoint inters_pt,datum_bolt_pos;
	double fMinDist=0;
	int nRetCode=Int3dDifferPlaneLL(btm_line_arr[datum_index].startPt,btm_line_arr[datum_index].endPt,datum_line.startPt,datum_line.endPt,inters_pt);
	CLDSLinePart *pRod=ljPartPtrArr[other_index]->pCurLinePart;
	bool bCurrConnStart=DISTANCE(pRod->Start(),vertex_arr[other_index])<DISTANCE(pRod->End(),vertex_arr[other_index]);
	GEPOINT xNearRodVec=bCurrConnStart?pRod->End()-pRod->Start():pRod->Start()-pRod->End();
	if(nRetCode>0||nRetCode==-2)
	{
		vertex_arr[datum_index]=inters_pt;
		Int3dlf(datum_vertex[other_index],btm_line_arr[other_index],vertex_arr[datum_index],tube_len_vec[other_index]);	//cut_plane_norm
		vertex_arr[other_index]=datum_vertex[other_index]+tube_len_vec[other_index]*platePtrArr[other_index]->Thick;
		//取另外一根角钢端头螺栓，保证火曲线与圆不相交
		CMinDouble minValue;
		GEPOINT datum_pos=!bCurrConnStart?pRod->End():pRod->Start();
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pRod->FindLsByHandle((*pLsRef)->handle)==NULL)
				continue;
			pRod->GetBoltIntersPos(pLsRef->GetLsPtr(),inters_pt);
			minValue.Update(DISTANCE(inters_pt,datum_pos),pLsRef->GetLsPtr());
		}
		if(minValue.IsInited())
		{
			CLDSBolt *pBolt=(CLDSBolt*)minValue.m_pRelaObj;
			double D=0,H=0,H2=0;
			FindLsD_H(pBolt->get_d(),&D,&H,&H2);
			pRod->GetBoltIntersPos(pBolt,datum_bolt_pos);
			//螺栓位置转到other_index对应的钢板坐标系下
			coord_trans(datum_bolt_pos,platePtrArr[other_index]->ucs,FALSE);
			datum_bolt_pos.z=0;
			fMinDist=D*0.5+HUOQU_SPACE;
		}
	}
	const double STEP_LEN=0.5;
	const double MIN_OFFSET_LEN=0.1;//0.2*platePtrArr[other_index]->Thick;
	double fCurOffsetLen=platePtrArr[other_index]->Thick;
	f3dPoint plate_norm;
	f3dPoint huoqu_start,huoqu_end;
	while(true)
	{	
		btm_edge_vec=vertex_arr[1]-vertex_arr[0];
		normalize(btm_edge_vec);
		//3.5 计算基准面第三、四个轮廓点
		f3dPoint huoqu_vec_arr[2];
		int index_arr[2]={datum_index,1-datum_index};
		for(int j=0;j<2;j++)
		{
			int i=index_arr[j];
			f3dLine line;
			line.startPt=GetPlateHighestVertex(platePtrArr[i],spreadVecArr[i]);
			line.endPt=line.startPt+tube_len_vec[i]*1000;
			line.startPt-=tube_len_vec[i]*1000;
			if(i==datum_index)
				huoqu_vec_arr[i]=datum_line.endPt-datum_line.startPt;
			else
				huoqu_vec_arr[i]=plate_norm^platePtrArr[i]->ucs.axis_z;
			normalize(huoqu_vec_arr[i]);
			nRetCode=Int3dDifferPlaneLL(line.startPt,line.endPt,vertex_arr[i]-huoqu_vec_arr[i]*1000,vertex_arr[i]+huoqu_vec_arr[i]*1000,vertex_arr[3-i]);
			if(i==datum_index)
			{	//计算基准面法线方向
				plate_norm=btm_edge_vec^huoqu_vec_arr[i];
				if(plate_norm*platePtrArr[i]->ucs.axis_z<0)
					plate_norm=-plate_norm;
				normalize(plate_norm);
			}
		}
		huoqu_start=vertex_arr[other_index];
		huoqu_end=vertex_arr[other_index==0?3:2];
		GEPOINT huoquvec=huoqu_end-huoqu_start;
		GEPOINT verifyvec=huoquvec^xNearRodVec;
		bool bRodOnLeft=verifyvec*platePtrArr[other_index]->ucs.axis_z>0;
		coord_trans(huoqu_start,platePtrArr[other_index]->ucs,FALSE);
		coord_trans(huoqu_end,platePtrArr[other_index]->ucs,FALSE);
		huoqu_start.z=huoqu_end.z=0;
		double fCurDist=DistOf2dPtLine(datum_bolt_pos,huoqu_start,huoqu_end);
		if(!bRodOnLeft)
			fCurDist*=-1;
		//3.6 检测火曲线是否与螺栓孔碰撞
		if(fMinDist>0&&(fCurOffsetLen-STEP_LEN)>MIN_OFFSET_LEN&&(fCurDist<0||fCurDist<fMinDist))
		{
			fCurOffsetLen-=STEP_LEN;
			vertex_arr[other_index]=datum_vertex[other_index]+tube_len_vec[other_index]*fCurOffsetLen;
		}
		//安徽宏源要求火曲线尽量远离螺栓孔,以下代码用于调整火曲线靠近螺栓孔避免出现Z字型制弯，暂时注释 wht 16-10-12
		/*else if(fMinDist>0&&fCurDist<0&&fabs(fCurDist)>(fMinDist+5))
		{
			fCurOffsetLen+=STEP_LEN;
			vertex_arr[other_index]=datum_vertex[other_index]+tube_len_vec[other_index]*fCurOffsetLen;
		}*/
		else
			break;
	}
	//4.更新钢板坐标系
	ucs.origin=0.5*(vertex_arr[1]+vertex_arr[0]);
	ucs.axis_z=plate_norm;
	ucs.axis_x=btm_edge_vec;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_y);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	normalize(ucs.axis_x);
	if(ucs.axis_y*spreadVecArr[0]<0)
	{	// 保证合并之后的钢板Y轴方向与原钢板延伸方向一致
		ucs.axis_y*=-1;
		ucs.axis_z*=-1;
		for(int i=0;i<2;i++)
		{
			if(ucs.axis_z*huoqufacenorm[i]<0)
			{
				designInfo.huoqufacenorm[i].ReverseDirection();
				designInfo.huoqufacenorm[i].UpdateVector(BelongModel());
				GEPOINT temp_huoqufacenorm=designInfo.huoqufacenorm[i].vector;
				if(anglePtrArr[i]&&fabs(offsetAngleArr[i])>EPS)
				{
					if(temp_huoqufacenorm*huoqufacenorm[i]<0)
						huoqufacenorm[i]*=-1.0;
				}	
				else
					huoqufacenorm[i]=temp_huoqufacenorm;
			}
		}
	}
	//5.更新钢板轮廓点链表，修正顶部轮廓标保证无凹点
	vertex_list.Empty();
	for(int i=0;i<4;i++)
	{
		coord_trans(vertex_arr[i],ucs,FALSE);
		vertex_arr[i].z=0;
		vertex_arr[i].feature=(i==1||i==2)?12:13;
	}
	//
	f3dLine top_edge_line1,top_edge_line2;
	vertex_list.append((PROFILE_VERTEX(vertex_arr[0])));
	vertex_list.append((PROFILE_VERTEX(vertex_arr[1])));
	//TODO:2号火曲面为右侧面,3号火曲面为左侧面,不知是否正确?
	//将第二块钢板轮廓点添加至轮廓点列表中
	top_edge_line1=AddPlateVertexsToList(vertex_list,vertex_arr,ucs,2,
		platePtrArr[1],keyVertexPtr[1],reserveOrderArr[1]);
	PROFILE_VERTEX *pTopVertex1=vertex_list.append((PROFILE_VERTEX(vertex_arr[2])));
	PROFILE_VERTEX *pTopVertex2=vertex_list.append((PROFILE_VERTEX(vertex_arr[3])));
	//将第一块钢板轮廓点添加至轮廓点列表中
	top_edge_line2=AddPlateVertexsToList(vertex_list,vertex_arr,ucs,3,
		platePtrArr[0],keyVertexPtr[0],reserveOrderArr[0]);
	//bDeleteLine1End=TRUE表示top_edge_line1.endPt为凹点，bDeleteLine2Start=TRUE表示top_edge_line2.startPt为凹点
	BOOL bDeleteLine1End=FALSE,bDeleteLine2Start=FALSE;	
	f3dLine top_edge_line(top_edge_line1.startPt,top_edge_line2.endPt);	
	double fLine1EndState=DistOf2dPtLine(top_edge_line1.endPt,top_edge_line.startPt,top_edge_line.endPt);
	double fLine2StartState=DistOf2dPtLine(top_edge_line2.startPt,top_edge_line.startPt,top_edge_line.endPt);
	if( fLine1EndState<0&&fLine2StartState<0)	//在顶部轮廓边右侧
	{
		top_edge_line.startPt=top_edge_line1.endPt;
		top_edge_line.endPt=top_edge_line2.startPt;
	}
	else if(fLine2StartState<0)
	{
		top_edge_line.startPt=top_edge_line1.startPt;
		top_edge_line.endPt=top_edge_line2.startPt;
		bDeleteLine1End=TRUE;
	}
	else if(fLine1EndState<0)
	{
		top_edge_line.startPt=top_edge_line1.endPt;
		top_edge_line.endPt=top_edge_line2.endPt;
		bDeleteLine2Start=TRUE;
	}
	//删除与top_edge_line1.endPt\top_edge_line2.startPt对应的轮廓点,不删除火曲制弯点
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if( pVertex->vertex.feature<10&&
			((bDeleteLine1End&&pVertex->vertex.IsEqual(top_edge_line1.endPt))||
			 (bDeleteLine2Start&&pVertex->vertex.IsEqual(top_edge_line2.startPt))))
			vertex_list.DeleteCursor(FALSE);
	}
	vertex_list.Clean();
	//更新火曲线终点坐标
	nRetCode=Int3dll(top_edge_line,f3dLine(vertex_arr[0],vertex_arr[3]),vertex_arr[3]);
	vertex_arr[3].z=0;	//TODO:什么时候会导致.z!=0?
	if(nRetCode!=0&&nRetCode!=-1)
		pTopVertex2->vertex=vertex_arr[3];
	nRetCode=Int3dll(top_edge_line,f3dLine(vertex_arr[1],vertex_arr[2]),vertex_arr[2]);
	vertex_arr[2].z=0;
	if(nRetCode!=0&&nRetCode!=-1)
		pTopVertex1->vertex=vertex_arr[2];
	//火曲线
	for(int i=0;i<2;i++)
	{	//火曲线
		designInfo.huoquline_start[i].datum_pos_style=0;
		designInfo.huoquline_start[i].datum_pos=vertex_arr[(i*2+1)%4];
		designInfo.huoquline_start[i].SetPosition(vertex_arr[(i*2+1)%4]);
		designInfo.huoquline_end[i].datum_pos_style=0;
		designInfo.huoquline_end[i].datum_pos=vertex_arr[(i*2+2)%4];
		designInfo.huoquline_end[i].SetPosition(vertex_arr[(i*2+2)%4]);
		HuoQuLine[i].startPt=vertex_arr[(i*2+1)%4];
		HuoQuLine[i].endPt=vertex_arr[(i*2+2)%4];
	}
	DesignProfile2();	//之前合并顶点中可能存在plate[1]的新形成的凹点残余，需要重新去除
	//设置焊缝边及对活点
	PROFILE_VERTEX *pBtm1PrevVertex=vertex_list.GetTail(),*pBtmVertex1=NULL,*pBtmVertex2=NULL,*pBtm2NextVertex=NULL;
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->vertex.feature>10&&pVertex->vertex.IsEqual(vertex_arr[0]))
		{
			pBtmVertex1=pVertex;
			pBtmVertex2=vertex_list.GetNext();
			if(pBtmVertex2==NULL)
				pBtmVertex2=vertex_list.GetFirst();
			pBtm2NextVertex=vertex_list.GetNext();
			if(pBtm2NextVertex==NULL)
				pBtm2NextVertex=vertex_list.GetFirst();
			break;
		}
		pBtm1PrevVertex=pVertex;
	}
	if(pBtm1PrevVertex&&pBtmVertex1&&pBtmVertex2&&pBtm2NextVertex)
	{	//设置焊缝边及对活点 wht 16-11-03
		pBtm1PrevVertex->m_bWeldEdge=pBtmVertex1->m_bWeldEdge=pBtmVertex2->m_bWeldEdge=TRUE;
		GEPOINT btm_line_vec=GetRealVertex(pBtm2NextVertex->vertex)-GetRealVertex(pBtm1PrevVertex->vertex);
		normalize(btm_line_vec);
		vector_trans(btm_line_vec,ucs,TRUE);
		GEPOINT base_tube_vec=pTube->End()-pTube->Start();
		normalize(base_tube_vec);
		double dd=btm_line_vec*base_tube_vec;
		if(datum_index==0)
		{	//在pBtm1PrevVertex=>pBtmVertex1边上设置对话点
			pBtm1PrevVertex->local_point_vec=1;
			pBtm1PrevVertex->local_point_y=dd>0?-100:100;
		}
		else //if(datum_index==1)
		{	//在pBtmVertex2=>pBtm2NextVertex边上设置对话点
			pBtmVertex2->local_point_vec=1;
			pBtmVertex2->local_point_y=dd>0?100:-100;
		}
	}
	SetModified();
	return true;
}
//设计变电构件八字折叠板
bool CLDSPlate::DesignSubstationSplayFoldPlate()
{
	if(designInfo.iFaceType!=7)
		return false;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(designInfo.m_hBasePart,CLS_LINETUBE);
	if(pDatumTube==NULL)
		return false;
	if(designInfo.partList.GetNodeNum()==0)
		return false;	//无射线杆件，无法进行重新设计 wht 17-11-22
	//0.先设置为单面板计算出轮廓点
	designInfo.iFaceType=1;
	face_N=1;
	DesignPlate();
	designInfo.iFaceType=7;
	face_N=3;
	//1.计算基准线方向
	GEPOINT spread_vec=pDatumTube->GetPlateVertSpreadVec(this);
	PROFILE_VERTEX *pStartVertex=NULL,*pEndVertex=NULL;
	if(!GetPlateBottomEdgeProfileVertex(this,spread_vec,pDatumTube->ucs.axis_z,&pStartVertex,&pEndVertex))
		return false;
	GEPOINT btm_edge_vec=pEndVertex->vertex-pStartVertex->vertex;
	normalize(btm_edge_vec);
	vector_trans(btm_edge_vec,ucs,TRUE);
	GEPOINT datum_line_vec=(btm_edge_vec*pDatumTube->ucs.axis_z>0)?pDatumTube->ucs.axis_z:pDatumTube->ucs.axis_z*-1.0;
	//2.查找左右两侧最外侧的射线杆件、用于计算火曲面法线的基准角钢
	CMinDouble minAngle,minCosa;
	CMaxDouble maxAngle;
	for(CDesignLjPartPara *pLjPara=GetFirstLjPara();pLjPara;pLjPara=GetNextLjPara())
	{
		CLDSLinePart *pRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pLjPara->hPart,CLS_LINEANGLE);
		if(pRod==NULL||pRod->handle==designInfo.m_hBasePart||!pRod->IsAngle())
			continue;
		pLjPara->pCurLinePart=pRod;
		f3dPoint ray_vec=(pLjPara->start0_end1==0)?(pRod->End()-pRod->Start()):(pRod->Start()-pRod->End());
		normalize(ray_vec);
		double angle=cal_angle_of_2vec(datum_line_vec,ray_vec);
		maxAngle.Update(angle,pRod);
		minAngle.Update(angle,pRod);
		double cosa=fabs(datum_line_vec*ray_vec);
		minCosa.Update(cosa,pRod);
	}
	if(!minCosa.IsInited())
		return false;
	//3. 计算火曲线位置
	GEPOINT huoqu_line_start;
	if(fabs(spread_vec*ucs.axis_x)>fabs(spread_vec*ucs.axis_y))		//X轴垂直钢管
		huoqu_line_start.Set(pStartVertex->vertex.x,0,0);
	else												//Y轴垂直钢管
		huoqu_line_start.Set(0,pStartVertex->vertex.y,0);
	//3.2 根据角钢计算火曲终点位置
	GEPOINT huoqu_line_end[2];
	typedef CLDSLineAngle* ANGLEROD_PTR;
	ANGLEROD_PTR anglePtrArr[2]={(CLDSLineAngle*)minAngle.m_pRelaObj,(CLDSLineAngle*)maxAngle.m_pRelaObj};
	GEPOINT datumVecArr[2]={datum_line_vec,-datum_line_vec};
	GEPOINT wingNormArr[2],wingVecArr[2];
	BOOL outterBendArr[2]={TRUE,FALSE};	//TRUE:向法线正方向弯曲,FALSE:向方向负方向弯曲
	if(datum_line_vec*ucs.axis_z<0)
	{
		outterBendArr[0]=FALSE;
		outterBendArr[1]=TRUE;
	}
	const int HUOQU_SPACE=library->GetHuoQuClearance();
	for(int i=0;i<2;i++)
	{
		GEPOINT datum_pos=anglePtrArr[i]->Start()-anglePtrArr[i]->ucs.axis_z*anglePtrArr[i]->startOdd();
		if(DISTANCE(anglePtrArr[i]->Start(),ucs.origin)>DISTANCE(anglePtrArr[i]->End(),ucs.origin))
			datum_pos=anglePtrArr[i]->End()-anglePtrArr[i]->ucs.axis_z*anglePtrArr[i]->endOdd();
		wingNormArr[i]=anglePtrArr[i]->vyWingNorm;
		wingVecArr[i]=anglePtrArr[i]->GetWingVecY();
		if(fabs(anglePtrArr[i]->vxWingNorm*ucs.axis_z)>fabs(anglePtrArr[i]->vyWingNorm*ucs.axis_z))
		{
			wingNormArr[i]=anglePtrArr[i]->vxWingNorm;
			wingVecArr[i]=anglePtrArr[i]->GetWingVecX();
		}
		BOOL bMaxG=FALSE;
		huoqu_line_end[i]=datum_pos;
		//TODO:这里感觉判断有问题
		if(wingVecArr[i]*datumVecArr[i]>0)
		{
			huoqu_line_end[i]=datum_pos+wingVecArr[i]*(anglePtrArr[i]->GetWidth()+HUOQU_SPACE);
			bMaxG=TRUE;
		}
		else 
			huoqu_line_end[i]=datum_pos-wingVecArr[i]*HUOQU_SPACE;
		//
		BOOL bOutterAngle=(wingNormArr[i]*ucs.axis_z<0);
		if(outterBendArr[i]!=bOutterAngle)
		{	//外曲&&里铁或者内曲&&外铁,按螺栓查找制弯点
			f3dPoint bolt_pos;
			ARRAY_LIST<BOLT_EX> boltList;
			for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				if(anglePtrArr[i]->FindLsByHandle((*pLsRef)->handle)==NULL)
					continue;
				double g=0,dist=0;
				anglePtrArr[i]->GetBoltIntersPos(pLsRef->GetLsPtr(),bolt_pos);
				SnapPerp(&bolt_pos,anglePtrArr[i]->Start(),anglePtrArr[i]->End(),bolt_pos,&g);
				dist=DISTANCE(bolt_pos,datum_pos);
				boltList.append(BOLT_EX(pLsRef->GetLsPtr(),GEPOINT(g,0,dist)));
			}
			BOLT_EX *pDatumBoltEx=NULL;
			if(boltList.GetSize()>1)
			{
				CHeapSort<BOLT_EX>::HeapSort(boltList.m_pData,boltList.GetSize(),CompareBoltEx);
				BOLT_EX *pFirstBoltEx=boltList.GetFirst(),*pNextBoltEx=boltList.GetNext();
				pDatumBoltEx=pFirstBoltEx;
				const int G_EPS=5;//TODO:注释不清，逻辑不够严谨
				double differ_g=pNextBoltEx->pos.x-pFirstBoltEx->pos.x;
				if((bMaxG&&differ_g>G_EPS)||(!bMaxG&&differ_g<-G_EPS))
					pDatumBoltEx=pNextBoltEx;
			}
			else
				pDatumBoltEx=boltList.GetFirst();
			if(pDatumBoltEx)
			{	//按基准螺栓计算火曲点位置
				anglePtrArr[i]->GetBoltIntersPos(pDatumBoltEx->pBolt,huoqu_line_end[i]);
				GEPOINT huoquStart=huoqu_line_start;
				coord_trans(huoquStart,ucs,TRUE);
				GEPOINT offset_vec=huoqu_line_end[i]-huoquStart;
				normalize(offset_vec);
				RotateVectorAroundVector(offset_vec,1,0,ucs.axis_z);
				if(offset_vec*datumVecArr[i]<0)
					offset_vec*=-1.0;
				double D=0,H=0,H2=0;
				FindLsD_H(pDatumBoltEx->pBolt->get_d(),&D,&H,&H2);
				huoqu_line_end[i]+=offset_vec*(D+HUOQU_SPACE);
			}
		}
		//
		coord_trans(huoqu_line_end[i],ucs,FALSE);
		huoqu_line_end[i].z=0;
	}	
	//3.3 设置火曲线及火曲法线参数
	top_point=f3dPoint(huoqu_line_start);
	GEPOINT huoqu_line_vec[2];
	BOOL faceValidFlagArr[2]={TRUE,TRUE};
	for(int i=0;i<2;i++)
	{	//火曲线
		designInfo.huoquline_start[i].datum_pos_style=0;
		designInfo.huoquline_start[i].datum_pos=huoqu_line_start;
		designInfo.huoquline_start[i].SetPosition(huoqu_line_start);
		designInfo.huoquline_end[i].datum_pos_style=0;
		designInfo.huoquline_end[i].datum_pos=huoqu_line_end[i];
		designInfo.huoquline_end[i].SetPosition(huoqu_line_end[i]);
		HuoQuLine[i].startPt=huoqu_line_start;
		HuoQuLine[i].endPt=huoqu_line_end[i];
		//计算火曲线方向
		huoqu_line_vec[i]=huoqu_line_end[i]-huoqu_line_start;
		normalize(huoqu_line_vec[i]);
		vector_trans(huoqu_line_vec[i],ucs,TRUE);
		huoqufacenorm[i]=datum_line_vec^huoqu_line_vec[i];
		normalize(huoqufacenorm[i]);
		//TODO:逻辑上直接根据顺序号判断火曲面正方向更严谨，虽然目前这样也不会错
		if(huoqufacenorm[i]*ucs.axis_z<0)
			huoqufacenorm[i]*=-1;
		if(huoqu_line_vec[i]*spread_vec<0||fabs(huoqu_line_vec[i]*datumVecArr[i])>EPS_COS2)
			faceValidFlagArr[i]=FALSE;
	}
	//4.根据火曲线设置轮廓点面号
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		double dist1=DistOf2dPtLine(pVertex->vertex,HuoQuLine[0].startPt,HuoQuLine[0].endPt);
		double dist2=DistOf2dPtLine(pVertex->vertex,HuoQuLine[1].startPt,HuoQuLine[1].endPt);
		if(dist1<0)
			pVertex->vertex.feature=2;
		if(dist2>0)
			pVertex->vertex.feature=3;
	}
	//5.修正底边轮廓点坐标
	f3dPoint datumVertex=huoqu_line_start;
	coord_trans(datumVertex,ucs,TRUE);
	double start_offset_len=pStartVertex->vertex.y,end_offset_len=pEndVertex->vertex.y;
	f3dPoint datum_axis=ucs.axis_y;
	if(fabs(spread_vec*ucs.axis_x)<fabs(spread_vec*ucs.axis_y))		//Y轴垂直钢管
	{
		datum_axis=ucs.axis_x;
		start_offset_len=pStartVertex->vertex.x;
		end_offset_len=pEndVertex->vertex.x;
	}
	if(datum_axis*datum_line_vec<0)
		datum_line_vec*=-1.0;
	GEPOINT startVertex=datumVertex+datum_line_vec*start_offset_len;
	GEPOINT endVertex=datumVertex+datum_line_vec*end_offset_len;
	coord_trans(startVertex,ucs,FALSE);
	coord_trans(endVertex,ucs,FALSE);
	if(pStartVertex->vertex.feature>1||pEndVertex->vertex.feature>1)
	{
		pStartVertex->vertex=GetSpreadVertex(startVertex,pStartVertex->vertex.feature,true);
		pEndVertex->vertex=GetSpreadVertex(endVertex,pEndVertex->vertex.feature,true);
		pStartVertex->vertex.z=pEndVertex->vertex.z=0;
	}
	PROFILE_VERTEX startProfileVertex,endProfileVertex;
	if(pStartVertex)
		startProfileVertex=*pStartVertex;
	if(pEndVertex)
		endProfileVertex=*pEndVertex;
	//
	DesignProfile2();
	//计算钢板外形之后，再次判断钢板面数
	//TODO:应作为系统参数开放供用户设定,最其码可以作为用户定制参数
	const int MIN_EDGE_LEN=50;	//制弯面钢板边长小于MIN_EDGE_LEN时不需要制弯
	PROFILE_VERTEX *pVertex12=NULL,*pVertex13=NULL;
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->vertex.feature==12)
			pVertex12=pVertex;
		else if(pVertex->vertex.feature==13)
			pVertex13=pVertex;
	}
	PROFILE_VERTEX *pVertex2=NULL,*pVertex3=NULL;
	if(startProfileVertex.vertex.feature==2)
		pVertex2=&startProfileVertex;
	else if(startProfileVertex.vertex.feature==3)
		pVertex3=&startProfileVertex;
	if(endProfileVertex.vertex.feature==2)
		pVertex2=&endProfileVertex;
	else if(endProfileVertex.vertex.feature==3)
		pVertex3=&endProfileVertex;
	if( pVertex12==NULL||
		(pVertex12&&pVertex2&&DISTANCE(pVertex12->vertex,pVertex2->vertex)<MIN_EDGE_LEN))
		faceValidFlagArr[0]=FALSE;
	if( pVertex13==NULL||
		(pVertex13&&pVertex3&&DISTANCE(pVertex13->vertex,pVertex3->vertex)<MIN_EDGE_LEN))
		faceValidFlagArr[1]=FALSE;
	//调整钢板面数及设计参数
	long nFaceN=face_N;
	if(!faceValidFlagArr[0]&&!faceValidFlagArr[1])
		nFaceN=1;
	else if(!faceValidFlagArr[0]||!faceValidFlagArr[1])
	{
		nFaceN=2;
		//用户需要修改架构八字板类型(修改钢板面数)时，需通过类型wiType进行修改 wht 17-11-22
		//nFaceN=min(2,nOldFaceN);
		//if(nOldFaceN==1)	//用户指定为单面板
		//	faceValidFlagArr[0]=faceValidFlagArr[1]=FALSE;
	}
	if(nFaceN!=3)
	{	//钢板面数发生变化调整设计参数(三面板->双面或双面板->单面板) wht 16-11-02
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.feature>=nFaceN)
			{
				if(nFaceN==1||(nFaceN==2&&!faceValidFlagArr[1]))
				{	//单面板或双面板(第三面不可用,第三面轮廓点蜕变为第一面轮廓点)
					if(pVertex->vertex.feature>10)
					{
						if(pVertex->vertex.feature==13||nFaceN==1)
							vertex_list.DeleteCursor();
					}
					else if(pVertex->vertex.feature>nFaceN)
					{
						pVertex->vertex=GetRealVertex(pVertex->vertex,true);
						pVertex->vertex.feature=1;
					}
				}
				else //if(nFaceN==2&&!faceValidFlagArr[0])
				{	//双面板(第二面不可用，第三面变为第二面)
					if(pVertex->vertex.feature==12)
						vertex_list.DeleteCursor();
					else
					{
						if(pVertex->vertex.feature==2)
							pVertex->vertex=GetRealVertex(pVertex->vertex,true);
						pVertex->vertex.feature-=1;
					}
				}
			}
		}
		vertex_list.Clean();
		face_N=nFaceN;
		if(face_N==2&&!faceValidFlagArr[0])
		{	
			designInfo.huoquline_start[0]=designInfo.huoquline_start[1];
			designInfo.huoquline_end[0]=designInfo.huoquline_end[1];
			designInfo.huoqufacenorm[0]=designInfo.huoqufacenorm[1];
			HuoQuLine[0]=HuoQuLine[1];
			huoqufacenorm[0]=huoqufacenorm[1];
		}
	}
	if(face_N!=3)
		DesignProfile2();
	SetModified();
	return true;
}

bool CLDSPlate::DesignPlate()
{
	if(IsShadowObject())	//影像构件
		return ShadowDesign();
	DesignSetupUcs();	//提前以便未启动柔性设计的板其装配位置也可自动计算更新 wjh-2015.9.10
	if(!designInfo.m_bEnableFlexibleDesign)
		return FALSE;	//未启用连板柔性设计
	if(IsTubeFoldPlate()&&face_N==3)
		return DesignTubeFoldPlate();	//设计钢管折叠板
	else if(IsSplayFoldPlate())	//八字板可能为单面板、双面板、三面板
		return DesignSubstationSplayFoldPlate();	//设计变电构件八字折叠板
	//不移前有时对称的火曲板可能(基准角钢不严格对称)会出现火曲线与顶点不同步导致外形火曲线显示出错　wjh-2015.9.10
	//f3dPoint huoqu_line_vec[2];	//两火曲线方向
	for(int i=1;i<face_N;i++)
	{
		designInfo.huoquline_start[i-1].UpdatePos(BelongModel());
		designInfo.huoquline_end[i-1].UpdatePos(BelongModel());
		//datum_pos_style=0时为相对坐标不用进行坐标转换,designInfo.m_bSelfCorrectBendline==TRUE时统一为绝对坐标 wht 16-06-02
		if(designInfo.m_bSelfCorrectBendline||designInfo.huoquline_start[i-1].datum_pos_style!=0)
		{
			HuoQuLine[i-1].startPt=designInfo.huoquline_start[i-1].Position();
			coord_trans(HuoQuLine[i-1].startPt,ucs,FALSE);
		}
		if(designInfo.m_bSelfCorrectBendline||designInfo.huoquline_end[i-1].datum_pos_style!=0)
		{
			HuoQuLine[i-1].endPt=designInfo.huoquline_end[i-1].Position();
			coord_trans(HuoQuLine[i-1].endPt,ucs,FALSE);
		}
		if(designInfo.m_bSelfCorrectBendline)
		{	//暂时只纠正普通双面板及折叠三面板火曲线	
			GEPOINT vBendPlaneNorm=huoqufacenorm[i-1];
			vector_trans(vBendPlaneNorm,ucs,FALSE,TRUE);
			GEPOINT linePick,vBendAxisVec(-vBendPlaneNorm.y,vBendPlaneNorm.x,0);
			//计算两面交线上的一拾取点
			_Int3d2f(linePick,NULL,GEPOINT(0,0,0),GEPOINT(0,0,1),HuoQuLine[i-1].startPt,vBendPlaneNorm);
			/*以下代码已融入到_Int3d2f中
			GEPOINT prjvec(vBendPlaneNorm.x,vBendPlaneNorm.y); 
			normalize(prjvec);
			double dd = prjvec*vBendPlaneNorm;
			GEPOINT original_vec = HuoQuLine[i-1].startPt;
			GEPOINT linePick = prjvec*((original_vec*vBendPlaneNorm)/dd);
			*/
			double dist,scale_s2e=0;
			dist=DistOf2dPtLine(HuoQuLine[i-1].startPt.x,HuoQuLine[i-1].startPt.y,
				linePick.x,linePick.y,linePick.x+vBendAxisVec.x,linePick.y+vBendAxisVec.y,&scale_s2e);
			if(fabs(dist)>EPS)	//火曲线与火曲面法线偏差较大，需要纠正
				HuoQuLine[i-1].startPt=linePick+vBendAxisVec*scale_s2e;
			HuoQuLine[i-1].endPt=HuoQuLine[i-1].startPt+vBendAxisVec*100;	//火曲线终端初始值
			dist=DistOf2dPtLine(HuoQuLine[i-1].endPt.x,HuoQuLine[i-1].endPt.y,
				linePick.x,linePick.y,linePick.x+vBendAxisVec.x,linePick.y+vBendAxisVec.y,&scale_s2e);
			if(fabs(dist)>EPS)	//火曲线与火曲面法线偏差较大，需要纠正
				HuoQuLine[i-1].endPt=linePick+scale_s2e*vBendAxisVec;
		}
		if(this->IsPeakPointPlate())//designInfo.iFaceType==3)	//普通三面板
		{
			GEPOINT bendlinevec0=HuoQuLine[0].endPt-HuoQuLine[0].startPt;
			GEPOINT bendlinevec1=HuoQuLine[1].endPt-HuoQuLine[1].startPt;
			bendlinevec0.z=bendlinevec1.z=0;
			normalize(bendlinevec0);
			normalize(bendlinevec1);
			if(fabs(bendlinevec0*bendlinevec1)>EPS_COS2)
			{	//两火曲线平行 wjh-2018.1.17
				SnapPerp(&top_point,HuoQuLine[0],f3dPoint(0,0,0));
			}
			else
			{
				int nRetCode=Int2dll(f2dLine(HuoQuLine[0].startPt,HuoQuLine[0].endPt),
					f2dLine(HuoQuLine[1].startPt,HuoQuLine[1].endPt),top_point.x,top_point.y);
				if(nRetCode<=0)	//两火曲线求交失败，以原点向火曲线投影点作为top_point
					SnapPerp(&top_point,HuoQuLine[0],f3dPoint(0,0,0));
			}
			top_point.z=0;
		}
		HuoQuLine[i-1].startPt.z=0;
		HuoQuLine[i-1].endPt.z=0;
	}
	//自定义外形的钢板进行顶点坐标的更新（之前是包络外形的不进行更新）
	if(vertex_list.GetNodeNum()>3&&designInfo.iProfileStyle0123==3)
		return UpdateProfileVertexAndPos();
	if(jdb_style==4)
		return CalStdProfile()!=FALSE;
	double stress_diffuse_angle=CLDSPart::library->GetStressDiffuseAngle();
 	f3dPoint vertex_up_offset,vertex_down_offset;
	f3dPoint perp_up_offset,perp_down_offset;
	CLDSPart *pPart=BelongModel()->FromPartHandle(designInfo.m_hBasePart);
	CSuperSmartPtr<CLDSLinePart> pBaseLinePart,pCurLinePart;
	CLDSLineAngle agentBaseAngle;
	if(pPart&&pPart->IsArcPart())
		pPart=agentBaseAngle.Fabricate(pPart);
	if(pPart&&pPart->IsLinePart())
		pBaseLinePart=(CLDSLinePart*)pPart;
	else
		return FALSE;
	if(IsHuoQuLineCollinear3FacePlate())
	{	//重新设计火曲线共线三面板前，先提取存在轮廓点中的设计参数 wht 18-12-18
		_extendVertexOffsetLen=getExtendVertexOffsetLen();
		_peakPointMargin=getPeakPointMargin();
		_peakVertexOffsetY=getPeakVertexOffsetY();
	}
	if(designInfo.origin.datum_pos_style==10)
	{	//暂时在此处重新设计组合角钢填板外形(目前仅有组合角钢填板使用该类型基准点) wht 11-07-01
		CalStdProfile();
	}
	CLDSNode *pBaseNode=BelongModel()->FromNodeHandle(designInfo.m_hBaseNode);
	//if(!designInfo.m_bSelfCorrectBendline&&pBaseNode==NULL)
	//	return FALSE;
	if(m_fInnerRadius>0)
	{	//更新环向板内圆法线方向
		if(pBaseNode==pBaseLinePart->pStart)
			m_tInnerColumnNorm=pBaseLinePart->Start()-pBaseLinePart->End();
		else 
			m_tInnerColumnNorm=pBaseLinePart->End()-pBaseLinePart->Start();
		normalize(m_tInnerColumnNorm);
	}
	if(designInfo.iFaceType<3)
		face_N=designInfo.iFaceType;
	else if(designInfo.iFaceType>=3)
		face_N=3;
	//if(designInfo.iFaceType<4)
	//	jdb_style=0;	//普通板
	//else
	if(designInfo.iFaceType==4) //wht 10-02-26
		jdb_style=1;	//折叠板
	else if(designInfo.iFaceType==5)
		jdb_style=5;
	BOOL bSecondNormCaled=FALSE,bThirdNormCaled=FALSE;
	f3dLine ber_line;
	f3dPoint base_pos,ls_pos,ls_offset_vec,vice_vec,wing_vec;
	if(designInfo.iProfileStyle0123!=3)		//定制(标准)连板外形
		vertex_list.Empty();
	/*	下面代码经仔细核实已过时无效，过一段时间后应会同bFirstAcuteAngle&bSecondAcuteAngle一并去掉 wjh-2015.8.25
	if(face_N>=2)			
	{
		designInfo.huoqufacenorm[0].UpdateVector(BelongModel());
		huoqufacenorm[0]=designInfo.huoqufacenorm[0].vector;
		if(huoqufacenorm[0]*ucs.axis_z<0&&designInfo.bFirstAcuteAngle)
			huoqufacenorm[0]=-huoqufacenorm[0].normalized();
		else if(huoqufacenorm[0]*ucs.axis_z>0&&!designInfo.bFirstAcuteAngle)
			huoqufacenorm[0]=-huoqufacenorm[0].normalized();
		else
			huoqufacenorm[0]= huoqufacenorm[0].normalized();
	}
	if(face_N==3)
	{
		designInfo.huoqufacenorm[1].UpdateVector(BelongModel());
		huoqufacenorm[1]=designInfo.huoqufacenorm[1].vector;
		if(huoqufacenorm[1]*ucs.axis_z<0&&designInfo.bFirstAcuteAngle)
			huoqufacenorm[1]=-huoqufacenorm[1].normalized();
		else if(huoqufacenorm[1]*ucs.axis_z>0&&!designInfo.bFirstAcuteAngle)
			huoqufacenorm[1]=-huoqufacenorm[1].normalized();
		else
			huoqufacenorm[1]= huoqufacenorm[1].normalized();
	}
	*/
	f3dPoint weld_vertex_start,weld_vertex_end;
	BOOL bFirstRayPart=TRUE;
	PROFILE_VERTEX* pVertex=NULL,*pPrevVertex=NULL;
	double minCosa=1,maxCosa=-1,cosa=1;
	f3dPoint base_part_line_vec = pBaseLinePart->End()-pBaseLinePart->Start();
	normalize(base_part_line_vec);
	//确定射线材的散射角度范围
	CLDSLineAngle agentAngle;
	CDesignLjPartPara *pLinePartPara;
	for(pLinePartPara=designInfo.partList.GetFirst();pLinePartPara;pLinePartPara=designInfo.partList.GetNext())
	{
		pPart=BelongModel()->FromPartHandle(pLinePartPara->hPart);
		if(pPart!=NULL&&pPart->IsArcPart())
			pPart=agentAngle.Fabricate(pPart,ucs.origin);
		if(pPart==NULL||!pPart->IsLinePart())
		{
			char ss[200];
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(ss,"Can't find the link line part 0X%X in flexible design parameters of the plate 0X%X ！Program error！",handle,pLinePartPara->hPart);
#else 
			sprintf(ss,"0X%X板中的柔性设计参数中找不到0X%X连接杆件！程序出错！",handle,pLinePartPara->hPart);
#endif
			continue;
		}
		if(pPart->handle==pBaseLinePart->handle)
			continue;
		pCurLinePart=(CLDSLinePart*)pPart;
		if(pLinePartPara->start0_end1!=1)	//始端连接或中间连接
			vice_vec=pCurLinePart->End()-pCurLinePart->Start();
		else								//终端连接
			vice_vec=pCurLinePart->Start()-pCurLinePart->End();
		normalize(vice_vec);
		cosa=base_part_line_vec*vice_vec;
		if(minCosa>cosa)
			minCosa=cosa;
		if(maxCosa<cosa)
			maxCosa=cosa;
	}
	//判断是否双基准
	CLDSLineTube *pOtherBase = NULL;
	if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		if(pBaseLinePart->pStart==NULL||pBaseLinePart->pEnd==NULL)
			return FALSE;
		TUBEJOINT *pCurJoint=&pBaseLinePart.LineTubePointer()->m_tJointStart;
		if((pBaseNode&&pBaseLinePart->pEnd->handle==pBaseNode->handle)||
			(pBaseNode==NULL&&DISTANCE(pBaseLinePart->pEnd->Position(),ucs.origin)<DISTANCE(pBaseLinePart->pStart->Position(),ucs.origin)))
			pCurJoint=&pBaseLinePart.LineTubePointer()->m_tJointEnd;
		if(pCurJoint->type==1&&pCurJoint->hViceLinkObj<0&&pCurJoint->hLinkObj>0x20)
		{
			pLinePartPara=designInfo.FromHandle(pCurJoint->hLinkObj);
			if(pLinePartPara)
				pOtherBase=(CLDSLineTube *)BelongModel()->FromPartHandle(pLinePartPara->hPart);
		}	
	}
	CHashList<char> hashGroupAngleProfEdges;
	for(pLinePartPara=designInfo.partList.GetFirst();pLinePartPara;pLinePartPara=designInfo.partList.GetNext())
	{
		pPart=BelongModel()->FromPartHandle(pLinePartPara->hPart);
		if(pPart!=NULL&&pPart->IsArcPart())
			pPart=agentAngle.Fabricate(pPart,ucs.origin);
		if(pPart==NULL||!pPart->IsLinePart())
		{
			Log2File()->SetCurrTempRelation(0,handle,pLinePartPara->hPart);
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("Can't find the link line part 0X%X in flexible design parameters of the plate 0X%X ！Program error！",handle,pLinePartPara->hPart);
#else 
			Log2File()->Log("0X%X板中的柔性设计参数中找不到0X%X连接杆件！程序出错！",handle,pLinePartPara->hPart);
#endif
			continue;
		}
		pCurLinePart = pLinePartPara->pCurLinePart=(CLDSLinePart*)pPart;
		pCurLinePart->ClearFlag();
		CLDSLinePart::MOVEROD_STACK moverodstack(false);
		pCurLinePart->CalPosition();	//此处如不阻上杆件到位时追踪，可能导致堆栈溢出 wjh-2018.5.2
		moverodstack.RestoreStack();
		//非根据螺栓计算正负头
		if(pLinePartPara->start0_end1==0&&pCurLinePart->desStartOdd.m_iOddCalStyle==0)
			pCurLinePart->CalStartOddment();
		else if(pLinePartPara->start0_end1==1&&pCurLinePart->desEndOdd.m_iOddCalStyle==0)
			pCurLinePart->CalEndOddment();
		if(pCurLinePart->IsAngle()&&pCurLinePart.pAngle->blGroupSonAngle)
		{
			CLDSGroupLineAngle* pParentAngle=(CLDSGroupLineAngle*)this->BelongModel()->FromPartHandle(pCurLinePart.pAngle->hGroupAngle,CLS_GROUPLINEANGLE);
			char ciCurrIndex=0;
			for(ciCurrIndex=0;ciCurrIndex<4;ciCurrIndex++)
			{
				if(pCurLinePart->handle==pParentAngle->son_jg_h[ciCurrIndex])
					break;
			}
			if(pParentAngle->wSectStyle==1)	//T型组合角钢
			{
				char cMapWing=0,*pchMapWing=NULL;
				GEPOINT vWorkWingNorm=pLinePartPara->angle.cur_wing_x0_y1==0?pCurLinePart.pAngle->vxWingNorm:pCurLinePart.pAngle->vyWingNorm;
				if(fabs(vWorkWingNorm*pParentAngle->vxWingNorm)>fabs(vWorkWingNorm*pParentAngle->vyWingNorm))
					cMapWing='X';
				else
					cMapWing='Y';
				char cOverlapWing=pParentAngle->ciMasterIndex==0?'X':'Y';
				if( cMapWing==cOverlapWing&&
					(pchMapWing=hashGroupAngleProfEdges.GetValue(pParentAngle->handle))!=NULL&&*pchMapWing==cMapWing)
					continue;
				else
					hashGroupAngleProfEdges.SetValue(pParentAngle->handle,cMapWing);
			}
			else if(pParentAngle->wSectStyle==2)	//十字型组合角钢
			{
				char* pciSerial=NULL;
				if( (pciSerial=hashGroupAngleProfEdges.GetValue(pParentAngle->handle))!=NULL&&(
					(*pciSerial+ciCurrIndex!=2&&*pciSerial+ciCurrIndex!=4)))
					continue;	//非对角子角钢易形成重叠顶点
				else
					hashGroupAngleProfEdges.SetValue(pParentAngle->handle,ciCurrIndex);
			}
		}
		//根据射线杆件计算并添加轮廓点
		if(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE	//基准杆件为钢管
			||(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE
			&&designInfo.m_bWeldToDatumJg&&designInfo.iFaceType==1)) //基准杆件为角钢且单面板焊接与基准角钢
		{
			if (!CalProfileVertex(pLinePartPara,bFirstRayPart,&weld_vertex_start,&weld_vertex_end))
				continue;
			else
				bFirstRayPart = FALSE;
		}
		else if(!CalProfileVertex(pLinePartPara))
			continue;
	}
	//TODO:海涛仔细校审一下CalProfileVertex及下面一整段代码，看看焊缝起止点横向偏移方向是否正确，一定要与ucs.axis_x脱离 wjh-2015.6.18
	if(designInfo.iProfileStyle0123!=3&&(pBaseLinePart->GetClassTypeId()==CLS_LINETUBE
		||(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE&&designInfo.m_bWeldToDatumJg)))
	{	//钢管为安装定位基准杆件时需要焊接
		f3dPoint rod_vert_vec;
		if(pBaseLinePart->IsTube())
			rod_vert_vec=pBaseLinePart.LineTubePointer()->GetPlateVertSpreadVec(this);
		if(rod_vert_vec.IsZero())
			rod_vert_vec=ucs.axis_x;
		int rodside=0;	//0.初始值1.X轴侧;2.－X轴侧;3.双侧
		for(pLinePartPara=designInfo.partList.GetFirst();pLinePartPara;pLinePartPara=designInfo.partList.GetNext())
		{
			pPart=BelongModel()->FromPartHandle(pLinePartPara->hPart);
			if(pPart==NULL||!pPart->IsLinePart())
				continue;
			pCurLinePart = pLinePartPara->pCurLinePart=(CLDSLinePart*)pPart;
			if(pLinePartPara->start0_end1!=1)	//始端连接或中间连接
				vice_vec=pCurLinePart->End()-pCurLinePart->Start();
			else								//终端连接
				vice_vec=pCurLinePart->Start()-pCurLinePart->End();
			normalize(vice_vec);
			if( rodside<3&&pCurLinePart->handle!=pBaseLinePart->handle&&pCurLinePart!=pOtherBase)
			{//目前还不是两侧连接
				double justify=vice_vec*rod_vert_vec;
				if(justify>EPS)	//X轴侧有连接
				{
					if(rodside==0)	//初始值
						rodside=1;		//当前杆件在 X轴侧连接
					else if(rodside==2)	//已有杆件在-X轴侧连接
						rodside=3;		//两侧均有杆件连接
				}
				else if(justify<-EPS)
				{
					if(rodside==0)	//初始值
						rodside=2;		//当前杆件在-X轴侧连接
					else if(rodside==1)	//已有杆件在 X轴侧连接
						rodside=3;		//两侧均有杆件连接
				}
			}
		}
		if(rodside==3&&pBaseLinePart.IsHasPtr()&&pBaseLinePart->IsTube())	//需要开槽处理
		{
			m_bWeldPart=TRUE;
			pBaseLinePart.LineTubePointer()->m_bCutSlot|=true;
		}
		//基准材焊缝线上的焊缝顶点
		if(designInfo.iWeldLineStyle>=1)
		{	//用户指定焊缝起止顶点位置
			double coord_x=pBaseLinePart->GetWidth()*0.5;
			if(pBaseLinePart->GetClassTypeId()==CLS_LINEANGLE)
			{
				JGZJ jgzj;
				getjgzj(jgzj,pBaseLinePart->GetWidth());
				coord_x=jgzj.g;
			}
			weld_vertex_start=ucs.origin+rod_vert_vec*coord_x+base_part_line_vec*designInfo.weld_start_high;
			weld_vertex_end=ucs.origin+rod_vert_vec*coord_x+base_part_line_vec*designInfo.weld_end_high;
		}
		//由于双侧连接时，焊缝起止点可能位于钢管任何一侧，故需要统一为X轴正侧 wjh-2011.11.27
		//weld_vertex_start.x=weld_vertex_end.x=pBaseLinePart->GetWidth()*0.5;
		//焊缝宽造成的焊缝顶点
		f3dPoint weld_scope_start=weld_vertex_start+rod_vert_vec*designInfo.weld_width;
		f3dPoint weld_scope_end=weld_vertex_end+rod_vert_vec*designInfo.weld_width;

		if (pOtherBase)
		{	//TODO: 有对接管的情况目前缺少范例，故代码还有问题 wjh-2013.11.06
			if(pBaseLinePart->pStart==NULL||pBaseLinePart->pEnd==NULL)
			{
				Log2File()->Log("0x%X杆件的类型可能有误",pBaseLinePart->handle);
				return FALSE;
			}
			if((pBaseNode&&pBaseLinePart->pEnd->handle==pBaseNode->handle)||
				(pBaseNode==NULL&&DISTANCE(pBaseLinePart->pEnd->Position(),ucs.origin)<DISTANCE(pBaseLinePart->pStart->Position(),ucs.origin)))
			{
				f3dPoint weldScopeVec = base_part_line_vec^ucs.axis_z;
				if (weldScopeVec*ucs.axis_x<0)
					weldScopeVec *= -1;
				normalize(weldScopeVec);
				//将焊接区域延伸方向转换到钢板坐标系下 wht 10-06-07
				vector_trans(weldScopeVec,ucs,FALSE);	
				weldScopeVec.z=0;
				weld_scope_start = weld_vertex_start + f3dPoint(1,0,0)*designInfo.weld_width;
				weld_scope_end = weld_vertex_end + weldScopeVec*designInfo.weld_width;
				if(rodside==3)	//两侧都有连接，应考虑两侧焊接
				{
					weld_vertex_start.x-=designInfo.weld_width*2+pBaseLinePart->GetWidth();
					weld_vertex_end-=weldScopeVec*(designInfo.weld_width*2+pBaseLinePart->GetWidth());
				}
			}
			else
			{
				f3dPoint weldScopeVec = pOtherBase->Start() - pOtherBase->End();
				weldScopeVec = weldScopeVec^ucs.axis_z;
				if (weldScopeVec*ucs.axis_x<0)
					weldScopeVec *= -1;
				normalize(weldScopeVec);
				//将焊接区域延伸方向转换到钢板坐标系下 wht 10-06-07
				vector_trans(weldScopeVec,ucs,FALSE);	
				weldScopeVec.z=0;
				weld_scope_start = weld_vertex_start + weldScopeVec*designInfo.weld_width;
				weld_scope_end.Set(weld_vertex_end.x+designInfo.weld_width,weld_vertex_end.y,weld_vertex_end.z);
				if(rodside==3)	//两侧都有连接，应考虑两侧焊接
				{
					weld_vertex_start-=weldScopeVec*(designInfo.weld_width*2+pBaseLinePart->GetWidth());
					weld_vertex_end.x-=designInfo.weld_width*2+pBaseLinePart->GetWidth();
				}
			}
			if(rodside==3)	//需要开槽处理
				pOtherBase->m_bCutSlot|=true;
		}
		else if(rodside==3)	//两侧都有连接，应考虑两侧焊接
		{
			weld_vertex_start-=rod_vert_vec*(designInfo.weld_width+pBaseLinePart->GetWidth());
			weld_vertex_end-=rod_vert_vec*(designInfo.weld_width+pBaseLinePart->GetWidth());
		}
		weld_vertex_start.feature = weld_vertex_end.feature = weld_scope_start.feature = weld_scope_end.feature = 1;
		if (designInfo.iFaceType == 2&&pOtherBase)  
		{	//选择不同基准，保证基准线向外发散
			f3dPoint baseVec = base_part_line_vec;
			if((pBaseNode&&pBaseLinePart->pEnd->handle==pBaseNode->handle)||
				(pBaseNode==NULL&&DISTANCE(pBaseLinePart->pEnd->Position(),ucs.origin)<DISTANCE(pBaseLinePart->pStart->Position(),ucs.origin)))
				baseVec *= -1;
			if((ucs.axis_y*baseVec<0 && ucs.axis_y*base_part_line_vec<0)
				|| (ucs.axis_y*baseVec>0 && ucs.axis_y*base_part_line_vec>0))
				weld_vertex_start.feature = weld_scope_start.feature = 2;
			else
				weld_vertex_end.feature = weld_scope_end.feature = 2;
		}
		coord_trans(weld_vertex_start,ucs,FALSE);
		coord_trans(weld_vertex_end,ucs,FALSE);
		coord_trans(weld_scope_start,ucs,FALSE);
		coord_trans(weld_scope_end,ucs,FALSE);
		/*
		//焊缝宽度过宽时调整焊缝宽 此处不进行调整则DesignProfile()时会舍去有引射线角钢添加的轮廓点 
		//对外形影响不大 暂时不进行调整 wht 10-06-29
		f3dLine start_scope_line(weld_vertex_start,weld_scope_start),end_scope_line(weld_vertex_end,weld_scope_end);
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(start_scope_line.PtInLine(pVertex->vertex)>0)
				weld_scope_start=pVertex->vertex;
			if(end_scope_line.PtInLine(pVertex->vertex)>0)
				weld_scope_end=pVertex->vertex;
		}
		*/

		f3dPoint baseRayVec=base_part_line_vec;
		vector_trans(baseRayVec,ucs,FALSE);
		bool bPlateOnLeft=true;	//true钢板在基准钢管始－>终轴线的左侧（自钢板正面看）
		if((ucs.axis_z^base_part_line_vec)*rod_vert_vec<0)
			bPlateOnLeft=false;	//false钢板在基准钢管始－>终轴线的右侧（自钢板正面看）
		//if (ucs.axis_y*base_part_line_vec>0)
		{
			pVertex=vertex_list.append(PROFILE_VERTEX(weld_vertex_start));
			if(rodside==3)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDSTART_TIP;
			else
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDSTART;
			pVertex->vertex.ID=1;
			//pVertex->rayVecY=-1;//0;
			pVertex->rayVecX=-baseRayVec.x;
			pVertex->rayVecY=-baseRayVec.y;
			pVertex->m_bUpVertex=bPlateOnLeft;//false;
			pVertex=vertex_list.append(PROFILE_VERTEX(weld_vertex_end));
			if(rodside==3)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDEND_TIP;
			else
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDEND;
			pVertex->vertex.ID=1;
			//pVertex->rayVecY=1;//0;
			pVertex->rayVecX=baseRayVec.x;
			pVertex->rayVecY=baseRayVec.y;
			pVertex->m_bUpVertex=!bPlateOnLeft;//true;
			//weld_vertex_start==weld_scope_start时不用添加weld_scope_start，
			//否则修订轮廓点时可能删除weld_vertex_start导致无法添加焊缝标示和对活点 wht 15-07-04
			if(!weld_vertex_start.IsEqual(weld_scope_start))
			{	
				pVertex=vertex_list.append(PROFILE_VERTEX(weld_scope_start));
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDSTART_TIP;
				//pVertex->rayVecY=-1;//0;
				pVertex->rayVecX=-baseRayVec.x;
				pVertex->rayVecY=-baseRayVec.y;
				pVertex->m_bUpVertex=!bPlateOnLeft;//true;
			}
			//weld_vertex_end==weld_scope_end时不用添加weld_scope_end，
			//否则修订轮廓点时可能删除weld_vertex_end导致无法添加焊缝标示和对活点 wht 15-07-04
			if(!weld_vertex_end.IsEqual(weld_scope_end))
			{
				pVertex=vertex_list.append(PROFILE_VERTEX(weld_scope_end));
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDEND_TIP;
				//pVertex->rayVecY=1;//0;
				pVertex->rayVecX=baseRayVec.x;
				pVertex->rayVecY=baseRayVec.y;
				pVertex->m_bUpVertex=bPlateOnLeft;//false;
			}
		}
		/*else
		{
			pVertex=vertex_list.append(PROFILE_VERTEX(weld_vertex_start));
			if(rodside==3)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDSTART_TIP;
			else
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDSTART;
			pVertex->vertex.ID=1;
			pVertex->rayVecY=1;//0;
			pVertex->m_bUpVertex=true;
			pVertex=vertex_list.append(PROFILE_VERTEX(weld_vertex_end));
			if(rodside==3)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDEND_TIP;
			else
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDEND;
			pVertex->vertex.ID=1;
			pVertex->rayVecY=-1;//0;
			pVertex->m_bUpVertex=false;
			pVertex=vertex_list.append(PROFILE_VERTEX(weld_scope_start));
			pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDSTART_TIP;
			pVertex->rayVecY=1;//0;
			pVertex->m_bUpVertex=false;
			pVertex=vertex_list.append(PROFILE_VERTEX(weld_scope_end));
			pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WELDEND_TIP;
			pVertex->rayVecY=-1;//0;
			pVertex->m_bUpVertex=true;
		}*/
	}

	if(designInfo.iProfileStyle0123!=3)
	{
		if(designInfo.iProfileStyle0123==2)	//自动计算最小螺栓包络多边形
			CalWrapLsProfile(CLDSPlate::nPlankEdgeAngleThreshold);
		/*else 	//定制(标准)连板外形，目前已不需要附加顶点，完全可以通过只更新坐标来控制 wjh-2015.1.15
		{
			for(f3dPoint *pPt=designInfo.additional_vertex.GetFirst();pPt;pPt=designInfo.additional_vertex.GetNext())
			{
				pVertex=vertex_list.append(PROFILE_VERTEX(*pPt));
				pVertex->vertex.feature=pPt->feature;
			}
		}*/
		DesignProfile();
		CLDSLineTube *pLineTube=NULL;
		pLineTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
		pPrevVertex=vertex_list.GetTail();
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pPrevVertex->vertex.ID==1&&pVertex->vertex.ID==1)
			{
				pPrevVertex->m_bWeldEdge=1;	//焊缝线
				//填充焊接边对活点信息
				if(pLineTube)
				{	//焊接父构件为钢管时才需要对活点
					pPrevVertex->local_point_vec=1;	//0.无对活点 1.对活点50-->100方向同焊接父杆件始-->终方向 2.与边始-->终方向相反 
					pPrevVertex->local_point_y=0;	//对活点Y坐标值
				}
				else
					pPrevVertex->local_point_vec=0;
			}
			pPrevVertex=pVertex;
		}
	}
	pLsRefList->ResetBoltRefRayNo(this);
	//自定义外形 暂认为默认为定制外形更合理,否则负头更改时可能不能彻底更新外形 wjh-2013.12.20
	//if(designInfo.iProfileStyle0123<2)
	//	designInfo.iProfileStyle0123=3;	//重新设计后应自动将“标准外形”及“定制外形”转换为“自定义外形”
	/*if(face_N==3)
	{	//三面板重新设计完成后同时重新设计三面板补焊钢板 wht 10-01-12
		CLDSPlate *pComplementPlate=NULL;
		pComplementPlate=(CLDSPlate*)BelongModel()->FromPartHandle(complement_plate_handle,CLS_PLATE);
		if(pComplementPlate)
			pComplementPlate->Design3FacePlateComplementPlate(this);
		else //旧版本中生成三面板时未考虑生成补焊钢板,在此处生成
		{	//设计时不应该去生成钢板 暂时放在此处
			pComplementPlate=(CLDSPlate*)BelongModel()->AppendPart(CLS_PLATE);
			pComplementPlate->face_N=1;	//单面板
			pComplementPlate->iSeg=iSeg;
			pComplementPlate->SetLayer(layer());
			pComplementPlate->set_thick(thick);
			pComplementPlate->Design3FacePlateComplementPlate(this);
			complement_plate_handle=pComplementPlate->handle;
		}
	}*/
	return true;
}
//初始化垫板的连接杆件信息
void CLDSPlate::InitBoltPadLjPartPara(CDesignLjPartPara *pLjPartPara)
{
	if(pLjPartPara==NULL||jdb_style!=4)
		return;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(designInfo.m_hBasePart,CLS_LINEANGLE);
	if(pAngle==NULL)
		return;
	CLsRef* pHeadHole = GetFirstLsRef();
	if (pHeadHole == NULL)
	{
		logerr.Log("0x%X垫板缺少螺栓穿孔", handle);
		return;
	}
	f3dPoint pt = pHeadHole->pBolt->ucs.origin;
	pLjPartPara->hPart=designInfo.m_hBasePart;
	pLjPartPara->iFaceNo=1;
	pLjPartPara->start0_end1=DISTANCE(pt,pAngle->Start())<DISTANCE(pt,pAngle->End())?0:1;
	pLjPartPara->angle.bTwoEdge=true;
	pLjPartPara->angle.cur_wing_x0_y1=fabs(pAngle->vxWingNorm*ucs.axis_z)>fabs(pAngle->vyWingNorm*ucs.axis_z)?0:1;
	pLjPartPara->angle.cbSpaceFlag.SetBerSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
	pLjPartPara->angle.cbSpaceFlag.SetWingSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
	pLjPartPara->angle.cbSpaceFlag.SetEndSpaceStyle(ANGLE_SPACE_FLAG::SPACE_UDF);
	pLjPartPara->ber_space=gasketEdgeSpace.berSpace;
	pLjPartPara->wing_space=gasketEdgeSpace.wingSpace;
	pLjPartPara->startSideEndSpace=gasketEdgeSpace.endSpaceS;
	pLjPartPara->endSideEndSpace=gasketEdgeSpace.endSpaceE;
}
//更新钢板外形顶点坐标
bool CLDSPlate::UpdateProfileVertexAndPos()
{
	if(IsShadowObject())	//影像构件
		return ShadowDesign();
	//DesignSetupUcs();
	if(designInfo.iProfileStyle0123!=3)	//自定义外形时，统一按照定制外形更新钢板外形顶点坐标
		return false;
	PROFILE_VERTEX *pVertex,*pPrevVertex=NULL,*pNextVertex=NULL;
	//由于现在存在一个参数化顶点派生出两个切角顶点情况，故已不能通过哈希表处理，只能遍历 wjh-2017.12.22
	//CMapList<PROFILE_VERTEX*> vertexHashList;
	double basevec_y=0;
	CLDSLineTube* pBaseTube=NULL;
	if(designInfo.iWeldLineStyle>=1)
		pBaseTube=(CLDSLineTube*)BelongModel()->FromPartHandle(designInfo.m_hBasePart,CLS_LINETUBE);
	if(pBaseTube)
	{
		f3dPoint base_start=pBaseTube->Start();
		f3dPoint base_end=pBaseTube->End();
		coord_trans(base_start,ucs,FALSE);
		coord_trans(base_end,ucs,FALSE);
		basevec_y=base_end.y-base_start.y;
	}
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WELDSTART&&designInfo.iWeldLineStyle>=1&&pBaseTube)
		{
			if(basevec_y>0)
				pVertex->vertex.y= designInfo.weld_start_high;
			else
				pVertex->vertex.y=-designInfo.weld_start_high;
			if(pVertex->vertex.x>0)
				pVertex->vertex.x= 0.5*pBaseTube->GetDiameter();
			else
				pVertex->vertex.x=-0.5*pBaseTube->GetDiameter();
		}
		else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WELDEND&&designInfo.iWeldLineStyle>=1&&pBaseTube)
		{
			if(basevec_y>0)
				pVertex->vertex.y= designInfo.weld_end_high;
			else
				pVertex->vertex.y=-designInfo.weld_end_high;
			if(pVertex->vertex.x>0)
				pVertex->vertex.x= 0.5*pBaseTube->GetDiameter();
			else
				pVertex->vertex.x=-0.5*pBaseTube->GetDiameter();
		}
		else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WELDSTART_TIP&&designInfo.iWeldLineStyle>=1&&pBaseTube)
		{
			if(basevec_y>0)
				pVertex->vertex.y= designInfo.weld_start_high;
			else
				pVertex->vertex.y=-designInfo.weld_start_high;
			if(pVertex->vertex.x>0)
				pVertex->vertex.x= 0.5*pBaseTube->GetDiameter()+designInfo.weld_width;
			else
				pVertex->vertex.x=-0.5*pBaseTube->GetDiameter()-designInfo.weld_width;
		}
		else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WELDEND_TIP&&designInfo.iWeldLineStyle>=1&&pBaseTube)
		{
			if(basevec_y>0)
				pVertex->vertex.y= designInfo.weld_end_high;
			else
				pVertex->vertex.y=-designInfo.weld_end_high;
			if(pVertex->vertex.x>0)
				pVertex->vertex.x= 0.5*pBaseTube->GetDiameter()+designInfo.weld_width;
			else
				pVertex->vertex.x=-0.5*pBaseTube->GetDiameter()-designInfo.weld_width;
		}
		else if(pVertex->m_cPosDesignType<PROFILE_VERTEX::DT_BER||pVertex->m_cPosDesignType>PROFILE_VERTEX::DT_WING2)
			continue;	//非根据杆件计算顶点坐标
	}
	ATOM_LIST<CDesignLjPartPara*> xLjPartParaList;
	CDesignLjPartPara xLjParaPara;
	if(jdb_style==4)
	{
		InitBoltPadLjPartPara(&xLjParaPara);
		xLjPartParaList.append(&xLjParaPara);
	}
	else
	{
		for(CDesignLjPartPara *pLjPartPara=designInfo.partList.GetFirst();pLjPartPara;pLjPartPara=designInfo.partList.GetNext())
			xLjPartParaList.append(pLjPartPara);
	}
	for(CDesignLjPartPara **ppLjPartPara=xLjPartParaList.GetFirst();ppLjPartPara;ppLjPartPara=xLjPartParaList.GetNext())
	{
		ARRAY_LIST<f3dPoint> vertexArr;
		vertexArr.SetSize(0,4);
		if(!CalProfileVertexPos(*ppLjPartPara,vertexArr))	//根据杆件计算顶点坐标
			continue;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(jdb_style!=4&&pVertex->m_hRod!=(*ppLjPartPara)->hPart)
				continue;
			if(pVertex->m_cPosDesignType<1||pVertex->m_cPosDesignType>4)
				continue;
			if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER&&vertexArr.Count>0)
				pVertex->vertex.Set(vertexArr[0].x,vertexArr[0].y,vertexArr[0].z);
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING&&vertexArr.Count>1)
				pVertex->vertex.Set(vertexArr[1].x,vertexArr[1].y,vertexArr[1].z);
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BER2&&vertexArr.Count>2)
				pVertex->vertex.Set(vertexArr[2].x,vertexArr[2].y,vertexArr[2].z);
			else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_WING2&&vertexArr.Count>3)
				pVertex->vertex.Set(vertexArr[3].x,vertexArr[3].y,vertexArr[3].z);
		}
	}
	//根据顶点的偏移信息修正顶点坐标
	pPrevVertex=vertex_list.GetTail();
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		BOOL pushed=vertex_list.push_stack();
		pNextVertex=vertex_list.GetNext();
		if(pNextVertex==NULL)	//当前节点为末尾节点，则下一节点为链表首节点
			pNextVertex=vertex_list.GetFirst();
		vertex_list.pop_stack(pushed);
		//
		if(pVertex->m_cPosDesignType<1||pVertex->m_cPosDesignType>4)
			continue;
		if(pVertex->xOffset.ciOffMode==0)
		{	//板相对坐标系下偏移
			if(fabs(pVertex->xOffset.x)>0)
				pVertex->vertex.x+=pVertex->xOffset.x;
			if(fabs(pVertex->xOffset.y)>0)
				pVertex->vertex.y+=pVertex->xOffset.y;
		}
		else if(pVertex->xOffset.ciOffMode==1)
		{	//向前顶点偏移
			f3dPoint pre_vec=pPrevVertex->vertex-pVertex->vertex;
			normalize(pre_vec);
			pVertex->vertex+=pre_vec*pVertex->xOffset.x;
		}
		else if(pVertex->xOffset.ciOffMode==2)
		{	//向后顶点偏移
			f3dPoint next_vec=pNextVertex->vertex-pVertex->vertex;
			normalize(next_vec);
			pVertex->vertex+=next_vec*pVertex->xOffset.x;
		}
		pPrevVertex=pVertex;
	}
	//
	pPrevVertex=vertex_list.GetTail();
	f3dPoint inters,intersHL;
	//f2dLine HL1(HuoQuLine[0].startPt,HuoQuLine[0].endPt),HL2(HuoQuLine[1].startPt,HuoQuLine[1].endPt);;
	//int status=(face_N!=3)?0:Int2dll(HL1,HL2,intersHL.x,intersHL.y);
	//bool bValidInters=(status==-2||status>0);
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		BOOL pushed=vertex_list.push_stack();
		pNextVertex=vertex_list.GetNext();
		if(pNextVertex==NULL)	//当前节点为末尾节点，则下一节点为链表首节点
			pNextVertex=vertex_list.GetFirst();
		vertex_list.pop_stack(pushed);
		if(pVertex->vertex.feature>10&&pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_INNERBEND)
		{	//直线内火曲点
			Int3dpl(f3dLine(pPrevVertex->vertex,pNextVertex->vertex),HuoQuLine[pVertex->vertex.feature%10-2],inters);
			/************************************************************************
			 等未来另设一个火曲线交点内移类型时再启用如下代码　　wjh-2018.1.26
			if(bValidInters)
			{
				GEPOINT verifyvec=intersHL-inters;
				GEPOINT inside_direct(-(pNextVertex->vertex.y-pPrevVertex->vertex.y),(pNextVertex->vertex.x-pPrevVertex->vertex.x));
				if(verifyvec*inside_direct>0)	//两火曲线交于钢板内部
				{
					normalize(verifyvec);
					inters=intersHL+verifyvec*(_thick/2);
				}
			}*/
			pVertex->vertex.Set(inters.x,inters.y,inters.z);
		}
		else if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_ARCTANGENT)
		{	//直线与圆弧公切点
			UpdateVertexTangentPos(pPrevVertex,pVertex,pNextVertex);
		}
		pPrevVertex=pVertex;
	}
	return true;
}
//设计菱形内部交叉板
BOOL CLDSPlate::DesignRhombusPlate()
{	
	CLDSNode* pNode=BelongModel()->FromNodeHandle(designInfo.m_hBaseNode);
	if(pNode->m_cPosCalType!=4)
		return FALSE;
	designInfo.m_bEnableFlexibleDesign=FALSE;	//菱形螺栓垫板不启用连板柔性设计,设置后不能保证为菱形 wht 12-08-28
	CLDSLineAngle *pBaseJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pNode->arrRelationPole[0],CLS_LINEANGLE);
	CLDSLineAngle *pAnotherJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pNode->arrRelationPole[1],CLS_LINEANGLE);
	if(pAnotherJg==NULL||pBaseJg==NULL||pBaseJg->handle==pAnotherJg->handle)
		return FALSE;
	double base_wing_wide = pBaseJg->GetWidth();
	double other_wing_wide = pAnotherJg->GetWidth();
	//设计交叉点多孔螺栓垫板外形
	f3dPoint work_norm=ucs.axis_z,base_wing_vec,other_wing_vec;
	if(designInfo.norm.norm_wing==0)
		base_wing_vec=pBaseJg->GetWingVecX();
	else 
		base_wing_vec=pBaseJg->GetWingVecY();
	double ddx=work_norm*pAnotherJg->get_norm_x_wing();
	double ddy=work_norm*pAnotherJg->get_norm_y_wing();
	if(fabs(ddx)>fabs(ddy))
		other_wing_vec=pAnotherJg->GetWingVecX();
	else 
		other_wing_vec=pAnotherJg->GetWingVecY();
	//
	f3dLine datum_ber_line,datum_wing_line,other_ber_line,other_wing_line;
	datum_ber_line.startPt=pBaseJg->Start();
	datum_ber_line.endPt=pBaseJg->End();
	datum_wing_line.startPt=datum_ber_line.startPt+base_wing_vec*base_wing_wide;
	datum_wing_line.endPt=datum_ber_line.endPt+base_wing_vec*base_wing_wide;
	//
	other_ber_line.startPt=pAnotherJg->Start();
	other_ber_line.endPt=pAnotherJg->End();
	project_point(other_ber_line.startPt,datum_ber_line.startPt,work_norm);
	project_point(other_ber_line.endPt,datum_ber_line.startPt,work_norm);
	other_wing_line.startPt=other_ber_line.startPt+other_wing_vec*other_wing_wide;
	other_wing_line.endPt=other_ber_line.endPt+other_wing_vec*other_wing_wide;
	project_point(other_wing_line.startPt,datum_ber_line.startPt,work_norm);
	project_point(other_wing_line.endPt,datum_ber_line.startPt,work_norm);
	f3dPoint ber_inters_pt[2]; //基准角钢楞线与交叉角钢的两个交点 0.楞线 1.棱线
	f3dPoint wing_inters_pt[2];//基准角钢棱线与交叉角钢的两个交点 0.楞线 1.棱线
	int nRetCode[4]={0};
	nRetCode[0]=Int3dll(datum_ber_line,other_ber_line,ber_inters_pt[0]);
	nRetCode[1]=Int3dll(datum_ber_line,other_wing_line,ber_inters_pt[1]);
	nRetCode[2]=Int3dll(datum_wing_line,other_ber_line,wing_inters_pt[0]);
	nRetCode[3]=Int3dll(datum_wing_line,other_wing_line,wing_inters_pt[1]);
	if(nRetCode[0]==1&&nRetCode[1]==1&&nRetCode[2]==1&&nRetCode[3]==1)	
	{
		vertex_list.Empty();
		coord_trans(ber_inters_pt[0],ucs,FALSE);
		coord_trans(ber_inters_pt[1],ucs,FALSE);
		coord_trans(wing_inters_pt[0],ucs,FALSE);
		coord_trans(wing_inters_pt[1],ucs,FALSE);
		ber_inters_pt[0].z=ber_inters_pt[1].z=wing_inters_pt[0].z=wing_inters_pt[1].z=0;
		ber_inters_pt[0].feature=ber_inters_pt[1].feature
			=wing_inters_pt[0].feature=wing_inters_pt[1].feature=1;
		if(base_wing_vec*ucs.axis_x>0)
		{	//基准角钢先棱线后楞线
			if(other_wing_vec*ucs.axis_y>0)
			{	//先楞线后棱线
				vertex_list.append(PROFILE_VERTEX(wing_inters_pt[0]));
				vertex_list.append(PROFILE_VERTEX(wing_inters_pt[1]));
				vertex_list.append(PROFILE_VERTEX(ber_inters_pt[1]));
				vertex_list.append(PROFILE_VERTEX(ber_inters_pt[0]));
			}
			else
			{	//先棱线后楞线
				vertex_list.append(PROFILE_VERTEX(wing_inters_pt[1]));
				vertex_list.append(PROFILE_VERTEX(wing_inters_pt[0]));
				vertex_list.append(PROFILE_VERTEX(ber_inters_pt[0]));
				vertex_list.append(PROFILE_VERTEX(ber_inters_pt[1]));
			}
		}
		else 
		{	//基准角钢先楞线后棱线
			if(other_wing_vec*ucs.axis_y>0)
			{	//先楞线后棱线
				vertex_list.append(PROFILE_VERTEX(ber_inters_pt[0]));
				vertex_list.append(PROFILE_VERTEX(ber_inters_pt[1]));
				vertex_list.append(PROFILE_VERTEX(wing_inters_pt[1]));
				vertex_list.append(PROFILE_VERTEX(wing_inters_pt[0]));
			}
			else
			{	//先棱线后楞线
				vertex_list.append(PROFILE_VERTEX(ber_inters_pt[1]));
				vertex_list.append(PROFILE_VERTEX(ber_inters_pt[0]));
				vertex_list.append(PROFILE_VERTEX(wing_inters_pt[0]));
				vertex_list.append(PROFILE_VERTEX(wing_inters_pt[1]));
			}
		}
	}
	else
	{	//求取交点失败默认按定制外形设计
		designInfo.iProfileStyle0123=1;//定制外形
		DesignPlate();
	}
	return TRUE;
}
int CompareVertex(const PROFILE_VERTEX &vertex1,const PROFILE_VERTEX &vertex2)
{
	PROFILE_VERTEX v1=vertex1;
	double f1=sqrt(vertex1.rayVecX*vertex1.rayVecX+vertex1.rayVecY*vertex1.rayVecY);
	double f2=sqrt(vertex2.rayVecX*vertex2.rayVecX+vertex2.rayVecY*vertex2.rayVecY);
	double rayVecX1=vertex1.rayVecX/f1;
	double rayVecY1=vertex1.rayVecY/f1;
	double rayVecX2=vertex2.rayVecX/f2;
	double rayVecY2=vertex2.rayVecY/f2;
	if(fabs(rayVecX1-rayVecX2)<EPS&&fabs(rayVecY1-rayVecY2)<EPS)
	{
		if(vertex1.m_bUpVertex&&!vertex2.m_bUpVertex)
			return 1;
		else if(!vertex1.m_bUpVertex&&vertex2.m_bUpVertex)
			return -1;
		else
			return 0;
	}
	else if(rayVecY1>=0&&rayVecY2>=0)	//1~2象限
	{
		if(rayVecX1>rayVecX2)
			return -1;
		else
			return 1;
	}
	else if(rayVecY1<=0&&rayVecY2<=0)//3~4象限
	{
		if(rayVecX1>rayVecX2)
			return 1;
		else
			return -1;
	}
	else if(rayVecY1>=0&&rayVecY2<=0)
		return -1;
	else //if(rayVecY1<=0&&rayVecY2>=0)
		return 1;
}
//按逆时针顺序排序
void CLDSPlate::SortVertex(PROFILE_VERTEX *pPntArr, int nCount)
{	
	int i,j,k;
	f3dPoint origin;
	//求多边形顶点的几何中心点origin
	for(i=0;i<nCount;i++)
		origin+=pPntArr[i].vertex;
	origin /=nCount;
	for (i=0;i<nCount;i++)
	{
		if (fabs(pPntArr[i].rayVecX)+fabs(pPntArr[i].rayVecY)<EPS)
		{	//单位化 
			f3dPoint ray_vec=f3dPoint(pPntArr[i].vertex.x,pPntArr[i].vertex.y,0)-f3dPoint(origin.x,origin.y,0);
			normalize(ray_vec);
			pPntArr[i].rayVecX = ray_vec.x;
			pPntArr[i].rayVecY = ray_vec.y;
		}
	}
	CQuickSort<PROFILE_VERTEX>::QuickSort(pPntArr,nCount,CompareVertex);
	//按板面排序
    for(i=0;i<nCount-1;i++)   //冒泡排序
    {
		BOOL sorted = TRUE;
        for(j=0;j<nCount-i-1;j++)
        {
			PROFILE_VERTEX prev = pPntArr[(nCount+j-1)%nCount];
			PROFILE_VERTEX now  = pPntArr[j];
			PROFILE_VERTEX next = pPntArr[(j+1)%nCount];
			if(prev.vertex.feature==next.vertex.feature&&now.vertex.feature!=prev.vertex.feature)
            {
				int nCurFaceVertex=0; //当前面顶点数 wht 10-09-10
				for(k=0;k<nCount;k++)
				{
					if(pPntArr[k].vertex.feature==now.vertex.feature)
						nCurFaceVertex++;
				}
				if(nCurFaceVertex>1)
				{	//当前面顶点数大于1,才需要交换位置 wht 10-09-10
					pPntArr[j] = next;
					pPntArr[(j+1)%nCount] = now;
					next = now;
					now = pPntArr[j];
					sorted = FALSE;
				}
            }
        }
		if(sorted)	//已有序,不需再排
			break;
    }
}
//计算钢板外轮廓在指定投影线上的投影代数长度位置(含正负投影长度值)
//perpline_pick:塔坐标系下的投影线基点
//perpline_vec:塔坐标系下的投影线延伸方向
VALUE_SCOPE CLDSPlate::WeldLinePerpScope(f3dPoint perpline_pick,f3dPoint perpline_vec)
{
	VALUE_SCOPE scope;
	f3dLine perpline(perpline_pick,perpline_pick+perpline_vec);
	normalize(perpline_vec);
	PROFILE_VERTEX *pVertex,*pPrevVertex=vertex_list.GetTail();
	for(pVertex=vertex_list.GetFirst();pVertex;pPrevVertex=pVertex,pVertex=vertex_list.GetNext())
	{
		if(!pPrevVertex->m_bWeldEdge||pPrevVertex->type!=0)
			continue;	//不是直焊缝线
		if(GetEdgeFaceNo(pPrevVertex->vertex.feature,pVertex->vertex.feature)<=0)
			continue;
		f3dPoint edge_start=GetRealVertex(pPrevVertex->vertex);
		f3dPoint edge_end=GetRealVertex(pVertex->vertex);
		edge_start.z=edge_end.z=0;
		coord_trans(edge_start,ucs,TRUE);
		coord_trans(edge_end,ucs,TRUE);
		f3dPoint edge_vec=edge_end-edge_start;
		normalize(edge_vec);
		if(fabs(edge_vec*perpline_vec)>EPS_COS2)
		{
			f3dPoint perp,perp_vec;
			SnapPerp(&perp,perpline,edge_start);
			perp_vec=perp-perpline.startPt;
			scope.VerifyValue(perp_vec*perpline_vec);
			SnapPerp(&perp,perpline,edge_end);
			perp_vec=perp-perpline.startPt;
			scope.VerifyValue(perp_vec*perpline_vec);
			break;	//当前关联焊缝线
		}
	}
	return scope;
}
void CLDSPlate::AlignWeldLine()
{
	if(designInfo.iWeldLineStyle!=2)
		return;
	VALUE_SCOPE scope;
	f3dLine perpline;	//焊缝投影基准线(全塔坐标系)
	f3dPoint just_weld_vec[3],weld_vec,datum_face_pick,face_pick,face_norm,datum_perp;	//全塔坐标系矢量
	f3dPoint perp;
	PROFILE_VERTEX *pVertex=NULL,*pWeldStartVertex=NULL,*pWeldEndVertex=NULL;
	CSuperSmartPtr<CLDSPart>pDatumPart=m_pModel->FromPartHandle(m_hPartWeldParent);
	if(pDatumPart.IsNULL())
		return;
	else if(pDatumPart->IsTube()&&designInfo.m_hBaseNode>0x20)
	{	//基准钢管上连接的纵向板对齐
		CLDSNode *pNode=m_pModel->FromNodeHandle(designInfo.m_hBaseNode);
		just_weld_vec[0]=weld_vec=pDatumPart.LineTubePointer()->End()-pDatumPart.LineTubePointer()->Start();
		normalize(weld_vec);
		if(pNode&&fabs(ucs.axis_z*weld_vec)>EPS)
			return;	//只处理沿钢管纵向焊接的钢板
		perpline.startPt=pNode->Position(true);
	}
	else if(pDatumPart->IsPlate())
	{	//焊接在母板上的立板对齐
		just_weld_vec[0]=pDatumPart->ucs.axis_z^ucs.axis_z;
		normalize(just_weld_vec[0]);
		if(face_N>1)
		{
			just_weld_vec[1]=pDatumPart->ucs.axis_z^huoqufacenorm[0];
			normalize(just_weld_vec[1]);
		}
		if(face_N>2)
		{
			just_weld_vec[2]=pDatumPart->ucs.axis_z^huoqufacenorm[1];
			normalize(just_weld_vec[2]);
		}
	}
	else
		return;
	PROFILE_VERTEX *pPrevVertex=vertex_list.GetTail();
	for(pVertex=vertex_list.GetFirst();pVertex;pPrevVertex=pVertex,pVertex=vertex_list.GetNext())
	{
		if(!pPrevVertex->m_bWeldEdge||pPrevVertex->type!=0)
			continue;	//不是直焊缝线
		int face_no=GetEdgeFaceNo(pPrevVertex->vertex.feature,pVertex->vertex.feature);
		if(face_no<=0)
			continue;
		f3dPoint edge_start=GetRealVertex(pPrevVertex->vertex);
		f3dPoint edge_end=GetRealVertex(pVertex->vertex);
		edge_start.z=edge_end.z=0;
		f3dPoint edge_vec=edge_end-edge_start;
		vector_trans(edge_vec,ucs,TRUE);
		normalize(edge_vec);
		if(fabs(edge_vec*just_weld_vec[face_no-1])>EPS_COS2)
		{
			if(face_no==1)
			{
				face_pick=ucs.origin;
				face_norm=ucs.axis_z;
			}
			else 
			{
				if(face_no==2)
				{
					face_pick=HuoQuLine[0].startPt;
					face_norm=huoqufacenorm[0];
				}
				else
				{
					face_pick=HuoQuLine[1].startPt;
					face_norm=huoqufacenorm[1];
				}
				coord_trans(face_pick,ucs,FALSE);
			}
			face_pick+=ucs.axis_z*m_fNormOffset;
			weld_vec=just_weld_vec[face_no-1];
			pWeldStartVertex=pPrevVertex;
			pWeldEndVertex=pVertex;
			break;	//当前关联焊缝线
		}
	}
	if(pWeldStartVertex)
	{
		//根据基准钢管的关联纵向板或板计算出焊缝线范围
		if(pDatumPart->IsTube())
		{
			datum_perp=perpline.startPt;
			perpline.endPt=perpline.startPt+weld_vec;
			BOOL pushed=m_pModel->PushPartStack();
			for(CLDSPlate *pRelaPlate=(CLDSPlate*)m_pModel->EnumPartFirst(CLS_PLATE);pRelaPlate;
				pRelaPlate=(CLDSPlate*)m_pModel->EnumPartNext(CLS_PLATE))
			{
				if(pRelaPlate->m_hPartWeldParent!=pDatumPart->handle)
					continue;	//不是钢管焊接关联板
				if(fabs(pRelaPlate->ucs.axis_z*weld_vec)>EPS)
					continue;	//不是钢管纵向关联板
				VALUE_SCOPE sub_scope=pRelaPlate->WeldLinePerpScope(perpline.startPt,weld_vec);
				if(sub_scope.IsInitialized())
				{
					scope.VerifyValue(sub_scope.MinValue());
					scope.VerifyValue(sub_scope.MaxValue());
				}
			}
			m_pModel->PopPartStack(pushed);
			normalize(weld_vec);
		}
		else if(pDatumPart->IsPlate())
		{
			datum_face_pick=pDatumPart->ucs.origin;
			ATOM_LIST<PROFILE_VERTEX>*pVertexList=NULL;
			if(pDatumPart->GetClassTypeId()==CLS_PLATE)
			{
				datum_face_pick+=pDatumPart.PlatePointer()->m_fNormOffset*ucs.axis_z;
				pVertexList=&pDatumPart.PlatePointer()->vertex_list;
			}
			else if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE)
			{
				datum_face_pick+=pDatumPart.ParamPlatePointer()->m_fNormOffset*ucs.axis_z;
				pVertexList=&pDatumPart.ParamPlatePointer()->vertex_list;
			}
			if(Int3d2f(&perpline.startPt,&weld_vec,datum_face_pick,pDatumPart->ucs.axis_z,face_pick,face_norm)<1)
				return;	//两面交线计算失败
			perpline.endPt=perpline.startPt+weld_vec;
			f3dLine weldline3d(perpline.startPt,perpline.endPt);
			coord_trans(weldline3d.startPt,pDatumPart->ucs,FALSE);
			coord_trans(weldline3d.endPt,pDatumPart->ucs,FALSE);
			f2dLine weldline2d(weldline3d.startPt,weldline3d.endPt);
			//根据基准板计算出焊缝线范围
			pPrevVertex=pVertexList->GetTail();
			for(pVertex=pVertexList->GetFirst();pVertex;pVertex=pVertexList->GetNext())
			{
				f2dLine edgeline(pPrevVertex->vertex,pVertex->vertex);
				double inters_x,inters_y;
				if(Int2dpl(weldline2d,edgeline,inters_x,inters_y)>0)	//两直线有交点
				{
					double justify=0;
					if(fabs(pVertex->vertex.x-pPrevVertex->vertex.x)>EPS)
						justify=(inters_x-pPrevVertex->vertex.x)/(pVertex->vertex.x-pPrevVertex->vertex.x);
					else
						justify=(inters_y-pPrevVertex->vertex.y)/(pVertex->vertex.y-pPrevVertex->vertex.y);
					if(justify>-EPS&&justify<1+EPS)	//轮廓边内点
					{
						if(!scope.IsInitialized())
						{
							datum_perp.Set(inters_x,inters_y);
							coord_trans(datum_perp,pDatumPart->ucs,TRUE);
							SnapPerp(&datum_perp,perpline,datum_perp);
							scope.VerifyValue(0);
						}
						else
						{
							perp.Set(inters_x,inters_y);
							coord_trans(perp,pDatumPart->ucs,TRUE);
							SnapPerp(&perp,perpline,perp);
							f3dPoint perp_vec=perp-datum_perp;
							double perp_len=perp_vec*weld_vec;
							scope.VerifyValue(perp_len);
						}
					}
				}
				pPrevVertex=pVertex;
			}
		}
		if(scope.IsInitialized())
		{
			//延伸对齐当前板焊缝线范围
			perp=GetRealVertex(pWeldStartVertex->vertex);
			perp.z=0;
			coord_trans(perp,ucs,TRUE);
			SnapPerp(&perp,perpline,perp);
			f3dPoint extend_vec=perp-datum_perp;
			double start_len=extend_vec*weld_vec;
			perp=GetRealVertex(pWeldEndVertex->vertex);
			perp.z=0;
			coord_trans(perp,ucs,TRUE);
			SnapPerp(&perp,perpline,perp);
			extend_vec=perp-datum_perp;
			double end_len=extend_vec*weld_vec;
			if(pDatumPart->IsTube())
			{
				if(start_len<end_len&&scope.MinValue()<start_len)
					start_len=scope.MinValue();
				else if(start_len>end_len&&scope.MaxValue()>start_len)
					start_len=scope.MaxValue();
				if(end_len<start_len&&scope.MinValue()<end_len)
					end_len=scope.MinValue();
				else if(end_len>start_len&&scope.MaxValue()>end_len)
					end_len=scope.MaxValue();
				designInfo.weld_start_high=min(start_len,end_len);
				designInfo.weld_end_high=max(start_len,end_len);
			}
			else
			{
				f3dPoint perpline_vec_i=weld_vec;
				vector_trans(perpline_vec_i,ucs,FALSE);
				if(start_len<end_len&&scope.MinValue()<start_len)
				{
					perp=pWeldStartVertex->vertex+perpline_vec_i*(scope.MinValue()-start_len);
					pWeldStartVertex->vertex.Set(perp.x,perp.y);
				}
				else if(start_len>end_len&&scope.MaxValue()>start_len)
				{
					perp=pWeldStartVertex->vertex+perpline_vec_i*(scope.MaxValue()-start_len);
					pWeldStartVertex->vertex.Set(perp.x,perp.y);
				}
				if(end_len<start_len&&scope.MinValue()<end_len)
				{
					perp=pWeldEndVertex->vertex+perpline_vec_i*(scope.MinValue()-end_len);
					pWeldEndVertex->vertex.Set(perp.x,perp.y);
				}
				else if(end_len>start_len&&scope.MaxValue()>end_len)
				{
					perp=pWeldEndVertex->vertex+perpline_vec_i*(scope.MaxValue()-end_len);
					pWeldEndVertex->vertex.Set(perp.x,perp.y);
				}
			}
			SetModified();
		}
	}
}
//钢板是否为有杆件及螺栓连接的连接板
bool CLDSGeneralPlate::IsConnectPlate()
{
	if(m_nClassTypeId!=CLS_PARAMPLATE)
		return true;
	CLDSParamPlate* pParamPlate=(CLDSParamPlate*)this;
	if( pParamPlate->m_iParamType==TYPE_XEND||
		pParamPlate->m_iParamType==TYPE_UEND||
		pParamPlate->m_iParamType==TYPE_ROLLEND||
		pParamPlate->m_iParamType==TYPE_SHOEPLATE||
		pParamPlate->m_iParamType==TYPE_STATIONSHOEPLATE||
		pParamPlate->m_iParamType==TYPE_STATIONCONNPLATE)
		return true;
	else
		return false;
}
bool CLDSGeneralPlate::IsTubeFoldPlate()	//变电构架曲梁上折叠板（双火曲面为连接面，基面反而仅是过渡不是连接面） wjh-2017.11.8
{
	return ((wiType==0||wiType>=CONNECT_PLATE)&&((CLDSPlate*)this)->designInfo.iFaceType==6);
}
bool CLDSGeneralPlate::IsSplayFoldPlate()	//变电构件三角梁顶梁上八字板（基准面并非沿钢管轴向） wjh-2017.11.8
{
	return ((wiType==0||wiType>=CONNECT_PLATE)&&((CLDSPlate*)this)->designInfo.iFaceType==7);
}
bool CLDSGeneralPlate::IsMaybeBasePlate()
{	//是否为塔脚底座板（用于添加地脚螺栓）
	if(this->wiType==TYPE_BASEPLATE)
		return true;	//参数化底座板
	else if(this->m_nClassTypeId==CLS_PLATE)
	{	//已转化为普通钢板的底座板
		for(CLsRef* pLsRef=this->GetFirstLsRef();pLsRef;pLsRef=this->GetNextLsRef())
		{
			if(!pLsRef->GetLsPtr()->m_bVirtualPart)
				return false;
		}
	}
	return true;
}
bool CLDSPlate::IsConnectPlate()
{
	bool hasValidBolt=false;
	for(CLsRef* pLsRef=GetFirstLsRef();pLsRef&&!hasValidBolt;pLsRef=GetNextLsRef())
	{
		if(pLsRef->GetLsPtr()->m_bVirtualPart)
			continue;
		hasValidBolt=true;
	}
	if(!hasValidBolt)
		return false;
	return true;
}
/*
 * 是否为带顶角的三面板　wjh-2016.1.9
 * 由于过往对折叠板的jdb_style处理比较混乱，现统一通过IsPeakPointPlate()与IsFoldPlate()判断
*/
bool CLDSPlate::IsPeakPointPlate()
{
	if(_face_N==3&&(style.wiLegacyStyle==0||designInfo.iFaceType==7))
		return true;
	else
		return false;
}
bool CLDSPlate::IsFoldPlate()
{
	if(_face_N!=3||style.wiLegacyStyle==0||designInfo.iFaceType==7)
		return false;
	else
		return true;
}
//是否为火曲线共线的三面板 wht 18-12-17
bool CLDSPlate::IsHuoQuLineCollinear3FacePlate()
{
	if(!IsPeakPointPlate())
		return false;
	GEPOINT bendlinevec0=HuoQuLine[0].endPt-HuoQuLine[0].startPt;
	GEPOINT bendlinevec1=HuoQuLine[1].endPt-HuoQuLine[1].startPt;
	bendlinevec0.z=bendlinevec1.z=0;
	normalize(bendlinevec0);
	normalize(bendlinevec1);
	if(fabs(bendlinevec0*bendlinevec1)>EPS_COS2)
		return true;
	else
		return false;
}
double CLDSPlate::getPeakVertexOffsetY()
{
	if(!IsHuoQuLineCollinear3FacePlate())
		return 0;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
			break;
	}
	if(pVertex)
		return pVertex->xOffset.y;
	else
		return _peakVertexOffsetY;
}
void CLDSPlate::setPeakVertexOffsetY(double offset)
{
	if(!IsHuoQuLineCollinear3FacePlate())
		return;
	BOOL bHasVertex=FALSE;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
		{
			pVertex->xOffset.y=(float)offset;
			bHasVertex=TRUE;
		}
	}
	if(!bHasVertex)
		_peakVertexOffsetY=offset;

}
double CLDSPlate::getExtendVertexOffsetLen()
{
	if(!IsHuoQuLineCollinear3FacePlate())
		return 0;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
			break;
	}
	if(pVertex)
		return pVertex->xOffset.x;
	else
		return 200;	//初次设计默认偏移距离为200
}
void CLDSPlate::setExtendVertexOffsetLen(double offset)
{
	if(!IsHuoQuLineCollinear3FacePlate())
		return;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
			pVertex->xOffset.x=(float)offset;
	}
}
double CLDSPlate::getPeakPointMargin()
{
	if(!IsHuoQuLineCollinear3FacePlate())
		return 0;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
			break;
	}
	if(pVertex)
		return pVertex->xOffset.ciOffMode;
	else
		return 1;	//默认间隙值为1
}
void CLDSPlate::setPeakPointMargin(double margin)
{
	if(!IsHuoQuLineCollinear3FacePlate())
		return;
	PROFILE_VERTEX *pVertex=NULL;
	BOOL bHasVertex=FALSE;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
		{
			pVertex->xOffset.ciOffMode=(BYTE)margin;
			bHasVertex=TRUE;
		}
	}
	if(!bHasVertex)
		_peakPointMargin=margin;
}
static void ReversedPlateVertexList(ATOM_LIST<PROFILE_VERTEX> &vertexList)
{
	int i=0,nCount=vertexList.GetNodeNum();
	if(nCount<=0)
		return;
	DYN_ARRAY<PROFILE_VERTEX> vertexArr(nCount);
	for(PROFILE_VERTEX* pVertex=vertexList.GetFirst();pVertex;pVertex=vertexList.GetNext())
		vertexArr[i++]=*pVertex;
	vertexList.Empty();
	PROFILE_EDGEINFO edgeInfo,tailEdgeInfo;
	vertexArr[nCount-1].CopyEdgeInfoTo(&tailEdgeInfo);	//备份最后一个轮廓点存储的边信息
	for(i=nCount-1;i>=0;i--)
	{
		PROFILE_VERTEX *pVertex=vertexList.append(vertexArr[i]);
		pVertex->vertex.x*=-1;
		//将前一轮廓点存储的边信息拷贝到当前轮廓点
		if(i==0)
			pVertex->CopyEdgeInfoFrom(tailEdgeInfo);
		else
		{	
			vertexArr[i-1].CopyEdgeInfoTo(&edgeInfo);
			pVertex->CopyEdgeInfoFrom(edgeInfo);
		}
	}
}
//提取垂直于火曲线同时指向火曲面的判定方向
static GEPOINT GetBendLineVertVec(const double* huoqulineStart,const double* huoqulineEnd,
	ATOM_LIST<PROFILE_VERTEX>&vertex_list,int iPlaneNo2or3=2)
{
	GEPOINT vSpreadBendVertVec(huoqulineEnd[0]-huoqulineStart[0],huoqulineEnd[1]-huoqulineStart[1]);
	normalize(vSpreadBendVertVec);
	vSpreadBendVertVec.Set(-vSpreadBendVertVec.y,vSpreadBendVertVec.x);	//由火曲线方向转90度为垂直火曲线方向
	int sign=0;
	f3dPoint xPickOrg(huoqulineStart[0],huoqulineStart[1]);
	CMaxDouble maxdist;
	for(PROFILE_VERTEX* pVerter=vertex_list.GetFirst();pVerter;pVerter=vertex_list.GetNext())
	{
		if(pVerter->vertex.feature!=iPlaneNo2or3)
			continue;
		double distance=(pVerter->vertex-xPickOrg)*vSpreadBendVertVec;
		if(maxdist.Update(fabs(distance),pVerter)==fabs(distance))
			sign=(distance>0)?1:-1;
	}
	if(sign<0)
		vSpreadBendVertVec*=-1;
	return vSpreadBendVertVec;
}
static int GetHuoquVertexType(PROFILE_VERTEX *pVertex,PROFILE_VERTEX *pPrevVertex,BOOL bPeakPointPlate=FALSE)
{
	if(pVertex->vertex.feature==12)
	{
		if(pPrevVertex->vertex.feature==2)
			return 21;
		else
			return 12;
	}
	else if(pVertex->vertex.feature==13)
	{
		if(pPrevVertex->vertex.feature==3||(bPeakPointPlate&&pPrevVertex->vertex.feature>10))
			return 31;
		else
			return 13;
	}
	else
		return 0;
}
void CLDSPlate::DesignProfile2()
{
	ATOM_LIST<PROFILE_VERTEX>outVertexlist;
	BOOL bHasTwoVertex=FALSE,bOneAxisLineVertex=TRUE;	//所有螺栓都在一条心线上的情况
	f3dLine axis_line;
	struct HUOQU_VERTEX{
		BOOL bExistSrcVertex;
		PROFILE_VERTEX srcVertex;
		PROFILE_VERTEX *pPeerVertex;
		//12 1->2 从第1面到第2面
		//21 2->1 从第2面到第1面
		//13 1->3 从第1面到第3面
		//31 3->1 从第3面到第1面 
		long iType;
		HUOQU_VERTEX(){pPeerVertex=NULL;iType=0;bExistSrcVertex=FALSE;}
	};
	int nHuoquVertex=0;
	HUOQU_VERTEX huoquVertexArr[4];
	PROFILE_VERTEX *pVerter=NULL,*pStartVerter=NULL,*pEndVerter=NULL,*pFirstStartVerter=NULL;
	PROFILE_VERTEX *pPrevVertex=vertex_list.GetTail();
	BOOL bBackupHuoquVertex=(designInfo.iFaceType==6||designInfo.iFaceType==7);
	if(bBackupHuoquVertex)
	{
		for(pVerter=vertex_list.GetFirst();pVerter;pVerter=vertex_list.GetNext())
		{
			if(pVerter->vertex.feature>10&&nHuoquVertex<4)
			{	//特殊板,需要备份火曲点 wht 16-10-13
				huoquVertexArr[nHuoquVertex].srcVertex=*pVerter;
				huoquVertexArr[nHuoquVertex].iType=GetHuoquVertexType(pVerter,pPrevVertex,IsPeakPointPlate());
				huoquVertexArr[nHuoquVertex].bExistSrcVertex=TRUE;
				nHuoquVertex++;
			}
			pPrevVertex=pVerter;
		}
	}
	//双面板时，记录火曲面在火曲线的位置标记，用于更新轮廓点所在面号
	GEPOINT vSpreadBendVertVec=GetBendLineVertVec(HuoQuLine[0].startPt,HuoQuLine[0].endPt,vertex_list,2);
	//反转顶点链表顺序,保证与对称构件按同一顶点顺序计算外形 wht 16-05-20
	if(m_bClockwiseCalProfile)	
		ReversedPlateVertexList(vertex_list);
	CHashList<bool> hashReserved;
	for(pVerter=vertex_list.GetFirst();pVerter;pVerter=vertex_list.GetNext())
	{
		if(!pVerter->m_bCanCut)
			hashReserved.SetValue((DWORD)pVerter,false);
		if(pStartVerter==NULL)
		{
			pStartVerter=pVerter;
			axis_line.startPt.Set(pVerter->vertex.x,pVerter->vertex.y);
			axis_line.endPt.Set(pVerter->vertex.x,pVerter->vertex.y);
		}
		else
		{
			if(bHasTwoVertex)
			{
				if(bOneAxisLineVertex)
				{
					f3dPoint centre;
					centre.Set(pVerter->vertex.x, pVerter->vertex.y);
					int	ret =axis_line.PtInLine(centre);
					if(ret==-2)
					{
						axis_line.startPt=centre;
						axis_line.startPt.feature =pVerter->vertex.feature;
					}
					else if(ret==-1)
					{
						axis_line.endPt=centre;
						axis_line.endPt.feature=pVerter->vertex.feature;
					}
					if(ret==0)
						bOneAxisLineVertex=FALSE;
				}
			}
			else
			{
				bHasTwoVertex=TRUE;
				axis_line.endPt.Set(pVerter->vertex.x,pVerter->vertex.y);
				axis_line.endPt.feature=pVerter->vertex.feature;
			}
			//取左下角顶点为轮廓起始顶点
			if(pStartVerter->vertex.y>pVerter->vertex.y+EPS2)
				pStartVerter=pVerter;
			else if(fabs(pStartVerter->vertex.y-pVerter->vertex.y)<=EPS2&&pStartVerter->vertex.x>pVerter->vertex.x)
				pStartVerter=pVerter;	//Y分量相对取X分量较小者
		}
	}
	if(bOneAxisLineVertex)
		return;   //所有的点在一条线上形成不了板
	outVertexlist.append(*pStartVerter);
	hashReserved.SetValue((DWORD)pStartVerter,true);
	pFirstStartVerter=pStartVerter;
	int nLoop=0;
	f2dLine line;
	while(true)
	{
		if(nLoop==vertex_list.GetNodeNum())
			break;
		nLoop++;
		for (pEndVerter = vertex_list.GetFirst();pEndVerter;pEndVerter=vertex_list.GetNext())
		{
			if(pEndVerter==pStartVerter)
				continue;
			BOOL bOutVertex=TRUE;
			vertex_list.push_stack();
			line.startPt.Set(pStartVerter->vertex.x,pStartVerter->vertex.y);
			line.endPt.Set(pEndVerter->vertex.x,pEndVerter->vertex.y);
			for(pVerter=vertex_list.GetFirst();pVerter;pVerter=vertex_list.GetNext())
			{
				if(pVerter==pStartVerter||pVerter==pEndVerter)
			 		continue;
				double dd=DistOf2dPtLine(pVerter->vertex,line.startPt,line.endPt);
				if(fabs(dd)<=eps2)	//二块板合并的时候，对二个点共线要求太严格导致板外形错误  CSS 2016-11-9
				{	
					f3dPoint line_vec(line.endPt.x-line.startPt.x,line.endPt.y-line.startPt.y);
					f3dPoint vec1(pVerter->vertex.x-line.startPt.x,pVerter->vertex.y-line.startPt.y);
					f3dPoint vec2(pVerter->vertex.x-line.endPt.x,pVerter->vertex.y-line.endPt.y);
					if(line_vec*vec1>0&&line_vec*vec2>0)
					{
						bOutVertex=FALSE;
						break;
					}
				}
				else if(dd+eps2<0)  //点在线的外侧  //二块板合并的时候，对二个点共线要求太严格导致板外形错误  CSS 2016-11-9
				{
					bOutVertex=FALSE;
					break;
				}
			}
			vertex_list.pop_stack();
			if(bOutVertex)
				break;
		}
		if(pEndVerter==pFirstStartVerter)
			break;
		else if(pEndVerter)
		{
			outVertexlist.append(*pEndVerter);
			hashReserved.SetValue((DWORD)pEndVerter,true);
			pStartVerter=pEndVerter;
		}
		else
			continue;
	}
	//将不允许删除的凹点添加回来 wjh-2018.3.1
	for(bool *pbState=hashReserved.GetFirst();pbState;pbState=hashReserved.GetNext())
	{
		if(*pbState)	//已保留下来
			continue;
		PROFILE_VERTEX* pNoneCutVertex=(PROFILE_VERTEX*)hashReserved.GetCursorKey();
		pPrevVertex=outVertexlist.GetTail();
		CMinDouble nearedge;
		for (pVerter=outVertexlist.GetFirst();pVerter;pVerter=outVertexlist.GetNext()) 
		{
			GEPOINT vprev=pNoneCutVertex->vertex-pPrevVertex->vertex;
			GEPOINT vnext=pNoneCutVertex->vertex-pVerter->vertex;
			double sidedist=DistOf2dPtLine(pNoneCutVertex->vertex,pPrevVertex->vertex,pVerter->vertex);
			pPrevVertex=pVerter;
			if(vprev*vnext>0)
				continue;	//不在当前边垂直覆盖范围内
			nearedge.Update(sidedist,pPrevVertex);
		}
		PROFILE_VERTEX* pNearestEdgeEndVertex=(PROFILE_VERTEX*)nearedge.m_pRelaObj;
		for (pVerter=outVertexlist.GetFirst();pVerter;pVerter=outVertexlist.GetNext()) 
		{
			if(pVerter==pNearestEdgeEndVertex)
			{
				outVertexlist.insert(*pNoneCutVertex);
				break;
			}
		}
	}
	vertex_list.Empty();
	for (PROFILE_VERTEX* pVertex=outVertexlist.GetFirst();pVertex;pVertex=outVertexlist.GetNext()) 
	{
		pVertex->vertex.z=0;// 现在点是二位坐标因此z坐标没用
		vertex_list.append(*pVertex);
	}
	if(m_bClockwiseCalProfile)	
		ReversedPlateVertexList(vertex_list);
	//添加火曲线的点
	for(int iBendFace=0;iBendFace<face_N-1;iBendFace++)
	{
		f3dLine tempLine;
		int n = vertex_list.GetNodeNum();
		f3dPoint last,now,next,inters;
		for (int j=0;j<n;j++)
		{
			now  = vertex_list[j].vertex;
			next = vertex_list[(j+1)%n].vertex;
			tempLine.startPt = now;
			tempLine.endPt = next;
			tempLine.startPt.z = tempLine.endPt.z = 0;
			if(Int3dpl(tempLine,HuoQuLine[iBendFace],inters)<=0)
				continue;
			else
			{
				PROFILE_VERTEX *pHuoquVertex=NULL;
				PROFILE_VERTEX *pPrevVertex=&vertex_list[j];
				int ret=tempLine.PtInLine(inters,EPS2);
				if(ret==2)
				{
					inters.feature = 12+iBendFace;
					pHuoquVertex=vertex_list.insert(inters,j+1);
					j++;
					n++;
				}
				else if(ret==1)
				{	//端点即为火曲点
					if(tempLine.startPt.IsEqual(inters,EPS2))
					{
						vertex_list[j].vertex.feature=12+iBendFace;
						pHuoquVertex=&vertex_list[j];
						pPrevVertex=&vertex_list[(j-1+n)%n];
					}
					else //if(line.endPt==inters)
					{
						vertex_list[(j+1)%n].vertex.feature=12+iBendFace;
						pHuoquVertex=&vertex_list[(j+1)%n];
					}
				}
				if(bBackupHuoquVertex&&pHuoquVertex&&pPrevVertex)
				{	//与原有火曲点根据类型进行匹配
					int i=0;
					int iType=GetHuoquVertexType(pHuoquVertex,pPrevVertex,IsPeakPointPlate());
					if(iType==13&&designInfo.iFaceType==6&&pPrevVertex->vertex.feature>10&&next.feature==2)
						iType=31;	//第3面火曲线与第2面边(12=>2)计算交点，出现交叉，此处暂时如此处理 wht 16-11-03
					for(i=0;i<nHuoquVertex;i++)
					{
						if(huoquVertexArr[i].iType==iType)
						{
							huoquVertexArr[i].pPeerVertex=pHuoquVertex;
							break;
						}
					}
					if(i>=nHuoquVertex&&nHuoquVertex<4)
					{	//未找到匹配的火曲点,添加火曲点
						huoquVertexArr[nHuoquVertex].pPeerVertex=pHuoquVertex;
						huoquVertexArr[nHuoquVertex].iType=iType;
						nHuoquVertex++;
					}
				}
			}
		}
	}
	if(designInfo.iFaceType==6)
	{	//底部折叠板,类型为31、12的底部火曲点需保留原有坐标
		PROFILE_VERTEX *pHuoquVertex12=NULL,*pHuoquVertex31=NULL;
		for(int i=0;i<nHuoquVertex;i++)
		{
			if( huoquVertexArr[i].pPeerVertex&&huoquVertexArr[i].bExistSrcVertex&&
				(huoquVertexArr[i].iType==12||huoquVertexArr[i].iType==31))
			{
				*(huoquVertexArr[i].pPeerVertex)=huoquVertexArr[i].srcVertex;
				if(huoquVertexArr[i].iType==12)
					pHuoquVertex12=huoquVertexArr[i].pPeerVertex;
				else //if(huoquVertexArr[i].iType==31)
					pHuoquVertex31=huoquVertexArr[i].pPeerVertex;
			}
		}
		if(pHuoquVertex12&&pHuoquVertex31)
		{	//纠正轮廓点顺序
			int iVertex12=-1,iVertex31=-1,i=0;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(pVertex==pHuoquVertex12)
					iVertex12=i;
				else if(pVertex==pHuoquVertex31)
					iVertex31=i;
				i++;
			}
			if(iVertex31>iVertex12)
			{
				PROFILE_VERTEX vertex=*pHuoquVertex12;
				*pHuoquVertex12=*pHuoquVertex31;
				*pHuoquVertex31=vertex;
			}
		}
	}
	else if(IsSplayFoldPlate()&&face_N==3)//designInfo.iFaceType==7&&face_N==3)
	{	//顶部折叠板使用top_point,需删除类型为31、12的火曲点
		PROFILE_VERTEX *pHuoquVertex12=NULL,*pHuoquVertex31=NULL;
		for(int i=0;i<nHuoquVertex;i++)
		{
			if(huoquVertexArr[i].iType==12)
				pHuoquVertex12=huoquVertexArr[i].pPeerVertex;
			if(huoquVertexArr[i].iType==31)
				pHuoquVertex31=huoquVertexArr[i].pPeerVertex;
		}
		if(pHuoquVertex12||pHuoquVertex31)
		{
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(pVertex==pHuoquVertex12||pVertex==pHuoquVertex31)
					vertex_list.DeleteCursor(FALSE);
			}
			vertex_list.Clean();
		}
	}
	if(face_N==2)
	{	//根据火曲面在火曲线的位置标记,更新火曲面上轮廓点面号 wxc-2017.12.5
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.feature>10)
				continue;	//火曲点
			if((pVertex->vertex-HuoQuLine[0].startPt)*vSpreadBendVertVec>0)
				pVertex->vertex.feature=2;
		}
	}
}
//前提是连接板上的所有顶点都已旋转到第一面上(即全部顶点共面)
void CLDSPlate::DesignProfile(BOOL bSortingVertex/*=TRUE*/)
{
	if(m_siProfileAlgor==1&&face_N<=2)
	{
		DesignProfile2();	//个别板外形旧算法计算不对,需按新改进包络外形算法计算 wjh-2015.11.08
		return;
	}
	double dd;
	int i,j,k,n=vertex_list.GetNodeNum();
	if(n==0)	//顶点数为无法生成合法的钢板
		return;
	SetModified();
	PROFILE_VERTEX last,now,next,next2;
	f3dPoint pt1,pt2,inters;
	f3dLine line;
	PROFILE_VERTEX *pVertex,*pPrevPnt=NULL;
	//在设计外形前，提前对轮廓点按逆时针排序，一般都需要，只是在对齐焊缝时，
	//只是在原有顺序基础上修改焊缝线顶点位置，以防万一可不执行此段代码 wjh-2011.7.16
	if(bSortingVertex)
	{
		PROFILE_VERTEX *pPntArr = new PROFILE_VERTEX[n];
		for(pVertex=vertex_list.GetFirst(),i=0;pVertex;pVertex=vertex_list.GetNext(),i++)
		{
			pPntArr[i] = *pVertex;
			pPntArr[i].vertex.z = 0;
		}
		//------VVVVV------对各角钢上的连接板多边形顶点按逆时针进行排序
		SortVertex(pPntArr,n);
		for(i=0;i<n;i++)
			vertex_list[i] = pPntArr[i];
		delete []pPntArr;
	}
	if(m_bClockwiseCalProfile)	//反转顶点链表顺序,保证与对称构件按同一顶点顺序计算外形 wht 16-05-20
		ReversedPlateVertexList(vertex_list);
	//双面板时，记录火曲面在火曲线的位置标记，用于更新轮廓点所在面号
	GEPOINT vSpreadBendVertVec=GetBendLineVertVec(HuoQuLine[0].startPt,HuoQuLine[0].endPt,vertex_list,2);
	//双面板时，记录火曲面在火曲线的位置标记，用于更新轮廓点所在面号
	f3dPoint huoqu_vertical_vec(HuoQuLine[0].startPt.y-HuoQuLine[0].endPt.y,HuoQuLine[0].endPt.x-HuoQuLine[0].startPt.x);
	int bend_flag=0;
	if(face_N==2)
	{
		CMaxDouble max_dist;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.feature!=2)
				continue;
			double fDist=DistOf2dPtLine(pVertex->vertex,HuoQuLine[0].startPt,HuoQuLine[0].endPt);
			max_dist.Update(fabs(fDist),pVertex);		
		}
		if (max_dist.IsInited())
		{	//最初可能是想找取一个制弯面上的恰当拾取点（较远的点），但不知为什么一直没用现在也失去应用价值就不在追根了。wjh-2019.6.22
			double fCosa = huoqu_vertical_vec * (((PROFILE_VERTEX*)max_dist.m_pRelaObj)->vertex - HuoQuLine[0].startPt);
			if (fCosa > EPS)
				bend_flag = 1;
			else if (fCosa < -EPS)
				bend_flag = -1;
		}
	}
	for(i=0;i<face_N;i++)
	{
		BOOL bFirstLoop=TRUE; //第一次循环，消除明显的凹点 wht 10-09-13
		for(;;)//一次计算有可能不能完全将凹多边形削为凸多边形
		{
			BOOL bAllProtrusive=TRUE;
			for(j=0;j<vertex_list.GetNodeNum();j++)
			{
				n = vertex_list.GetNodeNum();
				last = vertex_list[(n+j-1)%n];
				now  = vertex_list[j];
				next = vertex_list[(j+1)%n];

				if(last.vertex.feature==next.vertex.feature&&(now.vertex.feature!=last.vertex.feature))
				{	//不同面顶点位置排序错误情况
					int nCurFaceVertex=0; //当前面顶点数 wht 10-09-10
					for(k=0;k<n;k++)
					{
						if(vertex_list[k].vertex.feature==now.vertex.feature)
							nCurFaceVertex++;
					}
					if(nCurFaceVertex>1)
					{	//当前面顶点数大于1,才需要交换位置 wht 10-09-10
						vertex_list[j] = next;
						vertex_list[(j+1)%n] = now;
						next = now;
						now = vertex_list[j];
					}
				}
				if(DISTANCE(last.vertex,now.vertex)<EPS2)	//两点过于接近删除一点
				{
					if(now.vertex.feature<10)
						vertex_list.DeleteAt(j);
					else
						vertex_list.DeleteAt((n+j-1)%n);
					j--;
					continue;
				}
				dd = DistOf2dPtLine(next.vertex,last.vertex,now.vertex);
				//当轮廓边出现交叉时仅使用 next是否在 last->now边的右侧不能完全将凹多边形削为凸多边形
				//应判断next后的所有顶点是否均在last->now的右侧,即可完全将凹多边形削为凸多边形
				//由于多面板可能存在排序错误的情况，此处暂时仅处理单面板的情况 wht 10-08-09
				//该算法当 当前顶点的前一个顶点为凹点时，可能出错,第一次循环时不进行以下操作 wht 10-09-13
				if(!bFirstLoop&&dd>eps&&face_N==1) 
				{	//dd>eps时继续查看以后的顶点是否均在last->now的右侧
					bFirstLoop=FALSE; 
					int iCurIndex=j;
					while(iCurIndex<n)
					{
						next2= vertex_list[(iCurIndex+2)%n];	
						dd = DistOf2dPtLine(next2.vertex,last.vertex,now.vertex);
						if(dd<-eps)
							break;
						iCurIndex++;
					}
				}
				line.startPt = last.vertex;
				line.endPt = next.vertex;
				if(dd<-eps||line.PtInLine(now.vertex)>0)//&&now.vertex.is_visible)	//-eps不可改为0否则会死机(进入死循环，因为有火曲点)
				{	//next点在 last->now边的右侧
					if(now.vertex.feature>10&&(last.vertex.feature==1||next.vertex.feature==1)&&last.vertex.feature!=next.vertex.feature)//火曲线上的顶点不能删除
					{
						int huoqu_i,retcode;
						if(dd<-eps)
							bAllProtrusive = FALSE;
						huoqu_i = __max(now.vertex.feature/10,now.vertex.feature%10);
						retcode = Int3dpl(HuoQuLine[huoqu_i-2],
							line,vertex_list[j].vertex);
						if(retcode<=0)
						{	//无合法交点
							PROFILE_VERTEX tempt;
							tempt = vertex_list[j];
							vertex_list[j] = next;
							vertex_list[(j+1)%n]=tempt;
						}
						else
						{
							//下面的语句必须有,因为面号在求交时有可能被冲掉
							vertex_list[j].vertex.feature = now.vertex.feature;
							vertex_list[j].vertex.z = 0;
						}
					}
					else if((now.vertex.feature==2&&next.vertex.feature==3)
							||(now.vertex.feature==3&&next.vertex.feature==2)
							||(last.vertex.feature==2&&now.vertex.feature==3)
							||(last.vertex.feature==3&&now.vertex.feature==2))
						continue;//三面板第二面和第三面的交界点不能删除
					//else if(last.m_bWeldEdge||now.m_bWeldEdge)
					//	continue;//焊缝线端顶点由于有特殊外形要求，不能删除,后发现由于排序原因会导致前后点并不一定是同一焊缝端点，尤其是在合并板时wjh 2011.07.08
					else if (!vertex_list[j].m_bCanCut)
						continue;//特殊属性点不能删去
					else//非火曲线上的顶点
					{
						if(dd<-eps)
							bAllProtrusive = FALSE;
						vertex_list.DeleteAt(j);
						j--;
					}
				}
			}
			if(bAllProtrusive)	//边界多边形是凸多边形
				break;
		}
	}
	//恢复顶点顺序 wht 16-05-20
	if(m_bClockwiseCalProfile)
		ReversedPlateVertexList(vertex_list);
	//先排除凹点再计算火曲点,否则可能因为存在凹点影响火曲点计算 wht 10-04-28
	BOOL bHuoQuLineOverLap=FALSE;
	if(face_N==3&&jdb_style==0)
	{
		GEPOINT bendlinevec0=(HuoQuLine[0].endPt-HuoQuLine[0].startPt).normalized();
		GEPOINT bendlinevec1=(HuoQuLine[1].endPt-HuoQuLine[1].startPt).normalized();
		if(fabs(bendlinevec0*bendlinevec1)>EPS_COS2)
		{	//两火曲线平行 wjh-2018.1.17
			SnapPerp(&top_point,HuoQuLine[0],f3dPoint(0,0,0));
			bHuoQuLineOverLap=TRUE;
		}
		else
		{
			int nRetCode=Int3dll(HuoQuLine[0],HuoQuLine[1],top_point);
			if(nRetCode<=0)
				SnapPerp(&top_point,HuoQuLine[0],f3dPoint(0,0,0));
		}
		top_point.z=0;
	}
	////////////////////////////////////////////////////
	//提前考虑两火曲线交于钢板内点情况 wjh-2018.1.26
	f3dPoint intersHL;
	f2dLine HL1(HuoQuLine[0].startPt,HuoQuLine[0].endPt),HL2(HuoQuLine[1].startPt,HuoQuLine[1].endPt);;
	int status=(face_N!=3)?0:Int2dll(HL1,HL2,intersHL.x,intersHL.y);
	bool bValidInters=(status==-2||status>0);

	n=vertex_list.GetNodeNum();	//删除凹点后应更新顶点数
	for(j=0;j<n;j++)
	{
		last = vertex_list[(n+j-1)%n];
		now  = vertex_list[j];
		next = vertex_list[(j+1)%n];
		if(now.type==0)
		{	//当前边位直线边
			line.startPt = now.vertex;
			line.endPt = next.vertex;
			line.startPt.z = line.endPt.z = 0;
			if(now.vertex.feature+next.vertex.feature==3)
			{	//1面与2面间的火曲点
				if(Int3dpl(line,HuoQuLine[0],inters)<=0)
					continue;	//无交点
				else if(inters.IsEqual(now.vertex,EPS))
					vertex_list[j].vertex.feature=12;	//用于通过合并三块单面板为帽子折叠板式基准面只有火曲点情况wjh 2012-7-10
				else if(inters.IsEqual(next.vertex,EPS))
					vertex_list[(j+1)%n].vertex.feature=12;
				else
				{
					inters.feature = 12;
					PROFILE_VERTEX* pInsertVeterx=vertex_list.insert(PROFILE_VERTEX(inters),j+1);
					pInsertVeterx->m_cPosDesignType=PROFILE_VERTEX::DT_INNERBEND;
					j++;
					n++;
				}
			}
			else if(face_N==3&&now.vertex.feature+next.vertex.feature==4&&now.vertex.feature!=next.vertex.feature)
			{	//1面与3面间的火曲点
				if(Int3dpl(line,HuoQuLine[1],inters)<=0)
					continue;	//无交点
				else
				{
					inters.feature = 13;
					PROFILE_VERTEX* pInsertVeterx=vertex_list.insert(PROFILE_VERTEX(inters),j+1);
					pInsertVeterx->m_cPosDesignType=PROFILE_VERTEX::DT_INNERBEND;
					j++;
					n++;
				}
			}
			else if(face_N==3&&wiLegacyStyle==1&&now.vertex.feature+next.vertex.feature==5)
			{	//折叠板2面与3面间的火曲点，个别情况下折叠板1号基面没有独立的轮廓顶点，只有共用的火曲点 wjh-2017.8.31
				GEPOINT just_vec=next.vertex-now.vertex;
				PROFILE_VERTEX* pInsertVeterx1=NULL,*pInsertVertex2=NULL;
				if(Int3dpl(line,HuoQuLine[0],inters)>0)
				{
					inters.feature = 12;
					pInsertVeterx1=vertex_list.insert(PROFILE_VERTEX(inters),j+1);
					//pInsertVeterx1->m_cPosDesignType=PROFILE_VERTEX::DT_INNERBEND;
					j++;
					n++;
				}
				f3dPoint inters2;
				if(Int3dpl(line,HuoQuLine[1],inters2)>0)
				{
					inters2.feature = 13;
					GEPOINT vec;
					if(pInsertVeterx1!=NULL)
						vec=(now.vertex.feature==2)?(inters2-inters):(inters-inters2);
					int iCurrInsertPos=(now.vertex.feature==2)?j+1:j;
					pInsertVertex2=vertex_list.insert(PROFILE_VERTEX(inters2),iCurrInsertPos);
					//pInsertVertex2->m_cPosDesignType=PROFILE_VERTEX::DT_INNERBEND;
					if(pInsertVeterx1&&vec*just_vec<0&&bValidInters)
					{	//两火曲线交于钢板内点
						GEPOINT verifyvec=intersHL-inters;
						normalize(verifyvec);
						inters=intersHL+verifyvec*(_thick/2);
						pInsertVeterx1->vertex.Set(inters.x,inters.y);

						verifyvec=intersHL-inters2;
						normalize(verifyvec);
						inters2=intersHL+verifyvec*(_thick/2);
						pInsertVertex2->vertex.Set(inters2.x,inters2.y);
					}
					j++;
					n++;
				}
			}
			else if(face_N==3 && bHuoQuLineOverLap && now.vertex.feature+next.vertex.feature==5)
			{	//三面板中两火曲线平行时,需添加火曲面过渡点保证板外形能够包住角钢 2018.1.17
				PROFILE_VERTEX* pInsertVertex1=NULL,*pInsertVertex2=NULL,*pInsertVertex3=NULL,*pInsertVertex4=NULL;
				CDesignLjPartPara* pLjPart1=GetLjRodPara(now.m_hRod);
				CDesignLjPartPara* pLjPart2=GetLjRodPara(next.m_hRod);
				BOOL bCalValidPt=FALSE;
				if(pLjPart1 && pLjPart2 && pLjPart1->hPart!=pLjPart2->hPart)
				{
					f3dPoint line_vec1,line_vec2,pt;
					if(pLjPart1->pCurLinePart==NULL)
						pLjPart1->pCurLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(now.m_hRod);
					line_vec1=(pLjPart1->pCurLinePart->End()-pLjPart1->pCurLinePart->Start()).normalized();
					if(pLjPart1->start0_end1==1)
						line_vec1*=-1;
					vector_trans(line_vec1,ucs,FALSE);
					line_vec1.z=0;
					//
					if(pLjPart2->pCurLinePart==NULL)
						pLjPart2->pCurLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(next.m_hRod);
					line_vec2=(pLjPart2->pCurLinePart->End()-pLjPart2->pCurLinePart->Start()).normalized();
					if(pLjPart2->start0_end1==1)
						line_vec2*=-1;
					vector_trans(line_vec2,ucs,FALSE);
					line_vec2.z=0;
					f3dPoint datumTopPoint=top_point;
					datumTopPoint.y+=_peakVertexOffsetY;
					f3dPoint ptS1=now.vertex,ptE1=now.vertex-line_vec1*_extendVertexOffsetLen;
					f3dPoint ptS2=next.vertex,ptE2=next.vertex-line_vec2*_extendVertexOffsetLen;
					//if(Int3dll(ptS1,ptE1,ptS2,ptE2,inters)==1)
					if(IsHuoQuLineCollinear3FacePlate())
					{
						f3dPoint upDatumPoint=datumTopPoint,downDatumPoint=datumTopPoint;
						if(line_vec1.y>0)
							upDatumPoint.y+=_peakPointMargin;
						else
							upDatumPoint.y-=_peakPointMargin;
						if(line_vec2.y>0)
							downDatumPoint.y+=_peakPointMargin;
						else
							downDatumPoint.y-=_peakPointMargin;
						f3dPoint mid_line_vec=(line_vec1+line_vec2)*0.5;
						mid_line_vec.y=mid_line_vec.z=0;
						normalize(mid_line_vec);
						pt=upDatumPoint+mid_line_vec*_extendVertexOffsetLen;
						pInsertVertex1=vertex_list.insert(PROFILE_VERTEX(pt),j+1);
						pInsertVertex1->vertex.feature=now.vertex.feature;
						pInsertVertex1->m_cPosDesignType=PROFILE_VERTEX::DT_BEND_EXTEND_POINT;
						j++;
						n++;
						if(fabs(_peakVertexOffsetY)>0)
						{	//调整top_point位置
							SnapPerp(&top_point,HuoQuLine[0],f3dPoint(0,0,0));
							top_point.y+=_peakVertexOffsetY;
						}
						//
						pt=downDatumPoint+mid_line_vec*_extendVertexOffsetLen;
						pInsertVertex2=vertex_list.insert(PROFILE_VERTEX(pt),j+1);
						pInsertVertex2->vertex.feature=next.vertex.feature;
						pInsertVertex2->m_cPosDesignType=PROFILE_VERTEX::DT_BEND_EXTEND_POINT;
						j++;
						n++;
						//将参数存储到轮廓点中 wht 18-12-18
						extendVertexOffsetLen=_extendVertexOffsetLen;
						peakPointMargin=_peakPointMargin;
						peakVertexOffsetY=_peakVertexOffsetY;
						bCalValidPt=TRUE;
					}
				}
				if(!bCalValidPt)
				{
					GEPOINT just_vec=(next.vertex-now.vertex).normalized();
					GEPOINT mid_pt=(next.vertex+now.vertex)*0.5;
					inters=mid_pt-just_vec*1;
					pInsertVertex1=vertex_list.insert(PROFILE_VERTEX(inters),j+1);
					pInsertVertex1->vertex.feature=now.vertex.feature;
					j++;
					n++;
					//
					inters=mid_pt+just_vec*1;
					pInsertVertex2=vertex_list.insert(PROFILE_VERTEX(inters),j+1);
					pInsertVertex2->vertex.feature=next.vertex.feature;
					j++;
					n++;
				}
			}
			else
				continue;
		}
		else if(now.type<4)
		{	//当前边为圆弧边
			f2dLine huoqu_line2d;	//当前火曲线
			if(now.vertex.feature+next.vertex.feature==3)
			{
				huoqu_line2d.startPt.Set(HuoQuLine[0].startPt.x,HuoQuLine[0].startPt.y);
				huoqu_line2d.endPt.Set(HuoQuLine[0].endPt.x,HuoQuLine[0].endPt.y);
			}
			else if(face_N==3&&now.vertex.feature+next.vertex.feature==4&&now.vertex.feature!=next.vertex.feature)
			{
				huoqu_line2d.startPt.Set(HuoQuLine[1].startPt.x,HuoQuLine[1].startPt.y);
				huoqu_line2d.endPt.Set(HuoQuLine[1].endPt.x,HuoQuLine[1].endPt.y);
			}
			else
				continue;
			//获取圆弧边
			int sign=1;
			f3dArcLine arcLine;
			if(now.type==1)
			{	//指定圆弧夹角
				if(now.sector_angle>0)
					sign=-1;
				arcLine.CreateMethod2(now.vertex,next.vertex,f3dPoint(0,0,sign),now.sector_angle);
			}
			else if(now.type==2)
			{	//指定R
				if(now.radius<0)
					sign=-1;
				arcLine.CreateMethod3(now.vertex,next.vertex,f3dPoint(0,0,sign),sign*now.radius,now.center);
				//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wht 12-06-14
				//arcLine.CreateMethod2(now.vertex,next.vertex,f3dPoint(0,0,sign),sector_angle);
			}
			else if(now.type==3)
			{	//椭圆弧
				f3dPoint center = now.center;
				f3dPoint col_axis=now.column_norm;
				coord_trans(center,ucs,FALSE);
				vector_trans(col_axis,ucs,FALSE);
				if(now.radius<0)
					sign=-1;
				arcLine.CreateEllipse(center,now.vertex,next.vertex,col_axis,f3dPoint(0,0,sign),sign*now.radius);
			}
			else 
				continue;
			f2dArc arc;
			arc.angle=arcLine.SectorAngle();
			arc.centre=arcLine.Center();
			if(arcLine.WorkNorm().z>0)
				arc.startPt=arcLine.Start();
			else
				arc.startPt=arcLine.End();
			//火曲线与圆弧边求交
			f3dPoint inter_vertex,inter_vertex2;
			int nRetCode=Int2dla(huoqu_line2d,arc,inter_vertex.x,inter_vertex.y,inter_vertex2.x,inter_vertex2.y);
			if(nRetCode>0)
			{	//此处暂未考虑直线与(椭)圆弧有两交点时的交点取舍问题
				if(now.type==3)
				{	//椭圆弧
					f3dPoint inters1,inters2;
					f3dPoint pick(huoqu_line2d.startPt.x,huoqu_line2d.endPt.y);
					f3dPoint vec=f3dPoint(huoqu_line2d.startPt.x,huoqu_line2d.startPt.y)
						-f3dPoint(huoqu_line2d.endPt.x,huoqu_line2d.endPt.y);
					normalize(vec);
					coord_trans(pick,ucs,TRUE);
					vector_trans(vec,ucs,TRUE);
					int nRet=Int3dlc(&inters1,&inters2,pick,vec,now.center-now.column_norm*1000,
						now.center+now.column_norm*1000,fabs(now.radius),fabs(now.radius));
					if(nRet>0)
					{
						coord_trans(inters1,ucs,FALSE);
						coord_trans(inters2,ucs,FALSE);
						inters1.z=inters2.z=0;
						if(nRet==2&&DISTANCE(inter_vertex,inters1)>DISTANCE(inter_vertex,inters2))
							inter_vertex=inters2;
						else
							inter_vertex=inters1;
					}
				}
				PROFILE_VERTEX *pArcVertex=vertex_list.insert(now,j+1);
				pArcVertex->vertex=inter_vertex;
				if(now.vertex.feature+next.vertex.feature==3)
					pArcVertex->vertex.feature=12;
				else if(face_N==3&&now.vertex.feature+next.vertex.feature==4&&now.vertex.feature!=next.vertex.feature)
					pArcVertex->vertex.feature=13;
				if(pArcVertex->type==1)
				{	//指定圆弧夹角
					int sign=1;
					if(now.sector_angle<0)
						sign=-1;
					double half_len = 0.5*DISTANCE(now.vertex,inter_vertex);
					if(half_len>=fabs(now.radius))
						now.sector_angle = sign*Pi;
					else
						now.sector_angle = sign*2*asin(half_len/fabs(now.radius));
					pArcVertex->sector_angle=sign*(fabs(arc.angle)-fabs(now.sector_angle));
				}
				j++;
				n++;
			}
		}
	}

	if(face_N==3&&wiLegacyStyle==1)
	{	//处理折叠板1号基面无独立轮廓点情况（把临时添加的1号面点改为相应的火曲点） wjh-2017.8.31
		pPrevPnt=vertex_list.GetTail();
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pPrevPnt->vertex.feature+pVertex->vertex.feature==4&&pPrevPnt->vertex.feature!=pVertex->vertex.feature)
			{
				if(pVertex->vertex.feature==1)
					pVertex->vertex.feature=13;
				else if(pPrevPnt->vertex.feature==1)
					pPrevPnt->vertex.feature=13;
			}
			else if(pPrevPnt->vertex.feature+pVertex->vertex.feature==3)
			{
				if(pVertex->vertex.feature==1)
					pVertex->vertex.feature=12;
				else if(pPrevPnt->vertex.feature==1)
					pPrevPnt->vertex.feature=12;
			}
			pPrevPnt=pVertex;
		}
	}
	//以下代码用于处理三面板时火曲点与两火曲线交点间的矛盾 2005/08/19 WJH
	j=0;
	pPrevPnt=NULL;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pPrevPnt&&pPrevPnt->vertex.feature>10&&pVertex->vertex.feature>10&&jdb_style==0&&face_N==3)
		{
			vertex_list.DeleteCursor();
			vertex_list.DeleteAt(j-1);
		}
		pPrevPnt=pVertex;
		j++;
	}
	pPrevPnt=vertex_list.GetTail();
	pVertex=vertex_list.GetFirst();
	if(pPrevPnt&&pPrevPnt->vertex.feature>10&&pVertex->vertex.feature>10&&jdb_style==0&&face_N==3)
	{
		vertex_list.GetFirst();
		vertex_list.DeleteCursor();
		vertex_list.GetTail();
		vertex_list.DeleteCursor();
	}
	if(designInfo.iFaceType==7&&face_N==3)
	{	//删除与top_point重叠的火曲点
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.IsEqual(top_point))
			{
				vertex_list.DeleteCursor(TRUE);
				break;
			}
		}
	}
	if(face_N==2)
	{	//根据火曲面在火曲线的位置标记,更新火曲面上轮廓点面号 wxc-2017.12.5
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.feature>10)
				continue;	//火曲点
			if((pVertex->vertex-HuoQuLine[0].startPt)*vSpreadBendVertVec>0)
				pVertex->vertex.feature=2;
		}
	}
}

void CLDSPlate::DesignProtrusiveProfile()
{
	int j;
	double dd;
	f3dPoint last,now,next;
	f3dLine line;

	//------^^^^^^------对各角钢上的连接板多边形顶点按逆时针进行排序
	for(;;)//一次计算有可能不能完全将凹多边形削为凸多边形
	{
		BOOL bAllProtrusive=TRUE;
		for(j=0;j<vertex_list.GetNodeNum();j++)
		{
			if(j==0)
				last = vertex_list[vertex_list.GetNodeNum()-1].vertex;
			else
				last = vertex_list[j-1].vertex;
			now  = vertex_list[j].vertex;
			if(j+1>=vertex_list.GetNodeNum())
				next = vertex_list[0].vertex;
			else
				next = vertex_list[j+1].vertex;
			if(DISTANCE(last,now)<EPS)	//两点过于接近删除一点
			{
				vertex_list.DeleteAt(j);
				j--;
				continue;
			}
			dd = DistOf2dPtLine(next,last,now);
			line.startPt = last;
			line.endPt = next;
			if(dd<-eps||line.PtInLine(now)>0)	//-eps不可改为0否则会死机(进入死循环)
			{
				if(dd<-eps)
					bAllProtrusive = FALSE;
				vertex_list.DeleteAt(j);
				j--;
			}
		}
		if(bAllProtrusive)	//边界多边形是凸多边形
			break;
	}
}

bool CLDSGeneralPlate::UpdateHoleWall(HOLE_WALL* pHoleWall)	//当螺栓或钢板发生变化时，实时更新孔壁
{
	int iCurSelItem=-1;
	for(int i=0;i<4;i++)
	{
		if(pHoleWall->items[i].hRelaPart==handle)
		{
			iCurSelItem=i;
			break;
		}
	}
	if(iCurSelItem==-1)
		return false;
	GEPOINT axis_z=ucs.axis_z.normalized();
	GEPOINT qunorm[2],qufacepick[2];
	GEPOINT vNormOffset;
	if(GetNormOffset()!=0)
		vNormOffset=axis_z*GetNormOffset();
	GEPOINT origin=ucs.origin+vNormOffset;
	for(int i=1;i<GetFaceN();i++)
	{
		qunorm[i-1]=GetHuoquFaceNorm(i-1).normalized();
		qufacepick[i-1]=GetHuoquLine(i-1).startPt;
		coord_trans(qufacepick[i-1],ucs,TRUE);//,TRUE);
		qufacepick[i-1]+=vNormOffset;
	}
	double worklen=0;
	GEPOINT worklenvec=pHoleWall->vEnd-pHoleWall->vStart;
	GEPOINT worknorm=pHoleWall->pBolt->get_norm();
	if(!worklenvec.IsZero())
	{
		if(worklenvec*pHoleWall->pBolt->get_norm()<0)
			worknorm=-pHoleWall->pBolt->get_norm();
		worklen=worknorm*worklenvec;
	}
	GEPOINT vIntersStart,vIntersEnd,vCurrPlanePick=origin,vCurrPlaneNorm=axis_z;
	double justifies[3]={1.0,0,0},justify=worknorm*axis_z;
	if(GetFaceN()>1)
	{
		justifies[0]=worknorm*axis_z;
		for(i=1;i<GetFaceN();i++)
			justifies[i]=worknorm*qunorm[i-1];
	}
	if(fabs(justifies[1])>fabs(justifies[0])&&fabs(justifies[1])>fabs(justifies[2]))
	{
		justify=justifies[1];
		vCurrPlanePick=qufacepick[0];
		vCurrPlaneNorm=qunorm[0];
	}
	else if(fabs(justifies[2])>fabs(justifies[0])&&fabs(justifies[2])>fabs(justifies[1]))
	{
		justify=justifies[2];
		vCurrPlanePick=qufacepick[1];
		vCurrPlaneNorm=qunorm[1];
	}
	Int3dlf(vIntersStart,pHoleWall->pBolt->ucs.origin,worknorm,vCurrPlanePick,vCurrPlaneNorm);
	pHoleWall->items[iCurSelItem].ptS=vIntersStart;
	pHoleWall->items[iCurSelItem].ptE=vIntersStart+worknorm*(GetThick()/justify);
	if(worklen==0)
	{
		pHoleWall->vStart=vIntersStart;
		pHoleWall->vEnd=vIntersStart+worknorm*(GetThick()/justify);
	}
	else
	{
		GEPOINT vLenVecToStart=vIntersStart-pHoleWall->vStart;
		GEPOINT vLenVecToEnd=vLenVecToStart+worknorm*(GetThick()/justify);
		if(justify<0)
		{
			GEPOINT v=vLenVecToStart;
			vLenVecToStart=vLenVecToEnd;
			vLenVecToEnd=vLenVecToStart;
		}
		if(vLenVecToStart*worknorm<0)
			pHoleWall->vStart=vIntersStart;
		if(vLenVecToEnd*worknorm>worklen)
			pHoleWall->vEnd=pHoleWall->vStart+vLenVecToEnd;
	}
	return true;
}
int CLDSGeneralPlate::GatherHoleWalls(CHash64ListEx<HOLE_WALL>& hashHoles)
{
	int gather_count=0;
	HOLE_WALL* pHoleWall;
	CXhSimpleList<HOLE_WALL*> listHoles;
	if(CLDSObject::sysPara->HoleWallDisplayMode()==0)
		return 0;
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		if( (CLDSObject::sysPara->HoleWallDisplayMode()==1&&!pLsRef->GetLsPtr()->m_bVirtualPart)||(
			!CLDSObject::sysPara->IsDisplayAllHole()&&!m_bDisplayHole&&!pLsRef->GetLsPtr()->m_bVirtualPart))
			continue;
		pHoleWall=hashHoles.Add(DUALKEY((*pLsRef)->handle,handle));
		pHoleWall->pBolt=pLsRef->GetLsPtr();
		for(int j=0;j<4;j++)
		{
			if(pHoleWall->items[j].hRelaPart==handle)
				break;
			else if(pHoleWall->items[j].hRelaPart==0)
			{
				pHoleWall->items[j].hRelaPart=handle;
				break;
			}
		}
		listHoles.AttachObject(pHoleWall);
		gather_count++;
	}
	if(gather_count==0)
		return gather_count;
	//计算孔壁的始端和终端坐标
	for(HOLE_WALL** ppHole=listHoles.EnumObjectFirst();ppHole;ppHole=listHoles.EnumObjectNext())
		UpdateHoleWall(*ppHole);
	return gather_count;
}
struct WELDING_PLATE : public ICompareClass{
	CLDSGeneralPlate* pWeldPlate;
	double alfa;	//弧度制，
	//double innerR;	//焊烧死角处工艺流水孔半径
	WORD BG,BS;	//BG:流水孔到管壁距离;BS;流水孔到立板壁距离
	GEPOINT vxRayVec;
	WELDING_PLATE(CLDSGeneralPlate* pRelaPlate=NULL){
		pWeldPlate=pRelaPlate;
		alfa=0;	//需要后续计算
		BG=BS=25;	//BG:流水孔到管壁距离;BS;流水孔到立板壁距离
	}
	virtual int Compare(const ICompareClass *pCompareObj)const
	{
		WELDING_PLATE* pCover2=(WELDING_PLATE*)pCompareObj;
		if(alfa>pCover2->alfa)
			return -1;
		else if(alfa<pCover2->alfa)
			return  1;
		else
			return 0;
	}
};
UINT CLDSGeneralPlate::AddMiscellaneousHoles(IXhList<HOLE>* pListHoles)	//在GetHoleSet内调用,以添加除典型螺栓孔外的杂项孔
{
	UINT incremental=0;
	CSuperSmartPtr<CLDSGeneralPlate>pPlate=this;
	if(wiType==TYPE_RIBPLATE)
	{
		double H=0,G=0,F=0;
		GetDesignItemValue(KEY2C("MH"),&H);
		if(H>0)// && (m_bDisplayHole || bDisplayHole))	
		{
			GetDesignItemValue(KEY2C("MG"),&G);
			GetDesignItemValue(KEY2C("MF"),&F);
			//
			f3dPoint along_axisvec;
			long hDatumPart=0,hAssisPlate1=0;
			GetDesignItemValue('A',&hDatumPart);			//基准构件
			GetDesignItemValue(KEY2C("C"),&hAssisPlate1);	//辅助钢板1
			CLDSPart* pDatumPart=BelongModel()->FromPartHandle(abs(hDatumPart));
			CLDSPart* pAssistantPart1=BelongModel()->FromPartHandle(abs(hAssisPlate1));
			if(pDatumPart&&pAssistantPart1)
			{	//角肋板上的通孔绘制
				along_axisvec=pAssistantPart1->ucs.axis_z^ucs.axis_z;	//沿1号辅板的角焊缝走向
				if(along_axisvec*ucs.axis_y<0)
					along_axisvec*=-1.0;
				vector_trans(along_axisvec,ucs,FALSE,TRUE);
				normalize(along_axisvec);
				double k=0;
				if(along_axisvec.y>eps)
					k=along_axisvec.x/along_axisvec.y;
				HOLE hole;
				hole.xLocalPosition.Set(G+F*k,F);
				hole.wBoltD=0;
				hole.fHoleD=float(H);
				pListHoles->Append(hole);
				incremental++;
			}
		}
	}
	else if(wiType==TYPE_CIRRIBPLATE||wiType==TYPE_TUBERIBPLATE)
	{	//环形肋板
		double BH=0;
		GetDesignItemValue(KEY2C("BH"),&BH);	//流水孔直径,BH=0表示不生成流水孔
		if(BH<=EPS)
			return 0;
		long hTube=0;
		GetDesignItemValue('C',&hTube);
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hTube,CLS_LINETUBE);
		if(!pDatumTube)	//如基准钢管为空 可能是基准钢管没当做参数存储时做的环向板
			pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
		if(pDatumTube==NULL)
			return 0;
		//参数化板的buffer中不保存顶点的计算句柄,第一次绘制实体时需要重新设计钢板来获取顶点的设计句柄
		BOOL bNeedDesPlate=TRUE;
		for(int index=0;index<vertex_list.GetNodeNum();index++)
		{
			if(vertex_list[index].m_hRod>0x20)
			{
				bNeedDesPlate=FALSE;
				break;
			}
		}
		if(bNeedDesPlate)
			((CLDSParamPlate*)this)->DesignPlate();
		//绘制工艺孔
		DESIGN_PARAM itemparam;
		DESIGN_PARAM::DUAL_VALUE rela_part1,rela_part2;
		if(GetDesignItemValue('E',&itemparam)&&itemparam.cValueType==3)	//第一块定位基准钢板
		{
			rela_part1.hPart=itemparam.value.hPart;
			rela_part1.uiAttribute=itemparam.value.uiAttribute;
		}
		if(GetDesignItemValue('F',&itemparam)&&itemparam.cValueType==3)	//第二块定位基准钢板
		{
			rela_part2.hPart=itemparam.value.hPart;
			rela_part2.uiAttribute=itemparam.value.uiAttribute;
		}
		GEPOINT center;
		Int3dlf(center,f3dLine(pDatumTube->Start(),pDatumTube->End()),ucs.origin,ucs.axis_z);
		coord_trans(center,ucs,FALSE,TRUE);
		int num=vertex_list.GetNodeNum();
		for(int index=0;index<num;index++)
		{
			if(vertex_list[index].m_hRod<0x20)
				continue;
			WORD BG=0,BS=0;
			if(vertex_list[index].m_hRod==rela_part1.hPart)
			{
				WORD* pw_BGorBS=(WORD*)&rela_part1.uiAttribute;
				BG=*pw_BGorBS;
				BS=*(pw_BGorBS+1);
			}
			else if(vertex_list[index].m_hRod==rela_part2.hPart)
			{
				WORD* pw_BGorBS=(WORD*)&rela_part2.uiAttribute;
				BG=*pw_BGorBS;
				BS=*(pw_BGorBS+1);
			}
			if(BG<=0||BS<=0)
				continue;
			//计算流水孔坐标
			f3dPoint ptS,ptE,vec,off_vec;
			ptS=vertex_list[index].vertex;
			if(vertex_list[index].type==0)
			{
				ptE=vertex_list[(index+1)%num].vertex;
				vec=(ptE-ptS).normalized();
				off_vec.Set(-vec.y,vec.x,0);
			}
			else
			{
				ptE=vertex_list[(index-1+num)%num].vertex;
				vec=(ptE-ptS).normalized();
				off_vec.Set(vec.y,-vec.x,0);
			}
			ptS+=off_vec*BS;
			ptE+=off_vec*BS;
			f2dLine line2d(f2dPoint(ptS.x,ptS.y),f2dPoint(ptE.x,ptE.y));
			f2dCircle cir(pDatumTube->GetDiameter()*0.5+BG,center.x,center.y);
			GEPOINT inters,inters1,inters2;
			Int2dlc(line2d,cir,inters1.x,inters1.y,inters2.x,inters2.y);
			inters=((inters1-center)*vec>0)?inters1:inters2;
			//
			HOLE hole;
			hole.wBoltD=0;
			hole.fHoleD=float(BH);
			hole.xLocalPosition=inters;
			hole.vHoleAxisVec=ucs.axis_z;
			hole.xMcsPosition=hole.xLocalPosition;
			coord_trans(hole.xMcsPosition,ucs,TRUE,TRUE);
			pListHoles->Append(hole);
			incremental+=1;
		}
	}
	else if(wiType==TYPE_CIRCOVERPLATE)
	{
		double BETA=0,BH=0,R=0,N=0;
		GetDesignItemValue('B',&BETA);			//扇形夹角
		GetDesignItemValue('R',&R);
		GetDesignItemValue('N',&N);
		GetDesignItemValue(KEY2C("BH"),&BH);	//流水孔直径,BH=0表示不生成流水孔
		if(BH<=EPS)
			return 0;
		long hPart=0,Y=0;
		GetDesignItemValue('Y',&Y);
		GetDesignItemValue('C',&hPart);	//定位基准钢管
		if(Y==3)
		{	//直角封板，根据封板长度计算beta
			double fCosa=sqrt(SQR(R)-SQR(N*0.5))/R;
			BETA=ACOS(fCosa)*2*DEGTORAD_COEF;
		}
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(hPart,CLS_LINETUBE);
		if(!pDatumTube)	//如基准钢管为空 可能是基准钢管没当做参数存储时做的环向板
			pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
		if(pDatumTube==NULL)
			return 0;
		//处理封板上分布于覆盖立板两侧的工艺流水孔（含半圆孔或圆孔两种类型）
		UINT i,j;
		DESIGN_PARAM itemparam;
		WELDING_PLATE* pWeldCoverPlate=NULL;
		CLDSGeneralPlate *pDatumPlate1=NULL,*pDatumPlate2=NULL,*pWeldingPlate=NULL;
		PRESET_ARRAY8<WELDING_PLATE> arrWeldPlates;	//封板所盖住的其它关联钢板（可能没有定位关系，但有焊接需要定流水孔） wjh-2017.12.30
		if(GetDesignItemValue('F',&itemparam)&&itemparam.cValueType==3)	//第一块定位基准钢板
		{
			pDatumPlate1=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(itemparam.hValue,CLS_PLATE,CLS_PARAMPLATE);
			if(pDatumPlate1!=NULL)
			{
				WORD* pw_BGorBS=(WORD*)&itemparam.value.uiAttribute;
				pWeldCoverPlate=arrWeldPlates.Append(WELDING_PLATE(pDatumPlate1));
				pWeldCoverPlate->BG=*pw_BGorBS;
				pWeldCoverPlate->BS=*(pw_BGorBS+1);
			}
		}
		if(GetDesignItemValue('G',&itemparam)&&itemparam.cValueType==3)	//第二块定位基准钢板
		{
			if((pDatumPlate2=(CLDSPlate*)BelongModel()->FromPartHandle(itemparam.hValue,CLS_PLATE))!=NULL)//,CLS_PARAMPLATE
			{
				WORD* pw_BGorBS=(WORD*)&itemparam.value.uiAttribute;
				pWeldCoverPlate=arrWeldPlates.Append(WELDING_PLATE(pDatumPlate2));
				pWeldCoverPlate->BG=*pw_BGorBS;
				pWeldCoverPlate->BS=*(pw_BGorBS+1);
			}
		}
		for(i=0;i<4;i++)
		{
			CXhChar16 idstr("P%d",i);
			if(GetDesignItemValue(KEY2C(idstr),&itemparam)&&itemparam.cValueType==3)
			{
				if(pDatumPlate1!=NULL&&pDatumPlate1->handle==itemparam.hValue)
					continue;
				else if(pDatumPlate2!=NULL&&pDatumPlate2->handle==itemparam.hValue)
					continue;
				for(j=0;j<arrWeldPlates.Count;j++)
				{
					pWeldCoverPlate=&arrWeldPlates.At(j);
					if(pWeldCoverPlate->pWeldPlate->handle==itemparam.hValue)
						break;
				}
				if(j==arrWeldPlates.Count)
					pWeldingPlate=(CLDSGeneralPlate*)BelongModel()->FromPartHandle(itemparam.hValue,CLS_PLATE,CLS_PARAMPLATE);
				if(pWeldingPlate!=NULL)
				{
					WORD* pw_BGorBS=(WORD*)&itemparam.value.uiAttribute;
					pWeldCoverPlate=arrWeldPlates.Append(WELDING_PLATE(pWeldingPlate));
					pWeldCoverPlate->BG=*pw_BGorBS;
					pWeldCoverPlate->BS=*(pw_BGorBS+1);
				}
			}
		}
		int plateIndexArr[2]={0,1};
		double fInitEdgeAngle=(Y==0||Y==3)?(-BETA*0.5)*RADTODEG_COEF:0;
		for(i=0;i<arrWeldPlates.Count;i++)
		{
			WELDING_PLATE* pWeldCover=&arrWeldPlates.At(i);
			GEPOINT vxRayVec=pDatumTube->GetPlateVertSpreadVec(pWeldCover->pWeldPlate);
			bool bTransSectPlate=false;
			if(vxRayVec.IsZero()&&Y==0&&arrWeldPlates.Count==1)	//贯穿板
			{
				GEPOINT vTubeAxisZ=pDatumTube->End()-pDatumTube->Start();
				normalize(vTubeAxisZ);
				vxRayVec=vTubeAxisZ^pWeldCover->pWeldPlate->ucs.axis_z;
				normalize(vxRayVec);
				bTransSectPlate=true;
			}
			vector_trans(vxRayVec,ucs,FALSE,TRUE);
			arrWeldPlates.At(i).vxRayVec=vxRayVec;
			double ray_alfa=Cal2dLineAng(0,0,vxRayVec.x,vxRayVec.y)-fInitEdgeAngle;
			pWeldCover->alfa=(ray_alfa>=Pi*2)?ray_alfa-Pi*2:ray_alfa;
			if(bTransSectPlate && pWeldCover->alfa>BETA*RADTODEG_COEF)
			{
				pWeldCover->alfa-=Pi;
				arrWeldPlates.At(i).vxRayVec=-vxRayVec;
			}
		}
		CHeapSort<WELDING_PLATE>::HeapSortClassic(arrWeldPlates.pPresetObjs,arrWeldPlates.Count);
		double betaByRad=BETA*RADTODEG_COEF;
		for(i=0;i<arrWeldPlates.Count;i++)
		{
			WELDING_PLATE* pWeldCover=&arrWeldPlates.At(i);
			CLDSGeneralPlate *pPlate=pWeldCover->pWeldPlate;
			GEPOINT axis_x=pWeldCover->vxRayVec;
			if(Y==0&&(pWeldCover->alfa>(betaByRad-RADTODEG_COEF)||pWeldCover->alfa<RADTODEG_COEF))
				continue;	//超出封板扇形角范围
			if(Y!=0&&(pWeldCover->alfa>betaByRad || pWeldCover->alfa<0))
				continue;
			if(pPlate==NULL||axis_x.IsZero())
				continue;	//数据有误
			HOLE hole;
			hole.wBoltD=0;
			hole.fHoleD=float(BH);
			f2dLine line2d;
			GEPOINT center,axis_y(-axis_x.y,axis_x.x),inters,inters2;
			Int3dlf(center,f3dLine(pDatumTube->Start(),pDatumTube->End()),ucs.origin,ucs.axis_z);
			coord_trans(center,ucs,FALSE,TRUE);
			f2dCircle cir(pDatumTube->GetDiameter()*0.5+pWeldCover->BG,center.x,center.y);
			//感觉可能用不上center.z=0;
			double ro=pDatumTube->GetDiameter()*0.5+200;
			line2d.startPt=center;//+pDatumTube->GetDiameter()*0.5*axis_x;
			line2d.endPt=center+ro*axis_x;
			double BSU=pWeldCover->BS,BSD=pWeldCover->BS;
			double fVertPlateNormOffset=pPlate->GetNormOffset();
			if(fabs(fVertPlateNormOffset-pPlate->Thick*0.5)<1)
				BSU=BSD=pWeldCover->BS+fVertPlateNormOffset;	//射线方向在立板中性层上
			else
			{
				GEPOINT vxMcsRayVec=axis_x;
				vector_trans(vxMcsRayVec,ucs,TRUE,TRUE);
				if(vxMcsRayVec*pPlate->ucs.axis_z>0)
				{
					BSU+=fVertPlateNormOffset+pPlate->Thick;
					BSD-=fVertPlateNormOffset;
				}
				else
				{
					BSD+=fVertPlateNormOffset+pPlate->Thick;
					BSU-=fVertPlateNormOffset;
				}
			}
			
			if(Y>=0||Y==3)
			{	//基板外侧
				f2dLine line2dUpper;
				line2dUpper.startPt.Set(line2d.startPt.x+axis_y.x*BSU,line2d.startPt.y+axis_y.y*BSU);
				line2dUpper.endPt.Set(line2d.endPt.x+axis_y.x*BSU,line2d.endPt.y+axis_y.y*BSU);
				Int2dlc(line2dUpper,cir,inters.x,inters.y,inters2.x,inters2.y);
				GEPOINT intersUpperNear=((inters-center)*axis_x>0)?inters:inters2;
				hole.xLocalPosition=intersUpperNear;
				hole.vHoleAxisVec=ucs.axis_z;
				hole.xMcsPosition=hole.xLocalPosition;
				coord_trans(hole.xMcsPosition,ucs,TRUE,TRUE);
				pListHoles->Append(hole);
				incremental+=1;
			}
			if(Y<=0||Y==3)
			{	//基板内侧
				f2dLine line2dDown;
				line2dDown.startPt.Set(line2d.startPt.x-axis_y.x*BSD,line2d.startPt.y-axis_y.y*BSD);
				line2dDown.endPt.Set(line2d.endPt.x-axis_y.x*BSD,line2d.endPt.y-axis_y.y*BSD);
				Int2dlc(line2dDown,cir,inters.x,inters.y,inters2.x,inters2.y);
				GEPOINT intersDownNear=((inters-center)*axis_x>0)?inters:inters2;
				hole.xLocalPosition=intersDownNear;
				hole.vHoleAxisVec=ucs.axis_z;
				hole.xMcsPosition=hole.xLocalPosition;
				coord_trans(hole.xMcsPosition,ucs,TRUE,TRUE);
				pListHoles->Append(hole);
				incremental+=1;
			}
		}
	}
	else if(wiType==TYPE_ELBOWPLATE)
	{	//肘板焊角镀锌孔
		double H,G=0,F=0;
		GetDesignItemValue('H',&H);
		if(H>0)	//肘板带孔
		{
			long L=0,K=0,V=0;
			GetDesignItemValue('G',&G);
			GetDesignItemValue('F',&F);
			if(!GetDesignItemValue('V',&V))
				V=0;	//是否为竖直带自动正位的肋板
			GEPOINT line_vec;
			CLDSPart *pPlate=NULL;
			CLDSLineTube *pDatumTube=NULL;
			if(GetDesignItemValue('L',&L))	//获得基准底板
				pPlate=BelongModel()->FromPartHandle(L,CLS_PLATE,CLS_PARAMPLATE);
			if(GetDesignItemValue('K',&K))	//基准钢管
			{
				pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(abs(K),CLS_LINETUBE);
				if(pDatumTube&&K>0)
					line_vec=pDatumTube->End()-pDatumTube->Start();
				else if(pDatumTube&&K<0)
					line_vec=pDatumTube->Start()-pDatumTube->End();
			}
			else
			{
				GetDesignItemValue('x', &line_vec.x);
				GetDesignItemValue('y', &line_vec.y);
				GetDesignItemValue('z', &line_vec.z);
			}
			normalize(line_vec);
			vector_trans(line_vec,ucs,FALSE,TRUE);
			line_vec.z=0;

			CHoleMaker holeMaker;
			UCS_STRU hole_ucs;
			if(V&&pPlate&&pDatumTube)
			{
				f3dPoint vertex=vertex_list.GetTail()->vertex;
				line_vec.Set(vertex.x,vertex.y,0);
				normalize(line_vec);
			}
			double k=0;
			if(line_vec.y>eps)//防止line_vec.y=0 
				k=line_vec.x/line_vec.y;
			HOLE hole;
			hole.xLocalPosition.Set(G+F*k,F);
			hole.wBoltD=0;
			hole.fHoleD=(float)H;
			pListHoles->Append(hole);
			incremental++;
		}
	}
	else if(wiType==TYPE_LINEANGLERIBPLATE)
	{	//角钢肋板上的杂项孔
		long I=0;
		double d=0,P=0,R=0;
		GetDesignItemValue('I',&I);
		GetDesignItemValue('d',&d);
		GetDesignItemValue('P',&P);
		GetDesignItemValue('R',&R);
		HOLE hole;
		hole.wBoltD=0;
		hole.fHoleD=(float)d;
		if(P>0&&d>0&&I==0)
		{
			long A;
			GetDesignItemValue('A',&A);
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(A,CLS_LINEANGLE);
			if(pLineAngle)
			{
				double alfa=pLineAngle->GetRadWingAngle();
				double sina=sin(alfa);
				double cosa=cos(alfa);
				double c=0;
				P+=pLineAngle->GetThick();
				if(fabs(cosa)>EPS_COS)
				{	//以防出现除0
					c=P;
					P=0;
				}
				else
					c=P*sina/(1-cosa);
				f3dPoint datum_pos,centre;
				f3dPoint line_vec=pLineAngle->End()-pLineAngle->Start();
				normalize(line_vec);
				Int3dlf(datum_pos,pLineAngle->Start(),line_vec,ucs.origin,ucs.axis_z);
				centre=datum_pos+pLineAngle->GetWingVecY()*c-pLineAngle->get_norm_y_wing()*P;
				coord_trans(centre,ucs,FALSE);
				hole.xLocalPosition=centre;
				pListHoles->Append(hole);
				incremental++;
			}
		}
		else if(d>0&&R>d*0.5&&I==2)
		{
			double L=0,W=0;
			GetDesignItemValue('L',&L);
			GetDesignItemValue('W',&W);
			for(int i=0;i<2;i++)
			{
				int sign=(i==0)?1:-1;
				hole.xLocalPosition.Set(sign*(L*0.5-R),W-R,0);
				pListHoles->Append(hole);
				incremental++;
			}
		}
	}
	else if(wiType==TYPE_TUBESTAYWIREPLATE)
	{
		long N=0;
		double d=0,P=0,R=0,L=0,W=0;
		GetDesignItemValue('N',&N);
		GetDesignItemValue('d',&d);
		GetDesignItemValue('R',&R);
		GetDesignItemValue('P',&P);
		GetDesignItemValue('L',&L);
		GetDesignItemValue('W',&W);
		HOLE hole;
		hole.wBoltD=0;
		hole.fHoleD=(float)d;
		f3dPoint pt(W,L-R,0);
		for(int i=0;i<N;i++)
		{
			hole.xLocalPosition.Set(pt.x,pt.y-P*i,0);
			pListHoles->Append(hole);
		}
	}
	else if(wiType==TYPE_STATIONCONNPLATE)
	{	//有端板内圆环
		HOLE hole;
		double inner_r=0,H=0;
		GetDesignItemValue('r',&inner_r);
		GetDesignItemValue('H',&H);
		hole.xLocalPosition.Set(0,H,0);
		hole.wBoltD=0;
		hole.fHoleD=(float)inner_r*2;
		pListHoles->Append(hole);
		incremental++;
	}
	else if(wiType==TYPE_STATIONSHOEPLATE)
	{	//添加柱顶板的穿梢孔
		HOLE hole;
		long hEndPlate=0;
		GetDesignItemValue(KEY2C("LE"),&hEndPlate);
		CLDSParamPlate* pEndPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(hEndPlate,CLS_PARAMPLATE);
		if(pEndPlate&&pEndPlate->m_iParamType==TYPE_STATIONCONNPLATE)	
		{
			double inner_r=0,H=0;
			pEndPlate->GetDesignItemValue('r',&inner_r);
			pEndPlate->GetDesignItemValue('H',&H);
			hole.xLocalPosition.Set(0,H,0);
			coord_trans(hole.xLocalPosition,pEndPlate->ucs,TRUE);
			coord_trans(hole.xLocalPosition,ucs,FALSE,TRUE);
			hole.wBoltD=0;
			hole.fHoleD=(float)inner_r*2;
			pListHoles->Append(hole);
		}
	}
	return incremental;
}
UINT CLDSGeneralPlate::GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole/*=false*/)	//返回值为孔总数
{
	if(pListHoles==NULL)
		return 0;
	UINT count=0;
	for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		count++;
		HOLE* pHole=pListHoles->Append(HOLE(pLsRef),pLsRef->pBolt->handle);
		SpreadLsPos(pLsRef->pBolt,pHole->xLocalPosition);
		int residePlane=0;
		GetBoltIntersPos(pHole->pBolt,pHole->xMcsPosition,&residePlane);
		pHole->ciResidePlane=(BYTE)residePlane;
		pHole->wBoltD=pLsRef->pBolt->d;
		pHole->fHoleMargin=(float)pLsRef->pBolt->hole_d_increment;
		pHole->wWaistLen=pLsRef->waistLen;
		pHole->pBolt=pLsRef->pBolt;
		if(residePlane==0)
			pHole->vHoleAxisVec=ucs.axis_z;
		else
			pHole->vHoleAxisVec=GetHuoquFaceNorm(residePlane-1);
	}
	UINT count2=bOnlyBoltHole?0:AddMiscellaneousHoles(pListHoles);
	return count+count2;
}
int CLDSGeneralPlate::GetLsCount(short d)
{
	CLDSPart* pMotherPart=MotherPart();
	if(pMotherPart!=this)
		return pMotherPart->GetLsCount(d);
	//统计典型螺栓孔
	int nCount1=(GetLsRefList())?GetLsRefList()->GetLsCount(d):0;
	//统计杂项圆孔
	CXhSimpleList<CLDSPart::HOLE> hole_list;
	AddMiscellaneousHoles(&hole_list);
	int nCount2=(d==0)?hole_list.GetCount():0;
	for(CLDSPart::HOLE* pHole=hole_list.EnumObjectFirst();pHole;pHole=hole_list.EnumObjectNext())
	{
		if(d>0&&pHole->wBoltD==d)
			nCount2++;
	}
	return nCount1+nCount2;
}
CLsRef* CLDSGeneralPlate::AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	if(bNearLsCheck)	//检查附近螺栓
	{
		CLsRef *pLsRef=pLsRefList->FindLsByPos(ls_ref->ucs.origin,ls_ref->get_norm(),ls_ref->get_d());
		if(pLsRef&&(*pLsRef)->get_d()==ls_ref->get_d())
			return pLsRef;
	}
	SetModified();
	return pLsRefList->append(ls_ref);
}

//沿板相对坐标系Y轴
BOOL CLDSPlate::CalStdProfile()
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->CalStdProfile();
	if(face_N>2 || GetLsCount()==0)
		return FALSE;
	SCOPE_STRU scope;
	f3dPoint ls_pos,vertice,pt;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(designInfo.m_hBasePart,CLS_LINEANGLE);
	if(wiType==WASHER_PLATE && pAngle &&GetLsCount()>1)
	{	//垫板根据GASKETEDGESPACE计算scope
		int min_d=0;
		for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			pt=ls_pos=(*pLsRef)->ucs.origin;
			SpreadLsPos(pLsRef,ls_pos);
			scope.VerifyVertex(ls_pos);
			min_d=(min_d==0)?pLsRef->pBolt->d:min(min_d,pLsRef->pBolt->d);
		}
		if(designInfo.iProfileStyle0123==0)
		{
			LSSPACEEX space;
			IBoltLibrary::GetLsSpaceEx(min_d,&space);
			gasketEdgeSpace.endSpaceS=gasketEdgeSpace.endSpaceE=gasketEdgeSpace.wingSpace=gasketEdgeSpace.berSpace=(short)space.WasherEdgeSpace;
		}
		BYTE start0_end1=DISTANCE(pt,pAngle->Start())<DISTANCE(pt,pAngle->End())?0:1;
		char ciWorkWing=fabs(pAngle->vxWingNorm*ucs.axis_z)>fabs(pAngle->vyWingNorm*ucs.axis_z)?'X':'Y';
		GEPOINT vWingVec=ciWorkWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
		GEPOINT vLenVec=(start0_end1==0)?(pAngle->End()-pAngle->Start()):(pAngle->Start()-pAngle->End());
		if(cDatumAxis=='Y')
		{	//基准杆件纵向为垫板局部坐标系的Y轴
			int signLenY =vLenVec *ucs.axis_y<0?-1:1;
			int signWingX=vWingVec*ucs.axis_x<0?-1:1;
			if(signLenY>0)
			{
				scope.fMinY-=gasketEdgeSpace.endSpaceS;
				scope.fMaxY+=gasketEdgeSpace.endSpaceE;
			}
			else
			{
				scope.fMinY-=gasketEdgeSpace.endSpaceE;
				scope.fMaxY+=gasketEdgeSpace.endSpaceS;
			}
			if(signWingX>0)
			{
				scope.fMinX-=gasketEdgeSpace.berSpace;
				scope.fMaxX+=gasketEdgeSpace.wingSpace;
			}
			else
			{
				scope.fMinX-=gasketEdgeSpace.wingSpace;
				scope.fMaxX+=gasketEdgeSpace.berSpace;
			}
		}
		else
		{	//基准杆件纵向为垫板局部坐标系的X轴
			int signLenX =vLenVec *ucs.axis_x<0?-1:1;
			int signWingY=vWingVec*ucs.axis_y<0?-1:1;
			if(signLenX>0)
			{
				scope.fMinX-=gasketEdgeSpace.endSpaceS;
				scope.fMaxX+=gasketEdgeSpace.endSpaceE;
			}
			else
			{
				scope.fMinX-=gasketEdgeSpace.endSpaceE;
				scope.fMaxX+=gasketEdgeSpace.endSpaceS;
			}
			if(signWingY>0)
			{
				scope.fMinY-=gasketEdgeSpace.berSpace;
				scope.fMaxY+=gasketEdgeSpace.wingSpace;
			}
			else
			{
				scope.fMinY-=gasketEdgeSpace.wingSpace;
				scope.fMaxY+=gasketEdgeSpace.berSpace;
			}	
		}
	}
	else
	{	//
		LSSPACEEX space;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			ls_pos=(*pLsRef)->ucs.origin;
			IBoltLibrary::GetLsSpaceEx((*pLsRef)->get_d(),&space);
			int end_space=(wiType==WASHER_PLATE&&space.WasherEdgeSpace>0)?space.WasherEdgeSpace:space.EndSpace;
			SpreadLsPos(pLsRef,ls_pos);
			vertice.Set(ls_pos.x-end_space,ls_pos.y+end_space);
			scope.VerifyVertex(vertice);	//左上角
			vertice.Set(ls_pos.x+end_space,ls_pos.y+end_space);
			scope.VerifyVertex(vertice);	//右上角
			vertice.Set(ls_pos.x+end_space,ls_pos.y-end_space);
			scope.VerifyVertex(vertice);	//右下角
			vertice.Set(ls_pos.x-end_space,ls_pos.y-end_space);
			scope.VerifyVertex(vertice);	//左下角
		}
	}
	PROFILE_VERTEX *pVertex;
	if(face_N==2)
	{
		GEPOINT center;
		int hits=0;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.feature!=1)
				continue;
			center+=pVertex->vertex;
			hits++;
		}
		center/=hits;
		double dist=DistOf2dPtLine(center,HuoQuLine[0].startPt,HuoQuLine[0].endPt);
		if(dist<0)
		{	//保证基面在火曲线左侧
			center=HuoQuLine[0].startPt;
			HuoQuLine[0].startPt=HuoQuLine[0].endPt;
			HuoQuLine[0].endPt=center;
		}

	}
	vertex_list.Empty();

	pVertex=vertex_list.append();
	pVertex->vertex.Set(scope.fMinX,scope.fMaxY,0);
	if(face_N==2&&DistOf2dPtLine(pVertex->vertex,HuoQuLine[0].startPt,HuoQuLine[0].endPt)<0)
		pVertex->vertex.feature=2;
	else
		pVertex->vertex.feature=1;
	pVertex=vertex_list.append();
	pVertex->vertex.Set(scope.fMinX,scope.fMinY,0);
	if(face_N==2&&DistOf2dPtLine(pVertex->vertex,HuoQuLine[0].startPt,HuoQuLine[0].endPt)<0)
		pVertex->vertex.feature=2;
	else
		pVertex->vertex.feature=1;
	pVertex=vertex_list.append();
	pVertex->vertex.Set(scope.fMaxX,scope.fMinY,0);
	if(face_N==2&&DistOf2dPtLine(pVertex->vertex,HuoQuLine[0].startPt,HuoQuLine[0].endPt)<0)
		pVertex->vertex.feature=2;
	else
		pVertex->vertex.feature=1;
	pVertex=vertex_list.append();
	pVertex->vertex.Set(scope.fMaxX,scope.fMaxY,0);
	if(face_N==2&&DistOf2dPtLine(pVertex->vertex,HuoQuLine[0].startPt,HuoQuLine[0].endPt)<0)
		pVertex->vertex.feature=2;
	else
		pVertex->vertex.feature=1;
	PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pPrevVertex->vertex.feature!=pVertex->vertex.feature)
		{
			double inters_x=0,inters_y=0;
			Int2dll(f2dLine(pPrevVertex->vertex,pVertex->vertex),f2dLine(HuoQuLine[0].startPt,HuoQuLine[0].endPt),
				inters_x,inters_y);
			PROFILE_VERTEX profile_vertex(inters_x,inters_y,0);
			PROFILE_VERTEX* pBendingVertex=vertex_list.insert(profile_vertex);
			pBendingVertex->vertex.feature=12;
		}
		pPrevVertex=pVertex;
	}
	SetModified();
	return TRUE;
}

f2dRect CLDSPlate::GetWrapRect()
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->GetWrapRect();
	SCOPE_STRU scope;
	f2dRect rect;
	UCS_STRU tmucs;
	int i=0,n=vertex_list.GetNodeNum();
	PROFILE_VERTEX *vertex_arr=new PROFILE_VERTEX[n];
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext(),i++)
		vertex_arr[i]=*pVertex;
	tmucs.axis_z.Set(0,0,1);
	double minarea=10000000000000000;	//预置任一大数
	double min_wide=0,min_high=0;
	for(i=0;i<n;i++)
	{
		tmucs.axis_x=vertex_arr[(i+1)%n].vertex-vertex_arr[i].vertex;
		if(tmucs.axis_x.IsZero())
			continue;	//有重复顶点存在
		tmucs.origin=vertex_arr[i].vertex;
		tmucs.axis_x.z = tmucs.origin.z = 0;
		tmucs.axis_y=tmucs.axis_z^tmucs.axis_x;
		normalize(tmucs.axis_x);
		normalize(tmucs.axis_y);
		scope.ClearScope();
		for(int j=0;j<n;j++)
		{
			f3dPoint tm_vec=vertex_arr[(j+1)%n].vertex-vertex_arr[j].vertex;
			if(tm_vec.mod()<EPS2)
				continue;	//跳过重复顶点 wht 11-01-23
			f3dPoint vertice=vertex_arr[j].vertex;
			coord_trans(vertice,tmucs,FALSE);
			scope.VerifyVertex(vertice);
			if(vertex_arr[j].type==0&&vertex_arr[j].m_bRollEdge)
			{	//考虑卷边带来的面积变化 wht 11-06-01
				normalize(tm_vec);
				f3dPoint tm_norm(tm_vec.y,-tm_vec.x);	//顺时针旋转90°
				vertice+=tm_norm*abs(vertex_arr[j].manu_space);
				if(vertex_arr[j].roll_edge_offset_dist>0)	//考虑卷边外移量(此处仅考虑按垂直方向偏移)
					vertice+=tm_norm*vertex_arr[j].roll_edge_offset_dist;
				scope.VerifyVertex(vertice);
				//
				vertice=vertex_arr[(j+1)%n].vertex;
				coord_trans(vertice,tmucs,FALSE);
				vertice+=tm_norm*abs(vertex_arr[j].manu_space);
				if(vertex_arr[j].roll_edge_offset_dist>0)	//考虑卷边外移量(此处仅考虑按垂直方向偏移)
					vertice+=tm_norm*vertex_arr[j].roll_edge_offset_dist;
				scope.VerifyVertex(vertice);
			}	
			else if(vertex_arr[j].type>0&&vertex_arr[j].type<=3)
			{	//当前边为圆弧边
				int sign=1;
				f3dArcLine arcline;
				if(vertex_arr[j].type==1)	//指定圆弧夹角
				{
					if(vertex_arr[j].sector_angle<0)
						sign=-1;
					arcline.CreateMethod2(vertex_arr[j].vertex,vertex_arr[(j+1)%n].vertex,f3dPoint(0,0,sign),sign*vertex_arr[j].sector_angle);
				}
				else //if(vertex_arr[j].type==2||vertex_arr[j].type==3)
				{	//椭圆也按圆来近似处理
					if(vertex_arr[j].radius<0)
						sign=-1;
					f3dPoint center_pick=vertex_arr[j].center;
					double radius=vertex_arr[j].radius;
					if(vertex_arr[j].type==3)	
					{	//椭圆弧圆心拾取点为绝对坐标需要转换到相对坐标系下 wht 11-07-02
						coord_trans(center_pick,ucs,FALSE);	//相对坐标 wht 11-01-21
						f3dPoint columnnorm=vertex_arr[j].column_norm;
						vector_trans(columnnorm,ucs,FALSE);
						normalize(columnnorm);
						double cosa=f3dPoint(0,0,1)*columnnorm;
						radius/=fabs(cosa);	//计算椭圆弧按圆弧近似处理时的半径
					}
					arcline.CreateMethod3(vertex_arr[j].vertex,vertex_arr[(j+1)%n].vertex,
						f3dPoint(0,0,sign),sign*radius,center_pick);
				}
				double radius=arcline.Radius();
				f3dPoint center=arcline.Center();
				f3dPoint arc_start=vertex_arr[j].vertex;
				f3dPoint arc_end=vertex_arr[(j+1)%n].vertex;
				coord_trans(center,tmucs,FALSE);
				coord_trans(arc_start,tmucs,FALSE);
				coord_trans(arc_end,tmucs,FALSE);
				double sector_angle=arcline.SectorAngle();
				double angle_start=Cal2dLineAng(center.x,center.y,arc_start.x,arc_start.y);
				if((sign>0&&angle_start+sector_angle>=2*Pi)||(sign<0&&angle_start-sector_angle<=0))
				{	//圆弧相对坐标系X轴正向上有交点
					scope.VerifyVertex(center+f3dPoint(radius,0,0));
				}
				else if((sign>0&&angle_start<0.5*Pi&&angle_start+sector_angle>0.5*Pi) ||
						(sign>0&&angle_start>0.5*Pi&&angle_start+sector_angle>2.5*Pi) ||
						(sign<0&&angle_start<0.5*Pi&&angle_start+sector_angle<2.5*Pi) ||
						(sign<0&&angle_start>0.5*Pi&&angle_start-sector_angle<0.5*Pi))
				{	//圆弧相对坐标系Y轴正向上有交点
					scope.VerifyVertex(center+f3dPoint(0,radius,0));
				}
				else if((sign>0&&angle_start<Pi&&angle_start+sector_angle>1*Pi) ||
						(sign>0&&angle_start>Pi&&angle_start+sector_angle>3*Pi) ||
						(sign<0&&angle_start<Pi&&angle_start+sector_angle<3*Pi) ||
						(sign<0&&angle_start>Pi&&angle_start-sector_angle<1*Pi))
				{	//圆弧相对坐标系X轴负向上有交点
					scope.VerifyVertex(center+f3dPoint(-radius,0,0));
				}
				else if((sign>0&&angle_start<1.5*Pi&&angle_start+sector_angle>1.5*Pi) ||
						(sign>0&&angle_start>1.5*Pi&&angle_start+sector_angle>3.5*Pi) ||
						(sign<0&&angle_start<1.5*Pi&&angle_start+sector_angle<3.5*Pi) ||
						(sign<0&&angle_start>1.5*Pi&&angle_start-sector_angle<3.5*Pi))
				{	//圆弧相对坐标系Y轴负向上有交点
					scope.VerifyVertex(center+f3dPoint(0,-radius,0));
				}
			}
		}
		if(minarea>scope.wide()*scope.high())
		{
			minarea=scope.wide()*scope.high();
			min_wide = scope.wide();
			min_high = scope.high();
		}
	}
	delete []vertex_arr;
	rect.topLeft.Set(0,0);
	rect.bottomRight.Set(min_wide,min_high);
	return rect;
}

//计算连接板正交包络平面矩形面积mm2
//输入角度均为角度制°
static BOOL IsInSector(double start_ang,double sector_ang,double verify_ang,BOOL bClockWise=TRUE)
{
	double ang=0;
	if(!bClockWise)	//逆时针
		ang=ftoi(verify_ang-start_ang)%360;
	else			//顺时针
		ang=ftoi(start_ang-verify_ang)%360;
	if(ang<0)
		ang+=360;
	if(sector_ang>ang)
		return TRUE;
	else
		return FALSE;
}

void CLDSGeneralPlate::CalEquidistantShape(double minDistance,ATOM_LIST<PROFILE_VERTEX> *pList)
{
	ATOM_LIST<PROFILE_VERTEX> pListTemp;
	for (PROFILE_VERTEX *vertex = vertex_list.GetFirst();vertex;vertex=vertex_list.GetNext())
	{
		PROFILE_VERTEX *vertexNext = vertex_list.GetNext();
		if (vertexNext == NULL)
		{
			vertexNext = vertex_list.GetFirst();
			vertex_list.GetTail();
		}
		else
			vertex_list.GetPrev();
		if (vertexNext->vertex != vertex->vertex)
			pListTemp.append(*vertex);
	}
	for (vertex = pListTemp.GetFirst();vertex;vertex=pListTemp.GetNext())
	{
		PROFILE_VERTEX *vertexPre = pListTemp.GetPrev();
		if (vertexPre == NULL)
		{
			vertexPre = pListTemp.GetTail();
			pListTemp.GetFirst();
		}
		else
			pListTemp.GetNext();
		PROFILE_VERTEX *vertexNext = pListTemp.GetNext();
		if (vertexNext == NULL)
		{
			vertexNext = pListTemp.GetFirst();
			pListTemp.GetTail();
		}
		else
			pListTemp.GetPrev();
		f3dPoint curPt = vertex->vertex;
		f3dPoint prePt = vertexPre->vertex;
		f3dPoint nextPt = vertexNext->vertex;
		f3dPoint preLineVec = curPt - prePt;
		normalize(preLineVec);
		f3dPoint nextLineVec = curPt - nextPt;
		normalize(nextLineVec);
		PROFILE_VERTEX exterVertex=*vertex;
		if(fabs(preLineVec*nextLineVec)<0.9999)
		{	//三点不共线,计算等距偏移点
			double angle = cal_angle_of_2vec(preLineVec,nextLineVec);
			double offset = minDistance/sin(angle/2);
			f3dPoint vec = preLineVec + nextLineVec;
			normalize(vec);
			exterVertex.vertex = curPt + vec*offset;
			if(vertex->type >= 2)
				exterVertex.radius += offset;
		}
		else
		{
			if(vertex->type>=2 || vertexPre->type==2)
			{//圆弧角度为180°，半径向外延伸
				f3dPoint vec=curPt-nextPt;
				if(vertexPre->type==2)
					vec=curPt-prePt;
				normalize(vec);
				exterVertex.vertex=curPt+vec*minDistance;
				exterVertex.radius+=minDistance;
			}
			else
			{
				GEPOINT vOutwardVec(-preLineVec.y,preLineVec.x,preLineVec.z);
				exterVertex.vertex=curPt+vOutwardVec*minDistance;
			}
		}
		pList->append(exterVertex);
	}
}
bool CLDSGeneralPlate::IsGeomMir(CLDSGeneralPlate *pMirPlate,MIRMSG mirmsg,BOOL bCheckProfile/*=TRUE*/,const double *pSrcGeomCenter/*=NULL*/)
{	
	if( this==pMirPlate||(GetClassTypeId()!=pMirPlate->GetClassTypeId())||
		pMirPlate->GetThick()!=GetThick()||!ModuleTogetherWith(pMirPlate))
		return false;
	if(bCheckProfile&&IsEqualProfile(pMirPlate,FALSE)>0)	//0.外形相同 
		return false;
	typedef CLDSGeneralPlate* PLATE_PTR;
	f3dPoint axis_x[2],geom_center;
	PLATE_PTR platePtrArr[2]={this,pMirPlate};
	for(int i=0;i<2;i++)
	{	//1.计算钢板几何中心方向
		if(i==0&&pSrcGeomCenter)
			geom_center=f3dPoint(pSrcGeomCenter);
		else 
			geom_center=CLDSPlate::GetGeomCenter(platePtrArr[i]);
		coord_trans(geom_center,platePtrArr[i]->ucs,TRUE);
		project_point(geom_center,platePtrArr[i]->ucs.origin,platePtrArr[i]->ucs.axis_z);
		axis_x[i]=geom_center-platePtrArr[i]->ucs.origin;
		normalize(axis_x[i]);
	}
	//对称板的延伸方向
	f3dPoint mir_aixs_x=GetMirVector(axis_x[0],mirmsg);
	if(mir_aixs_x*axis_x[1]<EPS_COS2) //重心位置不同
		return false;
	else
		return true;
}
bool CLDSGeneralPlate::IsEqualMirWith(CLDSGeneralPlate* pMirPlate,MIRMSG mirmsg)
{
	//TODO：需要进行对称性验证 IsMirWith((pParamPlate,mirmsg) wjh-2018.3.12
	return true;
}
bool CLDSGeneralPlate::IsPtInPlate(double* pos_coord)
{
	GEPOINT pick(pos_coord[0],pos_coord[1]);
	if(GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)this)->m_iParamType==TYPE_FL)
	{
		double Ro=0,Ri=0;
		((CLDSParamPlate*)this)->GetDesignItemValue('W',&Ro);
		((CLDSParamPlate*)this)->GetDesignItemValue('R',&Ri);
		Ro*=0.5;
		Ri*=0.5;
		double r=pick.mod();
		if( r<Ri-1||r>Ro+1)
			return false;
		else
			return true;
	}
	else
	{
		if(vertex_list.GetNodeNum()<3)
			return false;
		bool pointInRgn=true,pointInRgn2=true;
		GEPOINT xPoint;
		PROFILE_VERTEX *pVertex, *pPrevVertex=vertex_list.GetTail();
		ARRAY_LIST<f3dPoint> vertexes;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pPrevVertex->type==0)
			{
				xPoint=GetRealVertex(pPrevVertex->vertex);
				vertexes.append(xPoint);
			}
			else //if(pPrevVertex->type>0)
			{
				f3dArcLine arcline;
				pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&ucs);
				UINT slices=arcline.SamplePoints(NULL);//CalArcResolution(arcline.Radius(),arcline.SectorAngle(),1,5,36);
				DYN_ARRAY<GEPOINT> svarr(slices);
				arcline.SamplePoints(svarr);
				for(UINT i=0;i<slices;i++)
					vertexes.append(svarr[i]);
			}
			pPrevVertex=pVertex;
		}
		POLYGON rgn;
		rgn.CreatePolygonRgn(vertexes.m_pData,vertexes.GetSize());
		int justify=rgn.PtInRgn2(pick);
		pointInRgn=(justify==1||justify==2);	//在区域内或边界上
		/*经初步测试rgn.PtInRgn2()是可靠的 wjh-2017.11.22
#ifdef _DEBUG
		f2dRay ray;
		f3dLine line;
		f3dPoint inters;//根据奇偶算法求出的多边表域边界线交点集合

		ray.basePt = pick;
		ray.vec.Set(1,0.1);
		f3dArcLine arcline;
		for(int iter_hits=0;iter_hits<3;iter_hits++)
		{
			double fHits = 0.0;
			PROFILE_VERTEX *pVertex, *pPrevVertex=vertex_list.GetTail();
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				bool linear=true;
				if(pPrevVertex->type>0)
				{
					linear=false;
					pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&ucs);
				}
				if(linear)	//1：交点为直线段的内点;2：交点为直线段端点
				{
					int retcode=Int2dlr(f2dLine(pPrevVertex->vertex,pVertex->vertex),ray,inters.x,inters.y);
					if(retcode==1)
						fHits+=1.0;	//直线与射线相交于内点
					else if(retcode==2)
					{
						if(inters==pick)
							goto DETECT_NOT;	//拾取点在板边缘上认为无焊接关系
						else
						{
							if(iter_hits<2)
							{	//如遇交点为端点情况，为了避免计算误差，最好旋转射线方向一定角度重新计算
								ray.vec.y+=0.5;
								//RotateVectorAroundVector(ray.vec,0.1,ucs.axis_z);
								break;
							}
							else
								fHits+=0.5;	//直线与射线相交于端点
						}
					}
				}
				else
				{
					f2dArc arc;
					arc.centre=arcline.Center();
					GEPOINT work_norm=arcline.WorkNorm(),column_norm=arcline.ColumnNorm();
					if(work_norm.z>0)
						arc.startPt=arcline.Start();
					else
						arc.startPt=arcline.End();
					arc.angle=arcline.SectorAngle();
					f2dPoint inters2;
					int inters_n=0;
					if(column_norm.IsZero()||work_norm==column_norm)	//圆弧与射线求交
						inters_n=Int2dla(f2dLine(ray.basePt,f2dPoint(ray.basePt.x+ray.vec.x*10000,ray.basePt.y+ray.vec.y*10000)),arc,inters.x,inters.y,inters2.x,inters2.y);
					else
					{	//椭圆弧
						double cosa=fabs(work_norm*column_norm);
						double shortRadius=arcline.Radius();
						double longRadius=cosa>EPS ? shortRadius/cosa : shortRadius;
						GEPOINT vLongAxisVec=work_norm^column_norm;
						normalize(vLongAxisVec);
						vLongAxisVec.Set(-vLongAxisVec.y*longRadius,vLongAxisVec.x*longRadius);
						GEPOINT lineStart(ray.basePt.x,ray.basePt.y),lineEnd(ray.basePt.x+ray.vec.x*10000,ray.basePt.y+ray.vec.y*10000);
						inters_n=Int2dle(&inters.x,&inters2.x,lineStart,lineEnd,&arc.centre.x,&arc.startPt.x,shortRadius,arc.angle,vLongAxisVec);
					}
					for(int j=0;j<inters_n;j++)
					{
						f3dPoint inters_pt=inters;
						if(j==1)
							inters_pt.Set(inters2.x,inters2.y);
						f2dPoint vec(inters_pt.x-ray.basePt.x,inters_pt.y-ray.basePt.y);
						if(vec.x*ray.vec.x+vec.y*ray.vec.y<=0)	//交点在射线外
							continue;
						else	//交点在射线内
						{
							if(inters_pt==arcline.Start() || inters_pt==arcline.End())
								fHits+=0.5;
							else
								fHits+=1;
						}

					}
				}
				pPrevVertex=pVertex;
			}
			if(pVertex==NULL)
			{
				if(ftol(fHits)%2>0)
					goto DETECT_OK;	//焊缝拾取点在钢板外形区域内
				else
					goto DETECT_NOT;
			}
		}
		goto DETECT_NOT;
DETECT_OK:
		pointInRgn2=true;
DETECT_NOT:
		pointInRgn2=true;
		if(pointInRgn!=pointInRgn2)
		{
			Log2File()->Log("0x%x钢板，x=%.0f,y=%.0f,%d不同于%d",handle,pick.x,pick.y,pointInRgn,pointInRgn2);
		}
#endif
		*/
		return pointInRgn;
	}
}
bool CLDSGeneralPlate::OptimizeStructure()
{
	return false;
}
//计算指定条件下，返回角钢可承载的最大块剪力(kN)
//dfValidSectLength:螺栓端距+螺栓间距总和;c螺栓心线到肢翼边距离,f:设计强度;fv:抗剪强度
double _LocalCalMaxBlockShearForce(double dfValidSectLength,double c,double t,double f,double fv)
{
	double Av=t*dfValidSectLength;
	double At=t*c;
	return Av*fv+At*f;
}
struct SHEARSECT{
	double dfLenSectLength;	//轴向撕裂面长度	
	double dfSummHoleDeduct;//螺栓孔扣除
	double dfDistAxis2Edge;	//螺栓心线到角钢肢翼边间距
	SHEARSECT() {
		dfLenSectLength=25;	//轴向撕裂面长度	
		dfSummHoleDeduct=0;	//螺栓孔扣除
		dfDistAxis2Edge=0;	//螺栓心线到角钢肢翼边间距
	}
};
bool CLDSGeneralPlate::StableCheck(ILog2File* pRptLog,DWORD dwCheckFlag/*=0xffff*/,
	double (*CallBackDialogUIFunc)(CLDSLineAngle* pAngle,double dfAxisNByBolts,DWORD *pdwCheckFlag)/*=NULL*/) 
{
	if (this->wiType!=CONNECT_PLATE)
		return true;	//目前仅处理普通 节点板的强度稳定验算
	//《输电线路铁塔制图和构造规定》 DLT 5442-2010
	//8.5.2 当节占板的自由边长度Lf与节点板厚度t之比Lf/t>A时（Q235,A=60； Q345=50；Q420,A=45），
	//应沿自由边加强采用卷边处理或加焊加劲板
	//8.5.3 节点板厚度应等于或大于斜材或横材肢厚，当斜材长细比<=120时，节点板应加厚1~2mm。
	//《架空输电线路杆塔结构设计技术规定》 DLT 5154-2012
	//7.3.1 连接节点板的强度除应满足螺栓孔壁承压要求外，还应按下式计算（应力扩散角θ=30°）
	//σ = N/(Be*t) ≤f, Be板件的有效宽度,mm；当用螺栓连接时，应减去孔径；N为作用于板件的拉力或压力,N
	//t ——板件厚度，mm；f为钢板设计强度
	STEELMAT *pSteelMat=QuerySteelMatByBriefMark(this->cMaterial);
	if(pSteelMat==NULL)
		pSteelMat=CSteelMatLibrary::Q235Material();
	CLDSPlate* pPlate=(CLDSPlate*)this;
	CDesignLjPartPara* pRodPara;
	SmartRodPtr pRod=BelongModel()->FromRodHandle(pPlate->m_hBaseRod);
	CLDSLineAngle* pBaseAngle=NULL;
	CLDSGroupLineAngle* pBaseGroupAngle=NULL;
	pRptLog->Log("****************************************");
	pRptLog->Log("0x%X{%s}钢板开始力学审核",handle,Label);
	if (!pRod.blValid)
	{
		pRptLog->Log("0x%X钢板上的基准杆件0x%X丢失,无法进行进一步拉压强度校审验算",handle,pPlate->m_hBaseRod);
		return false;
	}
	if (pRod->IsAngle())
		pBaseAngle=pRod.pAngle;
	if (pBaseAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pBaseGroupAngle=(CLDSGroupLineAngle*)pBaseAngle;
	else if (pBaseAngle->hGroupAngle>0x20)
		pBaseGroupAngle=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(pBaseAngle->hGroupAngle,CLS_GROUPLINEANGLE);
	bool blCheckPassed=true;
	for (pRodPara=pPlate->designInfo.partList.GetFirst();pRodPara;pRodPara=pPlate->designInfo.partList.GetNext())
	{
		if (pRodPara->hPart==this->m_hBaseRod)
			continue;	//基准杆件
		pRod=BelongModel()->FromRodHandle(pRodPara->hPart);
		if (!pRod.blValid)
		{
			blCheckPassed=false;
			pRptLog->Log("0x%X钢板上的0x%X杆件丢失,无法进行进一步拉压强度校审验算",handle,pRodPara->hPart);
			continue;
		}
		else if (!pRod->IsAngle())
			continue;
		else if (pBaseGroupAngle!=NULL&&(pRod.pAngle==pBaseGroupAngle||pRod.pAngle->hGroupAngle==pBaseGroupAngle->handle))
			continue;	//基准角钢
		CLDSLineAngle* pAngle=pRod.pAngle;
		//PRESET_ARRAY32<CLDSBolt*> xarr32Bolts;
		double dfSummAxisN=0;	//杆件最大承载拉压轴力,kN
		bool blDirectConnect=false;	//是否与基准角钢存在直接螺栓连接
		GECS rcs;	//非必须符合右手螺旋规则
		rcs.axis_x=pRod->xPosEnd-pRod->xPosStart;
		normalize(rcs.axis_x);
		if (pRodPara->start0_end1==0)
			rcs.origin=pRod->xPosStart-pRod->dfStartOdd*rcs.axis_x;
		else if (pRodPara->start0_end1==1)
		{
			rcs.axis_x*=-1;
			rcs.origin=pRod->xPosEnd-pRod->dfEndOdd*rcs.axis_x;
		}
		else
			continue;
		if (pRodPara->angle.cur_wing_x0_y1==0)
		{
			rcs.axis_y=pAngle->GetWingVecX();
			rcs.axis_z=pAngle->vxWingNorm;
		}
		else //if(pRodPara->angle.cur_wing_x0_y1==1)
		{
			rcs.axis_y=pAngle->GetWingVecY();
			rcs.axis_z=pAngle->vyWingNorm;
		}
		GEPOINT xRayTipPoint;	//射线末端螺栓位置
		GEPOINT xRayTipPoint2;	//用于末端并列螺栓分布于不同心距情况
		double dfMinXOfBolt=100;
		SHEARSECT xShearSect;	//仅支持最内侧块剪断裂面
		bool blFirstValidBolt=true;
		for (CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if (pAngle->FindLsByHandle(pLsRef->pBolt->handle)==NULL)
				continue;	//钢板上该螺栓不是当前射线角钢上的连接螺栓
			if (pBaseAngle->FindLsByHandle(pLsRef->pBolt->handle)!=NULL)
			{
				blDirectConnect=true;
				continue;
			}
			CONNBOLTS raybolt(pLsRef->pBolt->d,1,this->_thick,pLsRef->pBolt->Grade());
			dfSummAxisN+=min(raybolt.Avf,raybolt.Ahf);
			GEPOINT xLocation;
			pAngle->GetBoltIntersPos(pLsRef->pBolt,xLocation);
			xLocation=rcs.TransPToCS(xLocation);
			if (blFirstValidBolt||xLocation.x<dfMinXOfBolt)
				dfMinXOfBolt=xLocation.x;
			blFirstValidBolt=false;
			if (xRayTipPoint.x<xLocation.x-1)
				xRayTipPoint=xRayTipPoint2=xLocation;
			else if (xRayTipPoint.x<xLocation.x+1)
			{
				if(xLocation.y>xRayTipPoint.y)
					xRayTipPoint2=xLocation;
				else if(xLocation.y<xRayTipPoint.y)
					xRayTipPoint=xLocation;	//靠近角钢楞线侧
			}
			if (pAngle->size_wide-xLocation.y-5>xShearSect.dfDistAxis2Edge)
			{
				xShearSect.dfLenSectLength=xLocation.x;
				xShearSect.dfDistAxis2Edge=pAngle->size_wide-xLocation.y;
				xShearSect.dfSummHoleDeduct=pLsRef->pBolt->d+pLsRef->pBolt->hole_d_increment;
			}
			else if(pAngle->size_wide-xLocation.y+5>xShearSect.dfDistAxis2Edge&&
				xShearSect.dfLenSectLength<xLocation.x)
			{
				xShearSect.dfLenSectLength=xLocation.x;
				xShearSect.dfDistAxis2Edge=min(xShearSect.dfDistAxis2Edge,pAngle->size_wide-xLocation.y);
				xShearSect.dfSummHoleDeduct+=pLsRef->pBolt->d+pLsRef->pBolt->hole_d_increment;
			}
		}
		GEPOINT vSectLineVec,xSectLineStart,xSectLineEnd;
		if (pRodPara->iFaceNo<=1)
		{
			double dfTana30=0.57735;
			double dfLength =xRayTipPoint.x-dfMinXOfBolt;
			double dfLength2=xRayTipPoint2.x-dfMinXOfBolt;
			xRayTipPoint=ucs.TransPToCS(rcs.TransPFromCS(xRayTipPoint));
			xRayTipPoint2=ucs.TransPToCS(rcs.TransPFromCS(xRayTipPoint2));
			vSectLineVec=ucs.TransVToCS(rcs.axis_y);
			GEPOINT vStdLineVec=ucs.TransVToCS(rcs.axis_x);
			//根据应力扩散角30°限定有效切割线起止点
			xSectLineStart=xRayTipPoint-dfLength*vStdLineVec-vSectLineVec*(dfLength *dfTana30);
			xSectLineEnd=xRayTipPoint2-dfLength2*vStdLineVec+vSectLineVec*(dfLength2*dfTana30);
		}
		else if(pRodPara->iFaceNo<=3)
		{	//制弯面
			double dfTana30=0.57735;
			double dfLength =xRayTipPoint.x-dfMinXOfBolt;
			double dfLength2=xRayTipPoint2.x-dfMinXOfBolt;
			xRayTipPoint=pPlate->SpreadToACS(rcs.TransPFromCS(xRayTipPoint),pRodPara->iFaceNo,FROM_MCS|IS_VERTEX);
			xRayTipPoint2=pPlate->SpreadToACS(rcs.TransPFromCS(xRayTipPoint2),pRodPara->iFaceNo,FROM_MCS|IS_VERTEX);
			vSectLineVec=pPlate->SpreadToACS(rcs.axis_y,pRodPara->iFaceNo,FROM_MCS|IS_VECTOR);
			GEPOINT vStdLineVec=pPlate->SpreadToACS(rcs.axis_x,pRodPara->iFaceNo,FROM_MCS|IS_VECTOR);
			//根据应力扩散角30°限定有效切割线起止点
			xSectLineStart=xRayTipPoint -dfLength *vStdLineVec-vSectLineVec*(dfLength *dfTana30);
			xSectLineEnd  =xRayTipPoint2-dfLength2*vStdLineVec+vSectLineVec*(dfLength2*dfTana30);
		}
		xSectLineStart.z=xSectLineEnd.z=0;
		f2dLine sectline(xSectLineStart,xSectLineEnd);
		PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
		GEPOINT inters,midsect=0.5*(xSectLineStart+xSectLineEnd);
		BYTE cbCorrTipState=0;
		for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			int status=Int2dll(sectline,f2dLine(pPrevVertex->vertex,pVertex->vertex),inters.x,inters.y);
			pPrevVertex=pVertex;
			if(status<=0)
				continue;	//无有效交点
			double justify=(inters.x-midsect.x)*vSectLineVec.x+(inters.y-midsect.y)*vSectLineVec.y;
			if(justify>EPS)
			{
				xSectLineEnd=inters;
				cbCorrTipState|=0x01;
			}
			else if(justify<-EPS)
			{
				xSectLineStart=inters;
				cbCorrTipState|=0x02;
			}
			if(cbCorrTipState==0x03)
				break;
		}
		//连接板角钢连接强度验算
		//σ = N/(Be*t) ≤f, Be板件的有效宽度,mm；当用螺栓连接时，应减去孔径；N为作用于板件的拉力或压力,N
		double Be=DISTANCE(xSectLineEnd,xSectLineStart);
		DWORD dwCheckFlag=0xff;
		if (CallBackDialogUIFunc)
			dfSummAxisN=CallBackDialogUIFunc(pAngle,dfSummAxisN*0.001,&dwCheckFlag)*1000;
		double dfMaxForce=pSteelMat->f(_thick);
		if ((dwCheckFlag&0x01)>0)
		{
			double sigema=dfSummAxisN/(Be*this->_thick);
			if (sigema>dfMaxForce)
			{
				blCheckPassed=false;
				pRptLog->Log("0x%x射线杆件端截面长度%.0fmm, 最大轴力%.2f(kN),当前强度%.1f(MPa）>%.0f(MPa)超限！",pRodPara->hPart,Be,dfSummAxisN*0.001,sigema,dfMaxForce);
			}
		}
		if((dwCheckFlag&0x02)>0)
		{
			double dfMaxShearForce=pSteelMat->fv(_thick);
			double dfMaxBlockShearN=_LocalCalMaxBlockShearForce(xShearSect.dfLenSectLength-xShearSect.dfSummHoleDeduct,
				xShearSect.dfDistAxis2Edge,this->_thick,dfMaxForce,dfMaxShearForce);
			if (dfSummAxisN>dfMaxBlockShearN) 
			{
				blCheckPassed=false;
				pRptLog->Log("0x%x射线角钢端面块剪长度%.0fmm,宽度%.0fmm。 最大轴力>最大承载力(%.2f>%.0f kN)超限！",pRodPara->hPart,
					xShearSect.dfLenSectLength-xShearSect.dfSummHoleDeduct,xShearSect.dfDistAxis2Edge,dfSummAxisN*0.001,dfMaxBlockShearN*0.001);
			}
		}
	}
	return blCheckPassed;
}
bool CLDSPlate::OptimizeStructure()
{
	return false;
}
//计算卷边火曲线及卷边线位置
void CLDSGeneralPlate::CalRollLineAndHuoquLine(PROFILE_VERTEX *pStartVertex,PROFILE_VERTEX *pEndVertex,PROFILE_VERTEX *pPreVertex,PROFILE_VERTEX *pNextVertex,
									f3dLine &huoqu_line,f3dLine &roll_line,BOOL *pbRetainRollLineStart/*=NULL*/,BOOL *pbRetainRollLineEnd/*=NULL*/)
{
	if(pStartVertex==NULL||pEndVertex==NULL||pPreVertex==NULL||pNextVertex==NULL)
		return;
	if(pbRetainRollLineStart)
		*pbRetainRollLineStart=FALSE;
	if(pbRetainRollLineEnd)
		*pbRetainRollLineEnd=FALSE;
	f3dPoint prev_vec=pStartVertex->vertex-pPreVertex->vertex;
	f3dPoint next_vec=pEndVertex->vertex-pNextVertex->vertex;
	f3dPoint cur_vec=pEndVertex->vertex-pStartVertex->vertex;//当前边方向
	normalize(prev_vec);
	normalize(next_vec);
	normalize(cur_vec);
	GEPOINT offset_vec=cur_vec;	
	offset_vec.Set(offset_vec.y,-offset_vec.x);
	normalize(offset_vec);
	double cosa=fabs(cur_vec*prev_vec);//判断当前边与前一边是否共线
	double cosb=fabs(cur_vec*next_vec);//判断当前边与下一边是否共线
	f2dLine edge_line,extend_line;
	//根据不同的卷边位置处理方式计算火曲线和卷边线的偏移距离
	int nHuoquOffDist=0,nRollOffDist=0;
	if(m_ciRollOffsetType==0)
	{	//外侧方式
		nHuoquOffDist=pStartVertex->roll_edge_offset_dist+ftoi(Thick);
		nRollOffDist=pStartVertex->manu_space;
	}
	else
	{	//内侧方式
		nHuoquOffDist=pStartVertex->roll_edge_offset_dist;
		nRollOffDist=pStartVertex->manu_space+ftoi(Thick);
	}
	//计算偏移后火曲线的位置
	huoqu_line.startPt=pStartVertex->vertex+offset_vec*nHuoquOffDist;
	huoqu_line.endPt=pEndVertex->vertex+offset_vec*nHuoquOffDist;
	edge_line.startPt.Set(huoqu_line.startPt.x,huoqu_line.startPt.y);
	edge_line.endPt.Set(huoqu_line.endPt.x,huoqu_line.endPt.y);
	if(cosa<EPS_COS2)
	{	//当前边与前一边不共线	
		extend_line.startPt.Set(pStartVertex->vertex.x,pStartVertex->vertex.y);
		extend_line.endPt.Set(extend_line.startPt.x+prev_vec.x,extend_line.startPt.y+prev_vec.y);
		Int2dll(extend_line,edge_line,huoqu_line.startPt.x,huoqu_line.startPt.y);
	}
	else if(pbRetainRollLineStart)
		*pbRetainRollLineStart=TRUE;
	if(cosb<EPS_COS2)
	{	//当前边与下一边不共线
		extend_line.startPt.Set(pEndVertex->vertex.x,pEndVertex->vertex.y);
		extend_line.endPt.Set(extend_line.startPt.x+next_vec.x,extend_line.startPt.y+next_vec.y);
		Int2dll(extend_line,edge_line,huoqu_line.endPt.x,huoqu_line.endPt.y);
	}
	else if(pbRetainRollLineEnd)
		*pbRetainRollLineEnd=TRUE;
	f3dPoint new_edge_vec=huoqu_line.endPt-huoqu_line.startPt;
	normalize(new_edge_vec);	//卷边后新边方向
	if(new_edge_vec*cur_vec<0)	//得到的新边始末端颠倒，边始端取垂直方向为延伸方向 wht 09-08-14
	{
		huoqu_line.startPt=pStartVertex->vertex+offset_vec*nHuoquOffDist;
		prev_vec=offset_vec;
		huoqu_line.endPt=pEndVertex->vertex+offset_vec*nHuoquOffDist;
		next_vec=offset_vec;
		if(pbRetainRollLineStart)
			*pbRetainRollLineStart=TRUE;
		if(pbRetainRollLineEnd)
			*pbRetainRollLineEnd=TRUE;
	}
	//计算卷边线的位置
	roll_line.startPt=huoqu_line.startPt+offset_vec*abs(nRollOffDist);
	roll_line.endPt=huoqu_line.endPt+offset_vec*abs(nRollOffDist);
	if(m_cRollProcessType==1||m_cRollProcessType==2)
	{	//最少刀数 wht 11-06-23
		edge_line.startPt.Set(roll_line.startPt.x,roll_line.startPt.y);
		edge_line.endPt.Set(roll_line.endPt.x,roll_line.endPt.y);
		if(fabs(cur_vec*prev_vec)<EPS_COS2)
		{	//当前边与前一边不共线	
			extend_line.startPt.Set(pStartVertex->vertex.x,pStartVertex->vertex.y);
			extend_line.endPt.Set(extend_line.startPt.x+prev_vec.x,extend_line.startPt.y+prev_vec.y);
			Int2dll(extend_line,edge_line,roll_line.startPt.x,roll_line.startPt.y);
		}
		if(fabs(cur_vec*next_vec)<EPS_COS2)
		{	//当前边与下一个边不共线 
			extend_line.startPt.Set(pEndVertex->vertex.x,pEndVertex->vertex.y);
			extend_line.endPt.Set(extend_line.startPt.x+next_vec.x,extend_line.startPt.y+next_vec.y);
			Int2dll(extend_line,edge_line,roll_line.endPt.x,roll_line.endPt.y);
		}
		new_edge_vec=roll_line.endPt-roll_line.startPt;
		normalize(new_edge_vec);	//卷边后新边方向
		if(new_edge_vec*cur_vec<0)	//得到的新边始末端颠倒，边始端取垂直方向为延伸方向 wht 09-08-14
		{
			roll_line.startPt=huoqu_line.startPt+offset_vec*abs(nRollOffDist);
			roll_line.endPt=huoqu_line.endPt+offset_vec*abs(nRollOffDist);
		}
	}
	huoqu_line.startPt.z=huoqu_line.endPt.z=0;
	roll_line.startPt.z=roll_line.endPt.z=0;
}
bool CLDSGeneralPlate::GetBendlineVertexies(
	PROFILE_VERTEX** ppBendLine1Start/*=NULL*/,PROFILE_VERTEX** ppBendLine1End/*=NULL*/,
	PROFILE_VERTEX** ppBendLine2Start/*=NULL*/,PROFILE_VERTEX** ppBendLine2End/*=NULL*/)
{
	CLDSPlate* pPlate=GetClassTypeId()==CLS_PLATE?(CLDSPlate*)this:NULL;
	PROFILE_VERTEX* pBendPlane2Pick=NULL,*pBendPlane3Pick=NULL;
	PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL;
	PROFILE_VERTEX* pBendLine2Start=NULL,*pBendLine2End=NULL;
	if(GetFaceN()<2||pPlate==NULL)
		return false;
	bool isNullCursor=vertex_list.GetCursor()==NULL;
	BOOL pushed=vertex_list.push_stack();
	PROFILE_VERTEX* pVertex;
	if(pPlate&&pPlate->IsPeakPointPlate())
	{
		PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pBendPlane2Pick==NULL&&pVertex->vertex.feature==2)
				pBendPlane2Pick=pVertex;
			if(pBendPlane3Pick==NULL&&pVertex->vertex.feature==3)
				pBendPlane3Pick=pVertex;
			if(pVertex->vertex.feature==12||pVertex->vertex.feature==21)
				pBendLine1Start==NULL ? pBendLine1Start=pVertex : pBendLine1End=pVertex;
			if(pVertex->vertex.feature==13||pVertex->vertex.feature==31)
				pBendLine2Start==NULL ? pBendLine2Start=pVertex : pBendLine2End=pVertex;
			if(pBendLine1Start&&pBendLine2Start&&pBendPlane2Pick&&pBendPlane3Pick)
				break;
		}
		pBendLine1End=pBendLine2End=InternalGet3PlanePlatePeakVertex();
	}
	else
	{
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pBendPlane2Pick==NULL&&pVertex->vertex.feature==2)
				pBendPlane2Pick=pVertex;
			if(pBendPlane3Pick==NULL&&pVertex->vertex.feature==3)
				pBendPlane3Pick=pVertex;
			if(pVertex->vertex.feature==12||pVertex->vertex.feature==21)
				pBendLine1Start==NULL ? pBendLine1Start=pVertex : pBendLine1End=pVertex;
			if(pVertex->vertex.feature==13||pVertex->vertex.feature==31)
				pBendLine2Start==NULL ? pBendLine2Start=pVertex : pBendLine2End=pVertex;
			if( pBendLine1Start&&pBendLine1End&&((GetFaceN()==2&&pBendPlane2Pick!=NULL)||
				(GetFaceN()>=3&&pBendLine2Start&&pBendLine2End&&pBendPlane2Pick!=NULL&&pBendPlane3Pick!=NULL)))
				break;
		}
	}
	double dist2=0,dist3=0;
	if(pBendLine1Start&&pBendLine1End&&pBendPlane2Pick)
		dist2=DistOf2dPtLine(pBendPlane2Pick->vertex,pBendLine1Start->vertex,pBendLine1End->vertex);
	else
		Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X钢板火曲线信息提取有误!",handle);
	if(GetFaceN()>=3&&pBendLine2Start&&pBendLine2End&&pBendPlane3Pick)
		dist3=DistOf2dPtLine(pBendPlane3Pick->vertex,pBendLine2Start->vertex,pBendLine2End->vertex);
	else
		dist3=100;	//双面板dist3无效
	if(dist3==100&&GetFaceN()>=3)
		Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X钢板火曲线信息提取有误!",handle);
	for(pVertex=vertex_list.GetFirst();pVertex&&(dist2<10||dist3<10);pVertex=vertex_list.GetNext())
	{
		if(pVertex->vertex.feature==2&&pBendPlane2Pick!=pVertex&&dist2<10)
		{
			if(pBendLine1Start&&pBendLine1End)
				dist2=DistOf2dPtLine(pVertex->vertex,pBendLine1Start->vertex,pBendLine1End->vertex);
			pBendPlane2Pick=pVertex;
		}
		if(pVertex->vertex.feature==3&&pBendPlane3Pick==NULL&&dist3<10)
		{
			if(pBendLine2Start&&pBendLine2End)
				dist3=DistOf2dPtLine(pVertex->vertex,pBendLine2Start->vertex,pBendLine2End->vertex);
			pBendPlane3Pick=pVertex;
		}
	}
	if(GetFaceN()>=2&&pBendLine1Start&&pBendLine1End&&pBendPlane2Pick)
	{
		GEPOINT huoquvec=pBendLine1End->vertex-pBendLine1Start->vertex;
		GEPOINT verifyvec=GetRealVertex(pBendPlane2Pick->vertex)-pBendLine1Start->vertex;
		GEPOINT verifynorm=verifyvec^huoquvec;
		normalize(verifynorm);
		vector_trans(verifynorm,ucs,TRUE,TRUE);
		if(verifynorm*GetHuoquFaceNorm(0)<-EPS)
		{
			pVertex=pBendLine1Start;
			pBendLine1Start=pBendLine1End;
			pBendLine1End=pVertex;
		}
	}
	if(GetFaceN()>=3&&pBendLine2Start&&pBendLine2End&&pBendPlane3Pick)
	{
		GEPOINT huoquvec=pBendLine2End->vertex-pBendLine2Start->vertex;
		GEPOINT verifyvec=GetRealVertex(pBendPlane3Pick->vertex)-pBendLine2Start->vertex;
		GEPOINT verifynorm=verifyvec^huoquvec;
		normalize(verifynorm);
		vector_trans(verifynorm,ucs,TRUE,TRUE);
		if(verifynorm*GetHuoquFaceNorm(1)<-EPS)
		{
			pVertex=pBendLine2Start;
			pBendLine2Start=pBendLine2End;
			pBendLine2End=pVertex;
		}
	}
	if(isNullCursor)
	{
		vertex_list.GetTail();
		vertex_list.GetNext();
	}
	else
		vertex_list.pop_stack(pushed);
	//
	ppBendLine1Start!=NULL ? *ppBendLine1Start=pBendLine1Start : NULL;
	ppBendLine1End  !=NULL ? *ppBendLine1End  =pBendLine1End   : NULL;
	ppBendLine2Start!=NULL ? *ppBendLine2Start=pBendLine2Start : NULL;
	ppBendLine2End  !=NULL ? *ppBendLine2End  =pBendLine2End   : NULL;
	if(pPlate&&pPlate->IsPeakPointPlate())
	{
		if(pBendLine1Start==NULL||pBendLine2Start==NULL)
			return false;
	}
	if(GetFaceN()>=2&&(pBendLine1Start==NULL||pBendLine1End==NULL))
		return false;
	if(GetFaceN()>=3&&(pBendLine2Start==NULL||pBendLine2End==NULL))
		return false;
	return true;
}
bool CLDSGeneralPlate::IntelliRecogWeldLine(PARTSET* pSearchSet/*=NULL*/)
{
	BOOL bRecogWeldParent=TRUE;
	CLDSParamPlate* pParamPlate=NULL;
	PARTSET relativePlates;
	if(m_nClassTypeId==CLS_PARAMPLATE)
	{
		pParamPlate=(CLDSParamPlate*)this;	//肘板也需要同普通钢板那进行焊缝识别处理
		if(((CLDSParamPlate*)this)->m_iParamType==TYPE_ELBOWPLATE)
		{
			CLDSParamPlate::RIBPLATE_ELBOW elbow;
			elbow.LoadParams(pParamPlate);
			_hPartWeldParent=elbow.hDatumTube.ToLong();
			bRecogWeldParent=FALSE;
		}
		else if(wiType==TYPE_CIRCOVERPLATE||wiType==TYPE_CIRRIBPLATE||wiType==TYPE_TUBERIBPLATE)
		{
			_hPartWeldParent=pParamPlate->m_hBaseRod;
			bRecogWeldParent=FALSE;
			if(pSearchSet==NULL&&(wiType==TYPE_CIRRIBPLATE||wiType==TYPE_TUBERIBPLATE))
			{
				CLDSGeneralPlate::DESIGN_PARAM itemparam;
				long h1=0,h2=0;
				if(pParamPlate->GetDesignItemValue('E',&itemparam))
				{
					if(itemparam.cValueType==3)	//句柄+数值
						h1=itemparam.value.hPart;
					else //if(item_param.cValueType)
						h1=itemparam.ToLong();
				}
				if(pParamPlate->GetDesignItemValue('F',&itemparam))
				{
					if(itemparam.cValueType==3)	//句柄+数值
						h2=itemparam.value.hPart;
					else //if(item_param.cValueType)
						h2=itemparam.ToLong();
				}
				CLDSPlate *pDatumPlate1=(CLDSPlate*)BelongModel()->FromPartHandle(h1,CLS_PLATE);
				CLDSPlate *pDatumPlate2=(CLDSPlate*)BelongModel()->FromPartHandle(h2,CLS_PLATE);
				if(pDatumPlate1)
					relativePlates.append(pDatumPlate1);
				if(pDatumPlate2)
					relativePlates.append(pDatumPlate2);
				CLDSPart *pPart=BelongModel()->FromPartHandle(m_hPartWeldParent);
				if(pPart)
					relativePlates.append(pPart);
				if(relativePlates.getCount()>0)
					pSearchSet=&relativePlates;
			}
		}
		else if(pParamPlate->m_iParamType==TYPE_RIBPLATE)
		{
			long hDatumPart=0,hNode=0,hAssPart1=0,hAssPart2=0,work_norm_style=0;
			if(pSearchSet==NULL&&pParamPlate->GetDesignItemValue('A',&hDatumPart))//基准构件
			{	//暂时只处理钢管塔脚肋板 wht 18-08-13
				GetDesignItemValue('C',&hAssPart1);	//辅助构件1
				GetDesignItemValue('D',&hAssPart2);	//辅助构件2
				CLDSPart *pDatumPart=BelongModel()->FromPartHandle(abs(hDatumPart),CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
				CLDSPart *pAssistantPart1=BelongModel()->FromPartHandle(abs(hAssPart1),CLS_PLATE,CLS_PARAMPLATE);
				CLDSPart *pAssistantPart2=BelongModel()->FromPartHandle(hAssPart2,CLS_PLATE,CLS_PARAMPLATE);
				if( pDatumPart!=NULL&&pAssistantPart1!=NULL&&pAssistantPart2==NULL&&
					pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pAssistantPart1->IsPlate())
				{	//肘板型肋板
					relativePlates.append(pDatumPart);
					relativePlates.append(pAssistantPart1);
					pSearchSet=&relativePlates;
				}
				else if(pDatumPart!=NULL)
				{
					relativePlates.append(pDatumPart);
					if(pAssistantPart1!=NULL)
						relativePlates.append(pAssistantPart1);
					if(pAssistantPart2!=NULL)
						relativePlates.append(pAssistantPart2);
					pSearchSet=&relativePlates;
				}
				else if(pSearchSet==NULL)
					return false;
			}
			else if(pSearchSet==NULL)
				return false;
		}
		else if(pParamPlate->IsFL()||wiType==TYPE_UEND||wiType==TYPE_ROLLEND||wiType==TYPE_XEND||wiType==TYPE_LINEANGLERIBPLATE)
		{
			_hPartWeldParent=pParamPlate->m_hBaseRod;
			return true;
		}
		else
			return false;
	}
	else if(m_nClassTypeId!=CLS_PLATE)
		return false;	//只处理普通钢板
	PARTSET allparts;
	if(pSearchSet==NULL)
	{
		pSearchSet=&allparts;
		CLDSPart* pPart=NULL;
		BOOL pushed=BelongModel()->PushPartStack();
		for(pPart=BelongModel()->EnumPartFirst();pPart;pPart=BelongModel()->EnumPartNext())
		{
			if(pPart!=this)
				allparts.append(pPart);
		}
		if(pushed)
			BelongModel()->PopPartStack(pushed);
	}
	PROFILE_VERTEX* pVertex,*pPrevVertex=vertex_list.GetTail();
	f3dLine line;
	f3dArcLine arcline;
	long hWeldParentPart=m_hPartWeldParent;
	double weldlength=0;
	line.startPt=GetRealVertex(pPrevVertex->vertex);
	coord_trans(line.startPt,ucs,TRUE,TRUE);
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		BOOL pushed2=vertex_list.push_stack();
		line.endPt=GetRealVertex(pVertex->vertex);
		coord_trans(line.endPt,ucs,TRUE,TRUE);
		if(pPrevVertex->type>0&&pParamPlate==NULL)
		{	//出于计算效率考虑，暂不处理肘板上的弧形焊缝　wjh-2016.8.7
			pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&ucs);
			arcline.CoordTrans(ucs,true);
		}
		if(!pPrevVertex->m_bRollEdge)
		{	//卷边与焊缝相排斥
			CLDSPart* pPart=NULL;
			BOOL pushed=pSearchSet->push_stack();
			for(pPart=pSearchSet->GetFirst();pPart;pPart=pSearchSet->GetNext())
			{
				if(pPart==this||pPart->m_bVirtualPart||!pPart->ModuleTogetherWith(pPart))
					continue;
				if((pPrevVertex->type==0&&pPart->IsWeldWith(line.startPt,line.endPt))||
					(pPrevVertex->type >0&&pPart->IsWeldWith(arcline)))
					pPrevVertex->weldline.hWeldMotherPart=pPart->handle;
				else
					continue;
				pPrevVertex->m_bWeldEdge=true;
				pPrevVertex->weldline.hWeldMotherPart=pPart->handle;
				break;
			}
			pSearchSet->pop_stack(pushed);

			double len=pPart!=NULL ? DISTANCE(pPrevVertex->vertex,pVertex->vertex) : 0;
			if(len>weldlength)
			{	//取较长的焊缝边所贴附构件作为默认焊接父构件
				weldlength=len;
				hWeldParentPart=pPart->handle;
			}
		}
		pPrevVertex=pVertex;
		line.startPt=line.endPt;
		vertex_list.pop_stack(pushed2);
	}
	if(bRecogWeldParent)
		m_hPartWeldParent=hWeldParentPart;
	return m_hPartWeldParent>0;
}
//lineStartCoord与lineEndCoord均为模型坐标系下的绝对坐标
bool CLDSGeneralPlate::IsWeldWith(double* lineStartCoord,double* lineEndCoord)
{
	GEPOINT lineStart(lineStartCoord),lineEnd(lineEndCoord);
	GEPOINT lineStdVec=lineEnd-lineStart;
	double length=DISTANCE(lineStart,lineEnd);
	if(length<10)	//边长过小，不进行焊缝检测
		return false;
	lineStdVec/=length;
	GEPOINT huoqufacenorm[2];
	int face_N=1;
	if(GetClassTypeId()==CLS_PLATE)
	{
		face_N=((CLDSPlate*)this)->face_N;
		for(int i=0;i<face_N-1;i++)
			huoqufacenorm[i]=((CLDSPlate*)this)->huoqufacenorm[i];
	}
	if(fabs(lineStdVec*ucs.axis_z)<EPS)
	{	//该边线与当前板基准面法线垂直
		coord_trans(lineStart,ucs,FALSE,TRUE);
		//加工间隙取2mm太大，会将非焊接关系的连板误判为焊接关系（如：横隔面共用板与正侧面连接板，横隔面挂板与正面连接板） wht 18-10-24
		double clearance=0.5;	//2mm焊接边加工间隙误差
		if(lineStart.z<GetNormOffset()-clearance||lineStart.z>GetThick()+GetNormOffset()+clearance)
			return false;	//不与板紧贴
		coord_trans(lineEnd,ucs,FALSE,TRUE);
		f3dPoint middlePick=0.5*(lineStart+lineEnd);
		if(IsPtInPlate(middlePick))
			return true;
		else
			return false;
	}
	else if(face_N>1&&fabs(lineStdVec*huoqufacenorm[0])<EPS)
	{	//该边线与当前板１号火曲面法线垂直
	}
	else if(face_N>2&&fabs(lineStdVec*huoqufacenorm[1])<EPS)
	{	//该边线与当前板２号火曲面法线垂直
	}
	return false;
}
bool CLDSGeneralPlate::IsWeldWith(f3dArcLine& arcline)
{
	return false;
}
//得到连接板最小包络平面矩形
f2dRect CLDSGeneralPlate::GetMinWrapRect(UCS_STRU *pRectucs/*=NULL*/,double minDistance/*=0*/,BOOL bExportDeformedProfile/*FALSE*/)
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->GetMinWrapRect(pRectucs,minDistance,bExportDeformedProfile);
	f2dRect rect;
	rect.SetRect(f2dPoint(0,0),f2dPoint(0,0));
	UCS_STRU tmucs,minUcs;
	SCOPE_STRU scope,min_scope;
	int i=0,n=vertex_list.GetNodeNum();
	if (n<3)
		return rect;
	PROFILE_VERTEX *vertex_arr = new PROFILE_VERTEX[n];
	if (minDistance > EPS)
	{
		ATOM_LIST<PROFILE_VERTEX> list;
		CalEquidistantShape(minDistance,&list);
		for(PROFILE_VERTEX *pVertex=list.GetFirst();pVertex;pVertex=list.GetNext(),i++)
			vertex_arr[i]=*pVertex;
	}
	else
	{
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext(),i++)
			vertex_arr[i]=*pVertex;
	}
	if (GetClassTypeId()==CLS_PLATE&&bExportDeformedProfile)
	{
		for (i=0;i<n;i++)
			vertex_arr[i].vertex=((CLDSPlate*)this)->GetDeformedVertex(vertex_arr[i].vertex);
	}
	tmucs.axis_z.Set(0,0,1);
	double minarea=10000000000000000;	//预置任一大数
	BOOL bTurn = FALSE;
	n = i;
	double fRollEdgeOffsetDist=GetThick();	//卷边外移量默认为钢板厚度 wht 11-06-27
	for(i=0;i<n;i++)
	{
		tmucs.axis_x=vertex_arr[(i+1)%n].vertex-vertex_arr[i].vertex;
		if(tmucs.axis_x.IsZero())
			continue;	//接近重点
		tmucs.origin=vertex_arr[i].vertex;
		tmucs.axis_x.z = tmucs.origin.z = 0;
		tmucs.axis_y=tmucs.axis_z^tmucs.axis_x;
		normalize(tmucs.axis_x);
		normalize(tmucs.axis_y);
		//卷边放到下边统一考虑 wht 11-06-01
		//if(vertex_arr[i].m_bRollEdge)
		//	tmucs.origin-=tmucs.axis_y*vertex_arr[i].manu_space;
		scope.ClearScope();
		for(int j=0;j<n;j++)
		{
			f3dPoint tm_vec=vertex_arr[(j+1)%n].vertex-vertex_arr[j].vertex;
			if(tm_vec.mod()<EPS2)
				continue;	//跳过重复顶点 wht 11-01-23
			f3dPoint vertice=vertex_arr[j].vertex;
			coord_trans(vertice,tmucs,FALSE);
			scope.VerifyVertex(vertice);
			if(vertex_arr[j].type==0&&vertex_arr[j].m_bRollEdge)
			{	//考虑卷边带来的面积变化 wht 11-06-01
				vector_trans(tm_vec,tmucs,FALSE);		//转换到临时坐标系下 wht 11-06-27
				normalize(tm_vec);
				f3dPoint tm_norm(tm_vec.y,-tm_vec.x);	//顺时针旋转90°
				vertice+=tm_norm*(abs(vertex_arr[j].manu_space)+fRollEdgeOffsetDist); //考虑卷边偏移量 wht 11-07-19
				if(vertex_arr[j].roll_edge_offset_dist>0)	//考虑卷边外移量(此处仅考虑按垂直方向偏移)
					vertice+=tm_norm*vertex_arr[j].roll_edge_offset_dist;
				scope.VerifyVertex(vertice);
				//
				vertice=vertex_arr[(j+1)%n].vertex;
				coord_trans(vertice,tmucs,FALSE);
				vertice+=tm_norm*(abs(vertex_arr[j].manu_space)+fRollEdgeOffsetDist);
				if(vertex_arr[j].roll_edge_offset_dist>0)	//考虑卷边外移量(此处仅考虑按垂直方向偏移)
					vertice+=tm_norm*vertex_arr[j].roll_edge_offset_dist;
				scope.VerifyVertex(vertice);
			}	
			else if(vertex_arr[j].type>0)
			{
				f3dArcLine arcLine;
				f3dPoint worknorm;
				if(GetClassTypeId()==CLS_PLATE)
				{
					CLDSPlate* pPlate=(CLDSPlate*)this;
					if(vertex_arr[j].vertex.feature==2)
						worknorm=pPlate->huoqufacenorm[0];
					else if(vertex_arr[j].vertex.feature==3)
						worknorm=pPlate->huoqufacenorm[1];
					else
						worknorm=ucs.axis_z;
				}
				else
					worknorm=ucs.axis_z;
				vector_trans(worknorm,ucs,FALSE);
				vector_trans(worknorm,tmucs,FALSE);
				f3dPoint start_vertex=vertex_arr[j].vertex;
				f3dPoint end_vertex=vertex_arr[(j+1)%n].vertex;
				coord_trans(start_vertex,tmucs,FALSE);
				coord_trans(end_vertex,tmucs,FALSE);
				int sign=1;
				if(vertex_arr[j].type==1)	//指定圆弧夹角
				{
					arcLine.CreateMethod2(start_vertex,end_vertex,worknorm,vertex_arr[j].sector_angle);
					if(vertex_arr[j].sector_angle<0)
						sign=-1;
				}
				else if(vertex_arr[j].type==2)	//指定圆弧半径
				{	
					f3dPoint center_pick=vertex_arr[j].center;
					coord_trans(center_pick,tmucs,FALSE);	//将圆心坐标(ucs相对坐标)转换到tmucs下 wht 12-06-14
					if(vertex_arr[j].radius<0)
						sign=-1;
					arcLine.CreateMethod3(start_vertex,end_vertex,sign*worknorm,sign*vertex_arr[j].radius,center_pick);
				}
				else //if(vertex_arr[j]->type==3)	//椭圆弧，在统计面积时暂按椭圆的包络圆处理
				{	
					f3dPoint center_pick=vertex_arr[j].center;
					f3dPoint columnnorm=vertex_arr[j].column_norm;
					normalize(worknorm);
					normalize(columnnorm);
					coord_trans(center_pick,ucs,FALSE);
					coord_trans(center_pick,tmucs,FALSE);
					vector_trans(columnnorm,ucs,FALSE);
					vector_trans(columnnorm,tmucs,FALSE);
					double cosa=worknorm*columnnorm;
					if(vertex_arr[j].radius<0)
						sign=-1;
					arcLine.CreateMethod3(start_vertex,end_vertex,sign*worknorm,fabs(vertex_arr[j].radius/cosa),center_pick);
				}
				//计算由于圆弧带来的面积区域变化
				int nSlices=CalArcResolution(arcLine.Radius(),arcLine.SectorAngle(),1.0,5.0,18);
				double angle=arcLine.SectorAngle()/nSlices;
				for(int i=1;i<nSlices;i++)
				{
					f3dPoint pos=arcLine.PositionInAngle(angle*i);
					scope.VerifyVertex(pos);
				}
				scope.VerifyVertex(arcLine.Start());
				scope.VerifyVertex(arcLine.End());
			}
		}
		if(minarea>scope.wide()*scope.high())
		{
			minarea=scope.wide()*scope.high();
			min_scope=scope;
			minUcs = tmucs;
		}
	}
	delete []vertex_arr;
	rect.topLeft.Set(min_scope.fMinX,min_scope.fMaxY);
	rect.bottomRight.Set(min_scope.fMaxX,min_scope.fMinY);
	if (pRectucs)
	{
		f3dPoint origin(min_scope.fMinX,min_scope.fMinY);
		coord_trans(origin,minUcs,TRUE);
		coord_trans(origin,ucs,TRUE);
		*pRectucs=minUcs;
		pRectucs->origin = origin;
		vector_trans(pRectucs->axis_x,ucs,TRUE);
		vector_trans(pRectucs->axis_y,ucs,TRUE);
		vector_trans(pRectucs->axis_z,ucs,TRUE);
		//对绘图坐标系进行右手定则规范
		pRectucs->axis_z = ucs.axis_z;
		pRectucs->axis_y = pRectucs->axis_z^pRectucs->axis_x;
		normalize(pRectucs->axis_y);
		pRectucs->axis_x = pRectucs->axis_y^pRectucs->axis_z;
		normalize(pRectucs->axis_x);
	}
	return rect;
}

//计算连接板最小包络平面矩形面积mm2
double CLDSGeneralPlate::CalAreaOfMinWrapRect(SCOPE_STRU *pScope/*=NULL*/)
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->CalAreaOfMinWrapRect(pScope);
	f2dRect rect = GetMinWrapRect();
	return rect.Width()*rect.Height();
}

//计算连接板净包络多边形面积mm2
double CLDSGeneralPlate::CalAreaOfNetWrap()
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->CalAreaOfNetWrap();
	UCS_STRU tmucs;
	int i=0,n=vertex_list.GetNodeNum();
	PROFILE_VERTEX *pVertex,*pPrevVertex=NULL;
	double wrap_area=0,area_odd=0;	//由于弦变圆弧造成的总面积增减
	BOOL bEnd=FALSE;
	for(pVertex=vertex_list.GetFirst();1;pVertex=vertex_list.GetNext())
	{
		if(pVertex==NULL)
		{
			bEnd=TRUE;
			pVertex=vertex_list.GetFirst();
		}
		if(pPrevVertex==NULL)
			pPrevVertex=pVertex;
		else if(pPrevVertex->type>0)
		{
			f3dArcLine arcLine;
			f3dPoint worknorm;
			if(GetClassTypeId()==CLS_PLATE)
			{
				CLDSPlate* pPlate=(CLDSPlate*)this;
				if(pPrevVertex->vertex.feature==2)
					worknorm=pPlate->huoqufacenorm[0];
				else if(pPrevVertex->vertex.feature==3)
					worknorm=pPlate->huoqufacenorm[1];
				else
					worknorm=ucs.axis_z;
			}
			else
				worknorm=ucs.axis_z;
			vector_trans(worknorm,ucs,FALSE);
			int sign=1;
			if(pPrevVertex->type==1)	//指定圆弧夹角
			{
				arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,worknorm,pPrevVertex->sector_angle);
				if(pPrevVertex->sector_angle<0)
					sign=-1;
			}
			else if(pPrevVertex->type==2)	//指定圆弧半径
			{	
				//圆心拾取点为相对坐标不用再次进行转换 wht 12-06-14
				//f3dPoint center_pick=pPrevVertex->center;
				//coord_trans(center_pick,ucs,FALSE);
				if(pPrevVertex->radius<0)
					sign=-1;
				arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,worknorm,pPrevVertex->radius,pPrevVertex->center);
				
			}
			else //if(pPrevVertex->type==3)	//椭圆弧，在统计面积时暂按椭圆的包络圆处理
			{	
				f3dPoint center_pick=pPrevVertex->center;
				f3dPoint columnnorm=pPrevVertex->column_norm;
				normalize(worknorm);
				normalize(columnnorm);
				coord_trans(center_pick,ucs,FALSE);
				vector_trans(columnnorm,ucs,FALSE);
				double cosa=worknorm*columnnorm;
				if(pPrevVertex->radius<0)
					sign=-1;
				arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,sign*worknorm,fabs(pPrevVertex->radius/cosa),center_pick);
			}
			//计算由于圆弧造成的面积增减值
			double sector_angle=arcLine.SectorAngle();
			if(sign==-1)	//凹弧
				area_odd-=(sector_angle-sin(sector_angle))*0.5*(arcLine.Radius()*arcLine.Radius());
			else
				area_odd+=(sector_angle-sin(sector_angle))*0.5*(arcLine.Radius()*arcLine.Radius());
		}
		else if(pPrevVertex->m_bRollEdge)
		{	
			int roll_edge_offset_dist=pPrevVertex->roll_edge_offset_dist+ftoi(GetThick());
			if(roll_edge_offset_dist>0)	
			{	//计算因卷边及卷边偏移量造成的面积增加值 wht 11-05-20
				int cur_index=vertex_list.GetCurrentIndex();
				int prev_prev_index=(cur_index+n-2)%n;	//
				int next_index=(cur_index+1)%n;
				PROFILE_VERTEX *pPrevPrevVertex=vertex_list.GetByIndex(prev_prev_index);
				PROFILE_VERTEX *pNextVertex=vertex_list.GetByIndex(next_index);
				//GetByIndex()会改变链表当前索引值,需要恢复到cur_index,否则将无法结束循环 wht 11-07-04
				pVertex=vertex_list.GetByIndex(cur_index);	
				if(pPrevPrevVertex==NULL||pNextVertex==NULL)	//未找到前一点或后一点按长方形计算面积增加值
				{
					double edge_len=DISTANCE(pPrevVertex->vertex,pVertex->vertex);
					double edge_high=roll_edge_offset_dist+abs(pPrevVertex->manu_space);
					wrap_area+=(edge_high*edge_len);
				}
				else 
				{	//根据偏移量按梯形计算卷边偏移量造成的面积增加值
					f3dPoint prev_vec=pPrevVertex->vertex-pPrevPrevVertex->vertex;	//前一边方向
					f3dPoint next_vec=pVertex->vertex-pNextVertex->vertex;	//下一边方向
					f3dPoint cur_vec=pVertex->vertex-pPrevVertex->vertex;	//当前边方向
					normalize(cur_vec);
					f3dPoint offset_vec=cur_vec;
					offset_vec.Set(offset_vec.y,-offset_vec.x);	//顺指针旋转90°
					normalize(offset_vec);
					double cosa=fabs(cur_vec*prev_vec.normalized());//判断当前边与前一边是否共线
					double cosb=fabs(cur_vec*next_vec.normalized());//判断当前边与下一边是否共线
					f3dLine edge_line,extend_line;
					//
					edge_line.startPt=pPrevVertex->vertex+offset_vec*roll_edge_offset_dist;
					edge_line.endPt=pVertex->vertex+offset_vec*roll_edge_offset_dist;
					edge_line.startPt.z=edge_line.endPt.z=0;
					if(cosa<EPS_COS2)
					{	//当前边与前一边不共线	
						extend_line.startPt.Set(pPrevVertex->vertex.x,pPrevVertex->vertex.y);
						extend_line.endPt.Set(extend_line.startPt.x+prev_vec.x,extend_line.startPt.y+prev_vec.y);
						extend_line.startPt.z=extend_line.endPt.z=0;
						Int3dll(extend_line,edge_line,edge_line.startPt);
					}
					if(cosb<EPS_COS2)
					{	//当前边与下一边不共线
						extend_line.startPt.Set(pVertex->vertex.x,pVertex->vertex.y);
						extend_line.endPt.Set(extend_line.startPt.x+next_vec.x,extend_line.startPt.y+next_vec.y);
						extend_line.startPt.z=extend_line.endPt.z=0;
						Int3dll(extend_line,edge_line,edge_line.endPt);
					}
					f3dPoint new_edge_vec=edge_line.endPt-edge_line.startPt;
					normalize(new_edge_vec);	//卷边后新边方向
					if(new_edge_vec*cur_vec<0)	//得到的新边始末端颠倒，边始端取垂直方向为延伸方向 wht 09-08-14
						edge_line.startPt=pPrevVertex->vertex+offset_vec*roll_edge_offset_dist;
					new_edge_vec=edge_line.endPt-edge_line.startPt;
					normalize(new_edge_vec);
					if(new_edge_vec*cur_vec<0)	//得到的新边始末端颠倒，边末端取垂直方向为延伸方向 wht 09-08-14
						edge_line.endPt=pVertex->vertex+offset_vec*roll_edge_offset_dist;
					//卷边偏移量造成的面积增加值
					double up_edge_len=DISTANCE(edge_line.startPt,edge_line.endPt);
					double down_edge_len=DISTANCE(pPrevVertex->vertex,pVertex->vertex);
					double high=roll_edge_offset_dist;
					wrap_area+=((up_edge_len+down_edge_len)*high*0.5);	//梯形面积
					//卷边造成的面积增加值
					wrap_area+=(up_edge_len*abs(pPrevVertex->manu_space));
				}
			}
			else 
			{
				f3dPoint edge_vec=pVertex->vertex-pPrevVertex->vertex;
				wrap_area+=edge_vec.mod()*abs(pPrevVertex->manu_space);
			}
		}
		pPrevVertex = pVertex;
		if(bEnd)
			break;
	}
	f3dPoint *pnt_arr=new f3dPoint[n];
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext(),i++)
		pnt_arr[i]=pVertex->vertex;
	try{
		for(i=1;i<n-1;i++)
		{
			double result=DistOf2dPtLine(pnt_arr[i+1],pnt_arr[0],pnt_arr[i]);
			if(result>0)		// 后点在线左侧，正三角形面积
				wrap_area+=CalTriArea(pnt_arr[0].x,pnt_arr[0].y,pnt_arr[i].x,pnt_arr[i].y,pnt_arr[i+1].x,pnt_arr[i+1].y);
			else if(result<0)	// 后点在线右侧，负三角形面积
				wrap_area-=CalTriArea(pnt_arr[0].x,pnt_arr[0].y,pnt_arr[i].x,pnt_arr[i].y,pnt_arr[i+1].x,pnt_arr[i+1].y);
		}
		wrap_area+=area_odd;
	}
	catch(char *sError)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Log2File()->Log("The outline vertex of plate 0X%X have problems，area calculation is wrong(%s)！",handle,sError);
#else 
		Log2File()->Log("0X%X板轮廓顶点有问题，面积计算有误(%s)！",handle,sError);
#endif
	}
	delete []pnt_arr;
	return wrap_area;
}
double CLDSGeneralPlate::CalPerimeter()	//计算钢板周长,mm
{
	double perimeter=0;
	PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
	for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		f3dArcLine arcline;	//其中已集成了普通直边特例
		pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&ucs);
		perimeter+=arcline.Length();
		if(pPrevVertex->m_bRollEdge)
			perimeter+=pPrevVertex->manu_space*2;
		pPrevVertex=pVertex;
	}
	return perimeter;
}
//是否为角钢带劲板
BOOL CLDSGeneralPlate::IsAngleRibPlate()
{
	CLDSLineAngle* pWeldJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINEANGLE);
	if(pWeldJg==NULL)
		return FALSE;
	int nWeldEdge=0;
	f3dPoint xWingVec,yWingVec,weldVec[2];
	xWingVec=pWeldJg->GetWingVecX();
	yWingVec=pWeldJg->GetWingVecY();
	PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
	for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pPrevVertex->m_bWeldEdge&&pPrevVertex->type==0)
		{
			f3dPoint vec=(pVertex->vertex-pPrevVertex->vertex).normalized();
			vector_trans(vec,ucs,TRUE);
			if(weldVec[0].IsZero())
				weldVec[0]=vec;
			else if(weldVec[1].IsZero())
				weldVec[1]=vec;
			nWeldEdge++;
		}
		pPrevVertex=pVertex;
	}
	if(nWeldEdge!=2)
		return FALSE;
	double dd1=fabs(weldVec[0]*xWingVec)>fabs(weldVec[0]*yWingVec)?fabs(weldVec[0]*xWingVec):fabs(weldVec[0]*yWingVec);
	double dd2=fabs(weldVec[1]*xWingVec)>fabs(weldVec[1]*yWingVec)?fabs(weldVec[1]*xWingVec):fabs(weldVec[1]*yWingVec);
	if(dd1>EPS_COS2 && dd2>EPS_COS2)
		return TRUE;
	return FALSE;
}
//是否为套管焊接板
BOOL CLDSGeneralPlate::IsBushWeldPlate()
{
	for(CLDSLineTube* pBush=(CLDSLineTube*)BelongModel()->EnumPartFirst(CLS_LINETUBE);pBush;pBush=(CLDSLineTube*)BelongModel()->EnumPartNext(CLS_LINETUBE))
	{
		if((pBush->FuncType()==CLDSLineTube::FUNC_WIREBUSH||(pBush->pStart==NULL&&pBush->pEnd==NULL))&&
			pBush->m_hPartWeldParent==this->handle)
			return TRUE;
	}
	return FALSE;
}
//焊接板一般采用双面焊(角焊缝),一条焊接边会有两条焊缝线，根据青岛汇金通提供的焊缝线计算方法，进行计算
double CLDSGeneralPlate::CalWeldLineLength()
{
	double fWeldLineLen=0;
	CLDSPart* pWeldParent=BelongModel()->FromPartHandle(m_hPartWeldParent);
	if(pWeldParent && pWeldParent->IsPlate())
	{	//焊接钢板通过螺栓连接在主材角钢上，进行焊接时紧靠角钢的焊缝线需要进行磨平处理
		f3dPoint linkJgVec;
		for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			for(THICK_ITEM item=pBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=pBolt->GetNextL0ThickItem())
			{
				if(!item.IsHandle()||item.val==this->handle)
					continue;
				CLDSLineAngle* pLinkJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(item.val,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pLinkJg&&towupper(pLinkJg->Layer(1))=='Z')
					linkJgVec=(pLinkJg->End()-pLinkJg->Start()).normalized();
			}
			if(!linkJgVec.IsZero())
				break;
		}
		//根据焊接边计算焊缝线
		PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
		for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(!pPrevVertex->m_bWeldEdge)
			{
				pPrevVertex=pVertex;
				continue;
			}
			f3dPoint vec=(pVertex->vertex-pPrevVertex->vertex).normalized();
			vector_trans(vec,ucs,TRUE);
			f3dArcLine arcline;	//其中已集成了普通直边特例
			pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&ucs);
			double fWeldLen=arcline.Length();
			if(pPrevVertex->type==0 && fabs(linkJgVec*vec)>EPS_COS2)
			{	//焊缝线方向与连接角钢轴向同向，一条为角焊缝，另一条焊缝线需要进行磨平处理
				//角焊缝线
				if(Thick<=6)
					fWeldLen*=0.9;
				else if(Thick<=8)
					fWeldLen*=1;
				else if(Thick<=12)
					fWeldLen*=1.7;
				else if(Thick<=16)
					fWeldLen*=2.8;
				else if(Thick<=20)
					fWeldLen*=3.8;
				else if(Thick<=24)
					fWeldLen*=4.7;
				else if(Thick<=28)
					fWeldLen*=5.5;
				fWeldLineLen+=fWeldLen;
				//焊后磨平焊缝线
				fWeldLen=arcline.Length();
				if(Thick<=10)
					fWeldLen*=1;
				else if(Thick<=16)
					fWeldLen*=1.6;
				else if(Thick<=25)
					fWeldLen*=2.2;
				fWeldLineLen+=fWeldLen;
			}
			else
			{	//两条焊缝线都为角焊缝
				fWeldLen*=2;
				if(Thick<=6)
					fWeldLen*=0.9;
				else if(Thick<=8)
					fWeldLen*=1;
				else if(Thick<=12)
					fWeldLen*=1.7;
				else if(Thick<=16)
					fWeldLen*=2.8;
				else if(Thick<=20)
					fWeldLen*=3.8;
				else if(Thick<=24)
					fWeldLen*=4.7;
				else if(Thick<=28)
					fWeldLen*=5.5;
				fWeldLineLen+=fWeldLen;
			}
			pPrevVertex=pVertex;
		}
	}
	else if(pWeldParent&&pWeldParent->IsAngle()&&IsAngleRibPlate())
	{	//角钢肋板,根据角钢的肢宽指定焊缝长度
		CLDSLineAngle* pWeldJg=(CLDSLineAngle*)pWeldParent;
		if(pWeldJg->GetWidth()<=100)
			fWeldLineLen=600;
		else if(pWeldJg->GetWidth()<=160)
			fWeldLineLen=1000;
		else
			fWeldLineLen=1300;
	}
	else if(IsBushWeldPlate())
	{	//套管焊接板,焊缝线长度指定500
		fWeldLineLen=500;
	}
	else if((fWeldLineLen=CalWeldEdgeLength())>0)
	{	//默认都为角焊缝
		fWeldLineLen*=2;
		if(Thick<=6)
			fWeldLineLen*=0.9;
		else if(Thick<=8)
			fWeldLineLen*=1;
		else if(Thick<=12)
			fWeldLineLen*=1.7;
		else if(Thick<=16)
			fWeldLineLen*=2.8;
		else if(Thick<=20)
			fWeldLineLen*=3.8;
		else if(Thick<=24)
			fWeldLineLen*=4.7;
		else if(Thick<=28)
			fWeldLineLen*=5.5;
	}
	return fWeldLineLen;
}
double CLDSGeneralPlate::CalWeldEdgeLength()	//计算钢板焊缝边长,mm
{
	double weldlength=0;
	PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
	for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pPrevVertex->m_bWeldEdge)	//普通直边
		{
			f3dArcLine arcline;	//其中已集成了普通直边特例
			pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&ucs);
			weldlength+=arcline.Length();
		}
		pPrevVertex=pVertex;
	}
	return weldlength;
}
double CLDSPlate::CalWeldEdgeLength()	//计算钢板焊缝边长,mm
{
	double weldlength=CLDSGeneralPlate::CalWeldEdgeLength();
	CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
	if(pDatumTube&&IsTransPlate())
	{	//贯通板
		f3dPoint startPt=pDatumTube->Start(),endPt=pDatumTube->End();
		coord_trans(startPt,ucs,FALSE);
		coord_trans(endPt,ucs,FALSE);
		startPt.z=endPt.z=0;
		int nIntersPt=0;
		f3dLine inters_line;
		PROFILE_VERTEX *pPrevVertex=vertex_list.GetTail();
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(nIntersPt==0&&Int3dll(startPt,endPt,pVertex->vertex,pPrevVertex->vertex,inters_line.startPt)>0)
				nIntersPt++;
			else if(nIntersPt==1&&Int3dll(startPt,endPt,pVertex->vertex,pPrevVertex->vertex,inters_line.endPt)>0)
			{
				nIntersPt++;
				break;
			}
			pPrevVertex=pVertex;
		}
		if(nIntersPt==2)
			weldlength+=DISTANCE(inters_line.startPt,inters_line.endPt)*2;	//贯通板两侧需要焊接
	}
	return weldlength;
}
//计算钢板切割长度大于指定值(mm)的边数
long CLDSGeneralPlate::GetEdgeNum(double lenGreatThan/*=0*/)
{
	long edge_num=0;
	PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
	for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(lenGreatThan<EPS)
			edge_num++;
		else
		{
			f3dArcLine arcline;	//其中已集成了普通直边特例
			pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&ucs);
			if(arcline.Length()>lenGreatThan)
				edge_num++;
		}
		pPrevVertex=pVertex;
	}
	return edge_num;
}
UCS_STRU CLDSGeneralPlate::GetUnifiedACS()
{
	UCS_STRU acs=ucs;
	acs.origin+=acs.axis_z*GetNormOffset();
	return acs;
}
//计算指定坐标所在面面号
int CLDSGeneralPlate::GetFaceNoByPos(GEPOINT pos)
{
	if(vertex_list.GetNodeNum()<=0)
		return 0;
	bool bHasTopPoint=(GetClassTypeId()==CLS_PLATE&&IsPeakPointPlate());
	ARRAY_LIST<f3dPoint> ptList[3];
	PROFILE_VERTEX *pPreVertex = vertex_list.GetTail();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		//处理圆弧边引起的轮廓区域变化 wxc-2019.6.17
		if (pPreVertex->type > 0)
		{
			f3dArcLine arcLine;
			int sign = (pPreVertex->sector_angle < 0) ? -1 : 1;
			if (pPreVertex->type == 1)		//指定圆弧角度
				arcLine.CreateMethod2(pPreVertex->vertex, pVertex->vertex, f3dPoint(0, 0, sign), fabs(pPreVertex->sector_angle));
			else if (pPreVertex->type == 2)	//指定圆弧R
				arcLine.CreateMethod3(pPreVertex->vertex, pVertex->vertex, f3dPoint(0, 0, 1), pPreVertex->radius, pPreVertex->center);
			else if (pPreVertex->type == 3)	//椭圆弧
			{
				f3dPoint center = pPreVertex->center;
				f3dPoint col_axis = pPreVertex->column_norm;
				coord_trans(center, ucs, FALSE);
				vector_trans(col_axis, ucs, FALSE);
				if (pPreVertex->radius < 0)
					sign = -1;
				arcLine.CreateEllipse(center, pPreVertex->vertex, pVertex->vertex, col_axis, f3dPoint(0, 0, sign), sign*pPreVertex->radius);
			}
			int nSlices = CalArcResolution(arcLine.Radius(), arcLine.SectorAngle(), 1.0, 5.0, 18);
			double angle = arcLine.SectorAngle() / nSlices;
			for (int i = 1; i < nSlices; i++)
			{
				f3dPoint pos = arcLine.PositionInAngle(angle*i);
				if (pVertex->vertex.feature < 10)
					ptList[max(0, pVertex->vertex.feature - 1)].append(pos);
				else
					ptList[max(0, pPreVertex->vertex.feature - 1)].append(pos);
			}
		}
		//处理当前轮廓点
		if(pVertex->vertex.feature<10)
			ptList[max(0,pVertex->vertex.feature-1)].append(pVertex->vertex);
		else
		{
			int iFaceNo=pVertex->vertex.feature%10;
			if(bHasTopPoint&&pVertex->vertex.feature==12||pVertex->vertex.feature==13)
			{
				if(pPreVertex ->vertex.feature==2|| pPreVertex->vertex.feature ==3)
				{
					ptList[0].append(((CLDSPlate*)this)->top_point);
					ptList[0].append(pVertex->vertex);
					ptList[iFaceNo-1].append(pVertex->vertex);
					ptList[iFaceNo-1].append(((CLDSPlate*)this)->top_point);
				}
				else //if(pPreVertex ->vertex.feature==1)
				{
					ptList[0].append(pVertex->vertex);
					ptList[0].append(((CLDSPlate*)this)->top_point);
					ptList[iFaceNo-1].append(((CLDSPlate*)this)->top_point);
					ptList[iFaceNo-1].append(pVertex->vertex);
				}
			}
			else
			{
				ptList[0].append(pVertex->vertex);
				if(iFaceNo==2||iFaceNo==3)
					ptList[iFaceNo-1].append(pVertex->vertex);
			}
		}
		pPreVertex = pVertex;
	}
	POLYGON poly;
	for(int i=0;i<GetFaceN();i++)
	{
		BOOL successed=poly.CreatePolygonRgn(ptList[i].m_pData,ptList[i].GetSize());
		if (!successed)
		{
			CXhChar50 label("0x%X", handle);
			if (strlen(this->Label) > 0)
				label.Append(CXhChar16("{%s}", Label));
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "%s钢板第%d面外形生成失败!", (char*)label, i);
		}
		else if(poly.PtInRgn(pos)==1)
			return i+1;
	}
	return 0;
}
//输入一个法线向量，返回此向量所在的板平面号0,1,2
int CLDSGeneralPlate::GetNormFaceI(CLDSBolt* pBolt)
{
	if(pBolt->IsLocalCoordLocation(this))
		return pBolt->des_base_pos.reside_plane;
	else
	{
		//TODO:有时间此处应优化为按相对于火曲线的位置判断 wjh-2017.12.8
		//double j1=fabs(ucs.axis_z*pBolt->get_norm());
		//double j2=GetFaceN()>1?fabs(GetHuoquFaceNorm(0)*pBolt->get_norm()):0;
		//double j3=GetFaceN()>2?fabs(GetHuoquFaceNorm(1)*pBolt->get_norm()):0;
		int iFaceNo=GetNormFaceI(pBolt->get_norm());
		if(GetFaceN()==3&&fabs(GetHuoquFaceNorm(0)*GetHuoquFaceNorm(1))>EPS_COS)
		{	//三面板两火曲线法线近似时根据螺栓位置计算螺栓归属面
			if(iFaceNo>0)
			{
				f3dPoint posInFace2,posInFace3;
				f3dPoint huoquLineStart1=GetHuoquLine(0).startPt,huoquLineStart2=GetHuoquLine(1).startPt;
				coord_trans(huoquLineStart1,ucs,TRUE);
				coord_trans(huoquLineStart2,ucs,TRUE);
				Int3dlf(posInFace2,pBolt->ucs.origin,pBolt->ucs.axis_z,huoquLineStart1,GetHuoquFaceNorm(0));
				Int3dlf(posInFace3,pBolt->ucs.origin,pBolt->ucs.axis_z,huoquLineStart2,GetHuoquFaceNorm(1));
				coord_trans(posInFace2,ucs,FALSE);
				coord_trans(posInFace3,ucs,FALSE);
				posInFace2=GetSpreadVertex(posInFace2,2);
				posInFace3=GetSpreadVertex(posInFace3,3);
				posInFace2.z=posInFace3.z=0;
				if(GetFaceNoByPos(posInFace2)==2)
					iFaceNo=1;
				if(GetFaceNoByPos(posInFace3)==3)
					iFaceNo=2;
			}
		}
		return iFaceNo;
	}
}
int CLDSGeneralPlate::GetNormFaceI(const double* facenorm)
{
	GEPOINT face_norm(facenorm);
	int nPlanes=GetFaceN();
	if(nPlanes==1)
		return 0;
	else if(nPlanes==2)
	{
		normalize(face_norm);
		GEPOINT bendplanenorm=GetHuoquFaceNorm(0);
		if(fabs(face_norm*ucs.axis_z)>fabs(face_norm*bendplanenorm))
			return 0;
		else
			return 1;
	}
	else if(nPlanes==3)
	{
		normalize(face_norm);
		GEPOINT bendplanenorm0=GetHuoquFaceNorm(0);
		GEPOINT bendplanenorm1=GetHuoquFaceNorm(1);
		if( fabs(face_norm*ucs.axis_z)>fabs(face_norm*bendplanenorm0)
			&&fabs(face_norm*ucs.axis_z)>fabs(face_norm*bendplanenorm1))
			return 0;
		else if(fabs(face_norm*bendplanenorm0)>fabs(face_norm*bendplanenorm1))
			return 1;
		else
			return 2;
	}
	else
		return 0;
}

void CLDSGeneralPlate::GetPeerPlateUcs(CLDSGeneralPlate *pPeerPlate,int base_i,BOOL bReverse,UCS_STRU& acs)
{
	UCS_STRU tm_ucs_arr[2],acsArr[2]={GetUnifiedACS(),pPeerPlate->GetUnifiedACS()};
	int n=vertex_list.GetNodeNum();
	int baseIndexArr[2]={0,base_i};
	DYN_ARRAY<PROFILE_VERTEX> vertex_list_arr[2];
	vertex_list_arr[0].Resize(n);
	vertex_list_arr[1].Resize(n);
	for(int i=0;i<n;i++)
	{
		vertex_list_arr[0][i]=*vertex_list.GetByIndex(i);
		vertex_list_arr[1][i]=*pPeerPlate->vertex_list.GetByIndex(i);
	}
	for(int i=0;i<2;i++)
	{
		f3dPoint cur_vertex,prev_vertex,next_vertex;
		cur_vertex=vertex_list_arr[i][baseIndexArr[i]].vertex;
		prev_vertex=vertex_list_arr[i][(baseIndexArr[i]+n-1)%n].vertex;
		next_vertex=vertex_list_arr[i][(baseIndexArr[i]+1)%n].vertex;
		cur_vertex.z=prev_vertex.z=next_vertex.z=0;
		if(bReverse&&i==1)
		{	//反比相同时前后点对调
			next_vertex=vertex_list_arr[i][(baseIndexArr[i]+n-1)%n].vertex;
			prev_vertex=vertex_list_arr[i][(baseIndexArr[i]+1)%n].vertex;
			tm_ucs_arr[i].axis_z.Set(0,0,-1);
		}
		else
			tm_ucs_arr[i].axis_z.Set(0,0,1);
		tm_ucs_arr[i].origin=cur_vertex;
		tm_ucs_arr[i].axis_x=next_vertex-cur_vertex;
		tm_ucs_arr[i].axis_y=tm_ucs_arr[i].axis_z^tm_ucs_arr[i].axis_x;
		normalize(tm_ucs_arr[i].axis_x);
		normalize(tm_ucs_arr[i].axis_y);
		normalize(tm_ucs_arr[i].axis_z);
		//将坐标系原点及轴向转到绝对坐标系下
		coord_trans(tm_ucs_arr[i].origin,acsArr[i],TRUE);
		vector_trans(tm_ucs_arr[i].axis_x,acsArr[i],TRUE);
		vector_trans(tm_ucs_arr[i].axis_y,acsArr[i],TRUE);
		vector_trans(tm_ucs_arr[i].axis_z,acsArr[i],TRUE);
	}
	//将基准板坐标系转换到tm_ucs_arr[0]下，再转换到tm_ucs_arr[1]下获取当前钢板的装配坐标系
	UCS_STRU newUcs=acsArr[0];
	coord_trans(newUcs.origin,tm_ucs_arr[0],FALSE);
	vector_trans(newUcs.axis_x,tm_ucs_arr[0],FALSE);
	vector_trans(newUcs.axis_y,tm_ucs_arr[0],FALSE);
	vector_trans(newUcs.axis_z,tm_ucs_arr[0],FALSE);
	coord_trans(newUcs.origin,tm_ucs_arr[1],TRUE);
	vector_trans(newUcs.axis_x,tm_ucs_arr[1],TRUE);
	vector_trans(newUcs.axis_y,tm_ucs_arr[1],TRUE);
	vector_trans(newUcs.axis_z,tm_ucs_arr[1],TRUE);
	if(bReverse)	//反比外形相同
		newUcs.origin-=newUcs.axis_z*GetThick();
	//更新钢板火曲面法线
	for(int i=0;i<GetFaceN()-1;i++)
	{
		coord_trans(pPeerPlate->GetHuoquFaceNorm(i),tm_ucs_arr[1],TRUE);
		coord_trans(pPeerPlate->GetHuoquLine(i).startPt,tm_ucs_arr[1],TRUE);
		coord_trans(pPeerPlate->GetHuoquLine(i).endPt,tm_ucs_arr[1],TRUE);
		coord_trans(pPeerPlate->GetHuoquFaceNorm(i),newUcs,FALSE);
		coord_trans(pPeerPlate->GetHuoquLine(i).startPt,newUcs,FALSE);
		coord_trans(pPeerPlate->GetHuoquLine(i).endPt,newUcs,FALSE);
	}
	acs=newUcs;
}
int compare_func1(const f3dPoint &pt1,const f3dPoint &pt2)
{
	if(pt1.y-pt2.y>eps2)
		return 1;
	else if(pt2.y-pt1.y>eps2)
		return -1;
	else
	{
		if(pt1.x-pt2.x>eps2)
			return 1;
		else
			return -1;
	}
}
int compare_func2(const BOLTHOLE_INFO &hole1,const BOLTHOLE_INFO &hole2)
{
	f3dPoint pt1(hole1.centre.x,hole1.centre.y,0);
	f3dPoint pt2(hole2.centre.x,hole2.centre.y,0);
	return compare_func1(pt1,pt2);
}
BOOL CLDSGeneralPlate::GetProfileByBoltHoles(ATOM_LIST<f3dPoint>& pt_list)
{
	ATOM_LIST<BOLTHOLE_INFO> holeList;
	int nHoleNum=GetAllBoltHoles(holeList);
	if(nHoleNum<2)
		return FALSE;
	const double EPSILON = 0.5;
	//2、根据螺栓孔计算最小轮廓边
	pt_list.Empty();
	ARRAY_LIST<f3dPoint> ptArr;
	ptArr.SetSize(0,nHoleNum);
	BOOL bOneAxisLineLs=TRUE;
	f3dLine axis_line;
	for(int i=0;i<nHoleNum;i++)
	{
		f3dPoint vertex(holeList[i].posX,holeList[i].posY,0);
		ptArr.append(vertex);
		if(i==0)
			axis_line.startPt=vertex;
		else if(i==1)
			axis_line.endPt=vertex;
		else if(bOneAxisLineLs&&axis_line.PtInLine(vertex)==0)
			bOneAxisLineLs=FALSE;
	}
	if(bOneAxisLineLs)	
	{	//所有螺栓都在同一直线上
		CQuickSort<f3dPoint>::QuickSort(ptArr.m_pData,nHoleNum,compare_func1);
		for(int i=0;i<nHoleNum;i++)
			pt_list.append(ptArr[i]);
		return FALSE;
	}
	//根据螺栓计算最小外轮廓
	CQuickSort<f3dPoint>::QuickSort(ptArr.m_pData,nHoleNum,compare_func1);
	f3dPoint firStartPt=ptArr[0];
	f3dPoint startPt=firStartPt;
	pt_list.append(startPt);
	int nLoops=0;
	for(;;)
	{
		if(nLoops==nHoleNum)
			break;
		nLoops++;
		for(BOLTHOLE_INFO* pEndHole=holeList.GetFirst();pEndHole;pEndHole=holeList.GetNext())
		{
			f3dPoint endPt(pEndHole->posX,pEndHole->posY);
			if(endPt.IsEqual(startPt))
				continue;
			BOOL bOutLs=TRUE;
			holeList.push_stack();
			for(BOLTHOLE_INFO* pHole=holeList.GetFirst();pHole;pHole=holeList.GetNext())
			{
				f3dPoint vertex(pHole->posX,pHole->posY);
				if(vertex.IsEqual(startPt) || vertex.IsEqual(endPt))
					continue;
				double dd=DistOf2dPtLine(vertex,startPt,endPt);
				if(dd<-EPSILON)
				{	//清除掉内侧螺栓
					bOutLs=FALSE;
					break;
				}
				else if(dd>-EPSILON&&dd<EPSILON)
				{	//清除掉共线螺栓
					f3dPoint line_vec=endPt-startPt;
					f3dPoint vec1=vertex-startPt;
					f3dPoint vec2=vertex-endPt;
					if(line_vec*vec1>0&&line_vec*vec2>0)
					{
						bOutLs=FALSE;
						break;
					}
				}
			}
			holeList.pop_stack();
			if(bOutLs)		//终端螺栓为边缘螺栓
				break;
		}
		if(pEndHole)//找到下一颗外轮廓螺栓
		{
			//末螺栓为第一颗外轮廓螺栓时终止(证明已找了一周)
			if(firStartPt.IsEqual(f3dPoint(pEndHole->posX,pEndHole->posY,0)))
				break;
			f3dPoint vertex(pEndHole->posX,pEndHole->posY);
			pt_list.append(vertex);
			startPt=vertex;
		}
	}
	if(pt_list.GetNodeNum()<3)
		return FALSE;
	return TRUE;
}
int CLDSGeneralPlate::GetAllBoltHoles(ATOM_LIST<BOLTHOLE_INFO>& holeList/*=NULL*/)
{
	holeList.Empty();
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		f3dPoint ls_pos;
		SpreadLsPos(pLsRef->GetLsPtr(),ls_pos);
		holeList.append(BOLTHOLE_INFO(ls_pos,pLsRef));
	}
	for(BOLTHOLE_INFO* pHole=EnumFirstBoltHole();pHole;pHole=EnumNextBoltHole())
		holeList.append(*pHole);
	return holeList.GetNodeNum();
}
bool CLDSGeneralPlate::IsEqualProfile(ATOM_LIST<f3dPoint>& ptList1,ATOM_LIST<f3dPoint>& ptList2,double epsilon
										,int& base_i,BOOL& bReverse)
{
	int iRetCode=1;
	for(int i=0;i<2;i++)
	{
		bReverse=i;
		for(int j=0;j<vertex_list.GetNodeNum();j++)
		{
			iRetCode=CompareProfileVertex(ptList1,ptList2,bReverse,j,epsilon);
			if(iRetCode==0)
			{	
				base_i=j;
				break;
			}
		}
		if(iRetCode==0)
			return true;
	}
	return false;
}
//对比同一直线上的两组顶点是否相同
int CLDSGeneralPlate::CompareVertexOnLine(ATOM_LIST<f3dPoint>& ptList1,ATOM_LIST<f3dPoint>& ptList2,BOOL bReverseOrder,double epsilon)
{
	if(ptList1.GetNodeNum()!=ptList2.GetNodeNum())
		return 1;
	//初始化各顶点特征信息
	int n=ptList1.GetNodeNum();
	ARRAY_LIST<double> base_dist_arr;
	ARRAY_LIST<double> peer_dist_arr;
	f3dPoint base_start,peer_start;
	for(int i=0;i<n;i++)
	{
		if(i==0)
			base_start=ptList1[i];
		else
			base_dist_arr.append(DISTANCE(base_start,ptList1[i]));
		int index=i;
		if(bReverseOrder)
			index=n-i-1;
		if(i==0)
			peer_start=ptList2[index];
		else
			peer_dist_arr.append(DISTANCE(peer_start,ptList2[index]));
	}
	//进行比较
	int iRet=0;
	for(i=0;i<base_dist_arr.GetSize();i++)
	{
		if(fabs(base_dist_arr[i]-peer_dist_arr[i])>epsilon)
		{
			iRet=1;
			break;
		}
	}
	return iRet;
}
//指定正反比，对比两轮廓边外形是否相同
int CLDSGeneralPlate::CompareProfileVertex(ATOM_LIST<f3dPoint>& ptList1,ATOM_LIST<f3dPoint>& ptList2,BOOL bReverseOrder,
											int base_i,double epsilon)
{
	struct ve_item{
		f3dPoint vertex;
		double pre_len;
		double cur_len;
		double cosine_angle;
	};
	if(ptList1.GetNodeNum()!=ptList2.GetNodeNum())
		return 1;
	//初始化各顶点特征信息
	int n=ptList1.GetNodeNum();
	f3dPoint vec_cur,vec_pre;
	struct ve_item* base_item_arr = new struct ve_item[n];
	struct ve_item* peer_item_arr = new struct ve_item[n];
	for(int i=0;i<n;i++)
	{
		base_item_arr[i].vertex = ptList1[i];
		vec_pre=ptList1[(n+i-1)%n]-ptList1[i];
		vec_cur=ptList1[(i+1)%n]-ptList1[i];
		base_item_arr[i].pre_len = vec_pre.mod();
		base_item_arr[i].cur_len = vec_cur.mod();
		base_item_arr[i].cosine_angle = vec_cur.normalized()*vec_pre.normalized();
		if(bReverseOrder)
		{	//反面比
			int index=n-i-1;
			peer_item_arr[i].vertex = ptList2[index];
			vec_pre = ptList2[(index+1)%n]-ptList2[index];
			vec_cur = ptList2[(n+index-1)%n]-ptList2[index];
			peer_item_arr[i].pre_len = vec_pre.mod();
			peer_item_arr[i].cur_len = vec_cur.mod();
			peer_item_arr[i].cosine_angle = vec_cur.normalized()*vec_pre.normalized();
		}
		else
		{	//正面比
			peer_item_arr[i].vertex = ptList2[i];
			vec_pre=ptList2[(n+i-1)%n]-ptList2[i];
			vec_cur=ptList2[(i+1)%n]-ptList2[i];
			peer_item_arr[i].pre_len = vec_pre.mod();
			peer_item_arr[i].cur_len = vec_cur.mod();
			peer_item_arr[i].cosine_angle = vec_cur.normalized()*vec_pre.normalized();
		}
	}
	//进行比较
	int iRet=0;
	for(i=0;i<n;i++)
	{
		if(fabs(base_item_arr[i].pre_len-peer_item_arr[(base_i+i)%n].pre_len)>epsilon ||
			fabs(base_item_arr[i].cur_len-peer_item_arr[(base_i+i)%n].cur_len)>epsilon ||
			fabs(base_item_arr[i].cosine_angle-peer_item_arr[(base_i+i)%n].cosine_angle)>epsilon*1e-2)
		{
			iRet=1;
			break;
		}
	}
	delete []base_item_arr;
	delete []peer_item_arr;
	return iRet;
}
static void CalCompareUcs(ATOM_LIST<f3dPoint>& pt_list,UCS_STRU& acs,UCS_STRU plate_ucs,int base_i,BOOL bReverse=FALSE)
{
	int n=pt_list.GetNodeNum();
	DYN_ARRAY<f3dPoint> pt_arr;
	pt_arr.Resize(n);
	for(int i=0;i<n;i++)		
		pt_arr[i]=pt_list[i];
	int baseIndex=base_i;
	if(bReverse)
		baseIndex=n-base_i-1;
	f3dPoint cur_vertex,next_vertex;
	cur_vertex=pt_arr[baseIndex];
	next_vertex=pt_arr[(baseIndex+1)%n];
	if(bReverse)
	{	//反比相同时前后点对调
		next_vertex=pt_arr[(baseIndex+n-1)%n];
		acs.axis_z.Set(0,0,-1);
	}
	else
		acs.axis_z.Set(0,0,1);
	cur_vertex.z=next_vertex.z=0;
	acs.origin=cur_vertex;
	acs.axis_x=next_vertex-cur_vertex;
	acs.axis_y=acs.axis_z^acs.axis_x;
	normalize(acs.axis_x);
	normalize(acs.axis_y);
	normalize(acs.axis_z);
}
bool CLDSGeneralPlate::CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg/*=NULL*/,BOOL bCmpls/*=TRUE*/,
										PEER_PART *pPeerInfo/*=NULL*/,double epsilon/*=1.0*/)
{
	if(pPeerPart->GetClassTypeId()!=GetClassTypeId())
		return false;
	if(pPeerPart->m_hBlock!=m_hBlock)
		return false;	//从属部件不一致,返回false
	CLDSGeneralPlate* pPeerPlate=(CLDSGeneralPlate*)pPeerPart;
	if(pPeerInfo)
	{
		pPeerInfo->hPeerPart=pPeerPlate->handle;
		pPeerInfo->m_hBlock=m_hBlock;
		pPeerInfo->csBase=GetUnifiedACS();
		pPeerInfo->csPeer=pPeerPart->GetUnifiedACS();
	}
	if(handle==pPeerPlate->handle)
		return true;
	//比较基本信息(材质、厚度、面数、点数、孔数)
	if(GetThick()!=pPeerPlate->GetThick())
		return false;
	if(cMaterial!=pPeerPlate->cMaterial)
		return false;
	if(GetFaceN()!=pPeerPlate->GetFaceN())
		return false;
	if(vertex_list.GetNodeNum()<3)
		return false;
	if(vertex_list.GetNodeNum()!=pPeerPlate->vertex_list.GetNodeNum())
		return false;
	ATOM_LIST<BOLTHOLE_INFO> holeList1,holeList2;
	int nHole1=GetAllBoltHoles(holeList1);
	int nHole2=pPeerPlate->GetAllBoltHoles(holeList2);
	if(bCmpls&&nHole1!=nHole2)
		return false;
	//根据对称关系进行钢板的工艺比较
	PEER_BOLT* pPeerBolt=NULL;
	if(pMirmsg && pMirmsg->axis_flag!=0)
	{	//
		UCS_STRU mir_ucs;
		mir_ucs.origin=GetMirPos(ucs.origin,*pMirmsg);
		mir_ucs.axis_x=GetMirVector(ucs.axis_x,*pMirmsg);
		mir_ucs.axis_y=GetMirVector(ucs.axis_y,*pMirmsg);
		mir_ucs.axis_z=GetMirVector(ucs.axis_z,*pMirmsg);
		//比较轮廓点
		f3dPoint datumPt,peerPt;
		for(int i=0;i<vertex_list.GetNodeNum();i++)
		{
			datumPt=vertex_list[i].vertex;
			peerPt=pPeerPlate->vertex_list[i].vertex;
			coord_trans(peerPt,pPeerPlate->ucs,TRUE);
			coord_trans(peerPt,mir_ucs,FALSE);
			peerPt.z=0;
			if(!datumPt.IsEqual(peerPt,epsilon))
				return false;
		}
		//比较螺栓
		if(bCmpls)
		{
			for(int i=0;i<holeList1.GetNodeNum();i++)
			{
				datumPt.Set(holeList1[i].posX,holeList1[i].posY,0);
				peerPt.Set(holeList2[i].posX,holeList2[i].posY,0);
				coord_trans(peerPt,pPeerPlate->ucs,TRUE);
				coord_trans(peerPt,mir_ucs,FALSE);
				peerPt.z=0;
				if(!datumPt.IsEqual(peerPt,epsilon))
					return false;
			}
		}
		//初始化比对信息
		if(pPeerInfo)
		{		
			GetPeerPlateUcs(pPeerPlate,0,FALSE,pPeerInfo->csPeer);
			//记录螺栓比对信息
			for(int i=0;i<holeList1.GetNodeNum();i++)
			{
				pPeerBolt=pPeerInfo->hashBoltsByBase.Add(holeList1[i].BoltHandle());
				pPeerBolt->ciState=0;
				pPeerBolt->hPeerBolt=holeList2[i].BoltHandle();
				//
				pPeerBolt=pPeerInfo->hashBoltsByPeer.Add(holeList2[i].BoltHandle());
				pPeerBolt->ciState=0;
				pPeerBolt->hPeerBolt=holeList1[i].BoltHandle();
			}
		}
		return true;
	}
	//根据正反比进行钢板的外形比较
	int base_i=0;
	BOOL bReverse=FALSE;
	UCS_STRU datum_ucs,peer_ucs;
	f3dPoint datumPt,peerPt;
	ATOM_LIST<f3dPoint> datumPtList,peerPtList;
	if(bCmpls==FALSE || nHole1<=1)
	{	//不比较螺栓或钢板上少于1个螺栓孔时优先比较钢板外形
		PROFILE_VERTEX* pVertex=NULL;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			datumPtList.append(pVertex->vertex);
		for(pVertex=pPeerPlate->vertex_list.GetFirst();pVertex;pVertex=pPeerPlate->vertex_list.GetNext())
			peerPtList.append(pVertex->vertex);
		if(IsEqualProfile(datumPtList,peerPtList,epsilon,base_i,bReverse)==FALSE)
			return false;
		if(bCmpls && nHole1==1)
		{	//比较单个螺栓
			CalCompareUcs(datumPtList,datum_ucs,GetUnifiedACS(),0);
			CalCompareUcs(peerPtList,peer_ucs,pPeerPlate->GetUnifiedACS(),base_i,bReverse);
			datumPt.Set(holeList1[0].posX,holeList1[0].posY,0);
			coord_trans(datumPt,datum_ucs,FALSE);
			peerPt.Set(holeList2[0].posX,holeList2[0].posY,0);
			coord_trans(peerPt,peer_ucs,FALSE);
			datumPt.z=peerPt.z=0;
			if(datumPt.IsEqual(peerPt,epsilon)==FALSE)
				return false;
		}
	}
	else
	{	//钢板上多个螺栓孔，优先根据螺栓进行比较
		BOOL bRet1=GetProfileByBoltHoles(datumPtList);
		BOOL bRet2=pPeerPlate->GetProfileByBoltHoles(peerPtList);
		int nPtNum=datumPtList.GetNodeNum();
		if(nPtNum<=1 || nPtNum!=peerPtList.GetNodeNum())
			return false;
		int nInnerLoop=0;	//此变量用于控制内层循环次数
		if(!bRet1)	//螺栓共线时，内层循环只需一次即可
			nInnerLoop=1;
		else		//螺栓非共线时，需逐个遍历顶点
			nInnerLoop=nPtNum;
		bool bEqual=false;
		for(int i=0;i<2;i++)
		{
			for(int j=0;j<nInnerLoop;j++)
			{
				if(!bRet1&&CompareVertexOnLine(datumPtList,peerPtList,i,epsilon)!=0)	//比较共线螺栓
					continue;
				else if(CompareProfileVertex(datumPtList,peerPtList,i,j,epsilon)!=0)	//比较螺栓构成的多边形
					continue;
				//螺栓比较相同，则以螺栓为基准建立钢板的临时坐标系，将钢板轮廓点转换的比较坐标系下
				ATOM_LIST<f3dPoint> pt_list1,pt_list2;
				PROFILE_VERTEX* pVertex=NULL;
				CalCompareUcs(datumPtList,datum_ucs,GetUnifiedACS(),0);
				CalCompareUcs(peerPtList,peer_ucs,pPeerPlate->GetUnifiedACS(),j,i);
				base_i=-1;	//初始化base_i
				f3dPoint first_pt;
				for(int k=0;k<vertex_list.GetNodeNum();k++)
				{
					datumPt=vertex_list[k].vertex;
					coord_trans(datumPt,datum_ucs,FALSE);
					datumPt.z=0;
					if(k==0)
						first_pt=datumPt;
					pt_list1.append(datumPt);
					//
					peerPt=pPeerPlate->vertex_list[k].vertex;
					coord_trans(peerPt,peer_ucs,FALSE);
					peerPt.z=0;
					pt_list2.append(peerPt);
					if(base_i<0&&first_pt.IsEqual(peerPt,epsilon))
						base_i=k;
				}
				if(base_i<0)
					continue;
				//比较钢板外形
				bReverse=(i>0);
				int ii=0,nVertexCount=pt_list1.GetNodeNum();
				if(!bReverse)
				{	//正比
					for(ii=0;ii<nVertexCount;ii++)
					{
						if(!pt_list1[ii].IsEqual(pt_list2[(base_i+ii)%nVertexCount],epsilon))
							break;
					}
				}
				else
				{	//反比
					for(ii=0;ii<nVertexCount;ii++)
					{
						if(!pt_list1[ii].IsEqual(pt_list2[(base_i+nVertexCount-ii)%nVertexCount],epsilon))
							break;
					}
				}
				bEqual=(ii==nVertexCount);
				if(bEqual)
					break;
			}
			if(bEqual)
				break;
		}
		if(!bEqual)
			return false;
	}
	if(pPeerInfo)
	{
		GetPeerPlateUcs(pPeerPlate,base_i,bReverse,pPeerInfo->csPeer);
		//记录螺栓比对信息
		f3dPoint ls_pos;
		BOLTHOLE_INFO hole;
		ARRAY_LIST<BOLTHOLE_INFO> holeArr1,holeArr2;
		for(int i=0;i<nHole1;i++)
		{	
			hole=holeList1[i];
			ls_pos.Set(hole.posX,hole.posY,0);
			coord_trans(ls_pos,ucs,TRUE);
			coord_trans(ls_pos,pPeerInfo->csBase,FALSE);
			hole.posX=ls_pos.x;
			hole.posY=ls_pos.y;
			holeArr1.append(hole);
			//
			hole=holeList2[i];
			ls_pos.Set(hole.posX,hole.posY,0);
			coord_trans(ls_pos,pPeerPlate->ucs,TRUE);
			coord_trans(ls_pos,pPeerInfo->csPeer,FALSE);
			hole.posX=ls_pos.x;
			hole.posY=ls_pos.y;
			holeArr2.append(hole);
		}
		CQuickSort<BOLTHOLE_INFO>::QuickSort(holeArr1.m_pData,nHole1,compare_func2);
		CQuickSort<BOLTHOLE_INFO>::QuickSort(holeArr2.m_pData,nHole1,compare_func2);
		for(int i=0;i<nHole1;i++)
		{
			pPeerBolt=pPeerInfo->hashBoltsByBase.Add(holeArr1[i].BoltHandle());
			pPeerBolt->ciState=0;
			pPeerBolt->hPeerBolt=holeArr2[i].BoltHandle();
			//
			pPeerBolt=pPeerInfo->hashBoltsByPeer.Add(holeArr2[i].BoltHandle());
			pPeerBolt->ciState=0;
			pPeerBolt->hPeerBolt=holeArr1[i].BoltHandle();
		}
	}
	return true;
}
//用于返回外包切线的夹角和切线 WJH-2004.9.22
double WrapLine2dcc(double start_x,double start_y,double start_radius,double end_x,double end_y,double end_radius,f2dLine &wrap_tan_line)
{
	f2dCircle cir1,cir2;
	f2dPoint pick1,pick2,axis_norm;
	cir1.centre.Set(start_x,start_y);
	cir2.centre.Set(end_x,end_y);
	cir1.radius=start_radius;
	cir2.radius=end_radius;
	axis_norm.Set(end_y-start_y,start_x-end_x);//顺时针转90度,(x,y)-->(y,-x)
	pick1.Set(start_x+axis_norm.x,start_y+axis_norm.y);
	pick2.Set(end_x+axis_norm.x,end_y+axis_norm.y);
	if(TangLine2dcc(cir1,pick1,cir2,pick2,wrap_tan_line)!=1)
		return -1;	//没找到合法的外包切线
	return Cal2dLineAng(wrap_tan_line.startPt,wrap_tan_line.endPt);
}

double WrapLine2dcc(f2dCircle cir1,f2dCircle cir2,f2dLine &wrap_tan_line)
{
	return WrapLine2dcc(cir1.centre.x,cir1.centre.y,cir1.radius,cir2.centre.x,cir2.centre.y,cir2.radius,wrap_tan_line);
}

BOOL CLDSPlate::CalWrapLsProfile(double angle_lim/*=60.0*/)
{
	if(face_N>=3)
		return FALSE;
	ATOM_LIST<f2dCircle> ls_cir_list,out_ls_list;
	f3dPoint ls_pos;
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		if(!SpreadLsPos(pLsRef,ls_pos))
			continue;	//展开螺栓位置失败
		f2dCircle ls_cir;
		ls_cir.centre.Set(ls_pos.x,ls_pos.y);
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,(*pLsRef)->get_d());
		ls_cir.radius = LsSpace.PlateEdgeSpace;
		double dd,dd0=fabs((*pLsRef)->get_norm()*ucs.axis_z);
		//判断螺栓在第几面上
		ls_cir.feature=1;		//第一面的螺栓
		for(int i=0;i<face_N-1;i++)
		{
			dd=fabs((*pLsRef)->get_norm()*huoqufacenorm[i]);
			if(dd0<dd)
			{
				dd0=dd;
				ls_cir.feature=i+2;
			}
		}
		ls_cir_list.append(ls_cir);
	}
	//查找外轮廓特征螺栓
		//通过最简便的方式找出其中一颗外轮廓特征螺栓,作为外轮廓首螺栓
	BOOL bHasTwoLs=FALSE,bOneAxisLineLs=TRUE;	//所有螺栓都在一条心线上的情况
	int start_space,end_space;
	f3dLine axis_line;
	f2dCircle *pLsCir=NULL,*pStartLsCir=NULL,*pEndLsCir=NULL,*pFirstStartLsCir=NULL;
	for(pLsCir=ls_cir_list.GetFirst();pLsCir;pLsCir=ls_cir_list.GetNext())
	{
		if(pStartLsCir==NULL)
		{
			pStartLsCir=pLsCir;
			axis_line.startPt.Set(pLsCir->centre.x,pLsCir->centre.y);
			axis_line.startPt.feature=pLsCir->feature;
			axis_line.endPt=axis_line.startPt;	//添加用于处理单螺栓情况板外形的创建
			axis_line.endPt.feature=pLsCir->feature;
			end_space=start_space=ftoi(pLsCir->radius);
		}
		else
		{
			if(bHasTwoLs)
			{
				if(bOneAxisLineLs)
				{
					f3dPoint centre;
					centre.Set(pLsCir->centre.x,pLsCir->centre.y);
					int ret=axis_line.PtInLine(centre);	//螺栓不在螺栓射线上
					if(ret==-2)			//起点外侧
					{
						axis_line.startPt=centre;
						axis_line.startPt.feature=pLsCir->feature;
						start_space=ftoi(pLsCir->radius);
					}
					else if(ret==-1)	//终点外侧
					{
						axis_line.endPt=centre;
						axis_line.endPt.feature=pLsCir->feature;
						end_space=ftoi(pLsCir->radius);
					}
					if(ret==0)
						bOneAxisLineLs=FALSE;
				}
			}
			else
			{
				bHasTwoLs=TRUE;
				axis_line.endPt.Set(pLsCir->centre.x,pLsCir->centre.y);
				axis_line.endPt.feature=pLsCir->feature;
				end_space=ftoi(pLsCir->radius);
			}
			if(pStartLsCir->centre.y>pLsCir->centre.y)
				pStartLsCir=pLsCir;		//查获最低处的螺栓
		}
	}
	if(bOneAxisLineLs)
	{	//所有螺栓都在同一直线上
		f3dPoint axis_vec=axis_line.endPt-axis_line.startPt;
		if(normalize(axis_vec)==0)
			axis_vec.Set(1);
		f3dPoint axis_norm=f3dPoint(0,0,1)^axis_vec;
		normalize(axis_norm);
		vertex_list.Empty();
		f3dPoint pt;
		pt=axis_line.startPt-start_space*axis_vec+axis_norm*start_space;
		pt.feature=axis_line.startPt.feature;
		vertex_list.append(pt);
		pt=axis_line.startPt-start_space*axis_vec-axis_norm*start_space;
		pt.feature=axis_line.startPt.feature;
		vertex_list.append(pt);
		pt=axis_line.endPt+end_space*axis_vec-axis_norm*end_space;
		pt.feature=axis_line.endPt.feature;
		vertex_list.append(pt);
		pt=axis_line.endPt+end_space*axis_vec+axis_norm*end_space;
		pt.feature=axis_line.endPt.feature;
		vertex_list.append(pt);
	}
	else
	{	//至少有三个非共线螺栓
		out_ls_list.append(*pStartLsCir);
		pFirstStartLsCir=pStartLsCir;
		//逆时针依次查找其余外轮廓特征螺栓
		int nLoops=0;
		f2dLine tan_line;
		for(;;)
		{
			if(nLoops==ls_cir_list.GetNodeNum())
				break;
			nLoops++;
			for(pEndLsCir=ls_cir_list.GetFirst();pEndLsCir;pEndLsCir=ls_cir_list.GetNext())
			{
				if(pEndLsCir==pStartLsCir)
					continue;
				BOOL bOutLs=TRUE;
				ls_cir_list.push_stack();
				//由点在线左侧算法改为直接求包络公切线夹角算法,这样
				//可以避免圆直径不同的影响 WJH-2004.09.22
				WrapLine2dcc(*pStartLsCir,*pEndLsCir,tan_line);
				for(pLsCir=ls_cir_list.GetFirst();pLsCir;pLsCir=ls_cir_list.GetNext())
				{
					if(pLsCir==pStartLsCir||pLsCir==pEndLsCir)
						continue;
					//由点在线左侧算法改为直接求包络公切线夹角算法,这样
					//可以避免圆直径不同的影响 WJH-2004.09.22
					//double dd=DistOf2dPtLine(pLsCir->centre,pStartLsCir->centre,pEndLsCir->centre);
					//if(dd<-eps)	//终端螺栓不是边缘螺栓
					double dd=DistOf2dPtLine(pLsCir->centre,tan_line.startPt,tan_line.endPt);
					if(dd<pLsCir->radius-eps)	//终端螺栓不是边缘螺栓
					{
						bOutLs=FALSE;
						break;
					}
					else if(dd>pLsCir->radius-eps&&dd<pLsCir->radius+eps)
					{	//清除掉共线螺栓
						f2dPoint perp;
						SnapPerp(&perp,tan_line,pLsCir->centre);
						f3dPoint line_vec(tan_line.endPt.x-tan_line.startPt.x,tan_line.endPt.y-tan_line.startPt.y);
						f3dPoint vec1(pLsCir->centre.x-tan_line.startPt.x,pLsCir->centre.y-tan_line.startPt.y);
						f3dPoint vec2(pLsCir->centre.x-tan_line.endPt.x,pLsCir->centre.y-tan_line.endPt.y);
						if(line_vec*vec1>0&&line_vec*vec2>0)
						{	//pLsCir为pEndLsCir的下一个共线螺栓,所以pEndLsCir不为外轮廓特征螺栓
							bOutLs=FALSE;
							break;
						}
					}
				}
				ls_cir_list.pop_stack();
				if(bOutLs)		//终端螺栓为边缘螺栓
					break;
			}
			if(pEndLsCir==pFirstStartLsCir)	//末螺栓为第一颗外轮廓螺栓时终止(证明已找了一周)
				break;
			else if(pEndLsCir)				//找到下一颗外轮廓螺栓
			{
				out_ls_list.append(*pEndLsCir);
				pStartLsCir=pEndLsCir;
			}
			else							//当前螺栓不是下一颗外轮廓螺栓
				continue;
		}
		
		//根据圆公切线,自动设计板外形
		//清空已有设计
		vertex_list.Empty();
		f3dPoint start,end,vec,out_vec,pick_start,pick_end;
		//计算公切线组成的切线边列表
		ATOM_LIST<f2dLine>edge_line_list;
		for(pStartLsCir=out_ls_list.GetFirst();pStartLsCir;pStartLsCir=out_ls_list.GetNext())
		{
			pEndLsCir=out_ls_list.GetNext();
			if(pEndLsCir==NULL)
			{
				pEndLsCir=out_ls_list.GetFirst();
				out_ls_list.GetTail();
			}
			else
				out_ls_list.GetPrev();	//复原位置
			start.Set(pStartLsCir->centre.x,pStartLsCir->centre.y);
			end.Set(pEndLsCir->centre.x,pEndLsCir->centre.y);
			vec=end-start;
			out_vec=vec^f3dPoint(0,0,1);
			normalize(out_vec);
			pick_start=start+out_vec*20;
			pick_end=end+out_vec*20;
			if(TangLine2dcc(*pStartLsCir,pick_start,*pEndLsCir,pick_end,tan_line)>0)	//切线计算成功
			{
				tan_line.startPt.feature=ftoi(pStartLsCir->radius);
				tan_line.startPt.pen.style=pStartLsCir->feature;	//利用图层名的第一个字母表示所在的面
				tan_line.endPt.feature=ftoi(pEndLsCir->radius);
				tan_line.endPt.pen.style=pEndLsCir->feature;		//利用图层名的第一个字母表示所在的面
				edge_line_list.append(tan_line);
			}
		}
		//根据公切线计算板外形
		f2dLine *pLine=NULL,*pNextLine=NULL;
		for(pLine=edge_line_list.GetFirst();pLine;pLine=edge_line_list.GetNext())
		{
			pNextLine=edge_line_list.GetNext();
			if(pNextLine==NULL)
			{
				pNextLine=edge_line_list.GetFirst();
				edge_line_list.GetTail();
			}
			else
				edge_line_list.GetPrev();	//复原位置
			f3dPoint vec_prev(pLine->startPt.x-pLine->endPt.x,pLine->startPt.y-pLine->endPt.y);
			f3dPoint vec_next(pNextLine->endPt.x-pNextLine->startPt.x,pNextLine->endPt.y-pNextLine->startPt.y);
			normalize(vec_prev);
			normalize(vec_next);
			double fai=cal_angle_of_2vec(vec_prev,vec_next);
			f3dPoint inters;
			if(Int2dpl(*pLine,*pNextLine,inters.x,inters.y)>0)
			{
				if(fai>=Pi*angle_lim/180)
				{
					inters.feature=pLine->endPt.pen.style;
					vertex_list.append(inters);
				}
				else
				{
					f3dPoint prev,next;
					double radius=pLine->endPt.feature;
					double cut_len=radius*2*(1-sin(fai/2))/sin(fai);
					prev=inters+vec_prev*cut_len;
					next=inters+vec_next*cut_len;
					prev.feature=next.feature=pLine->endPt.pen.style;
					vertex_list.append(prev);
					vertex_list.append(next);
				}
			}
		}
	}
	f3dLine line;
	f3dPoint last,now,next,inters;
	int n=vertex_list.GetNodeNum();
	for(int j=0;j<n;j++)
	{
		now  = vertex_list[j].vertex;
		next = vertex_list[(j+1)%n].vertex;
		line.startPt = now;
		line.endPt = next;
		line.startPt.z = line.endPt.z = 0;
		if(now.feature+next.feature==3)
		{
			if(Int3dpl(line,HuoQuLine[0],inters)<=0)
				continue;	//无交点
			else
			{
				int ret=line.PtInLine(inters);
				if(ret==2)
				{	//火曲点在中间，需要新生成火曲点
					inters.feature = 12;
					vertex_list.insert(inters,j+1);
					j++;
					n++;
				}
				else if(ret==1)
				{	//端点即为火曲点
					if(line.startPt==inters)
						vertex_list[j].vertex.feature=12;
					else //if(line.endPt==inters)
						vertex_list[(j+1)%n].vertex.feature=12;
				}
			}
		}
	}
	return TRUE;
}

//iResidePlane=0,1,2,分别表示基准面、１火曲面、２火曲面
bool CLDSPlate::GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* iResidePlane/*=NULL*/,int iIntersPlane/*=0*/)
{
	int retcode,iLsFace=GetNormFaceI(pBolt);	//直接传入pBolt对于计算局部坐标定位的螺栓归属工作平面更准确 wjh-2016.5.7
	if(iResidePlane)
		*iResidePlane=iLsFace;
	f3dPoint xFacePick=ucs.origin+ucs.axis_z*GetNormOffset();
	f3dPoint worknorm=ucs.axis_z;
	f3dPoint boltnorm=pBolt->ucs.axis_z;
	if(pBolt->m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
		boltnorm*=-1.0;
	if(iLsFace>0)
	{
		xFacePick=HuoQuLine[iLsFace-1].startPt;
		coord_trans(xFacePick,ucs,TRUE);
		xFacePick+=ucs.axis_z*GetNormOffset();
		worknorm=huoqufacenorm[iLsFace-1];
		retcode=Int3dlf(inters_coords,pBolt->ucs.origin,boltnorm,xFacePick,huoqufacenorm[iLsFace-1]);
	}
	else if(fabs(boltnorm*ucs.axis_z)<EPS2)
		retcode=0;	//防止螺栓与钢板近似平行时，计算异常 wjh-2019.7.29
	else
		retcode=Int3dlf(inters_coords,pBolt->ucs.origin,boltnorm,xFacePick,ucs.axis_z);
	f3dPoint inters(inters_coords);
	if(iIntersPlane==TOP_PLANE||iIntersPlane==MID_PLANE)
	{
		double dfOffset=(iIntersPlane==TOP_PLANE)?_thick:_thick*0.5;
		if(iLsFace==0)
			inters+=ucs.axis_z*dfOffset;
		else
			inters+=GetHuoquFaceNorm(iLsFace-1)*dfOffset;
	}
	else if(iIntersPlane==CLDSPart::BOLTBTM_PLANE && boltnorm*worknorm<0)
		inters+=worknorm*GetThick();
	memcpy(inters_coords,(double*)inters,24);
	return retcode==1;
}

//要求输入的螺栓必须是此连接板上的螺栓,否则
//可能不进行转换或转换错误(由实际绝对坐标-->展开相对坐标)
BOOL CLDSPlate::SpreadLsPos(CLDSBolt *pLs,double* lspos)
{
	int i;
	double angle[2];
	f3dPoint rotate_axis,tempt,vec;

	for(i=0;i<face_N-1;i++)
	{
		//计算第一面法线到其余面法线的旋转轴线
		rotate_axis = cross_prod(ucs.axis_z,huoqufacenorm[i]);
		if(i==0)	//第一面与第二面间的夹角
			angle[0] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
		else		//第一面与第三面间的夹角
			angle[1] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
			//计算当前火曲线方向
		Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);
			
		//将第一面向其余面旋转的轴线转到相对坐标系下来
		vector_trans(rotate_axis,ucs,FALSE);
		if(dot_prod(vec,rotate_axis)<0)	//当前火曲线与旋转轴线反向
		{							//火曲线方向应与旋转轴线方向一致
			tempt = HuoQuLine[i].startPt;
			HuoQuLine[i].startPt = HuoQuLine[i].endPt;
			HuoQuLine[i].endPt = tempt;
		}
	}

	GEPOINT ls_pos = pLs->ucs.origin;
	//ls_pos -= pLs->des_base_pos.norm_offset.Thick(BelongModel())*pLs->get_norm();
	int iLsFace=0;
	if(pLs->IsLocalCoordLocation(this))
		iLsFace=pLs->des_base_pos.reside_plane;
	else
		iLsFace=GetNormFaceI(pLs);
	GetBoltIntersPos(pLs,ls_pos);
	//face_N与reside_plane不一致时angle未初始化螺栓计算可能错误 wht 16-04-14
	if(face_N>1&&iLsFace==1)		//第二面上的螺栓
	{
		ls_pos-=ucs.axis_z*this->GetNormOffset();	//由于当前实际位置复原到法向偏移前的位置
		coord_trans(ls_pos,ucs,FALSE);
		rotate_point_around_axis(ls_pos,-angle[0],HuoQuLine[0]);
		coord_trans(ls_pos,ucs,TRUE);
		project_point(ls_pos,ucs.origin,ucs.axis_z);
		coord_trans(ls_pos,ucs,FALSE);
		if(lspos)
			memcpy(lspos,(double*)ls_pos,24);
		return TRUE;
	}
	else if(face_N>2&&iLsFace==2)	//第三面上的螺栓
	{
		ls_pos-=ucs.axis_z*this->GetNormOffset();	//由于当前实际位置复原到法向偏移前的位置
		coord_trans(ls_pos,ucs,FALSE);
		rotate_point_around_axis(ls_pos,-angle[1],HuoQuLine[1]);
		coord_trans(ls_pos,ucs,TRUE);
		project_point(ls_pos,ucs.origin,ucs.axis_z);
		coord_trans(ls_pos,ucs,FALSE);
		if(lspos)
			memcpy(lspos,(double*)ls_pos,24);
		return TRUE;
	}
	else				//第一面上的螺栓
	{
		project_point(ls_pos,ucs.origin,ucs.axis_z);
		coord_trans(ls_pos,ucs,FALSE);
		if(lspos)
			memcpy(lspos,(double*)ls_pos,24);
		return TRUE;
	}
}

BOOL CLDSPlate::SpreadLsPos(const double* ls_norm,double* ls_pos_coord)
{
	int i;
	double angle[2];
	f3dPoint rotate_axis,tempt,vec;
	f3dPoint ls_pos(ls_pos_coord);
	for(i=0;i<face_N-1;i++)
	{
		//计算第一面法线到其余面法线的旋转轴线
		rotate_axis = cross_prod(ucs.axis_z,huoqufacenorm[i]);
		if(i==0)	//第一面与第二面间的夹角
			angle[0] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
		else		//第一面与第三面间的夹角
			angle[1] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
			//计算当前火曲线方向
		Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);
			
		//将第一面向其余面旋转的轴线转到相对坐标系下来
		vector_trans(rotate_axis,ucs,FALSE);
		if(dot_prod(vec,rotate_axis)<0)	//当前火曲线与旋转轴线反向
		{							//火曲线方向应与旋转轴线方向一致
			tempt = HuoQuLine[i].startPt;
			HuoQuLine[i].startPt = HuoQuLine[i].endPt;
			HuoQuLine[i].endPt = tempt;
		}
	}

	//ls_pos = pLs->ucs.origin;
	//ls_pos += pLs->offset*ls_norm;
	int iLsFace=GetNormFaceI(ls_norm);
	if(iLsFace==1)		//第二面上的螺栓
	{
		coord_trans(ls_pos,ucs,FALSE);
		rotate_point_around_axis(ls_pos,-angle[0],HuoQuLine[0]);
		coord_trans(ls_pos,ucs,TRUE);
		project_point(ls_pos,ucs.origin,ucs.axis_z);
		coord_trans(ls_pos,ucs,FALSE);
		if(ls_pos_coord)
		{
			ls_pos_coord[0]=ls_pos.x;
			ls_pos_coord[1]=ls_pos.y;
			ls_pos_coord[2]=ls_pos.z;
		}
		return TRUE;
	}
	else if(iLsFace==2)	//第三面上的螺栓
	{
		coord_trans(ls_pos,ucs,FALSE);
		rotate_point_around_axis(ls_pos,-angle[1],HuoQuLine[1]);
		coord_trans(ls_pos,ucs,TRUE);
		project_point(ls_pos,ucs.origin,ucs.axis_z);
		coord_trans(ls_pos,ucs,FALSE);
		if(ls_pos_coord)
		{
			ls_pos_coord[0]=ls_pos.x;
			ls_pos_coord[1]=ls_pos.y;
			ls_pos_coord[2]=ls_pos.z;
		}
		return TRUE;
	}
	else				//第一面上的螺栓
	{
		project_point(ls_pos,ucs.origin,ucs.axis_z);
		coord_trans(ls_pos,ucs,FALSE);
		if(ls_pos_coord)
		{
			ls_pos_coord[0]=ls_pos.x;
			ls_pos_coord[1]=ls_pos.y;
			ls_pos_coord[2]=ls_pos.z;
		}
		return TRUE;
	}
}

//要求输入的螺栓位置为此连接板相对坐标系下的位置,此外还要求输入的面位置iFace012
//正确，否则返回值不正确或原位置值。正常情况下应返回全塔坐标系下的实际坐标位置
f3dPoint CLDSPlate::RestoreLsPos(f3dPoint ls_pos,int iFace012/*=0*/)
{
	double angle[2];
	f3dPoint rotate_axis,tempt,vec;

	for(int i=0;i<face_N-1;i++)
	{
		//计算第一面法线到其余面法线的旋转轴线
		rotate_axis = cross_prod(ucs.axis_z,huoqufacenorm[i]);
		if(i==0)	//第一面与第二面间的夹角
			angle[0] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
		else		//第一面与第三面间的夹角
			angle[1] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
			//计算当前火曲线方向
		Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);
			
		//将第一面向其余面旋转的轴线转到相对坐标系下来
		vector_trans(rotate_axis,ucs,FALSE);
		if(dot_prod(vec,rotate_axis)<0)	//当前火曲线与旋转轴线反向
		{							//火曲线方向应与旋转轴线方向一致
			tempt = HuoQuLine[i].startPt;
			HuoQuLine[i].startPt = HuoQuLine[i].endPt;
			HuoQuLine[i].endPt = tempt;
		}
	}
	if(iFace012==0)
	{
		coord_trans(ls_pos,ucs,TRUE);
		ls_pos+=ucs.axis_z*this->m_fNormOffset;
		return ls_pos;
	}
	else if(iFace012==1&&face_N>=2)
	{
		rotate_point_around_axis(ls_pos,angle[0],HuoQuLine[0]);
		coord_trans(ls_pos,ucs,TRUE);
		ls_pos+=ucs.axis_z*this->m_fNormOffset;	//必须考虑制弯板沿基面法向的偏移量影响 wjh-2019.9.10
		return ls_pos;
	}
	else if(iFace012==2&&face_N>2)
	{
		rotate_point_around_axis(ls_pos,angle[1],HuoQuLine[1]);
		coord_trans(ls_pos,ucs,TRUE);
		ls_pos+=ucs.axis_z*this->m_fNormOffset;
		return ls_pos;
	}
	else
		return ls_pos;
}

double CLDSPlate::GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine/*=NULL*/)
{
	if(face_N<=1||vertex_list.GetNodeNum()<3||iFace<1||iFace>face_N-1)	//单面板没有变形
		return 0;
	else
	{
		iFace++;	
		f3dLine huoquline;
		BOOL bFindHuoQuStart=FALSE,bFindHuoQuEnd=FALSE;
		if(face_N==2||IsFoldPlate())	//双面板或折叠板
		{
			BOOL bReverse=FALSE;
			for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(pVertex->vertex.feature>10&&pVertex->vertex.feature%10==iFace)
				{
					if(!bFindHuoQuStart)
					{
						huoquline.startPt=pVertex->vertex;
						bFindHuoQuStart=TRUE;
						continue;
					}
					else
					{
						huoquline.endPt=pVertex->vertex;
						bFindHuoQuEnd=TRUE;
						break;
					}
				}
				if(bFindHuoQuStart&&!bReverse&&(pVertex->vertex.feature==1||pVertex->vertex.feature/10==1))
					bReverse=TRUE;	//始终点插入了基本面顶点，说明始终端反了，否则始终顶点间只能是火曲面顶点
			}
			if(bFindHuoQuStart&&bFindHuoQuEnd)
			{
				if(bReverse)
				{	//始终端反向
					f3dPoint pt=huoquline.startPt;
					huoquline.startPt=huoquline.endPt;
					huoquline.endPt=pt;
				}
			}
		}
		else if(face_N==3)	//普通三面板
		{
			int prev_face_no=0,now_face_no=0,next_face_no=0;
			bool bNeedInverseHuoQuFace=false;	//三面板是否需要颠倒两火曲面（便于逆时针生成实体面）
			PROFILE_VERTEX *pVertex;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(!bNeedInverseHuoQuFace&&IsPeakPointPlate())
				{
					if(prev_face_no==0)
						prev_face_no=pVertex->vertex.feature;
					else if(now_face_no==0)
						now_face_no=pVertex->vertex.feature;
					else if(next_face_no==0)
						next_face_no=pVertex->vertex.feature;
					else
					{
						prev_face_no=now_face_no;
						now_face_no=next_face_no;
						next_face_no=pVertex->vertex.feature;
					}
					if((prev_face_no==3&&next_face_no==1)||(prev_face_no==1&&next_face_no==2))
					{
						bNeedInverseHuoQuFace=true;
						break;
					}
				}
			}
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(pVertex->vertex.feature>10&&pVertex->vertex.feature%10==iFace)
				{
					huoquline.startPt=pVertex->vertex;
					break;
				}
			}
			if(pVertex!=NULL)	//找到火曲线端点
			{
				bFindHuoQuStart=bFindHuoQuEnd=TRUE;
				if(iFace==2)
				{
					if(bNeedInverseHuoQuFace)
						huoquline.endPt=top_point;
					else
					{
						huoquline.endPt=huoquline.startPt;
						huoquline.startPt=top_point;	
					}
				}
				else if(iFace==3)
				{
					if(bNeedInverseHuoQuFace)
					{
						huoquline.endPt=huoquline.startPt;
						huoquline.startPt=top_point;	
					}
					else
						huoquline.endPt=top_point;
				}
				else
					bFindHuoQuStart=bFindHuoQuEnd=FALSE;
			}
		}
		if(!bFindHuoQuStart||!bFindHuoQuEnd)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar100 sError("Can't find the reasonable intersection points  between bending line and plate edge！");
#else 
			CXhChar100 sError("没找到钢板(0X%X)合理的火曲线与板边缘相交顶点！",handle);
#endif
			Log2File()->Log(sError);
			return 0;
		}
		f3dPoint huoqu_axis_vec=huoquline.endPt-huoquline.startPt;
		f3dPoint huoqu_norm=huoqufacenorm[iFace-2];
		if(huoqu_norm.IsZero())
		{
			Log2File()->Log("0x%X钢板的第%d(火曲)面法线为空，火曲角计算有误!",handle,iFace-1);
			return 0;
		}
		UCS_STRU huoquucs;
		huoquucs.axis_z=huoqu_axis_vec;
		normalize(huoquucs.axis_z);
		huoquucs.axis_x.Set(0,0,1);
		huoquucs.axis_y=huoquucs.axis_z^huoquucs.axis_x;
		vector_trans(huoqu_norm,ucs,FALSE);
		if(fabs(huoqu_norm*huoquucs.axis_z)>0.001)//EPS)//部分挂线板火曲面与火曲线误差偏大一点,如果太严格总报警示信息 wjh-2016.3.28
			Log2File()->Log("0x%X钢板的第%d(火曲)面法线方向与火曲线不垂直，火曲角计算有误!",handle,iFace-1);
		vector_trans(huoqu_norm,huoquucs,FALSE);
		double huoquangle=Cal2dLineAng(0,0,huoqu_norm.x,huoqu_norm.y);
		if(huoquangle>Pi)
			huoquangle-=2*Pi;
		if(pHuoQuLine)
			*pHuoQuLine=huoquline;
		return huoquangle;
	}
}

//调整钢板坐标系后计算火曲变形点坐标时使用 new_ucs为传入坐标所在的相对坐标系 wht 10-12-08
f3dPoint CLDSGeneralPlate::GetDeformedVertex(f3dPoint vertice,UCS_STRU new_ucs)
{
	if(GetFaceN()<=1||vertex_list.GetNodeNum()<3||vertice.feature==1||m_ciDeformType==0)	//单面板没有变形
		return vertice;
	else
	{
		coord_trans(vertice,new_ucs,TRUE);	//转换到绝对坐标系下
		coord_trans(vertice,ucs,FALSE);		//转换到钢板坐标系下
		vertice=GetDeformedVertex(vertice);	
		coord_trans(vertice,ucs,TRUE);		//转换到绝对坐标系下
		coord_trans(vertice,new_ucs,FALSE);	//转换到new_ucs坐标系下
		return vertice;
	}
}
//由内侧圆弧(凹面)半径r确定扇形区域后，的x系数层折线长度 wjh-2017.6.30
double GetBendPolyLayerLength(double huoquangle,double thick,double x,double r)
{
	double R=x*thick+r;
	double halfangle=fabs(huoquangle*0.5);
	double tanhalf=tan(halfangle);
	return tanhalf*R*2;
}
f3dPoint CLDSGeneralPlate::GetDeformedVertex(f3dPoint vertice)
{
	if(GetFaceN()<=1||vertex_list.GetNodeNum()<3||vertice.feature<=1||m_ciDeformType==0 )	//单面板没有变形
		return vertice;
	else
	{
		f3dLine huoquline;
		int iFace=vertice.feature%10;
		//为避免制弯线与制弯角度获取错误，统一都由GetHuoQuAngle来实现 wjh-2011.8.31
		double huoquangle=GetHuoQuAngle(iFace-1,&huoquline);
		f3dPoint huoqu_axis_vec=huoquline.endPt-huoquline.startPt;
		//查找火曲点时未区分始终端，需要在此处根据HuoQuLine方向进行修正 wht 11-07-21
		f3dPoint huoqu_norm=GetHuoquFaceNorm(iFace-2);
		UCS_STRU huoquucs;
		huoquucs.axis_z=huoqu_axis_vec;
		normalize(huoquucs.axis_z);
		huoquucs.axis_x.Set(0,0,1);
		huoquucs.axis_y=huoquucs.axis_z^huoquucs.axis_x;
		//根据用户选择的火曲变形方式，计算火曲补偿量,调整火曲面上的坐标
		double r=GetHuoQuR(iFace-2);
		double x=GetDeformCoef(iFace-2);
		double odd_len=0;
		if(x>=0.55)
		{	//正常中性层系数一定是<0.5,大于0.5(设0.1个精度误差)一定是出现其它原因考虑（此时不考虑x层的圆弧因素，而直接取折线)
			double actual_length=GetBendPolyLayerLength(huoquangle,Thick,x,r);
			if(huoquangle>0)	//内曲,底面是基准面
				odd_len=actual_length-GetBendPolyLayerLength(huoquangle,Thick,0,r);//*r*tan(huoquangle/2);
			else	//外曲,顶面是基准面
				odd_len=actual_length-GetBendPolyLayerLength(huoquangle,Thick,1,r);//*(r+Thick)*tan(-huoquangle/2);
		}
		else
		{
			if(huoquangle>0)	//内曲,底面是基准面
				odd_len=huoquangle*(r+x*Thick)-GetBendPolyLayerLength(huoquangle,Thick,0,r);//*r*tan(huoquangle/2);
			else	//外曲,顶面是基准面
				odd_len=-huoquangle*(r+x*Thick)-GetBendPolyLayerLength(huoquangle,Thick,1,r);//*(r+Thick)*tan(-huoquangle/2);
		}
		if(vertice.feature>10)	//火曲点
			vertice+=huoquucs.axis_y*(odd_len/2);
		else	//火曲面上的其它点
			vertice+=huoquucs.axis_y*odd_len;
		return vertice;
	}
}

//要求输入的顶点必须是此连接板上的顶点,否则
//可能不进行转换或转换错误(由展开相对坐标-->实际相对坐标)
f3dPoint CLDSPlate::GetRealVertex(f3dPoint vertice,bool bCalVertex/*=true*/)
{
	BYTE cbFlag=FROM_ACS|TO_ACS;
	if(bCalVertex)
		cbFlag|=IS_VERTEX;
	else
		cbFlag|=IS_VECTOR;
	f3dPoint real_vertice=TransToActual(vertice,vertice.feature,cbFlag);
	real_vertice.feature=vertice.feature;
	return real_vertice;
}
GEPOINT CLDSPlate::TransToActual(const double* coord3d,int iResidePlane/*=1*/,BYTE cbFlag/*=0*/)
{
	GEPOINT real_vertice(coord3d);
	//real_vertice = vertice;
	bool bCalVertex = (cbFlag&IS_VECTOR)==0;
	if(iResidePlane==2||iResidePlane==3)
	{
		int i;
		double angle[2];
		f3dPoint rotate_axis,tempt,vec;

		for(i=0;i<face_N-1;i++)
		{
			//计算第一面法线到其余面法线的旋转轴线
			rotate_axis = cross_prod(ucs.axis_z,huoqufacenorm[i]);
			if(i==0)	//第一面与第二面间的夹角
				angle[0] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
			else		//第一面与第三面间的夹角
				angle[1] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
				//计算当前火曲线方向
			Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);
				
			//将第一面向其余面旋转的轴线转到相对坐标系下来
			vector_trans(rotate_axis,ucs,FALSE);
			if(dot_prod(vec,rotate_axis)<0)	//当前火曲线与旋转轴线反向
			{							//火曲线方向应与旋转轴线方向一致
				tempt = HuoQuLine[i].startPt;
				HuoQuLine[i].startPt = HuoQuLine[i].endPt;
				HuoQuLine[i].endPt = tempt;
			}
		}
		if(iResidePlane==2)	//第二面上的顶点
		{
			if(bCalVertex)
				rotate_point_around_axis(real_vertice,angle[0],HuoQuLine[0].startPt,HuoQuLine[0].endPt);
			else 
				RotateVectorAroundVector(real_vertice,angle[0],HuoQuLine[0].endPt-HuoQuLine[0].startPt);
		}
		else	//第三面上的顶点
		{
			if(bCalVertex)
				rotate_point_around_axis(real_vertice,angle[1],HuoQuLine[1].startPt,HuoQuLine[1].endPt);
			else 
				RotateVectorAroundVector(real_vertice,angle[1],HuoQuLine[1].endPt-HuoQuLine[1].startPt);
		}
	}
	if(cbFlag&TO_MCS)
	{
		if(bCalVertex)
			coord_trans(real_vertice,ucs,TRUE,TRUE);
		else
			vector_trans(real_vertice,ucs,TRUE,TRUE);
	}

	return real_vertice;	
}

//要求输入的顶点或向量必须是此连接板上的顶点,否则
//可能不进行转换或转换错误(由实际相对坐标-->展开相对坐标)
//bSpreadVertex=true表示展开顶点false表示展开矢量方向
f3dPoint CLDSPlate::GetSpreadVertex(const double* coord3d,int face_i,bool bSpreadVertex/*=true*/)
{
	f3dPoint V(coord3d);
	if(face_i==2||face_i==3)
	{
		int i;
		double angle[2];
		f3dPoint rotate_axis,tempt,vec;

		for(i=0;i<face_N-1;i++)
		{
			//计算第一面法线到其余面法线的旋转轴线
			rotate_axis = cross_prod(ucs.axis_z,huoqufacenorm[i]);
			if(i==0)	//第一面与第二面间的夹角
				angle[0] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
			else		//第一面与第三面间的夹角
				angle[1] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
				//计算当前火曲线方向
			Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);
				
			//将第一面向其余面旋转的轴线转到相对坐标系下来
			vector_trans(rotate_axis,ucs,FALSE);
			if(dot_prod(vec,rotate_axis)<0)	//当前火曲线与旋转轴线反向
			{							//火曲线方向应与旋转轴线方向一致
				tempt = HuoQuLine[i].startPt;
				HuoQuLine[i].startPt = HuoQuLine[i].endPt;
				HuoQuLine[i].endPt = tempt;
			}
		}
		if(face_i==2||face_i==3)
		{
			if(bSpreadVertex)
				rotate_point_around_axis(V,-angle[face_i-2],HuoQuLine[face_i-2].startPt,HuoQuLine[face_i-2].endPt);
			else
			{
				f3dPoint rot_vec = HuoQuLine[face_i-2].endPt - HuoQuLine[face_i-2].startPt;
				RotateVectorAroundVector(V,-angle[face_i-2],rot_vec);
			}
		}
	}
	if(bSpreadVertex)
		V.feature=face_i;
	return V;	
}
//cbFlag由FROM_MCS、FROM_ACS、IS_VECTOR与IS_VERTEX进行组合，默认为FROM_MCS|IS_VERTEX wjh-2015.2.27
//iResidePlane取值:1、2、3表示待转换点或矢量所驻存的面号 wjh-2015.2.27
//static const BYTE FROM_MCS =0x00;	//coord3d为模型坐标
//static const BYTE FROM_ACS =0x01;	//coord3d为当前钢板下装配坐标(即相对坐标)
//static const BYTE IS_VERTEX=0x00;	//coord3d为位置点, 执行位置坐标转换
//static const BYTE IS_VECTOR=0x02;	//coord3d为方向量, 执行矢量坐标转换
f3dPoint CLDSPlate::SpreadToACS(const double* coord3d,int iResidePlane/*=1*/,BYTE cbFlag/*=0*/)
{
	f3dPoint V(coord3d);
	bool isVector=(cbFlag&IS_VECTOR)>0;
	bool isVertex=!isVector;
	bool fromAcs=(cbFlag&FROM_ACS)>0;
	bool fromMcs=!fromAcs;
	//统一转换到钢板装配坐标系下
	if(fromMcs&&isVertex)
		coord_trans(V,ucs,FALSE);
	else if(fromMcs&&isVector)
		vector_trans(V,ucs,FALSE);
	if(iResidePlane==2||iResidePlane==3)
	{
		int i;
		double angle[2];
		f3dPoint rotate_axis,tempt,vec;

		for(i=0;i<face_N-1;i++)
		{
			//计算第一面法线到其余面法线的旋转轴线
			rotate_axis = cross_prod(ucs.axis_z,huoqufacenorm[i]);
			if(i==0)	//第一面与第二面间的夹角
				angle[0] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
			else		//第一面与第三面间的夹角
				angle[1] = cal_angle_of_2vec(ucs.axis_z,huoqufacenorm[i]);
				//计算当前火曲线方向
			Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);
				
			//将第一面向其余面旋转的轴线转到相对坐标系下来
			vector_trans(rotate_axis,ucs,FALSE);
			if(dot_prod(vec,rotate_axis)<0)	//当前火曲线与旋转轴线反向
			{							//火曲线方向应与旋转轴线方向一致
				tempt = HuoQuLine[i].startPt;
				HuoQuLine[i].startPt = HuoQuLine[i].endPt;
				HuoQuLine[i].endPt = tempt;
			}
		}
		if(iResidePlane==2||iResidePlane==3)
		{
			if(isVertex)
				rotate_point_around_axis(V,-angle[iResidePlane-2],HuoQuLine[iResidePlane-2].startPt,HuoQuLine[iResidePlane-2].endPt);
			else
			{
				f3dPoint rot_vec = HuoQuLine[iResidePlane-2].endPt - HuoQuLine[iResidePlane-2].startPt;
				RotateVectorAroundVector(V,-angle[iResidePlane-2],rot_vec);
			}
		}
	}
	if(isVertex)
		V.feature=iResidePlane;
	return V;	
}

CLDSPart *CLDSPlate::GetMirPart(MIRMSG mirmsg,const double *pPickPos/*=NULL*/,BOOL bCheckProfile/*=TRUE*/)
{
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//先在关联构件列表中查找
		CLDSPlate *pPlate=NULL;
		if(pItem->mirInfo.IsSameMirMsg(mirmsg))
		{
			pPlate=(CLDSPlate*)BelongModel()->FromPartHandle(pItem->hObj,CLS_PLATE);
			if(pPlate&&ModuleTogetherWith(pPlate))
				return pPlate;
			else
				break;
		}
	}
	f3dPoint mir_origin=GetMirPos(ucs.origin,mirmsg);
	f3dPoint mir_axis_z=GetMirVector(ucs.axis_z,mirmsg);
	int cls_typeid=GetClassTypeId();
	PROFILE_VERTEX *pVertex=NULL;
	const int MAX_OFFSET_DIST_SQUARE=3600;
	double mir_offset_dist=0,offset_dist=0;
	f3dPoint vertice,axis_x1,axis_x2,mir_aixs_x1;
	if(vertex_list.GetNodeNum()>0)
	{
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			vertice=pVertex->vertex;
			coord_trans(vertice,ucs,TRUE);
			axis_x1+=vertice;
		}
		//根据板的重心点计算板的延伸方向
		axis_x1/=vertex_list.GetNodeNum();
		project_point(axis_x1,ucs.origin,ucs.axis_z);
		axis_x1-=ucs.origin;
		mir_offset_dist=axis_x1.mod2();
		normalize(axis_x1);
		//对称板的延伸方向
		mir_aixs_x1=GetMirVector(axis_x1,mirmsg);
	}
	BOOL pushed=m_pModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
	for(CLDSPart *pMirPart=m_pModel->EnumPartFirst(cls_typeid);pMirPart;pMirPart=m_pModel->EnumPartNext(cls_typeid))
	{
		CLDSPlate *pMirPlate=(CLDSPlate*)pMirPart;
		if(pMirPlate==this)
			continue;
		if(pMirPlate->GetThick()!=_thick||!ModuleTogetherWith(pMirPlate))
			continue;
		//对称标注构件编号时不需要保证钢板外形完全一致，仅需要保证位置一致即可 wht 11-07-22
		else if(bCheckProfile&&IsEqualProfile(pMirPlate,FALSE))	//0.外形相同 
			continue;
		//double dd=mir_axis_z*pMirPlate->ucs.axis_z;
		//同一个节点上可能有多块板，他们坐标原点相同，且在同一个平面上但并不是同一块板
		//保证板的重心所在延伸方向相同
		if(pMirPlate->vertex_list.GetNodeNum()>0)
		{
			axis_x2.Set();	//重新初始化
			for(pVertex=pMirPlate->vertex_list.GetFirst();pVertex;pVertex=pMirPlate->vertex_list.GetNext())
			{
				vertice=pVertex->vertex;
				coord_trans(vertice,pMirPlate->ucs,TRUE);
				axis_x2+=vertice;
			}
			//根据板的重心点计算板的延伸方向
			axis_x2/=pMirPlate->vertex_list.GetNodeNum();
			project_point(axis_x2,pMirPlate->ucs.origin,pMirPlate->ucs.axis_z);
			axis_x2-=pMirPlate->ucs.origin;
			offset_dist=axis_x2.mod2();
			normalize(axis_x2);
			if(mir_aixs_x1*axis_x2<EPS_COS2||fabs(mir_offset_dist-offset_dist)>MAX_OFFSET_DIST_SQUARE) //重心位置不同
				continue;
		}
		if(pMirPlate->ucs.origin==mir_origin&&fabs(mir_axis_z*pMirPlate->ucs.axis_z)>EPS_COS2)
		{	
			if((mir_axis_z*pMirPlate->ucs.axis_z>0&&pMirPlate->m_fNormOffset==m_fNormOffset)||
				(mir_axis_z*pMirPlate->ucs.axis_z<0&&pMirPlate->m_fNormOffset+pMirPlate->GetThick()==-m_fNormOffset))
			{
				m_pModel->PopPartStack(pushed);
				return pMirPart;
			}
		}
	}
	m_pModel->PopPartStack(pushed);
	return NULL;
}

//指定正反比，对比钢板起始边索引比较钢板外形是否相同	wht 10-01-26
int CLDSPlate::CompareProfile(CLDSPlate *pMirPlate,BOOL bReverseOrder,int base_i,BOOL bCompareLs,double epsilon,
							  ATOM_LIST<PROFILE_VERTEX> &src_vertex_list,ATOM_LIST<PROFILE_VERTEX> &mir_vertex_list)
{
	if(MotherPart()==pMirPlate->MotherPart())
		return 0;	//同一母构件不须比对,肯定相同
	struct ve_item{
		f3dPoint vertex;
		double len;
		double cosine_angle;
	};
	int i,n,vice_n;
	f3dLine std_line,vice_std_line;
	f3dPoint vec_cur,vec_next;
	UCS_STRU tm_ucs,tm_vice_ucs;
	n = src_vertex_list.GetNodeNum();
	vice_n = mir_vertex_list.GetNodeNum();
	if(n!=vice_n||n<3||vice_n<3)
		return 1;	//外形不合理或不同
	struct ve_item* item_arr = new struct ve_item[n];
	struct ve_item* vice_item_arr = new struct ve_item[vice_n];
	for(i=0;i<n;i++)
	{
		item_arr[i].vertex = src_vertex_list[i].vertex;
		vec_cur = src_vertex_list[(i+1)%n].vertex-item_arr[i].vertex;
		vec_next= src_vertex_list[(i+2)%n].vertex-src_vertex_list[(i+1)%n].vertex;
		item_arr[i].len = vec_cur.mod();
		item_arr[i].cosine_angle = vec_cur.normalized()*vec_next.normalized();
	}
	for(i=0;i<vice_n;i++)
	{
		if(bReverseOrder)
		{	//反面比
			vice_item_arr[i].vertex = mir_vertex_list[vice_n-1-i].vertex;
			vec_cur = mir_vertex_list[(n+vice_n-i-2)%n].vertex-vice_item_arr[i].vertex;
			vec_next= mir_vertex_list[(n+vice_n-i-3)%n].vertex-mir_vertex_list[(n+vice_n-i-2)%n].vertex;
			vice_item_arr[i].len = vec_cur.mod();
			vice_item_arr[i].cosine_angle = vec_cur.normalized()*vec_next.normalized();
		}
		else
		{	//正面比
			vice_item_arr[i].vertex = mir_vertex_list[i].vertex;
			vec_cur = mir_vertex_list[(i+1)%n].vertex-vice_item_arr[i].vertex;
			vec_next= mir_vertex_list[(i+2)%n].vertex-mir_vertex_list[(i+1)%n].vertex;
			vice_item_arr[i].len = vec_cur.mod();
			vice_item_arr[i].cosine_angle = vec_cur.normalized()*vec_next.normalized();
		}
	}
	for(i=0;i<n;i++)
	{
		if( fabs(item_arr[i].len-vice_item_arr[(base_i+i)%n].len)>epsilon
		 || fabs(item_arr[i].cosine_angle-vice_item_arr[(base_i+i)%n].cosine_angle)>epsilon*1e-2)
		{
			delete []item_arr;
			delete []vice_item_arr;
			return 1;	//外形不合理或不同
		}
	}
	int iFaceNoS,iFaceNoE,std_face,vice_std_face;
	std_line.startPt = src_vertex_list[0].vertex;
	std_line.endPt   = src_vertex_list[1].vertex;
	iFaceNoS=max(std_line.startPt.feature/10,std_line.startPt.feature%10);
	iFaceNoE=max(std_line.endPt.feature/10,std_line.endPt.feature%10);
	std_face=min(iFaceNoS,iFaceNoE);
	//
	vice_std_line.startPt = vice_item_arr[base_i].vertex;
	vice_std_line.endPt   = vice_item_arr[(base_i+1)%n].vertex;
	iFaceNoS=max(vice_std_line.startPt.feature/10,vice_std_line.startPt.feature%10);
	iFaceNoE=max(vice_std_line.endPt.feature/10,vice_std_line.endPt.feature%10);
	vice_std_face=min(iFaceNoS,iFaceNoE);
	if(face_N>1)
	{
		//比较火曲方向
		tm_ucs.axis_z=ucs.axis_z;
		tm_ucs.axis_x=std_line.endPt-std_line.startPt;
		vector_trans(tm_ucs.axis_x,ucs,TRUE);//coord_trans-->vector_trans wjh-2004.09.18
		normalize(tm_ucs.axis_x);
		tm_ucs.axis_y=tm_ucs.axis_z^tm_ucs.axis_x;
		tm_ucs.axis_x=tm_ucs.axis_y^tm_ucs.axis_z;
		
		if(bReverseOrder)	//反面比
			tm_vice_ucs.axis_z=-pMirPlate->ucs.axis_z;
		else				//正面比
			tm_vice_ucs.axis_z=pMirPlate->ucs.axis_z;
		tm_vice_ucs.axis_x=vice_std_line.endPt-vice_std_line.startPt;
		vector_trans(tm_vice_ucs.axis_x,pMirPlate->ucs,TRUE);//coord_trans-->vector_trans wjh-2004.09.18
		tm_vice_ucs.axis_y=tm_vice_ucs.axis_z^tm_vice_ucs.axis_x;
		tm_vice_ucs.axis_x=tm_vice_ucs.axis_y^tm_vice_ucs.axis_z;
		normalize(tm_vice_ucs.axis_x);
		normalize(tm_vice_ucs.axis_y);
		//根据是否翻转，及比对的基准边所在面调整火曲面法向	wxc-2017.12.5
		f3dPoint huoqu_norm1=huoqufacenorm[0]*(bReverseOrder?-1:1);
		f3dPoint huoqu_norm2=huoqufacenorm[1]*(bReverseOrder?-1:1);
		f3dPoint vice_huoqu_norm1=pMirPlate->huoqufacenorm[0];
		f3dPoint vice_huoqu_norm2=pMirPlate->huoqufacenorm[1];
		if(std_face>1)
		{	//比对的基准轮廓边在火曲面
			huoqu_norm1*=-1;
			huoqu_norm2*=-1;
		}
		if(vice_std_face>1)
		{	//比对的基准轮廓边在火曲面
			vice_huoqu_norm1*=-1;
			vice_huoqu_norm2*=-1;
		}
		vector_trans(huoqu_norm1,tm_ucs,FALSE);
		vector_trans(huoqu_norm2,tm_ucs,FALSE);
		vector_trans(vice_huoqu_norm1,tm_vice_ucs,FALSE);
		vector_trans(vice_huoqu_norm2,tm_vice_ucs,FALSE);
		if(huoqu_norm1*vice_huoqu_norm1<0.9999)
		{	//火曲度偏差不可超过1°cos(1°)=0.9998476951
			if(face_N==2)
			{
				delete []item_arr;
				delete []vice_item_arr;
				return 1;	//正面比火曲角度不同
			}
			else
			{
				if(huoqu_norm1*vice_huoqu_norm2<0.99)
				{
					delete []item_arr;
					delete []vice_item_arr;
					return 1;	//正面比火曲角度不同
				}
				if(huoqu_norm2*vice_huoqu_norm1<0.99)
				{
					delete []item_arr;
					delete []vice_item_arr;
					return 1;	//正面比火曲角度不同
				}
			}
		}
		else if(face_N==3)
		{
			if(huoqu_norm2*vice_huoqu_norm2<0.99)
			{
				delete []item_arr;
				delete []vice_item_arr;
				return 1;	//正面比火曲角度不同
			}
		}
	}
	if(bCompareLs)	//对照螺栓孔位置及大小
	{
		if(GetLsCount()!=pMirPlate->GetLsCount())
		{
			delete []item_arr;
			delete []vice_item_arr;
			return 2;
		}
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			BOOL bFindEqualLs = FALSE;
			f3dPoint pos;
			SpreadLsPos(pLsRef,pos);
			//coord_trans(pos,ucs,FALSE);
			pos.z = 0;
			double dist_start = DISTANCE(pos,std_line.startPt);
			double dist_end   = DISTANCE(pos,std_line.endPt);
			for(CLsRef *pViceLsRef=pMirPlate->GetFirstLsRef();pViceLsRef;pViceLsRef=pMirPlate->GetNextLsRef())
			{
				//pos = (*pViceLsRef)->ucs.origin;
				pMirPlate->SpreadLsPos(pViceLsRef,pos);
				//coord_trans(pos,pMirPlate->ucs,FALSE);
				pos.z = 0;
				double vice_dist_start = DISTANCE(pos,vice_std_line.startPt);
				double vice_dist_end   = DISTANCE(pos,vice_std_line.endPt);
				if( (*pLsRef)->get_d()+(*pLsRef)->hole_d_increment!=
					(*pViceLsRef)->get_d()+(*pViceLsRef)->hole_d_increment)
					continue;
				else if(fabs(dist_start-vice_dist_start)>epsilon||fabs(dist_end-vice_dist_end)>epsilon)
					continue;
				else
				{
					bFindEqualLs = TRUE;
					break;
				}
			}
			if(!bFindEqualLs)	//螺栓孔不相同
			{
				delete []item_arr;
				delete []vice_item_arr;
				return 2;	
			}
		}
		delete []item_arr;
		delete []vice_item_arr;
		return 0;	//两板相同
	}
	else	//不需对比螺栓孔
	{
		delete []item_arr;
		delete []vice_item_arr;
		return 0;
	}
}
void RemoveOverlapProfileVertex(ATOM_LIST<PROFILE_VERTEX> &vertex_list)
{
	const double DIST_EPS=0.5;
	PROFILE_VERTEX *pPrevVertex=vertex_list.GetTail();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(DISTANCE(pPrevVertex->vertex,pVertex->vertex)<DIST_EPS)
		{
			if(pPrevVertex->vertex.feature>10)
			{	//前一轮廓点为火曲点，删除当前轮廓点
				vertex_list.DeleteCursor();
			}
			else if(pVertex->vertex.feature>10)
			{	//当前轮廓点为火曲点，删除前一个轮廓点
				vertex_list.push_stack();
				if(vertex_list.GetPrev()==NULL)
					vertex_list.GetTail();
				vertex_list.DeleteCursor();
				vertex_list.pop_stack();
			}
			else 
			{	//取轮廓点中点为新轮廓点坐标
				f3dPoint vertex=(pPrevVertex->vertex+pVertex->vertex)*0.5;
				*pPrevVertex=*pVertex;
				pPrevVertex->vertex=vertex;
				vertex_list.DeleteCursor();
			}
		}
		pPrevVertex=pVertex;
	}
}
//0.钢板外形相同 1.外形不同 2.螺栓信息不同 3.线属性卷边方向不同或者卷边高度不同
int CLDSPlate::IsEqualProfile(CLDSGeneralPlate* pMirPlate,BOOL bCompareLs/*=TRUE*/,double epsilon/*=1.0*/)
{
	if(GetClassTypeId()!=pMirPlate->GetClassTypeId())
		return 1;	//暂时要求两钢板类型必须一致 wht 15-12-22//比较钢板外形之前，去除重复轮廓点
	PROFILE_VERTEX *pVertex=NULL;
	ATOM_LIST<PROFILE_VERTEX> src_vertex_list,mir_vertex_list;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		src_vertex_list.append(*pVertex);
	RemoveOverlapProfileVertex(src_vertex_list);
	for(pVertex=pMirPlate->vertex_list.GetFirst();pVertex;pVertex=pMirPlate->vertex_list.GetNext())
		mir_vertex_list.append(*pVertex);
	RemoveOverlapProfileVertex(mir_vertex_list);
	int i=0,retCode=-1;
	int base_i=0;	//记录外形相同的基准边索引
	int n=src_vertex_list.GetNodeNum();
	int vice_n=mir_vertex_list.GetNodeNum();
	if(n!=vice_n)
		return 1;	//钢板轮廓点数目不同
	typedef PROFILE_VERTEX*	VERTEX_PTR;
	VERTEX_PTR* vertex_arr = new VERTEX_PTR[n];
	VERTEX_PTR* vice_vertex_arr = new VERTEX_PTR[vice_n];
	i=0;
	for(pVertex=src_vertex_list.GetFirst();pVertex;pVertex=src_vertex_list.GetNext())
	{
		vertex_arr[i]=pVertex;
		i++;
	}
	i=0;
	for(pVertex=mir_vertex_list.GetFirst();pVertex;pVertex=mir_vertex_list.GetNext())
	{	
		vice_vertex_arr[i]=pVertex;
		i++;
	}
	//正面比钢板外形
	for(i=0;i<n;i++)
	{
		retCode=CompareProfile((CLDSPlate*)pMirPlate,FALSE,i,bCompareLs,epsilon,src_vertex_list,mir_vertex_list);
		if(retCode==0)
		{	
			base_i=i;
			break;
		}
	}
	//以下代码用来判断 正比钢板外形相同的情况下线属性进行的卷边是否相同 wht 10-01-27
	if(retCode==0)
	{	//正面比外形相同,单独比较卷边是否相同
		for(i=0;i<n;i++)
		{
			VERTEX_PTR pCurVertex=NULL,pViceVertex=NULL;
			pCurVertex=vertex_arr[i];
			pViceVertex=vice_vertex_arr[(base_i+i)%n];
			if(pCurVertex&&pViceVertex)
			{	
				//两边卷边属性不同且卷边的钢板卷边高度绝对值大于0
				if(pCurVertex->m_bRollEdge!=pViceVertex->m_bRollEdge&&
					((pCurVertex->m_bRollEdge&&abs((int)pCurVertex->manu_space)>0)
					||(pViceVertex->m_bRollEdge&&abs((int)pViceVertex->manu_space)>0)))
				{
					retCode=3;	//卷边属性不相同
					break;
				}
				//反比钢板时如果卷边数值正负号相反说明卷边方向相反
				else if(pCurVertex->m_bRollEdge&&ftol(pCurVertex->manu_space)*ftol(pViceVertex->manu_space)<0)
					retCode=3;	//卷边方向不同
				else if(pCurVertex->m_bRollEdge&&abs(abs((int)pCurVertex->manu_space)-abs((int)pViceVertex->manu_space))>EPS_COS2)
					retCode=3;	//卷边高度不同
				if(retCode==3)
					break;
			}
		}
		delete []vertex_arr;
		delete []vice_vertex_arr;
		return retCode;
	}
	//反面比
	for(i=0;i<n;i++)
	{
		retCode=CompareProfile((CLDSPlate*)pMirPlate,TRUE,i,bCompareLs,epsilon,src_vertex_list,mir_vertex_list);
		if(retCode==0)
		{
			base_i=i;
			break;
		}
	}
	//以下代码用来判断 反比钢板外形相同的情况下线属性进行的卷边是否相同 wht 10-01-27
	if(retCode==0)
	{	//反面比外形相同，单独比较卷边是否相同
		for(i=0;i<vice_n;i++)
			vice_vertex_arr[i]=&mir_vertex_list[vice_n-1-i];
		for(i=0;i<vice_n;i++)
		{
			VERTEX_PTR pCurVertex=NULL,pViceVertex=NULL;
			pCurVertex=vertex_arr[i];
			//当前为反比钢板卷边属性存储在下一个轮廓点上 wht 10-01-27
			pViceVertex=vice_vertex_arr[(base_i+i+1)%n];
			if(pCurVertex&&pViceVertex)
			{	
				//两边卷边属性不同且卷边的钢板卷边高度绝对值大于0
				if(pCurVertex->m_bRollEdge!=pViceVertex->m_bRollEdge&&
					((pCurVertex->m_bRollEdge&&abs((int)pCurVertex->manu_space)>0)
					||(pViceVertex->m_bRollEdge&&abs((int)pViceVertex->manu_space)>0)))
				{
					retCode=3;	//卷边属性不相同
					break;
				}
				//反比钢板时如果卷边数值正负号相同说明卷边方向相反
				else if(pCurVertex->m_bRollEdge&&ftol(pCurVertex->manu_space)*ftol(pViceVertex->manu_space)>0)
					retCode=3;	//卷边方向不同
				else if(pCurVertex->m_bRollEdge&&abs(abs((int)pCurVertex->manu_space)-abs((int)pViceVertex->manu_space))>EPS_COS2)
					retCode=3;	//卷边高度不同
				if(retCode==3)
					break;
			}
		}
		delete []vertex_arr;
		delete []vice_vertex_arr;
		return retCode;
	}
	delete []vertex_arr;
	delete []vice_vertex_arr;
	return retCode;
}

//旧的比较钢板外形函数,未处理量钢板有多条边对应相对，或者正反比外形均相同的情况,未处理线属性卷边钢板
//int CLDSPlate::IsEqualProfile(CLDSPlate* pMirPlate,BOOL bCompareLs/*=TRUE*/,double epsilon/*=1.0*/)
/*{
	struct ve_item{
		f3dPoint vertex;
		double len;
		double cosine_angle;
	};
	int i,n,vice_n;
	f3dLine std_line,vice_std_line;
	f3dPoint vec_cur,vec_next;
	UCS_STRU tm_ucs,tm_vice_ucs;
	n = vertex_list.GetNodeNum();
	vice_n = pMirPlate->vertex_list.GetNodeNum();
	if(n!=vice_n||n<3||vice_n<3)
		return 1;	//外形不合理或不同
	struct ve_item* item_arr = new struct ve_item[n];
	struct ve_item* vice_item_arr = new struct ve_item[vice_n];
	for(i=0;i<n;i++)
	{
		item_arr[i].vertex = vertex_list[i].vertex;
		vec_cur = vertex_list[(i+1)%n].vertex-item_arr[i].vertex;
		vec_next= vertex_list[(i+2)%n].vertex-vertex_list[(i+1)%n].vertex;
		item_arr[i].len = vec_cur.mod();
		item_arr[i].cosine_angle = vec_cur.normalized()*vec_next.normalized();
	}
	for(i=0;i<vice_n;i++)
	{
		vice_item_arr[i].vertex = pMirPlate->vertex_list[i].vertex;
		vec_cur = pMirPlate->vertex_list[(i+1)%n].vertex-vice_item_arr[i].vertex;
		vec_next= pMirPlate->vertex_list[(i+2)%n].vertex-pMirPlate->vertex_list[(i+1)%n].vertex;
		vice_item_arr[i].len = vec_cur.mod();
		vice_item_arr[i].cosine_angle = vec_cur.normalized()*vec_next.normalized();
	}
	int base_i=0;
	for(i=0;i<n;i++)
	{
		if( fabs(item_arr[i].len-vice_item_arr[(base_i+i)%n].len)>epsilon
		 || fabs(item_arr[i].cosine_angle-vice_item_arr[(base_i+i)%n].cosine_angle)>epsilon*1e-2)
		{
			base_i++;
			if(base_i>=n)
				goto REVERSE_ORDER;
			i=-1;
			continue;
		}
	}
	std_line.startPt = vertex_list[0].vertex;
	std_line.endPt   = vertex_list[1].vertex;
	vice_std_line.startPt = vice_item_arr[base_i].vertex;
	vice_std_line.endPt   = vice_item_arr[(base_i+1)%n].vertex;
	if(face_N>1)
	{
		//比较火曲方向
		tm_ucs.axis_z=ucs.axis_z;
		tm_ucs.axis_x=std_line.endPt-std_line.startPt;
		vector_trans(tm_ucs.axis_x,ucs,TRUE);//coord_trans-->vector_trans wjh-2004.09.18
		normalize(tm_ucs.axis_x);
		tm_ucs.axis_y=tm_ucs.axis_z^tm_ucs.axis_x;
		tm_ucs.axis_x=tm_ucs.axis_y^tm_ucs.axis_z;
		
		tm_vice_ucs.axis_z=pMirPlate->ucs.axis_z;
		tm_vice_ucs.axis_x=vice_std_line.endPt-vice_std_line.startPt;
		vector_trans(tm_vice_ucs.axis_x,pMirPlate->ucs,TRUE);//coord_trans-->vector_trans wjh-2004.09.18
		tm_vice_ucs.axis_y=tm_vice_ucs.axis_z^tm_vice_ucs.axis_x;
		tm_vice_ucs.axis_x=tm_vice_ucs.axis_y^tm_vice_ucs.axis_z;
		normalize(tm_vice_ucs.axis_x);
		normalize(tm_vice_ucs.axis_y);
		f3dPoint tm_norm,tm_vice_norm;
		tm_norm=huoqufacenorm[0];
		tm_vice_norm=pMirPlate->huoqufacenorm[0];
		vector_trans(tm_norm,tm_ucs,FALSE);
		vector_trans(tm_vice_norm,tm_vice_ucs,FALSE);
		if(DISTANCE(tm_norm,tm_vice_norm)>EPS2)
		{
			if(face_N==2)
				goto REVERSE_ORDER;	//正面比火曲角度不同
			else
			{
				tm_norm=huoqufacenorm[0];
				tm_vice_norm=pMirPlate->huoqufacenorm[1];
				vector_trans(tm_norm,tm_ucs,FALSE);
				vector_trans(tm_vice_norm,tm_vice_ucs,FALSE);
				if(DISTANCE(tm_norm,tm_vice_norm)>EPS2)
					goto REVERSE_ORDER;
				tm_norm=huoqufacenorm[1];
				tm_vice_norm=pMirPlate->huoqufacenorm[0];
				vector_trans(tm_norm,tm_ucs,FALSE);
				vector_trans(tm_vice_norm,tm_vice_ucs,FALSE);
				if(DISTANCE(tm_norm,tm_vice_norm)>EPS2)
					goto REVERSE_ORDER;
			}
		}
		else if(face_N==3)
		{
			tm_norm=huoqufacenorm[1];
			tm_vice_norm=pMirPlate->huoqufacenorm[1];
			vector_trans(tm_norm,tm_ucs,FALSE);
			vector_trans(tm_vice_norm,tm_vice_ucs,FALSE);
			if(DISTANCE(tm_norm,tm_vice_norm)>EPS2)
				goto REVERSE_ORDER;	//正面比火曲角度不同
		}
	}
	if(bCompareLs)	//对照螺栓孔位置及大小
	{
		if(GetLsCount()!=pMirPlate->GetLsCount())
		{
			delete []item_arr;
			delete []vice_item_arr;
			return 2;
		}
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			BOOL bFindEqualLs = FALSE;
			f3dPoint pos;
			SpreadLsPos(pLsRef,pos);
			//coord_trans(pos,ucs,FALSE);
			pos.z = 0;
			double dist_start = DISTANCE(pos,std_line.startPt);
			double dist_end   = DISTANCE(pos,std_line.endPt);
			for(CLsRef *pViceLsRef=pMirPlate->GetFirstLsRef();pViceLsRef;pViceLsRef=pMirPlate->GetNextLsRef())
			{
				//pos = (*pViceLsRef)->ucs.origin;
				pMirPlate->SpreadLsPos(pViceLsRef,pos);
				//coord_trans(pos,pMirPlate->ucs,FALSE);
				pos.z = 0;
				double vice_dist_start = DISTANCE(pos,vice_std_line.startPt);
				double vice_dist_end   = DISTANCE(pos,vice_std_line.endPt);
				if( (*pLsRef)->get_d()+(*pLsRef)->hole_d_increment!=
					(*pViceLsRef)->get_d()+(*pViceLsRef)->hole_d_increment)
					continue;
				else if(fabs(dist_start-vice_dist_start)>epsilon||fabs(dist_end-vice_dist_end)>epsilon)
					continue;
				else
				{
					bFindEqualLs = TRUE;
					break;
				}
			}
			if(!bFindEqualLs)	//螺栓孔不相同
				goto REVERSE_ORDER;
		}
		delete []item_arr;
		delete []vice_item_arr;
		return 0;	//两板相同
	}
	else	//不需对比螺栓孔
	{
		delete []item_arr;
		delete []vice_item_arr;
		return 0;
	}
REVERSE_ORDER:
	for(i=0;i<vice_n;i++)
	{
		vice_item_arr[i].vertex = pMirPlate->vertex_list[vice_n-1-i].vertex;
		vec_cur = pMirPlate->vertex_list[(n+vice_n-i-2)%n].vertex-vice_item_arr[i].vertex;
		vec_next= pMirPlate->vertex_list[(n+vice_n-i-3)%n].vertex-pMirPlate->vertex_list[(n+vice_n-i-2)%n].vertex;
		vice_item_arr[i].len = vec_cur.mod();
		vice_item_arr[i].cosine_angle = vec_cur.normalized()*vec_next.normalized();
	}
	base_i=0;
	for(i=0;i<n;i++)
	{
		if( fabs(item_arr[i].len-vice_item_arr[(base_i+i)%n].len)>epsilon
		 || fabs(item_arr[i].cosine_angle-vice_item_arr[(base_i+i)%n].cosine_angle)>epsilon*1e-2)
		{
			base_i++;
			if(base_i>=n)
			{	//外形不相同
				delete []item_arr;
				delete []vice_item_arr;
				return 1;
			}
			i=-1;
			continue;
		}
	}
	std_line.startPt = vertex_list[0].vertex;
	std_line.endPt   = vertex_list[1].vertex;
	vice_std_line.startPt = vice_item_arr[base_i].vertex;
	vice_std_line.endPt   = vice_item_arr[(base_i+1)%n].vertex;
	if(face_N>1)
	{
		//比较火曲方向
		tm_ucs.axis_z=ucs.axis_z;
		tm_ucs.axis_x=std_line.endPt-std_line.startPt;
		vector_trans(tm_ucs.axis_x,ucs,TRUE);
		normalize(tm_ucs.axis_x);
		tm_ucs.axis_y=tm_ucs.axis_z^tm_ucs.axis_x;
		tm_ucs.axis_x=tm_ucs.axis_y^tm_ucs.axis_z;
		
		tm_vice_ucs.axis_z=-pMirPlate->ucs.axis_z;
		tm_vice_ucs.axis_x=vice_std_line.endPt-vice_std_line.startPt;
		vector_trans(tm_vice_ucs.axis_x,pMirPlate->ucs,TRUE);
		tm_vice_ucs.axis_y=tm_vice_ucs.axis_z^tm_vice_ucs.axis_x;
		tm_vice_ucs.axis_x=tm_vice_ucs.axis_y^tm_vice_ucs.axis_z;

		f3dPoint tm_norm,tm_vice_norm;
		tm_norm=huoqufacenorm[0];
		tm_vice_norm=pMirPlate->huoqufacenorm[0];
		vector_trans(tm_norm,tm_ucs,FALSE);
		vector_trans(tm_vice_norm,tm_vice_ucs,FALSE);
		if(DISTANCE(tm_norm,tm_vice_norm)>EPS2)
		{
			delete []item_arr;
			delete []vice_item_arr;
			return 1;	//火曲角度为同
		}
		if(face_N==3)
		{
			tm_norm=huoqufacenorm[1];
			tm_vice_norm=pMirPlate->huoqufacenorm[1];
			vector_trans(tm_norm,tm_ucs,FALSE);
			vector_trans(tm_vice_norm,tm_vice_ucs,FALSE);
			if(DISTANCE(tm_norm,tm_vice_norm)>EPS2)
			{
				delete []item_arr;
				delete []vice_item_arr;
				return 1;	//火曲角度为同
			}
		}
	}
	if(bCompareLs)	//对照螺栓孔位置及大小
	{
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			BOOL bFindEqualLs = FALSE;
			f3dPoint pos;
			SpreadLsPos(pLsRef,pos);
			pos.z = 0;
			double dist_start = DISTANCE(pos,std_line.startPt);
			double dist_end   = DISTANCE(pos,std_line.endPt);
			for(CLsRef *pViceLsRef=pMirPlate->GetFirstLsRef();pViceLsRef;pViceLsRef=pMirPlate->GetNextLsRef())
			{
				pMirPlate->SpreadLsPos(pViceLsRef,pos);
				pos.z = 0;
				double vice_dist_start = DISTANCE(pos,vice_std_line.startPt);
				double vice_dist_end   = DISTANCE(pos,vice_std_line.endPt);
				if( (*pLsRef)->get_d()+(*pLsRef)->hole_d_increment!=
					(*pViceLsRef)->get_d()+(*pViceLsRef)->hole_d_increment)
					continue;
				else if(fabs(dist_start-vice_dist_start)>epsilon||fabs(dist_end-vice_dist_end)>epsilon)
					continue;
				else
				{
					bFindEqualLs = TRUE;
					break;
				}
			}
			if(!bFindEqualLs)	//螺栓孔不相同
			{
				delete []item_arr;
				delete []vice_item_arr;
				return 2;
			}
		}
		delete []item_arr;
		delete []vice_item_arr;
		return 0;
	}
	else	//不需对比螺栓孔
	{
		delete []item_arr;
		delete []vice_item_arr;
		return 0;
	}
}*/

bool CLDSPlate::IsEqual(CLDSPlate *pPlank,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	f3dPoint vertex,vertex_next,vice_vertex,vice_vertex_next;
	if(this==pPlank)
		return true;
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHK_PLATE_BASICINFO)
	{
		bool bRet=true;
		if(_thick!=pPlank->GetThick())
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Plate's thickness is different",(char*)sPartNo);
#else 
				sprintf(error_info,"{%s}板厚不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		if(cMaterial!=pPlank->cMaterial)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"{%s}Material is different",(char*)sPartNo);
#else 
				sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		if(face_N!=pPlank->face_N)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The plane number of bending plane{%s} is different",(char*)sPartNo);
#else 
				sprintf(error_info,"{%s}火曲面数不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		if(bRet==false)
		{
			if(pErrorInfo)
				*pErrorInfo=CHK_PLATE_BASICINFO;
			return false;
		}
	}
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHK_PLATE_PROFILEANDBOLTINFO)
	{
		bool bRet=true;
		int retCode=IsEqualProfile(pPlank,TRUE,epsilon);
		if(retCode==1)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The positive and negative profile of part{%s} is different",(char*)sPartNo);
#else 
				sprintf(error_info,"{%s}正反面外形不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(retCode==2)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The information of {%s}'s bolt is different",(char*)sPartNo);
#else 
				sprintf(error_info,"{%s}螺栓信息不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		else if(retCode==3)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The cur edge direction or edge height of {%s} is different",(char*)sPartNo);
#else 
				sprintf(error_info,"{%s}卷边方向或卷边高度不同",(char*)sPartNo);
#endif
			bRet=false;
		}
		if(bRet==false)
		{
			if(pErrorInfo)
				*pErrorInfo=CHK_PLATE_PROFILEANDBOLTINFO;
			return false;
		}
	}
	return true;
}

int CLDSPlate::ValidCheck()
{
	if(!AssertValid())
		return -1;	//板的基准节点句柄为空,为不合法板,请清除!
	if(!designInfo.m_bEnableFlexibleDesign)
		return 0;
	if(!designInfo.origin.AssertValid(BelongModel()))
		return -2;	//柔性设计基准点参数不合法
	else if(!designInfo.norm.AssertValid(BelongModel()))
		return -3;	//柔性设计基准面法线参数不合法
	for(CDesignLjPartPara *pLinePartPara=designInfo.partList.GetFirst();pLinePartPara;pLinePartPara=designInfo.partList.GetNext())
	{
		if(BelongModel()->FromPartHandle(pLinePartPara->hPart,CLS_LINEPART)==NULL)
			return -4;	//部分连接角钢柔性设计参数不合法
	}
	return 0;
}
void CLDSPlate::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
		(doc_type==1&&version>=4020005)||	//TMA(V4.2.0.5)
		(doc_type==2&&version>=1020005)||	//LMA(V1.2.0.5)
		(doc_type==3&&version>=2000005)||	//TSA(V2.0.0.5)
		(doc_type==4&&version>=1020005)||	//LDS(V1.2.0.5)
		(doc_type==5&&version>=1020005))	//TDA(V1.2.0.5)
	{
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		buffer.ReadDword(&dwPermission);
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&_hPartWeldParent);
		buffer.ReadString(layer_tag,4);
		buffer.ReadString(sNotes,51);
		if(version==0||
			(doc_type==1&&version>=5000010)||
			(doc_type==2&&version>=2000005)||
			(doc_type==4&&version>=1030005)||
			(doc_type==5&&version>=1030005))
		{
			buffer.ReadBooleanThin(&_bSyncShadowPartNo);
			if(!_bSyncShadowPartNo)
			{
				buffer.ReadInteger(&_iSeg.iSeg);
				buffer.ReadString(sPartNo,16);
			}
		}
		buffer.ReadDouble(&m_fNormOffset);
		bool bHasInnerCircle;
		buffer.ReadBooleanThin(&bHasInnerCircle);
		if(bHasInnerCircle)
		{
			buffer.ReadPoint(m_tInnerColumnNorm);
			buffer.ReadPoint(m_tInnerOrigin);
		}
		buffer.ReadString(sPartNo);
		buffer.ReadPoint(ucs.axis_x);
		buffer.ReadPoint(ucs.axis_y);
		buffer.ReadPoint(ucs.axis_z);
		buffer.ReadPoint(ucs.origin);
		int n=0;
		/*buffer.ReadInteger(&n);
		vertex_list.Empty();
		for(int ii=0;ii<n;ii++)
		{
			f3dPoint vertex;
			buffer.ReadPoint(vertex);
			PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertex));
			buffer.ReadInteger(&pVertex->vertex.feature);
			buffer.ReadInteger(&pVertex->vertex.ID);
			buffer.ReadByte(&pVertex->type);
			buffer.ReadBooleanThin(&pVertex->m_bWeldEdge);
			buffer.ReadBooleanThin(&pVertex->m_bRollEdge);
			if(pVertex->m_bRollEdge)	
				buffer.ReadWord(&pVertex->roll_edge_offset_dist);	
			if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
				buffer.ReadDouble(&pVertex->radius);	
			if(pVertex->type==1)	
			{
				buffer.ReadDouble(&pVertex->radius);
				buffer.ReadDouble(&pVertex->sector_angle);
			}
			buffer.ReadWord(&pVertex->manu_space);
			if(pVertex->type==3)	//椭圆弧	
				buffer.ReadPoint(pVertex->column_norm);
			buffer.ReadInteger(&pVertex->local_point_vec);
			buffer.ReadDouble(&pVertex->local_point_y);
		}*/
		buffer.ReadInteger(&n);
		for(int j=0;j<n;j++)
		{
			RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
			buffer.ReadPoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&0x08)>0)
			{
				buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
				buffer.ReadByte(&pMirObj->mirInfo.array_num);
			}
			if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
				buffer.ReadPoint(pMirObj->mirInfo.mir_norm);
		}
	}
	//连接板螺栓区
	pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);
}
void CLDSPlate::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.Write(&cfgword,24);
	buffer.WriteDword(dwPermission);
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	buffer.WriteString(layer_tag);
	buffer.WriteString(sNotes);
	if(version==0||
		(doc_type==1&&version>=5000010)||
		(doc_type==2&&version>=2000005)||
		(doc_type==4&&version>=1030005)||
		(doc_type==5&&version>=1030005))
	{
		buffer.WriteBooleanThin(_bSyncShadowPartNo);
		if(!_bSyncShadowPartNo)
		{
			buffer.WriteInteger(_iSeg.iSeg);
			buffer.WriteString(sPartNo,16);
		}
	}
	buffer.WriteDouble(m_fNormOffset);
	//m_fInnerRadius为与母板共用属性，不需存储 wjh-2013.2.3
	buffer.WriteBooleanThin(m_fInnerRadius>0);
	if(m_fInnerRadius>0)
	{
		buffer.WritePoint(m_tInnerColumnNorm);
		buffer.WritePoint(m_tInnerOrigin);
	}
	buffer.WriteString(sPartNo);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	buffer.WritePoint(ucs.origin);
	/*
	* 板轮廓点是与母板构件共用属性，不需存储 wjh-2013.2.3
	buffer.WriteInteger(vertex_list.GetNodeNum());
	BOOL bPush=vertex_list.push_stack();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		buffer.WritePoint(pVertex->vertex);
		buffer.WriteInteger(pVertex->vertex.feature);
		buffer.WriteInteger(pVertex->vertex.ID);
		buffer.WriteByte(pVertex->type);
		buffer.WriteBooleanThin(pVertex->m_bWeldEdge);
		buffer.WriteBooleanThin(pVertex->m_bRollEdge);
		if(pVertex->m_bRollEdge)	
			buffer.WriteWord(pVertex->roll_edge_offset_dist);	
		if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
		{
			buffer.WriteDouble(pVertex->radius);
			buffer.WritePoint(pVertex->center); //指定半径需保存圆心坐标 wht 10-08-27
		}
		if(pVertex->type==1)	//指定圆弧夹角
			buffer.WriteDouble(pVertex->sector_angle);
		buffer.WriteWord(pVertex->manu_space);
		if(pVertex->type==3)	//椭圆弧
			buffer.WritePoint(pVertex->column_norm);
		//对活点信息
		buffer.WriteInteger(pVertex->local_point_vec);
		buffer.WriteDouble(pVertex->local_point_y);
	}
	if(bPush)
		vertex_list.pop_stack();*/
	//关联构件
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pMirObj->hObj);
		buffer.WriteByte(pMirObj->mirInfo.axis_flag);
		buffer.WritePoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
			buffer.WriteByte(pMirObj->mirInfo.array_num);
		}
		if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
			buffer.WritePoint(pMirObj->mirInfo.mir_norm);
	}
	//连接板螺栓区
	pLsRefList->ToBuffer(buffer,version,doc_type);
}
void CLDSPlate::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteDword(dwPermission);
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
		mpcl.ToBuffer(&buffer,version,doc_type);
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	buffer.WriteString(layer());
	buffer.WriteInteger(iSeg);
	if(  version==0||
		(doc_type==1&&version>=5000200)||	//TMA V5.0.2.0
		(doc_type==2&&version>=2000200)||	//LMA V2.0.2.0
		(doc_type==3&&version>=2010100)||	//TSA V2.1.1.0
		(doc_type==4&&version>=1030200)||	//LDS V1.3.2.0
		(doc_type==5&&version>=1030200))	//TDA V1.3.2.0
	{
		buffer.WriteBooleanThin(m_bVirtualPart);
		buffer.WriteByte(_cbAlignment);
	}
	//buffer.WriteInteger(sub_type);
	buffer.WriteInteger(face_N);
	buffer.WriteDouble(m_fNormOffset);
	buffer.WriteBooleanThin(m_fInnerRadius>0);
	if(m_fInnerRadius>0)
	{
		buffer.WritePoint(m_tInnerColumnNorm);
		buffer.WriteDouble(m_fInnerRadius);
		buffer.WritePoint(m_tInnerOrigin);
	}
	buffer.WriteInteger(m_bWeldPart);
	buffer.WriteInteger(m_bNeedFillet);
	buffer.WriteInteger(neutrosphere);
	//buffer.WriteInteger(base_node_handle);
	//buffer.WriteInteger(base_part_handle);
	if(	version>0&&IsFoldPlate()&&(		//旧版本中jdb_style中错误的用3代表折叠板，而新版本则是1 wjh-2016.5.16
		(doc_type==1&&version<5000000)||//TMA V5.0.0.0
		(doc_type==2&&version<2000000))) //LMA V2.0.5.0
		buffer.WriteInteger(3);
	else if(version>0&&(	//旧版本中jdb_style中错误的用3代表折叠板，而新版本则是1 wjh-2016.5.16
		(doc_type==1&&version<5010500)||	//TMA V5.1.5.0
		(doc_type==2&&version<2000800)||	//LMA V2.0.8.0
		(doc_type==4&&version<1030700)||	//LDS V1.3.7.0
		(doc_type==5&&version<1030700)))	//TDA V1.3.7.0
		buffer.WriteInteger(style.wiLegacyStyle);
	else
		buffer.WriteInteger(style.iLegacyStyle);
	if(	style.wiType==WASHER_PLATE&&
		(version==0&&doc_type!=3||
		(doc_type==1&&version>=5010800)||	//TMA V5.1.7.0
		(doc_type==2&&version>=2000900)||	//LMA V2.0.9.0
		(doc_type==4&&version>=1030900)||	//LDS V1.3.9.0
		(doc_type==5&&version>=1030800)))	//TDA V1.3.8.0
	{
		buffer.WriteWord(gasketEdgeSpace.berSpace);
		buffer.WriteWord(gasketEdgeSpace.wingSpace);
		buffer.WriteWord(gasketEdgeSpace.endSpaceS);
		buffer.WriteWord(gasketEdgeSpace.endSpaceE);
	}
	if( style.wiType>0&&(version==0||
		(doc_type==1&&version>=5010500)||	//TMA V5.1.5.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&version>=1030700)||	//LDS V1.3.7.0
		(doc_type==5&&version>=1030700)))	//TDA V1.3.7.0
	{	//以后普通钢板也会逐步支持附加设计参数 wjh-2017.1.21
		BUFFERPOP stack(&buffer,params.GetNodeNum());
		buffer.WriteInteger(params.GetNodeNum());
		for(DESIGN_PLATE_ITEM *pItem=params.GetFirst();pItem;pItem=params.GetNext())
		{
			buffer.WriteWord(pItem->key);
			buffer.WriteByte(pItem->dataType);
			if(pItem->dataType==1)
				buffer.WriteInteger(pItem->value.hPart);
			else if(pItem->dataType==2)
				buffer.WriteInteger(pItem->value.iType);
			else
				buffer.WriteDouble(pItem->value.fValue);
			buffer.WriteString(pItem->sNotes);
			buffer.WriteByte(pItem->paramType);
			stack.Increment();
		}
		if(!stack.VerifyAndRestore())
	#ifdef AFX_TARG_ENU_ENGLISH
			CLDSObject::Log2File()->Log("The number record of parametric plate is wrong!");
	#else
			CLDSObject::Log2File()->Log("钢板设计参数记录数出现错误!");
	#endif
	}
	buffer.WriteByte(cMaterial);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteInteger(m_bReplPart);
	buffer.WriteByte(org_material);
	buffer.WriteString(sPartNo);
	buffer.WriteInteger(ftoi(GetThick()));
	buffer.WriteBooleanThin(m_bDisplayHole);
	buffer.WriteInteger(m_bAttachPartByWeldRoad);	//是否以焊道线为基准
	if(	version==0||
		(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&version>=2000500)||	//LMA V2.0.5.0
		(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
		(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
		buffer.WriteByte(m_cFuncType);					//钢板功用类型
	for(int i=0;i<face_N-1;i++)
	{
		buffer.WritePoint(HuoQuLine[i].startPt);
		buffer.WritePoint(HuoQuLine[i].endPt);
		buffer.WriteInteger(HuoQuLine[i].feature);
		buffer.Write(HuoQuLine[i].layer,16);
		buffer.WritePoint(huoqufacenorm[i]);
	}
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	buffer.WritePoint(ucs.origin);
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030401))	//TDA(V1,3,4,1)
		buffer.WriteByte(m_cDatumAxis);	//定义坐标系时工作平面的计算基准轴 wjh-2016.3.26
	if(face_N>2)
		buffer.WritePoint(top_point);
	buffer.WriteBooleanThin(m_bUserSpecColor);
	if(m_bUserSpecColor)
		buffer.WriteDword(crMaterial);

	//连板柔性设计参数
	designInfo.ToBuffer(buffer,version,doc_type);
	//buffer.WriteInteger(GetSubPlankCount());
	//for(long *pH=GetFirstPlankH();pH;pH=GetNextPlankH())
	//	buffer.WriteInteger(*pH);

	if(	version==0||
		(doc_type==1&&version>=5010300)||	//TMA V5.1.3.0
		(doc_type==2&&version>=2000600)||	//LMA V2.0.6.0
		(doc_type==4&&version>=1030600)||	//LDS V1.3.6.0
		(doc_type==5&&version>=1030600))	//TDA V1.3.6.0
	{
		buffer.WriteByte(_cRollProcessType);
		buffer.WriteBooleanThin(m_bClockwiseCalProfile);
	}
	if(	version==0||
		(doc_type==1&&version>=5010500)||	//TMA V5.1.5.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&version>=1030700)||	//LDS V1.3.7.0
		(doc_type==5&&version>=1030700))	//TDA V1.3.7.0
	{
		buffer.WriteByte(_ciRollOffsetType);
		buffer.WriteByte(_ciDeformType);
	}
	buffer.WriteInteger(vertex_list.GetNodeNum());
	BOOL bPush=vertex_list.push_stack();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		pVertex->ToBuffer(buffer,version,doc_type);
		/* 移至PROFILE_VERTEX::ToBuffer函数
		buffer.WritePoint(pVertex->vertex);
		buffer.WriteInteger(pVertex->vertex.feature);
		buffer.WriteInteger(pVertex->vertex.ID);
		buffer.WriteByte(pVertex->type);
		buffer.WriteBooleanThin(pVertex->m_bWeldEdge);
		buffer.WriteBooleanThin(pVertex->m_bRollEdge);
		if(pVertex->m_bRollEdge)	//保存卷边偏移量 wht 11-04-02
			buffer.WriteWord(pVertex->roll_edge_offset_dist);	
		if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
		{
			buffer.WriteDouble(pVertex->radius);
			buffer.WritePoint(pVertex->center); //指定半径需保存圆心坐标 wht 10-08-27
		}
		if(pVertex->type==1)	//指定圆弧夹角
			buffer.WriteDouble(pVertex->sector_angle);
		buffer.WriteWord(pVertex->manu_space);
		if(pVertex->type==3)	//椭圆弧
			buffer.WritePoint(pVertex->column_norm);
		//对活点信息
		buffer.WriteInteger(pVertex->local_point_vec);
		buffer.WriteDouble(pVertex->local_point_y);
		//记录顶点设计信息,用于更新顶点坐标
		buffer.WriteByte(pVertex->m_cPosDesignType);
		if(pVertex->m_cPosDesignType>0)
			buffer.WriteInteger(pVertex->m_hRod);*/
	}
	if(bPush)
		vertex_list.pop_stack();
	buffer.WriteString(sNotes);
	//连接板螺栓区
	pLsRefList->ToBuffer(buffer,version,doc_type);
	//关联构件列表
	RELATIVE_OBJECT *pRelaObj=NULL;
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pRelaObj->hObj);
		buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		buffer.WriteBooleanThin(bSpecOrigin);
		if(bSpecOrigin)
			buffer.WritePoint(pRelaObj->mirInfo.origin);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			buffer.WriteByte(pRelaObj->mirInfo.array_num);
			buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
		}
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("关联对象记录数出现错误!");
#endif
}
void PROFILE_VERTEX::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/,bool bParamPlateVertex/*=false*/,UCS_STRU* pPlateCS/*=NULL*/)
{
	buffer.ReadPoint(vertex);
	buffer.ReadInteger(&vertex.feature);
	buffer.ReadInteger(&vertex.ID);
	BOOL bBeforVersion86=TRUE;	//LMA86版本以前 wht 10-08-27
	if(version==0||(doc_type==1&&version>=4000031)||(doc_type==2&&version>=1000086)||
		(doc_type==4&&version>=1000014)||doc_type==5)
		bBeforVersion86=FALSE;
	if(!bParamPlateVertex&&((doc_type==1&&version>0&&version<4000027)||	//TMA
		(doc_type==2&&version>0&&version<1000082)||(doc_type==4&&version>0&&version<1000009)))//LMA,LDS
	{	//普通钢板顶点文件瘦身之前的版本
		DWORD dw;
		buffer.ReadInteger(&dw);
		type=(BYTE)dw;
		buffer.ReadInteger(&dw);
		m_bWeldEdge=(dw!=0);
		buffer.ReadInteger(&dw);
		m_bRollEdge=(dw!=0);
		buffer.ReadDouble(&radius);
		buffer.ReadDouble(&sector_angle);
		double ff;
		buffer.ReadDouble(&ff);
		manu_space=(short)ff;
	}
	else
	{
		buffer.ReadByte(&type);
		buffer.ReadBooleanThin(&m_bWeldEdge);
		buffer.ReadBooleanThin(&m_bRollEdge);
		if( version==0||doc_type==5||
			(doc_type==1&&version>=4000102)|| //TMA V4.0.1.2
			(doc_type==2&&version>=1000102)|| //LMA V1.0.1.2
			(doc_type==3&&version>=1070302)|| //TSA V1.7.3.2
			(doc_type==4&&version>=1000302))  //LDS V1.0.3.2
		{
			if(!bParamPlateVertex&&m_bRollEdge)	//保存卷边偏移量 wht 11-04-02,参数化板不考虑卷边 wjh 2014.8.13
				buffer.ReadWord(&roll_edge_offset_dist);	
		}
		if(type==2||type==3)	//指定圆弧半径
		{
			buffer.ReadDouble(&radius);
			if(!bBeforVersion86)
			{
				buffer.ReadPoint(center);
				bool bVersionNeedTransUcs=false;
				//#if defined(__TMA_FILE_)||defined(__TMA_)
				if(doc_type==1&&version>0&&version<=4000101&&type==2)
					bVersionNeedTransUcs=true;
				//#elif defined(__LMA_FILE_)||defined(__LMA_)
				else if(doc_type==2&&version>0&&version<=1000101&&type==2)
					bVersionNeedTransUcs=true;
				//#elif defined(__LDS_FILE_)||defined(__LDS_)
				else if(doc_type==4&&version>0&&version<=1000301&&type==2)
					bVersionNeedTransUcs=true;
				//#endif
				if(bVersionNeedTransUcs&&(fabs(center.z)>EPS||
					fabs(center.x)>1000||fabs(center.y)>1000)&&pPlateCS)
				{	//由绝对坐标系转到板相对坐标系
					coord_trans(center,*pPlateCS,FALSE);
				}
			}
		}
		if(type==1)	//指定圆弧夹角
			buffer.ReadDouble(&sector_angle);
		buffer.ReadWord(&manu_space);
	}
	if(type==3)	//椭圆弧
	{
		if(bBeforVersion86)
			buffer.ReadPoint(center);
		buffer.ReadPoint(column_norm);
	}
	if( m_bWeldEdge&&(version==0||
		(doc_type==1&&version>=4010200)|| //TMA V4.1.2.0
		(doc_type==2&&version>=1010200)|| //LMA V1.1.2.0
		(doc_type==4&&version>=1020101)|| //LDS V1.2.1.1
		(doc_type==5&&version>=1020101))) //TDA V1.2.1.1
		weldline.FromBuffer(buffer);
	if(!bParamPlateVertex)
	{	//普通钢板还需存储额外的钢板对活点及顶点设计位置参数
		buffer.ReadInteger(&local_point_vec);
		buffer.ReadDouble(&local_point_y);
		if( version==0||
			(doc_type==1&&version>=4010100)|| //TMA V4.1.1.0
			(doc_type==2&&version>=1010100)|| //LMA V1.1.0.0
			(doc_type==3&&version>=2000008)|| //TSA V2.0.0.8
			(doc_type==4&&version>=1020008)|| //LDS V1.2.0.8
			(doc_type==5&&version>=1020008))  //TDA V1.2.0.8
		{	//记录顶点设计信息,用于更新顶点坐标
			buffer.ReadByte(&m_cPosDesignType);
			if(m_cPosDesignType>0)
				buffer.ReadInteger(&m_hRod);
			if(((m_cPosDesignType>=1&&m_cPosDesignType<=4)||m_cPosDesignType==DT_BEND_EXTEND_POINT)&&
				(version==0&&doc_type!=3||
				(doc_type==1&&version>=5010900)||	//TMA V5.1.9.0
				(doc_type==2&&version>=2000900)||	//LMA V2.0.9.0
				(doc_type==4&&version>=1030900)||	//LDS V1.3.9.0
				(doc_type==5&&version>=1030900)))	//TDA V1.3.9.0
			{
				buffer.ReadByte(&xOffset.ciOffMode);
				buffer.ReadFloat(&xOffset.x);
				buffer.ReadFloat(&xOffset.y);
			}
		}
	}
}
void PROFILE_VERTEX::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/,bool bParamPlateVertex/*=false*/)
{
	buffer.WritePoint(vertex);
	buffer.WriteInteger(vertex.feature);
	buffer.WriteInteger(vertex.ID);
	buffer.WriteByte(type);
	buffer.WriteBooleanThin(m_bWeldEdge);
	buffer.WriteBooleanThin(m_bRollEdge);
	if(!bParamPlateVertex&&m_bRollEdge)	//保存卷边偏移量 wht 11-04-02
		buffer.WriteWord(roll_edge_offset_dist);	
	if(type==2||type==3)	//指定圆弧半径
	{
		buffer.WriteDouble(radius);
		buffer.WritePoint(center); //指定半径需保存圆心坐标 wht 10-08-27
	}
	if(type==1)	//指定圆弧夹角
		buffer.WriteDouble(sector_angle);
	buffer.WriteWord(manu_space);
	if(type==3)	//椭圆弧
		buffer.WritePoint(column_norm);
	if( m_bWeldEdge&&(version==0||
		(doc_type==1&&version>=4010200)|| //TMA V4.1.2.0
		(doc_type==2&&version>=1010200)|| //LMA V1.1.2.0
		(doc_type==4&&version>=1020101)|| //LDS V1.2.1.1
		(doc_type==5&&version>=1020101))) //TDA V1.2.1.1
		weldline.ToBuffer(buffer);
	if(!bParamPlateVertex)
	{	//对活点信息
		buffer.WriteInteger(local_point_vec);
		buffer.WriteDouble(local_point_y);
		//记录顶点设计信息,用于更新顶点坐标
		buffer.WriteByte(m_cPosDesignType);
		if(m_cPosDesignType>0)
			buffer.WriteInteger(m_hRod);
		if(((m_cPosDesignType>=1&&m_cPosDesignType<=4)||m_cPosDesignType==DT_BEND_EXTEND_POINT)&&
			(version==0&&doc_type!=3||
			(doc_type==1&&version>=5010900)||	//TMA V5.1.9.0
			(doc_type==2&&version>=2000900)||	//LMA V2.0.9.0
			(doc_type==4&&version>=1030900)||	//LDS V1.3.9.0
			(doc_type==5&&version>=1030900)))	//TDA V1.3.9.0
		{
			buffer.WriteByte(xOffset.ciOffMode);
			buffer.WriteFloat(xOffset.x);
			buffer.WriteFloat(xOffset.y);
		}
	}
}
void CLDSPlate::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	fprintf(fp,"<普通板信息 handle=\"0x%X\">\n",handle);
	fprintf(fp,"<配材号 cfgword=\"%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3]);
	fprintf(fp,"<权限子 dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<统材系数 statMatCoef=\"%d\"/>\n",m_uStatMatCoef);
	fprintf(fp,"<焊接父构件 hPartWeldParent=\"0x%x\"/>\n",m_hPartWeldParent);
	fprintf(fp,"<所在图层 layer=\"%s\"/>\n",layer());
	fprintf(fp,"<段号 iSeg=\"%d\"/>\n",iSeg);
	fprintf(fp,"<板的面数 face_N=\"%d\"/>\n",face_N);
	sprintf(sText,"%f",m_fNormOffset);
	SimplifiedNumString(sText);
	fprintf(fp,"<法向偏移量 NormOffset=\"%s\"/>\n",sText);
	if(m_fInnerRadius>0)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<是否有内圆弧 bInnerR=\"%s\"/>\n",sText);
	if(m_fInnerRadius>0)
	{
		fprintf(fp,"<内圆弧法线方向 Norm=\"(%.3f,%.3f,%.3f)\"/>\n",m_tInnerColumnNorm.x,m_tInnerColumnNorm.y,m_tInnerColumnNorm.z);
		sprintf(sText,"%f",m_fInnerRadius);
		SimplifiedNumString(sText);
		fprintf(fp,"<内圆弧半径 InnerR=\"%s\"/>\n",sText);
		fprintf(fp,"<圆心坐标 InnerOrigin=\"(%.3f,%.3f,%.3f)\"/>\n",m_tInnerOrigin.x,m_tInnerOrigin.y,m_tInnerOrigin.z);
	}
	if(m_bWeldPart==1)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<是否是焊接件 bWelPart=\"%s\"/>\n",sText);
	if(m_bNeedFillet==1)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<是否需要坡口 bNeedFillet=\"%s\"/>\n",sText);
	fprintf(fp,"<中性层位置 neutrosphere=\"%d\"/>\n",neutrosphere);
	fprintf(fp,"<普通板类型 jdb_style=\"%d\"/>\n",jdb_style);
	fprintf(fp,"<材质 cMaterial=\"%c\"/>\n",cMaterial);
	if(m_bReplPart==1)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<是否为代用构件 bReplPart=\"%s\"/>\n",sText);
	fprintf(fp,"<原材质 org_material=\"%c\"/>\n",org_material);
	fprintf(fp,"<构件编号 sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<板厚 thick=\"%d\"/>\n",ftoi(GetThick()));
	if(m_bDisplayHole)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<是否显示孔 bDisplayHole=\"%s\"/>\n",sText);
	if(m_bAttachPartByWeldRoad==1)
		strcpy(sText,"true");
	else
		strcpy(sText,"false");
	fprintf(fp,"<是否以焊道线为基准 bAttachPartByWeldRoad=\"%s\"/>\n",sText);
	for(int i=0;i<face_N-1;i++)
	{
		fprintf(fp,"<火曲信息>\n");
		fprintf(fp,"<火曲线始端坐标 startPt=\"(%.3f,%.3f,%.3f)\"/>\n",HuoQuLine[i].startPt.x,HuoQuLine[i].startPt.y,HuoQuLine[i].startPt.z);
		fprintf(fp,"<火曲线终端坐标 endPt=\"(%.3f,%.3f,%.3f)\"/>\n",HuoQuLine[i].endPt.x,HuoQuLine[i].endPt.y,HuoQuLine[i].endPt.z);
		fprintf(fp,"<标记 feature=\"%d\"/>\n",HuoQuLine[i].feature);
		fprintf(fp,"<图层 layer=\"%s\"/>\n",HuoQuLine[i].layer);
		fprintf(fp,"<火曲面坐标 huoqufacenorm=\"(%.3f,%.3f,%.3f)\"/>\n",huoqufacenorm[i].x,huoqufacenorm[i].y,huoqufacenorm[i].z);
		fprintf(fp,"</火曲信息>\n");
	}
	fprintf(fp,"<X轴方向 axis_x=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_x.x,ucs.axis_x.y,ucs.axis_x.z);
	fprintf(fp,"<Y轴方向 axis_y=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_y.x,ucs.axis_y.y,ucs.axis_y.z);
	fprintf(fp,"<Z轴方向 axis_z=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.axis_z.x,ucs.axis_z.y,ucs.axis_z.z);
	fprintf(fp,"<坐标原点 origin=\"(%.3f,%.3f,%.3f)\"/>\n",ucs.origin.x,ucs.origin.y,ucs.origin.z);
	if(face_N>2)
		fprintf(fp,"<火曲线交点坐标 top_point=\"(%.3f,%.3f,%.3f)\"/>\n",top_point.x,top_point.y,top_point.z);
	if(m_bUserSpecColor)
		strcpy(sText,"true");
	else 
		strcpy(sText,"false");
	fprintf(fp,"<用户指定构件颜色 bUserSpacColor=\"%s\"/>\n",sText);
	if(m_bUserSpecColor)
		fprintf(fp,"<构件颜色 crMaterial=\"%d\"/>\n",crMaterial);

	//连板柔性设计参数
	designInfo.ToXmlFile(fp,schema);
	fprintf(fp,"<顶点列表 num=\"%d\"/>\n",vertex_list.GetNodeNum());
	BOOL bPush=vertex_list.push_stack();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		fprintf(fp,"<顶点信息 vertex=\"(%.3f,%.3f,%.3f)\" feature=\"%d\" ID=\"%d\" type=\"%d\">\n",pVertex->vertex.x,pVertex->vertex.y,pVertex->vertex.z,
			pVertex->vertex.feature,pVertex->vertex.ID,pVertex->type);
		if(pVertex->m_bWeldEdge)
			strcpy(sText,"true");
		else 
			strcpy(sText,"false");
		fprintf(fp,"<是否需焊缝边 bWeldEdge=\"%s\"/>\n",sText);
		if(pVertex->m_bRollEdge)
			strcpy(sText,"true");
		else
			strcpy(sText,"false");
		fprintf(fp,"<是否需卷边 bRollEdge=\"%s\"/>\n",sText);
		if(pVertex->m_bRollEdge)
			fprintf(fp,"<卷边偏移量 offset_dist=\"%d\"/>\n",pVertex->roll_edge_offset_dist);
		if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
		{
			sprintf(sText,"%f",pVertex->radius);
			SimplifiedNumString(sText);
			fprintf(fp,"<圆弧半径 radius=\"%s\"/>\n",sText);
			fprintf(fp,"<圆心坐标 center=\"(%.3f,%.3f,%.3f)\"/>\n",pVertex->center.x,pVertex->center.y,pVertex->center.z);
		}
		if(pVertex->type==1)	//指定圆弧夹角
			fprintf(fp,"<圆弧夹角 sector_angle=\"%.2f\"/>\n",pVertex->sector_angle);
		fprintf(fp,"<焊缝间隙或卷边高度 manu_space=\"%d\"/>\n",pVertex->manu_space);
		if(pVertex->type==3)	//椭圆弧
			fprintf(fp,"<椭圆弧法向 column_norm=\"(%.3f,%.3f,%.3f)\"/>\n",pVertex->column_norm.x,pVertex->column_norm.y,pVertex->column_norm.z);
		//对活点信息
		fprintf(fp,"<对活点信息 local_point_vec=\"%d\" local_point_y=\"%.2f\"/>\n",pVertex->local_point_vec,pVertex->local_point_y);
		fprintf(fp,"</顶点信息 >\n");
		//重新计算顶点坐标信息
		fprintf(fp,"<重新计算顶点坐标信息  nPosDesignType=\"%d\" hRod=\"0X%X\"/>\n",pVertex->m_cPosDesignType,pVertex->m_hRod);
		fprintf(fp,"</重新计算顶点坐标信息 >\n");
	}
	if(bPush)
		vertex_list.pop_stack();
	fprintf(fp,"<备注 sNotes=\"%s\"/>\n",sNotes);
	pLsRefList->ToXmlFile(fp,schema);
	RELATIVE_OBJECT *pRelaObj=NULL;
	fprintf(fp,"<关联构件数量 objsNum=\"%d\"/>\n",relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		fprintf(fp,"<关联构件信息 handle=\"0x%x\" axis_flag=\"%d\">\n",pRelaObj->hObj,pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		if(bSpecOrigin)
			strcpy(sText,"true");
		else
			strcpy(sText,"false");
		fprintf(fp,"<是否有对称基点 bSepcOrigin=\"%s\"/>\n",sText);
		if(bSpecOrigin)
			fprintf(fp,"<对称基点坐标 origin=\"(%.3f,%.3,%.3f)\"/>\n",pRelaObj->mirInfo.origin.x,pRelaObj->mirInfo.origin.y,pRelaObj->mirInfo.origin.z);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			fprintf(fp,"<对称旋转信息>\n");
			fprintf(fp,"<旋转对称次数 array_num=\"%d\"/>\n",pRelaObj->mirInfo.array_num);
			fprintf(fp,"<旋转角度 rotate_angle=\"%.2f\"/>\n",pRelaObj->mirInfo.rotate_angle);
			fprintf(fp,"</对称旋转信息>\n");
		}
		fprintf(fp,"</关联构件信息 >\n");
	}
	fprintf(fp,"</普通板信息>\n");
}
void CLDSPlate::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
		return;
	_console->AddToObjSetBuf(this);
	if(!bAddRelaObj)
		return;
	bAddRelaObj=bAddSubLevelRelaObj;
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		pLsRef->GetLsPtr()->AddToConsoleObjBuf(bAddRelaObj);
}

bool CLDSGeneralPlate::IntelliRetrieveGasketEdgeSpace(CLDSPlate::GASKETEDGESPACE* pEdgeSpace/*=NULL*/,BOOL bInitPosDesType/*=TRUE*/)
{
	if(GetClassTypeId()!=CLS_PLATE)
		return false;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(((CLDSPlate*)this)->designInfo.m_hBasePart,CLS_LINEANGLE);
	if(pAngle==NULL)
		return false;
	SCOPE_STRU scope;
	CMinDouble minX,minY;
	CMaxDouble maxX,maxY;
	//计算局部坐标螺栓范围
	f3dPoint ls_pos;
	GEPOINT interspoint;
	for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		ls_pos=pLsRef->pBolt->ucs.origin;
		this->GetBoltIntersPos(pLsRef->pBolt,interspoint);
		coord_trans(interspoint,ucs,FALSE,TRUE);
		scope.VerifyVertex(interspoint);
	}
	for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		minX.Update(pVertex->vertex.x);
		maxX.Update(pVertex->vertex.x);
		minY.Update(pVertex->vertex.y);
		maxY.Update(pVertex->vertex.y);
	}
	BYTE start0_end1=DISTANCE(ls_pos,pAngle->Start())<DISTANCE(ls_pos,pAngle->End())?0:1;
	char ciWorkWing=fabs(pAngle->vxWingNorm*ucs.axis_z)>fabs(pAngle->vyWingNorm*ucs.axis_z)?'X':'Y';
	GEPOINT vWingVec=ciWorkWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	GEPOINT vLenVec=start0_end1==0?(pAngle->End()-pAngle->Start()):(pAngle->Start()-pAngle->End());
	if(cDatumAxis=='Y')
	{	//基准杆件纵向为垫板局部坐标系的Y轴
		int signLenY =vLenVec *ucs.axis_y<0?-1:1;
		int signWingX=vWingVec*ucs.axis_x<0?-1:1;
		gasketEdgeSpace.endSpaceS=ftoi(scope.fMinY-minY.number);
		gasketEdgeSpace.wingSpace=ftoi(maxX.number-scope.fMaxX);
		gasketEdgeSpace.berSpace =ftoi(scope.fMinX-minX.number);
		if(signLenY<0)
		{
			gasketEdgeSpace.endSpaceE=gasketEdgeSpace.endSpaceS;
			gasketEdgeSpace.endSpaceS=ftoi(maxY.number-scope.fMaxY);
		}
		else
			gasketEdgeSpace.endSpaceE=ftoi(maxY.number-scope.fMaxY);
		if(signWingX<0)
		{
			gasketEdgeSpace.wingSpace=gasketEdgeSpace.berSpace;
			gasketEdgeSpace.berSpace =ftoi(maxX.number-scope.fMaxX);
		}
	}
	else
	{	//基准杆件纵向为垫板局部坐标系的X轴
		int signLenX =vLenVec *ucs.axis_x<0?-1:1;
		int signWingY=vWingVec*ucs.axis_y<0?-1:1;
		gasketEdgeSpace.endSpaceS=ftoi(scope.fMinX-minX.number);
		gasketEdgeSpace.wingSpace=ftoi(maxY.number-scope.fMaxY);
		gasketEdgeSpace.berSpace =ftoi(scope.fMinY-minY.number);
		if(signLenX<0)
		{
			gasketEdgeSpace.endSpaceE=gasketEdgeSpace.endSpaceS;
			gasketEdgeSpace.endSpaceS=ftoi(maxX.number-scope.fMaxX);
		}
		else
			gasketEdgeSpace.endSpaceE=ftoi(maxX.number-scope.fMaxX);
		if(signWingY<0)
		{
			gasketEdgeSpace.wingSpace=gasketEdgeSpace.berSpace;
			gasketEdgeSpace.berSpace =ftoi(maxY.number-scope.fMaxY);
		}
	}
	((CLDSPlate*)this)->designInfo.m_bEnableFlexibleDesign=TRUE;	
	if(bInitPosDesType)
		RetrieveGasketVertexCalType();
	if(pEdgeSpace)
		*pEdgeSpace=gasketEdgeSpace;
	return true;
}
bool CLDSGeneralPlate::RetrieveGasketVertexCalType()
{
	if(GetClassTypeId()!=CLS_PLATE||vertex_list.GetNodeNum()<=3)
		return false;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(((CLDSPlate*)this)->designInfo.m_hBasePart,CLS_LINEANGLE);
	if(pAngle==NULL)
		return false;
	CMinDouble minX,minY;
	CMaxDouble maxX,maxY;
	for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		minX.Update(pVertex->vertex.x);
		maxX.Update(pVertex->vertex.x);
		minY.Update(pVertex->vertex.y);
		maxY.Update(pVertex->vertex.y);
	}
	f3dPoint pt=(*GetFirstLsRef())->ucs.origin;
	BYTE start0_end1=DISTANCE(pt,pAngle->Start())<DISTANCE(pt,pAngle->End())?0:1;
	char ciWorkWing=fabs(pAngle->vxWingNorm*ucs.axis_z)>fabs(pAngle->vyWingNorm*ucs.axis_z)?'X':'Y';
	GEPOINT vWingVec=ciWorkWing=='X'?pAngle->GetWingVecX():pAngle->GetWingVecY();
	GEPOINT vLenVec=(start0_end1==0)?(pAngle->End()-pAngle->Start()):(pAngle->Start()-pAngle->End());
	if(cDatumAxis=='Y')
	{	//基准杆件纵向为垫板局部坐标系的Y轴
		int signLenY =vLenVec *ucs.axis_y<0?-1:1;
		int signWingX=vWingVec*ucs.axis_x<0?-1:1;
		PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
		BYTE ciPrevInB1_W2=0;
		if((ftoi(pPrevVertex->vertex.x)==ftoi(minX.number)&&signWingX>0)||(ftoi(pPrevVertex->vertex.x)==ftoi(maxX.number)&&signWingX<0))
			ciPrevInB1_W2=1;
		else if((ftoi(pPrevVertex->vertex.x)==ftoi(minX.number)&&signWingX<0)||(ftoi(pPrevVertex->vertex.x)==ftoi(maxX.number)&&signWingX>0))
			ciPrevInB1_W2=2;
		for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			BYTE ciInS1_E2=0,ciInB1_W2=0;
			if((ftoi(pVertex->vertex.y)==ftoi(minY.number)&&signLenY>0)||(ftoi(pVertex->vertex.y)==ftoi(maxY.number)&&signLenY<0))
				ciInS1_E2=1;
			else if((ftoi(pVertex->vertex.y)==ftoi(minY.number)&&signLenY<0)||(ftoi(pVertex->vertex.y)==ftoi(maxY.number)&&signLenY>0))
				ciInS1_E2=2;
			if((ftoi(pVertex->vertex.x)==ftoi(minX.number)&&signWingX>0)||(ftoi(pVertex->vertex.x)==ftoi(maxX.number)&&signWingX<0))
				ciInB1_W2=1;
			else if((ftoi(pVertex->vertex.x)==ftoi(minX.number)&&signWingX<0)||(ftoi(pVertex->vertex.x)==ftoi(maxX.number)&&signWingX>0))
				ciInB1_W2=2;
			pVertex->m_hRod=((CLDSPlate*)this)->designInfo.m_hBasePart;
			if(ciInS1_E2==1&&ciInB1_W2==1)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER2;
			else if(ciInS1_E2==1&&ciInB1_W2==2)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING2;
			else if(ciInS1_E2==2&&ciInB1_W2==1)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER;
			else if(ciInS1_E2==2&&ciInB1_W2==2)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING;
			else if(ciInS1_E2==1||ciInS1_E2==2)
			{
				GEPOINT vPrevEdgeVec=pVertex->vertex-pPrevVertex->vertex;
				if(ciPrevInB1_W2>0)
				{
					if(fabs(vPrevEdgeVec.y)<0.1)	//0.1mm是经验值误差
						ciInB1_W2=3-ciPrevInB1_W2;	//两端原始边（非切角边）
					else //if(fabs(vPrevEdgeVec.y)>=EPS)
						ciInB1_W2=ciPrevInB1_W2;	//切角边
					if(ciInB1_W2==1)
						pVertex->xOffset.x=signWingX>0?(float)(pVertex->vertex.x-minX.number):(float)(pVertex->vertex.x-maxX.number);
					else
						pVertex->xOffset.x=signWingX<0?(float)(pVertex->vertex.x-minX.number):(float)(pVertex->vertex.x-maxX.number);
				}
				else if((maxX.number-pVertex->vertex.x)>(pVertex->vertex.x-minX.number))
				{
					pVertex->xOffset.x=(float)(pVertex->vertex.x-minX.number);
					ciInB1_W2=signWingX>0?1:2;
				}
				else
				{
					pVertex->xOffset.x=(float)(pVertex->vertex.x-maxX.number);
					ciInB1_W2=signWingX>0?2:1;
				}
				if(ciInS1_E2==1)
					pVertex->m_cPosDesignType=ciInB1_W2==1?PROFILE_VERTEX::DT_BER2:PROFILE_VERTEX::DT_WING2;
				else
					pVertex->m_cPosDesignType=ciInB1_W2==1?PROFILE_VERTEX::DT_BER:PROFILE_VERTEX::DT_WING;
			}
			else if(ciInB1_W2==1||ciInB1_W2==2)
			{
				if((maxY.number-pVertex->vertex.y)>pVertex->vertex.y-minY.number)
				{
					pVertex->xOffset.y=(float)(pVertex->vertex.y-minY.number);
					ciInS1_E2=signLenY>0?1:2;
				}
				else
				{
					pVertex->xOffset.y=(float)(pVertex->vertex.y-maxY.number);
					ciInS1_E2=signLenY>0?2:1;
				}
				if(ciInB1_W2==1)
					pVertex->m_cPosDesignType=ciInS1_E2==1?PROFILE_VERTEX::DT_BER2:PROFILE_VERTEX::DT_BER;
				else
					pVertex->m_cPosDesignType=ciInS1_E2==1?PROFILE_VERTEX::DT_WING2:PROFILE_VERTEX::DT_WING;
			}
			else
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_NONE;
			ciPrevInB1_W2=ciInB1_W2;
			pPrevVertex=pVertex;
		}
	}
	else
	{	//基准杆件纵向为垫板局部坐标系的X轴
		int signLenX =vLenVec *ucs.axis_x<0?-1:1;
		int signWingY=vWingVec*ucs.axis_y<0?-1:1;
		PROFILE_VERTEX* pPrevVertex=vertex_list.GetTail();
		BYTE ciPrevInB1_W2=0;
		if((ftoi(pPrevVertex->vertex.y)==ftoi(minY.number)&&signWingY>0)||(ftoi(pPrevVertex->vertex.y)==ftoi(maxY.number)&&signWingY<0))
			ciPrevInB1_W2=1;
		else if((ftoi(pPrevVertex->vertex.y)==ftoi(minY.number)&&signWingY<0)||(ftoi(pPrevVertex->vertex.y)==ftoi(maxY.number)&&signWingY>0))
			ciPrevInB1_W2=2;
		for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			BYTE ciInS1_E2=0,ciInB1_W2=0;
			if((ftoi(pVertex->vertex.x)==ftoi(minX.number)&&signLenX>0)||(ftoi(pVertex->vertex.x)==ftoi(maxX.number)&&signLenX<0))
				ciInS1_E2=1;
			else if((ftoi(pVertex->vertex.x)==ftoi(minX.number)&&signLenX<0)||(ftoi(pVertex->vertex.x)==ftoi(maxX.number)&&signLenX>0))
				ciInS1_E2=2;
			if((ftoi(pVertex->vertex.y)==ftoi(minY.number)&&signWingY>0)||(ftoi(pVertex->vertex.y)==ftoi(maxY.number)&&signWingY<0))
				ciInB1_W2=1;
			else if((ftoi(pVertex->vertex.y)==ftoi(minY.number)&&signWingY<0)||(ftoi(pVertex->vertex.y)==ftoi(maxY.number)&&signWingY>0))
				ciInB1_W2=2;
			//
			pVertex->m_hRod=((CLDSPlate*)this)->designInfo.m_hBasePart;
			if(ciInS1_E2==1&&ciInB1_W2==1)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER2;
			else if(ciInS1_E2==1&&ciInB1_W2==2)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING2;
			else if(ciInS1_E2==2&&ciInB1_W2==1)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_BER;
			else if(ciInS1_E2==2&&ciInB1_W2==2)
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_WING;
			else if(ciInS1_E2==1||ciInS1_E2==2)
			{
				GEPOINT vPrevEdgeVec=pVertex->vertex-pPrevVertex->vertex;
				if(ciPrevInB1_W2>0)
				{
					if(fabs(vPrevEdgeVec.x)<0.1)	//0.1mm是经验值误差
						ciInB1_W2=3-ciPrevInB1_W2;	//两端原始边（非切角边）
					else //if(fabs(vPrevEdgeVec.x)>=EPS)
						ciInB1_W2=ciPrevInB1_W2;	//切角边
					if(ciInB1_W2==1)
						pVertex->xOffset.y=signWingY>0?(float)(pVertex->vertex.y-minY.number):(float)(pVertex->vertex.y-maxY.number);
					else
						pVertex->xOffset.y=signWingY<0?(float)(pVertex->vertex.y-minY.number):(float)(pVertex->vertex.y-maxY.number);
				}
				else if((maxY.number-pVertex->vertex.y)>(pVertex->vertex.y-minY.number))
				{
					pVertex->xOffset.y=(float)(pVertex->vertex.y-minY.number);
					ciInB1_W2=signWingY>0?1:2;
				}
				else
				{
					pVertex->xOffset.y=(float)(pVertex->vertex.y-maxY.number);
					ciInB1_W2=signWingY>0?2:1;
				}
				if(ciInS1_E2==1)
				{
					pVertex->m_cPosDesignType=ciInB1_W2==1?PROFILE_VERTEX::DT_BER2:PROFILE_VERTEX::DT_WING2;
				}
				else
				{
					pVertex->m_cPosDesignType=ciInB1_W2==1?PROFILE_VERTEX::DT_BER:PROFILE_VERTEX::DT_WING;
				}
			}
			else if(ciInB1_W2==1||ciInB1_W2==2)
			{
				if((maxX.number-pVertex->vertex.x)>pVertex->vertex.x-minX.number)
				{
					pVertex->xOffset.x=(float)(pVertex->vertex.x-minX.number);
					ciInS1_E2=signLenX>0?1:2;
				}
				else
				{
					pVertex->xOffset.x=(float)(pVertex->vertex.x-maxX.number);
					ciInS1_E2=signLenX>0?2:1;
				}
				if(ciInB1_W2==1)
				{
					pVertex->m_cPosDesignType=ciInS1_E2==1?PROFILE_VERTEX::DT_BER2:PROFILE_VERTEX::DT_BER;
				}
				else
				{
					pVertex->m_cPosDesignType=ciInS1_E2==1?PROFILE_VERTEX::DT_WING2:PROFILE_VERTEX::DT_WING;
				}
			}
			else
				pVertex->m_cPosDesignType=PROFILE_VERTEX::DT_NONE;
			ciPrevInB1_W2=ciInB1_W2;
			pPrevVertex=pVertex;
		}
	}
	return true;
}
void CLDSPlate::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
	{	
		IndependentShadowPropFromBuffer(buffer,version,doc_type);
		return;
	}
	DWORD dw;
	bool bPrevDeflateVersion=false;	//文件瘦身之前的版本
	if(	  (doc_type==1&&version>0&&version<4000027)	//TMA
		||(doc_type==2&&version>0&&version<1000082)	//LMA
		||(doc_type==4&&version>0&&version<1000009))//LDS
		bPrevDeflateVersion=true;
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.ReadDword(&dwPermission);	
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
		mpcl.FromBuffer(&buffer,version,doc_type);
	buffer.ReadInteger(&m_uStatMatCoef);
	buffer.ReadInteger(&_hPartWeldParent);
	buffer.ReadString(layer(),4);
	buffer.ReadInteger(&_iSeg.iSeg);
	if(  version==0||
		(doc_type==1&&version>=5000200)||	//TMA V5.0.2.0
		(doc_type==2&&version>=2000200)||	//LMA V2.0.2.0
		(doc_type==3&&version>=2010100)||	//LMA V2.1.1.0
		(doc_type==4&&version>=1030200)||	//LDS V1.3.2.0
		(doc_type==5&&version>=1030200))	//TDA V1.3.2.0
	{
		buffer.ReadBooleanThin(&_bVirtualPart);
		buffer.ReadByte(&_cbAlignment);
	}
	buffer.ReadInteger(&_face_N);
	buffer.ReadDouble(&m_fNormOffset);
	if(bPrevDeflateVersion)
	{
		buffer.ReadPoint(m_tInnerColumnNorm);
		buffer.ReadDouble(&_fInnerRadius);
		buffer.ReadPoint(m_tInnerOrigin);
	}
	else
	{
		bool bHasInnerCircle;
		buffer.ReadBooleanThin(&bHasInnerCircle);
		if(bHasInnerCircle)
		{
			buffer.ReadPoint(m_tInnerColumnNorm);
			buffer.ReadDouble(&_fInnerRadius);
			buffer.ReadPoint(m_tInnerOrigin);
		}
	}
	buffer.ReadInteger(&m_bWeldPart);
	buffer.ReadInteger(&m_bNeedFillet);
	buffer.ReadInteger(&neutrosphere);
	buffer.ReadInteger(&style.iLegacyStyle);
	gasketEdgeSpace.initialized=false;
	if(	style.wiType==WASHER_PLATE&&
		(version==0&&doc_type!=3||
		(doc_type==1&&version>=5010800)||	//TMA V5.1.7.0
		(doc_type==2&&version>=2000900)||	//LMA V2.0.9.0
		(doc_type==4&&version>=1030900)||	//LDS V1.3.9.0
		(doc_type==5&&version>=1030800)))	//TDA V1.3.8.0
	{
		buffer.ReadWord(&gasketEdgeSpace.berSpace);
		buffer.ReadWord(&gasketEdgeSpace.wingSpace);
		buffer.ReadWord(&gasketEdgeSpace.endSpaceS);
		buffer.ReadWord(&gasketEdgeSpace.endSpaceE);
		gasketEdgeSpace.initialized=true;
	}
	if( style.wiType>0&&(version==0||
		(doc_type==1&&version>=5010500)||	//TMA V5.1.5.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&version>=1030700)||	//LDS V1.3.7.0
		(doc_type==5&&version>=1030700)))	//TDA V1.3.7.0
	{	//以后普通钢板也会逐步支持附加设计参数 wjh-2017.1.21
		int count=0;
		buffer.ReadInteger(&count);
		params.Empty();
		DESIGN_PLATE_ITEM *pItem;
		for(int i=0;i<count;i++)
		{
			KEY2C key;
			buffer.ReadWord(&key.wItem);
			pItem=params.Add(key);
			buffer.ReadByte(&pItem->dataType);
			if(pItem->dataType==1)
				buffer.ReadInteger(&pItem->value.hPart);
			else if(pItem->dataType==2)
				buffer.ReadInteger(&pItem->value.iType);
			else
				buffer.ReadDouble(&pItem->value.fValue);
			buffer.ReadString(pItem->sNotes);
			buffer.ReadByte(&pItem->paramType);
		}
	}
	buffer.ReadByte(&_material);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	buffer.ReadInteger(&m_bReplPart);
	buffer.ReadByte(&org_material);
	buffer.ReadString(sPartNo);
	//long thick;
	buffer.ReadInteger(&_thick);
	//Thick=thick;
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);//显示螺栓孔及切割面
		m_bDisplayHole=(dw!=0);
	}
	else
		buffer.ReadBooleanThin(&m_bDisplayHole);
	buffer.ReadInteger(&m_bAttachPartByWeldRoad);
	if(	version==0||
		(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&version>=2000500)||	//LMA V2.0.5.0
		(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
		(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
		buffer.ReadByte(&_cFuncType);
	if(bPrevDeflateVersion)
	{
		buffer.ReadPoint(HuoQuLine[0].startPt);
		buffer.ReadPoint(HuoQuLine[0].endPt);
		buffer.ReadPoint(HuoQuLine[1].startPt);
		buffer.ReadPoint(HuoQuLine[1].endPt);
		buffer.ReadInteger(&HuoQuLine[0].feature);
		buffer.ReadInteger(&HuoQuLine[1].feature);
		buffer.Read(HuoQuLine[0].layer,16);
		buffer.Read(HuoQuLine[1].layer,16);
	}
	else
	{
		for(int i=0;i<face_N-1;i++)
		{
			buffer.ReadPoint(HuoQuLine[i].startPt);
			buffer.ReadPoint(HuoQuLine[i].endPt);
			buffer.ReadInteger(&HuoQuLine[i].feature);
			buffer.Read(HuoQuLine[i].layer,16);
			buffer.ReadPoint(huoqufacenorm[i]);
		}
	}
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	buffer.ReadPoint(ucs.origin);
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030401))	//TDA(V1,3,4,1)
		buffer.ReadByte(&m_cDatumAxis);	//定义坐标系时工作平面的计算基准轴 wjh-2016.3.26
	if(bPrevDeflateVersion||face_N>2)
		buffer.ReadPoint(top_point);
	if(bPrevDeflateVersion)
	{
		buffer.ReadPoint(huoqufacenorm[0]);
		buffer.ReadPoint(huoqufacenorm[1]);
	}
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000021)//version==NULL||compareVersion(version,"4.0.0.21")>=0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000076)//version==NULL||compareVersion(version,"1.0.0.76")>=0)
#endif*/
	if(version==0||(doc_type==1&&version>=4000021)||(doc_type==2&&version>=1000076)||doc_type>2)
	{
		if(bPrevDeflateVersion)
		{
			buffer.ReadInteger(&dw);
			m_bUserSpecColor=(dw!=0);
		}
		else
			buffer.ReadBooleanThin(&m_bUserSpecColor);
		if(m_bUserSpecColor)
			buffer.ReadDword(&crMaterial);
	}
	//连板柔性设计参数
	designInfo.FromBuffer(buffer,version,doc_type);
	if(designInfo.origin.datum_pos_style==0)
		designInfo.origin.SetPosition(ucs.origin);
	if(designInfo.norm.norm_style==0)
		designInfo.norm.vector=ucs.axis_z;
	BOOL bBeforVersion86=TRUE;	//LMA86版本以前 wht 10-08-27
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000031)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000086)
#elif defined(__TDA_FILE_)
	if(true)
#else
	if(version==0||version>=1000014)
#endif*/
	if(version==0||(doc_type==1&&version>=4000031)||(doc_type==2&&version>=1000086)||
		(doc_type==4&&version>=1000014)||doc_type==5)
		bBeforVersion86=FALSE;
	int n;
	if(	version==0||
		(doc_type==1&&version>=5010300)||	//TMA V5.1.3.0
		(doc_type==2&&version>=2000600)||	//LMA V2.0.6.0
		(doc_type==4&&version>=1030600)||	//LDS V1.3.6.0
		(doc_type==5&&version>=1030600))	//TDA V1.3.6.0
	{
		buffer.ReadByte(&_cRollProcessType);
		buffer.ReadBooleanThin(&m_bClockwiseCalProfile);
	}
	if(	version==0||
		(doc_type==1&&version>=5010500)||	//TMA V5.1.5.0
		(doc_type==2&&version>=2000800)||	//LMA V2.0.8.0
		(doc_type==4&&version>=1030700)||	//LDS V1.3.7.0
		(doc_type==5&&version>=1030700))	//TDA V1.3.7.0
	{
		buffer.ReadByte(&_ciRollOffsetType);
		buffer.ReadByte(&_ciDeformType);
	}
	buffer.ReadInteger(&n);
	vertex_list.Empty();
	for(int ii=0;ii<n;ii++)
	{
		PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX());
		pVertex->FromBuffer(buffer,version,doc_type);
		/* 以下代码已移至PROFILE_VERTEX::FromBuffer()
		buffer.ReadPoint(pVertex->vertex);
		buffer.ReadInteger(&pVertex->vertex.feature);
		buffer.ReadInteger(&pVertex->vertex.ID);
		if(bPrevDeflateVersion)
		{
			buffer.ReadInteger(&dw);
			pVertex->type=(BYTE)dw;
			buffer.ReadInteger(&dw);
			pVertex->m_bWeldEdge=(dw!=0);
			buffer.ReadInteger(&dw);
			pVertex->m_bRollEdge=(dw!=0);
			buffer.ReadDouble(&pVertex->radius);
			buffer.ReadDouble(&pVertex->sector_angle);
			double ff;
			buffer.ReadDouble(&ff);
			pVertex->manu_space=(short)ff;
		}
		else
		{
			buffer.ReadByte(&pVertex->type);
			buffer.ReadBooleanThin(&pVertex->m_bWeldEdge);
			buffer.ReadBooleanThin(&pVertex->m_bRollEdge);
			if( version==0||doc_type==5||
				(doc_type==1&&version>=4000102)|| //TMA V4.0.1.2
				(doc_type==2&&version>=1000102)|| //LMA V1.0.1.2
				(doc_type==3&&version>=1070302)|| //TSA V1.7.3.2
				(doc_type==4&&version>=1000302))  //LDS V1.0.3.2
			{
				if(pVertex->m_bRollEdge)	//保存卷边偏移量 wht 11-04-02
					buffer.ReadWord(&pVertex->roll_edge_offset_dist);	
			}
			if(pVertex->type==2||pVertex->type==3)	//指定圆弧半径
			{
				buffer.ReadDouble(&pVertex->radius);
				if(!bBeforVersion86)
				{
					buffer.ReadPoint(pVertex->center);
					bool bVersionNeedTransUcs=false;
//#if defined(__TMA_FILE_)||defined(__TMA_)
					if(doc_type==1&&version>0&&version<=4000101&&pVertex->type==2)
						bVersionNeedTransUcs=true;
//#elif defined(__LMA_FILE_)||defined(__LMA_)
					else if(doc_type==2&&version>0&&version<=1000101&&pVertex->type==2)
						bVersionNeedTransUcs=true;
//#elif defined(__LDS_FILE_)||defined(__LDS_)
					else if(doc_type==4&&version>0&&version<=1000301&&pVertex->type==2)
						bVersionNeedTransUcs=true;
//#endif
					if(bVersionNeedTransUcs&&(fabs(pVertex->center.z)>EPS||
						fabs(pVertex->center.x)>1000||fabs(pVertex->center.y)>1000))
					{	//由绝对坐标系转到板相对坐标系
						coord_trans(pVertex->center,ucs,FALSE);
					}
				}
			}
			if(pVertex->type==1)	//指定圆弧夹角
				buffer.ReadDouble(&pVertex->sector_angle);
			buffer.ReadWord(&pVertex->manu_space);
		}
		if(pVertex->type==3)	//椭圆弧
		{
			if(bBeforVersion86)
				buffer.ReadPoint(pVertex->center);
			buffer.ReadPoint(pVertex->column_norm);
		}
		buffer.ReadInteger(&pVertex->local_point_vec);
		buffer.ReadDouble(&pVertex->local_point_y);
		if( version==0||
			(doc_type==1&&version>=4010100)|| //TMA V4.1.1.0
			(doc_type==2&&version>=1010100)|| //LMA V1.1.0.0
			(doc_type==3&&version>=2000008)|| //TSA V2.0.0.8
			(doc_type==4&&version>=1020008)|| //LDS V1.2.0.8
			(doc_type==5&&version>=1020008))  //TDA V1.2.0.8
		{	//记录顶点设计信息,用于更新顶点坐标
			buffer.ReadByte(&pVertex->m_cPosDesignType);
			if(pVertex->m_cPosDesignType>0)
				buffer.ReadInteger(&pVertex->m_hRod);
		}*/
	}
	buffer.ReadString(sNotes,51);
	//钢板螺栓区
	pLsRefList->FromBuffer(buffer,BelongModel(),version,doc_type);
	/*if(!gasketEdgeSpace.initialized)
		gasketEdgeSpaceInitialized=(wiType!=WASHER_PLATE);
	if(!gasketEdgeSpaceInitialized&&pLsRefList->GetNodeNum()>1)
	{	//旧版文件中多孔螺栓垫板缺少垫板边距设计参数，需要自动提取 wjh-2017.12.8
		//但由于螺栓垫板中的螺栓有可能位于垫板之后此刻提取垫板参数有可能出错，故移至最后初始化 wjh-2018.6.11
		if(!IntelliRetrieveGasketEdgeSpace(NULL))
		{	//没有基准角钢时，以最小直径螺栓对应的垫板边距初始化
			int min_d=0;
			for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				if(min_d==0)
					min_d=pLsRef->pBolt->d;
				else if(pLsRef->pBolt->d<min_d)
					min_d=pLsRef->pBolt->d;
			}
			LSSPACEEX space;
			IBoltLibrary::GetLsSpaceEx(min_d,&space);
			gasketEdgeSpace.endSpaceS=gasketEdgeSpace.endSpaceE=gasketEdgeSpace.wingSpace=gasketEdgeSpace.berSpace=(short)space.WasherEdgeSpace;
		}
	}*/
	//关联构件列表
	int i,nn=0;
	buffer.ReadInteger(&nn);
	RELATIVE_OBJECT *pRelaObj=NULL;
	relativeObjs.Empty();
	for(i=0;i<nn;i++)
	{
		pRelaObj=relativeObjs.Add(buffer.ReadInteger());
		if(bPrevDeflateVersion)
		{
			buffer.ReadDword(&dw);
			pRelaObj->mirInfo.axis_flag=(BYTE)dw;
			buffer.ReadInteger(&dw);
			pRelaObj->mirInfo.array_num=(BYTE)dw;
			buffer.ReadPoint(pRelaObj->mirInfo.origin);
			double ff;
			buffer.ReadDouble(&ff);
			pRelaObj->mirInfo.rotate_angle=(short)ff;
		}
		else
		{
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if(bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
	}
	SetModified();
}

//将钢板信息复制到指定Buffer中
BOOL CLDSPlate::CopyPlateToBuffer(CBuffer &buffer)
{
	//基本信息
	buffer.Write(cfgword.flag.word,24);
	buffer.WriteDword(dwPermission);
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	buffer.WriteString(layer());
	buffer.WriteInteger(iSeg);
	buffer.WriteInteger(face_N);
	buffer.WriteDouble(m_fNormOffset);
	buffer.WritePoint(m_tInnerColumnNorm);
	buffer.WriteDouble(m_fInnerRadius);
	buffer.WritePoint(m_tInnerOrigin);
	buffer.WriteInteger(m_bWeldPart);
	buffer.WriteInteger(m_bNeedFillet);
	buffer.WriteInteger(neutrosphere);
	buffer.WriteInteger(jdb_style);
	buffer.WriteByte(cMaterial);
	buffer.WriteByte(m_cQualityLevel);
	buffer.WriteInteger(m_bReplPart);
	buffer.WriteByte(org_material);
	buffer.WriteString(sPartNo);
	buffer.WriteInteger(ftoi(GetThick()));
	buffer.WriteBooleanThin(m_bDisplayHole);
	buffer.WriteInteger(m_bAttachPartByWeldRoad);	//是否以焊道线为基准
	buffer.WritePoint(HuoQuLine[0].startPt);
	buffer.WritePoint(HuoQuLine[0].endPt);
	buffer.WritePoint(HuoQuLine[1].startPt);
	buffer.WritePoint(HuoQuLine[1].endPt);
	buffer.WriteInteger(HuoQuLine[0].feature);
	buffer.WriteInteger(HuoQuLine[1].feature);
	buffer.Write(HuoQuLine[0].layer,16);
	buffer.Write(HuoQuLine[1].layer,16);
	buffer.WritePoint(top_point);
	buffer.WritePoint(huoqufacenorm[0]);
	buffer.WritePoint(huoqufacenorm[1]);
	buffer.WriteBooleanThin(m_bUserSpecColor);
	if(m_bUserSpecColor)
		buffer.WriteDword(crMaterial);
	//坐标系定义
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	buffer.WritePoint(ucs.origin);
	//连板柔性设计参数
	designInfo.ToBuffer(buffer);
	buffer.WriteInteger(vertex_list.GetNodeNum());
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		buffer.WritePoint(pVertex->vertex);
		buffer.WriteInteger(pVertex->vertex.feature);
		buffer.WriteInteger(pVertex->vertex.ID);
		buffer.WriteByte(pVertex->type);
		buffer.WriteBooleanThin(pVertex->m_bWeldEdge);
		buffer.WriteBooleanThin(pVertex->m_bRollEdge);
		if(pVertex->type==2||pVertex->type==3)
			buffer.WriteDouble(pVertex->radius);
		if(pVertex->type==1)
			buffer.WriteDouble(pVertex->sector_angle);
		buffer.WriteWord(pVertex->manu_space);
		if(pVertex->type==3)	//椭圆弧
		{
			buffer.WritePoint(pVertex->center);
			buffer.WritePoint(pVertex->column_norm);
		}
		//对活点信息
		buffer.WriteInteger(pVertex->local_point_vec);
		buffer.WriteDouble(pVertex->local_point_y);
	}
	buffer.WriteString(sNotes);
	//螺栓信息
	CLsRef *pLsRef=NULL;
	buffer.WriteInteger(pLsRefList->GetNodeNum());	
	for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		(*pLsRef)->ToBuffer(buffer);
	return TRUE;	
}

//将缓存中的信息写到钢板中
BOOL CLDSPlate::PastePlateFromBuffer(CBuffer &buffer,CLDSNode *pDatumNode)
{
	if(pDatumNode==NULL)
		return FALSE;
	CLDSLinePart *pLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pDatumNode->hFatherPart,CLS_LINEPART);
	if(pLinePart==NULL)
		return FALSE;
	if(pLinePart->GetClassTypeId()!=CLS_LINEANGLE&&pLinePart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
		return FALSE;
	//基本信息
	buffer.Read(cfgword.flag.word,24);
	buffer.ReadDword(&dwPermission);
	buffer.ReadInteger(&m_uStatMatCoef);
	buffer.ReadInteger(&_hPartWeldParent);
	buffer.ReadString(layer(),4);
	buffer.ReadInteger(&_iSeg.iSeg);
	buffer.ReadInteger(&_face_N);
	buffer.ReadDouble(&m_fNormOffset);
	buffer.ReadPoint(m_tInnerColumnNorm);
	buffer.ReadDouble(&_fInnerRadius);
	buffer.ReadPoint(m_tInnerOrigin);
	buffer.ReadInteger(&m_bWeldPart);
	buffer.ReadInteger(&m_bNeedFillet);
	buffer.ReadInteger(&neutrosphere);
	buffer.ReadInteger(&style.iLegacyStyle);
	buffer.ReadByte(&_material);
	buffer.ReadByte(&_cQualityLevel);
	buffer.ReadInteger(&m_bReplPart);
	buffer.ReadByte(&org_material);
	buffer.ReadString(sPartNo);
	//long thick;
	buffer.ReadInteger(&_thick);
	//set_thick(thick);
	buffer.ReadBooleanThin(&m_bDisplayHole);
	buffer.ReadInteger(&m_bAttachPartByWeldRoad);	//是否以焊道线为基准
	buffer.ReadPoint(HuoQuLine[0].startPt);
	buffer.ReadPoint(HuoQuLine[0].endPt);
	buffer.ReadPoint(HuoQuLine[1].startPt);
	buffer.ReadPoint(HuoQuLine[1].endPt);
	buffer.ReadInteger(&HuoQuLine[0].feature);
	buffer.ReadInteger(&HuoQuLine[1].feature);
	buffer.Read(HuoQuLine[0].layer,16);
	buffer.Read(HuoQuLine[1].layer,16);
	buffer.ReadPoint(top_point);
	buffer.ReadPoint(huoqufacenorm[0]);
	buffer.ReadPoint(huoqufacenorm[1]);
	buffer.ReadBooleanThin(&m_bUserSpecColor);
	if(m_bUserSpecColor)
		buffer.ReadDword(&crMaterial);
	//坐标系定义
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	buffer.ReadPoint(ucs.origin);
	UCS_STRU src_ucs=ucs;	//记录复制
	//连板柔性设计参数
	designInfo.FromBuffer(buffer);
	long hSrcDatumNode=0,hSrcDatumPole=0;
	hSrcDatumNode=designInfo.m_hBaseNode;
	hSrcDatumPole=designInfo.m_hBasePart;
	CLDSLinePart *pSrcLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(hSrcDatumPole,CLS_LINEPART);
	if(pSrcLinePart==NULL)
		return FALSE;	//复制的源钢板无基准构件
	if(pSrcLinePart->GetClassTypeId()!=pLinePart->GetClassTypeId())
		return FALSE;	//源钢板与要生成的钢板基准杆件类型不同
	//根据基准点更新钢板坐标系
	ucs.origin=pDatumNode->Position(true);
	SnapPerp(&ucs.origin,pLinePart->Start(),pLinePart->End(),pDatumNode->Position(true));
	designInfo.m_hBaseNode=pDatumNode->handle;
	designInfo.origin.des_para.hNode=pDatumNode->handle;
	designInfo.m_hBasePart=pDatumNode->hFatherPart;
	designInfo.origin.des_para.hPart=pDatumNode->hFatherPart;
	if(designInfo.origin.datum_pos_style==1)
		designInfo.origin.des_para.RODEND.hRod=designInfo.m_hBasePart;
	else if(designInfo.origin.datum_pos_style==2)
	{
		designInfo.origin.des_para.RODNODE.hRod=designInfo.m_hBasePart;
		designInfo.origin.des_para.RODNODE.hNode=designInfo.m_hBaseNode;
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSGroupLineAngle *pGroupJg=NULL,*pSrcGroupJg=NULL;
	if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		pGroupJg=(CLDSGroupLineAngle*)pLinePart;
		pSrcGroupJg=(CLDSGroupLineAngle*)pSrcLinePart;
	}
	if(pGroupJg&&pSrcGroupJg)
	{	//基准构件为组合角钢
		//更新法线设计信息
		designInfo.norm.hVicePart=pGroupJg->handle;
		designInfo.norm.UpdateVector(BelongModel());
		ucs.axis_z=designInfo.norm.vector;
		//更新射线杆件列表
		if(designInfo.partList.GetNodeNum()>0)
		{
			CDesignLjPartPara *pLjPara=NULL;
			for(pLjPara=designInfo.partList.GetFirst();pLjPara;pLjPara=designInfo.partList.GetNext())
			{
				for(int i=0;i<4;i++)
				{
					if(pSrcGroupJg->group_style<2&&i>=2)
						continue;	//个别四角钢转换为双拼角钢时，会把第三、四根子角钢转换为虚角钢存在 wht 11-08-05
					if(pLjPara->hPart==pSrcGroupJg->son_jg_h[i])
					{
						pLjPara->hPart=pGroupJg->son_jg_h[i];
						break;
					}
				}
			}
			DesignSetupUcs();	//重新设计钢板坐标系
		}
		else
		{	//组合角钢填板
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupJg->m_hDatumSonAngle ,CLS_LINEANGLE);
			SnapPerp(&ucs.origin,pLineAngle->Start(),pLineAngle->End(),pDatumNode->Position(true));
			ucs.axis_x=pLinePart->End()-pLinePart->Start();
			normalize(ucs.axis_x);
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_y);
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
		}
	}
	else
#endif
	{
		designInfo.norm.hVicePart=pDatumNode->hFatherPart;
		DesignSetupUcs();	//重新设计钢板坐标系
	}
	if(designInfo.origin.datum_pos_style==0)
		designInfo.origin.SetPosition(ucs.origin);
	if(designInfo.norm.norm_style==0)
		designInfo.norm.vector=ucs.axis_z;
	int n;
	buffer.ReadInteger(&n);
	vertex_list.Empty();
	for(int ii=0;ii<n;ii++)
	{
		f3dPoint vertex;
		buffer.ReadPoint(vertex);
		PROFILE_VERTEX *pVertex=vertex_list.append(PROFILE_VERTEX(vertex));
		buffer.ReadInteger(&pVertex->vertex.feature);
		buffer.ReadInteger(&pVertex->vertex.ID);
		buffer.ReadByte(&pVertex->type);
		buffer.ReadBooleanThin(&pVertex->m_bWeldEdge);
		buffer.ReadBooleanThin(&pVertex->m_bRollEdge);
		if(pVertex->type==2||pVertex->type==3)
			buffer.ReadDouble(&pVertex->radius);
		if(pVertex->type==1)
			buffer.ReadDouble(&pVertex->sector_angle);
		buffer.ReadWord(&pVertex->manu_space);
		if(pVertex->type==3)	//椭圆弧
		{
			buffer.ReadPoint(pVertex->center);
			buffer.ReadPoint(pVertex->column_norm);
		}
		buffer.ReadInteger(&pVertex->local_point_vec);
		buffer.ReadDouble(&pVertex->local_point_y);
	}
	buffer.ReadString(sNotes,51);
	//螺栓信息
	int i,nLsCount=0;
	buffer.ReadInteger(&nLsCount);
	for(i=0;i<nLsCount;i++)
	{
		CLDSBolt *pBolt=(CLDSBolt*)BelongModel()->AppendPart(CLS_BOLT);
		pBolt->FromBuffer(buffer);
		f3dPoint ls_pos=pBolt->ucs.origin;
		coord_trans(ls_pos,src_ucs,FALSE);
		//指定构件上的相对坐标
		pBolt->des_base_pos.datumPoint.datum_pos_style=9;//基准构件上的相对坐标
		pBolt->des_base_pos.datumPoint.datum_pos=ls_pos;
		pBolt->des_base_pos.datumPoint.des_para.hPart=handle;
		coord_trans(ls_pos,ucs,TRUE);
		pBolt->ucs.origin=ls_pos;
		//螺栓定位参数
		pBolt->des_base_pos.datumPoint.SetPosition(pBolt->ucs.origin);
		pBolt->des_base_pos.hPart=0;
		pBolt->des_base_pos.offset_wing=0;
		pBolt->des_base_pos.wing_offset_dist=0;
		pBolt->des_base_pos.len_offset_dist=0;
		//螺栓法线方向
		pBolt->des_work_norm.norm_style=4;	//基准构件上的相对方向
		pBolt->des_work_norm.nearVector=f3dPoint(0,0,1);
		pBolt->des_work_norm.hVicePart=handle;
		pBolt->des_work_norm.vector=ucs.axis_z;
		pBolt->correct_worknorm();
		//将LS引入到相应构件
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
			pLinePart->AppendMidLsRef(pBolt->GetLsRef());
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupLineAngle=(CLDSGroupLineAngle*)pLinePart;
			for(int i=0;i<4;i++)
			{
				if(pGroupLineAngle->group_style<2&&i>=2)
					continue;	//个别四角钢转换为双拼角钢时，会把第三、四根子角钢转换为虚角钢存在 wht 11-08-05
				CLDSLineAngle *pSonLineAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupLineAngle->son_jg_h[i],CLS_LINEANGLE);
				if(pSonLineAngle==NULL)
					continue;
				if(pSonLineAngle->IsLsInWing(pBolt))
					pSonLineAngle->AppendMidLsRef(pBolt->GetLsRef());
			}
		}
#endif
		AppendLsRef(pBolt->GetLsRef());
	}
	return TRUE;
}

double CLDSPlate::GetRollEdgeHeight()
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->GetRollEdgeHeight();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->type==0&&pVertex->m_bRollEdge)
			return pVertex->manu_space;
	}
	return 0;
}

BOOL CLDSPlate::AssertValid()
{
	if(handle<0x20)
		return FALSE;
	else
		return TRUE;
}

IMPLEMENT_GET(CLDSPlate,jdb_style){
	CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
	if(pMotherObj!=NULL&&pMotherObj!=this)
		return pMotherObj->jdb_style;
	else
		return style.wiLegacyStyle;
}
IMPLEMENT_SET(CLDSPlate,jdb_style){
	if(value>=0)
	{
		CLDSPlate* pMotherObj=(CLDSPlate*)MotherObject();
		style.wiLegacyStyle = (WORD)value;
		if(style.wiLegacyStyle==4)	//螺栓垫板
			style.wiType=CLDSPlate::WASHER_PLATE;
		SetModified();
		if(pMotherObj!=NULL&&pMotherObj!=this)
			pMotherObj->jdb_style=value;
	}
}
WORD CLDSGeneralPlate::GetLegacyStyle()
{
	if(GetClassTypeId()==CLS_PARAMPLATE)
		return 10;	//参数化钢板
	else
		return style.wiLegacyStyle;
}
void CLDSGeneralPlate::SetLegacyStyle(WORD wiLegacyStyle)
{
	if(GetClassTypeId()==CLS_PLATE)
		style.wiLegacyStyle=wiLegacyStyle;
}
WORD CLDSGeneralPlate::InternalGetDesignType()
{	//该函数纯粹是因为解决不能直接get=派生虚函数导致 wjh-2017.1.20
	if(GetClassTypeId()==CLS_PARAMPLATE)
		return ((CLDSParamPlate*)this)->m_iParamType;
	else
		return style.wiType;
}
void CLDSGeneralPlate::InternalSetDesignType(WORD wiDesignType)
{
	style.wiType=wiDesignType;
	if(GetClassTypeId()==CLS_PARAMPLATE&&wiDesignType<=TYPE_STATIONNODEPLATE)
		((CLDSParamPlate*)this)->m_iParamType=style.wiType;
	if(style.wiType==WASHER_PLATE)
		style.wiLegacyStyle=4;
}
static bool ValidateWrapPlate(CLDSPlate* pPlate)
{
	if( pPlate->designInfo.partList.GetNodeNum()>0||pPlate->Thick<6||
		pPlate->face_N!=1||pPlate->vertex_list.GetNodeNum()!=4)
		return false;
	PROFILE_VERTEX* pVertex,*pPrevVertex=pPlate->vertex_list.GetTail();
	char cOrthAxis=0;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		if(fabs(pPrevVertex->vertex.x-pVertex->vertex.x)<0.1&&cOrthAxis!='Y')
			cOrthAxis='Y';
		else if(fabs(pPrevVertex->vertex.y-pVertex->vertex.y)<0.1&&cOrthAxis!='X')
			cOrthAxis='X';
		else
			return false;
	}
	return true;
}
bool IntelliRetrieveWrapperPlateParms(CLDSPlate* pPlate)
{
	if(pPlate->wiType!=CLDSPlate::WRAPPER_PLATE)
		return false;
	//提取包钢板宽度和高度
	CMaxDouble maxX,maxY,maxBoltX,maxBoltY;
	CMinDouble minX,minY,minBoltX,minBoltY;
	for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		maxX.Update(pVertex->vertex.x);
		maxY.Update(pVertex->vertex.y);
		minX.Update(pVertex->vertex.x);
		minY.Update(pVertex->vertex.y);
	}
	for(CLsRef* pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		GEPOINT inters;
		if(!pPlate->GetBoltIntersPos(pLsRef->pBolt,inters))
			continue;
		coord_trans(inters,pPlate->ucs,FALSE);
		maxBoltX.Update(inters.x);
		maxBoltY.Update(inters.y);
		minBoltX.Update(inters.x);
		minBoltY.Update(inters.y);
	}
	DESIGN_PLATE_ITEM* pParam;
	pParam=pPlate->params.GetValue(KEY2C("LM"));
	if(pParam==NULL)
	{
		pParam=pPlate->params.Add(KEY2C("LM"));
		pParam->paramType=PARAM_SHAPE;//1;
		pParam->dataType=0;	//TYPE_DOUBLE
		pParam->value.fValue=ftoi(minBoltX.number-minX.number);
		pParam->sNotes.Copy("左边距");

	}
	pParam=pPlate->params.GetValue(KEY2C("RM"));
	if(pParam==NULL)
	{
		pParam=pPlate->params.Add(KEY2C("RM"));
		pParam->paramType=PARAM_SHAPE;//1;
		pParam->dataType=0;	//TYPE_DOUBLE
		pParam->value.fValue=ftoi(maxX.number-maxBoltX.number);
		pParam->sNotes.Copy("右边距");
	}
	pParam=pPlate->params.GetValue(KEY2C("TM"));
	if(pParam==NULL)
	{
		pParam=pPlate->params.Add(KEY2C("TM"));
		pParam->paramType=PARAM_SHAPE;//1;
		pParam->dataType=0;	//TYPE_DOUBLE
		pParam->value.fValue=ftoi(maxY.number-maxBoltY.number);
		pParam->sNotes.Copy("上边距");

	}
	pParam=pPlate->params.GetValue(KEY2C("BM"));
	if(pParam==NULL)
	{
		pParam=pPlate->params.Add(KEY2C("BM"));
		pParam->paramType=PARAM_SHAPE;//1;
		pParam->dataType=0;	//TYPE_DOUBLE
		pParam->value.fValue=ftoi(minBoltY.number-minY.number);
		pParam->sNotes.Copy("下边距");
	}
	pPlate->wiType=CLDSPlate::WRAPPER_PLATE;
	return true;
}
WORD CLDSPlate::InternalGetDesignType()		//该函数纯粹是因为解决不能直接get=派生虚函数导致 wjh-2017.1.20
{
	if(style.wiType>=CONNECT_PLATE)
		return style.wiType;	//已初始化普通钢板的设计类型
	//未初始化时则自动提取
	//新0.表示普通板 1.折叠板 2.环向板 3.钢管夹板 4.螺栓垫板 5.相对于基准构件位置不变的附加板 6.共用板
	if(style.wiLegacyStyle==4)
		return style.wiType=WASHER_PLATE;
	if(ValidateWrapPlate(this))
		return style.wiLegacyStyle=WRAPPER_PLATE;
	if(style.wiLegacyStyle<=1||style.wiLegacyStyle==6)
		return style.wiType=CONNECT_PLATE;
	bool hasConnBolt=false;
	int nPush=pLsRefList->push_stack();
	for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		if(!pLsRef->pBolt->m_bVirtualPart)
			hasConnBolt=true;
		if(hasConnBolt)
			break;
	}
	pLsRefList->pop_stack(nPush);
	if(hasConnBolt)
		return CONNECT_PLATE;
	return ELSE_PLATE;
}
void CLDSPlate::InternalSetDesignType(WORD wiDesignType)
{
	CLDSGeneralPlate::InternalSetDesignType(wiDesignType);
}
BOOL ValidateWrapperPlateEdgeSpace(CLDSPlate* pWrapPlate,double epsilon/*=1.0*/,CLDSPlate::EDGEBOLT *endBoltInfoArr/*=NULL*/,int *count/*=NULL*/)
{
	CMaxDouble maxBoltX,maxBoltY;
	CMinDouble minBoltX,minBoltY;
	GEPOINT left,right,top,bottom;
	if(count)
		*count=0;
	for(CLsRef* pLsRef=pWrapPlate->GetFirstLsRef();pLsRef;pLsRef=pWrapPlate->GetNextLsRef())
	{
		GEPOINT inters;
		if(!pWrapPlate->GetBoltIntersPos(pLsRef->pBolt,inters))
			continue;
		coord_trans(inters,pWrapPlate->ucs,FALSE);
		if(fabs(maxBoltX.Update(inters.x,pLsRef->pBolt)-inters.x)<EPS)
			right=inters;
		if(fabs(maxBoltY.Update(inters.y,pLsRef->pBolt)-inters.y)<EPS)
			top=inters;
		if(fabs(minBoltX.Update(inters.x,pLsRef->pBolt)-inters.x)<EPS)
			left=inters;
		if(fabs(minBoltY.Update(inters.y,pLsRef->pBolt)-inters.y)<EPS)
			bottom=inters;
	}
	CLDSBolt* pLeftBolt =(CLDSBolt*)minBoltX.m_pRelaObj;
	CLDSBolt* pRightBolt=(CLDSBolt*)maxBoltX.m_pRelaObj;
	CLDSBolt* pTopBolt=(CLDSBolt*)maxBoltX.m_pRelaObj;
	CLDSBolt* pBtmBolt=(CLDSBolt*)minBoltY.m_pRelaObj;
	LSSPACEEX LsSpace;
	DESIGN_PLATE_ITEM* pParam;
	struct EDGESPACE{
		int space,margin;
		EDGESPACE(int _space=30){
			space=_space;
			margin=0;
		}
	}leftSpace,rightSpace,topSpace,btmSpace;
	if((pParam=pWrapPlate->params.GetValue(KEY2C("LM")))!=NULL)
		leftSpace.margin=pParam->ToLong();
	if(IBoltLibrary::GetLsSpaceEx(pLeftBolt->get_d(),&LsSpace))
		leftSpace.space=LsSpace.EndSpace;
	if((pParam=pWrapPlate->params.GetValue(KEY2C("RM")))!=NULL)
		rightSpace.margin=pParam->ToLong();
	if(IBoltLibrary::GetLsSpaceEx(pRightBolt->get_d(),&LsSpace))
		rightSpace.space=LsSpace.EndSpace;
	if((pParam=pWrapPlate->params.GetValue(KEY2C("TM")))!=NULL)
		topSpace.margin=pParam->ToLong();
	if(IBoltLibrary::GetLsSpaceEx(pTopBolt->get_d(),&LsSpace))
		topSpace.space=LsSpace.EndSpace;
	if((pParam=pWrapPlate->params.GetValue(KEY2C("BM")))!=NULL)
		btmSpace.margin=pParam->ToLong();
	if(IBoltLibrary::GetLsSpaceEx(pBtmBolt->get_d(),&LsSpace))
		btmSpace.space=LsSpace.EndSpace;
	//提取包钢板宽度和高度
	//验证左边距
	PROFILE_VERTEX *pVertex,*pPrevVertex=pWrapPlate->vertex_list.GetTail();
	for(pVertex=pWrapPlate->vertex_list.GetFirst();pVertex;pPrevVertex=pVertex,pVertex=pWrapPlate->vertex_list.GetNext())
	{
		//1.验证左边距
		if(leftSpace.margin>0&&fabs(pPrevVertex->vertex.x-pVertex->vertex.x)<EPS&&left.x-pPrevVertex->vertex.x>0)
		{	//检验左边距
			if(fabs(left.x-pPrevVertex->vertex.x-leftSpace.margin)>epsilon)
			{	//左边距不符
				if(endBoltInfoArr&&count)
				{
					endBoltInfoArr[*count].hBolt=pLeftBolt->handle;
					endBoltInfoArr[*count].needEndSpace=leftSpace.margin;
					endBoltInfoArr[*count].actualEndSpace=left.x-pPrevVertex->vertex.x;
					(*count)++;
				}
				else
					return FALSE;
			}
		}
		else
		{
			double dist=DistOf2dPtLine(left.x,left.y,pPrevVertex->vertex.x,pPrevVertex->vertex.y,pVertex->vertex.x,pVertex->vertex.y);
			if(ftoi(dist)<leftSpace.space)
			{	//左边距小于钢板常规螺栓边距
				if(endBoltInfoArr&&count)
				{
					endBoltInfoArr[*count].hBolt=pLeftBolt->handle;
					endBoltInfoArr[*count].needEndSpace=leftSpace.space;
					endBoltInfoArr[*count].actualEndSpace=dist;
					(*count)++;
				}
				else
					return FALSE;
			}
		}
		//2.验证右边距
		if(rightSpace.margin>0&&fabs(pPrevVertex->vertex.x-pVertex->vertex.x)<EPS&&right.x-pPrevVertex->vertex.x<0)
		{	//检验左边距
			if(fabs(pPrevVertex->vertex.x-right.x-rightSpace.margin)>epsilon)
			{	//左边距不符
				if(endBoltInfoArr&&count)
				{
					endBoltInfoArr[*count].hBolt=pRightBolt->handle;
					endBoltInfoArr[*count].needEndSpace=rightSpace.margin;
					endBoltInfoArr[*count].actualEndSpace=pPrevVertex->vertex.x-right.x;
					(*count)++;
				}
				else
					return FALSE;
			}
		}
		else
		{
			double dist=DistOf2dPtLine(right.x,right.y,pPrevVertex->vertex.x,pPrevVertex->vertex.y,pVertex->vertex.x,pVertex->vertex.y);
			if(ftoi(dist)<rightSpace.space)
			{	//左边距小于钢板常规螺栓边距
				if(endBoltInfoArr&&count)
				{
					endBoltInfoArr[*count].hBolt=pRightBolt->handle;
					endBoltInfoArr[*count].needEndSpace=rightSpace.space;
					endBoltInfoArr[*count].actualEndSpace=dist;
					(*count)++;
				}
				else
					return FALSE;
			}
		}
		//3.验证上边距
		if(topSpace.margin>0&&fabs(pPrevVertex->vertex.y-pVertex->vertex.y)<EPS&&top.y-pPrevVertex->vertex.y<0)
		{	//检验左边距
			if(fabs(pPrevVertex->vertex.y-top.y-topSpace.margin)>epsilon)
			{	//上边距不符
				if(endBoltInfoArr&&count)
				{
					endBoltInfoArr[*count].hBolt=pTopBolt->handle;
					endBoltInfoArr[*count].needEndSpace=topSpace.margin;
					endBoltInfoArr[*count].actualEndSpace=pPrevVertex->vertex.y-top.y;
					(*count)++;
				}
				else
					return FALSE;
			}
		}
		else
		{
			double dist=DistOf2dPtLine(top.x,top.y,pPrevVertex->vertex.x,pPrevVertex->vertex.y,pVertex->vertex.x,pVertex->vertex.y);
			if(ftoi(dist)<topSpace.space)
			{	//左边距小于钢板常规螺栓边距
				if(endBoltInfoArr&&count)
				{
					endBoltInfoArr[*count].hBolt=pTopBolt->handle;
					endBoltInfoArr[*count].needEndSpace=topSpace.space;
					endBoltInfoArr[*count].actualEndSpace=dist;
					(*count)++;
				}
				else
					return FALSE;
			}
		}
		//4.验证下边距
		if(btmSpace.margin>0&&fabs(pPrevVertex->vertex.y-pVertex->vertex.y)<EPS&&bottom.y-pPrevVertex->vertex.y>0)
		{	//检验下边距
			if(fabs(bottom.y-pPrevVertex->vertex.y-btmSpace.margin)>epsilon)
			{	//下边距不符
				if(endBoltInfoArr&&count)
				{
					endBoltInfoArr[*count].hBolt=pBtmBolt->handle;
					endBoltInfoArr[*count].needEndSpace=btmSpace.margin;
					endBoltInfoArr[*count].actualEndSpace=top.y-pPrevVertex->vertex.y;
					(*count)++;
				}
				else
					return FALSE;
			}
		}
		else
		{
			double dist=DistOf2dPtLine(bottom.x,bottom.y,pPrevVertex->vertex.x,pPrevVertex->vertex.y,pVertex->vertex.x,pVertex->vertex.y);
			if(ftoi(dist)<btmSpace.space)
			{	//下边距小于钢板常规螺栓边距
				if(endBoltInfoArr&&count)
				{
					endBoltInfoArr[*count].hBolt=pBtmBolt->handle;
					endBoltInfoArr[*count].needEndSpace=btmSpace.space;
					endBoltInfoArr[*count].actualEndSpace=dist;
					(*count)++;
				}
				else
					return FALSE;
			}
		}
	}
	if(endBoltInfoArr&&count&&*count>0)
		return FALSE;
	else
		return TRUE;	//边距合法
}
BOOL ValidateWasherPlateEdgeSpace(CLDSPlate* pWasherPlate,double epsilon/*=1.0*/,CLDSPlate::EDGEBOLT *endBoltInfoArr/*=NULL*/,int *count/*=NULL*/)
{
	CMaxDouble maxBoltX,maxBoltY;
	CMinDouble minBoltX,minBoltY;
	if(count)
		*count=0;
	for(CLsRef* pLsRef=pWasherPlate->GetFirstLsRef();pLsRef;pLsRef=pWasherPlate->GetNextLsRef())
	{
		GEPOINT inters;
		if(!pWasherPlate->GetBoltIntersPos(pLsRef->pBolt,inters))
			continue;
		coord_trans(inters,pWasherPlate->ucs,FALSE);
		//验证垫板边距
		LSSPACEEX LsSpace;
		if(!IBoltLibrary::GetLsSpaceEx(pLsRef->pBolt->get_d(),&LsSpace))
		{
			if(CLDSPlate::log2file)
				CLDSPlate::log2file->Log("0x%x螺栓的直径为%dmm, 而螺栓间距库中找不到相应的记录",pLsRef->pBolt->handle,pLsRef->pBolt->get_d());
			continue;
		}
		PROFILE_VERTEX *pVertex,*pPrevVertex=pWasherPlate->vertex_list.GetTail();
		for(pVertex=pWasherPlate->vertex_list.GetFirst();pVertex;pPrevVertex=pVertex,pVertex=pWasherPlate->vertex_list.GetNext())
		{
			double dist=DistOf2dPtLine(inters.x,inters.y,pPrevVertex->vertex.x,pPrevVertex->vertex.y,pVertex->vertex.x,pVertex->vertex.y);
			if(dist<LsSpace.WasherEdgeSpace)
			{	//左边距小于钢板常规螺栓边距
				if(endBoltInfoArr&&count)
				{
					endBoltInfoArr[*count].hBolt=pLsRef->pBolt->handle;
					endBoltInfoArr[*count].needEndSpace=LsSpace.WasherEdgeSpace;
					endBoltInfoArr[*count].actualEndSpace=dist;
					(*count)++;
					break;
				}
				else
					return FALSE;
			}
		}
	}
	if(endBoltInfoArr&&count&&*count>0)
		return FALSE;
	else
		return TRUE;	//边距合法
}
BOOL CLDSGeneralPlate::ValidateProfile(double epsilon/*=1.0*/,EDGEBOLT *endBoltInfoArr/*=NULL*/,
							   int *count/*=NULL*/,BYTE cbValidateFlag/*=0xff*/)
{
	if(vertex_list.GetNodeNum()<3||GetLsRefList()->GetLsCount()==0||m_bVirtualPart)
		return TRUE;
	int i,n=0;
	f3dPoint *pnt_arr;
	n=vertex_list.GetNodeNum();
	CArray<PROFILE_VERTEX,PROFILE_VERTEX&>vertexArr;
	if(GetFaceN()!=3||IsFoldPlate())
	{
		i=0;
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->type>0)
				n++;	//圆弧边多添加一个点
		}
		vertexArr.SetSize(n);
		pnt_arr=new f3dPoint[n];
		PROFILE_VERTEX *pPrevVertex=vertex_list.GetTail();
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pPrevVertex->type>0)
			{
				f3dArcLine arcLine;
				pPrevVertex->RetrieveArcLine(arcLine,pVertex->vertex,&ucs);
				pnt_arr[i]=arcLine.PositionInAngle(arcLine.SectorAngle()*0.5);
				vertexArr[i]=*pVertex;
				i++;
			}
			pnt_arr[i].Set(pVertex->vertex.x,pVertex->vertex.y);
			vertexArr[i]=*pVertex;
			i++;
			pPrevVertex=pVertex;
		}
	}
	else	//带顶点的三面板（即二面与三面相交）
	{
		i=0;
		vertexArr.SetSize(n+1);
		pnt_arr=new f3dPoint[n+1];	//多一个剪切顶点
		CLDSPlate* pCommPlate=NULL;
		if(GetClassTypeId()==CLS_PLATE)
			pCommPlate=(CLDSPlate*)this;
		for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(i>0&&pnt_arr[i].feature==3&&pVertex->vertex.feature==2)
			{	//插入三面板剪切顶点
				pnt_arr[i]=pCommPlate->top_point;
				i++;
				n++;
			}
			vertexArr[i]=*pVertex;
			pnt_arr[i].Set(pVertex->vertex.x,pVertex->vertex.y);
			pnt_arr[i].feature=pVertex->vertex.feature;
			i++;
		}
	}
	POLYGON rgn;
	if(rgn.CreatePolygonRgn(pnt_arr,n))
	{
		f3dPoint ls_pos,perp;
		double dist=0;
		int end_space;	//螺栓端距
		LSSPACEEX space;
		if(count)
			*count=0;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(!IBoltLibrary::GetLsSpaceEx((*pLsRef)->get_d(),&space))
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL2_COMMON,"0x%x螺栓的直径为%dmm, 而螺栓间距库中找不到相应的记录",pLsRef->GetLsPtr()->handle,pLsRef->GetLsPtr()->get_d());
			if(SpreadLsPos(pLsRef->GetLsPtr(),ls_pos))
			{
				end_space=(wiType==WASHER_PLATE&&space.WasherEdgeSpace>0)?space.WasherEdgeSpace:space.EndSpace;
				ls_pos.z=0;
				if(rgn.PtInRgn(ls_pos)!=1)
				{
					if(endBoltInfoArr&&count)
					{
						endBoltInfoArr[*count].hBolt=pLsRef->GetLsPtr()->handle;
						endBoltInfoArr[*count].actualEndSpace=0;
						endBoltInfoArr[*count].needEndSpace=0;
						(*count)++;
						//TODO:环向封板螺栓边距校核可能误报
						continue;	//螺栓不在钢板范围内不需要再继续执行下面代码(count大于螺栓总数时会导致endBoltInfoArr越界) wht 17-02-25
					}
					else
					{
						delete []pnt_arr;
						return FALSE;
					}
				}
				for(i=0;i<n;i++)
				{
					if(vertexArr[i].type==0)	//指定夹角
					{
						f3dLine line(pnt_arr[i],pnt_arr[(i+1)%n]);
						if(SnapPerp(&perp,line,ls_pos,&dist))
						{
							int retCode=line.PtInLine(perp);
							if(retCode>0&&dist<end_space-epsilon)	//垂足点在线段上
							{
								if(endBoltInfoArr&&count)
								{
									endBoltInfoArr[*count].hBolt=pLsRef->GetLsPtr()->handle;
									endBoltInfoArr[*count].actualEndSpace=dist;
									endBoltInfoArr[*count].needEndSpace=end_space;
									(*count)++;
									break;
								}
								else
								{
									delete []pnt_arr;
									return FALSE;
								}
							}
							else if(retCode==-1&&DISTANCE(ls_pos,line.endPt)<end_space-epsilon)
							{
								if(endBoltInfoArr&&count)
								{
									endBoltInfoArr[*count].hBolt=pLsRef->GetLsPtr()->handle;
									endBoltInfoArr[*count].actualEndSpace=dist;
									endBoltInfoArr[*count].needEndSpace=end_space;
									(*count)++;
									break;
								}
								else
								{
									delete []pnt_arr;
									return FALSE;
								}
							}
							else if(retCode==-2&&DISTANCE(ls_pos,line.startPt)<end_space-epsilon)
							{
								if(endBoltInfoArr&&count)
								{
									endBoltInfoArr[*count].hBolt=pLsRef->GetLsPtr()->handle;
									endBoltInfoArr[*count].actualEndSpace=dist;
									endBoltInfoArr[*count].needEndSpace=end_space;
									(*count)++;
									break;
								}
								else
								{
									delete []pnt_arr;
									return FALSE;
								}
							}
						}
						/*else	//技术还不成熟不足以判断螺栓到圆弧边的距离是否满足
						{
							f3dArcLine arcline;
							vertexArr[i].RetrieveArcLine(arcline,vertexArr[(i+1)%vertexArr.GetSize()].vertex,&ucs);
							dist=DISTANCE(arcline.Center(),ls_pos);
							if(dist-arcline.Radius()
						}*/
					}
				}
			}
		}
		//检测螺栓孔到火曲线间距的合理性
		PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL;
		PROFILE_VERTEX* pBendLine2Start=NULL,*pBendLine2End=NULL;
		GetBendlineVertexies(&pBendLine1Start,&pBendLine1End,&pBendLine2Start,&pBendLine2End);
		for(int m=1;m<GetFaceN();m++)
		{
			f3dPoint huoqu_start,huoqu_end,perp_pt;
			if(m==1&&pBendLine1Start&&pBendLine1End)
			{
				huoqu_start=pBendLine1Start->vertex;
				huoqu_end  =pBendLine1End->vertex; 
			}
			if(m==2&&pBendLine2Start&&pBendLine2End)
			{
				huoqu_start=pBendLine2Start->vertex;
				huoqu_end  =pBendLine2End->vertex;
			}
			if(huoqu_start.IsZero()&&huoqu_end.IsZero())
				continue;
			f3dLine huo_line(huoqu_start,huoqu_end);
			CMinDouble xMinDist;
			for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				CLDSBolt* pBolt=pLsRef->GetLsPtr();
				SpreadLsPos(pBolt,ls_pos);
				ls_pos.z=0;
				double fDist=0;
				if(!SnapPerp(&perp_pt,huo_line,ls_pos,&fDist)||huo_line.PtInLine(perp_pt)!=2)
					continue;	//垂足不在火曲线上
				xMinDist.Update(fDist,(void*)pBolt,EPS2);
			}
			if(xMinDist.IsInited()&&xMinDist.number<end_space-epsilon&&endBoltInfoArr&&count)
			{	//到火曲距离不满足边距(不是所有制弯面上都有螺栓，比如踏板wjh-2019.9.11)
				endBoltInfoArr[*count].hBolt=((CLDSBolt*)xMinDist.m_pRelaObj)->handle;
				endBoltInfoArr[*count].actualEndSpace=xMinDist.number;
				endBoltInfoArr[*count].needEndSpace=end_space;
				endBoltInfoArr[*count].bToHuoquLine=TRUE;
				(*count)++;
			}
		}
	}
	else
	{
		delete []pnt_arr;
		return FALSE;
	}
	delete []pnt_arr;
	if(endBoltInfoArr&&count&&*count>0)
		return FALSE;
	else
		return TRUE;	//边距合法
}
BOOL CLDSPlate::ValidateProfile(double epsilon/*=1.0*/,EDGEBOLT *endBoltInfoArr/*=NULL*/,
								int *count/*=NULL*/,BYTE cbValidateFlag/*=0xff*/)
{
	return CLDSGeneralPlate::ValidateProfile(epsilon,endBoltInfoArr,count,cbValidateFlag);
}
void CLDSPlate::CopyProperty(CLDSDbObject *pObj)
{
	if(pObj->GetClassTypeId()!=CLS_PLATE)
		return;
	CLDSPlate *pPlate=(CLDSPlate*)pObj;
	_thick=ftoi(pPlate->GetThick());				//板厚
	//sub_type=pPlate->sub_type;				//板的子类型
	face_N=pPlate->face_N;					//面数
	neutrosphere=pPlate->neutrosphere;		//表示火曲时中性层位于几分之一的位置
	m_fNormOffset=pPlate->m_fNormOffset;	//装配时法向位移量，设此参数便于钢管上焊接板的位置设定
	m_fInnerRadius=pPlate->m_fInnerRadius;	//内圆弧半径
	m_tInnerOrigin=pPlate->m_tInnerOrigin;	//内圆环位置
	m_tInnerColumnNorm=pPlate->m_tInnerColumnNorm;//内圆法线方向
	m_bAttachPartByWeldRoad=pPlate->m_bAttachPartByWeldRoad;	//主要用于钢管塔脚圆底板上的地脚螺栓布置与对称 wjh-2017.6.26
	m_bReplPart=pPlate->m_bReplPart;		//代用构件
	m_bWeldPart=pPlate->m_bWeldPart;		//焊接件
	m_bNeedFillet=pPlate->m_bNeedFillet;	//需要坡口
	m_iLsArrayStyle=pPlate->m_iLsArrayStyle;////钢板上螺栓的阵列方式 0-矩形阵列 非0-环形阵列 	
	org_material=pPlate->org_material;
	jdb_style=pPlate->jdb_style;
	cMaterial=pPlate->cMaterial;
	cbVirginQualityLevel = pPlate->cbVirginQualityLevel;
	CopyModuleInstanceInfo(pPlate);
	iSeg=pPlate->iSeg;
	m_uStatMatCoef=pPlate->m_uStatMatCoef;
	m_bDisplayHole=pPlate->m_bDisplayHole;
	strcpy(sPartNo,pPlate->sPartNo);
	strcpy(sNotes,pPlate->sNotes);
	m_bUserSpecColor=pPlate->m_bUserSpecColor;
	crMaterial=pPlate->crMaterial;
	cDatumAxis=pPlate->cDatumAxis;
	if(wiType==CLDSGeneralPlate::WASHER_PLATE)
	{
		gasketEdgeSpace.endSpaceS=pPlate->gasketEdgeSpace.endSpaceS;
		gasketEdgeSpace.endSpaceE=pPlate->gasketEdgeSpace.endSpaceE;
		gasketEdgeSpace.wingSpace=pPlate->gasketEdgeSpace.wingSpace;
		gasketEdgeSpace.berSpace=pPlate->gasketEdgeSpace.berSpace;
	}
}

int CLDSPlate::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->GetSpecification(spec,bIncMat);
	char steelmark[20]="";
	char tmp_spec[200];
	if(bIncMat&&cMaterial!='S')
		QuerySteelMatMark(cMaterial,steelmark);
	sprintf(tmp_spec,"%s-%d",steelmark,_thick);
	SimplifiedNumString(tmp_spec);
	if(spec)
		strcpy(spec,tmp_spec);
	return strlen(tmp_spec);
}
int CLDSPlate::m_iPlankWeightStatType=1;	//最小包容矩形统计
//BYTE CLDSPlate::m_cRollProcessType=0;		//最佳受力
short CLDSPlate::m_siProfileAlgor=1;		//0.传统外形计算算法;1.新改进包络外形计算算法
double CLDSPlate::GetWidth()
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->GetWidth();
	f2dRect rect;
	if(m_iPlankWeightStatType==0)
		rect=GetWrapRect();
	else if(m_iPlankWeightStatType==2)
	{
		rect=GetMinWrapRect();
		double netarea=CalAreaOfNetWrap();
		double width=rect.Width();
		double height=rect.Height();
		if(width>height)
		{	//宽>高，宽边为矩形长
			width=netarea/height;
			rect.bottomRight.x=rect.topLeft.x+width;
		}
		else
		{	//宽<高，高边为矩形长
			height=netarea/width;
			rect.bottomRight.y=rect.topLeft.y+height;
		}
	}
	else
		rect=GetMinWrapRect();
	if(rect.Width()<rect.Height())
		return rect.Width();
	else
		return rect.Height();
}
double CLDSPlate::GetLength()
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->GetLength();
	f2dRect rect;
	if(m_iPlankWeightStatType==0)
		rect=GetWrapRect();
	else if(m_iPlankWeightStatType==2)
	{
		rect=GetMinWrapRect();
		double netarea=CalAreaOfNetWrap();
		double width=rect.Width();
		double height=rect.Height();
		if(width>height)
		{	//宽>高，宽边为矩形长
			width=netarea/height;
			rect.bottomRight.x=rect.topLeft.x+width;
		}
		else
		{	//宽<高，高边为矩形长
			height=netarea/width;
			rect.bottomRight.y=rect.topLeft.y+height;
		}
	}
	else
		rect=GetMinWrapRect();
	if(rect.Width()<rect.Height())
		return rect.Height();
	else
		return rect.Width();
}
double CLDSPlate::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->GetWeight();
	//板的重量根据文件属性中设置的连扳面积统计方式返回相应的重量值  xc 2010-12-28
	double area = 0;
	if (m_iPlankWeightStatType==2)
	{
		area = CalAreaOfNetWrap();
		return CalSteelWeight(area*_thick);
	}
	else if(m_iPlankWeightStatType==1)
	{
		area = CalAreaOfMinWrapRect();
		return CalSteelWeight(area*_thick);
	}
	else
	{
		f2dRect rect=GetWrapRect();
		return CalSteelWeight(rect.Width()*rect.Height()*_thick);
	}
}
double CLDSPlate::GetThick(){	return Thick;}
void CLDSPlate::DesignSetupUcs()
{
	if(IsShadowObject())	//影像构件
	{
		ShadowDesign();
		return;
	}
	CLDSPart *pPart=BelongModel()->FromPartHandle(designInfo.m_hBasePart);//hBasePart);
	if(pPart==NULL&&log2file!=NULL)
#ifdef	AFX_TARG_ENU_ENGLISH
		Log2File()->Log("Lose 0X%X plate's datum rod 0X%X for locating!",handle,designInfo.m_hBasePart);
#else
		Log2File()->Log("0X%X钢板的定位基准杆件0X%X丢失！",handle,designInfo.m_hBasePart);
#endif
	CSuperSmartPtr<CLDSLinePart> pBaseLinePart,pCurLinePart;
	CSuperSmartPtr<CLDSPart> pDatumPart;
	SetModified();
	designInfo.origin.UpdatePos(BelongModel());
	UCS_STRU oldcs=ucs;
	if(designInfo.origin.datum_pos_style>0)
		ucs.origin = designInfo.origin.Position();
	//没有把jdb_style==2括进去的原因是为了最大程度与旧代码继承（如后续确认需要括进去再改），
	//jdb_style=1时可能会是变电构架上的特殊折叠板 wjh-2016.9.19
	if(!designInfo.m_bEnableFlexibleDesign&&(jdb_style==0||jdb_style==1)||jdb_style==2)
		return;	//如果不柔性设计外形只更改坐标系,可能会导致板面翻转错误 wjh-2011.8.10
	if(jdb_style==5)	
	{	//相对于基准构件位置不动的附加板
		if(pPart)
			pDatumPart=pPart;
		else
			return;
	}
	else
	{	
		if(pPart&&pPart->IsLinePart())
		{
			ucs.axis_x.Set();	//清空X轴方向，根据参数重新计算(对称生成的板重新设计X轴方向需要纠正) wht 10-08-25
			pBaseLinePart=(CLDSLinePart*)pPart;
		}
		//else
		//	return;
	}
	designInfo.norm.UpdateVector(BelongModel());
	ucs.axis_z=designInfo.norm.vector;
	normalize(ucs.axis_z);
	if(pBaseLinePart.IsHasPtr()&&ucs.axis_y.IsZero())
		ucs.axis_y=pBaseLinePart->Start()-pBaseLinePart->End();
	normalize(ucs.axis_y);
	if(jdb_style==5)//相对于基准构件位置不动的附加板
	{
		CLDSParamPlate *pFatherPlate=NULL;
		if(pDatumPart->GetClassTypeId()==CLS_PLATE)
			pFatherPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(pDatumPart->m_hPartWeldParent,CLS_PARAMPLATE);
		if((pDatumPart->GetClassTypeId()==CLS_PARAMPLATE&&pDatumPart.ParamPlatePointer()->m_iParamType==TYPE_XEND)
			||(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE&&pDatumPart.ParamPlatePointer()->m_iParamType==TYPE_ROLLEND) 
			||(pFatherPlate&&pFatherPlate->m_iParamType==TYPE_XEND))	//十字插板附加板 wht 11-01-21
		{	//十字插板上的包板或立板或十字插板附加板
			//由原来的X-Z平面为基准平面改为X-Y,因为相对基准构件位置不动的附加板一般是立板 wjh 2011.11.22
			//ucs.axis_z=ucs.axis_x^ucs.axis_y;
			ucs.axis_x=pDatumPart->ucs.axis_x;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;	
		}
	}
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	for(int i=1;i<face_N;i++)
	{	//之前多年以来重新设计时未更新火曲面法线 wjh-2015.9.16
		//norm_style=0时UpdateVector与nearVector无关　wjh-2016.4.12
		if(!huoqufacenorm[i-1].IsZero())
			designInfo.huoqufacenorm[i-1].vector=designInfo.huoqufacenorm[i-1].nearVector=huoqufacenorm[i-1];
		designInfo.huoqufacenorm[i-1].UpdateVector(BelongModel());
		huoqufacenorm[i-1]=designInfo.huoqufacenorm[i-1].vector;
		normalize(huoqufacenorm[i-1]);
	}
	if(5==jdb_style||(designInfo.origin.datum_pos_style>0&&designInfo.norm.norm_style==0))
		return;
	if(wiType==WASHER_PLATE)//螺栓垫板
	{	
		//重新计算螺栓垫板的位置
		CLDSBolt *pBolt=NULL;
		if(GetLsCount()>0)
			pBolt=GetFirstLsRef()->GetLsPtr();
		if(pBolt&&ucs.axis_z.IsZero()) //法线为0时默认取螺栓的法线 wht 11-01-10
			ucs.axis_z=pBolt->ucs.axis_z;
		if(pBaseLinePart.IsHasPtr()&&pBolt)
		{
			if(cDatumAxis=='Y')
			{	//以Y轴为基准轴时坐标原点位置存储于designInfo.origin中,已在前面计算过,此处仅需要重新计算坐标轴方向 wht 17-02-20
				GEPOINT axis_y=pBaseLinePart->End()-pBaseLinePart->Start();	
				if(!ucs.axis_y.IsZero()&&ucs.axis_y*axis_y<0)
					ucs.axis_y=-1.0*axis_y;
				else
					ucs.axis_y=axis_y;
				ucs.axis_x=ucs.axis_y^ucs.axis_z;
			}
			else
			{
			ucs.origin=pBolt->ucs.origin;
			project_point(ucs.origin,pBaseLinePart->Start(),ucs.axis_z);
			//SnapPerp(&ucs.origin,pBaseLinePart->ucs.origin-pBaseLinePart->ucs.axis_z*1000000,
			//pBaseLinePart->ucs.origin+pBaseLinePart->ucs.axis_z*1000000,pBolt->ucs.origin);
			//TODO:之前垫板的工作平项的基准轴应该是Y轴吧? wjh-2017.1.11
			//X轴方向取杆件延伸方向,否则角钢位置变化之后多孔螺栓垫板坐标系未实时更新，导致外形错误 wht 16-11-25
			ucs.axis_x=pBaseLinePart->End()-pBaseLinePart->Start();	
			}
		}
		if(GetLsCount()==1)//仅有一个螺栓的时候要保证，螺栓垫板的X轴垂直切割基准角钢
		{
			CDesignLjPartPara *pLjPara=GetFirstLjPara();
			CLDSLineAngle *pCutAngle=NULL;
			if(pLjPara)
				pCutAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pLjPara->hPart,CLS_LINEANGLE);
			if(pCutAngle)
			{
				ucs.axis_y=pCutAngle->End()-pCutAngle->Start();
				ucs.axis_x=ucs.axis_y^ucs.axis_z;
				f3dPoint inter_pos;
				SnapPerp(&inter_pos,pCutAngle->Start(),pCutAngle->End(),ucs.origin);
				f3dPoint vec=inter_pos-ucs.origin;
				if(ucs.axis_x*vec<0)
					ucs.axis_x*=-1.0;
			}
		}
		//重新计算螺栓垫板的坐标系
		ucs.axis_y=ucs.axis_z^ucs.axis_x;
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(ucs.axis_x);
		normalize(ucs.axis_y);
	}
	/*else if(jdb_style==5)//相对于基准构件位置不动的附加板
	{
		CLDSParamPlate *pFatherPlate=NULL;
		if(pDatumPart->GetClassTypeId()==CLS_PLATE)
			pFatherPlate=(CLDSParamPlate*)BelongModel()->FromPartHandle(pDatumPart->m_hPartWeldParent,CLS_PARAMPLATE);
		if((pDatumPart->GetClassTypeId()==CLS_PARAMPLATE&&pDatumPart.ParamPlatePointer()->m_iParamType==TYPE_XEND) 
			||(pFatherPlate&&pFatherPlate->m_iParamType==TYPE_XEND))	//十字插板附加板 wht 11-01-21
		{	//十字插板上的包板或立板或十字插板附加板
			//由原来的X-Z平面为基准平面改为X-Y,因为相对基准构件位置不动的附加板一般是立板 wjh 2011.11.22
			//ucs.axis_z=ucs.axis_x^ucs.axis_y;
			ucs.axis_x=pDatumPart->ucs.axis_x;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			normalize(ucs.axis_x);
			normalize(ucs.axis_y);
			normalize(ucs.axis_z);	
		}
		else
		{	//普通垫板也出现过三坐标轴轻微不正交情况,因此需要修正 wjh-2015.2.28
			ucs.axis_x=ucs.axis_y^ucs.axis_z;
			ucs.axis_y=ucs.axis_z^ucs.axis_x;
			normalize(ucs.axis_x);
			normalize(ucs.axis_y);
		}
		return;	//由于pBaseLinePart值为空，再往后执行只可能导致异常出现(如十字插板对称时，立板可能出现异常) wjh-2011.11.05　
	}*/
	else if(jdb_style>1)	//钢管上的附加环向板，需要重新根据设定计算板的X轴方向
	{
		CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(designInfo.m_hBasePart,CLS_LINETUBE);
		if(pDatumTube&&!IsFoldPlate())
		{
			pDatumTube->BuildUCS();
			f3dPoint axis_x_vec=pDatumTube->ucs.axis_x;
			if(pDatumTube->m_bHasWeldRoad&&m_bAttachPartByWeldRoad)
			{	//以焊道为基准作为角度定位起始边
				double rot_ang=pDatumTube->CalWeldRoadAngle();
				RotateVectorAroundVector(axis_x_vec,rot_ang,pDatumTube->ucs.axis_z);
			}
			if(fabs(ucs.axis_z*axis_x_vec)>EPS_COS)
				ucs.axis_x=axis_x_vec;
			else
			{
				f3dPoint inters;
				Int3dlf(inters,pDatumTube->Start()+axis_x_vec*pDatumTube->GetDiameter(),pDatumTube->ucs.axis_z,
					ucs.origin,ucs.axis_z);
				ucs.axis_x=inters-ucs.origin;
			}
		}
	}
	//判断X轴是否与Z轴方向相同或相反,清空X轴方向重新计算 wht 10-07-22
	if(fabs(ucs.axis_x*ucs.axis_z)>EPS_COS)
		ucs.axis_x.Set();
	if(ucs.axis_x.IsZero())
	{
		f3dPoint rayVec;
		long hCoupleTube=0; //对接相贯的钢管句柄
		if (pBaseLinePart.IsHasPtr()&&pBaseLinePart->GetClassTypeId() == CLS_LINETUBE)
		{
			if(pBaseLinePart->pStart&&pBaseLinePart->pStart->handle==designInfo.m_hBaseNode)
				hCoupleTube=pBaseLinePart.LineTubePointer()->m_tJointStart.hLinkObj;
			else if(pBaseLinePart->pEnd&&pBaseLinePart->pEnd->handle==designInfo.m_hBaseNode)
				hCoupleTube=pBaseLinePart.LineTubePointer()->m_tJointEnd.hLinkObj;
			if(jdb_style==0)
				m_fNormOffset=-0.5*GetThick();
		}
		long hBaseRod=pBaseLinePart.IsHasPtr()?pBaseLinePart->handle:0;
		for(CDesignLjPartPara *pLjPart=designInfo.partList.GetFirst();pLjPart;pLjPart=designInfo.partList.GetNext())
		{
			if(pLjPart->hPart!=hBaseRod&&pLjPart->hPart!=hCoupleTube)
			{
				CLDSPart *pPart=BelongModel()->FromPartHandle(pLjPart->hPart);
				if(pPart&&pPart->IsLinePart())
				{
					CLDSLinePart *pLinePart = (CLDSLinePart*)pPart;
					if(pLjPart->start0_end1==0)	//始端连接
					{
						rayVec=pLinePart->End()-pLinePart->Start();
						break;
					}
					else if(pLjPart->start0_end1==1)	//终端连接
					{
						rayVec=pLinePart->Start()-pLinePart->End();
						break;
					}
				}
			}
		}
		ucs.axis_x=ucs.axis_y^ucs.axis_z;
		normalize(rayVec);
		//添加fabs(ucs.axis_x*rayVec)>EPS判断目的是处理组合角钢中分割子板上组合子角钢即为射线角钢与Y轴重合情况 wjh-2011.7.18
		double dd=ucs.axis_x*rayVec;
		if(!rayVec.IsZero()&&fabs(dd)>EPS&&(dd<0))
			ucs.axis_x*=-1.0;	
	}
	/*else
	{
		f3dPoint axis_x=ucs.axis_y^ucs.axis_z;
		if(ucs.axis_x*axis_x<0)
			ucs.axis_x=-axis_x;
		else
			ucs.axis_x=axis_x;
	}*/
	ucs.axis_y = ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_y);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	normalize(ucs.axis_x);
	//如不进行顶点轮廓点转换,对称生成的挂线板在重新设计时极有可能跑掉 wjh-2015.4.8
	//现在实在想不起来，当初是什么情况下不添加如下代码会导致外形设计后板会跑掉了，但添加此代码肯定会导致如
	//包钢板之类的钢板在更改主材正负头后，板外形不随动情况，另可能因坐标系转换带来额外的Z分量不为零情况,
	//因此暂时注释掉，待以后发现问题后再完善　wjh-2015.9.10
	/*for(PROFILE_VERTEX* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		GEPOINT pos=pVertex->vertex;
		coord_trans(pos,oldcs,TRUE);
		coord_trans(pos,ucs,FALSE);
		pVertex->vertex.Set(pos.x,pos.y,pos.z);
	}
	if(jdb_style==0&&face_N==3)
	{
		coord_trans(top_point,oldcs,TRUE);
		coord_trans(top_point,ucs,FALSE);
	}
	for(int i=0;i<face_N-1;i++)
	{
		coord_trans(HuoQuLine[i].startPt,oldcs,TRUE);
		coord_trans(HuoQuLine[i].startPt,ucs,FALSE);
		coord_trans(HuoQuLine[i].endPt,oldcs,TRUE);
		coord_trans(HuoQuLine[i].endPt,ucs,FALSE);
		HuoQuLine[i].startPt.z=HuoQuLine[i].endPt.z=0;	//避免因坐标系转换带来的Ｚ坐标偏差
	}*/
	SetModified();
}

BOOL CLDSPlate::DeleteLjPara(long handle)
{
	for(CDesignLjPartPara *pLjPara=GetFirstLjPara(); pLjPara; pLjPara=GetNextLjPara())
	{
		if(pLjPara->hPart==handle)
		{
			designInfo.partList.DeleteCursor();
			return TRUE;
		}
	}
	return FALSE;
}
//
CDesignLjPartPara* CLDSPlate::GetLjRodPara(long handle)
{
	CDesignLjPartPara *pLjPara=NULL;
	for(pLjPara=GetFirstLjPara(); pLjPara; pLjPara=GetNextLjPara())
	{
		if(pLjPara->hPart==handle)
			break;
	}
	return pLjPara;
}
//根据钢板上螺栓更新指定连接杆件上的偏移距
BOOL CLDSPlate::UpdateLjRodEdgeSpace(long hPart,BYTE cbEdgeFlag/*=0x03*/)
{
	CDesignLjPartPara *pLjPara=GetLjRodPara(hPart);
	if(pLjPara==NULL)
		return FALSE;
	return UpdateLjRodEdgeSpace(pLjPara,cbEdgeFlag);
}
BOOL CLDSPlate::UpdateLjRodEdgeSpace(CDesignLjPartPara* pLjPara, BYTE cbEdgeFlag /*= 0x03*/)
{
	if (pLjPara == NULL)
		return FALSE;
	CLDSLinePart* pCurRod = (CLDSLinePart*)BelongModel()->FromPartHandle(pLjPara->hPart);
	if (pCurRod == NULL)
		return FALSE;
	//根据依附杆件计算特征点坐标及方向
	CSuperSmartPtr<CLDSLinePart> pCurLinePart = pCurRod;
	f3dPoint base_pos, norm;
	if (pLjPara->start0_end1 != 1)	//始端连接或中间连接
		base_pos = pCurLinePart->Start();
	else								//终端连接
		base_pos = pCurLinePart->End();
	f3dLine ber_line;
	if (pCurRod->IsAngle())
	{
		ber_line.startPt = pCurLinePart->Start();
		ber_line.endPt = pCurLinePart->End();
		if (pLjPara->angle.cur_wing_x0_y1 == 0)
			norm = pCurLinePart.LineAnglePointer()->get_norm_x_wing();
		else
			norm = pCurLinePart.LineAnglePointer()->get_norm_y_wing();
	}
	else if (pCurRod->GetClassTypeId() == CLS_LINEFLAT || pCurRod->GetClassTypeId() == CLS_LINESLOT)
	{
		f3dPoint wing_vec = pCurLinePart->ucs.axis_y;
		norm = pCurLinePart->WorkPlaneNorm();
		base_pos -= wing_vec * 0.5*pCurLinePart->GetWidth();
		ber_line.startPt -= wing_vec * 0.5*pCurLinePart->GetWidth();
		ber_line.endPt -= wing_vec * 0.5*pCurLinePart->GetWidth();
	}
	//根据螺栓位置计算距楞线最小距离和最大距离
	double max_ber_dist = 0, min_ber_dist = pCurRod->GetWidth();
	long bolt_d = 20;
	for (CLsRef *pLsRef = GetFirstLsRef(); pLsRef; pLsRef = GetNextLsRef())
	{
		f3dPoint ls_pos, ls_offset_vec;
		CLDSBolt *pBolt = pLsRef->GetLsPtr();
		if (pBolt&&pCurRod->FindLsByHandle(pBolt->handle))
		{	//角钢正负头修改后螺栓位置和法线都有可能修改，所以需要再次更新
			double dist = 0;
			bolt_d = pBolt->get_d();
			pBolt->correct_worknorm();
			pBolt->correct_pos();
			ls_pos = pBolt->ucs.origin;
			project_point(ls_pos, base_pos, norm);
			SnapPerp(&ls_pos, ber_line, ls_pos, &dist);
			if (max_ber_dist == 0)
				max_ber_dist = min_ber_dist = dist;
			else if (dist > max_ber_dist)
				max_ber_dist = dist;
			else if (dist < min_ber_dist)
				min_ber_dist = dist;
		}
	}
	//根据边距类型计算所需边距
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace, bolt_d);
	if (cbEdgeFlag & 0x01)
	{
		if (pLjPara->angle.cbSpaceFlag.StyleOfBerSpace() == ANGLE_SPACE_FLAG::SPACE_BOLTEND)//标准边距
			pLjPara->ber_space = LsSpace.EndSpace;
		else if (pLjPara->angle.cbSpaceFlag.StyleOfBerSpace() == ANGLE_SPACE_FLAG::SPACE_TOEDGE)//延伸到边缘
			pLjPara->ber_space = min_ber_dist;
		else if (pLjPara->angle.cbSpaceFlag.StyleOfBerSpace() == ANGLE_SPACE_FLAG::SPACE_TOBER)//延伸到楞线边缘-肢厚度
			pLjPara->ber_space = min_ber_dist - pCurLinePart->GetThick();
	}
	if (cbEdgeFlag & 0x02)
	{
		if (pLjPara->angle.cbSpaceFlag.StyleOfWingSpace() == ANGLE_SPACE_FLAG::SPACE_BOLTEND)//标准边距
			pLjPara->wing_space = LsSpace.EndSpace;
		else if (pLjPara->angle.cbSpaceFlag.StyleOfWingSpace() == ANGLE_SPACE_FLAG::SPACE_TOEDGE)//延伸到边缘
			pLjPara->wing_space = pCurLinePart->GetWidth() - max_ber_dist;
		else if (pLjPara->angle.cbSpaceFlag.StyleOfWingSpace() == ANGLE_SPACE_FLAG::SPACE_TOBER)//延伸到楞线边缘-肢厚度
			pLjPara->wing_space = pCurLinePart->GetWidth() - max_ber_dist - pCurLinePart->GetThick();
	}
	if (cbEdgeFlag & 0x04)
	{
		if (pLjPara->angle.cbSpaceFlag.StyleOfEndSpace() == ANGLE_SPACE_FLAG::SPACE_BOLTEND)
			pLjPara->end_space = LsSpace.EndSpace;
	}
	return TRUE;
}
//返回钢板基准节点
CLDSNode *CLDSPlate::GetBaseNode(bool bClearLocks/*=true*/)
{
	CDepthCounter<BYTE> visiter(NULL);//避免组焊件焊接父构件导致的死循环
	if(!bClearLocks&&IsLocked())
		return NULL;	//内部调用时发现该板已在之前分步中调用过，为防止死循环直接返回NULL wjh-2016.10.24
	else if(!IsLocked())
		visiter.AttachCounter(&m_bLocked);
	//此刻已可以保证m_bLocked=true
	long hBaseNode=0;
	CLDSNode *pBaseNode=NULL;
	if(designInfo.m_hBaseNode>0X20)
		hBaseNode=designInfo.m_hBaseNode;
	else 
	{	//根据钢板焊接父构件查找基准节点
		CSuperSmartPtr<CLDSPart> pPlate;
		pPlate=BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_PLATE,CLS_PARAMPLATE);
		if(pPlate.IsHasPtr())
		{
			if(pPlate->GetClassTypeId()==CLS_PLATE)
				return pPlate.PlatePointer()->GetBaseNode(false);
			else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
				return pPlate.ParamPlatePointer()->GetBaseNode(false);
		}
	}
	if(hBaseNode>0x20)
		pBaseNode=BelongModel()->FromNodeHandle(designInfo.m_hBaseNode);
	return pBaseNode;
}

void CLDSPlate::CloneTo(CLDSPlate &plate)
{
	CBuffer buffer;
	ToBuffer(buffer);
	buffer.SeekToBegin();
	plate.FromBuffer(buffer);
}

//此函数仅用于生成普通三面板剪切面补焊时的补料钢板的多边外形
bool CLDSPlate::GetComplementFaceVertex(f3dPoint &vertex1,f3dPoint &vertex2)
{
	if(jdb_style!=0||face_N!=3)
		return false;
	PROFILE_VERTEX *pFirstVertex=vertex_list.GetFirst(),*pTailVertex=vertex_list.GetTail();
	PROFILE_VERTEX *pPrevVertex=vertex_list.GetTail();
	for(PROFILE_VERTEX *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pPrevVertex->vertex.feature!=pVertex->vertex.feature &&
			pPrevVertex->vertex.feature<10 && pVertex->vertex.feature<10 &&
			pPrevVertex->vertex.feature>1 && pVertex->vertex.feature>1)
		{
			if(pPrevVertex->vertex==pVertex->vertex)
			{	//处理补焊顶点重合的情况 wht 10-09-03
				PROFILE_VERTEX *pNextVertex=vertex_list.GetNext();
				if(pNextVertex==NULL)
				{
					pNextVertex=vertex_list.GetFirst();
					vertex_list.GetTail(); //恢复原来位置
				}
				else 
					vertex_list.GetPrev(); //恢复原来位置
				//此处不用恢复链表原来位置
				if(pPrevVertex==pFirstVertex)
					pPrevVertex=pTailVertex;
				else if(pPrevVertex==pTailVertex)
				{
					vertex_list.GetTail();
					pPrevVertex=vertex_list.GetPrev();
				}
				else 
				{
					vertex_list.GetPrev();
					pPrevVertex=vertex_list.GetPrev();
				}
				vertex1=GetRealVertex(pPrevVertex->vertex);
				vertex2=GetRealVertex(pNextVertex->vertex);
			}
			else
			{
				vertex1=GetRealVertex(pPrevVertex->vertex);
				vertex2=GetRealVertex(pVertex->vertex);
			}
			f3dPoint vec1=vertex1-top_point;
			f3dPoint vec2=vertex2-top_point;
			f3dPoint norm=vec1^vec2;
			normalize(norm);
			f3dPoint rot_axis=norm^f3dPoint(0,0,1);
			double rot_ang=ACOS(norm.z);
			rotate_point_around_axis(vertex1,rot_ang,top_point,top_point+rot_axis);
			rotate_point_around_axis(vertex2,rot_ang,top_point,top_point+rot_axis);
			return true;
		}
		pPrevVertex=pVertex;
	}
	return false;
}
bool CLDSPlate::UpdateVertexTangentPos(PROFILE_VERTEX* pPrevVertex,PROFILE_VERTEX* pVertex,PROFILE_VERTEX* pNextVertex)
{
	if(pPrevVertex->type==1||pPrevVertex->type==2)
	{	//前一线边为圆弧边
		f3dArcLine arcline;
		if(pPrevVertex->type==1)
		{	//指定角度
			arcline.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,1),pPrevVertex->sector_angle);
#ifdef AFX_TARG_ENU_ENGLISH
			if(AfxMessageBox("Unable to calculate the arc tangent point of specified angle,whether modify to specified arc radius and calculate tangent point?",MB_YESNO)==IDYES)
#else 
			if(AfxMessageBox("无法计算(指定角度的)圆弧公切点,是否修改为指定半径的圆弧继续计算公切点？",MB_YESNO)==IDYES)
#endif
			{
				pPrevVertex->type=2;	//指定半径
				pPrevVertex->radius=fabs(arcline.Radius());
				if(pPrevVertex->sector_angle<0)
					pPrevVertex->radius*=-1.0;
				pPrevVertex->center=arcline.Center();
			}
			else
				return false;
		}
		else //if(pPrevVertex->type==2)	//指定半径
			arcline.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,1),pPrevVertex->radius,pPrevVertex->center);
		f2dPoint basePt(pNextVertex->vertex.x,pNextVertex->vertex.y),retPt,pickPt(pVertex->vertex.x,pVertex->vertex.y);
		f2dCircle cir(arcline.Radius(),arcline.Center().x,arcline.Center().y);
		if(Tang2dpc(basePt,cir,pickPt,retPt)==1)
		{
			pVertex->vertex.Set(retPt.x,retPt.y);
			//指定半径和圆心拾取点模式由于求公切会引起弦线变化进而必须用真实圆心调整原圆心拾取点,
			//否则可能会引起圆心变到弦的另一侧-wjh.2011-6-26
			if(pPrevVertex->type==2)
				pPrevVertex->center=arcline.Center();
		}
	}
	else if(pVertex->type==1||pVertex->type==2)
	{	//后一边为圆弧边
		f3dArcLine arcline;
		if(pVertex->type==1)
		{	//指定半径
			arcline.CreateMethod2(pVertex->vertex,pNextVertex->vertex,f3dPoint(0,0,1),pVertex->sector_angle);
#ifdef AFX_TARG_ENU_ENGLISH
			if(AfxMessageBox("Unable to calculate the arc tangent point of specified angle,whether modify to specified arc radius and calculate tangent point?",MB_YESNO)==IDYES)
#else 
			if(AfxMessageBox("无法计算(指定角度的)圆弧公切点,是否修改为指定半径的圆弧继续计算公切点？",MB_YESNO)==IDYES)
#endif
			{
				pVertex->type=2;	//指定半径
				pVertex->radius=fabs(arcline.Radius());
				if(pVertex->sector_angle<0)
					pVertex->radius*=-1.0;
				pVertex->center=arcline.Center();
			}
			else
				return false;
		}
		else //if(pVertex->type==2)	//指定半径
			arcline.CreateMethod3(pVertex->vertex,pNextVertex->vertex,f3dPoint(0,0,1),pVertex->radius,pVertex->center);
		f2dPoint basePt(pPrevVertex->vertex.x,pPrevVertex->vertex.y),retPt,pickPt(pVertex->vertex.x,pVertex->vertex.y);
		f2dCircle cir(arcline.Radius(),arcline.Center().x,arcline.Center().y);
		if(Tang2dpc(basePt,cir,pickPt,retPt)==1)
		{
			pVertex->vertex.Set(retPt.x,retPt.y);
			//指定半径和圆心拾取点模式由于求公切会引起弦线变化进而必须用真实圆心调整原圆心拾取点,
			//否则可能会引起圆心变到弦的另一侧-wjh.2011-6-26
			if(pVertex->type==2)
				pVertex->center=arcline.Center();
		}
	}
	return true;
}

bool CLDSPlate::Design3FacePlateComplementPlate(CLDSPlate *pDatumPlate)
{
	/*if(pDatumPlate==NULL)
		return false;
	if(pDatumPlate->face_N!=3)
		return false;
	//重新设计三面板补焊钢板外形
	f3dPoint vertex1,vertex2,top_point;
	pDatumPlate->GetComplementFaceVertex(vertex1,vertex2);
	//设计补焊钢板坐标系以top_point为坐标原点
	ucs.origin=top_point;
	coord_trans(ucs.origin,pDatumPlate->ucs,TRUE);
	f3dPoint vec1=vertex1-top_point;
	f3dPoint vec2=vertex2-top_point;
	normalize(vec1);
	normalize(vec2);
	vector_trans(vec1,pDatumPlate->ucs,TRUE);
	vector_trans(vec2,pDatumPlate->ucs,TRUE);
	ucs.axis_x=vec1;
	ucs.axis_y=vec2;
	ucs.axis_z=ucs.axis_x^ucs.axis_y;
	normalize(ucs.axis_z);
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_y);
	//重新生成补焊钢板顶点列表
	vertex_list.Empty();
	coord_trans(vertex1,pDatumPlate->ucs,TRUE);
	coord_trans(vertex2,pDatumPlate->ucs,TRUE);
	coord_trans(vertex1,ucs,FALSE);
	coord_trans(vertex2,ucs,FALSE);
	f3dPoint tmpVertex;
	vertex_list.append(tmpVertex);
	vertex_list.append(vertex1);
	tmpVertex.Set(vertex1.x,vertex1.y+20);
	vertex_list.append(tmpVertex);
	f3dPoint offset_norm(vertex2.y,-vertex2.x);
	normalize(offset_norm);
	tmpVertex=vertex2+offset_norm*20;
	vertex_list.append(tmpVertex);
	vertex_list.append(vertex2);*/
	return true;
}
/*
//	CClonePlate相关代码已完全移至ClonePlate.cpp中 wjh-2017.8.7
	...
*/
double CLDSPlate::GetOutterPlaneOffset(const f3dPoint &outter_vec)
{
	if(ucs.axis_z*outter_vec>0)
		return m_fNormOffset+GetThick();
	else
		return m_fNormOffset;
}

//判断是否为贯通钢管的钢板 wht 11-05-27
BOOL CLDSPlate::IsTransPlate()
{
	CLDSPlate* pMotherPlate=(CLDSPlate*)MotherPart();
	if(pMotherPlate!=NULL&&pMotherPlate!=this)
		return pMotherPlate->IsTransPlate();
	PROFILE_VERTEX *pVertex=NULL,*pFirstVertex=vertex_list.GetFirst();
	if(pFirstVertex==NULL)
		return FALSE;
	//1.钢板焊接父构件为钢管，获得基准钢管
	CLDSLineTube *pDatumTube=(CLDSLineTube*)BelongModel()->FromPartHandle(m_hPartWeldParent,CLS_LINETUBE);
	if(pDatumTube==NULL)
		return FALSE;	//焊接父构件不是钢管，不可能是贯通板
	//2.取钢板上任意一点向钢管心线投影作为原点，钢管延伸方向为X轴，钢板Z轴为Y轴，
	//  取钢板上任意一点纠正Z轴方向
	UCS_STRU temp_ucs;
	f3dPoint vertex=pFirstVertex->vertex;
	coord_trans(vertex,ucs,TRUE);
	SnapPerp(&temp_ucs.origin,pDatumTube->Start(),pDatumTube->End(),vertex);
	temp_ucs.axis_x=pDatumTube->End()-pDatumTube->Start();
	normalize(temp_ucs.axis_x);
	//目前所谓的贯通板都是指与钢管轴线方向平行的钢板，即钢板法线应垂直与钢管轴线 wht 11-07-18
	if(fabs(ucs.axis_z*temp_ucs.axis_x)>0.2)//EPS2)应该允许钢板与钢管轴线有较大的平行偏差度（12°) wjh-2015.8.28
		return FALSE;	
	temp_ucs.axis_y=ucs.axis_z;
	temp_ucs.axis_z=temp_ucs.axis_x^temp_ucs.axis_y;
	normalize(temp_ucs.axis_z);
	//钢板法线不一定与钢管严格垂直，为了保证temp_ucs的正交线，必须校正Y轴方向 wjh-2015.8.28
	temp_ucs.axis_y=temp_ucs.axis_z^temp_ucs.axis_x;
	f3dPoint verfiy_vec=vertex-ucs.origin;
	normalize(verfiy_vec);
	if(verfiy_vec.IsZero())
		return TRUE;	//顶点在钢管轴线上认为钢板为贯通板
	else if(verfiy_vec*temp_ucs.axis_z<0)
	{
		temp_ucs.axis_x*=-1.0;
		temp_ucs.axis_z*=-1.0;
	}
	//3.如出现Z坐标为负的顶点，则认为为贯通板 
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		f3dPoint pt=pVertex->vertex;
		coord_trans(pt,ucs,TRUE);
		coord_trans(pt,temp_ucs,FALSE);
		if(pt.z<=0)	//存在Z坐标小于0的顶点则认为钢板为贯通板
			return TRUE;
	}
	return FALSE;
}

//<DEVELOP_PROCESS_MARK nameId="CLDSPlate::GetGeomCenter()">
//得到钢板几何形心位置
f3dPoint CLDSPlate::GetGeomCenter(CLDSPart *pPlate)
{
	if(pPlate==NULL)
		return f3dPoint();
	long handle=pPlate->handle;
	f3dPoint huoqufacenorm[2];
	ATOM_LIST<PROFILE_VERTEX>* pVertexList=NULL;
	if(pPlate->GetClassTypeId()==CLS_PLATE)
	{
		huoqufacenorm[0]=((CLDSPlate*)pPlate)->huoqufacenorm[0];
		huoqufacenorm[1]=((CLDSPlate*)pPlate)->huoqufacenorm[1];
		pVertexList=&(((CLDSPlate*)pPlate)->vertex_list);
	}
	else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		huoqufacenorm[0]=((CLDSParamPlate*)pPlate)->HuoquFaceNorm();
		pVertexList=&(((CLDSParamPlate*)pPlate)->vertex_list);
	}
	else 
		return f3dPoint();

	UCS_STRU tmucs,ucs=pPlate->ucs;
	PROFILE_VERTEX *pVertex,*pPrevVertex=NULL;
	double wrap_area=0;
	BOOL bEnd=FALSE;
	f3dPoint geom_center;
	try{
		PROFILE_VERTEX *pFirstVertex=pVertexList->GetFirst();
		PROFILE_VERTEX *pPrevVertex=pVertexList->GetNext();
		for(pVertex=pVertexList->GetNext();pVertex;pVertex=pVertexList->GetNext())
		{
			double result=DistOf2dPtLine(pVertex->vertex,pFirstVertex->vertex,pPrevVertex->vertex);
			f3dPoint center=(pVertex->vertex+pFirstVertex->vertex+pPrevVertex->vertex)/3.0;
			double area=0;
			if(result>0)		// 后点在线左侧，正三角形面积
				area= CalTriArea(pFirstVertex->vertex.x,pFirstVertex->vertex.y,pPrevVertex->vertex.x,pPrevVertex->vertex.y,pVertex->vertex.x,pVertex->vertex.y);
			else if(result<0)	// 后点在线右侧，负三角形面积
				area=-CalTriArea(pFirstVertex->vertex.x,pFirstVertex->vertex.y,pPrevVertex->vertex.x,pPrevVertex->vertex.y,pVertex->vertex.x,pVertex->vertex.y);
			pPrevVertex=pVertex;
			geom_center+=area*center;
			wrap_area+=area;
		}
		for(pVertex=pVertexList->GetFirst();1;pVertex=pVertexList->GetNext())
		{
			if(pVertex==NULL)
			{
				bEnd=TRUE;
				pVertex=pVertexList->GetFirst();
			}
			if(pPrevVertex==NULL)
				pPrevVertex=pVertex;
			else if(pPrevVertex->type>0)
			{
				f3dArcLine arcLine;
				//pPrevVertex->RetrieveArcLine(arcLine,pVertex->vertex,&ucs)
				f3dPoint worknorm;
				if(pPrevVertex->vertex.feature==2)
					worknorm=huoqufacenorm[0];
				else if(pPrevVertex->vertex.feature==3)
					worknorm=huoqufacenorm[1];
				else
					worknorm=ucs.axis_z;
				vector_trans(worknorm,ucs,FALSE);
				int sign=1;
				if(pPrevVertex->type==1)	//指定圆弧夹角
				{
					arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,worknorm,pPrevVertex->sector_angle);
					if(pPrevVertex->sector_angle<0)
						sign=-1;
				}
				else if(pPrevVertex->type==2)	//指定圆弧半径
				{	//圆心拾取点为相对坐标不用再次进行转换 wht 12-06-14
					//f3dPoint center_pick=pPrevVertex->center;
					//coord_trans(center_pick,ucs,FALSE);
					if(pPrevVertex->radius<0)
						sign=-1;
					arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,sign*worknorm,sign*pPrevVertex->radius,pPrevVertex->center);
				}
				else //if(pPrevVertex->type==3)	//椭圆弧，在统计面积时暂按椭圆的包络圆处理
				{	
					f3dPoint center_pick=pPrevVertex->center;
					f3dPoint columnnorm=pPrevVertex->column_norm;
					normalize(worknorm);
					normalize(columnnorm);
					coord_trans(center_pick,ucs,FALSE);
					vector_trans(columnnorm,ucs,FALSE);
					double cosa=worknorm*columnnorm;
					if(pPrevVertex->radius<0)
						sign=-1;
					arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,sign*worknorm,fabs(pPrevVertex->radius/cosa),center_pick);
				}
				//计算由于圆弧造成的面积增减值
				//扇形的几何形心位于2*r*sin(half_angle)/(3*half_angle)处
				//弓形面积r*r*(2*half_angle-sina(2*half_angle))/2
				double sector_angle=arcLine.SectorAngle();
				double r2=arcLine.Radius()*arcLine.Radius();
				double sina=sin(sector_angle),sinHalfA=sin(sector_angle*0.5);
				f3dPoint mid_vec=pPrevVertex->vertex-f3dPoint(arcLine.Center());
				mid_vec.z=0;
				RotateVectorAroundVector(mid_vec,sector_angle*0.5,arcLine.WorkNorm());	//起始边延逆时针方向旋转0.5*sector_angle
				normalize(mid_vec);
				double sector_area=(r2*(sector_angle-sina))/2.0;	//扇形面积减去弓形面积
				//弓形几何形心位于4*r*sin(half_angle)*sin(half_angle)*sin(half_angle)/3(2*half_angle-sin(2*half_angle))
				double coef=(4*arcLine.Radius()*sinHalfA*sinHalfA*sinHalfA)/(3*(sector_angle-sina));
				f3dPoint sector_center=f3dPoint(arcLine.Center())+mid_vec*coef;
				wrap_area+=sign*sector_area;
				geom_center+=sign*sector_center*sector_area;
			}
			/*
			//由于计算几何形心主要是用于判断钢板的大概位置,故在此不计入卷边影响
			else if(pPrevVertex->m_bRollEdge)
			{
				f3dPoint edge_vec=pVertex->vertex-pPrevVertex->vertex;
				wrap_area+=edge_vec.mod()*pPrevVertex->manu_space;
			}*/
			pPrevVertex = pVertex;
			if(bEnd)
				break;
		}
		geom_center/=wrap_area;
	}
	catch(char *sError)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		Log2File()->Log("The outline vertex of plate 0X%X has problem，形心计算有误(%s)！",handle,sError);
#else 
		Log2File()->Log("0X%X板轮廓顶点有问题，形心计算有误(%s)！",handle,sError);
#endif
	}
	return geom_center;
}
//</DEVELOP_PROCESS_MARK>
//获取指火曲线(0.第1、2面的火曲线 1.第2、3面的获取线)，为板坐标系下的相对坐标
f3dLine CLDSPlate::GetHuoquLine(int index)
{
	if(index<0||index>1)
		return f3dLine();
	else 
		return HuoQuLine[index];
}
f3dPoint CLDSPlate::GetHuoquFaceNorm(int faceI)
{
	if(faceI==0)
		return huoqufacenorm[0];
	else if(faceI==1)
		return huoqufacenorm[1];
	else 
		return f3dPoint();
}

//处理卷边（相对板相对坐标系而言）得到处理后钢板边对应点
bool CLDSGeneralPlate::ProcessRollEdgeVertex(ATOM_LIST<PROFILE_VERTEX> &vertex_list)
{
	if(vertex_list.GetNodeNum()<3)
		return FALSE;
	//TODO：还未处理卷边火曲线属性在ppi中的存储　wjh-2016.11.28
	int i,roll_edge_num=0,n=vertex_list.GetNodeNum();
	f3dLine huoquLine[2]={GetHuoquLine(0),GetHuoquLine(1)};
	ATOM_LIST<PROFILE_VERTEX> vertex_list_temp;
	for(i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=&vertex_list[i];
		PROFILE_VERTEX *pEndVertex=&vertex_list[(i+1)%n];
		vertex_list_temp.append(*pStartVertex);
		if(pStartVertex->type==0&&pStartVertex->m_bRollEdge&&pStartVertex->manu_space!=0)
		{	//输出卷边
			PROFILE_VERTEX *pPrevVertex=&vertex_list[(i+n-1)%n];
			PROFILE_VERTEX *pNextVertex=&vertex_list[(i+2)%n];
			if((pStartVertex->vertex==huoquLine[0].startPt&&pEndVertex->vertex==huoquLine[0].endPt)||
				(pStartVertex->vertex==huoquLine[1].startPt&&pEndVertex->vertex==huoquLine[1].endPt))
				continue;
			f3dLine huoqu_line,roll_edge_line;
			CalRollLineAndHuoquLine(pStartVertex,pEndVertex,pPrevVertex,pNextVertex,huoqu_line,roll_edge_line);
			//
			PROFILE_VERTEX new_vertex;
			new_vertex.type=0;
			new_vertex.vertex=roll_edge_line.startPt;
			/*if((i+roll_edge_num*2+1)%(n+roll_edge_num*2)==0)
				vertex_list.append(new_vertex);
			else
				vertex_list.insert(new_vertex,(i+roll_edge_num*2+1)%(n+roll_edge_num*2));*/
			//上下两段代码遇到双卷边时会出问题　wjh-2016.4.12
			vertex_list_temp.append(new_vertex);
			new_vertex.vertex=roll_edge_line.endPt;
			vertex_list_temp.append(new_vertex);
			/*if((i+roll_edge_num*2+2)%(n+roll_edge_num*2+1)==0)
				vertex_list.append(new_vertex);
			else
				vertex_list.insert(new_vertex,(i+roll_edge_num*2+2)%(n+roll_edge_num*2+1));*/

			roll_edge_num++;
		}
	}
	vertex_list.Empty();
	for(PROFILE_VERTEX* pVertex=vertex_list_temp.GetFirst();pVertex;pVertex=vertex_list_temp.GetNext())
		vertex_list.append(*pVertex);
	return true;
}
long CLDSGeneralPlate::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	return CLDSPart::StatObjHandleReference(pListHandlers);
}
bool CLDSGeneralPlate::ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType/*=0*/)
{
	return CLDSPart::ReplaceReferenceHandle(replacer,idClassType);
}
void CLDSGeneralPlate::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(m_nClassTypeId==CLS_PLATE)
	{
		CLDSPlate* pPlate=(CLDSPlate*)this;
		m_pModel->ValidateObjRefH(&pPlate->designInfo.m_hBaseNode);
		m_pModel->ValidateObjRefH(&pPlate->designInfo.m_hBasePart);
		for(CDesignLjPartPara* pLjRod=pPlate->designInfo.partList.GetFirst();pLjRod;pLjRod=pPlate->designInfo.partList.GetNext())
			m_pModel->ValidateObjRefH(&pLjRod->hPart);
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if( pVertex->m_cPosDesignType>=1&&pVertex->m_cPosDesignType<=4)
				m_pModel->ValidateObjRefH(&pVertex->m_hRod);
		}
	}
}
long CLDSPlate::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	return CLDSGeneralPlate::StatObjHandleReference(pListHandlers);
}
bool CLDSPlate::ReplaceReferenceHandle(CHandleReplacer& replacer, int idClassType/*=0*/)
{
	return CLDSGeneralPlate::ReplaceReferenceHandle(replacer, idClassType);
}
void CLDSPlate::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	CLDSGeneralPlate::CleanInvalidObjRef(pModel);
}

int CLDSGeneralPlate::WriteProcessBuffer(CBuffer& buffer,bool bSaveSubSerial/*=false*/,long version/*=0*/)
{
	int face_N = GetFaceN();
	f3dPoint huoquNorm[2] = { GetHuoquFaceNorm(0),GetHuoquFaceNorm(1) };
	f3dLine huoquLine[2] = { GetHuoquLine(0),GetHuoquLine(1) };
	//处理卷边（相对板相对坐标系而言）
	ATOM_LIST<PROFILE_VERTEX> vertex_list_temp;	//板顶点列表
	vertex_list_temp.Empty();
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		vertex_list_temp.append(*pVertex);				 //得到加工坐标系
	if(GetClassTypeId()==CLS_PARAMPLATE)
	{	
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)this;
		if (pParamPlate->m_iParamType == TYPE_ROLLEND)
		{	//在槽型插板轮廓点中记录卷边信息 wht 18-08-19
			double L = 0, H = 0, S = 0, N = 0, W = 0;
			pParamPlate->GetDesignItemValue('L', &L);
			pParamPlate->GetDesignItemValue('H', &H);
			pParamPlate->GetDesignItemValue('S', &S);
			pParamPlate->GetDesignItemValue('N', &N);
			pParamPlate->GetDesignItemValue('W', &W);
			if (W > 0)
			{
				PROFILE_VERTEX *pPrevVertex = vertex_list.GetTail();
				for (pVertex = vertex_list_temp.GetFirst(); pVertex; pVertex = vertex_list_temp.GetNext())
				{
					if ((pPrevVertex->vertex.IsEqual(f3dPoint(L - N, 0.5*H)) && pVertex->vertex.IsEqual(f3dPoint(0, 0.5*H))) ||
						(pPrevVertex->vertex.IsEqual(f3dPoint(0, 0.5*H)) && pVertex->vertex.IsEqual(f3dPoint(L - N, 0.5*H))) ||
						(pPrevVertex->vertex.IsEqual(f3dPoint(L - N, -0.5*H)) && pVertex->vertex.IsEqual(f3dPoint(0, -0.5*H))) ||
						(pPrevVertex->vertex.IsEqual(f3dPoint(0, -0.5*H)) && pVertex->vertex.IsEqual(f3dPoint(L - N, -0.5*H))))
					{
						pPrevVertex->m_bRollEdge = true;
						pPrevVertex->manu_space = -(int)W;
					}
					pPrevVertex = pVertex;
				}
			}
		}
		else if (pParamPlate->m_iParamType == TYPE_PEDALPLATE)
		{	//处理脚踏板轮廓点中卷边信息 wxc 19-11-14
			long A = 0, V = 0;
			double W = 0, H = 0;
			pParamPlate->GetDesignItemValue('A', &A);
			pParamPlate->GetDesignItemValue('V', &V);
			pParamPlate->GetDesignItemValue('W', &W);
			pParamPlate->GetDesignItemValue('H', &H);
			CLDSLineAngle* pDatumJg = (CLDSLineAngle*)BelongModel()->FromPartHandle(A, CLS_LINEANGLE);
			double ddx = fabs(ucs.axis_z*pDatumJg->vxWingNorm);
			double ddy = fabs(ucs.axis_z*pDatumJg->vyWingNorm);
			GEPOINT spreadVec = (ddx > ddy) ? pDatumJg->GetWingVecX() : pDatumJg->GetWingVecY();
			spreadVec *= (V == 0) ? 1 : -1;
			vector_trans(spreadVec, ucs, FALSE);
			f3dPoint linePtS, linePtE;
			for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = pParamPlate->vertex_list.GetNext())
			{
				if (pVertex->vertex.feature == PROFILE_VERTEX::DT_BEND_DATUM_POINT)
					linePtS = pVertex->vertex;
				else if (pVertex->vertex.feature == PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
					linePtE = pVertex->vertex;
			}
			//计算火曲线和卷边线
			f3dLine roll_line, huoqu_line[2], roll_edge[2];
			for (int ii = 0; ii < 2; ii++)
			{
				roll_line.startPt = (ii == 0) ? linePtS : huoqu_line[0].endPt;
				roll_line.endPt = (ii == 0) ? linePtE : roll_edge[0].endPt;
				GEPOINT vec = (ii == 0) ? GEPOINT(0, 1, 0) : spreadVec;
				GEPOINT line_vec = (roll_line.endPt - roll_line.startPt).normalized();
				GEPOINT offset_vec(line_vec.y, -line_vec.x);
				normalize(offset_vec);
				if (offset_vec*vec < 0)
					offset_vec *= -1;
				huoquNorm[ii] = offset_vec;
				double roll_len = (ii == 0) ? W : H;
				huoqu_line[ii].startPt = roll_line.startPt + offset_vec * GetThick();
				huoqu_line[ii].endPt = roll_line.endPt + offset_vec * GetThick();
				roll_edge[ii].startPt = huoqu_line[ii].startPt + offset_vec * roll_len;
				roll_edge[ii].endPt = huoqu_line[ii].endPt + offset_vec * roll_len;
			}
			//按照三面板处理，重新写入轮廓点
			face_N = 3;
			vector_trans(huoquNorm[0], ucs, TRUE);
			vector_trans(huoquNorm[1], ucs, TRUE);
			huoquLine[0] = huoqu_line[0];
			huoquLine[1] = huoqu_line[1];
			BOOL bInsertPt = TRUE;
			f3dPoint vertex_temp;
			vertex_list_temp.Empty();
			for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
			{
				vertex_temp = pVertex->vertex;
				vertex_temp.feature = 1;
				vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
				if (pVertex->vertex.feature>10 && bInsertPt)
				{
					if (pVertex->vertex.feature == PROFILE_VERTEX::DT_BEND_DATUM_POINT)
					{
						vertex_temp = huoqu_line[0].startPt;
						vertex_temp.feature = 12;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = roll_edge[0].startPt;
						vertex_temp.feature = 2;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = roll_edge[0].endPt;
						vertex_temp.feature = 2;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = huoqu_line[1].endPt;
						vertex_temp.feature = 13;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = roll_edge[1].endPt;
						vertex_temp.feature = 3;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = roll_edge[1].startPt;
						vertex_temp.feature = 3;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = huoqu_line[1].startPt;
						vertex_temp.feature = 13;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = huoqu_line[0].endPt;
						vertex_temp.feature = 12;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
					}
					else if(pVertex->vertex.feature== PROFILE_VERTEX::DT_BEND_EXTEND_POINT)
					{
						vertex_temp = huoqu_line[0].endPt;
						vertex_temp.feature = 12;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = huoqu_line[1].startPt;
						vertex_temp.feature = 13;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = roll_edge[1].startPt;
						vertex_temp.feature = 3;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = roll_edge[1].endPt;
						vertex_temp.feature = 3;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = huoqu_line[1].endPt;
						vertex_temp.feature = 13;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = roll_edge[0].endPt;
						vertex_temp.feature = 2;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = roll_edge[0].startPt;
						vertex_temp.feature = 2;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
						vertex_temp = huoqu_line[0].startPt;
						vertex_temp.feature = 12;
						vertex_list_temp.append(PROFILE_VERTEX(vertex_temp));
					}
					bInsertPt = FALSE;
				}
			}
		}
	}
	//钢板工艺信息中不计算卷边后的轮廓点，通过存储的卷边工艺信息，统一在PPE中进行处理  wxc-2017.1.17
	//if(bSaveSubSerial)	//保存子对象序号时用于ToPPIBuffer此时应处理卷边 wht 14-09-29
		//ProcessRollEdgeVertex(vertex_list_temp);//得到处理卷边后的点（vertex_list_temp）
	
	int nInitLen = buffer.GetLength();
	//1.孔数、边数及面数
	buffer.WriteWord((short)GetLsCount());					//孔数
	buffer.WriteWord((short)vertex_list_temp.GetNodeNum());	//边数
	buffer.WriteByte((BYTE)face_N);							//面数
	//2.火曲信息（钢板固接坐标系下）
	for(int i=2;i<=face_N;i++)
	{
		f3dPoint norm = huoquNorm[i-2];
		vector_trans(norm,ucs,FALSE);
		buffer.WritePoint(norm);							//(板相对坐标系下的)火曲面法线
		buffer.WritePoint(huoquLine[i-2].startPt);			//(板相对坐标系下的)火曲线起点
		buffer.WritePoint(huoquLine[i-2].endPt);			//(板相对坐标系下的)火曲线终点
		if(version>=1000003)
		{
			double r,x;
			memcpy(&r,huoquLine[i-2].layer,8);
			memcpy(&x,&huoquLine[i-2].layer[8],8);
			buffer.WriteDouble(r);
			buffer.WriteDouble(x);
		}
	}
	if(version>=1000005)
	{	//存储卷边信息及火曲变形信息  wxc-2017.1.17
		bool m_bIncDeformed=false;	//钢板轮廓点及螺栓是否考虑火曲变形，由TMA生成ppi_bufer统一不考虑
		buffer.WriteByte(m_cRollProcessType);	//卷边处理方式
		buffer.WriteByte(m_ciRollOffsetType);	//卷边线计算方式
		buffer.WriteBoolean(m_bIncDeformed);	//是否考虑火曲变形
		buffer.WriteByte(m_ciDeformType);		//火曲变形处理方式
	}
	CLsRef *pLsRef=pLsRefList->GetFirst();
	if(pLsRef)
		buffer.WriteFloat((float)(*pLsRef)->hole_d_increment);	//螺栓直径增量
	else 
		buffer.WriteFloat((float)0);
	//3.螺栓信息
	//4B：X方向位置（float）
	//4B：Y方向位置（float）。
	//2B：螺栓孔径
	//2B：腰圆孔腰长
	//8B：螺栓所在的射线号
	//24B：腰圆孔方向
	int serial=1;
	for(pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext(),serial++)
	{
		CLDSBolt *pBolt=pLsRef->GetLsPtr();
		f3dPoint ls_pos;
		SpreadLsPos(pBolt,ls_pos);
		//ls_pos=GetDeformedVertex(ls_pos);				//生成PPI不考虑火曲变形  wxc-2016.10.27
		if(bSaveSubSerial)
			buffer.WriteDword(serial);					//螺栓孔的唯一标识，在ProcessAngle初始化
		buffer.WriteFloat((float)ls_pos.x);				//螺栓X方向位置
		buffer.WriteFloat((float)ls_pos.y);				//螺栓Y方向位置
		buffer.WriteWord((short)(*pLsRef)->get_d());	//螺栓直径
		buffer.WriteWord(pLsRef->waistLen);				//腰圆孔腰长
		buffer.WriteDword(pLsRef->dwRayNo);				//螺栓所在射线号
		buffer.WritePoint((*pLsRef)->ucs.axis_x);		//腰圆孔方向
		buffer.WriteByte(pBolt->FuncType());			//螺栓孔功用类型
		if(version>=1000001)
			buffer.WriteFloat((float)pBolt->hole_d_increment);	//孔径增量
	}
	//4.板外形
	if(version>=1000003&&version<1000005)
		buffer.WriteBoolean(false);
	if(face_N==3)
	{
		f3dPoint top_point;
		if(m_nClassTypeId==CLS_PLATE)
			top_point=((CLDSPlate*)this)->top_point;
		buffer.WritePoint(top_point);
	}
	serial=1;
	vertex_list_temp.EmptyStack();
	pVertex = vertex_list_temp.GetTail();
	for(PROFILE_VERTEX* pNextVertex=vertex_list_temp.GetFirst();pNextVertex;pNextVertex=vertex_list_temp.GetNext(),serial++)
	{	
		if(bSaveSubSerial)	//标识序号
			buffer.WriteDword(serial);
		//4.1 1B：边类型(1表示直线段；2表示圆弧；3表示椭圆弧）
		if(pVertex->type==3)
			buffer.WriteByte((BYTE)3);			//椭圆弧
		else if(pVertex->type==2||pVertex->type==1)
			buffer.WriteByte((BYTE)2);			//圆弧
		else 
			buffer.WriteByte((BYTE)1);			//直线段
		//4.2 1B属性标识字节
		//1~4位表示起点归属面（如0x03表示起点同时归属于第一面和第二面，即1-2面火曲线上）
		//第8位为0时起点位置之后的后续2字节表示卷边高度，为1时表示焊缝边的加工间隙值
		//其余位保留
		BYTE  propId=0;
		if(pVertex->vertex.feature==2)
			propId = 0x02;
		else if(pVertex->vertex.feature==12)
			propId = 0x03;
		else if(pVertex->vertex.feature==3)
			propId = 0x04;
		else if(pVertex->vertex.feature ==13)
			propId = 0x05;
		else //if(vertex.feature==1)
			propId =0x01;
		//4.3 焊缝、卷边
		if(pVertex->m_bRollEdge)
			propId &=0x7F;								//标识卷边
		else if(pVertex->m_bWeldEdge)
			propId |=0x80;								//标识焊缝
		buffer.WriteByte(propId);
		f3dPoint vertice_ref = pVertex->vertex;
		//计算火曲点制弯后的坐标，导致工艺管理器中火曲线与火曲点不共线，此处待商定  wxc-2016.4.12
		//vertice_ref=GetDeformedVertex(vertice_ref);
		buffer.WriteDouble(vertice_ref.x);				//边起点X坐标
		buffer.WriteDouble(vertice_ref.y);				//边起点Y坐标
		buffer.WriteWord((short)pVertex->manu_space);	//焊缝加工预留间隙或卷边高度(mm)
		if(version>=1000006)
			buffer.WriteWord((short)pVertex->roll_edge_offset_dist);//卷边偏移距离
		if(pVertex->m_bWeldEdge)
		{	
			buffer.WriteByte((BYTE)0);		//1B：焊缝高度
			buffer.WriteWord((WORD)0);		//2B：焊缝起点距板边起点距离
			buffer.WriteWord((WORD)0);		//2B：焊缝长度，为0时表示一直延伸至板边终点
		}
		//4.4 边类型=2，3时，（椭）圆弧扇形角、圆弧面法线方向
		if(pVertex->type>0)
		{	//8B：（椭）圆弧扇形角，弧度制
			//24B：圆弧面法线方向（固接坐标系下相对方向）
			f3dArcLine arcline;
			pVertex->RetrieveArcLine(arcline,pNextVertex->vertex,&ucs);
			buffer.WriteDouble(arcline.SectorAngle());
			buffer.WritePoint(arcline.WorkNorm());  //圆弧面法线
			//4.5  边类型=3时，椭圆弧短半轴长度、椭圆弧圆心位置、椭圆弧原始定义柱面轴向
			if(pVertex->type==3)
			{
				buffer.WriteDouble(arcline.Radius());	//8B：椭圆弧短半轴长度（即原始定义圆柱面半径）
				f3dPoint center_pick=pVertex->center;
				f3dPoint columnnorm=pVertex->column_norm;
				normalize(columnnorm);
				coord_trans(center_pick,ucs,FALSE);
				vector_trans(columnnorm,ucs,FALSE);
				buffer.WritePoint(center_pick);			//24B：椭圆弧圆心位置（固接坐标系下相对坐标）
				buffer.WritePoint(columnnorm);			//24B：椭圆弧原始定义柱面轴向（固接坐标系下相对方向）
			}
		}
		pVertex=pNextVertex;
	}
	//5.加工坐标系基准边索引默认为-1及打号位置
	buffer.WriteWord((WORD)-1);		//加工坐标系基准边索引默认为-1
	buffer.WriteFloat((float)0);	//打号位置X坐标
	buffer.WriteFloat((float)0);	//打号位置Y坐标
	if(version>=1000002)
	{	//记录矩形钢印号的方向
		buffer.WriteFloat((float)1);
		buffer.WriteFloat((float)0);	
	}
	return buffer.GetLength()-nInitLen;
}
#ifdef __LDS_CONTEXT_
bool CLDSGeneralPlate::CloneToBomPart(BOMPART* pBomPart)
{	
	if(pBomPart->cPartType!=BOMPART::PLATE)
		return false;
	CLDSPart::CloneToBomPart(pBomPart);	//克隆基本属性
	//克隆角钢特有属性
	PART_PLATE *pPlate=(PART_PLATE*)pBomPart;
	pPlate->m_cFaceN=(BYTE)GetFaceN();
	pPlate->bNeedFillet=FALSE;
	IModel *pModel=BelongModel();
	pPlate->m_iPlateType=0;
	if(pModel&&pModel->IsTowerModel())
	{
		CLDSPartGroup *pPartGroup=((CTower*)pModel)->PartGroup.FromHandle(handle);
		if(pPartGroup&&pPartGroup->IdentifyAngleFoot())
			pPlate->m_iPlateType=PART_PLATE::TYPE_TOWER_FOOT;
	}
	if (GetClassTypeId() == CLS_PLATE)
	{	//计算钢板豁口长度 wht 19-07-19
		CLDSPlate *pLdsPlate = (CLDSPlate*)this;
		if (pLdsPlate->IsFoldPlate())
			pPlate->m_iPlateType = PART_PLATE::TYPE_FOLD_3FACE;
		else if (pLdsPlate->IsHuoQuLineCollinear3FacePlate())
		{
			pPlate->m_iPlateType = PART_PLATE::TYPE_CUT_MOUTH_3FACE;
			pPlate->fCutMouthLen = pLdsPlate->extendVertexOffsetLen;	//豁口长度
		}
	}

	if(pPlate->m_cFaceN>=2)
	{
		pPlate->huoQuAngleArr[0]=GetHuoQuAngle(1)*DEGTORAD_COEF;
		if(pPlate->m_cFaceN>2)
			pPlate->huoQuAngleArr[1]=GetHuoQuAngle(2)*DEGTORAD_COEF;
	}
	pPlate->min_area=CalAreaOfMinWrapRect();
	pPlate->real_area=CalAreaOfNetWrap();
	pPlate->fPieceNetWeight=CalSteelPlankWei(pPlate->real_area*pPlate->thick);
	pPlate->fPerimeter=CalPerimeter();
	pPlate->fWeldEdgeLen=CalWeldEdgeLength();
	pPlate->fWeldLineLen=CalWeldLineLength();
	pPlate->bWeldPart=(pPlate->fWeldEdgeLen>0);
	//焊接父构件或焊接子构件都归属于焊接件
	BOOL bWeldPart=FALSE;
	CLDSPart *pFatherPart=m_pModel->FromPartHandle(m_hPartWeldParent);
	if(pFatherPart==NULL)
	{	//判断该构件是否存在焊接子构件，存在则认为该构件亦为焊接件
		CLDSPart *pSonPart=NULL;
		BOOL bPush=m_pModel->PushPartStack();
		for(pSonPart=m_pModel->EnumPartFirst();pSonPart;pSonPart=m_pModel->EnumPartNext())
		{
			if(pSonPart->m_hPartWeldParent==this->handle)
			{
				bWeldPart=TRUE;
				break;
			}
		}
		if(bPush)
			m_pModel->PopPartStack();
	}
	else
		bWeldPart=TRUE;
	pPlate->bWeldPart=pPlate->bWeldPart?TRUE:bWeldPart;
	if(GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)this;
		if(bWeldPart&&pFatherPart!=NULL)
			pPlate->bNeedFillet=TRUE;	//仅有焊接父构件的钢板需要坡口工艺 wjh-2016.12-22
		if(pParamPlate->m_iParamType==TYPE_ROLLEND)
			pPlate->siSubType=BOMPART::SUB_TYPE_PLATE_C;	//槽型插板
		else if(pParamPlate->m_iParamType==TYPE_UEND)
			pPlate->siSubType=BOMPART::SUB_TYPE_PLATE_U;	//U型插板
		else if(pParamPlate->m_iParamType==TYPE_XEND)
			pPlate->siSubType=BOMPART::SUB_TYPE_PLATE_X;	//X型插板
		else if(pParamPlate->m_iParamType==TYPE_FL)
			pPlate->siSubType=BOMPART::SUB_TYPE_PLATE_FL;	//法兰
		else if(pParamPlate->m_iParamType==TYPE_FLD)
			pPlate->siSubType=BOMPART::SUB_TYPE_PLATE_FLD;	//对焊法兰
		else if(pParamPlate->m_iParamType==TYPE_FLP)
			pPlate->siSubType=BOMPART::SUB_TYPE_PLATE_FLP;	//平焊法兰
		else if(pParamPlate->m_iParamType==TYPE_FLG)
			pPlate->siSubType=BOMPART::SUB_TYPE_PLATE_FLG;	//刚性法兰
		else if(pParamPlate->m_iParamType==TYPE_FLR)
			pPlate->siSubType=BOMPART::SUB_TYPE_PLATE_FLR;	//柔性法兰
		else if(pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
			pPlate->siSubType=BOMPART::SUB_TYPE_PLATE_WATER;//遮水板

		if(pParamPlate->IsFL()&&pParamPlate->m_bStdPart)
		{
			CLDSStdPartModel stdPartModel;
			pParamPlate->GetStdPartModel(&stdPartModel);
			if(pParamPlate->m_iParamType==TYPE_FLD||pParamPlate->m_iParamType==TYPE_FLP)
			{
				pPlate->fFlInnerD=stdPartModel.param.fl_param.B;
				pPlate->fFlOutterD=stdPartModel.param.fl_param.D;
				pPlate->fFlBoltLayoutD=stdPartModel.param.fl_param.K;
			}
			else //if(pParamPlate->m_iParamType==TYPE_FLG||pParamPlate->m_iParamType==TYPE_FLR)
			{
				pPlate->fFlInnerD=stdPartModel.param.fl_param2.fInnerD;
				pPlate->fFlOutterD=stdPartModel.param.fl_param2.fOuterD;
				pPlate->fFlBoltLayoutD=stdPartModel.param.fl_param2.fBoltCirD;
			}
		}
		else if(pParamPlate->m_iParamType==TYPE_FL||pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		{
			double W=0,R=0;
			if(pParamPlate->m_iParamType==TYPE_FL)
				pParamPlate->GetDesignItemValue('R',&R);
			pParamPlate->GetDesignItemValue('W',&W);
			pPlate->fFlInnerD=R*2;
			pPlate->fFlOutterD=W;
			CLsRef *pLsRef=pParamPlate->GetFirstLsRef();
			if(pLsRef==NULL)
				pPlate->fFlBoltLayoutD=0;
			else
			{
				f3dPoint ls_pos=(*pLsRef)->ucs.origin;
				coord_trans(ls_pos,pParamPlate->ucs,FALSE);
				ls_pos.z=0;
				pPlate->fFlBoltLayoutD=ls_pos.mod()*2;
			}
		}
	}
	else if(GetClassTypeId()==CLS_PLATE)
	{
		pPlate->bNeedFillet=((CLDSPlate*)this)->m_bNeedFillet;
		if(ftoi(((CLDSPlate*)this)->m_fInnerRadius)>0)
		{
			pPlate->siSubType=BOMPART::SUB_TYPE_FOOT_FL;	//底脚法兰
			pPlate->fFlInnerD=((CLDSPlate*)this)->m_fInnerRadius*2;
			pPlate->fFlOutterD=GetWidth();
			CLsRef *pLsRef=GetFirstLsRef();
			if(pLsRef==NULL)
				pPlate->fFlBoltLayoutD=0;
			else
			{
				f3dPoint ls_pos=(*pLsRef)->ucs.origin;
				coord_trans(ls_pos,ucs,FALSE);
				ls_pos.z=0;
				pPlate->fFlBoltLayoutD=ls_pos.mod()*2;
			}
		}
		else if(((CLDSPlate*)this)->wiType==WASHER_PLATE)
			pPlate->siSubType=BOMPART::SUB_TYPE_BOLT_PAD;	//螺栓垫板
	}
	//螺栓
	pPlate->nMSumLs = GetLsCount();
	pPlate->m_arrBoltRecs.SetSize(pPlate->nMSumLs);
	int i=0;
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		pPlate->m_arrBoltRecs[i].d = ftoi((*pLsRef)->get_d());
		pPlate->m_arrBoltRecs[i].hole_d_increment = (float)(*pLsRef)->hole_d_increment;
		f3dPoint ls_pos=(*pLsRef)->ucs.origin;
		coord_trans(ls_pos,ucs,FALSE);
		pPlate->m_arrBoltRecs[i].x = (float)ls_pos.x;
		pPlate->m_arrBoltRecs[i].y = (float)ls_pos.y;
		pPlate->m_arrBoltRecs[i].cFuncType=(*pLsRef)->FuncType();
		pPlate->m_arrBoltRecs[i].cFlag=pLsRef->cFlag;
		pPlate->m_arrBoltRecs[i].waistLen=pLsRef->waistLen;
		i++;
	}
	//轮廓边信息
	pPlate->listVertex.DeleteList();
	BOOL bRecogWeldLine=TRUE;
	PROFILE_VERTEX *pVertex = NULL;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->m_bWeldEdge && pVertex->weldline.hWeldMotherPart<0X20)
			bRecogWeldLine=FALSE;
	}
	if(!bRecogWeldLine)	//智能识别焊缝线信息
		IntelliRecogWeldLine();
	pVertex = vertex_list.GetTail();
	for(PROFILE_VERTEX *pNextVertex=vertex_list.GetFirst(); pNextVertex; pNextVertex =vertex_list.GetNext())
	{
		LIST_NODE<BOMPROFILE_VERTEX> *pNode=pPlate->listVertex.AttachNode();
		pNode->data.type=pVertex->type;
		pNode->data.m_bWeldEdge=pVertex->m_bWeldEdge;
		CLDSPart *pWeldMotherPart=m_pModel->FromPartHandle(pVertex->weldline.hWeldMotherPart);
		if(pVertex->m_bWeldEdge&&pWeldMotherPart)
			pNode->data.sWeldMotherPart.Copy(pWeldMotherPart->GetPartNo());
		pNode->data.m_bRollEdge=pVertex->m_bRollEdge;
		pNode->data.roll_edge_offset_dist=pVertex->roll_edge_offset_dist;
		pNode->data.radius=pVertex->radius;
		pNode->data.sector_angle=pVertex->sector_angle;
		pNode->data.manu_space=pVertex->manu_space;
		pNode->data.vertex.Set(pVertex->vertex.x,pVertex->vertex.y);
		pNode->data.center.Set(pVertex->center.x,pVertex->center.y,pVertex->center.z);
		pNode->data.column_norm.Set(pVertex->column_norm.x,pVertex->column_norm.y,pVertex->column_norm.z);
		pNode->data.work_norm.Set(0,0,1);
		//计算钢板轮廓边长 wht  19-07-19
		f3dArcLine arcline;
		pVertex->RetrieveArcLine(arcline, pNextVertex->vertex,NULL);
		pNode->data.edge_len = arcline.Length();
		//焊缝参数
		pNode->data.cStateFlag=pVertex->weldline.cStateFlag;
		pNode->data.height=pVertex->weldline.height;
		pNode->data.length=pVertex->weldline.length;
		pNode->data.lenpos=pVertex->weldline.lenpos;
		//
		pVertex = pNextVertex;
	}
	return true;
}
#endif
bool CLDSPlate::SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL||pDestObj->GetClassTypeId()!=m_nClassTypeId)
		return false;
	return CLDSGeneralPlate::SyncMirPropTo(pDestObj,propKeyStr,dwPropFilterFlag);
}
bool CLDSPlate::SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL||pDestObj->GetClassTypeId()!=m_nClassTypeId)
		return false;
	CLDSGeneralPlate::SyncMirPropTo(pDestObj,mirmsg,propKeyStr,dwPropFilterFlag);
	CLDSPlate* pPlate=(CLDSPlate*)pDestObj;
	if((propKeyStr==NULL||strcmp(propKeyStr,"Profile")==0)&&IsSyncMirPropWith(pDestObj,"Profile"))
	{
		pPlate->m_bAttachPartByWeldRoad=m_bAttachPartByWeldRoad;
		if((mirmsg.axis_flag&0x01)||(mirmsg.axis_flag&0x02)||(mirmsg.axis_flag&0x10))
			pPlate->bClockwiseCalProfile=!bClockwiseCalProfile;	//X、Y及平面镜像对称，与原始钢板相反 wht 16-05-20
		else 
			pPlate->bClockwiseCalProfile=bClockwiseCalProfile;
		SyncPlateProfile((CLDSPlate*)this,(CLDSPlate*)pPlate,mirmsg);
		pPlate->DesignPlate();
	}
	return true;
}
BYTE CLDSPlate::FuncType()
{	//人为设置
	if(m_cFuncType>0)
		return m_cFuncType;
	//智能判断
	if(wiType==WASHER_PLATE)
		m_cFuncType=CLDSPart::FUNC_GASKET;
	return m_cFuncType;
}
double CLDSPlate::GetHuoQuR(int iFace/*=0*/)
{
	double fHuoquR=0;
	if(_ciDeformType==0XFF)
		fHuoquR=BelongModel()->GetHuoquR();
	else if(_ciDeformType==1)	//用户指定火曲半径
		memcpy(&fHuoquR,HuoQuLine[iFace].layer,8);
	else
		fHuoquR=_fHuoquR;
	return fHuoquR;
}
double CLDSPlate::GetDeformCoef(int iFace/*=0*/)
{
	double fDeformCoef=0;
	if(_ciDeformType==0XFF)
		fDeformCoef=BelongModel()->GetDeformCoef();
	else if(_ciDeformType==1)	//用户指定中性层系数
		memcpy(&fDeformCoef,&HuoQuLine[iFace].layer[8],8);
	else if(_ciDeformType==2)
		fDeformCoef=1;
	else //if(_ciDeformType==3)
		fDeformCoef=0;	
	return fDeformCoef;
}
void CLDSPlate::SetHuoQuR(double r/*=0*/,int iFace/*=0*/)
{
	if(m_ciDeformType==1)
		memcpy(HuoQuLine[iFace].layer,&r,8);
	else if(iFace==0)
		_fHuoquR=r;
}
void CLDSPlate::SetDeformCoef(double deform_coef/*=0.35*/,int iFace/*=0*/)
{
	if(m_ciDeformType==1)
		memcpy(&HuoQuLine[iFace].layer[8],&deform_coef,8);
	else if(iFace==0)
		_fDeformCoef=deform_coef;
}
//
double CLDSGeneralPlate::GetDeformCoef(int iFace/*=0*/)
{
	if(_ciDeformType==0XFF)
		return BelongModel()->GetDeformCoef();
	else if(_ciDeformType==1)
		return _fDeformCoef;
	else if(_ciDeformType==2)
		return 1;
	else //if(_ciDeformType==3)
		return 0;
}
double CLDSGeneralPlate::GetHuoQuR(int iFace/*=0*/)
{
	if(_ciDeformType==0XFF)
		return BelongModel()->GetHuoquR();
	else
		return _fHuoquR;
}
IMPLEMENT_GET(CLDSGeneralPlate,m_fDeformCoef) {return GetDeformCoef(0);}
IMPLEMENT_SET(CLDSGeneralPlate,m_fDeformCoef) {SetDeformCoef(value,0);}
IMPLEMENT_GET(CLDSGeneralPlate,m_fDeformCoef2){return GetDeformCoef(1);}
IMPLEMENT_SET(CLDSGeneralPlate,m_fDeformCoef2){SetDeformCoef(value,1);}
IMPLEMENT_GET(CLDSGeneralPlate,m_fHuoquR) {return GetHuoQuR(0);}
IMPLEMENT_SET(CLDSGeneralPlate,m_fHuoquR) {SetHuoQuR(value,0);}
IMPLEMENT_GET(CLDSGeneralPlate,m_fHuoquR2){return GetHuoQuR(1);}
IMPLEMENT_SET(CLDSGeneralPlate,m_fHuoquR2){SetHuoQuR(value,1);}
#endif
//<LOCALE_TRANSLATE/>
// PlankDrawing.cpp: implementation of the CPlankDrawing class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdarx.h"
#include "PlankDrawing.h"
#include "Query.h"
#include "sysPara.h"
#include "env_def.h"
#include "LocalFeatureDef.h"
#include "SortFunc.h"
#include "ArrayList.h"
#include ".\LdsDrwLocale.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifndef __TSA_FILE_
double ROTATEDIM_STRU::PRECISION=1.0;
CXhChar16 ROTATEDIM_STRU::DimText()
{
	CXhChar16 sTemp;
	if(sDimText.GetLength()>0)
	{
		sTemp.Copy(sDimText);
		SimplifiedNumString((char*)sTemp);
	}
	else
	{
		if(bDiameterStyle)
			sTemp.Copy("%%%%c");
		if(PRECISION==0.1)
			sTemp.Printf("%s%.1f",(char*)sTemp,dist);
		else if(PRECISION==0.5) 
		{
			double dimdist=fto_halfi(dist);
			if(dimdist==(int)dimdist)
				sTemp.Printf("%s%.0f",(char*)sTemp,dimdist);
			else
				sTemp.Printf("%s%.1f",(char*)sTemp,dimdist);
		}
		else
			sTemp.Printf("%s%.0f",(char*)sTemp,dist);
		SimplifiedNumString((char*)sTemp);
		if(nHits>1)
		{	//多次连续标注合并
			CXhChar16 temp;
			temp.Printf("X%d",nHits);
			sTemp.Append(temp);
		}
	}
	return sTemp;
}
//注意：dcs有值时，pPrev->vertex及pCurrent->vertex为dcs下坐标
f3dArcLine CreateIsometricEdgeLine(PROFILE_VERTEX* pPrev,PROFILE_VERTEX* pCurrent,CLDSPart* pPart,UCS_STRU *dcs/*=NULL*/,double inner_offset/*=0*/)
{
	f3dArcLine arcline;
	f3dLine chord(pPrev->vertex,pCurrent->vertex);
	chord.startPt.z=chord.endPt.z=0;
	if(pPrev->type==1)	//指定圆弧夹角
		arcline.CreateMethod2(chord.startPt,chord.endPt,f3dPoint(0,0,1),pPrev->sector_angle);
	else if(pPrev->type==2)	//指定圆弧半径
		//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wjh-2012.6.14
		arcline.CreateMethod3(chord.startPt,chord.endPt,f3dPoint(0,0,1),pPrev->radius,pPrev->center);
	else if(pPrev->type==3)
	{
		int sign=1;
		if(pPrev->radius<0)
			sign=-1;
		f3dPoint center=pPrev->center;
		f3dPoint col_axis=pPrev->column_norm;
		int face_i=max(pPrev->vertex.feature%10,pCurrent->vertex.feature%10);
		if( pPart&&pPart->GetClassTypeId()==CLS_PLATE)
		{	//制弯面上的椭圆弧，首先应将圆心标定点及柱面法线旋转展开至基面上再进行坐标系转换 wht 11-05-19
			CLDSPlate* pPlate=(CLDSPlate*)pPart;
			vector_trans(col_axis,pPlate->ucs,FALSE);	//转换到普通钢板装配坐标系下
			coord_trans(center,pPlate->ucs,FALSE);		//转换到普通钢板装配坐标系下
			//在装配坐标系下对圆柱线及圆心拾取点进行展开操作
			center=pPlate->GetSpreadVertex(center,face_i);
			col_axis=pPlate->GetSpreadVertex(col_axis,face_i,false);
			if(dcs)
			{	//恢复圆柱线及圆心拾取点到绘图坐标系下
				vector_trans(col_axis,pPlate->ucs,TRUE,TRUE);	//转换到普通钢板装配坐标系下
				coord_trans(center,pPlate->ucs,TRUE,TRUE);		//转换到普通钢板装配坐标系下
				vector_trans(col_axis,*dcs,FALSE,TRUE);	//转换到普通钢板装配坐标系下
				coord_trans(center,*dcs,FALSE,TRUE);		//转换到普通钢板装配坐标系下
			}
		}
		else if(pPart&&pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{	//制弯面上的椭圆弧，首先应将圆心标定点及柱面法线旋转展开至基面上再进行坐标系转换 wht 11-05-19
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*)pPart;
			center=pParamPlate->SpreadTransV(center);
			col_axis=pParamPlate->SpreadTransV(col_axis,false);
			vector_trans(col_axis,pParamPlate->ucs,FALSE);
			coord_trans(center,pParamPlate->ucs,FALSE);
		}
		else if(dcs)
		{
			coord_trans(center,*dcs,FALSE);
			vector_trans(col_axis,*dcs,FALSE);
		}
		arcline.CreateEllipse(center,chord.startPt,chord.endPt,col_axis,f3dPoint(0,0,sign),sign*pPrev->radius);
	}
	if(inner_offset!=0)	//需要等距偏移
	{
		if(arcline.SectorAngle()==0)	//直线
		{	//等距偏移直线边
			f3dPoint line_vec=chord.endPt-chord.startPt;
			normalize(line_vec);
			f3dPoint perp_vec(-line_vec.y,line_vec.x);
			chord.startPt+=perp_vec*inner_offset;
			chord.endPt+=perp_vec*inner_offset;
		}
		else
		{
			if(arcline.WorkNorm().z>0)	//凸圆弧边
				arcline.SetRadius(arcline.Radius()-inner_offset);
			else
				arcline.SetRadius(arcline.Radius()+inner_offset);
		}
	}
	if(arcline.SectorAngle()==0)
		return f3dArcLine(chord);
	else
		return arcline;
}
//判断钢板火曲类型 冷弯/热弯  TRUE---冷弯  FALSE---热弯
static BOOL ConfirmPlateHuoquType(double fPlateThick,double fHuoquAngle)
{
	if(((fPlateThick-2.000001)<0&&(fHuoquAngle-89.999999)<0)								//thick<=2 && angle<90
		||((fPlateThick-5.000001)>0&&(fPlateThick-7.999999)<0&&(fHuoquAngle-44.999999)<0)	//5<thick<8 && angle<45 
		||((fPlateThick-9.000001)>0&&(fPlateThick-9.999999)<0&&(fHuoquAngle-19.999999)<0)	//9<thick<10 && angle<20
		||((fPlateThick-10.000001)>0&&(fHuoquAngle-9.999999)<0))							//thick>10 && angle<10
		return TRUE;	//冷弯
	else
		return FALSE;	//热弯
}

//得到钢板火曲工艺字符串
static int GetPlateHuoQuProcessStr(TXTDIM_STRU& txt_dim,BOOL bNeiQu,double fHuoquAngle,double fDeformCoef,double fPlateThick)
{
	//正反曲、外内曲
#ifdef AFX_TARG_ENU_ENGLISH
	char sWaiQu[16]="outer blending",sNeiQu[16]="inner blending"; //inner and outter blending
	if(sys.part_map.bUseZhengFanQun)
	{	//front and reverse blending
		strcpy(sWaiQu,"front blending");
		strcpy(sNeiQu,"reverse blending");
#else
	char sWaiQu[16]="外曲",sNeiQu[16]="内曲"; //内外曲
	if(sys.part_map.bUseZhengFanQun)
	{	//正反曲
		strcpy(sWaiQu,"正曲");
		strcpy(sNeiQu,"反曲");
#endif
	}
	if(sys.part_map.bDimPlaneAngle)	
	{	//标注两火曲面之间的夹角
		if(!bNeiQu)	//火曲面向外曲(正曲)
			sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,fabs(fHuoquAngle));	//由于算法变更当前fHuoquAngle由CLDSPlate::GetHuoQuAngle获取，负数表外曲故取绝对值 wjh-2015.11.25
		else		//火曲面向内曲(反曲)
			sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,fabs(fHuoquAngle));
		char sPlaneAngle[50];
		double fPlaneAngle=180-fabs(fHuoquAngle);
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sPlaneAngle,"angle of two planes %.1f°",fPlaneAngle);
#else
		sprintf(sPlaneAngle,"两面夹角%.1f°",fPlaneAngle);
#endif
		strcat(txt_dim.dimText,sPlaneAngle);
	}
	else 
	{	//不标注两面夹角时 支持选择火曲角度类型:锐角、钝角或者自动判断 wht 10-11-18
		if(sys.part_map.iDimAngleType==0)
		{	//标注锐角
			if(!bNeiQu)	
			{	//火曲面向外曲(正曲)
				if(fabs(fHuoquAngle)<90)	//锐角
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,fabs(fHuoquAngle));
				else						//converts obtuse angle to acute angle
					sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,180-fabs(fHuoquAngle));
#else
					sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,fabs(fHuoquAngle));
				else						//将钝角转换为锐角
					sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,180-fabs(fHuoquAngle));
#endif
			}
			else
			{	//火曲面向内曲(反曲)
				if(fabs(fHuoquAngle)<90)	//锐角
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,fabs(fHuoquAngle));
				else						//converts obtuse angle to acute angle
					sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,180-fabs(fHuoquAngle));
#else
					sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,fabs(fHuoquAngle));
				else						//将钝角转换为锐角
					sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,180-fabs(fHuoquAngle));
#endif
			}
		}
		else if(sys.part_map.iDimAngleType==1)
		{	//标注钝角
			if(!bNeiQu)	
			{	//火曲面向外曲(正曲)
				if(fabs(fHuoquAngle)>90)	//钝角
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,fabs(fHuoquAngle));
				else						//converts acute angle to obtuse angle
					sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,180-fabs(fHuoquAngle));
#else
					sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,fabs(fHuoquAngle));
				else						//将锐角转换为钝角
					sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,180-fabs(fHuoquAngle));
#endif
			}
			else
			{	//火曲面向内曲(反曲)
				if(fabs(fHuoquAngle)>90)	//钝角
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,fabs(fHuoquAngle));
				else						//converts acute angle to obtuse angle
					sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,180-fabs(fHuoquAngle));
#else
					sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,fabs(fHuoquAngle));
				else						//将锐角转换为钝角
					sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,180-fabs(fHuoquAngle));
#endif
			}
		}
		else
		{	//自动选择
			if(!bNeiQu)	//火曲面向外曲(正曲)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,fabs(fHuoquAngle));
			else		//blended plane is inner blending (reverse blending)
				sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,fabs(fHuoquAngle));
#else
				sprintf(txt_dim.dimText,"%s%.1f°",sWaiQu,fabs(fHuoquAngle));
			else		//火曲面向内曲(反曲)
				sprintf(txt_dim.dimText,"%s%.1f°",sNeiQu,fabs(fHuoquAngle));
#endif
		}
	}
	if(sys.part_map.bDimHuoquType)		//标注火曲类型 冷弯热弯
	{
		char sHuoquType[50];
		if(ConfirmPlateHuoquType(fPlateThick,fabs(fHuoquAngle)))
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sHuoquType,"(cold blending)");
		else
			strcpy(sHuoquType,"(hot blending)");
#else
			strcpy(sHuoquType,"(冷弯)");
		else
			strcpy(sHuoquType,"(热弯)");
#endif
		strcat(txt_dim.dimText,sHuoquType);
	}
	if(sys.part_map.bExportDeformedProfile
		&&sys.part_map.bDimDeformedProfileText)//标注火曲变形且标注火曲变形文字 wht 10-11-01
	{
		char sDeformedProfile[50];
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sDeformedProfile,"Size to the neutral layer(%.2ft)",fDeformCoef);
#else
		sprintf(sDeformedProfile,"尺寸指向中性层(%.2ft)",fDeformCoef);
#endif
		strcat(txt_dim.dimText,sDeformedProfile);
	}
	return strlen(txt_dim.dimText);
}
//调整钢板坐标系，返回最长边的长度 
	//1.保证Z轴方向不变
	//2.Y轴垂直于最长边，且指向钢板的几何中心
	//3.保证X轴方向与最长边在一个方向上(相反或相同用Y轴与Z轴的方向来确定)，
static double AdjustPlateUcs(CLDSPart *pPart,UCS_STRU &new_ucs,f3dPoint &geom_center,short longedge0_nature1=0)
{	
	new_ucs=pPart->ucs;	//初始化坐标系
	PROFILE_VERTEX *pVertex=NULL,*pNextVertex=NULL;
	if(pPart==NULL)
		return 0;
	if(pPart->GetClassTypeId()!=CLS_PLATE&&pPart->GetClassTypeId()!=CLS_PARAMPLATE)
		return 0;
	CLDSParamPlate *pParamPlate=NULL;
	if(pPart->GetClassTypeId()==CLS_PLATE&&longedge0_nature1==1&&
		(pPart->Layer(2)=='Q'||pPart->Layer(2)=='H'||pPart->Layer(2)=='Z'||pPart->Layer(2)=='Y'||(pPart->Layer(2)>='1'&&pPart->Layer(2)<='6')))
	{
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		if(pPlate->vertex_list.GetNodeNum()<3)
			return 0;
		geom_center.Set();
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pPlate->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=pPlate->vertex_list.GetNodeNum();
		project_point(geom_center,pPart->ucs.origin,pPart->ucs.axis_z);
		new_ucs=pPart->ucs;
		if(fabs(pPart->ucs.axis_z.z)<0.3)//接近竖直的钢板
		{
			if(fabs(pPart->ucs.axis_y.z)<fabs(pPart->ucs.axis_x.z))
			{
				new_ucs.axis_y= pPart->ucs.axis_x;
				new_ucs.axis_x=-pPart->ucs.axis_y;
			}
			if(new_ucs.axis_y.z>0)
			{
				new_ucs.axis_y*=-1;
				new_ucs.axis_x*=-1;
			}
			return 1;
		}
	}
	else if(longedge0_nature1==1)
		return 0;	//自然状态调整坐标系失败，应保留原有装配坐标系绘图

	int i,nVertexNum=0;
	double max_len=0;
	f3dLine maxLenEdge;	//最长边
	f3dPoint vertex,start_vertex,end_vertex;
	if(pPart->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)pPart;
		if(pPlate->vertex_list.GetNodeNum()<3)
			return 0;
		nVertexNum=pPlate->vertex_list.GetNodeNum();
		for(i=0;i<nVertexNum;i++)
		{
			pVertex=&pPlate->vertex_list[i];
			pNextVertex=&pPlate->vertex_list[(i+1)%nVertexNum];
			vertex=pVertex->vertex;
			coord_trans(vertex,pPlate->ucs,TRUE);
			geom_center+=vertex;
			if(pVertex&&pVertex->m_bRollEdge&&pVertex->manu_space!=0)
			{
				PROFILE_VERTEX *pPrevVertex=&pPlate->vertex_list[(i+nVertexNum-1)%nVertexNum];
				PROFILE_VERTEX *pNextNextVertex=&pPlate->vertex_list[(i+1)%nVertexNum];
				f3dLine huoqu_line,roll_edge_line;
				pPlate->CalRollLineAndHuoquLine(pVertex,pNextVertex,pPrevVertex,pNextNextVertex,huoqu_line,roll_edge_line);
				double cur_len=DISTANCE(roll_edge_line.startPt,roll_edge_line.endPt);
				if(cur_len>max_len)
				{
					max_len=cur_len;
					maxLenEdge=roll_edge_line;
				}
			}
			else if(pVertex&&pVertex->type==0)
			{	//查找直线边中最长的边
				double cur_len=DISTANCE(pVertex->vertex,pNextVertex->vertex);
				if(cur_len>max_len)
				{
					max_len=cur_len;
					maxLenEdge.startPt=pVertex->vertex;
					maxLenEdge.endPt=pNextVertex->vertex;
				}
			}
		}
	}
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pParamPlate=(CLDSParamPlate*)pPart;
		//由于某些参数化板参数可能被破坏重新设计后将得不到正确的外形 
		//顶点数小于3时才重新设计该参数化板	wht 11-07-06
		if(pParamPlate->vertex_list.GetNodeNum()<3)
			pParamPlate->DesignPlate();
		if(pParamPlate->vertex_list.GetNodeNum()<3)
			return 0;
		nVertexNum=pParamPlate->vertex_list.GetNodeNum();
		for(i=0;i<nVertexNum;i++)
		{
			pVertex=&pParamPlate->vertex_list[i];
			pNextVertex=&pParamPlate->vertex_list[(i+1)%nVertexNum];
			vertex=pVertex->vertex;
			coord_trans(vertex,pParamPlate->ucs,TRUE);
			geom_center+=vertex;
			if(pVertex&&pVertex->type==0)
			{	//查找直线边中最长的边
				double cur_len=DISTANCE(pVertex->vertex,pNextVertex->vertex);
				if(cur_len>max_len)
				{
					max_len=cur_len;
					maxLenEdge.startPt=pVertex->vertex;
					maxLenEdge.endPt=pNextVertex->vertex;
				}
			}
		}
	} 
	//根据板的重心点计算板的延伸方向
	geom_center/=nVertexNum;
	project_point(geom_center,pPart->ucs.origin,pPart->ucs.axis_z);
	if(pParamPlate&&pParamPlate->m_iParamType==TYPE_ROLLEND)
	{	//槽型插板
		double odd_len=4*pParamPlate->thick/3;//卷边造成的板高火曲变形缩短值
		double H=0,W=0;
		pParamPlate->GetDesignItemValue('H',&H);
		pParamPlate->GetDesignItemValue('W',&W);
		f3dPoint origin(0,-0.5*H-W+odd_len);
		coord_trans(origin,pParamPlate->ucs,TRUE);
		new_ucs=pParamPlate->ucs;
		new_ucs.origin=origin;
	}
	else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_XEND)
	{	//十字插板
		double H=0,L=0;
		pParamPlate->GetDesignItemValue('H',&H);
		pParamPlate->GetDesignItemValue('L',&L);
		f3dPoint origin(0,-0.5*H);
		coord_trans(origin,pParamPlate->ucs,TRUE);
		new_ucs=pParamPlate->ucs;
		new_ucs.origin=origin;
	}
	else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{	//环向封板
		//环向封板中心
		double R=0,W=0;
		pParamPlate->GetDesignItemValue('R',&R);
		pParamPlate->GetDesignItemValue('W',&W);
		geom_center.Set(R+0.1*W,0);
		coord_trans(geom_center,pParamPlate->ucs,TRUE);
		//重新计算坐标系
		f3dPoint startPt=pParamPlate->vertex_list.GetFirst()->vertex;
		f3dPoint endPt=pParamPlate->vertex_list.GetTail()->vertex;
		coord_trans(startPt,pParamPlate->ucs,TRUE);
		coord_trans(endPt,pParamPlate->ucs,TRUE);
		new_ucs.origin=endPt;
		new_ucs.axis_x=startPt-endPt;
		normalize(new_ucs.axis_x);
		new_ucs.axis_z=pParamPlate->ucs.axis_z;
		new_ucs.axis_y=new_ucs.axis_z^new_ucs.axis_x;
		normalize(new_ucs.axis_y);
		new_ucs.axis_x=new_ucs.axis_y^new_ucs.axis_z;
		normalize(new_ucs.axis_x);
		new_ucs.axis_z=new_ucs.axis_x^new_ucs.axis_y;
		normalize(new_ucs.axis_z);
	}
	else if(pParamPlate&&pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{	//跨越钢管连板
		new_ucs=pParamPlate->ucs;
		f3dPoint origin=pParamPlate->vertex_list.GetTail()->vertex;
		PROFILE_VERTEX *pPrevVertex=pParamPlate->vertex_list.GetPrev();
		while(pPrevVertex->type!=0)
			pPrevVertex=pParamPlate->vertex_list.GetPrev();
		if(pPrevVertex&&pPrevVertex->type==0)
		{
			origin=pPrevVertex->vertex;
			coord_trans(origin,pParamPlate->ucs,TRUE);
			new_ucs.origin=origin;
		}
	}
	else
	{
		if(max_len==0)
			return 0;
		//定义新坐标系
		start_vertex=maxLenEdge.startPt;
		coord_trans(start_vertex,pPart->ucs,TRUE);
		end_vertex=maxLenEdge.endPt;
		coord_trans(end_vertex,pPart->ucs,TRUE);
		new_ucs.origin=start_vertex;//新坐标系坐标原点初始位置
		f3dPoint line_vec;
		line_vec=end_vertex-start_vertex;
		normalize(line_vec);
		new_ucs.axis_x=line_vec;	//X轴方向初始位置
		new_ucs.axis_z=pPart->ucs.axis_z;
		new_ucs.axis_y=new_ucs.axis_z^new_ucs.axis_x;
		normalize(new_ucs.axis_y);	//Y轴方向初始位置
		//根据板的几何中心位置确定Y轴方向
		f3dPoint center_vec=geom_center;
		coord_trans(center_vec,new_ucs,FALSE);
		if(center_vec.y<0)
			new_ucs.axis_y*=-1.0;
		//根据Y轴与Z轴方向重新计算X轴方向
		new_ucs.axis_x=new_ucs.axis_y^new_ucs.axis_z;
		normalize(new_ucs.axis_x);
		//根据X轴方向确定坐标原点位置
		if(line_vec*new_ucs.axis_x>0)
			new_ucs.origin=start_vertex;
		else
			new_ucs.origin=end_vertex;
	}
	//返回最长边的长度
	return max_len;	
}

static f3dArcLine LineToArc(f3dLine &line)
{
	f3dArcLine arcline;
	arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
	arcline.feature=line.feature;
	arcline.pen=line.pen;
	arcline.ID=line.ID;
	return arcline;
}

static int compare_func1(const RAYROD_AXIS &pRay1,const RAYROD_AXIS &pRay2)
{
	if(pRay1.fAngle>pRay2.fAngle)
		return 1;
	else if(pRay1.fAngle<pRay2.fAngle)
		return -1;
	else
		return 0;
	return 0;
}
static int compare_func2(const ANGULARDIM_STRU &pAngleDim1,const ANGULARDIM_STRU &pAngleDim2)
{
	if(pAngleDim1.angle>pAngleDim2.angle)
		return 1;
	else if(pAngleDim1.angle<pAngleDim2.angle)
		return -1;
	else
		return 0;
	return 0;
}
double WrapLine2dcc(f2dCircle cir1,f2dCircle cir2,f2dLine &wrap_tan_line);
//////////////////////////////////////////////////////////////////////////
//CWeldPlateDrawing
CWeldPlateDrawing::CWeldPlateDrawing()
{
	InitWeldDrawing();	
}
CWeldPlateDrawing::CWeldPlateDrawing(CPlankDrawing* pParentDrawing)
{
	InitWeldDrawing(pParentDrawing);
}
CWeldPlateDrawing::~CWeldPlateDrawing()
{

}
void CWeldPlateDrawing::InitWeldDrawing(CPlankDrawing* pParentDrawing/*=NULL*/)
{
	m_pPlate=NULL;
	m_fWeldThick=0;
	m_fWeldLen=0;
	m_fTxtOff=0;
	m_bFrontWeld=TRUE;
	m_xEntContext.InitContext(this,STORE_XERO);
	if(pParentDrawing)
		m_pBelongDrawing=pParentDrawing;
}
//通过焊缝线,竖直绘制钢板轮廓边
BOOL CWeldPlateDrawing::CreatePlateSideEdge(CLDSGeneralPlate *pSubPlate,long hDatumPlate,UCS_STRU& draw_ucs)
{
	if(pSubPlate==NULL)
		return FALSE;
	if(fabs(draw_ucs.axis_z*pSubPlate->ucs.axis_z)>=0.707)	//过滤水平绘制
		return FALSE;
	m_pPlate=pSubPlate;
	f3dLine main_datum_line;
	CLDSGeneralPlate* pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(hDatumPlate,CLS_PLATE,CLS_PARAMPLATE);
	BOOL bTransPlate=pDatumPlate->GetClassTypeId()==CLS_PLATE&&((CLDSPlate*)pDatumPlate)->IsTransPlate();
	BOOL bCirFootPlate=FALSE;
	if(pDatumPlate&&pDatumPlate->GetClassTypeId()==CLS_PLATE)
	{
		CLDSPlate *pPlate=(CLDSPlate*)pDatumPlate;
		bCirFootPlate=(pPlate->m_fInnerRadius>0&&!pPlate->m_tInnerColumnNorm.IsZero());
	}
	CLDSLineTube *pDatumTube=(CLDSLineTube*)console.FromPartHandle(pDatumPlate->m_hPartWeldParent,CLS_LINETUBE);
	if(pDatumTube&&!bTransPlate)
	{	//
		GEPOINT datum_line_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_line_vec);
		PROFILE_VERTEX *pPrevVertex=pDatumPlate->vertex_list.GetTail();
		for(PROFILE_VERTEX* pVertex=pDatumPlate->vertex_list.GetFirst();pVertex;pVertex=pDatumPlate->vertex_list.GetNext())
		{
			GEPOINT edge_line_vec=pPrevVertex->vertex-pVertex->vertex;
			normalize(edge_line_vec);
			vector_trans(edge_line_vec,pDatumPlate->ucs,TRUE);
			if(fabs(datum_line_vec*edge_line_vec)>EPS_COS2)
			{
				main_datum_line.startPt=pPrevVertex->vertex;
				main_datum_line.endPt=pVertex->vertex;
				coord_trans(main_datum_line.startPt,pDatumPlate->ucs,TRUE);
				coord_trans(main_datum_line.endPt,pDatumPlate->ucs,TRUE);
				coord_trans(main_datum_line.startPt,draw_ucs,FALSE);
				coord_trans(main_datum_line.endPt,draw_ucs,FALSE);
				main_datum_line.startPt.z=main_datum_line.endPt.z=0;
				break;
			}
			pPrevVertex=pVertex;
		}
	}
	//计算板投影坐标系,坐标原点位于两面相交线
	BOOL bPrjQuFace[2]={FALSE,FALSE};
	UCS_STRU prj_ucs,prj_qu_ucs[2];
	f3dPoint prjAxisZ=pSubPlate->ucs.axis_z;
	f3dPoint origin=pSubPlate->ucs.origin+pSubPlate->GetNormOffset()*prjAxisZ;
	f3dPoint line_vec=prjAxisZ^draw_ucs.axis_z;
	f3dPoint extend_vec=prjAxisZ^line_vec;
	Int3dlf(prj_ucs.origin,origin,extend_vec,draw_ucs.origin,draw_ucs.axis_z);	//使投影原点位于两面交线上
	prj_ucs.axis_y=prjAxisZ;
	prj_ucs.axis_z=draw_ucs.axis_z;
	prj_ucs.axis_x=prj_ucs.axis_y^prj_ucs.axis_z;
	prj_ucs.axis_y=prj_ucs.axis_z^prj_ucs.axis_x;
	normalize(prj_ucs.axis_x);
	normalize(prj_ucs.axis_y);
	for(int i=0;i<pSubPlate->GetFaceN()-1;i++)
	{
		f3dPoint huoqu_norm=pSubPlate->GetHuoquFaceNorm(i);
		f3dLine huoqu_line=pSubPlate->GetHuoquLine(i);
		f3dPoint vec=pSubPlate->ucs.axis_z^huoqu_norm;
		normalize(vec);
		if(fabs(vec*draw_ucs.axis_z)>0.9)
		{
			bPrjQuFace[i]=TRUE;
			coord_trans(huoqu_line.startPt,pSubPlate->ucs,TRUE);
			coord_trans(huoqu_line.endPt,pSubPlate->ucs,TRUE);
			Int3dlf(origin,huoqu_line,draw_ucs.origin,draw_ucs.axis_z);
			prj_qu_ucs[i].origin=origin;
			prj_qu_ucs[i].axis_y=huoqu_norm;
			prj_qu_ucs[i].axis_z=draw_ucs.axis_z;
			prj_qu_ucs[i].axis_x=prj_qu_ucs[i].axis_y^prj_qu_ucs[i].axis_z;
			prj_qu_ucs[i].axis_y=prj_qu_ucs[i].axis_z^prj_qu_ucs[i].axis_x;
			normalize(prj_qu_ucs[i].axis_x);
			normalize(prj_qu_ucs[i].axis_y);
		}
	}
	//根据钢板焊缝线，提取特征轮廓边
	f3dPoint geom_center;
	f3dLine base_face_line,qu_face_line_arr[2];
	int n = pSubPlate->vertex_list.GetNodeNum();
	int nArcPointCount=0,nBaseWeldEdgeCount=0;
	for(int i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=pSubPlate->vertex_list.GetByIndex(i);
		PROFILE_VERTEX *pEndVertex=pSubPlate->vertex_list.GetByIndex((i+1)%n);
		if(pStartVertex->type>0)
		{	//圆弧边简化为多条线段处理，计算钢板几何中心 wht 18-08-12
			f3dArcLine arcLine;
			pStartVertex->RetrieveArcLine(arcLine,pEndVertex->vertex,&pSubPlate->ucs);
			int nSlices=CalArcResolution(arcLine.Radius(),arcLine.SectorAngle(),1.0,5.0,18);
			double angle=arcLine.SectorAngle()/nSlices;
			for(int i=1;i<nSlices;i++)
			{
				f3dPoint pos=arcLine.PositionInAngle(angle*i);
				pos=pSubPlate->GetRealVertex(pos);
				coord_trans(pos,pSubPlate->ucs,TRUE);
				geom_center+=pos;
			}
			nArcPointCount+=nSlices;
		}
		f3dPoint f_start = pSubPlate->GetRealVertex(pStartVertex->vertex);
		f3dPoint f_end   = pSubPlate->GetRealVertex(pEndVertex->vertex);
		if(pSubPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{	//
			CLDSParamPlate *pSubParamPlate=(CLDSParamPlate*)pSubPlate;
			double E=0;
			long hDatumPart=0,hAssPart1=0,hAssPart2=0;
			if(pSubParamPlate->GetDesignItemValue('A',&hDatumPart))//基准构件
			{	//暂时只处理钢管塔脚肋板 wht 18-08-13
				pSubParamPlate->GetDesignItemValue('C',&hAssPart1);	//辅助构件1
				pSubParamPlate->GetDesignItemValue('D',&hAssPart2);	//辅助构件2
				CLDSPart *pDatumPart=console.FromPartHandle(abs(hDatumPart),CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
				CLDSPart *pAssistantPart1=console.FromPartHandle(abs(hAssPart1),CLS_PLATE,CLS_PARAMPLATE);
				CLDSPart *pAssistantPart2=console.FromPartHandle(hAssPart2,CLS_PLATE,CLS_PARAMPLATE);
				if( pDatumPart!=NULL&&pAssistantPart1!=NULL&&pAssistantPart2==NULL&&
					pDatumPart->GetClassTypeId()==CLS_LINETUBE&&pAssistantPart1->IsPlate())
				{	//肘板型肋板,焊接边从0,0位置开始 wht 18-08-14
					CLDSParamPlate::RIBPLATE_ELBOW ribPlate;
					ribPlate.LoadParams(pSubParamPlate);
					if(fabs(fabs(f_start.x)-ribPlate.E.fValue)<EPS&&fabs(f_start.y)<EPS)
						f_start.x=0;
					else if(fabs(fabs(f_end.x)-ribPlate.E.fValue)<EPS&&fabs(f_end.y)<EPS)
						f_end.x=0;
				}
			}
		}
		coord_trans(f_start,pSubPlate->ucs,TRUE);
		coord_trans(f_end,pSubPlate->ucs,TRUE);
		geom_center+=f_start;
		if(!pStartVertex->m_bWeldEdge||pStartVertex->weldline.hWeldMotherPart!=hDatumPlate)
			continue;
		int start_max_face=max(f_start.feature/10,f_start.feature%10);
		int end_max_face=max(f_end.feature/10,f_end.feature%10);
		if(start_max_face==0)
			start_max_face=1;
		if(end_max_face==0)
			end_max_face=1;
		if(start_max_face==1||end_max_face==1)
		{	//第一面上的板顶点
			f3dPoint start=f_start;
			f3dPoint end=f_end;
			coord_trans(start,prj_ucs,FALSE);
			coord_trans(end,prj_ucs,FALSE);
			start.y=start.z=end.y=end.z=0;
			if(bTransPlate&&nBaseWeldEdgeCount>0)
			{
				int nRetCode=base_face_line.PtInLine(start);
				if(nRetCode==-2)	
					base_face_line.startPt=start;
				else if(nRetCode==-1)
					base_face_line.endPt=end;
				nRetCode=base_face_line.PtInLine(end);
				if(nRetCode==-2)
					base_face_line.startPt=end;
				else if(nRetCode==-1)
					base_face_line.endPt=end;
			}
			else
			{
				base_face_line.startPt=start;
				base_face_line.endPt = end;
			}
			nBaseWeldEdgeCount++;
		}
		for(int j=0;j<pSubPlate->GetFaceN()-1;j++)
		{
			if(bPrjQuFace[i]&&(start_max_face==2+j&&end_max_face==2+j))
			{	//第二/三面上的板顶点
				f3dPoint start=f_start;
				f3dPoint end=f_end;
				coord_trans(start,prj_qu_ucs[j],FALSE);
				coord_trans(end,prj_qu_ucs[j],FALSE);
				start.y=start.z=end.y=end.z=0;
				qu_face_line_arr[j].startPt=start;
				qu_face_line_arr[j].endPt=end;
			}
		}
	}
	if(base_face_line.startPt.IsZero()&&base_face_line.endPt.IsZero())
	{
		logerr.Log("%s钢板没有焊缝线，或焊缝线不在0x%X基准钢板上",(char*)pSubPlate->GetPartNo(),hDatumPlate);
		return FALSE;
	}
	if(pSubPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		if(bPrjQuFace[0]&&qu_face_line_arr[0].startPt.IsZero()&&qu_face_line_arr[0].endPt.IsZero())
		{
			logerr.Log("%s双面钢板获取火曲面焊缝线失败！",(char*)pSubPlate->GetPartNo());
			return FALSE;
		}
	}
	int tagBS,tagBE,max_face_S,max_face_E;
	tagBS=base_face_line.startPt.feature;
	tagBE=base_face_line.endPt.feature;
	max_face_S=max(tagBS/10,tagBS%10);
	max_face_E=max(tagBE/10,tagBE%10);
	short penStyle=PS_OTHERLINE;
	f3dPoint center_pos=geom_center/(n+nArcPointCount);
	f3dPoint extended_vec=center_pos-draw_ucs.origin;
	normalize(extended_vec);
	if(extended_vec*draw_ucs.axis_z<0 || gxDrwLocalizer.ciCustomerSerial==6)
	{	//钢板在焊接底板背面,或广州增立统一用虚线绘制焊接子板轮廓边
		penStyle=PS_DASH;
		m_bFrontWeld=FALSE;
	}
	else
	{
		penStyle=PS_OTHERLINE;
		m_bFrontWeld=TRUE;
	}
	//绘制钢板的轮廓边
	f3dLine line,base_offset_line;	
	line.ID=0;
	line.pen.style=penStyle;
	int plate_thick=ftoi(pSubPlate->GetThick());
	coord_trans(base_face_line.startPt,prj_ucs,TRUE);
	coord_trans(base_face_line.endPt,prj_ucs,TRUE);
	coord_trans(base_face_line.startPt,draw_ucs,FALSE,TRUE);
	coord_trans(base_face_line.endPt,draw_ucs,FALSE,TRUE);
	base_face_line.startPt.z=base_face_line.endPt.z=0;
	f3dPoint base_vec(prj_ucs.axis_y),pick_pt;
	normalize(base_vec);
	vector_trans(base_vec,draw_ucs,FALSE,TRUE);
	normalize(base_vec);
	base_offset_line.startPt=base_face_line.startPt+base_vec*plate_thick;
	base_offset_line.endPt=base_face_line.endPt+base_vec*plate_thick;
	LOCALE_PROPITEM* pLoaleItem=gxDrwLocalizer.GetLocaleItem("WeldSubPartDrawWeldingSymbol");
	bool blDrawWeldSymbol=pLoaleItem!=NULL?pLoaleItem->bVal:false;
	for(int i=0;i<2;i++)
	{
		if(!bPrjQuFace[i])
			break;
		UCS_STRU qu_ucs=prj_qu_ucs[i];
		f3dLine qu_face_line=qu_face_line_arr[i];
		f3dPoint offset_vec=prj_qu_ucs[i].axis_y;
		int tagQS=qu_face_line.startPt.feature;
		int tagQE=qu_face_line.endPt.feature;
		coord_trans(qu_face_line.startPt,qu_ucs,TRUE);
		coord_trans(qu_face_line.endPt,qu_ucs,TRUE);
		coord_trans(qu_face_line.startPt,draw_ucs,FALSE,TRUE);
		coord_trans(qu_face_line.endPt,draw_ucs,FALSE,TRUE);
		qu_face_line.startPt.z=qu_face_line.endPt.z=0;
		vector_trans(offset_vec,draw_ucs,FALSE,TRUE);
		normalize(offset_vec);
		f3dLine qu_offset_line;
		qu_offset_line.startPt=qu_face_line.startPt+offset_vec*plate_thick;
		qu_offset_line.endPt=qu_face_line.endPt+offset_vec*plate_thick;
		f3dPoint vertex1,vertex2;
		int ret1=Int3dll(base_face_line,qu_face_line,vertex1);
		int ret2=Int3dll(base_offset_line,qu_offset_line,vertex2);
		if(tagQS>10)
		{
			qu_face_line.startPt=vertex1;
			qu_offset_line.startPt=vertex2;
		}
		else if(tagQE>10)
		{
			qu_face_line.endPt=vertex1;
			qu_offset_line.endPt=vertex2;
		}
		if(i==0)
		{
			if(max_face_S==2)
			{
				base_face_line.startPt=vertex1;
				base_offset_line.startPt=vertex2;
			}
			if(max_face_E==2)
			{	
				base_face_line.endPt=vertex1;
				base_offset_line.endPt=vertex2;
			}
		}
		else
		{
			if(max_face_S==3)
			{
				base_face_line.startPt=vertex1;
				base_offset_line.startPt=vertex2;
			}
			if(max_face_E==3)
			{	
				base_face_line.endPt=vertex1;
				base_offset_line.endPt=vertex2;
			}
		}
		//绘制火曲面轮廓线
		ClipLine(qu_face_line);
		line.startPt=qu_face_line.startPt;
		line.endPt=qu_face_line.endPt;
		m_xEntContext.AppendLine(line);
		//if(gxDrwLocalizer.ciCustomerSerial!=6)
		//if( gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_DongDian||
		//	gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_ZhanYe||
		//	gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_JianYuan)
		if(blDrawWeldSymbol)
		{	//根据广州增立要求，绘制焊接子板时不绘制焊缝标识线
			pick_pt=line.startPt-offset_vec*plate_thick;
			m_xEntContext.DrawWeldLineSymbol(line,pick_pt);
		}
		ClipLine(qu_offset_line);
		line.startPt=qu_offset_line.startPt;
		line.endPt=qu_offset_line.endPt;
		m_xEntContext.AppendLine(line);
		//if(gxDrwLocalizer.ciCustomerSerial!=6)
		//if( gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_DongDian||
		//	gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_ZhanYe||
		//	gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_JianYuan)
		if(blDrawWeldSymbol)
		{	//根据广州增立要求，绘制焊接子板时不绘制焊缝标识线
			pick_pt=line.startPt+offset_vec*plate_thick;
			m_xEntContext.DrawWeldLineSymbol(line,pick_pt);
		}
		if(tagQS<10)
		{	//厚度线
			line.startPt=qu_face_line.startPt;
			line.endPt=qu_offset_line.startPt;
			m_xEntContext.AppendLine(line);
		}
		if(tagQE<10)
		{	//厚度线
			line.startPt=qu_face_line.endPt;
			line.endPt=qu_offset_line.endPt;
			m_xEntContext.AppendLine(line);
		}
	}
	//基准面轮廓线
	if(!bCirFootPlate)
		ClipLine(base_face_line);
	line.startPt=base_face_line.startPt;
	line.endPt=base_face_line.endPt;
	m_xEntContext.AppendLine(line);
	//if(gxDrwLocalizer.ciCustomerSerial!=6)
	//if( gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_DongDian||
	//	gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_ZhanYe||
	//	gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_JianYuan)
	if(blDrawWeldSymbol)
	{	//根据广州增立要求，绘制焊接子板时不绘制焊缝标识线
		pick_pt=line.startPt-base_vec*plate_thick;
		m_xEntContext.DrawWeldLineSymbol(line,pick_pt);
	}
	if(!bCirFootPlate)
		ClipLine(base_offset_line);
	line.startPt=base_offset_line.startPt;
	line.endPt=base_offset_line.endPt;
	m_xEntContext.AppendLine(line);
	//if(gxDrwLocalizer.ciCustomerSerial!=6)
	//if( gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_DongDian||
	//	gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_ZhanYe||
	//	gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_JianYuan)
	if(blDrawWeldSymbol)
	{	//根据广州增立要求，绘制焊接子板时不绘制焊缝标识线
		pick_pt=line.startPt+base_vec*plate_thick;
		m_xEntContext.DrawWeldLineSymbol(line,pick_pt);
	}
	//延伸至主板焊接边
	BOOL bDrawStartThickLine=TRUE,bDrawEndThickLine=TRUE;
	if(!main_datum_line.startPt.IsZero()||!main_datum_line.endPt.IsZero())
	{	
		f3dPoint base_inters_pt,offset_inters_pt;
		int nRetCode1=Int3dll(main_datum_line,base_face_line,base_inters_pt);
		int nRetCode2=Int3dll(main_datum_line,base_offset_line,offset_inters_pt);
		f3dLine extend_line;
		extend_line.pen.style=PS_DASH;
		if(nRetCode1==-2&&nRetCode2==-2)
		{
			extend_line.startPt=base_inters_pt;
			if(DISTANCE(base_inters_pt,base_face_line.startPt)>DISTANCE(base_inters_pt,base_face_line.endPt))
			{
				extend_line.endPt=base_face_line.endPt;
				bDrawEndThickLine=FALSE;
			}
			else
			{
				extend_line.endPt=base_face_line.startPt;
				bDrawStartThickLine=FALSE;
			}
			m_xEntContext.AppendLine(extend_line);
			//
			extend_line.startPt=offset_inters_pt;
			if(DISTANCE(base_inters_pt,base_offset_line.startPt)>DISTANCE(base_inters_pt,base_offset_line.endPt))
				extend_line.endPt=base_offset_line.endPt;
			else
				extend_line.endPt=base_offset_line.startPt;
			m_xEntContext.AppendLine(extend_line);
		}
	}
	//绘制厚度线
	if(bDrawStartThickLine&&(tagBS<10||(max_face_S==2&&!bPrjQuFace[0])||(max_face_S==3&&!bPrjQuFace[1])))
	{	//绘制厚度线
		line.startPt=base_face_line.startPt;
		line.endPt=base_offset_line.startPt;
		m_xEntContext.AppendLine(line);
	}
	if(bDrawEndThickLine&&(tagBE<10||(max_face_E==2&&!bPrjQuFace[0])||(max_face_E==3&&!bPrjQuFace[1])))
	{	//绘制厚度线
		line.startPt=base_face_line.endPt;
		line.endPt=base_offset_line.endPt;
		m_xEntContext.AppendLine(line);
	}
	if(gxDrwLocalizer.ciCustomerSerial==6)
	{	//添加螺栓到焊接子板的间距标注
		CLDSGeneralPlate* pPlank=(CLDSGeneralPlate*)console.FromPartHandle(hDatumPlate,CLS_PLATE,CLS_PARAMPLATE);
		f3dPoint ptS,ptE,ls_pt,perp_pt;
		double fMinDist=1000,fDist=0;
		for(CLsRef* pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
		{
			pPlank->SpreadLsPos(pLsRef->pBolt,ls_pt);
			coord_trans(ls_pt,pPlank->ucs,TRUE,TRUE);
			coord_trans(ls_pt,draw_ucs,FALSE,TRUE);
			ls_pt.z=0;
			if(SnapPerp(&perp_pt,base_face_line,ls_pt,&fDist)&&base_face_line.PtInLine(perp_pt)==2)
			{
				if(fMinDist<=fabs(fDist))
					continue;
				fMinDist=fabs(fDist);
				ptS=ls_pt;
				ptE=perp_pt;
			}
		}
		if(!ptS.IsZero()||!ptE.IsZero())
		{
			if(Int3dll(base_offset_line,f3dLine(ptS,ptE),perp_pt)==1)
				ptE=perp_pt;
			m_xRotateDim.startPos=ptS;
			m_xRotateDim.endPos=ptE;
			m_xRotateDim.dimPos=0.5*(ptS+ptE);
			m_xRotateDim.dist =DISTANCE(ptS,ptE);
			m_xRotateDim.angle=Cal2dLineAng(0,0,ptE.x-ptS.x,ptE.y-ptS.y);
			m_xRotateDim.bDisplayDimLine=TRUE;
		}
	}
	if(gxDrwLocalizer.ciCustomerSerial!=6)
	{	//按照广州增立要求,焊接子板上不需要添加数据标注
		f3dPoint dim_pos=base_face_line.startPt;
		f3dPoint dim_vec=base_face_line.endPt-base_face_line.startPt;
		double fLen=dim_vec.mod();
		normalize(dim_vec);
		double rotAngle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
		bool bTextHeightVecAlongAxisZ=false;	//位置高度方向与立板法线方向一致
		if(rotAngle>Pi/2&&rotAngle<3*Pi/2)
		{
			dim_vec*=-1;
			dim_pos=base_face_line.endPt;
			bTextHeightVecAlongAxisZ=true;
		}
		dim_pos=dim_pos+(base_vec*plate_thick*0.5+dim_vec*fLen*0.5);	//标注基准位置（钢板中心线中点）
		if(base_vec*f3dPoint(0,1,0)<EPS)
			base_vec*=-1.0;

		m_fWeldLen=fLen;
		m_fWeldThick=plate_thick;
		m_fTxtOff=plate_thick+sys.general_set.fWeldLineSymbolHigh;
		m_xTxtDim.dimVec=dim_vec;
		m_xTxtDim.dimPos=dim_pos;
		double txt_size=sys.fonts.plate.fPartNoTextSize;
		if(m_bFrontWeld)
		{
			if(bTextHeightVecAlongAxisZ)
				m_xTxtDim.dimPos=dim_pos+base_vec*plate_thick;
			else
				m_xTxtDim.dimPos=dim_pos-base_vec*(plate_thick+txt_size);
			m_xTxtDim.dimText.Printf("正焊%s",(char*)pSubPlate->GetPartNo());
			if(gxDrwLocalizer.ciCustomerSerial==7)
				m_xTxtDim.dimText.Replace("正","");
		}
		else
		{
			if(bTextHeightVecAlongAxisZ)
				m_xTxtDim.dimPos=dim_pos-base_vec*(plate_thick+txt_size);
			else
				m_xTxtDim.dimPos=dim_pos+base_vec*plate_thick;
			m_xTxtDim.dimText.Printf("反焊%s",(char*)pSubPlate->GetPartNo());
			if(gxDrwLocalizer.ciCustomerSerial==7)
				m_xTxtDim.dimText.Replace("反","");
		}
	}
	return TRUE;
}
//竖直绘制参数化板轮廓边
/*BOOL CWeldPlateDrawing::CreateParaPlateSideEdge(CLDSParamPlate* pSubParamPlate,long hDatumPlate,UCS_STRU& draw_ucs)
{
	if(pSubParamPlate==NULL)
		return FALSE;
	if(fabs(draw_ucs.axis_z*pSubParamPlate->ucs.axis_z)>0.707)
		return FALSE;
	m_pPlate=pSubParamPlate;
	CLDSGeneralPlate* pDatumPlate=(CLDSGeneralPlate*)console.FromPartHandle(hDatumPlate,CLS_PLATE,CLS_PARAMPLATE);
	//计算投影坐标系
	BOOL bPrjQuFace=FALSE;
	UCS_STRU prj_ucs,prj_qu_ucs;
	f3dPoint prjAxisZ=pSubParamPlate->ucs.axis_z;
	f3dPoint line_vec=prjAxisZ^draw_ucs.axis_z;
	f3dPoint extend_vec=prjAxisZ^line_vec;
	f3dPoint origin=pSubParamPlate->ucs.origin+pSubParamPlate->m_fNormOffset*pSubParamPlate->ucs.axis_z;
	Int3dlf(prj_ucs.origin,origin,extend_vec,draw_ucs.origin,draw_ucs.axis_z);	//使投影原点位于两面交线上
	prj_ucs.axis_y=pSubParamPlate->ucs.axis_z;
	prj_ucs.axis_z=draw_ucs.axis_z;
	prj_ucs.axis_x=prj_ucs.axis_y^prj_ucs.axis_z;
	prj_ucs.axis_y=prj_ucs.axis_z^prj_ucs.axis_x;
	normalize(prj_ucs.axis_x);
	normalize(prj_ucs.axis_y);
	if(pSubParamPlate->IsZhiWan())
	{
		f3dPoint huoqu_norm=pSubParamPlate->HuoquFaceNorm();
		f3dPoint vec=pSubParamPlate->ucs.axis_z^huoqu_norm;
		normalize(vec);
		if(fabs(vec*draw_ucs.axis_z)>0.9)
		{
			bPrjQuFace=TRUE;
			//查找火曲线
			f3dLine huoquline;
			for(PROFILE_VERTEX* pVertex=pSubParamPlate->vertex_list.GetFirst();pVertex;pVertex=pSubParamPlate->vertex_list.GetNext())
			{
				if(pVertex->vertex.feature<=10)
					continue;
				if(huoquline.startPt.IsZero())
					huoquline.startPt=pVertex->vertex;
				else if(huoquline.endPt.IsZero())
					huoquline.endPt=pVertex->vertex;
			}
			if(huoquline.startPt.IsZero()&& huoquline.endPt.IsZero())
			{
				logerr.Log("%s板的火曲线获取失败!",(char*)pSubParamPlate->GetPartNo());
				return FALSE;
			}
			//建立火曲投影坐标系
			coord_trans(huoquline.startPt,pSubParamPlate->ucs,TRUE);
			coord_trans(huoquline.endPt,pSubParamPlate->ucs,TRUE);
			Int3dlf(origin,huoquline,draw_ucs.origin,draw_ucs.axis_z);
			prj_qu_ucs.origin=origin;
			prj_qu_ucs.axis_y=huoqu_norm;
			prj_qu_ucs.axis_z=draw_ucs.axis_z;
			prj_qu_ucs.axis_x=prj_qu_ucs.axis_y^prj_qu_ucs.axis_z;
			prj_qu_ucs.axis_y=prj_qu_ucs.axis_z^prj_qu_ucs.axis_x;
			normalize(prj_qu_ucs.axis_x);
			normalize(prj_qu_ucs.axis_y);
		}
	}
	//查找焊缝线
	f3dLine base_face_line,offset_line,qu_face_line,qu_offset_line;
	f3dPoint geom_center;
	int n = pSubParamPlate->vertex_list.GetNodeNum();
	for(int i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=pSubParamPlate->vertex_list.GetByIndex(i);
		PROFILE_VERTEX *pEndVertex=pSubParamPlate->vertex_list.GetByIndex((i+1)%n);
		f3dPoint f_start = pStartVertex->vertex;
		f3dPoint f_end   = pEndVertex->vertex;
		coord_trans(f_start,pSubParamPlate->ucs,TRUE);
		coord_trans(f_end,pSubParamPlate->ucs,TRUE);
		f3dPoint lineStdVec=f_end-f_start;
		normalize(lineStdVec);
		geom_center+=f_start;
		if(!pStartVertex->m_bWeldEdge ||fabs(lineStdVec*draw_ucs.axis_z)>EPS_COS2)
			continue;
		if(!pDatumPlate->IsWeldWith(f_start,f_end))
			continue;
		int start_max_face=max(f_start.feature/10,f_start.feature%10);
		int end_max_face=max(f_end.feature/10,f_end.feature%10);
		if(start_max_face==1||end_max_face==1)
		{	//第一面上的板顶点
			f3dPoint start=f_start;
			f3dPoint end=f_end;
			coord_trans(start,prj_ucs,FALSE,TRUE);
			coord_trans(end,prj_ucs,FALSE,TRUE);
			start.y=start.z=end.y=end.z=0;
			base_face_line.startPt=start;
			base_face_line.endPt = end;
		}
		if(bPrjQuFace&&(start_max_face==2&&end_max_face==2))
		{	//第二面上的板顶点
			f3dPoint start=f_start;
			f3dPoint end=f_end;
			coord_trans(start,prj_qu_ucs,FALSE);
			coord_trans(end,prj_qu_ucs,FALSE);
			start.y=start.z=end.y=end.z=0;
			qu_face_line.startPt=start;
			qu_face_line.endPt=end;
		}
	}
	if(base_face_line.startPt.IsZero()&&base_face_line.endPt.IsZero())
	{
		logerr.Log("%s钢板没有焊缝线，或焊缝线不在0x%X基准钢板上",(char*)pSubParamPlate->GetPartNo(),hDatumPlate);
		return FALSE;
	}
	if(bPrjQuFace&&qu_face_line.startPt.IsZero()&&qu_face_line.endPt.IsZero())
	{
		logerr.Log("%s双面钢板获取火曲面焊缝线失败！",(char*)pSubParamPlate->GetPartNo());
		return FALSE;
	}
	short pen_style=PS_OTHERLINE;
	f3dPoint center_pos=geom_center;
	center_pos/=n;
	f3dPoint extended_vec=center_pos-draw_ucs.origin;
	normalize(extended_vec);
	if(extended_vec*draw_ucs.axis_z<0 || gxDrwLocalizer.ciCustomerSerial==6)
	{	//钢板在焊接底板背面,或广州增立统一用虚线绘制焊接子板轮廓边
		pen_style=PS_DASH;
		m_bFrontWeld=FALSE;
	}
	//绘制钢板轮廓边
	int tagBS=base_face_line.startPt.feature;
	int tagBE=base_face_line.endPt.feature;
	coord_trans(base_face_line.startPt,prj_ucs,TRUE);
	coord_trans(base_face_line.endPt,prj_ucs,TRUE);
	coord_trans(base_face_line.startPt,draw_ucs,FALSE,TRUE);
	coord_trans(base_face_line.endPt,draw_ucs,FALSE,TRUE);
	base_face_line.startPt.z = 0;
	base_face_line.endPt.z   = 0;
	f3dPoint offset_vec(prj_ucs.axis_y);
	vector_trans(offset_vec,draw_ucs,FALSE);
	normalize(offset_vec);
	f3dLine line;
	line.ID=0;
	line.pen.style=pen_style;
	base_face_line.startPt+=offset_vec*pSubParamPlate->GetNormOffset();
	base_face_line.endPt+=offset_vec*pSubParamPlate->GetNormOffset();
	offset_line.startPt=base_face_line.startPt+offset_vec*pSubParamPlate->GetThick();
	offset_line.endPt=base_face_line.endPt+offset_vec*pSubParamPlate->GetThick();
	f3dPoint pick_pt;
	if(bPrjQuFace)
	{
		int tagQS=qu_face_line.startPt.feature;
		int tagQE=qu_face_line.endPt.feature;
		coord_trans(qu_face_line.startPt,prj_qu_ucs,TRUE);
		coord_trans(qu_face_line.endPt,prj_qu_ucs,TRUE);
		coord_trans(qu_face_line.startPt,draw_ucs,FALSE,TRUE);
		coord_trans(qu_face_line.endPt,draw_ucs,FALSE,TRUE);
		qu_face_line.startPt.z=qu_face_line.endPt.z=0;
		offset_vec=prj_qu_ucs.axis_y;
		vector_trans(offset_vec,draw_ucs,FALSE,TRUE);
		normalize(offset_vec);
		qu_face_line.startPt+=offset_vec*pSubParamPlate->GetNormOffset();
		qu_face_line.endPt+=offset_vec*pSubParamPlate->GetNormOffset();
		qu_offset_line.startPt=qu_face_line.startPt+offset_vec*pSubParamPlate->GetThick();
		qu_offset_line.endPt=qu_face_line.endPt+offset_vec*pSubParamPlate->GetThick();
		f3dPoint vertex1,vertex2;
		int ret1=Int3dll(base_face_line,qu_face_line,vertex1);
		int ret2=Int3dll(offset_line,qu_offset_line,vertex2);
		if(tagQS>10)
		{
			qu_face_line.startPt=vertex1;
			qu_offset_line.startPt=vertex2;
		}
		else if(tagQE>10)
		{
			qu_face_line.endPt=vertex1;
			qu_offset_line.endPt=vertex2;
		}
		if(tagBS>10)
		{
			base_face_line.startPt=vertex1;
			offset_line.startPt=vertex2;
		}
		else if(tagBE>10)
		{	
			base_face_line.endPt=vertex1;
			offset_line.endPt=vertex2;
		}
		//绘制火曲面轮廓线
		ClipLine(qu_face_line);
		line.startPt=qu_face_line.startPt;
		line.endPt=qu_face_line.endPt;
		m_xEntContext.AppendLine(line);
		if(gxDrwLocalizer.ciCustomerSerial!=6)
		{	//根据广州增立要求，绘制焊接子板时不绘制焊缝标识线
			pick_pt=line.startPt-offset_vec*pSubParamPlate->GetThick();
			//m_xEntContext.DrawWeldLineSymbol(line,pick_pt);
		}
		ClipLine(qu_offset_line);
		line.startPt=qu_offset_line.startPt;
		line.endPt=qu_offset_line.endPt;
		m_xEntContext.AppendLine(line);
		if(gxDrwLocalizer.ciCustomerSerial!=6)
		{	//根据广州增立要求，绘制焊接子板时不绘制焊缝标识线
			pick_pt=line.startPt+offset_vec*pSubParamPlate->GetThick();
			//m_xEntContext.DrawWeldLineSymbol(line,pick_pt);
		}
		if(tagQS<10)
		{	//厚度线
			line.startPt=qu_face_line.startPt;
			line.endPt=qu_offset_line.startPt;
			m_xEntContext.AppendLine(line);
		}
		if(tagQE<10)
		{	//厚度线
			line.startPt=qu_face_line.endPt;
			line.endPt=qu_offset_line.endPt;
			m_xEntContext.AppendLine(line);
		}
	}
	//生成偏移矢量和等距偏移线
	ClipLine(base_face_line);
	line.startPt=base_face_line.startPt;
	line.endPt=base_face_line.endPt;
	m_xEntContext.AppendLine(line);
	if(gxDrwLocalizer.ciCustomerSerial!=6)
	{	//根据广州增立要求，绘制焊接子板时不绘制焊缝标识线
		pick_pt=line.startPt-offset_vec*pSubParamPlate->GetThick();
		m_xEntContext.DrawWeldLineSymbol(line,pick_pt);
	}
	ClipLine(offset_line);
	line.startPt=offset_line.startPt;
	line.endPt=offset_line.endPt;
	m_xEntContext.AppendLine(line);
	if(gxDrwLocalizer.ciCustomerSerial!=6)
	{	//根据广州增立要求，绘制焊接子板时不绘制焊缝标识线
		pick_pt=line.startPt+offset_vec*pSubParamPlate->GetThick();
		m_xEntContext.DrawWeldLineSymbol(line,pick_pt);
	}
	//绘制厚度线
	if(tagBS<10)
	{
		line.startPt=base_face_line.startPt;
		line.endPt=offset_line.startPt;
		m_xEntContext.AppendLine(line);
	}
	if(tagBE<10)
	{
		line.startPt=base_face_line.endPt;
		line.endPt=offset_line.endPt;
		m_xEntContext.AppendLine(line);
	}
	if(gxDrwLocalizer.ciCustomerSerial==6)
	{	//添加螺栓到焊接子板的间距标注
		CLDSGeneralPlate* pPlank=(CLDSGeneralPlate*)console.FromPartHandle(hDatumPlate,CLS_PLATE,CLS_PARAMPLATE);
		f3dPoint ptS,ptE,ls_pt,perp_pt;
		double fMinDist=1000,fDist=0;
		for(CLsRef* pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
		{
			pPlank->SpreadLsPos(pLsRef->pBolt,ls_pt);
			coord_trans(ls_pt,pPlank->ucs,TRUE,TRUE);
			coord_trans(ls_pt,draw_ucs,FALSE,TRUE);
			ls_pt.z=0;
			if(SnapPerp(&perp_pt,base_face_line,ls_pt,&fDist)&&base_face_line.PtInLine(perp_pt)==2)
			{
				if(fMinDist<=fabs(fDist))
					continue;
				fMinDist=fabs(fDist);
				ptS=ls_pt;
				ptE=perp_pt;
			}
		}
		if(!ptS.IsZero()||!ptE.IsZero())
		{
			if(Int3dll(offset_line,f3dLine(ptS,ptE),perp_pt)==1)
				ptE=perp_pt;
			m_xRotateDim.startPos=ptS;
			m_xRotateDim.endPos=ptE;
			m_xRotateDim.dimPos=0.5*(ptS+ptE);
			m_xRotateDim.dist =DISTANCE(ptS,ptE);
			m_xRotateDim.angle=Cal2dLineAng(0,0,ptE.x-ptS.x,ptE.y-ptS.y);
			m_xRotateDim.bDisplayDimLine=TRUE;
		}
	}
	if(gxDrwLocalizer.ciCustomerSerial!=6)
	{	//按照广州增立要求,焊接子板上不需要添加数据标注
		f3dPoint dim_pos=base_face_line.startPt;
		f3dPoint dim_vec=base_face_line.endPt-base_face_line.startPt;
		double fLen=dim_vec.mod(),thick=pSubParamPlate->GetThick();
		normalize(dim_vec);
		double rotAngle=Cal2dLineAng(0,0,dim_vec.x,dim_vec.y);
		if(rotAngle>Pi/2&&rotAngle<3*Pi/2)
		{
			dim_vec*=-1;
			dim_pos=base_face_line.endPt;
		}
		m_fWeldLen=fLen;
		m_fWeldThick=thick;
		m_fTxtOff=thick+sys.general_set.fWeldLineSymbolHigh;
		m_xTxtDim.dimVec=dim_vec;
		m_xTxtDim.dimPos=dim_pos+offset_vec*thick*0.5+dim_vec*fLen*0.5;
		if(m_bFrontWeld)
			m_xTxtDim.dimText.Printf("正焊%s",(char*)pSubParamPlate->GetPartNo());
		else
			m_xTxtDim.dimText.Printf("反焊%s",(char*)pSubParamPlate->GetPartNo());
	}
	return TRUE;
}*/
void CWeldPlateDrawing::ClipLine(f3dLine &line)
{
	int iAmendModel=-1;
	if(!m_pBelongDrawing->PtInPlateRgn(line.startPt)&&m_pBelongDrawing->PtInPlateRgn(line.endPt))
		iAmendModel=0;	//始端在外
	else if(m_pBelongDrawing->PtInPlateRgn(line.startPt)&&!m_pBelongDrawing->PtInPlateRgn(line.endPt))
		iAmendModel=1;	//终端在外
	else
		return;
	int nNum=m_pBelongDrawing->GetPlateRgnPtNum();
	GEPOINT startPt,endPt;
	for(int i=0;i<nNum;i++)
	{
		startPt=m_pBelongDrawing->GetPlateRgnPt(i);
		endPt=m_pBelongDrawing->GetPlateRgnPt((i+1)%nNum);
		f3dLine tem_line(startPt,endPt);
		f3dPoint intersPt;
		if(Int3dll(line,tem_line,intersPt)==1)	
		{	//两直线求交
			if(iAmendModel==0)
				line.startPt=intersPt;
			else
				line.endPt=intersPt;
			return;
		}
	}
}
//完善焊接钢板图形特征信息
void CWeldPlateDrawing::PerfectWeldDrawing()
{
	for(IDbEntity* pEnt=EnumFirstDbEntity();pEnt;pEnt=EnumNextDbEntity())
		pEnt->SetHiberId(HIBERID(m_pPlate->handle));
	//
	f3dPoint center=m_xTxtDim.dimPos;
	f3dPoint extend_vec=m_xTxtDim.dimVec;
	f3dPoint offset_vec(-extend_vec.y,extend_vec.x);
	topLeft =center+extend_vec*m_fWeldLen*0.5+offset_vec*m_fWeldThick*0.5;
	topRight=center+extend_vec*m_fWeldLen*0.5-offset_vec*m_fWeldThick*0.5;
	btmLeft =center-extend_vec*m_fWeldLen*0.5+offset_vec*m_fWeldThick*0.5;
	btmRight=center-extend_vec*m_fWeldLen*0.5-offset_vec*m_fWeldThick*0.5;
}
//判断与其他焊接图形是否有重合
BOOL CWeldPlateDrawing::IsOverlap(CWeldPlateDrawing& weld_drawing)
{
	double fMin=0.5*m_fWeldThick;
	double dd1=0,dd2=0,dd3=0,dd4=0;
	double fCos=m_xTxtDim.dimVec*weld_drawing.m_xTxtDim.dimVec;
	if(fabs(fCos)<EPS_COS)	//标注方向不平行
		return FALSE;
	if(fCos>0)
	{	//同向
		dd1=DISTANCE(topLeft,weld_drawing.topLeft);
		dd2=DISTANCE(topRight,weld_drawing.topRight);
		dd3=DISTANCE(btmLeft,weld_drawing.btmLeft);
		dd4=DISTANCE(btmRight,weld_drawing.btmRight);
	}
	else
	{	//反向
		dd1=DISTANCE(topLeft,weld_drawing.btmRight);
		dd2=DISTANCE(topRight,weld_drawing.btmLeft);
		dd3=DISTANCE(btmLeft,weld_drawing.topRight);
		dd4=DISTANCE(btmRight,weld_drawing.topLeft);
	}
	if(dd1>fMin || dd2>fMin || dd3>fMin || dd4>fMin)
		return FALSE;
	return TRUE;
}
void CWeldPlateDrawing::CopyMember(CWeldPlateDrawing& weld_drawing)
{
	m_pPlate=weld_drawing.m_pPlate;
	for(IDbLine* pTemLine=(IDbLine*)weld_drawing.EnumFirstDbEntity();pTemLine;pTemLine=(IDbLine*)weld_drawing.EnumNextDbEntity())
	{
		IDbLine* pLine=(IDbLine*)AppendDbEntity(IDbEntity::DbLine);
		pLine->SetPen(pTemLine->GetPen());
		pLine->SetStartPosition(pTemLine->StartPosition());
		pLine->SetEndPosition(pTemLine->EndPosition());
		pLine->SetHiberId(pTemLine->GetHiberId());
	}
	//
	m_bFrontWeld=weld_drawing.m_bFrontWeld;
	m_fTxtOff=weld_drawing.m_fTxtOff;
	m_fWeldLen=weld_drawing.m_fWeldLen;
	m_fWeldThick=weld_drawing.m_fWeldThick;
	m_xTxtDim.dimPos=weld_drawing.m_xTxtDim.dimPos;
	m_xTxtDim.dimVec=weld_drawing.m_xTxtDim.dimVec;
	m_xTxtDim.dimText=weld_drawing.m_xTxtDim.dimText;
	m_xRotateDim.startPos=weld_drawing.m_xRotateDim.startPos;
	m_xRotateDim.endPos=weld_drawing.m_xRotateDim.endPos;
	m_xRotateDim.dimPos=weld_drawing.m_xRotateDim.dimPos;
	m_xRotateDim.dist =weld_drawing.m_xRotateDim.dist;
	m_xRotateDim.angle=weld_drawing.m_xRotateDim.angle;
	m_xRotateDim.bDisplayDimLine=TRUE;
	topLeft=weld_drawing.topLeft;
	topRight=weld_drawing.topRight;
	btmLeft=weld_drawing.btmLeft;
	btmRight=weld_drawing.btmRight;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD CPlankDrawing::DimBoltSpaceToWeldEdge		= 0x00000100;//:00000000 0000000000000001 00000000
DWORD CPlankDrawing::DimHoleDiameterAlways		= 0x00000200;//:00000000 0000000000000002 00000000
DWORD CPlankDrawing::DimBoltSpaceToHuoQuLine	= 0x00000400;
DWORD CPlankDrawing::DimBoltSpaceToEdgeLine		= 0x00000800;	
DWORD CPlankDrawing::DimBoltSpaceToNextLsRay	= 0x00001000;
DWORD CPlankDrawing::DimBoltAngle				= 0x00002000;
MASKFLAG CPlankDrawing::PREFER_STYLEFLAG;
CPlankDrawing::CPlankDrawing()
{
	m_ciHuoQuType=0;
	long_edge_angle=0;
	m_bSpecifyUCS=FALSE;
	m_pPlate=NULL;
	dimStyleFlag=PREFER_STYLEFLAG;
	m_xEntContext.InitContext(this,STORE_XERO);
}

CPlankDrawing::~CPlankDrawing()
{

}
static f3dPoint  GetDimPosByLine(f3dLine line,double dist)
{
	f3dPoint vec,norm,mid_pnt,result,Z_axis(0,0,1);
	Sub_Pnt(vec,line.endPt,line.startPt);
	Add_Pnt(mid_pnt,line.startPt,line.endPt);
	mid_pnt = mid_pnt*0.5;
	vec.z = 0;
	norm = cross_prod(vec,Z_axis);
	normalize(norm);
	Add_Pnt(result,mid_pnt,norm*dist);
	return result;
}

void CPlankDrawing::ProcessPlateWeldEdge(PROFILE_VERTEX *pFeatureVertex,f3dArcLine &arcline)
{
	if(sys.part_map.plate.bDrawSlopeCutLine)
	{
		LOCALE_PROPITEM* pLoaleItem=gxDrwLocalizer.GetLocaleItem("WeldSubPartDrawWeldingSymbol");
		bool blDrawWeldSymbol=pLoaleItem!=NULL?pLoaleItem->bVal:false;
		if(gxDrwLocalizer.ciCustomerSerial==6)
		{	//按照广州增立要求,绘制焊缝标识线
			if(arcline.SectorAngle()==0)	//直线
				m_xEntContext.DrawWeldLineSymbol(arcline.Start(),arcline.End());
			else
				m_xEntContext.DrawWeldLineSymbol(arcline);
			//标注坡口文本
			DimSlopeCutLineText(arcline);	
		}
		//else if(gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_DongDian||
		//		gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_JianYuan||
		//		gxDrwLocalizer.ciCustomerSerial==CLIENT_YunNan_ZhanYe)
		else if(blDrawWeldSymbol)
		{
			if(arcline.SectorAngle()==0)	//直线
				m_xEntContext.DrawWeldLineSymbol(arcline.Start(),arcline.End());
			else
				m_xEntContext.DrawWeldLineSymbol(arcline);
		}
		else if(gxDrwLocalizer.ciCustomerSerial==7)
		{	//标注焊接件号
			int nBoltCount=0;
			CLsRefList *pLsRefList=m_pPlate->GetLsRefList();
			if(pLsRefList!=NULL)
			{
				for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
				{
					if(!((*pLsRef)->m_bVirtualPart))
						nBoltCount++;
				}
			}
			CLDSPart *pWeldMotherPart=pFeatureVertex?Ta.Parts.FromHandle(pFeatureVertex->weldline.hWeldMotherPart):NULL;
			if(pFeatureVertex&&pFeatureVertex->type==0&&pWeldMotherPart&&!pWeldMotherPart->IsTube()&&nBoltCount>0)
			{	//标注肋板焊接边关联的构件 wht 18-08-10
				TXTDIM_STRU txt_dim;
				txt_dim.dimPos=(arcline.Start()+arcline.End())*0.5;
				txt_dim.dimVec=(arcline.End()-arcline.Start());
				normalize(txt_dim.dimVec);
				txt_dim.dimText.Printf("焊%s",pWeldMotherPart->Label);
				double fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
				txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
				text_dim_arr.Add(txt_dim);
			}
			else	//标注坡口尺寸
				DimSlopeCutLineText(arcline);
		}
		else
			DrawSlopeCutLine(arcline);	//绘制与焊接边平行的坡口线
	}
	
}
//绘制与焊接边平行的线表示坡口线
void CPlankDrawing::DrawSlopeCutLine(f3dArcLine arcline)
{
	double offset_thick = m_pPlate?m_pPlate->GetThick():6;
	if(arcline.SectorAngle()==0)	//直线
	{	//等距偏移直线边
		f3dLine chord(arcline.Start(),arcline.End());
		f3dPoint line_vec=chord.endPt-chord.startPt;
		normalize(line_vec);
		f3dPoint perp_vec(-line_vec.y,line_vec.x);
		chord.startPt+=perp_vec*offset_thick;
		chord.endPt+=perp_vec*offset_thick;
		arcline=LineToArc(chord);
	}
	else
	{
		if(arcline.WorkNorm().z>0)	//凸圆弧边
			arcline.SetRadius(arcline.Radius()-offset_thick);
		else
			arcline.SetRadius(arcline.Radius()+offset_thick);
	}
	arcline.pen.style=PS_DASH;
	edge_arr.Add(arcline);
}
static CXhChar100 GetSlopeCutLineDimText(double plate_thick)
{
	CXhChar100 sDimText;
	if(gxDrwLocalizer.ciCustomerSerial==6)
	{	//广州增立
		//板    厚   6mm     8-12mm    14mm及以上
		//坡口样式   无       单面坡    双面坡
		if(plate_thick>=8 && plate_thick<=12)
			sDimText.Copy("单坡口电焊");
		else if(plate_thick>=14)
			sDimText.Copy("双坡口电焊");
	}
	else if(gxDrwLocalizer.ciCustomerSerial==7)
	{	//安徽宏源
		//板    厚   6mm     8-10mm    12mm及以上
		//坡口样式   无       单面坡    双面坡
		//坡口尺寸   无       板厚-2mm   (板厚-2mm)/2
		if(plate_thick>=8&&plate_thick<=10)
			sDimText.Printf("单面坡口%.fx%.f",plate_thick-2,plate_thick-2);
		else if(plate_thick>=12)
			sDimText.Printf("双面坡口%.fx%.f",(plate_thick-2)*0.5,(plate_thick-2)*0.5);
	}
	else if(gxDrwLocalizer.ciCustomerSerial==9)
	{	//重庆江电
		//板    厚   6-8mm     10-12mm    12mm以上
		//坡口样式   无       单面坡    双面坡
		//坡口尺寸   无       板厚-2mm   (板厚-2mm)/2
		if(plate_thick>=10&&plate_thick<=12)
			sDimText.Printf("单面坡口%.fx%.f",plate_thick-2,plate_thick-2);
		else if(plate_thick>12)
			sDimText.Printf("双面坡口%.fx%.f",(plate_thick-2)*0.5,(plate_thick-2)*0.5);
	}
	return sDimText;
}
//在坡口线上添加文字标注
void CPlankDrawing::DimSlopeCutLineText(f3dArcLine arcline)
{	
	double plate_thick=m_pPlate->GetThick();
	double fTxtOff=sys.fonts.plate.fPartNoTextSize+sys.general_set.fWeldLineSymbolHigh;
	f3dPoint ptS=arcline.Start(),ptE=arcline.End();
	f3dPoint line_vec=(ptE-ptS).normalized();
	f3dPoint dest_vec(-line_vec.y,line_vec.x);
	f3dPoint dim_pos=(ptS+ptE)*0.5;
	if(arcline.SectorAngle()>0)
		dim_pos=arcline.PositionInAngle(arcline.SectorAngle()*0.5);
	//
	TXTDIM_STRU xTxtDim;
	xTxtDim.dimVec=line_vec;
	xTxtDim.dimPos=dim_pos+dest_vec*fTxtOff;
	xTxtDim.dimText=GetSlopeCutLineDimText(plate_thick);
	if(xTxtDim.dimText.GetLength()>1)
	{
		xTxtDim.horzAlignMode=AcDb::kTextCenter;
		text_dim_arr.Add(xTxtDim);
	}
}
//绘制内侧平行轮廓边
void CPlankDrawing::DrawInnerEdge(CLDSGeneralPlate* pPart,ATOM_LIST<PROFILE_VERTEX> *pVertexList)
{
	CLogErrorLife logErrLife;
	//初始化内侧轮廓点
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||sys.part_map.plate.bDimCoord||m_bSpecifyUCS)
		bAdjustPlateUcs=TRUE;
	ATOM_LIST<PROFILE_VERTEX> xInnerVerterxList;
	PROFILE_VERTEX* pVertex=NULL;
	double fOffDist=sys.part_map.plate.fInnerEdgeOffset;
	int nNum=pVertexList->GetNodeNum();
	for(int i=0;i<nNum;i++)
	{
		PROFILE_VERTEX* pPreVertex=pVertexList->GetByIndex((nNum+i-1)%nNum);
		PROFILE_VERTEX* pCurVertex=pVertexList->GetByIndex(i%nNum);
		PROFILE_VERTEX* pNextVertex=pVertexList->GetByIndex((i+1)%nNum);
		f3dArcLine preArcline,nextArcLine;
		if(bAdjustPlateUcs)
		{
			preArcline=CreateIsometricEdgeLine(pPreVertex,pCurVertex,pPart,&new_ucs,fOffDist);
			nextArcLine=CreateIsometricEdgeLine(pCurVertex,pNextVertex,pPart,&new_ucs,fOffDist);
		}
		else
		{
   			preArcline=CreateIsometricEdgeLine(pPreVertex,pCurVertex,pPart,NULL,fOffDist);
			nextArcLine=CreateIsometricEdgeLine(pCurVertex,pNextVertex,pPart,NULL,fOffDist);
		}
		//
		f3dPoint vertex;
		if(fabs(preArcline.SectorAngle())<EPS2 && fabs(nextArcLine.SectorAngle())<EPS2)//直线
		{	//两条直线
			int iResult=Int3dll(preArcline.Start(),preArcline.End(),nextArcLine.Start(),nextArcLine.End(),vertex);
			if(iResult==0)
			{	//平行，取端点
				pVertex=xInnerVerterxList.append();
				pVertex->type=0;
				pVertex->vertex=nextArcLine.Start();
			}
			else
			{	//有交点
				pVertex=xInnerVerterxList.append();
				pVertex->type=0;
				pVertex->vertex=vertex;
			}
		}
		else
		{	//有弧线
			pVertex=xInnerVerterxList.append();
			pVertex->type=pCurVertex->type;
			pVertex->vertex=nextArcLine.Start();
			if(nextArcLine.SectorAngle()>EPS2)
			{
				pVertex->center=nextArcLine.Center();
				pVertex->radius=nextArcLine.Radius();
				if(nextArcLine.WorkNorm().z<0)	//凹圆弧边
					pVertex->radius*=-1;
				pVertex->sector_angle=nextArcLine.SectorAngle();
				pVertex->column_norm=nextArcLine.ColumnNorm();
			}
		}
	}
	//绘制轮廓边
	if(nNum!=xInnerVerterxList.GetNodeNum())
	{
		logerr.Log("钢板内侧轮廓点提取失败!");
		return;
	}
	f3dArcLine arcline;
	arcline.pen.style=PS_DAMAGELINE;
	nNum=xInnerVerterxList.GetNodeNum();
	for(int i=0;i<nNum;i++)
	{
		PROFILE_VERTEX startVer=xInnerVerterxList[i];
		PROFILE_VERTEX endVer=xInnerVerterxList[(i+1)%nNum];
		if(startVer.type==0)
			arcline.CreateMethod2(startVer.vertex,endVer.vertex,f3dPoint(0,0,1),0);
		else if(startVer.type==1)
			arcline.CreateMethod2(startVer.vertex,endVer.vertex,f3dPoint(0,0,1),startVer.sector_angle);
		else if(startVer.type==2)
			arcline.CreateMethod3(startVer.vertex,endVer.vertex,f3dPoint(0,0,1),startVer.radius,startVer.center);
		else if(startVer.type==3)
			arcline.CreateEllipse(startVer.center,startVer.vertex,endVer.vertex,startVer.column_norm,f3dPoint(0,0,1),startVer.radius);
		edge_arr.Add(arcline);
	}
}
//绘制焊接子构件轮廓边
void CPlankDrawing::DrawWeldSubPlate(CLDSGeneralPlate* pDatumPlate,BOOL bAdjustPlateUcs)
{
	if(!ValidateLocalizeFeature(FEATURE::DRAWING_WELD_BRIEFSTYLE)&&AgentDogType()!=3)
		return;
	//1.提取钢板的焊接子钢板集合
	PARTSET subPlateSet;
	long hBaseNode=pDatumPlate->m_hBaseNode;
	
	PARTSET partSet;
	for(CLDSPart *pPart=console.EnumPartFirst();pPart;pPart=console.EnumPartNext())
	{
		if(!pPart->IsPlate() || pPart->m_bVirtualPart ||!pPart->ModuleTogetherWith(pDatumPlate))
			continue;
		if(pPart->iSeg!=pDatumPlate->iSeg)
			continue;
		if(pPart->IsBlockRefPart())
			continue;
		CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
		if(hBaseNode!=pPlate->m_hBaseNode)
			continue;	//按基准节点过滤，所有钢板都判断影响速度 wht 18-08-15
		partSet.append(pPart);
	}
	for(CLDSPart* pPart=partSet.GetFirst();pPart;pPart=partSet.GetNext())
	{
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
		if(pPlate->GetClassTypeId()==CLS_PLATE)
		{	//普通钢板通过焊缝边获取焊缝父构件
			int nPush=partSet.push_stack();
			pPlate->IntelliRecogWeldLine(&partSet);
			partSet.pop_stack(nPush);

			f3dLine line;
			f3dArcLine arcline;
			bool isWeldToDatumPlate=false;
			PROFILE_VERTEX* pVertex=NULL,*pPrevVertex=pPlate->vertex_list.GetTail();
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pPrevVertex=pVertex,pVertex=pPlate->vertex_list.GetNext())
			{
				if(!pPrevVertex->m_bWeldEdge)
					continue;
				if(pPrevVertex->weldline.hWeldMotherPart==pDatumPlate->handle && !isWeldToDatumPlate)
				{	//虽已确认焊接关系，但为了绘制火曲板存在两条焊接边情况，还需要继续检测 wjh-2016.10.20
					subPlateSet.append(pPlate);
					isWeldToDatumPlate=true;
				}
				else if(pPrevVertex->weldline.hWeldMotherPart==0)
				{	//重新识别此边是否与当前钢板pDatumPlate有焊接关系
					line.startPt=pPlate->GetRealVertex(pPrevVertex->vertex);
					line.endPt=pPlate->GetRealVertex(pVertex->vertex);
					if(pPrevVertex->type>0)
					{	//出于计算效率考虑，暂不处理肘板上的弧形焊缝　wjh-2016.8.7
						pPrevVertex->RetrieveArcLine(arcline,pVertex->vertex,&pPlate->ucs);
						arcline.CoordTrans(pPlate->ucs,true);
					}
					else
					{
						coord_trans(line.startPt,pPlate->ucs,TRUE,TRUE);
						coord_trans(line.endPt,pPlate->ucs,TRUE,TRUE);
					}
					if((pPrevVertex->type==0&&pDatumPlate->IsWeldWith(line.startPt,line.endPt))||
						(pPrevVertex->type >0&&pDatumPlate->IsWeldWith(arcline)))
						pPrevVertex->weldline.hWeldMotherPart=pDatumPlate->handle;
					if(!isWeldToDatumPlate&&pPrevVertex->weldline.hWeldMotherPart==pDatumPlate->handle)
					{
						subPlateSet.append(pPlate);
						isWeldToDatumPlate=true;
					}
				}
			}
		}
		else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{	//参数化板通过设计参数获取焊接父构件
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*)pPart;
			long hDatumPart=0,hAssPart1=0;
			if(pParamPlate->m_iParamType==TYPE_RIBPLATE)
			{
				pParamPlate->GetDesignItemValue('A',&hDatumPart);//基准构件
				pParamPlate->GetDesignItemValue('C',&hAssPart1); //辅助构件1
				
			}
			else if(pParamPlate->m_iParamType==TYPE_SHOEPLATE)
			{
				pParamPlate->GetDesignItemValue('C',&hDatumPart);//基准构件
				pParamPlate->GetDesignItemValue('G',&hAssPart1); //辅助构件1
			}
			else if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
			{
				pParamPlate->GetDesignItemValue('L',&hDatumPart);//基准构件
			}
			else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
			{
				CLDSGeneralPlate::DESIGN_PARAM itemparam;
				pParamPlate->GetDesignItemValue('E',&itemparam);
				if(itemparam.cValueType==3)	//句柄+数值
					hDatumPart=itemparam.value.hPart;
				else
					hDatumPart=itemparam.ToLong();
				//
				pParamPlate->GetDesignItemValue('F',&itemparam);
				if(itemparam.cValueType==3)	//句柄+数值
					hAssPart1=itemparam.value.hPart;
				else 
					hAssPart1=itemparam.ToLong();
			}
			if(abs(hDatumPart)==pDatumPlate->handle || abs(hAssPart1)==pDatumPlate->handle)
			{
				pPlate->IntelliRecogWeldLine(NULL);
				subPlateSet.append(pPlate);
			}
		}
	}
	if(subPlateSet.GetNodeNum()<=0)
		return;
	//2.在基准钢板绘图坐标系下，绘制子钢板轮廓边
	UCS_STRU draw_ucs=pDatumPlate->ucs;
	if(bAdjustPlateUcs)
		draw_ucs=new_ucs;
	ATOM_LIST<CWeldPlateDrawing> weldDrawingList;
	CWeldPlateDrawing* pWeldDrawing=NULL;
	for(CLDSPart* pPart=subPlateSet.GetFirst();pPart;pPart=subPlateSet.GetNext())
	{
		BOOL bRet=FALSE;
		CWeldPlateDrawing weld_drawing(this);
		if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
			bRet=weld_drawing.CreatePlateSideEdge((CLDSPlate*)pPart,pDatumPlate->handle,draw_ucs);
		//else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		//	bRet=weld_drawing.CreateParaPlateSideEdge((CLDSParamPlate*)pPart,pDatumPlate->handle,draw_ucs);
		if(bRet==FALSE)
			continue;
		weld_drawing.PerfectWeldDrawing();
		//合并同一位置重叠的焊接绘制图形
		for(pWeldDrawing=weldDrawingList.GetFirst();pWeldDrawing;pWeldDrawing=weldDrawingList.GetNext())
		{
			if(pWeldDrawing->IsOverlap(weld_drawing))
			{
				if(!pWeldDrawing->m_bFrontWeld&&weld_drawing.m_bFrontWeld)
				{
					weld_drawing.m_xTxtDim.dimText.Append(pWeldDrawing->m_xTxtDim.dimText);
					weldDrawingList.DeleteCursor();
					pWeldDrawing=NULL;
				}
				else
					pWeldDrawing->m_xTxtDim.dimText.Append(weld_drawing.m_xTxtDim.dimText);
				break;
			}
		}
		weldDrawingList.Clean();
		if(pWeldDrawing==NULL)
		{
			pWeldDrawing=weldDrawingList.append();
			pWeldDrawing->CopyMember(weld_drawing);
		}
	}
	//将焊接子板图形整合到母板的edge_arr和text_dim_arr中
	double txt_size=sys.dim_map.PartNo.fPartNoTextSize;
	for(pWeldDrawing=weldDrawingList.GetFirst();pWeldDrawing;pWeldDrawing=weldDrawingList.GetNext())
	{
		//整合图形edge_arr
		for(IDbEntity* pEnt=pWeldDrawing->EnumFirstDbEntity();pEnt;pEnt=pWeldDrawing->EnumNextDbEntity())
		{
			if(pEnt->GetDbEntType()!=IDbEntity::DbLine)
				continue;
			IDbLine* pLine=(IDbLine*)pEnt;
			f3dLine line(pLine->StartPosition(),pLine->EndPosition());
			line.pen=pLine->GetPen();
			edge_arr.Add(LineToArc(line));
		}
		//整合尺寸标注 rotate_dim_arr
		if(pWeldDrawing->m_xRotateDim.dist>0)
			rotate_dim_arr.Add(pWeldDrawing->m_xRotateDim);
		//整合标注text_dim_arr
		double fTxtLen=strlen(pWeldDrawing->m_xTxtDim.dimText)*txt_size*0.7;
		f3dPoint dim_vec=pWeldDrawing->m_xTxtDim.dimVec;
		f3dPoint offset_vec(-dim_vec.y,dim_vec.x);
		f3dPoint dim_pos=pWeldDrawing->m_xTxtDim.dimPos-dim_vec*0.5*fTxtLen;
		/*for(int i=0;i<2;i++)
		{
			if(i==0)
				dim_pos+=offset_vec*pWeldDrawing->m_fTxtOff;
			else
				dim_pos-=offset_vec*(pWeldDrawing->m_fTxtOff+txt_size);
			f3dPoint ptArr[4]={dim_pos,dim_pos+dim_vec*fTxtLen,dim_pos+offset_vec*txt_size,
								dim_pos+dim_vec*fTxtLen+offset_vec*txt_size};
			if(PtInPlateRgn(ptArr[0])&&PtInPlateRgn(ptArr[1])&&PtInPlateRgn(ptArr[2])&&PtInPlateRgn(ptArr[3]))
				break;
		}*/
		pWeldDrawing->m_xTxtDim.dimPos=dim_pos;
		text_dim_arr.Add(pWeldDrawing->m_xTxtDim);
	}
}
void CPlankDrawing::MarkHuoquLine(f3dLine line)
{	//标记火曲线位置  样条曲线
	double fDist=m_pPlate->GetThick()*0.5;
	f3dPoint line_vec=line.endPt-line.startPt;
	normalize(line_vec);
	f3dPoint offset_vec(-line_vec.y,line_vec.x,0);
	//始端样条曲线
	fPtList vertex_list;
	f3dPoint pt=line.startPt,vertex;
	for(int i=0;i<4;i++)
	{
		pt+=line_vec*fDist;
		if(i%2==0)
			vertex=pt-offset_vec*fDist;
		else
			vertex=pt+offset_vec*fDist;
		vertex.pen.style=PS_SOLID;
		vertex_list.append(vertex);
	}
	m_xEntContext.AppendSpLine(vertex_list);
	//终端样条曲线
	vertex_list.Empty();
	pt=line.endPt;
	for(int i=0;i<4;i++)
	{
		pt-=line_vec*fDist;
		if(i%2!=0)
			vertex=pt-offset_vec*fDist;
		else
			vertex=pt+offset_vec*fDist;
		vertex.pen.style=PS_SOLID;
		vertex_list.append(vertex);
	}
	m_xEntContext.AppendSpLine(vertex_list);
}
int GetWeldCornerArcRadius(double plateThick)//通过板厚获取圆弧半径
{
	if(plateThick==5 || plateThick ==6)
		return 15;
	else if(plateThick>=8&&plateThick<=10)
		return 20;
	else if(plateThick>=12&&plateThick<=16)
		return 25;
	else if(plateThick>=18&&plateThick<=24)
		return 30;
	else if(plateThick>24)
		return 35;
	else 
		return 0;
}
//<DEVELOP_PROCESS_MARK nameId="CPlankDrawing.CreateLineOnWeldCorner">
void CPlankDrawing::CreateLineOnWeldCorner(CLDSGeneralPlate* pPart,ATOM_LIST<PROFILE_VERTEX> *pVertexList)
{
	int arcRadius=GetWeldCornerArcRadius(pPart->GetThick());
	if(arcRadius==0)
		return;
	f3dArcLine beforArcline,afterArcLine,arcLine;
	f3dPoint startPt,endPt,pos;
	f3dLine line;
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||sys.part_map.plate.bDimCoord||m_bSpecifyUCS)
		bAdjustPlateUcs=TRUE;

	PROFILE_VERTEX *pVertex=NULL;
	int nNum=pVertexList->GetNodeNum();
	for(int i=0;i<nNum;i++)
	{
		PROFILE_VERTEX* pPreVertex=pVertexList->GetByIndex((nNum+i-1)%nNum);
		PROFILE_VERTEX* pCurVertex=pVertexList->GetByIndex(i%nNum);
		PROFILE_VERTEX* pNextVertex=pVertexList->GetByIndex((i+1)%nNum);
		if(pPreVertex->m_bWeldEdge&&pCurVertex->m_bWeldEdge&&sys.part_map.plate.iCutArcsType!=0)
		{
			if(bAdjustPlateUcs)
			{
				beforArcline=CreateIsometricEdgeLine(pPreVertex,pCurVertex,pPart,&new_ucs,0);
				afterArcLine=CreateIsometricEdgeLine(pCurVertex,pNextVertex,pPart,&new_ucs,0);
			}
			else
			{
   				beforArcline=CreateIsometricEdgeLine(pPreVertex,pCurVertex,pPart,NULL,0);
				afterArcLine=CreateIsometricEdgeLine(pCurVertex,pNextVertex,pPart,NULL,0);
			}
			//前一条焊缝边
			GEPOINT lineVect;
			if(fabs(beforArcline.SectorAngle())<EPS2 )//直线
			{
				lineVect= beforArcline.Start() - beforArcline.End();
				normalize(lineVect);
				startPt = beforArcline.End() + lineVect*arcRadius;
			}
			else//圆弧
			{
				/*lineVect=beforArcline.End()-beforArcline.Center();
				normalize(lineVect);
				if(beforArcline.WorkNorm().z<0)
					lineVect.Set(-lineVect.y, lineVect.x);
				else
					lineVect.Set( lineVect.y,-lineVect.x);
				startPt = beforArcline.End() + lineVect*arcRadius;*/
				//
				double cosa=0.5*arcRadius/beforArcline.Radius();
				double angle=ACOS(cosa);
				lineVect=beforArcline.Center()-beforArcline.End();
				normalize(lineVect);
				if(beforArcline.WorkNorm().z<0)
					RotateVectorAroundVector(lineVect,angle,f3dPoint(0,0,-1));
				else
					RotateVectorAroundVector(lineVect,angle,f3dPoint(0,0,1));
				startPt=beforArcline.End()+lineVect*arcRadius;
			}
			//后一条焊缝边
			if(fabs(afterArcLine.SectorAngle())<EPS2)
			{
				lineVect= afterArcLine.Start() - afterArcLine.End();
				normalize(lineVect);
				endPt = afterArcLine.Start() - lineVect*arcRadius;
			}
			else 
			{
				/*lineVect=afterArcLine.Start()-afterArcLine.Center();
				normalize(lineVect);
				if(afterArcLine.WorkNorm().z>0)
					lineVect.Set(-lineVect.y, lineVect.x);
				else
					lineVect.Set( lineVect.y,-lineVect.x);
				endPt = afterArcLine.Start() + lineVect*arcRadius;*/
				//
				double cosa=0.5*arcRadius/afterArcLine.Radius();
				double angle=ACOS(cosa);
				lineVect=afterArcLine.Center()-afterArcLine.Start();
				normalize(lineVect);
				if(beforArcline.WorkNorm().z>0)
					RotateVectorAroundVector(lineVect,angle,f3dPoint(0,0,-1));
				else
					RotateVectorAroundVector(lineVect,angle,f3dPoint(0,0,1));
				endPt=afterArcLine.Start()+lineVect*arcRadius;
			}
			//切角
			if(sys.part_map.plate.iCutArcsType==1)
			{
				line.startPt=startPt;
				line.endPt=endPt;
				line.pen.style=PS_DASH;
				arcLine=LineToArc(line);
			}
			//切弧
			else
			{
				arcLine.CreateMethod3(endPt,startPt,f3dPoint(0,0,1),arcRadius,pCurVertex->vertex);
				arcLine.pen.style=PS_DASH;
			}
			edge_arr.Add(arcLine);
		}
	}
}
//</DEVELOP_PROCESS_MARK>
BOOL CPlankDrawing::CreateParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing/*=TRUE*/)
{
	if(pParamPlate->m_iParamType==TYPE_UEND||pParamPlate->m_iParamType==TYPE_ROLLEND||pParamPlate->m_iParamType==TYPE_XEND)
		return CreateUEndOrRollEndParamPlate(pParamPlate,bPatternDrawing);
	else if(pParamPlate->m_iParamType==TYPE_FL||pParamPlate->m_iParamType==TYPE_FLG||pParamPlate->m_iParamType==TYPE_FLR)
		return CreateFLParamPlate(pParamPlate,bPatternDrawing);
	else if(pParamPlate->m_iParamType==TYPE_FLD||pParamPlate->m_iParamType==TYPE_FLP)
		return CreateStdFLParamPlate(pParamPlate,bPatternDrawing);	//生成高颈法兰
	else if(pParamPlate->m_iParamType==TYPE_CIRRIBPLATE||pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE
		||pParamPlate->m_iParamType==TYPE_TUBERIBPLATE)
		return CreateCircularParamPlate2(pParamPlate,bPatternDrawing);
		//return CreateCircularParamPlate(pParamPlate,bPatternDrawing);
	else if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE||pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE
		||pParamPlate->m_iParamType==TYPE_RIBPLATE||pParamPlate->m_iParamType==TYPE_TUBESTAYWIREPLATE)
		return CreateElbowParamPlate(pParamPlate,bPatternDrawing);
	else if(pParamPlate->m_iParamType==TYPE_WATERSHIELDEDPLATE)
		return CreateWaterShieldedPlate(pParamPlate,bPatternDrawing);
	else if(pParamPlate->m_iParamType==TYPE_BASEPLATE || pParamPlate->m_iParamType==TYPE_SHOEPLATE)
		return CreateFootParamPlate(pParamPlate,bPatternDrawing);
	else if(pParamPlate->m_iParamType==TYPE_PEDALPLATE)
		return CreatePedalParamPlate(pParamPlate, bPatternDrawing);
	else
		return FALSE;
}
BOOL CPlankDrawing::CreatePedalParamPlate(CLDSParamPlate *pParamPlate, BOOL bPatternDrawing/*=TRUE*/)
{
	if (pParamPlate->vertex_list.GetNodeNum() < 3)
		pParamPlate->DesignPlate();
	if (pParamPlate->vertex_list.GetNodeNum() < 3)
	{
		CString sPrompt;
#ifdef AFX_TARG_ENU_ENGLISH
		sPrompt.Format("handle is:plate parameter of 0X%x is wrong，please confirm!", pParamPlate->handle);
#else
		sPrompt.Format("句柄为:0X%x的板参数不正确，请确认!", pParamPlate->handle);
#endif
		AfxMessageBox(sPrompt);
		return FALSE;
	}
	//调整绘图坐标系
	BOOL bAdjustPlateUcs = FALSE;
	if (sys.part_map.plate.bAdjustPlateUcs || (sys.part_map.plate.bDimCoord && !bPatternDrawing))
		bAdjustPlateUcs = TRUE;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	PROFILE_VERTEX *pVertex = NULL;
	for (pVertex = pParamPlate->vertex_list.GetFirst(); pVertex; pVertex = pParamPlate->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	if (m_bSpecifyUCS)
	{	//指定绘图坐标系优先于内部调整坐标系
		bAdjustPlateUcs = TRUE;
		for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex, pParamPlate->ucs, TRUE);
			geom_center += pVertex->vertex; //
			coord_trans(pVertex->vertex, new_ucs, FALSE);
			if (pVertex->type == 1 || pVertex->type == 2)
			{	//转换圆心拾取点
				coord_trans(pVertex->center, pParamPlate->ucs, TRUE);
				coord_trans(pVertex->center, new_ucs, FALSE);
			}
		}
		//计算几何中心
		geom_center /= vertex_list.GetNodeNum();
		project_point(geom_center, pParamPlate->ucs.origin, pParamPlate->ucs.axis_z);
	}
	else if (bAdjustPlateUcs)	//标注坐标
	{	//对于参数化板可以直接修改顶点列表中的坐标值，因为每次使用参数板时重新设计就可以生成新的顶点列表
		UCS_STRU ucs;
		if (AdjustPlateUcs(pParamPlate, ucs, geom_center) > EPS2)
		{
			for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
			{
				coord_trans(pVertex->vertex, pParamPlate->ucs, TRUE);
				coord_trans(pVertex->vertex, ucs, FALSE);
				if (pVertex->type == 1 || pVertex->type == 2)
				{	//转换圆心拾取点
					coord_trans(pVertex->center, pParamPlate->ucs, TRUE);
					coord_trans(pVertex->center, ucs, FALSE);
				}
			}
			new_ucs = ucs;		//记录板坐标系
		}
		else
			new_ucs = pParamPlate->ucs;
	}
	else
	{
		new_ucs = pParamPlate->ucs;
		for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
		{
			f3dPoint vertex = pVertex->vertex;
			coord_trans(vertex, pParamPlate->ucs, TRUE);
			geom_center += vertex;
		}
		geom_center /= vertex_list.GetNodeNum();
		project_point(geom_center, pParamPlate->ucs.origin, pParamPlate->ucs.axis_z);
	}
	//调整卷边展开方向
	long A = 0, V = 0;
	double W = 0, H = 0;
	pParamPlate->GetDesignItemValue('A', &A);
	pParamPlate->GetDesignItemValue('V', &V);
	pParamPlate->GetDesignItemValue('W', &W);
	pParamPlate->GetDesignItemValue('H', &H);
	CLDSLineAngle* pDatumJg = (CLDSLineAngle*)Ta.FromPartHandle(A, CLS_LINEANGLE);
	double ddx = fabs(pParamPlate->ucs.axis_z*pDatumJg->vxWingNorm);
	double ddy = fabs(pParamPlate->ucs.axis_z*pDatumJg->vyWingNorm);
	GEPOINT spreadVec = (ddx > ddy) ? pDatumJg->GetWingVecX() : pDatumJg->GetWingVecY();
	spreadVec *= (V == 0) ? 1 : -1;
	vector_trans(spreadVec, new_ucs, FALSE);
	//绘制轮廓边
	double max_edge_len = 0;
	f3dArcLine arcLine;
	ARRAY_LIST<f3dPoint> profileVertexList;
	PROFILE_VERTEX *pPrevVertex = vertex_list.GetTail();
	for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
	{
		if (!bPatternDrawing)	//标注顶点相对坐标值
			DimCoordValue(pParamPlate, pVertex->vertex);
		double edge_len = 0;
		if (pPrevVertex)
		{
			
			if ((pPrevVertex->vertex.feature == 12 && pVertex->vertex.feature == 13) ||
				(pPrevVertex->vertex.feature == 13 && pVertex->vertex.feature == 12))
			{
				pPrevVertex = pVertex;
				continue;	//制弯线特殊绘制
			}
			f3dLine line;
			line.startPt = pPrevVertex->vertex;
			line.endPt = pVertex->vertex;
			line.startPt.z = line.endPt.z = 0;
			if (sys.part_map.plate.bDimPlateEdgeLen)//标注钢板边长
				DimPlateEdgeLen(line);
			arcLine.pen.style = PS_SOLID;
			arcLine.CreateMethod2(line.startPt, line.endPt, f3dPoint(0, 0, 1), 0);
			edge_arr.Add(arcLine);
			if (max_edge_len < DISTANCE(arcLine.Start(), arcLine.End()))
			{
				max_edge_len = DISTANCE(arcLine.Start(), arcLine.End());
				long_edge_mid = 0.5*(arcLine.Start() + arcLine.End());
				long_edge_angle = Cal2dLineAng(arcLine.Start().x, arcLine.Start().y, arcLine.End().x, arcLine.End().y);
			}
			//记录钢板外形轮廓点
			profileVertexList.append(arcLine.Start());
		}
		pPrevVertex = pVertex;
	}
	//绘制卷边
	double thick = pParamPlate->GetThick();
	f3dLine roll_line, huoqu_line, roll_edge;
	for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
	{
		if (pVertex->vertex.feature == 12)
			roll_line.startPt = pVertex->vertex;
		else if (pVertex->vertex.feature == 13)
			roll_line.endPt = pVertex->vertex;
	}
	for (int ii = 0; ii < 2; ii++)
	{
		GEPOINT vec = (ii == 0) ? GEPOINT(0, 1, 0) : spreadVec;
		GEPOINT line_vec = (roll_line.endPt - roll_line.startPt).normalized();
		GEPOINT offset_vec(line_vec.y, -line_vec.x);
		normalize(offset_vec);
		if (offset_vec*vec < 0)
			offset_vec *= -1;
		double roll_len = (ii == 0) ? W : H;
		huoqu_line.startPt = roll_line.startPt + offset_vec * thick;
		huoqu_line.endPt = roll_line.endPt + offset_vec * thick;
		roll_edge.startPt = roll_line.startPt + offset_vec * (roll_len + thick);
		roll_edge.endPt = roll_line.endPt + offset_vec * (roll_len + thick);
		profileVertexList.append(roll_edge.startPt);
		profileVertexList.append(roll_edge.endPt);
		arcLine.pen.style = PS_DASHDOTDOT;
		arcLine.CreateMethod2(huoqu_line.startPt, huoqu_line.endPt, f3dPoint(0, 0, 1), 0);
		edge_arr.Add(arcLine);
		//arcLine.pen.style = PS_DASHDOT;
		//arcLine.CreateMethod2(roll_line.startPt, roll_line.endPt, f3dPoint(0, 0, 1), 0);
		//edge_arr.Add(arcLine);
		arcLine.pen.style = PS_SOLID;
		arcLine.CreateMethod2(roll_edge.startPt, roll_edge.endPt, f3dPoint(0, 0, 1), 0);
		edge_arr.Add(arcLine);
		arcLine.CreateMethod2(roll_line.startPt, roll_edge.startPt, f3dPoint(0, 0, 1), 0);
		edge_arr.Add(arcLine);
		if (ii == 0)
		{
			arcLine.CreateMethod2(roll_line.endPt, huoqu_line.endPt, f3dPoint(0, 0, 1), 0);
			edge_arr.Add(arcLine);
			//记录挡板的卷边线
			roll_line.startPt = huoqu_line.endPt;
			roll_line.endPt = roll_edge.endPt;
		}
		else
		{
			arcLine.CreateMethod2(roll_line.endPt, roll_edge.endPt, f3dPoint(0, 0, 1), 0);
			edge_arr.Add(arcLine);
		}
		//添加火曲线工艺内容标注
		m_ciHuoQuType = 2;
		TXTDIM_STRU txt_dim;
		txt_dim.txtHeight = sys.fonts.plate.fDimTextSize;	//火曲工艺内容字体高度独立设置
		txt_dim.dimPos = (huoqu_line.startPt + huoqu_line.endPt)*0.5;
		char sHuoquType[50];
		if (ConfirmPlateHuoquType(thick, 90))
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sHuoquType, "(cold blending)");
		else
			strcpy(sHuoquType, "(hot blending)");
#else
			strcpy(sHuoquType, "(冷弯)");
		else
			strcpy(sHuoquType, "(热弯)");
#endif
		if (sys.part_map.bUseZhengFanQun)
		{	//正反曲
			if (m_ciHuoQuType==2)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(txt_dim.dimText, "front blending rolled edge 90°");
			else
				strcpy(txt_dim.dimText, "reverse blending rolled edge 90°");
#else
				strcpy(txt_dim.dimText, "正曲卷边90°");
			else
				strcpy(txt_dim.dimText, "反曲卷边90°");
#endif
		}
		else
		{	//外内曲
			if (m_ciHuoQuType == 2)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(txt_dim.dimText, "outer blending rolled edge 90°");
			else
				strcpy(txt_dim.dimText, "inner blending rolled edge 90°");
		}
		if (sys.part_map.bDimPlaneAngle)	//mark the angle between the two blended plane
			strcat(txt_dim.dimText, "two plane angle 90°");
#else
				strcpy(txt_dim.dimText, "外曲卷边90°");
			else
				strcpy(txt_dim.dimText, "内曲卷边90°");
	}
		if (sys.part_map.bDimPlaneAngle)	//标注两火曲面之间的夹角
			strcat(txt_dim.dimText, "两面夹角90°");
#endif
		if (sys.part_map.bDimHuoquType)		//标注火曲类型 冷弯热弯
			strcat(txt_dim.dimText, sHuoquType);
		txt_dim.dimVec = (huoqu_line.endPt - huoqu_line.startPt).normalized();
		text_dim_arr.Add(txt_dim);
		//标注卷边尺寸
		ROTATEDIM_STRU rotate_dim;
		rotate_dim.startPos = roll_edge.startPt + (roll_edge.endPt - roll_edge.startPt)*0.1;
		Int3dpl(huoqu_line, f3dLine(rotate_dim.startPos, rotate_dim.startPos + offset_vec * 1000), rotate_dim.endPos);
		rotate_dim.dimPos = (rotate_dim.startPos + rotate_dim.endPos)*0.5;
		rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x, rotate_dim.startPos.y, rotate_dim.endPos.x, rotate_dim.endPos.y);
		short nRollHeight = (short)ftol(DISTANCE(rotate_dim.startPos, rotate_dim.endPos));
#ifdef AFX_TARG_ENU_ENGLISH
		rotate_dim.sDimText.Printf("rolled edge%d", ftol(roll_len));
#else
		rotate_dim.sDimText.Printf("卷边 %d", ftol(roll_len));
#endif
		rotate_dim.bDisplayDimLine = TRUE;
		rotate_dim.bCanTextOutter = FALSE;
		rotate_dim_arr.Add(rotate_dim);
	}
	//初始化钢板外形轮廓区域
	if (profileVertexList.GetSize() > 0)
		profileRegion.CreatePolygonRgn(profileVertexList.m_pData, profileVertexList.GetSize());
	//标注螺栓孔
	f3dPoint basePt;
	f3dCircle ls_cir;
	for (CLsRef *pLsRef = pParamPlate->GetFirstLsRef(); pLsRef != NULL; pLsRef = pParamPlate->GetNextLsRef())
	{
		basePt = (*pLsRef)->ucs.origin;
		Sub_Pnt(basePt, (*pLsRef)->ucs.origin,
			(*pLsRef)->des_base_pos.norm_offset.Thick(pParamPlate->BelongModel())*(*pLsRef)->get_norm());
		ls_cir.centre = basePt;
		//标注螺栓相对坐标值
		if (bAdjustPlateUcs)
			coord_trans(ls_cir.centre, new_ucs, FALSE);
		else
			coord_trans(ls_cir.centre, pParamPlate->ucs, FALSE);
		ls_cir.centre.z = 0;		//必须清零，不然螺栓坐标间距计算不准
		//标注坐标时也有可能要分析螺栓射线
		for (int i = 1; i <= 32; i++)
		{
			if (pLsRef->dwRayNo&GetSingleWord(i))
				AnalizeLsRay(ls_cir.centre, i);
		}
		ls_cir.radius = (*pLsRef)->get_d() / 2.0;
		ls_cir.feature = (int)((*pLsRef)->hole_d_increment * 10);
		ls_cir.pen.style = (*pLsRef)->FilterFuncType();	//记录螺栓孔的功用类型
		ls_cir.centre.pen.style = pLsRef->cFlag;			//记录螺栓孔的工艺类型
		if ((int)pLsRef->waistLen > 0)
		{	//腰圆孔
			ls_cir.pen.width = (int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度
			ls_cir.norm = (*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
			if (bAdjustPlateUcs)
				vector_trans(ls_cir.norm, new_ucs, FALSE);
			else
				vector_trans(ls_cir.norm, pParamPlate->ucs, FALSE);//转换到钢板坐标系下
		}
		cir_arr.Add(ls_cir);
		//标注螺栓相对坐标值	wht 09-09-16
		if (!bPatternDrawing)
			DimCoordValue(pParamPlate, ls_cir.centre);
	}
	if (sys.part_map.plate.bDimPlateRayLs)
		DimLsRay(pParamPlate, bAdjustPlateUcs);
	return TRUE;
}
BOOL CPlankDrawing::CreateFootParamPlate(CLDSParamPlate *pParamPlate,BOOL bPatternDrawing/*=TRUE*/)
{
	if(pParamPlate->vertex_list.GetNodeNum()<3)
		pParamPlate->DesignPlate();
	if(pParamPlate->vertex_list.GetNodeNum()<3)
	{
		CString sPrompt;
#ifdef AFX_TARG_ENU_ENGLISH
		sPrompt.Format("handle is:plate parameter of 0X%x is wrong，please confirm!",pParamPlate->handle);
#else
		sPrompt.Format("句柄为:0X%x的板参数不正确，请确认!",pParamPlate->handle);
#endif
		AfxMessageBox(sPrompt);
		return FALSE;
	}
	//
	f3dLine line;
	f3dArcLine arcLine;
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||(sys.part_map.plate.bDimCoord&&!bPatternDrawing))
		bAdjustPlateUcs=TRUE;
	//复制顶点链表数据，绘制钢板零件图时使用该顶点链表,因为绘制过程可能修改链表数据
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=pParamPlate->vertex_list.GetFirst();pVertex;pVertex=pParamPlate->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	if(m_bSpecifyUCS) 
	{	//指定绘图坐标系优先于内部调整坐标系
		bAdjustPlateUcs=TRUE;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,pParamPlate->ucs,TRUE);
			geom_center+=pVertex->vertex; //
			coord_trans(pVertex->vertex,new_ucs,FALSE);
			if(pVertex->type==1||pVertex->type==2)
			{	//转换圆心拾取点
				coord_trans(pVertex->center,pParamPlate->ucs,TRUE);
				coord_trans(pVertex->center,new_ucs,FALSE);
			}
		}
		//计算几何中心
		geom_center/=vertex_list.GetNodeNum();
		project_point(geom_center,pParamPlate->ucs.origin,pParamPlate->ucs.axis_z);
	}
	else if(bAdjustPlateUcs)	//标注坐标
	{	//对于参数化板可以直接修改顶点列表中的坐标值，因为每次使用参数板时重新设计就可以生成新的顶点列表
		UCS_STRU ucs;
		if(AdjustPlateUcs(pParamPlate,ucs,geom_center)>EPS2)
		{
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				coord_trans(pVertex->vertex,pParamPlate->ucs,TRUE);
				coord_trans(pVertex->vertex,ucs,FALSE);
				if(pVertex->type==1||pVertex->type==2)
				{	//转换圆心拾取点
					coord_trans(pVertex->center,pParamPlate->ucs,TRUE);
					coord_trans(pVertex->center,ucs,FALSE);
				}
			}
			new_ucs=ucs;		//记录板坐标系
		}
		else
			new_ucs=pParamPlate->ucs;
	}
	else
	{
		new_ucs=pParamPlate->ucs;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pParamPlate->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=vertex_list.GetNodeNum();
		project_point(geom_center,pParamPlate->ucs.origin,pParamPlate->ucs.axis_z);
	}
	//绘制板轮廓
	ARRAY_LIST<f3dPoint> profileVertexList;
	double max_edge_len=0;
	PROFILE_VERTEX *pPrevVertex=vertex_list.GetTail();
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		//标注顶点相对坐标值
		if(!bPatternDrawing)
			DimCoordValue(pParamPlate,pVertex->vertex);
		double edge_len=0;
		if(pPrevVertex)
		{
			if(pPrevVertex->type>0)
			{
				int sign=1;
				if(pPrevVertex->type==1)
				{	//圆弧
					if(pPrevVertex->sector_angle<0)
						sign=-1;
					arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->sector_angle);
				}
				else if(pPrevVertex->type==2)	//指定圆弧R
				{	//圆心拾取点为相对坐标，此处不需要转换 
					f3dPoint center_pick=pPrevVertex->center;
					arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,1),pPrevVertex->radius,center_pick);
				}
				else if(pPrevVertex->type==3)	//椭圆弧
				{
					f3dPoint center = pPrevVertex->center;
					f3dPoint col_axis=pPrevVertex->column_norm;
					coord_trans(center,pParamPlate->ucs,FALSE);
					vector_trans(col_axis,pParamPlate->ucs,FALSE);
					if(pPrevVertex->radius<0)
						sign=-1;
					arcLine.CreateEllipse(center,pPrevVertex->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPrevVertex->radius);
					//重新绘制钢板的椭圆弧边
					DrawPlateEllipseLine(pParamPlate,arcLine,FALSE);
				}
			}
			else
			{	//直线
				line.startPt=pPrevVertex->vertex;
				line.endPt=pVertex->vertex;
				line.startPt.z = line.endPt.z=0;
				arcLine.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
				if(sys.part_map.plate.bDimPlateEdgeLen)//标注钢板边长
					DimPlateEdgeLen(line);
			}
			arcLine.pen.style=PS_SOLID;
			edge_arr.Add(arcLine);
			if(pPrevVertex->m_bWeldEdge)
				ProcessPlateWeldEdge(pPrevVertex,arcLine);//处理焊缝边(绘制焊缝线、绘制坡口线、标注焊接件号、标注坡口尺寸等) wht 18-08-09
			if(max_edge_len<DISTANCE(arcLine.Start(),arcLine.End()))
			{
				max_edge_len=DISTANCE(arcLine.Start(),arcLine.End());
				long_edge_mid=0.5*(arcLine.Start()+arcLine.End());
				long_edge_angle=Cal2dLineAng(arcLine.Start().x,arcLine.Start().y,arcLine.End().x,arcLine.End().y);
			}
			//记录钢板外形轮廓点
			profileVertexList.append(arcLine.Start());
		}
		pPrevVertex=pVertex;
	}
	//初始化钢板外形轮廓区域
	if(profileVertexList.GetSize()>0)
		profileRegion.CreatePolygonRgn(profileVertexList.m_pData,profileVertexList.GetSize());
	//标注螺栓孔
	f3dPoint basePt;
	f3dCircle ls_cir;
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef!=NULL;pLsRef=pParamPlate->GetNextLsRef())
	{
		basePt = (*pLsRef)->ucs.origin;
		Sub_Pnt(basePt,(*pLsRef)->ucs.origin,
			(*pLsRef)->des_base_pos.norm_offset.Thick(pParamPlate->BelongModel())*(*pLsRef)->get_norm());
		ls_cir.centre = basePt;
		//标注螺栓相对坐标值
		if(bAdjustPlateUcs)
			coord_trans(ls_cir.centre,new_ucs,FALSE);
		else
			coord_trans(ls_cir.centre,pParamPlate->ucs,FALSE);
		ls_cir.centre.z = 0;		//必须清零，不然螺栓坐标间距计算不准
		//标注坐标时也有可能要分析螺栓射线
		for(int i=1;i<=32;i++)
		{
			if(pLsRef->dwRayNo&GetSingleWord(i))
				AnalizeLsRay(ls_cir.centre,i);
		}
		ls_cir.radius = (*pLsRef)->get_d()/2.0;
		ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
		ls_cir.pen.style=(*pLsRef)->FilterFuncType();	//记录螺栓孔的功用类型
		ls_cir.centre.pen.style=pLsRef->cFlag;			//记录螺栓孔的工艺类型
		if((int)pLsRef->waistLen>0)
		{	//腰圆孔
			ls_cir.pen.width=(int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度
			ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
			if(bAdjustPlateUcs)
				vector_trans(ls_cir.norm,new_ucs,FALSE);
			else
				vector_trans(ls_cir.norm,pParamPlate->ucs,FALSE);//转换到钢板坐标系下
		}
		cir_arr.Add(ls_cir);
		//标注螺栓相对坐标值	wht 09-09-16
		if(!bPatternDrawing)
			DimCoordValue(pParamPlate,ls_cir.centre);
	}
	if(sys.part_map.plate.bDimPlateRayLs)
		DimLsRay(pParamPlate,bAdjustPlateUcs);
	//青岛东方要求大样图中不绘制焊接子构件
	if(!(gxDrwLocalizer.ciCustomerSerial==5&&bPatternDrawing))
		DrawWeldSubPlate(pParamPlate,bAdjustPlateUcs);
	return TRUE;
}
void InitRollEndPlateHuoquInfo(CLDSParamPlate* pPlate,CXhChar100& sDimText)
{
	if(pPlate==NULL)
		return;
	char sHuoquType[50]="";
	if(ConfirmPlateHuoquType(pPlate->GetThick(),90))
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(sHuoquType,"(cold blending)");
	else
		strcpy(sHuoquType,"(hot blending)");
#else
		strcpy(sHuoquType,"(冷弯)");
	else
		strcpy(sHuoquType,"(热弯)");
#endif
	double fAngle=0;
	if(pPlate->m_bStdPart)
		pPlate->GetDesignItemValue('X',&fAngle);
	else 
		pPlate->GetDesignItemValue('D',&fAngle);
	if(sys.part_map.bUseZhengFanQun)
	{	//正反曲
		if(fabs(fAngle)<EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sDimText,"front blending rolled edge 90°");
		else
			strcpy(sDimText,"reverse blending rolled edge 90°");
#else
		{
			if(gxDrwLocalizer.ciCustomerSerial==7)
				strcpy(sDimText,"正压90°");
			else
				strcpy(sDimText,"正曲卷边90°");
		}
		else
		{
			if(gxDrwLocalizer.ciCustomerSerial==7)
				strcpy(sDimText,"反压90°");
			else
				strcpy(sDimText,"反曲卷边90°");
		}
#endif
	}
	else
	{	//外内曲
		if(fabs(fAngle)<EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sDimText,"outter rolled edge 90°");
		else
			strcpy(sDimText,"inner rolled edge 90°");
	}
	if(sys.part_map.bDimPlaneAngle)	//mark the angle between the two blended plane
		strcat(sDimText,"the angle of two planes 90°");
#else
		{
			if(gxDrwLocalizer.ciCustomerSerial==7)
				strcpy(sDimText,"正压90°");
			else
				strcpy(sDimText,"外曲卷边90°");
		}
		else
		{
			if(gxDrwLocalizer.ciCustomerSerial==7)
				strcpy(sDimText,"反压90°");
			else
				strcpy(sDimText,"内曲卷边90°");
		}
	}
	if(sys.part_map.bDimPlaneAngle)	//标注两火曲面之间的夹角
		strcat(sDimText,"两面夹角90°");
#endif
	if(sys.part_map.bDimHuoquType)		//标注火曲类型 冷弯热弯
		strcat(sDimText,sHuoquType);
}

//绘制插板钢管简图
static bool CreateInsertPlateTubeSketchMap(CLDSGeneralPlate *pPlate,CLDSLineTube *pLineTube,
										   CArray<f3dArcLine,f3dArcLine&> &edge_arr,f3dPoint &origin,
										   f3dPoint &axisX,f3dPoint &axisY,f3dPoint &dim_pos)
{
	if(gxDrwLocalizer.ciCustomerSerial!=7)
		return false;
	BOOL bDrawVertPlate=FALSE;
	double M=0,L=0,N=0;
	CLDSParamPlate *pParentPlate=NULL;
	if(pPlate->GetClassTypeId()==CLS_PLATE)
	{	//十字插板竖直立板上
		CLDSPlate *pCurPlate=(CLDSPlate*)pPlate;
		if(pCurPlate->designInfo.origin.datum_pos.x!=0||pCurPlate->designInfo.origin.datum_pos.y!=0)
			return false;
		if(pCurPlate->designInfo.origin.des_para.hPart!=pCurPlate->designInfo.m_hBasePart)
			return false;
		if(pCurPlate->designInfo.origin.des_para.hPart!=pCurPlate->m_hPartWeldParent)
			return false;
		pParentPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pCurPlate->designInfo.m_hBasePart,CLS_PARAMPLATE);
		if( pParentPlate==NULL||pParentPlate->m_iParamType!=TYPE_XEND||
			pParentPlate->AttachPart.GetValue(pCurPlate->handle)==NULL)
			return false;
		pParentPlate->GetDesignItemValue('L',&L);
		pParentPlate->GetDesignItemValue('M',&M);
		pParentPlate->GetDesignItemValue('N',&N);
		if(pLineTube==NULL)
			pLineTube=(CLDSLineTube*)Ta.Parts.FromHandle(pParentPlate->m_hPartWeldParent,CLS_LINETUBE);
	}
	else if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPlate;
		if(pParamPlate->m_iParamType!=TYPE_XEND&&pParamPlate->m_iParamType!=TYPE_ROLLEND)
			return false;
		pParamPlate->GetDesignItemValue('L', &L);
		pParamPlate->GetDesignItemValue('M', &M);
		if(pLineTube==NULL)
			pLineTube=(CLDSLineTube*)Ta.Parts.FromHandle(pParamPlate->m_hPartWeldParent,CLS_LINETUBE);
		bDrawVertPlate=pParamPlate->m_iParamType==TYPE_XEND;
	}
	if(pLineTube==NULL)
		return false;
	CMaxDouble maxXUpBolt,maxXDownBolt;
	if(N==0||bDrawVertPlate)
	{
		for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			f3dPoint centre=pBolt->ucs.origin;
			coord_trans(centre,pPlate->ucs,FALSE);
			centre.z=0;
			if(bDrawVertPlate)
			{	//十字插板
				if(fabs(pLsRef->GetLsPtr()->get_norm()*pPlate->ucs.axis_z)<EPS_COS2)
					continue;	//螺栓不在十字型插板基板上
				if(centre.x>L)
					continue;	//螺栓不在十字型插板的基板上
			}
			if(centre.y>0)
				maxXUpBolt.Update(centre.x,pBolt);
			else
				maxXDownBolt.Update(centre.x,pBolt);
		}
	}
	//初始化钢管插入插板的特征线
	double fTubeD=pLineTube->GetDiameter(),fTubeThick=pLineTube->GetThick();
	double fDrawY=N>0?N:(0.5*fTubeD);
	f3dLine line,up_base_line,dw_base_line,up_thick_line,dw_thick_line;
	up_base_line.startPt=origin+axisY*fDrawY;
	up_base_line.endPt	=origin+axisX*M+axisY*fDrawY;
	up_thick_line.startPt=origin+axisY*(fDrawY-fTubeThick);
	up_thick_line.endPt	=origin+axisX*M+axisY*(fDrawY-fTubeThick);
	dw_base_line.startPt=origin-axisY*fDrawY;
	dw_base_line.endPt	=origin+axisX*M-axisY*fDrawY;
	dw_thick_line.startPt=origin-axisY*(fDrawY-fTubeThick);
	dw_thick_line.endPt	=origin+axisX*M-axisY*(fDrawY-fTubeThick);
	//上侧轮廓边
	line=up_base_line;
	line.pen.style=PS_OTHERLINE;
	edge_arr.Add(LineToArc(line));
	line=up_thick_line;
	line.pen.style=PS_DASH;
	edge_arr.Add(LineToArc(line));
	if(N==0)
	{	//下侧轮廓边
		line=dw_base_line;
		line.pen.style=PS_OTHERLINE;
		edge_arr.Add(LineToArc(line));
		line=dw_thick_line;
		line.pen.style=PS_DASH;
		edge_arr.Add(LineToArc(line));
		//端头线
		line=f3dLine(up_base_line.endPt,dw_base_line.endPt);
		line.pen.style=PS_OTHERLINE;
		edge_arr.Add(LineToArc(line));
		//
		dim_pos=origin+axisX*0.5*M-axisY*(0.5*fTubeD-fTubeThick);
	}
	else
	{	//半侧端头线
		line.startPt=up_base_line.endPt;
		line.endPt=origin+axisX*M;
		line.pen.style=PS_OTHERLINE;
		edge_arr.Add(LineToArc(line));
		dim_pos=origin+axisX*0.5*M+axisY*3*fTubeThick;
		//标注立板X坐标最大螺栓至板边的距离

	}
	if(bDrawVertPlate)
	{	//立板截面图
		line.startPt=origin+axisY*0.5*pPlate->GetThick();
		line.endPt	=origin+axisX*L+axisY*0.5*pPlate->GetThick();
		line.pen.style=PS_OTHERLINE;
		edge_arr.Add(LineToArc(line));
		line.startPt=origin-axisY*0.5*pPlate->GetThick();
		line.endPt	=origin+axisX*L-axisY*0.5*pPlate->GetThick();
		line.pen.style=PS_OTHERLINE;
		edge_arr.Add(LineToArc(line));
		line=f3dLine(origin,origin+axisX*L);
		line.pen.style=PS_DASHDOT;
		edge_arr.Add(LineToArc(line));
		//标注十字插板基板螺栓至立板上下面距离

	}
	return true;
}


BOOL CPlankDrawing::CreateUEndOrRollEndParamPlate(CLDSParamPlate *pPlate,BOOL bPatternDrawing/*=TRUE*/)
{
	if(pPlate->m_iParamType!=TYPE_UEND&&pPlate->m_iParamType!=TYPE_ROLLEND&&pPlate->m_iParamType!=TYPE_XEND)
		return FALSE;
	//获取插板相关设计参数
	double fTubeD=0,fTubeThick=0;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent);
	if(pDatumTube)
	{
		fTubeD=pDatumTube->GetDiameter();
		fTubeThick=pDatumTube->GetThick();
	}
	else if(pDatumTube==NULL&&pPlate->m_bStdPart)
	{
		pPlate->GetStdPartParamValue('d',&fTubeD);
		pPlate->GetStdPartParamValue('t',&fTubeThick);
	}
	else
		return FALSE;
	double L=0,N=0,M=0,S=0,H=0,W=0,A=0,R=0;
	pPlate->GetDesignItemValue('L', &L);
	pPlate->GetDesignItemValue('N', &N);
	pPlate->GetDesignItemValue('M', &M);
	pPlate->GetDesignItemValue('S', &S);
	pPlate->GetDesignItemValue('H', &H);
	pPlate->GetDesignItemValue('W', &W);
	pPlate->GetDesignItemValue('A', &A);
	pPlate->GetDesignItemValue('R', &R);
	//初始化钢板几何中心
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		f3dPoint vertex=pVertex->vertex;
		coord_trans(vertex,pPlate->ucs,TRUE);
		geom_center+=vertex;
	}
	geom_center/=pPlate->vertex_list.GetNodeNum();
	project_point(geom_center,pPlate->ucs.origin,pPlate->ucs.axis_z);
	//调整插板绘图坐标系
	UCS_STRU ucs,old_ucs=pPlate->ucs;	//记录未调整前的坐标系
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||sys.part_map.plate.bDimCoord)
		bAdjustPlateUcs=TRUE;	//调整坐标系
	if(m_bSpecifyUCS)	//指定绘图坐标系优先于内部调整坐标系
		bAdjustPlateUcs=TRUE;
	else if(bAdjustPlateUcs&&AdjustPlateUcs(pPlate,ucs,geom_center)>EPS2&&
			(pPlate->m_iParamType==TYPE_ROLLEND||pPlate->m_iParamType==TYPE_XEND))
		new_ucs=ucs;	//记录板坐标系
	else
		new_ucs=pPlate->ucs;
	pPlate->ucs=new_ucs;
	//绘制插板的大样图及构件图
	f3dLine line;
	f3dCircle ls_cir;
	f3dArcLine arcline;
	if(bPatternDrawing||bAdjustPlateUcs||m_bSpecifyUCS)
	{	//加工大样纸图或者标注顶点坐标
		f3dPoint origin,axisX(1,0,0),axisY(0,1,0);
		if(bAdjustPlateUcs)
		{	//调整长边中点位置
			coord_trans(origin,old_ucs,TRUE);
			coord_trans(origin,pPlate->ucs,FALSE);
			vector_trans(axisX,old_ucs,TRUE);
			vector_trans(axisX,pPlate->ucs,FALSE);
			vector_trans(axisY,old_ucs,TRUE);
			vector_trans(axisY,pPlate->ucs,FALSE);
		}
		//数据点位置
		double fTextH=sys.fonts.plate.fDimTextSize;
		double fCutN=pPlate->m_iParamType==TYPE_XEND?0:N;
		long_edge_mid=origin+axisX*(L*0.5-fCutN)+axisY*(-fTextH*3);
		//
		if(pPlate->m_iParamType==TYPE_UEND)
		{
			double rough_len=2*(L-0.5*W-pPlate->thick)+(W+pPlate->thick)*Pi/2;	//实际下料长度（已考虑火曲变形）
			double half_len=rough_len*0.5;
			double huoqu_len=0.5*(W+pPlate->thick)*Pi;
			//
			f3dPoint vertex[14];
			vertex[0]=origin+axisX*(-half_len+N)+axisY*-0.5*H;
			vertex[1]=origin+axisX*(half_len-N)+axisY*-0.5*H;
			vertex[2]=origin+axisX*half_len+axisY*(-0.5*H+S);
			vertex[3]=origin+axisX*half_len+axisY*(0.5*H-S);
			vertex[4]=origin+axisX*(half_len-N)+axisY*0.5*H;
			vertex[5]=origin+axisX*(-half_len+N)+axisY*0.5*H;
			vertex[6]=origin+axisX*-half_len+axisY*(0.5*H-S);
			vertex[7]=origin+axisX*-half_len+axisY*(-0.5*H+S);
			//绘制轮廓边
			line.pen.style=PS_SOLID;
			for(int i=0;i<8;i++)
			{	//绘制槽型插板轮廓线
				line.startPt=vertex[i];
				line.endPt=vertex[(i+1)%8];
				edge_arr.Add(LineToArc(line));
				if(sys.part_map.plate.bDimPlateEdgeLen)//标注钢板边长
					DimPlateEdgeLen(line);
				if(!bPatternDrawing)
					DimCoordValue(pPlate,line.startPt);	//标注顶点坐标
			}
			//绘制火曲线
			line.pen.style=PS_DASHDOTDOT;
			line.startPt=origin+axisY*(-0.5*H);
			line.endPt  =origin+axisY*0.5*H;
			edge_arr.Add(LineToArc(line));
			line.startPt=origin+axisX*0.5*huoqu_len+axisY*(-0.5*H);
			line.endPt  =origin+axisX*0.5*huoqu_len+axisY*0.5*H;
			edge_arr.Add(LineToArc(line));
			line.startPt=origin+axisX*(-0.5*huoqu_len)+axisY*(-0.5*H);
			line.endPt  =origin+axisX*(-0.5*huoqu_len)+axisY*.5*H;
			edge_arr.Add(LineToArc(line));
			//绘制螺栓孔或图符
			for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{
				f3dPoint base_pt=pLsRef->GetLsPtr()->ucs.origin;
				coord_trans(base_pt,pPlate->ucs,FALSE);
				base_pt.z=0;
				base_pt+=axisX*(0.5*rough_len-L);
				for(int i=0;i<2;i++)
				{
					if(i==0)
						ls_cir.centre.Set(base_pt.x,base_pt.y);
					else
						ls_cir.centre.Set(-base_pt.x,base_pt.y);
					//标注坐标时也有可能要分析螺栓射线
					for(int j=1;j<=32;j++)
					{
						if(pLsRef->dwRayNo&GetSingleWord(j))
							AnalizeLsRay(ls_cir.centre,j);
					}
					ls_cir.radius = (*pLsRef)->get_d()/2.0;
					ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
					ls_cir.pen.style=(*pLsRef)->FilterFuncType();	//记录螺栓孔的功用类型
					ls_cir.centre.pen.style=pLsRef->cFlag;			//记录螺栓孔的工艺类型
					if((int)pLsRef->waistLen>0)
					{	//腰圆孔	wht 10-02-25
						ls_cir.pen.width=(int)pLsRef->waistLen;	//使用画笔宽度保存腰圆孔长度
						ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
						vector_trans(ls_cir.norm,pPlate->ucs,FALSE);//转换到钢板坐标系下
					}
					cir_arr.Add(ls_cir);
					//标注顶点坐标
					if(!bPatternDrawing)
						DimCoordValue(pPlate,ls_cir.centre);	
				}
			}
		}
		else if(pPlate->m_iParamType==TYPE_ROLLEND)
		{	//卷边端板
			//折弯90°时 设火曲半径/钢板厚度(r/t)=0.5,则中性层系数x=0.38
			double x=0.38;				//中性层系数
			double r=pPlate->thick*0.5;	//火曲半径
			//火曲线位置向中心移动半个0.5*odd_len 外侧两轮廓边向中心移动odd_len  wht 10-03-18
			double odd_len=2*(r+pPlate->thick)-0.5*Pi*(r+x*pPlate->thick); //火曲变形量
			if(W<EPS||!sys.part_map.bExportDeformedProfile)//无卷边或不考虑火曲变形
				odd_len=0;
			//绘制内侧制弯线，向里偏移一个板厚  wxc-2018.10.30
			if(sys.part_map.plate.iZhiWanLineType==1)
				odd_len+=pPlate->thick+sys.part_map.plate.fInnerOffThreshold;
			int nVertexCount=0;	//轮廓点个数 wht 11-04-18
			f3dPoint vertex[14];
			if(pPlate->m_cRollProcessType==0)
			{	//展开后倒角处为两条直线 最佳受力
				vertex[0]=origin+axisY*(-0.5*H-W+odd_len);
				vertex[1]=origin+axisX*(L-N)+axisY*(-0.5*H-W+odd_len);
				vertex[2]=origin+axisX*(L-N)+axisY*(-0.5*H+0.5*odd_len);
				vertex[3]=origin+axisX*L+axisY*(-0.5*H+S);
				vertex[4]=origin+axisX*L+axisY*(0.5*H-S);
				vertex[5]=origin+axisX*(L-N)+axisY*(0.5*H-0.5*odd_len);
				vertex[6]=origin+axisX*(L-N)+axisY*(0.5*H+W-odd_len);
				vertex[7]=origin+axisY*(0.5*H+W-odd_len);
				nVertexCount=8;
			}
			else if(pPlate->m_cRollProcessType==1)
			{	//保证卷边外侧边长度为L-N 
				vertex[0]=origin+axisY*(-0.5*H-W+odd_len);
				vertex[1]=origin+axisX*(L-N)+axisY*(-0.5*H-W+odd_len);
				vertex[2]=origin+axisX*L+axisY*(-0.5*H+S);
				vertex[3]=origin+axisX*L+axisY*(0.5*H-S);
				vertex[4]=origin+axisX*(L-N)+axisY*(0.5*H+W-odd_len);
				vertex[5]=origin+axisY*(0.5*H+W-odd_len);
				nVertexCount=6;
			}
			else //if(pPlate->cRollProcessType==2)
			{	//保证卷边内侧边长度为L-N 
				double fOffsetLen=0.0;
				if(S>0) //S==0时偏移量为0
					fOffsetLen=W*N/S;
				vertex[0]=origin+axisY*(-0.5*H-W+odd_len);
				vertex[1]=origin+axisX*(L-N-fOffsetLen)+axisY*(-0.5*H-W+odd_len);
				vertex[2]=origin+axisX*L+axisY*(-0.5*H+S);
				vertex[3]=origin+axisX*L+axisY*(0.5*H-S);
				vertex[4]=origin+axisX*(L-N-fOffsetLen)+axisY*(0.5*H+W-odd_len);
				vertex[5]=origin+axisY*(0.5*H+W-odd_len);
				nVertexCount=6;
			}
			//绘制轮廓边
			ARRAY_LIST<f3dLine> line_arr;
			line.pen.style=PS_SOLID;
			for(int i=0;i<nVertexCount;i++)
			{	//绘制槽型插板轮廓线
				line.startPt=vertex[i];
				line.endPt=vertex[(i+1)%nVertexCount];
				edge_arr.Add(LineToArc(line));
				if(sys.part_map.plate.bDimPlateEdgeLen)//标注钢板边长
					DimPlateEdgeLen(line);
				if(!bPatternDrawing)
					DimCoordValue(pPlate,line.startPt);	//标注顶点坐标
				if(i>0&&i<nVertexCount-2)
					line_arr.append(line);
			}
			//绘制中心线
			line.pen.style=PS_DASHDOT;
			line.startPt=origin;
			line.endPt	=origin+axisX*L;
			edge_arr.Add(LineToArc(line));
			//存在卷边时需要绘制火曲线
			if(W>EPS)
			{
				CXhChar100 sHuoquText;
				InitRollEndPlateHuoquInfo(pPlate,sHuoquText);
				f3dPoint huoqu1_ptS,huoqu1_ptE,huoqu2_ptS,huoqu2_ptE,inters;
				//上侧火曲线,保证火曲线在轮廓边内
				huoqu1_ptS=origin+axisY*(0.5*H-odd_len);
				huoqu1_ptE=origin+axisX*L+axisY*(0.5*H-odd_len);
				for(int i=0;i<line_arr.GetSize();i++)
				{
					if(Int3dll(f3dLine(huoqu1_ptS,huoqu1_ptE),line_arr[i],inters)>0)
					{
						huoqu1_ptE=inters;
						break;
					}
				}
				//绘制火曲线
				line.pen.style=PS_DASHDOTDOT;
				line.startPt=huoqu1_ptS;
				line.endPt=huoqu1_ptE;
				edge_arr.Add(LineToArc(line));
				//标注火曲信息 wht 10-08-26
				TXTDIM_STRU txt_dim;
				txt_dim.txtHeight=sys.fonts.plate.fDimTextSize;	//火曲工艺内容字体高度独立设置
				txt_dim.dimText.Copy(sHuoquText);
				txt_dim.dimPos=(line.startPt+line.endPt)*0.5;
				txt_dim.dimVec=(line.endPt-line.startPt).normalized();
				double fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
				txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
				text_dim_arr.Add(txt_dim);
				//标注上卷边长度长度
				txt_dim.dimPtS=vertex[nVertexCount-1];
				txt_dim.dimPtE=huoqu1_ptS;
				txt_dim.dimPos=(txt_dim.dimPtS+txt_dim.dimPtE)*0.5;
				txt_dim.dimVec=(txt_dim.dimPtE-txt_dim.dimPtS).normalized();
				double fDist=DISTANCE(txt_dim.dimPtS,txt_dim.dimPtE);
				txt_dim.dimText.Printf("%d",ftoi(fDist));
				text_dim_arr.Add(txt_dim);
				//下侧火曲线,保证火曲线在轮廓边内
				huoqu2_ptS=origin+axisY*(-0.5*H+odd_len);
				huoqu2_ptE=origin+axisX*L+axisY*(-0.5*H+odd_len);
				for(int i=0;i<line_arr.GetSize();i++)
				{
					if(Int3dll(f3dLine(huoqu2_ptS,huoqu2_ptE),line_arr[i],inters)>0)
					{
						huoqu2_ptE=inters;
						break;
					}
				}
				//绘制火曲线
				line.pen.style=PS_DASHDOTDOT;
				line.startPt=huoqu2_ptS;
				line.endPt=huoqu2_ptE;
				edge_arr.Add(LineToArc(line));
				//标注火曲信息 wht 10-08-26
				txt_dim.dimText.Copy(sHuoquText);
				txt_dim.dimPos=(line.startPt+line.endPt)*0.5;
				txt_dim.dimVec=(line.endPt-line.startPt).normalized();
				fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
				txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
				text_dim_arr.Add(txt_dim);
				//标注下卷边长度
				txt_dim.dimPtS=huoqu2_ptS;
				txt_dim.dimPtE=vertex[0];
				txt_dim.dimPos=(txt_dim.dimPtS+txt_dim.dimPtE)*0.5;
				txt_dim.dimVec=(txt_dim.dimPtE-txt_dim.dimPtS).normalized();
				fDist=DISTANCE(txt_dim.dimPtS,txt_dim.dimPtE);
				txt_dim.dimText.Printf("%d",ftoi(fDist));
				text_dim_arr.Add(txt_dim);
				//标注上下火曲间距
				txt_dim.dimPtS=huoqu1_ptS;
				txt_dim.dimPtE=huoqu2_ptS;
				txt_dim.dimPos=(txt_dim.dimPtS+txt_dim.dimPtE)*0.5;
				txt_dim.dimVec=(txt_dim.dimPtE-txt_dim.dimPtS).normalized();
				fDist=DISTANCE(txt_dim.dimPtS,txt_dim.dimPtE);
				txt_dim.dimText.Printf("%d",ftoi(fDist));
				text_dim_arr.Add(txt_dim);
				//卷边外侧宽度
				txt_dim.dimText.Printf("卷边后外侧边宽度为%d",ftoi(H));
				txt_dim.dimPos=(huoqu1_ptS+huoqu2_ptS)*0.5+axisX*txt_dim.txtHeight*3;
				txt_dim.dimVec=(huoqu2_ptS-huoqu1_ptS).normalized();
				fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
				txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
				text_dim_arr.Add(txt_dim);
			}
			//绘制螺栓孔或图符
			for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{
				ls_cir.centre=pLsRef->GetLsPtr()->ucs.origin;
				coord_trans(ls_cir.centre,pPlate->ucs,FALSE);
				ls_cir.centre.z=0;
				//标注坐标时也有可能要分析螺栓射线
				for(int j=1;j<=32;j++)
				{
					if(pLsRef->dwRayNo&GetSingleWord(j))
						AnalizeLsRay(ls_cir.centre,j);
				}
				ls_cir.radius = (*pLsRef)->get_d()/2.0;
				ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
				ls_cir.pen.style=(*pLsRef)->FilterFuncType();	//记录螺栓孔的功用类型
				ls_cir.centre.pen.style=pLsRef->cFlag;			//记录螺栓孔的工艺类型
				if((int)pLsRef->waistLen>0)
				{	//腰圆孔	wht 10-02-25
					ls_cir.pen.width=(int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度
					ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
					vector_trans(ls_cir.norm,pPlate->ucs,FALSE);//转换到钢板坐标系下
				}
				cir_arr.Add(ls_cir);
				//标注螺栓相对坐标值
				if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
					DimCoordValue(pPlate,ls_cir.centre);	//标注顶点坐标
			}
		}
		else //if(pPlate->m_iParamType==TYPE_XEND)
		{	//十字插板大样图
			f3dPoint vertex[4];
			vertex[0]=origin+axisY*(-0.5*H);
			vertex[1]=origin+axisX*L+axisY*(-0.5*H);
			vertex[2]=origin+axisX*L+axisY*0.5*H;
			vertex[3]=origin+axisY*0.5*H;
			//绘制轮廓边
			line.pen.style=PS_SOLID;
			for(int i=0;i<4;i++)
			{	//绘制槽型插板轮廓线
				line.startPt=vertex[i];
				line.endPt=vertex[(i+1)%4];
				edge_arr.Add(LineToArc(line));
				if(sys.part_map.plate.bDimPlateEdgeLen)//标注钢板边长
					DimPlateEdgeLen(line);
				if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
					DimCoordValue(pPlate,line.startPt);	//标注顶点坐标
				
			}
			//绘制螺栓孔或图符
			for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
			{
				if(fabs(pLsRef->GetLsPtr()->get_norm()*pPlate->ucs.axis_z)<EPS_COS2)
					continue;	//螺栓不在十字型插板基板上
				ls_cir.centre=pLsRef->GetLsPtr()->ucs.origin;
				coord_trans(ls_cir.centre,pPlate->ucs,FALSE);
				ls_cir.centre.z=0;
				if(ls_cir.centre.x>L)
					continue;	//螺栓不在十字型插板的基板上
				//标注坐标时也有可能要分析螺栓射线
				for(int j=1;j<=32;j++)
				{
					if(pLsRef->dwRayNo&GetSingleWord(j))
						AnalizeLsRay(ls_cir.centre,j);
				}
				ls_cir.radius = (*pLsRef)->get_d()/2.0;
				ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
				ls_cir.pen.style=(*pLsRef)->FilterFuncType();	//记录螺栓孔的功用类型
				ls_cir.centre.pen.style=pLsRef->cFlag;			//记录螺栓孔的工艺类型
				if((int)pLsRef->waistLen>0)
				{	//腰圆孔	wht 10-02-25
					ls_cir.pen.width=(int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度
					ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
					vector_trans(ls_cir.norm,pPlate->ucs,FALSE);//转换到钢板坐标系下
				}
				cir_arr.Add(ls_cir);
				//标注顶点坐标
				if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
					DimCoordValue(pPlate,ls_cir.centre);	
			}
		}
		//绘制插板大样图中的钢管简图 wht 18-08-09
		f3dPoint datum_pos;
		if(CreateInsertPlateTubeSketchMap(pPlate,pDatumTube,edge_arr,origin,axisX,axisY,datum_pos))
			long_edge_mid=datum_pos;
	}
	else
	{	//零件详图
		//水平放置图绘制
			//钢管上侧外壁线
		line.startPt.Set(0,0.5*fTubeD);
		line.endPt.Set(M,0.5*fTubeD);
		line.pen.style=PS_WELDLINE;
		edge_arr.Add(LineToArc(line));	//上侧焊缝线
		line.startPt.Set(-(H-30)/2,0.5*fTubeD);
		line.endPt.Set(M,0.5*fTubeD);
		if(pDatumTube)
			line.feature=pDatumTube->handle;
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
		line.feature=0;
			//钢管上侧内壁线
		//line.startPt.x+=0.028*H;	//此附加值目的是为了与钢管破断线对齐
		line.startPt.y-=fTubeThick;
		line.endPt.y-=fTubeThick;
		line.pen.style=PS_DASH;
		edge_arr.Add(LineToArc(line));
			//钢管中心轴线
		line.startPt.Set(-H*0.5,0);
		line.endPt.Set(L+20,0);
		line.pen.style=PS_DASHDOT;
		edge_arr.Add(LineToArc(line));
			//钢管下侧外壁线
		line.startPt.Set(M,-0.5*fTubeD);
		line.endPt.Set(0,-0.5*fTubeD);
		line.pen.style=PS_WELDLINE;
		edge_arr.Add(LineToArc(line));	//下侧焊缝线
		line.startPt.Set(-(H-30)/2,-0.5*fTubeD);
		line.endPt.Set(M,-0.5*fTubeD);
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
		line.feature=0;
			//钢管下侧内壁线
		//line.startPt.x-=0.015*H;	//此附加值目的是为了与钢管破断线对齐
		line.startPt.y+=fTubeThick;
		line.endPt.y+=fTubeThick;
		line.pen.style=PS_DASH;
		edge_arr.Add(LineToArc(line));
			//钢管左侧破断线
		line.startPt.Set(-(H-30)/2,fTubeD*0.5);
		line.endPt.Set(line.startPt.x,line.startPt.y-fTubeD);
		line.pen.style=PS_TUBEBREAK;
		line.feature=-ftoi(fTubeThick);
		edge_arr.Add(LineToArc(line));
			//钢管右侧边线
		line.startPt.Set(M,0.5*fTubeD);
		line.endPt.Set(M,-0.5*fTubeD);
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
			//端板边缘线（自下侧外壁线开始）
		if(pPlate->m_iParamType==TYPE_UEND||pPlate->m_iParamType==TYPE_ROLLEND)
		{
			line.startPt.Set(0,-0.5*H);
			line.endPt.Set(L-N,-0.5*H);
			edge_arr.Add(LineToArc(line));
			line.startPt=line.endPt;
			line.endPt+=f3dPoint(N,S);
			edge_arr.Add(LineToArc(line));
			line.startPt=line.endPt;
			line.endPt.y+=H-2*S;
			edge_arr.Add(LineToArc(line));
			line.startPt=line.endPt;
			line.endPt+=f3dPoint(-N,+S);
			edge_arr.Add(LineToArc(line));
			line.startPt=line.endPt;
			line.endPt.x=0;
			edge_arr.Add(LineToArc(line));
			line.startPt=line.endPt;
			line.endPt.y=-0.5*H;
			edge_arr.Add(LineToArc(line));
			if(pPlate->m_iParamType==TYPE_ROLLEND&&W>0)
			{
				line.startPt.Set(0,0.5*H-pPlate->thick);
				line.endPt.Set(L-N+pPlate->thick*N/S,0.5*H-pPlate->thick);
				edge_arr.Add(LineToArc(line));
				line.startPt.Set(0,-0.5*H+pPlate->thick);
				line.endPt.Set(L-N+pPlate->thick*N/S,-0.5*H+pPlate->thick);
				edge_arr.Add(LineToArc(line));
			}
			if(pPlate->m_iParamType==TYPE_ROLLEND&&pPlate->AttachPart.GetNodeNum()>0)
			{
				ATTACHPART_ITEM *pItem=pPlate->GetFirstAttachPart();
				CLDSPlate *pTempPlate=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(pItem->h);
				double vert_plate_L=0;
				for(PROFILE_VERTEX* pVertex=pTempPlate->vertex_list.GetFirst();pVertex;pVertex=pTempPlate->vertex_list.GetNext())
				{
					if(pVertex->vertex.x>vert_plate_L)
						vert_plate_L=pVertex->vertex.x;
				}
				if(vert_plate_L>0)
				{	//绘制插板上附加立板
					line.startPt.Set(0,0.5*pPlate->thick);
					line.endPt.Set(vert_plate_L,0.5*pPlate->thick);
					edge_arr.Add(LineToArc(line));
					line.startPt.Set(0,-0.5*pPlate->thick);
					line.endPt.Set(vert_plate_L,-0.5*pPlate->thick);
					edge_arr.Add(LineToArc(line));
					line.startPt.Set(0,0.5*pPlate->thick);
					line.endPt.Set(0,-0.5*pPlate->thick);
					edge_arr.Add(LineToArc(line));
					line.startPt.Set(vert_plate_L,0.5*pPlate->thick);
					line.endPt.Set(vert_plate_L,-0.5*pPlate->thick);
					edge_arr.Add(LineToArc(line));
				}	
			}
		}
		else if(pPlate->m_iParamType==TYPE_XEND)
		{
			line.pen.style=PS_SOLID;
			line.startPt.Set(0,-0.5*H);
			line.endPt.Set(L,-0.5*H);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(L,-0.5*H);
			line.endPt.Set(L,0.5*H);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(L,0.5*H);
			line.endPt.Set(0,0.5*H);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(0,0.5*H);
			line.endPt.Set(0,-0.5*H);
			edge_arr.Add(LineToArc(line));
			//立板
			line.startPt.Set(0,0.5*pPlate->thick);
			line.endPt.Set(L,0.5*pPlate->thick);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(0,-0.5*pPlate->thick);
			line.endPt.Set(L,-0.5*pPlate->thick);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(0,0.5*pPlate->thick);
			line.endPt.Set(0,-0.5*pPlate->thick);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(L,0.5*pPlate->thick);
			line.endPt.Set(L,-0.5*pPlate->thick);
			edge_arr.Add(LineToArc(line));
			//上包板正视图
			line.startPt.Set(M+A,R);
			line.endPt.Set(M+A+S,R);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A,N+R);
			line.endPt.Set(M+A+S,N+R);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A+S,R);
			line.endPt.Set(M+A+S,N+R);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A,N+R);
			line.endPt.Set(M+A,R);
			edge_arr.Add(LineToArc(line));
			//下包板正视图
			line.startPt.Set(M+A,-R);
			line.endPt.Set(M+A+S,-R);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A,-N-R);
			line.endPt.Set(M+A+S,-N-R);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A+S,-R);
			line.endPt.Set(M+A+S,-N-R);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A,-N-R);
			line.endPt.Set(M+A,-R);
			edge_arr.Add(LineToArc(line));
			//上包板侧视图
			line.startPt.Set(M+A,0.5*pPlate->thick);
			line.endPt.Set(M+A+S,0.5*pPlate->thick);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A,0.5*pPlate->thick+W);
			line.endPt.Set(M+A+S,0.5*pPlate->thick+W);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A,0.5*pPlate->thick);
			line.endPt.Set(M+A,0.5*pPlate->thick+W);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A+S,0.5*pPlate->thick);
			line.endPt.Set(M+A+S,0.5*pPlate->thick+W);
			edge_arr.Add(LineToArc(line));
			//下包板侧视图
			line.startPt.Set(M+A,-0.5*pPlate->thick);
			line.endPt.Set(M+A+S,-0.5*pPlate->thick);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A,-0.5*pPlate->thick-W);
			line.endPt.Set(M+A+S,-0.5*pPlate->thick-W);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A,-0.5*pPlate->thick);
			line.endPt.Set(M+A,-0.5*pPlate->thick-W);
			edge_arr.Add(LineToArc(line));
			line.startPt.Set(M+A+S,-0.5*pPlate->thick);
			line.endPt.Set(M+A+S,-0.5*pPlate->thick-W);
			edge_arr.Add(LineToArc(line));
		}
		//绘制螺栓孔或图符
		ATOM_LIST<double>row_y_arr,column_x_arr;
		for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			if(pPlate->m_iParamType==TYPE_XEND&&fabs(pLsRef->GetLsPtr()->get_norm()*pPlate->ucs.axis_z)<EPS_COS2)
				continue;	//螺栓不在十字型插板基板上
			ls_cir.centre=pLsRef->GetLsPtr()->ucs.origin;
			coord_trans(ls_cir.centre,pPlate->ucs,FALSE);
			ls_cir.centre.z=0;
			//if(pPlate->m_iParamType==TYPE_XEND&&ls_cir.centre.x>L)
			//	continue;	//螺栓不在十字型插板基板上
			ls_cir.radius = (*pLsRef)->get_d()/2.0;
			ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
			cir_arr.Add(ls_cir);
			if(!sys.part_map.plate.bDimCoord)
			{	//不标注坐标时才需要分析螺栓射线
				for(int j=1;j<=32;j++)
				{
					if(pLsRef->dwRayNo&GetSingleWord(j))
						AnalizeLsRay(ls_cir.centre,j);
				}
			}
			double *pPosX;
			for(pPosX=column_x_arr.GetFirst();pPosX;pPosX=column_x_arr.GetNext())
			{
				if(fabs(*pPosX-ls_cir.centre.x)<EPS2)
					break;
				else if(*pPosX>ls_cir.centre.x)	//升序排列
				{
					column_x_arr.insert(ls_cir.centre.x);
					break;
				}
			}
			if(pPosX==NULL)
				column_x_arr.append(ls_cir.centre.x);
			double *pPosY;
			for(pPosY=row_y_arr.GetFirst();pPosY;pPosY=row_y_arr.GetNext())
			{
				if(fabs(*pPosY-ls_cir.centre.y)<EPS2)
					break;
				else if(*pPosY<ls_cir.centre.y)	//降序排列
				{
					row_y_arr.insert(ls_cir.centre.y);
					break;
				}
			}
			if(pPosY==NULL)
				row_y_arr.append(ls_cir.centre.y);
		}
		//尺寸标注
		ROTATEDIM_STRU rotate_dim;
		rotate_dim.bDisplayDimLine=TRUE;
		rotate_dim.dist=L;
		rotate_dim.startPos.Set(0,0.5*H+5);
		rotate_dim.endPos.Set(L,0.5*H+5);
		rotate_dim.dimPos.Set(0.5*L,0.5*H+35);
		rotate_dim.angle=0;
		rotate_dim_arr.Add(rotate_dim);
		rotate_dim.endPos.x=M;
		rotate_dim.dist=M;
		rotate_dim.dimPos.Set(0.5*M,0.5*H+10);
		rotate_dim_arr.Add(rotate_dim);
		if(pPlate->m_iParamType==TYPE_XEND)
		{
			rotate_dim.startPos.x=M;
			rotate_dim.endPos.x=M+A;
			rotate_dim.dist=A;
			rotate_dim.dimPos.Set(0.5*M,0.5*H+10);
			rotate_dim_arr.Add(rotate_dim);
		}
		//--VV--螺栓列距标注区
		int i;
		for(i=0;i<column_x_arr.GetNodeNum();i++)
		{
			rotate_dim.startPos.x=rotate_dim.endPos.x;
			rotate_dim.endPos.x=column_x_arr[i];
			rotate_dim.dist=fabs(rotate_dim.endPos.x-rotate_dim.startPos.x);
			rotate_dim.dimPos.Set(0.5*(rotate_dim.endPos.x+rotate_dim.startPos.x),0.5*H+10);
			rotate_dim_arr.Add(rotate_dim);
		}
		rotate_dim.startPos.x=rotate_dim.endPos.x;
		if(pPlate->m_iParamType!=TYPE_XEND)
			rotate_dim.endPos.x=L-N;
		else
			rotate_dim.endPos.x=M+A+S;
		rotate_dim.dist=fabs(rotate_dim.endPos.x-rotate_dim.startPos.x);
		if(rotate_dim.dist>EPS2)
		{
			rotate_dim.dimPos.Set(0.5*(rotate_dim.endPos.x+rotate_dim.startPos.x),0.5*H+10);
			rotate_dim_arr.Add(rotate_dim);
		}
		//--^^--螺栓列距标注区
		if(pPlate->m_iParamType!=TYPE_XEND)
		{
			rotate_dim.startPos.x=L-N;
			rotate_dim.endPos.x=L;
			rotate_dim.dimPos.x=L-0.5*N;
			rotate_dim.dist=N;
			rotate_dim_arr.Add(rotate_dim);
		}
		if(pPlate->m_iParamType==TYPE_XEND)
		{	//R
			rotate_dim.startPos.Set(M+A+S+5,0);
			rotate_dim.endPos.Set(M+A+S+5,R);
			rotate_dim.dimPos.Set(M+A+S+30,0);
			rotate_dim.dist=R;
			rotate_dim.angle=Pi/2;
			rotate_dim_arr.Add(rotate_dim);
			//N
			rotate_dim.startPos.Set(M+A+S+5,R);
			rotate_dim.endPos.Set(M+A+S+5,R+N);
			rotate_dim.dimPos.Set(M+A+S+30,0);
			rotate_dim.dist=N;
			rotate_dim.angle=Pi/2;
			rotate_dim_arr.Add(rotate_dim);
			//H
			rotate_dim.startPos.Set(M+A+S+5,0.5*H);
			rotate_dim.endPos.Set(M+A+S+5,-0.5*H);
			rotate_dim.dimPos.Set(M+A+S+90,0);
		}
		else
		{	//H
			rotate_dim.startPos.Set(L+5,0.5*H);
			rotate_dim.endPos.Set(L+5,-0.5*H);
			rotate_dim.dimPos.Set(L+60,0);
		}
		rotate_dim.dist=H;
		rotate_dim.angle=Pi/2;
		rotate_dim_arr.Add(rotate_dim);
		if(pPlate->m_iParamType!=TYPE_XEND)
		{
			rotate_dim.endPos.y=rotate_dim.startPos.y-S;
			rotate_dim.dimPos.x-=30;
			rotate_dim.dimPos.y=0.5*H-0.5*S;
			rotate_dim.dist=S;
			rotate_dim_arr.Add(rotate_dim);
		}
		else
			rotate_dim.dimPos.x-=30;
		//--VV--螺栓行距标注区
		if(row_y_arr.GetNodeNum()>0)
			rotate_dim.endPos.y=row_y_arr[0];
		for(i=1;i<row_y_arr.GetNodeNum();i++)
		{
			rotate_dim.startPos.y=rotate_dim.endPos.y;
			rotate_dim.endPos.y=row_y_arr[i];
			rotate_dim.dist=fabs(rotate_dim.startPos.y-rotate_dim.endPos.y);
			rotate_dim.dimPos.y=0.5*(rotate_dim.endPos.y+rotate_dim.startPos.y);
			rotate_dim_arr.Add(rotate_dim);
		}
		//--^^--螺栓列距标注区
		if(pPlate->m_iParamType==TYPE_XEND)
		{	//S
			rotate_dim.startPos.Set(M+A,-0.5*H-5);
			rotate_dim.endPos.Set(M+A+S,-0.5*H-5);
			rotate_dim.dimPos.Set(M+A+0.5*S,-0.5*H-35);
			rotate_dim.dist=S;
			rotate_dim.angle=-Pi;
			rotate_dim_arr.Add(rotate_dim);
		}
		//侧立剖面放置图绘制
		{
			f3dPoint offset_origin;
			if(pPlate->m_iParamType==TYPE_UEND)
			{
				offset_origin.Set(0,-0.5*H-0.5*W-pPlate->thick-50);
				long_edge_mid.Set(L/2,offset_origin.y-W-2*pPlate->thick-30);
				//上侧外壁
				line.startPt.Set(0.5*W+pPlate->thick+offset_origin.x,0.5*W+pPlate->thick+offset_origin.y);
				line.endPt.Set(L+offset_origin.x,0.5*W+pPlate->thick+offset_origin.y);
				line.pen.style=PS_SOLID;
				edge_arr.Add(LineToArc(line));
				//左侧外壁圆弧
				f3dPoint center(0.5*W+pPlate->thick+offset_origin.x,offset_origin.y);
				arcline.CreateMethod1(center,line.startPt,f3dPoint(0,0,1),Pi);
				offset_origin.pen.style=PS_SOLID;
				edge_arr.Add(arcline);
				//上侧内壁
				line.startPt.y-=pPlate->thick;
				line.endPt.y-=pPlate->thick;
				edge_arr.Add(LineToArc(line));
				//左侧内壁圆弧
				arcline.CreateMethod1(center,line.startPt,f3dPoint(0,0,1),Pi);
				offset_origin.pen.style=PS_SOLID;
				edge_arr.Add(arcline);
				//下侧内壁
				line.startPt.y-=W;
				line.endPt.y-=W;
				edge_arr.Add(LineToArc(line));
				//下侧外壁
				line.startPt.y-=pPlate->thick;
				line.endPt.y-=pPlate->thick;
				edge_arr.Add(LineToArc(line));
				//上侧边右
				line.startPt.Set(L+offset_origin.x,0.5*W+pPlate->thick+offset_origin.y);
				line.endPt.Set(L+offset_origin.x,0.5*W+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				//下侧边右
				line.startPt.Set(L+offset_origin.x,-0.5*W-pPlate->thick+offset_origin.y);
				line.endPt.Set(L+offset_origin.x,-0.5*W+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				//尺寸标注
				rotate_dim.startPos.Set(L+offset_origin.x+1,0.5*W+offset_origin.y);
				rotate_dim.endPos.Set(L+offset_origin.x+1,-0.5*W+offset_origin.y);
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim.dist=W;
				rotate_dim.dimPos.Set(rotate_dim.startPos.x+50,offset_origin.y);
				rotate_dim.angle=Pi/2;
				rotate_dim_arr.Add(rotate_dim);
			}
			else if(pPlate->m_iParamType==TYPE_ROLLEND)
			{
				offset_origin.Set(L+fTubeD+80,0);
				long_edge_mid.Set(L/2,-0.5*H-30);
				double fRollH=0;
				if(W>pPlate->thick)
					fRollH=W-pPlate->thick;
				line.startPt.Set(0+offset_origin.x, 0.5*H);
				line.endPt.Set(fRollH+pPlate->thick+offset_origin.x,0.5*H);	
				edge_arr.Add(LineToArc(line));
				line.endPt.Set(0+offset_origin.x,-0.5*H);
				edge_arr.Add(LineToArc(line));
				line.startPt.Set(0+offset_origin.x,-0.5*H);
				line.endPt.Set(fRollH+pPlate->thick+offset_origin.x,-0.5*H);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.y+=pPlate->thick;
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.x-=fRollH;
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.y+=H-2*pPlate->thick;
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.x+=fRollH;
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.y+=pPlate->thick;
				edge_arr.Add(LineToArc(line));
				//绘制钢管剖面图，以便将来标注偏心距
				f3dPoint perp,offset_vec;
				double dist=0;
				//应根据法线偏移量绘制插板偏移位置，根据插板实际位置计算虽可与实体模型保持一致
				//但与参数描述位置不一致，主要是因为钢管位置变化后插板位置未及时更新
				//因为同一编号插板可能在多个位置出现而此处时取第一个该编号的插板，如要求用户重新设计所有插板不太合理。
				//故此处为保证与参数一致，使用法向偏移量进行绘制 wht 11-07-20
				dist-=(pPlate->thick*0.5+pPlate->m_fNormOffset); 
				/*if(pDatumTube)	
				{
					SnapPerp(&perp,pDatumTube->Start(),pDatumTube->End(),pPlate->ucs.origin,&dist);
					offset_vec = pPlate->ucs.origin-perp;
					if(offset_vec*pPlate->ucs.axis_z<0)
						dist=-dist;
				}
				dist=-pPlate->thick*0.5;*/
				offset_vec.Set(dist);
				arcline.CreateMethod1(offset_origin-offset_vec,offset_origin-offset_vec+f3dPoint(fTubeD*0.5,0,0),f3dPoint(0,0,1),2*Pi);
				edge_arr.Add(arcline);
				arcline.CreateMethod1(offset_origin-offset_vec,offset_origin-offset_vec+f3dPoint(fTubeD*0.5-fTubeThick,0,0),f3dPoint(0,0,1),2*Pi);
				edge_arr.Add(arcline);
				line.startPt.Set(offset_origin.x-dist-H*0.5-20,offset_origin.y);
				line.endPt.Set(offset_origin.x-dist+H*0.5+20,offset_origin.y);
				line.pen.style=PS_DASHDOT;
				edge_arr.Add(LineToArc(line));
				line.startPt.Set(offset_origin.x-dist,offset_origin.y-H*0.5-20);
				line.endPt.Set(offset_origin.x-dist,offset_origin.y+H*0.5+20);
				line.pen.style=PS_DASHDOT;
				edge_arr.Add(LineToArc(line));
				if(W>0)
				{	//尺寸标注
					rotate_dim.startPos.Set(offset_origin.x,0.5*H+5);
					rotate_dim.endPos.Set(W+offset_origin.x,0.5*H+5);
					rotate_dim.dimPos.Set(0.5*W+offset_origin.x,0.5*H+10);
					rotate_dim.dist=W;
					rotate_dim.angle=0;
					rotate_dim_arr.Add(rotate_dim);
				}
				//绘制附加板
				for(ATTACHPART_ITEM *pItem=pPlate->GetFirstAttachPart();pItem;pItem=pPlate->GetNextAttachPart())
				{
					CLDSPart *pAttachPart=pPlate->BelongModel()->FromPartHandle(pItem->h);
					if(pAttachPart==NULL||pAttachPart->GetClassTypeId()!=CLS_PLATE)
						continue;
					CLDSPlate *pTempPlate=(CLDSPlate*)pAttachPart;
					if(pTempPlate->designInfo.m_hBasePart!=pPlate->handle)
						continue;
					double vert_plate_H=0;
					for(PROFILE_VERTEX* pVertex=pTempPlate->vertex_list.GetFirst();pVertex;pVertex=pTempPlate->vertex_list.GetNext())
					{
						if(pVertex->vertex.y>vert_plate_H)
							vert_plate_H=pVertex->vertex.y;
					}
					f3dPoint extend_vec=pTempPlate->ucs.axis_y;
					vector_trans(extend_vec,pPlate->ucs,FALSE);
					f3dPoint axis_y(-extend_vec.z,extend_vec.y);
					f3dPoint norm_y(-axis_y.y,axis_y.x,0);
					f3dPoint datum_point=offset_origin;
					if(axis_y.x>0)
						datum_point=offset_origin+axis_y*pPlate->Thick;
					line.pen.style=PS_SOLID;
					line.startPt=datum_point+norm_y*pPlate->thick*0.5;
					line.endPt=datum_point-norm_y*pPlate->thick*0.5;
					edge_arr.Add(LineToArc(line));
					line.endPt=datum_point+norm_y*pPlate->thick*0.5+axis_y*vert_plate_H;
					edge_arr.Add(LineToArc(line));
					line.startPt=datum_point-norm_y*pPlate->thick*0.5+axis_y*vert_plate_H;
					edge_arr.Add(LineToArc(line));
					line.endPt=datum_point-norm_y*pPlate->thick*0.5;
					edge_arr.Add(LineToArc(line));
				}
			}
			else if(pPlate->m_iParamType==TYPE_XEND)
			{
				ATTACHPART_ITEM *pItem=NULL;
				CLDSPlate *pVertPlate[2]={NULL,NULL};
				BOOL bUpDimAngle=FALSE,bDownDimAngle=FALSE;
				for(pItem=pPlate->GetFirstAttachPart();pItem;pItem=pPlate->GetNextAttachPart())
				{
					CLDSPart *pAttachPart=pPlate->BelongModel()->FromPartHandle(pItem->h);
					if(pAttachPart==NULL)
						continue;
					if(pAttachPart->GetClassTypeId()!=CLS_PLATE)
						continue;
					CLDSPlate *pTempPlate=(CLDSPlate*)pAttachPart;
					if(pTempPlate->designInfo.origin.des_para.hPart!=pPlate->handle)
						continue;
					BOOL bDimAngle=FALSE;
					if(fabs(pTempPlate->ucs.axis_z*pPlate->ucs.axis_z)>EPS2)
						bDimAngle=TRUE;
					f3dPoint plate_pos=pTempPlate->ucs.origin;
					coord_trans(plate_pos,pPlate->ucs,FALSE);
					if(pVertPlate[0]==NULL&&plate_pos.z>0)
					{
						pVertPlate[0]=pTempPlate;
						bUpDimAngle=bDimAngle;
					}
					if(pVertPlate[1]==NULL&&plate_pos.z<0)
					{	
						pVertPlate[1]=pTempPlate;
						bDownDimAngle=bDimAngle;
					}
				}
				offset_origin.Set(M+A+S+fTubeD+120,0);
				long_edge_mid.Set(0.5*L,-0.5*H-30);
				line.startPt.Set(offset_origin.x-0.5*pPlate->thick,0.5*H);
				line.endPt.Set(offset_origin.x+0.5*pPlate->thick,0.5*H);	
				edge_arr.Add(LineToArc(line));
				line.endPt.Set(offset_origin.x-0.5*pPlate->thick,-0.5*H);
				edge_arr.Add(LineToArc(line));
				line.startPt.Set(offset_origin.x+0.5*pPlate->thick,-0.5*H);
				edge_arr.Add(LineToArc(line));
				line.endPt.Set(offset_origin.x+0.5*pPlate->thick,0.5*H);
				edge_arr.Add(LineToArc(line));
				//暂不支持包板用角钢代替的十字插板非90度时的角度标注问题 wht 10-06-30
				if(pVertPlate[0]&&pVertPlate[1])
				{	
					//下侧立板
					f3dPoint extend_vec=pVertPlate[1]->ucs.axis_y;
					vector_trans(extend_vec,pPlate->ucs,FALSE);
					f3dPoint axis_y(-extend_vec.z,extend_vec.y);
					f3dPoint norm_y(-axis_y.y,axis_y.x,0);
					f3dPoint datum_point(offset_origin.x+pPlate->thick*0.5,0,0);
					line.startPt=datum_point+norm_y*pPlate->thick*0.5;
					line.endPt=datum_point-norm_y*pPlate->thick*0.5;
					edge_arr.Add(LineToArc(line));
					line.endPt=datum_point+norm_y*pPlate->thick*0.5+axis_y*(H-pPlate->thick)*0.5;
					edge_arr.Add(LineToArc(line));
					line.startPt=datum_point-norm_y*pPlate->thick*0.5+axis_y*(H-pPlate->thick)*0.5;
					edge_arr.Add(LineToArc(line));
					line.endPt=datum_point-norm_y*pPlate->thick*0.5;
					edge_arr.Add(LineToArc(line));
					if(bDownDimAngle)
					{	//标注下侧立板与基板夹角
						ANGULARDIM_STRU angular_dim;
						angular_dim.vertex=datum_point;
						angular_dim.startPos.Set(datum_point.x,datum_point.y+0.5*H);
						angular_dim.endPos=datum_point+axis_y*0.5*H;
						angular_dim.dimPos=0.5*(angular_dim.startPos+angular_dim.endPos);
						angular_dim_arr.Add(angular_dim);
					}
					//上侧立板
					extend_vec=pVertPlate[0]->ucs.axis_y;
					vector_trans(extend_vec,pPlate->ucs,FALSE);
					axis_y.Set(-extend_vec.z,extend_vec.y);
					norm_y.Set(-axis_y.y,axis_y.x,0);
					datum_point.Set(offset_origin.x-pPlate->thick*0.5,0,0);
					line.startPt=datum_point+norm_y*pPlate->thick*0.5;
					line.endPt=datum_point-norm_y*pPlate->thick*0.5;
					edge_arr.Add(LineToArc(line));
					line.endPt=datum_point+norm_y*pPlate->thick*0.5+axis_y*(H-pPlate->thick)*0.5;
					edge_arr.Add(LineToArc(line));
					line.startPt=datum_point-norm_y*pPlate->thick*0.5+axis_y*(H-pPlate->thick)*0.5;
					edge_arr.Add(LineToArc(line));
					line.endPt=datum_point-norm_y*pPlate->thick*0.5;
					edge_arr.Add(LineToArc(line));
					//
					if(bUpDimAngle)
					{	//标注上侧立板与基板夹角
						ANGULARDIM_STRU angular_dim;
						angular_dim.vertex=datum_point;
						angular_dim.startPos.Set(datum_point.x,datum_point.y+0.5*H);
						angular_dim.endPos=datum_point+axis_y*0.5*H;
						angular_dim.dimPos=0.5*(angular_dim.startPos+angular_dim.endPos);
						angular_dim_arr.Add(angular_dim);
					}
				}
				//绘制钢管剖面图，以便将来标注偏心距
				arcline.CreateMethod1(offset_origin,offset_origin+f3dPoint(fTubeD*0.5,0,0),f3dPoint(0,0,1),2*Pi);
				edge_arr.Add(arcline);
				arcline.CreateMethod1(offset_origin,offset_origin+f3dPoint(fTubeD*0.5-fTubeThick,0,0),f3dPoint(0,0,1),2*Pi);
				edge_arr.Add(arcline);
				line.startPt.Set(offset_origin.x-H*0.5-20,offset_origin.y);
				line.endPt.Set(offset_origin.x+H*0.5+20,offset_origin.y);
				line.pen.style=PS_DASHDOT;
				edge_arr.Add(LineToArc(line));
				line.startPt.Set(offset_origin.x,offset_origin.y-H*0.5-20);
				line.endPt.Set(offset_origin.x,offset_origin.y+H*0.5+20);
				line.pen.style=PS_DASHDOT;
				edge_arr.Add(LineToArc(line));
			}
		}
	}
	if(sys.part_map.plate.bDimPlateRayLs)
		DimLsRay(pPlate,bAdjustPlateUcs);
	//恢复板坐标系
	if(bAdjustPlateUcs)
		pPlate->ucs=old_ucs;
	return TRUE;
}

static void trans_pos(f3dArcLine &line,f3dPoint origin,double x_scale=1.0,double y_scale=1.0)
{
	f3dPoint startPt=line.Start();
	f3dPoint endPt=line.End();
	f3dPoint center=line.Center();
	double radius=line.Radius();
	startPt.x*=x_scale;
	startPt.y*=y_scale;
	endPt.x*=x_scale;
	endPt.y*=y_scale;
	center.x*=x_scale;
	center.y*=y_scale;
	startPt+=origin;
	endPt+=origin;
	center+=origin;
	if(line.SectorAngle()==0)
		line.CreateMethod2(startPt,endPt,line.WorkNorm(),0);
	else if(line.ColumnNorm()==line.WorkNorm())	//圆
		line.CreateMethod1(center,startPt,line.WorkNorm(),line.SectorAngle());
	else //if	//椭圆
		line.CreateEllipse(center,startPt,endPt,line.ColumnNorm(),line.WorkNorm(),radius);
}

BOOL CPlankDrawing::CreateStdFLParamPlate(CLDSParamPlate *pPlate,BOOL bPatternDrawing/*=TRUE*/)
{
	if(pPlate->m_iParamType!=TYPE_FLD&&pPlate->m_iParamType!=TYPE_FLP)
		return FALSE;
	//标准构件 平焊法兰/对焊法兰
	//	1----------0----------1
	//	|		   			  |
	//	2____             ____2
	//		3\	   	     /3
	//		  |5   	   5|			
	//		  4---------4 
	double A=0,B=0,C=0,D=0,H=0,H1=0,N=0,R=0,S=0,K=0,L=0;
	CLDSStdPartModel stdPartModel;
	if(!pPlate->GetStdPartModel(&stdPartModel))
		return FALSE;
	A=stdPartModel.param.fl_param.A;
	B=stdPartModel.param.fl_param.B;
	C=stdPartModel.param.fl_param.C;
	D=stdPartModel.param.fl_param.D;
	H=stdPartModel.param.fl_param.H;
	H1=stdPartModel.param.fl_param.H1;
	if(pPlate->m_iParamType==TYPE_FLP)
		pPlate->GetDesignItemValue('O',&H1);
	N=stdPartModel.param.fl_param.N;
	R=stdPartModel.param.fl_param.R;
	S=stdPartModel.param.fl_param.S;
	if(pPlate->m_iParamType==TYPE_FLD)
		S+=stdPartModel.m_fRelateTubeThick;
	K=stdPartModel.param.fl_param.K;
	L=stdPartModel.param.fl_param.L;
	//
	f3dLine line;
	f3dArcLine arcline;
	ROTATEDIM_STRU rotate_dim;
	double T=pPlate->thick;
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||sys.part_map.plate.bDimCoord)
		bAdjustPlateUcs=TRUE;
	if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
	{	//侧立剖面放置图绘制
		if(pPlate->m_iParamType==TYPE_FLP)
		{
			line.startPt.Set(-0.5*B,0);
			line.endPt.Set(-0.5*D,0);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(-0.5*D,C);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(-0.5*B-R-S,C);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(line.endPt.x+R,line.endPt.y+R);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(line.endPt.x,H);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(-0.5*B,line.endPt.y);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(-0.5*B,0);
			edge_arr.Add(LineToArc(line));
			
			line.startPt.Set(-0.5*B,0);
			line.endPt.Set(0.5*B,0);
			edge_arr.Add(LineToArc(line));
			
			line.startPt.Set(0.5*B,0);
			line.endPt.Set(0.5*D,0);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(0.5*D,C);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(0.5*B+R+S,C);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(line.endPt.x-R,line.endPt.y+R);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(line.endPt.x,H);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(0.5*B,line.endPt.y);
			edge_arr.Add(LineToArc(line));
			line.startPt = line.endPt;
			line.endPt.Set(0.5*B,0);
			edge_arr.Add(LineToArc(line));
		}
		else 
		{	//对焊法兰
			f3dPoint vertex_arr[9];
			vertex_arr[0].Set(-0.5*B,0);
			vertex_arr[1].Set(-0.5*D,0);
			vertex_arr[2].Set(-0.5*D,C);
			vertex_arr[3].Set(-0.5*N,C);
			vertex_arr[6].Set(-0.5*B-S,H-H1);
			vertex_arr[7].Set(-0.5*B-S,H);
			vertex_arr[8].Set(-0.5*B,H);
			//求两圆公切线
			f2dPoint pick1,pick2;
			f2dCircle cir1,cir2;
			cir1.centre.Set(-0.5*N,C+R);
			cir1.radius=R;
			pick1.Set(-0.5*N,C);
			cir2.centre.Set(-0.5*B-S-R,H-H1);
			cir2.radius=R;
			pick2.Set(-0.5*B-S,H-H1);
			f2dLine tan_line;
			int nRetCode=TangLine2dcc(cir1,pick1,cir2,pick2,tan_line);
			if(nRetCode==1)
			{
				vertex_arr[4].Set(tan_line.startPt.x,tan_line.startPt.y);
				vertex_arr[5].Set(tan_line.endPt.x,tan_line.endPt.y);
			}
			else 
			{
				vertex_arr[4].Set(-0.5*N,C);
				vertex_arr[5].Set(-0.5*B-S,H-H1);
			}
			f3dArcLine arcLine;
			for(int i=0;i<9;i++)
			{
				line.startPt=vertex_arr[i];
				line.endPt=vertex_arr[(i+1)%9];
				f3dPoint center;
				if(i==3||i==5)
				{
					if(i==3)
						center.Set(-0.5*N,C+R,0);
					else 
						center.Set(-0.5*B-S,H-H1);
					arcLine.CreateMethod3(line.startPt,line.endPt,f3dPoint(0,0,1),R,center);
					edge_arr.Add(arcLine);
				}
				else
					edge_arr.Add(LineToArc(line));
				line.startPt.x *= -1;
				line.endPt.x *= -1;
				center.x *= -1;
				if(i==3||i==5)
				{
					arcLine.CreateMethod3(line.startPt,line.endPt,f3dPoint(0,0,-1),R,center);
					edge_arr.Add(arcLine);
				}
				else
					edge_arr.Add(LineToArc(line));
			}
			line.startPt = vertex_arr[0];
			line.endPt = line.startPt*-1;
			edge_arr.Add(LineToArc(line));
		}
		//标注
		double zoomCoef=sys.general_set.PartMapScale.fRealToDraw;
		rotate_dim.startPos.Set(-0.5*D-1,C);
		if (pPlate->m_iParamType==TYPE_FLD)
			rotate_dim.endPos.Set(-0.5*A-1,C+R);
		else if(pPlate->m_iParamType==TYPE_FLP)
			rotate_dim.endPos.Set(-0.5*B-S-1,C+R);
		rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
		rotate_dim.dimPos.x-=sys.dim_map.fDimTextSize*1.5*zoomCoef+(0.5*D+rotate_dim.dimPos.x);
		rotate_dim.dist = R;
		rotate_dim.angle=-Pi*0.5;
		rotate_dim.bDisplayDimLine=TRUE;
		rotate_dim.bCanTextOutter=TRUE;
		rotate_dim_arr.Add(rotate_dim);
		rotate_dim.bCanTextOutter=FALSE;
		
		for(int i = 1;i<=2;i++)
		{
			double fdistance = 0;
			if(i==1)
				fdistance = B;
			else if(i==2)
				fdistance = D;
			rotate_dim.startPos.Set(-fdistance*0.5,-1);
			rotate_dim.endPos.Set(fdistance*0.5,-1);
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dimPos.y-=sys.dim_map.fDimTextSize*1.5*zoomCoef*i;
			rotate_dim.dist = fdistance;
			rotate_dim.angle=0;
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.bCanTextOutter=TRUE;
			rotate_dim_arr.Add(rotate_dim);
			rotate_dim.bCanTextOutter=FALSE;
		}
		for(i = 1;i<=2;i++)
		{
			double fdistance = 0;
			if(i==1)
			{
				fdistance = N;
				rotate_dim.startPos.Set(-fdistance*0.5,C+1);
				rotate_dim.endPos.Set(fdistance*0.5,C+1);
			}
			else if(i==2&&pPlate->m_iParamType==TYPE_FLD)
			{
				fdistance = A;
				rotate_dim.startPos.Set(-fdistance*0.5,H+1);
				rotate_dim.endPos.Set(fdistance*0.5,H+1);
			}
			else if(i==2&&pPlate->m_iParamType==TYPE_FLP)
			{
				fdistance = S;
				rotate_dim.startPos.Set(-fdistance-0.5*B,H+1);
				rotate_dim.endPos.Set(-0.5*B,H+1);
			}
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dimPos.y+=sys.dim_map.fDimTextSize*1.5*zoomCoef*i+(H-rotate_dim.dimPos.y);
			rotate_dim.dist = fdistance;
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.bCanTextOutter=TRUE;
			rotate_dim_arr.Add(rotate_dim);
			rotate_dim.bCanTextOutter=FALSE;
		}
		for(i = 1;i<=3;i++)
		{
			double fdistance = 0;
			if(i==1 && pPlate->m_iParamType==TYPE_FLD)
			{
				rotate_dim.startPos.Set(0.5*A+1,H-H1);
				rotate_dim.endPos.Set(0.5*A+1,H);
				fdistance = H1;
			}
			else if (i==1&&pPlate->m_iParamType!=TYPE_FLD)
				continue;
			else if(i==2)
			{
				rotate_dim.startPos.Set(0.5*D+1,0);
				rotate_dim.endPos.Set(0.5*D+1,C);
				fdistance = C;
			}
			else if(i==3)
			{
				rotate_dim.startPos.Set(0.5*D+1,0);
				if (pPlate->m_iParamType==TYPE_FLD)
					rotate_dim.endPos.Set(0.5*A+1,H);
				else
					rotate_dim.endPos.Set(0.5*B+S+1,H);
				fdistance = H;
			}
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dimPos.x+=sys.dim_map.fDimTextSize*1.5*zoomCoef*i+(0.5*D-rotate_dim.dimPos.x);
			rotate_dim.dist = fdistance;
			rotate_dim.angle=-Pi/2;
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.bCanTextOutter=TRUE;
			rotate_dim_arr.Add(rotate_dim);
			rotate_dim.bCanTextOutter=FALSE;
		}
	}
	DIAMETERDIM_STRU diam_dim;
	f3dPoint offset_origin(0,-0.5*D-90);	//具体值由经验确定
	//标注坐标或者出大样图时不需要偏移值 wht 10-01-25
	if(bAdjustPlateUcs||bPatternDrawing||m_bSpecifyUCS)
		offset_origin.Set();
	long_edge_mid.Set(0,offset_origin.y-0.5*D-30);
	if(m_bSpecifyUCS)
	{	//指定绘图坐标系时构件信息标注位置需转换到new_ucs下 wht 11-07-07
		coord_trans(long_edge_mid,pPlate->ucs,TRUE);
		coord_trans(long_edge_mid,new_ucs,FALSE);
	}
	//绘制法兰平面图
	f3dPoint center=offset_origin;
	f3dPoint start_pt=offset_origin+f3dPoint(0.5*B,0,0);
	f3dPoint work_norm(0,0,1);
	if(m_bSpecifyUCS)
	{	//指定绘图坐标系
		coord_trans(center,pPlate->ucs,TRUE);
		coord_trans(center,new_ucs,FALSE);
		coord_trans(start_pt,pPlate->ucs,TRUE);
		coord_trans(start_pt,new_ucs,FALSE);
		vector_trans(work_norm,pPlate->ucs,TRUE);
		vector_trans(work_norm,new_ucs,FALSE);
	}
	arcline.CreateMethod1(center,start_pt,work_norm,2*Pi);
	edge_arr.Add(arcline);
	arcline.feature=0;
	center=offset_origin;
	start_pt=offset_origin+f3dPoint(0.5*D,0,0);
	if(m_bSpecifyUCS)
	{	//指定绘图坐标系
		coord_trans(center,pPlate->ucs,TRUE);
		coord_trans(center,new_ucs,FALSE);
		coord_trans(start_pt,pPlate->ucs,TRUE);
		coord_trans(start_pt,new_ucs,FALSE);
	}
	arcline.CreateMethod1(center,start_pt,work_norm,2*Pi);
	arcline.feature=pPlate->handle;
	edge_arr.Add(arcline);
	arcline.feature=0;

	//绘制螺栓孔或图符
	double R_bolt=0,new_R=0;
	f3dCircle ls_cir;
	int nBolt=pPlate->GetLsCount();
	diam_dim.dist=0;
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		ls_cir.centre=pLsRef->GetLsPtr()->ucs.origin;
		coord_trans(ls_cir.centre,pPlate->ucs,FALSE);
		//计算半径前Z坐标应归零 wht 10-10-14
		ls_cir.centre.z=0;
		diam_dim.chordPos=ls_cir.centre;
		new_R=ls_cir.centre.mod();
		ls_cir.centre+=offset_origin;
		ls_cir.radius = (*pLsRef)->get_d()/2.0;
		ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
			DimCoordValue(pPlate,ls_cir.centre);
		if((int)pLsRef->waistLen>0)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width=(int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度
			ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
			vector_trans(ls_cir.norm,pPlate->ucs,FALSE);//转换到钢板坐标系下
		}
		if(m_bSpecifyUCS)
		{	//指定绘图坐标系
			coord_trans(ls_cir.centre,pPlate->ucs,TRUE);
			coord_trans(ls_cir.centre,new_ucs,FALSE);
		}
		cir_arr.Add(ls_cir);
		if(!m_bSpecifyUCS&&!sys.part_map.plate.bDimCoord)
		{
			//绘制螺栓布置圆弧
			if(R_bolt<=0)
			{	//设置初始螺栓距轴心距离，并绘制螺栓布置圆弧
				R_bolt=new_R;
				arcline.CreateMethod1(offset_origin,offset_origin+f3dPoint(R_bolt),f3dPoint(0,0,1),2*Pi);
				arcline.pen.style=PS_DASHDOT;
				edge_arr.Add(arcline);
				//螺栓布置圆直径尺寸标注
				diam_dim.dist=ftoi(R_bolt*2);
				diam_dim.farChordPos=-diam_dim.chordPos;
				diam_dim.leaderLength=5;
				rotate_point_around_axis(diam_dim.chordPos,-0.5*Pi/nBolt,f3dPoint(0,0,0),f3dPoint(0,0,1));
				rotate_point_around_axis(diam_dim.farChordPos,-0.5*Pi/nBolt,f3dPoint(0,0,0),f3dPoint(0,0,1));
				diam_dim.chordPos+=offset_origin;
				diam_dim.farChordPos+=offset_origin;
				diameter_dim_arr.Add(diam_dim);
			}
			else if(fabs(R_bolt-new_R)>1)
			{	//螺栓距轴心距离发生变化
				R_bolt=ls_cir.centre.mod();
				arcline.CreateMethod1(offset_origin,offset_origin+f3dPoint(R_bolt),f3dPoint(0,0,1),2*Pi);
				arcline.pen.style=PS_DASHDOT;
				edge_arr.Add(arcline);
				//螺栓布置圆直径尺寸标注
				rotate_dim.startPos.Set(-R_bolt+offset_origin.x,offset_origin.y);
				rotate_dim.endPos.Set(R_bolt+offset_origin.x,offset_origin.y);
				rotate_dim.dimPos.y=offset_origin.y-R_bolt-5;;
				rotate_dim.bDiameterStyle=TRUE;
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim.dist=2*R_bolt;
				rotate_dim_arr.Add(rotate_dim);
			}
		}
	}
	if(!m_bSpecifyUCS)
	{	//非指定坐标系时标注直径
		f3dPoint diameter_vec(1,0,0);
		if(diam_dim.dist>0)
		{
			diameter_vec=diam_dim.farChordPos-diam_dim.chordPos;
			normalize(diameter_vec);
			RotateVectorAroundVector(diameter_vec,-2*Pi/nBolt,f3dPoint(0,0,1));
		}
		diam_dim.chordPos=offset_origin+diameter_vec*0.5*B;
		diam_dim.farChordPos=offset_origin-diameter_vec*0.5*B;
		diam_dim.dist=2*0.5*B;
		diam_dim.leaderLength=5;
		diameter_dim_arr.Add(diam_dim);
		if(nBolt>0)
			RotateVectorAroundVector(diameter_vec,2*Pi/nBolt,f3dPoint(0,0,1));
		else
			RotateVectorAroundVector(diameter_vec,0.25*Pi,f3dPoint(0,0,1));
		diam_dim.chordPos=offset_origin+diameter_vec*(0.5*D);
		diam_dim.farChordPos=offset_origin-diameter_vec*(0.5*D);
		diam_dim.dist=D;
		diam_dim.leaderLength=5;
		diameter_dim_arr.Add(diam_dim);
	}
	return TRUE;
}

BOOL CPlankDrawing::CreateFLParamPlate(CLDSParamPlate *pPlate,BOOL bPatternDrawing/*=TRUE*/)
{
	if(pPlate->m_iParamType!=TYPE_FL&&pPlate->m_iParamType!=TYPE_FLR&&pPlate->m_iParamType!=TYPE_FLG)
		return FALSE;
	f3dLine line;
	f3dArcLine arcline;
	f3dCircle Cir;
	ANGULARDIM_STRU angular_dim;
	ROTATEDIM_STRU rotate_dim;
	DIAMETERDIM_STRU diam_dim;
	double A=360,B=0,C=0,D=0,E=0,F=0,G=0,H=0,L=0,M=0,N=0,R=0,S=0,W=0;
	ATTACHPART_ITEM *pItem=NULL;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
	//获得法兰的基本设计参数
	pPlate->GetDesignItemValue('A', &A);	//部分历史文件中没有A参数，故A默认为360度 wjh-2012-9-27
	pPlate->GetDesignItemValue('R', &R);
	pPlate->GetDesignItemValue('W', &W);
	pPlate->GetDesignItemValue('H', &H);
	double T=pPlate->thick;
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||sys.part_map.plate.bDimCoord)
		bAdjustPlateUcs=TRUE;
	f3dPoint offset_origin(0,0);
	if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
	{
		//侧立剖面放置图绘制
		line.startPt.Set(-0.5*W,0);
		line.endPt.Set(line.startPt.x+W,line.startPt.y);
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt.Set(line.endPt.x,line.endPt.y+T);
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt.Set(line.endPt.x-W,line.endPt.y);
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt.Set(line.endPt.x,line.endPt.y-T);
		edge_arr.Add(LineToArc(line));
		line.startPt=f3dPoint(-R,T);
		line.endPt.Set(line.startPt.x,line.startPt.y-T);
		edge_arr.Add(LineToArc(line));
		line.startPt.Set(line.startPt.x+R*2,line.startPt.y);
		line.endPt.Set(line.endPt.x+R*2,line.endPt.y);
		edge_arr.Add(LineToArc(line));
		//绘制钢管侧的法兰肘板
		if(pPlate->AttachPart.GetNodeNum()>1)
		{
			CLDSParamPlate *pElbowPlate=NULL;
			for(ATTACHPART_ITEM *pItem=pPlate->AttachPart.GetFirst();pItem;pItem=pPlate->AttachPart.GetNext())
			{
				pElbowPlate=(CLDSParamPlate*)pPlate->BelongModel()->FromPartHandle(pItem->h,CLS_PARAMPLATE);
				if(pElbowPlate&&(pElbowPlate->m_iParamType==TYPE_ELBOWPLATE||pElbowPlate->m_iParamType==TYPE_RIBPLATE))
					break;
			}
			if(pElbowPlate)
			{
				CLDSParamPlate::RIBPLATE_ELBOW elbow;
				elbow.LoadParams(pElbowPlate);
				double A=elbow.A.ToDouble(),B=elbow.B.ToDouble(),C=elbow.C.ToDouble(),D=elbow.D.ToDouble();
				double E=elbow.E.ToDouble(),F=elbow.F.ToDouble(),G=elbow.G.ToDouble(),H=elbow.H.ToDouble();
				//左侧肘板
				line.endPt.Set(-R-E,T);
				line.startPt.Set(-R-B,T);
				line.pen.style=PS_WELDLINE;
				edge_arr.Add(LineToArc(line));
				line.pen.style=PS_SOLID;
				//line.startPt=line.endPt;
				line.endPt.Set(-R-B,T+C);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(-R-D,T+A);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(-R,T+A);
				edge_arr.Add(LineToArc(line));
				line.endPt=line.endPt;
				line.startPt.Set(-R,T+E);
				line.pen.style=PS_WELDLINE;
				edge_arr.Add(LineToArc(line));
				line.pen.style=PS_SOLID;
				//右侧肘板
				line.startPt.Set(R+E,T);
				line.endPt.Set(+R+B,T);
				line.pen.style=PS_WELDLINE;
				edge_arr.Add(LineToArc(line));
				line.pen.style=PS_SOLID;
				line.startPt=line.endPt;
				line.endPt.y+=C;
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(R+D,T+A);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(R,T+A);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(R,T+E);
				line.pen.style=PS_WELDLINE;
				edge_arr.Add(LineToArc(line));
				line.pen.style=PS_SOLID;
			}
		}
		//绘制相关联的焊接钢管
		if(pDatumTube)
		{
			double t2=pDatumTube->GetThick();
			f3dPoint axis_z=pDatumTube->End()-pDatumTube->Start();
			vector_trans(axis_z,pPlate->ucs,FALSE);
			if(axis_z.z>=0)
				axis_z.Set(axis_z.x,axis_z.z);
			else
				axis_z.Set(-axis_z.x,-axis_z.z);
			normalize(axis_z);
			double tube_r=max(R,pDatumTube->GetDiameter()*0.5);
			f3dLine breakLine,TubeEndLine;
			line.startPt.Set(-tube_r,+H);
			line.endPt=line.startPt+axis_z*500;
			breakLine.startPt=line.endPt;
			TubeEndLine.startPt=line.startPt;
			edge_arr.Add(LineToArc(line));
			line.pen.style=PS_DASH;
			line.startPt.Set(line.startPt.x+t2/axis_z.y,line.startPt.y);
			line.endPt.Set(line.endPt.x+t2/axis_z.y,line.endPt.y);
			edge_arr.Add(LineToArc(line));
			line.pen.style=PS_SOLID;
			line.startPt=f3dPoint(tube_r,H);
			line.endPt=line.startPt+axis_z*500-axis_z*(2*R*axis_z.x);
			breakLine.endPt=line.endPt;
			TubeEndLine.endPt=line.startPt;
			edge_arr.Add(LineToArc(line));
			edge_arr.Add(LineToArc(TubeEndLine));
			line.pen.style=PS_DASH;
			line.startPt.Set(line.startPt.x-t2/axis_z.y,line.startPt.y);
			line.endPt.Set(line.endPt.x-t2/axis_z.y,line.endPt.y);
			edge_arr.Add(LineToArc(line));
			line.pen.style=PS_DASHDOT;
			line.startPt=-axis_z*100;
			line.endPt=line.startPt+axis_z*700;
			edge_arr.Add(LineToArc(line));
			//绘制钢管破断面
			line.pen.style=PS_TUBEBREAK;
			line.feature=-ftoi(pDatumTube->GetThick());
			edge_arr.Add(LineToArc(breakLine));
		}
		//'H'参数尺寸标注
		rotate_dim.startPos.Set(-R/2,0);
		rotate_dim.endPos.Set(-R/2,H);
		rotate_dim.dimPos.Set(-R/2+8,-H);
		rotate_dim.dist = H;
		rotate_dim.angle=-Pi/2;
		rotate_dim.bDisplayDimLine=TRUE;
		rotate_dim.bCanTextOutter=TRUE;
		rotate_dim_arr.Add(rotate_dim);
		rotate_dim.bCanTextOutter=FALSE;
		offset_origin.Set(0,-800);	//具体值由经验确定
	}

	//水平放置图绘制
	long_edge_mid.Set(0,offset_origin.y-R-30);
	if(m_bSpecifyUCS)
	{	//指定绘图坐标系时构件信息标注位置需转换到new_ucs下 wht 11-07-07
		coord_trans(long_edge_mid,pPlate->ucs,TRUE);
		coord_trans(long_edge_mid,new_ucs,FALSE);
	}
	//绘制法兰平面图
	double arc_angle=A*RADTODEG_COEF;
	if(arc_angle>2*Pi-EPS)	//整圆
	{
		f3dPoint center=offset_origin;
		f3dPoint start_pt=offset_origin+f3dPoint(R,0,0);
		f3dPoint work_norm(0,0,1);
		if(m_bSpecifyUCS)
		{	//指定绘图坐标系
			coord_trans(center,pPlate->ucs,TRUE);
			coord_trans(center,new_ucs,FALSE);
			coord_trans(start_pt,pPlate->ucs,TRUE);
			coord_trans(start_pt,new_ucs,FALSE);
			vector_trans(work_norm,pPlate->ucs,TRUE);
			vector_trans(work_norm,new_ucs,FALSE);
		}
		arcline.CreateMethod1(center,start_pt,work_norm,2*Pi);
		edge_arr.Add(arcline);
		center=offset_origin;
		start_pt=offset_origin+f3dPoint(0.5*W,0,0);
		if(m_bSpecifyUCS)
		{	//指定绘图坐标系
			coord_trans(center,pPlate->ucs,TRUE);
			coord_trans(center,new_ucs,FALSE);
			coord_trans(start_pt,pPlate->ucs,TRUE);
			coord_trans(start_pt,new_ucs,FALSE);
		}
		arcline.CreateMethod1(center,start_pt,work_norm,2*Pi);
		arcline.feature=pPlate->handle;
		edge_arr.Add(arcline);
	}
	else
	{	//由于某些参数化板参数可能被破坏重新设计后将得不到正确的外形 
		//顶点数小于3时才重新设计该参数化板	wht 11-07-06
		if(pPlate->vertex_list.GetNodeNum()<3)
			pPlate->DesignPlate();
		PROFILE_VERTEX *pPrevVertex=NULL;
		BOOL bFinal=FALSE;
		for(PROFILE_VERTEX *pVertex=pPlate->vertex_list.GetFirst();1;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex==NULL)
			{
				pVertex=pPlate->vertex_list.GetFirst();
				bFinal=TRUE;
			}
			if(pVertex==NULL)
				break;
			//标注顶点相对坐标值
			if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
				DimCoordValue(pPlate,pVertex->vertex);
			if(pPrevVertex==NULL)
				line.startPt=pVertex->vertex;
			else
			{
				line.endPt=pVertex->vertex;
				if(bFinal)
					line.feature=pPlate->handle;
				if(pPrevVertex->m_bWeldEdge&&!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
					line.pen.style=PS_WELDLINE;	//零件图才需要绘制焊缝线
				else
					line.pen.style=PS_SOLID;
				if(pPrevVertex->type==0)
				{
					arcline=LineToArc(line);
					trans_pos(arcline,offset_origin);
					if(sys.part_map.plate.bDimPlateEdgeLen)//标注钢板边长
						DimPlateEdgeLen(f3dLine(arcline.Start(),arcline.End()));
					edge_arr.Add(arcline);
				}
				else
				{
					PROFILE_VERTEX profileVertex=*pPrevVertex;
					if(profileVertex.type==1)
						arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),profileVertex.sector_angle);
					else if(profileVertex.type==2)
					{	//圆心拾取点为相对坐标，此处不需要转换 wht 12-06-14
						//coord_trans(profileVertex.center,pPlate->ucs,FALSE);
						double sign=1.0;
						if(profileVertex.radius<0)
							sign=-1.0;
						arcline.CreateMethod3(line.startPt,line.endPt,f3dPoint(0,0,sign),sign*profileVertex.radius,profileVertex.center);
					}
					else if(profileVertex.type==3)
					{	//椭圆圆心拾取点及column_norm为绝对坐标，需要转换到相对坐标系下 wht 12-06-14
						int sign=1;
						if(profileVertex.radius<0)
							sign=-1;
						f3dPoint center=profileVertex.center;
						f3dPoint col_axis=profileVertex.column_norm;
						coord_trans(center,pPlate->ucs,FALSE);
						vector_trans(col_axis,pPlate->ucs,FALSE);
						arcline.CreateEllipse(center,line.startPt,line.endPt,col_axis,f3dPoint(0,0,sign),sign*profileVertex.radius);
						//重新绘制钢板的椭圆弧边
						DrawPlateEllipseLine(pPlate,arcline,FALSE);
					}
					if(profileVertex.m_bWeldEdge&&!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
						arcline.pen.style=PS_WELDLINE;	//零件图才需要绘制焊缝线
					else
						arcline.pen.style=PS_SOLID;
					//标注相对坐标
					DimArcLineCoordValue(pPlate,arcline);
					trans_pos(arcline,offset_origin);
					edge_arr.Add(arcline);
				}
				if(pPrevVertex->m_bWeldEdge)
					ProcessPlateWeldEdge(pPrevVertex,arcline);//处理焊缝边(绘制焊缝线、绘制坡口线、标注焊接件号、标注坡口尺寸等) wht 18-08-09
				line.startPt=line.endPt;
			}
			pPrevVertex=pVertex;
			if(bFinal)
				break;
		}
	}
	arcline.feature=0;
	if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
	{
		if(pDatumTube&&R>0)
		{	//法兰内径大于0时才需要绘制钢管截面图 wht 11-07-08
			arcline.CreateMethod1(offset_origin,offset_origin+f3dPoint(R-pDatumTube->GetThick(),0,0),f3dPoint(0,0,1),2*Pi);
			arcline.feature=pDatumTube->handle;
			edge_arr.Add(arcline);
			arcline.feature=0;
		}
		//绘制法兰上的肘板俯视图
		CLDSStdPartModel stdPartModel;
		if((pPlate->m_iParamType==TYPE_FLR||pPlate->m_iParamType==TYPE_FLG)&&
			pPlate->AttachPart.GetNodeNum()==0&&pPlate->GetStdPartModel(&stdPartModel)
			&&stdPartModel.param.fl_param2.nRibPlateNum>0)
		{	//刚性法兰或柔性法兰,附加钢板数位为0且肋板实际数大于0，该情况为使用标注库文件生成零件图 wht 12-05-21
			double fRibPalteT=stdPartModel.param.fl_param2.para.T;
			double fRibPlateB=stdPartModel.param.fl_param2.para.B;
			f3dPoint extend_vec(1,0,0);
			double start_angle=0.5*(2*Pi)/stdPartModel.param.fl_param2.nBoltNum;
			double piece_angle=(2*Pi)/stdPartModel.param.fl_param2.nRibPlateNum;
			for(int i=0;i<stdPartModel.param.fl_param2.nRibPlateNum;i++)
			{
				f3dPoint axis_x=extend_vec;
				RotateVectorAroundVector(axis_x,start_angle+piece_angle*i,f3dPoint(0,0,1));
				normalize(axis_x);
				axis_x.z=0;
				f3dPoint axis_z(axis_x.y,-axis_x.x);
				line.startPt=offset_origin+axis_x*R-axis_z*(fRibPalteT/2.0);
				line.endPt=line.startPt+axis_x*fRibPlateB;
				line.pen.style=PS_WELDLINE;
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt+=axis_z*fRibPalteT;
				line.pen.style=PS_SOLID;
				line.feature=0;
				edge_arr.Add(LineToArc(line));
				line.feature=0;
				line.startPt=line.endPt;
				line.endPt-=axis_x*fRibPlateB;
				line.pen.style=PS_WELDLINE;
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt-=axis_z*fRibPalteT;
				line.pen.style=PS_SOLID;
				edge_arr.Add(LineToArc(line));	
			}
		}
		else 
		{
			for(pItem=pPlate->AttachPart.GetFirst();pItem;pItem=pPlate->AttachPart.GetNext())
			{
				if(pItem->idClsType!=CLS_PARAMPLATE)
					continue;
				CSuperSmartPtr<CLDSPart> pPart=pPlate->BelongModel()->FromPartHandle(pItem->h,CLS_PARAMPLATE);
				if(pPart.IsNULL()||pPart.ParamPlatePointer()->m_iParamType!=TYPE_ELBOWPLATE)
					continue;
				f3dPoint axis_x=pPart->ucs.axis_x;
				vector_trans(axis_x,pPlate->ucs,FALSE);
				axis_x.z=0;
				f3dPoint axis_z(axis_x.y,-axis_x.x);
				double t2=pPart.ParamPlatePointer()->thick;
				line.startPt=offset_origin+axis_x*R-axis_z*(t2/2.0);
				line.endPt=line.startPt+axis_x*(0.5*W-R);
				line.pen.style=PS_WELDLINE;
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt+=axis_z*t2;
				line.pen.style=PS_SOLID;
				line.feature=pPart->handle;
				edge_arr.Add(LineToArc(line));
				line.feature=0;
				line.startPt=line.endPt;
				line.endPt-=axis_x*(0.5*W-R);
				line.pen.style=PS_WELDLINE;
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt-=axis_z*t2;
				line.pen.style=PS_SOLID;
				edge_arr.Add(LineToArc(line));
			}
		}
	}
	//绘制水平/竖直轴线
	line.startPt.Set(-(0.5*W+30)+offset_origin.x,offset_origin.y);
	line.endPt.Set((0.5*W+30)+offset_origin.x,offset_origin.y);
	line.pen.style=PS_DASHDOT;
	if(!m_bSpecifyUCS&&!sys.part_map.plate.bDimCoord)
		edge_arr.Add(LineToArc(line));
	line.startPt.Set(offset_origin.x,-(0.5*W+30)+offset_origin.y);
	line.endPt.Set(offset_origin.x,(0.5*W+30)+offset_origin.y);
	if(!m_bSpecifyUCS&&!sys.part_map.plate.bDimCoord)
		edge_arr.Add(LineToArc(line));
	//绘制螺栓孔或图符
	double R_bolt=0,new_R=0;
	f3dCircle ls_cir;
	int nBolt=pPlate->GetLsCount();
	diam_dim.dist=0;
	for(CLsRef *pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		ls_cir.centre=pLsRef->GetLsPtr()->ucs.origin;
		coord_trans(ls_cir.centre,pPlate->ucs,FALSE);
		//计算半径前Z坐标应归零 wht 10-10-14
		ls_cir.centre.z=0;
		diam_dim.chordPos=ls_cir.centre;
		new_R=ls_cir.centre.mod();
		ls_cir.centre+=offset_origin;
		ls_cir.radius = (*pLsRef)->get_d()/2.0;
		ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
			DimCoordValue(pPlate,ls_cir.centre);
		if((int)pLsRef->waistLen>0)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width=(int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度
			ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
			vector_trans(ls_cir.norm,pPlate->ucs,FALSE);//转换到钢板坐标系下
		}
		if(m_bSpecifyUCS)
		{	//指定绘图坐标系
			coord_trans(ls_cir.centre,pPlate->ucs,TRUE);
			coord_trans(ls_cir.centre,new_ucs,FALSE);
		}
		cir_arr.Add(ls_cir);
		if(!m_bSpecifyUCS&&!sys.part_map.plate.bDimCoord)
		{
			//绘制螺栓布置圆弧
			if(R_bolt<=0)
			{	//设置初始螺栓距轴心距离，并绘制螺栓布置圆弧
				R_bolt=new_R;
				arcline.CreateMethod1(offset_origin,offset_origin+f3dPoint(R_bolt),f3dPoint(0,0,1),2*Pi);
				arcline.pen.style=PS_DASHDOT;
				edge_arr.Add(arcline);
				//螺栓布置圆直径尺寸标注
				diam_dim.dist=ftoi(R_bolt*2);
				diam_dim.farChordPos=-diam_dim.chordPos;
				diam_dim.leaderLength=5;
				rotate_point_around_axis(diam_dim.chordPos,-0.5*Pi/nBolt,f3dPoint(0,0,0),f3dPoint(0,0,1));
				rotate_point_around_axis(diam_dim.farChordPos,-0.5*Pi/nBolt,f3dPoint(0,0,0),f3dPoint(0,0,1));
				diam_dim.chordPos+=offset_origin;
				diam_dim.farChordPos+=offset_origin;
				diameter_dim_arr.Add(diam_dim);
				/*rotate_dim.startPos.Set(-R_bolt+offset_origin.x,offset_origin.y);
				rotate_dim.endPos.Set(R_bolt+offset_origin.x,offset_origin.y);
				rotate_dim.dimPos.y=offset_origin.y-R_bolt-5;;
				rotate_dim.bDiameterStyle=TRUE;
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim.dist=2*R_bolt;
				rotate_dim_arr.Add(rotate_dim);*/
			}
			else if(fabs(R_bolt-new_R)>1)
			{	//螺栓距轴心距离发生变化
				R_bolt=ls_cir.centre.mod();
				arcline.CreateMethod1(offset_origin,offset_origin+f3dPoint(R_bolt),f3dPoint(0,0,1),2*Pi);
				arcline.pen.style=PS_DASHDOT;
				edge_arr.Add(arcline);
				//螺栓布置圆直径尺寸标注
				rotate_dim.startPos.Set(-R_bolt+offset_origin.x,offset_origin.y);
				rotate_dim.endPos.Set(R_bolt+offset_origin.x,offset_origin.y);
				rotate_dim.dimPos.y=offset_origin.y-R_bolt-5;;
				rotate_dim.bDiameterStyle=TRUE;
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim.dist=2*R_bolt;
				rotate_dim_arr.Add(rotate_dim);
			}
		}
	}
	if(!m_bSpecifyUCS)
	{	//非指定坐标系时标注直径
		f3dPoint diameter_vec(1,0,0);
		if(diam_dim.dist>0)
		{
			diameter_vec=diam_dim.farChordPos-diam_dim.chordPos;
			normalize(diameter_vec);
			RotateVectorAroundVector(diameter_vec,-2*Pi/nBolt,f3dPoint(0,0,1));
		}
		if(R>0)
		{	//内径大于0时才需要进行标注 wht 11-07-08
			diam_dim.chordPos=offset_origin+diameter_vec*R;
			diam_dim.farChordPos=offset_origin-diameter_vec*R;
			diam_dim.dist=2*R;
			diam_dim.leaderLength=5;
			diameter_dim_arr.Add(diam_dim);
		}
		if(nBolt>0)
			RotateVectorAroundVector(diameter_vec,2*Pi/nBolt,f3dPoint(0,0,1));
		else
			RotateVectorAroundVector(diameter_vec,0.25*Pi,f3dPoint(0,0,1));
		diam_dim.chordPos=offset_origin+diameter_vec*(0.5*W);
		diam_dim.farChordPos=offset_origin-diameter_vec*(0.5*W);
		diam_dim.dist=W;
		diam_dim.leaderLength=5;
		diameter_dim_arr.Add(diam_dim);
	}
	//标注螺栓孔
	/*if(pPlate->GetLsCount()>0)
	{
		char sNotes[200]="";
		f3dPoint pos=pPlate->GetFirstLsRef()->GetLsPtr()->ucs.origin;
		coord_trans(pos,pPlate->ucs,FALSE);
		pos.z=0;
		f3dPoint textEnd,textStart,textPos;
		if(pos.x>0)
			textStart.x = pos.x + 120;
		else 
			textStart.x = pos.x - 120;
		//if(pos.y>0)
		//	textStart.y = pos.y + 120;
		//else 
			textStart.y = pos.y - 120;
		if(pos.x>0)
			textEnd = textStart + ucs.axis_x*80;
		else
			textEnd = textStart - ucs.axis_x*80;
		pos/=PART_MAP_SCALE;
		textStart/=PART_MAP_SCALE;
		textEnd/=PART_MAP_SCALE;
		pos+=ucs.origin+offset_ucs.origin;
		textStart+=ucs.origin+offset_ucs.origin;
		textEnd+=ucs.origin+offset_ucs.origin;
		textPos.x = 0.5*(textStart.x+textEnd.x);
		textPos.y = 0.5*(textStart.y+textEnd.y)+0.5;
		CreateAcadLine(pBlockTableRecord,pos,textStart);
		CreateAcadLine(pBlockTableRecord,textStart,textEnd);
		sprintf(sNotes,"%dФ%d孔",pPlate->GetLsCount(),pPlate->GetFirstLsRef()->GetLsPtr()->get_d());
		DimText(pBlockTableRecord,textPos,sNotes,textStyleId,
			sys.dim_map.fPartNoTextSize/PART_MAP_SCALE*4,0,AcDb::kTextCenter,AcDb::kTextBottom);
	}*/
	return TRUE;
}
BOOL CPlankDrawing::CreateElbowParamPlate(CLDSParamPlate *pPlate,BOOL bPatternDrawing/*=TRUE*/)
{
	f3dPoint line_vec;
	f3dLine line;
	f3dArcLine arcLine;
	TXTDIM_STRU txt_dim;
	UCS_STRU old_ucs;
	//设计钢板得到顶点列表
	//由于某些参数化板参数可能被破坏重新设计后将得不到正确的外形 
	//顶点数小于3时才重新设计该参数化板	wht 11-07-06
	if(pPlate->vertex_list.GetNodeNum()<3)
		pPlate->DesignPlate();
	if(pPlate->vertex_list.GetNodeNum()<3)
	{
		CString sPrompt;
#ifdef AFX_TARG_ENU_ENGLISH
		sPrompt.Format("handle is:rib plate parameter of 0X%x is wrong,please confirm!",pPlate->handle);
#else
		sPrompt.Format("句柄为:0X%x的肋板参数不正确，请确认!",pPlate->handle);
#endif
		AfxMessageBox(sPrompt);
		return FALSE;
	}
	//绘制肋板上的孔（螺栓孔+杂项孔）(必须提前在调整坐标系之前，否则有一些杂项孔是按原始定位坐标系定义的 wjh-2019.8.14
	CXhSimpleList<CLDSPart::HOLE> hole_list;
	pPlate->GetHoleSet(&hole_list);
	old_ucs=pPlate->ucs;
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||sys.part_map.plate.bDimCoord)
		bAdjustPlateUcs=TRUE;
	PROFILE_VERTEX *pVertex=NULL;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	//复制顶点链表数据，绘制钢板零件图时使用该顶点链表,因为绘制过程可能修改链表数据 wht 11-07-07
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	if(m_bSpecifyUCS) 
	{	//指定绘图坐标系优先于内部调整坐标系 wht 10-08-13
		bAdjustPlateUcs=TRUE;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,pPlate->ucs,TRUE);
			geom_center+=pVertex->vertex; //
			coord_trans(pVertex->vertex,new_ucs,FALSE);
			if(pVertex->type==1||pVertex->type==2)
			{	//转换圆心拾取点
				coord_trans(pVertex->center,pPlate->ucs,TRUE);
				coord_trans(pVertex->center,new_ucs,FALSE);
			}
		}
		//计算几何中心
		geom_center/=vertex_list.GetNodeNum();
		project_point(geom_center,pPlate->ucs.origin,pPlate->ucs.axis_z);
		//设置钢板坐标系为调整后的坐标系，最后会恢复为原有坐标系 wht 11-07-07
		pPlate->ucs=new_ucs;
	}
	else if(bAdjustPlateUcs)	//标注坐标
	{	//对于参数化板可以直接修改顶点列表中的坐标值，因为每次使用参数板时重新设计就可以生成新的顶点列表
		UCS_STRU ucs;
		if(AdjustPlateUcs(pPlate,ucs,geom_center)>EPS2)
		{
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				coord_trans(pVertex->vertex,pPlate->ucs,TRUE);
				coord_trans(pVertex->vertex,ucs,FALSE);
				if(pVertex->type==1||pVertex->type==2)
				{	//转换圆心拾取点
					coord_trans(pVertex->center,pPlate->ucs,TRUE);
					coord_trans(pVertex->center,ucs,FALSE);
				}
			}
			old_ucs=pPlate->ucs; //存储板坐标系 计算孔位置时用
			new_ucs=ucs;	//记录板坐标系
			pPlate->ucs=ucs;		//调整板坐标系 
		}
		else
		{	
			old_ucs=pPlate->ucs;
			new_ucs=pPlate->ucs;
		}
	}
	else
	{
		new_ucs=pPlate->ucs;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pPlate->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=vertex_list.GetNodeNum();
		project_point(geom_center,pPlate->ucs.origin,pPlate->ucs.axis_z);
	}
	double max_edge_len=0;
	f3dPoint base_face_pick;	//基准面拾取点
	BOOL bZhiWanPlate=pPlate->IsZhiWan();
	PROFILE_VERTEX *pPrevVertex=vertex_list.GetTail();
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(bZhiWanPlate)
		{
			if(pVertex->vertex.feature==1)
				base_face_pick=pVertex->vertex;
		}
		//标注顶点相对坐标值
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
			DimCoordValue(pPlate,pVertex->vertex);
		double edge_len=0;
		if(pPrevVertex)
		{
			if(pPrevVertex->type>0)
			{
				int sign=1;
				if(pPrevVertex->type==1)
				{	//圆弧
					if(pPrevVertex->sector_angle<0)
						sign=-1;
					arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,sign),sign*pPrevVertex->sector_angle);
				}
				else if(pPrevVertex->type==2)	//指定圆弧R
				{	//圆心拾取点为相对坐标，此处不需要转换 wht 12-06-14
					f3dPoint center_pick=pPrevVertex->center;
					//coord_trans(center_pick,pPlate->ucs,FALSE);
					arcLine.CreateMethod3(pPrevVertex->vertex,pVertex->vertex,f3dPoint(0,0,1),pPrevVertex->radius,center_pick);
				}
				else if(pPrevVertex->type==3)	//椭圆弧
				{
					f3dPoint center = pPrevVertex->center;
					f3dPoint col_axis=pPrevVertex->column_norm;
					coord_trans(center,pPlate->ucs,FALSE);
					vector_trans(col_axis,pPlate->ucs,FALSE);
					if(pPrevVertex->radius<0)
						sign=-1;
					arcLine.CreateEllipse(center,pPrevVertex->vertex,pVertex->vertex,col_axis,f3dPoint(0,0,sign),sign*pPrevVertex->radius);
					//重新绘制钢板的椭圆弧边
					DrawPlateEllipseLine(pPlate,arcLine,FALSE);
				}
				arcLine.pen.style=PS_SOLID;
				edge_arr.Add(arcLine);
				edge_len=DISTANCE(arcLine.Start(),arcLine.End());
				if(max_edge_len<edge_len)
				{
					max_edge_len=edge_len;
					long_edge_mid=0.5*(arcLine.Start()+arcLine.End());
					long_edge_angle=Cal2dLineAng(arcLine.Start().x,arcLine.Start().y,arcLine.End().x,arcLine.End().y);
				}
			}
			else
			{	//直线
				line.startPt=pPrevVertex->vertex;
				line.endPt=pVertex->vertex;
				line.pen.style=PS_SOLID;
				arcLine=LineToArc(line);
				edge_arr.Add(arcLine);
				if(sys.part_map.plate.bDimPlateEdgeLen)//标注钢板边长
					DimPlateEdgeLen(line);
				edge_len=DISTANCE(line.startPt,line.endPt);
				if(max_edge_len<edge_len)
				{
					max_edge_len=edge_len;
					long_edge_mid=0.5*(line.startPt+line.endPt);
					long_edge_angle=Cal2dLineAng(line.startPt.x,line.startPt.y,line.endPt.x,line.endPt.y);
				}
			}
			if(pPrevVertex->m_bWeldEdge)
				ProcessPlateWeldEdge(pPrevVertex,arcLine);//处理焊缝边(绘制焊缝线、绘制坡口线、标注焊接件号、标注坡口尺寸等) wht 18-08-09
		}
		pPrevVertex=pVertex;
	}
	if(bZhiWanPlate)
	{	//绘制火曲线 wht 11-05-19
		f3dLine huoquline;
		bool bZhiWanStart=false,bZhiWanEnd=false;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.feature<=10)
				continue;
			if(!bZhiWanStart)
			{
				huoquline.startPt=pVertex->vertex;
				bZhiWanStart=true;
			}
			else if(!bZhiWanEnd)
			{
				huoquline.endPt=pVertex->vertex;
				bZhiWanEnd=true;
			}
		}
		if(bZhiWanStart&&bZhiWanEnd)
		{
			huoquline.pen.style=PS_DASHDOTDOT;
			edge_arr.Add(LineToArc(huoquline));
		}
		//
		f3dPoint prj_base_pick,base_vec;
		SnapPerp(&prj_base_pick,huoquline,base_face_pick);
		base_vec=base_face_pick-prj_base_pick;
		normalize(base_vec);
		f3dPoint norm = pPlate->HuoquFaceNorm();
		vector_trans(norm,pPlate->ucs,FALSE);
		BOOL bNeiQu=TRUE;
		double dd = cal_angle_of_2vec(base_vec,norm);
		if(dd<Pi/2||(fabs(dd-Pi/2)<EPS&&norm.z>0))	//火曲面向外曲(正曲)
			bNeiQu=FALSE;
		m_ciHuoQuType=bNeiQu?1:2;
		//标注钢板火曲工艺字符串 wht 11-05-19
		TXTDIM_STRU txt_dim;
		txt_dim.txtHeight=sys.fonts.plate.fDimTextSize;	//火曲工艺内容字体高度独立设置
		txt_dim.dimPos=0.5*(huoquline.startPt+huoquline.endPt);
		txt_dim.dimVec=huoquline.endPt-huoquline.startPt;
		double huoquangle=cal_angle_of_2vec(pPlate->HuoquFaceNorm(),pPlate->ucs.axis_z);
		GetPlateHuoQuProcessStr(txt_dim,bNeiQu,huoquangle*DEGTORAD_COEF,pPlate->m_fDeformCoef,pPlate->GetThick());
		double fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
		normalize(txt_dim.dimVec);
		txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
		text_dim_arr.Add(txt_dim);
	}
	f3dCircle ls_cir;
	for(CLDSPart::HOLE* pHole=hole_list.EnumObjectFirst();pHole;pHole=hole_list.EnumObjectNext())
	{
		ls_cir.centre=pHole->xLocalPosition;
		ls_cir.centre.z=0;
		if(pHole->pBolt)
		{
			CLsRef* pLsRef=pPlate->FindLsByHandle(pHole->pBolt->handle);
			ls_cir.radius=pHole->pBolt->get_d()*0.5;
			ls_cir.feature=ftoi(pHole->pBolt->hole_d_increment*10);
			ls_cir.pen.style=pHole->pBolt->FilterFuncType();	//记录螺栓孔的功用类型
			ls_cir.centre.pen.style=pLsRef->cFlag;				//记录螺栓孔的工艺类型
		}
		else
		{
			ls_cir.radius = pHole->wBoltD*0.5;
			ls_cir.feature=ftoi(pHole->fHoleMargin*10);
			ls_cir.pen.style=CLDSBolt::FUNC_WATERHOLE;			//杂项孔的功用类型默认为镀锌引流孔
		}
		if(bAdjustPlateUcs)
		{	//标注相对坐标	带自动正位的肘板在标注螺栓相对位置时有问题... wht 09-09-18
			coord_trans(ls_cir.centre,old_ucs,TRUE);
			coord_trans(ls_cir.centre,pPlate->ucs,FALSE);
		}
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
			DimCoordValue(pPlate,ls_cir.centre);
		cir_arr.Add(ls_cir);
	}
	if(bPatternDrawing)//大样图中需要在焊烧死角处切弧
		CreateLineOnWeldCorner(pPlate,&vertex_list);
	//大样图中绘制钢板的内侧轮廓边
	if(sys.part_map.plate.bDrawInnerEdge)
		DrawInnerEdge(pPlate,&vertex_list);
	//青岛东方要求大样图中不绘制焊接子构件
	if(!(gxDrwLocalizer.ciCustomerSerial==5&&bPatternDrawing))
		DrawWeldSubPlate(pPlate,bAdjustPlateUcs);
	//恢复板坐标系
	if(bAdjustPlateUcs)
		pPlate->ucs=old_ucs;
	return TRUE;
}
BOOL CPlankDrawing::CreateCircularParamPlate(CLDSParamPlate *pPlate,BOOL bPatternDrawing/*=TRUE*/)
{
	if(pPlate->m_iParamType!=TYPE_CIRRIBPLATE&&pPlate->m_iParamType!=TYPE_CIRCOVERPLATE&&
		pPlate->m_iParamType!=TYPE_TUBERIBPLATE)
		return FALSE;
	f3dPoint line_vec;
	f3dLine line,line_up,line_down;
	f3dArcLine arcline;
	f3dCircle Cir;
	TXTDIM_STRU txt_dim;
	DIAMETERDIM_STRU diam_dim;	//半\直径标注
	ANGULARDIM_STRU angular_dim;
	ROTATEDIM_STRU rotate_dim;
	double A=0,B=0,C=0,D=0,F=0,G=0,H=0,L=0,M=0,N=0,S=0,W=0,V=0;
	pPlate->GetDesignItemValue('D', &D);
	pPlate->GetDesignItemValue('W', &W);
	pPlate->GetDesignItemValue('N', &N);
	pPlate->GetDesignItemValue('H',	&H);
	pPlate->GetDesignItemValue('V',	&V);
	//由于某些参数化板参数可能被破坏重新设计后将得不到正确的外形 
	//顶点数小于3时才重新设计该参数化板	wht 11-07-06
	if(pPlate->vertex_list.GetNodeNum()<3)
		pPlate->DesignPlate();	
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent);
	if(pDatumTube==NULL)
		return FALSE;
	if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{
		pPlate->GetDesignItemValue('A', &A);
		pPlate->GetDesignItemValue('B', &B);
	}
	BOOL bZhiWanPlate=pPlate->IsZhiWan();	//制弯钢板 wht 11-05-19
	UCS_STRU old_ucs=pPlate->ucs;
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||sys.part_map.plate.bDimCoord)
		bAdjustPlateUcs=TRUE;
	PROFILE_VERTEX *pVertex=NULL;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	//复制顶点链表数据，绘制钢板零件图时使用该顶点链表,因为绘制过程可能修改链表数据 wht 11-07-07
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	if(m_bSpecifyUCS) 
	{	//指定绘图坐标系优先于内部调整坐标系 wht 10-08-13
		bAdjustPlateUcs=TRUE;
		pPlate->ucs=new_ucs;//调整板坐标系 
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,old_ucs,TRUE);
			geom_center+=pVertex->vertex; //
			coord_trans(pVertex->vertex,new_ucs,FALSE);
			if(pVertex->type==1||pVertex->type==2)
			{	//转换圆心拾取点
				coord_trans(pVertex->center,old_ucs,TRUE);
				coord_trans(pVertex->center,new_ucs,FALSE);
			}
		}
		//计算几何中心
		project_point(geom_center,old_ucs.origin,old_ucs.axis_z);
		geom_center/=vertex_list.GetNodeNum();
		project_point(geom_center,pPlate->ucs.origin,pPlate->ucs.axis_z);
		if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{	//环向封板
			double R=0,W=0;
			pPlate->GetDesignItemValue('R',&R);
			pPlate->GetDesignItemValue('W',&W);
			geom_center.Set(R+0.5*W,0);
			coord_trans(geom_center,old_ucs,TRUE);
		}
	}
	else if(bAdjustPlateUcs)	//标注坐标
	{	//对于参数化板可以直接修改顶点列表中的坐标值，因为每次使用参数板时重新设计就可以生成新的顶点列表
		UCS_STRU ucs;
		if(AdjustPlateUcs(pPlate,ucs,geom_center)>EPS2)
		{	
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				coord_trans(pVertex->vertex,pPlate->ucs,TRUE);
				coord_trans(pVertex->vertex,ucs,FALSE);
				if(pVertex->type==1||pVertex->type==2)
				{	//转换圆心拾取点
					coord_trans(pVertex->center,pPlate->ucs,TRUE);
					coord_trans(pVertex->center,ucs,FALSE);
				}
			}
			new_ucs=ucs;	//记录板坐标系
			pPlate->ucs=ucs;//调整板坐标系 
		}
		else
			new_ucs=pPlate->ucs;
	}
	else
	{
		new_ucs=pPlate->ucs;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pPlate->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=vertex_list.GetNodeNum();
		project_point(geom_center,pPlate->ucs.origin,pPlate->ucs.axis_z);
		if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{	//环向封板
			double R=0,W=0;
			pPlate->GetDesignItemValue('R',&R);
			pPlate->GetDesignItemValue('W',&W);
			geom_center.Set(R+0.5*W,0);
			coord_trans(geom_center,pPlate->ucs,TRUE);
		}
	}

	PROFILE_VERTEX *pPrevVertex=NULL;
	BOOL bFinal=FALSE;
	double max_edge_len=0;
	f3dPoint base_face_pick,qu_face_pick;	//基准面拾取点 火曲面拾取点
	f3dPoint arc_line_start,arc_line_end;	//记录钢管肋板圆弧边的起始点\终止点
	for(pVertex=vertex_list.GetFirst();1;pVertex=vertex_list.GetNext())
	{
		double edge_len=0;
		if(pVertex==NULL)
		{
			pVertex=vertex_list.GetFirst();
			bFinal=TRUE;
		}
		if(pVertex==NULL)
			break;
		if(bZhiWanPlate)
		{	//查找基准面拾取点和火曲面拾取点 wht 11-05-19
			if(pVertex->vertex.feature==1)
				base_face_pick=pVertex->vertex;
			else if(pVertex->vertex.feature==2)
				qu_face_pick=pVertex->vertex;
		}
		//标注顶点相对坐标值
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
			DimCoordValue(pPlate,pVertex->vertex);
		if(pPrevVertex==NULL)
			line.startPt=pVertex->vertex;
		else
		{
			line.endPt=pVertex->vertex;
			if(bFinal)
				line.feature=pPlate->handle;
			if(pPrevVertex->m_bWeldEdge&&!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
				line.pen.style=PS_WELDLINE;	//零件图才需要绘制焊缝线
			else
				line.pen.style=PS_SOLID;
			if(pPrevVertex->type==0)
			{
				if(sys.part_map.plate.bDimPlateEdgeLen)//标注钢板边长
					DimPlateEdgeLen(line);
				arcline=LineToArc(line);
				edge_arr.Add(arcline);
				edge_len=DISTANCE(line.startPt,line.endPt);
				if(max_edge_len<edge_len)
				{
					max_edge_len=edge_len;
					long_edge_mid=0.5*(line.startPt+line.endPt);
					long_edge_angle=Cal2dLineAng(line.startPt.x,line.startPt.y,line.endPt.x,line.endPt.y);
				}
			}
			else
			{
				PROFILE_VERTEX profileVertex=*pPrevVertex;
				if(profileVertex.type==1)
					arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),profileVertex.sector_angle);
				else if(profileVertex.type==2)
				{	
					double sign=1.0;
					arc_line_start=line.startPt;	//记录圆弧起始点
					arc_line_end=line.endPt;		//记录圆弧终止点
					if(profileVertex.radius<0)
						sign=-1.0;
					arcline.CreateMethod3(line.startPt,line.endPt,f3dPoint(0,0,sign),sign*profileVertex.radius,profileVertex.center);
					//不能用CreateMethod2,否则丢失了拾取点信息，对于大于180°的弧是错的。 wjh-2012.6.14
					//arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,sign),pPrevVertex->sector_angle);
				}
				else //if(profileVertex.type==3)
				{
					int sign=1;
					arc_line_start=line.startPt;	//记录圆弧起始点
					arc_line_end=line.endPt;		//记录圆弧终止点
					if(profileVertex.radius<0)
						sign=-1;
					f3dPoint center=profileVertex.center;
					f3dPoint col_axis=profileVertex.column_norm;
					coord_trans(center,pPlate->ucs,FALSE);
					vector_trans(col_axis,pPlate->ucs,FALSE);
					if(pPrevVertex->vertex.feature%10==2&&pVertex->vertex.feature%10==2)
					{	//制弯面上的椭圆弧，首先应将圆心标定点及柱面法线旋转展开至基面上再进行坐标系转换 wht 11-05-19
						center=pPlate->SpreadTransV(center);
						col_axis=pPlate->SpreadTransV(col_axis,false);
					}
					line.startPt.z=line.endPt.z=0;	//起始点终止点Z坐标归零 wht 11-06-23
					arcline.CreateEllipse(center,line.startPt,line.endPt,col_axis,f3dPoint(0,0,sign),sign*profileVertex.radius);
					//重新绘制钢板的椭圆弧边
					DrawPlateEllipseLine(pPlate,arcline,FALSE);
				}
				if(profileVertex.m_bWeldEdge&&!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
					arcline.pen.style=PS_WELDLINE;	//零件图才需要绘制焊缝线
				else
					arcline.pen.style=PS_SOLID;
				//标注相对坐标
				DimArcLineCoordValue(pPlate,arcline);
				edge_arr.Add(arcline);
				edge_len=DISTANCE(arcline.Start(),arcline.End());
				if(max_edge_len<edge_len)
				{
					max_edge_len=edge_len;
					long_edge_mid=0.5*(arcline.Start()+arcline.End());
					long_edge_angle=Cal2dLineAng(arcline.Start().x,arcline.Start().y,arcline.End().x,arcline.End().y);
				}
			}
			if(pPrevVertex->m_bWeldEdge)
				ProcessPlateWeldEdge(pPrevVertex,arcline);//处理焊缝边(绘制焊缝线、绘制坡口线、标注焊接件号、标注坡口尺寸等) wht 18-08-09
			line.startPt=line.endPt;
		}
		pPrevVertex=pVertex;
		if(bFinal)
			break;
	}
	if(bZhiWanPlate)
	{	//绘制火曲线 wht 11-05-19
		f3dLine huoquline;
		bool bZhiWanStart=false,bZhiWanEnd=false;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.feature<=10)
				continue;
			if(!bZhiWanStart)
			{
				huoquline.startPt=pVertex->vertex;
				bZhiWanStart=true;
			}
			else if(!bZhiWanEnd)
			{
				huoquline.endPt=pVertex->vertex;
				bZhiWanEnd=true;
			}
		}
		if(bZhiWanStart&&bZhiWanEnd)
		{
			huoquline.pen.style=PS_DASHDOTDOT;
			edge_arr.Add(LineToArc(huoquline));
		}
		//判断火曲方式
		f3dPoint prj_base_pick,base_vec;
		SnapPerp(&prj_base_pick,huoquline,base_face_pick);
		base_vec=base_face_pick-prj_base_pick;
		normalize(base_vec);
		f3dPoint norm = pPlate->HuoquFaceNorm();
		vector_trans(norm,pPlate->ucs,FALSE);
		BOOL bNeiQu=TRUE;
		double dd = cal_angle_of_2vec(base_vec,norm);
		if(dd<Pi/2||(fabs(dd-Pi/2)<EPS&&norm.z>0))	//火曲面向外曲(正曲)
			bNeiQu=FALSE;
		m_ciHuoQuType=bNeiQu?1:2;
		//标注钢板火曲工艺字符串 wht 11-05-19
		TXTDIM_STRU txt_dim;
		txt_dim.txtHeight=sys.fonts.plate.fDimTextSize;	//火曲工艺内容字体高度独立设置
		txt_dim.dimPos=0.5*(huoquline.startPt+huoquline.endPt);
		txt_dim.dimVec=huoquline.endPt-huoquline.startPt;
		double huoquangle=cal_angle_of_2vec(pPlate->HuoquFaceNorm(),pPlate->ucs.axis_z);
		GetPlateHuoQuProcessStr(txt_dim,bNeiQu,huoquangle*DEGTORAD_COEF,pPlate->m_fDeformCoef,pPlate->GetThick());
		double fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
		normalize(txt_dim.dimVec);
		txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
		text_dim_arr.Add(txt_dim);
	}
	//显示环向板内圆 wht 11-05-27
	double inner_r=0;	//内圆半径
	f3dPoint col_axis;	//圆柱法线方向
	if(pPlate->GetInnerCirRadius(inner_r,col_axis))
	{
		f3dPoint inner_origin;
		if(bAdjustPlateUcs)
		{	//标注相对坐标
			coord_trans(inner_origin,old_ucs,TRUE);	//pPlate->ucs
			coord_trans(inner_origin,new_ucs,FALSE);
		}
		if(fabs(col_axis*pPlate->ucs.axis_z)<EPS_COS)
		{	//椭圆
			f3dPoint workNorm(0,0,1),columnNorm=col_axis;
			if(bAdjustPlateUcs)
				vector_trans(columnNorm,new_ucs,FALSE);
			else
				vector_trans(columnNorm,pPlate->ucs,FALSE);
			f3dPoint minorAxis=columnNorm^workNorm;	
			normalize(minorAxis);		//椭圆短轴方向
			//顺时针旋转90度 wht 11-05-12
			f3dPoint majorAxis(minorAxis.y,-minorAxis.x,minorAxis.z);	
			normalize(majorAxis);		//椭圆长轴方向
			double radiusRatio=fabs(columnNorm*workNorm);
			double minorRadius=inner_r;				//椭圆短半轴长度
			double majorRadius=inner_r/radiusRatio;	//椭圆长半轴长度
			f3dPoint start=inner_origin+majorAxis*majorRadius;
			f3dPoint end = inner_origin+minorAxis*minorRadius;
			arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,inner_r);
			DrawPlateEllipseLine(pPlate,arcline,FALSE);
			edge_arr.Add(arcline);
			start=end;
			end=inner_origin-majorAxis*majorRadius;
			arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,inner_r);
			DrawPlateEllipseLine(pPlate,arcline,FALSE);
			edge_arr.Add(arcline);
			start=end;
			end = inner_origin-minorAxis*minorRadius;
			arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,inner_r);
			DrawPlateEllipseLine(pPlate,arcline,FALSE);
			edge_arr.Add(arcline);
			start=end;
			end=inner_origin+majorAxis*majorRadius;
			arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,inner_r);
			DrawPlateEllipseLine(pPlate,arcline,FALSE);
			edge_arr.Add(arcline);
			//标注长轴半径 
			rotate_dim.startPos=inner_origin;
			rotate_dim.endPos=inner_origin+majorAxis*majorRadius;
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dist = majorRadius;	//长轴半径
			rotate_dim.angle=Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.bCanTextOutter=TRUE;
			rotate_dim_arr.Add(rotate_dim);
			rotate_dim.bCanTextOutter=FALSE;
			//标注短轴半径 
			rotate_dim.startPos=inner_origin;
			rotate_dim.endPos=inner_origin+minorAxis*minorRadius;
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dist = minorRadius;
			rotate_dim.angle=Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.bCanTextOutter=TRUE;
			rotate_dim_arr.Add(rotate_dim);
			rotate_dim.bCanTextOutter=FALSE;
			//内圆环为椭圆是需要绘图截面图 标注椭圆长轴长度 wht 10-01-21
			double plate_len=pPlate->GetLength();
			double plate_thick=pPlate->GetThick();
			double tana=tan(radiusRatio);
			double offset_dist=tana*plate_thick;	
			f3dPoint offset_origin(0,-plate_len);	//具体值由经验确定
			line.pen.style=PS_SOLID;
			line.ID=0;
			//指定坐标系时 不绘制截面图 wht 10-08-31
			if(!m_bSpecifyUCS)
			{	//环向板截面图
				line.startPt.Set(-0.5*plate_len,-0.5*plate_thick+offset_origin.y);
				line.endPt.Set(-0.5*plate_len,0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(0.5*plate_len,0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(0.5*plate_len,-0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(-0.5*plate_len,-0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				//标注椭圆长轴直径
				rotate_dim.startPos.Set(-(majorRadius-0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
				rotate_dim.endPos.Set(majorRadius+0.5*offset_dist,-0.5*plate_thick+offset_origin.y);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dimPos.y-=15;
				rotate_dim.dist=majorRadius*2;
				rotate_dim.angle = 0;
				rotate_dim_arr.Add(rotate_dim);
				
				//椭圆弧截面线--右侧
				line.startPt.Set(majorRadius-0.5*offset_dist,0.5*plate_thick+offset_origin.y);
				line.endPt.Set(majorRadius+0.5*offset_dist,-0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				//椭圆弧截面线--左侧
				line.startPt.Set(-(majorRadius+0.5*offset_dist),0.5*plate_thick+offset_origin.y);
				line.endPt.Set(-(majorRadius-0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				//标注偏移值
				rotate_dim.startPos.Set(-(majorRadius+0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
				rotate_dim.endPos.Set(-(majorRadius-0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dimPos.y-=15;
				rotate_dim.dist=offset_dist;
				rotate_dim.angle = 0;
				rotate_dim_arr.Add(rotate_dim);
			}
		}
		else
		{	//圆
			f3dPoint startPt(inner_origin.x,inner_origin.y+inner_r);
			arcline.CreateMethod1(inner_origin,startPt,f3dPoint(0,0,1),2*Pi);
			edge_arr.Add(arcline);
			//标注内圆直径
			if(!m_bSpecifyUCS)
			{	
				diam_dim.chordPos.Set(inner_origin.x+arcline.Radius(),inner_origin.y);
				diam_dim.dist=ftoi(arcline.Radius()*2);
				diam_dim.farChordPos=-diam_dim.chordPos;
				diam_dim.leaderLength=5;
				rotate_point_around_axis(diam_dim.chordPos,-0.5*Pi,f3dPoint(0,0,0),f3dPoint(0,0,1));
				rotate_point_around_axis(diam_dim.farChordPos,-0.5*Pi,f3dPoint(0,0,0),f3dPoint(0,0,1));
				diameter_dim_arr.Add(diam_dim);
			}
		}
	}
	if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{	//环向封板标注位置
		long_edge_angle=0;
		long_edge_mid=geom_center;
		coord_trans(long_edge_mid,pPlate->ucs,FALSE);
	}
	line.pen.style=PS_SOLID;
	arcline.pen.style=PS_SOLID;
	line.feature=0;
	arcline.feature=0;
	//生成大样图 或者标注构件顶点坐标值时不需要绘制钢管横截面
	if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)//零件图
	{	
		arcline.pen.style=PS_DASH;	//隐藏线
		//绘制钢管横截面图
		f3dPoint tube_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(tube_vec);
		if((pPlate->m_iParamType==TYPE_CIRRIBPLATE
			||pPlate->m_iParamType==TYPE_TUBERIBPLATE)
			&&fabs(pPlate->ucs.axis_z*tube_vec)<EPS_COS)
		{	//绘图钢管椭圆截面图
			f3dPoint worknorm(0,0,1),columnnorm=tube_vec;
			double radius=pDatumTube->GetDiameter()*0.5;
			vector_trans(columnnorm,pPlate->ucs,FALSE);
			f3dPoint majorAxis(columnnorm.x,columnnorm.y);
			normalize(majorAxis);
			f3dPoint start,end;
			int i=0;
			for(i=0;i<2;i++)
			{
				if(i==1)	//绘制内圆
					radius=pDatumTube->GetDiameter()*0.5-pDatumTube->GetThick();
				start=majorAxis*radius;
				end = f3dPoint(-majorAxis.y*radius,majorAxis.x*radius);
				arcline.CreateEllipse(f3dPoint(0,0,0),start,end,columnnorm,f3dPoint(0,0,1),radius);
				edge_arr.Add(arcline);
				start*=-1.0;
				arcline.CreateEllipse(f3dPoint(0,0,0),end,start,columnnorm,f3dPoint(0,0,1),radius);
				edge_arr.Add(arcline);
				end*=-1.0;
				arcline.CreateEllipse(f3dPoint(0,0,0),start,end,columnnorm,f3dPoint(0,0,1),radius);
				edge_arr.Add(arcline);
				start*=-1.0;
				arcline.CreateEllipse(f3dPoint(0,0,0),end,start,columnnorm,f3dPoint(0,0,1),radius);
				edge_arr.Add(arcline);
			}
		}
		else
		{
			arcline.CreateMethod1(f3dPoint(0,0,0),f3dPoint(pDatumTube->GetDiameter()*0.5-pDatumTube->GetThick()),f3dPoint(0,0,1),2*Pi);
			edge_arr.Add(arcline);
			arcline.CreateMethod1(f3dPoint(0,0,0),f3dPoint(pDatumTube->GetDiameter()*0.5),f3dPoint(0,0,1),2*Pi);
			arcline.feature=pDatumTube->handle;
			edge_arr.Add(arcline);
		}
		arcline.pen.style=PS_SOLID;	//恢复到可见层
	}
	//绘制与环向板相连的两块钢板的俯视图
	long hPart1,hPart2;
	CLDSPlate *pPlate1=NULL,*pPlate2=NULL;
	if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		pPlate->GetDesignItemValue('F',&hPart1);
	else if(pPlate->m_iParamType==TYPE_CIRRIBPLATE||pPlate->m_iParamType==TYPE_TUBERIBPLATE)
		pPlate->GetDesignItemValue('E',&hPart1);	//'B'改为'E' wht 11-05-12
	if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		pPlate->GetDesignItemValue('G',&hPart2);
	else if(pPlate->m_iParamType==TYPE_CIRRIBPLATE||pPlate->m_iParamType==TYPE_TUBERIBPLATE)
		pPlate->GetDesignItemValue('F',&hPart2);	//'C'改为'F' wht 11-05-12
	pPlate1 = (CLDSPlate*)pPlate->BelongModel()->FromPartHandle(hPart1,CLS_PLATE);
	pPlate2 = (CLDSPlate*)pPlate->BelongModel()->FromPartHandle(hPart2,CLS_PLATE);
	//此处不可以随便返回，否则以下代码段都将无法执行 wht 11-05-16
	//if(pPlate1==NULL&&pPlate2==NULL)
	//	return FALSE;
	if(pPlate1==NULL&&pPlate2)
	{	//仅有一块基准钢板时基准钢板保存在pPlate1中
		pPlate1=pPlate2;
		pPlate2=NULL;
	}
		//根据基准钢板在钢管垂直截面内的射线方向
	f3dPoint axis_x_1,axis_x_2;
	if(pPlate1)
		axis_x_1 = pPlate->ucs.axis_z^pPlate1->ucs.axis_z;
	if(pPlate2)
		axis_x_2 = pPlate->ucs.axis_z^pPlate2->ucs.axis_z;
	normalize(axis_x_1);
	normalize(axis_x_2);
		//修正射线方向的正负号
	f3dPoint vertice_vec;
	if(pPlate1&&pPlate1->vertex_list.GetNodeNum()>0)
	{
		f3dPoint vertice=pPlate1->vertex_list.GetFirst()->vertex;
		coord_trans(vertice,pPlate1->ucs,TRUE);
		vertice_vec=vertice-pPlate->ucs.origin;
		if(vertice_vec*axis_x_1<0)
			axis_x_1*=-1.0;
	}
	if(pPlate2&&pPlate2->vertex_list.GetNodeNum()>0)
	{
		f3dPoint vertice=pPlate2->vertex_list.GetFirst()->vertex;
		coord_trans(vertice,pPlate2->ucs,TRUE);
		vertice_vec=vertice-pPlate->ucs.origin;
		if(vertice_vec*axis_x_2<0)
			axis_x_2*=-1.0;
	}
		//得到两块钢板按逆时针排列的前后位置
	int order = 0;	//0-第一个钢板在前 1-第二个钢板在前
	f3dPoint axis_z_verify=axis_x_1^axis_x_2;
	if(axis_z_verify*pPlate->ucs.axis_z<0)
	{	//第二个钢板在前对调 W，N值
		if(pPlate->m_iParamType==TYPE_CIRRIBPLATE)
		{	//环向肋板
			long G=1;	//'O'改为'G' wht 11-05-12
			pPlate->GetDesignItemValue('G', &G);
			if(G<=0)	//顶部为圆弧连接
				N=W;//保证N、W值相等顶部才可能为圆弧连接
			double tempW=W;
			W=N;
			N=tempW;
		}
		order = 1;
	}

	double r=pDatumTube->GetDiameter()*0.5;
	int i=1;
	f3dPoint axis_x;
	//if(pPlate->m_iParamType!=TYPE_TUBERIBPLATE)
	//暂时不绘制与封板相连的两块钢板的俯视图 wht 10-08-04
	if( pPlate->m_iParamType==TYPE_CIRRIBPLATE||
		pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{
		while(i<3)
		{
			double t2=0;
			long hPlate=0;
			if((order==0&&i==1)||(order==1&&i==2))
			{
				axis_x = axis_x_1;
				if(pPlate1)
				{
					t2=pPlate1->GetThick();
					hPlate=pPlate1->handle;
				}
				else
					i++;
			}
			else if((order==0&&i==2)||(order==1&&i==1))
			{
				axis_x = axis_x_2;
				if(pPlate2)
				{
					t2=pPlate2->GetThick();
					hPlate=pPlate2->handle;
				}
				else
					i++;
			}
			if(i==1)
			{
				line_up.feature=0;
				line_down.feature=hPlate;
			}
			else
			{
				line_up.feature=hPlate;
				line_down.feature=0;
			}
			vector_trans(axis_x,pPlate->ucs,FALSE);
			axis_x.z=0;
			f3dPoint axis_z(-axis_x.y,axis_x.x);
			line_up.startPt=axis_x*r+axis_z*(t2/2.0);
			double offset_dist=0;
			if(pPlate->m_iParamType==TYPE_CIRRIBPLATE)
				offset_dist=r-sqrt(r*r-(t2*0.5)*(t2*0.5));
			if(i==1)
				line_up.endPt  =axis_x*(r+W-offset_dist)+axis_z*(t2/2.0);
			else
			{
				if(pPlate->m_iParamType==TYPE_CIRRIBPLATE) //肋板
					line_up.endPt  =axis_x*(r+N-offset_dist)+axis_z*(t2/2.0);
				else	//封板
					line_up.endPt  =axis_x*(r+W-offset_dist)+axis_z*(t2/2.0);
			}
			//if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE || i==1)
			//	line_up.pen.style=PS_WELDLINE;	//环形封板上下边线及环形肋板第一竖向板上边线
			//else
			line_up.pen.style=PS_SOLID;
			if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
				edge_arr.Add(LineToArc(line_up));
			line_down.startPt=line_up.endPt-axis_z*t2;
			line_down.endPt=line_up.startPt-axis_z*t2;
			//if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE || i==2)
			//	line_down.pen.style=PS_WELDLINE;	//环形封板上下边线及环形肋板第二竖向板下边线
			//else
			line_down.pen.style=PS_SOLID;
			if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
				edge_arr.Add(LineToArc(line_down));
			
			line.startPt=line_up.endPt;
			line.endPt=line_down.startPt;
			line.pen.style=PS_SOLID;
			if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)//&&pPlate->m_iParamType==TYPE_CIRRIBPLATE
				edge_arr.Add(LineToArc(line));
			i++;
		}
	}
	//生成大样图 或者标注构件顶点坐标值时不绘制钢管俯视图 不标注尺寸
	if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
	{
		double R=pDatumTube->GetDiameter()*0.5+V;	//内圆弧半径
		ANGULARDIM_STRU angular_dim;//角度标注
		rotate_dim.bDisplayDimLine=TRUE;
		if(pPlate->m_iParamType==TYPE_TUBERIBPLATE)
		{	//标注钢管肋板的 W、N、D
			double thick_w,thick_n;
			if(pPlate1)
				thick_n=thick_w=pPlate1->GetThick();
			if(pPlate2)
				thick_n=pPlate2->GetThick();
			if(pPlate1&&pPlate2==NULL) 
				axis_x_2=-1.0*axis_x_1;	//仅有一块基准钢板 axis_x_2与axis_x_1方向相反
			f3dPoint axis_w,axis_n,axis_z;
			axis_w=axis_x_1;
			axis_n=axis_x_2;
			if(axis_x_1*pPlate->ucs.axis_x>0)
			{
				double t=thick_w;
				thick_w=thick_n;
				thick_n=t;
				axis_w=axis_x_2;
				axis_n=axis_x_1;
			}
			//内圆弧大于180度时分两段绘制
			PROFILE_VERTEX *pTailVertex=NULL;
			pTailVertex=vertex_list.GetTail();
			pTailVertex=vertex_list.GetPrev();
			if(pTailVertex&&pTailVertex->type>0)
				arc_line_start=pTailVertex->vertex;
			//标注W
			vector_trans(axis_w,pPlate->ucs,FALSE);
			axis_w.z=0;
			axis_z.Set(axis_w.y,-axis_w.x);
			if(arc_line_start.x<0)
				rotate_dim.startPos=arc_line_start;
			else 
				rotate_dim.startPos=arc_line_end;
			rotate_dim.endPos=rotate_dim.startPos+axis_w*W;
			rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5-axis_z*(max(30,thick_w));
			rotate_dim.dist=W;
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim_arr.Add(rotate_dim);
			
			//绘制W侧钢板示意图
			line.startPt=rotate_dim.startPos;
			line.endPt  =rotate_dim.endPos;
			line.pen.style=PS_SOLID;
			edge_arr.Add(LineToArc(line));
			line.startPt=rotate_dim.startPos-axis_z*thick_w;
			line.endPt  =rotate_dim.endPos-axis_z*thick_w;
			line.pen.style=PS_SOLID;
			edge_arr.Add(LineToArc(line));
			//------绘制W侧钢板中心线  wht 10-08-04
			line.startPt.Set();
			line.endPt = rotate_dim.endPos-axis_z*thick_w*0.5;
			line.pen.style=PS_DASHDOT;
			edge_arr.Add(LineToArc(line));
			//标注半个钢板的厚度
			rotate_dim.startPos=line.endPt;
			rotate_dim.dist=0.5*thick_w;
			rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5+axis_w*(max(30,pPlate->GetThick()));
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim_arr.Add(rotate_dim);
			//------
			//标注N
			vector_trans(axis_n,pPlate->ucs,FALSE);
			axis_n.z=0;
			axis_z.Set(-axis_n.y,axis_n.x);
			if(arc_line_start.x>0)
				rotate_dim.startPos=arc_line_start;
			else 
				rotate_dim.startPos=arc_line_end;
			rotate_dim.endPos=rotate_dim.startPos+axis_n*N;
			rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5-axis_z*(max(30,thick_n));
			rotate_dim.dist=N;
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim_arr.Add(rotate_dim);
			//绘制N侧钢板
			line.startPt=rotate_dim.startPos;
			line.endPt  =rotate_dim.endPos;
			line.pen.style=PS_SOLID;
			edge_arr.Add(LineToArc(line));
			line.startPt=rotate_dim.startPos-axis_z*thick_n;
			line.endPt  =rotate_dim.endPos-axis_z*thick_n;
			line.pen.style=PS_SOLID;
			edge_arr.Add(LineToArc(line));
			//------绘制N侧钢板中心线  wht 10-08-04
			line.startPt.Set();
			line.endPt = rotate_dim.endPos-axis_z*thick_n*0.5;
			line.pen.style=PS_DASHDOT;
			edge_arr.Add(LineToArc(line));
			//标注半个钢板的厚度
			rotate_dim.startPos=line.endPt;
			rotate_dim.dist=0.5*thick_n;
			rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5+axis_n*(max(30,pPlate->GetThick()));
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim_arr.Add(rotate_dim);
			//标注圆弧夹角
			angular_dim.vertex.Set();
			angular_dim.startPos=axis_n*r*0.5;
			angular_dim.endPos=axis_w*r*0.5;
			f3dPoint mid_vec=axis_w+axis_n;
			normalize(mid_vec);
			if(mid_vec.IsZero())	//X轴方向
				mid_vec.Set(0,-1,0);
			angular_dim.dimPos=mid_vec*r*0.5;
			angular_dim_arr.Add(angular_dim);
			//标注顶部圆弧半径R
			diam_dim.dist=R;
			diam_dim.leaderLength=5;
			diam_dim.farChordPos.Set();
			diam_dim.bDimRadial=TRUE;
			diam_dim.chordPos=f3dPoint(0,0,0)-mid_vec*R;
			diameter_dim_arr.Add(diam_dim);
			//------

			//标注H
			rotate_dim.startPos=rotate_dim.endPos;
			rotate_dim.endPos=rotate_dim.startPos+axis_z*H;
			rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5+axis_n*(max(30,pPlate->GetThick()));
			rotate_dim.dist=H;
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim_arr.Add(rotate_dim);
		}
		else
		{	
			if(pPlate->m_iParamType==TYPE_CIRRIBPLATE&&pPlate1)
			{	
				long G=0; //'O'改为'G' wht 11-05-12
				pPlate->GetDesignItemValue('G',&G);
				//标注'W'尺寸
				int sign=1;
				if(pPlate1&&pPlate2==NULL)
				{	//仅有一块基准钢板
					if(arc_line_start.x<EPS)
					{
						sign=-1;
						rotate_dim.startPos=arc_line_start;
					}
					else
						rotate_dim.startPos=arc_line_end;
				}
				else if(pPlate1&&pPlate2)
				{	//有两块基准钢板
					if(arc_line_start.y<EPS)
						rotate_dim.startPos=arc_line_start;
					else
						rotate_dim.startPos=arc_line_end;
				}
				rotate_dim.endPos.Set(rotate_dim.startPos.x+sign*W,rotate_dim.startPos.y);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dimPos.y-=40;
				rotate_dim.dist=W;
				rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
				rotate_dim_arr.Add(rotate_dim);
				if(G>0)	//顶部直线连接才需要标注H
				{	
					//标注'H'尺寸
					rotate_dim.startPos=rotate_dim.endPos;
					rotate_dim.endPos.Set(rotate_dim.startPos.x,rotate_dim.startPos.y+H);
					rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
					rotate_dim.dimPos.x+=(sign*20);
					rotate_dim.dist=H;
					rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
					rotate_dim_arr.Add(rotate_dim);
				}
				//-----------------  wht 10-08-04
				f3dPoint axis_x_w=axis_x_1;
				f3dPoint axis_x_n=axis_x_2;
				if(order==1)
				{	//第二块钢板在前
					axis_x_w=axis_x_2;
					axis_x_n=axis_x_1;
				}
				//将两钢板延伸方向转换到相对坐标系下
				vector_trans(axis_x_w,pPlate->ucs,FALSE);
				vector_trans(axis_x_n,pPlate->ucs,FALSE);
				//
				f3dPoint arc_start=arc_line_end,arc_end=arc_line_start;
				double start_angle=Cal2dLineAng(f2dPoint(0,0),f2dPoint(arc_line_start.x,arc_line_start.y));
				double end_angle=Cal2dLineAng(f2dPoint(0,0),f2dPoint(arc_line_end.x,arc_line_end.y));
				if(start_angle<end_angle)
				{
					arc_start=arc_line_start;
					arc_end=arc_line_end;
				}
				//绘制关联钢板中心线
				double offset_dist=r-sqrt(r*r-(pPlate1->GetThick()*0.5)*(pPlate1->GetThick()*0.5));
				line.startPt.Set();
				line.endPt = line.startPt+axis_x_w*(W+R-offset_dist);
				line.pen.style=PS_DASHDOT;
				edge_arr.Add(LineToArc(line));
				//标注半个钢板的厚度
				rotate_dim.startPos=line.endPt;
				rotate_dim.endPos=arc_start+axis_x_w*W;
				rotate_dim.dist=0.5*pPlate1->GetThick();
				rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5+axis_x_w*20;
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
				rotate_dim_arr.Add(rotate_dim);
				//
				if(pPlate2)
				{	
					//标注参数N长度
					rotate_dim.startPos=arc_end;
					rotate_dim.endPos=rotate_dim.startPos+axis_x_n*N;
					f3dPoint axis_z(-axis_x_n.y,axis_x_n.x);
					rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos)+axis_z*(max(30,pPlate2->GetThick()));
					rotate_dim.dist=N;
					rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
					rotate_dim_arr.Add(rotate_dim);
					//绘制关联钢板中心线
					offset_dist=r-sqrt(r*r-(pPlate2->GetThick()*0.5)*(pPlate2->GetThick()*0.5));
					line.startPt.Set();
					line.endPt = line.startPt+axis_x_n*(N+R-offset_dist);
					line.pen.style=PS_DASHDOT;
					edge_arr.Add(LineToArc(line));
					//标注半个钢板的厚度
					rotate_dim.startPos=line.endPt;
					rotate_dim.endPos=arc_end+axis_x_n*N;
					rotate_dim.dist=0.5*pPlate2->GetThick();
					rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5+axis_x_n*20;
					rotate_dim.bDisplayDimLine=TRUE;
					rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
					rotate_dim_arr.Add(rotate_dim);
				}
				else
				{	//绘制内圆弧半径
					line.startPt.Set();
					line.endPt = arc_end;
					line.pen.style=PS_DASHDOT;
					edge_arr.Add(LineToArc(line));
				}
				//标注圆弧夹角
				angular_dim.vertex.Set();
				if(pPlate2)
					angular_dim.startPos=axis_x_n*r*0.5;
				else 
				{
					angular_dim.startPos=arc_end;
					axis_x_n=arc_end.normalized();
				}
				angular_dim.endPos=axis_x_w*r*0.5;
				f3dPoint mid_vec=axis_x_w+axis_x_n;
				normalize(mid_vec);
				if(mid_vec.IsZero())	//X轴方向
					mid_vec.Set(0,-1,0);
				angular_dim.dimPos=mid_vec*r*0.5;
				angular_dim_arr.Add(angular_dim);
				
				//标注钢管半径r
				diam_dim.dist=r;
				diam_dim.leaderLength=5;
				diam_dim.farChordPos.Set();
				diam_dim.bDimRadial=TRUE;
				diam_dim.chordPos=f3dPoint(0,0,0)-mid_vec*r;
				diameter_dim_arr.Add(diam_dim);
				//-----------------  wht 10-08-04
			}
			else if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
			{	
				//------绘制封板内圆弧始终点与圆心的连线，并标注半径以及夹角 wht 10-08-04
				line.startPt.Set();
				line.endPt=arc_line_start;
				line.pen.style=PS_DASHDOT;
				edge_arr.Add(LineToArc(line));
				line.endPt=arc_line_end;
				edge_arr.Add(LineToArc(line));
				//标注半径
				diam_dim.dist=r;
				diam_dim.leaderLength=5;
				diam_dim.farChordPos.Set();
				diam_dim.bDimRadial=TRUE;
				f3dPoint start_vec=arc_line_start.normalized();
				f3dPoint end_vec=arc_line_end.normalized();
				f3dPoint mid_vec=start_vec+end_vec;
				normalize(mid_vec);
				if(mid_vec.IsZero())	//X轴方向
					mid_vec.Set(-1,0,0);
				//将半径标注放在圆弧1/4位置反向，防止与塔心标注重叠 
				mid_vec+=start_vec;
				normalize(mid_vec);
				if(mid_vec.IsZero())
					mid_vec.Set(-1,0,0);
				diam_dim.chordPos=f3dPoint(0,0,0)-mid_vec*r;
				diameter_dim_arr.Add(diam_dim);
				//标注圆弧夹角
				angular_dim.vertex.Set();
				angular_dim.startPos=arc_line_start*0.5;
				angular_dim.endPos=arc_line_end*0.5;
				mid_vec=start_vec+end_vec;
				normalize(mid_vec);
				if(mid_vec.IsZero())	//X轴方向
					mid_vec.Set(1,0,0);
				angular_dim.dimPos=mid_vec*r*0.5;
				angular_dim_arr.Add(angular_dim);
				//------

				//标注'W'尺寸
				rotate_dim.startPos.Set(pDatumTube->GetDiameter()*0.5,0);
				rotate_dim.endPos.Set(pDatumTube->GetDiameter()*0.5+W,0);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dimPos.y-=15;
				rotate_dim.dist=W;
				rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
				rotate_dim_arr.Add(rotate_dim);
				//标注'D'尺寸
				B*=RADTODEG_COEF;
				axis_x=pPlate->ucs.axis_x;
				vector_trans(axis_x,pPlate->ucs,FALSE);
				RotateVectorAroundVector(axis_x,0.5*B,f3dPoint(0,0,1));
				rotate_dim.startPos=axis_x*r;
				rotate_dim.endPos=rotate_dim.startPos+axis_x*D;
				f3dPoint axis_z(-axis_x.y,axis_x.x);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dimPos+=axis_z*10;
				rotate_dim.dist=D;
				rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
				rotate_dim_arr.Add(rotate_dim);
			}
		}
		/*
		//绘制水平/竖直轴线
		R = pDatumTube->GetDiameter()*0.5;
		line.startPt.Set(-(R+W+30),0);
		line.endPt.Set((R+W+30),0);
		line.pen.style=PS_DASHDOT;
		edge_arr.Add(LineToArc(line));
		line.startPt.Set(0,-(R+W+30));
		line.endPt.Set(0,(R+W+30));
		edge_arr.Add(LineToArc(line));*/
	}
	//绘制螺栓孔或图符
	CXhSimpleList<CLDSPart::HOLE> hole_list;
	pPlate->GetHoleSet(&hole_list);
	f3dCircle ls_cir;
	for(CLDSPart::HOLE* pHole=hole_list.EnumObjectFirst();pHole;pHole=hole_list.EnumObjectNext())
	{
		ls_cir.centre=pHole->xLocalPosition;
		ls_cir.centre.z=0;
		if(pHole->pBolt)
		{
			CLsRef* pLsRef=pPlate->FindLsByHandle(pHole->pBolt->handle);
			ls_cir.radius=pHole->pBolt->get_d()*0.5;
			ls_cir.feature=ftoi(pHole->pBolt->hole_d_increment*10);
			ls_cir.pen.style=pHole->pBolt->FilterFuncType();	//记录螺栓孔的功用类型
			ls_cir.centre.pen.style=pLsRef->cFlag;				//记录螺栓孔的工艺类型
		}
		else
		{
			ls_cir.radius = pHole->wBoltD*0.5;
			ls_cir.feature=ftoi(pHole->fHoleMargin*10);
			ls_cir.pen.style=CLDSBolt::FUNC_WATERHOLE;			//杂项孔的功用类型默认为镀锌引流孔
		}
		//标注螺栓相对坐标值	wht 09-09-16
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
			DimCoordValue(pPlate,ls_cir.centre);
		if((int)pHole->wWaistLen>0&&pHole->pBolt)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width=(int)pHole->wWaistLen;		//使用画笔宽度保存腰圆孔长度
			ls_cir.norm=pHole->pBolt->ucs.axis_x;		//使用螺栓孔法线方向保存腰圆方向
			vector_trans(ls_cir.norm,pPlate->ucs,FALSE);//转换到钢板坐标系下
		}
		cir_arr.Add(ls_cir);
	}
	//大样图或标注坐标时不绘制塔心方向线 
	if(pDatumTube->m_bHasWeldRoad&&!bPatternDrawing&&!sys.part_map.plate.bDimCoord)//&&pDatumTube->m_bAttachPartByWeldRoad)
	{
		double startAngle=pDatumTube->CalWeldRoadAngle();
		axis_x.Set(cos(startAngle),sin(startAngle));
		vector_trans(axis_x,pDatumTube->ucs,TRUE);
		vector_trans(axis_x,pPlate->ucs,FALSE);
		axis_x.z=0;
		normalize(axis_x);
		line.startPt.Set();
		line.endPt=axis_x*(r+100);
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
		TXTDIM_STRU txt_dim;
		txt_dim.dimPos=axis_x*(r+50);
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(txt_dim.dimText,"weld road(tower core)direction");
#else
		strcpy(txt_dim.dimText,"焊道(塔心)方向");
#endif
		txt_dim.dimVec=axis_x;
		text_dim_arr.Add(txt_dim);
		line.startPt=line.endPt;
		RotateVectorAroundVector(axis_x,-Pi/12,f3dPoint(0,0,1));
		line.endPt=line.startPt-axis_x*20;
		edge_arr.Add(LineToArc(line));
		RotateVectorAroundVector(axis_x,Pi/6,f3dPoint(0,0,1));
		line.endPt=line.startPt-axis_x*20;
		edge_arr.Add(LineToArc(line));
	}
	if(bPatternDrawing)//大样图中需要在焊烧死角处切弧
		CreateLineOnWeldCorner(pPlate,&vertex_list);
	//大样图中绘制钢板的内侧轮廓边
	if(sys.part_map.plate.bDrawInnerEdge)
		DrawInnerEdge(pPlate,&vertex_list);
	//青岛东方要求大样图中不绘制焊接子构件
	if(!(gxDrwLocalizer.ciCustomerSerial==5&&bPatternDrawing))
		DrawWeldSubPlate(pPlate,bAdjustPlateUcs);
	//恢复板坐标系
	if(bAdjustPlateUcs)
		pPlate->ucs=old_ucs;
	return TRUE;
}

//绘制环向板关联钢板截面图
//1.环向封板：绘制关联连接板、标注坡口尺寸
//2.环向肋板：在两侧焊接边标注焊接件号、标注坡口尺寸
//3.跨越连班：在两侧直边标注坡口尺寸
int RetrievedWeldCoverPlates(CLDSParamPlate *pCoverPlate,PRESET_ARRAY8<WELDCOVER_PLATE> &arrWeldPlates,
							 CHashTable<DESIGN_PLATE_ITEM*>* pItemHashTbl/*=NULL*/) ;	//From ParamPlate.cpp
BOOL CPlankDrawing::CreateRelativePlateInCirParamPlate(CLDSParamPlate *pPlate,BOOL bPatternDrawing,f3dPoint origin)
{
	BOOL bDrawRelaPlate=!bPatternDrawing&&!sys.part_map.plate.bDimCoord;
	if(bPatternDrawing&&gxDrwLocalizer.ciCustomerSerial==7)
		bDrawRelaPlate=TRUE;
	//绘制封板关联钢板
	if(bDrawRelaPlate&&pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{	
		double W=0,R=0,beta=180;
		pPlate->GetDesignItemValue('W',&W);	//环形带宽W
		pPlate->GetDesignItemValue('R',&R);	//环向板内圆弧半径(=钢管半径+加工间隙)
		pPlate->GetDesignItemValue('B',&beta);	//扇形夹角
		PRESET_ARRAY8<WELDCOVER_PLATE> arrWeldPlates;
		RetrievedWeldCoverPlates(pPlate,arrWeldPlates,NULL);
		f3dLine line1,line2;
		double betaByRad=beta*RADTODEG_COEF;
		for(UINT i=0;i<arrWeldPlates.Size();i++)
		{
			if(arrWeldPlates[i].alfa>(betaByRad-RADTODEG_COEF)||arrWeldPlates[i].alfa<RADTODEG_COEF)
				continue;	//超出封板扇形角范围
			GEPOINT axis_x=arrWeldPlates[i].vxRayVec;
			normalize(axis_x);
			axis_x.z=0;
			double fHalfThick=arrWeldPlates[i].pWeldPlate->GetThick()*0.5;
			double fOffsetAngleOutter=asin(fHalfThick/(R+W));
			GEPOINT temp_axis_x=axis_x;
			RotateVectorAroundVector(temp_axis_x,fOffsetAngleOutter,GEPOINT(0,0,1));
			line1.startPt=temp_axis_x*(R+W);
			RotateVectorAroundVector(temp_axis_x,-2*fOffsetAngleOutter,GEPOINT(0,0,1));
			line2.startPt=temp_axis_x*(R+W);

			if(arrWeldPlates[i].innerR>0)
			{
				double fLen=sqrt(R*R-arrWeldPlates[i].innerR*arrWeldPlates[i].innerR);
				GEPOINT datum_pos=axis_x*fLen;
				double fOffsetAngle=asin(fHalfThick/arrWeldPlates[i].innerR);
				temp_axis_x=axis_x;
				RotateVectorAroundVector(temp_axis_x,fOffsetAngle,GEPOINT(0,0,1));
				line1.endPt=datum_pos+temp_axis_x*arrWeldPlates[i].innerR;
				RotateVectorAroundVector(temp_axis_x,-2*fOffsetAngle,GEPOINT(0,0,1));
				line2.endPt=datum_pos+temp_axis_x*arrWeldPlates[i].innerR;
			}
			else
			{
				double fOffsetAngleInner=asin(fHalfThick/R);
				temp_axis_x=axis_x;
				RotateVectorAroundVector(temp_axis_x,fOffsetAngleInner,GEPOINT(0,0,1));
				line1.endPt=temp_axis_x*R;
				RotateVectorAroundVector(temp_axis_x,-2*fOffsetAngleInner,GEPOINT(0,0,1));
				line2.endPt=temp_axis_x*R;
			}
			line1.startPt+=origin;
			line1.endPt+=origin;
			line2.startPt+=origin;
			line2.endPt+=origin;
			edge_arr.Add(LineToArc(line1));
			edge_arr.Add(LineToArc(line2));
		}
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CPlankDrawing::CreateCircularParamPlate2(CLDSParamPlate *pPlate,BOOL bPatternDrawing/*=TRUE*/)
{
	if(pPlate->m_iParamType!=TYPE_CIRRIBPLATE&&pPlate->m_iParamType!=TYPE_CIRCOVERPLATE&&
		pPlate->m_iParamType!=TYPE_TUBERIBPLATE)
		return FALSE;
	CLDSLineTube *pDatumTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent);
	if(pDatumTube==NULL)
		return FALSE;
	if(pPlate->vertex_list.GetNodeNum()<3)
		pPlate->DesignPlate();	
	if(pPlate->vertex_list.GetNodeNum()<3)
		return FALSE;
	double tube_r=pDatumTube->GetDiameter()*0.5;
	double tube_t=pDatumTube->Thick;
	f3dPoint tube_vec=(pDatumTube->End()-pDatumTube->Start()).normalized();
	//调整板坐标系之前，智能提取焊缝边的焊接构件
	PARTSET part_set;
	CLDSGeneralPlate::DESIGN_PARAM itemparam;
	CLDSGeneralPlate::DESIGN_PARAM::DUAL_VALUE rela_part1,rela_part2;
	if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{
		if(!pPlate->GetDesignItemValue('F',&itemparam))
			rela_part1.hPart=0;
		else
		{
			if(itemparam.cValueType==3)	//句柄+数值
			{
				rela_part1.hPart=itemparam.value.hPart;
				rela_part1.uiAttribute=itemparam.value.uiAttribute;
			}
			else //if(item_param.cValueType)
				rela_part1.hPart=itemparam.ToLong();
		}
		if(!pPlate->GetDesignItemValue('G',&itemparam))
			rela_part2.hPart=0;
		else
		{
			if(itemparam.cValueType==3)	//句柄+数值
			{
				rela_part2.hPart=itemparam.value.hPart;
				rela_part2.uiAttribute=itemparam.value.uiAttribute;
			}
			else //if(item_param.cValueType)
				rela_part2.hPart=itemparam.ToLong();
		}
	}
	else if(pPlate->m_iParamType==TYPE_CIRRIBPLATE||pPlate->m_iParamType==TYPE_TUBERIBPLATE)
	{	
		if(!pPlate->GetDesignItemValue('E',&itemparam))
			rela_part1.hPart=0;
		else
		{
			if(itemparam.cValueType==3)	//句柄+数值
			{
				rela_part1.hPart=itemparam.value.hPart;
				rela_part1.uiAttribute=itemparam.value.uiAttribute;
			}
			else //if(item_param.cValueType)
				rela_part1.hPart=itemparam.ToLong();
		}
		if(!pPlate->GetDesignItemValue('F',&itemparam))
			rela_part2.hPart=0;
		else
		{
			if(itemparam.cValueType==3)	//句柄+数值
			{
				rela_part2.hPart=itemparam.value.hPart;
				rela_part2.uiAttribute=itemparam.value.uiAttribute;
			}
			else //if(item_param.cValueType)
				rela_part2.hPart=itemparam.ToLong();
		}
	}
	CLDSPlate *pPlate1=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(rela_part1.hPart,CLS_PLATE);
	CLDSPlate *pPlate2=(CLDSPlate*)pPlate->BelongModel()->FromPartHandle(rela_part2.hPart,CLS_PLATE);
	if(pPlate1)
		part_set.append(pPlate1);
	if(pPlate2)
		part_set.append(pPlate2);
	if(pDatumTube)
		part_set.append(pDatumTube);
	//获取钢板上螺栓
	CXhSimpleList<CLDSPart::HOLE> hole_list;
	pPlate->GetHoleSet(&hole_list);
	//调整绘图坐标系
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||sys.part_map.plate.bDimCoord)
		bAdjustPlateUcs=TRUE;
	UCS_STRU ucs,old_ucs=pPlate->ucs;
	if(m_bSpecifyUCS)	//指定绘图坐标系优先于内部调整坐标系 wht 10-08-13
		bAdjustPlateUcs=TRUE;
	else if(bAdjustPlateUcs && AdjustPlateUcs(pPlate,ucs,geom_center)>EPS2)
		new_ucs=ucs;
	else
		new_ucs=pPlate->ucs;
	pPlate->ucs=new_ucs;	//调整板坐标系 
	//复制顶点链表数据，绘制钢板零件图时使用该顶点链表,因为绘制过程可能修改链表数据 wht 11-07-07
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	PROFILE_VERTEX *pVertex=NULL,*pPrevVertex=NULL;
	pPlate->IntelliRecogWeldLine(&part_set);	//在使用vertex_list之前，重新识别焊缝线，重新设置之后会丢失焊缝信息 wht 18-08-13
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		coord_trans(pVertex->vertex,old_ucs,TRUE);
		geom_center+=pVertex->vertex; //
		coord_trans(pVertex->vertex,new_ucs,FALSE);
		if(pVertex->type==1||pVertex->type==2)
		{	//转换圆心拾取点
			coord_trans(pVertex->center,old_ucs,TRUE);
			coord_trans(pVertex->center,new_ucs,FALSE);
		}
		long hPart=pVertex->weldline.hWeldMotherPart;
	}
	//计算几何中心
	project_point(geom_center,old_ucs.origin,old_ucs.axis_z);
	geom_center/=vertex_list.GetNodeNum();
	project_point(geom_center,pPlate->ucs.origin,pPlate->ucs.axis_z);
	if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{	//环向封板
		double R=0,W=0;
		pPlate->GetDesignItemValue('R',&R);
		pPlate->GetDesignItemValue('W',&W);
		geom_center.Set(R+0.5*W,0);
		coord_trans(geom_center,old_ucs,TRUE);
	}
	//绘制钢板轮廓边
	f3dLine line;
	f3dArcLine arcline;
	BOOL bFinal=FALSE;
	double max_edge_len=0;
	f3dPoint base_face_pick,qu_face_pick;	//基准面拾取点 火曲面拾取点
	f3dPoint arc_line_start,arc_line_end;	//记录钢管肋板圆弧边的起始点\终止点
	line.pen.style=arcline.pen.style=PS_SOLID;
	line.feature=arcline.feature=0;
	TXTDIM_STRU txt_dim;
	BOOL bDimWeldPartLabelInEdge=(pPlate->m_iParamType==TYPE_CIRRIBPLATE&&gxDrwLocalizer.ciCustomerSerial==7);
	for(pVertex=vertex_list.GetFirst();1;pVertex=vertex_list.GetNext())
	{
		double edge_len=0;
		if(pVertex==NULL)
		{
			pVertex=vertex_list.GetFirst();
			bFinal=TRUE;
		}
		if(pVertex==NULL)
			break;
		if(pPlate->IsZhiWan())
		{	//查找基准面拾取点和火曲面拾取点 wht 11-05-19
			if(pVertex->vertex.feature==1)
				base_face_pick=pVertex->vertex;
			else if(pVertex->vertex.feature==2)
				qu_face_pick=pVertex->vertex;
		}
		//标注顶点相对坐标值
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
			DimCoordValue(pPlate,pVertex->vertex);
		if(pPrevVertex==NULL)
			line.startPt=pVertex->vertex;
		else
		{
			line.endPt=pVertex->vertex;
			if(bFinal)
				line.feature=pPlate->handle;
			if(pPrevVertex->m_bWeldEdge&&!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
				line.pen.style=PS_WELDLINE;	//零件图才需要绘制焊缝线
			else
				line.pen.style=PS_SOLID;
			if(pPrevVertex->type==0)
			{
				if(sys.part_map.plate.bDimPlateEdgeLen)//标注钢板边长
					DimPlateEdgeLen(line);
				arcline=LineToArc(line);
				edge_arr.Add(arcline);
				edge_len=DISTANCE(line.startPt,line.endPt);
				if(max_edge_len<edge_len)
				{
					max_edge_len=edge_len;
					long_edge_mid=0.5*(line.startPt+line.endPt);
					long_edge_angle=Cal2dLineAng(line.startPt.x,line.startPt.y,line.endPt.x,line.endPt.y);
				}
			}
			else
			{
				int sign=1;
				PROFILE_VERTEX profileVertex=*pPrevVertex;
				if(profileVertex.type==1)
					arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),profileVertex.sector_angle);
				else if(profileVertex.type==2)
				{	
					arc_line_start=line.startPt;	//记录圆弧起始点
					arc_line_end=line.endPt;		//记录圆弧终止点
					if(profileVertex.radius<0)
						sign=-1;
					arcline.CreateMethod3(line.startPt,line.endPt,f3dPoint(0,0,sign),sign*profileVertex.radius,profileVertex.center);
				}
				else //if(profileVertex.type==3)
				{
					arc_line_start=line.startPt;	//记录圆弧起始点
					arc_line_end=line.endPt;		//记录圆弧终止点
					if(profileVertex.radius<0)
						sign=-1;
					f3dPoint center=profileVertex.center;
					f3dPoint col_axis=profileVertex.column_norm;
					if(pPrevVertex->vertex.feature%10==2&&pVertex->vertex.feature%10==2)
					{	//制弯面上的椭圆弧，首先应将圆心标定点及柱面法线旋转展开至基面上再进行坐标系转换 wht 11-05-19
						coord_trans(center,old_ucs,FALSE);
						vector_trans(col_axis,old_ucs,FALSE);
						center=pPlate->SpreadTransV(center);
						col_axis=pPlate->SpreadTransV(col_axis,false);
						coord_trans(center,old_ucs,TRUE);
						vector_trans(col_axis,old_ucs,TRUE);
					}
					coord_trans(center,pPlate->ucs,FALSE);
					vector_trans(col_axis,pPlate->ucs,FALSE);
					line.startPt.z=line.endPt.z=0;	//起始点终止点Z坐标归零 wht 11-06-23
					arcline.CreateEllipse(center,line.startPt,line.endPt,col_axis,f3dPoint(0,0,sign),sign*profileVertex.radius);
					//重新绘制钢板的椭圆弧边
					DrawPlateEllipseLine(pPlate,arcline,FALSE);
				}
				if(profileVertex.m_bWeldEdge&&!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
					arcline.pen.style=PS_WELDLINE;	//零件图才需要绘制焊缝线
				else
					arcline.pen.style=PS_SOLID;
				//标注相对坐标
				DimArcLineCoordValue(pPlate,arcline);
				edge_arr.Add(arcline);
				edge_len=DISTANCE(arcline.Start(),arcline.End());
				if(max_edge_len<edge_len)
				{	//计算圆弧处的数据点位置
					f3dPoint mid_pt=arcline.PositionInAngle(arcline.SectorAngle()*0.5);
					f3dPoint center=arcline.Center();
					f3dPoint off_vec=sign*(center-mid_pt).normalized();
					long_edge_mid=mid_pt+off_vec*1.2*sys.fonts.plate.fDimTextSize;
					max_edge_len=edge_len;
					long_edge_angle=Cal2dLineAng(arcline.Start().x,arcline.Start().y,arcline.End().x,arcline.End().y);
				}
			}
			if(pPrevVertex->m_bWeldEdge)
				ProcessPlateWeldEdge(pPrevVertex,arcline);//处理焊缝边(绘制焊缝线、绘制坡口线、标注焊接件号、标注坡口尺寸等) wht 18-08-09
			line.startPt=line.endPt;
		}
		pPrevVertex=pVertex;
		if(bFinal)
			break;
	}
	//绘制火曲线
	if(pPlate->IsZhiWan())
	{	
		f3dLine huoquline;
		bool bZhiWanStart=false,bZhiWanEnd=false;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pVertex->vertex.feature<=10)
				continue;
			if(!bZhiWanStart)
			{
				huoquline.startPt=pVertex->vertex;
				bZhiWanStart=true;
			}
			else if(!bZhiWanEnd)
			{
				huoquline.endPt=pVertex->vertex;
				bZhiWanEnd=true;
			}
		}
		if(bZhiWanStart&&bZhiWanEnd)
		{
			huoquline.pen.style=PS_DASHDOTDOT;
			edge_arr.Add(LineToArc(huoquline));
		}
		//判断火曲方式
		f3dPoint prj_base_pick,base_vec;
		SnapPerp(&prj_base_pick,huoquline,base_face_pick);
		base_vec=base_face_pick-prj_base_pick;
		normalize(base_vec);
		f3dPoint norm = pPlate->HuoquFaceNorm();
		vector_trans(norm,pPlate->ucs,FALSE);
		BOOL bNeiQu=TRUE;
		double dd = cal_angle_of_2vec(base_vec,norm);
		if(dd<Pi/2||(fabs(dd-Pi/2)<EPS&&norm.z>0))	//火曲面向外曲(正曲)
			bNeiQu=FALSE;
		m_ciHuoQuType=bNeiQu?1:2;
		//标注钢板火曲工艺字符串 wht 11-05-19
		TXTDIM_STRU txt_dim;
		txt_dim.txtHeight=sys.fonts.plate.fDimTextSize;	//火曲工艺内容字体高度独立设置
		txt_dim.dimPos=0.5*(huoquline.startPt+huoquline.endPt);
		txt_dim.dimVec=huoquline.endPt-huoquline.startPt;
		double huoquangle=cal_angle_of_2vec(pPlate->HuoquFaceNorm(),pPlate->ucs.axis_z);
		GetPlateHuoQuProcessStr(txt_dim,bNeiQu,huoquangle*DEGTORAD_COEF,pPlate->m_fDeformCoef,pPlate->GetThick());
		double fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
		normalize(txt_dim.dimVec);
		txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
		text_dim_arr.Add(txt_dim);
	}
	//绘制环向板内圆
	f3dPoint origin;	//圆柱法线方向
	if(bAdjustPlateUcs)
	{	//标注相对坐标
		coord_trans(origin,old_ucs,TRUE);
		coord_trans(origin,new_ucs,FALSE);
	}
	f3dPoint start_vec=(arc_line_start-origin).normalized();
	f3dPoint end_vec=(arc_line_end-origin).normalized();
	f3dPoint mid_vec=(start_vec+end_vec).normalized();
	f3dPoint col_axis;
	double inner_r=0;			//内圆半径
	BOOL bHasInnerCir=pPlate->GetInnerCirRadius(inner_r,col_axis);
	if(bHasInnerCir)
	{
		f3dPoint inner_origin=origin;
		if(fabs(col_axis*pPlate->ucs.axis_z)<EPS_COS)
		{	//椭圆
			f3dPoint workNorm(0,0,1),columnNorm=col_axis;
			if(bAdjustPlateUcs)
				vector_trans(columnNorm,new_ucs,FALSE);
			else
				vector_trans(columnNorm,pPlate->ucs,FALSE);
			f3dPoint minorAxis=columnNorm^workNorm;	
			normalize(minorAxis);		//椭圆短轴方向
			//顺时针旋转90度 wht 11-05-12
			f3dPoint majorAxis(minorAxis.y,-minorAxis.x,minorAxis.z);	
			normalize(majorAxis);		//椭圆长轴方向
			double radiusRatio=fabs(columnNorm*workNorm);
			double minorRadius=inner_r;				//椭圆短半轴长度
			double majorRadius=inner_r/radiusRatio;	//椭圆长半轴长度
			f3dPoint start=inner_origin+majorAxis*majorRadius;
			f3dPoint end = inner_origin+minorAxis*minorRadius;
			arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,inner_r);
			DrawPlateEllipseLine(pPlate,arcline,FALSE);
			edge_arr.Add(arcline);
			start=end;
			end=inner_origin-majorAxis*majorRadius;
			arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,inner_r);
			DrawPlateEllipseLine(pPlate,arcline,FALSE);
			edge_arr.Add(arcline);
			start=end;
			end = inner_origin-minorAxis*minorRadius;
			arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,inner_r);
			DrawPlateEllipseLine(pPlate,arcline,FALSE);
			edge_arr.Add(arcline);
			start=end;
			end=inner_origin+majorAxis*majorRadius;
			arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,inner_r);
			DrawPlateEllipseLine(pPlate,arcline,FALSE);
			edge_arr.Add(arcline);
			//标注长轴半径 
			ROTATEDIM_STRU rotate_dim;
			rotate_dim.startPos=inner_origin;
			rotate_dim.endPos=inner_origin+majorAxis*majorRadius;
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dist = majorRadius;	//长轴半径
			rotate_dim.angle=Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.bCanTextOutter=TRUE;
			rotate_dim_arr.Add(rotate_dim);
			rotate_dim.bCanTextOutter=FALSE;
			//标注短轴半径 
			rotate_dim.startPos=inner_origin;
			rotate_dim.endPos=inner_origin+minorAxis*minorRadius;
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dist = minorRadius;
			rotate_dim.angle=Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.bCanTextOutter=TRUE;
			rotate_dim_arr.Add(rotate_dim);
			rotate_dim.bCanTextOutter=FALSE;
			//内圆环为椭圆是需要绘图截面图 标注椭圆长轴长度 wht 10-01-21
			double plate_len=pPlate->GetLength();
			double plate_thick=pPlate->GetThick();
			double tana=tan(radiusRatio);
			double offset_dist=tana*plate_thick;	
			f3dPoint offset_origin(0,-plate_len);	//具体值由经验确定
			line.pen.style=PS_SOLID;
			line.ID=0;
			//指定坐标系时 不绘制截面图 wht 10-08-31
			if(!m_bSpecifyUCS)
			{	//环向板截面图
				line.startPt.Set(-0.5*plate_len,-0.5*plate_thick+offset_origin.y);
				line.endPt.Set(-0.5*plate_len,0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(0.5*plate_len,0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(0.5*plate_len,-0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				line.startPt=line.endPt;
				line.endPt.Set(-0.5*plate_len,-0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				//标注椭圆长轴直径
				rotate_dim.startPos.Set(-(majorRadius-0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
				rotate_dim.endPos.Set(majorRadius+0.5*offset_dist,-0.5*plate_thick+offset_origin.y);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dimPos.y-=15;
				rotate_dim.dist=majorRadius*2;
				rotate_dim.angle = 0;
				rotate_dim_arr.Add(rotate_dim);
				//椭圆弧截面线--右侧
				line.startPt.Set(majorRadius-0.5*offset_dist,0.5*plate_thick+offset_origin.y);
				line.endPt.Set(majorRadius+0.5*offset_dist,-0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				//椭圆弧截面线--左侧
				line.startPt.Set(-(majorRadius+0.5*offset_dist),0.5*plate_thick+offset_origin.y);
				line.endPt.Set(-(majorRadius-0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
				edge_arr.Add(LineToArc(line));
				//标注偏移值
				rotate_dim.startPos.Set(-(majorRadius+0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
				rotate_dim.endPos.Set(-(majorRadius-0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dimPos.y-=15;
				rotate_dim.dist=offset_dist;
				rotate_dim.angle = 0;
				rotate_dim_arr.Add(rotate_dim);
			}
		}
		else
		{	//圆
			f3dPoint startPt(inner_origin.x,inner_origin.y+inner_r);
			arcline.CreateMethod1(inner_origin,startPt,f3dPoint(0,0,1),2*Pi);
			edge_arr.Add(arcline);
			//标注内圆直径
			if(!m_bSpecifyUCS)
			{	
				f3dPoint dim_vec=(mid_vec+end_vec).normalized();
				if(dim_vec.IsZero())
					dim_vec.Set(1,0,0);
				DIAMETERDIM_STRU diam_dim;
				diam_dim.dist=arcline.Radius();
				diam_dim.leaderLength=5;
				diam_dim.bDimRadial=TRUE;
				diam_dim.farChordPos=inner_origin;
				diam_dim.chordPos=inner_origin+dim_vec*arcline.Radius();
				diameter_dim_arr.Add(diam_dim);
			}
		}
	}
	//构件图中需要绘制钢管横截面
	if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
	{	
		arcline.pen.style=PS_DASH;	//隐藏线
		if(fabs(pPlate->ucs.axis_z*tube_vec)<EPS_COS)
		{	//绘图钢管椭圆截面图
			f3dPoint worknorm(0,0,1),columnnorm=tube_vec;
			vector_trans(columnnorm,pPlate->ucs,FALSE);
			f3dPoint majorAxis(columnnorm.x,columnnorm.y);
			normalize(majorAxis);
			f3dPoint start,end;
			for(int i=0;i<2;i++)
			{
				double radius=tube_r;
				if(i==1)	//绘制内圆
					radius=tube_r-tube_t;
				start=origin+majorAxis*radius;
				end = origin+f3dPoint(-majorAxis.y*radius,majorAxis.x*radius);
				arcline.CreateEllipse(origin,start,end,columnnorm,f3dPoint(0,0,1),radius);
				edge_arr.Add(arcline);
				start*=-1.0;
				arcline.CreateEllipse(origin,end,start,columnnorm,f3dPoint(0,0,1),radius);
				edge_arr.Add(arcline);
				end*=-1.0;
				arcline.CreateEllipse(origin,start,end,columnnorm,f3dPoint(0,0,1),radius);
				edge_arr.Add(arcline);
				start*=-1.0;
				arcline.CreateEllipse(origin,end,start,columnnorm,f3dPoint(0,0,1),radius);
				edge_arr.Add(arcline);
			}
		}
		else
		{
			arcline.CreateMethod1(origin,f3dPoint(origin.x,origin.y+tube_r-tube_t),f3dPoint(0,0,1),2*Pi);
			edge_arr.Add(arcline);
			arcline.CreateMethod1(origin,f3dPoint(origin.x,origin.y+tube_r),f3dPoint(0,0,1),2*Pi);
			arcline.feature=pDatumTube->handle;
			edge_arr.Add(arcline);
		}
		//标注钢管半径标注
		f3dPoint dim_vec=(mid_vec+start_vec).normalized();
		if(dim_vec.IsZero())
			dim_vec.Set(0,1,0);
		if(!bHasInnerCir)
			RotateVectorAroundVector(dim_vec,-Pi/2,f3dPoint(0,0,1));
		DIAMETERDIM_STRU diam_dim;
		diam_dim.dist=tube_r;
		diam_dim.leaderLength=5;
		diam_dim.bDimRadial=TRUE;
		diam_dim.farChordPos=origin;
		diam_dim.chordPos=origin+dim_vec*tube_r;
		diameter_dim_arr.Add(diam_dim);
		//绘制塔心方向线 
		if(pDatumTube->m_bHasWeldRoad)
		{
			double startAngle=pDatumTube->CalWeldRoadAngle();
			f3dPoint vec,dim_vec(cos(startAngle),sin(startAngle));
			vector_trans(dim_vec,pDatumTube->ucs,TRUE);
			vector_trans(dim_vec,pPlate->ucs,FALSE);
			normalize(dim_vec);
			dim_vec.z=0;
			//箭头线
			line.startPt=origin;
			line.endPt=origin+dim_vec*(tube_r+100);
			line.pen.style=PS_SOLID;
			edge_arr.Add(LineToArc(line));
			line.startPt=line.endPt;
			vec=dim_vec;
			RotateVectorAroundVector(vec,-Pi/12,f3dPoint(0,0,1));
			line.endPt=line.startPt-vec*20;
			edge_arr.Add(LineToArc(line));
			RotateVectorAroundVector(vec,Pi/6,f3dPoint(0,0,1));
			line.endPt=line.startPt-vec*20;
			edge_arr.Add(LineToArc(line));
			//
			TXTDIM_STRU txt_dim;
			txt_dim.dimVec=dim_vec;
			txt_dim.dimPos=origin+dim_vec*(tube_r+20);
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(txt_dim.dimText,"weld road(tower core)direction");
#else
			strcpy(txt_dim.dimText,"焊道(塔心)方向");
#endif
			text_dim_arr.Add(txt_dim);
		}
	}
	//构件图中绘制依赖钢板俯视图和标注尺寸
	if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
	{
		ANGULARDIM_STRU angular_dim;//角度标注
		ROTATEDIM_STRU rotate_dim;
		DIAMETERDIM_STRU diam_dim;
		rotate_dim.bDisplayDimLine=TRUE;
		double B=0,D=0,H=0,V=0,N=0,W=0,BH=-1;
		pPlate->GetDesignItemValue('B',	&B);
		pPlate->GetDesignItemValue('D', &D);
		pPlate->GetDesignItemValue('H',	&H);
		pPlate->GetDesignItemValue('V',	&V);
		pPlate->GetDesignItemValue('W', &W);
		pPlate->GetDesignItemValue('N', &N);
		pPlate->GetDesignItemValue(KEY2C("BH"),&BH);
		if(pPlate->m_iParamType==TYPE_TUBERIBPLATE||pPlate->m_iParamType==TYPE_CIRRIBPLATE)
		{	//标注钢管肋板的 W、N
			double thick_w=pPlate1?pPlate1->GetThick():0;
			double thick_n=pPlate2?pPlate2->GetThick():thick_w;
			int iCur=0,num=vertex_list.GetNodeNum();
			while(1)
			{
				for(iCur=0;iCur<num;iCur++)
				{
					if(vertex_list[iCur].weldline.hWeldMotherPart>0X20)
						break;
				}
				if(iCur<0||iCur>=num)
					break;
				f3dPoint ptS=vertex_list[iCur].vertex;
				f3dPoint ptE=vertex_list[(iCur+1)%num].vertex;
				f3dPoint axis_v=(ptE-ptS).normalized(),vertice_vec=(ptE-origin).normalized();
				f3dPoint axis_z(axis_v.y,-axis_v.x);
				double fCutR=0;
				if(fabs(BH)<EPS)
					fCutR=(vertex_list[iCur].weldline.hWeldMotherPart==pPlate1->handle)?rela_part1.uiAttribute*0.1:rela_part2.uiAttribute*0.1;
				if(fCutR>0)
				{
					if(vertex_list[(iCur-1+num)%num].radius==-fCutR)
						ptS=vertex_list[(iCur-1+num)%num].center;
					else if(vertex_list[(iCur+1)%num].radius==-fCutR)
						ptE=vertex_list[(iCur+1)%num].center;
				}
				if(axis_v*vertice_vec<0)
				{
					f3dPoint pt=ptS;ptS=ptE;ptE=pt;
					axis_v*=-1;
				}
				double thick=(vertex_list[iCur].weldline.hWeldMotherPart==pPlate1->handle)?thick_w:thick_n;
				double fDist=(vertex_list[iCur].weldline.hWeldMotherPart==pPlate1->handle)?W:N;
				vertex_list[iCur].weldline.hWeldMotherPart=0;
				//标注距离
				rotate_dim.startPos=ptS;
				rotate_dim.endPos  =ptE;
				rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5+axis_z*(sys.fonts.plate.fDimTextSize*2+thick);
				rotate_dim.dist=fDist;
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
				rotate_dim_arr.Add(rotate_dim);
				//钢板示意图
				line.startPt=ptS;
				line.endPt  =ptE;
				line.pen.style=PS_SOLID;
				edge_arr.Add(LineToArc(line));
				line.startPt=ptS+axis_z*thick;
				line.endPt  =ptE+axis_z*thick;
				line.pen.style=PS_SOLID;
				edge_arr.Add(LineToArc(line));
				line.startPt=ptS+axis_z*thick*0.5;
				line.endPt  =ptE+axis_z*thick*0.5;
				line.pen.style=PS_DASHDOT;
				edge_arr.Add(LineToArc(line));
				//标注半个钢板的厚度
				rotate_dim.startPos=line.endPt;
				rotate_dim.endPos=ptE;
				rotate_dim.dist=0.5*thick;
				rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5+axis_v*sys.fonts.plate.fDimTextSize;
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
				rotate_dim_arr.Add(rotate_dim);
			}
			if(pPlate->GetCircularPlateSubType()==CLDSParamPlate::TYPE_SPECIAL_CIR_PLATE)
			{	//异性法兰标注外径
				double out_r=tube_r+V+D;
				f3dPoint dim_vec=mid_vec;
				diam_dim.dist=out_r;
				diam_dim.leaderLength=5;
				diam_dim.bDimRadial=TRUE;
				diam_dim.farChordPos=origin;
				diam_dim.chordPos=origin+dim_vec*out_r;
				diameter_dim_arr.Add(diam_dim);
			}
		}
		else if(pPlate->m_iParamType==TYPE_CIRCOVERPLATE)
		{	
			double R=0;
			pPlate->GetDesignItemValue('R',&R);
			long Y=0;
			pPlate->GetDesignItemValue('Y',&Y);	//封板类型
			line.startPt=origin;
			line.endPt=arc_line_start;
			line.pen.style=PS_DASHDOT;
			edge_arr.Add(LineToArc(line));
			line.endPt=arc_line_end;
			edge_arr.Add(LineToArc(line));
			if(Y==0)
			{	//普通封板
				//标注'W'尺寸
				f3dPoint axis_x=old_ucs.axis_x;
				vector_trans(axis_x,pPlate->ucs,FALSE);
				axis_x.z=0;
				rotate_dim.startPos=origin+axis_x*R;
				rotate_dim.endPos=origin+axis_x*(R+W);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dimPos.y-=15;
				rotate_dim.dist=W;
				rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
				rotate_dim_arr.Add(rotate_dim);
				//标注'D'尺寸
				if(D>0 && D<W)
				{
					B*=RADTODEG_COEF;
					for(int i=0;i<2;i++)
					{
						f3dPoint off_vec,vec=axis_x;
						if(i==0)
						{
							RotateVectorAroundVector(vec,0.5*B,f3dPoint(0,0,1));
							off_vec.Set(-vec.y,vec.x);
						}
						else
						{
							RotateVectorAroundVector(vec,-0.5*B,f3dPoint(0,0,1));
							off_vec.Set(vec.y,-vec.x);
						}
						rotate_dim.startPos=origin+vec*R;
						rotate_dim.endPos=origin+vec*(R+D);
						rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
						rotate_dim.dimPos+=off_vec*11;
						rotate_dim.dist=D;
						rotate_dim.angle = Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
						rotate_dim_arr.Add(rotate_dim);
					}
				}
			}
			else
			{	//简化封板

			}
		}
	}
	else if(bPatternDrawing)
		CreateRelativePlateInCirParamPlate(pPlate,bPatternDrawing,origin);
	//绘制螺栓孔或图符(根据刘伟要求，环向板上的孔不按照特殊孔处理)
	f3dCircle ls_cir;
	for(CLDSPart::HOLE* pHole=hole_list.EnumObjectFirst();pHole;pHole=hole_list.EnumObjectNext())
	{
		ls_cir.centre=pHole->xLocalPosition;
		if(bAdjustPlateUcs)
		{
			coord_trans(ls_cir.centre,old_ucs,TRUE);
			coord_trans(ls_cir.centre,new_ucs,FALSE);
		}
		ls_cir.centre.z=0;
		if(pHole->pBolt)
		{
			CLsRef* pLsRef=pPlate->FindLsByHandle(pHole->pBolt->handle);
			ls_cir.radius=pHole->pBolt->get_d()*0.5;
			ls_cir.feature=ftoi(pHole->pBolt->hole_d_increment*10);
			//ls_cir.pen.style=pHole->pBolt->FilterFuncType();	//记录螺栓孔的功用类型
			//ls_cir.centre.pen.style=pLsRef->cFlag;				//记录螺栓孔的工艺类型
		}
		else
		{	//根据刘伟要求，环向板上的孔不按照特殊孔处理
			ls_cir.radius = pHole->wBoltD*0.5;
			ls_cir.feature=ftoi(pHole->fHoleMargin*10);
			//ls_cir.pen.style=CLDSBolt::FUNC_WATERHOLE;			//杂项孔的功用类型默认为镀锌引流孔
		}
		//标注螺栓相对坐标值	wht 09-09-16
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
			DimCoordValue(pPlate,ls_cir.centre);
		if((int)pHole->wWaistLen>0&&pHole->pBolt)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width=(int)pHole->wWaistLen;		//使用画笔宽度保存腰圆孔长度
			ls_cir.norm=pHole->pBolt->ucs.axis_x;		//使用螺栓孔法线方向保存腰圆方向
			vector_trans(ls_cir.norm,pPlate->ucs,FALSE);//转换到钢板坐标系下
		}
		cir_arr.Add(ls_cir);
	}
	if(bPatternDrawing)//大样图中需要在焊烧死角处切弧
		CreateLineOnWeldCorner(pPlate,&vertex_list);
	//大样图中绘制钢板的内侧轮廓边
	if(sys.part_map.plate.bDrawInnerEdge)
		DrawInnerEdge(pPlate,&vertex_list);
	//青岛东方要求大样图中不绘制焊接子构件
	if(!(gxDrwLocalizer.ciCustomerSerial==5&&bPatternDrawing))
		DrawWeldSubPlate(pPlate,bAdjustPlateUcs);
	if(sys.part_map.plate.bDimPlateRayLs)
		DimLsRay(pPlate,bAdjustPlateUcs);
	//恢复板坐标系
	if(bAdjustPlateUcs)
		pPlate->ucs=old_ucs;
	return TRUE;
}
BOOL CPlankDrawing::CreateWaterShieldedPlate(CLDSParamPlate *pParamPlate, BOOL bPatternDrawing/*=TRUE*/)
{
	if(pParamPlate==NULL||pParamPlate->m_iParamType!=TYPE_WATERSHIELDEDPLATE)
		return FALSE;
	long C=0;
	pParamPlate->GetDesignItemValue('C', &C);	//基准钢管句柄
	CLDSLineTube *pDatumTube=(CLDSLineTube*)pParamPlate->BelongModel()->FromPartHandle(C,CLS_LINETUBE);
	if(pDatumTube==NULL)
		return FALSE;
	pDatumTube->BuildUCS();
	//初始化遮水板设计参数
	double A=0,H=0,L=0,W=0,O=0,K=0;
	pParamPlate->GetDesignItemValue('W', &W);	//外圆直径
	pParamPlate->GetDesignItemValue('K', &K);	//流水槽宽度
	pParamPlate->GetDesignItemValue('A', &A);	//两流水槽之间夹角
	pParamPlate->GetDesignItemValue('H', &H);	//流水槽深度
	pParamPlate->GetDesignItemValue('O', &O);	//两流水槽夹角的平分线与基准边之间的夹角
	pParamPlate->GetDesignItemValue('L', &L);	//流水槽长度
	double R=W*0.5;
	BOOL bHasWaterSolt=TRUE;
	if(fabs(A)<EPS||fabs(H)<EPS||fabs(K)<EPS||fabs(L)<EPS)
		bHasWaterSolt=FALSE;	//两流水槽夹角、长度、深度或宽度为0时表示无流水槽 wht 11-07-01
	//调整绘图坐标
	f3dPoint origin,axisX(1,0,0),axisY(0,1,0);
	if(m_bSpecifyUCS)
	{	
		coord_trans(origin,pParamPlate->ucs,TRUE);
		coord_trans(origin,new_ucs,FALSE);
		vector_trans(axisX,pParamPlate->ucs,TRUE);
		vector_trans(axisX,new_ucs,FALSE);
		vector_trans(axisY,pParamPlate->ucs,TRUE);
		vector_trans(axisY,new_ucs,FALSE);
	}
	long_edge_mid=origin-axisY*0.3*R;
	//绘制轮廓边
	f3dArcLine arcline;
	arcline.CreateMethod1(origin,origin+axisX*R,f3dPoint(0,0,1),2*Pi);
	arcline.feature=pParamPlate->handle;
	edge_arr.Add(arcline);
	//标注外圆直径
	DIAMETERDIM_STRU diam_dim;
	diam_dim.chordPos=origin+axisY*R;
	diam_dim.farChordPos=origin-axisY*R;
	diam_dim.dist=W;
	diam_dim.leaderLength=5;
	diameter_dim_arr.Add(diam_dim);
	//坐标轴线
	f3dLine line,axis_line(origin-axisX*R,origin+axisX*R),tem_line;
	line.startPt=origin-axisX*(R+30);
	line.endPt	=origin+axisX*(R+30);
	line.pen.style=PS_DASHDOT;	
	edge_arr.Add(LineToArc(line));
	line.startPt=origin+axisY*(R+30);
	line.endPt	=origin-axisY*(R+30);
	edge_arr.Add(LineToArc(line));
	//流水槽轮廓边
	if(bHasWaterSolt)
	{	
		double fAngle=A*RADTODEG_COEF*0.5;//两流水槽之间夹角的一半
		f3dPoint up_axis=axisX,up_norm;
		RotateVectorAroundVector(up_axis,fAngle,f3dPoint(0,0,1));
		up_norm.Set(-up_axis.y,up_axis.x);
		f3dPoint dw_axis=axisX,dw_norm;
		RotateVectorAroundVector(dw_axis,-fAngle,f3dPoint(0,0,1));
		dw_norm.Set(dw_axis.y,-dw_axis.x);
		if(up_axis.y<dw_axis.y)
		{	
			f3dPoint temp_vec;
			temp_vec=up_axis;up_axis=dw_axis;dw_axis=temp_vec;
			temp_vec=up_norm;up_norm=dw_norm;dw_norm=temp_vec;
		}
		f2dCircle cir(R,origin.x,origin.y);
		f3dPoint pt_S,pt_E,intersPt1,intersPt2;
		//Y+流水槽
		//轴线
		line.startPt=pt_S=origin;
		line.endPt	=pt_E=origin+up_axis*R;
		line.pen.style=PS_DASHDOT;
		edge_arr.Add(LineToArc(line));
		//外侧线
		line.startPt=pt_S+up_norm*K*0.5;
		line.endPt	=pt_E+up_norm*K*0.5;
		Int3dll(line,axis_line,intersPt1);
		line.startPt=intersPt1;
		f2dLine line2d1(f2dPoint(line.startPt.x,line.startPt.y),f2dPoint(line.endPt.x,line.endPt.y));
		Int2dlc(line2d1,cir,intersPt1.x,intersPt1.y,intersPt2.x,intersPt2.y);
		if(intersPt1.y>intersPt2.y)
			line.endPt=intersPt1;
		else
			line.endPt=intersPt2;
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
		//内侧线
		line.startPt=pt_S-up_norm*K*0.5;
		line.endPt	=pt_E-up_norm*K*0.5;
		Int3dll(line,axis_line,intersPt1);
		line.startPt=intersPt1;
		f2dLine line2d2(f2dPoint(line.startPt.x,line.startPt.y),f2dPoint(line.endPt.x,line.endPt.y));
		Int2dlc(line2d2,cir,intersPt1.x,intersPt1.y,intersPt2.x,intersPt2.y);
		if(intersPt1.y>intersPt2.y)
			line.endPt=intersPt1;
		else
			line.endPt=intersPt2;
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
		//Y-流水槽
		//轴线
		line.startPt=pt_S=origin;
		line.endPt	=pt_E=origin+dw_axis*R;
		line.pen.style=PS_DASHDOT;
		edge_arr.Add(LineToArc(line));
		//外侧线
		line.startPt=pt_S+dw_norm*K*0.5;
		line.endPt	=pt_E+dw_norm*K*0.5;
		Int3dll(line,axis_line,intersPt1);
		line.startPt=intersPt1;
		f2dLine line2d3(f2dPoint(line.startPt.x,line.startPt.y),f2dPoint(line.endPt.x,line.endPt.y));
		Int2dlc(line2d3,cir,intersPt1.x,intersPt1.y,intersPt2.x,intersPt2.y);
		if(intersPt1.y>intersPt2.y)
			line.endPt=intersPt2;
		else
			line.endPt=intersPt1;
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
		//内侧线
		line.startPt=pt_S-dw_norm*K*0.5;
		line.endPt	=pt_E-dw_norm*K*0.5;
		Int3dll(line,axis_line,intersPt1);
		line.startPt=intersPt1;
		f2dLine line2d4(f2dPoint(line.startPt.x,line.startPt.y),f2dPoint(line.endPt.x,line.endPt.y));
		Int2dlc(line2d4,cir,intersPt1.x,intersPt1.y,intersPt2.x,intersPt2.y);
		if(intersPt1.y>intersPt2.y)
			line.endPt=intersPt2;
		else
			line.endPt=intersPt1;
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
		//标注两流水槽之间的夹角
		ANGULARDIM_STRU angular_dim;
		angular_dim.vertex	=origin;
		angular_dim.startPos=origin+up_axis*0.6*R;
		angular_dim.endPos	=origin+dw_axis*0.6*R;
		angular_dim.dimPos	=origin+axisX*0.6*R;
		angular_dim_arr.Add(angular_dim);
	}
	//绘制钢管特征线
	if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord)
	{
		long E=0;
		pParamPlate->GetDesignItemValue('E', &E);	//基准边标志 0.基准边 1.焊道线
		double offset_angle=0;
		if(E==1&&pDatumTube->m_bHasWeldRoad)	//以焊道线为基准
			offset_angle +=pDatumTube->CalWeldRoadAngle();
		f3dPoint datum_axis=pDatumTube->ucs.axis_x;
		f3dPoint tube_axis=(pDatumTube->End()-pDatumTube->Start()).normalized();
		RotateVectorAroundVector(datum_axis,offset_angle,tube_axis);
		if(m_bSpecifyUCS)
			vector_trans(datum_axis,new_ucs,FALSE);
		else
			vector_trans(datum_axis,pParamPlate->ucs,FALSE);
		normalize(datum_axis);
		datum_axis.z=0;
		//箭头线
		line.startPt=origin;
		line.endPt=origin+datum_axis*(R+100);
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		f3dPoint vec=datum_axis;
		RotateVectorAroundVector(vec,-Pi/12,f3dPoint(0,0,1));
		line.endPt=line.startPt-vec*20;
		edge_arr.Add(LineToArc(line));
		RotateVectorAroundVector(vec,Pi/6,f3dPoint(0,0,1));
		line.endPt=line.startPt-vec*20;
		edge_arr.Add(LineToArc(line));
		//
		TXTDIM_STRU txt_dim;
		txt_dim.dimVec=datum_axis;
		txt_dim.dimPos=origin+datum_axis*(R+50);
#ifdef AFX_TARG_ENU_ENGLISH
		if(E==1&&pDatumTube->m_bHasWeldRoad)
			strcpy(txt_dim.dimText,"weld road(tower core)direction");
		else
			strcpy(txt_dim.dimText,"Datum Line Vector");
#else
		if(E==1&&pDatumTube->m_bHasWeldRoad)
			strcpy(txt_dim.dimText,"焊道(塔心)方向");
		else
			strcpy(txt_dim.dimText,"基准边方向");
#endif
		text_dim_arr.Add(txt_dim);
		//标注两流水槽角平分线与基准边之间夹角
		if(O>0) 
		{	
			f3dPoint vec=(axisX+datum_axis).normalized();
			ANGULARDIM_STRU angular_dim;
			angular_dim.vertex=origin;
			angular_dim.startPos=origin+axisX*(R+20);
			angular_dim.endPos	=origin+datum_axis*(R+20);
			angular_dim.dimPos	=origin+vec*(R+20);
			angular_dim_arr.Add(angular_dim);
		}
	}
	//绘制流水槽侧立剖面放置图
	if(!bPatternDrawing&&!sys.part_map.plate.bDimCoord&&bHasWaterSolt)
	{	//绘制流水槽截面
		double T=pParamPlate->GetThick();
		f3dPoint offset_origin=origin+axisY*(R+30+2*T),vecX(1,0,0),vecY(0,1,0);
		line.startPt=offset_origin-vecX*R;
		line.endPt	=offset_origin+vecX*R;
		line.pen.style=PS_SOLID;
		edge_arr.Add(LineToArc(line));
		line.startPt=offset_origin-vecX*R;
		line.endPt	=line.startPt+vecY*T;
		edge_arr.Add(LineToArc(line));
		line.startPt=offset_origin+vecX*R;
		line.endPt	=line.startPt+vecY*T;
		edge_arr.Add(LineToArc(line));
		//X-侧流水槽
		line.startPt=offset_origin-vecX*R+vecY*T;
		line.endPt	=offset_origin-vecX*0.5*R+vecY*T;
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt	=line.startPt-vecY*H;
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt	=line.startPt+vecX*K;
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt	=line.startPt+vecY*H;
		edge_arr.Add(LineToArc(line));
		//X+侧流水孔
		line.startPt=line.endPt;
		line.endPt	=offset_origin+vecX*(0.5*R-K)+vecY*T;
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt	=line.startPt-vecY*H;
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt	=line.startPt+vecX*K;
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt	=line.startPt+vecY*H;
		edge_arr.Add(LineToArc(line));
		line.startPt=line.endPt;
		line.endPt	=offset_origin+vecX*R+vecY*T;
		edge_arr.Add(LineToArc(line));
		//标注流水槽深度H
		double fTestH=sys.fonts.plate.fDimTextSize;
		ROTATEDIM_STRU rotate_dim;
		rotate_dim.startPos=offset_origin-vecX*0.5*R+vecY*T;
		rotate_dim.endPos=rotate_dim.startPos-vecY*H;
		rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos)+vecX*1.5*fTestH;
		rotate_dim.dist = H;	
		rotate_dim.angle=-Pi/2;
		rotate_dim.bDisplayDimLine=TRUE;
		rotate_dim.bCanTextOutter=TRUE;
		rotate_dim_arr.Add(rotate_dim);
		//标注遮水板厚度T
		rotate_dim.startPos=offset_origin+vecX*R;
		rotate_dim.endPos=offset_origin+vecX*R+vecY*T;
		rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos)+vecX*1.5*fTestH;;
		rotate_dim.dist = T;	
		rotate_dim.angle=-Pi/2;
		rotate_dim.bDisplayDimLine=TRUE;
		rotate_dim.bCanTextOutter=TRUE;
		rotate_dim_arr.Add(rotate_dim);
		//标注遮水板厚度K
		rotate_dim.startPos=offset_origin+vecX*0.5*R+vecY*T;
		rotate_dim.endPos=offset_origin+vecX*(0.5*R-K)+vecY*T;
		rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
		rotate_dim.dist = K;	
		rotate_dim.angle = 0;
		rotate_dim.bDisplayDimLine=TRUE;
		rotate_dim.bCanTextOutter=TRUE;
		rotate_dim_arr.Add(rotate_dim);
	}	
	//绘制螺栓孔或图符
	double R_bolt=0,new_R=0;
	diam_dim.dist=0;
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{
		f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
		project_point(ls_pos,pParamPlate->ucs.origin,pParamPlate->ucs.axis_z);
		if(m_bSpecifyUCS)
			coord_trans(ls_pos,new_ucs,FALSE);
		else 
			coord_trans(ls_pos,pParamPlate->ucs,FALSE);
		ls_pos.z=0;
		new_R=ls_pos.mod();
		//
		f3dCircle ls_cir;
		ls_cir.centre=ls_pos;
		ls_cir.radius=(*pLsRef)->get_d()/2.0;
		ls_cir.feature=(int)((*pLsRef)->hole_d_increment*10);
		ls_cir.pen.style=(*pLsRef)->FilterFuncType();	//记录螺栓孔的功用类型
		ls_cir.centre.pen.style=pLsRef->cFlag;			//记录螺栓孔的工艺类型
		if((int)pLsRef->waistLen>0)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width=(int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度
			ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
			vector_trans(ls_cir.norm,pParamPlate->ucs,FALSE);//转换到钢板坐标系下
		}
		cir_arr.Add(ls_cir);
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.4.2
			DimCoordValue(pParamPlate,ls_cir.centre);
		//绘制螺栓布置圆弧
		if(fabs(R_bolt-new_R)>1)
		{	//设置初始螺栓距轴心距离，并绘制螺栓布置圆弧
			R_bolt=new_R;
			arcline.CreateMethod1(origin,origin+axisX*R_bolt,f3dPoint(0,0,1),2*Pi);
			arcline.pen.style=PS_DASHDOT;
			edge_arr.Add(arcline);
			//螺栓布置圆直径尺寸标注
			diam_dim.dist=ftoi(R_bolt*2);
			diam_dim.chordPos=ls_pos;
			diam_dim.farChordPos=-diam_dim.chordPos;
			diam_dim.leaderLength=5;
			diameter_dim_arr.Add(diam_dim);
		}
	}
	return TRUE;
}

//根据卷边高度以及卷边外移量绘制卷边相关线段 wht 11-04-09
void  CPlankDrawing::DrawRollEdgeLine(PROFILE_VERTEX *pStartVertex,PROFILE_VERTEX *pEndVertex,
			PROFILE_VERTEX *pPreVertex,PROFILE_VERTEX *pNextVertex,CLDSPlate *pPlate,
			f3dLine &roll_edge_line,f3dLine &huoqu_line,BOOL bPatternDrawing)
{
	f3dLine line;
	pPlate->CalRollLineAndHuoquLine(pStartVertex,pEndVertex,pPreVertex,pNextVertex,huoqu_line,roll_edge_line);
	line.startPt = pStartVertex->vertex;
	line.endPt   = pEndVertex->vertex;
	line.startPt.z=line.endPt.z=0;
	//更新前轮廓边的终点坐标并添加新生成的轮廓边
	f3dArcLine arcline;
	arcline.pen.style=PS_SOLID;
	if(edge_arr.GetSize()>0&&pStartVertex->vertex.feature<10)
		edge_arr[edge_arr.GetSize()-1].SetEnd(huoqu_line.startPt);
	else if(pStartVertex->vertex.feature>10&&!line.startPt.IsEqual(huoqu_line.startPt))
	{	//在火曲线始端进行卷边处理，保留火曲点
		arcline.CreateMethod2(line.startPt,huoqu_line.startPt,f3dPoint(0,0,1),0);
		edge_arr.Add(arcline);
	}
	arcline.CreateMethod2(huoqu_line.startPt,roll_edge_line.startPt,f3dPoint(0,0,1),0);
	edge_arr.Add(arcline);
	arcline.CreateMethod2(roll_edge_line.startPt,roll_edge_line.endPt,f3dPoint(0,0,1),0);
	edge_arr.Add(arcline);
	arcline.CreateMethod2(roll_edge_line.endPt,huoqu_line.endPt,f3dPoint(0,0,1),0);	
	edge_arr.Add(arcline);
	if(pEndVertex->vertex.feature>10&&!line.endPt.IsEqual(huoqu_line.endPt))
	{	//在火曲线终端进行卷边处理，保留火曲点
		arcline.CreateMethod2(huoqu_line.endPt,line.endPt,f3dPoint(0,0,1),0);
		edge_arr.Add(arcline);
	}
	else
	{
		m_bPrevIsRollEdgeLine=true;
		m_xRollEdgeLineStart=huoqu_line.startPt;
		m_xRollEdgeLineEnd=huoqu_line.endPt;
	}
	//
	if(!bPatternDrawing)
	{
		DimCoordValue(pPlate,roll_edge_line.startPt);
		DimCoordValue(pPlate,roll_edge_line.endPt);
	}
	//处理火曲线
	arcline.pen.style=PS_DASHDOTDOT;
	arcline.CreateMethod2(huoqu_line.startPt,huoqu_line.endPt,f3dPoint(0,0,1),0);
	edge_arr.Add(arcline);
	if(sys.part_map.plate.bMarkHuoquPos)
		MarkHuoquLine(huoqu_line);
	//添加火曲线方向的剖面图
	HUOQU_MARK huoquMark;
	f3dPoint intersPt;
	SnapPerp(&intersPt,huoqu_line,pPreVertex->vertex);
	f3dPoint base_vec=(pPreVertex->vertex-intersPt).normalized();
	f3dPoint huoqu_vec=(huoqu_line.endPt-huoqu_line.startPt).normalized();
	huoquMark.prj_vec=huoqu_vec;
	huoquMark.origin=0.5*(huoqu_line.startPt+huoqu_line.endPt);
	huoquMark.offset_dist=-10;
	huoquMark.offset_vec=base_vec;
	huoquMark.line_arr[0].startPt=base_vec;
	huoquMark.line_arr[0].endPt=base_vec+huoquMark.prj_vec*15.0;
	huoquMark.line_arr[1].startPt=huoquMark.line_arr[0].endPt;
	huoquMark.line_arr[1].endPt=huoquMark.line_arr[1].startPt+base_vec*2.0-huoquMark.prj_vec*2.0;
	huoquMark.line_arr[2].startPt=huoquMark.line_arr[1].endPt;
	huoquMark.line_arr[2].endPt=huoquMark.line_arr[2].startPt-base_vec*2.0-huoquMark.prj_vec*2.0;
	f3dPoint startPt=huoquMark.line_arr[0].endPt+huoquMark.prj_vec*5.0-base_vec;
	huoquMark.line_arr[3].startPt=startPt;
	huoquMark.line_arr[3].endPt=startPt+base_vec*15;
	huoquMark.line_arr[4].startPt=startPt;
	huoquMark.line_arr[4].endPt=startPt+base_vec*10;
	double fPlaneAngle=(m_ciHuoQuType==1)?Pi*0.5:-Pi*0.5;
	rotate_point_around_axis(huoquMark.line_arr[4].endPt,fPlaneAngle,f3dLine(startPt,startPt+f3dPoint(0,0,1)*10));
	huoquMark.plane_angle=fPlaneAngle;
	if(!bPatternDrawing&&sys.part_map.plate.bDrawHuoQuPrjMap)
		AddHuoQuPrjMap(huoqu_line,huoquMark);
	if(huoqu_mark[0].origin.IsZero())
		huoqu_mark[0]=huoquMark;
	else
		huoqu_mark[1]=huoquMark;
	//添加火曲线工艺内容标注
	m_ciHuoQuType=(pStartVertex->manu_space>0)?2:1;
	TXTDIM_STRU txt_dim;
	txt_dim.txtHeight=sys.fonts.plate.fDimTextSize;	//火曲工艺内容字体高度独立设置
	txt_dim.dimPos=(huoqu_line.startPt+huoqu_line.endPt)*0.5;
	char sHuoquType[50];
	if(ConfirmPlateHuoquType(pPlate->GetThick(),90))
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(sHuoquType,"(cold blending)");
	else
		strcpy(sHuoquType,"(hot blending)");
#else
		strcpy(sHuoquType,"(冷弯)");
	else
		strcpy(sHuoquType,"(热弯)");
#endif
	if(sys.part_map.bUseZhengFanQun)
	{	//正反曲
		if(pStartVertex->manu_space>0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(txt_dim.dimText,"front blending rolled edge 90°");
		else
			strcpy(txt_dim.dimText,"reverse blending rolled edge 90°");
#else
			strcpy(txt_dim.dimText,"正曲卷边90°");
		else
			strcpy(txt_dim.dimText,"反曲卷边90°");
#endif
	}
	else
	{	//外内曲
		if(pStartVertex->manu_space>0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(txt_dim.dimText,"outer blending rolled edge 90°");
		else
			strcpy(txt_dim.dimText,"inner blending rolled edge 90°");
	}
	if(sys.part_map.bDimPlaneAngle)	//mark the angle between the two blended plane
		strcat(txt_dim.dimText,"two plane angle 90°");
#else
			strcpy(txt_dim.dimText,"外曲卷边90°");
		else
			strcpy(txt_dim.dimText,"内曲卷边90°");
	}
	if(sys.part_map.bDimPlaneAngle)	//标注两火曲面之间的夹角
		strcat(txt_dim.dimText,"两面夹角90°");
#endif
	if(sys.part_map.bDimHuoquType)		//标注火曲类型 冷弯热弯
		strcat(txt_dim.dimText,sHuoquType);
	txt_dim.dimVec=(huoqu_line.endPt-huoqu_line.startPt).normalized();
	text_dim_arr.Add(txt_dim);
	//标注卷边尺寸
	f3dPoint roll_edge_vec=roll_edge_line.endPt-roll_edge_line.startPt;
	normalize(roll_edge_vec);
	f3dPoint norm(-roll_edge_vec.y,roll_edge_vec.x);
	ROTATEDIM_STRU rotate_dim;
	//不令标注居中是为了躲开大样图中默认文字标注位置重叠带来搜索空白位置算法效率低下 wjh-2015.11.11
	rotate_dim.startPos=roll_edge_line.startPt+(roll_edge_line.endPt-roll_edge_line.startPt)*0.1;
	Int3dpl(huoqu_line,f3dLine(rotate_dim.startPos,rotate_dim.startPos+norm*1000),rotate_dim.endPos);
	rotate_dim.dimPos=(rotate_dim.startPos+rotate_dim.endPos)*0.5;
	rotate_dim.angle=Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
	short nRollHeight=(short)ftol(DISTANCE(rotate_dim.startPos,rotate_dim.endPos));
	if(pStartVertex->manu_space>nRollHeight)
#ifdef AFX_TARG_ENU_ENGLISH
		rotate_dim.sDimText.Printf("rolled edge%d-%d",pStartVertex->manu_space,pStartVertex->manu_space-nRollHeight);
	else if(pStartVertex->manu_space<nRollHeight)
		rotate_dim.sDimText.Printf("rolled edge%d+%d",pStartVertex->manu_space,nRollHeight-pStartVertex->manu_space);
	else 
		rotate_dim.sDimText.Printf("rolled edge%d",pStartVertex->manu_space);
#else
		rotate_dim.sDimText.Printf("卷边 %d-%d",pStartVertex->manu_space,pStartVertex->manu_space-nRollHeight);
	else if(pStartVertex->manu_space<nRollHeight)
		rotate_dim.sDimText.Printf("卷边 %d+%d",pStartVertex->manu_space,nRollHeight-pStartVertex->manu_space);
	else 
		rotate_dim.sDimText.Printf("卷边 %d",pStartVertex->manu_space);
#endif
	rotate_dim.bDisplayDimLine=TRUE;
	rotate_dim.bCanTextOutter=FALSE;
	rotate_dim_arr.Add(rotate_dim);
}
//将椭圆弧段拆分成钢板底面的椭圆弧段+过渡平滑直线+钢板顶面椭圆弧段
void CPlankDrawing::DrawPlateEllipseLine(CLDSGeneralPlate* pPlate,f3dArcLine arcline,BOOL bOriInBtmPlane/*=TRUE*/)
{
	//TODO: 临时return，下次版本发布时经过完整测试可放开 wjh-2016.6.27
	return;
	//初始化椭圆信息
	f3dPoint startPt,endPt,center,col_axis,work_norm;
	startPt=arcline.Start();
	endPt=arcline.End();
	center=arcline.Center();
	col_axis=arcline.ColumnNorm();
	work_norm=arcline.WorkNorm();
	f3dPoint minorAxis,majorAxis;
	minorAxis=col_axis^work_norm;
	normalize(minorAxis);		//椭圆短轴方向
	majorAxis.Set(minorAxis.y,-minorAxis.x,minorAxis.z);	
	normalize(majorAxis);		//椭圆长轴方向
	UCS_STRU tem_ucs;
	tem_ucs.origin=center;
	tem_ucs.axis_x=majorAxis;
	tem_ucs.axis_y=minorAxis;
	tem_ucs.axis_z=majorAxis^minorAxis;
	coord_trans(startPt,tem_ucs,FALSE);
	coord_trans(endPt,tem_ucs,FALSE);
	double radiusRatio=fabs(col_axis*work_norm);
	double norm_offset=pPlate->GetThick();
	if(bOriInBtmPlane==FALSE)	//圆心不在底面
		norm_offset=pPlate->GetThick()*0.5;
	double column_offset=norm_offset/radiusRatio;
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||sys.part_map.plate.bDimCoord||m_bSpecifyUCS)
		bAdjustPlateUcs=TRUE;
	//绘制椭圆弧段
	f3dPoint ellipse_center,minor_pt,top_pt,btm_pt;
	f3dArcLine ellipse;
	ellipse.pen.style=PS_DASH;
	ellipse.pen.width=1;
	for(int i=0;i<2;i++)
	{
		UCS_STRU ucs;
		ucs=pPlate->ucs;
		if(bAdjustPlateUcs)
			ucs=new_ucs;
		ellipse_center=center;	//椭圆圆心
		if(i==0)	//顶面
		{
			ucs.origin+=ucs.axis_z*norm_offset;
			if((f3dPoint(0,0,1)*col_axis)>0)
				ellipse_center=center+col_axis*column_offset;
			else
				ellipse_center=center-col_axis*column_offset;
			if(bAdjustPlateUcs)
				coord_trans(ellipse_center,new_ucs,TRUE);
			else
				coord_trans(ellipse_center,pPlate->ucs,TRUE);
			coord_trans(ellipse_center,ucs,FALSE);
		}
		else if(bOriInBtmPlane==FALSE)		//底面
		{	
			ucs.origin-=ucs.axis_z*norm_offset;
			if((f3dPoint(0,0,1)*col_axis)>0)
				ellipse_center=center-col_axis*column_offset;
			else
				ellipse_center=center+col_axis*column_offset;
			if(bAdjustPlateUcs)
				coord_trans(ellipse_center,new_ucs,TRUE);
			else
				coord_trans(ellipse_center,pPlate->ucs,TRUE);
			coord_trans(ellipse_center,ucs,FALSE);
		}
		//判断椭圆圆心位置，根据圆心位置绘制相应椭圆弧
		if(f3dPoint(ellipse_center-center)*majorAxis>0)	//位于短轴右侧
		{	
			if(startPt.x>0 && endPt.x>0)
				ellipse.CreateEllipse(ellipse_center,arcline.Start(),arcline.End(),col_axis,work_norm,arcline.Radius());
			else if(startPt.x<=0 && endPt.x>=0)
			{
				minor_pt=ellipse_center+minorAxis*arcline.Radius();
				ellipse.CreateEllipse(ellipse_center,minor_pt,arcline.End(),col_axis,work_norm,arcline.Radius());
			}
			else if(startPt.x>0 && endPt.x<0)
			{
				minor_pt=ellipse_center-minorAxis*arcline.Radius();
				ellipse.CreateEllipse(ellipse_center,arcline.Start(),minor_pt,col_axis,work_norm,arcline.Radius());
			}
			edge_arr.Add(ellipse);
			//短轴过渡点
			if(startPt.x<0&&endPt.x==0)
				minor_pt=center+minorAxis*arcline.Radius();
			else if(startPt.x==0&&endPt.x<0)
				minor_pt=center-minorAxis*arcline.Radius();
		}
		else	//位于短轴左侧
		{	
			if(startPt.x<0 && endPt.x<0)
				ellipse.CreateEllipse(ellipse_center,arcline.Start(),arcline.End(),col_axis,work_norm,arcline.Radius());
			else if(startPt.x<0 && endPt.x>=0)
			{
				minor_pt=ellipse_center+minorAxis*arcline.Radius();
				ellipse.CreateEllipse(ellipse_center,arcline.Start(),minor_pt,col_axis,work_norm,arcline.Radius());
			}
			else if(startPt.x>=0 && endPt.x<0)
			{
				minor_pt=ellipse_center-minorAxis*arcline.Radius();
				ellipse.CreateEllipse(ellipse_center,minor_pt,arcline.End(),col_axis,work_norm,arcline.Radius());
			}
			edge_arr.Add(ellipse);
			//短轴过渡点
			if(startPt.x==0&&endPt.x>0)
				minor_pt=center+minorAxis*arcline.Radius();
			else if(startPt.x>0&&endPt.x==0)
				minor_pt=center-minorAxis*arcline.Radius();
		}
		if(i==0)
			top_pt=minor_pt;
		else
			btm_pt=minor_pt;
	}
	//过渡直线
	if(!top_pt.IsEqual(btm_pt))
	{
		f3dLine line(top_pt,btm_pt);
		line.pen=ellipse.pen;
		edge_arr.Add(LineToArc(line));
	}
}
//生成钢板内圆环 wht 11-07-09
BOOL CPlankDrawing::CreatePlateInnerCir(CLDSPlate *pPlate,BOOL bPatternDrawing,BOOL bAdjustPlateUcs)
{
	if(!(pPlate->m_fInnerRadius>0))	//有内圆
		return FALSE;
	f3dLine line;
	f3dArcLine arcline;
	DIAMETERDIM_STRU diam_dim;
	ROTATEDIM_STRU rotate_dim;
	f3dPoint inner_origin=pPlate->m_tInnerOrigin;
	if(bAdjustPlateUcs)
	{	//标注相对坐标
		coord_trans(inner_origin,pPlate->ucs,TRUE);
		coord_trans(inner_origin,new_ucs,FALSE);
	}
	//pPlate->m_tInnerColumnNorm为零表示与钢板垂直 wxc-2017.2.28
	if(!pPlate->m_tInnerColumnNorm.IsZero()&&fabs(pPlate->m_tInnerColumnNorm*pPlate->ucs.axis_z)<EPS_COS)
	{	//椭圆
		f3dPoint workNorm(0,0,1),columnNorm=pPlate->m_tInnerColumnNorm;
		if(bAdjustPlateUcs)
			vector_trans(columnNorm,new_ucs,FALSE);
		else
			vector_trans(columnNorm,pPlate->ucs,FALSE);
		f3dPoint minorAxis=columnNorm^workNorm;	
		normalize(minorAxis);		//椭圆短轴方向
		f3dPoint majorAxis(minorAxis.y,-minorAxis.x,minorAxis.z);	//顺时针旋转90度 wht 11-05-12
		normalize(majorAxis);		//椭圆长轴方向
		double radiusRatio=fabs(columnNorm*workNorm);
		double minorRadius=pPlate->m_fInnerRadius;				//椭圆短半轴长度
		double majorRadius=pPlate->m_fInnerRadius/radiusRatio;	//椭圆长半轴长度
		f3dPoint start=inner_origin+majorAxis*majorRadius;
		f3dPoint end = inner_origin+minorAxis*minorRadius;
		arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,pPlate->m_fInnerRadius);
		edge_arr.Add(arcline);
		start=end;
		end=inner_origin-majorAxis*majorRadius;
		arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,pPlate->m_fInnerRadius);
		edge_arr.Add(arcline);
		start=end;
		end = inner_origin-minorAxis*minorRadius;
		arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,pPlate->m_fInnerRadius);
		edge_arr.Add(arcline);
		start=end;
		end=inner_origin+majorAxis*majorRadius;
		arcline.CreateEllipse(inner_origin,start,end,columnNorm,workNorm,pPlate->m_fInnerRadius);
		edge_arr.Add(arcline);
		//指定坐标系时 不绘制截面图 wht 10-08-31 
		//生成零件图时才需要标注内圆半径及钢板截面图
		if(!m_bSpecifyUCS&&!bPatternDrawing)
		{
			//标注长轴半径 
			rotate_dim.startPos=inner_origin;
			rotate_dim.endPos=inner_origin+majorAxis*majorRadius;
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dist = majorRadius;	//长轴半径
			rotate_dim.angle=Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.bCanTextOutter=TRUE;
			rotate_dim_arr.Add(rotate_dim);
			rotate_dim.bCanTextOutter=FALSE;
			//标注短轴半径 
			rotate_dim.startPos=inner_origin;
			rotate_dim.endPos=inner_origin+minorAxis*minorRadius;
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dist = minorRadius;
			rotate_dim.angle=Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim.bCanTextOutter=TRUE;
			rotate_dim_arr.Add(rotate_dim);
			rotate_dim.bCanTextOutter=FALSE;
			//内圆环为椭圆是需要绘图截面图 标注椭圆长轴长度 wht 10-01-21
			double plate_len=pPlate->GetLength();
			double plate_thick=pPlate->GetThick();
			double tana=tan(radiusRatio);
			double offset_dist=tana*plate_thick;	
			f3dPoint offset_origin(0,-plate_len);	//具体值由经验确定 *0.6改为*1 异形法兰 wht 11-05-12
			line.pen.style=PS_SOLID;
			line.ID=0;
			//环向板截面图
			line.startPt.Set(-0.5*plate_len,-0.5*plate_thick+offset_origin.y);
			line.endPt.Set(-0.5*plate_len,0.5*plate_thick+offset_origin.y);
			edge_arr.Add(LineToArc(line));
			line.startPt=line.endPt;
			line.endPt.Set(0.5*plate_len,0.5*plate_thick+offset_origin.y);
			edge_arr.Add(LineToArc(line));
			line.startPt=line.endPt;
			line.endPt.Set(0.5*plate_len,-0.5*plate_thick+offset_origin.y);
			edge_arr.Add(LineToArc(line));
			line.startPt=line.endPt;
			line.endPt.Set(-0.5*plate_len,-0.5*plate_thick+offset_origin.y);
			edge_arr.Add(LineToArc(line));
			//标注椭圆长轴直径
			rotate_dim.startPos.Set(-(majorRadius-0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
			rotate_dim.endPos.Set(majorRadius+0.5*offset_dist,-0.5*plate_thick+offset_origin.y);
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dimPos.y-=15;
			rotate_dim.dist=majorRadius*2;
			rotate_dim.angle = 0;
			rotate_dim_arr.Add(rotate_dim);
			
			//椭圆弧截面线--右侧
			line.startPt.Set(majorRadius-0.5*offset_dist,0.5*plate_thick+offset_origin.y);
			line.endPt.Set(majorRadius+0.5*offset_dist,-0.5*plate_thick+offset_origin.y);
			edge_arr.Add(LineToArc(line));
			//椭圆弧截面线--左侧
			line.startPt.Set(-(majorRadius+0.5*offset_dist),0.5*plate_thick+offset_origin.y);
			line.endPt.Set(-(majorRadius-0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
			edge_arr.Add(LineToArc(line));
			//标注偏移值
			rotate_dim.startPos.Set(-(majorRadius+0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
			rotate_dim.endPos.Set(-(majorRadius-0.5*offset_dist),-0.5*plate_thick+offset_origin.y);
			rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
			rotate_dim.dimPos.y-=15;
			rotate_dim.dist=offset_dist;
			rotate_dim.angle = 0;
			rotate_dim_arr.Add(rotate_dim);
		}
	}
	else
	{	//圆
		f3dPoint startPt(inner_origin.x,pPlate->m_tInnerOrigin.y+pPlate->m_fInnerRadius);
		arcline.CreateMethod1(inner_origin,startPt,f3dPoint(0,0,1),2*Pi);
		edge_arr.Add(arcline);
		//标注内圆直径,生成零件图时才需要标注内圆直径
		if(!m_bSpecifyUCS&&!bPatternDrawing)
		{	
			diam_dim.chordPos.Set(inner_origin.x+arcline.Radius(),inner_origin.y);
			diam_dim.dist=ftoi(arcline.Radius()*2);
			diam_dim.farChordPos=-diam_dim.chordPos;
			diam_dim.leaderLength=5;
			rotate_point_around_axis(diam_dim.chordPos,-0.5*Pi,f3dPoint(0,0,0),f3dPoint(0,0,1));
			rotate_point_around_axis(diam_dim.farChordPos,-0.5*Pi,f3dPoint(0,0,0),f3dPoint(0,0,1));
			diameter_dim_arr.Add(diam_dim);
		}
	}
	return TRUE;
}

//TODO:未审，绘制钢板定位标识符
BOOL CPlankDrawing::DrawPlateLocateFlag(CLDSPlate *pPlank,BOOL bAdjustPlateUcs)
{
	CLDSLineTube *pDatumTube=NULL;
	f3dPoint line_vec,datum_line_vec;
	if(pPlank->m_hPartWeldParent>0x20)
		pDatumTube=(CLDSLineTube*)pPlank->BelongModel()->FromPartHandle(pPlank->m_hPartWeldParent,CLS_LINETUBE);
	if(pDatumTube)
	{
		datum_line_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_line_vec);
		vector_trans(datum_line_vec,pPlank->ucs,FALSE);
	}
	else
		return FALSE;
	CLDSNode *pDatumNode=pPlank->BelongModel()->FromNodeHandle(pPlank->designInfo.m_hBaseNode);
	if(bAdjustPlateUcs)
	{	//将基准钢管方向转换到新坐标系下
		vector_trans(datum_line_vec,pPlank->ucs,TRUE);
		vector_trans(datum_line_vec,new_ucs,FALSE);
	}
	ROTATEDIM_STRU rotate_dim;
	f3dLine line,datum_edge_line;
	PROFILE_VERTEX startVertex,endVertex,*pStartVertex=&startVertex,*pEndVertex=&endVertex;
	PROFILE_VERTEX *pPrevVertex=pPlank->vertex_list.GetTail(),*pVertex=NULL;
	for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
	{
		if(pPrevVertex&&pPrevVertex->m_bWeldEdge&&datum_edge_line.startPt==datum_edge_line.endPt)
		{	//第一道与基准钢管平行的焊缝线为基准边
			*pStartVertex=*pPrevVertex;
			*pEndVertex=*pVertex;
			pStartVertex->vertex=pPlank->GetRealVertex(pStartVertex->vertex);
			if(bAdjustPlateUcs)
			{
				vector_trans(pStartVertex->vertex,pPlank->ucs,TRUE);
				vector_trans(pStartVertex->vertex,new_ucs,FALSE);
				pEndVertex->vertex=pPlank->GetRealVertex(pEndVertex->vertex);
				vector_trans(pEndVertex->vertex,pPlank->ucs,TRUE);
				vector_trans(pEndVertex->vertex,new_ucs,FALSE);
			}
			line_vec=pEndVertex->vertex-pStartVertex->vertex;
			normalize(line_vec);
			if(fabs(line_vec*datum_line_vec)<EPS_COS)
				continue;
			f3dPoint N_vec(-line_vec.y,line_vec.x);	//钢板定位基准边左侧偏移法向量
			datum_edge_line.startPt=pStartVertex->vertex;
			datum_edge_line.endPt=pEndVertex->vertex;
			if(sys.part_map.plate.bDrawDatumLine)
			{	//绘制钢管心线及加工间隙线
				datumLine.startPt=pDatumTube->Start();
				datumLine.endPt=pDatumTube->End();
				if(bAdjustPlateUcs)
				{
					coord_trans(datumLine.startPt,new_ucs,FALSE);
					coord_trans(datumLine.endPt,new_ucs,FALSE);
				}
				else
				{
					coord_trans(datumLine.startPt,pPlank->ucs,FALSE);
					coord_trans(datumLine.endPt,pPlank->ucs,FALSE);
				}
				datumLine.startPt.z=datumLine.endPt.z=0;
				line.pen.style=PS_DASHDOT;
				SnapPerp(&line.startPt,datumLine,datum_edge_line.startPt);
				SnapPerp(&line.endPt,datumLine,datum_edge_line.endPt);
				//标注螺栓射线时 才需要显示钢管心线 wht 10-06-21
				if(sys.part_map.plate.bDimPlateRayLs&&sys.part_map.plate.bDrawDatumLine)
					edge_arr.Add(LineToArc(line));
				if(pStartVertex->manu_space!=0)
				{
					line.startPt=datum_edge_line.startPt+N_vec*pStartVertex->manu_space;
					line.endPt=datum_edge_line.endPt+N_vec*pStartVertex->manu_space;
					line.pen.style=PS_DASH;
					edge_arr.Add(LineToArc(line));
				}
			}
			//装配定位基点
			if( sys.part_group_map.iPlateLocateDrawStyle>0&&	//需要标注钢板焊缝边对活点
				pDatumNode&&pStartVertex->local_point_vec>0)
			{	//标注坐标时不标注对活点信息
				GEPOINT datum_org=pPlank->ucs.origin;
				if(bAdjustPlateUcs)
					coord_trans(datum_org,new_ucs,FALSE,TRUE);	//转换到统一坐标系下
				else
					datum_org.Set();
				f3dPoint mark_pos=pStartVertex->RetrieveWeldLocation(pEndVertex->vertex,datum_line_vec,datum_org);

				double start_mark_offset=0,end_mark_offset=0;
				SnapPerp(&mark_pos,datum_edge_line,mark_pos);

				if(sys.part_group_map.iPlateLocateDrawStyle==2)
				{	//三竖线标注方式
					line.pen.style=PS_SOLID;
					line.startPt=mark_pos+N_vec*15*sys.general_set.PartMapScale.fRealToDraw;
					line.endPt=mark_pos;//-N_vec*15;
					edge_arr.Add(LineToArc(line));	//中间基准线

					f3dLine arrowline=line;
					arrowline.startPt=mark_pos+N_vec*(5*sys.general_set.PartMapScale.fRealToDraw);
					arrowline.endPt=arrowline.startPt+line_vec*(2*sys.general_set.PartMapScale.fRealToDraw);
					edge_arr.Add(LineToArc(arrowline));	//应广州增立要求箭头标记
					arrowline.startPt=arrowline.endPt;
					arrowline.endPt=mark_pos;
					edge_arr.Add(LineToArc(arrowline));	//应广州增立要求箭头标记
				}
				else
				{	//三竖线标注方式
					//边始-->终方向与焊接父杆件始-->终方向相同且对火点方向为始-->终
					//或者边与杆件相反且对活点方向为终-->始
					if((pStartVertex->local_point_vec==1&&datum_line_vec*line_vec>EPS_COS2)||
						(pStartVertex->local_point_vec==2&&(-datum_line_vec)*line_vec>EPS_COS2))//同向(line_vec*datum_line_vec>0)
					{	//50在边始端侧	wht 10-01-11
						/*start_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.endPt)-10);
						end_mark_offset=0.5*start_mark_offset;
						end_mark_offset=min(end_mark_offset,DISTANCE(mark_pos,datum_edge_line.startPt)-10);*/
						end_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.endPt)-10);
						start_mark_offset=0.5*end_mark_offset;
						start_mark_offset=min(start_mark_offset,DISTANCE(mark_pos,datum_edge_line.startPt)-10);
					}
					//边始-->终方向与焊接父杆件始-->终方向相同且对火点方向为终-->始
					//或者边与杆件方向相反且对活点方向为始-->终
					else if((pStartVertex->local_point_vec==2&&datum_line_vec*line_vec>EPS_COS2)||
							(pStartVertex->local_point_vec==1&&(-datum_line_vec)*line_vec>EPS_COS2))//反向
					{	//100在边始端侧		wht 10-01-11
						start_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.startPt)-10);
						end_mark_offset=0.5*start_mark_offset;
						end_mark_offset=min(end_mark_offset,DISTANCE(mark_pos,datum_edge_line.endPt)-10);
						/*end_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.startPt)-10);
						start_mark_offset=0.5*end_mark_offset;
						start_mark_offset=min(start_mark_offset,DISTANCE(mark_pos,datum_edge_line.endPt)-10);*/
					}
					line.pen.style=PS_SOLID;
					line.startPt=mark_pos+N_vec*10*sys.general_set.PartMapScale.fRealToDraw;
					line.endPt=mark_pos;//-N_vec*15;
					edge_arr.Add(LineToArc(line));	//中间基准线
					line.startPt-=N_vec*(5*sys.general_set.PartMapScale.fRealToDraw)+line_vec*start_mark_offset;//datum_line_vec*start_mark_offset;
					line.endPt-=line_vec*start_mark_offset;			//datum_line_vec*start_mark_offset;
					edge_arr.Add(LineToArc(line));	//始端侧（左侧）装配方向定位线
					line.startPt+=line_vec*(start_mark_offset+end_mark_offset);//datum_line_vec*(start_mark_offset+end_mark_offset);
					line.endPt+=line_vec*(start_mark_offset+end_mark_offset);//datum_line_vec*(start_mark_offset+end_mark_offset);
					edge_arr.Add(LineToArc(line));	//终端侧（右侧）装配方向定位线
					rotate_dim.startPos=mark_pos-line_vec*start_mark_offset;//datum_line_vec*start_mark_offset;
					rotate_dim.endPos=mark_pos;
					rotate_dim.dimPos=mark_pos-line_vec*(start_mark_offset/2)+N_vec*20;//datum_line_vec*(start_mark_offset/2)+N_vec*20;
					rotate_dim.dist=start_mark_offset;
					rotate_dim.angle=Cal2dLineAng(
						rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
					rotate_dim_arr.Add(rotate_dim);
					rotate_dim.startPos=mark_pos;
					rotate_dim.endPos=mark_pos+line_vec*end_mark_offset;	//datum_line_vec*end_mark_offset;
					rotate_dim.dimPos=mark_pos+line_vec*(end_mark_offset/2)+N_vec*20;//datum_line_vec*(end_mark_offset/2)+N_vec*20;
					rotate_dim.dist=end_mark_offset;
					rotate_dim_arr.Add(rotate_dim);
				}
			}
		}
		pPrevVertex=pVertex;
	}
	return TRUE;
}
BOOL CPlankDrawing::Create1FacePlank(CLDSPlate *pPlank,BOOL bPatternDrawing/*=TRUE*/)
{
	int i,n;
	CLsRef *pLsRef;
	f3dPoint basePt,dimPos,dim_vec;
	f3dLine line,datum_edge_line;
	f3dArcLine arcline;
	f3dCircle ls_cir;
	DIAMETERDIM_STRU diam_dim;
	ROTATEDIM_STRU rotate_dim;
	line.feature = 0;
	n = pPlank->vertex_list.GetNodeNum();
	if(n<3)
		return FALSE;
	CLDSLineTube *pDatumTube=NULL;
	f3dPoint line_vec,datum_line_vec;
	if(pPlank->m_hPartWeldParent>0x20)
		pDatumTube=(CLDSLineTube*)pPlank->BelongModel()->FromPartHandle(pPlank->m_hPartWeldParent,CLS_LINETUBE);
	if(pDatumTube)
	{
		datum_line_vec=pDatumTube->End()-pDatumTube->Start();
		normalize(datum_line_vec);
		vector_trans(datum_line_vec,pPlank->ucs,FALSE);
	}
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||(sys.part_map.plate.bDimCoord&&!bPatternDrawing))
		bAdjustPlateUcs=TRUE;
	if(m_bSpecifyUCS) //指定绘图坐标系优先于内部调整坐标系 wht 10-08-13
	{
		bAdjustPlateUcs=TRUE;
		PROFILE_VERTEX *pVertex=NULL;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,pPlank->ucs,TRUE);
			coord_trans(pVertex->vertex,new_ucs,FALSE);
			if(pVertex->type==1||pVertex->type==2)
			{	//转换圆心拾取点
				coord_trans(pVertex->center,pPlank->ucs,TRUE);
				coord_trans(pVertex->center,new_ucs,FALSE);
			}
		}
		//计算几何中心
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pPlank->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=pPlank->vertex_list.GetNodeNum();
		project_point(geom_center,pPlank->ucs.origin,pPlank->ucs.axis_z);
	}
	else //if(bAdjustPlateUcs)	//标注坐标
	{
		UCS_STRU ucs;
		short style=0;
		if(!bAdjustPlateUcs) style=1;
		if(AdjustPlateUcs(pPlank,ucs,geom_center,style)>EPS2)
		{
			PROFILE_VERTEX *pVertex=NULL;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				coord_trans(pVertex->vertex,pPlank->ucs,TRUE);
				coord_trans(pVertex->vertex,ucs,FALSE);
				if(pVertex->type==1||pVertex->type==2)
				{	//转换圆心拾取点
					coord_trans(pVertex->center,pPlank->ucs,TRUE);
					coord_trans(pVertex->center,ucs,FALSE);
				}
			}
			new_ucs=ucs;	//记录调整后的坐标系
			bAdjustPlateUcs=TRUE;
		}
		else
		{
			new_ucs=pPlank->ucs;
			for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pPlank->ucs,TRUE);
				geom_center+=vertex;
			}
			geom_center/=pPlank->vertex_list.GetNodeNum();
			project_point(geom_center,pPlank->ucs.origin,pPlank->ucs.axis_z);
		}
	}
	
	//查找四周边界
	BOOL bFirstDimR=TRUE;
	double max_edge_len=0;
	ARRAY_LIST<f3dPoint> profileVertexList;
	int indexOfFirstEdgeLine=-1;
	m_bPrevIsRollEdgeLine=false;
	for(i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=&vertex_list[i];
		PROFILE_VERTEX *pEndVertex=&vertex_list[(i+1)%n];
		TXTDIM_STRU txt_dim;
		if(pStartVertex->m_bWeldEdge&&pStartVertex->manu_space!=0)
		{	//输出加工间隙线
			f3dLine manu_cut_line(pStartVertex->vertex,pEndVertex->vertex);	//考虑了加工间隙的实际加工切割线
			GEPOINT offset_vec=manu_cut_line.endPt-manu_cut_line.startPt;
			if(pStartVertex->type==0)
			{
				normalize(offset_vec);
				offset_vec.Set(-offset_vec.y,offset_vec.x);	//逆时针旋转90°确定垂直与板边向里的间隙偏移方向
				manu_cut_line.startPt+=offset_vec*pStartVertex->manu_space;
				manu_cut_line.endPt+=offset_vec*pStartVertex->manu_space;
				line.startPt = manu_cut_line.startPt;
				line.endPt   = manu_cut_line.endPt;
				line.startPt.z = 0;
				line.endPt.z = 0;
				arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
			}
			else
			{
				if(pStartVertex->type==1)
					arcline.CreateMethod2(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,1),pStartVertex->sector_angle);
				else if(pStartVertex->type==2)
				{	
					f3dPoint center_pick=pStartVertex->center;
					/*if(bAdjustPlateUcs)
					{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
						coord_trans(center_pick,pPlank->ucs,TRUE);
						coord_trans(center_pick,new_ucs,FALSE);
						center_pick.z=0;
					}*/
					int flag=1;
					if(pStartVertex->radius<0)
						flag=-1;
					pStartVertex->vertex.z=pEndVertex->vertex.z=0;	//起始点终止点Z坐标归零
					arcline.CreateMethod3(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,flag),flag*pStartVertex->radius,center_pick);
				}
				else if(pStartVertex->type==3)
				{
					int sign=1;
					f3dPoint center_pick=pStartVertex->center;
					f3dPoint column_norm=pStartVertex->column_norm;
					if(bAdjustPlateUcs)
					{
						coord_trans(center_pick,new_ucs,FALSE);
						vector_trans(column_norm,new_ucs,FALSE);
					}
					else
					{
						coord_trans(center_pick,pPlank->ucs,FALSE);
						vector_trans(column_norm,pPlank->ucs,FALSE);
					}
					if(pStartVertex->radius<0)
						sign=-1;
					arcline.CreateEllipse(center_pick,pStartVertex->vertex,pEndVertex->vertex,column_norm,f3dPoint(0,0,sign),sign*pStartVertex->radius);
				}
				//将圆弧向里偏移出一个同心加工间隙圆
				int sign=1;
				if(arcline.WorkNorm().z<0)
					sign=-1;
				GEPOINT center=arcline.Center();
				offset_vec=arcline.Start()-center;
				normalize(offset_vec);
				manu_cut_line.startPt=arcline.Start()-offset_vec*(sign*pStartVertex->manu_space);
				offset_vec=arcline.End()-center;
				normalize(offset_vec);
				manu_cut_line.endPt=arcline.End()-offset_vec*(sign*pStartVertex->manu_space);
				if(pStartVertex->type==1)
					arcline.CreateMethod2(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,1),pStartVertex->sector_angle);
				else if(pStartVertex->type==2)
				{	
					f3dPoint center_pick=pStartVertex->center;
					/*if(bAdjustPlateUcs)
					{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
						coord_trans(center_pick,pPlank->ucs,TRUE);
						coord_trans(center_pick,new_ucs,FALSE);
						center_pick.z=0;
					}*/
					int flag=1;
					if(pStartVertex->radius<0)
						flag=-1;
					pStartVertex->vertex.z=pEndVertex->vertex.z=0;	//起始点终止点Z坐标归零
					arcline.CreateMethod3(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,flag),flag*pStartVertex->radius,center_pick);
				}
				else if(pStartVertex->type==3)
				{
					int flag=1;
					f3dPoint center_pick=pStartVertex->center;
					f3dPoint column_norm=pStartVertex->column_norm;
					if(bAdjustPlateUcs)
					{
						coord_trans(center_pick,new_ucs,FALSE);
						vector_trans(column_norm,new_ucs,FALSE);
					}
					else
					{
						coord_trans(center_pick,pPlank->ucs,FALSE);
						vector_trans(column_norm,pPlank->ucs,FALSE);
					}
					if(pStartVertex->radius<0)
						flag=-1;
					pStartVertex->vertex.z=pEndVertex->vertex.z=0;	//起始点终止点Z坐标归零 wht 11-06-23
					arcline.CreateEllipse(center_pick,pStartVertex->vertex,pEndVertex->vertex,column_norm,f3dPoint(0,0,flag),sign*(fabs(pStartVertex->radius)+pStartVertex->manu_space));
					//重新绘制钢板的椭圆弧边
					DrawPlateEllipseLine(pPlank,arcline);
				}
			}
			arcline.pen.style=PS_DASH;
			edge_arr.Add(arcline);
			arcline.pen.style=PS_SOLID;
		}
		if(pStartVertex->type==0&&pStartVertex->m_bRollEdge&&pStartVertex->manu_space!=0)
		{	//输出卷边
			f3dLine roll_edge_line,huoqu_line;
			PROFILE_VERTEX *pPrevVertex=&vertex_list[(i+n-1)%n];
			PROFILE_VERTEX *pNextVertex=&vertex_list[(i+2)%n];
			//根据卷边高度以及卷边外移量绘制卷边相关线段 wht 11-04-09
			DrawRollEdgeLine(pStartVertex,pEndVertex,pPrevVertex,pNextVertex,pPlank,roll_edge_line,huoqu_line,bPatternDrawing);
			arcline.CreateMethod2(roll_edge_line.startPt,roll_edge_line.endPt,f3dPoint(0,0,1),0);
			if(i==n-1&&indexOfFirstEdgeLine>=0)
			{
				edge_arr[indexOfFirstEdgeLine].SetStart(m_xRollEdgeLineEnd);
				m_bPrevIsRollEdgeLine=false;
			}
			//卷边处理后会多出3条轮廓边
			profileVertexList.append(huoqu_line.startPt);
			profileVertexList.append(roll_edge_line.startPt);
			profileVertexList.append(roll_edge_line.endPt);
			//标注顶点相对坐标值
			if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
				DimCoordValue(pPlank,huoqu_line.startPt);
		}
		else
		{
			if(pStartVertex->type==0)
			{
				//前一个边为卷边时，更新当前边的起点
				line.startPt = pStartVertex->vertex;
				if(m_bPrevIsRollEdgeLine)
					line.startPt=m_xRollEdgeLineEnd;
				//第一个边为卷边时，更新最后一个边的终点
				line.endPt=pEndVertex->vertex;
				if(i==n-1&&pEndVertex->m_bRollEdge&&pEndVertex->manu_space!=0)
					line.endPt= edge_arr[0].Start();
				line.startPt.z = 0;
				line.endPt.z = 0;
				arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
			}
			else if(pStartVertex->type==1)
			{	
				int sign=1;
				if(pStartVertex->sector_angle<0)
					sign=-1;
				arcline.CreateMethod2(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,sign),fabs(pStartVertex->sector_angle));
			}
			else if(pStartVertex->type==2)
			{	
				f3dPoint center_pick=pStartVertex->center;
				/*if(bAdjustPlateUcs)
				{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
					coord_trans(center_pick,pPlank->ucs,TRUE);
					coord_trans(center_pick,new_ucs,FALSE);
					center_pick.z=0;
				}*/
				int sign=1;
				if(pStartVertex->radius<0)
					sign=-1;
				pStartVertex->vertex.z=pEndVertex->vertex.z=0;	//起始点终止点Z坐标归零
				arcline.CreateMethod3(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,sign),sign*pStartVertex->radius,center_pick);
			}
			else if(pStartVertex->type==3)
			{
				int sign=1;
				f3dPoint center_pick=pStartVertex->center;
				f3dPoint column_norm=pStartVertex->column_norm;
				if(bAdjustPlateUcs)
				{
					coord_trans(center_pick,new_ucs,FALSE);
					vector_trans(column_norm,new_ucs,FALSE);
				}
				else 
				{
					coord_trans(center_pick,pPlank->ucs,FALSE);
					vector_trans(column_norm,pPlank->ucs,FALSE);
				}
				if(pStartVertex->radius<0)
					sign=-1;
				pStartVertex->vertex.z=pEndVertex->vertex.z=0;	//起始点终止点Z坐标归零 wht 11-06-23
				arcline.CreateEllipse(center_pick,pStartVertex->vertex,pEndVertex->vertex,column_norm,f3dPoint(0,0,sign),sign*pStartVertex->radius);
				//重新绘制钢板的椭圆弧边
				DrawPlateEllipseLine(pPlank,arcline);
			}
			m_bPrevIsRollEdgeLine=false;
			if(pStartVertex->type>0)
			{	//绘制圆弧辅助线 标注圆弧夹角
				double fMinRadius=20;
				if(sys.part_map.plate.bDimArcAngle&&pPlank->m_fInnerRadius<EPS&&arcline.Radius()>fMinRadius)
				{	//环向板无需标注角度
					f3dLine assistant_line;
					assistant_line.pen.style=PS_DASHDOT;
					assistant_line.startPt=arcline.Center();
					assistant_line.endPt=arcline.Start();
					edge_arr.Add(LineToArc(assistant_line));
					assistant_line.endPt=arcline.End();
					edge_arr.Add(LineToArc(assistant_line));
					//添加角度标注
					GEPOINT mid_pt=0.5*(arcline.Start()+arcline.End());
					GEPOINT center_vec=mid_pt-arcline.Center();
					normalize(center_vec);
					ANGULARDIM_STRU angular_dim;
					angular_dim.vertex=arcline.Center();
					angular_dim.startPos=0.5*(arcline.Center()+arcline.Start());
					angular_dim.endPos=0.5*(arcline.Center()+arcline.End());
					angular_dim.dimPos=arcline.Center()+center_vec*arcline.Radius()*0.5;
					angular_dim_arr.Add(angular_dim);
				}
				if(sys.part_map.plate.bDimArcEdgeR&&
					((pPlank->m_fInnerRadius<EPS&&arcline.Radius()>fMinRadius)||(pPlank->m_fInnerRadius>0&&bFirstDimR)))
				{	//标注圆弧半径
					bFirstDimR=FALSE;
					double half_angle=arcline.SectorAngle()*0.5;
					rotate_dim.startPos=sys.part_map.plate.bDimArcAngle?arcline.Start():arcline.PositionInAngle(half_angle);
					rotate_dim.endPos=arcline.Center();
					rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
					rotate_dim.dist = arcline.Radius();
					rotate_dim.angle=Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
					rotate_dim.bDisplayDimLine=TRUE;
					rotate_dim_arr.Add(rotate_dim);
				}
			}
			int iNewInsert=edge_arr.Add(arcline);
			if(indexOfFirstEdgeLine<0)
				indexOfFirstEdgeLine=iNewInsert;
			//记录钢板外形轮廓点 wht 15-08-12
			profileVertexList.append(arcline.Start());
			//标注顶点相对坐标值
			if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
				DimCoordValue(pPlank,arcline.Start());
		}
		if(pStartVertex->m_bWeldEdge)
			ProcessPlateWeldEdge(pStartVertex,arcline);//处理焊缝边(绘制焊缝线、绘制坡口线、标注焊接件号、标注坡口尺寸等) wht 18-08-09
		//
		if(max_edge_len<DISTANCE(arcline.Start(),arcline.End()))
		{
			max_edge_len=DISTANCE(arcline.Start(),arcline.End());
			long_edge_mid=0.5*(arcline.Start()+arcline.End());
			long_edge_angle=Cal2dLineAng(arcline.Start().x,arcline.Start().y,arcline.End().x,arcline.End().y);
		}
		if(pDatumTube&&pStartVertex->m_bWeldEdge&&datum_edge_line.startPt==datum_edge_line.endPt)
		{	//第一道与基准钢管平行的焊缝线为基准边
			line_vec=pEndVertex->vertex-pStartVertex->vertex;
			normalize(line_vec);
			if(bAdjustPlateUcs)
			{	//将基准钢管方向转换到新坐标系下
				vector_trans(datum_line_vec,pPlank->ucs,TRUE);
				vector_trans(datum_line_vec,new_ucs,FALSE);
			}
			if(fabs(line_vec*datum_line_vec)<EPS_COS)
				continue;
			f3dPoint N_vec(-line_vec.y,line_vec.x);	//钢板定位基准边左侧偏移法向量
			datum_edge_line.startPt=pStartVertex->vertex;
			datum_edge_line.endPt=pEndVertex->vertex;
			if(sys.part_map.plate.bDrawDatumLine)
			{	//绘制钢管心线及加工间隙线
				datumLine.startPt=pDatumTube->Start();
				datumLine.endPt=pDatumTube->End();
				if(bAdjustPlateUcs)
				{
					coord_trans(datumLine.startPt,new_ucs,FALSE);
					coord_trans(datumLine.endPt,new_ucs,FALSE);
				}
				else
				{
					coord_trans(datumLine.startPt,pPlank->ucs,FALSE);
					coord_trans(datumLine.endPt,pPlank->ucs,FALSE);
				}
				datumLine.startPt.z=datumLine.endPt.z=0;
				line.pen.style=PS_DASHDOT;
				SnapPerp(&line.startPt,datumLine,datum_edge_line.startPt);
				SnapPerp(&line.endPt,datumLine,datum_edge_line.endPt);
				//标注螺栓射线时 才需要显示钢管心线 wht 10-06-21
				if(sys.part_map.plate.bDimPlateRayLs&&!m_bSpecifyUCS)	//标注相对坐标或指定坐标时不需要绘制钢管心线 wht 09-09-16
					edge_arr.Add(LineToArc(line));
				if(pStartVertex->manu_space!=0)
				{
					line.startPt=datum_edge_line.startPt+N_vec*pStartVertex->manu_space;
					line.endPt=datum_edge_line.endPt+N_vec*pStartVertex->manu_space;
					line.pen.style=PS_DASH;
					edge_arr.Add(LineToArc(line));
				}
			}
			//装配定位基点
			CLDSNode *pDatumNode=pPlank->BelongModel()->FromNodeHandle(pPlank->designInfo.m_hBaseNode);
			if( sys.part_group_map.iPlateLocateDrawStyle>0&&	//需要标注钢板焊缝边对活点
				pDatumNode&&pStartVertex->local_point_vec>0)
			{	//标注坐标时不标注对活点信息
				GEPOINT datum_org=pPlank->ucs.origin;
				if(bAdjustPlateUcs)
					coord_trans(datum_org,new_ucs,FALSE,TRUE);	//转换到统一坐标系下
				else
					datum_org.Set();
				f3dPoint mark_pos=pStartVertex->RetrieveWeldLocation(pEndVertex->vertex,datum_line_vec,datum_org);

				double start_mark_offset=0,end_mark_offset=0;
				SnapPerp(&mark_pos,datum_edge_line,mark_pos);

				if(sys.part_group_map.iPlateLocateDrawStyle==2)
				{	//三竖线标注方式
					line.pen.style=PS_SOLID;
					line.startPt=mark_pos+N_vec*15*sys.general_set.PartMapScale.fRealToDraw;
					line.endPt=mark_pos;//-N_vec*15;
					edge_arr.Add(LineToArc(line));	//中间基准线

					f3dLine arrowline=line;
					arrowline.startPt=mark_pos+N_vec*(5*sys.general_set.PartMapScale.fRealToDraw);
					arrowline.endPt=arrowline.startPt+line_vec*(2*sys.general_set.PartMapScale.fRealToDraw);
					edge_arr.Add(LineToArc(arrowline));	//应广州增立要求箭头标记
					arrowline.startPt=arrowline.endPt;
					arrowline.endPt=mark_pos;
					edge_arr.Add(LineToArc(arrowline));	//应广州增立要求箭头标记
				}
				else
				{	//三竖线标注方式
					//边始-->终方向与焊接父杆件始-->终方向相同且对火点方向为始-->终
					//或者边与杆件相反且对活点方向为终-->始
					if((pStartVertex->local_point_vec==1&&datum_line_vec*line_vec>EPS_COS2)||
						(pStartVertex->local_point_vec==2&&(-datum_line_vec)*line_vec>EPS_COS2))//同向(line_vec*datum_line_vec>0)
					{	//50在边始端侧	wht 10-01-11
						/*start_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.endPt)-10);
						end_mark_offset=0.5*start_mark_offset;
						end_mark_offset=min(end_mark_offset,DISTANCE(mark_pos,datum_edge_line.startPt)-10);*/
						end_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.endPt)-10);
						start_mark_offset=0.5*end_mark_offset;
						start_mark_offset=min(start_mark_offset,DISTANCE(mark_pos,datum_edge_line.startPt)-10);
					}
					//边始-->终方向与焊接父杆件始-->终方向相同且对火点方向为终-->始
					//或者边与杆件方向相反且对活点方向为始-->终
					else if((pStartVertex->local_point_vec==2&&datum_line_vec*line_vec>EPS_COS2)||
							(pStartVertex->local_point_vec==1&&(-datum_line_vec)*line_vec>EPS_COS2))//反向
					{	//100在边始端侧		wht 10-01-11
						start_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.startPt)-10);
						end_mark_offset=0.5*start_mark_offset;
						end_mark_offset=min(end_mark_offset,DISTANCE(mark_pos,datum_edge_line.endPt)-10);
						/*end_mark_offset=min(100,DISTANCE(mark_pos,datum_edge_line.startPt)-10);
						start_mark_offset=0.5*end_mark_offset;
						start_mark_offset=min(start_mark_offset,DISTANCE(mark_pos,datum_edge_line.endPt)-10);*/
					}
					line.pen.style=PS_SOLID;
					line.startPt=mark_pos+N_vec*10*sys.general_set.PartMapScale.fRealToDraw;
					line.endPt=mark_pos;//-N_vec*15;
					edge_arr.Add(LineToArc(line));	//中间基准线
					line.startPt-=N_vec*(5*sys.general_set.PartMapScale.fRealToDraw)+line_vec*start_mark_offset;//datum_line_vec*start_mark_offset;
					line.endPt-=line_vec*start_mark_offset;			//datum_line_vec*start_mark_offset;
					edge_arr.Add(LineToArc(line));	//始端侧（左侧）装配方向定位线
					line.startPt+=line_vec*(start_mark_offset+end_mark_offset);//datum_line_vec*(start_mark_offset+end_mark_offset);
					line.endPt+=line_vec*(start_mark_offset+end_mark_offset);//datum_line_vec*(start_mark_offset+end_mark_offset);
					edge_arr.Add(LineToArc(line));	//终端侧（右侧）装配方向定位线
					rotate_dim.startPos=mark_pos-line_vec*start_mark_offset;//datum_line_vec*start_mark_offset;
					rotate_dim.endPos=mark_pos;
					rotate_dim.dimPos=mark_pos-line_vec*(start_mark_offset/2)+N_vec*20;//datum_line_vec*(start_mark_offset/2)+N_vec*20;
					rotate_dim.dist=start_mark_offset;
					rotate_dim.angle=Cal2dLineAng(
						rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
					rotate_dim_arr.Add(rotate_dim);
					rotate_dim.startPos=mark_pos;
					rotate_dim.endPos=mark_pos+line_vec*end_mark_offset;	//datum_line_vec*end_mark_offset;
					rotate_dim.dimPos=mark_pos+line_vec*(end_mark_offset/2)+N_vec*20;//datum_line_vec*(end_mark_offset/2)+N_vec*20;
					rotate_dim.dist=end_mark_offset;
					rotate_dim_arr.Add(rotate_dim);
				}
			}
		}
	}
	if(pPlank->m_fInnerRadius>0)	//有内圆
		CreatePlateInnerCir(pPlank,bPatternDrawing,bAdjustPlateUcs);
	//标注钢板边长
	if(sys.part_map.plate.bDimPlateEdgeLen)
	{
		int iStart=text_dim_arr.GetCount();
		for(int i=0;i<edge_arr.GetCount();i++)
		{
			f3dArcLine arc_line=edge_arr.GetAt(i);
			if(arc_line.SectorAngle()==0&&arc_line.pen.style==PS_SOLID)
				DimPlateEdgeLen(f3dLine(arc_line.Start(),arc_line.End()));
		}
		//青岛汇金通要求合并共线的边长标注
		if(gxDrwLocalizer.ciCustomerSerial==10)
			MergeParallelEdgeDim(iStart);
	}
	//初始化钢板外形轮廓区域 wht 15-08-12
	if(profileVertexList.GetSize()>0)
		profileRegion.CreatePolygonRgn(profileVertexList.m_pData,profileVertexList.GetSize());

	for(pLsRef=pPlank->GetFirstLsRef();pLsRef!=NULL;pLsRef=pPlank->GetNextLsRef())
	{
		basePt = (*pLsRef)->ucs.origin;
		Sub_Pnt(basePt,(*pLsRef)->ucs.origin,
			(*pLsRef)->des_base_pos.norm_offset.Thick(pPlank->BelongModel())*(*pLsRef)->get_norm());
		ls_cir.centre = basePt;
		//标注螺栓相对坐标值	wht 09-09-16
		if(bAdjustPlateUcs)
			coord_trans(ls_cir.centre,new_ucs,FALSE);
		else
			coord_trans(ls_cir.centre,pPlank->ucs,FALSE);
		ls_cir.centre.z=0;	//必须清零，不然螺栓坐标间距计算不准
		//标注坐标时也有可能要分析螺栓射线
		for(int i=1;i<=32;i++)
		{
			if(pLsRef->dwRayNo&GetSingleWord(i))
			{
				GEPOINT resideRayVec=pLsRef->rayVecArr[i-1];
				if(bAdjustPlateUcs)
				{
					vector_trans(resideRayVec,pPlank->ucs,TRUE);
					vector_trans(resideRayVec,new_ucs,FALSE);
				}
				AnalizeLsRay(ls_cir.centre,i,resideRayVec);
			}
		}
		ls_cir.radius = (*pLsRef)->get_d()/2.0;
		ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
		ls_cir.pen.style=(*pLsRef)->FilterFuncType();	//记录螺栓孔的功用类型
		ls_cir.centre.pen.style=pLsRef->cFlag;			//记录螺栓孔的工艺类型
		if((int)pLsRef->waistLen>0)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width=(int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度
			ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
			if(bAdjustPlateUcs)
				vector_trans(ls_cir.norm,new_ucs,FALSE);
			else
				vector_trans(ls_cir.norm,pPlank->ucs,FALSE);//转换到钢板坐标系下
		}
		cir_arr.Add(ls_cir);
		//TODO:DimCoordValue一般都是需要调整绘制坐标系的，这样改不合适 wjh-2017.8.30
		//标注螺栓相对坐标值	wht 09-09-16
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
			DimCoordValue(pPlank,ls_cir.centre);
	}
	//螺栓射线处理
	if(sys.part_map.plate.bDimPlateRayLs)
	{
		//DimLsEndSpace(pPlank);
		DimLsRay(pPlank,bAdjustPlateUcs);
	}
	if(bPatternDrawing)//大样图中需要在焊烧死角处切弧
	{
		CreateLineOnWeldCorner(pPlank,&vertex_list);
		//绘制插板大样图中的钢管简图 wht 18-08-09
		f3dPoint origin,axisX(1,0,0),axisY(0,1,0);
		if(bAdjustPlateUcs)
		{	//调整长边中点位置
			coord_trans(origin,pPlank->ucs,TRUE);
			coord_trans(origin,new_ucs,FALSE);
			vector_trans(axisX,pPlank->ucs,TRUE);
			vector_trans(axisX,new_ucs,FALSE);
			vector_trans(axisY,pPlank->ucs,TRUE);
			vector_trans(axisY,new_ucs,FALSE);
		}
		if(CreateInsertPlateTubeSketchMap(pPlank,NULL,edge_arr,origin,axisX,axisY,dimPos))
			long_edge_mid=dimPos;
		if(pDatumTube&&pPlank->IsTransPlate()&&gxDrwLocalizer.ciCustomerSerial==7)
		{	//贯通板需要绘制钢管示意图 wht 18-08-13
			f3dLine datum_line(pDatumTube->Start(),pDatumTube->End());
			coord_trans(datum_line.startPt,new_ucs,FALSE);
			coord_trans(datum_line.endPt,new_ucs,FALSE);
			datum_line.startPt.z=datum_line.endPt.z=0;
			n=vertex_list.GetNodeNum();
			int nIntersCount=0;
			f3dPoint inters_pt_arr[2];
			for(int i=0;i<n;i++)
			{
				PROFILE_VERTEX *pStartVertex=&vertex_list[i];
				PROFILE_VERTEX *pEndVertex=&vertex_list[(i+1)%n];
				line.startPt=pStartVertex->vertex;
				line.endPt=pEndVertex->vertex;
				f3dPoint inters_pt;
				if(Int3dll(datum_line,line,inters_pt)==1)
				{
					inters_pt_arr[nIntersCount]=inters_pt;
					nIntersCount++;
					if(nIntersCount==2)
						break;
				}
			}
			if(nIntersCount==2)
			{	//绘制钢管简图
				datum_line_vec=pDatumTube->End()-pDatumTube->Start();
				normalize(datum_line_vec);
				vector_trans(datum_line_vec,new_ucs,FALSE);	//转换到钢板坐标系下
				f3dPoint vert_line_vec=datum_line_vec;
				RotateVectorAroundVector(vert_line_vec,1,0,f3dPoint(0,0,1));
				normalize(vert_line_vec);
				line.startPt=inters_pt_arr[0];
				line.endPt=inters_pt_arr[1];
				line.pen.style=PS_DASHDOT;
				edge_arr.Add(LineToArc(line));
				double fHalfD=pDatumTube->GetDiameter()*0.5;
				line.startPt=inters_pt_arr[0]+vert_line_vec*fHalfD;
				line.endPt=inters_pt_arr[1]+vert_line_vec*fHalfD;
				line.pen.style=PS_OTHERLINE;
				edge_arr.Add(LineToArc(line));
				line.startPt=inters_pt_arr[0]-vert_line_vec*fHalfD;
				line.endPt=inters_pt_arr[1]-vert_line_vec*fHalfD;
				line.pen.style=PS_OTHERLINE;
				edge_arr.Add(LineToArc(line));
				line.startPt=inters_pt_arr[0]+vert_line_vec*(fHalfD-pDatumTube->GetThick());
				line.endPt=inters_pt_arr[1]+vert_line_vec*(fHalfD-pDatumTube->GetThick());
				line.pen.style=PS_DASH;
				edge_arr.Add(LineToArc(line));
				line.startPt=inters_pt_arr[0]-vert_line_vec*(fHalfD-pDatumTube->GetThick());
				line.endPt=inters_pt_arr[1]-vert_line_vec*(fHalfD-pDatumTube->GetThick());
				line.pen.style=PS_DASH;
				edge_arr.Add(LineToArc(line));
				//标注钢管编号
				TXTDIM_STRU dim_txt;
				datum_line_vec=inters_pt_arr[1]-inters_pt_arr[0];
				normalize(datum_line_vec);
				dim_txt.dimVec=datum_line_vec;
				dim_txt.dimPos=inters_pt_arr[0]+datum_line_vec*DISTANCE(inters_pt_arr[0],inters_pt_arr[1])*0.1-vert_line_vec*pDatumTube->GetThick();
				dim_txt.dimText.Printf("焊%s管",(char*)pDatumTube->Label);
				text_dim_arr.Add(dim_txt);
			}
		}
	}
	//大样图中绘制钢板的内侧轮廓边
	if(sys.part_map.plate.bDrawInnerEdge)
		DrawInnerEdge(pPlank,&vertex_list);
	//青岛东方要求大样图中不绘制焊接子构件
	if(!(gxDrwLocalizer.ciCustomerSerial==5&&bPatternDrawing))
		DrawWeldSubPlate(pPlank,bAdjustPlateUcs);
	//分析杆件射线
	if(sys.part_map.plate.bDimCoord)
		AnalizePoleRay(pPlank);
	return TRUE;
}

BOOL CPlankDrawing::Create2FacePlank(CLDSPlate *pPlank,BOOL bPatternDrawing/*=TRUE*/)
{
	int i,n;
	f3dLine line;
	f3dPoint line_vec;
	f3dArcLine arcline;
	f3dCircle ls_cir;
	f2dLine line2d;
	f2dCircle circle2d;
	f3dPoint base_face_pick,qu_face_pick,basePt,dimPos,start_vec,end_vec,dim_vec,norm;
	CLsRef *pLsRef;
	ANGULARDIM_STRU angular_dim;

	double left=0,top=0,right=0,bottom=0;
	f3dPoint left_pt,top_pt,right_pt,bottom_pt;
	line.feature = 0;
	n = pPlank->vertex_list.GetNodeNum();
	if(n<3)
		return FALSE;
	base_face_pick=pPlank->ucs.origin;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	f3dLine HuoQuLine[2];
	HuoQuLine[0]=pPlank->HuoQuLine[0];	//HuoQuLine[0].Layer中保存了火曲半径，以及中心层系数
	//HuoQuLine[0].startPt=pPlank->HuoQuLine[0].startPt;
	//HuoQuLine[0].endPt=pPlank->HuoQuLine[0].endPt;
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||(sys.part_map.plate.bDimCoord&&!bPatternDrawing))
		bAdjustPlateUcs=TRUE;
	if(m_bSpecifyUCS) //指定绘图坐标系优先于内部调整坐标系 wht 10-08-13
	{
		bAdjustPlateUcs=TRUE;
		PROFILE_VERTEX *pVertex=NULL;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,pPlank->ucs,TRUE);
			coord_trans(pVertex->vertex,new_ucs,FALSE);
			if(pVertex->type==1||pVertex->type==2)
			{	//转换圆心拾取点
				coord_trans(pVertex->center,pPlank->ucs,TRUE);
				coord_trans(pVertex->center,new_ucs,FALSE);
			}
		}
		if(pPlank->face_N==2)
		{
			coord_trans(HuoQuLine[0].startPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[0].startPt,new_ucs,FALSE);
			coord_trans(HuoQuLine[0].endPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[0].endPt,new_ucs,FALSE);
		}
		//计算几何中心
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pPlank->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=pPlank->vertex_list.GetNodeNum();
		project_point(geom_center,pPlank->ucs.origin,pPlank->ucs.axis_z);
	}
	else //if(bAdjustPlateUcs)	//标注坐标
	{
		UCS_STRU ucs;
		short style=0;
		if(!bAdjustPlateUcs) style=1;
		if(AdjustPlateUcs(pPlank,ucs,geom_center,style)>EPS2)
		{
			PROFILE_VERTEX *pVertex=NULL;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				coord_trans(pVertex->vertex,pPlank->ucs,TRUE);
				coord_trans(pVertex->vertex,ucs,FALSE);
				if(pVertex->type==1||pVertex->type==2)
				{	//转换圆心拾取点
					coord_trans(pVertex->center,pPlank->ucs,TRUE);
					coord_trans(pVertex->center,ucs,FALSE);
				}
			}
			if(pPlank->face_N==2)
			{
				coord_trans(HuoQuLine[0].startPt,pPlank->ucs,TRUE);
				coord_trans(HuoQuLine[0].startPt,ucs,FALSE);
				coord_trans(HuoQuLine[0].endPt,pPlank->ucs,TRUE);
				coord_trans(HuoQuLine[0].endPt,ucs,FALSE);
			}
			new_ucs=ucs;	//记录调整后的坐标系
			bAdjustPlateUcs=TRUE;
		}
		else
		{
			new_ucs=pPlank->ucs;
			for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pPlank->ucs,TRUE);
				geom_center+=vertex;
			}
			geom_center/=pPlank->vertex_list.GetNodeNum();
			project_point(geom_center,pPlank->ucs.origin,pPlank->ucs.axis_z);
		}
	}
	double max_edge_len=0;
	ARRAY_LIST<f3dPoint> profileVertexList;
	bool basepick_finded=false,huoqupick_finded=false;
	int indexOfFirstEdgeLine=-1;
	m_bPrevIsRollEdgeLine=false;
	for(i=0;i<n;i++)
	{
		f3dPoint pt = vertex_list[i].vertex;
		//保证基准面和火曲面的拾取点不在火曲线上，否则无法正确判断内外曲 wht 11-04-21
		/** 后面添加fabs(dist)>10判断是为了避免连板轮廓顶点与火曲点接近（常出现于合并生成火曲连接板），
			且基面火曲面错乱时，导致错误 wjh-2016.4.29
		*/
		double dist=0;
		if(!basepick_finded||!huoqupick_finded)
			dist=DistOf2dPtLine(pt,HuoQuLine[0].startPt,HuoQuLine[0].endPt);
		if(!basepick_finded&&fabs(dist)>10&&pt.feature==1&&HuoQuLine[0].PtInLine(pt)==0)		//基准面拾取点
		{
			basepick_finded=true;
			base_face_pick = pt;
		}
		else if(!huoqupick_finded&&fabs(dist)>10&&pt.feature==2&&HuoQuLine[0].PtInLine(pt)==0)//火曲面拾取点
		{
			huoqupick_finded=true;
			qu_face_pick = pt;
		}
		if(pt.x<left)
		{
			left_pt = pt;
			left=pt.x;
		}
		if(pt.x>right)
		{
			right_pt = pt;
			right = pt.x;
		}
		if(pt.y>top)
		{
			top_pt=pt;
			top = pt.y;
		}
		if(pt.y<bottom)
		{
			bottom_pt = pt;
			bottom = pt.y;
		}

		PROFILE_VERTEX startVertex=vertex_list[i];
		PROFILE_VERTEX endVertex=vertex_list[(i+1)%n];
		if(sys.part_map.bExportDeformedProfile)
		{	//考虑火曲变形
			if(bAdjustPlateUcs)
			{	//调整坐标系后计算火曲变形点
				startVertex.vertex=pPlank->GetDeformedVertex(startVertex.vertex,new_ucs);
				endVertex.vertex=pPlank->GetDeformedVertex(endVertex.vertex,new_ucs);
			}
			else
			{	
				startVertex.vertex=pPlank->GetDeformedVertex(startVertex.vertex);
				endVertex.vertex=pPlank->GetDeformedVertex(endVertex.vertex);
			}
		}
		TXTDIM_STRU txt_dim;
		if(startVertex.m_bWeldEdge&&startVertex.manu_space!=0)
		{	//输出加工间隙线
			f3dLine manu_cut_line(startVertex.vertex,endVertex.vertex);	//考虑了加工间隙的实际加工切割线
			GEPOINT offset_vec=manu_cut_line.endPt-manu_cut_line.startPt;
			if(startVertex.type==0)
			{
				normalize(offset_vec);
				offset_vec.Set(-offset_vec.y,offset_vec.x);	//逆时针旋转90°确定垂直与板边向里的间隙偏移方向
				manu_cut_line.startPt+=offset_vec*startVertex.manu_space;
				manu_cut_line.endPt+=offset_vec*startVertex.manu_space;
				line.startPt = manu_cut_line.startPt;
				line.endPt   = manu_cut_line.endPt;
				line.startPt.z = 0;
				line.endPt.z = 0;
				arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
			}
			else
			{
				if(startVertex.type==1)
					arcline.CreateMethod2(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,1),startVertex.sector_angle);
				else if(startVertex.type==2)
				{
					f3dPoint center_pick=startVertex.center;
					/*if(bAdjustPlateUcs)
					{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
						coord_trans(center_pick,pPlank->ucs,TRUE);
						coord_trans(center_pick,new_ucs,FALSE);
						center_pick.z=0;
					}*/
					int flag=1;
					if(startVertex.radius<0)
						flag=-1;
					startVertex.vertex.z=endVertex.vertex.z=0;	//起始点终止点Z坐标归零
					arcline.CreateMethod3(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,flag),flag*startVertex.radius,center_pick);
				}
				//将圆弧向里偏移出一个同心加工间隙圆
				int sign=1;
				if(arcline.WorkNorm().z<0)
					sign=-1;
				GEPOINT center=arcline.Center();
				offset_vec=arcline.Start()-center;
				normalize(offset_vec);
				manu_cut_line.startPt=arcline.Start()-offset_vec*(sign*startVertex.manu_space);
				offset_vec=arcline.End()-center;
				normalize(offset_vec);
				manu_cut_line.endPt=arcline.End()-offset_vec*(sign*startVertex.manu_space);
				if(startVertex.type==1)
					arcline.CreateMethod2(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,1),startVertex.sector_angle);
				else if(startVertex.type==2)
				{
					f3dPoint center_pick=startVertex.center;
					/*if(bAdjustPlateUcs)
					{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
						coord_trans(center_pick,pPlank->ucs,TRUE);
						coord_trans(center_pick,new_ucs,FALSE);
						center_pick.z=0;
					}*/
					int flag=1;
					if(startVertex.radius<0)
						flag=-1;
					startVertex.vertex.z=endVertex.vertex.z=0;	//起始点终止点Z坐标归零
					arcline.CreateMethod3(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,flag),flag*startVertex.radius,center_pick);
				}
			}
			arcline.pen.style=PS_DASH;
			edge_arr.Add(arcline);
			arcline.pen.style=PS_SOLID;
		}
		if(startVertex.type==0&&startVertex.m_bRollEdge&&startVertex.manu_space!=0)
		{	//输出卷边
			f3dLine roll_edge_line,huoqu_line;
			PROFILE_VERTEX prevVertex=vertex_list[(i+n-1)%n];
			PROFILE_VERTEX nextVertex=vertex_list[(i+2)%n];
			if(startVertex.vertex==HuoQuLine[0].startPt&&endVertex.vertex==HuoQuLine[0].endPt)
				continue;
			if(sys.part_map.bExportDeformedProfile)
			{
				if(bAdjustPlateUcs)
				{
					prevVertex.vertex=pPlank->GetDeformedVertex(prevVertex.vertex,new_ucs);
					nextVertex.vertex=pPlank->GetDeformedVertex(nextVertex.vertex,new_ucs);
				}
				else
				{
					prevVertex.vertex=pPlank->GetDeformedVertex(prevVertex.vertex);
					nextVertex.vertex=pPlank->GetDeformedVertex(nextVertex.vertex);
				}
			}
			//根据卷边高度以及卷边外移量绘制卷边相关线段 wht 11-04-09
			DrawRollEdgeLine(&startVertex,&endVertex,&prevVertex,&nextVertex,pPlank,roll_edge_line,huoqu_line,bPatternDrawing);
			arcline.CreateMethod2(roll_edge_line.startPt,roll_edge_line.endPt,f3dPoint(0,0,1),0);
			if(i==n-1&&indexOfFirstEdgeLine>=0)
			{
				edge_arr[indexOfFirstEdgeLine].SetStart(m_xRollEdgeLineEnd);
				m_bPrevIsRollEdgeLine=false;
			}
			//卷边处理后会多出3条轮廓边
			profileVertexList.append(huoqu_line.startPt);
			profileVertexList.append(roll_edge_line.startPt);
			profileVertexList.append(roll_edge_line.endPt);
			//标注顶点相对坐标值	wht 09-09-16
			if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
				DimCoordValue(pPlank,huoqu_line.startPt);
		}
		else
		{
			if(startVertex.type==0)
			{
				line.startPt = startVertex.vertex;
				if(m_bPrevIsRollEdgeLine&&startVertex.vertex.feature<10)
					line.startPt=m_xRollEdgeLineEnd;
				line.endPt   = endVertex.vertex;
				if(i==n-1&&endVertex.m_bRollEdge&&endVertex.manu_space!=0)
					line.endPt=m_xRollEdgeLineStart;
				line.startPt.z = 0;
				line.endPt.z = 0;
				arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
			}
			else if(startVertex.type==1)
				arcline.CreateMethod2(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,1),startVertex.sector_angle);
			else if(startVertex.type==2)
			{
				f3dPoint center_pick=startVertex.center;
				/*if(bAdjustPlateUcs)
				{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
					coord_trans(center_pick,pPlank->ucs,TRUE);
					coord_trans(center_pick,new_ucs,FALSE);
					center_pick.z=0;
				}*/
				int flag=1;
				if(startVertex.radius<0)
					flag=-1;
				startVertex.vertex.z=endVertex.vertex.z=0;	//起始点终止点Z坐标归零
				arcline.CreateMethod3(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,flag),flag*startVertex.radius,center_pick);
			}
			else if(startVertex.type==3)
			{
				int sign=1;
				f3dPoint center_pick=startVertex.center;
				f3dPoint column_norm=startVertex.column_norm;
				if(bAdjustPlateUcs)
				{
					coord_trans(center_pick,new_ucs,FALSE);
					vector_trans(column_norm,new_ucs,FALSE);
				}
				else 
				{
					coord_trans(center_pick,pPlank->ucs,FALSE);
					vector_trans(column_norm,pPlank->ucs,FALSE);
				}
				if(startVertex.radius<0)
					sign=-1;
				startVertex.vertex.z=endVertex.vertex.z=0;	//起始点终止点Z坐标归零 wht 11-06-23
				arcline.CreateEllipse(center_pick,startVertex.vertex,endVertex.vertex,column_norm,f3dPoint(0,0,sign),sign*startVertex.radius);
				//重新绘制钢板的椭圆弧边
				DrawPlateEllipseLine(pPlank,arcline);
			}
			m_bPrevIsRollEdgeLine=false;
			if(startVertex.type>0)
			{	
				double fMinRadius=20;
				if(sys.part_map.plate.bDimArcAngle&&pPlank->m_fInnerRadius<EPS&&arcline.Radius()>fMinRadius)
				{	//环向板无需标注角度
					f3dLine assistant_line;
					assistant_line.pen.style=PS_DASHDOT;
					assistant_line.startPt=arcline.Center();
					assistant_line.endPt=arcline.Start();
					edge_arr.Add(LineToArc(assistant_line));
					assistant_line.endPt=arcline.End();
					edge_arr.Add(LineToArc(assistant_line));
					//添加角度标注
					GEPOINT mid_pt=0.5*(arcline.Start()+arcline.End());
					GEPOINT center_vec=mid_pt-arcline.Center();
					normalize(center_vec);
					ANGULARDIM_STRU angular_dim;
					angular_dim.vertex=arcline.Center();
					angular_dim.startPos=0.5*(arcline.Center()+arcline.Start());
					angular_dim.endPos=0.5*(arcline.Center()+arcline.End());
					angular_dim.dimPos=arcline.Center()+center_vec*arcline.Radius()*0.5;
					angular_dim_arr.Add(angular_dim);
				}
				if(sys.part_map.plate.bDimArcEdgeR&&pPlank->m_fInnerRadius<EPS&&arcline.Radius()>fMinRadius)
				{	//标注圆弧半径
					ROTATEDIM_STRU rotate_dim;
					double half_angle=arcline.SectorAngle()*0.5;
					rotate_dim.startPos=sys.part_map.plate.bDimArcAngle?arcline.Start():arcline.PositionInAngle(half_angle);
					rotate_dim.endPos=arcline.Center();
					rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
					rotate_dim.dist = arcline.Radius();
					rotate_dim.angle=Cal2dLineAng(rotate_dim.startPos.x,rotate_dim.startPos.y,rotate_dim.endPos.x,rotate_dim.endPos.y);
					rotate_dim.bDisplayDimLine=TRUE;
					rotate_dim_arr.Add(rotate_dim);
				}
			}
			int iNewInsert=edge_arr.Add(arcline);
			if(indexOfFirstEdgeLine<0)
				indexOfFirstEdgeLine=iNewInsert;
			//记录钢板外形轮廓点 wht 15-08-12
			profileVertexList.append(arcline.Start());
			//标注顶点相对坐标值	wht 09-09-16
			if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
				DimCoordValue(pPlank,arcline.Start());
		}
		if(startVertex.m_bWeldEdge)
			ProcessPlateWeldEdge(&startVertex,arcline);	//处理焊缝边(绘制焊缝线、绘制坡口线、标注焊接件号、标注坡口尺寸等) wht 18-08-09
		//
		Sub_Pnt(start_vec,line.endPt,line.startPt);
		if(max_edge_len<DISTANCE(arcline.Start(),arcline.End()))
		{
			max_edge_len=DISTANCE(arcline.Start(),arcline.End());
			long_edge_mid=0.5*(arcline.Start()+arcline.End());
			long_edge_angle=Cal2dLineAng(arcline.Start().x,arcline.Start().y,arcline.End().x,arcline.End().y);
		}
	}
	//初始化钢板外形轮廓区域 wht 15-08-12
	if(profileVertexList.GetSize()>0)
		profileRegion.CreatePolygonRgn(profileVertexList.m_pData,profileVertexList.GetSize());
	//标注钢板边长
	if(sys.part_map.plate.bDimPlateEdgeLen)
	{
		int iStart=text_dim_arr.GetCount();
		for(int i=0;i<edge_arr.GetCount();i++)
		{
			f3dArcLine arc_line=edge_arr.GetAt(i);
			if(arc_line.SectorAngle()==0&&arc_line.pen.style==PS_SOLID)
				DimPlateEdgeLen(f3dLine(arc_line.Start(),arc_line.End()));
		}
		//青岛汇金通要求合并共线的边长标注
		if(gxDrwLocalizer.ciCustomerSerial==10)
			MergeParallelEdgeDim(iStart);
	}
	//处理火曲线
	f3dLine huoquline = pPlank->HuoQuLine[0];
	PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL;
	pPlank->GetBendlineVertexies(&pBendLine1Start,&pBendLine1End,NULL,NULL);
	if(pBendLine1Start&&pBendLine1End)
	{
		huoquline.startPt=pBendLine1Start->vertex;
		huoquline.endPt  =pBendLine1End->vertex; 
	}
	line=huoquline;
	if(sys.part_map.bExportDeformedProfile)
	{
		line.startPt=pPlank->GetDeformedVertex(line.startPt);
		line.endPt=pPlank->GetDeformedVertex(line.endPt);
	}
	if(bAdjustPlateUcs)
	{	//标注相对坐标 wht 09-09-17 该代码应在GetDeformedVertex()之前 wht 11-02-27
		coord_trans(line.startPt,pPlank->ucs,TRUE);
		coord_trans(line.startPt,new_ucs,FALSE);
		coord_trans(line.endPt,pPlank->ucs,TRUE);
		coord_trans(line.endPt,new_ucs,FALSE);
	}
	line.pen.style=PS_DASHDOTDOT;
	edge_arr.Add(LineToArc(line));
	if(sys.part_map.plate.bMarkHuoquPos)
		MarkHuoquLine(line);
	//判断火曲方式(内曲、外曲)
	if(bAdjustPlateUcs)
	{	
		coord_trans(base_face_pick,new_ucs,TRUE);
		coord_trans(base_face_pick,pPlank->ucs,FALSE);
	}
	f3dPoint prj_base_pick,base_vec;
	SnapPerp(&prj_base_pick,huoquline,base_face_pick);
	base_vec=base_face_pick-prj_base_pick;
	normalize(base_vec);
	norm = pPlank->huoqufacenorm[0];
	vector_trans(norm,pPlank->ucs,FALSE);
	BOOL bNeiQu=TRUE;
	double dd = cal_angle_of_2vec(base_vec,norm);
	if(dd<Pi/2||(fabs(dd-Pi/2)<EPS&&norm.z>0))	//火曲面向外曲(正曲)
		bNeiQu=FALSE;
	m_ciHuoQuType=bNeiQu?1:2;
	if(bAdjustPlateUcs)
	{	//标注相对坐标 wht 09-09-17 该代码应在GetDeformedVertex()之前 wht 11-02-27
		vector_trans(base_vec,pPlank->ucs,TRUE);
		vector_trans(base_vec,new_ucs,FALSE);
	}
	//添加火曲线工艺内容标注(在火曲线的右侧)
	TXTDIM_STRU txt_dim;
	txt_dim.txtHeight=sys.fonts.plate.fDimTextSize;	//火曲工艺内容字体高度独立设置
	txt_dim.dimPos=0.5*(line.startPt+line.endPt);
	txt_dim.dimVec.Set(-base_vec.y,base_vec.x,0);
	double huoquangle=pPlank->GetHuoQuAngle(1);
	GetPlateHuoQuProcessStr(txt_dim,bNeiQu,huoquangle*DEGTORAD_COEF,pPlank->m_fDeformCoef,pPlank->GetThick());
	double fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
	txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
	text_dim_arr.Add(txt_dim);
	//添加火曲线方向的投影面（在火曲线的左侧）
	huoqu_mark[0].prj_vec=line.endPt-line.startPt;
	normalize(huoqu_mark[0].prj_vec);
	huoqu_mark[0].origin=0.5*(line.startPt+line.endPt);
	huoqu_mark[0].offset_dist=-10;
	huoqu_mark[0].offset_vec=base_vec;
	huoqu_mark[0].line_arr[0].startPt=base_vec;
	huoqu_mark[0].line_arr[0].endPt=base_vec+huoqu_mark[0].prj_vec*15.0;
	huoqu_mark[0].line_arr[1].startPt=huoqu_mark[0].line_arr[0].endPt;
	huoqu_mark[0].line_arr[1].endPt=huoqu_mark[0].line_arr[1].startPt+base_vec*2.0-huoqu_mark[0].prj_vec*2.0;
	huoqu_mark[0].line_arr[2].startPt=huoqu_mark[0].line_arr[1].endPt;
	huoqu_mark[0].line_arr[2].endPt=huoqu_mark[0].line_arr[2].startPt-base_vec*2.0-huoqu_mark[0].prj_vec*2.0;
	//剖面图
	f3dPoint startPt=huoqu_mark[0].line_arr[0].endPt+huoqu_mark[0].prj_vec*5.0-base_vec;
	huoqu_mark[0].line_arr[3].startPt=startPt;
	huoqu_mark[0].line_arr[3].endPt=startPt+base_vec*15;
	huoqu_mark[0].line_arr[4].startPt=startPt;
	huoqu_mark[0].line_arr[4].endPt=startPt+base_vec*10;
	double fPlaneAngle=bNeiQu?Pi-fabs(huoquangle):fabs(huoquangle)-Pi;
	rotate_point_around_axis(huoqu_mark[0].line_arr[4].endPt,fPlaneAngle,f3dLine(startPt,startPt+f3dPoint(0,0,1)*10));
	huoqu_mark[0].plane_angle=fPlaneAngle;
	if(!bPatternDrawing&&sys.part_map.plate.bDrawHuoQuPrjMap)
		AddHuoQuPrjMap(line,huoqu_mark[0]);
	//生成钢板内圆环 wht 11-07-09
	if(pPlank->m_fInnerRadius>0)	
		CreatePlateInnerCir(pPlank,bPatternDrawing,bAdjustPlateUcs);
	//标注螺栓相对坐标值	wht 09-09-16
	UCS_STRU old_ucs=pPlank->ucs;//设置板的坐标系为调整后的坐标系，获得正确的螺栓位置
	//if(bAdjustPlateUcs)
	//	pPlank->ucs=new_ucs;
	for(pLsRef=pPlank->GetFirstLsRef();pLsRef!=NULL;pLsRef=pPlank->GetNextLsRef())
	{
		int iLsFace=pPlank->GetNormFaceI((*pLsRef)->get_norm());
		pPlank->SpreadLsPos(pLsRef,ls_cir.centre);
		if(bAdjustPlateUcs)
		{	//该代码应在GetDeformedVertex()之前 wht 11-02-27
			coord_trans(ls_cir.centre,pPlank->ucs,TRUE);
			coord_trans(ls_cir.centre,new_ucs,FALSE);
		}
		if(sys.part_map.bExportDeformedProfile)
		{	//调整坐标系后计算火曲面顶点坐标系时应保证ls_cir.centre为new_ucs坐标系下的相对坐标 wht 11-02-27
			ls_cir.centre.feature=iLsFace+1;
			if(bAdjustPlateUcs)
				ls_cir.centre=pPlank->GetDeformedVertex(ls_cir.centre,new_ucs);
			else
				ls_cir.centre=pPlank->GetDeformedVertex(ls_cir.centre);
		}
		ls_cir.centre.z = 0;		//必须清零，不然螺栓坐标间距计算不准
		
		//标注坐标时也有可能要分析螺栓射线
		for(int i=1;i<=32;i++)
		{
			if(pLsRef->dwRayNo&GetSingleWord(i))
			{
				GEPOINT resideRayVec=pLsRef->rayVecArr[i-1];
				if(bAdjustPlateUcs)
				{
					vector_trans(resideRayVec,pPlank->ucs,TRUE);
					vector_trans(resideRayVec,new_ucs,FALSE);
				}
				AnalizeLsRay(ls_cir.centre,i,resideRayVec);
			}
		}
		ls_cir.radius = (*pLsRef)->get_d()/2.0;
		ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
		ls_cir.pen.style=(*pLsRef)->FilterFuncType();	//记录螺栓孔的功用类型
		ls_cir.centre.pen.style=pLsRef->cFlag;			//记录螺栓孔的工艺类型
		if((int)pLsRef->waistLen>0)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width=(int)pLsRef->waistLen;		//使用画笔宽度保存腰圆孔长度 
			ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
			if(bAdjustPlateUcs)
				vector_trans(ls_cir.norm,new_ucs,FALSE);
			else
				vector_trans(ls_cir.norm,pPlank->ucs,FALSE);//转换到钢板坐标系下
		}
		cir_arr.Add(ls_cir);
		//标注螺栓相对坐标值	wht 09-09-16
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
			DimCoordValue(pPlank,ls_cir.centre);
	}
	//绘制钢板定位点
	DrawPlateLocateFlag(pPlank,bAdjustPlateUcs);

	if(bAdjustPlateUcs)
		pPlank->ucs=old_ucs;	//恢复板坐标系
	//螺栓射线处理
	if(sys.part_map.plate.bDimPlateRayLs)
	{
		//DimLsEndSpace(pPlank);
		DimLsRay(pPlank,bAdjustPlateUcs);
	}
	if(bPatternDrawing)//大样图中需要在焊烧死角处切弧
		CreateLineOnWeldCorner(pPlank,&vertex_list);
	//大样图中绘制钢板的内侧轮廓边
	if(sys.part_map.plate.bDrawInnerEdge)
		DrawInnerEdge(pPlank,&vertex_list);
	//青岛东方要求大样图中不绘制焊接子构件
	if(!(gxDrwLocalizer.ciCustomerSerial==5&&bPatternDrawing))
		DrawWeldSubPlate(pPlank,bAdjustPlateUcs);
	//分析杆件射线
	if(sys.part_map.plate.bDimCoord)
		AnalizePoleRay(pPlank);
	return TRUE;
}

bool GetComplementFaceVertex(CLDSPlate *pPlate,f3dPoint &top_vertex,f3dPoint &vertex1,f3dPoint &vertex2,
							 f3dPoint &vertex1_org,f3dPoint &vertex2_org)
{
	if(!pPlate->IsPeakPointPlate())
		return false;
	if(!pPlate->IsHuoQuLineCollinear3FacePlate())
	{
		top_vertex=pPlate->top_point;
		return pPlate->GetComplementFaceVertex(vertex1,vertex2);
	}
	else
	{
		PROFILE_VERTEX *pPrevVertex=NULL,*pCurVertex=NULL,*pNextVertex=NULL,*pNextNextVertex=NULL;
		int n=pPlate->vertex_list.GetNodeNum();
		for(int i=0;i<n;i++)
		{
			pPrevVertex=pPlate->vertex_list.GetByIndex((i-1+n)%n);
			pCurVertex=pPlate->vertex_list.GetByIndex(i);
			pNextVertex=pPlate->vertex_list.GetByIndex((i+1)%n);
			pNextNextVertex=pPlate->vertex_list.GetByIndex((i+2)%n);
			if( pPrevVertex&&pCurVertex&&pNextVertex&&pNextNextVertex&&
				pCurVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BEND_EXTEND_POINT&&
				pNextVertex->m_cPosDesignType==PROFILE_VERTEX::DT_BEND_EXTEND_POINT&&
				(pPrevVertex->vertex.feature+pNextNextVertex->vertex.feature==5))
			{
				vertex1_org=pPrevVertex->vertex;
				vertex2_org=pNextNextVertex->vertex;
				vertex1=pPlate->GetRealVertex(pPrevVertex->vertex);
				vertex2=pPlate->GetRealVertex(pNextNextVertex->vertex);
				f3dPoint vertex3=pPlate->GetRealVertex(pCurVertex->vertex);
				f3dPoint vertex4=pPlate->GetRealVertex(pNextVertex->vertex);
				top_vertex=(vertex3+vertex4)*0.5;
				return true;
			}
		}
		return false;
	}
}

BOOL CPlankDrawing::Create3FacePlank(CLDSPlate *pPlank,BOOL bPatternDrawing/*=TRUE*/)
{
	int i,n;
	//start_i,end_i仅用在三面板中表示剪切面起终顶点号
	int start_i=0,end_i=0;
	f3dLine line;
	f3dArcLine arcline;
	f3dCircle ls_cir;
	f3dPoint top_pnt,vec,line_vec;
	f3dPoint basePt,base_face_pick,qu_face_pick[2],dimPos,start_vec,end_vec,dim_vec,norm;
	CLsRef *pLsRef;
	ANGULARDIM_STRU angular_dim;

	double left=0,top=0,right=0,bottom=0;
	f3dPoint left_pt,top_pt,right_pt,bottom_pt;
	line.feature = 0;
	n = pPlank->vertex_list.GetNodeNum();
	if(n<3)
		return FALSE;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	f3dLine HuoQuLine[2];
	HuoQuLine[0]=pPlank->HuoQuLine[0];	//HuoQuLine[0].Layer中保存了火曲半径，以及中心层系数
	HuoQuLine[1]=pPlank->HuoQuLine[1];	//HuoQuLine[1].Layer中保存了火曲半径，以及中心层系数
	/*HuoQuLine[0].startPt=pPlank->HuoQuLine[0].startPt;
	HuoQuLine[0].endPt=pPlank->HuoQuLine[0].endPt;
	HuoQuLine[1].startPt=pPlank->HuoQuLine[1].startPt;
	HuoQuLine[1].endPt=pPlank->HuoQuLine[1].endPt;*/
	f3dPoint top_point=pPlank->top_point;
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||(sys.part_map.plate.bDimCoord&&!bPatternDrawing))
		bAdjustPlateUcs=TRUE;
	if(m_bSpecifyUCS) //指定绘图坐标系优先于内部调整坐标系 wht 10-08-13
	{
		bAdjustPlateUcs=TRUE;
		PROFILE_VERTEX *pVertex=NULL;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,pPlank->ucs,TRUE);
			coord_trans(pVertex->vertex,new_ucs,FALSE);
			if(pVertex->type==1||pVertex->type==2)
			{	//转换圆心拾取点
				coord_trans(pVertex->center,pPlank->ucs,TRUE);
				coord_trans(pVertex->center,new_ucs,FALSE);
			}
		}
		if(pPlank->face_N==3)
		{
			coord_trans(HuoQuLine[0].startPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[0].startPt,new_ucs,FALSE);
			coord_trans(HuoQuLine[0].endPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[0].endPt,new_ucs,FALSE);
			coord_trans(HuoQuLine[1].startPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[1].startPt,new_ucs,FALSE);
			coord_trans(HuoQuLine[1].endPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[1].endPt,new_ucs,FALSE);
			coord_trans(top_point,pPlank->ucs,TRUE);
			coord_trans(top_point,new_ucs,FALSE);
		}
		//计算几何中心
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pPlank->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=pPlank->vertex_list.GetNodeNum();
		project_point(geom_center,pPlank->ucs.origin,pPlank->ucs.axis_z);
	}
	else //if(bAdjustPlateUcs)	//标注坐标
	{
		UCS_STRU ucs;
		short style=0;
		if(!bAdjustPlateUcs) style=1;
		if(AdjustPlateUcs(pPlank,ucs,geom_center,style)>EPS2)
		{
			PROFILE_VERTEX *pVertex=NULL;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				coord_trans(pVertex->vertex,pPlank->ucs,TRUE);
				coord_trans(pVertex->vertex,ucs,FALSE);
				if(pVertex->type==1||pVertex->type==2)
				{	//转换圆心拾取点
					coord_trans(pVertex->center,pPlank->ucs,TRUE);
					coord_trans(pVertex->center,ucs,FALSE);
				}
			}
			if(pPlank->face_N==3)
			{
				coord_trans(HuoQuLine[0].startPt,pPlank->ucs,TRUE);
				coord_trans(HuoQuLine[0].startPt,ucs,FALSE);
				coord_trans(HuoQuLine[0].endPt,pPlank->ucs,TRUE);
				coord_trans(HuoQuLine[0].endPt,ucs,FALSE);
				coord_trans(HuoQuLine[1].startPt,pPlank->ucs,TRUE);
				coord_trans(HuoQuLine[1].startPt,ucs,FALSE);
				coord_trans(HuoQuLine[1].endPt,pPlank->ucs,TRUE);
				coord_trans(HuoQuLine[1].endPt,ucs,FALSE);
				coord_trans(top_point,pPlank->ucs,TRUE);
				coord_trans(top_point,ucs,FALSE);
			}
			new_ucs=ucs;	//记录调整后的坐标系
			bAdjustPlateUcs=TRUE;
		}
		else
		{
			new_ucs=pPlank->ucs;
			for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
			{
				f3dPoint vertex=pVertex->vertex;
				coord_trans(vertex,pPlank->ucs,TRUE);
				geom_center+=vertex;
			}
			geom_center/=pPlank->vertex_list.GetNodeNum();
			project_point(geom_center,pPlank->ucs.origin,pPlank->ucs.axis_z);
		}
	}
	//生成连接板的外包轮廓线
	base_face_pick=pPlank->ucs.origin;
	double max_edge_len=0;
	ARRAY_LIST<f3dPoint> profileVertexList;
	bool basepick_finded=false,huoqu1pick_finded=false,huoqu2pick_finded=false;
	int indexOfFirstEdgeLine=-1;
	m_bPrevIsRollEdgeLine=false;
	for(i=0;i<n;i++)
	{
		f3dPoint pt = vertex_list[i].vertex;
		//保证基准面和火曲面的拾取点不在火曲线上，否则无法正确判断内外曲 wht 11-04-21
		/** 后面添加fabs(dist)>10判断是为了避免连板轮廓顶点与火曲点接近（常出现于合并生成火曲连接板），
			且基面火曲面错乱时，导致错误 wjh-2016.4.29
		*/
		double dist1=0,dist2=0;
		if(!basepick_finded||!huoqu1pick_finded||!huoqu2pick_finded)
		{
			dist1=DistOf2dPtLine(pt,HuoQuLine[0].startPt,HuoQuLine[0].endPt);
			dist2=DistOf2dPtLine(pt,HuoQuLine[1].startPt,HuoQuLine[1].endPt);
		}
		//保证基准面和火曲面的拾取点不在火曲线上，否则无法正确判断内外曲 wht 11-04-21
		if(fabs(dist1)>10&&fabs(dist2)>10&&HuoQuLine[0].PtInLine(pt)==0&&HuoQuLine[1].PtInLine(pt)==0)
		{
			if(!basepick_finded&&pt.feature==1)		//基准面拾取点
			{
				basepick_finded=true;
				base_face_pick=pt;
			}
			else if(!huoqu1pick_finded&&pt.feature==2)
			{
				huoqu1pick_finded=true;
				qu_face_pick[0]=pt;
			}
			else if(!huoqu2pick_finded&&pt.feature==3)	//以前没有判断条件，剩下的就是第三面的，这样对火曲点不合理。 WJH-2004/4/27
			{
				huoqu2pick_finded=true;
				qu_face_pick[1]=pt;
			}
		}
		if(pt.x<left)
		{
			left_pt = pt;
			left=pt.x;
		}
		if(pt.x>right)
		{
			right_pt = pt;
			right = pt.x;
		}
		if(pt.y>top)
		{
			top_pt=pt;
			top = pt.y;
		}
		if(pt.y<bottom)
		{
			bottom_pt = pt;
			bottom = pt.y;
		}

		PROFILE_VERTEX startVertex=vertex_list[i];
		PROFILE_VERTEX endVertex=vertex_list[(i+1)%n];
		if(startVertex.vertex.feature==2&&endVertex.vertex.feature==3
			||startVertex.vertex.feature==3&&endVertex.vertex.feature==2)
			continue;
		if(sys.part_map.bExportDeformedProfile)
		{	//考虑火曲变形
			if(bAdjustPlateUcs)
			{
				startVertex.vertex=pPlank->GetDeformedVertex(startVertex.vertex,new_ucs);
				endVertex.vertex=pPlank->GetDeformedVertex(endVertex.vertex,new_ucs);
			}
			else
			{
				startVertex.vertex=pPlank->GetDeformedVertex(startVertex.vertex);
				endVertex.vertex=pPlank->GetDeformedVertex(endVertex.vertex);
			}
		}
		TXTDIM_STRU txt_dim;
		if(startVertex.m_bWeldEdge&&startVertex.manu_space!=0)
		{	//输出加工间隙线
			f3dLine manu_cut_line(startVertex.vertex,endVertex.vertex);	//考虑了加工间隙的实际加工切割线
			GEPOINT offset_vec=manu_cut_line.endPt-manu_cut_line.startPt;
			if(startVertex.type==0)
			{
				normalize(offset_vec);
				offset_vec.Set(-offset_vec.y,offset_vec.x);	//逆时针旋转90°确定垂直与板边向里的间隙偏移方向
				manu_cut_line.startPt+=offset_vec*startVertex.manu_space;
				manu_cut_line.endPt+=offset_vec*startVertex.manu_space;
				line.startPt = manu_cut_line.startPt;
				line.endPt   = manu_cut_line.endPt;
				line.startPt.z = 0;
				line.endPt.z = 0;
				arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
			}
			else
			{
				if(startVertex.type==1)
					arcline.CreateMethod2(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,1),startVertex.sector_angle);
				else if(startVertex.type==2)
				{	
					f3dPoint center_pick=startVertex.center;
					/*if(bAdjustPlateUcs)
					{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
						coord_trans(center_pick,pPlank->ucs,TRUE);
						coord_trans(center_pick,new_ucs,FALSE);
						center_pick.z=0;
					}*/
					int flag=1;
					if(startVertex.radius<0)
						flag=-1;
					startVertex.vertex.z=endVertex.vertex.z=0;	//起始点终止点Z坐标归零
					arcline.CreateMethod3(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,flag),flag*startVertex.radius,center_pick);
				}
				//将圆弧向里偏移出一个同心加工间隙圆
				int sign=1;
				if(arcline.WorkNorm().z<0)
					sign=-1;
				GEPOINT center=arcline.Center();
				offset_vec=arcline.Start()-center;
				normalize(offset_vec);
				manu_cut_line.startPt=arcline.Start()-offset_vec*(sign*startVertex.manu_space);
				offset_vec=arcline.End()-center;
				normalize(offset_vec);
				manu_cut_line.endPt=arcline.End()-offset_vec*(sign*startVertex.manu_space);
				if(startVertex.type==1)
					arcline.CreateMethod2(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,1),startVertex.sector_angle);
				else if(startVertex.type==2)
				{
					f3dPoint center_pick=startVertex.center;
					/*if(bAdjustPlateUcs)
					{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
						coord_trans(center_pick,pPlank->ucs,TRUE);
						coord_trans(center_pick,new_ucs,FALSE);
						center_pick.z=0;
					}*/
					int flag=1;
					if(startVertex.radius<0)
						flag=-1;
					startVertex.vertex.z=endVertex.vertex.z=0;	//起始点终止点Z坐标归零
					arcline.CreateMethod3(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,flag),flag*startVertex.radius,center_pick);
				}
			}
			arcline.pen.style=PS_DASH;
			edge_arr.Add(arcline);
			arcline.pen.style=PS_SOLID;
		}
		if(startVertex.type==0&&startVertex.m_bRollEdge&&startVertex.manu_space!=0)
		{	//输出卷边
			f3dLine roll_edge_line,huoqu_line;
			PROFILE_VERTEX *pPrevVertex=&vertex_list[(i+n-1)%n];
			PROFILE_VERTEX *pNextVertex=&vertex_list[(i+2)%n];
			if((startVertex.vertex==HuoQuLine[0].startPt&&endVertex.vertex==HuoQuLine[0].endPt)||
				(startVertex.vertex==HuoQuLine[1].startPt&&endVertex.vertex==HuoQuLine[1].endPt))
				continue;
			if(sys.part_map.bExportDeformedProfile)
			{
				if(bAdjustPlateUcs)
				{
					pPrevVertex->vertex=pPlank->GetDeformedVertex(pPrevVertex->vertex,new_ucs);
					pNextVertex->vertex=pPlank->GetDeformedVertex(pNextVertex->vertex,new_ucs);
				}
				else
				{
					pPrevVertex->vertex=pPlank->GetDeformedVertex(pPrevVertex->vertex);
					pNextVertex->vertex=pPlank->GetDeformedVertex(pNextVertex->vertex);
				}
			}
			//根据卷边高度以及卷边外移量绘制卷边相关线段 wht 11-04-09
			DrawRollEdgeLine(&startVertex,&endVertex,pPrevVertex,pNextVertex,pPlank,roll_edge_line,huoqu_line,bPatternDrawing);
			arcline.CreateMethod2(roll_edge_line.startPt,roll_edge_line.endPt,f3dPoint(0,0,1),0);
			if(i==n-1&&indexOfFirstEdgeLine>=0)
			{
				edge_arr[indexOfFirstEdgeLine].SetStart(m_xRollEdgeLineEnd);
				m_bPrevIsRollEdgeLine=false;
			}
			//卷边处理后会多出3条轮廓边
			profileVertexList.append(huoqu_line.startPt);
			profileVertexList.append(roll_edge_line.startPt);
			profileVertexList.append(roll_edge_line.endPt);
			//标注顶点相对坐标值	wht 09-09-16
			if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
				DimCoordValue(pPlank,huoqu_line.startPt);
		}
		else
		{
			if(startVertex.type==0)
			{
				line.startPt = startVertex.vertex;
				if(m_bPrevIsRollEdgeLine&&startVertex.vertex.feature<10)
					line.startPt=m_xRollEdgeLineEnd;
				line.endPt   = endVertex.vertex;
				if(i==n-1&&endVertex.m_bRollEdge&&endVertex.manu_space!=0)
					line.endPt=m_xRollEdgeLineStart;
				line.startPt.z = 0;
				line.endPt.z = 0;
				arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
			}
			else if(startVertex.type==1)
				arcline.CreateMethod2(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,1),startVertex.sector_angle);
			else if(startVertex.type==2)
			{	
				f3dPoint center_pick=startVertex.center;
				/*if(bAdjustPlateUcs)
				{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
					coord_trans(center_pick,pPlank->ucs,TRUE);
					coord_trans(center_pick,new_ucs,FALSE);
					center_pick.z=0;
				}*/
				int flag=1;
				if(startVertex.radius<0)
					flag=-1;
				startVertex.vertex.z=endVertex.vertex.z=0;	//起始点终止点Z坐标归零
				arcline.CreateMethod3(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,flag),flag*startVertex.radius,center_pick);
			}
			else if(startVertex.type==3)
			{
				int sign=1;
				f3dPoint center_pick=startVertex.center;
				f3dPoint column_norm=startVertex.column_norm;
				if(bAdjustPlateUcs)
				{
					coord_trans(center_pick,new_ucs,FALSE);
					vector_trans(column_norm,new_ucs,FALSE);
				}
				else
				{
					coord_trans(center_pick,pPlank->ucs,FALSE);
					vector_trans(column_norm,pPlank->ucs,FALSE);
				}
				if(startVertex.radius<0)
					sign=-1;
				startVertex.vertex.z=endVertex.vertex.z=0;	//起始点终止点Z坐标归零 wht 11-06-23
				arcline.CreateEllipse(center_pick,startVertex.vertex,endVertex.vertex,column_norm,f3dPoint(0,0,sign),sign*startVertex.radius);
				//重新绘制钢板的椭圆弧边
				DrawPlateEllipseLine(pPlank,arcline);
			}
			m_bPrevIsRollEdgeLine=false;
			int iNewInsert=edge_arr.Add(arcline);
			if(indexOfFirstEdgeLine<0)
				indexOfFirstEdgeLine=iNewInsert;
			//记录钢板外形轮廓点 wht 15-08-12
			profileVertexList.append(arcline.Start());
			//标注顶点相对坐标值	wht 09-09-16
			if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
				DimCoordValue(pPlank,arcline.Start());
		}
		if(startVertex.m_bWeldEdge)
			ProcessPlateWeldEdge(&startVertex,arcline);//处理焊缝边(绘制焊缝线、绘制坡口线、标注焊接件号、标注坡口尺寸等) wht 18-08-09
		//
		//Sub_Pnt(start_vec,line.endPt,line.startPt);
		if(max_edge_len<DISTANCE(arcline.Start(),arcline.End()))
		{
			max_edge_len=DISTANCE(arcline.Start(),arcline.End());
			long_edge_mid=0.5*(arcline.Start()+arcline.End());
			long_edge_angle=Cal2dLineAng(arcline.Start().x,arcline.Start().y,arcline.End().x,arcline.End().y);
		}
	}
	//初始化钢板外形轮廓区域 wht 15-08-12
	if(profileVertexList.GetSize()>0)
		profileRegion.CreatePolygonRgn(profileVertexList.m_pData,profileVertexList.GetSize());
	//标注钢板边长
	if(sys.part_map.plate.bDimPlateEdgeLen)
	{
		int iStart=text_dim_arr.GetCount();
		for(int i=0;i<edge_arr.GetCount();i++)
		{
			f3dArcLine arc_line=edge_arr.GetAt(i);
			if(arc_line.SectorAngle()==0&&arc_line.pen.style==PS_SOLID)
				DimPlateEdgeLen(f3dLine(arc_line.Start(),arc_line.End()));
		}
		//青岛汇金通要求合并共线的边长标注
		if(gxDrwLocalizer.ciCustomerSerial==10)
			MergeParallelEdgeDim(iStart);
	}
	//生成三面板的板顶点(三面交汇点)"十"字标记
	line.startPt.Set(top_point.x-5,top_point.y);
	line.endPt.Set(top_point.x+5,top_point.y);
	arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
	edge_arr.Add(arcline);	//水平线
	line.startPt.Set(top_point.x,top_point.y-5);
	line.endPt.Set(top_point.x,top_point.y+5);
	arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
	edge_arr.Add(arcline);	//坚直线
	//处理火曲线
	PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL,*pBendLine2Start=NULL,*pBendLine2End=NULL;
	pPlank->GetBendlineVertexies(&pBendLine1Start,&pBendLine1End,&pBendLine2Start,&pBendLine2End);
	for(i=0;i<2;i++)
	{	//添加火曲线
		f3dLine huoquline=pPlank->HuoQuLine[i];
		PROFILE_VERTEX* pBendLineStart=(i==0)?pBendLine1Start:pBendLine2Start;
		if(pBendLineStart)
			huoquline.startPt=pBendLineStart->vertex;
		else
			huoquline.startPt=pPlank->top_point;
		PROFILE_VERTEX* pBendLineEnd=(i==0)?pBendLine1End:pBendLine2End;
		if(pBendLineEnd)
			huoquline.endPt=pBendLineEnd->vertex;
		else
			huoquline.endPt=pPlank->top_point;
		if(huoquline.startPt.IsEqual(huoquline.endPt))
		{	//火曲线查找失败后，根据top_point计算
			top_pnt=pPlank->top_point;
			if(DISTANCE(pPlank->HuoQuLine[i].startPt,top_pnt)>DISTANCE(pPlank->HuoQuLine[i].endPt,top_pnt))
				Sub_Pnt(vec,pPlank->HuoQuLine[i].startPt,top_pnt);
			else
				Sub_Pnt(vec,pPlank->HuoQuLine[i].endPt,top_pnt);
			normalize(vec);
			huoquline.startPt = top_pnt;
			double len = __max(fabs(left_pt.x-right_pt.x),fabs(top_pt.y-bottom_pt.y))/2;
			Add_Pnt(huoquline.endPt,top_pnt,vec*len);
		}
		line=huoquline;
		if(sys.part_map.bExportDeformedProfile)
		{	//考虑火曲变形
			line.startPt.feature=line.endPt.feature=10+i+2;
			line.startPt=pPlank->GetDeformedVertex(line.startPt);
			line.endPt=pPlank->GetDeformedVertex(line.endPt);
		}
		if(bAdjustPlateUcs)
		{	//标注相对坐标 wht 09-09-17 该代码应在GetDeformedVertex()之前 wht 11-02-27
			coord_trans(line.startPt,pPlank->ucs,TRUE);
			coord_trans(line.startPt,new_ucs,FALSE);
			coord_trans(line.endPt,pPlank->ucs,TRUE);
			coord_trans(line.endPt,new_ucs,FALSE);
		}
		line.pen.style=PS_DASHDOTDOT;
		edge_arr.Add(LineToArc(line));
		if(sys.part_map.plate.bMarkHuoquPos)
			MarkHuoquLine(line);
		//判断火曲方式(内曲、外曲)
		f3dPoint base_pick=base_face_pick;
		if(bAdjustPlateUcs)
		{	
			coord_trans(base_pick,new_ucs,TRUE);
			coord_trans(base_pick,pPlank->ucs,FALSE);
		}
		f3dPoint prj_base_pick,base_vec;
		SnapPerp(&prj_base_pick,huoquline,base_pick);
		base_vec=base_pick-prj_base_pick;
		normalize(base_vec);
		norm = pPlank->huoqufacenorm[i];
		vector_trans(norm,pPlank->ucs,FALSE);
		BOOL bNeiQu=TRUE;
		double dd = cal_angle_of_2vec(base_vec,norm);
		if(dd<Pi/2||(fabs(dd-Pi/2)<EPS&&norm.z>0))	//火曲面向外曲(正曲)
			bNeiQu=FALSE;
		if(bAdjustPlateUcs)
		{	
			vector_trans(base_vec,pPlank->ucs,TRUE);
			vector_trans(base_vec,new_ucs,FALSE);
		}
		//添加火曲内容标注
		TXTDIM_STRU txt_dim;
		txt_dim.txtHeight=sys.fonts.plate.fDimTextSize;	//火曲工艺内容字体高度独立设置
		txt_dim.dimPos=0.5*(line.startPt+line.endPt);
		txt_dim.dimVec.Set(-base_vec.y,base_vec.x,0);
		double huoquangle=pPlank->GetHuoQuAngle(i+1);
		GetPlateHuoQuProcessStr(txt_dim,bNeiQu,huoquangle*DEGTORAD_COEF,pPlank->m_fDeformCoef,pPlank->GetThick());
		double fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
		txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
		text_dim_arr.Add(txt_dim);
		//添加火曲线方向的投影面
		huoqu_mark[i].prj_vec=line.endPt-line.startPt;
		normalize(huoqu_mark[i].prj_vec);
		huoqu_mark[i].origin=0.5*(line.startPt+line.endPt);
		huoqu_mark[i].offset_dist=-10;
		huoqu_mark[i].offset_vec=base_vec;
		huoqu_mark[i].line_arr[0].startPt=base_vec;
		huoqu_mark[i].line_arr[0].endPt=base_vec+huoqu_mark[i].prj_vec*15.0;
		huoqu_mark[i].line_arr[1].startPt=huoqu_mark[i].line_arr[0].endPt;
		huoqu_mark[i].line_arr[1].endPt=huoqu_mark[i].line_arr[1].startPt+base_vec*2.0-huoqu_mark[i].prj_vec*2.0;
		huoqu_mark[i].line_arr[2].startPt=huoqu_mark[i].line_arr[1].endPt;
		huoqu_mark[i].line_arr[2].endPt=huoqu_mark[i].line_arr[2].startPt-base_vec*2.0-huoqu_mark[i].prj_vec*2.0;
		//剖面图
		f3dPoint startPt=huoqu_mark[i].line_arr[0].endPt+huoqu_mark[i].prj_vec*5.0-base_vec;
		huoqu_mark[i].line_arr[3].startPt=startPt;
		huoqu_mark[i].line_arr[3].endPt=startPt+base_vec*15;
		huoqu_mark[i].line_arr[4].startPt=startPt;
		huoqu_mark[i].line_arr[4].endPt=startPt+base_vec*10;
		double fPlaneAngle=bNeiQu?Pi-fabs(huoquangle):fabs(huoquangle)-Pi;
		rotate_point_around_axis(huoqu_mark[i].line_arr[4].endPt,fPlaneAngle,f3dLine(startPt,startPt+f3dPoint(0,0,1)*10));
		huoqu_mark[i].plane_angle=fPlaneAngle;
		if(!bPatternDrawing&&sys.part_map.plate.bDrawHuoQuPrjMap)
			AddHuoQuPrjMap(line,huoqu_mark[i],'A'+i);
	}

	//查找剪切面的起终点顶点号
	for(i=0;i<n;i++)
	{
		PROFILE_VERTEX *pCurVertex=&vertex_list[i];
		PROFILE_VERTEX *pNextVertex=&vertex_list[(i+1)%n];
		if((pCurVertex->vertex.feature==3&&pNextVertex->vertex.feature==2)||
			(pCurVertex->vertex.feature==2&&pNextVertex->vertex.feature==3))
		{
			start_i=i;
			end_i=(i+1)%n;
			break;
		}
	}
	if(start_i==0&&end_i==0)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "the data of three sides connection plates is wrong";
#else
		throw "三面的连接板数据错误";
#endif
	//添加剪切线
	line.startPt = top_point;
	line.endPt = vertex_list[start_i].vertex;
	line.pen.style = PS_SOLID;
	edge_arr.Add(LineToArc(line));
	line.endPt = vertex_list[end_i].vertex;
	edge_arr.Add(LineToArc(line)); 	
	//指定绘图坐标系时不生成补焊钢板 wht 10-09-03
	if(!m_bSpecifyUCS) 
	{	//生成三面板补焊钢板	wht 10-01-12
		f3dPoint vertex1,vertex2,top_vertex,vertex1_org,vertex2_org;	
		GetComplementFaceVertex(pPlank,top_vertex,vertex1,vertex2,vertex1_org,vertex2_org);
		//pPlank->GetComplementFaceVertex(vertex1,vertex2);
		if(bAdjustPlateUcs)
		{	//调整钢板坐标系时，应进行坐标转换 wht 14-04-15	
			coord_trans(vertex1,pPlank->ucs,TRUE);
			coord_trans(vertex1,new_ucs,FALSE);
			coord_trans(vertex2,pPlank->ucs,TRUE);
			coord_trans(vertex2,new_ucs,FALSE);
			if(pPlank->IsHuoQuLineCollinear3FacePlate())
			{
				coord_trans(top_vertex,pPlank->ucs,TRUE);
				coord_trans(top_vertex,new_ucs,FALSE);
			}
		}
		f3dPoint vertex1_vec,vertex2_vec,verfiy_vec,vertex1_norm,vertex2_norm;
		vertex1_vec=vertex1-top_vertex;
		normalize(vertex1_vec);
		vertex2_vec=vertex2-top_vertex;
		normalize(vertex2_vec);
		verfiy_vec=vertex1-vertex2;	
		normalize(verfiy_vec);	//用来纠正偏移方向 vertex2=>vertex1
		vertex1_norm.Set(-vertex1_vec.y,vertex1_vec.x);
		vertex2_norm.Set(vertex2_vec.y,-vertex2_vec.x);
		if(vertex1_norm*verfiy_vec>0)
			vertex1_norm*=-1.0;
		if(vertex2_norm*verfiy_vec<0)
			vertex2_norm*=-1.0;
		//沿两剪切线角平分线方向移动一端距离 
		f3dPoint mid_vec=0.5*(vertex1_norm+vertex2_norm);
		normalize(mid_vec);
		if(pPlank->IsHuoQuLineCollinear3FacePlate())
		{	//三点不在一个面内，需统一至一个平面再绘制
			UCS_STRU temp_ucs;
			temp_ucs.origin=top_vertex;
			temp_ucs.axis_x=(vertex1_vec+vertex2_vec)*0.5;
			temp_ucs.axis_y=vertex2_vec;
			temp_ucs.axis_z=temp_ucs.axis_x^temp_ucs.axis_y;
			temp_ucs.axis_y=temp_ucs.axis_z^temp_ucs.axis_x;
			normalize(temp_ucs.axis_x);
			normalize(temp_ucs.axis_y);
			normalize(temp_ucs.axis_z);
			double offset_dist=0.25*(DISTANCE(vertex1,top_vertex)+DISTANCE(vertex2,top_vertex));
			//按轮廓点修订X轴方向
			vertex1_vec=vertex1_org-top_point;
			vertex2_vec=vertex2_org-top_point;
			mid_vec=(vertex1_vec+vertex2_vec)*0.5;
			normalize(mid_vec);
			project_vector(mid_vec,temp_ucs.axis_z);
			if(mid_vec*temp_ucs.axis_x<0)
				temp_ucs.axis_x=-1.0*mid_vec;
			else
				temp_ucs.axis_x=mid_vec;
			temp_ucs.axis_y=temp_ucs.axis_z^temp_ucs.axis_x;
			normalize(temp_ucs.axis_x);
			normalize(temp_ucs.axis_y);
			//
			coord_trans(vertex1,temp_ucs,false);
			coord_trans(vertex2,temp_ucs,false);
			vertex1.z=vertex2.z=top_vertex.z=0;
			f3dPoint vertex_arr[3];
			vertex_arr[0]=top_vertex+temp_ucs.axis_x*offset_dist;
			vertex_arr[1]=vertex1+vertex_arr[0];
			vertex_arr[2]=vertex2+vertex_arr[0];
			for(i=0;i<3;i++)
			{	
				line.startPt = vertex_arr[i%3];
				line.endPt = vertex_arr[(i+1)%3];
				line.pen.style = PS_SOLID;
				edge_arr.Add(LineToArc(line));
			}
		}
		else
		{
			f3dPoint vertex_top_point=top_vertex;
			if(vertex_list[start_i].vertex==vertex_list[end_i].vertex)
				vertex_top_point=vertex_list[start_i].vertex; //处理top_point前后两点位置相同的特殊三面板 wht 10-09-03
			double offset_dist=0.25*(DISTANCE(vertex1,top_vertex)+DISTANCE(vertex2,top_vertex));
			f3dPoint vertex_arr[5];
			vertex_arr[0]=top_vertex+mid_vec*offset_dist;
			vertex_arr[1]=vertex1+mid_vec*offset_dist;
			vertex_arr[2]=vertex1+vertex1_norm*20+mid_vec*offset_dist;
			vertex_arr[3]=vertex2+vertex2_norm*20+mid_vec*offset_dist;
			vertex_arr[4]=vertex2+mid_vec*offset_dist;
			for(i=0;i<5;i++)
			{	
				line.startPt = vertex_arr[i%5];
				line.endPt = vertex_arr[(i+1)%5];
				line.pen.style = PS_SOLID;
				edge_arr.Add(LineToArc(line));
			}
		}
	}
	//生成钢板内圆环 wht 11-07-09
	if(pPlank->m_fInnerRadius>0)	
		CreatePlateInnerCir(pPlank,bPatternDrawing,bAdjustPlateUcs);
	//标注螺栓相对坐标值	wht 09-09-16
	UCS_STRU old_ucs;
	if(bAdjustPlateUcs)
	{	//设置板的坐标系为调整后的坐标系，获得正确的螺栓位置
		old_ucs=pPlank->ucs;
		//pPlank->ucs=new_ucs;	//不能在此处替换钢板坐标系 否则计算非基准面上螺栓位置错误
	}
	for(pLsRef=pPlank->GetFirstLsRef();pLsRef!=NULL;pLsRef=pPlank->GetNextLsRef())
	{	//调用GetNormFaceI时传入螺栓,会同时根据螺栓位置计算螺栓所在面号更准确 wht 17-02-28
		int iLsFace=pPlank->GetNormFaceI(pLsRef->GetLsPtr());
		pPlank->SpreadLsPos(pLsRef,ls_cir.centre);
		if(bAdjustPlateUcs)
		{	//该代码应在GetDeformedVertex()之前 wht 11-02-27
			coord_trans(ls_cir.centre,pPlank->ucs,TRUE);
			coord_trans(ls_cir.centre,new_ucs,FALSE);
		}
		if(sys.part_map.bExportDeformedProfile)
		{
			ls_cir.centre.feature=iLsFace+1;
			if(bAdjustPlateUcs)
				ls_cir.centre=pPlank->GetDeformedVertex(ls_cir.centre,new_ucs);
			else
				ls_cir.centre=pPlank->GetDeformedVertex(ls_cir.centre);
		}
		ls_cir.centre.z = 0;		//必须清零，不然螺栓坐标间距计算不准
		//标注坐标时也有可能要分析螺栓射线
		for(int i=1;i<=32;i++)
		{
			if(pLsRef->dwRayNo&GetSingleWord(i))
			{
				GEPOINT resideRayVec=pLsRef->rayVecArr[i-1];
				if(bAdjustPlateUcs)
				{
					vector_trans(resideRayVec,pPlank->ucs,TRUE);
					vector_trans(resideRayVec,new_ucs,FALSE);
				}
				AnalizeLsRay(ls_cir.centre,i,resideRayVec);
			}
		}
		ls_cir.radius = (*pLsRef)->get_d()/2.0;
		ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
		ls_cir.pen.style=(*pLsRef)->FilterFuncType();
		ls_cir.centre.pen.style=pLsRef->cFlag;			//记录螺栓孔的工艺类型
		if((int)pLsRef->waistLen>0)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width=(int)pLsRef->waistLen;	//使用画笔宽度保存腰圆孔长度
			ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
			if(bAdjustPlateUcs)
				vector_trans(ls_cir.norm,new_ucs,FALSE);
			else
				vector_trans(ls_cir.norm,pPlank->ucs,FALSE);//转换到钢板坐标系下
		}
		cir_arr.Add(ls_cir);
		//标注螺栓相对坐标值	wht 09-09-16
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
			DimCoordValue(pPlank,ls_cir.centre);
	}
	//绘制钢板定位点
	DrawPlateLocateFlag(pPlank,bAdjustPlateUcs);

	if(bAdjustPlateUcs)
		pPlank->ucs=old_ucs;	//恢复板坐标系
	//螺栓射线处理
	if(sys.part_map.plate.bDimPlateRayLs)
	{
		//DimLsEndSpace(pPlank);
		DimLsRay(pPlank,bAdjustPlateUcs);
	}
	//分析杆件射线
	if(sys.part_map.plate.bDimCoord)
		AnalizePoleRay(pPlank);
	return TRUE;
}
BOOL CPlankDrawing::CreateFoldPlank(CLDSPlate *pPlank,BOOL bPatternDrawing/*=TRUE*/)
{
	int i,n;
	f3dLine line;
	f3dArcLine arcline;
	f3dCircle ls_cir;
	f3dPoint top_pnt,vec;
	f3dPoint basePt,base_face_pick,qu_face_pick[2],dimPos,start_vec,end_vec,dim_vec,norm;
	CLsRef *pLsRef;
	ANGULARDIM_STRU angular_dim;

	double left=0,top=0,right=0,bottom=0;
	f3dPoint left_pt,top_pt,right_pt,bottom_pt;
	line.feature = 0;
	n = pPlank->vertex_list.GetNodeNum();
	if(n<3)
		return FALSE;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		vertex_list.append(*pVertex);
	f3dLine HuoQuLine[2];
	HuoQuLine[0]=pPlank->HuoQuLine[0];	//HuoQuLine[0].Layer中保存了火曲半径，以及中心层系数
	HuoQuLine[1]=pPlank->HuoQuLine[1];	//HuoQuLine[1].Layer中保存了火曲半径，以及中心层系数
	/*HuoQuLine[0].startPt=pPlank->HuoQuLine[0].startPt;
	HuoQuLine[0].endPt=pPlank->HuoQuLine[0].endPt;
	HuoQuLine[1].startPt=pPlank->HuoQuLine[1].startPt;
	HuoQuLine[1].endPt=pPlank->HuoQuLine[1].endPt;*/
	BOOL bAdjustPlateUcs=FALSE;
	if(sys.part_map.plate.bAdjustPlateUcs||(sys.part_map.plate.bDimCoord&&!bPatternDrawing))
		bAdjustPlateUcs=TRUE;
	if(m_bSpecifyUCS) //指定绘图坐标系优先于内部调整坐标系 wht 10-08-13
	{
		bAdjustPlateUcs=TRUE;
		PROFILE_VERTEX *pVertex=NULL;
		for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			coord_trans(pVertex->vertex,pPlank->ucs,TRUE);
			coord_trans(pVertex->vertex,new_ucs,FALSE);
			if(pVertex->type==1||pVertex->type==2)
			{	//转换圆心拾取点
				coord_trans(pVertex->center,pPlank->ucs,TRUE);
				coord_trans(pVertex->center,new_ucs,FALSE);
			}
		}
		coord_trans(HuoQuLine[0].startPt,pPlank->ucs,TRUE);
		coord_trans(HuoQuLine[0].startPt,new_ucs,FALSE);
		coord_trans(HuoQuLine[0].endPt,pPlank->ucs,TRUE);
		coord_trans(HuoQuLine[0].endPt,new_ucs,FALSE);
		coord_trans(HuoQuLine[1].startPt,pPlank->ucs,TRUE);
		coord_trans(HuoQuLine[1].startPt,new_ucs,FALSE);
		coord_trans(HuoQuLine[1].endPt,pPlank->ucs,TRUE);
		coord_trans(HuoQuLine[1].endPt,new_ucs,FALSE);
		//计算几何中心
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pPlank->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=pPlank->vertex_list.GetNodeNum();
		project_point(geom_center,pPlank->ucs.origin,pPlank->ucs.axis_z);
	}
	else if(bAdjustPlateUcs)	//标注坐标
	{
		UCS_STRU ucs;
		if(AdjustPlateUcs(pPlank,ucs,geom_center)>EPS2)
		{
			PROFILE_VERTEX *pVertex=NULL;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				coord_trans(pVertex->vertex,pPlank->ucs,TRUE);
				coord_trans(pVertex->vertex,ucs,FALSE);
				if(pVertex->type==1||pVertex->type==2)
				{	//转换圆心拾取点
					coord_trans(pVertex->center,pPlank->ucs,TRUE);
					coord_trans(pVertex->center,ucs,FALSE);
				}
			}
			coord_trans(HuoQuLine[0].startPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[0].startPt,ucs,FALSE);
			coord_trans(HuoQuLine[0].endPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[0].endPt,ucs,FALSE);
			coord_trans(HuoQuLine[1].startPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[1].startPt,ucs,FALSE);
			coord_trans(HuoQuLine[1].endPt,pPlank->ucs,TRUE);
			coord_trans(HuoQuLine[1].endPt,ucs,FALSE);
			new_ucs=ucs;	//记录调整后的坐标系
		}
		else
			new_ucs=pPlank->ucs;
	}
	else
	{
		new_ucs=pPlank->ucs;
		for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
		{
			f3dPoint vertex=pVertex->vertex;
			coord_trans(vertex,pPlank->ucs,TRUE);
			geom_center+=vertex;
		}
		geom_center/=pPlank->vertex_list.GetNodeNum();
		project_point(geom_center,pPlank->ucs.origin,pPlank->ucs.axis_z);
	}
	//生成连接板的外包轮廓线
	base_face_pick.Set();
	double max_edge_len=0;
	int base_face_vertex_n=0;
	ARRAY_LIST<f3dPoint> profileVertexList;
	int indexOfFirstEdgeLine=-1;
	m_bPrevIsRollEdgeLine=false;
	for(i=0;i<n;i++)
	{
		f3dPoint pt = vertex_list[i].vertex;
		if(pt.feature/10==1||pt.feature%10==1)
		{
			base_face_pick+=pt;
			base_face_vertex_n++;
		}
		else if(HuoQuLine[0].PtInLine(pt)==0&&HuoQuLine[1].PtInLine(pt)==0) 
		{	//保证火曲面的拾取点不在火曲线上，否则无法正确判断内外曲 wht 11-04-21
			if(pt.feature==2)
				qu_face_pick[0]=pt;
			else
				qu_face_pick[1]=pt;
		}
		if(pt.x<left)
		{
			left_pt = pt;
			left=pt.x;
		}
		if(pt.x>right)
		{
			right_pt = pt;
			right = pt.x;
		}
		if(pt.y>top)
		{
			top_pt=pt;
			top = pt.y;
		}
		if(pt.y<bottom)
		{
			bottom_pt = pt;
			bottom = pt.y;
		}

		PROFILE_VERTEX startVertex=vertex_list[i];
		PROFILE_VERTEX endVertex=vertex_list[(i+1)%n];
		if(startVertex.vertex.feature==2&&endVertex.vertex.feature==3
			||startVertex.vertex.feature==3&&endVertex.vertex.feature==2)
			continue;
		if(sys.part_map.bExportDeformedProfile)
		{	//考虑火曲变形
			if(bAdjustPlateUcs)
			{
				startVertex.vertex = pPlank->GetDeformedVertex(startVertex.vertex,new_ucs);
				endVertex.vertex=pPlank->GetDeformedVertex(endVertex.vertex,new_ucs);
			}
			else
			{
				startVertex.vertex = pPlank->GetDeformedVertex(startVertex.vertex);
				endVertex.vertex=pPlank->GetDeformedVertex(endVertex.vertex);
			}
		}
		TXTDIM_STRU txt_dim;
		if(startVertex.m_bWeldEdge&&startVertex.manu_space!=0)
		{	//输出加工间隙线
			f3dLine manu_cut_line(startVertex.vertex,endVertex.vertex);	//考虑了加工间隙的实际加工切割线
			GEPOINT offset_vec=manu_cut_line.endPt-manu_cut_line.startPt;
			if(startVertex.type==0)
			{
				normalize(offset_vec);
				offset_vec.Set(-offset_vec.y,offset_vec.x);	//逆时针旋转90°确定垂直与板边向里的间隙偏移方向
				manu_cut_line.startPt+=offset_vec*startVertex.manu_space;
				manu_cut_line.endPt+=offset_vec*startVertex.manu_space;
				line.startPt = manu_cut_line.startPt;
				line.endPt   = manu_cut_line.endPt;
				line.startPt.z = 0;
				line.endPt.z = 0;
				arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
			}
			else
			{
				if(startVertex.type==1)
					arcline.CreateMethod2(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,1),startVertex.sector_angle);
				else if(startVertex.type==2)
				{	
					f3dPoint center_pick=startVertex.center;
					/*if(bAdjustPlateUcs)
					{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
						coord_trans(center_pick,pPlank->ucs,TRUE);
						coord_trans(center_pick,new_ucs,FALSE);
						center_pick.z=0;
					}*/
					int flag=1;
					if(startVertex.radius<0)
						flag=-1;
					startVertex.vertex.z=endVertex.vertex.z=0;	//Z坐标归零
					arcline.CreateMethod3(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,flag),flag*startVertex.radius,center_pick);
				}
				//将圆弧向里偏移出一个同心加工间隙圆
				int sign=1;
				if(arcline.WorkNorm().z<0)
					sign=-1;
				GEPOINT center=arcline.Center();
				offset_vec=arcline.Start()-center;
				normalize(offset_vec);
				manu_cut_line.startPt=arcline.Start()-offset_vec*(sign*startVertex.manu_space);
				offset_vec=arcline.End()-center;
				normalize(offset_vec);
				manu_cut_line.endPt=arcline.End()-offset_vec*(sign*startVertex.manu_space);
				if(startVertex.type==1)
					arcline.CreateMethod2(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,1),startVertex.sector_angle);
				else if(startVertex.type==2)
				{	
					f3dPoint center_pick=startVertex.center;
					/*if(bAdjustPlateUcs)
					{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
						coord_trans(center_pick,pPlank->ucs,TRUE);
						coord_trans(center_pick,new_ucs,FALSE);
						center_pick.z=0;
					}*/
					int flag=1;
					if(startVertex.radius<0)
						flag=-1;
					startVertex.vertex.z=endVertex.vertex.z=0;	//Z坐标归零
					arcline.CreateMethod3(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,flag),flag*startVertex.radius,center_pick);
				}
			}
			arcline.pen.style=PS_DASH;
			edge_arr.Add(arcline);
			arcline.pen.style=PS_SOLID;
		}
		if(startVertex.type==0&&startVertex.m_bRollEdge&&startVertex.manu_space>0)
		{	//输出卷边
			f3dLine roll_edge_line,huoqu_line;
			PROFILE_VERTEX prevVertex= vertex_list[(i+n-1)%n];
			PROFILE_VERTEX nextVertex= vertex_list[(i+2)%n];
			if(sys.part_map.bExportDeformedProfile)
			{
				if(bAdjustPlateUcs)
				{
					prevVertex.vertex=pPlank->GetDeformedVertex(prevVertex.vertex,new_ucs);
					nextVertex.vertex=pPlank->GetDeformedVertex(nextVertex.vertex,new_ucs);
				}
				else
				{
					prevVertex.vertex=pPlank->GetDeformedVertex(prevVertex.vertex);
					nextVertex.vertex=pPlank->GetDeformedVertex(nextVertex.vertex);
				}
			}
			//根据卷边高度以及卷边外移量绘制卷边相关线段 wht 11-04-09
			DrawRollEdgeLine(&startVertex,&endVertex,&prevVertex,&nextVertex,pPlank,roll_edge_line,huoqu_line,bPatternDrawing);
			arcline.CreateMethod2(roll_edge_line.startPt,roll_edge_line.endPt,f3dPoint(0,0,1),0);
			if(i==n-1&&indexOfFirstEdgeLine>=0)
			{
				edge_arr[indexOfFirstEdgeLine].SetStart(m_xRollEdgeLineEnd);
				m_bPrevIsRollEdgeLine=false;
			}
			//卷边处理后会多出3条轮廓边
			profileVertexList.append(huoqu_line.startPt);
			profileVertexList.append(roll_edge_line.startPt);
			profileVertexList.append(roll_edge_line.endPt);
			//标注顶点相对坐标值	wht 09-09-16
			if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
				DimCoordValue(pPlank,huoqu_line.startPt);
		}
		else
		{
			if(startVertex.type==0)
			{
				line.startPt = startVertex.vertex;
				line.endPt   = endVertex.vertex;
				line.startPt.z = 0;
				line.endPt.z = 0;
				arcline.CreateMethod2(line.startPt,line.endPt,f3dPoint(0,0,1),0);
			}
			else if(startVertex.type==1)
				arcline.CreateMethod2(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,1),startVertex.sector_angle);
			else if(startVertex.type==2)
			{	
				f3dPoint center_pick=startVertex.center;
				/*if(bAdjustPlateUcs)
				{	//普通圆弧圆心拾取点为相对坐标，此处需要先转为绝对坐标再转换为new_ucs下的相对坐标 wht 12-06-14
					coord_trans(center_pick,pPlank->ucs,TRUE);
					coord_trans(center_pick,new_ucs,FALSE);
					center_pick.z=0;
				}*/
				int flag=1;
				if(startVertex.radius<0)
					flag=-1;
				startVertex.vertex.z=endVertex.vertex.z=0;	//Z坐标归零
				arcline.CreateMethod3(startVertex.vertex,endVertex.vertex,f3dPoint(0,0,flag),flag*startVertex.radius,center_pick);
			}
			else if(startVertex.type==3)
			{
				int sign=1;
				f3dPoint center_pick=startVertex.center;
				f3dPoint column_norm=startVertex.column_norm;
				if(bAdjustPlateUcs)
				{
					coord_trans(center_pick,new_ucs,FALSE);
					vector_trans(column_norm,new_ucs,FALSE);
				}
				else 
				{
					coord_trans(center_pick,pPlank->ucs,FALSE);
					vector_trans(column_norm,pPlank->ucs,FALSE);
				}
				if(startVertex.radius<0)
					sign=-1;
				startVertex.vertex.z=endVertex.vertex.z=0;	//起始点终止点Z坐标归零 wht 11-06-23
				arcline.CreateEllipse(center_pick,startVertex.vertex,endVertex.vertex,column_norm,f3dPoint(0,0,sign),sign*startVertex.radius);
				//重新绘制钢板的椭圆弧边
				DrawPlateEllipseLine(pPlank,arcline);
			}
			int iNewInsert=edge_arr.Add(arcline);
			if(indexOfFirstEdgeLine<0)
				indexOfFirstEdgeLine=iNewInsert;
			//记录钢板外形轮廓点 wht 15-08-12
			profileVertexList.append(arcline.Start());
			//标注顶点相对坐标值	wht 09-09-16
			if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
				DimCoordValue(pPlank,arcline.Start());
		}
		if(startVertex.m_bWeldEdge)
			ProcessPlateWeldEdge(&startVertex,arcline);	//处理焊缝边(绘制焊缝线、绘制坡口线、标注焊接件号、标注坡口尺寸等) wht 18-08-09
		//Sub_Pnt(start_vec,line.endPt,line.startPt);
		if(max_edge_len<DISTANCE(arcline.Start(),arcline.End()))
		{
			max_edge_len=DISTANCE(arcline.Start(),arcline.End());
			long_edge_mid=arcline.PositionInAngle(arcline.SectorAngle()*0.5);
			long_edge_angle=Cal2dLineAng(arcline.Start().x,arcline.Start().y,arcline.End().x,arcline.End().y);
		}
	}
	base_face_pick*=1.0/base_face_vertex_n;
	//初始化钢板外形轮廓区域 wht 15-08-12
	if(profileVertexList.GetSize()>0)
		profileRegion.CreatePolygonRgn(profileVertexList.m_pData,profileVertexList.GetSize());
	//处理火曲线
	PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL,*pBendLine2Start=NULL,*pBendLine2End=NULL;
	pPlank->GetBendlineVertexies(&pBendLine1Start,&pBendLine1End,&pBendLine2Start,&pBendLine2End);
	for(i=0;i<pPlank->face_N-1;i++)
	{
		//添加火曲线
		f3dLine huoquline=pPlank->HuoQuLine[i];
		if(i==0&&pBendLine1Start&&pBendLine1End)
		{
			huoquline.startPt=pBendLine1Start->vertex;
			huoquline.endPt  =pBendLine1End->vertex; 
		}
		if(i==1&&pBendLine2Start&&pBendLine2End)
		{
			huoquline.startPt=pBendLine2Start->vertex;
			huoquline.endPt  =pBendLine2End->vertex;
		}
		line=huoquline;
		if(sys.part_map.bExportDeformedProfile)
		{
			line.startPt=pPlank->GetDeformedVertex(line.startPt);
			line.endPt=pPlank->GetDeformedVertex(line.endPt);
		}
		if(bAdjustPlateUcs)
		{	//标注相对坐标 wht 09-09-17 该代码应在GetDeformedVertex()之前 wht 11-02-27
			coord_trans(line.startPt,pPlank->ucs,TRUE);
			coord_trans(line.startPt,new_ucs,FALSE);
			coord_trans(line.endPt,pPlank->ucs,TRUE);
			coord_trans(line.endPt,new_ucs,FALSE);
		}
		line.pen.style=PS_DASHDOTDOT;
		edge_arr.Add(LineToArc(line));
		if(sys.part_map.plate.bMarkHuoquPos)
			MarkHuoquLine(line);
		//判断火曲方式(内曲、外曲)
		f3dPoint base_pick=base_face_pick;
		if(bAdjustPlateUcs)
		{	
			coord_trans(base_pick,new_ucs,TRUE);
			coord_trans(base_pick,pPlank->ucs,FALSE);
		}
		f3dPoint prj_base_pick,base_vec;
		SnapPerp(&prj_base_pick,huoquline,base_pick);
		base_vec=base_pick-prj_base_pick;
		normalize(base_vec);
		norm = pPlank->huoqufacenorm[i];
		vector_trans(norm,pPlank->ucs,FALSE);
		BOOL bNeiQu=TRUE;
		double dd = cal_angle_of_2vec(base_vec,norm);
		if(dd<Pi/2||(fabs(dd-Pi/2)<EPS&&norm.z>0))	//火曲面向外曲(正曲)
			bNeiQu=FALSE;
		if(bAdjustPlateUcs)
		{	//标注相对坐标 wht 09-09-17 该代码应在GetDeformedVertex()之前 wht 11-02-27
			vector_trans(base_vec,pPlank->ucs,TRUE);
			vector_trans(base_vec,new_ucs,FALSE);
		}
		//添加火曲线工艺内容标注(在火曲线的右侧)
		TXTDIM_STRU txt_dim;
		txt_dim.txtHeight=sys.fonts.plate.fDimTextSize;	//火曲工艺内容字体高度独立设置
		txt_dim.dimPos=0.5*(line.startPt+line.endPt);
		txt_dim.dimVec.Set(-base_vec.y,base_vec.x,0);
		double huoquangle=pPlank->GetHuoQuAngle(i+1);
		GetPlateHuoQuProcessStr(txt_dim,bNeiQu,huoquangle*DEGTORAD_COEF,pPlank->m_fDeformCoef,pPlank->GetThick());
		double fLen=TestDrawTextLength(txt_dim.dimText,txt_dim.txtHeight,TextStyleTable::hzfs.textStyleId);
		normalize(txt_dim.dimVec);
		txt_dim.dimPos-=0.5*fLen*txt_dim.dimVec;
		text_dim_arr.Add(txt_dim);
		//添加火曲线方向的投影面（在火曲线的左侧）
		huoqu_mark[i].prj_vec=line.endPt-line.startPt;
		normalize(huoqu_mark[i].prj_vec);
		huoqu_mark[i].origin=0.5*(line.startPt+line.endPt);;
		huoqu_mark[i].offset_dist=-10;
		huoqu_mark[i].offset_vec=base_vec;
		huoqu_mark[i].line_arr[0].startPt=base_vec;
		huoqu_mark[i].line_arr[0].endPt=base_vec+huoqu_mark[i].prj_vec*15.0;
		huoqu_mark[i].line_arr[1].startPt=huoqu_mark[i].line_arr[0].endPt;
		huoqu_mark[i].line_arr[1].endPt=huoqu_mark[i].line_arr[1].startPt+base_vec*2.0-huoqu_mark[i].prj_vec*2.0;
		huoqu_mark[i].line_arr[2].startPt=huoqu_mark[i].line_arr[1].endPt;
		huoqu_mark[i].line_arr[2].endPt=huoqu_mark[i].line_arr[2].startPt-base_vec*2.0-huoqu_mark[i].prj_vec*2.0;
		//剖面图
		f3dPoint startPt=huoqu_mark[i].line_arr[0].endPt+huoqu_mark[i].prj_vec*5.0-base_vec;
		huoqu_mark[i].line_arr[3].startPt=startPt;
		huoqu_mark[i].line_arr[3].endPt=startPt+base_vec*15;
		huoqu_mark[i].line_arr[4].startPt=startPt;
		huoqu_mark[i].line_arr[4].endPt=startPt+base_vec*10;
		double fPlaneAngle=bNeiQu?Pi-fabs(huoquangle):fabs(huoquangle)-Pi;
		rotate_point_around_axis(huoqu_mark[i].line_arr[4].endPt,fPlaneAngle,f3dLine(startPt,startPt+f3dPoint(0,0,1)*10));
		huoqu_mark[i].plane_angle=fPlaneAngle;
		if(!bPatternDrawing&&sys.part_map.plate.bDrawHuoQuPrjMap)
			AddHuoQuPrjMap(line,huoqu_mark[i],'A'+i);
	}
	
	//生成钢板内圆环 wht 11-07-09
	if(pPlank->m_fInnerRadius>0)	
		CreatePlateInnerCir(pPlank,bPatternDrawing,bAdjustPlateUcs);
	//标注螺栓相对坐标值	wht 09-09-16
	UCS_STRU old_ucs;
	if(bAdjustPlateUcs)
	{	//设置板的坐标系为调整后的坐标系，获得正确的螺栓位置
		old_ucs=pPlank->ucs;
		//pPlank->ucs=new_ucs;	//不能在此处替换钢板坐标系 否则计算非基准面上螺栓位置错误
	}
	for(pLsRef=pPlank->GetFirstLsRef();pLsRef!=NULL;pLsRef=pPlank->GetNextLsRef())
	{
		int iLsFace=pPlank->GetNormFaceI((*pLsRef)->get_norm());
		pPlank->SpreadLsPos(pLsRef,ls_cir.centre);
		if(bAdjustPlateUcs)
		{	//调整坐标系 该代码应在GetDeformedVertex()之前 wht 11-02-27
			coord_trans(ls_cir.centre,pPlank->ucs,TRUE);
			coord_trans(ls_cir.centre,new_ucs,FALSE);
		}
		if(sys.part_map.bExportDeformedProfile)
		{
			ls_cir.centre.feature=iLsFace+1;
			if(bAdjustPlateUcs)
				ls_cir.centre=pPlank->GetDeformedVertex(ls_cir.centre,new_ucs);
			else
				ls_cir.centre=pPlank->GetDeformedVertex(ls_cir.centre);
		}
		ls_cir.centre.z = 0;		//必须清零，不然螺栓坐标间距计算不准
		//标注坐标时也有可能要分析螺栓射线
		for(int i=1;i<=32;i++)
		{
			if(pLsRef->dwRayNo&GetSingleWord(i))
			{
				GEPOINT resideRayVec=pLsRef->rayVecArr[i-1];
				if(bAdjustPlateUcs)
				{
					vector_trans(resideRayVec,pPlank->ucs,TRUE);
					vector_trans(resideRayVec,new_ucs,FALSE);
				}
				AnalizeLsRay(ls_cir.centre,i,resideRayVec);
			}
		}
		ls_cir.radius = (*pLsRef)->get_d()/2.0;
		ls_cir.feature = (int)((*pLsRef)->hole_d_increment*10);
		ls_cir.pen.style=(*pLsRef)->FilterFuncType();
		ls_cir.centre.pen.style=pLsRef->cFlag;			//记录螺栓孔的工艺类型
		if((int)pLsRef->waistLen>0)
		{	//腰圆孔	wht 10-02-25
			ls_cir.pen.width=(int)pLsRef->waistLen;	//使用画笔宽度保存腰圆孔长度
			ls_cir.norm=(*pLsRef)->ucs.axis_x;			//使用螺栓孔法线方向保存腰圆方向
			vector_trans(ls_cir.norm,pPlank->ucs,FALSE);//转换到钢板坐标系下
		}
		cir_arr.Add(ls_cir);
		//标注顶点相对坐标值	wht 09-09-16
		if(!bPatternDrawing)	//1:1大样图不需标注坐标值 wjh-2015.2.3
			DimCoordValue(pPlank,ls_cir.centre);
	}
	//绘制钢板定位点
	DrawPlateLocateFlag(pPlank,bAdjustPlateUcs);

	if(bAdjustPlateUcs)
		pPlank->ucs=old_ucs;	//恢复板坐标系
	//螺栓射线处理
	if(sys.part_map.plate.bDimPlateRayLs)
	{
		//DimLsEndSpace(pPlank);
		DimLsRay(pPlank,bAdjustPlateUcs);
	}
	//分析杆件射线
	if(sys.part_map.plate.bDimCoord)
		AnalizePoleRay(pPlank);
	return TRUE;
}


BOOL CPlankDrawing::Create(CLDSPart *pPlankTypePart,BOOL bPatternDrawing/*=TRUE*/)
{
	CSuperSmartPtr<CLDSPart>pPlate=pPlankTypePart;
	m_pPlate=pPlankTypePart;	//保存关联钢板指针 wht 10-08-23
	if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)	//参数化钢板
		return CreateParamPlate(pPlate.ParamPlatePointer(),bPatternDrawing);
	else if(pPlate->GetClassTypeId()==CLS_PLATE)
	{	//普通钢板
		if(pPlate.PlatePointer()->IsFoldPlate())	//折叠板
			return CreateFoldPlank(pPlate.PlatePointer(),bPatternDrawing);
		else if(pPlate.PlatePointer()->face_N==1)
			return Create1FacePlank(pPlate.PlatePointer(),bPatternDrawing);
		else if(pPlate.PlatePointer()->face_N==2)
			return Create2FacePlank(pPlate.PlatePointer(),bPatternDrawing);
		else if(pPlate.PlatePointer()->face_N==3)
			return Create3FacePlank(pPlate.PlatePointer(),bPatternDrawing);
	}
	return FALSE;
}
int CPlankDrawing::GetAngularDimCount()
{
	return angular_dim_arr.GetSize();
}
int CPlankDrawing::GetTextDimCount()
{
	return text_dim_arr.GetSize();
}
int CPlankDrawing::GetLsCirCount()
{
	return cir_arr.GetSize();
}
int CPlankDrawing::GetEdgeCount()
{
	return edge_arr.GetSize();
}
int CPlankDrawing::GetDiameterDimCount()
{
	return diameter_dim_arr.GetSize();
}
BOOL CPlankDrawing::GetDiameterDimAt(int index, DIAMETERDIM_STRU&dim)
{
	if(index<0||index>diameter_dim_arr.GetUpperBound())
		return FALSE;
	dim = diameter_dim_arr[index];
	return TRUE;
}

BOOL CPlankDrawing::GetAngularDimAt(int index, ANGULARDIM_STRU &dim)
{
	if(index<0||index>angular_dim_arr.GetUpperBound())
		return FALSE;
	dim = angular_dim_arr[index];
	return TRUE;
}
BOOL CPlankDrawing::GetTextDimAt(int index, TXTDIM_STRU &dim)
{
	if(index<0||index>text_dim_arr.GetUpperBound())
		return FALSE;
	dim = text_dim_arr[index];
	return TRUE;
}
BOOL CPlankDrawing::GetEdgeAt(int index, f3dArcLine &line)
{
	if(index<0||index>edge_arr.GetUpperBound())
		return FALSE;
	line = edge_arr[index];
	return TRUE;
}
BOOL CPlankDrawing::GetLsCirAt(int index, f3dCircle &Cir)
{
	if(index<0||index>cir_arr.GetUpperBound())
		return FALSE;
	Cir = cir_arr[index];
	return TRUE;
}
int CPlankDrawing::GetRotateDimCount()
{
	return rotate_dim_arr.GetSize();
}
BOOL CPlankDrawing::GetRotateDimAt(int index, ROTATEDIM_STRU &dim)
{
	if(index<0||index>rotate_dim_arr.GetUpperBound())
		return FALSE;
	dim = rotate_dim_arr[index];
	return TRUE;
}
int CPlankDrawing::GetLsRayCount()
{
	return ls_ray_arr.GetSize();
}
BOOL CPlankDrawing::GetLsRayLineAt(int index, f3dLine &line,DWORD* pwDrawFlag/*=NULL*/)
{
	int upper_i = ls_ray_arr.GetUpperBound();
	if(index<0||index>upper_i)
		return FALSE;
	upper_i = ls_ray_arr[index].ls_arr.GetNodeNum()-1;
	if(ls_ray_arr.GetSize()<2 || upper_i<1)
		return FALSE;
	bool bExtendStart=true,bExtendEnd=true;
	line.startPt = ls_ray_arr[index].ls_arr[0];
	line.endPt   = ls_ray_arr[index].ls_arr[upper_i];
	if(line.startPt.feature<0)
		bExtendStart=false;
	if(line.endPt.feature<0)
		bExtendEnd=false;
	line.pen.style=PS_DASHDOT;	//图层4
	if(ls_ray_arr[index].iRayNo<0)
		line.pen.style=PS_SOLID;//图层2
	if(!sys.part_map.plate.bDimCoord&&!m_bSpecifyUCS)
	{
		f2dLine line1(f2dPoint(datumLine.startPt.x,datumLine.startPt.y),f2dPoint(datumLine.endPt.x,datumLine.endPt.y));
		f2dLine line2(f2dPoint(line.startPt.x,line.startPt.y),f2dPoint(line.endPt.x,line.endPt.y));
		f3dPoint inters,line_vec=line.endPt-line.startPt;
		int ret=Int2dll(line1,line2,inters.x,inters.y);
		//修改为原来代码否则不能标注螺栓射线角 wht 11-01-06
		//射线杆件为钢管时 分析得到的螺栓射线有问题 (暂不处理 射线与基准线交点位于基准线外侧的情况) wht 10-06-21
		if(ret==-2||ret>0) 
		{	//平行时不处理
			f3dPoint verify_vec1=inters-line.startPt;
			f3dPoint verify_vec2=inters-line.endPt;
			if(verify_vec1*line_vec>0&&verify_vec2*line_vec>0)
				line.endPt=inters;
			else if(verify_vec1*line_vec<0&&verify_vec2*line_vec<0)
				line.startPt=inters;
		}
	}
	if(pwDrawFlag)
	{
		*pwDrawFlag=0;
		if(bExtendStart)
			*pwDrawFlag=0x01;
		if(bExtendEnd)
			*pwDrawFlag|=0x02;
	}
	return TRUE;
}
int CPlankDrawing::GetAssistLsRayCount()
{
	return assist_ls_ray_arr.GetSize();
}
BOOL CPlankDrawing::GetAssLsRayLineAt(int index, f3dLine &line)
{
	if(index<0||index>assist_ls_ray_arr.GetUpperBound())
		return FALSE;
	line=assist_ls_ray_arr[index];
	return TRUE;
}

void CPlankDrawing::AnalizeLsRay(f3dPoint ls_centre, int iRayNo,const double* resideRayVec)
{
	LSRAY_STRU ls_ray;
	f3dLine line;
	int i,nDimCount = ls_ray_arr.GetSize();
	for(i=0;i<nDimCount;i++)
	{
		if(ls_ray_arr[i].iRayNo==iRayNo)
		{
			int n = ls_ray_arr[i].ls_arr.GetNodeNum();
			if(n<2)	//现只有一个螺栓
				ls_ray_arr[i].ls_arr.append(ls_centre);
			else
			{
				line.startPt = ls_ray_arr[i].ls_arr[0];
				int j;
				for(j=1;j<n;j++)
				{
					line.endPt = ls_ray_arr[i].ls_arr[j];
					//TODO:找出几个例子来测试一下，再确定容差改为0.5是否合适
					int iRetCode = line.PtInLine(ls_centre,0.5);
					if(iRetCode==-2)		//起点外侧
					{
						ls_ray_arr[i].ls_arr.insert(ls_centre,j-1);
						break;
					}
					else if(iRetCode==2)	//位于两螺栓之间
					{
						ls_ray_arr[i].ls_arr.insert(ls_centre,j);
						break;
					}
					line.startPt = line.endPt;
				}
				if(j==n &&f3dLine(ls_ray_arr[i].ls_arr[0],ls_ray_arr[i].ls_arr[n-1]).PtInLine(ls_centre,EPS2)!=0)
					ls_ray_arr[i].ls_arr.append(ls_centre);
			}
			break;
		}
	}
	if(i==nDimCount)	//暂时还没有此射线上的螺栓，添加当前射线
	{
		ls_ray.iRayNo = iRayNo;
		if(resideRayVec!=NULL)
			ls_ray.rayResideVec=GEPOINT(resideRayVec);
		ls_ray_arr.Add(ls_ray);
		i = ls_ray_arr.GetUpperBound();
		ls_ray_arr[i].ls_arr.append(ls_centre);
	}
}
//判断坐标点是否在钢板内
bool CPlankDrawing::PtInPlateRgn(const double* poscoord)
{	
	if(profileRegion.GetAxisZ().IsZero())
		return true;
	int nRetCode=profileRegion.PtInRgn(poscoord);
	return nRetCode==1;
}
//获取调整板坐标系后，二位平面的几何中心
f3dPoint CPlankDrawing::Get2dGeomCenter()
{
	f3dPoint geom_center;
	int nNum=profileRegion.GetVertexCount();
	for(int i=0;i<nNum;i++)
	{
		GEPOINT pt=profileRegion.GetVertexAt(i);
		geom_center+=pt;
	}
	if(nNum>0)
		geom_center/=nNum;
	return geom_center;
}

void CPlankDrawing::CorrectLsRays()
{
	//删除无效的螺栓射线
	LSRAY_STRU* pLsRay=NULL;
	ATOM_LIST<LSRAY_STRU> xLsRayList;
	for(int i=0;i<ls_ray_arr.GetSize();i++)
	{
		pLsRay=xLsRayList.append();
		pLsRay->CloneMember(&ls_ray_arr[i]);
	}
	int nNodeNum=xLsRayList.GetNodeNum();
	for(int i=0;i<nNodeNum;i++)
	{
		pLsRay=&xLsRayList[i];
		if(pLsRay->ls_arr.GetNodeNum()==0)
		{
			pLsRay->featrue=1;
			continue;
		}
		f3dPoint ls_pos=pLsRay->ls_arr[0];
		if(pLsRay->ls_arr.GetNodeNum()>1)
		{	//判断多个螺栓的是否有效
			for(int j=1;j<pLsRay->ls_arr.GetNodeNum();j++)
			{
				if(ls_pos.IsEqual(pLsRay->ls_arr[j],EPS2))
				{
					pLsRay->featrue=1;
					break;
				}
			}
		}
		else
		{	//判断单个螺栓的是否有效
			int nPush=xLsRayList.push_stack();
			for(int j=0;j<nNodeNum;j++)
			{
				if(i==j || xLsRayList[j].featrue==1)
					continue;
				f3dPoint* pPt=NULL;
				for(pPt=xLsRayList[j].ls_arr.GetFirst();pPt;pPt=xLsRayList[j].ls_arr.GetNext())
				{
					if(ls_pos.IsEqual(*pPt,EPS2))
						break;
				}
				if(pPt)
				{
					pLsRay->featrue=1;
					break;
				}
			}
			xLsRayList.pop_stack(nPush);
		}
		//判断有两个点的螺栓射线是否有效(多排螺栓临时添加的辅助射线不做处理)
		if (pLsRay->ls_arr.GetNodeNum() == 2 && pLsRay->featrue != 1 && pLsRay->iRayNo != 0)
		{
			GEPOINT ray_vec = (pLsRay->ls_arr[1]- pLsRay->ls_arr[0]).normalized();
			if (fabs(ray_vec*pLsRay->rayResideVec) < 0.999)
				pLsRay->featrue = 1;
		}
	}
	for(pLsRay=xLsRayList.GetFirst();pLsRay;pLsRay=xLsRayList.GetNext())
	{
		if(pLsRay->featrue==1)
			xLsRayList.DeleteCursor();
	}
	xLsRayList.Clean();
	//重新填充螺栓射线信息
	ls_ray_arr.RemoveAll();
	for(pLsRay=xLsRayList.GetFirst();pLsRay;pLsRay=xLsRayList.GetNext())
	{
		BOOL bCopyLsArr=FALSE;
		if(pLsRay->ls_arr.GetNodeNum()<=1)
			bCopyLsArr=TRUE;
		else
		{
			f3dPoint vec=pLsRay->ls_arr[1]-pLsRay->ls_arr[0];
			normalize(vec);
			if(vec*pLsRay->rayResideVec>0)
				bCopyLsArr=TRUE;
		}
		LSRAY_STRU lsRay;
		ls_ray_arr.Add(lsRay);
		int i=ls_ray_arr.GetUpperBound();
		ls_ray_arr[i].CloneMember(pLsRay,bCopyLsArr);
		if(bCopyLsArr==FALSE)
		{	//需反转螺栓坐标列表
			for(f3dPoint* pPt=pLsRay->ls_arr.GetTail();pPt;pPt=pLsRay->ls_arr.GetPrev())
				ls_ray_arr[i].ls_arr.append(*pPt);
		}
	}
}
BOOL CPlankDrawing::FindLsRayOrgAndDatumVec(f3dPoint &origin,f3dPoint &datum_vec)
{
	LSRAY_STRU* pDatumLsRay=NULL;
	for(int i=0;i<ls_ray_arr.GetSize();i++)
	{
		if(!ls_ray_arr[i].bDatumLsRay||ls_ray_arr[i].ls_arr.GetNodeNum()<2)
			continue;
		pDatumLsRay=&ls_ray_arr[i];
		break;
	}
	if(pDatumLsRay==NULL)
		return FALSE;
	//查找螺栓射线的基准方向
	if(datum_vec.IsZero())
		datum_vec=pDatumLsRay->rayResideVec;
	f3dPoint ptS=pDatumLsRay->rayOrg;
	f3dPoint ptE=ptS+datum_vec*100;
	f3dLine datum_line(ptS,ptE);
	SnapPerp(&ptS,datum_line,pDatumLsRay->ls_arr[0]);
	SnapPerp(&ptE,datum_line,pDatumLsRay->ls_arr[pDatumLsRay->ls_arr.GetNodeNum()-1]);
	datum_line.startPt=ptS;
	datum_line.endPt=ptE;
	//查找螺栓射线的定位基点
	ATOM_LIST<PT_ITEM> ptList;
	for(int i=0;i<ls_ray_arr.GetSize();i++)
	{
		LSRAY_STRU* pLsRay=&ls_ray_arr[i];
		if(pLsRay->bDatumLsRay)
			continue;
		PT_ITEM* pItem=NULL;
		for(pItem=ptList.GetFirst();pItem;pItem=ptList.GetNext())
		{
			if(pItem->origPt.IsEqual(pLsRay->rayOrg,EPS2))
			{
				pItem->nNum++;
				break;
			}
		}
		if(pItem==NULL)
		{
			pItem=ptList.append();
			pItem->origPt=pLsRay->rayOrg;
			pItem->extendPt=pLsRay->rayOrg+pLsRay->rayResideVec*20;
			pItem->nNum=1;
		}
	}
	int nMax=0;
	for(PT_ITEM* pItem=ptList.GetFirst();pItem;pItem=ptList.GetNext())
	{
		if(nMax<pItem->nNum&&datum_line.PtInLine(pItem->origPt,2)!=0)
		{
			nMax=pItem->nNum;
			origin=pItem->origPt;
		}
	}
	if(nMax==0 && ls_ray_arr.GetSize()>=2)
	{	//多跟根螺栓射线，取第一根与基准射线直接求交计算原点
		for (int i = 0; i < ls_ray_arr.GetSize(); i++)
		{
			if(ls_ray_arr[i].bDatumLsRay)
				continue;
			if (fabs(ls_ray_arr[i].rayResideVec*datum_vec) < 0.9)
			{
				int iRet = Int3dll(datum_line, f3dLine(ls_ray_arr[i].rayOrg, ls_ray_arr[i].rayOrg + ls_ray_arr[i].rayResideVec), origin);
				if (iRet != 0 && iRet != -1 && datum_line.PtInLine(origin) > 0)
					return TRUE;
			}
		}
	}
	if(nMax==0)
		return FALSE;
	return TRUE;
}
//完善螺栓射线信息(从DimLsRay单独提取出来)
void CPlankDrawing::CompleteLsRayInfo(CLDSGeneralPlate* pGeneralPlate,BOOL bAdjustPlateUcs/*=FALSE*/)
{
	if(pGeneralPlate==NULL || pGeneralPlate->GetLsCount()<=1)
		return;
	//查找钢板的依赖杆件及定位节点
	CSuperSmartPtr<CLDSPart> pSuperPlate=pGeneralPlate;
	CLDSLineAngle* pDatumAngle=NULL, *pAngle = NULL;
	CLDSNode* pDatumNode=NULL;
	if(pGeneralPlate->GetClassTypeId()==CLS_PLATE)
	{
		pDatumNode=pGeneralPlate->BelongModel()->FromNodeHandle(pSuperPlate.PlatePointer()->designInfo.m_hBaseNode);
		pDatumAngle=(CLDSLineAngle*)pGeneralPlate->BelongModel()->FromPartHandle(pSuperPlate.PlatePointer()->designInfo.m_hBasePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pDatumAngle && pDatumAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupJg=(CLDSGroupLineAngle*)pDatumAngle;
			CDesignLjPartPara* pDesLjPara=NULL;
			for(int i=0;i<4;i++)
			{
				pDesLjPara=pSuperPlate.PlatePointer()->GetLjRodPara(pGroupJg->son_jg_h[i]);
				if(pDesLjPara)
					break;
			}
			if(pDesLjPara)
				pDatumAngle=(CLDSLineAngle*)pGeneralPlate->BelongModel()->FromPartHandle(pDesLjPara->hPart);
			else
				pDatumAngle=(CLDSLineAngle*)pGeneralPlate->BelongModel()->FromPartHandle(pGroupJg->m_hDatumSonAngle);
		}
		else if (pDatumAngle != NULL && pSuperPlate.pPlate->GetLjRodPara(pDatumAngle->handle) == NULL)
		{	//基准杆件一定是连接杆件之一，否则表示不合法，此时查找与基准杆件共线的连接杆件
			CPtInLineCheck check(pDatumAngle->xPosStart, pDatumAngle->xPosEnd);
			for (CDesignLjPartPara* pDesLjPara = pSuperPlate.pPlate->designInfo.partList.GetFirst(); pDesLjPara; pDesLjPara = pSuperPlate.pPlate->designInfo.partList.GetNext())
			{
				if ((pAngle = (CLDSLineAngle*)m_pPlate->BelongModel()->FromPartHandle(pDesLjPara->hPart)) == NULL)
					continue;
				bool blStartInLine = false, blEndInLine = false;
				check.CheckPointEx(pAngle->xPosStart, &blStartInLine, 1);
				check.CheckPointEx(pAngle->xPosStart, &blEndInLine, 1);
				if (blStartInLine&&blEndInLine)
				{	//钢板连接杆件中找到了更合适的基准杆件，这部分主要是修正过往钢板基准杆件数据错误的。 wjh-2019.5.19
					pDatumAngle = pAngle;
					break;
				}
			}
		}
	}
	else //if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{	
		if(pSuperPlate.ParamPlatePointer()->m_iParamType==TYPE_SHOEPLATE)
		{	//靴板
			pDatumNode=pSuperPlate.pParamPlate->GetBaseNode();
			long hDatumJg=0;
			pSuperPlate.ParamPlatePointer()->GetDesignItemValue('D',&hDatumJg);
			pDatumAngle=(CLDSLineAngle*)pGeneralPlate->BelongModel()->FromPartHandle(hDatumJg,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		}
		else if(pSuperPlate.ParamPlatePointer()->m_iParamType==TYPE_TUBERIBPLATE)
		{	//分析跨越连板螺栓射线

		}
	}
	if(pDatumAngle==NULL||pDatumNode==NULL)
		return;	//暂只处理角钢
	RAYROD_AXIS* pRayRodAxis=NULL;
	BOOL bDrawOuterLsRay=FALSE;	//添加临时变量控制是否绘制位于板外的螺栓射线 wht 15-08-12
	CHashList<LSRAYLINE_PARA>hashBelongRodAxies;	//键值为射线号，数据值为归属的射线杆件的句柄
	CHashListEx<RAYROD_AXIS> hashRayRodAxes;
	CLDSLineAngle* pRayAngle=NULL;
	LSRAY_STRU *pLsRay=NULL;
	hashRayRodAxes.Empty();
	if (pGeneralPlate->GetClassTypeId() == CLS_PLATE)
	{	//单孔垫板不需标注任何心线
		CLDSPlate *pPlate = (CLDSPlate*)pGeneralPlate;
		for (CDesignLjPartPara* pLjPart = pPlate->GetFirstLjPara(); pLjPart; pLjPart = pPlate->GetNextLjPara())
		{
			CLDSLineAngle* pAngle = (CLDSLineAngle*)pPlate->BelongModel()->FromPartHandle(pLjPart->hPart, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
			if (pAngle == NULL)
				continue;	//暂只处理角钢
			pRayRodAxis = hashRayRodAxes.Add(pLjPart->hPart);
			pRayRodAxis->iFaceNo = pLjPart->iFaceNo;
			pRayRodAxis->ciWorkWingX0Y1 = pLjPart->angle.cur_wing_x0_y1;
			//为防止原始数据中连接端信息有错误进而导致心线延伸有误,需要进一步进行核实 wjh-2015.2.12
			double ddx = pPlate->ucs.axis_z*pAngle->get_norm_x_wing();
			double ddy = pPlate->ucs.axis_z*pAngle->get_norm_y_wing();
			if (pAngle->pStart&&pAngle->pStart->handle == pPlate->designInfo.m_hBaseNode)
				pRayRodAxis->start_end = 0;
			else if (pAngle->pEnd&&pAngle->pEnd->handle == pPlate->designInfo.m_hBaseNode)
				pRayRodAxis->start_end = 1;
			else
				pRayRodAxis->start_end = 2;
			if (fabs(ddx) > fabs(ddy))
				pRayRodAxis->ciWorkWingX0Y1 = 0;
			else
				pRayRodAxis->ciWorkWingX0Y1 = 1;
			//判断当前杆件是否为基准角钢
			if (pLjPart->hPart == pDatumAngle->handle)
				pRayRodAxis->bDatumRod = true;
		}
	}
	//当缺少CLDSPart::designInfo信息时，从螺栓定位基准杆件汇总射线角钢
	for (CLsRef* pLsRef = pGeneralPlate->GetFirstLsRef(); pLsRef; pLsRef = pGeneralPlate->GetNextLsRef())
	{
		long hLsDesRod = pLsRef->GetLsPtr()->des_base_pos.hPart;
		pRayRodAxis = hashRayRodAxes.GetValue(hLsDesRod);
		if (pRayRodAxis == NULL)
		{
			pRayAngle = (CLDSLineAngle*)pGeneralPlate->BelongModel()->FromPartHandle(hLsDesRod, CLS_LINEANGLE, CLS_GROUPLINEANGLE);
			if (pRayAngle == NULL)
				continue;	//暂只处理角钢 wjh-2015.2.8
			pRayRodAxis = hashRayRodAxes.Add(pRayAngle->handle);
			double ddx = pLsRef->GetLsPtr()->ucs.axis_z*pRayAngle->get_norm_x_wing();
			double ddy = pLsRef->GetLsPtr()->ucs.axis_z*pRayAngle->get_norm_y_wing();
			if (pRayAngle->pStart&&pRayAngle->pStart->handle == pDatumNode->handle)
				pRayRodAxis->start_end = 0;
			else if (pRayAngle->pEnd&&pRayAngle->pEnd->handle == pDatumNode->handle)
				pRayRodAxis->start_end = 1;
			else
				pRayRodAxis->start_end = 2;
			if (fabs(ddx) > fabs(ddy))
				pRayRodAxis->ciWorkWingX0Y1 = 0;
			else
				pRayRodAxis->ciWorkWingX0Y1 = 1;
			//判断当前杆件是否为基准角钢
			if (pRayRodAxis->hRod == pDatumAngle->handle)
				pRayRodAxis->bDatumRod = true;
		}
	}
	if(hashRayRodAxes.GetNodeNum()>0)
	{
		for(pRayRodAxis=hashRayRodAxes.GetFirst();pRayRodAxis;pRayRodAxis=hashRayRodAxes.GetNext())
		{
			pRayAngle=(CLDSLineAngle*)pGeneralPlate->BelongModel()->FromRodHandle(pRayRodAxis->hRod);
			if(pRayAngle==NULL)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("0X%X plate's linking rod 0X%X is already not exist!",m_pPlate->handle,pRayRodAxis->hRod);
#else
				logerr.Log("0X%X钢板的连接杆件0X%X已不存在!",m_pPlate->handle,pRayRodAxis->hRod);
#endif
				continue;
			}
			JGZJ jgzj;
			if(pRayRodAxis->ciWorkWingX0Y1==0)
			{
				pRayRodAxis->wing_vec=pRayAngle->GetWingVecX();
				jgzj=pRayAngle->GetZhunJu('X');
			}
			else
			{
				pRayRodAxis->wing_vec=pRayAngle->GetWingVecY();
				jgzj=pRayAngle->GetZhunJu('Y');
			}
			pRayRodAxis->ber_space=jgzj.g;
			pRayRodAxis->wing_space=pRayAngle->GetWidth()-pRayRodAxis->ber_space;
			if(pRayRodAxis->start_end==0)
			{	//始端连接
				pRayRodAxis->dfOrgTipExtend = max(0, pRayAngle->startOdd());
				pRayRodAxis->org=pRayAngle->Start()+pRayRodAxis->wing_vec*jgzj.g;
				pRayRodAxis->axis_vec=pRayAngle->End()-pRayAngle->Start();
			}
			else
			{	//终端连接
				pRayRodAxis->dfOrgTipExtend = max(0, pRayAngle->endOdd());
				pRayRodAxis->org=pRayAngle->End()+pRayRodAxis->wing_vec*jgzj.g;
				pRayRodAxis->axis_vec=pRayAngle->Start()-pRayAngle->End();
			}
			normalize(pRayRodAxis->axis_vec);
			//将全部坐标转换到最终执行的新绘图坐标系new_ucs下
			if(pRayRodAxis->iFaceNo<=1)
			{	//单面板
				coord_trans(pRayRodAxis->org,new_ucs,FALSE,TRUE);
				vector_trans(pRayRodAxis->axis_vec,new_ucs,FALSE,TRUE);
				vector_trans(pRayRodAxis->wing_vec,new_ucs,FALSE,TRUE);
			}
			else if(pRayRodAxis->iFaceNo>1)
			{
				coord_trans(pRayRodAxis->org, pGeneralPlate->ucs,FALSE,TRUE);
				vector_trans(pRayRodAxis->axis_vec, pGeneralPlate->ucs,FALSE,TRUE);
				vector_trans(pRayRodAxis->wing_vec, pGeneralPlate->ucs,FALSE,TRUE);
				pRayRodAxis->org= pGeneralPlate->GetSpreadVertex(pRayRodAxis->org,pRayRodAxis->iFaceNo,true);
				pRayRodAxis->axis_vec= pGeneralPlate->GetSpreadVertex(pRayRodAxis->axis_vec,pRayRodAxis->iFaceNo,false);
				pRayRodAxis->wing_vec= pGeneralPlate->GetSpreadVertex(pRayRodAxis->wing_vec,pRayRodAxis->iFaceNo,false);
				if(bAdjustPlateUcs)
				{
					coord_trans(pRayRodAxis->org, pGeneralPlate->ucs,TRUE,TRUE);
					vector_trans(pRayRodAxis->axis_vec, pGeneralPlate->ucs,TRUE,TRUE);
					vector_trans(pRayRodAxis->wing_vec, pGeneralPlate->ucs,TRUE,TRUE);

					coord_trans(pRayRodAxis->org,new_ucs,FALSE,TRUE);
					vector_trans(pRayRodAxis->axis_vec,new_ucs,FALSE,TRUE);
					vector_trans(pRayRodAxis->wing_vec,new_ucs,FALSE,TRUE);
				}
			}
			pRayRodAxis->org.z=pRayRodAxis->axis_vec.z=pRayRodAxis->wing_vec.z=0;	//z坐标归零
		}
		//识别每根射线杆件涵盖的螺栓线
		for(pRayRodAxis=hashRayRodAxes.GetFirst();pRayRodAxis;pRayRodAxis=hashRayRodAxes.GetNext())
		{	
			if(pRayRodAxis->bDatumRod)
				continue;	//只有射线杆件需要处理，基准杆件此处不处理
			GECS rectCS;
			rectCS.origin=pRayRodAxis->org;
			rectCS.axis_x=pRayRodAxis->axis_vec;
			rectCS.axis_y=-pRayRodAxis->wing_vec;
			rectCS.axis_x.z=rectCS.axis_y.z=0;
			normalize(rectCS.axis_x);
			rectCS.axis_z=rectCS.axis_x^rectCS.axis_y;
			if(rectCS.axis_z.z>0)
				rectCS.axis_y=f3dPoint(0,0,1)^rectCS.axis_x;
			else
				rectCS.axis_y=f3dPoint(0,0,-1)^rectCS.axis_x;
			for(int i=0;i<ls_ray_arr.GetSize();i++)
			{
				pLsRay=&ls_ray_arr[i];
				//不绘制单排螺栓丁字尺寸时,max_y和min_y不需要设置默认值为0 wht 15-08-12
				CMaxDouble max_y;
				CMinDouble min_x,min_y;
				bool bOuterBolt=false,samelenVec=true;
				GEPOINT rayvec=pLsRay->rayResideVec;
				rayvec=rectCS.TransVToCS(rayvec);
				if(pLsRay->ls_arr.GetNodeNum()>1)
				{
					rayvec=pLsRay->ls_arr[1]-pLsRay->ls_arr[0];
					normalize(rayvec);
					rayvec=rectCS.TransVToCS(rayvec);
				}
				for(f3dPoint *pLocate=pLsRay->ls_arr.GetFirst();pLocate;pLocate=pLsRay->ls_arr.GetNext())
				{
					GEPOINT location=rectCS.TransPToCS(*pLocate);
					if (location.x <= -pRayRodAxis->dfOrgTipExtend - EPS2)
						bOuterBolt=true;
					else if(location.y>=pRayRodAxis->ber_space)
						bOuterBolt=true;
					else if(location.y<=-pRayRodAxis->wing_space)
						bOuterBolt=true;
					if(fabs(rayvec.y)>EPS2)	//该射线方向与当前射线杆件方向不一致，即使全部螺栓在当前杆件内也不可视为涵盖射线
						bOuterBolt=true;
					if(bOuterBolt)
						break;
					max_y.Update(location.y,pLocate);
					min_y.Update(location.y,pLocate);
					if(min_x.IsInited()&&location.x<min_x)
						samelenVec=false;	//pLsRay->ls_arr由远至近，与螺栓射线方向相反
					min_x.Update(location.x);
				}
				if(!bOuterBolt)
				{
					if(pRayRodAxis->ber_dist<max_y)
						pRayRodAxis->ber_dist=max_y;
					if(pRayRodAxis->wing_dist>min_y)
						pRayRodAxis->wing_dist=min_y;
					if(pRayRodAxis->ray_dist>min_x)
						pRayRodAxis->ray_dist=min_x;
					double coord_y=0;
					if(min_y.m_pRelaObj)
						coord_y=min_y;
					else if(max_y.m_pRelaObj)
						coord_y=max_y;
					LSRAYLINE_PARA* pRayPara=hashBelongRodAxies.SetValue(pLsRay->iRayNo,LSRAYLINE_PARA(pRayRodAxis->hRod,min_x,coord_y));
					pRayPara->reverseLenvec=!samelenVec;
				}
			}
		}
		//补齐单孔射线
		for(int i=0;i<ls_ray_arr.GetSize();i++)
		{
			pLsRay = &ls_ray_arr[i];
			f3dPoint line_vec;
			LSRAYLINE_PARA* pRayBelongPara=hashBelongRodAxies.GetValue(pLsRay->iRayNo);
			if(pRayBelongPara)
				pRayRodAxis=hashRayRodAxes.GetValue(pRayBelongPara->hRod);
			else
				pRayRodAxis=NULL;
			if(pRayRodAxis==NULL)
				continue;	//可能该射线属基准角钢螺栓心线
			pLsRay->rayOrg=pRayRodAxis->org;			//用以查找与基准角钢的基点
			pLsRay->rayResideVec=pRayRodAxis->axis_vec;	//用以调整ls_arr顺序
			if(pLsRay->ls_arr.GetNodeNum()==1)
			{	//补齐单孔射线
				if(pRayRodAxis->start_end<2&&fabs(pRayBelongPara->coord_y)<1&&!pLsRay->ls_arr[0].IsEqual(pRayRodAxis->org,1))
				{
					if(bDrawOuterLsRay||PtInPlateRgn(pRayRodAxis->org))
						pLsRay->ls_arr.insert(pRayRodAxis->org,0)->feature=-1;	//feature=-1表示该点不需要延伸绘制
				}
			}
			else if(pRayBelongPara->coord_x>pRayRodAxis->ray_dist+1)	//容许误差预定值为1mm
			{	//补齐多孔射线与同属（归属同一射线杆件）射线螺栓孔间的错孔点
				f3dPoint ray_pick1=pLsRay->ls_arr[0];
				f3dPoint ray_pick2=pLsRay->ls_arr[1];
				f3dPoint insertpos=pRayRodAxis->org+pRayRodAxis->axis_vec*pRayRodAxis->ray_dist;
				SnapPerp(&insertpos,ray_pick1,ray_pick2,insertpos);
				if(pRayBelongPara->reverseLenvec)
					pLsRay->ls_arr.append(insertpos)->feature=-1;	//feature=-1表示该点不需要延伸绘制
				else
					pLsRay->ls_arr.insert(insertpos,0)->feature=-1;	//feature=-1表示该点不需要延伸绘制
			}
			else if(pRayBelongPara->coord_x>1&&fabs(pRayBelongPara->coord_y)<1&&pRayRodAxis->start_end<2)
			{	//单排射线未交于钢板定位原点螺栓的情况，补齐预定位点
				if(!bDrawOuterLsRay&&!PtInPlateRgn(pRayRodAxis->org))
					continue;	//pRayRodAxis->org不在钢板内
				if(pRayBelongPara->reverseLenvec)
					pLsRay->ls_arr.append(pRayRodAxis->org)->feature=-1;	//feature=-1表示该点不需要延伸绘制
				else
					pLsRay->ls_arr.insert(pRayRodAxis->org,0)->feature=-1;	//feature=-1表示该点不需要延伸绘制
			}
		}
		for(pRayRodAxis=hashRayRodAxes.GetFirst();pRayRodAxis;pRayRodAxis=hashRayRodAxes.GetNext())
		{	
			if(pRayRodAxis->ray_dist>1&&fabs(pRayRodAxis->ber_dist-pRayRodAxis->wing_dist)>1
				&&(bDrawOuterLsRay||PtInPlateRgn(pRayRodAxis->org)))	//pRayRodAxis->org在钢板内
			{
				LSRAY_STRU lsray;
				lsray.iRayNo=0;
				if(pRayRodAxis->start_end<2)
				{
					ls_ray_arr.Add(lsray);
					i = ls_ray_arr.GetUpperBound();
					ls_ray_arr[i].iRayNo = 0;
					ls_ray_arr[i].rayOrg = pRayRodAxis->org;
					ls_ray_arr[i].rayResideVec = pRayRodAxis->axis_vec;
					ls_ray_arr[i].ls_arr.append(pRayRodAxis->org)->feature=-1;	//feature=-1表示该点不需要延伸绘制
					ls_ray_arr[i].ls_arr.append(pRayRodAxis->org+pRayRodAxis->ray_dist*pRayRodAxis->axis_vec)->feature=-1;
				}
				GEPOINT ray_pos = pRayRodAxis->org + pRayRodAxis->ray_dist*pRayRodAxis->axis_vec;
				GEPOINT ber_pos = ray_pos - pRayRodAxis->wing_vec*pRayRodAxis->ber_dist;
				GEPOINT wing_pos = ray_pos - pRayRodAxis->wing_vec*pRayRodAxis->wing_dist;
				ls_ray_arr.Add(lsray);
				i = ls_ray_arr.GetUpperBound();
				ls_ray_arr[i].iRayNo = 0;
				ls_ray_arr[i].ls_arr.append(ber_pos)->feature=-1;
				if( fabs(pRayRodAxis->ber_dist)>=1&&fabs(pRayRodAxis->wing_dist)>1&&fabs(pRayRodAxis->ber_dist-pRayRodAxis->wing_dist)>1
					&&pRayRodAxis->start_end<2)
					ls_ray_arr[i].ls_arr.append(ray_pos)->feature=-1;
				ls_ray_arr[i].ls_arr.append(wing_pos)->feature=-1;
				ls_ray_arr[i].rayOrg = ber_pos;
				ls_ray_arr[i].rayResideVec = (ls_ray_arr[i].ls_arr[1]- ls_ray_arr[i].ls_arr[0]).normalized();
			}
		}
	}
	//查找基准螺栓射线,初始化基准射线信息
	RAYROD_AXIS* pDatumRodAxis=hashRayRodAxes.GetValue(pDatumAngle->handle);
	if(pDatumRodAxis)
	{
		f3dPoint origin,wing_vec,axis_vec;
		origin=pDatumAngle->Start();
		axis_vec=pDatumAngle->End()-pDatumAngle->Start();
		normalize(axis_vec);
		if(pDatumRodAxis->ciWorkWingX0Y1==0)
			wing_vec=pDatumAngle->GetWingVecX();
		else
			wing_vec=pDatumAngle->GetWingVecY();
		GECS rectCS;
		rectCS.origin=origin;
		rectCS.axis_x=axis_vec;
		rectCS.axis_y=wing_vec;
		rectCS.axis_z=rectCS.axis_x^rectCS.axis_y;
		for(int i=0;i<ls_ray_arr.GetSize();i++)
		{
			LSRAY_STRU *pLsRay=&ls_ray_arr[i];
			if(fabs(pLsRay->rayResideVec*pDatumRodAxis->axis_vec)<0.999)
				continue;	//延伸方向不一致
			BOOL bOuterBolt=FALSE;
			for(f3dPoint *pLocate=pLsRay->ls_arr.GetFirst();pLocate;pLocate=pLsRay->ls_arr.GetNext())
			{
				GEPOINT location=*pLocate;
				coord_trans(location,new_ucs,TRUE);
				coord_trans(location,rectCS,FALSE,TRUE);
				if(location.y<=-EPS2)
					bOuterBolt=TRUE;
				else if(location.y>=pDatumAngle->GetWidth())
					bOuterBolt=TRUE;
				if(bOuterBolt)
					break;
			}
			if(bOuterBolt)
				continue;
			pLsRay->bDatumLsRay=TRUE;
			pLsRay->rayOrg=pDatumRodAxis->org;
			pLsRay->rayResideVec=pDatumRodAxis->axis_vec;
			CLDSGroupLineAngle* pGroupJg=(CLDSGroupLineAngle*)pGeneralPlate->BelongModel()->FromPartHandle(pDatumAngle->group_father_jg_h);
			if(pGroupJg)
			{	//组合角钢重新计算射线原点
				if(pDatumRodAxis->start_end==0)
					pLsRay->rayOrg=pGroupJg->Start();
				else
					pLsRay->rayOrg=pGroupJg->End();
				coord_trans(pLsRay->rayOrg,new_ucs,FALSE,TRUE);
				pLsRay->rayOrg.z=0;
			}
		}
	}
	CorrectLsRays();	//对螺栓射线信息进行修正
}
void CPlankDrawing::DimLsRay(CLDSPart *pPart/*=NULL*/,BOOL bAdjustPlateUcs/*=FALSE*/)
{	//调整钢板坐标系时，需要参数pPart。用来调整螺栓线的位置
	if(pPart==NULL||!pPart->IsPlate()||pPart->GetLsCount()<=1)
		return;	//单孔螺栓不做处理
	//根据钢板上的螺栓及连接杆件完善螺栓射线信息
	CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
	CompleteLsRayInfo(pPlate,bAdjustPlateUcs);
	//
	f3dPoint orgin,datum_vec;
	BOOL bValid=FindLsRayOrgAndDatumVec(orgin,datum_vec);
	//标注螺栓射线间夹角
	ROTATEDIM_STRU rotate_dim;
	for(int i=0;i<ls_ray_arr.GetSize();i++)
	{
		LSRAY_STRU* pLsRay = &ls_ray_arr[i];
		if(pLsRay->ls_arr.GetNodeNum()<2)
			continue;
		for(int j=0;j<pLsRay->ls_arr.GetNodeNum()-1;j++)
		{
			rotate_dim.startPos=pLsRay->ls_arr[j];
			rotate_dim.endPos  =pLsRay->ls_arr[j+1];
			rotate_dim.angle = Cal2dLineAng(0,0,rotate_dim.endPos.x-rotate_dim.startPos.x,rotate_dim.endPos.y-rotate_dim.startPos.y);
			rotate_dim.dist = DISTANCE(rotate_dim.startPos,rotate_dim.endPos);	//输出时控制精度 wht 15-07-13
			Sub_Pnt(rotate_dim.dimPos,rotate_dim.endPos,rotate_dim.startPos);
			Add_Pnt(rotate_dim.dimPos,rotate_dim.startPos,rotate_dim.dimPos*0.5);
			rotate_dim_arr.Add(rotate_dim);
		}
	}
	//螺栓射线角度的标注
	if(dimStyleFlag.IsHasFlag(CPlankDrawing::DimBoltAngle)&&bValid)
	{
		ARRAY_LIST<RAYROD_AXIS> rayAxisArr;
		rayAxisArr.SetSize(0,ls_ray_arr.GetSize());
		f3dPoint datum_ls_pos,intersPt;
		int nLeft=0,nRight=0;
		for(int i=0;i<ls_ray_arr.GetSize();i++)
		{
			LSRAY_STRU* pLsRay=&ls_ray_arr[i];
			f3dLine ray_line(pLsRay->rayOrg,pLsRay->rayOrg+pLsRay->rayResideVec*100);
			if(ray_line.PtInLine(orgin,2)==0)
				continue;	//标注原点不在此射线轴
			RAYROD_AXIS* pRayAxis=NULL;
			for(pRayAxis=rayAxisArr.GetFirst();pRayAxis;pRayAxis=rayAxisArr.GetNext())
			{
				if(pLsRay->rayResideVec*pRayAxis->axis_vec>0.9999)
					break;	//已存在此射线轴
			}
			if(pRayAxis==NULL)
			{
				pRayAxis=rayAxisArr.append();
				pRayAxis->axis_vec=pLsRay->rayResideVec;
				pRayAxis->fAngle=CalRotateAngle(f3dPoint(0,0,1),datum_vec,pRayAxis->axis_vec);
				pRayAxis->fRadius=DISTANCE(orgin,pLsRay->ls_arr[pLsRay->ls_arr.GetNodeNum()-1])*0.8;
				if(pLsRay->bDatumLsRay)
				{	//基准射线需要添加反向射线
					pRayAxis=rayAxisArr.append();
					pRayAxis->axis_vec=datum_vec*(-1);
					pRayAxis->fAngle=CalRotateAngle(f3dPoint(0,0,1),datum_vec,pRayAxis->axis_vec);
					pRayAxis->fRadius=DISTANCE(orgin,pLsRay->ls_arr[0])*0.8;
					if(datum_ls_pos.IsZero())
						SnapPerp(&datum_ls_pos,ray_line,pLsRay->ls_arr[0]);
				}
				else
				{	//记录其他射线的分布
					if(fabs(pRayAxis->fAngle)<0.01)
						continue;
					else if(pRayAxis->fAngle>0.01)
						nLeft++;
					else if(pRayAxis->fAngle<-0.01)
						nRight++;
				}
			}
		}
		CQuickSort<RAYROD_AXIS>::QuickSort(rayAxisArr.m_pData,rayAxisArr.GetSize(),compare_func1);
		//计算标注角度并对角度进行排序
		int nn=rayAxisArr.GetSize();
		ANGULARDIM_STRU angular_dim;
		ARRAY_LIST<ANGULARDIM_STRU> dimAngleArr;
		dimAngleArr.SetSize(0,nn);
		for(int i=0;i<nn;i++)
		{
			angular_dim.vertex=orgin;
			angular_dim.startPos=rayAxisArr[i].axis_vec;
			angular_dim.endPos=rayAxisArr[(i+1)%nn].axis_vec;
			angular_dim.fRadius=min(rayAxisArr[i].fRadius,rayAxisArr[(i+1)%nn].fRadius);
			angular_dim.angle=CalRotateAngle(f3dPoint(0,0,1),angular_dim.startPos,angular_dim.endPos);
			if(angular_dim.angle<0)
				angular_dim.angle+=2*Pi;
			dimAngleArr.append(angular_dim);
		}
		CQuickSort<ANGULARDIM_STRU>::QuickSort(dimAngleArr.m_pData,dimAngleArr.GetSize(),compare_func2);
		//添加角度标注
		double fMinR=40;
		double fRadius=fMinR;
		f3dLine edge_line;
		if(!PtInPlateRgn(orgin))
		{	//标注原点在板轮廓外,考虑原点到最近轮廓边的距离
			int nVertex=pPlate->vertex_list.GetNodeNum();
			for(int j=0;j<nVertex;j++)
			{
				if(pPlate->vertex_list[j].type!=0||pPlate->vertex_list[j].m_bRollEdge)
					continue;
				f3dPoint f_start=pPlate->vertex_list[j].vertex;
				f3dPoint f_end=pPlate->vertex_list[(j+1)%nVertex].vertex;
				coord_trans(f_start,pPlate->ucs,TRUE);
				coord_trans(f_end,pPlate->ucs,TRUE);
				coord_trans(f_start,new_ucs,FALSE);
				coord_trans(f_end,new_ucs,FALSE);
				if(sys.part_map.bExportDeformedProfile)
				{
					f_start=pPlate->GetDeformedVertex(f_start,new_ucs);
					f_end=pPlate->GetDeformedVertex(f_end,new_ucs);
				}
				if(Int3dll(f_start,f_end,orgin,datum_ls_pos,intersPt)==1)
				{
					fRadius=fMinR+DISTANCE(orgin,intersPt);
					edge_line.startPt=f_start;
					edge_line.endPt=f_end;
					break;
				}				
			}
		}
		int nSize=(nLeft>0&&nRight>0)?dimAngleArr.GetSize()-1:dimAngleArr.GetSize()-2;
		for(int i=0;i<nSize;i++)
		{
			double fR=max(dimAngleArr[i].fRadius,fRadius);
			f3dPoint dimStart=orgin+dimAngleArr[i].startPos*fR;
			f3dPoint dimEnd  =orgin+dimAngleArr[i].endPos*fR;
			if(fRadius>fMinR)
			{	//角度标注的辅助线
				Int3dll(edge_line,f3dLine(orgin,dimStart),intersPt);
				assist_ls_ray_arr.Add(f3dLine(intersPt,dimStart));
				Int3dll(edge_line,f3dLine(orgin,dimEnd),intersPt);
				assist_ls_ray_arr.Add(f3dLine(intersPt,dimEnd));
			}
			//
			angular_dim.vertex=orgin;
			angular_dim.startPos=dimStart;
			angular_dim.endPos=dimEnd;
			angular_dim.dimPos=0.5*(dimStart+dimEnd);
			angular_dim_arr.Add(angular_dim);
		}
	}
	if(dimStyleFlag.IsHasFlag(CPlankDrawing::DimBoltSpaceToWeldEdge))
	{	//标注距离焊接边最近的螺栓孔到焊接边的最小垂直距离 wjh-2016.9.21
		PROFILE_VERTEX* pPrevVertex=pPlate->vertex_list.GetTail();
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(!pPrevVertex->m_bWeldEdge)
			{
				pPrevVertex=pVertex;
				continue;
			}
			CLsRef* pLsRef;
			if(pPrevVertex->type==0)	//直焊缝边
			{
				GEPOINT pos;
				CMinDouble mindist;
				for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					if(!pPlate->SpreadLsPos(pLsRef->GetLsPtr(),pos))
						continue;
					double scaleInLine=0;
					double distance=DistOf2dPtLine(pos.x,pos.y,pPrevVertex->vertex.x,pPrevVertex->vertex.y,
												pVertex->vertex.x,pVertex->vertex.y,&scaleInLine);
					if(scaleInLine>-EPS&&scaleInLine<1+EPS)
						mindist.Update(distance,pLsRef->GetLsPtr(),EPS2);
				}
				if(!mindist.IsInited())
				{
					pPrevVertex=pVertex;
					continue;	//可能本来就没有螺栓孔
				}
				CLDSBolt* pNearestBolt=(CLDSBolt*)mindist.m_pRelaObj;
				pPlate->SpreadLsPos(pNearestBolt,pos);
				f3dPoint perp;
				double nearest=mindist.number;
				SnapPerp(&perp,pPrevVertex->vertex,pVertex->vertex,pos,&nearest);
				coord_trans(pos ,pPlate->ucs,TRUE,TRUE);
				coord_trans(perp,pPlate->ucs,TRUE,TRUE);
				coord_trans(pos ,new_ucs,FALSE,TRUE);
				coord_trans(perp,new_ucs,FALSE,TRUE);
				rotate_dim.startPos=pos;
				rotate_dim.endPos=perp;
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dist = nearest;
				rotate_dim.angle = Cal2dLineAng(0,0,rotate_dim.endPos.x-rotate_dim.startPos.x,rotate_dim.endPos.y-rotate_dim.startPos.y);
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim_arr.Add(rotate_dim);
				pPrevVertex=pVertex;
			}
			else
				pPrevVertex=pVertex;
		}
	}
	//标注与火曲线平行的螺栓射线间距   wxc-2015.9.9
	if(dimStyleFlag.IsHasFlag(CPlankDrawing::DimBoltSpaceToHuoQuLine) && pPlate->GetFaceN()>1)
	{
		PROFILE_VERTEX* pBendLine1Start=NULL,*pBendLine1End=NULL;
		PROFILE_VERTEX* pBendLine2Start=NULL,*pBendLine2End=NULL;
		pPlate->GetBendlineVertexies(&pBendLine1Start,&pBendLine1End,&pBendLine2Start,&pBendLine2End);
		for(int m=1;m<pPlate->GetFaceN();m++)
		{
			f3dPoint huoqu_start,huoqu_end;
			if(m==1&&pBendLine1Start&&pBendLine1End)
			{
				huoqu_start=pBendLine1Start->vertex;
				huoqu_end  =pBendLine1End->vertex; 
			}
			if(m==1&&pBendLine2Start&&pBendLine2End)
			{
				huoqu_start=pBendLine2Start->vertex;
				huoqu_end  =pBendLine2End->vertex;
			}
			if(huoqu_start.IsZero()&&huoqu_end.IsZero())
				continue;
			if(sys.part_map.bExportDeformedProfile)
			{
				huoqu_start=pPlate->GetDeformedVertex(huoqu_start);
				huoqu_end=pPlate->GetDeformedVertex(huoqu_end);
			}
			if(bAdjustPlateUcs)
			{
				coord_trans(huoqu_start,pPlate->ucs,TRUE,TRUE);
				coord_trans(huoqu_start,new_ucs,FALSE,TRUE);
				coord_trans(huoqu_end,pPlate->ucs,TRUE,TRUE);
				coord_trans(huoqu_end,new_ucs,FALSE,TRUE);
			}
			f3dPoint huoqu_vec=huoqu_end-huoqu_start;
			normalize(huoqu_vec);
			//螺栓射线到火曲线的距离标注
			int index=-1;
			double fMinDist=1000;
			for(i=0;i<ls_ray_arr.GetSize();i++)
			{
				LSRAY_STRU* pLsRay = &ls_ray_arr[i];
				if(fabs(huoqu_vec*pLsRay->rayResideVec)<EPS_COS2)
					continue;
				double dist=DistOf2dPtLine(pLsRay->ls_arr[0],huoqu_start,huoqu_end);
				if(fMinDist>fabs(dist))
				{
					fMinDist=fabs(dist);
					index=i;
				}
			}
			if(index>=0)
			{
				LSRAY_STRU* pLsRay = &ls_ray_arr[index];
				int n=pLsRay->ls_arr.GetNodeNum();
				rotate_dim.startPos=0.5*(pLsRay->ls_arr[0]+pLsRay->ls_arr[n-1]);
				SnapPerp(&rotate_dim.endPos,huoqu_start,huoqu_end,rotate_dim.startPos);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dist = fMinDist;
				rotate_dim.angle = Cal2dLineAng(0,0,rotate_dim.endPos.x-rotate_dim.startPos.x,rotate_dim.endPos.y-rotate_dim.startPos.y);
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim_arr.Add(rotate_dim);
			}
			//挂线孔到火曲线的距离标注
			f3dLine line;
			ATOM_LIST<LSRAY_STRU> xGuaXianLsRay;
			for(int i=0;i<cir_arr.GetSize();i++)
			{
				if(cir_arr[i].pen.style<=1)
					continue;	//连接螺栓或脚钉不处理	
				f3dPoint intersPt,ls_centre=cir_arr[i].centre;
				SnapPerp(&intersPt,huoqu_start,huoqu_end,ls_centre);
				double fDist=DISTANCE(ls_centre,intersPt);
				LSRAY_STRU* pLsRay=NULL;
				for(pLsRay=xGuaXianLsRay.GetFirst();pLsRay;pLsRay=xGuaXianLsRay.GetNext())
				{
					if(pLsRay->featrue==ftoi(fDist))
						break;
				}
				if(pLsRay)
				{
					int n = pLsRay->ls_arr.GetNodeNum();
					if(n<2)	//现只有一个螺栓
						pLsRay->ls_arr.append(ls_centre);
					else
					{
						line.startPt=pLsRay->ls_arr[0];
						int j;
						for(j=1;j<n;j++)
						{
							line.endPt = pLsRay->ls_arr[j];
							int iRetCode = line.PtInLine(ls_centre,0.5);
							if(iRetCode==-2)
							{
								pLsRay->ls_arr.insert(ls_centre,j-1);
								break;
							}
							else if(iRetCode==2)
							{
								pLsRay->ls_arr.insert(ls_centre,j);
								break;
							}
							line.startPt = line.endPt;
						}
						if(j==n)
							pLsRay->ls_arr.append(ls_centre);
					}
				}
				else
				{
					pLsRay=xGuaXianLsRay.append();
					pLsRay->rayOrg=ls_centre;
					pLsRay->ls_arr.append(ls_centre);
					pLsRay->featrue=ftoi(fDist);
				}
			}
			for(int i=0;i<xGuaXianLsRay.GetNodeNum();i++)
			{
				rotate_dim.startPos=xGuaXianLsRay[i].ls_arr[0];
				if(xGuaXianLsRay[i].ls_arr.GetNodeNum()>1)
				{	//多个挂线孔在一排时，绘制辅助线
					line.startPt=xGuaXianLsRay[i].ls_arr[0];
					line.endPt=xGuaXianLsRay[i].ls_arr[xGuaXianLsRay[i].ls_arr.GetNodeNum()-1];
					assist_ls_ray_arr.Add(line);
					//调整标注原点
					f3dPoint vec=xGuaXianLsRay[i].ls_arr[1]-xGuaXianLsRay[i].ls_arr[0];
					rotate_dim.startPos+=vec.normalized()*0.3*vec.mod();
				}
				SnapPerp(&rotate_dim.endPos,huoqu_start,huoqu_end,rotate_dim.startPos);
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dist=DISTANCE(rotate_dim.startPos,rotate_dim.endPos);
				rotate_dim.angle= Cal2dLineAng(0,0,rotate_dim.endPos.x-rotate_dim.startPos.x,rotate_dim.endPos.y-rotate_dim.startPos.y);
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim_arr.Add(rotate_dim);
			}
		}
	}
	//标注轮廓特征螺栓相关标注项
	DimWrapLsSpace(pPlate);
}
void CPlankDrawing::DimWrapLsSpace(CLDSGeneralPlate* pGeneralPlate)
{
	if(!dimStyleFlag.IsHasFlag(CPlankDrawing::DimBoltSpaceToNextLsRay)&&
		!dimStyleFlag.IsHasFlag(CPlankDrawing::DimBoltSpaceToEdgeLine))
		return;
	f3dPoint ls_pos;
	ATOM_LIST<f2dCircle> ls_cir_list;
	double L=0;
	if(pGeneralPlate->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pGeneralPlate)->m_iParamType==TYPE_XEND)
		pGeneralPlate->GetDesignItemValue('L',&L);
	for(CLsRef *pLsRef=pGeneralPlate->GetFirstLsRef();pLsRef;pLsRef=pGeneralPlate->GetNextLsRef())
	{	
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		int iLsFace=pGeneralPlate->GetNormFaceI(pBolt->get_norm());	
		pGeneralPlate->SpreadLsPos(pBolt,ls_pos);
		ls_pos.feature=iLsFace+1;
		if(sys.part_map.bExportDeformedProfile)
			ls_pos=pGeneralPlate->GetDeformedVertex(ls_pos);
		if(L>0&&ls_pos.x>L)
		{	//不在十字插板基板上 wht 18-08-09
			continue;
		}
		//
		f2dCircle ls_cir(pBolt->get_d()*0.5,ls_pos.x,ls_pos.y);
		ls_cir.feature=iLsFace+1;
		ls_cir.pen.style=pBolt->FilterFuncType();
		ls_cir_list.append(ls_cir);
	}
	//查找外轮廓特征螺栓
	//通过最简便的方式找出其中一颗外轮廓特征螺栓,作为外轮廓首螺栓
	BOOL bHasTwoLs=FALSE,bOneAxisLineLs=TRUE;	//所有螺栓都在一条心线上的情况
	int start_space,end_space;
	f3dLine axis_line;
	f2dCircle *pStartLsCir=NULL,*pEndLsCir=NULL;
	for(f2dCircle *pLsCir=ls_cir_list.GetFirst();pLsCir;pLsCir=ls_cir_list.GetNext())
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
		return;		//所有螺栓都在同一直线上
	ATOM_LIST<f2dCircle> out_ls_list;
	out_ls_list.append(*pStartLsCir);
	f2dCircle *pFirstStartLsCir=pStartLsCir;
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
				//由点在线左侧算法改为直接求包络公切线夹角算法,这样可以避免圆直径不同的影响 WJH-2004.09.22
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
	//
	ROTATEDIM_STRU rotate_dim;
	if(dimStyleFlag.IsHasFlag(CPlankDrawing::DimBoltSpaceToNextLsRay))
	{	//标注轮廓特征螺栓之间的螺栓距离
		f3dPoint f_start,f_end;
		int nNum=out_ls_list.GetNodeNum();
		for(int i=0;i<nNum;i++)
		{
			f_start.Set(out_ls_list[i].centre.x,out_ls_list[i].centre.y,0);
			f_end.Set(out_ls_list[(i+1)%nNum].centre.x,out_ls_list[(i+1)%nNum].centre.y,0);
			coord_trans(f_start,pGeneralPlate->ucs,TRUE);
			coord_trans(f_start,new_ucs,FALSE);
			coord_trans(f_end,pGeneralPlate->ucs,TRUE);
			coord_trans(f_end,new_ucs,FALSE);
			if(FindLsRay(&f_start,&f_end)!=NULL)
				continue;	//两特征螺栓在同一个螺栓射线上，不再进行标注
			//
			rotate_dim.startPos=f_start;
			rotate_dim.endPos=f_end;
			rotate_dim.dimPos=0.5*(f_start+f_end);
			rotate_dim.dist=DISTANCE(rotate_dim.startPos,rotate_dim.endPos);
			rotate_dim.angle=Cal2dLineAng(0,0,rotate_dim.endPos.x-rotate_dim.startPos.x,rotate_dim.endPos.y-rotate_dim.startPos.y);
			rotate_dim.bDisplayDimLine=TRUE;
			rotate_dim_arr.Add(rotate_dim);
		}
	}
	if(dimStyleFlag.IsHasFlag(CPlankDrawing::DimBoltSpaceToEdgeLine))
	{	//标注轮廓特征螺栓到轮廓边距离
		//获取绘图坐标下的轮廓点
		int nVertex=pGeneralPlate->vertex_list.GetNodeNum();
		ATOM_LIST<f3dPoint> vertexArr;
		for(int i=0;i<nVertex;i++)
		{
			f3dPoint vertex=pGeneralPlate->vertex_list[i].vertex;
			coord_trans(vertex,pGeneralPlate->ucs,TRUE);
			coord_trans(vertex,new_ucs,FALSE);
			if(sys.part_map.bExportDeformedProfile)
				vertex=pGeneralPlate->GetDeformedVertex(vertex,new_ucs);
			vertex.z=0;
			vertex.pen.style=pGeneralPlate->vertex_list[i].type;
			if(pGeneralPlate->vertex_list[i].m_bRollEdge)
				vertex.pen.style=-1;
			vertexArr.append(vertex);				
		}
		//标注特征螺栓到轮廓边距离
		LSRAY_STRU* pLsRay=NULL;
		for(int i=0;i<out_ls_list.GetNodeNum();i++)
		{
			f3dPoint startPt,endPt,vertex,intersPt;
			startPt.Set(out_ls_list[i].centre.x,out_ls_list[i].centre.y,0);
			coord_trans(startPt,pGeneralPlate->ucs,TRUE);
			coord_trans(startPt,new_ucs,FALSE);
			startPt.z=0;
			int iFaceNo=out_ls_list[i].feature;
			if(out_ls_list[i].pen.style>1)
			{	//特殊功用螺栓孔
				double fDist=0,fMinDist=500; //特殊功用孔边距最大值暂取500
				for(int j=0;j<nVertex;j++)
				{
					f3dPoint f_start=vertexArr[j];
					f3dPoint f_end=vertexArr[(j+1)%nVertex];
					if(f_start.pen.style==-1||(iFaceNo!=f_start.feature&&iFaceNo!=f_end.feature))
						continue;
					f3dLine edge_line(f_start,f_end);
					SnapPerp(&intersPt,edge_line,startPt,&fDist);
					if(edge_line.PtInLine(intersPt)!=2)
						continue;
					if(f_start.pen.style>0&&fDist<fMinDist)
					{	//垂足在圆弧处,通过圆弧半径标注已处理
						vertex.Set();
						fMinDist=fDist;
					}
					else if(f_start.pen.style==0&&fDist<fMinDist)
					{	//垂足在线段端内,取该垂点进行标注
						vertex=intersPt;
						fMinDist=fDist;
					}
				}
			}
			else if((pLsRay=FindLsRay(&startPt,NULL)))
			{	//螺栓射线的螺栓孔
				double fDist=0,fMinDist=100; //连接螺栓边距最大值暂取100
				f3dPoint ray_vec=pLsRay->rayResideVec;
				if(ray_vec.IsZero()&&pLsRay->ls_arr.GetNodeNum()>1)
					ray_vec=(pLsRay->ls_arr[1]-pLsRay->ls_arr[0]).normalized();
				if(startPt.IsEqual(pLsRay->ls_arr[0],EPS2))
					ray_vec*=-1;
				endPt=startPt+ray_vec*300;
				for(int j=0;j<nVertex;j++)
				{
					if(vertexArr[j].pen.style!=0)
						continue;
					if(Int3dll(vertexArr[j],vertexArr[(j+1)%nVertex],startPt,endPt,intersPt)!=1)
						continue;
					fDist=DISTANCE(startPt,intersPt);
					if(fDist<fMinDist)
					{
						fMinDist=fDist;
						vertex=intersPt;
					}		
				}
			}
			if(!vertex.IsZero())
			{
				rotate_dim.startPos=startPt;
				rotate_dim.endPos=vertex;
				rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
				rotate_dim.dist=DISTANCE(rotate_dim.startPos,rotate_dim.endPos);
				rotate_dim.angle=Cal2dLineAng(0,0,rotate_dim.endPos.x-rotate_dim.startPos.x,rotate_dim.endPos.y-rotate_dim.startPos.y);
				rotate_dim.bDisplayDimLine=TRUE;
				rotate_dim_arr.Add(rotate_dim);
			}
		}
		//处理基准螺栓射线到平行轮廓边的距离
		f3dPoint datum_vec;
		ATOM_LIST<f3dPoint> datumLsRayPtArr;
		for(int i=0;i<ls_ray_arr.GetSize();i++)
		{
			if(!ls_ray_arr[i].bDatumLsRay)
				continue;
			if(datum_vec.IsZero())
				datum_vec=ls_ray_arr[i].rayResideVec;
			datumLsRayPtArr.append(ls_ray_arr[i].ls_arr[0]);
		}
		for(int i=0;i<nVertex;i++)
		{
			f3dLine edge_line(vertexArr[i],vertexArr[(i+1)%nVertex]);
			f3dPoint line_vec=(edge_line.endPt-edge_line.startPt).normalized();
			if(vertexArr[i].pen.style==0&&fabs(line_vec*datum_vec)>0.9999)
			{	//与基准螺栓射线平行的轮廓边
				double fMinDist=100,fDist=0;
				f3dPoint vertex,intersPt;
				int iSel=-1;
				for(int j=0;j<datumLsRayPtArr.GetNodeNum();j++)
				{
					SnapPerp(&intersPt,edge_line,datumLsRayPtArr[j],&fDist);
					if(edge_line.PtInLine(intersPt)!=2)
						continue;
					if(fDist<fMinDist)
					{
						iSel=j;
						fMinDist=fDist;
						vertex=intersPt;
					}
				}
				if(!vertex.IsZero())
				{
					rotate_dim.startPos=datumLsRayPtArr[iSel];
					rotate_dim.endPos=vertex;
					rotate_dim.dimPos=0.5*(rotate_dim.startPos+rotate_dim.endPos);
					rotate_dim.dist=DISTANCE(rotate_dim.startPos,rotate_dim.endPos);
					rotate_dim.angle=Cal2dLineAng(0,0,rotate_dim.endPos.x-rotate_dim.startPos.x,rotate_dim.endPos.y-rotate_dim.startPos.y);
					rotate_dim.bDisplayDimLine=TRUE;
					rotate_dim_arr.Add(rotate_dim);
				}
				break;
			}
		}
	}
}
LSRAY_STRU* CPlankDrawing::FindLsRay(f3dPoint* lsPos1,f3dPoint* lsPos2)
{
	for(int i=0;i<ls_ray_arr.GetSize();i++)
	{
		LSRAY_STRU* pLsRay=&ls_ray_arr[i];
		BOOL bFind1=FALSE,bFind2=FALSE;
		for(int j=0;j<pLsRay->ls_arr.GetNodeNum();j++)
		{
			f3dPoint pt=pLsRay->ls_arr[j];
			if(pt.IsEqual(*lsPos1,EPS2))
				bFind1=TRUE;
			if(lsPos2==NULL||pt.IsEqual(*lsPos2,EPS2))
				bFind2=TRUE;
			if(bFind1&&bFind2)
				return pLsRay;
		}
	}
	return NULL;
}
void CPlankDrawing::AddHuoQuPrjMap(f3dLine huoqu_line,HUOQU_MARK mark,char cMark/*='A'*/)
{
	f2dRect rect=GetDrawRegion(1);
	f3dPoint ptArr[4];
	ptArr[0].Set(rect.topLeft.x,rect.topLeft.y);
	ptArr[1].Set(rect.bottomRight.x,rect.topLeft.y);
	ptArr[2].Set(rect.bottomRight.x,rect.bottomRight.y);
	ptArr[3].Set(rect.topLeft.x,rect.bottomRight.y);
	//计算火曲线与钢板区域的交点
	TXTDIM_STRU txt_dim;
	txt_dim.txtHeight=1.5*sys.fonts.plate.fDimTextSize;
	double fDist=30,fEdgeLen=80;
	f3dPoint prj_vec=(huoqu_line.endPt-huoqu_line.startPt).normalized();
	f3dPoint offset_vec(-prj_vec.y,prj_vec.x,0);
	f3dPoint ptS=huoqu_line.startPt-prj_vec*10000;
	f3dPoint ptE=huoqu_line.endPt+prj_vec*10000;
	f3dPoint intersPt;
	for(int i=0;i<4;i++)
	{
		if(Int3dll(f3dLine(ptS,ptE),f3dLine(ptArr[i],ptArr[(i+1)%4]),intersPt)!=1)
			continue;
		if(DISTANCE(intersPt,huoqu_line.startPt)<DISTANCE(intersPt,huoqu_line.endPt))
			huoqu_line.startPt=intersPt;
		else
			huoqu_line.endPt=intersPt;
	}
	//绘制剖视图符
	f3dPoint origin=huoqu_line.startPt-prj_vec*fEdgeLen*0.5;
	f3dLine line;
	line.pen.style=PS_OTHERLINE;
	//上侧
	line.startPt=origin+offset_vec*fDist*2;
	line.endPt=line.startPt-offset_vec*fDist;
	edge_arr.Add(LineToArc(line));
	line.endPt=line.startPt+prj_vec*fDist;
	edge_arr.Add(LineToArc(line));
	//方向箭头
	line.startPt=line.endPt;
	line.endPt=line.startPt-prj_vec*fDist*0.25+offset_vec*fDist*0.25;
	edge_arr.Add(LineToArc(line));
	line.endPt=line.startPt-prj_vec*fDist*0.25-offset_vec*fDist*0.25;
	edge_arr.Add(LineToArc(line));
	//剖视符号
	txt_dim.horzAlignMode=AcDb::kTextCenter;
	txt_dim.vertAlignMode=AcDb::kTextTop;
	txt_dim.dimPos=origin+offset_vec*(fDist*2-2*txt_dim.txtHeight)+prj_vec*2*txt_dim.txtHeight;
	txt_dim.dimText.Printf("%C",cMark);
	txt_dim.dimVec=prj_vec;
	text_dim_arr.Add(txt_dim);
	//下侧
	line.startPt=origin-offset_vec*fDist*2;
	line.endPt=line.startPt+offset_vec*fDist;
	edge_arr.Add(LineToArc(line));
	line.endPt=line.startPt+prj_vec*fDist;
	edge_arr.Add(LineToArc(line));
	//方向箭头
	line.startPt=line.endPt;
	line.endPt=line.startPt-prj_vec*fDist*0.25+offset_vec*fDist*0.25;
	edge_arr.Add(LineToArc(line));
	line.endPt=line.startPt-prj_vec*fDist*0.25-offset_vec*fDist*0.25;
	edge_arr.Add(LineToArc(line));
	//剖视符号
	txt_dim.horzAlignMode=AcDb::kTextCenter;
	txt_dim.vertAlignMode=AcDb::kTextBottom;
	txt_dim.dimPos=origin-offset_vec*(fDist*2-2*txt_dim.txtHeight)+prj_vec*2*txt_dim.txtHeight;
	txt_dim.dimText.Printf("%C",cMark);
	txt_dim.dimVec=prj_vec;
	text_dim_arr.Add(txt_dim);
	//剖视符号
	//绘制火曲板投影示意图
	f3dPoint huoqo_ptS=mark.line_arr[4].startPt;
	f3dPoint huoqu_ptE=mark.line_arr[4].endPt;
	BOOL bNeiQu=(mark.plane_angle>0)?TRUE:FALSE;
	double fPlaneAngle=fabs(mark.plane_angle)*DEGTORAD_COEF;
	if(fPlaneAngle>150)
	{
		fPlaneAngle=150*RADTODEG_COEF;
		fPlaneAngle*=bNeiQu?1:-1;
		huoqu_ptE=huoqo_ptS+mark.offset_vec*10;
		rotate_point_around_axis(huoqu_ptE,fPlaneAngle,f3dLine(huoqo_ptS,huoqo_ptS+f3dPoint(0,0,1)*10));
	}
	f3dPoint pt,basePt1,basePt2,quPt1,quPt2,base_vec,qu_vec,vec;
	base_vec=mark.offset_vec;
	qu_vec=(huoqu_ptE-huoqo_ptS).normalized();
	//
	origin=huoqu_line.endPt+prj_vec*fEdgeLen;
	line.startPt=origin;
	line.endPt=basePt1=origin+base_vec*fEdgeLen;
	edge_arr.Add(LineToArc(line));
	line.endPt=quPt1=line.startPt+qu_vec*fEdgeLen;
	edge_arr.Add(LineToArc(line));
	//
	vec=base_vec+qu_vec;
	double cosa=fabs(vec*prj_vec);
	double thick=16;
	line.startPt=pt=origin+vec*thick;
	line.endPt=basePt2=line.startPt+base_vec*fEdgeLen;;
	edge_arr.Add(LineToArc(line));
	line.endPt=quPt2=line.startPt+qu_vec*fEdgeLen;
	edge_arr.Add(LineToArc(line));
	//
	line.startPt=basePt1;
	line.endPt=basePt2;
	edge_arr.Add(LineToArc(line));
	line.startPt=quPt1;
	line.endPt=quPt2;
	edge_arr.Add(LineToArc(line));
	//添加角度标注
	ANGULARDIM_STRU angular_dim; 
	angular_dim.vertex=pt;
	angular_dim.startPos=0.5*(pt+basePt2);
	angular_dim.endPos=0.5*(pt+quPt2);
	angular_dim.dimPos=origin+vec*(2*thick/cosa);
	angular_dim.sDimText.Printf("%.1f°",fabs(mark.plane_angle)*DEGTORAD_COEF);
	angular_dim_arr.Add(angular_dim);
	//添加剖视图符
	txt_dim.horzAlignMode=AcDb::kTextCenter;
	txt_dim.vertAlignMode=AcDb::kTextTop;
	txt_dim.dimPos=origin+vec*thick*0.5;
	txt_dim.dimPos.y-=(fEdgeLen+2*txt_dim.txtHeight);
	txt_dim.dimText.Printf("%C-%C",cMark,cMark);
	txt_dim.dimVec=f3dPoint(1,0,0);
	text_dim_arr.Add(txt_dim);
}

f2dRect CPlankDrawing::GetDrawRegion(double PART_MAP_SCALE)
{
	int i,n;
	f3dArcLine arcline;
	TXTDIM_STRU txt_dim;
	ROTATEDIM_STRU rotate_dim;
	ANGULARDIM_STRU angular_dim;
	SCOPE_STRU scope;
	n = GetEdgeCount();
	for(i=0;i<n;i++)
	{
		GetEdgeAt(i,arcline);
		arcline.ZoomScale(PART_MAP_SCALE);
		if(arcline.SectorAngle()>EPS&&arcline.Radius()>EPS)
		{	//圆弧(将圆弧段简化成若干个直线段)  wxc-2018.3.14
			int nSlices=CalArcResolution(arcline.Radius(),arcline.SectorAngle(),1.0,5.0,18);
			double angle=arcline.SectorAngle()/nSlices;
			for(int i=1;i<nSlices;i++)
			{
				f3dPoint pos=arcline.PositionInAngle(angle*i);
				scope.VerifyVertex(pos);
			}
		}
		scope.VerifyVertex(arcline.Start());
		scope.VerifyVertex(arcline.End());
	}
	n = GetRotateDimCount();
	for(i=0;i<n;i++)
	{
		GetRotateDimAt(i,rotate_dim);
		rotate_dim.startPos*=PART_MAP_SCALE;
		rotate_dim.endPos*=PART_MAP_SCALE;
		rotate_dim.dimPos*=PART_MAP_SCALE;
		scope.VerifyVertex(rotate_dim.startPos);
		scope.VerifyVertex(rotate_dim.endPos);
		scope.VerifyVertex(rotate_dim.dimPos);
	}
	n = GetAngularDimCount();
	for(i=0;i<n;i++)
	{
		GetAngularDimAt(i,angular_dim);
		angular_dim.vertex*=PART_MAP_SCALE;
		angular_dim.startPos*=PART_MAP_SCALE;
		angular_dim.endPos*=PART_MAP_SCALE;
		angular_dim.dimPos*=PART_MAP_SCALE;
		scope.VerifyVertex(angular_dim.vertex);
		scope.VerifyVertex(angular_dim.startPos);
		scope.VerifyVertex(angular_dim.endPos);
		scope.VerifyVertex(angular_dim.dimPos);
	}
	n = GetTextDimCount();
	for(i=0;i<n;i++)
	{
		GetTextDimAt(i,txt_dim);
		txt_dim.dimPos*=PART_MAP_SCALE;
		scope.VerifyVertex(txt_dim.dimPos);
	}
	for(i=0;i<2;i++)
	{
		if(huoqu_mark[i].line_arr[0].startPt!=huoqu_mark[i].line_arr[0].endPt)
		{
			for(int j=0;j<5;j++)
			{
				huoqu_mark[i].line_arr[j].startPt*=PART_MAP_SCALE;
				huoqu_mark[i].line_arr[j].endPt*=PART_MAP_SCALE;
				scope.VerifyVertex(huoqu_mark[i].line_arr[j].startPt);
				scope.VerifyVertex(huoqu_mark[i].line_arr[j].endPt);
			}
		}
	}
	if(sys.part_map.plate.bDimCoord||sys.part_map.plate.bAdjustPlateUcs)
	{	//标注顶点坐标，螺栓射线数组中存储钢板上的杆件射线
		n = GetLsRayCount();
		for(i=0;i<n;i++)
		{
			f3dLine line;
			if(GetLsRayLineAt(i,line))
			{
				line.startPt*=PART_MAP_SCALE;
				line.endPt*=PART_MAP_SCALE;
				scope.VerifyVertex(line.startPt);
				scope.VerifyVertex(line.endPt);
			}
		}
	}
	f2dRect rect;
	rect.topLeft.x = scope.fMinX;
	rect.topLeft.y = scope.fMaxY;
	rect.bottomRight.x = scope.fMaxX;
	rect.bottomRight.y = scope.fMinY;
	return rect;
}
/*
f2dRect CPlankDrawing::GetDrawRegion(double PART_MAP_SCALE,UCS_STRU tem_ucs)
{
	int i,n;
	f3dPoint startPt,endPt,pt,vertex;
	f3dArcLine arcline;
	TXTDIM_STRU txt_dim;
	ROTATEDIM_STRU rotate_dim;
	ANGULARDIM_STRU angular_dim;
	SCOPE_STRU scope;
	n = GetEdgeCount();
	for(i=0;i<n;i++)
	{
		GetEdgeAt(i,arcline);
		arcline.ZoomScale(PART_MAP_SCALE);
		startPt=arcline.Start();
		endPt=arcline.End();
		coord_trans(startPt,tem_ucs,FALSE);
		coord_trans(endPt,tem_ucs,FALSE);
		scope.VerifyVertex(startPt);
		scope.VerifyVertex(endPt);
		if(arcline.SectorAngle()>EPS&&arcline.Radius()>EPS)
		{	//圆弧
			f3dPoint center=arcline.Center();
			coord_trans(center,tem_ucs,FALSE);
			double radius=arcline.Radius();
			scope.VerifyVertex(f3dPoint(center.x+radius,center.y));
			scope.VerifyVertex(f3dPoint(center.x,center.y+radius));
			scope.VerifyVertex(f3dPoint(center.x-radius,center.y));
			scope.VerifyVertex(f3dPoint(center.x,center.y-radius));
		}
	}
	n = GetRotateDimCount();
	for(i=0;i<n;i++)
	{
		GetRotateDimAt(i,rotate_dim);
		rotate_dim.startPos*=PART_MAP_SCALE;
		rotate_dim.endPos*=PART_MAP_SCALE;
		rotate_dim.dimPos*=PART_MAP_SCALE;
		startPt=rotate_dim.startPos;
		endPt=rotate_dim.endPos;
		pt=rotate_dim.dimPos;
		coord_trans(startPt,tem_ucs,FALSE);
		coord_trans(endPt,tem_ucs,FALSE);
		coord_trans(pt,tem_ucs,FALSE);
		scope.VerifyVertex(startPt);
		scope.VerifyVertex(endPt);
		scope.VerifyVertex(pt);
	}
	n = GetAngularDimCount();
	for(i=0;i<n;i++)
	{
		GetAngularDimAt(i,angular_dim);
		angular_dim.vertex*=PART_MAP_SCALE;
		angular_dim.startPos*=PART_MAP_SCALE;
		angular_dim.endPos*=PART_MAP_SCALE;
		angular_dim.dimPos*=PART_MAP_SCALE;
		vertex=angular_dim.vertex;
		startPt=angular_dim.startPos;
		endPt=angular_dim.endPos;
		pt=angular_dim.dimPos;
		coord_trans(vertex,tem_ucs,FALSE);
		coord_trans(startPt,tem_ucs,FALSE);
		coord_trans(endPt,tem_ucs,FALSE);
		coord_trans(pt,tem_ucs,FALSE);
		scope.VerifyVertex(vertex);
		scope.VerifyVertex(startPt);
		scope.VerifyVertex(endPt);
		scope.VerifyVertex(pt);
	}
	n = GetTextDimCount();
	for(i=0;i<n;i++)
	{
		GetTextDimAt(i,txt_dim);
		txt_dim.dimPos*=PART_MAP_SCALE;
		pt=txt_dim.dimPos;
		coord_trans(pt,tem_ucs,FALSE);
		scope.VerifyVertex(pt);
	}
	for(i=0;i<2;i++)
	{
		if(huoqu_mark[i].line_arr[0].startPt!=huoqu_mark[i].line_arr[0].endPt)
		{
			for(int j=0;j<5;j++)
			{
				huoqu_mark[i].line_arr[j].startPt*=PART_MAP_SCALE;
				huoqu_mark[i].line_arr[j].endPt*=PART_MAP_SCALE;
				startPt=huoqu_mark[i].line_arr[j].startPt;
				endPt=huoqu_mark[i].line_arr[j].endPt;
				coord_trans(startPt,tem_ucs,FALSE);
				coord_trans(endPt,tem_ucs,FALSE);
				scope.VerifyVertex(startPt);
				scope.VerifyVertex(endPt);

			}
		}
	}
	if(sys.part_map.plate.bDimCoord||sys.part_map.plate.bAdjustPlateUcs)
	{	//标注顶点坐标，螺栓射线数组中存储钢板上的杆件射线
		n = GetLsRayCount();
		for(i=0;i<n;i++)
		{
			f3dLine line;
			if(GetLsRayLineAt(i,line))
			{
				line.startPt*=PART_MAP_SCALE;
				line.endPt*=PART_MAP_SCALE;
				startPt=line.startPt;
				endPt=line.endPt;
				coord_trans(startPt,tem_ucs,FALSE);
				coord_trans(endPt,tem_ucs,FALSE);
				scope.VerifyVertex(startPt);
				scope.VerifyVertex(endPt);
			}
		}
	}
	f2dRect rect;
	rect.topLeft.x = scope.fMinX;
	rect.topLeft.y = scope.fMaxY;
	rect.bottomRight.x = scope.fMaxX;
	rect.bottomRight.y = scope.fMinY;
	return rect;
}
*/
//标注顶点或螺栓孔坐标值
void CPlankDrawing::DimCoordValue(CLDSPart *pPart,f3dPoint pos)
{
	if(!sys.part_map.plate.bDimCoord||pPart==NULL)
		return;
	if(pPart->GetClassTypeId()!=CLS_PLATE&&pPart->GetClassTypeId()!=CLS_PARAMPLATE)
		return;
	double line_len=10;
	f3dLine line;
	TXTDIM_STRU dim_coord_x,dim_coord_y;
	//将板的几何中心转换到新坐标系下
	f3dPoint center=geom_center;
	coord_trans(center,new_ucs,FALSE);
	f3dPoint vec=pos-center;
	normalize(vec); //用来判断坐标延伸线的方向
	//绘制坐标延伸线---X轴方向
	line.pen.style=PS_SOLID;
	line.startPt=pos;
	if(vec.x>0)
		line.endPt.Set(pos.x+10,pos.y);
	else
		line.endPt.Set(pos.x-10,pos.y);
	//edge_arr.Add(LineToArc(line));
	m_xEntContext.AppendLine(line);
	//标注相对坐标
	dim_coord_x.dimPos=line.endPt;
	//暂时使用 角钢孔位纵向绝对尺寸标注精度 来控制板坐标的精度
	if(sys.general_set.iDimPrecision==0)	//精确到1mm
		sprintf(dim_coord_x.dimText,"%.0f",pos.x);
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
	{
		pos.x=ftol(pos.x*2)/2.0;
		if(fabs(pos.x-ftoi(pos.x))>=0.05)
			sprintf(dim_coord_x.dimText,"%.1f",pos.x);
		else
			sprintf(dim_coord_x.dimText,"%.0f",pos.x);
	}
	else if(sys.general_set.iDimPrecision==2)//精确到0.1mm
	{
		if(fabs(pos.x-ftoi(pos.x))>=0.05)
			sprintf(dim_coord_x.dimText,"%.1f",pos.x);
		else
			sprintf(dim_coord_x.dimText,"%.0f",pos.x);
	}
	else
		sprintf(dim_coord_x.dimText,"%.2f",pos.x);
	dim_coord_x.dimVec=line.endPt-line.startPt;
	normalize(dim_coord_x.dimVec);
	//绘制坐标延伸线---Y轴方向
	line.pen.style=PS_SOLID;
	line.startPt=pos;
	if(vec.y>0)
		line.endPt.Set(pos.x,pos.y+10);
	else
		line.endPt.Set(pos.x,pos.y-10);
	//edge_arr.Add(LineToArc(line));
	m_xEntContext.AppendLine(line);
	//标注相对坐标
	dim_coord_y.dimPos=line.endPt;
	//暂时使用 角钢孔位纵向绝对尺寸标注精度 来控制板坐标的精度
	if(sys.general_set.iDimPrecision==0)	//精确到1mm
		sprintf(dim_coord_y.dimText,"%.0f",pos.y);
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
	{
		pos.y=ftol(pos.y*2)/2.0;
		if(fabs(pos.y-ftoi(pos.y))>=0.05)
			sprintf(dim_coord_y.dimText,"%.1f",pos.y);
		else
			sprintf(dim_coord_y.dimText,"%.0f",pos.y);
	}
	else if(sys.general_set.iDimPrecision==2)//精确到0.1mm
	{
		if(fabs(pos.y-ftoi(pos.y))>=0.05)
			sprintf(dim_coord_y.dimText,"%.1f",pos.y);
		else
			sprintf(dim_coord_y.dimText,"%.0f",pos.y);
	}
	else
		sprintf(dim_coord_y.dimText,"%.2f",pos.y);
	dim_coord_y.dimVec=line.endPt-line.startPt;
	normalize(dim_coord_y.dimVec);
	//根据坐标射线的方向确定坐标文字标注的对齐方式
	dim_coord_x.horzAlignMode=AcDb::kTextLeft;
	dim_coord_x.vertAlignMode=AcDb::kTextVertMid;
	dim_coord_y.horzAlignMode=AcDb::kTextLeft;
	dim_coord_y.vertAlignMode=AcDb::kTextVertMid;
	dim_coord_x.txtHeight=dim_coord_y.txtHeight=2.0;
	text_dim_arr.Add(dim_coord_x);
	text_dim_arr.Add(dim_coord_y);			
}

//标注圆弧上nCount个坐标值
void CPlankDrawing::DimArcLineCoordValue(CLDSPart *pPart,f3dArcLine arcline)
{
	if(!sys.part_map.plate.bDimCoord||pPart==NULL)
		return;
	if(arcline.SectorAngle()==0)
		return;		
	int i,nCount=8;
	f3dPoint startPt=arcline.Start();
	//计算圆弧等分数  最小8° 该变量应由用户设置
	nCount=(int)(arcline.SectorAngle()/(RADTODEG_COEF*8));
	double angle=arcline.SectorAngle()*(1.0/nCount);
	if(arcline.WorkNorm()==arcline.ColumnNorm())
	{	//圆弧
		for(i=1;i<nCount;i++)
		{
			startPt=arcline.Start();
			rotate_point_around_axis(startPt,angle*i,arcline.Center(),arcline.Center()+arcline.WorkNorm()*100);
			DimCoordValue(pPart,startPt);
		}
	}
	else	//暂时不能处理大于180度椭圆弧的坐标标注问题 值能标注左右对称的椭圆弧
	{	//椭圆弧 (x*x)/(a*a)+(y*y)/(b*b)=1
		UCS_STRU temp_ucs=new_ucs;
		temp_ucs.origin=arcline.Center();
		coord_trans(temp_ucs.origin,pPart->ucs,TRUE);
		double radiusRatio=fabs(arcline.ColumnNorm()*arcline.WorkNorm());
		double b=arcline.Radius();				//椭圆短半轴长度
		double a=arcline.Radius()/radiusRatio;	//椭圆长半轴长度
		nCount=(int)((b)/10);	//10为常数 每间隔10mm标注一个坐标点
		startPt=arcline.Start();
		coord_trans(startPt,pPart->ucs,TRUE);
		coord_trans(startPt,temp_ucs,FALSE);
		f3dPoint endPt=arcline.End();
		coord_trans(endPt,pPart->ucs,TRUE);
		coord_trans(endPt,temp_ucs,FALSE);
		int nFlag=-1;
		if(0.5*(startPt.y+endPt.y)>0)
			nFlag=1;
		//if((nFlag*startPt.y<0)||(nFlag*endPt.y<0))
		//	return;
		if(fabs(startPt.x+endPt.x)>EPS2)
			return;	//当前值标注左右对称的椭圆弧上的顶点坐标
		double offset_dist=0;
		double min_y=startPt.y,max_y=endPt.y;
		if(startPt.y>endPt.y)
		{
			min_y=endPt.y;
			max_y=startPt.y;
		}
		if(startPt.y*endPt.y>0)
		{
			if(startPt.y>0)
				max_y=b;
			else 
				min_y=-b;
		}
		for(i=0;i<nCount;i++)
		{
			f3dPoint cur_pt,mir_pt;
			cur_pt.Set(0,b*nFlag);
			offset_dist+=i*2;
			cur_pt.y-=nFlag*offset_dist;	
			cur_pt.x=sqrt(a*a-(a*a*cur_pt.y*cur_pt.y)/(b*b));
			mir_pt.Set(-cur_pt.x,cur_pt.y);
			if(cur_pt.y>max_y||cur_pt.y<min_y)
				continue;
			//if((startPt.x>0&&cur_pt.x<startPt.x)||(endPt.x>0&&cur_pt.x<endPt.x))
			{	//X在始终点范围之内
				coord_trans(cur_pt,temp_ucs,TRUE);
				coord_trans(cur_pt,pPart->ucs,FALSE);
				DimCoordValue(pPart,cur_pt);
			}
			if(i>0)
			{	//X在始终点范围之内
				//if((startPt.x<0&&mir_pt.x<startPt.x)||(endPt.x<0&&mir_pt.x<endPt.x))
				//	continue;
				coord_trans(mir_pt,temp_ucs,TRUE);
				coord_trans(mir_pt,pPart->ucs,FALSE);
				DimCoordValue(pPart,mir_pt);
			}
		}
	}
}

void CPlankDrawing::DimPlateEdgeLen(f3dLine line)
{
	if(!sys.part_map.plate.bDimPlateEdgeLen)
		return;
	f3dPoint line_vec;
	TXTDIM_STRU txt_dim;
	txt_dim.txtHeight=sys.fonts.plate.fDimTextSize;	//轮廓边长度标注字体高度独立设置
	//标注钢板边长
	txt_dim.dimPtS=line.startPt;
	txt_dim.dimPtE=line.endPt;
	txt_dim.dimPos=(line.startPt+line.endPt)*0.5;
	double len=DISTANCE(line.startPt,line.endPt);
	if(len<=0)	//边长为0时不用标注
		return;
	//暂时使用 角钢孔位纵向绝对尺寸标注精度 来控制板坐标的精度
	if(sys.general_set.iDimPrecision==0)	//精确到1mm
		sprintf(txt_dim.dimText,"%.0f",len);
	else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
	{
		len=ftol(len*2)/2.0;
		if(fabs(len-ftoi(len))>=0.05)
			sprintf(txt_dim.dimText,"%.1f",len);
		else
			sprintf(txt_dim.dimText,"%.0f",len);
	}
	else if(sys.general_set.iDimPrecision==2)//精确到0.1mm
	{
		if(fabs(len-ftoi(len))>=0.05)
			sprintf(txt_dim.dimText,"%.1f",len);
		else
			sprintf(txt_dim.dimText,"%.0f",len);
	}
	else
		sprintf(txt_dim.dimText,"%.2f",len);
	line_vec=line.endPt-line.startPt;
	normalize(line_vec);
	txt_dim.dimVec=line_vec;
	text_dim_arr.Add(txt_dim);
}
//合并共线的边长标注项
void CPlankDrawing::MergeParallelEdgeDim(int iStart)
{
	if(!sys.part_map.plate.bDimPlateEdgeLen)
		return;
	ATOM_LIST<TXTDIM_STRU> dimList1,dimList2;
	for(int i=0;i<iStart;i++)
		dimList1.append(text_dim_arr[i]);
	for(int i=iStart;i<text_dim_arr.GetSize();i++)
		dimList2.append(text_dim_arr[i]);
	TXTDIM_STRU* pPreDim=dimList2.GetTail();
	for(TXTDIM_STRU* pCurDim=dimList2.GetFirst();pCurDim;pCurDim=dimList2.GetNext())
	{
		if(pPreDim->dimVec.IsZero()||!pPreDim->dimVec.IsEqual(pCurDim->dimVec,EPS2))
		{
			pPreDim=pCurDim;
			continue;
		}
		if(pPreDim->dimPtE.IsEqual(pCurDim->dimPtS,EPS2))
		{
			pPreDim->dimPtE=pCurDim->dimPtE;
			pPreDim->dimPos=(pPreDim->dimPtS+pPreDim->dimPtE)*0.5;
			double len=DISTANCE(pPreDim->dimPtS,pPreDim->dimPtE);
			if(sys.general_set.iDimPrecision==0)	//精确到1mm
				sprintf(pPreDim->dimText,"%.0f",len);
			else if(sys.general_set.iDimPrecision==1)	//精确到0.5mm
			{
				len=ftol(len*2)/2.0;
				if(fabs(len-ftoi(len))>=0.05)
					sprintf(pPreDim->dimText,"%.1f",len);
				else
					sprintf(pPreDim->dimText,"%.0f",len);
			}
			else if(sys.general_set.iDimPrecision==2)//精确到0.1mm
			{
				if(fabs(len-ftoi(len))>=0.05)
					sprintf(pPreDim->dimText,"%.1f",len);
				else
					sprintf(pPreDim->dimText,"%.0f",len);
			}
			else
				sprintf(pPreDim->dimText,"%.2f",len);
			dimList2.DeleteCursor();
		}
		pPreDim=pCurDim;
	}
	dimList2.Clean();
	//
	text_dim_arr.RemoveAll();
	for(int i=0;i<dimList1.GetNodeNum();i++)
		text_dim_arr.Add(dimList1[i]);
	for(int i=0;i<dimList2.GetNodeNum();i++)
		text_dim_arr.Add(dimList2[i]);
}
//主要针对LMA基准构件为钢管的钢板
void CPlankDrawing::AnalizePoleRay(CLDSPlate *pPlate)
{	
	if(pPlate==NULL||new_ucs.axis_x.IsZero()||new_ucs.axis_y.IsZero()||new_ucs.axis_z.IsZero())
		return;
	if(pPlate->vertex_list.GetNodeNum()<3)
		return;
	CLDSLineTube *pDatumLineTube=(CLDSLineTube*)pPlate->BelongModel()->FromPartHandle(pPlate->m_hPartWeldParent,CLS_LINETUBE);
	if(pDatumLineTube==NULL)
		return;
	ls_ray_arr.RemoveAll();	//清空杆件射线数组
	int iCount=0;
	f3dLine long_edge;		
	f3dPoint inter_pos[5];	//连板中心线与钢板轮廓边的交点
	f3dPoint startPt,endPt;
	int i=0, j=0, iRayNo=0;
	CDesignLjPartPara *pLjPart=NULL;
	f3dLine datum_line(pDatumLineTube->Start(),pDatumLineTube->End());
	f3dPoint datum_line_vec=pDatumLineTube->End()-pDatumLineTube->Start();
	normalize(datum_line_vec);
	vector_trans(datum_line_vec,new_ucs,FALSE);	//转换到钢板坐标系下
	f3dPoint ray_pole_vec[10];	//记录射线杆件的延伸方向 如果其中两根射线杆件方向相反则过中心点绘制垂直与该射线的直线
	for(pLjPart=pPlate->designInfo.partList.GetFirst();pLjPart;pLjPart=pPlate->designInfo.partList.GetNext())
	{	
		LSRAY_STRU pole_ray;
		if(pLjPart->hPart==pPlate->designInfo.m_hBasePart)
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pPlate->BelongModel()->FromPartHandle(pLjPart->hPart,CLS_LINEANGLE,CLS_LINETUBE);
		if(pLinePart==NULL)
			continue;
		//确定射线杆件中心线所在直线
		f3dLine line(pLinePart->Start(),pLinePart->End());
		f3dPoint line_vec=pLinePart->End()-pLinePart->Start();
		normalize(line_vec);
		BOOL bDrawDatumLine=FALSE;	//是否需要绘制基准线
		for(j=0;j<iRayNo;j++)
		{
			if(fabs(ray_pole_vec[j]*line_vec)>EPS_COS)
			{
				bDrawDatumLine=TRUE;
				break;
			}
		}
		ray_pole_vec[iRayNo]=line_vec;
		//查找角钢上Z坐标最大或最小的螺栓(角钢坐标系下)
		double max_z=-10000000,min_z=1000000000;
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			CLsRef *pLsRef=NULL;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
			pLineAngle->getUCS(pLineAngle->ucs);
			for(pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
			{
				if(pPlate->FindLsByHandle(pLsRef->GetLsPtr()->handle))
				{
					f3dPoint ls_pos=(*pLsRef)->ucs.origin;
					coord_trans(ls_pos,pLineAngle->ucs,FALSE);
					if(ls_pos.z>max_z)
						max_z=ls_pos.z;
					if(ls_pos.z<min_z)
						min_z=ls_pos.z;
				}
			}
		}
		if(pLjPart->start0_end1==0)
		{
			startPt=pLinePart->Start();
			endPt=pLinePart->Start()-line_vec*pLinePart->startOdd();
		}
		else 
		{
			startPt=pLinePart->End();
			endPt=pLinePart->End()+line_vec*pLinePart->endOdd();
		}
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		{
			JGZJ jgzj;
			f3dPoint wing_vec;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pLinePart;
			getjgzj(jgzj,pLineAngle->GetWidth());
			if(pLjPart->angle.cur_wing_x0_y1==0)
			{
				if(pLineAngle->m_bEnableTeG)
					jgzj = pLineAngle->xWingXZhunJu;
				wing_vec = pLineAngle->GetWingVecX();
			}
			else
			{
				if(pLineAngle->m_bEnableTeG)
					jgzj = pLineAngle->xWingYZhunJu;
				wing_vec = pLineAngle->GetWingVecY();
			}
			Add_Pnt(startPt,startPt,wing_vec*jgzj.g);
			Add_Pnt(endPt,endPt,wing_vec*jgzj.g);
			double dist=max_z-min_z+80;
			if(dist<pLineAngle->GetLength()&&dist>0)
			{	//调整角钢射线长度
				if(pLjPart->start0_end1==0)
					endPt=endPt+line_vec*dist;
				else if(pLjPart->start0_end1==1)
					endPt=endPt-line_vec*dist;
			}
		}
		coord_trans(startPt,new_ucs,FALSE);
		coord_trans(endPt,new_ucs,FALSE);
		startPt.z=endPt.z=0;
		//将射线方向转换到当前板坐标下
		vector_trans(line_vec,new_ucs,FALSE);
		//添加基准线射线
		if(bDrawDatumLine)
		{
			f3dPoint ray_vec(-line_vec.y,line_vec.x,0);
			double offset_dist=DISTANCE(startPt,endPt);
			pole_ray.iRayNo = iRayNo;
			ls_ray_arr.Add(pole_ray);
			i = ls_ray_arr.GetUpperBound();
			ls_ray_arr[i].rayOrg=startPt+ray_vec*offset_dist;
			f3dPoint tem_pos=startPt-ray_vec*offset_dist;
			ls_ray_arr[i].ls_arr.append(tem_pos);
			DimCoordValue(pPlate,startPt);
		}
		BOOL bTransPlate=FALSE;
		//贯通整连板
		SCOPE_STRU scope;
		PROFILE_VERTEX *pVertex=NULL,*pPrevVertex=pPlate->vertex_list.GetTail();
		for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			scope.VerifyVertex(pVertex->vertex);
		if(scope.IsIncludePoint(f3dPoint(0,0,0)))
			bTransPlate=TRUE;
		//添加连接板中心线
		if(iRayNo==0&&!bTransPlate)
		{	
			iCount=0;	//初始化
			f3dPoint center_line_vec(-datum_line_vec.y,datum_line_vec.x,0);
			double cosa=line_vec*center_line_vec;
			if(pLjPart->start0_end1==1)	//终端连接
				cosa*=-1.0;	
			if(cosa<EPS)
				center_line_vec*=-1.0;
			f3dPoint center_line_end=startPt+center_line_vec*pDatumLineTube->GetDiameter();
			pole_ray.iRayNo = -1;
			ls_ray_arr.Add(pole_ray);
			i = ls_ray_arr.GetUpperBound();
			ls_ray_arr[i].rayOrg=startPt;
			ls_ray_arr[i].ls_arr.append(center_line_end);
			//连板中心线与钢板轮廓边的交点
			f3dLine center_line(startPt,center_line_end);
			for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
			{
				if(pPrevVertex)
				{
					f3dLine cur_line(pPrevVertex->vertex,pVertex->vertex);
					coord_trans(cur_line.startPt,pPlate->ucs,TRUE);
					coord_trans(cur_line.endPt,pPlate->ucs,TRUE);
					coord_trans(cur_line.startPt,new_ucs,FALSE);
					coord_trans(cur_line.endPt,new_ucs,FALSE);
					cur_line.startPt.z=cur_line.endPt.z=0;
					if(Int3dll(center_line,cur_line,inter_pos[iCount])>0)
					{
						long_edge=cur_line;
						iCount++;
					}
				}
				pPrevVertex=pVertex;
			}
			if(iCount==1)
				DimCoordValue(pPlate,inter_pos[0]);
		}
		//标注射线角度
		if(iCount>0)
		{
			f3dPoint inters1;
			ANGULARDIM_STRU angular_dim;
			if(Int3dll(startPt,endPt,long_edge.startPt,long_edge.endPt,inters1)>0)
				angular_dim.vertex=inters1;
			f3dPoint verfiy_vec=inters1-inter_pos[0];
			normalize(verfiy_vec);
			angular_dim.startPos=inters1+verfiy_vec*80;
			if(pLjPart->start0_end1==0)
				angular_dim.endPos=inters1+line_vec*80;
			else 
				angular_dim.endPos=inters1-line_vec*80;
			angular_dim.dimPos=0.5*(angular_dim.startPos+angular_dim.endPos);
			angular_dim_arr.Add(angular_dim);
		}
		//添加杆件射线
		pole_ray.iRayNo = iRayNo;
		ls_ray_arr.Add(pole_ray);
		i = ls_ray_arr.GetUpperBound();
		ls_ray_arr[i].rayOrg=startPt;
		ls_ray_arr[i].ls_arr.append(endPt);
		iRayNo++;
	}
}

//查找钢板文字说明标注位置
BOOL CPlankDrawing::FindRectValidPos(f3dPoint &dimPos,CLDSPart *pPlate,double fWidth,double fHeight,double zoomCoef/*=1.0*/)
{	
	if(pPlate->GetClassTypeId()!=CLS_PLATE&&pPlate->GetClassTypeId()!=CLS_PARAMPLATE)
		return FALSE;
	int i=0;
	SCOPE_STRU scopePlate;
	CRgn rgnPlate;
	CPoint ptArr[1000];
	for(i=0;i<GetEdgeCount();i++)
	{
		f3dArcLine arcLine;
		GetEdgeAt(i,arcLine);
		arcLine.ZoomScale(zoomCoef);
		if(arcLine.SectorAngle()>EPS&&arcLine.Radius()>EPS)
		{	//圆弧
			GEPOINT mid_pos=0.5*(arcLine.Start()+arcLine.End());
			GEPOINT mid_vec=mid_pos-arcLine.Center();
			normalize(mid_vec);
			mid_pos=arcLine.Center()+mid_vec*arcLine.Radius();
			scopePlate.VerifyVertex(mid_pos);
		}
		scopePlate.VerifyVertex(arcLine.Start());
		scopePlate.VerifyVertex(arcLine.End());
		ptArr[2*i].x=ftoi(arcLine.Start().x);
		ptArr[2*i].y=ftoi(arcLine.Start().y);
		ptArr[2*i+1].x=ftoi(arcLine.End().x);
		ptArr[2*i+1].y=ftoi(arcLine.End().y);
	}
	rgnPlate.CreatePolygonRgn(ptArr,GetEdgeCount()*2,ALTERNATE);
	ATOM_LIST<SCOPE_STRU> scopeList;
	for(i=0;i<GetLsCirCount();i++)
	{
		f3dCircle cir;
		GetLsCirAt(i,cir);
		cir.centre*=zoomCoef;
		double radius=0;
		if(sys.general_set.StruMapScale.iBoltSymbolStyle==1)		//图符处理
			radius=1.0;
		else if(sys.general_set.StruMapScale.iBoltSymbolStyle==2)	//比例图符处理
			radius=zoomCoef*sys.general_set.StruMapScale.fBoltSymbolFactor;
		else //if(sys.general_set.iLsApertureStyle==2)	//比例直径图符处理
			//	||sys.general_set.iLsApertureStyle==0)	//圆孔处理
			radius=(cir.radius+cir.feature/20.0+sys.general_set.fHoleAugmentInNC*0.5)*zoomCoef*sys.general_set.StruMapScale.fBoltDiaFactor;
		SCOPE_STRU scope;
		scope.VerifyVertex(f3dPoint(cir.centre.x-radius,cir.centre.y-radius));
		scope.VerifyVertex(f3dPoint(cir.centre.x-radius,cir.centre.y+radius));
		scope.VerifyVertex(f3dPoint(cir.centre.x+radius,cir.centre.y-radius));
		scope.VerifyVertex(f3dPoint(cir.centre.x+radius,cir.centre.y+radius));
		scopeList.append(scope);
	}
	double txt_size=sys.dim_map.PartNo.fPartNoTextSize;
	//根据钢板长度与宽度中较小值确定钢板上标注字高缩放系数 
	if(sys.part_map.plate.bZoomDimTextSizeByWidth)
	{	//默认宽度与字高比例为3:100 即宽度为100时 字高为3
		double plate_width=min(pPlate->GetWidth(),pPlate->GetLength())*zoomCoef;
		txt_size=fto_halfi(plate_width*sys.part_map.plate.fTextSizeZoomCoef+0.5);	//字体高度
	}
	for(i=0;i<GetTextDimCount();i++)
	{
		TXTDIM_STRU text_dim;
		GetTextDimAt(i,text_dim);
		text_dim.dimPos*=zoomCoef;
		double width=0.7*txt_size*strlen(text_dim.dimText);
		f3dPoint line_vec=text_dim.dimVec;
		normalize(line_vec);
		f3dPoint norm_vec(-line_vec.y,line_vec.x);
		SCOPE_STRU scope;
		scope.VerifyVertex(text_dim.dimPos+line_vec*0.5*width-norm_vec*txt_size);
		scope.VerifyVertex(text_dim.dimPos+line_vec*0.5*width+norm_vec*txt_size);
		scope.VerifyVertex(text_dim.dimPos-line_vec*0.5*width-norm_vec*txt_size);
		scope.VerifyVertex(text_dim.dimPos-line_vec*0.5*width+norm_vec*txt_size);
		scopeList.append(scope);
	}
	f3dPoint init_pos(scopePlate.fMinX+0.5*fWidth+3,scopePlate.fMinY+3);//3为间隙值
	dimPos=init_pos; 
	while(!rgnPlate.PtInRegion(ftoi(dimPos.x),ftoi(dimPos.y)))
	{
		init_pos.y+=1;
		dimPos=init_pos;
		if(dimPos.y>scopePlate.fMaxY)
			return FALSE;
	}
	while(1)
	{
		SCOPE_STRU scopeDim;
		f3dPoint vertex_arr[4];
		vertex_arr[0].Set(dimPos.x-0.5*fWidth,dimPos.y);
		vertex_arr[1].Set(dimPos.x+0.5*fWidth,dimPos.y);
		vertex_arr[2].Set(dimPos.x-0.5*fWidth,dimPos.y+fHeight);
		vertex_arr[3].Set(dimPos.x+0.5*fWidth,dimPos.y+fHeight);
		for(i=0;i<4;i++)
			scopeDim.VerifyVertex(vertex_arr[i]);
		while(!rgnPlate.PtInRegion(ftoi(dimPos.x),ftoi(dimPos.y)))
		{
			init_pos.y+=1;
			dimPos=init_pos;
			if(dimPos.y>scopePlate.fMaxY)
				return FALSE;
		}
		SCOPE_STRU *pScope=NULL;
		for(pScope=scopeList.GetFirst();pScope;pScope=scopeList.GetNext())
		{
			if(scopeDim.IsIntersWith(*pScope))
				break;
		}
		if(pScope==NULL)
			return TRUE;
		
		while(!rgnPlate.PtInRegion(ftoi(vertex_arr[0].x),ftoi(vertex_arr[0].y))
			||!rgnPlate.PtInRegion(ftoi(vertex_arr[1].x),ftoi(vertex_arr[1].y))
			||!rgnPlate.PtInRegion(ftoi(vertex_arr[2].x),ftoi(vertex_arr[2].y))
			||!rgnPlate.PtInRegion(ftoi(vertex_arr[3].x),ftoi(vertex_arr[3].y)))
			dimPos.x+=1;
	}
	return FALSE;
}

//设置绘制坐标系
void CPlankDrawing::SetNewUcs(CLDSPart *pPart,UCS_STRU plate_ucs)
{
	new_ucs=plate_ucs;
	m_bSpecifyUCS=TRUE;	//外部直接指定绘图坐标系 wht 10-08-13
	//计算钢板几何中心
	CLDSPlate *pPlate=NULL;
	CLDSParamPlate *pParamPlate=NULL;
	ATOM_LIST<PROFILE_VERTEX> *pVertexList=NULL;
	if(pPart->GetClassTypeId()==CLS_PLATE)
	{
		pPlate=(CLDSPlate*)pPart;
		pVertexList=&pPlate->vertex_list;
	}
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{
		pParamPlate=(CLDSParamPlate*)pPart;
		if(pParamPlate->vertex_list.GetNodeNum()<3)
			pParamPlate->DesignPlate();
		pVertexList=&pParamPlate->vertex_list;
	}
	else 
		return;
	if(pVertexList->GetNodeNum()<=0)
		return;	//顶点数为0,为非法钢板
	PROFILE_VERTEX *pVertex=NULL;
	for(pVertex=pVertexList->GetFirst();pVertex;pVertex=pVertexList->GetNext())
	{
		f3dPoint vertex=pVertex->vertex;
		coord_trans(vertex,pPart->ucs,TRUE);
		geom_center+=vertex;
	}
	if(pVertexList->GetNodeNum()>0)
		geom_center/=pVertexList->GetNodeNum();
	project_point(geom_center,pPart->ucs.origin,pPart->ucs.axis_z);
	if(pParamPlate&&pParamPlate->m_iParamType==TYPE_CIRCOVERPLATE)
	{	//环向封板
		double R=0,W=0;
		pParamPlate->GetDesignItemValue('R',&R);
		pParamPlate->GetDesignItemValue('W',&W);
		geom_center.Set(R+0.5*W,0);
		coord_trans(geom_center,pParamPlate->ucs,TRUE);
	}
}
#endif

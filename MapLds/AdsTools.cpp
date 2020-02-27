#include "stdafx.h"
#include "stdarx.h"
#include "AdsTools.h"
#include "RxTools.h"
#include "dbsymtb.h"
#include "aced.h"
#include "adslib.h"
#include "dbents.h"
#include "adesk.h"
#include "rxdefs.h"
#include "dbxrecrd.h"
#include "adsdef.h"
#include "DragEntSet.h"
#include "SysPara.h"


ads_matrix ads_identmat={
	{1.0,0.0,0.0,0.0},
	{0.0,1.0,0.0,0.0},
	{0.0,0.0,1.0,0.0},
	{0.0,0.0,0.0,1.0}
};
ads_matrix     CTM;		      // Current Transformation Matrix 
ads_point Base; 		      /* Base point for xform */
/*  ENTITEM  --  Search an entity buffer chain and return an item
		 with the specified group code.  */

struct resbuf *entitem(struct resbuf *rchain, int gcode)
{
    while ((rchain != NULL) && (rchain->restype != gcode))
	rchain = rchain->rbnext;

    return rchain;
}
/******************************************************************************/
/*.doc ads_mat_ident(internal) */
/*+
Set up an identity matrix.
-*/
/******************************************************************************/
void/*FCN*/ads_mat_ident(ads_matrix matrix)
{
    extern ads_matrix ads_identmat;
    memcpy(matrix, ads_identmat, sizeof(ads_matrix));
}

/******************************************************************************/
/*.doc ads_subvec(internal) */
/*+
Subtract two ads_points.
-*/
/******************************************************************************/
void
/*FCN*/ads_subvec(ads_point ap, ads_point bp,ads_point dp)
{
    dp[X] = ap[X] - bp[X];
    dp[Y] = ap[Y] - bp[Y];
    dp[Z] = ap[Z] - bp[Z];
}


void ads_mat_x_pt(ads_matrix mat,ads_point pin,ads_point pout)
{
    int i;
    ads_point temp;
	
    for (i = X; i <= Z; i++)
		temp[i] = mat[i][X] * pin[X] +
		mat[i][Y] * pin[Y] +
		mat[i][Z] * pin[Z] +
		mat[i][T];
    memcpy(pout, temp, sizeof(ads_point));
}

void ads_mat_x_vec(ads_matrix mat,ads_point  pin,ads_point  pout)
{
    int i;
    ads_point temp;
	
    for (i = X; i <= Z; i++)
		temp[i] = mat[i][X] * pin[X] +
		mat[i][Y] * pin[Y] +
		mat[i][Z] * pin[Z];
    memcpy(pout, temp, sizeof(ads_point));
}

void ads_mat_x_mat(ads_matrix mata,ads_matrix matb,ads_matrix matout)
{
    ads_matrix t;
    int i, j, k;
    ads_real sum;
	
    for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			sum = 0.0;
			for (k=0; k<4; k++)
				sum += mata[i][k] * matb[k][j];
			t[i][j] = sum;
		}
		memcpy(matout, t, sizeof(ads_matrix));
}

/****************************************************************************/
/*.doc sa_cross(external) */
/*+
  Cross product of two vectors.
-*/
/****************************************************************************/

//Wjh 向量v1、v2叉乘
void /*FCN*/sa_cross(ads_point v, ads_point v1,ads_point  v2){
    v[X] = v1[Y] * v2[Z] - v1[Z] * v2[Y];
    v[Y] = v1[Z] * v2[X] - v1[X] * v2[Z];
    v[Z] = v1[X] * v2[Y] - v1[Y] * v2[X];
} /*sa_cross*/


int dragsampler(ads_point pt,ads_matrix mat)
{

    ads_point	    tp;

    // generate translation matrix
    ads_subvec(pt, Base, tp);
    ads_mat_x_vec(mat, tp, tp);
    mat[X][T] = tp[X];//X轴向偏移距离
    mat[Y][T] = tp[Y];//Y轴向偏移距离
    mat[Z][T] = tp[Z];//Z轴向偏移距离

    memcpy(CTM, mat, sizeof(ads_matrix));
    return RTNORM;
}

void GreateSelectionSet(ads_name set,char *sLayer, BOOL bSelLine, BOOL bSelPoint)
{
    struct resbuf *filter;
	if(bSelLine&&bSelPoint)
		filter=acutBuildList(8,sLayer,0);
#if defined(_ARX_2007)&&!defined(_ZRX_2012)
	acedSSGet(L"X",NULL,NULL,filter,set);
#else
	acedSSGet("X",NULL,NULL,filter,set);
#endif
	acutRelRb(filter);
}
//移动图层到当前位置
//准线图层 非结构图 ＝""
int DragEntSet(ads_point base,char *prompt)
{
	ads_point result;
	Base[X] = base[X];
	Base[Y] = base[Y];
	Base[Z] = base[Z]=0;
	int nRetCode=0;
#ifdef _ARX_2007
	nRetCode=acedDragGen(DRAGSET.drag_ent_set,(ACHAR*)_bstr_t(prompt),0,dragsampler,result);
#else
	nRetCode=ads_draggen(DRAGSET.drag_ent_set,prompt,0,dragsampler,result);
#endif
	if(nRetCode==RTNORM)
		ads_xformss(DRAGSET.drag_ent_set, CTM);
	else
		result[X]=result[Y]=result[Z]=0.0;
	base[X] = result[X];
	base[Y] = result[Y];
	base[Z] = result[Z];
	//修改视图位置后需要及时更新界面,否则acedSSGet()可能不能获取正确的实体集 wht 11-06-25
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
	return  nRetCode;
}

//平移实体集
/*int TranslationEntSet(ads_point base,ads_point cur,ads_name set)
{
	Base[X] = base[X];
	Base[Y] = base[Y];
	Base[Z] = base[Z]=0;
	ads_matrix mat;
	ads_mat_ident(mat);
	dragsampler(cur,mat);
	int nRetCode=ads_xformss(set, CTM);
	//修改视图位置后需要及时更新界面,否则acedSSGet()可能不能获取正确的实体集 wht 11-06-25
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
	return nRetCode;
}*/

static resbuf verts4[4];
int JustifyInters(f3dPoint rgn_vert[4],const struct resbuf *filter)
{
	long i,ll;
	ads_name ss_name;
	//struct resbuf *plist;
	for(i=0;i<4;i++)
	{
		verts4[i].restype=5002;
		verts4[i].resval.rpoint[X] = rgn_vert[i].x;
		verts4[i].resval.rpoint[Y] = rgn_vert[i].y;
		verts4[i].resval.rpoint[Z] = rgn_vert[i].z;
		if(i<3)
			verts4[i].rbnext=&verts4[i+1];
		else
			verts4[3].rbnext=NULL;
	}
	//plist=acutBuildList(RTPOINT,ads_vert[0],RTPOINT,ads_vert[1],RTPOINT,ads_vert[2],RTPOINT,ads_vert[3],RTNONE);
	
#if defined(_ARX_2007)&&!defined(_ZRX_2012)
	if (acedSSGet(L"cp",&verts4[0],NULL,filter,ss_name)==RTNORM)
#else
	if (acedSSGet("cp",&verts4[0],NULL,filter,ss_name)==RTNORM)
#endif
	{
		acedSSLength(ss_name,&ll);
		acedSSFree(ss_name);
		return ll;
	}
	else
	{
		acedSSFree(ss_name);
		return 0;
	}
}
void ProcessHideLine(AcDbBlockTableRecord *pBlockTableRecord,fPtList& vertex_list,char *layer)
{
	int i,j,n,iRetCode=0;
	long ll;
	ads_name ss_name,ent_name;
	struct resbuf *poly=NULL,*plist=NULL,*filter;
	if(vertex_list.GetNodeNum()<=1)
		return;
	for(f3dPoint *pPt=vertex_list.GetFirst();pPt;pPt=vertex_list.GetNext())
	{	//应使用acutNewRb()函数而不是new struct resbuf新建返回值缓存，否则使用acutRelRb()释放内存时出错 wht 12-09-21
		if(plist==NULL)
			poly=plist=acutNewRb(RTPOINT);	//new struct resbuf;//acutBuildList(RTDXF0,"LINE",0);
		else
		{
			plist->rbnext=acutNewRb(RTPOINT);//new struct resbuf;
			plist=plist->rbnext;
		}
		plist->restype=RTPOINT;
		plist->resval.rpoint[X]=pPt->x;
		plist->resval.rpoint[Y]=pPt->y;
		plist->resval.rpoint[Z]=0;
	}
	//结束符
	plist->rbnext=NULL;
	//plist->rbnext->restype=RTNONE;

#if defined(_ARX_2007)&&!defined(_ZRX_2012)
	filter = acutBuildList(RTDXF0,L"lwpolyline",8,(ACHAR*)_bstr_t((char*)LayerTable::VisibleProfileLayer.layerName),0);
	int retCode=acedSSGet(L"all",poly,NULL,filter,ss_name);////NULL,NULL,NULL,NULL,ss_name);
#else
	filter = acutBuildList(8,(char*)LayerTable::VisibleProfileLayer.layerName,0);
	int retCode=acedSSGet("all",poly,NULL,filter,ss_name);////NULL,NULL,NULL,NULL,ss_name);
#endif
	acutRelRb(poly);
	acutRelRb(filter);
  	if(retCode==RTNORM)
	{
		acedSSLength(ss_name,&ll);
		f3dPoint inters,break_start,break_end;
		fPtList inters_list;
		f3dLine line;
		AcDbObjectId entId;
		fPolygonRgn rgn;
		n=vertex_list.GetNodeNum();
		f3dPoint *PtArr=new f3dPoint[n];
		if(ll>0)
		{
			for(i=0;i<n;i++)
				PtArr[i]=vertex_list[i];
			if(rgn.CreatePolygonRgn(PtArr,n))
			{	//不能构成一个合法的平面
				for(i=0;i<ll;i++)
				{
					AcDbEntity *pEnt;
					AcDbLine *pLine=NULL;
					AcDbPolyline *pPolyline=NULL;
					acedSSName(ss_name,i,ent_name);
					if(!DRAGSET.IsInDragSet(ent_name))
						continue;
					acdbGetObjectId(entId,ent_name);
					acdbOpenObject(pEnt,entId,AcDb::kForWrite);
					bool bFindLine=false;
					AcGePoint3d acad_start,acad_end;
					AcDbPolyline::SegType segType=AcDbPolyline::kLine;
					if(pEnt->isKindOf(AcDbLine::desc()))
					{
						bFindLine=true;
						pLine=(AcDbLine*)pEnt;
						acad_start=pLine->startPoint();
						acad_end=pLine->endPoint();
						Cpy_Pnt(line.startPt,acad_start);
						Cpy_Pnt(line.endPt,acad_end);
					}
					else if(pEnt->isKindOf(AcDbPolyline::desc()))
					{
						bFindLine=true;
						pPolyline=(AcDbPolyline*)pEnt;
						AcGePoint2d acad_point;
						pPolyline->getPointAt(0,acad_point);
						segType=pPolyline->segType(0);
						line.startPt.Set(acad_point.x,acad_point.y);
						pPolyline->getPointAt(1,acad_point);
						line.endPt.Set(acad_point.x,acad_point.y);
					}
					if(bFindLine)
					{
						retCode=rgn.LineInRgn(line);
						BOOL bStartInPoly,bEndInPoly;
						if(retCode==1)
						{	//直线完全在多边形区域内部时分两段绘制该直线,中间留一段空隙 wht 11-04-07
							if(pPolyline==NULL||segType!=AcDbPolyline::kArc)
							{	//圆弧时不人为分成两段处理,否则现有算法会破坏原有圆弧　wjh-2011.8.23
								f3dPoint line_vec=line.endPt-line.startPt;
								normalize(line_vec);
								double fNewLineLen=DISTANCE(line.startPt,line.endPt)*(1.0/3.0);
								f3dPoint startOffsetPt=line.startPt+line_vec*fNewLineLen;
								f3dPoint endOffsetPt=line.endPt-line_vec*fNewLineLen;
								//新生成一段直线 
								Cpy_Pnt(acad_start,line.startPt);
								Cpy_Pnt(acad_end,startOffsetPt);
								AcDbLine *pDashLine=new AcDbLine(acad_start,acad_end);
								SetCurLayer(layer);
								if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pDashLine))
									pDashLine->close();
								//调整原有直线起点坐标
								if(pLine)
									pLine->setStartPoint(AcGePoint3d(endOffsetPt.x,endOffsetPt.y,endOffsetPt.z));
								else if(pPolyline)
									pPolyline->setPointAt(0,AcGePoint2d(endOffsetPt.x,endOffsetPt.y));
								if(pPolyline)
								{
									pPolyline->setWidthsAt(0,0,0);
									pPolyline->setWidthsAt(1,0,0);
								}
							}
							SetEntLayer(pEnt,layer);
						}
						else
						{
							if(rgn.PtInRgn(line.startPt)==1)
								bStartInPoly=TRUE;
							else
								bStartInPoly=FALSE;
							if(rgn.PtInRgn(line.endPt)==1)
								bEndInPoly=TRUE;
							else
								bEndInPoly=FALSE;
							inters_list.Empty();
							for(j=0;j<n;j++)
							{
								f3dLine edge;
								edge.startPt=PtArr[j];
								edge.endPt=PtArr[(j+1)%n];
								if(Int3dll(edge,line,inters)>0)
									inters_list.append(inters,TRUE);
							}
							if(bStartInPoly)
							{
								f3dPoint vec=line.endPt-line.startPt;
								BOOL bFind=FALSE;
								double dd=0;
								for(f3dPoint *pPt=inters_list.GetFirst();pPt;pPt=inters_list.GetNext())
								{
									f3dPoint vec2=*pPt-line.startPt;
									if(dd<vec2.mod())
									{
										dd=vec2.mod();
										line.endPt=*pPt;
										bFind=TRUE;
									}
								}
								if(pPolyline&&segType==AcDbPolyline::kArc)
									bFind=FALSE;	//圆弧时不人为分成两段处理,否则现有算法会破坏原有圆弧　wjh-2011.8.23
								if(bFind)
								{
									//在多边形区域内部的线段分两段绘制,中间留一段空隙 wht 11-04-07
									f3dPoint line_vec=line.endPt-line.startPt;
									normalize(line_vec);
									double fNewLineLen=DISTANCE(line.startPt,line.endPt)*(1.0/3.0);
									f3dPoint startOffsetPt=line.startPt+line_vec*fNewLineLen;
									f3dPoint endOffsetPt=line.endPt-line_vec*fNewLineLen;
									SetCurLayer(layer);	//设置当前图层
									//生成第一段直线
									Cpy_Pnt(acad_start,line.startPt);
									Cpy_Pnt(acad_end,startOffsetPt);
									AcDbLine *pDashLine1=new AcDbLine(acad_start,acad_end);
									if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pDashLine1))
										pDashLine1->close();
									//生成第二段直线
									Cpy_Pnt(acad_start,endOffsetPt);
									Cpy_Pnt(acad_end,line.endPt);
									AcDbLine *pDashLine2=new AcDbLine(acad_start,acad_end);
									if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pDashLine2))
										pDashLine2->close();
									//调整原有直线起点坐标
									if(pLine)
										pLine->setStartPoint(AcGePoint3d(line.endPt.x,line.endPt.y,line.endPt.z));
									else if(pPolyline)
										pPolyline->setPointAt(0,AcGePoint2d(line.endPt.x,line.endPt.y));
								}
							}
							else if(bEndInPoly)
							{
								f3dPoint vec=line.startPt-line.endPt;
								BOOL bFind=FALSE;
								double dd=0;
								for(f3dPoint *pPt=inters_list.GetFirst();pPt;pPt=inters_list.GetNext())
								{
									f3dPoint vec2=*pPt-line.endPt;
									if(dd<vec2.mod())
									{
										dd=vec2.mod();
										line.startPt=*pPt;
										bFind=TRUE;
									}
								}
								if(pPolyline&&segType==AcDbPolyline::kArc)
									bFind=FALSE;	//圆弧时不人为分成两段处理,否则现有算法会破坏原有圆弧　wjh-2011.8.23
								if(bFind)
								{
									//在多边形区域内部的线段分两段绘制,中间留一段空隙 wht 11-04-07
									f3dPoint line_vec=line.endPt-line.startPt;
									normalize(line_vec);
									double fNewLineLen=DISTANCE(line.startPt,line.endPt)*(1.0/3.0);
									f3dPoint startOffsetPt=line.startPt+line_vec*fNewLineLen;
									f3dPoint endOffsetPt=line.endPt-line_vec*fNewLineLen;
									SetCurLayer(layer);	//设置当前图层
									//生成第一段直线
									Cpy_Pnt(acad_start,line.startPt);
									Cpy_Pnt(acad_end,startOffsetPt);
									AcDbLine *pDashLine1=new AcDbLine(acad_start,acad_end);
									if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pDashLine1))
										pDashLine1->close();
									//生成第二段直线
									Cpy_Pnt(acad_start,endOffsetPt);
									Cpy_Pnt(acad_end,line.endPt);
									AcDbLine *pDashLine2=new AcDbLine(acad_start,acad_end);
									if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pDashLine2))
										pDashLine2->close();
									//调整原有直线终点坐标
									if(pLine)
										pLine->setEndPoint(AcGePoint3d(line.startPt.x,line.startPt.y,line.startPt.z));
									else if(pPolyline)
										pPolyline->setPointAt(1,AcGePoint2d(line.startPt.x,line.startPt.y));
								}
							}
							else
							{
								f3dPoint vec,vec2;
								vec=line.endPt-line.startPt;
								BOOL bFindStart=FALSE,bFindEnd=FALSE;
								double dd=0;
								for(f3dPoint *pPt=inters_list.GetFirst();pPt;pPt=inters_list.GetNext())
								{
									f3dPoint vec2=*pPt-line.endPt;
									if(!bFindStart)
									{
										line.startPt=*pPt;
										bFindStart=TRUE;
									}
									else if(!bFindEnd)
									{
										line.endPt=*pPt;
										bFindEnd=TRUE;
										vec2=line.endPt-line.startPt;
										if(vec*vec2<0)
										{
											f3dPoint pt=line.startPt;
											line.startPt=line.endPt;
											line.endPt=pt;
										}
									}
									else
									{
										vec2=*pPt-line.startPt;
										if(vec2*vec<0)
											line.startPt=*pPt;
										else
										{
											vec2=*pPt-line.endPt;
											if(vec2*vec>0)
												line.endPt=*pPt;
										}
									}
								}
								if(bFindStart&&bFindEnd)
								{	//需要消隐的线段分两段绘制 wht 11-04-07
									f3dPoint line_vec=line.endPt-line.startPt;
									normalize(line_vec);
									double fNewLineLen=DISTANCE(line.startPt,line.endPt)*(1.0/3.0);
									f3dPoint startOffsetPt=line.startPt+line_vec*fNewLineLen;
									f3dPoint endOffsetPt=line.endPt-line_vec*fNewLineLen;
									SetCurLayer(layer);	//设置当前图层
									//生成第一段直线
									Cpy_Pnt(acad_start,line.startPt);
									Cpy_Pnt(acad_end,startOffsetPt);
									AcDbLine *pDashLine1=new AcDbLine(acad_start,acad_end);
									if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pDashLine1))
										pDashLine1->close();
									//生成第二段直线
									Cpy_Pnt(acad_start,endOffsetPt);
									Cpy_Pnt(acad_end,line.endPt);
									AcDbLine *pDashLine2=new AcDbLine(acad_start,acad_end);
									if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pDashLine2))
										pDashLine2->close();
									SetCurLayer(LayerTable::VisibleProfileLayer.layerName);
									if(pLine)
									{
										AcDbLine *pBreakLine=new AcDbLine(AcGePoint3d(line.endPt.x,line.endPt.y,line.endPt.z),
																		  pLine->endPoint());
										if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pBreakLine))
											pBreakLine->close();
										pLine->setEndPoint(AcGePoint3d(line.startPt.x,line.startPt.y,line.startPt.z));
									}
									else if(pPolyline)
									{
										AcDbPolyline *pBreakLine=new AcDbPolyline(2);
										pBreakLine->addVertexAt(0,AcGePoint2d(line.endPt.x,line.endPt.y),0,
																sys.stru_map.fPolyLineWidth,sys.stru_map.fPolyLineWidth);
										AcGePoint2d acad_start2d,acad_end2d;
										pPolyline->getPointAt(0,acad_start2d);
										pPolyline->getPointAt(1,acad_end2d);
										pBreakLine->addVertexAt(1,acad_end2d,0,sys.stru_map.fPolyLineWidth,sys.stru_map.fPolyLineWidth);
										if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,entId,pBreakLine))
											pBreakLine->close();
										pPolyline->setPointAt(1,AcGePoint2d(line.startPt.x,line.startPt.y));
									}
								}
							}
						}
					}
					pEnt->close();
				}
			}
		}
		delete []PtArr;
	}
	acedSSFree(ss_name);

//	acedCommand(RTSTR,"layer",RTSTR,"t",RTSTR,"*",RTSTR,"s",RTSTR,"0",RTSTR,"",RTNONE);
}
int SearchNoneIntersPos(f3dPoint *ent_pos,f3dPoint rgn_vert[4],
						const struct resbuf *filter,
					  int *quad_order,double scope_radius/*=0*/,int quad0_axis1X2Y/*=0*/,f3dPoint *searchDirect/*=NULL*/)
{
	int i,j,k,iRetCode=0;
	long ll=0;
	ads_name ss_name;
	f3dPoint vert[4],offset_x,offset_y;
	ads_point ads_vert[4];
	if(quad0_axis1X2Y==1)	//以X轴为搜索优先规则
	{
		offset_y = rgn_vert[1]-rgn_vert[0];
		offset_x = rgn_vert[2]-rgn_vert[1];
	}
	else
	{
		offset_x = rgn_vert[1]-rgn_vert[0];
		offset_y = rgn_vert[2]-rgn_vert[1];
	}
	normalize(offset_x);normalize(offset_y);	//规格化为单位长
	struct resbuf *plist;
	SCOPE_STRU scope;
	for(i=0;i<4;i++)
	{
		ads_vert[i][X] = rgn_vert[i].x;
		ads_vert[i][Y] = rgn_vert[i].y;
		ads_vert[i][Z] = rgn_vert[i].z;
		scope.VerifyVertex(rgn_vert[i]);
	}
	int sl=20;	//搜索范围长度
	int hr=10;	//搜索半径（范围长度的一半）
	if(scope_radius>20)
	{
		sl=ftoi(scope_radius);
		hr=sl/2;
	}
	ZoomAcadView(scope,sl+10);	//为保证acedSSGet()可以正确获取选择集，调整视口范围 wht 11-06-23
	plist=acutBuildList(RTPOINT,ads_vert[0],RTPOINT,ads_vert[1],RTPOINT,ads_vert[2],RTPOINT,ads_vert[3],RTNONE);
	//retCode更名为rslt以便与iRetCode区分 wjh-2018.3.9
#if defined(_ARX_2007)&&!defined(_ZRX_2012)
	int rslt=acedSSGet(L"cp",plist,NULL,filter,ss_name);
#else
	int rslt=acedSSGet("cp",plist,NULL,filter,ss_name);
#endif
	if(rslt==RTNORM)
		acedSSLength(ss_name,&ll);
	//acedSSGet()获取完选择集后,需立即释放,否则JustifyInters函数中还会进行acedSSGet()处理,选择集过多导致函数运行速率降低  wxc-2018.3.9
	acutRelRb(plist);
	acedSSFree(ss_name);
	{	//此处保留{}仅为了源代码比对 wjh-2018.3.9
		double scope_square=scope_radius*scope_radius;
		if(ll>0)
		{
			if(quad_order==NULL)	//无象限搜索优先顺序，按最近位置搜索
			{
				if(quad0_axis1X2Y==0)
				{	//以象限为基准搜索
					for(int kk=0;kk<=sl;kk++)
					{
						for(j=0;j<=hr;j++)
						{
							i=kk-j;
							if(i==0&&j==0)
								continue;
							else if(i>hr)
								continue;
							//搜索半径大于0时才生效 wht 11-08-05
							else if(scope_radius>0&&(i*i+j*j>scope_square))
								continue;	//不在搜索搜索范围内
							else if(i<0)
								break;
							if(searchDirect&&!searchDirect->IsZero())
							{	//指定搜索方向
								f3dPoint direct=*searchDirect;
								f3dPoint offset=offset_x*i+offset_y*j;
								if(offset*direct>0)
								{
									for(k=0;k<4;k++)
										vert[k]=rgn_vert[k]+offset;
									ll=JustifyInters(vert,filter);
									if(ll<=0)
									{
										i=abs(i);j=abs(j);
										break;
									}
								}
								offset=-offset_x*i+offset_y*j;
								if(offset*direct>0)
								{
									for(k=0;k<4;k++)
										vert[k]=rgn_vert[k]+offset;
									ll=JustifyInters(vert,filter);
									if(ll<=0)
									{
										i=-abs(i);j=abs(j);
										break;
									}
								}
								offset=-offset_x*i-offset_y*j;
								if(offset*direct>0)
								{
									for(k=0;k<4;k++)
										vert[k]=rgn_vert[k]+offset;
									ll=JustifyInters(vert,filter);
									if(ll<=0)
									{
										i=-abs(i);j=-abs(j);
										break;
									}
								}
								offset=offset_x*i-offset_y*j;
								if(offset*direct>0)
								{
									for(k=0;k<4;k++)
										vert[k]=rgn_vert[k]+offset;
									ll=JustifyInters(vert,filter);
									if(ll<=0)
									{
										i=abs(i);j=-abs(j);
										break;
									}
								}
							}
							else
							{
								for(k=0;k<4;k++)
									vert[k]=rgn_vert[k]+offset_x*i+offset_y*j;
								ll=JustifyInters(vert,filter);
								if(ll<=0)
								{
									i=abs(i);j=abs(j);
									break;
								}
								for(k=0;k<4;k++)
									vert[k]=rgn_vert[k]-offset_x*i+offset_y*j;
								ll=JustifyInters(vert,filter);
								if(ll<=0)
								{
									i=-abs(i);j=abs(j);
									break;
								}
								for(k=0;k<4;k++)
									vert[k]=rgn_vert[k]-offset_x*i-offset_y*j;
								ll=JustifyInters(vert,filter);
								if(ll<=0)
								{
									i=-abs(i);j=-abs(j);
									break;
								}
								for(k=0;k<4;k++)
									vert[k]=rgn_vert[k]+offset_x*i-offset_y*j;
								ll=JustifyInters(vert,filter);
								if(ll<=0)
								{
									i=abs(i);j=-abs(j);
									break;
								}
							}
						}
						if(ll<=0)
							break;
					}
				}
				else
				{	//以坐标轴为基准进行搜索
					for(i=0;i<=hr;i++)
					{
						for(j=0;j<=hr;j++)
						{
							if(i==0&&j==0)
								continue;
							else if(scope_radius>0&&(i*i+j*j>scope_square))
								continue;	//不在搜索搜索范围内
							for(k=0;k<4;k++)
								vert[k]=rgn_vert[k]+offset_x*i+offset_y*j;
							ll=JustifyInters(vert,filter);
							if(ll<=0)
							{
								i=abs(i);j=abs(j);
								break;
							}
							for(k=0;k<4;k++)
								vert[k]=rgn_vert[k]-offset_x*i+offset_y*j;
							ll=JustifyInters(vert,filter);
							if(ll<=0)
							{
								i=-abs(i);j=abs(j);
								break;
							}
							for(k=0;k<4;k++)
								vert[k]=rgn_vert[k]-offset_x*i-offset_y*j;
							ll=JustifyInters(vert,filter);
							if(ll<=0)
							{
								i=-abs(i);j=-abs(j);
								break;
							}
							for(k=0;k<4;k++)
								vert[k]=rgn_vert[k]+offset_x*i-offset_y*j;
							ll=JustifyInters(vert,filter);
							if(ll<=0)
							{
								i=abs(i);j=-abs(j);
								break;
							}
						}
						if(ll<=0)
							break;
					}
				}
				if(ll<=0)
					*ent_pos +=offset_x*i+offset_y*j; 
				else
					iRetCode = 1;
			}
			else
			{
				int sign[4][2];
				for(i=0;i<4;i++)
				{
					if(quad_order[i]==1||quad_order[i]==4)
						sign[i][0] = 1;
					else if(quad_order[i]==2||quad_order[i]==3)
						sign[i][0] = -1;
					else
						sign[i][0] = 0;
					if(quad_order[i]==3||quad_order[i]==4)
						sign[i][1] = -1;
					else if(quad_order[i]==1||quad_order[i]==2)
						sign[i][1] = 1;
					else
						sign[i][1] = 0;
				}
				//搜索的第order个优先象限
				for(int order=0;order<4;order++)
				{
					if(sign[order][0]==0||sign[order][1]==0)
						continue;
					f3dPoint offsetx=offset_x*sign[order][0];
					f3dPoint offsety=offset_y*sign[order][1];
					if(quad0_axis1X2Y==0)
					{	//以象限为搜索基准
						for(int kk=0;kk<=sl;kk++)
						{
							for(j=0;j<=hr;j++)
							{
								i=kk-j;
								if(i==0&&j==0)
									continue;
								else if(i>hr)
									continue;
								else if(scope_radius>0&&(i*i+j*j>scope_square))
									continue;	//不在搜索搜索范围内
								else if(i<0)
									break;
								for(k=0;k<4;k++)
									vert[k]=rgn_vert[k]+offsetx*i+offsety*j;
								ll=JustifyInters(vert,filter);
								if(ll<=0)
								{
									i = abs(i)*sign[order][0];
									j = abs(j)*sign[order][1];
									break;
								}
							}
							if(ll<=0)
								break;
						}
					}
					else
					{	//以坐标轴为搜索基准
						for(i=0;i<=hr;i++)
						{
							for(j=0;j<=hr;j++)
							{
								if(i==0&&j==0)
									continue;
								else if(scope_radius>0&&(i*i+j*j>scope_square))
									continue;	//不在搜索搜索范围内
								for(k=0;k<4;k++)
									vert[k]=rgn_vert[k]+offset_x*i*sign[order][0]+offset_y*j*sign[order][1];
								ll=JustifyInters(vert,filter);
								if(ll<=0)
								{
									i = abs(i)*sign[order][0];
									j = abs(j)*sign[order][1];
									break;
								}
							}
							if(ll<=0)
								break;
						}
					}
					if(ll<=0)
						break;
				}
				if(ll<=0)
					*ent_pos +=offset_x*i+offset_y*j; 
				else
					iRetCode = 1;
			}
		}
	}
	return iRetCode;
}

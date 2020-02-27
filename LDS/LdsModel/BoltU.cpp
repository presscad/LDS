#include "stdafx.h"
#include "Tower.h"
#include "CreateFace.h"

CLDSBoltU::CLDSBoltU()
{
	d=4;
	R=20;
	m=52;
	a=55;
	b=32;
	m_bExternalFittingPart=true;
	m_uDisplaySlices=6;
	m_nClassTypeId=CLS_BOLTU;
    //strcpy(m_sClassName,"CLDSBoltU");
}
static void  CreateFaceBy4Vertex(int *vertexArr,CCreateFace *pCreateFace,COLORREF bolt_color,fBody *pBody)
{
	f3dPolyFace* pFace=pBody->faceList.append();
	pFace->material=bolt_color;
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[1],vertexArr[0]);
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[2]);
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[3]);
	pCreateFace->NewOutterEdgeLine(pFace,vertexArr[0]);
	f3dPoint vertex1=pCreateFace->VertexAt(vertexArr[0]);
	f3dPoint vertex2=pCreateFace->VertexAt(vertexArr[1]);
	f3dPoint vertex3=pCreateFace->VertexAt(vertexArr[2]);
	f3dPoint vec1=vertex1-vertex2;
	f3dPoint vec2=vertex3-vertex2;
	pFace->poly_norm=vec1^vec2;
	normalize(pFace->poly_norm);
};
//添加圆弧段钢管轮廓点至实体
//pBody：添加轮廓至pBody中
//arc_start_pt：圆弧起始点
//rotate_center：旋转中心（默认绕-Y轴旋转）
//arcAngle：待生成圆弧角度
//add_head1_tail2_none0：0.不添加是终端轮廓点，1.添加始端轮廓点，2.添加终端轮廓点
//org_section_vertex_arr：钢管圆弧原始截面轮廓点(在坐标系原点位置生成的一组截面点)
static int AddArcTubeSectionVertexsToBody(fBody *pBody,f3dPoint arc_start_pt,f3dPoint rotate_center,double arcAngle,
										  BYTE add_head1_tail2_none0,DYN_ARRAY<f3dPoint> &org_section_vertex_arr,
										  double scale_of_user2scr=1.0,double sample_len=5.0,int smoothness=36)
{
	UCS_STRU  face_ucs;
	double r=DISTANCE(rotate_center,arc_start_pt);
	face_ucs.origin=arc_start_pt;
	int nSlices=CalArcResolution(r,arcAngle,scale_of_user2scr,sample_len,smoothness);
	double rot_ang = arcAngle/nSlices;
	for(int i=0;i<=nSlices;i++)
	{
		if(i==0&&add_head1_tail2_none0!=1)
			continue;
		if(i==nSlices&&add_head1_tail2_none0!=2)
			continue;
		rotate_point_around_axis(face_ucs.origin,rot_ang,rotate_center,rotate_center+f3dPoint(0,-1,0)*100);
		face_ucs.axis_x=rotate_center-face_ucs.origin;
		normalize(face_ucs.axis_x);
		face_ucs.axis_y.Set(0,1,0);
		face_ucs.axis_z=face_ucs.axis_x^face_ucs.axis_y;
		normalize(face_ucs.axis_z);
		for(DWORD j=0;j<org_section_vertex_arr.Size();j++)
		{	//进行平面点的转换
			f3dPoint vertex=org_section_vertex_arr[j];
			coord_trans(vertex ,face_ucs, TRUE);
			pBody->vertex.append(vertex);
		}
	}
	return nSlices;
}
BOOL CLDSBoltU::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

	if(pSolidBody!=NULL&& !IsSolidModified())
		return TRUE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody();
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	fBody body;
	fBody *pBody=&body;
	COLORREF bolt_color=GetColor(CLS_BOLT,d,0);
	if(m_bUserSpecColor)	//用户指定构件颜色
		bolt_color=crMaterial;
	int slices;
	if(m_uDisplaySlices>0)
		slices= m_uDisplaySlices;
	else
		slices= CalArcResolution(d*0.5,2*Pi,scale_of_user2scr,sample_len,smoothness);
	f3dPoint btm_vertex_arr[5000],vertex;
	slices=min(slices,5000);	//防止后面临时建立的outter_down_vertex_arr数组越界
	//1.计算U型螺栓柱底部截面轮廓点数组，先在坐标原点位置计算，然后移动至U型螺栓左侧(即X坐标偏移-m/2)
	GetCircleSimuPolyVertex(d*0.5,btm_vertex_arr,slices);
	DYN_ARRAY<f3dPoint> org_section_vertex_arr(slices);
	double fHalfM=m/2;
	double fCornerR=m*0.05;	//底部为方形的U型螺栓拐角半径
	bool bHalfCir=(R>0);	//底部为半圆形
	for(int i=0;i<slices;i++)
	{
		org_section_vertex_arr[i]=btm_vertex_arr[i];
		btm_vertex_arr[i].x-=fHalfM;
		if(!bHalfCir)	//螺栓柱底部顶点上移拐角半径
			btm_vertex_arr[i].z+=fCornerR;
	}
	//2.添加左侧螺栓柱实体轮廓点
	f3dPolyFace *pFace;
	for(int i=0;i<slices;i++)
		pBody->vertex.append(btm_vertex_arr[i]);
	for(int i=0;i<slices;i++)
		pBody->vertex.append(f3dPoint(btm_vertex_arr[i].x,btm_vertex_arr[i].y,a));
	//3.添加右侧螺栓柱实体轮廓点(左右侧螺栓柱轮廓点关于Y轴对称)
	for (int i=slices-1;i>=0;i--)
		pBody->vertex.append(f3dPoint(-btm_vertex_arr[(i+1)%slices].x,btm_vertex_arr[(i+1)%slices].y,btm_vertex_arr[(i+1)%slices].z));
	for (int i=slices-1;i>=0;i--)
		pBody->vertex.append(f3dPoint(-btm_vertex_arr[(i+1)%slices].x,btm_vertex_arr[(i+1)%slices].y,a));
	//4.计算半圆的实体轮廓点
	int nSlices=0;
	if(!bHalfCir)
	{
		double fArcAngle=0.5*Pi;
		f3dPoint init_pos(-fHalfM,0,fCornerR),rotate_center(-fHalfM+fCornerR,0,fCornerR);
		nSlices+=AddArcTubeSectionVertexsToBody(pBody,init_pos,rotate_center,fArcAngle,2,
			org_section_vertex_arr,scale_of_user2scr,sample_len,smoothness);
		init_pos.Set(fHalfM-fCornerR,0,0);
		rotate_center.Set(fHalfM-fCornerR,0,fCornerR);
		nSlices+=AddArcTubeSectionVertexsToBody(pBody,init_pos,rotate_center,fArcAngle,1,
			org_section_vertex_arr,scale_of_user2scr,sample_len,smoothness);
	}
	else 
	{
		nSlices=AddArcTubeSectionVertexsToBody(pBody,f3dPoint(-fHalfM),f3dPoint(),Pi,0,
			org_section_vertex_arr,scale_of_user2scr,sample_len,smoothness);
	}
	//5.绘制左右两侧两螺栓柱实体
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4]={0,0,0,0};
	for(int ii=0;ii<2;ii++)
	{	//绘制螺栓顶面
		pFace=pBody->faceList.append();
		pFace->poly_norm.Set(0,0,1);
		pFace->material=bolt_color;
		int iStartIndex=ii*2*slices;
		for (int i=slices;i<2*slices;i++)
			createFace.NewOutterEdgeLine(pFace,(i+1)%slices+slices+iStartIndex,i+iStartIndex);
		//绘制螺栓柱侧面
		for(int i=0;i<slices;i++)
		{
			vertexIndexArr[0]=i+iStartIndex;
			vertexIndexArr[1]=i+slices+iStartIndex;
			vertexIndexArr[2]=(i+1)%slices+slices+iStartIndex;
			vertexIndexArr[3]=(i+1)%slices+iStartIndex,&createFace;
			CreateFaceBy4Vertex(vertexIndexArr,&createFace,bolt_color,pBody);
		}
	}
	int iStartIndex=0;
	//计算初始面的中心点
	for (int ii=0;ii<nSlices;ii++)
	{
		iStartIndex=((ii-1)+4)*slices;
		for (int i=0;i<slices;i++)
		{
			if(ii==0)
			{
				vertexIndexArr[0]=i;
				vertexIndexArr[1]=(i+1)%slices;
				vertexIndexArr[2]=(i+1)%slices+4*slices;
				vertexIndexArr[3]=i+4*slices;
			}
			else if(ii==nSlices-1)
			{	//两侧螺栓立柱关于Y轴对称,最后一段生成时右侧立柱轮廓点需倒序循环
				vertexIndexArr[0]=i+iStartIndex;
				vertexIndexArr[1]=(i+1)%slices+iStartIndex;
				vertexIndexArr[2]=(slices-1-i)+2*slices;
				vertexIndexArr[3]=(slices-i)%slices+2*slices;
			}
			else
			{
				vertexIndexArr[0]=i+iStartIndex;
				vertexIndexArr[1]=(i+1)%slices+iStartIndex;
				vertexIndexArr[2]=(i+1)%slices+slices+iStartIndex;
				vertexIndexArr[3]=i+slices+iStartIndex;
			}
			CreateFaceBy4Vertex(vertexIndexArr,&createFace,bolt_color,pBody);
		}
	}
	pSolidBody->ConvertFrom(pBody);
	//生成螺栓帽
	fBody boltCapBody;
	double D=0;     	  	// 螺帽直径.
	double H=0;				// 螺栓头高度.
	double H2=0;			// 螺帽高度.
	FindLsD_H(d,&D,&H,&H2);
	CLDSBolt::CreateBoltCapSolidBody(&boltCapBody,d,D,H2);
	GECS rightAcs(ucs.origin,ucs.axis_x,ucs.axis_y,ucs.axis_z),leftAcs=rightAcs;
	rightAcs.origin.Set(fHalfM,0,a-b);
	leftAcs.origin.Set(-fHalfM,0,a-b);
	coord_trans(rightAcs.origin,ucs,TRUE);
	coord_trans(leftAcs.origin,ucs,TRUE);
	CSolidBody leftCapBody,rightCapBody;
	leftCapBody.ConvertFrom(&boltCapBody);
	leftCapBody.TransToACS(leftAcs);
	rightCapBody.ConvertFrom(&boltCapBody);
	rightCapBody.TransToACS(rightAcs);
	pSolidBody->MergeBodyBuffer(leftCapBody.BufferPtr(),leftCapBody.BufferLength());
    pSolidBody->MergeBodyBuffer(rightCapBody.BufferPtr(),rightCapBody.BufferLength());
	SetModified(FALSE);	//防止重复生成
	return TRUE;
}

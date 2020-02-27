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
//���Բ���θֹ���������ʵ��
//pBody�����������pBody��
//arc_start_pt��Բ����ʼ��
//rotate_center����ת���ģ�Ĭ����-Y����ת��
//arcAngle��������Բ���Ƕ�
//add_head1_tail2_none0��0.��������ն������㣬1.���ʼ�������㣬2.����ն�������
//org_section_vertex_arr���ֹ�Բ��ԭʼ����������(������ϵԭ��λ�����ɵ�һ������)
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
		{	//����ƽ����ת��
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
	if(m_bUserSpecColor)	//�û�ָ��������ɫ
		bolt_color=crMaterial;
	int slices;
	if(m_uDisplaySlices>0)
		slices= m_uDisplaySlices;
	else
		slices= CalArcResolution(d*0.5,2*Pi,scale_of_user2scr,sample_len,smoothness);
	f3dPoint btm_vertex_arr[5000],vertex;
	slices=min(slices,5000);	//��ֹ������ʱ������outter_down_vertex_arr����Խ��
	//1.����U����˨���ײ��������������飬��������ԭ��λ�ü��㣬Ȼ���ƶ���U����˨���(��X����ƫ��-m/2)
	GetCircleSimuPolyVertex(d*0.5,btm_vertex_arr,slices);
	DYN_ARRAY<f3dPoint> org_section_vertex_arr(slices);
	double fHalfM=m/2;
	double fCornerR=m*0.05;	//�ײ�Ϊ���ε�U����˨�սǰ뾶
	bool bHalfCir=(R>0);	//�ײ�Ϊ��Բ��
	for(int i=0;i<slices;i++)
	{
		org_section_vertex_arr[i]=btm_vertex_arr[i];
		btm_vertex_arr[i].x-=fHalfM;
		if(!bHalfCir)	//��˨���ײ��������ƹսǰ뾶
			btm_vertex_arr[i].z+=fCornerR;
	}
	//2.��������˨��ʵ��������
	f3dPolyFace *pFace;
	for(int i=0;i<slices;i++)
		pBody->vertex.append(btm_vertex_arr[i]);
	for(int i=0;i<slices;i++)
		pBody->vertex.append(f3dPoint(btm_vertex_arr[i].x,btm_vertex_arr[i].y,a));
	//3.����Ҳ���˨��ʵ��������(���Ҳ���˨�����������Y��Գ�)
	for (int i=slices-1;i>=0;i--)
		pBody->vertex.append(f3dPoint(-btm_vertex_arr[(i+1)%slices].x,btm_vertex_arr[(i+1)%slices].y,btm_vertex_arr[(i+1)%slices].z));
	for (int i=slices-1;i>=0;i--)
		pBody->vertex.append(f3dPoint(-btm_vertex_arr[(i+1)%slices].x,btm_vertex_arr[(i+1)%slices].y,a));
	//4.�����Բ��ʵ��������
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
	//5.����������������˨��ʵ��
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	int vertexIndexArr[4]={0,0,0,0};
	for(int ii=0;ii<2;ii++)
	{	//������˨����
		pFace=pBody->faceList.append();
		pFace->poly_norm.Set(0,0,1);
		pFace->material=bolt_color;
		int iStartIndex=ii*2*slices;
		for (int i=slices;i<2*slices;i++)
			createFace.NewOutterEdgeLine(pFace,(i+1)%slices+slices+iStartIndex,i+iStartIndex);
		//������˨������
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
	//�����ʼ������ĵ�
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
			{	//������˨��������Y��Գ�,���һ������ʱ�Ҳ������������赹��ѭ��
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
	//������˨ñ
	fBody boltCapBody;
	double D=0;     	  	// ��ñֱ��.
	double H=0;				// ��˨ͷ�߶�.
	double H2=0;			// ��ñ�߶�.
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
	SetModified(FALSE);	//��ֹ�ظ�����
	return TRUE;
}

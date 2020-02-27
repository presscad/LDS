//<LOCALE_TRANSLATE BY hxr /> 2015-04-27

//  ���庯�� ---- SimilarTriangleAnnote            ���ƶ���α�ע
#include "stdafx.h"
#include "stdarx.h"
//#include "TMA.h"
#include "MenuFunc.h"
#include "DrawTaMap.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "adstools.h"
#include "rxtools.h"
#include "rxtools.h"
#include "SysPara.h"
#include "Tower.h"
#include "DragEntSet.h"
#include "DimSimPolyCfgDlg.h"
#include "Drawing.h"
#include "StackAcadVar.h"

//from SegMap.cpp
extern UCS_STRU seg_map_base_face_ucs;	//�ֶ�ͼ�����棨��һ������������ϵ��
static f3dPoint huoqu_face_norm;		//�趨��������Ʒ���
static BOOL g_bHuoQuCursor;				//��ǰ��עΪ������
void DimSimPolyCfg()
{
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
    // When resource from this ARX app is needed, just
    // instantiate a local CAcModuleResourceOverride
    CAcModuleResourceOverride resOverride;
	CDimSimPolyCfgDlg dlg;
	dlg.base_face_ucs = seg_map_base_face_ucs;
	dlg.m_fNormX = huoqu_face_norm.x;
	dlg.m_fNormY = huoqu_face_norm.y;
	dlg.m_fNormZ = huoqu_face_norm.z;
	dlg.m_bDimHuoQuFace = g_bHuoQuCursor;
	if(dlg.DoModal()==IDOK)
	{
		seg_map_base_face_ucs = dlg.base_face_ucs;
		huoqu_face_norm.Set(dlg.m_fNormX,dlg.m_fNormY,dlg.m_fNormZ);
		g_bHuoQuCursor = dlg.m_bDimHuoQuFace;
	}
}
static double CalDistAuto(f3dPoint start,f3dPoint end)
{
	f3dPoint line_vec = end-start;
	if(g_bHuoQuCursor)	//��ǰ��ע���ǻ�����
	{
		double angle = cal_angle_of_2vec(seg_map_base_face_ucs.axis_z,huoqu_face_norm);
		f3dPoint huoqu_vec=seg_map_base_face_ucs.axis_z^huoqu_face_norm;
		vector_trans(huoqu_vec,seg_map_base_face_ucs,FALSE);
		huoqu_vec.z=0;
		normalize(huoqu_vec);
		double dist_huoqu_vec_prj=huoqu_vec*line_vec;
		double dist_huoqu_vec_perp=sqrt(fabs(line_vec.mod2()-pow(dist_huoqu_vec_prj,2)));
		double cos_ang=cos(angle);
		if(cos_ang<eps)
			cos_ang=1.0;
		double real_dist=sqrt(pow(dist_huoqu_vec_prj,2)+pow(dist_huoqu_vec_perp/cos_ang,2));
		return real_dist;
	}
	else
		return line_vec.mod();
}

#ifdef __SMART_DRAW_INC_
/*//���Ŷ�������
void ZoomVertexList(AcDbPolyline *pPolyLine,f3dPoint base_pt,double scale)
{
	if(pPolyLine==NULL)
		return;
	AcGePoint3d location;
	int nn = pPolyLine->numVerts();
	for(int i=0;i<nn;i++)
	{
		pPolyLine->getPointAt(i,location);
		f3dPoint vertex;
		Cpy_Pnt(vertex,location);
		f3dPoint extend_vec=vertex-base_pt;
		double len=extend_vec.mod();
		normalize(extend_vec);
		vertex=base_pt+extend_vec*len*scale;
		Cpy_Pnt(location,vertex);
		pPolyLine->setPointAt(i,AcGePoint2d(location.x,location.y));
	}
}
void ExecuteDimSimPolyCmd(CDrawingCommand *pCmd);
void AutoDimSimPolyCmd(CDrawingCommand *pCmd)
{
	if(pCmd==NULL)
		return;
	if(pCmd->m_eCmdType!=CMD_DIM_SIMPOLY)
		return;
	if(pCmd->m_paraSimPoly.nSimPolyCmdType==3
		||(pCmd->m_paraSimPoly.nSimPolyCmdType==2
		&&pCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum()>3))
	{
		ExecuteDimSimPolyCmd(pCmd);
		return;
	}
	int i;
	ads_name seqent;
	struct resbuf *plist;
	f3dPoint f_start,f_end,vec,old_vec;
	fPtList vertex;
	double scale = 1.0/pCmd->m_pDrawingCom->m_fScale;
	//---------------------------------------------------------	
	InitDrawingEnv();
	AcDbObjectId layerId;
	acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"s",RTSTR,"5.������",RTSTR,"",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"5.������,6.������,7.������",RTSTR,"s",RTSTR,"5.������",RTSTR,"",RTNONE);
	acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
	
	ads_point base_pnt;
	base_pnt[X]=pCmd->m_paraSimPoly.fPickPtX;
	base_pnt[Y]=pCmd->m_paraSimPoly.fPickPtY;
	base_pnt[Z]=0;
	acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
#ifndef _MAPTMA_GRX
	int resCode = acedCommand(RTSTR,"bpoly",RTPOINT,base_pnt,RTSTR,"",RTNONE);
#else
	int resCode = acedCommand(RTSTR,"-boundary",RTPOINT,base_pnt,RTSTR,"",RTNONE);
#endif
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
	DRAGSET.ClearEntSet();

	AcGePoint3d location;
	int nn,vertexNumber;
	if (resCode == RTNORM)
	{
		acdbEntLast(seqent);
		//����Polyline����
		AcDbObjectId plineId;
		acdbGetObjectId(plineId,seqent);
		AcDbPolyline *pPline;
		acdbOpenObject(pPline,plineId,AcDb::kForRead);
		if(pPline==NULL)
		{
			acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
			acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,"5.������",RTSTR,"",RTNONE);
			AfxMessageBox("ѡ���˴���ı߽�,������ѡ��");
			return;
		}
		pPline->close();
		DRAGSET.Add(seqent);
		//plist=acutBuildList(RTSTR,"scale",RTENAME,seqent,RTSTR,"",RTPOINT,base_pnt,RTREAL,pCmd->m_paraSimPoly.fScale,RTNONE);
		//acedCmd(plist);
		//acutRelRb(plist);
		acdbOpenObject(pPline,plineId,AcDb::kForWrite);
		ZoomVertexList(pPline,base_pnt,pCmd->m_paraSimPoly.fScale);
		GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
		nn = pPline->numVerts();
		//�Ը����ߵ�ͷ����
		vertex.Empty();
		for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
		{
			pPline->getPointAt(vertexNumber,location);
			if (vertexNumber==0)
			{
				Cpy_Pnt(f_start,location);
				vertex.append(f_start);
			}
			else
			{
				Cpy_Pnt(f_end,location);
				Sub_Pnt(vec,f_end,f_start);
				normalize(vec);
				if(vertexNumber>1)
				{
					double dd = dot_prod(old_vec,vec);
					if(fabs(dd)>EPS_COS)
					{
						pPline->removeVertexAt(vertexNumber-1);
						vertexNumber--;
						nn--;
						Cpy_Pnt(vertex[vertexNumber],location);
						continue;
					}
				}
				old_vec = vec;
				if(CalDistAuto(f_start,f_end)<0.05)//���� 
				{
					pPline->removeVertexAt(vertexNumber);
					vertexNumber--;
					nn--;
					continue;
				}
				else
				{
					Cpy_Pnt(f_start,f_end);
					vertex.append(f_start);
				}
			}
		}
		pPline->close();
		//
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		nn = vertex.GetNodeNum();
		for (i=0;i<nn;i++)//��ע
		{
			f_start = vertex[i];
			if(i==nn-1)
				f_end = vertex[0];
			else
				f_end   = vertex[i+1];
			char sLen[15];
			double dist = CalDistAuto(f_start,f_end);
			sprintf(sLen,"%.0f",dist*scale/pCmd->m_paraSimPoly.fScale);
			double line_angle,dim_angle;
			line_angle= Cal2dLineAng(f_start.x,f_start.y,f_end.x,f_end.y);
			f3dPoint f_text_pos;
			f_text_pos.x = f_start.x + (f_end.x-f_start.x)/2;
			f_text_pos.y = f_start.y + (f_end.y-f_start.y)/2;
			f_text_pos = GetPtInPolar(f_text_pos,line_angle-Pi/2,0.5);
			if(line_angle>=Pi/2&&line_angle<1.5*Pi)
			{
				dim_angle=line_angle-Pi;
				DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
					AcDb::kTextCenter,AcDb::kTextBottom);
			}
			else
			{
				dim_angle=line_angle;
				DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
					AcDb::kTextCenter,AcDb::kTextTop);
			}
		}
		if(fabs(pCmd->m_paraSimPoly.fDimPosOffsetX)>EPS||fabs(pCmd->m_paraSimPoly.fDimPosOffsetY)>EPS)
		{
			ads_point base_pnt,cur_pnt;
			base_pnt[X]=pCmd->m_paraSimPoly.fPickPtX;
			base_pnt[Y]=pCmd->m_paraSimPoly.fPickPtY;
			base_pnt[Z]=0;
			cur_pnt[X]=base_pnt[X]+pCmd->m_paraSimPoly.fDimPosOffsetX;
			cur_pnt[Y]=base_pnt[Y]+pCmd->m_paraSimPoly.fDimPosOffsetY;
			cur_pnt[Z]=0;
			TranslationEntSet(base_pnt,cur_pnt,DRAGSET.drag_ent_set);
		}
		pBlockTableRecord->close();
	}
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,"5.������",RTSTR,"",RTNONE);
	acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
}

//���Ŷ�������
void ZoomVertexList(fPtList &vertexList,f3dPoint base_pt,double scale)
{
	f3dPoint *pPt=NULL;
	for(pPt=vertexList.GetFirst();pPt;pPt=vertexList.GetNext())
	{
		f3dPoint extend_vec=*pPt-base_pt;
		double len=extend_vec.mod();
		normalize(extend_vec);
		*pPt=base_pt+extend_vec*len*scale;
	}
}

//ƽ�ƶ�������
void MoveVertexList(fPtList &vertexList,double fOffsetX,double fOffsetY)
{
	f3dPoint *pPt=NULL;
	for(pPt=vertexList.GetFirst();pPt;pPt=vertexList.GetNext())
	{
		pPt->x+=fOffsetX;
		pPt->y+=fOffsetY;
	}
}*/

/*
void ExecuteDimSimPolyCmd(CDrawingCommand *pCmd)
{
	if(pCmd==NULL)
		return;
	if(pCmd->m_eCmdType!=CMD_DIM_SIMPOLY)
		return;
	if(pCmd->m_pDrawingCom==NULL)
		return;
	double scale=sys.general_set.StruMapScale;
	SIMPOLY_VERTEX *pVertex=NULL;
	//����������������
	fPtList vertexList;
	f3dPoint f_vertex,f_start,f_end;
	long i=0,nn=pCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum();
	for(pVertex=pCmd->m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
	pVertex=pCmd->m_paraSimPoly.simPolyVertexList.GetNext())
	{
		AcDbEntity *pPrevEnt=NULL,*pNextEnt=NULL;
		pPrevEnt=pCmd->m_pDrawingCom->FindCADEntByHandle(pVertex->hPrevPole);
		pNextEnt=pCmd->m_pDrawingCom->FindCADEntByHandle(pVertex->hNextPole);
		if(pPrevEnt==NULL||pNextEnt==NULL)
			return;	//�Ҳ���ָ����ʵ���޷���ע������
		if(!pPrevEnt->isKindOf(AcDbLine::desc())||!pNextEnt->isKindOf(AcDbLine::desc()))
			return;
		AcDbLine *pPrevLine=(AcDbLine*)pPrevEnt;
		AcDbLine *pNextLine=(AcDbLine*)pNextEnt;
		f3dLine prev_line,next_line;
		Cpy_Pnt(prev_line.startPt,pPrevLine->startPoint());
		Cpy_Pnt(prev_line.endPt,pPrevLine->endPoint());
		Cpy_Pnt(next_line.startPt,pNextLine->startPoint());
		Cpy_Pnt(next_line.endPt,pNextLine->endPoint());
		f3dPoint inters_pt;
		int nRetCode=Int3dll(prev_line,next_line,inters_pt);
		if(nRetCode==-2||nRetCode>0)
			vertexList.append(inters_pt);
	}
	if(nn<=0)	//���������������Ϊ0,����Ҫ����������
		return;
	//�������μ�������
	f3dPoint geom_center;
	nn = vertexList.GetNodeNum();
	for(i=0;i<nn;i++)
		geom_center+=vertexList[i];
	geom_center.x/=nn;
	geom_center.y/=nn;
	//����ʰȡ��λ��(����ڶ���μ������ĵ�ƫ��ֵ) wht 11-06-17
	f3dPoint base_pt;
	if(pCmd->m_paraSimPoly.nSimPolyCmdType==3)
	{	//δָ�����������㼸������
		base_pt=geom_center;
		pCmd->m_paraSimPoly.fPickPtX=base_pt.x;
		pCmd->m_paraSimPoly.fPickPtY=base_pt.y;
		pCmd->m_paraSimPoly.nSimPolyCmdType=2;	//2.��ָ��������������
	}
	else 
	{
		base_pt.x=geom_center.x+pCmd->m_paraSimPoly.fPickPtX;
		base_pt.y=geom_center.y+pCmd->m_paraSimPoly.fPickPtY;
		base_pt.z=0;
	}
	//����������
	ZoomVertexList(vertexList,base_pt,pCmd->m_paraSimPoly.fScale);
	//ƽ��������
	MoveVertexList(vertexList,pCmd->m_paraSimPoly.fDimPosOffsetX,pCmd->m_paraSimPoly.fDimPosOffsetY);
	//����������
	AcDbPolyline *pPolyLine=new AcDbPolyline(nn);
	pPolyLine->setClosed(Adesk::kTrue);
	nn = vertexList.GetNodeNum();
	for(i=0;i<nn;i++)
		pPolyLine->addVertexAt(i,AcGePoint2d(vertexList[i].x,vertexList[i].y));
	
	AcDbObjectId polyId=0;
	DRAGSET.ClearEntSet();
	GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,polyId,pPolyLine))
		pPolyLine->close();//���ùرն���ĳ�Ա����
	else 
	{
		pBlockTableRecord->close();
		return;
	}
	//��ע�����α߳�
	BOOL bIntersPoly=FALSE;
	nn = vertexList.GetNodeNum();
	for(i=0;i<nn;i++)
	{
		f_start = vertexList[i];
		if(i==nn-1)
			f_end = vertexList[0];
		else
			f_end   = vertexList[i+1];
		char sLen[15];
		double dist = CalDistAuto(f_start,f_end);
		sprintf(sLen,"%.0f",dist*scale/pCmd->m_paraSimPoly.fScale);
		double line_angle,dim_angle;
		line_angle= Cal2dLineAng(f_start.x,f_start.y,f_end.x,f_end.y);
		f3dPoint f_text_pos;
		f_text_pos.x = f_start.x + (f_end.x-f_start.x)/2;
		f_text_pos.y = f_start.y + (f_end.y-f_start.y)/2;
		f_text_pos = GetPtInPolar(f_text_pos,line_angle-Pi/2,0.5);
		//��ײ�����������ı���ע,��Ȼ�ᷢ�ָձ�ע�ı���ѡ��
		if(!bIntersPoly)	//��δ�����ཻ�ı߼���ע
		{
			f3dPoint rgn_vert[4];
			double length=strlen(sLen)*sys.dim_map.fSimPolyTextSize*0.70;
			double height=sys.dim_map.fSimPolyTextSize;
			double dd=DistOf2dPtLine(f2dPoint(base_pt.x,base_pt.y),f_start,f_end);
			if(dd<0)
			{
				rgn_vert[0]=GetPtInPolar(f_start,line_angle+Pi/2,0.2);
				rgn_vert[1]=GetPtInPolar(f_end,line_angle+Pi/2,0.2);
				rgn_vert[2]=GetPtInPolar(f_text_pos,line_angle,length/2.0);
				rgn_vert[2]=GetPtInPolar(rgn_vert[2],line_angle+Pi/2,height);
				rgn_vert[3]=GetPtInPolar(f_text_pos,line_angle,-length/2.0);
				rgn_vert[3]=GetPtInPolar(rgn_vert[3],line_angle+Pi/2,height);
			}
			else
			{
				rgn_vert[0]=GetPtInPolar(f_start,line_angle-Pi/2,0.2);
				rgn_vert[1]=GetPtInPolar(f_end,line_angle-Pi/2,0.2);
				rgn_vert[2]=GetPtInPolar(f_text_pos,line_angle,length/2.0);
				rgn_vert[2]=GetPtInPolar(rgn_vert[2],line_angle-Pi/2,height);
				rgn_vert[3]=GetPtInPolar(f_text_pos,line_angle,-length/2.0);
				rgn_vert[3]=GetPtInPolar(rgn_vert[3],line_angle-Pi/2,height);
			}
			if(JustifyInters(rgn_vert,NULL)>0)
				bIntersPoly=TRUE;
		}
		if(line_angle>=Pi/2&&line_angle<1.5*Pi)
		{
			dim_angle=line_angle-Pi;
			DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
				AcDb::kTextCenter,AcDb::kTextBottom);
		}
		else
		{
			dim_angle=line_angle;
			DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
				AcDb::kTextCenter,AcDb::kTextTop);
		}
	}
	if(bIntersPoly)
	{
		ads_point L_T,R_B;
		SCOPE_STRU scope;
		for(int jj=0;jj<vertexList.GetNodeNum();jj++)
			scope.VerifyVertex(vertexList[jj]);
		L_T[X]=scope.fMinX-50;
		L_T[Y]=scope.fMaxY+50;
		L_T[Z]=0;
		R_B[X]=scope.fMaxX+50;
		R_B[Y]=scope.fMinY-50;
		R_B[Z]=0;
		acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
		ads_point insert_pt;
		insert_pt[X]=base_pt.x;
		insert_pt[Y]=base_pt.y;
		insert_pt[Z]=base_pt.z;
		if(DragEntSet(insert_pt,"\n��ѡ��һ�����ʵ������α�עλ��:\n")==RTNORM)
		{
			//base_pt.Set(insert_pt[X],insert_pt[Y],insert_pt[Z]);
			//pCmd->m_paraSimPoly.fDimPosOffsetX=base_pt.x-pCmd->m_paraSimPoly.fPickPtX;	//X�᷽��ƫ����
			//pCmd->m_paraSimPoly.fDimPosOffsetY=base_pt.y-pCmd->m_paraSimPoly.fPickPtY;	//Y�᷽��ƫ����
			pCmd->m_paraSimPoly.fDimPosOffsetX=insert_pt[X]-base_pt.x;
			pCmd->m_paraSimPoly.fDimPosOffsetY=insert_pt[Y]-base_pt.y;
		}
		//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
		acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
	}
	pBlockTableRecord->close();
}*/

//����ʰȡ���ע������ wht 11-08-11
void ExecuteDimSimPolyCmdByPickPt(CDrawingCommand *pCmd)
{
	if(pCmd==NULL)
		return;
	if(pCmd->m_eCmdType!=CMD_DIM_SIMPOLY)
		return;
	if(pCmd->m_paraSimPoly.nSimPolyCmdType<=1)
		return;
	int i;
	ads_name seqent;
	f3dPoint f_start,f_end,vec,old_vec;
	fPtList vertex;
	double scale=1.0/pCmd->m_pDrawingCom->m_fScale;
	//---------------------------------------------------------	
	InitDrawingEnv();
	AcDbObjectId layerId;
	
	ads_point base_pnt;
	base_pnt[X]=pCmd->m_paraSimPoly.fPickPtX;
	base_pnt[Y]=pCmd->m_paraSimPoly.fPickPtY;
	base_pnt[Z]=0;
	if(pCmd->m_paraSimPoly.bPickPosIsLocalCoord)
	{
		base_pnt[X]+=pCmd->m_pDrawingCom->drawing_offset.x;
		base_pnt[Y]+=pCmd->m_pDrawingCom->drawing_offset.y;
		pCmd->m_paraSimPoly.fPickPtX=base_pnt[X];
		pCmd->m_paraSimPoly.fPickPtY=base_pnt[Y];
		pCmd->m_paraSimPoly.bPickPosIsLocalCoord=false;
	}

	CStackAcadVar stackCmdEcho("CMDECHO",0);
	//���������粻���,�ᵼ����Ļˢ������"RE"��Ч,��֪��Ϊʲô,  wjh 2011.8.11
	//acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
	//CXhChar500 sLayers("%s,%s,%s",&(LayerTable::SimPolyLayer.layerName),&(LayerTable::AxisLineLayer.layerName),&(LayerTable::AssistHuoQuLineLayer.layerName));
	//acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,&sLayers,RTSTR,"s",RTSTR,&(LayerTable::SimPolyLayer.layerName),RTSTR,"",RTNONE);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
	acedCommand(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
	acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
	acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
	CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,(ACHAR*)_bstr_t((char*)sLayers),RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
	acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);
	#ifndef _MAPTMA_GRX
	int resCode = acedCommand(RTSTR,L"bpoly",RTPOINT,base_pnt,RTSTR,L"",RTNONE);
	#else
	int resCode = acedCommand(RTSTR,L"-boundary",RTPOINT,base_pnt,RTSTR,L"",RTNONE);
	#endif
#else
	acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
	acedCommand(RTSTR,"zoom",RTSTR,L"e",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
	CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,&sLayers,RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
	acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
	#ifndef _MAPTMA_GRX
	int resCode = acedCommand(RTSTR,"bpoly",RTPOINT,base_pnt,RTSTR,"",RTNONE);
	#else
	int resCode = acedCommand(RTSTR,"-boundary",RTPOINT,base_pnt,RTSTR,"",RTNONE);
	#endif
#endif
	//acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"s",RTSTR,&(LayerTable::SimPolyLayer.layerName),RTSTR,"",RTNONE);
	DRAGSET.ClearEntSet();

	AcGePoint3d location;
	int nn,vertexNumber;
	if (resCode == RTNORM)
	{
		acdbEntLast(seqent);
		//����Polyline����
		AcDbObjectId plineId;
		acdbGetObjectId(plineId,seqent);
		AcDbPolyline *pPline=NULL;
		acdbOpenObject(pPline,plineId,AcDb::kForWrite);
		if(pPline==NULL)
			return;	//AfxMessageBox("ѡ���˴���ı߽�,������ѡ��");
#ifdef _ARX_2007
		acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
#else
		acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
#endif
		//����AcDbPolyline��transformBy()�������ź�ƽ�� wht 11-07-16
		pPline->transformBy(AcGeMatrix3d::scaling(pCmd->m_paraSimPoly.fScale,AcGePoint3d(base_pnt[X],base_pnt[Y],0)));
		pPline->transformBy(AcGeMatrix3d::translation(AcGeVector3d(pCmd->m_paraSimPoly.fDimPosOffsetX,pCmd->m_paraSimPoly.fDimPosOffsetY,0)));
		GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
		nn = pPline->numVerts();
		//
		vertex.Empty();
		//���뽫�����жϷ�����ǰ������ᵼ�¸������Ͻ���С�ǵ��·���ϲ����� wjh-2011.8.27
		if(nn>0)
		{
			pPline->getPointAt(nn-1,location);
			Cpy_Pnt(f_start,location);
		}
		for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
		{
			pPline->getPointAt(vertexNumber,location);
			Cpy_Pnt(f_end,location);
			if(CalDistAuto(f_start,f_end)<0.05)//���� 
			{
				pPline->removeVertexAt(vertexNumber);
				vertexNumber--;
				nn--;
				continue;
			}
			else
				f_start=f_end;
		}
		for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
		{
			if(vertexNumber==0&&nn>1)
			{	//��������ѭ������ wjh-2011.8.27
				pPline->getPointAt(nn-1,location);
				Cpy_Pnt(f_start,location);
				pPline->getPointAt(nn-2,location);
				Cpy_Pnt(f_end,location);
				old_vec=f_start-f_end;
				normalize(old_vec);
			}
			pPline->getPointAt(vertexNumber,location);
			Cpy_Pnt(f_end,location);

			Sub_Pnt(vec,f_end,f_start);
			normalize(vec);
			double dd = dot_prod(old_vec,vec);
			if(fabs(dd)>EPS_COS)
			{
				if(vertexNumber>0)
					pPline->removeVertexAt(vertexNumber-1);
				else
					pPline->removeVertexAt(nn-1);
				vertexNumber--;
				nn--;
				continue;
			}
			old_vec = vec;
			f_start=f_end;
		}
		for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
		{
			pPline->getPointAt(vertexNumber,location);
			Cpy_Pnt(f_end,location);
			vertex.append(f_end);
		}
		pPline->close();
		DRAGSET.Add(seqent);
		//
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		nn = vertex.GetNodeNum();
		for (i=0;i<nn;i++)//��ע
		{
			f_start = vertex[i];
			if(i==nn-1)
				f_end = vertex[0];
			else
				f_end   = vertex[i+1];
			CXhChar16 sLen;
			double dist = CalDistAuto(f_start,f_end);
			//�����ȷ��0.5�ᵼ�´�����0.5���ֲ��뾫ȷ�������ĸ˼������ܲ�����һ��
			//sLen.Printf("%.1f",fto_halfi(dist*scale/pCmd->m_paraSimPoly.fScale));
			//SimplifiedNumString(sLen);
			sLen.Printf("%.0f",dist*scale/pCmd->m_paraSimPoly.fScale);
			double line_angle,dim_angle;
			line_angle= Cal2dLineAng(f_start.x,f_start.y,f_end.x,f_end.y);
			f3dPoint f_text_pos;
			f_text_pos.x = f_start.x + (f_end.x-f_start.x)/2;
			f_text_pos.y = f_start.y + (f_end.y-f_start.y)/2;
			f_text_pos = GetPtInPolar(f_text_pos,line_angle-Pi/2,0.5);
			if(line_angle>=Pi/2&&line_angle<1.5*Pi)
			{
				dim_angle=line_angle-Pi;
				DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
					AcDb::kTextCenter,AcDb::kTextBottom);
			}
			else
			{
				dim_angle=line_angle;
				DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
					AcDb::kTextCenter,AcDb::kTextTop);
			}
		}
		pBlockTableRecord->close();
#ifdef _ARX_2007
		acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);
#else 
		acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
#endif
	}
#ifdef _ARX_2007
	acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"",RTNONE);
	acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
#else
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
#endif
}

//ͨ����ע�����������¼���������б��ע������
void ExecuteDimSimPolyCmdByVertextList(CDrawingCommand *pCmd)
{
	if(pCmd==NULL)
		return;
	if(pCmd->m_eCmdType!=CMD_DIM_SIMPOLY)
		return;
	if(pCmd->m_pDrawingCom==NULL)
		return;
	double scale=1.0/pCmd->m_pDrawingCom->m_fScale;
	long i=0,nn = pCmd->m_paraSimPoly.simPolyVertexList.GetNodeNum();
	if(nn<=0)	//���������������Ϊ0,����Ҫ����������
		return;
	//����ʰȡ��λ��
	f3dPoint base_pt;
	base_pt.x=pCmd->m_paraSimPoly.fPickPtX;
	base_pt.y=pCmd->m_paraSimPoly.fPickPtY;
	base_pt.z=0;
	if(pCmd->m_paraSimPoly.bPickPosIsLocalCoord)
	{
		base_pt.x+=pCmd->m_pDrawingCom->drawing_offset.x;
		base_pt.y+=pCmd->m_pDrawingCom->drawing_offset.y;
		pCmd->m_paraSimPoly.fPickPtX=base_pt.x;
		pCmd->m_paraSimPoly.fPickPtY=base_pt.y;
	}
	//����������
	AcDbPolyline *pPolyLine=new AcDbPolyline(nn);
	pPolyLine->setClosed(Adesk::kTrue);
	SIMPOLY_VERTEX *pVertex=NULL;
	for(pVertex=pCmd->m_paraSimPoly.simPolyVertexList.GetFirst();pVertex;
	pVertex=pCmd->m_paraSimPoly.simPolyVertexList.GetNext())
	{
		GEPOINT vertice(pVertex->vertex.x,pVertex->vertex.y);
		if(pCmd->m_paraSimPoly.bPickPosIsLocalCoord)
		{
			vertice.x+=pCmd->m_pDrawingCom->drawing_offset.x;
			vertice.y+=pCmd->m_pDrawingCom->drawing_offset.y;
			pVertex->vertex=vertice;
		}
		pPolyLine->addVertexAt(i,AcGePoint2d(vertice.x,vertice.y));
		i++;
	}
	pCmd->m_paraSimPoly.bPickPosIsLocalCoord=false;
	//����AcDbPolyline��transformBy()�������ź�ƽ�� wht 11-07-16
	pPolyLine->transformBy(AcGeMatrix3d::scaling(pCmd->m_paraSimPoly.fScale,AcGePoint3d(base_pt.x,base_pt.y,0)));
	pPolyLine->transformBy(AcGeMatrix3d::translation(AcGeVector3d(pCmd->m_paraSimPoly.fDimPosOffsetX,pCmd->m_paraSimPoly.fDimPosOffsetY,0)));
	AcDbObjectId polyId=0;
	DRAGSET.ClearEntSet();
	GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,polyId,pPolyLine))
		pPolyLine->close();//���ùرն���ĳ�Ա����
	else 
	{
		pBlockTableRecord->close();
		return;
	}
	//��ע�����α߳�
	BOOL bClockwise=FALSE;
	nn=pPolyLine->numVerts();
	f3dPoint f_start,f_end;
	for(i=0;i<nn;i++)
	{
		AcGePoint3d temp_pt;
		pPolyLine->getPointAt(i,temp_pt);
		Cpy_Pnt(f_start,temp_pt);
		pPolyLine->getPointAt((i+1)%nn,temp_pt);
		Cpy_Pnt(f_end,temp_pt);
		if(i==0&&nn>2)
		{	//ʹ��ǰ��������㷨�߷���
			pPolyLine->getPointAt((i+2)%nn,temp_pt);
			f3dPoint first_vec,sec_vec;
			Sub_Pnt(first_vec,f_end,f_start);
			Sub_Pnt(sec_vec,temp_pt,f_end);
			normalize(first_vec);
			normalize(sec_vec);
			f3dPoint plane_norm=first_vec^sec_vec;
			if(plane_norm.z<0)
				bClockwise=TRUE;
		}
		if(bClockwise)
		{	//������������˳��Ϊ˳ʱ�뷽��ʼ�ն���Ҫ�Ե���֤���ֱ�ע����� wht 11-07-31
			Cpy_Pnt(temp_pt,f_start);
			Cpy_Pnt(f_start,f_end);
			Cpy_Pnt(f_end,temp_pt);
		}
		char sLen[15];
		double dist = CalDistAuto(f_start,f_end);
		sprintf(sLen,"%.0f",dist*scale/pCmd->m_paraSimPoly.fScale);
		double line_angle,dim_angle;
		line_angle= Cal2dLineAng(f_start.x,f_start.y,f_end.x,f_end.y);
		f3dPoint f_text_pos;
		f_text_pos.x = f_start.x + (f_end.x-f_start.x)/2;
		f_text_pos.y = f_start.y + (f_end.y-f_start.y)/2;
		f_text_pos = GetPtInPolar(f_text_pos,line_angle-Pi/2,0.5);
		if(line_angle>=Pi/2&&line_angle<1.5*Pi)
		{
			dim_angle=line_angle-Pi;
			DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
				AcDb::kTextCenter,AcDb::kTextBottom);
		}
		else
		{
			dim_angle=line_angle;
			DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
				AcDb::kTextCenter,AcDb::kTextTop);
		}
	}
	pBlockTableRecord->close();
}

void AddDimSimPolyCmd(CDrawingComponent *pDrawingCom,ATOM_LIST<CDrawingCommand*> *pCmdList=NULL)
{
	if(pDrawingCom==NULL)
		return;
	int i;
	ads_point base_pnt;
	ads_name seqent,sss;
	f3dPoint f_start,f_end,vec,old_vec;
	fPtList vertex;
	double scale = 1.0/pDrawingCom->m_fScale;
    CAcModuleResourceOverride resOverride;
//---------------------------------------------------------	
	InitDrawingEnv();
	AcDbObjectId layerId;
#ifdef _ARX_2007
	acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
	acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
	acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
	CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,(ACHAR*)_bstr_t((char*)sLayers),RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
	acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);
#else
	acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
	CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,&sLayers,RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
	acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
#endif
	for(;;)
	{
#ifdef _ARX_2007
		acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
#else
		acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
		if (ads_getpoint(NULL,L"\n Please click any point on similar polygon area,<Enter Exit>: ",base_pnt)==RTNORM)
#else
		if (ads_getpoint(NULL,"\n Please click any point on similar polygon area,<Enter Exit>: ",base_pnt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
		if (ads_getpoint(NULL,L"\n�����������������һ��,<Enter�˳�>: ",base_pnt)==RTNORM)
#else
		if (ads_getpoint(NULL,"\n�����������������һ��,<Enter�˳�>: ",base_pnt)==RTNORM)
#endif
#endif
		{
#ifdef _ARX_2007
#ifndef _MAPTMA_GRX
			int resCode = acedCommand(RTSTR,L"bpoly",RTPOINT,base_pnt,RTSTR,L"",RTNONE);
#else
			int resCode = acedCommand(RTSTR,L"-boundary",RTPOINT,base_pnt,RTSTR,L"",RTNONE);
#endif
			acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"",RTNONE);
#else
#ifndef _MAPTMA_GRX
			int resCode = acedCommand(RTSTR,"bpoly",RTPOINT,base_pnt,RTSTR,"",RTNONE);
#else
			int resCode = acedCommand(RTSTR,"-boundary",RTPOINT,base_pnt,RTSTR,"",RTNONE);
#endif
			acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
#endif
			acedSSAdd(NULL,NULL,sss);
			DRAGSET.ClearEntSet();
			BOOL bIntersPoly=FALSE;
			
			AcGePoint3d location;
			int nn,vertexNumber;
			CDrawingCommand *pCmd=NULL;
			if (resCode == RTNORM)
			{
				acdbEntLast(seqent);
				//����Polyline����
				AcDbObjectId plineId;
				acdbGetObjectId(plineId,seqent);
				AcDbPolyline *pPline;
				acdbOpenObject(pPline,plineId,AcDb::kForRead);
				if(pPline==NULL)
				{
#ifdef _ARX_2007
					acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
					CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
					acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,(ACHAR*)_bstr_t((char*)sLayers),RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
#else
					acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,L"*",RTSTR,L"",RTNONE);
					CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
					acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,(char*)sLayers,RTSTR,L"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Selected a wrong border,please select again");
#else
					AfxMessageBox("ѡ���˴���ı߽�,������ѡ��");
#endif
					continue;
				}
				pPline->close();
				//�Ƴ�����(���߻����Ͻ�)������
				acdbOpenObject(pPline,plineId,AcDb::kForWrite);
				GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
				nn = pPline->numVerts();
				for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
				{
					pPline->getPointAt(vertexNumber,location);
					if (vertexNumber==0)
						Cpy_Pnt(f_start,location);
					else
					{
						Cpy_Pnt(f_end,location);
						Sub_Pnt(vec,f_end,f_start);
						normalize(vec);
						if(vertexNumber>1)
						{
							double dd = dot_prod(old_vec,vec);
							if(fabs(dd)>EPS_COS)
							{
								pPline->removeVertexAt(vertexNumber-1);
								vertexNumber--;
								nn--;
								continue;
							}
						}
						old_vec = vec;
						if(CalDistAuto(f_start,f_end)<0.05)//���� 
						{
							pPline->removeVertexAt(vertexNumber);
							vertexNumber--;
							nn--;
							continue;
						}
						else 
							Cpy_Pnt(f_start,f_end);
					}
				}
				pPline->close();
				//��������
				pCmd=pDrawingCom->cmdList.append();
				pCmd->m_pDrawingCom=pDrawingCom;
				pCmd->m_eCmdType=CMD_DIM_SIMPOLY;
				pCmd->InitCommandPara();
				pCmd->m_paraSimPoly.nSimPolyCmdType=2;		//2.��ָ��������������
				pCmd->m_paraSimPoly.fPickPtX=base_pnt[X];
				pCmd->m_paraSimPoly.fPickPtY=base_pnt[Y];
				pCmd->m_paraSimPoly.simPolyVertexList.Empty();
				if(pCmdList)	//�����¼ӵ�������������ڸ��������б� wht 11-07-25
					pCmdList->append(pCmd);
				//�������������������� wht 11-06-29
				for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
				{
					pPline->getPointAt(vertexNumber,location);
					SIMPOLY_VERTEX *pSimPolyVertex=pCmd->m_paraSimPoly.simPolyVertexList.append();
					Cpy_Pnt(pSimPolyVertex->vertex,location);
				}
				DRAGSET.Add(seqent);	
				//����������
				//struct resbuf *plist;
				//plist=acutBuildList(RTSTR,"scale",RTENAME,seqent,RTSTR,"",RTPOINT,base_pnt,RTREAL,sys.dim_map.fSimTriScale,RTNONE);
				//acedCmd(plist);
				//acutRelRb(plist);

				//����AcDbPolyline��transformBy()�������� wht 11-07-16
				acdbOpenObject(pPline,plineId,AcDb::kForWrite);
				pPline->transformBy(AcGeMatrix3d::scaling(sys.dim_map.fSimTriScale,AcGePoint3d(base_pnt[X],base_pnt[Y],0)));
				pPline->close();
				nn = pPline->numVerts();
				//��䶥���б�
				vertex.Empty();
				for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
				{
					pPline->getPointAt(vertexNumber,location);
					Cpy_Pnt(f_start,location);
					vertex.append(f_start);
				}
			}
			else
			{
#ifdef _ARX_2007
				acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"",RTNONE);
				acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
#else
				acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
				acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
#endif
				continue;
			}
			//����������
			GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
			AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
			nn = vertex.GetNodeNum();
			for (i=0;i<nn;i++)//��ע
			{
				f_start = vertex[i];
				if(i==nn-1)
					f_end = vertex[0];
				else
					f_end   = vertex[i+1];
				char sLen[15];
				double dist = CalDistAuto(f_start,f_end);
				sprintf(sLen,"%.0f",dist*scale/sys.dim_map.fSimTriScale);
				double line_angle,dim_angle;
				line_angle= Cal2dLineAng(f_start.x,f_start.y,f_end.x,f_end.y);
				f3dPoint f_text_pos;
				f_text_pos.x = f_start.x + (f_end.x-f_start.x)/2;
				f_text_pos.y = f_start.y + (f_end.y-f_start.y)/2;
				f_text_pos = GetPtInPolar(f_text_pos,line_angle-Pi/2,0.5);
				//��ײ�����������ı���ע,��Ȼ�ᷢ�ָձ�ע�ı���ѡ��
				if(!bIntersPoly)	//��δ�����ཻ�ı߼���ע
				{
					f3dPoint rgn_vert[4];
					double length=strlen(sLen)*sys.dim_map.fSimPolyTextSize*0.70;
					double height=sys.dim_map.fSimPolyTextSize;
					double dd=DistOf2dPtLine(f2dPoint(base_pnt[X],base_pnt[Y]),f_start,f_end);
					if(dd<0)
					{
						rgn_vert[0]=GetPtInPolar(f_start,line_angle+Pi/2,0.2);
						rgn_vert[1]=GetPtInPolar(f_end,line_angle+Pi/2,0.2);
						rgn_vert[2]=GetPtInPolar(f_text_pos,line_angle,length/2.0);
						rgn_vert[2]=GetPtInPolar(rgn_vert[2],line_angle+Pi/2,height);
						rgn_vert[3]=GetPtInPolar(f_text_pos,line_angle,-length/2.0);
						rgn_vert[3]=GetPtInPolar(rgn_vert[3],line_angle+Pi/2,height);
					}
					else
					{
						rgn_vert[0]=GetPtInPolar(f_start,line_angle-Pi/2,0.2);
						rgn_vert[1]=GetPtInPolar(f_end,line_angle-Pi/2,0.2);
						rgn_vert[2]=GetPtInPolar(f_text_pos,line_angle,length/2.0);
						rgn_vert[2]=GetPtInPolar(rgn_vert[2],line_angle-Pi/2,height);
						rgn_vert[3]=GetPtInPolar(f_text_pos,line_angle,-length/2.0);
						rgn_vert[3]=GetPtInPolar(rgn_vert[3],line_angle-Pi/2,height);
					}
					if(JustifyInters(rgn_vert,NULL)>0)
						bIntersPoly=TRUE;
				}
				if(line_angle>=Pi/2&&line_angle<1.5*Pi)
				{
					dim_angle=line_angle-Pi;
					DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
						AcDb::kTextCenter,AcDb::kTextBottom);
				}
				else
				{
					dim_angle=line_angle;
					DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
						AcDb::kTextCenter,AcDb::kTextTop);
				}
			}
			if(bIntersPoly)
			{
				ads_point L_T,R_B;
				SCOPE_STRU scope;
				for(int jj=0;jj<vertex.GetNodeNum();jj++)
					scope.VerifyVertex(vertex[jj]);
				L_T[X]=scope.fMinX-50;
				L_T[Y]=scope.fMaxY+50;
				L_T[Z]=0;
				R_B[X]=scope.fMaxX+50;
				R_B[Y]=scope.fMinY-50;
				R_B[Z]=0;
#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
#else
				acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
#endif
				f3dPoint old_base_pnt(base_pnt[X],base_pnt[Y],0);
#ifdef AFX_TARG_ENU_ENGLISH
				if(DragEntSet(base_pnt,"\n Please select a right place to dimension similar polygon:\n")==RTNORM)
#else
				if(DragEntSet(base_pnt,"\n��ѡ��һ�����ʵ������α�עλ��:\n")==RTNORM)
#endif
				{	
					pCmd->m_paraSimPoly.fDimPosOffsetX=base_pnt[X]-old_base_pnt.x;	//X�᷽��ƫ����
					pCmd->m_paraSimPoly.fDimPosOffsetY=base_pnt[Y]-old_base_pnt.y;	//Y�᷽��ƫ����
				}
				//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
#else
				acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
#endif
			}
			//��һ�����������ʱ�ڴ˴��������ʵ��lD�б� wht 11-03-28
			pCmd->entIdList.Empty();
			for(i=0;i<DRAGSET.GetEntNum();i++)
			{
				AcDbObjectId ent_id;
				if(DRAGSET.GetEntId(i,ent_id))
					pCmd->entIdList.append(ent_id);
			}
			pBlockTableRecord->close();
		}
#ifdef _ARX_2007
		else
		{
			acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"",RTNONE);
			acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
			break;
		}
		acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
		CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
		acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,(ACHAR*)_bstr_t((char*)sLayers),RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
		acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);
#else
		else
		{
			acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
			acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
			break;
		}
		acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
		CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
		acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,&sLayers,RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
		acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
#endif
	}
}
#endif

void DimSimPoly()
{
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	int i;
	ads_point base_pnt;
	ads_name seqent,sss;
	struct resbuf *plist;
	f3dPoint f_start,f_end,vec,old_vec;
	fPtList vertex;
	double scale = sys.general_set.StruMapScale.fRealToDraw;
    CAcModuleResourceOverride resOverride;
//---------------------------------------------------------	
	InitDrawingEnv();
	AcDbObjectId layerId;
#ifdef _ARX_2007
	acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
	acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
	acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
	CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,(ACHAR*)_bstr_t((char*)sLayers),RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
	acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);
#else
	acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
	acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
	CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
	acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,&sLayers,RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
	acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
#endif
	
	for(;;)
	{
#ifdef _ARX_2007
		acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
#else
		acedCommand(RTSTR,"undo",RTSTR,"be",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
		if (ads_getpoint(NULL,L"\n Please click any point on similar polygon area,<Enter Exit>: ",base_pnt)==RTNORM)
#else
		if (ads_getpoint(NULL,"\n Please click any point on similar polygon area,<Enter Exit>: ",base_pnt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
		if (ads_getpoint(NULL,L"\n�����������������һ��,<Enter�˳�>: ",base_pnt)==RTNORM)
#else
		if (ads_getpoint(NULL,"\n�����������������һ��,<Enter�˳�>: ",base_pnt)==RTNORM)
#endif
#endif
		{
#ifdef _ARX_2007
#ifndef _MAPTMA_GRX
			int resCode = acedCommand(RTSTR,L"bpoly",RTPOINT,base_pnt,RTSTR,L"",RTNONE);
#else
			int resCode = acedCommand(RTSTR,L"-boundary",RTPOINT,base_pnt,RTSTR,L"",RTNONE);
#endif
			acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"",RTNONE);
#else
#ifndef _MAPTMA_GRX
			int resCode = acedCommand(RTSTR,"bpoly",RTPOINT,base_pnt,RTSTR,"",RTNONE);
#else
			int resCode = acedCommand(RTSTR,"-boundary",RTPOINT,base_pnt,RTSTR,"",RTNONE);
#endif
			acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
#endif
			acedSSAdd(NULL,NULL,sss);
			DRAGSET.ClearEntSet();
			BOOL bIntersPoly=FALSE;
			
			AcGePoint3d location;
			int nn,vertexNumber;
			if (resCode == RTNORM)
			{
				acdbEntLast(seqent);
				//����Polyline����
				AcDbObjectId plineId;
				acdbGetObjectId(plineId,seqent);
				AcDbPolyline *pPline;
				acdbOpenObject(pPline,plineId,AcDb::kForRead);
				if(pPline==NULL)
				{
#ifdef _ARX_2007
					acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
					CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
					acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,(ACHAR*)_bstr_t((char*)sLayers),RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
#else
					acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,L"*",RTSTR,L"",RTNONE);
					CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
					acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,(char*)sLayers,RTSTR,L"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Selected a wrong border,please select again");
#else
					AfxMessageBox("ѡ���˴���ı߽�,������ѡ��");
#endif
					continue;
				}
				pPline->close();
				DRAGSET.Add(seqent);
#ifdef _ARX_2007
				plist=acutBuildList(RTSTR,L"scale",RTENAME,seqent,RTSTR,L"",RTPOINT,base_pnt,RTREAL,sys.dim_map.fSimTriScale,RTNONE);
#else
				plist=acutBuildList(RTSTR,"scale",RTENAME,seqent,RTSTR,"",RTPOINT,base_pnt,RTREAL,sys.dim_map.fSimTriScale,RTNONE);
#endif
				acedCmd(plist);
				acutRelRb(plist);
				acdbOpenObject(pPline,plineId,AcDb::kForWrite);
				GetCurDwg()->setClayer(LayerTable::SimPolyLayer.layerId);
				nn = pPline->numVerts();
				//�Ը����ߵ�ͷ����
				vertex.Empty();
				//���뽫�����жϷ�����ǰ������ᵼ�¸������Ͻ���С�ǵ��·���ϲ����� wjh-2011.8.27
				if(nn>0)
				{
					pPline->getPointAt(nn-1,location);
					Cpy_Pnt(f_start,location);
				}
				for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
				{
					pPline->getPointAt(vertexNumber,location);
					Cpy_Pnt(f_end,location);
					if(CalDistAuto(f_start,f_end)<0.05)//���� 
					{
						pPline->removeVertexAt(vertexNumber);
						vertexNumber--;
						nn--;
						continue;
					}
					else
						f_start=f_end;
				}
				for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
				{
					if(vertexNumber==0&&nn>1)
					{	//��������ѭ������ wjh-2011.8.27
						pPline->getPointAt(nn-1,location);
						Cpy_Pnt(f_start,location);
						pPline->getPointAt(nn-2,location);
						Cpy_Pnt(f_end,location);
						old_vec=f_start-f_end;
						normalize(old_vec);
					}
					pPline->getPointAt(vertexNumber,location);
					Cpy_Pnt(f_end,location);
	
					Sub_Pnt(vec,f_end,f_start);
					normalize(vec);
					double dd = dot_prod(old_vec,vec);
					if(fabs(dd)>EPS_COS)
					{
						if(vertexNumber>0)
							pPline->removeVertexAt(vertexNumber-1);
						else
							pPline->removeVertexAt(nn-1);
						vertexNumber--;
						nn--;
						continue;
					}
					old_vec = vec;
					f_start=f_end;
				}
				for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
				{
					pPline->getPointAt(vertexNumber,location);
					Cpy_Pnt(f_end,location);
					vertex.append(f_end);
				}
				pPline->close();
			}
			else
			{
#ifdef _ARX_2007
				acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"",RTNONE);
				acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
#else
				acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
				acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
#endif
				continue;
			}
			
			AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
			nn = vertex.GetNodeNum();
			for (i=0;i<nn;i++)//��ע
			{
				f_start = vertex[i];
				if(i==nn-1)
					f_end = vertex[0];
				else
					f_end   = vertex[i+1];
				char sLen[15];
				double dist = CalDistAuto(f_start,f_end);
				sprintf(sLen,"%.0f",dist*scale/sys.dim_map.fSimTriScale);
				double line_angle,dim_angle;
				line_angle= Cal2dLineAng(f_start.x,f_start.y,f_end.x,f_end.y);
				f3dPoint f_text_pos;
				f_text_pos.x = f_start.x + (f_end.x-f_start.x)/2;
				f_text_pos.y = f_start.y + (f_end.y-f_start.y)/2;
				f_text_pos = GetPtInPolar(f_text_pos,line_angle-Pi/2,0.5);
				//��ײ�����������ı���ע,��Ȼ�ᷢ�ָձ�ע�ı���ѡ��
				if(!bIntersPoly)	//��δ�����ཻ�ı߼���ע
				{
					f3dPoint rgn_vert[4];
					double length=strlen(sLen)*sys.dim_map.fSimPolyTextSize*0.70;
					double height=sys.dim_map.fSimPolyTextSize;
					double dd=DistOf2dPtLine(f2dPoint(base_pnt[X],base_pnt[Y]),f_start,f_end);
					if(dd<0)
					{
						rgn_vert[0]=GetPtInPolar(f_start,line_angle+Pi/2,0.2);
						rgn_vert[1]=GetPtInPolar(f_end,line_angle+Pi/2,0.2);
						rgn_vert[2]=GetPtInPolar(f_text_pos,line_angle,length/2.0);
						rgn_vert[2]=GetPtInPolar(rgn_vert[2],line_angle+Pi/2,height);
						rgn_vert[3]=GetPtInPolar(f_text_pos,line_angle,-length/2.0);
						rgn_vert[3]=GetPtInPolar(rgn_vert[3],line_angle+Pi/2,height);
					}
					else
					{
						rgn_vert[0]=GetPtInPolar(f_start,line_angle-Pi/2,0.2);
						rgn_vert[1]=GetPtInPolar(f_end,line_angle-Pi/2,0.2);
						rgn_vert[2]=GetPtInPolar(f_text_pos,line_angle,length/2.0);
						rgn_vert[2]=GetPtInPolar(rgn_vert[2],line_angle-Pi/2,height);
						rgn_vert[3]=GetPtInPolar(f_text_pos,line_angle,-length/2.0);
						rgn_vert[3]=GetPtInPolar(rgn_vert[3],line_angle-Pi/2,height);
					}
					if(JustifyInters(rgn_vert,NULL)>0)
						bIntersPoly=TRUE;
				}
				if(line_angle>=Pi/2&&line_angle<1.5*Pi)
				{
					dim_angle=line_angle-Pi;
					DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
						AcDb::kTextCenter,AcDb::kTextBottom);
				}
				else
				{
					dim_angle=line_angle;
					DimText(pBlockTableRecord,f_text_pos,sLen,TextStyleTable::hzfs.textStyleId,sys.dim_map.fSimPolyTextSize,dim_angle,
						AcDb::kTextCenter,AcDb::kTextTop);
				}
			}
			if(bIntersPoly)
			{
				ads_point L_T,R_B;
				SCOPE_STRU scope;
				for(int jj=0;jj<vertex.GetNodeNum();jj++)
					scope.VerifyVertex(vertex[jj]);
				L_T[X]=scope.fMinX-50;
				L_T[Y]=scope.fMaxY+50;
				L_T[Z]=0;
				R_B[X]=scope.fMaxX+50;
				R_B[Y]=scope.fMinY-50;
				R_B[Z]=0;
#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
#else
				acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
				DragEntSet(base_pnt,"\n Please select a right place to dimension similar polygon:\n");
#else
				DragEntSet(base_pnt,"\n��ѡ��һ�����ʵ������α�עλ��:\n");
#endif
				//����ָ�ԭ�е���ͼ״̬����ȻacedSSGet��������ʧ��
#ifdef _ARX_2007
				acedCommand(RTSTR,L"zoom",RTSTR,L"P",RTNONE);
#else
				acedCommand(RTSTR,"zoom",RTSTR,"P",RTNONE);
#endif
			}
			pBlockTableRecord->close();
		}
#ifdef _ARX_2007
		else
		{
			acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,L"*",RTSTR,L"",RTNONE);
			acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
			break;
		}
		acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
		CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
		acedCommand(RTSTR,L"layer",RTSTR,L"T",RTSTR,(ACHAR*)_bstr_t((char*)sLayers),RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)LayerTable::SimPolyLayer.layerName),RTSTR,L"",RTNONE);
		acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);
#else
		else
		{
			acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
			acedCommand(RTSTR,"layer",RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
			break;
		}
		acedCommand(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
		CXhChar500 sLayers("%s,%s,%s",(char*)LayerTable::SimPolyLayer.layerName,(char*)LayerTable::AxisLineLayer.layerName,(char*)LayerTable::AssistHuoQuLineLayer.layerName);
		acedCommand(RTSTR,"layer",RTSTR,"T",RTSTR,&sLayers,RTSTR,"s",RTSTR,(char*)LayerTable::SimPolyLayer.layerName,RTSTR,"",RTNONE);
		acedCommand(RTSTR,"undo",RTSTR,"e",RTNONE);
#endif
	}
}

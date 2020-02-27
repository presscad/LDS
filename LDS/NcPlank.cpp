// NcPlank.cpp: implementation of the CNcPlank class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NcPlank.h"
#include "LibraryQuery.h"
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNcPlank::CNcPlank()
{
}

CNcPlank::~CNcPlank()
{

}
BOOL CNcPlank::GenNCFile(CLDSGeneralPlate *pPlank,char *sFileName,BOOL bExportDeformedProfile/*=TRUE*/)
{
	if(pPlank==NULL)
		return FALSE;
	double max_edge = 0, edge_dist = 0;
	f3dPoint vertice_ref,ls_pos,NoPos;
	machine_ucs.axis_z.Set(0,0,1);
	if(pPlank->vertex_list.GetNodeNum()<3)
		return FALSE;
	FILE *fp;
	char nc_file[MAX_PATH];
	strcpy(nc_file,sFileName);
	strcat(nc_file,".pnc");
	if((fp=fopen(nc_file,"wb"))==NULL)
		return FALSE;
	PROFILE_VERTEX *pVertex=NULL;
	ATOM_LIST<PROFILE_VERTEX> vertex_list;	//�嶥���б�
	CHashSet<PROFILE_VERTEX*> hashDestVertexPtrBySrcVertexPtr;
	vertex_list.Empty();
	for(pVertex=pPlank->vertex_list.GetFirst();pVertex;pVertex=pPlank->vertex_list.GetNext())
	{
		PROFILE_VERTEX *pNewVertex=vertex_list.append(*pVertex);
		hashDestVertexPtrBySrcVertexPtr.SetValue((DWORD)pVertex,pNewVertex);
	}

	int i,roll_edge_num=0,n=vertex_list.GetNodeNum();
	f3dLine huoquLine0=pPlank->GetHuoquLine(0);
	f3dLine huoquLine1=pPlank->GetHuoquLine(1);
	for(i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=&pPlank->vertex_list[i];
		PROFILE_VERTEX *pEndVertex=&pPlank->vertex_list[(i+1)%n];
		if(pStartVertex->type==0&&pStartVertex->m_bRollEdge&&pStartVertex->manu_space!=0)
		{	//������
			PROFILE_VERTEX *pPrevVertex=&pPlank->vertex_list[(i+n-1)%n];
			PROFILE_VERTEX *pNextVertex=&pPlank->vertex_list[(i+2)%n];
			if((pStartVertex->vertex==huoquLine0.startPt&&pEndVertex->vertex==huoquLine0.endPt)||
				(pStartVertex->vertex==huoquLine1.startPt&&pEndVertex->vertex==huoquLine1.endPt))
				continue;
			f3dLine huoqu_line,roll_edge_line;
			pPlank->CalRollLineAndHuoquLine(pStartVertex,pEndVertex,pPrevVertex,pNextVertex,huoqu_line,roll_edge_line);
			//���ǻ�����ƫ���������������Ӧ����Ӧ�����������������ʱ�����������߲���ֱ wht 16-05-30
			PROFILE_VERTEX *pNewStartVertex=hashDestVertexPtrBySrcVertexPtr.GetValue((DWORD)pStartVertex);
			PROFILE_VERTEX *pNewEndVertex=hashDestVertexPtrBySrcVertexPtr.GetValue((DWORD)pEndVertex);
			if(pNewStartVertex)
				pNewStartVertex->vertex=huoqu_line.startPt;
			if(pNewEndVertex)
				pNewEndVertex->vertex=huoqu_line.endPt;
			PROFILE_VERTEX new_vertex;
			new_vertex.type=0;
			new_vertex.vertex=roll_edge_line.startPt;
			if((i+roll_edge_num*2+1)%(n+roll_edge_num*2)==0)
				vertex_list.append(new_vertex);
			else
				vertex_list.insert(new_vertex,(i+roll_edge_num*2+1)%(n+roll_edge_num*2));
			new_vertex.vertex=roll_edge_line.endPt;
			if((i+roll_edge_num*2+2)%(n+roll_edge_num*2+1)==0)
				vertex_list.append(new_vertex);
			else
				vertex_list.insert(new_vertex,(i+roll_edge_num*2+2)%(n+roll_edge_num*2+1));
			roll_edge_num++;
		}
	}
	/*
	n=vertex_list.GetNodeNum();
	for(i=0;i<n;i++)
	{
		PROFILE_VERTEX *pStartVertex=&vertex_list[i];
		PROFILE_VERTEX *pEndVertex=&vertex_list[(i+1)%n];
		
		if(pStartVertex->m_bWeldEdge&&pStartVertex->manu_space!=0)
		{	//����ӹ���϶��
			f3dLine manu_cut_line(pStartVertex->vertex,pEndVertex->vertex);	//�����˼ӹ���϶��ʵ�ʼӹ��и���
			f3dPoint offset_vec=manu_cut_line.endPt-manu_cut_line.startPt;
			if(pStartVertex->type==0)
			{
				normalize(offset_vec);
				offset_vec.Set(-offset_vec.y,offset_vec.x);	//��ʱ����ת90��ȷ����ֱ��������ļ�϶ƫ�Ʒ���
				manu_cut_line.startPt+=offset_vec*pStartVertex->manu_space;
				manu_cut_line.endPt+=offset_vec*pStartVertex->manu_space;
				vertex_list[i]=manu_cut_line.startPt;
				vertex_list[(i+1)%n]=manu_cut_line.endPt;
			}
			else
			{
				f3dArcLine arcline;
				if(pStartVertex->type==1)
					arcline.CreateMethod2(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,1),pStartVertex->sector_angle);
				else if(pStartVertex->type==2)
				{
					f3dPoint center_pick=pStartVertex->center;
					coord_trans(center_pick,pPlank->ucs,FALSE);
					arcline.CreateMethod3(pStartVertex->vertex,pEndVertex->vertex,f3dPoint(0,0,1),pStartVertex->radius,center_pick);
				}
				else if(pStartVertex->type==3)
				{
					f3dPoint center_pick=pStartVertex->center;
					f3dPoint column_norm=pStartVertex->column_norm;
					coord_trans(center_pick,pPlank->ucs,FALSE);
					vector_trans(column_norm,pPlank->ucs,FALSE);
					arcline.CreateEllipse(center_pick,pStartVertex->vertex,pEndVertex->vertex,column_norm,f3dPoint(0,0,1),pStartVertex->radius);
				}
				//��Բ������ƫ�Ƴ�һ��ͬ�ļӹ���϶Բ
				int sign=1;
				if(arcline.WorkNorm().z<0)
					sign=-1;
				f3dPoint center=arcline.Center();
				offset_vec=arcline.Start()-center;
				normalize(offset_vec);
				manu_cut_line.startPt=arcline.Start()-offset_vec*(sign*pStartVertex->manu_space);
				offset_vec=arcline.End()-center;
				normalize(offset_vec);
				manu_cut_line.endPt=arcline.End()-offset_vec*(sign*pStartVertex->manu_space);
				vertex_list[i]=manu_cut_line.startPt;
				vertex_list[(i+1)%n]=manu_cut_line.endPt;
			}
		}
	}*/
	//ȷ���ӹ�����ϵ
	f3dPoint prevPnt = vertex_list.GetFirst()->vertex;
	f3dPoint prev_vec,cursor_vec;
	double prev_edge_dist=0;
	int bottom_edge_i=0;
	i=0;
	for(pVertex=vertex_list.GetNext();pVertex;pVertex=vertex_list.GetNext())
	{
		cursor_vec=pVertex->vertex-prevPnt;
		normalize(cursor_vec);
		edge_dist = DISTANCE(prevPnt,pVertex->vertex);
		if(prev_vec*cursor_vec>EPS_COS)	//�������߱�����
			edge_dist+=edge_dist+prev_edge_dist;
		if(edge_dist>max_edge)
		{
			max_edge = edge_dist;
			machine_ucs.axis_x = pVertex->vertex-prevPnt;
			machine_ucs.origin = prevPnt;
			bottom_edge_i=i;
		}
		prev_edge_dist=edge_dist;
		prevPnt = pVertex->vertex;
		prev_vec=cursor_vec;
		i++;
	}
	//ȷ�����һ��
	pVertex = vertex_list.GetFirst();
	edge_dist = DISTANCE(prevPnt,pVertex->vertex);
	cursor_vec=pVertex->vertex-prevPnt;
	normalize(cursor_vec);
	if(prev_vec*cursor_vec>EPS_COS)	//�������߱�����
		edge_dist+=edge_dist+prev_edge_dist;
	if(edge_dist>max_edge)
	{
		max_edge = edge_dist;
		machine_ucs.axis_x = pVertex->vertex-prevPnt;
		machine_ucs.origin = prevPnt;
		bottom_edge_i=i;
	}
	NoPos = machine_ucs.origin+f3dPoint(20,20);

	machine_ucs.axis_x.z=0;
	normalize(machine_ucs.axis_x);
	machine_ucs.axis_y.Set(-machine_ucs.axis_x.y,machine_ucs.axis_x.x);
	max_edge = 0;
	//ȷ����������λ��
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		vertice_ref = pVertex->vertex;
		coord_trans(vertice_ref,machine_ucs,FALSE);
		if(vertice_ref.x<max_edge)
			max_edge = vertice_ref.x;
	}
	machine_ucs.origin += machine_ucs.axis_x*max_edge;
	fPtList processHoleList;				//���տ�
	CHashSet<CLDSBolt*> hashInvalidBoltSet;	//��Ч��˨������Ҫ������¼ʮ�ֲ���ϲ��ڻ����ϵ���˨ 
	//ͳ�Ʋ������ְ��ϵķ���˨�����γɵĿ�,�ų�ʮ�ֲ�岻�ڸְ巶Χ�ڵ���Ч��˨����	wht 15-12-31
	if(pPlank->GetClassTypeId()==CLS_PARAMPLATE)
	{
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPlank;
		if(pParamPlate->m_iParamType==TYPE_ELBOWPLATE)
		{
			long L=0,K=0,V=0;
			double H=0,G=0,F=0;
			pParamPlate->GetDesignItemValue('H',&H);
			pParamPlate->GetDesignItemValue('G',&G);
			pParamPlate->GetDesignItemValue('F',&F);
			if(H>0)	//������
			{
				f3dPoint line_vec;
				CLDSPart *pBottomPlate=NULL;
				CLDSLineTube *pDatumTube=NULL;
				if(!pParamPlate->GetDesignItemValue('V',&V))
					V=0;	//�Ƿ�Ϊ��ֱ���Զ���λ���߰�
				if(pParamPlate->GetDesignItemValue('L',&L))	//��û�׼�װ�
					pBottomPlate=pParamPlate->BelongModel()->FromPartHandle(L,CLS_PLATE,CLS_PARAMPLATE);
				if(pParamPlate->GetDesignItemValue('K',&K))	//��׼�ֹ�
				{
					pDatumTube=(CLDSLineTube*)pParamPlate->BelongModel()->FromPartHandle(abs(K),CLS_LINETUBE);
					if(pDatumTube&&K>0)
						line_vec=pDatumTube->End()-pDatumTube->Start();
					else if(pDatumTube&&K<0)
						line_vec=pDatumTube->Start()-pDatumTube->End();
				}
				else
				{
					pParamPlate->GetDesignItemValue('x', &line_vec.x);
					pParamPlate->GetDesignItemValue('y', &line_vec.y);
					pParamPlate->GetDesignItemValue('z', &line_vec.z);
				}
				normalize(line_vec);
				vector_trans(line_vec,pParamPlate->ucs,FALSE);
				line_vec.z=0;
				if(V&&pBottomPlate&&pDatumTube)
				{
					f3dPoint vertex=pParamPlate->vertex_list.GetTail()->vertex;
					line_vec.Set(vertex.x,vertex.y,0);
					normalize(line_vec);
				}
				double k=0;
				if(line_vec.y>eps)//��ֹline_vec.y=0 
					k=line_vec.x/line_vec.y;
				f3dPoint *pPt=processHoleList.append();
				pPt->Set(G+F*k,F,0);
				pPt->feature=(int)H;	//���տ�ֱ��
			}
		}
		else if(pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)
		{
			double d=0,P=0;	
			if (pParamPlate->GetDesignItemValue('d',&d)&&
				pParamPlate->GetDesignItemValue('P',&P)&&P>0&&d>0)
			{
				long A;
				pParamPlate->GetDesignItemValue('A',&A);
				CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pParamPlate->BelongModel()->FromPartHandle(A,CLS_LINEANGLE);
				if(pLineAngle)
				{
					double alfa=pLineAngle->GetRadWingAngle();
					double sina=sin(alfa);
					double cosa=cos(alfa);
					double c=0;
					P+=pLineAngle->GetThick();
					if(fabs(cosa)>EPS_COS)
					{	//�Է����ֳ�0
						c=P;
						P=0;
					}
					else
						c=P*sina/(1-cosa);
					f3dPoint datum_pos;
					f3dPoint line_vec=pLineAngle->End()-pLineAngle->Start();
					normalize(line_vec);
					Int3dlf(datum_pos,pLineAngle->Start(),line_vec,pParamPlate->ucs.origin,pParamPlate->ucs.axis_z);
					f3dPoint centre=datum_pos+pLineAngle->GetWingVecY()*c-pLineAngle->get_norm_y_wing()*P;
					coord_trans(centre,pParamPlate->ucs,FALSE);
					f3dPoint *pPt=processHoleList.append();
					pPt->Set(centre.x,centre.y,0);
					pPt->feature=(int)d;	//���տ�ֱ��
				}
			}
		}
		else if(pParamPlate->m_iParamType==TYPE_XEND)
		{	//ʮ�ֲ��
			int i=0;
			POLYGON polygon;
			DYN_ARRAY<f3dPoint> pt_arr(pParamPlate->vertex_list.GetNodeNum());
			for(pVertex=pParamPlate->vertex_list.GetFirst();pVertex;pVertex=pParamPlate->vertex_list.GetNext(),i++)
				pt_arr[i]=pVertex->vertex;
			polygon.CreatePolygonRgn(pt_arr,pt_arr.Size());
			for(CLsRef *pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(fabs(pBolt->get_norm()*pParamPlate->ucs.axis_z)<EPS_COS2)
					hashInvalidBoltSet.SetValue(pBolt->handle,pBolt);
				else
				{
					f3dPoint ls_pos=pBolt->ucs.origin;
					coord_trans(ls_pos,pParamPlate->ucs,FALSE);
					ls_pos.z=0;
					if(polygon.PtInRgn(ls_pos)!=1)
						hashInvalidBoltSet.SetValue(pBolt->handle,pBolt);
				}
			}
		}
	}
//----------------VVVVVVVVVVVVVV---����NC����---VVVVVVVVVVVVVV----------------------
	CBuffer buffer;
	buffer.WriteByte(100);	//�°�֧�ְָ�Բ����wjh-2014.9.17
	buffer.WriteString(pPlank->GetPartNo());
	buffer.WriteInteger((int)pPlank->GetThick());
	buffer.WriteByte(pPlank->cMaterial);
	UCS_STRU ucs;	//������������������ϵ
	LoadDefaultUCS(&ucs);
	buffer.WritePoint(ucs.origin);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	buffer.WriteInteger(bottom_edge_i);
	int hole_count=pPlank->GetLsCount()+processHoleList.GetNodeNum()-hashInvalidBoltSet.GetNodeNum();
	BUFFERPOP stack(&buffer,hole_count);
	buffer.WriteInteger(hole_count);
	for(CLsRef *pLsRef=pPlank->GetFirstLsRef();pLsRef;pLsRef=pPlank->GetNextLsRef())
	{
		if(hashInvalidBoltSet.GetValue((*pLsRef)->handle))
			continue;
		pPlank->SpreadLsPos(pLsRef->GetLsPtr(),ls_pos);
		if(bExportDeformedProfile)
		{
			int iLsFace=pPlank->GetNormFaceI((*pLsRef)->get_norm());
			ls_pos.feature=iLsFace+1;
			ls_pos=pPlank->GetDeformedVertex(ls_pos);
		}
		ls_pos.z = 0;		//�������㣬��Ȼ��˨��������㲻׼
		coord_trans(ls_pos,machine_ucs,FALSE);
		buffer.WriteInteger((*pLsRef)->get_d());
		buffer.WriteDouble((*pLsRef)->hole_d_increment);
		buffer.WriteDouble(ls_pos.x);
		buffer.WriteDouble(ls_pos.y);
		stack.Increment();
	}
	//���������Ϲ��տ�
	for(f3dPoint *pPt=processHoleList.GetFirst();pPt;pPt=processHoleList.GetNext())
	{
		f3dPoint ls_pos=*pPt;
		coord_trans(ls_pos,machine_ucs,FALSE);
		buffer.WriteInteger(pPt->feature);	//��˨ֱ��
		buffer.WriteDouble(0);
		buffer.WriteDouble(ls_pos.x);
		buffer.WriteDouble(ls_pos.y);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef  AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The 0X%X Plate's hole count is wrong!",pPlank->handle);
#else 
		CLDSObject::Log2File()->Log("�ְ�0X%X�׼�¼������!",pPlank->handle);
#endif
	//������
	buffer.WriteInteger(vertex_list.GetNodeNum());
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		buffer.WriteByte(pVertex->type);
		f3dPoint point =pVertex->vertex;
		if(bExportDeformedProfile)	//���ǻ�������
			point=pPlank->GetDeformedVertex(point);
		coord_trans(point,machine_ucs,FALSE);
		buffer.WritePoint(point);
		if(pVertex->type==1)	//ָ��Բ���н�
			buffer.WriteDouble(pVertex->sector_angle);
	    else if(pVertex->type==2||pVertex->type==3)
		{
			buffer.WriteDouble(pVertex->radius);
			point = pVertex->center;
			if(pVertex->type==2)	//ָ��Բ���뾶ʱ��Բ������ʰȡ��
			{
				coord_trans(point,machine_ucs,FALSE);
				buffer.WritePoint(point); //ָ���뾶�豣��Բ������ 
			}
			else //if(pVertex->type==3)
			{	//��Բ������Ҫ�Ĳ�������Զ�����������
				//Բ������������ʰȡ��
				coord_trans(point,pPlank->ucs,FALSE);	//ģ��(����)����ϵת���ְ�װ������ϵ
				coord_trans(point,machine_ucs,FALSE);	//�ɸְ�װ������ϵת���ְ�ӹ�����ϵ��
				buffer.WritePoint(point); //ָ���뾶�豣��Բ������ 
				//Բ��������
				point=pVertex->column_norm;
				vector_trans(point,pPlank->ucs,FALSE);
				vector_trans(point,machine_ucs,FALSE);
				buffer.WritePoint(point);
			}
		}
	}
	//���λ��
	coord_trans(NoPos,machine_ucs,FALSE);
	buffer.WriteDouble(NoPos.x+30);
	buffer.WriteDouble(NoPos.y+15);

	long file_len=buffer.GetLength();
	fwrite(&file_len,sizeof(long),1,fp);
	fwrite(buffer.GetBufferPtr(),buffer.GetLength(),1,fp);
	fclose(fp);
	return TRUE;
}
#endif
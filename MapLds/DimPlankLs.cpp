// DimPlankLs.cpp: implementation of the CDimPlankLs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdarx.h"
#include "DimPlankLs.h"
#include "PlankDrawing.h"
#include "Query.h"
#include "SysPara.h"
#include "DrawUnit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifndef __TSA_FILE_
CDimPlankLs::CDimPlankLs()
{

}

CDimPlankLs::~CDimPlankLs()
{

}
//�ӱ���˨�˾�(��Ҫ���ǽ�һ�������ߴ�ֱ�Ĵ���)
void CDimPlankLs::DimLsEndSpace(CLDSPlate *pPlank)
{
	LSRAY_STRU ls_ray;
	f3dLine ray_line,edge_line,line;
	int i,j,k,nDimCount;
	if(pPlank==NULL)
		return;
	nDimCount = ls_ray_arr.GetSize();
	for(i=0;i<nDimCount;i++)
	{
		int n = ls_ray_arr[i].ls_arr.GetNodeNum();
		if(n<2)	//��ֻ��һ����˨
			continue;
		else
		{
			f3dPoint perp;
			int nn = pPlank->vertex_list.GetNodeNum();
			ray_line.startPt = ls_ray_arr[i].ls_arr[0];
			ray_line.endPt = ls_ray_arr[i].ls_arr[1];
			for(k=0;k<nn;k++)
			{
				edge_line.startPt = pPlank->vertex_list[k].vertex;
				edge_line.endPt = pPlank->vertex_list[(k+1)%nn].vertex;
				if(!IsPerpToLine(ray_line,edge_line,perp,0.01))
					continue;	//�˱߽��߲�����˨���ߴ�ֱ���Թ�
				perp.feature = -1;	//��ʾ�߽罻�㲻����˨��
				line.startPt = ls_ray_arr[i].ls_arr[0];
				for(j=1;j<n;j++)
				{
					line.endPt = ls_ray_arr[i].ls_arr[j];
					int iRetCode = line.PtInLine(perp,EPS2);
					if(iRetCode==-2)		//������
					{
						if(DISTANCE(line.startPt,perp)>80)
							break;
						else
						{
							ls_ray_arr[i].ls_arr.insert(perp,j-1);
							break;
						}
					}
					else if(iRetCode==2)	//λ������˨֮��
					{
						//ls_ray_arr[i].ls_arr.insert(perp,j);
						break;
					}
					//line.startPt = line.endPt;
				}
				if(j==n&&DISTANCE(line.endPt,perp)<80)//&&line.PtInLine(ls_centre,EPS2)==-1)
					ls_ray_arr[i].ls_arr.append(perp);
			}
		}
	}
}
//��ע��˨���߼����ϵ���˨���
void CDimPlankLs::DimLsRay(CLDSPart *pPlate/*=NULL*/)
{
	int i,j,n;
	ALIGNDIM_STRU align_dim;
	TXTDIM_STRU text_dim;
	n = ls_ray_arr.GetSize();
	LSRAY_STRU *pLsRay;
	for(i=0;i<n;i++)
	{
		f3dPoint vec,norm;
		pLsRay = &ls_ray_arr[i];
		sprintf(text_dim.dimText,"%dM%dX%d",ls_ray_info_arr[i].N,
			ls_ray_info_arr[i].d,ls_ray_info_arr[i].L);
		//ȱʡ��˨�ı���Ϣ����λ��
		text_dim.dimVec = pLsRay->ls_arr[0];
		normalize(text_dim.dimVec);//չ�������Ӱ���ƽ����������ϵΪ��׼
		text_dim.dimPos = pLsRay->ls_arr[0];//ȱʡ�Ե�һ��˨�����׼���ʸ��Ϊ��׼
		text_dim.dimPos.x-=20;
		text_dim.dimPos.y-=20;
		if(pLsRay->ls_arr.GetNodeNum()>1)
		{
			/*norm.Set(0,0,1);
			Sub_Pnt(vec,pLsRay->ls_arr[1],pLsRay->ls_arr[0]);
			norm = cross_prod(norm,vec);
			normalize(vec);
			normalize(norm);
			text_dim.dimVec = vec;	//չ�������Ӱ���ƽ����������ϵΪ��׼
			text_dim.dimPos = pLsRay->ls_arr[pLsRay->ls_arr.GetNodeNum()-1];
			Add_Pnt(text_dim.dimPos,text_dim.dimPos,vec*20);
			Sub_Pnt(text_dim.dimPos,text_dim.dimPos,norm*15);
			text_dim_arr.Add(text_dim);*/

			align_dim.dist=0;
			align_dim.endPos = pLsRay->ls_arr[0];
			align_dim.nLsSpaceNum=1;
			double fPrevDist=0;
			for(j=1;j<pLsRay->ls_arr.GetNodeNum();j++)
			{
				align_dim.startPos = align_dim.endPos;
				align_dim.endPos = pLsRay->ls_arr[j];
				align_dim.dist = DISTANCE(align_dim.startPos,align_dim.endPos);
				if(!m_bDimStdBoltSpace)
				{
					LSSPACE_STRU LsSpace;
					if(align_dim.startPos.feature==align_dim.endPos.feature)
					{	//��ͬ��˨���
						if( GetLsSpace(LsSpace,align_dim.startPos.feature)
							&&(fabs(LsSpace.SingleRowSpace-align_dim.dist)<0.5
							||(pLsRay->bIsDoubleLsRay	//��ǰ�˼�Ϊ˫����˨ʱ����Ҫ�ж�˫����˨��� wht 11-06-30
							&&fabs(LsSpace.doubleRowSpace-align_dim.dist)<0.5)))
							continue;	//��׼��˨��಻����
					}
					else if(align_dim.startPos.feature>0)
					{
						if(GetLsSpace(LsSpace,align_dim.startPos.feature))
						{
							if(fabs(LsSpace.EndSpace-align_dim.dist)<3)
								continue;	//��׼��˨�˾�
						}
					}
					else if(align_dim.endPos.feature>0)
					{
						if(GetLsSpace(LsSpace,align_dim.endPos.feature))
						{
							if(fabs(LsSpace.EndSpace-align_dim.dist)<3)
								continue;	//��׼��˨�˾�
						}
					}
				}
				if (j>1 && ftoi(fPrevDist)==ftoi(align_dim.dist) && align_dim_arr.GetSize()>0)
				{
					align_dim.nLsSpaceNum++;
					ALIGNDIM_STRU* pDim=&align_dim_arr[align_dim_arr.GetSize()-1];
					pDim->endPos=align_dim.endPos;
					pDim->nLsSpaceNum=align_dim.nLsSpaceNum;
					align_dim.startPos=pDim->startPos;
				}
				else
				{
					align_dim.nLsSpaceNum=1;
					fPrevDist=align_dim.dist;
					align_dim_arr.Add(align_dim);
				}

				CLsSpaceDimUnit dimspace;
				dimspace.nLsSpaceNum=align_dim.nLsSpaceNum;
				dimspace.dist=align_dim.dist;
				dimspace.hStartLs=align_dim.startPos.ID;
				dimspace.hEndLs=align_dim.endPos.ID;
				if(pPlate)	//��¼��˨�����ְ��� wht 11-07-23 
					dimspace.hFatherPlate=pPlate->handle;
				LSSPACEDIM.append(dimspace);
				if (dimspace.nLsSpaceNum>1)	//ɾ��ǰһ����¼
					LSSPACEDIM.DeleteAt(LSSPACEDIM.GetNodeNum()-2);
				/*Sub_Pnt(align_dim.dimPos,align_dim.startPos,
					align_dim.endPos);
				Add_Pnt(align_dim.dimPos,align_dim.startPos,
					align_dim.dimPos*0.5);
				Add_Pnt(align_dim.dimPos,align_dim.dimPos,
					norm*80);
				align_dim_arr.Add(align_dim);*/
			}
		}
		else
			text_dim_arr.Add(text_dim);
	}
}
void CDimPlankLs::AnalizeLsRay(f3dPoint ls_centre,int iRayNo,char *guige)
{
	LSRAY_STRU ls_ray;
	HOLERAYINFO_STRU ls_ray_info;
	f3dLine line;
	int i,j,nDimCount = ls_ray_arr.GetSize();
	for(i=0;i<nDimCount;i++)
	{
		if(ls_ray_arr[i].iRayNo==iRayNo)
		{
			ls_ray_info_arr[i].N++;	//��˨������1

			int n = ls_ray_arr[i].ls_arr.GetNodeNum();
			if(n<2)	//��ֻ��һ����˨
				ls_ray_arr[i].ls_arr.append(ls_centre);
			else
			{
				line.startPt = ls_ray_arr[i].ls_arr[0];
				for(j=1;j<n;j++)
				{
					line.endPt = ls_ray_arr[i].ls_arr[j];
					int iRetCode = line.PtInLine(ls_centre,EPS2);
					if(iRetCode==-2)		//������
					{
						ls_ray_arr[i].ls_arr.insert(ls_centre,j-1);
						break;
					}
					else if(iRetCode==2)	//λ������˨֮��
					{
						ls_ray_arr[i].ls_arr.insert(ls_centre,j);
						break;
					}
					line.startPt = line.endPt;
				}
				if(j==n)//&&line.PtInLine(ls_centre,EPS2)==-1)
					ls_ray_arr[i].ls_arr.append(ls_centre);
			}
			break;
		}
	}
	if(i==nDimCount)	//��ʱ��û�д������ϵ���˨����ӵ�ǰ����
	{
		CLDSBolt ls(&Ta);
		restore_Ls_guige(guige,ls);
		ls_ray.iRayNo = iRayNo;
		ls_ray_info.d = ls.get_d();
		ls_ray_info.L = (int)ls.get_L();
		ls_ray_info.N = 1;
		ls_ray_info.iRayNo = iRayNo;
		ls_ray_info_arr.Add(ls_ray_info);
		ls_ray_arr.Add(ls_ray);
		i = ls_ray_arr.GetUpperBound();
		ls_ray_arr[i].ls_arr.append(ls_centre);
	}
}
BOOL CDimPlankLs::Create(CLDSParamPlate *pPlate,BOOL bDimStdBoltSpace/*=FALSE*/)
{
	CLsRef *pLsRef;
	ALIGNDIM_STRU align_dim;
	char sGuiGe[MAX_CHAR_GUIGE_16+1];
	f3dPoint basePt;
	if(pPlate==NULL)
		return FALSE;
	m_bDimStdBoltSpace=bDimStdBoltSpace;
	for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		sprintf(sGuiGe,"%sM%dX%.f",(*pLsRef)->Grade(),(*pLsRef)->get_d(),(*pLsRef)->get_L());
		Int3dlf(basePt,(*pLsRef)->ucs.origin,(*pLsRef)->ucs.axis_z,pPlate->ucs.origin,pPlate->ucs.axis_z);
		coord_trans(basePt,pPlate->ucs,FALSE);
		basePt.feature = (*pLsRef)->get_d();
		basePt.ID=(*pLsRef)->handle;
		for(int i=1;i<=32;i++)
		{
			if(pLsRef->dwRayNo&GetSingleWord(i))
				AnalizeLsRay(basePt,i,sGuiGe);
		}
	}
	if(sys.stru_map.bDimSpecialLsSpace)
	{
		//if(sys.stru_map.bDimSpecialLsEndSpace)
		//	DimLsEndSpace(pPlank);	//������ע��˨����˾�
		DimLsRay(pPlate);
	}
	return TRUE;
}
BOOL CDimPlankLs::Create(CLDSPlate *pPlate,BOOL bDimStdBoltSpace/*=FALSE*/)
{
	CLsRef *pLsRef;
	ALIGNDIM_STRU align_dim;
	char sGuiGe[MAX_CHAR_GUIGE_16+1];
	f3dPoint basePt;
	if(pPlate==NULL)
		return FALSE;
	m_bDimStdBoltSpace=bDimStdBoltSpace;
	for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
	{
		sprintf(sGuiGe,"%sM%dX%.f",(*pLsRef)->Grade(),(*pLsRef)->get_d(),(*pLsRef)->get_L());
		basePt = (*pLsRef)->ucs.origin;
		pPlate->SpreadLsPos(pLsRef,basePt);
		basePt.feature = (*pLsRef)->get_d();
		basePt.ID=(*pLsRef)->handle;
		for(int i=1;i<=32;i++)
		{
			if(pLsRef->dwRayNo&GetSingleWord(i))
				AnalizeLsRay(basePt,i,sGuiGe);
		}
	}
	if(sys.stru_map.bDimSpecialLsSpace)
	{
		//if(sys.stru_map.bDimSpecialLsEndSpace)
		//	DimLsEndSpace(pPlank);	//������ע��˨����˾�

		//�ж���˨�����ϵ���˨�Ƿ�Ϊ˫����˨ wht 11-06-30
		CHashTable<int> rayNoTable;			//��λ����Ϊkey ���ߺ�Ϊ����
		CHashTable<int> doubleLsRayNoTable;	//key�����ݾ�Ϊ���ߺ�,������¼˫����˨���ߺ�
		rayNoTable.CreateHashTable(50);
		doubleLsRayNoTable.CreateHashTable(32);
		for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			long hPart=pLsRef->GetLsPtr()->des_base_pos.hPart;
			int iRayNo=0;
			for(int i=1;i<=32;i++)
			{
				if(pLsRef->dwRayNo&GetSingleWord(i))
				{
					if(iRayNo==0)
						iRayNo=i;
					else if(iRayNo>0)
					{	//����˨�����ж�����ߺ�
						iRayNo=0;
						break;
					}
				}
			}
			if(iRayNo==0)
				continue;	//�������ж�����ߺŵ���˨����(һ�㶼�ɶ��������)
			int iFirstRayNo=0;
			if(rayNoTable.GetValueAt(hPart,iFirstRayNo))
			{
				if(iFirstRayNo>0&&iFirstRayNo!=iRayNo)
				{	//ͬһ�˼��ϴ��ڴ��ڶ�����˨���ߣ��ø˼��ϵ���˨Ϊ˫����˨
					int nTemp=0;
					if(!doubleLsRayNoTable.GetValueAt(iRayNo,nTemp))
						doubleLsRayNoTable.SetValueAt(iRayNo,iRayNo);
					if(!doubleLsRayNoTable.GetValueAt(iFirstRayNo,nTemp))
						doubleLsRayNoTable.SetValueAt(iFirstRayNo,iFirstRayNo);
				}
			}
			else //δ����
				rayNoTable.SetValueAt(hPart,iRayNo);
		}
		for(int i=0;i<ls_ray_arr.GetSize();i++)
		{
			int nTemp=0;
			if(doubleLsRayNoTable.GetValueAt(ls_ray_arr[i].iRayNo,nTemp))
				ls_ray_arr[i].bIsDoubleLsRay=TRUE;
		}
		DimLsRay(pPlate);
	}
	return TRUE;
}
int CDimPlankLs::GetAlignDimCount()
{
	return align_dim_arr.GetSize();
}
int CDimPlankLs::GetLsRayCount()
{
	return ls_ray_arr.GetSize();
}
int CDimPlankLs::GetTextDimCount()
{
	return text_dim_arr.GetSize();
}
BOOL CDimPlankLs::GetAlignDimAt(int index, ALIGNDIM_STRU &dim)
{
	if(index<0||index>align_dim_arr.GetUpperBound())
		return FALSE;
	dim = align_dim_arr[index];
	return TRUE;
}
BOOL CDimPlankLs::GetTextDimAt(int index, TXTDIM_STRU&dim)
{
	if(index<0||index>text_dim_arr.GetUpperBound())
		return FALSE;
	dim = text_dim_arr[index];
	return TRUE;
}
#endif
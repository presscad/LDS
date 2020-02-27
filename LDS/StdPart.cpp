#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CTMAStdPart::FromBuffer(CBuffer &buffer,char *version/*=NULL*/)
{
}
void CTMAStdPart::ToBuffer(CBuffer &buffer)
{
	;
}
BOOL CTMAStdPart::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/)
{
	int i,j;
	double A,B,C,D,H,K,L,N,R,S,H1,n;
	if(pBody==NULL)
		pBody = new fBody;
	else if(!is_data_modified)	//数据未修改
		return TRUE;
	else
		pBody->Empty();
	f3dPoint vertice;
	if(m_iStdPartType==STDPART_FLD)
	{
		/*GetDesignItemValue('A',&A);
		GetDesignItemValue('B',&B);
		GetDesignItemValue('C',&C);
		GetDesignItemValue('D',&D);
		GetDesignItemValue('H',&H);
		GetDesignItemValue('K',&K);
		GetDesignItemValue('L',&L);
		GetDesignItemValue('N',&N);
		GetDesignItemValue('R',&R);
		GetDesignItemValue('S',&S);
		GetDesignItemValue('I',&H1);
		GetDesignItemValue('O',&n);*/
		A=275;
		B=259;
		C=20;
		D=400;
		H=62;
		K=350;
		L=26;
		N=299;
		R=10;
		S=8;
		H1=18;
		n=16;
		f3dPoint vertex_arr[9];
		double outter_radius=0.5*D;
		vertex_arr[0].Set(0.5*B);
		vertex_arr[1].Set(outter_radius);
		vertex_arr[2].Set(outter_radius,0,C);
		vertex_arr[3].Set(0.5*N,0,C);
		f3dPoint up_c(0.5*B+S+R,0,H-H1),down_c(0.5*N,0,C+R);
		f3dPoint axis_vec=up_c-down_c;
		f3dPoint N_vec(-axis_vec.z,0,axis_vec.x);
		normalize(N_vec);
		vertex_arr[4]=down_c+N_vec*R;
		vertex_arr[5]=up_c+N_vec*R;
		vertex_arr[6].Set(0.5*B+S,0,H-H1);
		vertex_arr[7].Set(0.5*B+S,0,H);
		vertex_arr[8].Set(0.5*B,0,H);
		int uSlices = CalArcResolution(outter_radius,2*Pi,scale_of_user2scr,sample_len);
		uSlices = max(uSlices,6);
		double piece_angle=2*Pi/uSlices;
		double *cosa_arr=new double[uSlices];
		double *sina_arr=new double[uSlices];
		for(j=0;j<uSlices;j++)
		{
			double angle=j*piece_angle;
			cosa_arr[j]=cos(angle);
			sina_arr[j]=sin(angle);
		}
		for(i=0;i<9;i++)
		{
			for(j=0;j<uSlices;j++)
			{
				vertice.Set(vertex_arr[i].x*cosa_arr[j],vertex_arr[i].x*sina_arr[j],vertex_arr[i].z);
				pBody->vertex.append(vertice);
			}
		}
		delete[] cosa_arr;
		delete[] sina_arr;
		COLORREF fl_color=RGB(100,100,100);
		f3dPolyFace *pFLDownFace=pBody->faceList.append();
		pFLDownFace->material=fl_color;
		for(j=uSlices-1;j>=0;j--)
			pBody->NewEdgeLine(pFLDownFace,(j+uSlices-1)%uSlices+uSlices,j+uSlices);
		fLoop *pInnerLoop=pFLDownFace->inner_loop.append();
		for(j=0;j<uSlices;j++)
		{
			f3dPoint *pStart=pBody->vertex.GetByIndex(j);
			f3dPoint *pEnd=pBody->vertex.GetByIndex((j+1)%uSlices);
			pInnerLoop->append(pStart,pEnd);
		}
		f3dPolyFace *pFLUpFace=pBody->faceList.append();
		pFLUpFace->material=fl_color;
		for(j=0;j<uSlices;j++)
			pBody->NewEdgeLine(pFLUpFace,(j+1)%uSlices+2*uSlices,j+2*uSlices);
		pInnerLoop=pFLUpFace->inner_loop.append();
		for(j=uSlices-1;j>=0;j--)
		{
			f3dPoint *pStart=pBody->vertex.GetByIndex(j+3*uSlices);
			f3dPoint *pEnd=pBody->vertex.GetByIndex((j+uSlices-1)%uSlices+3*uSlices);
			pInnerLoop->append(pStart,pEnd);
		}
		f3dPolyFace *pNeckFlatFace=pBody->faceList.append();
		pNeckFlatFace->material=fl_color;
		for(j=0;j<uSlices;j++)
			pBody->NewEdgeLine(pNeckFlatFace,(j+1)%uSlices+7*uSlices,j+7*uSlices);
		pInnerLoop=pNeckFlatFace->inner_loop.append();
		for(j=uSlices-1;j>=0;j--)
		{
			f3dPoint *pStart=pBody->vertex.GetByIndex(j+8*uSlices);
			f3dPoint *pEnd=pBody->vertex.GetByIndex((j+uSlices-1)%uSlices+8*uSlices);
			pInnerLoop->append(pStart,pEnd);
		}
		int k_arr[6]={1,3,4,5,6,8};
		for(i=0;i<6;i++)
		{
			for(j=0;j<uSlices;j++)
			{
				f3dPolyFace *pFace=pBody->faceList.append();
				pFace->material=fl_color;
				pBody->NewEdgeLine(pFace,(j+1)%uSlices+k_arr[i]*uSlices,j+k_arr[i]*uSlices);
				if(k_arr[i]<8)
				{
					pBody->NewEdgeLine(pFace,(j+1)%uSlices+k_arr[i]*uSlices+uSlices);
					pBody->NewEdgeLine(pFace,j+k_arr[i]*uSlices+uSlices);
				}
				else
				{
					pBody->NewEdgeLine(pFace,(j+1)%uSlices);
					pBody->NewEdgeLine(pFace,j);
				}
				pBody->NewEdgeLine(pFace,j+k_arr[i]*uSlices);
			}
		}
		CHoleMaker hole_maker;
		UCS_STRU hole_ucs;
		LoadDefaultUCS(&hole_ucs);
		piece_angle=2*Pi/n;
		for(j=0;j<n;j++)
		{
			double angle=j*piece_angle;
			hole_ucs.origin.Set(0.5*K*cos(angle),0.5*K*sin(angle));
			hole_maker.Create(hole_ucs,L,0,pFLUpFace,pFLDownFace,pBody,CTMAPart::crBolt24);
		}
		for(f3dPoint *pVertex=pBody->vertex.GetFirst();pVertex;pVertex=pBody->vertex.GetNext())
		{
			if(!coord_trans(*pVertex,ucs,TRUE,TRUE))
			{
				char sError[100];
				sprintf(sError,"句柄为0X%X的标准件｛对焊法兰｝实体坐标转换出错！",handle);
				throw sError;
			}
		}
	}
	else if(m_iStdPartType==STDPART_FLP)
	{
	}
	else if(m_iStdPartType==STDPART_RABBETC)
	{
	}
	else if(m_iStdPartType==STDPART_RABBETU)
	{
	}
	else if(m_iStdPartType==STDPART_RABBETX)
	{
	}
	return FALSE;
}
BOOL CTMAStdPart::GetDesignItemValue(char cKey, double *pfValue)
{
	return 0;
}
BOOL CTMAStdPart::GetDesignItemValue(char cKey, long *piVal)
{
	return 0;
}
char* CTMAStdPart::GetDesignItemNotes(char cKey)
{
	return "";
}
BOOL CTMAStdPart::IsHasDesignItemValue(char cKey)
{
	return 0;
}
BOOL CTMAStdPart::DeleteDesignItem(char cKey)
{
	return 0;
}
int CTMAStdPart::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	return 0;
}
double CTMAStdPart::GetWidth()
{
	return 0;
}
double CTMAStdPart::GetLength()
{
	return 0;
}
double CTMAStdPart::GetPieceWeight()
{
	return 0;
}
BOOL CTMAStdPart::SetDesignItemValue(char cKey,double fValue,char *sNotes/*=NULL*/,int iDatatype/*=0*/,BOOL bModify/*=FALSE*/)
{
	return 0;
}
//属性管理
int CTMAStdPart::GetPropValueStr(long id,char* valueStr)	//根据属性ID得到属性值
{
	return 0;
}

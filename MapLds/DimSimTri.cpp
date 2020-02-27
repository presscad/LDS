
//  定义函数 ---- SimilarTriangleAnnote            相似三角形标注
#include "stdafx.h"
#include "TMA.h"
#include "MenuFunc.h"
#include "DrawTaMap.h"
#include "GlobalVAR.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "adstools.h"
#include "rxtools.h"
#include "rxtools.h"
#include "SysPara.h"
#include "Tower.h"
#include "dbpl.h"

void DimSimPoly()//int iFace)
{
#ifdef aslkf
	int i;
	ads_point base_pnt;
	ads_name seqent,sss;
	struct resbuf *plist;
	f3dPoint f_start,f_end,vec,old_vec;
	fPtList vertex;
	double scale = g_sysPara.StruMapScale;

//---------------------------------------------------------	

	AcDbObjectId layerId,textStyleId;
	GetTextStyleId(textStyleId,sys.dim_map.fPartGuigeTextSize);
	ads_command(RTSTR,"undo",RTSTR,"g",RTNONE);
	ads_command(RTSTR,"layer",RTSTR,"F",RTSTR,"*",RTSTR,"",RTNONE);
	ads_command(RTSTR,"layer",RTSTR,"T",RTSTR,"4_front,4_side,4_RANDOM,3",RTSTR,"s",RTSTR,"SimTri",RTSTR,"",RTNONE);
	
	for(;;)
	{
		
		if (ads_getpoint(NULL,"\n请点三角形区域内任一点,<Enter退出>: ",base_pnt)==RTNORM)
		{
			int resCode = ads_command(RTSTR,"bpoly",RTPOINT,base_pnt,RTSTR,"",RTNONE);
			//ads_command(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
			ads_ssadd(NULL,NULL,sss);
			
			AcGePoint3d location;
			int nn,vertexNumber;
			if (resCode == RTNORM)
			{
				ads_entlast(seqent);
				//遍历Polyline顶点
				AcDbObjectId plineId;
				acdbGetObjectId(plineId,seqent);
				AcDbPolyline *pPline;
				acdbOpenObject(pPline,plineId,AcDb::kForRead);
				if(pPline==NULL)
				{
					AfxMessageBox("选择了错误的边界,请重新选择");
					continue;
				}
				pPline->close();
				plist=ads_buildlist(RTSTR,"scale",RTENAME,seqent,RTSTR,"",RTPOINT,base_pnt,RTREAL,sys.dim_map.fSimTriScale,RTNONE);
				ads_cmd(plist);
				ads_relrb(plist);
				acdbOpenObject(pPline,plineId,AcDb::kForWrite);
				SetEntLayer(pPline,"3");
				nn = pPline->numVerts();
				//以复合线的头结束
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
						if(DISTANCE(f_start,f_end)<0.05)//误差常数 
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
						//未完成 WJH--01.04.25
						/*if(vertexNumber==nn-1&&
							DISTANCE(vertex[0],f_)*/
					}
				}
				pPline->close();
			}
			else
				continue;
			
			nn = vertex.GetNodeNum();
			for (i=0;i<nn;i++)//标注
			{
				f_start = vertex[i];
				if(i==nn-1)
					f_end = vertex[0];
				else
					f_end   = vertex[i+1];
				char sLen[15];
				double dist = DISTANCE(f_start,f_end);
				sprintf(sLen,"%.0f",dist*scale/sys.dim_map.fSimTriScale);
				double line_angle = Cal2dLineAng(
					f_start.x,f_start.y,f_end.x,f_end.y);
				f3dPoint f_text_pos;
				ads_point text_pos;
				f_text_pos.x = f_start.x + (f_end.x-f_start.x)/2;
				f_text_pos.y = f_start.y + (f_end.y-f_start.y)/2;
				f_text_pos = GetPtInPolar(f_text_pos,line_angle-Pi/2,0.5);
				text_pos[X] = f_text_pos.x;
				text_pos[Y] = f_text_pos.y;
				line_angle = RadToDegree(line_angle);
				if(line_angle>=90&&line_angle<270)
				{
					line_angle -= 180;
					line_angle = line_angle*Pi/180.0;
					DimText(text_pos,sLen,textStyleId,sys.dim_map.fPartGuigeTextSize,line_angle,
						AcDb::kTextCenter,AcDb::kTextBottom);
				}
				else
				{
					line_angle = line_angle*Pi/180.0;
					DimText(text_pos,sLen,textStyleId,sys.dim_map.fPartGuigeTextSize,line_angle,
						AcDb::kTextCenter,AcDb::kTextTop);
				}
			}
		}
		else
		{
			ads_command(RTSTR,"layer",RTSTR,"T",RTSTR,"*",RTSTR,"",RTNONE);
			ads_command(RTSTR,"layer",RTSTR,"s",RTSTR,"0",RTSTR,"",RTNONE);
			break;
		}
	}
#endif
}

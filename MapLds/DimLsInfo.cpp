//<LOCALE_TRANSLATE BY hxr /> 2015-04-27
// 螺栓信息标注
#include "stdafx.h"
#include "stdarx.h"
//#include "TMA.h"
#include "MenuFunc.h"
#include "DrawTaMap.h"
#include "adstools.h"
#include "RxTools.h"
#include "geassign.h"
#include "sysPara.h"
#include "Tower.h"
#include "DragEntSet.h"
//4.1
typedef struct tagLSINFO{
	f3dPoint pos;
	char guige[20];
}LSINFO;
void DimLsInfo()//螺栓信息标注
#ifndef __TSA_FILE_
{
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	char dimText[40];						//尺寸文字
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
    CAcModuleResourceOverride resOverride;
	//通过选择实体获取实体的ID
	ads_name entname,ent_sel_set;
	struct resbuf *filter;
#ifdef _ARX_2007
	filter = acutBuildList(RTDXF0,L"INSERT",0);
	acedCommand(RTSTR,L"OSNAP",RTSTR,L"OFF",RTNONE);
#else
	filter = acutBuildList(RTDXF0,"INSERT",0);
	acedCommand(RTSTR,"OSNAP",RTSTR,"OFF",RTNONE);
#endif
	if(acedSSGet(NULL,NULL,NULL,filter,ent_sel_set)==RTNORM)
	{
		long ll;
		acedSSLength(ent_sel_set,&ll);
		ATOM_LIST<LSINFO>lsinfolist;
		for(long i=0;i<ll;i++)
		{
			AcDbEntity *pEnt;
			acedSSName(ent_sel_set,i,entname);
			AcDbObjectId entId;
			acdbGetObjectId(entId,entname);
			acdbOpenObject(pEnt,entId,AcDb::kForRead);
			long handle=GetTaAtomHandle(pEnt);
			CLDSBolt *pBolt=(CLDSBolt*)Ta.FromPartHandle(handle,CLS_BOLT);
			if(pBolt)
			{
				LSINFO *pLsInfo=lsinfolist.append();
				sprintf(pLsInfo->guige,"M%dX%.0f",pBolt->get_d(),pBolt->get_L());
				AcGePoint3d center = ((AcDbBlockReference*)pEnt)->position();
				pLsInfo->pos.Set(center.x,center.y,center.z);
			}
			pEnt->close();
		}
		double angle;
		LSINFO *pFirstLsInfo,*pSecLsInfo;
		if(lsinfolist.GetNodeNum()==1)
		{
			angle=0;
			pFirstLsInfo=lsinfolist.GetFirst();
		}
		else if(lsinfolist.GetNodeNum()>1)
		{
			pFirstLsInfo=lsinfolist.GetFirst();
			pSecLsInfo=lsinfolist.GetNext();
			f3dPoint vec = pSecLsInfo->pos-pFirstLsInfo->pos;
			angle=Cal2dLineAng(0,0,vec.x,vec.y);
			if(angle>Pi/2.0&&angle<=1.5*Pi)
				angle-=Pi;
		}
		if(lsinfolist.GetNodeNum()>0)
		{
			int n=0;
			for(LSINFO *pLsInfo=lsinfolist.GetFirst();pLsInfo;pLsInfo=lsinfolist.GetNext())
			{
				if(stricmp(pLsInfo->guige,pFirstLsInfo->guige)==0)
					n++;
			}
			sprintf(dimText,"%d%s",n,pFirstLsInfo->guige);
			f3dPoint text_pos=pFirstLsInfo->pos;
			DRAGSET.ClearEntSet();
			AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
			DimText(pBlockTableRecord,text_pos,dimText,TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,
				angle,AcDb::kTextCenter,AcDb::kTextBottom);
#ifdef AFX_TARG_ENU_ENGLISH
			DragEntSet(text_pos,"Please select the inserting point for dimension information of bolt text:");
#else
			DragEntSet(text_pos,"请选择螺栓文本标注信息插入点:");
#endif
			pBlockTableRecord->close();
		}
	}
	acutRelRb(filter);
	acedSSFree(ent_sel_set);
}
#else
{;}
#endif
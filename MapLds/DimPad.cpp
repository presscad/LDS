//// 垫圈规格及数量标注
#include "stdafx.h"
#include "stdarx.h"
//#include "TMA.h"
#include "MenuFunc.h"
#include "DrawTaMap.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "adstools.h"
#include "rxtools.h"
#include "SysPara.h"
#ifndef __TSA_FILE_
static void DimPadText(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint pad_pos,f3dPoint start,char *sPadText)
{
	f3dPoint end;
	AcGePoint3d acad_end,curPos;
	AcDbObjectId line1Id,line2Id,textId;
	int type, track = 1;//设置鼠标拖动控制
	struct resbuf result;
	AcDbLine *pLine;
	AcDbText *pDimText;

	end = start;
	end.x += end.x+7;
	ads_name ent_name;
	CreateAcadLine(pBlockTableRecord,pad_pos,start);
	acdbEntLast(ent_name);
	acdbGetObjectId(line1Id,ent_name);
	CreateAcadLine(pBlockTableRecord,start,end);
	acdbEntLast(ent_name);
	acdbGetObjectId(line2Id,ent_name);
	curPos.x +=0.5;
	start.y += 0.5;
	textId = DimText(pBlockTableRecord,start,sPadText,TextStyleTable::hzfs.textStyleId,sys.dim_map.fLsGuigeTextSize,0);
	Cpy_Pnt(acad_end,end);

	ads_point L_T,R_B;
	L_T[X]=pad_pos.x-30;
	L_T[Y]=pad_pos.y+30;
	L_T[Z]=0;
	R_B[X]=pad_pos.x+30;
	R_B[Y]=pad_pos.y-30;
	R_B[Z]=0;
#ifdef _ARX_2007
	acedCommand(RTSTR,L"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
#else
	acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
#endif
	while(track>0)
	{
		ads_grread(track,&type,&result);
		curPos.x = result.resval.rpoint[X];
		curPos.y = result.resval.rpoint[Y];
		curPos.z = 0;
		acdbOpenObject(pLine,line1Id,AcDb::kForWrite);
		pLine->setEndPoint(curPos);
		pLine->close();
		acdbOpenObject(pLine,line2Id,AcDb::kForWrite);
		pLine->setStartPoint(curPos);
		acad_end = pLine->endPoint();
		Cpy_Pnt(acad_end,curPos);
		acad_end.x += 7;
		pLine->setEndPoint(acad_end);
		pLine->close();
		acdbOpenObject(pDimText,textId,AcDb::kForWrite);
		curPos.x +=0.5;
		curPos.y +=0.5;
		pDimText->setPosition(curPos);
		pDimText->close();
		if(type==3)
			break;
	}
}

void DimBoltPad() // 垫圈规格及数量标注
{
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	//通过选择实体获取实体的ID
	AcDbObjectId entId;
	ads_name entname,ent_sel_set;
	struct resbuf *filter;

	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
#ifdef _ARX_2007
	filter = acutBuildList(RTDXF0,L"POINT",0);
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"34",RTNONE);
#else
	filter = acutBuildList(RTDXF0,"POINT",0);
	acedCommand(RTSTR,"PDMODE",RTSTR,"34",RTNONE);
#endif
	if(acedSSGet(NULL,NULL,NULL,filter,ent_sel_set)==RTNORM)
	{
		long ll;
	#ifdef _ARX_2007
		acedCommand(RTSTR,L"PDMODE",RTSTR,L"1",RTNONE);
	#else
		acedCommand(RTSTR,"PDMODE",RTSTR,"1",RTNONE);
	#endif
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		acedSSLength(ent_sel_set,&ll);
		f3dPoint pad_pos,insert_pos;
		for(long i=0;i<ll;i++)
		{
			AcDbEntity *pEnt;
			acedSSName(ent_sel_set,i,entname);
			acdbGetObjectId(entId,entname);
			acdbOpenObject(pEnt,entId,AcDb::kForRead);
			long handle=GetTaAtomHandle(pEnt);
			CLDSBolt *pBolt=(CLDSBolt*)Ta.FromPartHandle(handle,CLS_BOLT);
			if(pBolt)
			{
				if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					AcDbPoint *pPadPoint=(AcDbPoint*)pEnt;
					AcGePoint3d acad_insert_pos = pPadPoint->position();
					Cpy_Pnt(pad_pos,acad_insert_pos);
					insert_pos.Set(pad_pos.x-7,pad_pos.y+12,0);
					char sPadText[20]="";
					if(GetPadText(pPadPoint,sPadText))
						DimPadText(pBlockTableRecord,pad_pos,insert_pos,sPadText);//porg 起点  pend 终点  buffer //垫圈规格及数量
				}
				else
				{
					pEnt->close();
					continue;
				}
			}
			pEnt->close();
		}
		pBlockTableRecord->close();
	}
	else
	{
	#ifdef _ARX_2007
		acedCommand(RTSTR,L"PDMODE",RTSTR,L"1",RTNONE);
	#else
		acedCommand(RTSTR,"PDMODE",RTSTR,"1",RTNONE);
	#endif
	}
	acutRelRb(filter);
	acedSSFree(ent_sel_set);
}
#else
void DimBoltPad(){;}
#endif
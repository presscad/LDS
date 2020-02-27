#include "StdAfx.h"
#include "CheckDataProject.h"
#include "env_def.h"
#include "SortFunc.h"
#include "ComparePartNoString.h"

 SCOPE_STRU GetTaAtomScope(const CLDSPartPtr pObj); // from DrawMapInAcad.cpp 52;

//1.组合角钢编号及间隙放大值
void ExportGroupAngleEnlarge()
{
	logerr.ClearContents();
	ARRAY_LIST<EXPORT_INFO> list;
	const int nSpace=25;
	for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
	{
		if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)pPart;
			CLDSLineAngle* pLineAngle=(CLDSLineAngle*) Ta.FromPartHandle(pGroupAngle->m_hDatumSonAngle);
			CLsRefList *pLsRefList=(pLineAngle)?pLineAngle->GetLsRefList():NULL;
			if(pLsRefList==NULL)
				continue;
			CLsRef *pLsRef=pLsRefList->GetFirst();
			CLDSBolt* pBolt=(pLsRef)?pLsRef->GetLsPtr():NULL;
			if(pBolt==NULL) 
			{
				logerr.Log("0X%x组合角钢异常没有螺栓穿过,原间隙%0.3f",pGroupAngle->handle,pGroupAngle->jg_space);
				continue;
			}
			SCOPE_STRU scope = GetTaAtomScope(pGroupAngle);
			//增大间隙值
			scope.fMinX-=nSpace;
			scope.fMinY-=nSpace;
			scope.fMinZ-=nSpace;
			scope.fMaxX+=nSpace;
			scope.fMaxY+=nSpace;
			scope.fMaxZ+=nSpace;
			int nPush=Ta.Parts.push_stack();
			for(CLDSPart* pPart=Ta.Parts.GetFirst();pPart;pPart=Ta.Parts.GetNext())
			{
				if(!pPart->IsPlate())
					continue;
				CLDSPlate* pPlate=(CLDSPlate*) pPart;
				f3dPoint pos=pPart->ucs.origin;
				if(scope.IsIncludePoint(pos))
				{
					CLsRef* pBoltRef=pPlate->FindLsByHandle(pBolt->handle);
					if(pBoltRef)
					{
						EXPORT_INFO* pExportInfo=list.append();
						pExportInfo->fSpace=pPlate->GetThick();
						pExportInfo->pGroupAngle=pGroupAngle;
						pExportInfo->fEnlarge=pGroupAngle->jg_space-pExportInfo->fSpace;
						pExportInfo->segI=pLineAngle->iSeg;
						break;
					}
				}
			}
			Ta.Parts.pop_stack(nPush);
		}
	}
	for (EXPORT_INFO* pExportInfo=list.GetFirst();pExportInfo;pExportInfo=list.GetNext())
		logerr.Log("组合角钢0X%X,增大值%s，原间隙%s",pExportInfo->pGroupAngle->handle,
			(char*)CXhChar16(pExportInfo->fEnlarge),(char*)CXhChar16(pExportInfo->fSpace));
	logerr.ShowToScreen();
}

//2.角肋板及槽肋板报告及内圆弧报告
static int CompareRibExportInfo(const RIB_EXPORT_INFO& item1,const RIB_EXPORT_INFO& item2)
{
	return ComparePartNoString(item1.partLable,item2.partLable);
}
void  ExportAngleLibPlateAndGroovePlate()
{	//找出所有的肋板
	ARRAY_LIST<RIB_EXPORT_INFO> ribExportInfoList;
	CHashSet<CLDSGeneralPlate*> hashlibPartByHandel;
	for(CLDSPart* pPart=Ta.EnumPartFirst();pPart;pPart=Ta.EnumPartNext())
	{
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
		//参数板
		if(pPlate->GetClassTypeId()==CLS_PARAMPLATE)
		{
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*) pPart;
			if(pParamPlate->m_iParamType==TYPE_LINEANGLERIBPLATE)		//TODO 曹型肋板
			{
				hashlibPartByHandel.SetValue(pPlate->handle,pPlate);
				continue;
			}
		}
		//普通钢板
		ATOM_LIST<int> weldEdgelist;
		int i=0;
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext(),i++)
		{
			if(pVertex->m_bWeldEdge)
				weldEdgelist.append(i);
		}
		if(weldEdgelist.GetNodeNum()<2)
			continue;
		int* pPrevWeldEdge=weldEdgelist.GetTail();
		for (int* pWeldEdge=weldEdgelist.GetFirst();pWeldEdge;pWeldEdge=weldEdgelist.GetNext())
		{
			int nMargin=(*pWeldEdge)-(*pPrevWeldEdge);
			if(abs(nMargin)==2)			//焊接边相隔是2
			{
				hashlibPartByHandel.SetValue(pPart->handle,pPlate);
				break;
			}
		}
	}

	for (CLDSGeneralPlate* pPlate=hashlibPartByHandel.GetFirst();pPlate;pPlate=hashlibPartByHandel.GetNext())
	{
		RIB_EXPORT_INFO ribExprotInfo;
		ribExprotInfo.fThick=pPlate->GetThick();
		ribExprotInfo.partLable=pPlate->GetPartNo();
		if(pPlate->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pPlate)->m_iParamType==TYPE_LINEANGLERIBPLATE)
		{
			CLDSParamPlate* pParamPlate=(CLDSParamPlate*)pPlate;
			long hDatumPart=0;double V=0;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pPlate->BelongModel()->FromPartHandle(abs(hDatumPart),CLS_LINETUBE,CLS_PLATE,CLS_PARAMPLATE);
			ribExprotInfo.sPlateType="角肋板";
			CHashTable<DESIGN_PLATE_ITEM*>itemHashTbl;
			itemHashTbl.CreateHashTable(pParamPlate->designItemList.GetNodeNum());
			for(DESIGN_PLATE_ITEM* pItem=pParamPlate->designItemList.GetFirst();pItem;pItem=pParamPlate->designItemList.GetNext())
				itemHashTbl.SetValueAt(pItem->key,pItem);
			pParamPlate->GetDesignItemValue(KEY2C("S"),&V,&itemHashTbl);
			if(pAngle)
			{
				if(V>pAngle->GetThick()*1.2)
					ribExprotInfo.bStart="√";
				else
					ribExprotInfo.bStart="X";
			}
			else
				ribExprotInfo.bStart="--";
			ribExprotInfo.sAngle.Printf("%d x %d",int(V),int(V));
		}
		ribExprotInfo.segI=pPlate->iSeg;
		ribExportInfoList.append(ribExprotInfo);
	}
	//临时输出
	CBubbleSort<RIB_EXPORT_INFO>::BubSort(ribExportInfoList.m_pData,ribExportInfoList.GetSize(),CompareRibExportInfo);
	logerr.ClearContents();
	logerr.Log("段号 状态  件号  厚度   倒角  类型");
	for (RIB_EXPORT_INFO* pRibExportInfo=ribExportInfoList.GetFirst();
			pRibExportInfo;pRibExportInfo=ribExportInfoList.GetNext())
		logerr.Log("%s     %s    %s   %s   %s   %s",(char*) pRibExportInfo->segI.ToString(),(char*)pRibExportInfo->bStart,
					(char*)pRibExportInfo->partLable,(char*)CXhChar16(pRibExportInfo->fThick),
					(char*)pRibExportInfo->sAngle, (char*) pRibExportInfo->sPlateType);
	logerr.ShowToScreen();

}
//3.卷边处理
static int CompareRibExportInfo(const ROLL_EDGE_EXPORT_INFO& item1,const ROLL_EDGE_EXPORT_INFO& item2)
{
	return ComparePartNoString(item1.partLable,item2.partLable);
}
void  ExportRollEdgePart()
{
	ARRAY_LIST<ROLL_EDGE_EXPORT_INFO> rollEdgeExportInfoList;
	for(CLDSPart* pPart=Ta.EnumPartFirst();pPart;pPart=Ta.EnumPartNext())
	{
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
		int	 nRollEdge=0,nEdge=0;
		CXhChar100 sRollEdgeHeigth="";
		for(PROFILE_VERTEX* pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex->m_bRollEdge)
			{
				nRollEdge++;
				if(sRollEdgeHeigth!=""&&sRollEdgeHeigth.GetLength()>0)
					sRollEdgeHeigth+="#";
				sRollEdgeHeigth+=CXhChar16(pVertex->manu_space);
			}
			nEdge++;
		}
		if(nRollEdge>0)
		{
			ROLL_EDGE_EXPORT_INFO rollEdgeInfo;
			rollEdgeInfo.segI=pPlate->iSeg;
			rollEdgeInfo.nRollEdge=nRollEdge;
			rollEdgeInfo.partLable=pPlate->GetPartNo();
			rollEdgeInfo.cMaterial=pPlate->cMaterial;
			rollEdgeExportInfoList.append(rollEdgeInfo);
			rollEdgeInfo.sRollEdgeHeigth=sRollEdgeHeigth;
			if(pPlate->m_cRollProcessType==1)	
				rollEdgeInfo.sRollEdgeDispose="最少刀数";
			else if(pPlate->m_cRollProcessType==1)
				rollEdgeInfo.sRollEdgeDispose="最佳受力";
			else
				rollEdgeInfo.sRollEdgeDispose="默认值";	
			g_pSolidDraw->SetEntSnapStatus(pPlate->handle);
		}
	}
	//临时输出
	CBubbleSort<ROLL_EDGE_EXPORT_INFO>::BubSort(rollEdgeExportInfoList.m_pData,rollEdgeExportInfoList.GetSize(),CompareRibExportInfo);
	logerr.ClearContents();
	logerr.Log("段号  件号   卷边高度    卷边处理方式   边数  卷边数   规格 材质");
	for (ROLL_EDGE_EXPORT_INFO* pInfo=rollEdgeExportInfoList.GetFirst();pInfo;pInfo=rollEdgeExportInfoList.GetNext())
	{
		logerr.Log("%s  %s   %s   %s  %d   %d   %c",(char*)pInfo->segI.ToString(),(char*)pInfo->partLable,
				(char*)pInfo->sRollEdgeHeigth,(char*)pInfo->sRollEdgeDispose,
				"",pInfo->nRollEdge,pInfo->cMaterial);
	}
	logerr.ShowToScreen();
}
//4.端连接螺栓外孔间隙


void test()
{
	ExportRollEdgePart();
}
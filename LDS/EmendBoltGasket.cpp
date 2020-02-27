//<LOCALE_TRANSLATE BY wbt />
#include "StdAfx.h"
#include "env_def.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "GlobalFunc.h"
#include "EmendBoltGasket.h"
#include "LogFile.h"
#include "SortFunc.h"
#include "VertexsLifeObject.h"
#include "Hash64Table.h"

//#ifdef __SMART_DRAW_INC_
BOLT_RELA_PART::BOLT_RELA_PART(CLDSPart* pRelaPart/*=NULL*/)
{
	_pRelaPart=pRelaPart;
	hPart=pRelaPart!=NULL?pRelaPart->handle:0;
	_zDepth=siThick=0;
}
bool BOLT_RELA_PART::IsWasherPlate()
{
	if(_pRelaPart==NULL||!_pRelaPart->IsPlate())
		return false;
	return ((CLDSGeneralPlate*)_pRelaPart)->wiType==CLDSPlate::WASHER_PLATE;
}
CLDSPart* BOLT_RELA_PART::get_pPart()
{
	if(_pRelaPart!=NULL&&_pRelaPart->handle==hPart)
		return _pRelaPart;
	return _pRelaPart=Ta.Parts.FromHandle(hPart);
}
void BOLT_RELA_PART::set_pPart(CLDSPart* pRelaPart)
{
	_pRelaPart=pRelaPart;
	this->hPart=pRelaPart!=NULL?pRelaPart->handle:0;
}
static double CalSignedDistance(f3dPoint startPt,f3dPoint endPt,f3dPoint norm)
{
	if((endPt-startPt)*norm<0)
		return DISTANCE(startPt,endPt)*-1;
	else 
		return DISTANCE(startPt,endPt);
}
//计算构件中间层到螺栓原点的距离，根据距离大小进行排序并将结果保存到列表中
static BOOL CalAndSortIntersPoint(ATOM_LIST<INTERS_POINT> &interPointList,CLDSPart *pPart,CLDSBolt* pBolt);
static void InitBoltGasketThickAndOffset(ATOM_LIST<INTERS_POINT> &InterPointList,CBoltGasket* pBoltGasket);//初始化螺栓垫板垫圈的厚度和偏移距信息
CLDSLineAngle *GetBoltGasketCutAngle(CLDSBolt* pBolt);	//得到螺栓垫板进行切角的角钢
static CLDSNode* GetCrossNode(CHash64Set<CLDSNode*> &hashCrossNodes,CBoltGasket* pBoltGasket1,CBoltGasket* pBoltGasket2);//判断螺栓基准点的依附杆件是否与节点的依附杆件相同
static int compare_func1(const BOLTGASKET_PTR &pGasket1,const BOLTGASKET_PTR &pGasket2)	//同一基准构件上的螺栓排序比较函数
{	return int(pGasket1->boltLengthPos-pGasket2->boltLengthPos); }
static CLDSPlate* DesignCrossPlate(CRawGasket *pRawGasket);	//设计交叉垫板
static void AppendOutputInfoToList(GASKET_WARNING_RECORD* pRecordPart,CTaskOutputList* pInfoList);//将结果信息保存到输出信息列表中

WORD CBoltGasket::get_wiCurrDianQuanThick()
{
	if(pBolt==NULL)
		return 0;
	if(pBolt->DianQuan.N>0&&pBolt->DianQuan.thick>0)
		return pBolt->DianQuan.N*pBolt->DianQuan.thick;
	return 0;
}
WORD CBoltGasket::get_wiCurrGasketThick()
{
	for(SmartPartPtr pSmartPart=hashRelaPartPtrs.GetFirst();pSmartPart.IsHasPtr();pSmartPart=hashRelaPartPtrs.GetNext())
	{
		if(pSmartPart->IsPlate()&&pSmartPart.pBasicPlate->wiType==CLDSPlate::WASHER_PLATE)
			return (WORD)pSmartPart->GetThick();
	}
	return 0;
}
GASKET_WARNING_RECORD* CEmendBoltGasket::GetWarningRecord(long hBolt)
{
	GASKET_WARNING_RECORD* pWarningRecord=hashErrorRecords.GetValue(hBolt);
	if(pWarningRecord==NULL)
		pWarningRecord=hashErrorRecordsView.GetValue(hBolt);
	return pWarningRecord;
}
//1、遍历所有螺栓，查找需要添加垫圈或垫板的螺栓，并计算螺栓垫板垫圈的厚度和位置
void CEmendBoltGasket::InitGasketBoltSet(bool advanceMode/*=true*/)
{ 
	SmartPartPtr pPart;
	//1.初始化hashBoltGaskets，将螺栓及螺栓的关联构件保存到hashBoltGaskets中
	hashBoltGaskets.Empty();
	if(advanceMode)
	{	//启用TAP中的螺栓通厚间隙识别技术
		/*
		ARRAY_LIST<DWORD> assmbleBoltIdArr;
		if(DATACENTERCMD_CONTEXT::OBJ_TYPE_SELECTED_OBJS==pContext->m_cOperObjType)
		{
			for(CAssembleBolt *pBoltRec=boltRecSet.GetFirst();pBoltRec;pBoltRec=boltRecSet.GetNext())
				assmbleBoltIdArr.append(pBoltRec->Id);
		}
		if(assmbleBoltIdArr.GetSize()>0)*/
		//	UpdateBoltRelaObjs(assmbleBoltIdArr.m_pData,assmbleBoltIdArr.GetSize());
		//else //一般在文件打开或导入文件时已更新螺栓穿插关系,如确有需要可手动在右侧模型树右键菜单中手动执行 wjh-2017.3.23
			UpdateBoltRelaObjs();
		for(pPart=Ta.Parts.GetFirst(CLS_BOLT);pPart.IsHasPtr();pPart=Ta.Parts.GetNext(CLS_BOLT))
		{
			if(pPart.pBolt->m_bVirtualPart||pPart.pBolt->IsFootNail())
				continue;
			CBoltGasket *pBoltGasket=hashBoltGaskets.GetValue(pPart.pBolt->handle);
			if(pBoltGasket==NULL)
			{
				pBoltGasket = hashBoltGaskets.Add(pPart.pBolt->handle);
				pBoltGasket->pBolt=pPart.pBolt;
			}
		}

		//2.遍历hashBoltGaskets，通过线面求交的方式计算每颗螺栓所需的垫板垫圈厚度及位置
		for(CBoltGasket* pBoltGasket=hashBoltGaskets.GetFirst();pBoltGasket;pBoltGasket=hashBoltGaskets.GetNext())
		{
			CLDSBolt *pBolt = pBoltGasket->pBolt;
			CBoltRelaPartList* pRelaPartList=this->GetBoltRelaParts(pBolt->handle);
			if(pRelaPartList->GetRelaPartCount()==0)
			{	//孤立螺栓肯定不需要垫板，警示信息暂不在此提示而是由常规校核身负责
				hashBoltGaskets.DeleteCursor();
				continue;
			}
			else if(pRelaPartList->GetRelaPartCount()==1)
			{	//删除只有一个相关构件的螺栓, 同时保留错误警示信息输出记录
				GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add(pBoltGasket->pBolt->handle);
				pRecordPart->style=ERROR_VIRTUAL_HOLE_LIKELY;
				pRecordPart->pBolt=pBoltGasket->pBolt;
				BOLT_RELA_PART* pRelaPartInfo=pRelaPartList->EnumFirstRelaPart();
				pRecordPart->pAttachPart=pRelaPartInfo->pPart;
				hashBoltGaskets.DeleteCursor();
				continue;
			}
			//根据interPointList，初始化垫板垫片的厚度和偏移距
			if(!pRelaPartList->InitializeBoltGasket(pBoltGasket))
				hashBoltGaskets.DeleteCursor();	//将确定不需要垫板垫圈(板)的螺栓删除
		}
	}
	else
	{
		for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_PLATE&&pPart.pPlate->wiType==CLDSPlate::WASHER_PLATE)//jdb_style==4)
				continue;

			CLsRefList *pLsRefList=pPart->GetLsRefList();
			if(pLsRefList==NULL)
				continue;
			for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				CLDSBolt *pBolt=pLsRef->GetLsPtr();
				if(pBolt==NULL)
					continue;
				if(pBolt->m_bVirtualPart||pBolt->IsFootNail())
					continue;
				CBoltGasket *pBoltGasket=hashBoltGaskets.GetValue(pBolt->handle);
				if(pBoltGasket==NULL)
				{
					pBoltGasket = hashBoltGaskets.Add(pBolt->handle);
					pBoltGasket->pBolt=pBolt;
				}
				if(pBoltGasket->hashRelaPartPtrs.GetValue(pPart->handle)==NULL)
					pBoltGasket->hashRelaPartPtrs.SetValue(pPart->handle,pPart);
			}
		}

		//2.遍历hashBoltGaskets，通过线面求交的方式计算每颗螺栓所需的垫板垫圈厚度及位置
		for(CBoltGasket* pBoltGasket=hashBoltGaskets.GetFirst();pBoltGasket;pBoltGasket=hashBoltGaskets.GetNext())
		{
			CLDSBolt *pBolt = pBoltGasket->pBolt;
			if(pBolt==NULL)
				continue;
			//删除只有一个相关构件的螺栓
			if(pBoltGasket->hashRelaPartPtrs.GetNodeNum()==1)
			{
				GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add(pBoltGasket->pBolt->handle);
				pRecordPart->style=ERROR_VIRTUAL_HOLE_LIKELY;
				pRecordPart->pBolt=pBoltGasket->pBolt;
				pRecordPart->pAttachPart=pBoltGasket->hashRelaPartPtrs.GetFirst();
				hashBoltGaskets.DeleteCursor();
				continue;
			}
			BOOL bInvalid = FALSE;
			ATOM_LIST<INTERS_POINT> interPointList;
			for(pPart=pBoltGasket->hashRelaPartPtrs.GetFirst();pPart.IsHasPtr();pPart=pBoltGasket->hashRelaPartPtrs.GetNext())
			{	//计算螺栓的关联构件中间层到螺栓原点的距离，并根据距离大小进行排序，将结果保存到interPointList	
				if(pPart->IsPlate()&&pPart.pBasicPlate->wiType==CLDSPlate::WASHER_PLATE)
					continue;
				if(!CalAndSortIntersPoint(interPointList,pPart,pBolt))
				{
					//logerr.Log("error");
					bInvalid=TRUE;
					break;
				}
			}
			if(bInvalid)
				hashBoltGaskets.DeleteCursor();
			else
			{
				//根据interPointList，初始化垫板垫片的厚度和偏移距
				InitBoltGasketThickAndOffset(interPointList,pBoltGasket);
				if(pBoltGasket->gasketThick<=CEmendBoltGasket::MINSPACING)	//将不需要垫板垫圈的螺栓删除
					hashBoltGaskets.DeleteCursor();
			}
		}
	}
	hashBoltGaskets.Clean();
}
//3、修正已存在的垫板，并将关联螺栓从hashBoltGaskets中移除
void CEmendBoltGasket::EmendExistingBoltPadSet(bool advanceMode/*=true*/)
{
	//遍历所有钢板，初始化已存在垫板列表 jdb_style==4为螺栓垫板
	existingBoltPadSet.Empty();
	for(CLDSPlate* pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		if(pPlate->wiType==CLDSPlate::WASHER_PLATE)
			existingBoltPadSet.append(pPlate);
	}
	//从gasketSet删除与已存在垫板吻合或进行了修正的螺栓
	if(existingBoltPadSet.GetNodeNum()<1)
		return;
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	for(CLDSPlate* pPlate=(CLDSPlate*)existingBoltPadSet.GetFirst();pPlate;pPlate=(CLDSPlate*)existingBoltPadSet.GetNext())
	{
		CLsRef* pLsRef;
		CBoltGasket *pBoltGasket=NULL;
		for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
		{
			pBoltGasket = hashBoltGaskets.GetValue((*pLsRef)->handle);
			if(pBoltGasket!=NULL)
				break;
		}
		//多孔垫片暂时仅取第一个螺栓的间隙作为判断标准
		if(pBoltGasket!=NULL)
		{
			GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pBoltGasket->pBolt->handle);
			if(fabs(pPlate->GetThick()-pBoltGasket->gasketThick)>MINMODIFYDIST)//修正厚度
			{
				pRecordPart->thick = FindPlankThick(pBoltGasket->gasketThick);
				//
				pRecordPart->style=ERROR_GASKET_THICK;
				if(!advanceMode)	//高级模式时，当前不修正而是交由用户在任务栏里进行修正
					pPlate->Thick=pRecordPart->thick;
				pRecordPart->pBolt=pBoltGasket->pBolt;
				pRecordPart->pGasketPlate=pPlate;
				for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{	//除垫板基准螺栓保留外，清除其余螺栓的垫板预设信息
					if(pLsRef->pBolt->handle==pBoltGasket->pBolt->handle)
						continue;
					hashBoltGaskets.DeleteNode((*pLsRef)->handle);
					hashErrorRecords.DeleteNode((*pLsRef)->handle);
					hashErrorRecordsView.SetValue((*pLsRef)->handle,pRecordPart);
				}
			}
			else
			{	//此处已有垫板,且垫板厚度刚好,不需要调整
				for(pLsRef=pPlate->GetFirstLsRef();pLsRef;pLsRef=pPlate->GetNextLsRef())
				{
					hashErrorRecordsView.DeleteNode((*pLsRef)->handle);
					hashErrorRecords.DeleteNode((*pLsRef)->handle);
				}
				continue;
			}
			if(pPlate->GetLsCount()>1&&pPlate->vertex_list.GetNodeNum()==4)	//对于矩形垫板要判断是否进行切角处理
			{
				CLDSLineAngle* pCutAngle=GetBoltGasketCutAngle(pBoltGasket->pBolt);
				CVertexsLifeObject manageVertexs(pPlate->vertex_list);
				if(pView->CutBoltPadByAngle(pPlate,pCutAngle))	//垫板进行切角处理
				{
					GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pBoltGasket->pBolt->handle);
					pRecordPart->style=ERROR_BOLTPAD_CUTANGLE;
					pRecordPart->pBolt=pBoltGasket->pBolt;
					pRecordPart->pGasketPlate=pPlate;
					pRecordPart->pCutAngle=pCutAngle;
				}
				else	//未发生外形改变
					manageVertexs.DisableBackup();
			}
		}
		else 
		{
			GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pPlate->handle);
			pRecordPart->style=ERROR_REDUNDANT_GASKET;
			pRecordPart->pGasketPlate=pPlate;
			existingBoltPadSet.DeleteCursor();
		}
	}
	hashBoltGaskets.Clean();
	existingBoltPadSet.Clean();
}
//4、初始化未加工的垫板集合，并初始化垫板模式(单孔、多孔或多孔交叉点垫板)
void CEmendBoltGasket::InitRawGasketSet()
{
	hashRawGaskets.Empty();
	//交叉节点集合，key值两依附杆件句柄
	CHash64Set<CLDSNode*> hashCrossNodes;
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->m_cPosCalType==4)//两杆件交叉点
		{
			DUALKEY dualkey(pNode->arrRelationPole[0],pNode->arrRelationPole[1],true);
			if(hashCrossNodes.GetValue(dualkey)==NULL)
				hashCrossNodes.SetValue(dualkey,pNode);
		}
	}
	//1、查找在同一个基准构件上的螺栓集合,并按照螺栓在基准构件上位置进行排序
	//如果基准构件是角钢,找在同一肢法线的螺栓集合
	//建议流程:1.分组;2.根据组基准构件类型,分组计算距离参数;3.分组排序
	CHashStrList<CDatumPartBolts> partBoltsHashHandle;	//非角钢构件以句柄为键值，角钢以句柄+法线肢
	CDatumPartBolts* pDatumPartBolts=NULL;
	//1、根据基准构件类型对螺栓进行分组（如果基准构件是角钢，找同一肢法线上的螺栓集合）
	for(CBoltGasket *pGasket=hashBoltGaskets.GetFirst();pGasket;pGasket=hashBoltGaskets.GetNext())
	{
		char key[100]="";
		long hPart=pGasket->pBolt->des_base_pos.hPart;
		CLDSPart* pDatumPart = Ta.FromPartHandle(hPart);
		f3dPoint norm,origin_pos;
		//暂仅支持角钢，其余构件上使用垫板较少
		if(pDatumPart->IsAngle())
		{	//应向角钢楞线投影，即保证所有点在同一直线上，然后再算距离
			CLDSLineAngle* pAngle = (CLDSLineAngle*)pDatumPart;
			int x0_y1=2;
			pAngle->getUCS(pAngle->ucs);
			pAngle->IsLsInWing(pGasket->pBolt,&x0_y1);
			if(x0_y1==0)
			{
				sprintf(key,"0x%d+X",hPart);
				norm=pAngle->get_norm_x_wing();
			}
			else if(x0_y1==1)
			{	
				sprintf(key,"0x%d+Y",hPart);
				norm=pAngle->get_norm_y_wing();
			}
			origin_pos=pDatumPart->ucs.origin;
			pDatumPartBolts=partBoltsHashHandle.GetValue(key);
			if(pDatumPartBolts==NULL)
			{
				pDatumPartBolts=partBoltsHashHandle.Add(key);
				pDatumPartBolts->datum_origin=origin_pos;
				pDatumPartBolts->norm=norm;
			}
			f3dPoint ls_pos=pGasket->pBolt->ucs.origin;
			f3dLine ber_line;
			ber_line.startPt=pAngle->Start();
			ber_line.endPt=pAngle->End();
			project_point(ls_pos,origin_pos,norm);
			SnapPerp(&ls_pos,ber_line,ls_pos);
			pGasket->boltLengthPos=DISTANCE(ls_pos,origin_pos);
			pDatumPartBolts->boltGasketSet.append(pGasket);
		}
		else if(pDatumPart->GetClassTypeId()==CLS_PLATE)
		{	//基准构件为钢板时应先计算包络正交矩形框,然后向长边投影,并根据投影点算距离
			CLDSPlate* pPlate=(CLDSPlate*)pDatumPart;
			norm=pPlate->ucs.axis_z;
		}
	}
	//2、将同一基准构件上的螺栓根据位置进行排序
	for(pDatumPartBolts=partBoltsHashHandle.GetFirst();pDatumPartBolts;pDatumPartBolts=partBoltsHashHandle.GetNext())
	{
		ARRAY_LIST<BOLTGASKET_PTR> boltGasketArr;
		boltGasketArr.SetSize(0,pDatumPartBolts->boltGasketSet.GetNodeNum());
		for(CBoltGasket* pGasket=pDatumPartBolts->boltGasketSet.GetFirst();pGasket;pGasket=pDatumPartBolts->boltGasketSet.GetNext())
			boltGasketArr.append(pGasket);
		CQuickSort<BOLTGASKET_PTR>::QuickSort(boltGasketArr.m_pData,boltGasketArr.GetSize(),compare_func1);
		pDatumPartBolts->boltGasketSet.Empty();
		for(int i=0;i<boltGasketArr.GetSize();i++)
			pDatumPartBolts->boltGasketSet.append(boltGasketArr[i]);
	}
	//3、根据基准构件上相邻螺栓的距离确定垫片模式
	CRawGasket* pRawGasket=NULL;
	for(pDatumPartBolts=partBoltsHashHandle.GetFirst();pDatumPartBolts;pDatumPartBolts=partBoltsHashHandle.GetNext())
	{
		if(pDatumPartBolts->boltGasketSet.GetNodeNum()==1)	//基准构件法线肢上只有单个螺栓
		{
			CBoltGasket* pBoltPad=pDatumPartBolts->boltGasketSet.GetFirst();
			pRawGasket=hashRawGaskets.Append();
			pBoltPad->feature=1;
			pRawGasket->boltGasketSet.SetValue(pBoltPad->pBolt->handle,pBoltPad);
			int type=CLDSBolt::CalPadPara(pBoltPad->pBolt->get_d(),pBoltPad->gasketThick);
			if(type==DIANQUAN_STRU::DIANBAN)
			{	
				pRawGasket->type=CRawGasket::ONEHOLE;//单孔垫板
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pBoltPad->pBolt);
			}
			else if(type==DIANQUAN_STRU::DIANQUAN)
				pRawGasket->type=CRawGasket::WASHER;//垫圈
			continue;
		}
		f3dPoint prebolt_origin,curbolt_origin;
		CBoltGasket* pPrePadBolt=pDatumPartBolts->boltGasketSet.GetFirst();
		prebolt_origin=pPrePadBolt->pBolt->ucs.origin;
		project_point(prebolt_origin,pDatumPartBolts->datum_origin,pDatumPartBolts->norm);
		pRawGasket=hashRawGaskets.Append();
		pRawGasket->boltGasketSet.SetValue(pPrePadBolt->pBolt->handle,pPrePadBolt);
		pRawGasket->pCutAngle=GetBoltGasketCutAngle(pPrePadBolt->pBolt);
		for(CBoltGasket* pCurPadBolt=pDatumPartBolts->boltGasketSet.GetNext();pCurPadBolt;pCurPadBolt=pDatumPartBolts->boltGasketSet.GetNext())
		{
			curbolt_origin=pCurPadBolt->pBolt->ucs.origin;
			project_point(curbolt_origin,pDatumPartBolts->datum_origin,pDatumPartBolts->norm);
			if(DISTANCE(prebolt_origin,curbolt_origin)<BOLTSPACE_THRESHOLD)//两孔
			{
				if(pRawGasket->boltGasketSet.GetValue(pPrePadBolt->pBolt->handle)==NULL)
				{
					pPrePadBolt->feature=1;//标识已经判断了螺栓所属的垫板模式
					pRawGasket->boltGasketSet.SetValue(pPrePadBolt->pBolt->handle,pPrePadBolt);
				}
				if(pRawGasket->boltGasketSet.GetValue(pCurPadBolt->pBolt->handle)==NULL)
				{
					pCurPadBolt->feature=1;
					pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				}
				if(pRawGasket->type==0)
				{
					CLDSNode* pCrossNode=GetCrossNode(hashCrossNodes,pPrePadBolt,pCurPadBolt);
					if(pCrossNode)
					{
						pRawGasket->type=CRawGasket::CROSSMULTIHOLE;	//交叉多孔
						pRawGasket->pCrossNode=pCrossNode;
					}
					else 
						pRawGasket->type=CRawGasket::MULTIHOLE;//多孔
				}
			}
			else
			{
				if(pRawGasket->boltGasketSet.GetNodeNum()==1)
				{
					int type=CLDSBolt::CalPadPara(pPrePadBolt->pBolt->get_d(),pPrePadBolt->gasketThick);
					if(type==DIANQUAN_STRU::DIANBAN)
						pRawGasket->type=CRawGasket::ONEHOLE;//单孔垫板
					else if(type==DIANQUAN_STRU::DIANQUAN)
						pRawGasket->type=CRawGasket::WASHER;//垫圈
				}
				pRawGasket=hashRawGaskets.Append();
				pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pCurPadBolt->pBolt);
				if(pDatumPartBolts->boltGasketSet.GetCurrentIndex()==pDatumPartBolts->boltGasketSet.GetNodeNum()-1)	//处理最后一个螺栓
				{	
					int type=CLDSBolt::CalPadPara(pCurPadBolt->pBolt->get_d(),pCurPadBolt->gasketThick);
					if(type==DIANQUAN_STRU::DIANBAN)
						pRawGasket->type=CRawGasket::ONEHOLE;//单孔垫板
					else if(type==DIANQUAN_STRU::DIANQUAN)
						pRawGasket->type=CRawGasket::WASHER;//垫圈
				}
			}
			pPrePadBolt=pCurPadBolt;
			prebolt_origin=curbolt_origin;
		}
	}
}
void CEmendBoltGasket::InitRawGasketSetAdvance()
{
	hashRawGaskets.Empty();
	//交叉节点集合，key值两依附杆件句柄
	CHash64Set<CLDSNode*> hashCrossNodes;
	for(CLDSNode* pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		if(pNode->m_cPosCalType!=4)//非两杆件交叉点
			continue;
		DUALKEY dualkey(pNode->arrRelationPole[0],pNode->arrRelationPole[1],true);
		if(hashCrossNodes.GetValue(dualkey)==NULL)
			hashCrossNodes.SetValue(dualkey,pNode);
	}
	//1、查找在同一个基准构件上的螺栓集合,并按照螺栓在基准构件上位置进行排序
	//如果基准构件是角钢,找在同一肢法线的螺栓集合
	//建议流程:1.分组;2.根据组基准构件类型,分组计算距离参数;3.分组排序
	CHash64List<CDatumPartBolts> hashPartBoltSet;	//非角钢构件以句柄为键值，角钢以句柄+法线肢
	CDatumPartBolts* pDatumPartBolts=NULL;
	//1、根据基准构件类型对螺栓进行分组（如果基准构件是角钢，找同一肢法线上的螺栓集合）
	for(CBoltGasket *pGasket=hashBoltGaskets.GetFirst();pGasket;pGasket=hashBoltGaskets.GetNext())
	{
		long hPart=pGasket->pBolt->des_base_pos.hPart;
		CAssemblePart* pAssmPart=this->FromAssmPartHandle(hPart);
		//暂仅支持角钢，其余构件上使用垫板较少
		if(pAssmPart->pSmartPart.IsNULL())
			continue;
		if(pAssmPart->pSmartPart->IsAngle())
		{	//应向角钢楞线投影，即保证所有点在同一直线上，然后再算距离
			CLDSPart::HOLE* pHole=pAssmPart->GetBoltHole(pGasket->pBolt->handle);
			DUALKEY dualkey(pHole->ciResidePlane,pAssmPart->hPart);
			pDatumPartBolts=hashPartBoltSet.Add(dualkey);
			pDatumPartBolts->datum_origin=pAssmPart->pSmartPart.pAngle->Start();
			pDatumPartBolts->norm=pHole->ciResidePlane==0?pAssmPart->pSmartPart.pAngle->vxWingNorm:pAssmPart->pSmartPart.pAngle->vyWingNorm;
			pGasket->boltLengthPos=pHole->xLocalPosition.x;	//临时存储便于后序代码排序
			pDatumPartBolts->boltGasketSet.append(pGasket);
		}
		else if(pAssmPart->pSmartPart->GetClassTypeId()==CLS_PLATE)
		{	//基准构件为钢板时应先计算包络正交矩形框,然后向长边投影,并根据投影点算距离
			//norm=pAssmPart->pSmartPart->ucs.axis_z;
		}
	}
	//2、将同一基准构件上的螺栓根据位置进行排序
	for(pDatumPartBolts=hashPartBoltSet.GetFirst();pDatumPartBolts;pDatumPartBolts=hashPartBoltSet.GetNext())
	{
		ARRAY_LIST<BOLTGASKET_PTR> boltGasketArr;
		boltGasketArr.SetSize(0,pDatumPartBolts->boltGasketSet.GetNodeNum());
		for(CBoltGasket* pGasket=pDatumPartBolts->boltGasketSet.GetFirst();pGasket;pGasket=pDatumPartBolts->boltGasketSet.GetNext())
			boltGasketArr.append(pGasket);
		CQuickSort<BOLTGASKET_PTR>::QuickSort(boltGasketArr.m_pData,boltGasketArr.GetSize(),compare_func1);
		pDatumPartBolts->boltGasketSet.Empty();
		for(int i=0;i<boltGasketArr.GetSize();i++)
			pDatumPartBolts->boltGasketSet.append(boltGasketArr[i]);
	}
	//3、根据基准构件上相邻螺栓的距离确定垫片模式
	CRawGasket* pRawGasket=NULL;
	GASKET_WARNING_RECORD* pWarnRecord;
	for(pDatumPartBolts=hashPartBoltSet.GetFirst();pDatumPartBolts;pDatumPartBolts=hashPartBoltSet.GetNext())
	{
		if(pDatumPartBolts->boltGasketSet.GetNodeNum()==1)	//基准构件法线肢上只有单个螺栓
		{
			CBoltGasket* pBoltPad=pDatumPartBolts->boltGasketSet.GetFirst();
			pWarnRecord=hashErrorRecords.GetValue(pBoltPad->pBolt->handle);
			if(pWarnRecord==NULL)
			{	//不需要任何修订
				hashBoltGaskets.DeleteNode(pBoltPad->pBolt->handle);
				hashPartBoltSet.DeleteCursor();
				continue;
			}
			if(pWarnRecord->style!=ERROR_MISSING_CROSSGASKET&&pWarnRecord->style!=ERROR_MISSING_ENDGASKET)
				continue;	//此处不需要增加垫板垫圈
			pRawGasket=hashRawGaskets.Append();
			pBoltPad->feature=1;
			pRawGasket->boltGasketSet.SetValue(pBoltPad->pBolt->handle,pBoltPad);
			int type=CLDSBolt::CalPadPara(pBoltPad->pBolt->get_d(),pBoltPad->gasketThick);
			if(type==DIANQUAN_STRU::DIANBAN)
			{	
				pRawGasket->type=CRawGasket::ONEHOLE;//单孔垫板
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pBoltPad->pBolt);
			}
			else if(type==DIANQUAN_STRU::DIANQUAN)
				pRawGasket->type=CRawGasket::WASHER;//垫圈
			continue;
		}
		f3dPoint prebolt_origin,curbolt_origin;
		CBoltGasket* pPrePadBolt;
		for(pPrePadBolt=pDatumPartBolts->boltGasketSet.GetFirst();pPrePadBolt!=NULL;pPrePadBolt=pDatumPartBolts->boltGasketSet.GetNext())
		{
			pWarnRecord=GetWarningRecord(pPrePadBolt->pBolt->handle);
			if(pWarnRecord==NULL||(pWarnRecord->style!=ERROR_MISSING_CROSSGASKET&&pWarnRecord->style!=ERROR_MISSING_ENDGASKET))
				continue;
			pRawGasket=hashRawGaskets.Append();
			pRawGasket->boltGasketSet.SetValue(pPrePadBolt->pBolt->handle,pPrePadBolt);
			pRawGasket->pCutAngle=GetBoltGasketCutAngle(pPrePadBolt->pBolt);
			break;
		}
		if(pPrePadBolt==NULL)
			continue;	//当前杆件工作肢平面的螺栓不需要添加垫板
		prebolt_origin=pPrePadBolt->pBolt->ucs.origin;
		project_point(prebolt_origin,pDatumPartBolts->datum_origin,pDatumPartBolts->norm);
		for(CBoltGasket* pCurPadBolt=pDatumPartBolts->boltGasketSet.GetNext();pCurPadBolt;pCurPadBolt=pDatumPartBolts->boltGasketSet.GetNext())
		{
			curbolt_origin=pCurPadBolt->pBolt->ucs.origin;
			project_point(curbolt_origin,pDatumPartBolts->datum_origin,pDatumPartBolts->norm);
			pWarnRecord=GetWarningRecord(pCurPadBolt->pBolt->handle);
			if(pWarnRecord==NULL||(pWarnRecord->style!=ERROR_MISSING_CROSSGASKET&&pWarnRecord->style!=ERROR_MISSING_ENDGASKET))
				pRawGasket=NULL;
			else if(pRawGasket==NULL)
			{
				pRawGasket=hashRawGaskets.Append();
				pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pCurPadBolt->pBolt);
			}
			else if(DISTANCE(prebolt_origin,curbolt_origin)<BOLTSPACE_THRESHOLD)//两孔
			{
				if(pRawGasket->boltGasketSet.GetValue(pCurPadBolt->pBolt->handle)==NULL)
				{
					pPrePadBolt->feature=1;//标识已经判断了螺栓所属的垫板模式
					pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				}
				if(pRawGasket->type==0)
				{
					CLDSNode* pCrossNode=GetCrossNode(hashCrossNodes,pPrePadBolt,pCurPadBolt);
					if(pCrossNode)
					{
						pRawGasket->type=CRawGasket::CROSSMULTIHOLE;	//交叉多孔
						pRawGasket->pCrossNode=pCrossNode;
					}
					else 
						pRawGasket->type=CRawGasket::MULTIHOLE;//多孔
				}
			}
			else
			{
				pRawGasket=hashRawGaskets.Append();
				pRawGasket->boltGasketSet.SetValue(pCurPadBolt->pBolt->handle,pCurPadBolt);
				pRawGasket->pCutAngle=GetBoltGasketCutAngle(pCurPadBolt->pBolt);
			}
			pPrePadBolt=pCurPadBolt;
			prebolt_origin=curbolt_origin;
		}
	}
	for(pRawGasket=hashRawGaskets.GetFirst();pRawGasket;pRawGasket=hashRawGaskets.GetNext())
	{
		CBoltGasket* pDatumGasket=pRawGasket->boltGasketSet.GetFirst();
		GASKET_WARNING_RECORD* pErrRec=hashErrorRecords.Add(pDatumGasket->pBolt->handle);
		pErrRec->thick=pDatumGasket->gasketThick;
		pErrRec->idRawGasket=pRawGasket->Id;
		if(pRawGasket->boltGasketSet.GetNodeNum()==1)
		{
			int type=CLDSBolt::CalPadPara(pDatumGasket->pBolt->get_d(),pDatumGasket->gasketThick);
			if(type==DIANQUAN_STRU::DIANBAN)
				pRawGasket->type=CRawGasket::ONEHOLE;//单孔垫板
			else if(type==DIANQUAN_STRU::DIANQUAN)
				pRawGasket->type=CRawGasket::WASHER;//垫圈
		}
		else
		{	//同一块新增垫板仅保留一条错误警示信息
			for(CBoltGasket* pNextGasket=pRawGasket->boltGasketSet.GetNext();pNextGasket;pNextGasket=pRawGasket->boltGasketSet.GetNext())
			{
				hashErrorRecords.DeleteNode(pNextGasket->pBolt->handle);
				hashErrorRecordsView.SetValue(pNextGasket->pBolt->handle,pErrRec);
			}
		}
	}
}
//5、遍历查找rawGasketSet对于填充厚度较小的螺栓进行垫圈修正，然后移除。
void CEmendBoltGasket::EmendBoltWasherInfo()
{
	if(hashBoltGaskets.GetNodeNum()<1)
		return;
	for(CRawGasket* pRaw=hashRawGaskets.GetFirst();pRaw;pRaw=hashRawGaskets.GetNext())
	{
		if(pRaw->type!=CRawGasket::WASHER)
			continue;
		CBoltGasket* pBoltGasket=pRaw->boltGasketSet.GetFirst();
		DIANQUAN_STRU oldDianQuan=pBoltGasket->pBolt->DianQuan;
		pBoltGasket->pBolt->CalPadPara(pBoltGasket->gasketThick);
		pBoltGasket->pBolt->DianQuan.offset=pBoltGasket->washerOffset;
		if(!(oldDianQuan.N==pBoltGasket->pBolt->DianQuan.N&&
			oldDianQuan.thick==pBoltGasket->pBolt->DianQuan.thick))
		{
			GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add(pBoltGasket->pBolt->handle);
			pRecordPart->pPart = pBoltGasket->pBolt;
			if(oldDianQuan.N>0&&oldDianQuan.thick>0)//修正垫圈
			{
				pRecordPart->style= EMEND_GASKET;
				pRecordPart->gasketNum=oldDianQuan.N;
				pRecordPart->thick = oldDianQuan.thick;
			}
			else	//新增垫圈
				pRecordPart->style=NEW_GASKET;
			pBoltGasket->pBolt->SetModified();
			pBoltGasket->pBolt->Create3dSolidModel();
		}
		hashRawGaskets.DeleteCursor();
	}
	hashRawGaskets.Clean();
}
//6、生成未加工垫片,并添加到newGasketSet中
void CEmendBoltGasket::CreateGaskets()
{
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSPlate *pPlate=NULL;
	for(CRawGasket* pRaw=hashRawGaskets.GetFirst();pRaw;pRaw=hashRawGaskets.GetNext())
	{
		CBoltGasket* pFirstBoltGasket=pRaw->boltGasketSet.GetFirst();
		if(pRaw->type!=CRawGasket::CROSSMULTIHOLE)
		{
			BOLTSET boltSet;
			for(CBoltGasket *pBoltGasket=pRaw->boltGasketSet.GetFirst();pBoltGasket;pBoltGasket=pRaw->boltGasketSet.GetNext())
			{
				if(pBoltGasket->pBolt)
				{
					boltSet.append(pBoltGasket->pBolt);
					if(pBoltGasket->pBolt->DianQuan.N>0 ||pBoltGasket->pBolt->DianQuan.thick>0)
					{	//垫圈应升级为垫板处理（相邻同基准构件的垫圈 或 垫圈厚度过大）
						pBoltGasket->pBolt->DianQuan.N=0;
						pBoltGasket->pBolt->DianQuan.thick=0;
					}
				}
			}
			CLDSPart *pDatumPart=Ta.Parts.FromHandle(pFirstBoltGasket->pBolt->des_base_pos.hPart);
			pPlate=pView->DesignBoltPad(pDatumPart,pRaw->pCutAngle,boltSet,pFirstBoltGasket->gasketThick,pFirstBoltGasket->gasketOffset);
		}
		else
			pPlate=DesignCrossPlate(pRaw);
		GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pFirstBoltGasket->pBolt->handle);
		pRecordPart->pBolt=pFirstBoltGasket->pBolt;	
		if(pPlate)
		{
			hashNewGaskets.SetValue(pPlate->handle,pPlate);
			pRecordPart->style=NEW_BOLTPAD;
			pRecordPart->pPart=pPlate;
		}
		else 
		{
			GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pFirstBoltGasket->pBolt->handle);
			pRecordPart->style=EMEND_OTHER;
		}
	}
}
//7、.合并同一段内模式相同的垫板
void CEmendBoltGasket::MergeSimilarGasketInSameSeg()
{
	//1.汇总螺栓垫板
	ATOM_LIST<CLDSPlate*> boltPadList;
	for(CLDSPlate** ppPlate=hashNewGaskets.GetFirst();ppPlate;ppPlate=hashNewGaskets.GetNext())
	{
		if((*ppPlate)->iSeg.iSeg==0 )
			continue;	//不处理0段构件
		boltPadList.append(*ppPlate);
	}
	for(CLDSPlate* pPlate=existingBoltPadSet.GetFirst();pPlate;pPlate=existingBoltPadSet.GetNext())
	{
		if(pPlate->iSeg.iSeg<=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("The segment number of new generation plate 0X%X is undefined!",pPlate->handle);
#else 
			logerr.Log("未给新生成垫板0X%X的段号赋值!",pPlate->handle);
#endif
			continue;	//不处理0段构件
		}
		boltPadList.append(pPlate);
	}
	//2.根据不同的垫板模式对垫板进行分类
	CHashList<CBoltPadProperty> platePropertyHashId;
	ATOM_LIST<SAMESCHEMA_BOLTPAD> sameSchemaBoltPadList;
	for(CLDSPlate** ppBoltPad=boltPadList.GetFirst();ppBoltPad;ppBoltPad=boltPadList.GetNext())
	{
		CBoltPadProperty *pPadProp = platePropertyHashId.Add((*ppBoltPad)->handle);
		pPadProp->InitProperty(*ppBoltPad);
		if(pPadProp->m_iType==CBoltPadProperty::SPECIALBOLTPAD)
		{
			GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add((*ppBoltPad)->handle);
			pRecordPart->style=ERROR_SPECIAL_GASKET;
			pRecordPart->pPart=(*ppBoltPad);
			continue;
		}
		//MODE->SCHEMA
		SAMESCHEMA_BOLTPAD *pSameSchemaBoltPad=NULL;
		for(pSameSchemaBoltPad=sameSchemaBoltPadList.GetFirst();pSameSchemaBoltPad;pSameSchemaBoltPad=sameSchemaBoltPadList.GetNext())
		{	
			if(pSameSchemaBoltPad->IsSameSpec(pPadProp)==1)
			{	//对于相同模式下的垫板，对其顶点信息进行最终匹配标记
				pPadProp->MarkProfileVertexs();
				break;
			}
		}
		if(pSameSchemaBoltPad==NULL)
		{
			pPadProp->MarkProfileVertexs();
			pSameSchemaBoltPad=sameSchemaBoltPadList.append();
			pSameSchemaBoltPad->pDatumProperty=pPadProp;
		}
		pSameSchemaBoltPad->boltPadSet.SetValue((*ppBoltPad)->handle,(*ppBoltPad));
	}
	//3.对于具有相同模式的垫板，以最小外形原则统一调整外形顶点坐标
	for(SAMESCHEMA_BOLTPAD *pSimBoltPad=sameSchemaBoltPadList.GetFirst();pSimBoltPad;pSimBoltPad=sameSchemaBoltPadList.GetNext())
	{
		if(pSimBoltPad->boltPadSet.GetNodeNum()==1)
			continue;
		//1 获取最小外形顶点坐标
		CBoltPadProperty *pBoltPadProp=NULL,*pDatumProperty=pSimBoltPad->pDatumProperty;
		CXhChar16 sPartNo;
		for(CLDSPlate **ppPlate=pSimBoltPad->boltPadSet.GetFirst();ppPlate;ppPlate=pSimBoltPad->boltPadSet.GetNext())
		{
			if((*ppPlate)->handle==pDatumProperty->m_pBoltPad->handle)
				continue;
			pBoltPadProp=platePropertyHashId.GetValue((*ppPlate)->handle);
			pBoltPadProp->UpdateMinProfileVertex(pDatumProperty);
			if(sPartNo.GetLength()<=0&&strlen((*ppPlate)->GetPartNo())>0)
				sPartNo.Copy((*ppPlate)->GetPartNo());
		}
		//2 调整外形顶点坐标
		for(CLDSPlate **ppPlate=pSimBoltPad->boltPadSet.GetFirst();ppPlate;ppPlate=pSimBoltPad->boltPadSet.GetNext())
		{
			pBoltPadProp=platePropertyHashId.GetValue((*ppPlate)->handle);
			if(pBoltPadProp==NULL)
				continue;
			if(sPartNo.GetLength()>0)
				(*ppPlate)->SetPartNo(sPartNo);
			BOOL bRetCode = pBoltPadProp->EmendProfileVertexs(pDatumProperty);
			if(bRetCode && hashNewGaskets.GetValue((*ppPlate)->handle)==NULL)
			{	//修正已有钢板外形
				//TODO:这里使用了钢板板句柄，以后还需要改进
				GASKET_WARNING_RECORD* pRecordPart=this->hashErrorRecords.Add((*ppPlate)->handle);
				pRecordPart->style=ERROR_GASKET_PROFILE;
				pRecordPart->pPart=(*ppPlate);	
			}
		}
	}
}
//执行垫板自动增补修订
void CEmendBoltGasket::Execute(DATACENTERCMD_CONTEXT *pContext,bool advanceMode/*=true*/)
{
	CLogErrorLife loglife(&logerr);
	InitGasketBoltSet(advanceMode);			//遍历所有螺栓，查找需要添加垫圈或垫板的螺栓	
	if(advanceMode)
	{
		StatWireAngles();
		for(CBoltGasket* pGasket=hashBoltGaskets.GetFirst();pGasket;pGasket=hashBoltGaskets.GetNext())
		{
			long hErrPosAngle=0;
			BYTE ciErrorCode=ValidateTheroyGasket(pGasket,&hErrPosAngle);
			if(ciErrorCode==CBoltRelaPartList::TYPE_SUCCESS)	//0.修订成功
				continue;
			if(ciErrorCode==CBoltRelaPartList::TYPE_NONE_CONNECTED)	//1.无关联构件 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_ISOLATED_BOLT;
				pRecordPart->pBolt=pGasket->pBolt;	
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_VIRTUAL_BOLT)	//2.非脚钉螺栓仅连接一个构件 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_VIRTUAL_HOLE_LIKELY;
				pRecordPart->pBolt=pGasket->pBolt;	
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_WIRE_VIRTUAL_BOLT)	//7.挂线孔假螺栓
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_VIRTUAL_WIREHOLE_LIKELY;
				pRecordPart->pAttachPart=FromPartHandle(pGasket->pBolt->des_base_pos.hPart);
				pRecordPart->pBolt=pGasket->pBolt;	
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_MISS_CROSSED_PAD)	//3.交叉点处少垫片 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_MISSING_CROSSGASKET;
				pRecordPart->pBolt=pGasket->pBolt;
				pRecordPart->thick=pGasket->gasketThick;
				pRecordPart->pAttachPart=FromPartHandle(pGasket->pBolt->des_base_pos.hPart);
				pRecordPart->gasketOffset=pGasket->gasketOffset;
				pRecordPart->washerOffset=pGasket->washerOffset;
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_MISS_END_PAD)	//4.端头需要添加螺栓垫片 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_MISSING_ENDGASKET;
				pRecordPart->pBolt=pGasket->pBolt;
				pRecordPart->thick=pGasket->gasketThick;
				pRecordPart->pAttachPart=FromPartHandle(pGasket->pBolt->des_base_pos.hPart);
				pRecordPart->gasketOffset=pGasket->gasketOffset;
				pRecordPart->washerOffset=pGasket->washerOffset;
				//pRecordPart->pCutAngle=GetBoltGasketCutAngle(pGasket->pBolt);	//改为设计垫板时实时提取
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_ERROR_GASKET_THICK)	//6.垫圈厚度不正确 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_GASKET_THICK;
				pRecordPart->pBolt=pGasket->pBolt;
				pRecordPart->thick=pGasket->gasketThick;
			}
			else if(ciErrorCode==CBoltRelaPartList::TYPE_PART_POS_ERROR)	//5.构件位置错误 
			{
				GASKET_WARNING_RECORD* pRecordPart=hashErrorRecords.Add(pGasket->pBolt->handle);
				pRecordPart->style=ERROR_PART_POS;
				pRecordPart->pBolt=pGasket->pBolt;
				pRecordPart->pAttachPart=this->FromPartHandle(hErrPosAngle);
			}
		}
	}
	EmendExistingBoltPadSet();		//修正已存在的垫板，并将关联螺栓从hashBoltGaskets中移除
	if(advanceMode)
	{
		InitRawGasketSetAdvance();
		MergeSimilarGasketInSameSeg();	//合并同一段内外形相近的垫片
	}
	else
	{
		InitRawGasketSet();				//初始化未加工的垫片集合，并初始化垫板模式(单孔、多孔或多孔交叉点垫板)
		EmendBoltWasherInfo();			//遍历查找rawGasketSet中的填充厚度较小的螺栓修正垫圈信息，然后移除
		CreateGaskets();				//生成未加工垫片,并添加到newGasketSet中
		MergeSimilarGasketInSameSeg();	//合并同一段内外形相近的垫片
	}
}
//初始化输出信息列表
void CEmendBoltGasket::InitOutputInfoList(CTaskOutputList* pInfoList)
{
	//1.新增的螺栓垫片
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if( pRecordPart->feature==0&&(pRecordPart->style==NEW_BOLTPAD||
			pRecordPart->style==ERROR_MISSING_CROSSGASKET||pRecordPart->style==ERROR_MISSING_ENDGASKET))
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//2.螺栓垫片厚度修复
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_GASKET_THICK&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//3.螺栓垫片切角处理
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_BOLTPAD_CUTANGLE&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//4.螺栓垫片外形修正(对于切角后的螺栓垫片)
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_GASKET_PROFILE&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//5.新增的螺栓垫圈
	/*for(RECORD_PART* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==NEW_GASKET&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//6.螺栓垫圈修正
	for(RECORD_PART* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==EMEND_GASKET&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}*/
	//7.螺栓垫片冗余
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_REDUNDANT_GASKET&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//8.只连接一个构件的螺栓
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if((pRecordPart->style==ONE_PART_BOLT||pRecordPart->style==ERROR_VIRTUAL_HOLE_LIKELY||
			pRecordPart->style==ERROR_VIRTUAL_WIREHOLE_LIKELY)&&pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//9.
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->style==ERROR_SPECIAL_GASKET && pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
	//10.其余
	for(GASKET_WARNING_RECORD* pRecordPart = hashErrorRecords.GetFirst();pRecordPart;pRecordPart=hashErrorRecords.GetNext())
	{
		if(pRecordPart->feature==0)
			AppendOutputInfoToList(pRecordPart,pInfoList);
	}
}
BOOL CalAndSortIntersPoint(ATOM_LIST<INTERS_POINT> &interPointList,CLDSPart *pPart,CLDSBolt* pBolt)
{
	if(pPart==NULL)
		return FALSE;
	//1、根据指定的螺栓法线，得到构件中间层拾取点及法线方向
	f3dPoint face_pick,face_norm,bolt_norm;
	bolt_norm=pBolt->ucs.axis_z;
	if(pPart->GetClassTypeId()==CLS_PLATE)
	{	//普通钢板
		CLDSPlate *pDatumPlate=(CLDSPlate*)pPart;
		/*int iFace=pDatumPlate->GetNormFaceI(bolt_norm);
		if(iFace==0)
		{
			face_pick = pDatumPlate->ucs.origin;
			face_norm = pDatumPlate->ucs.axis_z;
		}
		else if(iFace==1||iFace==2)
		{
			face_pick=pDatumPlate->HuoQuLine[iFace-1].startPt;
			coord_trans(face_pick,pDatumPlate->ucs,TRUE);
			face_norm = pDatumPlate->huoqufacenorm[iFace-1];
		}
		else
			return FALSE;
		face_pick+=pDatumPlate->ucs.axis_z*(pDatumPlate->m_fNormOffset+0.5*pDatumPlate->GetThick());
		*/
		int iResidePlane=0;
		if(!pDatumPlate->GetBoltIntersPos(pBolt,face_pick,&iResidePlane,CLDSPlate::DATUM_PLANE))
			return FALSE;
		face_norm=iResidePlane==0?pDatumPlate->ucs.axis_z:pDatumPlate->GetHuoquFaceNorm(iResidePlane);
		face_pick+=face_norm*(0.5*pDatumPlate->GetThick());
	}
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
	{	//参数化板
		CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPart;
		double dd1=fabs(pParamPlate->ucs.axis_z*bolt_norm);
		if(dd1<EPS_COS2)
			return FALSE;
		if(pParamPlate->m_iParamType==TYPE_UEND||
			pParamPlate->m_iParamType==TYPE_XEND||
			pParamPlate->m_iParamType==TYPE_ROLLEND)
			return FALSE;	//U型、卷边插板、十字插板不需要添加垫片
		else 
			face_pick=pParamPlate->ucs.origin;
		face_norm = pParamPlate->ucs.axis_z;
	}
	else if(pPart->IsAngle())
	{
		CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pPart;
		double dd1=fabs(pLineAngle->get_norm_x_wing()*bolt_norm);
		double dd2=fabs(pLineAngle->get_norm_y_wing()*bolt_norm);
		if(dd1>dd2&&dd1>EPS_COS2)
			face_norm = pLineAngle->get_norm_x_wing();
		else if(dd2>dd1&&dd2>EPS_COS2)
			face_norm = pLineAngle->get_norm_y_wing();
		else 
			return FALSE;
		face_pick=pLineAngle->Start()-face_norm*pLineAngle->GetThick()*0.5;
	}
	else if(pPart->GetClassTypeId()==CLS_LINESLOT)
	{
		CLDSLineSlot *pSlot=(CLDSLineSlot*)pPart;
		face_norm = pSlot->WorkPlaneNorm();
		pSlot->BuildUCS();
		face_pick=pSlot->ucs.origin-face_norm*pSlot->GetThick()*0.5;
		double dd1=fabs(face_norm*bolt_norm);
		if(dd1<EPS_COS2)
			return FALSE;
	}
	else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
	{
		CLDSLineFlat *pFlat=(CLDSLineFlat*)pPart;
		face_norm = pFlat->WorkPlaneNorm();
		pFlat->BuildUCS();
		face_pick=pFlat->ucs.origin+face_norm*pFlat->GetThick()*0.5;
		double dd1=fabs(face_norm*bolt_norm);
		if(dd1<EPS_COS2)
			return FALSE;
	}
	//2、计算构件中间层到螺栓原点的距离，并进行排序
	f3dPoint inter_pos;
	f3dLine line(pBolt->ucs.origin,pBolt->ucs.origin+pBolt->ucs.axis_z*1000);
	if(Int3dlf(inter_pos,line,face_pick,face_norm)!=1)
		return FALSE;
	INTERS_POINT interPt(pPart,inter_pos,CalSignedDistance(pBolt->ucs.origin,inter_pos,pBolt->ucs.axis_z));
	INTERS_POINT* pInters;
	for(pInters=interPointList.GetFirst();pInters;pInters=interPointList.GetNext())
	{
		if(pInters->distance > interPt.distance)
		{
			interPointList.insert(interPt);
			break;
		}
	}
	if(pInters==NULL)
		interPointList.append(interPt);
	return TRUE;
}
void InitBoltGasketThickAndOffset(ATOM_LIST<INTERS_POINT> &interPointList,CBoltGasket* pBoltGasket)
{
	if(interPointList.GetNodeNum()<=0)
		return;
	INTERS_POINT* pDatumIntersPoint=NULL;
	int iDatumIndex=-1;
	for(INTERS_POINT* pInters=interPointList.GetFirst();pInters;pInters=interPointList.GetNext())
	{
		iDatumIndex++;
		if(pBoltGasket->pBolt->des_base_pos.hPart==pInters->pPart->handle)
		{
			pDatumIntersPoint=pInters;
			break;
		}
	}
	if(pDatumIntersPoint==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("It can't find the datum part by the bolt's reference",pBoltGasket->pBolt->handle);
#else 
		logerr.Log("根据此螺栓引用0X%X找不到基准构件",pBoltGasket->pBolt->handle);
#endif
		return;
	}
	f3dPoint norm;
	if(pDatumIntersPoint->pPart->IsAngle())
	{
		CLDSLineAngle* pAngle = (CLDSLineAngle*)pDatumIntersPoint->pPart;
		int x0_y1=2;
		if(pAngle->IsLsInWing(pBoltGasket->pBolt,&x0_y1))
		{	
			if(x0_y1==0)
				norm=pAngle->get_norm_x_wing();
			else if(x0_y1==1)
				norm=pAngle->get_norm_y_wing();
		}
	}
	else
		norm=pDatumIntersPoint->pPart->ucs.axis_z;
	INTERS_POINT* pPrevPt = interPointList.GetFirst();
	for(INTERS_POINT *pCurPt = interPointList.GetNext();pCurPt;pCurPt=interPointList.GetNext())
	{
		double fFullDist,fExistDist,fSpacing;
		fFullDist=DISTANCE(pPrevPt->inters_pt,pCurPt->inters_pt);
		fExistDist=pPrevPt->GetHalfThick()+pCurPt->GetHalfThick();
		fSpacing=fFullDist-fExistDist;
		if(fabs(fSpacing)<CEmendBoltGasket::MINSPACING)
		{
			pPrevPt=pCurPt;
			continue;	//不考虑接近0的间隙值
		}
		if(fSpacing > pBoltGasket->gasketThick)
		{
			pBoltGasket->gasketThick = fSpacing;
			//垫板：偏移量自基准构件算起,垫片法线与基准构件法线一致
			//TODO: 未处理垫板法线与基准构件法线不一致情况
			//TODO: 建议写成独立函数,并同时完成该函数的测试代码
			int index=interPointList.GetCurrentIndex();
			if(norm*pBoltGasket->pBolt->ucs.axis_z>0)	//螺栓法线与基准构件法线同向
			{
				double halfThick2 = pCurPt->GetHalfThick()+pDatumIntersPoint->GetHalfThick();
				if(iDatumIndex>=index)
					pBoltGasket->gasketOffset=-DISTANCE(pCurPt->inters_pt,pDatumIntersPoint->inters_pt)-pBoltGasket->gasketThick-halfThick2;
				else
					pBoltGasket->gasketOffset=DISTANCE(pCurPt->inters_pt,pDatumIntersPoint->inters_pt)-pBoltGasket->gasketThick-halfThick2;
			}
			else	//螺栓法线与基准构件法线反向
			{
				double halfThick2 = pCurPt->GetHalfThick()-pDatumIntersPoint->GetHalfThick();
				if(iDatumIndex>=index)
					pBoltGasket->gasketOffset=DISTANCE(pCurPt->inters_pt,pDatumIntersPoint->inters_pt)+halfThick2;
				else  
					pBoltGasket->gasketOffset=-DISTANCE(pCurPt->inters_pt,pDatumIntersPoint->inters_pt)+halfThick2;
			}
			//以上算出的垫板偏移距都是以角钢为基准构件的，如果基准构件不是角钢，偏移距还需沿基准构件法线方向偏移一个厚度
			if(!pDatumIntersPoint->pPart->IsAngle())
				pBoltGasket->gasketOffset+=pDatumIntersPoint->pPart->GetThick();
			//用 type应定义为DIANQUAN::下的静态常量 
			if(CLDSBolt::CalPadPara(pBoltGasket->pBolt->get_d(),fSpacing)==DIANQUAN_STRU::DIANQUAN) //垫圈:偏移量自螺栓算起,垫圈法线和螺栓法线一致
			{
				f3dPoint pos=pBoltGasket->pBolt->ucs.origin;
				pBoltGasket->washerOffset=DISTANCE(pCurPt->inters_pt,pos)-fFullDist+pPrevPt->GetHalfThick();
			}
		}
		pPrevPt=pCurPt;
	}
}
CLDSLineAngle *GetBoltGasketCutAngle(CLDSBolt* pBolt)
{
	CLDSPart *pDatumPart=Ta.Parts.FromHandle(pBolt->des_base_pos.hPart);
	if(pDatumPart->IsLinePart())
	{
		CLDSLinePart *pRod=(CLDSLinePart*)pDatumPart;
		double fStartDist=DISTANCE(pBolt->ucs.origin,pRod->Start());
		double fEndDist=DISTANCE(pBolt->ucs.origin,pRod->End());
		CLDSNode *pNode=NULL;
		if(fStartDist>fEndDist&&pRod->pEnd)
			pNode=pRod->pEnd;
		else if(pRod->pStart)
			pNode=pRod->pStart;
		else 
			return NULL;
		CLDSLineAngle *pCutAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pNode->hFatherPart,CLS_LINEANGLE);
		if(pCutAngle&&pCutAngle->handle!=pDatumPart->handle)
			return pCutAngle;
	}
	return NULL;
}
CLDSNode* GetCrossNode(CHash64Set<CLDSNode*> &hashCrossNodes,CBoltGasket* pBoltGasket1,CBoltGasket* pBoltGasket2)
{
	if(pBoltGasket1->pBolt->des_base_pos.datumPoint.datum_pos_style!=3||
		pBoltGasket1->pBolt->des_base_pos.datumPoint.datum_pos_style!=3)
		return NULL;
	THANDLE handle1_1 =pBoltGasket1->pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
	THANDLE handle1_2 =pBoltGasket1->pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
	THANDLE handle2_1 =pBoltGasket2->pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum1;
	THANDLE handle2_2 =pBoltGasket2->pBolt->des_base_pos.datumPoint.des_para.AXIS_INTERS.hDatum2;
	if((handle1_1==handle2_1&&handle1_2==handle2_2)||(handle1_1==handle2_2&&handle1_2==handle2_1))
	{
		DUALKEY dualkey(handle1_1,handle1_2,true);
		return hashCrossNodes.GetValue(dualkey);
	}
	return NULL;
}
//TODO: 建议与CDesignJdb::DesignCrossPlank中iProfileCalType01==2时代码进行集成
CLDSPlate* DesignCrossPlate(CRawGasket *pRawGasket)
{
	if(pRawGasket==NULL||pRawGasket->type!=3||pRawGasket->pCrossNode==NULL)
		return NULL;
	CLDSNode *pBaseNode=pRawGasket->pCrossNode;
	CBoltGasket *pBoltGasket=pRawGasket->boltGasketSet.GetFirst();
	if(pBoltGasket==NULL)
		return NULL;
	// 1.获取基准角钢及交叉角钢，以肢宽较大的角钢为基准角钢
	CLDSLineAngle *pBaseAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pBaseNode->arrRelationPole[0],CLS_LINEANGLE);
	CLDSLineAngle *pAnotherJg=(CLDSLineAngle*)Ta.Parts.FromHandle(pBaseNode->arrRelationPole[1],CLS_LINEANGLE);
	if(pAnotherJg==NULL||pBaseAngle==NULL||pBaseAngle->handle==pAnotherJg->handle)
		return FALSE;
	// 2.判断两交叉角钢是否干涉
	BOOL IsJgCollide=FALSE;
	double base_wing_wide = pBaseAngle->GetWidth();
	double other_wing_wide = pAnotherJg->GetWidth();
	double ddXX = pBaseAngle->get_norm_x_wing()*pAnotherJg->get_norm_x_wing();
	double ddXY = pBaseAngle->get_norm_x_wing()*pAnotherJg->get_norm_y_wing();
	double ddYX = pBaseAngle->get_norm_y_wing()*pAnotherJg->get_norm_x_wing();
	double ddYY = pBaseAngle->get_norm_y_wing()*pAnotherJg->get_norm_y_wing();
	f3dPoint axis_z;
	int norm_wing=0;
	if(IsMaxInAll(ddXX,ddXY,ddYX,ddYY))
	{
		if(ddXX>0)	//两交叉材角钢相互干涉
			IsJgCollide=TRUE;
		axis_z = pBaseAngle->get_norm_x_wing();
		norm_wing=0;
	}
	else if(IsMaxInAll(ddXY,ddXX,ddYX,ddYY))
	{
		if(ddXY>0)	//两交叉材角钢相互干涉
			IsJgCollide=TRUE;
		axis_z = pBaseAngle->get_norm_x_wing();
		norm_wing=0;
	}
	else if(IsMaxInAll(ddYX,ddXY,ddXX,ddYY))
	{
		if(ddYX>0)	//两交叉材角钢相互干涉
			IsJgCollide=TRUE;
		axis_z = pBaseAngle->get_norm_y_wing();
		norm_wing=1;
	}
	else// if(IsMaxInAll(ddYY,ddXY,ddXX,ddYX))
	{
		if(ddYY>0)	//两交叉材角钢相互干涉
			IsJgCollide=TRUE;
		axis_z = pBaseAngle->get_norm_y_wing();
		norm_wing=1;
	}
	if(IsJgCollide)	//两交叉材角钢相互干涉
		return NULL;
	// 3.生成交叉点螺栓垫板并设置参数
	// 3.1 生成螺栓垫板并设置基本信息
	CLDSPlate *pPlate=(CLDSPlate*)console.AppendPart(CLS_PLATE);
	pPlate->face_N = 1;
	pPlate->cMaterial = pBaseAngle->cMaterial;
	pPlate->Thick=FindPlankThick(pBoltGasket->gasketThick);
	pPlate->iSeg=pBaseAngle->iSeg;
	pPlate->jdb_style = 4;	//交叉螺栓垫板
	pPlate->m_fNormOffset = ftoi(pBoltGasket->gasketOffset);	//法向偏移量
	// 3.2. 设置螺栓垫板设计信息
	pPlate->designInfo.m_bEnableFlexibleDesign=FALSE;
	pPlate->designInfo.iFaceType=1;
	pPlate->designInfo.iProfileStyle0123=3; 
	pPlate->designInfo.m_hBaseNode = pBaseNode->handle;
	pPlate->designInfo.m_hBasePart=pBaseNode->arrRelationPole[0];
	pPlate->designInfo.norm.norm_style=1;
	pPlate->designInfo.norm.norm_wing=norm_wing;
	pPlate->designInfo.norm.hVicePart=pBaseAngle->handle;
	pPlate->designInfo.norm.direction=0;
	pPlate->designInfo.origin.datum_pos_style=3;
	pPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum1=pBaseAngle->handle;
	pPlate->designInfo.origin.des_para.AXIS_INTERS.hDatum2=pAnotherJg->handle;
	// 3.3. 建立交叉板的相对坐标系
	pBaseNode->GetIntersZhun(&pPlate->ucs.origin);
	f3dPoint perp;
	SnapPerp(&perp,pBaseAngle->Start(),pBaseAngle->End(),pPlate->ucs.origin,NULL);
	project_point(pPlate->ucs.origin,perp,axis_z);
	pPlate->ucs.axis_z = axis_z;
	pPlate->ucs.axis_y = pBaseAngle->End()-pBaseAngle->Start();
	pPlate->ucs.axis_x = pPlate->ucs.axis_y^pPlate->ucs.axis_z;
	pPlate->ucs.axis_y = pPlate->ucs.axis_z^pPlate->ucs.axis_x;
	normalize(pPlate->ucs.axis_x);
	normalize(pPlate->ucs.axis_y);
	normalize(pPlate->ucs.axis_z);
	// 3.4. 添加螺栓引用
	for(CBoltGasket *pBoltGasket=pRawGasket->boltGasketSet.GetFirst();pBoltGasket;pBoltGasket=pRawGasket->boltGasketSet.GetNext())
	{
		CLDSBolt *pBolt=pBoltGasket->pBolt;
		if(pBolt==NULL)
			continue;
		pBolt->DianQuan.N=0;
		pBolt->DianQuan.thick=0;
		pPlate->AppendLsRef(pBolt->GetLsRef());
		pBolt->AddL0Thick(pPlate->handle,TRUE);
		pBolt->CalGuigeAuto();
		pBolt->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
		g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
	}
	//4. 生成交叉螺栓垫板
	if(!pPlate->DesignRhombusPlate())
		return NULL;
	pPlate->SetModified();
	pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
	g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
	g_pSolidDraw->Draw();
	return pPlate;
}
void CEmendBoltGasket::AppendOutputInfoToList(GASKET_WARNING_RECORD* pRecordPart,CTaskOutputList* pInfoList)
{
	if(pRecordPart->pBolt==NULL&&pRecordPart->style==0)
	{
		logerr.Log("error");
		return;
	}
	CCheckGasketErrorInfo* pInfo=(CCheckGasketErrorInfo*)pInfoList->Add(0,3);
	pRecordPart->feature=1;
	pInfo->hRelaBolt=pRecordPart->pBolt!=NULL?pRecordPart->pBolt->handle:0;
	pInfo->hRelaGasket=pRecordPart->pGasketPlate!=NULL?pRecordPart->pGasketPlate->handle:0;
	pInfo->m_uType=pRecordPart->style;
	//if(pRecordPart->style==ERROR_ISOLATED_BOLT)		//无关联构件的孤立螺栓, 处理方式为:忽略或确认清除
	//	;
	//if(pRecordPart->style==ERROR_REDUNDANT_GASKET)	//冗余垫板，处理方式为：清除垫板
	//	;
	if( pRecordPart->style==ERROR_MISSING_CROSSGASKET||	//交叉点处垫板缺失，处理方式为：添加垫板或垫圈
		pRecordPart->style==ERROR_MISSING_ENDGASKET)	//端连接处垫板缺失，处理方式为：添加垫板或垫圈
	{
		CRawGasket* pRawGasket=this->hashRawGaskets.GetValue(pRecordPart->idRawGasket);
		if(pRawGasket==NULL)
			return;	//TODO:正常不应执行到此处 2017.12.8
		CBoltGasket* pGasket=pRawGasket->boltGasketSet.GetFirst();
		if(pGasket)
			pInfo->siGasketThick=(short)ftoi(pGasket->gasketThick);
		for(;pGasket;pGasket=pRawGasket->boltGasketSet.GetNext())
			pInfo->relaObjIds.Append(pGasket->pBolt->handle);
		if(pRecordPart->pAttachPart)
			pInfo->anchor.hAnchorPart=pRecordPart->pAttachPart->handle;
		pInfo->anchor.gasketOffset=pRecordPart->gasketOffset;
		pInfo->anchor.washerOffset=pRecordPart->washerOffset;
	}
	else if(pRecordPart->style==ERROR_GASKET_THICK)
	{
		pInfo->siGasketThick=(short)(pRecordPart->thick+0.5);
	}
	else if(pRecordPart->style==ERROR_VIRTUAL_HOLE_LIKELY||pRecordPart->style==ERROR_VIRTUAL_WIREHOLE_LIKELY)
	{
		if(pRecordPart->pAttachPart)
			pInfo->m_hRelaObj2=pRecordPart->pAttachPart->handle;	//仅有的关联构件
	}
	else if(pRecordPart->style==ERROR_PART_POS)			//构件位置错误
	{
		if(pRecordPart->pAttachPart)
			pInfo->m_hRelaObj2=pRecordPart->pAttachPart->handle;
	}
	else if(pRecordPart->style==ERROR_BOLTPAD_CUTANGLE)	//垫板缺少切角处理
	{
		if(pRecordPart->pCutAngle)
			pInfo->relaObjIds.Append(pRecordPart->pCutAngle->handle);
	}
	else if(pRecordPart->style==ERROR_SPECIAL_GASKET)	//特殊垫板
	{
		if(pRecordPart->pGasketPlate)
			pInfo->relaObjIds.Append(pRecordPart->pGasketPlate->handle);
	}
	//else if(pRecordPart->style==ERROR_GASKET_PROFILE)		//垫板外形不合适，处理方式为：修正垫板外形
	//	;
	pInfo->SetMessage(pRecordPart->GetDescription());
};
//
GASKET_WARNING_RECORD::GASKET_WARNING_RECORD()
{
	idDatumBolt=0;
	style=EMENDGASKET_TYPE(0);
	thick=0;
	gasketNum=0;
	pPart=pBolt=NULL;
	pAttachPart=NULL;
	pCutAngle=NULL;
	gasketOffset=0;//垫板自基准螺栓在基准构件上的穿插交点的偏移值，用来确定垫板位置（基准构件的工作法线为偏移正向)
	washerOffset=0;//垫圈自螺栓的偏移距
	feature=idRawGasket=0;
}
CXhChar50 GASKET_WARNING_RECORD::GetDescription()
{
	if(style==ERROR_ISOLATED_BOLT)		//无关联构件的孤立螺栓, 处理方式为:忽略或确认清除
		return CXhChar50("0x%X为冗余孤立螺栓",pBolt->handle);
	else if(style==ERROR_VIRTUAL_HOLE_LIKELY||style==ONE_PART_BOLT)	//目前有一个关联构件的非脚钉疑似虚拟孔，处理方式为：确认设为虚拟螺栓孔
	{
#ifdef AFX_TARG_ENU_ENGLISH
		return CXhChar50("0x%X bolt connects only one part. It's a dummy bolt.",pBolt->handle);
#else 
		return CXhChar50("0x%X为单构件连接的假螺栓",pBolt->handle);
#endif
	}
	else if(style==ERROR_VIRTUAL_WIREHOLE_LIKELY)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		return CXhChar50("0x%X bolt is wire hole. It's a dummy bolt.",pBolt->handle);
#else 
		return CXhChar50("0x%X为挂线孔假螺栓",pBolt->handle);
#endif
	}
	else if(style==ERROR_MISSING_CROSSGASKET)	//交叉点处垫板缺失，处理方式为：添加垫板或垫圈
		return CXhChar50("0x%X螺栓处交叉点垫板(圈)缺失(-%g)",pBolt->handle,this->thick);
	else if(style==ERROR_MISSING_ENDGASKET)	//端连接处垫板缺失，处理方式为：添加垫板或垫圈
		return CXhChar50("0x%X螺栓处端头垫板(圈)缺失(-%g)",pBolt->handle,this->thick);
	else if(style==ERROR_GASKET_THICK)			//垫板或垫圈厚度错误，处理方式为：重新修订垫板或垫圈厚度
	{
		if(pGasketPlate)
			return CXhChar50("0x%X垫板厚度错误(%g->%g)",pGasketPlate->handle,pGasketPlate->Thick,this->thick);
		else
			return CXhChar50("0x%X螺栓垫圈厚度错误(%d->%g)",pBolt->handle,pBolt->DianQuan.thick*pBolt->DianQuan.N,this->thick);
	}
	else if(style==ERROR_PART_POS)				//构件位置错误
		return CXhChar50("0x%X螺栓处端头构件位置错误",pBolt->handle);
	else if(style==ERROR_BOLTPAD_CUTANGLE)		//垫板缺少切角处理
		return CXhChar50("0x%X垫板需与靠近角钢进行切角处理",pGasketPlate->handle);
	else if(style==ERROR_GASKET_PROFILE)		//垫板外形不合适，处理方式为：修正垫板外形
		return CXhChar50("0x%X垫板需与近似切角垫板统一化处理",pGasketPlate->handle);
	else if(style==ERROR_REDUNDANT_GASKET)		//冗余垫板，处理方式为：清除垫板
		return CXhChar50("0x%X垫板冗余",pGasketPlate->handle);
	else if(style==ERROR_SPECIAL_GASKET)
		return CXhChar50("0x%X特殊垫板",pGasketPlate->handle);
	else if(pPart==NULL)
		return "";
	//
	CString sDescription="";
	if(style==NEW_BOLTPAD)	
#ifdef AFX_TARG_ENU_ENGLISH
		sDescription.Format("0X%X  create new bolt's gasket",pPart->handle);
	else if(style==ERROR_GASKET_THICK)
		sDescription.Format("Correct gasket's thickness of 0X%X bolt:%d->%d",pPart->handle,ftoi(thick),ftoi(pPart->GetThick()));
	else if(style==ERROR_BOLTPAD_CUTANGLE)
		sDescription.Format("0X%X bolt's gasket cut angle",pPart->handle);
	else if(style==ERROR_GASKET_PROFILE)
		sDescription.Format("Correct the outline of 0x%X bolt's gasket in %s segment",pPart->handle,(char*)pPart->iSeg.ToString());
	else if(style==ERROR_REDUNDANT_GASKET)
		sDescription.Format("0x%X bolt's gasket is redundant",pPart->handle);
#else 
		sDescription.Format("0X%X新增螺栓垫片",pPart->handle);
	else if(style==ERROR_GASKET_THICK)
		sDescription.Format("0X%X螺栓垫片厚度修正:%d->%d",pPart->handle,ftoi(thick),ftoi(pPart->GetThick()));
	else if(style==ERROR_BOLTPAD_CUTANGLE)
		sDescription.Format("0X%X螺栓垫片进行切角处理",pPart->handle);
	else if(style==ERROR_GASKET_PROFILE)
		sDescription.Format("%s段0x%X螺栓垫片外形修正",(char*)pPart->iSeg.ToString(),pPart->handle);
	else if(style==ERROR_REDUNDANT_GASKET)
		sDescription.Format("0x%X螺栓垫片为冗余构件",pPart->handle);
#endif
	else if(style==NEW_GASKET)
	{
		CLDSBolt* pBolt = (CLDSBolt*)pPart;
#ifdef AFX_TARG_ENU_ENGLISH
		sDescription.Format("0x%X create new bolt's washer:%d*%d",pPart->handle,pBolt->DianQuan.N,pBolt->DianQuan.thick);
#else 
		sDescription.Format("0x%X新增螺栓垫圈:%d*%d",pPart->handle,pBolt->DianQuan.N,pBolt->DianQuan.thick);
#endif
	}
	else if(style==EMEND_GASKET)
	{
		CLDSBolt* pBolt = (CLDSBolt*)pPart;
#ifdef AFX_TARG_ENU_ENGLISH
		sDescription.Format("Correct 0x%X bolt's washer:%d*%d->%d*%d",pBolt->handle,gasketNum,ftoi(thick),pBolt->DianQuan.N,pBolt->DianQuan.thick);
#else 
		sDescription.Format("0x%X螺栓垫圈修正:%d*%d->%d*%d",pBolt->handle,gasketNum,ftoi(thick),pBolt->DianQuan.N,pBolt->DianQuan.thick);
#endif
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		sDescription.Format("0x%X can't be corrected ",pPart->handle);
#else 
		sDescription.Format("0x%X无法进行修正",pPart->handle);
#endif
	return CXhChar50((const char*)sDescription);
}
CBoltPadProperty::CBoltPadProperty()
{
	m_pBoltPad=NULL;
	m_iSignIndex=-1;
	m_iType=-1;
}
void CBoltPadProperty::InitProperty(CLDSPlate* pPlate)
{	
	m_pBoltPad=pPlate;
	double cos90=cos(0.5*Pi);
	//删除垫板中的重复顶点
	PROFILE_VERTEX* pVertex=NULL,*pNextVertex=NULL;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{
		int nPush=pPlate->vertex_list.push_stack();
		for(pNextVertex=pPlate->vertex_list.GetNext();pNextVertex;pNextVertex=pPlate->vertex_list.GetNext())
		{
			if(pVertex->vertex.IsEqual(pNextVertex->vertex))
				pPlate->vertex_list.DeleteCursor();
		}
		pPlate->vertex_list.pop_stack(nPush);
	}
	pPlate->vertex_list.Clean();
	//统计垫板外形直角顶点数量并进行标记
	int nRectangularNum=0,n=pPlate->vertex_list.GetNodeNum();
	vertexItemArr.SetSize(0,n);
	for(int i=0;i<n;i++)
	{
		VERTEX_ITEM *pVertexItem=vertexItemArr.append();
		pVertexItem->pVertex=&(pPlate->vertex_list[i]);
		pVertexItem->bRectangular=FALSE;	//通过append()添加的顶点项中bRectangular=TRUE,此处要进行初始化
		f3dPoint vec_cur = pPlate->vertex_list[(i+1)%n].vertex-pPlate->vertex_list[i].vertex;
		f3dPoint vec_prev= pPlate->vertex_list[(n+i-1)%n].vertex-pPlate->vertex_list[i].vertex;
		double cosa = vec_cur.normalized()*vec_prev.normalized();
		if(fabs(cosa-cos90)<EPS)
		{
			pVertexItem->bRectangular=TRUE;
			nRectangularNum++;
		}
	}
	if(nRectangularNum==3)
	{	//存在三个直角顶点的垫板是切角垫板
		m_iType=CUTANGELBOLTPAD;
		InitCutAnglePadProperty();
	}
	else if(nRectangularNum==4)	
	{	//存在四个直角顶点的垫板是矩形垫板
		m_iType=RECTANGLEBOLTPAD;
		InitRectanglePadProperty();
	}
	else
	{	//非正规垫板(菱形垫板或轮廓点有误)
		m_iType=SPECIALBOLTPAD;
	}
}
void CBoltPadProperty::InitCutAnglePadProperty()
{
	//1、查找第二个直角顶点，以此顶点为原点，以长边为X轴，短边为Y轴建立坐标系
	UCS_2dSTRU ucs2d;
	int n=vertexItemArr.GetSize();
	for(int i=0;i<n;i++)
	{
		VERTEX_ITEM *pPrevItem=&vertexItemArr[(n+i-1)%n];
		VERTEX_ITEM *pCurItem=&vertexItemArr[i%n];
		VERTEX_ITEM *pNextItem=&vertexItemArr[(i+1)%n];
		if(pPrevItem->bRectangular&&pCurItem->bRectangular&&pNextItem->bRectangular)
		{	
			f3dPoint curVec=pNextItem->pVertex->vertex-pCurItem->pVertex->vertex;
			double fCurLen=curVec.mod();
			normalize(curVec);
			f3dPoint preVec=pPrevItem->pVertex->vertex-pCurItem->pVertex->vertex;
			double fPreLen=preVec.mod();
			normalize(preVec);
			if(fCurLen>fPreLen)
			{
				ucs2d.axis_x.Set(curVec.x,curVec.y);
				ucs2d.axis_y.Set(preVec.x,preVec.y);
			}
			else
			{
				ucs2d.axis_x.Set(preVec.x,preVec.y);
				ucs2d.axis_y.Set(curVec.x,curVec.y);
			}
			ucs2d.origin.Set(pCurItem->pVertex->vertex.x,pCurItem->pVertex->vertex.y);
			m_iSignIndex=i;
			break;
		}
	}
	//2、将垫板上的螺栓坐标转换到上述坐标系下，确定左下角螺栓建立匹配模式坐标系
	ATOM_LIST<f2dPoint>	boltVertexList;
	double fMinX=100,fMinY=100;
	f3dPoint ls_3dPos;
	f2dPoint vertex2d;
	for(CLsRef* pLsRef=m_pBoltPad->GetFirstLsRef();pLsRef;pLsRef=m_pBoltPad->GetNextLsRef())
	{
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		Int3dlf(ls_3dPos,pBolt->ucs.origin,pBolt->ucs.axis_z,m_pBoltPad->ucs.origin,m_pBoltPad->ucs.axis_z);
		coord_trans(ls_3dPos,m_pBoltPad->ucs,FALSE);
		vertex2d.Set(ls_3dPos.x,ls_3dPos.y);
		coord_trans(vertex2d,ucs2d,FALSE);
		if(vertex2d.x<=fMinX+0.5 && vertex2d.y<=fMinY+0.5)
		{
			fMinX=vertex2d.x;
			fMinY=vertex2d.y;
		}
		coord_trans(vertex2d,ucs2d,TRUE);
		vertex2d.feature=pBolt->get_d();	//记录螺栓直径
		boltVertexList.append(vertex2d);
	}
	vertex2d.Set(fMinX,fMinY);
	coord_trans(vertex2d,ucs2d,TRUE);
	modeUcs=ucs2d;
	modeUcs.origin=vertex2d;
	//3、将切角垫板螺栓转换到匹配模式坐标系下
	boltItemArr.SetSize(0,boltVertexList.GetNodeNum());
	for(f2dPoint* pLsPos=boltVertexList.GetFirst();pLsPos;pLsPos=boltVertexList.GetNext())
	{
		coord_trans(*pLsPos,modeUcs,FALSE);
		BOLT_ITEM* pBoltItem=boltItemArr.append();
		pBoltItem->vertex=*pLsPos;
		pBoltItem->d=pLsPos->feature;
	}
}
void CBoltPadProperty::InitRectanglePadProperty()
{
	//1、遍历顶点分别以四个顶点为原点、长边为X轴、短边为Y轴，建立坐标系
	int n=vertexItemArr.GetSize();
	for(int i=0;i<n;i++)
	{
		f3dPoint curVertex=vertexItemArr[i%n].pVertex->vertex;
		f3dPoint curVec=vertexItemArr[(i+1)%n].pVertex->vertex-curVertex;
		double fCurLen=curVec.mod();
		normalize(curVec);
		f3dPoint preVec=vertexItemArr[(n+i-1)%n].pVertex->vertex-curVertex;
		double fPreLen=preVec.mod();
		normalize(preVec);
		if(fCurLen>=fPreLen)
		{
			cs2d[i].axis_x.Set(curVec.x,curVec.y);
			cs2d[i].axis_y.Set(preVec.x,preVec.y);
		}
		else
		{
			cs2d[i].axis_x.Set(preVec.x,preVec.y);
			cs2d[i].axis_y.Set(curVec.x,curVec.y);
		}
		cs2d[i].origin.Set(curVertex.x,curVertex.y);
	}
	//2、将垫板上的螺栓坐标转换到上述坐标系下，确定左下角螺栓并建立匹配模式坐标系
	ATOM_LIST<f2dPoint> boltVertexList;
	f3dPoint lsPos;
	f2dPoint vertex2d;
	for(CLsRef* pLsRef=m_pBoltPad->GetFirstLsRef();pLsRef;pLsRef=m_pBoltPad->GetNextLsRef())
	{
		CLDSBolt* pBolt=pBolt=pLsRef->GetLsPtr();
		Int3dlf(lsPos,pBolt->ucs.origin,pBolt->ucs.axis_z,m_pBoltPad->ucs.origin,m_pBoltPad->ucs.axis_z);
		coord_trans(lsPos,m_pBoltPad->ucs,FALSE);
		vertex2d.Set(lsPos.x,lsPos.y);
		vertex2d.feature=pBolt->get_d();
		boltVertexList.append(vertex2d);
	}
	for(int i=0;i<4;i++)
	{
		double fMinX=100,fMinY=100;
		for(f2dPoint* pPoint=boltVertexList.GetFirst();pPoint;pPoint=boltVertexList.GetNext())
		{
			vertex2d=*pPoint;
			coord_trans(vertex2d,cs2d[i],FALSE);
			if(vertex2d.x<=fMinX+0.5 && vertex2d.y<=fMinY+0.5)
			{
				fMinX=vertex2d.x;
				fMinY=vertex2d.y;
			}
		}
		vertex2d.Set(fMinX,fMinY);
		coord_trans(vertex2d,cs2d[i],TRUE);
		cs2d[i].origin=vertex2d;
	}
	//3、将垫板上螺栓转换到匹配模式坐标下,保存到对应的列表中
	for(int i=0;i<4;i++)
	{
		boltItemList[i].Empty();	//螺栓项列表做清空处理，否则会多出一项
		for(f2dPoint* pPoint=boltVertexList.GetFirst();pPoint;pPoint=boltVertexList.GetNext())
		{
			vertex2d=*pPoint;
			coord_trans(vertex2d,cs2d[i],FALSE);
			BOLT_ITEM* pBoltItem=boltItemList[i].append();
			pBoltItem->vertex=vertex2d;
			pBoltItem->d=vertex2d.feature;
		}
	}
	//4、将cs2d[0]作为标准模式坐标系，并将对应的螺栓项列表保存到boltItemArr中
	m_iSignIndex=0;
	modeUcs=cs2d[0];
	boltItemArr.SetSize(0,boltVertexList.GetNodeNum());
	for(BOLT_ITEM* pBoltItem=boltItemList[0].GetFirst();pBoltItem;pBoltItem=boltItemList[0].GetNext())
		boltItemArr.append(*pBoltItem);
}
//逐个比较螺栓信息：螺栓直径和匹配模式坐标系下的螺栓坐标，并确定最终的匹配坐标系
BOOL CBoltPadProperty::CompareBoltInfo(CBoltPadProperty* pDatumPadProp)
{
	if(pDatumPadProp==NULL)
		return FALSE;
	int nMatchingNum=0;
	BOLT_ITEM* pBoltItem=NULL,*pDatumBoltItem=NULL;
	if(m_iType==CUTANGELBOLTPAD)	//切角垫板的螺栓比对
	{	
		for(pDatumBoltItem=pDatumPadProp->boltItemArr.GetFirst();pDatumBoltItem;pDatumBoltItem=pDatumPadProp->boltItemArr.GetNext())
		{
			for(pBoltItem=boltItemArr.GetFirst();pBoltItem;pBoltItem=boltItemArr.GetNext())
			{
				if(pDatumBoltItem->d==pBoltItem->d &&fabs(pDatumBoltItem->vertex.x-pBoltItem->vertex.x)<EPS2&&
					fabs(pDatumBoltItem->vertex.y-pBoltItem->vertex.y)<EPS2)
					break;
			}			
			if(pBoltItem==NULL)
				return FALSE;
		}
	}
	else //if(pPadProperty->type==CBoltPadProperty::RECTANGLEBOLTPAD)	//矩形垫板的螺栓比对
	{
		for(int i=0;i<4;i++)
		{
			nMatchingNum=0;
			for(pDatumBoltItem=pDatumPadProp->boltItemArr.GetFirst();pDatumBoltItem;pDatumBoltItem=pDatumPadProp->boltItemArr.GetNext())
			{
				for(pBoltItem=boltItemList[i].GetFirst();pBoltItem;pBoltItem=boltItemList->GetNext())
				{
					if(pDatumBoltItem->d==pBoltItem->d&&fabs(pDatumBoltItem->vertex.x-pBoltItem->vertex.x)<EPS2&&
						fabs(pDatumBoltItem->vertex.y-pBoltItem->vertex.y)<EPS2)
					{
						nMatchingNum++;
						break;
					}
				}
			}
			if(nMatchingNum==pDatumPadProp->boltItemArr.GetSize())
			{
				modeUcs=cs2d[i];
				m_iSignIndex=i;
				return TRUE;
			}
		}
		return FALSE;
	}
	return TRUE;
}
static int compare_func3(const VERTEX_ITEM &vertexItem1,const VERTEX_ITEM &vertexItem2)
{	
	return int(vertexItem1.feature-vertexItem2.feature); 
}
//以第m_iSignVertex个顶点起，对垫板顶点信息进行标记
void CBoltPadProperty::MarkProfileVertexs()
{
	//将垫板外形顶点转换到模式坐标下并保存
	f2dPoint vertex2d;
	VERTEX_ITEM* pVertexItem;
	for(pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
	{
		vertex2d.Set(pVertexItem->pVertex->vertex.x,pVertexItem->pVertex->vertex.y);
		coord_trans(vertex2d,modeUcs,FALSE);
		pVertexItem->vertex2d=vertex2d;
	}
	//以第m_iSignVertex顶点开始对所有的顶点进行标记并排序
	int n=vertexItemArr.GetSize();
	for(pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
	{
		int i=vertexItemArr.GetCurrentIndex();
		if(i<m_iSignIndex)
			pVertexItem->feature=n-m_iSignIndex+i;
		else
			pVertexItem->feature=i-m_iSignIndex;
	}
	CQuickSort<VERTEX_ITEM>::QuickSort(vertexItemArr.m_pData,vertexItemArr.GetSize(),compare_func3);
}
//计算同一模式下垫板的最小外形
void CBoltPadProperty::UpdateMinProfileVertex(CBoltPadProperty* pDatumPadProp)
{
	if(pDatumPadProp==NULL)
		return;
	VERTEX_ITEM* pDatumVertexItem=NULL;
	for(VERTEX_ITEM* pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
	{
		pDatumVertexItem =pDatumPadProp->vertexItemArr.GetByIndex(pVertexItem->feature);
		if((pVertexItem->vertex2d.x<0 && pDatumVertexItem->vertex2d.x<pVertexItem->vertex2d.x) ||
			(pVertexItem->vertex2d.x>0 && pDatumVertexItem->vertex2d.x>pVertexItem->vertex2d.x))
			pDatumVertexItem->vertex2d.x=pVertexItem->vertex2d.x;
		if((pVertexItem->vertex2d.y<0 && pDatumVertexItem->vertex2d.y<pVertexItem->vertex2d.y) ||
			(pVertexItem->vertex2d.y>0 && pDatumVertexItem->vertex2d.y>pVertexItem->vertex2d.y))
			pDatumVertexItem->vertex2d.y=pVertexItem->vertex2d.y;
	}
}
//根据最小外形统一修正垫板外形顶点
BOOL CBoltPadProperty::EmendProfileVertexs(CBoltPadProperty* pDatumPadProp)
{
	BOOL bRet=FALSE;
	VERTEX_ITEM* pDatumVertexItem=NULL,*pVertexItem=NULL;
	if(m_pBoltPad->handle==pDatumPadProp->m_pBoltPad->handle)	//标准垫板本身
	{
		for(pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
		{
			f2dPoint vertex2d(pVertexItem->vertex2d.x,pVertexItem->vertex2d.y);
			coord_trans(vertex2d,modeUcs,TRUE);
			pVertexItem->pVertex->vertex.Set(vertex2d.x,vertex2d.y,0);
		}
		bRet=TRUE;
	}
	else	//同模型下的其他垫板
	{
		for(VERTEX_ITEM* pVertexItem=vertexItemArr.GetFirst();pVertexItem;pVertexItem=vertexItemArr.GetNext())
		{
			pDatumVertexItem=pDatumPadProp->vertexItemArr.GetByIndex(pVertexItem->feature);
			if(fabs(pVertexItem->vertex2d.x-pDatumVertexItem->vertex2d.x)>EPS2 || fabs(pVertexItem->vertex2d.y-pDatumVertexItem->vertex2d.y)>EPS)
			{
				pVertexItem->vertex2d.Set(pDatumVertexItem->vertex2d.x,pDatumVertexItem->vertex2d.y);
				coord_trans(pVertexItem->vertex2d,modeUcs,TRUE);
				pVertexItem->pVertex->vertex.Set(pVertexItem->vertex2d.x,pVertexItem->vertex2d.y,0);
				bRet=TRUE;
			}
		}
	}
	if(bRet)
	{
		m_pBoltPad->RetrieveGasketVertexCalType();
		if(m_pBoltPad->designInfo.iProfileStyle0123==1)
			m_pBoltPad->designInfo.iProfileStyle0123=3;
	}
	return bRet;
}
//判断两垫板是否属于同一模式
int SAMESCHEMA_BOLTPAD::IsSameSpec(CBoltPadProperty* pPadProperty)
{
	if(pPadProperty->m_iType!=pDatumProperty->m_iType)	//比较类型
		return 0;
	if(pPadProperty->m_pBoltPad->iSeg!=pDatumProperty->m_pBoltPad->iSeg)	//比较段号
		return 0;
	if(pPadProperty->m_pBoltPad->GetLsCount()!=pDatumProperty->m_pBoltPad->GetLsCount())	//比较螺栓数
		return 0;	
	if(pPadProperty->m_pBoltPad->GetThick()!=pDatumProperty->m_pBoltPad->GetThick())	//比较板厚
		return 0;
	if(!pPadProperty->CompareBoltInfo(pDatumProperty))	//比较螺栓信息
		return 0;
	return 1;
}
#ifdef __ALFA_TEST_VERSION_
void CBoltPadProperty::Test_InitProperty(int index,CLDSPlate* pBoltPad)
{
	InitProperty(pBoltPad);
	if(index==0)
	{
		if(modeUcs.origin.x!=0||modeUcs.origin.y!=0 ||
			modeUcs.axis_x.x!=-1||modeUcs.axis_x.y!=0||
			modeUcs.axis_y.x!=0||modeUcs.axis_y.y!=-1)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate single-hole rectangular plate: model CSYS is not correct!");
#else 
			logerr.Log("单孔矩形垫板初始化失败:模式坐标系不正确!");
#endif
		if(boltItemArr[0].vertex.x!=0 && boltItemArr[0].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate single-hole rectangular plate:The bolt's coordinate isn't correct in model CSYS!");
#else 
			logerr.Log("单孔矩形垫板初始化失败:模式坐标下螺栓坐标不正确!");
#endif
	}
	else if(index==1)
	{
		if(modeUcs.origin.x!=10||modeUcs.origin.y!=0 ||
			modeUcs.axis_x.x!=-1||modeUcs.axis_x.y!=0||
			modeUcs.axis_y.x!=0||modeUcs.axis_y.y!=-1)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate multihole rectangular plate: model CSYS is not correct!");
#else 
			logerr.Log("多孔矩形垫板初始化失败:模式坐标系不正确!");
#endif
		if(boltItemArr[0].vertex.x!=10 || boltItemArr[0].vertex.y!=0 ||
			boltItemArr[1].vertex.x!=0 || boltItemArr[1].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate multihole rectangular plate:the bolt's coordinate isn't correct in model CSYS!");
#else 
			logerr.Log("多孔矩形垫板初始化失败:模式坐标下螺栓坐标不正确!");
#endif
	}
	else 
	{
		if(modeUcs.origin.x!=-10||modeUcs.origin.y!=0 ||
			modeUcs.axis_x.x!=1||modeUcs.axis_x.y!=0||
			modeUcs.axis_y.x!=0||modeUcs.axis_y.y!=1)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate cut-leg gasket:model CSYS is not correct!");
#else 
			logerr.Log("切角垫板初始化失败:模式坐标系不正确!");
#endif
		if(boltItemArr[0].vertex.x!=0 || boltItemArr[0].vertex.y!=0||
			boltItemArr[1].vertex.x!=20||boltItemArr[1].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Failed to initiate cut-leg gasket:the bolt's coordinate isn't correct in model CSYS!");
#else 
			logerr.Log("切角垫板初始化失败:模式坐标下螺栓坐标不正确!");
#endif
	}
}
void CBoltPadProperty::Test_CompareBoltInfo(int index,CBoltPadProperty* pDatumPadProp)
{
	CompareBoltInfo(pDatumPadProp);
	if(index==0)
	{
		BOLT_ITEM datumBoltItem=pDatumPadProp->boltItemArr[0];
		if(boltItemArr[0].d!=datumBoltItem.d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing single-hole rectangular plate's bolts failure: their bolts' diameterg are different!");
#else 
			logerr.Log("单孔矩形垫板螺栓比较失败:对应的螺栓直径不同!");
#endif
		if(boltItemArr[0].vertex.x!=datumBoltItem.vertex.x ||
			boltItemArr[0].vertex.y!=datumBoltItem.vertex.y)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing single-hole rectangular plate's bolts failure:The bolt's coordiante is different!");
#else 
			logerr.Log("单孔矩形垫板螺栓比较失败:对应的螺栓坐标不同!");
#endif
		if(m_iSignIndex!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing single-hole rectangular plate's bolts failure:The standard model CSYS determine the error!");	
#else 
			logerr.Log("单孔矩形垫板螺栓比较失败:标准模式坐标系确定错误!");			
#endif		
	}
	else if(index==1)
	{
		if(boltItemArr[0].d!=pDatumPadProp->boltItemArr[0].d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:their bolts' diameterg are different!");
#else 
			logerr.Log("多孔矩形垫板螺栓比较失败:对应的螺栓直径不同!");
#endif
		if(boltItemArr[0].vertex.x!=pDatumPadProp->boltItemArr[0].vertex.x ||
			boltItemArr[0].vertex.y!=pDatumPadProp->boltItemArr[0].vertex.y||
			boltItemArr[0].vertex.x!=10 || boltItemArr[0].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:The bolts' coordinate are error!");
#else 
			logerr.Log("多孔矩形垫板螺栓比较失败:对应的螺栓坐标错误!");
#endif
		if(boltItemArr[1].d!=pDatumPadProp->boltItemArr[1].d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:their bolts' diameterg are different!");
#else 
			logerr.Log("多孔矩形垫板螺栓比较失败:对应的螺栓直径不同!");
#endif
		if(boltItemArr[1].vertex.x!=pDatumPadProp->boltItemArr[1].vertex.x ||
			boltItemArr[1].vertex.y!=pDatumPadProp->boltItemArr[1].vertex.y||
			boltItemArr[1].vertex.x!=0 || boltItemArr[1].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:The bolts' coordinate are error!");
#else 
			logerr.Log("多孔矩形垫板螺栓比较失败:对应的螺栓坐标错误!");
#endif
		if(m_iSignIndex!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing multihole rectangular gasket's bolts failure:The standard model CSYS determine the error!");
#else 
			logerr.Log("多孔矩形垫板螺栓比较失败:标准模式坐标系确定错误!");
#endif
	}
	else 
	{
		if(boltItemArr[0].d!=pDatumPadProp->boltItemArr[1].d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:their bolts' diameterg are different!");
#else 
			logerr.Log("切割垫板螺栓比较失败:对应的螺栓直径不同!");
#endif
		if(boltItemArr[0].vertex.x!=pDatumPadProp->boltItemArr[1].vertex.x ||
			boltItemArr[0].vertex.y!=pDatumPadProp->boltItemArr[1].vertex.y||
			boltItemArr[0].vertex.x!=0 || boltItemArr[0].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:The bolts' coordinate are error!");
#else 
			logerr.Log("切割垫板螺栓比较失败:对应的螺栓坐标错误!");
#endif
		if(boltItemArr[1].d!=pDatumPadProp->boltItemArr[0].d)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:their bolts' diameterg are different!");
#else 
			logerr.Log("切割垫板螺栓比较失败:对应的螺栓直径不同!");
#endif
		if(boltItemArr[1].vertex.x!=pDatumPadProp->boltItemArr[0].vertex.x ||
			boltItemArr[1].vertex.y!=pDatumPadProp->boltItemArr[0].vertex.y||
			boltItemArr[1].vertex.x!=20 || boltItemArr[1].vertex.y!=0)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:The bolts' coordinate are error!");
#else 
			logerr.Log("切割垫板螺栓比较失败:对应的螺栓坐标错误!");
#endif
		if(m_iSignIndex!=2)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Comparing cut gasket's bolts failure:The standard model CSYS determine the error!");
#else 
			logerr.Log("切割垫板螺栓比较失败:标准模式坐标系确定错误!");
#endif
	}
}
void CBoltPadProperty::Test_MarkProfileVertexs(int index)
{
	MarkProfileVertexs();
	if(index==0)
	{
		if(vertexItemArr[0].vertex2d.x!=-20|| vertexItemArr[0].vertex2d.y!=-10||
			vertexItemArr[1].vertex2d.x!=20 || vertexItemArr[1].vertex2d.y!=-10||
			vertexItemArr[2].vertex2d.x!=20 || vertexItemArr[2].vertex2d.y!=10||
			vertexItemArr[3].vertex2d.x!=-20|| vertexItemArr[3].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Single-hole rectangular gasket,transform form the vertices to the mode CSYS,coordinate error!");
#else 
			logerr.Log("单孔矩形垫板,将外形顶点转换到模式坐标系,坐标错误!");
#endif
	}
	else if(index==1)
	{
		if(vertexItemArr[0].vertex2d.x!=-10|| vertexItemArr[0].vertex2d.y!=-10||
			vertexItemArr[1].vertex2d.x!=30 || vertexItemArr[1].vertex2d.y!=-10||
			vertexItemArr[2].vertex2d.x!=30 || vertexItemArr[2].vertex2d.y!=10||
			vertexItemArr[3].vertex2d.x!=-10|| vertexItemArr[3].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Multihole rectangular gasket,transform form the vertices to the mode CSYS,coordinate error!");
#else 
			logerr.Log("多孔矩形垫板,将外形顶点转换到模式坐标系,坐标错误!");
#endif
	}
	else
	{
		if(vertexItemArr[0].vertex2d.x!=-10|| vertexItemArr[0].vertex2d.y!=-10||
			vertexItemArr[1].vertex2d.x!=30 || vertexItemArr[1].vertex2d.y!=-10||
			vertexItemArr[2].vertex2d.x!=30 || vertexItemArr[2].vertex2d.y!=5||
			vertexItemArr[3].vertex2d.x!=20|| vertexItemArr[3].vertex2d.y!=10||
			vertexItemArr[4].vertex2d.x!=-10|| vertexItemArr[4].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Cut gasket,transform form the vertices to the mode CSYS,coordinate error!");
#else 
			logerr.Log("切割垫板,将外形顶点转换到模式坐标系,坐标错误!");
#endif
	}
}
void CBoltPadProperty::Test_UpdateMinProfileVertex(int index,CBoltPadProperty* pDatumPadProp)
{
	UpdateMinProfileVertex(pDatumPadProp);
	if(index==0)
	{
		if(pDatumPadProp->vertexItemArr[0].vertex2d.x!=-15||pDatumPadProp->vertexItemArr[0].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[1].vertex2d.x!=20||pDatumPadProp->vertexItemArr[1].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[2].vertex2d.x!=20||pDatumPadProp->vertexItemArr[2].vertex2d.y!=10||
			pDatumPadProp->vertexItemArr[3].vertex2d.x!=-15||pDatumPadProp->vertexItemArr[3].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Single-hole rectangular gasket,calculate the minimum outline's vertex error!");
#else 
			logerr.Log("单孔矩形垫板,计算最小外形顶点错误!");
#endif
	}
	else if(index==1)
	{
		if(pDatumPadProp->vertexItemArr[0].vertex2d.x!=-10||pDatumPadProp->vertexItemArr[0].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[1].vertex2d.x!=25||pDatumPadProp->vertexItemArr[1].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[2].vertex2d.x!=25||pDatumPadProp->vertexItemArr[2].vertex2d.y!=10||
			pDatumPadProp->vertexItemArr[3].vertex2d.x!=-10||pDatumPadProp->vertexItemArr[3].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Multihole rectangular gasket,calculate the minimum outline's vertex error!");
#else 
			logerr.Log("多孔矩形垫板,计算最小外形顶点错误!");
#endif
	}
	else
	{
		if(pDatumPadProp->vertexItemArr[0].vertex2d.x!=-5||pDatumPadProp->vertexItemArr[0].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[1].vertex2d.x!=30||pDatumPadProp->vertexItemArr[1].vertex2d.y!=-10||
			pDatumPadProp->vertexItemArr[2].vertex2d.x!=30||pDatumPadProp->vertexItemArr[2].vertex2d.y!=5||
			pDatumPadProp->vertexItemArr[3].vertex2d.x!=20||pDatumPadProp->vertexItemArr[3].vertex2d.y!=10||
			pDatumPadProp->vertexItemArr[4].vertex2d.x!=-5||pDatumPadProp->vertexItemArr[4].vertex2d.y!=10)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Cut gasket,calculate the minimum outline's vertex error!");
#else 
			logerr.Log("切角垫板,计算最小外形顶点错误!");
#endif
	}
}
void CBoltPadProperty::Test_EmendProfileVertexs(int index,CBoltPadProperty* pDatumPadProp)
{
	EmendProfileVertexs(pDatumPadProp);
	if(index==0)
	{
		if(m_pBoltPad->handle==pDatumPadProp->m_pBoltPad->handle)
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(20,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-15,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-15,-10,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(20,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Single-hole rectangular gasket,correct vertexs of datum gasket's outline!");
#else 
				logerr.Log("单孔矩形垫板,修正基准垫板外形顶点错误!");
#endif
		}
		else
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(15,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-20,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-20,-10,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(15,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Single-hole rectangular gasket,correct errors of outline's vertexs!");
#else 
				logerr.Log("单孔矩形垫板,修正外形顶点错误!");
#endif
		}
	}
	else if(index==1)
	{
		if(m_pBoltPad->handle==pDatumPadProp->m_pBoltPad->handle)
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(15,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-20,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-20,-10,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(15,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Multihole rectangular gasket,correct vertexs of datum gasket's outline!");
#else 
				logerr.Log("多孔矩形垫板,修正基准垫板外形顶点错误!");
#endif
		}
		else
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(20,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-15,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-15,-10,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(20,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Multihole rectangular gasket,correct errors of outline's vertexs!");
#else 
				logerr.Log("多孔矩形垫板,修正外形顶点错误!");
#endif
		}
	}
	else
	{
		if(m_pBoltPad->handle==pDatumPadProp->m_pBoltPad->handle)
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(15,10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(-20,10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(-20,-5,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(-10,-10,0))||
				!vertexItemArr[4].pVertex->vertex.IsEqual(f3dPoint(15,-10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Cut gasket,correct vertexs of datum gasket's outline!");
#else 
				logerr.Log("切割垫板,修正基准垫板外形顶点错误!");
#endif
		}
		else
		{
			if(!vertexItemArr[0].pVertex->vertex.IsEqual(f3dPoint(-15,-10,0))||
				!vertexItemArr[1].pVertex->vertex.IsEqual(f3dPoint(20,-10,0))||
				!vertexItemArr[2].pVertex->vertex.IsEqual(f3dPoint(20,5,0))||
				!vertexItemArr[3].pVertex->vertex.IsEqual(f3dPoint(10,10,0))||
				!vertexItemArr[4].pVertex->vertex.IsEqual(f3dPoint(-15,10,0)))
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Cut gasket,correct errors of outline's vertexs!");
#else 
				logerr.Log("切割垫板,修正外形顶点错误!");
#endif
		}
	}
}
void Test_BoltPadProperty()
{
	CLDSPlate boltPad[3],datumboltPad[3];
	CBoltPadProperty boltPadProp,datumPadProp;
	f3dPoint vertex;
	vertex.feature=1;
	logerr.ClearContents();
	for(int i=0;i<3;i++)
	{
		boltPad[i].ucs.origin.Set(0,0,0);
		boltPad[i].ucs.axis_x.Set(1,0,0);
		boltPad[i].ucs.axis_y.Set(0,1,0);
		boltPad[i].ucs.axis_z.Set(0,0,1);
		boltPad[i].handle=1;
		datumboltPad[i].ucs.origin.Set(0,0,0);
		datumboltPad[i].ucs.axis_x.Set(1,0,0);
		datumboltPad[i].ucs.axis_y.Set(0,1,0);
		datumboltPad[i].ucs.axis_z.Set(0,0,1);
		datumboltPad[i].handle=2;
		if(i==0)
		{	//单孔矩形垫板
			vertex.Set(20,10,0);
			boltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,10,0);
			boltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,-10,0);
			boltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-10,0);
			boltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt bolt;
			bolt.set_d(20);
			bolt.ucs.origin.Set(0,0,0);
			bolt.ucs.axis_z.Set(0,0,1);
			boltPad[0].AppendLsRef(bolt.GetLsRef(),FALSE);
			//
			vertex.Set(20,15,0);
			datumboltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-15,15,0);
			datumboltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-15,-15,0);
			datumboltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-15,0);
			datumboltPad[0].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt datumBolt;
			datumBolt.set_d(20);
			datumBolt.ucs.origin.Set(5,0,0);
			datumBolt.ucs.axis_z.Set(0,0,1);
			datumboltPad[0].AppendLsRef(datumBolt.GetLsRef(),FALSE);
		}
		else if(i==1)
		{	//多孔矩形垫板
			vertex.Set(20,10,0);
			boltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,10,0);
			boltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,-10,0);
			boltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-10,0);
			boltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt bolt[2];
			bolt[0].set_d(20);
			bolt[0].ucs.origin.Set(0,0,0);
			bolt[0].ucs.axis_z.Set(0,0,1);
			boltPad[1].AppendLsRef(bolt[0].GetLsRef(),FALSE);
			bolt[1].set_d(20);
			bolt[1].ucs.origin.Set(10,0,0);
			bolt[1].ucs.axis_z.Set(0,0,1);
			boltPad[1].AppendLsRef(bolt[1].GetLsRef(),FALSE);
			//
			vertex.Set(20,15,0);
			datumboltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,15,0);
			datumboltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,-10,0);
			datumboltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-10,0);
			datumboltPad[1].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt datumBolt[2];
			datumBolt[0].set_d(20);
			datumBolt[0].ucs.origin.Set(-5,0,0);
			datumBolt[0].ucs.axis_z.Set(0,0,1);
			datumboltPad[1].AppendLsRef(datumBolt[0].GetLsRef(),FALSE);
			datumBolt[1].set_d(20);
			datumBolt[1].ucs.origin.Set(5,0,0);
			datumBolt[1].ucs.axis_z.Set(0,0,1);
			datumboltPad[1].AppendLsRef(datumBolt[1].GetLsRef(),FALSE);
		}
		else if(i==2)
		{	//多孔切角垫板
			vertex.Set(10,10,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,10,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-20,-10,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,-10,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(20,5,0);
			boltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt bolt[2];
			bolt[0].ucs.origin.Set(-10,0,0);
			bolt[0].ucs.axis_z.Set(0,0,1);
			boltPad[2].AppendLsRef(bolt[0].GetLsRef(),FALSE);
			bolt[1].ucs.origin.Set(10,0,0);
			bolt[1].ucs.axis_z.Set(0,0,1);
			boltPad[2].AppendLsRef(bolt[1].GetLsRef(),FALSE);
			//
			vertex.Set(15,15,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-25,15,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-25,-5,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(-15,-20,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			vertex.Set(15,-20,0);
			datumboltPad[2].vertex_list.append((PROFILE_VERTEX)vertex);
			CLDSBolt datumBolt[2];
			datumBolt[0].set_d(20);
			datumBolt[0].ucs.origin.Set(-10,0,0);
			datumBolt[0].ucs.axis_z.Set(0,0,1);
			datumboltPad[2].AppendLsRef(datumBolt[0].GetLsRef(),FALSE);
			datumBolt[1].set_d(20);
			datumBolt[1].ucs.origin.Set(10,0,0);
			datumBolt[1].ucs.axis_z.Set(0,0,1);
			datumboltPad[2].AppendLsRef(datumBolt[1].GetLsRef(),FALSE);
		}
		boltPadProp.Test_InitProperty(i,&boltPad[i]);
		datumPadProp.InitProperty(&datumboltPad[i]);
		boltPadProp.Test_CompareBoltInfo(i,&datumPadProp);
		boltPadProp.Test_MarkProfileVertexs(i);
		datumPadProp.MarkProfileVertexs();
		boltPadProp.Test_UpdateMinProfileVertex(i,&datumPadProp);
		boltPadProp.Test_EmendProfileVertexs(i,&datumPadProp);
		datumPadProp.Test_EmendProfileVertexs(i,&datumPadProp);
	}
	if(logerr.IsHasContents())
		logerr.ShowToScreen();
}
#endif

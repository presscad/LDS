#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "..\DesignStdComTemplDlg.h"
#include "..\StdComTmplLibraryDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#ifdef __COMMON_PART_INC_
void CLDSView::OnStdComTmplRepository()
{
	StdComTmpl();
}
#include "NewPartsListen.h"
bool UpdateBoltPosNormal(CNewPartsListen *pListenObj,CLDSView* pView)
{
	CLDSBoltPtr xarrBoltsPool[500]={ NULL };
	ARRAY_LIST<CLDSBoltPtr> xarrNewBolts(xarrBoltsPool,500,100);
	for (int i=0;i<pListenObj->xarrNewParts.Count;i++)
	{
		if (pListenObj->xarrNewParts[i]->IsBolt())
			xarrNewBolts.append((CLDSBolt*)pListenObj->xarrNewParts[i]);
	}
	CLDSBolt::CorrectBoltZnL0Para(xarrNewBolts.m_pData,xarrNewBolts.Count);
	CLDSModule* pModule=Ta.GetActiveModule();
	for (i=0;i<xarrNewBolts.Count;i++)
	{
		xarrNewBolts[i]->CalGuigeAuto();
		if(pView->IsNeedReverseBoltDirection(xarrNewBolts[i]))
			pView->ReverseBoltDirection(xarrNewBolts[i],pModule);
	}
	return true;
}
int CLDSView::StdComTmpl()
{
	//if(!PRODUCT_FUNC::IsHasInternalTest())
	//	return -1;
	CStdComTmplLibraryDlg dlg;
	if(dlg.DoModal()!=IDOK)
		return 0;
	//挂点设计过程非常复杂，内部难免会调用其余自带Undo的子程序，此处必须设bGroupUndo=true wjh-2019.12.2
	CUndoOperObject undo(&Ta,true);
	if( dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM1||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM2||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_EB_DANGLE_EARTH_WIRE_ARM3||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM1||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_EB_DANGLE_WIRE_I_ARM2||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_NZ_FRT_2_GD||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_NZ_BTM_2_GD||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_NZ_BTM_3_GD||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_EB||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_2_EB||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_META_SECT_FRT_1_GD||
		dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_META_SECT_BTM_1_GD)
	{
		CStackVariant<char> stackprop0(&CLDSBolt::BOLTL0_CALMODE, CLDSBolt::L0CAL_BY_SUMM_THICK);
		//智能设计默认螺栓均为计入缺失的垫板间隙 wjh-2019.9.3
		CStackVariant<char> stackprop0i(&CLDSBolt::BOLTL0_PREFER_MODE,CLDSBolt::L0CAL_INC_GAP_THICK);
		CLDSPartPtr xarrNewPartsPool[500];
		CNewPartsListen listen(&Ta.Parts,xarrNewPartsPool,500);

		CDesignWireStruTemplDlgEx wirearmdlg;
		wirearmdlg.m_idStdComTmpl=dlg.StdComTmplId;
		if (wirearmdlg.DoModal() != IDOK)
			return 0;
		listen.EndListen(CLDSBolt::BOLTL0_PREFER_MODE==CLDSBolt::L0CAL_INC_GAP_THICK);
		stackprop0.PopStack();	//恢复根据螺栓属性计算，以便集中修正螺栓通厚 wjh-2019.9.3
		UpdateBoltPosNormal(&listen,this);
		for (int i=0;i<listen.xarrNewParts.Count;i++)
		{
			if (listen.xarrNewParts[i]->m_bVirtualPart)
				continue;
			listen.xarrNewParts[i]->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(listen.xarrNewParts[i]->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
		return 1;
	}
#ifdef __COMPLEX_PART_INC_
	else if(dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_TUBE_WALKWAY)//0x010100)	//管塔走道架
		return DesignTubeWalkWay();
	else if(dlg.StdComTmplId==IStdComTempl::STDTMPL_STRU_TUBE_WORKBENCH)//0x010100)	//管塔站位平台
		return DesignTubeSafeHolder();
#endif
	else
	{
		CStdComTempl* pStdTmpl=globalStdComTmplLibrary.LoadStdComTempl(dlg.StdComTmplId);
		if (pStdTmpl == NULL)
			return 0;
		pStdTmpl->SetMappingNode(1,0x2e);
		pStdTmpl->hashMatchRodHid.SetValue(1,0x1ae);
		pStdTmpl->hashMatchRodHid.SetValue(2,0x176);
		pStdTmpl->hashMatchRodHid.SetValue(3,0x192);
		AfxMessageBox("暂不支持该种类型的模板");
		return 0;
		CStdTemplParaPlate* pParaPlate=pStdTmpl->listParaPlates.GetValue(1);
		if(pParaPlate!=NULL)
		{
			pParaPlate->design.idBaseNode=1;
			pParaPlate->design.idBaseRod=1;
			pParaPlate->connrods[0].sidRod=1;
			CStdComModel modeler(&Ta);
			modeler.BuildModel(pStdTmpl);
		}
	}
	return 1;
}
#endif

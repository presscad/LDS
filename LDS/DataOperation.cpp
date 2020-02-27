//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "MainFrm.h"
#include "env_def.h"
#include "CmdLineDlg.h"
#include "LocalFeatureDef.h"

//直接执行指定命令
int CLDSView::DataOperation()
{
	//CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//数据 -
	//		检查C-
	//			|-漏孔
	//			|-垫圈垫板
	//			|-异常心距
	//			|-异常边距
	//			|-异常端距
	//			|-肋板倒弧
	//			|-构件方位稳定
	//			|-孔间隙
	//			|-构件对称性
	//			|-重叠冗余
	//		清除P-
	//			|-数据对象，钢板，螺栓
	//		筛选F-
	//			|-规格
	//			|-材质
	KEY4C xarrCmdLevel[4];
	int cnLevelCount=2;
	xarrCmdLevel[0]=KEY4C('P');
	xarrCmdLevel[1]=GetSingleWord(SELECTINDEX_BOLT)|GetSingleWord(SELECTINDEX_PLATE)|GetSingleWord(SELECTINDEX_PARAMPLATE);
	SmartPartPtr pSmartPart;
	if (console.m_pWorkPart!=NULL&&console.m_pWorkPart->IsPlate())
		console.m_pWorkPart=NULL;
	for (pSmartPart=console.DispPartSet.GetFirst();pSmartPart.blValid;pSmartPart=console.DispPartSet.GetNext())
	{
		if (pSmartPart->GetClassTypeId()==CLS_BOLT)
			console.DispPartSet.DeleteCursor();
		if (pSmartPart->IsPlate())
			console.DispPartSet.DeleteCursor();
	}
	CStackVariant<IConsole*> stackver(&CLDSObject::_console,NULL);
	for (pSmartPart=Ta.EnumPartFirst();pSmartPart.blValid;pSmartPart=Ta.EnumPartNext())
	{
		if (pSmartPart->GetLsRefList())
			pSmartPart->GetLsRefList()->Empty();
		if (pSmartPart->GetClassTypeId()==CLS_BOLT)
			Ta.Parts.DeleteCursor();
		if (pSmartPart->IsPlate())
			Ta.Parts.DeleteCursor();
	}
	console.DispPartSet.Clean();
	Ta.Parts.Clean();
	return this->RedrawAll();
}

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

//ֱ��ִ��ָ������
int CLDSView::DataOperation()
{
	//CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//���� -
	//		���C-
	//			|-©��
	//			|-��Ȧ���
	//			|-�쳣�ľ�
	//			|-�쳣�߾�
	//			|-�쳣�˾�
	//			|-�߰嵹��
	//			|-������λ�ȶ�
	//			|-�׼�϶
	//			|-�����Գ���
	//			|-�ص�����
	//		���P-
	//			|-���ݶ��󣬸ְ壬��˨
	//		ɸѡF-
	//			|-���
	//			|-����
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

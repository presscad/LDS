#include "stdafx.h"
#include "LDS.h"
#include "XhCharString.h"
#include "Tower.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "CmdLineDlg.h"
#include "SnapTypeVerify.h"
#include "GlobalFunc.h"
#include "DesFittingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//设计连接金具
int CLDSView::DesignFitting()
{
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	CDesFittingDlg dlg;
	//切换到实体模式
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	g_pSolidDraw->Draw();
	CString cmdStr,cmdValue;
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//选择金具类型(EB|GD)
#ifdef AFX_TARG_ENU_ENGLISH			
	pCmdLine->FillCmdLine("Fitting Please select fitting's type[EB-Plate(E)/GD-Plate(G)/U-Ring(U)] <E>:","");
#else
	pCmdLine->FillCmdLine("Fitting 选择连接金具类型[EB挂板(E)/GD挂板(G)/U型螺丝(S)/U型挂板(B)] <E>:","");
#endif
	if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"E|G|S|B"))
		return FALSE;
	BYTE ciFittingType=CLDSFitting::TYPE_EB;
	if(cmdStr.GetLength()>0)
	{	
		if (cmdStr[0] == 'G' || cmdStr[0] == 'g')
			ciFittingType = CLDSFitting::TYPE_GD;
		else if (cmdStr[0] == 'U' || cmdStr[0] == 'u')
			ciFittingType = CLDSFitting::TYPE_UR;
		else if (cmdStr[0] == 'S' || cmdStr[0] == 's')
			ciFittingType = CLDSFitting::TYPE_US;
		else if (cmdStr[0] == 'B' || cmdStr[0] == 'b')
			ciFittingType = CLDSFitting::TYPE_UB;
	}
	//选择连接螺栓孔，计算坐标位置
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_HOLEWALL)|GetSingleWord(SELECTINDEX_BOLT));
	DWORD dwhObj=0,dwExportFlag=0;
	if(ciFittingType!=CLDSFitting::TYPE_UR)
	{
		CLDSBolt* pHoleL=NULL,*pHoleR=NULL;
		long hResideJgL=0,hResideJgR=0;
		//选择左侧挂孔
		UINT idRawSolid=0;
		while(pHoleL==NULL)
		{
			pCmdLine->FillCmdLine("Fitting 请选择挂板的左侧挂孔:","");
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			idRawSolid=obj.hObj;
			if(obj.hObj<0&&obj.iObjType==CLS_HOLEWALL)
			{
				obj.TranslateHoleWall(g_pSolidDraw);
				pHoleL=(CLDSBolt*)Ta.Parts.FromHandle(obj.hObj,CLS_BOLT);
				hResideJgL=obj.hResidePart;
			}
			else
			{
				pHoleL=(CLDSBolt*)Ta.Parts.FromHandle(obj.hRelaObj,CLS_BOLT);
				hResideJgL=(pHoleL)?pHoleL->des_base_pos.hPart:0;
			}
		}
		pCmdLine->FinishCmdLine(CXhChar50("0X%X",pHoleL->handle));
		g_pSolidDraw->SetEntSnapStatus(idRawSolid);
		//选择右侧挂孔
		idRawSolid=0;
		while(pHoleR==NULL)
		{
			pCmdLine->FillCmdLine("Fitting 请选择挂板的右侧挂孔:","");
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			idRawSolid=obj.hObj;
			if(obj.hObj<0&&obj.iObjType==CLS_HOLEWALL)
			{
				obj.TranslateHoleWall(g_pSolidDraw);
				pHoleR=(CLDSBolt*)Ta.Parts.FromHandle(obj.hObj,CLS_BOLT);
				hResideJgR=obj.hResidePart;
			}
			else
			{
				pHoleR=(CLDSBolt*)Ta.Parts.FromHandle(obj.hRelaObj,CLS_BOLT);
				hResideJgR=(pHoleR)?pHoleR->des_base_pos.hPart:0;
			}
		}
		pCmdLine->FinishCmdLine(CXhChar50("0X%X",pHoleR->handle));
		g_pSolidDraw->SetEntSnapStatus(idRawSolid);
		//根据挂孔获取所在角钢
		CLDSLineAngle* pResideJgL=(CLDSLineAngle*)Ta.Parts.FromHandle(hResideJgL,CLS_LINEANGLE);
		CLDSLineAngle* pResideJgR=(CLDSLineAngle*)Ta.Parts.FromHandle(hResideJgR,CLS_LINEANGLE);
		if(pResideJgL==NULL||pResideJgR==NULL)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		//
		dlg.m_ciType=ciFittingType;
		dlg.m_pHole1=pHoleL;
		dlg.m_pHole2=pHoleR;
		dlg.m_pResidePart1=pResideJgL;
		dlg.m_pResidePart2=pResideJgR;
	}
	else
	{
		CLDSBolt* pHole=NULL;
		long hResidePart=0;
		//选择螺栓孔
		while(pHole==NULL)
		{
			pCmdLine->FillCmdLine("Fitting 请选择U型挂环所在的挂孔:","");
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<=0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if(obj.hObj<0&&obj.iObjType==CLS_HOLEWALL)
			{
				obj.TranslateHoleWall(g_pSolidDraw);
				pHole=(CLDSBolt*)Ta.Parts.FromHandle(obj.hObj,CLS_BOLT);
				hResidePart=obj.hResidePart;
			}
			else
			{
				pHole=(CLDSBolt*)Ta.Parts.FromHandle(obj.hRelaObj,CLS_BOLT);
				hResidePart=(pHole)?pHole->des_base_pos.hPart:0;
			}
		}
		//根据挂孔获取所在钢板
		CLDSGeneralPlate* pResidePart=(CLDSGeneralPlate*)Ta.Parts.FromHandle(hResidePart,CLS_PLATE,CLS_PARAMPLATE);
		if(pResidePart==NULL)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		//
		dlg.m_ciType=ciFittingType;
		dlg.m_pHole1=pHole;
		dlg.m_pResidePart1=pResidePart;
	}
	//弹出对话框设计金具
	if(dlg.DoModal()!=IDOK)
	{
		g_pSolidDraw->ReleaseSnapStatus();
		pCmdLine->CancelCmdLine();
		return FALSE;
	}
	CUndoOperObject undo(&Ta,true);
	dlg.CreateFitting();
	pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("command:","");
#else
	pCmdLine->FillCmdLine("命令:","");
#endif
	return TRUE;
}
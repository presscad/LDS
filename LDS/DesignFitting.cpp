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

//������ӽ��
int CLDSView::DesignFitting()
{
	CCmdLockObject cmdlock(this);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	CDesFittingDlg dlg;
	//�л���ʵ��ģʽ
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidSet->SetDisplayType(DISP_SOLID);
	g_pSolidDraw->Draw();
	CString cmdStr,cmdValue;
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	//ѡ��������(EB|GD)
#ifdef AFX_TARG_ENU_ENGLISH			
	pCmdLine->FillCmdLine("Fitting Please select fitting's type[EB-Plate(E)/GD-Plate(G)/U-Ring(U)] <E>:","");
#else
	pCmdLine->FillCmdLine("Fitting ѡ�����ӽ������[EB�Ұ�(E)/GD�Ұ�(G)/U����˿(S)/U�͹Ұ�(B)] <E>:","");
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
	//ѡ��������˨�ף���������λ��
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_HOLEWALL)|GetSingleWord(SELECTINDEX_BOLT));
	DWORD dwhObj=0,dwExportFlag=0;
	if(ciFittingType!=CLDSFitting::TYPE_UR)
	{
		CLDSBolt* pHoleL=NULL,*pHoleR=NULL;
		long hResideJgL=0,hResideJgR=0;
		//ѡ�����ҿ�
		UINT idRawSolid=0;
		while(pHoleL==NULL)
		{
			pCmdLine->FillCmdLine("Fitting ��ѡ��Ұ�����ҿ�:","");
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
		//ѡ���Ҳ�ҿ�
		idRawSolid=0;
		while(pHoleR==NULL)
		{
			pCmdLine->FillCmdLine("Fitting ��ѡ��Ұ���Ҳ�ҿ�:","");
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
		//���ݹҿ׻�ȡ���ڽǸ�
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
		//ѡ����˨��
		while(pHole==NULL)
		{
			pCmdLine->FillCmdLine("Fitting ��ѡ��U�͹һ����ڵĹҿ�:","");
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
		//���ݹҿ׻�ȡ���ڸְ�
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
	//�����Ի�����ƽ��
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
	pCmdLine->FillCmdLine("����:","");
#endif
	return TRUE;
}
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "DesignStdComTemplDlg.h"
#include "CmdLineDlg.h"
#include "SnapTypeVerify.h"

#ifdef __COMPLEX_PART_INC_
//设计钢管的走道
static CDesignTubeWalkWayDlgEx tubeWalkWayDlg;
static BOOL _DesignTubeWalkWay(CLDSView* pView,bool bContinousCall=true)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CCmdLockObject cmdLock(pView);
	if(!cmdLock.LockSuccessed())
		return FALSE;
	CXhChar16 shCurrObject;
	//long hTube=(tubeWalkWayDlg.m_pBaseTube)?tubeWalkWayDlg.m_pBaseTube->handle:0;
	//tubeWalkWayDlg.m_pBaseTube=(CLDSLineTube*)console.FromPartHandle(hTube,CLS_LINETUBE);
	if(tubeWalkWayDlg.VerifyRelaRodAndNodes())
	{
		if(!bContinousCall)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			DWORD dwhObj=0,dwExportFlag=0;
			CString cmdStr;
			//选择基准钢管
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please choose the line-tube：");
#else
			cmdStr.Format("请选择设计走道的基准钢管:");
#endif	
			CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_LINE);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE));
			if(tubeWalkWayDlg.m_hBaseTube>0x20)
			{
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pBaseTube->handle);
				shCurrObject.Printf("0x%X",tubeWalkWayDlg.m_hBaseTube);
			}
			else
				shCurrObject="";
			long hOldBaseTube=tubeWalkWayDlg.m_hBaseTube;
			while(true)
			{
				pCmdLine->FillCmdLine(cmdStr,(char*)shCurrObject);
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				if(obj.hRelaObj==0&&(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN||obj.ciTriggerType==SELOBJ::TRIGGER_KEYESCAPE))
					tubeWalkWayDlg.m_pBaseTube=(CLDSLineTube*)console.FromPartHandle(tubeWalkWayDlg.m_hBaseTube,CLS_LINETUBE);
				else
					tubeWalkWayDlg.m_pBaseTube=(CLDSLineTube*)console.FromPartHandle(obj.hRelaObj,CLS_LINETUBE);
				if(tubeWalkWayDlg.m_pBaseTube)
				{
					tubeWalkWayDlg.m_hBaseTube=obj.hRelaObj;
					g_pSolidDraw->ReleaseSnapStatus();
					g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pBaseTube->handle);
					pCmdLine->FinishCmdLine(CXhChar16("钢管0x%X",tubeWalkWayDlg.m_pBaseTube->handle));
					break;
				}
			}
			if(tubeWalkWayDlg.m_pBaseTube&&hOldBaseTube!=tubeWalkWayDlg.m_pBaseTube->handle)
			{	//基准钢管变化时，清空爬梯角钢
				tubeWalkWayDlg.m_xTemplUI.ResetParams();
				tubeWalkWayDlg.m_hBaseNode=0;
				tubeWalkWayDlg.m_hInnerAngle=0;
				tubeWalkWayDlg.m_hOuterAngle=0;
				tubeWalkWayDlg.m_pInnerJg=NULL;
				tubeWalkWayDlg.m_pOuterJg=NULL;
			}
			//选择设计节点
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please base nodes first:");
#else
			cmdStr.Format("请选择基准节点:");
#endif
			verifier.ClearSnapFlag();
			verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
			{
				CDisplayNodeAtFrontLife displayNode;
				displayNode.DisplayNodeAtFront();
				if(tubeWalkWayDlg.m_hBaseNode>0x20)
				{
					g_pSolidDraw->ReleaseSnapStatus();
					g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_hBaseNode);
					shCurrObject.Printf("0x%X",tubeWalkWayDlg.m_hBaseNode);
				}
				else
					shCurrObject="";
				while(true)
				{
					pCmdLine->FillCmdLine(cmdStr,(char*)shCurrObject);
					if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
					{
						pCmdLine->CancelCmdLine();
						return FALSE;
					}
					SELOBJ obj(dwhObj,dwExportFlag);
					if(obj.hRelaObj==0&&(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN||obj.ciTriggerType==SELOBJ::TRIGGER_KEYESCAPE))
						tubeWalkWayDlg.m_pBaseNode=console.FromNodeHandle(tubeWalkWayDlg.m_hBaseNode);
					else
						tubeWalkWayDlg.m_pBaseNode=console.FromNodeHandle(obj.hRelaObj);
					if(tubeWalkWayDlg.m_pBaseNode&&tubeWalkWayDlg.m_pBaseNode->hFatherPart==tubeWalkWayDlg.m_pBaseTube->handle)
						break;
					else if(tubeWalkWayDlg.m_pBaseNode)
						pCmdLine->FinishCmdLine(CXhChar50("节点0x%X不在基准钢管上",tubeWalkWayDlg.m_pBaseNode->handle));
				}
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pBaseNode->handle);
				pCmdLine->FinishCmdLine(CXhChar16("节点0x%X",tubeWalkWayDlg.m_pBaseNode->handle));
				tubeWalkWayDlg.m_hInnerBeamDatumNode=0;
				tubeWalkWayDlg.m_hOuterBeamDatumNode=0;
			}
			tubeWalkWayDlg.m_xTemplUI.WorkPlaneSketch();
			//选择走道角钢外侧角钢
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please WalkWay Outer LineAngle():");
#else
			cmdStr.Format("请选择走道外侧角钢:");
#endif
			verifier.ClearSnapFlag();
			verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
			if(tubeWalkWayDlg.m_hOuterAngle>0)
			{
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_hOuterAngle);
				shCurrObject.Printf("0x%X",tubeWalkWayDlg.m_hOuterAngle);
				while (true)
				{
					pCmdLine->FillCmdLine(cmdStr,(char*)shCurrObject);
					if (g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
					{
						tubeWalkWayDlg.m_pOuterJg=NULL;
						pCmdLine->CancelCmdLine();
						return FALSE;
					}
					SELOBJ obj(dwhObj,dwExportFlag);
					if (obj.hRelaObj==0&&(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN||obj.ciTriggerType==SELOBJ::TRIGGER_KEYESCAPE))
						tubeWalkWayDlg.m_pOuterJg=(CLDSLineAngle*)console.FromPartHandle(tubeWalkWayDlg.m_hOuterAngle,CLS_LINEANGLE);
					else
						tubeWalkWayDlg.m_pOuterJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
					if (tubeWalkWayDlg.m_pOuterJg)
					{
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pOuterJg->handle);
						pCmdLine->FinishCmdLine(CXhChar16("角钢0x%X",tubeWalkWayDlg.m_pOuterJg->handle));
					}
					else
						pCmdLine->FinishCmdLine();
					break;
				}
			}
			if(tubeWalkWayDlg.m_pOuterJg)
			{	//选择走道角钢内侧角钢
#ifdef AFX_TARG_ENU_ENGLISH
				cmdStr.Format("please WalkWay Inner LineAngle:");
#else
				cmdStr.Format("请选择走道内侧角钢:");
#endif
				verifier.ClearSnapFlag();
				verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
				verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
				if(tubeWalkWayDlg.m_hInnerAngle>0)
				{
					g_pSolidDraw->ReleaseSnapStatus();
					g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_hInnerAngle);
					shCurrObject.Printf("0x%X",tubeWalkWayDlg.m_hInnerAngle);
				}
				else
					shCurrObject="";
				while(true)
				{
					pCmdLine->FillCmdLine(cmdStr,(char*)shCurrObject);
					if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
					{
						pCmdLine->CancelCmdLine();
						return FALSE;
					}
					SELOBJ obj(dwhObj,dwExportFlag);
					if(obj.hRelaObj==0&&(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN||obj.ciTriggerType==SELOBJ::TRIGGER_KEYESCAPE))
						tubeWalkWayDlg.m_pInnerJg=(CLDSLineAngle*)console.FromPartHandle(tubeWalkWayDlg.m_hInnerAngle,CLS_LINEANGLE);
					else
						tubeWalkWayDlg.m_pInnerJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
					if(tubeWalkWayDlg.m_pInnerJg)
					{
						g_pSolidDraw->ReleaseSnapStatus();
						g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pInnerJg->handle);
						pCmdLine->FinishCmdLine(CXhChar16("角钢0x%X",tubeWalkWayDlg.m_pInnerJg->handle));
						break;
					}
				}
			}
		}
		if(tubeWalkWayDlg.m_pOuterJg!=NULL)
		{
			//选择基准钢管
			static int STEP_LENGTH=1000;
			CString cmdStr;
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("Please input step offset length of the next holder<%d>:",STEP_LENGTH);
#else
			cmdStr.Format("请输入下一走道支架沿基准钢管的偏移步长<%d>:",STEP_LENGTH);
#endif	
			pCmdLine->FillCmdLine(cmdStr,"");
			BOOL status=pCmdLine->GetStrFromCmdLine(cmdStr);
			pCmdLine->m_bGettingStrFromCmdLine=FALSE;
			if(!status)
				return FALSE;
			else if(cmdStr.GetLength()>0)
				STEP_LENGTH=atoi(cmdStr);
			tubeWalkWayDlg.m_xTemplUI.params.m_fZjOffDist=STEP_LENGTH;
		}
		//else
		//	tubeWalkWayDlg.m_xTemplUI.params.m_fZjOffDist=1000;
	}
	else
	{
		g_pSolidDraw->ReleaseSnapStatus();
		DWORD dwhObj=0,dwExportFlag=0;
		CString cmdStr;
		//选择基准钢管
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("please choose the line-tube：");
#else
		cmdStr.Format("请选择设计走道的基准钢管:");
#endif	
		CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_LINE);
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE));
		if(tubeWalkWayDlg.m_hBaseTube>0x20)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pBaseTube->handle);
			shCurrObject.Printf("0x%X",tubeWalkWayDlg.m_hBaseTube);
		}
		else
			shCurrObject="";
		long hOldBaseTube=tubeWalkWayDlg.m_pBaseTube!=NULL?tubeWalkWayDlg.m_pBaseTube->handle:0;
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,(char*)shCurrObject);
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			if(obj.hRelaObj==0&&(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN||obj.ciTriggerType==SELOBJ::TRIGGER_KEYESCAPE))
				tubeWalkWayDlg.m_pBaseTube=(CLDSLineTube*)console.FromPartHandle(tubeWalkWayDlg.m_hBaseTube,CLS_LINETUBE);
			else
				tubeWalkWayDlg.m_pBaseTube=(CLDSLineTube*)console.FromPartHandle(obj.hRelaObj,CLS_LINETUBE);
			if(tubeWalkWayDlg.m_pBaseTube)
			{
				tubeWalkWayDlg.m_hBaseTube=tubeWalkWayDlg.m_pBaseTube->handle;
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pBaseTube->handle);
				pCmdLine->FinishCmdLine(CXhChar16("钢管0x%X",tubeWalkWayDlg.m_pBaseTube->handle));
				break;
			}
		}
		if(tubeWalkWayDlg.m_pBaseTube&&hOldBaseTube!=tubeWalkWayDlg.m_pBaseTube->handle)
		{	//基准钢管变化时，清空爬梯角钢
			tubeWalkWayDlg.m_xTemplUI.ResetParams();
			tubeWalkWayDlg.m_hBaseNode=0;
			tubeWalkWayDlg.m_hInnerAngle=0;
			tubeWalkWayDlg.m_hOuterAngle=0;
			tubeWalkWayDlg.m_pInnerJg=NULL;
			tubeWalkWayDlg.m_pOuterJg=NULL;
		}
		//选择设计节点
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("please base nodes first:");
#else
		cmdStr.Format("请选择基准节点:");
#endif
		verifier.ClearSnapFlag();
		verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
		{
			CDisplayNodeAtFrontLife displayNode;
			displayNode.DisplayNodeAtFront();
			if(tubeWalkWayDlg.m_hBaseNode>0x20)
			{
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_hBaseNode);
				shCurrObject.Printf("0x%X",tubeWalkWayDlg.m_hBaseNode);
			}
			else
				shCurrObject="";
			while(true)
			{
				pCmdLine->FillCmdLine(cmdStr,(char*)shCurrObject);
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				if(obj.hRelaObj==0&&(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN||obj.ciTriggerType==SELOBJ::TRIGGER_KEYESCAPE))
					tubeWalkWayDlg.m_pBaseNode=console.FromNodeHandle(tubeWalkWayDlg.m_hBaseNode);
				else
					tubeWalkWayDlg.m_pBaseNode=console.FromNodeHandle(obj.hRelaObj);
				if(tubeWalkWayDlg.m_pBaseNode&&tubeWalkWayDlg.m_pBaseNode->hFatherPart==tubeWalkWayDlg.m_pBaseTube->handle)
					break;
			}
			tubeWalkWayDlg.m_hBaseNode=tubeWalkWayDlg.m_pBaseNode->handle;
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pBaseNode->handle);
			pCmdLine->FinishCmdLine(CXhChar16("节点0X%X",tubeWalkWayDlg.m_pBaseNode->handle));
			tubeWalkWayDlg.m_hInnerBeamDatumNode=0;
			tubeWalkWayDlg.m_hOuterBeamDatumNode=0;
		}
		//选择走道角钢外侧角钢
#ifdef AFX_TARG_ENU_ENGLISH
		cmdStr.Format("please WalkWay Outer LineAngle():");
#else
		cmdStr.Format("请选择走道外侧角钢(若无直接按回车键或空格键):");
#endif
		verifier.ClearSnapFlag();
		verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
		verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
		if(tubeWalkWayDlg.m_hOuterAngle>0)
		{
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_hOuterAngle);
			shCurrObject.Printf("0x%X",tubeWalkWayDlg.m_hOuterAngle);
			while(true)
			{
				pCmdLine->FillCmdLine(cmdStr,(char*)shCurrObject);
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					tubeWalkWayDlg.m_pOuterJg=NULL;
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				if(obj.hRelaObj==0&&(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN||obj.ciTriggerType==SELOBJ::TRIGGER_KEYESCAPE))
				{
					tubeWalkWayDlg.m_pOuterJg=(CLDSLineAngle*)console.FromPartHandle(tubeWalkWayDlg.m_hOuterAngle,CLS_LINEANGLE);
					pCmdLine->FinishCmdLine();
					break;
				}
				else
					tubeWalkWayDlg.m_pOuterJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
				if(tubeWalkWayDlg.m_pOuterJg)
				{
					g_pSolidDraw->ReleaseSnapStatus();
					g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pOuterJg->handle);
					pCmdLine->FinishCmdLine(CXhChar16("角钢0X%X",tubeWalkWayDlg.m_pOuterJg->handle));
					break;
				}
				pCmdLine->FillCmdLine(cmdStr,"");
			}
		}
		if(tubeWalkWayDlg.m_pOuterJg)
		{	//选择走道角钢内侧角钢
#ifdef AFX_TARG_ENU_ENGLISH
			cmdStr.Format("please WalkWay Inner LineAngle:");
#else
			cmdStr.Format("请选择走道内侧角钢:");
#endif
			verifier.ClearSnapFlag();
			verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
			verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINEANGLE));
			if(tubeWalkWayDlg.m_hInnerAngle>0)
			{
				g_pSolidDraw->ReleaseSnapStatus();
				g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_hInnerAngle);
				shCurrObject.Printf("0x%X",tubeWalkWayDlg.m_hInnerAngle);
			}
			else
				shCurrObject="";
			while(true)
			{
				pCmdLine->FillCmdLine(cmdStr,(char*)shCurrObject);
				if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
				{
					pCmdLine->CancelCmdLine();
					return FALSE;
				}
				SELOBJ obj(dwhObj,dwExportFlag);
				if(obj.hRelaObj==0&&(obj.ciTriggerType==SELOBJ::TRIGGER_KEYRETURN||obj.ciTriggerType==SELOBJ::TRIGGER_KEYESCAPE))
				{
					tubeWalkWayDlg.m_pInnerJg=(CLDSLineAngle*)console.FromPartHandle(tubeWalkWayDlg.m_hInnerAngle,CLS_LINEANGLE);
					pCmdLine->FinishCmdLine();
					break;
				}
				else
					tubeWalkWayDlg.m_pInnerJg=(CLDSLineAngle*)console.FromPartHandle(obj.hRelaObj,CLS_LINEANGLE);
				if(tubeWalkWayDlg.m_pInnerJg)
				{
					g_pSolidDraw->ReleaseSnapStatus();
					g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pInnerJg->handle);
					pCmdLine->FinishCmdLine(CXhChar16("角钢0X%X",tubeWalkWayDlg.m_pInnerJg->handle));
					break;
				}
			}
			g_pSolidDraw->SetEntSnapStatus(tubeWalkWayDlg.m_pInnerJg->handle);
			pCmdLine->FinishCmdLine(CXhChar16("内侧角钢0X%X",tubeWalkWayDlg.m_pInnerJg->handle));
		}
		else
		{
			tubeWalkWayDlg.m_xTemplUI.params.m_fZdOffDist=700;
			tubeWalkWayDlg.m_xTemplUI.params.m_fZjOffDist=400;
		}
	}
	if(tubeWalkWayDlg.DoModal()!=IDOK)
		return 0;
	CUndoOperObject undo(&Ta,true);
	CTubeWalkWayStdComModel model(&Ta);
	model.BuildModel(&tubeWalkWayDlg.m_xTemplUI.m_xWalkWayTempl);
	CLDSNode* pNewBaseNode=model.GetLdsNodeFromId(KEY4C("BN"));
	if(pNewBaseNode)
	{
		tubeWalkWayDlg.m_pBaseNode=pNewBaseNode;
		GEPOINT lenStdVec=tubeWalkWayDlg.m_pBaseTube->pEnd->xOriginalPos-tubeWalkWayDlg.m_pBaseTube->pStart->xOriginalPos;
		bool reverse=lenStdVec*model.acs.axis_x<0;
		double length=lenStdVec.mod();
		if(reverse)
			lenStdVec*=-1;
		lenStdVec/=length;
		GEPOINT lv2start;
		if(!reverse)
		{
			lv2start=pNewBaseNode->xOriginalPos-tubeWalkWayDlg.m_pBaseTube->pStart->xOriginalPos;
			length+=tubeWalkWayDlg.m_pBaseTube->endOdd();
		}
		else
		{
			lv2start=pNewBaseNode->xOriginalPos-tubeWalkWayDlg.m_pBaseTube->pEnd->xOriginalPos;
			length+=tubeWalkWayDlg.m_pBaseTube->startOdd();
		}
		double dist2start=lv2start*lenStdVec;
		if(dist2start>length)
		{	//超出当前钢管长度范围，重新搜索另一根
			LINEPARTSET rodset;
			Ta.SearchSortedColinearRods(tubeWalkWayDlg.m_pBaseTube,rodset,NULL,NULL);
			for(CLDSLinePart* pRod=rodset.GetFirst();pRod;pRod=rodset.GetNext())
			{
				if(pRod==tubeWalkWayDlg.m_pBaseTube)
					continue;
				CPtInLineCheck check(pRod->pStart->xOriginalPos,pRod->pEnd->xOriginalPos);
				double len2=check.Length();
				double scale=check.CheckPointEx(pNewBaseNode->xOriginalPos);
				if(scale>-pRod->startOdd()/len2&&scale<1+pRod->endOdd()/len2)
				{
					pNewBaseNode->hFatherPart=pRod->handle;
					break;
				}
			}
			tubeWalkWayDlg.m_pBaseNode=pNewBaseNode;
			if(pNewBaseNode->hFatherPart!=tubeWalkWayDlg.m_pBaseTube->handle)
			{	//替换更新支架座板中的关联钢管句柄 wjh-2018.10.8
				for(CLDSPart* pPart=model.m_xMapPartSet.GetFirst();pPart;pPart=model.m_xMapPartSet.GetNext())
				{
					if(pPart->m_hPartWeldParent==tubeWalkWayDlg.m_pBaseTube->handle)
						pPart->m_hPartWeldParent=pNewBaseNode->hFatherPart;
					if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
						continue;
					CLDSParamPlate* pParamPlate=(CLDSParamPlate*)pPart;
					if(pParamPlate->m_iParamType!=TYPE_CIRRIBPLATE)
						continue;
					DESIGN_PLATE_ITEM* pItem=pParamPlate->GetDesignItem('C');
					if(pItem&&pItem->hValue==tubeWalkWayDlg.m_pBaseTube->handle)
						pParamPlate->SetDesignItemValue('C',pNewBaseNode->hFatherPart,NULL,PARAM_POS,1);
					pParamPlate->DesignPlate();
				}
			}
		}
	}
	tubeWalkWayDlg.m_pInnerJg=(CLDSLineAngle*)model.GetLdsRodFromId(CTubeWalkWayStdComTempl::WAY_INNER_ANGLE_ID);
	tubeWalkWayDlg.m_pOuterJg=(CLDSLineAngle*)model.GetLdsRodFromId(CTubeWalkWayStdComTempl::WAY_OUTER_ANGLE_ID);
	tubeWalkWayDlg.m_hOuterAngle=tubeWalkWayDlg.m_pOuterJg?tubeWalkWayDlg.m_pOuterJg->handle:0;
	tubeWalkWayDlg.m_hInnerAngle=tubeWalkWayDlg.m_pInnerJg?tubeWalkWayDlg.m_pInnerJg->handle:0;
	tubeWalkWayDlg.m_pBaseNode=model.GetLdsNodeFromId(KEY4C("BN"));
	if(tubeWalkWayDlg.m_pBaseNode!=NULL)
		tubeWalkWayDlg.m_hBaseNode=tubeWalkWayDlg.m_pBaseNode->handle;
	CLDSNode* pInnerBeamDatumNode=model.GetLdsNodeFromId(KEY4C("HI"));
	CLDSNode* pOuterBeamDatumNode=model.GetLdsNodeFromId(KEY4C("HS"));
	tubeWalkWayDlg.m_hInnerBeamDatumNode=pInnerBeamDatumNode!=NULL?pInnerBeamDatumNode->handle:0;
	tubeWalkWayDlg.m_hOuterBeamDatumNode=pOuterBeamDatumNode!=NULL?pOuterBeamDatumNode->handle:0;
	//刷新实体
	for(CLDSNode* pNode=model.m_xMapNodeSet.GetFirst();pNode;pNode=model.m_xMapNodeSet.GetNext())
		NewNode(pNode);
	//重置杆件位置
	SmartPartPtr pSmartPart;
	for(pSmartPart=model.m_xMapPartSet.GetFirst();pSmartPart.IsHasPtr();pSmartPart=model.m_xMapPartSet.GetNext())
	{
		if(pSmartPart->IsAngle())
		{
			pSmartPart.pAngle->ClearFlag();
			pSmartPart.pAngle->cal_x_wing_norm();
			pSmartPart.pAngle->cal_y_wing_norm();
			pSmartPart.pAngle->CalPosition();
			if(pSmartPart.pAngle->desStartOdd.m_iOddCalStyle==0)
				pSmartPart.pAngle->CalStartOddment();
			if(pSmartPart.pAngle->desEndOdd.m_iOddCalStyle==0)
				pSmartPart.pAngle->CalEndOddment();
		}
		else if(pSmartPart->IsLinePart())
			pSmartPart.pRod->CalPosition();
	}
	//重置钢板位置
	for(pSmartPart=model.m_xMapPartSet.GetFirst();pSmartPart.IsHasPtr();pSmartPart=model.m_xMapPartSet.GetNext())
	{
		if(pSmartPart->GetClassTypeId()==CLS_PARAMPLATE)
			pSmartPart.pParamPlate->DesignPlate();
	}
	//重置螺栓位置
	for(pSmartPart=model.m_xMapPartSet.GetFirst();pSmartPart.IsHasPtr();pSmartPart=model.m_xMapPartSet.GetNext())
	{
		if(pSmartPart->IsBolt())
		{
			PARTSET partset;
			pView->GetBoltActiveRelaPartSet(pSmartPart.pBolt,partset,NULL);
			pSmartPart.pBolt->des_base_pos.norm_offset.key_str=CLDSBolt::EmendZOffsetStr(pSmartPart.pBolt,partset);
			pSmartPart.pBolt->correct_worknorm();
			pSmartPart.pBolt->correct_pos();
		}
	}
	for(CLDSPart* pPart=model.m_xMapPartSet.GetFirst();pPart;pPart=model.m_xMapPartSet.GetNext())
	{
		pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
		if(!pPart->IsAngle())
			continue;
		//更新螺栓实体
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pPart;
		for(CLsRef* pLsRef=pAngle->GetFirstLsRef();pLsRef;pLsRef=pAngle->GetNextLsRef())
		{
			CLDSBolt* pBolt=pLsRef->GetLsPtr();
			pBolt->Create3dSolidModel();
			g_pSolidDraw->NewSolidPart(pBolt->GetSolidPartObject());
		}
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
	return TRUE;
}
int CLDSView::DesignTubeWalkWay()
{
	bool firstCall=true;
	while(true){
		if(!_DesignTubeWalkWay(this,!firstCall))
			return FALSE;
		firstCall=false;
	}
	return !firstCall;
}
//////////////////////////////////////////////////////////////////////////
//设计钢管安全挂点支架（站位平台的挂点设置）
static CDesignTubeSafeHolderDlgEx safeHolderDlg;
int CLDSView::DesignTubeSafeHolder()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CCmdLockObject cmdLock(this);
	if(!cmdLock.LockSuccessed())
		return FALSE;
	g_pSolidDraw->ReleaseSnapStatus();
	CString cmdStr;
	//选择基准钢管
	CLDSLineTube* pBaseTube=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("please choose the base linetube：");
#else
	cmdStr.Format("请选择设计安全挂点支架的基准钢管:");
#endif	
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::LINESPACE,SNAP_LINE);
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_LINETUBE));
	while(true)
	{
		pCmdLine->FillCmdLine(cmdStr,"");
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		pBaseTube=(CLDSLineTube*)console.FromPartHandle(obj.hRelaObj,CLS_LINETUBE);
		if(pBaseTube)
			break;
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->SetEntSnapStatus(pBaseTube->handle);
	pCmdLine->FinishCmdLine(CXhChar16("钢管0x%X",pBaseTube->handle));
	//选择设计节点
	CLDSNode* pBaseNode=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("please base nodes first:");
#else
	cmdStr.Format("请选择基准节点:");
#endif
	verifier.ClearSnapFlag();
	verifier.SetVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
	verifier.AddVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	{
		CDisplayNodeAtFrontLife displayNode;
		displayNode.DisplayNodeAtFront();
		while(true)
		{
			pCmdLine->FillCmdLine(cmdStr,"");
			if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
			{
				pCmdLine->CancelCmdLine();
				return FALSE;
			}
			SELOBJ obj(dwhObj,dwExportFlag);
			pBaseNode=console.FromNodeHandle(obj.hRelaObj);
			if(pBaseNode && pBaseNode->hFatherPart==pBaseTube->handle)
				break;
		}
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pBaseNode->handle);
		pCmdLine->FinishCmdLine(CXhChar16("节点0X%X",pBaseNode->handle));
	}
	//初始化设计参数
	safeHolderDlg.m_xTemplUI.pStdTempl->InitComponentTempl(IStdComTempl::STDTMPL_STRU_TUBE_WORKBENCH);
	safeHolderDlg.m_xTemplUI.m_pBaseTube=pBaseTube;
	safeHolderDlg.m_xTemplUI.param.m_hBaseTube=pBaseTube->handle;
	safeHolderDlg.m_xTemplUI.param.m_hBaseNode=pBaseNode->handle;
	if(safeHolderDlg.DoModal()!=IDOK)
		return 0;
	CUndoOperObject undo(&Ta,true);
	CStdComModel model(&Ta);
	model.BuildModel(&safeHolderDlg.m_xTemplUI.m_xSafeHolderTeml);
	//刷新实体
	for(CLDSPart* pPart=model.m_xMapPartSet.GetFirst();pPart;pPart=model.m_xMapPartSet.GetNext())
	{
		pPart->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
		g_pSolidDraw->NewSolidPart(pPart->GetSolidPartObject());
	}
	g_pSolidDraw->ReleaseSnapStatus();
	g_pSolidDraw->Draw();
	return TRUE;
}
#endif

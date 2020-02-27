//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "CmdLineDlg.h"
#include "env_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CLDSView::OnLinkLinePart() 
{
	m_nPrevCommandID=ID_LINK_LINEPART;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="repeat to connect rods";
#else
	m_sPrevCommandName="重复连接杆件";
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	Command("Link");
	//FinishLinkPart();
}

int CLDSView::FinishLinkPart()
{	//连接杆件
	CString cmdStr;
	int retCode=0;
	static char cDefaultType;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSDbObject *pFirstObj=NULL,*pSecondObj=NULL;
	if(m_setSnapObjects.GetNodeNum()==0)
	{	//还不具备执行命令的条件，先设定系统操作环境
		if(!theApp.m_bAnalysisWorkEnv)
		{
			BOOL bHasMultiLinePart=TRUE;
#ifdef __TMA_
			bHasMultiLinePart=FALSE;	//默认TMA基本功能中不含角钢外的多种连接杆件类型
			if(PRODUCT_FUNC::IsHasRoofDesign())		//TMA微波平台功能(辰春钢结构特殊要求)
				bHasMultiLinePart=TRUE;
#endif
			if(bHasMultiLinePart)
			{
				if(cDefaultType==0)
					cDefaultType='G';
#ifdef AFX_TARG_ENU_ENGLISH
				cmdStr.Format("LINK rod type[angle(L)/flat(B)/slot(C)/tube(G)]<%C>:",cDefaultType);
#else
				cmdStr.Format("LINK 杆件类型[角钢(L)/扁钢(B)/槽钢(C)/钢管(G)]<%C>:",cDefaultType);
#endif
				pCmdLine->FillCmdLine(cmdStr,"");
				while(1)
				{
					if(!pCmdLine->GetStrFromCmdLine(cmdStr,CCmdLineDlg::KEYWORD_AS_RETURN,"L|B|C|G"))
					{
						pCmdLine->m_bGettingStrFromCmdLine=FALSE;
						g_pSolidSnap->SetSnapType(SNAP_ALL);
						return -1;
					}
					pCmdLine->m_bGettingStrFromCmdLine=FALSE;
					if(cmdStr.CompareNoCase("L")==0)
						cDefaultType='L';
					else if(cmdStr.CompareNoCase("B")==0)
						cDefaultType='B';
					else if(cmdStr.CompareNoCase("C")==0)
						cDefaultType='C';
					else if(cmdStr.CompareNoCase("G")==0)
						cDefaultType='G';
					else if(cmdStr.GetLength()>0)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						pCmdLine->FillCmdLine("Need option keywords","");
						pCmdLine->FinishCmdLine();
						cmdStr.Format("LINK rod type[angle(L)/flat(B)/slot(C)/tube(G)]]<%C>:",cDefaultType);
#else
						pCmdLine->FillCmdLine("需要选项关键字","");
						pCmdLine->FinishCmdLine();
						cmdStr.Format("LINK 杆件类型[角钢(L)/扁钢(B)/槽钢(C)/钢管(G)]<%C>:",cDefaultType);
#endif
						pCmdLine->FillCmdLine(cmdStr,"");
						continue;
					}
					break;
				}
			}
			else//只能连接角钢
				cDefaultType='L';
		}
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidSet->SetDisplayType(DISP_LINE);//切换到单线显示模式
		m_curTask=TASK_LINK_LINEPART;
		g_pSolidSet->SetDrawType(DRAW_LINK);
		//g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_POINT);
		m_setSnapObjects.Empty();
		m_nObjectSnapedCounts=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("specify starting point:","");
#else
		pCmdLine->FillCmdLine("LINK 指定始端节点:","");
#endif
		return -1;
	}
	else
		pFirstObj=m_setSnapObjects.GetFirst();
	if(m_setSnapObjects.GetNodeNum()==1)
	{
		if(pCmdLine->m_bGettingStrFromCmdLine)
			pCmdLine->FinishCmdLine();
		if(pFirstObj->GetClassTypeId()==CLS_NODE)
		{
			pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
			pCmdLine->FillCmdLine("specify ending point:","");
#else
			pCmdLine->FillCmdLine("LINK 指定终端节点:","");
#endif
			return -1;
		}
		else
			retCode=-3;
	}
	else
	{
		pSecondObj=m_setSnapObjects.GetFirst();
		pSecondObj=m_setSnapObjects.GetNext();
		if(pFirstObj->handle==pSecondObj->handle)
			retCode=-2;
		else if(pSecondObj&&pSecondObj->GetClassTypeId()==CLS_NODE)
		{
			if(theApp.m_bAnalysisWorkEnv)
				retCode= FinishLinkTsaPole((CLDSNode*)pFirstObj,(CLDSNode*)pSecondObj);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			else if(cDefaultType=='L')
				retCode= FinishLinkAngle((CLDSNode*)pFirstObj,(CLDSNode*)pSecondObj)!=NULL;
			else if(cDefaultType=='G')
				retCode= FinishLinkTube((CLDSNode*)pFirstObj,(CLDSNode*)pSecondObj);
			else if(cDefaultType=='B')
				retCode= FinishLinkFlat((CLDSNode*)pFirstObj,(CLDSNode*)pSecondObj);
			else if(cDefaultType=='C')
				retCode= FinishLinkSlot((CLDSNode*)pFirstObj,(CLDSNode*)pSecondObj);
#endif
		}
		else
			retCode=-3;
	}
	//继续连材命令状态
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
	m_setSnapObjects.Empty();
	g_pSolidSet->SetDrawType(DRAW_LINK);	//通过此行恢复鼠标的连材状态
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("specify starting point:","");
#else
	pCmdLine->FillCmdLine("指定始端节点:","");
#endif
	return retCode;
}

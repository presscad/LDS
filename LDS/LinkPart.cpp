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
	m_sPrevCommandName="�ظ����Ӹ˼�";
#endif
	g_pSolidDraw->ReleaseSnapStatus();
	m_setSnapObjects.Empty();
	Command("Link");
	//FinishLinkPart();
}

int CLDSView::FinishLinkPart()
{	//���Ӹ˼�
	CString cmdStr;
	int retCode=0;
	static char cDefaultType;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSDbObject *pFirstObj=NULL,*pSecondObj=NULL;
	if(m_setSnapObjects.GetNodeNum()==0)
	{	//�����߱�ִ����������������趨ϵͳ��������
		if(!theApp.m_bAnalysisWorkEnv)
		{
			BOOL bHasMultiLinePart=TRUE;
#ifdef __TMA_
			bHasMultiLinePart=FALSE;	//Ĭ��TMA���������в����Ǹ���Ķ������Ӹ˼�����
			if(PRODUCT_FUNC::IsHasRoofDesign())		//TMA΢��ƽ̨����(�����ֽṹ����Ҫ��)
				bHasMultiLinePart=TRUE;
#endif
			if(bHasMultiLinePart)
			{
				if(cDefaultType==0)
					cDefaultType='G';
#ifdef AFX_TARG_ENU_ENGLISH
				cmdStr.Format("LINK rod type[angle(L)/flat(B)/slot(C)/tube(G)]<%C>:",cDefaultType);
#else
				cmdStr.Format("LINK �˼�����[�Ǹ�(L)/���(B)/�۸�(C)/�ֹ�(G)]<%C>:",cDefaultType);
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
						pCmdLine->FillCmdLine("��Ҫѡ��ؼ���","");
						pCmdLine->FinishCmdLine();
						cmdStr.Format("LINK �˼�����[�Ǹ�(L)/���(B)/�۸�(C)/�ֹ�(G)]<%C>:",cDefaultType);
#endif
						pCmdLine->FillCmdLine(cmdStr,"");
						continue;
					}
					break;
				}
			}
			else//ֻ�����ӽǸ�
				cDefaultType='L';
		}
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidSet->SetDisplayType(DISP_LINE);//�л���������ʾģʽ
		m_curTask=TASK_LINK_LINEPART;
		g_pSolidSet->SetDrawType(DRAW_LINK);
		//g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_POINT);
		m_setSnapObjects.Empty();
		m_nObjectSnapedCounts=0;
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("specify starting point:","");
#else
		pCmdLine->FillCmdLine("LINK ָ��ʼ�˽ڵ�:","");
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
			pCmdLine->FillCmdLine("LINK ָ���ն˽ڵ�:","");
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
	//������������״̬
	g_pSolidDraw->ReleaseSnapStatus();
	pCmdLine->FinishCmdLine();
	m_setSnapObjects.Empty();
	g_pSolidSet->SetDrawType(DRAW_LINK);	//ͨ�����лָ���������״̬
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("specify starting point:","");
#else
	pCmdLine->FillCmdLine("ָ��ʼ�˽ڵ�:","");
#endif
	return retCode;
}

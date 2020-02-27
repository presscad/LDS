// InsertStdFacePanel.cpp : implementation of the CMainFrame class
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "CmdLineDlg.h"
#include "SelFacePanelDlg.h"
#include "Tower.h"
//#include "ToolKits.h"
//#include "Lic.h"
#include "MirTaAtom.h"
#include "GlobalFunc.h"
#include "InsertStdTemplateOperation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef CFacePanelNode* FACEPANELNODE_PTR;
typedef CFacePanelPole* FACEPANELPOLE_PTR;
//from MirTaAtom.cpp
void AddMirObjToSrcObjRelaObjList(CLDSDbObject *pSrcObj,CLDSDbObject *pMirObj,MIRMSG msg);
CSelFacePanelDlg selfacedlg;
bool InsertStdFacePanel(BOOL bNewCmd)
{
	CCmdLineDlg *pCmdLine=((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSNode *pNode=NULL;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSLineAngle *pPole=NULL;
#else
	CLDSLinePart *pPole=NULL;
#endif
	//static CSelFacePanelDlg selfacedlg;
	if(bNewCmd&&selfacedlg.DoModal()!=IDOK)
		return false;
	NODESET keynode;
	for(int i=0;i<selfacedlg.m_nKeyNodeNum;i++)
	{
		f3dPoint *pPt=NULL;
		pNode=NULL;
		CString ss;
#ifdef AFX_TARG_ENU_ENGLISH
		ss.Format("Please select the %d key nodes of standard template(Total: %d)！",i+1,selfacedlg.m_nKeyNodeNum);
#else 
		ss.Format("请选择第%d个标准模板关键节点(共%d个)！",i+1,selfacedlg.m_nKeyNodeNum);
#endif
		pCmdLine->FillCmdLine(ss,"");
		g_pSolidSnap->SnapPoint(pPt);
		if(pPt)
		{
			pNode=Ta.Node.FromHandle(pPt->ID);
			g_pSolidDraw->SetEntSnapStatus(pPt->ID);
			keynode.append(pNode);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		if(pNode==NULL&&AfxMessageBox("You can't select suitable node, whether to select others？",MB_YESNO)==IDNO)
#else 
		if(pNode==NULL&&AfxMessageBox("没有选中合适的节点，是否继续进行节点选择？",MB_YESNO)==IDNO)
#endif
			return false;
		else if(pNode==NULL)
			i--;
	}
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	if(keynode.GetNodeNum()!=selfacedlg.m_nKeyNodeNum)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The number of key nodes is not enough,standard surface insert failure！");
#else 
		AfxMessageBox("关键节点数量不够，插入标准面失败！");
#endif
		return false;
	}
	CUndoOperObject undo(&Ta,TRUE);
	try{
		CInsertStdTemplateOperation operation;
		if(selfacedlg.m_iTemplateGroup==0)
		{
			CLDSNode* keyNodeArr[4]={NULL};
			for(pNode=keynode.GetFirst(),i=0;pNode!=NULL&&i<4;pNode=keynode.GetNext(),i++)
				keyNodeArr[i]=pNode;
			CStackVariant<BOOL> stack(&g_sysPara.m_bAnglePreferSimplePosMode,FALSE);//出于稳妥考虑插入头部模板时暂时还不启用简单定位模式
			//为防止误操作，标准头部模板暂不支持对称
			operation.CreateStdTemplate(&selfacedlg.m_xHeadTemplate,keyNodeArr,selfacedlg.m_iKeyNodeMir,0,selfacedlg.m_vAcsAxisX);
		}
		else
		{
			CLDSNode* keyNodeArr[4]={NULL};
			for(pNode=keynode.GetFirst(),i=0;pNode!=NULL&&i<4;pNode=keynode.GetNext(),i++)
				keyNodeArr[i]=pNode;
			selfacedlg.m_xFacePanel.m_iFaceType=selfacedlg.m_iTemplateGroup;
			char specLayer[4]="???";
			specLayer[2]=selfacedlg.m_cThirdLayer;
			STDPANEL_INFO panel(specLayer);
			CStackVariant<BOOL> stack(&g_sysPara.m_bAnglePreferSimplePosMode,TRUE);	//新插入杆件，一定是简单定位 wjh-2019.8.11
			operation.CreateStdTemplate(&selfacedlg.m_xFacePanel,keyNodeArr,selfacedlg.m_iKeyNodeMir,selfacedlg.m_iStdFaceMir,&panel);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->PromptStrInCmdLine("Press the Ctrl key and click the left mouse button, and repeated insert standard template's command");
		pCmdLine->FillCmdLine("Command:","");
#else 
		pCmdLine->PromptStrInCmdLine("按下Ctrl键同时单击鼠标左键，重复插入标准模板命令");
		pCmdLine->FillCmdLine("命令:","");
#endif
		return true;
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return false;
	}
	catch(int error)
	{
		error=-1;
		return false;
	}
}

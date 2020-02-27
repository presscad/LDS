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
		ss.Format("Please select the %d key nodes of standard template(Total: %d)��",i+1,selfacedlg.m_nKeyNodeNum);
#else 
		ss.Format("��ѡ���%d����׼ģ��ؼ��ڵ�(��%d��)��",i+1,selfacedlg.m_nKeyNodeNum);
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
		if(pNode==NULL&&AfxMessageBox("You can't select suitable node, whether to select others��",MB_YESNO)==IDNO)
#else 
		if(pNode==NULL&&AfxMessageBox("û��ѡ�к��ʵĽڵ㣬�Ƿ�������нڵ�ѡ��",MB_YESNO)==IDNO)
#endif
			return false;
		else if(pNode==NULL)
			i--;
	}
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	if(keynode.GetNodeNum()!=selfacedlg.m_nKeyNodeNum)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("The number of key nodes is not enough,standard surface insert failure��");
#else 
		AfxMessageBox("�ؼ��ڵ����������������׼��ʧ�ܣ�");
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
			CStackVariant<BOOL> stack(&g_sysPara.m_bAnglePreferSimplePosMode,FALSE);//�������׿��ǲ���ͷ��ģ��ʱ��ʱ�������ü򵥶�λģʽ
			//Ϊ��ֹ���������׼ͷ��ģ���ݲ�֧�ֶԳ�
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
			CStackVariant<BOOL> stack(&g_sysPara.m_bAnglePreferSimplePosMode,TRUE);	//�²���˼���һ���Ǽ򵥶�λ wjh-2019.8.11
			operation.CreateStdTemplate(&selfacedlg.m_xFacePanel,keyNodeArr,selfacedlg.m_iKeyNodeMir,selfacedlg.m_iStdFaceMir,&panel);
		}
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->PromptStrInCmdLine("Press the Ctrl key and click the left mouse button, and repeated insert standard template's command");
		pCmdLine->FillCmdLine("Command:","");
#else 
		pCmdLine->PromptStrInCmdLine("����Ctrl��ͬʱ�������������ظ������׼ģ������");
		pCmdLine->FillCmdLine("����:","");
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

//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "dlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "PromptDlg.h"
#include "MirTaAtom.h"
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
#include "lds_part.h"
#include "database.h"
#include "LinkTsaPoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////
extern char CalPartPos(CLDSLinePart* pPart);
/////////////////////////////////////////////////////////
BOOL CLDSView::FinishLinkTsaPole(CLDSNode *pStartNode, CLDSNode *pEndNode)
#if !defined(__LDS_)&&!defined(__TSA_)
{return TRUE;}
#else
{
	static CLinkTsaPoleDlg link_pole_dlg;
	CLDSLinePart* pNowPole=NULL;
	f3dLine line;
	f3dPoint view_norm,side_view_norm;
	link_pole_dlg.m_pStartNode=pStartNode;
	link_pole_dlg.m_pEndNode=pEndNode;
	if(link_pole_dlg.DoModal()!=IDOK)
		return FALSE;
	Ta.BeginUndoListen();
	try
	{
		pNowPole=(CLDSLinePart*)console.AppendPart(CLS_LINEPART);
		if(pStartNode->hFatherPart<0x20)
		{
			pStartNode->hFatherPart = pNowPole->handle;
			pStartNode->SetModified();
		}
		if(pEndNode->hFatherPart<0x20)
		{
			pEndNode->hFatherPart = pNowPole->handle;
			pEndNode->SetModified();
		}
		pNowPole->cMaterial = CSteelMatLibrary::RecordAt(link_pole_dlg.m_iPoleMat).cBriefMark;
		pNowPole->m_uStatMatNo=link_pole_dlg.m_uStatMatNo;
		pNowPole->m_bNeedSelSize=link_pole_dlg.m_bSelSize;
		if(link_pole_dlg.m_iStartJointType==0)
			pNowPole->start_joint_type=JOINT_HINGE;
		else if(link_pole_dlg.m_iStartJointType==1)
			pNowPole->start_joint_type=JOINT_RIGID;
		else
			pNowPole->start_joint_type=JOINT_AUTOJUSTIFY;
		if(link_pole_dlg.m_iEndJointType==0)
			pNowPole->end_joint_type=JOINT_HINGE;
		else if(link_pole_dlg.m_iEndJointType==1)
			pNowPole->end_joint_type=JOINT_RIGID;
		else
			pNowPole->end_joint_type=JOINT_AUTOJUSTIFY;
		if(link_pole_dlg.m_iStartForceType==0)
			pNowPole->start_force_type=CENTRIC_FORCE;
		else
			pNowPole->start_force_type=ECCENTRIC_FORCE;
		if(link_pole_dlg.m_iEndForceType==0)
			pNowPole->end_force_type=CENTRIC_FORCE;
		else
			pNowPole->end_force_type=ECCENTRIC_FORCE;
		if(link_pole_dlg.m_iCalLenType==0)
			pNowPole->CalLenCoef.iTypeNo=0;
		else
			pNowPole->CalLenCoef.iTypeNo=link_pole_dlg.m_iCalLenType+3;
		pNowPole->pStart = pStartNode;
		pNowPole->pEnd   = pEndNode;

		//-------Wjh 未完成 --------------
		switch(link_pole_dlg.m_iPart1stChar)
		{
		case 0:
			pNowPole->layer(0) = 'T';
			break;
		case 1:
			pNowPole->layer(0) = 'S';
			break;
		case 2:
			pNowPole->layer(0) = 'L';
		default:
			break;
		}
		switch(link_pole_dlg.m_iPart2ndChar)
		{
		case 0:
			pNowPole->layer(1) = 'Z';
			break;
		case 1:
			pNowPole->layer(1) = 'X';
			break;
		case 2:
			pNowPole->layer(1) = 'F';
			break;
		case 3:
			pNowPole->layer(1) = 'H';
			break;
		default:
			break;
		}
		pNowPole->layer(2) = theApp.env.layer[2];
		//设置塔腿杆件配材号 wht 13-04-01
		if(pNowPole->Layer(0)=='L'&&pStartNode->Layer(0)=='L')
			pNowPole->cfgword&=pStartNode->cfgword;
		if(pNowPole->Layer(0)=='L'&&pEndNode->Layer(0)=='L')
			pNowPole->cfgword&=pEndNode->cfgword;
		pNowPole->SetStart(pNowPole->pStart->Position(false));
		pNowPole->SetEnd(pNowPole->pEnd->Position(false));
		NewLinePart(pNowPole);
		g_pSolidDraw->SetEntSnapStatus(pNowPole->handle);
		if(!MirTaAtom(pNowPole, link_pole_dlg.mirmsg))
		{
			Ta.EndUndoListen();
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("The work condition can't symmetry like this，symmetry failer.");
#else 
			AfxMessageBox("此工况不能这样的对称，对称失败");
#endif
			return FALSE;
		}
	}
	catch(char* sErrorMsg)
	{
		Ta.EndUndoListen();
		AfxMessageBox(sErrorMsg);
		RedrawAll();
#ifdef AFX_TARG_ENU_ENGLISH
		MessageBox("Process fail,roll back!");
#else 
		MessageBox("操作失败,程序回滚!");
#endif
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
	Invalidate(FALSE);
	return TRUE;
}
#endif
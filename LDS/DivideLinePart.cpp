//<LOCALE_TRANSLATE BY wbt />
#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "CmdLineDlg.h"
#include "SortFunc.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "LinePartDivideDlg.h"
#include "MirTaAtom.h"
#include "dlg.h"
#include "SnapTypeVerify.h"
#include "KeypointLifeObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __TSA_
//重新布置被截断角钢上的螺栓
static void ReLayDivideAngleLs(CLsRefList *pLsRefList,CLDSLineAngle *pBreakJg,long hOld)
{
	f3dPoint perp;
	f3dPoint actualStart,actualEnd;
	f3dPoint vec=pBreakJg->Start()-pBreakJg->End();
	normalize(vec);
	actualStart = pBreakJg->Start()+vec*pBreakJg->startOdd();
	actualEnd = pBreakJg->End()-vec*pBreakJg->endOdd();
	f3dLine new_line(actualStart,actualEnd);
	for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
	{
		if(SnapPerp(&perp,new_line,(*pLsRef)->ucs.origin,NULL))
		{
			if(new_line.PtInLine(perp)==2)
			{
				if(pLsRef->start1_end2_mid0==1)
					pLsRef=pBreakJg->AppendStartLsRef(*pLsRef);
				else if(pLsRef->start1_end2_mid0==2)
					pLsRef=pBreakJg->AppendEndLsRef(*pLsRef);
				else
					pLsRef=pBreakJg->AppendMidLsRef(*pLsRef);
				(*pLsRef)->ReplaceReferenceHandle(CHandleReplacer(hOld,pBreakJg->handle));
				//pOrgJg->DeleteLsRef((*pLsRef)->handle);如果链表中所有内容均需删除时,无法删除链表最后一项
				pLsRefList->DeleteCursor();	//标记未删除状态，最后统一删除
			}
		}
	}
	//删除链表中标记为删除状态的节点 wht 10-03-17
	pLsRefList->Clean();
}
#endif
void CLDSView::OnBreakLinePart() 
{
	m_nPrevCommandID=ID_BREAK_LINEPART;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Break Rod";
#else 
	m_sPrevCommandName="重复打断杆件";
#endif
	Command("BREAK");
}

BOOL CLDSView::BreakLinePart()
{
#ifdef __GGG_
	BreakConeTube();
	return TRUE;
#endif
	CLogErrorLife errlife;
	//打断杆件
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr,errCmdStr;
	CSuperSmartPtr<CLDSLinePart> pLinePart;
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("BREAK Rod Divide Specify the rod needs divide:");
	errCmdStr.Format("BREAK Without selecting a suitable rod,Please select a section rod:");
#else
	cmdStr.Format("BREAK 指定要分段的杆件:");
	errCmdStr.Format("BREAK 没有选中合适的杆件，请选择一根待分段的杆件:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	DWORD dwhObj=0,dwExportFlag=0;
	CSnapTypeVerify verifier(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECT_LINEPART));
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_LINE);
	while(1)
	{
		if((g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier))<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		CLDSPart* pPart=Ta.Parts.FromHandle(dwhObj);
		if(pPart&&pPart->IsLinePart())
		{
			pLinePart = (CLDSLinePart*)pPart;
			if(pLinePart.IsHasPtr())//&&pLinePart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
			{	//如果截断不具有完全权限的杆件,会造成重复杆件
				if(!theApp.m_bCooperativeWork||theApp.IsHasModifyPerm(pLinePart->dwStartPermission&pLinePart->dwEndPermission&pLinePart->dwPermission))
					break;
			}
		}
		pCmdLine->FillCmdLine(errCmdStr,"");
	}
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pLinePart->handle));
	g_pSolidDraw->SetEntSnapStatus(pLinePart->handle);
	//
	f3dLine line;
	line.startPt = pLinePart->pStart->Position(false);
	line.endPt = pLinePart->pEnd->Position(false);
	f3dPoint line_vec=line.endPt-line.startPt;
	double line_len=line_vec.mod();
	normalize(line_vec);
	CNodeList nodeListArr[8];
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("BREAK Specifies the section node of selected rod:");
#else 
	cmdStr.Format("BREAK 指定所选杆件上的分段节点:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	verifier.ClearSnapFlag();
	verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
	{//大括号控制CDisplayNodeAtFrontLife的生命周期
	CDisplayNodeAtFrontLife displayNode;
	displayNode.DisplayNodeAtFront();
	while(1)
	{
		if(g_pSolidSnap->SnapObject(&dwhObj,&dwExportFlag,&verifier)<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		SELOBJ obj(dwhObj,dwExportFlag);
		dwhObj=obj.hRelaObj;
		SORTEDNODE_ITEM item;
		item.pNode=Ta.Node.FromHandle(dwhObj);
		if(item.pNode&&line.PtInLine(item.pNode->Position(false))==2)
		{
			f3dPoint vec=item.pNode->Position(false)-line.startPt;
			item.distToStart=vec*line_vec;
			if(item.distToStart>0&&item.distToStart<line_len)
			{	//使用插入排序的方法将选择杆件上的节点按距离始端的距离进行排序(升序)
				for(SORTEDNODE_ITEM *pItem=nodeListArr[0].GetFirst();pItem;pItem=nodeListArr[0].GetNext())
				{
					if(pItem->distToStart>item.distToStart)
					{
						nodeListArr[0].insert(item);
						break;
					}
					else if(pItem->distToStart==item.distToStart)
						break;	//重复节点略去不管
				}
				if(pItem==NULL)
					nodeListArr[0].append(item);
			}
		}
		if(nodeListArr[0].GetNodeNum()>0)
			break;
		pCmdLine->FillCmdLine(cmdStr,"");
	}
	CLDSNode *pSelNode=nodeListArr[0].GetFirst()->pNode;
	pCmdLine->FinishCmdLine(CXhChar16("0x%X",pSelNode->handle));
	g_pSolidDraw->SetEntSnapStatus(pSelNode->handle);
	}
	Ta.StartUndoGroup();
	typedef CLDSLinePart* LINEPARTPTR;
	LINEPARTPTR LinePartPtrArr[8]={NULL};
	LINEPARTSET partset[8];
	static CMirMsgDlg mir_dlg;
	int flagArr[4]={0,1,2,4};
	if(mir_dlg.DoModal()==IDOK)
	{
		for(int i=0;i<4;i++)
		{	
			if(i==0)
			{
				LinePartPtrArr[0]=pLinePart;
				continue;
			}
			else if((mir_dlg.mirmsg.axis_flag&flagArr[i])==0)
				continue;
			MIRMSG msg=mir_dlg.mirmsg.GetSubMirItem(i);
			CLDSLinePart *pMirLinePart=(CLDSLinePart*)pLinePart.LinePartPointer()->GetMirPart(msg);
			if(pMirLinePart)
			{
				LinePartPtrArr[i]=pMirLinePart;
				//查找对称节点
				SORTEDNODE_ITEM *pItem;
				for(pItem=nodeListArr[0].GetFirst();pItem;pItem=nodeListArr[0].GetNext())
				{
					CLDSNode *pMirNode=pItem->pNode->GetMirNode(msg);
					if(pMirNode)
					{
						SORTEDNODE_ITEM *pMirItem=nodeListArr[i].append();
						pMirItem->pNode=pMirNode;
						pMirItem->distToStart=pItem->distToStart;
					}
					else
						break;
				}
				if(pItem)
				{	//未找到全部对称节点 清空节点链表 方便打断时判断是否合法
					nodeListArr[i].Empty();
					continue;
				}
			}
		}
		//处理旋转对称
		MIRMSG mirmsg=mir_dlg.mirmsg;
		int array_num=mirmsg.array_num;
		if((mirmsg.axis_flag&8)==0)	//无旋转对称
			array_num=0;
		else
			mirmsg.axis_flag=8;
		short init_angle=mirmsg.rotate_angle;
		int index=4;
		for(int i=0;i<4;i++)
		{	//旋转对称
			if(LinePartPtrArr[i]==NULL)
				continue;
			CLDSLinePart *pSrcLineAngle = LinePartPtrArr[i];
			for(int j=0;j<array_num;j++)
			{	//进行多次旋转对称时基准构件为同一个构件，只需要调整旋转角度即可
				if(pSrcLineAngle==NULL)
					continue;
				//调整旋转角度
				mirmsg.rotate_angle=init_angle*(j+1);
				CLDSLinePart *pMirLinePart=(CLDSLinePart*)pSrcLineAngle->GetMirPart(mirmsg);
				if(pMirLinePart==NULL)
					continue;
				//查找对称节点
				LinePartPtrArr[index]=pMirLinePart;
				SORTEDNODE_ITEM *pItem;
				for(pItem=nodeListArr[i].GetFirst();pItem;pItem=nodeListArr[i].GetNext())
				{
					CLDSNode *pMirNode=pItem->pNode->GetMirNode(mirmsg);
					if(pMirNode)
					{
						SORTEDNODE_ITEM *pMirItem=nodeListArr[index].append();
						pMirItem->pNode=pMirNode;
						pMirItem->distToStart=pItem->distToStart;
					}
					else
						break;
				}
				if(pItem)	
				{	//未找到全部对称节点 清空节点链表 方便打断时判断是否合法
					nodeListArr[index].Empty();
					continue;
				}
				index++;
			}
		}
		//打断杆件
		LINEPARTSET linePartSet;
		for (i=0;i<8;i++)
		{
			if(LinePartPtrArr[i]==NULL || nodeListArr[i].GetNodeNum()<=0)
				continue;
			FinishBreakLinePart(LinePartPtrArr[i],&nodeListArr[i],partset[i]);
			for(CLDSLinePart* pRod=partset[i].GetFirst();pRod;pRod=partset[i].GetNext())
				linePartSet.append(pRod);
		}
		//对打断的杆件添加关联对象
		for(CLDSLinePart* pRod=linePartSet.GetFirst();pRod;pRod=linePartSet.GetNext())
			FinishRetrieveRelaMirObject(pRod);
	}
	RedrawAll();
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
	Ta.EndUndoGroup();
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	BreakLinePart();//pCmdLine->FillCmdLine("命令:","");
	return TRUE;
}
#ifdef __GGG_
BOOL CLDSView::BreakConeTube()
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	int ID = 0;
	CString cmdStr;
	CLDSNode *pNode = NULL;
	CLDSPart *pPart = NULL;
	CSuperSmartPtr<CLDSLinePart> pLinePart;
	g_pSolidDraw->ReleaseSnapStatus();
	//切换到单线显示状态
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
	pCmdLine->FillCmdLine("BREAK 钢管杆分段 指定要分段的钢管杆:","");
	while(1)
	{
		f3dLine line;
		int ret = g_pSolidSnap->SnapLine(line);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		else if(ret>0)
		{
			pPart=Ta.Parts.FromHandle(line.ID);
			if(pPart&&pPart->IsLinePart())
				pLinePart = (CLDSLinePart*)pPart;
			g_pSolidDraw->ReleaseSnapStatus();
			g_pSolidDraw->SetEntSnapStatus(line.ID);
			if(pLinePart.IsHasPtr())//&&pLinePart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
			{	//如果截断不具有完全权限的杆件,会造成重复杆件
				if(!theApp.m_bCooperativeWork||theApp.IsHasModifyPerm(pLinePart->dwStartPermission&pLinePart->dwEndPermission&pLinePart->dwPermission))
					break;
			}
		}
		g_pSolidDraw->ReleaseSnapStatus();
		pCmdLine->FillCmdLine("没有选中合适的杆件！","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("请选择一根待分段的杆件:","");
	}
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("指定所选杆件上的分段节点:","");
	g_pSolidDraw->ReleaseSnapStatus();
	f3dLine line;
	line.startPt = pLinePart->pStart->Position(false);
	line.endPt = pLinePart->pEnd->Position(false);
	f3dPoint line_vec=line.endPt-line.startPt;
	double line_len=line_vec.mod();
	normalize(line_vec);
	CNodeList nodeListArr;
	while(1)
	{
		f3dPoint *point=NULL;
		int ret = g_pSolidSnap->SnapPoint(point);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		else if(ret==0&&nodeListArr.GetNodeNum()>0)
			break;
		SORTEDNODE_ITEM item;
		if(!point) 
			continue;
		item.pNode=Ta.Node.FromHandle(point->ID);
		if(item.pNode&&line.PtInLine(item.pNode->Position(true))==2)
		{
			if(item.pNode)
			{
				f3dPoint vec=item.pNode->Position(true)-line.startPt;
				item.distToStart=vec*line_vec;
				if(item.distToStart>0&&item.distToStart<line_len)
				{	//使用插入排序的方法将选择杆件上的节点按距离始端的距离进行排序(升序)
					for(SORTEDNODE_ITEM *pItem=nodeListArr.GetFirst();pItem;pItem=nodeListArr.GetNext())
					{
						if(pItem->distToStart>item.distToStart)
						{
							nodeListArr.insert(item);
							break;
						}
						else if(pItem->distToStart==item.distToStart)
							break;	//重复节点略去不管
					}
					if(pItem==NULL)
						nodeListArr.append(item);
					g_pSolidDraw->SetEntSnapStatus(point->ID);
				}
			}
		}
	}
	CONETUBESET partset;
	FinishBreakConeTube(pLinePart.ConeTubePointer(),&nodeListArr,partset);
	RedrawAll();
	m_pDoc->SetModifiedFlag();  // 修改数据后应调用此函数置修改标志.
	Ta.EndUndoListen();
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	BreakLinePart();//pCmdLine->FillCmdLine("命令:","");
	return TRUE;
}
BOOL CLDSView::FinishBreakConeTube(CLDSConeTube* pConeTube,CNodeList *pNodeList,CONETUBESET &partset)
{
	if(pConeTube==NULL||pNodeList==NULL)
		return FALSE;
	if(pNodeList->GetNodeNum()<=0)
		return FALSE;
	CLDSNode *pPrevNode=pConeTube->pStart;
	CLDSConeTube* pNewConeTube;
	int i=0; 
	double fDispersion = (pConeTube->size_wideUpSide - pConeTube->size_wide)/(pNodeList->GetNodeNum()+1);
	for(SORTEDNODE_ITEM *pItem=pNodeList->GetFirst();pItem;pItem=pNodeList->GetNext())
	{
		pNewConeTube=(CLDSConeTube*)console.AppendPart(pConeTube->GetClassTypeId());
		pNewConeTube->pStart=pPrevNode;
		pNewConeTube->pEnd=pItem->pNode;
		pNewConeTube->CopyProperty(*pConeTube);
		pNewConeTube->longth = pNewConeTube->pEnd->Position(false).z - pNewConeTube->pStart->Position(false).z;
		pNewConeTube->size_wideUpSide = pConeTube->size_wide+(i+1)*fDispersion;
		pNewConeTube->size_wide = pConeTube->size_wide+i*fDispersion;
		i++;
		pPrevNode=pNewConeTube->pEnd;
		partset.append(pNewConeTube);
	}
	CLDSConeTube *pStartLinePart=partset.GetFirst();
	//最后一段杆件
	CLDSConeTube *pEndLinePart=(CLDSConeTube*)console.AppendPart(pConeTube->GetClassTypeId());
	pEndLinePart->pStart=pPrevNode;
	pEndLinePart->pEnd=pConeTube->pEnd;
	pEndLinePart->CopyProperty(*pConeTube);
	pEndLinePart->longth = pEndLinePart->pEnd->Position(false).z - pEndLinePart->pStart->Position(false).z;
	pEndLinePart->size_wide = pConeTube->size_wide+pNodeList->GetNodeNum()*fDispersion;
	partset.append(pEndLinePart);
	f3dPoint pos;
	f3dLine ber_line(pConeTube->Start(),pConeTube->End());
	//设置各分段杆件的两端位置设计参数及肢/工作法线的计算参数
	for(pNewConeTube=partset.GetFirst();pNewConeTube;pNewConeTube=partset.GetNext())
	{
#ifndef __TSA_
		if(pConeTube->desEndPos.endPosType!=2&&pConeTube->desStartPos.endPosType!=2)
		{	//被打断的杆件起始/终止点定位方式都不是点线投影
			if(pNewConeTube!=pEndLinePart)
			{	//不是终分段杆件
				pNewConeTube->desEndPos.endPosType=2;	//点线投影
				pNewConeTube->desEndPos.bDatumStartStartEnd=TRUE;
				pNewConeTube->desEndPos.bDatumEndStartEnd=FALSE;
				pNewConeTube->desEndPos.hDatumStartPart=pStartLinePart->handle;
				pNewConeTube->desEndPos.hDatumEndPart=pEndLinePart->handle;
				SnapPerp(&pos,ber_line,pNewConeTube->pEnd->Position(false));
				pNewConeTube->SetEnd(pos);
			}
			else
			{
				pNewConeTube->desEndPos=pConeTube->desEndPos;
				pNewConeTube->SetEnd(pConeTube->End());
			}
			if(pNewConeTube!=pStartLinePart)
			{	//不是始分段杆件
				pNewConeTube->desStartPos.endPosType=2;	//点线投影
				pNewConeTube->desStartPos.bDatumStartStartEnd=TRUE;
				pNewConeTube->desStartPos.bDatumEndStartEnd=FALSE;
				pNewConeTube->desStartPos.hDatumStartPart=pStartLinePart->handle;
				pNewConeTube->desStartPos.hDatumEndPart=pEndLinePart->handle;
				SnapPerp(&pos,ber_line,pNewConeTube->pStart->Position(false));
				pNewConeTube->SetStart(pos);
			}
			else
			{
				pNewConeTube->desStartPos=pConeTube->desStartPos;
				pNewConeTube->SetStart(pConeTube->Start());
			}
		}
		else if(pConeTube->desStartPos.endPosType!=2&&pConeTube->desEndPos.endPosType==2)
		{	//被打断的杆件起始点定位方式不是点线投影,终止点定位方式是点线投影
			if(pNewConeTube!=pStartLinePart)
			{	//不是始分段杆件
				pNewConeTube->desStartPos=pConeTube->desEndPos;
				pNewConeTube->desStartPos.hDatumStartPart=pStartLinePart->handle;
			}
			else
				pNewConeTube->desStartPos=pConeTube->desStartPos;
			pNewConeTube->SetStart(pNewConeTube->pStart->Position(false));	
			pNewConeTube->desEndPos=pConeTube->desEndPos;
			pNewConeTube->desEndPos.hDatumStartPart=pStartLinePart->handle;
			pNewConeTube->SetEnd(pNewConeTube->pEnd->Position(false));
		}
		else if(pConeTube->desStartPos.endPosType==2&&pConeTube->desEndPos.endPosType!=2)
		{	//被打断的杆件起始点定位方式是点线投影,终止点定位方式不是点线投影
			if(pNewConeTube!=pEndLinePart)
			{	//不是终分段杆件
				pNewConeTube->desEndPos=pConeTube->desStartPos;
				pNewConeTube->desEndPos.hDatumEndPart=pEndLinePart->handle;
			}
			else
				pNewConeTube->desEndPos=pConeTube->desEndPos;
			pNewConeTube->SetEnd(pNewConeTube->pEnd->Position(false));	
			pNewConeTube->desStartPos=pConeTube->desStartPos;
			pNewConeTube->desStartPos.hDatumEndPart=pEndLinePart->handle;
			pNewConeTube->SetStart(pNewConeTube->pStart->Position(false));
		}
		else if(pConeTube->desStartPos.endPosType==2&&pConeTube->desEndPos.endPosType==2)
		{	//被打断的杆件起始/终止点定位方式均是点线投影	打断点定位方式同被打断钢管的其诗/终止点定位方式均一致
			pNewConeTube->desEndPos=pConeTube->desEndPos;
			pNewConeTube->SetEnd(pNewConeTube->pEnd->Position(false));
			pNewConeTube->desStartPos=pConeTube->desStartPos;
			pNewConeTube->SetStart(pNewConeTube->pStart->Position(false));
		}
		pNewConeTube->nearWorkPlaneNorm=pConeTube->nearWorkPlaneNorm;
		pNewConeTube->hWorkNormRefPart=pConeTube->hWorkNormRefPart;
#endif
	}
#ifndef __TSA_
	//查找当前杆件连接的钢板,更新钢板连接材列表  wht 10-02-06
	CLDSPlate *pPlate=NULL;
	for(pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	{
		CDesignLjPartPara *pLjPartPara=pPlate->designInfo.FromHandle(pConeTube->handle);
		if(pLjPartPara)
		{
			if(pLjPartPara->start0_end1==0)
				pLjPartPara->hPart=pStartLinePart->handle;
			else if(pLjPartPara->start0_end1==1)
				pLjPartPara->hPart=pEndLinePart->handle;
		}
	}
	pStartLinePart->SetStartOdd(pConeTube->startOdd());
	pEndLinePart->SetEndOdd(pConeTube->endOdd());
	long oldHandleArr[4]={0};
	CLsRefList lsListArr[4];
#endif
	for(pNewConeTube=partset.GetFirst();pNewConeTube;pNewConeTube=partset.GetNext())
	{
		pNewConeTube->ClearFlag();
		pNewConeTube->CalPosition();
		NewLinePart(pNewConeTube);
	}
	long hFirstNewPart=0;
	for(CLDSNode *pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		for(pNewConeTube=partset.GetFirst();pNewConeTube;pNewConeTube=partset.GetNext())
		{
			if(hFirstNewPart==0)
				hFirstNewPart=pNewConeTube->handle;
			f3dLine segLine(pNewConeTube->pStart->Position(false),pNewConeTube->pEnd->Position(false));
			//更新交叉节点依附杆件	wht 09-09-04
			if(pNode->m_cPosCalType==4)	//交叉点
			{
				if(pNode->arrRelationPole[0]==pConeTube->handle
					&&segLine.PtInLine(pNode->Position(false))>0)	//在线段上
				{
					pNode->arrRelationPole[0]=pNewConeTube->handle;
					//更新依附杆件的同时需要更新依附节点
					pNode->arrRelationNode[0]=pNewConeTube->pStart->handle;
					pNode->arrRelationNode[1]=pNewConeTube->pEnd->handle;
				}
				if(pNode->arrRelationPole[1]==pConeTube->handle
					&&segLine.PtInLine(pNode->Position(false))>0)	//在线段上
				{
					pNode->arrRelationPole[1]=pNewConeTube->handle;
					//更新依附杆件的同时需要更新依附节点
					pNode->arrRelationNode[2]=pNewConeTube->pStart->handle;
					pNode->arrRelationNode[3]=pNewConeTube->pEnd->handle;
				}
			}
			//更新节点父杆件
			if(pNode==pNewConeTube->pStart&&pNode->hFatherPart==pConeTube->handle)
			{
				pNode->hFatherPart=pNewConeTube->handle;
				break;
			}
			else if(pNode==pNewConeTube->pEnd&&pNode==pConeTube->pEnd&&
				pNode->hFatherPart==pConeTube->handle)
			{
				pNode->hFatherPart=pNewConeTube->handle;
				break;
			}
			else if(pNode->hFatherPart==pConeTube->handle&&segLine.PtInLine(pNode->Position(false))==2)
			{
				pNode->hFatherPart=pNewConeTube->handle;
				break;
			}
		}
	}
	g_pSolidDraw->DelEnt(pConeTube->handle);
	console.DispPartSet.DeleteNode(pConeTube->handle);
	Ta.ReplaceReferenceHandle(CHandleReplacer(pConeTube->handle,hFirstNewPart));
	console.DeletePart(pConeTube->handle);
	g_pSolidDraw->Draw();
	return TRUE;
}
#endif
//修正打断后定位方式为【指定线投影或指定线平推】角钢的参照句柄
static void EmendRodPosParam(LINEPARTSET &partSet)
{
	if(partSet.GetNodeNum()<2)
		return;
	CLDSLinePart *pRod=NULL;
	
	CLDSLinePart *pUpRod=partSet.GetFirst(),*pDownRod=partSet.GetTail();
	if(pUpRod->IsTube())
		return;	//钢管定位方式不需要修正
	BOOL bUpReversal=FALSE,bDownReversal=FALSE;	//是否需要颠倒始终端
	f3dPoint rod_vec=pUpRod->pEnd->Position(false)-pUpRod->pStart->Position(false);
	normalize(rod_vec);
	//1.向上查找共线杆件
	while(1)
	{
		long hNode=pUpRod->pStart->handle;
		if(bUpReversal)
			hNode=pUpRod->pEnd->handle;
		for(pRod=Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			if(pRod==pUpRod||!pRod->ModuleTogetherWith(pUpRod))
				continue;
			else if(pRod->GetClassTypeId()!=pUpRod->GetClassTypeId())
				continue;	//防止双角钢变单角钢时由于单线共线导致误判断为共线情况 wjh-2015.2.13
			else if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0)
				continue;	//怱略组合角钢的子角钢
			f3dPoint vec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
			normalize(vec);
			double dd=vec*rod_vec;
			if(fabs(vec*rod_vec)<EPS_COS)
				continue;	//不共线
			if(pRod->pStart->handle==hNode&&dd<0)
			{
				bUpReversal=TRUE;
				pUpRod=pRod;
				break;
			}
			else if(pRod->pEnd->handle==hNode&&dd>0)
			{
				pUpRod=pRod;
				break;
			}
		}
		if(pRod==NULL)
			break;
	}
	//2.向下查找共线杆件
	while(1)
	{
		long hNode=pDownRod->pEnd->handle;
		if(bDownReversal)
			hNode=pDownRod->pStart->handle;
		for(pRod=Ta.Parts.GetFirstLinePart();pRod;pRod=Ta.Parts.GetNextLinePart())
		{
			if(pRod->pStart==NULL||pRod->pEnd==NULL)
				continue;
			if(pRod==pDownRod||!pRod->ModuleTogetherWith(pDownRod))
				continue;
			else if(pRod->GetClassTypeId()!=pDownRod->GetClassTypeId())
				continue;	//防止双角钢变单角钢时由于单线共线导致误判断为共线情况 wjh-2015.2.13
			else if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0)
				continue;	//怱略组合角钢的子角钢
			f3dPoint vec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
			normalize(vec);
			double dd=vec*rod_vec;
			if(fabs(vec*rod_vec)<EPS_COS)
				continue;	//不共线
			if(pRod->pStart->handle==hNode&&dd>0)
			{
				pDownRod=pRod;
				break;
			}
			else if(pRod->pEnd->handle==hNode&&dd<0)
			{
				bDownReversal=TRUE;
				pDownRod=pRod;
				break;
			}
		}
		if(pRod==NULL)
			break;
	}
	//3.修正打断后杆件定位方式为【指定线投影】或【指定线平推】杆件的参照杆件句柄
	for(pRod=partSet.GetFirst();pRod;pRod=partSet.GetNext())
	{
#ifdef __COMPLEX_PART_INC_
		if(pRod->IsTube())
		{
			CLDSLineTube *pTube=(CLDSLineTube*)pRod;
			//pTube->desEndPos
		}
#endif
#ifdef __ANGLE_PART_INC_
		if(pRod->IsAngle())
		{
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)pRod;
			if(pLineAngle->desStartPos.jgber_cal_style!=1&&(pLineAngle->desStartPos.spatialOperationStyle==16||pLineAngle->desStartPos.spatialOperationStyle==17))
			{	//3.1始端定位方式为指定线平推或投影
				if((pLineAngle->desStartPos.datumLineStart.datum_pos_style==1||pLineAngle->desStartPos.datumLineStart.datum_pos_style==7)
					&&pLineAngle->desStartPos.datumLineStart.des_para.RODEND.hRod!=pUpRod->handle)
				{
					pLineAngle->desStartPos.datumLineStart.des_para.RODEND.hRod=pUpRod->handle;
					if(bUpReversal)
						pLineAngle->desStartPos.datumLineStart.des_para.RODEND.direction=1;
				}
				if((pLineAngle->desStartPos.datumLineStart.datum_pos_style==1||pLineAngle->desStartPos.datumLineStart.datum_pos_style==7)
					&&pLineAngle->desStartPos.datumLineEnd.des_para.RODEND.hRod!=pDownRod->handle)
				{
					pLineAngle->desStartPos.datumLineEnd.des_para.RODEND.hRod=pDownRod->handle;
					if(bDownReversal)
						pLineAngle->desStartPos.datumLineEnd.des_para.RODEND.direction=1;
				}
				if(pLineAngle->pStart&&pLineAngle->pStart->m_cPosCalType==CLDSNode::COORD_SCALE)
				{
					pLineAngle->desStartPos.origin_pos_style=5;	//初始点改为智能识别计算
					pLineAngle->ClearFlag();
				}
			}
			if(pLineAngle->desEndPos.jgber_cal_style!=1&&(pLineAngle->desEndPos.spatialOperationStyle==16||pLineAngle->desEndPos.spatialOperationStyle==17))
			{	//3.2终端定位方式为指定线平推或投影
				if((pLineAngle->desEndPos.datumLineStart.datum_pos_style==1||pLineAngle->desEndPos.datumLineStart.datum_pos_style==7)
					&&pLineAngle->desEndPos.datumLineStart.des_para.RODEND.hRod!=pUpRod->handle)
				{
					pLineAngle->desEndPos.datumLineStart.des_para.RODEND.hRod=pUpRod->handle;
					if(bUpReversal)
						pLineAngle->desEndPos.datumLineStart.des_para.RODEND.direction=1;
				}
				if((pLineAngle->desEndPos.datumLineStart.datum_pos_style==1||pLineAngle->desEndPos.datumLineStart.datum_pos_style==7)
					&&pLineAngle->desEndPos.datumLineEnd.des_para.RODEND.hRod!=pDownRod->handle)
				{
					pLineAngle->desEndPos.datumLineEnd.des_para.RODEND.hRod=pDownRod->handle;
					if(bDownReversal)
						pLineAngle->desEndPos.datumLineEnd.des_para.RODEND.direction=1;
				}
				if(pLineAngle->pEnd&&pLineAngle->pEnd->m_cPosCalType==CLDSNode::COORD_SCALE)
				{
					pLineAngle->desEndPos.origin_pos_style=5;	//初始点改为智能识别计算
					pLineAngle->ClearFlag();
				}
			}
		}
#endif
	}
}

//修正钢板设计参数
static void EmendPlateDesignInfo(LINEPARTSET &partset,CLDSLinePart *pRod)
{
	CLDSLinePart *pStartRod=partset.GetFirst();
	CLDSLinePart *pEndRod=partset.GetTail();
	f3dLine datumLine(pRod->pStart->Position(false),pRod->pEnd->Position(false));
	CLDSModule* pModule=Ta.GetActiveModule();
	for(CLDSPlate *pPlate=(CLDSPlate*)console.EnumPartFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)console.EnumPartNext(CLS_PLATE))
	{
		if(!pModule->IsSonPart(pPlate))		//不属于当前激活呼高
			continue;
		CLDSNode *pDatumNode=console.FromNodeHandle(pPlate->designInfo.m_hBaseNode);
		if(pDatumNode==NULL)
			continue;
		CLDSLinePart *pDestRod=NULL;
		int result=datumLine.PtInLine(pDatumNode->Position(false));
		if(pDatumNode==pRod->pStart||result==-2)
			pDestRod=pStartRod;	//基准节点为该那件始端节点
		else if(pDatumNode==pRod->pEnd||result==-1)
			pDestRod=pEndRod;	//基准节点为该那件终端节点
		else if(result==2)
		{	//基准节点在杆件线段内
			for(CLDSLinePart *pNewLinePart=partset.GetFirst();pNewLinePart;pNewLinePart=partset.GetNext())
			{
				f3dLine segLine(pNewLinePart->pStart->Position(false),pNewLinePart->pEnd->Position(false));
				if(pDatumNode==pNewLinePart->pStart||pDatumNode==pNewLinePart->pEnd
					||segLine.PtInLine(pDatumNode->Position(false))==2)
				{	//基准节点始终端节点或基准节点在当前杆件线段内
					pDestRod=pNewLinePart;
					break;
				}
			}
		}
		else if(result==1)
			pDestRod=pRod;
		else	//基准点不在线上
			continue;
		if(pDestRod==NULL)
			continue;
		if(pDestRod!=pRod)
			pPlate->designInfo.ReplaceReferenceHandle(CHandleReplacer(pRod->handle,pDestRod->handle));
		CDesignLjPartPara *pLjPara;
		//补充基准节点不在打断杆件，但射线杆件是打断杆件的情况
		for(pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
		{
			if(pLjPara->hPart!=pRod->handle)
				continue;
			//根据钢板的设计节点找到目标杆件,然后根据目标杆件调整连接杆件的设计端
			//否则, 横担或隔面Z轴对称的两根杆件如果按X/Y轴对称打断时可能会出错  wxc-2017.5.16
			pLjPara->hPart=pDestRod->handle;
			if(pLjPara->start0_end1==0&&pLjPara->hPart!=pStartRod->handle)
				pLjPara->start0_end1=1;
			else if(pLjPara->start0_end1==1&&pLjPara->hPart!=pEndRod->handle)
				pLjPara->start0_end1=0;
			break;
		}
		//更新钢板射线杆件正负头设计参数
		for(pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
		{
			CLDSLinePart *pRayRod=(CLDSLinePart*)console.FromPartHandle(pLjPara->hPart,CLS_LINEPART);
			if(pRayRod==NULL)
				continue;
			if(pLjPara->start0_end1==0&&pRayRod->desStartOdd.m_hRefPart1==pRod->handle)
				pRayRod->desStartOdd.m_hRefPart1=pDestRod->handle;
			else if(pLjPara->start0_end1==1&&pRayRod->desEndOdd.m_hRefPart1==pRod->handle)
				pRayRod->desEndOdd.m_hRefPart1=pDestRod->handle;
			if(pLjPara->start0_end1==0&&pRayRod->desStartOdd.m_hRefPart2==pRod->handle)
				pRayRod->desStartOdd.m_hRefPart2=pDestRod->handle;
			else if(pLjPara->start0_end1==1&&pRayRod->desEndOdd.m_hRefPart2==pRod->handle)
				pRayRod->desEndOdd.m_hRefPart2=pDestRod->handle;
		}
	}
}
BOOL CLDSView::FinishBreakLinePart(CSuperSmartPtr<CLDSLinePart> pLinePart,CNodeList *pNodeList,LINEPARTSET &partset)
{
	if(pLinePart.IsNULL()||pNodeList==NULL)
		return FALSE;
	if(pNodeList->GetNodeNum()<=0)
		return FALSE;
	CLDSNode *pPrevNode=pLinePart->pStart;
	CSuperSmartPtr<CLDSLinePart> pNewLinePart;
	for(SORTEDNODE_ITEM *pItem=pNodeList->GetFirst();pItem;pItem=pNodeList->GetNext())
	{
		pNewLinePart=(CLDSLinePart*)console.AppendPart(pLinePart->GetClassTypeId());
		pNewLinePart->pStart=pPrevNode;
		pNewLinePart->pEnd=pItem->pNode;
		pNewLinePart->CopyProperty(pLinePart);
		pNewLinePart->SetArcLift(pLinePart->ArcLiftHandle());
		if(pNewLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
			pNewLinePart.LineTubePointer()->m_iNormCalStyle=pLinePart.LineTubePointer()->m_iNormCalStyle;
			pNewLinePart.LineTubePointer()->hWorkNormRefPart=pLinePart.LineTubePointer()->hWorkNormRefPart;
			pNewLinePart.LineTubePointer()->nearWorkPlaneNorm=pLinePart.LineTubePointer()->nearWorkPlaneNorm;
			pNewLinePart.LineTubePointer()->m_iWeldRoadStyle=pLinePart.LineTubePointer()->m_iWeldRoadStyle;
			pNewLinePart.LineTubePointer()->nearWeldRoadVec=pLinePart.LineTubePointer()->nearWeldRoadVec;
			pNewLinePart.LineTubePointer()->SetWorkPlaneNorm(pLinePart.LineTubePointer()->WorkPlaneNorm());
		}
		pPrevNode=pNewLinePart->pEnd;
		partset.append(pNewLinePart);
	}
	CLDSLinePart *pStartLinePart=partset.GetFirst();
	//最后一段杆件
	CLDSLinePart *pEndLinePart=(CLDSLinePart*)console.AppendPart(pLinePart->GetClassTypeId());
	pEndLinePart->pStart=pPrevNode;
	pEndLinePart->pEnd=pLinePart->pEnd;
	pEndLinePart->CopyProperty(pLinePart);
	if(pEndLinePart->GetClassTypeId()==CLS_LINETUBE)
	{
		CLDSLineTube* pEndLineTube=(CLDSLineTube*)pEndLinePart;
		pEndLineTube->m_iNormCalStyle=pLinePart.LineTubePointer()->m_iNormCalStyle;
		pEndLineTube->hWorkNormRefPart=pLinePart.LineTubePointer()->hWorkNormRefPart;
		pEndLineTube->nearWorkPlaneNorm=pLinePart.LineTubePointer()->nearWorkPlaneNorm;
		pEndLineTube->m_iWeldRoadStyle=pLinePart.LineTubePointer()->m_iWeldRoadStyle;
		pEndLineTube->nearWeldRoadVec=pLinePart.LineTubePointer()->nearWeldRoadVec;
		pEndLineTube->SetWorkPlaneNorm(pLinePart.LineTubePointer()->WorkPlaneNorm());
	}
	partset.append(pEndLinePart);
	f3dPoint pos;
	f3dLine ber_line(pLinePart->Start(),pLinePart->End());
	GEPOINT lenStdVec=pLinePart->End()-pLinePart->Start();
	normalize(lenStdVec);
	GEPOINT face_offset_norm;	//不筛选可能导致平推面与当前杆件方向不一致,如塔身平推横材中间打断情况 wjh-2016.4.13
	if(pLinePart->IsAngle()&&fabs(lenStdVec*pLinePart.LineAnglePointer()->desStartPos.face_offset_norm)>0.5)
		face_offset_norm=pLinePart.LineAnglePointer()->desStartPos.face_offset_norm;
	//设置各分段杆件的两端位置设计参数及肢/工作法线的计算参数
	for(pNewLinePart=partset.GetFirst();pNewLinePart.IsHasPtr();pNewLinePart=partset.GetNext())
	{
		if(pLinePart->GetClassTypeId()==CLS_LINEPART)
		{	//被打断的杆件起始/终止点定位方式均是点线投影	打断点定位方式同被打断钢管的起始/终止点定位方式均一致
			pNewLinePart->SetEnd(pNewLinePart->pEnd->ActivePosition());
			pNewLinePart->SetStart(pNewLinePart->pStart->ActivePosition());
			//pNewLinePart->des_wing_norm=pLinePart->des_wing_norm;
		}
#ifndef __TSA_
		else if(pLinePart->GetClassTypeId()==CLS_LINEANGLE||pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			pNewLinePart.LineAnglePointer()->m_cPosCtrlDatumLineType=pLinePart.LineAnglePointer()->m_cPosCtrlDatumLineType;
			pNewLinePart.LineAnglePointer()->m_cPosCtrlType=pLinePart.LineAnglePointer()->m_cPosCtrlType;
			if(pNewLinePart!=pEndLinePart)
			{	//不是终分段杆件
				pNewLinePart.LineAnglePointer()->desEndPos.jgber_cal_style=0;	//点向楞线投影
				pNewLinePart.LineAnglePointer()->desEndPos.origin_pos_style=0;
				//pNewLinePart.LineAnglePointer()->desEndPos.spatialOperationStyle=1;
				BOOL bPush=partset.push_stack();
				CLDSLinePart *pNextLinePart=partset.GetNext();
				//当前杆件的终端节点的父杆件不是下一个杆件，需要指定楞线投影的基准角钢 wht 11-01-19
				if(pNextLinePart&&pNewLinePart->pEnd->hFatherPart!=pNextLinePart->handle)
					pNewLinePart.LineAnglePointer()->desEndPos.datum_jg_h=pNextLinePart->handle;
				if(bPush)
					partset.pop_stack();
				pNewLinePart.LineAnglePointer()->desEndPos.spatialOperationStyle=1;
				pNewLinePart.LineAnglePointer()->desEndPos.wing_x_offset.gStyle=
					pNewLinePart.LineAnglePointer()->desEndPos.wing_y_offset.gStyle=4;
				pNewLinePart.LineAnglePointer()->desEndPos.wing_x_offset.offsetDist=
					pNewLinePart.LineAnglePointer()->desEndPos.wing_y_offset.offsetDist=0;
				/*pNewLinePart.LineAnglePointer()->des_end_pos.start_jg_h=pStartLinePart->handle;
				pNewLinePart.LineAnglePointer()->des_end_pos.end_jg_h=pEndLinePart->handle;
				pNewLinePart.LineAnglePointer()->des_end_pos.iStartJgBerStyle=0;
				pNewLinePart.LineAnglePointer()->des_end_pos.iEndJgBerStyle=1;
				pNewLinePart.LineAnglePointer()->des_end_pos.fStartJgLengOffsetX=
					pNewLinePart.LineAnglePointer()->des_end_pos.fStartJgLengOffsetY=
					pNewLinePart.LineAnglePointer()->des_end_pos.fEndJgLengOffsetX=
					pNewLinePart.LineAnglePointer()->des_end_pos.fEndJgLengOffsetY=0;
				if((pLinePart.LineAnglePointer()->des_end_pos.jgber_cal_style==0&&
					pLinePart.LineAnglePointer()->des_end_pos.datum_pos_style==0&&
					pLinePart.LineAnglePointer()->des_end_pos.datum_to_ber_style==1)||
					pLinePart.LineAnglePointer()->des_end_pos.jgber_cal_style==2) //平推
				{
					pNewLinePart.LineAnglePointer()->desEndPos.jgber_cal_style=2;	//点线平推
					pNewLinePart.LineAnglePointer()->desEndPos.datum_to_ber_style=1;	//平推
					pNewLinePart.LineAnglePointer()->desEndPos.face_offset_norm=
						pLinePart.LineAnglePointer()->desEndPos.face_offset_norm;
					pNewLinePart.LineAnglePointer()->desEndPos.bByOtherEndOffset=
						pLinePart.LineAnglePointer()->desEndPos.bByOtherEndOffset;
					pNewLinePart.LineAnglePointer()->desEndPos.cFaceOffsetDatumLine=
						pLinePart.LineAnglePointer()->desEndPos.cFaceOffsetDatumLine;
				}*/
				SnapPerp(&pos,ber_line,pNewLinePart->pEnd->ActivePosition());
				pNewLinePart->SetEnd(pos);
			}
			else
			{	//终端分段杆件继承原杆件终端设计信息(修正原不继承正负头和切角信息错误) wjh-2011.6.26
				pNewLinePart.LineAnglePointer()->m_bUserSpecEndCutAngle=pLinePart.LineAnglePointer()->m_bUserSpecEndCutAngle;
				pNewLinePart.LineAnglePointer()->cut_angle[2][0]=pLinePart.LineAnglePointer()->cut_angle[2][0];
				pNewLinePart.LineAnglePointer()->cut_angle[2][1]=pLinePart.LineAnglePointer()->cut_angle[2][1];
				pNewLinePart.LineAnglePointer()->cut_angle[3][0]=pLinePart.LineAnglePointer()->cut_angle[3][0];
				pNewLinePart.LineAnglePointer()->cut_angle[3][1]=pLinePart.LineAnglePointer()->cut_angle[3][1];
				pNewLinePart.LineAnglePointer()->cut_wing_para[1][0]=pLinePart.LineAnglePointer()->cut_wing_para[1][0];
				pNewLinePart.LineAnglePointer()->cut_wing_para[1][1]=pLinePart.LineAnglePointer()->cut_wing_para[1][1];
				pNewLinePart.LineAnglePointer()->cut_wing_para[1][2]=pLinePart.LineAnglePointer()->cut_wing_para[1][2];
				pNewLinePart.LineAnglePointer()->cut_wing[1]=pLinePart.LineAnglePointer()->cut_wing[1];
				pNewLinePart.LineAnglePointer()->desEndOdd=pLinePart.LineAnglePointer()->desEndOdd;
				pNewLinePart.LineAnglePointer()->desEndPos=pLinePart.LineAnglePointer()->desEndPos;
				pNewLinePart->SetEnd(pLinePart->End());
			}
			if(pNewLinePart!=pStartLinePart)
			{	//不是始分段杆件
				pNewLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle=16;	//点线投影
				if(pLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle==16||pLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle==17)
				{	//原杆件本身就属于点线投影或平推的直线段中的一部分，此时应继承 wjh-2011.8.21
					pNewLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle=pLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineStart=pLinePart.LineAnglePointer()->desStartPos.datumLineStart;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineEnd=pLinePart.LineAnglePointer()->desStartPos.datumLineEnd;
					/*pNewLinePart.LineAnglePointer()->desStartPos.start_jg_h=pLinePart.LineAnglePointer()->desStartPos.start_jg_h;
					pNewLinePart.LineAnglePointer()->desStartPos.end_jg_h=pLinePart.LineAnglePointer()->desStartPos.end_jg_h;
					pNewLinePart.LineAnglePointer()->desStartPos.iStartJgBerStyle=pLinePart.LineAnglePointer()->desStartPos.iStartJgBerStyle;
					pNewLinePart.LineAnglePointer()->desStartPos.iEndJgBerStyle=pLinePart.LineAnglePointer()->desStartPos.iEndJgBerStyle;*/
				}
				else
				{
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineStart.datum_pos_style=1;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineStart.des_para.RODEND.hRod=pStartLinePart->handle;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineStart.des_para.RODEND.direction=0;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineStart.des_para.RODEND.wing_offset_style=4;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineStart.des_para.RODEND.wing_offset_dist=0;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineEnd.datum_pos_style=1;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineEnd.des_para.RODEND.hRod=pEndLinePart->handle;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineEnd.des_para.RODEND.direction=1;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineEnd.des_para.RODEND.wing_offset_style=4;
					pNewLinePart.LineAnglePointer()->desStartPos.datumLineEnd.des_para.RODEND.wing_offset_dist=0;
					/*pNewLinePart.LineAnglePointer()->desStartPos.start_jg_h=pStartLinePart->handle;
					pNewLinePart.LineAnglePointer()->desStartPos.end_jg_h=pEndLinePart->handle;
					pNewLinePart.LineAnglePointer()->desStartPos.iStartJgBerStyle=0;
					pNewLinePart.LineAnglePointer()->desStartPos.iEndJgBerStyle=1;*/
				}
				/*pNewLinePart.LineAnglePointer()->desStartPos.fStartJgLengOffsetX=
					pNewLinePart.LineAnglePointer()->desStartPos.fStartJgLengOffsetY=
					pNewLinePart.LineAnglePointer()->desStartPos.fEndJgLengOffsetX=
					pNewLinePart.LineAnglePointer()->desStartPos.fEndJgLengOffsetY=0;*/
				/*if((pLinePart.LineAnglePointer()->desStartPos.jgber_cal_style==1&&
					pLinePart.LineAnglePointer()->desStartPos.datum_pos_style==0&&
					pLinePart.LineAnglePointer()->desStartPos.datum_to_ber_style==1)||
					pLinePart.LineAnglePointer()->desStartPos.jgber_cal_style==2) //平推*/
				if(pLinePart.LineAnglePointer()->desStartPos.IsFaceOffset())
				{	//拷贝平推面信息
					pNewLinePart.LineAnglePointer()->desStartPos.jgber_cal_style=0;	//点线平推
					pNewLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle=17;	//平推
					pNewLinePart.LineAnglePointer()->desStartPos.face_offset_norm=face_offset_norm;
					/*pNewLinePart.LineAnglePointer()->desStartPos.bByOtherEndOffset=
						pLinePart.LineAnglePointer()->desStartPos.bByOtherEndOffset;*/
					pNewLinePart.LineAnglePointer()->desStartPos.cFaceOffsetDatumLine=
						pLinePart.LineAnglePointer()->desStartPos.cFaceOffsetDatumLine;
				}
				SnapPerp(&pos,ber_line,pNewLinePart->pStart->ActivePosition());
				pNewLinePart->SetStart(pos);
			}
			else
			{	//始端分段杆件继承原杆件始端设计信息(修正原不继承正负头和切角信息错误) wjh-2011.6.26
				pNewLinePart.LineAnglePointer()->m_bUserSpecStartCutAngle=pLinePart.LineAnglePointer()->m_bUserSpecStartCutAngle;
				pNewLinePart.LineAnglePointer()->cut_angle[0][0]=pLinePart.LineAnglePointer()->cut_angle[0][0];
				pNewLinePart.LineAnglePointer()->cut_angle[0][1]=pLinePart.LineAnglePointer()->cut_angle[0][1];
				pNewLinePart.LineAnglePointer()->cut_angle[1][0]=pLinePart.LineAnglePointer()->cut_angle[1][0];
				pNewLinePart.LineAnglePointer()->cut_angle[1][1]=pLinePart.LineAnglePointer()->cut_angle[1][1];
				pNewLinePart.LineAnglePointer()->cut_wing_para[0][0]=pLinePart.LineAnglePointer()->cut_wing_para[0][0];
				pNewLinePart.LineAnglePointer()->cut_wing_para[0][1]=pLinePart.LineAnglePointer()->cut_wing_para[0][1];
				pNewLinePart.LineAnglePointer()->cut_wing_para[0][2]=pLinePart.LineAnglePointer()->cut_wing_para[0][2];
				pNewLinePart.LineAnglePointer()->cut_wing[0]=pLinePart.LineAnglePointer()->cut_wing[0];
				pNewLinePart.LineAnglePointer()->desStartOdd=pLinePart.LineAnglePointer()->desStartOdd;
				pNewLinePart.LineAnglePointer()->desStartPos=pLinePart.LineAnglePointer()->desStartPos;
				pNewLinePart->SetStart(pLinePart->Start());		
			}
			pNewLinePart.LineAnglePointer()->des_norm_x=pLinePart.LineAnglePointer()->des_norm_x;
			pNewLinePart.LineAnglePointer()->des_norm_y=pLinePart.LineAnglePointer()->des_norm_y;
			pNewLinePart.LineAnglePointer()->set_norm_x_wing(pLinePart.LineAnglePointer()->vxWingNorm,pLinePart.LineAnglePointer()->des_norm_x.bSpecific);
			pNewLinePart.LineAnglePointer()->set_norm_y_wing(pLinePart.LineAnglePointer()->vyWingNorm,pLinePart.LineAnglePointer()->des_norm_y.bSpecific);
		}
		else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
		{
			if(pLinePart.LineTubePointer()->desEndPos.endPosType!=2&&pLinePart.LineTubePointer()->desStartPos.endPosType!=2)
			{	//被打断的杆件起始/终止点定位方式都不是点线投影
				if(pNewLinePart!=pEndLinePart)
				{	//不是终分段杆件
					pNewLinePart.LineTubePointer()->desEndPos.endPosType=2;	//点线投影
					pNewLinePart.LineTubePointer()->desEndPos.bDatumStartStartEnd=TRUE;
					pNewLinePart.LineTubePointer()->desEndPos.bDatumEndStartEnd=FALSE;
					pNewLinePart.LineTubePointer()->desEndPos.hDatumStartPart=pStartLinePart->handle;
					pNewLinePart.LineTubePointer()->desEndPos.hDatumEndPart=pEndLinePart->handle;
					SnapPerp(&pos,ber_line,pNewLinePart->pEnd->ActivePosition());
					pNewLinePart->SetEnd(pos);
				}
				else
				{
					pNewLinePart.LineTubePointer()->desEndPos=pLinePart.LineTubePointer()->desEndPos;
					pNewLinePart->SetEnd(pLinePart->End());
				}
				if(pNewLinePart!=pStartLinePart)
				{	//不是始分段杆件
					pNewLinePart.LineTubePointer()->desStartPos.endPosType=2;	//点线投影
					pNewLinePart.LineTubePointer()->desStartPos.bDatumStartStartEnd=TRUE;
					pNewLinePart.LineTubePointer()->desStartPos.bDatumEndStartEnd=FALSE;
					pNewLinePart.LineTubePointer()->desStartPos.hDatumStartPart=pStartLinePart->handle;
					pNewLinePart.LineTubePointer()->desStartPos.hDatumEndPart=pEndLinePart->handle;
					SnapPerp(&pos,ber_line,pNewLinePart->pStart->ActivePosition());
					pNewLinePart->SetStart(pos);
				}
				else
				{
					pNewLinePart.LineTubePointer()->desStartPos=pLinePart.LineTubePointer()->desStartPos;
					pNewLinePart->SetStart(pLinePart->Start());
				}
			}
			else if(pLinePart.LineTubePointer()->desStartPos.endPosType!=2&&pLinePart.LineTubePointer()->desEndPos.endPosType==2)
			{	//被打断的杆件起始点定位方式不是点线投影,终止点定位方式是点线投影
				if(pNewLinePart!=pStartLinePart)
				{	//不是始分段杆件
					pNewLinePart.LineTubePointer()->desStartPos=pLinePart.LineTubePointer()->desEndPos;
					pNewLinePart.LineTubePointer()->desStartPos.hDatumStartPart=pStartLinePart->handle;
				}
				else
					pNewLinePart.LineTubePointer()->desStartPos=pLinePart.LineTubePointer()->desStartPos;
				pNewLinePart->SetStart(pNewLinePart->pStart->ActivePosition());	
				pNewLinePart.LineTubePointer()->desEndPos=pLinePart.LineTubePointer()->desEndPos;
				pNewLinePart.LineTubePointer()->desEndPos.hDatumStartPart=pStartLinePart->handle;
				pNewLinePart->SetEnd(pNewLinePart->pEnd->ActivePosition());
			}
			else if(pLinePart.LineTubePointer()->desStartPos.endPosType==2&&pLinePart.LineTubePointer()->desEndPos.endPosType!=2)
			{	//被打断的杆件起始点定位方式是点线投影,终止点定位方式不是点线投影
				if(pNewLinePart!=pEndLinePart)
				{	//不是终分段杆件
					pNewLinePart.LineTubePointer()->desEndPos=pLinePart.LineTubePointer()->desStartPos;
					pNewLinePart.LineTubePointer()->desEndPos.hDatumEndPart=pEndLinePart->handle;
				}
				else
					pNewLinePart.LineTubePointer()->desEndPos=pLinePart.LineTubePointer()->desEndPos;
				pNewLinePart->SetEnd(pNewLinePart->pEnd->ActivePosition());	
				pNewLinePart.LineTubePointer()->desStartPos=pLinePart.LineTubePointer()->desStartPos;
				pNewLinePart.LineTubePointer()->desStartPos.hDatumEndPart=pEndLinePart->handle;
				pNewLinePart->SetStart(pNewLinePart->pStart->ActivePosition());
			}
			else if(pLinePart.LineTubePointer()->desStartPos.endPosType==2&&pLinePart.LineTubePointer()->desEndPos.endPosType==2)
			{	//被打断的杆件起始/终止点定位方式均是点线投影	打断点定位方式同被打断钢管的其诗/终止点定位方式均一致
				pNewLinePart.LineTubePointer()->desEndPos=pLinePart.LineTubePointer()->desEndPos;
				pNewLinePart->SetEnd(pNewLinePart->pEnd->ActivePosition());
				pNewLinePart.LineTubePointer()->desStartPos=pLinePart.LineTubePointer()->desStartPos;
				pNewLinePart->SetStart(pNewLinePart->pStart->ActivePosition());
			}
			pNewLinePart.LineTubePointer()->nearWorkPlaneNorm=pLinePart.LineTubePointer()->nearWorkPlaneNorm;
			pNewLinePart.LineTubePointer()->hWorkNormRefPart=pLinePart.LineTubePointer()->hWorkNormRefPart;
			if(pNewLinePart.LineTubePointer()->m_bHasWeldRoad)	//存在焊道线 wht 11-04-25
				pNewLinePart.LineTubePointer()->nearWeldRoadVec=pLinePart.LineTubePointer()->nearWeldRoadVec;
		}
#endif
	}
#ifndef __TSA_
	if(pStartLinePart->GetClassTypeId()==CLS_LINETUBE)
	{	//更新钢管端连接
		CLDSLineTube *pStartLineTube = (CLDSLineTube*)pStartLinePart;
		pStartLineTube->m_tJointStart = pLinePart.LineTubePointer()->m_tJointStart;
		CLDSLineTube *pEndLineTube = (CLDSLineTube*)pEndLinePart;
		pEndLineTube->m_tJointEnd = pLinePart.LineTubePointer()->m_tJointEnd;
		//更新钢管端连接插板或法兰的焊接父构件
		CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
		pStartParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pStartLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		pEndParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pEndLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pStartParamPlate)
			pStartParamPlate->m_hPartWeldParent=pStartLineTube->handle;
		if(pEndParamPlate)
			pEndParamPlate->m_hPartWeldParent=pEndLineTube->handle;
	}
	//查找当前杆件连接的钢板,更新钢板连接材列表  wht 10-02-06
	//EmendPlateDesignInfo函数已处理钢板的设计信息，此处不需要重复处理 wxc-2017.5.15
	//CLDSPlate *pPlate=NULL;
	//for(pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
	//{
	//	CDesignLjPartPara *pLjPartPara=pPlate->designInfo.FromHandle(pLinePart->handle);
	//	if(pLjPartPara)
	//	{
	//		if(pLjPartPara->start0_end1==0)
	//			pLjPartPara->hPart=pStartLinePart->handle;
	//		else if(pLjPartPara->start0_end1==1)
	//			pLjPartPara->hPart=pEndLinePart->handle;
	//		pPlate->SetModified();	//无此句可能导致undo错误
	//	}
	//}
	pStartLinePart->SetStartOdd(pLinePart->startOdd());
	pEndLinePart->SetEndOdd(pLinePart->endOdd());
	long oldHandleArr[4]={0};
	CLsRefList lsListArr[4];
	if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		for (int i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pLinePart.LineAnglePointer()->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle)
			{
				oldHandleArr[i]=pSonAngle->handle;
				for(CLsRef *pLsRef=pSonAngle->GetFirstLsRef();pLsRef;pLsRef=pSonAngle->GetNextLsRef())
					lsListArr[i].append(*pLsRef);
			}
		}
	}
#endif
	CHandleReplacer replacer(pLinePart->handle,0);
	int priorSerial=0;
	for(pNewLinePart=partset.GetFirst();pNewLinePart.IsHasPtr();pNewLinePart=partset.GetNext(),priorSerial++)
	{	//因角钢到位存在互相紧密的关连性,必须一起重置为参数化定位 wjh-2016.3.28
		if(pNewLinePart->IsAngle())	//打断杆件涉及替换构件句柄有一般会影响节点父杆件，进而可能导致角钢简单定位智能判断有误 wjh-2016.3.23
			pNewLinePart.LineAnglePointer()->desStartPos.jgber_cal_style=pNewLinePart.LineAnglePointer()->desEndPos.jgber_cal_style=0;
	}
	for(pNewLinePart=partset.GetFirst();pNewLinePart.IsHasPtr();pNewLinePart=partset.GetNext(),priorSerial++)
	{
		pNewLinePart->ClearFlag();
		pNewLinePart->CalPosition();
#ifndef __TSA_
		if(pNewLinePart->GetClassTypeId()==CLS_LINEANGLE||pNewLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			//是否启动特殊准距
			pNewLinePart.LineAnglePointer()->m_bEnableTeG=pLinePart.LineAnglePointer()->m_bEnableTeG;
			//复制特殊准距数据 wht 10-09-26
			pNewLinePart.LineAnglePointer()->xWingXZhunJu=pLinePart.LineAnglePointer()->xWingXZhunJu;
			pNewLinePart.LineAnglePointer()->xWingYZhunJu=pLinePart.LineAnglePointer()->xWingYZhunJu;
			//计算肢法线
			pNewLinePart.LineAnglePointer()->cal_x_wing_norm();
			pNewLinePart.LineAnglePointer()->cal_y_wing_norm();
			if(pNewLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{	//组合角钢
				int i=0;
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pNewLinePart.LineAnglePointer();
				if(pNewLinePart->pStart->handle==pLinePart.LineAnglePointer()->pStart->handle)
				{	//更新子角钢句柄
					for (i=0;i<4;i++)
						pGroupJg->son_jg_h[i]=pLinePart.LineAnglePointer()->son_jg_h[i];
				}
				else
					pGroupJg->AppendSonJg();
				pGroupJg->UpdateSonJgBasicInfo();
				pGroupJg->UpdateSonJgWingNorm();
				pGroupJg->UpdateSonJgPos();	
				//更新组合角钢子角钢的螺栓及正负头
				for (i=0;i<4;i++)
				{
					CLDSLineAngle *pSonAngle=(CLDSLineAngle*)Ta.Parts.FromHandle(pNewLinePart.LineAnglePointer()->son_jg_h[i],CLS_LINEANGLE);
					if(pSonAngle)
					{
						pSonAngle->SetStartOdd(pNewLinePart->startOdd());
						pSonAngle->SetEndOdd(pNewLinePart->endOdd());
						pSonAngle->EmptyLsRef();
						ReLayDivideAngleLs(&lsListArr[i],pSonAngle,oldHandleArr[i]);
					}
				}
			}
			else	//重新布置打断后角钢的螺栓
				ReLayDivideAngleLs(pLinePart.LineAnglePointer()->GetLsRefList(),pNewLinePart.LineAnglePointer(),pLinePart->handle);
		}
		else if(pNewLinePart->GetClassTypeId()==CLS_LINETUBE)
			pNewLinePart.LineTubePointer()->CalWorkPlaneNorm();
#endif
		replacer.AppendSectRod(pNewLinePart->pStart->Position(),pNewLinePart->pEnd->Position(),pNewLinePart->handle,priorSerial);
		NewLinePart(pNewLinePart);
		if(replacer.hNew==0)
			replacer.hNew=pNewLinePart->handle;
	}
	for(pNewLinePart=partset.GetFirst();pNewLinePart.IsHasPtr();pNewLinePart=partset.GetNext(),priorSerial++)
	{
		if(pNewLinePart->IsAngle()&&g_sysPara.IsPreferAngleSimplePosMode())
		{
			pNewLinePart.LineAnglePointer()->desStartPos.jgber_cal_style=pNewLinePart.LineAnglePointer()->desEndPos.jgber_cal_style=2;
			pNewLinePart.LineAnglePointer()->desStartPos.bUdfDatumPointMode=TRUE;
			pNewLinePart.LineAnglePointer()->desEndPos.bUdfDatumPointMode=TRUE;
		}
	}
#ifndef __TSA_
	if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
	{	//终止端钢管相贯
		long hEndNewPart = partset.GetTail()->handle;
		if(pLinePart.LineTubePointer()->m_tJointEnd.hViceLinkObj<0)
			replacer.hNew=hEndNewPart;
	}
	//修正钢板设计参数
	EmendPlateDesignInfo(partset,pLinePart);
#endif
	g_pSolidDraw->DelEnt(pLinePart->handle);
	console.DispPartSet.DeleteNode(pLinePart->handle);
	Ta.ReplaceReferenceHandle(replacer);
	for(pNewLinePart=partset.GetFirst();pNewLinePart.IsHasPtr()&&pNewLinePart->GetClassTypeId()==CLS_LINETUBE;
		pNewLinePart=partset.GetNext())
		pNewLinePart.LineTubePointer()->CalWorkPlaneNorm();
	console.DeletePart(pLinePart->handle);
	g_pSolidDraw->Draw();
	//修正打断后定位方式为指定线投影或指定线平推角钢的参照句柄 wht 12-06-11
	EmendRodPosParam(partset);
	return TRUE;
}


void CLDSView::OnDivideMainLinePart() 
{
	m_nPrevCommandID=ID_DIVIDE_MAIN_LINEPART;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Partition Leg Rods";
#else 
	m_sPrevCommandName="重复塔材分段";
#endif
	Command("DivideMainLinePart");
}

BOOL CLDSView::DivideMainLinePart() 
{	//塔材分段 暂时只支持轴对称
	CLDSLinePart *pMainLinePart=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLogErrorLife errlife;
	/*
	GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL),CLS_LINEPART);
	scr_part_para.disp_type=DISP_LINE;
	scr_part_para.cmdStr="请选择一根要进行分段的主材:";
	scr_part_para.cmdErrorFeedbackStr="没有选中合适的构件，请重新选择一根要进行分段的主材:";
	while(1)
	{
		if(!GetPartsFromScr(scr_part_para))
			return FALSE;
		pMainLinePart=(CLDSLinePart*)scr_part_para.pResultPartsArr[0];
		if(pMainLinePart==NULL)
			return FALSE;	
		if(pMainLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;
		break;
	}
	if(pMainLinePart==NULL||(pMainLinePart&&!pMainLinePart->IsLinePart()))
	{
		pCmdLine->CancelCmdLine();
		return FALSE;
	}
	*/
	g_pSolidDraw->ReleaseSnapStatus();
	//切换到单线显示状态
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select a rod to be segmented :","");
#else 
	pCmdLine->FillCmdLine("请选择一根要进行分段的杆件:","");
#endif
	while(1)
	{
		f3dLine line;
		int ret = g_pSolidSnap->SnapLine(line);
		if(ret<0)
		{
			pCmdLine->CancelCmdLine();
			return FALSE;
		}
		else if(ret>0)
		{
			CLDSPart *pPart=Ta.Parts.FromHandle(line.ID);
			if(pPart&&pPart->IsLinePart()&&pPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
				pMainLinePart = (CLDSLinePart*)pPart;
			if(pMainLinePart)
			{
				g_pSolidDraw->SetEntSnapStatus(line.ID);
				break;
			}
			else
			{
				pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
				pCmdLine->FillCmdLine("Select wrong part,Please select another rod to be segmented :","");
#else 
				pCmdLine->FillCmdLine("选择了错误的构件，请重新选择一根要进行分段的杆件:","");
#endif
				continue;
			}
		}
	}
	CNodeList node_list,mir_node_list;
	CLinePartDivideDlg dlg;//对话框, 提示输入垂高值.
	dlg.InitLinePart(pMainLinePart);
	if(dlg.DoModal()!=IDOK)
	{
		pCmdLine->CancelCmdLine();
		return FALSE;
	}
	if(dlg.m_nSegNum<=0)
	{
		pCmdLine->CancelCmdLine();
		return FALSE;
	}
	f3dLine line;
	line.startPt = pMainLinePart->pStart->Position(false);
	line.endPt = pMainLinePart->pEnd->Position(false);
	f3dPoint line_vec=line.endPt-line.startPt;
	double line_len=line_vec.mod();
	normalize(line_vec);
	f3dPoint start_pos,end_pos,cur_pos;
	if( (pMainLinePart->pStart->Position(false).x<pMainLinePart->pEnd->Position(false).x&&dlg.m_iRefAxis==0)||
		(pMainLinePart->pStart->Position(false).y<pMainLinePart->pEnd->Position(false).y&&dlg.m_iRefAxis==1)||
		(pMainLinePart->pStart->Position(false).z<pMainLinePart->pEnd->Position(false).z&&dlg.m_iRefAxis==2)||
		dlg.m_iRefAxis==3)
	{	//杆件始节点为分段起始点
		start_pos=pMainLinePart->pStart->Position(false);// 选中杆件起始节点的坐标
		end_pos=pMainLinePart->pEnd->Position(false);	// 选中杆件终止节点的坐标
	}
	else
	{	//杆件终节点为分段终节点
		start_pos=pMainLinePart->pEnd->Position(false);// 选中杆件起始节点的坐标
		end_pos=pMainLinePart->pStart->Position(false);	// 选中杆件终止节点的坐标
	}
	cur_pos=start_pos; //从此点开始累计计算. 
	// 斜率
	double facx=0,facy=0,facz=0;
	if(dlg.m_iRefAxis==0)		//以X轴为基准
	{
		facy=(end_pos.x-start_pos.x)/(end_pos.y-start_pos.y);
		facz=(end_pos.x-start_pos.x)/(end_pos.z-start_pos.z);
	}
	else if(dlg.m_iRefAxis==1)	//以Y轴为基准
	{
		facx=(end_pos.y-start_pos.y)/(end_pos.x-start_pos.x);
		facz=(end_pos.y-start_pos.y)/(end_pos.z-start_pos.z);
	}
	else if(dlg.m_iRefAxis==2)	//以Z轴为基准
	{
		facx=(end_pos.z-start_pos.z)/(end_pos.x-start_pos.x);
		facy=(end_pos.z-start_pos.z)/(end_pos.y-start_pos.y);
	}
	else						//以长度为基准
	{
		f3dPoint offset_vec = end_pos-start_pos;
		normalize(offset_vec);
		facx=offset_vec.x;
		facy=offset_vec.y;
		facz=offset_vec.z;
	}
	Ta.BeginUndoListen();
	for(int i=0;i<dlg.m_nSegNum;i++ ) // 主材分段数据N个, 将分为 N+1 段.
	{
		double fCurVerHigh = dlg.VertDistList[i];	//当前垂高
		if(dlg.m_iRefAxis==0)		//以X轴为基准
		{
			cur_pos.x += fCurVerHigh;
			cur_pos.y += fCurVerHigh/facy;
			cur_pos.z += fCurVerHigh/facz;
		}
		else if(dlg.m_iRefAxis==1)	//以Y轴为基准
		{
			cur_pos.x += fCurVerHigh/facx;
			cur_pos.y += fCurVerHigh;
			cur_pos.z += fCurVerHigh/facz;
		}
		else if(dlg.m_iRefAxis==2)	//以Z轴为基准
		{
			cur_pos.x += fCurVerHigh/facx;//Wjh X—Y面坐标展开转换
			cur_pos.y += fCurVerHigh/facy;//Wjh X—Y面坐标展开转换
			cur_pos.z += fCurVerHigh;
		}
		else						//以长度为基准
		{
			cur_pos.x += fCurVerHigh*facx;
			cur_pos.y += fCurVerHigh*facy;//Wjh X—Y面坐标展开转换
			cur_pos.z += fCurVerHigh*facz;
		}
		//生成新节点
		SORTEDNODE_ITEM item;
		item.pNode=console.AppendNode();
		item.pNode->CopyProperty(pMainLinePart->pStart);//从分段原始节点拷贝部分属性
		item.pNode->layer(2) = pMainLinePart->layer(2);	//节点图层位置与角钢一致
		item.pNode->cfgword = pMainLinePart->cfgword;	//节点应与原始杆件的配材号一致而不是始端节点 wjh-2016.6.21
		item.pNode->SetPosition(cur_pos);
		item.pNode->hFatherPart=pMainLinePart->handle;
		NewNode(item.pNode);
		if(dlg.m_iRefAxis<3)
		{
			item.pNode->m_cPosCalType = dlg.m_iRefAxis+1;//X/Y/Z坐标值不变点
			item.pNode->arrRelationNode[0] = pMainLinePart->pStart->handle;
			item.pNode->arrRelationNode[1] = pMainLinePart->pEnd->handle;
		}
		else
		{
			item.pNode->m_cPosCalType = 7;	//偏移点
			item.pNode->attach_offset=DISTANCE(cur_pos,start_pos);
			item.pNode->arrRelationNode[0] = pMainLinePart->pStart->handle; 
			item.pNode->arrRelationPole[0] = pMainLinePart->handle;
			item.pNode->arrRelationNode[1] = pMainLinePart->pStart->handle;
			item.pNode->arrRelationNode[2] = pMainLinePart->pEnd->handle;
		}
		if(item.pNode&&line.PtInLine(item.pNode->Position(false))==2)
		{
			f3dPoint vec=item.pNode->Position(false)-line.startPt;
			item.distToStart=vec*line_vec;
			if(item.distToStart>0&&item.distToStart<line_len)
			{	//使用插入排序的方法将选择杆件上的节点按距离始端的距离进行排序(升序)
				for(SORTEDNODE_ITEM *pItem=node_list.GetFirst();pItem;pItem=node_list.GetNext())
				{
					if(pItem->distToStart>item.distToStart)
					{
						node_list.insert(item);
						break;
					}
					else if(pItem->distToStart==item.distToStart)
						break;	//重复节点略去不管
				}
				if(pItem==NULL)
					node_list.append(item);
			}
		}
	}
	MIRMSG mir_msg;
	int flag_arr[4]={0,0,0,0};
	if(dlg.m_nMirMsg==1||dlg.m_nMirMsg==4)
		flag_arr[1]=1;
	if(dlg.m_nMirMsg==2||dlg.m_nMirMsg==4)
		flag_arr[2]=2;
	if(dlg.m_nMirMsg==3||dlg.m_nMirMsg==4)
		flag_arr[3]=4;
	LINEPARTSET partset[4];
	for(i=1;i<4;i++)
	{
		partset[i].Empty();
		mir_msg.axis_flag=flag_arr[i];
		if(mir_msg.axis_flag==0)
			continue;
		CLDSLinePart *pLinePart=(CLDSLinePart*)pMainLinePart->GetMirPart(mir_msg);
		if(pLinePart==NULL||(pLinePart&&pLinePart->GetClassTypeId()!=pMainLinePart->GetClassTypeId()))
			continue;
		mir_node_list.Empty();	//清空对称节点列表
		for(SORTEDNODE_ITEM *pItem=node_list.GetFirst();pItem;pItem=node_list.GetNext())
		{
			SORTEDNODE_ITEM item;
			item.pNode=console.AppendNode();
			item.pNode->CopyProperty(pLinePart->pStart);//从分段原始节点拷贝部分属性
			item.pNode->layer(2) = pLinePart->layer(2);	//节点图层位置与角钢一致
			item.pNode->cfgword = pLinePart->cfgword;
			item.pNode->SetPosition(GetMirPos(pItem->pNode->Position(false),mir_msg));
			item.pNode->hFatherPart=pLinePart->handle;
			NewNode(item.pNode);
			//添加对称关联信息
			RELATIVE_OBJECT *pMirObj=pItem->pNode->relativeObjs.Add(item.pNode->handle);
			pMirObj->mirInfo=mir_msg;
			if(dlg.m_iRefAxis<3)
			{
				item.pNode->m_cPosCalType = dlg.m_iRefAxis+1;//X/Y/Z坐标值不变点
				item.pNode->arrRelationNode[0] = pLinePart->pStart->handle;
				item.pNode->arrRelationNode[1] = pLinePart->pEnd->handle;
			}
			else
			{
				item.pNode->m_cPosCalType = 7;	//偏移点
				item.pNode->attach_offset=pItem->pNode->attach_offset;
				item.pNode->arrRelationNode[0] = pLinePart->pStart->handle; 
				item.pNode->arrRelationPole[0] = pLinePart->handle;
				item.pNode->arrRelationNode[1] = pLinePart->pStart->handle;
				item.pNode->arrRelationNode[2] = pLinePart->pEnd->handle;
			}
			mir_node_list.append(item);
		}
		FinishBreakLinePart(pLinePart,&mir_node_list,partset[i]);
	}
	//最后再将所选杆件分段,前边要用到
	partset[0].Empty();
	FinishBreakLinePart(pMainLinePart,&node_list,partset[0]);
	//添加对称节点相互之间的关联信息
	for(SORTEDNODE_ITEM *pItem=node_list.GetFirst();pItem;pItem=node_list.GetNext())
	{	
		if(pItem->pNode==NULL)
			continue;
		SetRelativeObj(pItem->pNode);
	}
	//添加对称杆件相互之间的关联信息
	for(i=0;i<4;i++)
	{
		for(CLDSLinePart *pLinePart=partset[i].GetFirst();pLinePart;pLinePart=partset[i].GetNext())
		{
			for(int j=1; j<4; j++)
			{
				mir_msg.axis_flag=flag_arr[j];
				if(mir_msg.axis_flag==0)
					continue;
				CLDSLinePart *pMirLinePart=(CLDSLinePart*)pLinePart->GetMirPart(mir_msg);
				if(pMirLinePart==NULL||(pLinePart&&pMirLinePart->GetClassTypeId()!=pLinePart->GetClassTypeId()))
					continue;
				RELATIVE_OBJECT *pMirObj=pLinePart->relativeObjs.Add(pMirLinePart->handle);
				pMirObj->mirInfo=mir_msg;
			}
		}
	}
	m_pDoc->SetModifiedFlag();
	Ta.EndUndoListen();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Command:","");
#else 
	pCmdLine->FillCmdLine("命令:","");
#endif
	Command("DivideMainLinePart");
	return TRUE;
}

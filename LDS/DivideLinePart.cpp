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
//���²��ñ��ضϽǸ��ϵ���˨
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
				//pOrgJg->DeleteLsRef((*pLsRef)->handle);����������������ݾ���ɾ��ʱ,�޷�ɾ���������һ��
				pLsRefList->DeleteCursor();	//���δɾ��״̬�����ͳһɾ��
			}
		}
	}
	//ɾ�������б��Ϊɾ��״̬�Ľڵ� wht 10-03-17
	pLsRefList->Clean();
}
#endif
void CLDSView::OnBreakLinePart() 
{
	m_nPrevCommandID=ID_BREAK_LINEPART;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Break Rod";
#else 
	m_sPrevCommandName="�ظ���ϸ˼�";
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
	//��ϸ˼�
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CString cmdStr,errCmdStr;
	CSuperSmartPtr<CLDSLinePart> pLinePart;
	g_pSolidDraw->ReleaseSnapStatus();
#ifdef AFX_TARG_ENU_ENGLISH
	cmdStr.Format("BREAK Rod Divide Specify the rod needs divide:");
	errCmdStr.Format("BREAK Without selecting a suitable rod,Please select a section rod:");
#else
	cmdStr.Format("BREAK ָ��Ҫ�ֶεĸ˼�:");
	errCmdStr.Format("BREAK û��ѡ�к��ʵĸ˼�����ѡ��һ�����ֶεĸ˼�:");
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
			{	//����ضϲ�������ȫȨ�޵ĸ˼�,������ظ��˼�
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
	cmdStr.Format("BREAK ָ����ѡ�˼��ϵķֶνڵ�:");
#endif
	pCmdLine->FillCmdLine(cmdStr,"");
	verifier.ClearSnapFlag();
	verifier.SetVerifyFlag(OBJPROVIDER::SOLIDSPACE,GetSingleWord(SELECTINDEX_NODE));
	verifier.AddVerifyFlag(OBJPROVIDER::LINESPACE,SNAP_POINT);
	{//�����ſ���CDisplayNodeAtFrontLife����������
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
			{	//ʹ�ò�������ķ�����ѡ��˼��ϵĽڵ㰴����ʼ�˵ľ����������(����)
				for(SORTEDNODE_ITEM *pItem=nodeListArr[0].GetFirst();pItem;pItem=nodeListArr[0].GetNext())
				{
					if(pItem->distToStart>item.distToStart)
					{
						nodeListArr[0].insert(item);
						break;
					}
					else if(pItem->distToStart==item.distToStart)
						break;	//�ظ��ڵ���ȥ����
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
				//���ҶԳƽڵ�
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
				{	//δ�ҵ�ȫ���Գƽڵ� ��սڵ����� ������ʱ�ж��Ƿ�Ϸ�
					nodeListArr[i].Empty();
					continue;
				}
			}
		}
		//������ת�Գ�
		MIRMSG mirmsg=mir_dlg.mirmsg;
		int array_num=mirmsg.array_num;
		if((mirmsg.axis_flag&8)==0)	//����ת�Գ�
			array_num=0;
		else
			mirmsg.axis_flag=8;
		short init_angle=mirmsg.rotate_angle;
		int index=4;
		for(int i=0;i<4;i++)
		{	//��ת�Գ�
			if(LinePartPtrArr[i]==NULL)
				continue;
			CLDSLinePart *pSrcLineAngle = LinePartPtrArr[i];
			for(int j=0;j<array_num;j++)
			{	//���ж����ת�Գ�ʱ��׼����Ϊͬһ��������ֻ��Ҫ������ת�Ƕȼ���
				if(pSrcLineAngle==NULL)
					continue;
				//������ת�Ƕ�
				mirmsg.rotate_angle=init_angle*(j+1);
				CLDSLinePart *pMirLinePart=(CLDSLinePart*)pSrcLineAngle->GetMirPart(mirmsg);
				if(pMirLinePart==NULL)
					continue;
				//���ҶԳƽڵ�
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
				{	//δ�ҵ�ȫ���Գƽڵ� ��սڵ����� ������ʱ�ж��Ƿ�Ϸ�
					nodeListArr[index].Empty();
					continue;
				}
				index++;
			}
		}
		//��ϸ˼�
		LINEPARTSET linePartSet;
		for (i=0;i<8;i++)
		{
			if(LinePartPtrArr[i]==NULL || nodeListArr[i].GetNodeNum()<=0)
				continue;
			FinishBreakLinePart(LinePartPtrArr[i],&nodeListArr[i],partset[i]);
			for(CLDSLinePart* pRod=partset[i].GetFirst();pRod;pRod=partset[i].GetNext())
				linePartSet.append(pRod);
		}
		//�Դ�ϵĸ˼���ӹ�������
		for(CLDSLinePart* pRod=linePartSet.GetFirst();pRod;pRod=linePartSet.GetNext())
			FinishRetrieveRelaMirObject(pRod);
	}
	RedrawAll();
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	Ta.EndUndoGroup();
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	BreakLinePart();//pCmdLine->FillCmdLine("����:","");
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
	//�л���������ʾ״̬
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
	pCmdLine->FillCmdLine("BREAK �ֹܸ˷ֶ� ָ��Ҫ�ֶεĸֹܸ�:","");
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
			{	//����ضϲ�������ȫȨ�޵ĸ˼�,������ظ��˼�
				if(!theApp.m_bCooperativeWork||theApp.IsHasModifyPerm(pLinePart->dwStartPermission&pLinePart->dwEndPermission&pLinePart->dwPermission))
					break;
			}
		}
		g_pSolidDraw->ReleaseSnapStatus();
		pCmdLine->FillCmdLine("û��ѡ�к��ʵĸ˼���","");
		pCmdLine->FinishCmdLine();
		pCmdLine->FillCmdLine("��ѡ��һ�����ֶεĸ˼�:","");
	}
	pCmdLine->FinishCmdLine();
	pCmdLine->FillCmdLine("ָ����ѡ�˼��ϵķֶνڵ�:","");
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
				{	//ʹ�ò�������ķ�����ѡ��˼��ϵĽڵ㰴����ʼ�˵ľ����������(����)
					for(SORTEDNODE_ITEM *pItem=nodeListArr.GetFirst();pItem;pItem=nodeListArr.GetNext())
					{
						if(pItem->distToStart>item.distToStart)
						{
							nodeListArr.insert(item);
							break;
						}
						else if(pItem->distToStart==item.distToStart)
							break;	//�ظ��ڵ���ȥ����
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
	m_pDoc->SetModifiedFlag();  // �޸����ݺ�Ӧ���ô˺������޸ı�־.
	Ta.EndUndoListen();
	g_pSolidSnap->SetSnapType(SNAP_ALL);
	BreakLinePart();//pCmdLine->FillCmdLine("����:","");
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
	//���һ�θ˼�
	CLDSConeTube *pEndLinePart=(CLDSConeTube*)console.AppendPart(pConeTube->GetClassTypeId());
	pEndLinePart->pStart=pPrevNode;
	pEndLinePart->pEnd=pConeTube->pEnd;
	pEndLinePart->CopyProperty(*pConeTube);
	pEndLinePart->longth = pEndLinePart->pEnd->Position(false).z - pEndLinePart->pStart->Position(false).z;
	pEndLinePart->size_wide = pConeTube->size_wide+pNodeList->GetNodeNum()*fDispersion;
	partset.append(pEndLinePart);
	f3dPoint pos;
	f3dLine ber_line(pConeTube->Start(),pConeTube->End());
	//���ø��ֶθ˼�������λ����Ʋ�����֫/�������ߵļ������
	for(pNewConeTube=partset.GetFirst();pNewConeTube;pNewConeTube=partset.GetNext())
	{
#ifndef __TSA_
		if(pConeTube->desEndPos.endPosType!=2&&pConeTube->desStartPos.endPosType!=2)
		{	//����ϵĸ˼���ʼ/��ֹ�㶨λ��ʽ�����ǵ���ͶӰ
			if(pNewConeTube!=pEndLinePart)
			{	//�����շֶθ˼�
				pNewConeTube->desEndPos.endPosType=2;	//����ͶӰ
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
			{	//����ʼ�ֶθ˼�
				pNewConeTube->desStartPos.endPosType=2;	//����ͶӰ
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
		{	//����ϵĸ˼���ʼ�㶨λ��ʽ���ǵ���ͶӰ,��ֹ�㶨λ��ʽ�ǵ���ͶӰ
			if(pNewConeTube!=pStartLinePart)
			{	//����ʼ�ֶθ˼�
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
		{	//����ϵĸ˼���ʼ�㶨λ��ʽ�ǵ���ͶӰ,��ֹ�㶨λ��ʽ���ǵ���ͶӰ
			if(pNewConeTube!=pEndLinePart)
			{	//�����շֶθ˼�
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
		{	//����ϵĸ˼���ʼ/��ֹ�㶨λ��ʽ���ǵ���ͶӰ	��ϵ㶨λ��ʽͬ����ϸֹܵ���ʫ/��ֹ�㶨λ��ʽ��һ��
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
	//���ҵ�ǰ�˼����ӵĸְ�,���¸ְ����Ӳ��б�  wht 10-02-06
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
			//���½���ڵ������˼�	wht 09-09-04
			if(pNode->m_cPosCalType==4)	//�����
			{
				if(pNode->arrRelationPole[0]==pConeTube->handle
					&&segLine.PtInLine(pNode->Position(false))>0)	//���߶���
				{
					pNode->arrRelationPole[0]=pNewConeTube->handle;
					//���������˼���ͬʱ��Ҫ���������ڵ�
					pNode->arrRelationNode[0]=pNewConeTube->pStart->handle;
					pNode->arrRelationNode[1]=pNewConeTube->pEnd->handle;
				}
				if(pNode->arrRelationPole[1]==pConeTube->handle
					&&segLine.PtInLine(pNode->Position(false))>0)	//���߶���
				{
					pNode->arrRelationPole[1]=pNewConeTube->handle;
					//���������˼���ͬʱ��Ҫ���������ڵ�
					pNode->arrRelationNode[2]=pNewConeTube->pStart->handle;
					pNode->arrRelationNode[3]=pNewConeTube->pEnd->handle;
				}
			}
			//���½ڵ㸸�˼�
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
//������Ϻ�λ��ʽΪ��ָ����ͶӰ��ָ����ƽ�ơ��ǸֵĲ��վ��
static void EmendRodPosParam(LINEPARTSET &partSet)
{
	if(partSet.GetNodeNum()<2)
		return;
	CLDSLinePart *pRod=NULL;
	
	CLDSLinePart *pUpRod=partSet.GetFirst(),*pDownRod=partSet.GetTail();
	if(pUpRod->IsTube())
		return;	//�ֹܶ�λ��ʽ����Ҫ����
	BOOL bUpReversal=FALSE,bDownReversal=FALSE;	//�Ƿ���Ҫ�ߵ�ʼ�ն�
	f3dPoint rod_vec=pUpRod->pEnd->Position(false)-pUpRod->pStart->Position(false);
	normalize(rod_vec);
	//1.���ϲ��ҹ��߸˼�
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
				continue;	//��ֹ˫�Ǹֱ䵥�Ǹ�ʱ���ڵ��߹��ߵ������ж�Ϊ������� wjh-2015.2.13
			else if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0)
				continue;	//�S����ϽǸֵ��ӽǸ�
			f3dPoint vec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
			normalize(vec);
			double dd=vec*rod_vec;
			if(fabs(vec*rod_vec)<EPS_COS)
				continue;	//������
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
	//2.���²��ҹ��߸˼�
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
				continue;	//��ֹ˫�Ǹֱ䵥�Ǹ�ʱ���ڵ��߹��ߵ������ж�Ϊ������� wjh-2015.2.13
			else if(pRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pRod)->group_father_jg_h>0)
				continue;	//�S����ϽǸֵ��ӽǸ�
			f3dPoint vec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
			normalize(vec);
			double dd=vec*rod_vec;
			if(fabs(vec*rod_vec)<EPS_COS)
				continue;	//������
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
	//3.������Ϻ�˼���λ��ʽΪ��ָ����ͶӰ����ָ����ƽ�ơ��˼��Ĳ��ո˼����
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
			{	//3.1ʼ�˶�λ��ʽΪָ����ƽ�ƻ�ͶӰ
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
					pLineAngle->desStartPos.origin_pos_style=5;	//��ʼ���Ϊ����ʶ�����
					pLineAngle->ClearFlag();
				}
			}
			if(pLineAngle->desEndPos.jgber_cal_style!=1&&(pLineAngle->desEndPos.spatialOperationStyle==16||pLineAngle->desEndPos.spatialOperationStyle==17))
			{	//3.2�ն˶�λ��ʽΪָ����ƽ�ƻ�ͶӰ
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
					pLineAngle->desEndPos.origin_pos_style=5;	//��ʼ���Ϊ����ʶ�����
					pLineAngle->ClearFlag();
				}
			}
		}
#endif
	}
}

//�����ְ���Ʋ���
static void EmendPlateDesignInfo(LINEPARTSET &partset,CLDSLinePart *pRod)
{
	CLDSLinePart *pStartRod=partset.GetFirst();
	CLDSLinePart *pEndRod=partset.GetTail();
	f3dLine datumLine(pRod->pStart->Position(false),pRod->pEnd->Position(false));
	CLDSModule* pModule=Ta.GetActiveModule();
	for(CLDSPlate *pPlate=(CLDSPlate*)console.EnumPartFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)console.EnumPartNext(CLS_PLATE))
	{
		if(!pModule->IsSonPart(pPlate))		//�����ڵ�ǰ�������
			continue;
		CLDSNode *pDatumNode=console.FromNodeHandle(pPlate->designInfo.m_hBaseNode);
		if(pDatumNode==NULL)
			continue;
		CLDSLinePart *pDestRod=NULL;
		int result=datumLine.PtInLine(pDatumNode->Position(false));
		if(pDatumNode==pRod->pStart||result==-2)
			pDestRod=pStartRod;	//��׼�ڵ�Ϊ���Ǽ�ʼ�˽ڵ�
		else if(pDatumNode==pRod->pEnd||result==-1)
			pDestRod=pEndRod;	//��׼�ڵ�Ϊ���Ǽ��ն˽ڵ�
		else if(result==2)
		{	//��׼�ڵ��ڸ˼��߶���
			for(CLDSLinePart *pNewLinePart=partset.GetFirst();pNewLinePart;pNewLinePart=partset.GetNext())
			{
				f3dLine segLine(pNewLinePart->pStart->Position(false),pNewLinePart->pEnd->Position(false));
				if(pDatumNode==pNewLinePart->pStart||pDatumNode==pNewLinePart->pEnd
					||segLine.PtInLine(pDatumNode->Position(false))==2)
				{	//��׼�ڵ�ʼ�ն˽ڵ���׼�ڵ��ڵ�ǰ�˼��߶���
					pDestRod=pNewLinePart;
					break;
				}
			}
		}
		else if(result==1)
			pDestRod=pRod;
		else	//��׼�㲻������
			continue;
		if(pDestRod==NULL)
			continue;
		if(pDestRod!=pRod)
			pPlate->designInfo.ReplaceReferenceHandle(CHandleReplacer(pRod->handle,pDestRod->handle));
		CDesignLjPartPara *pLjPara;
		//�����׼�ڵ㲻�ڴ�ϸ˼��������߸˼��Ǵ�ϸ˼������
		for(pLjPara=pPlate->GetFirstLjPara();pLjPara;pLjPara=pPlate->GetNextLjPara())
		{
			if(pLjPara->hPart!=pRod->handle)
				continue;
			//���ݸְ����ƽڵ��ҵ�Ŀ��˼�,Ȼ�����Ŀ��˼��������Ӹ˼�����ƶ�
			//����, �ᵣ�����Z��ԳƵ������˼������X/Y��Գƴ��ʱ���ܻ����  wxc-2017.5.16
			pLjPara->hPart=pDestRod->handle;
			if(pLjPara->start0_end1==0&&pLjPara->hPart!=pStartRod->handle)
				pLjPara->start0_end1=1;
			else if(pLjPara->start0_end1==1&&pLjPara->hPart!=pEndRod->handle)
				pLjPara->start0_end1=0;
			break;
		}
		//���¸ְ����߸˼�����ͷ��Ʋ���
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
	//���һ�θ˼�
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
	GEPOINT face_offset_norm;	//��ɸѡ���ܵ���ƽ�����뵱ǰ�˼�����һ��,������ƽ�ƺ���м������ wjh-2016.4.13
	if(pLinePart->IsAngle()&&fabs(lenStdVec*pLinePart.LineAnglePointer()->desStartPos.face_offset_norm)>0.5)
		face_offset_norm=pLinePart.LineAnglePointer()->desStartPos.face_offset_norm;
	//���ø��ֶθ˼�������λ����Ʋ�����֫/�������ߵļ������
	for(pNewLinePart=partset.GetFirst();pNewLinePart.IsHasPtr();pNewLinePart=partset.GetNext())
	{
		if(pLinePart->GetClassTypeId()==CLS_LINEPART)
		{	//����ϵĸ˼���ʼ/��ֹ�㶨λ��ʽ���ǵ���ͶӰ	��ϵ㶨λ��ʽͬ����ϸֹܵ���ʼ/��ֹ�㶨λ��ʽ��һ��
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
			{	//�����շֶθ˼�
				pNewLinePart.LineAnglePointer()->desEndPos.jgber_cal_style=0;	//��������ͶӰ
				pNewLinePart.LineAnglePointer()->desEndPos.origin_pos_style=0;
				//pNewLinePart.LineAnglePointer()->desEndPos.spatialOperationStyle=1;
				BOOL bPush=partset.push_stack();
				CLDSLinePart *pNextLinePart=partset.GetNext();
				//��ǰ�˼����ն˽ڵ�ĸ��˼�������һ���˼�����Ҫָ������ͶӰ�Ļ�׼�Ǹ� wht 11-01-19
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
					pLinePart.LineAnglePointer()->des_end_pos.jgber_cal_style==2) //ƽ��
				{
					pNewLinePart.LineAnglePointer()->desEndPos.jgber_cal_style=2;	//����ƽ��
					pNewLinePart.LineAnglePointer()->desEndPos.datum_to_ber_style=1;	//ƽ��
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
			{	//�ն˷ֶθ˼��̳�ԭ�˼��ն������Ϣ(����ԭ���̳�����ͷ���н���Ϣ����) wjh-2011.6.26
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
			{	//����ʼ�ֶθ˼�
				pNewLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle=16;	//����ͶӰ
				if(pLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle==16||pLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle==17)
				{	//ԭ�˼���������ڵ���ͶӰ��ƽ�Ƶ�ֱ�߶��е�һ���֣���ʱӦ�̳� wjh-2011.8.21
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
					pLinePart.LineAnglePointer()->desStartPos.jgber_cal_style==2) //ƽ��*/
				if(pLinePart.LineAnglePointer()->desStartPos.IsFaceOffset())
				{	//����ƽ������Ϣ
					pNewLinePart.LineAnglePointer()->desStartPos.jgber_cal_style=0;	//����ƽ��
					pNewLinePart.LineAnglePointer()->desStartPos.spatialOperationStyle=17;	//ƽ��
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
			{	//ʼ�˷ֶθ˼��̳�ԭ�˼�ʼ�������Ϣ(����ԭ���̳�����ͷ���н���Ϣ����) wjh-2011.6.26
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
			{	//����ϵĸ˼���ʼ/��ֹ�㶨λ��ʽ�����ǵ���ͶӰ
				if(pNewLinePart!=pEndLinePart)
				{	//�����շֶθ˼�
					pNewLinePart.LineTubePointer()->desEndPos.endPosType=2;	//����ͶӰ
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
				{	//����ʼ�ֶθ˼�
					pNewLinePart.LineTubePointer()->desStartPos.endPosType=2;	//����ͶӰ
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
			{	//����ϵĸ˼���ʼ�㶨λ��ʽ���ǵ���ͶӰ,��ֹ�㶨λ��ʽ�ǵ���ͶӰ
				if(pNewLinePart!=pStartLinePart)
				{	//����ʼ�ֶθ˼�
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
			{	//����ϵĸ˼���ʼ�㶨λ��ʽ�ǵ���ͶӰ,��ֹ�㶨λ��ʽ���ǵ���ͶӰ
				if(pNewLinePart!=pEndLinePart)
				{	//�����շֶθ˼�
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
			{	//����ϵĸ˼���ʼ/��ֹ�㶨λ��ʽ���ǵ���ͶӰ	��ϵ㶨λ��ʽͬ����ϸֹܵ���ʫ/��ֹ�㶨λ��ʽ��һ��
				pNewLinePart.LineTubePointer()->desEndPos=pLinePart.LineTubePointer()->desEndPos;
				pNewLinePart->SetEnd(pNewLinePart->pEnd->ActivePosition());
				pNewLinePart.LineTubePointer()->desStartPos=pLinePart.LineTubePointer()->desStartPos;
				pNewLinePart->SetStart(pNewLinePart->pStart->ActivePosition());
			}
			pNewLinePart.LineTubePointer()->nearWorkPlaneNorm=pLinePart.LineTubePointer()->nearWorkPlaneNorm;
			pNewLinePart.LineTubePointer()->hWorkNormRefPart=pLinePart.LineTubePointer()->hWorkNormRefPart;
			if(pNewLinePart.LineTubePointer()->m_bHasWeldRoad)	//���ں����� wht 11-04-25
				pNewLinePart.LineTubePointer()->nearWeldRoadVec=pLinePart.LineTubePointer()->nearWeldRoadVec;
		}
#endif
	}
#ifndef __TSA_
	if(pStartLinePart->GetClassTypeId()==CLS_LINETUBE)
	{	//���¸ֹܶ�����
		CLDSLineTube *pStartLineTube = (CLDSLineTube*)pStartLinePart;
		pStartLineTube->m_tJointStart = pLinePart.LineTubePointer()->m_tJointStart;
		CLDSLineTube *pEndLineTube = (CLDSLineTube*)pEndLinePart;
		pEndLineTube->m_tJointEnd = pLinePart.LineTubePointer()->m_tJointEnd;
		//���¸ֹܶ����Ӳ������ĺ��Ӹ�����
		CLDSParamPlate *pStartParamPlate=NULL,*pEndParamPlate=NULL;
		pStartParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pStartLineTube->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		pEndParamPlate=(CLDSParamPlate*)Ta.Parts.FromHandle(pEndLineTube->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pStartParamPlate)
			pStartParamPlate->m_hPartWeldParent=pStartLineTube->handle;
		if(pEndParamPlate)
			pEndParamPlate->m_hPartWeldParent=pEndLineTube->handle;
	}
	//���ҵ�ǰ�˼����ӵĸְ�,���¸ְ����Ӳ��б�  wht 10-02-06
	//EmendPlateDesignInfo�����Ѵ���ְ�������Ϣ���˴�����Ҫ�ظ����� wxc-2017.5.15
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
	//		pPlate->SetModified();	//�޴˾���ܵ���undo����
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
	{	//��Ǹֵ�λ���ڻ�����ܵĹ�����,����һ������Ϊ��������λ wjh-2016.3.28
		if(pNewLinePart->IsAngle())	//��ϸ˼��漰�滻���������һ���Ӱ��ڵ㸸�˼����������ܵ��½Ǹּ򵥶�λ�����ж����� wjh-2016.3.23
			pNewLinePart.LineAnglePointer()->desStartPos.jgber_cal_style=pNewLinePart.LineAnglePointer()->desEndPos.jgber_cal_style=0;
	}
	for(pNewLinePart=partset.GetFirst();pNewLinePart.IsHasPtr();pNewLinePart=partset.GetNext(),priorSerial++)
	{
		pNewLinePart->ClearFlag();
		pNewLinePart->CalPosition();
#ifndef __TSA_
		if(pNewLinePart->GetClassTypeId()==CLS_LINEANGLE||pNewLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			//�Ƿ���������׼��
			pNewLinePart.LineAnglePointer()->m_bEnableTeG=pLinePart.LineAnglePointer()->m_bEnableTeG;
			//��������׼������ wht 10-09-26
			pNewLinePart.LineAnglePointer()->xWingXZhunJu=pLinePart.LineAnglePointer()->xWingXZhunJu;
			pNewLinePart.LineAnglePointer()->xWingYZhunJu=pLinePart.LineAnglePointer()->xWingYZhunJu;
			//����֫����
			pNewLinePart.LineAnglePointer()->cal_x_wing_norm();
			pNewLinePart.LineAnglePointer()->cal_y_wing_norm();
			if(pNewLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{	//��ϽǸ�
				int i=0;
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pNewLinePart.LineAnglePointer();
				if(pNewLinePart->pStart->handle==pLinePart.LineAnglePointer()->pStart->handle)
				{	//�����ӽǸ־��
					for (i=0;i<4;i++)
						pGroupJg->son_jg_h[i]=pLinePart.LineAnglePointer()->son_jg_h[i];
				}
				else
					pGroupJg->AppendSonJg();
				pGroupJg->UpdateSonJgBasicInfo();
				pGroupJg->UpdateSonJgWingNorm();
				pGroupJg->UpdateSonJgPos();	
				//������ϽǸ��ӽǸֵ���˨������ͷ
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
			else	//���²��ô�Ϻ�Ǹֵ���˨
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
	{	//��ֹ�˸ֹ����
		long hEndNewPart = partset.GetTail()->handle;
		if(pLinePart.LineTubePointer()->m_tJointEnd.hViceLinkObj<0)
			replacer.hNew=hEndNewPart;
	}
	//�����ְ���Ʋ���
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
	//������Ϻ�λ��ʽΪָ����ͶӰ��ָ����ƽ�ƽǸֵĲ��վ�� wht 12-06-11
	EmendRodPosParam(partset);
	return TRUE;
}


void CLDSView::OnDivideMainLinePart() 
{
	m_nPrevCommandID=ID_DIVIDE_MAIN_LINEPART;
#ifdef AFX_TARG_ENU_ENGLISH
	m_sPrevCommandName="Repeat Partition Leg Rods";
#else 
	m_sPrevCommandName="�ظ����ķֶ�";
#endif
	Command("DivideMainLinePart");
}

BOOL CLDSView::DivideMainLinePart() 
{	//���ķֶ� ��ʱֻ֧����Գ�
	CLDSLinePart *pMainLinePart=NULL;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLogErrorLife errlife;
	/*
	GET_SCR_PART_PARA scr_part_para(GetSingleWord(SELECT_ALL),CLS_LINEPART);
	scr_part_para.disp_type=DISP_LINE;
	scr_part_para.cmdStr="��ѡ��һ��Ҫ���зֶε�����:";
	scr_part_para.cmdErrorFeedbackStr="û��ѡ�к��ʵĹ�����������ѡ��һ��Ҫ���зֶε�����:";
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
	//�л���������ʾ״̬
	g_pSolidSet->SetDisplayType(DISP_LINE);
	g_pSolidDraw->Draw();
#ifdef AFX_TARG_ENU_ENGLISH
	pCmdLine->FillCmdLine("Please select a rod to be segmented :","");
#else 
	pCmdLine->FillCmdLine("��ѡ��һ��Ҫ���зֶεĸ˼�:","");
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
				pCmdLine->FillCmdLine("ѡ���˴���Ĺ�����������ѡ��һ��Ҫ���зֶεĸ˼�:","");
#endif
				continue;
			}
		}
	}
	CNodeList node_list,mir_node_list;
	CLinePartDivideDlg dlg;//�Ի���, ��ʾ���봹��ֵ.
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
	{	//�˼�ʼ�ڵ�Ϊ�ֶ���ʼ��
		start_pos=pMainLinePart->pStart->Position(false);// ѡ�и˼���ʼ�ڵ������
		end_pos=pMainLinePart->pEnd->Position(false);	// ѡ�и˼���ֹ�ڵ������
	}
	else
	{	//�˼��սڵ�Ϊ�ֶ��սڵ�
		start_pos=pMainLinePart->pEnd->Position(false);// ѡ�и˼���ʼ�ڵ������
		end_pos=pMainLinePart->pStart->Position(false);	// ѡ�и˼���ֹ�ڵ������
	}
	cur_pos=start_pos; //�Ӵ˵㿪ʼ�ۼƼ���. 
	// б��
	double facx=0,facy=0,facz=0;
	if(dlg.m_iRefAxis==0)		//��X��Ϊ��׼
	{
		facy=(end_pos.x-start_pos.x)/(end_pos.y-start_pos.y);
		facz=(end_pos.x-start_pos.x)/(end_pos.z-start_pos.z);
	}
	else if(dlg.m_iRefAxis==1)	//��Y��Ϊ��׼
	{
		facx=(end_pos.y-start_pos.y)/(end_pos.x-start_pos.x);
		facz=(end_pos.y-start_pos.y)/(end_pos.z-start_pos.z);
	}
	else if(dlg.m_iRefAxis==2)	//��Z��Ϊ��׼
	{
		facx=(end_pos.z-start_pos.z)/(end_pos.x-start_pos.x);
		facy=(end_pos.z-start_pos.z)/(end_pos.y-start_pos.y);
	}
	else						//�Գ���Ϊ��׼
	{
		f3dPoint offset_vec = end_pos-start_pos;
		normalize(offset_vec);
		facx=offset_vec.x;
		facy=offset_vec.y;
		facz=offset_vec.z;
	}
	Ta.BeginUndoListen();
	for(int i=0;i<dlg.m_nSegNum;i++ ) // ���ķֶ�����N��, ����Ϊ N+1 ��.
	{
		double fCurVerHigh = dlg.VertDistList[i];	//��ǰ����
		if(dlg.m_iRefAxis==0)		//��X��Ϊ��׼
		{
			cur_pos.x += fCurVerHigh;
			cur_pos.y += fCurVerHigh/facy;
			cur_pos.z += fCurVerHigh/facz;
		}
		else if(dlg.m_iRefAxis==1)	//��Y��Ϊ��׼
		{
			cur_pos.x += fCurVerHigh/facx;
			cur_pos.y += fCurVerHigh;
			cur_pos.z += fCurVerHigh/facz;
		}
		else if(dlg.m_iRefAxis==2)	//��Z��Ϊ��׼
		{
			cur_pos.x += fCurVerHigh/facx;//Wjh X��Y������չ��ת��
			cur_pos.y += fCurVerHigh/facy;//Wjh X��Y������չ��ת��
			cur_pos.z += fCurVerHigh;
		}
		else						//�Գ���Ϊ��׼
		{
			cur_pos.x += fCurVerHigh*facx;
			cur_pos.y += fCurVerHigh*facy;//Wjh X��Y������չ��ת��
			cur_pos.z += fCurVerHigh*facz;
		}
		//�����½ڵ�
		SORTEDNODE_ITEM item;
		item.pNode=console.AppendNode();
		item.pNode->CopyProperty(pMainLinePart->pStart);//�ӷֶ�ԭʼ�ڵ㿽����������
		item.pNode->layer(2) = pMainLinePart->layer(2);	//�ڵ�ͼ��λ����Ǹ�һ��
		item.pNode->cfgword = pMainLinePart->cfgword;	//�ڵ�Ӧ��ԭʼ�˼�����ĺ�һ�¶�����ʼ�˽ڵ� wjh-2016.6.21
		item.pNode->SetPosition(cur_pos);
		item.pNode->hFatherPart=pMainLinePart->handle;
		NewNode(item.pNode);
		if(dlg.m_iRefAxis<3)
		{
			item.pNode->m_cPosCalType = dlg.m_iRefAxis+1;//X/Y/Z����ֵ�����
			item.pNode->arrRelationNode[0] = pMainLinePart->pStart->handle;
			item.pNode->arrRelationNode[1] = pMainLinePart->pEnd->handle;
		}
		else
		{
			item.pNode->m_cPosCalType = 7;	//ƫ�Ƶ�
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
			{	//ʹ�ò�������ķ�����ѡ��˼��ϵĽڵ㰴����ʼ�˵ľ����������(����)
				for(SORTEDNODE_ITEM *pItem=node_list.GetFirst();pItem;pItem=node_list.GetNext())
				{
					if(pItem->distToStart>item.distToStart)
					{
						node_list.insert(item);
						break;
					}
					else if(pItem->distToStart==item.distToStart)
						break;	//�ظ��ڵ���ȥ����
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
		mir_node_list.Empty();	//��նԳƽڵ��б�
		for(SORTEDNODE_ITEM *pItem=node_list.GetFirst();pItem;pItem=node_list.GetNext())
		{
			SORTEDNODE_ITEM item;
			item.pNode=console.AppendNode();
			item.pNode->CopyProperty(pLinePart->pStart);//�ӷֶ�ԭʼ�ڵ㿽����������
			item.pNode->layer(2) = pLinePart->layer(2);	//�ڵ�ͼ��λ����Ǹ�һ��
			item.pNode->cfgword = pLinePart->cfgword;
			item.pNode->SetPosition(GetMirPos(pItem->pNode->Position(false),mir_msg));
			item.pNode->hFatherPart=pLinePart->handle;
			NewNode(item.pNode);
			//��ӶԳƹ�����Ϣ
			RELATIVE_OBJECT *pMirObj=pItem->pNode->relativeObjs.Add(item.pNode->handle);
			pMirObj->mirInfo=mir_msg;
			if(dlg.m_iRefAxis<3)
			{
				item.pNode->m_cPosCalType = dlg.m_iRefAxis+1;//X/Y/Z����ֵ�����
				item.pNode->arrRelationNode[0] = pLinePart->pStart->handle;
				item.pNode->arrRelationNode[1] = pLinePart->pEnd->handle;
			}
			else
			{
				item.pNode->m_cPosCalType = 7;	//ƫ�Ƶ�
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
	//����ٽ���ѡ�˼��ֶ�,ǰ��Ҫ�õ�
	partset[0].Empty();
	FinishBreakLinePart(pMainLinePart,&node_list,partset[0]);
	//��ӶԳƽڵ��໥֮��Ĺ�����Ϣ
	for(SORTEDNODE_ITEM *pItem=node_list.GetFirst();pItem;pItem=node_list.GetNext())
	{	
		if(pItem->pNode==NULL)
			continue;
		SetRelativeObj(pItem->pNode);
	}
	//��ӶԳƸ˼��໥֮��Ĺ�����Ϣ
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
	pCmdLine->FillCmdLine("����:","");
#endif
	Command("DivideMainLinePart");
	return TRUE;
}
